/********************************************************************************/
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_
/********************************************************************************/
#ifdef  PROTOCOL_GLOBALS
#define PROTOCOL_EXT
#else
#define PROTOCOL_EXT extern
#endif
/**************************************************************/

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
// below will be deleted
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
// end of delet
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////





#define TICK_CO_SEC (1000) // iro ms timer
#define TICK_CO_MIN (60000)


//==============================================
#define		_UP_		0
#define		_DN_		1
#define		UPDN		2

//==============================================
#define		_FWD_		0
#define		_RVS_		1
#define		_LNA_		2
//==============================================
#define		_TX_		0
#define		_RX0_		1
#define		_RX1_		2
//==============================================
#define		_IN_		0
#define		_OUT_		1
//==============================================
#define		_LED_ON_		0
#define		_LED_OFF_		1
//==============================================
#define		_Reg_LED__ON	1
#define		_Reg_LED_OFF	0
//==============================================
#define     _Use_Disp_ 1
#define     _Not_Disp_ 0

//==============================================
// Voltage Control
#define		_HIGH_	1
#define		_LOW_	0
//==============================================
#define		ON	    0
#define		OFF	    1
#define     ON_OFF  2
//==============================================
#define		NOT_USE	0x00
#define		USE	0x01
//==============================================
#define		_AMP_ON_		0
#define		_AMP_OFF_		1
//==============================================
#define		_EN_		0
#define		_DIS_		1
//==============================================
#define		_TRUE_		1
#define		_FALSE_		0
//==============================================
#define 	_F_SET_ (1)
#define 	_F_CLR_ (0)
//==============================================
#define		_SET_	(1)
#define		_CLR_	(0)
//==============================================
#define		__Check__	(1)
#define		_UnCheck_	(0)
//==============================================
#define RET_FAIL  (0)
#define RET_OK    (1)
#define RET_None (-1)
//==============================================
#define RET_SUCCESS (0)
#define RET_Error   (-1)
//==============================================
#define CHK_FAIL (-1)
#define CHK_OK   (0)
//==============================================
#define AMP_USE (0x01) // 2020.02.03 : 0x0/2/3 : 미사용 0x1 : 사용





#define NODE_INFO_MAX_NAME_LEN	100

#define MAX_STATUS_DATA_LEN	2048
#define MAX_RU_STATUS_DATA_LEN	MAX_STATUS_DATA_LEN

#define EMHS_PKT_MAX_SIZE (2048)
#define _MHS_PKT_MAX_SIZE (2048)
#define _SHS_PKT_MAX_SIZE (2048)
#define __RO_PKT_MAX_SIZE (2048)


//---------------------------------
// 2015.03.02 : 13s -> 90s
#define RET_ON_Timeout_ms (90000) 
//---------------------------------
#define _DIG_ETHERNET_READY_ (0x5A)

//==============================================
// R-Code
#define _RCODE_ 		0x50 // 5G PRU/AAU, 2019.02.23.
// TREE_2019_0521
#define _RCODE_GUI 		0x90 // GUI-Serial 에서 수신 (GUI 보고용),  // packet to/from other-RU @ 5G
#define _RCODE_UDP 		0xC0 // GUI-UDP 에서 수신 (AAU : bluetooth ??) // packet to/from other-RU @ 5G
#define _RCODE_USB 		0xB0 // USB 에서 수신 (AAU : bluetooth ??)
// END : TREE_2019_0521


#define _RCODE_DEV 		0x80 // 장비 내부간 통신 용, 5G 미사용.
// TREE_2019_0521
#define _BR_RCODE_ 		0x80 // Broad, Alive Message -- 5G 미사용. because of direct-report
#define _AR_RCODE_ 		0xA1 // Alarm RCODE - Alarm, CF/nC, CF/C, 통계 자동 보고 -- 5G 미사용. because of direct-report
#define _RCODE_RCS 		0xA0 // IRCS 에서 수신 (IRCS 보고용) - RO 미사용 -- 5G 미사용. because of direct-report
// END : TREE_2019_0521



#define NOT_INSTALL	0x00
#define INSTALL		0x01

#define LINK_FAIL_CO	10

#define AID_NOT_INSTALL	0x00
#define AID_INSTALL		0x01

#define AID_ON	0x00
#define AID_OFF	0x01


#define _INT8U_MAX_INIT		0
#define _INT8U_MIN_INIT		0xFF

#define _INT8S_MAX_INIT		0x80
#define _INT8S_MIN_INIT		0x7F

#define _INT16U_MAX_INIT		0
#define _INT16U_MIN_INIT		0xFFFF

#define _INT16S_MAX_INIT		0x8000
#define _INT16S_MIN_INIT		0x7FFF

#define _INT32U_MAX_INIT		0x0
#define _INT32U_MIN_INIT		0xFFFFFFFF

//==============================================
#define REPEATER_TYPE_1		   (0x02)
//------------------------------------
#define REPEATER_TYPE_5G_PRU     (0xA2)
#define REPEATER_TYPE_5G_AAU_C   (0xA3) // Ceiling Type
#define REPEATER_TYPE_5G_AAU_W   (0xA4) // Wall Type
#define REPEATER_TYPE_PRU_gIRO   (0xA5) // 2020.07.01
#define REPEATER_TYPE_PRU_gIRO_T (0xA6) // 2020.07.01
//==============================================
#define TYPE_5G_PRU    (0x07)
#define TYPE_5G_AAU    (0x07)
//==============================================
#define MAKER_ANTS     (0x26)
#define MAKER_HFR      (0x10)
//==============================================


//==============================================
// PRU/AAU Alarm Max Count
#define RU_ALARM_CNT	(27) // 2019.03.13

//==============================================
#ifdef __OLD_AID__
#define AID_MAX_Data (255) // 1byte, 0~0xFF
#else
#define AID_MAX_Data (512) // 2byte, 0~0xXXXX
#endif
//----------------------------------------
typedef struct __attribute__((packed))
{
	//------------ header 3 byte
	INT8U AID[2];
	INT8U len;

	//------------ data max 255 byte
	INT8U data[AID_MAX_Data];
} AIDSTR;
//----------------------------------------

typedef struct __attribute__((packed))
{
	INT16U tot__len; // total, AID ~ data
	//------------
	INT16U aid2_len; // type ~ data
	INT16U data_len; // data only
	INT16S flag_len; // flag only	
	//------------	
	INT8U size;
	INT8U layer;
	INT8U carrier;
	INT8U reserved;
} AID2_INFO;

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
	//------------ flag + data
	INT8U data[AID_MAX_Data]; // flag(N) +  (size) x (layer) x (carrier)
	//------------
} AIDSTR2;




#define AID2_AID_LEN_Size        4 // AID(2) + len(2)
#define AID2__Type_Carrier_Size  6 // type/step/reserved/size/layer/carrier

/*
AIDSTR_2 Size = AID2_AID_LEN_Size(4) + AID2__Type~Carrier_Size(6) + flag_data_len
data_only_len = aid.len - AID2__Type_Carrier_Size - flag_len
*/

PROTOCOL_EXT INT8U AID2_Chk_Size(INT8U size);
PROTOCOL_EXT INT8U AID2_Chk_Layer(INT8U layer);
PROTOCOL_EXT INT8U AID2_Chk_Carrier(INT8U carrier);

//----------------------------------------

///// YCSO TABLE added 
#define MAX_AMP_TEMP_COUNT		(27)	/// real : LEN 23 + header(3)	23 --> 27 190520
#define MAX_ATT_TBL_COUNT		(64)	/// real : LEN 64 + header(3)
#define MAX_PWR_TBL_COUNT		(28)	/// real : LEN 	28 *2 + header(3)





/*----------------------------------------------------------*/
typedef struct __attribute__((packed))
{
	INT8U  use;
	INT8U idx1;
	INT8U idx2;
}BAND_SEL;
/*----------------------------------------------------------*/
enum
{
	TX_0=0,
	TX_1,
	RX_0,
	RX_1,
	//--
	DLY_TXRX_MAX
};
/*----------------------------------------------------------*/
// WRCS Command with GUI
enum
{
	//------------------------------------------------
	// IRCS 1.0 Only
	//------------------------------------------------
	CMD_STATUS           = 0x00,
	CMD_CONTROL          = 0x01,
	CMD_ALARM_RPT        = 0x02, // alarm reason 및 발생시간.
	CMD_ALARM_MASK       = 0x03,
	// 0x04	 System Restart
	// 0x05  중계기 자동보고
	// 0x06  Wibro Modem 중계기 초기 보고
	// 0x07  reserved
	// 0x08  reserved
	//------------------------------------------------
	// IRCS 1.0 & IRCS 2.0
	//------------------------------------------------
	// 0x0B ~ 0x0E : 중계기 다운로드.
	// 0xE0 ~ 0xE3 : Module 다운로드.
	CMD_Get_INVENTORY    = 0x60,
	CMD_AUTO_INVENTORY   = 0x61,
	CMD_RVS_WAVE_MON     = 0x63, // DDR의 파형 조회 명령과 같음.	

	CMD_INVENTORY_SET    = 0xE6,
	CMD_PSU_RESET_REQ    = 0xEA,

	
	
	//------------------------------------------------
	// IRCS 2.0 Only
	//------------------------------------------------
	CMD2_SUPP_ALARM_RPT  = 0x46, // 0x07 @ iRO, Alarm Suppression Report/ACK
	CMD2_ALARM_LOC_RPT   = 0x45, // (0x16 @ iRO), Alarm Report/ACK (LOC. 정보 및 Inform. 추가)
	CMD2_STATISTIC_RPT   = 0x47, // (0x17 @ iRO), 'ST' 항목(통계), Statistic Report/ACK
	CMD2_ST2_MEA_RPT     = 0x44, // (0x18 @ iRO), 'Meas' 항목(Status2), Measurement Request/Response	
	CMD2_CFG_nC_REQ      = 0x49, // (0x19 @ iRO), CF/nC Request/Response	
	CMD2_CFG__C_SET_REQ  = 0x48, // (0x1A @ iRO), CF/C Request, Control/Response	
	CMD2_CFG__C_AUTO_RPT = 0x4A, // (0x1C @ iRO), CF/C Auto Report/ACK, (1일1회, Reset 시, GUI 제어 시).
	CMD2_CFG_nC_AUTO_RPT = 0x4B, // (0x1B @ iRO), F/nC Auto Report/ACK, (1일1회, Reset 시, GUI 제어 시).
	CMD2_PLD_SET_REQ     = 0x4C, // (0x1D @ iRO), PLD 설정

	CMD2_FTP_COMPLETE    = 0x51, // DOWNLOAD 완료(Device -> EMS), 2019.09.04	
	
	CMD_FW_UPDATE        = 0x5B, // (0x1E @ IRO), Repeater FW 적용.  
	CMD_WAVE_Spectrum	 = 0x5F, // 파형감시 분할. added ycso 191212

	CMD2_LocalTime_REQ   = 0x68, // Local Time 요청
	CMD2_LocalTime_RSP   = 0x69, // Local Time 요청 응답

	CMD2_SFP_Mod_Inv_St  = 0x6A, // SFP Module Inventory 상태 메시지(유선)
	CMD2_BAR_Code	     = 0x6B, // BarCode

	/*		
	2019.06.10
	------------------	
	0x6A	SFP Module Inventory 상태 조회	: PRU/AAU : Not Use.		
	*/

/////////////////////////////////////////////////////////////////////////////////////

	CMD_TABLE_SET_REQ        = 0x75, // Table Ctrl request
	CMD_TABLE_SET_RSP        = 0x75, // Table Ctrl response
	CMD_TABLE_STS_REQ        = 0x76, // Table Status request
	CMD_TABLE_STS_RSP        = 0x76, // Table Status response
	
/////////////////////////////////////////////////////////////////////////////////////

	CMD_TOPOLOGY         = 0x79, // Topology response

	// Compression download
	CMD2_COMPRESS_DN_NOTI_1 = 0xA0,
	CMD2_COMPRESS_DN_NOTI_2 = 0xA1,
	CMD2_COMPRESS_DN_DATA   = 0xA2,
	CMD2_COMPRESS_DN_END    = 0xA3,

	//-------------------------
	CMD_GUI_LOG_File_REQ      = 0xB0,
	CMD_RCU_LOG_File_RSP      = 0xB1,
	
	CMD_GUI_LOG_File_GET_REQ  = 0xB2,
	CMD_RCU_LOG_File_GET_RSP  = 0xB3,
	
	CMD_GUI_LOG_File_LIST_REQ = 0xB4,
	CMD_RCU_LOG_File_LIST_RSP = 0xB5,
	//-------------------------

	CMD2_GUI_ID_PW       = 0xEC, // GUI ID/Password 조회	
	CMD_REMS_HIDDEN      = 0xE9, //	Hidden Request/Response, PRU/AAU	
	CMD2_EMS_LOOPBACK    = 0xEF, // ETHERNET Loopback

	CMD_RO_ALIVE         = 0xF0,
	CMD_UP_DEV_BROAD     = 0xF1,
	CMD_RO_DLY_REQ       = 0xF2,
	CMD_CW_OFF_BROADCAST = 0xF3,
	CMD_GUI_LOG_REQ      = 0xF4, // 2019.09.04 : Log Request from Local GUI
	CMD_RO_WAVE_REQ      = 0xF5,	

	CMD_TOPOLOGY_BROAD   = 0xF9,


	CMD_SIM_TCP_Conn	 = 0xFB, // SIM
	CMD_SIM_Spectrum     = 0xFC, // SIM ------- Not used @ RCU, only used SIM-GUI ~ DTU via TCP communication.
	CMD_SIM_TestRslt     = 0xFD, // SIM Test Result
};

#define SINGLE_CTRL (1) // 0 아닌 값.
#define MAX_ATT_x2	(63) // 63 == 31.5dB

/*----------------------------------------------------------*/

enum
{
	BW_200 = 0,
	BW_180,
	BW_160,
	BW_140,
	
	BW_Max
};


enum
{
	CPRI_0 = 0, // 100M
	CPRI_1,     // A
	CPRI_2,     // 100M
	CPRI_3,     // A
	
	CPRI_PATH
};


enum
{
	Cpri0 = 0,	
	Cpri2,
	Cpri1,
	Cpri3,
	
	CPRI4
};

enum
{
	FA_0 = 0,
	FA_1,
	MAX_FA_PATH
};

enum
{
	MIMO_0 = 0,
	MIMO_1,
	MIMO_2,
	MIMO_3,
	MAX_MIMO_PATH
};

