/////////////////////////////
//	PROTOCOL.C
/////////////////////////////
#define PROTOCOL_GLOBALS
#include "includes.h"
#include "ver_info.h"
/////////////////////////////


 


/////////////////////////////
// Local Variables
/////////////////////////////

//--------------------------------------------------------------------------------------------------
//
// Legacy AID : 0x0000 ~ 0x4FFF
// NEW	 AID2 : 0x5000 ~ 0xFFFE
//
//--------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
const SKT_AID_STR skt_aid_list[] =\
{
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|	
	{ 0x00,0x01, ___GUI___|_Measure_|__Hidden_|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // alarm 	

	//-------------------------------------------------------------------------------------------------------------------------------------
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|
#ifdef _AID_DELETE_NEEDED__		
	{ 0x01,0x00, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // maker
	{ 0x01,0x01, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // supplyer
	{ 0x01,0x02, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // repeater type	
	{ 0x01,0x49, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // Intergartion Version (RCU)
#endif		
	//-------------------------------------------------------------------------------------------------------------------------------------

#ifdef _AID_DELETE_NEEDED__			
	// AID0,1 -|------------ -- Attribute  -------------|-- device --| 
	{ 0x02,0x00, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // door alarm
	{ 0x02,0x01, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // temperature
	{ 0x02,0x02, ___GUI___|____o____|__Alarm__|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // temperature upper limit
	{ 0x02,0x04, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // fan1 alarm	
	{ 0x02,0x05, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // fan2 alarm	
	{ 0x02,0x08, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // AC alarm
	{ 0x02,0x09, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // DC alarm
	{ 0x02,0x0A, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // Gui connection alarm
	{ 0x02,0x0B, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // batter low alarm
	{ 0x02,0x10, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // battery exist or not
	{ 0x02,0x49, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // OSM 설치 유무
	{ 0x02,0x7D, ___GUI___|____o____|__Alarm__|____o____, _PRU_|__o__, 0, 0, 0, 0, 0}, // battery status	
	//-------------------------------------------------------------------------------------------------------------------------------------
	{ 0x09,0x14, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // reset reason
	//-------------------------------------------------------------------------------------------------------------------------------------
#endif


	// AID0,1 -|------------ -- Attribute  -------------|-- device --|
#ifdef _AID_DELETE_NEEDED__	
	{ 0x2B,0x30, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, 0, 0, 0, 0, 0}, // RCU D/L State
#endif
	
	//-------------------------------------------------------------------------------------------------------------------------------------
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0x50,0x00, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // System ID
	{ 0x50,0x01, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 1}, // System Cascade ID, 2019.04.18 : __INT8U
	{ 0x50,0x02, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 4, 1}, // Layer 사용 정보
	{ 0x50,0x03, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 2}, // PCI
	{ 0x50,0x04, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 2}, // Cell ID
	{ 0x50,0x05, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // gNB ID
	{ 0x50,0x06, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 2}, // Port No
	
#ifdef _AID_DELETE_NEEDED__		
	{ 0x50,0x07, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // NG-ARFCN
#endif

	{ 0x50,0x08, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 2}, // Channel Bandwidth  
	{ 0x50,0x09, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 2}, // DUH-ID
	{ 0x50,0x0A, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // LS 제조사.
	{ 0x50,0x0B, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step___0001, 4, 1, 2}, // CenterFreq, 2019.05.30
	{ 0x50,0x0C, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 2}, // LS ID  
	{ 0x50,0x0D, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // RU Initial status 
	{ 0x50,0x0E, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use,10, 1, 1}, // Layer Splitter 등록 번호, 0xB007

	// 2019.06.09
	{ 0x50,0x12, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // temperature
	{ 0x50,0x13, ___GUI___|_CF_CTRL_|__Alarm__|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // temperature upper limit
	{ 0x50,0x1A, ___GUI___|_CF_nCTL_|__Alarm__|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // battery status	
	{ 0x50,0x1B, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // battery exist or not
	{ 0x50,0x1C, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // OSM 설치 유무

	// 2020.06.24	
	{ 0x50,0x21, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // IP (AID 0xB000)	
	{ 0x50,0x22, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 2}, // VSS(NOTI IP1)(0xB445) 정보
	{ 0x50,0x23, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 2}, // VSS(NOTI IP2)(0xB446) 정보		
	{ 0x50,0x24, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 온도(PSU)
	{ 0x50,0x25, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전압(PSU)(Main DC)
	{ 0x50,0x26, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전압(PSU)(Sub DC)
	//-------------------------------------------------------------------------------------------------------------------------------------
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0x51,0x00, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL TSSI
	{ 0x51,0x01, ___GUI___|_CF_CTRL_|__Alarm__|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL TSSI 상한
	{ 0x51,0x02, ___GUI___|_CF_CTRL_|__Alarm__|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL TSSI 하한
	{ 0x51,0x03, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // DL 출력
	{ 0x51,0x04, ___GUI___|_CF_CTRL_|_Measure_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // DL 출력 상한
	{ 0x51,0x05, ___GUI___|_CF_CTRL_|_Measure_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // DL 출력 하한	
	{ 0x51,0x06, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // DL ATTEN	
#ifdef _AID_DELETE_NEEDED__	
	{ 0x51,0x07, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // DL TSSI_Decimal	
#endif	
	{ 0x51,0x08, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL Output(Calculation)
	{ 0x51,0x10, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 0, 4, 1}, // Overpower(DL)
	{ 0x51,0x11, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 0, 4, 1}, // VSWR(DL)
	{ 0x51,0x12, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 0, 4, 1}, // Device Fail(Low Gain)(DL)
	{ 0x51,0x13, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 0, 4, 1}, // PAU High Temp(DL)
	{ 0x51,0x14, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 0, 4, 1}, // PAU DC(DL)
	{ 0x51,0x15, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 0, 4, 1}, // Shutdown(DL)	
	{ 0x51,0x16, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // PAU On/Off(DL), 2019.06.28 : CF/C
	{ 0x51,0x17, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 0, 4, 1}, // Disabled(DL)	
	{ 0x51,0x18, __Alarm__|____o____|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 0, 4, 1}, // PAU LINK (DL)
	{ 0x51,0x1A, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Retrun Loss(PAU)(DL)
	//-------------------------------------------------------------------------------------------------------------------------------------
	
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0x52,0x00, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL RSSI 
	{ 0x52,0x01, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL RSSI 상한
	{ 0x52,0x02, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL RSSI 하한
	{ 0x52,0x06, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // UL ATTEN, 2019.04.09 : 4x1 -> 4x2
	{ 0x52,0x07, ___GUI___|_Measure_|____o____|____o____, __o__|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // UL Gain
	{ 0x52,0x08, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // ATTEN(UL DTU Gain backoff)
	{ 0x52,0x09, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // UL Gain Balance
	{ 0x52,0x10, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 0, 4, 1}, // 과입력(UL)
	{ 0x52,0x12, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 0, 4, 1}, // LNA(UL)	
	{ 0x52,0x16, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // LNA ON / OFF (UL)
	//-------------------------------------------------------------------------------------------------------------------------------------

	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0x53,0x00, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 0, 1, 1}, // CPRI
	{ 0x53,0x01, ___GUI___|_Measure_|__Alarm__|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 2, 2}, // CPRI
	
	/*
	- LOF(CPRI) 0x5303 0x00: 정상/ 0x01: 이상
	*/
	
	{ 0x53,0x05, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // LD Power(CPRI)
	{ 0x53,0x08, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // PD Power(CPRI)
	{ 0x53,0x09, ___GUI___|_CF_CTRL_|__Alarm__|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // PD Power 하한	
	{ 0x53,0x0A, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 2, 2}, // BIP Count(CPRI)
	{ 0x53,0x0B, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 1, 1, 1}, // BIP Error 누적 Count 초기화 
	{ 0x53,0x0C, ___GUI___|_CF_CTRL_|__Alarm__|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // PD Power 상한
	{ 0x53,0x10, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // SFP Vcc(CPRI)
	{ 0x53,0x11, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // LD Bias(CPRI)
	{ 0x53,0x12, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step___0001, 4, 2, 2}, // Wave-Length(CPRI), 2019.04.25
	{ 0x53,0x13, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 2, 2}, // BitRate(CPRI)
	{ 0x53,0x14, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 2}, // SFP온도(CPRI)	 
	{ 0x53,0x15, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 2, 2}, // SFP Insert Status
	{ 0x53,0x16, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use,16, 2, 2}, // SFP Vendor PN
	{ 0x53,0x20, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 1}, // Hard Clipping Cnt, AID change 2019.04.02
	{ 0x53,0x21, ___GUI___|_Measure_|__Alarm__|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 4, 1}, // DPD(DL)
	//-------------------------------------------------------------------------------------------------------------------------------------

	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0x54,0x00, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // Toffset, 2019.05.09
	{ 0x54,0x01, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // TBdelayDL
	{ 0x54,0x02, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // TBdelayUL
	{ 0x54,0x03, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // FPGA Delay(DL), 2019.05.30
	{ 0x54,0x04, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // FPGA Delay(UL), 2019.05.30
	{ 0x54,0x05, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // Delay(CPRI) (Round Trip), 2019.08.21
	{ 0x54,0x08, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 9, 1, 2}, // L/S 수신 DL/UL Delay Verification
	{ 0x54,0x09, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // Optic Delay (one-way)
	
	////// YCSO added 190417 0x54 0x0B
	{ 0x54,0x0B, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 1}, // Scale Factor(UL)
	{ 0x54,0x10, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // FAN Mode, AAU/PRU
	{ 0x54,0x11, ___GUI___|_Measure_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // FAN Mode 동작 상태, AAU/PRU
	{ 0x54,0x12, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // FAN Mode 동작 시간, AAU/PRU
	{ 0x54,0x20, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // TDD Mode		
	{ 0x54,0x21, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // DL ON Guard Time
	{ 0x54,0x22, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // DL OFF Guard Time
	{ 0x54,0x23, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // DL to UL Guard Time
	{ 0x54,0x24, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // UL to DL Guard Time
	{ 0x54,0x30, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // MIMO Standby Mode (On/Off)
	{ 0x54,0x31, ___GUI___|_Measure_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // MIMO Standby Mode 동작 상태
	{ 0x54,0x32, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // MIMO Standby Mode 동작 시간
	{ 0x54,0x33, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // Layer 별 운용 Mode
	{ 0x54,0x34, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // AMP Standby Mode (On/Off)
	{ 0x54,0x35, ___GUI___|_Measure_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // AMP Standby Mode 동작 상태
	{ 0x54,0x36, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // AMP Standby Mode 동작 시간
	{ 0x54,0x37, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // AMP Standby Mode 적용 Path, 
	{ 0x54,0x38, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 1, 1}, // Battery Extension Mode (On/Off)	
	{ 0x54,0x39, ___GUI___|_Measure_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 1, 1}, // Battery Extension Mode 동작 상태 
	{ 0x54,0x3A, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // MIMO Standby Mode(진입 Level)
	{ 0x54,0x3B, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // MIMO Standby Mode(clear Level)
	{ 0x54,0x3F, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // Test Port 출력	
	{ 0x54,0x40, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 0, 0, 0}, // UDA_불요파	
	{ 0x54,0x43, __Alarm__|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 0, 0, 0}, // UDA_PD(CPRI)
	{ 0x54,0x44, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // UDA_불요파_동작 Mode
	{ 0x54,0x46, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // UDA_Clock_동작 Mode
	{ 0x54,0x47, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // UDA_PD(CPRI)_동작 Mode
	{ 0x54,0x4C, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // UDA_불요파_동작 Layer
	{ 0x54,0x50, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // UL Gain 자동 backoff
	{ 0x54,0x51, ___GUI___|_CF_CTRL_|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // UL Gain 감시
	//-------------------------------------------------------------------------------------------------------------------------------------
	{ 0x54,0x53, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // logging On/Off
	{ 0x54,0x54, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 1}, // log count
	{ 0x54,0x55, ___GUI___|_CF_CTRL_|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // AAU LED Auto OFF
	
	//-------------------------------------------------------------------------------------------------------------------------------------
	{ 0x55,0x00, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // maker
	{ 0x55,0x01, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // supplyer
	{ 0x55,0x02, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // repeater type	
	{ 0x55,0x03, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 3, 1, 1}, // Intergartion Version (RCU), Main

	{ 0x56,0x00, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // 통계보고 기준시
	{ 0x56,0x01, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // 통계보고 주기
	{ 0x56,0x02, ___GUI___|_Measure_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 6, 1, 1}, // 통계 시작 시간
	{ 0x56,0x03, ___GUI___|_CF_CTRL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // 자동보고 기준시
	{ 0x56,0x10, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // RCU D/L State
	{ 0x56,0x20, ___GUI___|_Operate_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // reset reason

	
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|

	// 2020.07.01 다시 사용. 0xB000
    { 0xB0,0x00, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // IP 
	{ 0xB0,0x01, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 6, 1, 1}, // MAC
	{ 0xB0,0x02, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 1}, // CID
	{ 0xB0,0x03, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // System Time(time tick)
	{ 0xB0,0x04, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // Base IP 
	{ 0xB0,0x05, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 6, 1, 1}, // Base MAC
	{ 0xB0,0x06, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // Time tick Offset
	{ 0xB0,0x07, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use,10, 1, 1}, // S/N
	{ 0xB0,0x08, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use,16, 1, 1}, // Board Name
////{ 0xB0,0x09, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // RU Initial status
	{ 0xB0,0x0A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // Alarm Acknowledge
	{ 0xB0,0x0B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // Alarm Valid indicator
#ifdef __TYPE__AAU__	
	{ 0xB0,0x0C, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, __ASCII, Step_No_Use, 30, 1, 1}, // Bluetooth Device ID
	{ 0xB0,0x0D, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, __ASCII, Step_No_Use, 16, 1, 1}, // Bluetooth Pairing Code
	{ 0xB0,0x10, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // AAU Type
#endif	
	{ 0xB0,0x11, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use, 100, 1, 1}, // 국소명

	{ 0xB0,0x12, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use, 11, 1, 1}, // gREMS GUI ID
	{ 0xB0,0x13, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use, 11, 1, 1}, // gREMS GUI PW
	{ 0xB0,0x14, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use,  1, 1, 1}, // gREMS Alarm Mask
	{ 0xB0,0x15, ___GUI___|_CF_nCTL_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use,  3, 1, 1}, // Intergartion Version (RCU), hidden

	//-------------------------------------------------------------------------------------------------------------------------------------

	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xB1,0x00, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // TSSI(Decimal)
////{0x5107, ___GUI___|_Measure_|_STatist_|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // DL TSSI_Decimal	
	//-------------------------------------------------------------------------------------------------------------------------------------
	
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xB2,0x00, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // 온도(PAU)(DL)
	{ 0xB2,0x01, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 4, 1}, // 온도 보상 On/Off(PAU)
	{ 0xB2,0x02, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 3, 4, 1}, // S/W 버전(PAU)(DL)
	{ 0xB2,0x03, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 4, 1}, // 제조사(PAU)(DL)
	{ 0xB2,0x04, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, __INT8U, Step_No_Use, 1, 4, 1}, // 용량(PAU)(DL)
	{ 0xB2,0x05, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 4, 4, 1}, // S/N(PAU)(DL)
	{ 0xB2,0x06, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // 전압(PAU)(DL)
	{ 0xB2,0x07, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step____001, 2, 4, 1}, // VSWR(PAU)(DL)	
	{ 0xB2,0x08, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Retrun Loss(PAU)(DL)
	{ 0xB2,0x09, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Return Loss Threshold(DL)
	{ 0xB2,0x0A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Return Loss Revcover(DL)
	{ 0xB2,0x0B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // High Temp Threshold(DL)
	{ 0xB2,0x0C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // High Temp Recovery(DL)
	{ 0xB2,0x0D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Over Power Shutdown Level(DL), AAU added. 2019.04.22
	{ 0xB2,0x0E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 4, 1}, // PAU Link 이상(DL)
	{ 0xB2,0x10, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Offset(DL 출력)
	{ 0xB2,0x11, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Offset(DL 입력)
	{ 0xB2,0x12, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Offset(UL 출력)
	{ 0xB2,0x13, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Offset(UL 입력)
	{ 0xB2,0x14, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // Offset(FB 출력)
	{ 0xB2,0x15, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 4, 1}, // Offset(Delay DL)
	{ 0xB2,0x16, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 4, 1}, // Offset(Delay UL)
	{ 0xB2,0x1A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // FB 출력(DL)
	{ 0xB2,0x1B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // LNA 출력(UL)
	{ 0xB2,0x1C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // LNA 입력(UL)
	{ 0xB2,0x1D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // AMP 입력(DL)
	{ 0xB2,0x20, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // ATTEN(PAU)(DL), 2019.05.09 : AAU Added.
	{ 0xB2,0x21, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step_____01, 2, 4, 1}, // ATTEN(PAU FB)(DL)
	{ 0xB2,0x22, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(PAU 온도보상)(DL), 2019.05.09 : AAU Added.
	{ 0xB2,0x23, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(LNA 온도보상)(UL), 2019.05.09 : AAU Added.
	{ 0xB2,0x24, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // ATTEN(LNA)(UL)
	{ 0xB2,0x25, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // ATTEN(LNA ALC)(UL), 2019.05.09 : AAU Added.
	{ 0xB2,0x26, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(PAU FB 온도보상)(DL)
	{ 0xB2,0x27, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(PAU Gain 보상)(DL)
	{ 0xB2,0x28, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(PAU FB Gain 보상)(DL)
	{ 0xB2,0x29, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(LNA Gain 보상)(UL)
	{ 0xB2,0x2A, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // ATTEN(ALC)(DL)
	{ 0xB2,0x30, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // ALC On/Off(UL)
	{ 0xB2,0x31, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 4, 1}, // ATTEN Fail On/Off(DL)
	{ 0xB2,0x32, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // Auto Shutdown Mode On/Off(UL)
	{ 0xB2,0x33, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // Gain Balance On/Off(UL)
	{ 0xB2,0x34, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // ALC Level(UL)
	{ 0xB2,0x35, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Shutdown Level(UL)
	{ 0xB2,0x36, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Gain Balance value(UL)
	{ 0xB2,0x37, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // LNA On/Off(UL)
	{ 0xB2,0x3A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 2, 4, 1}, // D/L State(PAU)(FW)(DL) 
#ifdef __TYPE__AAU__ 	
	{ 0xB2,0x3B, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // FWD ALC onoff, 2019.05.30, AAU
	{ 0xB2,0x3C, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // FWD SD onoff,  2019.05.30, AAU
#endif
	//-------------------------------------------------------------------------------------------------------------------------------------


	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xB3,0x00, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // RSSI(Decimal)

	{ 0xB3,0x02, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // RSSI(Filter 전)
	{ 0xB3,0x03, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // RSSI(Filter 후)
	{ 0xB3,0x04, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // UL DTU Gain 보상(타사 과입력)
	{ 0xB3,0x05, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // Gain 저하(UL) Threshold 
	{ 0xB3,0x06, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // RSSI(UL Noise)
	{ 0xB3,0x07, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // Offset(RSSI)
	{ 0xB3,0x08, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // Threshold(UL 불요파)
	{ 0xB3,0x09, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // Offset(Noise 전)	
	//-------------------------------------------------------------------------------------------------------------------------------------
	
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xB4,0x00, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 온도(DTU)
	{ 0xB4,0x01, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 온도(DTU FPGA)
	{ 0xB4,0x02, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 3, 1, 1}, // S/W 버전(DTU)
	{ 0xB4,0x03, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 3, 1, 1}, // S/W 버전(FPGA)
	{ 0xB4,0x04, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 4, 1}, // DPD On/Off(DL)
	{ 0xB4,0x05, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 2, 4, 1}, // DPD Status(DL)
	{ 0xB4,0x06, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // DPD Input Power(DL)
	{ 0xB4,0x07, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // DPD Output Power(DL)
	{ 0xB4,0x08, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // DPD Engine Output Power(DL)
	{ 0xB4,0x09, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // DPD F/B Input Power(DL)
	{ 0xB4,0x0A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // On/Off(DL), 2019.04.15. PRU & AAU
	{ 0xB4,0x0B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // PRU CFR Level(DL) / AAU PAR
	{ 0xB4,0x0C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // Hard Clipping On/Off(DL)
	{ 0xB4,0x0D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Hard Clipping Level(DL)
/*	
// 2020.06.24 삭제. 
////{ 0xB4,0x0E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 1}, // Hard Clipping Count(DL)
*/	
	{ 0xB4,0x10, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Offset(DL 출력)

	// 0xB412, 2019.0420. deleted
	
	///// YCSO added AID 0xB4 0x13
	{ 0xB4,0x13, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // DTU F/B input power(DL);    
	{ 0xB4,0x15, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // Offset(DTU Delay DL)
	{ 0xB4,0x16, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // Offset(DTU Delay UL)
	{ 0xB4,0x17, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // Offset(온도), EMS 보고 온도에 대한 Offset 용도
	{ 0xB4,0x19, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // CPRI Recovery Path Status
	{ 0xB4,0x1A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // Clock Status
	{ 0xB4,0x1B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 1, 2}, // Function Fail(VSS)
	{ 0xB4,0x1C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 2}, // Delay 알람(Layer)
	{ 0xB4,0x1D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // Delay 설정값
	{ 0xB4,0x1E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // RE Channel Bandwidth  
	///// YCSO reInit AID 0xB4 0x1F
	{ 0xB4,0x1F, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 2}, // ARFCN 
	{ 0xB4,0x20, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // ATTEN(Digital DL)
	{ 0xB4,0x21, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // ATTEN(Digital UL)
	{ 0xB4,0x22, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // ATTEN(Digital DL 온도보상)
	{ 0xB4,0x23, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // ATTEN(Digital UL 온도보상)
	///// YCSO added AID 0xB4 0x24, 0x2E, 0x2F
	{ 0xB4,0x24, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // ATTEN(Digital DL)(SUM)
	{ 0xB4,0x26, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_____01, 2, 4, 2}, // ATTEN(UL DTU Gain backoff)
/*	
// 2020.06.24 삭제. 
////{ 0xB4,0x27, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 2, 2}, // BIP Error 누적 Count(CPRI)
*/
	{ 0xB4,0x29, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // Delay 설정값(FPGA)(DL)
	{ 0xB4,0x2A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // Delay 설정값(FPGA)(UL)	
	{ 0xB4,0x2B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // Toffset,   2019.05.17
	{ 0xB4,0x2C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // TBdelayDL, 2019.05.17
	{ 0xB4,0x2D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 1, 2}, // TBdelayUL, 2019.05.17	
	{ 0xB4,0x2E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 2}, // Path On / OFF DTU (DL)
	{ 0xB4,0x2F, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 2}, // Path On / OFF DTU (UL)	
	{ 0xB4,0x30, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 1}, // Test Pattern On/Off(DL)
	{ 0xB4,0x31, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 4, 2}, // Payload On/Off, 2019.04.19
	{ 0xB4,0x32, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 2, 2}, // SFP On/Off
	{ 0xB4,0x33, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // Debug On/Off(L2 SW)
	{ 0xB4,0x34, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // 온도 보상 On/Off(DTU)
	{ 0xB4,0x35, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // Rx/Cascade Add On/Off(UL)
	{ 0xB4,0x36, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // Loopback On/Off
	{ 0xB4,0x37, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 전압(DTU)
	{ 0xB4,0x38, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // 전압(DTU)(Sub), 2019.04.15
	{ 0xB4,0x3A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // D/L State(DTU)(FW)
	{ 0xB4,0x3B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // D/L State(DTU)(FPGA)
	{ 0xB4,0x3C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 사용율(CPU)
	{ 0xB4,0x3D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 사용율(Memory)
	{ 0xB4,0x3E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // Link (L2 SW) 
	{ 0xB4,0x3F, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 2, 2}, // SFP Insert Status
	//vss
	{ 0xB4,0x40, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 2, 2}, // CPRI Status
	{ 0xB4,0x41, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 2, 2}, // CPRI Version(Z.2.0)
	{ 0xB4,0x42, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 2, 2}, // CPRI startup(Z.66.0)
	{ 0xB4,0x43, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 2, 2}, // CPRI L1-reset-LOS(Z.130.0)
	{ 0xB4,0x44, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 2, 2}, // Pointer P(Z.194.0)
/*
////{ 0xB4,0x45, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 2}, // VSS(NOTI IP1), 2020.06.24 삭제.
////{ 0xB4,0x46, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 2}, // VSS(NOTI IP2), 2020.06.24 삭제.
*/
	{ 0xB4,0x47, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use,20, 1, 2}, // VSS(TDD Slot Number)

	// VSS
	{ 0xB4,0x50, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.16.0) REC/RE SERDES LOS 
	{ 0xB4,0x51, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.80.0) REC/RE LOF Alarm 
	{ 0xB4,0x52, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.144.0) REC Type ID/RE Func Fail
	{ 0xB4,0x53, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.208.0) C&M Path Selection
	{ 0xB4,0x54, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.17.0) RE Reset/RE Reset Ack
	{ 0xB4,0x55, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.81.0)(RE Channel BW/ALIVE ACK)
	{ 0xB4,0x56, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.85.0)(Open CPRI Version)
	{ 0xB4,0x57, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.149.0)(CPRI Compression On/Off)
	{ 0xB4,0x58, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.213.0)(RE Cascade ID)
	{ 0xB4,0x59, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.22.0)(Optic LOS Detect/Alarm)
	{ 0xB4,0x5A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.86.0)(Loopback mode config/Ack)
	{ 0xB4,0x5B, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.23.0)(RE1 Reset/ACK)
	{ 0xB4,0x5C, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.87.0)(RE2 Reset/ACK)
	{ 0xB4,0x5D, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.151.0)(RE3 Reset/ACK)
	{ 0xB4,0x5E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.215.0)(RE4 Reset/ACK)
	{ 0xB4,0x5F, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.24.0)(Reserv/RE1 Func Fail(Cascade)
	{ 0xB4,0x60, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.88.0)(RE3 Reset/ACK)
	{ 0xB4,0x61, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.152.0)(RE4 Reset/ACK)
	{ 0xB4,0x62, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.216.0)(Reserv/RE1 Func Fail(Cascade)
	{ 0xB4,0x64, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.30.0) (Reserved/RE Vendor ID)
	{ 0xB4,0x65, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 4, 2}, // VSS(Z.60.0) (REC Port No)
	//end vss

	{ 0xB4,0x6E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // VSS Monitoring On/Off 
	{ 0xB4,0x6F, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use,255,1, 1}, // VSS(Receive)
	{ 0xB4,0x70, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use,255,1, 1}, // VSS(Send)
	//-------------------------------------------------------------------------------------------------------------------------------------

	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xB6,0x00, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 온도(PSU)
	{ 0xB6,0x02, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 3, 1, 1}, // S/W 버전(PSU)
	{ 0xB6,0x03, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전압(PSU)(AC)
	{ 0xB6,0x04, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전압(PSU)(Main DC)
	{ 0xB6,0x05, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전류(PSU)(Main DC)
	{ 0xB6,0x06, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전압(PSU)(Sub DC)
	{ 0xB6,0x07, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // 전압(PSU)(Battery)
	
	{ 0xB6,0x08, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 전압(DTU) 48V DC Fail 설정값
	{ 0xB6,0x09, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 전압(DTU) Main 전원 DC Fail 설정값
	{ 0xB6,0x0A, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 전압(DC 48V) 상한
	{ 0xB6,0x0B, ___GUI___|__Hidden_|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 전압(DC 48V) 하한
	{ 0xB6,0x0E, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 1, 1}, // PSU Link이상
	{ 0xB6,0x10, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // FAN(외장) ON 온도
	{ 0xB6,0x11, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // FAN(외장) OFF 온도
	{ 0xB6,0x12, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_No_Use, 2, 1, 1}, // FAN(외장) RPM
	{ 0xB6,0x13, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // FAN(외장) 자동제어
	{ 0xB6,0x14, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // FAN(PSU) ON 온도
	{ 0xB6,0x15, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 1, 1}, // FAN(PSU) OFF 온도
	{ 0xB6,0x16, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_No_Use, 2, 1, 1}, // FAN(PSU) RPM
	{ 0xB6,0x17, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___BOOL, Step_No_Use, 1, 1, 1}, // FAN(PSU) 자동제어	
	{ 0xB6,0x18, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16S, Step_No_Use, 2, 1, 1}, // FAN(Ext) Max RPM(LowNoise Mode)
	{ 0xB6,0x20, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // Watt_Tot_Accu
	{ 0xB6,0x21, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 2, 1, 1}, // Watt_real_time
	{ 0xB6,0x22, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 2, 1, 1}, // Watt_hour_aver
	{ 0xB6,0x23, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 2, 1, 1}, // Watt_day_aver 
	{ 0xB6,0x24, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 2, 1, 1}, // Watt_week_aver
	{ 0xB6,0x25, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _INT16U, Step_No_Use, 2, 1, 1}, // Watt_month 
	{ 0xB6,0x30, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // PAU SW Status
	{ 0xB6,0x31, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // FAN Status(함체)
	{ 0xB6,0x32, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // PSU Status(AC/DC)
	{ 0xB6,0x33, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // PSU Status(Temp/DC)
	{ 0xB6,0x34, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 1, 1}, // PSU Status(ETC)
	{ 0xB6,0x39, ____o____|____o____|____o____|____o____, _PRU_|__o__, ___None, Step_No_Use, 1, 1, 1}, // Reset(PSU)
	{ 0xB6,0x3A, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 2, 1, 1}, // D/L State(PSU)(FW) 
	//-------------------------------------------------------------------------------------------------------------------------------------
	// 0xC0__
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xC0,0x00, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // DTU App/MB(microbraze) status
	{ 0xC0,0x01, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // FPGA stauts
	{ 0xC0,0x02, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // DTU Mode	
	{ 0xC0,0x03, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // RE Reset	
	{ 0xC0,0x04, _Operate_|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // RE Reset Clear
	{ 0xC0,0x10, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 2, 1}, // NCO
	{ 0xC0,0x11, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 2, 1}, // NCO VALID
	{ 0xC0,0x12, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 4, 2}, // DL_CW_LEVEL
	{ 0xC0,0x13, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 4, 2}, // DL_CW_ON
	{ 0xC0,0x14, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_No_Use, 2, 4, 2}, // DL_Equalizer_Coeff
	{ 0xC0,0x15, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL Gain
	{ 0xC0,0x16, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL Gain
	{ 0xC0,0x19, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_No_Use, 2, 4, 2}, // UL_Equalizer_Coeff
#ifdef __TYPE__AAU__	
	{ 0xC0,0x1A, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_No_Use, 2, 4, 2}, // DL_Equalizer_Coeff_1
	{ 0xC0,0x1B, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_No_Use, 2, 4, 2}, // UL_Equalizer_Coeff_1
#endif
	{ 0xC0,0x1C, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_____01, 1, 4, 2}, // DL Input Power Balance Att
	{ 0xC0,0x1D, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // TSSI Shutdown Level(Short)
	{ 0xC0,0x1E, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // TSSI Shutdown Level(long)
	{ 0xC0,0x1F, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // TSSI Shutdown Level(Low)
	{ 0xC0,0x20, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 5, 1}, // UL Gain Guard time[0~4]
	{ 0xC0,0x21, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // T SYNC Delay		
	{ 0xC0,0x22, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // t SYNC FAIL
#ifdef __TYPE__PRU__	
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // Capture_DL_Time
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // Capture_UL_Time
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // Capture_ULNoise_Time
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // ULNoise_RSSI_StartTime
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // ULNoise_RSSI_StopTime
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // UL_RSSI_StartTime
	{ 0xC0,0x23, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_No_Use, 4, 1, 1}, // UL_RSSI_StopTime
#endif
	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xC0,0x30, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 1}, // CFR Version, 2019.04.15 : PRU&AAU
	{ 0xC0,0x31, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 3, 1, 1}, // DPD Version
	{ 0xC0,0x32, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32S, Step_____01, 4, 4, 1}, // DPD Error Status
	{ 0xC0,0x33, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_____01, 4, 4, 1}, // DPD Count
	{ 0xC0,0x34, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT32U, Step_____01, 4, 4, 1}, // DPD Engine Input Power
	{ 0xC0,0x35, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 4, 1}, // CFR output gain
	{ 0xC0,0x36, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_000001, 1, 4, 1}, // DL ATT (RF 0.25 step)
	{ 0xC0,0x37, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_000001, 1, 4, 1}, // UL ATT (RF 0.25 step)
	{ 0xC0,0x40, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 2}, // CPRI Status
	{ 0xC0,0x41, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 2, 1, 2}, // CPRI Miscellaneous Status
	{ 0xC0,0x42, _Operate_|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 2}, // CPRI Ethernet FIFO Reset
////{ 0xC0,0x43, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 4}, // BIP Value, 맞음.
	{ 0xC0,0x43, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // BIP Value, GUI 때문에 변경.
////{ 0xC0,0x44, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 1}, // BIP Value Debug, deleted..
	{ 0xC0,0x45, _Operate_|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 2}, // BIP Clear
	{ 0xC0,0x46, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 2}, // BIP Error Count
	{ 0xC0,0x47, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 4, 1, 1}, // Ethernet Path Sel, Carrier 2 -> 1	
	{ 0xC0,0x48, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // (DTU) 48V DC Fail 설정값, 2019.04.22.
	{ 0xC0,0x49, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // (DTU) Main 전원 DC Fail 설정값, 2019.04.22.
	{ 0xC0,0x4A, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // AFE ALC Level (UL)
	{ 0xC0,0x4B, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, ___None, Step_No_Use, 1, 4, 1}, // FWD ALC onoff, 2019.05.13, AAU
	{ 0xC0,0x4C, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // ALC Level(DL)
	{ 0xC0,0x4D, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // Shutdown Level(DL)
	{ 0xC0,0x4E, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // System Offset(DTU Delay DL)
	{ 0xC0,0x4F, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // System Offset(DTU Delay UL)
	{ 0xC0,0x50, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, ___None, Step_No_Use, 1, 4, 1}, // FWD SD onoff, 2019.05.13, AAU
	{ 0xC0,0x51, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // Offset(DL출력 FB)
	{ 0xC0,0x52, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // Offset(DL TSSI)
	{ 0xC0,0x53, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // GC_RSSI_RMS
	{ 0xC0,0x54, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // Gain Check Mode
	{ 0xC0,0x55, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 4, 1, 1}, // test L/S ip
	{ 0xC0,0x56, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 1, 1}, // debug optic
	{ 0xC0,0x57, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 1, 1}, // debug path
	{ 0xC0,0x58, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // threshold ul gain low
	{ 0xC0,0x59, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // dl mode
	{ 0xC0,0x5A, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // LNA 입력(UL)
	{ 0xC0,0x5B, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 2}, // Offset(FILTER 전)
	{ 0xC0,0x5C, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // Offset(Noise 전)	
	{ 0xC0,0x5D, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 1}, // GC_RSSI_RMS Offset
	{ 0xC0,0x5E, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 1, 1}, // 48V In Voltage Offset
	
////2019.08.12 : AID 0xB60A/0B 의 중복으로 0xC05F "48V DC Fail limit" 삭제.	
////{ 0xC0,0x5F, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 2, 1}, // 48V DC Fail limit

	{ 0xC0,0x60, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // AAU UL Noise Mode
	{ 0xC0,0x61, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // ANTS_LED_Status

	{ 0xC0,0x62, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL LSH 연동 Gain(SS)
	{ 0xC0,0x63, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL LSH 연동 Gain(ER)
	{ 0xC0,0x64, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // DL LSH 연동 Gain(NK)

	{ 0xC0,0x65, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL LSH 연동 Gain(SS)
	{ 0xC0,0x66, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL LSH 연동 Gain(ER)
	{ 0xC0,0x67, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // UL LSH 연동 Gain(NK)

	{ 0xC0,0x68, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // Test Pattern Band Gain
	{ 0xC0,0x69, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT32S, Step_____01, 4, 4, 1}, // CFR_Level_x10, 4byte, AAU Only
	////// YCSO added 190716
	#ifdef __TYPE__AAU__
	{ 0xC0,0x6A, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // 파형감시 DL Inband offset
	{ 0xC0,0x6B, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 4, 2}, // 파형감시 UL Inband offset
	#endif
	{ 0xC0,0x6C, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // Height Balnance On/Off
	{ 0xC0,0x6D, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16S, Step_____01, 2, 2, 1}, // 파형감시 DL/UL Offset
	{ 0xC0,0x6E, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _PrdDef, Step_No_Use, 1, 4, 1}, // PAU EQ State
	{ 0xC0,0x6F, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT32S, Step_No_Use, 4, 4, 2}, // TSSI Decimal Offset
	{ 0xC0,0x70, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // AAU_CW_Auto_Onoff	

	// AID0,1 -|------------ -- Attribute  -------------|-- device --|-- Type--|--- Step --| S| L| C|
	{ 0xC0,0x71, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 2, 2}, // Delay 제어 수신 값
	{ 0xC0,0x72, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 1, 1, 1}, // Hidden Version
	{ 0xC0,0x73, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // TSSI EQ전 Decimal
	{ 0xC0,0x74, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_____01, 2, 3, 2}, // test pattern gain
	{ 0xC0,0x75, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32S, Step_No_Use, 4, 1, 1}, // Delay 제어 수신 return 값
	{ 0xC0,0x76, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // LS Gain Balance On/Off

	{ 0xC0,0x77, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // BW Gain Comp.(140MHz)
	{ 0xC0,0x78, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // BW Gain Comp.(160MHz)
	{ 0xC0,0x79, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // BW Gain Comp.(180MHz)
	{ 0xC0,0x7A, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, _INT16S, Step_____01, 2, 4, 1}, // BW Gain Comp.(200MHz)
	{ 0xC0,0x7B, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_No_Use, 2, 4, 1}, // CFR Offset
	{ 0xC0,0x7C, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_No_Use, 2, 3, 1}, // DL Detector SSB Frame
	{ 0xC0,0x7D, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_No_Use, 2, 3, 1}, // DL Detector SSB Start
	{ 0xC0,0x7E, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16S, Step_No_Use, 2, 3, 1}, // DL Detector SSB Length
	{ 0xC0,0x7F, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, _INT16U, Step_____01, 2, 4, 1}, // ATTEN(LNA)(UL)

	{ 0xC0,0x80, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, __ASCII, Step_No_Use, 4, 1, 1}, // PSU Maker Mode, 2020.02.14
	{ 0xC0,0x81, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, __INT8U, Step_No_Use, 1, 1, 1}, // PSU Compatiblity, 2020.02.14

	{ 0xC0,0x90, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, __INT8U, Step_No_Use, 4, 1, 1}, // Samsung PLL 설정, 2020.04.02
	{ 0xC0,0x91, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, __INT8U, Step_No_Use, 4, 1, 1}, // Nokia   PLL 설정, 2020.04.02
	{ 0xC0,0x92, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, __INT8U, Step_No_Use, 4, 1, 1}, // Ericson PLL 설정, 2020.04.02

	// 2020.06.09
	{ 0xC0,0xA0, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, __INT8U, Step_No_Use, 4, 1, 1}, // ping IP
	{ 0xC0,0xA1, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // ping Packet Count
	{ 0xC0,0xA2, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // ping Packet size
	{ 0xC0,0xA3, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _PrdDef, Step_No_Use, 1, 1, 1}, // ping onoff
	{ 0xC0,0xA4, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, __ASCII, Step_No_Use,255,1, 1}, // ping result
	{ 0xC0,0xA5, ___GUI___|____o____|____o____|____o____, _PRU_|__o__, __ASCII, Step_No_Use,20, 1, 1}, // DDR-Memory Info

	{ 0xC0,0xC0, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___BOOL, Step_No_Use, 1, 1, 1}, // Factory Mode On/Off

	{ 0xC0,0xF0, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // debug command
	{ 0xC0,0xF1, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT32U, Step_No_Use, 4, 4, 2}, // debug data

	{ 0xC0,0xFE, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 4, 2}, // 
	{ 0xC0,0xFF, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 4, 2}, // 
	
	//// YCSO added 190728 START
	#ifdef __TYPE__AAU__
	{ 0xCF,0x01, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, __INT8U, Step_No_Use, 4, 1, 1}, // kst_inventory_SN
	{ 0xCF,0x02, ___GUI___|____o____|____o____|____o____, __o__|_AAU_, __INT8U, Step_No_Use, 3, 1, 1}, // kst_inventory_date
	#endif
	
	
	
	
	{ 0xD4,0x54, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, _INT16U, Step_No_Use, 2, 1, 1}, // Log File count, 2020.02.11.
	//-------------------------------------------------------------------------------------------------------------------------------------
	{ 0xF2,0xF1, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 1, 1}, // Alarm TEST, 2019.04.22.
	{ 0xF2,0xF3, ___GUI___|____o____|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 1, 4, 1}, // other RVS ALC onoff, 2019.05.06

	//------------------------------------------
	{ 0xFF,0xFF, 0, 0, 0, 0, 0, 0, 0} // END
	//------------------------------------------	
	/*
	//		, ___GUI___|__Hidden_|____o____|____o____, _PRU_|_AAU_, ___None, Step_No_Use, 0, 0, 0}, 
	//		, ___GUI___|__Hidden_|____o____|____o____, _PRU_|__o__, ___None, Step_No_Use, 0, 0, 0}, 
	//
	//	-----------------------------------
	//		___None = 0x00, // 미사용					Step_No_Use = 0x00, // 미사용	
	//		__INT8S = 0x01, // signed char				Step_____01 = 0x01, // 0.1	   
	//		__INT8U = 0x02, // unsigned char			Step____001 = 0x02, // 0.01 	
	//		_INT16S = 0x03, // signed int				Step___0001 = 0x03, // 0.001	
	//		_INT16U = 0x04, // unsigned int 			Step_____10 = 0x11, // 10		
	//		_INT32S = 0x05, // signed long int			Step____100 = 0x12, // 100		
	//		_INT32U = 0x06, // unsigend log int 		Step___1000 = 0x13, // 1000 	
	//		___BOOL = 0x07, // boolean								
	//		__ASCII = 0x08, // ASCII								
	//		_PrdDef = 0x09, // pre-defined									
	//	-----------------------------------
	*/
};
// END of USE_EMS
//=====================================================================================================


//=====================================================================================================
const EMS_AID_Func ems_aid_func[] =
{
	//--------------------------------------------
	{Make_AID_0x00__},
	{Make_AID_0x01__},
	{Make_AID_0x02__},
	{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},
	{Make_AID_0x09__},
	{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},
	//--------------------------------------------
	{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},
	{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},
	//--------------------------------------------
	{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},{NULL},
	{NULL},{NULL},{NULL},
	{Make_AID_0x2B__},
	{NULL},{NULL},{NULL},{NULL},
	//--------------------------------------------
	// 0x30 ~ 0x3F
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0x40 ~ 0x4F
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0x50 ~ 0x5F
	{Make_AID_0x50__},
	{Make_AID_0x51__},
	{Make_AID_0x52__},
	{Make_AID_0x53__},
	{Make_AID_0x54__},
	{Make_AID_0x55__},	
	{Make_AID_0x56__},	
	{NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0x60 ~ 0x6F
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0x70 ~ 0x7F
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0x80 ~ 0x8F
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0x90 ~ 0x9F
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},	
	//--------------------------------------------
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},	
	//--------------------------------------------
	// 0xB0 ~ 0xBF
	{Make_AID_Internal_B0}, 
	{Make_AID_Internal_B1}, 
	{Make_AID_Internal_B2}, 
	{Make_AID_Internal_B3}, 
	{Make_AID_Internal_B4}, 
	{Make_AID_Internal_B5}, 
	{Make_AID_Internal_B6},	{NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0xC0 ~ 0xCF
	{Make_AID_Internal_C0}, 
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, 
	{Make_AID_Internal_CF},		//// YCSO added 190728
	//--------------------------------------------
	// 0xD0 ~ 0xDF
	{NULL}, {NULL}, {NULL}, {NULL}, {Make_AID_Internal_D4}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0xE0 ~ 0xEE
	{NULL},	{NULL},	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	//--------------------------------------------
	// 0xF0 ~ 0xFF
	{NULL}, {NULL}, {Make_AID_Internal_F2}, 
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL},
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}
	//--------------------------------------------	
};


//---------------------------------------------------------------------------------------------
void Conv_uVal16_to_big_endian(INT16U uVal16, INT8U* pData )
{	
	pData[0] = uVal16 >> 8;
	pData[1] = uVal16 & 0xFF;
}
//---------------------------------------------------------------------------------------------
void Conv_uVal32_to_big_endian(INT32U uVal32, INT8U* pData )
{	
	pData[0] = (INT8U)((uVal32>>24)&0xFF);
	pData[1] = (INT8U)((uVal32>>16)&0xFF);
	pData[2] = (INT8U)((uVal32>>8) &0xFF);
	pData[3] = (INT8U)((uVal32>>0) &0xFF);
}


//---------------------------------------------------------------------------------------------
// AID 0x00__
INT16U Make_AID_0x00__( void* pAID, INT16U idx, INT8U port, INT8U arg  )
{	
	AIDSTR* paid = (AIDSTR*)pAID;
	(void)arg;	(void)port;
	//ALM_ST AC_Fail_Bk_Alm; // backup alarm when AMP AC Fail

	switch( paid->AID[1] )
	{
		case 0x01:
			paid->len = RU_ALARM_CNT*2;	
			memcpy( &paid->data[0], RU_ARM_SUPP, RU_ALARM_CNT ); // needed or not ??			
			memcpy( &paid->data[RU_ALARM_CNT], &m_St.Last_Alm, RU_ALARM_CNT );

		#if 0 /// #ifdef __TYPE__PRU__
			if(m_St.AlmSt.A_0.AC)
			{
				memcpy(&AC_Fail_Bk_Alm, &m_St.Last_Alm, RU_ALARM_CNT);
				Alarm_Mask__Caused_by_AC_Fail(&AC_Fail_Bk_Alm);
				memcpy(&paid->data[RU_ALARM_CNT], &AC_Fail_Bk_Alm, RU_ALARM_CNT);
			}
		#endif			
			break;

		default: return 0; // 항목 없는 경우 return 0
	}
	return 1;
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x01__( void* pAID, INT16U idx, INT8U port, INT8U arg )
{
	AIDSTR* paid = (AIDSTR*)pAID;	
	(void)arg;	(void)port;	

	#ifdef _AID_DELETE_NEEDED__
	switch( paid->AID[1] )
	{
		case 0x00: paid->data[0] = m_St.Maker[0]; break;
		case 0x01: paid->data[0] = m_St.Maker[1]; break;

		case 0x02: paid->len = 2; 
		#ifdef __TYPE__PRU__
			paid->data[0] = m_St.Type[0]; 
			paid->data[1] = Get_Repeater_Type();
		#else
			paid->data[0] = m_St.Type[0]; 
			paid->data[1] = m_St.Type[1]; 
		#endif
			break;
			
		case 0x49:
			paid->len = 3; 
			memcpy ( paid->data, m_St.RCU_Ver_Main, 3);
			break;			

		default: return 0; // 항목 없는 경우 return 0			
	}
	return 1;
	#else
	return 0;
	#endif
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x02__( void* pAID, INT16U idx, INT8U port, INT8U arg )
{	
	AIDSTR* paid = (AIDSTR*)pAID;
	(void)arg;	(void)port;
	
	#ifdef _AID_DELETE_NEEDED__
	switch( paid->AID[1] )
	{		
		case 0x01: paid->data[0] = m_St.Temp; break;
		case 0x02: paid->data[0] = m_St.TempLmt; break;		
		case 0x10: paid->data[0] = m_St.Batt_Insert; break; //// 2019.04.25.batt.		
		case 0x49: paid->data[0] = m_St.OSM_Use; break;
		case 0x7D: paid->data[0] = m_St.Batt_Sts; break;
		
		default: return 0; // 항목 없는 경우 return 0			
	}
	return 1;
	#else
	return 0;
	#endif
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x09__( void* pAID, INT16U idx, INT8U port, INT8U arg )
{	
	AIDSTR* paid = (AIDSTR*)pAID;
	(void)arg;	(void)port;

	switch( paid->AID[1] )
	{		
		case 0x14: paid->data[0] = m_St.ResetReason_log; break;
		
		default: return 0; // 항목 없는 경우 return 0			
	}
	return 1;
}

//---------------------------------------------------------------------------------------------





//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x2B__( void* pAID, INT16U idx, INT8U port, INT8U arg ) // iro
{
	AIDSTR* paid = (AIDSTR*)pAID;	
	(void)arg;	(void)port;

	#ifdef _AID_DELETE_NEEDED__
	switch( paid->AID[1] )
	{
		case 0x30:
			paid->len = 2;
			paid->data[0] = m_St.DL_State[DN_RCU][0];
			paid->data[1] = m_St.DL_State[DN_RCU][1];
			break;			
			
		default: return 0; // 항목 없는 경우 return 0
	}	
	return 1;
	#else
	return 0;
	#endif
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
INT8U AID2_Chk_Size(INT8U size)
{
	return size; // max 255
}
INT8U AID2_Chk_Layer(INT8U layer)
{
	if(layer == 0xFF) return 1;
	else return layer;
}
INT8U AID2_Chk_Carrier(INT8U carrier)
{
	if(carrier == 0xFF) return 1;
	else return carrier;
}
//---------------------------------------------------------------------------------------------
/*
	Make_AID2_STR_Header
	----------------------------------------------
	- 'idx' is skt_aid_list[idx]
	- make AID2.type ~ AID2.carrier
	- calculate flag size
*/

#define MAX_AID2_Flag_Size (8)
void Make_AID2_STR_Header(AIDSTR2* paid2, INT16U idx, INT8U* pflag_len)
{	
	INT16U data_len;
	INT8U  flag_len;
	INT8U  layer;
	INT8U  carrier;
	INT8U  size;

	paid2->type     = skt_aid_list[idx].type;
	paid2->step     = skt_aid_list[idx].step;	
	paid2->size     = AID2_Chk_Size(skt_aid_list[idx].size);
	paid2->layer    = AID2_Chk_Layer(skt_aid_list[idx].layer);
	paid2->carrier  = AID2_Chk_Carrier(skt_aid_list[idx].carrier);
	paid2->reserved = 0;	
	//-------------------------------
	layer    = paid2->layer;
	carrier  = paid2->carrier;
	size     = paid2->size;
	//-------------------------------
	data_len = size * layer * carrier; // data only
	//-------------------------------
	flag_len = (layer * carrier)/8;	
	if((flag_len*8) < (layer * carrier)) flag_len+=1;
	
	if(flag_len > MAX_AID2_Flag_Size) 
		flag_len = MAX_AID2_Flag_Size; // max limit	

	*pflag_len = flag_len;
	//-------------------------------
	paid2->len = htons(data_len + AID2__Type_Carrier_Size + flag_len);

	// for display, debygging
#if 0
	if((paid2->AID[0] == 0xB4)&&(paid2->AID[1] == 0x03))
	{
		D_Printf(DEBUG_SYSTEM, "[Make_AID_Header] AID 0x%02X%02X, Len(%d) = %d + flag(%d) + data(%d)\n", 
			paid2->AID[0], 
			paid2->AID[1], 		
			ntohs(paid2->len),
			AID2__Type_Carrier_Size,
			flag_len, 
			data_len); 
			
		D_Printf(DEBUG_SYSTEM, "[Make_AID_Header]            data(%d) = size(%d) x layer(%d) x carrier(%d).\n\n", 
			data_len, 
			size, 
			layer,
			carrier); 	
	}
#endif	
}


void Make_AID2_STR_Header_Var_Size_2(AIDSTR2* paid2, INT8U var_size)
{	
	INT16U data_len;
	INT8U  flag_len;
	INT8U  layer_x_carrier;
		
	paid2->size = var_size;
	//-------------------------------
	data_len = paid2->size * paid2->layer * paid2->carrier; // data only
	//-------------------------------
	layer_x_carrier = paid2->layer * paid2->carrier;
	flag_len = layer_x_carrier/8;	
	if((flag_len*8) < layer_x_carrier) flag_len+=1;
	
	if(flag_len > MAX_AID2_Flag_Size) 
		flag_len = MAX_AID2_Flag_Size; // max limit	
	
	//-------------------------------
	paid2->len = htons(data_len + AID2__Type_Carrier_Size + flag_len);
}


/*
2019.09.19
for Variable size like AUD 0xB011 : BTS Name, max size is 100
*/
void Make_AID2_STR_Header_Var_Size(AIDSTR2* paid2, INT16U idx, INT8U* pflag_len, INT8U var_size)
{	
	INT16U data_len;
	INT8U  flag_len;
	INT8U  layer;
	INT8U  carrier;
	INT8U  size;

	paid2->type     = skt_aid_list[idx].type;
	paid2->step     = skt_aid_list[idx].step;	
	paid2->size     = var_size; // 2019.09.16 : variable size
	paid2->layer    = AID2_Chk_Layer(skt_aid_list[idx].layer);
	paid2->carrier  = AID2_Chk_Carrier(skt_aid_list[idx].carrier);
	paid2->reserved = 0;	
	//-------------------------------
	layer    = paid2->layer;
	carrier  = paid2->carrier;
	size     = paid2->size;
	//-------------------------------
	data_len = size * layer * carrier; // data only
	//-------------------------------
	flag_len = (layer * carrier)/8;	
	if((flag_len*8) < (layer * carrier)) flag_len+=1;
	
	if(flag_len > MAX_AID2_Flag_Size) 
		flag_len = MAX_AID2_Flag_Size; // max limit	

	*pflag_len = flag_len;
	//-------------------------------
	paid2->len = htons(data_len + AID2__Type_Carrier_Size + flag_len);

	// for display, debygging
#if 0
	if((paid2->AID[0] == 0xB4)&&(paid2->AID[1] == 0x03))
	{
		D_Printf(DEBUG_SYSTEM, "[Make_AID_Header] AID 0x%02X%02X, Len(%d) = %d + flag(%d) + data(%d)\n", 
			paid2->AID[0], 
			paid2->AID[1], 		
			ntohs(paid2->len),
			AID2__Type_Carrier_Size,
			flag_len, 
			data_len); 
			
		D_Printf(DEBUG_SYSTEM, "[Make_AID_Header]            data(%d) = size(%d) x layer(%d) x carrier(%d).\n\n", 
			data_len, 
			size, 
			layer,
			carrier); 	
	}
#endif	
}

//---------------------------------------------------------------------------------
INT16U Make_AID_0x50__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	
	INT8U* pVal8;
	INT16U* pVal16;
	INT32U* pVal32;
	
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;

	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8  = &paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{	
		case 0x00:
			flag[0] = 0x80;
			*pVal16 = htons((INT16U)AufoConfigFlag.RU_Id);			
			break;

		case 0x01:		
			flag[0] = 0x80;			
			pVal8[0] = AufoConfigFlag.RE_Cascade_ID;
			break;

		case 0x02:		
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.LayerUseInfo, MAX_DL);		
			break;

		case 0x03:
			flag[0] = 0xC0;
			*pVal16++ = htons(m_St.d_PCI[CPRI_0]);
			*pVal16++ = htons(m_St.d_PCI[CPRI_1]); 
			break;

		case 0x04:
			flag[0] = 0xC0;
			*pVal8++ = m_St.d_Cell_ID[CPRI_0];
			*pVal8++ = m_St.d_Cell_ID[CPRI_1]; 
			break;

		case 0x05:
			flag[0] = 0xC0;
			memcpy(&pVal8[0], m_St.d_gNB_ID[CPRI_0], 4);
			memcpy(&pVal8[4], m_St.d_gNB_ID[CPRI_1], 4);
			break;	

		case 0x06:
			flag[0] = 0xC0;
			*pVal8++ = m_St.g_LS_Port_No[CPRI_0]; 
			*pVal8++ = m_St.g_LS_Port_No[CPRI_1]; 
			break;

		#ifdef _AID_DELETE_NEEDED__	
		case 0x07:
			flag[0] = 0xC0;
			*pVal32++ = htonl(m_St.g_NG_ARFCN[_100M]); 
			*pVal32++ = htonl(m_St.g_NG_ARFCN[Alpha]); 
			break;
		#endif	

		case 0x08:
			flag[0] = 0xC0;		
			*pVal8++ = m_Eep.d_RE_Channel_BW[_100M];
			*pVal8++ = m_Eep.d_RE_Channel_BW[Alpha];
			break;

		// 2020.01.28 : DUH-ID(AID 0x5009) 추가.
		case 0x09:
			flag[0] = 0xC0;		
			*pVal8++ = m_St.d_DUH_ID[CPRI_0];
			*pVal8++ = m_St.d_DUH_ID[CPRI_1];
			break;

		case 0x0A: // 2019.04.16
			flag[0] = 0x80;
			*pVal8 = m_St.LS_Maker;
			break;

		// 2019.04.22
		case 0x0B: 
			flag[0] = 0xC0;	
			*pVal32++ = htonl(m_Eep.d_CenterFreq[0]);
			*pVal32++ = htonl(m_Eep.d_CenterFreq[1]);
			break;

		case 0x0C: // 0x500C, 2019.05.17
			flag[0] = 0xC0; 	
			*pVal8++ = m_St.LS_ID[_100M]; 
			*pVal8++ = m_St.LS_ID[Alpha]; 
			break;

		// RU Initial status 		
		case 0x0D: flag[0] = 0x80; *pVal8 = m_St.RU_Init_Status; break;
		
		case 0x0E: // Layer Splitter 등록 번호, 2020.02.03
			flag[0] = 0x80;
			memcpy(pVal8, m_St.LS_Regist_Num, SN_SIZE);
			break;			

		case 0x12: flag[0] = 0x80; 
			*pVal16 = htons(m_St.Temp_x10);			
		//#ifdef __AAU_WATCHDOG_TIME_TEST__
			//*pVal16 = htons(watchdog_elapsed());
		//#endif			
			break;
		
		case 0x13: flag[0] = 0x80; *pVal16 = htons(m_Eep.TempLmt_x10); break;
		case 0x1A: flag[0] = 0x80; *pVal8 = m_St.Batt_Sts; break;	
		case 0x1B: flag[0] = 0x80; *pVal8 = m_St.Batt_Insert; break;
		case 0x1C: flag[0] = 0x80; *pVal8 = m_St.OSM_Use; break;

		case 0x21: // 2020.06.24
			flag[0] = 0x80;
			memcpy(pVal8, m_St.Operate_Ip, 4);
			break;

		case 0x22: // 2020.06.24 : VSS (NOTI IP1)
			flag[0] = 0xC0;
			memcpy(&pVal8[0], m_St.d_VSS_Noti_IP_1[CPRI_0], 4);
			memcpy(&pVal8[4], m_St.d_VSS_Noti_IP_1[CPRI_1], 4);
			break;

		case 0x23: // 2020.06.24 : VSS (NOTI IP2)
			flag[0] = 0xC0;
			memcpy(&pVal8[0], m_St.d_VSS_Noti_IP_2[CPRI_0], 4);
			memcpy(&pVal8[4], m_St.d_VSS_Noti_IP_2[CPRI_1], 4);
			break;

		case 0x24: // 2020.06.24
			flag[0] = 0x80;					
			*pVal16 = htons((INT16U)m_PSU.Temp_x10);
			break;		
			
		case 0x25: // 2020.06.24
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.DC_Out_Volt_x10[0]);
			break;

		case 0x26: // 2020.06.24
			flag[0] = 0x80;		
			*pVal16 = htons((INT16U)m_PSU.DC_Out_Volt_x10[1]);
			break;	

		default: return 0; // 항목 없는 경우 return 0
	}

	memcpy(paid2->data, flag, flag_len);
	
	return 1;
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x51__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

	INT8U*  pVal8;
	INT16U* pVal16;
	INT32U* pVal32;

	#ifdef __TYPE__AAU__
	INT8U  uVal8;
	INT16U uVal16;
	#endif
	
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	INT8U c, l;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8  = &paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		case 0x00:
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons((INT16U)m_St.d_DL_TSSI_dBm_x10[c][l]); 
			}
			break;

		case 0x01:
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
				{					
					*pVal16++ = htons((INT16U)m_Eep.DL_TSSI_UP_x10[c][l]); 
					// D_Printf(DEBUG_SYSTEM, "[AID_Sts] m_St.DL_TSSI_UP_x10[%d][%d] = %d \n", 
					//	c, l, m_Eep.DL_TSSI_UP_x10[c][l]);
				}
			}
			break;
			
		case 0x02:
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons((INT16U)m_Eep.DL_TSSI_DN_x10[c][l]); 
			}
			break;
			
		case 0x03: // DL 출력
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.Fwd_Out_Pwr_x10[l]); 
			break;

		case 0x04: // DL out upper limit
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_Eep.Fwd_Out_Lmt_x10[l][HI]); 
			break;

		case 0x05: // DL out lower limit
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_Eep.Fwd_Out_Lmt_x10[l][LO]); 
			break;

		case 0x06: // DL ATTEN, 0.01dB step.	
			flag[0] = 0xF0;			
			#ifdef __TYPE__PRU__  // ANTS, HFR				
				for(l = 0; l < AMPU_4_MAX; l++)
					*pVal16++ = htons(conv_att_05_to_01(m_St.Fwd_Out_Att_x2[l])); 					
			#endif
			// AID 0x5106
			#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU
				for(l = 0; l < AMPU_4_MAX; l++) 
				{				
				/*
				---------------------------------
				2019.11.28 : 
					0xB220, 0x5206 : 같은 기능,
					m_St.DL_Att -> m_Eep.DL_Att_User, 변경. User ATT만 상태로 응답되도록 
				---------------------------------	
				*/ 
					uVal8 = conv_att_025_to_05(m_Eep.DL_Att_User[l]);
					uVal16 = conv_att_05_to_01(uVal8);
					*pVal16++ = htons(uVal16);
				}
			#endif		
			break;	
			
	#ifdef _AID_DELETE_NEEDED__
		case 0x07: // DL TSSI_Decimal
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal32++ = htonl(m_St.d_DL_TSSI_dec[c][l]); // Deleted
			}
			break;	
	#endif		

		case 0x08: // DL Output (Calculation)
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons((INT16U)m_St.DL_OutPwr_Cal_x10[c][l]); 
			}
			break;		
		
		case 0x16: // AID 0x5116 
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.Pwr_Amp_En, AMPU_4_MAX);
			break;

		case 0x1A:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)(m_St.Ret_Loss_x10[l]));
			break;		
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x52__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

	//#ifdef __TYPE__AAU__
	//INT8U  uVal8;
	//INT16U uVal16;
	//#endif
	
	INT16U* pVal16;	
	INT8U*  pVal8;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	INT8U c, l; // carrier, layer
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);
	
	pVal8  = &paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];
	//pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		case 0x00:
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
				{
					*pVal16++ = htons((INT16U)m_St.d_UL_RSSI_dBm_x10[c][l]); 
				}
			}
			break;
			
		case 0x01:
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
				{						
					*pVal16++ = htons((INT16U)m_Eep.UL_RSSI_UP_x10[c][l]); 
				}
			}
			break;

		case 0x02:
			flag[0] = 0xFF;								
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons((INT16U)m_Eep.UL_RSSI_DN_x10[c][l]); 
			}
			break;		
			
		case 0x06: // UL ATTEN
			flag[0] = 0xFF;				
		#ifdef __TYPE__PRU__
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.d_UL_Att_x10[FA_0][l]);
			for(l = 0; l < AMPU_4_MAX; l++) // for alpha
				*pVal16++ = htons(m_St.d_UL_Att_x10[FA_1][l]);
		#else			
			/*
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.d_UL_Att_x10[FA_0][l]);
			for(l = 0; l < AMPU_4_MAX; l++) // for alpha
				*pVal16++ = htons(m_St.d_UL_Att_x10[FA_1][l]);
			*/
			// 2019.11.28
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_Eep.d_Att_Dig_UL_User_x10[FA_0][l]);
			for(l = 0; l < AMPU_4_MAX; l++) // for alpha
				*pVal16++ = htons(m_Eep.d_Att_Dig_UL_User_x10[FA_1][l]);
			#endif			
			break;

		case 0x07: // AAU Only, 2019.05.24
			flag[0] = 0xF0;			
			for(l = 0; l < MAX_UL; l++)
				*pVal16++ = htons(m_St.UL_Gain_x10[l]);
			break;	

		case 0x08: // 2019.05.09
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for( l = 0; l < MAX_DL; l++)
					*pVal16++ = htons(m_St.d_UL_DTU_Gain_Backoff_GUI_x10[c][l]); // 2020.01.08 
			}
			break;

		case 0x09: // UL Gain Balance
			flag[0] = 0xFF;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.d_UL_GainBal_x10);
				
			for(l = 0; l < AMPU_4_MAX; l++) // for alpha
				*pVal16++ = htons(m_St.d_UL_GainBal_x10);
			break;	

		case 0x16:	// UL LNA ON OFF
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.RVS_Amp_Onoff, AMPU_4_MAX);
			break;

		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_0x53__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

	INT32U* pVal32;
	INT16U* pVal16;
	INT8U*  pVal8;

	INT8U   l;	
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
		
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	//----------------------------------------
	pVal8  = &paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];	
	pVal32 = (INT32U*)&paid2->data[flag_len];
	//----------------------------------------
	switch(paid2->AID[1])
	{	
		case 0x01: // CPRI
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_CPRI_Alarm[CPRI_0]; // carrier 100+A
			*pVal8++ = m_St.d_CPRI_Alarm[CPRI_2]; // carrier 100+A
			*pVal8++ = m_St.d_CPRI_Alarm[CPRI_1]; // carrier A
			*pVal8++ = m_St.d_CPRI_Alarm[CPRI_3]; // carrier A
		break;
	
		case 0x05: // LD Power(CPRI)
			flag[0] = 0xF0;			
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Pwr_x10[CPRI_0]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Pwr_x10[CPRI_2]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Pwr_x10[CPRI_1]); // carrier A
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Pwr_x10[CPRI_3]); // carrier A
		break;

		case 0x08: // PD Power(CPRI)
			flag[0] = 0xF0;
			*pVal16++ = htons((INT16U)m_St.d_SFP_PD_Pwr_x10[CPRI_0]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_PD_Pwr_x10[CPRI_2]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_PD_Pwr_x10[CPRI_1]); // carrier A
			*pVal16++ = htons((INT16U)m_St.d_SFP_PD_Pwr_x10[CPRI_3]); // carrier A
			break;

		case 0x09: // PD Power 하한
		case 0x0C: // PD Power 상한
			/*
			sequence : DL-CPRI0, UL-CPRI1, DL-CPRI1, UL-CPRI3
			*/			
			if(paid2->AID[1] == 0x09) l = _DN_;
			else l = _UP_;
			flag[0] = 0xF0;
			*pVal16++ = htons(m_Eep.PD_Pwr_Lmt_x10[l][CPRI_0]); // carrier 100+A
			*pVal16++ = htons(m_Eep.PD_Pwr_Lmt_x10[l][CPRI_2]); // carrier 100+A
			*pVal16++ = htons(m_Eep.PD_Pwr_Lmt_x10[l][CPRI_1]); // carrier A
			*pVal16++ = htons(m_Eep.PD_Pwr_Lmt_x10[l][CPRI_3]); // carrier A
			break;	

		/*		
			- LOS(CPRI) 0x5302 0x00: 정상/ 0x01: 이상
			- LOF(CPRI) 0x5303 0x00: 정상/ 0x01: 이상
			- LD(CPRI)  0x5304 0x00: 정상/ 0x01: 이상
			
			from ANTS
				MainAID "CPRI, 0x53 0x01, Mea Alm)에 LOS LOF LD PD 포함되어 있습니다. 
				무시하셔도 됩니다.


			AID flag sequence			
			-----------------------------
			 - LD Power(CPRI)
			 - PD Power(CPRI)
			 - BIP Count(CPRI)
			 - SFP Vcc(CPRI)
			 - LD Bias(CPRI)
			 - 파장(CPRI)
			 - BitRate(CPRI)
			 - SFP온도(CPRI)
			-----------------------------
			CPRI_0 : Main----100
			CPRI_2 : Main----100-Cascade
			CPRI_1 : Alpha---alpah
			CPRI_3 : Alpha---alpah-Cascade
		*/	

		case 0x0A: // BIP Err Count(CPRI)
			/*
			sequence : DL-CPRI0, UL-CPRI1, DL-CPRI1, UL-CPRI3
			*/						
			/*
			// 2020.06.24 : 순시값(d_BIP_Count) -> 누적값(d_BIP_ErrCnt) 0xB427 을 0x530A에 사용.
			// *pVal32++ = htonl(m_St.d_BIP_Count[DL][CPRI_0]); // carrier 100+A
			// *pVal32++ = htonl(m_St.d_BIP_Count[UL][CPRI_2]); // carrier 100+A
			// *pVal32++ = htonl(m_St.d_BIP_Count[DL][CPRI_1]); // carrier A
			// *pVal32++ = htonl(m_St.d_BIP_Count[UL][CPRI_3]); // carrier A
			*/
			// 2020.06.24 : 순시값 -> 누적값0xB427 을 0x530A에 사용.
			flag[0] = 0xF0;	
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[DL][CPRI_0]); // carrier 100+A
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[UL][CPRI_2]); // carrier 100+A
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[DL][CPRI_1]); // carrier A
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[UL][CPRI_3]); // carrier A
			break;


		case 0x10: // SFP Vcc(CPRI)
			flag[0] = 0xF0;			
			*pVal16++ = htons((INT16U)m_St.d_SFP_Vcc_x10[CPRI_0]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_Vcc_x10[CPRI_2]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_Vcc_x10[CPRI_1]); // carrier A
			*pVal16++ = htons((INT16U)m_St.d_SFP_Vcc_x10[CPRI_3]); // carrier A
			break;
			
		case 0x11: // LD Bias(CPRI)
			flag[0] = 0xF0;			
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Bias_x10[CPRI_0]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Bias_x10[CPRI_2]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Bias_x10[CPRI_1]); // carrier A
			*pVal16++ = htons((INT16U)m_St.d_SFP_LD_Bias_x10[CPRI_3]); // carrier A
			break;	
			
		case 0x12: // 파장(CPRI)
			flag[0] = 0xF0;			
			// 2019.04.25. AID is Changed.
			*pVal32++ = htonl(m_St.d_SFP_WaveLen_nm[CPRI_0]); // carrier 100+A
			*pVal32++ = htonl(m_St.d_SFP_WaveLen_nm[CPRI_2]); // carrier 100+A
			*pVal32++ = htonl(m_St.d_SFP_WaveLen_nm[CPRI_1]); // carrier A
			*pVal32++ = htonl(m_St.d_SFP_WaveLen_nm[CPRI_3]); // carrier A
			break;	

		case 0x13: // BitRate(CPRI)
			flag[0] = 0xF0;			
			*pVal16++ = htons((INT16U)m_St.d_SFP_BitRate[CPRI_0]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_BitRate[CPRI_2]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_BitRate[CPRI_1]); // carrier A
			*pVal16++ = htons((INT16U)m_St.d_SFP_BitRate[CPRI_3]); // carrier A
			break;	
			
		case 0x14: // SFP온도(CPRI)  	
			flag[0] = 0xF0;			
			*pVal16++ = htons((INT16U)m_St.d_SFP_Temp_x10[CPRI_0]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_Temp_x10[CPRI_2]); // carrier 100+A
			*pVal16++ = htons((INT16U)m_St.d_SFP_Temp_x10[CPRI_1]); // carrier A
			*pVal16++ = htons((INT16U)m_St.d_SFP_Temp_x10[CPRI_3]); // carrier A
			break;	

		case 0x15: // 2019.05.09
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_SFP_Insert_St[_100M][0];
			*pVal8++ = m_St.d_SFP_Insert_St[_100M][1];
			*pVal8++ = m_St.d_SFP_Insert_St[Alpha][0];
			*pVal8++ = m_St.d_SFP_Insert_St[Alpha][1];
			break;

		case 0x16: // 2020.07.22
			flag[0] = 0xF0;
			memcpy( &pVal8[0] , m_St.d_SFP_Vendor___PN[CPRI_0], 16);
			memcpy( &pVal8[16], m_St.d_SFP_Vendor___PN[CPRI_2], 16);
			memcpy( &pVal8[32], m_St.d_SFP_Vendor___PN[CPRI_1], 16);
			memcpy( &pVal8[48], m_St.d_SFP_Vendor___PN[CPRI_3], 16);
			break;	

		case 0x20: // 2019.05.09 : Hard Clipping Count Accumlation -> Hard Clipping Count(DL) 
			flag[0] = 0xF0;			
			for(l = 0; l < MAX_DL; l++) 
			{
				/*
				// 2020.06.24 : 
				0x5320에 순시값 (d_Hard_Clip_Cnt_DL) 대신,
				0xB40E Accumulate value(d_Hard_Clip_Cnt_Accum)를 사용.
				*pVal32++ = htonl(m_St.d_Hard_Clip_Cnt_DL[l]);
				*/				
				*pVal32++ = htonl((INT32U)m_St.d_Hard_Clip_Cnt_Accum[l]); // 2020.06.24
			}
			break;
			
		case 0x21: // DPD(DL)	
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.DPD_Alarm_St, MAX_DL);
			break;	
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);
	return 1;
}




INT16U Make_AID_0x54__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	INT32U* pVal32;
	INT16U* pVal16;
	INT8U*  pVal8;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	type32 tVal32;
	INT8U c;
	INT32U  uVal32;
	
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8 = &paid2->data[flag_len];	
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		#ifdef _AID_DELETE_NEEDED__		
		case 0x00: // Toffset
			flag[0] = 0xC0;			
			*pVal32++ = htonl(m_St.d_Toffset[_100M]);
			*pVal32++ = htonl(m_St.d_Toffset[Alpha]); // 2019.05.09
			break;

		case 0x01: // TBdelay(DL)
			flag[0] = 0xC0;			
			pVal32[0] = htonl(m_St.d_TBdelay_DL[_100M]);
			pVal32[1] = htonl(m_St.d_TBdelay_DL[Alpha]);
			break;
			
		case 0x02: // TBdelay(UL)
			flag[0] = 0xC0;			
			pVal32[0] = htonl(m_St.d_TBdelay_UL[_100M]);
			pVal32[1] = htonl(m_St.d_TBdelay_UL[Alpha]);
			break;	
		#endif
		
		case 0x03: // FPGA Delay(DL)
			flag[0] = 0xC0; // 2019.05.30
			for(c = 0; c < MAX_FA_PATH; c++)
				*pVal32++ = htonl(m_St.LS_Delay_DL_nsec[c]);
			break;	
			
		case 0x04: // FPGA Delay(UL)
			flag[0] = 0xC0; // 2019.05.30
			for(c = 0; c < MAX_FA_PATH; c++)
				*pVal32++ = htonl(m_St.LS_Delay_UL_nsec[c]);
			break;

		case 0x05: // Round Trip Delay
			flag[0] = 0xC0;
			// Optic Delay(Round Trip Delay) 0x5405 항목 개수 수정 2 --> 1
			pVal32[0] = htonl(m_St.d_RoundTrip_Dly[_100M]);
			pVal32[1] = htonl(m_St.d_RoundTrip_Dly[Alpha]);
			break;

		case 0x09: // 2020.06.24 : One-way delay
			/*
			Optic Delay(0x5405) / 2 표기
			Optic Delay(0x5405) 값이 0xFFFFFFFF인 경우, 0xFFFFFFFF 보고
			*/
			flag[0] = 0xC0;
			
			uVal32 = m_St.d_RoundTrip_Dly[_100M];
			if(uVal32 != 0xFFFFFFFF) uVal32 /= 2;
			pVal32[0] = htonl(uVal32);

			uVal32 = m_St.d_RoundTrip_Dly[Alpha];
			if(uVal32 != 0xFFFFFFFF) uVal32 /= 2;
			pVal32[1] = htonl(uVal32);			
			break;
			

		case 0x08:	// 2019.09.04, Delay 제어 수신 값, L/S 수신 DL/UL Delay Verification
			flag[0] = 0xC0;
			for(c = 0; c < MAX_Optic; c++)
			{
				tVal32.uD32 = htonl(m_St.LS_Dly_Ctrl_ns[c][DL]);
				memcpy(&pVal8[9*c+0], tVal32.uD8, 4);				
				
				tVal32.uD32 = htonl(m_St.LS_Dly_Ctrl_ns[c][UL]);
				memcpy(&pVal8[9*c+4], tVal32.uD8, 4);
				
				if(m_St.LS_Dly_Ret32 == 0) pVal8[9*c+8] = 0; // valid
				else                       pVal8[9*c+8] = 1; // invalid			
			}			
			break;

		///// YCSO added 190417
		case 0x0B:
			flag[0] = 0x80;
			pVal8[0] = m_St.Scale_Factor_UL;
			break;

		case 0x10: // FAN_Mode (On/Off)
			flag[0] = 0x80;
			pVal8[0] = m_Eep.FAN_Mode_Onoff;
			break;
			
		case 0x11: // FAN_Mode_St 동작 상태
			flag[0] = 0x80;			
			pVal8[0] = m_St.FAN_Mode_St;
			break;

		case 0x12: // FAN Mode 저소음 동작 시간
			flag[0] = 0x80;
			/*
				4: 시 단위(시작)
				3: 분 단위(시작)
				2: 시 단위(종료)
				1: 분 단위(종료)
			*/
			pVal8[0] = m_St.FAN_Mode_Stop__Time[1];
			pVal8[1] = m_St.FAN_Mode_Stop__Time[0];
			pVal8[2] = m_St.FAN_Mode_Start_Time[1];
			pVal8[3] = m_St.FAN_Mode_Start_Time[0];			
			break;
		
		case 0x20: // TDD Mode
			flag[0] = 0x80;			
			pVal8[0] = m_St.d_RW_TDD_Mode;
			break;
			
		case 0x21: // DL ON Guard Time
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_St.d_DL_ON_Guard_Time);
			break;
			
		case 0x22: // DL OFF Guard Time
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_St.d_UL_ON_Guard_Time);
			break;
			
		case 0x23: // DL to UL Guard Time
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_St.d_DL_to_UL_Guard_Time);
			break;
			
		case 0x24: // UL to DL Guard Time
			flag[0] = 0x80;
			*pVal16 = htons((INT16U)m_St.d_UL_to_DL_Guard_Time);
			break;

		
		case 0x30: // MIMO Standby Mode (On/Off)
			flag[0] = 0x80;			
			pVal8[0] = m_St.MIMO_Stdby_Enable;
			break;			

		case 0x31: // MIMO Standby Mode 동작 상태
			flag[0] = 0x80;			
			pVal8[0] = m_St.Sum_MIMO_Stdby_St;
			break;


		case 0x32: // MIMO Standby Mode 동작 시간
			flag[0] = 0x80;
			pVal8[0] = m_St.MiMo_Stdby_Stop_Time[1];
			pVal8[1] = m_St.MiMo_Stdby_Stop_Time[0];
			pVal8[2] = m_St.MiMo_Stdby_Start_Time[1];
			pVal8[3] = m_St.MiMo_Stdby_Start_Time[0];			
			break;
			
		case 0x33:
			flag[0] = 0x80;
			pVal8[0] = m_Eep.Layer_Mode;
			break;

		case 0x34: // AMP Standby Mode (On/Off)
		case 0x35: // AMP Standby Mode 동작 상태
		case 0x37: // AMP Standby Mode 적용 Path
			flag[0] = 0x80;
			     if(paid2->AID[1] == 0x34) pVal8[0] = m_St.AMP_Stdby_Enable; //
			else if(paid2->AID[1] == 0x35) pVal8[0] = m_St.Sum_AMP_Stdby_St;
			else                           pVal8[0] = m_St.AMP_Stdby_Path; //
			break;

		case 0x36: // AMP Standby Mode 동작 시간	
			flag[0] = 0x80;			
			pVal8[0] = m_St.AMP_Stdby_Stop_Time[1];
			pVal8[1] = m_St.AMP_Stdby_Stop_Time[0];
			pVal8[2] = m_St.AMP_Stdby_Start_Time[1];
			pVal8[3] = m_St.AMP_Stdby_Start_Time[0];
			break;

		case 0x38: // Batt Ext Mode Onoff
			flag[0] = 0x80;
			pVal8[0] = m_Eep.Batt_Ext_Mode_Onoff;
			break;
			
		case 0x39: // Batt_Ext_Mode 동작 상태
			flag[0] = 0x80;
			pVal8[0] = m_St.Batt_Ext_Mode_St;
			break;

		case 0x3A: // MIMO Standby Mode(진입 Level)
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.MIMO_Stdby_Level_x10[_IN_]);
			break;	

		case 0x3B: // MIMO Standby Mode(해제 Level)
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.MIMO_Stdby_Level_x10[_OUT_]);
			break;

		case 0x3F: // Test Port 출력
			flag[0] = 0x80;
			pVal8[0] = m_St.TestPort_CIM;
			break;

		case 0x44:
			flag[0] = 0x80;
			pVal8[0] = m_Eep.UDA_Oper_Mode[UDA_Spurious]; // 2019.04.27
			break;

		case 0x46:
			flag[0] = 0x80;
			pVal8[0] = m_Eep.UDA_Oper_Mode[UDA_Clock]; // 2019.04.27
			break;

		case 0x47:
			flag[0] = 0x80;
			pVal8[0] = m_Eep.UDA_Oper_Mode[UDA_PD_CP]; // 2019.04.27
			break;
			
		case 0x4C:
			flag[0] = 0x80;
			pVal8[0] = m_St.UDA_Spurious_Layer; // 2019.04.27
			break;

		case 0x50:
			flag[0] = 0x80;
			pVal8[0] = m_St.UL_Gain_A_BackOff; // 2019.05.13			
			break;

		case 0x51: // 2019.09.03
			flag[0] = 0x80;
			if(m_Eep.AAU_CW_Auto_Onoff) // 2020.01.20 added
				 *pVal8++ = ON;
			else *pVal8++ = OFF;
			break;	

		case 0x53: flag[0] = 0x80; pVal8[0] = m_Eep.Log_OnOff; break;
		case 0x54: flag[0] = 0x80; pVal8[0] = m_St.Log_Alm_Ctl_Cnt; break; // 2019.09.30


		

		/*
			AAU only
			LED 자동 Off 0x5455 : Default: ON,  Off 시간: 5분
			2019.08.21
		*/
		case 0x55: flag[0] = 0x80; pVal8[0] = m_St.AAU_LED_Auto_Off; break;
			
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}
//---------------------------------------------------------------------------------------------

			

INT16U Make_AID_0x55__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
//	INT32U* pVal32;
//	INT16U* pVal16;
	INT8U*  pVal8;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8 = &paid2->data[flag_len];	
//	pVal16 = (INT16U*)&paid2->data[flag_len];
//	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{		
		case 0x00: 
			flag[0] = 0x80;
			pVal8[0] = m_St.Maker[0]; 
			break;
			
		case 0x01: 
			flag[0] = 0x80;
			pVal8[0] = m_St.Maker[1]; 
			break;

		case 0x02: 
			flag[0] = 0x80;			
		#ifdef __TYPE__PRU__			
			pVal8[0] = m_St.Type[0]; 
			pVal8[1] = Get_Repeater_Type();
		#else
			memcpy(pVal8, m_St.Type, 2);
		#endif
			break;
			
		case 0x03:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.RCU_Ver_Main, 3);
			break;
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}




INT16U Make_AID_0x56__(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
//	INT32U* pVal32;
//	INT16U* pVal16;
	INT8U*  pVal8;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8 = &paid2->data[flag_len];	
//	pVal16 = (INT16U*)&paid2->data[flag_len];
//	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{		
		case 0x00: 
			flag[0] = 0x80;
			memcpy(pVal8, m_Eep.Statis_Rpt_Ref, 2);
			break;
			
		case 0x01: 
			flag[0] = 0x80;
			pVal8[0] = m_Eep.Statis_Rpt_Prd;
			break;

		case 0x02: // stat start time
			flag[0] = 0x80;
			memcpy(pVal8, m_St.stat_start_time, 6);
			break;

		case 0x03: // 자동보고 기준시
			flag[0] = 0x80;
			memcpy(pVal8, m_Eep.REMS_Auto_Rpt_Time, 2);
			break;	

		case 0x10:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.DL_State[DN_RCU], 2);
			break;

		case 0x20: 
			flag[0] = 0x80;
			pVal8[0] = m_St.ResetReason_log;
			break;	
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}


//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B0(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	//INT16U* pVal16;
	INT32U* pVal32;
	//INT16S	sVal16;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	//INT8U c; // carrier
	//INT8U l; // layer
	INT8U* pVal8;
	INT8U  var_size;

	// 2019.09.16
	switch(paid2->AID[1])
	{
		case 0x11: 
			var_size = m_Eep.Node_Name_len; 
		#ifdef __CHECK_BTS_SIZE__
			if(var_size > Node_Name_MAX)
				var_size = Node_Name_MAX;
		#endif
			break;
		
		default  : var_size = 0; break;
	}
	
	if(var_size > 0) // 2019.09.16
		Make_AID2_STR_Header_Var_Size(paid2, idx, &flag_len, var_size);
	else
		Make_AID2_STR_Header(paid2, idx, &flag_len);
		
	memset(flag, 0, MAX_AID2_Flag_Size);	

	pVal8 = &paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		case 0x00:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.Operate_Ip, 4);
			break;

		case 0x01:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.Operate_Mac, 6);
			break;

		case 0x02:
			flag[0] = 0x80;
			pVal8[0] = AufoConfigFlag.RE_Cascade_ID;
			break;

		case 0x03:
			flag[0] = 0x80;			
			*pVal32 = htonl(m_St.UTC); 
			break;	

		case 0x04: // Base IP 
			flag[0] = 0x80;				
			memcpy(pVal8, AufoConfigFlag.Base_Ip, 4);
			break;

		case 0x05: // Base MAC
			flag[0] = 0x80;
			memcpy(pVal8, AufoConfigFlag.Base_Mac, 6);
			break;

		case 0x06: // Time tick Offset
			flag[0] = 0x80;
			*pVal32 = htonl(m_Eep.Time_Tick_Offset); 
			break;			
			
		case 0x07: // S/N, "L/S 등록 번호"
			flag[0] = 0x80;
			memcpy(pVal8, m_St.LS_Regist_Num, SN_SIZE); // 2019.08.29 : eep-save 안된 현재의 값.
			break;			

		case 0x08: // Board Name
			flag[0] = 0x80;
			memcpy(pVal8, m_Eep.RU_Board_Name, BD_NANEMSIZE);
			break;	
		
		case 0x09:
			flag[0] = 0x80;
			pVal8[0] = AufoConfigFlag.Init_Status;
			break;

		case 0x0A:
			flag[0] = 0x80;
			pVal8[0] = AufoConfigFlag.Alarm_Acknowledge;
			break;

		case 0x0B:
			flag[0] = 0x80;
			pVal8[0] = AufoConfigFlag.Vaild_Indicator;
			break;
		///// YCSO added 190715 START
		#ifdef __TYPE__AAU__
		case 0x0C:	///// bluetooth Device ID
			flag[0] = 0x80;
			memcpy(pVal8, m_St.BT_Device_Code, BT_DEVICE_NAME_SIZE);
			break;

		case 0x0D:	////// bluetooth pairing code
			flag[0] = 0x80;
			memcpy(pVal8, m_Eep.BT_Pairing_Code, BT_PAIRING_CODE_SIZE);
			break;
		#endif
		////// YCSO added END

		case 0x10:
			flag[0] = 0x80;			
		#ifdef __TYPE__PRU__			
			pVal8[0] = m_St.Type[0]; 
			pVal8[1] = Get_Repeater_Type();
		#else			
			memcpy(pVal8, m_St.Type, 2);
		#endif
			break;
			
		case 0x11:
			flag[0] = 0x80;
			memcpy(pVal8, m_Eep.Node_Name, m_Eep.Node_Name_len);
			break;

		case 0x12: 
			flag[0] = 0x80;			
			memcpy(pVal8, m_Eep.User_ID, 11);
			break;

		case 0x13: 
			flag[0] = 0x80;			
			memcpy(pVal8, m_Eep.User_PW, 11);
			break;
			
		case 0x14:
			flag[0] = 0x80;
			pVal8[0] = m_Eep.mask;
			break;

		case 0x15:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.RCU_Ver, 3);
			break;	
					
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B1(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	
	//INT16U* pVal16;
	INT32U* pVal32;	
	//INT16S  sVal16;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	INT8U c; // carrier
	INT8U l; // layer
	//INT8U* pVal8;
	//INT8U  data[10];
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	//pVal8 = &paid2->data[flag_len];	
	//pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{	
		case 0x00:
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{			
				for(l = 0; l < MAX_UL; l++)
				#ifdef __TYPE__PRU__
					*pVal32++ = htonl(m_St.d_DL_TSSI_dec[c][l]); // AID 0xB100
				#else
					*pVal32++ = htonl(m_St.d_DL_TSSI_dec_Pre_EQ[c][l]); 
				#endif	
			}
			break;	
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B2(void* pAID, INT16U idx, INT8U port, INT8U arg) // AMP
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	
	INT16U* pVal16;
	INT32U* pVal32;	
	//INT16S  sVal16;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	//INT8U c; // carrier
	INT8U l; // layer
	INT8U* pVal8;	
	#ifdef __TYPE__PRU__
	#else
	INT16U  uVal16;
	INT8U   uVal8;
	#endif
	
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8 = &paid2->data[flag_len];	
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{	
		case 0x00:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.AMP_Temp_x10[l]);
			break;

		case 0x01: // 온도 보상 On/Off(PAU)
			flag[0] = 0xF0;			
			memcpy(pVal8, m_St.AMP_Temp_Onoff, AMPU_4_MAX);
			break;		

		case 0x02:
			flag[0] = 0xF0;						
			memcpy(pVal8, m_St.PAU_I_Ver, AMPU_4_MAX*3);
			break;

		case 0x03:
			flag[0] = 0xF0;					
			memcpy(pVal8, m_St.Pwr_Amp_Made, AMPU_4_MAX);			
			break;	

		case 0x04:
			flag[0] = 0xF0;						
			memcpy(pVal8, m_St.Pwr_Amp_Watt, AMPU_4_MAX);
			break;	

		case 0x05:
			flag[0] = 0xF0;					
			memcpy(pVal8, m_St.Pwr_Amp_SN, AMPU_4_MAX*4);			
			break;

		case 0x06:
			flag[0] = 0xF0;				
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)m_St.AMP_DC_Volt_x10[l]);
			break;

		case 0x07:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)(m_St.VSWR_x100[l]));
			break;	

		case 0x08:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)(m_St.Ret_Loss_x10[l]));
			break;		

		case 0x09:
			flag[0] = 0xF0;					
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)m_St.Ret_Loss_Thr_x10[l]);
			break;	

		case 0x0A:
			flag[0] = 0xF0;					
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)m_St.Ret_Loss_Thr_Rec_x10[l]);
			break;		

		case 0x0B:
			flag[0] = 0xF0;					
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)m_St.Over_Temp_Thr_x10[l]);
			break;

		case 0x0C:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)m_St.Over_Temp_Rec_Thr_x10[l]);
			break;
			
		case 0x0D:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons((INT16U)m_St.Over_Pwr_Thr_x10[l]);
			break;

		case 0x0E: // PAU Link 이상(DL)
			flag[0] = 0xF0;			
			memcpy(pVal8, m_St.Pwr_Amp_Link_Err, AMPU_4_MAX*4);
			break;

		case 0x10:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)			
				*pVal16++ = htons((INT16S)m_St.FWD_Pwr_Offset_x10[l]);
			break;

		case 0x12:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16S)m_St.Rvs_OUT_Pwr_Offset_x10[l]);
			break;
			
		case 0x13:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16S)m_St.Rvs_IN_Pwr_Offset_x10[l]);
			break;

		case 0x14:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16S)m_St.FB_Pwr_Offset_x10[l]);
			break;

		case 0x15: // Offset(Delay DL)
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal32++ = htonl(10*m_St.FWD_Dly_Offset_x100[l]); // 0.01us -> 1ns
			break;	

		case 0x16: // Offset(Delay UL)
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal32++ = htonl(10*m_St.RVS_Dly_Offset_x100[l]); // 0.01us -> 1ns
			break;

		case 0x17: // Offset(PAU Temp)
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_Eep.Temp_Offset_x10[l]);
			break;

		case 0x1A:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.FB_Pwr_x10[l]);
			break;

		case 0x1B:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.Rvs_OUT_Pwr_x10[l]);
			break;

		case 0x1C: // 2019.05.30
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.Rvs_IN_Pwr_x10[l]);
			break;	

		case 0x1D: // 2019.07.29
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.Fwd_IN_Pwr_x10[l]);
			break;		

			

		case 0x20: // ATTEN(PAU)(DL), PRU Only
			#ifdef __TYPE__PRU__  // ANTS, HFR
			flag[0] = 0xF0;	
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.Fwd_TPL_Att_x2[l]*5); // 0.5dB --> 0.1dB step
			#else // 0.1dB
			flag[0] = 0xF0;	
			for(l = 0; l < AMPU_4_MAX; l++)
			{
				/*
				---------------------------------
				2019.11.28 : 
					0xB220, 0x5206 : 같은 기능,
					m_St.DL_Att -> m_Eep.DL_Att_User, 변경. User ATT만 상태로 응답되도록 
				---------------------------------	
				*/
				uVal8 = conv_att_025_to_05(m_Eep.DL_Att_User[l]); 
				uVal16 = conv_att_05_to_01(uVal8);
				*pVal16++ = htons(uVal16);				
			}			
			#endif
			break;			

		case 0x21: // ATTEN(PAU FB)(DL)
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.FB_ATT_x2[l]*5);
			break;

		case 0x22: // ATTEN(PAU 온도보상)(DL)	
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16S)m_St.Fwd_Temp_Att_x2[l]*5); // PRU/AAU
			break;

		case 0x23: // ATTEN(LNA 온도보상)(UL)	
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16S)m_St.Rvs_Temp_Att_x2[l]*5); // PRU/AAU
			break;

		
		case 0x24: // ATTEN(LNA)(UL)						
		#ifdef __TYPE__PRU__  // ANTS, HFR
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.Rvs_Att_x2[l]*5);
		#else //---------------- AAU			
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++) // 2019.04.17
				*pVal16++ = htons(conv_att_025_to_001(m_Eep.UL_Att[l])/10);
			/*
			2019.11.28 : AID 0xB224
				m_St.UL_Att -> m_Eep.UL_Att, 변경, User ATT만 상태로 응답되도록 
			*/	
		#endif
			break;	
			

		case 0x25: // ATTEN(LNA ALC)(UL)	
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.RX_ALC_Att_x2[l]*5); // PRU/AAU Common
			break;

		case 0x26: // ATTEN(PAU FB 온도보상)(DL)
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16S)m_St.FB_Temp_Att_x2[l]*5); // PRU Only
			break;	

		
		case 0x27: // ATTEN(PAU Gain 보상)(DL), PRU Only
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.FWD_Att_Offset_x2[l]*5); // 0.5dB --> 0.1dB step
			break;
			

		case 0x28: // ATTEN(PAU FB Gain 보상)(DL)
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.FB_Offset_ATT_x2[l]*5);
			break;

		case 0x29: // ATTEN(LNA Gain 보상)(UL), PRU Only
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.Rvs_Att_Offset_x2[l]*5); // 0.5dB --> 0.1dB step
			break;	

		case 0x2A:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.TX_ALC_Att_x10[l]);
			break;

		case 0x30:
			flag[0] = 0xF0;			
			memcpy(pVal8, m_St.Rvs_IN_ALC_Onoff, AMPU_4_MAX);
			break;

		case 0x31:
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.ATT_Pair_Onoff, AMPU_4_MAX);
			break;

		case 0x32:
			flag[0] = 0xF0;			
			memcpy(pVal8, m_St.AMP_Auto_SD_Mode_En, AMPU_4_MAX);
			break;

		case 0x33:
			flag[0] = 0xF0;			
			memcpy(pVal8, m_St.Rvs_Gain_Bal_On, AMPU_4_MAX);
			break;

		case 0x34:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.Rvs_IN_ALC_x10[l]);
			break;

		case 0x35:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_St.Rvs_IN_SD_x10[l]);
			break;				

		case 0x36:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons(m_St.Rvs_Gain_Bal_x2[l]*5);
			break;				
			
		case 0x37:
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.RVS_Amp_Onoff, AMPU_4_MAX);
			break;

		case 0x3A:
			flag[0] = 0xF0;			
			memcpy(pVal8, m_St.AMP_DL_St, AMPU_4_MAX*2);
			break;

		#ifdef __TYPE__AAU__ 
		case 0x3B: // DL ALC On/Off(UL), 2019.05.30
			flag[0] = 0xF0;			
			memcpy(pVal8, m_Eep.AAU_Fwd_ALC_Onoff, AMPU_4_MAX);
			break;

		case 0x3C: // DL SD On/Off(UL), 2019.05.30
			flag[0] = 0xF0;			
			memcpy(pVal8, m_Eep.AAU_Fwd_SD__Onoff, AMPU_4_MAX);
			break;
		#endif		
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B3(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	
	INT16U* pVal16;
	//INT32U* pVal32;	
	//INT16S  sVal16;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	INT8U c; // carrier
	INT8U l; // layer
	//INT8U* pVal8;
	//INT8U  data[10];
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	//pVal8 = &paid2->data[flag_len];	
	pVal16 = (INT16U*)&paid2->data[flag_len];
	//pVal32 = (INT32U*)&paid2->data[flag_len];		
	
	switch(paid2->AID[1])
	{	
		case 0x02: // 2019.05.04 : 0xB302, RSSI(Filter 전)
			flag[0] = 0xFF;			
			for(l = 0; l < MAX_UL; l++)
				*pVal16++ = htons(m_St.RSSI_Before_Filter_x10[l]); 
			
			break;	

		case 0x03: // 2019.05.04 : 0xB303, RSSI(Filter 후)
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons(m_St.RSSI_After__Filter_x10[c][l]); 
			}
			break;

		case 0x04: // 2019.05.04 : 0xB304, UL DTU Gain 보상(타사 과입력)
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons(m_St.UL_DTU_Gain_Comp_x10[c][l]); 
			}
			break;	

		case 0x05:
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.Threshold_UL_Gain_Low_x10);
			break;	

		case 0x06: // 2019.05.14
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons(m_St.d_UL_Noise_RMS_x10[c][l]); 
			}
			break;		

		case 0x07: // 2019.05.10
			flag[0] = 0xFF;			
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_UL; l++)
					*pVal16++ = htons((INT16U)m_Eep.RSSI_Offset_x10[c][l]); 
			}
			break;

		case 0x08:
			flag[0] = 0x80;
			*pVal16 = htons((INT16U)m_Eep.Threshold_UL_Spurious_x10);
			break;	
		
		case 0x09: // 2019.08.27 changed 0x5C -> 0x09
			flag[0] = 0xFF;
			for(l = 0; l < MAX_DL; l++) *pVal16++ = htons(m_Eep.UL_Noise_Offset_x10[_100M][l]);
			for(l = 0; l < MAX_DL; l++) *pVal16++ = htons(m_Eep.UL_Noise_Offset_x10[Alpha][l]);
			break;
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B4(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

	INT32U* pVal32;
	INT16U* pVal16;
	INT8U* pVal8;
	INT8U l, uVal8;	
	INT8U c;	
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	INT16U uVal16;
	

	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal32 = (INT32U*)&paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal8  = &paid2->data[flag_len]; 	
	
	switch(paid2->AID[1])
	{		
		case 0x00: // 온도(DTU)
			flag[0] = 0x80; 		
			*pVal16 = htons((INT16U)m_St.d_DTU_Temp_x10);
			break;
			
		case 0x01: // 온도(DTU FPGA)		
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_St.d_R_FPGA_Temp_x10);
			break;

		case 0x02: // S/W 버전(DTU)
			flag[0] = 0x80;			
			memcpy(pVal8, m_St.d_SW_VER_DTU, 3);
			break;	
			
		case 0x03: // S/W 버전(FPGA)
			flag[0] = 0x80;			
			memcpy(pVal8, m_St.d_SW_VER_FPGA, 3);
			break;

		case 0x04: // DPD onoff, 0xB404
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.d_DPD_OnOff, MAX_DL);
			break;
			
		case 0x05: // DPD Err, 0xB405
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.d_DPD_Err_St[l]); // 2019.05.20
			break;	

		case 0x06: // 0xB406
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons((INT16U)m_St.d_DPD_In__Pwr_x10[l]);
			break;
			
		case 0x07: // 0xB407
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons((INT16U)m_St.d_DPD_Out_Pwr_x10[l]);
			break;

		case 0x08: // DPD Engine Out Power
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons((INT16U)m_St.d_DPD_Eng_In__Pwr_x10[l]); // 2019.07.03
			break;
			
		case 0x09:
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.d_DPD_FB_Eng_In_Pwr_x10[l]);
			break;		

		case 0x0A: // 0xB40A
			flag[0] = 0xF0;
			memcpy(pVal8, m_St.d_CFR_OnOff, MAX_DL);
			break;

		case 0x0B:
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
			#ifdef __TYPE__PRU__
				*pVal16++ = htons((INT16U)m_St.d_CFR_Level_x10[l]);
			#else			
				*pVal16++ = htons(m_Eep.d_PAR_x10[l]);
			#endif
			break;

		///// AID_NEEDED : Hard Clipping on/off : How to use ??
		case 0x0C:
			flag[0] = 0xF0;
			memcpy(pVal8, m_Eep.d_Hard_Clip_OnOff, MAX_DL);
			break;
		
		case 0x0D:	
			flag[0] = 0xF0;
			for(l =0; l < MAX_DL; l++)
			{
				if(m_Eep.d_Hard_Clip_OnOff[l] == ON)
					*pVal16++ = htons(m_St.d_Hard_Clip_Lvl_x10[l]);
				else
					*pVal16++ = htons(m_Eep.d_Hard_Clip_Lvl_x10[l]);
			}				
			break;
		
		case 0x0E:	
			// 2019.05.09 : Hard Clipping Count(DL) -> Hard Clipping Count Accumlation
			flag[0] = 0xF0;
			for(l =0; l < MAX_DL; l++)
				*pVal32++ = htonl((INT32U)m_St.d_Hard_Clip_Cnt_Accum[l]); // 2019.05.09
			break;
		
		case 0x10:
			flag[0] = 0xF0;
			for(l =0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.d_Offset_DPD_x10[l]); 
			break;

		case 0x13:
			flag[0] = 0xF0;
			for(l =0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.d_DPD_FB_In__Pwr_x10[l]); // m_St.d_DPD_FB_Eng_In_Pwr_x10[l] ????
			break;

		case 0x15: // 2019.04.06
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal32++ = htonl(m_Eep.d_Offset_DTU_Delay_DL[c][l]);
			}
			break;
			
		case 0x16: // 2019.04.06
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal32++ = htonl(m_Eep.d_Offset_DTU_Delay_UL[c][l]);
			}
			break;

		case 0x17: // 2019.05.09
			flag[0] = 0x80;
			*pVal16 = htons((INT16U)m_Eep.RCU_Temp_Offset_x10);
			break;

		case 0x19: // 2019.04.06 : CPRI Recovery Path Status
			flag[0] = 0xF0;			
			*pVal8 = conv_Reg_Clk_Recovery_to__AID(m_St.d_CPRI_Recovery_Path_St);
			break;		
		
		case 0x1A:
			flag[0] = 0x80;
			pVal8[0] = m_St.d_Clock_Status;
			break;
		
		case 0x1B:
			if(m_St.Ref_CPRI_No == CPRI_0) c = CPRI_0;
			else                           c = CPRI_1;	

			flag[0] = 0xC0;
			for(l = 0; l < MAX_FA_PATH; l++)
				*pVal8++ = (INT8U)(m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[l].uD32);
			break;		

		///// AID_NEEDED : how to use ??
		case 0x1C:
			flag[0] = 0xC0;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				uVal16 = (INT16U)m_St.d_Delay_Alarm_Layer[0][c]<<8;
				uVal16 += m_St.d_Delay_Alarm_Layer[1][c];
				*pVal16++ = htons(uVal16);
			}
			break;

		///// AID_NEEDED : how to use ??
		case 0x1D:
			flag[0] = 0xC0;
			for(c = 0; c < MAX_FA_PATH; c++)
				*pVal32++ = htonl(m_St.d_Delay_Set_Val[c]);
			break;

			
			
		case 0x1E:
			flag[0] = 0xF0;
			uVal8 = (m_St.d_RW_DL_BW_Sel[1][0] & 0x0F) <<4;
			uVal8 |= m_St.d_RW_DL_BW_Sel[0][0] & 0x0F;
			pVal8[0] = uVal8;
			break;

		///// AID_NEEDED  : how to use ??
		case 0x1F:
			flag[0] = 0xC0;
			for(c = 0; c < MAX_FA_PATH; c++)
				*pVal16++ = htons(m_St.d_ARFCN[c]);
			break;


		case 0x20:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons(m_St.d_Att_Dig_DL_x10[c][l]);
				}
			}
			break;

		case 0x21:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons(m_St.d_Att_Dig_UL_x10[c][l]);
				}
			}
			break;	
		
		case 0x22:
			flag[0] = 0xFF;
			for(c = 0; c < Optic_2; c++)
			{
				for( l = 0; l < MAX_DL; l++)
					*pVal16++ = htons((INT16U)m_St.d_Temp_Att_Dig_DL_x10[c][l]);
			}
			break;

		case 0x23:
			flag[0] = 0xFF;
			for(c = 0; c < Optic_2; c++)
			{
				for( l = 0; l < MAX_DL; l++)
					*pVal16++ = htons((INT16U)m_St.d_Temp_Att_Dig_UL_x10[c][l]);
			}
			break;			
		
		case 0x24:
			flag[0] = 0xF0;
			for( l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.d_Att_DL_SUM[l]);
			break;

		case 0x26:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for( l = 0; l < MAX_DL; l++)
					*pVal16++ = htons(m_St.d_UL_DTU_Gain_Backoff_GUI_x10[c][l]); // 2020.01.08 
			}
			break;	

		
		case 0x27: // BIP Err Accumulate Count(CPRI), 2019.05.09
			/*
			sequence : DL-CPRI0, UL-CPRI2, DL-CPRI1, UL-CPRI3
			*/
			flag[0] = 0xF0;
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[DL][CPRI_0]); // carrier 100+A
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[UL][CPRI_2]); // carrier 100+A
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[DL][CPRI_1]); // carrier A
			*pVal32++ = htonl(m_St.d_BIP_ErrCnt[UL][CPRI_3]); // carrier A
			break;

		case 0x29: // FPGA Delay(DL)
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal32++ = htonl(m_St.d_FPGA_Delay_ns_DL[c][l]); // 2019.05.13
				}
			}
			break;	
			
		case 0x2A: // FPGA Delay(UL)
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal32++ = htonl(m_St.d_FPGA_Delay_ns_UL[c][l]); // 2019.05.13
				}
			}
			break;

		case 0x2B: // Toffset
			flag[0] = 0xC0;			
			*pVal32++ = htonl(m_St.d_Toffset[_100M]);
			*pVal32++ = htonl(m_St.d_Toffset[Alpha]); // 2019.05.09
			break;

		case 0x2C: // TBdelay(DL)
			flag[0] = 0xC0;			
			pVal32[0] = htonl(m_St.d_TBdelay_DL[_100M]);
			pVal32[1] = htonl(m_St.d_TBdelay_DL[Alpha]);
			break;
			
		case 0x2D: // TBdelay(UL)
			flag[0] = 0xC0;			
			pVal32[0] = htonl(m_St.d_TBdelay_UL[_100M]);
			pVal32[1] = htonl(m_St.d_TBdelay_UL[Alpha]);
			break;
		
		case 0x2E:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal8++ = m_St.d_Path_OnOff_DL[c][l];
			}
			break;

		
		case 0x2F:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal8++ = m_St.d_Path_OnOff_UL[c][l];
			}
			break;


		case 0x30:
			flag[0] = 0xF0;
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal8++ = m_St.d_Test_Pattern_OnOff[_100M][l];
			break;
			
		case 0x31: // Payload On/Off, 0xB431
			flag[0] = 0xFF;
			memcpy(pVal8, m_St.d_Payload_OnOff, 8); // 2019.04.13
			break;


		case 0x32:
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_RW_SFP_Reset[CPRI_0];
			*pVal8++ = m_St.d_RW_SFP_Reset[CPRI_2];
			*pVal8++ = m_St.d_RW_SFP_Reset[CPRI_1];
			*pVal8++ = m_St.d_RW_SFP_Reset[CPRI_3];
			break;
			

		///// AID_NEEDED : how to use ??		
		case 0x33:
			flag[0] = 0x80;
		#ifdef __TYPE__PRU__
			pVal8[0] = m_St.PRU_DTU_TempComp; // 2019.12.18
		#else
			pVal8[0] = m_St.d_Debug__OnOff_L2_SW;
		#endif	
			break;


		case 0x34:
			flag[0] = 0x80;			
			pVal8[0] = m_Eep.TempComp_Onoff;			
			break;
			
		case 0x35: // Rx/Cascade Add On/Off(UL)
			flag[0] = 0x80; // 2018.05.06
			pVal8[0] = m_St.d_Rx_Casc_Add_OnOff_UL;
			/*
				// if AID is changed, below will be used... maybe..
				//m_St.d_Rx_Casc_Add_OnOff[MAX_Optic][MAX_MIMO_PATH];
			*/
			break;

		/*		
		2019.07.17 : 
			Loopback On/Off는 제어는 안되고 상태만 표시하며, 
			VSS 필드의 Loopback 필드값이 0xCC, 0xCA인 경우 On, 
			이외 다른값은 Off로 처리
		*/
		case 0x36:
			flag[0] = 0x80;
			pVal8[0] = m_St.d_Loopback_OnOff;
			break;

		case 0x37: // 2019.04.15
			flag[0] = 0x80;
			*pVal16 = htons(m_St.d_Volt_DTU);
			break;

		case 0x38: // 2019.04.15
		#ifdef __TYPE__PRU__
			flag[0] = 0xF0;
			*pVal16++ = htons(m_St.d_Volt_DTU_Sub[0]);
			*pVal16++ = htons(m_St.d_Volt_DTU_Sub[1]);
			*pVal16++ = htons(m_St.d_Volt_DTU_Sub[2]);
			*pVal16++ = htons(m_St.d_Volt_DTU_Sub[3]);
		#else
			flag[0] = 0x80;
			*pVal16++ = htons(m_St.d_Volt_DTU_Sub[PATH_0]);
			*pVal16++ = 0;
			*pVal16++ = 0;
			*pVal16++ = 0;
		#endif	
			break;	

		case 0x3A:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.DL_State[DN_DTU], 2);
			break;

		case 0x3B:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.DL_State[DN_FPGA], 2);
			break;


		case 0x3C:
			flag[0] = 0x80;
			*pVal16 = htons(m_St.CPU_Usage);
			break;
			
		case 0x3D:
			flag[0] = 0x80;
			*pVal16 = htons(m_St.Mem_Usage);
			break;

		///// AID_NEEDED, L2 switch
		case 0x3E:
			flag[0] = 0x80;
			pVal8[0] = m_St.Link_L2_SW;
			break;
		
		case 0x3F:
		/*
			// 2019.05.09 : delete needed... AID is overlapped with "0x5315"
		*/		
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_SFP_Insert_St[_100M][0];
			*pVal8++ = m_St.d_SFP_Insert_St[_100M][1];
			*pVal8++ = m_St.d_SFP_Insert_St[Alpha][0];
			*pVal8++ = m_St.d_SFP_Insert_St[Alpha][1];
			break;

		
		case 0x40:
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_CPRI_Status[_100M][0];
			*pVal8++ = m_St.d_CPRI_Status[_100M][1];
			*pVal8++ = m_St.d_CPRI_Status[Alpha][0];
			*pVal8++ = m_St.d_CPRI_Status[Alpha][1];
			break;

		/////////////////////////////////////////////////////////////////////////////////////////
		// VSS
		/////////////////////////////////////////////////////////////////////////////////////////	

		case 0x41:
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_VSS_Z__2__CPRI_Version[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__2__CPRI_Version[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__2__CPRI_Version[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__2__CPRI_Version[CPRI_3][DL];
			break;	

		case 0x42:
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_VSS_Z_66__CPRI_Startup[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_66__CPRI_Startup[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_66__CPRI_Startup[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_66__CPRI_Startup[CPRI_3][DL];
			break;	

		case 0x43:
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_VSS_Z_130_CPRI_L1_RST_LOS[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_130_CPRI_L1_RST_LOS[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_130_CPRI_L1_RST_LOS[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_130_CPRI_L1_RST_LOS[CPRI_3][DL];
			break;	
		
		case 0x44:
			flag[0] = 0xF0;
			*pVal8++ = m_St.d_VSS_Z_194_Pointer_P[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_194_Pointer_P[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_194_Pointer_P[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_194_Pointer_P[CPRI_3][DL];
			break;

		////////////////////////////////////////////////////
		case 0x45: // VSS (NOTI IP1)
			flag[0] = 0xC0;
			memcpy(&pVal8[0], m_St.d_VSS_Noti_IP_1[CPRI_0], 4);
			memcpy(&pVal8[4], m_St.d_VSS_Noti_IP_1[CPRI_1], 4);
			break;

		case 0x46: // VSS (NOTI IP2)
			flag[0] = 0xC0;
			memcpy(&pVal8[0], m_St.d_VSS_Noti_IP_2[CPRI_0], 4);
			memcpy(&pVal8[4], m_St.d_VSS_Noti_IP_2[CPRI_1], 4);
			break;

		case 0x47: // VSS (TDD Slot Number), 
			flag[0] = 0xC0;
			memcpy(&pVal8[0],  m_St.d_VSS_TDD_Slot_Num[CPRI_0], 20); // 100M
			memcpy(&pVal8[20], m_St.d_VSS_TDD_Slot_Num[CPRI_1], 20); // alpha
			//
			// Cascade(CPRI2, RCPI3) is excluded..
			//
			break;

		////////////////////////////////////////////////////	
		
		case 0x50:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_2][UL];	
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_2][DL];					
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_3][UL];	
			*pVal8++ = m_St.d_VSS_Z__16__Rec___ReSerDesLOS[CPRI_3][DL];				
			break;	

		case 0x51:
			flag[0] = 0xFF;	
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__80__Rec___ReLOFAlarm[CPRI_3][DL];			
			break;	
			
		case 0x52:
			flag[0] = 0xFF;	
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[CPRI_3][DL];			
			break;	

		case 0x53:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_208__CnM_Path_Select[CPRI_3][DL];
			break;	

		case 0x54:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_0][UL];					
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__17__ReReset___ReResetAck[CPRI_3][DL];
			break;	

		case 0x55:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__81__ReChannelBW___AliveAck[CPRI_3][DL];
			break;	

		case 0x56:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[CPRI_3][DL];
			break;	

		case 0x57:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_149__CpriCompressOnOff[CPRI_3][DL];			
			break;	

		case 0x58:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[CPRI_3][DL];
			break;	

		case 0x59:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_2][DL];			
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__22__OpticLosDetect___Alarm[CPRI_3][DL];			
			break;	

		case 0x5A:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_2][DL];			
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[CPRI_3][DL];
			break;
			
		case 0x5B:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_0][UL];		
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__23__Re1Reset___ACK[CPRI_3][DL];		
			break;

		case 0x5C:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_0][UL];			
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__87__Re2Reset___ACK[CPRI_3][DL];
			break;

		case 0x5D:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_0][UL];		
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_2][DL];		
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_151__Re3Reset___ACK[CPRI_3][DL];		
			break;

		case 0x5E:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_1][UL];		
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_215__Re4Reset___ACK[CPRI_3][DL];
			break;

		case 0x5F:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_0][UL];	
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_2][DL];	
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_1][UL];	
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[CPRI_3][DL];	
			break;

		case 0x60:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[CPRI_3][DL];
			break;

		case 0x61:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[CPRI_3][DL];
			break;

		case 0x62:
			flag[0] = 0xFF; 		
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[CPRI_3][DL];
			break;

		case 0x64:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_1][DL];			
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__30__Reserved___ReVendorID[CPRI_3][DL];
			break;

		case 0x65:
			flag[0] = 0xFF;
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_0][DL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_0][UL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_2][UL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_2][DL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_1][DL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_1][UL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_3][UL];
			*pVal8++ = m_St.d_VSS_Z__60__Rec_PortNum[CPRI_3][DL];			
			break;

		// 2019.11.30
		case 0x6E:
			flag[0] = 0x80;
			pVal8[0] = m_St.VSS_Monitor_OnOff;
			break;
			
		case 0x6F:
			flag[0] = 0x80;
			if((m_St.VSS_Monitor_OnOff < 1)||(m_St.VSS_Monitor_OnOff > 4))
		     	return 0;			
			else			
				memcpy(pVal8, m_St.VSS_Rx, 255);			
			break;
		
		case 0x70:
			flag[0] = 0x80;
			if((m_St.VSS_Monitor_OnOff < 1)||(m_St.VSS_Monitor_OnOff > 4))
				return 0;
			else
				memcpy(pVal8, m_St.VSS_Tx, 255);
			break;
		
		/////////////////////////////////////////////////////////////////////////////////////////
			
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B5(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	(void)arg;	(void)port;
	return 0;
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_B6(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

	INT32U* pVal32;
	INT16U* pVal16;
	INT8U* pVal8;
	
	//INT16S	sVal16;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	
	pVal8 = &paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{	
		case 0x00:
			flag[0] = 0x80;					
			*pVal16 = htons((INT16U)m_PSU.Temp_x10);
			break;

		case 0x02:
			flag[0] = 0x80;					
			memcpy(pVal8, m_PSU.PSU_I_Ver, 3);			
			break;

		case 0x03:
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.AC_In_Volt_x10);
			break;
			
		case 0x04:
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.DC_Out_Volt_x10[0]);
			break;	
			
		case 0x05:
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.Load_Current_x10[0]);
			break;

		case 0x06:
			flag[0] = 0x80;		
			*pVal16 = htons((INT16U)m_PSU.DC_Out_Volt_x10[1]);
			break;

		case 0x07: // Batt Voltage
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.Batt_Volt_x10);
			break;
		 
		case 0x08:
			flag[0] = 0x80;
			// 2019.09.05 : 'm_Eep.DC_Fail_Volt_x10[DC__48V]' --> m_St.d_Volt_DTU			 
			*pVal16 = htons(m_St.d_Volt_DTU); 
			break;
			
		// 전압(DTU) Main 전원 DC Fail 설정값
		case 0x09:		
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.DC_Fail_Volt_x10[DC_Main]);
			break;

		case 0x0A: // 2019.07.29, 전압(DC 48V) 상한
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.AAU_48V_Limit_x10[_UP_]);
			break;	

		case 0x0B: // 2019.07.29, 전압(DC 48V) 하한
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.AAU_48V_Limit_x10[_DN_]);
			break;
			
		case 0x0E: // PSU Link Fail Status
			flag[0] = 0x80;			
			pVal8[0] = m_St.PSU_Link_St;
			break;	

		case 0x10:
			flag[0] = 0x80;	
			*pVal16 = htons((INT16U)(m_St.FAN_On_Temp[EXT_FAN]*10));
			break;
			
		case 0x11:
			flag[0] = 0x80;	
			*pVal16 = htons((INT16U)(m_St.FAN_OffTemp[EXT_FAN]*10));
			break;

		
		case 0x12: // FAN(외장) RPM
			flag[0] = 0x80;
			#ifdef __TYPE__PRU__
			*pVal16 = htons((INT16U)m_PSU.Fan_Speed); // FAN(외장) RPM
			#else
			*pVal16 = htons(m_St.AAU_Fan_Speed);
			#endif
			break;
			
		case 0x13:
			flag[0] = 0x80;			
			pVal8[0] = m_St.FAN_Auto[EXT_FAN];
			break;

		
		case 0x14:
			flag[0] = 0x80;	
			*pVal16 = htons((INT16U)(m_St.FAN_On_Temp[INT_FAN]*10));
			break;
			
		case 0x15:
			flag[0] = 0x80;
			*pVal16 = htons((INT16U)(m_St.FAN_OffTemp[INT_FAN]*10));
			break;
			
		case 0x16: // FAN(PSU) RPM
			flag[0] = 0x80;	
			if(m_St.FAN_OnOff[INT_FAN] == ON)
				*pVal16 = htons(100);
			else
				*pVal16 = htons(0);
			break;
			
		case 0x17:
			flag[0] = 0x80;			
			pVal8[0] = m_St.FAN_Auto[INT_FAN];
			break;

		case 0x18:
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_St.FAN_Ext_Max_RPM);
			break;	

		///// AID_NEEDED : check needed
		case 0x20:
			flag[0] = 0x80;			
			*pVal32 = htonl((INT32U)m_PSU.Watt_Tot_Accu);
			break;	
		case 0x21:
			flag[0] = 0x80;
			*pVal16 = htons((INT16U)m_PSU.Watt_real_time);
			break;
		case 0x22:
			flag[0] = 0x80;	
			*pVal16 = htons((INT16U)m_PSU.Watt_hour_aver);
			break;
		case 0x23:
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.Watt_day_aver);
			break;
		case 0x24:
			flag[0] = 0x80;			
			*pVal16 = htons((INT16U)m_PSU.Watt_week_aver);
			break;
			
		#if 0  ///// AID_NEEDED : current month
		case 0x25:
			flag[0] = 0x80;			
			*pVal32 = htons((INT32U)m_PSU.Watt_month);
			break;
		#endif

		case 0x30: // PAU SW Status
			flag[0] = 0x80;
			paid2->data[flag_len] = m_PSU.PAU_Switch_St;
			break;	

		case 0x31 : // FAN Status(case)
			flag[0] = 0x80;
			pVal8[0] = m_PSU.PSU_Ext_FAN_Sum;
			break;
			
		case 0x32 : // PSU Status(AC/DC) 
			flag[0] = 0x80;			
			pVal8[0] = m_PSU.PSU_St_AC_DC;
			break;

		case 0x33 : // PSU Status(Temp/DC)
			flag[0] = 0x80;			
			pVal8[0] = m_PSU.PSU_St_Temp_DC;
			break;

		case 0x34 : // PSU Status(etc)
			flag[0] = 0x80;			
			pVal8[0] = m_PSU.PSU_St_Etc;
			break;
			
		case 0x3A : // D/L State(PSU)(FW) 
			flag[0] = 0x80;			
			memcpy(pVal8, m_PSU.DL_St, 2);
			break;
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}
//---------------------------------------------------------------------------------------------
INT16U Make_AID_Internal_F2(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

//	INT32U* pVal32;
//	INT16U* pVal16;
	INT8U* pVal8;
		
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);
	
	pVal8 = &paid2->data[flag_len];
//	pVal16 = (INT16U*)&paid2->data[flag_len];
//	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		case 0xF3:
			flag[0] = 0xF0; 		
			memcpy(pVal8, m_Eep.Other_Rvs_IN_ALC_Onoff, AMPU_4_MAX);
			break;		
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}
//---------------------------------------------------------------------------------------------


/*
INT8U  d_DTU_App_MB_St; // 0xC000, DTU App/MB(microbraze) status
INT8U  d_FPGA_St;		// 0xC001
INT8U  d_DTU_Mode;		// 0xC002
INT16U d_RE_Reset;		// 0xC003
INT16U d_RE_Reset_Clr;	// 0xC004, control
INT8U  d_NCO_VALID; 	// 0xC011


INT8U  d_CFR_Ver[2];	// 0xC030
INT8U  d_DPD_Ver[3];	// 0xC031


INT32U d_DPD_Eng_InPwr[4]; // 0xC034

INT16U d_CPRI_Misc_St[2];  // 0xC041
INT8U  d_CPRI_eth_FIFO_Reset[2]; // 0xC042, control only
*/

// ANTS AID
INT16U Make_AID_Internal_C0(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;
	
	INT16U* pVal16;
	INT32U* pVal32;	
	//INT16S  sVal16;
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	INT8U uVal8;
	INT8U c; // carrier
	INT8U l; // layer
	INT8U* pVal8;
	
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);

	pVal8 = &paid2->data[flag_len];	
	pVal16 = (INT16U*)&paid2->data[flag_len];
	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{	
		case 0x10: 
			flag[0] = 0xC0;
			#ifdef __TYPE__PRU__
			*pVal32++ = htonl(m_St.d_NCO[0]); 
			*pVal32++ = htonl(m_St.d_NCO[1]);
			#else
			*pVal32++ = htonl(m_St.d_NCO_PRU[_100M][DL]);
			*pVal32++ = htonl(m_St.d_NCO_PRU[Alpha][DL]);
			#endif
			break;

		///// AID_NEEDED : how to use ??
		case 0x11:	///// NCO_Valid
			flag[0] = 0xC0;
			*pVal8++ = m_St.d_NCO_VALID[0];
			*pVal8++ = m_St.d_NCO_VALID[1];
			break;

		
		case 0x12:	
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons(m_St.d_DL_CW_LEVEL[c][l]);
				}
			}
			break;
		
		case 0x13:	
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal8++ = m_St.d_DL_CW_ON[c][l];
				}
			}
			break;

		case 0x14: 
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons(m_St.d_EQ_Coeff_DL[c][l]);
				}
			}
			break;

		case 0x15: 
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons((INT16U)m_St.d_Gain_DL_x10[c][l]);
				}
			}
			break;


		case 0x16:
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons((INT16U)m_St.d_Gain_UL_x10[c][l]);
				}
			}
			break;

		case 0x19: 
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{				
					*pVal16++ = htons(m_St.d_EQ_Coeff_UL[c][l]);				
				}
			}
			break;

		case 0x1A: 
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons(m_St.d_EQ_Coeff_1_DL[c][l]);
				}
			}
			break;

		case 0x1B: 
			flag[0] = 0xFF;				
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal16++ = htons(m_St.d_EQ_Coeff_1_UL[c][l]);
				}
			}
			break;

		case 0x1C:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{
					*pVal8++ = m_St.d_DL_TSSI_BalAtt_x10[c][l];
				}
			}
			break;

		case 0x1D: 
		case 0x1E: 
		case 0x1F: 
			flag[0] = 0xFF;
			uVal8 = paid2->AID[1] - 0x1D;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
					*pVal32++ = htonl(m_St.d_TSSI_SD_Level_x10[c][l][uVal8]);
			}
			break;

		case 0x20:
			flag[0] = 0xF8;
			for(l = 0; l < 5; l++)
				*pVal16++ = htons(m_St.d_UL_Gain_Guard_time[l]);			
			break;
			

///// YCSO added START 190410
		case 0x21:
			flag[0] = 0x80;
			*pVal32++ = htonl( m_St.d_t_Sync_Delay );
			break;

		case 0x22:
			flag[0] = 0x80;
			*pVal8++ = m_St.d_t_Sync_Fail;
			break;

//-----------------------------------
#ifdef __TYPE__PRU__
//-----------------------------------	
		case 0x23: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_Capture_DL_Time); 
			break;
			
		case 0x24: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_Capture_UL_Time); 
			break;

		case 0x25: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_Capture_ULNoise_Time); 
			break;			

		case 0x26: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_UL_Noise_RSSI_StartTime); 
			break;

		case 0x27: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_UL_Noise_RSSI_StopTime); 
			break;			
		
		case 0x28: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_UL_RSSI_StartTime); 
			break;
			
		case 0x29: // 2019.06.01
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.d_UL_RSSI_StopTime); 
			break;
	
		case 0x30:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.d_SW_VER_CFR, 2);
			break;

		case 0x31:
			flag[0] = 0x80;
			memcpy(pVal8, m_St.d_SW_VER_DPD, 3);
			break;

		case 0x32:
			flag[0] = 0xF0;
			for( l = DL_0; l < MAX_DL; l++ )
			 	*pVal32++ = htonl(m_St.d_DPD_St[l]); // 2019.05.20
			break;

		case 0x33:
			flag[0] = 0xF0;
			for( l = DL_0; l < MAX_DL; l++ )
			 	*pVal32++ = htonl(m_St.d_DPD_Success_Cnt[l]); // 2019.05.20 : name changed.
			break;

		case 0x34:
			flag[0] = 0xF0;
			for( l = DL_0; l < MAX_DL; l++ )
			 	*pVal32++ = htonl(m_St.d_DPD_Eng_In__Pwr_x10[l]);
			break;
//-----------------------------------			
#endif
//-----------------------------------
		case 0x35:
			flag[0] = 0xF0;
			for( l = DL_0; l < MAX_DL; l++ )
			 	*pVal16++ = htons(m_St.d_CFR_Out_Gain[l]);
			break;
///// YCSO added END 

		case 0x36:		//// DL ATT
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
			{
			#ifdef __TYPE__PRU__  // ANTS
				*pVal8++ = conv_att_05_to_025(m_St.Fwd_Out_Att_x2[l]);
			#else // AAU
				*pVal8++ = m_Eep.DL_Att_Offs[l]; // 0.25 step 
			#endif
			}
			break;	

		case 0x37:
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
			{
			#ifdef __TYPE__PRU__  // ANTS
				*pVal8++ = conv_att_05_to_025(m_St.Rvs_Att_x2[l]);
			#else // ANTS-AAU			
				*pVal8++ = m_St.UL_AFE_ATT_x1[l]*4; // 1[dB] step -> 0.25[dB] step
			#endif
			}
			break;
		
		
		case 0x40:	///// CPRI STATUS			
			#ifdef __TYPE__PRU__ // ANTS-PRU
			flag[0] = 0xC0;
			pVal8[0] = (INT8U)A_MMAP_St[CPRI_0].CPRI_Status;
			pVal8[1] = (INT8U)A_MMAP_St[CPRI_1].CPRI_Status;
			#else
			flag[0] = 0xC0;
			pVal8[0] = (INT8U)m_Reg_Sts.AXI[CPRI_0].Cpri_Sts.Data;
			pVal8[1] = (INT8U)m_Reg_Sts.AXI[CPRI_1].Cpri_Sts.Data;
			#endif
			
		case 0x41:	///// CPRI Miscellaneous Status
			#ifdef __TYPE__PRU__ // ANTS-PRU
			flag[0] = 0xC0;
			*pVal16++ = htons((INT16U)A_MMAP_St[CPRI_0].Mscl_Status.Data);
			*pVal16++ = htons((INT16U)A_MMAP_St[CPRI_1].Mscl_Status.Data);
			#else
			flag[0] = 0xC0;
			*pVal16++ = htons((INT16U)m_Reg_Sts.AXI[CPRI_0].Mscl_Status.Data);
			*pVal16++ = htons((INT16U)m_Reg_Sts.AXI[CPRI_1].Mscl_Status.Data);
			#endif
			break;
		/*	
		///// AID_NEEDED  : how to use ??
		case 0x42:	///// CPRI Ethernet FIFO Reset
			break;
		*/
		
		case 0x43:	///// BIP value 128 bit
			flag[0] = 0xFF;
			flag[1] = 0xFF;
			
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal32++ = htonl(m_St.d_BIP_Cnt128[DL][CPRI_0][l]);
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal32++ = htonl(m_St.d_BIP_Cnt128[DL][CPRI_1][l]);
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal32++ = htonl(m_St.d_BIP_Cnt128[UL][CPRI_2][l]);
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal32++ = htonl(m_St.d_BIP_Cnt128[UL][CPRI_3][l]);
			break;


		///// 
		/////case 0x44: 2019.04.23 : deleted....		
		/////break;		
		

		case 0x46:	
			flag[0] = 0xC0;			
			*pVal16++ = htons(m_St.d_BIP_Count[UL][CPRI_2]);
			*pVal16++ = htons(m_St.d_BIP_Count[UL][CPRI_3]);
			break;
			
		case 0x47:
			flag[0] = 0x80;
			*pVal16 = htons(m_Reg_Sts.RW_ETH_Path_Sel.Data);
			break;			

		// 2019.04.22.		
		// 전압(DTU) 48V DC Fail 설정값		
		case 0x48:
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.DC_Fail_Volt_x10[DC__48V]);
			break;
			
		// 전압(DTU) Main 전원 DC Fail 설정값
		case 0x49:		
			flag[0] = 0x80;
			*pVal16 = htons(m_Eep.DC_Fail_Volt_x10[DC_Main]);
			break;

		case 0x4A:
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_Eep.AFE_Rvs_ALC_x10[l]);
			break;

		case 0x4B: // DL ALC On/Off(UL), 2019.05.13, AAU Only
			flag[0] = 0xF0;
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal8++ = m_Eep.AAU_Fwd_ALC_Onoff[l];
			break;

		case 0x4C: // 2019.05.13, 0xC04C, AAU-ANTS Only
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_Eep.AAU_Fwd_ALC_x10[l]);
			break;	

		case 0x4D: // 2019.05.13, 0xC04D, AAU-ANTS Only
			flag[0] = 0xF0;			
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal16++ = htons((INT16U)m_Eep.AAU_Fwd_SD__x10[l]);
			break;

		case 0x4E: // 2019.05.13
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
				{
				
					*pVal32++ = htonl(m_St.T2A_Dly__ns[c]); 				
				}
			}
			break;
			
		case 0x4F: // 2019.05.13
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
				{					
					*pVal32++ = htonl(m_St.TA3_Dly__ns[c]);
				}
			}
			break;

		case 0x50: // DL SD On/Off(UL), 2019.05.13, AAU Only
			flag[0] = 0xF0;
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal8++ = m_Eep.AAU_Fwd_SD__Onoff[l];
			break;

		case 0x51: // 2019.05.15
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal16++ = htons(m_Eep.DL_FB_Offset_x10[c][l]);
			}
			break;		

		case 0x52: // 2019.05.15
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal16++ = htons(m_Eep.TSSI_Offset_x10[c][l]);
			}
			break;

		case 0x53: // 2019.05.16
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal16++ = htons(m_St.d_R_GC_RSSI_RMS_x10[c][l]);
			}
			break;

		case 0x54: // 2019.05.16
			flag[0] = 0x80;
			*pVal8++ = m_Reg_Sts.Gain_Check_Mode.onoff;
			break;

		case 0x55:
			flag[0] = 0x80;
			memcpy(pVal8, m_Env.test_ls_ip, 4);
			break;

		case 0x56:
			flag[0] = 0x80;
			*pVal8 = m_Env.debug_optic;
			break;

		case 0x57:
			flag[0] = 0x80;
			*pVal8 = m_Env.debug_path4;
			break;
		
		case 0x58: // 2019.05.23, AAU-ANTS Only
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.UL_Gain_Low_x10[l]);
			break;

		case 0x59: // 2019.05.23, AAU-ANTS Only
			flag[0] = 0x80;
			*pVal8 = m_St.DL_Mode_En;
			break;	

		case 0x5A: // 2019.05.27
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_St.Rvs_IN_Pwr_x10[l]);
			break;

		case 0x5B: // 2019.05.27
			flag[0] = 0xFF;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.Offset_RSSI_Before_Filter_x10[l]);
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = 0;
			break;

		case 0x5C: // 2019.05.27
			flag[0] = 0xFF;
			for(l = 0; l < MAX_DL; l++) *pVal16++ = htons(m_Eep.UL_Noise_Offset_x10[_100M][l]);
			for(l = 0; l < MAX_DL; l++) *pVal16++ = htons(m_Eep.UL_Noise_Offset_x10[Alpha][l]);
			break;

		case 0x5D: // 2019.05.27
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.UL_GC_RSSI_Offset_x10[_100M][l]);
			break;	

		case 0x5E: // 2019.05.27
			flag[0] = 0x80;
			*pVal16++ = htons(m_Eep.AAU_48V_Offset_x10);
			break;

		/*
		//2019.08.12 : AID 0xB60A/0B 의 중복으로 0xC05F "48V DC Fail limit" 삭제.
		//case 0x5F: // 2019.05.27
		//	flag[0] = 0xC0;
		//	*pVal16++ = htons(m_Eep.AAU_48V_Limit_x10[0]);
		//	*pVal16++ = htons(m_Eep.AAU_48V_Limit_x10[1]);
		//	break;
		*/
		
		case 0x60: // 2019.05.30
			flag[0] = 0x80;
			*pVal8 = m_St.Gain_Noise_Auto;
			break;

		case 0x61: // 2019.06.29
			flag[0] = 0x80;
			*pVal8 = m_St.ANTS_LED_Status;
			break;

		//------------------------------------------
		case 0x62: 
		case 0x63: 
		case 0x64: 
			flag[0] = 0xFF;
			uVal8 = paid2->AID[1] - 0x62;
			if(uVal8 >= 3) break;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
					*pVal16++ = htons(m_Eep.d_LS_gain_x10[uVal8][DL][c][l]);
			}
			break;
		//------------------------------------------
		case 0x65: 
		case 0x66: 
		case 0x67: 
			flag[0] = 0xFF;
			uVal8 = paid2->AID[1] - 0x65;
			if(uVal8 >= 3) break;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
					*pVal16++ = htons(m_Eep.d_LS_gain_x10[uVal8][UL][c][l]);
			}
			break;
		//------------------------------------------
		case 0x68: 
			flag[0] = 0xFF;
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
					*pVal16++ = htons(m_St.d_Test_Pattern_Band_Gain[c][l]);
			}
			break;		
			
		//------------------------------------------
		case 0x69: 
			flag[0] = 0xF0;
			for(l = 0; l < MAX_MIMO_PATH; l++)
				*pVal32++ = htonl(m_St.d_CFR_Level_x10[l]);
			break;

		///// YCSO added 190716
		#ifdef __TYPE__AAU__
		case 0x6A: 
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.aau_spectrum_DL_offest[l]);
			break;

		case 0x6B: 
			flag[0] = 0xF0;
			for(l = 0; l < MAX_UL; l++)
				*pVal16++ = htons(m_Eep.aau_spectrum_UL_offest[l]);
			break;
		#endif
		///// YCSO added END

		case 0x6C: // 2019.08.06, Height Balnance On/Off
			flag[0] = 0x80;
			*pVal8 = m_Eep.Height_Bal_Onoff;
			break;

		// 2019.08.13 : 파형감시 DL/UL Offset
		case 0x6D:
			flag[0] = 0xC0;
			*pVal16++ = htons(m_Eep.spectrum_offest_x10[DL]);
			*pVal16++ = htons(m_Eep.spectrum_offest_x10[UL]);
			break;
		
		case 0x6E:	//// 2019.08.12 : PAU EQ State
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++)
				*pVal8++ = m_St.Pwr_Amp_EQ[l];
			break;

		case 0x6F:
			flag[0] = 0xFF;
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_DL; l++)
					*pVal32++ = htonl(m_Eep.TSSI_Dec_Offset[c][l]);
			}
			break;

		case 0x70: // 2019.08.12
			flag[0] = 0x80;
			*pVal8 = m_Eep.AAU_CW_Auto_Onoff; // 2020.01.20
			break;

		case 0x71:	// 2019.08.22, Delay 제어 수신 값
			flag[0] = 0xF0;
			*pVal32++ = htonl(m_St.LS_Dly_Ctrl_ns[_100M][DL]);
			*pVal32++ = htonl(m_St.LS_Dly_Ctrl_ns[_100M][UL]);
			*pVal32++ = htonl(m_St.LS_Dly_Ctrl_ns[Alpha][DL]);
			*pVal32++ = htonl(m_St.LS_Dly_Ctrl_ns[Alpha][UL]);
			break;
			
		case 0x72: // 2019.08.23, Hidden Version
			flag[0] = 0x80;
			*pVal8 = m_St.Int_Ver[0];
			break;

		case 0x73: // AAU Only
			flag[0] = 0xFF;
			for(c = 0; c < MAX_Optic; c++)
			{
				for(l = 0; l < MAX_DL; l++)				
					*pVal32++ = htonl(m_St.d_DL_TSSI_dec[c][l]); // AAU Only
			}
			break;

		case 0x74:	// 2019.08.22
			flag[0] = 0xFC;
			for(c = 0; c < MAX_Optic; c++)
				for(l=0; l<LSH_Max; l++)			
					*pVal16++ = htons(m_Eep.d_t_pattern_gain_x10[c][l]);
			
			break;			

		case 0x75: // 2019.08.23, Delay 제어 수신 return 값
			flag[0] = 0x80;
			*pVal32 = htonl(m_St.LS_Dly_Ret32);
			break;				
			
		case 0x76: // 2019.08.23
			flag[0] = 0x80;
			*pVal8 = m_Eep.LS_Gain_Bal_OnOff;
			break;

	//---------------
	#ifdef __TYPE__PRU__	
	//---------------
		case 0x77:				
		case 0x78:
		case 0x79:
		case 0x7A:		
			     if(paid2->AID[1] == 0x77) c = BW_140;
			else if(paid2->AID[1] == 0x78) c = BW_160;
			else if(paid2->AID[1] == 0x79) c = BW_180;
			else                           c = BW_200;
			
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.BW_Gain_Comp_PRU_x10[c][l]);
			break;
			
		case 0x80: // 2020.02.14 : PSU Maker Mode
			flag[0] = 0x80;
			memcpy(pVal8, PSU_Info.Maker_Mode, 4);
			break;			
			
		case 0x81: // 2020.02.14 : PSU Compatiblity
			flag[0] = 0x80;
			*pVal8 = m_PSU.Compatible;
			break;
	//---------------	
	#else // AAU
	//---------------
		case 0x7B:	// 2019.09.16
			flag[0] = 0xF0;
			for(l = 0; l < MAX_DL; l++)
				*pVal16++ = htons(m_Eep.d_CFR_Offset_x10[l]);			
			break;

		case 0x7C:	// 2019.09.16
			flag[0] = 0xE0;
			for(c=0; c<LSH_Max; c++)
				*pVal16++ = htons(m_Eep.d_DL_Det_SSB_Frame[c]);
			break;
			
		case 0x7D:	// 2019.09.16
			flag[0] = 0xE0;
			for(c=0; c<LSH_Max; c++)
				*pVal16++ = htons(m_Eep.d_DL_Det_SSB_Start[c]);
			break;
			
		case 0x7E:	// 2019.09.16
			flag[0] = 0xE0;
			for(c=0; c<LSH_Max; c++)
				*pVal16++ = htons(m_Eep.d_DL_Det_SSB_Lngth[c]);
			break;

		case 0x7F: // ATTEN(LNA)(UL), 2020.02.13
			flag[0] = 0xF0;
			for(l = 0; l < AMPU_4_MAX; l++) 
				*pVal16++ = htons(conv_att_025_to_001(m_St.UL_Att[l])/10);
			break;

		case 0x90: // L/S(Samsung) PLL Set, 2020.04.02
		case 0x91: 
		case 0x92: 
			     if(paid2->AID[1]==0x90) c = LSH_SS;
			else if(paid2->AID[1]==0x91) c = LSH_NK;
			else                         c = LSH_ER;
			
			flag[0] = 0xF0;
			for(l = 0; l < 4; l++) // 4-cascade
				*pVal8++ = m_Eep.d_Reg_PLL_Set[c][l];
			break;
			
	//---------------		
	#endif	
	//---------------
		case 0xA0: // 2020.06.09
			flag[0] = 0x80;
			memcpy(pVal8, m_ping_test.ip, 4);
			break;

		case 0xA1: // 2020.06.09
			flag[0] = 0x80;
			*pVal16 = htons(m_ping_test.cnt16);
			break;

		case 0xA2: // 2020.06.09
			flag[0] = 0x80;
			*pVal16 = htons(m_ping_test.size16);
			break;

		case 0xA3: // 2020.06.09
			flag[0] = 0x80;
			*pVal8 = m_ping_test.on;
			break;	

		case 0xA4: // 2020.06.09
			flag[0] = 0x80;
			Make_AID2_STR_Header_Var_Size(paid2, idx, &flag_len, strlen(m_ping_test.result));
			memcpy(pVal8, m_ping_test.result, 255);
			break;

		case 0xA5: // 2020.07.15 : DDR-Memory Clock info AID(0xC0A5) Added
			flag[0] = 0x80;
			memcpy(pVal8, m_St.DDR_Mem_Clk_Info, 20);
			break;			
			
		case 0xC0: // 2019.08.06, Factory Mode On/Off
			flag[0] = 0x80;
			*pVal8 = m_Env.Factory_Onoff;
			break;

		case 0xF0: 
			flag[0] = 0x80;
			*pVal16 = htons(m_Env.debug__cmd); 
			break;
			
		case 0xF1:
				if(m_Env.debug__cmd < dbg___MAX)
				{
					flag[0] = 0xFF;
					for(c = 0; c < dbg_2MAX; c++)
					{
						for(l = 0; l < dbg_4MAX; l++)
							*pVal32++ = htonl(m_Env.debug_data[m_Env.debug__cmd][c][l]);
					}
				}
				break;

		case 0xFE:
			flag[0] = 0;
			for(c=0; c<DL_UL; c++)
			{
				for(l=0; l < MAX_MIMO_PATH; l++)
					*pVal8++ = f_DTU_Path_OnOff[FA_0][c][l];
			}
			break;
			
		case 0xFF:
			flag[0] = 0;			
			for(c=0; c<DL_UL; c++)
			{
				for(l=0; l < MAX_MIMO_PATH; l++)
					*pVal8++ = f_DTU_Path_OnOff[FA_1][c][l];
			}
			break;
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}

//// YCSO added 190728 START
INT16U Make_AID_Internal_CF(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

//	INT32U* pVal32;
//	INT16U* pVal16;
	INT8U* pVal8;
		
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);
	
	pVal8 = &paid2->data[flag_len];
//	pVal16 = (INT16U*)&paid2->data[flag_len];
//	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		case 0x01:
			flag[0] = 0x80; 		
			memcpy(	pVal8	, m_St.kst_inventory_SN		,	4);
			break;

		case 0x02:
			flag[0] = 0x80; 		
			memcpy(	pVal8	, m_St.kst_inventory_date	,	3);
			break;
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}
//// YCSO added 190728 END


INT16U Make_AID_Internal_D4(void* pAID, INT16U idx, INT8U port, INT8U arg)
{
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	(void)arg;	(void)port;

//	INT32U* pVal32;
	INT16U* pVal16;
//	INT8U* pVal8;
		
	INT8U flag[MAX_AID2_Flag_Size];
	INT8U flag_len;
	
	Make_AID2_STR_Header(paid2, idx, &flag_len);
	memset(flag, 0, MAX_AID2_Flag_Size);
	
//	pVal8 = &paid2->data[flag_len];
	pVal16 = (INT16U*)&paid2->data[flag_len];
//	pVal32 = (INT32U*)&paid2->data[flag_len];
	
	switch(paid2->AID[1])
	{
		case 0x54:
			flag[0] = 0x80;
			*pVal16 = htons(m_St.Log_Cnt);
			break;
		
		default: return 0; // 항목 없는 경우 return 0
	}
	memcpy(paid2->data, flag, flag_len);	
	return 1;
}




//---------------------------------------------------------------------------------------------
// 호출 안하고 있음.
INT16U Make_AID( INT8U aid0, INT8U aid1, INT16U idx, void* pAID, INT8U port, INT8U arg )
{	
	AIDSTR*  paid  = (AIDSTR*)pAID;
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	INT8U is_send = 0;	// default 0 
	INT16U len;
	INT8U is_aid2_type;

	//-------------------------------
	if(Is_AID2_Range(aid0))
	{	
		paid2->AID[0] = aid0;
		paid2->AID[1] = aid1;
		paid2->len = 1;
		is_aid2_type = 1;

		//D_Printf(DEBUG_SYSTEM, " [AID2] 0x%02X-%02X -1-\n", aid0, aid1);
	}
	else
	{
		paid->AID[0] = aid0;
		paid->AID[1] = aid1;
		paid->len = 1;
		is_aid2_type = 0;

		//D_Printf(DEBUG_SYSTEM, " [AID1] 0x%02X-%02X -1-\n", aid0, aid1);
	}
	//-------------------------------	
	is_send = 0;
	switch(aid0)
	{
		case 0x00: is_send = Make_AID_0x00__( pAID, idx, port, arg ); break;
		case 0x01: is_send = Make_AID_0x01__( pAID, idx, port, arg ); break;
		case 0x02: is_send = Make_AID_0x02__( pAID, idx, port, arg ); break;
		case 0x09: is_send = Make_AID_0x09__( pAID, idx, port, arg ); break;		

		case 0x2B: is_send = Make_AID_0x2B__( pAID, idx, port, arg ); break;
		
		case 0x50: is_send = Make_AID_0x50__( pAID, idx, port, arg ); break;
		case 0x51: is_send = Make_AID_0x51__( pAID, idx, port, arg ); break;		
		case 0x52: is_send = Make_AID_0x52__( pAID, idx, port, arg ); break;
		case 0x53: is_send = Make_AID_0x53__( pAID, idx, port, arg ); break;
		case 0x54: is_send = Make_AID_0x54__( pAID, idx, port, arg ); break;
		case 0x55: is_send = Make_AID_0x55__( pAID, idx, port, arg ); break;
		case 0x56: is_send = Make_AID_0x56__( pAID, idx, port, arg ); break;

		case 0xB0: is_send = Make_AID_Internal_B0( pAID, idx, port, arg ); break;
		case 0xB1: is_send = Make_AID_Internal_B1( pAID, idx, port, arg ); break;
		case 0xB2: is_send = Make_AID_Internal_B2( pAID, idx, port, arg ); break;
		case 0xB3: is_send = Make_AID_Internal_B3( pAID, idx, port, arg ); break;
		case 0xB4: is_send = Make_AID_Internal_B4( pAID, idx, port, arg ); break;
		case 0xB5: is_send = Make_AID_Internal_B5( pAID, idx, port, arg ); break;
		case 0xB6: is_send = Make_AID_Internal_B6( pAID, idx, port, arg ); break;

		case 0xC0: is_send = Make_AID_Internal_C0( pAID, idx, port, arg ); break;
		case 0xCf: is_send = Make_AID_Internal_CF( pAID, idx, port, arg ); break;	//// YCSO added 190728
		case 0xD4: is_send = Make_AID_Internal_D4( pAID, idx, port, arg ); break;		
		case 0xF2: is_send = Make_AID_Internal_F2( pAID, idx, port, arg ); break;
		
		default: return 0;
	}
	
	if( is_send )
	{
		if(is_aid2_type)
			len = ntohs(paid2->len) + AID2_AID_LEN_Size;
		else
			len = paid->len + 3;
	}
	else len = 0;
	
	return len;
}

//---------------------------------------------------------------------------------------------
INT16U GetStatus( INT8U* sdu, INT8U port )
{
	INT16U l=0;	

	//D_Printf(DEBUG_SYSTEM, " [GetStatus] GUI_ST_AID_num %d --\r\n", GUI_ST_AID_num);
	l = Get_GUI_AID_Val( GUI_ST_AID_idx, GUI_ST_AID_num, sdu, port, _ADD_EMS_ALARM_ );

	return l;
}

INT16U GetStatus_REMS_Hidden( INT8U* sdu, INT8U port )
{
	INT16U l=0;	

	D_Printf(DEBUG_SYSTEM, " [GetStatus] Hidden_AID_num %d --\r\n", Hidden_AID_num);
	l = Get_GUI_AID_Val( Hidden_AID_idx, Hidden_AID_num, sdu, port, _ADD_EMS_ALARM_ );

	return l;
}

//--------------------------------------------------------------------------
/*
	Tree : 0xB00F

	Data type: 0x09(pre-defined)
	Data Step: 0x00(미사용)

	1번째 byte: Reserved
	2번째 byte: 1st PRU/AAU Status
	3번째 byte: 2nd PRU/AAU Status
	4번째 byte: 3rd PRU/AAU Status
	5번째 byte: 4rd PRU/AAU Status
	6번째 byte: GUI 연결 PRU/AAU Cascade ID (#0 ~ #3)

	PRU/AAU Statsu
	0x00: Not Equip
	0x01: Equip
	0x02: Equip(Alarm) --> 미지원
	0x03: Equip(Link 이상) --> 미지원
*/

// TREE_2019_0521
// return total RU counte @ current branch
INT8U Get_RU_Cnt(void)
{
	INT8U ru_cnt, c, k;
	INT8U c_id = AufoConfigFlag.RE_Cascade_ID;
	
	ru_cnt = c_id+1; // include self-RU

	if(c_id < 3)
	{
		if(m_St.Ref_CPRI_No == CPRI_0) 
			 c = CPRI_2;
		else c = CPRI_3;	
		
		for(k = (c_id+1); k < 4; k++) // from next RU
		{
			if(m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[k].uD32 == 0xCC) ru_cnt++;
			if(m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[k].uD32 == 0x33) ru_cnt++;	
		}		
	}
	return ru_cnt;
}
// END : TREE_2019_0521


// TREE_2019_0521
INT8U Make_Tree_Info( INT8U* sdu, INT8U aid_needed )
{
	INT8U i = 0;
	INT8U tree_data[6];
	INT8U my_ru_id;
	INT8U ru_cnt;
	INT8U k;

	i = 0; // init
	////if(aid_needed) // when topology@MiBOS, 'aid_needed' is 0
	{
		//----------------------
		sdu[i++] = 0xB0;
		sdu[i++] = 0x0F;
		sdu[i++] = 0;  // length
		sdu[i++] = 13; // length : 7+6
		//----------------------
		sdu[i++] = 0x09; // Data type: 0x09(pre-defined)
		sdu[i++] = 0x00; // Data Step: 0x00(미사용)
		sdu[i++] = 0; // Reserved
		sdu[i++] = 6; // 1개 Data 길이
		sdu[i++] = 1; // 항목 개수
		sdu[i++] = 1; // Carrier 개수
		sdu[i++] = 0x80; // Flag
		//----------------------
	}	

	// check sum-alarm
	
	memset(tree_data, 0, 6);	
	ru_cnt = Get_RU_Cnt();

	for(k=0;k<ru_cnt;k++)
		tree_data[1+k] = 0x01;
	
	my_ru_id = Get__My_RU_ID();
	tree_data[my_ru_id+1] = 0x01; // tree status
	tree_data[5] = my_ru_id; // current conneted ru-id
	
	memcpy(&sdu[i], tree_data, 6);
	i+=6;
	
	return i;
}
// END : TREE_2019_0521

//=======================================================================================




//=======================================================================================

void Get_AID(INT16U idx, INT8U* pAID_0, INT8U* pAID_1)
{
	*pAID_0 = skt_aid_list[idx].aid_0;
	*pAID_1 = skt_aid_list[idx].aid_1;
}
//----------------------------------------------------------------------
INT16U Make_SKT_AID( INT16U idx, void* pAID, INT8U port )
{
	INT8U is_send;
	INT8U AID_Major;	
	
	AIDSTR*  paid  = (AIDSTR*)pAID;
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	
	INT16U len;
	INT8U is_aid2_type;
	INT8U AID[2];	

	//-------------------------------
	AID[0] = skt_aid_list[idx].aid_0;
	AID[1] = skt_aid_list[idx].aid_1;	
	AID_Major = AID[0];
	
	if(Is_AID2_Range(AID[0]))
	{
		paid2->AID[0] = AID[0];
		paid2->AID[1] = AID[1];
		paid2->len = 1; // default
		is_aid2_type = 1;
	}
	else
	{
		paid->AID[0] = AID[0];
		paid->AID[1] = AID[1];		
		paid->len = 1;
		is_aid2_type  = 0;
	}
	//-------------------------------	

	if((AID[0] == 0xFF) && (AID[1] == 0xFF)) return 0;
	if((AID[0] == 0x00) && (AID[1] == 0x00)) return 0;	
	if(ems_aid_func[AID_Major].aid_func == NULL) return 0;

	is_send = ems_aid_func[AID_Major].aid_func( pAID, idx, port, 0 );	

	if( is_send )
	{
		if(is_aid2_type)
			len = ntohs(paid2->len) + AID2_AID_LEN_Size;
		else
			len = paid->len + 3;
	}
	else len = 0;
	
	return len;
}
//----------------------------------------------------------------------
INT16U Get__GUI_AID(INT16U idx)
{
	INT16U aid16;

	if(idx >= Tot_AID_Num) return 0xFFFF;

	aid16 = skt_aid_list[idx].aid_0; aid16 <<= 8;
	aid16 |= skt_aid_list[idx].aid_1;

	return aid16;
}

INT16U Make_GUI_AID( INT16U idx, void* pAID, INT8U port )
{
	INT8U is_send;
	INT8U AID_Major;
	
	AIDSTR*  paid  = (AIDSTR*)pAID;
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	
	INT16U len = 0;
	INT8U is_aid2_type;
	INT8U AID[2];	

	//-------------------------------
	AID[0] = skt_aid_list[idx].aid_0;
	AID[1] = skt_aid_list[idx].aid_1;
	AID_Major = AID[0];
	//-------------------------------
	if(Is_AID2_Range(AID[0]))
	{
		paid2->AID[0] = AID[0];
		paid2->AID[1] = AID[1];
		paid2->len = 1; // default
		is_aid2_type = 1;
		//D_Printf(DEBUG_SYSTEM, " [Is_AID2_Range] 0x%02X-%02X --\r\n", AID[0], AID[1]);
	}
	else
	{
		paid->AID[0] = AID[0]; 
		paid->AID[1] = AID[1];
		paid->len = 1;
		is_aid2_type = 0;
		//D_Printf(DEBUG_SYSTEM, " [Is_AID] 0x%02X-%02X --\r\n", AID[0], AID[1]);
	}
	//-------------------------------
	if((AID[0] == 0xFF) && (AID[1] == 0xFF)) return 0;
	if((AID[0] == 0x00) && (AID[1] == 0x00)) return 0;
		
	if(ems_aid_func[AID_Major].aid_func == NULL) 
	{
		//D_Printf(DEBUG_SYSTEM, " [Is_AID2_Range] 0x%02X-%02X return. --\r\n", AID[0], AID[1]);
		return 0;
	}
	
	is_send = ems_aid_func[AID_Major].aid_func( pAID, idx, port, 0 );	
	
	if( is_send )
	{
		if(is_aid2_type)
			len = ntohs(paid2->len) + AID2_AID_LEN_Size;
		else
			len = paid->len + 3;
	} 
	else 
	{
		//D_Printf(DEBUG_SYSTEM, " [Is_AID2_Range] 0x%02X-%02X len = 0. --\r\n", AID[0], AID[1]);
		len = 0;
	}
	
	return len;
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
INT16U Get_Log_AID_List(INT16U* pAid_list)
{	
	/*
		국소명	   : 0xB011
		LS등록번호 : 0xB007
		LS ID	   : 0x500C
		REC Port No: 0x5006
		Cascade ID : 0xB002
	*/
	INT8U aid[Log_IDX_MAX][2] = {{0xB0,0x11},{0xB0,0x07},{0x50,0x0C},{0x50,0x06},{0xB0,0x02},{0xD4,0x54}};
	INT16U i, k, m;


	for(k=0, i=0; i<Tot_AID_Num; i++)
	{
		if((skt_aid_list[i].aid_0 == 0xFF)&&(skt_aid_list[i].aid_1 == 0xFF))
			break;
		
		for(m=0; m < Log_IDX_MAX; m++)
		{
			if((aid[m][0] == 0xFF)&&(aid[m][1] == 0xFF)) // if found
				continue;			
			
			if((skt_aid_list[i].aid_0 == aid[m][0]) && (skt_aid_list[i].aid_1 == aid[m][1]))
			{
				print_agent(DEBUG_SYSTEM, VT_CYAN, "[Get_Log_AID_List] (%d) AID 0x%02X%02X(index %d)",  
								k, aid[m][0], aid[m][1], i);
								
				aid[m][0] = 0xFF;
				aid[m][1] = 0xFF;

				if(k < Log_IDX_MAX)				
					pAid_list[k++] = i;
				break;
			}			
		}

		if(k >= Log_IDX_MAX)
			break;
	}

	pAid_list[k] = 0xFFFF; // End of AID List
	return k;
}


//----------------------------------------------------------------------
INT16U Get_SKT_AID_List( INT8U dev_f, INT8U attr, INT16U* pAid_list, INT16U max_num )
{
	INT16U i, k;

	for( k=0, i=0; i < Tot_AID_Num; i++ )
	{
		//-------------------------------------------
		if((skt_aid_list[i].aid_0 == 0xFF) && (skt_aid_list[i].aid_1 == 0xFF))
			break;

		if( k >= ( max_num-1 ))
			break;
		//-------------------------------------------
		if(dev_f == 0) continue;
		if(attr == 0)  continue;		
		//-------------------------------------------
		if((skt_aid_list[i].attr & attr) != attr) continue;
		if((skt_aid_list[i].dev_f & dev_f) != dev_f) continue;
		//-------------------------------------------

		//-------------------------------------------
		pAid_list[k++] = i;
	}
	pAid_list[k] = 0xFFFF; // End of AID List

	return k;
}
//----------------------------------------------------------------------
void Get_Tot_AID_NUM(void)
{
	INT16U i, k;

	for( k=0, i=0; i<MAX_AID_NUM; i++ )
	{
		if((skt_aid_list[i].aid_0 == 0xFF) && (skt_aid_list[i].aid_1 == 0xFF)) break;
		else k++;
	}
	Tot_AID_Num = k;
	D_Printf(DEBUG_SYSTEM, " [Tot_AID_Num] %d --\r\n", Tot_AID_Num);
}
//----------------------------------------------------------------------
INT16U Get_GUI_AID_Val( INT16U* pAid_list, INT16U max_num, INT8U* sdu, INT8U port, INT8U alarm_add )
{
	INT16U l=0;
	INT16U k, idx;
	(void)alarm_add;
	
	//-----------------------------------------------------
	l = 0; // init
	//-----------------------------------------------------
	for( k=0; k < max_num; k++ )
	{
		idx = pAid_list[k];

		if (idx == 0xFFFF) break;

		l += Make_GUI_AID( idx, &sdu[l], port );		
	}
	return l;
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
INT16U Get_SKT_AID_2B( INT16U idx )
{
	INT16U k;

	k = skt_aid_list[idx].aid_0;
	k <<=8;
	k |= skt_aid_list[idx].aid_1;

	return k;
}
//----------------------------------------------------------------------
//
void Get_Total_AID_LIST(void)
{
	INT8U type;
	
	Get_Tot_AID_NUM();	
	
#ifdef __TYPE__PRU__
	type = _PRU_;
#else
	type = _AAU_;
#endif
	
	Meas___AID_num = Get_SKT_AID_List( type, _Measure_, Meas___AID_idx, Meas_IDX_MAX ); // Mea 항목,
	CFG__C_AID_num = Get_SKT_AID_List( type, _CF_CTRL_, CFG__C_AID_idx, CF_C_IDX_MAX ); // CF__C 인 항목,	
	CFG_nC_AID_num = Get_SKT_AID_List( type, _CF_nCTL_, CFG_nC_AID_idx, CFnC_IDX_MAX ); // CFG_nC
	GUI_ST_AID_num = Get_SKT_AID_List( type, ___GUI___, GUI_ST_AID_idx, GUI__IDX_MAX ); // GUI 상태 항목.
	Hidden_AID_num = Get_SKT_AID_List( type, __Hidden_, Hidden_AID_idx, Hidden_IDX_MAX ); // Hidden 항목.

	print_agent(DEBUG_SYSTEM, VT_CYAN, "[GetStatus] Get_Total_AID_LIST Mea %d, CF/C %d, CF/nC %d, GUI %d, Hidden %d", 
			Meas___AID_num, 
			CFG__C_AID_num, 
			CFG_nC_AID_num, 
			GUI_ST_AID_num,
			Hidden_AID_num);

	LOG___AID_num = Get_Log_AID_List(LOG___AID_idx);
}
//----------------------------------------------------------------------



INT8U Is_AID2_Range(INT8U aid0)
{
	if(aid0 < 0x50) return 0; // AID-legacy	
	else            return 1; // AID2-Main
}
//----------------------------------------------------------------------
// 미사용
INT16U Make_AID_Str_Reason( INT8U aid0, INT8U aid1, INT8U* pData, INT8U alm_f )
{	
	AIDSTR*  paid  = (AIDSTR*)pData;
	AIDSTR2* paid2 = (AIDSTR2*)pData;	
	
	INT8U alarm = AMP_OFF_REASON_Flag;
	INT16U len;
	INT8U is_aid2_type;
	INT16U idx;

	if(pData == NULL) return 0;
	if((aid0 == 0xFF) && (aid1 == 0xFF)) return 0;
	if(ems_aid_func[aid0].aid_func == NULL) return 0;

	//-------------------------------
	if(Is_AID2_Range(aid0))	
	{
		paid2->AID[0] = aid0;
		paid2->AID[1] = aid1;
		paid2->len = 1; // default
		is_aid2_type = 1;
	}
	else
	{	paid->AID[0] = aid0;
		paid->AID[1] = aid1;
		paid->len = 1;
		is_aid2_type  = 0;
	}
	//-------------------------------

	if(alm_f) alarm |= 0x01;

	idx = 0; 
	// idx = search_skt_aid_list_index(...);
	if (ems_aid_func[aid0].aid_func( pData, idx, 0, alarm ) == 0) 
		return 0;
	else 
	{
		if(is_aid2_type)
			len = ntohs(paid2->len) + AID2_AID_LEN_Size;
		else
			len = paid->len + 3;
			
		return len;
	}
}

//---------------------------------------------------------------------------------------------
// AID 에 해당하는 상태 값을 pData에 AID 형식으로 Write
// 미사용.
INT16U Make_gREMS_Alarm_AID_Struct(INT8U aid0, INT8U aid1, INT8U* pData)
{
	AIDSTR*  paid  = (AIDSTR*)pData;
	AIDSTR2* paid2 = (AIDSTR2*)pData;
	
	INT16U len;
	INT8U is_aid2_type;
	INT16U idx;

	if(pData == NULL) return 0;
	if((aid0 == 0xFF) && (aid1 == 0xFF)) return 0;	
	if(ems_aid_func[aid0].aid_func == NULL) return 0;
	
	//-------------------------------
	if(Is_AID2_Range(aid0))
	{	
		paid2->AID[0] = aid0;
		paid2->AID[1] = aid1;
		paid2->len = 1;	// default
		is_aid2_type = 1;
	}
	else
	{
		paid->AID[0] = aid0;
		paid->AID[1] = aid1;
		paid->len = 1;	
		is_aid2_type = 0;
	}
	//-------------------------------
	idx = 0; 
	// idx = search_skt_aid_list_index(...);
	if(ems_aid_func[aid0].aid_func(pData, idx,  0, 0)) // success
	{
		if(is_aid2_type)
			len = ntohs(paid2->len) + AID2_AID_LEN_Size;
		else
			len = paid->len + 3;
	}	
	else len = 0;
	return len;	 
}

// AID 에 해당하는 상태 값을 pData에 AID 형식으로 Write
// 미사용.
INT16U Make_AID_Struct( INT8U aid0, INT8U aid1, INT8U* pData )
{
	AIDSTR*  paid  = (AIDSTR*)pData;
	AIDSTR2* paid2 = (AIDSTR2*)pData;
	
	INT16U len;
	INT8U is_aid2_type;
	INT16U idx;

	if(pData == NULL) return 0;
	if((aid0 == 0xFF) && (aid1 == 0xFF)) return 0;	
	if(ems_aid_func[aid0].aid_func == NULL) return 0;
	
	//-------------------------------
	if(Is_AID2_Range(aid0))
	{	
		paid2->AID[0] = aid0;
		paid2->AID[1] = aid1;
		paid2->len = 1;	// default
		is_aid2_type = 1;
	}
	else
	{
		paid->AID[0] = aid0;
		paid->AID[1] = aid1;
		paid->len = 1;	
		is_aid2_type = 0;
	}
	//-------------------------------
	idx = 0; 
	// idx = search_skt_aid_list_index(...);
	if(ems_aid_func[aid0].aid_func(pData, idx,  0, 0)) // success
	{
		if(is_aid2_type)
			len = ntohs(paid2->len) + AID2_AID_LEN_Size;
		else
			len = paid->len + 3;
	}	
	else len = 0;
	return len;	 
}

//---------------------------------------------------------------------------------------------



#define Print_AID2_Max (512)
void Print_AID2(char* title, AIDSTR2* paid2, AID2_INFO* paid2_info, char* disp_str, INT16U max_buff)
{
	INT16U i;
	INT16U data_len = paid2_info->data_len;
	INT16U flag_len = paid2_info->flag_len;
	char  str[Print_AID2_Max];
	char  log_str[Print_AID2_Max];

	disp_str[0] = 0;	

	sprintf(str, "%s AID 0x%02X%02X", 
		title, 
		paid2->AID[0], 
		paid2->AID[1]); 
	strcpy(log_str, str);
	strcat(disp_str, str);
	strcat(disp_str, "\n");	
		
	sprintf(str, "\t Type, Step, Rsrv : 0x%02X, 0x%02X, 0x%02X\n", 
		paid2->type, paid2->step,  paid2->reserved);
	strcat(disp_str, str);	
	
	sprintf(str, "\t Size, Layr, Carr : 0x%02X, 0x%02X, 0x%02X\n",  
		paid2->size, paid2->layer, paid2->carrier);	
	strcat(disp_str, str);	

	sprintf(str, "\t Type ~ Data : %d byte\n", paid2_info->aid2_len);
	strcat(disp_str, str);	
	
	sprintf(str, "\t Flag(%d) + Data(%d) :", flag_len, data_len);
	strcat(disp_str, str);
	
	if((3*data_len + 3*flag_len + strlen(disp_str)) < Print_AID2_Max) 
	{		
		for(i = 0; i < flag_len; i++)
		{
			sprintf(str, " %02X", paid2->data[i]);
			strcat(disp_str, str);			
		}		
		strcat(disp_str, " |");
		for(i = 0; i < data_len; i++)
		{
			sprintf(str, " %02X", paid2->data[i+flag_len]);
			strcat(disp_str, str);
			strcat(log_str, str);
		}		
	}

	D_Printf(DEBUG_SYSTEM, "%s\n", disp_str);
	Wr__Event_to_LogFile("%s", log_str);
}

/*
----------------------------------------------------------
	2019.10.15
	Display TSSI every 10[s]
----------------------------------------------------------	
*/


void Proc__Chk_and_Disp__TSSI_Level(void)
{
#ifdef __USE_TSSI_LOG__
	static INT8U disp_req = 0;
	char reg_num_str[SN_SIZE+1];
	double d_tssi[MAX_DL];
	double d_out_pwr[MAX_DL];
	INT8U i;
	
	for(i = 0; i < MAX_DL; i++)
	{
		if(disp_req & cbits[i])
		{
			disp_req &= nbits[i]; // clear for next-display
		}
		else
		{
			if(m_St.d_DL_TSSI_dBm_x10[_100M][i] > -200) 
				disp_req |= cbits[i];
		}
	}

#ifdef __disable_frequent_debug__	
	disp_req = 0;
#endif

	if(disp_req)
	{
		memcpy(reg_num_str, m_St.LS_Regist_Num, SN_SIZE);	
		reg_num_str[SN_SIZE] = 0;

		for(i = 0; i < MAX_DL; i++)
		{
			d_tssi[i] = (double)m_St.d_DL_TSSI_dBm_x10[_100M][i]/10.0;
			d_out_pwr[i] = (double)m_St.Fwd_Out_Pwr_x10[i]/10.0;
		}	
		
		print_agent(DEBUG_SYSTEM, VT_CYAN, "[LS-RegNum %s]-TSSI [dBm] %.1f, %.1f, %.1f, %.1f TSSI [dBm] %.1f, %.1f, %.1f, %.1f TSSI-High %.1f, %.1f, %.1f, %.1f", 
						reg_num_str, 
						d_tssi[0], d_tssi[1], d_tssi[2], d_tssi[3],
						d_tssi[0], d_tssi[1], d_tssi[2], d_tssi[3],
						d_out_pwr[0], d_out_pwr[1], d_out_pwr[2], d_out_pwr[3]);		
	}
#endif	
}


void Proc__Chk_and_Disp__Out_Upper_Alarm(void)
{
#ifdef __USE_TSSI_LOG__
	static INT8U alm_prev[MAX_DL] = {0,0,0,0};
	char reg_num_str[SN_SIZE+1];
	double d_tssi[MAX_DL];
	double d_out_pwr[MAX_DL];	
	INT8U disp_req = 0; // default
	INT8U i;

	for(disp_req = 0, i = 0; i < MAX_DL; i++)
	{
		if(alm_prev[i] != m_St.AlmSt.Path[i].DL.Out_Upper) 
		{
			if(m_St.AlmSt.Path[i].DL.Out_Upper) // if alarm 0 -> 1
				disp_req |= cbits[i];
		}
		alm_prev[i] = m_St.AlmSt.Path[i].DL.Out_Upper;
	}
	
	#ifdef __disable_frequent_debug__	
		disp_req = 0;
	#endif

	if(disp_req)
	{
		memcpy(reg_num_str, m_St.LS_Regist_Num, SN_SIZE);	
		reg_num_str[SN_SIZE] = 0;

		for(i = 0; i < MAX_DL; i++)
		{
			d_tssi[i] = (double)m_St.d_DL_TSSI_dBm_x10[_100M][i]/10.0;
			d_out_pwr[i] = (double)m_St.Fwd_Out_Pwr_x10[i]/10.0;
		}
		
		print_agent(DEBUG_SYSTEM, VT_CYAN, "[LS-RegNum %s]-TSSI [dBm] %.1f, %.1f, %.1f, %.1f TSSI [dBm] %.1f, %.1f, %.1f, %.1f OP-High   %.1f, %.1f, %.1f, %.1f", 
				reg_num_str, 
				d_tssi[0], d_tssi[1], d_tssi[2], d_tssi[3],
				d_tssi[0], d_tssi[1], d_tssi[2], d_tssi[3],
				d_out_pwr[0], d_out_pwr[1], d_out_pwr[2], d_out_pwr[3]);
	}
#endif	
}


#ifdef __USE_TSSI_LOG__
static OS_TIMER Timer_Disp_TSSI;
#endif

void Proc___Display_TSSI(void)
{
#ifdef __USE_TSSI_LOG__
	static INT8U init = 0;
	char reg_num_str[SN_SIZE+1];
	double d_tssi[MAX_DL];
	
	if(init == 0)
	{
		init = 1;
		OSTimerReset(&Timer_Disp_TSSI);
	}

	if(OSTimerGet_msec(&Timer_Disp_TSSI) < (10*TICK_CO_SEC))
		return;
	else
		OSTimerReset(&Timer_Disp_TSSI);
		
	memcpy(reg_num_str, m_St.LS_Regist_Num, SN_SIZE);	
	reg_num_str[SN_SIZE] = 0;

	d_tssi[0] = (double)m_St.d_DL_TSSI_dBm_x10[_100M][0]/10.0;
	d_tssi[1] = (double)m_St.d_DL_TSSI_dBm_x10[_100M][1]/10.0;
	d_tssi[2] = (double)m_St.d_DL_TSSI_dBm_x10[_100M][2]/10.0;
	d_tssi[3] = (double)m_St.d_DL_TSSI_dBm_x10[_100M][3]/10.0;

	#ifdef __disable_frequent_debug__
	#else
	print_agent(DEBUG_SYSTEM, VT_CYAN, "[LS-RegNum %s]-TSSI [dBm] %.1f, %.1f, %.1f, %.1f TSSI [dBm] %.1f, %.1f, %.1f, %.1f", 
				reg_num_str, 
				d_tssi[0], d_tssi[1], d_tssi[2], d_tssi[3],
				d_tssi[0], d_tssi[1], d_tssi[2], d_tssi[3]);	
	#endif			
#endif
}



/*
-----------------------------------------------------------------
2019.12.24 : Alarm timeout
-----------------------------------------------------------------
*/
void Init__Alarm_Timer(ALARM_TIMER* pAlmTimer)
{
	OSTimerReset(&pAlmTimer->alm_ms);
	OSTimerReset(&pAlmTimer->clr_ms);
}

void Init__All_Alarm_Timer(void)
{
	INT8U path4;	
	
	Init__Alarm_Timer(&m_St.AlmTime.TEST.test); // test

	Init__Alarm_Timer(&m_St.AlmTime.A_4.Clock);

	for(path4 = 0; path4 < MAX_PATH_4; path4++)
	{
		Init__Alarm_Timer(&m_St.AlmTime.Path[path4].DL.TSSI_Lower_a);
		Init__Alarm_Timer(&m_St.AlmTime.Path[path4].DL.TSSI_Lower__);
		Init__Alarm_Timer(&m_St.AlmTime.Path[path4].DL.TSSI_Upper_a);
		Init__Alarm_Timer(&m_St.AlmTime.Path[path4].DL.TSSI_Upper__);
	}
}

INT8U IS__alarm_timeout(ALARM_TIMER* pAlmTimer, INT32U timeout_ms)
{
	if(OSTimerGet_msec(&pAlmTimer->alm_ms) >= timeout_ms)
	{
		OSTimerReset(&pAlmTimer->alm_ms);
		return 1;
	}
	else return 0;
}

INT8U IS_normal_timeout(ALARM_TIMER* pAlmTimer, INT32U timeout_ms)
{
	if(OSTimerGet_msec(&pAlmTimer->clr_ms) >= timeout_ms)
	{
		OSTimerReset(&pAlmTimer->clr_ms);
		return 1;
	}
	else return 0;
}

void Rst__alarm_timeout(ALARM_TIMER* pAlmTimer)
{
	OSTimerReset(&pAlmTimer->alm_ms);
}

void Rst_normal_timeout(ALARM_TIMER* pAlmTimer)
{
	OSTimerReset(&pAlmTimer->clr_ms);
}

// return : 최종 alarm을 return
INT8U Chk_clock__alarm_normal_timeout(ALARM_TIMER* pAlmTimer, INT8U last_alm, INT8U tmp_alm ,INT32U time_alm, INT32U time_clr)
{
	if(last_alm && tmp_alm) // both alarm
	{
		Rst_normal_timeout(pAlmTimer);
		Rst__alarm_timeout(pAlmTimer);
		return last_alm;
	}
	else if((last_alm==0) && (tmp_alm==0)) // both normal
	{
		Rst_normal_timeout(pAlmTimer);
		Rst__alarm_timeout(pAlmTimer);
		return last_alm;
	}
	else // last=0/tmp=1, last=1/tmp=0 
	{	
		if(last_alm == 0) // normal -> alarm : to check alarm time, last=0/tmp=1
		{
			Rst_normal_timeout(pAlmTimer);
			if(IS__alarm_timeout(pAlmTimer, time_alm))
			{
				Rst__alarm_timeout(pAlmTimer);
				return 1; // alarm
			}
			/*
			//else
			//{
			//	print_agent(DEBUG_SYSTEM, VT_CYAN, "[Clock]-alarm time %d[ms], (last %d, tmp %d)", 
			//		OSTimerGet_msec(&pAlmTimer->alm_ms), last_alm, tmp_alm);
			//}
			*/
		}
		else // alarm -> normal :  now alarm, last=1/tmp=0 
		{
			Rst__alarm_timeout(pAlmTimer);
			if(IS_normal_timeout(pAlmTimer, time_clr))
			{				
				Rst_normal_timeout(pAlmTimer);
				return 0; // normal
			}
		}
		return last_alm;
	}
}

INT8U Chk__alarm_normal_timeout(ALARM_TIMER* pAlmTimer, INT8U last_alm, INT8U tmp_alm ,INT32U time_alm, INT32U time_clr)
{
	if(last_alm && tmp_alm)
	{
		Rst_normal_timeout(pAlmTimer);
		Rst__alarm_timeout(pAlmTimer);		
		return last_alm;
	}
	else if((last_alm==0) && (tmp_alm==0))
	{
		Rst_normal_timeout(pAlmTimer);
		Rst__alarm_timeout(pAlmTimer);		
		return last_alm;
	}
	else 
	{	
		if(last_alm == 0) // to check alarm time
		{
			Rst_normal_timeout(pAlmTimer);
			if(IS__alarm_timeout(pAlmTimer, time_alm))
			{
				Rst__alarm_timeout(pAlmTimer);
				return 1; // alarm
			}
		}
		else // now alarm
		{
			Rst__alarm_timeout(pAlmTimer);
			if(IS_normal_timeout(pAlmTimer, time_clr))
			{				
				Rst_normal_timeout(pAlmTimer);
				return 0; // normal
			}
		}
		return last_alm;
	}
}


//=======================================================================================
/* EOF */

