/////////////////////////////
// control.c
/////////////////////////////
#define CONTROL_5G_GLOBALS
#include "includes.h"
/////////////////////////////

#include <math.h>


/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

pthread_mutex_t	Ctrl_Sem;

void InitCtrl( void )
{
	if( pthread_mutex_init(&Ctrl_Sem, NULL) )
	{
		syslog( LOG_ERR, "Mutex initialization failed");
		exit(EXIT_FAILURE);
	}
}

void CtrlLock( void )
{
	pthread_mutex_lock(&Ctrl_Sem);
}

void CtrlUnLock( void )
{
	pthread_mutex_unlock(&Ctrl_Sem);
}

/*
---------------------------------------------------------------------------
PING Test
---------------------------------------------------------------------------
*/


INT8U Ping_Test( INT8U* pDst_IP )
{
	/*
	//char cmd[128];
	//sprintf( cmd, "arping -D -I eth0 -c 2 %d.%d.%d.%d", pDst_IP[0], pDst_IP[1], pDst_IP[2], pDst_IP[3]); 	
	//if( system( cmd ) ) return 1; // ping ok
	//else                return 0;
	*/	
	
	int re;
	
	re = system_agent("arping -D -I eth0 -c 2 %d.%d.%d.%d", pDst_IP[0], pDst_IP[1], pDst_IP[2], pDst_IP[3]);
	
	if(re == 0) return 1; // ping ok
	else        return 0;
}

INT8U Ping_Self_IP( INT8U* pSUBID )
{	
	INT8U Dst_IP[4];
	INT8U re;

	if(Get_SUB_ID_IP( pSUBID, Dst_IP ))
	{
		re = Ping_Test( Dst_IP );
		return re; 		
	}
	return 1;
}


void Proc____GUI_Test_Alarm(void)
{
	INT8U alm[RU_ALARM_CNT];
	INT8U flag[RU_ALARM_CNT];
	INT8U ctrl[RU_ALARM_CNT];
	INT8U path, i;

	if(m_St.AlmTest)
	{
		//-------------------------- 2020.02.19
		if(m_St.AlmFlag.A_0.Temp_Up)     Update_Alm_Str_Lmt( 0x50, 0x13, 0, 0, m_St.AlmCtrl.A_0.Temp_Up);
		if(m_St.AlmFlag.A_0.DC)	         Update_Alm_Str_Lmt( 0x50, 0x17, 0, 0, m_St.AlmCtrl.A_0.DC);
		//---------------------------------------- // LOS(CPRI)LOS(SFP)
		if(m_St.AlmFlag.A_2.LOS_SFP_0__) Update_Alm_Str_Lmt( 0x53, 0x02, 0, _100M, m_St.AlmCtrl.A_2.LOS_SFP_0__);		
		if(m_St.AlmFlag.A_2.LOS_SFP_0_a) Update_Alm_Str_Lmt( 0x53, 0x02, 0, Alpha, m_St.AlmCtrl.A_2.LOS_SFP_0_a);		
		if(m_St.AlmFlag.A_2.LOS_SFP_1__) Update_Alm_Str_Lmt( 0x53, 0x02, 1, _100M, m_St.AlmCtrl.A_2.LOS_SFP_1__);		
		if(m_St.AlmFlag.A_2.LOS_SFP_1_a) Update_Alm_Str_Lmt( 0x53, 0x02, 1, Alpha, m_St.AlmCtrl.A_2.LOS_SFP_1_a);
		//---------------------------------------- // LD(CPRI)LD(SFP)
		if(m_St.AlmFlag.A_3.LD_SFP_0__)  Update_Alm_Str_Lmt( 0x53, 0x04, 0, _100M, m_St.AlmCtrl.A_3.LD_SFP_0__);		
		if(m_St.AlmFlag.A_3.LD_SFP_0_a)  Update_Alm_Str_Lmt( 0x53, 0x04, 0, Alpha, m_St.AlmCtrl.A_3.LD_SFP_0_a);		
		if(m_St.AlmFlag.A_3.LD_SFP_1__)	 Update_Alm_Str_Lmt( 0x53, 0x04, 1, _100M, m_St.AlmCtrl.A_3.LD_SFP_1__);		
		if(m_St.AlmFlag.A_3.LD_SFP_1_a)	 Update_Alm_Str_Lmt( 0x53, 0x04, 1, Alpha, m_St.AlmCtrl.A_3.LD_SFP_1_a);

		// 2020.07.01
		if(m_St.AlmFlag.A_3.PD_Pwr_U_0__) Update_Alm_Str_Lmt( 0x53, 0x0C, 0, _100M, m_St.AlmCtrl.A_3.PD_Pwr_U_0__);
		if(m_St.AlmFlag.A_3.PD_Pwr_U_0_a) Update_Alm_Str_Lmt( 0x53, 0x0C, 0, Alpha, m_St.AlmCtrl.A_3.PD_Pwr_U_0_a);
		if(m_St.AlmFlag.A_4.PD_Pwr_U_1__) Update_Alm_Str_Lmt( 0x53, 0x0C, 1, _100M, m_St.AlmCtrl.A_4.PD_Pwr_U_1__);
		if(m_St.AlmFlag.A_4.PD_Pwr_U_1_a) Update_Alm_Str_Lmt( 0x53, 0x0C, 1, Alpha, m_St.AlmCtrl.A_4.PD_Pwr_U_1_a);

		// 2020.07.01
		if(m_St.AlmFlag.A_4.PD_Pwr_L_0__) Update_Alm_Str_Lmt( 0x53, 0x09, 0, _100M, m_St.AlmCtrl.A_4.PD_Pwr_L_0__);
		if(m_St.AlmFlag.A_4.PD_Pwr_L_0_a) Update_Alm_Str_Lmt( 0x53, 0x09, 0, Alpha, m_St.AlmCtrl.A_4.PD_Pwr_L_0_a);
		if(m_St.AlmFlag.A_4.PD_Pwr_L_1__) Update_Alm_Str_Lmt( 0x53, 0x09, 1, _100M, m_St.AlmCtrl.A_4.PD_Pwr_L_1__);
		if(m_St.AlmFlag.A_4.PD_Pwr_L_1_a) Update_Alm_Str_Lmt( 0x53, 0x09, 1, Alpha, m_St.AlmCtrl.A_4.PD_Pwr_L_1_a);
		
		for(path = DL_0; path < MAX_DL; path++) 
		{
			if(m_St.AlmFlag.Path[path].DL.TSSI_Upper__)	Update_Alm_Str_Lmt( 0x51, 0x01, path, _100M, m_St.AlmCtrl.Path[path].DL.TSSI_Upper__);
			if(m_St.AlmFlag.Path[path].DL.TSSI_Upper_a)	Update_Alm_Str_Lmt( 0x51, 0x01, path, Alpha, m_St.AlmCtrl.Path[path].DL.TSSI_Upper_a);		
			if(m_St.AlmFlag.Path[path].DL.TSSI_Lower__)	Update_Alm_Str_Lmt( 0x51, 0x02, path, _100M, m_St.AlmCtrl.Path[path].DL.TSSI_Lower__);		
			if(m_St.AlmFlag.Path[path].DL.TSSI_Lower_a)	Update_Alm_Str_Lmt( 0x51, 0x02, path, Alpha, m_St.AlmCtrl.Path[path].DL.TSSI_Lower_a);

			if(m_St.AlmFlag.Path[path].DL.Out_Upper)   Update_Alm_Str_Lmt( 0x51, 0x04, path, 0, m_St.AlmCtrl.Path[path].DL.Out_Upper);
			if(m_St.AlmFlag.Path[path].DL.Out_Lower)   Update_Alm_Str_Lmt( 0x51, 0x05, path, 0, m_St.AlmCtrl.Path[path].DL.Out_Lower);
			if(m_St.AlmFlag.Path[path].UL.Over_Input)  Update_Alm_Str_Lmt( 0x52, 0x10, path, 0, m_St.AlmCtrl.Path[path].UL.Over_Input);
		}

		if(m_St.AlmFlag.A_0.AC) Update_Alm_Str_Lmt(0x50, 0x16, 0, 0, m_St.AlmCtrl.A_0.AC);
		
		
		//------------------------------------------
		memcpy(alm,  &m_St.AlmSt,   RU_ALARM_CNT);
		memcpy(flag, &m_St.AlmFlag, RU_ALARM_CNT);
		memcpy(ctrl, &m_St.AlmCtrl, RU_ALARM_CNT);

		for(i=0; i < RU_ALARM_CNT; i++)
		{
			alm[i] &= (~flag[i]);
			ctrl[i] &= flag[i];
			alm[i] |= ctrl[i];
		}
		
		memcpy(&m_St.AlmSt, alm, RU_ALARM_CNT);
	}
}						


/*
---------------------------------------------------------------------------
Control AID
---------------------------------------------------------------------------
*/


/*
	Parsing AID2 sturcture
	----------------------------------------------
	- if(layer == 0xFF)   set layer = 1
	- if(carrier == 0xFF) set carrier = 1
	- Calculate flag size
	- Calculate data only size
	- Calculate aid2->len : big-endian -> host-byte-order
*/

int Parse_AID2_STR(AIDSTR2* paid2, AID2_INFO* paid2_info)
{	
	INT16U data_len;
	
	paid2_info->size    = AID2_Chk_Size(paid2->size);
	paid2_info->layer   = AID2_Chk_Layer(paid2->layer);
	paid2_info->carrier = AID2_Chk_Carrier(paid2->carrier);
	
	paid2_info->aid2_len = ntohs(paid2->len);
	paid2_info->tot__len = ntohs(paid2->len) + AID2_AID_LEN_Size;

	data_len = paid2_info->size * paid2_info->layer * paid2_info->carrier;
	paid2_info->data_len = data_len;
	paid2_info->flag_len = ntohs(paid2->len) - (data_len) - AID2__Type_Carrier_Size;	

	if(paid2_info->flag_len < 1)
		return RET_Error;		
	else
		return RET_SUCCESS;
}




#define AID_CTRL_DISP_BUF (512)
static char aid_ctrl_disp_data[AID_CTRL_DISP_BUF];
#define CTRL_BUFF_SIZE (50)

