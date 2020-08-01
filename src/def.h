/********************************************************************************/
#ifndef  __DEF_H__
#define  __DEF_H__
/********************************************************************************/
#ifdef  MAIN_GLOBALS
#define DEF_EXT
#else
#define DEF_EXT extern
#endif



//##############################################################################



//##############################################################################

/**************************************************************/
#define EEP0_ADDR_0	    (0)	     // m_Eep  시작 위치          ==> 일반 EEP 141016시점 264Byte
#define EEP0_ADDR_ACC_START	    (0x01000) // Account 정보 6004 Byte
/*
#define EEP0_ADDR_LOG_START	    (0x01000)
#define EEP0_ADDR_LOG_SIZE	    (0x1C000) // 112 KByte,
//#define EEP0_ADDR_LOG_SIZE	    (600) // 112 KByte,
#define EEP0_ADDR_LOG_END	    (EEP0_ADDR_LOG_START+EEP0_ADDR_LOG_SIZE)

#define EEP0_ADDR_REV		    (0x1E000)
*/
#define _MAX_Delay	((INT16U)10000)	// 100usec
//#define _Delay_Err	10	// 100nsec

// 2015_12_05 수정 지연 보정 알람 역치는 0.4usec.
//////////////////////////////////////////////////////////////////
#define _Delay_Err	40	// 400nsec

#define RO_NOTE_LEN (30) // 길이 30바이트.
#define RO_NOTE_LEN_100 (100) // 길이 100바이트.


#define Node_Name_MAX (100)
#define BarCode_Name_MAX (14) // 실제는 13byte+Null
#define BarCode_MAX (50)

#define MUTI_DOWN_NEED (0x55ABCDEF)

//------------------------------------------------
// m_Eep ASCII Data Size
//----------------------
#define  SN_SIZE      (10) // 10 byte used, 10byte reserved
#define  BD_NANEMSIZE (16) // 16 byte used, 16byte reserved

#ifdef __TYPE__AAU__
///// YCSO added 190716 START
#define	BT_DEVICE_NAME_SIZE (30)
#define	BT_PAIRING_CODE_SIZE (16)
///// YCSO added END
#endif

//AAU Det Table
#define MAX_RF_PWR_TABLE       28




