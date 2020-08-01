/////////////////////////////
//	psu_comm.c
/////////////////////////////
#define PSU_COMMM_GLOBALS
#include "includes.h"
#include <math.h>
/////////////////////////////


/*
---------------------------------------------------
PSU Protocol ID @ 5G
---------------------------------------------------
*/
#ifdef __MAKER_ANTS__    // ANTS-------------------
#define _PSUID_PSU_ 0xA0 // ANTS 0xA0~0xBF : Use 0xA0 Only @ 5G
#else					 // HFR -------------------
#define _PSUID_PSU_ 0xC0 // HFR  0xC0~0xDF : Use 0xC0 Only @ 5G
#endif
//----------------------------------------------------

#define PSU_LINK_FAIL_CO (10)
#define PSU_Timout_ms    (200)
#define PSU_MSG_ID_Idx   (5) // 0~3 is reserved for AMP


/////////////////////////////
// Local Variables
/////////////////////////////
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
static PAU_PROTOCOL_FRAME psu_txf; // PAU and PSU have same protocol
static PAU_PROTOCOL_FRAME psu_rxf; // PAU and PSU have same protocol

static pthread_mutex_t	Psu_COMM_Evt;
static pthread_mutex_t	Psu_Ctrl_Sem;
#endif


/////////////////////////////
// Local Functions
/////////////////////////////




//----------------------------------------------------------------------
// 휴지기간에 mode_on, 그 밖에 mode_off 
void Green_Mode_Ctrl( INT8U mode_on ) // 5G
{
	if(mode_on == ON) // ON is 0x00
		m_St.Green_Mode_Req = 0;
	else              // OFF is 0x01
		m_St.Green_Mode_Req = 1; 
}
//------------------------------------------------------------------
void PSU_Init_Set(void)
{	
	// 초기화 위해.
	PSU_Init_End = (-1);
	Init_Psu_Down_St();
	Psu_Inv_Read_Req();
}
//------------------------------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
static INT8U psu_rst_standby = 0;
#endif


INT16U PSU_DownLoad_PassData( INT8U type, PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
	INT16U l, len = 0;
	INT8U Mod_SUBID[3];
	INT8U pau_type;
	INT8U gui_type;
	INT8U totPkt = 1, curPkt = 1;
	
	// Module SUBID Check
	Mod_SUBID[0] = rxf->data[0];
	Mod_SUBID[1] = rxf->data[1];
	Mod_SUBID[2] = rxf->data[2];

	pthread_mutex_lock(&Psu_COMM_Evt);
	
	// cpy len and data
	psu_txf.len = rxf->len - 3; // From index 3
	l = psu_txf.len;

	memcpy( psu_txf.data, &rxf->data[3], l );

	// 다운로드 명령(타입)을 PAU가 사용하는 타입으로 변경.
	switch( type )
	{	
		case 0xE8: 
			pau_type = PSU_CMD_DN_NOTI_FW;
			
			if(psu_txf.data[3] == 0) psu_rst_standby = 1; // reset-standby after download
			else                     psu_rst_standby = 0; // auto reset after download

			D_Printf( DEBUG_PSU, "\r\n [RCU] PSU Reset_Standby(%d)", psu_rst_standby);			
			break;			
			
		case 0xE0: 
			pau_type = PSU_CMD_DN_NOTI;  
			psu_rst_standby = 0;  
			break;
			
		case 0xE1: pau_type = PSU_CMD_DN_NOTI_CHK; break;
		case 0xE2: pau_type = PSU_CMD_DN_DATA; break;
		case 0xE3: pau_type = PSU_CMD_DN_END; break;
	}	
	
	psu_txf.ScrAddr = 0x00; // RCU Addr
	psu_txf.DesAddr = _PSUID_PSU_;
	
	len = Make_Frame_PAU(pau_type, &psu_txf, l, PSU_MSG_ID_Idx);

	if(Send_DN_Data(COMM_UART___PSU, (INT8U*)&psu_txf, len, (INT8U*)&psu_rxf, PSU_Timout_ms))
	{
		if((psu_rxf.Type == pau_type) && (psu_rxf.ScrAddr == _PSUID_PSU_))
		{			
			//D_Printf( DEBUG_PSU, "\r\n [RCU] Rx PSU Dn Cmd 0x%02X.", pau_type); 		

			switch( pau_type )
			{	
				case PSU_CMD_DN_NOTI_FW:  gui_type = 0xE8; break;
				case PSU_CMD_DN_NOTI:	  gui_type = 0xE0; break;
				case PSU_CMD_DN_NOTI_CHK: gui_type = 0xE1; break;
				case PSU_CMD_DN_DATA:	  gui_type = 0xE2; break;				
				case PSU_CMD_DN_END:	  gui_type = 0xE3; break;
			}
			
			txf->Type = gui_type;
			txf->len = psu_rxf.len; 	
			
			if( pau_type == PSU_CMD_DN_END) m_Env.PSU_Dn_St = 2;
				
			memcpy( &txf->data[3], psu_rxf.data, txf->len);
			
			// Module SUBID 3byte 추가.
			txf->data[0] = Mod_SUBID[0];
			txf->data[1] = Mod_SUBID[1];
			txf->data[2] = Mod_SUBID[2];			
			txf->len += 3;

			len = Make_Frame( txf->Type, rxf->Rcode, txf, txf->len, rxf->seqNo16, totPkt, curPkt );
			
			if (gui_type == 0xE3)
			{
				if(psu_rst_standby)
				{
					m_Env.is_down_load = 0;
					psu_rst_standby = 0; // init
				}
				else
				{			
					PSU_Init_Set();
				}				
			}
			//-----------------------------------------------------	
			pthread_mutex_unlock(&Psu_COMM_Evt);
			return len;
		}
	}
	pthread_mutex_unlock(&Psu_COMM_Evt);
	CheckUART_Error(COMM_UART___PSU);
#endif
	return 0;
}
//-----------------------------------------------------------------
// Get Sub-Pwr-switch status
INT8U Get_PSU_AMP_SW_St(INT8U path4)
{
	INT8U bit;
	
	switch(path4)
	{
		case AMPU_4_1: bit = 7; break; // 5G PRU AMP-#1
		case AMPU_4_2: bit = 6; break; // 5G PRU AMP-#2
		case AMPU_4_3: bit = 5; break; // 5G PRU AMP-#3
		case AMPU_4_4: bit = 4; break; // 5G PRU AMP-#4

		default: return OFF;
	}
	
	if(m_PSU.PAU_Switch_St & cbits[bit])
		return OFF;
	else 
		return ON;
}
//-----------------------------------------------------------------
INT8U Is_Ext_FAN_Alm(void)
{
	INT8U alm = 0;
	
	alm |= m_PSU.Ext_FAN_Alm[0];
	alm |= m_PSU.Ext_FAN_Alm[1];
	alm |= m_PSU.Ext_FAN_Alm[2];

	return alm;
}
//-----------------------------------------------------------------
void RecvPSU_Status( PSU_ST_STR* psu ) 
{
	INT32U uVal32, i;
	INT8U status_tot;

	m_PSU.PAU_Switch_St = psu->PSU_Arm[0];

	//----------------------------------------------------------------------------------------
	m_PSU.Ext_FAN_Alm[0] = (psu->PSU_Arm[1]&C_BIT(7))?1:0; // Ext FAN1
	m_PSU.Ext_FAN_Alm[1] = (psu->PSU_Arm[1]&C_BIT(6))?1:0; // Ext FAN2
	m_PSU.Ext_FAN_Alm[2] = (psu->PSU_Arm[1]&C_BIT(5))?1:0; // Ext FAN3
	m_PSU.PSU_FAN_St     = (psu->PSU_Arm[1]&C_BIT(3))?1:0; // PSU FAN

	status_tot = 0; // init
	if(m_PSU.Ext_FAN_Alm[0]) status_tot |= cbits[7];
	if(m_PSU.Ext_FAN_Alm[1]) status_tot |= cbits[6];
	if(m_PSU.Ext_FAN_Alm[2]) status_tot |= cbits[5];	
	m_PSU.PSU_Ext_FAN_Sum = status_tot;
	
	/*
	AID 0xB630 : FAN Status(함체)

	PRU : (1~5: Reserved)
	6: 외장 FAN2, 
	7: 외장 FAN1, 
	8: 외장 FAN0 
	*/
	
	//----------------------------------------------------------------------------------------
	m_PSU.AC_Alm_Recti   = (psu->PSU_Arm[2] & C_BIT(7) )?1:0; // AC Alarm (Under Voltage)
	m_PSU.AC_Alm_OV      = (psu->PSU_Arm[2] & C_BIT(6) )?1:0; // AC Alarm (Over Voltage)
	
	// DC Main
	m_PSU.DC_UV_Recti[0] = (psu->PSU_Arm[2]&C_BIT(5))?1:0; // DC Alarm(50.2v) (Under Voltage)
	m_PSU.DC_OV_Recti[0] = (psu->PSU_Arm[2]&C_BIT(4))?1:0; // DC Alarm(50.2v) (Over Voltage)
	m_PSU.DC_OC_Recti[0] = (psu->PSU_Arm[2]&C_BIT(3))?1:0; // DC Alarm(50.2v) (Over Current)
	
	// DC 5.8V
	m_PSU.DC_UV_Recti[1] = (psu->PSU_Arm[2]&C_BIT(2))? 1:0; // DC Alarm(5.8v) (Under Voltage)
	m_PSU.DC_OV_Recti[1] = (psu->PSU_Arm[2]&C_BIT(1))? 1:0; // DC Alarm(5.8v) (Over Voltage)
	m_PSU.DC_OC_Recti[1] = (psu->PSU_Arm[2]&C_BIT(0))? 1:0; // DC Alarm(5.8v) (Over Current)
	//-----------------------
	status_tot = 0; // init
	if(m_PSU.AC_Alm_Recti)   status_tot |= cbits[7];
	if(m_PSU.AC_Alm_OV)      status_tot |= cbits[6];
	if(m_PSU.DC_UV_Recti[0]) status_tot |= cbits[5];
	if(m_PSU.DC_OV_Recti[0]) status_tot |= cbits[4];
	if(m_PSU.DC_OC_Recti[0]) status_tot |= cbits[3];
	if(m_PSU.DC_UV_Recti[1]) status_tot |= cbits[2];
	if(m_PSU.DC_OV_Recti[1]) status_tot |= cbits[1];
	if(m_PSU.DC_OC_Recti[1]) status_tot |= cbits[0];
	m_PSU.PSU_St_AC_DC = status_tot;
	//----------------------------------------------------------------------------------------

	// DC 24V : maybe 'FAN'
	m_PSU.DC_UV_Recti[2] = (psu->PSU_Arm[3]&C_BIT(7))? 1:0; // DC Alarm(24v) (Under Voltage)
	m_PSU.DC_OV_Recti[2] = (psu->PSU_Arm[3]&C_BIT(6))? 1:0; // DC Alarm(24v) (Over Voltage)
	m_PSU.DC_OC_Recti[2] = (psu->PSU_Arm[3]&C_BIT(5))? 1:0; // DC Alarm(24v) (Over Current)
	
	m_PSU.PSU_St_Temp_DC = psu->PSU_Arm[3] & 0xF8;
	//----------------------------------------------------------------------------------------

	m_PSU.Battery_Low       = (psu->PSU_Arm[4]&C_BIT(6))?OFF:ON; // Battery Low
	m_PSU.Batt_Charge_OnOff = (psu->PSU_Arm[4]&C_BIT(5))?OFF:ON; // Battery 충전 On/Off상태
	m_PSU.Cur_Green_Mode    = (psu->PSU_Arm[4]&C_BIT(4))?OFF:ON; // GreenMode State
	
	/*	
		PSU_Arm[4]
		----------------------------
		bit 7: Ripple(5.6v)
		bit 6: Battery Low
		bit 5: Battery 충전 On/Off 상태
		bit 4: GreenMode State
		bit 3: Battery Status1
		bit 2: Battery Status2
		bit 1: Battery Status3
		bit 0: Battery 실장 정보 : 0(insert), 1(not-insert)
	*/
	m_PSU.PSU_St_Etc = psu->PSU_Arm[4];
	
	
	//------------------------------------
	// 2019.04.06 : if batt. not insert, mask-alarm of battery-low(bit6)
	if(m_PSU.PSU_St_Etc & C_BIT(0))// if batt. not-insert
	{
		m_PSU.PSU_St_Etc &= N_BIT(6);
	}
	//------------------------------------
	
	//----------------------------------------------------------------------------------------
		
	m_PSU.AC_In_Volt_x10 = AHex2Dig_INT16( psu->AC_In_Volt );

	m_PSU.DC_Out_Volt_x10[0] = AHex2Dig_INT16( psu->DC_Out_Volt_50 );  // 출력전압(50.2v)
	m_PSU.DC_Out_Volt_x10[1] = AHex2Dig_INT16( psu->DC_Out_Volt_5_8 ); // 출력전압(5.8v)
	m_PSU.DC_Out_Volt_x10[2] = AHex2Dig_INT16( psu->DC_Out_Volt_24 );  // 출력전압(24v)
	m_PSU.Batt_Volt_x10      = AHex2Dig_INT16( psu->Batt_Volt );	   // Batt 출력전압
	m_PSU.Load_Current_x10[0]= AHex2Dig_INT16( psu->Load_Current_A );  // 출력전류(50.2v)
	
	m_St.FAN_On_Temp[INT_FAN] = AHex2Dig( psu->FAN_On_Temp);
	m_St.FAN_OffTemp[INT_FAN] = AHex2Dig( psu->FAN_OffTemp);
	m_St.FAN_OnOff[INT_FAN] = psu->FAN_OnOff - '0';
	m_St.FAN_Auto[INT_FAN]  = psu->FAN_Auto - '0';

	m_PSU.Compatible = psu->Compatibility; // 2020.02.14

	memcpy(m_PSU.IRO_remote_type, psu->iro_remote_type, 2); // 2020.07.10
	memcpy(m_PSU.PRU_remote_type, psu->pru_remote_type, 2); // 2020.07.10
		
	m_PSU.Temp_x10 = (INT16S)AHex2Dig_INT16(psu->PSU_Temp);	// 4  PSU 온도
	m_PSU.Fan_Speed = AHex2Dig_INT16( psu->FAN_RPM )/10;	// 4  FAN RPM

	uVal32 = AHex2Dig_INT16( &psu->Watt_Tot_Accu[0] );
	uVal32 <<= 16;
	uVal32 |= AHex2Dig_INT16( &psu->Watt_Tot_Accu[4] );

	m_PSU.Watt_Tot_Accu = uVal32;

	m_PSU.Watt_real_time = AHex2Dig_INT16( psu->Watt_real_time );	// 4  실시간 전력량
	m_PSU.Watt_hour_aver = AHex2Dig_INT16( psu->Watt_hour_aver );	// 4  1시간평균전력량
	m_PSU.Watt_day_aver  = AHex2Dig_INT16( psu->Watt_day_aver );	// 4  1일평균전력량
	m_PSU.Watt_week_aver = AHex2Dig_INT16( psu->Watt_week_aver );	// 4  1주일평균전력량

	for( i=0; i<12; i++ )
		m_PSU.Watt_month[i] = AHex2Dig_INT16( psu->Watt_month[i] );	// 4  1월~12월 전력량

	m_St.DL_State[DN_PSU][0] = psu->DL_St[0];
	m_St.DL_State[DN_PSU][1] = psu->DL_St[1];
	
	m_PSU.DL_St[0] = psu->DL_St[0];
	m_PSU.DL_St[1] = psu->DL_St[1];
	
	m_PSU.Hi_Temp        = (psu->PSU_Arm[3] & C_BIT(4) )?1:0;
	m_PSU.Hi_Temp_SD     = (psu->PSU_Arm[3] & C_BIT(3) )?1:0;
	m_PSU.Ripple_Alm_5v6 = (psu->PSU_Arm[4] & C_BIT(7) )?1:0;	
	
	// 12 시간정보: PSU 프로토콜에 있지만 AID 는 미사용	
} 