enum
{
	_100M = 0,
	Alpha,
	MAX_Optic
};

enum
{
	COMMON = 0,
	A_ONLY,
	Optic_2
};

enum
{
	_I_ = 0, 
	_Q_,
	_IQ_
};


enum
{
	DL_0 = 0,
	DL_1,
	DL_2,
	DL_3,
	MAX_DL
};
enum
{
	UL_0 = 0,
	UL_1,
	UL_2,
	UL_3,
	MAX_UL
};

enum
{
	PATH_0 = 0,
	PATH_1,
	PATH_2,
	PATH_3,
	
	MAX_PATH_4
};


enum
{
	STS = 0,
	CTL,
	FLG, // flag
	ST_CTL
};

enum
{
	DL = 0,
	UL,
	DL_UL
};






//-------------------------------------------------------------------------------
enum
{
	_DIG_DPD_ = 0,
	_DIG_BYPASS_,

	_DIG_BYPASS_MAX
};
//-------------------------------------------------------------------------------

enum 
{
	LO = 0,
	HI,
	LO_HI
};

enum 
{
	f_CLR = 0,
	f_SET
};



enum
{
	AMPU_4_1 = 0,
	AMPU_4_2,
	AMPU_4_3,
	AMPU_4_4,

	AMPU_4_MAX
};



enum
{
	_INIT_ = 0,
	_SWITCH_,
};





// AC_Fail_St 값.
enum
{
	_AC_NORMAL_ = 0,
	_AC_FAIL_,
	_AC_FAIL_30_SEC_,
	_AC_FAIL_30_SEC_OVER_,
	_AC_FAIL_5_MIN_OVER_,
	_AC_FAIL_MAX_
};

enum
{
	_AC_F_NONE_ = 0,
	_AC_F_ATT_REQ = 0x01, // ATT SET, bit0
	_AC_F_AMP_REQ = 0x02  // AMP OFF, bit1
};




//-------------------------------------------------------------------------------
/*
enum // iRO
{
	DLY_LTE_21 = 0,
	DLY_LTE_08,
	DLY_LTE_18, 	
	DLY_LTE_26, // 20M
	DLY_LTE_26_10M, // 10M
	
	DLY_SET_MAX 
};
*/
//-----------------------------------------------------------

//-----------------------------------------------------------

enum 
{
	_SS_ = 0,
	_MM_,
	_SSMM_
};

enum 
{
	_TX0_ = 0,
	_TX1_,
	_TX_MAX_
};

//-----------------------------------------------------------

#define iROO_MAX_RVS_ATT_x2 (30*2)


 //-------------------------------------------------------------------------------
#define PD_MAX_NUM (9) // PD PWR, 상한 9개.
//-------------------------------------------------------------------------------


enum
{
	_AMP_=0,
	_DIG_,
};

//-----------------------------------------------------

//-----------------------------------------------------
// OSM Remote SWitching
enum
{
	OSM_SW_AND_Restore = 0, // 절체(OSM SW On) 후 복구(OSM SW Off).
	OSM_SW,                 // 절체만(OSM SW On), 복구안함.
	OSM_Restore,            // 바로 복구(OSM SW Off함)만.

	OSM_Ctrl_MAX
};
//-----------------------------------------------------
// Digital Optic Switching
enum
{
	DIG_OPT_Restore = 0,
	DIG_OPT_SW,

	DIG_OPT_MAX
};
//-----------------------------------------------------
// Rvs-Spurious : payload onoff
// UDA 관련 
enum
{
	RS_PayloadOFF = 0,
	RS_No_Action,
	RS_Alm_Only,

	RS_MAX
};
//-----------------------------------------------------
// Auto_Reset
enum
{
	AR_Reset = 0,
	AR_No_Action,
	AR_Alm_Only,

	AR_MAX
};

/*
IRO
------------------
UDA_불요파
UDA_Link Fail / Suppress
UDA_PD  / Suppress
UDA_Digital Board  / Suppress


5G
------------------
UDA_불요파
      X / UDA_Link Suppress
UDA_PD  / Suppress
UDA_Digital Board  / Suppress

*/

// Auto_Reset : 0x2329, 4byte 위치.
enum
{
	UDA_Spurious = 0,
	UDA_Link, // uda link 동작 mode
	UDA_Clock, // uda dig Borad 동작 mode -----> clock
	UDA_PD_CP,  // uda PD 동작 mode  ----------> PD(CPRI)

	UDA_MAX
};
//-----------------------------------------------------
// Hidden OSM
enum
{
	OPT_SWBack = 0, // 미절체.
	OPT_SW,
	OPT_MAX
};
//-----------------------------------------------------

//-----------------------------------------------------
// 2015.10.06
enum
{
	DL_St_IDLE = 0,					// 0x00: Idle (전원 Reset등으로 초기화)
	DL_St_Complete,					// D/L 완료(Reset 대기)
	DL_St_Now,      				// D/L 중
	DL_St_Fail,     				// D/L 실패 (Timeout)
	DL_St_Rst_Wait, 				// reset 대기, reset time count down...
	DL_St_Compression_Now,			// 압축 해제 중.
	DL_St_Compression_Check,		// 압축 유효성 검사 중.
	DL_St_Inside_DL_Now,			// 압축 해제 후 내부 DL 중.

	DL_St_MAX
};
//---------------------------
enum
{
	DL_ST = 0,
	DL_CNT,  // percent or time
};
//---------------------------
enum
{
	DN_RCU = 0,
	DN_PSU,
	DN_DTU,	
	DN_FPGA,
	
	DN_MAX
};
//---------------------------
enum // DIG Intergration version
{
	I_VER_DTU = 0,
	I_VER_FPGA,

	I_VER_MAX
};
//---------------------------

/*----------------------------------------------------------*/
enum // EBER
{
	EBER_NO_USE = 0,	
	EBER_10_9, // 10^(-9), 5
	EBER_10_8, // 10^(-8), 50.
	EBER_10_7, // 10^(-7), 500
	EBER_10_6, // 10^(-6), 5000
	EBER_10_5, // 10^(-5), 50000.
	EBER_MAX,  
};
/*----------------------------------------------------------*/
typedef struct __attribute__((packed))
{	
	OS_TIMER timer_ms;
	INT32U max_time_ms; // 누적 요청 시간	
	INT8U cnt_en;
} BIP_ERR;

//-----------------------------------------------------
#define SUPPRESS_MAX (3) // 연속 TBD 회 이면, suppression 상태 됨.
#define Is_Visible_Char(x)  ( ((x) >= 0x20)&&((x) <= 0x7E) )

//-----------------------------------------------------
#define CW_SELF_CHK_TIME 6
#define CW_SELF_SIZE 3 // (8이 맞을 듯)


#define FTP_DN_ST_SIZE (20)


//-----------------------------------------------------------------------------
// PSU
typedef struct __attribute__((packed))
{
	//------------------------------------------
	// 5G PRU : PSU AID
	//------------------------------------------
	INT16S Temp_x10;            // 0xB600, Temp (PSU)
	INT8U  PSU_I_Ver[3];        // 0xB601
	INT16U AC_In_Volt_x10;      // 0xB603, AC In Volage
	INT16U DC_Out_Volt_x10[3];  // 0xB604, 0xB606,  DC output Volage, (전송순서: 50.2v, 5.8v, 24v)	
	INT16U Load_Current_x10[3]; // 0xB605, load current, (전송순서: 50.2v, 5.8v, 24v)
	INT16U Batt_Volt_x10;       // 0xB607, Battery Volage
	
	INT16U Fan_Speed;     // 0xB612/0xB616,
	INT8U  PAU_Switch_St; // 0xB630,
		/*  5G PRU
			- Bit 7: PAU #1 SW Status
			- Bit 6: PAU #2 SW Status
			- Bit 5: PAU #3 SW Status
			- Bit 4: PAU #4 SW Status
		*/
	
	INT8U  PSU_Ext_FAN_Sum;// 0xB631
	INT8U  PSU_St_AC_DC;   // 0xB632
	INT8U  PSU_St_Temp_DC; // 0xB633
	INT8U  PSU_St_Etc;     // 0xB634
	INT8U  DL_St[2];       // 0xB63A,
	
	//------------------------------------------
	// etc
	//------------------------------------------
	INT8U Hi_Temp;     
	INT8U Hi_Temp_SD;     // high temp shutdown
	INT8U Ripple_Alm_5v6; 
	INT8U AC_Alm_OV;	
	INT8U Batt_Charge_OnOff; 
	
	INT8U Battery_Low;  // 장애 항목
	INT8U Batt_Abnormal;// RCU Algorithm needed....
	
	
	INT8U desire_Fan_Speed;
	INT8U Cur_Green_Mode;

	INT8U Ext_FAN_Alm[3]; // [FAN #1/#2/#3] / 0x00(normal), 0x01(abnormal), Ext FAN
	INT8U PSU_FAN_St;     // Case FAN, 0x00 : 정상, 0x01 : 이상 
	INT8U IRO_remote_type[2];	// 2  iRO Remote Type 정보
	INT8U PRU_remote_type[2];	// 2  PRU Remote Type 정보
	INT8U Compatible;	  // compatibility, 2020.02.14, (0x5A is compatible, 0xFF not compatible)
	//------------------------------------------
	INT8U  AC_Alm_Recti;       //  AC Alarm (Rectifier)
	INT8U  DC_OV_Recti[3];	   // [Main/Sub], DC OV (Rectifier), (전송순서: Main/Sub5.8/ 24v)
	INT8U  DC_UV_Recti[3];	   // [Main/Sub], DC UV (Rectifier), (전송순서: Main/Sub5.8/ 24v)
	INT8U  DC_OC_Recti[3];	   // [Main/Sub], DC OC (Rectifier), (전송순서: Main/Sub5.8/ 24v)

	// 50.2v : Main, 5.8v  : Sub

	INT32U Watt_Tot_Accu;  // Wattage (Total Accumlate)
	INT16U Watt_real_time; // Wattage(real time)
	INT16U Watt_hour_aver; // Wattage(1 hour average)
	INT16U Watt_day_aver;  // Wattage(1 day average)
	INT16U Watt_week_aver; // Wattage(1 week average)
	INT16U Watt_month[12]; // Wattage(1 month), 12 months
	
} PSU_ST;

//-----------------------------------------------------------------------------
// IRO Digital
#define SFP_5G_NUM (4) // optic, ext0 optic, ext1 optic, ext2 optic
//typedef struct __attribute__((packed))
//{
//	INT16S Fwd_Pwr_DPD_x10[AMPU_4_MAX];
//	INT16S FB_Pwr_DPD_x10[AMPU_4_MAX];
//} DIG_ST; // delete 2019.05.08

//-----------------------------------------------------------------------------
// IRO Downlaod
typedef struct __attribute__((packed))
{
	INT8U RCU_Progress; // 0xAE04
	INT8U DTU_Progress[2]; // 0xAE0B
	INT8U RCU_Dn_St; // 0xAE08
	/*
		0x00 : Normal Operation
		0x01 : State1. 
		0x02 : State2. 
		0x03 : State3.
	*/
	
	INT8U DTU_Dn_St; // 0xAE0A
	/*
		0x00 : Normal Operation
		0x01 : FTP 다운로드 (CPU)
		0x02 : FTP 다운로드 (FPGA)
		0x03:  FTP 다운로드 후 RESET 대기
		0x04 : FTP 다운로드 실패
	*/
	
	INT8U Dn_Ctrl_DTU; // 0xAE09, Download State Control (DTU)
	/*
		0x00 : Normal Operation
		0x01 : FTP 다운로드 시작
		0x02 : FTP 다운로드 후 RESET 대기
		0x03 : DTU RESET
		0x04 : FTP 다운로드 중지
		0x05 : 다운로드 State 초기화
	*/
	
} DOWN_ST;


typedef struct  __attribute__((packed))
{
	INT8U info_AE05[4]; // download inform.
	/*
	1st Byte : Download Target
	  0x00 : Normal Operation
	  0x01 : All Node
	  0x02 : Branch
	  0x03 : ID
	  
	2nd ~ 4rd Byte : 최상위 Node가 이미지를 전송하는 target Node의 Sub ID
	*/
	INT8U st_AE06; // upDev 최상위 download state (0 : normal, 1~ : state1 ~)
	INT8U br_st_AE07[9]; // branch별 download state
	INT8U reserve[2];
}COMPRESS_DN_UP_DEV;

/*----------------------------------------------------------*/
typedef struct __attribute__((packed))
{
	INT8U   Type;			// 종류
	INT8U	Rep_Type[2];	// 타입(AID 문서의 중계기 타입)
	INT8U	supply;			// 공급사(AID 문서)
	INT8U	maker;			// 제조사(AID 문서)
	INT8U	Ver;
	INT8U	sn[4];			// 시리얼번호
	INT8U	date[3];		// 제조일 : 년월일 각각 1바이트
	INT8U	ERP_code[15];	// ERP Code
	INT8S	Maker_Mode[4];	// 제조사 모드
} INVENTORY_STR;
/*----------------------------------------------------------*/
typedef struct __attribute__((packed))
{
	INT8U   Type;			// 종류
	INT8U	Rep_Type[2];	// 타입(AID 문서의 중계기 타입)
	INT8U	supply;			// 공급사(AID 문서)
	INT8U	maker;			// 제조사(AID 문서)
	INT8U	Ver;
	INT8U	sn[4];			// 시리얼번호
	INT8U	date[3];		// 제조일 : 년월일 각각 1바이트
	INT8U	ERP_code[15];	// ERP Code
	INT8S	Maker_Mode[4];	// 제조사 모드
	//-------------------------
	INT8U	Ver2[3];
} INVENTORY_2_STR; // AMP inventory status, version2 Added.

enum
{
	LTE26_B_20_10 = 0,
	LTE26_B_20,
	LTE26_B_10,
};


enum
{
	BIP_0 = 0,
	BIP_Ex_0,
	BIP_Ex_1, // MIMO
	BIP_Ex_2, // MIMO	
	BIP_NUM
};


// ftp compress-download
// Ro_kind
enum
{
	Br_lo_Ro = 0,
	Br_Hi_Ro,
	Sever_Ro,
};


enum
{
	__Mount = 0,
	UnMount,
};

