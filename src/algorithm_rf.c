/////////////////////////////
// algorithm_rf.c
/////////////////////////////
#define ALGORITHM_RF_GLOBALS
#include "includes.h"
/////////////////////////////


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
static OS_TIMER Amp_Standby_Timer_ms;
static OS_TIMER OSM_Proc_Timer_ms;
static OS_TIMER Mimo_Stdby_Re_ON_Req_Timer; // 일정시간 지난 후에, ON을 시킴.
static OS_TIMER AMP_Stdby_Re_ON_Req_Timer; 


static INT8U f_Amp_In_Time_Prev=0;
static INT8U AMP_Stdby_En_Prev=0;
static INT32U elapse_time32;

static INT8U Is_Skip_AMP_Std = 0;
static OS_TIMER AMP_Stdby_Skip_ms;

//----------------------------------------------------
void G_Timer_All_Init(void)
{
	INT8U i;

	for ( i = 0; i < G_Timer_MAX; i++ )
	{
		g_Timer[i].enable = 0;
		g_Timer[i].is_timeout = 0;
	}
}
//----------------------------------------------------
void G_Timer_Start( INT8U i, INT32U timeout_ms )
{
	if(i >= G_Timer_MAX) return;
	
	g_Timer[i].enable = 1;
	g_Timer[i].is_timeout = 0;
	OSTimerReset( &g_Timer[i].timer );
	g_Timer[i].timeout_ms = timeout_ms;

	//D_Printf(DEBUG_SYSTEM, "G Timer[%d] Start en %d timeout %d", i, g_Timer[i].enable, g_Timer[i].timeout_ms);
}
//----------------------------------------------------
void G_Timer_Stop( INT8U i )
{
	if(i >= G_Timer_MAX) return;

	g_Timer[i].enable = 0;
	g_Timer[i].is_timeout = 0;
}
//----------------------------------------------------
INT32U Get_G_CurTime_ms(INT8U i)
{
	if(i >= G_Timer_MAX) return 0;	

	return OSTimerGet_msec( &g_Timer[i].timer );
}
//----------------------------------------------------
INT8U Is_G_Timeout( INT8U i )
{
	if(i >= G_Timer_MAX) return 0;

	if (g_Timer[i].is_timeout)
	{
		g_Timer[i].is_timeout = 0;
		return 1;
	}
	else return 0;
}
//----------------------------------------------------
INT8U Is_Timer_En( INT8U i )
{
	if(i >= G_Timer_MAX) return 0;

	return (g_Timer[i].enable);
}
//----------------------------------------------------
void Proc_G_Timer(void)
{
	INT8U i;
	INT32U time32;

	for ( i = 0; i < G_Timer_MAX; i++ )
	{
		if(g_Timer[i].enable == 0) continue;

		time32 = OSTimerGet_msec( &g_Timer[i].timer );
		
		if( time32 > g_Timer[i].timeout_ms)
		{
			g_Timer[i].enable = 0;
			g_Timer[i].is_timeout = 1;

			//D_Printf(DEBUG_SYSTEM, "G Timer[%d] timeout en %d timeout %d", i, g_Timer[i].enable, g_Timer[i].timeout_ms);
		}
	}
}



void Init_RU_Proc(void)
{
	OSTimerReset( &AC_Fail_Timer_ms );
	OSTimerReset( &OSM_Proc_Timer_ms );	
}


INT8U Is_PD_Alarm(void)
{
#if 1
	return 0;
#else // 5G, lager	
	return m_St.AlmSt.UDA.PD_CPRI; // ??
#endif	
}


//----------------------------------------------------------
INT8U Is_PD_Alarm_AMP_Std(void)
{
#if 1 // @5G, later will be used..
	return 0;
#else	
	return m_St.AlmSt.UDA.PD_CPRI; // ??
#endif	
}

//----------------------------------------------------------
INT8U IS_Dig_Alarm(void)
{
	//-----------------
	// Digital Board
	//-----------------
	return 0;
}
//----------------------------------------------------------

INT8U GetOffReson( INT8U Amp_SD )
{
	INT8U reason = 0; // default
	
	if     ( Amp_SD & AMP_SD_PD )     reason = rsn_PD;	// PD 이상.
	else if( Amp_SD & AMP_SD_DIG )    reason = rsn_Digital;  // Digital Board 이상.
	else if( Amp_SD & AMP_SD_LOCAL )  reason = rsn_PLL;  // PLL Unlock.

	return reason;
} 

//-----------------------------------------------------------
void Alarm_SD_Fwd( INT8U path4 )
{
	if(Check_PAU_Use(path4)==0) 
		Fwd_Amp_SD[path4] = 0;
	
	// Check Only off-reason upper 4bit
	Fwd_Amp_SD[path4] &= 0xF0; // PD, DIG, PLL만 남음.

	// shutdown reason
	if(Fwd_Amp_SD[path4]) // if by PD, DIG, PLL
	{
		// to Update the Reason
		if(Fwd_Amp_Off_rsn[path4]==0)
		{
			// if No off-reason and count over 50, 
			// then update that reason
			#if 0
			if(Fwd_Cnt_Off[path4]++ >= 5) // 50 -> 5
			#endif
			{
				Fwd_Cnt_Off[path4] = 0;
				Fwd_Amp_Off_rsn[path4] = GetOffReson(Fwd_Amp_SD[path4]);
			}	
		}		
		else // if reason exist, use priority.
		{
			Fwd_Cnt_Off[path4] = 0;
			Fwd_Amp_Off_rsn[path4] = GetOffReson( Fwd_Amp_SD[path4] ); 
		}
	}
	// Now No shutdown reason, then user-off is amp-off-reason
	else
	{
		// clear reason after count 100( == 5[s])
		if(Fwd_Amp_Off_rsn[path4]) 
		{
			#if 0
			if(Fwd_Cnt_Off[path4]++ >= 5) // 100 -> 5
			#endif
			{
				Fwd_Cnt_Off[path4] = 0;
				Fwd_Amp_Off_rsn[path4] = 0;
			}	
		}
		else Fwd_Cnt_Off[path4] = 0;
	}
}
//-----------------------------------------------------------
void Alarm_SD_Rev( INT8U path4 )
{
	INT8U prev_onoff;

	//----------------------------------------
	#ifdef __TYPE__PRU__ 
	//----------------------------------------
	if( Rvs_Amp_SD[path4] & AMP_REV_IN )
	{
		Rvs_Amp_Off_rsn[path4] = rsn_RVS; // Rev 과입력	
		m_PAU_C_Req[path4].Rvs_AMP_En = OFF; // PRU/AAU
		
		//if(path4 == 0)
		//	D_Printf( DEBUG_STDBY, "[RCU] SD_Rev : AMP#%d Off.", path4+1);
	}
	else /////////// 2019.12.20 : if(Rvs_Amp_Off_rsn[path4] == rsn_RVS) // clear reason
	{
		Rvs_Amp_Off_rsn[path4] = 0;
		//D_Printf( DEBUG_STDBY, VT_YELLOW "[RCU] <AMP %d> Rvs_Amp_Off_rsn clear ", path4);
	}
	//----------------------------------------
	#else  
	//----------------------------------------
	// 2019.06.27 : AAU 분리
	if(Rvs_Amp_SD[path4])
	{
		m_PAU_C_Req[path4].Rvs_AMP_En = OFF; // AAU

		if(Rvs_Amp_SD[path4] & AMP_REV_IN)
			Rvs_Amp_Off_rsn[path4] = rsn_RVS; // Rev 과입력	
			
		Rvs_Amp_Off_rsn[path4] = 0xFF; // etc AMP_SD_DIG, AMP_SUB_SW_OFF ??
	}
	else // clear reason
	{
		Rvs_Amp_Off_rsn[path4] = 0;		
		//D_Printf( DEBUG_STDBY, VT_YELLOW "[RCU] <AMP %d> Rvs_Amp_Off_rsn clear ", path4);
	}
	//----------------------------------------
	#endif
	//----------------------------------------
	
	
	// PRU는 하위 먼저 체크 됨. PAU가 스스로 off 하므로.
	// Check Shutdown except RVS-Over-IN
	if((Rvs_Amp_SD[path4] & AMP_REV_IN)==0)
	{
		Rvs_Amp_SD[path4] &= 0xF0;

		if(Rvs_Amp_SD[path4])
		{
			if(Rvs_Amp_Off_rsn[path4]==0)
			{
				if(Rvs_Cnt_Off[path4]++ > 50)
				{
					Rvs_Cnt_Off[path4] = 0;
					Rvs_Amp_Off_rsn[path4] = GetOffReson(Rvs_Amp_SD[path4]);
				}	
			}
			else Rvs_Cnt_Off[path4] = 0;	
		}
		else
		{
			if(Rvs_Amp_Off_rsn[path4])
			{
				if(Rvs_Cnt_Off[path4]++ >= 100)
					Rvs_Amp_Off_rsn[path4] = 0;
			}
			else Rvs_Cnt_Off[path4] = 0;	
		}
	}


	if( Rvs_Amp_Off_rsn[path4] ) 
	{	
		m_PAU_C_Req[path4].Rvs_AMP_En = OFF; // PRU/AAU
		//if(path4 == 0) D_Printf( DEBUG_STDBY, "[RCU] SD_Rev : AMP#%d Off [2].", path4+1);
	}
	#ifdef __TYPE__AAU__
	else if(m_St.Pwr_Amp_Off_Reason_PAU[path4]) // 2019.09.11
	{
		m_PAU_C_Req[path4].Rvs_AMP_En = OFF; // AAU
	}
	#endif
	else 
	{	
		if((IS__Standby_Status(path4) == 0)	&& (AC_Fail_Ctrl_Req[path4] == 0)) // 2020.01.13
		{
			prev_onoff = m_PAU_C_Req[path4].Rvs_AMP_En; // PRU/AAU
		#ifdef __TYPE__PRU__		
			m_PAU_C_Req[path4].Rvs_AMP_En = m_Eep.RVS_Amp_Onoff[path4];		
		#else
			m_Eep.RVS_Amp_Onoff[path4] = m_Eep.Pwr_Amp_En[path4];			
			m_PAU_C_Req[path4].Rvs_AMP_En = m_Eep.Pwr_Amp_En[path4];
		#endif
			if(prev_onoff != m_PAU_C_Req[path4].Rvs_AMP_En)
				D_Printf( DEBUG_STDBY,  VT_MAGENTA "[RCU] SD_Rev : AMP#%d USER (%d).", path4+1, m_PAU_C_Req[path4].Rvs_AMP_En);
		}		
	}	
	/*
	if(path4 == 1) 
		D_Printf( DEBUG_STDBY,  VT_BLUE "[RCU] SD_Rev : AMP#%d, Rvs_Amp_Off_rsn (0x%X), Rvs_Amp_SD (0x%X).", 
			path4+1, 
			Rvs_Amp_Off_rsn[path4], 
			Rvs_Amp_SD[path4]);
	*/		
}
//----------------------------------------------------------

//----------------------------------------------------------------------------------------------
// off Fwd and Rvs
void AMP_All_Off(INT8U sd_rsn) // 2019.12.19 : defined but not used
{	
	INT8U i;	
	
	for( i = 0; i < AMPU_4_MAX; i++) 
	{
		Fwd_Amp_SD[i] |= sd_rsn;
		Rvs_Amp_SD[i] |= sd_rsn;
	}	
}
//----------------------------------------------------------------------------------------------


/*
----------------------------------------------------------
	flag
	AMP_SD_PD
	AMP_SD_DIG
	AMP_SD_LOCAL
	AMP_SD_PAYLOAD
----------------------------------------------------------	
*/

//----------------------------------------------------------
void Rvs_AMP_SD_f_Ctrl( INT8U path4, INT8U sd_rsn, INT8U set )
{
	if( set == f_SET )
		Rvs_Amp_SD[path4]  |= sd_rsn;
	else // clear
		Rvs_Amp_SD[path4]  &= ~sd_rsn;
}
//----------------------------------------------------------
void Fwd_AMP_SD_f_Ctrl( INT8U path4, INT8U sd_rsn, INT8U set )
{
	if( set == f_SET )
		Fwd_Amp_SD[path4] |= sd_rsn;
	else // clear
		Fwd_Amp_SD[path4] &= ~sd_rsn;
}
//----------------------------------------------------------