typedef struct __attribute__((packed))
{
	// -------------------
	// 5G GUI 
	// -------------------
	INT8U  TempComp_Onoff; // Temp Compensation onoff, 0xB434, AAU-DTU


	// -------------------
	// 5G : Used IE(Information Element) with L/S
	// -------------------
	
	INT8U  LS_Regist_Num[20];    // 0xB007, 10 byte used, 10byte reserved, LS_Registration_Num, name change 'Serial_Num' -> LS_Regist_Num
	INT8U  RU_Board_Name[32]; // 0xB008, 16 byte used, 16byte reserved
	INT32S Time_Tick_Offset;  // 0xB006, unit : SecTime Tick 기준(-12~12 시간), 4-byte




	// -------------------
	// RCU 
	// -------------------
	INT32U Wr_Cnt32;
	INT32U reset_cnt;
	
	
	INT8U  mask; // Alarm Mask, 0x00: Unmasking,  0x01: Masking
	
	INT8U  MY_SUBID[4];
	
	INT8S  reserved_Temp_Offset_RCU; // 2019.06.18
	INT8S  TempLmt; // reserved	
	INT8U  AAU_Type[2];
	

	INT16S DL_TSSI_UP_x10[MAX_Optic][MAX_UL]; // DL TSSI upper limit, 0x5101
	INT16S DL_TSSI_DN_x10[MAX_Optic][MAX_UL]; // DL TSSI lower limit, 0x5102

	INT16S UL_RSSI_UP_x10[MAX_Optic][MAX_UL]; // UL RSSI upper limit, 0x5201
	INT16S UL_RSSI_DN_x10[MAX_Optic][MAX_UL]; // UL RSSI lower limit, 0x5202

	INT8U  Layer_Mode; // 0x5433
	/*
		0x00: 1-Layer SISO mode 
		0x01: 2-Layer MIMO mode (DL0/DL1,DL2/DL3)
		0x02: 2-Layer MIMO mode (DL0/DL2, DL1/DL3) 
		0x03: 2-Layer MIMO mode (DL0/DL3, DL1/DL2) 
		0x04: 4-Layer MIMO mode 
	*/
	
	INT8U  System_Shape_reserved; // 0x5009, System 형상, PRU Only -- 2019.04.18. AID deleted.
	INT8U  Fused_MAC[6]; // 2019.05.30
	INT16S TempLmt_x10;	
	INT16U AAU_DTU_Comm_Fail_Cnt;
	//----------------------
	INT8U  PRU_TYPE;
	INT8U  RCU_Reserve[419]; // 2020.07.14 : 420 -> 419
	//----------------------

	INT16S PD_Pwr_Lmt_x10[UPDN][CPRI_PATH]; // 2020.06.24 : 0x530C/09
	
	//----------------------
	// RCU Inventory
	//----------------------	
	INT8U sn[4];
	INT8U Maker_Mode[4];
	INT8U date[3];
	INT8U date_reserve;
	INT8U ERP_code[15];
	INT8U ERP_reserve;	

	// -------------------
	// DTU
	// -------------------
	INT16U d_DL_ON_Guard_Time;
	INT16U d_UL_ON_Guard_Time;
	INT16U d_DL_to_UL_Guard_Time;
	INT16U d_UL_to_DL_Guard_Time;	
	INT8U  d_Payload_OnOff[MAX_Optic][MAX_MIMO_PATH]; // Payload On/Off, 0xB431, ON(1), OFF(0)
	INT8U  d_RE_Channel_BW[MAX_Optic];     // AID 0xB41E, 0x5008
	
	INT8U  d_Rx_Casc_Add_OnOff_UL;        // 0xB435
	INT8U  d_Rx_Casc_Add_OnOff[MAX_Optic][MAX_MIMO_PATH];

	INT16U d_UL_Gain_Guard_time[5]; // AID 0xC020, [usec]
	
	INT32U d_t_Sync_Delay;	// 0xC021
	INT32U d_UL_Gain_Meas_time; // AID Not Allocated

	INT32U d_Offset_DTU_Delay_DL[MAX_Optic][MAX_DL]; // 0xB415, nano_sec, 2019.04.06
	INT32U d_Offset_DTU_Delay_UL[MAX_Optic][MAX_DL]; // 0xB416, nano_sec, 2019.04.06
	
	INT8U  d_Path_OnOff_DL[MAX_Optic][MAX_MIMO_PATH]; // 0xB42E
	INT8U  d_Path_OnOff_UL[MAX_Optic][MAX_MIMO_PATH]; // 0xB42F

	INT32U LS_Delay_DL_nsec[MAX_Optic]; // Delay Setting Value from LayerSplitter 
	INT32U LS_Delay_UL_nsec[MAX_Optic]; // Delay Setting Value from LayerSplitter 

	INT32U Variable_Init_20190418;
	INT8U  DTU_Reserve[22];    // 95 -> 94 -> 78 -> 62 -> 34 -> 26 -> 22 : 2019.08.12
	INT32S reserved_TSSI_Dec_Offset;
	INT16S d_Offset_DPD_x10[MAX_DL]; // 0xB410.

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
	
	INT16U d_Att_Dig_UL_User_x10[MAX_Optic][MAX_DL]; // 0x5206, 0.1 step, 2019.05.31, conversion-att
	INT16U d_UL_GainBal_x10[MAX_FA_PATH][MAX_MIMO_PATH]; // AID 0x5209 : Gain Balance
	INT8U  Height_Bal_Onoff; // 2019.08.06, 0xC06C, 1(on), 0(0ff)

	INT16S AAU_48V_Offset_x10; // 2019.05.27, 0xC05E
	INT16S AAU_48V_Limit_x10[2]; // 2019.05.27, 0xC05F
	
	INT16S Offset_RSSI_Before_Filter_x10[MAX_MIMO_PATH]; // 2019.05.27, AID 0xC05B	
	INT8U  reserved_8[MAX_FA_PATH][MAX_MIMO_PATH];

	INT16S UL_Noise_Offset_x10[MAX_Optic][MAX_MIMO_PATH]; // 2019.05.27, AID 0xB309, 0xC05C
	INT16S UL_GC_RSSI_Offset_x10[MAX_Optic][MAX_MIMO_PATH]; // 2019.05.27
	INT8U  UL_Gain_Chk_Mode; // ANTS-AAU Only, 2019.05.23, 0xC054, 1(on), 0(0ff)
	INT8U  DL_Mode_En;   // 0xC059, ANTS AAU Only
	INT16S UL_Gain_Low_x10[MAX_MIMO_PATH];   // 0xC058, Gain 저하(UL) Threshold, ANTS AAU Only
	INT16S Threshold_UL_Gain_Low_x10;   // 0xB305, Gain 저하(UL) Threshold 
	INT16U d_DL_CW_LEVEL[2][4];  // 0xC012, 2019.05.16
	INT16S RSSI_Offset_x10[MAX_Optic][MAX_MIMO_PATH]; // RSSI Offset : 0xB307, 2019.05.10
	INT16S TSSI_Offset_x10[MAX_Optic][MAX_MIMO_PATH]; // TSSI Offset : 0xC052, 2019.05.15
	INT16S DL_FB_Offset_x10[MAX_Optic][MAX_MIMO_PATH]; // AAU, DL FB Offset : 0xC051, 2019.05.15

	INT32U d_SysOffset_Delay_DL[MAX_Optic][MAX_DL];
	INT32U d_SysOffset_Delay_UL[MAX_Optic][MAX_DL];	

	INT32U d_FPGA_Delay_ns_DL[MAX_Optic][MAX_MIMO_PATH]; // FPGA Delay : (DL) 0xB429, eep save 하지만, 사용되지 않음.
	INT32U d_FPGA_Delay_ns_UL[MAX_Optic][MAX_MIMO_PATH]; // FPGA Delay : (UL) 0xB42A, eep save 하지만, 사용되지 않음.
	
	INT16S Threshold_UL_Spurious_x10;   // 0xB308, Threshold(UL 불요파)
	
	INT8U  d_Hard_Clip_OnOff[MAX_DL];  // 0xB40C, 2019.04.22, added
	INT16S d_Hard_Clip_Lvl_x10[MAX_DL]; // 0xB40D, 2019.04.22, added
	// 2019.04.17 : 403 -> 387
	// 2019.04.18 : 387 -> 371
	// 2019.04.18 : 371 -> 367
	// 2019.04.22 : 367 -> 363 -> 355
	
	// -------------------
	// AMP
	// -------------------
	INT8U  Rvs_Att_x2[AMPU_4_MAX];  // 0xAC05
	
	INT8U  LayerUseInfo[AMPU_4_MAX]; // AID 0x5002, PAU USE Info, (1:use, 0/2/3:not-use)
	INT16S Fwd_Out_Lmt_x10[AMPU_4_MAX][LO_HI]; // 5G, Used, AID 0x5104/05 	

	INT8U  Fwd_Out_Att_x2[AMPU_4_MAX];
	INT8S  FWD_Att_Offset_x2[AMPU_4_MAX];  // 0xAC12	

	INT8U  AMP_Rvs_Att_x2[AMPU_4_MAX];
	INT8U  RVS_Amp_Onoff[AMPU_4_MAX];	// AAU
	INT8U  Rvs_Gain_Bal_x2[AMPU_4_MAX]; // 0xB232, AAU, 2019.04.20
	INT16S Rvs_IN_ALC_x10[AMPU_4_MAX];  // ALC Level, 0xB234, AAU, 2019.04.19
	INT8U  Rvs_IN_ALC_Onoff[AMPU_4_MAX];  // 0xB230, AAU, 2019.04.19
	INT16S Rvs_IN_SD_x10[AMPU_4_MAX];  // SDLevel, 0xB235, AAU, 2019.04.19
	
	INT8U  Rvs_Att_Offset_x2[AMPU_4_MAX];  // 0xAC06
	INT16U Over_Temp_Thr_x10[AMPU_4_MAX];  
	INT16U Over_Temp_Rec_Thr_x10[AMPU_4_MAX];  
	
	INT16U Ret_Loss_Rec_x10[AMPU_4_MAX];  
	
	INT16S Over_Pwr_Thr_x10[AMPU_4_MAX]; // AID 0xB20D, AAU Only, 2019.04.22, DL OverPwr Shutdown Level 
	INT16U Ret_Loss_Thr_x10[AMPU_4_MAX];  
	
	
	INT16U FWD_Dly_Offset_x100[AMPU_4_MAX]; // [AMP], 0xAC45 
	INT16U RVS_Dly_Offset_x100[AMPU_4_MAX]; // [AMP], 0xAC47

	INT8U  ATT_Pair_Onoff[AMPU_4_MAX];   // 5G, 0xB231, 
	INT8U  FB_ATT_x2[AMPU_4_MAX];        // 0xB221, AMP Feedback Pwr Offset, 
	
	INT8U  Rvs_Gain_Bal_On[AMPU_4_MAX];  // 0xB233,	AAU, 2019.04.20	

	INT16S FWD_Pwr_Offset_x10[AMPU_4_MAX];	    // 0xB210, 2019.04.30
	INT16S Rvs_OUT_Pwr_Offset_x10[AMPU_4_MAX];	// 0xB212, AAU Only, 2019.04.20
	INT16S Rvs_IN_Pwr_Offset_x10[AMPU_4_MAX];	// 0xB213, AAU Only, 2019.04.20

	INT16S Temp_Offset_x10[AMPU_4_MAX]; // 0xB217
	INT16S AFE_Rvs_ALC_x10[AMPU_4_MAX]; // 2019.05.13, 0xC04A, AAU-ANTS Only
	INT16S AAU_Fwd_ALC_x10[AMPU_4_MAX]; // 2019.05.13, 0xC04C, AAU-ANTS Only
	INT16S AAU_Fwd_SD__x10[AMPU_4_MAX]; // 2019.05.13, 0xC04D, AAU-ANTS Only

	INT8U  AAU_Fwd_ALC_Onoff[AMPU_4_MAX];  // 0xC04B, AAU, 2019.05.13
	INT8U  AAU_Fwd_SD__Onoff[AMPU_4_MAX];  // 0xC050, AAU, 2019.05.13
	
	INT8U  AMP_Reserve[444]; // 2019.05.13 : 468 -> 444

	
	//------------------------------
	// AMP Stdby mode
	//------------------------------
	INT8U AMP_Stdby_Init_Use; // 'G' 인지 확인. 아니면, 디폴트 값 적용. ('F', 'G')
	INT8U AMP_Stdby_Mode_Onoff; // <0: On, 1: Off>
	INT8U AMP_Stdby_Start_Time[2];// [hour/mimute]
	INT8U AMP_Stdby_Stop_Time[2]; // [hour/mimute]
	INT8U AMP_Stdby_Path; // AID 0x5437, <0: On, 1: Off>, bit 0 ~ bit 3
	//------------------------------


	// -------------------
	// PSU 
	// -------------------
	INT8U  Ext_FAN_Speed;  // 0xB612, FAN RPM
	INT8U  FAN_Auto[2];    //  자동 제어 [case/dtu]
	INT8U  FAN_OnOff[2];   //  
	INT8S  FAN_On_Temp[2]; //  동작 온도
	INT8S  FAN_OffTemp[2]; //  동작 온도
	
	INT8U  Green_Mode_On;	
	INT8U  Green_Mode_Start_Time[2];
	INT8U  Green_Mode_Stop_Time[2]; 
	INT8U  Batt_Use; // (0 : not Insert)

	INT8U  FAN_Mode_Onoff;	// 0x5410, FAN Mode
	INT16U FAN_Ext_Max_RPM; // 0xB616
	
	INT8U  FAN_Mode_Start_Time[2]; // 0x5412, [hour/mimute], // FAN Mode 동작 시간	
	INT8U  FAN_Mode_Stop__Time[2]; // 0x5412, [hour/mimute], // FAN Mode 동작 시간	
	INT8U  Batt_Ext_Mode_Onoff;    // 0x5438
	
	INT8U  PSU_Reserve[500];

	INT16U DC_Fail_Volt_x10[2]; // 0xC048, 0xC049	
	//###################################################
	
	INT16S RCU_Temp_Offset_x10; // 0xB417, 2019.05.09, EMS 보고 온도에 대한 Offset 용도
	INT8U CW_Freq_reserved[14];
	INT8U f_UL_Gain_Low_Offset[AMPU_4_MAX]; // if 'F' is 'ckeck_end', else 'need to check'.
	
	INT8U Auto_SD_Mode_En[AMPU_4_MAX];
	INT8U UL_Gain_A_BackOff; // AID 0x5450, 2019.04.27, Auto-Backoff
	INT8U UDA_Oper_Mode[4];  // UDA Mode (0:on), AID 0x5444/??/0x5446/0x5447/
		
	INT8U reserved_UL_Gain_Low_Offset[AMPU_4_MAX];
	//------------------	
	INT8S	PD_Pwr_Low_lmt_x2;	
	//------------------	
	INT16U  Delay_Set_x10;
	INT8U	reserved_SD;
	INT32U  AAU_Def_Setting_20200204; // 0x20200204
	//---------------------------------------------------------

	INT8U  Optic_SW; // 0x00 : 미절체, 0x01 : 절체	
	INT8U  OSM_Use;  // 5G, AID 0x501C, <0x00: 미사용, 0x01: 사용>
			
	INT16U  FWD_FPGA_Dly_x100[AMPU_4_MAX]; // 사용자 설정.
	INT16U  RVS_FPGA_Dly_x100[AMPU_4_MAX];
		
	INT8U  d_DPD_OnOff[MAX_DL]; // DPD onoff, 0xB404
	INT8U  d_CFR_OnOff[AMPU_4_MAX];
	INT16U d_CFR_Level_x10[AMPU_4_MAX]; // 2019.07.04
	
	INT8U  Note_Str[RO_NOTE_LEN+10];
	
	//---------------------
	// BIP_Err
	//---------------------
	INT8U  Bip_Err_Ref; // Err가 몇회일 때, 알람처리?, 0x114A
	INT16U Bip_Err_Accum_Time; // 누적시간, 1~1000초, 0x114B
	INT8U  EBER; // 0x114B
	//---------------------
	//---------------------
	INT8U MIMO_Stdby_Mode_Onoff;
	INT8U MiMo_Stdby_Start_Time[2];// [hour/mimute]
	INT8U MiMo_Stdby_Stop_Time[2]; // [hour/mimute]

	INT16S MIMO_Stdby_Level_x10[2]; // AID 0x543A/3B
	INT8U Ftp_Dn_St_reserved[12];	
	//---------------------	
	/*
	2019.11.18 : GUI ID/PASS 오류 건 보완: 	GUI ID/PASS 저장 영역을 새로 할당
	*/
	INT8U reserved_User_ID[12];
	INT8U reserved_User_PW[12];	
	// ----------------------------	
	INT8U Log_OnOff;  // 0x5453, (0 : ON, 1 : OFF)	
	INT8U reserved_Dig;
	// ----------------------------
	
	

	// EMS
	INT8U Statis_Rpt_Ref[2]; // 0x5600, @ 5G, stat. report ref time
	INT8U Statis_Rpt_Prd;
	INT8U REMS_Auto_Rpt_Time[2]; // auto report ref time (CF/C, CF/nC)
	INT8U reserved_for_EMS[20];
	//------------------------------
	// Node Name
	INT8U Node_Name[Node_Name_MAX]; // max 100byte, 국소명
	INT8U Node_Name_len;

	//------------------------------
	// Bar-Code
	INT8U BarCode_Name[BarCode_MAX];
	INT8U BarCode_len;
	//------------------------------	

	//------------------------------
	// SIM threshold
	INT16U thhld_VSWR_x100; // threshlod, 0xACCD
	INT16U thhld_AC_x10;    // threshlod, 0xACCE
	INT16U thhld_Watt;   // threshlod, 0xACD0/CF
	//------------------------------	

	INT8U  reserved_Note_Str_100[98]; // 2020.04.02, '110' -> '98'
	INT8U  d_Reg_PLL_Set[LSH_Max][4]; // [LS-3][Cascade-4], 2020.04.02, AAU Only, 0xC090~92
	//------------------------------
	//PRU/AAU AutoConfiguration Backup
	//190226 --> Start
	
	INT8U Last_RECTypeID;

	INT8U Last_Ip_SS[4];
	INT8U Last_GW_SS[4];
	INT8U Last_SM_SS[4];
	INT8U Last_Mac_SS[6];

	INT8U Last_Ip_Etc[4];
	INT8U Last_GW_Etc[4];
	INT8U Last_SM_Etc[4];
	INT8U Last_Mac_Etc[6];

	//-------------------------------	
	INT8U RW_TDD_Mode;	
	INT8U DL_Att_Offs[AMPU_4_MAX]; // ATNS-AAU-RF-0.25dB Unit, Offset Atten
	INT8U DL_Att_User[AMPU_4_MAX]; // ATNS-AAU-RF-0.25dB Unit, User Atten
	INT8U UL_Att[AMPU_4_MAX]; // ATNS-AAU-RF-0.25dB Unit
	INT8U Pwr_Amp_En[AMPU_4_MAX]; // AID 0x5116 

//--------------------------------------------------------------	
// New EEP DATA here !! 
// from 2019.03.10
//--------------------------------------------------------------	

	INT8U   UL_AFE_ATT_x1[AMPU_4_MAX]; // AAU Only, 2019.08.13, 1dB Step
	INT32S  d_TBdelay_DL[MAX_Optic];    //(DL) 0x5401, 2019.04.18 : Status only, it will be deleted.. 
	INT32S  d_TBdelay_UL[MAX_Optic];    //(UL) 0x5402, 2019.04.18 : Status only, it will be deleted.. 

	INT8U   d_t_reserved[2];
	INT32U  d_CenterFreq[MAX_FA_PATH];	// 0x540B, 0.001Mhz, 2019.05.30
	
	INT16S d_EQ_Coeff_DL[MAX_FA_PATH][MAX_MIMO_PATH]; 	// 	0x540A, 0xC014
	INT16S d_EQ_Coeff_UL[MAX_FA_PATH][MAX_MIMO_PATH]; 	// 	0x540B, 0xC019

	INT16U d_UL_Att_x10[MAX_FA_PATH][MAX_MIMO_PATH];  // AID 0x5206	: PRU(DTU), 0.1dB Step, 2019.04.19	

	INT8U  VSS_Monitor_OnOff; // 0xB46E, Needed

	// 2019.03.21 --------------------
	INT16U d_Att_Dig_DL_x10[MAX_Optic][MAX_DL]; // 0xB420, 0.1 step
	INT16U d_Att_Dig_UL_Offs_x10[MAX_Optic][MAX_DL]; // 0xB421, 0.1 step, Offset att, delta-att
	
	INT32U d_NCO[MAX_Optic]; // 0xC010
	INT8U  d_NCO_VALID[MAX_Optic]; 	// 0xC011	

	INT32S d_Toffset[MAX_Optic];   	   // 0x5400, 2019.04.18 : Status only, it will be deleted.. 
	INT32U d_RoundTrip_Dly[MAX_Optic]; // 0x5405, 2019.04.18 : Status only, it will be deleted.. 
	

	INT8U  TestPort_CIM; // Test Port 출력	0x543F
	INT8U  d_RE_Channel_BW_H;     // AID 0xB41E, 0x5008

	INT8U TempComp_OnOff;

	INT8U  Other_Rvs_IN_ALC_Onoff[AMPU_4_MAX];  // 0xF2F3, 2019.05.06, Temporary

	//-------------------------------------------------------------------------
	////////////INT32U reserved_d_NCO_PRU[MAX_Optic][DL]; // 2019.08.13-- deleted......	
	//-------------------------------------------------------------------------
	
	INT16S d_EQ_Coeff_1_DL[MAX_FA_PATH][MAX_MIMO_PATH]; 	// 	0xC01A, AAU Only
	INT16S d_EQ_Coeff_1_UL[MAX_FA_PATH][MAX_MIMO_PATH]; 	// 	0xC01B, AAU Only
	
	INT16U d_LS_gain_x10[LSH_Max][DL_UL][MAX_FA_PATH][MAX_MIMO_PATH]; // 2x2x2x4	
	INT16U d_Test_Pattern_Band_Gain[MAX_FA_PATH][MAX_MIMO_PATH];
	INT16U d_Att_DL_SUM[MAX_DL]; // 0xB424,
	INT16U d_PAR_x10[MAX_MIMO_PATH]; // 0xC069

	///// YCSO added start 1900716
	#ifdef __TYPE__AAU__
	INT16S	aau_spectrum_DL_offest[MAX_DL];	///// 0xC06A, AAU Only , 파형감시 DL offset
	INT16S	aau_spectrum_UL_offest[MAX_UL];	///// 0xC06B, AAU Only , 파형감시 UL offset
	INT8U	BT_Device_Code[BT_DEVICE_NAME_SIZE];	/////사용 않하게 되었지만 일단 유지.
	INT8U	BT_Pairing_Code[BT_PAIRING_CODE_SIZE];
	#endif
	
	INT32U spectrum_offest_init; // 20190813
	INT16S spectrum_offest_x10[DL_UL];	// 0xC06D, 파형감시 DL/UL offset
	INT16S reserved_d_t_pattern_gain_x10[MAX_FA_PATH]; // not used.

	////////INT8U  AAU_LS_Gain_Bal_OnOff; // AAU Only, 0xC076, (0:OFF, 1:ON), 2019.08.23
	INT8U  LS_Gain_Bal_OnOff; // 0xC076, (0:OFF, 1:ON), 2020.02.03 : change from 'AAU_LS_Gain_Bal_OnOff'
	INT16S BW_Gain_Comp_PRU_x10[BW_Max][MAX_DL]; // PRU Only, 0xC077 ~ 0xC07A
	INT32S TSSI_Dec_Offset[MAX_FA_PATH][MAX_DL]; // 0xC06F, AAU Only

	INT16S d_t_pattern_gain_x10[MAX_FA_PATH][LSH_Max]; // AAU Only, 0xC074, 2019.09.09 : layer : 1 -> 3	
	
	INT16S d_CFR_Offset_x10[MAX_DL];       // AAU Only, 0xC07B, 2019.09.16
	INT16S d_DL_Det_SSB_Frame[LSH_Max]; // AAU Only, 0xC07C, 2019.09.16
	INT16S d_DL_Det_SSB_Start[LSH_Max]; // AAU Only, 0xC07D, 2019.09.16
	INT16S d_DL_Det_SSB_Lngth[LSH_Max]; // AAU Only, 0xC07E, 2019.09.16

	INT8U  gREMS_Ip[4]; // 2019.11.11
	INT8U  Layer_Ip[4]; // 2019.11.18 : LayerSplitter IP

	/*
	2019.11.18 : GUI ID/PASS 오류 건 보완: 	GUI ID/PASS 저장 영역을 새로 할당
	*/	
	INT8U User_ID[12];
	INT8U User_PW[12];	

	INT32U PRU_Def_Setting_20200123; // 0x20200123
	INT8U  AAU_CW_Auto_Onoff; // AAU Only, 2020.01.20 Added

	INT8U  AAU_Reset_Cnt_by_Low_Alarm; // 2020.02.24 Added
	INT32U AAU_Reset_by_Low_Alarm_flag; // 2020.02.24 Added
	INT32U AAU_LS_PLL_def_set_20200402; // 0x20200402
	
	///// YCSO added END
} EEPDATASTR;

