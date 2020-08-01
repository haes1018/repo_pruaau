/////////////////////////////
//	PAU_COMMM.C
/////////////////////////////
#define PAU_COMMM_GLOBALS
#include "includes.h"
#include <math.h>
/////////////////////////////


/*
---------------------------------------------------
PAU Protocol ID @ 5G
---------------------------------------------------
*/
#ifdef __MAKER_ANTS__    // ANTS-------------------
#define _PAUID_PAU_ 0x20 // ANTS 0x20~0x3F : Use 0x20~0x23 Only @ 5G
#else					 // HFR -------------------
#define _PAUID_PAU_ 0x40 // HFR  0x40~0x5F : Use 0x40~0x43 Only @ 5G
#endif


#ifdef __TYPE__PRU__ // PRU Only
static PAU_PROTOCOL_FRAME pau_txf[AMPU_4_MAX];
static PAU_PROTOCOL_FRAME pau_rxf[AMPU_4_MAX];

static pthread_mutex_t	PAU_COMM_Evt[AMPU_4_MAX];
static pthread_mutex_t	PAU_Ctrl_Sem[AMPU_4_MAX];
#endif

static INT8U PAU_Ctrl_Req[AMPU_4_MAX];
static INT8U THR_Ctrl_Req[AMPU_4_MAX];



#define PAU_Timout_ms (300)


/////////////////////////////
// Local Functions
/////////////////////////////
//void Conv_RLoss_to_Vswr( INT8U path4 );
void Recv_PAU_Info(INT8U path4, INVENTORY_STR* inv, INVENTORY_2_STR* inv2);

//------------------------------------------------------------------------------------------


void Get_AMP_Prev_Alm( INT8U path4 );
void Init_prev_amp_alm(INT8U path4);
INT8U Proc_PAU_PkgDn(INT8U path4);



//------------------------------------------------------------------
// signed 16bit 값을 merge 하는 경우에는 리턴값을 INT16S로 읽음.
INT16U uConv_x10(INT8U* pVal)
{
	INT16U uVal16;

	//---------------------
	// Big Endian
	//---------------------
	uVal16 = pVal[0];
	uVal16 <<= 8;
	uVal16 |= pVal[1];

	return uVal16;
}
//------------------------------------------------------------------
// PAU AMP OFF Reason을 AID 방식으로 변경.
INT8U Conv_AMP_OFF_Reason(INT8U pau_off_reason)
{
	INT8U reason;

	switch(pau_off_reason)
	{
		case 0 : reason = 0;    break; // off state of after reset
		case 1 : reason = 0x0C; break; // PLL Unlock, not used @5G
		case 2 : reason = 0x03; break; // High Temperature
		case 3 : reason = 0x07; break; // VSWR
		case 4 : reason = 0x04; break; // Over Power
		case 5 : reason = 0x09; break; // DC Fail
		case 6 : reason = 0;    break; // User

		default: reason = 0xFF; break;
	}
	return reason;
}
//----------------------------------------------------------------------
void PAU_Link_Fail_Set(INT8U path4, INT8U link_Fail )
{
	m_St.Pwr_Amp_Link_Err[path4] = link_Fail;

	if(m_St.AlmTest) // 2019.05.01
		return;	
	m_St.AlmSt.Path[path4].PAU.Link_Fail = link_Fail;
}
//----------------------------------------------------------------------

INT8U IS__SD_Check_Enabled(INT8U path4)
{
	INT8U is_sd_chk_enable;
	//INT8U reason_dis;
	
	if(m_St.AlmSt.Path[path4].PAU.Disabled == 0)
	{
		is_sd_chk_enable = 1; // disable is normal
	}
	else // now disable alarm
	{
		////reason_dis = Get__Reason_Dis_5117_Ori(path4);
		////if(reason_dis == Rsn_Dis_User___Ctrl) // by user
		if(m_Eep.Pwr_Amp_En[path4] == OFF) // 2020.01.13 : by user
			is_sd_chk_enable = 0;
		else 
			is_sd_chk_enable = 2; // disable by system
	}	

	return is_sd_chk_enable;
}




/*
// 2020.04.02 : Use 'AMP.HW.Insert.Info' instead of 'AMP_Use'
*/
INT8U IS__AMP_Use_Allowed(INT8U path4)
{
	if(path4 >= AMPU_4_MAX) return 0;
	
	/*		
	// 2020.04.02 : Use 'AMP.HW.Insert.Info' instead of 'AMP_Use'
	////return Check_PAU_Use(path4); 
	*/

	if(m_St.AMP_HW_Insert[path4] == _PAU_Insert_) 
		return 1;
	else
		return 0;
}

//-----------------------------------------------------------------------------------
#ifdef __TYPE__PRU__
void Recv_PAU_Status(INT8U path4, AMP_ST_STR* pau) 
{
	INT16S sVal16;
	INT16U uVal16;	
	INT8U  fwd_enable;
	INT8U  pau_off_reason;
	INT8U  reason_dis;
	INT8U  reason__sd;
	static INT8U Alm_UL_Over_Prev[AMPU_4_MAX] = {0,0,0,0};	
	
	INT8U pau_dl_sd_alarm[AMPU_4_MAX];	
	INT8U pau_ul_sd_alarm[AMPU_4_MAX];
	
	INT8U  is_sd_chk_enable;
	static INT8U amp_on_conti_cnt[AMPU_4_MAX] = {0,0,0,0};
	
	//-------------------------------------------------
	if(m_St.AlmTest) // 2019.05.01
	{
		m_St.gREMS.Path[path4].PAU.Shutdown = m_St.AlmSt.Path[path4].PAU.Shutdown;
		return;	
	}
	
	if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27	
	{
		m_St.AlmSt.Path[path4].PAU.Dev_Fail   = (pau->AMP_Arm[1] & cbits[7]) ? 1:0; // Dev fail	
		m_St.AlmSt.Path[path4].PAU.Over_Power = (pau->AMP_Arm[1] & cbits[6]) ? 1:0; // Over Pwr		
		m_St.AlmSt.Path[path4].PAU.VSWR       = (pau->AMP_Arm[0] & cbits[2]) ? 1:0; // VSWR	
		m_St.AlmSt.Path[path4].PAU.High_Temp  = (pau->AMP_Arm[0] & cbits[0]) ? 1:0; // High Temp
		m_St.AlmSt.Path[path4].PAU.DC_Fail    = (pau->AMP_Arm[0] & cbits[1]) ? 1:0; // DC-Fail	
	}
	//-------------------------------------------------
	if(pau->AMP_Arm[0] & cbits[6]) // bit set is 'disable', off
	{	
		fwd_enable = 0; 
		amp_on_conti_cnt[path4] = 0;
	}
	else // enable, on
	{
		amp_on_conti_cnt[path4]++;
		if(amp_on_conti_cnt[path4] >= 2)
		{
			amp_on_conti_cnt[path4] = 2;
			fwd_enable = 1; 
		}
		else fwd_enable = 0;
	}
	//-------------------------------------------------
	
	if(IS__AMP_Use_Allowed(path4)) // 2020.02.03
	{	
		if(fwd_enable == 0)
		{
			m_St.Fwd_AMP_Alm[path4] = 1; // 2019.04.25
			m_St.Pwr_Amp_En[path4] = OFF; // 2019.06.28
		}
		else // enable, on
		{
			m_St.Fwd_AMP_Alm[path4] = 0; // 2019.04.25
			m_St.Pwr_Amp_En[path4] = ON; // 2019.06.28		
		}
	}
	else // 2020.02.03 : 미사용시, alarm clear, Amp OFF 표시를 위해.
	{
		m_St.Fwd_AMP_Alm[path4] = 0;
		m_St.Pwr_Amp_En[path4] = OFF;
		fwd_enable = 1; // for clearing alarm(Disable, Shutdown)		
	}

	pau_dl_sd_alarm[path4] = (pau->AMP_Arm[1]  & cbits[4]) ? 1:0; // DL Shutdown, 2020.02.27
	pau_ul_sd_alarm[path4] = (pau->Path_Arm[0] & cbits[3]) ? 1:0; // UL Shutdown Alarm

	/*
	--------------------------------------------------------------
		(1) m_St.AlmSt.Path[path4].UL.Over_Input : 
			RVS.SD.algorithm 진행 중, AMP 상태는 On 혹은 Off 중.
			
		(2) pau_ul_sd_alarm[path4] : 
			0 : RVS.SD.algorithm 30m 진행 전, 혹은 30m+5s 이후 정상 복귀시.
			1 : RVS.SD.algorithm 30m(AMP Off) + 5s(AMP On, 정상 체크 중), 완전 Shutdown(AMP is OFF)
			
		(3) pau_dl_sd_alarm[path4] : 			
			1 : VSWR/DC/HighTemp/OverPwr 로 인해, DL Shutdown 중. VSWR의 경우, 정상 복귀 전에는 알람(?)
			0 : 정상인 경우, 혹은 Rvs.OverIn.Shutdown 되거나 User.oFF 이면, 0
	--------------------------------------------------------------		
	*/
			
	//-----------------
	// RVS LNA	
	//-----------------	
	
	if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27	
	{
		m_St.AlmSt.Path[path4].UL.LNA_Fail = (pau->Path_Arm[0] & cbits[2]) ? 1:0; // RVS Dev Alarm
		m_St.AlmSt.Path[path4].UL.Over_Input = (pau->Path_Arm[0] & cbits[0]) ? 1:0; // RVS In-Over Alm, PRU
		
		// 2019.12.20
		if(Alm_UL_Over_Prev[path4] != m_St.AlmSt.Path[path4].UL.Over_Input)
			Update_Alm_Str_Lmt(0x52, 0x10, path4, 0, m_St.AlmSt.Path[path4].UL.Over_Input); // alarm or normal, PRU
		
		Alm_UL_Over_Prev[path4] = m_St.AlmSt.Path[path4].UL.Over_Input;		
	}

	if( pau->Path_Arm[0] & cbits[6] ) m_St.RVS_Amp_Onoff[path4] = 1;
	else		                      m_St.RVS_Amp_Onoff[path4] = 0;	
	//---------------------------------------------------	

#ifdef _PAU_1st_TEST_Debug_
	if(path4 == 0) // 2020.01.20 TEST Debug
	{
		print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[AMP#%d_Fail] FWD Amp %s : dev(%d), overPwr(%d), vswr(%d), high(%d), DC(%d)",			
						path4+1,
						Get_OnOff_Str(m_St.Pwr_Amp_En[path4]),
						m_St.AlmSt.Path[path4].PAU.Dev_Fail,
						m_St.AlmSt.Path[path4].PAU.Over_Power,
						m_St.AlmSt.Path[path4].PAU.VSWR,
						m_St.AlmSt.Path[path4].PAU.High_Temp,
						m_St.AlmSt.Path[path4].PAU.DC_Fail);
		
		print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[AMP#%d_Fail] RVS Amp %s : lna(%d), over_In(%d)",
						path4+1,
						Get_OnOff_Str(m_St.RVS_Amp_Onoff[path4]),
						m_St.AlmSt.Path[path4].UL.LNA_Fail,
						m_St.AlmSt.Path[path4].UL.Over_Input);
	}
#endif	

	//---------------------------------------------------	
	// 2번째알람, 하위3bit.
	pau_off_reason = pau->AMP_Arm[1] & 0x07;
	m_St.Pwr_Amp_Off_Reason_PAU[path4] = Conv_AMP_OFF_Reason(pau_off_reason);

	if(fwd_enable) // FWD AMP Enable
	{
		if(m_St.AlmSt.Path[path4].PAU.Disabled)
		{			
			m_St.AlmSt.Path[path4].PAU.Disabled = 0;			
		}
		
		if(m_St.AlmSt.Path[path4].PAU.Shutdown)
		{
			if(Get__Reason_SD_5115_Ori(path4) == Rsn_SD_UL_OverIn) // by UL.Over.In			
			{
				if(pau_ul_sd_alarm[path4] == 0) // from AMP : SD by RX_Over_In is cleared
					m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
			}
			else
			{
				#ifdef __USE__gIRO_DL_Shutdown_Field__
				if(pau_dl_sd_alarm[path4] == 0) // 2020.02.27 : 'if condition' added.
					m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
				#else
					m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
				#endif
			}			
		}
	}
	else // now amp off
	{	
		if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27		
		{				
			reason_dis = Get__Reason_Dis_5117_Ori(path4);
			reason__sd = Get__Reason_SD_5115_Ori(path4);
			
			switch(reason_dis)
			{
				case Rsn_Dis_User___Ctrl:							
					m_St.AlmSt.Path[path4].PAU.Disabled = 1;
					m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
					m_St.gREMS.Path[path4].PAU.Shutdown = 0; 
					break;
					
				case Rsn_Dis_System_Ctrl:
					if(m_St.AlmSt.Path[path4].PAU.Shutdown == 0)
						m_St.AlmSt.Path[path4].PAU.Disabled = 1;
					break;
			}
			//----------------------------------------------
			// check S/D alarm
			is_sd_chk_enable = IS__SD_Check_Enabled(path4);

			if(is_sd_chk_enable == 0) 
			{
			/*
				disable alarm 우선 순위에 밀려서, sd reason은 있는데, sd 정상이어서 sd reason이 필요 없는 상태에서,
				이후, sd alarm 이 되어, 원하는 reason 으로 갱신되어야 함.
				(reason overwrite 안되는 조건으로 이때, 원하는 reason 으로 갱신 안됨.)								
				"SD alarm 정상" && "정상으로 이미 보고가 됨" 이면, Reason을 clear한다.
			*/
				if(reason__sd != 0)
				{
					if(m_St.AlmSt.Path[path4].PAU.Shutdown == 0)
					{
						if(m_St.NMS_Alm.Path[path4].PAU.Shutdown == 0)
						{
							if(m_St.Old_Alm.Path[path4].PAU.Shutdown == 0)
							{
								Clr__Reason_SD_5115(path4);
								Clr__Reason_SD_5115_Prev(path4);

								print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Clr__Reason_SD_5115(%d)", path4);
								print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Clr__Reason_SD_5115_Prev(%d)", path4);
							}
						}
					}					
				}
			}			
			else // is_sd_chk_enable > 0
			{
				switch(pau_off_reason)
				{
					case 1 : Set__Reason_SD_5115(path4, Rsn_SD____by_PLL); break; // PLL Unlock, not used @5G
					case 2 : Set__Reason_SD_5115(path4, Rsn_SD__HighTemp); break; // High Temperature
					case 3 : Set__Reason_SD_5115(path4, Rsn_SD__by__VSWR); break; // VSWR
					case 4 : Set__Reason_SD_5115(path4, Rsn_SD_Over__Pwr); break; // Over Power
					case 5 : Set__Reason_SD_5115(path4, Rsn_SD_____by_DC); break; // DC Fail 

					default:
						if(m_St.AlmSt.Path[path4].UL.Over_Input) // PRU, algorithm 중(AMP is On or Off, now).
						{
							if(pau_ul_sd_alarm[path4])
								Set__Reason_SD_5115(path4, Rsn_SD_UL_OverIn); // PRU, 30m and after			
							else if(m_St.Pwr_Amp_En[path4] == OFF) // now amp off
								Set__Reason_SD_5115(path4, Rsn_SD_UL_OverIn_Skip); // PRU, before 30m, alarm only
						}
						break;
				}			
				
				switch(reason__sd)
				{							
					case Rsn_SD_TSSI_Over : // by RCU
					case Rsn_SD_CPRI_Fail :
					case Rsn_SD_CLock_Alm :
					case Rsn_SD____by_UDA :
					case Rsn_SD__by__VSWR :
					case Rsn_SD_UL_OverIn :					
					case Rsn_SD____by_PLL : // by AMP
					case Rsn_SD__HighTemp :
					case Rsn_SD_Over__Pwr :
					case Rsn_SD_____by_DC :
						if(is_sd_chk_enable == 1) // disable is normal
						{
							m_St.AlmSt.Path[path4].PAU.Shutdown = 1;
						}
						else if(is_sd_chk_enable == 2) // disable is alarm but system-control
						{
							m_St.AlmSt.Path[path4].PAU.Shutdown = 1;
							m_St.AlmSt.Path[path4].PAU.Disabled = 0;
						}							
						break;
						
					#if 0						
					/*
						활성화 하면, 
						30m 전에 disable alarm 이있었다면, 
						disable alarm이 clear되고, sd alarm 이 발생하는데,
						amp disable 상태에서 ul.over.in algorithm 이 동작 할 수 없으므로,
						아래를 활성화 해도 무방할 듯.
					*/ 
					case Rsn_SD_UL_OverIn_Skip:							
						m_St.AlmSt.Path[path4].PAU.Shutdown = 1;
						m_St.AlmSt.Path[path4].PAU.Disabled = 0;
						break;
					#endif
					
					//default :
					//	m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
					//	break;
				}
			}
		}	
	}	
	
	// update previous alarm
	////m_St.AlmSt_prev.Path[path4].PAU.Disabled = m_St.AlmSt.Path[path4].PAU.Disabled;
	////m_St.AlmSt_prev.Path[path4].PAU.Shutdown = m_St.AlmSt.Path[path4].PAU.Shutdown;
	
	// pwr offset ---------------------------------------------------------------------- from here
	sVal16 = (INT16S)uConv_x10((INT8U*)pau->FwdPwr_Offset);
	m_St.FWD_Pwr_Offset_x10[path4] = sVal16;

	sVal16 = (INT16S)uConv_x10((INT8U*)pau->RX_Pwr_Offset[0]); // in offset
	m_St.Rvs_IN_Pwr_Offset_x10[path4] = sVal16;
	
	sVal16 = (INT16S)uConv_x10((INT8U*)pau->RX_Pwr_Offset[1]); // out offset
	m_St.Rvs_OUT_Pwr_Offset_x10[path4] = sVal16;

	sVal16 = (INT16S)uConv_x10((INT8U*)pau->FB_Pwr_Offset);
	m_St.FB_Pwr_Offset_x10[path4] = sVal16;

	//------------------------------
	// 2019.08.28
	if(m_St.Pwr_Amp_En[path4] == ON)
	{
		sVal16 = (INT16S)uConv_x10((INT8U*)pau->FwdPwr);
		if(sVal16 <= 0) sVal16 = 0; 	
		m_St.Fwd_Out_Pwr_x10[path4] = sVal16;
	}
	else m_St.Fwd_Out_Pwr_x10[path4] = 0;
	
	//------------------------------
	// 2019.05.09 : PAU Temp Offset Added.
	m_St.AMP_Temp_x10[path4] = (INT16S)uConv_x10((INT8U*)pau->Temp) + m_Eep.Temp_Offset_x10[path4];
	m_St.Pwr_Amp_Watt[path4] = uConv_x10(pau->Watt)/10;
	//------------------------------
	uVal16 = uConv_x10((INT8U*)pau->R_Loss);
	m_St.Ret_Loss_x10[path4] = uVal16;
	
	m_St.Rvs_Gain_Bal_x2[path4] = pau->RX_Gain_Bal_x2;	

	m_St.VSWR_x100[path4] = uConv_x10((INT8U*)pau->VSWR);

	//
	if(m_St.Pwr_Amp_En[path4] == ON)
	{
		m_St.VSWR_Amp__On_x100[path4] = m_St.VSWR_x100[path4]; // 2020.01.28
	}
	if(m_St.AlmSt.Path[path4].PAU.VSWR == 0)
	{
		// 2020.01.29 : VSWR Alarm -> Normal 되는 순간의 VSWR update
		if(m_St.AlmSt_prev.Path[path4].PAU.VSWR != m_St.AlmSt.Path[path4].PAU.VSWR)
		{
			m_St.VSWR_Amp_Off_x100[path4] = m_St.VSWR_x100[path4]; 
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[AMP#%d] VSWR %d/100",
						path4+1, m_St.VSWR_Amp_Off_x100[path4]);
		}
	}
	m_St.AlmSt_prev.Path[path4].PAU.VSWR = m_St.AlmSt.Path[path4].PAU.VSWR;

	//--------------------------------------------------------------------
	// 2018.08.13 : if RVS-AMP-OFF, then Gain-balance-off-display
	//if(m_St.RVS_Amp_Onoff[path4] != ON) pau->RX_Gain_Bal_Onoff = 1;
	//--------------------------------------------------------------------	
	m_St.Rvs_Gain_Bal_On[path4] = pau->RX_Gain_Bal_Onoff;
	
	// 'pau->TX_TPL_Att_x2' not used
	m_St.Fwd_Out_Att_x2[path4]  = pau->TX_Att_x2;
	m_St.Fwd_TPL_Att_x2[path4]  = pau->TX_TPL_Att_x2;
	
	m_St.FWD_Att_Offset_x2[path4] = pau->TX_Gain_Att_x2;
	m_St.Fwd_Temp_Att_x2[path4]   = (INT8S)pau->TX_Temp_Att_x2;	
	//-----------------------------------------------------

	m_St.FB_Offset_ATT_x2[path4] = pau->FB_Offset_ATT_x2;
	m_St.FB_Temp_Att_x2[path4]   = (INT8S)pau->FB_Temp_Att_x2;

	sVal16 = (INT16S)uConv_x10((INT8U*)pau->RX_Out_Pwr);
	m_St.Rvs_OUT_Pwr_x10[path4] = sVal16;

	sVal16 = (INT16S)uConv_x10((INT8U*)pau->RX_Pwr);
	m_St.Rvs_IN_Pwr_x10[path4] = sVal16;

	//-----------------------------------------------------
	uVal16 = uConv_x10(pau->TH_OverTemp);
	m_St.Over_Temp_Thr_x10[path4] = uVal16;
	
	uVal16 = uConv_x10(pau->TH_OverTemp_Rec);
	m_St.Over_Temp_Rec_Thr_x10[path4] = uVal16;
	
	uVal16 = uConv_x10(pau->TH_Over_Pwr);
	m_St.Over_Pwr_Thr_x10[path4] = uVal16;
	
	uVal16 = uConv_x10((INT8U*)pau->TH_R_Loss);
	m_St.Ret_Loss_Thr_x10[path4] = uVal16;
	
	uVal16 = uConv_x10((INT8U*)pau->TH_R_Loss_Rec);
	m_St.Ret_Loss_Thr_Rec_x10[path4] = uVal16;

	uVal16 = uConv_x10((INT8U*)pau->TX_Dly_Offset);
	m_St.FWD_Dly_Offset_x100[path4] = uVal16;

	uVal16 = uConv_x10((INT8U*)pau->RX_Dly_Offset);
	m_St.RVS_Dly_Offset_x100[path4] = uVal16;
	
	//-----------------------------------------------------
	m_St.Rvs_IN_Pwr_x10[path4]    = (INT16S)uConv_x10((INT8U*)pau->RX_Pwr);	
	m_St.Rvs_Att_x2[path4]        = pau->RX_Att_x2;
	m_St.RX_ALC_Att_x2[path4]     = pau->RX_ALC_Att_x2;	
	m_St.Rvs_Att_Offset_x2[path4] = pau->RX_Gain_Att_x2;
	m_St.Rvs_Temp_Att_x2[path4]   = (INT8S)pau->RX_Temp_Att_x2;
	m_St.RX_AMP_Det_x1000[path4]  = uConv_x10((INT8U*)pau->RX_Volt);	
	m_St.Rvs_IN_ALC_x10[path4]    = (INT16S)uConv_x10((INT8U*)pau->RX_ALC_Lvl);
	m_St.Rvs_IN_ALC_Onoff[path4]  = pau->RX_ALC_Onoff;
	m_St.Rvs_IN_SD_x10[path4]     = (INT16S)uConv_x10((INT8U*)pau->RX_SD_Lvl);	
	
	// 2019.08.12 : AMP Forward Input Power
	sVal16 = (INT16S)uConv_x10((INT8U*)pau->TX_IN_Pwr);
	m_St.Fwd_IN_Pwr_x10[path4] = sVal16;
	
	m_St.ATT_Pair_Onoff[path4]    = pau->Att_Pair_OnOff;
	m_St.AMP_Auto_SD_Mode_En[path4] = pau->RVS_IN_Auto_SD;
	m_St.AMP_Temp_Onoff[path4]    = pau->Temp_Onoff;	
	//=================================================================
	m_St.FB_ATT_x2[path4] = pau->FB_ATT_x2;	
	m_St.FB_Pwr_x10[path4] = (INT16S)uConv_x10((INT8U*)pau->FB_Pwr);

	m_St.AMP_DL_St[path4][0] = pau->DL_St[0];
	m_St.AMP_DL_St[path4][1] = pau->DL_St[1];
	
	#ifdef _gREMS_TEST_
	#else	
	m_St.Pwr_Amp_EQ[path4] = pau->EQ_State; // 2019.08.12
	#endif

	uVal16 = uConv_x10(pau->DC_Volt);
	m_St.AMP_DC_Volt_x10[path4] = uVal16;	
} 
#endif // PRU
//------------------------------------------------------------------
/*
--------------------------------------------
	 5G Table Command
-------------------------------------------- 
	0x81	Reserved (Power Table 조회)
	0x82	Reserved (Power Table 제어)	
	---------------------------------------
	0x83	ATTEN 및 온도 보상 Table 조회
	0x84	ATTEN 및 온도 보상 Table 제어
	-------------------------------------------- 
*/
#define TX0_TEMP_ATT_IDX (0x01) // @ 5G
#define RX0_TEMP_ATT_IDX (0x02) // @ 5G
#define FB0_TEMP_ATT_IDX (0x03) // @ 5G