/*
----------------------------------------------------------
2019.12.23 : Amp Off 원인 표시 수정

--------------------------------------+----------
0x5117 : Disabled Alarm
--------------------------------------+----------
	User에 의하여 Off  	              | User Control
	System Algorithm에 의해 Off       |	System Control
	
--------------------------------------+----------
0x5115 : Shutdown Alarm
--------------------------------------+----------
	TSSI 상한 초과로 인한 SD	      | Overinput
	Over Power로 인한 SD	          | Overpower
	CPRI Unlock으로 인한 SD	          | CPRI
	Clock Unlock으로 인한 SD          |	Clock
	UL 과입력에 의한 SD	              | UL Overinput
	UDA에 의한 SD	                  | UDA
--------------------------------------+----------

off 제어시, off_reason set (Null의 경우에만)
alarm off report 시, off_reason 사용.
alarm on update 시, off_reason 을 string으로 사용하고, off_reason 삭제.
*/
//---------------------------------------------------------
static INT8U Reason_DIS_5117[AMPU_4_MAX] = {0,0,0,0};
static INT8U Reason_SD__5115[AMPU_4_MAX] = {0,0,0,0};

static INT8U Reason_DIS_5117_prev[AMPU_4_MAX] = {0,0,0,0};
static INT8U Reason_SD__5115_prev[AMPU_4_MAX] = {0,0,0,0};
//---------------------------------------------------------
void Set__Reason_Dis_5117(INT8U path4, INT8U reason)
{
	static INT8U reason_prev[AMPU_4_MAX] = {0,0,0,0};
	
#if 1 
	/* 
	disable alarm 상태이고, system_ctrl 상태에서 user 제어 할 일은 없지만, 
	disable normal 상태이고, system_ctrl 상태에서 user 제어 되어야 함.

	혹은 disable alarm is normal 이면, reason overwrite 가능하게 하면??
	*/

	if(reason == Rsn_Dis_User___Ctrl)			
	{		
		if(reason_prev[path4] != reason)
			print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[AMP_%d] reason_Dis_5117 : (%d) -> (%d), [1:User, 2:System]", 
					path4, reason_prev[path4], reason); 
					
		Reason_DIS_5117[path4] = reason;
	}
	else if(Reason_DIS_5117[path4] == 0)
	{		
		if(reason_prev[path4] != reason)
			print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[AMP_%d] reason_Dis_5117 : (%d) -> (%d), [1:User, 2:System]", 
					path4, reason_prev[path4], reason); 
					
		Reason_DIS_5117[path4] = reason;
	}
		
	reason_prev[path4] = reason;
#else	
	if(Reason_DIS_5117[path4] == 0) 
		Reason_DIS_5117[path4] = reason;
#endif
}

void Set__Reason_SD_5115(INT8U path4, INT8U reason)
{
	//static INT8U reason_prev[AMPU_4_MAX] = {0,0,0,0};
	INT8U curr;

	if(IS__SD_Check_Enabled(path4)==0) return;
	
	//if(reason_prev[path4] != reason)
	//	print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[AMP_%d] reason_SD_5115: (%d) -> (%d), ctrl req", 
	//			path4, reason_prev[path4], reason);	
	//reason_prev[path4] = reason;
	
	curr = Reason_SD__5115[path4];
	if((curr == 0)||(curr == Rsn_SD_UL_OverIn_Skip)) // 또는 sd alarm is normal 이면, overwrite 가능
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[AMP_%d] Set__Reason_SD_5115: (%d) -> (%d)", 
				path4, curr, reason);
		
		Reason_SD__5115[path4] = reason;
	}	
}
//---------------------------------------------------------
INT8U Get__Reason_Dis_5117(INT8U path4)
{
	if(Reason_DIS_5117[path4] > 0)
		return Reason_DIS_5117[path4];
	else
		return Reason_DIS_5117_prev[path4];
}

INT8U Get__Reason_SD_5115(INT8U path4)
{
	if(Reason_SD__5115[path4] > 0)
		return Reason_SD__5115[path4];
	else
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[AMP_%d] Get__Reason_SD_5115 --- previous", path4);		
		return Reason_SD__5115_prev[path4];
	}
}

INT8U Get__Reason_Dis_5117_Ori(INT8U path4)
{
	return Reason_DIS_5117[path4];
}


INT8U Get__Reason_SD_5115_Ori(INT8U path4)
{
	return Reason_SD__5115[path4];
}

//---------------------------------------------------------
void Clr__Reason_Dis_5117(INT8U path4)
{
	if(Reason_DIS_5117[path4] > 0)
	{
		Reason_DIS_5117_prev[path4] = Reason_DIS_5117[path4];
		Reason_DIS_5117[path4] = 0;	
	}
}
void Clr__Reason_SD_5115(INT8U path4)
{
	if(Reason_SD__5115[path4] > 0)
	{
		Reason_SD__5115_prev[path4] = Reason_SD__5115[path4];
		Reason_SD__5115[path4] = 0;	
	}
}

void Clr__Reason_Dis_5117_Prev(INT8U path4)
{
	Reason_DIS_5117_prev[path4] = 0;
}

void Clr__Reason_SD_5115_Prev(INT8U path4)
{
	Reason_SD__5115_prev[path4] = 0;
}
//---------------------------------------------------------
char* Get__Str_Dis_5117(INT8U reason)
{
	switch(reason)
	{
		case Rsn_Dis_User___Ctrl: return "User Control";
		case Rsn_Dis_System_Ctrl: return "System Control";
		
		default : return "Rsn_Dis_";
	}
}
char* Get__Str_SD_5115(INT8U reason)
{
	switch(reason)
	{
		case Rsn_SD__HighTemp: return "HighTemp";
		case Rsn_SD__by__VSWR: return "VSWR";
		case Rsn_SD_____by_DC: return "DC";
		case Rsn_SD_Over__Pwr: return "OverPower";
		case Rsn_SD_CPRI_Fail: return "CPRI";
		case Rsn_SD_CLock_Alm: return "Clock";
		case Rsn_SD_TSSI_Over: return "DL TSSI 상한"; // 2019.12.24
		case Rsn_SD_UL_OverIn: return "UL 과입력"; // 2019.12.24
		case Rsn_SD____by_UDA: return "UDA";
		
		default : return "Rsn_SD__"; // maybe 0
	}
}
//---------------------------------------------------------





///////////////////////////////////////////////////////////////////////////////////////
//
//  AMP Standby
//
///////////////////////////////////////////////////////////////////////////////////////

/*
----------------------------------------------------
	When User-Amp Control, 
	check AMP is in the AMP_Standby_Mode or not

	Sum of Amp_standby_mode-AMPs
----------------------------------------------------	
*/
INT8U Get_Amp_stdby_st_Sum(void)
{	
	INT8U path4;
	INT8U stdbySt = 0; // init 0

	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{		
		if(Amp_Stdby_Use(path4) && Check_PAU_Use(path4))
		{
			stdbySt |= m_St.AMP_Stdby_St[path4];
			stdbySt |= m_St.AMP_Stdby_St_Dly_REQ[path4]; // DPD Stablization
		}	
	}	
	
	if(stdbySt) m_St.Sum_AMP_Stdby_St = ON;
	else        m_St.Sum_AMP_Stdby_St = OFF;

	return stdbySt;
}

/*
----------------------------------------------------
	Check amp-standby-use or not
	
	retur : 1(USE), 0(Not-Use)
----------------------------------------------------
*/
INT8U Amp_Stdby_Use(INT8U path4)
{
	INT8U amp_stdby_path;

	if(path4 >= AMPU_4_MAX) return 0; // not used
		
	amp_stdby_path = m_St.AMP_Stdby_Path ^ 0xFF; // toggle, 0 is use, 1 is not used
	
	return (amp_stdby_path & cbits[path4]);
}

/*
----------------------------------------------------
when digigal alarm,
skip Amp-Standby
----------------------------------------------------
*/
// AMP standby mode check 안된 amp 들이 amp alarm 인지 ?



void Chk_Skip_AMP_Stdby(void)
{
	INT8U dig_alm = 0;

	Get_Amp_stdby_st_Sum();
	
	if(m_St.Sum_AMP_Stdby_St == ON) // if standby status
	{
		Is_Skip_AMP_Std = 0;
	}
	else
	{
		if(Is_PD_Alarm_AMP_Std()) dig_alm = 1;
		else if(IS_Dig_Alarm())   dig_alm = 1;
		else                      dig_alm = 0;

		if(dig_alm)
		{
			Is_Skip_AMP_Std = 1;
			OSTimerReset( &AMP_Stdby_Skip_ms );
		}
		else
		{
			if(Is_Skip_AMP_Std)
			{
				if(OSTimerGet_msec(&AMP_Stdby_Skip_ms) > 10000)
				{
					Is_Skip_AMP_Std = 0;
					OSTimerReset( &Amp_Standby_Timer_ms );
				}
			}
		}
	}
}


INT8U Is_Not_Amp_Stdby_Use(INT8U path4)
{
	INT8U use;

	use = Amp_Stdby_Use(path4);

	return (!use);	
}



INT8U Is_AMP_Rvs_Off_Alarm( INT8U path4) // @ 5G
{	
	return (m_St.RVS_Amp_Onoff[path4]);
}

INT8U Is_AMP_Alm_AMP_STD( INT8U path4 )
{
	if( m_St.AlmSt.Path[path4].PAU.Over_Power ) return 1;
	if( m_St.AlmSt.Path[path4].PAU.High_Temp ) return 1;
	if( m_St.AlmSt.Path[path4].PAU.VSWR ) return 1;
	if( m_St.AlmSt.Path[path4].PAU.Disabled ) return 1;	
	if( m_St.AlmSt.Path[path4].PAU.Shutdown ) return 1;	
	if( m_St.AlmSt.Path[path4].PAU.DC_Fail ) return 1;
	if( m_St.AlmSt.Path[path4].PAU.Dev_Fail ) return 1;		
	if( m_St.AlmSt.Path[path4].DPD.DPD_Alm ) return 1;
	if( m_St.AlmSt.Path[path4].UL.Over_Input ) return 1;
	if( m_St.AlmSt.Path[path4].UL.LNA_Fail ) return 1;
	if( m_St.Pwr_Amp_Link_Err[path4] ) return 1;	
	
	if( Is_AMP_Rvs_Off_Alarm(path4) ) return 1;

	return 0;
}



/*
----------------------------------------------------------------
2020.01.13 : UL.Over.In algorithm 등으로 shutdown 상태에서,
바록 AMP-Stdby 들어가면, SD alarm 이 그대로 유지 된 채로 
AMP-Stdby 동작됨.
(MIMO Stdby는 10초 시간으로 인해, SD alarm 이 해제 되는 것이어서, 같은 동작 방식으로 되도록)

SD Alarm 상테애서는 AMP-Stdby를 동작시키지 않고,
또 이전에 SD Alarm 이었다가 정상이 되면, 
10초 정도 지연시간을 주어서 
SD alarm 이 해제 되고, AMP-Stdby 동작되게 하기 위해 아래 
를 Is_AMP_SD_Alm_St()추가.

return 
1 : sd alarm 혹은 지연 중, AMP-Stdby 동작 불가.
0 : AMP-Stdby 동작 가능.
----------------------------------------------------------------
*/
static OS_TIMER SD_Alm_Dly_ms[AMPU_4_MAX];
INT8U Is_AMP_SD_Alm_St(INT8U path4)
{
	INT8S is_sd_alarm_st = 0;
	static INT8U alm_prev[AMPU_4_MAX] = {0,0,0,0};
	
	if(m_St.AlmSt.Path[path4].PAU.Shutdown) 
	{
		is_sd_alarm_st = 1;
		
		alm_prev[path4] = 1;
		OSTimerReset(&SD_Alm_Dly_ms[path4]);
	}
	else if(alm_prev[path4])
	{
		if(OSTimerGet_msec(&SD_Alm_Dly_ms[path4]) < (10*TICK_CO_SEC))
		{
			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP#%d-Stdby] SD_Alarm Delay End", path4+1);
			alm_prev[path4] = 0;
			OSTimerReset(&SD_Alm_Dly_ms[path4]);
			is_sd_alarm_st = 1;
		}
		else is_sd_alarm_st = 0;
	} 
	else is_sd_alarm_st = 0;

	return is_sd_alarm_st;
}





/*
----------------------------------------------
Not-Checked-AMP have alarms,
return 1(alarm), 
for not-going-to "standby mode"

"amp-std-미사용" amp가 모두 알람이 없어야 
나머지 "amp-std-사용" amp의 amp standby 동작이 가능.


"amp-std-미사용" amp 중, 1개라도 알람이 있으면, 
1을 리턴하며, 
"amp-std-미사용" amp 모두 알람이 없으면, 
0을 리턴.
----------------------------------------------
*/
INT8U IS___AmpStd_UnChk_AMP___Alarm(void)
{
#if 1 // @5G, later used
	return 0; // all amp normal
#endif

	INT8U Alm = 0; // default 0
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Is_Not_Amp_Stdby_Use(path4) && Check_PAU_Use(path4))
		{
			if(Is_AMP_Alm_AMP_STD(path4)) Alm |= cbits[path4];
		}
	}	
	return Alm;		
}

