/********************************************************************************/
#ifndef __ALARM_5G_H
#define __ALARM_5G_H
/********************************************************************************/
#ifdef  ALARM_5G_GLOBALS
#define ALARM_5G_EXT
#else
#define ALARM_5G_EXT extern
#endif
/**************************************************************/


/*----------------------------------------------------------*/
// 외부 호출 함수
//
ALARM_5G_EXT void RepeaterInit( void );
ALARM_5G_EXT void Disp_Version_Console(void);
ALARM_5G_EXT INT8U Check_PAU_Use( INT8U path4 );

ALARM_5G_EXT void Reset_Local_Timer(void);
ALARM_5G_EXT void Update_GUI_Alm(void);
ALARM_5G_EXT void SetResetInfo( INT32U rst_info, int idx );
ALARM_5G_EXT void Update__Task_Count(INT8U i);
ALARM_5G_EXT INT8U IS__Task_Count_Update(INT8U i);
ALARM_5G_EXT void Proc___Task_Working(void);
/*----------------------------------------------------------*/

ALARM_5G_EXT INT8U Local_Terminal;
ALARM_5G_EXT INT8U BlueT_Terminal; // bluetooth

ALARM_5G_EXT INT8U PAU_FWD_AMP_St( INT8U path4 );
ALARM_5G_EXT INT8U AMP_Alarm_St(INT8U path);
ALARM_5G_EXT void Disp_AMP_St(INT8U path4);

ALARM_5G_EXT void Disp_version_only(void);
/*----------------------------------------------------------*/



// System Time
ALARM_5G_EXT INT8U Is_RU_Time_Sync; // sync with Layter Splitter

ALARM_5G_EXT INT8U rcu_reset_req;
ALARM_5G_EXT INT8U Chk_Sum_Arm(void);
ALARM_5G_EXT void Chk_Alarm_JIG_Timeout(void);
ALARM_5G_EXT OS_TIMER Alarm_JIG_Timer;
ALARM_5G_EXT void Init_Local_Termial(void);

ALARM_5G_EXT void Update_PAU_HW_Insert(void);
ALARM_5G_EXT void Proc_DL_OutPwr_Cal(void);

ALARM_5G_EXT void ClearFwdSD_Mode( INT8U path4 );
ALARM_5G_EXT void AMP_Not_Use_Pwr_Chk(void);
ALARM_5G_EXT INT8U Is_SFP_Inserted(INT8U cpri);
ALARM_5G_EXT void Init_AMP_Install(void);

ALARM_5G_EXT INT8U Is_RU_SISO(void);
ALARM_5G_EXT INT8U Is_SISO_AMP(INT8U path4);
ALARM_5G_EXT INT8U Is_MIMO_AMP(INT8U path4);
ALARM_5G_EXT INT8U Get_AMP_SS_MM(INT8U path4);
ALARM_5G_EXT INT8U Get_AMP_Pair(INT8U path4);

///// YCSO added START
ALARM_5G_EXT void Load_POWER_Table( INT8U idx );
ALARM_5G_EXT void Load_ATT_Table( INT8U idx );
ALARM_5G_EXT void Load_AMP_TEMP_Table( INT8U idx );
ALARM_5G_EXT void Load_AMP_TEMP_Alpha_Table(INT8U idx);

ALARM_5G_EXT void Save_POWER_Table( INT8U idx );
ALARM_5G_EXT void Save_ATT_Table( INT8U idx );
ALARM_5G_EXT void Save_AMP_TEMP_Table( INT8U idx );
ALARM_5G_EXT void Save_AMP_TEMP_Alpha_Table(INT8U idx);

#ifdef __TYPE__AAU__
ALARM_5G_EXT void Reset_BT_Re_Init_Timer( void );	////// YCSO added 191204 
#endif
///// YCSO added END


ALARM_5G_EXT INT8U AC_Fail_TEST_PAU0_RST_PIN_Toggle_Count;

ALARM_5G_EXT void Init___PRU_PYH_IC(void);
ALARM_5G_EXT INT8U IS__Standby_Status(INT8U path4);
ALARM_5G_EXT INT8U IS__TSSI_Not_Checking(INT8U path4);

ALARM_5G_EXT void Alarm_Process( void );


enum
{
	_ye_=0,
	_mo_,
	_da_,
	_ho_=3,
	_mi_,
	_se_
};

ALARM_5G_EXT void def_IP_Info_Set(void);
ALARM_5G_EXT void Start_Alm_Rpt_Pause_Timer(void);

ALARM_5G_EXT void Init_Status_struct(void);

ALARM_5G_EXT void LoadEEP( void );
ALARM_5G_EXT void SaveEEP( void );

ALARM_5G_EXT void Net_Start( INT16U idx );
ALARM_5G_EXT void Net_Stop( void );

#ifdef __TYPE__AAU__
ALARM_5G_EXT void Set__AAU_ADC_Chk_En(INT8U endis);
#endif

ALARM_5G_EXT void Proc___Inventory_SN(void);
ALARM_5G_EXT void PRU_PSU_Reset_Reg_Ctrl(void);


/********************************************************************************/
#endif
/********************************************************************************/
/* EOF */
