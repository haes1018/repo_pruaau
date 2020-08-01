/********************************************************************************/
#ifndef __REG_MAP_H
#define __REG_MAP_H
/********************************************************************************/

/*
------------------------------------------------------------------------------------
	Start of 
	PRU/AAU Address
------------------------------------------------------------------------------------	
*/




//----------------------------------------
enum // Common
{	
	Reg_Common_R_SW_Ver  = 0x00, // DTU SW Version
	Reg_Common_R_HW_Ver  = 0x01, // DTU HW Version, 2019.03.19
	Reg_Common_RW_Test   = 0x02,	
	Reg_Common_R_App_MB_Counter = 0x03, // PRU(DTU App Counter), AAU(M/B Counter)
	Reg_Common_R_FPGA_Counter = 0x04, // AAU only
	Reg_Common_R_Board_INFO   = 0x05, // PRU only	
	Reg_Common_R_Board_Status = 0x07,
	Reg_Common_R_FPGA_Temp,	
	Reg_Common_RW_Reset,	
	Reg_Common_RW_ETH_Path_Sel    = 0x0A,
	Reg_Common_RW_Mapper_Loopback = 0x0B,
	
	Reg_Common_RW_RF_Loopback = 0x0C,
	Reg_Common_RW_FAN_Control = 0x0D,
	Reg_Common_RW_Recovery_Clock_Set = 0x0E,
	Reg_Common_RW_Gain_Check_Mode = 0x1E, // 2019.05.16
	
	Reg_Common_RW_Interrupt_mask0 = 0x20,
	Reg_Common_RW_Interrupt_mask1,	
	
	Reg_Common_RW_Interrupt_status0 = 0x22,
	Reg_Common_RW_Interrupt_status1,

	Reg_Common_RW_r_pattern_add_fa0 = 0x24,  // 0x24 AAU Test Pattern
	Reg_Common_RW_r_pattern_i_fa0,    // 0x25 AAU Test Pattern
	Reg_Common_RW_r_pattern_q_fa0,    // 0x26 AAU Test Pattern
	Reg_Common_RW_r_pattern_wren_fa0, // 0x27 AAU Test Pattern
	Reg_Common_RW_r_pattern_add_fa1,  // 0x28 AAU Test Pattern
	Reg_Common_RW_r_pattern_i_fa1,    // 0x29 AAU Test Pattern
	Reg_Common_RW_r_pattern_q_fa1,    // 0x2A AAU Test Pattern
	Reg_Common_RW_r_pattern_wren_fa1, // 0x2B AAU Test Pattern
	
	Reg_Common_RW_PLL_Addr = 0x2C, // 0x2C AAU PLL_Addr, 2019.07.26
	Reg_Common_RW_PLL_Data,        // 0x2D AAU PLL_Data, 2019.07.26

	Reg_Common_R__RE_Reset = 0x30, // 2019.03.20. Added
	Reg_Common_RW_RE_Reset_Clr,    // 2019.03.20. Added
	Reg_Common_RW_VSS_Table_Debug,
	Reg_Common_RW_PSU_Reset,       // PSU Reset, 2019.04.06. Added
	Reg_Common_RW_DTU_LED,         // 2019.04.08. Added, PRU Only
	Reg_Common_AC_ALARM_INFO,      // 2019.04.22. Added, PRU Only YCSO

	// 2010.04.30 : DTU Inventory
	Reg_Common_RW_DTU_INV_INFO = 0x36, // 0x36 ~ 0x3D, 0x36 is Read-Only, AAU only
	Reg_Common_RW_DTU_INV_INFO_02,
	Reg_Common_RW_DTU_INV_INFO_03,
	Reg_Common_RW_DTU_INV_INFO_04,
	Reg_Common_RW_DTU_INV_INFO_05,
	Reg_Common_RW_DTU_INV_INFO_06,
	Reg_Common_RW_DTU_INV_INFO_07,
	Reg_Common_RW_DTU_INV_INFO_08,
	
	Reg_Common_RW_RE_Reset_ACK_SW = 0x3E, // 2019.05.17
	Reg_Common_R__AFE_PLL_LD = 0x3F, // 2019.05.29
	
	Reg_Common_RW_Det_Enable = 0x42,     // 2019.09.16, Detect Enable
	Reg_Common_RW_Det_Start_Time = 0x43, // 2019.09.16, Detect Start Time
	Reg_Common_RW_Det_Range_Time = 0x44, // 2019.09.16, Detect Range Time
	//---------------------------------
	Reg_Common_MAX
};
INT32U Reg_Common__Cnt;
INT32U Reg_Common_Addr[Reg_Common_MAX];
INT8U  Reg_Common_flag[Reg_Common_MAX];
INT32U* pReg_Common_Addr[Reg_Common_MAX][ST_CTL]; 

//------------------------------------------------------------------------
enum // CPRI #0 ~ #3 (4ea)
{	
	Reg_Common_MIMO_R_SFP_LD_Power = 0x00,
	Reg_Common_MIMO_R_SFP_PD_Power,	
	Reg_Common_MIMO_R_SFP_VCC,	
	Reg_Common_MIMO_R_SFP_LD_Bias,	
	Reg_Common_MIMO_R_SFP_Temp,	
	Reg_Common_MIMO_R_SFP_WaveLen,	
	Reg_Common_MIMO_RW_SFP_TX_Disable,	//------ fail, 2019.02.17
	Reg_Common_MIMO_RW_SFP_Reset,	    // 2019.06.21
	Reg_Common_MIMO_R_SFP_BitRate,      // 2019.03.27. Added.
	Reg_Common_MIMO_R_SFP_LOS_Alarm,    // 2019.05.21. Added.
	Reg_Common_MIMO_R_SFP_LD_Alarm,     // 2019.05.21. Added.
	
	// 2010.04.06 : SFP Inventory
	Reg_Common_MIMO_R_SFP_INV_INFO = 0xB, // 0x0B ~ 0x012 
	Reg_Common_MIMO_R_SFP_INV_INFO_02,
	Reg_Common_MIMO_R_SFP_INV_INFO_03,
	Reg_Common_MIMO_R_SFP_INV_INFO_04,
	Reg_Common_MIMO_R_SFP_INV_INFO_05,
	Reg_Common_MIMO_R_SFP_INV_INFO_06,
	Reg_Common_MIMO_R_SFP_INV_INFO_07,
	Reg_Common_MIMO_R_SFP_INV_INFO_08,
	//---------------------------------
	Reg_Common_MIMO_R_SFP_VENDOR_NAME = 0x13, // 0x13 ~ 0x16
	Reg_Common_MIMO_R_SFP_VENDOR___PN = 0x17, // 0x17 ~ 0x1A
	Reg_Common_MIMO_R_SFP_VENDOR___SN = 0x1B, // 0x1B ~ 0x1E
	Reg_Common_MIMO_R_SFP_DATE___CODE = 0x1F, // 0x1F ~ 0x20
	
	Reg_Common_MIMO_END = 0x20, // END
	//---------------------------------
	Reg_Common_MIMO_MAX
};
INT32U Reg_Common_MIMO__Cnt[CPRI_PATH];
INT32U Reg_Common_MIMO_Addr[CPRI_PATH][Reg_Common_MIMO_MAX];
INT8U  Reg_Common_MIMO_flag[CPRI_PATH][Reg_Common_MIMO_MAX];
INT32U* pReg_Common_MIMO_Addr[CPRI_PATH][Reg_Common_MIMO_MAX][ST_CTL]; 

//------------------------------------------------------------------------
enum // Common Delay
{	
	Reg_DSP_Common_RW_DL_Delay = 0x00,	
	Reg_DSP_Common_RW_UL_Delay,
	Reg_DSP_Common_RW_4T2R_Mode, //--------------------------> 삭제대상. 2019.03.22.
	//---------------------------------	
	// FFT : make address,.... needed.
	Reg_DSP_Common_RW_FFT_MUX = 0x10, // test needed
	Reg_DSP_Common_RW_FFT_Trigger, // test needed
	Reg_DSP_Common_R_FFT_Done,     // test needed
	Reg_DSP_Common_RW_FFT_Addr,    // test needed
	Reg_DSP_Common_R_FFT_Result,   // test needed

	Reg_DSP_Common_RW_Capture_DL_Time = 0x20 , // PRU Only
	Reg_DSP_Common_RW_Capture_UL_Time,         //  PRU Only
	Reg_DSP_Common_RW_Capture_ULNoise_Time,    //  PRU Only
	Reg_DSP_Common_RW_UL_Noise_RSSI_StartTime = 0x30, //  PRU Only
	Reg_DSP_Common_RW_UL_Noise_RSSI_StopTime,  //  PRU Only
	Reg_DSP_Common_RW_UL_RSSI_StartTime,       //  PRU Only
	Reg_DSP_Common_RW_UL_RSSI_StopTime,        //  PRU Only
	Reg_DSP_Common_RW_SG_Flat,                 //  PRU Only
	//---------------------------------	
	Reg_DSP_Common_Max
	//---------------------------------
};
INT32U Reg_DSP_Common__Cnt;
INT32U Reg_DSP_Common_Addr[Reg_DSP_Common_Max];
INT8U  Reg_DSP_Common_flag[Reg_DSP_Common_Max];
INT32U* pReg_DSP_Common_Addr[Reg_DSP_Common_Max][ST_CTL];
//------------------------------------------------------------------------

enum // DSP_Common_MIMO #0 ~ #3
{
	Reg_DSP_Common_MIMO_RW_Threshold = 0x00,
	Reg_DSP_Common_MIMO_RW_HC_CLR,
	Reg_DSP_Common_MIMO_R_HC_COUNT,
	Reg_DSP_Common_MIMO_RW_CFR_OUT_GAIN, // 2019.03.20. AAU Only
	Reg_DSP_Common_MIMO_RW_UL_ALC_GAIN,  // 2019.04.06. PRU Only
	Reg_DSP_Common_MIMO_RW_PAU_DL_Attn_Sync, // 2019.04.06. PRU Only
	Reg_DSP_Common_MIMO_UL_IN_RSSI_DET,  // 2019.05.03. PRU Only
	Reg_DSP_Common_MIMO_RW_DL_Sum_Gain,  // 2019.07.01. PRU Only
	Reg_DSP_Common_MIMO_RW_EQ_Common,    // 2019.08.12. PRU Only	
	//---------------------------------
	Reg_DSP_Common_MIMO_MAX
};
INT32U Reg_DSP_Common_MIMO__Cnt[MAX_MIMO_PATH];
INT32U Reg_DSP_Common_MIMO_Addr[MAX_MIMO_PATH][Reg_DSP_Common_MIMO_MAX];
INT8U  Reg_DSP_Common_MIMO_flag[MAX_MIMO_PATH][Reg_DSP_Common_MIMO_MAX];
INT32U* pReg_DSP_Common_MIMO_Addr[MAX_MIMO_PATH][Reg_DSP_Common_MIMO_MAX][ST_CTL];
//------------------------------------------------------------------------
enum // FA #0 ~ #1
{
	Reg_DSP_FA_Common_RW_NCO = 0x00,     // AAU Only, 2019.05.30
	Reg_DSP_FA_Common_RW_NCO_Valid = 0x01,	