INT8U IS__PSU_Compatible(void) // 2020.02.14
{
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
	if(m_PSU.Compatible == 0x5A) 
		return 1;
	else
		return 0;
#else
	return 0;
#endif
}

/*
-----------------------------------------------------------
PSU/giRO 구분 
-----------------------------------------------------------
*/
#include <ctype.h>

INT8U IS__PSU_giRO(void) // 2020.02.14
{
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
	char maker_mode[10];
	int i;
	
	memset(maker_mode, 0, 10);	 
	memcpy(maker_mode, PSU_Info.Maker_Mode, 4);
	
	if(strlen(maker_mode) == 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[PSU Maker Mode] in null");
		return 0;
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[PSU Maker Mode] %s", maker_mode);

		for(i = 0; i < 4; i++)
			maker_mode[i] = tolower(maker_mode[i]);	
		
		if(memcmp(maker_mode, "giro", 4)==0)
		{
			return 1;
		}
		else
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[IS__PSU_giRO] not giRO");
			return 0;
		}
	}
#else	
	return 0;
#endif	
}







#ifdef __TYPE__PRU__ // 5G-PRU-Only 
INT8U Get_Repeater_Type(void) // 2020.07.10
{
	static INT8U init = 0;

	if(init == 0)
	{
		init = 1;
		
		switch(m_Eep.PRU_TYPE)
		{		
			case REPEATER_TYPE_5G_PRU:
			case REPEATER_TYPE_PRU_gIRO:
			case REPEATER_TYPE_PRU_gIRO_T:
				break;
				
			default:
				m_Eep.PRU_TYPE = REPEATER_TYPE_5G_PRU;
				m_Env.is_eep_write = EEP_WRITE_CODE;
				break;
		}
	}
	//----------------------------------------	
	return m_Eep.PRU_TYPE;
}

#include <ctype.h>

void Update_Repeater_Type(void) // 2020.07.10
{
	char maker_mode[10];
	INT8U maker_size = 4;
	int i;
	INT8U type = 0; //  0 : default 5G_PRU
	INT8U remot_type[2];
	
	
	memset(maker_mode, 0, maker_size+1);
	memcpy(maker_mode, PSU_Info.Maker_Mode, maker_size); // psu.inventory.maker.mode
	memcpy(remot_type, m_PSU.IRO_remote_type, 2);

	type = 0; // default 0

	////print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Update_Repeater_Type] %s, %d", maker_mode, strlen(maker_mode));
	
	if(strlen(maker_mode) > 0)
	{		
		for(i = 0; i < maker_size; i++) maker_mode[i] = tolower(maker_mode[i]);	

		////print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[maker_mode] %s, %#x %#x----------", maker_mode, remot_type[0], remot_type[1]);
		
		if((memcmp(maker_mode, "giro", maker_size)==0)||(memcmp(maker_mode, "gIRO", maker_size)==0)) // giRO or gIRO
		{			
			     if((remot_type[0] == 0x30)&&(remot_type[1] == 0x43)) type = 'T'; // "0C" giRO_T
			else if((remot_type[0] == 0x30)&&(remot_type[1] == 0x37)) type = 'g'; // "07" giRO
			else type = 'g'; // iRO

			////print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[maker_mode] %c", type);
		}
		else
		{
			////print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[maker_mode] not");
		}
	}	
	//----------------------------------------
	INT8U type_pru;		
	
	switch(type)
	{
		case 'g': type_pru = REPEATER_TYPE_PRU_gIRO;   break;
		case 'T': type_pru = REPEATER_TYPE_PRU_gIRO_T; break;	
		default : type_pru = REPEATER_TYPE_5G_PRU;     break;
	}	
	//----------------------------------------	
	static INT8U init = 0;
	static INT8U type_prev;
	static INT8U same_cnt;

	////print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[type_pru] %#x, %#x prev", type_pru, type_prev);
	
	if(init == 0)
	{
		init = 1;
		type_prev = m_Eep.PRU_TYPE;
		same_cnt = 0;
	}
	
	if(type_prev == type_pru)
	{
		same_cnt++;
		if(same_cnt >= 5)
		{
			same_cnt = 0;
			if(type_pru != m_Eep.PRU_TYPE)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[PRU Type] changed, %#x -> %#x", m_Eep.PRU_TYPE, type_pru);
				m_Eep.PRU_TYPE = type_pru;
				m_Env.is_eep_write = EEP_WRITE_CODE;				
			}
		}
	}
	else
	{
		same_cnt = 0;
		type_prev = type_pru;
	}
	//----------------------------------------
}
#endif	



/*
//	--------------------------------------------------------
//	before 2020.04.01
//	----------------------------
//	     if(IS__PSU_giRO()) 	  return 0; // "gps";
//	else if(IS__PSU_Compatible()) return 0; // "gps";
//	else		                  return 1; // "psu";
//	--------------------------------------------------------	
*/