//---------------------------------------

typedef struct __attribute__((packed))
{
	INT32U	Reset_Code; // warm (== Auto) reset 확인 : 다른 이유로 reset 여부 확인 위해.
	INT32U  UDA_Alarm__Code;
	INT32U  RST_Suppr__Code;

	INT32U time_backup_ms[UDA_MAX]; // rcu reset 으로 인한 이전의 suppress 경과시간 저장.
	INT8U  Rst_Suppress[UDA_MAX];
	INT8U  UDA_Rst_Cnt[UDA_MAX];

	INT8U  UDA_Alarm_f; // UDA Alarm backup by UDA
	INT8U  RST_Suppr_f; // Reset Suppress Flag	
	INT8U  reserve2[2];
	
	INT32U reserve32;
}UDA_STR;

//---------------------------------------

// below max 64byte must...
typedef struct __attribute__((packed))
{
	INT32U	reset_info; // rcu reset reason	
	UDA_STR UDA;        // UDA Reset
} DownInfoSTR;
//---------------------------------------





#define ACC_MAX_USER_NUM	100
#define ACC_MAX_USER_ID_LEN_MAX	16
#define ACC_MAX_USER_PASSWD_LEN_MAX	32

#define DL_FILENAME_MAX_SIZE 29 // (파일명：최대 24문자 영숫자, 확장자：최대 4문자 영어) 24+4+1(dot)
#define DL_FILENAME_MAX_SIZE_NULL 30 // NULL 포함



