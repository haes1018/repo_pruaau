/////////////////////////////
//	NMS_ALARM.C
/////////////////////////////
#define NMS_ALARM_GLOBALS
#include "includes.h"
/////////////////////////////

//static char dbgStr[100];
/////////////////////////////
// Local Functions
/////////////////////////////

/*
------------------------------------------------------------------------------
[1] When User On/Off
	Disabled Alarm(AID 0x5117), REMS String : "User Control"

[2] when AC Fail/AMP Standby/MiMo Standby(by RCU Algorithm)
	Disabled Alarm(AID 0x5117), REMS String : "System Control"
	?? "CPRI", "Clock" 는 by-RCU-Alarm 아닌가??

[3] When Amp-off by AMP/PAU	
	AMP_SD_Alarm (AID 0x5215)
	reason(string)
		"Overpower"(완전 shutdown의 경우에만 1회 발생)
		"VSWR"     (완전 shutdown의 경우에만 1회 발생)
		"HighTemp"
		"DC"
		"CPRI"
		"Clock"
		"Dev Fail" (REMS String 없음)	
	

AMP_SD_Alarm(AID 0x5215), Disabled_Alarm(AID 0x5117) 은 동시 발생하지 않음.


************ Check point
[1] - "CPRI", "Clock" 일때, AMP_SD_Alarm (AID 0x5215) 재확인.
[2] AC Fail/AMP Standby/MiMo Standby(by RCU Algorithm) : Disabled Alarm(AID 0x5117), REMS String : "System Control"
------------------------------------------------------
AID 0x5210 : UL_Over_In	Alarm
	- Algorithm 중 Alarm과 해제가 GUI/gREMS 계속 보고.
	- Shutdown algorithm 중, AMP_SD_Alarm (AID 0x5215) 도 발생 됨.	
------------------------------------------------------------------------------
*/


//--------------------------
#define DATA_ALM_ASCII_MAX (20)

typedef struct __attribute__((packed))
{
	//------------ 
	INT8U AID[2];
	INT16U len; // 6 + data[flag + data], A_ID 및 Length 필드 길이 제외
	//------------
	INT8U type; // Type___None ~ Type_PrdDef
	INT8U step; // Step_No_Use ~ Step_100000
	INT8U reserved;
	INT8U size;  // data 1 size
	INT8U layer; // layer num
	INT8U carrier; // carrier num
	INT8U is_alarm; // 0 : clr, 1 : alarm	
	INT8U data[DATA_ALM_ASCII_MAX]; // ASCII
	//------------
} ALM_AIDSTR_5G;

//--------------------------
INT16U Make_Arm_gREMS( ALM_AIDSTR_5G* paid, INT8U byte, INT8U bit, INT8U alm_f );


static REMS_1024_Pkt UDA_Alm_EMS_Pkt;
static REMS_1024_Pkt Alm_EMS_Pkt;
static ALM_AIDSTR_5G Alm_Frame;

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
void Make_ALM_EMS_SubData_Init( REMS_1024_Pkt* pPkt )
{
	INT16U l;
	
	pPkt->AID[0] = 0x23; // Not used @ 5G, it will be deleted..
	pPkt->AID[1] = 0x23; // Not used @ 5G, it will be deleted..
	pPkt->len = 2;		 // Not used @ 5G, it will be deleted..
	//-----------------------------------
	pPkt->cur = 0;
	pPkt->tot = 0;
	pPkt->l = 0;
	pPkt->aid_cnt = 0;

	for( l=0; l < MAX_PKT_NUM; l++ ) pPkt->size[l] = 0; // init
}
//-------------------------------------------------------------------
INT8S Make_ALM_EMS_SubData( REMS_1024_Pkt* pPkt, INT8U* paid, INT8U len)
{
	INT16U l;
	INT8U cur;
	INT8U wr_ok = 0; // default

	//----------------------------------------------------- 		
	if(paid == NULL) return -1;

	cur = pPkt->cur;
	l = pPkt->l;
	if(pPkt->l != 0xFFFF) // if empty
	{
		//-----------------------------------------------------
		if ((l+len) <= MAX_LEN_SUB_DATA_ALM)
		{					
			memcpy( &pPkt->data[UDP_MAX_LEN * cur + l], paid, len );
			l += len;
			pPkt->aid_cnt++;
			wr_ok = 1;
		}
		else
		{	
			pPkt->size[cur++] = l;
			pPkt->tot = cur;
			l = 0; // init

			if (cur >= MAX_PKT_NUM) pPkt->l = 0xFFFF;
		}
		pPkt->cur = cur;
		pPkt->l = l;
	}	
	return wr_ok;
}
//-------------------------------------------------------------------
void Make_ALM_EMS_SubData_End( REMS_1024_Pkt* pPkt )
{
	INT16U l;
	INT8U cur;
	INT8U len;
	INT8U AID_0xFFFE[30];

	cur = pPkt->cur;
	l = pPkt->l;
	
	//-----------------------------------------------------
	//#if 1
	// 2019.09.02 : alarm 보고시에는 time 정보 사용.(통계는 사용안함.)
		
	len = Make_gREMS_Time_AID_Struct(AID_0xFFFE); // time info added

	//D_Printf( DEBUG_REMS, VT_Color_Evt "Make_ALM_EMS_SubData, cur %d, l %d, len %d, max %d", 
	//	cur, l, len, MAX_LEN_SUB_DATA_ALM);

	if((l+len) > MAX_LEN_SUB_DATA_ALM)	
	{	
		pPkt->size[cur++] = l;
		pPkt->tot = cur;
		l = 0; // init

		if (cur >= MAX_PKT_NUM) pPkt->l = 0xFFFF;
	}
	memcpy( &pPkt->data[UDP_MAX_LEN * cur + l], AID_0xFFFE, len );
	l += len;
	//#endif	

	pPkt->size[cur++] = l;
	pPkt->cur = cur;
	pPkt->tot = cur;
	pPkt->l = l;	

	//D_Printf( DEBUG_REMS, VT_Color_Evt "Make_ALM_EMS_SubData : cur %d, l %d", 
	//	cur, l);

}
//-------------------------------------------------------------------





//-------------------------------------------------------------------


void Check_ResetAlarm_gREMS(REMS_1024_Pkt* pPkt) // Reset_AC_Alm_EMS_Pkt
{
	INT16U len = 0; 
	INT8U data_len;
	char* reason_str;

	ALM_AIDSTR_5G* paid = &Alm_Frame;
	
	//-----------------------------
	if(m_St.ResetReason == 0) return;
	if(m_Env.gREMS_Noti_Ready==0) return;
	//----------------------------- 

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "Check_ResetAlarm_gREMS(%d)", m_St.ResetReason); 
	
	Make_ALM_EMS_SubData_Init(pPkt); // init
	//-----------------------	
	
	memset(paid->data, 0, DATA_ALM_ASCII_MAX); // 2019.09.03
	reason_str = Get__gREMS_Reset_Reason_Str(m_St.ResetReason);
	data_len = strlen(reason_str);	
	if(data_len > DATA_ALM_ASCII_MAX) data_len = DATA_ALM_ASCII_MAX;
	memcpy(paid->data, reason_str, data_len);
		//paid->data[data_len] = 0;
		//data_len++;		
	
	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "gREMS Reset Reason : %s", reason_str); 	
	
	//-----------------------	
	paid->AID[0] = 0x56;
	paid->AID[1] = 0x20;
	paid->type = __ASCII;
	paid->step = Step_No_Use;
	paid->reserved = 0;
	paid->is_alarm = 1; // 0 : clr, 1 : alarm	
	paid->size = DATA_ALM_ASCII_MAX;
	paid->layer = 0xFF;		
	paid->carrier = 0xFF;
	paid->len = htons(7 + DATA_ALM_ASCII_MAX); // A_ID 및 Length 필드 길이 제외	
	len = (7 + DATA_ALM_ASCII_MAX+4); // 4 = aid(2) + len(2)
	//-----------------------	
	Make_ALM_EMS_SubData(pPkt, (INT8U*)paid, len);
	Make_ALM_EMS_SubData_End(pPkt);	
	//-----------------------	
	Insert_Auto_Rpt_Q( pPkt->data, len, CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
	Insert_Auto_Log_Q( pPkt->data, len, CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );

	m_St.ResetReason = 0; // 리즌 올린 후, clear.(중복방지)
}

static REMS_1024_Pkt Reset_AC_Alm_EMS_Pkt;

void Chk_RebootAlarm_gREMS(void)
{	
	Check_ResetAlarm_gREMS( &Reset_AC_Alm_EMS_Pkt ); 
}

/*		
Battery 미실장시 AC 알람 발생시에는 알람 전송 이후 ACK 대기시간을 1초로 합니다.
즉, 전원 차단시 Super Cap으로 동작시에는 ACK가 미수신되면 1초 주기로 AC 알람을 보고 합니다.
*/			

INT16U Make_REMS_1024_Pkt_for_AC_Alarm(INT8U alarm, REMS_1024_Pkt* pPkt)
{	
	INT16U len = 0; 
	INT8U data_len;
	char alm_str[100];
	ALM_AIDSTR_5G* paid = &Alm_Frame;
		
	memset(alm_str, 0, 100);
	Update_Alm_Str_Lmt(0x50, 0x16, 0, 0, alarm); // AC, PRU, 2020.01.29
	Make_Alm_Str(0x50, 0x16, 0x00, 0x00, alarm, alm_str); // 2020.02.17 : flag 추가: 항상 0 -> 'alarm'.

	Make_ALM_EMS_SubData_Init(pPkt); // init, Reset_AC_Alm_EMS_Pkt

	memset(paid->data, 0, DATA_ALM_ASCII_MAX); // 2019.09.03
	
	data_len = strlen(alm_str);
	if(data_len > DATA_ALM_ASCII_MAX) data_len = DATA_ALM_ASCII_MAX;
	memcpy(paid->data, alm_str, data_len);	
	//-----------------------	
	paid->AID[0] = 0x50;
	paid->AID[1] = 0x16;
	paid->type = __ASCII;
	paid->step = Step_No_Use;
	paid->reserved = 0;
	paid->is_alarm = alarm; // 0 : clr, 1 : alarm	
	paid->size = DATA_ALM_ASCII_MAX;
	paid->layer = 0xFF;		
	paid->carrier = 0xFF;
	paid->len = htons(7 + DATA_ALM_ASCII_MAX); // A_ID 및 Length 필드 길이 제외	
	len = (7 + DATA_ALM_ASCII_MAX+4); // 4 = aid(2) + len(2)
	//-----------------------	
	Make_ALM_EMS_SubData(pPkt, (INT8U*)paid, len);
	Make_ALM_EMS_SubData_End(pPkt);

	return len;
}