	Reg_DSP_FA_Common_RW_DL_NCO = 0x02, // PRU Only, 2019.05.30
	Reg_DSP_FA_Common_RW_UL_NCO = 0x03, // PRU Only, 2019.05.30
	//---------------------------------
	Reg_DSP_FA_Common_MAX
};
INT32U Reg_DSP_FA_Common__Cnt[MAX_FA_PATH];
INT32U Reg_DSP_FA_Common_Addr[MAX_FA_PATH][Reg_DSP_FA_Common_MAX];
INT8U  Reg_DSP_FA_Common_flag[MAX_FA_PATH][Reg_DSP_FA_Common_MAX];
INT32U* pReg_DSP_FA_Common_Addr[MAX_FA_PATH][Reg_DSP_FA_Common_MAX][ST_CTL];

//------------------------------------------------------------------------

enum // FA #0 ~ #1 (2ea) / MIMO #0 ~ #3 (4ea) => total (2ea) x (4ea) = (8ea)
{
	Reg_DSP_FA_MIMO_RW_DL_OnOff = 0x00, 
	Reg_DSP_FA_MIMO_RW_DL_Gain,
	Reg_DSP_FA_MIMO_RW_DL_CW_Level,
	Reg_DSP_FA_MIMO_RW_DL_CW_On,
	Reg_DSP_FA_MIMO_R_DL_TSSI = 0x04,
	Reg_DSP_FA_MIMO_RW_DL_Inpahase_Cont,  // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_RW_DL_Quad_Cont,      // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_RW_DL_BW_Sel = 0x07,
	Reg_DSP_FA_MIMO_RW_DL_EQ_Coeff,
	Reg_DSP_FA_MIMO_R_DL_RSSI_Peak,
	Reg_DSP_FA_MIMO_RW_DL_EQ_1_Coeff,      // AAU Only 
	//---------------------------------
	Reg_DSP_FA_MIMO_RW_UL_OnOff = 0x20,	
	Reg_DSP_FA_MIMO_RW_UL_Gain,
	Reg_DSP_FA_MIMO_R_UL_RSSI,
	Reg_DSP_FA_MIMO_RW_UL_Inpahase_Cont, // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_RW_UL_Quad_Cont,     // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_R_UL_Inpahase_Det,   // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_R_UL_Quad_Det,	     // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_RW_UL_BW_Sel = 0x27,
	Reg_DSP_FA_MIMO_RW_UL_EQ_Coeff,
	Reg_DSP_FA_MIMO_R_UL_RSSI_Peak,
	//---------------------------------
	Reg_DSP_FA_MIMO_R_FB_RSSI_RMS = 0x30,    // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_R_FB_RSSI_Peak = 0x31,   // AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_R_GC_RSSI_RMS = 0x32,    // AAU Only, 2019.06.26.	
	
	Reg_DSP_FA_MIMO_RW_UL_OUTPUT_ATT = 0x33, // PRU(UL Out ATT), AAU(Reg_DSP_FA_MIMO_RW_UL_EQ_1_Coeff) 
	Reg_DSP_FA_MIMO_RW_DL_DELAY = 0x34, 	 // 2019.04.06.
	Reg_DSP_FA_MIMO_RW_UL_DELAY = 0x35, 	 // 2019.04.06. 
	Reg_DSP_FA_MIMO_R__TSSI_SD_St = 0x36, 	 // 2019.04.26. 
	Reg_DSP_FA_MIMO_RW_TEST_Pattern = 0x37,  // 2019.05.03.
	Reg_DSP_FA_MIMO_R__UL_NOISE_RMS = 0x38,  // 2019.05.14.
	
	Reg_DSP_FA_MIMO_RW_DL_LSH_Gain = 0x39,      // 2019.07.01
	Reg_DSP_FA_MIMO_RW_UL_LSH_Gain,             // 2019.07.01
	Reg_DSP_FA_MIMO_RW_Test_Pattern_Band_Gain,  // 2019.07.01
	Reg_DSP_FA_MIMO_RW_TSSI_SD_Level_short, // 0x3C  // 2019.07.01
	Reg_DSP_FA_MIMO_RW_TSSI_SD_Level__long, // 0x3D  // 2019.07.01
	Reg_DSP_FA_MIMO_RW_TSSI_SD_Level___Low, // 0x3E  // 2019.07.01
	Reg_DSP_FA_MIMO_RW_Height_Bal_Gain,     // 0x3F  // 2019.07.01
	Reg_DSP_FA_MIMO_RW_PAU_EQ_Status,       // 0x40  // 2019.08.12
	
	//---------------------------------
	Reg_DSP_FA_MIMO_R_RE_Reset = 0x41, 			// AAU Only //--------------------------> 삭제대상. 2019.03.22.
	Reg_DSP_FA_MIMO_W_RE_Reset_Clear = 0x42,	// AAU Only //--------------------------> 삭제대상. 2019.03.22.
	//---------------------------------

	Reg_DSP_FA_MIMO_MAX
};

#define Reg_DSP_FA_MIMO_RW_UL_EQ_1_Coeff (Reg_DSP_FA_MIMO_RW_UL_OUTPUT_ATT) // 2019.06.01

INT32U Reg_DSP_FA_MIMO__Cnt[MAX_FA_PATH][MAX_MIMO_PATH];
INT32U Reg_DSP_FA_MIMO_Addr[MAX_FA_PATH][MAX_MIMO_PATH][Reg_DSP_FA_MIMO_MAX];
INT8U  Reg_DSP_FA_MIMO_flag[MAX_FA_PATH][MAX_MIMO_PATH][Reg_DSP_FA_MIMO_MAX];
INT32U* pReg_DSP_FA_MIMO_Addr[MAX_FA_PATH][MAX_MIMO_PATH][Reg_DSP_FA_MIMO_MAX][ST_CTL];


/*
------------------------------------------------------------------------------------
100M :	[Upper-Dev] ============ CPRI-#0-[ ------- ]-CPRI-#2 ============ [Lower-RU] 	
                                         [ Self-RU ]
etc  :	[Upper-Dev] ============ CPRI-#1-[ ------- ]-CPRI-#3 ============ [Lower-RU] 		
------------------------------------------------------------------------------------
*/



/*
------------------------------------------------------------------------------------
DL CPRI #0(#1) : LayerSplitter --> Self-RU : All Register : Read Only
DL CPRI #2(#3) : Self-RU  --> Lower Remote : All Register : R/W
------------------------------------------------------------------------------------
*/
enum 
{
	Reg_VSS_DL_RECSERDESLOS = 0x00, 
	Reg_VSS_DL_RECLOFALARM,
	Reg_VSS_DL_RECTypeID,
	Reg_VSS_DL_CnM_Path_selection,
	Reg_VSS_DL_RE_Reset,
	Reg_VSS_DL_RE_Channel_BW,
	Reg_VSS_DL_Noti_IP2_4th_Byte, 
	Reg_VSS_DL_Noti_IP1_4th_Byte, 
	Reg_VSS_DL_RE_Base_Mac_3rd_Byte, 
	Reg_VSS_DL_RE_Base_Mac_6th_Byte, 
	Reg_VSS_DL_Noti_IP2_3rd_Byte =	0x0A, 
	Reg_VSS_DL_Noti_IP1_3rd_Byte, 
	Reg_VSS_DL_RE_Base_Mac_2nd_Byte, 
	Reg_VSS_DL_RE_Base_Mac_5th_Byte, 
	Reg_VSS_DL_Noti_IP2_2nd_Byte, 
	Reg_VSS_DL_Noti_IP1_2nd_Byte, 
	Reg_VSS_DL_RE_Base_Mac_1st_Byte = 0x10, 
	Reg_VSS_DL_RE_Base_Mac_4th_Byte, 
	Reg_VSS_DL_Noti_IP2_1st_Byte, 
	Reg_VSS_DL_Noti_IP1_1st_Byte, 
	//-----------------------------
	Reg_VSS_DL_OpenCPRIVersion = 0x15,
	Reg_VSS_DL_CPRICompressionOnOff,
	Reg_VSS_DL_RE_Cascade_ID,
	Reg_VSS_DL_OpticLOSDetectAlarm,
	Reg_VSS_DL_Loopbackmode,
	Reg_VSS_DL_Delay_H,
	Reg_VSS_DL_Delay_L,	
	Reg_VSS_DL_RE1Reset,
	Reg_VSS_DL_RE2Reset,
	Reg_VSS_DL_RE3Reset,
	Reg_VSS_DL_RE4Reset,
	Reg_VSS_DL_RE1TxDelay_H = 0x20,
	Reg_VSS_DL_RE1TxDelay_L,
	Reg_VSS_DL_RE1RxDelay_H,
	Reg_VSS_DL_RE1RxDelay_L,
	Reg_VSS_DL_TDD_Slot_Number00 = 0x24,
	Reg_VSS_DL_TDD_Slot_Number01,
	Reg_VSS_DL_TDD_Slot_Number02,
	Reg_VSS_DL_TDD_Slot_Number03,
	Reg_VSS_DL_TDD_Slot_Number04,
	Reg_VSS_DL_TDD_Slot_Number05,
	Reg_VSS_DL_TDD_Slot_Number06,
	Reg_VSS_DL_TDD_Slot_Number07,
	Reg_VSS_DL_TDD_Slot_Number08,
	Reg_VSS_DL_TDD_Slot_Number09,
	Reg_VSS_DL_TDD_Slot_Number10,
	Reg_VSS_DL_TDD_Slot_Number11,
	Reg_VSS_DL_TDD_Slot_Number12,	
	Reg_VSS_DL_TDD_Slot_Number13,
	Reg_VSS_DL_TDD_Slot_Number14,
	Reg_VSS_DL_TDD_Slot_Number15,
	Reg_VSS_DL_TDD_Slot_Number16,
	Reg_VSS_DL_TDD_Slot_Number17,
	Reg_VSS_DL_TDD_Slot_Number18,
	Reg_VSS_DL_TDD_Slot_Number19 = 0x37,
	//---------------------------------
	Reg_VSS_DL_UL_GainBackOff_Onoff = 0x38, // 2019.05.13.
	Reg_VSS_DL_DUH_ID = 0x3F,
	//---------------------------------
	Reg_VSS_DL_PCI_1st_Byte = 0x40,
	Reg_VSS_DL_PCI_2nd_Byte = 0x41,
	Reg_VSS_DL_REC_Port_No  = 0x42,
	//-----------------------------
	Reg_VSS_DL_Cell_ID = 0x43,    // 2019.03.26, Changed
	//---------------------------------
	Reg_VSS_DL_Base_IP_Address1 = 0x44, 
	Reg_VSS_DL_Base_IP_Address2,
	Reg_VSS_DL_Base_IP_Address3,
	Reg_VSS_DL_Base_IP_Address4,
	//-----------------------------	
	Reg_VSS_DL_gNB_ID_1st_Byte = 0x48, // 2019.03.26, Changed
	Reg_VSS_DL_gNB_ID_2nd_Byte,        // 2019.03.26, Changed
	Reg_VSS_DL_gNB_ID_3rd_Byte,        // 2019.03.26, Changed
	Reg_VSS_DL_gNB_ID_4th_Byte,        // 2019.03.26, Changed