INT16U AID_Control( void* pAID, INT8U cmd )
{	
	AIDSTR*  paid  = (AIDSTR*)pAID;
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	AID2_INFO aid2_info;
	INT16U tot__len;
	INT16S len; // data_only_len (without flag)
	int re;


	INT8U flag_len; // default 1
	INT8U AID[2];	
	
	INT32U* pVal32 = NULL;
	INT16U* pVal16 = NULL;
	INT8U*  pVal8 = NULL;
	
	INT32U uVal32;	
	INT32S sVal32;
	INT16U uVal16;
	INT16S sVal16;
	INT8U  uVal8;

	INT8U  data[CTRL_BUFF_SIZE];
	INT8U  buf_size = CTRL_BUFF_SIZE;
	INT8U  c, l, i, amp, fa; // carrier, layer
	#ifdef __TYPE__PRU__
	INT8U  byte, bit;
	#else
	INT8S  att_x2;	
	INT16S val_x10;	
	INT8S  sVal8;
	#endif
	
	INT32U flag32 = 0;
	INT8S  flag_msb = 0; // flag_index

	
	INT32U cal_u32 = 0;
	//INT8U systime[6];

	AID[0] = paid->AID[0];
	AID[1] = paid->AID[1];

	if((AID[0] == 0xC0)&&(AID[1] == 0x36))
	{
		paid2->len = htons(0x0B);
	}
	
	memset(data, 0, CTRL_BUFF_SIZE);

	if(Is_AID2_Range(AID[0])) 
	{	
		re = Parse_AID2_STR(paid2, &aid2_info);
		if(re == RET_Error)
			return aid2_info.tot__len;	
		
		len = aid2_info.data_len; // data_only		
		tot__len = aid2_info.tot__len;		
		flag_len = aid2_info.flag_len;
			
		/*
		INT8U size,	layer, carrier;	
		INT16U aid2_len;    // size of "type ~ data"
		
		aid2_len = aid2_info.aid2_len;
		size     = aid2_info.size;
		layer    = aid2_info.layer;
		carrier  = aid2_info.carrier;
		*/

		for(flag32=0, i = 0; i < flag_len; i++)
		{
			flag32 <<= 8;
			flag32 |= paid2->data[i];
		}
		flag_msb = 7 + 8*(flag_len-1);
		//----------------------------------------
		pVal8  = &paid2->data[flag_len];

		//print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[ctrl] [1] pointer %p ", pVal8);
		
		pVal16 = (INT16U*)&paid2->data[flag_len];
		pVal32 = (INT32U*)&paid2->data[flag_len];
		//----------------------------------------
//		printf("_YCSO_ flag_len %x , paid2 %p , pVal8 %p \n",flag_len, paid2, pVal8);
//		fflush(stdout);

		Print_AID2("[AID_Control]", paid2, &aid2_info, aid_ctrl_disp_data, AID_CTRL_DISP_BUF);
		/*
		D_Printf(DEBUG_SYSTEM, "[AID_Control] flag32 0x%X, flag_msb %d\n", flag32, flag_msb);
		D_Printf(DEBUG_SYSTEM, "[AID_Control] size %d, layer %d, carrier %d.", 
			size, layer, carrier);
		*/	

		//print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[ctrl] [2] pointer %p ", pVal8);
	}
	else
	{
		len = paid->len; // data_only
		tot__len = len + 3;
	}

	//-----------------------------------------
	Chk_CFG_Ctrl_Auto_RPT_REQ( AID[0], AID[1] ); // later...when REMS Ctrol
	//-----------------------------------------

	//print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[ctrl] [3] pointer %p ", pVal8);
	
	switch(AID[0])
	{
		/*
		////2020.04.01
		//////---------------------------------------------------------------
		////case 0x01:
		//////---------------------------------------------------------------
		////	switch(AID[1])
		////	{
		////		case 0x00:
		////		case 0x01:
		////			if( paid->AID[1] == 0x00 ) // maker
		////				m_St.Maker[0] = paid->data[0];
		////			else                       // supplyer
		////				m_St.Maker[1] = paid->data[0];
		////			break;
		////		//---------------
		////		default:break;
		////		//---------------
		////	}
		////	break;
		*/	

		#ifdef _AID_DELETE_NEEDED__	
		//---------------------------------------------------------------
		case 0x02:
		//---------------------------------------------------------------
			switch(AID[1])
			{				
				case 0x02:					
					Use_TempLmt(paid->data[0]);					
					break;

				case 0x49: 
					m_Eep.OSM_Use = paid->data[0] & 0x01;
					break;					

				//---------------
				default:break;
				//---------------
			}
			break;
		#endif


		#ifdef _AID_DELETE_NEEDED__	
		//---------------------------------------------------------------
		case 0x09:
		//---------------------------------------------------------------
			switch( AID[1] )
			{				
				case 0x14: 
					SetResetInfo( Rst_By_User, 262 );	
					m_Env.is_reboot = 1; 
					break;
				
				//---------------
				default:break;
				//---------------
			}
			break;
		#endif


		
		//---------------------------------------------------------------
		case 0xF2:
		//---------------------------------------------------------------
			switch(AID[1])
			{					
				case 0xF2: // 알람 지그모드. 2019.05.01				
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_St.AlmTest = 1;

						ALM_JIG_MODE = ALM_JIG_MODE_CODE;
						OSTimerReset( &Alarm_JIG_Timer );
												
						memcpy(&m_St.AlmFlag, pVal8, RU_ALARM_CNT);
						memcpy(&m_St.AlmCtrl, &pVal8[RU_ALARM_CNT], RU_ALARM_CNT);

						Proc____GUI_Test_Alarm();
					}
					break;	

				case 0xF3: // 2019.05.06 : 타사 과입력 ALC on/off
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							if(*pVal8 == ON) uVal8 = ON;
							else  uVal8 = OFF;
							
							m_Eep.Other_Rvs_IN_ALC_Onoff[amp] = uVal8;
						}
						pVal8++; // next
					}					
					break;
			}
			break;	
			
		//---------------------------------------------------------------
		case 0x50:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x02: // Layer 사용 정보	
					for(l = 0; l < MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						uVal8 = *pVal8++;
						
						if(uVal8 >= 0x04) continue; // 2020.02.03 : 0~3 only
						
						if(flag32 & cbits32[flag_msb--])							
						{
							m_St.LayerUseInfo[l] = uVal8;
							//---------------------------------
							// 2020.02.03
							if(m_Eep.LayerUseInfo[l] == AMP_USE)
							{
								if(m_St.LayerUseInfo[l] != AMP_USE) // if not-use
								{
									m_Env.Use_Not_Req[l] = 1;
									m_Env.Use_Not_Req2[l] = 1;
								}
							}
							//---------------------------------
							m_Eep.LayerUseInfo[l] = uVal8;
						}						
					}
					break;

				case 0x0B:
					for(c = 0; c < MAX_Optic; c++)
					{
						if(flag_msb < 0) break;
						
						if(flag32 & cbits32[flag_msb--])
						{	
							 uVal32 = ntohl(*pVal32);
							 m_Eep.d_CenterFreq[c] = uVal32;
						#ifdef __TYPE__PRU__
							cal_u32 = Use_d_Cal_VCO_PRU_1khz(c, DL, uVal32);		
							m_Reg_Flg.General_FA[c].RW_PRU_DL_NCO.uD32 = 1;
							m_Reg_Ctl.General_FA[c].RW_PRU_DL_NCO.uD32 = cal_u32;
							
							cal_u32 = Use_d_Cal_VCO_PRU_1khz(c, UL, uVal32);
							m_Reg_Flg.General_FA[c].RW_PRU_UL_NCO.uD32 = 1;
							m_Reg_Ctl.General_FA[c].RW_PRU_UL_NCO.uD32 = cal_u32;
						#else // ANTS-AAU
							cal_u32 = Use_d_Cal_VCO_AAU_1khz(uVal32);
							m_Reg_Flg.General_FA[c].RW_NCO.uD32 = 1;
							m_Reg_Ctl.General_FA[c].RW_NCO.uD32 = cal_u32;
						#endif
							printf(VT_BOLD_YELLOW "[RCU] FA%d Fc %d" VT_END_CR, c, uVal32);
							D_Printf(DEBUG_SYSTEM, VT_GREEN "[ctrl] FA%d Fc %d \n", c, uVal32);
						}
						pVal32++;
					}
					break;	

				case 0x13:
					if(flag_msb < 0) break;						
					if(flag32 & cbits32[flag_msb--])
					{
						sVal16 = ntohs(*pVal16);
					
					#if 1 // TEST 2019.09.16
						/*
						systime[0] = 19;
						systime[1] = 8;
						systime[2] = 7;
						systime[3] = 23;
						systime[4] = 0;
						systime[5] = 0;
						if(sVal16 == -1000) 
						{							
							Change_Local_Time(systime);
							break;
						}						
						if(sVal16 == -999) 
						{
							system_agent("hwclock --hctosys");
							break;
						}
						if(sVal16 == -998) 
						{							
							if(IS___Valid_gREMS_Local_Time(systime))
							{
								system_agent("hwclock --systohc");
							}
							break;
						}
						*/
						#if 0 // ifdef __ALARM_LOG_BACKUP__
						if(sVal16 == -990)
						{
							m_Env.gREMS_Test = 0;
							break;
						}
						else if(sVal16 == -991)
						{
							m_Env.gREMS_Test = 1;
							break;
						}
						
						if((sVal16 <= -990)&&(sVal16 >= -1000))
						{							
							make_log_chksum_test(sVal16);
							break;
						}
						#endif						

						if(sVal16 == -997)
						{
							TEST__send_5m_log_file_via_sz();
							break;
						}	
						if(sVal16 == -998)
						{
							char file_name[128];
							INT8U size;
							
							m_St.Log_Cnt = get__list_of_5m_log_file();	// get log file list
							print_agent(DEBUG_LOG, VT_BOLD_CYAN, "count %03d", m_St.Log_Cnt);

							for(c = 0; c < m_St.Log_Cnt; c++)
							{
								strcpy(file_name, get__name_of_5m_log_file(c));
								size = strlen(file_name);
								
								print_agent(DEBUG_LOG, VT_BOLD_CYAN, "%3d (%3d byte) %s", 
											c, size, get__name_of_5m_log_file(c));
							}
							break;
						}	

						if(sVal16 == -999)
						{
							m_Eep.AAU_Def_Setting_20200204 = 0;	
							m_Eep.PRU_Def_Setting_20200123 = 0;
							break;
						}
						if(sVal16 == -1000)
						{						
							m_Eep.f_UL_Gain_Low_Offset[0] = 0;
							m_Eep.f_UL_Gain_Low_Offset[1] = 0;
							m_Eep.f_UL_Gain_Low_Offset[2] = 0;
							m_Eep.f_UL_Gain_Low_Offset[3] = 0;

							print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] f_UL_Gain_Low_Offset Clear"); 
							
							break;
						}
					#endif
					
						Use_TempLmt_x10(sVal16);
					}					
					break;

					

				case 0x1C: 
					if(flag_msb < 0) break;						
					if(flag32 & cbits32[flag_msb--])
						m_Eep.OSM_Use = pVal8[0] & 0x01;
					break;						

				//---------------
				default:break;
				//---------------
			}
			break;
			
		//---------------------------------------------------------------
		case 0x51:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x01: // DL TSSI Lower limit -------------------					
					for(c = 0; c < MAX_Optic; c++)
					{
						for(l = 0; l < MAX_UL; l++)
						{	
							if(flag_msb < 0) break;

							uVal16 = ntohs((INT16S)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.DL_TSSI_UP_x10[c][l] = (INT16S)uVal16;
								TSSI_SD_Level_Set(c, l, (INT16S)uVal16);
							}
						}
					}
					break;

				case 0x02: // DL TSSI Lower limit					
					for(c = 0; c < MAX_Optic; c++)
					{
						for(l = 0; l < MAX_UL; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs((INT16S)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
								m_Eep.DL_TSSI_DN_x10[c][l] = (INT16S)uVal16;
						}
					}
					break;

				case 0x04: // DL 출력 상한
				case 0x05: // DL 출력 하한
					if(AID[1] == 0x04) i = HI;
					else               i = LO;
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						uVal16 = ntohs(*pVal16++); 
						if(flag32 & cbits32[flag_msb--])
							Use_Fwd_Out_Pwr_Lmt_Set(amp, i, (INT16S)uVal16);
					}
					break;

				case 0x06: // DL ATTEN, 0.1dB Step
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						
						uVal16 = ntohs((INT16S)*pVal16++); 
						
						if(flag32 & cbits32[flag_msb--])
						{						
						#ifdef __TYPE__PRU__ // ANTS, HFR
							uVal8 = conv_att_01_to_05(uVal16); // step GUI(0.1) RF(0.5)
							Add_PAU_N_Ctrl_Data(amp, 0, 7, &uVal8);
							UseFwd_Out_Att(amp, uVal8);
						#endif
						
						#ifdef __TYPE__AAU__
							// step GUI(0.1) RF(0.5)
							uVal8 = conv_att_01_to_05(uVal16);
							uVal8 = conv_att_05_to_025(uVal8);

							D_Printf(DEBUG_SYSTEM, "[AID_Control] DL ATT : GUI 0x%X -> Reg 0x%X\n", uVal16, uVal8);	

							AAU_RF_DL_Att_Set(amp, (m_St.DL_Att[amp] - m_Eep.DL_Att_User[amp] + uVal8)/2);
							m_Eep.DL_Att_User[amp] = uVal8;							
						#endif							
						}
					}
					break;

				case 0x16: // PAU On/Off(DL)						
					if(m_Env.AMP_Ctrl_Allowed == 0) // 2019.09.24
					{
						break;
					}
				#ifdef __TYPE__PRU__ // ANTS-PRU	
					for(l = 0; l < AMPU_4_MAX; l++)
					{
						if(flag_msb < 0) break;
						uVal8 = *pVal8++; 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Env.AMP_User_Ctrl[l] = 1;
							Use_PRU_PwrAmp_OnOff(l, uVal8);

							if(uVal8 == ON)							
								m_Env.Pwr_Amp_Ctrl[l] = 'N'; // oN
							else
							{
								m_Env.Pwr_Amp_Ctrl[l] = 'F'; // ofF
								Set__Reason_Dis_5117(l, Rsn_Dis_User___Ctrl); // 2019.12.20
							}
						}
					}
				#else // ANTS-AAU
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						D_Printf(DEBUG_SYSTEM, "[AID_Control] onoff 0x%02X\n", *pVal8);
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							m_Reg_Ctl.RFU[amp].PA_OnOff.Data = Read_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff]);
							
							if(*pVal8) 	m_Reg_Ctl.RFU[amp].PA_OnOff.On = 0; // Register OFF
							else 		m_Reg_Ctl.RFU[amp].PA_OnOff.On = 1; // Register ON

							m_Env.AMP_User_Ctrl[amp] = 1;

							uVal8 = *pVal8;
							if(uVal8 == ON)
							{
								m_Env.Pwr_Amp_Ctrl[amp] = 'N'; // oN
							}
							else
							{
								m_Env.Pwr_Amp_Ctrl[amp] = 'F'; // ofF
								Set__Reason_Dis_5117(amp, Rsn_Dis_User___Ctrl); // 2019.12.20 
							}

							Write_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff], m_Reg_Ctl.RFU[amp].PA_OnOff.Data, "PA OnOff");

							m_Reg_Flg.RFU[amp].PA_OnOff.Data = _F_SET_;
							m_Eep.Pwr_Amp_En[amp] = *pVal8; // GUI Ctrl Value
							m_Eep.RVS_Amp_Onoff[amp] = *pVal8; // GUI Ctrl Value
							m_PAU_C_Req[amp].Rvs_AMP_En = *pVal8;
							SD_Init_Req(amp);
							AAU_FWD_SD_Init_Req(amp);
							AAU_RVS_SD_Init_Ctrl(amp); // for algorithm restart							
						}
						pVal8++;
					}
				#endif
					break;
				
				case 0x19: // PAU Reset
					for(l = 0; l < AMPU_4_MAX; l++)
					{
						if(flag_msb < 0) break;
						uVal8 = *pVal8++; 
						if(flag32 & cbits32[flag_msb--])
							Use_PwrAmp_Reset(l, uVal8);
					}
					break;	
				//---------------
				default:break;
				//---------------
			}
			break;
		//---------------------------------------------------------------
		case 0x52:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x01: // UL RSSI Upper limit					
					for(c = 0; c < MAX_Optic; c++)
					{
						for(l = 0; l < MAX_UL; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs((INT16S)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
								m_Eep.UL_RSSI_UP_x10[c][l] = (INT16S)uVal16;
						}
					}
					break;

				case 0x02: // UL RSSI Lower limit					
					for(c = 0; c < MAX_Optic; c++)
					{
						for(l = 0; l < MAX_UL; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs((INT16S)*pVal16++);
							if(flag32 & cbits32[flag_msb--])
								m_Eep.UL_RSSI_DN_x10[c][l] = (INT16S)uVal16;
						}
					}
					break;				

				case 0x06: // ATTEN (UL)
				#ifdef __TYPE__PRU__ // ANTS-PRU, digital Atten	
					// 2019.04.18
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							uVal16 = ntohs((INT16S)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								if(uVal16 > 20*10) // max 20dB
									uVal16 = 20*10; 
								
								m_Eep.d_UL_Att_x10[c][l] = uVal16;
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_UL_OUTPUT_ATT.uD32 = _F_SET_;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_UL_OUTPUT_ATT.uD32 = uVal16;
							}
						}
					}
					break;
				#else  // ANTS-AAU, AID 0x5206
					// 2019.05.31
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;							
							uVal16 = ntohs(*pVal16++); 
							uVal8  = step_01__round__step_05(uVal16);
							uVal16 = roundoff_05(uVal16);
							
							if(flag32 & cbits32[flag_msb--])
							{	
								if(uVal16 > 20*10) // max 20dB
									uVal16 = 20*10; 
									
								m_Eep.d_Att_Dig_UL_User_x10[c][l] = uVal16;
								Set_ALC_Gain_Comp_x10(c, l, m_St.AAU_ALC_Gain_Comp_x10[c][l]);								
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, Main UL-DIG-Att-Offset %d/10, cal_u16 0x%X\n", 
									c, l, uVal16, cal_u32);
							}
						}
					}					
					break;
				#endif	

				case 0x09: // Gain Balance (UL)
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							uVal16 = ntohs((INT16S)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								// 2019.06.11 : 15[dB] limit, 소수점 버림.
								if(uVal16 > 150) uVal16 = 150; // max 15.0[dB]
								uVal8 = uVal16%10;
								m_Eep.d_UL_GainBal_x10[c][l] = (uVal16 - uVal8);
							}
						}
					}
					break;	
					
				case 0x16:	// UL LNA ON OFF
					if(m_Env.AMP_Ctrl_Allowed == 0)  // 2019.09.24
					{
						break;
					}
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{						
							m_Eep.RVS_Amp_Onoff[amp] = *pVal8; // GUI Ctrl Value
							
						#ifdef __TYPE__PRU__
							byte = 2; bit = 6; // LNA On/Off(UL)
							Add_PAU_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian
						#else // AAU
							D_Printf(DEBUG_SYSTEM, "[AID_Control] onoff 0x%02X\n", *pVal8);
							
							m_Reg_Ctl.RFU[amp].PA_OnOff.Data = Read_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff]);
							
							if(*pVal8)	m_Reg_Ctl.RFU[amp].PA_OnOff.On = 0; // Register OFF
							else		m_Reg_Ctl.RFU[amp].PA_OnOff.On = 1; // Register ON

							m_Env.AMP_User_Ctrl[amp] = 1;

							Write_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff], m_Reg_Ctl.RFU[amp].PA_OnOff.Data, "PA OnOff");

							m_Reg_Flg.RFU[amp].PA_OnOff.Data = _F_SET_;
							m_Eep.Pwr_Amp_En[amp] = *pVal8; // GUI Ctrl Value
							m_PAU_C_Req[amp].Rvs_AMP_En = *pVal8;
							SD_Init_Req(amp);							
						#endif							
						}
						pVal8++; // next
					}					
					break;
				
				//---------------
				default:break;
				//---------------
			}
			break;
		//---------------------------------------------------------------
		case 0x53:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x09: // PD Power 하한 // 2020.06.24
				case 0x0C: // PD Power 상한
					if(AID[1] == 0x09) c = _DN_;
					else c = _UP_;
					
					for(l=0; l<4; l++)
					{
						if(flag_msb < 0) break;
						uVal16 = ntohs(*pVal16++);
						
						if(flag32 & cbits32[flag_msb--])
							m_Eep.PD_Pwr_Lmt_x10[c][l] = (INT16S)uVal16;
					}
					break;	

				case 0x0B: // 2020.06.24
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						if(pVal8[0] == 0x01)							
							Init__BIP_ErrCnt_Accum();
					}
					break;

				
				// 2020.07.01 added.
				case 0x22:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_St.d_Hard_Clip_Cnt_Init = 1; // init request
					}
					break;
				
				//---------------
				default:break;
				//---------------
			}
			break;			
		
		//---------------------------------------------------------------
		case 0x54:
		//---------------------------------------------------------------
			switch(AID[1])
			{
			#ifdef _AID_DELETE_NEEDED__
				case 0x00: // Toffset // register is read-only
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal32 = ntohl((INT32S)*pVal32++); 

						m_Eep.d_Toffset[_100M] = uVal32;
						m_Reg_Flg.DLY[_100M].t_Offset.uD32 = 1;
						m_Reg_Ctl.DLY[_100M].t_Offset.uD32 = conv_Reg_GuardTime_from_usec(uVal32);

						m_Eep.d_Toffset[Alpha] = uVal32;
						m_Reg_Flg.DLY[Alpha].t_Offset.uD32 = 1;						
						m_Reg_Ctl.DLY[Alpha].t_Offset.uD32 = conv_Reg_GuardTime_from_usec(uVal32);

						D_Printf(DEBUG_SYSTEM, "[AID_Control] Toffset %d 0x%08X\n", uVal32, uVal32);
					}
					break;

				case 0x01: // Toffset_DL // register is read-only
					for(c = 0; c < MAX_Optic; c++)
					{
						if(flag_msb < 0) break;
					
						uVal32 = ntohl((INT32S)*pVal32++); 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.d_TBdelay_DL[c] = uVal32;
							m_Reg_Flg.DLY[c].TB_Delay_DL.uD32 = 1;
							m_Reg_Ctl.DLY[c].TB_Delay_DL.uD32 = conv_Reg_GuardTime_from_usec(uVal32);
							
							D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d, Toffset_DL %d 0x%08X\n", c, uVal32, uVal32);
							D_Printf(DEBUG_SYSTEM, "[AID_Control] flag32 0x%X, flag_msb %d\n", flag32, flag_msb);
						}
					}
					break;

				case 0x02: // Toffset_UL // register is read-only
					for(c = 0; c < MAX_Optic; c++)
					{
						if(flag_msb < 0) break;
					
						uVal32 = ntohl((INT32S)*pVal32++); 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.d_TBdelay_UL[c] = uVal32;
							m_Reg_Flg.DLY[c].TB_Delay_UL.uD32= 1;
							m_Reg_Ctl.DLY[c].TB_Delay_UL.uD32 = conv_Reg_GuardTime_from_usec(uVal32);
							
							D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d, Toffset_UL %d 0x%08X\n", c, uVal32, uVal32);
							D_Printf(DEBUG_SYSTEM, "[AID_Control] flag32 0x%X, flag_msb %d\n", flag32, flag_msb);
						}
					}
					break;
				#endif

				//------------------------------------
				case 0x03: // FPGA Delay(DL), 2019.05.21
					for(c=0; c<MAX_FA_PATH; c++) // 2019.05.30, AID changed
					{
						if(flag_msb < 0) break;
						uVal32 = ntohl(*pVal32++);
						
						if(flag32 & cbits32[flag_msb--])
							m_Eep.LS_Delay_DL_nsec[c] = uVal32;
					}
					Proc___Ctrl_from_LS();
					break;

				case 0x04: // FPGA Delay(UL), 2019.05.21
					for(c=0; c<MAX_FA_PATH; c++) // 2019.05.30, AID changed
					{
						if(flag_msb < 0) break;							
						uVal32 = ntohl(*pVal32++);
							
						if(flag32 & cbits32[flag_msb--])
							m_Eep.LS_Delay_UL_nsec[c] = uVal32;
					}
					Proc___Ctrl_from_LS();
					break;

				//------------------------------------
				case 0x10:				
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
					#ifdef __TYPE__PRU__
						if(pVal8[0] <= FAN_Mode_Low_Noise) // 0x01
						{
							m_Eep.FAN_Mode_Onoff = pVal8[0];
						}
					#else // 2019.06.18 : "Off Mode" Added
						if(pVal8[0] <= FAN_Mode_OFF) // 0x02
						{
							m_Eep.FAN_Mode_Onoff = pVal8[0];
						}
					#endif
					}
				break;
				
				case 0x12:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						Use_FAN_Mode_Time( 0, pVal8[3], pVal8[2]);
						Use_FAN_Mode_Time( 1, pVal8[1], pVal8[0]);
					}
					break;
					
				case 0x20: // TDD Mode
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.RW_TDD_Mode = pVal8[0] & 0x03; // 0(Normal), 1(DL), 2(UL), 3(FDD) : 2019.04.11 : FDD Added.						
					
						m_Reg_Flg.RW_TDD_Mode.Mode = 1;
						m_Reg_Ctl.RW_TDD_Mode.Mode = pVal8[0] & 0x03;

						D_Printf(DEBUG_SYSTEM, "[AID_Control] TDD Mode 0x%02X, 0x%08X, 0x%08X\n", 
							pVal8[0],
							*pReg_TDD_Addr[Reg_TDD_RW_TDD_Mode][STS], 
							*pReg_TDD_Addr[Reg_TDD_RW_TDD_Mode][CTL]);
					
					}
					break;




				case 0x21: // DL ON Guard Time
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal16 = ntohs(*pVal16);						
						Use_DL_ON_Guard_Time_Ctrl(uVal16); // 2019.06.28 : 0 ~ 5[us]
					}
					break;
					
				case 0x22: // DL OFF Guard Time
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal16 = ntohs(*pVal16);
						Use_UL_ON_Guard_Time_Ctrl(uVal16); // 2019.06.28 : 0 ~ 10[us]
					}
					break;
					
				case 0x23: // DL to UL Guard Time
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal16 = ntohs(*pVal16);
						Use_DL_to_UL_Guard_Time_Ctrl(uVal16); // 2019.06.28 : 0 ~ 10[us]
					}
					break;
					
				case 0x24: // UL to DL Guard Time
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal16 = ntohs(*pVal16);
						Use_UL_to_DL_Guard_Time_Ctrl(uVal16); // 2019.06.28 : 0 ~ 5[us]
					}
					break;

				case 0x30: // MIMO Standby Mode (On/Off)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.MIMO_Stdby_Mode_Onoff = pVal8[0];
						m_St.MIMO_Stdby_Enable = pVal8[0];

						// if both AMP_Stdby and Mimo_Stdby ON, off the Amp_Standby
						if((m_Eep.MIMO_Stdby_Mode_Onoff == ON) && (m_Eep.AMP_Stdby_Mode_Onoff == ON))
						{
							m_Eep.AMP_Stdby_Mode_Onoff = OFF;
							m_St.AMP_Stdby_Enable = OFF;
						}
					}
					break;

				case 0x32:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						Use_MIMO_Stdby_Mode_Time( 0, pVal8[3], pVal8[2]);
						Use_MIMO_Stdby_Mode_Time( 1, pVal8[1], pVal8[0]);
					}
					break;
				
					
				case 0x33:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						if(pVal8[0] < Oper_Max)
						{
							m_Eep.Layer_Mode = pVal8[0];

							// 2019.08.29 : 다시 제어 가능하게 수정.
							//#ifdef __TYPE__AAU__	
							// 2019.04.24 : AAU is "MIMO Mode" Only	, AID 0x5433							
							////m_Eep.Layer_Mode = Oper_MIMO; 
							//#endif
						}
					}
					break;					
				
				case 0x34: // AMP Standby Mode (On/Off)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.AMP_Stdby_Mode_Onoff = pVal8[0];
						m_St.AMP_Stdby_Enable = pVal8[0];

						// if both AMP_Stdby and Mimo_Stdby ON, off the Mimo_Stdby
						if((m_Eep.MIMO_Stdby_Mode_Onoff == ON) && (m_Eep.AMP_Stdby_Mode_Onoff == ON))
						{
							m_Eep.MIMO_Stdby_Mode_Onoff = OFF;
							m_St.MIMO_Stdby_Enable = OFF;
						}		
					}
					break;					

				case 0x36:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						Use_AMP_Stdby_Mode_Time( 0, pVal8[3], pVal8[2]);
						Use_AMP_Stdby_Mode_Time( 1, pVal8[1], pVal8[0]);
					}
					break;

				case 0x37: // AMP Standby Mode 적용 Path
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.AMP_Stdby_Path = pVal8[0];
						m_St.AMP_Stdby_Path = pVal8[0];
					}
					break;	

				case 0x38: // Batt_Ext_Mode_Onoff
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						m_Eep.Batt_Ext_Mode_Onoff = pVal8[0];
					break;

				case 0x3A: // MIMO Standby Mode(진입 Level)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						m_Eep.MIMO_Stdby_Level_x10[_IN_] = ntohs(*pVal16);
					break;

				case 0x3B: // MIMO Standby Mode(해제 Level)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						m_Eep.MIMO_Stdby_Level_x10[_OUT_] = ntohs(*pVal16);
					break;

				case 0x3F: // Test Port 출력
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						Use_TestPort_CIM(pVal8[0]);
					}
					break;

				case 0x44:
					if(flag_msb < 0) break;
					if(pVal8[0] < RS_MAX)
					{
						m_Eep.UDA_Oper_Mode[UDA_Spurious] = pVal8[0]; // 2019.04.27
					}
					break;

				case 0x46:
					if(flag_msb < 0) break;
					if(pVal8[0] < AR_MAX)
					{
						m_Eep.UDA_Oper_Mode[UDA_Clock] = pVal8[0]; // 2019.04.27
						Use_UDA_Oper_Mode_Ctrl(UDA_Clock, pVal8[0]);
					}
					break;

				case 0x47:
					if(flag_msb < 0) break;
					if(pVal8[0] < AR_MAX)
					{
						m_Eep.UDA_Oper_Mode[UDA_PD_CP] = pVal8[0]; // 2019.04.27
						Use_UDA_Oper_Mode_Ctrl(UDA_PD_CP, pVal8[0]);
					}
					break;

				case 0x50:
					if(flag_msb < 0) break;
					if((pVal8[0] == ON)||(pVal8[0] == OFF))
					{
						/*
						2020.01.20
						(1) 아래 삭제.
						////if(AufoConfigFlag.RE_Cascade_ID == 0) // 2019.05.13
						(2) cascade ID == 0 에 무관하게 저장가능하도록 수정.
						(3) 제어시, 상태도 바로 바뀌도록 수정.(cpri lock 여부에 관계없이)
						*/
						m_Eep.UL_Gain_A_BackOff = pVal8[0]; // 2019.04.27
						// 2020.01.20.
						m_St.UL_Gain_A_BackOff = m_Eep.UL_Gain_A_BackOff;
					}
					break;

				case 0x51: // 2019.09.03
					if(flag_msb < 0) break;
					if(pVal8[0] == ON) 
					{
						 m_Eep.AAU_CW_Auto_Onoff = 1; // 'AID  ON(0)', 2020.01.20 added
					}
					else
					{
						m_Eep.AAU_CW_Auto_Onoff = 0; // 'AID  ON(0)', 2020.01.20 added
					}
					break;
					
				case 0x52: // Super Cap. 교체, Local Only
				/*
					Super Cap. 명령 수신 시
					Super Cap. Alarm 보고
					Alarm ACK 수신 후 Super Cap. Alarm 해제
				*/
					if(flag_msb < 0) break;
					if(pVal8[0] == 0x01)
					{
						m_St.AlmSt.ETC.SuperCap_Exch = 1; // 교체
					}
					break;
					
				case 0x53: // logging On/Off
					if(flag_msb < 0) break;
					if((pVal8[0] == ON)||(pVal8[0] == OFF))
					{
						if(m_Eep.Log_OnOff != pVal8[0])
						{
							m_St.Log_Prev = m_Eep.Log_OnOff;
							m_Eep.Log_OnOff = pVal8[0];
							if(m_Eep.Log_OnOff == ON) 
								m_St.Log_Cnt = 0;	
						
							if(m_Eep.Log_OnOff == ON)
								User_alarm_log_on();
							else 
								User_alarm_log_off();
						}
					}
					break;
					
				case 0x54: // log count
					if(flag_msb < 0) break;					
					
					/*
					2020.06.11
						(1) '88' 로 제어시에만 LS 등록번호 Null로 제어 후 저장
						(2) '0' 로 제어시에만 로그 초기화
						(3) 그외 제어시에는 아무것도 하지 않음.
					*/
					
					if(pVal8[0] == 88)
					{
						// for auto generation.
						memset(m_Eep.LS_Regist_Num, 0, 20); 
						memset(m_St.LS_Regist_Num,  0, 20);
						// user control이라, eep save 됨. 아래가 자동 호출됨.
						// m_Env.is_eep_write = EEP_WRITE_CODE;
					}
					else if(pVal8[0] == 0) // 0 제어시 count 0으로 초기화
					{					
						m_St.Log_Cnt = Remove_all_AlmLog_Files();
					}
					break;
					
				/*
					AAU only
					LED 자동 Off 0x5455 : Default: ON,  Off 시간: 5분
				*/
				case 0x55:
					m_St.AAU_LED_Auto_Off = pVal8[0];
					break;
					
				//---------------
				default:break;
				//---------------
			}
			break;

		
		//---------------------------------------------------------------
		case 0x56:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x00:				
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						memcpy(m_Eep.Statis_Rpt_Ref, pVal8, 2);
						m_Env.STATIS.time_change = 1;
					}
					break;
				
				case 0x01:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.Statis_Rpt_Prd = pVal8[0];
						m_Env.STATIS.time_change = 1;
					}
					break;

				case 0x03:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						memcpy(m_Eep.REMS_Auto_Rpt_Time, pVal8, 2);
					break;	

				case 0x20:
					#ifdef __TYPE__PRU__						
						m_Env.psu_reset_req = 1;						
						SetResetInfo( Rst_By_User, 262 ); // 2019.10.22 : FPGA(DTU) Reset, 운용자 제어
					#else // 2019.06.19
						SetResetInfo( Rst_By_User, 262 );	
						m_Env.is_reboot = 1; 
					#endif
					break;	
					
				//---------------
				default:break;
				//---------------
			}
			break;
		//---------------------------------------------------------------
		case 0xB0:
		//---------------------------------------------------------------
			//print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[ctrl] [4] pointer %p ", pVal8);
			//fflush(stdout);
			switch(AID[1])
			{				
				case 0x01:					
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Env.Ctrl_MAC_Req = 1;
						memcpy(m_Env.Ctrl_MAC, pVal8, 6);
					}
					break;
					
				case 0x07: // S/N					
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						if(SN_SIZE <= buf_size) // 2019.05.14
						{
							memcpy(data, pVal8, SN_SIZE);
							for(i = 0; i < SN_SIZE; i++)
							{
								if((data[i] == '\0')&&((i+1) < SN_SIZE))
								{
									for(c = i+1; c < SN_SIZE; c++)
									{
										data[c] = '\0';
									}	
									break;	
								}
							}
							// 2020.03.20 : init
							memset(m_Eep.LS_Regist_Num, 0, SN_SIZE+1); 
							memset(m_St.LS_Regist_Num,  0, SN_SIZE+1);
							memset(AufoConfigFlag.LS_Regist_Num, 0, SN_SIZE+1);

							// 2020.03.20 : User 제어는 eep 저장함.
							memcpy(m_Eep.LS_Regist_Num, data, SN_SIZE); 
							memcpy(m_St.LS_Regist_Num,  data, SN_SIZE);
							memcpy(AufoConfigFlag.LS_Regist_Num, data, SN_SIZE);
					
							#ifdef __TYPE__AAU__
							Set_Bt_req(BT_SERIAL);
							#endif
						}						
					}
					break;

				case 0x08: // Board Name					
					if(flag_msb < 0) break;					
					if(flag32 & cbits32[flag_msb])
					{
						if(BD_NANEMSIZE <= buf_size) // 2019.05.14
						{
							memcpy(data, pVal8, BD_NANEMSIZE);
							for(i = 0; i < BD_NANEMSIZE; i++)
							{
								if((data[i] == '\0')&&((i+1) < BD_NANEMSIZE))
								{
									for(c = i+1; c < BD_NANEMSIZE; c++)
									{
										data[c] = '\0';
									}	
									break;	
								}
							}
							memcpy(m_Eep.RU_Board_Name, data, BD_NANEMSIZE); // 2019.05.14.
							memcpy(AufoConfigFlag.RU_Board_Name, m_Eep.RU_Board_Name, BD_NANEMSIZE);
						}
					}
					break;

				#ifdef __TYPE__AAU__ 
				case 0x10: // AAU Type
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						if(BD_NANEMSIZE <= buf_size) // 2019.05.14
						{							
							if(pVal8[0] == REPEATER_TYPE_1)
							{
								if((pVal8[1] == REPEATER_TYPE_5G_AAU_C)||(pVal8[1] == REPEATER_TYPE_5G_AAU_W))
								{
									memcpy(m_Eep.AAU_Type, pVal8, 2);
									memcpy(m_St.Type, pVal8, 2);
								}
							}
						}
					}
					break;
				///// YCSO added START 190716
				case 0x0D:	////// bluetooth pairing code
					if(flag_msb < 0) break;					
					if(flag32 & cbits32[flag_msb])
					{
						if(BT_PAIRING_CODE_SIZE <= buf_size) // 2019.05.14
						{							
							memcpy(data, pVal8, BT_PAIRING_CODE_SIZE);
							for(i = 0; i < BT_PAIRING_CODE_SIZE; i++)
							{
								if((data[i] == '\0')&&((i+1) < BT_PAIRING_CODE_SIZE))
								{
									for(c = i+1; c < BT_PAIRING_CODE_SIZE; c++)
									{
										data[c] = '\0';
									}	
									break;	
								}
							}
							memcpy(m_Eep.BT_Pairing_Code, data, BT_PAIRING_CODE_SIZE); // 2019.05.14.
							Set_Bt_req(Get_Bt_status());
						}
					}
					break;
				///// YCSO added END
				#endif
					case 0x12:
						if(cmd == CMD_CONTROL)
						{
							if(flag_msb < 0) break;
							if(flag32 & cbits32[flag_msb])
							{
								memset(m_Eep.User_ID, 0, 11);
								memcpy(m_Eep.User_ID, pVal8, 10);

								print_agent (DEBUG_REMS, VT_BOLD_RED, "[CTRL_GUI_ID] %02X %02X %02X %02X %02X %02X %02X %02X", 
								pVal8[0], pVal8[1], pVal8[2], pVal8[3], 
								pVal8[4], pVal8[5], pVal8[6], pVal8[7]);
							}
						}
						else if(cmd == CMD_STATUS)
						{
							memset(m_Env.User_ID, 0, 11);
							memcpy(m_Env.User_ID, pVal8, 10);
							m_Env.gui_id_req = 1;							
						}
						break;
						
					case 0x13:
						if(cmd == CMD_CONTROL)
						{
							if(flag_msb < 0) break;
							if(flag32 & cbits32[flag_msb])
							{
								memset(m_Eep.User_PW, 0, 11);
								memcpy(m_Eep.User_PW, pVal8, 10);
								
								print_agent (DEBUG_REMS, VT_BOLD_RED, "[CTRL_GUI_PW] %02X %02X %02X %02X %02X %02X %02X %02X", 
									pVal8[0], pVal8[1], pVal8[2], pVal8[3], 
									pVal8[4], pVal8[5], pVal8[6], pVal8[7]);
							}
						}
						else if(cmd == CMD_STATUS)
						{
							memset(m_Env.User_PW, 0, 11);
							memcpy(m_Env.User_PW, pVal8, 10);
							m_Env.gui_pw_req = 1;
						}
						break;

					case 0x14:
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb])
							m_Eep.mask = pVal8[0];
						break;						
						
				//--------------
				default: break;
				//--------------
			}
			break;	
		
		//---------------------------------------------------------------
		case 0xB2:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x01: // 온도 보상 On/Off(PAU)
					D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						D_Printf(DEBUG_SYSTEM, "[AID_Control] onoff 0x%02X\n", *pVal8);
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							Add_PAU_N_Ctrl_Data(amp, 6, 7, pVal8);
						}
						pVal8++;
					}					
					break;
					
				case 0x09: // Return Loss Threshold(DL)//
				case 0x0A: // Return Loss Revcover(DL)
				case 0x0B: // High Temp Threshold(DL)
				case 0x0C: // High Temp Recovery(DL)
				case 0x0D: // Over Power Shutdown Level(DL)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);
					#ifdef __TYPE__PRU__	
					     if(AID[1] == 0x09) {byte = 0; bit = 4;} // Return Loss Threshold(DL)
					else if(AID[1] == 0x0A) {byte = 0; bit = 3;} // Return Loss Revcover(DL)
					else if(AID[1] == 0x0B) {byte = 0; bit = 7;} // High Temp Threshold(DL)
					else if(AID[1] == 0x0C) {byte = 0; bit = 6;} // High Temp Recovery(DL)
					else                    {byte = 0; bit = 5;} // Over Power Shutdown Level(DL)
					#endif
										
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							pVal8  = (INT8U*)pVal16;
							
							#ifdef __TYPE__PRU__
							Add_THRE_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian
							#else // AAU, 2019.04.20
							if(AID[1] == 0x0D)
							{
								m_St.Over_Pwr_Thr_x10[amp] = (INT16S)uVal16;
								m_Eep.Over_Pwr_Thr_x10[amp] = (INT16S)uVal16;
							}
							#endif
							
							uVal8 = *pVal8++;
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X-%02X, 0x%04X\n", uVal8, *pVal8, uVal16);
						}
						pVal16++; // next 2byte
					}					
					break;

				case 0x17: // Offset(PAU Temp)
					#ifdef __TYPE__PRU__
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{ 
							m_Eep.Temp_Offset_x10[amp] = (INT16S)ntohs(*pVal16); 
						}
						pVal16++; // next 2byte
					}
					#endif
					break;

				//	
				// ATTEN (PAU)(DL), TPL : not used @ 5G, maybe....
				//
				case 0x20: 
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					#ifdef __TYPE__PRU__
					byte = 0; bit = 6; // ATTEN (PAU DL), TPL
					#endif
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal16 = ntohs((INT16S)*pVal16); 
							#ifdef __TYPE__PRU__
							uVal8 = uVal16/5; // 0.1dB -> 0.5dB step
							TPL_Fwd_Att_Set(amp, uVal8);
							Add_PAU_N_Ctrl_Data(amp, byte, bit, &uVal8); // pass as big-endian
							D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X, 0x%04X\n", uVal8, uVal16);						
							#endif
						}
						pVal16++;
					}
					break;				
	
				case 0x27: // ATTEN (PAU Gain 보상)(DL)
				case 0x28: // ATTEN (PAU FB Gain 보상)(DL)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					#ifdef __TYPE__PRU__
					if(AID[1] == 0x27) {byte = 0; bit = 5;} // ATTEN (PAU Gain 보상)(DL)
					else               {byte = 0; bit = 3;} // ATTEN (PAU FB Gain 보상)(DL)
					#endif
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal16 = ntohs((INT16S)*pVal16); 
							#ifdef __TYPE__PRU__
							uVal8 = uVal16/5; // 0.1dB -> 0.5dB step
							Add_PAU_N_Ctrl_Data(amp, byte, bit, &uVal8); // pass as big-endian
							D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X, 0x%04X\n", uVal8, uVal16);						
							#endif
						}
						pVal16++;
					}
					break;

				case 0x21: // ATTEN (PAU FB)(DL)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);	
					#ifdef __TYPE__PRU__
					byte = 0; bit = 4;
					#endif
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal16 = ntohs((INT16S)*pVal16); 
							#ifdef __TYPE__PRU__
							uVal8 = uVal16/5; // 0.1dB -> 0.5dB step
							Use_FB_Att(amp, uVal8);
							Add_PAU_N_Ctrl_Data(amp, byte, bit, &uVal8); // pass as big-endian
							D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X, 0x%04X\n", uVal8, uVal16);						
							#endif
						}
						pVal16++;
					}
					break;	
					
				
				case 0x24: // ATTEN (LNA)(UL)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						
						if(flag32 & cbits32[flag_msb--])
						{
							if(m_St.Rvs_IN_ALC_Onoff[amp] == OFF)
							{
								uVal16 = ntohs((INT16S)*pVal16); 							
							#ifdef __TYPE__PRU__
								uVal8 = conv_att_01_to_05(uVal16); // 2019.04.06 : 0.1dB -> 0.5dB step, 
								
								Add_PAU_N_Ctrl_Data(amp, 1, 7, &uVal8); // pass as big-endian

								uVal8 = conv_att_01_to_05(uVal16);
								m_PAU_C_Req[amp].Rvs_Att_x2 = uVal8;
								m_Eep.AMP_Rvs_Att_x2[amp] = uVal8;

								D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X, 0x%04X\n", uVal8, uVal16);
							#else							
								uVal8 = conv_att_01_to_05(uVal16); // 2019.04.06 : 0.1dB -> 0.5dB step, 
								uVal8 = conv_att_05_to_025(uVal8); // 2019.04.17 : 0.5 -> 0.25
								
								m_Eep.UL_Att[amp] = uVal8; // 0.25 step	
								AAU_RF_UL_Att_Set(amp, uVal8/2);
							#endif
							}
						}
						pVal16++;
					}
					break;

				case 0x29: // ATTEN(LNA Gain 보상)(UL)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal16 = ntohs((INT16S)*pVal16); 							
						#ifdef __TYPE__PRU__
							uVal8 = uVal16/5; // 0.1dB -> 0.5dB step
							Add_PAU_N_Ctrl_Data(amp, 1, 6, &uVal8); // pass as big-endian
						#else
							#ifdef __MAKER_ANTS__							
							#else							
							#endif
						#endif
						}
						pVal16++;
					}
					break;

				#ifdef __TYPE__PRU__ 
				case 0x30: // ALC On/Off(UL)
				case 0x33: // Gain Balance On/Off(UL)			
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);		
					#ifdef __TYPE__PRU__
					     if(AID[1] == 0x30) {byte = 1; bit = 4;} // ALC On/Off(UL)			
					else                    {byte = 1; bit = 3;} // Gain Balance On/Off(UL)
					#endif					
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							Add_PAU_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass[0] 0x%02X\n", *pVal8);
						}
						pVal8++; // next
					}					
					break;					

				case 0x31: // ATTEN Fair On/Off(DL)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);
					#ifdef __TYPE__PRU__
					byte = 6; bit = 5;
					#endif
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							Use_ATT_Pair_Onoff(amp, *pVal8);
							Add_PAU_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass[0] 0x%02X\n", *pVal8);
						}
						pVal8++; // next
					}					
					break;	
				#endif

				#ifdef __TYPE__AAU__ 
				case 0x30: // ALC On/Off(UL), 2019.04.19
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							if(*pVal8 == ON) uVal8 = ON;
							else  uVal8 = OFF;
							// 2019.04.19
							m_Eep.Rvs_IN_ALC_Onoff[amp] = uVal8;
							m_St.Rvs_IN_ALC_Onoff[amp] = uVal8;
						}
						pVal8++; // next
					}					
					break;

				case 0x33:
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							m_St.Rvs_Gain_Bal_On[amp] = *pVal8;
							m_Eep.Rvs_Gain_Bal_On[amp] = *pVal8;
						}
						pVal8++; // next
					}		
					break;			
				#endif
				
				
				
				
				
				case 0x32: // Auto Shutdown Mode On/Off(UL)			
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);		
					#ifdef __TYPE__PRU__
					byte = 6; bit = 6;
					#endif
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							Use_Auto_SD_Mode_En(amp, *pVal8);
							#ifdef __TYPE__PRU__
							Add_PAU_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian
							#endif
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass[0] 0x%02X\n", *pVal8);
						}
						pVal8++; // next
					}					
					break;


			#ifdef __TYPE__PRU__
				case 0x10: // Offset(DL 출력)
				case 0x12: // Offset(UL 출력)
				case 0x13: // Offset(UL 입력)
				case 0x14: // Offset(FB 출력)
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					     if(AID[1] == 0x10) {byte = 0; bit = 2;} // Offset(DL 출력)
					else if(AID[1] == 0x12) {byte = 1; bit = 5;} // Offset(UL 출력)
					else if(AID[1] == 0x13) {byte = 1; bit = 6;} // Offset(UL 입력)
					else                    {byte = 0; bit = 0;} // Offset(FB 출력)
										
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							Add_THRE_N_Ctrl_Data(amp, byte, bit, (INT8U*)pVal16); // pass as big-endian
						}
						pVal16++; // next 2byte
					}
					break;					
			#else // AAU
				case 0x10: // Offset (DL 출력)
				case 0x12: // Offset (UL 출력)
				case 0x13: // Offset (UL 입력)										
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal16 = ntohs(*pVal16); // 2019.12.11 added
							
							if(AID[1] == 0x10) 
							{
								m_Eep.FWD_Pwr_Offset_x10[amp] = (INT16S)uVal16;  // 0xB210
								m_St.FWD_Pwr_Offset_x10[amp]  = (INT16S)uVal16;  // 0xB210
							}
							else if(AID[1] == 0x12) 
							{
								m_Eep.Rvs_OUT_Pwr_Offset_x10[amp] = (INT16S)uVal16; // 0xB212
								m_St.Rvs_OUT_Pwr_Offset_x10[amp]  = (INT16S)uVal16; // 0xB212
							}
							else
							{
								m_Eep.Rvs_IN_Pwr_Offset_x10[amp] = (INT16S)uVal16;	 // 0xB213
								m_St.Rvs_IN_Pwr_Offset_x10[amp]  = (INT16S)uVal16;	 // 0xB213
							}
						}
						pVal16++; // next 2byte
					}
					break;
			#endif	

			#ifdef __TYPE__PRU__
				case 0x15: // Offset(Delay DL) // PRU Only
				case 0x16: // Offset(Delay UL) // PRU Only
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);
					
					if(AID[1] == 0x15) {byte = 1; bit = 2;} // Offset(Delay DL)
					else               {byte = 1; bit = 0;} // Offset(Delay UL)
										
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal32 = ntohl(*pVal32);
							uVal16 = htons(conv_delay_1ns_to_001us(uVal32));
							pVal8  = (INT8U*)&uVal16;
							Add_THRE_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian

							uVal8 = *pVal8++;
							D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X-%02X, %d[ns]", uVal8, *pVal8, uVal32);
						}
						pVal32++; // next 4byte
					}
					break;	
				#endif

				
				#ifdef __TYPE__PRU__
				case 0x34: // ALC Level(UL)			    0xB2,0x34
				case 0x35: // Shutdown Level(UL)		0xB2,0x35
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					if(AID[1] == 0x34) {byte = 0; bit = 1;} // ALC Level(UL)	
					else               {byte = 0; bit = 0;} // Shutdown Level(UL)
										
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							pVal8  = (INT8U*)pVal16;
							Add_PAU_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian

							uVal8 = *pVal8++;
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X-%02X, 0x%04X\n", uVal8, *pVal8, uVal16);							
						}
						pVal16++; // next 2byte
					}
					break;
				#else // ------------ AAU
				case 0x34: // ALC Level(UL)			    0xB2,0x34
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_St.Rvs_IN_ALC_x10[amp] = (INT16S)uVal16;
							m_Eep.Rvs_IN_ALC_x10[amp] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;
					
				case 0x35: // Shutdown Level(UL)		0xB2,0x35
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_St.Rvs_IN_SD_x10[amp] = (INT16S)uVal16;
							m_Eep.Rvs_IN_SD_x10[amp] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;				
				#endif

				

				case 0x36: // Gain Balance value(UL)	0xB2,0x36
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);					
					#ifdef __TYPE__PRU__
					byte = 1; bit = 5; // Gain Balance value(UL)
					#endif
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							uVal8  = uVal16/5; // 0.1 stpe -> 0.5step
							#ifdef __TYPE__PRU__
							Add_PAU_N_Ctrl_Data(amp, byte, bit, &uVal8);
							#else // AAU
							m_Eep.Rvs_Gain_Bal_x2[amp] = uVal8;
							m_St.Rvs_Gain_Bal_x2[amp] = uVal8;
							#endif
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass 0x%02X, 0x%04X\n", uVal8, uVal16);
						}
						pVal16++; // next 2byte
					}
					break;		
					
				///// YCSO_LNA_CEHCK
				case 0x37:  // LNA On/Off(UL), 0xB237 AID				
					if(m_Env.AMP_Ctrl_Allowed == 0) // 2019.09.24
					{						
						break;
					}
				#ifdef __TYPE__PRU__				
					//D_Printf(DEBUG_SYSTEM, "[AID_Control] AID[1] 0x%02X\n", AID[1]);
					byte = 2; bit = 6; // LNA On/Off(UL)
										
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.RVS_Amp_Onoff[amp] = *pVal8;
							Add_PAU_N_Ctrl_Data(amp, byte, bit, pVal8); // pass as big-endian
							//D_Printf(DEBUG_SYSTEM, "[AID_Control] pass[0] 0x%02X\n", *pVal8);
						}
						pVal8++; // next
					}					
					break;									
				#else	//AAU			
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						D_Printf(DEBUG_SYSTEM, "[AID_Control] onoff 0x%02X\n", *pVal8);
						if(flag_msb < 0) break;;
						if(flag32 & cbits32[flag_msb--])
						{							
							m_Reg_Ctl.RFU[amp].PA_OnOff.Data = Read_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff]);
							
							if(*pVal8) 	m_Reg_Ctl.RFU[amp].PA_OnOff.On = 0; // Register OFF
							else 		m_Reg_Ctl.RFU[amp].PA_OnOff.On = 1; // Register ON

							m_Env.AMP_User_Ctrl[amp] = 1;

							Write_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff], m_Reg_Ctl.RFU[amp].PA_OnOff.Data, "PA OnOff");

							m_Reg_Flg.RFU[amp].PA_OnOff.Data = _F_SET_;
							m_Eep.Pwr_Amp_En[amp] = *pVal8; // GUI Ctrl Value
							m_Eep.RVS_Amp_Onoff[amp] = *pVal8; // GUI Ctrl Value
							m_PAU_C_Req[amp].Rvs_AMP_En = *pVal8;
							SD_Init_Req(amp);
						}
						pVal8++;
					}				
				#endif								
				break;

				#ifdef __TYPE__AAU__ 
				case 0x3B: // DL ALC On/Off(UL), 2019.05.30
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							if(*pVal8 == ON) uVal8 = ON;
							else  uVal8 = OFF;
							
							m_Eep.AAU_Fwd_ALC_Onoff[amp] = uVal8;
						}
						pVal8++; // next
					}					
					break;
				
				case 0x3C: // DL SD On/Off(UL), 2019.05.30
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							if(*pVal8 == ON) uVal8 = ON;
							else  uVal8 = OFF;
							
							m_Eep.AAU_Fwd_SD__Onoff[amp] = uVal8;
						}
						pVal8++; // next
					}					
					break;
				#endif	
					
				//--------------
				default:break;
				//--------------				
			}
			break;

		//---------------------------------------------------------------
		case 0xB3:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x05:
					if(flag_msb < 0) break;					
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.Threshold_UL_Gain_Low_x10 = ntohs(*pVal16);

						for(l=0; l<MAX_DL; l++)
							m_Eep.UL_Gain_Low_x10[l] = ntohs(*pVal16);
					}
					break;
					
				case 0x07:
					for(c=0; c<MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = (INT16S)ntohs(*pVal16++); 
							
							if(flag32 & cbits32[flag_msb--])
								m_Eep.RSSI_Offset_x10[c][l] = sVal16;
						}
					}					
					break;							
					
				case 0x08:
					if(flag_msb < 0) break;					
					if(flag32 & cbits32[flag_msb])
						m_Eep.Threshold_UL_Spurious_x10 = ntohs(*pVal16);
					break;

				case 0x09: // 2019.05.30
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.UL_Noise_Offset_x10[c][l] = sVal16;
							}
						}
					}
					break;
					
				//--------------
				default: break;
				//--------------
			}
			break;

		//---------------------------------------------------------------
		case 0xB4:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x04: // DPD onoff
					for(i = 0; i < MAX_DL; i++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
							Use_d_DPD_OnOff(i, pVal8[i] & 0x01);
					}
					break;

				case 0x0A: // CFR onoff
					for(i = 0; i < MAX_DL; i++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
							Use_d_CFR_OnOff(i, pVal8[i] & 0x01);
					}
					break;	

				case 0x0B: // CFR level
					for(i = 0; i < MAX_DL; i++)
					{
						if(flag_msb < 0) break;
						
						uVal16 = ntohs(*pVal16++); 
						if(flag32 & cbits32[flag_msb--])
						#ifdef __TYPE__PRU__
							Use_d_CFR_Level(i, uVal16);
						#else
							Use_d_PAR_Level(i, uVal16);
						#endif
					}
					break;

				
				case 0x0C:	///// Hard Clipping On/Off(DL)
					for(i = 0; i < MAX_DL; i++)
					{
						if(flag_msb < 0) break;
						uVal8 = *pVal8++; 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.d_Hard_Clip_OnOff[i] = uVal8 & 0x01;
						#ifdef __TYPE__AAU__							
							Use_d_HC_Threshold_AAU(i, m_Eep.d_Hard_Clip_Lvl_x10[i]);
						#endif						
						}							
					}
					break;
				
				case 0x0D:
					for(i = 0; i < MAX_DL; i++)
					{
						if(flag_msb < 0) break;
						uVal16 = ntohs(*pVal16++); 
						if(flag32 & cbits32[flag_msb--])
						{
						#ifdef __TYPE__PRU__
							Use_d_HC_Threshold(i, uVal16);
						#else
							Use_d_HC_Threshold_AAU(i, uVal16); // 2019.09.20
						#endif							
						}
					}
					break;

				/*
				// 2020.07.01 삭제.				
				//case 0x0F:
				//	if(flag_msb < 0) break;
				//	if(flag32 & cbits32[flag_msb])
				//	{
				//		m_St.d_Hard_Clip_Cnt_Init = 1; // init request
				//	}
				//	break;
				*/

				case 0x10:	// DPD Offset(DL 출력)					
					for(i = 0; i < MAX_DL; i++)
					{
						if(flag_msb < 0) break;
						
						sVal16 = (INT16S)ntohs(*pVal16++); 
						if(flag32 & cbits32[flag_msb--])						
							m_Eep.d_Offset_DPD_x10[i] = sVal16;
					}
					break;


				case 0x11:		///// Reset(DPD) 
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						DPD_Reset_Ctrl(pVal8[0]); // 2019.05.01					
					break;			
						

				case 0x15: // 2019.04.06, 0xB415
					for(c=0; c<MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							uVal32 = ntohl(*pVal32++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.d_Offset_DTU_Delay_DL[c][l] = uVal32;
								sVal32 = Use_d_Delay_Ctrl(DL, c, l, _Use_Disp_);
								
								if(sVal32 >= 0)
									Use_d_FPGA_Delay_Ctrl(DL, c, l, sVal32, _Use_Disp_);
							}
						}
					}
					break;					

				case 0x16: // 2019.04.06
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							uVal32 = ntohl(*pVal32++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.d_Offset_DTU_Delay_UL[c][l] = uVal32;
								sVal32 = Use_d_Delay_Ctrl(UL, c, l, _Use_Disp_);

								if(sVal32 >= 0)
									Use_d_FPGA_Delay_Ctrl(UL, c, l, sVal32, _Use_Disp_);
							}
						}
					}
					break;	

				case 0x17: // 2019.05.09
					if(flag_msb < 0) break;
					m_Eep.RCU_Temp_Offset_x10 = ntohs(*pVal16);
					break;
					
				case 0x1E: // RE Channel Bandwidth 
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						data[0] = pVal8[0] & 0x0F; // 100M
						data[1] = (pVal8[0] & 0xF0) >> 4; // alpha
						Use_d_RE_Channel_BandWidth(data[0], data[1], 'A'); // 1 : AID control
						Update_ARFCN_from_BW();						
					}					
					break;

				///// AID_NEEDED : AID 0x5007과 유사, 단, byte size 다름. How ??
				case 0x1F: // ARFCN : AID 0x5007과 유사, 단, byte size 다름. How ??
					for(c=0; c < MAX_FA_PATH; c++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							//////
							//////
							//////
						}
					}					
					break;
					
				case 0x20: // DL Digital Att (RW_DL_Gain)
					#ifdef __TYPE__PRU__ // ANTS-PRU
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							uVal16 = ntohs(*pVal16++);

							D_Printf(DEBUG_SYSTEM, "[AID_Control] %d, %d\n", uVal8, uVal16);
							
							if(flag32 & cbits32[flag_msb--])
							{								
								PRU_DL_Gain_Set_Req(c, l, uVal16);
								m_Eep.d_Att_Dig_DL_x10[c][l] = uVal16;
								
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, DL-DIG-Att %d/10, att_x2 %d, regVal 0x%X\n", 
									c, l, uVal16, uVal8, cal_u32);
							}
						}
					}
					#else // ANTS-AAU
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							uVal16 = ntohs(*pVal16++); // Att_x10							
							uVal8 = step_01__round__step_05(uVal16);
							//--------------------------------------							
							val_x10 = Get__AAU_DL_Gain_Att_x10(c, l);
							//--------------------------------------
							D_Printf(DEBUG_SYSTEM, "[AID_Control] %d, %d\n", uVal8, uVal16);
							
							uVal16 = roundoff_05(uVal16);

							D_Printf(DEBUG_SYSTEM, "[AID_Control] roundoff %d\n", uVal16);
							
							if(flag32 & cbits32[flag_msb--])
							{		
								cal_u32 = Get_GainRegVal(DL, uVal8 + val_x10/5);
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_Gain.uD32 = 1;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_Gain.uD32 = cal_u32;
								
								m_Eep.d_Att_Dig_DL_x10[c][l] = uVal16;
								
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, DL-DIG-Att %d/10, att_x2 %d, regVal 0x%X\n", 
									c, l, uVal16, uVal8, cal_u32);
							}
						}
					}
					#endif
					break;	

				case 0x21: // UL Digital Att (RW_UL_Gain)
					#ifdef __TYPE__PRU__ // ANTS-PRU
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;							
							uVal16 = ntohs(*pVal16++); 							
							if(flag32 & cbits32[flag_msb--])
								Set_PRU_RVS_DTU_Att_x10(c, l, uVal16);
						}
					}
					#else // ANTS-AAU UL ATT
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;							
							uVal16 = ntohs(*pVal16++); // Att_x10							
							uVal8  = step_01__round__step_05(uVal16);
							uVal16 = roundoff_05(uVal16);
							
							if(flag32 & cbits32[flag_msb--])
							{	
								m_Eep.d_Att_Dig_UL_Offs_x10[c][l] = uVal16;
								Set_ALC_Gain_Comp_x10(c, l, m_St.AAU_ALC_Gain_Comp_x10[c][l]);								
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, UL-DIG-Att %d/10, cal_u16 0x%X\n", 
									c, l, uVal16, cal_u32);
							}
						}
					}
					#endif	
					break;					
				
				case 0x24: // ATTEN(Digital DL)(SUM)
					if(flag_msb < 0) break;
					for(l = 0; l < MAX_DL; l++)
					{
						if(flag_msb < 0) break;							
						uVal16 = ntohs(*pVal16++);
						if(flag32 & cbits32[flag_msb--])
							Use_Att_Digital_DL_SUM(l, uVal16);
					}
					break;

				// 2020.06.24 삭제. 
				/*
				//case 0x28: // 2019.05.09
				//	if(flag32 & cbits32[flag_msb])					
				//	{						
				//		for(l = 0; l < 4; l++)
				//			m_St.d_BIP_Clr[l] = 1;
				//	}
				//	break;
				*/

				case 0x29: // FPGA Delay(DL)
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;							
							uVal32 = ntohl(*pVal32++);
							
							if(flag32 & cbits32[flag_msb--])
								Use_d_FPGA_Delay_Ctrl(DL, c, l, uVal32, _Use_Disp_);
						}
					}
					break;	
					
				case 0x2A: // FPGA Delay(UL)
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;							
							uVal32 = ntohl(*pVal32++);
							
							if(flag32 & cbits32[flag_msb--])
								Use_d_FPGA_Delay_Ctrl(UL, c, l, uVal32, _Use_Disp_);
						}
					}
					break;	

				
				case 0x2B: // Toffset // register is read-only
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal32 = ntohl((INT32S)*pVal32++); 

						m_Eep.d_Toffset[_100M] = uVal32;
						m_Reg_Flg.DLY[_100M].t_Offset.uD32 = 1;
						m_Reg_Ctl.DLY[_100M].t_Offset.uD32 = conv_Reg_GuardTime_from_usec(uVal32);

						m_Eep.d_Toffset[Alpha] = uVal32;
						m_Reg_Flg.DLY[Alpha].t_Offset.uD32 = 1;						
						m_Reg_Ctl.DLY[Alpha].t_Offset.uD32 = conv_Reg_GuardTime_from_usec(uVal32);

						D_Printf(DEBUG_SYSTEM, "[AID_Control] Toffset %d 0x%08X\n", uVal32, uVal32);
					}
					break;

				case 0x2C: // register is read-only
					for(c = 0; c < MAX_Optic; c++)
					{
						if(flag_msb < 0) break;
					
						uVal32 = ntohl((INT32S)*pVal32++); 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.d_TBdelay_DL[c] = uVal32;
							m_Reg_Flg.DLY[c].TB_Delay_DL.uD32 = 1;
							m_Reg_Ctl.DLY[c].TB_Delay_DL.uD32 = conv_Reg_GuardTime_from_usec(uVal32);
							
							D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d, Toffset_DL %d 0x%08X\n", c, uVal32, uVal32);
							D_Printf(DEBUG_SYSTEM, "[AID_Control] flag32 0x%X, flag_msb %d\n", flag32, flag_msb);
						}
					}
					break;

				case 0x2D: // register is read-only
					for(c = 0; c < MAX_Optic; c++)
					{
						if(flag_msb < 0) break;
					
						uVal32 = ntohl((INT32S)*pVal32++); 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.d_TBdelay_UL[c] = uVal32;
							m_Reg_Flg.DLY[c].TB_Delay_UL.uD32= 1;
							m_Reg_Ctl.DLY[c].TB_Delay_UL.uD32 = conv_Reg_GuardTime_from_usec(uVal32);
							
							D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d, Toffset_UL %d 0x%08X\n", c, uVal32, uVal32);
							D_Printf(DEBUG_SYSTEM, "[AID_Control] flag32 0x%X, flag_msb %d\n", flag32, flag_msb);
						}
					}
					break;
				
						
				case 0x2E:	/// DL PATH ON/OFF, 2019.04.17
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{							
							if(flag_msb < 0) break;

							uVal8 = *pVal8 & 0x01; 							
							if(flag32 & cbits32[flag_msb--])
								Use_DTU_DL_Path_Onoff(c, l, uVal8);
							pVal8++; 
						}
					}
					break;

				// added 2019.04.17
				case 0x2F:	/// UL PATH ON/OFF
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;

							uVal8 = *pVal8 & 0x01;
							if(flag32 & cbits32[flag_msb--])
								Use_DTU_UL_Path_Onoff(c, l, uVal8);
							pVal8++; 
						}
					}
					break;


				case 0x30:	/// Test Pattern On/Off(DL)
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							uVal8 = *pVal8 & 0x01;
							m_Reg_Flg.General_FA[_100M].General_FA_MIMO[l].RW_TEST_Pattern.OnOff = _SET_;
							m_Reg_Ctl.General_FA[_100M].General_FA_MIMO[l].RW_TEST_Pattern.OnOff = uVal8 ^ 1;

							m_Reg_Flg.General_FA[Alpha].General_FA_MIMO[l].RW_TEST_Pattern.OnOff = _SET_;
							m_Reg_Ctl.General_FA[Alpha].General_FA_MIMO[l].RW_TEST_Pattern.OnOff = uVal8 ^ 1;							
						}
						pVal8++;
					}
					break;					
			
				case 0x31: // Payload On/Off
					for(c = 0; c < MAX_FA_PATH; c++) // 2019.04.13
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							uVal8 = *pVal8 & 0x01;
							if(flag32 & cbits32[flag_msb--])
								Use_d_Payload_OnOff(c, l, uVal8);
							pVal8++;
						}
					}
					break;

				
				case 0x32:	/// SFP On/Off
					// 2019.06.21
					for(fa = 0; fa < MAX_FA_PATH; fa++) 
					{
						for(l = 0; l < 2; l++)
						{
							if(flag_msb < 0) break;
							if(flag32 & cbits32[flag_msb--])
							{									
								c = fa + 2*l;
								uVal8 = *pVal8 & 0x01;
								
								if(uVal8 == 0) i = 0x1; // ON - Normal
								else           i = 0x0; // OFF - Reset
								
								Write_FPGA_Reg(Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_Reset], i, "SFP ON/OFF"); 							
							}
							pVal8++;
						}
					}
					break;					
	
				///// AID_NEEDED : how to use ??
				case 0x33:	/// Debug On/Off(L2 SW)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
					{
						//////
						//////
						//////
					}
					break;

				///// AID_NEEDED : how to use ??
				case 0x34:	/// 온도 보상 On/Off(DTU)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
					{
						m_Eep.TempComp_Onoff = pVal8[0];
					}
					break;

									
				case 0x35: // Rx/Cascade Add On/Off(UL)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						Use_d_Rx_Casc_Add_OnOff_UL(pVal8[0]);
					break;					

				
				case 0x36:	/// Loopback On/Off
					/*
					2019.07.17 : 
					Loopback On/Off는 제어는 안되고 상태만 표시하며, 
					VSS 필드의 Loopback 필드값이 0xCC, 0xCA인 경우 On, 
					이외 다른값은 Off로 처리
					*/
					break;				
				
				case 0x39:	/// Reset(DTU)
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						User_DTU_Reset(pVal8[0]);
					}
					break;

				case 0x6E: // VSS Monitoring On/Off 
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						if(pVal8[0] <= 0x04)
						{
							m_St.VSS_Monitor_OnOff = pVal8[0];
						}
						// D_Printf(DEBUG_SYSTEM, "[AID_Control] m_St.VSS_Monitor_OnOff 0x%02X\n", pVal8[0]);
					}
					break;

				///// ANTS Not Use
				/*
				//case 0x6F:
				//case 0x70:	/// VSS 송신.
				//	if(flag_msb < 0) break;
				//	if(flag32 & cbits32[flag_msb])
				//	{					
				//	}
				//	break;
				*/	
				//--------------
				default: break;
				//--------------
			}
			break;		

		//---------------------------------------------------------------
		case 0xB6:
		//---------------------------------------------------------------
			switch(AID[1])
			{			
				case 0x08:
					if(flag_msb < 0) break;
					m_Eep.DC_Fail_Volt_x10[DC__48V] = ntohs(*pVal16);
					break;
					
				// 전압(DTU) Main 전원 DC Fail 설정값
				case 0x09:		
					if(flag_msb < 0) break;
					m_Eep.DC_Fail_Volt_x10[DC_Main] = ntohs(*pVal16);
					break;

				case 0x0A: // 2019.07.29, 전압(DC 48V) 상한
				case 0x0B: // 2019.07.29, 전압(DC 48V) 하한					
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
					{							
						uVal16 = ntohs(*pVal16);
						
						if(AID[1] == 0x0A) m_Eep.AAU_48V_Limit_x10[_UP_] = (INT16S)uVal16;
						if(AID[1] == 0x0B) m_Eep.AAU_48V_Limit_x10[_DN_] = (INT16S)uVal16;
					}
					break;	
					
				case 0x10: // FAN(외장) ON  온도
				case 0x11: // FAN(외장) OFF 온도
					uVal16 = ntohs(*pVal16);
					uVal8  = uVal16/10; // 0.1 stpe -> 1 step
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{					
						if(AID[1] == 0x10) Use_FAN_On__Temp_Ctrl(EXT_FAN, uVal8);
						if(AID[1] == 0x11) Use_FAN_Off_Temp_Ctrl(EXT_FAN, uVal8);
					}
					break;
					
				case 0x12: // FAN(외장) RPM 
					uVal16 = ntohs(*pVal16);
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						Use_Ext_FAN_Speed_Ctrl((INT8U)uVal16);
					break;


				case 0x14: // FAN(INT_FAN) ON  온도
				case 0x15: // FAN(INT_FAN) OFF 온도
					uVal16 = ntohs(*pVal16);
					uVal8  = uVal16/10; // 0.1 stpe -> 1 step
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{					
						if(AID[1] == 0x14) Use_FAN_On__Temp_Ctrl(INT_FAN, uVal8);
						else               Use_FAN_Off_Temp_Ctrl(INT_FAN, uVal8);
					}
					break;

				case 0x16: // PRU-PSU-Internal FAN
					uVal16 = ntohs(*pVal16);
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						if(uVal16 == 100)
							Use_PSU_INT_FAN_Onoff_Ctrl(ON);
						else if(uVal16 == 0)						
							Use_PSU_INT_FAN_Onoff_Ctrl(OFF);
					}					
					break;

				case 0x18:
					uVal16 = ntohs(*pVal16);
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_St.FAN_Ext_Max_RPM = (INT8U)uVal16;
						m_Eep.FAN_Ext_Max_RPM = (INT8U)uVal16;
					}
					break;
				break;		

				
				case 0x13: // FAN(외장) 자동제어
				case 0x17: // FAN(PSU) 자동제어					
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{					
						if(AID[1] == 0x13) Use_FAN_Auto_Ctrl(EXT_FAN, pVal8[0]);
						else               Use_FAN_Auto_Ctrl(INT_FAN, pVal8[0]);
					}
					break;				

				case 0x26:	//// 전력량 초기화.......
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						PSU_Pwr_Reset_Ctrl(); // 2019.04.18
					}
					break;

				case 0x39:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						PSU_Reset_Ctrl();
					break;
					
				//--------------
				default:break;
				//--------------				
			}
			break;	


		
			
		
		//---------------------------------------------------------------
		case 0xC0: // ANTS Hidden AID
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x12:
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs((INT16U)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								Use_DL_CW_Level_Ctrl(c, l, uVal16);
							}
						}
					}		
					break;

				case 0x13:
					aid_user_ctrl++;
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							uVal8 = *pVal8++; 
							if(flag32 & cbits32[flag_msb--])
							{
								m_St.User_DL_CW_OnOff[c][l] = uVal8; // 2019.09.25
								Use_DL_CW_OnOff_Ctrl(c, l, uVal8);
							}
						}
					}		
					break;

				
				case 0x14: // RW_DL_EQ_Coeff
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{							
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_EQ_Coeff.uD32 = 1;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_EQ_Coeff.uD16[0] = sVal16*m_St.EQ_Toggle[c][DL][0];
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_EQ_Coeff.uD16[1] = 0;
								m_Eep.d_EQ_Coeff_DL[c][l] = sVal16;
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, DL_EQ_Coeff %d(0x%8X)", c, l, sVal16, sVal16);
							}
						}
					}		
					break;


				case 0x15: // 0xC015 DL Gain Ctrl
					#ifdef __TYPE__PRU__ // ANTS-PRU
					// NOT Use This AID @ PRU
					#else // ANTS-AAU
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							sVal8  = step_01__round__step_05(sVal16); // gain_x2
							
							if(flag32 & cbits32[flag_msb--])
							{
								att_x2 = Conv_Gain_to_Att(sVal8*5)/5;
								//----------------------------------------------
								val_x10 = Get__AAU_DL_Gain_Att_x10(c, l);
								//----------------------------------------------
								cal_u32 = Get_GainRegVal(DL, att_x2 + val_x10/5);
								m_Eep.d_Att_Dig_DL_x10[c][l] = 5*att_x2;
								
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_Gain.uD32 = 1;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_Gain.uD32 = cal_u32;								
																
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, RW_DL_Gain %d, att_x2 %d/2, 0x%08X(with temp)\n", 
									c, l, sVal8, att_x2, cal_u32);
							}
						}							
					}
					#endif
					break;					

				case 0x16: // 0xC016 UL Gain Ctrl
					#ifdef __TYPE__PRU__ // ANTS-PRU
					// not use this AID @ PRU
					#else // ANTS-AAU
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							sVal8  = step_01__round__step_05(sVal16); // gain_x2

							if(flag32 & cbits32[flag_msb--])
							{
								att_x2 = Conv_Gain_to_Att(sVal8*5)/5;
								m_Eep.d_Att_Dig_UL_Offs_x10[c][l] = 5*att_x2;
								Set_ALC_Gain_Comp_x10(c, l, m_St.AAU_ALC_Gain_Comp_x10[c][l]);								
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, RW_UL_Gain %d, att_x2 %d/2, 0x%08X\n", 
									c, l, sVal8, att_x2, cal_u32);
							}
						}							
					}
					#endif
					break;

				
				case 0x19: // RW_UL_EQ_Coeff
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs((INT32S)*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_UL_EQ_Coeff.uD32 = 1;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_UL_EQ_Coeff.uD16[0] = sVal16*m_St.EQ_Toggle[c][UL][0];
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_UL_EQ_Coeff.uD16[1] = 0;
								m_Eep.d_EQ_Coeff_UL[c][l] = sVal16;
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, UL_EQ_Coeff %d(0x%8X)", c, l, sVal16, sVal16);
							}
						}
					}
					break;
					
				#ifdef __TYPE__AAU__
				case 0x1A: // RW_DL_EQ_Coeff_1
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_EQ_1_Coeff.uD32 = 1;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_EQ_1_Coeff.uD16[0] = sVal16*m_St.EQ_Toggle[c][DL][1];
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_EQ_1_Coeff.uD16[1] = 0;								
								
								m_Eep.d_EQ_Coeff_1_DL[c][l] = sVal16;
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, DL_EQ_Coeff_1 %d(0x%8X)", c, l, sVal16, sVal16);
							}
						}
					}		
					break;
				#endif
				
				#ifdef __TYPE__AAU__
				case 0x1B: // RW_UL_EQ_Coeff_1
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_UL_EQ_1_Coeff.uD32 = 1;
								m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_UL_EQ_1_Coeff.uD32 = sVal16*m_St.EQ_Toggle[c][UL][1];
								
								m_Eep.d_EQ_Coeff_1_UL[c][l] = sVal16;
								D_Printf(DEBUG_SYSTEM, "[AID_Control] c %d l %d, UL_EQ_Coeff_1 %d(0x%8X)", c, l, sVal16, sVal16);
							}
						}
					}		
					break;	
				#endif	

			
				case 0x62: // LSH_Gain_Set
				case 0x63: // LSH_Gain_Set
				case 0x64: // LSH_Gain_Set
					uVal8 = AID[1]- 0x62;
					if(uVal8 >= 3) break;
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
								Use_LSH_Gain_Set(uVal8, c, l, DL, uVal16);
						}
					}		
					break;
					
				case 0x65: // LSH_Gain_Set
				case 0x66: // LSH_Gain_Set
				case 0x67: // LSH_Gain_Set
					uVal8 = AID[1]- 0x65;
					if(uVal8 >= 3) break;
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
								Use_LSH_Gain_Set(uVal8, c, l, UL, uVal16);
						}
					}		
					break;

				case 0x68: // Test Pattern
					for(c = 0; c < MAX_FA_PATH; c++)
					{
						for(l = 0; l < MAX_MIMO_PATH; l++)
						{
							if(flag_msb < 0) break;
							
							uVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
								Use_Test_Pattern_Band_Gain_Set(c, l, uVal16);
						}
					}		
					break;
				////// YCSO added 190715
				#ifdef __TYPE__AAU__
				case 0x6A:	/// 파형감시 DL offset
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.aau_spectrum_DL_offest[l] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}

					break;
					
				case 0x6B:	/// 파형감시 UL offset
					for(l=0; l<MAX_UL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.aau_spectrum_UL_offest[l] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}

					break;
				#endif
				////// YCSO added END
				case 0x20:
					for(l = 0; l < 5; l++)
					{
						if(flag_msb < 0) break;
						
						uVal16 = ntohs((INT16U)*pVal16++); 
						if(flag32 & cbits32[flag_msb--])
						{
							m_Reg_Flg.UL_GAIN_Guard_Time[l].uD32 = 1;
							m_Reg_Ctl.UL_GAIN_Guard_Time[l].uD32 = conv_Reg_GuardTime_from_usec(uVal16);
							
							m_Eep.d_UL_Gain_Guard_time[l] = uVal16;
						}
					}
					break;	

				#ifdef __TYPE__AAU__ // AID Control : AAU Only
				case 0x21:					
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
						Use_TSYNC_Delay_Set_AAU(uVal32);
					break;
				#endif	

				case 0x23: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_Capture_DL_Time = conv_Reg_Capture_from_usec(uVal32); 
					break;
					
				case 0x24: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_Capture_UL_Time = conv_Reg_Capture_from_usec(uVal32); 
					break;

				case 0x25: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_Capture_ULNoise_Time = conv_Reg_Capture_from_usec(uVal32); 
					break;			

				case 0x26: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_UL_Noise_RSSI_StartTime = conv_Reg_RSSI_Time_from_usec(uVal32); 
					break;

				case 0x27: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_UL_Noise_RSSI_StopTime = conv_Reg_RSSI_Time_from_usec(uVal32); 
					break;			
				
				case 0x28: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_UL_RSSI_StartTime = conv_Reg_RSSI_Time_from_usec(uVal32); 
					break;
					
				case 0x29: // 2019.06.01
					if(flag_msb < 0) break;
					uVal32 = ntohl(*pVal32); 
					if(flag32 & cbits32[flag_msb--])
					m_Eep.d_UL_RSSI_StopTime = conv_Reg_RSSI_Time_from_usec(uVal32); 
					break;	
					
				///// AID_NEEDED AID 0xC035 ?/
				case 0x35:
					break;

				case 0x36:	//// ATT DL, 0.25 step
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						
						uVal8 = *pVal8++; // 0.25 step
						if(flag32 & cbits32[flag_msb--])
						{	
						#ifdef __MAKER_ANTS__
							#ifdef __TYPE__PRU__ 
							uVal8 = conv_att_025_to_05(uVal8);	// Step 0.25 -> RF(0.5)
							Add_PAU_N_Ctrl_Data(amp, 0, 7, &uVal8);
							UseFwd_Out_Att(amp, uVal8);
							#else // AAU, RF ATT Offset
							m_Eep.DL_Att_Offs[amp] = uVal8;
							AAU_RF_DL_Att_Set(amp, m_St.DL_Att[amp]/2);
							#endif
						#endif
						}
					}
					break;

				case 0x37:	//// ATT UL, 0.25dB Step
					#ifdef __MAKER_ANTS__
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;					

						#ifdef __TYPE__PRU__
						uVal8 = *pVal8++; // 0.25 step
						if(flag32 & cbits32[flag_msb--])
						{	
							// not exist, it will be Digital att(registar)
							//--------------------------------
							if(m_St.Rvs_IN_ALC_Onoff[amp] == OFF)
							{
								uVal8 = conv_att_025_to_05(uVal8);	// Step 0.25 -> (0.5)
								Add_PAU_N_Ctrl_Data(amp, 1, 7, &uVal8); // pass as big-endian
								m_PAU_C_Req[amp].Rvs_Att_x2 = uVal8;
								m_Eep.AMP_Rvs_Att_x2[amp] = uVal8;
							}
							//--------------------------------							
						}
						#else // AAU : UL_AFE_ATT : 2019.08.13			
						uVal8 = *pVal8++; // 0.25 step
						if(flag32 & cbits32[flag_msb--])
						{
							m_Eep.UL_AFE_ATT_x1[amp] = uVal8/4; // 0.25[dB] step -> 1[dB] step
							AAU_UL_AFE_Att_Set(amp, m_Eep.UL_AFE_ATT_x1[amp]);
						}
						#endif
					}
					#endif
					break;
								
				case 0x45:
					for(l = 0; l < 2; l++)
					{
						uVal8 = *pVal8 & 0x01; 							
						if(flag32 & cbits32[flag_msb--])
							m_St.d_BIP_Clr[l] = uVal8;
						pVal8++;
					}
					break;

				case 0x48:
					if(flag_msb < 0) break;
					m_Eep.DC_Fail_Volt_x10[DC__48V] = ntohs(*pVal16);
					break;
					
				// 전압(DTU) Main 전원 DC Fail 설정값
				case 0x49:		
					if(flag_msb < 0) break;
					m_Eep.DC_Fail_Volt_x10[DC_Main] = ntohs(*pVal16);
					break;

				case 0x4A: // AFE ALC Level(UL)
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.AFE_Rvs_ALC_x10[amp] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;

				#ifdef __TYPE__AAU__ 
				case 0x4B: // DL ALC On/Off(UL), 2019.05.13
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							if(*pVal8 == ON) uVal8 = ON;
							else  uVal8 = OFF;
							
							m_Eep.AAU_Fwd_ALC_Onoff[amp] = uVal8;
						}
						pVal8++; // next
					}					
					break;
				#endif	
					
				case 0x4C: // 2019.05.13, 0xC04C, AAU-ANTS Only
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.AAU_Fwd_ALC_x10[amp] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;	
					
				case 0x4D: // 2019.05.13, 0xC04D, AAU-ANTS Only
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.AAU_Fwd_SD__x10[amp] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;


				#ifdef __TYPE__AAU__ 
				case 0x50: // DL SD On/Off(UL), 2019.05.13
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							if(*pVal8 == ON) uVal8 = ON;
							else  uVal8 = OFF;
							
							m_Eep.AAU_Fwd_SD__Onoff[amp] = uVal8;
						}
						pVal8++; // next
					}					
					break;

				case 0x51: // 2019.05.15
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.DL_FB_Offset_x10[c][l] = sVal16;
							}
						}
					}
					break;

				case 0x54: // 2019.05.16
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
					{
						uVal8 = *pVal8;
						if(uVal8 > 1) break;
						AAU__Gain_Check_Mode_Ctrl(uVal8);
					}
					break;
				#endif

				case 0x52: // 2019.05.15 
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.TSSI_Offset_x10[c][l] = sVal16;
								// 2019.07.12 : 
								TSSI_SD_Level_Set(c, l, (INT16S)m_Eep.DL_TSSI_UP_x10[c][l]);
							}
						}
					}
					break;

				case 0x55:
					memcpy(m_Env.test_ls_ip, pVal8, 4);
					memcpy(AufoConfigFlag.Recv_Ip, pVal8, 4);
					break;

				case 0x56:
					m_Env.debug_optic = pVal8[0];
					break;

				case 0x57:
					m_Env.debug_path4 = pVal8[0];
					break;

				case 0x58: // 2019.05.23, AAU-ANTS Only
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.UL_Gain_Low_x10[l] = (INT16S)uVal16;
							
							if(l==0)
								m_Eep.Threshold_UL_Gain_Low_x10 = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;		

				case 0x59: // 2019.05.23, AAU-ANTS Only
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						m_Eep.DL_Mode_En = pVal8[0] & 0x01;
						AAU__Gain_Check_Mode_dl_mode_Ctrl(m_Eep.DL_Mode_En);						
					}
					break;					

				case 0x5B: // 2019.05.27
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.Offset_RSSI_Before_Filter_x10[l] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;

				case 0x5C: // 2019.05.27
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							sVal16 = ntohs(*pVal16++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.UL_Noise_Offset_x10[c][l] = sVal16;
							}
						}
					}
					break;

				case 0x5D: // 2019.05.27
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.UL_GC_RSSI_Offset_x10[_100M][l] = (INT16S)uVal16;
							////m_Eep.UL_GC_RSSI_Offset_x10[Alpha][l] = 0;
						}
						pVal16++; // next 2byte
					}
					break;

				case 0x5E: // 2019.05.27
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
						m_Eep.AAU_48V_Offset_x10 = (INT16S)ntohs(*pVal16);
					break;

				/*
				2019.08.12 : AID 0xB60A/0B 의 중복으로 0xC05F "48V DC Fail limit" 삭제.
				case 0x5F: // 2019.05.27				
					for(l=0; l<2; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.AAU_48V_Limit_x10[l] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}					
					break;	
				*/	

				case 0x60:
					m_St.Gain_Noise_Auto = pVal8[0];
					break;
				
				case 0x69: 					
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.d_PAR_x10[l] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;

				case 0x6C: // 2019.08.06, Height Balnance On/Off
					m_Eep.Height_Bal_Onoff = pVal8[0] & 0x01;
					break;

				// 2019.08.13 : 파형감시 DL/UL Offset
				case 0x6D:
					for(l=0; l<DL_UL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{							
							uVal16 = ntohs(*pVal16);
							m_Eep.spectrum_offest_x10[l] = (INT16S)uVal16;
						}
						pVal16++; // next 2byte
					}
					break;

				#ifdef __TYPE__PRU__ 
				case 0x6E:	//// 2019.08.12 : PAU EQ State
					for(amp = 0; amp < AMPU_4_MAX; amp++)
					{
						if(flag_msb < 0) break;
						uVal8 = *pVal8++;
						if(flag32 & cbits32[flag_msb--])
							Add_PAU_N_Ctrl_Data(amp, 6, 4, &uVal8);
					}
					break;
				#endif	

				case 0x6F: // 2019.08.12
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<MAX_DL; l++)
						{
							if(flag_msb < 0) break;
							
							sVal32 = (INT32S)ntohl(*pVal32++); 
							if(flag32 & cbits32[flag_msb--])
							{
								m_Eep.TSSI_Dec_Offset[c][l] = sVal32;
							}
						}
					}
					break;

				case 0x70: // 2019.08.12
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
					{
						m_Eep.AAU_CW_Auto_Onoff = *pVal8; // 2020.01.20 added
					}
					break;

				case 0x74:	// 2019.08.22					
					for(c=0; c < MAX_FA_PATH; c++)
					{
						for(l=0; l<LSH_Max; l++)
						{
							if(flag_msb < 0) break;
							if(flag32 & cbits32[flag_msb--])
								m_Eep.d_t_pattern_gain_x10[c][l] = (INT16S)ntohs(*pVal16);
								
							pVal16++; // next 2byte
						}
					}
					break;

				case 0x76: // 2019.08.23
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb--])
						m_Eep.LS_Gain_Bal_OnOff = *pVal8 & 0x01;
					break;

			#ifdef __TYPE__PRU__	
				case 0x77:
				case 0x78:
				case 0x79:
				case 0x7A:
					     if(AID[1] == 0x77) c = BW_140;
					else if(AID[1] == 0x78) c = BW_160;
					else if(AID[1] == 0x79) c = BW_180;
					else                    c = BW_200;
					
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
						{
							sVal16 = ntohs(*pVal16);							
							m_Eep.BW_Gain_Comp_PRU_x10[c][l] = sVal16;
						}
						pVal16++; // next 2byte
					}
					break;
			#else
				case 0x7B:	// 2019.09.16
					for(l=0; l<MAX_DL; l++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])
							m_Eep.d_CFR_Offset_x10[l] = ntohs(*pVal16);
						
						pVal16++; // next 2byte
					}
					break;				

				case 0x7C:	// 2019.09.16
					for(c=0; c<LSH_Max; c++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])						
							m_Eep.d_DL_Det_SSB_Frame[c] = ntohs(*pVal16);
						
						pVal16++; // next 2byte
					}	
					break;
					
				case 0x7D:	// 2019.09.16
					for(c=0; c<LSH_Max; c++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])						
							m_Eep.d_DL_Det_SSB_Start[c] = ntohs(*pVal16);
						
						pVal16++; // next 2byte
					}		
					break;
					
				case 0x7E:	// 2019.09.16
					for(c=0; c<LSH_Max; c++)
					{
						if(flag_msb < 0) break;
						if(flag32 & cbits32[flag_msb--])						
							m_Eep.d_DL_Det_SSB_Lngth[c] = ntohs(*pVal16);
						
						pVal16++; // next 2byte
					}		
					break;		

				case 0x90: // L/S(Samsung) PLL Set, 2020.04.02
				case 0x91: 
				case 0x92: 
					     if(AID[1] == 0x90) c = LSH_SS;
					else if(AID[1] == 0x91) c = LSH_NK;
					else                    c = LSH_ER;

					for(l = 0; l < 4; l++) // 4-cascade
					{
						if(flag_msb < 0) break;
						uVal8 = *pVal8++;
						if(flag32 & cbits32[flag_msb--])
							m_Eep.d_Reg_PLL_Set[c][l] = uVal8;
					}
					break;	
			#endif
				case 0xA0:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						memcpy(m_ping_test.ip, pVal8, 4);
					break;

				case 0xA1:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						m_ping_test.cnt16 = ntohs(*pVal16);
					break;

				case 0xA2:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
						m_ping_test.size16 = ntohs(*pVal16);
					break;

				case 0xA3:
					if(flag_msb < 0) break;
					if(flag32 & cbits32[flag_msb])
					{
						uVal8 = *pVal8;
						if((uVal8 == 0)||(uVal8 == 1))
						{
							if((m_ping_test.on == 2)&&(uVal8 == 1)) // now in progress, control on
							{
								m_ping_test.on = 2; // in progress
							}
							else if((m_ping_test.on == 0)&&(uVal8 == 0)) // now off, control off
							{
								m_ping_test.result[0] = '-'; // clear
								m_ping_test.result[1] = '\0'; // NULL
							}
							else
							{
								m_ping_test.on = uVal8;
							}
						}
					}
					break;	
				
				case 0xC0: // 2019.08.06, Factory Mode On/Off
					if(pVal8[0]==0)
						Set_Factory_Mode_Onoff(OFF);
					else	
						Set_Factory_Mode_Onoff(ON);
					break;
					

				case 0xF0: 
					uVal16 = ntohs(*pVal16); 
					if((uVal16 > dbg___None)&&(uVal16 < dbg___MAX))
						m_Env.debug__cmd = uVal16;
					break;
					
				case 0xF1:
					if((m_Env.debug__cmd > dbg___None)&&(m_Env.debug__cmd < dbg___MAX))
					{
						for(c=0; c < dbg_2MAX; c++)
						{
							for(l=0; l<dbg_4MAX; l++)
							{
								if(flag_msb < 0) break;
								
								sVal32 = (INT32S)ntohl(*pVal32++); 
								if(flag32 & cbits32[flag_msb--])
								{
									m_Env.debug_data[m_Env.debug__cmd][c][l] = sVal32;
								}
							}
						}
					}
					break;

				//--------------
				default:break;
				//--------------				
			}
			break;

		//---------------------------------------------------------------
		case 0xD4:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x54: // log count, 2020.02.11
					if(flag_msb < 0) break;
					// 제어시 count 0으로 초기화
					m_St.Log_Cnt = Remove_all_AlmLog_Files();
					break;
					
				//--------------
				default: break;
				//--------------
			}
			break;			
		//---------------------------------------------------------------
		default: break;
		//---------------------------------------------------------------
	}
	return tot__len;
}