void Send_PRU_AC_Alarm_gREMS(INT8U ac_alarm)
{
	REMS_1024_Pkt* pPkt = &Reset_AC_Alm_EMS_Pkt;
	INT16U len = 0; 
	INT8U alarm;

	if(ac_alarm > 0) alarm = 1;
	else alarm = 0;

	m_St.NMS_Alm.A_0.AC = alarm;
	m_St.Old_Alm.A_0.AC = alarm;
	m_St.Last_Alm.A_0.AC = alarm;

#ifdef __Seperate_AlarmLog__From__AutoReport__	
	// Checking is needed even if gREMS.Not-Ready
#else	
	//-----------------------------
	if(m_Env.gREMS_Noti_Ready==0)	
		return;
	//----------------------------- 
#endif

#ifdef __Seperate_AlarmLog__From__AutoReport__
	len = Make_REMS_1024_Pkt_for_AC_Alarm(alarm, pPkt);
#else	
	INT8U data_len;
	char alm_str[100];
	ALM_AIDSTR_5G* paid = &Alm_Frame;
	
	Make_ALM_EMS_SubData_Init(pPkt); // init, Reset_AC_Alm_EMS_Pkt
	//-----------------------	
	memset(alm_str, 0, 100);
	Update_Alm_Str_Lmt(0x50, 0x16, 0, 0, alarm); // AC, PRU, 2020.01.29
	Make_Alm_Str(0x50, 0x16, 0x00, 0x00, alarm, alm_str); // 2020.02.17 : flag 추가: 항상 0 -> 'alarm'.

	memset(paid->data, 0, DATA_ALM_ASCII_MAX); // 2019.09.03
	
	data_len = strlen(alm_str);
	if(data_len > DATA_ALM_ASCII_MAX) data_len = DATA_ALM_ASCII_MAX;
	memcpy(paid->data, alm_str, data_len);	
	//-----------------------	
	paid->AID[0] = 0x50;
	paid->AID[1] = 0x16;
	paid->type = __ASCII;
	paid->step = Step_No_Use;
	paid->reserved = 0;
	paid->is_alarm = alarm; // 0 : clr, 1 : alarm	
	paid->size = DATA_ALM_ASCII_MAX;
	paid->layer = 0xFF;		
	paid->carrier = 0xFF;
	paid->len = htons(7 + DATA_ALM_ASCII_MAX); // A_ID 및 Length 필드 길이 제외	
	len = (7 + DATA_ALM_ASCII_MAX+4); // 4 = aid(2) + len(2)
	//-----------------------	
	Make_ALM_EMS_SubData(pPkt, (INT8U*)paid, len);
	Make_ALM_EMS_SubData_End(pPkt);	
#endif
	//-----------------------
	Insert_Auto_Rpt_Q( pPkt->data, len, CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );	
	Insert_Auto_Log_Q( pPkt->data, len, CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
}

void Send_PRU_AC_Alarm_gREMS_Only(INT8U alarm) // log not save.
{
	REMS_1024_Pkt* pPkt = &Reset_AC_Alm_EMS_Pkt;	
	INT16U len = 0;

	len = Make_REMS_1024_Pkt_for_AC_Alarm(alarm, pPkt);	
	Insert_Auto_Rpt_Q( pPkt->data, len, CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
}

//-------------------------------------------------------------------------------




/*
	 7 UDA_불요파	0x5440
	-6 UDA_PD(CPRI)	0x5443
	-5 UDA_Clock		0x5442
	 4 Reserved		UDA_Link
	 3 Reserved		
	-2 UDA_Clock_Reset Suppression	0x544A
	-1 UDA_CPRI_Reset Suppression	0x544B
	 0 Reserved		
*/
INT8U Is_UDA_Alm_Posi(INT8U byte, INT8U bit)
{		
	if(byte == 25)
	{
		switch(bit)
		{
			case 1:
			case 2:
			case 5:
			case 6:
				return 1;
			
			default: return 0;
		}
	}
	else return 0;
}

//-------------------------------------------------------------------------------
static ALM_AIDSTR_5G UDA_Alm_Frame;
void Check_UDA_Alarm_gREMS( REMS_1024_Pkt* pPkt ) // UDA_Alm_EMS_Pkt
{		
	INT8U f, i, k, bit_p;
	INT16U len = 0;
	
	////INT8U* pOld_Arm = (INT8U*)&m_St.AlmSt;	
	INT8U* pOld_Arm = (INT8U*)&m_St.Old_Alm; // 2020.04.09
	INT8U* pNMS_Arm = (INT8U*)&m_St.NMS_Alm;

	#ifdef __Seperate_AlarmLog__From__AutoReport__	
	// Checking is needed even if gREMS.Not-Ready
	#else
	//-----------------------------
	if(m_Env.gREMS_Noti_Ready==0)	
		return;
	//----------------------------- 
	#endif

	Make_ALM_EMS_SubData_Init(pPkt);	

	i = 25; // UDA 불요파 제외
	
	f = pOld_Arm[i] ^ pNMS_Arm[i];

	for( k=0; k < 8; k++ )
	{	
		if(Is_UDA_Alm_Posi(i, k)==0) continue;		
		if(RU_ARM_SUPP[i] & cbits[k]) continue;
		
		// if aid is 0xFFFF, then skip..
		if((Alm_AID[i][7-k][0] == 0xFF) && (Alm_AID[i][7-k][1] == 0xFF)) continue;

		#ifdef __TYPE__PRU__
		if((Alm_AID[i][7-k][2] & _pru_)==0) continue;
		#else			
		if((Alm_AID[i][7-k][2] & _aau_)==0) continue;
		#endif		

		bit_p = cbits[k];
		
		if( f & bit_p )
		{
			// 알람 이전 데이터 백업.
			calcBits (&pOld_Arm[i], (pNMS_Arm[i] & bit_p), k);
			
			len = Make_Arm_gREMS( &UDA_Alm_Frame, i, k, (pNMS_Arm[i] & bit_p));	

			if(len > 0) // 2019.11.30
			{
				if(Make_ALM_EMS_SubData( pPkt, (INT8U*)&UDA_Alm_Frame, len) == 0)
				{
					if(pPkt->l != 0xFFFF) k--; // retry 를 위해.
				}
			}
			else print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "0 = Make_Arm_gREMS, 0, %d-%d", i,k); 	
		}
	}	
	//-----------------------------------------------------------------------
	if( m_Eep.mask==0 )
	{		
		if(pPkt->aid_cnt > 0) 
		{
			Make_ALM_EMS_SubData_End(pPkt);

			for (i = 0; i < pPkt->tot; i++)
			{
				pPkt->cur = i+1;
				if(pPkt->size[i] > 0)
				{
					Insert_Auto_Rpt_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
					Insert_Auto_Log_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
				}
			}

			// 2019.12.18 : gREMS alarm 2회 발생관련 수정
			for (i = 0; i < pPkt->tot; i++)
				pPkt->size[i] = 0;
		}
	}
	//-----------------------------------------------------------------------
}


static ALM_AIDSTR_5G UDA_Alm_Frame_temp;

void Check_UDA_Alarm_gREMS___When_RuInit( INT8U* pAlarm ) // UDA_Alm_EMS_Pkt
{		
	INT8U  k;
	INT16U len = 0;
	INT8U  i = 25; // UDA 불요파 제외
	REMS_1024_Pkt* pPkt = &UDA_Alm_EMS_Pkt;
	
	Make_ALM_EMS_SubData_Init(pPkt);	
	
	for( k = 0; k < 8; k++ )
	{	
		if(Is_UDA_Alm_Posi(i, k)==0) continue;		
		if((Alm_AID[i][7-k][0] == 0xFF) && (Alm_AID[i][7-k][1] == 0xFF)) continue; 
		
	#ifdef __TYPE__PRU__
		if((Alm_AID[i][7-k][2] & _pru_)==0) continue;
	#else			
		if((Alm_AID[i][7-k][2] & _aau_)==0) continue;
	#endif
		
		if( pAlarm[i] & cbits[k] )
		{
			len = Make_Arm_gREMS( &UDA_Alm_Frame_temp, i, k, 1);	

			if(len > 0)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[RuInit_End] send UDA_alarm %d.%d", i, k);
				if(Make_ALM_EMS_SubData( pPkt, (INT8U*)&UDA_Alm_Frame_temp, len) == 0)
				{
					if(pPkt->l != 0xFFFF) k--; // retry 를 위해.
				}
			}
		}
	}	
	//-----------------------------------------------------------------------
	if( m_Eep.mask==0 )
	{		
		if(pPkt->aid_cnt > 0) 
		{
			Make_ALM_EMS_SubData_End(pPkt);

			for (i = 0; i < pPkt->tot; i++)
			{
				pPkt->cur = i+1;
				if(pPkt->size[i] > 0)
				{
					Insert_Auto_Rpt_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );									
				}
			}
			for (i = 0; i < pPkt->tot; i++)
				pPkt->size[i] = 0;
		}
	}
	//-----------------------------------------------------------------------
}

//-------------------------------------------------------------------------------
void Check_Alarm_gREMS( REMS_1024_Pkt* pPkt ) // Alm_EMS_Pkt
{	
	INT8U f, i, k;
	INT16U len = 0;
	INT8U* pOld_Arm = (INT8U*)&m_St.Old_Alm;
	INT8U* pNMS_Arm = (INT8U*)&m_St.NMS_Alm;
	INT8U* pChk_Arm;
	ALM_ST AlmChk; 
	//INT8U is_alarm, layer;
	static INT8U user_ctrl_prev[AMPU_4_MAX]; // AID 0x5116 
	static INT8U init = 0;
	
	if(init == 0)
	{
		init = 1;
		memcpy(user_ctrl_prev, m_Eep.Pwr_Amp_En, AMPU_4_MAX); // AID 0x5116 
	}
		
	Make_ALM_EMS_SubData_Init(pPkt); // init
	
	pChk_Arm = (INT8U*)&AlmChk;
	memset(pChk_Arm, 0, sizeof(ALM_ST));

	for( i=0; i<RU_ALARM_CNT; i++ )
	{
		f = pOld_Arm[i] ^ pNMS_Arm[i];

		for( k=0; k < 8; k++ )
		{			
		#ifdef __TYPE__PRU__
			if((Alm_AID[i][7-k][2] & _pru_)==0) continue;

			// 2020.01.29 : byte.0.bit.3, (AC Fail, PRU Only)
			if((i == 0)&&(k == 3)) continue; 
		#else			
			if((Alm_AID[i][7-k][2] & _aau_)==0) continue;
		#endif

			// alarm suppression 상태 : skip
			if( RU_ARM_SUPP[i] & cbits[k] ) continue;

			// if aid is 0xFFFF, then skip..
			if((Alm_AID[i][7-k][0] == 0xFF) && (Alm_AID[i][7-k][1] == 0xFF)) 
				continue;
				
			//---------------------------------
			if( f & cbits[k] )
			{
				// 알람 이전 데이터 백업.
				ctrlBits(&pOld_Arm[i], (pNMS_Arm[i] & cbits[k]), k);
				ctrlBits(&pChk_Arm[i], 1, k);				
			
				len = Make_Arm_gREMS( &Alm_Frame, i, k, (pNMS_Arm[i] & cbits[k]));
				
				if(len > 0) // 2019.11.30				
				{				
					if(Make_ALM_EMS_SubData(pPkt, (INT8U*)&Alm_Frame, len) == 0) // fail
					{
						if(pPkt->l != 0xFFFF) k--; // retry 를 위해.
					}
				}
				else print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "0 = Make_Arm_gREMS, 1,  %d-%d", i,k);	

				//D_Printf( DEBUG_REMS, VT_Color_Evt "AID 0x%02X%02X, cur %d, tot %d, l %d,  len %d", 
				//	Alm_AID[i][7-k][0], Alm_AID[i][7-k][1], 
				//	pPkt->cur,
				//	pPkt->tot,
				//	pPkt->l, len);			
			}
			//---------------------------------
		}
	}	
	//---------------------------------
	if( m_Eep.mask==0 )
	{		
		if(pPkt->aid_cnt > 0) 
		{
			Make_ALM_EMS_SubData_End(pPkt);

			if(AlmChk.ETC.SuperCap_Exch)
			{
				if(pPkt->tot > 0)
				{
					Set__alm_que_idx_SuperCapChange();
				}
			}

			for (i = 0; i < pPkt->tot; i++)
			{
				pPkt->cur = i+1;
				if(pPkt->size[i] > 0)
				{
					Insert_Auto_Rpt_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
					Insert_Auto_Log_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );
				}
			}

			// 2019.12.18 : gREMS alarm 2회 발생관련 수정
			for (i = 0; i < pPkt->tot; i++)
				pPkt->size[i] = 0;
		}	
	}
}


