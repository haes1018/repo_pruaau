/********************************************************************************/
#ifndef __PAU_COMM_H
#define __PAU_COMM_H
/********************************************************************************/
#ifdef  PAU_COMMM_GLOBALS
#define PAU_COMM_EXT
#else
#define PAU_COMM_EXT extern
#endif
/**************************************************************/

/*----------------------------------------------------------*/
// 현재 상태와 비교해서 제어.
typedef struct __attribute__((packed))
{
	INT8U   Fwd_Att_x2;
	INT8U   Tpl_Att_x2;
	INT8U   Rvs_Att_x2;
	INT8U   Rvs_AMP_En;

	INT8U   Att_Pair;
	INT8U   Att_Pair_req;
	
	INT8U   FB_Att_x2;
	INT8U   FB_Att_x2_req;
	
	INT8U   FB_Offset_x2;
	INT8U   FB_Offset_x2_req;
	
} PAU_CTRL_REQ_STR;



#define MAX_FWD_ATT_x2 (25*2) 

/*----------------------------------------------------------*/
enum
{
	AMP_CMD_ST = 0x50,
	AMP_CMD_EN = 0x51,
	AMP_CMD_DIS = 0x52,
	
	AMP_CMD_TH_Ctrl = 0x55,  // threshold 제어
	AMP_CMD_RST_Ctrl = 0x56, // Reset 제어
	AMP_CMD_INFO = 0x57,
	AMP_CMD_INFO_Ctrl = 0x58,	

	AMP_CMD_DN_NOTI = 0x70,     // 1st/2nd Notify.
	AMP_CMD_DN_NOTI_CHK = 0x71, // Notify Confirm
	AMP_CMD_DN_DATA = 0x72,     // Download
	AMP_CMD_DN_END  = 0x73,      // Confirm
	AMP_CMD_DN_NOTI_FW = 0x74,  // 1,2차 통보, FW 적용여부 포함.
	AMP_CMD_FW_APPLY   = 0x75,  // FW 적용.

	AMP_CMD_TRx_PATH_Ctrl = 0x80,