#define PSU_ZIP_FILES_DIR	"/tmp/zip"
#define ZIP_FILE_PARSE_PSU	"/tmp/zip/parse_psu.txt"

// module 2원화('__USE_MOD_DUALITY__') 에서는 호출 안됨.

INT8U IS___file_ext___psu(void) 
{
	INT8U is_file_psu; // 1 if *.psu, else 0

	if(IS__PSU_giRO()) // PSU의 제조사 모드가 gIRO
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[PSU Down File] maker_mode gIRO, use '.GPS'"); 
		
		is_file_psu = 0; // "gps";
	}
	/*
		Compatible 0x5A이라도 ".PSU" 파일(파일이 없으면 ".GPS" 파일)
	*/
	else if(IS__PSU_Compatible())
	{	
		system_agent_sync( "ls %s/*.psu > %s\n", PSU_ZIP_FILES_DIR, ZIP_FILE_PARSE_PSU);
	
		if(_is_file_size(ZIP_FILE_PARSE_PSU) > 4) // if exist (4 is ".psu")
		{		
			system_agent_sync( "cat %s\n", ZIP_FILE_PARSE_PSU);
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[PSU Down File] Compatible 0x5A, use '.PSU'"); 
			
			is_file_psu = 1; // "psu";			
		}
		else
		{	
			system_agent_sync( "ls %s/*.PSU > %s\n", PSU_ZIP_FILES_DIR, ZIP_FILE_PARSE_PSU);
	
			if(_is_file_size(ZIP_FILE_PARSE_PSU) > 4) // if exist (4 is ".psu")
			{		
				system_agent_sync( "cat %s\n", ZIP_FILE_PARSE_PSU);
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[PSU Down File] Compatible 0x5A, use '.PSU'"); 
				
				is_file_psu = 1; // "psu";			
			}
			else
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[PSU Down File] Compatible 0x5A, but use '.GPS'"); 
			
				is_file_psu = 0; // "gps";
			}
		}
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[PSU Down File] not compatible nor gIRO, use '.PSU'"); 
		
		is_file_psu = 1; // "psu";
	}
	return is_file_psu;
}








//-----------------------------------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
void RecvPsu_INFO(INVENTORY_STR* inv, INVENTORY_2_STR* inv2)
{	
	/*
		// not used -------
		// inv->Ver, inv->sn, inv->date
	*/
	
	memcpy(m_PSU.PSU_I_Ver, inv2->Ver2, 3);
	Set_Ver_Info_PSU(inv->maker, m_PSU.PSU_I_Ver);

	memcpy(&PSU_Info, (INT8U*)inv2, sizeof(INVENTORY_2_STR));	

	RecvModuleInventory( INV_IDX_R_PSU, inv );
} 
#endif
//-----------------------------------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
void PSU_Data_Analysis( PAU_PROTOCOL_FRAME* rxf )
{
	switch( rxf->Type )
	{	
		//---------------
		case PSU_CMD_ST:
		case PSU_CMD_Green_Mode_Ctrl:
		case PSU_CMD_FAN_Speed_Ctrl:
		case PSU_CMD_PSU_FAN_Ctrl:
		case PSU_CMD_Pwr_Reset_Ctrl:
		case PSU_CMD_BattCharge_Ctrl:
			if( rxf->len >= sizeof(PSU_ST_STR) )
			{
				RecvPSU_Status((PSU_ST_STR*)rxf->data);				
				Update_Repeater_Type();
				
				/*
				2020.07.08 : add statistics
				*/
				Update_Stat( &m_Stat.PSU_Temp_x10, m_PSU.Temp_x10 );
				Update_Stat( &m_Stat.PSU_Main_DC_x10, m_PSU.DC_Out_Volt_x10[0] );
				Update_Stat( &m_Stat.PSU_Sub__DC_x10, m_PSU.DC_Out_Volt_x10[1] );
			}
			break;

		 case PSU_CMD_PSU_Reset_Ctrl:
 			break;

		//---------------
		case PSU_CMD_INFO:	
		case PSU_CMD_INFO_Ctrl:
			RecvPsu_INFO((INVENTORY_STR*)rxf->data, (INVENTORY_2_STR*)rxf->data);
			break;

		case PSU_CMD_FW_APPLY:
			m_St.PSU_FW_NACK = rxf->data[0];
			set_psu_DN_confirm_END(_SET_);
			break;
	}
}
#endif
//-----------------------------------------------------------------------

INT8U PSU_Send_Data( INT8U type, INT8U* data, INT8U l )
{
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
	INT16U len;

	pthread_mutex_lock(&Psu_COMM_Evt);

	// RCU
	psu_txf.ScrAddr = 0x00;
	psu_txf.DesAddr = _PSUID_PSU_;
	psu_txf.Type = type;

	if(l) memcpy(psu_txf.data, data, l);
	len = Make_Frame_PAU(type, &psu_txf, l, PSU_MSG_ID_Idx);

	if(Send_DN_Data(COMM_UART___PSU, (INT8U*)&psu_txf, len, (INT8U*)&psu_rxf, PSU_Timout_ms)) 
	{
		if( (psu_rxf.Type == type) && (psu_rxf.ScrAddr == _PSUID_PSU_) )
		{			
			PSU_Data_Analysis( &psu_rxf );

			//sprintf( dbgstr, "PSU RX (path %d)", path4);
			//Debug_PutString((INT8U*)dbgstr, 1);
			pthread_mutex_unlock(&Psu_COMM_Evt);

			return 1;	
		}
	}	
	
	//D_Printf( DEBUG_SYSTEM, "[!] psu RX Fail [0x%02X : msg idx 0x%02X]\r\n", type, msg_id[9] );
	//printf("[!] psu RX Fail [0x%02X : msg idx 0x%02X]\r\n", type, msg_id[9] );
	
	pthread_mutex_unlock(&Psu_COMM_Evt);
	CheckUART_Error( COMM_UART___PSU ); 
#endif
	return 0;	
}
//-----------------------------------------------------------------------
static INT8U info_ctrl_psu[50];
INT8U sendPsu_INFO_Ctrl( INT8U* pCtrlVal, INVENTORY_STR* inv )
{	
	INT8U ret = 0; // init 0	
	INT16U len;	
	
	info_ctrl_psu[0] = pCtrlVal[0];
	info_ctrl_psu[1] = pCtrlVal[1];
	memcpy(&info_ctrl_psu[2], (INT8U*)inv, sizeof(INVENTORY_STR));
	len = sizeof(INVENTORY_STR)+2;	

	ret = PSU_Send_Data( PSU_CMD_INFO_Ctrl, info_ctrl_psu, len );

	return ret;	
}
//-----------------------------------------------------------------------




#define PSU_REP_TYPE 0x01 // 2019.03.20.
INT8U PSU_Polling( void ) // 5G
{
	INT8U ret = 0; // init 0
	INT8U data_ascii[14];
	INT8U PSU_Rep_Type;

	PSU_Link_Fail_Co++;

	if(PSU_Link_Fail_Co >= PSU_LINK_FAIL_CO)
	{
		PSU_Link_Fail_Co = PSU_LINK_FAIL_CO;
		PSU_Link_Fail = 1;		
		/*
		// @ 5G
		// Use below if PSU-Link-Fail is used... later
		// Set_RU_Alarm with _Alm_Link_PSU_
		*/
	}

	date_to_a( data_ascii, m_St.Ru_Sys_Date, 6 );
	/*
	//D_Printf (DEBUG_SYSTEM, "[RCU] Ru_Sys_Date (%d %d %d %d %d %d)", 
	//	m_St.Ru_Sys_Date[0],
	//	m_St.Ru_Sys_Date[1],
	//	m_St.Ru_Sys_Date[2],
	//	m_St.Ru_Sys_Date[3],
	//	m_St.Ru_Sys_Date[4],
	//	m_St.Ru_Sys_Date[5]);
	*/
	
	// Type
	PSU_Rep_Type = PSU_REP_TYPE;	
	data_ascii[12] = '0' + ((PSU_Rep_Type & 0xF0)>>4);
	data_ascii[13] = '0' + (PSU_Rep_Type & 0x0F);
	
	ret = PSU_Send_Data( PSU_CMD_ST, data_ascii, 14 );

	if( ret==1 )
	{
		PSU_Link_Fail_Co=0;		
		PSU_Link_Fail = 0;
		/*
		// @ 5G
		// Use below if PSU-Link-Fail is used... later
		// Set_RU_Alarm with _Alm_Link_PSU_ Clear
		*/
	}
	m_St.PSU_Link_St = PSU_Link_Fail;
	return ret;
}

INT8U PSU_5G_Green_Mode_Ctrl( INT8U en )
{
	if( en == ON ) en = '0';
	else en = '1';

	D_Printf (DEBUG_SYSTEM, "[RCU] PSU Green Ctrl (%c)", en );
	
	return PSU_Send_Data( PSU_CMD_Green_Mode_Ctrl, &en, 1 );
}

INT8U PSU_FAN_Speed_Ctrl( INT8U Fan_Speed )
{
	INT8U vdata[4];
	
	Dig2AHex_INT16U( vdata, Fan_Speed*10 );
	return PSU_Send_Data( PSU_CMD_FAN_Speed_Ctrl, vdata, 4 );
}

INT8U PSU_PSU_FAN_Ctrl( INT8U FAN_Auto, INT8U FAN_OnOff, INT8S FAN_On_Temp, INT8S FAN_OffTemp )
{
	INT8U vdata[6];
	
	Dig2AHex( &vdata[0], FAN_On_Temp );
	Dig2AHex( &vdata[2], FAN_OffTemp );

	if( FAN_OnOff == ON ) vdata[4] = '0';
	else                  vdata[4] = '1';

	if( FAN_Auto == ON )  vdata[5] = '0';
	else                  vdata[5] = '1';

	return PSU_Send_Data( PSU_CMD_PSU_FAN_Ctrl, vdata, 6 );
}


INT8U PSU_Reset_Ctrl( void ) // PSU-cpu Reset (cpu만 reset, 전원은 그대로.) 
{
	return PSU_Send_Data( PSU_CMD_PSU_Reset_Ctrl, NULL, 0 );
}

INT8U PSU_Pwr_Reset_Ctrl( void ) // 전력량 초기화
{
	return PSU_Send_Data( PSU_CMD_Pwr_Reset_Ctrl, NULL, 0 );
}

