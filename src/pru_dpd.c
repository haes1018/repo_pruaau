/////////////////////////////
// pru_dpd.c
/////////////////////////////
#define PRU_DPD_GLOBALS
#include "includes.h"
/////////////////////////////


/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////
#ifdef __TYPE__PRU__
//===================================================================================

// ----------------------------------------------------------
#ifdef _SHORTEN_TIME____DPD_ALARM_ 
#define DPD_ALM_KEEP_ms     (15000)    
#define DPD_RST_Cnt_Init_ms (15000)
#define DPD_ALM_Clr_ms      (15000)
// ----------------------------------------------------------
#else // Normal Algorithm Time 
#define DPD_ALM_KEEP_ms     (30000) // dpd reset counter 위한 dpd alarm 유지 시간.
#define DPD_RST_Cnt_Init_ms (30000) // dpd reset counter clear 0 되기 위한 dpd normal 유지 시간.
#define DPD_ALM_Clr_ms      (30000) // PAU Reset 후, dpd normal 유지시간.
#endif  
//-------------------------------------------------------------------
/*
5G, DPD-Engine-In-Pwr
2019.09.17 : -23.0[dBm] --> -20.0[dBm]
*/
#define DPD_ALM_PWR_x10 (-200)   // -20.0[dBm]
//-------------------------------------------------------------------
typedef struct
{
	INT8U st;
	INT8U keep_en;
	INT8U reset_cnt; 
	INT8U recovery_chk; 
	OS_TIMER timer;
	OS_TIMER alm_timer; // alarm check
	OS_TIMER clr_timer; // normal check
}DPD_CHK_ST;

static DPD_CHK_ST dpd_err[AMPU_4_MAX];

enum
{
	d_chk_none = 0,
	d_after_30sec,
	d_chk_recovery, // recovery 위한 동작.
	d_chk_att_chk,
	d_chk_att_pair_off,	
	d_chk_att_pair_on,
	d_chk_fb_offset_set,
	d_chk_recovery_end,
	d_chk_alm,      // error-check

	d_standby_user_pau_reset,
	d_chk_pau_reset_ack,
};

//-----------------------------------------------------

//-----------------------------------------------------

void Init_dpd_err(void)
{
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		dpd_err[path4].st = d_chk_none;
}
//-----------------------------------------------------

void set_dpd_t_alm( INT8U idx, INT8U path4, INT8U set )
{
	if(set) m_St.dpd_t_alm[path4] |= cbits[idx];
	else    m_St.dpd_t_alm[path4] &= nbits[idx];
}

void set_dpd_alarm(INT8U path4, INT8U set, INT16S dpd_err )
{
	m_St.AlmSt.Path[path4].DPD.DPD_Alm = set;
	
	if(set) m_St.DPD_Alarm_St[path4] = dpd_err;
	else    m_St.DPD_Alarm_St[path4] = 0;	
}


INT8U Is_dpd_t_alm( INT8U idx, INT8U path4 )
{
	return (m_St.dpd_t_alm[path4] & cbits[idx]);
}

//-----------------------------------------------------

INT8U Is_DpdErrSt_Alm( INT8U path4 ) // dpd_chk
{
	INT8U is_dpd_err;
	
	if(path4 >= MAX_DL) return 0;

	switch(m_St.d_DPD_Err_St[path4])
	{		
		case -114 : // CAPTURE_SCA_FAILURE
		case -110 : // CAPTURE_DCL_SCA_FAILURE
		case   33 : // LOW_RX_POWER
		case  255 : // OVERDRIVE_DETECTED
		case  256 : // OVERDRIVE_PROTECTED
		case  258 : // EXPANSION_SATURATION
			is_dpd_err = 1;
			break;			

		default:
			/*
			ZERO(0), 
			COMMAND_IN_PROGRESS(1), 
			SUCCESSFUL(2)
			*/
			is_dpd_err = 0;
			break;
	}
	
	if(is_dpd_err)
	{		
		if(m_St.d_DPD_Eng_In__Pwr_x10[path4] > DPD_ALM_PWR_x10) 
			return 1; // alarm
		else return 0; 
	}
	else return 0; 	
}

