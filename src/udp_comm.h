/********************************************************************************/
#ifndef __UDP_COMM_H_
#define __UDP_COMM_H_
/********************************************************************************/
#ifdef  UDP_COMMM_GLOBALS
#define UDP_COMMM_EXT
#else
#define UDP_COMMM_EXT extern
#endif


#define ALM_RPT_Chk_Max 10
typedef struct  __attribute__((packed))
{	
	INT8U cmd;
	INT8U pkt_num;	
	INT8U pkt_tot;	
	INT8U subid[4];
	INT8U  chkcnt;
	INT16U aid16[ALM_RPT_Chk_Max];
} STR_AUTO_RPT_INFO;
UDP_COMMM_EXT STR_AUTO_RPT_INFO m_AutoRpt_Info; 
UDP_COMMM_EXT STR_AUTO_RPT_INFO m_Alm_Rpt_Info; 

UDP_COMMM_EXT void Init_Auto_Rpt_Flag(void);



//UDP_COMMM_EXT void Send_Alive_Status( void );
// TREE_2019_0521
UDP_COMMM_EXT INT16U Pass_Node_Data( INT8U port, DEV_TYPE *dev, PROTOCOL_FRAME* rxf, INT8U* r_ip, INT16U r_udp_port );
// END : TREE_2019_0521

//UDP_COMMM_EXT INT16U Send_Node_Alarm( PROTOCOL_FRAME* rxf, INT16U len );
UDP_COMMM_EXT void* Broad_Comm_Task(void *data);

UDP_COMMM_EXT void* Node_Comm_Rx_Task(void *data);
UDP_COMMM_EXT void* REMS_Comm_Rx_Task(void *data); ////// it will be deleted later....



//UDP_COMMM_EXT void* UDP_Rx_Task(void *data); ////// it will be deleted later....
UDP_COMMM_EXT void* UDP_DTU_Rx_Task(void *data);
UDP_COMMM_EXT void* UDP_Sel_Task(void *data);
UDP_COMMM_EXT void* UDP_Sel_Proc_Task(void *data);

UDP_COMMM_EXT void pipe_create(INT8U i);
UDP_COMMM_EXT void udp_rx_que_pipe_create(void);
UDP_COMMM_EXT void udp_rx_que_pipe_destroy(void);
UDP_COMMM_EXT int pipe_udp_push_write (INT8U comm_num );

UDP_COMMM_EXT int udp_pop_fd[COMM_NUM];
UDP_COMMM_EXT int udp_push_fd[COMM_NUM];





UDP_COMMM_EXT INT8U Get_SUB_ID_IP( INT8U* sub_id, INT8U *ip );

UDP_COMMM_EXT void Send_Auto_Inventory( void );

UDP_COMMM_EXT INT8U Ro_IP_Set_Req;







#endif
