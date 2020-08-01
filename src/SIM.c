/////////////////////////////
//	SIM.C
/////////////////////////////
#define SIM_GLOBALS
#include "includes.h"
/////////////////////////////
///// YCSO added 
//static PROTOCOL_FRAME_SIM_TX	sim_txf;
static PROTOCOL_FRAME			sim_txf;
static PROTOCOL_FRAME			sim_rxf;

OS_TIMER SIM_task_req_timer;

INT8U Check_UP_Data_SIM(INT8U port, INT8U* txf, INT8U* rxf, INT8U delay )
{
	int ret;
	INT16U len;
	INT8U  rx_ip[4] = {0}; // pkt을 보내온 node-RCU의 IP
//	INT16U port16;
	INT16U re = 0;

	ret = Check_Node_SIM_Data( COMM_SIM_TCP, rxf, 300 );	

//	memcpy(rx_ip, (INT8U*)&comm_init[port].recv_ip,4);
//	port16 = udp_port[port];
	if( ret )
	{
//		printf(VT_BOLD_GREEN"CHECK_SIM_reve data here !!!!!!!!!! \n");
		len = Recv_UP_Data(COMM_SIM_TCP, txf, rxf, rx_ip, 0);
		if(len)
		{				
			Send_TCP_Data_GUI( COMM_SIM_TCP, rx_ip, txf , len );
			re = 1;
		}
	}
	return re;
}

void* SIM_Task(void *data)
{
	INT8U* txf;
	INT8U* rxf;
	INT8U port;
	Thread_ARG_Var* p_th_arg;

	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	port = (INT8U)p_th_arg->Arg_v;
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "SIM_Task RUN");

	txf = (INT8U*)&sim_txf;
	rxf = (INT8U*)&sim_rxf;
	OSTimerReset( &SIM_task_req_timer );

//	Init_SIM_TCP_fd(COMM_SIM_TCP, ip);
//	Init_Comm(port);


	Init_Tx_Frame_SeqNo(port);	
	OSTimeDly( OS_DELAY_1sec );

	while( m_Env.task_flag[_TASK_SIM_] )		///// YCSO TASK END
	{
		OSTimeDly( OS_DELAY_100msec*2 );
		Update__Task_Count(_TASK_SIM_);
		
		if( m_Env.SIM_task_req )
		{
			if( Check_UP_Data_SIM( port, txf, rxf, 50 ) )
			{
				OSTimerReset( &SIM_task_req_timer );
//				printf("SIM_task_req!!!!!\n");	
			}

			if( OSTimerGet_msec(&SIM_task_req_timer) > 3*TICK_CO_MIN )
			{
				Close_SIM_TCP_fd();
				printf("_YCSO_ TCP SIM TIME OUT!!!!!\n");
				OSTimerReset( &SIM_task_req_timer );
				m_Env.SIM_task_req = 0;
			}
			
		}

	}
	m_Env.task_END_status[_TASK_SIM_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}

INT16U SIM_task_req(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf)
{
	INT8U totPkt = 1, curPkt = 1;

	INT8U ip[4] = {0};
	INT16U port = 0;
	int x;

	for(x = 0; x < 4; x++)
		ip[x] = rxf->data[x];

	port = ((INT16U)(rxf->data[4]) << 8 ) + (INT16U)rxf->data[5];

	if( Get_UDP_Sock(COMM_SIM_TCP) > 0 )
		Close_SIM_TCP_fd();

	Init_SIM_TCP_fd(port, ip);

	Init_Comm(COMM_SIM_TCP);


	OSTimerReset( &SIM_task_req_timer );

	m_Env.SIM_task_req = 1;

	

	return Make_Frame( rxf->Type, rxf->Rcode, txf, 1, rxf->seqNo16, totPkt, curPkt );


}

//---------------------------------------------------------------------------------------------


/* EOF */