//----------------------------------------------------
typedef struct __Alarm_UL
{	
	struct{
		//-----------------------------------------
		union{
			INT8U Data;
			struct{
				INT8U Shutdown		:1; // 0, AID 0x5115
				INT8U r___1			:1; // 1
				INT8U r___2			:1; // 2
				INT8U r___3			:1; // 3
				//------------------------------
				INT8U Disabled		:1; // 4, AID 0x5117
				INT8U r___5			:1; // 0
				INT8U r___6			:1; // 0
				INT8U r___7			:1; // 0
			}__attribute__((packed));
		}PAU;
		//-----------------------------------------
		union{
			INT8U Data;
			struct{		
				INT8U r___0			:1; // 0
				INT8U r___1			:1; // 1				
				INT8U r___2			:1; // 2
				INT8U r___3			:1; // 3
				//------------------------------
				INT8U r___4			:1; // 4
				INT8U Over_Input 	:1; // 5, AID 0x5210
				INT8U r___6			:1; // 6
				INT8U r___7			:1; // 7
			}__attribute__((packed));
		}UL;
	}Path[MAX_PATH_4];// UL 0 ~ 4
}ALM_gREMS; // gREMS and L/S
//----------------------------------------------------
typedef struct __AlarmStatsu
{
	//-----------------------------------------
	union{
		INT8U Data;
		struct{
			INT8U r___0 				:1; // 0
			INT8U GUI					:1; // 1
			INT8U DC					:1; // 2
			INT8U AC					:1; // 3, (PRU Only)
			//------------------------------
			INT8U FAN2_PSU				:1; // 4, Internal FAN (PRU Only)
			INT8U FAN1_Case				:1; // 5, External FAN
			INT8U Temp_Up				:1; // 6 
			INT8U Door					:1; // 7, (PRU Only)
		}__attribute__((packed));
	}A_0;
	//-----------------------------------------
	union{
		INT8U Data;
		struct{
			INT8U r___0 			:1; // 0
			INT8U r___1 			:1; // 1
			INT8U r___2 			:1; // 2
			INT8U r___3 			:1; // 3
			//------------------------------
			INT8U r___4				:1; // 4 // 2019.04.02.Delete
			INT8U r___5				:1; // 5 // 2019.04.02.Delete
			INT8U Batt_St			:1; // 6, (PRU Only)
			INT8U Batt_Low			:1; // 7, (PRU Only)
		}__attribute__((packed));
	}A_1;	
	//-----------------------------------------
	union{
		INT8U Data;
		struct{
			INT8U LOS_SFP_1_a 		:1; // 0 // 2019.05.17. LOS(SFP1)(+α) (OPTIC_TRANSCEIVER_RX_LOS)
			INT8U LOS_SFP_1__ 		:1; // 1 // 2019.05.17. LOS(SFP1)      (OPTIC_TRANSCEIVER_RX_LOS)
			INT8U LOS_SFP_0_a 		:1; // 2 // 2019.05.17. LOS(SFP0)(+α) (OPTIC_TRANSCEIVER_RX_LOS)
			INT8U LOS_SFP_0__ 		:1; // 3 // 2019.05.17. LOS(SFP0)      (OPTIC_TRANSCEIVER_RX_LOS)
			//------------------------------
			INT8U CPRI_1_a			:1; // 4, AID 0x5301
			INT8U CPRI_1__			:1; // 5, AID 0x5301
			INT8U CPRI_0_a			:1; // 6, AID 0x5301
			INT8U CPRI_0__			:1; // 7, AID 0x5301
		}__attribute__((packed));
	}A_2;
	//-----------------------------------------
	union{
		INT8U Data;
		struct{
			INT8U PD_Pwr_U_0_a 		:1; // 0 // 2020.07.01. PD Power 상한(SFP0)(+α)
			INT8U PD_Pwr_U_0__ 		:1; // 1 // 2020.07.01. PD Power 상한(SFP0) 
			INT8U SFP_Ins_0_a 		:1; // 2 // 2019.04.02. SFP 탈실장(SFP0)(+α), 0x5315
			INT8U SFP_Ins_0__		:1; // 3 // 2019.04.02. SFP 탈실장(SFP0)     , 0x5315
			//------------------------------
			INT8U LD_SFP_1_a		:1; // 4 // 2019.05.17. LD(SFP1)(+α) (OPTIC_TRANSCEIVER_TX_FAULT), AID 0x5304
			INT8U LD_SFP_1__		:1; // 5 // 2019.05.17. LD(SFP1)      (OPTIC_TRANSCEIVER_TX_FAULT), AID 0x5304
			INT8U LD_SFP_0_a		:1; // 6 // 2019.05.17. LD(SFP0)(+α) (OPTIC_TRANSCEIVER_TX_FAULT), AID 0x5304
			INT8U LD_SFP_0__		:1; // 7 // 2019.05.17. LD(SFP0)      (OPTIC_TRANSCEIVER_TX_FAULT), AID 0x5304 
		}__attribute__((packed));
	}A_3;
	//-----------------------------------------
	union{
		INT8U Data;
		struct{
			INT8U PD_Pwr_L_1_a 		:1; // 0 // 2020.07.01. PD Power 하한(SFP1)(+α) 
			INT8U PD_Pwr_L_1__ 		:1; // 1 // 2020.07.01. PD Power 하한(SFP1) 
			INT8U PD_Pwr_L_0_a 		:1; // 2 // 2020.07.01. PD Power 하한(SFP0)(+α) 
			INT8U PD_Pwr_L_0__ 		:1; // 3 // 2020.07.01. PD Power 하한(SFP0) 
			//------------------------------
			INT8U PD_Pwr_U_1_a 		:1; // 4 // 2020.07.01. PD Power 상한(SFP1)(+α)
			INT8U PD_Pwr_U_1__ 		:1; // 5 // 2020.07.01. PD Power 상한(SFP1) 
			INT8U r___6				:1; // 6
			INT8U Clock				:1; // 7
		}__attribute__((packed));
	}A_4;	

	//===================================================
	// DL/UL 0 ~ 4
	//===================================================	
	struct{
		union{
			INT8U Data;
			struct{
				INT8U r___0			:1; // 0
				INT8U r___1			:1; // 1
				INT8U TSSI_Lower_a	:1; // 2, AID 0x5102
				INT8U TSSI_Lower__	:1; // 3, AID 0x5102
				//------------------------------
				INT8U Out_Lower		:1; // 4
				INT8U Out_Upper 	:1; // 5
				INT8U TSSI_Upper_a	:1; // 6, AID 0x5101
				INT8U TSSI_Upper__	:1; // 7, AID 0x5101
			}__attribute__((packed));
		}DL;
		//-----------------------------------------	
		union{
			INT8U Data;
			struct{
				INT8U Shutdown		:1; // 0, AID 0x5115
				INT8U Dev_Fail		:1; // 1
				INT8U DC_Fail		:1; // 2
				INT8U Link_Fail		:1; // 3, (PRU Only)
				//------------------------------
				INT8U Disabled		:1; // 4, AID 0x5117
				INT8U VSWR 			:1; // 5, AID 0x5111, PAU Algorithm 중(amp on/off 중) VSWR alarm.
				INT8U High_Temp		:1; // 6
				INT8U Over_Power	:1; // 7, AID 0x5110
			}__attribute__((packed));
		}PAU;
		//-----------------------------------------
		union{
			INT8U Data;
			struct{
				INT8U r___0				:1; // 0
				INT8U r___1				:1; // 1
				INT8U r___2				:1; // 2
				INT8U r___3				:1; // 3
				//------------------------------
				INT8U r___4				:1; // 4
				INT8U r___5 			:1; // 5
				INT8U r___6				:1; // 6
				INT8U DPD_Alm			:1; // 7
			}__attribute__((packed));
		}DPD;	
		//-----------------------------------------
		union{
			INT8U Data;
			struct{
				INT8U LNA_Fail_a	:1; // 0, for TEST Alarm
				INT8U Over_Input_a	:1; // 1, for TEST Alarm
				
				INT8U r___2			:1; // 2
				INT8U Gain_Low		:1; // 3, (AAU Only), AID 0x5213
				//------------------------------
				INT8U LNA_Fail		:1; // 4, (PRU Only), AID 0x5212
				INT8U Over_Input 	:1; // 5, AID 0x5210, PRU(PAU)/AAU Algorithm 중(amp on/off 중) Over.In alarm.
				INT8U RSSI_Lower_a	:1; // 6 // 2019.05.14 : UL RSSI 하한 삭제 
				INT8U RSSI_Lower__	:1; // 7 // 2019.05.14 : UL RSSI 하한 삭제 
			}__attribute__((packed));
		}UL;
		//-----------------------------------------
		union{
			INT8U Data;
			struct{
				INT8U r___0				:1; // 0
				INT8U r___1				:1; // 1
				INT8U r___2				:1; // 2
				INT8U r___3				:1; // 3
				//------------------------------
				INT8U r___4				:1; // 4
				INT8U r___5 			:1; // 5
				INT8U r___6				:1; // 6
				INT8U r___7				:1; // 7
			}__attribute__((packed));
		}DlUl;
	}Path[MAX_PATH_4];	
	//===================================================
	// UDA
	//===================================================
	union{
		INT8U Data;
		struct{
			INT8U r___0				:1; // 0
			INT8U CPRI_RST_Suppr	:1; // 1, UDA CPRI  Reset Suppression
			INT8U Clk__RST_Suppr	:1; // 2, UDA Clock Reset Suppression
			INT8U r___3 			:1; // 3, UDA Link  Reset Suppression, Link_RST_Suppr, deleted..
			//------------------------------
			INT8U r___4				:1; // 4
			INT8U Clock 			:1; // 5, UDA Clock
			INT8U PD_CPRI			:1; // 6, UDA_PD(CPRI)
			INT8U Spurious			:1; // 7, UDA_불요파
		}__attribute__((packed));
	}UDA;		
	//-----------------------------------------
	union{
		INT8U Data;
		struct{
			INT8U SuperCap_Exch		:1; // 0, Super Cap. 교체, (PRU Only)
			INT8U Mismatch_ID		:1; // 1, Mismatch(ID), AID 0x5611
			INT8U r___2				:1; // 2
			INT8U r___3				:1; // 3
			//------------------------------
			INT8U r___4				:1; // 4
			INT8U r___5 			:1; // 5
			INT8U r___6				:1; // 6
			INT8U r___7				:1; // 7
		}__attribute__((packed));
	}ETC;
//------------------------------------------------------------
// End of 5G Alarm  
//------------------------------------------------------------	
}ALM_ST;


//------------------------------------------------------------	
typedef struct
{	
	OS_TIMER alm_ms;
	OS_TIMER clr_ms;	
}ALARM_TIMER;

typedef struct __AlarmTimer
{
	struct{			
		ALARM_TIMER test;
	}TEST;
	// unlock 2s 유지시 알람.
	// lock 10s 유지시 정상으로 되어 있음.
	/*
	struct{
		ALARM_TIMER CPRI_1_a;
		ALARM_TIMER CPRI_1__;
		ALARM_TIMER CPRI_0_a;
		ALARM_TIMER CPRI_0__;
	}A_2;
	*/
	//-----------------------------------------	
	struct // 2020.01.15 added
	{		
		ALARM_TIMER FAN1_Case; // for AAU
	}A_0;
	//-----------------------------------------
	struct{			
		ALARM_TIMER Clock; // normal 10s 유지시 normal, alarm은 즉시. alarm 3s로 변경되어야 
	}A_4;
	//===================================================	
	struct{	
		struct{
			ALARM_TIMER TSSI_Lower_a; // DL.TSSI_Lower_a, alarm 5s
			ALARM_TIMER TSSI_Lower__; // DL.TSSI_Lower_a, alarm 5s

			ALARM_TIMER TSSI_Upper_a; // alarm 3s
			ALARM_TIMER TSSI_Upper__; // alarm 3s
		}DL;
		//-----------------------------------------	
		/*
		//struct{
		//	ALARM_TIMER Shutdown;
		//	ALARM_TIMER Dev_Fail;
		//	ALARM_TIMER DC_Fail;			
		//	ALARM_TIMER Disabled;
		//	ALARM_TIMER VSWR;
		//	ALARM_TIMER High_Temp;
		//	ALARM_TIMER Over_Power;
		//}PAU;
		//-----------------------------------------
		//struct{				
		//	ALARM_TIMER DPD_Alm;
		//}DPD;	
		//-----------------------------------------
		//struct{				
		//	ALARM_TIMER Gain_Low;
		//	ALARM_TIMER LNA_Fail;
		//	ALARM_TIMER Over_Input; : 알고리즘 동작 중.
		//}UL;		
		*/
	}Path[MAX_PATH_4];
}ALM_TIMER;
//------------------------------------------------------------	


enum
{
	ANTS_LED_R___ON = 0, // RED ON
	ANTS_LED_R_Blnk,     // RED Blink
	ANTS_LED_B___ON,     // GREEN(Blue) ON
	ANTS_LED_B_Blnk,     // GREEN(Blue) Blink
	ANTS_LED_Toggle,     // 교차 점멸
	ANTS_LED_R_B_ON,     // RED ON && Blue ON 
	
	ANTS_LED_Max
};


enum
{
	LSH_SS = 0,	
	LSH_NK,
	LSH_ER,
	
	LSH_Max
};


