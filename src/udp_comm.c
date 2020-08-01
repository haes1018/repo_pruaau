/////////////////////////////
//	UDP_Commm.C
/////////////////////////////
#define UDP_COMMM_GLOBALS
#include "includes.h"
/////////////////////////////


static PROTOCOL_FRAME	node_comm_txf;
static PROTOCOL_FRAME	node_comm_rxf;

static PROTOCOL_FRAME	rems_comm_txf;
static PROTOCOL_FRAME	rems_comm_rxf;

//static PROTOCOL_FRAME	alive_comm_txf;






//-----------------------------------------------


//-------------------------------------------------------------------

INT8U Get_Node_IP( DEV_TYPE *dev, INT8U *ip )
{
	ip[0] = 192;
	ip[1] = 168;			
	ip[2] = 0;						
	ip[3] = (dev->id)+31; // Not defined...
	
	return 1;
}

INT8U Get_SUB_ID_IP( INT8U* sub_id, INT8U *ip )
{
	DEV_TYPE dev;

	if( SUB_ID_Check( sub_id, &dev ) ) return 0;
	
	return Get_Node_IP( &dev, ip );
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TREE_2019_0521
INT16U Pass_Node_Data( INT8U port, DEV_TYPE *dev, PROTOCOL_FRAME* rxf, INT8U* r_ip, INT16U r_udp_port )
{
	INT16U len;
	INT8U ip[4] = {0,0,0,0};
	INT16U port16 = 20000;	

	// Make IP Address	
	if(Cal_Node_IP_Addr(rxf->SUB_ID, ip) == RET_Error) return 0;

	switch( port )
	{
		case COMM_TERM:
			rxf->Rcode = _RCODE_GUI; // packet to other-RU @ 5G from GUI
			break;

		case COMM_UDP_NODE:
			rxf->Rcode = _RCODE_UDP; // packet to other-RU @ 5G from GUI
			break;	

		#ifdef __TYPE__AAU__ // 5G-AAU-Only 
		case COMM_BT:
			rxf->Rcode = _RCODE_USB; // packet to other-RU @ 5G from GUI
			break;
		#endif 
			
		default: // Pass_Node_Data 는 COMM_TERM, COMM_IRCS 만 사용
			return 0;
	}	
	
	if(r_ip != NULL)
		Insert_GUI_IP_Table(r_ip, r_udp_port, rxf->seqNo16);	

	len = Re_Make_Frame(rxf);	
	
	Send_UDP_Node_Data( COMM_UDP_NODE, ip, port16, (INT8U*)rxf, len );
	return 0;
}
// END : TREE_2019_0521





//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



//-------------------------------------------------------------------------------------

static OS_TIMER reset_dly_mea_timer;
static OS_TIMER Auto_Inv_Rpt_Timer;

static OS_TIMER Optic_Ready_timer;

#define BROAD_CAST_POLL_TIME	3000

void* Broad_Comm_Task(void *data)
{
	
	OSTimerReset(&reset_dly_mea_timer);
	OSTimerReset(&Auto_Inv_Rpt_Timer);
	OSTimerReset(&Optic_Ready_timer);
		

    while (1)
	{
		OSTimeDly( OS_DELAY_10msec );
	}
	pthread_exit(NULL);		
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
void Proc_UDP_GUI_Pkt(void)
{
	int ret;
	INT16U len;
	INT8U  rx_ip[4]; // pkt을 보내온 node-RCU의 IP
	INT16U port16;

	ret = Check_Node_Data( COMM_UDP_NODE, (INT8U*)&node_comm_rxf, 300, rx_ip, &port16);	
	
	
	if(ret==0xFF) // CRC Fail
	{
		memcpy(node_comm_txf.SUB_ID, node_comm_rxf.SUB_ID, 3);	
		len = SendError( &node_comm_txf, node_comm_rxf.Rcode, gREMS_Err_CRC_Fail, COMM_UDP_NODE );
		if(len)
		{
			Send_UDP_Data_GUI( COMM_UDP_NODE, rx_ip, (INT8U*)&node_comm_txf, len, port16 );
			return;
		}
	}	

	if( ret ) // CRC_Fail(0xFF) or Success
	{
		/*
		struct sockaddr_in addr;	
		memset(&addr, 0, sizeof(addr));	
		addr.sin_addr.s_addr = *(INT32U*)rx_ip;
		D_Printf(DEBUG_SYSTEM, "[RCU] UDP ip is %s\n", inet_ntoa(addr.sin_addr));
		*/
		// TREE_2019_0521
		if(memcmp(rx_ip, m_St.Operate_Ip, 4)) // 자기가 Tx 한 DATA를 받는 경우 제외
		{
			if( NodeDataAck( &node_comm_rxf, rx_ip )==0 )
			{
				len = Recv_UP_Data(COMM_UDP_NODE, (INT8U*)&node_comm_txf, (INT8U*)&node_comm_rxf, rx_ip, port16);
				if(len)
				{				
					Send_UDP_Data_GUI( COMM_UDP_NODE, rx_ip, (INT8U*)&node_comm_txf, len, port16 );
				}
			}
		}
		// END : TREE_2019_0521
	}	
}
//-------------------------------------------------------------------------------------









//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void Proc_REMS_Node_Pkt(void)
{
	INT16U len;
	INT8U  ip[4]; // pkt을 보내온 node-RCU의 IP
	INT16U port16;
	INT8U ret;

	ret = Check_Node_Data( COMM_UDP_REMS, (INT8U*)&rems_comm_rxf, 300, ip, &port16 );
	
	if(ret==0xFF) // CRC Fail
	{
		memcpy(rems_comm_txf.SUB_ID, rems_comm_rxf.SUB_ID, 3);
		len = SendError( &rems_comm_txf, rems_comm_rxf.Rcode, gREMS_Err_CRC_Fail, COMM_UDP_REMS );
		
		if(len)
		{
			Send_gREMS_Data( COMM_UDP_REMS, ip, port16, (INT8U*)&rems_comm_txf, len );
			return;
		}
	}	

	if(ret)
	{
		/*
		struct sockaddr_in addr;	
		memset(&addr, 0, sizeof(addr));	
		addr.sin_addr.s_addr = *(INT32U*)ip;
		D_Printf(DEBUG_SYSTEM, "[RCU] REMS ip is %s\n", inet_ntoa(addr.sin_addr));
		*/

		print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, "---------------- ");
		print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, "Proc_REMS_Node_Pkt ");
		print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, "---------------- ");

		if(memcmp(ip, m_St.Operate_Ip, 4)) // 자기가 Tx 한 DATA를 받는 경우 제외
		{
		// TREE_2019_0521
			len = Recv_UP_Data(COMM_UDP_REMS, (INT8U*)&rems_comm_txf, (INT8U*)&rems_comm_rxf, ip, port16);
			if(len)
				Send_gREMS_Data( COMM_UDP_REMS, ip, port16, (INT8U*)&rems_comm_txf, len );
		// END : TREE_2019_0521
		}
	}	
}