	//---------------------------------
	// BIP-DL is CPRI#0, CPRI#1 only 
	//---------------------------------
	Reg_VSS_DL_CPRI_Version_Z_2_0  = 0x4C,     // CPRI #0/#1 only, 2019.03.26, Changed
	Reg_VSS_DL_CPRI_Startup_Z_66_0 = 0x4D,     // CPRI #0/#1 only, 2019.03.26, Changed 
	Reg_VSS_DL_CPRI_L1_RST_LOS_Z_130_0 = 0x4E, // CPRI #0/#1 only, 2019.03.26, Changed 
	Reg_VSS_DL_Pointer_P_Z_194_0       = 0x4F, // CPRI #0/#1 only, 2019.03.26, Changed 
	//-----------------------------	

	//-----------------------------	
	Reg_VSS_DL_L1Reset = 0x60,

	//---------------------------------
	// BIP-DL is CPRI#0, CPRI#1 only 
	//---------------------------------
	Reg_VSS_DL_BIP_Count = 0x61,   
	Reg_VSS_DL_BIP_Count_1, // addr : 0x62, bit  63~32 // CPRI #0/#1 only 
	Reg_VSS_DL_BIP_Count_2, // addr : 0x63, bit  95~64 // CPRI #0/#1 only 
	Reg_VSS_DL_BIP_Count_3, // addr : 0x64, bit 127~96 // CPRI #0/#1 only 
	Reg_VSS_DL_BIP_Clear,   // R, R/W 반대동작(#0/#1에서 RW, #2/3에서 Read Only) // CPRI #0/#1 only 
	Reg_VSS_DL_BIP_Err_Cnt, // addr : 0x66             // CPRI #0/#1 only 
	//-----------------------------

	//---------------------------------
	Reg_VSS_DL_MAX
};
INT32U Reg_VSS_DL__Cnt[CPRI_PATH];
INT32U Reg_VSS_DL_Addr[CPRI_PATH][Reg_VSS_DL_MAX];
INT8U  Reg_VSS_DL_flag[CPRI_PATH][Reg_VSS_DL_MAX];
INT32U* pReg_VSS_DL_Addr[CPRI_PATH][Reg_VSS_DL_MAX][ST_CTL];


/*
------------------------------------------------------------------------------------
UL CPRI #2(#3) : Self-RU <-- Lower Remote  : All Register : Read Only
UL CPRI #0(#1) : LayerSplitter <-- Self-RU : All Register : R/W
------------------------------------------------------------------------------------
*/
enum // CPRI UL #0,#2 (100M), #1,#3 (etc)
{
	Reg_VSS_UL_RECSERDESLOS = 0x00, 
	Reg_VSS_UL_RECLOFALARM,
	Reg_VSS_UL_REFunctionFail,
	//--------------------------
	Reg_VSS_UL_RE_Reset_Ack = 0x04,	
	Reg_VSS_UL_ALIVE_ACK_00,
	//--------------------------
	Reg_VSS_UL_ALIVE_ACK_01 = 0x15,  //--------------------------> 삭제대상. 2019.03.22.
	Reg_VSS_UL_Mismatch_Noti = 0x16, // CPRI #0/#1 Only !!!!, 2019.03.26, Changed.
	//--------------------------
	Reg_VSS_UL_RE_Cascade_ID_ACK = 0x17,	
	Reg_VSS_UL_RE_OpticLOSDetectAlarm,
	Reg_VSS_UL_RE_Loopbackmode_ACK,
	//--------------------------
	Reg_VSS_UL_RE1Reset = 0x1C,	
	Reg_VSS_UL_RE2Reset,
	Reg_VSS_UL_RE3Reset,
	Reg_VSS_UL_RE4Reset,
	//--------------------------
	Reg_VSS_UL_RE1FunctionFail = 0x20,
	Reg_VSS_UL_RE2FunctionFail,
	Reg_VSS_UL_RE3FunctionFail,
	Reg_VSS_UL_RE4FunctionFail,
	//--------------------------
	Reg_VSS_UL_RE_Type_ID = 0x38,
	//--------------------------
	Reg_VSS_UL_RE_Vendor_ID = 0x3C,
	//--------------------------
	Reg_VSS_UL_CPRI_Version_Z_2_0        = 0x48, // CPRI #2/#3 Only 
	Reg_VSS_UL_CPRI_Startup_Z_66_0       = 0x49, // CPRI #2/#3 Only 
	Reg_VSS_UL_CPRI_L1_Reset_LOS_Z_130_0 = 0x4A, // CPRI #2/#3 Only 
	Reg_VSS_UL_Pointer_P_Z_194_0         = 0x4B, // CPRI #2/#3 Only 
	//--------------------------
	Reg_VSS_UL_L1_Reset_ACK = 0x60,	
	//---------------------------------
	// BIP-UL is CPRI #2/#3 Only 
	//---------------------------------
	Reg_VSS_UL_BIP_Count = 0x61,// CPRI #2/#3 Only 
	Reg_VSS_UL_BIP_Count_1, 	// CPRI #2/#3 Only, bit  63~32 
	Reg_VSS_UL_BIP_Count_2, 	// CPRI #2/#3 Only, bit  95~64
	Reg_VSS_UL_BIP_Count_3, 	// CPRI #2/#3 Only, bit 127~96 
	Reg_VSS_UL_BIP_Clear,   	// CPRI #2/#3 Only 
	Reg_VSS_UL_BIP_Err_Cnt, 	// CPRI #2/#3 Only 
	//---------------------------------
	
	Reg_VSS_UL_MAX
};
INT32U Reg_VSS_UL__Cnt[CPRI_PATH];
INT32U Reg_VSS_UL_Addr[CPRI_PATH][Reg_VSS_UL_MAX];
INT8U  Reg_VSS_UL_flag[CPRI_PATH][Reg_VSS_UL_MAX];
INT32U* pReg_VSS_UL_Addr[CPRI_PATH][Reg_VSS_UL_MAX][ST_CTL];
//------------------------------------------------------------------------


//------------------------------------------------------------------------
enum // Common
{	
	Reg_TDD_RW_TDD_Mode = 0x00,
	Reg_TDD_RW_Slot00_Format = 0x01, // 00_(0x01) ~ 19_(0x14)
	Reg_TDD_RW_Slot01_Format,				
	Reg_TDD_RW_Slot02_Format,				
	Reg_TDD_RW_Slot03_Format,				
	Reg_TDD_RW_Slot04_Format,				
	Reg_TDD_RW_Slot05_Format,				
	Reg_TDD_RW_Slot06_Format,				
	Reg_TDD_RW_Slot07_Format,				
	Reg_TDD_RW_Slot08_Format,				
	Reg_TDD_RW_Slot09_Format,				
	Reg_TDD_RW_Slot10_Format,				
	Reg_TDD_RW_Slot11_Format,				
	Reg_TDD_RW_Slot12_Format,				
	Reg_TDD_RW_Slot13_Format,				
	Reg_TDD_RW_Slot14_Format,				
	Reg_TDD_RW_Slot15_Format,				
	Reg_TDD_RW_Slot16_Format,				
	Reg_TDD_RW_Slot17_Format,				
	Reg_TDD_RW_Slot18_Format,				
	Reg_TDD_RW_Slot19_Format,		
	//-----------------------------
	Reg_TDD_RW_DL_ON_Guard_Time = 0x15,
	Reg_TDD_RW_UL_ON_Guard_Time,
	//-----------------------------
	Reg_TDD_RW_DL_to_UL_Guard_Time = 0x17,
	Reg_TDD_RW_UL_to_DL_Guard_Time,
	//-----------------------------
	Reg_TDD_RW_UL_GAIN_Guard_Time0 = 0x19,
	Reg_TDD_RW_UL_GAIN_Guard_Time1,
	Reg_TDD_RW_UL_GAIN_Guard_Time2,
	Reg_TDD_RW_UL_GAIN_Guard_Time3,
	Reg_TDD_RW_UL_GAIN_Guard_Time4,
	Reg_TDD_RW_UL_GAIN_Measure_Time,
	//-----------------------------
	Reg_TDD_RW_TSYNC_Delay = 0x1F,	
	Reg_TDD_R_TSYNC_Fail,
	//-----------------------------	
	Reg_TDD_MAX
};
INT32U Reg_TDD__Cnt;
INT32U Reg_TDD_Addr[Reg_TDD_MAX];
INT8U  Reg_TDD_flag[Reg_TDD_MAX];
INT32U* pReg_TDD_Addr[Reg_TDD_MAX][ST_CTL]; 
//------------------------------------------------------------------------
enum // Common
{
	Reg_TDD2_R_tOffset = 0x21,
	Reg_TDD2_R_TBDelayDL,
	Reg_TDD2_R_TBDelayUL,
	Reg_TDD2_R_Round_Trip_Dly,
	Reg_TDD2_R_T2A_Dly,
	Reg_TDD2_R_TA3_Dly,
	//-----------------------------	
	Reg_TDD2_MAX
};
INT32U Reg_TDD2__Cnt[MAX_Optic];
INT32U Reg_TDD2_Addr[MAX_Optic][Reg_TDD2_MAX];
INT8U  Reg_TDD2_flag[MAX_Optic][Reg_TDD2_MAX];
INT32U* pReg_TDD2_Addr[MAX_Optic][Reg_TDD2_MAX][ST_CTL]; 
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// PRU Only
//------------------------------------------------------------------------
enum // DL #0 ~ #3,
{
	Reg_CFR_Version = 0x00, // path number 0(common) only use.
	Reg_CFR_OnOff, 
	Reg_CFR_Level,	

	Reg_CFR_MAX
};
INT32U Reg_CFR__Cnt[MAX_DL];
INT32U Reg_CFR_Addr[MAX_DL][Reg_CFR_MAX];
INT8U  Reg_CFR_flag[MAX_DL][Reg_CFR_MAX];
INT32U* pReg_CFR_Addr[MAX_DL][Reg_CFR_MAX][ST_CTL];

//------------------------------------------------------------------------
// PRU Only
//------------------------------------------------------------------------
enum // DL #0 ~ #3,
{
	Reg_DPD_Version = 0x00, // path number 0(common) only use.
	Reg_DPD_OnOff,
	Reg_DPD_St,
	Reg_DPD_Err_St,
	Reg_DPD_Success_Cnt, // success count
	Reg_DPD_Input_Pwr,
	Reg_DPD_Eng_In_Pwr, // 0x06
	Reg_DPD_Output_Pwr,
	Reg_DPD_Eng_Out_Pwr,
	Reg_DPD_FB_In_Pwr = 0x09,
	Reg_DPD_FB_Eng_In_Pwr, // 0x0A
	Reg_DPD_RESET = 0x0B, // DPD_Reset, 2019.04.13