void Check_Alarm_gREMS_______When_RuInit( REMS_1024_Pkt* pPkt, INT8U* pAlarm ) // Alm_EMS_Pkt
{	
	INT8U  i, k;
	INT16U len = 0;
			
	Make_ALM_EMS_SubData_Init(pPkt); // init
	
	for( i=0; i<RU_ALARM_CNT; i++ )
	{
		for( k=0; k < 8; k++ )
		{
			if((Alm_AID[i][7-k][0] == 0xFF) && (Alm_AID[i][7-k][1] == 0xFF)) 
				continue;
		#ifdef __TYPE__PRU__
			if((Alm_AID[i][7-k][2] & _pru_)==0) continue;			
			if((i == 0)&&(k == 3)) continue; // byte.0.bit.3, (AC Fail, PRU Only)
		#else			
			if((Alm_AID[i][7-k][2] & _aau_)==0) continue;
		#endif			
				
			//---------------------------------
			if( pAlarm[i] & cbits[k] )
			{			
				len = Make_Arm_gREMS( &Alm_Frame, i, k, 1); // alarm only				
				
				if(len > 0) // 2019.11.30				
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[RuInit_End] send alarm_%d.%d", i, k);
					if(Make_ALM_EMS_SubData(pPkt, (INT8U*)&Alm_Frame, len) == 0) // fail
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RuInit_End] send alarm_%d.%d, fail.", i, k);
						if(pPkt->l != 0xFFFF) k--; // retry 를 위해.
					}
				}
			}
			//---------------------------------
		}
	}	
	//---------------------------------
	if( m_Eep.mask==0 )
	{		
		if(pPkt->aid_cnt > 0) 
		{
			Make_ALM_EMS_SubData_End(pPkt);			

			for (i = 0; i < pPkt->tot; i++)
			{
				pPkt->cur = i+1;
				if(pPkt->size[i] > 0)
				{
					Insert_Auto_Rpt_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_ALARM_LOC_RPT, pPkt->cur, pPkt->tot );					
				}
			}
			for (i = 0; i < pPkt->tot; i++)
				pPkt->size[i] = 0;
		}	
	}
}



//-----------------------------------------------------------------------------------------
void Get_gREMS_Alarm_Loc(INT8U byte, INT8U bit, INT8U* pLayer, INT8U* pCarrier)
{
	INT8U layer = 0xFF; // layer
	INT8U carrier = 0xFF; // carrier
	INT8U sub;

	if(byte == 2)
	{	
		     if((bit==7)||(bit==3)) { layer = 0; carrier = _100M; }
		else if((bit==6)||(bit==2)) { layer = 0; carrier = Alpha; }
		else if((bit==5)||(bit==1)) { layer = 1; carrier = _100M; }
		else                        { layer = 1; carrier = Alpha; }
	}
	else if(byte == 3)
	{	
		switch(bit)
		{
			case 7:
			case 3: 
			case 1:	layer = 0; carrier = _100M; break;

			case 6:
			case 2: 
			case 0: layer = 0; carrier = Alpha; break;

			case 5: layer = 1; carrier = _100M; break;
			case 4: layer = 1; carrier = Alpha; break;
		}	
	}
	else if(byte == 4)
	{	
		switch(bit)
		{
			case 3: layer = 0; carrier = _100M; break;			
			case 2: layer = 0; carrier = Alpha; break;
			
			case 1:	
			case 5: layer = 1; carrier = _100M; break;
			
			case 0: 
			case 4: layer = 1; carrier = Alpha; break;
		}		
	}
	else if((byte >= 5)&&(byte <= 24))
	{		
		layer = (byte - 5)/5; // 0 ~ 3
		sub = (byte - 5)%5; // 0 ~ 4
		
		if(sub == 0) // byte ( 5, 10, 15, 20)
		{
			     if(bit==7) carrier = _100M;
			else if(bit==6) carrier = Alpha;
			else if(bit==3) carrier = _100M;
			else if(bit==2) carrier = Alpha;			
		}
	}

	*pLayer = layer;
	*pCarrier = carrier;
}


/*
-------------------------------------------------------------------
gREMS Alarm Report를 위한, 
Alarm AID 단위의 구조체를 만들어 ALM_AIDSTR_5G 에 정리.
전체 사이즈를 리턴 
-------------------------------------------------------------------
*/
enum
{
	Batt_Normal = 0,
	Batt_Fault_Charge,   // 1: Battery Fault(충전차단)
	Batt_Fault_DisCharge,// 2: Battery Fault(방전차단)
	Batt_Fault_Voltage,  // 3: Battery Fault(Battery 전압 이상)
	Batt_Fault_HW,       // 4: Battery Fault(HW)
};

/*
----------------------------------------------------------------------------------
2019.11.27:
	Get CPRI_Num 01/2/3 from 100M/Alpha, up/down
	0 : Upper
	1 : Lower
----------------------------------------------------------------------------------
*/
INT8U Get__CPRI_Num(INT8U carrier, INT8U updn)
{	
	if(carrier == _100M)
	{
		if(updn == 0) return CPRI_0;
		else          return CPRI_2; 
	}
	else
	{
		if(updn == 0) return CPRI_1;
		else          return CPRI_3;
	}
}


/*
----------------------------------------------------------------------------------
2019.11.27:
	Alarm 발생/히제 시, Limit 및 발생/히제 당시의 상태값 저장을 위해.
----------------------------------------------------------------------------------
*/

//---------------------------------------------------------------
#define ALM_LMT_STR_MAX (32)
typedef struct __attribute__((packed))
{
	char str[ALM_LMT_STR_MAX];
}ALM_LMT_STR;

enum
{
	Norm = 0,
	Alrm = 1,
	Norm_Alrm
};

static ALM_LMT_STR alm_limit_str____PD_Upper[CPRI_PATH][Norm_Alrm]; // AID 0x530C, PD Power Upper
static ALM_LMT_STR alm_limit_str____PD_Lower[CPRI_PATH][Norm_Alrm]; // AID 0x5309, PD Power Lower


static ALM_LMT_STR alm_limit_str_____Los_Sfp[CPRI_PATH][Norm_Alrm]; // AID 0x5302, LOS(SFP) / PD Power(CPRI)(0x5308)
static ALM_LMT_STR alm_limit_str______LD_Sfp[CPRI_PATH][Norm_Alrm]; // AID 0x5304, LD_(SFP) / LD Power(CPRI)(0x5305)

static ALM_LMT_STR alm_limit_str__DL_Tssi_Up[MAX_Optic][MAX_DL][Norm_Alrm]; // AID 0x5101 : DL_TSSI Upper / DL TSSI(0x5100), DL TSSI 상한(0x5101)
static ALM_LMT_STR alm_limit_str_DL_Tssi_Low[MAX_Optic][MAX_DL][Norm_Alrm]; // AID 0x5102 : DL_TSSI Lower / DL TSSI(0x5100), DL TSSI 하한(0x5102)

static ALM_LMT_STR alm_limit_str__DL_Out__Up[MAX_DL][Norm_Alrm]; // AID 0x5104 : DL 출력 상한 / DL 출력(0x5103), DL 출력 상한(0x5104)
static ALM_LMT_STR alm_limit_str__DL_Out_Low[MAX_DL][Norm_Alrm]; // AID 0x5105 : DL 출력 하한 / DL 출력(0x5103), DL 출력 하한(0x5105)
static ALM_LMT_STR alm_limit_str__AmpDisable[MAX_DL][Norm_Alrm]; // AID 0x5117 : Amp Disabled

static ALM_LMT_STR alm_limit_str__Temp_Upper[Norm_Alrm]; // AID 0x5013 : 온도상한/ 함체 온도(0x5012), 함체 온도 상한(0x5013)
static ALM_LMT_STR alm_limit_str__________AC[Norm_Alrm]; // AID 0x5016 : AC      / 전압(PSU)(AC) 0xB603
static ALM_LMT_STR alm_limit_str__________DC[Norm_Alrm]; // AID 0x5017 : DC      / 전압(PSU)(Main DC)(0xB604), 전압(PSU)(Sub DC)(0xB606)

static ALM_LMT_STR alm_limit_str__UL_Over_IN[MAX_DL][Norm_Alrm]; // AID 0x5210 : UL Over in

//---------------------------------------------------------------

void Init__Alm_Str_Lmt(void)
{
	INT8U cpri;
	INT8U optic, dl, idx;

	for(cpri = 0; cpri < CPRI_PATH; cpri++)
	{
		for(idx = 0; idx < Norm_Alrm; idx++)
		{
			memset(alm_limit_str_____Los_Sfp[cpri][idx].str, 0, ALM_LMT_STR_MAX); 
			memset(alm_limit_str______LD_Sfp[cpri][idx].str, 0, ALM_LMT_STR_MAX); 

			memset(alm_limit_str____PD_Upper[cpri][idx].str, 0, ALM_LMT_STR_MAX); 
			memset(alm_limit_str____PD_Lower[cpri][idx].str, 0, ALM_LMT_STR_MAX); 
		}
	}

	

	for(optic = 0; optic < MAX_Optic; optic++)
	{
		for(dl = 0; dl < MAX_DL; dl++)
		{
			for(idx = 0; idx < Norm_Alrm; idx++)
			{
				memset(alm_limit_str__DL_Tssi_Up[optic][dl][idx].str, 0, ALM_LMT_STR_MAX);
				memset(alm_limit_str_DL_Tssi_Low[optic][dl][idx].str, 0, ALM_LMT_STR_MAX);
			}
		}
	}

	for(dl = 0; dl < MAX_DL; dl++)
	{
		for(idx = 0; idx < Norm_Alrm; idx++)
		{
			memset(alm_limit_str__DL_Out__Up[dl][idx].str, 0, ALM_LMT_STR_MAX);
			memset(alm_limit_str__DL_Out_Low[dl][idx].str, 0, ALM_LMT_STR_MAX);
			memset(alm_limit_str__AmpDisable[dl][idx].str, 0, ALM_LMT_STR_MAX);
			memset(alm_limit_str__UL_Over_IN[dl][idx].str, 0, ALM_LMT_STR_MAX);
		}
	}

	for(idx = 0; idx < Norm_Alrm; idx++)
	{
		memset(alm_limit_str__Temp_Upper[idx].str, 0, ALM_LMT_STR_MAX); 
		memset(alm_limit_str__________AC[idx].str, 0, ALM_LMT_STR_MAX); 
		memset(alm_limit_str__________DC[idx].str, 0, ALM_LMT_STR_MAX); 
	}
}


