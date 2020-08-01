/********************************************************************************/
#ifndef __LSH_UDP_COMM_H
#define __LSH_UDP_COMM_H
/********************************************************************************/
#ifdef  LSH_UDP_COMM_GLOBALS
#define LSH_UDP_COMM_EXT
#else
#define LSH_UDP_COMM_EXT extern
#endif
/**************************************************************/

#define MAX_LSH_SYNC_SIZE						3				// 0x10 0x00 0x00 
#define MAX_LSH_HEADER_SIZE						9				// MessageID 1 + Len 4 + cnt 4 = 9 Byte

#define MAX_LSH_IE_FRAME_SIZE					8				// IE_ID 2 + IE_LEN 2 + IF_CNT 2 + IF_LEN 2 = 8 Byte
#define MAX_LSH_IE_EXT_FRAME_SIZE				12				// IE_ID 2 + IE_LEN 2 + IF_CNT 4 + IF_CNT 2 + IF_LEN 2 = `1 Byte
// 2019.04.18 : L/S
#define MAX_LSH_IE_DELAY_REQ_FRAME_SIZE			10				// IE_ID 2 + IE_LEN 2 + IF_CNT 2 + IF_CNT 2 + IF_LEN 2 = `1 Byte




#define MAX_LSH_IE_PRU_ALM_CNT						17				
#define MAX_LSH_IE_AAU_ALM_CNT						15				

/*
// FX Id ----------------------------------------------------------------------- 
// MessageID 0x100000XX
*/
#define FX_RUID_ACQU_NOTI						0x9A // to Layer Splitter
#define FX_RUID_ACQU_NOTI_ACK					0x9B

#define FX_INIT_NOTI							0x02 // to Layer Splitter
#define FX_INIT_NOTI_ACK						0x03

#define FX_SYSTEM_STS_REQ						0x10
#define FX_SYSTEM_STS_RSP						0x11 // to Layer Splitter

#define FX_ALM_INFO_NOTI						0x26 // to Layer Splitter --------- needed
#define FX_ALM_INFO_ACK							0x27 // ------ needed

#define FX_ALM_INFO_REQ							0x28
#define FX_ALM_INFO_RSP							0x29 // to Layer Splitter

#define FX_RESET_REQ							0x34
#define FX_RESET_RSP							0x35 // to Layer Splitter -------- needed

#define FX_DELAY_SET_REQ						0x5C
#define FX_DELAY_SET_RSP						0x5D // to Layer Splitter

#define FX_TIME_SYNC_REQ						0x60 // deleted.. not used..
#define FX_TIME_SYNC_RSP						0x61 // to Layer Splitter, deleted.. not used..

#define FX_CnM_SW_NOTI							0x98 // to Layer Splitter -------- needed
#define FX_CnM_SW_NOTI_ACK						0x99 // -------- needed

#define FX_CPRI_STS_REQ							0x1C		
#define FX_CPRI_STS_RSP						    0x1D // to Layer Splitter


// 2019.04.30
#define FX_FA_STS_REQ							0x14		
#define FX_FA_STS_RSP						    0x15 // to Layer Splitter

// 2019.04.30
#define FX_PATH_STS_REQ							0x18		
#define FX_PATH_STS_RSP						    0x19 // to Layer Splitter


/*
	Port16 
	for FX-Id ----------------------------------------------------------------------- 
*/

#define LSH_Port_0x5662 0x5662 // RU -> LS
#define LSH_Port_0x5071 0x5071 // RU -> LS





/*
// IE Id ----------------------------------------------------------------------- 
*/
#define IE_Lower_Node_Id_Config					0x5900
#define IE_System_Sts							0x0200 // 2019.04.18 : L/S
#define IE_Initial_Information					0x0D00
#define IE_Initial_Information_Ack				0x0E00
#define IE_ALM									0x1000
#define IE_ALM_Data_Vaild_Indicator				0x1100
#define IE_Time_Tick							0x2d00 // 2019.04.11
#define IE_System_Topology						0x2200 // 2019.04.18 
#define IE_Reset								0x3000
#define IE_Delay_Value							0x2B00 // 2019.04.30 : 0x31 --> 0x2B 
#define IE_Measured_Power						0x3200
#define IE_Optic_Delay  						0x3400

#define IE_TSSI_RSSI  						    0x0B00 // 2019.04.30. by haes
#define IE_Path_Status  						0x0700 // 2019.04.30. by haes



///////////////////////////////////////////////////////////////////////////////////////////////////////
//E