// PSU Reset 되면, batt default 충전 on 제어됨.
// CPU Reset 시, on 제어 필요.
INT8U PSU_BattCharge_Ctrl( INT8U en )
{
	INT8U onoff;

	if( en == ON ) onoff = '0';
	else onoff = '1';

	m_Env.Batt_Charge_OnOff = en;

	// 2019.04.06 : when AC-Fail, 
	// do not send 'PSU_CMD_BattCharge_Ctrl'
	if(m_St.AlmSt.A_0.AC == 0)
		return PSU_Send_Data( PSU_CMD_BattCharge_Ctrl, &onoff, 1);
	else
		return 0;
}

INT8U PSU_Get_Info( void ) // read inventory
{
	return PSU_Send_Data( PSU_CMD_INFO, NULL, 0 );
}

void Chk_PSU_Green_St(void)
{	
#if 0 // 5G : not use Green mode, but it will be used later, maybe...
	INT8U mode_req;
	
	if(m_Eep.Green_Mode_On == OFF) // by User-Off
		mode_req = OFF;
	else                           // by algorithm
		mode_req = m_St.Green_Mode_Req & 0x01; 
	
	if(m_PSU.Cur_Green_Mode != mode_req)
		PSU_5G_Green_Mode_Ctrl( mode_req );
#endif		
}
//----------------------------------------------------------
void Init_Psu_Down_St(void) // 5G
{
	m_Env.PSU_Dn_St = 0;
}
//----------------------------------------------------------
void PSU_FAN_Ctrl_Chk(void) // PSU Internal FAN
{
	INT8U PSU_Int_Fan_Req = 0;
	INT8U FAN_Auto;
	INT8U FAN_OnOff;
	INT8S FAN_On_Temp;
	INT8S FAN_OffTemp;

	// init
	FAN_Auto  = m_St.FAN_Auto[INT_FAN];
	FAN_OnOff = m_St.FAN_OnOff[INT_FAN];
	
	FAN_On_Temp = m_St.FAN_On_Temp[INT_FAN];
	FAN_OffTemp = m_St.FAN_OffTemp[INT_FAN];
	//------------------------------	
	if( f_Req.FAN_On_Temp[INT_FAN] )
	{
		if(m_St.FAN_On_Temp[INT_FAN] != m_Req.FAN_On_Temp[INT_FAN])
		{
			PSU_Int_Fan_Req++;
			FAN_On_Temp = m_Req.FAN_On_Temp[INT_FAN];

			D_Printf( DEBUG_SYSTEM, "PSU Int-FAN : On Temp %d.\n", FAN_On_Temp ); //
		}
		else
		{
			f_Req.FAN_On_Temp[INT_FAN] = 0;			
		}		
	}
	//------------------------------
	if( f_Req.FAN_OffTemp[INT_FAN] )
	{
		if(m_St.FAN_OffTemp[INT_FAN] != m_Req.FAN_OffTemp[INT_FAN])
		{
			PSU_Int_Fan_Req++;
			FAN_OffTemp = m_Req.FAN_OffTemp[INT_FAN];

			D_Printf( DEBUG_SYSTEM, "PSU Int-FAN : Off Temp %d.\n", FAN_OffTemp ); //
		}
		else
		{
			f_Req.FAN_OffTemp[INT_FAN] = 0;			
		}		
	}
	//------------------------------	
	if( f_Req.FAN_Auto[INT_FAN] )
	{
		if(m_St.FAN_Auto[INT_FAN] != m_Req.FAN_Auto[INT_FAN])
		{
			PSU_Int_Fan_Req++;
			FAN_Auto = m_Req.FAN_Auto[INT_FAN];

			D_Printf( DEBUG_SYSTEM, "PSU Int-FAN : Auto %d.\n", FAN_Auto ); //
		}
		else
		{
			f_Req.FAN_Auto[INT_FAN] = 0;			
		}		
	}
	//------------------------------
	if( f_Req.FAN_OnOff[INT_FAN] )
	{
		// 2019.07.17
		if(m_St.FAN_Auto[INT_FAN] == OFF)
		{		
			if(m_St.FAN_OnOff[INT_FAN] != m_Req.FAN_OnOff[INT_FAN])
			{
				PSU_Int_Fan_Req++;
				FAN_OnOff = m_Req.FAN_OnOff[INT_FAN];

				D_Printf( DEBUG_SYSTEM, "PSU Int-FAN : OnOff %d.\n", FAN_OnOff ); //
			}
			else
				f_Req.FAN_OnOff[INT_FAN] = 0;
		}
		else
		{
			FAN_OnOff = m_St.FAN_OnOff[INT_FAN]; // current PSU Status
			f_Req.FAN_OnOff[INT_FAN] = 0;
		}
	}

	if(PSU_Int_Fan_Req)
	{
		D_Printf( DEBUG_SYSTEM, "PSU Int-FAN Ctrl.\r\n" );
		
		PSU_PSU_FAN_Ctrl( FAN_Auto, FAN_OnOff, FAN_On_Temp, FAN_OffTemp );
	}
}





/*-----------------------------------------------------------------------
* PSU Ext PAN : RPM Ctrl Algorithm
                           
	PAU max temp  |	RPM(%)
	--------------+-------
	     <30	  |   0    
	--------------+------- 
	30<= <50	  |   40   
	--------------+-------
	50<= <70	  |   60   
	--------------+-------
	70<= <90	  |   80   
	--------------+-------
	90 < 	      |   100  
	--------------+-------

	1.	Auto 경우 RPM Auto Ctrl
	2.	Manual 경우 : RPM 사용자 마지막 제어값 동작 (Default 60)



설명추가 by YCSO START 190417
					RPM UP					|||						RPM down
-----------------------------------------------------------------------------------------------
		PAU max temp	|		RPM %		|||			PAU max temp	|		RPM %
	85 <= temp			|	80	->	100		|||		75 <= temp			|	100
	70 <= temp < 85 	|	60	->	80		|||		60 <= temp < 75 	|	100	->	80
	60 <= temp < 70		|	40	->	60		|||		50 <= temp < 60 	|	80	->	60
	50 <= temp < 60		|	0	->	40		|||		40 <= temp < 50 	|	60	->	40
 		  temp < 50		|	0				|||			  temp < 30 	|	40 	->	0

&&&&
저 소음 모드일때에는 평상시처럼 동작 하나 최대 RPM에 제한을 두는 것 뿐.
설명추가 by YCSO END 190417

-----------------------------------------------------------------------*/

static OS_TIMER psu_ext_fan_timer;

void Proc_Ext_FAN_PWM_Ctrl( void )
{	
	static INT8U init = 0;
	static INT8U use_20_rpm = 0;
	static INT8U rpm_prev = 0;
	INT16S cur_temp_x10;
	INT8U fan_rpm; // 0 ~ 100
	INT8U cur_fan_rpm = m_PSU.Fan_Speed; // 2019.04.17
	
	//---------------------
	// Ext. FAN : RPM Ctrl
	//---------------------	
	/*
	----------------------------------------------
		시스템 초기 부팅시 외부 FAN을 20%에서 시작 5초 유지.
		이후 현재 온도에 맞는 RPM으로 한 Step(20%)씩 변경하여 설정
		
		(1) Auto 모드 조건
			5초 이후 온도 조건에 맞는 RPM으로 한 Step씩 변경
			- 20% ==> 40 ==> 60% ==> 80% ==> 100%(last)
			- 20% ==> 0%(last)

		(2) Manual 모드
			5초 이후 마지막 사용자 설정값으로 변경
	----------------------------------------------	
	*/
	if(init == 0)
	{
		init = 1;
		use_20_rpm = 1;
		OSTimerReset(&psu_ext_fan_timer);		
	}
	if(use_20_rpm)
	{	
		if(OSTimerGet_msec(&psu_ext_fan_timer) > 5*TICK_CO_SEC)
		{
			OSTimerReset(&psu_ext_fan_timer);
			use_20_rpm = 0;			
		}		
	}
	if(use_20_rpm)	
	{
		fan_rpm = 20;
		m_PSU.desire_Fan_Speed = fan_rpm;
		PSU_Ext_FAN_Onoff_Ctrl( m_St.FAN_OnOff[EXT_FAN] );	
		return;
	}
	// every 1[s]
	if(OSTimerGet_msec(&psu_ext_fan_timer) < TICK_CO_SEC) 
		return;
	OSTimerReset(&psu_ext_fan_timer);	
	//-------------------------------------------------------
		
	if( m_St.FAN_Auto[EXT_FAN] == ON ) // 0xB613
	{
		// 2019.07.29 : max of AMP and DTU 
		cur_temp_x10 = Get_Hightest_AMP_Temp();
		if(cur_temp_x10 < m_St.d_DTU_Temp_x10) 
			cur_temp_x10 = m_St.d_DTU_Temp_x10;

//		D_Printf( DEBUG_SYSTEM, "Max temp %d.\r\n", cur_temp );

			 if( cur_fan_rpm >= 100 )						cur_fan_rpm = 100;
		else if( cur_fan_rpm >= 80	&& cur_fan_rpm < 100 )	cur_fan_rpm = 80;
		else if( cur_fan_rpm >= 60	&& cur_fan_rpm < 80  )	cur_fan_rpm = 60;
		else if( cur_fan_rpm >= 40	&& cur_fan_rpm < 60  )	cur_fan_rpm = 40;
		else												cur_fan_rpm = 0;

		fan_rpm = cur_fan_rpm;
		
		switch( cur_fan_rpm )
		{
			case 100:
				if( cur_temp_x10 < 750 ) fan_rpm = 80;
				break;	

			case 80:
				if( cur_temp_x10 > 850 ) fan_rpm = 100;
				if( cur_temp_x10 < 600 ) fan_rpm = 60;
				break;

			case 60:
				if( cur_temp_x10 > 700 ) fan_rpm = 80;
				if( cur_temp_x10 < 500 ) fan_rpm = 40;
				break;

			case 40:
				if( cur_temp_x10 > 600 ) fan_rpm = 60;
				if( cur_temp_x10 < 300 ) fan_rpm = 0; // 40 -> 30: 2019.08.12
				break;

			case 0:
				if( cur_temp_x10 > 500 ) fan_rpm = 40;
				break;

			default:
				break;
		}

		if(m_St.FAN_Mode_St == FAN_Mode_Low_Noise)
		{
			// limiting when fan rpm of table is over max_rpm
			if( fan_rpm > m_St.FAN_Ext_Max_RPM )
				fan_rpm = m_St.FAN_Ext_Max_RPM;
		}

////// YCSO changed END 190417		
			
		
		if(fan_rpm == 0) m_St.FAN_OnOff[EXT_FAN] = OFF;
		else             m_St.FAN_OnOff[EXT_FAN] = ON; 
	}
	else // manual ctrl ---------------------------------------
	{
		fan_rpm = m_Eep.Ext_FAN_Speed;

		if(fan_rpm == 0) m_St.FAN_OnOff[EXT_FAN] = OFF;
		else             m_St.FAN_OnOff[EXT_FAN] = ON;
	
	/*
	----------------------------------------------
		시스템 초기 부팅시 외부 FAN을 20%에서 시작 5초 유지.
		이후 현재 온도에 맞는 RPM으로 한 Step(20%)씩 변경하여 설정
		
		(1) Auto 모드 조건
			5초 이후 온도 조건에 맞는 RPM으로 한 Step씩 변경
			- 20% ==> 40 ==> 60% ==> 80% ==> 100%(last)
			- 20% ==> 0%(last)

		(2) Manual 모드
			5초 이후 마지막 사용자 설정값으로 변경
	----------------------------------------------	
	*/
		if(cur_fan_rpm < fan_rpm) // +20/step(every 1[s])
		{
			if((cur_fan_rpm + 20) <= fan_rpm)
				fan_rpm = cur_fan_rpm + 20;
		}
		else // -20/step(every 1[s])
		{
			if((cur_fan_rpm - 20) >= fan_rpm)
				fan_rpm = cur_fan_rpm - 20;
		}
		
		if(rpm_prev != fan_rpm)
			print_agent(DEBUG_SYSTEM, VT_GREEN, "[Ext FAN Manual] user %d, ctrl %d", m_Eep.Ext_FAN_Speed, fan_rpm);
			
		rpm_prev = fan_rpm;
	}
	//------------------------------------------	
	m_PSU.desire_Fan_Speed = fan_rpm;
	PSU_Ext_FAN_Onoff_Ctrl( m_St.FAN_OnOff[EXT_FAN] );	
}

