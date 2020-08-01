/////////////////////////////
//	UP_COM.C
/////////////////////////////
#define UP_COM_GLOBALS
#include "includes.h"
/////////////////////////////






/////////////////////////////
// Local Variables
/////////////////////////////
static PROTOCOL_FRAME	up_txf[2];
static PROTOCOL_FRAME	up_rxf[2];



/////////////////////////////
// Local Functions
/////////////////////////////


/////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////



void* UP_Comm_Task(void *data)
{
	INT8U* txf;
	INT8U* rxf;
	INT8U port;

	Thread_ARG_Var* p_th_arg;

	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	port = (INT8U)p_th_arg->Arg_v;

	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "Up-Comm %d Task RUN\n", port);

	switch( port )
	{
		case COMM_TERM:
			txf = (INT8U*)&up_txf[0];
			rxf = (INT8U*)&up_rxf[0];
			break;

		default :
			while(1) OSTimeDly( OS_DELAY_1sec );
	}

	OSTimeDly( 5*OS_DELAY_1sec );

	Init_Comm(port);
	Init_Tx_Frame_SeqNo(port);

	while( m_Env.task_flag[_TASK_UP_COMM_] )		///// YCSO TASK END
	{	
		Check_UP_Data( port, txf, rxf, 50 );
		OSTimeDly( OS_DELAY_10msec );
		Update__Task_Count(_TASK_UP_COMM_);
		
		CheckUART_Error( port );		
	}
	m_Env.task_END_status[_TASK_UP_COMM_] = _CLR_;
	pthread_exit(NULL);
}

/* EOF */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