INT8U LocalCtrl( INT8U* data, INT16U tot_len, INT8U cmd )
{
	INT8U ret = 1;
	INT16U cnt=0;
	INT8U i;
	INT8U path4;
	

	//----------------------------------
	cnt = 0; // init

	for(i=0; i<AMPU_4_MAX; i++)
		Init_PAU_N_Ctrl( i );
		
	//----------------------------------
	//D_Printf(DEBUG_SYSTEM, "[LocalCtrl] start, len %d.", tot_len);
	//----------------------------------

	Wr__Event_to_LogFile("[Control REQ] Command 0x%02X", cmd);

    while(cnt < tot_len)
    {
		cnt += AID_Control( &data[cnt], cmd );
	}	
		
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(m_Env.Use_Not_Req[path4])
		{
			m_Env.Use_Not_Req[path4] = 0;
			
			if(path4 != 0xFF)
			{
				if( Check_PAU_Use( path4 ) == 0) 
				{
					if(PAU_Link_Fail_Co[path4]==0) 
					{
						sendPAU_EnDis(path4, _DIS_); // PRU, 미사용 off 제어, reason 불필요.
						Rvs_Amp_OnOff(path4, OFF); // PRU,AAU
						m_St.Pwr_Amp_Link_Err[path4] = 0;
					}
				}
			}			
		}
	}	
	//----------------------------------
	//----------------------------------
	for(i=0; i<AMPU_4_MAX; i++) 
		Send_PAU_N_Ctrl( i );	
	//----------------------------------

	m_Env.is_eep_write = EEP_WRITE_CODE;

	User_Ctrl_Req = 1;	

	//D_Printf(DEBUG_SYSTEM, "[LocalCtrl] end.");
	
	return ret;
}
//------------------------------------------------------------------------------------