/*
---------------------------------------------------------
2020.01.13
1. Standby 종료시, Amp-onoff 제어에 대한 설정.
	기존 : User-off control 했던 상태대로 복귀.
	수정 : 
	(1) User-off control 했던 상태라면, Amp off 
	(2) Shutdown alarm 발생되었다면, Amp Off.
---------------------------------------------------------
*/

INT8U Make_Stdby_Bk_AMP_St(INT8U path4)
{
	INT8U bk_amp_st;
	
	if(path4 >= AMPU_4_MAX) return 0;

	if(m_Eep.Pwr_Amp_En[path4] == OFF)
		bk_amp_st = OFF;
	else if(m_St.Last_Alm.Path[path4].PAU.Shutdown)
		bk_amp_st = OFF;
	else
		bk_amp_st = ON;

	return bk_amp_st;
}

/*
----------------------------------------------------------------------------
	Restore values 
	which User has Controlled previously
----------------------------------------------------------------------------
*/
void AMP_User_Ctrl_Restore(void)
{	
	INT8U path4;	
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Amp_Stdby_Use(path4) && Check_PAU_Use(path4))
		{
			AMP_Stby_Bk_AMP_St[path4] = Make_Stdby_Bk_AMP_St(path4);
			AMP_Stby_Bk_RVS_AMP_St[path4] = m_St.RVS_Amp_Onoff[path4]; // but not used...
		}
	}
}
//-------------------------------------------------------------------------------------------------
// Amp Standby mode 동작 시간이면 1, 아니면 0 리턴.
// start,stop time이 바뀐 경우, 시간 save하고, 2리턴.
INT8U Is_Amp_Std_Mode_Time(void)
{
	INT16U start16, stop16, cur16;
	static INT16U start_prev, stop_prev;

	//-----------------------------	
	if(IS_RU_Init_Completed()==0) 
	{	
		return 0;
	}
	//-----------------------------	


	// cur time
	cur16 = m_St.Ru_Sys_Date[3]; cur16 <<= 8; 
	cur16 += m_St.Ru_Sys_Date[4];
	
	// start time
	start16 = m_St.AMP_Stdby_Start_Time[0]; start16 <<= 8;
	start16 += m_St.AMP_Stdby_Start_Time[1];
	
	// stop time
	stop16 = m_St.AMP_Stdby_Stop_Time[0]; stop16 <<= 8;
	stop16 += m_St.AMP_Stdby_Stop_Time[1];

	//---------------------------------------------
	if((start_prev != start16)||(stop_prev != stop16))
	{
		start_prev = start16;
		stop_prev = stop16;

		return 2; // start or stop is changed by User-Control
	}
	start_prev = start16;
	stop_prev = stop16;
	//---------------------------------------------

	// Amp Stdby time
	if(start16 <= stop16)
	{
		if((cur16 >= start16)&&(cur16 < stop16)) 
			return 1;
		else return 0;
	}
	else
	{
		if((cur16 >= stop16)&&(cur16 < start16)) 
			return 0;
		else return 1;
	}
}

//----------------------------------------------------------------------------------------------
// AMP Stdby 시간이고, User가 AMP Stdby Mode On 제어한 경우이면, 
// AMP를 사용자 제어 못하게 함.
void AMP_Stdby_AMP_Ctrl_Chk(void)
{
	if((m_St.AMP_Stdby_Enable == ON) && Is_Amp_Std_Mode_Time() )
	{
		AMP_Stdby_Mode_St_Sum = ON; // amp ctrl disable
	}
	else AMP_Stdby_Mode_St_Sum = OFF; // amp ctrl enable
}

INT8U IS___AmpStd_St_AMP(INT8U path4)
{
	if( Check_PAU_Use( path4 ) && Amp_Stdby_Use( path4 )) 
	{
		if(m_St.AMP_Stdby_St[path4]) return 1;
	}
	
	return 0;		
}
//---------------------------------------------------------

/*
슈퍼캐패시터 동작으로 AC 알람 발생시 다른 알람들은 Mask 처리
알람 Mask 항목 : 
	Clock Fail, TSSI 상한/하한, SFP LD,LOS 알람, CPRI Fail 알람, PAU 알람(Disable 포함)

1) AC Fail, super C로 인한 AC Fail ? 구분 ??
2) AC fail 이면, 해당 alarm update 안함 ==> alarm 상위 보고 X
3) GUI 에 보이는 Last alarm 만 0으로 clear : 기존 last 알람 중, 해당하는 부분만 mask 해서 보여줌.
AC Fail 일때만..
*/

// ---- Not Used
/*
----------------------------------------------------------------------------

----------------------------------------------------------------------------
*/



//----------------------------------------------------------------------------
// when amp-standby-start, backup(save) current alarm
void Backup_Cur_Alarm_St( INT8U path4 )
{	
	if(path4 >= AMPU_4_MAX) 
		return;

	m_St.Stdby_Bk_Alm.Path[path4].PAU.High_Temp = m_St.AlmSt.Path[path4].PAU.High_Temp;
	m_St.Stdby_Bk_Alm.Path[path4].PAU.VSWR      = m_St.AlmSt.Path[path4].PAU.VSWR;
	m_St.Stdby_Bk_Alm.Path[path4].PAU.Dev_Fail  = m_St.AlmSt.Path[path4].PAU.Dev_Fail;
	m_St.Stdby_Bk_Alm.Path[path4].DPD.DPD_Alm   = m_St.AlmSt.Path[path4].DPD.DPD_Alm;	

	m_St.Stdby_Bk_Alm.Path[path4].PAU.Shutdown   = m_St.AlmSt.Path[path4].PAU.Shutdown;
	m_St.Stdby_Bk_Alm.Path[path4].PAU.Disabled   = m_St.AlmSt.Path[path4].PAU.Disabled;
	m_St.Stdby_Bk_Alm.Path[path4].PAU.Over_Power = m_St.AlmSt.Path[path4].PAU.Over_Power;
	m_St.Stdby_Bk_Alm.Path[path4].PAU.DC_Fail    = m_St.AlmSt.Path[path4].PAU.DC_Fail;

	m_St.Stdby_Bk_Alm.Path[path4].DL.Out_Lower    = m_St.AlmSt.Path[path4].DL.Out_Lower;
	m_St.Stdby_Bk_Alm.Path[path4].DL.Out_Upper    = m_St.AlmSt.Path[path4].DL.Out_Upper;
	
	m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Lower_a = m_St.AlmSt.Path[path4].DL.TSSI_Lower_a;
	m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Lower__ = m_St.AlmSt.Path[path4].DL.TSSI_Lower__;

	m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Upper_a = m_St.AlmSt.Path[path4].DL.TSSI_Upper_a;
	m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Upper__ = m_St.AlmSt.Path[path4].DL.TSSI_Upper__;

	m_St.Stdby_Bk_Alm.Path[path4].UL.RSSI_Lower_a = m_St.AlmSt.Path[path4].UL.RSSI_Lower_a;
	m_St.Stdby_Bk_Alm.Path[path4].UL.RSSI_Lower__ = m_St.AlmSt.Path[path4].UL.RSSI_Lower__;	
}

//-------------------------------------------------
// when amp-standby 동작 중일때, "Stdby_Bk_Alm" alarm 을 현재 alarm 으로 보정함.
// Amp_Stdby_St 일때 기존 알람 유지.
void AMP_Stby_Amp_Alarm_Adj(ALM_ST* pLast_Alm, INT8U path4)
{	
	if(path4 >= AMPU_4_MAX) 
		return;
	
	pLast_Alm->Path[path4].PAU.High_Temp = m_St.Stdby_Bk_Alm.Path[path4].PAU.High_Temp;
	pLast_Alm->Path[path4].PAU.VSWR      = m_St.Stdby_Bk_Alm.Path[path4].PAU.VSWR;
	pLast_Alm->Path[path4].PAU.Dev_Fail  = m_St.Stdby_Bk_Alm.Path[path4].PAU.Dev_Fail;
	pLast_Alm->Path[path4].DPD.DPD_Alm   = m_St.Stdby_Bk_Alm.Path[path4].DPD.DPD_Alm;

	pLast_Alm->Path[path4].PAU.Shutdown   = m_St.Stdby_Bk_Alm.Path[path4].PAU.Shutdown;
	pLast_Alm->Path[path4].PAU.Disabled   = m_St.Stdby_Bk_Alm.Path[path4].PAU.Disabled;
	pLast_Alm->Path[path4].PAU.Over_Power = m_St.Stdby_Bk_Alm.Path[path4].PAU.Over_Power;
	pLast_Alm->Path[path4].PAU.DC_Fail    = m_St.Stdby_Bk_Alm.Path[path4].PAU.DC_Fail;
	
	pLast_Alm->Path[path4].DL.Out_Lower   = m_St.Stdby_Bk_Alm.Path[path4].DL.Out_Lower;
	pLast_Alm->Path[path4].DL.Out_Upper   = m_St.Stdby_Bk_Alm.Path[path4].DL.Out_Upper;
	
	pLast_Alm->Path[path4].DL.TSSI_Lower_a = m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Lower_a;
	pLast_Alm->Path[path4].DL.TSSI_Lower__ = m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Lower__;

	pLast_Alm->Path[path4].DL.TSSI_Upper_a = m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Upper_a;
	pLast_Alm->Path[path4].DL.TSSI_Upper__ = m_St.Stdby_Bk_Alm.Path[path4].DL.TSSI_Upper__;

	pLast_Alm->Path[path4].UL.RSSI_Lower_a = m_St.Stdby_Bk_Alm.Path[path4].UL.RSSI_Lower_a;
	pLast_Alm->Path[path4].UL.RSSI_Lower__ = m_St.Stdby_Bk_Alm.Path[path4].UL.RSSI_Lower__;	
}
//-------------------------------------------------


//------------------------------------------------------------------------


	

//------------------------------------------------------------------------



//------------------------------------------------------------------------
static OS_TIMER Amp_Std_Exit_Dly_Timer[AMPU_4_MAX];

/*
------------------------------------------------------------------------
When AmpStandby Exit, 
Delay
------------------------------------------------------------------------
*/
void Chk_End_of_AMP_Standby_Exit_Dly(void)
{
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{				
		if(m_St.AMP_Stdby_St_Dly_REQ[path4])
		{
			if(OSTimerGet_msec(&Amp_Std_Exit_Dly_Timer[path4] ) > (10*TICK_CO_SEC))
			{
				m_St.AMP_Stdby_St_Dly_REQ[path4] = 0;
			}
		}
	}	
}
/*
------------------------------------------------------------------------
All path Exit from AMP-STD
------------------------------------------------------------------------
*/
void Amp_Stdby_Mode_Exit(void)
{	
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		// dly request 먼저.	
		if(IS___AmpStd_St_AMP(path4))
		{
			m_St.AMP_Stdby_St_Dly_REQ[path4] = 1;			
			OSTimerReset( &Amp_Std_Exit_Dly_Timer[path4] );
		}
		m_St.AMP_Stdby_St[path4] = 0; // all path Exit from AMP-STD
	}	
}

/*
standby mode 동작 중, exit(빠져 나올지) 체크 
--------------------------------------------
	1. UnCheck-AMP-Standby AMP 모두가 alarm 없으면, exit 없이 standby mode 동작을 계속함.
	2. UnCheck-AMP-Standby AMP 하나라도 alarm 있으면, standby mode 를 강제로 빠져 나옴.
	(단, PD alarm이나, digital alarm 있으면, 제외, 즉, exit 없이 standby mode 동작을 계속함.)
*/
INT8U IS__AMP_Std_Exit_Possible(void)
{
//	INT8U alarm;
	INT8U exit_possible;
	
#if 1 // @5G, later used
	exit_possible = 0; // 0 : not exit
//	alarm = 0; // 0
#else

	alarm = IS___AmpStd_UnChk_AMP___Alarm();

	/*
		UnCheck-AMP-Standby AMP가 alarm 인데, 
		pd alarm, dig alarm 만 아니면, 
		
		Check-AMP-Standby AMP가 
		Amp-Standby에서 Exit 가능.
	*/
	if(alarm)
	{
		if(Is_PD_Alarm_AMP_Std()) exit_possible = 0;
		else if(IS_Dig_Alarm())   exit_possible = 0;
		else                      exit_possible = 1;
	}
	else exit_possible = 0; ////// ---- current -----
	
	if(exit_possible)
	{
		D_Printf (DEBUG_STDBY, "[AMP-Stdby]] Exit(0x%02X):0x%02X, %d\r\n",\
			alarm, m_St.SFP_Ins, m_St.AlmSt.UDA.PD_CPRI);
	}
#endif
	return exit_possible;		
}