	Reg_DPD_PeakExpansion, // 2019.05.20
	Reg_DPD_MSE,     // 2019.05.20
	Reg_DPD_PRE_MSE, // 2019.05.20
	Reg_DPD_LUT_Value, // 2019.05.20
	Reg_DPD_Counts,    // 2019.05.20
		
	Reg_DPD_MAX
};
INT32U Reg_DPD__Cnt[MAX_DL];
INT32U Reg_DPD_Addr[MAX_DL][Reg_DPD_MAX];
INT8U  Reg_DPD_flag[MAX_DL][Reg_DPD_MAX];
INT32U* pReg_DPD_Addr[MAX_DL][Reg_DPD_MAX][ST_CTL];



enum // 
{
	Reg_DPD_Core_RESET = 0x04, // DPD_Core Reset, 2019.05.01
	Reg_DPD_All_RESET = 0x05, // DPD_All Reset, 2019.05.01
		
	Reg_DPD2_MAX
};
INT32U Reg_DPD2__Cnt;
INT32U Reg_DPD2_Addr[Reg_DPD2_MAX];
INT8U  Reg_DPD2_flag[Reg_DPD2_MAX];
INT32U* pReg_DPD2_Addr[Reg_DPD2_MAX][ST_CTL];


//--------------------------------------------------------------------------------
// AAU Only
//--------------------------------------------------------------------------------
enum // MIMO #0 ~ #3,
{
	Reg_RFU_CTRL_RW_DL_ATT = 0x00,
	Reg_RFU_CTRL_RW_UL_ATT,	
	Reg_RFU_CTRL_RW_PA_OnOff,	
	Reg_RFU_CTRL_RW_Layer_Info, // 2019.05.03, PRU only	

	Reg_RFU_CTRL_MAX
};
INT32U Reg_RFU_CTRL__Cnt[MAX_MIMO_PATH];
INT32U Reg_RFU_CTRL_Addr[MAX_MIMO_PATH][Reg_RFU_CTRL_MAX];
INT8U  Reg_RFU_CTRL_flag[MAX_MIMO_PATH][Reg_RFU_CTRL_MAX];
INT32U* pReg_RFU_CTRL_Addr[MAX_MIMO_PATH][Reg_RFU_CTRL_MAX][ST_CTL];

//--------------------------------------------------------------------------------
// AAU Only
//--------------------------------------------------------------------------------
enum // MIMO #0 ~ #3
{
	Reg_ALC_RW_ALC_Threadhold = 0x00,				
	Reg_ALC_RW_Default_ATT, // UL AFE ATT, 2019.08.13
	Reg_ALC_R_ALC_ATT,				
	Reg_ALC_RW_Mode,
	
	Reg_ALC_MAX,
};
INT32U Reg_ALC__Cnt[MAX_MIMO_PATH];
INT32U Reg_ALC_Addr[MAX_MIMO_PATH][Reg_ALC_MAX];
INT8U  Reg_ALC_flag[MAX_MIMO_PATH][Reg_ALC_MAX];
INT32U* pReg_ALC_Addr[MAX_MIMO_PATH][Reg_ALC_MAX][ST_CTL];

//--------------------------------------------------------------------------------
// AAU Only
//--------------------------------------------------------------------------------
enum // MIMO #0 ~ #3
{
	Reg_AXI_CPRI_R_Status = 0x00,
	Reg_AXI_CPRI_R_MiscAlm = 4,	// Miscellaneous alarm
	Reg_AXI_CPRI_MAX
};
INT32U Reg_AXI_CPRI_Cnt[CPRI_PATH]; 
INT32U Reg_AXI_CPRI_Addr[CPRI_PATH][Reg_AXI_CPRI_MAX];
INT8U  Reg_AXI_CPRI_flag[CPRI_PATH][Reg_AXI_CPRI_MAX];
INT32U* pReg_AXI_CPRI_Addr[CPRI_PATH][Reg_AXI_CPRI_MAX][ST_CTL];


//--------------------------------------------------------------------------------
// AAU , PRU
//--------------------------------------------------------------------------------
INT32U Reg_COMP_ONOFF_Cnt[CPRI_PATH];
INT32U Reg_COMP_ONOFF_Addr[CPRI_PATH];
INT8U  Reg_COMP_ONOFF_flag[CPRI_PATH];
INT32U* pReg_COMP_ONOFF_Addr[CPRI_PATH][ST_CTL];


/*
------------------------------------------------------------------------------------
	End of 
	PRU/AAU Address
------------------------------------------------------------------------------------	
*/