enum
{
	ALM_UDA =0,
	ALM_FAN,
	ALM_TEMPERATURE_HIGH,
	ALM_TEMPERATURE_LOW,
	ALM_TEMPERATURE_HIGH_MINOR,
	ALM_VOLTAGE_HIGH,
	ALM_VOLTAGE_LOW,
	ALM_VOLTAGE_LOW_MINOR,
	ALM_CLOCK_FAIL,
	ALM_CPRI_FAIL,
	ALM_OPTIC_TRANSCEIVER_RX_LOS,
	ALM_OPTIC_TRANSCEIVER_TX_FAULT,
	ALM_DISABLED,
	ALM_LOW_GAIN,
	ALM_OVERPOWER,
	ALM_VSWR_FAIL,
	ALM_SHUTDOWN,
	ALM_DIGITALM_INPUT_HIGH,
	ALM_RX_PATH_FAIL,
	ALM_DIGITALM_INPUT_LOW,	
	ALM_RSSI_HIGH,
	ALM_RSSI_LOW,
	ALM_CPRI_RDI,

	ALM_MAX,
};

enum
{
	E_ALM_UDA 							= 202,
	E_ALM_FAN 							= 210,
	E_ALM_TEMPERATURE_HIGH 				= 216,
	E_ALM_TEMPERATURE_LOW 				= 219,
	E_ALM_TEMPERATURE_HIGH_MINOR 		= 244,
	E_ALM_VOLTAGE_HIGH 					= 344,
	E_ALM_VOLTAGE_LOW 					= 345,
	E_ALM_VOLTAGE_LOW_MINOR 			= 349,
	E_ALM_CLOCK_FAIL					= 351,
	E_ALM_CPRI_FAIL 					= 376,
	E_ALM_OPTIC_TRANSCEIVER_RX_LOS 		= 378,
	E_ALM_OPTIC_TRANSCEIVER_TX_FAULT 	= 379,
	E_ALM_DISABLED 						= 385,
	E_ALM_LOW_GAIN 						= 389,
	E_ALM_OVERPOWER 					= 391,
	E_ALM_VSWR_FAIL 					= 392,
	E_ALM_SHUTDOWN 						= 393,
	E_ALM_DIGITE_ALM_INPUT_HIGH 		= 395,
	E_ALM_RX_PATH_FAIL 					= 396,
	E_ALM_DIGITE_ALM_INPUT_LOW 			= 408,
	E_ALM_RSSI_HIGH						= 609,
	E_ALM_RSSI_LOW 						= 610,
	E_ALM_CPRI_RDI 						= 447,

	E_ALM_MAX,
};


enum
{
	SC_RUnRAU = 0,
	SC_Path,
	SC_FAnSplit,
	SC_PathFASplit,
	SC_UDA,
	SC_ClippingLocation,
	SC_CPRI,
	SC_UpDownConverterBlock,

	SC_MAX,
};

enum
{
	SV_Indeterminate = 0,
	SV_Critical,
	SV_Major,
	SV_Minor,
	SV_Warnig,
	SV_Normal,

	SV_MAX,
};


enum
{
	OP_100M = 1,
	OP_a___,

	OP_MAX,
}; // 2019.04.18

///////////////////////////////////////////////////////////////////////////////////////////////////////
//st

typedef struct __attribute__((packed))
{
    INT8U	STX[3];						// Start 3Byte - 0x10 0x00 0x00
    INT8U	Type;						// Start 1Byte - Message Id
    INT32U	len;						// Message Len - 4Byte 
	INT32U	SeqNum;					    // Sequence Number - 4Byte
    INT8U	data[MAX_COM_BUFFER];		// Check return_rst - 4Byte
} LSH_PROTOCOL_FRAME;

typedef struct __attribute__((packed))
{
	type16 IE_Id;
	type16 IE_Len;
	type16 IF_Cnt; // number of IF
	type16 IF_Len;

} ST_IE_FRAME;

//Alarm/Cpri rsp
typedef struct __attribute__((packed))
{
	type16 IE_Id;
	type16 IE_Len;
	type32 Item_Cnt;
	type16 IF_Cnt; // number of IF
	type16 IF_Len;

} ST_IE_FRAME_EXT;

typedef struct __attribute__((packed))
{
	type16 IE_Id;
	type16 IE_Len;
	type32 CPRI_Cnt; // Number of CPRI
	type16 IF_Cnt; // number of IF
	type16 IF_Len;

} ST_IE_OPTIC_DELAY_FRAME;


#define IF_LOWER_NODE_ID_CONFIG_CNT 6
typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type16 RU_Id;
	type16 Cascade_Id;
	type32 Base_IP;
	INT8U Seial_Number[10];
	type16 Dummy;
	INT8U RU_Board_Name[16];
	///////////////////////////////////////

}ST_Lower_Node_Id_Config;