// 2019.04.15
INT8U Is_Ext_FAN_Low_Noise_Mode_for_24Hour(void)
{
	INT16U start16, stop16;		
	
	// start time
	start16 = m_St.FAN_Mode_Start_Time[0]; start16 <<= 8;
	start16 += m_St.FAN_Mode_Start_Time[1];
	
	// stop time
	stop16 = m_St.FAN_Mode_Stop__Time[0]; stop16 <<= 8;
	stop16 += m_St.FAN_Mode_Stop__Time[1];

	if(start16 == stop16) return 1; // always low noise mode
	else return 0;
}


INT8U Is_Ext_FAN_Mode_Time(void)
{
	INT16U start16, stop16, cur16;	
	
	//-----------------------------
	if(m_Env.gREMS_Noti_Ready==0)
	{
	#ifdef __USE__RuInit_6_of_6_Cnt__
		if(m_Env.RU_Init_Cnt == 0) // 2020.07.08 : 6/6 단계가 한번이라도 되면, return 하지 않음.
			return 0;
	#else	
		return 0;
	#endif
	}
	//----------------------------- 

	// cur time
	cur16 = m_St.Ru_Sys_Date[3]; cur16 <<= 8;
	cur16 += m_St.Ru_Sys_Date[4];
	
	// start time
	start16 = m_St.FAN_Mode_Start_Time[0]; start16 <<= 8;
	start16 += m_St.FAN_Mode_Start_Time[1];
	
	// stop time
	stop16 = m_St.FAN_Mode_Stop__Time[0]; stop16 <<= 8;
	stop16 += m_St.FAN_Mode_Stop__Time[1];

	/*
	static int cnt = 0;
	cnt++;
	if(cnt > 3)
	{
		cnt = 0;
		D_Printf( DEBUG_SYSTEM, "[PSU_DN] start 0x%04X, stop 0x%04X, cur16 0x%04X \n",
			start16, stop16, cur16);
	}
	*/
	
	if(start16 <= stop16) // start time is lower than stop
	{
		if((cur16 >= start16) && (cur16 < stop16)) return 1;
		else return 0;
	}
	else  // stop time is lower than start
	{
		if((cur16 >= stop16) && (cur16 < start16)) return 0; // stop ~ start 간격은 off
		else return 1;
	}
}

/*
--------------------------------------------------------------------------------------
	FAN Mode
	A.	일반모드
		- RPM Control according to temperature
		
	B.	저소음 모드		
		- when operation time :  FAN(외장) Max RPM(저소음 Mode) 값으로 RPM을 control
		- otherwise : 일반모드와 동일하게 동작
--------------------------------------------------------------------------------------
*/

void Proc_Ext_FAN_Mode(void) // PRU FAN
{
	switch(m_Eep.FAN_Mode_Onoff)
	{
		case FAN_Mode____Normal:
			m_St.FAN_Mode_St = FAN_Mode____Normal;
			break;
			
		case FAN_Mode_Low_Noise:
			if(Is_Ext_FAN_Mode_Time())
				m_St.FAN_Mode_St = FAN_Mode_Low_Noise;
			else if(Is_Ext_FAN_Low_Noise_Mode_for_24Hour()) // 2019.04.17 : "FAN 저소음Mode" is always On.
				m_St.FAN_Mode_St = FAN_Mode_Low_Noise;					
			else
				m_St.FAN_Mode_St = FAN_Mode____Normal;
			break;
	}
	Proc_Ext_FAN_PWM_Ctrl();
}



static INT8U Psu_Dnld_End_Chk_Req = 0;

#ifdef __YCSO__TEST__CONFIRM__

//INT8U PSU_FW_APPLY( INT8U* data, INT8U len, INT8U* pACK )
INT8U PSU_FW_APPLY( INT8U* data, INT8U len )
{
	INT8U dnld_St;
	
	PSU_Send_Data( PSU_CMD_ST, NULL, 0 );

	dnld_St = m_St.DL_State[DN_PSU][DL_ST];
	D_Printf( DEBUG_SYSTEM, "\r\n[RCU] [PAU_FW_APPLY] PSU (%d)", dnld_St);
	
	if(( dnld_St == DL_St_Complete)||( dnld_St == DL_St_Fail))
	{	
		if( PSU_Send_Data( PSU_CMD_FW_APPLY, data, len ) )
		{		
//			*pACK = m_St.PSU_FW_NACK;
				
			if(data[0] == 0) Psu_Dnld_End_Chk_Req = 1; // apply
			else Psu_Dnld_End_Chk_Req = 0; // cancel

			Clr_Psu_Inv_Read_Req();
			return 1;
		}		
	}	
	// default
	Psu_Dnld_End_Chk_Req = 0;
	return 0;
}
#endif

void Chk_PSU_Download_End(void)
{
	if(Psu_Dnld_End_Chk_Req)
	{	
		if(m_St.DL_State[DN_PSU][DL_ST] == DL_St_IDLE)
		{
			D_Printf( DEBUG_SYSTEM, "[PSU_DN] END ");
	
			Psu_Dnld_End_Chk_Req = 0;
			PSU_Init_Set();
		}
	}
}

#ifdef __YCSO__TEST__CONFIRM__
///// YCSO added 190516
void PSU_FW_apply_reqest(INT8U env)
{
	INT8U data[2];
	INT8U retry_cnt = 0;

	switch( env )
	{
		case 0x01:
			data[0] = 0x00;
			data[1] = 0x00;
			break;

		case 0x02:		////// cancle
			data[0] = 0x01;
			data[1] = 0x00;
			break;

		default:
			data[0] = 0x00;
			data[1] = 0x00;
			break;
	}
	for( retry_cnt = 0; retry_cnt <10; retry_cnt ++) // 2019.09.05 : retry added. //// added YCSO 190920 // 2019.09.20
	{
		if( PSU_FW_APPLY( data, 2) )
		{
			m_Env.PSU_confirm_DL = 0;
			return;
		}
		OSTimeDly(OS_DELAY_500msec); // 2019.09.20

	}

	printf("[PSU_FW_apply_reqest] fail conrim PSU FS APPLY cnt = %d \n", retry_cnt);
	PSU_Down_Fail(); // 2019.09.20

	m_Env.PSU_confirm_DL = 0;
}
#endif
/*
--------------------------------------------------------------------------------------
* Battery Related 
--------------------------------------------------------------------------------------
	* algorithm 동작 조건.
		[1] Battery : not_insert ---> insert 변경 시
		[2] RCU Reset시 Battery insert 일 때.
			- not insert : algorithm (X)
		    - AC/DC/Battery Low alarm : algorithm (X)


	* Flow
		가) algorithm 동작 중 동작 조건 재 발생시 algorithm 초기화.		
		나) algorithm 30분 후 PSU의 충전회로 OFF
			( 30분 : 완전 방전된 Battery 충전대기 시간)

			충전 회로 OFF 제어 후 5초 후에 alarm 체크
			 - 기준 전압 30V 이하로 측정 시 불량, Alarm 발생, 충전회로 ON	

	* 24시간 마다 Battery 장애 체크 진행
--------------------------------------------------------------------------------------		
*/

typedef struct  __attribute__((packed))
{
	INT8U St;
	INT8U Batt_Use_Ctrl; // user ctrl로 batt use 여부 check 중.
	INT8U batt_Ins_Prev;
	OS_TIMER timer;
}BATT_ABNORMAL_MON;
static BATT_ABNORMAL_MON m_Batt_Mon;

enum
{	
	Batt_St_When_Reset = 0,  // init time after reset
	Batt_St_None,            // batt-not-inserted-state
	Batt_St_Related_Alm_Chk, // 
	Batt_St_Charge_OFF,      // Off_Charging_Circuit
	Batt_St_Delay,           // delay before alarm-check
	Batt_St_24_Hour,         // check paused for 24hour	
	BattChk_Start,	
	BattChk_Decision,
	Batt_St_2_Hour,         // check after 2hour, when reset.
};

#define BATT_USE_WATI_ms (30000) // 30[s]
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// RCU reset 되면 바로 disable.
void Batt_Abnormal_Mon_Disable(void)
{
	m_Batt_Mon.St = Batt_St_When_Reset;
}