/*
// 2019.05.14
---------- DPD Alarm 
-114 : CAPTURE_SCA_FAILURE
-110 : CAPTURE_DCL_SCA_FAILURE
  33 : LOW_RX_POWER
 255 : OVERDRIVE_DETECTED
 256 : OVERDRIVE_PROTECTED
 258 : EXPANSION_SATURATION
 
 -------------------------
 DPD 알람/해제 발생 Procedure
 -------------------------
. 상기 알람 항목 발생 후 30초 유지 시, Reset 수행
- 초기 2번은 path만 Reset, 
- 마지막 1번 DPD Core Reset 수행
- 3회 Reset 수행 후에도 알람 조건이 30초간 유지되면 DPD 알람 발생 후 해당 Path에 대해 Bypass Mode 수행

. Reset 후 DPD Status 
상기 ZERO(0), COMMAND_IN_PROGRESS(1), SUCCESSFUL(2) 상태가 
30초 유지시 정상으로 판단 
조건 충족 시DPD 알람 해제.
*/

//-----------------------------------------------------
// Clear Condition : DPD_Err_St
INT8U Is_DpdErrSt_Normal( INT8U path4 ) //------------------------> // dpd_chk
{
	INT16S sErr_Val;	
	INT32U uVal32;

	uVal32 = m_St.d_DPD_Err_St[path4]; 
	
	sErr_Val = (INT16S)uVal32;	
	
	if(sErr_Val == (255) ) return 0;
	if(sErr_Val == (256) ) return 0;
	if(sErr_Val == (-120)) return 0; // ALIGN_FAILURE (-120)
	
	return 1;
}


// err condition 이면, 1 리턴 .

/*
// Not-Used
INT8U Is_Dpd_Pwr_Normal( INT8U path4 ) //------------------------> // dpd_chk
{
	INT16S dpd_pwr_10;
	
	dpd_pwr_10 = m_St.d_DPD_In__Pwr_x10[path4];

	if(dpd_pwr_10 < DPD_ALM_PWR_x10) return 1;
	else return 0;
}
*/

//-----------------------------------------------------
void DPD_Reset_Req(INT8U path4)
{
	// DPD Reset Set to FPGA
}

//-----------------------------------------------------

// err condition 이면, 1 리턴 .

INT16S Get_DPD_FB_Pwr_differ( INT8U path4 )
{	
	INT16S fb_pwr_x10, dpd_pwr_x10;

	dpd_pwr_x10 = m_St.d_DPD_In__Pwr_x10[path4]; // dpd_chk
	fb_pwr_x10 = m_St.d_DPD_FB_In__Pwr_x10[path4];

	return (dpd_pwr_x10 - fb_pwr_x10);
}
//-----------------------------------------------------

//-----------------------------------------------------
/*
#define Differ_ALARM__Chk_x10  (90) // normal에서 alarm 발생조건.: 9dB
#define Differ_NORMAL_Chk_x10  (70) // alarm 에서 normal해제조건.: 7dB

INT8U Is_DpdSt_Alm( INT8U path4, INT16U max_diff_x10 ) // dpd_chk
{
	INT16S differ_10;

	differ_10 = Get_DPD_FB_Pwr_differ( path4 );
	
	if( differ_10 < max_diff_x10 ) return 0; // clear condition
	else if(m_St.d_DPD_St[path4] != (33)) return 0; // clear condition
	else 
	{
	//	D_Printf( DEBUG_SYSTEM, "[DPD] Is-DpdSt-Alm, differ %d, DPD_St(%d)\n", differ_10, m_St.d_DPD_St[path4]);
		return 1;
	}
}
*/

//-----------------------------------------------------




//-----------------------------------------------------