/*----------------------------------------------------------*/

#define EEP_WRITE_CODE	0x23456789
#define JIG_MODE_CODE	0xABCD0102
#define DUMMY_MODE_CODE	0x20140927
#define ALM_JIG_MODE_CODE	0xABCD3456

typedef struct
{
	INT8U Req;
	INT8U St;
	INT8U retry;
	INT8U reserve;
} DTU_RST;


enum TASK_NUM
{
	_TASK_WDT_				= 0,
	_TASK_UP_COMM_			,
	_TASK_UDP_SEL_			,
#ifdef __DEL_UDP_REMS__
#else
	_TASK_UDP_SEL_PROC_REMS_,   
#endif	
	_TASK_UDP_SEL_PROC_NODE_,
	_TASK_UDP_SEL_PROC___LS_, 
	_TASK_gREMS_COMM_	    , 
	_TASK_UDP_DTU_RX_		, 
	_TASK_AUTO_CONFIG_		,
	_TASK_FPGA_REGISTAER_	, 
#ifdef __TYPE__PRU__
	_TASK_PAU_COMM_AMP_1_	,
	_TASK_PAU_COMM_AMP_2_	, 
	_TASK_PAU_COMM_AMP_3_	,
	_TASK_PAU_COMM_AMP_4_	, 
	_TASK_PSU_COMM_			, 
#else
	_TASK_BLUETOOTH_COMM_	,
#endif
	_TASK_CPU_USAGE_		, 
	_TASK_SYSTEM_CALL_		, 
	_TASK_Rems_Spectrum_	,	////YCSO added 191212
	_TASK_SIM_				, 
	_TASK_LOG_              , 
	_TASK_zMODEM_           , 
//---------------------------
	_TASK_MAX_NUM_
};