#define IF_INITIAL_INFORMATION_CNT 5
typedef struct __attribute__((packed))
{
	///////////////////////////////////////
	type16 FW_Mode;
	type16 Log_Avail;
	type32 Cascade_Id;
	type32 FW_Version;
	type32 IF_Version;
	///////////////////////////////////////

}ST_Initial_Information;

#define IF_INITIAL_INFORMATION_ACK_CNT 6
typedef struct __attribute__((packed))
{
	///////////////////////////////////////
	type32 Time_Tick;
	type16 System_Id;
	type16 Alm_Ack;
	type32 Time_Tick_Offset;
	type16 Reserved;
	type16 Dummy;
	///////////////////////////////////////

}ST_Initial_Information_Ack;

#define IF_ALM_DATA_VALID_CNT 1  // 2019.04.11
typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type32 Vaild_Indicator;;
	///////////////////////////////////////

}ST_ALM_Data_Vaild_Indicator;

typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type16 Alm_Code;
	type16 Alm_Subcode_Type;
	type32 Alm_Subcode;				//??
	type32 Alm_Severity;
	type32 Alm_Flag;
	char Add_Information[20];
	///////////////////////////////////////

}ST_ALM;

typedef struct __attribute__((packed))
{
	// 0000 : Not Used
	// 0001 : Normal
	// 0010 : Minor Alarm
	// 0011 : Major Alarm
	// 0100 : Comm. Fail
	// 0101~1111 : Reserved

	//0:7
	union{
		INT8U Data;
		struct{

			INT8U Sts				:4; 	
			INT8U 					:2; 	
			INT8U Type				:2; 	
			
		}Bit;
	}Split_Hub_Status;

	INT8U Reserved0;

	// 0:7
	union{
		INT8U Data;
		struct{

			INT8U Sts				:4; 	
			INT8U 					:2; 	
			INT8U Type				:2; 	
			
		}Bit;
	}RE_BR[16];

	INT8U Reserved1[2];
	
}ST_System_Topology;

#define IF_SYSTEN_STAUS_CNT 22  // 2019.04.11
typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	INT8U RU_Init_Sts;
	INT8U Cascade_Id;
	INT8U Reserved0;
	INT8U FW_Mode;
	type32 System_Temp;
	type32 IF_Version;
	type32 Supported_OFR_Start_Frequency;
	type32 Supported_OFR_Bandwidth;
	type32 Supported_IBW_Bandwidth;
	type32 DC_In_Volt;
	type32 DC_Out_Volt;
	type32 Tx_Filter_Center_Frequency;
	type32 Rx_Filter_Center_Frequency;
	INT8U Tx_Filter_Bandwidth;
	INT8U Rx_Filter_Bandwidth;
	type16 Battery_Equipment_Status;
	type32 FAN_Equipment_Status;
	type16 FAN1_Speed;
	type16 FAN2_Speed;
	type16 FAN3_Speed_Reserved;  // 2019.04.11
	type16 FAN4_Speed_Reserved;  // 2019.04.11
	type32 Reserved1;

	#if 0
	type16 Cascade_Delay;
	type16 Toffset;
	type32 TBdelatDL;
	type32 TBdelatUL;
	#endif
	///////////////////////////////////////

}ST_System_Sts;

#define IF_TIME_TICK_CNT 4
typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type32 Time_Tick;	//utc
	type32 Time_Tick_Offset;	//local
	type16 Reserved;
	type32 Dummy;
	///////////////////////////////////////

}ST_Time_Tick;

typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type32 Delay_Time;
	///////////////////////////////////////

}ST_Reset;

#define IF_DELAY_VALUE_CNT 6 // 2019.04.18, 3 -> 6
typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type32 CPRI_Id;
	
	type32 DL_Delay;
	type32 UL_Delay;
	///////////////////////////////////////

}ST_Delay_Value;

#define IF_MEASURED_POWER_CNT 14
typedef struct __attribute__((packed))
{

	///////////////////////////////////////
	type32 Path_A_TSSI_IQ_Level;
	type32 Path_B_TSSI_IQ_Level;
	type32 Path_C_TSSI_IQ_Level;
	type32 Path_D_TSSI_IQ_Level;
	type32 Reserved0;
	type16 Path_A_DL_Output_Power_Level;
	type16 Path_B_DL_Output_Power_Level;
	type16 Path_C_DL_Output_Power_Level;
	type16 Path_D_DL_Output_Power_Level;
	type32 Reserved1;
	type16 Path_A_RSSI_Level;
	type16 Path_B_RSSI_Level;
	type16 Path_C_RSSI_Level;
	type16 Path_D_RSSI_Level;
	///////////////////////////////////////

}ST_Measured_Power;