INT16U PAU_Table_PassData( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{	
	INT16U len = 0;
	
	(void)rxf; (void)txf;
	return len;
}
//----------------------------------------------------------------------

void PAU_ATT_Table_Conv_GUI_to_AMP( INT8U* pGUI, INT8U* pAMP )
{
	INT8U len;
	INT8U i;

	pAMP[1] = 0; // miminum-ascending-sort
	
	len = pGUI[2]; // length index
	pAMP[2] = len;	

	for( i = 0; i < len; i++) pAMP[len+2-i] = pGUI[i+3];
}
//----------------------------------------------------------------------


INT16U PAU_Table_ATT_PassData( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{	
#ifdef __TYPE__PRU__ // PRU Only
	INT16U len;
    INT8U path4;
    INT8U ComPort;
    INT16U l = 0;
	INT8U pau_type;
	INT8U gui_type;
	INT8U Tbl_idx;
	INT8U pau_id;
	INT8U amp_tbl_cmd;
	INT8U totPkt = 1, curPkt = 1;
	
	//------------------------------------
	Tbl_idx = rxf->data[0];
	//------------------------------------
	switch(Tbl_idx)
	{
		case Tbl_ATT_AMP1_FWD_OUT_ATT:
		case Tbl_ATT_AMP1_RVS_ATT:
		case Tbl_FWD_AMP1_TEMP:
		case Tbl_RVS_AMP1_TEMP:
		case Tbl_FB0_AMP1_TEMP:
			path4 = AMPU_4_1; 
			ComPort = COMM_UART_AMP_1; 
			break;
		
		case Tbl_ATT_AMP2_FWD_OUT_ATT:
		case Tbl_ATT_AMP2_RVS_ATT:
		case Tbl_FWD_AMP2_TEMP:
		case Tbl_RVS_AMP2_TEMP:
		case Tbl_FB0_AMP2_TEMP:
			path4 = AMPU_4_2; 
			ComPort = COMM_UART_AMP_2; 
			break;
		
		case Tbl_ATT_AMP3_FWD_OUT_ATT:
		case Tbl_ATT_AMP3_RVS_ATT:
		case Tbl_FWD_AMP3_TEMP:
		case Tbl_RVS_AMP3_TEMP:
		case Tbl_FB0_AMP3_TEMP:
			path4 = AMPU_4_3; 
			ComPort = COMM_UART_AMP_3; 
			break;
		
		case Tbl_ATT_AMP4_FWD_OUT_ATT:
		case Tbl_ATT_AMP4_RVS_ATT:
		case Tbl_FWD_AMP4_TEMP:
		case Tbl_RVS_AMP4_TEMP:
		case Tbl_FB0_AMP4_TEMP:
			path4 = AMPU_4_4; 
			ComPort = COMM_UART_AMP_4; 
			break;

		default: return 0;
	}		
	//------------------------------------
	// converting index-GUI & index-AMP
	
	switch(Tbl_idx)
	{
		case Tbl_ATT_AMP1_FWD_OUT_ATT:
		case Tbl_ATT_AMP2_FWD_OUT_ATT:
		case Tbl_ATT_AMP3_FWD_OUT_ATT:
		case Tbl_ATT_AMP4_FWD_OUT_ATT:
			amp_tbl_cmd = Tbl_AMP_TX0_Accr; // TX0 ATT
			break;

		case Tbl_ATT_AMP1_RVS_ATT:
		case Tbl_ATT_AMP2_RVS_ATT:
		case Tbl_ATT_AMP3_RVS_ATT:
		case Tbl_ATT_AMP4_RVS_ATT:
			amp_tbl_cmd = Tbl_AMP_RX0_Accr; // RX0 ATT
			break;

		case Tbl_FWD_AMP1_TEMP:
		case Tbl_FWD_AMP2_TEMP:
		case Tbl_FWD_AMP3_TEMP:
		case Tbl_FWD_AMP4_TEMP:
			amp_tbl_cmd = Tbl_AMP_TX0_Temp; // TX0 Temp
			break;

		case Tbl_RVS_AMP1_TEMP:
		case Tbl_RVS_AMP2_TEMP:
		case Tbl_RVS_AMP3_TEMP:
		case Tbl_RVS_AMP4_TEMP:
			amp_tbl_cmd = Tbl_AMP_RX0_Temp; // RX0 Temp
			break;

		case Tbl_FB0_AMP1_TEMP:
		case Tbl_FB0_AMP2_TEMP:
		case Tbl_FB0_AMP3_TEMP:
		case Tbl_FB0_AMP4_TEMP:
			amp_tbl_cmd = Tbl_AMP_FB0_Temp; // FB0 Temp
			break;	

		default: return 0;
	}	
	
	switch(rxf->Type) // GUI Command
	{
		case 0x75: pau_type = AMP_CMD_ATT_TEMP_TBL_WR; break;
		case 0x76: pau_type = AMP_CMD_ATT_TEMP_TBL_READ; break;

		default: return 0;
	}
	//------------------------------------------------------------------------
	pthread_mutex_lock(&PAU_COMM_Evt[path4]);
	
	// cpy len and data
	pau_txf[path4].len = rxf->len; // same format, same length
	l = rxf->len;
	
	pau_txf[path4].data[0] = amp_tbl_cmd; 
	memcpy( &pau_txf[path4].data[1], &rxf->data[1], l ); // 1 부터 복사.
	
	// RCU
	pau_txf[path4].ScrAddr = 0x00;
	pau_id = _PAUID_PAU_ + path4;
	// 2019.08.12 : PAUID : 0x20 -> 0x20/21/22/23
	////pau_id = _PAUID_PAU_;
	pau_txf[path4].DesAddr = pau_id;
	
	len = Make_Frame_PAU( pau_type, &pau_txf[path4], l, path4 );

	if( Send_DN_Data( ComPort, (INT8U*)&pau_txf[path4], len, (INT8U*)&pau_rxf[path4], PAU_Timout_ms ) )
	{
		if( (pau_rxf[path4].Type == pau_type) && (pau_rxf[path4].ScrAddr == pau_id) )
		{			
			D_Printf( DEBUG_SYSTEM, "Rx PAU Table [%d] 0x%02X.\r", path4, pau_type); 		

			switch( pau_type )
			{	
				case AMP_CMD_ATT_TEMP_TBL_WR:  gui_type = 0x75; break;
				case AMP_CMD_ATT_TEMP_TBL_READ:	gui_type = 0x76; break;
			}
			
			txf->Type = gui_type;			
			txf->len = pau_rxf[path4].len;
			
			txf->data[0] = Tbl_idx; // index from GUI
			memcpy( &txf->data[1], &pau_rxf[path4].data[1], txf->len);
			
			len = Make_Frame( txf->Type, rxf->Rcode, txf, txf->len, rxf->seqNo16, totPkt, curPkt );

			//-----------------------------------------------------	
			pthread_mutex_unlock(&PAU_COMM_Evt[path4]);
			return len;
		}
	}
	pthread_mutex_unlock(&PAU_COMM_Evt[path4]);
	CheckUART_Error( ComPort );
#endif	
	return 0;
}

//----------------------------------------------------------------------
#ifdef __TYPE__PRU__ // PRU Only
void PAU_Data_Analysis(INT8U path4, PAU_PROTOCOL_FRAME* rxf)
{
	switch( rxf->Type )
	{
		//---------------
		// AMP Status, En, Dis, ATT
		//---------------
		case AMP_CMD_ST:
		case AMP_CMD_EN:		
		case AMP_CMD_DIS:
		case AMP_CMD_TH_Ctrl:
		case AMP_CMD_TRx_PATH_Ctrl:	
			Get_AMP_Prev_Alm(path4);
			
			if(rxf->len >= sizeof(AMP_ST_STR))
			{
				Recv_PAU_Status(path4, (AMP_ST_STR*)rxf->data);				
				Update_Stat(&m_Stat.Fwd_Out_Pwr_x10[path4], m_St.Fwd_Out_Pwr_x10[path4]);
				Update_Stat(&m_Stat.Ret_Loss_x10[path4]   , m_St.Ret_Loss_x10[path4]);				
			}
			Proc_DL_OutPwr_Cal();			
			break;

		//---------------
		// AMP INFO
		//---------------
		case AMP_CMD_INFO:	
		case AMP_CMD_INFO_Ctrl:			
			Recv_PAU_Info(path4, (INVENTORY_STR*)rxf->data, (INVENTORY_2_STR*)rxf->data);
			break;

		//---------------
		// Table
		//---------------			
		case AMP_CMD_ATT_TEMP_TBL_READ:
		case AMP_CMD_ATT_TEMP_TBL_WR:
			//RecvPAU_ATT_Temp_Table( path6, (??*)rxf->data );
			break;

		case AMP_CMD_FW_APPLY:
			m_St.AMP_FW_NACK[path4] = rxf->data[0];
			///// YCSO added 190521
			set_amp_DN_confirm_END(path4, _SET_);
			break;

		case AMP_CMD_RST_Ctrl:
			break;
	}
}
#endif

/*
----------------------------------------------------------------------
if 
	AMP-HW-Not-Inset, m_St.LayerUseInfo = 0/2/3 : layer not use
else
	m_St.LayerUseInfo = m_Eep.LayerUseInfo : layer use/not-use by User-Control
----------------------------------------------------------------------	
*/
void Proc_Chk_AMP_Install(INT8U path4)
{	
#ifdef __TYPE__AAU__
	m_St.LayerUseInfo[path4] = m_Eep.LayerUseInfo[path4] & 0x03; // 2020.02.03 : '&0x01' -> '&0x03'
#else
	if(Is_Amp_Install(path4))
	{		
		m_St.LayerUseInfo[path4] = m_Eep.LayerUseInfo[path4] & 0x03; // 2020.02.03 : '&0x01' -> '&0x03'
		/*
		D_Printf( DEBUG_SYSTEM, "[Proc_Chk_AMP_Install] AMP#%d Insert, LayerUse(%d).\n", 
			path4+1, 
			m_St.LayerUseInfo[path4]);
		*/
	}
	else 
	{
		/*
		D_Printf( DEBUG_SYSTEM, "[Proc_Chk_AMP_Install] AMP#%d Not-Insert.\n", path4+1 );
		*/
		
		m_St.LayerUseInfo[path4] = 0;
	}
#endif
}

//----------------------------------------------------------------------
// When AID 0x5002 is not-use, then alarm-mask
void AMP_Not_Use_Alarm_Mask(INT8U path4)
{
	if(m_St.AlmTest) // 2019.05.01
		return;

	//---------------------------------------------------
	m_St.AlmSt.Path[path4].DL.Data = 0;
	m_St.AlmSt.Path[path4].UL.Data = 0;	
	m_St.AlmSt.Path[path4].PAU.Data = 0;
	m_St.AlmSt.Path[path4].DPD.Data = 0;
	m_St.AlmSt.Path[path4].DlUl.Data = 0;

	m_St.Last_Alm.Path[path4].DPD.Data = 0;
	m_St.Last_Alm.Path[path4].PAU.Data = 0;
	m_St.Last_Alm.Path[path4].DL.Data = 0;
	m_St.Last_Alm.Path[path4].UL.Data = 0;
	m_St.Last_Alm.Path[path4].DlUl.Data = 0;	

	m_St.Pwr_Amp_Link_Err[path4] = 0;	
}
//----------------------------------------------------------------------
INT8U Is_Amp_Install(INT8U path4)
{
	if(Get_Factory_Mode() == ON)
		return 1;
	
	if(m_St.AMP_HW_Insert[path4] == _PAU_Insert_) 
		return 1;
	else return 0;
}
//----------------------------------------------------------------------
void Disp_PAU_Cmd( INT8U path4, INT8U type, INT8U send )
{
	switch(type)
	{
		case AMP_CMD_EN: //= 0x51,
			if(send) D_Printf( DEBUG_SYSTEM, "-> [%d] AMP EN.\r\n", path4 );
			else     D_Printf( DEBUG_SYSTEM, "<- <%d> AMP EN.\r\n", path4 );
			break;
			
		case AMP_CMD_DIS: //= 0x52,	
			if(send) D_Printf( DEBUG_SYSTEM, "-> [%d] AMP Dis.\r\n", path4 );
			else	 D_Printf( DEBUG_SYSTEM, "Rx <%d> AMP Dis.\r\n", path4 );
			break;

		case AMP_CMD_TH_Ctrl: //= 0x55, // threshold 제어
			if(send) D_Printf( DEBUG_SYSTEM, "-> [%d] Thresh.\r\n", path4 );
			else	 D_Printf( DEBUG_SYSTEM, "Rx <%d> Thresh.\r\n", path4 );
			break;
			
		case AMP_CMD_RST_Ctrl: //= 0x56, // Reset 제어
			if(send) D_Printf( DEBUG_SYSTEM, "-> [%d] Reset.\r\n", path4 );
			else	 D_Printf( DEBUG_SYSTEM, "Rx <%d> Reset.\r\n", path4 );
			break;	
			
		case AMP_CMD_INFO_Ctrl: //= 0x58,
			if(send) D_Printf( DEBUG_SYSTEM, "-> [%d] info.\r\n", path4 );
			else	 D_Printf( DEBUG_SYSTEM, "Rx <%d> info.\r\n", path4 );
			break;	

		case AMP_CMD_TRx_PATH_Ctrl: //= 0x80,	
			if(send) D_Printf( DEBUG_SYSTEM, "-> [%d] Ctrl.\r\n", path4 );
			else	 D_Printf( DEBUG_SYSTEM, "Rx <%d> Ctrl.\r\n", path4 );
			break;	
	}
}

INT8U PAU_Send_Data(INT8U path4, INT8U type, INT8U* data, INT8U l)
{
#ifdef __TYPE__PRU__ // PRU Only
	INT16U len;
	INT8U ComPort;	
	INT8U pau_id;
	
	
	switch (path4)
	{
		case AMPU_4_1: ComPort = COMM_UART_AMP_1; break;
		case AMPU_4_2: ComPort = COMM_UART_AMP_2; break;
		case AMPU_4_3: ComPort = COMM_UART_AMP_3; break;
		case AMPU_4_4: ComPort = COMM_UART_AMP_4; break;

		default: return 0;
	}

	pthread_mutex_lock(&PAU_COMM_Evt[path4]);

	// RCU
	pau_txf[path4].ScrAddr = 0x00;
	pau_id = _PAUID_PAU_ + path4;
	// 2019.08.12 : PAUID : 0x20 -> 0x20/21/22/23
	////pau_id = _PAUID_PAU_;
	pau_txf[path4].DesAddr = pau_id;
	pau_txf[path4].Type = type;

	if( l ) memcpy( pau_txf[path4].data, data, l );		
	len = Make_Frame_PAU(type, &pau_txf[path4], l, path4);

	if( Send_DN_Data( ComPort, (INT8U*)&pau_txf[path4], len, (INT8U*)&pau_rxf[path4], PAU_Timout_ms ) )
	{	
		if((pau_rxf[path4].Type == type) && (pau_rxf[path4].ScrAddr == pau_id))
		{	
			PAU_Data_Analysis(path4, &pau_rxf[path4]);
			pthread_mutex_unlock(&PAU_COMM_Evt[path4]);

			/*
			//if((type == AMP_CMD_EN)||(type == AMP_CMD_DIS))
			//{
			//	if(path4 == AMPU_4_4)
			//	{
			//		if(type == AMP_CMD_EN)
			//			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[AMP#4] AMP_CMD_EN Ctrl ---");
			//		else
			//			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[AMP#4] AMP_CMD_DIS Ctrl ---");
			//	}
			//}
			*/
			return 1;	
		}
	}
	else
	{
		//D_Printf( DEBUG_SYSTEM, "[!] PAU RX Fail [%s] 0x%02X\r\n", Get_path4_Str(path4), type );
		//printf("[!] PAU RX Fail [%s] 0x%02X\r\n", Get_path4_Str(path4), type );
	}

	pthread_mutex_unlock(&PAU_COMM_Evt[path4]);
	CheckUART_Error( ComPort );
#endif
	return 0;	
}
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
INT8U AC_Fail_PAU_EN_Ctrl(INT8U path4, INT8U en)
{
	INT8U cmd = 0;

#ifdef __TYPE__PRU__

	// 2019.04.22.
	if(en == OFF) StdBy_Off_rsn[path4] = _SET_;
	else          StdBy_Off_rsn[path4] = _CLR_;

	if( en == OFF ) cmd = AMP_CMD_DIS;
	else            cmd = AMP_CMD_EN;
	//----------------------------------
	// 2019.04.26.
	if(IS_PAU_Algorithm_Off(path4)) 
	{	
		if(cmd == AMP_CMD_EN)
			return cmd;	
	}
	//----------------------------------	
	if(en == OFF) // 2019.12.20
	{
		Set__Reason_Dis_5117(path4, Rsn_Dis_System_Ctrl);	
	}	
	//----------------------------------
	Check_DPD_DCL_Reset(path4, en);
	cmd = PAU_Send_Data(path4, cmd, NULL, 0);
#endif

	return cmd;	
}


//----------------------------------------------------------
// AC Fail 상태에서는 amp on 하지 않음.
//----------------------------------------------------------
INT8U AC_Fail_Chk(INT8U path4)
{
	// AC Fail로 amp off 했으면..
	if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ) 
		return 1;
	else
		return 0;

	////if(AC_Fail_St != _AC_NORMAL_)
	////{
	////	if (AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ) 
	////		return 1;
	////}
	////return 0;
}

//------------------------------------------------------------------
INT8U sendPAU_EnDis(INT8U path4, INT8U enable )
{
	INT8U ret = 0; // init 0
	INT8U cmd;

	if (path4 >= AMPU_4_MAX) return ret;

	//----------------------------------------------------------
	// AC Fail 상태에서는 amp on 하지 않음.
	//----------------------------------------------------------
	if(enable == _EN_ )
	{		
		if(AC_Fail_Chk(path4)) 
		{
			print_agent(DEBUG_STDBY, VT_BOLD_CYAN, "[!] %s AMP ON Fail(AC Fail).",Get_path4_Str(path4));
			return 0;
		}		
	}
	//----------------------------------------------------------

	if(enable == _EN_ ) cmd = AMP_CMD_EN;
	else    	        cmd = AMP_CMD_DIS;

	/*
	//if(path4 == AMPU_4_4)
	//{
	//	if(cmd == AMP_CMD_EN)
	//		print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[AMP#4] sendPAU_EnDis : EN ---");
	//	else
	//		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[AMP#4] sendPAU_EnDis : Dis ---");
	//}
	*/	
	
#ifdef __TYPE__PRU__
	if(enable == _EN_)
		Check_DPD_DCL_Reset(path4, ON);
#endif		
	ret = PAU_Send_Data(path4, cmd, NULL, 0);

	return ret;	
}
//----------------------------------------------------------


INT8U PAU_EN_Ctrl(INT8U path4, INT8U en)
{
	INT8U cmd;
	
	//----------------------------------------------------------
	// Amp Not Controlled
	//
	// when AC Fail / STDBY mode
	//----------------------------------------------------------		
	
	if(AC_Fail_Chk(path4))
		return 0;

	if(IS__Standby_Status(path4)) // 2020.01.13
	{
		return 0;	
	}
	
	//if(m_St.MIMO_Stdby_Enable == ON)  // 2020.01.13 : deleted
	{
		if (Mimo_Stby_Ctrl_Req[path4]) return 0;	
	}	
	if(m_St.AMP_Stdby_Enable == ON)
	{
		if(AMP_Stby_Off_Req[path4]) return 0; // AMP_Std가 진행중이어서 아직 exit 안되었으면.
	}
	/*
	User가 off 제어하면, 이부분이 amp-std algorithm보다 먼저 동작.
	AMP_Stdby_Mode_Onoff가 off 되면서 아래 code 실행.
	먼저 AMP ON.
	*/	
	//----------------------------------------------------------
	
	if( en == OFF ) cmd = AMP_CMD_DIS;
	else            cmd = AMP_CMD_EN;

	if( en == ON )	
	{
		if(StdBy_Off_rsn[path4] == _SET_)
		{
			StdBy_Off_rsn[path4] = _CLR_;			
			print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[PAU_EN_Ctrl] PAU#%d : StdBy_Off_rsn Clear.",	path4+1);
		}
	}
	//----------------------------------
	#ifdef __TYPE__PRU__ // ANTS / HFR
	//----------------------------------
	// 2019.04.26.
	if(IS_PAU_Algorithm_Off(path4)) 
	{
		if(cmd == AMP_CMD_EN)
			return cmd;	
	}
	
	Check_DPD_DCL_Reset(path4, en);
	cmd = PAU_Send_Data(path4, cmd, NULL, 0);
	//----------------------------------
	#else // ANTS AAU
	//----------------------------------
	AAU__RF_AMP_OnOff(path4, en, 827);	
	m_PAU_C_Req[path4].Rvs_AMP_En = en & 0x01;	// AAU
	//----------------------------------
	#endif	
	//----------------------------------
	
	return cmd;	
}

#ifdef __TYPE__PRU__
INT8U PRU_User_AMP_EN_Ctrl(INT8U path4, INT8U en)
{
	INT8U cmd;
	
	//----------------------------------------------------------
	// Amp Not Controlled
	//
	// when AC Fail / STDBY mode
	//----------------------------------------------------------		
	
	if(AC_Fail_Chk(path4))
		return 0;

	if(IS__Standby_Status(path4)) // 2020.01.13
	{
		return 0;	
	}
	
	//if(m_St.MIMO_Stdby_Enable == ON)  // 2020.01.13 : deleted
	{
		if (Mimo_Stby_Ctrl_Req[path4]) return 0;	
	}	
	if(m_St.AMP_Stdby_Enable == ON)
	{
		if(AMP_Stby_Off_Req[path4]) return 0; // AMP_Std가 진행중이어서 아직 exit 안되었으면.
	}
	/*
	User가 off 제어하면, 이부분이 amp-std algorithm보다 먼저 동작.
	AMP_Stdby_Mode_Onoff가 off 되면서 아래 code 실행.
	먼저 AMP ON.
	*/	
	//----------------------------------------------------------
	
	if( en == OFF ) cmd = AMP_CMD_DIS;
	else            cmd = AMP_CMD_EN;

	if( en == ON )	
	{
		if(StdBy_Off_rsn[path4] == _SET_)
		{
			StdBy_Off_rsn[path4] = _CLR_;			
			print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[PAU_EN_Ctrl] PAU#%d : StdBy_Off_rsn Clear.",	path4+1);
		}
	}	
	//----------------------------------	
	/*
	--------------------------
	2020.04.01 : PAU DL Shutdown 상태일 때, PAU 'ON/OFF' 유저 제어 안되는 현상
	아래 return 되지 않도록 수정.
	--------------------------
	////if(IS_PAU_Algorithm_Off(path4)) 
	////{
	////	if(cmd == AMP_CMD_EN)
	////		return cmd;	
	////}
	--------------------------
	*/
	
	Check_DPD_DCL_Reset(path4, en);
	cmd = PAU_Send_Data(path4, cmd, NULL, 0);
	//----------------------------------	
	return cmd;	
}
#endif

//------------------------------------------------------------------
INT8U PAU_RESET_Ctrl(INT8U path4)
{
	return PAU_Send_Data(path4, AMP_CMD_RST_Ctrl, NULL, 0);
}


void Fwd_Out_Att_Set(INT8U path4, INT8U data)
{
	if(data > MAX_FWD_ATT_x2) data = MAX_FWD_ATT_x2;

	m_PAU_C_Req[path4].Fwd_Att_x2 = data;
	
#ifdef __TYPE__PRU__ // ANTS-PRU		
	Add_PAU_N_Ctrl_Data( path4, 0, 7, &data ); // USER ATT	
#endif		
	
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
	// 2019.04.06. PRU Only	
	m_Reg_Flg.DSP_Common_MIMO[path4].PAU_DL_Attn_Sync.Data = 1;
	m_Reg_Ctl.DSP_Common_MIMO[path4].PAU_DL_Attn_Sync.Data = data;
#endif
}
//------------------------------------------------------------------
void Rvs_Amp_OnOff(INT8U path4, INT8U data ) // PRU,AAU
{
/*
	if(data == ON)
		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d RVS_Amp_ON\n", path4+1);
	else
		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d RVS_Amp_OFF\n", path4+1);
*/		
#ifdef __TYPE__PRU__ // ANTS / HFR

/*
RuInit(6/6)되기 전, LNA ON/OFF 상태 toggle 되지 않도록 수정.
*/	
	if(m_Env.AMP_Ctrl_Allowed==0) // 2020.01.14		
		data = 1; // (1 : Disable)

	Add_PAU_N_Ctrl_Data( path4, 2, 6, &data ); // @5G : Byte 2, bit 6
#else	// ANTS-AAU
	AAU__RF_AMP_OnOff(path4, data, 873);
#endif
}

void TPL_Fwd_Att_Set( INT8U path4, INT8U Att_x2 )
{
	if(Att_x2 > MAX_FWD_ATT_x2) Att_x2 = MAX_FWD_ATT_x2;
		
	Add_PAU_N_Ctrl_Data( path4, 0, 6, &Att_x2 );
	m_PAU_C_Req[path4].Tpl_Att_x2 = Att_x2;
}

// 
//============================================================================
void Init_PAU_N_Ctrl( INT8U path4 )
{
#ifdef __TYPE__PRU__
	pthread_mutex_lock(&PAU_Ctrl_Sem[path4]);
	
	// Ctrl 구조체 0.
	memset((INT8U*)&m_PAU_Ctrl[path4], 0, sizeof(AMP_TRx_CTRL_STR)); 
	memset((INT8U*)&m_THRESH_Ctrl[path4], 0, sizeof(THRE_CTRL_STR)); 

	PAU_Ctrl_Req[path4] = 0; 
	THR_Ctrl_Req[path4] = 0;
#endif
}

//============================================================================
// 미사용이거나 하드웨어 인서트 안되어 있으면, 제어될 필요 없게.
void PAU_Ctrl_Req_Chk( INT8U path4 )
{	
	INT8U ctrl_cancel_Req=0;

	if( Check_PAU_Use(path4)==0 )
	{
		ctrl_cancel_Req=1;
	}
	
	// 미사용 등이어서 제어 취소이면, Req 변수 클리어.
	if(ctrl_cancel_Req)
	{
		PAU_Ctrl_Req[path4] = 0; 
		THR_Ctrl_Req[path4] = 0;
	}
}
//============================================================================

//============================================================================
// byte, bit is position of Flag
// data is control data
// 오류의 경우, 리턴 0.
INT8U Add_PAU_N_Ctrl_Data( INT8U path4, INT8U byte, INT8U bit, INT8U* data )
{	
#ifdef __TYPE__AAU__
	(void)path4; (void)byte; (void)bit; (void)data;
	return 0;
#else
	AMP_TRx_CTRL_STR* pPAU_Ctrl;	
	
	// Flag 위치에 따라 제어 데이터 생성(OR-ing).
	if(path4 >= AMPU_4_MAX) return 0;	
	if(byte > 7) return 0;
	if(bit > 7) return 0;

	pPAU_Ctrl = &m_PAU_Ctrl[path4];	
	
	switch(byte)
	{
		//---------------------
		case 0:
		//---------------------
			switch(bit)
			{
				case 7: pPAU_Ctrl->TX_Att_x2 = data[0]; break;		   // TX0 Atten
				case 6: pPAU_Ctrl->TX_TPL_Att_x2 = data[0]; break;     // TX0 TPL Atten, not used @ 5G
				case 5: pPAU_Ctrl->TX_Gain_C_Att_x2 = data[0]; break;  // TX0 Gain보상 Atten
				case 4: pPAU_Ctrl->FB_ATT_x2 = data[0]; break;         // FB0 Atten
				case 3: pPAU_Ctrl->FB_Offset_ATT_x2 = data[0]; break;  // FB0 Gain보상 Atten
				case 2: memcpy(pPAU_Ctrl->TX_PLL_khz, data, 4); break; // Tx0 PLL Freq, not used @ 5G
				case 1: memcpy(pPAU_Ctrl->RX_ALC_Lvl, data, 2); break; // Rx0 ALC Level
				case 0: memcpy(pPAU_Ctrl->RX_SD_Lvl, data, 2); break;  // Rx0 Shutdown Level
				default: 
					return 0;
			}	
			break;
		//---------------------	
		case 1:
		//---------------------
			switch(bit)
			{
				case 7: pPAU_Ctrl->RX_Att_x2 = data[0]; break;        // Rx0 Atten
				case 6: pPAU_Ctrl->RX_Gain_C_Att_x2 = data[0]; break; // Rx0 Gain보상 Atten
				case 5: pPAU_Ctrl->RX_GainBal_x2 = data[0]; break;    // Rx0 Gain Balance Value
				case 4: pPAU_Ctrl->RX_ALC_Onoff = data[0]; break;     // Rx0 ALC On/Off 상태
				case 3: pPAU_Ctrl->RX_GainBal_Onoff = data[0]; break; // Rx0 Gain Balance On/Off 상태				
				case 2: return 0;
				case 1: return 0;
				case 0: return 0;
				default: 
					return 0;
			}	
			break;	
		//---------------------
		case 2:
		//---------------------
			switch(bit)
			{
				case 7: return 0;   // CW0 PLL Freq, not used @ 5G
				case 6: pPAU_Ctrl->RX_Path_En = data[0]; break; // Rx0 Path Enable
				
				default: 
					return 0;
			}	
			break;	
		//---------------------
		case 3:
		case 4:
		case 5:
			return 0;
		//---------------------
		case 6:
		//---------------------
			switch(bit)
			{
				case 7: pPAU_Ctrl->Temp_C_Onoff = data[0]; break;   // 온도 보상 On/Off
				case 6: pPAU_Ctrl->RVS_IN_Auto_SD = data[0]; break; // Rx 과입력에 대한 Auto Shutdown Mode
				case 5: pPAU_Ctrl->Att_Pair_OnOff = data[0]; break; // ATTEN Pair On/Off
				case 4: pPAU_Ctrl->EQ_State = data[0]; break; // EQ 정보

				default: 
					return 0;
			}	
			break;		
		//---------------------
		default : return 0;
		//---------------------
	}

	pPAU_Ctrl->Flag[byte] |= cbits[bit];
	PAU_Ctrl_Req[path4] = 1;	

	return 1;
#endif
}

//============================================================================
// byte, bit is position of Flag
// data is control data
// 오류의 경우, 리턴 0.
//============================================================================
INT8U Add_THRE_N_Ctrl_Data( INT8U path4, INT8U byte, INT8U bit, INT8U* data )
{
#ifdef __TYPE__AAU__
	(void)path4; (void)byte; (void)bit; (void)data;
	return 0;
#else

	THRE_CTRL_STR* pThresh_Ctrl;	

	if(path4 >= AMPU_4_MAX) return 0;	
	if(byte > 2) return 0;
	if(bit > 7) return 0;
	
	pThresh_Ctrl = &m_THRESH_Ctrl[path4];
	
	// Flag 위치에 따라 제어 데이터 생성(OR-ing).
	//---------------------
	switch(byte)
	{
		//---------------------
		case 0:
		//---------------------
			switch(bit)
			{
				case 7: memcpy( pThresh_Ctrl->Over_Temp_Val, data, 2 ); break;   // Over Temp
				case 6: memcpy( pThresh_Ctrl->Over_Temp_Rec, data, 2 ); break;   // Over Temp(Recovery)
				case 5: memcpy( pThresh_Ctrl->Over_Pwr_Val, data, 2 ); break;    // Over Power Value(Shutdown)
				case 4: memcpy( pThresh_Ctrl->Return_Loss, data, 2 ); break;     // Return Loss
				case 3: memcpy( pThresh_Ctrl->Return_Loss_Rec, data, 2 ); break; // Return Loss(Recovery)
				case 2: memcpy( pThresh_Ctrl->Fwd_Offset_Val, data, 2 ); break;  // Forward Offset
				case 1: return 0;
				case 0: memcpy( pThresh_Ctrl->FB_Pwr_Offset, data, 2 ); break;  // FB0 Power Offset
				default: 
					return 0;
			}	
			break;			
		//---------------------
		case 1:
		//---------------------
			switch(bit)
			{
				case 7: return 0; // FB1 Power Offset, not used @ 5G
				case 6: memcpy( pThresh_Ctrl->Rx_In__Pwr_Offset, data, 2 ); break; // Rx0 Input Offset
				case 5: memcpy( pThresh_Ctrl->Rx_Out_Pwr_Offset, data, 2 ); break; // Rx0 Power Offset
				case 4: return 0; // Rx1 Input Offset, not used @ 5G
				case 3: return 0; // Rx1 Power Offset, not used @ 5G
				case 2: memcpy( pThresh_Ctrl->TX_Dly_Offset, data, 2 ); break;     // Tx0 Delay Offset
				case 1: return 0; // Tx1 Delay Offset, not used @ 5G
				case 0: memcpy( pThresh_Ctrl->RX_Dly_Offset, data, 2 ); break;     // Rx0 Delay Offset
			
				default: return 0;
			}	
			break;
		//---------------------
		case 2:
		//---------------------
			switch(bit)
			{
				case 7: return 0; // Rx1 Delay Offset, not used @ 5G			
				default: return 0;
			}	
			break;

		//---------------------
		default : return 0;
		//---------------------		
	}	

	pThresh_Ctrl->Flag[byte] |= cbits[bit];
	THR_Ctrl_Req[path4] = 1;	

	return 1;
#endif	
}
//============================================================================

INT8U Send_PAU_N_Ctrl(INT8U path4)
{	
#ifdef __TYPE__AAU__
	(void)path4;
	return 0;
#else
	INT8U ret = 0;

	PAU_Ctrl_Req_Chk(path4); // 미사용이거나 실제 하드웨어 인스톨 안되어 있으면, 제어 내려가지 않게 함.
	
	if( Check_PAU_Use(path4)==0 )
	{
		PAU_Ctrl_Req[path4] = 0;
		THR_Ctrl_Req[path4] = 0;
	}	
	//----------------------------------------
	if(PAU_Ctrl_Req[path4])
	{	
		ret = PAU_Send_Data(path4, AMP_CMD_TRx_PATH_Ctrl, (INT8U*)&m_PAU_Ctrl[path4], sizeof(AMP_TRx_CTRL_STR));
	}
	PAU_Ctrl_Req[path4] = 0; // init
	//----------------------------------------
	if( THR_Ctrl_Req[path4] )
	{
		ret = PAU_Send_Data(path4, AMP_CMD_TH_Ctrl, (INT8U*)&m_THRESH_Ctrl[path4], sizeof(THRE_CTRL_STR));
	}
	THR_Ctrl_Req[path4] = 0; // init
	//----------------------------------------
	pthread_mutex_unlock(&PAU_Ctrl_Sem[path4]);
	//----------------------------------------
	return ret;
#endif	
}

INT8U Send_PAU_N_Ctrl_Not_Used_AMP(INT8U path4)
{	
#ifdef __TYPE__AAU__
	(void)path4;
	return 0;
#else
	INT8U ret = 0;
		
	ret = PAU_Send_Data(path4, AMP_CMD_TRx_PATH_Ctrl, (INT8U*)&m_PAU_Ctrl[path4], sizeof(AMP_TRx_CTRL_STR));
	
	pthread_mutex_unlock(&PAU_Ctrl_Sem[path4]);

	return ret;
#endif	
}

//============================================================================
//----------------------------------------------------------
// path4 에 해당하는 PAU의 inventory를 얻는다.
INT8U sendPAU_GetInfo(INT8U path4)
{
	if(path4 >= AMPU_4_MAX) 
		return 0;

	return PAU_Send_Data(path4, AMP_CMD_INFO, NULL, 0);
}

//----------------------------------------------------------
// INFO 응답 처리 함수.
//----------------------------------------------------------
void Recv_PAU_Info(INT8U path4, INVENTORY_STR* inv, INVENTORY_2_STR* inv2)
{	
	char str[20];
	
	m_St.Pwr_Amp_Made[path4] = inv->maker; // AID 와 AMP protocol의 code 값이 같음.
	m_St.Pwr_Amp_Ver[path4] = inv->Ver;

	memcpy(&m_St.Pwr_Amp_SN[path4][0], inv->sn, 4);	
	memcpy(&m_St.Pwr_Amp_Date[path4][0], inv->date, 3);
	memcpy(&m_St.PAU_I_Ver[path4][0], inv2->Ver2, 3);

	memcpy(&PAU_Info[path4], (INT8U*)inv, sizeof(INVENTORY_STR));	

	////print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "PAU#%d Info : maker1 0x%02X", path4+1, inv->maker);
	
	     if(inv->maker == 0x11) {inv->maker = 0xCF; m_St.Inv_Amp_Made[path4] = Code_NWV;} // NewWave, 3.5G
	else if(inv->maker == 0x06) {inv->maker = 0xD6; m_St.Inv_Amp_Made[path4] = Code_WvE;} // WE(Wave Electronic), 3.5G
	else if(inv->maker == 0x10) {inv->maker = 0xD0; m_St.Inv_Amp_Made[path4] = 0;}        // KRF, reserved.
	else                        {inv->maker = 0x00; m_St.Inv_Amp_Made[path4] = 0;}	

	//D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] maker2 : 0x%02X.\r", path4+1, inv->maker);
	//D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] supply : 0x%02X.\r", path4+1, inv->supply);
	
	RecvModuleInventory( Get_Inv_idx(path4), inv );		

	Set_Ver_Info_AMP(path4, m_St.Pwr_Amp_Made[path4], m_St.PAU_I_Ver[path4] );

	/*	
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Type : 0x%02X.\r", path4+1, inv->Type);
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Rep-Type : 0x%02X-%02X.\r", path4+1, inv->Rep_Type[0], inv->Rep_Type[1]);
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Pwr_Amp_Made : 0x%02X.\r", path4+1, m_St.Pwr_Amp_Made[path4]); 	
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Pwr_Amp_Ver : 0x%02X.\r", path4+1, m_St.Pwr_Amp_Ver[path4]);
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Pwr_Amp_SN : %c%c%c%c.\r", path4+1, 
		m_St.Pwr_Amp_SN[path4][0], m_St.Pwr_Amp_SN[path4][1], m_St.Pwr_Amp_SN[path4][2], m_St.Pwr_Amp_SN[path4][3]);

	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Pwr_Amp_Date : 0x%02X%02X%02X.\r", path4+1, 
		m_St.Pwr_Amp_Date[path4][0], m_St.Pwr_Amp_Date[path4][1], m_St.Pwr_Amp_Date[path4][2]);

	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] PAU_I_Ver : 0x%02X%02X%02X.\r", path4+1, 
		m_St.PAU_I_Ver[path4][0], m_St.PAU_I_Ver[path4][1], m_St.PAU_I_Ver[path4][2]);
	*/


	memset(str, 0, 20);
	memcpy(str, inv->ERP_code, 15);

	memset(str, 0, 20);
	memcpy(str, inv->Maker_Mode, 4);
	
	/*
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] ERP_code : %s.\r", path4+1, str);
	D_Printf( DEBUG_SYSTEM, "PAU Info [#%d] Maker_Mode : %s.\r", path4+1, str);
	*/	
}
//------------------------------------------------------------------------------------
static INT8U info_ctrl[50];

INT8U sendPAU_INFO_Ctrl(INT8U path4, INT8U* pCtrlVal, INVENTORY_STR* inv)
{	
	INT8U ret = 0; // init 0	
	INT16U len;

	if(path4 >= AMPU_4_MAX) return ret;
	
	info_ctrl[0] = pCtrlVal[0];
	info_ctrl[1] = pCtrlVal[1];
	memcpy(&info_ctrl[2], (INT8U*)inv, sizeof(INVENTORY_STR));
	len = sizeof(INVENTORY_STR)+2;

	info_ctrl[len++] = 0; // version, protocol 상.
	info_ctrl[len++] = 0; // version, protocol 상.
	info_ctrl[len++] = 0; // version, protocol 상.

	ret = PAU_Send_Data(path4, AMP_CMD_INFO_Ctrl, info_ctrl, len);

	return ret;	
}
//============================================================================

	

//============================================================================


//============================================================================




//----------------------------------------------------------
// PAU 상태를 폴링.
#define PAU_LINK_FAIL_CO (10)
INT8U PAU_Polling(INT8U path4)
{
	INT8U ret = 0; // init 0
	INT8U fail_max_cnt = PAU_LINK_FAIL_CO;
	
	if (JIG_MODE == JIG_MODE_CODE)
	{		
		fail_max_cnt = 3;
	}

	if (path4 >= AMPU_4_MAX) return ret;

	PAU_Link_Fail_Co[path4]++;

	if( PAU_Link_Fail_Co[path4] >= fail_max_cnt )
	{
		PAU_Link_Fail_Co[path4] = fail_max_cnt;
		m_St.Pwr_Amp_Link_Err[path4] = 1;
		PAU_Link_Fail_Set( path4, 1 );
	}	

	ret = PAU_Send_Data(path4, AMP_CMD_ST, NULL, 0);

	if( ret==1 )
	{
		PAU_Link_Fail_Co[path4]=0;
		m_St.Pwr_Amp_Link_Err[path4] = 0;
		PAU_Link_Fail_Set(path4, 0);
	}
	return ret;
}
//-------------------------------------------------------------------
void PAU_Auto_SD_Chk( INT8U path4 )
{
	INT8U req = m_Eep.Auto_SD_Mode_En[path4];
	
	if(m_St.AMP_Auto_SD_Mode_En[path4] != req)
	{		
		Add_PAU_N_Ctrl_Data( path4, 6, 6, &req ); // 5G
	}
}
//------------------------------------------------------------------

void AMP_Init_Set(INT8U path4)
{	
	// 초기화 위해.
	Pau_Init_End[path4] = (-1);
	Init_PAU_Down_St(path4);	
	PAU_Inv_Read_Req(path4);
}
//------------------------------------------------------------------
#ifdef __TYPE__PRU__ // PRU Only
static INT8U rst_standby[AMPU_4_MAX] = {0,0,0,0};
#endif

/*
-------------------------------------------------------------
// Used When download from GUI (NOT Package download)
-------------------------------------------------------------
*/
INT16U PAU_DownLoad_PassData( INT8U type, PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
#ifdef __TYPE__PRU__ // PRU Only
	INT16U l, len = 0;	
	INT8U Mod_SUBID[3];
	INT8U path4;	
	INT8U ComPort;
	INT8U pau_type;
	INT8U gui_type;
	INT8U pau_id;
	INT8U totPkt = 1, curPkt = 1;
	
	// Module SUBID Check
	Mod_SUBID[0] = rxf->data[0];
	Mod_SUBID[1] = rxf->data[1];
	Mod_SUBID[2] = rxf->data[2];

	switch (Mod_SUBID[2])
	{		
		case 0x01 : path4 = AMPU_4_1; ComPort = COMM_UART_AMP_1; break;
		case 0x02 : path4 = AMPU_4_2; ComPort = COMM_UART_AMP_2; break;
		case 0x03 : path4 = AMPU_4_3; ComPort = COMM_UART_AMP_3; break;
		case 0x04 : path4 = AMPU_4_4; ComPort = COMM_UART_AMP_4; break;	
		
		default: return 0;		
	}

	OSTimeDly(5*OS_DELAY_10msec); // 50[ms] 이상 이어야 함.
	

	pthread_mutex_lock(&PAU_COMM_Evt[path4]);

	
	// cpy len and data
	pau_txf[path4].len = rxf->len - 3; // From index 3
	l = pau_txf[path4].len;
	
	memcpy( pau_txf[path4].data, &rxf->data[3], l );

	// 다운로드 명령(타입)을 PAU가 사용하는 타입으로 변경.
	switch( type )
	{	
		case 0xE8: 
			pau_type = AMP_CMD_DN_NOTI_FW;
			
			if(pau_txf[path4].data[3] == 0) 
				rst_standby[path4] = 1;
			else
				rst_standby[path4] = 0;

			D_Printf( DEBUG_SYSTEM, "\r\n [RCU] AMP#%d Reset_Standby(%d)", path4+1, rst_standby[path4]);			
			break;			
			
		case 0xE0: 
			pau_type = AMP_CMD_DN_NOTI;  
			rst_standby[path4] = 0;  
			break;
			
		case 0xE1: pau_type = AMP_CMD_DN_NOTI_CHK; break;
		case 0xE2: pau_type = AMP_CMD_DN_DATA; break;
		case 0xE3: pau_type = AMP_CMD_DN_END; break;
	}	


	// RCU
	pau_txf[path4].ScrAddr = 0x00;
	pau_id = _PAUID_PAU_ + path4;
	// 2019.08.12 : PAUID : 0x20 -> 0x20/21/22/23
	////pau_id = _PAUID_PAU_;
	pau_txf[path4].DesAddr = pau_id;
	
	len = Make_Frame_PAU( pau_type, &pau_txf[path4], l, path4 );

	if( Send_DN_Data( ComPort, (INT8U*)&pau_txf[path4], len, (INT8U*)&pau_rxf[path4], PAU_Timout_ms ) )
	{
		if( (pau_rxf[path4].Type == pau_type) && (pau_rxf[path4].ScrAddr == pau_id) )
		{			
			//D_Printf( DEBUG_SYSTEM, "\r\n Rx PAU Dn [%d] 0x%02X.",path4, pau_type); 		

			switch( pau_type )
			{	
				case AMP_CMD_DN_NOTI_FW:  gui_type = 0xE8; break;
				case AMP_CMD_DN_NOTI:	  gui_type = 0xE0; break;
				case AMP_CMD_DN_NOTI_CHK: gui_type = 0xE1; break;
				case AMP_CMD_DN_DATA:	  gui_type = 0xE2; break;				
				case AMP_CMD_DN_END:	  gui_type = 0xE3; break;
			}			
			txf->Type = gui_type;
			//----------------------
			txf->len = pau_rxf[path4].len; 	
			
			if( pau_type == AMP_CMD_DN_END) m_Env.AMP_Dn_St[path4] = 2;
			
			// Module SUBID 3byte 이후부터 복사.	
			memcpy( &txf->data[3], pau_rxf[path4].data, txf->len);
			
			// Module SUBID 3byte 추가.
			txf->data[0] = Mod_SUBID[0];
			txf->data[1] = Mod_SUBID[1];
			txf->data[2] = Mod_SUBID[2];
			
			txf->len += 3;

			len = Make_Frame( txf->Type, rxf->Rcode, txf, txf->len, rxf->seqNo16, totPkt, curPkt );

			if (gui_type == 0xE3)
			{
				if(rst_standby[path4])
				{
					m_Env.is_down_load = 0;
					rst_standby[path4] = 0; // init
				}
				else
				{
					AMP_Init_Set(path4);
					m_Env.Pwr_AMP_Download_Req[path4] = 1;
				}				
			}
			//-----------------------------------------------------	
			pthread_mutex_unlock(&PAU_COMM_Evt[path4]);
			return len;
		}
	}
	pthread_mutex_unlock(&PAU_COMM_Evt[path4]);
	CheckUART_Error( ComPort );
#endif	
	return 0;
}
//------------------------------------------------------------------


#ifdef __TYPE__PRU__ // PRU Only
static OS_TIMER timer_PRU_Dl_Out_Alm[AMPU_4_MAX];
static OS_TIMER timer_PRU_Dl_Out_NRM[AMPU_4_MAX]; // Normal

INT8U Chk__PRU_DL_Out_High_Alarm(INT8U path4, INT8U alarm)
{	
	static INT8U init[AMPU_4_MAX] = {0,0,0,0};
	INT8U last_alm;
	INT8U temp_alm;

	if(path4 >= AMPU_4_MAX) return 0;

	if(init[path4] == 0)
	{
		init[path4] = 1;
		OSTimerReset(&timer_PRU_Dl_Out_Alm[path4]);
		OSTimerReset(&timer_PRU_Dl_Out_NRM[path4]);
	}
	
#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__ // 2019.12.23
	if(IS__Clock_CPRI_Alarm())
	{
		OSTimerReset(&timer_PRU_Dl_Out_Alm[path4]);
		OSTimerReset(&timer_PRU_Dl_Out_NRM[path4]);
	}
#endif

	last_alm = (m_St.AlmSt.Path[path4].DL.Out_Upper > 0)? 1:0;
	temp_alm = (alarm > 0)? 1:0;

	if(temp_alm == last_alm) // same status
	{
		OSTimerReset(&timer_PRU_Dl_Out_Alm[path4]);
		OSTimerReset(&timer_PRU_Dl_Out_NRM[path4]);
		
		return temp_alm;
	}
	else if(temp_alm) // for last.normal -> last.alarm
	{
		OSTimerReset(&timer_PRU_Dl_Out_NRM[path4]);
		if(OSTimerGet_msec(&timer_PRU_Dl_Out_Alm[path4]) > (3*TICK_CO_SEC))
		{
			OSTimerReset(&timer_PRU_Dl_Out_Alm[path4]);
			return 1; // alarm
		}
		else 
		{
			return 0; // normal
		}
	}
	else // for last.alarm -> last.normal
	{
		OSTimerReset(&timer_PRU_Dl_Out_Alm[path4]);
		if(OSTimerGet_msec(&timer_PRU_Dl_Out_NRM[path4]) > (3*TICK_CO_SEC))
		{
			OSTimerReset(&timer_PRU_Dl_Out_NRM[path4]);
			return 0; // normal
		}
		else 
		{
			return 1; // alarm
		}
	}	
}
#endif

INT8U IS__TSSI_Low_Alarm(INT8U path4)
{
	INT8U alarm__tssi_low;
	INT8U alarm_100 = 0;
	INT8U alarm___A = 0;

	if(IS__Vaild_CPRI__Main() && m_St.AlmSt.Path[path4].DL.TSSI_Lower__) alarm_100 = 1; 
	else alarm_100 = 0;
	
	if(IS__Vaild_CPRI_Alpha() && m_St.AlmSt.Path[path4].DL.TSSI_Lower_a) alarm___A = 1; 
	else alarm___A = 0;
		
	
	if(alarm_100 || alarm___A)
		alarm__tssi_low = 1;
	else
		alarm__tssi_low = 0;
	
	return alarm__tssi_low;
}

/*
---------------------------------------------------------------
2020.02.24 : 출력하한 알람 30분 발생시, system reset
---------------------------------------------------------------
*/
static OS_TIMER timer_AAU_Dl_Out_Low_30m;
void Proc__AAU_Reset__By_OutLow_Alarm(void)
{
	INT8U path4;
	INT8U use, alm, normal;
	static INT8U init = 0;
	static INT8U timer_30m_start = 0;
	INT8U max_reset_cnt = 3;
	static int prev_sec;
	int sec;
	static INT8U check_en = 1; // default

	if(init == 0)
	{
		init = 1;
		if(m_Eep.AAU_Reset_by_Low_Alarm_flag != 0x20200224) // if not initialized, like 0xFF.
		{	
			m_Eep.AAU_Reset_Cnt_by_Low_Alarm = 0;
			m_Eep.AAU_Reset_by_Low_Alarm_flag = 0x20200224;			
			m_Env.is_eep_write = EEP_WRITE_CODE;

			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[OutLow_Alarm] reset count initial.");
		}
		
		if(m_Eep.AAU_Reset_Cnt_by_Low_Alarm >= max_reset_cnt) // if max reset count
		{
			check_en = 0; // do not check if already max_reset_cnt

			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[OutLow_Alarm] max reset count(%d), algorithm disabled.", 
						m_Eep.AAU_Reset_Cnt_by_Low_Alarm);
			
			m_Eep.AAU_Reset_Cnt_by_Low_Alarm = 0;
			m_Env.is_eep_write = EEP_WRITE_CODE;
		}
		else
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[OutLow_Alarm] curr reset count(%d)", 
						m_Eep.AAU_Reset_Cnt_by_Low_Alarm);
		}
	}
	
	//---------------------------------------------------------------
	if(check_en == 0) return;
	//---------------------------------------------------------------
	
	use = 0;
	alm = 0;
	normal = 0;
	
	for(path4 = 0; path4 < MAX_PATH_4; path4++)
	{
		if(Check_PAU_Use(path4))
		{
			use |= cbits[path4];
			
			if(m_St.AlmSt.Path[path4].DL.Out_Lower)
				alm |= cbits[path4];
			else
				normal |= cbits[path4];
		}
	}

	if(timer_30m_start == 0)
	{
		OSTimerReset(&timer_AAU_Dl_Out_Low_30m);
		
		if(use && (use == alm))
		{
			if(m_Eep.AAU_Reset_Cnt_by_Low_Alarm < max_reset_cnt)
			{
				timer_30m_start = 1;

				for(path4 = 0; path4 < MAX_PATH_4; path4++)
				{
					if(Check_PAU_Use(path4)==0) continue;
					
					if(alm & cbits[path4])
					{
						// payload on
						
						if(Is_SFP_Inserted(CPRI_0)) 
						{
							Use_DTU_DL_Path_Onoff(FA_0, path4, ON);
							Use_DTU_UL_Path_Onoff(FA_0, path4, ON);
						}
						if(IS_Alpha_Mhz_Existing() && Is_SFP_Inserted(CPRI_1)) 
						{
							Use_DTU_DL_Path_Onoff(FA_1, path4, ON);
							Use_DTU_UL_Path_Onoff(FA_1, path4, ON);
						}						
					}
				}
			}
		}
	}
	else // already start
	{		
		if((use && normal) || (use == 0)) // 1개라도 사용인데, normal 이면, 
		{
			timer_30m_start = 0;

			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[OutLow_Alarm] reset count (%d) cleared.", 
						m_Eep.AAU_Reset_Cnt_by_Low_Alarm);
						
			m_Eep.AAU_Reset_Cnt_by_Low_Alarm = 0;
			m_Env.is_eep_write = EEP_WRITE_CODE;
		}		
	}

	if(timer_30m_start)
	{
		sec = OSTimerGet_msec(&timer_AAU_Dl_Out_Low_30m);

		if(sec >= (30*TICK_CO_MIN)) // (120*TICK_CO_SEC)
		{
			OSTimerReset(&timer_AAU_Dl_Out_Low_30m);

			m_Eep.AAU_Reset_Cnt_by_Low_Alarm++;
			m_Env.is_eep_write = EEP_WRITE_CODE;
			
			// reset system			
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[OutLow_Alarm] reset count %d, reboot start", m_Eep.AAU_Reset_Cnt_by_Low_Alarm);
			
			SetResetInfo( Rst_By_Etc, 1872 );
			m_Env.is_reboot = 1;
		}
		else
		{
			if(prev_sec != (sec/10000))
				print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[OutLow_Alarm] %d[s] elapsed/ reset cnt %d.", 
							prev_sec*10000, m_Eep.AAU_Reset_Cnt_by_Low_Alarm);

			prev_sec = sec/10000;
		}
	}
	else 
	{
		OSTimerReset(&timer_AAU_Dl_Out_Low_30m);
		prev_sec = 0;
	}
}