typedef struct __DtuStatus
{
	type32 R_SW_Ver;							// 0x00, Version 예) 1.0.0.5 -> 0x01000005
	type32 R_HW_Ver;					        // 0x01, 예) 19.01.01 -> 0x00130101, 2019.07.08 : R_HW_Ver -> R_Date, name change
	type32 RW_Test;								// 0x02, Default value = 0x0000 0000
	type32 R_App_MB_Cnter;						// 0x03, [31:0] Counter, M/B increase Counter per 1Sec
	type32 R_FPGA_Counter;						// 0x04, [31:0] Counter, M/B increase Counter per 1Sec
	type32 R_Board_INFO;						// 0x05, [0] : JIG Board = '1', PRU Board = '0', PRU only.
	//----------------------------------------	
	// [31:20] : Reserved 
	// [19:16] : Optic Injection, Insert = '0'  // 2019.04.03. changed..
    // [19:16] : CPRI #4 ~ #1 Optic
 	// [15:3] : Reserved
 	// [2] : JESD Lock, Lock ='1', Unlock ='0'
 	// [1] : PLL/AFE Init Done = '1', NotDone = '0'
 	// [0] : PLL Lock, Lock = '1', Fail = '0'
 	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Pll_Lock			:1; // 0	
			INT8U Pll_Init_Done		:1; // 1	
			INT8U JESD_Lock			:1; // 2	
			INT8U 				    :1; 	
			INT16U				    :12; // 4~15			
			INT8U Optic1_Injection	:1;  // 16 
			INT8U Optic2_Injection	:1;  // 17 
			INT8U Optic3_Injection	:1;  // 18 
			INT8U Optic4_Injection	:1;  // 19 
			INT16U				    :12; // 20:31 
		}__attribute__((packed));
	}R_Board_Status; // Addr : 0x07

	//----------------------------------------	
	// [31:16] Reserved
	// [15:0] Termperature  예) -517 => -51.7 ℃
	type32 R_FPGA_Temp; // addr : 0x08
	
	//----------------------------------------	
	// Defaul value = 0xFFFF FFFF
	// bit : Normal = '1', Reset = '0'
	//----------------------------
	// [31] : DSP Reset 
	// [30] : M/B & PLL & AFE Reset
	// [29:24] : --
	// [23:20] : CPRI Reset of #4 ~ #1
	// [19:16] : --
	// [15:12] : AXI I/F Reset of #4 ~ #1
	// [11:2]  : --
	// [1]  : Eth Block Reset
	// [0]  : Eth Switch Reset
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Eth_Switch			:1; // 0
			INT8U Eth_Block				:1; // 1
			INT8U 						:2; // 2~3
			INT8U						:4; // 4
			INT8U						:4; // 8
			INT8U AXI_IF1				:1; // 9
			INT8U AXI_IF2				:1; // 10
			INT8U AXI_IF3				:1; // 11
			INT8U AXI_IF4				:1; // 12
			INT8U 						:4; // 16~19(#1~#4)
			INT8U CPRI1					:1; // 20(#1) 
			INT8U CPRI2					:1; // 21(#2)  
			INT8U CPRI3					:1; // 22(#3)  
			INT8U CPRI4					:1; // 23(#4)  
			INT8U						:4; // 24
			INT8U						:1; // 28
			INT8U AFE					:1; // 29, 2019.07.01, AAU
			INT8U MB					:1; // 30
			INT8U DSP					:1; // 31
		}__attribute__((packed));
	}RW_Reset; // addr : 0x09, Reset 

	//----------------------------------------
	// Defaul value = 0x0000 0000
	//----------------------------------------	
	// [16] CPRI #1 = '0', CPRI #2 ='1'	
	// [0] CPRI #3 = '0', CPRI #4 ='1'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U CPRI3_4				:1; // 0	
			INT8U 						:7; 				
			INT8U 						:8; 	
			INT8U CPRI1_2				:1; // 16
			INT8U						:7; 
			INT8U 						:8; 
		}__attribute__((packed));
	}RW_ETH_Path_Sel; // addr : 0x0A,
	
	//----------------------------------------	
	// Defaul value = 0x0000 0000
	//----------------------------------------	
	// [31:1] : --
	// [4] : CPRI MGT Loopback    (On = '1', Off = '0')
	// [0] : CPRI Mapper Loopback (On = '1', Off = '0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U OnOff				:1; // 0
			INT8U 					:3; // 1~3
			INT8U MGT_OnOff			:1; // 4
		}__attribute__((packed));
	}RW_Mapper_Loopback; // addr : 0x0B

	//----------------------------------------	
	// Default value = 0x0000 0000
	// LoopbackOn = '1', LoopbackOff ='0'
	//----------------------------------------	
	// [1] : JESD Loopback
	// [0] : DSP Loopback
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U DSP_OnOff				:1; // 0	
			INT8U JESD_OnOff 			:1; // 1			
		}__attribute__((packed));
	}RW_RF_Loopback; // addr : 0x0C
		
	//----------------------------------------	
	// Default value = 0x0000 0000
	// ON ='1', OFF='0'
	//----------------------------------------
	// [31:2] Reserved
	//[1] FAN ALARM, ALARM='1', NORMAL='0'
	//[0] : FAN, ON='1', OFF='0'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U FAN_OnOff				:1; // 0, FAN Mode_2
			INT8U FAN_Mode_1			:1; // 1, FAN Mode_1
			INT8U FAN_Alm 				:1; // 1
		}__attribute__((packed));
	}RW_FAN_Control; // addr : 0x0D, AAU Only

	//----------------------------------------	
	// Default value = 0x0000 0000	
	//----------------------------------------
	// [31:1] : --
	// [1:0] :  Recovery clock select
    //        Main='0', Alpha='1, External ='2'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Set					:2; // 0
		}__attribute__((packed));
	}RW_Recovery_Clock_Set;
	
	//----------------------------------------
	// [31:1] : --
	// [3] : NoiseDetector mode, enable = '1', disable = '0'
	// [2] : DL Detector 1 mode, enable = '1', disable = '0'
	// [1] : DL Detector 0 mode, enable = '1', disable = '0'
	// [0] : UL Gain check mode, enable = '1', disable = '0'	
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U onoff					:1; // 0
			INT8U dl_mode_0				:1; // 1
			INT8U dl_mode_1				:1; // 2
			INT8U noise_det_mode		:1; // 3
		}__attribute__((packed));
	}Gain_Check_Mode; // TDD mode enable, 2019.05.22, AAU Only
	
	//----------------------------------------	
	// Default value = 0x0000 0000	
	//----------------------------------------
	// [31:1] : --
	// [0] : PLL Lock (Mask='1', Unmask='0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Mask					:1; // 0
		}__attribute__((packed));
	}RW_Interrupt_mask0; // 0x20 : Interrupt Mask0 - PLL
	
	//----------------------------------------	
	// Default value = 0x0000 0000
	//----------------------------------------	
	// [31:30] : CPRI #1~#2 L1 Reset, Normal = '0', Reset = '1'
	// [30:20] : --
	// [19:16] : Optic Injection (None = '0', Injection = '1')
	// [19:16] : CPRI #4 ~ #1 Optic
	// [15:12] : --
	// [11: 8] : Rx LOS, (Mask = '1', Unmask = '0')
	// [11: 8] : CPRI #4 ~ #1
	// [ 7: 4] : --
	// [ 3: 0] : Tx Fault (Mask = '1', Unmask = '0')
	// [ 3: 0] : CPRI #4 ~ #1, 
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Tx1_Fault				:1; // 0 
			INT8U Tx2_Fault				:1; // 1 
			INT8U Tx3_Fault				:1; // 2
			INT8U Tx4_Fault				:1; // 3
			INT8U 						:4; // 4~7
			INT8U Rx1_LOS				:1; // 8 
			INT8U Rx2_LOS				:1; // 9 
			INT8U Rx3_LOS				:1; // 10 
			INT8U Rx4_LOS				:1; // 11
			INT8U						:4; // 12~15
			INT8U Optic1_Injection		:1; // 16 
			INT8U Optic2_Injection		:1; // 17
			INT8U Optic3_Injection		:1; // 18 
			INT8U Optic4_Injection		:1; // 19			
			INT8U						:4; // 20~23
			INT8U						:4; // 24~27
			INT8U						:1; // 28 
			INT8U						:1; // 29 
			INT8U CPRI2_L1_Reset		:1; // 30 
			INT8U CPRI1_L1_Reset		:1; // 31			
		}__attribute__((packed));
	}RW_Interrupt_mask1; // 0x21 : Interrupt Mask0 - Optic module & L1 Reset

	//----------------------------------------	
	
	//----------------------------------------	
 	// Default value = 0xFFFF FFFF
 	//----------------------------------------	
 	// [31:1] : --
 	// [0] : PLL Lock, Lock = '1', Fail = '0'
	//----------------------------------------	
	// Write '1' to clear each bit
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Sts				:1; // 0				
		}__attribute__((packed));
	}RW_Interrupt_Status0; // 0x22 : Interrupt Status0 - PLL
	
	//----------------------------------------	
 	// Defaul value = 0x3FFF FFFF
 	//----------------------------------------	
 	// [31:30] : CPRI #1~#2 L1 Reset, Normal = '0', Reset = '1'
 	// [30:20] : --
 	// [19:16] : Optic Injection, (None ='0', Injection ='1')
   	// [19:16] : CPRI #4 ~ #1 
 	// [15:12] : --
 	// [11: 8] : Rx Optic LOS (Mask = '1', Unmask = '0')
   	// [11: 8] : CPRI #4 ~ #1 
 	// [ 7: 4] : --
 	// [ 3: 0] : Tx Fault (Mask = '1', Unmask = '0')
    // [ 3: 0] : CPRI #4 ~ #1 
	//----------------------------------------	
	// Write '1' to clear each bit
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Tx1_Fault				:1; // 0 
			INT8U Tx2_Fault				:1; // 1 
			INT8U Tx3_Fault				:1; // 2 
			INT8U Tx4_Fault				:1; // 3 
			INT8U 						:4; // 4~7 
			INT8U Rx1_LOS				:1; // 8 
			INT8U Rx2_LOS				:1; // 9 
			INT8U Rx3_LOS				:1; // 10 
			INT8U Rx4_LOS				:1; // 11
			INT8U						:4; // 12~15 
			INT8U Optic1_Injection		:1; // 16 
			INT8U Optic2_Injection		:1; // 17
			INT8U Optic3_Injection		:1; // 18 
			INT8U Optic4_Injection		:1; // 19			
			INT8U						:4; // 20~23 
			INT8U						:4; // 24~27 
			INT8U						:1; // 28 
			INT8U						:1; // 29 
			INT8U CPRI2_L1_Reset		:1; // 30 
			INT8U CPRI1_L1_Reset		:1; // 31			
		}__attribute__((packed));
	}RW_Interrupt_Status1; // 0x23 : Interrupt Status1 - Optic module & L1 Reset

	// 2019.07.08 : AAU Test Pattern
	type32 RW_r_pattern_add_fa0;  // 0x24 AAU Test Pattern
	type32 RW_r_pattern_i_fa0;    // 0x25 AAU Test Pattern
	type32 RW_r_pattern_q_fa0;    // 0x26 AAU Test Pattern
	type32 RW_r_pattern_wren_fa0; // 0x27 AAU Test Pattern
	type32 RW_r_pattern_add_fa1;  // 0x28 AAU Test Pattern
	type32 RW_r_pattern_i_fa1;    // 0x29 AAU Test Pattern
	type32 RW_r_pattern_q_fa1;    // 0x2A AAU Test Pattern
	type32 RW_r_pattern_wren_fa1; // 0x2B AAU Test Pattern

	type32 RW_PLL_Addr; // 0x2C AAU PLL_Addr
	type32 RW_PLL_Data; // 0x2D AAU PLL_Data
	
 	//----------------------------------------	
    // [12] : alpha RE4 Reset(Z.215.0)
    // [11] : alpha RE3 Reset(Z.151.0)
    // [10] : alpha RE2 Reset(Z.87.0)
    // [9]  : alpha RE1 Reset(Z.23.0)
    // [8]  : alpha RE Reset(Z.17.0)
    // [7:5]: -
    // [4]  : 100M RE4 Reset(Z.215.0)
    // [3]  : 100M RE3 Reset(Z.151.0)
    // [2]  : 100M RE2 Reset(Z.87.0)
    // [1]  : 100M RE1 Reset(Z.23.0)
    // [0]  : 100M RE Reset(Z.17.0)
	//----------------------------------------	
	// 0 : Disable
	// 1 : Enable
	//----------------------------------------	
	// Write '1' to clear each bit
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U RE__Reset_M			:1; // 0 
			INT8U RE1_Reset_M			:1; // 1 
			INT8U RE2_Reset_M			:1; // 2 
			INT8U RE3_Reset_M			:1; // 3 
			INT8U RE4_Reset_M			:1; // 4 
			INT8U 						:3; // 5~7 
			INT8U RE__Reset_A			:1; // 8 
			INT8U RE1_Reset_A			:1; // 9 
			INT8U RE2_Reset_A			:1; // 10 
			INT8U RE3_Reset_A			:1; // 11
			INT8U RE4_Reset_A			:1; // 12
		}__attribute__((packed));
	}R_RE_Reset; // 0x30 : RE_Reset, 2019.03.20

	//----------------------------------------	
	// [12] : alpha RE4 Reset(Z.215.0)
	// [11] : alpha RE3 Reset(Z.151.0)
	// [10] : alpha RE2 Reset(Z.87.0)
	// [9]	: alpha RE1 Reset(Z.23.0)
	// [8]	: alpha RE Reset(Z.17.0)
	// [7:5]: -
	// [4]	: 100M RE4 Reset(Z.215.0)
	// [3]	: 100M RE3 Reset(Z.151.0)
	// [2]	: 100M RE2 Reset(Z.87.0)
	// [1]	: 100M RE1 Reset(Z.23.0)
	// [0]	: 100M RE Reset(Z.17.0)
	//----------------------------------------	
	// 0 : Disable
	// 1 : clear
	//----------------------------------------	
	// Write '1' to clear each bit
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U RE__Reset_M			:1; // 0 
			INT8U RE1_Reset_M			:1; // 1 
			INT8U RE2_Reset_M			:1; // 2 
			INT8U RE3_Reset_M			:1; // 3 
			INT8U RE4_Reset_M			:1; // 4 
			INT8U						:3; // 5~7 
			INT8U RE__Reset_A			:1; // 8 
			INT8U RE1_Reset_A			:1; // 9 
			INT8U RE2_Reset_A			:1; // 10 
			INT8U RE3_Reset_A			:1; // 11
			INT8U RE4_Reset_A			:1; // 12
		}__attribute__((packed));
	}RW_RE_Reset_Clr; // 0x31 : RE_Reset Clear, 2019.03.20

	//----------------------------------------	
	// 0 : off
	// 1 : on
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U CPRI_0 				:1; // 0 
			INT8U CPRI_1				:1; // 1 
			INT8U CPRI_2				:1; // 2 
			INT8U CPRI_3				:1; // 3 
			INT16U						:12;// 4~15 
			INT16U Disp_Period			:16;// 16~31
		}__attribute__((packed));
	}RW_VSS_Tbl_Dbg; 

	//----------------------------------------	
	// bit[1,0] : '10' Reset, '01' Normal
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Reset 				:2; // 0,1
		}__attribute__((packed));
	}PSU_Reset; // 0x33 , 2019.04.06

	
	//----------------------------------------	
	// bit[1,0] : Green, Red
	//           '1' ON, '0' OFF
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Red 				:1; // 0
			INT8U Green 			:1; // 1
		}__attribute__((packed));
	}RW_DTU_LED; // 0x34 , 2019.04.08
	
	///// YCSO added 190422 AC_Alarm_info
	type32 AC_Alarm_info;	// 0x35	,	2019.04.22

	type32 RW_DTU_Inventory[8]; // Addr : 0x36 ~ 0x3D, 2019.04.30

	//----------------------------------------	
    // [12] : alpha RE4 Reset(Z.215.0)
    // [11] : alpha RE3 Reset(Z.151.0)
    // [10] : alpha RE2 Reset(Z.87.0)
    // [9]  : alpha RE1 Reset(Z.23.0)
    // [8]  : alpha RE Reset(Z.17.0)
    // [7:5]: -
    // [4]  : 100M RE4 Reset(Z.215.0)
    // [3]  : 100M RE3 Reset(Z.151.0)
    // [2]  : 100M RE2 Reset(Z.87.0)
    // [1]  : 100M RE1 Reset(Z.23.0)
    // [0]  : 100M RE Reset(Z.17.0)
	//----------------------------------------	
	// 0 : FPGA Bypass
	// 1 : CPU Access
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U RE__Reset_M			:1; // 0 
			INT8U RE1_Reset_M			:1; // 1 
			INT8U RE2_Reset_M			:1; // 2 
			INT8U RE3_Reset_M			:1; // 3 
			INT8U RE4_Reset_M			:1; // 4 
			INT8U 						:3; // 5~7 
			INT8U RE__Reset_A			:1; // 8 
			INT8U RE1_Reset_A			:1; // 9 
			INT8U RE2_Reset_A			:1; // 10 
			INT8U RE3_Reset_A			:1; // 11
			INT8U RE4_Reset_A			:1; // 12
		}__attribute__((packed));
	}RW_RE_Reset_ACK_SW; // 0x3E : RE_Reset ACK Switch, 2019.05.17
 	
 	//----------------------------------------	
 	// [31:1] : Reserved
	// [0] : AFEPLLLD - ALARM = '0', NOMAL = '1'
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U PLL_LD			:1; // 0				
		}__attribute__((packed));
	}R_AFE_PLL_LD; // 0x3F : 2019.05.29, // AAU Only


	//----------------------------------------	
	// [15:0] wavelength  예) 1330 => 1330 nm
	//----------------------------------------	
	type32 RW_Det_Enable;     // Addr : 0x42 	// [ 7:0] : 각 위치 enable
	type32 RW_Det_Start_Time; // Addr : 0x43 	// [11:0] : Detect Start Time
	type32 RW_Det_Range_Time; // Addr : 0x44 	// [11:0] : Detect Range Time	
	