/*
----------------------------------------------------------
for
m_Env.debug__cmd; // INT16U 
m_Env.debug_data[2][4]; // INT32S
----------------------------------------------------------
*/
enum
{
	dbg___None = 0,
	dbg___AAU_PLL_Test, // 0x0001 : AAU PLL Test, register test 
	
	dbg___MAX
};

enum
{
	dbg_ctrl = 0,
	dbg_data,	
	
	dbg_2MAX
};

enum
{
	dbg_en = 0,	
	dbg_d0,	
	dbg_d1,	
	dbg_d2,
	
	dbg_4MAX
};


/*----------------------------------------------------------*/
typedef struct  __attribute__((packed))
{
    INT32U is_eep_write;
    INT8U is_reboot; // system이 리셋되어야 함을 나타냄. 플래그가 1이되면 소프트웨어리셋된다.
    INT8U psu_reset_req;

	INT8U is_down_load;
	OS_TIMER dl_timer;
	INT32U dl_time_out;
	INT8U is_down_load_me;
	INT8U is_down_load_zip;
	
	INT8U Text_Debug_Mode;
	INT8U Comm_Debug_Mode;
	INT8U Comm_Debug_Port;
	INT8U Comm_Debug_Req;
	//-----------------------------------------------------	

	//-----------------------------------------------------	
	// 5G PRU, AAU
	//-----------------------------------------------------	
	
	INT8U	svr_ip[4];
	INT16U	svr_port;

	//-----------------------------------------------------		
	
	
	INT8U	lohost_ip[4];
	INT32U	lohost_ip_32;

	INT32U USB_Comm_Check_Count; // USB Port 단절을 Check 하기 위한 Count
	INT32U Task_Cnt[16]; // Task 도는지 확인 하기 위한 변수 디버그용
	INT32U UDP_Init[100];
	
	INT32U OSM_Timeout_ms; // is not timer variable.
	INT8U  OSM_Auto_Req; // 원격 OSM 절체/복구 요구

	INT8U Dig_Board_Alm_Mask;

	// BIP Debug
	INT32U bip_init_cnt;
	INT32U bip_eber_no_use_cnt;
	INT32U bip_cnt_en_is_0_cnt;

	INT8U Pwr_AMP_Rst_Req[AMPU_4_MAX];
	INT8U User_Pwr_AMP_Rst[AMPU_4_MAX];
	INT8U Pwr_AMP_Rst_Ack[AMPU_4_MAX];
	INT8U Pwr_AMP_Download_Req[AMPU_4_MAX];
	INT8U Test;
	INT8U gREMS_Test;

	//-------------------------------
	// EMS
	//-------------------------------
	INT8U AutoRep_Req[_EMS_AUTO_MAX_];
	

	INT8U CF_nC_Chk_REQ;

	STATIS_INFO STATIS;
	INT8U flag_1_time_1_day; // 2019.11.18
	DownMode DnMode;
	
	
	//-------------------------------
	
	DTU_RST DTU_Rst;

	INT8U  AMP_Dn_St[AMPU_4_MAX]; // 0 : init, 1:download now, 2:complete, 3:paused
	INT8U  PSU_Dn_St;

	INT8U Dig_Down_AMP_Off_Req;
	INT8U ALL_AMP_Standby_Exit_Req; // mode on->off, time off, exit-by-force
	OS_TIMER AMP_Standby_Exit_Timer;	

	// delay setting 에 대한 variables
	INT16U	FWD_FPGA_Dly_x100[AMPU_4_MAX];
	INT16U	RVS_FPGA_Dly_x100[AMPU_4_MAX];
	INT8U	FWD_FPGA_Dly_Req[AMPU_4_MAX];
	INT8U	RVS_FPGA_Dly_Req[AMPU_4_MAX];	

	INT8U	Use_Not_Req[AMPU_4_MAX];
	INT8U   Use_Not_Req2[AMPU_4_MAX]; // 2020.02.03


	INT8U  UDA_Reset_En; // 2019.04.27. Reset by UDA Alarm
	INT8U  Batt_Charge_OnOff; // last control status
	

	INT8U HardClip_Cnt_Clr_Req;
	INT8U LOF_Cnt_Clr_Req;	

	INT8U app_exit_req;
	INT8S dtu_restart_req;
	INT8U Download_syscall_req; // 2019.04.06, package

	// 2019.04.13 : FWD/RVS Shutdown
	INT8U Fwd_In_Over[AMPU_4_MAX];
	INT8U Rvs_In_Over[AMPU_4_MAX]; // AAU Only
	///// YCSO added 190415 
	INT8U Bluetooth_re_Init_Req;	//// AAU Only

	INT8U CPRI_Lock_St; // CPRI0 ~ 3 : bit0 ~ 3, if bit set, then locked. 

	INT8U wave_req;	///// added YCSO to 파형감시기. 
	INT8U wave_type;
	INT8U SIM_task_req;
	INT8U SIM_task_init_req;
	INT8U PSU_confirm_DL;
	INT8U PAU_confirm_DL[AMPU_4_MAX];

	INT8U debug_optic;
	INT8U debug_path4;

	INT16U debug__cmd;
	INT32S debug_data[dbg___MAX][dbg_2MAX][dbg_4MAX]; // [type][ctrl/status][4]

	INT8U test_ls_ip[4]; // test layersplitter ip

	INT8U task_flag[_TASK_MAX_NUM_];		///// YCSO TASK END
	INT8U task_END_status[_TASK_MAX_NUM_];
	int task_loop[_TASK_MAX_NUM_];
	int task_cnt[_TASK_MAX_NUM_];
	int task_cnt_prev[_TASK_MAX_NUM_];

	// 2019.07.10
	INT8U User_ID[12];
	INT8U User_PW[12];	
	INT8U gui_id_req;
	INT8U gui_pw_req;

	INT8U Ctrl_MAC[6];
	INT8U Ctrl_MAC_Req;
	INT16U Test_Rvs_Alc_Cnt;
	INT8U  AAU_Alarm_Fan_and_Temp; // 2019.08.01

	INT8U  Factory_Onoff; // 2019.08.06, 0xC0C0, ANTS-AAU Only, default OFF
	INT8U  IS_RTC_Init; // 2019.08.07
	INT32U rtc_refresh_cnt; // 2019.08.07
	
	/*
	* 2019.08.12 (ON:1, OFF:0), default ON
	* UL Gain Check 및 UL Noise Check 기능 주기적인 측정 Off
	*/
	INT8U  gREMS_Noti_Ready; // gREMS Alarm Report Allowed or not, AID 0x500D is '6/6' or '5/6 is over 10m'
	// 2019.09.24 : 10분 Timer 돌고, gREMS 보고 시작 하는 시점에서 "AMP On/Off 제어"가 가능하도록
	INT8U  AMP_Ctrl_Allowed; 
	INT8U  AMP_User_Ctrl[AMPU_4_MAX];
	INT8U  Pwr_Amp_Ctrl[AMPU_4_MAX];
	
	

	INT32U Delay_Adjust_ns; // PRU, processing delay offset
	INT32U Tsync_Offset_ns; // PRU, tsync offset 500ns	
	INT32U Delay_DL_Dpd_ns[MAX_DL]; // PRU, DL-DPD-Off delay, dpd_off(170), dpd_on(0)

	INT8U  AAU_alc_max_cnt; // AAU, 2019.08.31
	INT8U  AAU_gain_chk_level; // AAU, 2019.09.06

	INT8U  DPD_DCL_Rst_Req[AMPU_4_MAX]; // 2019.10.25, PRU only
	INT8U  DPD_Rst_Normal_Chk_Cnt[AMPU_4_MAX]; // 2019.10.25, PRU only

	INT8U  gREMS_Spectrum_req;			/// 2019.12.12 YCSO added


	INT8U AAU_UL_Over_Input_Clr_Req; // 2019.12.26
	// TEST
	INT8U AlmTime_Test;
	INT8U AlmTime_Test_Tmp;

	INT8U RU_Init_St_Reset;	// AID 0x500D : now complete and init is requested
	INT8U RU_Init_Cnt;
	INT8U CPRI_CLK_Alm_Mask_at_Reboot;

	INT8U Rvs_Spur_OFF; // 2020.01.28
	INT8U Rvs_Spur_Chk;
	INT32U watchdog_cnt;
} PGMEnvSTR;