//-------------------------------------------------
void Update_Fwd_Pwr_Alm( INT8U path4 )
{
	INT8U alarm;
	INT8U alm_mask_req;
	INT8U chk_req = 0;
	//INT8U alarm__tssi_low;
	//static INT8U alarm__tssi_low_prev[MAX_PATH_4] = {0,0,0,0};
	
	if(m_St.AlmTest) // 2019.05.01
		return;
	//----------------------------------------------
	if(Check_PAU_Use(path4) == 0)
	{
		m_St.AlmSt.Path[path4].DL.Out_Upper = 0;
		m_St.AlmSt.Path[path4].DL.Out_Lower = 0;

	#ifdef __TYPE__PRU__	
		Chk__PRU_DL_Out_Lower_Alarm(path4, 0, 1, 0); // for timer reset
		Chk__PRU_DL_Out_High_Alarm(path4, 0); // for timer reset
	#else
		Chk__AAU_DL_Out_Lower_Alarm(path4, 0); // for timer reset
	#endif		
	}
	else
	{
		//-------------------------------------------------------
		// Forward Over PWR Alarm
		//-------------------------------------------------------	
		alarm = m_St.AlmSt.Path[path4].DL.Out_Upper;
	#ifdef __TYPE__PRU__	
		
		if(AMP_Alarm_St(path4) == ON)
		{
			Hysterisis_Alm_Chk(&alarm, m_St.Fwd_Out_Pwr_x10[path4], m_Eep.Fwd_Out_Lmt_x10[path4][HI], 20);
			alarm = Chk__PRU_DL_Out_High_Alarm(path4, alarm);
		}
		else 
		{
			alarm = 0; // AMP-OFF, mask Upper alarm
			Chk__PRU_DL_Out_High_Alarm(path4, 0); // for timer reset
		}
	#else // __TYPE__AAU__	
		if(AMP_Alarm_St(path4) == ON)
			Hysterisis_Alm_Chk(&alarm, m_St.Fwd_Out_Pwr_x10[path4], m_Eep.Fwd_Out_Lmt_x10[path4][HI], 20);
		else 
			alarm = 0; // AMP-OFF, mask Upper alarm
				
		/*		
		2019.11.18 : when FWD_ALC_Att > Max (15.0dB), Check Alarm
		(출력 상한 alarm : FWD_ALC_Att > Max (15.0dB) 의 상태에서 Alarm Check 하도록 수정.)
		*/
		if(alarm)
		{
			if(m_St.TX_ALC_Att_x10[path4] < (15*10))
				alarm = 0;
		}
	#endif
	
		m_St.AlmSt.Path[path4].DL.Out_Upper = alarm;
		// 2019.11.27
		if(m_St.AlmSt_prev.Path[path4].DL.Out_Upper != m_St.AlmSt.Path[path4].DL.Out_Upper)
		{
			m_St.AlmSt_prev.Path[path4].DL.Out_Upper = m_St.AlmSt.Path[path4].DL.Out_Upper;
			Update_Alm_Str_Lmt( 0x51, 0x04, path4, 0, m_St.AlmSt.Path[path4].DL.Out_Upper); // DL 출력 상한
		}		

		//-------------------------------------------------------
		// Forward Low PWR Alarm
		//-------------------------------------------------------		
		     if( m_Eep.Pwr_Amp_En[path4] == OFF )    alm_mask_req = 1;		
		else if(IS__Vaild_CPRI__Main() && Is_RVS_Spurious(_100M, path4)) alm_mask_req = 1; // 2020.01.28
		else if(IS__Vaild_CPRI_Alpha() && Is_RVS_Spurious(Alpha, path4)) alm_mask_req = 1; 
		else if(AMP_Alarm_St(path4) == OFF) alm_mask_req = 1;		
	#ifdef __TYPE__AAU__
		/*
		// 2020.01.29
		DL TSSI 하한 Alarm 발생 시 DL 출력 하한 Alarm은 Masking. (GUI 출력 하한 Alarm Green. 상위 보고 하지 않음.)
		DL 출력 하한 Alarm 발생 후 DL TSSI 하한 Alarm 발생 시 출력 하한 Alarm은 유지 및 TSSI 하한 Alarm 발생
		*/
		else if(m_St.AlmSt.Path[path4].DL.Out_Lower == 0)
		{
			if(IS__TSSI_Low_Alarm(path4)) alm_mask_req = 1; 			
			else alm_mask_req = 0; // init no mask
		}
	#endif
		else
		{
			alm_mask_req = 0; // init no mask
		}

	#ifdef __TYPE__PRU__
		if(m_St.AlmSt.A_0.AC == 0) // AC 정상일때만 알람 체크.
		{				
			     if(m_St.AMP_Stdby_St[path4])          chk_req = 0;
			else if(m_St.AMP_Stdby_St_Dly_REQ[path4] ) chk_req = 0;
			else if(m_St.MIMO_Stdby_St[path4] ) 	   chk_req = 0;
			else if(m_St.Mimo_Stdby_St_Dly_REQ[path4]) chk_req = 0;
			else                                       chk_req = 1;
			//-----------------------------	
			if(chk_req) // if normal
			{				
				alarm = m_St.AlmSt.Path[path4].DL.Out_Lower;
				alarm = Chk__PRU_DL_Out_Lower_Alarm(path4, alarm, alm_mask_req, 1);
				m_St.AlmSt.Path[path4].DL.Out_Lower = alarm;
			}
			else
			{
				Chk__PRU_DL_Out_Lower_Alarm(path4, 0, 1, 0); // for timer reset
			}
		}
		// if AC Fail, not update Low-Alarm
	#else // AAU
		//alarm__tssi_low = IS__TSSI_Low_Alarm(path4);
		if(alm_mask_req)
		{
			m_St.AlmSt.Path[path4].DL.Out_Lower = 0;
			Chk__AAU_DL_Out_Lower_Alarm(path4, 0); // for timer reset
		}
		else
		{			
			     if(m_St.AMP_Stdby_St[path4])          chk_req = 0;
			else if(m_St.AMP_Stdby_St_Dly_REQ[path4] ) chk_req = 0;
			else if(m_St.MIMO_Stdby_St[path4] ) 	   chk_req = 0;
			else if(m_St.Mimo_Stdby_St_Dly_REQ[path4]) chk_req = 0;
			else                                       chk_req = 1;
			//-----------------------------
			/*
			// 2020.01.29
			DL TSSI 하한 Alarm 발생 시 DL 출력 하한 Alarm은 Masking. (GUI 출력 하한 Alarm Green. 상위 보고 하지 않음.)
			DL 출력 하한 Alarm 발생 후 DL TSSI 하한 Alarm 발생 시 출력 하한 Alarm은 유지 및 TSSI 하한 Alarm 발생
			*/

			if(chk_req) // if normal
			{			
				alarm = m_St.AlmSt.Path[path4].DL.Out_Lower;				
				Hysterisis_Alm_Chk(&alarm, m_Eep.Fwd_Out_Lmt_x10[path4][LO], m_St.Fwd_Out_Pwr_x10[path4], 20);
				alarm = Chk__AAU_DL_Out_Lower_Alarm(path4, alarm);
				m_St.AlmSt.Path[path4].DL.Out_Lower = alarm;
				
				/*
				// 2020.01.29 : '출력하한 알람' 중, "TSSI 하한알람" 발생한 경우, '출력하한 알람' mask 하지 않도록 수정.										
				//if(m_St.AlmSt.Path[path4].DL.Out_Lower && alarm__tssi_low)
				//{
				//	if(alarm__tssi_low_prev[path4] == alarm__tssi_low) // clear
				//		m_St.AlmSt.Path[path4].DL.Out_Lower = 0;
				//}
				*/
			}
			else
			{
				// 2019.12.23, AAU
				Chk__AAU_DL_Out_Lower_Alarm(path4, 0); // for timer reset, AAU
			}

			Proc__AAU_Reset__By_OutLow_Alarm();
		}
		//alarm__tssi_low_prev[path4] = alarm__tssi_low;
	#endif		
	}
	// 2019.11.27
	if(m_St.AlmSt_prev.Path[path4].DL.Out_Lower != m_St.AlmSt.Path[path4].DL.Out_Lower)
	{
		m_St.AlmSt_prev.Path[path4].DL.Out_Lower = m_St.AlmSt.Path[path4].DL.Out_Lower;
		Update_Alm_Str_Lmt( 0x51, 0x05, path4, 0, m_St.AlmSt.Path[path4].DL.Out_Lower); // DL 출력 하한
	}
}