//-------------------------------------------------------------------------------------
//
void* UDP_DTU_Rx_Task(void *data)
{
	Thread_Priority_Init((Thread_ARG_Var*)data, 1);

	Init_Comm( COMM_UDP_DTU );
	Init_Tx_Frame_SeqNo(COMM_UDP_DTU);
	m_Env.UDP_Init[COMM_UDP_DTU] = 1;

	
//	Init_Comm( COMM_UDP_LS );
//	Init_Tx_Frame_SeqNo(COMM_UDP_LS);
//	m_Env.UDP_Init[COMM_UDP_LS] = 1;

	while( m_Env.task_flag[_TASK_UDP_DTU_RX_] )		///// YCSO TASK END
	{
		OSTimeDly( OS_DELAY_10msec );
		Update__Task_Count(_TASK_UDP_DTU_RX_);
		RecvUDPDataQue_lohost();
//		RecvUDPDataQue_LSH();
	}
	m_Env.task_END_status[_TASK_UDP_DTU_RX_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);		
}



/*
--------------------------------------------------------

--------------------------------------------------------
*/

#ifdef __DEL_UDP_REMS__
#define port_sel_max (2)
#else
#define port_sel_max (3)
#endif
void* UDP_Sel_Task(void *data)
{	
	//Thread_ARG_Var* p_th_arg;
	INT8U port;	
	INT8U port_array[port_sel_max] = 
	{
		COMM_UDP_NODE,
		COMM_UDP_LS,
#ifdef __DEL_UDP_REMS__
#else		
		COMM_UDP_REMS
#endif		
	};
	
	int fd[port_sel_max];
	int i;
	int state;	
	int last_fd;
	struct timeval tv;


	//--------------------------------------------------------
 	Thread_ARG_Var* p_th_arg;
	char str[100];
	int idx;
		
	p_th_arg = (Thread_ARG_Var*)data;
	Thread_Priority_Init((Thread_ARG_Var*)data, 1); 
		
	idx = MAX_THREAD_PRIORITY -(INT32U)p_th_arg->priority; 
	if(idx >= 0)
	{
		//m_Task_Info[idx].task_id = local_gettid();
		//Task_Cnt_Init(idx);
		strcpy(m_Task_Info[idx].task_name, "Thread_UDP_Sel");
	}
	
	sprintf( str, "---(%02d/%d)--- %s [%d]\n", 
		idx, Max_Task_Cnt,
		m_Task_Info[idx].task_name, 
		m_Task_Info[idx].task_id );
	
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "%s", str);
	syslog( LOG_INFO, "%s", str);
	//--------------------------------------------------------

	udp_rx_que_pipe_create();

	fd_set readfds;//, writefds;

	for(i=0;i < port_sel_max; i++)
	{
		port = port_array[i];
		
		Init_Comm(port);
		Init_Tx_Frame_SeqNo(port);		
		m_Env.UDP_Init[port] = 1;
		
		fd[i] = Get_UDP_Sock(port);
		
		D_Printf(DEBUG_SYSTEM, "[RCU] UDP_Sel_Task, port %d, fd[%d] = %d, init\n", port, i, fd[i]);		
	}
	
	OSTimeDly( OS_DELAY_1sec ); // Ethernet 초기화 시간 필요

	while( m_Env.task_flag[_TASK_UDP_SEL_] )		///// YCSO TASK END
	{
		FD_ZERO(&readfds); // ready for input, delete all fds froem readfds set
		last_fd	= 0;

		Update__Task_Count(_TASK_UDP_SEL_);

		for(i=0;i < port_sel_max; i++)
		{
			FD_SET(fd[i], &readfds); // add fd[i] to readfds set
			
			if (last_fd < fd[i]) 
				last_fd = fd[i];
		}	

        if (last_fd == 0)
        {
            sleep(1);
            D_Printf( DEBUG_SYSTEM, "[RCU] !! select sleep(1) -----------\n");
            
            continue;
        }

		tv.tv_sec  = 0;
        tv.tv_usec = 10000; // 10[ms], 2019.05.16, 400 -> 10
		state = select( last_fd+1, &readfds, NULL, NULL, &tv);

		switch(state)
		{
			case -1:
				D_Printf( DEBUG_SYSTEM,"[RCU] !! select -----------\n");
				exit(0);
				break;
				
			case 0:
				// timeout
				break;

			default:
				for(i=0; i < port_sel_max; i++)
				{
					if(FD_ISSET(fd[i], &readfds))
					{
						RecvUDPDataQue_All(port_array[i]);
					}
				}				
				break;
		}
	}
	m_Env.task_END_status[_TASK_UDP_SEL_] = _CLR_;
	pthread_exit(NULL);
}
//--------------------------------------------------------------------
void* UDP_Sel_Proc_Task(void *data)
{
	INT8U port; 
	int state;	
	int last_fd;
	INT8U re;	
	struct timeval tv;
	//--------------------------------------------------------
	Thread_ARG_Var* p_th_arg;
	char str[100];
	int idx;
	INT8U *task_flag = NULL;				///// YCSO TASK END
	INT8U *task_END = NULL;
	INT8U  task_id;


	p_th_arg = (Thread_ARG_Var*)data;
	Thread_Priority_Init((Thread_ARG_Var*)data, 1); 
	port = (INT8U)p_th_arg->Arg_v;
		
	idx = MAX_THREAD_PRIORITY -(INT32U)p_th_arg->priority; 
	if(idx >= 0)
	{
	//	m_Task_Info[idx].task_id = local_gettid();
	//	Task_Cnt_Init(idx);
		sprintf( str, "UDP_Sel_Proc_Task (P %d)", port);
		strcpy(m_Task_Info[idx].task_name, str);
	}
	/*	
	sprintf( str, "[DAU]---(%02d/%d)--- %s [%d]\n", 
		idx, 
		Max_Task_Cnt,
		m_Task_Info[idx].task_name, 
		m_Task_Info[idx].task_id );		
	D_Printf( DEBUG_SYSTEM, "%s", str);
	*/	
	syslog( LOG_INFO, "%s", str);
	//--------------------------------------------------------
	
	switch(port)
	{
		case COMM_UDP_NODE:
			task_flag = &m_Env.task_flag[_TASK_UDP_SEL_PROC_NODE_];		///// YCSO TASK END
			task_END = &m_Env.task_END_status[_TASK_UDP_SEL_PROC_NODE_];		///// YCSO TASK END
			task_id = _TASK_UDP_SEL_PROC_NODE_;
			break;
			
		case COMM_UDP_LS: 
			task_flag = &m_Env.task_flag[_TASK_UDP_SEL_PROC___LS_];		///// YCSO TASK END
			task_END = &m_Env.task_END_status[_TASK_UDP_SEL_PROC___LS_];		///// YCSO TASK END
			task_id = _TASK_UDP_SEL_PROC___LS_;
			break;
	#ifdef __DEL_UDP_REMS__
	#else
		case COMM_UDP_REMS: 
			task_flag = &m_Env.task_flag[_TASK_UDP_SEL_PROC_REMS_];		///// YCSO TASK END
			task_END = &m_Env.task_END_status[_TASK_UDP_SEL_PROC_REMS_];		///// YCSO TASK END
			task_id = _TASK_UDP_SEL_PROC_REMS_;
			break;						
	#endif
		default:
			task_flag = NULL;		///// YCSO TASK END
			task_END = NULL;		///// YCSO TASK END
		break;
	}

	fd_set readfds; 

	OSTimeDly( OS_DELAY_1sec ); // Ethernet 초기화 시간 필요

	if( task_flag != NULL)		///// YCSO TASK END
	{
		while(*task_flag )		///// YCSO TASK END
		{
			Update__Task_Count(task_id);
			FD_ZERO(&readfds); // ready for input, delete all fds froem readfds set
			last_fd = 0;		
			
			if (udp_pop_fd[port] > 0)
			{
				FD_SET(udp_pop_fd[port], &readfds); ////////////// set check pipe fd
				if (last_fd < udp_pop_fd[port])
					last_fd = udp_pop_fd[port];
			}		

			if (last_fd == 0)
			{
				sleep(1);

				sprintf( str, "[RCU] !! UDP_Sel_Proc_Task(%d) sleep.\n", port);
				D_Printf( DEBUG_SYSTEM,"%s", str);
				syslog( LOG_ERR, "%s", str);

				if (udp_pop_fd[port] <= 0)
				{
					sprintf( str, "[RCU] !! udp_pop_fd[%d] <= 0, pipe re-create... \n", port);
					D_Printf( DEBUG_SYSTEM,"%s", str);
					syslog( LOG_ERR, "%s", str);
					
					pipe_create(port);
				}
				
				continue;
			}

			tv.tv_sec  = 0;
			tv.tv_usec = 10000; // 10[ms] 2019.05.16, 300 -> 10
			state = select( last_fd+1, &readfds, NULL, NULL, &tv);

			switch(state)
			{
				case -1:
					sprintf( str, "[RCU] !! UDP_Sel_Proc_Task(%d) Err#1.\n", port);
					D_Printf( DEBUG_SYSTEM,"%s", str);
					syslog( LOG_ERR, "%s", str);
					break;
				
				case 0: // timeout
					break;

				default:				
					if(FD_ISSET(udp_pop_fd[port], &readfds))
					{
						re = 0; // init
						read(udp_pop_fd[port], &re, 1);  //////////// pipe read

						switch(re)
						{
							case COMM_UDP_NODE: 
								Proc_UDP_GUI_Pkt(); 
								break;
								
							case COMM_UDP_LS  : 
								Proc_LSH_Node_Pkt();
								break;
						#ifdef __DEL_UDP_REMS__
						#else
							case COMM_UDP_REMS: 
								Proc_REMS_Node_Pkt();
								break;						
						#endif
							default:
								D_Printf( DEBUG_SYSTEM,"[RCU] !! FD_ISSET %d", re);
							break;
						}
					}				
					break;
			}
		}
	}
	
	if( task_END != NULL )		
		*task_END = _CLR_;		
	
	pthread_exit(NULL);
}
//------------------------------------------------------------------------------
	