int Make_Alm_Str_Lmt( INT8U aid0, INT8U aid1, INT8U layer, INT8U carrier, INT8U alarm, char* alm_info_str)	
{	
	INT8U cpri;
	ALM_LMT_STR* pAlm_Lmt_Str;
	int re = RET_OK; // init with OK(1)
	INT8U alm;
	INT8U reason;

	alm = (alarm > 0)? 1:0;

	
	switch(aid0)
	{			
		case 0x50:
			switch( aid1 )
			{
				case 0x13: // 함체 온도 상한 
					pAlm_Lmt_Str = &alm_limit_str__Temp_Upper[alm];
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;
					
				#ifdef __TYPE__PRU__
				case 0x16: // AC 
					pAlm_Lmt_Str = &alm_limit_str__________AC[alm];
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX); 

					print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Make_Alm_Str_Lmt, AC Alarm(%d), %s]", 
								alm, alm_info_str);						
					break;
				#endif	
			
				case 0x17:  // DC					
					pAlm_Lmt_Str = &alm_limit_str__________DC[alm];
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX); 
					break;
					
				default: re = RET_None; break;
			}
			break;
		//---------------------------------------------------	
		case 0x51:
			switch( aid1 )
			{
				case 0x01: // DL TSSI 상한
					//------------------------------
					if(carrier >= MAX_Optic) return RET_FAIL; // do not report
					if(layer >= MAX_DL) return RET_FAIL; // do not report
					//-------------------------------- 
					pAlm_Lmt_Str = &alm_limit_str__DL_Tssi_Up[carrier][layer][alm];					
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX); 
					break;					
					
				case 0x02: // DL TSSI 하한
					//------------------------------
					if(carrier >= MAX_Optic) return RET_FAIL; // do not report
					if(layer >= MAX_DL) return RET_FAIL; // do not report
					//--------------------------------
					pAlm_Lmt_Str = &alm_limit_str_DL_Tssi_Low[carrier][layer][alm];					
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;

				case 0x04: // DL 출력 상한
					//------------------------------					
					if(layer >= MAX_DL) return RET_FAIL; // do not report
					//--------------------------------
					pAlm_Lmt_Str = &alm_limit_str__DL_Out__Up[layer][alm];
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;
				
				case 0x05: // DL 출력 하한
					//------------------------------
					if(layer >= MAX_DL) return RET_FAIL; // do not report
					//--------------------------------
					pAlm_Lmt_Str = &alm_limit_str__DL_Out_Low[layer][alm];
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;

				case 0x15: // Amp Shutdown
					//------------------------------
					if(layer >= MAX_DL) 
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "0x15: // Shutdown(DL) : %d", layer);	
						return RET_FAIL; // do not report
					}
					//--------------------------------
					reason = Get__Reason_SD_5115(layer);
					if(reason == Rsn_SD_UL_OverIn_Skip) // do not report
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Report Skip] Rsn_SD_UL_OverIn : %d", layer);
						return RET_FAIL; // do not report
					}					
					else if(reason == 0)// 2020.02.18 added
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "Reason_SD__5115_prev[%d] = 0", layer);						
						return RET_FAIL; // do not report
					}
					else 
					{
						strcpy(alm_info_str, Get__Str_SD_5115(reason)); // 2020.02.03
					}
						
					////memcpy(alm_info_str, Get__Str_SD_5115(reason), DATA_ALM_ASCII_MAX);

					if(alm==0)
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Clr__Reason_SD_5115(%d) Report Normal", layer);
						Clr__Reason_SD_5115(layer);
					}
					break;	

				case 0x17: // Amp Disabled
					//------------------------------
					if(layer >= MAX_DL) 
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "0x17: // Disabled(DL) : %d", layer);
						return RET_FAIL; // do not report
					}
					//--------------------------------
					//pAlm_Lmt_Str = &alm_limit_str__AmpDisable[layer][alm];
					//memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);

					reason = Get__Reason_Dis_5117(layer);
					strcpy(alm_info_str, Get__Str_Dis_5117(reason)); // 2020.02.03
					////memcpy(alm_info_str, Get__Str_Dis_5117(reason), DATA_ALM_ASCII_MAX);

					if(alm==0)
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Clr__Reason_Dis_5117(%d) Report Normal", layer);
						Clr__Reason_Dis_5117(layer);
					}
					break;		
				
				default: re = RET_None; break;
			}
			break;		
		//---------------------------------------------------
		case 0x52: 
			switch( aid1 )
			{
				case 0x10: // UL 과입력, 2019.12.26
					//------------------------------
					if(layer >= MAX_DL) return RET_FAIL; // do not report
					//--------------------------------
					pAlm_Lmt_Str = &alm_limit_str__UL_Over_IN[layer][alm];
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;
				
				default: re = RET_None; break;
			}
			break;
		//---------------------------------------------------
		case 0x53:
			switch( aid1 )
			{
				case 0x02: // LOS(CPRI)LOS(SFP)
				case 0x04: // LD(CPRI)LD(SFP)
					cpri = Get__CPRI_Num(carrier, layer);
					
					if(aid1 == 0x02) pAlm_Lmt_Str = &alm_limit_str_____Los_Sfp[cpri][alm];
					else             pAlm_Lmt_Str = &alm_limit_str______LD_Sfp[cpri][alm];
					
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;

				case 0x0C: // PD 상한
				case 0x09: // PD 하한
					cpri = Get__CPRI_Num(carrier, layer);
					
					if(aid1 == 0x0C) pAlm_Lmt_Str = &alm_limit_str____PD_Upper[cpri][alm];
					else             pAlm_Lmt_Str = &alm_limit_str____PD_Lower[cpri][alm];
					
					memcpy(alm_info_str, pAlm_Lmt_Str->str, DATA_ALM_ASCII_MAX);
					break;
				
				default: re = RET_None; break;
			}
			break;
		//---------------------------------------------------
		default: re = RET_None; break;
		//---------------------------------------------------
	}
	return re;
}

/*
--------------------------------------------------------------
 2019.11.27 : alarm status 변경 시, 상태값 저장.
--------------------------------------------------------------

// Byte 6 --------------------------------------------------------
Overpower(DL)	,7	,0x5110 ,Reason : DL 출력, 0x5103
High Temp	    ,6	,0x5113	,Reason : 온도(PAU)(DL) 0xB200
VSWR(DL)  	    ,5	,0x5111	,Reason : VSWR(PAU)(DL) 0xB207
PAU DC(DL)	    ,2	,0x5114	,Reason : 전압(PAU)(DL) 0xB206
Dev.Fail	    ,1	,0x5112	,Reason : DL 출력, 0x5103
// Byte 7 --------------------------------------------------------
DPD	        ,7	,0x5321	,Reason : DPD(DL)(0x5321)
// Byte 8 --------------------------------------------------------
UL 과입력	,5	,0x5210	,Reason : UL RSSI(0x5200)
UL LNA Fail	,4	,0x5212	,Reason : UL RSSI(0x5200), PRU Only
//--------------------------------------------------------
*/







INT8U Update_Alm_Str_Lmt( INT8U aid0, INT8U aid1, INT8U layer, INT8U carrier, INT8U alm_f)
{	
	INT8U cpri;
	double temp_num1=0,temp_num2=0;
	INT32U uVal32;
	ALM_LMT_STR* pAlm_Lmt_Str;
	INT8U alm;


	alm = (alm_f > 0)? 1:0;
	
	switch(aid0)
	{			
		case 0x50:
			switch( aid1 )
			{
				case 0x13: // 함체 온도 상한
					pAlm_Lmt_Str = &alm_limit_str__Temp_Upper[alm];
					
					temp_num1 = (double)m_St.Temp_x10 /10.0;
					temp_num2 = (double)m_Eep.TempLmt_x10/10.0;
					sprintf(pAlm_Lmt_Str->str, "Temp:%.1f, Th:%.1f", temp_num1, temp_num2); // MAX 20byte(including sign '-')
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "%s, ALARM", pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "%s, Normal", pAlm_Lmt_Str->str);
					break;
				
				//case 0x14: // FAN1 ??
				//	temp_num1 = (double)m_St.Temp_x10 /10.0;
				//	temp_num2 = (double)m_Eep.TempLmt_x10/10.0;
				//	sprintf(pAlm_Lmt_Str->str, "Temp:%.1f, Th:%.1f", temp_num1, temp_num2); // MAX 20byte(including sign '-')
				//	break;

				#ifdef __TYPE__PRU__
				//case 0x15: // FAN2 ??
				//	temp_num1 = (double)m_PSU.Temp_x10/10.0;
				//	sprintf(pAlm_Lmt_Str->str, "Temp(PSU):%.1f", temp_num1); // MAX 20byte(including sign '-')
				//	break;

				case 0x16: // AC
					pAlm_Lmt_Str = &alm_limit_str__________AC[alm];
					
					temp_num1 = (double)m_PSU.AC_In_Volt_x10/10.0;
					sprintf(pAlm_Lmt_Str->str, "Vol(PSU)(AC):%.1f", temp_num1); // MAX 20byte(including sign '-')
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "%s, ALARM", pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "%s, Normal", pAlm_Lmt_Str->str);
					break;
				#endif

				
				case 0x17: // DC
					pAlm_Lmt_Str = &alm_limit_str__________DC[alm];
					
				#ifdef __TYPE__PRU__
					temp_num1 = (double)m_PSU.DC_Out_Volt_x10[0]/10.0;
					temp_num2 = (double)m_PSU.DC_Out_Volt_x10[1]/10.0;
					sprintf(pAlm_Lmt_Str->str, "DC:%.1f, %.1f", temp_num1, temp_num2); // MAX 20byte(including sign '-')	
				#else // AAU
					temp_num1 = (double)m_St.d_Volt_DTU/10.0; // 2020.01.08
					sprintf(pAlm_Lmt_Str->str, "DC:%.1f", temp_num1); // MAX 20byte(including sign '-')		
				#endif
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "%s, ALARM", pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "%s, Normal", pAlm_Lmt_Str->str);
					break;
					
				default: break;
			}
			break;		
		//---------------------------------------------------
		case 0x51:
			switch( aid1 )
			{
				case 0x01: // DL TSSI 상한
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					if(carrier == _100M)
					{
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[_100M][layer]/10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_UP_x10[_100M][layer] /10.0;
					}
					else
					{							 
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[Alpha][layer]/10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_UP_x10[Alpha][layer] /10.0;
					}
					pAlm_Lmt_Str = &alm_limit_str__DL_Tssi_Up[carrier][layer][alm];
					
					sprintf(pAlm_Lmt_Str->str, "TSSI:%.1f, Th:%.1f", temp_num1, temp_num2);
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "DL_TSSI__Up.Optic_%d.Layer_%d:%s, ALARM", 
									carrier, layer, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "DL_TSSI__Up.Optic_%d.Layer_%d:%s, Normal", 
									carrier, layer, pAlm_Lmt_Str->str);
					break;
					
				case 0x02: // DL TSSI 하한
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					if(carrier == _100M)
					{
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[_100M][layer] /10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_DN_x10[_100M][layer]   /10.0;
					}
					else
					{
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[Alpha][layer] /10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_DN_x10[Alpha][layer]   /10.0;
					}
					pAlm_Lmt_Str = &alm_limit_str_DL_Tssi_Low[carrier][layer][alm];
					
					sprintf(pAlm_Lmt_Str->str, "TSSI:%.1f, Th:%.1f", temp_num1, temp_num2);
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "DL_TSSI_Low.Optic_%d.Layer_%d:%s, ALARM", 
									carrier, layer, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "DL_TSSI_Low.Optic_%d.Layer_%d:%s, Normal",
									carrier, layer, pAlm_Lmt_Str->str);
					break;		

				case 0x04: // DL 출력 상한
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.Fwd_Out_Pwr_x10[layer] /10.0;
					temp_num2 = (double)m_Eep.Fwd_Out_Lmt_x10[layer][HI] /10.0;

					pAlm_Lmt_Str = &alm_limit_str__DL_Out__Up[layer][alm];
					
					sprintf(pAlm_Lmt_Str->str, "DL:%.1f, Th:%.1f", temp_num1, temp_num2);
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "DL_Out__Up.Layer_%d:%s, ALARM", 
									layer, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "DL_Out__Up.Layer_%d:%s, Normal", 
									layer, pAlm_Lmt_Str->str);
					break;
				
				case 0x05: // DL 출력 하한
					//------------------------------
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.Fwd_Out_Pwr_x10[layer] /10.0;
					temp_num2 = (double)m_Eep.Fwd_Out_Lmt_x10[layer][LO] /10.0;

					pAlm_Lmt_Str = &alm_limit_str__DL_Out_Low[layer][alm];
					
					sprintf(pAlm_Lmt_Str->str, "DL:%.1f, Th:%.1f", temp_num1, temp_num2);
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "DL_Out_Low.Layer_%d:%s, ALARM", 
									layer, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "DL_Out_Low.Layer_%d:%s, Normal", 
									layer, pAlm_Lmt_Str->str);
					break;
				
				default: break;
			}
			break;
		//---------------------------------------------------
		case 0x52:
			switch( aid1 )
			{
				case 0x10: // UL Over In
					//------------------------------
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.d_UL_RSSI_dBm_x10[_100M][layer] /10.0;
					temp_num2 = (double)m_St.d_UL_RSSI_dBm_x10[Alpha][layer] /10.0;

					pAlm_Lmt_Str = &alm_limit_str__UL_Over_IN[layer][alm];
					
					sprintf(pAlm_Lmt_Str->str, "UL:%3.1f, %3.1f", temp_num1, temp_num2);
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "UL_Over_In.Layer_%d:%s, ALARM", 
									layer, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "UL_Over_In.Layer_%d:%s, Normal", 
									layer, pAlm_Lmt_Str->str);
					break;
						
				
				default: break;
			}
			break;
		//---------------------------------------------------
		case 0x53:
			switch( aid1 )
			{
				case 0x02: // LOS(CPRI)LOS(SFP)
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= 2) return 0; // fail
					//------------------------------
					cpri = Get__CPRI_Num(carrier, layer); // 100/A, 0(up)/1(dn)

					temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[cpri] /10.0;
					
					if(layer == 0) uVal32 = m_St.d_BIP_Count[DL][cpri];
					else		   uVal32 = m_St.d_BIP_Count[UL][cpri];
					
					pAlm_Lmt_Str = &alm_limit_str_____Los_Sfp[cpri][alm];
					
					sprintf(pAlm_Lmt_Str->str, "PD:%.1f,BIP:%07d", temp_num1, uVal32 );
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "LOS(SFP).CPRI_%d:%s, ALARM", 
									cpri, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "LOS(SFP).CPRI_%d:%s, Normal", 
									cpri, pAlm_Lmt_Str->str);
					break;
			
				case 0x04: // LD(CPRI)LD(SFP)
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					cpri = Get__CPRI_Num(carrier, layer);					
					//--------------------------------
					if(carrier == _100M)
					{
						if(layer == 0) temp_num1 = (double)m_St.d_SFP_LD_Pwr_x10[CPRI_0] /10.0;
						else		   temp_num1 = (double)m_St.d_SFP_LD_Pwr_x10[CPRI_2] /10.0;
					}
					else
					{
						if(layer == 0) temp_num1 = (double)m_St.d_SFP_LD_Pwr_x10[CPRI_1] /10.0;
						else		   temp_num1 = (double)m_St.d_SFP_LD_Pwr_x10[CPRI_3] /10.0;
					}
					pAlm_Lmt_Str = &alm_limit_str______LD_Sfp[cpri][alm];
					
					sprintf(pAlm_Lmt_Str->str, "LD:%.1f", temp_num1);
					
					if(alm)
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "LD_(SFP).CPRI_%d:%s, ALARM", 
									cpri, pAlm_Lmt_Str->str);
					else
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "LD_(SFP).CPRI_%d:%s, Normal", 
									cpri, pAlm_Lmt_Str->str);
					break;

				case 0x0C: // PD 상한, 2020.07.01
				case 0x09: // PD 하한
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= 2) return 0; // fail
					//--------------------------------
					cpri = Get__CPRI_Num(carrier, layer);

					if(carrier == _100M)
					{
						if(layer == 0) temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[CPRI_0] /10.0;
						else		   temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[CPRI_2] /10.0;
					}
					else
					{
						if(layer == 0) temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[CPRI_1] /10.0;
						else		   temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[CPRI_3] /10.0;
					}

					if(aid1 == 0x0C) 
					{
						pAlm_Lmt_Str = &alm_limit_str____PD_Upper[cpri][alm];
						sprintf(pAlm_Lmt_Str->str, "PD:%.1f,TH:%.1f", temp_num1, (double)m_Eep.PD_Pwr_Lmt_x10[_UP_][cpri] /10.0);

						if(alm) print_agent(DEBUG_SYSTEM, VT_BOLD_RED  , "PD_Upper.CPRI_%d:%s, ALARM",  cpri, pAlm_Lmt_Str->str);
						else	print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "PD_Upper.CPRI_%d:%s, Normal", cpri, pAlm_Lmt_Str->str);
					}
					else
					{
						pAlm_Lmt_Str = &alm_limit_str____PD_Lower[cpri][alm];
						sprintf(pAlm_Lmt_Str->str, "PD:%.1f,TH:%.1f", temp_num1, (double)m_Eep.PD_Pwr_Lmt_x10[_DN_][cpri] /10.0);

						if(alm)	print_agent(DEBUG_SYSTEM, VT_BOLD_RED  , "PD_Lower.CPRI_%d:%s, ALARM",  cpri, pAlm_Lmt_Str->str);
						else	print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "PD_Lower.CPRI_%d:%s, Normal", cpri, pAlm_Lmt_Str->str);
					}				
					break;	
				
				default: break;
			}
			break;
		//---------------------------------------------------
		default: 			
			break;
		//---------------------------------------------------
	}
	return 1;
}