typedef struct  __attribute__((packed))
{
	INT8U FAN_Auto[2];	  //  자동 제어 [case/dtu]
	INT8U FAN_OnOff[2];   //  
	INT8S FAN_On_Temp[2]; //  동작 온도
	INT8S FAN_OffTemp[2]; //  동작 온도
} CTRL_REQ;

//-------------------------------------------
// AID 0x0914
enum RESET_REASON
{
	Rst_None = 0,
	Rst_By_User,     // 0x01: 운용자 제어  [v]
	Rst_By_Download, // 0x02: F/W Download [v]
	Rst_By_PwrFail,  // 0x03: 전원이상     [v] ??
	Rst_By_Etc,      // 0x04: 기타         [v] ??
	Rst_By_Period,   // 0x05: 주기적
	Rst_By_PwrCtrl,  // 0x06: 전원제어     [v], PSU reset
	Rst_By_Vss         = 0x10,
	Rst_By_Uda_Pd_CPRI = 0x11,
	Rst_By_Uda_Link    = 0x12,
	Rst_By_Uda_Clock   = 0x13,
		
	Rst_MAX
};
//-------------------------------------------

#define Rst_By_WDT_RST Rst_By_Etc

/*----------------------------------------------------------*/
// 외부 호출 함수
//

DEF_EXT DownInfoSTR		m_rst_info;
DEF_EXT UDA_STR         m_Uda;