//------------------------------------------------------------------------
//

//------------------------------------------------------------------------

void Init_AMP_Standby_Mode_Proc(void)
{
	OSTimerReset( &Amp_Standby_Timer_ms );
	OSTimerReset( &AMP_PROC_Start_Timer_ms );	
	AMP_Stdby_Mode_St_Sum = OFF; // default OFF
}

/*
------------------------------------------------------------------------

	AMP Standby Mode
	Algorithm

------------------------------------------------------------------------
*/

static INT8U AMP_Standby_Start_En = 0; // init not start, after 60[s], AmpStandby Start.

//#define AMP_Stdby_Start_Dly_sec (60 * 1000) // 60[s] @ IRO
#define AMP_Stdby_Start_Dly_sec (6 * 1000) // 60[s] @ IRO


/*
---------------------------------------------------------
추가 구현 할 부분.

1. 	algorithm 동작 시, amp-off alarm, low alarm mask
2.  동시 발생 우선 순위.
 	Battery Extension > AMP-Standby
---------------------------------------------------------



*/
INT8U Proc_AMP_Standby( void )
{
	INT8U f_In_Time;
	INT8U path4;
	INT16U start16, stop16;
	static INT16U start16_prev = 0;
	static INT16U stop16__prev = 0;
	static INT8U  init = 0;
	static INT8U  prev_st = 0;
	static INT8U  skip_cnt = 0;

	if(init == 0)
	{
		prev_st = m_Eep.AMP_Stdby_Path;
		init = 1;
	}

	//---------------------------------------------------------
	// When aftger RCU-Reset, 
	// AMP-Standby Start after 60[s]
	//---------------------------------------------------------
	if(AMP_Standby_Start_En == 0)
	{
		elapse_time32 = OSTimerGet_msec(&AMP_PROC_Start_Timer_ms);		
		if( elapse_time32 >= AMP_Stdby_Start_Dly_sec )
		{
			AMP_Standby_Start_En = 1;
			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Stdby] AMP Standby Algorithm");
		}
		return 0;
	}
	//---------------------------------------------------------
	if( OSTimerGet_msec(&Amp_Standby_Timer_ms) < 500 ) return 0; // every 500[ms]
	OSTimerReset( &Amp_Standby_Timer_ms );
	//---------------------------------------------------------	

	Chk_Skip_AMP_Stdby(); // 현재 미사용
	Chk_End_of_AMP_Standby_Exit_Dly(); // 10[s]
	
	//---------------------------------------------------------
	// DPD Stablization
	// AmpStandby mode off or amp standby time off 될 때, 
	// 당시의 PAU AMP oFF 상태의 알람이 발생해도,
	// PAU AMP oN 명령 보내기 전에, off 알람이 올라가지 않도록.
	if(m_Env.ALL_AMP_Standby_Exit_Req)
	{
		if(OSTimerGet_msec(&m_Env.AMP_Standby_Exit_Timer) >= 10000) // 10[s]
		{		 
			m_Env.ALL_AMP_Standby_Exit_Req = 0;
				
			for(path4 = 0; path4 < AMPU_4_MAX; path4++)
				m_St.AMP_Stdby_St_Dly_REQ[path4] = 0;

			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Stdby] Dly_REQ All End --");
		}		
		return 1; // 바로 amp stdby mode 동작하지 않도록 return (end)
	}

	// 2019.05.25
	//---------------------------------------------------------
	start16 = m_St.AMP_Stdby_Start_Time[0]; start16 <<= 8;
	start16 += m_St.AMP_Stdby_Start_Time[1];

	stop16 = m_St.AMP_Stdby_Stop_Time[0]; stop16 <<= 8;
	stop16 += m_St.AMP_Stdby_Stop_Time[1];
	//---------------------------------------------------------
	// 동작 중, 설정 변경시 amp standby off 되게 해서,
	// user가 다시 설정하도록 .
	if(m_St.Sum_AMP_Stdby_St == ON)
	{
		if((prev_st != m_Eep.AMP_Stdby_Path)||(start16_prev != start16)||(stop16__prev != stop16))
		{
			m_St.AMP_Stdby_Enable = OFF;
			m_Eep.AMP_Stdby_Mode_Onoff = OFF;
			
			m_Env.is_eep_write = EEP_WRITE_CODE;

			start16_prev = start16;
			stop16__prev = stop16;
			prev_st = m_Eep.AMP_Stdby_Path;

			return 1;
		}		
	}	
	//---------------------------------------------------------
	start16_prev = start16;
	stop16__prev = stop16;
	prev_st = m_Eep.AMP_Stdby_Path;
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	// if Mode : ON --> Off
	//---------------------------------------------------------	
	if(m_St.AMP_Stdby_Enable == OFF) // user-off or when-AC-Fail or user-mimo-stdby-ON
	{		
		if(AMP_Stdby_En_Prev != m_St.AMP_Stdby_Enable) // On -> Off, 1st time.
		{
			AMP_Stdby_En_Prev = m_St.AMP_Stdby_Enable;

			Amp_Stdby_Mode_Exit();
			AMP_User_Ctrl_Restore();
			
			m_Env.ALL_AMP_Standby_Exit_Req = 1;
			OSTimerReset( &m_Env.AMP_Standby_Exit_Timer );
			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Stdby] Exit(by Mode Off)");

			/*
				in time 중에 off되었다가 다시 on 되어도, 
				초기루틴이 돌수 있도록.			
			*/
			f_Amp_In_Time_Prev = 0; 
		}		
		return 1;
	}

	//---------------------------------------------------------
	// Now AMP Stdby mode is ON	
	AMP_Stdby_En_Prev = m_St.AMP_Stdby_Enable;
	f_In_Time = Is_Amp_Std_Mode_Time();
	
	if(m_Env.AMP_Ctrl_Allowed==0) // 2020.01.14
	{
		f_In_Time = 0;

		if(skip_cnt >= 10) skip_cnt = 0;
		if(skip_cnt == 0) print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[AMP-Stdby] But Under RU.Init, Skipping AMP-Stdby.");
		skip_cnt++;
	}
	else skip_cnt = 0;
	//---------------------------------------------------------		
	if(Is_AC_Fail()) 
	{		
		return 0;	
	}
	//---------------------------------------------------------
	if(f_In_Time == 2) // Test mode or User-Changed Start/Stop Time	
	{
		D_Printf( DEBUG_STDBY, "[AMP-Stdby] f_In_Time == 2\n");		
		return 0; 
	}
	else if(f_In_Time==0) // Out-Of-Time, Exit AMP-Standby
	{		
		Amp_Stdby_Mode_Exit();
		
		// Restore Value is previous User-Control-Value
		
		if(f_Amp_In_Time_Prev != f_In_Time) // 1st Exit
		{			
			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP-Stdby] On -> Off ");
			
			AMP_User_Ctrl_Restore();
			m_Env.ALL_AMP_Standby_Exit_Req = 1;
			OSTimerReset( &m_Env.AMP_Standby_Exit_Timer );
			print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Stdby] Exit (by Time Off)");
		}
	}
	else // In the AMP-Standby_Mode Time
	{
		if(f_Amp_In_Time_Prev != f_In_Time) // AMP-Standby-Mode : Off ---> On.
		{
			D_Printf( DEBUG_STDBY, "[AMP-Stdby] Off -> On.");	
			
			Amp_Stdby_Mode_Exit(); // Initialize
			AMP_User_Ctrl_Restore();			
			m_Env.ALL_AMP_Standby_Exit_Req = 0;
		}
		//----------------------------------------
		if(Is_Skip_AMP_Std)
		{
			OSTimerReset(&AMP_PROC_Start_Timer_ms);

			D_Printf( DEBUG_STDBY, "[AMP-Stdby] Skip AMP-Stdby.");	
			return 1; // do not procede...
		}		
		//----------------------------------------		
		if(IS__AMP_Std_Exit_Possible()) // 강제-exit ??
		{
			if(m_Env.ALL_AMP_Standby_Exit_Req == 0)
			{
				Amp_Stdby_Mode_Exit();
				AMP_User_Ctrl_Restore(); // 사용자 제어 값.
				m_Env.ALL_AMP_Standby_Exit_Req = 1;
				OSTimerReset( &m_Env.AMP_Standby_Exit_Timer );
				
				print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Stdby] Exit (by alarm check)");
			}
			f_Amp_In_Time_Prev = f_In_Time;	
			return 1;
		}		
		
		/*
			알고리즘 동작 시(Mode_ON && Time_In), 
			User AMP On/Off 제어 안됨.
		*/
		//-----------------------------------------------------
	
		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		{
			if(Check_PAU_Use(path4) && Amp_Stdby_Use(path4))
			{
				// 2020.01.13
				if(Is_AMP_SD_Alm_St(path4))
					continue;
				
				// "amp-std-미사용" amp가 모두 알람이 없어야 
				// 나머지 "amp-std-사용" amp의 amp standby 동작이 가능.
				if(IS___AmpStd_UnChk_AMP___Alarm() == 0) // if all-not-chk-amp are normal
				{
					if(m_St.AMP_Stdby_St[path4] == 0)
					{
						Backup_Cur_Alarm_St( path4 );
						
						print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP-Stdby] AMP#%d bkup %d\n",
							path4+1, 
							m_St.AlmSt.Path[path4].PAU.Disabled);
					}
					
					m_St.AMP_Stdby_St[path4] = 1; // Enter into AMP-STD
					
					if(AMP_Alarm_St(path4) == ON) // AMP is ON
					{
						Amp_StdBy_PAU_OFF_Req(path4, 'A');
						print_agent(DEBUG_STDBY, VT_BOLD_WHITE, "[AMP_Stdby] AMP#%d Off", path4+1);
					}
				}		
			}
			else 
			{
				if(m_St.AMP_Stdby_St[path4])
				{
					// path4 Exit from AMP-STD
					m_St.AMP_Stdby_St[path4] = 0; 
					m_St.AMP_Stdby_St_Dly_REQ[path4] = 1;
					OSTimerReset(&Amp_Std_Exit_Dly_Timer[path4]);
				}
			}
		}
		//-----------------------------------------------------
	}
	f_Amp_In_Time_Prev = f_In_Time;	
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//  END Of AMP Standby
//
///////////////////////////////////////////////////////////////////////////////////////









///////////////////////////////////////////////////////////////////////////////////////
//
//  MIMO Standby 
//  Algorithm
//
///////////////////////////////////////////////////////////////////////////////////////

INT8U Is_MIMO_Mode_Time(void)
{
	static INT16U start_prev, stop_prev;
	INT16U start16, stop16, cur16;	
	INT8U ret_2;	
	INT8U in_out;
	
	
	//-----------------------------
	if(IS_RU_Init_Completed()==0) 
		return 0;
	//----------------------------- 

	cur16 = m_St.Ru_Sys_Date[3]; cur16 <<= 8;
	cur16 += m_St.Ru_Sys_Date[4];

	start16 = m_St.MiMo_Stdby_Start_Time[0]; start16 <<= 8;
	start16 += m_St.MiMo_Stdby_Start_Time[1];

	stop16 = m_St.MiMo_Stdby_Stop_Time[0]; stop16 <<= 8;
	stop16 += m_St.MiMo_Stdby_Stop_Time[1];

	if(start16 <= stop16)
	{
		if((cur16 >= start16) && (cur16 < stop16)) 
			in_out = 1;
		else in_out = 0;
	}
	else
	{
		if((cur16 >= stop16) && (cur16 < start16)) 
			in_out = 0;
		else in_out = 1;
	}

	if(in_out == 0) return 0;
	else
	{
		if(start_prev != start16) ret_2 = 1;
		else if(stop_prev != stop16) ret_2 = 1;		
		else ret_2 = 0;		

		start_prev = start16;
		stop_prev = stop16;

		if(ret_2) return 2;
		else      return 1;
	}
}





void Mimo_Stdby_AMP_Ctrl_Chk(void)
{
	if((m_St.MIMO_Stdby_Enable == OFF)||(Is_MIMO_Mode_Time() == 0))
		 Mimo_Stdby_Mode_St = OFF; // amp ctrl enable
	else Mimo_Stdby_Mode_St = ON;  // amp ctrl disable
}

static OS_TIMER MIMO_Standby_Timer_ms;

void Init_MIMO_Std_Mode(void)
{
	OSTimerReset( &MIMO_Standby_Timer_ms );	
	Mimo_Stdby_Mode_St = OFF; // default OFF
}