//void AMP_Rvs_Att_Ctrl_Req(INT8U path4, INT8U att_x2 )
//{
//	m_PAU_C_Req[path4].Rvs_Att_x2 = att_x2;
//}

INT8U Get_User_Rvs_Att(INT8U path4)
{
	return m_Eep.AMP_Rvs_Att_x2[path4];
} 

void Rvs_Att_User_Init(INT8U path4)
{	
	INT8U req_val;
		
	if(m_Eep.AMP_Rvs_Att_x2[path4] > iROO_MAX_RVS_ATT_x2)
	{
		m_Eep.AMP_Rvs_Att_x2[path4] = 0;
	}
	req_val = m_Eep.AMP_Rvs_Att_x2[path4];

	m_PAU_C_Req[path4].Rvs_Att_x2 = req_val; // 2019.05.24
	
	Add_PAU_N_Ctrl_Data( path4, 1, 7, &req_val ); // 5G	
} 










// 2019.04.25
// off 이면, amp_on 은 cancel 되고, payload off 됨.
INT8U IS_PAU_TX_RX_All_Off(INT8U path4)
{
	if(path4 >= MAX_PATH_4) return 0; // ON
	
	if(Check_PAU_Use(path4)==0) return 0; // ON

	if(m_St.Fwd_AMP_Alm[path4]) // if OFF
	{
		if(m_St.AlmSt.Path[path4].PAU.Over_Power) return 1; // both off
		if(m_St.AlmSt.Path[path4].PAU.High_Temp)  return 1; // both off
		if(m_St.AlmSt.Path[path4].PAU.VSWR)       return 1; // both off
		if(m_St.AlmSt.Path[path4].PAU.DC_Fail)    return 1; // both off
	}
	
	if(m_St.RVS_Amp_Onoff[path4] == OFF)
	{
		if(m_St.AlmSt.Path[path4].UL.Over_Input) 
			return 1; // both off
	}
	return 0;
}