	AMP_CMD_ATT_TEMP_TBL_READ = 0x83, // Read : TEMP, ATT
	AMP_CMD_ATT_TEMP_TBL_WR   = 0x84, // Write : TEMP, ATT
};
/*----------------------------------------------------------*/
// 상태 관련 bit 값 : 1 일때, 알람 혹은 disable.
typedef struct __attribute__((packed))
{
	INT8U   AMP_Arm[2];
	INT8S	FwdPwr[2]; // 0.1 step
	INT8S	Temp[2];   // 0.1 step
	INT8U	R_Loss[2]; // return loss, 0.1 step
	INT8U	Watt[2];   // 용량, 0.1 step
	INT8U   DC_Volt[2];// 0.01 step => 27.1 is 270
	INT8U   VSWR[2];   // 0.01 step => 1.30 is 130	
	// threshold ------------------------------------
	INT8U	TH_OverTemp[2];     // 0.1 step
	INT8U	TH_OverTemp_Rec[2]; // Over Temp (Recovery) // 0.1 step
	INT8U   TH_Over_Pwr[2];     // Over Power (Shutdown) // 0.1 step
	INT8U	TH_R_Loss[2];       // Return Loss // 0.1 step
	INT8U	TH_R_Loss_Rec[2];   // Return Loss (Recovery) // 0.1 step	
	// Offset --------------------	
	INT8S	FwdPwr_Offset[2];   // 0.1 step
	INT8U   reserve_2[2];	
	INT8S   FB_Pwr_Offset[2];    // 0.1 step, 
	INT8S   FB_Pwr_Offset_Reserve[2];
	INT8S   RX_Pwr_Offset[2][2]; // 0.1 단위, 2 byte, [IN/OUT]
	INT8S   RX_Pwr_Offset_Reserve[2][2]; 
	INT8U	TX_Dly_Offset[2];    // 0.001 step
	INT8U	TX_Dly_Offset_Reserve[2];
	INT8U	RX_Dly_Offset[2];    // 0.001 step	
	INT8U	RX_Dly_Offset_Reserve[2];
	INT8U   reserve_10_1[10];    // 0xFF	
	// Alarm --------------------	
	INT8U   Path_Arm[2];
	INT8U   Path_Arm_Reserve[2];
	//--------------------		
	INT8U   TX_Att_x2;
	INT8U   TX_TPL_Att_x2;  // not used @ 5G
	INT8U   TX_Gain_Att_x2; // 0xAC9B
	INT8U   TX_Temp_Att_x2;
	INT8U   FB_ATT_x2; 
	INT8U   FB_Offset_ATT_x2; // gain compensation
	INT8U   FB_Temp_Att_x2;
	INT8S   FB_Pwr[2];     // 0.1 step
	INT8U   TX_PLL_Freq[4]; // not used @ 5G
	INT8S   RX_Out_Pwr[2]; // 0.1 step
	INT8S   RX_Pwr[2];     // 0.1 step, In pwr
	INT8S   RX_ALC_Lvl[2]; // 0.1 step
	INT8S   RX_SD_Lvl[2];  // 0.1 step
	INT8U   RX_Att_x2;     // current att
	INT8U   RX_Gain_Att_x2;
	INT8U   RX_Temp_Att_x2;
	INT8U   RX_ALC_Att_x2; 	
	INT8U   RX_Gain_Bal_x2;
	INT8U   RX_ALC_Onoff;      // 0 : ON, 1 : Disable
	INT8U   RX_Gain_Bal_Onoff; // 0 : ON
	INT8U   FB_PLL_Freq[4]; // not used @ 5G
	INT8U   RX_PLL_Freq[4]; // not used @ 5G
	INT8U   RX_Volt[2];     // 10bit_ADC x 5 / 1023
	INT8U   CW_OnOff;       // not used @ 5G
	INT8U   CW_PLL_Freq[4]; // not used @ 5G
	INT8S   TX_IN_Pwr[2];   // 0.1 step, 2019.08.12 : AMP Forward Input Power
	INT8U   reserve_10_2[8]; // 0xFF, 2019.08.12 : 10 -> 8
	//--------------------	below is not used	
	INT8U   TX1_Att_x2;
	INT8U   TX1_TPL_Att_x2;  // not used @ 5G
	INT8U   TX1_Gain_Att_x2; // 0xAC9B
	INT8U   TX1_Temp_Att_x2;
	INT8U   FB1_ATT_x2; 
	INT8U   FB1_Offset_ATT_x2; // gain compensation
	INT8U   FB1_Temp_Att_x2;
	INT8S   FB1_Pwr[2];     // 0.1 step
	INT8U   TX1_PLL_Freq[4]; // not used @ 5G
	INT8S   RX1_Out_Pwr[2]; // 0.1 step
	INT8S   RX1_Pwr[2];     // 0.1 step, In pwr
	INT8S   RX1_ALC_Lvl[2]; // 0.1 step
	INT8S   RX1_SD_Lvl[2];  // 0.1 step
	INT8U   RX1_Att_x2;     // current att
	INT8U   RX1_Gain_Att_x2;
	INT8U   RX1_Temp_Att_x2;
	INT8U   RX1_ALC_Att_x2; 	
	INT8U   RX1_Gain_Bal_x2;
	INT8U   RX1_ALC_Onoff;      // 0 : ON, 1 : Disable
	INT8U   RX1_Gain_Bal_Onoff; // 0 : ON
	INT8U   FB1_PLL_Freq[4]; // not used @ 5G
	INT8U   RX1_PLL_Freq[4]; // not used @ 5G
	INT8U   RX1_Volt[2];     // 10bit_ADC x 5 / 1023
	INT8U   CW1_OnOff;       // not used @ 5G
	INT8U   CW1_PLL_Freq[4]; // not used @ 5G
	INT8U   TxRx1_reserve_10_2[10];   // 0xFF		
	// PAU Environment --------------------	
	INT8U   Temp_Onoff; // 온도보상 ON(0)
	INT8U   RVS_IN_Auto_SD; // 0:En, 1:Disable
	INT8U   Att_Pair_OnOff; // 0:En, 1:Disable
	INT8U   DL_St[2];
	#ifdef _gREMS_TEST_
	#else	
	INT8U   EQ_State;
	#endif	
} AMP_ST_STR;
/*----------------------------------------------------------*/
// Threshold & offset Ctrl // 0x55 Command, iro
typedef struct __attribute__((packed))
{	
	//-----------------------------------------
	INT8U   Flag[3]; // flag 3 byte @ 5G
	//-----------------------------------------
	INT8S   Over_Temp_Val[2];
	INT8S   Over_Temp_Rec[2];
	INT8S   Over_Pwr_Val[2]; // shutdown
	INT8S   Return_Loss[2];
	INT8S   Return_Loss_Rec[2];
	INT8S   Fwd_Offset_Val[2];
	INT8U   reseve_1[2];	
	INT8S   FB_Pwr_Offset[2];
	INT8S   FB1_Pwr_Offset[2];     // not used @ 5G
	INT8S   Rx_In__Pwr_Offset[2];  // input
	INT8S   Rx_Out_Pwr_Offset[2];  // output
	INT8S   Rx1_In__Pwr_Offset[2]; // input, not used @ 5G
	INT8S   Rx1_Out_Pwr_Offset[2]; // output, not used @ 5G
	INT8U   TX_Dly_Offset[2];  // 0.01
	INT8U   TX1_Dly_Offset[2]; // 0.01, not used @ 5G
	INT8U   RX_Dly_Offset[2];  // 0.01	
	INT8U   RX1_Dly_Offset[2]; // 0.01, not used @ 5G
} THRE_CTRL_STR; // 
/*----------------------------------------------------------*/
typedef struct __attribute__((packed)) // 0x80 Command
{
	//-----------------------------------------
	INT8U   Flag[7]; // flag 3 byte
	//-----------------------------------------	
	INT8U   TX_Att_x2;     // not used @ 5G
	INT8U   TX_TPL_Att_x2; // not used @ 5G
	INT8U   TX_Gain_C_Att_x2; // Compensation ATT
	INT8U   FB_ATT_x2;	
	INT8U   FB_Offset_ATT_x2; // Gain compensation
	INT8U   TX_PLL_khz[4];  // not used @ 5G
	INT8S	RX_ALC_Lvl[2]; // 2byte, 전송시 MSB 먼저.
	INT8S   RX_SD_Lvl[2]; // 2byte, 전송시 MSB 먼저.
	INT8U   RX_Att_x2; 
	INT8S   RX_Gain_C_Att_x2;
	INT8U   RX_GainBal_x2;
	INT8U   RX_ALC_Onoff; 
	INT8U   RX_GainBal_Onoff;
	INT8U   FB_PLL_Freq[4]; // not used @ 5G
	INT8U   RX_PLL_Freq[4]; // not used @ 5G
	INT8U   CW_OnOff;       // not used @ 5G
	INT8U   CW_PLL_Freq[4]; // not used @ 5G
	INT8U   RX_Path_En;     // check needed..
	INT8U	reserved_10_1[10];
	//--------------- below not used -----------------------
	INT8U   TX1_Att_x2;        // not used @ 5G
	INT8U   TX1_TPL_Att_x2;    // not used @ 5G
	INT8U   TX1_Gain_C_Att_x2; // not used @ 5G
	INT8U   FB1_ATT_x2;	       // not used @ 5G
	INT8U   FB1_Offset_ATT_x2; // not used @ 5G
	INT8U   TX1_PLL_khz[4];    // not used @ 5G
	INT8S	RX1_ALC_Lvl[2];    // not used @ 5G
	INT8S   RX1_SD_Lvl[2];     // not used @ 5G.
	INT8U   RX1_Att_x2;        // not used @ 5G
	INT8S   RX1_Gain_C_Att_x2; // not used @ 5G
	INT8U   RX1_GainBal_x2;    // not used @ 5G
	INT8U   RX1_ALC_Onoff;     // not used @ 5G
	INT8U   RX1_GainBal_Onoff; // not used @ 5G
	INT8U   FB1_PLL_Freq[4];   // not used @ 5G
	INT8U   RX1_PLL_Freq[4];   // not used @ 5G
	INT8U   CW1_OnOff;         // not used @ 5G
	INT8U   CW1_PLL_Freq[4];   // not used @ 5G
	INT8U   RX1_Path_En;       // check needed..
	INT8U	TXRX1_reserved_10_1[10];
	//-----------------------------------------
	INT8U   Temp_C_Onoff;      // 온도보상 onoff, (0:on)
	INT8U   RVS_IN_Auto_SD;    // 0:En, 1:Disable
	INT8U   Att_Pair_OnOff;    // 0:En, 1:Disable
	INT8U   EQ_State;
	/*	
	- Main(상위 4bit)/ Alpha(하위 4bit)
		0x00 : 적용 안함
		0x01 : 1.0dB(반시계 방향)
		0x02 : 1.0dB(시계 방향)
		0x03 : 0.5dB(반시계 방향)	
	*/
} AMP_TRx_CTRL_STR;

