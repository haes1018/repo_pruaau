/********************************************************************************/
#ifndef __SIM_H_
#define __SIM_H_
/********************************************************************************/
#ifdef  SIM_GLOBALS
#define SIM__EXT
#else
#define SIM__EXT extern
#endif
/**************************************************************/

// ACCA : Sim State
enum
{
	SIM_Idle = 0x00, // idle
	SIM_Auto_ID,     // 0x01: SIM Auto ID 설정
	SIM_Install,     // 0x02: SIM Auto Install 설정
	SIM_Param,       // 0x03: SIM Auto Paramter  Auto 설정
	SIM_Env_Test,    // 0x04: SIM 구축환경 Test
	SIM_Path,        // 0x05: SIM Path Auto 설정
	SIM_Delay,       // 0x06: SIM Delay Auto 설정
	SIM_RF,          // 0x07: SIM RF 시험

	SIM_Rlt_End = 0x10, // SIM 동작 완료
	SIM_Rlt_VSWR_S,      // 0x11: SIM 동작 완료(점검필요-VSWR SISO)
	SIM_Rlt_VSWR_M,      // 0x12: SIM 동작 완료(점검필요-VSWR MIMO)
	SIM_Rlt_AC_Pwr,      // 0x13: SIM 동작 완료(점검필요-AC 전원)
	SIM_Rlt_Watt,        // 0x14: SIM 동작 완료(점검필요-전력량)
	SIM_Rlt_Comm,        // 0x15: SIM 동작 완료(점검필요-통신 이상) 
	SIM_Rlt_Cancel,      // 0x16: SIM 동작 완료(수행취소) --- user cancel 요청 시.
	SIM_Rlt_Impossible,  // 0x17 : SIM 수행 불가 : iMHS/DTU/AMP/PSU 통신 불가로 인해 SIM 수행 불가한 상태
	
	SIM_Max
};

#define SIM_Dbg_Max (128)
typedef struct // __attribute__((packed))
{
	INT8U st; // 0xACCA
	INT8U start_req; 
	INT8U stop_req;
	INT8U dbg_len;
	INT8U list[8];
	INT8U start_idx;
	INT8U result_id;
	INT8U erase_id;
	INT8U erase_id_req;
	INT8U empty_num;
	INT8U auto_id_success;
	INT8U chk_cnt;
	
	char dbg[SIM_Dbg_Max]; // 0xACCB, SIM 동작중에만 전송하며, FW에서 Debug Message를 문자열 형태로 전송
	
	
	

	// Test pattern
	INT8U  Pttn_Onoff[AMPU_4_MAX]; // 0xACC9, S/C, [AMP4] 의 순서. with Digital
	INT8U  Pttn_Onoff_Req[AMPU_4_MAX]; // request value
	INT8U  Pttn_Onoff_f[AMPU_4_MAX]; // request flag
	
	INT8U  Pttn_St; // 0xACD1, loading state, (0 : loading..  1 : complete)
	INT8U  Pttn_Mode; // 0xACD2, S/C
	/*
		0 : LTE Only
		1 : CDMA Add
		2 : WCDMA Add
		3 : CDMA/WCDMA Add
	*/
}SIM;
SIM__EXT SIM m_Sim;


SIM__EXT void* SIM_Task(void *data);
SIM__EXT void Init_SIM(void);
SIM__EXT void Proc_SIM(void);


/*
threshold(VSWR)	        0xAC	0xCD	S,C	L	2	"Value(unsigned 16bit) * 100 (단위 없음)113 수신시 1.13:1 로I 표기"
threshold(AC)	        0xAC	0xCE	S,C	L	2	"Value(signed 16bit) (V)Value(Signed 16bit) * 10 (iRO) (0.1v)"
threshold(전력량(MIMO))	0xAC	0xCF	S,C	L	2	Value(unsigned int)
threshold(전력량(SISO))	0xAC	0xD0	S,C	L	2	Value(unsigned int)

0xACCB Debug Message(SIM)	S, SIM 동작중에만 전송, FW에서 Debug Message N byte 문자열 표시

0xACCC  Start(SIM)		C	L	1	0x00: 시작, 0x01: 취소
*/

SIM__EXT void SIM_Proc_Auto_ID_Set(void);
SIM__EXT INT8U SIM_Auto_ID_Set_Req;
SIM__EXT INT8U SIM_Auto_Install_Set_Req;


SIM__EXT INT8U Is_SIM_OnGoing(void);

SIM__EXT INT16U tx_SIM_Result( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );




SIM__EXT INT8U Is_Test_Pattern_Load_End(void);

SIM__EXT INT16U SIM_task_req(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf);
//SIM__EXT INT16U UP_DATA_SIM(PROTOCOL_FRAME * rxf, PROTOCOL_FRAME_SIM_TX * txf, INT8U port, INT8U * r_ip);




/********************************************************************************/
#endif
/********************************************************************************/