/*
// PAU self algorithm으로 인해, PAU 자체적으로 AMP On/Off 제어함.
// 이경우, 우선순위가 PAU이어야 함.
*/
INT8U IS_PAU_Algorithm_Off(INT8U path4)
{
	if(path4 >= MAX_PATH_4) return 0; // ON
	
	if(Check_PAU_Use(path4)==0) return 0; // ON

	// 2020.01.20
	return IS_PAU_TX_RX_All_Off(path4);
}


//============================================================================
// FWD ATT, RVS ATT, RVS_AMP의 현재값과 Req 값을 비교해서 재설정.
void Check_PAU_RF_Val( INT8U path4 )
{
    INT8U req_val;       
	
	// RVS AMP OFF ----- below priority --------- // 2019.04.25	
	req_val = m_PAU_C_Req[path4].Rvs_AMP_En; // PRU Only

	if(IS_PAU_Algorithm_Off(path4)) 
	{
		if(m_St.RVS_Amp_Onoff[path4] != OFF)
			Rvs_Amp_OnOff(path4, OFF); // PRU
	}	
	else if(m_St.RVS_Amp_Onoff[path4] != req_val)
	{		
		if((Rvs_Amp_SD[path4] & AMP_REV_IN) == 0)
			Rvs_Amp_OnOff(path4, req_val); // PRU
	}
	

	// FWD User ATT ---------------------------------
	#if 0
	INT8U amp_st = AMP_Alarm_St(path4);
	// 2019.04.23 : ANTS-PRU : Do Not Want Below
	/////if(amp_st == ON) req_val = m_PAU_C_Req[path4].Fwd_Att_x2;
	/////else             req_val = (30*2); // for minum gain (max att)	
	#else	
	req_val = m_PAU_C_Req[path4].Fwd_Att_x2;
	#endif	
	
	if ( m_St.Fwd_Out_Att_x2[path4] != req_val )
	{
		Add_PAU_N_Ctrl_Data( path4, 0, 7, &req_val ); // USER ATT

		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d FWD ATT Set : Att_x2(%d/2).\n", 
			path4+1, req_val );
	}
	
	// FWD TPL ATT ---------------------------------
	req_val = m_PAU_C_Req[path4].Tpl_Att_x2;
	if ( m_St.Fwd_TPL_Att_x2[path4] != req_val )
	{
		Add_PAU_N_Ctrl_Data( path4, 0, 6, &req_val ); // TPL ATT

		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d TPL ATT Set : Att_x2(%d/2).\n", 
			path4+1, req_val );
	}
		
	// RVS ATT ---------------------------------
	#if 0
	// 2019.04.23 : ANTS-PRU : Do Not Want Below
	/////if(amp_st == ON) req_val = m_PAU_C_Req[path4].Rvs_Att_x2;
	/////else             req_val = (30*2); // for minum gain (max att)
	#else
	req_val = m_PAU_C_Req[path4].Rvs_Att_x2; 
	#endif	
	
	if ( m_St.Rvs_Att_x2[path4] != req_val )
	{
		Add_PAU_N_Ctrl_Data( path4, 1, 7, &req_val ); // RVS ATT		
		
		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d RVS ATT Set : Att_x2(%d/2).\n", 
			path4+1, req_val );
	}
	
	// ATT_Pair_Onoff --------------------------
	if(m_PAU_C_Req[path4].Att_Pair_req)
	{
		req_val = m_PAU_C_Req[path4].Att_Pair;
		
		if(m_St.ATT_Pair_Onoff[path4] != req_val)
		{
			Add_PAU_N_Ctrl_Data( path4, 6, 5, &req_val );

			D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d ATT Pair OnOff %d.\n", 
				path4+1, req_val );
		}
		else
			m_PAU_C_Req[path4].Att_Pair_req = 0;
	}
	
	// FB_Att -------------------------------------------------
	if(m_PAU_C_Req[path4].FB_Att_x2_req)
	{	
		req_val = m_PAU_C_Req[path4].FB_Att_x2;
		if(m_St.FB_ATT_x2[path4] != req_val)
		{
			Add_PAU_N_Ctrl_Data( path4, 0, 4, &req_val );

			D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d FB ATT Set : Att_x2(%d/2).\n", 
				path4+1, req_val );
		}
		else
			m_PAU_C_Req[path4].FB_Att_x2_req = 0;
	}

	// FB Offset Att : 2019.05.08 -------------------------------------------------
	if(m_PAU_C_Req[path4].FB_Offset_x2_req)
	{	
		req_val = m_PAU_C_Req[path4].FB_Offset_x2;
		if(m_St.FB_Offset_ATT_x2[path4] != req_val)
		{
			Add_PAU_N_Ctrl_Data( path4, 0, 3, &req_val );

			D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d FB Offset ATT Set : Att_x2(%d/2).\n", 
				path4+1, req_val );
		}
		else
			m_PAU_C_Req[path4].FB_Offset_x2_req = 0;
	}
}
//============================================================================
///// YCSO added 190520
INT8U chk_amp_FW_confirm(INT8U path4)
{
	INT8U data[2] ={0};
	INT8U retry_cnt = 0;
	INT8U re_val = 0;

	if( check_amp_DN_flag( path4 ) )		//// added YCSO 190920 // 2019.09.20
	{
		if( m_Env.PAU_confirm_DL[path4] == 0x01 )
		{
			for( retry_cnt = 0; retry_cnt < 10; retry_cnt++) // 2019.09.05 : retry added, 2019.09.19 : retry 4 -> 10
			{
				printf("[chk_amp_FW_confirm] check point confirm cnt = %d AMP num = %d !!!!!\n",retry_cnt, path4);
				re_val = PAU_FW_APPLY(path4, data, 2 );
				if( re_val )
				{
					m_Env.PAU_confirm_DL[path4] = _CLR_;
					return 1;
				}
				OSTimeDly(3*OS_DELAY_100msec); // 2019.09.20
			}
		}
		else if( m_Env.PAU_confirm_DL[path4] == 0x02 )
		{
			data[0] = 0x01; 
			for( retry_cnt = 0; retry_cnt < 10; retry_cnt++) // 2019.09.05 : retry added
			{
				re_val = PAU_FW_APPLY(path4, data, 2 );
				if( re_val )
				{
					m_Env.PAU_confirm_DL[path4] = _CLR_;
					return 1;
				}
				OSTimeDly(3*OS_DELAY_100msec); // 2019.09.20
			}
		}

		else
			return 0;


		///// 2019.09.05 성공하든 실패하든 재부팅은 해야 하니까 세팅 해 줌.
		printf("[chk_amp_FW_confirm] confirm fail cnt = %d \n",retry_cnt);
		m_Env.PAU_confirm_DL[path4] = _CLR_;
	////set_amp_DN_confirm_END( path4, _CLR_); // 2019.09.20 : delete
		AMP_Down_Fail(path4); // 2019.09.20
	}
	return 0;


}



//============================================================================
INT8U Chk_Pwr_AMP_Reset(INT8U path4 )
{	
	INT8U down_req;
	INT8U rst__req;

	down_req = m_Env.Pwr_AMP_Download_Req[path4];
	rst__req = m_Env.Pwr_AMP_Rst_Req[path4];
	
	if((down_req == 0)&&(rst__req == 0))
		return 0;
		
	if(down_req) 
	{
		m_Env.Pwr_AMP_Download_Req[path4] = 0;
		
		OSTimeDly(5*OS_DELAY_1sec);
		
		if(rst__req == 0)
		{
			OSTimeDly(2*OS_DELAY_1sec);
			return 1;
		}
	}
	//------------------------------------------------	
	m_Env.Pwr_AMP_Rst_Req[path4] = 0;
	m_Env.Pwr_AMP_Rst_Ack[path4] = 0;	
	m_Env.Pwr_AMP_Rst_Ack[path4] = PAU_RESET_Ctrl(path4);

	ClearFwdSD_Mode( path4 );

	// 초기화 위해.	
	AMP_Init_Set(path4);

	D_Printf(DEBUG_SYSTEM, VT_BOLD_GREEN "[RCU] AMP#%d RST (%d).\r\n", path4+1, m_Env.Pwr_AMP_Rst_Ack[path4]);
	
	// amp reset time
	OSTimeDly( 2*OS_DELAY_1sec );

	return 1;
}

//-----------------------------------------------------------------------