//--------------------------------------------------------
typedef struct __attribute__ ((packed))
{
	INT8U	init_MAC[6];
	INT8U	def__MAC[6];
	INT8U	init__IP[4];
// ---------------------------------------------------------
// RCU : default
// ---------------------------------------------------------
	////union 
	////{
	////	INT8U	SUB_ID[4];
	////	INT32U	SUB_ID_idx;
	////}; // 2019.05.01 : use only 'SUB_ID[4]' below
	INT8U	SUB_ID[4];
	//---------------------------------
	ALM_ST AlmSt;    // current immediate alarm
	ALM_ST AlmSt_prev; // previous immediate alarm, alarm 변경시, string update를 위해.
	
	ALM_TIMER AlmTime;	

	// AAU Over Input Alarm Report when Total-Shutdown
	ALM_gREMS gREMS; // gREMS, LS only
	//---------------------------------
	/*
	------------------------------------------
	AID 0xF2F2	: GUI Test Alarm Flag, if 1, Test alarm is Used.
	------------------------------------------
	*/
	INT8U  AlmTest;  // AID 0xF2F2
	ALM_ST AlmFlag;  // AID 0xF2F2, GUI TEST Alarm Flag  (1:use, 0:not-use)
	ALM_ST AlmCtrl;  // AID 0xF2F2, GUI TEST Alarm Value (1:alarm, 0:normal)
	//---------------------------------
	ALM_ST Last_Alm; // Last updated Alarm, used Status Response of GUI/REMS
	ALM_ST Prev_Alm; // previous Last-alarm
	//---------------------------------
	ALM_ST NMS_Alm;  // alarm for REMS
	ALM_ST Old_Alm;  // previous REMS Alarm
	//---------------------------------
	ALM_ST Stdby_Bk_Alm; // backup alarm when AMP standby	
	//----------------------	
	/////INT8U  Arm[RU_ALARM_CNT];	// 2019.05.01 : deleted..  // 일시적인 알람. 포트에서 일시적으로 읽은 알람.
	//----------------------	
	INT8U  dpd_t_alm[AMPU_4_MAX];
	INT8U  Sum_Alarm;	// Sum Alarm : if alarm 1, else 0.
	//----------------------
	INT8U  ResetReason;
	INT8U  ResetReason_cpy;
	INT8U  ResetReason_log;	
	
	INT8U  Rep_Type;
	INT8U  AMP_HW_Insert[AMPU_4_MAX]; // 0(insert), 1(not insert, When RCU reset, init value)

	INT8U  Ru_Sys_Date[6]; // UTC, System Date : year,month,day,hour,min,sec, 2byte is reserved	
	INT8U  Dig_Ethernet_Ready;
	
	//////////////////////DIG_ST Dig_St; .. deleted 2019.05.08
	
	INT8U  Is_CPRI_Lock; // 0 is unlock, 1 is lock. Summation of CPRI0, CPRI1 only
	INT8U  Ref_CPRI_No;  // 'CPRI0' or 'CPRI1'
// -------------------------------------------------
// AID : 0x01__
// -------------------------------------------------
	INT8U  Maker[2];   // AID 0x0100/0x0101, maker/supplyer, 
	INT8U  Type[2];    // AID 0x0102
	INT8U  Pkg_Ver[3]; // Package version
	INT8U  RCU_Ver[3]; // AID 0xB015
	INT8U  RCU_Ver_Main[3]; // AID 0x5503
	
	INT8U  Int_Ver[2]; // Internal Version

// -------------------------------------------------
// AID : 0x02__
// -------------------------------------------------		
	INT8S  Temp;     // 0x0201, Temp
	INT16S Temp_x10; // 0x0201, 0x5012,Temp, Temp sensor
	INT8S  TempLmt;  // 0x0202, Temp Upper limit
	
			
	////INT8U  Batt_Use; // (0x00: 미설치, 0x07: 외장(50v)), deleted. 2019.04.25
	INT8U  Batt_Insert; // AID 0x0210, 0x06: 내장(50V), 2019.04.16. AID Changed 
	INT8U  OSM_Use;  // 0x501C, <0: 미사용, 1: 사용>
	INT8U  Batt_Sts; // AID 0x027D	


	/*
	Batt_Insert; // AID 0x0210
	--------------------------
	0x00: 미설치
	0x07: 외장(50v)
	*/
	
	/*
	Batt_Sts; // AID 0x027D	
	--------------------------
	0x00: 정상
	0x01: Battery Fault(충전차단)
	0x02: Battery Fault(방전차단)
	0x03: Battery Fault(Battery 전압 이상)
	0x04: Battery Fault(HW)
	*/
	
// -------------------------------------------------
// AID : 0x50__
// -------------------------------------------------
	INT16U d_RU_ID;           // RU ID,	0x5000
	INT16U d_Cascade_ID[CPRI_PATH];      // Cascade ID, 0x5001
	INT8U  LayerUseInfo[AMPU_4_MAX]; // 0x5002, AMP-Use-Info, 1(use), 0/2/3(not-use)
	INT16U d_PCI[CPRI_PATH];     // 0x5003
	INT8U  d_Cell_ID[CPRI_PATH]; // 0x5004
	INT8U  d_gNB_ID[CPRI_PATH][4];  // 0x5005
	INT8U  g_LS_Port_No[CPRI_PATH]; // 0x5006
	INT32U g_NG_ARFCN[MAX_Optic];   // 0x5007
	INT8U  d_Channel_BW[MAX_FA_PATH]; // 0x5008	
	INT8U  LS_Maker; // 0x500A	(0xEA: SS, 0xEC: ELG, 0xEE: NKA)
	INT8U  LS_ID[MAX_Optic]; // 0x500C, 2019.05.17
	INT8U  RU_Init_Status; // 0x500D, 2019.08.21
	INT8U  RU_InitSt_Prev;
	INT8U  d_DUH_ID[CPRI_PATH]; // 0x5009, 2020.01.28 
// -------------------------------------------------
// AID : 0x51__
// -------------------------------------------------	
	INT8U  d_TSSI_SD_St[MAX_Optic][MAX_DL]; // ANTS-PRU Only, AID not defined. 2019.04.26.
	
	INT16S d_DL_TSSI_dBm_x10[MAX_Optic][MAX_DL]; // RMS, DL TSSI, 0x5100
	INT16S d_UL_RSSI_dBm_x10[MAX_Optic][MAX_UL]; // RMS, UL RSSI, 0x5200
	INT16S d_UL_RSSI_dBm_Alc_x10[MAX_Optic][MAX_UL]; // AAU Only, for ALC/Shutdown
	INT16S d_UL_IN_RSSI_dBm_x10[MAX_UL]; // RMS, UL IN RSSI, PRU, 2019.05.03

	INT16S d_DL_FB_RSSI_x10[MAX_Optic][MAX_UL]; // RMS, DL Feedback, ANTS-AAU Only
	INT16S d_DL_FB_RSSI_Tot_x10[MAX_UL]; // RMS, DL Feedback Total Power, ANTS-AAU Only	
	INT16S d_R_GC_RSSI_RMS_x10[MAX_Optic][MAX_UL]; // ANTS-AAU Only, 2019.05.11, 0xC053
	INT8U  UL_Gain_Chk_Mode; // ANTS-AAU Only, 2019.05.16, 0xC054, 1(on), 0(0ff)
	INT8U  DL_Mode_En;       // ANTS-AAU Only, 2019.05.31, 0xC059, 0(on), 1(0ff)	
	
	INT16S d_DL_TSSI_dBm_peak_x10[MAX_Optic][MAX_DL]; // Peak, DL TSSI, AID Not Allocated
	INT16S d_UL_RSSI_dBm_peak_x10[MAX_Optic][MAX_UL]; // Peak, UL RSSI, AID Not Allocated
	
	INT32U d_DL_TSSI_dec[MAX_Optic][MAX_DL]; //	RMS, DL TSSI_Decimal, PRU(0xB100), AAU(0xC073)	
	INT32S d_DL_TSSI_dec_Pre_EQ[MAX_Optic][MAX_DL]; // Only AAU(0xB100) = 'DTU_Reg(DL_RSSI_PEAK)' + 'Offset(AID 0xC06F)'
	INT32U d_UL_RSSI_dec[MAX_Optic][MAX_DL]; //	RMS, UL RSSI_Decimal, AID Not Allocated

	INT16S DL_OutPwr_Cal_x10[MAX_Optic][AMPU_4_MAX]; //	DL Output Calculation, 0x5108
	
	INT32U d_DL_TSSI_dec_peak_x10[MAX_Optic][MAX_DL]; // Peak, DL TSSI_Decimal, AID Not Allocated
	INT32U d_UL_RSSI_dec_peak_x10[MAX_Optic][MAX_DL]; // Peak, UL RSSI_Decimal, AID Not Allocated
	
	INT16S Fwd_Out_Pwr_x10[AMPU_4_MAX];        // AID 0x5103	
	INT16S Fwd_Out_Lmt_x10[AMPU_4_MAX][LO_HI]; // AID 0x5104/0x5105

	INT8U  Fwd_Out_Att_x2[AMPU_4_MAX]; // AID 0x5106 : PRU
	INT8U  DL_Att[4];				   // AID 0x5106 : AAU. 0.25dB	(ALC + User_ATT)
	INT8U  Pwr_Amp_En[AMPU_4_MAX];     // AID 0x5116 : AID is Control Pnly, Status is used in the RCU-F/W.
// -------------------------------------------------
// AID : 0x52__
// -------------------------------------------------
	INT8U  UL_Att[4];		             // AID 0x5206, 0xB224 : AAU(DTU-RF), ANTS-AAU-RF-UL-ATTEN, 0.25 Step, temp-att is excluded.
	INT16U d_UL_Att_x10[MAX_FA_PATH][MAX_MIMO_PATH]; // AID 0x5206 : PRU(DTU), 0.5dB Step
	INT16U UL_Gain_x10[AMPU_4_MAX];     // AID 0x5207	: UL Gain
	INT16U d_UL_GainBal_x10; // AID 0x5209 : Gain Balance
	INT8U  UL_LNA_ON_OFF[AMPU_4_MAX];	// AID 0x5216	: UL LNA ON OFF


// -------------------------------------------------
// AID : 0x53__
// -------------------------------------------------

	INT8U  d_CPRI_Alarm[CPRI_PATH];      // 0x5301
	/*
	  Bit 4: CPRI Core Unlock	 
	  Bit 1: LOF (CPRI)
	  Bit 0: LOS (CPPRI)
	  
	  0: Normal, 1: Alarm, 
	  bit2,3 not used
	*/
	INT8U  LOS_SFP_St[CPRI_PATH];      // 0x5302, Alarm Status
	INT8U  LD_SFP_St[CPRI_PATH];      // 0x5304
	
	
	INT16S d_SFP_LD_Pwr_x10[CPRI_PATH];  // 0x5305
	INT16S d_SFP_PD_Pwr_x10[CPRI_PATH];  // 0x5308
	
	INT32U d_BIP_Count[DL_UL][CPRI_PATH];     // AID 0x530A, 0xC046, Error Cnt Instant, 2019.04.23	
	INT32U d_BIP_Cnt128[DL_UL][CPRI_PATH][4]; // 128bit count for ANTS Only
	INT32U d_BIP_ErrCnt[DL_UL][CPRI_PATH];    // AID 0xB427, Accumulated Value, 2019.05.09
	
	INT16S d_SFP_Vcc_x10[CPRI_PATH];     // 0x5310
	INT16S d_SFP_LD_Bias_x10[CPRI_PATH]; // 0x5311	

	////INT16S d_SFP_WaveLen_x10[CPRI_PATH]; // 0x5312, 2019.04.25. deleted
	INT32U d_SFP_WaveLen_nm[CPRI_PATH];  // 0x5312, 2019.04.25. added.
	
	INT16U d_SFP_BitRate[CPRI_PATH];     // 0x5313, BitRate(CPRI), 
	INT16S d_SFP_Temp_x10[CPRI_PATH];    // 0x5314

	INT32U d_Hard_Clip_Cnt_Accum[MAX_DL]; // 0xB40E, 2019.05.09, 제어시 Conut 초기화.
	////INT16U d_Hard_Clip_thhold[MAX_DL]; // AID Not Allocated, 2019.04.22 :--deleted
	////INT8U  d_Hard_Clip_Clear[MAX_DL];  // AID Not Allocated, 2019.04.22 :--deleted
	
	INT32U d_CFR_Out_Gain[MAX_DL];     // AID Not Allocated, 2019.04.02 : register 1x4
	
	INT8U  DPD_Alarm_St[MAX_DL];     // AID 0x5321
	/*
		AID 0x5321
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

	INT8U  d_RW_SFP_TX_DISABLE[CPRI_PATH];	// AID not allocated..	
	INT8U  d_RW_SFP_Reset[CPRI_PATH];       // AID 0xB432
	
// -------------------------------------------------
// AID : 0x54__
// -------------------------------------------------
	INT32U  d_Toffset[MAX_Optic];      	// 0xB42B, Status Only
	INT32U  d_TBdelay_DL[MAX_Optic];    // 0xB42C, Status Only, TBdelayDL(DL Hopping Delay)
	INT32U  d_TBdelay_UL[MAX_Optic];    // 0xB42D, Status Only, TBdelayUL(UL Hopping Delay)
	INT32U  d_RoundTrip_Dly[MAX_Optic]; // 0x5405, Status Only, optic Delay (Round Trip Delay)

	INT32S  T2A_Dly__ns[MAX_Optic]; // 2019.05.23
	INT32S  TA3_Dly__ns[MAX_Optic]; // 2019.05.23	
		
	INT8U  Scale_Factor_UL; // 0x540B 
	
	INT8S  d_Gain_DL_reserved[MAX_FA_PATH][MAX_MIMO_PATH]; 		// 	0x540C -> deleted, 2019.04.09
	INT8S  d_Gain_UL_reserved[MAX_FA_PATH][MAX_MIMO_PATH]; 		// 	0x540D -> deleted, 2019.04.09
	
	INT8U  FAN_Mode_Onoff;    // 0x5410, FAN Mode
	INT8U  FAN_Mode_St;       // 0x5411, FAN Mode 동작 상태	
	INT8U  FAN_Mode_Start_Time[2]; // 0x5412, [hour/mimute], // FAN Mode 동작 시간	
	INT8U  FAN_Mode_Stop__Time[2]; // 0x5412, [hour/mimute], // FAN Mode 동작 시간	


	
	INT32U d_FPGA_Delay_ns_DL[MAX_Optic][MAX_MIMO_PATH]; // FPGA Delay : (DL), 0xB429
	INT32U d_FPGA_Delay_ns_UL[MAX_Optic][MAX_MIMO_PATH]; // FPGA Delay : (UL), 0xB42A
	INT8U  d_RW_4T2R_Mode;

	INT8U  d_RW_TDD_Mode;         // 0x5420
	INT16U d_DL_ON_Guard_Time;    // 0x5421
	INT16U d_UL_ON_Guard_Time;    // 0x5422
	INT16U d_DL_to_UL_Guard_Time; // 0x5423
	INT16U d_UL_to_DL_Guard_Time; // 0x5424
	
	// MIMO Stdby AID	
	INT8U  MIMO_Stdby_Enable;	   // 0x5430, name change : 'MIMO_Stdby_Mode_Onoff' -> 'MIMO_Stdby_Enable'
	INT8U  Sum_MIMO_Stdby_St;      // 0x5431, MIMO Standby Mode 동작 상태, '0' is 'ON'
	INT8U  MIMO_Stdby_St[AMPU_4_MAX]; // 0x00 : Not MiMo Stnadby

	INT8U  Sum_AMP_Stdby_St;        // 0x5435
	INT8U  Batt_Ext_Mode_Onoff;    // 0x5438
	INT8U  Batt_Ext_Mode_St;       // 0x5439

	INT8U  UDA_Spurious_Layer;     // 0x544C, 2019.04.27, status only
	/*
		Bit 7: DL3(Alpha)
		Bit 6: DL2(Alpha)
		Bit 5: DL1(Alpha)
		Bit 4: DL0(Alpha)
		-------------------
		Bit 3: DL3
		Bit 2: DL2
		Bit 1: DL1
		Bit 0: DL0

		0: Payload On,
		1: Payload Off
	*/
	INT8U  d_Layer_Use[MAX_DL];     // PRU-ANTS Only, 2019.05.03, same with "AID 0x5002"
	
	// MIMO Stdby algorithm
	INT8U  AC_Normal__MIMO_Stdby_Re_ON_Req;	// when AC fail -> Normal, MIMO_Std_Enable Request 
	INT8U  MIMO_Stdby_Oper_Mode;    // AID not allocated..
		
	// AMP Stdby mode AID		
	INT8U  AMP_Stdby_Start_Time[2]; // 0x5432, [hour/mimute]
	INT8U  AMP_Stdby_Stop_Time[2];  // 0x5432, [hour/mimute]

	INT8U  AMP_Stdby_Enable;        // 0x5434, <0: On, 1: Off>, 'AMP_Stdby_Mode_Onoff' -> 'AMP_Stdby_Enable'
	                                // Same with User-control, but When AC-Fail, On or Off	
	INT8U  AMP_Stdby_Path;		    // 0x5437, <0: On, 1: Off>, 적용 path.
	INT8U  AMP_Stdby_Sync;          // sync for 'AMP-Standby-algorithm' and 'PAU-Task' 
	INT8U  MIMO_Stdby_Sync;          // sync for 'MIMO-Standby-algorithm' and 'PAU-Task' 

	INT8U MiMo_Stdby_Start_Time[2]; // 0x5436, [hour/mimute]
	INT8U MiMo_Stdby_Stop_Time[2];  // 0x5436, [hour/mimute]
	
	// AMP Stdby algorithm
	INT8U  AMP_Stdby_St[AMPU_4_MAX]; // 동작 상태, 1 이면, AMP stdby 시간이어서, 동작중.	
	INT8U  AMP_Stdby_St_Dly_REQ[AMPU_4_MAX]; // AID not allocated..
	INT8U  AC_Normal__AMP_Stdby_Re_ON_Req;	 // when AC fail -> Normal, AMP_Std_Enable Request 

	INT8U  TestPort_CIM; // Test Port 출력	0x543F, '1' is OFF
	INT8U  UL_Gain_A_BackOff; // AID 0x5450, 2019.05.13, Auto-Backoff
// -------------------------------------------------
// AID : 0xB0__
// -------------------------------------------------
	INT32U UTC;             // 0xB003, Time_Tick_UTC current time, 4-byte
	INT8U  LS_Regist_Num[20];    // 0xB007, 'Serial_Num' -> 'LS_Regist_Num'
	INT8U  d_RU_Initial_St; // 0xB009, Needed.
	INT8U  d_Alarm_Ack;     // 0xB00A, Needed.
	INT8U  d_Alarm_Valid_Indicator;// 0xB00B, Needed.
	INT8U  BT_Device_Code[30];	/////YCSO added 190812


// -------------------------------------------------
// AID : 0xB1__
// -------------------------------------------------	
	INT8U  d_RW_UL_OnOff_IT[2][MAX_MIMO_PATH]; // AID not allocated..2019.04.13, deleted
	INT8U  d_RW_UL_OnOff_CS[2][MAX_MIMO_PATH]; // AID not allocated..2019.04.13, deleted

// -------------------------------------------------
// AID : 0xB2__
// -------------------------------------------------
	

	INT16S AMP_Temp_x10[AMPU_4_MAX];   // 0xB200, 
	INT8U  AMP_Temp_Onoff[AMPU_4_MAX]; // 0xB201,
	INT8U  PAU_I_Ver[AMPU_4_MAX][3]; // 0xB202,
	INT8U  Pwr_Amp_Made[AMPU_4_MAX]; // 0xB203,
	INT8U  Inv_Amp_Made[AMPU_4_MAX];
	INT8U  Pwr_Amp_Watt[AMPU_4_MAX]; // 0xB204,
	INT8U  Pwr_Amp_SN[AMPU_4_MAX][4]; // 0xB205, ASCII 4byte,			
	INT16U Ret_Loss_x10[AMPU_4_MAX]; 	 // 0xB208,
	INT16U Ret_Loss_Thr_x10[AMPU_4_MAX]; // 0xB209,
	INT16U Ret_Loss_Thr_Rec_x10[AMPU_4_MAX]; // 0xB20A,
	INT16U Over_Temp_Thr_x10[AMPU_4_MAX];    // 0xB20B,
	INT16U Over_Temp_Rec_Thr_x10[AMPU_4_MAX];// 0xB20C,
	INT16U Over_Pwr_Thr_x10[AMPU_4_MAX];     // 0xB20D,
	INT8U  Pwr_Amp_Link_Err[AMPU_4_MAX]; // 0xB20E, (0:normal, 1:abnormal)	

	INT16U VSWR_x100[AMPU_4_MAX];       // 0xB207
	INT16U VSWR_Amp__On_x100[AMPU_4_MAX]; // 2020.01.28
	INT16U VSWR_Amp_Off_x100[AMPU_4_MAX]; // 2020.01.28
	
	INT16U AMP_DC_Volt_x10[AMPU_4_MAX]; // 0xB206,
	INT16S FWD_Pwr_Offset_x10[AMPU_4_MAX];   // 0xB210,	
	
	INT16S Rvs_OUT_Pwr_Offset_x10[AMPU_4_MAX];	// Offset(UL LNA 출력), 0xB212
	INT16S Rvs_IN_Pwr_Offset_x10[AMPU_4_MAX];   // 0xB213,
	
	INT16S FB_Pwr_Offset_x10[AMPU_4_MAX];   // 0xB214, AMP Feedback Pwr Offset, 
	INT16U FWD_Dly_Offset_x100[AMPU_4_MAX]; // 0xB215,
	INT16U RVS_Dly_Offset_x100[AMPU_4_MAX]; // 0xB216,	
	INT16S FB_Pwr_x10[AMPU_4_MAX];          // 0xB21A, AMP Feedback Pwr, 
	INT16S Rvs_OUT_Pwr_x10[AMPU_4_MAX];	    // 0xB21B,	
	
	INT8U  FB_ATT_x2[AMPU_4_MAX];         // 0xB221, AMP Feedback Pwr Offset, 
	INT8S  FB_Temp_Att_x2[AMPU_4_MAX];    // 0xB226, AMP Feedback Temp Att, 
	INT8U  FB_Offset_ATT_x2[AMPU_4_MAX];  // 0xB228, AMP Feedback Pwr Offset, 
	
	
	INT8U  Fwd_TPL_Att_x2[AMPU_4_MAX];  // 0xB220,
		
	INT16S Rvs_IN_Pwr_x10[AMPU_4_MAX];  // AID Not Allocated
	INT16S Fwd_IN_Pwr_x10[AMPU_4_MAX];  // AID 0xB21D
	
	INT8S  Fwd_Temp_Att_x2[AMPU_4_MAX]; // 0xB222, PRU/AAU Common
	INT8S  Rvs_Temp_Att_x2[AMPU_4_MAX]; // 0xB223, PRU/AAU Common
	INT8U  Rvs_Att_x2[AMPU_4_MAX];		// 0xB224, AMP RVS ATT
	
	INT8U  TX_ALC_Att_x10[AMPU_4_MAX];   // 0xB22A, AAU, TX ALC
	INT8S  RX_ALC_Att_x2[AMPU_4_MAX];   // 0xB225, PRU/AAU Common
	
	INT8S  RX_Qck_Att_x2[AMPU_4_MAX];   // 2019.04.23. RVS IN Quick ALC Atten.(ANTS-AAU)

	INT8S  FWD_Att_Offset_x2[AMPU_4_MAX]; // 0xB227,
	INT8U  Rvs_Att_Offset_x2[AMPU_4_MAX]; // 0xB229,

	INT8U  Rvs_IN_ALC_Onoff[AMPU_4_MAX];  // 0xB230,
	INT8U  ATT_Pair_Onoff[AMPU_4_MAX];    // 0xB231, 
	INT8U  AMP_Auto_SD_Mode_En[AMPU_4_MAX]; // 0xB232,
	INT8U  Rvs_Gain_Bal_On[AMPU_4_MAX];  // 0xB233,		
	INT16S Rvs_IN_ALC_x10[AMPU_4_MAX];   // 0xB234, ALC Level, 	
	INT16S Rvs_IN_SD_x10[AMPU_4_MAX];    // 0xB235, SDLevel, 
	INT8S  Rvs_Gain_Bal_x2[AMPU_4_MAX];  // 0xB236,
	INT8U  RVS_Amp_Onoff[AMPU_4_MAX];	 // 0xB237, UL LNA On/Off			
	INT8U  AMP_DL_St[AMPU_4_MAX][2];     // 0xB23A, [Dl_State/%]

	INT8U  Fwd_AMP_Alm[AMPU_4_MAX];       // PAU FWD AMP On/Off Status, (
	// ------------------------------------
	// Not allocated
	// but
	// below will be used later...
	// ------------------------------------
	
	INT8U  Pwr_Amp_Off_Reason[AMPU_4_MAX]; // Reason : RCU/DTU or PAU/RFU AMP
	INT8U  Off_Reason____Prev[AMPU_4_MAX]; // save previous reason which is not 0 
	INT8U  Pwr_Amp_Off_Reason_PAU[AMPU_4_MAX]; // Reason only from "PAU(PRU) or RFU(AAU)"
	INT8U  Pwr_Amp_Ver[AMPU_4_MAX];
	INT8U  Pwr_Amp_Date[AMPU_4_MAX][3];		
	INT8U  Pwr_Amp_Reset[AMPU_4_MAX];	
	INT8U  Fwd_AMP_Var_Att_x2[AMPU_4_MAX];
	INT16U RX_AMP_Det_x1000[AMPU_4_MAX];
	INT8U  Pwr_Amp_EQ[AMPU_4_MAX]; // bit7~4:Main, bit3~0:Alpha


// -------------------------------------------------
// AID : 0xB3__
// -------------------------------------------------
	// 2019.05.04
	INT16S RSSI_Before_Filter_x10[AMPU_4_MAX];   // 0xB302, RSSI(Filter 전)
	INT16S RSSI_After__Filter_x10[MAX_Optic][AMPU_4_MAX];   // 0xB303, RSSI(Filter 후)
	INT16S UL_DTU_Gain_Comp_x10[MAX_Optic][AMPU_4_MAX];   // 0xB304, UL DTU Gain 보상(타사 과입력)
	INT16S d_UL_Noise_RMS_x10[MAX_Optic][MAX_UL];  // 2019.05.14, 0xB306, RSSI(UL Noise)
	
// -------------------------------------------------
// AID : 0xB4__
// -------------------------------------------------
	// DPD, CFR : // PRU Only
	INT16S d_DTU_Temp_x10;     // 0xB400, temperature-DTU
	INT16S d_R_FPGA_Temp_x10;  // 0xB401, temperature DTU FPGA
	INT8U  d_SW_VER_DTU[3];    // 0xB402, S/W Version DTU,
	INT8U  d_SW_VER_FPGA[3];   // 0xB403, S/W Version FPGA, ANTS : FPGA Register	
	
	
	INT8U  d_SW_VER_DPD[3];    // DPD Version, AID not allocated..
	INT8U  d_DPD_OnOff[MAX_DL];// DPD onoff, 0xB404, ON(0),OFF(1)
	INT16U d_DPD_St[MAX_DL];   // DPD St, 2019.05.17, length is 2byte. 2019.05.20 : AID 0xB405 -> 0xC032

	INT16S d_DPD_In__Pwr_x10[MAX_DL];  // 0xB406
	INT16S d_DPD_Out_Pwr_x10[MAX_DL];  // 0xB407
	
	INT16S d_DPD_Eng_In__Pwr_x10[MAX_DL];
	INT16S d_DPD_Eng_Out_Pwr_x10[MAX_DL]; // 0xB408	
	INT16S d_DPD_FB_Eng_In_Pwr_x10[MAX_DL];  // 0xB409
	INT16S d_DPD_FB_In__Pwr_x10[MAX_DL];  // 0xB413

	
	INT8U  d_DPD_Reason[MAX_DL]; // AID not allocated..
	INT8U  d_SW_VER_CFR[2];      // CFR Version

	INT8U  d_CFR_OnOff[MAX_DL];     // 0xB40A
	INT32U d_CFR_Level_x10[MAX_DL]; // PRU(0xB40B, 2byte), AAU(0xC069, 4byte)
	

	
	INT16S d_Hard_Clip_Lvl_x10[MAX_DL]; // 0xB40D, 2019.04.22, changed.	
	INT32U d_Hard_Clip_Cnt_DL[MAX_DL]; // 0x5320
	INT8U  d_Hard_Clip_Cnt_Init;       // 0xB40F, Count 초기화, control only
	INT8U  d_Hard_Clip_Clear[MAX_DL];  // 0xB40F, from Register
	
	
	INT8U  d_DPD_Reset;	// 0xB411, Count 초기화, control only

	
	
	
	
	INT8U  d_CPRI_Recovery_Path_St;    // 0xB419, Needed.
	INT8U  d_Clock_Status;             // 0xB41A, Needed.
	INT8U  d_Func_Fail_VSS[MAX_Optic]; // 0xB41B, Needed.

	///// YCSO change d_Delay_Alarm_Layer[2] --> d_Delay_Alarm_Layer[2][MAX_Optic]

	INT8U  d_Delay_Alarm_Layer[2][MAX_Optic];     // 0xB41C, Needed
	INT32U d_Delay_Set_Val[MAX_Optic]; // 0xB41D, Needed, [nsec]
	
	INT8U  d_RW_DL_BW_Sel[MAX_FA_PATH][MAX_MIMO_PATH]; // 0xB41E
	INT8U  d_RW_UL_BW_Sel[MAX_FA_PATH][MAX_MIMO_PATH]; // 0xB41E

	///// YCSO change d_ARFCN --> d_ARFCN[MAX_Optic]
	INT16U d_ARFCN[MAX_Optic];          // 0xB41F, Needed

	INT16U d_Att_Dig_DL_x10[MAX_Optic][MAX_DL]; // 0xB420
	INT16U d_Att_Dig_UL_x10[MAX_Optic][MAX_DL]; // 0xB421
	/*
	@ AAU
	d_Att_Dig_UL_x10 = offset + Gain_Compensation
	(m_Eep.d_Att_Dig_UL_User_x10 is excluded)
	*/

	INT16S d_Temp_Att_Dig_DL_x10[Optic_2][MAX_DL]; // 0xB422, Singed
	INT16S d_Temp_Att_Dig_UL_x10[Optic_2][MAX_DL]; // 0xB423, Singed

	INT16S AAU_ALC_Gain_Comp_x10[MAX_Optic][MAX_DL]; // AAU Gain Comp (delta)
	INT16U d_Att_DL_SUM[MAX_DL]; // 0xB424,
	INT16U d_RU_UL__Gain_Backoff_x10;
	INT16U d_UL_DTU_Gain_Backoff_x10[MAX_Optic][MAX_MIMO_PATH]; // 0xB426, 0x5208, Gain Backoff, 2019.05.04.
	INT16U d_UL_DTU_Gain_Backoff_GUI_x10[MAX_Optic][MAX_MIMO_PATH]; // 0xB426, 0x5208, Gain Backoff, 2019.05.04.

	INT8U  d_Path_OnOff_DL[MAX_Optic][MAX_MIMO_PATH]; // 0xB42E
	INT8U  d_Path_OnOff_UL[MAX_Optic][MAX_MIMO_PATH]; // 0xB42F
	
	INT8U  d_Test_Pattern_OnOff[MAX_Optic][MAX_MIMO_PATH]; // 0xB430
	INT8U  d_Payload_OnOff[MAX_Optic][MAX_MIMO_PATH]; // Payload On/Off, 0xB431, 2019.04.13, 0(ON)	
	
	INT8U  d_Debug__OnOff_L2_SW;   // 0xB433
	INT8U  PRU_DTU_TempComp;       // 0xB433, PRU Temp Comp' OnOff (0 is ON), Status Only.	
	
	INT8U  d_Rx_Casc_Add_OnOff_UL;        // 0xB435
	INT8U  d_Rx_Casc_Add_OnOff[MAX_Optic][MAX_MIMO_PATH];
	
	
	INT8U  d_Loopback_OnOff;           // 0xB436, Needed
	INT16S d_Volt_DTU;                 // 0xB437, 48V(AAU : -48V input, DC Fail Check), 0.1 step
	INT16S d_Volt_DTU_Sub[6];          // 0xB438, 24V(AAU,Main), 0.1 step

	INT8U  d_DTU_Reset;	              // 0xB439, control only, Needed

	INT8U  DL_State[DN_MAX][2]; // [Dl_State/%], Needed
	/*
		DN_RCU : AID 0x2B30
		DN_PSU : AID 0xB63A
		DN_DTU : AID 0xB43A	
		DN_FPGA: AID 0xB43B
	*/

	INT16U CPU_Usage;  // 0xB43C, 0.1 unit
	INT16U Mem_Usage;  // 0xB43D, Needed, Memory Usage,
	INT8U  Link_L2_SW; // 0xB43E, Needed
	INT8U  d_SFP_Insert_St[MAX_Optic][2]; // 0xB43F, 0(mount), 1(Unmount)
	INT8U  SFP_Ins; // used at iro, but do not delete...

	// 2020.07.22
	INT8U  d_SFP_Vendor___PN[CPRI_PATH][16]; // 0x5316
	INT8U  d_SFP_Vendor___SN[CPRI_PATH][16];
	INT8U  d_SFP_Vendor_Name[CPRI_PATH][16];
	INT8U  d_SFP_Date___Code[CPRI_PATH][8];
	
	// ------------------------------------------
	// VSS.Z.XXX
	// ------------------------------------------	
	INT8U d_CPRI_Status[2][2]; // 0xB440
	// -----------------------
	INT8U d_VSS_Z__2__CPRI_Version[CPRI_PATH][DL_UL];    // 0xB441, AID : Send DL only
	INT8U d_VSS_Z_66__CPRI_Startup[CPRI_PATH][DL_UL];    // 0xB442, AID : Send DL only
	INT8U d_VSS_Z_130_CPRI_L1_RST_LOS[CPRI_PATH][DL_UL]; // 0xB4x43, AID : Send DL only
	INT8U d_VSS_Z_194_Pointer_P[CPRI_PATH][DL_UL];       // 0xB4x44, AID : Send DL only
	// -----------------------
	INT8U d_VSS_Noti_IP_1[CPRI_PATH][4]; // 0xB445, main cpri DL, alpha cpri DL
	INT8U d_VSS_Noti_IP_2[CPRI_PATH][4]; // 0xB446, main cpri DL, alpha cpri DL

	INT8U d_VSS_TDD_Slot_Num[CPRI_PATH][20]; // 0xB447, slot 0~19,  // 2019.04.19, INT32U -> INT8U
	
	INT8U d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_PATH][DL_UL];           // 0xB450
	INT8U d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_PATH][DL_UL];            // 0xB451
	INT8U d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_PATH][DL_UL];      // 0xB452
	INT8U d_VSS_Z_208__CnM_Path_Select[CPRI_PATH][DL_UL];             // 0xB453
	INT8U d_VSS_Z__17__ReReset___ReResetAck[CPRI_PATH][DL_UL];        // 0xB454
	INT8U d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_PATH][DL_UL];      // 0xB455
	INT8U d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_PATH][DL_UL]; // 0xB456
	INT8U d_VSS_Z_149__CpriCompressOnOff[CPRI_PATH][DL_UL];           // 0xB457
	INT8U d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_PATH][DL_UL];      // 0xB458
	INT8U d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_PATH][DL_UL];      // 0xB459
	INT8U d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_PATH][DL_UL];    // 0xB45A

	INT8U d_VSS_Z__23__Re1Reset___ACK[CPRI_PATH][DL_UL];              // 0xB45B
	INT8U d_VSS_Z__87__Re2Reset___ACK[CPRI_PATH][DL_UL];              // 0xB45C
	INT8U d_VSS_Z_151__Re3Reset___ACK[CPRI_PATH][DL_UL];              // 0xB45D
	INT8U d_VSS_Z_215__Re4Reset___ACK[CPRI_PATH][DL_UL];              // 0xB45E
	
	INT8U d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_PATH][DL_UL]; // 0xB45F
	INT8U d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_PATH][DL_UL]; // 0xB460
	INT8U d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_PATH][DL_UL]; // 0xB461
	INT8U d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_PATH][DL_UL]; // 0xB462
	
	INT8U d_VSS_Z__30__Reserved___ReVendorID[CPRI_PATH][DL_UL];       // 0xB464
	INT8U d_VSS_Z__60__Rec_PortNum[CPRI_PATH][DL_UL];                 // 0xB465

	INT8U   VSS_Monitor_OnOff; // 0xB46E, Needed
	/*
		0x00: 미보고
		0x01: CPRI0
		0x02: CPRI0 +α
		0x03: CPRI1
		0x04: CPRI1 +α
	*/
	INT8U   VSS_Rx[255];       // 0xB46F, Needed
	INT8U   VSS_Tx[255];       // 0xB470, Needed