//---------------------------------------------------------------------------
// PSU 통신 한 후에 아래 함수 호출.: batt insert info 때문에...
// AC/DC/Battery Low alarm : algorithm (X)
INT8U Is_Batt_Related_Alm(void)
{
	if(m_St.AlmSt.A_1.Batt_Low) return 1;
	if(m_St.AlmSt.A_0.AC) return 1;
	if(m_St.AlmSt.A_0.DC) return 1;

	return 0;
}
//---------------------------------------------------------------------------

#define MAX_INIT_BATT_MON_CNT (10)

#ifdef _SHORTEN_TIME____BATT_MON_
#define BATT_CHK_24_HOUR_ms (2*60*1000)
#define BATT_CHK_02_HOUR_ms (1*60*1000)
#else
#define BATT_CHK_24_HOUR_ms (24*60*60*1000)
#define BATT_CHK_02_HOUR_ms (2*60*60*1000)
#endif


/*
	---------------------------------------------
	5G, PRU
	---------------------------------------------
	0x00: 미설치
	0x07: 내장(50V) : 2019.04.16. AID Changed
	---------------------------------------------
*/
#define BATT_INT_50V (0x07) // internal built-in, 2019.04.16. AID Changed


void Use_Batt_Use( INT8U use_val )
{	
	if(m_St.AlmTest) // 2019.05.01
		return; 
		
	if(use_val == 0) // not use
	{
		m_Batt_Mon.St = Batt_St_None;

		m_St.AlmSt.A_1.Batt_St = _ALM_CLR_;

		//// 2019.04.25.batt.
		PSU_BattCharge_Ctrl( ON ); // ON		
	}
	else if(use_val == BATT_INT_50V)
	{
		m_Batt_Mon.St = Batt_St_When_Reset;
	}
	else return; // not control..
}






/*
-----------------------------------------------------

Battery Self Diagnosis

-----------------------------------------------------
*/
static INT8U batt_diag_alarm = 0; // Last Alarm


INT8U Get_Result_of_Batt_Diagnosis(void)
{
	return batt_diag_alarm;
}
void Proc_Batt_Abnormal_Monitor(void)
{
	static INT8U prev_st = 0xFF;
	INT16U volt_x10;
	INT8U Batt_Abnormal;
	static INT8U cnt = 0;
	static INT8U batt_related_alm = 0;
	INT8U batt_insert;

	
	//-------------------------------------------------
	if(m_St.AlmTest) // 2019.05.01
		return;
	//-------------------------------------------------

	if(cnt <= 3)
	{
		cnt++;
		//D_Printf( DEBUG_SYSTEM, "[BATT] cnt %d\r\n", cnt);
		return;
	}

	batt_insert = (m_PSU.PSU_St_Etc & C_BIT(0))? 0:1; 
	if(batt_insert == 0) // if not insert
	{
		m_Batt_Mon.St = Batt_St_When_Reset;
		prev_st = m_Batt_Mon.St;
		return;
	}
	
	switch( m_Batt_Mon.St )
	{					
		//-------------------
		case Batt_St_When_Reset: // init time after reset
			m_Batt_Mon.Batt_Use_Ctrl = 0;
			OSTimerReset( &m_Batt_Mon.timer );			 
			
			if(m_St.Batt_Insert == BATT_INT_50V) 			
				m_Batt_Mon.St = Batt_St_2_Hour; // 2019.04.25.batt.
			else if(m_St.Batt_Insert == 0)
				m_Batt_Mon.St = Batt_St_None;			
			else // neither insert nor not-insert...			
				m_Batt_Mon.St = BattChk_Start;	
		break;			
		//-------------------
		case BattChk_Start: // user ctrl 일때 ..
			D_Printf( DEBUG_SYSTEM, "[BATT] USE-Chk\r\n");
			OSTimerReset( &m_Batt_Mon.timer );
			m_Batt_Mon.St = Batt_St_Related_Alm_Chk;
		break;			
		//-------------------
		case Batt_St_Related_Alm_Chk:
			D_Printf( DEBUG_SYSTEM, "[BATT] Related Alm Chk\r\n");
			batt_related_alm = Is_Batt_Related_Alm();
			
			if(m_Batt_Mon.Batt_Use_Ctrl == 0) // every 24 hour
			{
				if( batt_related_alm )
				{
					// go to next 24hour
					m_Batt_Mon.St = Batt_St_24_Hour;
					OSTimerReset( &m_Batt_Mon.timer );				
					batt_related_alm = 0; // not used..
					D_Printf( DEBUG_SYSTEM, "[BATT] go to 24 hour\r\n");
					break;
				}
			}
			// every 24 hour && no-related alarm OR
			// user-ctrl-request
			m_Batt_Mon.St = Batt_St_Charge_OFF;
		break;
		//-------------------
		// 충전 회로 off request
		case Batt_St_Charge_OFF: 
			D_Printf( DEBUG_SYSTEM, "[BATT] Charging Off\r\n");
			
			PSU_BattCharge_Ctrl( OFF );
			OSTimerReset( &m_Batt_Mon.timer );
			m_Batt_Mon.St = Batt_St_Delay;			
			break;
			
		//-------------------
		case Batt_St_Delay: // 30[s]
			if( m_PSU.Batt_Charge_OnOff != OFF )
			{
				PSU_BattCharge_Ctrl( OFF );
				OSTimerReset( &m_Batt_Mon.timer );
			}

			if( OSTimerGet_msec(&m_Batt_Mon.timer) > 30000 )
				m_Batt_Mon.St = BattChk_Decision;
		break;
		
		//-------------------
		case BattChk_Decision:
			volt_x10 = m_PSU.Batt_Volt_x10;			
							
			if(volt_x10 <= 40*10) Batt_Abnormal = 1; // below 20[V]	
			else                  Batt_Abnormal = 0;				
			//----------------------------------------------------

			//// 2019.04.25.batt.
			if(m_St.Batt_Insert == BATT_INT_50V) // USE, install
			{
				// check every 24 hour
				m_Batt_Mon.St = Batt_St_24_Hour;
				OSTimerReset( &m_Batt_Mon.timer ); 				
			}
			else // not install
			{
				m_Batt_Mon.St = Batt_St_None;
			}
			
			PSU_BattCharge_Ctrl( ON );

			if(Batt_Abnormal) m_St.AlmSt.A_1.Batt_St = _ALM_SET_;
			else              m_St.AlmSt.A_1.Batt_St = _ALM_CLR_;

			batt_diag_alarm = Batt_Abnormal;

			if(batt_related_alm)
			{
				batt_related_alm = 0;
				m_St.AlmSt.A_1.Batt_St = _ALM_CLR_;
			}			
		break;

		//-------------------
		case Batt_St_24_Hour:
			if( OSTimerGet_msec(&m_Batt_Mon.timer) > BATT_CHK_24_HOUR_ms )
			{
				OSTimerReset( &m_Batt_Mon.timer );
				m_Batt_Mon.St = Batt_St_Related_Alm_Chk;
			}
		break;

		//-------------------
		case Batt_St_2_Hour:
			if( OSTimerGet_msec(&m_Batt_Mon.timer) > BATT_CHK_02_HOUR_ms )
			{
				OSTimerReset( &m_Batt_Mon.timer );
				m_Batt_Mon.St = Batt_St_Related_Alm_Chk;
			}
		break;

		
				
		//-------------------
		case Batt_St_None:
			// do nothing
		break;

		//-------------------
		default: 
			m_Batt_Mon.St = Batt_St_None;
		break;
	}
	
	if(m_Batt_Mon.St != prev_st)
	{
		D_Printf( DEBUG_SYSTEM, "[BATT] St:%d\r\n", m_Batt_Mon.St);
	}
	prev_st = m_Batt_Mon.St;	
}




/*
-	동작 조건 : 시스템 Reset 시에 Battery 실장인 경우
              :  '미실장 -> 실장'으로 변경 된 경우
-	동작 Flow  :  동작 조건 만족하는 경우 2분 경과 후 충전회로 OFF하여 Batt 전압 측정
               :  Batt전압 < 40V 이면 Alm(0x03 전압이상) 발생 후, 충전 회로 ON.
-	주기적 체크 :  상기 동작을 Flow 완료 시점 기준으로 2분마다 반복
-	알람 상위 보고 (추후 구현)
@ 2분 조건은 DVT 테스트 용도입니다. 추후 24시간으로 변경.


*/
INT8U Is_Under_Batt_Self_Diagnosis(void)
{
	switch(m_Batt_Mon.St)
	{
		case Batt_St_When_Reset:
		case Batt_St_None:
		case Batt_St_24_Hour:
			return 0;
			
		default:
			return 1;
	}
}


/*
--------------------------------------------------------------------------------------
* Psu Communication
--------------------------------------------------------------------------------------
*/
INT8U Is_PSU_CommFail(void)
{		
	if( PSU_Link_Fail > 0) return 60;

	return 0;
}
//---------------------------------------------------------------------------
INT8U Is_PSU_CommFail_Co(void)
{		
	if( PSU_Link_Fail_Co > 0) return 60;

	return 0;
}

/*
--------------------------------------------------------------------------------------
* Psu Auto-download
--------------------------------------------------------------------------------------
*/
/////////////////ycso START //////////////////////
static INT8U* DnImg_PSU = NULL;

void Free_RAM_DnImg_PSU(void)
{
	if (DnImg_PSU != NULL)
	{
		free(DnImg_PSU);
		DnImg_PSU = NULL;
	}
}

int Load_RAM_DnImg_PSU( char* pullFilePath )
{
	int fd;
	struct stat st;
	int fileSize = -1;
	int re = -1;
		
	fd = open(pullFilePath, O_RDONLY);
	
	if( fd < 0 )
	{
		syslog( LOG_WARNING, "[RCU] no File(1).\n");
		return re;
	}
	
	if(stat(pullFilePath, &st) == 0)
	{
		fileSize = st.st_size;		
	}
	
	if(fileSize <= 0)
	{
		syslog( LOG_WARNING, "[RCU] no File(2).\n");
		return re;
	}	

	Free_RAM_DnImg_PSU();
	
	DnImg_PSU = (INT8U*)malloc(fileSize+1);
		
	if (DnImg_PSU)
	{			
		memset( DnImg_PSU, 0, fileSize+1);
		re = read( fd, DnImg_PSU, fileSize );
		if( re <= 0 )
		{
			Free_RAM_DnImg_PSU();	
			syslog( LOG_WARNING, "[RCU] rd fail(1).\n");
			re = -1;
		}		
	}
	else
	{
		syslog( LOG_WARNING, "[RCU] malloc fail(2).\n");
		re = -1;
	}
	close(fd);

	return re;
}