#define IF_OP_DELAY_VALUE_CNT (9) // Number of Information Field, 2019.05.10 : 7 -> 9
typedef struct __attribute__((packed))
{
	///////////////////////////////////////
	type32 CPRI_Id;
	type32 OP_Delay; // Round Trip Delay, 0~210000
	type32 Toffset;  // '0~9999
	type32 DL_Hopping_Delay; // 0~19999
	type32 UL_Hopping_Delay; // 0~19999
	type32 Tx_Delay; // 0~500000
	type32 Rx_Delay; // 0~500000

	type32 Tx_Delay_not_Ru_dly; // 2019.05.10, Tx Delay (excluding RU proc.delay), 0~500000
	type32 Rx_Delay_not_Ru_dly; // 2019.05.10, Rx Delay (excluding RU proc.delay), 0~500000
	///////////////////////////////////////

}ST_Op_Delay_Value;


// 2019.04.30, by haes
#define IF_TSSI_RSSI_VALUE_CNT 6
typedef struct __attribute__((packed))
{
	///////////////////////////////////////
	type16 FA_Id;
	type16 Split_Id; // fixed with 0
	type16 Path_Id;
	type16 RSSI_dBm; // 0.1dB step
	type32 TSSI_IQ;  // Decimal value
	type32 RSSI_IQ;  // Decimal value
	///////////////////////////////////////
}ST_TSSI_RSSI_Value;

// 2019.04.30, by haes
#define IF_Path_Status_VALUE_CNT 6
typedef struct __attribute__((packed))
{
	///////////////////////////////////////
	type16 Path_Id; // 0 ~ 7
	type16 Ref_Sig_Num; // Reference_Signal_Number
	type16 Temp;	// Temperature, 0.1 step
	type16 Tx_RF_Pwr;	// Tx_RF_Power, 0.1 dB step
	type16 RU_Gain;	 // ¹ÌÁö¿ø. 0 fixed
	type16 Ret_Loss; // Return_Loss, 0.1 dB step, PRU only
	///////////////////////////////////////

}ST_Path_Status_Value;




typedef struct __attribute__((packed))
{
	///////////////////////////////////////
	type32 data;
	///////////////////////////////////////

}ST_SubCode; // 2019.04.18

///////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __attribute__((packed))
{
	//if allign
	ST_Lower_Node_Id_Config LowerNodeIdConfig;		//Splitter <-> RU, Common

	//inite config
	ST_Initial_Information_Ack InitInfo;			//Splitter -> RU

	//System Status Request
	ST_System_Topology SystemTopology;				//Splitter -> RU

	//System Status Response
	ST_System_Sts SystemSts;						//RU -> Splitter
	ST_Time_Tick TimeTick;							//RU -> Splitter
	ST_Measured_Power MeaPower;						//RU -> Splitter

	//Delay Setup Request
	ST_Delay_Value DelayValue[MAX_Optic];			//Splitter <-> RU, Common // 2019.04.18

	//Time Tick Request
	ST_Time_Tick rTimeTick;							//Splitter -> RU, Common

	ST_ALM Alm[ALM_MAX];

	ST_Op_Delay_Value Op_DelaySts;					//RU -> Splitter

}LSH_ST_STR;

///////////////////////////////////////////////////////////////////////////////////////////////////////


LSH_UDP_COMM_EXT INT8U GetFrame_LSH( COMM_QUE* pque, LSH_PROTOCOL_FRAME* pframe ); // 0:no data, 1:Data ?????, 2:Data ??? ???;


//////LSH_UDP_COMM_EXT void* LSH_UDP_Comm_Task(void *data); // 2019.05.07
LSH_UDP_COMM_EXT void Proc_LSH_Node_Pkt(void);

LSH_UDP_COMM_EXT INT8U Send_FX_RUID_ACQU_NOTI(INT8U* lsh_ip, INT32U seqnum);
LSH_UDP_COMM_EXT INT8U Send_FX_INIT_NOTI(INT8U* lsh_ip, INT32U seqnum);
LSH_UDP_COMM_EXT INT8U Send_FX_CnM_SW_NOTI(INT8U* lsh_ip, INT32U seqnum);
LSH_UDP_COMM_EXT INT8U Send_FX_ALM_INFO_NOTI(INT8U* lsh_ip, INT32U seqnum);

LSH_UDP_COMM_EXT LSH_ST_STR m_LSH_Sts;

LSH_UDP_COMM_EXT INT8U Send_FX_ALM_INFO(INT8U type, INT8U* lsh_ip, INT16U port16, INT32U seqnum);
LSH_UDP_COMM_EXT void Proc___Ctrl_from_LS(void);


/********************************************************************************/
#endif
/********************************************************************************/


