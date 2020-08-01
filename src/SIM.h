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
	SIM_Auto_ID,     // 0x01: SIM Auto ID ����
	SIM_Install,     // 0x02: SIM Auto Install ����
	SIM_Param,       // 0x03: SIM Auto Paramter  Auto ����
	SIM_Env_Test,    // 0x04: SIM ����ȯ�� Test
	SIM_Path,        // 0x05: SIM Path Auto ����
	SIM_Delay,       // 0x06: SIM Delay Auto ����
	SIM_RF,          // 0x07: SIM RF ����

	SIM_Rlt_End = 0x10, // SIM ���� �Ϸ�
	SIM_Rlt_VSWR_S,      // 0x11: SIM ���� �Ϸ�(�����ʿ�-VSWR SISO)
	SIM_Rlt_VSWR_M,      // 0x12: SIM ���� �Ϸ�(�����ʿ�-VSWR MIMO)
	SIM_Rlt_AC_Pwr,      // 0x13: SIM ���� �Ϸ�(�����ʿ�-AC ����)
	SIM_Rlt_Watt,        // 0x14: SIM ���� �Ϸ�(�����ʿ�-���·�)
	SIM_Rlt_Comm,        // 0x15: SIM ���� �Ϸ�(�����ʿ�-��� �̻�) 
	SIM_Rlt_Cancel,      // 0x16: SIM ���� �Ϸ�(�������) --- user cancel ��û ��.
	SIM_Rlt_Impossible,  // 0x17 : SIM ���� �Ұ� : iMHS/DTU/AMP/PSU ��� �Ұ��� ���� SIM ���� �Ұ��� ����
	
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
	
	char dbg[SIM_Dbg_Max]; // 0xACCB, SIM �����߿��� �����ϸ�, FW���� Debug Message�� ���ڿ� ���·� ����
	
	
	

	// Test pattern
	INT8U  Pttn_Onoff[AMPU_4_MAX]; // 0xACC9, S/C, [AMP4] �� ����. with Digital
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
threshold(VSWR)	        0xAC	0xCD	S,C	L	2	"Value(unsigned 16bit) * 100 (���� ����)113 ���Ž� 1.13:1 ��I ǥ��"
threshold(AC)	        0xAC	0xCE	S,C	L	2	"Value(signed 16bit) (V)Value(Signed 16bit) * 10 (iRO) (0.1v)"
threshold(���·�(MIMO))	0xAC	0xCF	S,C	L	2	Value(unsigned int)
threshold(���·�(SISO))	0xAC	0xD0	S,C	L	2	Value(unsigned int)

0xACCB Debug Message(SIM)	S, SIM �����߿��� ����, FW���� Debug Message N byte ���ڿ� ǥ��

0xACCC  Start(SIM)		C	L	1	0x00: ����, 0x01: ���
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