// -------------------------------------------------
// AID : 0xB5__
// -------------------------------------------------

// -------------------------------------------------
// AID : 0xB6__
// -------------------------------------------------
// PSU
	INT8U  PSU_Link_St;    // 0xB60E
	INT8U  FAN_Auto[2];    // 0xB613, 0xB617, [CASE/PSU], 자동 제어 
	INT16U FAN_Ext_Max_RPM;// 0xB618
	INT8U  FAN_OnOff[2];   // [CASE/PSU]
	INT8S  FAN_On_Temp[2]; // 0xB610, 0xB614, [CASE/PSU], 동작 온도
	INT8S  FAN_OffTemp[2]; // 0xB611, 0xB615, [CASE/PSU], 동작 온도
	INT8U  AAU_Fan_Speed;  // AAU_Fan_Speed
	/*
	// 0xB620 ~ 25 : PSU Structure is used
	*/
	INT8U  PSU_Reset;	   // 0xB639, control only, Needed


// --------------------------------------------------------------------------------------------
// ANT AAU Add
// --------------------------------------------------------------------------------------------	
	union 
	{
		INT8U	Operate_Ip[4];
		INT32U	Operate_Ip32;
	}; // 2019.05.29
	INT8U Operate_Mac[6];

// -------------------------------------------------
// AID : 0xC0__ : for ANTS
// -------------------------------------------------

	INT8U  d_DTU_App_MB_St; // 0xC000, DTU App/MB(microbraze) status
	INT8U  d_FPGA_St;		// 0xC001
	INT8U  d_DTU_Mode;		// 0xC002
	INT16U d_RE_Reset;		// 0xC003
	INT16U d_RE_Reset_Clr;	// 0xC004, control
	
	INT32U d_NCO[MAX_Optic];// 0xC010
	INT32U d_NCO_PRU[MAX_Optic][DL_UL];// 0xC010, 2019.05.30
	
	INT8U  d_NCO_VALID[MAX_Optic]; 	// 0xC011
	
	INT16U d_DL_CW_LEVEL[2][4];  // 0xC012
	INT8U  d_DL_CW_ON[2][4];	 // 0xC013, GUI & Regiter (0:OFF, 1:ON)
	INT8U  User_DL_CW_OnOff[MAX_FA_PATH][MAX_MIMO_PATH];  // 0xC013, 2019.09.25, On is '1' @ AID

	INT16S d_EQ_Coeff_DL[MAX_FA_PATH][MAX_MIMO_PATH]; // 0x540A, 0xC014
	INT16S d_EQ_Coeff_UL[MAX_FA_PATH][MAX_MIMO_PATH]; // 0x540B, 0xC019
	
	INT16S d_EQ_Coeff_1_DL[MAX_FA_PATH][MAX_MIMO_PATH]; 	// 	0xC01A, AAU Only
	INT16S d_EQ_Coeff_1_UL[MAX_FA_PATH][MAX_MIMO_PATH]; 	// 	0xC01B, AAU Only

	INT8S  EQ_Toggle[MAX_FA_PATH][DL_UL][2];
	
	INT8U  d_DL_TSSI_BalAtt_x10[MAX_FA_PATH][MAX_MIMO_PATH]; // 0xC01C
	
	INT16U d_LSH_Gain_x10[DL_UL][MAX_FA_PATH][MAX_MIMO_PATH];    // from register
	INT16U d_Test_Pattern_Band_Gain[MAX_FA_PATH][MAX_MIMO_PATH]; // 0xC065
	INT32U d_TSSI_SD_Level_x10[MAX_FA_PATH][MAX_MIMO_PATH][3];   // 0xC01D/1E/1F
	
	// 2019.04.09
	INT16S d_Gain_DL_x10[MAX_FA_PATH][MAX_MIMO_PATH]; // 0xC015, 0.1 step, signed, ANTS-AAU Only
	INT16S d_Gain_UL_x10[MAX_FA_PATH][MAX_MIMO_PATH]; // 0xC016, 0.1 step, signed, ANTS-AAU Only


	INT16U d_UL_Gain_Guard_time[5]; // 0xC020
	
	INT32U d_t_Sync_Delay;	// 0xC021, usec
	INT8U  d_t_Sync_Fail;	// 0xC022
	//--------------------------------
	// PRU Only, 2019.06.01
	INT32U d_Capture_DL_Time; // 0xC023, usec
	INT32U d_Capture_UL_Time; // 0xC024, usec
	INT32U d_Capture_ULNoise_Time; // 0xC025, usec
	INT32U d_UL_Noise_RSSI_StartTime; // 0xC026, usec
	INT32U d_UL_Noise_RSSI_StopTime;  // 0xC027, usec
	INT32U d_UL_RSSI_StartTime; // 0xC028, usec
	INT32U d_UL_RSSI_StopTime;  // 0xC029, usec
	//--------------------------------
	INT8U  d_CFR_Ver[2];	// 0xC030
	INT8U  d_DPD_Ver[3];	// 0xC031
	INT32S d_DPD_Err_St[MAX_DL]; // 0xB405 : 2019.05.20, AID Changed by Regiseter-Document
	INT32U d_DPD_Success_Cnt[MAX_DL];	   // 0xC033
	INT32U d_DPD_Eng_InPwr[MAX_DL]; // 0xC034

	INT8U  UL_AFE_ATT_x1[AMPU_4_MAX]; // 0xC037, AAU Only, 2019.08.13, 1dB Step

	INT16S d_DPD_PeakExp_x10[MAX_DL]; // 2019.05.20
	INT16S d_DPD_____MSE_x10[MAX_DL]; // 2019.05.20
	INT16S d_DPD__RE_MSE_x10[MAX_DL]; // 2019.05.20
	INT32U d_DPD_LUT_Value[MAX_DL];   // 2019.05.20
	INT32U d_DPD_Counts[MAX_DL];      // 2019.05.20
	
	INT8U  d_CPRI_St[2];	   // 0xC040
	INT16U d_CPRI_Misc_St[2];  // 0xC041
	INT8U  d_CPRI_eth_FIFO_Reset[2]; // 0xC042, control only
	INT32U d_BIP_Val[4];	   // 0xC043
	INT32U d_BIP_Val_Debug[4]; // 0xC044
	INT8U  d_BIP_Clr[4];	 // 0xC045, 0xB428 control only, 2019.05.09 : [2] -> [4]

	INT8U  Gain_Noise_Auto; // 0xC060
	INT8U  ANTS_LED_Status; // 0xC061
	INT8U  Wave_Check;

	INT32U d_UL_Gain_Meas_time; // AID Not Allocated

	


	// from Layer Splitter //////////////////////////////////////////////////////////////////////
	// 2019.04.18
	INT32U LS_Delay_DL_nsec[MAX_Optic]; // Delay Setting Value from LayerSplitter, T2a(DL Delay), AID 0x5403
	INT32U LS_Delay_UL_nsec[MAX_Optic]; // Delay Setting Value from LayerSplitter, Ta3(UL Delay), AID 0x5404


	// 2019.07.08
	INT16U d_AAU_Test_Pattern_addr[MAX_FA_PATH]; // AAU Only
	INT16S d_AAU_Test_Pattern_data[MAX_FA_PATH][_IQ_]; // AAU Only
	INT8U  d_AAU_Test_Pattern_wrEn[MAX_FA_PATH]; // AAU Only

	
	INT32S LS_Dly_Ctrl_ns[MAX_Optic][DL_UL]; // 0xC071	
	INT32S LS_Dly_Ret32; // 0xC075
	INT16U Curr_LS_Gain_Bal_x10[MAX_FA_PATH][DL_UL][MAX_MIMO_PATH]; // AAU

	char   DDR_Mem_Clk_Info[20]; // AID 0xC0A5, DDR-Memory Clock info AID(0xC0A5) Added.
	

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//---------------------------------------
// legacy iro --------
//---------------------------------------
//
//	 Do not delete !!
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////	

	INT8U	Mimo_Stdby_St_Dly_REQ[AMPU_4_MAX]; // 1 
	INT8U	Mimo_Stdby_AMP_Off_St[AMPU_4_MAX]; // 1 is off
	

	INT16U BIP_ErrCnt[MAX_Optic][DL_UL][CPRI_PATH];
	
	INT8U  CPRI_Alm_Reason;
	INT8U  Func_Fail_VSS; // Function Fail (VSS)
	
	INT16U	Delay_Set_x10; // delay setting value
	INT16U	Delay_St_x100; // result, calculation
	INT16U	Delay_CC_x100; // Cascade Delay
	
	INT32U Bip_Accum_Cnt[BIP_NUM]; //  누적시간에 해당하는 누적카운트. 바로 전의 누적 cnt. 0xA52A/33	
	INT32U Bip_Accum_Cnt_Cur[BIP_NUM];
	INT8U  Bip_Err_Ref; // Err가 몇회일 때, 알람처리?
	INT16U Bip_Err_Accum_Time; // 누적시간, 1~1000초
	INT8U  EBER; // 0x114C
	INT16U BIP0_Err_Cnt; // 0xA27B
	INT32U Bip_Accum_Time_ms_Cur; // 현재 누적시간, 1~1000초.
	INT8U  Bip_Err_Cur[4]; //  [BIP/Ext.BIP], 현재 누적 Err 수. 최대 Bip_Err_Ref 값.
	//---------------------------------------
	
	
	INT32U RW_Slot_Format[20];

	

	INT32U TSYNC_FAIL;

	INT8U  DTU_LOS_LOF_Alm; // 5G
	
	INT8U  Green_Mode_Req; // 0x081D, <0: On, 1: Off>, Measure 항목.
	INT8U  Green_Mode_Start_Time[2];// 0x081E, [시/분]
	INT8U  Green_Mode_Stop_Time[2]; // 0x081F, [시/분] 
	
	// --------------------------------------------------------------------------------------------
	// Download
	// --------------------------------------------------------------------------------------------
	DOWN_ST Down_St;		
	INT8U AMP_FW_NACK[AMPU_4_MAX]; // 1 : nack, 0 : ack
	INT8U PSU_FW_NACK; // 1 : nack, 0 : ack

	// --------------------------------------------------------------------------------------------
	// etc
	// --------------------------------------------------------------------------------------------
	INT8U Supp_Arm_f[RU_ALARM_CNT];
	INT8U stat_start_time[6];// 구간의 시작시간. // 0x237A : 실제 통계를 한 시작시간.

	// Compress download
	INT8U comp_dn_ro_kind; // self Ro 종류
	/*
	 0 : etc-Ro
	 1 : Ro#0@Br or 
	 2 : module-server-RO
	*/
	INT8U comp_dn_st_AE08;
	/*
		0 : none
		1 : sever ro downloading now
		2 : sever ro download end / un-compress start
		3 : un-compress end /  ftp server start
		4 : ftp server start end / start internal download
		6 : inter download end / wait reset-command
		7 : waiting reset-command / Reset
	*/
	INT8U Log_Prev; // AID 0x5453
	INT16U Log_Cnt;  // AID 0xD454, 2020.02.11, INT8U -> ONT16U
	INT8U Log_Alm_Ctl_Cnt;  // AID 0x5454
	INT8U AAU_LED_Auto_Off; // AID 0x5455
	INT8U AAU_LED_Disable;  // related with AID 0x5455
	
			
	INT16U	FWD_FPGA_Dly_x100[AMPU_4_MAX]; // dly 설정값, 0.1us, 0xA2A6 /0xA2A7 / 0xAC15 /16 / 0xAC17/18 / 0xAC19/1A
	INT16U	RVS_FPGA_Dly_x100[AMPU_4_MAX]; // dly 설정값, 0.1us, 0xA2AB /0xA2AD / 0xAC1B /1C / 0xAC1D/1E / 0xAC1F/20
		
	INT8S	PD_Pwr_x2;		   // 0x030D
	INT8S	PD_Pwr_Low_lmt_x2; // 0x030F
				
	INT8U Optic0_Alarm;
	INT8U Dig_FWD_Path_Req_F[AMPU_4_MAX]; // AID 0x082C/2D, LTE/LTEA, Fwd Path Select, 제어여부.
	INT8U Dig_FWD_Path_Req_Val[AMPU_4_MAX]; // AID 0x082C/2D, LTE/LTEA, Fwd Path Select, 실제 user 제어값.

	INT8U kst_inventory_SN[4];
	INT8U kst_inventory_date[3];
	
	
	//// --------------------------------------------------------------------------------------------

} ST_STR;




