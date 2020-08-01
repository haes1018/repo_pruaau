/********************************************************************************/
// algorithm_rf.h
#ifndef __ALGORITHM_RF_H
#define __ALGORITHM_RF_H
/********************************************************************************/
#ifdef  ALGORITHM_RF_GLOBALS
#define ALGORITHM_RF_EXT
#else
#define ALGORITHM_RF_EXT extern
#endif
/**************************************************************/


/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
ALGORITHM_RF_EXT INT8U AMP_Stdby_Mode_St_Sum; // default OFF
ALGORITHM_RF_EXT INT8U Proc_AMP_Standby(void);
ALGORITHM_RF_EXT INT8U AMP_Stby_Bk_AMP_St[AMPU_4_MAX]; // exit 시에 복구될 target value.
ALGORITHM_RF_EXT INT8U AMP_Stby_Bk_RVS_AMP_St[AMPU_4_MAX]; // @ AMP-Standby, RVS-AMP not off.

ALGORITHM_RF_EXT void Init_AMP_Standby_Mode_Proc(void);

ALGORITHM_RF_EXT INT8U Is_PD_Alarm(void);
ALGORITHM_RF_EXT INT8U Is_PD_Alarm_AMP_Std(void);
ALGORITHM_RF_EXT void  Init_RU_Proc(void);

ALGORITHM_RF_EXT INT8U Get_Amp_stdby_st_Sum(void);
/*----------------------------------------------------------*/
// AC Fail
ALGORITHM_RF_EXT INT8U  AC_Fail_St; // 시간 상태.
ALGORITHM_RF_EXT INT8U  AC_Extension_St; // AC-Fail(0x1) or Normal(0x0)  
ALGORITHM_RF_EXT INT8U  AC_Fail_Ctrl_Req[AMPU_4_MAX];
ALGORITHM_RF_EXT INT8U  AC_Fail_Bk_ATT_x2[AMPU_4_MAX];


ALGORITHM_RF_EXT INT8U Is_AMP_Rvs_Off_Alarm(INT8U path4);
ALGORITHM_RF_EXT OS_TIMER AC_Fail_Timer_ms; // AC Fail 지속시간 타이머.


ALGORITHM_RF_EXT OS_TIMER AMP_PROC_Start_Timer_ms;






ALGORITHM_RF_EXT INT8U Proc_MIMO_Standby( void );//OK
ALGORITHM_RF_EXT INT8U Mimo_StBy_PAU_Ctrl_Req( INT8U path4, INT8U en, char from );
ALGORITHM_RF_EXT INT8U Amp_StdBy_PAU_OFF_Req(INT8U path4, char from);

ALGORITHM_RF_EXT INT8U StandBy_PAU_EN_Ctrl(INT8U path4, INT8U en, char from);


ALGORITHM_RF_EXT INT8U IS__Mimo_Standby_AMP_Off_flag(INT8U path4);

ALGORITHM_RF_EXT void Set_MIMO_Standby_St(INT8U path4, INT8U set_clr);

ALGORITHM_RF_EXT INT8U Mimo_Stby_Bk_AMP_St[AMPU_4_MAX]; // iRo
ALGORITHM_RF_EXT INT8U Mimo_Stby_Ctrl_Req[AMPU_4_MAX]; // LTE-MM, LTEA-MM만 사용//OK
ALGORITHM_RF_EXT INT8U Mimo_Stby_Ctrl_Val[AMPU_4_MAX]; // LTE-MM, LTEA-MM만 사용//OK
ALGORITHM_RF_EXT void Mimo_Stdby_AMP_Ctrl_Chk(void);
ALGORITHM_RF_EXT void Init_MIMO_Std_Mode(void);
ALGORITHM_RF_EXT INT8U Mimo_Stdby_Mode_St; // default OFF//OK
ALGORITHM_RF_EXT void Mimo_Stby_Amp_Alarm_Adj(ALM_ST* pLast_Alm);
ALGORITHM_RF_EXT void Mimo_Standby_Exit_Dly_REQ(INT8U path4);
ALGORITHM_RF_EXT void Chk_End_of_Mimo_Standby_Exit_Dly(void);




ALGORITHM_RF_EXT void Proc_AC_Fail_Batt_Save(void);
ALGORITHM_RF_EXT void AC_Alarm_Amp_Alarm_Adj(ALM_ST* pLast_Alm);
ALGORITHM_RF_EXT void AMP_Stby_Amp_Alarm_Adj(ALM_ST* last_Alm, INT8U path4);
ALGORITHM_RF_EXT void Proc__SuperCap_AC_Alarm_Report(void);