/*
----------------------------------------------------------------------------------
	"alm_info_str" max size is 20 !!
	
	if "Make_Alm_Str" success return 1, 
	else return 0

	AID 0x5011 외에는 string에 alarm/normal의 정보는 없음.
----------------------------------------------------------------------------------
*/
INT8U Make_Alm_Str( INT8U aid0, INT8U aid1, INT8U layer, INT8U carrier, INT8U alm_f, char* alm_info_str)	
{	
	INT8U cpri;
	INT8U make_alm = 1; // default 1 (fail 0, success 1)
	double temp_num1=0,temp_num2=0;
	INT32U uVal32;
	int ret_alm_limit;

	memset(alm_info_str, 0, DATA_ALM_ASCII_MAX); // 2020.02.03

	/*
		return value of "Make_Alm_Str_Lmt()"
		------------------------------------
			1 (  OK) : alarm 당시의 String 사용.
			0 (fail) : fail
			-1(none) : 기존의 limit 없는 string 사용.
	*/
	ret_alm_limit = Make_Alm_Str_Lmt(aid0, aid1, layer, carrier, alm_f, alm_info_str);
	if(ret_alm_limit == RET_OK)	  return 1;
	if(ret_alm_limit == RET_FAIL) return 0;
	// IF ret_alm_limit == RET_None, do below
	//------------------------------------------------
	
	switch(aid0)
	{			
		case 0x50:
			switch( aid1 )
			{
				case 0x10: // 상태감시 장치접속
					sprintf(alm_info_str, "-");
					break;
					
				#ifdef __TYPE__PRU__
				case 0x11: // Door
					if(alm_f) sprintf(alm_info_str, "Open");
					else	  sprintf(alm_info_str, "Close");
					break;
				#endif

				//case 0x13: // 함체 온도 상한
				//	sprintf(alm_info_str, "Temp:%2d.%1d, Th:%2d.%1d",  // VV
				//		m_St.Temp_x10/10, m_St.Temp_x10%10, m_Eep.TempLmt_x10/10, m_Eep.TempLmt_x10%10);
				//	break;
				
				case 0x14: // FAN1
					sprintf(alm_info_str, "Temp:%2d.%1d, Th:%2d.%1d",  // VV
						m_St.Temp_x10/10, m_St.Temp_x10%10, m_Eep.TempLmt_x10/10, m_Eep.TempLmt_x10%10);
					break;

				#ifdef __TYPE__PRU__
				case 0x15: // FAN2
					sprintf(alm_info_str, "Temp(PSU):%2d.%1d", m_PSU.Temp_x10/10, m_PSU.Temp_x10%10); // VV
					break;

				//case 0x16: // AC
				//	sprintf(alm_info_str, "Voltage(AC):%3d.%1d", m_PSU.AC_In_Volt_x10/10, m_PSU.AC_In_Volt_x10%10); // VV
				//	break;
				#endif

				/*
				case 0x17: // DC
				#ifdef __TYPE__PRU__
					temp_num1 = (double)m_PSU.DC_Out_Volt_x10[0]/10.0;
					temp_num2 = (double)m_PSU.DC_Out_Volt_x10[1]/10.0;
					sprintf(alm_info_str, "DC:%.1f, %.1f", temp_num1, temp_num2); // MAX 20byte(including sign '-')	
				#else // AAU
					temp_num1 = (double)m_St.d_Volt_DTU/10.0; // 2020.01.08
					sprintf(alm_info_str, "DC:%.1f", temp_num1); // MAX 20byte(including sign '-')		
				#endif
					break;
				*/	
					
				#ifdef __TYPE__PRU__
				case 0x18: // Battery Low
					//temp_num1 = (double)m_PSU.Batt_Volt_x10/10.0;
					//sprintf(alm_info_str, "Battery DC: +%3.1f", temp_num1);
					sprintf(alm_info_str, "Battery DC: %2d.%1d", // VV
						m_PSU.Batt_Volt_x10/10, m_PSU.Batt_Volt_x10%10);
					break;

				case 0x1A: // Battery Status
					switch(m_St.Batt_Sts)
					{
						case Batt_Normal: 	        sprintf(alm_info_str, "-"); break;
						case Batt_Fault_Charge: 	sprintf(alm_info_str, "충전차단"); break;
						case Batt_Fault_DisCharge:	sprintf(alm_info_str, "방전차단"); break;
						case Batt_Fault_Voltage:	sprintf(alm_info_str, "전압이상"); break;
						case Batt_Fault_HW: 		sprintf(alm_info_str, "HW이상"); break;
						default : 	                sprintf(alm_info_str, "-"); break;
					}
					break;					
				#endif
				default: make_alm = 0; break;
			}
			break;
		//---------------------------------------------------	
		case 0x51:
			switch( aid1 )
			{
				case 0x01: // DL TSSI 상한
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					if(carrier == _100M)
					{
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[_100M][layer]/10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_UP_x10[_100M][layer] /10.0;
					}
					else
					{							 
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[Alpha][layer]/10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_UP_x10[Alpha][layer] /10.0;						
					}
					sprintf(alm_info_str, "TSSI:%3.1f, Th:%3.1f", temp_num1, temp_num2); // VVf
					break;
					
				case 0x02: // DL TSSI 하한
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					if(carrier == _100M)
					{
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[_100M][layer] /10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_DN_x10[_100M][layer]   /10.0;
					}
					else
					{
						temp_num1 = (double)m_St.d_DL_TSSI_dBm_x10[Alpha][layer] /10.0;
						temp_num2 = (double)m_Eep.DL_TSSI_DN_x10[Alpha][layer]   /10.0;
					}					
					sprintf(alm_info_str, "TSSI:%3.1f, Th:%3.1f", temp_num1, temp_num2); // VVf
					break;

				case 0x04: // DL 출력 상한
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.Fwd_Out_Pwr_x10[layer] /10.0;
					temp_num2 = (double)m_Eep.Fwd_Out_Lmt_x10[layer][HI] /10.0;

					sprintf(alm_info_str, "DL:%3.1f, Th:%3.1f", temp_num1, temp_num2); // VVf
					break;
				
				case 0x05: // DL 출력 하한
					//------------------------------
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.Fwd_Out_Pwr_x10[layer] /10.0;
					temp_num2 = (double)m_Eep.Fwd_Out_Lmt_x10[layer][LO] /10.0;

					sprintf(alm_info_str, "DL:%3.1f, Th:%3.1f", temp_num1, temp_num2); // VVf
					break;

				#ifdef __TYPE__PRU__
				case 0x10: // Overpower(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.Fwd_Out_Pwr_x10[layer] /10.0;
					temp_num2 = (double)m_Eep.Fwd_Out_Lmt_x10[layer][HI] /10.0;
					
					sprintf(alm_info_str, "DL:%3.1f, Th:%3.1f", temp_num1, temp_num2); // VVf
					break;
					
				case 0x11: // VSWR(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					// 2020.01.28 : use 'VSWR_Amp__On_x100' and 'VSWR_Amp_Off_x100'
					if(alm_f)  temp_num1 = (double)m_St.VSWR_Amp__On_x100[layer]/100.0;
					else       temp_num1 = (double)m_St.VSWR_Amp_Off_x100[layer]/100.0;
					sprintf(alm_info_str, "VSWR(PAU): %3.2f:1", temp_num1); // 2019.09.26 VVf
					break;

				case 0x12: // Device Fail(Low Gain)(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.Fwd_Out_Pwr_x10[layer] /10.0;
					temp_num2 = (double)m_Eep.Fwd_Out_Lmt_x10[layer][HI] /10.0;
					
					sprintf(alm_info_str, "DL:%3.1f, Th:%3.1f", temp_num1, temp_num2); // VVf
					break;
					
				case 0x13: // PAU High Temp(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.AMP_Temp_x10[layer] /10.0;
					sprintf(alm_info_str, "Temp(PAU):%3.1f", temp_num1); // VVf
					break;

				case 0x14: // PAU DC(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.AMP_DC_Volt_x10[layer] /10.0;
					sprintf(alm_info_str, "Vol(PAU)(DC):+%3.1f", temp_num1); // VVf
					break;
				#endif
				
				case 0x15: // Shutdown(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					     if(m_St.AlmSt.Path[layer].PAU.Over_Power) sprintf(alm_info_str, "Overpower");
					else if(m_St.AlmSt.Path[layer].PAU.High_Temp)  sprintf(alm_info_str, "HighTemp");
					else if(m_St.AlmSt.Path[layer].PAU.VSWR)       sprintf(alm_info_str, "VSWR");
					else if(m_St.AlmSt.Path[layer].PAU.DC_Fail)    sprintf(alm_info_str, "DC");					
					else if(m_St.AlmSt.A_4.Clock)                  sprintf(alm_info_str, "Clock");
					else
					{
						     if(m_St.AlmSt.A_2.CPRI_0__ && m_St.AlmSt.A_2.CPRI_0_a)       sprintf(alm_info_str, "CPRI");
						else if(m_St.AlmSt.A_2.CPRI_0__ && (IS_Alpha_Mhz_Existing()==0)) sprintf(alm_info_str, "CPRI");
						else sprintf(alm_info_str, "-");
					}
					break;	

				case 0x17: // Disabled(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail					
					else sprintf(alm_info_str, "User Control");
					//--------------------------------
					break;
					
				#ifdef __TYPE__PRU__
				case 0x18: // PAU Link(DL)
					//------------------------------
					if(layer >= MAX_DL) return 0; // fail
					else sprintf(alm_info_str, "-");
					//--------------------------------
					break;		
				#endif
				
				default: make_alm = 0; break;
			}
			break;

		//---------------------------------------------------	
		case 0x52:
			switch( aid1 )
			{
				case 0x02: // UL RSSI 하한					
					sprintf(alm_info_str, "-");
					break;

				case 0x10: // 과입력(UL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.d_UL_RSSI_dBm_x10[_100M][layer] /10.0;
					temp_num2 = (double)m_St.d_UL_RSSI_dBm_x10[Alpha][layer] /10.0;

					sprintf(alm_info_str, "UL:%3.1f, %3.1f", temp_num1, temp_num2);
					break;				

				#ifdef __TYPE__PRU__
				case 0x12: // LNA(UL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					temp_num1 = (double)m_St.d_UL_RSSI_dBm_x10[_100M][layer] /10.0;
					temp_num2 = (double)m_St.d_UL_RSSI_dBm_x10[Alpha][layer] /10.0;
					
					sprintf(alm_info_str, "UL:%3.1f, %3.1f", temp_num1, temp_num2);
					break;
				#endif

				#ifdef __TYPE__AAU__
				case 0x13 : // Gain 저하(UL)
					//------------------------------
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
						 if(layer == 0) sprintf(alm_info_str, "-");
					else if(layer == 1) sprintf(alm_info_str, "-");
					else if(layer == 2) sprintf(alm_info_str, "-");
					else				sprintf(alm_info_str, "-");
					break;
				#endif
				default: make_alm = 0; break;
			}
			break;	
		//---------------------------------------------------	
		case 0x53:
			switch( aid1 )
			{
				case 0x00: // Clock Fail, 2020.02.19 : bit 위치 수정.
					     if(m_St.d_Clock_Status & cbits[6])	sprintf(alm_info_str, "DCM");
					else if(m_St.d_Clock_Status & cbits[4])	sprintf(alm_info_str, "ADC/DAC");
					else if(m_St.d_Clock_Status & cbits[5])	sprintf(alm_info_str, "ADC/DAC JESD");
					else					                sprintf(alm_info_str, "정상");
					break;

				case 0x01: // CPRI
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= 2) return 0; // fail
					//--------------------------------
					cpri = Get__CPRI_Num(carrier, layer); // 100/A, up(0)/dn(1)
					
					     if(m_St.d_CPRI_Alarm[cpri] & cbits[4]) sprintf(alm_info_str, "CPRI Core Unlock");
					else if(m_St.d_CPRI_Alarm[cpri] & cbits[3])	sprintf(alm_info_str, "PD (SFP)");     
					else if(m_St.d_CPRI_Alarm[cpri] & cbits[2])	sprintf(alm_info_str, "LD (SFP");
					else if(m_St.d_CPRI_Alarm[cpri] & cbits[1])	sprintf(alm_info_str, "LOF (CPRI)");
					else if(m_St.d_CPRI_Alarm[cpri] & cbits[0])	sprintf(alm_info_str, "LOS (CPRI)");
					else                                        sprintf(alm_info_str, "정상");
					break;

				case 0x02: // LOS(CPRI)LOS(SFP)
					cpri = Get__CPRI_Num(carrier, layer); // 100/A, up(0)/dn(1)
					
					//----------------------------------					
					if(layer == 0) 
					{
						temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[cpri] /10.0;
						uVal32    = m_St.d_BIP_Count[DL][cpri];
					}
					else
					{
						temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[cpri] /10.0;
						uVal32    = m_St.d_BIP_Count[UL][cpri];
					}
					sprintf(alm_info_str, "PD:%3.1f,BIP:%07d", temp_num1, uVal32 ); // VVf
					break;

				case 0x0C: // PD 상한, 2020.07.01
				case 0x09: // PD 하한
					cpri = Get__CPRI_Num(carrier, layer); // 100/A, up(0)/dn(1)
					
					temp_num1 = (double)m_St.d_SFP_PD_Pwr_x10[cpri] /10.0;

					if(aid1 == 0x0C) 
						sprintf(alm_info_str, "PD:%3.1f,TH:%.1f", temp_num1, (double)m_Eep.PD_Pwr_Lmt_x10[_UP_][cpri] /10.0 ); // VVf
					else
						sprintf(alm_info_str, "PD:%3.1f,TH:%.1f", temp_num1, (double)m_Eep.PD_Pwr_Lmt_x10[_DN_][cpri] /10.0 ); // VVf
					break;

				case 0x03: // LOF(CPRI)
					sprintf(alm_info_str, "-");
					break;

				case 0x04: // LD(CPRI)LD(SFP)
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					if(layer >= 2) return 0; // fail
					//--------------------------------
					cpri = Get__CPRI_Num(carrier, layer); // 100/A, up(0)/dn(1)
					
					temp_num1 = (double)m_St.d_SFP_LD_Pwr_x10[cpri] /10.0;					
					sprintf(alm_info_str, "LD:%3.1f", temp_num1); // VVf
					break;
					
				case 0x15: // SFP 탈실장
					//------------------------------
					if(carrier >= MAX_Optic) return 0; // fail
					//--------------------------------
					if(carrier == _100M) sprintf(alm_info_str, "-");
					else				 sprintf(alm_info_str, "-");
					break;			
					
				#ifdef __TYPE__PRU__
				case 0x21: // DPD(DL)
					//------------------------------					
					if(layer >= MAX_DL) return 0; // fail
					//--------------------------------
					switch(m_St.DPD_Alarm_St[layer])
					{
						case 0x00: sprintf(alm_info_str, "Feedback Alarm"); break;
						case 0x01: sprintf(alm_info_str, "DPD Error Alarm"); break;
						case 0x08: sprintf(alm_info_str, "정상"); break;
						case 0x03: sprintf(alm_info_str, "etc"); break;
						default : sprintf(alm_info_str, "APD~"); break;
					}
					break;							
				#endif
				
				default: make_alm = 0; break;
			}
			break;
		//---------------------------------------------------	
		case 0x54:
			switch( aid1 )
			{
				/*
				case 0x40: // UDA_불요파
						 if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == 0x00)	sprintf(alm_info_str, "Payload Off");
					else if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == 0x01)	sprintf(alm_info_str, "정상");
					else if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == 0x02)	sprintf(alm_info_str, "Alarm만 보고");
					break;
					
				case 0x42: // UDA_Clock
						 if(m_Eep.UDA_Oper_Mode[UDA_Clock] == 0x00) 	sprintf(alm_info_str, "Reset");
					else if(m_Eep.UDA_Oper_Mode[UDA_Clock] == 0x01) 	sprintf(alm_info_str, "정상");
					else if(m_Eep.UDA_Oper_Mode[UDA_Clock] == 0x02) 	sprintf(alm_info_str, "Alarm만 보고");
					break;

				case 0x43: // UDA_PD(CPRI)
					     if(m_Eep.UDA_Oper_Mode[UDA_PD_CP] == 0x00)		sprintf(alm_info_str, "Reset");
					else if(m_Eep.UDA_Oper_Mode[UDA_PD_CP] == 0x01) 	sprintf(alm_info_str, "정상");
					else if(m_Eep.UDA_Oper_Mode[UDA_PD_CP] == 0x02) 	sprintf(alm_info_str, "Alarm만 보고");					
					break;
				*/	

				case 0x40: // UDA_불요파
						 if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == 0x00)	sprintf(alm_info_str, "SPURIOUS-PayloadOff"); // 2020.01.22
					else if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == 0x01)	sprintf(alm_info_str, "SPURIOUS-Clear"); // 2020.01.22
					else if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == 0x02)	sprintf(alm_info_str, "SPURIOUS-AlarmOnly"); // 2020.01.22
					break;
					
				case 0x42: // UDA_Clock
						 if(m_Eep.UDA_Oper_Mode[UDA_Clock] == 0x00) 	sprintf(alm_info_str, "CLOCK-Reset"); // 2020.01.22
					else if(m_Eep.UDA_Oper_Mode[UDA_Clock] == 0x01) 	sprintf(alm_info_str, "CLOCK-Clear"); // 2020.01.22
					else if(m_Eep.UDA_Oper_Mode[UDA_Clock] == 0x02) 	sprintf(alm_info_str, "CLOCK-AlarmOnly"); // 2020.01.22
					break;

				case 0x43: // UDA_PD(CPRI)
					     if(m_Eep.UDA_Oper_Mode[UDA_PD_CP] == 0x00)		sprintf(alm_info_str, "CPRI-Reset"); // 2020.01.22
					else if(m_Eep.UDA_Oper_Mode[UDA_PD_CP] == 0x01) 	sprintf(alm_info_str, "CPRI-Clear"); // 2020.01.22
					else if(m_Eep.UDA_Oper_Mode[UDA_PD_CP] == 0x02) 	sprintf(alm_info_str, "CPRI-AlarmOnly"); // 2020.01.22
					break;
					
				case 0x4A: // UDA_Clock_Reset Suppression
					strcpy(alm_info_str, "-");
					break;					
					
				case 0x4B: // UDA_CPRI_Reset Suppression
					strcpy(alm_info_str, "-");
					break;
					
				case 0x52: // SuperCap_Exch
					strcpy(alm_info_str, "-");
					break;

				default: make_alm = 0; break;
			}
			break;			
		//---------------------------------------------------
		case 0x56:
			switch( aid1 )
			{
				case 0x11: // mismatch_ID
					// 2019.08.29
					sprintf(alm_info_str, "ID: %d, CID: %d", 
						AufoConfigFlag.RU_Id, AufoConfigFlag.RE_Cascade_ID);
					break;
					
				default: make_alm = 0; break;
			}
			break;
		
		//---------------------------------------------------
		default: 
			make_alm = 0; 
			break;
		//---------------------------------------------------
	}
	return make_alm;
}