/*----------------------------------------------------------*/

/*	
-------------------------	
	table index of 5G AMP Protocol
-------------------------
*/
enum
{
	Tbl_AMP_TX0_Temp = 0x01,
	Tbl_AMP_RX0_Temp = 0x02,
	Tbl_AMP_FB0_Temp = 0x03,
	Tbl_AMP_TX0_Accr = 0x20, // Att accuray, TX0 ATT
	Tbl_AMP_RX0_Accr = 0x22, // Att accuray, RX0 ATT
};

/*	
-------------------------	
	GUI Protocol for AMP-Table
-------------------------
*/
enum
{		
	Tbl_FWD_AMP1_TEMP = 0,	// 0	DL0 온도보상 Table
	Tbl_FWD_AMP2_TEMP,		// 1	DL1 온도보상 Table
	Tbl_FWD_AMP3_TEMP,		// 2	DL2 온도보상 Table
	Tbl_FWD_AMP4_TEMP,		// 3	DL3 온도보상 Table
	
	Tbl_RVS_AMP1_TEMP = 4,	// 4	UL0 온도보상 Table
	Tbl_RVS_AMP2_TEMP,		// 5	UL1 온도보상 Table
	Tbl_RVS_AMP3_TEMP,		// 6	UL2 온도보상 Table
	Tbl_RVS_AMP4_TEMP,		// 7	UL3 온도보상 Table
	