//-----------------------------------------------------------------------
// return loss에서 VSWR 구하기.
//	((10^(Loss/20))+1) / ((10^(Loss/20))-1)
//-----------------------------------------------------------------------
INT16S Get_Vswr(INT16S ret_Loss_x10) 
{
	return 0;
#if 0 // later used when 5G-AAU/PRU	
	double d_val;
	double pwr_10;

	//--------------------------------------------
	// 범위제한.
	//--------------------------------------------
	if(ret_Loss_x10 <= 0) ret_Loss_x10 = 1; // from 0.1
	else if(ret_Loss_x10 > 30000) ret_Loss_x10 = 30000; // from 3000.0
	//--------------------------------------------

	//--------------------------------------------
	d_val = (double)ret_Loss_x10;
	d_val /= 200.0;
	////loss /= 10.0; // 0.1 ~ 3000.0
	////loss /= 20.0;	
	pwr_10 = pow(10.0, d_val);
	//--------------------------------------------
	d_val = (pwr_10 + 1.0)/(pwr_10 - 1.0); // 173.72 ~ 1.00
	//--------------------------------------------
	// 소수 2자리 표기.   // 17372 ~ 100
	return ((INT16S)(d_val*100.0));
#endif	
}

/*
--------------------------------------------
Below is Not used : 2019.05.20
--------------------------------------------
void Conv_RLoss_to_Vswr( INT8U path4 )
{
	INT16S fwd_pwr_x10;	

	switch (path4)	
	{
		case AMPU_4_1: break;
		case AMPU_4_2: break;
		case AMPU_4_3: break;
		case AMPU_4_4: break;

		default: return;
	}

	fwd_pwr_x10 = m_St.Fwd_Out_Pwr_x10[path4];

	if (fwd_pwr_x10 <= 150) // below 15.0 dBm
	{
		m_St.VSWR_x100[path4] = 100; // 1:1
	}
	else
	{
		if(m_St.Ret_Loss_x10[path4] > 1000) // over 100.0 dB
			m_St.VSWR_x100[path4] = 100; // 1:1
		else if(m_St.Ret_Loss_x10[path4] == 0) // total reflection
			m_St.VSWR_x100[path4] = 60000; // 600:1
		else
			m_St.VSWR_x100[path4] = Get_Vswr(m_St.Ret_Loss_x10[path4]);
	}	
}
*/


//----------------------------------------------------------
void PAU_Init_Proc( INT8U path4 )
{
	Init_PAU_N_Ctrl( path4 );
		Fwd_Out_Att_Set( path4, m_Eep.Fwd_Out_Att_x2[path4] );			
		Rvs_Att_User_Init(path4);
	Send_PAU_N_Ctrl( path4 );

	OSTimeDly( OS_DELAY_100msec );

	// 2020.01.29 : AMP_Not_Use -> AMP_Use 변경시.
	if(m_Eep.Pwr_Amp_En[path4] == OFF)
		Set__Reason_Dis_5117(path4, Rsn_Dis_User___Ctrl); 

	print_agent(DEBUG_STDBY, VT_BOLD_CYAN, "[PAU_Init_Proc] %s",Get_path4_Str(path4));
	
	sendPAU_EnDis(path4, m_Eep.Pwr_Amp_En[path4]); // PWR AMP Enable/Disable, init control, reason 불필요.
	
	init[path4] = 1;
	TimerStart_Low_Alm_at_AMP_Init(path4);
}
//--------------------------------------------------------------------
void Init_PAU_Down_St(INT8U path4)
{
	m_Env.AMP_Dn_St[path4] = 0;
}
//--------------------------------------------------------------------
INT16S Get_Hightest_AMP_Temp(void)
{
	INT8U path4;
	///INT16S temp_max = m_St.Temp_x10; // default RCU Temp
	INT16S temp_max = 0; // default 0 [`C]
			
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4)==0) continue;
		
		if(temp_max < m_St.AMP_Temp_x10[path4])
			temp_max = m_St.AMP_Temp_x10[path4];
	}	
	return temp_max;
}
//--------------------------------------------------------------------
void PRU_RVS_In_SD_Proc( INT8U path4 )
{
	(void)path4;
/*
// 2019.06.17	
//	INT8U alm;		
//	alm = m_St.AlmSt.Path[path4].UL.Over_Input;
//	if(alm) Rvs_Amp_SD[path4] |= AMP_REV_IN;
//	else    Rvs_Amp_SD[path4] &= (~AMP_REV_IN);
*/
}
//--------------------------------------------------------------------		

static INT8U AMP_Dnld_End_Chk_Req[AMPU_4_MAX] = {0,0,0,0};
////// YCSO change 190521
INT8U PAU_FW_APPLY(INT8U path4, INT8U* data, INT8U len )
{
	INT8U dnld_St;
		
	PAU_Send_Data(path4, AMP_CMD_ST, NULL, 0);

	dnld_St = m_St.AMP_DL_St[path4][DL_ST];

	D_Printf( DEBUG_SYSTEM, "\r\n[RCU] [PAU_FW_APPLY] amp = %s (%d)", Get_path4_Str(path4), dnld_St);

	if(( dnld_St == DL_St_Complete)||( dnld_St == DL_St_Fail))	
	{		
		D_Printf( DEBUG_SYSTEM, "\r\n[RCU] PAU %s FW_APPLY (%d)", Get_path4_Str(path4), data[0]);

		if(PAU_Send_Data(path4, AMP_CMD_FW_APPLY, data, len))
		{		
			//*pACK = m_St.AMP_FW_NACK[path4];
			
			if(data[0] == 0) AMP_Dnld_End_Chk_Req[path4] = 1; // Apply
			else			 AMP_Dnld_End_Chk_Req[path4] = 0; // Cancel
			
			Clr_Inv_Read_Req( path4 );
			return 1;
		}		
	}	
	// default
	AMP_Dnld_End_Chk_Req[path4] = 0;
	return 0;
}
//-------------------------------------------------

void Chk_AMP_Download_End(INT8U path4 )
{
	if(AMP_Dnld_End_Chk_Req[path4])
	{	
		if(m_St.AMP_DL_St[path4][DL_ST] == DL_St_IDLE)
		{
			D_Printf( DEBUG_SYSTEM, "[AMP_DN] %s END ", Get_path4_Str(path4));
	
			AMP_Dnld_End_Chk_Req[path4] = 0;
			AMP_Init_Set(path4);
			m_Env.Pwr_AMP_Download_Req[path4] = 1;
		}
	}
}
//-------------------------------------------------

// vswr, over pwr, hi temp, dev fail	
#define CHK_NUM (4)
static INT8U prev_amp_alm[AMPU_4_MAX][CHK_NUM];
static INT8U curr_amp_alm[AMPU_4_MAX][CHK_NUM];

void Init_prev_amp_alm(INT8U path4)
{
	memset(prev_amp_alm[path4], 0, CHK_NUM);
}

// vswr, over pwr, hi temp, dev fail	
void Get_AMP_Prev_Alm( INT8U path4 )
{
	INT8U alarm[CHK_NUM];
	INT8U i;
		
	if(path4 >= AMPU_4_MAX) return;	
	
	alarm[0] = m_St.AlmSt.Path[path4].PAU.VSWR;
	alarm[1] = m_St.AlmSt.Path[path4].PAU.Over_Power;
	alarm[2] = m_St.AlmSt.Path[path4].PAU.High_Temp;
	alarm[3] = m_St.AlmSt.Path[path4].PAU.Dev_Fail;		

	if(Check_PAU_Use(path4))
	{
		for(i = 0; i < CHK_NUM; i++)
			 prev_amp_alm[path4][i] = alarm[i];
	}
	else
	{
		for(i = 0; i < CHK_NUM; i++)
		{
			prev_amp_alm[path4][i] = 0;
			curr_amp_alm[path4][i] = 0;
		}
	}
}

void Chk_AMP_Alm_Update( INT8U path4 )
{
	INT8U alarm[CHK_NUM];	
	INT8U i;
	//INT8U byte, bit;
	
	if(path4 >= AMPU_4_MAX) return;	
	if(Check_PAU_Use(path4)==0) return;

	alarm[0] = m_St.AlmSt.Path[path4].PAU.VSWR;
	alarm[1] = m_St.AlmSt.Path[path4].PAU.Over_Power;
	alarm[2] = m_St.AlmSt.Path[path4].PAU.High_Temp;
	alarm[3] = m_St.AlmSt.Path[path4].PAU.Dev_Fail;		
	
	for(i = 0; i < CHK_NUM; i++)
	{
		curr_amp_alm[path4][i] = alarm[i];

		if(prev_amp_alm[path4][i] != curr_amp_alm[path4][i]) // if changed
		{	
			#if 0 // later 5G
			////m_St.Sum_Alarm = Chk_Sum_Arm();
			////MakeNMS_Alarm(byte[i], bit[i]);			
			#endif
			prev_amp_alm[path4][i] = curr_amp_alm[path4][i];
        }
	}
}


void AMP_Std_AMP_Proc( INT8U path4 )
{	
	INT8U amp_onoff_req;

	if(Is_AC_Fail()) return;

	if((m_St.AMP_Stdby_Sync & cbits[path4])==0) return;
	else m_St.AMP_Stdby_Sync &= nbits[path4];

	// amp-off request during AMP-Std-time
	if(m_St.AMP_Stdby_St[path4]) 
	{	
		if(AMP_Stby_Off_Req[path4]) // if AMP_OFF is controled before..
		{
			if(PAU_FWD_AMP_St(path4) != AMP_Stby_Ctrl_Val[path4])
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Std_AMP_Proc] Enter, PAU#%d Req val(%d)",
					path4+1, AMP_Stby_Ctrl_Val[path4] );				
	
				StandBy_PAU_EN_Ctrl(path4, AMP_Stby_Ctrl_Val[path4], 'P');
			}
		}		
	}
	else // Amp-Stdby exit, 
	{	
		if (AMP_Stby_Off_Req[path4]) // if AMP_OFF is controled before..
		{			
			// Try to AMP-ON, 
			// but off-priority, Off-AMP
			
			amp_onoff_req = AMP_Stby_Bk_AMP_St[path4]; // default
			if(AMP_Alarm_St(path4) == OFF)
			{				
				if(m_Eep.Pwr_Amp_En[path4] == OFF) amp_onoff_req = OFF;
				//////if(m_St.Off_Reason____Prev[path4]) amp_onoff_req = OFF; // 2019.09.16 delete
			}

			if(amp_onoff_req == ON)
			{
				if(m_Env.AMP_Ctrl_Allowed==0) // 2020.01.14
				{
					amp_onoff_req = OFF;
					print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[AMP_Std_AMP_Proc] Exit, PAU#%d On Req, But Under RU.Init, AMP-OFF Set",
							path4+1);
				}
				else
				{
					print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Std_AMP_Proc] Exit, PAU#%d On Req",
							path4+1);
				}
			}
			else
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Std_AMP_Proc] Exit, PAU#%d Off Req(%d, %d, %d)",				
					path4+1, AMP_Stby_Bk_AMP_St[path4], m_Eep.Pwr_Amp_En[path4], m_St.Off_Reason____Prev[path4]);
			}
							
			StandBy_PAU_EN_Ctrl(path4, amp_onoff_req, 'P');
			
			if(PAU_FWD_AMP_St(path4) == amp_onoff_req)
			{
				AMP_Stby_Off_Req[path4] = 0;
				StdBy_Off_rsn[path4] = _CLR_;
			}
		}	
	}
}
/*
--------------------------------------------------------------------------------------
* MIMO Stby Mode 관련 처리 
*																					   
--------------------------------------------------------------------------------------
*/





void MIMO_Std_AMP_Proc(INT8U path4)
{
	INT8U amp_onoff_req;
	
	if(Is_SISO_AMP(path4)) return;
	if(Is_AC_Fail()) return;
	
	if((m_St.MIMO_Stdby_Sync & cbits[path4])==0) return;
		else m_St.MIMO_Stdby_Sync &= nbits[path4];

	if(m_St.MIMO_Stdby_St[path4])
	{		
		if(Mimo_Stby_Ctrl_Req[path4])
		{
			if(PAU_FWD_AMP_St(path4) != Mimo_Stby_Ctrl_Val[path4])
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[MM-Stdby_PAU] AMP#%d Ctrl Req (%d)", 
					path4+1, 
					Mimo_Stby_Ctrl_Val[path4]);
					
				StandBy_PAU_EN_Ctrl( path4, Mimo_Stby_Ctrl_Val[path4], 'M');
			}
		}		
	}	
	else // Restore if time is Over.
	{
		// if AMP-Std Ctrl, cancel Mimo Stdby backup
		if (AMP_Stby_Off_Req[path4])
		{
			Mimo_Stby_Ctrl_Req[path4] = 0;
		}
	
		if (Mimo_Stby_Ctrl_Req[path4]) // amp 제어 했으면..
		{
			amp_onoff_req = Mimo_Stby_Bk_AMP_St[path4]; // default
			
			if(AMP_Alarm_St(path4) == OFF)
			{
				if(m_Eep.Pwr_Amp_En[path4] == OFF)  amp_onoff_req = OFF;
				////if(m_St.Off_Reason____Prev[path4]) amp_onoff_req = OFF; // 2019.09.16 delete
			}
			/*
			D_Printf( DEBUG_STDBY, "[MM-Stdby] AMP#%d Ctrl Req bk (%d), Prev(%d)m AMP_Alarm_St(%d), user %d, req %d  \n", 
					path4+1, 
					Mimo_Stby_Bk_AMP_St[path4], 
					m_St.Off_Reason____Prev[path4],
					AMP_Alarm_St(path4),
					m_Eep.Pwr_Amp_En[path4],
					amp_onoff_req
					);
			*/	

			if((amp_onoff_req == ON) && (m_Env.AMP_Ctrl_Allowed==0)) // 2020.01.14
			{
				amp_onoff_req = OFF;
				print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[MM-Stdby_PAU] AMP#%d Bk_AMP (%d), But Under RU.Init, AMP-OFF Set",
							path4+1, amp_onoff_req);
			}
			else
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[MM-Stdby_PAU] AMP#%d Bk_AMP (%d)", 
							path4+1, amp_onoff_req);
			}

			StandBy_PAU_EN_Ctrl( path4, amp_onoff_req, 'M');

			if( PAU_FWD_AMP_St(path4) == amp_onoff_req )
			{
				Mimo_Stby_Ctrl_Req[path4] = 0;
				StdBy_Off_rsn[path4] = _CLR_; // 2020.01.13.
			}
		}	
	}
}

/*
21:07:01: [AMP-Stdby] Off -> On.
21:07:01: <RCU> [AMP-Stdby] AMP#1 bkup 0

21:07:01: <RCU> [StandBy_PAU_EN_Ctrl] PAU#1 (onoff 1), A
21:07:01: <RCU> [AMP_Stdby] AMP#1 Off
21:07:01: <RCU> [AMP-Stdby] AMP#4 bkup 0

21:07:01: <RCU> [StandBy_PAU_EN_Ctrl] PAU#4 (onoff 1), A
21:07:01: <RCU> [AMP_Stdby] AMP#4 Off

----------------------
21:08:01: <RCU> [AMP-Stdby] On -> Off 
21:08:01: <RCU> [AMP_Stdby] Exit (by Time Off)
21:08:01: <RCU> [AMP_Std_AMP_Proc] Exit, PAU#1 On Req
21:08:01: <RCU> [StandBy_PAU_EN_Ctrl] PAU#1 (onoff 0), P
21:08:01: <RCU> [AMP_Std_AMP_Proc] Exit, PAU#4 On Req
21:08:01: <RCU> [StandBy_PAU_EN_Ctrl] PAU#4 (onoff 0), P
21:08:01: <RCU>  DL_Payload_Off[0][0] : 0x0
21:08:01: <RCU>  DL_Payload_Off[0][3] : 0x0
21:08:01: <RCU>  DL_Payload_Off[1][0] : 0x40
21:08:01: <RCU>  DL_Payload_Off[1][3] : 0x40

21:08:02: <RCU> [AMP_Std_AMP_Proc] Exit, PAU#1 On Req
21:08:02: <RCU> [StandBy_PAU_EN_Ctrl] PAU#1 (onoff 0), P
21:08:02: <RCU> [AMP_Std_AMP_Proc] Exit, PAU#4 On Req
21:08:02: <RCU> [StandBy_PAU_EN_Ctrl] PAU#4 (onoff 0), P
21:08:11: <RCU> [AMP_Stdby] Dly_REQ All End --


*/


/*
--------------------------------------------------------------------------------------
* AC Fail
*																					   
--------------------------------------------------------------------------------------
*/
void AC_Fail_AMP_Proc( INT8U path4 )
{
	INT8U req_att_x2 = AC_Fail_Bk_ATT_x2[path4];

	/*
	////if((AC_Fail_St != _AC_NORMAL_)&&(m_Eep.Batt_Ext_Mode_Onoff == ON)) // algorithm 동작 중.
	////{
	////	if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ) // amp off
	////	{
	////		if(Is_MIMO_AMP(path4) && (PAU_FWD_AMP_St(path4) == ON)) 
	////		{
	////			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AC_Fail_AMP_Proc] AMP#%d AMP OFF Req.", path4+1 );		
	////			AC_Fail_PAU_EN_Ctrl(path4, OFF);
	////		}
	////	}
	////}	
	////else // 정상으로 되면, 복귀 동작 수행 : ac.normal or mode.off
	*/
	
	if((m_St.Batt_Ext_Mode_St == OFF) && (AC_Fail_Ctrl_Req[path4])) // algorithm end
	{
		if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ)
		{
			if(m_St.Pwr_Amp_En[path4] == OFF) // 2020.04.13 : "PAU_FWD_AMP_St()" --> m_St.Pwr_Amp_En[]						
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AC Batt-Ext AMP] AMP#%d Backup ON", path4+1);	
				AC_Fail_PAU_EN_Ctrl(path4, ON);
			}
			else 
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AC Batt-Ext AMP] AMP#%d Already ON", path4+1);	
				AC_Fail_Ctrl_Req[path4] &= (~_AC_F_AMP_REQ); // amp req clear
				
				if(AC_Fail_Ctrl_Req[path4] == 0)
					StdBy_Off_rsn[path4] = _CLR_;
			}
		}
		else if(AC_Fail_Ctrl_Req[path4] & _AC_F_ATT_REQ)
		{
			if(m_St.Fwd_Out_Att_x2[path4] != req_att_x2)
			{		
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AC Batt-Ext AMP] AMP#%d Bk_ATT (%d/2)", path4+1, req_att_x2 );					
				Fwd_Out_Att_Set(path4, req_att_x2);
			}
			else 
			{
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AC Batt-Ext AMP] AMP#%d Bk_ATT End (%d/2)", path4+1, req_att_x2 );
				AC_Fail_Ctrl_Req[path4] &= (~_AC_F_ATT_REQ); // att req clear

				if(AC_Fail_Ctrl_Req[path4] == 0)
					StdBy_Off_rsn[path4] = _CLR_;
			}
		}
	}
}

/*
------------------------------------------------------------
When AMP Is not used, set AMP-Temp to 0['C]
AMP not used : AMP Temp 0 set
------------------------------------------------------------
*/
void AMP_Temp_0_Set(INT8U path4)
{	
	m_St.AMP_Temp_x10[path4] = 0;
	
	//--------------------------------------------------------------------------------------------- 			
	#if 0 // later used when 5G-AAU/PRU
	//------------------------------------------------------------------	
	// REMS Temp
	//	m_Stat.AMP_Temp_x10[path4].s_max = 0;
	//	m_Stat.AMP_Temp_x10[path4].s_min = 0;
	//	m_Stat.AMP_Temp_x10[path4].s_sum = 0;
	//	m_Stat.AMP_Temp_x10[path4].s_avr = 0;	
	//--------------------------------------------------------------------------------------------- 			
	#endif // later used when 5G-AAU/PRU
	//------------------------------------------------------------------
}
//------------------------------------------------------------------
INT8U Is_All_PAU_CommFail(void)
{
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4)==0) continue;
		
		if(m_St.Pwr_Amp_Link_Err[path4]) 
			return 40;		
	}
	return 0;
}
//------------------------------------------------------------------
INT8U Is_All_PAU_CommFail_Co(void)
{
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4)==0) continue;
		
		if( PAU_Link_Fail_Co[path4] > 0) 
			return 40;		
	}
	return 0;
}
//------------------------------------------------------------------
/*
	if RVS-AMP-OFF, 
	then Gain-balance-off-display
*/
void AMP_Not_Use_RVS_OFF_Display( INT8U path4 )
{	
	m_St.RVS_Amp_Onoff[path4] = OFF; // off
	m_St.Rvs_Gain_Bal_On[path4] |= OFF;
}