INT16U Make_Arm_gREMS( ALM_AIDSTR_5G* paid, INT8U byte, INT8U bit, INT8U alm_f )
{	
	INT8U aid0 = Alm_AID[byte][7-bit][0];
	INT8U aid1 = Alm_AID[byte][7-bit][1];
	INT8U make_alm = 1; // default 1
	char alm_info_str[100];
	INT8U data_len;
	INT8U layer, carrier;
		
	//-----------------------
	memset(alm_info_str, 0, 100); // init
	

	Get_gREMS_Alarm_Loc(byte, bit, &layer, &carrier);
	
	switch(aid0)
	{			
		case 0x50:
			switch( aid1 )
			{
				case 0x10: // 상태감시 장치접속				
				case 0x13: // 함체 온도 상한
				case 0x14: // FAN1
				case 0x17: // DC
				case 0x18: // Battery Low
				case 0x1A: // Battery Status					
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
					
				#ifdef __TYPE__PRU__
				case 0x11: // Door
				case 0x15: // FAN2
				case 0x16: // AC
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
				#endif
				
				default: make_alm = 0; break;
			}
			break;
		//---------------------------------------------------	
		case 0x51:
			switch( aid1 )
			{
				case 0x01: // DL TSSI 상한
				case 0x02: // DL TSSI 하한				
				case 0x04: // DL 출력 상한
				case 0x05: // DL 출력 하한
				case 0x15: // Shutdown(DL)
				case 0x17: // Disabled(DL)
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					//if(aid1 == 0x17)
					//	print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "0x5117: make_alm %d", make_alm);	
					break;
				

				#ifdef __TYPE__PRU__
				case 0x10: // Overpower(DL)					
				case 0x11: // VSWR(DL)
				case 0x12: // Device Fail(Low Gain)(DL)					
				case 0x13: // PAU High Temp(DL)
				case 0x14: // PAU DC(DL)
				case 0x18: // PAU Link(DL)
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
				#endif
				
				default: make_alm = 0; break;
			}
			break;

		//---------------------------------------------------	
		case 0x52:
			switch( aid1 )
			{
				case 0x02: // UL RSSI 하한
				case 0x10: // 과입력(UL)
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
					
				#ifdef __TYPE__PRU__
				case 0x12: // LNA(UL)
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
				#else	// AAU
				case 0x13 : // Gain 저하(UL)
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
				#endif
				
				default: make_alm = 0; break;
			}
			break;	
		//---------------------------------------------------
		case 0x53:
			switch( aid1 )
			{
				case 0x00: // Clock Fail
				case 0x01: // CPRI
				case 0x02: // LOS(CPRI)LOS(SFP)
				case 0x03: // LOF(CPRI)
				case 0x04: // LD(CPRI)LD(SFP)
				case 0x15: // SFP 탈실장
				case 0x0C: // PD Upper
				case 0x09: // PD Lower
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;
					
				#ifdef __TYPE__PRU__
				case 0x21: // DPD(DL)
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;							
				#endif
				
				default: make_alm = 0; break;
			}
			break;
		//---------------------------------------------------	
		case 0x54:
			switch( aid1 )
			{
				case 0x40: // UDA_불요파
				case 0x42: // UDA_Clock
				case 0x43: // UDA_PD(CPRI)					
				case 0x4A: // UDA_Clock_Reset Suppression
				case 0x4B: // UDA_CPRI_Reset Suppression
				case 0x52: // SuperCap_Exch
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;

				default: make_alm = 0; break;
			}
			break;
		//---------------------------------------------------	
		case 0x56:
			switch( aid1 )
			{
				case 0x11: // Mismatch_ID, 2019.08.21				
					make_alm = Make_Alm_Str(aid0, aid1, layer, carrier, alm_f, alm_info_str);
					break;

				default: make_alm = 0; break;
			}
			break;

			
		//---------------------------------------------------

		
		//---------------------------------------------------
		default: 
			make_alm = 0; 
			break;
		//---------------------------------------------------
	}	

	if (make_alm)
	{
		data_len = strlen(alm_info_str);
		if(data_len > DATA_ALM_ASCII_MAX) 
			data_len = DATA_ALM_ASCII_MAX;
		
		memset(paid->data, 0, DATA_ALM_ASCII_MAX); // 2019.09.03
		memcpy(paid->data, alm_info_str, data_len);		

		if(layer < 0xFF) paid->layer = layer+1;
		else		     paid->layer = layer;
		
		if(carrier < 0xFF) paid->carrier = carrier+1;
		else		       paid->carrier = carrier;
		//-----------------------	
		paid->AID[0] = aid0;
		paid->AID[1] = aid1;		
		paid->type = __ASCII;
		paid->step = Step_No_Use;
		paid->reserved = 0;
		paid->is_alarm = (alm_f)? 1 : 0; // 0 : clr, 1 : alarm	
		paid->size = DATA_ALM_ASCII_MAX;
		paid->len = htons(7 + DATA_ALM_ASCII_MAX); // A_ID 및 Length 필드 길이 제외
		//-----------------------	
		return (7 + DATA_ALM_ASCII_MAX+4); // 4 = aid(2) + len(2)
	}
	else return 0;
}
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------