	Tbl_ATT_AMP1_FWD_OUT_ATT= 8,// 8	DL0 ATTEN Accuracy Table
	Tbl_ATT_AMP2_FWD_OUT_ATT,	// 9	DL1 ATTEN Accuracy Table
	Tbl_ATT_AMP3_FWD_OUT_ATT,	// 10	DL2 ATTEN Accuracy Table
	Tbl_ATT_AMP4_FWD_OUT_ATT,	// 11	DL3 ATTEN Accuracy Table
	
	Tbl_ATT_AMP1_RVS_ATT = 12,	// 12	UL0 ATTEN Accuracy Table
	Tbl_ATT_AMP2_RVS_ATT,		// 13	UL1 ATTEN Accuracy Table
	Tbl_ATT_AMP3_RVS_ATT,		// 14	UL2 ATTEN Accuracy Table
	Tbl_ATT_AMP4_RVS_ATT,		// 15	UL3 ATTEN Accuracy Table
	
	Tbl_FB0_AMP1_TEMP = 16,	// 16	FB0 온도보상 Table
	Tbl_FB0_AMP2_TEMP,		// 17	FB1 온도보상 Table
	Tbl_FB0_AMP3_TEMP,		// 18	FB2 온도보상 Table
	Tbl_FB0_AMP4_TEMP,		// 19	FB3 온도보상 Table


	// AAU : 2019.04.17
	Tbl_DL0_OUT_PWR_DET = 20,	// 20	
	Tbl_DL1_OUT_PWR_DET,		// 21	
	Tbl_DL2_OUT_PWR_DET,		// 22	
	Tbl_DL3_OUT_PWR_DET,		// 23	

	Tbl_UL0_IN_PWR_DET = 24,	// 24	
	Tbl_UL1_IN_PWR_DET,			// 25	
	Tbl_UL2_IN_PWR_DET,			// 26	
	Tbl_UL3_IN_PWR_DET,			// 27

	
	// AAU : 2019.06.27
	Tbl_DL0_TMP_ALPHA_ONLY	= 32,	// 32	
	Tbl_DL1_TMP_ALPHA_ONLY,		// 33	
	Tbl_DL2_TMP_ALPHA_ONLY,		// 34	
	Tbl_DL3_TMP_ALPHA_ONLY,		// 35	