enum
{
	DPD_ALM_Err = 0, // DPD Err Alarm
	DPD_ALM_FB       // Feedback Alarm
};


/*
AMP OFF
AMP 관련 알람 발생
출력이 0dB인 Path
*/ 
INT8U Is_DPD_Mask_State(INT8U path4)
{	
	if( Check_PAU_Use(path4) == 0 ) return 1;
	if( m_St.AlmSt.Path[path4].DL.Out_Lower ) return 2;
	if( m_St.AlmSt.Path[path4].PAU.Disabled ) return 3;
	if( m_St.AlmSt.Path[path4].PAU.Dev_Fail ) return 4;
	if( m_St.AlmSt.Path[path4].PAU.DC_Fail ) return 5;
	
	return 0;
}
//-----------------------------------------------------


	


/*
--------------------------------------------------------------------------------------
1. DPD Alarm(Error)
	<A> 알람 발생 조건   
	  (DPD_Err_St (0,1,2 제외) 10분 이상 유지) &&  (DPD_Pwr(DPD) >= -27dBm)
	  
	<B> 정상 해제 조건 
	  DPD_Err_St (0,1,2 제외) 30초 이상 유지 시.	  

	 <C> AMP link fail : dpd alarm check 동작 안함. (기존 발생된 알람은 유지)
	 <D> 아래 조건의 경우, dpd alarm check 동작 안함. ( 알람은 정상으로 mask )
		(1) PAU 미사용.
		(2) FWD AMP OFF
		(3) AMP Device Fail alarm
		(4) AMP DC Fail alarm
--------------------------------------------------------------------------------------
*/

/*
// 2019.05.14
---------- DPD Alarm 
-114 : CAPTURE_SCA_FAILURE
-110 : CAPTURE_DCL_SCA_FAILURE
  33 : LOW_RX_POWER
 255 : OVERDRIVE_DETECTED
 256 : OVERDRIVE_PROTECTED
 258 : EXPANSION_SATURATION

 ZERO(0), 
 COMMAND_IN_PROGRESS(1), 
 SUCCESSFUL(2)
 -------------------------
 DPD 알람/해제 발생 Procedure
 -------------------------
. 상기 알람 항목 발생 후 30초 유지 시, Reset 수행
- 초기 2번은 path만 Reset, 
- 마지막 1번 DPD Core Reset 수행
- 3회 Reset 수행 후에도 알람 조건이 30초간 유지되면 DPD 알람 발생 후 해당 Path에 대해 Bypass Mode 수행

. Reset 후 DPD Status 상태가 30초 정상 유지시 정상으로 판단, 조건 충족 시DPD 알람 해제.
*/

#define fb_att_sum_x2 (25*2)
#define fb_offset_req_x2  (2*2) // 2[dB] = 2x2