void DL_CW_Level_Ctrl(INT8U c, INT8U l, INT16U level)
{
	if(c >= MAX_FA_PATH) return;
	if(l >= MAX_MIMO_PATH) return;

	m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 = 1;
	m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 = level;
}

// 2019.05.16
void Use_DL_CW_Level_Ctrl(INT8U c, INT8U l, INT16U level)
{
	if(c >= MAX_FA_PATH) return;
	if(l >= MAX_MIMO_PATH) return;

	DL_CW_Level_Ctrl(c, l, level);

	#ifdef __TYPE__PRU__
	m_Eep.d_DL_CW_LEVEL[c][l] = level; 
	#else 
	//m_Eep.d_DL_CW_LEVEL[c][l] = 0; // default 0, 2019.06.13.
	m_Eep.d_DL_CW_LEVEL[c][l] = level; // 2019.09.25.
	#endif
}
//------------------------------------------------------------------------------------
// 2019.05.16
void Use_DL_CW_OnOff_Ctrl(INT8U c, INT8U l, INT8U onoff) // '0' is OFF (both AID and Register)
{
	if(c >= MAX_FA_PATH) return;
	if(l >= MAX_MIMO_PATH) return;
	if(onoff > 1) return; // 0(ON),1(FOff)

	m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_CW_On.OnOff = 1;
	m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_CW_On.OnOff = onoff;
}
//------------------------------------------------------------------------------------
void Use_Auto_SD_Mode_En(INT8U path, INT8U SD_Mode)
{
	m_Eep.Auto_SD_Mode_En[path] = SD_Mode & 0x01;
	#ifdef __TYPE__AAU__
	m_St.AMP_Auto_SD_Mode_En[path] = SD_Mode & 0x01;
	#endif
}
//------------------------------------------------------------------------------------
void Use_FAN_Auto_Ctrl( INT8U idx, INT8U val ) //
{
	if (idx > 1) return;	

	if(idx == EXT_FAN) m_St.FAN_Auto[idx] = val & 0x01;
	m_Eep.FAN_Auto[idx] = val & 0x01;

	if(idx == INT_FAN)
	{
		f_Req.FAN_Auto[idx] = 1;
		m_Req.FAN_Auto[idx] = val & 0x01;		
	}
}
//------------------------------------------------------------------------------------
// PRU-PSU-Internal FAN
// Internal/External FAN, 
void Use_PSU_INT_FAN_Onoff_Ctrl(INT8U val) //
{
	val &= 0x01;	
	m_Eep.FAN_OnOff[INT_FAN] = val;	
	
	f_Req.FAN_OnOff[INT_FAN] = 1;
	m_Req.FAN_OnOff[INT_FAN] = val;	
}

void Use_PSU_Ext_FAN_Onoff_Ctrl(INT8U val)
{
	val &= 0x01;
	m_Eep.FAN_OnOff[EXT_FAN] = val;

	PSU_Ext_FAN_Onoff_Ctrl(val);
}

void Use_Ext_FAN_Speed_Ctrl( INT8U speed ) // PRU, AAU
{	
	if(speed > 100) speed = 100;

#ifdef __TYPE__PRU__
	m_Eep.Ext_FAN_Speed = speed;
	////m_PSU.desire_Fan_Speed = speed;

	if(speed == 0) m_Eep.FAN_OnOff[EXT_FAN] = OFF; // Use_PSU_Ext_FAN_Onoff_Ctrl(1);	// Off
	else           m_Eep.FAN_OnOff[EXT_FAN] = ON;  // Use_PSU_Ext_FAN_Onoff_Ctrl(0);	// ON
#else // AAU
	m_Eep.Ext_FAN_Speed = speed;
#endif
}
//------------------------------------------------------------------------------------
void PSU_Ext_FAN_Onoff_Ctrl( INT8U val ) // PRU-PSU-FAN
{
	val &= 0x01;		
	m_St.FAN_OnOff[EXT_FAN] = val;
	
	if (val == OFF) 
	{
		if(m_PSU.Fan_Speed != 0)
			PSU_FAN_Speed_Ctrl(0);
	}
	else            
	{
		if( m_PSU.desire_Fan_Speed != m_PSU.Fan_Speed )
		{
			D_Printf( DEBUG_SYSTEM, "PSU Ext-FAN Speed Ctrl, %d != %d",
					m_PSU.desire_Fan_Speed, m_PSU.Fan_Speed);
			
			PSU_FAN_Speed_Ctrl(m_PSU.desire_Fan_Speed);
		}
	}		
}
//------------------------------------------------------------------------------------
void Use_FAN_On__Temp_Ctrl( INT8U idx, INT8S val )
{
	if(idx > 1) return;	

	if(val < 0) val = 0;

	if(idx == EXT_FAN) m_St.FAN_On_Temp[idx] = val;
	m_Eep.FAN_On_Temp[idx] = val;

	if(idx == INT_FAN)
	{
		f_Req.FAN_On_Temp[idx] = 1;
		m_Req.FAN_On_Temp[idx] = val;
	}
}
//------------------------------------------------------------------------------------
void Use_FAN_Off_Temp_Ctrl( INT8U idx, INT8S val )
{
	if(idx > 1) return;	

	if(val < 0) val = 0;

	if(idx == EXT_FAN) m_St.FAN_OffTemp[idx] = val;
	m_Eep.FAN_OffTemp[idx] = val;

	if(idx == INT_FAN)
	{
		f_Req.FAN_OffTemp[idx] = 1;
		m_Req.FAN_OffTemp[idx] = val;
	}
}
//------------------------------------------------------------------------------------
#define MAX_OUT_PWR_LMT_x10 (1000)
#define Min_OUT_PWR_LMT_x10 (-1000)