// 2019.04.18
ALGORITHM_RF_EXT void Rvs_AMP_SD_f_Ctrl( INT8U path4, INT8U sd_rsn, INT8U set );
ALGORITHM_RF_EXT void Fwd_AMP_SD_f_Ctrl( INT8U path4, INT8U sd_rsn, INT8U set );
ALGORITHM_RF_EXT void Alarm_SD_Fwd( INT8U path4 );
ALGORITHM_RF_EXT void Alarm_SD_Rev( INT8U path4 );





//----------------------------------------------------
enum
{
	Timer_AMP_RST = 0,     // AMP#1 ~ AMP#4
	Timer_AMP_SUB_SW = 4,  // AMP#1 ~ AMP#4
	Timer_AMP_Use    = 8,  // AMP#1 ~ AMP#4
	//----------------------
	G_Timer_MAX = 12
};
//----------------------------------------------------
typedef struct
{	
	INT8U  is_timeout; // 1, if timeout
	INT8U  enable;  // 1, if enabled
	INT8U  reserved[2];
	OS_TIMER timer;	
	INT32U timeout_ms;
} G_TIMER;
//----------------------------------------------------
ALGORITHM_RF_EXT G_TIMER g_Timer[G_Timer_MAX];
/*----------------------------------------------------------*/
ALGORITHM_RF_EXT void G_Timer_All_Init(void);
ALGORITHM_RF_EXT void G_Timer_Start( INT8U i, INT32U timeout_ms );
ALGORITHM_RF_EXT INT32U Get_G_CurTime_ms(INT8U i);
ALGORITHM_RF_EXT void Proc_G_Timer(void);
ALGORITHM_RF_EXT void G_Timer_Stop( INT8U i );
ALGORITHM_RF_EXT INT8U Is_G_Timeout( INT8U i );
ALGORITHM_RF_EXT INT8U Is_Timer_En( INT8U i );
ALGORITHM_RF_EXT void Alarm_Mask__Caused_by_AC_Fail(ALM_ST* pLast_Alm);
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
enum
{
	Rsn_Dis_User___Ctrl = 1,
	Rsn_Dis_System_Ctrl = 2,
};

enum
{
	//-----------------------------
	Rsn_SD_TSSI_Over = 1, // by RCU
	Rsn_SD_CPRI_Fail = 2, // by RCU
	Rsn_SD_CLock_Alm = 3, // by RCU	
	Rsn_SD____by_UDA = 4, // by RCU
	//-----------------------------
	Rsn_SD_UL_OverIn = 5, // AMP
	Rsn_SD_Over__Pwr = 6, // AMP, DL
	Rsn_SD__HighTemp = 7, // AMP
	Rsn_SD__by__VSWR = 8, // AMP
	Rsn_SD_____by_DC = 9, // AMP
	Rsn_SD____by_PLL = 10, // AMP, not used @ 5G

	Rsn_SD_UL_OverIn_Skip = 100, // GUI alarm but not report to gREMS/LS
	//-----------------------------
};

ALGORITHM_RF_EXT void Set__Reason_Dis_5117(INT8U path4, INT8U reason);
ALGORITHM_RF_EXT void Set__Reason_SD_5115(INT8U path4, INT8U reason);

ALGORITHM_RF_EXT INT8U Get__Reason_Dis_5117(INT8U path4);
ALGORITHM_RF_EXT INT8U Get__Reason_SD_5115(INT8U path4);


ALGORITHM_RF_EXT INT8U Get__Reason_Dis_5117_Ori(INT8U path4);
ALGORITHM_RF_EXT INT8U Get__Reason_SD_5115_Ori(INT8U path4);


ALGORITHM_RF_EXT void Clr__Reason_Dis_5117(INT8U path4);
ALGORITHM_RF_EXT void Clr__Reason_SD_5115(INT8U path4);

ALGORITHM_RF_EXT void Clr__Reason_Dis_5117_Prev(INT8U path4);
ALGORITHM_RF_EXT void Clr__Reason_SD_5115_Prev(INT8U path4);

ALGORITHM_RF_EXT char* Get__Str_Dis_5117(INT8U reason);
ALGORITHM_RF_EXT char* Get__Str_SD_5115(INT8U reason);

/*----------------------------------------------------------*/


ALGORITHM_RF_EXT INT8U Is_AC_Fail(void);
ALGORITHM_RF_EXT void AMP_Stdby_AMP_Ctrl_Chk(void);

ALGORITHM_RF_EXT INT8U Amp_Stdby_Use(INT8U path4);

/********************************************************************************/
#endif
/********************************************************************************/