#define amp_on_timer_timeout_ms (10000) // 20s -> 10s

static OS_TIMER Mimo_Std_Exit_Dly_Timer[AMPU_4_MAX];

//------------------------------------------------------------------------
void Mimo_Standby_Exit_Dly_REQ( INT8U path4 )
{
	m_St.Mimo_Stdby_St_Dly_REQ[path4] = 1;
	OSTimerReset( &Mimo_Std_Exit_Dly_Timer[path4] );
}

void Mimo_Standby_AMP_Off_flag(INT8U path4, INT8U set_clr)
{
	m_St.Mimo_Stdby_AMP_Off_St[path4] = set_clr;
}

INT8U IS__Mimo_Standby_AMP_Off_flag(INT8U path4)
{
	return m_St.Mimo_Stdby_AMP_Off_St[path4];
}
//------------------------------------------------------------------------
void Chk_End_of_Mimo_Standby_Exit_Dly(void)
{
	INT8U path4;

	for (path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(m_St.Mimo_Stdby_St_Dly_REQ[path4])
		{			
			if(OSTimerGet_msec(&Mimo_Std_Exit_Dly_Timer[path4]) > (10*TICK_CO_SEC))
			{
				m_St.Mimo_Stdby_St_Dly_REQ[path4] = 0;
				Mimo_Standby_AMP_Off_flag(path4, _CLR_);
			}
		}
	}
}



//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------




//------------------------------------------------------------------
void Set_MIMO_Standby_St(INT8U path4, INT8U set_clr)
{
	INT8U i;
	INT8U sum_st;

	if(path4 >= AMPU_4_MAX) return;
	
	m_St.MIMO_Stdby_St[path4] = set_clr;
	sum_st = _CLR_; // init

	//if(set_clr == _CLR_)
	//	D_Printf(DEBUG_STDBY, VT_BLINK_GREEN "[MM-Stdby] MIMO_Standby %d CLR...", path4);
	
	for(i=0; i<AMPU_4_MAX; i++)
	{
		if(m_St.MIMO_Stdby_St[i]) sum_st = _SET_;
	}
	
	if(sum_st == _SET_) m_St.Sum_MIMO_Stdby_St = ON;
	else                m_St.Sum_MIMO_Stdby_St = OFF;
}


/*
------------------------------------------------------------------
	AMP-Standby, 
	MiMo-Standby
------------------------------------------------------------------	
*/
INT8U StandBy_PAU_EN_Ctrl(INT8U path4, INT8U en, char from)
{
	INT8U cmd = 0;
///// YCSO block 190516
//	if( Is_SIM_OnGoing() ) return 0;	

#ifdef __TYPE__PRU__ // ANTS-PRU
	if( en == OFF ) cmd = AMP_CMD_DIS;
	else            cmd = AMP_CMD_EN;

	// 2019.04.25.: below changed
	//if(AMP_Stby_Off_Req[path4]) // AMP-Standby Only.(MIMO-Standby (X))
	// 2019.08.13 : delete "if(AMP_Stby_Off_Req[path4])"
	{
		if(en == OFF) StdBy_Off_rsn[path4] = _SET_;
		else          StdBy_Off_rsn[path4] = _CLR_;
	}
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
		
	Check_DPD_DCL_Reset(path4, en);
	cmd = PAU_Send_Data(path4, cmd, NULL, 0);	
	//----------------------------------
#else // ANTS-AAU

	if(en == OFF) // 2019.12.20
	{
		Set__Reason_Dis_5117(path4, Rsn_Dis_System_Ctrl);
	}
	
	// 2019.04.22
	AAU__RF_AMP_OnOff(path4, en, 1148);	
	m_PAU_C_Req[path4].Rvs_AMP_En = en & 0x01;
	
	if(en == OFF) StdBy_Off_rsn[path4] = _SET_;
	else          StdBy_Off_rsn[path4] = _CLR_;
#endif

	print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[StandBy_PAU_EN_Ctrl] PAU#%d (onoff %d), %C", path4+1, en, from);
	
	return cmd;	
}
//------------------------------------------------------------------
INT8U Amp_StdBy_PAU_OFF_Req(INT8U path4, char from)
{
	AMP_Stby_Off_Req[path4] = 1;
	AMP_Stby_Ctrl_Val[path4] = OFF;

	return StandBy_PAU_EN_Ctrl(path4, OFF, from);	
}
//------------------------------------------------------------------
INT8U Mimo_StBy_PAU_Ctrl_Req(INT8U path4, INT8U en, char from)
{
	Mimo_Stby_Ctrl_Req[path4] = 1;
	Mimo_Stby_Ctrl_Val[path4] = en;

	return StandBy_PAU_EN_Ctrl(path4, en, from);	
}
//------------------------------------------------------------------






static INT16S mimo_ref_x10[AMPU_4_MAX];
static INT16S siso_ref_x10[AMPU_4_MAX];

void Get_MIMO_Standby_Pwr_Level( void )
{	
	INT8U i;

	AMP_Not_Use_Pwr_Chk();

	for(i=0;i<AMPU_4_MAX;i++)
	{
	#ifdef __TYPE__PRU__
		siso_ref_x10[i] = 340; // 34.0[dBm] @ 5G
		mimo_ref_x10[i] = 340; // 34.0[dBm] @ 5G, MIMO AMP Off Level
	#else // ANTS-AAU-Only
		siso_ref_x10[i] = 120; // 12.0[dBm] @ 5G, 2019.05.25
		mimo_ref_x10[i] = 120; // 12.0[dBm] @ 5G, MIMO AMP Off Level
	#endif
	}
}


/*
---------------------------------------------------------
추가 구현 할 부분.

1. 
	1-layer SISO mode 일때, DL Gain 3dB 감소 -- 어디를 제어? PAU ATT ? FPGA DL ??
2. 	
	algorithm 동작 시, amp-off alarm, low alarm mask

3. 동시 발생 우선 순위.
 	Battery Extension > MIMO-Standby
---------------------------------------------------------
*/







