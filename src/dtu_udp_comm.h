/********************************************************************************/
#ifndef __DIG_UDP_COMM_H
#define __DIG_UDP_COMM_H
/********************************************************************************/
#ifdef  DIG_UDP_COMM_GLOBALS
#define DIG_UDP_COMM_EXT
#else
#define DIG_UDP_COMM_EXT extern
#endif
/**************************************************************/





DIG_UDP_COMM_EXT INT16U DTU_Dnload_PassData( INT8U type, PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );

DIG_UDP_COMM_EXT void OSM_Ctrl_via_AID(INT8U val);





DIG_UDP_COMM_EXT void Init_DTU_UDP_SIM_Ctrl(void);
DIG_UDP_COMM_EXT void Add_DTU_UDP_SIM_CtrlData( INT8U* data, INT8U l );
DIG_UDP_COMM_EXT INT8U Send_DTU_UDP_SIM_Ctrl(void);

#define D_FRAME_DATA_MAX (1000)
#define DTU_DN_DATA_MAX (D_FRAME_DATA_MAX + 20)


typedef struct
{
	INT8U st;	
	INT8U retry;
	INT16U tot_frame;
	INT16U cur_frame;
	INT16U crc;
	INT8U result;
	INT8U req;
	INT8U fw_apply_req;
	INT8U percent;
	char str[10];
	int tot_size;
	int fd;	
}DTU_Auto_DN;

DIG_UDP_COMM_EXT DTU_Auto_DN m_DTU_AutoDn;
DIG_UDP_COMM_EXT DTU_Auto_DN m_FPGA_AutoDn;
DIG_UDP_COMM_EXT void Update_DIG_DL_State(void);
DIG_UDP_COMM_EXT INT8U Is_Dig_MultiDown(void);




/********************************************************************************/
#endif
/********************************************************************************/