int Get_Multi_Dn_Tot_Frame_Crc_PSU( INT16U* pCRC, INT16U* pTotFrame, MOD_TYPE* pMod )
{
	char file[128];
	int size;
	int frame_co;
	INT16U tot_crc;

	Get_PSU_Dn_File_Name( file );
	size = Load_RAM_DnImg_PSU( file );
	D_Printf( DEBUG_SYSTEM, "[Multi-PSU] %d, %s \r\n", size, file);

	if(size < 0) return -1;

	tot_crc = crcLByteCalc( 0, DnImg_PSU, size ); // init with 0

	frame_co = size / FRAME_DATA_MAX;
	if((size % FRAME_DATA_MAX) > 0) frame_co += 1;
	
	*pCRC = tot_crc;
	*pTotFrame = frame_co;	

	D_Printf( DEBUG_SYSTEM, "[Multi-PSU] Tot CRC 0x%04X, Tot Frame %d \r\n", tot_crc, frame_co);

	return size;
}

INT16U Get_Multi_Dn_Data_PSU( INT8U* data, INT16U cur_frame, INT16U tot_frame, int tot_size )
{
	int idx;
	INT16U len;

	idx = cur_frame*FRAME_DATA_MAX;
	
	if( cur_frame < (tot_frame-1) )
	{		
		len = FRAME_DATA_MAX;
	}
	else
	{
		len = tot_size - idx;
	}

	memcpy( data, &DnImg_PSU[idx], len);

	return len;
}


/////////////////ycso END //////////////////////


void Init_Psu_AutoDn(void)
{
	m_AutoDn_Psu.req = 0;
	m_AutoDn_Psu.percent = 0;	
	m_AutoDn_Psu.result = Multi_Dn_None;
	m_AutoDn_Psu.fw_apply_req = 0;	
}
//---------------------------------------------------------------------------
#define Retry_Max (5)		//// added YCSO 190920 3 --> 5 // 2019.09.20
void Rx_Psu_AutoDown( INT8U rx_type, INT8U* data ) 
{
	INT16U uVal16;
	INT8U nack = 0; // default

	switch( rx_type )
	{	
		case PSU_CMD_DN_NOTI:    // 1,2차 통보.
		case PSU_CMD_DN_NOTI_FW: // 1,2차 통보, FW 적용.
		case PSU_CMD_DN_NOTI_CHK:			
		case PSU_CMD_DN_DATA:	
		case PSU_CMD_DN_END:
			if(data[0] != 0) 
			{
				nack = 1;
				m_AutoDn_Psu.retry++;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-PSU] down nak [0x%02X]", rx_type);
				return;
			}
			break;
	}	
	
	switch( rx_type )
	{	
		case PSU_CMD_DN_NOTI:    // 1,2차 통보.
		case PSU_CMD_DN_NOTI_FW: // 1,2차 통보, FW 적용.
			// total frame
			uVal16 = data[2]; uVal16 <<= 8; uVal16 |= data[3];			
			if(uVal16 != m_AutoDn_Psu.tot_frame) nack = 1;
						
			if(nack == 0)
			{
				if(m_AutoDn_Psu.st == Auto_Dn_Noti_1) 
					 m_AutoDn_Psu.st = Auto_Dn_Chk;
				else m_AutoDn_Psu.st = Auto_Dn_Data;
			}
			break;

		case PSU_CMD_DN_NOTI_CHK: // 통보 확인.
			if(nack == 0) m_AutoDn_Psu.st = Auto_Dn_Noti_2;
			break;
			
		case PSU_CMD_DN_DATA: // 다운로드
			// cur frame
			uVal16 = data[1]; uVal16 <<= 8; uVal16 |= data[2];
			if(uVal16 != m_AutoDn_Psu.cur_frame) 
			{
				nack = 1;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-PSU] down nak [%d != %d]", uVal16, m_AutoDn_Psu.cur_frame);
			}
			
			if(nack == 0)
			{				
				if(uVal16 >= (m_AutoDn_Psu.tot_frame-1)) // if last frame
				{
					m_AutoDn_Psu.st = Auto_Dn_End;
				}
				else m_AutoDn_Psu.cur_frame++;
			}
			break;
	
		case PSU_CMD_DN_END: // 확인
			// tot frame
			uVal16 = data[1]; uVal16 <<= 8; uVal16 |= data[2];
			if(uVal16 != m_AutoDn_Psu.tot_frame)
			{
				nack = 1;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-PSU] down nak [%d != %d]", uVal16, m_AutoDn_Psu.tot_frame);
			}
			
			if(nack == 0) m_AutoDn_Psu.st = Auto_Dn_Success;			
			break;
	}	

	if(nack) m_AutoDn_Psu.retry++;
	else	 m_AutoDn_Psu.retry = 0;
}
//---------------------------------------------------------------------------
INT8U AutoDown_Psu_Send_Data( INT8U type, INT8U* data, INT16U l )
{
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
	INT16U len;	

	pthread_mutex_lock(&Psu_COMM_Evt);

	// RCU
	psu_txf.ScrAddr = 0x00;
	psu_txf.DesAddr = _PSUID_PSU_;
	psu_txf.Type = type;

	if(l) memcpy( psu_txf.data, data, l );		
	len = Make_Frame_PAU( type, &psu_txf, l, PSU_MSG_ID_Idx );

	if(Send_DN_Data(COMM_UART___PSU, (INT8U*)&psu_txf, len, (INT8U*)&psu_rxf, PSU_Timout_ms)) 
	{		
		if((psu_rxf.Type == type) && (psu_rxf.ScrAddr == _PSUID_PSU_))
		{	
			Rx_Psu_AutoDown( type, psu_rxf.data );
			pthread_mutex_unlock(&Psu_COMM_Evt);
			return 1;	
		}
	}
	
	D_Printf( DEBUG_SYSTEM, "[!] Psu Dn [0x%02X]\r\n", type );	

	pthread_mutex_unlock(&Psu_COMM_Evt);
	CheckUART_Error( COMM_UART___PSU );
#endif
	return 0;	
}
//---------------------------------------------------------------------------
void PSU_Down_Fail(void)
{
	m_AutoDn_Psu.retry = 0;
	m_AutoDn_Psu.result = Multi_Dn_Fail;
	m_AutoDn_Psu.st = Auto_Dn_None;
	m_AutoDn_Psu.req = 0; // end
	Free_RAM_DnImg_PSU();
	m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920 // 2019.09.20
	D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down fail \r\n");
}
//---------------------------------------------------------------------------
void Proc_Psu_AutoDownload(void)
{
	INT8U pau_type;
	INT16U l = 0;
	INT8U ret;
	INT16U uVal16;
	int re;
	MOD_TYPE DnMod;

	DnMod.type = Mod_PSU;
	
	switch( m_AutoDn_Psu.st )
	{
		case Auto_Dn_None:			
			break;

		case Auto_Dn_Start:			
			m_AutoDn_Psu.cur_frame = 0;			
			m_AutoDn_Psu.retry = 0;
			m_AutoDn_Psu.result = Multi_Dn_Progress;
			m_AutoDn_Psu.percent = 0;
			m_AutoDn_Psu.fw_apply_req = 0;

			// get crc and tot-frame
			re = Get_Multi_Dn_Tot_Frame_Crc_PSU( &m_AutoDn_Psu.crc, &m_AutoDn_Psu.tot_frame, &DnMod );
			if(re <= 0) PSU_Down_Fail();
			else       
			{
				m_AutoDn_Psu.st = Auto_Dn_Noti_1;
				m_AutoDn_Psu.tot_size = re;
				D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down start \r\n");
				syslog( LOG_CRIT, VT_BOLD_GREEN " PSU down start \n" VT_END_CR );


			}
			//D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down start \r\n");
			break;		

		case Auto_Dn_Noti_1:
		case Auto_Dn_Noti_2:
			D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down noti %d \r\n", m_AutoDn_Psu.st);

		#ifdef __YCSO__TEST__CONFIRM__
			pau_type = AMP_CMD_DN_NOTI_FW; // AMP_CMD_DN_NOTI_FW
		#else
			pau_type = PSU_CMD_DN_NOTI; // AMP_CMD_DN_NOTI_FW
		#endif
			uVal16 = m_AutoDn_Psu.tot_frame;
			l = 0;
			m_AutoDn_Psu.data[l++] = 0; // reserved
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 >> 8); // total frame size
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 & 0xFF); // total frame size

			// operation mode (0:reset waiting, 1 : auto reset after download)
			if(pau_type == PSU_CMD_DN_NOTI_FW)
			{
				m_AutoDn_Psu.data[l++] = 0; 
			}
			break;

		case Auto_Dn_Chk:
			D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down chk \r\n");
			pau_type = PSU_CMD_DN_NOTI_CHK;
			l = 0; // no subdata
			break;

		case Auto_Dn_Data:			
			pau_type = PSU_CMD_DN_DATA;
			uVal16 = m_AutoDn_Psu.cur_frame;
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 >> 8); // cur frame #
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 & 0xFF); // cur frame #
			
			// download data (payload)
			l += Get_Multi_Dn_Data_PSU( &m_AutoDn_Psu.data[l], uVal16, m_AutoDn_Psu.tot_frame, m_AutoDn_Psu.tot_size );
			//printf("psu down data %d len %d \r\n", m_AutoDn_Psu.cur_frame, l-2);
			m_AutoDn_Psu.percent = uVal16 *100 / m_AutoDn_Psu.tot_frame;
			break;

		case Auto_Dn_End:
			D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down end \r\n");
			pau_type = PSU_CMD_DN_END;
			uVal16 = m_AutoDn_Psu.tot_frame;
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 >> 8); // total frame size
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 & 0xFF); // total frame size
			uVal16 = m_AutoDn_Psu.crc;
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 >> 8); // total crc
			m_AutoDn_Psu.data[l++] = (INT8U)(uVal16 & 0xFF); // total crc
			break;

		case Auto_Dn_Success:
			D_Printf( DEBUG_SYSTEM, "[Multi-PSU] down success \r\n");
			syslog( LOG_CRIT, VT_BOLD_GREEN " PSU down success \n" VT_END_CR );
			m_AutoDn_Psu.st = Auto_Dn_None; 
			m_AutoDn_Psu.req = 0; // end
			m_AutoDn_Psu.percent = 100;
			m_AutoDn_Psu.fw_apply_req = 1;
			#ifdef __YCSO__TEST__CONFIRM__
			set_psu_DN_sataus(_SET_);
			#else
			set_psu_DN_confirm_END(_SET_);
			#endif