void Use_Fwd_Out_Pwr_Lmt_Set(INT8U p, INT8U upDn, INT16S lmt_x10)
{
	if (p >= AMPU_4_MAX) return;
	if ((upDn != LO) && (upDn != HI)) return;	

	if (lmt_x10 > MAX_OUT_PWR_LMT_x10) lmt_x10 = MAX_OUT_PWR_LMT_x10;
	else if (lmt_x10 < Min_OUT_PWR_LMT_x10) lmt_x10 = Min_OUT_PWR_LMT_x10;

	m_Eep.Fwd_Out_Lmt_x10[p][upDn] = lmt_x10;
}
//----------------------------------------------------------------------
// when RCU_Init or User off Ctrl : 
void Use_Green_Mode(INT8U Green_Mode_On)
{
	Green_Mode_On &= 0x01;
	
	m_Eep.Green_Mode_On = Green_Mode_On;	

	if(Green_Mode_On != ON) Green_Mode_Ctrl( OFF );
}

//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// idx : (0 : Start, 1 : Stop)
void Use_Green_Mode_Time(INT8U idx, INT8U* pTime)
{
	if (idx>1) return;
	
	if (idx==0)
	{
		m_St.Green_Mode_Start_Time[0] = pTime[0];
		m_St.Green_Mode_Start_Time[1] = pTime[1];

		m_Eep.Green_Mode_Start_Time[0] = pTime[0];
		m_Eep.Green_Mode_Start_Time[1] = pTime[1];
	}
	else
	{
		m_St.Green_Mode_Stop_Time[0] = pTime[0];
		m_St.Green_Mode_Stop_Time[1] = pTime[1];

		m_Eep.Green_Mode_Stop_Time[0] = pTime[0];
		m_Eep.Green_Mode_Stop_Time[1] = pTime[1];
	}	
}
//------------------------------------------------------------------------------------
// idx : (0 : Start, 1 : Stop)
void Use_AMP_Stdby_Mode_Time(INT8U idx, INT8U hour, INT8U min)
{
	INT8U time[2];

	if (idx>1) return;

	time[0] = hour;
	time[1] = min;

	if (idx==0)
	{
		if(time[0]>23) time[0] = 3; // hour
		if(time[1]>59) time[1] = 0; // min

		m_St.AMP_Stdby_Start_Time[0] = time[0];
		m_St.AMP_Stdby_Start_Time[1] = time[1];

		m_Eep.AMP_Stdby_Start_Time[0] = time[0];
		m_Eep.AMP_Stdby_Start_Time[1] = time[1];
	}
	else
	{
		if(time[0]>23) time[0] = 5; // hour
		if(time[1]>59) time[1] = 0; // min

		m_St.AMP_Stdby_Stop_Time[0] = time[0];
		m_St.AMP_Stdby_Stop_Time[1] = time[1];

		m_Eep.AMP_Stdby_Stop_Time[0] = time[0];
		m_Eep.AMP_Stdby_Stop_Time[1] = time[1];
	}	
}

// idx : (0 : Start, 1 : Stop)
void Use_MIMO_Stdby_Mode_Time(INT8U idx, INT8U hour, INT8U min)
{
	INT8U time[2];

	if (idx>1) return;

	time[0] = hour;
	time[1] = min;

	if (idx==0)
	{
		if(time[0]>23) time[0] = 3; // hour
		if(time[1]>59) time[1] = 0; // min

		m_St.MiMo_Stdby_Start_Time[0] = time[0];
		m_St.MiMo_Stdby_Start_Time[1] = time[1];

		m_Eep.MiMo_Stdby_Start_Time[0] = time[0];
		m_Eep.MiMo_Stdby_Start_Time[1] = time[1];
	}
	else
	{
		if(time[0]>23) time[0] = 5; // hour
		if(time[1]>59) time[1] = 0; // min

		m_St.MiMo_Stdby_Stop_Time[0] = time[0];
		m_St.MiMo_Stdby_Stop_Time[1] = time[1];

		m_Eep.MiMo_Stdby_Stop_Time[0] = time[0];
		m_Eep.MiMo_Stdby_Stop_Time[1] = time[1];
	}	
}

// idx : (0 : Start, 1 : Stop)
void Use_FAN_Mode_Time(INT8U idx, INT8U hour, INT8U min)
{
	INT8U time[2];

	if (idx>1) return;

	time[0] = hour;
	time[1] = min;

	if (idx==0)
	{
		if(time[0]>23) time[0] = 3; // hour
		if(time[1]>59) time[1] = 0; // min

		m_St.FAN_Mode_Start_Time[0] = time[0];
		m_St.FAN_Mode_Start_Time[1] = time[1];

		m_Eep.FAN_Mode_Start_Time[0] = time[0];
		m_Eep.FAN_Mode_Start_Time[1] = time[1];
	}
	else
	{
		if(time[0]>23) time[0] = 5; // hour
		if(time[1]>59) time[1] = 0; // min

		m_St.FAN_Mode_Stop__Time[0] = time[0];
		m_St.FAN_Mode_Stop__Time[1] = time[1];

		m_Eep.FAN_Mode_Stop__Time[0] = time[0];
		m_Eep.FAN_Mode_Stop__Time[1] = time[1];
	}	
}



//-------------------------------------------------------
void Use_TempLmt(INT8S TempLmt)
{		
	if(TempLmt == 100)
	{
		Disp_Reg_Address();
		return;
	}	

	m_St.TempLmt = TempLmt;
	m_Eep.TempLmt = TempLmt;	
}

void Use_TempLmt_x10(INT16S TempLmt_x10)
{
	m_Eep.TempLmt_x10 = TempLmt_x10;	
}

//-------------------------------------------------------
//-------------------------------------------------------
void Wr_BarCode( INT8U* pBarCode_Info )
{
	INT8U len;

	len = pBarCode_Info[0];

	if( len > BarCode_Name_MAX ) len = BarCode_Name_MAX;
		
	m_Eep.BarCode_len = len;

	memset( m_Eep.BarCode_Name, 0, BarCode_MAX );	
	memcpy( m_Eep.BarCode_Name, &pBarCode_Info[1], len );
	m_Eep.BarCode_Name[BarCode_Name_MAX-1] = 0; // null-char
}
//-------------------------------------------------------
void Chk_BarCode(void)
{
	if(m_Eep.BarCode_len > BarCode_Name_MAX)
	{
		memset(m_Eep.BarCode_Name, 0, BarCode_Name_MAX);
		m_Eep.BarCode_len = 1; // null-char
	}	
}
//-------------------------------------------------------
void Wr_Node_Name( INT8U* pName, INT8U len )
{
	if( len <= Node_Name_MAX )
	{
		m_Eep.Node_Name_len = len;
		
		memset( m_Eep.Node_Name, 0, Node_Name_MAX );
		memcpy( m_Eep.Node_Name, pName, len );
	}
}
//-------------------------------------------------------
void Chk_Node_Name( void )
{
	if(m_Eep.Node_Name_len > Node_Name_MAX)
	{
		m_Eep.Node_Name_len = 0;
		memset( m_Eep.Node_Name, 0, Node_Name_MAX );
	}
}
//-------------------------------------------------------



//-------------------------------------------------------
// Get Sub-Pwr-switch status
INT8U Get_PAU_Sw_St(INT8U path4, INT8U* psw_idx)
{
	INT8U bit = 0;

	switch(path4)
	{
		case AMPU_4_1: bit = 7; break;
		case AMPU_4_2: bit = 6; break;
		case AMPU_4_3: bit = 5; break;
		case AMPU_4_4: bit = 4; break;

		default:
			return OFF;
	}
	*psw_idx = bit;	
	if( m_PSU.PAU_Switch_St & cbits[bit] )
		return OFF; // OFF
	else 
		return ON; // ON
}
//-------------------------------------------------------
void Use_PwrAmp_Reset( INT8U path4, INT8U data )
{	
	if(data != 1) return;		
	if(Check_PAU_Use(path4)==0) return;
	
	m_Env.Pwr_AMP_Rst_Req[path4] = 1;
	m_Env.User_Pwr_AMP_Rst[path4] = 1;
}

#ifdef __TYPE__PRU__
void Use_PRU_PwrAmp_OnOff(INT8U path4, INT8U data )
{
	if( Check_PAU_Use( path4 )==0 ) return;	
	if (data != ON) data = OFF;

#if 0 // 5G, later below used...
/*----------------------------------------------------------
	////// AMP Stdby mode 일 때, amp user 제어 안함.
	////// MIMO Stdby mode 일 때, amp user 제어 안함.
	////// AC Fail일 때, amp user 제어 안함. 
	////Mimo_Stdby_AMP_Ctrl_Chk();
	////AMP_Stdby_AMP_Ctrl_Chk();
	////// 제어하려는 amp 가 mimo 이면 아래 check				
	////	
	////if(Mimo_Stdby_Mode_St == ON) return;
	////if(AC_Fail_St != _AC_NORMAL_) return;	
	////if(AMP_Stdby_Mode_St_Sum == ON) // onoff && time
	////{		
	////	if(Amp_Stdby_Use(path4)) return;
	////}		
	////
	////if (data == ON)
	////{
	////	if(Get_PAU_Sw_St(path4, &sw_idx) == OFF) return;
	////	ClearFwdSD_Mode(path4);
	////	// Hard-Clip-Count-clear When AMP-ON 
	////	m_Env.HardClip_Cnt_Clr_Req = 1;
	////}
---------------------------------------------------------
*/
#endif	

	PRU_User_AMP_EN_Ctrl(path4, data); // 2020.04.01
	
	m_Eep.Pwr_Amp_En[path4] = data;
}
#endif

void Fwd_Out_Att_Set_Req(INT8U path4, INT8U data)
{
	if(data > MAX_FWD_ATT_x2) 
		data = MAX_FWD_ATT_x2;
		
	m_PAU_C_Req[path4].Fwd_Att_x2 = data;
}

void UseFwd_Out_Att(INT8U path4, INT8U data)
{
	if(data > MAX_FWD_ATT_x2) 
		data = MAX_FWD_ATT_x2;
	
	Fwd_Out_Att_Set(path4, data);
	m_Eep.Fwd_Out_Att_x2[path4] = data;	
}


void ATT_Pair_Onoff_Req(INT8U path4, INT8U onoff)
{
	m_PAU_C_Req[path4].Att_Pair = onoff & 0x01;
	m_PAU_C_Req[path4].Att_Pair_req = 1;	
	
	D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d Att-Pair OnOff(%d)", 
		path4+1, onoff );
}

void Use_ATT_Pair_Onoff(INT8U path4, INT8U onoff)
{
	m_Eep.ATT_Pair_Onoff[path4] = onoff & 0x01;
	
	ATT_Pair_Onoff_Req(path4, onoff & 0x01);
}

void FB_Att_Req( INT8U path4, INT8U att_x2 )
{
	m_PAU_C_Req[path4].FB_Att_x2 = att_x2;
	m_PAU_C_Req[path4].FB_Att_x2_req = 1;
	
	D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d FB_Att %d/2\n", 
		path4+1, att_x2 );
}


void Use_FB_Att( INT8U path4, INT8U att_x2 )
{
	m_Eep.FB_ATT_x2[path4] = att_x2;
	
	FB_Att_Req(path4, att_x2);
}


void FB_Offset_Att_Req( INT8U path4, INT8U offset_x2 )
{
	m_PAU_C_Req[path4].FB_Offset_x2 = offset_x2;
	m_PAU_C_Req[path4].FB_Offset_x2_req = 1;

	D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d FB_Offset Att %d/2\n", 
		path4+1, offset_x2 );
}






/*
--------------------------
FPGA : DPD Reset
------------------------
0 : DL #0
1 : DL #1
2 : DL #2
3 : DL #3
4 : Core reset
5 : DPD All Reset
*/

// 2019.05.01
void DPD_Reset_Ctrl(INT8U AID_Idx)
{	
#ifdef __TYPE__PRU__ // ANTS-PRU
	
	switch(AID_Idx)
	{	
		case 0x01: // DPD 전체
			m_Reg_Flg.DPD_All_Reset.uD32= _SET_;
			m_Reg_Ctl.DPD_All_Reset.uD32 = 0; // 0 : DPD Reset

			print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "DPD All Reset, Curr 0x%X", m_Reg_Sts.DPD_All_Reset.uD32);
			break;	

		case 0x02: // DPD Core
			m_Reg_Flg.DPD_Core_Reset.uD32= _SET_;
			m_Reg_Ctl.DPD_Core_Reset.uD32 = 0; // 0 : DPD Reset

			print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "DPD Core Reset, Curr 0x%X", m_Reg_Sts.DPD_Core_Reset.uD32);
			break;			
			
		case 0x03: // DPD DCL(DL0)
		case 0x04: // DPD DCL(DL1)
		case 0x05: // DPD DCL(DL2)
		case 0x06: // DPD DCL(DL3)
			if(Check_PAU_Use(AID_Idx-0x03)) // 2019.12.11
			{			
				m_Reg_Flg.DPD[AID_Idx-0x03].DPD_Reset.uD32 = _SET_;
				m_Reg_Ctl.DPD[AID_Idx-0x03].DPD_Reset.uD32 = 0; // 0 : DPD Reset

				m_Env.DPD_Rst_Normal_Chk_Cnt[AID_Idx-0x03] = DPD_Rst_Chk_MAX_Cnt;

				print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "DPD#%d Reset, Curr 0x%X", 
					AID_Idx-0x03+1, m_Reg_Sts.DPD[AID_Idx-0x03].DPD_Reset.uD32);
			}
			else
			{
				m_Reg_Ctl.DPD[AID_Idx-0x03].DPD_Reset.uD32 = 1; // 1 : DPD Normal
			}
			
			break;	

		default:
			break;
	}
#endif	
}


// 2019.05.01
/*
	After DPD-Reset(0x00) by user/algorithm,
	Control Register with Normal-value(0x01)	
*/
void Proc___DPD_Reset_to_Normal(void)
{
#ifdef __TYPE__PRU__ // ANTS-PRU
	INT8U path4;
	static INT8U conti_cnt[MAX_DL] = {0,0,0,0};
	
	for(path4 = DL_0; path4 < MAX_DL; path4++)
	{
		// 2019.11.05		
		if(m_St.Pwr_Amp_En[path4] == ON)
		{
			conti_cnt[path4] = 0;
		}
		else // AMP_OFF
		{				
			if(m_Reg_Sts.DPD[path4].DPD_Reset.uD32 == 0) // 0 : RESET
			{	
				conti_cnt[path4]++;
				if(conti_cnt[path4] > 5)
				{
					conti_cnt[path4] = 10; // max
					Write_FPGA_Reg(Reg_DPD_Addr[path4][Reg_DPD_RESET], 1, "Reg_DPD_RESET");
					print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "AMP_OFF, DPD#%d Normal Set", path4+1);
				}
				else
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[DPD#%d] amp_off conti_cnt %d", path4+1, conti_cnt[path4]);
				}
			}
			else conti_cnt[path4] = 0;
			continue;
		}
		
		if(m_Env.DPD_Rst_Normal_Chk_Cnt[path4] == 0) continue;
		
		if(m_Reg_Sts.DPD[path4].DPD_Reset.uD32 == 0) // 0 : RESET
		{				
			if(m_Env.DPD_Rst_Normal_Chk_Cnt[path4] > 0)
			{
				m_Env.DPD_Rst_Normal_Chk_Cnt[path4]--;

				/* 2019.12.04 : 미사용 Layer에 DPD Reset 하지 않도록 수정. */
				if(Check_PAU_Use(path4))
				{
					Write_FPGA_Reg(Reg_DPD_Addr[path4][Reg_DPD_RESET], 0, "Reg_DPD_RESET"); // 0 reset
					/*
					//print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "DPD#%d Chk_Cnt %d", 
					//			path4+1, m_Env.DPD_Rst_Normal_Chk_Cnt[path4]);
					*/			
				}
				
				if(m_Env.DPD_Rst_Normal_Chk_Cnt[path4] == 0)
				{
					Write_FPGA_Reg(Reg_DPD_Addr[path4][Reg_DPD_RESET], 1, "Reg_DPD_RESET");
					
					//m_Reg_Flg.DPD[path4].DPD_Reset.uD32 = _SET_;
					//m_Reg_Ctl.DPD[path4].DPD_Reset.uD32 = 1; // 1 : DPD Normal

					print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "DPD#%d Reset to Normal", path4+1);
				}
			}
		}
	}

	if(m_Reg_Sts.DPD_Core_Reset.uD32 == 0) // 0 : RESET
	{
		m_Reg_Flg.DPD_Core_Reset.uD32 = _SET_;
		m_Reg_Ctl.DPD_Core_Reset.uD32 = 1; // 1 : DPD Normal
		
		print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "DPD Core Reset to Normal");
	}

	if(m_Reg_Sts.DPD_All_Reset.uD32 == 0) // 0 : RESET
	{
		m_Reg_Flg.DPD_All_Reset.uD32 = _SET_;
		m_Reg_Ctl.DPD_All_Reset.uD32 = 1; // 1 : DPD Normal

		print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "DPD All Reset to Normal");
	}
#endif	
}




/*
--------------------------------------------------------------------------

	TEST 
	CIM
	When PRU(ANTS)

--------------------------------------------------------------------------
*/

void Init_TestPort_CIM(void)
{
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU
	/*	
	2019.04.06 : F/W restart 시, off 로 시작되도록 수정	
	//m_St.TestPort_CIM = 0xFF; // default All Off	
	*/		
	/*	
	2019.09.20 : Flash memory에 user 제어 저장되게 수정.
	*/
	m_St.TestPort_CIM = m_Eep.TestPort_CIM;
	Use_TestPort_CIM(m_St.TestPort_CIM);
#endif
}

void Use_TestPort_CIM(INT8U cim_ctrl)
{
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU

	m_St.TestPort_CIM = cim_ctrl & 0x0F;
	/*		
	2019.09.20 : Flash memory에 user 제어 저장되게 수정.
	*/
	m_Eep.TestPort_CIM = m_St.TestPort_CIM;
	
	if(m_St.TestPort_CIM & cbits[0]) CIM_0_Ctrl_L(); // OFF, Close
	else                             CIM_0_Ctrl_H(); // ON,  Pass
	
	if(m_St.TestPort_CIM & cbits[1]) CIM_1_Ctrl_L(); // OFF, Close
	else                             CIM_1_Ctrl_H(); // ON,  Pass
	
	if(m_St.TestPort_CIM & cbits[2]) CIM_2_Ctrl_L(); // OFF, Close
	else                             CIM_2_Ctrl_H(); // ON,  Pass
	
	if(m_St.TestPort_CIM & cbits[3]) CIM_3_Ctrl_L(); // OFF, Close
	else                             CIM_3_Ctrl_H(); // ON,  Pass

#endif
}

/*
--------------------------------------------------------------------------

	DTU Register
	Control
	Funcitons

--------------------------------------------------------------------------
*/
//-------------------------------------------------------------------------
void Use_d_HC_Threshold(INT8U path, INT16U level)
{
	m_Eep.d_Hard_Clip_Lvl_x10[path] = level;
	
	// PRU : 2019.05.31
	m_Reg_Flg.DSP_Common_MIMO[path].HC_Threshold.value = _F_SET_;
	m_Reg_Ctl.DSP_Common_MIMO[path].HC_Threshold.value = level;	
}
/*
	2019.09.20 : 
	AAU Hard Clipping Level control 기능 변경 : 
	(1) DTU_reg(0x10100) = power(10,(level+70)/20)
		reg = 10^((level+70)/20)
		log(reg) = (level+70)/20 
		10*(20*log(reg) - 70) = level_x10
	(2) Clipping OFF 시, DTU_reg = 0x7FFF	
*/
void Use_d_HC_Threshold_AAU(INT8U path, INT16U level)
{
	double d_level;
	INT16U reg_val16;
	
	if(m_Eep.d_Hard_Clip_OnOff[path] == ON)
	{
		// max 20.0 dB 제한. (register 0x7FFF 가 max 이므로)
		if(level > 200) level = 200; 
		
		m_Eep.d_Hard_Clip_Lvl_x10[path] = level;
		d_level = (70.0 + (double)level/10.0)/20.0;
		reg_val16 = (INT16U)pow(10.0, d_level);

		print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Use_d_HC_Threshold_AAU] Path(%d) level(%d, %f) reg 0x%X", 
				path, level, d_level, reg_val16);
		

		m_Reg_Flg.DSP_Common_MIMO[path].HC_Threshold.value = _F_SET_;
		m_Reg_Ctl.DSP_Common_MIMO[path].HC_Threshold.value = reg_val16;	
	}
	else
	{
		m_Reg_Flg.DSP_Common_MIMO[path].HC_Threshold.value = _F_SET_;
		m_Reg_Ctl.DSP_Common_MIMO[path].HC_Threshold.value = 0x7FFF;	
	}	
}







//--------------------------------------------------------------------------
// DL/UL same value control
// AID 0xB41E

static OS_TIMER timer_Cpri__Alarm[CPRI_PATH]; 
static OS_TIMER timer_Cpri_Normal[CPRI_PATH];

INT8U Proc__Cpri_Alarm_Keep_Xsec(INT8U cpri, INT8U alarm)
{
	// 2019.06.18
	static INT8U alarm_last[CPRI_PATH] = {1,1};
	static INT8U init[CPRI_PATH] = {0,0};

	if(cpri >= CPRI_PATH) return 0;
	
	if(init[cpri] == 0)
	{
		OSTimerReset(&timer_Cpri__Alarm[cpri]);
		OSTimerReset(&timer_Cpri_Normal[cpri]);
		alarm_last[cpri] = 0; // normal
		init[cpri] = 1;
	}
	else
	{	
		if(alarm) // check alrm 2[s]
		{
			//if(OSTimerGet_msec(&timer_Cpri__Alarm[cpri]) > (1*100))
			if(OSTimerGet_msec(&timer_Cpri__Alarm[cpri]) > (2*TICK_CO_SEC))
			{
				alarm_last[cpri] = 1;
				OSTimerReset( &timer_Cpri__Alarm[cpri] );
			}
			OSTimerReset( &timer_Cpri_Normal[cpri] );
		}
		else // check normal 10[s]
		{
			if(OSTimerGet_msec(&timer_Cpri_Normal[cpri]) > (10*TICK_CO_SEC))
			{
				alarm_last[cpri] = 0;
				OSTimerReset( &timer_Cpri_Normal[cpri] );
			}
			OSTimerReset(&timer_Cpri__Alarm[cpri]);
		}
	}
	return alarm_last[cpri];
}
//--------------------------------------------------------------------------
static OS_TIMER timer_Clk_Alarm;
INT8U Proc__Clock_Normal_Keep_Xsec(INT8U alarm)
{
	// 2019.06.18
	static INT8U alarm_last = 0;
	static INT8U init = 0;

	if(init == 0)
	{
		OSTimerReset( &timer_Clk_Alarm );
		alarm_last = alarm;
		init = 1;
		return alarm_last;
	}
	
	if(alarm)
	{
		OSTimerReset( &timer_Clk_Alarm );
		alarm_last = 1;
	}
	else // check normal
	{
		if(OSTimerGet_msec(&timer_Clk_Alarm) > (10*TICK_CO_SEC))
		{
			alarm_last = 0;
			OSTimerReset( &timer_Clk_Alarm );
		}			
	}
	return alarm_last;
}

/*
1.	VSS Table 접근 룰
	A.	VSS Table 접근전 CPRI Status(0x00)의 정보가 0x0F인 경우에만 Read 한다.
	B.	CPRI 알람의 경우 unlock/alarm 등의 정보가 2초 유지(ANTS 자체 규격)로 되어 있기 때문에 
	(정상에서 알람 발생 시점인 2초 이전 중에 VSS Table
	을 접근하면 오동작을 할 수 있음을 방지하기 위함)
	
2.	CPRI 알람/해제 조건
	A.	CPRI 알람은 초기 부팅시 unlock 상태 정보로 유지한다.
	B.	부팅시 최초 1회는 알람에서 정상 복귀 조건을 정상 상태가 2초 유지로 한다.
	C.	이후 운영 중에는 규격서상 10초로 한다.
	D.	CPRI 알람 발생은 장애 조건 2초 유지시로 발생한다.

*/


/*
	(1)	Band witdh(이하 BW) 정보의 경우 기본적으로 VSS Channel Z.81.0 필드를 참조, 
		총 10회(100ms 주기,1초 유지시) 연속 동일값 유지시 변경으로 인지하며,
		해당 정보는 System Main BW 정보로 반영한다(Main AID 정보에 반영)
		
	(2)	변경된 BW 정보는 내부에 Backup : m_Eep.d_RE_Channel_BW[_100M/Alpha]
		부팅시 BW 정보는 이전에 backup한 값을 사용한다.
	
	(3).	BW가 변경되면 Center Frequency는 해당 BW의 Default 값으로 변경, 
	Center Frequency 이 값도 같이 변경하며 설정값도 변경한다.
	(BW 정보 중 0(미사용), 1(10M), 2(20M) 의 값 : 0(미사용) 으로 간주.)
		
	(5)	CPRI 장애 발생시에는 해당 BW 정보를 0x0(미사용)으로 Register에 제어를 한다
	(불요파 제거를 위한 Channel Filter Off). 
	이때 System Main BW	는 VSS table과 동일하게 유지한다.
	
	6.	CPRI가 정상 상태인데 
	Z.81.0의 정보가 0x00, 0x11(10M/10M), 0x22(20M/20M)인 경우 마지막 상태 값을 유지하며, 
	Hidden 창에 있는 BW 정보 제어시 적용되도록 한다.
	m_Eep.d_RE_Channel_BW[_100M/Alpha] : Z.81.0의 정보가 0x00, 0x11(10M/10M), 0x22(20M/20M)인 경우 업데이트 안함.
	
	7.	BW 정보 중 0(미사용), 1(10M), 2(20M) 의 값은 미사용으로 간주
	system에서 필요시 10M와 20M BW에 대한 Center Frequency 정의가 필요

	BW 정보 중 0(미사용), 1(10M), 2(20M) 의 값 : 0(미사용) 으로 간주.

*/