//--------------------------------------------------------------------
static ALM_AIDSTR_5G Sup_Alm_Frame;
static REMS_1024_Pkt Sup_Alm_EMS_Pkt;
void Check_Suppress_Alm(void)
{
	INT16U len = 0;
	INT8U k, i;
	REMS_1024_Pkt* pPkt;

	//-----------------------------
	if(m_Eep.mask != 0)  
		return;
	#ifdef __Seperate_AlarmLog__From__AutoReport__	
	// Checking is needed even if gREMS.Not-Ready
	#else		
	if(m_Env.gREMS_Noti_Ready==0)	
		return;
	//----------------------------- 
	#endif

	//----------------------------------------------------- 
	pPkt = &Sup_Alm_EMS_Pkt;
	
	Make_ALM_EMS_SubData_Init(pPkt); // init, Sup_Alm_EMS_Pkt

	//-----------------------------------------------------
	for( i=0; i<RU_ALARM_CNT; i++ )
	{
		for( k=0; k<8; k++ )
		{
			// UDA Alarm 제외.
			if(Is_UDA_Alm_Posi(i, k)) continue;

			// if aid is 0xFFFF, then skip..
			if((Alm_AID[i][7-k][0] == 0xFF) && (Alm_AID[i][7-k][1] == 0xFF)) 
				continue;

			#ifdef __TYPE__PRU__
			if((Alm_AID[i][7-k][2] & _pru_)==0) continue;
			#else			
			if((Alm_AID[i][7-k][2] & _aau_)==0) continue;
			#endif	
			
			if( m_St.Supp_Arm_f[i] & cbits[k] ) // if flag set
			{
				//D_Printf( DEBUG_REMS, "[RCU] SUP_f set byte %d, bit %d", i, k);

				m_St.Supp_Arm_f[i] &= nbits[k]; // clr flag

				len = Make_Arm_gREMS( &Sup_Alm_Frame, i, k, (RU_ARM_SUPP[i] & cbits[k]));
				
				if(len > 0) // 2019.11.30
				{
					if(Make_ALM_EMS_SubData(pPkt, (INT8U*)&Sup_Alm_Frame, len) == 0) // if fail
					{
						if(pPkt->l != 0xFFFF) k--; // retry 를 위해.
					}
				}
				else print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "0 = Make_Arm_gREMS, 2,  %d-%d", i,k);	 
			}			
		}
	}	
	//-----------------------------------------------------	
	if(pPkt->aid_cnt > 0) 
	{
		Make_ALM_EMS_SubData_End(pPkt);
		//sprintf(dbgStr, "SUPP pPkt->len %d, cur %d, tot %d ", pPkt->len, pPkt->cur, pPkt->tot );
		
		for (i = 0; i < pPkt->tot; i++)
		{
			pPkt->cur = i+1;
			if(pPkt->size[i] > 0)
			{
				Insert_Auto_Rpt_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_SUPP_ALARM_RPT, pPkt->cur, pPkt->tot );
				Insert_Auto_Log_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_SUPP_ALARM_RPT, pPkt->cur, pPkt->tot );
			}
		}

		// 2019.12.18 : gREMS alarm 2회 발생관련 수정
		for (i = 0; i < pPkt->tot; i++)
			pPkt->size[i] = 0;
	}
}
//---------------------------------------------------------------------
/*
void Check_Suppress_Alm______When_RuInit( INT8U* pAlarm )
{
	INT16U len = 0;
	INT8U k, i;
	REMS_1024_Pkt* pPkt;

	//----------------------------------------------------- 
	pPkt = &Sup_Alm_EMS_Pkt;
	
	Make_ALM_EMS_SubData_Init(pPkt); // init, Sup_Alm_EMS_Pkt

	//-----------------------------------------------------
	for( i=0; i<RU_ALARM_CNT; i++ )
	{
		for( k=0; k<8; k++ )
		{
			// UDA Alarm 제외.
			if(Is_UDA_Alm_Posi(i, k)) continue;			
			if((Alm_AID[i][7-k][0] == 0xFF) && (Alm_AID[i][7-k][1] == 0xFF)) 
				continue;
		#ifdef __TYPE__PRU__
			if((Alm_AID[i][7-k][2] & _pru_)==0) continue;
		#else			
			if((Alm_AID[i][7-k][2] & _aau_)==0) continue;
		#endif	
			
			if( pAlarm[i] & cbits[k] ) // if flag set
			{
				len = Make_Arm_gREMS( &Sup_Alm_Frame, i, k, 1);
				
				if(len > 0)
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[RuInit_End] send Suprress %d.%d", i, k);
					if(Make_ALM_EMS_SubData(pPkt, (INT8U*)&Sup_Alm_Frame, len) == 0) // if fail
					{
						if(pPkt->l != 0xFFFF) k--; // retry 를 위해.
					}
				}
			}			
		}
	}	
	//-----------------------------------------------------	
	if(pPkt->aid_cnt > 0) 
	{
		Make_ALM_EMS_SubData_End(pPkt);
		//sprintf(dbgStr, "SUPP pPkt->len %d, cur %d, tot %d ", pPkt->len, pPkt->cur, pPkt->tot );
		
		for (i = 0; i < pPkt->tot; i++)
		{
			pPkt->cur = i+1;
			if(pPkt->size[i] > 0)
			{
				Insert_Auto_Rpt_Q( &pPkt->data[i*UDP_MAX_LEN], pPkt->size[i], CMD2_SUPP_ALARM_RPT, pPkt->cur, pPkt->tot );				
			}
		}		
		for (i = 0; i < pPkt->tot; i++)
			pPkt->size[i] = 0;
	}	
}
*/


//-----------------------------------------------------------------------------------------
void Proc__Sending_Init_Alarm__When_RuInit_End(void)
{
#ifdef __Send_Init_Alarm__When_0x500D_RuInit_is_6__
	static INT8U init = 0;	
	static INT8U gREMS_Noti_Ready;
	
	if(init == 0)
	{
		gREMS_Noti_Ready = m_Env.gREMS_Noti_Ready;
		init = 1;
	}

	//--------------------------------------
	// Check not-ready --> ready 
	//--------------------------------------
		
	INT8U Last_Alm[RU_ALARM_CNT];
	
	if(gREMS_Noti_Ready != m_Env.gREMS_Noti_Ready)
	{
		if( m_Eep.mask > 0 )
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Alarm When_RuInit_End] Alarm Mask Now.");
		}
		else
		{
			if(m_Env.gREMS_Noti_Ready) 
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Alarm When_RuInit_End]-----------");
				
				memcpy(Last_Alm, &m_St.Last_Alm, RU_ALARM_CNT); // init

				if(m_St.Last_Alm.A_0.AC)
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[RuInit_End] send AC alarm");
					Send_PRU_AC_Alarm_gREMS_Only(1);
				}
				Check_Alarm_gREMS_______When_RuInit( &Alm_EMS_Pkt, Last_Alm );
				Check_UDA_Alarm_gREMS___When_RuInit( Last_Alm );				
			}
		}
	}
	gREMS_Noti_Ready = m_Env.gREMS_Noti_Ready;