DEF_EXT EEPDATASTR  	m_Eep;
DEF_EXT PGMEnvSTR		m_Env;
DEF_EXT ST_STR			m_St;


typedef  struct
{	
	INT8S	id;		 // RO일때는 0~7, SHS/HUB는 MHS_BR와 같음.
} RO_DEV;

DEF_EXT RO_DEV  m_Ro;
DEF_EXT CTRL_REQ f_Req; // Ctrl Request Item
DEF_EXT CTRL_REQ m_Req; // Ctrl Request Item

///// YCSO added START 190503

#ifdef __MAKER_ANTS__
DEF_EXT Tbl_AMP_TEMP		m_AMP_tmp_tbl[8];
DEF_EXT Tbl_ATT				m_ATT_AMP_tbl[8];
DEF_EXT Tbl_DETEC_POWER		m_pwr_tbl[8];
DEF_EXT Tbl_AMP_TEMP		m_AMP_tmp_tbl_Alpha[8]; // 2019.06.28, AAU only

#endif
///// YCSO added END


/*----------------------------------------------------------*/
#ifdef MAIN_GLOBALS
/*----------------------------------------------------------*/
#ifdef __TYPE__PRU__
const unsigned char  *RCU_CODE_IMAGE = (unsigned char *)"H#16#A_5G_PRU190213#####CCCCVVVV";
const unsigned char  *DTU_CODE_IMAGE = (unsigned char *)"ANTS#PRU_DTU_190213#####CCCCVVVV";
const unsigned char  *FPG_CODE_IMAGE = (unsigned char *)"ANTS#PRU_FPGA190213#####CCCCVVVV";
#else // ----------- // ANTS-AAU
const unsigned char  *RCU_CODE_IMAGE = (unsigned char *)"H#16#A_5G_AAU190213#####CCCCVVVV";
const unsigned char  *DTU_CODE_IMAGE = (unsigned char *)"ANTS#AAU_FPGA190213#####CCCCVVVV"; // No DTU App
const unsigned char  *FPG_CODE_IMAGE = (unsigned char *)"ANTS#AAU_FPGA190213#####CCCCVVVV";
#endif

const unsigned char  *DPD_CODE_IMAGE = (unsigned char *)"ANTS#PRU_DPD_190213#####CCCCVVVV";

DEF_EXT INT32U JIG_MODE = 0;
DEF_EXT INT32U ALM_JIG_MODE = 0;
/*----------------------------------------------------------*/
#else
/*----------------------------------------------------------*/
DEF_EXT const unsigned char  *RCU_CODE_IMAGE;
DEF_EXT const unsigned char  *DTU_CODE_IMAGE;
DEF_EXT const unsigned char  *DPD_CODE_IMAGE;
DEF_EXT const unsigned char  *FPG_CODE_IMAGE;


DEF_EXT INT32U ALM_JIG_MODE;
DEF_EXT INT32U JIG_MODE;
/*----------------------------------------------------------*/
#endif
/*----------------------------------------------------------*/

DEF_EXT void Status_Lock( void );
DEF_EXT void Status_Unlock( void );


enum
{
	EXT_FAN =0, // CASE
	INT_FAN,    // PSU Interanl FAN
};

/*----------------------------------------------------------*/

/*----------------------------------------------------------*/




/********************************************************************************/
#endif
/********************************************************************************/