/*
// 1 : AID control
// 2 : EEP Setting
// 3 : VSS
*/
void Use_d_RE_Channel_BandWidth(INT8U BW_100, INT8U BW___A, char call_info) // 6, 0
{
	INT8U Re_Ch_BW[MAX_Optic];
	INT8U alarm_St[MAX_Optic];
	INT8U sfp_insert[MAX_Optic];
	INT8U i;
	INT8U fa;
	INT8U curr; // current register value	
	INT8U BW_A_100;
	

	BW_A_100 = BW___A; BW_A_100 <<= 4;
	BW_A_100 |= BW_100;
	//---------------------------------------
	
	alarm_St[_100M] = m_St.AlmSt.A_2.CPRI_0__; // 2019.05.14
	alarm_St[Alpha] = m_St.AlmSt.A_2.CPRI_0_a;

	sfp_insert[_100M] = Is_SFP_Inserted(CPRI_0);
	sfp_insert[Alpha] = Is_SFP_Inserted(CPRI_1);	

	     if(alarm_St[_100M])      Re_Ch_BW[_100M] = BW___0M;
	else if(sfp_insert[_100M]==0) Re_Ch_BW[_100M] = BW___0M;
	else                          Re_Ch_BW[_100M] = BW_100; // FA#0, 6

	     if(alarm_St[Alpha])      Re_Ch_BW[Alpha] = BW___0M;
	else if(sfp_insert[Alpha]==0) Re_Ch_BW[Alpha] = BW___0M;
	else                          Re_Ch_BW[Alpha] = BW___A; // FA#1, 0

	//-------------------------------------------		
	if(Get_Factory_Mode()==ON)
	{
		Re_Ch_BW[_100M] = BW_100;
		Re_Ch_BW[Alpha] = BW___A;
	}	
	//-------------------------------------------	

	/*	
		Re_Ch_BW : CPRI 장애 발생시에는 해당 BW 정보를 0x0(미사용)으로 Register에 제어
		BW_100 / BW___A : 이때 System Main BW는 VSS table과 동일하게 유지
	*/

	//cnt++;
	
	if(IS_BW_Valid(BW_A_100, BW_100, BW___A))
	{		
		if((m_Eep.d_RE_Channel_BW[_100M] != BW_100)||(m_Eep.d_RE_Channel_BW[Alpha] != BW___A))
		{
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[RCU] RE_Ch_BW : 100(%d) A(%d) by %C", BW_100, BW___A, call_info);
			m_Env.is_eep_write = EEP_WRITE_CODE;
		}
		m_Eep.d_RE_Channel_BW[_100M] = BW_100;
		m_Eep.d_RE_Channel_BW[Alpha] = BW___A;
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_RED, " IS_BW Not Valid : all(0x%X) 100(%d) A(%d)", 
				BW_A_100, BW_100, BW___A);
		return; // if not valid, return.
	}

		
	#ifdef __MAKER_ANTS__		
	for(fa = 0; fa < MAX_Optic; fa++)
	{
		//------------------------------------
		curr = m_Reg_Sts.General_FA[fa].General_FA_MIMO[0].RW_DL_BW_Sel.BW;

		/*
		if(fa == Alpha)
		{
			if((cnt%10)==0)
			{
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[RCU] Register : RE_Ch_BW DL, Req %d,  Curr %d, by %C, BW___A %d\n", 
					Re_Ch_BW[fa], curr, call_info, BW___A); 			                    
			}
		}
		*/
				
		if(curr != Re_Ch_BW[fa])
		{
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[RCU] Register : RE_Ch_BW (FA%d) DL, Req %d != Curr %d, by %C", 
				fa, Re_Ch_BW[fa], curr, call_info); 			                    

			for(i = 0; i < MAX_DL; i++)
			{
				m_Reg_Flg.General_FA[fa].General_FA_MIMO[i].RW_DL_BW_Sel.BW = 1;
				m_Reg_Ctl.General_FA[fa].General_FA_MIMO[i].RW_DL_BW_Sel.BW = Re_Ch_BW[fa];
			}	
		}

		curr = m_Reg_Sts.General_FA[fa].General_FA_MIMO[0].RW_UL_BW_Sel.BW;
		if(curr != Re_Ch_BW[fa])
		{
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[RCU] Register : RE_Ch_BW (FA%d) UL, Req %d != Curr %d, by %C", 
				fa, Re_Ch_BW[fa], curr, call_info);			                    

			for(i = 0; i < MAX_DL; i++)
			{
				m_Reg_Flg.General_FA[fa].General_FA_MIMO[i].RW_UL_BW_Sel.BW = 1;
				m_Reg_Ctl.General_FA[fa].General_FA_MIMO[i].RW_UL_BW_Sel.BW = Re_Ch_BW[fa];
			}	
		}
		//------------------------------------		
	}
	Update_CenterFreq_from_BW();
	#endif	
}

// called when RCU Restart
void RE_Channel_BandWidth(INT8U BW_100, INT8U BW___A)
{
	INT8U i;
	
	for(i = 0; i < MAX_DL; i++)
	{
		m_Reg_Flg.General_FA[_100M].General_FA_MIMO[i].RW_DL_BW_Sel.BW = 1;
		m_Reg_Ctl.General_FA[_100M].General_FA_MIMO[i].RW_DL_BW_Sel.BW = BW_100;
		m_Reg_Flg.General_FA[_100M].General_FA_MIMO[i].RW_UL_BW_Sel.BW = 1;
		m_Reg_Ctl.General_FA[_100M].General_FA_MIMO[i].RW_UL_BW_Sel.BW = BW_100;
	
		m_Reg_Flg.General_FA[Alpha].General_FA_MIMO[i].RW_DL_BW_Sel.BW = 1;
		m_Reg_Ctl.General_FA[Alpha].General_FA_MIMO[i].RW_DL_BW_Sel.BW = BW___A;
		m_Reg_Flg.General_FA[Alpha].General_FA_MIMO[i].RW_UL_BW_Sel.BW = 1;
		m_Reg_Ctl.General_FA[Alpha].General_FA_MIMO[i].RW_UL_BW_Sel.BW = BW___A;
	}
}




//-------------------------------------------------------------------------
void DPD_OnOff_Ctrl(INT8U path, INT8U onoff)
{
	D_Printf( DEBUG_DPD, "[DPD #%d] DPD_OnOff_Ctrl %d (ON(0),Off(1))", path+1, onoff);
			
	#ifdef __MAKER_ANTS__							
	m_Reg_Flg.DPD[path].OnOff.Val = 1;
	m_Reg_Ctl.DPD[path].OnOff.Val = onoff; // '0' is 'ON'		
	#endif
}
void Use_d_DPD_OnOff(INT8U path, INT8U onoff)
{
	m_Eep.d_DPD_OnOff[path] = onoff;
	
	DPD_OnOff_Ctrl(path, onoff);
}
//-------------------------------------------------------------------------
void Use_d_CFR_Level(INT8U path, INT16U level_x10)
{
	m_Eep.d_CFR_Level_x10[path] = level_x10;
	m_Reg_Flg.CFR[path].Level.uD32 = 1;
	m_Reg_Ctl.CFR[path].Level.uD32 = level_x10;
}
//-------------------------------------------------------------------------
void Use_d_PAR_Level(INT8U path, INT16U level_x10)
{	
	m_Eep.d_PAR_x10[path] = level_x10;
}
//-------------------------------------------------------------------------
void Use_d_CFR_OnOff(INT8U path, INT8U onoff)
{
	m_Eep.d_CFR_OnOff[path] = onoff;
	
	#ifdef __MAKER_ANTS__							
	m_Reg_Flg.CFR[path].OnOff.Val = 1; 
	m_Reg_Ctl.CFR[path].OnOff.Val = onoff; // AID ON = '0' = register CFR ON
	#endif
}
//-------------------------------------------------------------------------
void Use_d_Payload_OnOff(INT8U fa, INT8U path, INT8U OnOff)	
{	
	if(fa >= MAX_Optic) return;
	if(path >= MAX_MIMO_PATH) return;

	m_Eep.d_Payload_OnOff[fa][path] = OnOff & 0x01;

	Use_DTU_DL_Path_Onoff(fa, path, OnOff & 0x01); // 2018.04.18 added
	Use_DTU_UL_Path_Onoff(fa, path, OnOff & 0x01); // 2018.04.18 added
}
//-------------------------------------------------------------------------
void Use_d_Rx_Casc_Add_OnOff_UL(INT8U OnOff)	 // AID 0xB435
{
	INT8U reg_onoff;	
	INT8U fa, m;

	m_Eep.d_Rx_Casc_Add_OnOff_UL = OnOff & 0x01; // representitive
	
	for(fa = 0; fa < MAX_Optic; fa++) // fa #0/#1
	{			
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			m_Eep.d_Rx_Casc_Add_OnOff[fa][m] = OnOff & 0x01;
		}
	}	
	//----------------------------------------------------		
	
	reg_onoff = (OnOff & 0x01) ^ 0x01; // ON is 1 @ FPGA

	D_Printf(DEBUG_SYSTEM, "[AID_Control] Use_d_Rx_Casc_Add_OnOff_UL 0x%02X, 0x%02X\n", OnOff, reg_onoff);
	
	#ifdef __MAKER_ANTS__	
	for(fa = 0; fa < MAX_Optic; fa++) // fa #0/#1
	{			
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			// 2019.05.06: changed..
			DTU_UL_Cascade_Onoff(fa, m, OnOff);		
		}
	}
	#endif
}



void User_DTU_Reset(INT8U rst_idx)
{
	if((rst_idx == 0)||(rst_idx >= REG_RST_Max))
		return;

#if 1

/*
2019.05.25
	PRU : RegMAP의 0x09 번지 "Reset" 항목의 비트는 [20] 비트만 사용, CPRI BLOCK Reset로 사용
	AAU : ??
*/
	//m_Reg_Sts.RW_Reset.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset]);
	//m_Reg_Sts.RW_Reset.Data = 0xFFFFFFFF;
	
	print_agent(DEBUG_SYSTEM, VT_MAGENTA, "[User_DTU_Reset] rst_idx %d, m_Reg_Sts.RW_Reset.Data : 0x%X", rst_idx, m_Reg_Sts.RW_Reset.Data);	
	//print_agent(DEBUG_SYSTEM, VT_MAGENTA, "[User_DTU_Reset] Reg_Common_Addr[Reg_Common_RW_Reset] 0x%X", Reg_Common_Addr[Reg_Common_RW_Reset]);

	m_Reg_Ctl.RW_Reset.Data = 0xFFFFFFFF;
	m_Reg_Ctl.RW_Reset.CPRI1 = 0; // '0' is reset
	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "User_DTU_Reset");


	OSTimeDly(OS_DELAY_500msec);
	print_agent(DEBUG_SYSTEM, VT_MAGENTA, "[User_DTU_Reset] Restore\n");

	m_Reg_Ctl.RW_Reset.Data = 0xFFFFFFFF;
	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "RST_Restore");
	
	////m_Reg_Ctl.RW_Reset.Data = 0xFFFFFFFF; // restore
	////m_Reg_Flg.RW_Reset.Data = _SET_;	
#else		
	switch(rst_idx)
	{
		case REG_RST______DSP: // DTU			
			m_Reg_Ctl.RW_Reset.DSP = 0; // '0' is reset
			break;
			
		case REG_RST_Main_Brd: // FPGA
			m_Reg_Ctl.RW_Reset.MB = 0; // '0' is reset
			break;
			
		case REG_RST_CpriCore: // CPRI Core
			break;
			
		case REG_RST_Ethernet: // Ethernet
			break;
			
		case REG_RST__JESD204: // JESD204
			break;
			
		case REG_RST_Delay__B: // Delay Block
			break;
			
		case REG_RST_Filter_B: // Filter Block
			break;
			
		case REG_RST____L2_SW: // L2 SW
			break;
			
		case REG_RST_CalBlock: // Calibration Block
			break;
	}
	m_Reg_Flg.RW_Reset.Data = _SET_;
#endif
}

/*
소출력 FAN 알람 과 온도 상한 알람 동시 발생 시 아래와 같이 동작
	0x9 번지의 [31],[29] bit '0'으로 control 설정
	해제 조건 : Fan 알람해제 및 온도 상한 알람 해제
	해제 후 시스템을 복귀 시키기 위해서는 시스템 Reset
*/
#ifdef __TYPE__AAU__
void Proc____AAU_Alarm_Fan_and_Temp(void)
{
	static INT8U st = 0;
	INT8U path;

	//---------------------------
	if(m_St.AlmTest)
		return;
	//---------------------------	

	switch(st)
	{
		case 0:
			m_Env.AAU_Alarm_Fan_and_Temp = 0;
			if(m_St.AlmSt.A_0.FAN1_Case && m_St.AlmSt.A_0.Temp_Up)
			{
				/*
				2019.08.01 : 알고리즘 적용전 PAU Off (Payload Off 포함)를 먼저 적용
				*/
				for(path = 0; path < AMPU_4_MAX; path++)
				{
					AAU__RF_AMP_OnOff(path, OFF, 4340);
					AAU__RF_LNA_OnOff(path, OFF, 4341);
					
					DTU_DL_Path_Onoff(FA_0, path, OFF); // AAU
					DTU_UL_Path_Onoff(FA_0, path, OFF); // AAU
					DTU_DL_Path_Onoff(FA_1, path, OFF); // AAU
					DTU_UL_Path_Onoff(FA_1, path, OFF); // AAU					
				}
				m_Env.AAU_Alarm_Fan_and_Temp = 1;
				//-----------------------------------------				
				m_Reg_Sts.RW_Reset.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset]);
				m_Reg_Ctl.RW_Reset.Data = m_Reg_Sts.RW_Reset.Data;	
				m_Reg_Ctl.RW_Reset.AFE = 0; // bit29 : set 0
				m_Reg_Ctl.RW_Reset.DSP = 1; // bit31 : set 1 : normal, 2019.08.01
				m_Reg_Flg.RW_Reset.Data = _SET_;
				Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "DSP AFE Reset");
				st = 1;

				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[m_Reg_Ctl.RW_Reset.Data 1] 0x%X", m_Reg_Ctl.RW_Reset.Data);
				
				printf(VT_YELLOW "[RCU] FAN1 Alarm && Temp_Up Alarm" VT_END_CR);
			}
			break;

		case 1:
			if((m_St.AlmSt.A_0.FAN1_Case == 0) && (m_St.AlmSt.A_0.Temp_Up == 0))
			{
				// system reboot
				st = 2;				
				SetResetInfo( Rst_By_Etc, 4140 );
				m_Env.is_reboot = 1; 
				printf(VT_YELLOW "[RCU] reboot by FAN1 Alarm && Temp_Up Alarm" VT_END_CR);
			}
			break;	

		case 2:
			break;	
	}
}
#endif

/*
round 0.1-step to 0.5-step
--------------------------
(ex) 
	0.1(0x01) ~ 0.4(0x04) => 0.5(0x01)
	0.6(0x06) ~ 0.9(0x09) => 1.0(0x02)
*/
INT8S step_01__round__step_05(INT16S step_x10)	
{
	INT8S step_x2;

	step_x2 = step_x10/5;
	
	if(step_x2*5 == step_x10)
		return step_x2;
	else
		return (step_x2+1); // +0.5dB
}

// roundup +0.5
INT16U roundoff_05(INT16U Att_x10)	
{
	INT8U uVal8;

	uVal8 = Att_x10/5;
	
	if(Att_x10 == (uVal8*5))
		return Att_x10;
	else	
		return (uVal8*5 + 5); // +0.5dB		
}


#define DIG_ATT_MAX_x2 (120) // 60.0
//------------------------------------------------------------------------
// index : idx = 120 - Att_x2 : get gain with att
//------------------------------------------------------------------------
INT32U AAU_Attx2_to_Gain60_Reg[DIG_ATT_MAX_x2+1] = // 0.0[dB] ~ 60.0[dB]
{
	0x41, // 0.0 ---- atten 60.0[dB]
	0x45,
	0x49, // 1.0
	0x4D,
	0x52, // 2.0
	0x56,
	0x5B,
	0x61,
	0x67,
	0x6D,
	0x73,
	0x7A,
	0x81,
	0x89,
	0x91,
	0x9A,
	0xA3,
	0xAC,
	0xB7,
	0xC1,
	0xCD, // 10.0
	0xD9,
	0xE6,
	0xF3,
	0x102,
	0x111,
	0x121,
	0x132,
	0x145,
	0x158,
	0x16C,
	0x182,
	0x199,
	0x1B1,
	0x1CA,
	0x1E6,
	0x202,
	0x221,
	0x241,
	0x263,
	0x288, // 20.0
	0x2AE, 
	0x2D7,
	0x302,
	0x32F,
	0x360,
	0x393,
	0x3C9,
	0x402,
	0x43F,
	0x480,
	0x4C4,
	0x50C,
	0x559,
	0x5AA,
	0x600,
	0x65B,
	0x6BB,
	0x721,
	0x78D,
	0x800, // 30.0
	0x879, 
	0x8FA,
	0x982,
	0xA12,
	0xAAB,
	0xB4D,
	0xBF8,
	0xCAE,
	0xD6E,
	0xE3A,
	0xF12,
	0xFF6,
	0x10E8,
	0x11E9,
	0x12F9,
	0x1418,
	0x1549,
	0x168C,
	0x17E2,
	0x194C, // 40.0
	0x1ACC,
	0x1C63,
	0x1E11,
	0x1FD9,
	0x21BC,
	0x23BC,
	0x25DA,
	0x2818,
	0x2A79,
	0x2CFD,
	0x2FA7,
	0x327A,
	0x3578,
	0x38A3,
	0x3BFE,
	0x3F8C,
	0x4350,
	0x474D,
	0x4B86,
	0x5000, // 50.0
	0x54BE, 
	0x59C3,
	0x5F15,
	0x64B7,
	0x6AAF,
	0x7101,
	0x77B3,
	0x7ECB,
	0x864E,
	0x8E43,
	0x96B1,
	0x9F9F,
	0xA914,
	0xB319,
	0xBDB6,
	0xC8F3,
	0xD4DC,
	0xE178,
	0xEED5,
	0xFCFB // 60.0 ---- atten 00.0[dB]  
};
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// 2019.05.08 : below function is not used @ ANTS-PRU
INT32U Get_GainRegVal(INT8U dlul, INT8U Att_x2)	
{
	INT8U idx;
	
	if(Att_x2 > DIG_ATT_MAX_x2)
		Att_x2 = DIG_ATT_MAX_x2;
	
	idx = DIG_ATT_MAX_x2 - Att_x2;
	
	//------ ANTS-AAU Only	
	return 	AAU_Attx2_to_Gain60_Reg[idx];	
}

INT16U Chk_Dig_Max_Att(INT16U Att_x10)
{
	if(Att_x10 > (DIG_ATT_MAX_x2*5))
		return 0;
	else 
		return Att_x10;	
}

/*
------------------------------------------------------
	= 20*log(GAIN/0x4000)
	= 20log(GAIN) - 20log(16384)
	= 20log(GAIN) - 20*4.21
	= 20log(GAIN) - 84.3

	att + 84.3 = 20log(Gain)
	10^[(att + 84.3)/20] = Gain

	= 20*log(0x800)
	= 20*log(2048)
	= 66.2
------------------------------------------------------	
*/

/*
------------------------------------------------------
[when DL]
---------------------------
      att    hex     gain
---------------------------      
pru   6dB    0x4000  0dB
aau  20dB    0x4000  0dB

[when UL]
---------------------------
      att    hex     gain
---------------------------      
pru   6dB    0x4000  0dB
aau  20dB     0x800  0dB

------------------------------------------------------	
*/

INT16U conv_Reg_Gain_to___Att_x10(INT8U dlul, INT32U regVal)
{
	INT16S Att_x10;
	INT8U uVal8;

	if(regVal == 0) regVal = 1;
	if(dlul > UL) return 0;

#ifdef __TYPE__PRU__		
	Att_x10 = 843 - 200*log10((double)regVal); // 84.2 - 20log(GAIN)
	Att_x10 += 60; // + 6.0(ref)
#else	
	/*
	----------------------------------
	Att 변환식, 
	Gain = ROUND(0x0800 * POWER(10, -ATT/20),0)
	Min step = 0.5dB
	-----------------
	20log[Gain/0x0800] = -ATT
	20log[0x0800] - 20log[Gain] = ATT
	66.2 - 20log[Gain] = ATT
	----------------------------------
	*/	
	Att_x10 = 662 - 200*log10((double)regVal); // 66.2 - 20log(GAIN)	
	Att_x10 += 300; // + 30.0(ref) : reg val 0x800 을 ATT 30[dB] = Gain 0[dB]	
#endif
	// 0.5 roundup/roundoff
	uVal8 = Att_x10 % 5;
	Att_x10 -= uVal8;
	if(uVal8 >= 3)	
		Att_x10 += 5;
	
	return Att_x10;
}
//-------------------------------------------------------
INT16U conv_Reg_GuardTime_to___usec(INT16U regVal)
{
#ifdef __TYPE__PRU__
	return (regVal / 307);
#else
	return (regVal / 307); // 2019.05.16
#endif	
}
INT16U conv_Reg_GuardTime_from_usec(INT16U usec)
{
#ifdef __TYPE__PRU__
	return (usec * 307);
#else
	return (usec * 307); // 2019.05.16
#endif	
}

//-------------------------------------------------------
INT16U conv_Reg_TsyncDly_to___usec(INT16U regVal)
{
	return (regVal / 307);
}
INT16U conv_Reg_TsyncDly_from_usec(INT16U usec)
{
	return (usec * 307);
}








//-------------------------------------------------------
// AAU : UL-ON-Guard
INT16U conv_AAU_Reg_GuardTime_to___usec(INT16U regVal) 
{
	return ((regVal-307)/307); // 2019.05.15
}
// AAU : UL-ON-Guard
INT16U conv_AAU_Reg_GuardTime_from_usec(INT16U usec) //
{
	if(usec == 0) return 1;
	else return (307 + usec * 307); // 2019.05.13 : 307 -> 266
}
//-------------------------------------------------------
// 2019.04.19
#define reg_AAU_DL_to_UL_Max (23170)
INT16U conv_Reg_GuardTime_to___usec_AAU_dl_2_UL(INT16U regVal)
{
	return (regVal/307); // 2019.05.15
}
INT16U conv_Reg_GuardTime_from_usec_AAU_dl_2_UL(INT16U usec)
{
	return (usec*307);// 2019.05.15
}
//-------------------------------------------------------
// d_UL_to_DL_Guard_Time
INT16U conv_Reg_GuardTime_to___usec_AAU_ul_2_DL(INT16U regVal)
{
	// 2019.05.13
	return (regVal / 307); 
}

// d_UL_to_DL_Guard_Time
INT16U conv_Reg_GuardTime_from_usec_AAU_ul_2_DL(INT16U usec) //
{
	// 2019.05.13
	if(usec == 0) return 1;
	else return (usec*307); 
}
//-------------------------------------------------------
INT16U conv_Reg_CfrGain_to___Gain(INT16U regVal)
{
	INT16U gain_x10;
	
	gain_x10 = 10*log10((double)regVal) - 42; // log(0x4000) = log(16384) = 4.2dB
	
	return (gain_x10);
}
INT16U conv_Reg_CfrGain_from_Gain(INT16U dBm_x10)
{
	double cal_d;
	
	cal_d = (double)(dBm_x10 + 42)/10.0;
	return (INT16U)pow(10.0, cal_d);
}


//-------------------------------------------------------
/*
2019.04.08
	Calculation ARFCN
	From 
Center-Freq
*/

void Update_ARFCN_from_BW(void)
{
/*
	INT32U N_ref_offs = 600000;
	INT32U F_ref_offs_x1000 = 3000000; // 3000.000[Mhz]
	INT32U F_global_x1000 = 15; // 0.015
	INT32U F_ref_x1000[MAX_Optic]; // center freq
	INT8U fa;
	
	// 100M
	F_ref_x1000[_100M] = 3650010; // 3650.01

	// Alpha
	switch(m_Eep.d_RE_Channel_BW[Alpha])
	{		
		case BW__40M : F_ref_x1000[Alpha] = 3720000; break; // 3720.00
		case BW__60M : F_ref_x1000[Alpha] = 3729990; break; // 3729.99
		case BW__80M : F_ref_x1000[Alpha] = 3740010; break; // 3740.01
		case BW_100M : F_ref_x1000[Alpha] = 3750000; break; // 3750.00

		default : // BW : 0M, 10M, 20M
			F_ref_x1000[Alpha] = 0;
			break;
	}
	
	//	N_ref = N_ref_offs + (F_ref - F_ref_offs)/F_global

	for(fa = _100M; fa < MAX_Optic; fa++)
	{
		m_St.g_NG_ARFCN[fa] = N_ref_offs + (F_ref_x1000[fa] - F_ref_offs_x1000)/F_global_x1000;
	}	
*/	
}


void Update_CenterFreq_from_BW(void)
{
	INT8U fa;
	INT32U cal32, cur32;
	static INT8U d_RE_Channel_BW[MAX_Optic];
	static INT8U init = 0;
	INT32U F_ref_x1000[MAX_Optic]; // center freq

	if(init == 0)
	{
		d_RE_Channel_BW[_100M] = m_Eep.d_RE_Channel_BW[_100M];
		d_RE_Channel_BW[Alpha] = m_Eep.d_RE_Channel_BW[Alpha];	
		
		init = 1;
		return;
	}

	if(d_RE_Channel_BW[Alpha] == m_Eep.d_RE_Channel_BW[Alpha])
		return;

	d_RE_Channel_BW[_100M] = m_Eep.d_RE_Channel_BW[_100M];
	d_RE_Channel_BW[Alpha] = m_Eep.d_RE_Channel_BW[Alpha];	
	
	switch(m_Eep.d_RE_Channel_BW[Alpha]) // Alpha
	{		
		case BW__40M : F_ref_x1000[Alpha] = 3720000; break; // 3720.00
		case BW__60M : F_ref_x1000[Alpha] = 3729990; break; // 3729.99
		case BW__80M : F_ref_x1000[Alpha] = 3740010; break; // 3740.01
		case BW_100M : F_ref_x1000[Alpha] = 3750000; break; // 3750.00

		default : // when 0, 10, 20Mhz
		/*
			BW 정보 중 0(미사용), 1(10M), 2(20M) 의 값은 
			미사용(0)으로 간주
		*/
			return; // return, use previous "m_Eep.d_CenterFreq[Alpha]"
	}	
	F_ref_x1000[_100M] = 3650010; // 3650.01, // 100M
	

	for(fa = FA_0; fa < MAX_FA_PATH; fa++)
	{			
		if(F_ref_x1000[fa] == m_Eep.d_CenterFreq[fa])
			continue;

		m_Eep.d_CenterFreq[fa] = F_ref_x1000[fa];
		m_Env.is_eep_write = EEP_WRITE_CODE;
		
	#ifdef __TYPE__PRU__
		cal32 = Use_d_Cal_VCO_PRU_1khz(fa, DL, m_Eep.d_CenterFreq[fa]);
		cur32 = m_Reg_Ctl.General_FA[fa].RW_PRU_DL_NCO.uD32;		
		
		if(cal32 != cur32)
		{
			D_Printf(DEBUG_SYSTEM, VT_GREEN "[Update_CenterFreq_from_BW] fa(%d), %d != %d \n", fa, cal32, cur32);
			
			m_Reg_Flg.General_FA[fa].RW_PRU_DL_NCO.uD32 = 1;
			m_Reg_Ctl.General_FA[fa].RW_PRU_DL_NCO.uD32 = cal32;

			printf(VT_BOLD_YELLOW "[RCU] DL, FA%d Fc %d by BW" VT_END_CR, fa, m_Eep.d_CenterFreq[fa]);
			D_Printf(DEBUG_SYSTEM, VT_GREEN "[ctrl] DL, FA%d Fc %d by BW", fa, m_Eep.d_CenterFreq[fa]);
		}
		//---
		cal32 = Use_d_Cal_VCO_PRU_1khz(fa, UL, m_Eep.d_CenterFreq[fa]);
		cur32 = m_Reg_Ctl.General_FA[fa].RW_PRU_UL_NCO.uD32;
		if(cal32 != cur32)
		{
			m_Reg_Flg.General_FA[fa].RW_PRU_UL_NCO.uD32 = 1;
			m_Reg_Ctl.General_FA[fa].RW_PRU_UL_NCO.uD32 = cal32;

			printf(VT_BOLD_YELLOW "[RCU] UL, FA%d Fc %d by BW" VT_END_CR, fa, m_Eep.d_CenterFreq[fa]);
			D_Printf(DEBUG_SYSTEM, VT_GREEN "[ctrl] UL, FA%d Fc %d by BW", fa, m_Eep.d_CenterFreq[fa]);
		}
	#else // ANTS-AAU
		cal32 = Use_d_Cal_VCO_AAU_1khz(m_Eep.d_CenterFreq[fa]);
		cur32 = m_Reg_Ctl.General_FA[fa].RW_NCO.uD32;
		if(cal32 != cur32)
		{
			m_Reg_Flg.General_FA[fa].RW_NCO.uD32 = 1;
			m_Reg_Ctl.General_FA[fa].RW_NCO.uD32 = cal32;

			printf(VT_BOLD_YELLOW "[RCU] FA%d Fc %d by BW" VT_END_CR, fa, m_Eep.d_CenterFreq[fa]);
			D_Printf(DEBUG_SYSTEM, VT_GREEN "[ctrl] FA%d Fc %d by BW", fa, m_Eep.d_CenterFreq[fa]);
		}
	#endif
	}	
}				

