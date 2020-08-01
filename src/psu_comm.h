/********************************************************************************/
#ifndef __PSU_COMM_H
#define __PSU_COMM_H
/********************************************************************************/
#ifdef  PSU_COMMM_GLOBALS
#define PSU_COMM_EXT
#else
#define PSU_COMM_EXT extern
#endif
/**************************************************************/

enum
{
	PSU_CMD_ST        = 0x50,
	PSU_CMD_INFO      = 0x57,
	PSU_CMD_INFO_Ctrl = 0x58,

	PSU_CMD_DN_NOTI     = 0x70, // 1,2�� �뺸.
	PSU_CMD_DN_NOTI_CHK = 0x71, // �뺸 Ȯ��.
	PSU_CMD_DN_DATA     = 0x72, // �ٿ�ε�
	PSU_CMD_DN_END      = 0x73, // Ȯ��

	PSU_CMD_DN_NOTI_FW = 0x74, // 1,2�� �뺸, FW ����.
	PSU_CMD_FW_APPLY   = 0x75, // FW ����.


	PSU_CMD_Green_Mode_Ctrl = 0x77, // Green Mode On/Off
	PSU_CMD_PSU_Reset_Ctrl  = 0x78, // PSU Reset
	PSU_CMD_FAN_Speed_Ctrl  = 0x79, // FAN �ӵ� ����
	PSU_CMD_Pwr_Reset_Ctrl  = 0x7A, // ���·� �ʱ�ȭ
	
	PSU_CMD_BattCharge_Ctrl = 0x7B, // Battery ���� On/Off
	PSU_CMD_PSU_FAN_Ctrl    = 0x7C, // PSU_FAN ����
};

//----------------------------------------------------------
// Structure of 
// PSU Communication Protocol ----------
typedef struct __attribute__((packed))
{
	//-- PSU Alarm
	INT8U   PSU_Arm[5];
	
	//-- PSU Status
	INT8U	AC_In_Volt[4];		// 4  �Է�����
	INT8U	DC_Out_Volt_50[4];	// 4  �������(50.2v)
	INT8U	DC_Out_Volt_5_8[4];	// 4  �������(5.8v)
	INT8U	DC_Out_Volt_24[4];	// 4  �������(24v)
	INT8U	Batt_Volt[4];		// 4  Battery �������
	//-- PSU FAN
	INT8U	FAN_On_Temp[2];	    // 2, PSU FAN-On -Temp
	INT8U	FAN_OffTemp[2];	    // 2, PSU FAN-Off-Temp
	INT8U	FAN_OnOff;	
	INT8U	FAN_Auto;	
	
	//-- PSU Reserved
	INT8U   reserved_1[2];		// 2  Reserved
	INT8U   reserved_2[4];		// 4  Reserved
	
	//-- PSU Out Current
	INT8U	Load_Current_A[4];  // 4  �������(50.2v)

	//-- PSU Reserved
	INT8U   reserved_3[4];		// 4  Reserved
	INT8U   reserved_4[4];		// 4  Reserved
	INT8U   iro_remote_type[2];	// 2  iRO Remote Type ����
	INT8U   pru_remote_type[2];	// 2  PRU Remote Type ����
	INT8U   Compatibility;		// compatibility, 2020.02.14, (0x5A is compatible, 0xFF not compatible)
	INT8U   reserved_6[7];		// 7  Reserved, 2020.02.17 : '8' -> '7'
	
	//-- PSU etc
	INT8U   PSU_Temp[4];		// 4  PSU �µ�
	INT8U   FAN_RPM[4];			// 4  FAN RPM
	INT8U   SYS_Time[12];		// 12 �ð�����

	//-- PSU Wattage
	INT8U   Watt_Tot_Accu[8];	// 8  �� ���� ���·�
	INT8U   Watt_real_time[4];	// 4  �ǽð� ���·�
	INT8U   Watt_hour_aver[4];	// 4  1�ð�������·�
	INT8U   Watt_day_aver[4];	// 4  1��������·�
	INT8U   Watt_week_aver[4];	// 4  1����������·�
	INT8U	Watt_month[12][4];	// 4  1��~12�� ���·�

	//-- PSU download
	INT8U   DL_St[2];			// PSU D/L State	2
} PSU_ST_STR;
//---------------------------------------------------------------------

