/********************************************************************************/
#ifndef __AUTO_CONFIG_PROC_H
#define __AUTO_CONFIG_PROC_H
/********************************************************************************/
#ifdef  AUTO_CONFIG_PROC_GLOBALS
#define AUTO_CONFIG_PROC_EXT
#else
#define AUTO_CONFIG_PROC_EXT extern
#endif
/**************************************************************/
//def
#define _TYPE_LS_SS_		0xEA
#define _TYPE_LS_ELG_		0xEC
#define _TYPE_LS_NKA_		0xEE

///////////////////////////////////////////////////////////////////////////////////////////////////////
//E
enum
{
	AUTUCONFIG_READY = 0x00, 	 // Auto_Config_Ready,
	AUTUCONFIG_START,		 	 // Auto_Config_Start,	

	AUTUCONFIG_STEP01,			 // LosLof_Alarm_Check,	
	AUTUCONFIG_STEP02,			 // VSS_Decoding,	
	AUTUCONFIG_STEP03,			 // LSH_TypeCheck,	
	AUTUCONFIG_STEP04,			 // IP_Assin_Process,	
	AUTUCONFIG_STEP05,			 // Initial_Noti_Procerdure,
	AUTUCONFIG_STEP06,			 // Delay_Auto_Setting,	
	AUTUCONFIG_STEP07,		 	 // TDD_Config,
	AUTUCONFIG_STEP08,			 // Tx_Out_Alc,
	
	AUTUCONFIG_FINISH, 			 // Auto_Config_Finish,
	
}E_AUTO_CONFIG_STEP;

enum
{
	IP_ASSIGN_READY = 0x00, 	// Ip_Assign_Ready,
	IP_ASSIGN_START,		 	// RU_IP_NOTI_REQ,	
	
	IP_ASSIGN_CHECK,			// RU_IP_NOTI_RSP_Check,	
	IP_ASSIGN_PROCESSING,		// IP Change,	

	IP_ASSIGN_FINISH, 			// IP_ASSIGN_FINISH,
	
}E_IP_ASSIGN_STEP;

enum
{
	INIT_NOTI_READY = 0x00, 	// INIT_NOTI_Ready,
	INIT_NOTI_START,		 	// RU_IP_NOTI_REQ,	
	
	INIT_NOTI_CHECK,			// RU_IP_NOTI_RSP_Check,	
	INIT_NOTI_PROCESSING,		// IP Change,	

	INIT_NOTI_FINISH, 			// INIT_NOTI_FINISH,

	INIT_NOTI_END,
	
}E_INIT_NOTI_STEP;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//st

typedef struct  __attribute__((packed))
{
	//from VSS
	//DL
	INT8U RECTypeID;
	INT8U REC_Port_No;
	INT16U PCI_16;

	INT8U RE_Cascade_ID;

    INT8U	Recv_Ip[4];			// Noti Ip;
    INT8U	Recv_Ip2[4];		// Noti Ip2;
    INT16U  gREMS_Port16;       // gREMS port16

    INT8U	AutoConfig_Step;
	INT8U 	AutoConfig_Default_Flag;


	// 2019.04.04
	INT8U CnM_Path_Sel_Val;
	INT8U CnM_Path_Sel_Flag;
	INT8U CnM_Path_Sel_Cnt;
	INT8U CnM_Path_Sel_Send_Cnt;

	// 2019.04.04
	ALM_ST Alm_Prev;
	INT8U Alm_Val[ALM_MAX];
	INT8U Alm_Flag;
	INT8U Alm_Cnt;
	INT8U Alm_Send_Cnt;


	//ip Assign
    INT8U	Ip_Assign_Step;
    INT8U	Ru_Ip_Noti_Cnt;

    INT8U	Base_Ip[4];			// from VSS
    INT8U	Base_Ip_Prev[4];	// previous value
    INT8U	Base_Ip_Changed;
    INT8U	Base_Ip_Changed2;
    
    INT8U	Base_Mac_Vss[6];	// from VSS
    INT8U	Base_Mac_Vss_Prev[6];	// previous value
    INT8U	Base_Mac_Changed;
    INT8U	Base_Mac_Changed2;
    
    INT8U	Base_Mac[6];		// 
    INT8U	Base_Mac_Cal[6];	// calculation
    INT8U   Base_Is_Stable;     // 2019.05.03

	INT8U  LS_Regist_Num[20];		// GUI Set, L/S 등록번호(AID 0xB007), 변경 : 'Serial_Num' -> 'LS_Regist_Num'	
	INT8U  RU_Board_Name[32];   // GUI Set

	INT8U GW0[4];
	INT8U NetMask0[4];
	
    INT8U	RU_Id;
    INT8U	RU_Id_Alloc; // 0x01 if RU_Id is Allocated. 2019.05.08

	// Init_Noti
    INT8U	Init_Noti_Step;
    INT8U	Init_Noti_Cnt;

	/*
	Init_Status
	----------------
		0x00 : Initialization Noti Ack 미수신
		0x01 : Initialization Noti Ack 수신
	*/
	INT8U	Init_Status;
	INT8U   Cpri_cnt;

	INT8U	Alarm_Acknowledge; // if not_ack, L/S will dismiss all alarm from RU.
	INT8U   LS_ID; // 2019.05.20

	/*
	if "Recv_FX_INIT_NOTI_ACK" received, then 1.
	else 60[s] timeout, then 1.
	*/
	INT8U   Vaild_Indicator; // 2019.06.27

} ST_Auto_Config_Flag;



