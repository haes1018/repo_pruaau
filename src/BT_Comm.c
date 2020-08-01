/////////////////////////////
//	BT_COMMM.C
/////////////////////////////
#define BT_COMMM_GLOBALS
#include "includes.h"
/////////////////////////////

//----------------------------------------------
#ifdef __TYPE__AAU__ // 5G-AAU-Only 
//----------------------------------------------



static PROTOCOL_FRAME	bt_txf;
static PROTOCOL_FRAME	bt_rxf;

//static INT8U BT_Link_Fail;
//static INT8U BT_Link_Fail_Co;

//static pthread_mutex_t	BT_COMM_Evt;
//static pthread_mutex_t	BT_Ctrl_Evt;





#if 0

#define BT_POLL_MAX (300)
INT8U BT_Poll_data[BT_POLL_MAX];

INT8U Send_BT_Poll( void )
{
	INT8U ret;
	INT16U len = 0;	

	//BT_Poll_data[len++] = 0;
	//BT_Poll_data[len++] = 0;
	//BT_Poll_data[len++] = 0;
	
	ret = Send_Bluetooth_Data( 0x00, BT_Poll_data, len  );

	if( ret )
	{
		BT_Link_Fail = 0;
		BT_Link_Fail_Co = 0;		
	}
	else
	{
		BT_Link_Fail_Co++;

		if( BT_Link_Fail_Co>=LINK_FAIL_CO )
		{
			BT_Link_Fail_Co = LINK_FAIL_CO;
			BT_Link_Fail = 1;			
		}
	}
	return ret;
}

#endif

// 2019.08.15(v08.14.99)
INT8U BT_req = 0;
INT8U BT_status = 0;

INT8U Get_Bt_status(void)
{
	return BT_status;
}

void Set_Bt_req(INT8U bt_status)
{
	BT_req = bt_status;
}

void check_BT_init(void)
{
	static INT8U IN_BT_status = 0;
	
	if(m_St.Is_CPRI_Lock) // if cpri locked
	{
		//// YCSO change 190827
		if( m_St.RU_Init_Status == Init_St___Completed )
		{
			BT_status = BT_RU_ID;
		}
		else
		{
			BT_status = BT_CASCADE;
		}
	}

	else // cpri unlocked
	{
		BT_status = BT_SERIAL;		
	}

	if( BT_req == BT_status  )
	{
		IN_BT_status = BT_req;
		Set_BT_Name(BT_req);
		m_Env.Bluetooth_re_Init_Req = BT_req;
		BT_req = BT_CLR;
	}

	if( IN_BT_status != BT_status )
	{
		IN_BT_status = BT_status;
		Set_BT_Name(IN_BT_status);
		m_Env.Bluetooth_re_Init_Req = IN_BT_status;
	}
		


}

#ifdef __YCSO__BT__CMD__ //// YCSO added 191126
OS_TIMER BT_mode_timer;
#endif

void* BlueTooth_Comm_Task(void *data)
{	
	//INT32S res;		
	INT8U* txf;
	INT8U* rxf;
	INT8U port;
	#ifdef __YCSO__BT__CMD__ //// YCSO added 191126
	INT8U bt_cnt = 0;
	#endif
	
	#if 0
	Thread_Priority_Init((Thread_ARG_Var*)data, 1);
	D_Printf( DEBUG_SYSTEM, "BlueTooth Comm Task RUN\n" );

	res = pthread_mutex_init(&BT_COMM_Evt, NULL);
	if (res != 0) 
	{
		syslog(LOG_ERR, "[RCU] BT_Comm_Evt Mutex Init Fail.");
		exit(EXIT_FAILURE);
	}
	
	res = pthread_mutex_init(&BT_Ctrl_Evt, NULL);
	if (res != 0) 
	{
		syslog(LOG_ERR, "[RCU] BT_Ctrl_Evt Mutex Init Fail.");
		exit(EXIT_FAILURE);
	}
	#endif


	Thread_ARG_Var* p_th_arg;

	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	port = (INT8U)p_th_arg->Arg_v;
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "BlueTooth_Comm_Task RUN");

	txf = (INT8U*)&bt_txf;
	rxf = (INT8U*)&bt_rxf;
	

	BT_RESET();
	OSTimeDly( OS_DELAY_1sec );
	BT_SET();


	
	Init_Comm(port);
	Init_Tx_Frame_SeqNo(port);	
	OSTimeDly( OS_DELAY_1sec );

//	BT_Init();	
	

#ifdef __YCSO__BT__CMD__ //// YCSO added 191126
	Set_Bt_req(BT_SERIAL); // 2019.08.15(v08.14.99)
	OSTimerReset(&BT_mode_timer);
#endif
	
	while( m_Env.task_flag[_TASK_BLUETOOTH_COMM_] )		///// YCSO TASK END
	{		
		check_BT_init(); // 2019.08.15(v08.14.99)
		/////	YCSO added START 190411
		if( m_Env.Bluetooth_re_Init_Req != BT_CLR ) // 2019.08.15(v08.14.99)
		{
			BT_Init(m_Env.Bluetooth_re_Init_Req); // 2019.08.15(v08.14.99)
			m_Env.Bluetooth_re_Init_Req = BT_CLR; // 2019.08.15(v08.14.99)
		}		
		/////	YCSO added END

		OSTimeDly( 3*OS_DELAY_10msec ); // 2020.03.13 : 500 -> 30
		
		Update__Task_Count(_TASK_BLUETOOTH_COMM_);
		//------------------------------------------
		Check_UP_Data( port, txf, rxf, 50 );
		//OSTimeDly( OS_DELAY_10msec*10 );
		//CheckUART_Error( port );		

	#ifdef __YCSO__BT__CMD__ //// YCSO added 191126
		if( OSTimerGet_msec(&BT_mode_timer) > (10*TICK_CO_SEC))
		{
			bt_cnt++;
			OSTimerReset(&BT_mode_timer);
			if(bt_cnt >= 6)
			{
				bt_cnt = 0;
				printf("TEST_BT_MODE_CMD\n");
			}
			send_BT_mode_CMD();
		}
	#endif	
	}
	m_Env.task_END_status[_TASK_BLUETOOTH_COMM_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}
////////////////////////////////////////////////////////////














//----------------------------------------------
#endif // __TYPE__AAU__ : 5G-AAU-Only 
//----------------------------------------------

/* EOF */