//			set_psu_DN_sataus(_SET_);
//			set_zip_DN_flag(1);//	Zip_flag_PSU
			
			PSU_Init_Set();
			break;
			
		default :
			m_AutoDn_Psu.st = Auto_Dn_None; 
			break;
	}	

	switch( m_AutoDn_Psu.st )
	{
		case Auto_Dn_Noti_1:
		case Auto_Dn_Noti_2:
		case Auto_Dn_Chk:
		case Auto_Dn_Data:
		case Auto_Dn_End:
			ret = AutoDown_Psu_Send_Data( pau_type, m_AutoDn_Psu.data, l );

			if(ret == 0) m_AutoDn_Psu.retry++;
			
			if(m_AutoDn_Psu.retry >= Retry_Max)
			{
				PSU_Down_Fail();
			}
			break;

		default: break;
	}
}
//---------------------------------------------------------------------------
INT8U Is_PSU_MultiDown(void)
{		
	if(m_AutoDn_Psu.req) return 1;
	if(info_PSU.upgrade) return 1;
	
	return 0;
}
//---------------------------------------------------------------------------
INT8U Proc_PSU_PkgDown(void)
{
	if(info_PSU.upgrade)
	{
		info_PSU.upgrade = 0;
		if ( PSU_Link_Fail == 0 )		//// added YCSO 190920 // 2019.09.20
		{
			set_psu_DN_flag(_SET_);
			Init_Psu_AutoDn();
			m_AutoDn_Psu.req = 1;
		}
	}
	
	if(m_AutoDn_Psu.req)
	{
		if(m_AutoDn_Psu.result == Multi_Dn_None)
		{
			m_AutoDn_Psu.st = Auto_Dn_Start;
		}
		Proc_Psu_AutoDownload();
		
		return 1;
	}
		
	return 0;
}
//---------------------------------------------------------------------------
void Update_PSU_DL_State(void)
{	
#ifdef _Debug____MULTI_DOWN_ST_	
	if(m_AutoDn_Psu.req)
	{
		m_St.DL_State[DN_PSU][1] = m_AutoDn_Psu.percent;
		m_St.DL_State[DN_PSU][0] = DL_St_Now;
	}
	if(m_AutoDn_Psu.fw_apply_req)	
	{
		m_St.DL_State[DN_PSU][1] = 100;
		m_St.DL_State[DN_PSU][0] = DL_St_Complete;
	}
#endif	
}
//---------------------------------------------------------------------------
enum
{
	Batt_Normal = 0,
	Batt_Fault_Charge,   // 1: Battery Fault(충전차단)
	Batt_Fault_DisCharge,// 2: Battery Fault(방전차단)
	Batt_Fault_Voltage,  // 3: Battery Fault(Battery 전압 이상)
	Batt_Fault_HW,       // 4: Battery Fault(HW)
};
void Proc_Batt_Status(void)
{	
	INT8U batt_sts;
	INT8U batt_abnormal = 0;
	INT8U batt_insert;  // 1 : insert
	static INT8U batt_ins_prev = 0;
		/*	
			PSU_Arm[4]
			----------------------------
			bit 3: Battery Status1 // 충전차단.
			bit 2: Battery Status2 // 방전차단.
			bit 1: Battery Status3 // battery
		*/
	
	//-------------------------------------------------
	if(m_St.AlmTest) // 2019.05.01
		return;
	//-------------------------------------------------

	batt_insert = (m_PSU.PSU_St_Etc & C_BIT(0))? 0:1; 
	
	if(batt_insert) // if insert
	{
		m_St.Batt_Insert = BATT_INT_50V;		
		
		batt_sts = m_PSU.PSU_St_Etc & 0x0E;

		////D_Printf( DEBUG_SYSTEM, "[RCU] Proc_Batt_Status 0x%02X.\n", m_PSU.PSU_St_Etc);	

		//////////batt_abnormal = Is_Under_Batt_Self_Diagnosis();
		batt_abnormal = Get_Result_of_Batt_Diagnosis();
				
		if((batt_sts == 0)&&(batt_abnormal == 0))	
			m_St.Batt_Sts = Batt_Normal; // normal
		else
		{
			// batt fault
			if(batt_abnormal)                 m_St.Batt_Sts = Batt_Fault_Voltage; // batt.-fault
			else if(m_St.AlmSt.A_1.Batt_Low)  m_St.Batt_Sts = Batt_Fault_Voltage; // batt.-fault			
			else if(batt_sts & cbits[3]) m_St.Batt_Sts = Batt_Fault_Charge; // Cut-off-charging, Battery Status1
			else if(batt_sts & cbits[2]) m_St.Batt_Sts = Batt_Fault_DisCharge; // Cut-off-discharging, Battery Status2
			else if(batt_sts & cbits[1]) m_St.Batt_Sts = Batt_Fault_HW; // Battery Status3
			else                         m_St.Batt_Sts = Batt_Normal;
		}

		if(m_St.Batt_Sts == Batt_Normal)
			m_St.AlmSt.A_1.Batt_St = _ALM_CLR_;
		else
			m_St.AlmSt.A_1.Batt_St = _ALM_SET_;
	}
	else
	{
		m_St.Batt_Sts = Batt_Normal; // normal
		m_St.Batt_Insert = 0; // not insert
	}
	if(batt_ins_prev != m_St.Batt_Insert)
		Use_Batt_Use(m_St.Batt_Insert);
		
	batt_ins_prev = m_St.Batt_Insert;	
}



/*
--------------------------------------------------------------------------------------
 TASK of PSU																					   
--------------------------------------------------------------------------------------
*/
void* Psu_Comm_Task(void *data)
{
#ifdef __TYPE__PRU__ // PRU Only
	INT32S res;
	Thread_ARG_Var* p_th_arg;
	INT8U port = COMM_UART___PSU;	
	//INT8U cnt = 0;
	INT8U continue_x;
	
	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);	

	while(m_Env.UDP_Init[COMM_UDP_NODE] == 0)
		OSTimeDly( OS_DELAY_1sec ); // UDP 초기화 대기

	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "PSU Comm Task RUN.");	
	
	res = pthread_mutex_init(&Psu_COMM_Evt, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] PSU Mutex Init Fail.");
		exit(EXIT_FAILURE);
	}	
	res = pthread_mutex_init(&Psu_Ctrl_Sem, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] PSU Mutex2 Init Fail.");
		exit(EXIT_FAILURE);
	}	

	Init_Comm( port );

	memset(&m_PSU, 0, sizeof(PSU_ST));

	Init_Psu_Down_St();		
	Green_Mode_Ctrl(OFF);

	// Battery Extension 
	//// 2019.04.25.batt.
	Batt_Abnormal_Mon_Disable();

	//---------------------------------------------------

	if(m_Eep.Batt_Ext_Mode_Onoff != OFF) 
		m_Eep.Batt_Ext_Mode_Onoff = ON; // default ON
		
	if(m_Eep.FAN_Ext_Max_RPM > 100)
		m_Eep.FAN_Ext_Max_RPM = 75;
	m_St.FAN_Ext_Max_RPM = m_Eep.FAN_Ext_Max_RPM;
		
	//---------------------------------------------------
	
	Init_Psu_AutoDn();
	PSU_Comm_Cnt = 0; // 2020.04.08
	//---------------------------------------------------
	while( m_Env.task_flag[_TASK_PSU_COMM_] )		///// YCSO TASK END
	{
	/*
		//continue_x = 0;
		//if(m_Env.is_down_load)
		//{
		//	continue_x |= cbits[1];
		//	OSTimeDly(OS_DELAY_100msec*2);
		//}
		//else
		//	OSTimeDly(OS_DELAY_500msec);	
	*/
		
		OSTimeDly( OS_DELAY_100msec*3 );		///// ycso 전체 속도 향상을 위해 500ms -> 300ms
		Update__Task_Count(_TASK_PSU_COMM_);
		
		if(Get_Comm_fd_Status(port) < 0)
		{
			D_Printf(DEBUG_SYSTEM, "[RCU] PSU Serial Fail.");
			OSTimeDly( OS_DELAY_1sec*3 );			
			continue;
		}		
		//-------------------------------
		continue_x = 0;
		
		if(m_Env.is_down_load) continue_x |= cbits[1];
		if(Is_Dig_MultiDown()) continue_x |= cbits[3];
		if(Proc_PSU_PkgDown()) continue_x |= cbits[5];
		
		#ifdef __YCSO__TEST__CONFIRM__
		if(m_Env.PSU_confirm_DL)
		{
			PSU_FW_apply_reqest(m_Env.PSU_confirm_DL);
			continue_x |= cbits[6];
		}
		#endif
		if(continue_x)
		{
			//D_Printf( DEBUG_SYSTEM, "[RCU] PSU : Disabled by 0x%02X.\n", continue_x);
			continue;
		}
		//-------------------------------
		Proc_Ext_FAN_Mode();
		
		if(PSU_Polling())
		{
			// 2020.04.08
			PSU_Comm_Cnt++;
			if(PSU_Comm_Cnt >= 100) 
				PSU_Comm_Cnt = 100;
			
			Chk_PSU_Download_End();
			
			if(PSU_Init_End != 1)
			{
				OSTimeDly( OS_DELAY_1sec );
				InvetoryReadClear_mod( INV_IDX_R_PSU );	
				Batt_Abnormal_Mon_Disable();
				
				PSU_BattCharge_Ctrl(ON);  // default ON 2019.04.25
				Init_Psu_Down_St();

				PSU_Init_End = 1;
			}

			if(CheckInvetoryRead(INV_IDX_R_PSU) == 0)
				PSU_Get_Info();
			
			PSU_FAN_Ctrl_Chk();

			Chk_PSU_Green_St();

			if(m_PSU.Batt_Charge_OnOff != m_Env.Batt_Charge_OnOff)
				PSU_BattCharge_Ctrl(m_Env.Batt_Charge_OnOff);			
			
			Proc_Batt_Abnormal_Monitor(); // 2019.04.25. Restore
			Proc_Batt_Status();
		}
		else
		{
			PSU_Comm_Cnt = 0; // 2020.04.08
		}
	}

	m_Env.task_END_status[_TASK_PSU_COMM_] = _CLR_;		///// YCSO TASK END

#endif	
	pthread_exit(NULL);
}



//------------------------------------------------------------------

/* EOF */