/*
--------------------------------------------------------------------------------------
* TASK of AMP
*																					   
--------------------------------------------------------------------------------------
*/

void* PAU_Comm_Task(void *data)
{
#ifdef __TYPE__PRU__ // PRU Only
	INT8U path4;
	INT32S res;
	Thread_ARG_Var* p_th_arg;
	INT8U inv_idx; // inventory index
	INT8U port;
	INT8U continue_x;
	INT8U inv_cnt = 0;
	INT8U cnt;
	INT8U val8;

	
	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	path4 = (INT8U)p_th_arg->Arg_v;

	while( m_Env.UDP_Init[COMM_UDP_NODE] == 0 )
		OSTimeDly( OS_DELAY_1sec ); // UDP 초기화 대기

	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "AMP-#%d Comm Task RUN.", path4+1);

	res = pthread_mutex_init(&PAU_COMM_Evt[path4], NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] AMP-#%d Mutex Init Fail.", path4+1);
		exit(EXIT_FAILURE);
	}	
	res = pthread_mutex_init(&PAU_Ctrl_Sem[path4], NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] AMP-#%d Mutex2 Init Fail.", path4+1);
		exit(EXIT_FAILURE);
	}	

	port = COMM_UART_AMP_1 + path4;
	Init_Comm(port);
	if(Get_Comm_fd_Status(port) < 0)
	{
		syslog( LOG_ERR, "[RCU] AMP-#%d Serial Open Fail.", path4+1);
	}	
	
	Init_PAU_Down_St(path4);	

	m_PAU_C_Req[path4].FB_Att_x2_req = 0;
	m_PAU_C_Req[path4].FB_Offset_x2_req = 0;
			
	Init_Low_Alm_Mask_at_AMP_Init(path4);
	Init_prev_amp_alm( path4 );
	
	
	Init_AMP_MutiDn(path4);
	inv_idx = Get_Inv_idx(path4);	
	InvetoryReadClear_mod( inv_idx ); // read 1st @ initial

	//---------------------------------------------------
	// 2020.01.10 : amp off reason set by user if user-amp-off controlled.
	if(m_Eep.Pwr_Amp_En[path4] == OFF)
		Set__Reason_Dis_5117(path4, Rsn_Dis_User___Ctrl); 


	/*
	---------------------------------------------------
	2020.02.03 : 
	재부팅 시에 미사용 Layer의 Disabled 상태값 (0x5117)이  'ON' 상태로 되지 않도록 수정.	
	---------------------------------------------------
	*/
	if(m_Eep.LayerUseInfo[path4] != AMP_USE)
		m_Env.Use_Not_Req2[path4] = 1;	
	
	//---------------------------------------------------
	while( m_Env.task_flag[_TASK_PAU_COMM_AMP_1_+path4] )		///// YCSO TASK END
	{		
		cnt = 0;
		do
		{
			///// ycso 전체 속도 향상을 위해 500ms -> 300ms
			OSTimeDly(OS_DELAY_10msec);
			cnt++;
		}while(m_Env.task_flag[_TASK_PAU_COMM_AMP_1_+path4] && (cnt <= 30));
			
		Update__Task_Count(_TASK_PAU_COMM_AMP_1_ + path4);
		
		if(Get_Comm_fd_Status(port) < 0)
		{
			OSTimeDly( OS_DELAY_1sec*3 );
			D_Printf( DEBUG_SYSTEM, "[RCU] Get_Comm_fd_Status AMP-#%d\n", path4+1);
			continue;
		}		
		//-----------------------------------------
		// Insert Check		
		Proc_Chk_AMP_Install(path4);
		//-----------------------------------------
		continue_x = 0; // init		
		
		if(Proc_PAU_PkgDn(path4))  continue_x |= cbits[0];		
		if(m_Env.is_down_load) 	   continue_x |= cbits[3];
		if(Is_Dig_MultiDown())     continue_x |= cbits[5];
		if(Chk_Pwr_AMP_Reset(path4)) continue_x |= cbits[6];
		if(chk_amp_FW_confirm(path4)) continue_x |= cbits[7];

		
		if(continue_x)
		{
			//D_Printf( DEBUG_SYSTEM, "[RCU] AMP-#%d : Disabled by 0x%02X.\n", path4+1, continue_x);			
			continue;
		}
		//-----------------------------------------

		// if AMP Not Use -------------------			
		if(Check_PAU_Use(path4)==0)
		{
			PAU_Link_Fail_Set(path4, 0 ); // normal
			PAU_Inv_Read_Req(path4); // read req when amp is used later.
			Pau_Init_End[path4] = (-1); 
			
			Init_PAU_Down_St(path4);
			AMP_Temp_0_Set(path4);			
			AMP_Not_Use_RVS_OFF_Display(path4);
			AMP_Not_Use_Alarm_Mask(path4);

			// 2020.02.03
			Clr__Reason_SD_5115(path4);
			Clr__Reason_SD_5115_Prev(path4);
			//---------------------------------------
			// 2020.01.28
			//---------------------------------------
			// FWD AMP OFF
			if(m_Env.Use_Not_Req2[path4])
			{
				m_Env.Use_Not_Req2[path4] = 0;				
				
				PAU_Send_Data(path4, AMP_CMD_DIS, NULL, 0);
				// RVS AMP OFF
				Init_PAU_N_Ctrl( path4 );
				val8 = OFF; 
				Add_PAU_N_Ctrl_Data( path4, 2, 6, &val8 ); // @5G : Byte 2, bit 6
				Send_PAU_N_Ctrl_Not_Used_AMP( path4 );				
			}			

			/*
			2020.04.02 : AMP_Not_Use but AMP is installed..
			*/
			if(IS__AMP_Use_Allowed(path4))
			{
				PAU_Polling(path4);
				sendPAU_GetInfo(path4);
			}
			else
			{
				OSTimeDly(3*OS_DELAY_1sec);
			}
			//---------------------------------------			
			
			continue;
		}
		if(PAU_Polling(path4)==0)			
			continue;
		//-----------------------------------------
		// init
		//-----------------------------------------
		
		if(m_Env.AMP_Ctrl_Allowed==0) // 2019.09.24
		{
			sendPAU_EnDis(path4, _DIS_); // RU_Init 제어.
			Pau_Init_End[path4] = -1; // for initialization
		}
		else if(IS_RU_Init_Completed() || m_Env.AMP_User_Ctrl[path4])
		{	
			if(Pau_Init_End[path4] <= 0)
			{
				OSTimeDly(OS_DELAY_500msec); // reset 후 일정 시간 후 제어를 위해.
				
	 			PAU_Init_Proc(path4);
				Pau_Init_End[path4] = 1;
				Init_PAU_Down_St(path4);
				
				InvetoryReadClear_mod( inv_idx );				
				continue; 
			}
		}
		//-----------------------------------------
		// inventory
		//-----------------------------------------
		inv_cnt++;
		if(inv_cnt > 10)
		{
			inv_cnt = 0;
			PAU_Inv_Read_Req(path4);
		}
		
		if(CheckInvetoryRead(inv_idx)==0) sendPAU_GetInfo(path4);
		if(Inv_Read_Needed(path4)) sendPAU_GetInfo(path4);
		
		Chk_AMP_Download_End(path4);
		
		if(Chk_Pwr_AMP_Reset(path4)) continue;
	
	
	//------------------		
		Init_PAU_N_Ctrl( path4 );
			
			if(m_Env.AMP_Ctrl_Allowed==0) // 2020.01.14
			{
				// LNA On/Off(UL)
				val8 = 1; // 0: Enable,  1 : Disable
				Add_PAU_N_Ctrl_Data(path4, 2, 6, &val8); // byte = 2; bit = 6;
			}
			
			PAU_Auto_SD_Chk( path4 );
			Check_PAU_RF_Val( path4 );
		Send_PAU_N_Ctrl( path4 );
		
		PRU_RVS_In_SD_Proc( path4 );
	//------------------		

		// PAU-SUB-SW : Off --------------------------------------------
		// update reason and amp-off
		if( Get_PSU_AMP_SW_St(path4) == OFF ) 
		{	
			m_St.Off_Reason____Prev[path4] = rsn_SUB_Pwr_Off; 
			m_St.Pwr_Amp_Off_Reason[path4] = rsn_SUB_Pwr_Off;	

			// by DC-Fail @ AMP, AMP-Self will be Off.
		}		
		else  // PSU-SUB-SW : ON -------------------------------------------- 
		{				
			// if Dig, PD are normal, then use amp off reason of AMP Status.	            
			// AMP OFF Req의 경우, 현재 PAU AMP ON인 경우에만 OFF하고, OFF Reason 적는다.
			if(Fwd_Amp_Off_rsn[path4]) // check RCU/DTU off reason at first
			{
				if (PAU_FWD_AMP_St(path4) == ON)
					PAU_EN_Ctrl(path4, OFF); // check_need, reason 있으므르 별도 reason 불필요.
								
				m_St.Pwr_Amp_Off_Reason[path4] = Fwd_Amp_Off_rsn[path4]; 
				m_St.Off_Reason____Prev[path4] = Fwd_Amp_Off_rsn[path4]; 
			}			
			else 
			{	
				// Reason is now cleared..
				// and change reason to 'unknown' for below case
				switch(m_St.Pwr_Amp_Off_Reason[path4])
				{
					case rsn_PD:
					case rsn_Digital: // 2019.12.19
					case rsn_PLL:
					case rsn_SUB_Pwr_Off:
						m_St.Off_Reason____Prev[path4] = m_St.Pwr_Amp_Off_Reason[path4];
						m_St.Pwr_Amp_Off_Reason[path4] = rsn_Unknown; // reason 없음...
						break;
				}
				// External is Normal but AMP-OFF status now
				// Get the off-reason of Amp-status-reason.
				//
				if(PAU_FWD_AMP_St(path4) == OFF)
				{
					m_St.Pwr_Amp_Off_Reason[path4] = m_St.Pwr_Amp_Off_Reason_PAU[path4]; // PAU
					
					if(m_St.Pwr_Amp_Off_Reason_PAU[path4] > 0) // PAU
					{						
						m_St.Off_Reason____Prev[path4] = m_St.Pwr_Amp_Off_Reason_PAU[path4]; // PAU
					}
				}
			}
			//------------------------------------------------------
			if(Chk_Pwr_AMP_Reset(path4)) continue;			
			//------------------------------------------------------
			Init_PAU_N_Ctrl(path4);
				AC_Fail_AMP_Proc(path4);
			Send_PAU_N_Ctrl(path4);			
			//------------------------------------------------------		
			MIMO_Std_AMP_Proc(path4);
			AMP_Std_AMP_Proc(path4);
			//------------------------------------------------------			
			if(PAU_FWD_AMP_St(path4) != m_Eep.Pwr_Amp_En[path4]) // FWD : Check User-Ctrl, PRU
			{
				if(m_Eep.Pwr_Amp_En[path4] == ON) // if User ON Control
				{
					switch(m_St.Pwr_Amp_Off_Reason[path4])
					{
						// if No Off-reason, then AMP-ON
						// but when AC-Fail or Standby Mode 
						// Control is Cancelled.
						case 0x00:
						case rsn_Unknown:
							if(IS_RU_Init_Completed() || m_Env.AMP_User_Ctrl[path4])
							{	
								PAU_EN_Ctrl(path4, ON); 
							}
							break;
							
						default:
							// amp off now
							break;
					}
				}
				else // User-off-req
				{
					PAU_EN_Ctrl(path4, OFF); // by user
				}
			}
			//--------------------------------------------	
		} // end of PSU-SUB-SW --------------------------------------------
	}
	m_Env.task_END_status[_TASK_PAU_COMM_AMP_1_+path4] = _CLR_;		///// YCSO TASK END

#endif
	pthread_exit(NULL);
}
//--------------------------------------------------------------------





#ifdef __TYPE__AAU__
void Check_AAU_RF_Val(INT8U path4)
{
// ANTS AAU Only
    INT8U req_val;   
	INT8U cur_att_x2;
	INT8U req_att_x2;
	INT8U bk__att_x2; // backup atten
	INT8U amp_st;   

	amp_st = AMP_Alarm_St(path4);
	
	//-------------------------
	// FWD/RVS AMP ON/OFF 	
	//-------------------------
	req_val = m_PAU_C_Req[path4].Rvs_AMP_En; // AAU only
	
	if(m_St.RVS_Amp_Onoff[path4] != req_val)
	{
		AAU__RF_AMP_OnOff(path4, req_val, 2531);

		//D_Printf( DEBUG_STDBY, "[RCU] Check_AAU_RF_Val : AMP#%d onoff (%d).\n", 
		//	path4+1, req_val );

		//printf( VT_BOLD_RED "[RCU] Check_AAU_RF_Val : AMP#%d onoff (%d)" VT_END_CR, 
		//	path4+1, req_val );	
	}
	
	//-------------------------
	// FWD User ATT 
	//-------------------------		
	// for Atten when AMP_Off -> AMP ON
	bk__att_x2 = m_PAU_C_Req[path4].Fwd_Att_x2; 

	if(amp_st == ON) req_att_x2 = m_PAU_C_Req[path4].Fwd_Att_x2;
	else req_att_x2 = (30*2); // for minum gain (max att)

	// 2019.05.04 : not use 30dB when amp-off
	req_att_x2 = m_PAU_C_Req[path4].Fwd_Att_x2;

	cur_att_x2 = m_St.DL_Att[path4]/2;
	if(cur_att_x2 != req_att_x2)
	{
		AAU_RF_DL_Att_Set(path4, req_att_x2);
		m_PAU_C_Req[path4].Fwd_Att_x2 = bk__att_x2; // Restore when AMP_Off -> AMP ON

		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d FWD ATT Set : Att_x2(%d/2).\n", 
			path4+1, req_att_x2 );
	}
	
	//-------------------------
	// RVS ATT 
	//-------------------------	
	// for Atten when AMP_Off -> AMP ON
	bk__att_x2 = m_PAU_C_Req[path4].Rvs_Att_x2; 

	if(amp_st == ON) req_att_x2 = m_PAU_C_Req[path4].Rvs_Att_x2;
	else req_att_x2 = (25*2); // for minium gain (max att)

	// 2019.05.04 : not use 30dB when amp-off
	// 2019.09.19 : AMP Off 시, UL-ALC ATT(RF ATT) max 25dB Setting.
		
	cur_att_x2 = m_St.UL_Att[path4]/2;
	if(cur_att_x2 != req_att_x2)
	{
		AAU_RF_UL_Att_Set(path4, req_att_x2);
		m_PAU_C_Req[path4].Rvs_Att_x2 = bk__att_x2; // Restore when AMP_Off -> AMP ON 
		
		D_Printf( DEBUG_SYSTEM, "[RCU] AMP#%d RVS ATT Set : Att_x2(%d/2), m_PAU_C_Req(%d/2)", 
				path4+1, req_att_x2, m_PAU_C_Req[path4].Rvs_Att_x2 );
	}
}
#endif

/*
--------------------------------------------------------------------

Called after Read Register of "AAU_RF_Polling"

--------------------------------------------------------------------
*/


void Proc___AAU_RFU_Init(INT8U path4)
{
#ifdef __TYPE__AAU__ // AAU Only	
	static INT8U initialize[AMPU_4_MAX] = {0,0,0,0};

	// if AMP Not Use -------------------			
	if(Check_PAU_Use(path4)==0)
	{
		Pau_Init_End[path4] = (-1);
		initialize[path4] = 0;
		AMP_Not_Use_RVS_OFF_Display(path4); // check-need
		AMP_Not_Use_Alarm_Mask(path4);

		// 2019.09.09 : AMP Off when AMP-Not-Used
		if(m_Reg_Sts.RFU[path4].PA_OnOff.On == 1) // if 'ON(0x01)'
		{
			AAU__RF_AMP_OnOff(path4, OFF, 2675);
			AAU__RF_LNA_OnOff(path4, OFF, 2675);
		}

		// 2020.02.04
		Clr__Reason_SD_5115(path4);
		Clr__Reason_SD_5115_Prev(path4);
		
		return; // continue;
	}
	
	if(initialize[path4] == 0)
	{
		initialize[path4] = 1;
		
		Init_Low_Alm_Mask_at_AMP_Init(path4);
		Init_prev_amp_alm(path4);
	}
	
	//-----------------------------------------
	// init
	//-----------------------------------------	
	if(IS_RU_Init_Completed() || m_Env.AMP_User_Ctrl[path4]) // 2019.09.24	
	{
		if(Pau_Init_End[path4] <= 0 )
		{
			Pau_Init_End[path4] = 1;

			AAU_RF_DL_Att_Set(path4, m_Eep.DL_Att_User[path4]/2);
			AAU_RF_UL_Att_Set(path4, m_Eep.UL_Att[path4]/2);
			AAU__RF_AMP_OnOff(path4, m_Eep.Pwr_Amp_En[path4], 2627);
			AAU__RF_LNA_OnOff(path4, m_Eep.Pwr_Amp_En[path4], 2627);
			
			init[path4] = 1;
			TimerStart_Low_Alm_at_AMP_Init(path4);
			
			// 2020.02.04 : AMP_Not_Use -> AMP_Use 변경시.
			if(m_Eep.Pwr_Amp_En[path4] == OFF)
				Set__Reason_Dis_5117(path4, Rsn_Dis_User___Ctrl);
		}
	}
#endif
}