	Tbl_UL0_TMP_ALPHA_ONLY	= 36,	// 36	
	Tbl_UL1_TMP_ALPHA_ONLY,		// 37	
	Tbl_UL2_TMP_ALPHA_ONLY,		// 38	
	Tbl_UL3_TMP_ALPHA_ONLY,		// 39	
};
/*----------------------------------------------------------*/
// Fwd_Amp_SD Reason
enum
{
	// FWD, RVS AMP OFF REQ
	// 상위 nibble
	AMP_SD_PD		= 0x80, // 2019.12.19 : defined but not used
	AMP_SD_DIG		= 0x40, // CPRI Unlock : by DTU_SD_By_AMP_Not_Use, DTU_SD_BY_RU_NOT_Init,  DTU_SD_By_DIG
	AMP_SD_LOCAL	= 0x20, // 2019.12.19 : defined but not used
	AMP_SD_PAYLOAD	= 0x10, // 2019.12.19 : defined but not used
	// 하위 nibble
	AMP_FWD_OVER	= 0x08, // 2019.12.19 : defined but not used
	AMP_SUB_SW_OFF  = 0x04, // 2019.12.19 : defined but not used, AMP 서브전원 스위치 OFF
	AMP_SD_TDD_SW	= 0x02, // 2019.12.19 : defined but not used
	AMP_REV_IN		= 0x01, // RVS 과입력에 의한 셧다운.
};
/*----------------------------------------------------------*/
// AMP OFF Reason
enum
{
	rsn_User       = 0x00, // by user control
	rsn_Dev_Auto_SD= 0x01, // Device Auto Shutdown
	rsn_RVS        = 0x02, // 역방향 장애, RVS AMP OFF에 대해 reason 올리지 않으나, 내부에서만 사용위해.
	rsn_Temp_Limit = 0x03, // 온도 상한
	rsn_Over_Pwr   = 0x04, // Over Power
	rsn_Pwr_Sw_Off = 0x05, // 전원 S/W Off(DUO IBS)
	rsn_Amp_Link   = 0x06, // Amp Link Fail(SF형)
	rsn_Vswr_Fail  = 0x07, // VSWR Fail
	rsn_Loop_Fail  = 0x08, // Loop Fail
	rsn_DC_Fail    = 0x09, // DC Fail
	rsn_SUB_Pwr_Off= 0x0A, // PSU SUB 전원 off
	rsn_Wib_Sw     = 0x0B, // Switching 이상(WiBro)
	rsn_PLL        = 0x0C, // PLL Unlock
	rsn_Pwr_Alarm  = 0x0D, // 전원 이상
	rsn_Digital    = 0x0E, // Digital Board 이상
	rsn_Unknown    = 0x0F, // Unknown
	rsn_PD         = 0x10, // PD 이상
	rsn_Tran_Mode  = 0x11, // Train Mode  
	rsn_Off_Mode   = 0x12, // Off Mode
	rsn_Sleep_Mode = 0x13, // Sleep Mode
	rsn_Over_IN    = 0x14, // 과입력
	rsn_TDD_Sync   = 0x15, // TDD 동기 이상
	rsn_Input_Low  = 0x16, // 입력하한
	rsn_Sync_Fail  = 0x17, // Sync  Fail
};
/*----------------------------------------------------------*/




/*----------------------------------------------------------*/
// 외부 참조 변수
//

/*----------------------------------------------------------*/

PAU_COMM_EXT INT8S Pau_Init_End[AMPU_4_MAX]; // 초기화 했다면  1. AMP Usert reset에 의한 경우는 -1.

// AMP_CTRL
PAU_COMM_EXT AMP_TRx_CTRL_STR m_PAU_Ctrl[AMPU_4_MAX];
PAU_COMM_EXT THRE_CTRL_STR    m_THRESH_Ctrl[AMPU_4_MAX];

