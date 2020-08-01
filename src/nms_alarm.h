/********************************************************************************/
#ifndef __NMS_ALARM_H
#define __NMS_ALARM_H
/********************************************************************************/
#ifdef  NMS_ALARM_GLOBALS
#define NMS_ALARM_EXT
#else
#define NMS_ALARM_EXT extern
#endif
/**************************************************************/



//-----------------------------------------------------
// EMS
//-------------------
#define BUF_SIZE (1000)
#define MAX_ALRM_QUE (1) // EMS (30) -> (1)
//-----------------------------------------------------

#define _ALM_SET_ (1)
#define _ALM_CLR_ (0)



typedef struct __attribute__((packed))
{	
	INT8U	data[BUF_SIZE+10];
	INT16U  len; // data 사용 길이.
} PROTOCOL_ARM_PKT;




NMS_ALARM_EXT PROTOCOL_ARM_PKT ARM_REP_PKT_Ro[MAX_ALRM_QUE];

/*----------------------------------------------------------*/
// 외부 호출 함수
//
NMS_ALARM_EXT void Check_Alarm( void );
NMS_ALARM_EXT void MakeResetAlarm( void );

NMS_ALARM_EXT void Make_Ro_Arm_Data( INT8U* data, INT16U len );
NMS_ALARM_EXT INT16U Check_Alm_Q( INT8U* pUp_data );
//-----------------------------------------------------



//-----------------------------------------------------
// EMS
//-----------------------------------------------------
typedef struct __attribute__((packed))
{	
	INT8U  cmd;
	INT8U  use; // 1 if used
	INT16U  len; // data 사용 길이.
	INT8U	data[BUF_SIZE+10];	

	INT8U  cur;
	INT8U  tot; 
	INT16U  seq16; 
} AUTO_REP_PKT; // alarm, suppress alarm, statics, control, config 자동보고를 위한 AID 구조체 버퍼.

#define MAX_AUTO_RPT_QUE (50) // 2020.04.13 : 30 -> 50
#define MAX_AUTO_LOG_QUE (10) // 2020.04.13

NMS_ALARM_EXT AUTO_REP_PKT Auto_Rep_Pkt[MAX_AUTO_RPT_QUE];
NMS_ALARM_EXT AUTO_REP_PKT Auto_Wr__Log[MAX_AUTO_LOG_QUE];

NMS_ALARM_EXT INT16U Auto_Rep_Pkt_Seq;
NMS_ALARM_EXT INT16U Auto_Wr__Log_Seq;
//-----------------------------------------------------------------



NMS_ALARM_EXT void Check_Suppress_Alm(void);

NMS_ALARM_EXT void Insert_Auto_Rpt_Q( INT8U* data, INT16U len, INT8U cmd, INT8U cur, INT8U tot );
NMS_ALARM_EXT void Insert_Auto_Log_Q( INT8U* data, INT16U len, INT8U cmd, INT8U cur, INT8U tot );

NMS_ALARM_EXT void Init_Statis_Q(void);
NMS_ALARM_EXT void Make_Statis_Q( INT8U* data_1st, INT16U len_1st, INT8U* data_2nd, INT16U len_2nd, INT8U cmd, INT8U idx, INT8U cur, INT8U tot );
NMS_ALARM_EXT INT8U Insert_Statis_Q(void);





NMS_ALARM_EXT void Check_Alarm_gREMS(  REMS_1024_Pkt* pPkt  );



NMS_ALARM_EXT void Make_CF_C_Auto_Report( INT8U pkt_idx );
NMS_ALARM_EXT void Make_CF_nC_Auto_Report( INT8U pkt_idx );


NMS_ALARM_EXT INT8U Is_Low_Alm_Mask_at_AMP_Init(INT8U path4);
NMS_ALARM_EXT void Init_Low_Alm_Mask_at_AMP_Init(INT8U path4);
NMS_ALARM_EXT void TimerStart_Low_Alm_at_AMP_Init(INT8U path4);
NMS_ALARM_EXT void Proc_LowAlm_Mask_at_AMP_Init(void);

//-----------------------------------------------------
typedef struct 
{	
	INT8U byte;
	INT8U bit;
	INT8U idx;
	INT8U reserve;
}RU_ALM_POSI;

//-----------------------------------------------------

#define UDA_ALM_MAX (2)

//---------------------------------------------------------------------------
#define _pru_ (0x01)
#define _aau_ (0x02)
#define __0__ (0)