static OS_TIMER amp_on_timer_ms[AMPU_4_MAX];
static INT8U amp_on_timer_out[AMPU_4_MAX]={0,0,0,0};
#define Max_AMP_Pair (3)
INT8U Proc_MIMO_Standby(void)
{	
	static INT8U f_In_Time_Prev=0;
	static INT8U f_mode_onoff_Prev=0;
	static INT8U layer_mode_prev = 0;
	static INT16S pwr_x10[_SSMM_];
	
	INT8U f_In_Time;
	INT8U i;
	INT8U stdby_req[Max_AMP_Pair];
	INT8U target_cnt;//, off_req_cnt;
	INT8U siso, mimo;
	INT8U pair[Max_AMP_Pair][_SSMM_];
	INT16S level_x10[_SSMM_]; 
	INT16S level_clr_x10[_SSMM_]; 
	INT8U standby_quit_req = 0; // 2019.04.16
	
	INT16U start16, stop16;
	static INT16U start16_prev = 0;
	static INT16U stop16__prev = 0;
	INT8U PAU_OFF_Req[AMPU_4_MAX] = {0,0,0,0}; // 2020.01.10
	static INT8U  skip_cnt = 0;
	

	//---------------------------------------------------------
	if( OSTimerGet_msec(&MIMO_Standby_Timer_ms) < 500 ) return 0; // every 500[ms]
	OSTimerReset( &MIMO_Standby_Timer_ms );
	//---------------------------------------------------------	
	
	
	Chk_End_of_Mimo_Standby_Exit_Dly(); // DPD Stablization

	//---------------------------------------------------------
	start16 = m_St.MiMo_Stdby_Start_Time[0]; start16 <<= 8;
	start16 += m_St.MiMo_Stdby_Start_Time[1];

	stop16 = m_St.MiMo_Stdby_Stop_Time[0]; stop16 <<= 8;
	stop16 += m_St.MiMo_Stdby_Stop_Time[1];
	//---------------------------------------------------------
	if(m_St.Sum_MIMO_Stdby_St == ON)
	{
		if(layer_mode_prev != m_Eep.Layer_Mode)
		{
			m_St.MIMO_Stdby_Enable = OFF;
			m_Eep.MIMO_Stdby_Mode_Onoff = OFF;

			layer_mode_prev = m_Eep.Layer_Mode;
			m_Env.is_eep_write = EEP_WRITE_CODE;
			
			return 0;
		}
		if((start16_prev != start16)||(stop16__prev != stop16))
		{
			m_St.MIMO_Stdby_Enable = OFF;
			m_Eep.MIMO_Stdby_Mode_Onoff = OFF;
			
			m_Env.is_eep_write = EEP_WRITE_CODE;

			start16_prev = start16;
			stop16__prev = stop16;
			
			return 0;
		}		
	}
	//---------------------------------------------------------
	start16_prev = start16;
	stop16__prev = stop16;
	//---------------------------------------------------------
	/*
	---------------------------------------------------------
		// 2019.04.16
		결국, 4_Layer_MIMO Mode일때만 동작.
	---------------------------------------------------------	
	*/
	switch(m_Eep.Layer_Mode)
	{
		case Oper_MIMO:
			for(i=0; i<Max_AMP_Pair; i++)
			{
				pair[i][_SS_] = AMPU_4_1;
				pair[i][_MM_] = AMPU_4_2+i;
			}
			
			if(AMP_Alarm_St(AMPU_4_1) == OFF) 
				 standby_quit_req = 1; 
			else if(Check_PAU_Use(AMPU_4_1) == 0) 
				standby_quit_req = 1; 
			else
				standby_quit_req = 0; 
				
			break;
			
		default:
			standby_quit_req = 1;			
			break;
	}
	if(standby_quit_req)
	{
		//-----------------------------------------
		// 2019.04.22 : SISO-Mode 의 경우, 
		// MIMO-Standby 동작 하지 않고, MIMO-Standby 동작상태는 Off.
		//-----------------------------------------
		for(i=0; i<AMPU_4_MAX; i++)
		{
			if(m_St.MIMO_Stdby_St[i])
			{
				Set_MIMO_Standby_St(i, _CLR_);
				Mimo_Standby_Exit_Dly_REQ(i);
				amp_on_timer_out[i]=0;
				OSTimerReset( &amp_on_timer_ms[i] );
			}
		}
	}

	//---------------------------------------------------------
	// if mimo standby mode is off
	//---------------------------------------------------------				
	
	
	if((m_St.MIMO_Stdby_Enable == OFF) || Is_RU_SISO() || standby_quit_req)
	{		
		if((layer_mode_prev != m_Eep.Layer_Mode)|| // if layerMode 'mimo' -> 'siso'
		  (f_mode_onoff_Prev != m_St.MIMO_Stdby_Enable)) // if on -> off
		{
			OSTimerReset( &MIMO_Standby_Timer_ms );

			if(f_mode_onoff_Prev != m_St.MIMO_Stdby_Enable)
				print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] MIMO_Stdby_Enable :  %d -> %d", 
					f_mode_onoff_Prev, m_St.MIMO_Stdby_Enable);
			

			for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
			{
				mimo = pair[i][_MM_];
				if(mimo >= AMPU_4_MAX)
					continue;
				// restore with user-control value
				Mimo_Stby_Bk_AMP_St[mimo] = Make_Stdby_Bk_AMP_St(mimo);

				print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[MM-Stdby] m_St.MIMO_Stdby_St : %d", 
						m_St.MIMO_Stdby_St[mimo]);

				if(m_St.MIMO_Stdby_St[mimo]) // 2020.01.13
				{
					Mimo_StBy_PAU_Ctrl_Req(mimo, Mimo_Stby_Bk_AMP_St[mimo], 'A'); // 2020.01.13								
					Set_MIMO_Standby_St(mimo, _CLR_);
					Mimo_Standby_Exit_Dly_REQ(mimo); // 01.14
				}
				amp_on_timer_out[mimo]=0;
				OSTimerReset( &amp_on_timer_ms[mimo] );				
			}
			f_mode_onoff_Prev = m_St.MIMO_Stdby_Enable;
			
			// even if user off -> on control in the "in-time"
			// for init-routime operating
			f_In_Time_Prev=0; 
		}
		layer_mode_prev = m_Eep.Layer_Mode;		
		return 1;
	}
	f_mode_onoff_Prev = m_St.MIMO_Stdby_Enable;
	layer_mode_prev = m_Eep.Layer_Mode;
	//---------------------------------------------------------	
	#ifdef __TYPE__PRU__
	if(Is_AC_Fail()) return 1;	
	#endif
	//---------------------------------------------------------
	// Standby Mode ON
	//---------------------------------------------------------
	f_In_Time = Is_MIMO_Mode_Time();
	//---------------------------------------------------------
	if(m_Env.AMP_Ctrl_Allowed==0) // 2020.01.14
	{
		f_In_Time = 0;
		
		if(skip_cnt >= 10) skip_cnt = 0;
		if(skip_cnt == 0) print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[MM-Stdby] But Under RU.Init, Skipping MM-Stdby.");
		skip_cnt++;
	}
	else skip_cnt = 0;
	//---------------------------------------------------------
	if(f_In_Time==2) // if user changed start or stop time
	{
		OSTimerReset(&MIMO_Standby_Timer_ms);

		for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
		{
			mimo = pair[i][_MM_];
			if(mimo >= AMPU_4_MAX)
				continue;
			
			amp_on_timer_out[mimo]=0;
			OSTimerReset(&amp_on_timer_ms[mimo]);
		}
		return 1; // for restart algorithm
	}
	//---------------------------------------------------------
	if(f_In_Time==0) // if out of operation-time
	{
		for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
		{
			mimo = pair[i][_MM_];
			if(mimo >= AMPU_4_MAX)
				continue;
			
			
			amp_on_timer_out[mimo]=0;
			OSTimerReset( &amp_on_timer_ms[mimo] );

			// restore value is User-Controlled value
			if(f_In_Time_Prev != f_In_Time)
			{				
				Mimo_Stby_Bk_AMP_St[mimo] = Make_Stdby_Bk_AMP_St(mimo);

				if(m_St.MIMO_Stdby_St[mimo]) // 2020.01.13
				{
					Mimo_StBy_PAU_Ctrl_Req(mimo, Mimo_Stby_Bk_AMP_St[mimo], 'A'); // 2020.01.13
					Mimo_Standby_Exit_Dly_REQ(mimo);					
				}

				print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] AMP#%d f_In_Time == 0, Mimo_Stby_Bk_AMP_St = %d", 
					mimo+1, Mimo_Stby_Bk_AMP_St[mimo]);
			}
			Set_MIMO_Standby_St(mimo, _CLR_); // exit MIMO Standby			
		}		
		OSTimerReset( &MIMO_Standby_Timer_ms );
		f_In_Time_Prev=0;
		
		return 1;	// return...
	}
	else // MiMo Standby Time
	{
		//////D_Printf(DEBUG_STDBY, "[MM-Stdby] Proc...");

		if(f_In_Time_Prev != f_In_Time) // 1st time entry 
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] 1st time entry......" VT_END_CR);
			
			for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
			{
				mimo = pair[i][_MM_];
				if(mimo >= AMPU_4_MAX)
					continue;
				
				Mimo_Stby_Bk_AMP_St[mimo] = Make_Stdby_Bk_AMP_St(mimo);
				print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] AMP#%d : Mimo_Stby_Bk_AMP_St %d..(0 is ON)....", 
					mimo+1, Mimo_Stby_Bk_AMP_St[mimo]);
				
				// during mimo-standby-time, even if user controlled 'mimo-stdby-off' -> 'mimo-stdby-on' 
				// for restart to be restart
				Set_MIMO_Standby_St(mimo, _CLR_); // init

				// Disable / shutdown alarm 해제 되지 않도록 수정 								
				/////////Mimo_Standby_Exit_Dly_REQ(mimo); // 2020.01.10 : delete				
				m_St.Mimo_Stdby_St_Dly_REQ[mimo] = 0;
				
				amp_on_timer_out[mimo]=0;
				OSTimerReset( &amp_on_timer_ms[mimo] );
			}
		}		
		Get_MIMO_Standby_Pwr_Level();

		//======================================================================================	
		
		target_cnt = 0; // init
		for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
		{
			if((pair[i][_SS_] == 0xFF)||(pair[i][_MM_] == 0xFF))
			{
				continue;
			}
		
			siso = pair[i][_SS_];	
			mimo = pair[i][_MM_];
			
			pwr_x10[_SS_] = m_St.Fwd_Out_Pwr_x10[siso];
			pwr_x10[_MM_] = m_St.Fwd_Out_Pwr_x10[mimo];
			
			// check condition ------------------------------
			     if( pwr_x10[_SS_] == 0)                  stdby_req[i] = 0;
			else if( m_Eep.Pwr_Amp_En[mimo] == OFF)       stdby_req[i] = 0; // if user off mimo amp
			//------------------
			else if( Check_PAU_Use(siso) == 0)            stdby_req[i] = 0;
			else if( Check_PAU_Use(mimo) == 0)            stdby_req[i] = 0;			
			//------------------
			else if( m_St.AlmSt.Path[siso].PAU.VSWR)      stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].PAU.DC_Fail)   stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].DPD.DPD_Alm)   stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].PAU.Dev_Fail)  stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].PAU.Link_Fail) stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].PAU.High_Temp) stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].PAU.Shutdown)  stdby_req[i] = 0;
			else if( m_St.AlmSt.Path[siso].PAU.Disabled)  stdby_req[i] = 0;
			//------------------
			else stdby_req[i] = 1;								
			//-----------------------------------------------

			if(stdby_req[i] )
			{
				target_cnt++;
				level_x10[_SS_] = m_Eep.MIMO_Stdby_Level_x10[_IN_];
				level_x10[_MM_] = m_Eep.MIMO_Stdby_Level_x10[_IN_];
					
				level_clr_x10[_SS_] = m_Eep.MIMO_Stdby_Level_x10[_OUT_];
				level_clr_x10[_MM_] = m_Eep.MIMO_Stdby_Level_x10[_OUT_];

				// 2020.01.10 : deleted
				//////////////////////Set_MIMO_Standby_St(mimo, _SET_); // 2020.01.10 : deleted
				// 2020.01.10 : deleted

				//D_Printf(DEBUG_STDBY, "[MM-Stdby] MM AMP#%d OnOff St %d)\n", mimo+1, AMP_Alarm_St(mimo) );

				if(AMP_Alarm_St(mimo) == ON) // check off-condition
				{
					if (amp_on_timer_out[mimo]==0)
					{
						if(( pwr_x10[_SS_] <= level_x10[_SS_] ) && ( pwr_x10[_MM_] <= level_x10[_MM_] )) // 2019.04.24. 부등호.
						{										
							if( OSTimerGet_msec(&amp_on_timer_ms[mimo]) >= amp_on_timer_timeout_ms ) 
							{
								amp_on_timer_out[mimo]=1;

								if(PAU_OFF_Req[mimo] == 0)
								{
									PAU_OFF_Req[mimo] = 1;
									print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] MM AMP#%d off flag\n", mimo+1);
									
									// 2020.01.10 : add belows
									//---------------------------
									PAU_OFF_Req[mimo] = 0;
									Backup_Cur_Alarm_St(mimo);
									Mimo_Standby_AMP_Off_flag(mimo, _SET_);
									Mimo_StBy_PAU_Ctrl_Req(mimo, OFF, 'A');
									Set_MIMO_Standby_St(mimo, _SET_);
									print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] MM AMP#%d off req\n", mimo+1);									
									//---------------------------
								}
								OSTimerReset( &amp_on_timer_ms[mimo] );
							}
						}
						else OSTimerReset( &amp_on_timer_ms[mimo] );
					}
					else OSTimerReset( &amp_on_timer_ms[mimo] );
				}
				else if(amp_on_timer_out[mimo] && (AMP_Alarm_St(mimo) == OFF)) // check ON condition
				{	
					PAU_OFF_Req[mimo] = 0;
					if((AMP_Alarm_St(siso) == ON) && (pwr_x10[_SS_] > level_clr_x10[_SS_])) // 2019.04.24. 부등호.
					{
						if(OSTimerGet_msec(&amp_on_timer_ms[mimo]) >= 1000)
						{
							print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[MM-Stdby] SISO AMP#%d : Pwr:%d, lvl:%d.", 
								siso+1, 
								pwr_x10[_SS_], 
								level_clr_x10[_SS_] );
							
							amp_on_timer_out[mimo] = 0;
							Mimo_StBy_PAU_Ctrl_Req( mimo, ON, 'A');
							Mimo_Standby_Exit_Dly_REQ(mimo); // 2020.01.10
							Set_MIMO_Standby_St(mimo, _CLR_); // 2020.01.10
							Mimo_Stby_Bk_AMP_St[mimo] = Make_Stdby_Bk_AMP_St(mimo);
							OSTimerReset( &amp_on_timer_ms[mimo] );
						}
					}
					else OSTimerReset( &amp_on_timer_ms[mimo] );
				}
				else OSTimerReset( &amp_on_timer_ms[mimo] );				
			}
			else
			{
				if(m_St.MIMO_Stdby_St[mimo])
				{
					Set_MIMO_Standby_St(mimo, _CLR_);
					Mimo_Standby_Exit_Dly_REQ(mimo);
				}
				amp_on_timer_out[mimo]=0;
				OSTimerReset( &amp_on_timer_ms[mimo] );
				PAU_OFF_Req[mimo] = 0;
				//D4_Printf(DEBUG_STDBY, path4, "[MM-Stdby] MiMo AMP#%d : req = 0\n", path_mimo+1);
			}

			#if 0
			D_Printf(DEBUG_STDBY, "[MM-Stdby] SS-AMP#%d : Pwr %d/10, Level %d/10\n", 
				siso+1, pwr_x10[_SS_], level_x10[_SS_]);

			D_Printf(DEBUG_STDBY, "[MM-Stdby] MM-AMP#%d : Pwr %d/10, Level %d/10\n", 
				mimo+1, pwr_x10[_MM_], level_x10[_MM_]);
			
			D_Printf(DEBUG_STDBY4, "[MM-Stdby] MM-AMP#%d : amp_on_timer (%d / %d), req(%d)",
				mimo+1, 
				amp_on_timer_out[mimo], 
				amp_on_timer_ms[mimo], 
				stdby_req );			
			#endif
		}

		// 2019.06.18 : operation chagned : OR -> AND
		#if 0 
		/*
		// 2020.01.10 : delete belows, 동시에 들어가지 않아도 되도록 수정.
		if(target_cnt>0)
		{
			off_req_cnt = 0;
			for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
			{					
				mimo = pair[i][_MM_];
				if(mimo == 0xFF) continue;
				
				if(PAU_OFF_Req[mimo]) 
					off_req_cnt++;
			}		

			if(target_cnt == off_req_cnt) // if all off Requested...
			{
				for(i=0; i<Max_AMP_Pair; i++) // for pair num 3
				{
					mimo = pair[i][_MM_];
					if(mimo == 0xFF) continue;					
					else if(PAU_OFF_Req[mimo]) // Off requested-AMP Only
					{
						PAU_OFF_Req[mimo] = 0;
						Mimo_StBy_PAU_Ctrl_Req(mimo, OFF, 'A');
						D_Printf(DEBUG_STDBY, "[MM-Stdby] MM AMP#%d off req\n", mimo+1);

						Backup_Cur_Alarm_St(mimo); // 2019.07.19
					}
				}
			}
			else
			{
				D_Printf(DEBUG_STDBY, "[MM-Stdby] target_cnt(%d) != off_req_cnt(%d)\n", target_cnt, off_req_cnt);
			}
		}
		*/
		#endif
	}	
	//======================================================================================
	f_In_Time_Prev = f_In_Time; 
	return 1;
	//======================================================================================	
}


///////////////////////////////////////////////////////////////////////////////////////
//
//  END Of 
//  MIMO Standby
//
///////////////////////////////////////////////////////////////////////////////////////
























/*
 --------------------------------+-------------------------------
 AC Alarm 발생 후, 30초 이전  |                                   
 -----------------------------+--+-------------------------------
 AC Alarm 발생 후, 30초~5분전 |  | Mimo AMP                     
                              |  | att 3dB                      
단, siso path off 시,         |    (SISO : att 3dB만)       
    mimo path on 유지         |                                 
 -----------------------------+--+-------------------------------
 AC Alarm 발생 후, 5분 이후   |   
 -----------------------------+--+------------------------------- 
layer 운용 모드에 따라, 동작. (if keeping ac-fail for 30[s])

	- siso amp : att 3dB
	- mimo amp : amp off

1. SISO operation Mode : all att 3dB Set.
2. Mimo Opersation Mode
	siso : 3dB
	mimo : amp off
	siso - mimo pair : if siso amp off, mimo : no operation

algorithm 동작 중,  amp off 사용자 제어 불가.
AC 복구 되면, 이전 상태 복구.

algorithm 동작 중, amp-off alarm, low alarm 은 이전 상태 유지.
*/



//-------------------------------------------------
// AC Alarm - AMP OFF & ATT 3dB Add : 2013.06.11.
//-------------------------------------------------
#if 0 // TEST
#define _5_MIN_    (30000)
#define _30_SEC_   (10000)
#else
#define _5_MIN_   (300000)
#define _30_SEC_   (30000)
#endif