//----------------------------------------
//		SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	struct
	{
		//----------------------------------------
		// [15:0] LD Power,  예) -517 => -51.7 dBm
		//----------------------------------------	
		type32 R_SFP_LD_Power; // Addr : 0x00 		
		//----------------------------------------	
		// [15:0] PD Power,  예) -517 => -51.7 dBm
		//----------------------------------------	
		type32 R_SFP_PD_Power; // Addr : 0x01 		
		//----------------------------------------	
		// [15:0] [15:0] VCC  예) 31 => 3.1V
		//----------------------------------------	
		type32 R_SFP_VCC; // Addr : 0x02 
		//----------------------------------------	
		// [15:0] VCC  예) 135 => 13.5mA
		//----------------------------------------	
		type32 R_SFP_LD_Bias; // Addr : 0x03 
		//----------------------------------------	
		// [15:0] Termperature  예) -517 => -51.7 ℃
		//----------------------------------------	
		type32 R_SFP_Temp; // Addr : 0x04 
		//----------------------------------------	
		// [15:0] wavelength  예) 1330 => 1330 nm
		//----------------------------------------	
		type32 R_SFP_wavelength; // Addr : 0x05 		
		//----------------------------------------	
		// [0] : TX DISABLE (DISABLE='1',  ENABLE='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U EnDis				:1;				
			}__attribute__((packed));
		}RW_SFP_TX_DISABLE; // Addr : 0x06		
		//----------------------------------------	
		// [0] : Reset (Reset = '0',  Normal='1'), 2019.03.27.changed.
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Reset				:1;				
			}__attribute__((packed));
		}RW_SFP_Reset; // Addr : 0x07 
		//----------------------------------------
		type32 R_SFP_BitRate; // Addr : 0x08 
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U alarm				:1;				
			}__attribute__((packed));
		}R_SFP_LOS_Alarm; // Addr : 0x09
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U alarm				:1;				
			}__attribute__((packed));
		}R_SFP_LD_Alarm; // Addr : 0x0A
		//----------------------------------------
		type32 R_SFP_Inventory[8]; // Addr : 0x0B ~ 0x12 

		type32 R_SFP_Vendor_Name[4]; // Addr : 0x13 ~ 0x16 
		type32 R_SFP_Vendor___PN[4]; // Addr : 0x17 ~ 0x1A 
		type32 R_SFP_Vendor___SN[4]; // Addr : 0x1B ~ 0x1E 
		type32 R_SFP_Date___Code[2]; // Addr : 0x1F ~ 0x20 
	}Common_CPRI[CPRI_PATH]; // SFP Common	
//----------------------------------------
//	End of 
//  SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------




//---------------------------------------------------------------------
// DSP-Delay
// DELAY = delay(uS) * 102.4(MSPS), changed 2019.03.20
// Rang = 0 ~ 12531
//---------------------------------------------------------------------
	type32 RW_DL_DELAY; // addr 0x00
	type32 RW_UL_DELAY; // addr 0x01


	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------	
	// [2]	 : DL_UL,	(DL='0', UL='1')
	// [1:0] : MIMO Num (1st ='0', 2nd ='1', 3rd ='2', 4th ='3')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Mode				:1;			
		}__attribute__((packed));
	}RW_4T2R_Mode;

//------------------------------------------------------------
// Start of DSP-FFT
//------------------------------------------------------------	
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------	
	// [2]   : DL_UL,   (DL='0', UL='1')
	// [1:0] : MIMO Num (1st ='0', 2nd ='1', 3rd ='2', 4th ='3')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U MIMO_Num			:2;
			INT8U DLUL				:1;			
		}__attribute__((packed));
	}RW_FFT_Mux; // 0x10, not used
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [0] : Trigger   (En = '1', Dis='0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT8U Trigger			:1;			
		}__attribute__((packed));			
	}RW_FFT_Trigger; // 0x11, not used
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [0] : Done	(Done='1', NotDone='0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT8U Done				:1;			
		}__attribute__((packed));
	}R_FFT_Done;  // 0x12, not used
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [12:0] : Address
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT16U Address			:12;			
		}__attribute__((packed));
	}RW_FFT_Address; // 0x13, not used

	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [31:0] FFT Result
	//----------------------------------------
	type32 RW_FFT_Result;  // 0x14, not used
	
	type32 RW_Capture_DL_Time;       // 0x20
	type32 RW_Capture_UL_Time;       // 0x21
	type32 RW_Capture_ULNoise_Time;  // 0x22
	type32 RW_UL_Noise_RSSI_StartTime; // 0x30
	type32 RW_UL_Noise_RSSI_StopTime;  // 0x31
	type32 RW_UL_RSSI_StartTime;  // 0x32
	type32 RW_UL_RSSI_StopTime;   // 0x33

	
	//----------------------------------------
	// [1:0] : 
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT8U val		:2;
		}__attribute__((packed));
	}RW_SG_Flat; // 0x34, PRU Only
	
//------------------------------------------------------------
// End of DSP FFT
//------------------------------------------------------------



//------------------------------------------------------------
// Hard Clipping
//------------------------------------------------------------
	struct
	{
		union{
			INT32U Data;
			struct{			
				INT16U value	:16;
			}__attribute__((packed));
		}HC_Threshold; // HardClipping, 0x0

		union{
			INT32U Data;
			struct{			
				INT8U clear		:1;	// 1:clear, 0:normal
			}__attribute__((packed));
		}HC_Clr; // HardClipping, 0x1

		union{
			INT32U Data;
			struct{ 		
				INT16U value	:16;
			}__attribute__((packed));
		}HC_Count; // HardClipping, 0x2

		union{
			INT32U Data;
			struct{ 		
				INT16U value	:16;
			}__attribute__((packed));
		}CFR_Out_Gain; // 0x3, AAU Only

		union{
			INT32U Data;
			struct{ 		
				INT16U value	:16;
			}__attribute__((packed));
		}UL_ALC_GAIN; // 0x4, PRU Only, 2019.04.06

		union{
			INT32U Data;
			struct{ 		
				INT16U value	:16;
			}__attribute__((packed));
		}PAU_DL_Attn_Sync; // 0x5, PRU Only, 2019.04.06

		//----------------------------------------
		// [31:0] RSSI 
		//----------------------------------------
		// dBFs = 10*log10(RSSI/2^31)
		//----------------------------------------
		type32 R_UL_IN_RSSI; // 0x06, 2019.05.03

		type32 RW_DL_Sum_Gain; // 0x07, 2019.07.01

		//----------------------------------------
		// [1:0] val : '1:0' or '0:1' 
		//----------------------------------------
		union{
			INT32U Data;
			struct{ 		
				INT8U val	:2;
			}__attribute__((packed));
		}RW_EQ_Common; // 0x8, PRU Only, 2019.08.12
		
	}DSP_Common_MIMO[MAX_MIMO_PATH];