//-------------------------------------------------------
/*
Default Value = 0x0006 C000 (900us)
[31:0] Value,    Time(uS) =  Value / 491.52(MSPS)
*/
INT32U conv_Reg_Capture_to___usec(INT32U reg32)
{
	return (INT32U)((double)reg32 / 491.52);
}

INT32U conv_Reg_Capture_from_usec(INT32U usec32)
{
	return (INT32U)((double)usec32 * 491.52);
}
//-------------------------------------------------------
/*
Default Value = 0x0000 0000 (0us)
[31:0] Value,    Time(uS) =  Value / 102.4(MSPS)
*/
INT32U conv_Reg_RSSI_Time_to___usec(INT32U reg32)
{
	return (INT32U)((double)reg32 / 102.4);
}

INT32U conv_Reg_RSSI_Time_from_usec(INT32U usec32)
{
	return (INT32U)((double)usec32 * 102.4);
}
//-------------------------------------------------------


INT32U conv_Reg_delay_us_to___nsec(INT32U usec)
{	
	double d_val;

	d_val = (double)usec;
	
#ifdef __TYPE__PRU__	
	return (INT32U)(d_val / 102.4 * 1000.0);
#else
	return (INT32U)(d_val / 102.4 * 1000.0); // 2019.05.23
#endif
}

INT32U conv_Reg_delay_us_from_nsec(INT32U nsec)
{
/*
	conversion method
	------------------
	// from "ns" to "us" (/1000) and "x 102.4"   : PRU
	// from "ns" to "us" (/1000) and "x 122.88"  : AAU
*/
	double d_val;

	

#ifdef __TYPE__PRU__
	d_val = (double)nsec;
	//return (INT32U)(d_val /1000.0 * 102.4); // val * 102.4, ns -> us
	d_val /= 1000.0;
	d_val *= 102.4; 
	d_val += 0.5; // roundup
	return (INT32U)(d_val);
#else
	if(nsec > 120000) nsec = 120000; // 120.000[us] max, 2019.04.21 req by kks
	
	d_val = (double)nsec;
	//return (INT32U)(d_val /1000.0 * 102.4); // val * 102.4, ns -> us, 2019.05.23
	d_val /= 1000.0;
	d_val *= 102.4; // 2019.05.23
	d_val += 0.5; // roundup
	return (INT32U)(d_val);
#endif
}


// 2019.05.28 
#ifdef __TYPE__PRU__
void Use_TSYNC_Delay_Set_PRU(INT32U nsec)
{
	m_Reg_Flg.TSYNC_Delay.uD32 = 1;
	m_Reg_Ctl.TSYNC_Delay.uD32 = (INT32U)((double)nsec*0.307);
	m_Eep.d_t_Sync_Delay = nsec;

	print_agent(DEBUG_SYSTEM, VT_CYAN, "Use_TSYNC_Delay_Set : %d[ns] = register %d", 
		nsec, m_Reg_Ctl.TSYNC_Delay.uD32);
}
#else
void Use_TSYNC_Delay_Set_AAU(INT32U usec)
{
	m_Reg_Flg.TSYNC_Delay.uD32 = 1;
	m_Reg_Ctl.TSYNC_Delay.uD32 = conv_Reg_TsyncDly_from_usec(usec);
	m_Eep.d_t_Sync_Delay = usec;
}
#endif

/*
------------------------------------------------------------------
delay_dl_ns = DL_Delay(from L/S) -( PAU_DL_Offset + T2A_offset)
delay_dl_ns < 0 의 경우,
PRU && Factory_Mode 이면,
delay_dl_ns = 0 으로 처리.
------------------------------------------------------------------
*/
int Use_d_Delay_Ctrl(INT8U dlul, INT8U fa, INT8U path4, INT8U disp_en) //
{
	INT32U offset_ns;
	INT32U dly_LS_ns;
	INT32S fpga_delay_ns;

	if(dlul > UL) return RET_Error;
	if(fa >= MAX_FA_PATH) return RET_Error;
	if(path4 >= MAX_DL) return RET_Error;


	// 2019.06.13
	if((m_Eep.LS_Delay_DL_nsec[fa] >= 120000)||(m_Eep.LS_Delay_UL_nsec[fa] >= 120000))
		return RET_Error;

	if(disp_en)
	{
		print_agent(DEBUG_SYSTEM, VT_WHITE, " ");
		print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[FA %d] L/S Dly[ns] : DL %d, UL %d ", 
			fa, m_Eep.LS_Delay_DL_nsec[fa], m_Eep.LS_Delay_UL_nsec[fa]);
	}

	#ifdef __TYPE__AAU__
	m_St.FWD_Dly_Offset_x100[path4] = 8;  // AAU is 8.34[ns]	
	m_St.RVS_Dly_Offset_x100[path4] = 13; // AAU is 13.23[ns]
	#endif

	offset_ns = 0;
		
	if(dlul == DL)
	{
		/*
		FPGA Delay DL = DL_Delay(from L/S) -(DTU_Offset + PAU_Offset + T2A_offset)
		
		2019.10.31 : 'DTU_Offset' is 0, m_Eep.d_Offset_DTU_Delay_DL[fa][path4]
		*/
		dly_LS_ns = m_Eep.LS_Delay_DL_nsec[fa];
	#ifdef __TYPE__AAU__		
		offset_ns = m_Eep.d_Offset_DTU_Delay_DL[fa][path4]; // when AAU, this is not used and value is 0
	#else
		offset_ns = 0;
	#endif
		offset_ns += m_St.T2A_Dly__ns[fa]; // 2019.05.23
		offset_ns += 10*m_St.FWD_Dly_Offset_x100[path4];

		if(disp_en)
		{
			print_agent(DEBUG_SYSTEM,  VT_WHITE,"[FA %d] T2A_Delay	[ns] : %d", fa, m_St.T2A_Dly__ns[fa]);
	#ifdef __TYPE__AAU__	
			print_agent(DEBUG_SYSTEM, VT_WHITE, "[FA %d] Offset_DTU [ns] : %d (Path %d, AID 0xB415)", 
				fa, m_Eep.d_Offset_DTU_Delay_DL[fa][path4], path4);
	#endif		
			print_agent(DEBUG_SYSTEM, VT_WHITE, "[FA %d] PAU Offset [ns] : %d (Path %d)", 
				fa, 10*m_St.FWD_Dly_Offset_x100[path4], path4);
		}		
		fpga_delay_ns = dly_LS_ns - offset_ns;

	#ifdef __TYPE__PRU__ // 2019.11.11
		if((fpga_delay_ns < 0)&&(Get_Factory_Mode() == ON))
		{
			fpga_delay_ns = 0;
		}
	#endif
		
		if(fpga_delay_ns < 0) 
		{
			fpga_delay_ns = 0;
			return RET_Error;
		}
		else
		{
			// CFR On/Off : 2019.07.08
			// FPGA 설정 Delay가 50[u] : CFR Off시 FPGA 설정을 51.262[u]로 제어
			#ifdef __TYPE__AAU__
			if(m_St.d_CFR_OnOff[path4] == OFF) 
				fpga_delay_ns += 1262; // 1.262[us]
			#endif
			return fpga_delay_ns;
		}
	}
	else
	{
		/*	
		FPGA Delay UL = UL_Delay(from L/S) -(DTU_Offset + PAU_Offset + TA3_offset)
		
		2019.10.31 : 'DTU_Offset' is 0, m_Eep.d_Offset_DTU_Delay_UL[fa][path4]
		*/
		dly_LS_ns = m_Eep.LS_Delay_UL_nsec[fa];
	#ifdef __TYPE__AAU__			
		offset_ns = m_Eep.d_Offset_DTU_Delay_UL[fa][path4]; // when AAU, this is not used and value is 0
	#else
		offset_ns = 0;
	#endif	
		offset_ns += m_St.TA3_Dly__ns[fa]; // 2019.05.23		
		offset_ns += 10*m_St.RVS_Dly_Offset_x100[path4];

		if(disp_en)
		{
			print_agent(DEBUG_SYSTEM,  VT_WHITE,"[FA %d] TA3_Delay	[ns] : %d", fa, m_St.TA3_Dly__ns[fa]);	
			
	#ifdef __TYPE__AAU__
			print_agent(DEBUG_SYSTEM, VT_WHITE, "[FA %d] Offset_DTU [ns] : %d (Path %d, AID 0xB416)",
				fa, m_Eep.d_Offset_DTU_Delay_UL[fa][path4], path4);
	#endif		
			print_agent(DEBUG_SYSTEM, VT_WHITE, "[FA %d] PAU Offset [ns] : %d (Path %d)", 
				fa, 10*m_St.RVS_Dly_Offset_x100[path4], path4);
		}
		
		fpga_delay_ns = dly_LS_ns - offset_ns;
		
	#ifdef __TYPE__PRU__ // 2019.11.11
		if((fpga_delay_ns < 0)&&(Get_Factory_Mode() == ON))
		{
			fpga_delay_ns = 0;
		}
	#endif
	
		if(fpga_delay_ns < 0)
		{
			fpga_delay_ns = 0;
			return RET_Error;
		}
		else 
		{
			// CFR On/Off : 2019.07.08
			// FPGA 설정 Delay가 50[u] : CFR Off시 FPGA 설정을 51.262[u]로 제어
			#ifdef __TYPE__AAU__
			if(m_St.d_CFR_OnOff[path4] == OFF) 
				fpga_delay_ns += 1262; // 1.262[us]
			#endif
			return fpga_delay_ns;		
		}
	}
}



/*
---------------------------------------------------------------------
// 2019.08.29
PRU Gain Compensation
---------------------------------------------------------------------
*/
void Proc___PRU__BW_Gain_Compensation(void)
{
	static INT8U init = 0;
	INT8U l, bw;
	static INT16S comp_prev_x10[BW_Max][MAX_DL];
	static INT8U BW_prev;


	if(init == 0)
	{
		init = 1;
		BW_prev = m_Eep.d_RE_Channel_BW[Alpha];
		for(bw=0; bw < BW_Max; bw++)
		{
			for(l=0; l<MAX_DL; l++)
				comp_prev_x10[bw][l] = m_Eep.BW_Gain_Comp_PRU_x10[bw][l];
		}
	}		
	
	for(bw=0; bw < BW_Max; bw++)
	{	
		for(l=0; l<MAX_DL; l++)
		{
			if(comp_prev_x10[bw][l] != m_Eep.BW_Gain_Comp_PRU_x10[bw][l])
			{				
				comp_prev_x10[bw][l] = m_Eep.BW_Gain_Comp_PRU_x10[bw][l];
				Att_Digital_DL_SUM(l, m_Eep.d_Att_DL_SUM[l]);
			}

			if(BW_prev != m_Eep.d_RE_Channel_BW[Alpha])
				Att_Digital_DL_SUM(l, m_Eep.d_Att_DL_SUM[l]);
		}		
	}
	BW_prev = m_Eep.d_RE_Channel_BW[Alpha];
}

/*
---------------------------------------------------------------------
// 2019.08.29
DPD OFF 이면, bypass로 인해, delay가 줄어서, 
강제로 delay 를 +107[ns] 하여 Compensaion

---------------------------------------------------------------------
*/
void Proc___DPD_St_DL_Delay_Offset(void)
{	
#ifdef __TYPE__PRU__	
	static INT8U dpd_st_prev[MAX_DL];
	static INT8U init = 0;
	INT32S sVal32;
	INT8U fa, i;

	if(init == 0)
	{
		init = 1;
		for(i=0; i<MAX_DL; i++)
			dpd_st_prev[i] = m_St.d_DPD_OnOff[i];
	}

	for(i=0; i<MAX_DL; i++)
	{	
		if(m_St.d_DPD_OnOff[i] == OFF)
			m_Env.Delay_DL_Dpd_ns[i] = 170;
		else
			m_Env.Delay_DL_Dpd_ns[i] = 0;

		if(dpd_st_prev[i] != m_St.d_DPD_OnOff[i])
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "DPD.%d On/Off Status : %d -> %d, DL-Delay-Control", 
						i, dpd_st_prev[i], m_St.d_DPD_OnOff[i]);
			
			for(fa=0; fa<MAX_FA_PATH; fa++)
			{					
				sVal32 = Use_d_Delay_Ctrl(DL, fa, i, _Not_Disp_);
				if(sVal32 >= 0)
					Use_d_FPGA_Delay_Ctrl(DL, fa, i, sVal32, _Not_Disp_);
			}
			dpd_st_prev[i] = m_St.d_DPD_OnOff[i];
		}	
	}
#else // AAU
	INT8U i;
	
	for(i=0; i<MAX_DL; i++)
		m_Env.Delay_DL_Dpd_ns[i] = 0;
#endif
}

#ifdef __20191107__PRU_Tsync_Calc_Change__
void Proc___PRU_Tsync_Delay(void)	//
{
	// do nothing
}
#else
void Proc___PRU_Tsync_Delay(void)	//
{
	INT8U fa, path4;
	INT32U offset_ns = 0;
	INT32U rf_ns = 0;
	int diff;
	
	fa = m_St.Ref_CPRI_No;				
	if(fa >= MAX_FA_PATH) return;

	#ifdef __TYPE__PRU__
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4)==0) continue;
		
		rf_ns = 10*m_St.FWD_Dly_Offset_x100[path4]; 
		break;
	}
	if(path4 >= AMPU_4_MAX) path4 = 0;
	#else // below not used.... this fuction for only PRU
	rf_ns = 8;
	path4 = 0;
	#endif		
	
	offset_ns = m_St.d_FPGA_Delay_ns_DL[fa][path4];
	offset_ns += m_St.T2A_Dly__ns[fa];	
	offset_ns += rf_ns;
	offset_ns += m_Env.Delay_Adjust_ns; // 2019.08.28
	offset_ns += m_Env.Tsync_Offset_ns; // 2019.08.30

	/*
	D_Printf(DEBUG_SYSTEM, VT_CYAN "[RCU] CPRI%d : t-sync offset %d = FPGA %d + offset( %d + T2A (%d) + RF (%d))", 
				fa, 
				offset_ns, 
				m_St.d_FPGA_Delay_ns_DL[fa][path4], 			
				m_Eep.d_Offset_DTU_Delay_DL[fa][path4],
				m_St.T2A_Dly__ns[fa],
				rf_ns);
				
	D_Printf(DEBUG_SYSTEM, VT_CYAN "[RCU] CPRI%d : d_t_Sync_Delay(%d), offset_ns(%d)", 
				fa, m_St.d_t_Sync_Delay, offset_ns);
	*/			
	
	if(m_Eep.d_t_Sync_Delay != offset_ns)	
	{
		diff = m_Eep.d_t_Sync_Delay - offset_ns;
		if((diff < -2)||(diff > 2))
		{
			print_agent(DEBUG_SYSTEM, VT_WHITE, "CPRI%d : m_Eep.d_t_Sync_Delay(%d) != offset_ns(%d)", 
						fa, 				
						m_Eep.d_t_Sync_Delay, offset_ns);
					
			print_agent(DEBUG_SYSTEM, VT_CYAN, "CPRI%d : t-sync offset %d = FPGA %d + offset( T2A (%d) + RF (%d) + Dly_Adjust(%d) + Tsync_Offset(%d))", 
						fa, 
						offset_ns, 
						m_St.d_FPGA_Delay_ns_DL[fa][path4],
						m_St.T2A_Dly__ns[fa],
						rf_ns,
						m_Env.Delay_Adjust_ns,
						m_Env.Tsync_Offset_ns);			

		#ifdef __TYPE__PRU__
			Use_TSYNC_Delay_Set_PRU(offset_ns);
			m_Env.is_eep_write = EEP_WRITE_CODE;
		#endif	
		}
	}
}
#endif


INT8U conv_Reg_Clk_Recovery_to__AID(INT8U clock_st)
{
	     if(clock_st == 0) return 0x01; // 100MHz
	else if(clock_st == 1) return 0x02; // @MHz
	else                   return 0x00; // Unlock
}
//-------------------------------------------------------
#ifdef __20191107__PRU_Tsync_Calc_Change__
#if 0 // 2019.11.08
/*
(변경) 2019.11.08
DL_Delay(from L/S) + Tsync_Offset(500ns fix) + offset_ns(AID 0xB415)
*/
void Set___PRU_Tsync_Delay(INT8U path4)
{
	INT8U fa;
	INT32U offset_ns;
	int diff;
	INT32U LS_delay_ns;
	
	fa = m_St.Ref_CPRI_No;
	if(fa >= MAX_FA_PATH) return;
	
	LS_delay_ns = m_Eep.LS_Delay_DL_nsec[fa];
		
	offset_ns = LS_delay_ns;	
	offset_ns += m_Eep.d_Offset_DTU_Delay_DL[fa][path4];

	offset_ns += m_Env.Tsync_Offset_ns; // 2019.08.30
	
	if(m_Eep.d_t_Sync_Delay != offset_ns)	
	{
		diff = m_Eep.d_t_Sync_Delay - offset_ns;
		if((diff < -2)||(diff > 2))
		{
			print_agent(DEBUG_SYSTEM, VT_WHITE, "Cpri%d.Path#%d : t-sync offset_ns (%d) != m_Eep.d_t_Sync_Delay (%d)", 
						fa, path4+1,
						offset_ns,
						m_Eep.d_t_Sync_Delay );
					
			print_agent(DEBUG_SYSTEM, VT_CYAN,  "Cpri%d.Path#%d : t-sync offset_ns (%d) = LS_delay_ns (%d) + 0xB415_offset (%d) + Tsync_offset (%d)", 
						fa, path4+1,
						offset_ns, 
						LS_delay_ns,
						m_Eep.d_Offset_DTU_Delay_DL[fa][path4],
						m_Env.Tsync_Offset_ns);	
		
			Use_TSYNC_Delay_Set_PRU(offset_ns);
			m_Env.is_eep_write = EEP_WRITE_CODE;		
		}
	}
}	
#else // 2019.11.11
/*
--------------------------------------------------
2019.11.11
	delay_dl_ns 
	+ T2A_offset 
	+ PAU_DL_Offset 
	+ Tsync_Offset(500ns fix)
	+ offset_ns(AID 0xB415)
	+ Delay_Adjust_ns(87[ns])


delay_dl_ns is 'DL_Delay(from L/S) - (PAU_DL_Offset + T2A_offset)'
--------------------------------------------------
*/
void Set___PRU_Tsync_fpga_dly(INT32U delay_dl_ns, INT8U path4, INT8U disp_en)
{
	INT8U fa;
	INT32U offset_ns = 0;
	INT32U rf_ns = 0;
	int diff;
	
	fa = m_St.Ref_CPRI_No;
	if(fa >= MAX_FA_PATH) return;
			
	rf_ns = 10*m_St.FWD_Dly_Offset_x100[path4];
	
	offset_ns = delay_dl_ns;
	offset_ns += m_St.T2A_Dly__ns[fa];	
	offset_ns += rf_ns;
	offset_ns += m_Env.Tsync_Offset_ns; // 2019.08.30
	offset_ns += m_Eep.d_Offset_DTU_Delay_DL[fa][path4]; // 2019.11.11
	offset_ns += m_Env.Delay_Adjust_ns; // 2019.11.11 : 87[ns]
	
	if(m_Eep.d_t_Sync_Delay != offset_ns)	
	{
		diff = m_Eep.d_t_Sync_Delay - offset_ns;
		if((diff < -2)||(diff > 2))
		{
			if(disp_en)
			{
				print_agent(DEBUG_SYSTEM, VT_WHITE, "Cpri%d.Path#%d : t-sync offset_ns (%d) != m_Eep.d_t_Sync_Delay (%d)", 
							fa, path4+1,
							offset_ns,
							m_Eep.d_t_Sync_Delay );
						
				print_agent(DEBUG_SYSTEM, VT_CYAN,  "Cpri%d.Path#%d : t-sync offset_ns (%d) = delay_dl_ns (%d) + PAU_offset (%d) + T2A_offset (%d) + Tsync_offset (%d) + Delay_Adjust(%d)", 
							fa, path4+1,
							offset_ns, 
							delay_dl_ns,
							rf_ns,
							m_St.T2A_Dly__ns[fa],
							m_Env.Tsync_Offset_ns,
							m_Env.Delay_Adjust_ns);	
			}
		
			Use_TSYNC_Delay_Set_PRU(offset_ns);
			m_Env.is_eep_write = EEP_WRITE_CODE;		
		}
	}
}
#endif
#endif
//--------------------------------------------------------------------------------

void Use_d_FPGA_Delay_Ctrl(INT8U dlul, INT8U fa, INT8U path4, INT32U delay_ns, INT8U disp_en) //
{
	INT32U delay_us;
	INT32U offset_ns = 0;
	INT32U sum__ns;
#ifdef __TYPE__PRU__	
	INT8U  path;
#endif

	if(dlul > UL) return;
	if(fa >= MAX_FA_PATH) return;
	if(path4 >= MAX_DL) return;	

	/*
	2019.09.24 : 
	//delay_set 단계가 아니면 0으로 제어.(AutoConfig 수행 초기에 0 설정.)	
	
	//if(delay_ns != 0) // 0은 제어되도록.
	//{
	//	if(m_St.RU_Init_Status < Init_St_Dly_Set_Req)
	//	{
	//		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[FPGA_Delay_Ctrl] FA%d.path%d Canceled",  fa, path4);
	//		return;
	//	}
	//}
	*/

	if(dlul == DL)
	{
		m_Eep.d_FPGA_Delay_ns_DL[fa][path4] = delay_ns;	
		m_Env.is_eep_write = EEP_WRITE_CODE;
		
	#ifdef __USE_ProcessDly_plus_Offset__
		offset_ns = m_Eep.d_Offset_DTU_Delay_DL[fa][path4];

		if(disp_en)
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "FPGA DL Delay ctrl : fa %d, path %d : delay_ns %d[ns], offset %d[ns]", 
				fa, path4, delay_ns, offset_ns);

			print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "FPGA DL Delay ctrl : fa %d, path %d : DPD delay %d[ns]", 
				fa, path4, m_Env.Delay_DL_Dpd_ns[path4]);
		}
	#else
		offset_ns = 0;
	#endif	
	
		sum__ns  = delay_ns + offset_ns + m_Env.Delay_Adjust_ns + m_Env.Delay_DL_Dpd_ns[path4];
		delay_us = conv_Reg_delay_us_from_nsec(sum__ns); // 2019.11.05
		
		m_Reg_Flg.General_FA[fa].General_FA_MIMO[path4].RW_DL_DELAY.uD32 = 1;
		m_Reg_Ctl.General_FA[fa].General_FA_MIMO[path4].RW_DL_DELAY.uD32 = delay_us;

	#ifdef __TYPE__AAU__
		if((fa == FA_0)&&(path4 == MIMO_0))
		{
			Use_TSYNC_Delay_Set_AAU((delay_ns+offset_ns)/1000); // 2019.05.28
			
			if(disp_en)
				print_agent(DEBUG_SYSTEM, VT_CYAN, "TSYNC_Delay set %d[ns]", delay_ns+offset_ns);
		}
	#endif
		
	#ifdef __20191107__PRU_Tsync_Calc_Change__
		for(path = 0; path < AMPU_4_MAX; path++)
		{
			if(Check_PAU_Use(path))
				break;
		}
		if(path >= AMPU_4_MAX) path = 0;	

		if((path == path4) && (fa == m_St.Ref_CPRI_No))
		{
			Set___PRU_Tsync_fpga_dly(delay_ns, path4, disp_en);
		}
	#endif
	}
	else // UL
	{
		m_Eep.d_FPGA_Delay_ns_UL[fa][path4] = delay_ns;
		m_Env.is_eep_write = EEP_WRITE_CODE;
	#ifdef __USE_ProcessDly_plus_Offset__
		offset_ns = m_Eep.d_Offset_DTU_Delay_UL[fa][path4];

		if(disp_en)
			print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "FPGA UL Delay ctrl : fa %d, path %d : delay_ns %d[ns], offset %d[ns]", 
				fa, path4, delay_ns, offset_ns);
	#else
		offset_ns = 0;
	#endif			
		
		/*
		2019.09.24 : 
		delay_set 단계가 아니면 0으로 제어.(AutoConfig 수행 초기에 0 설정.) 
		
		//if(m_St.RU_Init_Status < Init_St_Dly_Set_Req) 
		//	delay_us = 0;
		//else 
		*/
		delay_us = conv_Reg_delay_us_from_nsec(delay_ns + offset_ns + m_Env.Delay_Adjust_ns);	
		
		m_Reg_Flg.General_FA[fa].General_FA_MIMO[path4].RW_UL_DELAY.uD32 = 1;
		m_Reg_Ctl.General_FA[fa].General_FA_MIMO[path4].RW_UL_DELAY.uD32 = delay_us;
	}
	
	if(disp_en)
		print_agent(DEBUG_SYSTEM, VT_CYAN, "FPGA Delay ctrl : dl/ul %d, fa %d, path %d : fpga %d[ns] -> fgpa reg 0x%X(%d)", 
			dlul, fa, path4, delay_ns, delay_us, delay_us);
}






//---------------------------------------------------

void Use_OSM_Use(INT8U use)
{
	m_Eep.OSM_Use = use & 0x01;
}


//-------------------------------------------------------------------------
// att + gain = 6.0 (PRU)
// att + gain = 20.0 (AAU)


INT16S Conv_Gain_to_Att(INT16S gain_x10)
{
	INT16S att_x10;

#ifdef __TYPE__PRU__	
	// 2019.05.06	
	if(gain_x10 > 20*10) // 20.0[dB]
		gain_x10 = 20*10; 
	else if(gain_x10 < (-20*10)) // -20.0[dB]
		gain_x10 = (-20*10);

	att_x10 = 20*10 - gain_x10;
#else	
	if(gain_x10 > 30*10) // 30.0[dB]
		gain_x10 = 30*10; 
	else if(gain_x10 < (-30*10)) // -30.0[dB]
		gain_x10 = (-30*10);

	att_x10 = 30*10 - gain_x10;
#endif

	return att_x10;
}
//-----------------------------------------------
INT16S Conv_Gain_from_Att(INT16S att_x10)
{	
#ifdef __TYPE__PRU__		
	return (20*10 - att_x10); // 2019.05.06 : 6dB -> 20dB
#else // AAU	
	return (30*10 - att_x10);	
#endif
}
//-----------------------------------------------

//-------------------------------------------------------------------------

INT32U Use_d_Cal_VCO( INT32U freq ) // freq is 10khz unit
{
	INT32S cal_s32;
	double cal_D64;
	
	/*
	cal_s32 = (freq - 370000);
	//D_Printf(DEBUG_SYSTEM, "cal_s32 %d, %f\n", cal_s32, cal_s32/100);
	
	if(cal_s32 < 0) cal_D64 = (double)((24576 + cal_s32)/24576.0)*pow(2.00, 32.0);
	else			cal_D64 = (double)(cal_s32/24576.0)*pow(2.00, 32.0);
	*/

	cal_s32 = (freq - 370000);
	if(cal_s32 < 0) cal_s32 += 24576;
	cal_D64 = (double)(cal_s32/24576.0)*4294967296; // 2^32 = pow(2.00, 32.0) = 4294967296

	return (INT32U) cal_D64;
}