PAU_COMM_EXT INT8U hi_pwr_alarm[AMPU_4_MAX];

PAU_COMM_EXT PAU_CTRL_REQ_STR m_PAU_C_Req[AMPU_4_MAX];


PAU_COMM_EXT INT8U PAU_Link_Fail_Co[AMPU_4_MAX];
PAU_COMM_EXT INT8U AMP_Stby_Off_Req[AMPU_4_MAX]; // OFF Request by AMP_Stdby
PAU_COMM_EXT INT8U AMP_Stby_Ctrl_Val[AMPU_4_MAX];

PAU_COMM_EXT INT8U init[AMPU_4_MAX];

PAU_COMM_EXT INVENTORY_STR PAU_Info[AMPU_4_MAX];





PAU_COMM_EXT void PAU_Inv_Read_Req(INT8U path4 );

PAU_COMM_EXT INT16S Get_Vswr(INT16S ret_Loss_x10);





//==============================================

enum
{
	_PAU_Insert_ = 0,
	_PAU_Remove_,
	
	_PAU_INS_INFO
};

PAU_COMM_EXT INT8U Fwd_Amp_Off_rsn[AMPU_4_MAX];
PAU_COMM_EXT INT8U Fwd_Cnt_Off[AMPU_4_MAX];
PAU_COMM_EXT INT8U Fwd_Amp_SD[AMPU_4_MAX]; // AMP OFF REQ를 OR-ing. ex) AMP_SD_LOCAL

PAU_COMM_EXT INT8U Rvs_Amp_Off_rsn[AMPU_4_MAX];
PAU_COMM_EXT INT8U Rvs_Cnt_Off[AMPU_4_MAX];
PAU_COMM_EXT INT8U Rvs_Amp_SD[AMPU_4_MAX];


// 2019.04.22
// For Off-control of DTU-DL-Path and UL-Path 
// when AMP was Off by AMP_Standby or MiMo_Standby or Batt.Extension
PAU_COMM_EXT INT8U StdBy_Off_rsn[AMPU_4_MAX]; 




//---------------------------------------------------------------------

PAU_COMM_EXT INT8U PAU_HW_Ins( INT8U ser_path6 );
PAU_COMM_EXT INT8U Is_PAU_HW_INS( INT8U pau_idx );





PAU_COMM_EXT void Init_PAU_Down_St(INT8U path4);


PAU_COMM_EXT INT8U Is_Amp_Install( INT8U path4 );



PAU_COMM_EXT INT16U PAU_Table_ATT_PassData( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );


//---------------------------------------------------------------------

//PAU_COMM_EXT void AMP_Rvs_Att_Ctrl_Req(INT8U path4, INT8U att_x2 );
PAU_COMM_EXT INT8U Get_User_Rvs_Att(INT8U path4);



PAU_COMM_EXT void Free_RAM_DnImg( INT8U path4 ); ///// YCSO changed 190520
PAU_COMM_EXT int Load_RAM_DnImg( char* pullFilePath , INT8U path4);	///
PAU_COMM_EXT int Get_Multi_Dn_Tot_Frame_Crc( INT16U* pCRC, INT16U* pTotFrame, MOD_TYPE* pMod );
PAU_COMM_EXT INT16U Get_Multi_Dn_Data(INT8U path4, INT8U* data, INT16U cur_frame, INT16U tot_frame, int tot_size );

PAU_COMM_EXT INT8U PAU_EN_Ctrl(INT8U path4, INT8U en);

#ifdef __TYPE__PRU__
PAU_COMM_EXT INT8U PRU_User_AMP_EN_Ctrl(INT8U path4, INT8U en);
#endif

PAU_COMM_EXT INT8U PAU_RESET_Ctrl(INT8U path4 );
PAU_COMM_EXT void Init_PAU_N_Ctrl( INT8U path4 );
PAU_COMM_EXT INT8U Add_PAU_N_Ctrl_Data( INT8U path4, INT8U byte, INT8U bit, INT8U* data );
PAU_COMM_EXT INT8U Add_THRE_N_Ctrl_Data( INT8U path4, INT8U byte, INT8U bit, INT8U* data );
PAU_COMM_EXT INT8U Send_PAU_N_Ctrl( INT8U path4 );
PAU_COMM_EXT INT8U Send_PAU_N_Ctrl_Not_Used_AMP( INT8U path4 );