//------------------------------------------------------------
// Start of 'Mode General-Dsp-Common'
//------------------------------------------------------------
	struct
	{
		//----------------------------------------
		// NCO = target_frequency(MHz)/245.76(MHz)*2^32
		//----------------------------------------
		type32 RW_NCO; // addr 0x00, AAU Only

		//----------------------------------------
		// Default Value = 0x0000 0000
		//----------------------------------------
		// [0] [ Valid = '1' , Invalid = '0']
		//----------------------------------------
		//* NCO Setting sequence
		//----------------------------------------
		// 1. write NCO value
		// 2. write NCO_VALID 1
		// 3. write NCO_VALID 0
		//----------------------------------------
		union{
			INT32U Data;
			struct{				
				INT8U Value				:1;				
			}__attribute__((packed));
		}R_NCO_VALID; // 0x01

		type32 RW_PRU_DL_NCO; // addr 0x02
		type32 RW_PRU_UL_NCO; // addr 0x03
		
	//------------------------------------------------------------
	// Start of 
	// Mode General - Dsp - FA/Port - MIMO #0~#3
	//------------------------------------------------------------
		struct
		{		
			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U OnOff			:1;					
				}__attribute__((packed));
			}RW_DL_OnOff; // 0x00
			//----------------------------------------
			// Default Value = 0x0000 4000
			//----------------------------------------
			// [15:0] Gain
			// {PRU} ----- name은 Gain 이지만, 실제로는 ATT 으로 사용.
			// ATT = 7.5dB = UL OUTPUT ATT Value = ATT * 10 = 75			
			//-------------------------------------
			// {AAU} -----
			// Att 변환식, Att = 20*log10(GAIN/0x4000)
			// Min step = 0.5dB
			//----------------------------------------
			type32 RW_DL_Gain; // 0x01

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [15:0] Level
			//----------------------------------------
			type32 RW_DL_CW_Level; // 0x02

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U OnOff			:1;					
				}__attribute__((packed));
			}RW_DL_CW_On;		//0x03

			//----------------------------------------
			// [31:0] TSSI 
			//----------------------------------------
			// TSSI = Mean(I^2 + Q^2)
			// dBFs 변환식, dBFs = 10*log10(RSSI/2^31)
			// = 10*log10(RSSI) - 10*log10(2^31)
			// = 10*log10(RSSI) - 10*9.331
			// = 10*log10(RSSI) - 93.31dBm
			//----------------------------------------
			type32 R_DL_TSSI; // 0x04

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Inphase Ctrl Value, 
			//        Value = cos(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_DL_Inphase_Cont; // 0x05

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Quadrature Ctrl Value 
			// Value = sin(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_DL_Quad_Cont; // 0x06

			//----------------------------------------
			// Default Value = 0x0000 0005
			//----------------------------------------
			// [2:0] BW
			// 10MHz='0', 20MHz='1', 40MHz='2', 
			// 60MHz='3', 80MHz='4', 100MHz='5'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U BW			:3;					
				}__attribute__((packed));
			}RW_DL_BW_Sel; // 0x07

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [15:0] EQUAIZER_COEFF
			//----------------------------------------
			type32 RW_DL_EQ_Coeff; // 0x08

			//----------------------------------------
			// [31:0] DL_RSSI_Peak 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_DL_RSSI_Peak; // 0x09
			//----------------------------------------
			// [15:0] EQUAIZER_COEFF
			//----------------------------------------
			type32 RW_DL_EQ_1_Coeff; // 0x0A, AAU only
			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{
					INT8U OnOff				:1; // itself
					INT8U Cascade_OnOff		:1;	// cascade 
				}__attribute__((packed));
			}RW_UL_OnOff; // 0x20

			//----------------------------------------
			// Default Value = 0x0000 4000
			//----------------------------------------
			// [15:0] Gain
			//----------------------------------------
			// {PRU} ----- name은 Gain 이지만, 실제로는 ATT 으로 사용.
			// ATT = 7.5dB = UL OUTPUT ATT Value = ATT * 10 = 75			
			//-------------------------------------
			// {AAU} -----
			// Att 변환식, Att = 20*log10(GAIN/0x4000)
			// Min step = 0.5dB
			//----------------------------------------
			type32 RW_UL_Gain; // 0x21

			//----------------------------------------
			// [31:0] RSSI 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_UL_RSSI; // 0x22

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Inphase Ctrl Value 
			//       Value = cos(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_UL_Inphase_Cont; // 0x23

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Quadrature Ctrl Value 
			//        Value = sin(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_UL_Quad_Cont; // 0x24
						
			//----------------------------------------
			// [15:0] Inphase Det
			//----------------------------------------
			type32 R_UL_Inphase_Det; // 0x25 
			
			//----------------------------------------
			// [15:0] UL_Quad_Det
			//----------------------------------------
			type32 R_UL_Quad_Det; // 0x26 

			//----------------------------------------
			// Default Value = 0x0000 0005			
			//----------------------------------------
			// [2:0] BW
			//----------------------------------------
		  	// 10MHz='0', 20MHz='1', 40MHz='2'
		  	// 60MHz='3', 80MHz='4', 100MHz='5'
		  	//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U BW			:3;					
				}__attribute__((packed));
			}RW_UL_BW_Sel; // 0x27
			
			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [15:0] EQUAIZER_COEFF
			//----------------------------------------
			type32 RW_UL_EQ_Coeff; // 0x28
			
			//----------------------------------------
			// [31:0] UL_RSSI_Peak 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_UL_RSSI_Peak; // 0x29
			
			//----------------------------------------
			// [31:0] FB_RSSI_RMS 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_FB_RSSI_RMS; // 0x30, AAU Only

			//----------------------------------------
			// [31:0] FB_RSSI_Peak 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_FB_RSSI_Peak; // 0x31, AAU Only

			//----------------------------------------
			// [31:0] GC_RSSI_RMS 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_GC_RSSI_RMS; // 0x32, AAU Only
			
			#ifdef __TYPE__PRU__
			type32 RW_UL_OUTPUT_ATT; // PRU Only, 0x33
			#else
			type32 RW_UL_EQ_1_Coeff; // 0x33
			#endif
			
			type32 RW_DL_DELAY; // 0x34
			type32 RW_UL_DELAY; // 0x35

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] Alarm = '1', Normal = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U Status		:1;					
				}__attribute__((packed));
			}R_TSSI_SD; // 0x36

			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U OnOff		:1;					
				}__attribute__((packed));
			}RW_TEST_Pattern; // 0x37

			//----------------------------------------
			// [31:0] GC_RSSI_RMS 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_UL_NOISE_RMS; // 0x38

			//----------------------------------------
			// [15:0] Gain, {PRU} ----- name은 Gain 이지만, 실제로는 ATT 으로 사용.
			// ATT = 7.5dB = UL OUTPUT ATT Value = ATT * 10 = 75			
			//-------------------------------------			
			type32 RW_DL_LSH_Gain;		// 2019.07.01
			type32 RW_UL_LSH_Gain;		// 2019.07.01
			type32 RW_Test_Pattern_Band_Gain;	// 2019.07.01
			type32 RW_TSSI_SD_Level_short; // 0x3C  // 2019.07.01
			type32 RW_TSSI_SD_Level__long; // 0x3D  // 2019.07.01
			type32 RW_TSSI_SD_Level___Low; // 0x3E  // 2019.07.01
			type32 RW_Height_Bal_Gain; // 0x3F  // 2019.07.01

			//----------------------------------------
			// [3:0] EQ
			//	0x00 : 적용 안함
			//	0x01 : 1.0dB(반시계 방향)
			//	0x02 : 1.0dB(시계 방향)
			//	0x03 : 0.5dB(반시계 방향
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U val	:4;
				}__attribute__((packed));
			}RW_PAU_EQ_St; // 0x40
		
			
			//----------------------------------------
			// [31:0] RE_Reset
			//----------------------------------------
			// [31:5] : Reserved 
			// [4] : RE4 Reset(Z.215.0)
			// [3] : RE3 Reset(Z.151.0)
			// [2] : RE2 Reset(Z.87.0)
			// [1] : RE1 Reset(Z.23.0)
			// [0] : RE  Reset(Z.17.0)
			// Disable='0', Enable='1'			
			//----------------------------------------
			union{
				INT32U Data;
				struct{ 				
					INT8U RE_Reset			:1; 				
					INT8U RE_Reset1			:1; 				
					INT8U RE_Reset2			:1; 				
					INT8U RE_Reset3			:1; 									
					INT8U RE_Reset4 		:1; 									
				}__attribute__((packed));
			}R_RE_Reset;

			//----------------------------------------
			// [31:0] RE_Reset_Clear
			//----------------------------------------
			// [31:5] : Reserved 
			// [4] : RE4 Reset(Z.215.0)
			// [3] : RE3 Reset(Z.151.0)
			// [2] : RE2 Reset(Z.87.0)
			// [1] : RE1 Reset(Z.23.0)
			// [0] : RE Reset(Z.17.0)
			// Disable='0', Enable='1'			
			//----------------------------------------
			union{
				INT32U Data;
				struct{ 				
					INT8U RE_Reset			:1; 				
					INT8U RE_Reset1 		:1; 				
					INT8U RE_Reset2 		:1; 				
					INT8U RE_Reset3 		:1; 									
					INT8U RE_Reset4 		:1; 									
				}__attribute__((packed));
			}R_RE_Reset_Clear;			
			
		}General_FA_MIMO[MAX_MIMO_PATH];
	
	//------------------------------------------------------------
	// End of 
	// Mode General - Dsp - FA/Port - MIMO #0~#3
	//------------------------------------------------------------
	}General_FA[MAX_FA_PATH];
	
//------------------------------------------------------------
// End of 'Mode General-Dsp-Common'
//         Mode General - Dsp - FA/Port - MIMO
//------------------------------------------------------------




//------------------------------------------------------------
// Start of 'Mode General-VSS-CPRI'
//         Mode General - Dsp - FA/Port - MIMO
//------------------------------------------------------------	

	//------------------------
	// Down-Ward CPRI #0 ~ #3
	//------------------------
	struct{		
		type32 RECSERDESLOS; // addr : 0x00	
		type32 RECLOFALARM;  // addr : 0x01
		type32 RECTypeID;    // addr : 0x02
		type32 CnM_Path_selection; // addr : 0x03
		type32 RE_Reset;           // addr : 0x04
		type32 RE_Channel_BW;      // addr : 0x05
		
		type32 Noti_IP1_Address[4]; // 
		type32 Noti_IP2_Address[4]; // 
		type32 Base_Mac_Address[6]; // 
		type32 OpenCPRIVersion;     // addr : 0x15
		type32 CPRICompressionOnOff;// addr : 0x16
		type32 RE_Cascade_ID;       // addr : 0x17
		type32 OpticLOSDetectAlarm; // addr : 0x18
		type32 Loopbackmode;        // addr : 0x19
		
		type32 Delay_H; // MSB, addr : 0x1A
		type32 Delay_L; // LSB, addr : 0x1B
		
		type32 REx_Reset[4]; // RE1_RESET ~ RE4_RESET, addr : 0x1C ~ 0x1F
		
		type32 RE1TxDelay_H; // MSB, addr : 0x20
		type32 RE1TxDelay_L; // LSB, addr : 0x21
		
		type32 RE1RxDelay_H; // MSB, addr : 0x22
		type32 RE1RxDelay_L; // LSB, addr : 0x23
		
		type32 TDD_Slot_Number[20]; // Number_00 ~ Number_19, addr : 0x24 ~ 0x37
		type32 UL_GainBackOff_Onoff; // addr 0x38, 2019.05.13.
		
		type32 Cell_ID; // addr 0x3A
		type32 gNB_ID_1st; // addr 0x3B
		type32 gNB_ID_2nd; // addr 0x3C
		type32 gNB_ID_3rd; // addr 0x3D
		type32 gNB_ID_4th; // addr 0x3E
		type32 DUH_ID; 	   // addr 0x3F, 2019.12.26
		type32 PCI_1st_Byte; 	// addr 0x40
		type32 PCI_2nd_Byte; 	// addr 0x41
		type32 REC_Port_No; 	// addr 0x42

		type32 Base_IP_Address[4]; // addr 0x44-0x45-0x46-0x47

		type32 CPRI_Version;			// addr 0x48		//3/12 add
		type32 CPRI_Startup;			// addr 0x49
		type32 CPRI_L1_Reset_LOS;		// addr 0x4A
		type32 Pointer_P;		   	    // addr 0x4B

		//----------------------------------------	
		// bit : Alarm = '1', Clear = '0'
		//----------------------------
		// [4] : LOF
		// [3] : LOS
		// [2] : SDI
		// [1] : RAI
		// [0] : Reset (Reset='1', NoReset='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Reset					:1; // 0
				INT8U RAI					:1; // 1
				INT8U SDI					:1; // 2
				INT8U LOS					:1; // 3
				INT8U LOF					:1; // 4
			}__attribute__((packed));
		}L1Reset; // addr : 0x60

		/*
		----------------------------------------
		BIP : DL below only
			0 : CPRI Main 하향
			1 : CPRI Alpha 하향
		----------------------------------------	
		*/
		//----------------------------------------
		// [15:0] BIP Counter
		//----------------------------------------
		type32 BIP_Count;    // addr : 0x61
		type32 BIP_Count_1;  // addr : 0x62, bit  63~32
		type32 BIP_Count_2;  // addr : 0x63, bit  95~64
		type32 BIP_Count_3;  // addr : 0x64, bit 127~96
		//----------------------------------------
		// [0] Clear = '1', Normal = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U Clear				:1; // 0
			}__attribute__((packed));			
		}BIP_Clear;           // addr : 0x65, 2019.03.20 : changed from addr 0x62
		
		type32 BIP_Err_Cnt; // addr : 0x66
		//----------------------------------------
	}DL_CPRI[CPRI_PATH]; // Down-Ward CPRI

	//------------------------
	// Up-Ward CPRI #0 ~ #3
	//------------------------
	struct{
		type32 RE_SERDESLOS; // addr 0x00
		type32 RE_LOFALARM;  // addr 0x01
		type32 REFunctionFail; // addr 0x02

		type32 RE_Reset_Ack; // addr 0x04		
		type32 ALIVE_ACK_00; // addr 0x05
		
		type32 ALIVE_ACK_01; // addr 0x15		
		type32 Mismatch_Noti;// addr 0x16
		type32 RE_Cascade_ID_ACK; // addr 0x17
		type32 RE_OpticLOSDetectAlarm; // addr 0x18
		type32 RE_Loopbackmode_ACK;    // addr 0x19

		type32 RE_Reset_ACK[4]; // RE1_RESET_ACK ~ RE4_RESET_ACK, addr : 0x1C ~ 0x1F		
		type32 RE_FunctionFail[4]; // RE1_FucnFail ~ RE4_FucnFail, addr 0x20 ~ 0x23
			
		type32 RE_Type_ID; // addr 0x38
		type32 RE_Vendor_ID; // addr 0x3B
		
		type32 CPRI_Version;			// addr 0x48		// 2019.03.12. added
		type32 CPRI_Startup;			// addr 0x49
		type32 CPRI_L1_Reset_LOS;		// addr 0x4A
		type32 Pointer_P;			    // addr 0x4B
		
		//----------------------------------------	
		// bit : Alarm = '1', Clear = '0'
		//----------------------------
		// [4] : LOF
		// [3] : LOS
		// [2] : SDI
		// [1] : RAI
		// [0] : Reset (Reset='1', NoReset='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Reset					:1; // 0
				INT8U RAI					:1; // 1
				INT8U SDI					:1; // 2
				INT8U LOS					:1; // 3
				INT8U LOF					:1; // 4
			}__attribute__((packed));
		}L1_Reset_ACK; // addr : 0x40

		
		/*
		----------------------------------------
		BIP : UL below only
			6 : CPRI Main Cascade 상향
			7 : CPRI Alpha Cascade 상향
		----------------------------------------	
		*/
		//----------------------------------------
		// [15:0] BIP Counter
		//----------------------------------------
		type32 BIP_Count;    // addr : 0x61		
		type32 BIP_Count_1;  // addr : 0x62, bit  63~32
		type32 BIP_Count_2;  // addr : 0x63, bit  95~64
		type32 BIP_Count_3;  // addr : 0x64, bit 127~96
		//----------------------------------------
		// [0] Clear = '1', Normal = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U Clear				:1; // 0
			}__attribute__((packed));
		}BIP_Clear; // addr : 0x65, 2019.03.20 : changed from addr 0x62
		type32 BIP_Err_Cnt; // addr : 0x66
		//----------------------------------------			
	}UL_CPRI[CPRI_PATH]; // Up-Ward CPRI