#ifdef __TYPE__PRU__
static OS_TIMER ac_normal_time_ms;
#endif 

///// 임시로 추가 된 부분 임으로 필수로 체크 확인 필요.

/*
-----------------------------------------------------------------------
Super cap. 활용 AC 알람보고
-----------------------------------------------------------------------

1. AC Fail GPIO를 20ms 주기로 감시 : 0.3초(20[ms] x 15회 연속) 유지시 AC 알람을 인지,
	- DTU DL/UL Off, DPD Core Reset hold , AFE Reset hold 순서로 제어하고 
	- AC 알람을 Noti IP2로 전송

	---->>>> 2019.04.10 박성진 차장 요청
	RW_Reset의 register 위치 문제로 
	pru_dtu App 을 아예 죽이는 걸로....

2. AC가 재 인가되어 AC Fail 알람이 복구 되었을때는.
	- AFE Reset 해제, DTU DPD Core Reset 해제, DTU DL/UL On 
	- 이후 pru_dtu Application을 재 실행.
-----------------------------------------------------------------------
*/

#ifdef __TYPE__PRU__ // ANTS-PRU	
static OS_TIMER ac_fail_chk_timer;
#endif

void Update_AC_Alm(void)
{
#ifdef __TYPE__PRU__ // ANTS-PRU	
	INT8U  psu_ac_alm; // battery 실장관련 알람.	
	static INT8U ac_alm = 0;
	static INT8U alm_prev = 0;	
	static INT8U init = 0;

	//-------------------------------------------------
	if(m_St.AlmTest) // 2019.05.02
		return;
	//-------------------------------------------------	

	if(init == 0)
	{
		init = 1;
		OSTimerReset(&ac_fail_chk_timer);
	}	

	if(GPIO_Get(GPIO_PSU_AC_FAIL_I)) // high is normal
	{
		OSTimerReset(&ac_fail_chk_timer);
		ac_alm = 0;
	}
	else // low is alarm
	{
		///// YCSO change 300 --> 200 190422
		// 200[ms] --> 300[ms] 2019.05.20.
		// 2019.05.22 DVT Test : 50[ms]
		// 2019.06.26 DVT Test : 300[ms]
		// 2019.06.26 DVT Test : 300[ms]
		// 2019.07.04 DVT Test : 100[ms]
		
		////if(ac_alm == 0) // 2020.02.17 deleted
		////{
			if(OSTimerGet_msec(&ac_fail_chk_timer) >= (100))
				ac_alm = 1;
		////}
		////else OSTimerReset(&ac_fail_chk_timer); // 2020.02.17 deleted		
	}

	if(m_St.Batt_Insert) // 2019.05.20.
		psu_ac_alm = (m_PSU.AC_Alm_Recti | m_PSU.AC_Alm_OV)? 1:0;
	else
		psu_ac_alm = 0; 
	
	m_St.AlmSt.A_0.AC = (ac_alm | psu_ac_alm);

	/*	
	2020.04.10 : 
	[현상] Battery 실장 상태에서 AC 단절 시 AC Alarm 발생이 보고 되나 다시 AC복구 시,
		AC Alarm 해제 보고가 아니라 '발생'보고가 올라 감.
	[수정]	if(alm_prev != m_St.AlmSt.A_0.AC) 비교하도록 수정.
		
	*/	
	if(alm_prev != m_St.AlmSt.A_0.AC) // 2020.04.10
	{
		alm_prev = m_St.AlmSt.A_0.AC;
		
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[AC Alarm(%d)] gpio alarm(%d), PSU alarm(%d)", 
					m_St.AlmSt.A_0.AC, ac_alm, psu_ac_alm);
		
		// REMS Report is needed
		/*		
		Battery 미실장시 AC 알람 발생시에는 알람 전송 이후 ACK 대기시간을 1초로 합니다.
		즉, 전원 차단시 Super Cap으로 동작시에는 ACK가 미수신되면 1초 주기로 AC 알람을 보고 합니다.
		*/

	#ifdef __AC_Fail_TEST_PAU0_RST_PIN_Toggle__
		AC_Fail_TEST_PAU0_RST_PIN_Toggle_Count = 0;
		GPIO_Set( GPIO_PAU_0_RST_O, GPIO_VALUE_HIGH); // init
	#endif
		
		Send_PRU_AC_Alarm_gREMS(m_St.AlmSt.A_0.AC);
		
	#ifdef __AC_Fail_TEST_PAU0_RST_PIN_Toggle__
		AC_Fail_TEST_PAU0_RST_PIN_Toggle_Count = 1;
		GPIO_Set( GPIO_PAU_0_RST_O, GPIO_VALUE_LOW );
	#endif
	}	
#else // __TYPE__AAU__
	m_St.AlmSt.A_0.AC = 0;	
#endif	
}
//--------------------------------------------------------------
void Proc__SuperCap_AC_Alarm_Report(void) // Alarm TEST
{
	static INT8U prev_ac = 0;
	//---------------------------
	if(m_St.AlmTest)
	{
		if(m_St.AlmSt.A_0.AC)
		{
			if(prev_ac != m_St.AlmSt.A_0.AC)
			{
				print_agent(DEBUG_REMS, VT_BOLD_GREEN, "AC Fail REMS TEST");
				Send_PRU_AC_Alarm_gREMS(1);
			}			
		}		
		prev_ac = m_St.AlmSt.A_0.AC;
		return;
	}
	//---------------------------	

	/*
	------------------------------------------------------		
	2020.04.08 : 
	PSU와 통신이 5회 이상 유지가 된 조건에서 AC Fail 로 인한, AFE Off 등의 동작할 수 있도록 변경
	---------------------------	---------------------------	
	*/
	if(PSU_Comm_Cnt < 5) 
	{
		return;
	}
	
#ifdef __TYPE__PRU__

	INT8U c; // fa, m,
	char cmd[128];
	static INT8U ac_alarm_prev = 0;
	INT8U ac_alarm_curr = 0;

extern int Chk_Kill_Dig_App(char* app_name);
extern int Kill_Dig_App(char* app_name);


	Update_AC_Alm(); // Get AC Fail Status Every 10[ms]

	ac_alarm_curr = m_St.AlmSt.A_0.AC;

	/*
	// 2019.07.04 
	//------------------------------
	// 0 ~ 100 ms : AC Fail
	// 100 ~ 300 ms : do nothing
	// 300ms ~ : Proc__SuperCap_AC_Alarm
	*/
	if(ac_alarm_curr)
	{
		if(OSTimerGet_msec(&ac_fail_chk_timer) < 300)
		{
			ac_alarm_curr = 0;
		}
	}
	//-----------------------------------	
	
	if(ac_alarm_curr)
	{		
		if((ac_alarm_prev != ac_alarm_curr) 
		   && (m_St.Batt_Insert==0)) // 2019.04.25. condition(batt.-Not-insert) added.
		{
			/*
			박성진 차장 요청으로 아래와 같이 바꿈. 190422
			*/			
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, " Super Cap, AC Fail Detection.");
			
			// Control Directry to FPGA
			Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_AC_ALARM_INFO], 0xABCD1234, "AC_Alarm");
			for(c = CPRI_1; c <= CPRI_3; c++ )
				Write_FPGA_Reg(Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_Reset], 0x00, "SFR_Reset");

			//// YCSO added 190516
			GPIO_Set( GPIO_OPT_SW_Status1, GPIO_VALUE_LOW ); // AFE Power Off
						
			///// YCSO change START 190422
			if(Chk_Kill_Dig_App("exec_dtu_startup.sh")==RET_SUCCESS)
			{
				printf( "[RCU] %s is terminated.", "exec_dtu_startup.sh");
			}
			else
			{

				printf( "[RCU] %s is exist.", "exec_dtu_startup.sh");
				sprintf(cmd, "kill -9 `pidof %s`", "exec_dtu_startup.sh");
				system( cmd );
				printf( "[RCU] %s is terminated.\n", "exec_dtu_startup.sh");

			}

			if(Chk_Kill_Dig_App("pru_dtu")==RET_SUCCESS)
			{
				printf( "[RCU] %s is terminated.\n", "pru_dtu");
			}
			else
			{
				printf( "[RCU] %s is exist.\n", "pru_dtu");
				sprintf(cmd, "kill -9 `pidof %s`", "pru_dtu");
				system( cmd );
				printf( "[RCU] %s is terminated.\n", "pru_dtu");
			}
			printf( VT_BOLD_RED "[RCU] Super Cap, AC Fail. All Off Ctrl" VT_END_CR);
			///// YCSO chagne END 
		}
	}
	else
	{
		if(ac_alarm_prev != ac_alarm_curr) // AC Fail --> Normal
		{
			printf(VT_BOLD_GREEN "[RCU] Super Cap, AC Normal. All ON Ctrl\n");
		#if 1
			/*
			*2019.08.12:
				*if AC Fail -> normal : system "halt" 로 PRU Restart되도록 수정.
			2019.09.11: 
				battery 미실장인 상태에서만 reboot 되도록 수정.
			*/
			if(m_St.Batt_Insert==0) // reboot only when battery-not-insert.
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, " system halt (AC-Fain --> Normal)");
				system_agent("halt");
				SetResetInfo( Rst_By_Etc, 2198 );
			}
		#else
			/*
			for(fa = FA_0; fa < MAX_FA_PATH; fa++ )
			{
				for( m = PATH_0 ; m < MAX_PATH_4; m++ )
				{
					DTU_DL_Path_Onoff(fa, m, ON); // X
					DTU_UL_Path_Onoff(fa, m, ON); // X
				}
			}
			//------------------------------------
			// DPD core
			for(m = DL_0; m < MAX_DL; m++ )
			{
				m_Reg_Flg.DPD[m].DPD_Reset.uD32 = _SET_;
				m_Reg_Ctl.DPD[m].DPD_Reset.uD32 = 1; // DPD Normal
			}
			//------------------------------------
			// AFE Reset
			m_Reg_Flg.RW_Reset.Data = _SET_;
			m_Reg_Ctl.RW_Reset.Data = 0xFFFFFFFF;
			*/
		#endif
		}
	}
	ac_alarm_prev = ac_alarm_curr; // update
#endif	
}