PAU_COMM_EXT INT8U PAU_Polling(INT8U path4);


PAU_COMM_EXT INT8U sendPAU_GetInfo(INT8U path4);

PAU_COMM_EXT INT8U sendPAU_AttCtrl(INT8U path, INT8U trx, INT8U idx, INT8U att_x2);
PAU_COMM_EXT INT8U sendPAU_EnDis(INT8U path, INT8U onOff );

PAU_COMM_EXT INT8U AC_Fail_PAU_EN_Ctrl(INT8U path4, INT8U en);


PAU_COMM_EXT INT16U PAU_DownLoad_PassData( INT8U type, PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );


PAU_COMM_EXT INT8U sendPAU_INFO_Ctrl(INT8U path4, INT8U* pCtrlVal, INVENTORY_STR* inv);

PAU_COMM_EXT INT8U PAU_FW_APPLY( INT8U path4, INT8U* data, INT8U len);

PAU_COMM_EXT void AMP_Init_Set(INT8U path4);

// Multidownload
PAU_COMM_EXT void Init_AMP_MutiDn(INT8U path4);

PAU_COMM_EXT void* PAU_Comm_Task(void *data);




#ifdef  __USE_PSU_PAU_DN_SIZE_500__
#define FRAME_DATA_MAX (500)
#else
#define FRAME_DATA_MAX (1000) // 2020.03.25 : '500' -> '1000'
#endif

typedef struct
{
	INT8U st[AMPU_4_MAX];
	INT8U data[AMPU_4_MAX][FRAME_DATA_MAX+20]; // 512byte
	INT8U retry[AMPU_4_MAX];
	INT8U cur_path4[AMPU_4_MAX];
	INT16U tot_frame[AMPU_4_MAX];
	INT16U cur_frame[AMPU_4_MAX];
	INT16U crc[AMPU_4_MAX];
	INT8U result[AMPU_4_MAX];
	INT8U req[AMPU_4_MAX];
	INT8U percent[AMPU_4_MAX];
	INT8U fw_apply_req[AMPU_4_MAX];	
	int   tot_size[AMPU_4_MAX];
}AMP_Auto_DN;

PAU_COMM_EXT void Update_AMP_DL_State(void);

PAU_COMM_EXT AMP_Auto_DN m_AutoDn;



PAU_COMM_EXT INT8U PAU_Send_Data(INT8U path4, INT8U type, INT8U* data, INT8U l);


PAU_COMM_EXT INT8U Is_All_PAU_CommFail(void);
PAU_COMM_EXT INT8U Is_All_PAU_CommFail_Co(void);

PAU_COMM_EXT INT8U Is_AMP_MultiDown(void);



PAU_COMM_EXT void AMP_Not_Use_RVS_OFF_Display( INT8U path4 );



PAU_COMM_EXT void Fwd_Out_Att_Set( INT8U path4, INT8U data );
PAU_COMM_EXT void Rvs_Amp_OnOff( INT8U path4, INT8U data );

PAU_COMM_EXT void Update_Fwd_Pwr_Alm( INT8U path4 );

PAU_COMM_EXT INT16S Get_Hightest_AMP_Temp(void);
PAU_COMM_EXT void TPL_Fwd_Att_Set(INT8U path4, INT8U Att_x2);


PAU_COMM_EXT void Proc___AAU_RFU(INT8U path4);
PAU_COMM_EXT void Proc___AAU_RFU_Init(INT8U path4);

PAU_COMM_EXT INT8U IS_PAU_TX_RX_All_Off(INT8U path4);
PAU_COMM_EXT INT8U IS_PAU_Algorithm_Off(INT8U path4);


PAU_COMM_EXT void AMP_Not_Use_Alarm_Mask(INT8U path4);

PAU_COMM_EXT void AMP_Down_Fail( INT8U path4 ); // 2019.09.20

PAU_COMM_EXT INT8U IS__SD_Check_Enabled(INT8U path4);


/********************************************************************************/
#endif
/********************************************************************************/