/*----------------------------------------------------------*/
// 외부 호출 함수
//
PROTOCOL_EXT INT8U Make_Tree_Info( INT8U* sdu, INT8U aid_needed );  
PROTOCOL_EXT INT16U GetStatus( INT8U* sdu, INT8U port );
PROTOCOL_EXT INT16U GetStatus_REMS_Hidden( INT8U* sdu, INT8U port );

PROTOCOL_EXT INT16U Make_AID( INT8U aid0, INT8U aid1, INT16U idx, void* paid, INT8U port, INT8U arg );
PROTOCOL_EXT void Disp_AID2(char* title, AIDSTR2* paid2, AID2_INFO* paid2_info);
PROTOCOL_EXT void Print_AID2(char* title, AIDSTR2* paid2, AID2_INFO* paid2_info, char* disp_str, INT16U max_buff);


/*----------------------------------------------------------*/
// 외부 참조 변수
//

PROTOCOL_EXT INT16U Make_AID_0x00__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x01__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x02__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x09__( void* paid, INT16U idx, INT8U port, INT8U arg );

PROTOCOL_EXT INT16U Make_AID_0x2B__( void* paid, INT16U idx, INT8U port, INT8U arg );

PROTOCOL_EXT INT16U Make_AID_0x50__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x51__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x52__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x53__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x54__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x55__( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_0x56__( void* paid, INT16U idx, INT8U port, INT8U arg );



PROTOCOL_EXT INT16U Make_AID_Internal_B0( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_B1( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_B2( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_B3( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_B4( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_B5( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_B6( void* paid, INT16U idx, INT8U port, INT8U arg );

PROTOCOL_EXT INT16U Make_AID_Internal_C0( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_CF( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_F2( void* paid, INT16U idx, INT8U port, INT8U arg );
PROTOCOL_EXT INT16U Make_AID_Internal_D4(void* pAID, INT16U idx, INT8U port, INT8U arg);







PROTOCOL_EXT INT8U RU_ARM_SUPP[RU_ALARM_CNT]; // Suppression.







//-----------------------------------------------------------------------------
//
//	EMS
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#define EMS_STATIC_PKT_BUF_MAX (3)


#define  ____o____ (0)
#define  _____o_____ (0)
#define  __o__ (0)
#define  _r_ (0) // reserved


//---------------------------------------------------
enum
{	
	_Measure_ = 0x01, // 순시 값
	_STatist_ = 0x02,
	_CF_CTRL_ = 0x04,	
	_CF_nCTL_ = 0x08,	
	__Alarm__ = 0x10,
	_Operate_ = 0x20, // Ctrl Only ( 예 : reset )
	___GUI___ = 0x40,
	__Hidden_ = 0x80  // Local AID
};
//---------------------------------------------------
enum
{	
	_PRU_ = 0x01, // SISO
	_AAU_ = 0x02,
};
//---------------------------------------------------
enum
{
	___None = 0x00, // 미사용
	__INT8S = 0x01, // signed char
	__INT8U = 0x02, // unsigned char
	_INT16S = 0x03, // signed int
	_INT16U = 0x04, // unsigned int
	_INT32S = 0x05, // signed long int
	_INT32U = 0x06, // unsigend log int
	___BOOL = 0x07, // boolean
	__ASCII = 0x08, // ASCII
	_PrdDef = 0x09, // pre-defined	
	
	Type_Max 
};
//---------------------------------------------------
enum
{
	Step_No_Use = 0x00, // 미사용
	Step_____01 = 0x01, // 0.1
	Step____001 = 0x02, // 0.01	
	Step___0001 = 0x03, // 0.001
	Step__00001 = 0x04, // 0.0001
	Step_000001 = 0x05, // 0.00001

	Step_____10 = 0x11, // 10
	Step____100 = 0x12, // 100 
	Step___1000 = 0x13, // 1000
	Step__10000 = 0x14, // 10000
	Step_100000 = 0x15, // 100000
	
	Step____Max 
};
//---------------------------------------------------

typedef struct __EMS_AID_FUNC__
{
	INT16U (*aid_func)( void* paid, INT16U idx, INT8U port, INT8U arg );	
} EMS_AID_Func;

//typedef INT16U (*)( void* paid, INT16U idx, INT8U port, INT8U arg ) aid_func_t;

typedef struct __attribute__((packed))
{
	INT8U aid_0;
	INT8U aid_1;
	INT8U attr;  // attribute (CF_nCTRL, CF_CTRL, Measure, Statistic, Alarm,,,)	
	INT8U dev_f; // PRU, AAU
	//-------------------
	INT8U type;
	INT8U step;
	INT8U size;
	INT8U layer;   // layer num	
	INT8U carrier; // carrier num
} SKT_AID_STR;


//---------------------------------------------------

//-------------------------------
// EMS
//-------------------------------
enum
{
	_EMS_CFG__C_ = 0,
	_EMS_CFG_nC_,

	_EMS_AUTO_MAX_
};
//-------------------------------

typedef struct __attribute__((packed))
{
	INT8U Amp[AMPU_4_MAX];
	INT8U Rcu;
	INT8U Psu;
	INT8U Dig;	
	INT8U pkg;	///// YCSO added 190521
	INT32U	down_load_Size;
} DownMode;

//-------------------------------
typedef struct __attribute__((packed))
{	
	INT32U  Time32_RepRef_Hour;   // 통계 구간 시작 : 매시 0 분.
	INT32U  Time32_Rep_HourMin; // 통계를 REMS 보고 : 통계 기준 시/분 + RO 분산 + 통계주기
	
	INT32U	time32[3];	    // 계산된 통계시작/끝/보고시간.

	INT8U   time_change;
	INT8U   report_skip;
	INT8U   f_1_time_1_day; // 1일 1회.
	INT8U   reserve;
} STATIS_INFO;
//-------------------------------

///// YCSO added START 190503
#ifdef __MAKER_ANTS__
typedef struct __attribute__((packed))
{
	INT8U	tbl_num;
	INT8S	init_val;
	INT8U	entry_cnt;
	INT8S	entry[ MAX_AMP_TEMP_COUNT ];		
}Tbl_AMP_TEMP;							// eep size 32

typedef struct __attribute__((packed))
{
	INT8U	tbl_num;
	INT8S	init_val;
	INT8U	entry_cnt;
	INT8S	entry[ MAX_ATT_TBL_COUNT ];		
}Tbl_ATT;							// eep size 128


typedef struct __attribute__((packed))
{
	INT8U	tbl_num;
	INT8S	init_val;
	INT8U	entry_cnt;
	INT16U	entry[ MAX_PWR_TBL_COUNT ]; //eep size 64
}Tbl_DETEC_POWER;


#endif

///// YCSO added END




enum
{
	_NO_EMS_ALARM_ = 0,
	_ADD_EMS_ALARM_,
};

// 1024 over packet 에 대한 처리.
#define MAX_PKT_NUM (10)
/*
packet max size is 1024

header prefix : 10
====> total : 28 ---> 30byte
*/
#define UDP_MAX_LEN       (1024)
#define LEN_STX_CRC_ETX   (21) // header & ETX: 0x16~~(4) +info(4+3) +subid(3) + cmd/rcode/len(4) + CRC/ETX(3) = 21
#define LEN_PREFIX        (10) // prefix : 0xFFFFFF~~~

#define LEN_0xFFFE        (9) // time
#define LEN_0x0001        (3 + RU_ALARM_CNT*2)
//#define LEN_0x237A        (3 + 6)
//#define LEN_0x2379        (3 + 1)

#define LEN_0x5602        (11 + 6) // AID : New AID : 통계 시작 시간  0x5602
#define LEN_0x5601        (11 + 1) // AID : New AID : 통계보고 주기   0x5601


//#define MAX_LEN_SUB_DATA_TOPO ( UDP_MAX_LEN - LEN_STX_CRC_ETX - LEN_PREFIX ) // 1024 - 
#define MAX_LEN_SUB_DATA_ALM  ( UDP_MAX_LEN - LEN_STX_CRC_ETX - LEN_PREFIX - LEN_0xFFFE) // 1024 - 
#define MAX_LEN_SUB_DATA	  ( UDP_MAX_LEN - LEN_STX_CRC_ETX - LEN_PREFIX ) // 1024 - 
#define MAX_ST2_LEN_SUB_DATA  ( UDP_MAX_LEN - LEN_STX_CRC_ETX - LEN_PREFIX - RU_ALARM_CNT*2) // 1024 - 
#define MAX_LEN_STATIS		  ( UDP_MAX_LEN - LEN_STX_CRC_ETX - LEN_PREFIX - LEN_0x0001 - LEN_0x5602 - LEN_0x5601) // 1024 - 
//------------------------------


// 1kbyte x 10 structure for Auto Report to gREMS
typedef  struct
{	
	INT8U cur;	
	INT8U tot;
	INT8U use;
	INT8U cmd;	


	INT16U seqNo16;	
	INT8U  ip[4];
	INT16U port16;	

	INT8U  data[UDP_MAX_LEN * MAX_PKT_NUM];
	INT16U size[MAX_PKT_NUM]; // data[] 실제 size
	INT16U l;
	INT16U aid_cnt;

	INT8U AID[2];
	INT8U len;
	INT8U  reserved;
	
} REMS_1024_Pkt;


PROTOCOL_EXT INT16U Get_GUI_AID_Val( INT16U* pAid_list, INT16U max_num, INT8U* sdu, INT8U port, INT8U alarm_add );

PROTOCOL_EXT INT16U Make_SKT_AID( INT16U idx, void* paid, INT8U port );
PROTOCOL_EXT INT8U Is_AID2_Range(INT8U aid0);





enum
{
	_start_ = 0,
	_stop_,
	_rpt_
};

enum
{
	_1_BYTE_ = 1, // 1byte 짜리 AID
	_2_BYTE_ = 2, // 2byte 짜리 AID

};

enum
{
	_Signed_ = 1,
	_unSigned_= 0,
};




typedef struct __attribute__((packed))
{
	//------------ 
	INT8U aid[2];
	INT16U len; // 6 + data[flag + data], A_ID 및 Length 필드 길이 제외
	//------------
	INT8U type; // Type___None ~ Type_PrdDef
	INT8U step; // Step_No_Use ~ Step_100000
	INT8U reserved;
	INT8U size;  // data 1 size
	INT8U layer; // layer num
	INT8U carrier; // carrier num	
	//------------
} STAT_AID;



typedef struct __attribute__((packed))
{	
	STAT_AID aid_stat;
	
	INT8S   sign; // 0 or 1(signed)
	INT8U   flags[2];
	INT8U   f_size; // flags size : 1 or 2
	//----------
	// Statistics
	//----------
	union {
		INT32U  max;
		INT32S	s_max;
	};
	union {
		INT32U  min;
		INT32S	s_min;
	};
	union {
		INT32U  avr;
		INT32S	s_avr;
	};
	union {
		INT32U  sum;
		INT32S	s_sum;
	};	
	
	INT32U  cnt;
	
	//---- previous
	union {
		INT32U  prev_max;
		INT32S	prev_s_max;
	};
	union {
		INT32U  prev_min;
		INT32S	prev_s_min;
	};
	union {
		INT32U  prev_avr;
		INT32S	prev_s_avr;
	};
	union {
		INT32U  prev_sum;
		INT32S	prev_s_sum;
	};	
	
}REMS_U32; // AID가 unsigned value
//-------------------------------------




typedef struct __attribute__((packed))
{
	REMS_U32 Temp_x10;     // 0x5012, INT16S
	REMS_U32 d_DL_TSSI_dBm_x10[MAX_Optic][MAX_DL]; // 0x5100, INT16S
	REMS_U32 Fwd_Out_Pwr_x10[AMPU_4_MAX];          // 0x5103, INT16S	

	// 2019.08.12 : "DL_TSSI_Decimal" is not stat : AID is 0xB100	
	
	REMS_U32 DL_OutPwr_Cal_x10[MAX_Optic][AMPU_4_MAX]; // 0x5108, INT16S
	REMS_U32 d_UL_RSSI_dBm_x10[MAX_Optic][MAX_UL];     // 0x5200, INT16S
	REMS_U32 d_SFP_LD_Pwr_x10[CPRI4];  // 0x5305, INT16S
	REMS_U32 d_SFP_PD_Pwr_x10[CPRI4];  // 0x5308, INT16S

	// sequence : DL-CPRI0, UL-CPRI1, DL-CPRI1, UL-CPRI3
	REMS_U32 d_BIP_AccCnt[CPRI4];// 0x530A, INT32U, Err-Cnt-Instant
	
	REMS_U32 d_SFP_Vcc_x10[CPRI4];     // 0x5310, INT16S
	REMS_U32 d_SFP_LD_Bias_x10[CPRI4]; // 0x5311, INT16S	
	REMS_U32 d_SFP_Temp_x10[CPRI4];    // 0x5314, INT16S
	REMS_U32 d_Hard_Clip_Cnt_DL[MAX_DL];   // 0x5320, INT32U
	REMS_U32 d_RoundTrip_Dly[MAX_Optic];   // 0x5405, Round Trip Delay, INT32U

	// 2020.07.08 : add statistics
	REMS_U32 PSU_Temp_x10;     // 0x5024, INT16S, m_PSU.Temp_x10
	REMS_U32 PSU_Main_DC_x10;  // 0x5025, INT16S, m_PSU.DC_Out_Volt_x10[0]
	REMS_U32 PSU_Sub__DC_x10;  // 0x5026, INT16S, m_PSU.DC_Out_Volt_x10[1]
	REMS_U32 Ret_Loss_x10[AMPU_4_MAX]; // 0x511A, INT16S	
	REMS_U32 d_Oneway_Dly[MAX_Optic];  // 0x5409, INT32U	
}REMS_ST_STR;






//------------------------------------
// header 다음에 {max,min,avr,reserved} 이 반복..
typedef struct __attribute__((packed))
{	
	INT16U  A_ID[2];
	INT8U   len;
	INT8U   num; // 전송 항목수
	INT8U   flag; // num 이 1 이면, 미사용. 전송 시 flag 항목 skip.
}EMS_AID_HEADER;
/*----------------------------------------------------------*/

typedef struct __attribute__((packed))
{
	INT8U  aid_i[2]; // packet index
	INT8U  len_i;
	INT8U  idx_p[2];

	INT8U  aid_t[2]; // start time
	INT8U  len_t;
	INT8U  time[6];

	INT8U  aid_p[2]; // period
	INT8U  len_p;
	INT8U  period;
}EMS_STATIC_PKT_HEADER;

typedef struct __attribute__((packed))
{		
	INT8U   stat_enable; // 통계평균 중.
	INT16U  size_static_pkt;
}REMS_ENV;

// alarm 발생 혹은 유지 된 시점의 alarm 보고. 발생이나 정상이 되었는데, 보고 시점이 지연되어서, 
// sync 맞지않는 데이터 올라가지 않도록 한 것.
#define AMP_OFF_REASON_Flag (0x80) // bit7 




//=======================================================================================
#define MAX_AID_NUM (1000) // Excel 문서의 사용되는 총 AID 항목 MAX 값.
PROTOCOL_EXT INT16U Tot_AID_Num; // Excel 문서의 사용되는 총 AID 항목 수.


#define Meas_IDX_MAX  (512)
#define CF_C_IDX_MAX  (512)
#define STAT_IDX_MAX  (512)
#define CFnC_IDX_MAX  (512)
#define GUI__IDX_MAX  (1000)  
#define Hidden_IDX_MAX  (1000)  


PROTOCOL_EXT INT16U Meas___AID_idx[Meas_IDX_MAX];
PROTOCOL_EXT INT16U CFG__C_AID_idx[CF_C_IDX_MAX];
PROTOCOL_EXT INT16U CFG_nC_AID_idx[CFnC_IDX_MAX];
PROTOCOL_EXT INT16U GUI_ST_AID_idx[GUI__IDX_MAX];
PROTOCOL_EXT INT16U Hidden_AID_idx[Hidden_IDX_MAX];


PROTOCOL_EXT INT16U Meas___AID_num;
PROTOCOL_EXT INT16U CFG__C_AID_num;
PROTOCOL_EXT INT16U CFG_nC_AID_num;
PROTOCOL_EXT INT16U GUI_ST_AID_num;
PROTOCOL_EXT INT16U Hidden_AID_num;


#define Log_IDX_MAX  (6)
PROTOCOL_EXT INT16U LOG___AID_idx[Log_IDX_MAX+2];
PROTOCOL_EXT INT16U LOG___AID_num;




PROTOCOL_EXT void Get_Total_AID_LIST(void);
PROTOCOL_EXT void Get_AID(INT16U idx, INT8U* pAID_0, INT8U* pAID_1);


// download mode
enum
{
	_DN_RST_WAIT_ = 0,
	_DN_RST_AUTO,
};


// 1ms Timer
enum
{
	TIMER_CHK_Update_CFnC = 0,
	TIMER_MAX
};

//-----------------------------------------------------------------------------
// EMS

//----------------
// inventory
//----------------
#ifdef __TYPE__PRU__ // ANTS
/* 2020.07.28 : Inventory Status Sequence 변경
 * PRU : DTU - PSU - PAU0/1/2/3 - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd
 */
enum // Inventory 순서.
{
	INV_IDX_R_DTU = 0, // DTU (RCU)		
	INV_IDX_R_PSU,	

	INV_IDX_PAU_1,
	INV_IDX_PAU_2,
	INV_IDX_PAU_3,	
	INV_IDX_PAU_4,

	INV_IDX_SFP_CPRI_0,		
	INV_IDX_SFP_CPRI_2,
	INV_IDX_SFP_CPRI_1,
	INV_IDX_SFP_CPRI_3,
	
	MAX_INVENTORY
};
#else // ---------------- ANTS-AAU 
/* 2020.07.28 : Inventory Status Sequence 변경
 * AAU : DTU - SFP_100 - SFP_100_2nd - SFP_Alpha- SFP_Alpha_2nd - RCU
 * */
enum // Inventory 순서.
{	
	INV_IDX_R_DTU  = 0, // DTU	
	INV_IDX_SFP_CPRI_0, // 100_1st
	INV_IDX_SFP_CPRI_2, // 100_2nd
	INV_IDX_SFP_CPRI_1, // A___1st
	INV_IDX_SFP_CPRI_3, // A___2nd
	INV_IDX_RCU,        // RCU, 2020.07.28
	
	MAX_INVENTORY
};
#endif


	
typedef struct	__attribute__((packed))
{
	INT8U inv_idx;// Inventory Index
	INT8U is_read;// Inventory 읽었는지
	INT8U type;// Inventory Default Type
	INVENTORY_STR inv;
} RHU_INVENTORY_STR;

PROTOCOL_EXT RHU_INVENTORY_STR RHU_Inventory[MAX_INVENTORY];
PROTOCOL_EXT RHU_INVENTORY_STR RHU_Inventory_Prev[MAX_INVENTORY];


PROTOCOL_EXT void InitInventory( void );
PROTOCOL_EXT void InvetoryReadClear( void );
PROTOCOL_EXT void InvetoryReadClear_mod( INT8U idx );
PROTOCOL_EXT void SetInventory( INT8U* sdu, INT16U len );
PROTOCOL_EXT INT16U Get_Inventory( INT8U* sdu );
PROTOCOL_EXT void RecvModuleInventory( INT8U idx, INVENTORY_STR* inv );
PROTOCOL_EXT INT8U CheckInvetoryRead( INT8U idx );

PROTOCOL_EXT INT8U Auto_Inv_Ready( void );


PROTOCOL_EXT void Conv_uVal16_to_big_endian(INT16U uVal16, INT8U* pData );
PROTOCOL_EXT void Conv_uVal32_to_big_endian(INT32U uVal32, INT8U* pData );
PROTOCOL_EXT INT16U Make_AID_Str_Reason( INT8U aid0, INT8U aid1, INT8U* pData, INT8U alm_f );
PROTOCOL_EXT INT16U Make_AID_Struct( INT8U aid0, INT8U aid1, INT8U* pData );

PROTOCOL_EXT INT16U Make_GUI_AID( INT16U idx, void* pAID, INT8U port );
PROTOCOL_EXT INT16U Get__GUI_AID(INT16U idx);

PROTOCOL_EXT void Proc___Display_TSSI(void);
PROTOCOL_EXT void Proc__Chk_and_Disp__Out_Upper_Alarm(void);
PROTOCOL_EXT void Proc__Chk_and_Disp__TSSI_Level(void);


/*
-----------------------------------------------------------------------------
Multidownload Module type
-----------------------------------------------------------------------------
*/
typedef struct	__attribute__((packed))
{
	INT8U type;
	INT8U path4;
	INT8U reserve[2];
}MOD_TYPE;





/*
-----------------------------------------------------------------------------
	AID 0x5433
	Layer Operation Mode
-----------------------------------------------------------------------------
*/
enum
{	
	// 2019.04.16 : AID Changed AID 0x5433
	Oper_SISO = 0, // 0x00: SISO mode 
	Oper_MIMO,     // 0x01: MIMO mode (DL0/DL1,DL2,DL3)
	Oper_Max
};


// AID 0x5009, System 형상, PRU Only
enum
{
	PRU_4T4R = 0,
	PRU_2T2R,

	PRU_Shape
};

// @5G, RE Channel Bandwidth : AID 0xB41E
enum
{
	BW___0M = 0,
	BW__10M,
	BW__20M,
	BW__40M,
	BW__60M,
	BW__80M,
	BW_100M,

	BW__MAX
};


typedef struct 
{
	pid_t task_id;
	char task_name[30];
}TASK_INFO;

PROTOCOL_EXT TASK_INFO m_Task_Info[30];
PROTOCOL_EXT int Max_Task_Cnt;


enum
{
	DC__48V = 0,
	DC_Main,

	DC_Max
};




// 2019.05.01
enum // AID 0xB411
{
	DPD_Rst__All = 0x01,
	DPD_Rst_Core = 0x02, // DPD Core
	DPD_Rst_DL_0 = 0x03, // DPD DCL(DL0)
	DPD_Rst_DL_1 = 0x04, // DPD DCL(DL1)
	DPD_Rst_DL_2 = 0x05, // DPD DCL(DL2)
	DPD_Rst_DL_3 = 0x06, // DPD DCL(DL3)
};

/*
	Command 0xF4 : log 조회/응답
*/
enum
{
	LOG_ALM = 0,
	LOG_CTL,
	LOG_Max
};

/********************************************************************************/

/********************************************************************************/
PROTOCOL_EXT void Init__Alarm_Timer(ALARM_TIMER* pAlmTimer);
PROTOCOL_EXT void Init__All_Alarm_Timer(void);
PROTOCOL_EXT INT8U IS__alarm_timeout(ALARM_TIMER* pAlmTimer, INT32U timeout_ms);
PROTOCOL_EXT INT8U IS_normal_timeout(ALARM_TIMER* pAlmTimer, INT32U timeout_ms);
PROTOCOL_EXT void Rst__alarm_timeout(ALARM_TIMER* pAlmTimer);
PROTOCOL_EXT void Rst_normal_timeout(ALARM_TIMER* pAlmTimer);
PROTOCOL_EXT INT8U Chk__alarm_normal_timeout(ALARM_TIMER* pAlmTimer, INT8U cur_alm, INT8U tmp_alm ,INT32U time_alm, INT32U time_clr);
PROTOCOL_EXT INT8U Chk_clock__alarm_normal_timeout(ALARM_TIMER* pAlmTimer, INT8U last_alm, INT8U tmp_alm ,INT32U time_alm, INT32U time_clr);






/********************************************************************************/
#endif
/********************************************************************************/