//------------------------------------------------------------
// End of 'Mode General-VSS-CPRI'
//         Mode General - Dsp - FA/Port - MIMO
//------------------------------------------------------------		





//------------------------------------------------------------
// Start of 'Mode General-TDD'
//------------------------------------------------------------	
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [1:0] : Mode  
	//       Normal='0', DL_ONLY='1', UL_ONLY='2', FDD='3'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Mode				:2;
		}__attribute__((packed));
	}RW_TDD_Mode; // 0x00

	//----------------------------------------	
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [7:0] : Slot Format
	//----------------------------------------
	type32 RW_Slot_Format[20]; // addr 0x01 ~ 0x14, Format_00 ~ Format_19

	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [10:0] Value    delay(uS) =  Value / 122.88(MSPS)
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:12; // 2019.04.17 : 11 -> 12
		}__attribute__((packed));
	}DL_ON_Guard_Time; // 0x15
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:12; // 2019.04.17 : 11 -> 12
		}__attribute__((packed));
	}UL_ON_Guard_Time; // 0x16
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:15; // 2019.04.17 : 11 -> 12, 2019.04.19: 12->15(by AAU) 
		}__attribute__((packed));
	}DL_to_UL_Guard_Time; // 0x17
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:15; // 2019.04.17 : 11 -> 12, 2019.04.19: 12->15(by AAU)
		}__attribute__((packed));
	}UL_to_DL_Guard_Time; // 0x18

	type32 UL_GAIN_Guard_Time[5];  // addr : 0x19 ~ 0x1D
	type32 UL_GAIN_Measure_Time; // addr : 0x1E
	type32 TSYNC_Delay;          // addr : 0x1F
	//----------------------------------------
	// [0] Fail = '1', Normal = '0'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Fail				:1; // 0
		}__attribute__((packed));			
	}TSYNC_FAIL; // addr : 0x19	
	
	//----------------------------------------	
	struct{
		type32 t_Offset;          // addr : 0x21
		type32 TB_Delay_DL;       // addr : 0x22
		type32 TB_Delay_UL;       // addr : 0x23
		type32 Round_Trip_Dly;    // addr : 0x24
		type32 T2A_Dly;    		  // addr : 0x25, T2A delay
		type32 TA3_Dly;           // addr : 0x26, TA3 delay
	}DLY[MAX_Optic]; // Delay
	
	
//------------------------------------------------
// CFR, PRU Only, DL #0 ~ #3
//------------------------------------------------
	struct{		
		union{		
			INT32U Data;
			struct{
				INT8U Val		:1;			
			}__attribute__((packed));
		}OnOff;
		
		type32 Version; 
		type32 Level; 
	}CFR[MAX_DL]; // CFR

//------------------------------------------------
// DPD, PRU Only, DL #0 ~ #3
//------------------------------------------------
	struct{		
		union{		
			INT32U Data;
			struct{
				INT8U Val		:1;	// '0' is 'ON'		
			}__attribute__((packed));
		}OnOff;
		
		type32 Version;
		type32 Status;
		type32 Err_St;
		type32 Success_Cnt; // 2019.05.20 Success_Cnt
		type32 Input_Pwr;		
		type32 Output_Pwr;
		type32 Eng_In_Pwr;
		type32 Eng_Out_Pwr;
		type32 FB_In_Pwr;
		type32 FB_Eng_In_Pwr;
		type32 DPD_Reset;   // 2019.03.14.added
		
		type32  PeakExpansion;
		type32  MSE;
		type32  PRE_MSE;
		type32  LUT_Value;
		type32  Counts;
		
	}DPD[MAX_DL]; // DPD

	type32 DPD_Core_Reset;  // 2019.05.01.added
	type32 DPD_All_Reset;   // 2019.05.01.added
	
//------------------------
// RF, AAU Only, MIMO #0 ~ #3
//------------------------
	struct{
		//----------------------------[[
		// [6] : Trigger
		// [5:0] : Reset (Reset='1', NoReset='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Val_x4				:8; // 0~5
				INT8U Trig					:1; // 6, trigger, 0->1, rising trigger
			}__attribute__((packed));
		}DL_Att; // addr : 0x00
		
		union{
			INT32U Data;
			struct{
				INT8U Val_x4				:8; // 0~5
				INT8U Trig					:1; // 6, trigger, 0->1, rising trigger
			}__attribute__((packed));
		}UL_Att; // addr : 0x01		
		//----------------------------------------
		// [0] ON = '1', Off = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U On				:1; // 0
				INT8U UL_LNA			:1; // 1
			}__attribute__((packed));
		}PA_OnOff; // addr : 0x02, 2019.05.03 : PAU && AAU	

		//----------------------------------------
		// [0] USE = '1', Not Use = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U Use				:1; // 0
			}__attribute__((packed));
		}LayerInfo; // addr : 0x03, 2019.05.03 : PAU, Layer Use, AID 0x5002
		//----------------------------------------			
	}RFU[MAX_MIMO_PATH]; // RFU



//------------------------
// ALC, AAU Only, MIMO #0 ~ #3
//------------------------
	struct{
		union{
			INT32U Data;
			struct{
				INT8U Val_x2				:6; // 0~5
			}__attribute__((packed));
		}ALC_Threshold; // addr : 0x00

		union{
			INT32U Data;
			struct{
				INT8U Val_x1				:6; // 0~5, 1dB Step				
			}__attribute__((packed));
		}Def_Att; // addr : 0x01, default ATT
		
		union{
			INT32U Data;
			struct{
				INT8U Val_x1				:6; // 0~5
				INT8U Trig					:1; // 6, trigger, 0->1, rising trigger
			}__attribute__((packed));
		}ALC_Att; // addr : 0x02		
		//----------------------------------------
		// [0] ON = '1', Off = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U On				:1; // 0
			}__attribute__((packed));
		}Mode; // addr : 0x03	
		//----------------------------------------			
	}ALC[MAX_MIMO_PATH]; // ALC

	//------------------------
	// Comp_OnOff
	//----------------------------------------
	// [0] ON = '1', Off = '0'
	//----------------------------------------
	struct{
		union{
			INT32U Data;
			struct{
				INT8U OnOff				:1;
			}__attribute__((packed));
		}Ctrl; // addr : 0x00

		//----------------------------------------			
	}COMP_ONOFF[CPRI_PATH]; // ALC


	//------------------------
	// Axi
	//------------------------
	struct
	{
		union{
			INT32U Data;
			struct{
				INT8U Alarm				:8;
			}__attribute__((packed));
		}Cpri_Sts; // addr : 0x00

		/*
			Miscellaneous Status	
			------------------------------
			15 Latched version of local loss of frame synchronization (LOF)
			14 Latched version of local loss of signal (LOS)
			11 Latched version of remote loss of signal (LOS)
			0  Local loss of signal(1)
		*/
		union{
			INT32U Data;		
			struct
			{
				INT8U SFP_PD	    :1; // bit 0, SFP PD
				INT8U  				:7;	// bit 1 ~ 7 
				INT8U  				:3;	// bit 8 ~ 10 
				INT8U SFP_LD	    :1; // bit 11, SFP LD, Remote
				INT8U  				:2;	// bit 12 ~ 13
				INT8U LOS_CPRI		:1; // bit 14, LOS, local
				INT8U LOF_CPRI		:1; // bit 15, LOF, local
			}__attribute__((packed));
		}Mscl_Status;	
	//----------------------------------------			
	}AXI[CPRI_PATH]; // ALC

//------------------------------------------------------------
// End of 'Mode General-TDD'
//------------------------------------------------------------	
}ST_DtuStatus;

typedef union{	
	INT32U Data;
	struct{
		INT16U reserved	:9; // 0~8 
		INT16U addr8	:8; // 9~16
		INT16U mimo		:4; // 17~20
		INT16U fa_port 	:4; // 21~24
		INT8U  func		:4; // 25~28
		INT8U  mode		:1; // 29
		INT8U  rdwr		:2; // 30~31
	}__attribute__((packed));	
}REG_ADDR;

/********************************************************************************/
#endif
/********************************************************************************/

/* EOF */