#ifdef __TYPE__AAU__
INT32U Use_d_Cal_VCO_AAU_1khz( INT32U freq_x1000 ) // freq is 1khz unit
{
	INT32S cal_s32;
	double cal_D64;
	
	/*
	-----------------------------------------------------------
	cal_s32 = (freq - 370000);
	//D_Printf(DEBUG_SYSTEM, "cal_s32 %d, %f\n", cal_s32, cal_s32/100);
	
	if(cal_s32 < 0) cal_D64 = (double)((24576 + cal_s32)/24576.0)*pow(2.00, 32.0);
	else			cal_D64 = (double)(cal_s32/24576.0)*pow(2.00, 32.0);
	-----------------------------------------------------------
	*/

	cal_s32 = (freq_x1000 - 3700000);
	if(cal_s32 < 0) cal_s32 += 245760;
	cal_D64 = (double)(cal_s32/245760.0)*4294967296; // 2^32 = pow(2.00, 32.0) = 4294967296

	return (INT32U) cal_D64;
}
#endif


#ifdef __TYPE__PRU__
INT32U Use_d_Cal_VCO_PRU_1khz(INT8U fa, INT8U dl_ul, INT32U freq_x1000 ) // freq is 1khz unit
{
	/*
	-----------------------------------------------------------------
	2019.05.30
	--------------------------------
	DL Offset : -0.30518 Hz
	UL Offset : +0.38147 Hz

	100M DL : 3650.01MHz(Center Frequency)
		Value = (245.76MHz-(3700MHz-3650.01MHz)+DL Offset)/245.76MHz*(2^32)
	         = 0xCBED5550
	         
	100M UL : 3650.01MHz(Center Frequency)
		Value = (3700MHz-3650.01MHz+UL Offset)/245.76MHz*(2^32)
	         = 0x3412AAB1

	Alpha DL : 3750MHz(Center Frequency)
		Value = (3750MHz-3700MHz+DL Offset)/245.76MHz*(2^32)
	         = 0x34155550

	Alpha UL : 3750MHz(Center Frequency)
		Value = (245.76MHz-(3750MHz-3700MHz)+UL Offset)/245.76MHz*(2^32)
	         = 0xCBEAAAB1
	-----------------------------------------------------------------
	*/

	double cal_D64;
	double d_offset;
	double d_Fc;
	double d_val;
	INT32U uVal32;

	if(dl_ul >= DL_UL) return 0;
	if(fa >= MAX_FA_PATH) return 0;

	d_Fc = (double)freq_x1000/1000.0;
	
	if(fa == FA_0) // 100M
	{	
		if(dl_ul == DL) d_val = 245.76-(3700.00-d_Fc);
		else 		    d_val = 3700.00-d_Fc;
	}
	else // Alpha M
	{	
		if(dl_ul == DL) d_val = d_Fc-3700.00;
		else 		    d_val = 245.76-(d_Fc-3700.00);
	}
	if(dl_ul == DL) d_offset = (-0.30518); // DL
	else 		    d_offset = 0.38147;    // UL
	
	cal_D64 = ((d_val + d_offset/1000000.0)/245.76 * 4294967296.0); // 2^32 = pow(2.00, 32.0) = 4294967296
	uVal32 = (INT32U) cal_D64;

	D_Printf(DEBUG_SYSTEM, VT_CYAN "[Cal_VCO_PRU_1khz] fa(%d), dl_ul(%d) %d khz/ %f Mhz", fa, dl_ul, freq_x1000, d_Fc);
	D_Printf(DEBUG_SYSTEM, VT_CYAN "[Cal_VCO_PRU_1khz] d_val(%f), d_offset(%f)", d_val, d_offset);
	D_Printf(DEBUG_SYSTEM, VT_GREEN "[Cal_VCO_PRU_1khz] uVal32(%f, 0x%x, %d)", cal_D64, uVal32, uVal32);	
	
	return uVal32;
}
#endif

/*
-----------------------------------------------------------------		
상호 연결 상태로 내부 제어
----------------------------
DL Off Guard Time <-> DL to UL Guard Time
DL	On Guard Time <-> UL to DL Guard Time  
 
DL ON/OFF Guard Time 기준으로 설정되어야
DL to UL Guard Time, UL to DL Guard Time 설정시  
DL ON/OFF Guard Time 설정값을 +
 
Ex-1) 
	DL OFF Time = 3us, DL ON Time = 2us 
	DL to UL Guard Time = 2us 제어시,
	DL OFF Time과 합인 5us를 제어 함.
	
  UL to DL Guard Time = 1us 로 설정시 
  DL ON Time과 합인 3us를 제어 함.
  
Ex-2) 
위 상태에서 DL OFF Time을 1us로 제어하면, DL to UL Guard Time 또한 5 us에서 3us 제어를 변경 해야 하고,
   DL ON Guard Time을 3us로 제어를 변경하면, UL to DL Guard Time은 3us에서 4us 제어로 변경해야 한다.
-----------------------------------------------------------------		 
*/

void Use_DL_ON_Guard_Time_Ctrl(INT16U usec)
{
	if(usec > 5) usec = 5;
	
#ifdef __TYPE__AAU__
	m_Eep.d_DL_ON_Guard_Time = usec;
	m_Reg_Flg.DL_ON_Guard_Time.Value = 1;
	m_Reg_Ctl.DL_ON_Guard_Time.Value = conv_Reg_GuardTime_from_usec(usec);		
#else // PRU	
	INT16U sum_sec;	
	
	m_Eep.d_DL_ON_Guard_Time = usec;
	
	// control summation
	sum_sec = usec + m_Eep.d_UL_to_DL_Guard_Time;

	///// YCSO added START 190418
	/* 만에 하나 총 합 (sum_sec)이 DL on guard time이랑 같거나 작으면 안되기 때문에 명령을 무시.
	*/
	if( sum_sec <= usec)	// usec = d_DL_ON_Guard_Time
		return;

	else m_Eep.d_DL_ON_Guard_Time = usec;
	///// YCSO added END 190418
	if(sum_sec > 0)
	{	
		m_Reg_Flg.DL_ON_Guard_Time.Value = 1;
		m_Reg_Ctl.DL_ON_Guard_Time.Value = conv_Reg_GuardTime_from_usec(usec);	
		
		m_Reg_Flg.UL_to_DL_Guard_Time.Value = 1;
		m_Reg_Ctl.UL_to_DL_Guard_Time.Value = conv_Reg_GuardTime_from_usec(sum_sec);
	}
#endif	
}

void Use_UL_to_DL_Guard_Time_Ctrl(INT16U usec)
{
	if(usec > 5) usec = 5; // 2020.03.13
	
#ifdef __TYPE__AAU__
	m_Eep.d_UL_to_DL_Guard_Time = usec; 		
	m_Reg_Flg.UL_to_DL_Guard_Time.Value = 1;
	m_Reg_Ctl.UL_to_DL_Guard_Time.Value = conv_Reg_GuardTime_from_usec_AAU_ul_2_DL(usec);	
#else // PRU
	INT16U sum_sec;

	///// YCSO added 190418
	if( usec <= 0 )
		return;

	// control summation
	sum_sec = usec + m_Eep.d_DL_ON_Guard_Time;

	///// YCSO added START 190418
	/* 만에 하나 총 합 (sum_sec)이 DL on guard time이랑 같거나 작으면 안되기 때문에 명령을 무시.
	*/
	if( sum_sec <= m_Eep.d_DL_ON_Guard_Time)
		return;

	else m_Eep.d_UL_to_DL_Guard_Time = usec;
	///// YCSO added END 190418
		
	if(sum_sec > 0)
	{
		m_Reg_Flg.UL_to_DL_Guard_Time.Value = 1;
		m_Reg_Ctl.UL_to_DL_Guard_Time.Value = conv_Reg_GuardTime_from_usec(sum_sec);	
	}
#endif	
}

void Use_UL_ON_Guard_Time_Ctrl(INT16U usec)
{
	if(usec > 10) usec = 10; // 2020.03.13
	
#ifdef __TYPE__AAU__
	m_Eep.d_UL_ON_Guard_Time = usec;		
	m_Reg_Flg.UL_ON_Guard_Time.Value = 1;
	m_Reg_Ctl.UL_ON_Guard_Time.Value = conv_AAU_Reg_GuardTime_from_usec(usec);	
#else // PRU

	INT16U sum_sec;
	
//	m_Eep.d_UL_ON_Guard_Time = usec;
	// control summation
	sum_sec = usec + m_Eep.d_DL_to_UL_Guard_Time;

	///// YCSO added START 190418
	/* 만에 하나 총 합 (sum_sec)이 DL on guard time이랑 같거나 작으면 안되기 때문에 명령을 무시.
	*/
	if( sum_sec <= usec)
		return;

	else m_Eep.d_UL_ON_Guard_Time = usec;
	///// YCSO added END 190418

	if(sum_sec > 0)
	{
		m_Reg_Flg.UL_ON_Guard_Time.Value = 1;
		m_Reg_Ctl.UL_ON_Guard_Time.Value = conv_Reg_GuardTime_from_usec(usec);
		
		m_Reg_Flg.DL_to_UL_Guard_Time.Value = 1;
		m_Reg_Ctl.DL_to_UL_Guard_Time.Value = conv_Reg_GuardTime_from_usec(sum_sec);
	}
#endif	
}

void Use_DL_to_UL_Guard_Time_Ctrl(INT16U usec)
{
	if(usec > 10) usec = 10; // 2020.03.13
	
#ifdef __TYPE__AAU__
	m_Eep.d_DL_to_UL_Guard_Time = usec; 
	m_Reg_Flg.DL_to_UL_Guard_Time.Value = 1;
	m_Reg_Ctl.DL_to_UL_Guard_Time.Value = conv_Reg_GuardTime_from_usec_AAU_dl_2_UL(usec);	
#else // PRU
	INT16U sum_sec;

	///// YCSO added 190418
	if( usec <= 0 )
		return;
	
//	m_Eep.d_DL_to_UL_Guard_Time = usec;	
	// control summation
	sum_sec = usec + m_Eep.d_UL_ON_Guard_Time;

	///// YCSO added START 190418
	/* 만에 하나 총 합 (sum_sec)이 DL on guard time이랑 같거나 작으면 안되기 때문에 명령을 무시.
	*/
	if( sum_sec <= m_Eep.d_UL_ON_Guard_Time)
		return;

	else m_Eep.d_DL_to_UL_Guard_Time = usec;
	///// YCSO added END 190418
	
	if(sum_sec > 0)
	{
		m_Reg_Flg.DL_to_UL_Guard_Time.Value = 1;
		m_Reg_Ctl.DL_to_UL_Guard_Time.Value = conv_Reg_GuardTime_from_usec(sum_sec);	
	}
#endif	
}

//-----------------------------------------------
void DTU_DL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff)
{
	INT32U addr32;
	static INT8U onoff_prev[MAX_FA_PATH][MAX_MIMO_PATH] = {{0,0,0,0},{0,0,0,0}};

	onoff &= 0x01; // toggle
	addr32 = Reg_DSP_FA_MIMO_Addr[fa][mimo][Reg_DSP_FA_MIMO_RW_DL_OnOff];
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_DL_OnOff.Data = Read_FPGA_Reg(addr32);

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[mimo].RW_DL_OnOff.OnOff = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_DL_OnOff.OnOff = onoff ^ 1; // ON is 1 @ FPGA
	Write_FPGA_Reg(addr32, m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_DL_OnOff.Data, "Path ON/Off");

	if(onoff_prev[fa][mimo] != onoff)
		print_agent(DEBUG_SYSTEM, VT_YELLOW,"[DTU_DL_Path_Onoff] DL [fa_%d][path_%d] onoff %d", fa, mimo, onoff);
		
	onoff_prev[fa][mimo] = onoff;
}

void Use_DTU_DL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff)
{
	onoff &= 0x01; // toggle

	DTU_DL_Path_Onoff( fa,  mimo, onoff); // CHECK
	
	m_Eep.d_Path_OnOff_DL[fa][mimo] = onoff;
}
//-----------------------------------------------
void DTU_UL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff)
{
	INT32U addr32;
	
	onoff &= 0x01; // toggle
	addr32 = Reg_DSP_FA_MIMO_Addr[fa][mimo][Reg_DSP_FA_MIMO_RW_UL_OnOff];
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.Data = Read_FPGA_Reg(addr32);

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.OnOff = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.OnOff = onoff ^ 1; // ON is 1 @ FPGA
	Write_FPGA_Reg(addr32, m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.Data, "Path ON/Off");

	//print_agent(DEBUG_SYSTEM, VT_YELLOW,"[DTU_UL_Path_Onoff] UL[%d][%d] %d\n", fa, mimo, onoff);
}
//-----------------------------------------------
void DTU_UL_Cascade_Onoff(INT8U fa, INT8U mimo, INT8U onoff)
{
	INT32U addr32;
	
	onoff &= 0x01; // toggle
	addr32 = Reg_DSP_FA_MIMO_Addr[fa][mimo][Reg_DSP_FA_MIMO_RW_UL_OnOff];
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.Data = Read_FPGA_Reg(addr32);

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.Cascade_OnOff = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.Cascade_OnOff = onoff ^ 1; // ON is 1 @ FPGA

	Write_FPGA_Reg(addr32, m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_UL_OnOff.Data, "UL Cascade ON/Off");
}


void Use_DTU_UL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff)
{
	onoff &= 0x01; // toggle

	DTU_UL_Path_Onoff( fa, mimo, onoff);
	
	m_Eep.d_Path_OnOff_UL[fa][mimo] = onoff;
}









//-----------------------------------------------
// 2019.07.01
INT8U LSH_Gain_Set(INT8U REC, INT8U fa, INT8U m, INT8U dlul, INT16U gain_x10)
{
	if(fa >= MAX_FA_PATH) return 0;
	if(m >= MAX_MIMO_PATH) return 0;	
	if(dlul >= DL_UL) return 0;
	if(REC >= LSH_Max) return 0;
	
	
	if(dlul == DL)
	{
		m_Reg_Flg.General_FA[fa].General_FA_MIMO[m].RW_DL_LSH_Gain.uD32 = _SET_;
		m_Reg_Ctl.General_FA[fa].General_FA_MIMO[m].RW_DL_LSH_Gain.uD32 = gain_x10;

		D_Printf(DEBUG_SYSTEM, "[RCU] LSH DL Gain Set[%d][%d] %d Req", fa, m, gain_x10);
	}
	else	
	{
		m_Reg_Flg.General_FA[fa].General_FA_MIMO[m].RW_UL_LSH_Gain.uD32 = _SET_;
		m_Reg_Ctl.General_FA[fa].General_FA_MIMO[m].RW_UL_LSH_Gain.uD32 = gain_x10;

		D_Printf(DEBUG_SYSTEM, "[RCU] LSH UL Gain Set[%d][%d] %d Req", fa, m, gain_x10);
	}
	return 1;
}

INT8U Use_LSH_Gain_Set(INT8U REC, INT8U fa, INT8U mimo, INT8U dlul, INT16U gain_x10)
{
	if(LSH_Gain_Set(REC, fa, mimo, dlul, gain_x10))
	{
		m_Eep.d_LS_gain_x10[REC][dlul][fa][mimo] = gain_x10;
		return 1;
	}
	else return 0;
}








//-----------------------------------------------
// 2019.07.01
INT8U Test_Pattern_Band_Gain_Set(INT8U fa, INT8U mimo, INT16U gain_x10)
{
	if(fa >= MAX_FA_PATH) return 0;
	if(mimo >= MAX_MIMO_PATH) return 0;	

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[mimo].RW_Test_Pattern_Band_Gain.uD32 = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[mimo].RW_Test_Pattern_Band_Gain.uD32 = gain_x10;

	D_Printf(DEBUG_SYSTEM, "[RCU] Test_Pattern_Band_Gain_Set %d Req", fa, mimo, gain_x10);
	
	return 1;
}

INT8U Use_Test_Pattern_Band_Gain_Set(INT8U fa, INT8U mimo, INT16U gain_x10)
{
	if(Test_Pattern_Band_Gain_Set(fa, mimo, gain_x10))
	{
		m_Eep.d_Test_Pattern_Band_Gain[fa][mimo] = gain_x10;
		return 1;
	}
	else return 0;
}

//-----------------------------------------------
// 2019.07.01
// ANTS PRU Only
INT8U TSSI_SD_Level_Set(INT8U fa, INT8U m, INT16S level_x10)
{
	double d_val;
	INT32U reg32[3];
	
	if(fa >= MAX_FA_PATH) return 0;
	if(m >= MAX_MIMO_PATH) return 0;	
	
	/*
	[31:0] value
	value = 2^31 x 10^(TSSI상한/10)
	*/	

	level_x10 -= m_Eep.TSSI_Offset_x10[fa][m]; // 2019.07.12 remove tssi_offset 
	
	d_val = (double)(level_x10+20)/100.0;
	reg32[0] = (INT32U)(pow(2.0, 31.0)*pow(10.00, d_val)/0.74); // 2019.07.12 : divide 0.74
		
	d_val = (double)level_x10/100.0;
	reg32[1] = (INT32U)(pow(2.0, 31.0)*pow(10.00, d_val)/0.74); // 2019.07.12 : divide 0.74

	D_Printf(DEBUG_SYSTEM, "[RCU] TSSI_SD_Level Set[%d][%d] d_val %f, reg32 0x%X, level_x10 %d", fa, m, d_val, reg32[1], level_x10);
		
	d_val = (double)(level_x10-20)/100.0;
	reg32[2] = (INT32U)(pow(2.0, 31.0)*pow(10.00, d_val)/0.74); // 2019.07.12 : divide 0.74
	

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level_short.uD32 = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level_short.uD32 = reg32[0];

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level__long.uD32 = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level__long.uD32 = reg32[1];

	m_Reg_Flg.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level___Low.uD32 = _SET_;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level___Low.uD32 = reg32[2];

	return 1;
}


INT8U Height_Bal_Gain_Set(INT8U fa, INT8U m, INT16U gain_x10)
{
	if(fa >= MAX_FA_PATH) return 0;
	if(m >= MAX_MIMO_PATH) return 0;	
	
#ifdef __TYPE__PRU__ // ANTS-PRU		
	m_Reg_Flg.General_FA[fa].General_FA_MIMO[m].RW_Height_Bal_Gain.uD32 = 1;
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[m].RW_Height_Bal_Gain.uD32 = gain_x10;
#endif
	return 1;
}


INT16S Get__BW_Gain_Comp_PRU_x10(INT8U path4)
{
	INT8U bw;
	INT16S gain_x10;

	if(path4 >= MAX_MIMO_PATH) 
		return 0;	
	
	switch(m_Eep.d_RE_Channel_BW[Alpha])
	{		
		case BW__40M: bw = BW_140; break;
		case BW__60M: bw = BW_160; break;
		case BW__80M: bw = BW_180; break;
		case BW_100M: bw = BW_200; break;
	
		default: bw = BW_Max; break;
	}
	if(bw == BW_Max)
		gain_x10 = 0;
	else	
		gain_x10 = m_Eep.BW_Gain_Comp_PRU_x10[bw][path4];

	return gain_x10;
}


INT8U Att_Digital_DL_SUM(INT8U path4, INT16U att_x10) 
{
	INT16S gain_x10;
	
	if(path4 >= MAX_MIMO_PATH) 
		return 0;	

	// Range : 10dB	
	if(att_x10 > 100) att_x10 = 30;

	/*
		2019.08.29 : BW Gain Comp.(140MHz ~ 200Mhz)
	*/		
	gain_x10 = Get__BW_Gain_Comp_PRU_x10(path4);
	if(att_x10 < gain_x10) 
	     att_x10 = 0;
	else att_x10 -= gain_x10;
	
#ifdef __TYPE__PRU__ // ANTS-PRU		
	m_Reg_Flg.DSP_Common_MIMO[path4].RW_DL_Sum_Gain.uD32 = 1;
	m_Reg_Ctl.DSP_Common_MIMO[path4].RW_DL_Sum_Gain.uD32 = att_x10;
#endif
	return 1;
}

INT8U Use_Att_Digital_DL_SUM(INT8U path4, INT16U att_x10) 
{
	if(Att_Digital_DL_SUM(path4, att_x10)) 
	{
		m_Eep.d_Att_DL_SUM[path4] = att_x10;
		return 1;
	}
	else
		return 0;
}


/*
-------------------------------------------------------
통계보고 기준시			5600	03시 00분
자동보고 기준시			5603	04시 00분
통계보고 주기			5601	15분
TSSI 상한(100/alpha)	5101	-15dBm
TSSI 하한(100/alpha)	5102	-60dBm
출력 상한				5104	+28dBm
출력 하한				5105	+4dBm
UDA CPRI				5447	Alarm만 보고
UDA Clock				5446	Alarm만 보고
UDA 불요파				5444	Alarm만 보고
-------------------------------------------------------
AMP_Stdby 시작시간		5436	23시 00분
AMP_Stdby 종료시간		5436	07시 00분
------------------------------------------
MIMO Stdby 시작 시간	5432	23시 00분
MIMO Stdby 종료 시간	5432	07시 00분
-------------------------------------------------------
AMP Stdby 적용Path	    5437	미적용/적용/적용/적용
Log	                    5453	On
-------------------------------------------------------
*/




#ifdef __TYPE__AAU__
void Default_Setting(void)
{
	INT8U c, l, amp;

	if(m_Eep.AAU_Def_Setting_20200204 != 0x20200204)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[AAU__Default_Setting]");
						
		m_Eep.AAU_Def_Setting_20200204 = 0x20200204;	
	
		//----------------------------
		m_Eep.Statis_Rpt_Ref[0] = 4; // 2020.01.23 : 03:00 -> 04:00
		m_Eep.Statis_Rpt_Ref[1] = 0; // 2020.01.23 : 03:00 -> 04:00
		//----------------------------
		m_Eep.REMS_Auto_Rpt_Time[0] = 3; // 2020.01.23 : 04:00 -> 03:00
		m_Eep.REMS_Auto_Rpt_Time[1] = 0; // 2020.01.23 : 04:00 -> 03:00
		//----------------------------
		m_Eep.Statis_Rpt_Prd = 0x02; // 15분
		//----------------------------	


		for(c = 0; c < MAX_Optic; c++)
		{
			for(l = 0; l < MAX_UL; l++)
			{	
				m_Eep.DL_TSSI_UP_x10[c][l] = -150; // 0x5101
				m_Eep.DL_TSSI_DN_x10[c][l] = -700; // 0x5102, 2020.0113 : -60.0 -> -70.0
			}
		}

		for(amp = 0; amp < AMPU_4_MAX; amp++)
		{
			m_Eep.Fwd_Out_Lmt_x10[amp][HI] = 250; // 0x5104, 28.0 -> 25.0
			m_Eep.Fwd_Out_Lmt_x10[amp][LO] = 0;   // 0x5105, 4.0  -> 0
		}

		m_Eep.UDA_Oper_Mode[UDA_Spurious] = RS_Alm_Only; // 0x5444
		m_Eep.UDA_Oper_Mode[UDA_Clock] = AR_Alm_Only; // 0x5446
		m_Eep.UDA_Oper_Mode[UDA_PD_CP] = AR_Alm_Only; // 0x5447


		// 0x5436
		m_Eep.AMP_Stdby_Start_Time[0] = 23;
		m_Eep.AMP_Stdby_Start_Time[1] = 0;	
		m_Eep.AMP_Stdby_Stop_Time[0] = 7;
		m_Eep.AMP_Stdby_Stop_Time[1] = 0;

		// 0x5432
		m_Eep.MiMo_Stdby_Start_Time[0] = 23;
		m_Eep.MiMo_Stdby_Start_Time[1] = 0;	
		m_Eep.MiMo_Stdby_Stop_Time[0] = 7;
		m_Eep.MiMo_Stdby_Stop_Time[1] = 0;
		
		// 0x5437: AMP Standby Mode 적용 Path
		// 2020.01.20 : 미적용/적용/적용/적용 -> 미적용/미적용/미적용/미적용
		m_Eep.AMP_Stdby_Path = 0xFF; // (0:use, 1:Not use)
		//------------------------------------------
		// 0x5453: logging On/Off
		// 2020.01.20 : ON -> OFF
		m_Eep.Log_OnOff = OFF;
		//------------------------------------------
		// 2020.01.20 : MIMO Standby Mode(진입 Level)
		m_Eep.MIMO_Stdby_Level_x10[_IN_] = 110; // 0x543A, 11.0[dBm]
		//------------------------------------------
		// 2020.01.20 : MIMO Standby Mode(해제 Level)
		m_Eep.MIMO_Stdby_Level_x10[_OUT_] = 150; // 0x543B, 15.0[dBm]

		// 2020.01.23 : UL Gain backoff (AID 0x5450)		
		m_Eep.UL_Gain_A_BackOff = OFF;	
		//------------------------------------------
		// 2020.01.20 : AID 0xC070
		m_Eep.AAU_CW_Auto_Onoff = 0; // default OFF(0)
		//------------------------------------------
		// 2020.01.20 : DL LSH_Gain_Set (AID 0xC063/64)					
		for(c = 0; c < MAX_FA_PATH; c++)
		{
			for(l = 0; l < MAX_MIMO_PATH; l++)
			{
				Use_LSH_Gain_Set(0, c, l, DL, 20); // 0 : 0xC062, LSH_Gain_Set, 2.0
				Use_LSH_Gain_Set(1, c, l, DL, 85); // 1 : 0xC063, LSH_Gain_Set, 8.5
				Use_LSH_Gain_Set(2, c, l, DL, 40); // 2 : 0xC064, LSH_Gain_Set, 4.0
			}
		}
		//------------------------------------------
		// 2020.01.21 : TDD Guard Time, 5/5/5/5[us]
		Use_DL_ON_Guard_Time_Ctrl(5);    // DL ON Guard Time   (AID 0x5421)
		Use_UL_ON_Guard_Time_Ctrl(5);    // DL OFF Guard Time  (AID 0x5422)
		Use_DL_to_UL_Guard_Time_Ctrl(5); // DL to UL Guard Time(AID 0x5423)
		Use_UL_to_DL_Guard_Time_Ctrl(5); // UL to DL Guard Time(AID 0x5424)		
		//------------------------------------------
		// 2020.02.04 : Test pattern gain(100/+a)(AID 0xC074)
		for(c=0; c < MAX_FA_PATH; c++)
		{
			m_Eep.d_t_pattern_gain_x10[c][LSH_SS] =  20; // +2.0(Samsung)
			m_Eep.d_t_pattern_gain_x10[c][LSH_NK] = -30; // -3.0(Nokia)
			m_Eep.d_t_pattern_gain_x10[c][LSH_ER] =  20; // +2.0(Ericson)
		}
		//------------------------------------------
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}
}
#else // PRU
void Default_Setting(void)
{
	INT8U c, l, amp;

	if(m_Eep.PRU_Def_Setting_20200123 != 0x20200123)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[PRU__Default_Setting]");
						
		m_Eep.PRU_Def_Setting_20200123 = 0x20200123;

		for(c = 0; c < MAX_Optic; c++)
		{
			for(l = 0; l < MAX_UL; l++)
			{
				m_Eep.DL_TSSI_DN_x10[c][l] = -700; // 0x5102, 2020.0113 : -60.0 -> -70.0
			}
		}
		
		for(amp = 0; amp < AMPU_4_MAX; amp++)
		{
			m_Eep.Fwd_Out_Lmt_x10[amp][HI] = 480; // 2020.01.23 : 0x5104, 48.0
			m_Eep.Fwd_Out_Lmt_x10[amp][LO] = 20;  // 2020.01.20 : 0x5105, 2.0[dBm]
		}
		
		// 2020.01.20 : DL LSH_Gain_Set (AID 0xC063/64) 				
		for(c = 0; c < MAX_FA_PATH; c++)
		{
			for(l = 0; l < MAX_MIMO_PATH; l++)
			{
				Use_LSH_Gain_Set(1, c, l, DL, 56); // 1 : 0x0x63, LSH_Gain_Set, 5.6
				Use_LSH_Gain_Set(2, c, l, DL,  9); // 2 : 0x0x64, LSH_Gain_Set, 0.9
			}
		}

		// 2020.01.20 : UL Gain backoff (AID 0x5450)		
		m_Eep.UL_Gain_A_BackOff = OFF;	
		
		// 2020.01.20 : 0xC076		
		m_Eep.LS_Gain_Bal_OnOff = 1; // default ON


		m_Env.is_eep_write = EEP_WRITE_CODE;
	}
}

//------------------------------------------

#endif




/* EOF */