///////////////////////////////////////////////////////////////////////////////////////////////////////

AUTO_CONFIG_PROC_EXT ST_Auto_Config_Flag AufoConfigFlag;

AUTO_CONFIG_PROC_EXT void* Auto_Config_Task(void *data);


AUTO_CONFIG_PROC_EXT void Copy_Data_From_VSS(void);
AUTO_CONFIG_PROC_EXT void AutoConfig_Flag_Clear(void);
AUTO_CONFIG_PROC_EXT void Check_CnM_Path_Change_Clear(void);


AUTO_CONFIG_PROC_EXT void Init_Base_IP_Mac_Chk(void);
AUTO_CONFIG_PROC_EXT void Timer_start_LS_Valid_Indicator(void);
AUTO_CONFIG_PROC_EXT char* Get_REC_Str(INT8U RCE_Type);
AUTO_CONFIG_PROC_EXT void Proc__PRU_Set_MAC_Addr(void);

/*
------------------------------------------------------------------------
	0x00: (1/6) System Initialization
	0x01: (2/6) RE ID Acquisition Notification ACK waiting
	0x02: (3/6) Initialization Notification  ACK waiting
	0x03: (4/6) CPRI Status Request waiting
	0x04: (5/6) Delay Setup Request waiting
	0x05: (6/6) Initialization Complete
------------------------------------------------------------------------
*/
enum // AID 0x500D
{	
	Init_St_System_Init = 0, // step 1/6
	Init_St_RE_ID_Acqui,     // step 2/6
	Init_St_Init_Notify,     // step 3/6, RU_ID Allocation End, Waiting UTC Time..
	Init_St_CPRI_St_Req,     // step 4/6
	Init_St_Dly_Set_Req,     // step 5/6
	Init_St___Completed,     // step 6/6, 초기 부팅 후 amp off 주장비사와 초기화 완료 후, amp on
	Init_St_Max
};


AUTO_CONFIG_PROC_EXT INT8U Get___RU_Initial_Status(void);
AUTO_CONFIG_PROC_EXT void Set___RU_Initial_Status(INT8U st);
AUTO_CONFIG_PROC_EXT INT8U IS_RU_Init_Completed(void);
AUTO_CONFIG_PROC_EXT INT8U IS_User_AMP_ON_Control(INT8U path4);
AUTO_CONFIG_PROC_EXT INT8U IS__TimeOver_Id_Mismatch_Alm(void);
AUTO_CONFIG_PROC_EXT void Clr__TimeOver_Id_Mismatch_Alm(void);



AUTO_CONFIG_PROC_EXT void Update__gREMS_IP(INT8U* ip);
AUTO_CONFIG_PROC_EXT void Update__Layer_IP(INT8U* ip);

AUTO_CONFIG_PROC_EXT void Load__Noti_IP(void);
AUTO_CONFIG_PROC_EXT void Set__Defult_Noti_IP(INT8U* noti_ip);
AUTO_CONFIG_PROC_EXT INT8U Get_REC_Idx(INT8U RCE_Type);


/********************************************************************************/
#endif
/********************************************************************************/