#endif	
}



//-----------------------------------------------------------------------------
// Algorithm
//
// CPU Reset 등으로 AMP off --> On 된 후, low alarm mask, 10초 후, low alarm mask 해제.
//



typedef struct
{	
	INT8U  is_timeout; // 1, if timeout
	INT8U  en;  // 1, if enabled
	INT8U  mask;
	INT8U  reserved;
	OS_TIMER timer;
} LOW_Alm_Mask_AMP_Init;
static LOW_Alm_Mask_AMP_Init m_LowAlm_Mask_Amp_Init[AMPU_4_MAX];


void Init_Low_Alm_Mask_at_AMP_Init(INT8U path4)
{	
	m_LowAlm_Mask_Amp_Init[path4].is_timeout = 0;
	m_LowAlm_Mask_Amp_Init[path4].en = 0;
	m_LowAlm_Mask_Amp_Init[path4].mask = 1;	
}

INT8U Is_Low_Alm_Mask_at_AMP_Init(INT8U path4)
{
	return m_LowAlm_Mask_Amp_Init[path4].mask;
}

void TimerStart_Low_Alm_at_AMP_Init(INT8U path4)
{	
	if( m_LowAlm_Mask_Amp_Init[path4].is_timeout ) return; // timer 1회만 check.
	
	if( m_LowAlm_Mask_Amp_Init[path4].en == 0 )
	{
		m_LowAlm_Mask_Amp_Init[path4].en = 1;
		m_LowAlm_Mask_Amp_Init[path4].mask = 1;
		OSTimerReset( &m_LowAlm_Mask_Amp_Init[path4].timer );
	}
}

void Proc_LowAlm_Mask_at_AMP_Init(void)
{
#if 0 // later @ 5G
	INT8U path4;

	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if( m_LowAlm_Mask_Amp_Init[path4].mask == 0 ) continue;
		if( m_LowAlm_Mask_Amp_Init[path4].is_timeout ) continue;
		if( m_LowAlm_Mask_Amp_Init[path4].en == 0 ) continue;
		
		if (OSTimerGet_msec(&m_LowAlm_Mask_Amp_Init[path4].timer) >= 10000) // 10[s]
		{
			 m_LowAlm_Mask_Amp_Init[path4].is_timeout = 1;
			 m_LowAlm_Mask_Amp_Init[path4].mask = 0;
		}
	}
#endif	
}

//-----------------------------------------------------------------------------
static OS_TIMER alarm_rpt_pause_timer;

// PD, DIG, Payload Shutdown에 의한 
// Fwd amp off alarm, low-out alarm, Rvs amp off alarm을 
// mask.
// 해제시에는 13초 정싱일 때, mask clear


// 10M_Ref alarm 일때, alarm 중, low alarm 항목만 mask 여부.
//



static OS_TIMER AlarmCheckTimer;

void Init_NMS_Alm_Proc(void)
{
	OSTimerReset( &AlarmCheckTimer );
	OSTimerReset( &NMS_Alarm_Timer );
	NMS_Alarm_En=0;
}

//-----------------------------------------------
void Start_Alm_Rpt_Pause_Timer(void)
{
	alarm_rpt_pause_req = 1;
	OSTimerReset( &alarm_rpt_pause_timer );
}





void Chk_UDA_Alm_gREMS( void )
{
	Check_UDA_Alarm_gREMS( &UDA_Alm_EMS_Pkt );
}

//----------------------------------------------------------------------------------------
INT8U Get_Alm_Conti_Max_Cnt(void)
{
#if 1 // later used when 5G-AAU/PRU
	return 1;
#else
	static INT8U ALM_CONTI_MAX_CNT = 10;
	static INT32U ALM_MAX_CNT_Chg_En = 0; // init 0
	static OS_TIMER ALM_MAX_CNT_Chg_Timer;
	//------------------------------------------------
	// 2016. Jan. : REMS Added
	// 2015.12.04 : CPU reset 1분간, 알람은 10초 유지, 1분 이후, 1초 유지.
	//------------------------------------------------
	
	if(ALM_CONTI_MAX_CNT == 10)
	{
		if(ALM_MAX_CNT_Chg_En == 0) // if not start
		{
			ALM_MAX_CNT_Chg_En = 1; // start
			OSTimerReset( &ALM_MAX_CNT_Chg_Timer );
		}
		else
		{
			if(OSTimerGet_msec(&ALM_MAX_CNT_Chg_Timer) >= (60*TICK_CO_SEC))
				ALM_CONTI_MAX_CNT = 1;
		}
	}	
	return ALM_CONTI_MAX_CNT;
#endif	
}





static INT8S  NMS_Arm_Co[RU_ALARM_CNT][8]; // x 8 bit

void Chk_n_Make_NMS_Alm(void)
{
//------------------------------------------------------------------
	INT8U i, j, bit, chg, arm;
	INT8U arm_before, cnt=0;	
	INT8U* pNMS_Arm = (INT8U*)&m_St.NMS_Alm;
	INT8U* pOld_Arm = (INT8U*)&m_St.Old_Alm;
	INT8U* pLast_Arm;	
	INT8U max_cnt = 1;		
#ifdef __2020_0107_Saving_AlarmLog_When_CPRI_Lock__ // AAU Only	
	INT8U pause = 0;
	INT8U bit_alm;	
#endif	
	ALM_ST Last_Alm_t; // 2019.12.03
	

	/*
	---------------------------------------------------------------------
	// 2019.12.04 : delete below
	---------------------------------------------------------------------
	*/
	pLast_Arm = (INT8U*)&Last_Alm_t;
	memcpy(&Last_Alm_t, &m_St.Last_Alm, sizeof(ALM_ST));

#ifdef __2020_0107_Saving_AlarmLog_When_CPRI_Lock__ // AAU
	// do nothing
#else // PRU
	#ifdef __Seperate_AlarmLog__From__AutoReport__	
	// Checking is needed even if gREMS.Not-Ready
	#else
	if(m_Env.gREMS_Noti_Ready==0)
	{
		// PRU
		// 2019.09.18 : gREMS Comm Start 시, Alarm Report 안하도록 다시 수정. 
		memcpy(&m_St.NMS_Alm, &Last_Alm_t, sizeof(ALM_ST));
		memcpy(&m_St.Old_Alm, &Last_Alm_t, sizeof(ALM_ST));		
		
		return;
	}
	#endif
#endif	
	//---------------------------------------------------------------------

	max_cnt = Get_Alm_Conti_Max_Cnt();	
	
	if( OSTimerGet_msec(&AlarmCheckTimer) < TICK_CO_SEC )  
		return;
	OSTimerReset( &AlarmCheckTimer );	
		
	for( i=0; i<RU_ALARM_CNT; i++ )
	{			
		chg = pNMS_Arm[i] ^ pLast_Arm[i];
		arm = pNMS_Arm[i];
		arm_before = arm;
		
		for( j=0; j<8; j++ )
		{			
		#ifdef __2020_0107_Saving_AlarmLog_When_CPRI_Lock__ // AAU Only
			if(m_Env.gREMS_Noti_Ready) // if ready then reporting
			{
				pause = 0; // reporting (pause=0)
			}
			else // not ready, pause reporting(but cpri-alarm is reporting)
			{
				pause = 1; // default skip 

				if(m_Env.RU_Init_St_Reset)
				{
					if((i == 2)&&((j >= 4)&&(j <= 7))) // CRPI Alarm
						pause = 0; // reporting (pause=0)
				}
			}

			if(ALM_JIG_MODE == ALM_JIG_MODE_CODE) pause = 0;	

			if(pause)
			{				
				NMS_Arm_Co[i][j] = 0; // init 0, 2020.02.20
				
				bit_alm = pLast_Arm[i] & cbits[j];

				// pause except for AID 0x5611(ID mismatch alarm)
				if((Alm_AID[i][7-j][0]!=0x56)&&(Alm_AID[i][7-j][1]!=0x11)) // except ID mismatch alarm
				{
					ctrlBits( &pNMS_Arm[i], bit_alm, j );
					ctrlBits( &pOld_Arm[i], bit_alm, j );
					ctrlBits( &arm        , bit_alm, j );
				}

				if(bit_alm == 0)
				{
					switch(i)
					{
						// byte 6,11,16,21
						case 6: 
						case 11:
						case 16:
						case 21:
							/*
							-----------------------------
							// 2020.04.02 : delete below
							//------------------------
							////path4 = (i-6)/5; // 0~3
							////if(j==4) {Clr__Reason_Dis_5117(path4); Clr__Reason_Dis_5117_Prev(path4);} // bit4
							////if(j==0) {Clr__Reason_SD_5115(path4); Clr__Reason_SD_5115_Prev(path4);} // bit0 
							------------
							'm_Env.gREMS_Noti_Ready = 0' 의 상태에서, 
							cpri alarm 은 발생했지만, 
							상위보고 하지는 않는 상태이므로, clear reason은 무의미 함.
							*/
							break;
					}
				}
				//--------------------
				continue;
				//--------------------
			}
		#endif
			//------------------------------
			// 2018.08.29 : ID_Mismatch Alarm
			if((Alm_AID[i][7-j][0]==0x56)&&(Alm_AID[i][7-j][1]==0x11)) 
			{
				if(IS__TimeOver_Id_Mismatch_Alm())
				{
					Clr__TimeOver_Id_Mismatch_Alm();
					print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Check_Alarm_gREMS] ID_Mismatch : i.%d.j.%d, chg.0x%X", 
								i, j, chg); // byte.26.bit.1
					print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Check_Alarm_gREMS] ID_Mismatch : Alm 0x%X(L), 0x%X(N), 0x%X(O), %d", 
								pLast_Arm[i], pNMS_Arm[i], pOld_Arm[i], m_St.AlmSt.ETC.Mismatch_ID);
					print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Check_Alarm_gREMS] ID_Mismatch : arm_before 0x%X", 
								arm_before);
				}
				else
				{
					NMS_Arm_Co[i][j]=0;
					continue;
				}
			}			
			//----------------------------
			bit = cbits[j];
			
			if( chg & bit )
			{
				NMS_Arm_Co[i][j]++;
				
				if(NMS_Arm_Co[i][j] >= max_cnt)
				{
					NMS_Arm_Co[i][j]=0;
					if(pLast_Arm[i] & bit) arm |= bit;
					else  				   arm &= (~bit);

					if((Alm_AID[i][7-j][0]==0x56)&&(Alm_AID[i][7-j][1]==0x11)) 
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Check_Alarm_gREMS] ID_Mismatch : Alm 0x%X(L), 0x%X(arm)", 
									pLast_Arm[i], arm);
					}
				}
			}
			else NMS_Arm_Co[i][j]=0;
		}
		pNMS_Arm[i] = arm;
		
		if(arm_before != arm) cnt++;
	}
	//-------------------------
	// check whether changed or not

	if(cnt) Check_Alarm_gREMS( &Alm_EMS_Pkt );

	Make_Suppress_Alarm();
	//-------------------------
#ifdef __2020_0107_Saving_AlarmLog_When_CPRI_Lock__
	if(m_Env.gREMS_Noti_Ready==0)
	{
		if(m_Env.RU_Init_St_Reset)
			Update_Only_Alm_Log('A');
	}
#endif

//--------------------------------------------------------------------------------------------- 			
}



//--------------------------------------------------------------------------------------------- 			
//------------------------------------------------------------------	




/* EOF */