void Proc__DPD_Alm____Error(void)
{
	INT8U path4;
	INT8U fwd_att_x2, fb_att_x2;
	INT8U att_x2, offset_x2;	
	
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{		
		fwd_att_x2 = m_St.Fwd_Out_Att_x2[path4];
		fb_att_x2  = m_St.FB_ATT_x2[path4];
		offset_x2  = m_St.FB_Offset_ATT_x2[path4];
		//------------------------------------
		if(Is_DPD_Mask_State(path4))
		{
			//if(dpd_err[path4].st != d_chk_none)
			//	D4_Printf(DEBUG_STDBY, path4, "[DPD #%d] Is_DPD_Mask_State(%d) \n", path4+1, dpd_err[path4].st);
			dpd_err[path4].st = d_chk_none;	
		}
		//------------------------------------
		switch(dpd_err[path4].st)	
		{		
			case d_standby_user_pau_reset:								
			case d_chk_pau_reset_ack:
				break;
				
			default:
				if(m_St.d_DPD_OnOff[path4] == OFF)
					dpd_err[path4].st = d_chk_none;
				break;
		}
		//------------------------------------		
		switch(dpd_err[path4].st)
		{
			case d_chk_none:
				dpd_err[path4].reset_cnt = 0;
				dpd_err[path4].recovery_chk = 0;
				OSTimerReset( &dpd_err[path4].alm_timer );
				OSTimerReset( &dpd_err[path4].clr_timer );				
				
				#ifdef __USE_2BYTE_DPD_Error__
				if(Is_dpd_t_alm( DPD_ALM_Err, path4 ))
					set_dpd_alarm(path4, _ALM_CLR_, 0);
				#else
				if(m_St.AlmSt.Path[path4].DPD.DPD_Alm)
				{
					set_dpd_t_alm( DPD_ALM_Err, path4, _ALM_CLR_);				
					set_dpd_t_alm( DPD_ALM_FB , path4, _ALM_CLR_); // 2020.04.01
				}
				#endif				
				
				if(Check_PAU_Use(path4))
					dpd_err[path4].st = d_chk_alm;
				break;
			//----------------------------
			case d_chk_alm:
				if(Is_DpdErrSt_Alm(path4))				
				{
					D4_Printf(DEBUG_DPD, path4, "[DPD #%d] dpd err st\n", path4+1);
						
					if(OSTimerGet_msec(&dpd_err[path4].alm_timer) >= DPD_ALM_KEEP_ms ) 
					{						
						OSTimerReset( &dpd_err[path4].alm_timer );
						OSTimerReset( &dpd_err[path4].clr_timer );

						dpd_err[path4].st = d_after_30sec;
						dpd_err[path4].reset_cnt++;
						
						D4_Printf(DEBUG_DPD, path4, "[DPD_Err #%d] reset count %d\n", path4+1, dpd_err[path4].reset_cnt);
					}
				}	
				else // normal
				{					
					D4_Printf(DEBUG_DPD, path4, "[DPD #%d] dpd Normal st, d_DPD_Err_St %d, DPD_Eng_In__Pwr %d \n", 
						path4+1, 
						m_St.d_DPD_Err_St[path4], 
						m_St.d_DPD_Eng_In__Pwr_x10[path4]);
						
					
					OSTimerReset( &dpd_err[path4].alm_timer );
				
					if(dpd_err[path4].reset_cnt==0)
						dpd_err[path4].st = d_chk_none;
					else
					{
						if(OSTimerGet_msec(&dpd_err[path4].clr_timer) >= DPD_RST_Cnt_Init_ms) 
						{							
							dpd_err[path4].reset_cnt =0;
							dpd_err[path4].st = d_chk_none;
						}
					}
				}
				break;
			//----------------------------
			case d_after_30sec:
			
				D4_Printf(DEBUG_DPD, path4, "[DPD_Err #%d] d_after_30sec\n", path4+1);
							
				if((m_St.d_DPD_Err_St[path4] == 33)&&(dpd_err[path4].recovery_chk==0))
				{
					dpd_err[path4].st = d_chk_recovery; // 1 time only				
				}
				else if(dpd_err[path4].reset_cnt <= 3) // when 1st, 2nd, 3rd
				{
					if(dpd_err[path4].reset_cnt < 3) DPD_Reset_Ctrl(0x03 + path4);  // when 1st, 2nd
					else 					         DPD_Reset_Ctrl(0x02); // when 3rd, core reset 

					OSTimerReset( &dpd_err[path4].alm_timer );
					OSTimerReset( &dpd_err[path4].clr_timer );
					
					dpd_err[path4].st = d_chk_alm;
				}
				else
				{	
					#ifdef __USE_2BYTE_DPD_Error__
					set_dpd_alarm(path4, _ALM_SET_, m_St.d_DPD_Err_St[path4]);
					#else
					if(m_St.d_DPD_Err_St[path4] == 33) // 2020.04.01 : DPD Error 33d일 경우 Feedback Alarm(0x00)으로 보고
						set_dpd_t_alm( DPD_ALM_FB, path4, _ALM_SET_ );
					else	
						set_dpd_t_alm( DPD_ALM_Err, path4, _ALM_SET_ );					
					#endif
					DPD_OnOff_Ctrl(path4, OFF); // DPD_OFF is bypass control
					dpd_err[path4].st = d_standby_user_pau_reset;

					
					D4_Printf(DEBUG_DPD, path4, "[DPD #%d] standby PAU User reset.\n", path4+1);
				}
				break;
			//----------------------------
			case d_standby_user_pau_reset:
				if(m_Env.User_Pwr_AMP_Rst[path4])
				{
					D4_Printf(DEBUG_DPD, path4, "[DPD #%d] User PAU Reset.\n", path4+1);
						
					m_Env.User_Pwr_AMP_Rst[path4] = 0;
					dpd_err[path4].st = d_chk_pau_reset_ack;
				}				
				break;
			//----------------------------				
			case d_chk_pau_reset_ack:							
				if(m_Env.Pwr_AMP_Rst_Ack[path4])
				{
					m_Env.Pwr_AMP_Rst_Ack[path4] = 0;

					OSTimeDly(2*OS_DELAY_1sec);					
					
					D4_Printf(DEBUG_DPD, path4, VT_GREEN "[DPD #%d] PAU Reset ACK\n", path4+1);
				}
				else 
				{
					D4_Printf(DEBUG_DPD, path4, VT_GREEN "[DPD #%d] PAU Reset ACK Fail \n", path4+1);
				}
				#ifdef __USE_2BYTE_DPD_Error__
				set_dpd_alarm(path4, _ALM_CLR_, 0);
				#else
				set_dpd_t_alm( DPD_ALM_Err, path4, _ALM_CLR_);
				set_dpd_t_alm( DPD_ALM_FB , path4, _ALM_CLR_); // 2020.04.01
				#endif
				DPD_OnOff_Ctrl(path4, ON); // DPD_ON is DPD_Bypass Clear
						
				dpd_err[path4].st = d_chk_none;												
				break;
			//-----------------------------------------	



			

			//-----------------------------------------
			case d_chk_recovery:
				dpd_err[path4].recovery_chk = 1;
				
				D4_Printf(DEBUG_DPD, path4, "[DPD #%d] recovery start\n", path4+1);
								
				if(m_St.Pwr_Amp_Link_Err[path4])
				{
					D4_Printf(DEBUG_DPD, path4, " [DPD #%d] PAU Link Fail. Goto recovery_end\n", path4+1);
					dpd_err[path4].st = d_chk_recovery_end;					
					break;
				}				
					
				if((fwd_att_x2 + fb_att_x2) != fb_att_sum_x2)
				{
					ATT_Pair_Onoff_Req( path4, OFF );
					dpd_err[path4].st = d_chk_att_pair_off;
				}					
				else if(offset_x2 > fb_offset_req_x2)
				{
					FB_Offset_Att_Req( path4, fb_offset_req_x2 );
					dpd_err[path4].st = d_chk_fb_offset_set;
				}
				else
					dpd_err[path4].st = d_chk_recovery_end;		
				break;

			//-----------------------------------------
			case d_chk_att_pair_off: // next fb-att-set	
				D4_Printf(DEBUG_DPD, path4, "[DPD #%d] att_pair_off\n", path4+1);
					
				if( m_St.ATT_Pair_Onoff[path4] != OFF)
					ATT_Pair_Onoff_Req( path4, OFF );
				else
				{
					FB_Att_Req( path4, (fb_att_sum_x2 - fwd_att_x2));
					dpd_err[path4].st = d_chk_att_chk; // next fb-att-set
				}
				break;

			case d_chk_att_chk: // next pair-on
				D4_Printf(DEBUG_DPD, path4, "[DPD #%d] att_chk\n", path4+1);
					
				att_x2 = fb_att_sum_x2 - fwd_att_x2;
				
				if( fb_att_x2 != att_x2)
					FB_Att_Req( path4, att_x2);
				else
				{
					ATT_Pair_Onoff_Req( path4, ON );
					dpd_err[path4].st = d_chk_att_pair_on; // next pair-on
				}
				break;

			case d_chk_att_pair_on:				
				D4_Printf(DEBUG_DPD, path4, "[DPD #%d] att_pair_on\n", path4+1);
					
				if( m_St.ATT_Pair_Onoff[path4] != ON)
				{
					ATT_Pair_Onoff_Req( path4, ON );
				}
				else if(offset_x2 > fb_offset_req_x2)
				{
					FB_Offset_Att_Req( path4, fb_offset_req_x2 );
					dpd_err[path4].st = d_chk_fb_offset_set; // next offset-set
				}
				else
				{
					dpd_err[path4].st = d_chk_recovery_end;
				}
				break;

			case d_chk_fb_offset_set:
				D4_Printf(DEBUG_DPD, path4, "[DPD #%d] fb_offset_set\n", path4+1);
					
				if(offset_x2 > fb_offset_req_x2)
					FB_Offset_Att_Req( path4, fb_offset_req_x2 );
				else 
					dpd_err[path4].st = d_chk_recovery_end; 
				break;				
	
			case d_chk_recovery_end:
				D4_Printf(DEBUG_DPD, path4, "[DPD #%d] recovery_end\n", path4+1);
					
				dpd_err[path4].st = d_after_30sec;
				break;

			//-----------------------------------------
			default :
				dpd_err[path4].st = d_chk_none;
				break;
		}		
	}
}