/*
fcntl : open()으로 이미 열려 있는 파일의 특성을 변경위해서
int fcntl(int fd, int cmd, int arg); 

    1] F_DUPFD : 기존 fd를 복제한다.
    2] F_GETFD : fd플래그를 조회
    3] F_SETFD : fd플래그를 설정
    4] F_GETFL : 파일 상태 플래그를 조회
    5] F_SETFL : 파일 상태 플래그를 설정
*/

void pipe_create(INT8U i)
{
    int re;
    int fds[2];

	if(i < COMM_NUM)
	{
	    udp_pop_fd[i] = -1;
	    udp_push_fd[i] = -1;
	    
		re = pipe(fds);
		
		if (re >= 0)
		{
			fcntl(fds[0], F_SETFL, O_NONBLOCK);
			fcntl(fds[1], F_SETFL, O_NONBLOCK);
		
			udp_pop_fd[i]  = fds[0]; // reading
			udp_push_fd[i] = fds[1]; // writing
		}
    }
}


void udp_rx_que_pipe_create(void)
{	
#if 1
	int i;
	for(i = 0; i < COMM_NUM; i++)
		pipe_create(i);
#else	
    int re;
    int fds[2];
    int i;

	for(i = 0; i < COMM_NUM; i++)
	{
		pipe_create(i);
		/*
	    udp_pop_fd[i] = -1;
	    udp_push_fd[i] = -1;
	    
		re = pipe(fds);
		
		if (re >= 0)
		{
			fcntl(fds[0], F_SETFL, O_NONBLOCK);
			fcntl(fds[1], F_SETFL, O_NONBLOCK);
		
			udp_pop_fd[i]  = fds[0]; // reading
			udp_push_fd[i] = fds[1]; // writing
		}
		*/
    }
#endif
}

void udp_rx_que_pipe_destroy(void)
{	
	int i;	

    for(i = 0; i < COMM_NUM; i++)
	{	    
		if (udp_pop_fd[i]  > 0) close(udp_pop_fd[i]);        
	    if (udp_push_fd[i] > 0) close(udp_push_fd[i]);

	    udp_pop_fd[i]  = 0;
    	udp_push_fd[i] = 0;
    }
}

int pipe_udp_push_write (INT8U comm_num )
{
	char req_event = comm_num;

	if(comm_num < COMM_NUM)
	{	
		if (udp_push_fd[comm_num] > 0)
		{
			write(udp_push_fd[comm_num], &req_event, 1);
		}
	}
	
	return 0;
}


//-------------------------------------------------------------------------------------


/* EOF */