void Proc___AAU_RFU(INT8U path4)
{	
#ifdef __TYPE__AAU__ // AAU Only
	INT8U static cnt[AMPU_4_MAX] = {0,0,0,0};	
	
	if(Check_PAU_Use(path4)==0)
		return;

	cnt[path4]++;
	if(cnt[path4] < 5) return;
	cnt[path4] = 0;
	
	//-----------------------------------------		
	Check_AAU_RF_Val(path4);
	
	//-----------------------------------------
	// OFF Reason = RCU/DTU reason or RFU_Reason
	//-------------------------
	// RCU/DTU reason  : Dig, PD, ...
	// RFU_Reason : over power, ...
	//-------------------------
	// Priority : RCU/DTU reason > RFU_Reason
	//-------------------------
	// AMP OFF Req의 경우, 현재 PAU AMP ON인 경우에만 OFF하고, OFF Reason 적는다.
	//-------------------------
	if(Fwd_Amp_Off_rsn[path4]) // RCU/DTU reason  : Dig, PD, ...
	{
		if (PAU_FWD_AMP_St(path4) == ON)
		{
			PAU_EN_Ctrl(path4, OFF); // check_need, reason 있으므르 별도 reason 불필요.
			
			if(m_Env.debug_path4 == path4)
				print_agent(DEBUG_STDBY, VT_BOLD_CYAN, "PAU_EN_Ctrl : AMP#%d Off", path4+1);
		}
						
		m_St.Pwr_Amp_Off_Reason[path4] = Fwd_Amp_Off_rsn[path4]; 
		m_St.Off_Reason____Prev[path4] = Fwd_Amp_Off_rsn[path4]; 
	}
	else // RFU_Reason : over power, ...
	{	
		// Reason is now cleared..
		// Change reason to 'unknown' for below case
		switch(m_St.Pwr_Amp_Off_Reason[path4])
		{
			case rsn_PD:
			case rsn_Digital:						
			case rsn_PLL:
			case rsn_SUB_Pwr_Off:
				m_St.Off_Reason____Prev[path4] = m_St.Pwr_Amp_Off_Reason[path4];
				m_St.Pwr_Amp_Off_Reason[path4] = rsn_Unknown; // reason 없음...
				break;
		}
		// External is Normal but AMP is OFF now...
		// Get the off-reason of Amp-status-reason.
		//
		if(m_St.Pwr_Amp_Off_Reason_PAU[path4] > 0) // AAU
		{
			if(PAU_FWD_AMP_St(path4) == ON)
			{
				if(m_Env.debug_path4 == path4)
				{
					print_agent(DEBUG_STDBY, VT_BOLD_CYAN, "[AAU] <AMP_%d> Off, Reason By 0x%X\n", 
						path4, 
						m_St.Pwr_Amp_Off_Reason_PAU[path4]);	 // AAU			
				}
			}
			
			PAU_EN_Ctrl(path4, OFF); // check_need, reason 있으므르 별도 reason 불필요.
			
			m_St.Pwr_Amp_Off_Reason[path4] = m_St.Pwr_Amp_Off_Reason_PAU[path4]; // AAU
			m_St.Off_Reason____Prev[path4] = m_St.Pwr_Amp_Off_Reason_PAU[path4]; // AAU		
		}
	}		
	//------------------------------------------------------		
	MIMO_Std_AMP_Proc(path4);
	AMP_Std_AMP_Proc(path4);
	//------------------------------------------------------			
	if(PAU_FWD_AMP_St(path4) != m_Eep.Pwr_Amp_En[path4]) // FWD : Check User-Ctrl
	{
		if(m_Eep.Pwr_Amp_En[path4] == ON) // if User ON Control
		{
			switch(m_St.Pwr_Amp_Off_Reason[path4])
			{
				// if No Off-reason, then AMP-ON
				// but when AC-Fail or Standby Mode 
				// Control is Cancelled.
				case 0x00:
				case rsn_Unknown:
					if(Rvs_Amp_Off_rsn[path4]==0) // 2019.05.04
					{
						if(m_St.Pwr_Amp_Off_Reason_PAU[path4]==0) // 2019.09.11
						{
							if(IS_RU_Init_Completed() || m_Env.AMP_User_Ctrl[path4]) // 2019.09.24							
								PAU_EN_Ctrl(path4, ON); 
						}
					}
					//D_Printf( DEBUG_STDBY, "[RCU] PAU_EN_Ctrl : AMP#%d ON .\n", path4+1 );
					break;
					
				default:
					// amp off now
					break;
			}
		}
		else // User-off-req
		{
			PAU_EN_Ctrl(path4, OFF); // USER
			
			if(m_Env.debug_path4 == path4)
				print_agent(DEBUG_STDBY, VT_BOLD_CYAN, "PAU_EN_Ctrl : AMP#%d off [3] by user.\n", path4+1 );
		}
	}
#endif
}



//=====================================================================

// Multi down

//=====================================================================


void Init_AMP_MutiDn(INT8U path4)
{
	m_AutoDn.req[path4] = 0;
	m_AutoDn.percent[path4] = 0;
	m_AutoDn.result[path4] = Multi_Dn_None;
	m_AutoDn.fw_apply_req[path4] = 0;
}

static INT8U* DnImg[4] = {NULL,NULL,NULL,NULL};
void Free_RAM_DnImg( INT8U path4 )
{
	if (DnImg[path4] != NULL)
	{
		free(DnImg[path4]);
		DnImg[path4] = NULL;
	}
}

/*
----------------------------------------------------------
PAU Only, 
	(1) memory allocation
	(2) open and read pullFilePath
	(3) loading file to memory
----------------------------------------------------------
*/
int Load_RAM_DnImg( char* pullFilePath ,INT8U path4)
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

	Free_RAM_DnImg( path4 );
	
	DnImg[path4] = (INT8U*)malloc(fileSize+1);
		
	if (DnImg[path4])
	{			
		memset( DnImg[path4], 0, fileSize+1);
		re = read( fd, DnImg[path4], fileSize );
		if( re <= 0 )
		{
			Free_RAM_DnImg( path4 );	
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


#ifdef __TYPE__PRU__
/*
------------------------------------------------------------------
path 에 해당하는 AMP full_path_file_name 을 얻어서, 
mem-alloc 받은 memory에 loading 한다.
------------------------------------------------------------------
*/
int Get_Multi_Dn_Tot_Frame_Crc( INT16U* pCRC, INT16U* pTotFrame, MOD_TYPE* pMod )
{
	char file[128];
	int size;
	int frame_co;
	INT16U tot_crc;
	INT8U path4;

	path4 = pMod->path4;
	
	Get_AMP_Dn_File_Name( m_St.Pwr_Amp_Made[path4], file);

	size = Load_RAM_DnImg( file, path4 );

	print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Multi-AMP_#%d] size %d, file %s", path4+1, size, file);	

	if(size < 0) return -1;

	tot_crc = crcLByteCalc( 0, DnImg[path4], size ); // init with 0

	frame_co = size / FRAME_DATA_MAX;
	if((size % FRAME_DATA_MAX) > 0) frame_co += 1;
	
	*pCRC = tot_crc;
	*pTotFrame = frame_co;	

	print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Multi-AMP_#%d] Tot CRC 0x%04X, Tot Frame %d", path4+1, tot_crc, frame_co);

	return size;
}
#endif


INT16U Get_Multi_Dn_Data(INT8U path4, INT8U* data, INT16U cur_frame, INT16U tot_frame, int tot_size )
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

	memcpy( data, &DnImg[path4][idx], len);

	return len;
}

void Rx_PAU_AutoDown( INT8U path4, INT8U rx_type, INT8U* data ) 
{
	INT16U uVal16;
	INT8U nack = 0; // default

	//printf("amp down rx cmd [0x%02X] \r\n", rx_type);

	switch( rx_type )
	{	
		case AMP_CMD_DN_NOTI:    // 1,2차 통보.
		case AMP_CMD_DN_NOTI_FW: // 1,2차 통보, FW 적용.
		case AMP_CMD_DN_NOTI_CHK:			
		case AMP_CMD_DN_DATA:	
		case AMP_CMD_DN_END:
			if(data[0] != 0) 
			{
				nack = 1;
				m_AutoDn.retry[path4]++;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-AMP%d] down rx nak [0x%02X]", path4, rx_type);
				return;
			}
			break;
	}	
	
	
	switch( rx_type )
	{	
		case AMP_CMD_DN_NOTI:    // 1,2차 통보.
		case AMP_CMD_DN_NOTI_FW: // 1,2차 통보, FW 적용.			
			
			// total frame
			uVal16 = data[2]; uVal16 <<= 8; uVal16 |= data[3];			
			if(uVal16 != m_AutoDn.tot_frame[path4]) 
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-AMP%d] down nak tot frame %d != %d", 
					path4, uVal16 ,m_AutoDn.tot_frame[path4]);
				nack = 1;
			}
						
			if(nack == 0)
			{
				if(m_AutoDn.st[path4] == Auto_Dn_Noti_1) 
					 m_AutoDn.st[path4] = Auto_Dn_Chk;
				else m_AutoDn.st[path4] = Auto_Dn_Data;
			}
			break;

		case AMP_CMD_DN_NOTI_CHK: // 통보 확인.
			if(nack == 0) m_AutoDn.st[path4] = Auto_Dn_Noti_2;
			break;
			
		case AMP_CMD_DN_DATA: // 다운로드
			// cur frame
			uVal16 = data[1]; uVal16 <<= 8; uVal16 |= data[2];
			if(uVal16 != m_AutoDn.cur_frame[path4]) 
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-AMP%d] down nak cur frame %d != %d", 
					path4, uVal16, m_AutoDn.cur_frame[path4]);
				nack = 1;
			}
			
			if(nack == 0)
			{				
				if(uVal16 >= (m_AutoDn.tot_frame[path4] - 1)) // if last frame
				{
					m_AutoDn.st[path4] = Auto_Dn_End;
					print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Multi-AMP%d] down last", path4 );
				}
				else m_AutoDn.cur_frame[path4]++;
			}
			break;
	
		case AMP_CMD_DN_END: // 확인
			// tot frame
			uVal16 = data[1]; uVal16 <<= 8; uVal16 |= data[2];
			if(uVal16 != m_AutoDn.tot_frame[path4]) 
			{
				nack = 1;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Multi-AMP%d] down nak tot frame %d != %d", 
					path4, uVal16, m_AutoDn.tot_frame[path4]);
			}
			
			if(nack == 0) 
			{
				m_AutoDn.st[path4] = Auto_Dn_Success;
				///// YCSO changed 190510
				set_amp_DN_sataus(path4, _SET_);
//				set_zip_DN_flag(2);//	Zip_flag_PAU
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Multi-AMP%d] down success", path4 );
			}
			break;
	}	

	if(nack) m_AutoDn.retry[path4]++;
	else	 m_AutoDn.retry[path4] = 0;
}

//---------------------------------------------------------------------------

INT8U AutoDown_PAU_Send_Data( INT8U path4, INT8U type, INT8U* data, INT16U l )
{
#ifdef __TYPE__PRU__ // PRU Only
	INT16U len;
	INT8U ComPort;
	INT8U pau_id;
	
	switch(path4)
	{
		case AMPU_4_1: ComPort = COMM_UART_AMP_1; break;		
		case AMPU_4_2: ComPort = COMM_UART_AMP_2; break;	
		case AMPU_4_3: ComPort = COMM_UART_AMP_3; break;		
		case AMPU_4_4: ComPort = COMM_UART_AMP_4; break;		

		default: return 0;
	}	

	pthread_mutex_lock(&PAU_COMM_Evt[path4]);

	// RCU
	pau_txf[path4].ScrAddr = 0x00;
	pau_id = _PAUID_PAU_ + path4;
	// 2019.08.12 : PAUID : 0x20 -> 0x20/21/22/23
	////pau_id = _PAUID_PAU_;
	pau_txf[path4].DesAddr = pau_id;
	pau_txf[path4].Type = type;

	if(l) memcpy( pau_txf[path4].data, data, l );		
	len = Make_Frame_PAU( type, &pau_txf[path4], l, path4 );

	if( Send_DN_Data( ComPort, (INT8U*)&pau_txf[path4], len, (INT8U*)&pau_rxf[path4], PAU_Timout_ms ) )
	{		
		if( (pau_rxf[path4].Type == type) && (pau_rxf[path4].ScrAddr == pau_id) )
		{	
			Rx_PAU_AutoDown( path4, type, pau_rxf[path4].data );
			pthread_mutex_unlock(&PAU_COMM_Evt[path4]);

			return 1;	
		}
		else
		{
			D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down rx fail type [0x%02X 0x%02X]  addr [0x%02X]\r\n",\
				pau_rxf[path4].Type, type, pau_rxf[path4].ScrAddr);
		}
	}
	else
	{
		D_Printf( DEBUG_SYSTEM, "[!] PAU Dn [%s] 0x%02X\r\n", Get_path4_Str(path4), type );	    

	    /*
	    2019.10.04 : Add PAU-Debug when Comm-Timeout.
	    */
	    Comm_Que_Dbg(ComPort, type);
	}

	pthread_mutex_unlock(&PAU_COMM_Evt[path4]);
	CheckUART_Error( ComPort );
#endif
	return 0;	
}

//---------------------------------------------------------------------------
#define Retry_Max (5)		//// added YCSO 190920 // 2019.09.20
void AMP_Down_Fail( INT8U path4 )
{
	m_AutoDn.retry[path4] = 0;
	m_AutoDn.result[path4] = Multi_Dn_Fail;
	m_AutoDn.st[path4] = Auto_Dn_None;
	m_AutoDn.req[path4] = 0; // end
	Free_RAM_DnImg( path4 );
	m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920 // 2019.09.20
	D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down fail[%d] \r\n", path4);
}
//---------------------------------------------------------------------------
#ifdef __TYPE__PRU__ // PRU Only

void Proc_AMP_MultiDownload(INT8U path4)
{
	INT8U pau_type;
	INT16U l = 0;
	INT8U ret;
	INT16U uVal16;
	MOD_TYPE DnMod;
	int re;


	DnMod.type = Mod_AMP;
	DnMod.path4 = path4;	
	
	switch( m_AutoDn.st[path4] )
	{
		case Auto_Dn_None:
			Free_RAM_DnImg( path4 );
			break;			

		case Auto_Dn_Start:			
			D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down start %s\r\n", Get_path4_Str(path4));
			syslog( LOG_CRIT, VT_BOLD_GREEN " AMP down start %s \n" VT_END_CR, Get_path4_Str(path4));

			m_AutoDn.cur_frame[path4] = 0;			
			m_AutoDn.retry[path4] = 0;
			m_AutoDn.result[path4] = Multi_Dn_Progress;
			m_AutoDn.percent[path4] = 0;
			m_AutoDn.fw_apply_req[path4] = 0;			
			// get crc and tot-frame
								
			re = Get_Multi_Dn_Tot_Frame_Crc( &m_AutoDn.crc[path4], &m_AutoDn.tot_frame[path4], &DnMod );
			if(re <= 0) AMP_Down_Fail( path4 );
			else 
			{
				m_AutoDn.st[path4] = Auto_Dn_Noti_1;
				m_AutoDn.tot_size[path4] = re;
			}
			break;		

		case Auto_Dn_Noti_1:
		case Auto_Dn_Noti_2:			
			D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down Noti %d %s \r\n", 
				m_AutoDn.st[path4], Get_path4_Str(path4));
			
		#ifdef __YCSO__TEST__CONFIRM__
			pau_type = AMP_CMD_DN_NOTI_FW;
		#else
			pau_type = AMP_CMD_DN_NOTI;
		#endif
			uVal16 = m_AutoDn.tot_frame[path4];
			
			m_AutoDn.data[path4][l++] = 0; // reserved
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 >> 8); // total frame size
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 & 0xFF); // total frame size

			// operation mode (0:reset waiting, 1 : auto reset after download)
			if(pau_type == AMP_CMD_DN_NOTI_FW)
			{
				m_AutoDn.data[path4][l++] = 0; 
			}
			break;

		case Auto_Dn_Chk:
			D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down chk %d %s \r\n", 
				m_AutoDn.cur_frame[path4], Get_path4_Str(path4));
			pau_type = AMP_CMD_DN_NOTI_CHK;
			l = 0; // no subdata
			break;

		case Auto_Dn_Data:			
			pau_type = AMP_CMD_DN_DATA;
			uVal16 = m_AutoDn.cur_frame[path4];
			l = 0;
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 >> 8); // cur frame #
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 & 0xFF); // cur frame #
			
			// download data (payload)			
			l += Get_Multi_Dn_Data(path4, &m_AutoDn.data[path4][l], uVal16, m_AutoDn.tot_frame[path4], m_AutoDn.tot_size[path4] );
			m_AutoDn.percent[path4] = uVal16 *100 / m_AutoDn.tot_frame[path4];
			
			//printf("amp down %d [%d] %d \r\n", m_AutoDn.cur_frame[path4], path4, l-2);
			//printf("#\n");
			break;

		case Auto_Dn_End:
			D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down end %s \r\n", Get_path4_Str(path4));
			pau_type = AMP_CMD_DN_END;
			uVal16 = m_AutoDn.tot_frame[path4];
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 >> 8); // total frame size
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 & 0xFF); // total frame size
			uVal16 = m_AutoDn.crc[path4];
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 >> 8);; // total crc
			m_AutoDn.data[path4][l++] = (INT8U)(uVal16 & 0xFF); // total crc
			break;

		case Auto_Dn_Success:
			D_Printf( DEBUG_SYSTEM, "[Multi-Amp] down success %s \r\n", Get_path4_Str(path4));
			syslog( LOG_CRIT, VT_BOLD_GREEN " AMP down success %s \n" VT_END_CR, Get_path4_Str(path4));
			m_AutoDn.st[path4] = Auto_Dn_None; 
			m_AutoDn.req[path4] = 0; // end
			m_AutoDn.percent[path4] = 100;
			m_AutoDn.fw_apply_req[path4] = 1;
//			m_St.AMP_DL_St[path4][1] = 100;
//			m_St.AMP_DL_St[path4][0] = DL_St_Complete;
			///// YCSO changed 190510
			#ifdef __YCSO__TEST__CONFIRM__
			set_amp_DN_sataus(path4, _SET_);
			#else
			set_amp_DN_confirm_END(path4, _SET_);
			#endif

			AMP_Init_Set(path4);
			break;
			
		default :
			m_AutoDn.st[path4] = Auto_Dn_None; 
			break;
	}

	switch( m_AutoDn.st[path4] )
	{
		case Auto_Dn_Noti_1:
		case Auto_Dn_Noti_2:
		case Auto_Dn_Chk:
		case Auto_Dn_Data:
		case Auto_Dn_End:
			ret = AutoDown_PAU_Send_Data( path4, pau_type, m_AutoDn.data[path4], l );

			if(ret == 0) m_AutoDn.retry[path4]++;
			
			if(m_AutoDn.retry[path4] >= Retry_Max)
			{
				AMP_Down_Fail(path4);
			}
			break;

		default: break;
	}
}
#endif


//---------------------------------------------------------------------------
void Update_AMP_DL_State(void)
{	
#ifdef _Debug____MULTI_DOWN_ST_
	INT8U path4;	
	
	for( path4 = 0; path4 < AMPU_4_MAX; path4++ )
	{
		if(m_AutoDn.req[path4])
		{
			m_St.AMP_DL_St[path4][1] = m_AutoDn.percent[path4]; // [Dl_State/%]
			m_St.AMP_DL_St[path4][0] = DL_St_Now;				
		}

		if(m_AutoDn.fw_apply_req[path4])
		{
			m_St.AMP_DL_St[path4][1] = 100;
			m_St.AMP_DL_St[path4][0] = DL_St_Complete;
		}	 	
	}
#endif	
}
//---------------------------------------------------------------------------
INT8U Is_AMP_MultiDown(void)
{
	INT8U path4;	
		
	if(info_PSU.upgrade) return 1;

	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if( Check_PAU_Use( path4 ) )
		{
			if(info_AMP[path4].upgrade) return 1;
			if(m_AutoDn.req[path4]) return 1;
		}
	}	
	return 0;
}
//---------------------------------------------------------------------------
#ifdef __TYPE__PRU__ // PRU Only
INT8U Proc_PAU_PkgDn(INT8U path4)
{
	/*
	// 2020.04.02 : Use 'AMP.HW.Insert.Info' instead of 'AMP_Use'
	////if(Check_PAU_Use(path4)) 
	*/
	if(IS__AMP_Use_Allowed(path4)) 	
	{
		if(info_AMP[path4].upgrade)
		{
			info_AMP[path4].upgrade = 0;
			if( m_St.Pwr_Amp_Link_Err[path4] == 0 )		//// added YCSO 190920 // 2019.09.20
			{
				set_amp_DN_flag(path4, _SET_);
				Init_AMP_MutiDn(path4);
				m_AutoDn.req[path4] = 1;
			}
		}
		
		if(m_AutoDn.req[path4])
		{
			if(m_AutoDn.result[path4] == Multi_Dn_None)
			{
				m_AutoDn.st[path4] = Auto_Dn_Start;
				m_AutoDn.cur_path4[path4] = path4;
			}
			Proc_AMP_MultiDownload( m_AutoDn.cur_path4[path4] );

			return 1;
		}
	}
	else
	{
		m_AutoDn.req[path4] = 0;
		m_AutoDn.fw_apply_req[path4] = 0;
	}
	return 0;
}
#endif
//------------------------------------------------------------------




//------------------------------------------------------------------

/* EOF */