//-----------------------------------------------------
void DPD_Reason_Set( INT8U path4, INT8U dpd_reason )
{		
	m_St.DPD_Alarm_St[path4] = dpd_reason;	
}

/*
	m_St.DPD_Alarm_St[MAX_DL] : AID 0x5321
	-------------------
	0x00 : Feedback Alarm
	0x01 : DPD Error Alarm
	0x02 : DPD Module Link 이상
	0x03 : 기타
	0x04 : APD EEPROM corrupted
	0x05 : APD Internal Chip Error
	0x06 : APD F/W Version fault
	0x07 : APD Link Fail
	0x08 : 정상
*/

//-----------------------------------------------------
void Proc___PRU_DPD_Alarm(void)
{
	INT8U path4;
	INT8U alm;
	INT16S reason;
	static INT8U init = 0;
	static INT8U cnt = 0;
	
	//--------------------------
	if(init == 0)
	{
		init = 1;
		Init_dpd_err();
		
		memset(m_St.DPD_Alarm_St, 0x08, MAX_DL);
	}	
	//--------------------------
	if(m_St.AlmTest) // 2019.05.01
	{		
		return;
	}
	//--------------------------
	cnt++;
	if(cnt < 10) return; // every 1000[ms]
	cnt = 0;
	//--------------------------	
	
	Proc__DPD_Alm____Error();

	#ifdef __USE_2BYTE_DPD_Error__
	#else
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{	
		if(Check_PAU_Use(path4)) 
		{
			// alarm reason : Feedback Alarm 우선
			//
			if( m_St.dpd_t_alm[path4] & cbits[DPD_ALM_FB] ) 
			{
				alm = _ALM_SET_;
				reason = 0x00; // 0:Feedback Alarm
			}
			else if( m_St.dpd_t_alm[path4] & cbits[DPD_ALM_Err] ) 
			{
				alm = _ALM_SET_;
				reason = 0x01; // 1:DPD Error Alarm
			}
			else
			{
				alm = _ALM_CLR_;
				reason = 0x03; // 기타
			}			
		}
		else
		{
			alm = _ALM_CLR_;
			reason = 0x03; // 기타

			m_St.dpd_t_alm[path4] = 0;
		}
		
		m_St.AlmSt.Path[path4].DPD.DPD_Alm = alm;
		DPD_Reason_Set( path4, reason);
	}
	#endif
}


//===================================================================================

#endif

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////






/* EOF */