PSU_COMM_EXT PSU_ST m_PSU;


enum
{
	FAN_Mode____Normal = 0,
	FAN_Mode_Low_Noise,
	FAN_Mode_OFF, // AAU Only
};



#define PSU_DN_DATA_MAX  (1020) // 2020.03.25 : '520' -> '1020'

typedef struct  __attribute__((packed))
{
	INT8U st;
	INT8U data[PSU_DN_DATA_MAX]; // max size
	INT8U retry;
	INT16U tot_frame;
	INT16U cur_frame;
	INT16U crc;
	INT8U result;
	INT8U req;
	INT8U percent;
	INT8U fw_apply_req;
	int tot_size;
}PSU_Auto_DN;

PSU_COMM_EXT PSU_Auto_DN m_AutoDn_Psu;
PSU_COMM_EXT INT8U PSU_Link_Fail_Co;
PSU_COMM_EXT INT8U PSU_Link_Fail;
PSU_COMM_EXT INT8S PSU_Init_End;
PSU_COMM_EXT INT8U PSU_Comm_Cnt; // 2020.04.08



PSU_COMM_EXT INVENTORY_2_STR PSU_Info;



PSU_COMM_EXT void PSU_Init_Set(void);
PSU_COMM_EXT INT8U Is_PSU_CommFail(void);
PSU_COMM_EXT INT8U Is_PSU_CommFail_Co(void);
PSU_COMM_EXT INT8U Is_PSU_MultiDown(void);

PSU_COMM_EXT INT16U PSU_DownLoad_PassData(INT8U type, PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf);
PSU_COMM_EXT INT8U PSU_FW_APPLY( INT8U* data, INT8U len );

PSU_COMM_EXT INT8U PSU_Get_Info( void ); // PSU inventory
PSU_COMM_EXT void Psu_Inv_Read_Req(void);
PSU_COMM_EXT void Init_Psu_Down_St(void);

PSU_COMM_EXT void Green_Mode_Ctrl( INT8U mode_on );
PSU_COMM_EXT INT8U PSU_5G_Green_Mode_Ctrl( INT8U en );
PSU_COMM_EXT INT8U PSU_FAN_Speed_Ctrl( INT8U Fan_Speed );
PSU_COMM_EXT INT8U PSU_PSU_FAN_Ctrl( INT8U FAN_Auto, INT8U FAN_OnOff, INT8S FAN_On_Temp, INT8S FAN_OffTemp );
PSU_COMM_EXT INT8U PSU_Reset_Ctrl( void ); // PSU Reset
PSU_COMM_EXT INT8U PSU_Pwr_Reset_Ctrl( void ); // ���·� �ʱ�ȭ
PSU_COMM_EXT INT8U PSU_BattCharge_Ctrl( INT8U en );
PSU_COMM_EXT INT8U sendPsu_INFO_Ctrl( INT8U* pCtrlVal, INVENTORY_STR* inv );

PSU_COMM_EXT INT8U Get_Result_of_Batt_Diagnosis(void);



PSU_COMM_EXT INT8U Is_Ext_FAN_Alm(void);
PSU_COMM_EXT INT8U Get_PSU_AMP_SW_St(INT8U path4);

PSU_COMM_EXT void Update_PSU_DL_State(void);

PSU_COMM_EXT void Init_Psu_AutoDn(void);
PAU_COMM_EXT void* Psu_Comm_Task(void *data);
PSU_COMM_EXT void Use_Batt_Use( INT8U use_val );

PSU_COMM_EXT int Load_RAM_DnImg_PSU(char * pullFilePath);

PSU_COMM_EXT INT8U Is_Ext_FAN_Mode_Time(void);
PSU_COMM_EXT INT8U Is_Ext_FAN_Low_Noise_Mode_for_24Hour(void);

PSU_COMM_EXT void PSU_Down_Fail(void );		//// added YCSO 190920 // 2019.09.20

PSU_COMM_EXT INT8U IS__PSU_Compatible(void);
PSU_COMM_EXT INT8U IS__PSU_giRO(void);

PSU_COMM_EXT INT8U IS___file_ext___psu(void);


#ifdef __TYPE__PRU__ // 5G-PRU-Only 
PSU_COMM_EXT INT8U Get_Repeater_Type(void); // 2020.07.10
#endif


/********************************************************************************/
#endif
/********************************************************************************/