#ifdef  NMS_ALARM_GLOBALS
// 2019.03.13 : PRU/AAU
NMS_ALARM_EXT const INT8U Alm_AID[RU_ALARM_CNT][8][3] = \
{
	{   // 0
		{0x50, 0x11, _pru_|__0__}, // 7 :  Door
		{0x50, 0x13, _pru_|_aau_}, // 6 :  온도 상한
		{0x50, 0x14, _pru_|_aau_}, // 5 :  FAN1 (FAN(함체))
		{0x50, 0x15, _pru_|__0__}, // 4 :  FAN2 (FAN(PSU))
		{0x50, 0x16, _pru_|__0__}, // 3 :  AC
		{0x50, 0x17, _pru_|_aau_}, // 2 :  DC
		{0x50, 0x10, _pru_|_aau_}, // 1 :  상태감시 접속
		{0xFF, 0xFF, __0__|__0__}, // 0 :  Reserved
	},
	{	// 1
		{0x50, 0x18, _pru_|__0__}, // 7 :  Battery Low
		{0x50, 0x1A, _pru_|__0__}, // 6 :  Battery Status
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 2
		{0x53, 0x01, _pru_|_aau_}, // 7 :  CPRI0
		{0x53, 0x01, _pru_|_aau_}, // 6 :  CPRI0(+α)
		{0x53, 0x01, _pru_|_aau_}, // 5 :  CPRI1
		{0x53, 0x01, _pru_|_aau_}, // 4 :  CPRI1(+α)
		{0x53, 0x02, _pru_|_aau_}, // 3 :  LD(CPRI0)
		{0x53, 0x02, _pru_|_aau_}, // 2 :  LD(CPRI0)(+α)
		{0x53, 0x02, _pru_|_aau_}, // 1 :  LD(CPRI1)
		{0x53, 0x02, _pru_|_aau_}, // 0 :  LD(CPRI1)(+α)
	},
	{	// 3
		{0x53, 0x04, _pru_|_aau_}, // 7 :  LOS(CPRI0)
		{0x53, 0x04, _pru_|_aau_}, // 6 :  LOS(CPRI0)(+α)
		{0x53, 0x04, _pru_|_aau_}, // 5 :  LOS(CPRI1)
		{0x53, 0x04, _pru_|_aau_}, // 4 :  LOS(CPRI1)(+α)
		{0x53, 0x15, _pru_|_aau_}, // 3 :  SFP_Ins_0__
		{0x53, 0x15, _pru_|_aau_}, // 2 :  SFP_Ins_0_a
		{0x53, 0x0C, _pru_|_aau_}, // 1 :  PD Power Upper Limit(SFP_0)
		{0x53, 0x0C, _pru_|_aau_}, // 0 :  PD Power Upper Limit(SFP_0_Alpha)
	},	
	{	// 4
		{0x53, 0x00, _pru_|_aau_}, // 7 :  Clock
		{0xFF, 0xFF, __0__|__0__}, // 6 :  --
		{0x53, 0x0C, _pru_|_aau_}, // 5 :  PD Power Upper Limit(SFP_1)
		{0x53, 0x0C, _pru_|_aau_}, // 4 :  PD Power Upper Limit(SFP_1_Alpha)
		{0x53, 0x09, _pru_|_aau_}, // 3 :  PD Power Lower Limit(SFP_0)
		{0x53, 0x09, _pru_|_aau_}, // 2 :  PD Power Lower Limit(SFP_0_Alpha)
		{0x53, 0x09, _pru_|_aau_}, // 1 :  PD Power Lower Limit(SFP_1)
		{0x53, 0x09, _pru_|_aau_}, // 0 :  PD Power Lower Limit(SFP_1_Alpha)
	},
	// 1st DL/UL ---------------------------------------------------
	{	// 5 ( 5, 10, 15, 20)
		{0x51, 0x01, _pru_|_aau_}, // 7 :  DL TSSI 상한
		{0x51, 0x01, _pru_|_aau_}, // 6 :  DL(+α) TSSI 상한
		{0x51, 0x04, _pru_|_aau_}, // 5 :  DL 출력 상한
		{0x51, 0x05, _pru_|_aau_}, // 4 :  DL 출력 하한
		{0x51, 0x02, _pru_|_aau_}, // 3 :  DL TSSI 하한
		{0x51, 0x02, _pru_|_aau_}, // 2 :  DL(+α)  TSSI 하한
		{0xFF, 0xFF, __0__|__0__}, // 1 :  Reserved
		{0xFF, 0xFF, __0__|__0__}, // 0 :  Reserved
	},
	{	// 6 (6, 11, 16, 21)
		{0x51, 0x10, _pru_|__0__}, // 7 :  Over Power
		{0x51, 0x13, _pru_|__0__}, // 6 :  High Temp
		{0x51, 0x11, _pru_|__0__}, // 5 :  VSWR
		{0x51, 0x17, _pru_|_aau_}, // 4 :  Disabled
		{0x51, 0x18, _pru_|__0__}, // 3 :  PAU Link(DL)
		{0x51, 0x14, _pru_|__0__}, // 2 :  Power Amp 전원(DC) 이상
		{0x51, 0x12, _pru_|__0__}, // 1 :  Device Fail
		{0x51, 0x15, _pru_|_aau_}, // 0 :  Shutdown
	},
	{	// 7 ( 7, 12, 17, 22)
		{0x53, 0x21, _pru_|__0__}, // 7 :  DPD
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 8 ( 8, 13, 18, 23)
		{0x52, 0x02, _pru_|_aau_}, // 7 : UL RSSI 하한
		{0x52, 0x02, _pru_|_aau_}, // 6 : UL(+α) RSSI 하한
		{0x52, 0x10, _pru_|_aau_}, // 5 : UL 과입력
		{0x52, 0x12, _pru_|__0__}, // 4 : UL LNA Fail
		{0x52, 0x13, __0__|_aau_}, // 3 : Gain 저하(UL)
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 9 (9 14, 19, 24)
		{0xFF, 0xFF, __0__|__0__}, // 7 :  
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	// 2nd DL/UL ---------------------------------------------------
	{	// 10 ( same with 5, 10, 15, 20)
		{0x51, 0x01, _pru_|_aau_}, // 7 :  DL TSSI 상한
		{0x51, 0x01, _pru_|_aau_}, // 6 :  DL(+α) TSSI 상한
		{0x51, 0x04, _pru_|_aau_}, // 5 :  DL 출력 상한
		{0x51, 0x05, _pru_|_aau_}, // 4 :  DL 출력 하한
		{0x51, 0x02, _pru_|_aau_}, // 3 :  DL TSSI 하한
		{0x51, 0x02, _pru_|_aau_}, // 2 :  DL(+α)  TSSI 하한
		{0xFF, 0xFF, __0__|__0__}, // 1 :  Reserved
		{0xFF, 0xFF, __0__|__0__}, // 0 :  Reserved
	},
	{	// 11 ( same with6, 11, 16, 21)
		{0x51, 0x10, _pru_|__0__}, // 7 :  Over Power
		{0x51, 0x13, _pru_|__0__}, // 6 :  High Temp
		{0x51, 0x11, _pru_|__0__}, // 5 :  VSWR
		{0x51, 0x17, _pru_|_aau_}, // 4 :  Disabled
		{0x51, 0x18, _pru_|__0__}, // 3 :  PAU Link(DL)
		{0x51, 0x14, _pru_|__0__}, // 2 :  Power Amp 전원(DC) 이상
		{0x51, 0x12, _pru_|__0__}, // 1 :  Device Fail
		{0x51, 0x15, _pru_|_aau_}, // 0 :  Shutdown
	},
	{	// 12 ( same with 7, 12, 17, 22)
		{0x53, 0x21, _pru_|__0__}, // 7 :  DPD
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 13 ( same with 8, 13, 18, 23)
		{0x52, 0x02, _pru_|_aau_}, // 7 : UL RSSI 하한
		{0x52, 0x02, _pru_|_aau_}, // 6 : UL(+α) RSSI 하한
		{0x52, 0x10, _pru_|_aau_}, // 5 : UL 과입력
		{0x52, 0x12, _pru_|__0__}, // 4 : UL LNA Fail
		{0x52, 0x13, __0__|_aau_}, // 3 : Gain 저하(UL)
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 14 ( same with9 14, 19, 24)
		{0xFF, 0xFF, __0__|__0__}, // 7 :  
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},	
	// 3rd DL/UL ---------------------------------------------------
	{	// 15 ( same with 5, 10, 15, 20)
		{0x51, 0x01, _pru_|_aau_}, // 7 :  DL TSSI 상한
		{0x51, 0x01, _pru_|_aau_}, // 6 :  DL(+α) TSSI 상한
		{0x51, 0x04, _pru_|_aau_}, // 5 :  DL 출력 상한
		{0x51, 0x05, _pru_|_aau_}, // 4 :  DL 출력 하한
		{0x51, 0x02, _pru_|_aau_}, // 3 :  DL TSSI 하한
		{0x51, 0x02, _pru_|_aau_}, // 2 :  DL(+α)	TSSI 하한
		{0xFF, 0xFF, __0__|__0__}, // 1 :  Reserved
		{0xFF, 0xFF, __0__|__0__}, // 0 :  Reserved
	},
	{	// 16 ( same with6, 11, 16, 21)
		{0x51, 0x10, _pru_|__0__}, // 7 :  Over Power
		{0x51, 0x13, _pru_|__0__}, // 6 :  High Temp
		{0x51, 0x11, _pru_|__0__}, // 5 :  VSWR
		{0x51, 0x17, _pru_|_aau_}, // 4 :  Disabled
		{0x51, 0x18, _pru_|__0__}, // 3 :  PAU Link(DL)
		{0x51, 0x14, _pru_|__0__}, // 2 :  Power Amp 전원(DC) 이상
		{0x51, 0x12, _pru_|__0__}, // 1 :  Device Fail
		{0x51, 0x15, _pru_|_aau_}, // 0 :  Shutdown
	},
	{	// 17 ( same with 7, 12, 17, 22)
		{0x53, 0x21, _pru_|__0__}, // 7 :  DPD
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 18 ( same with 8, 13, 18, 23)
		{0x52, 0x02, _pru_|_aau_}, // 7 : UL RSSI 하한
		{0x52, 0x02, _pru_|_aau_}, // 6 : UL(+α) RSSI 하한
		{0x52, 0x10, _pru_|_aau_}, // 5 : UL 과입력
		{0x52, 0x12, _pru_|__0__}, // 4 : UL LNA Fail
		{0x52, 0x13, __0__|_aau_}, // 3 : Gain 저하(UL)
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 19 ( same with9 14, 19, 24)
		{0xFF, 0xFF, __0__|__0__}, // 7 :  
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	// 4th DL/UL ---------------------------------------------------
	{	// 20 ( same with 5, 10, 15, 20)
		{0x51, 0x01, _pru_|_aau_}, // 7 :  DL TSSI 상한
		{0x51, 0x01, _pru_|_aau_}, // 6 :  DL(+α) TSSI 상한
		{0x51, 0x04, _pru_|_aau_}, // 5 :  DL 출력 상한
		{0x51, 0x05, _pru_|_aau_}, // 4 :  DL 출력 하한
		{0x51, 0x02, _pru_|_aau_}, // 3 :  DL TSSI 하한
		{0x51, 0x02, _pru_|_aau_}, // 2 :  DL(+α)	TSSI 하한
		{0xFF, 0xFF, __0__|__0__}, // 1 :  Reserved
		{0xFF, 0xFF, __0__|__0__}, // 0 :  Reserved
	},
	{	// 21 ( same with6, 11, 16, 21)
		{0x51, 0x10, _pru_|__0__}, // 7 :  Over Power
		{0x51, 0x13, _pru_|__0__}, // 6 :  High Temp
		{0x51, 0x11, _pru_|__0__}, // 5 :  VSWR
		{0x51, 0x17, _pru_|_aau_}, // 4 :  Disabled
		{0x51, 0x18, _pru_|__0__}, // 3 :  PAU Link(DL)
		{0x51, 0x14, _pru_|__0__}, // 2 :  Power Amp 전원(DC) 이상
		{0x51, 0x12, _pru_|__0__}, // 1 :  Device Fail
		{0x51, 0x15, _pru_|_aau_}, // 0 :  Shutdown
	},
	{	// 22 ( same with 7, 12, 17, 22)
		{0x53, 0x21, _pru_|__0__}, // 7 :  DPD
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 23 ( same with 8, 13, 18, 23)
		{0x52, 0x02, _pru_|_aau_}, // 7 : UL RSSI 하한
		{0x52, 0x02, _pru_|_aau_}, // 6 : UL(+α) RSSI 하한
		{0x52, 0x10, _pru_|_aau_}, // 5 : UL 과입력
		{0x52, 0x12, _pru_|__0__}, // 4 : UL LNA Fail
		{0x52, 0x13, __0__|_aau_}, // 3 : Gain 저하(UL)
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},
	{	// 24 ( same with 9 14, 19, 24)
		{0xFF, 0xFF, __0__|__0__}, // 7 :  
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0xFF, 0xFF, __0__|__0__}, // 1 :  
		{0xFF, 0xFF, __0__|__0__}, // 0 :  
	},	
	//------------------------------------------
	{   // 25
		{0x54, 0x40, _pru_|_aau_}, // 7 : UDA_불요파
		{0x54, 0x43, _pru_|_aau_}, // 6 : UDA_PD_CP(CPRI)
		{0x54, 0x42, _pru_|_aau_}, // 5 : UDA_LOF
		{0xFF, 0xFF, __0__|__0__}, // 4 : 
		{0xFF, 0xFF, __0__|__0__}, // 3 : 
		{0x54, 0x4A, _pru_|_aau_}, // 2 : UDA_Clock_Reset Suppression
		{0x54, 0x4B, _pru_|_aau_}, // 1 : UDA_CPRI_Reset Suppression
		{0xFF, 0xFF, __0__|__0__}, // 0 :  			
	},		
	{	// 26 
		{0xFF, 0xFF, __0__|__0__}, // 7 :  
		{0xFF, 0xFF, __0__|__0__}, // 6 :  
		{0xFF, 0xFF, __0__|__0__}, // 5 :  
		{0xFF, 0xFF, __0__|__0__}, // 4 :  
		{0xFF, 0xFF, __0__|__0__}, // 3 :  
		{0xFF, 0xFF, __0__|__0__}, // 2 :  
		{0x56, 0x11, _pru_|_aau_}, // 1 :  
		{0x54, 0x52, _pru_|__0__}, // 0 :  
	}
};




/*
------------------------------
DL1 DL2 DL3 DL4
------------------------------
 5, 10, 15, 20
 6, 11, 16, 21
 7, 12, 17, 22
 8, 13, 18, 23
 9  14, 19, 24
 ------------------------------
*/
//----------------------------------------------------------
#else
NMS_ALARM_EXT INT8U Alm_AID[RU_ALARM_CNT][8][3];
#endif
//-----------------------------------------------------










//NMS_ALARM_EXT INT8U Get_AlmPosi_Fwd_AMP_Off( INT8U amp_path4, INT8U* pbyte, INT8U* pbit ); 2019.04.15 deleted
//----------------------------------------------------------
//NMS_ALARM_EXT void Set_AMP_Rvs_Off_Alm( INT8U amp_path_5, INT8U set ); 2019.04.15 deleted

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//NMS_ALARM_EXT void Set_RU_Alarm( INT8U alm_idx, INT8U set ); 2019.04.15 deleted
//NMS_ALARM_EXT INT8U Is_RU_Alarm( INT8U alm_idx ); 2019.04.15 deleted
//----------------------------------------------------------







NMS_ALARM_EXT INT8U IS_Dig_Alarm(void);






NMS_ALARM_EXT INT8U alarm_rpt_pause_req;
NMS_ALARM_EXT INT8U alarm_rpt_pause_req_Type;

NMS_ALARM_EXT INT8U NMS_Alarm_En;
NMS_ALARM_EXT OS_TIMER NMS_Alarm_Timer; // 10sec 지난 후 알람 발생 시작.


NMS_ALARM_EXT void Init_NMS_Alm_Proc(void);
NMS_ALARM_EXT void Chk_n_Make_NMS_Alm(void);

NMS_ALARM_EXT void Chk_UDA_Alm_gREMS( void );
NMS_ALARM_EXT void Chk_RebootAlarm_gREMS(void);
NMS_ALARM_EXT void Send_PRU_AC_Alarm_gREMS(INT8U alarm);

NMS_ALARM_EXT INT8U Make_Alm_Str(INT8U aid0, INT8U aid1, INT8U layer, INT8U carrier, INT8U alm_f, char* alm_info_str);



NMS_ALARM_EXT INT8U Update_Alm_Str_Lmt( INT8U aid0, INT8U aid1, INT8U layer, INT8U carrier, INT8U alm_f);


NMS_ALARM_EXT INT8U Get__CPRI_Num(INT8U carrier, INT8U updn);
NMS_ALARM_EXT void Init__Alm_Str_Lmt(void);


NMS_ALARM_EXT void Send_PRU_AC_Alarm_gREMS_Only(INT8U alarm);
NMS_ALARM_EXT void Proc__Sending_Init_Alarm__When_RuInit_End(void);


/********************************************************************************/
#endif
/********************************************************************************/