/*
--------------------------------------------------------------	

Battery Extension Algorithm


2019.04.16 : 
	"AC 알람" 관계없이 "Batt.Ext.Mode == OFF" 이면, mode 동작 하지 않도록 수정.
	algorithm not going : AC_Normal 처럼 동작되게 함.(off-amp on, 3db-Att is 0dB)		


(1) "Layer 운영 모드 MIMO" :
	Layer 0     : On/3dB
	Layer 1,2,3 : Off

(2) "Layer 운영 모드 SISO" :
	Layer 0,1,2,3 : all 3dB


2020.04.08 : 현상
	(1) --> (2) 변경 시, Layer 1,2,3 PAU는 Off상태 유지 => On이 되도록 변경 필요
	(2) --> (1) 변경 후, Battery Extension mode off 시, Layer 1,2,3 Off 가 On 이 안됨.

2020.04.10 : 
	현상 : 
		['AC fail 상태']에서, 'extension mode off시', 혹은 'Layer 운동모드를 MIMO -> SISO 변경시' AMP On 인되는 부분.
	수정 : 
		['AC fail 상태'] 일때, 동작 하는 조건을 ['AC fail 상태' && 'extension mode on']으로 변경.
		algorithm 으로 amp off 하기 전 amp 상태 backup(algorithm 끝날때 원복되야 하는 amp 상태)할때,
		최초 1회만 backup 하도록 수정.


--------------------------------------------------------------	
*/
void Proc_AC_Fail_Batt_Save(void) // Battery Extension
{	
#ifdef __TYPE__PRU__
	INT32U ac_fail_conti_time_ms;
	INT8U path4;
	INT8U pair_amp;	
	INT8U not_use_cnt;
	INT8U operation_en;	
	static INT8U init = 0;
	static INT8U Layer_Mode;
	static INT8U Ext_Mode_Onoff;
	////static INT32U cnt = 0;
	
	//--------------------------------------------------------------
	if(init == 0)
	{
		init = 1;
		m_St.Batt_Ext_Mode_St = OFF;
		Layer_Mode = m_Eep.Layer_Mode;
		Ext_Mode_Onoff = m_Eep.Batt_Ext_Mode_Onoff;
	}

	/*
	cnt++;
	print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Proc_AC_Fail_Batt_Save] %d ------ Mode %d, ac.alm %d -------------", 
				cnt,
				m_Eep.Batt_Ext_Mode_Onoff, 
				m_St.AlmSt.A_0.AC);
	*/			
	
	////for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	////	Disp_AMP_St(path4);
	
	//------------	
	if(Layer_Mode != m_Eep.Layer_Mode)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[AC-Batt-Ext] Layer_Mode : %d --> %d", 
					Layer_Mode, m_Eep.Layer_Mode);
					
		Layer_Mode = m_Eep.Layer_Mode;
	}

	if(Ext_Mode_Onoff != m_Eep.Batt_Ext_Mode_Onoff)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[AC-Batt-Ext] Ext_Mode : %s --> %s", 
					Get_OnOff_Str(Ext_Mode_Onoff),
					Get_OnOff_Str(m_Eep.Batt_Ext_Mode_Onoff));
					
		Ext_Mode_Onoff = m_Eep.Batt_Ext_Mode_Onoff;
	}
	//------------	
	     if(m_Eep.Batt_Ext_Mode_Onoff == OFF) operation_en = 0;
   	else if(m_St.AlmSt.A_0.AC == 0)           operation_en = 0;
	else if(m_Eep.Layer_Mode >= Oper_Max)     operation_en = 0;
	else
	{
		operation_en = 1;		
		
		//---------------------------------------
		// 2019.04.22
		// when SISO is amp off When MIMO-Mode
		//---------------------------------------		
		for(not_use_cnt = 0,path4 = 0; path4 < AMPU_4_MAX; path4++)
		{
			if(Check_PAU_Use(path4)==0)
			{
				not_use_cnt++;			
				AC_Fail_Ctrl_Req[path4] = 0;
			}		
			else if(Is_MIMO_AMP(path4)) // MIMO
			{
				pair_amp = Get_AMP_Pair(path4);

				// if opposite-SISO-AMP is OFF
				if(m_St.Pwr_Amp_En[pair_amp] == OFF) // 2020.04.13 : "AMP_Alarm_St()" --> "m_St.Pwr_Amp_En[]"
				{
					operation_en = 0;
					break;
				}
			}
		}
		if(not_use_cnt >= AMPU_4_MAX) operation_en = 0;
	}
	
	//--------------------------------------------------------------
	// NOW AC.Normal or 'extension.mode off' 
	//--------------------------------------------------------------
	if(operation_en == 0)
	{
		//--------------------------------------------------------------
		if(AC_Extension_St == _AC_FAIL_)
		{
			if(OSTimerGet_msec(&ac_normal_time_ms) > 5000)
				AC_Extension_St = _AC_NORMAL_;
		}
		else
			OSTimerReset( &ac_normal_time_ms );
		//--------------------------------------------------------------
		OSTimerReset( &AC_Fail_Timer_ms );
		AC_Fail_St = _AC_NORMAL_;
		m_St.Batt_Ext_Mode_St = OFF;
		//--------------------------------------------------------------
		// Restore Standby mode
		//-------------------------------------------------
		// if AC-Fail is changed to normal, restore AMP Stdby mode
		// if AC-Fail is changed to normal, restore MIMO Stdby mode
		// mimo stdby와 amp stdby는 동시에 user on 제어 안됨.
		//-------------------------------------------------
		if((m_Eep.MIMO_Stdby_Mode_Onoff == ON)&&(m_Eep.AMP_Stdby_Mode_Onoff == OFF))
		{	
			if(m_St.MIMO_Stdby_Enable == OFF) // now off
			{
				if(m_St.AC_Normal__MIMO_Stdby_Re_ON_Req == 0) // on req
				{
					m_St.AC_Normal__MIMO_Stdby_Re_ON_Req = 1;
					OSTimerReset( &Mimo_Stdby_Re_ON_Req_Timer);		

					print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt Ext] MM-Stdby Re_ON_Req.", 1);	
				}
			}			
		}	
		if((m_Eep.AMP_Stdby_Mode_Onoff == ON)&&(m_Eep.MIMO_Stdby_Mode_Onoff == OFF))
		{
			if(m_St.AMP_Stdby_Enable == OFF) // now off
			{
				if(m_St.AC_Normal__AMP_Stdby_Re_ON_Req == 0) // on req
				{
					m_St.AC_Normal__AMP_Stdby_Re_ON_Req = 1;
					OSTimerReset( &AMP_Stdby_Re_ON_Req_Timer );
					print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt Ext] AMP_Stdby Re_ON_Req.");
				}
			}
		}		
	}
	//--------------------------------------------------------------
	// NOW AC Fail && 'extension.mode ON' 
	//--------------------------------------------------------------
	else // ac operation is enabled...
	{
		AC_Extension_St = _AC_FAIL_;
		OSTimerReset( &ac_normal_time_ms );

		//---------------------------------------------------------
		// Off controll 'MIMO_Std' or 'AMP_Std'
		if((m_St.MIMO_Stdby_Enable == ON) && (m_Eep.AMP_Stdby_Mode_Onoff == OFF))
		{
			m_St.MIMO_Stdby_Enable = OFF;
		}
		if((m_St.AMP_Stdby_Enable == ON) && (m_Eep.MIMO_Stdby_Mode_Onoff == OFF))
		{
			m_St.AMP_Stdby_Enable = OFF;
		}
		//---------------------------------------------------------
		ac_fail_conti_time_ms = OSTimerGet_msec(&AC_Fail_Timer_ms);

		if (AC_Fail_St >= _AC_FAIL_5_MIN_OVER_) // if over 5m
		{
			OSTimerReset( &AC_Fail_Timer_ms );
			ac_fail_conti_time_ms = _5_MIN_;
		}
		//------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------------
		if( ac_fail_conti_time_ms < _30_SEC_ ) // 0s ~ 30s
		{
			m_St.Batt_Ext_Mode_St = OFF;
			AC_Fail_St = _AC_FAIL_;	
		}		
		else //--------------------------------- 30s ~
		{
			m_St.Batt_Ext_Mode_St = ON;
			
			if(ac_fail_conti_time_ms >= _5_MIN_) AC_Fail_St = _AC_FAIL_5_MIN_OVER_;
			else                                 AC_Fail_St = _AC_FAIL_30_SEC_OVER_;				
			
			for(path4 = 0; path4 < AMPU_4_MAX; path4++)
			{				
				if(Check_PAU_Use(path4)==0) continue;
				
				//---------------------------------------
				// SISO : ATT + 3dB
				// SISO Mode : AMP#1/2/3/4 SISO
				// MIMO Mode : AMP#1  only SISO
				//---------------------------------------	
				if(Is_SISO_AMP(path4))
				{
					if(m_St.Pwr_Amp_En[path4] == ON) // 2020.04.13 : "AMP_Alarm_St()" --> "m_St.Pwr_Amp_En[]"					
					{
						if((AC_Fail_Ctrl_Req[path4] & _AC_F_ATT_REQ)==0) // if not att-controlled
						{
							print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt-Ext] SS AMP#%d AC_F_ATT_REQ", path4+1);
											
							AC_Fail_Ctrl_Req[path4] |= _AC_F_ATT_REQ;
							AC_Fail_Bk_ATT_x2[path4] = m_St.Fwd_Out_Att_x2[path4];
							
							Fwd_Out_Att_Set_Req(path4, m_St.Fwd_Out_Att_x2[path4] + 6); // + 3dB
						}
					}
					/*
					---------------------------------------
					2020.04.08 : 
					Layer 0 이 아닌 Layer 1,2,3 : m_Eep.Layer_Mode 가 MIMO -> SISO 변경된 경우.
					---------------------------------------
					*/
					if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ)
					{
						if(m_St.Pwr_Amp_En[path4] == OFF) // 2020.04.13 : "PAU_FWD_AMP_St()" --> m_St.Pwr_Amp_En[]
						{
							print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt-Ext] SS AMP#%d controlled ON", path4+1);				
							AC_Fail_PAU_EN_Ctrl(path4, ON);
						}
						else 
						{
							print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt-Ext] SS AMP#%d Already ON", path4+1);
							AC_Fail_Ctrl_Req[path4] &= (~_AC_F_AMP_REQ);
						}
					}
				}
				//---------------------------------------
				// MIMO AMP OFF, if SISO & MIMO All amp on..
				//---------------------------------------				
				else // mimo amp off control
				{
					pair_amp = Get_AMP_Pair(path4); // pair is siso amp(Layer_0)	

					if(m_St.Pwr_Amp_En[pair_amp] == ON) // 2020.04.13 : "AMP_Alarm_St()" --> m_St.Pwr_Amp_En[]
					{
						if(m_St.Pwr_Amp_En[path4] == ON) // 2020.04.13 : "AMP_Alarm_St()" --> m_St.Pwr_Amp_En[]						
						{						
							if((AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ)==0) // if not OFF controlled
							{
								Backup_Cur_Alarm_St(path4);
								AC_Fail_Ctrl_Req[path4] |= _AC_F_AMP_REQ;
								
								print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt-Ext] MM AMP#%d  _AC_F_AMP_REQ", path4+1);								
							}						
							AC_Fail_PAU_EN_Ctrl(path4, OFF);
							print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC-Batt-Ext] MM AMP#%d OFF Ctrl", path4+1);
						}
					}
					else // siso.amp is now off 
					{
						if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ) // if Amp is off, then on
						{
							if(m_St.Pwr_Amp_En[path4] == OFF) // 2020.04.13 : "PAU_FWD_AMP_St()" --> m_St.Pwr_Amp_En[]
							{
								print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt-Ext] MM AMP#%d ON Ctrl", path4+1);					
								AC_Fail_PAU_EN_Ctrl(path4, ON);
							}
							else 
							{
								print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[AC Batt-Ext] MM AMP#%d Already ON", path4+1);
								AC_Fail_Ctrl_Req[path4] &= (~_AC_F_AMP_REQ);							
							}
						}
					}
				}				
			}	
			//---------------------------------------			
		}		
		//------------------------------------------------------------------------------------------
	}
#endif	
}




///////////////////////////////////////////////////////////////////////////////////////
//
//  AC-Fail Algorithm
//
///////////////////////////////////////////////////////////////////////////////////////
// return whether AC-Fail-algorithm now or not 
INT8U Is_AC_Fail(void)
{
	if (AC_Extension_St != _AC_NORMAL_) return 1;
	else return 0;
}

//----------------------------------------------------------------------------------------
// 2020.01.29
// AC Fail일때, AMP alarm을 AC alarm 이전 상태로 보이게 함.
void AC_Alarm_Amp_Alarm_Adj(ALM_ST* pLast_Alm)
{	
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ)
		{
			pLast_Alm->Path[path4].PAU.Shutdown = m_St.Stdby_Bk_Alm.Path[path4].PAU.Shutdown;
			pLast_Alm->Path[path4].PAU.Disabled = m_St.Stdby_Bk_Alm.Path[path4].PAU.Disabled;
			pLast_Alm->Path[path4].DL.Out_Lower = m_St.Stdby_Bk_Alm.Path[path4].DL.Out_Lower;
		}
	}
}




//-------------------------------------------------
// AC Fail -> normal; mimo/amp standby mode restart after 15sec
//
void AC_Normal_Std_Mode_Restart_Chk(void)
{
	if(m_St.AC_Normal__MIMO_Stdby_Re_ON_Req)
	{
		if( OSTimerGet_msec(&Mimo_Stdby_Re_ON_Req_Timer ) > (15*TICK_CO_SEC) )
		{
			m_St.AC_Normal__MIMO_Stdby_Re_ON_Req = 0;
			OSTimerReset( &Mimo_Stdby_Re_ON_Req_Timer );
			
			if((m_Eep.MIMO_Stdby_Mode_Onoff == ON) && (m_St.MIMO_Stdby_Enable == OFF))
			{
				m_St.MIMO_Stdby_Enable = ON;
				D_Printf(DEBUG_SYSTEM, "m_St.MIMO_Stdby_Enable = ON.\r\n");
			}
		}			
	}
	//--------------------
	if(m_St.AC_Normal__AMP_Stdby_Re_ON_Req)
	{
		if( OSTimerGet_msec(&AMP_Stdby_Re_ON_Req_Timer ) > (15*TICK_CO_SEC) )
		{
			m_St.AC_Normal__AMP_Stdby_Re_ON_Req = 0;
			OSTimerReset( &AMP_Stdby_Re_ON_Req_Timer );
			
			if((m_Eep.AMP_Stdby_Mode_Onoff == ON) && ( m_St.AMP_Stdby_Enable == OFF))
			{
				m_St.AMP_Stdby_Enable = ON;
				D_Printf(DEBUG_SYSTEM, "m_St.AMP_Stdby_Enable = ON.\r\n");
			}
		}			
	}
}









///////////////////////////////////////////////////////////////////////////////////////
//
//  End of AC-Fail Algorithm
//
///////////////////////////////////////////////////////////////////////////////////////




/* EOF */

