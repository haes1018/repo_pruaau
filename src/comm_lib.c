/////////////////////////////
//	COMM_LIB.C
/////////////////////////////
#define COMM_LIB_GLOBALS
#include "includes.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <linux/udp.h>

/////////////////////////////

#define MAX_UDP_RX_QUE	8

typedef struct
{
    INT8U	port;
    INT8U   reserved;

	INT16U	front;
    INT16U	rear;
    INT8U	data[MAX_UDP_RX_QUE][MAX_COMM_QUE];
    INT16U	recv_len[MAX_UDP_RX_QUE];

    struct sockaddr_in recv_addr[MAX_UDP_RX_QUE];
//    INT32U recv_addr_size[MAX_UDP_RX_QUE];

} COMM_UDP_RX_QUE;

COMM_UDP_RX_QUE		udp_rx_que[COMM_NUM];

COMM_QUE		comm_que[COMM_NUM];
COMM_TX_BUFF	comm_tx_buff[COMM_NUM];
pthread_mutex_t	Tx_Wait_Evt[COMM_NUM];
pthread_mutex_t	UDP_Rx_Evt[COMM_NUM];

INT8U UART_ERR_CO[COMM_NUM] = {0,};

static PTR_GetFrame*	pGetFrame[COMM_NUM]; // 해당 Comm Port Frame 분석 함수

COMM_INIT_TYTE comm_init[COMM_NUM]; // Comm Port 초기화 정보


INT32U Get_IP32( INT8U comm_port )
{
	if(comm_port >= COMM_NUM) return 0;
	
	return comm_init[comm_port].recv_ip;
}

void Re_Init_Comm( INT8U port )
{
	UART_ERR_CO[port] = 0;
	if( comm_init[port].comm_fd != -1 )
	{
		printf("Comm(%d) Device Close %s\n", port, comm_init[port].DevName );
		close(comm_init[port].comm_fd);
		comm_init[port].comm_fd = -1;
	}
	Init_Comm( port );
	D_Printf( DEBUG_SYSTEM, "Re_Init_Comm %d\n", port );	
}


void Close_UART_Comm( INT8U port )
{	
	if( comm_init[port].comm_fd != -1 )
	{
		printf("Close Comm(%d) Device Close %s\n", port, comm_init[port].DevName );
		close(comm_init[port].comm_fd);
		comm_init[port].comm_fd = -1;
	}
}


// UDP IP ReSetting
void UDP_IP_Re_Setting( INT8U port, INT32U ip_32 )
{
	if( port < COMM_NUM )
	{
		comm_init[port].recv_ip = ip_32; 
	}
}

void Comm_Que_Dbg(INT8U ComPort, INT8U type)
{
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Port%d] Timeout. Cmd 0x%X (mode/front/rear/sync/len : %d,%d,%d,%d,%d)",
		ComPort, type,
		comm_que[ComPort].comm_mode,
		comm_que[ComPort].front,
		comm_que[ComPort].rear,
		comm_que[ComPort].sync_len,
		comm_que[ComPort].len);
}

//-----------------------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------------------

#define MAX_UDP_DATA_LEN	1472 // when use REMS

static void TxData( INT8U port, INT8U *buf, INT16U len, struct sockaddr_in* ip )
{
	int err;
	INT16U s_ad, send_len;
	INT16U max_len;	
	INT32U w_len;
	INT16U req_len;	
	INT16U etc_len;	
	int page_num = 0;
	//INT8U cnt = 0;

	if( comm_init[port].comm_fd<=0 ) 
	{
	//	printf( "\r\n!! TxData comm_init[%d].comm_fd %d\r\n\r\n", port, comm_init[port].comm_fd );
	//	syslog( LOG_CRIT, "\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!" );
	//	syslog( LOG_CRIT, "\r\n TxData comm_init[%d].comm_fd %d", port, comm_init[port].comm_fd );
	//	syslog( LOG_CRIT, "\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n" );		
		return;
	}

//	if( comm_init[port].comm_type==COMM_TYPE_UDP ) return;  // UDP 는 Tx Data 미사용


	comm_tx_buff[port].co = 0;
	comm_tx_buff[port].len = comm_tx_buff[port].frame_start + len;
	req_len = comm_tx_buff[port].len; // 2019.04.16

	// 2019.04.16
	if(len > MAX_COM_BUFFER)
	{
		printf(VT_BOLD_RED "\n!!!!! TX SIZE Is MAX !!!!" VT_END_CR);
		return;
	}
	
	memcpy( comm_tx_buff[port].data_start, buf, len );

	//Debug_Comm_Data( port, 1, 0, comm_tx_buff[port].data, comm_tx_buff[port].len );
	Debug_Comm_Data( port, 1, 0, comm_tx_buff[port].data, req_len );
	

	err = 0; // init

	switch( comm_init[port].comm_type )
	{
		case COMM_TYPE_AT91_UART:
		case COMM_TYPE_2550_UART:
			/*
			w_len = write( comm_init[port].comm_fd, &comm_tx_buff[port].data, req_len );

			printf( "[TxData] _YCSO_ port = %d, req_len = %d w_len = %d \n", port, req_len, w_len );
			*/

			page_num = 300;///// 최대 보내기 시도 횟수.
			etc_len = 0;
			send_len = 0;
			while( req_len )
			{
				w_len = write( comm_init[port].comm_fd, &comm_tx_buff[port].data[send_len] , req_len );
				send_len += w_len;
				req_len -= w_len;
				OSTimeDly( 35*OS_DELAY_1msec ); // minimum 30[ms], 115200, 4160byte, 2019.05.07 : 40ms -> 35ms
				page_num--;
				if( page_num == 0 )
				{
					D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[Term %d] len %d/%d page = %d ", port, w_len, etc_len, page_num );
					break;
				}
			}		

			#ifdef __YCSO__TEST2__		
			page_num = 1;
			if( req_len > COM_BUFFER_PAGE_SIZE )
			{
				page_num += ( req_len / COM_BUFFER_PAGE_SIZE );
				cnt = 0;
			}
				
			while(page_num)
			{
//				printf( "[TxData] _YCSO_ port = %d, req_len = %d page_num = %d \n", port, req_len, page_num );

				if( page_num > 1 )
				{
					w_len = write( comm_init[port].comm_fd, &comm_tx_buff[port].data[0 +(COM_BUFFER_PAGE_SIZE*cnt)], COM_BUFFER_PAGE_SIZE );
					req_len = req_len - COM_BUFFER_PAGE_SIZE;
					cnt++;
					if( w_len != COM_BUFFER_PAGE_SIZE )
					{
						D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[Term %d] len %d/%d page = %d ", port, w_len, etc_len, page_num );
						printf( "[Term %d] len %d/%d page = %d ", port, w_len, etc_len, page_num );
						break;
					}
				}
				else
				{
					w_len = write( comm_init[port].comm_fd, &comm_tx_buff[port].data[0 +(COM_BUFFER_PAGE_SIZE*cnt)], req_len );
					if( w_len != req_len )
					{
						D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[Term %d] len %d/%d page = %d ", port, w_len, etc_len, page_num );
						printf( "[Term %d] len %d/%d page = %d \n", port, w_len, etc_len, page_num );
						break;
					}
				}
				page_num--;
				OSTimeDly( 4*OS_DELAY_10msec ); // minimum 30[ms], 115200, 4160byte

				
			}
			#endif
			break;

				/*
				
				w_len = write( comm_init[port].comm_fd, comm_tx_buff[port].data, req_len );	
				etc_len = req_len-w_len;
				
				//D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[port %d] req_len (%d) = w_len (%d) + etc (%d) ", 
				//	port, req_len, w_len, etc_len );
				
				if(etc_len > 0)
				{
					OSTimeDly( 3*OS_DELAY_10msec ); // minimum 30[ms], 115200, 4160byte
					
					//D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[Term %d] w_len %d, etc_len %d ", port, w_len, etc_len );
					
					w_len = write( comm_init[port].comm_fd, &comm_tx_buff[port].data[w_len], etc_len );	
					D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[Term %d] len %d/%d ", port, w_len, etc_len );
					if(w_len < etc_len)
					{
						if( tcdrain( comm_init[port].comm_fd )<0 ) // Error
							err = 1;
					}
				}				
				break;
			#else
			// Tx 시에 4096 씩 보낼때 오류 있어서 마지막 STX 따로 한번..
			
			if( write( comm_init[port].comm_fd, comm_tx_buff[port].data, comm_tx_buff[port].len-1 )<(comm_tx_buff[port].len-1) )
			{
				err = 1;
				D_Printf( DEBUG_SYSTEM, "TxData %d Error %s.", port, strerror(errno) );			
			}
			else
			{

				OSTimeDly( OS_DELAY_1msec );
				if( write( comm_init[port].comm_fd, &comm_tx_buff[port].data[comm_tx_buff[port].len-1], 1 )<1 )
				{
					err = 1;
					D_Printf( DEBUG_SYSTEM, "TxData %d 0-Error %s.", port, strerror(errno) );
				}
				
				if( tcdrain( comm_init[port].comm_fd )<0 ) // Error
				{
				//	printf( "\r\n!! tcdrain comm_init[%d].comm_fd %d\r\n\r\n", port, comm_init[port].comm_fd );
				//	syslog( LOG_CRIT, "\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!" );
				//	syslog( LOG_CRIT, "\r\n tcdrain comm_init[%d].comm_fd %d ", port, comm_init[port].comm_fd );
				//	syslog( LOG_CRIT, "\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n" );
					
					err = 1;
				}
			}
			#endif
			break;
			*/

		 case COMM_TYPE_UDP:
			//	printf( "UDP send %08X : %d\n", ip->sin_addr.s_addr, comm_tx_buff[port].len );
			len = comm_tx_buff[port].len;

			// if( sendto( comm_init[port].comm_fd, comm_tx_buff[port].data, comm_tx_buff[port].len, 0, (struct sockaddr*)ip, sizeof(struct sockaddr)) < comm_tx_buff[port].len )
			//		  err = 1;

			//max_len = MAX_UDP_DATA_LEN; // when REMS
			max_len = MAX_COM_BUFFER;
			
			for( s_ad=0; s_ad<comm_tx_buff[port].len; s_ad+=max_len )
			{
				send_len = comm_tx_buff[port].len-s_ad;
				
				if( send_len > max_len )
				send_len = max_len;
				
				if(sendto(comm_init[port].comm_fd, &comm_tx_buff[port].data[s_ad], send_len, 0, (struct sockaddr*)ip, sizeof(struct sockaddr)) < send_len)
				err = 1;
			}
			/*
			#ifdef __YCSO__SPECTRUM__DEBUG__
			if( port == COMM_UDP_NODE )
				printf("%d ms || UDP Send END here!! ",OSTimerGet_msec(&spectrum_timer));

			#endif
			*/
			break;
		case COMM_TYPE_TCP:
		//	printf( "UDP send %08X : %d\n", ip->sin_addr.s_addr, comm_tx_buff[port].len );
			len = comm_tx_buff[port].len;

			// if( sendto( comm_init[port].comm_fd, comm_tx_buff[port].data, comm_tx_buff[port].len, 0, (struct sockaddr*)ip, sizeof(struct sockaddr)) < comm_tx_buff[port].len )
			//		  err = 1;

			//max_len = MAX_UDP_DATA_LEN; // when REMS
			max_len = MAX_COM_BUFFER;
			
			for( s_ad=0; s_ad<comm_tx_buff[port].len; s_ad+=max_len )
			{
				send_len = comm_tx_buff[port].len-s_ad;
				
				if( send_len > max_len )
				send_len = max_len;
				
				if(send(comm_init[port].comm_fd, &comm_tx_buff[port].data[s_ad], send_len, 0 ) < send_len)
				err = 1;
			}
			break;
	}

	if( err )
	{
		if(comm_init[port].comm_type != COMM_TYPE_UDP)
		{
			if( UART_ERR_CO[port]<100 )
				UART_ERR_CO[port]++;

			D_Printf( DEBUG_SYSTEM, "Comm%d Send Error\n", port );	
		}		
		

		if((UART_ERR_CO[port]>10) && (comm_init[port].comm_type!=COMM_TYPE_UDP))
		{
			D_Printf( DEBUG_SYSTEM, "Comm%d Re_Init_Comm\n", port );	
			Re_Init_Comm( port );
			UART_ERR_CO[port] = 0;
		}
	}
	else
	{
		UART_ERR_CO[port] = 0;
	}

/*
	if( err )
	{
		Re_Init_Comm(port );
		if( UART_ERR_CO[port]<100 )
			UART_ERR_CO[port]++;
		D_Printf( DEBUG_SYSTEM, "Comm%d Send Error\n", port );
	}
	else
	{
		UART_ERR_CO[port] = 0;
	}
*/

}



INT8U Send_DN_Data( INT8U port, INT8U* send, INT16U len, INT8U* ack, INT16U delay )
{
	INT16U i;
	INT8U ret;

	struct sockaddr_in ip;

	ip.sin_family = AF_INET;
	if(comm_init[port].comm_type == COMM_TYPE_UDP)
	{
		ip.sin_addr.s_addr = comm_init[port].recv_ip;
		ip.sin_port=htons(udp_port[port]);		
	}
	else
	{
		ip.sin_addr.s_addr=0;
		ip.sin_port=0;
	}

	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	Init_Que( &comm_que[port] );

	PreTxData(port);
	TxData( port, send, len, &ip );
	EndTxData(port);

	if( ack == NULL )
	{
		pthread_mutex_unlock(&Tx_Wait_Evt[port] );
		return 0;
	}

    for( i=0; i<delay; i++ )
	{
		OSTimeDly( OS_DELAY_10msec );

		ret = (pGetFrame[port])( &comm_que[port], ack );

		if( ret == 2 )
		{
			Recv_DN_Data( port, ack );
			pthread_mutex_unlock(&Tx_Wait_Evt[port] );
			return 1;
		}
	#ifdef __TYPE__PRU__
		else if( ret == 3 ) // 2016.10.28. by Haes
		{
			switch(port)
			{
				case COMM_UART_AMP_1:
				case COMM_UART_AMP_2:
				case COMM_UART_AMP_3:
				case COMM_UART_AMP_4:
					pthread_mutex_unlock(&Tx_Wait_Evt[port] );
				return 0;
			}
		}
	#endif	
	}

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
	return 0;
}


INT8U Send_UP_Data( INT8U port, INT8U* send, INT16U len )
{
	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	PreTxData(port);
	TxData( port, send, len, NULL );
	EndTxData(port);

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );

	return 1;
}


void Send_gREMS_Data( INT8U port, INT8U* dst_ip, INT16U port16,  INT8U *buf, INT16U len )
{
	struct sockaddr_in ip;

	// 2019.10.18
	if(Is__Valid_IP(2, dst_ip)==0) // if invalid
		return;

		

	ip.sin_family = AF_INET;
	ip.sin_addr.s_addr = BYTE_to_INT32L(dst_ip);
	ip.sin_port = htons(port16); 


	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	PreTxData(port);
	TxData( port, buf, len, &ip );
	EndTxData(port);

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
}

// TREE_2019_0521
void Send_UDP_Node_Data( INT8U port, INT8U* dst_ip, INT16U port_num, INT8U *buf, INT16U len )
{
	struct sockaddr_in ip;

	ip.sin_family = AF_INET;
	ip.sin_addr.s_addr = BYTE_to_INT32L(dst_ip);
	ip.sin_port = htons(port_num);


	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	PreTxData(port);
	TxData( port, buf, len, &ip );
	EndTxData(port);

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
}
// END : TREE_2019_0521



void Send_TCP_Data_GUI( INT8U port, INT8U* dst_ip, INT8U *buf, INT16U len)
{


	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	PreTxData(port);
	TxData( port, buf, len, NULL );
//	TxData_TCP( port, buf, len);
	EndTxData(port);

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
}

void Send_UDP_Data_GUI( INT8U port, INT8U* dst_ip, INT8U *buf, INT16U len, INT16U port16 )
{
	struct sockaddr_in ip;

	ip.sin_family = AF_INET;
	ip.sin_addr.s_addr = BYTE_to_INT32L(dst_ip);
	ip.sin_port = htons(port16);


	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	PreTxData(port);
	TxData( port, buf, len, &ip );
	EndTxData(port);

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
}


INT8U Send_Localhost_Data( INT8U port, INT8U* send, INT16U len, INT8U* ack, INT16U delay )
{
	INT16U i;
	INT8U ret;
	struct sockaddr_in ip;

	if(comm_init[port].comm_type == COMM_TYPE_UDP)
	{
		ip.sin_family = AF_INET;
		ip.sin_addr.s_addr=comm_init[port].recv_ip;	
		ip.sin_port=htons(DTU__LoHost_PORT);
	}
	else
	{
		ip.sin_family = AF_INET;
		ip.sin_addr.s_addr=0;
		ip.sin_port=0;
	}

	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	Init_Que( &comm_que[port] );

	PreTxData(port);
	TxData( port, send, len, &ip );
	EndTxData(port);

	if( ack == NULL )
	{
		pthread_mutex_unlock(&Tx_Wait_Evt[port] );
		return 0;
	}

    for( i=0; i<delay; i++ )
	{
		OSTimeDly( OS_DELAY_10msec );

		ret = (pGetFrame[port])(&comm_que[port], ack);

		if(ret == 2)
		{
			Recv_DN_Data( port, ack );
			pthread_mutex_unlock(&Tx_Wait_Evt[port] );
			return 1;
		}	
	}

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
	return 0;
}

///// YCSO added START
void Send_Tx_Only_Data(INT8U port, INT8U *buf, INT16U len )
{
	switch(port)
	{
		case COMM_UDP_NODE:
	#ifdef __DEL_UDP_REMS__
	#else	
		case COMM_UDP_REMS:
	#endif	
			Send_UDP_Data_GUI( port , comm_info[port].ip, buf, len, comm_info[port].port);
			break;

		case COMM_TERM:
			Send_UP_Data( port, buf, len );
			break;

		default :
			break;
	}	

}


///// YCSO added END



INT8U Check_UP_Data( INT8U port, INT8U* txf, INT8U* rxf, INT8U delay )
{
	INT16U len;
	INT8U i, ret;
// TREE_2019_0521
	INT16U port_no;	
// END : TREE_2019_0521


    struct sockaddr_in* pip_addr;

	ret =0;
    for( i=0; i<delay; i++ )
	{

/*
		if( comm_init[port].comm_type == COMM_TYPE_UDP )
		{
			if( UDP_Check(port, &comm_que[port]) == 0 )
				break;
		}
*/

		ret = (pGetFrame[port])( &comm_que[port], rxf );


		if( ret == 0 ) break;  // 패킷 받는 중인것도 아니고, 받기 완료된 것도 아님.

		if( ret == 2 )	// 패킷 하나 받기 완료됨.
		{
			if( comm_init[port].comm_type == COMM_TYPE_UDP )
			{
				pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
				// TREE_2019_0521
				port_no = ntohs(pip_addr->sin_port);
				len = Recv_UP_Data( port, txf, rxf, (INT8U*)&(pip_addr->sin_addr.s_addr), port_no );
				// END : TREE_2019_0521
				
				print_agent( DEBUG_SYSTEM, VT_BLINK_RED, " Check_UP_Data -- TERM, UDP ?"); // check 2019.08.26
			}
			// TREE_2019_0521
			else len = Recv_UP_Data( port, txf, rxf, NULL, 0 );
			// END : TREE_2019_0521
			if( len )
			{
				#ifdef __TYPE__AAU__ // ANTS-AAU for LED
				AAU_Polling_Aivie();
				#endif
				
				Send_UP_Data( port, txf, len );
			}

			break;
		}
		OSTimeDly( OS_DELAY_10msec );
	}

	if( ret == 1 ) // Time Out
		Init_Que( &comm_que[port] ); // 타임아웃되었으므로 DMA Rx Queue를 초기화한다.

//	if( comm_init[port].comm_type == COMM_TYPE_UDP )
//		ARP_Refresh(port);

	return 0;
}
///// YCSO added SIM 

INT8U Check_Node_SIM_Data( INT8U port, INT8U* rxf, INT16U delay )
{
	INT8U ret;
	int i;

	if( comm_init[port].comm_type != COMM_TYPE_TCP ) return 0;

	ret =0;
    for( i=0; i<delay; i++ )
	{
		ret = (pGetFrame[port])( &comm_que[port], rxf );


		if( ret == 0 ) break;  // 패킷 받는 중인것도 아니고, 받기 완료된 것도 아님.


		if( ret == 2 )	// 패킷 하나 받기 완료됨.
		{
//			pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
//			memcpy( recv_ip, (INT8U*)&(pip_addr->sin_addr.s_addr), 4 );
//			*port_no = ntohs(pip_addr->sin_port);
			
			break;
		}

		OSTimeDly( OS_DELAY_1msec );
	}

	if( ret == 1 ) // Time Out
	{
//		pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
		/*
			printf("\n comm_port[%d] : time out IP %s \n", 
				port, 
				inet_ntoa(pip_addr->sin_addr) );	
				
			D_Printf( DEBUG_SYSTEM, "\n comm_port[%d] : time out IP %s ", 
				port, 
				inet_ntoa(pip_addr->sin_addr) );	
		*/
		Init_Que( &comm_que[port] );
	}
	if( ret == 2 )
		return 1;
		
	return 0;
}


///// YCSO END

INT8U Check_Node_Data( INT8U port, INT8U* rxf, INT16U delay, INT8U* recv_ip, INT16U* port_no)
{
	INT8U ret;
	int i;
    struct sockaddr_in* pip_addr;

	if( comm_init[port].comm_type != COMM_TYPE_UDP ) return 0;

	ret =0;
    for( i=0; i<delay; i++ )
	{
		ret = (pGetFrame[port])( &comm_que[port], rxf );

		if( ret == 0 ) break;  // 패킷 받는 중인것도 아니고, 받기 완료된 것도 아님.
		if((ret == 2)||(ret == 0xFF))	// 패킷 하나 받기 완료됨.
		{
			pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
			memcpy( recv_ip, (INT8U*)&(pip_addr->sin_addr.s_addr), 4 );
			*port_no = ntohs(pip_addr->sin_port);

			//D_Printf( DEBUG_SYSTEM, "Check_Node_Data[%d] : delay %d\n", i);
			if(ret == 0xFF) return 0xFF;
			else return 1;
		}
		OSTimeDly( OS_DELAY_1msec );
	}

	if( ret == 1 ) // Time Out
	{
		pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
		/*
			printf("\n comm_port[%d] : time out IP %s \n", 
				port, 
				inet_ntoa(pip_addr->sin_addr) );	
				
			D_Printf( DEBUG_SYSTEM, "\n comm_port[%d] : time out IP %s ", 
				port, 
				inet_ntoa(pip_addr->sin_addr) );	
		*/
		Init_Que( &comm_que[port] );
	}
	if( ret == 2 )
		return 1;
		
	return 0;
}



INT8U Check_LSH_Data( INT8U port, INT8U* rxf, INT16U delay, INT8U* recv_ip, INT16U* recv_port )
{
	INT16U i;
	INT8U ret = 0;

    struct sockaddr_in* pip_addr;
   
	if( comm_init[port].comm_type != COMM_TYPE_UDP )
		return 0;
	
    for( i=0; i<delay; i++ )
	{
		ret = (pGetFrame[port])( &comm_que[port], rxf );

		if( ret == 0 ) break;  // 패킷 받는 중인것도 아니고, 받기 완료된 것도 아님.

		if( ret == 2 )	// 패킷 하나 받기 완료됨.
		{
			#if 0
			if( comm_init[port].comm_type == COMM_TYPE_UDP )
			{
				pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
				len = Recv_LSH_Data( port, txf, rxf, (INT8U*)&(pip_addr->sin_addr.s_addr) );
			}
			else len = Recv_LSH_Data( port, txf, rxf, NULL );

			if( len )
			{
				Send_LSH_Data( port, txf, len );
			}
			#endif

			pip_addr = (struct sockaddr_in*)&comm_que[port].recv_addr;
			memcpy(recv_ip, (INT8U*)&(pip_addr->sin_addr.s_addr), 4);
			*recv_port = ntohs(pip_addr->sin_port);

			break;
		}
		OSTimeDly( OS_DELAY_10msec );
	}

	Init_Que( &comm_que[port] ); // 2019.03.25 : without Init_Que, abnormal...!!	
	
	if( ret == 2 )
	{
		Init_Que( &comm_que[port] ); // 2019.03.25 : without Init_Que, abnormal...!!
		return 1;
	}
	else if( ret == 1 ) // Time Out
	{
		//D_Printf( DEBUG_SYSTEM, "Check_LSH_Data : time out...");
	}
	return 0;
}

void Send_LSH_Data( INT8U port, INT8U* dst_ip, INT16U dst_port, INT8U *buf, INT16U len )
{
	struct sockaddr_in ip;

	ip.sin_family = AF_INET;
	ip.sin_addr.s_addr = BYTE_to_INT32L(dst_ip);	
	ip.sin_port = htons(dst_port);

	pthread_mutex_lock(&Tx_Wait_Evt[port]);

	PreTxData(port);
	TxData( port, buf, len-MAX_LSH_SYNC_SIZE, &ip );
	EndTxData(port);

	pthread_mutex_unlock(&Tx_Wait_Evt[port] );
}


////////////////////////////////////////////////////////////////////////////

#define __ECP	0x0D

////// YCSO Change & added START 190716
#define BT_PARRING_BASIC	"skt5gxaau"
//#define BT_NAME_BASIC		"5GX-AAU_ANTS"
///// YCSO change 190827
#define BT_NAME_BASIC		"AAU"
////// YCSO 여기 바뀌었음.
// 2019.08.15(v08.14.99)
void Set_BT_Name( INT8U BT_init_status )
{
	#ifdef __TYPE__AAU__

	INT8U LS_ID = 0;
	INT8U LS_PORT = 0;
	INT8U RU_ID = 0;
	INT8U cascade_ID = 0;
	char* AAU_Bt = "ANTS AAU";
	///// YCSO change 190827
	INT8U serial[10];


	memset(m_St.BT_Device_Code, 0, BT_DEVICE_NAME_SIZE);

	OSTimeDly(OS_DELAY_1sec);

	printf("_YCSO_ [Set_BT_Name] %d \n",BT_init_status);

	memcpy(serial, m_St.LS_Regist_Num, 10);
	switch( BT_init_status )
	{
		case BT_SERIAL:
			snprintf((char*)m_St.BT_Device_Code,BT_DEVICE_NAME_SIZE, "%s%s", AAU_Bt, (char*)serial);
			break;

		case BT_CASCADE:
			LS_ID = AufoConfigFlag.LS_ID;

			if(m_St.Ref_CPRI_No == CPRI_0)
				LS_PORT = m_St.g_LS_Port_No[CPRI_0];
			else
				LS_PORT = m_St.g_LS_Port_No[CPRI_1];
			
			cascade_ID	= AufoConfigFlag.RE_Cascade_ID;
			///// YCSO change 190827
			//snprintf( (char*)m_St.BT_Device_Code, BT_DEVICE_NAME_SIZE, "%s_%02d%02d_%d",BT_NAME_BASIC,LS_ID, LS_PORT, cascade_ID);
			snprintf( (char*)m_St.BT_Device_Code, BT_DEVICE_NAME_SIZE, "%s_%02d%02d_%d-%s",BT_NAME_BASIC,LS_ID, LS_PORT, cascade_ID,serial);
			break;

		case BT_RU_ID:
			LS_ID	= AufoConfigFlag.LS_ID;

			if(m_St.Ref_CPRI_No == CPRI_0)
				LS_PORT = m_St.g_LS_Port_No[CPRI_0];
			else
				LS_PORT = m_St.g_LS_Port_No[CPRI_1];

			RU_ID 		= AufoConfigFlag.RU_Id;

			cascade_ID	= AufoConfigFlag.RE_Cascade_ID;

			///// YCSO change 190827
			snprintf( (char*)m_St.BT_Device_Code, BT_DEVICE_NAME_SIZE, "%s_%02d%02d_%d%d-%s",BT_NAME_BASIC,LS_ID, LS_PORT, RU_ID, cascade_ID,serial);
			break;

		default:
			memcpy(&m_St.BT_Device_Code[0], &m_St.LS_Regist_Num[0], 10);
			break;
	}

/*
	LS_ID	= AufoConfigFlag.LS_ID;

	if(m_St.Ref_CPRI_No == CPRI_0)
		LS_PORT = m_St.g_LS_Port_No[CPRI_0];
	else
		LS_PORT = m_St.g_LS_Port_No[CPRI_1];

	RU_ID 		= AufoConfigFlag.RU_Id;

	cascade_ID	= AufoConfigFlag.RE_Cascade_ID;

	snprintf( (char*)m_St.BT_Device_Code, BT_DEVICE_NAME_SIZE, "%s_%02d%02d_%d%d",BT_NAME_BASIC,LS_ID, LS_PORT, RU_ID, cascade_ID);

	m_Env.is_eep_write = EEP_WRITE_CODE;

	m_Env.Bluetooth_re_Init_Req = _SET_;
*/
/*		
	printf("TEST SET NAME 1 %s****************\n, ",m_St.BT_Device_Code);
	hex_dump(m_St.BT_Device_Code, BT_DEVICE_NAME_SIZE);
*/
	#endif

}

#ifdef __YCSO__BT__CMD__ ///191126 YCSO added 
void send_BT_mode_CMD(void)
{
	INT8U data[64] = {0};

	data[0]		= '+';
	data[1] 	= '+';
	data[2] 	= '+';
	data[3] 	= __ECP;	
	//-----------------------
	data[4] 	= 'A';
	data[5] 	= 'T';
	data[6] 	= '+';
	data[7] 	= 'B';
	data[8] 	= 'T';
	data[9] 	= 'M';
	data[10] 	= 'O';
	data[11] 	= 'D';	
	data[12] 	= 'E';
	data[13] 	= ',';
	data[14] 	= '3';
	data[15] 	= __ECP;	
	//-----------------------
	data[16]	= '+';
	data[17] 	= '+';
	data[18] 	= '+';
	data[19] 	= __ECP;
	//-----------------------
	write( comm_init[COMM_BT].comm_fd, &data[0], 20 );

//	check_BT_return();	
}
#endif

void BT_Init(INT8U BT_req)
{
	#ifdef __TYPE__AAU__ // 5G-AAU-Only	
	//Init Mode Command
	///// YCSO change Data size 30 -> 32
	///// YCSO change Data size 32 -> 64
	INT8U Data[64];

	int x = 0;

	BT_RESET();
	OSTimeDly( OS_DELAY_1sec );
	BT_SET();

	Data[0] = '+';
	Data[1] = '+';
	Data[2] = '+';
	Data[3] = __ECP;


	//PcSer->PutStr((INT8U *)&Data[0], 4, ENABLE, NULL);
	//BT_TxData(COMM_BT, (INT8U *)&Data[0], 4, NULL);
	write( comm_init[COMM_BT].comm_fd, &Data[0], 4 );


	OSTimeDly(OS_DELAY_500msec);

	//Device Name Command

	Data[0] = 'A';
	Data[1] = 'T';
	Data[2] = '+';
	Data[3] = 'B';
	Data[4] = 'T';
	Data[5] = 'N';
	Data[6] = 'A';
	Data[7] = 'M';	
	Data[8] = 'E';
	Data[9] = '=';
	Data[10] = '"';

	switch(BT_req)
	{
		case BT_SERIAL:
			for( x = 0; x < 18; x++)
			{
				Data[11+x] = m_St.BT_Device_Code[x];
				if( Data[11+x] == 0x00 )
					Data[11+x] = ' ';
			}
			Data[29] = '"';
			Data[30] = __ECP;
			write( comm_init[COMM_BT].comm_fd, &Data[0], 31 );
			break;

		///// YCSO change 190827

		case BT_CASCADE:
			/*
			memcpy(&Data[11], m_St.BT_Device_Code, 19);
			Data[30] = '"';
			Data[31] = __ECP;
			write( comm_init[COMM_BT].comm_fd, &Data[0], 32 );
			*/
			memcpy(&Data[11], m_St.BT_Device_Code, 20);
			Data[31] = '"';				//// YCSO changed 190830
			Data[32] = __ECP;			//// YCSO changed 190830
			write( comm_init[COMM_BT].comm_fd, &Data[0], 33 );

			break;

		case BT_RU_ID:
			memcpy(&Data[11], m_St.BT_Device_Code, 21);
			Data[32] = '"';				//// YCSO changed 190830
			Data[33] = __ECP;			//// YCSO changed 190830
			write( comm_init[COMM_BT].comm_fd, &Data[0], 34 );
			break;

		default:
			Data[11] = 'A';
			Data[12] = 'N';
			Data[13] = 'T';
			Data[14] = 'S';
			Data[15] = ' ';
			Data[16] = 'A';
			Data[17] = 'A';
			Data[18] = 'U';
			Data[19] = '"';
			Data[20] = __ECP;
			write( comm_init[COMM_BT].comm_fd, &Data[0], 21 );
			break;
	}

#if 0			
	if( m_St.BT_Device_Code[0] == 0xff )
	{
		//Device Name
		Data[11] = 'A';
		Data[12] = 'N';
		Data[13] = 'T';
		Data[14] = 'S';
		Data[15] = ' ';
		Data[16] = 'A';
		Data[17] = 'A';
		Data[18] = 'U';
		 
		//Device Name End
	
		///// YCSO added START 190411
		///// YCSO change 190416
		///// BT로 Data 통신 할 때에 0x00 '\0' 값이 있으면 안됨.
		//Device Name + serial Number
		if( m_Eep.LS_Regist_Num[0] != '\0' )
		{
			for( x = 0; x < 10; x++)
			{
				Data[19+x] = m_Eep.LS_Regist_Num[x];
				if( Data[19+x] == 0x00 )
					Data[19+x] = ' ';
			}
			Data[29] = '"';
			Data[30] = __ECP;
			write( comm_init[COMM_BT].comm_fd, &Data[0], 31 );
			memcpy( m_St.BT_Device_Code, &Data[11], 18);
			m_St.BT_Device_Code[18] = '\0';
		}

		else
		{
			Data[19] = '"';
			Data[20] = __ECP;
			write( comm_init[COMM_BT].comm_fd, &Data[0], 21 );

		}
	}

	else
	{
		memcpy(&Data[11], m_St.BT_Device_Code, 20);
		Data[31] = '"';
		Data[32] = __ECP;
		write( comm_init[COMM_BT].comm_fd, &Data[0], 33 );
/*
		printf("TEST NAME 1 %s****************\n, ",m_St.BT_Device_Code);
		hex_dump(Data, 33);
*/
	}
#endif
	
/////	YCSO added END 
	//PcSer->PutStr((INT8U *)&Data[0], 16, ENABLE, NULL);
	//BT_TxData(COMM_BT, (INT8U *)&Data[0], 16, NULL);

	//PcSer->PutStr((INT8U *)&Data[0], 16, ENABLE, NULL);
	//BT_TxData(COMM_BT, (INT8U *)&Data[0], 16, NULL);
//	write( comm_init[COMM_BT].comm_fd, &Data[0], 21 );


	OSTimeDly(OS_DELAY_500msec);

	//Device Pin code Command

	Data[0] = 'A';
	Data[1] = 'T';
	Data[2] = '+';
	Data[3] = 'C';
	Data[4] = 'H';
	Data[5] = 'P';
	Data[6] = 'A';
	Data[7] = 'S';
	Data[8] = 'S';
	Data[9] = '=';

	if( m_Eep.BT_Pairing_Code[0] == 0xff ) /// 초기값.
	{
		//Device Pin code	
		Data[10] = 's';
		Data[11] = 'k';
		Data[12] = 't';
		Data[13] = '5';
		Data[14] = 'g';
		Data[15] = 'x';
		Data[16] = 'a';
		Data[17] = 'a';
		Data[18] = 'u';
		//Device Pin code End
		Data[19] = __ECP;

		memcpy(m_Eep.BT_Pairing_Code, &Data[10], 9);
		m_Eep.BT_Pairing_Code[9] = '\0';
		//PcSer->PutStr((INT8U *)&Data[0], 15, ENABLE, NULL);
		//BT_TxData(COMM_BT, (INT8U *)&Data[0], 15, NULL);
		write( comm_init[COMM_BT].comm_fd, &Data[0], 20 );
		m_Env.is_eep_write = EEP_WRITE_CODE;
/*
		printf("TEST PASS 1 ****************\n");
		hex_dump(Data, 10+x+1);
*/
	}

	else
	{
		for( x = 0; x < BT_PAIRING_CODE_SIZE; x++)
		{
			if( m_Eep.BT_Pairing_Code[x] == '\0' )
				break;
			Data[10+x] = m_Eep.BT_Pairing_Code[x];
		}

		Data[10+x] = __ECP;
		write( comm_init[COMM_BT].comm_fd, &Data[0], 10+x+1 );
/*		
		printf("TEST PASS 2 ****************\n");
		hex_dump(Data, 10+x+1);
*/
	}


	OSTimeDly(OS_DELAY_500msec);

	//Act Command

	Data[0] = 'A';
	Data[1] = 'T';
	Data[2] = '+';
	Data[3] = 'B';
	Data[4] = 'T';
	Data[5] = 'M';
	Data[6] = 'O';
	Data[7] = 'D';	
	Data[8] = 'E';
	Data[9] = ',';
	Data[10] = '3';
	Data[11] = __ECP;
	
	//PcSer->PutStr((INT8U *)&Data[0], 12, ENABLE, NULL);
	//BT_TxData(COMM_BT, (INT8U *)&Data[0], 12, NULL);
	write( comm_init[COMM_BT].comm_fd, &Data[0], 12 );
	
	OSTimeDly(OS_DELAY_500msec);

	
	//BT Reset  Command

	Data[0] = 'A';
	Data[1] = 'T';
	Data[2] = 'Z';
	Data[3] = __ECP;


	//PcSer->PutStr((INT8U *)&Data[0], 4, ENABLE, NULL);
	//BT_TxData(COMM_BT, (INT8U *)&Data[0], 4, NULL);
	write( comm_init[COMM_BT].comm_fd, &Data[0], 4 );

	
	#endif	


	D_Printf( DEBUG_SYSTEM, "BlueTooth init done." );


}

////// YCSO Change & added END 190716


////////////////////////////////////////////////////////////////////////////


void Set_Frame_Func( INT8U port, PTR_GetFrame* pfunc )
{
	pGetFrame[port] = pfunc;

}


int AT91_UART_Init( char* dev_fname, INT32U baud, INT8U port )
{
	struct termios newtio;
	int uart_fd;
	        //핸들과 termios 구조체를 선언한다.

	uart_fd = open( dev_fname, O_RDWR | O_NOCTTY |O_NDELAY);//파일을 연다. 'O_NONBLOCK' 필요한듯.
	        //읽기/쓰기 모드로 모뎀장치를 연다.(O_RDWR)
	        //데이터 전송시에 <ctrl>-c 문자가 오면 프로그램이 종료되지 않게
	        //controlling tty가 안되도록 한다(O_NOCTTY)
	if(uart_fd < 0)
	{
        syslog( LOG_ERR,"Serial Open Fail-(%d)", port );
		exit(EXIT_FAILURE);
        return uart_fd;
	}

//	tcgetattr(uart_fd, &oldtio); //현재 열려진 통신 포트의 상태를 읽어온다.
	           //사용이 끝난 후 통신 포트 상태를 이전으로 되돌리기 위해 기록.

	memset( &newtio, 0, sizeof(newtio));
	newtio.c_cflag = CS8 | CLOCAL | CREAD;
//	newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	        //전송속도 , 8N1, 모뎀제어 안함, 문자 수신 가능
//	newtio.c_iflag = IGNPAR | ICRNL; //패리티 에러 문자 바이트 무시

	switch ( baud ){
		case 921600 : newtio.c_cflag |= B921600	; break ;
		case 460800 : newtio.c_cflag |= B460800	; break ;
		case 230400 : newtio.c_cflag |= B230400	; break ;
		case 576000 : newtio.c_cflag |= B57600 	; break ;
		case 38400 	: newtio.c_cflag |= B38400 	; break ;
		case 19200 	: newtio.c_cflag |= B19200 	; break ;
		case 9600 	: newtio.c_cflag |= B9600 	; break ;
		case 4800 	: newtio.c_cflag |= B4800 	; break ;
		case 2400 	: newtio.c_cflag |= B2400 	; break ;
		default		: newtio.c_cflag |= B115200 ; break ;
	}

	newtio.c_iflag = IGNPAR; //패리티 에러 문자 바이트 무시
	newtio.c_oflag = 0;

	newtio.c_lflag = 0;

	tcflush(uart_fd, TCIFLUSH); //통신 수행 이전에 전송되지 않았거나 수신처리
	                           //안된 데이터를 모두 비운다.

	tcsetattr(uart_fd, TCSANOW, &newtio); // 모뎀 라인 초기화, 포트 setting을 마침

	return uart_fd;
}

/*
int set_uart_dtu( int baud, int vtime, int vmin )
// Availible baud : 2400, 4800, 9600, 19200, 38400, 57600, 115200
{
	struct termios tio ;

	memset(&tio, 0, sizeof (tio)) ;
	tio.c_iflag = IGNPAR ; 	// No-parity
	tio.c_oflag = 0 ;
	tio.c_cflag = CS8 | CLOCAL | CREAD ; // No-RTS/CTS

	switch ( baud ){
		case 115200 : tio.c_cflag |= B115200	; break ;
		case 576000 : tio.c_cflag |= B57600 	; break ;
		case 38400 	: tio.c_cflag |= B38400 	; break ;
		case 19200 	: tio.c_cflag |= B19200 	; break ;
		case 9600 	: tio.c_cflag |= B9600 		; break ;
		case 4800 	: tio.c_cflag |= B4800 		; break ;
		case 2400 	: tio.c_cflag |= B2400 		; break ;
		default		: tio.c_cflag |= B115200 	; break ;
	}

	// Input mode ( non-canonical, no echo )
	tio.c_lflag = 0 ;
	tio.c_cc[VTIME] = vtime ;	// read timeout ( 0.1sec[100msec] unit )
	tio.c_cc[VMIN] = vmin ;		// wait until minimum vmin character received..

	tcflush( fDev_uartDTU, TCIFLUSH );
	tcsetattr( fDev_uartDTU, TCSANOW, &tio );

	return fDev_uartDTU ;
}
*/
///// YCSO added START
int tcp_Client_init( INT16U port , INT8U* ip_addr)
{
	int sock = 0;
	struct sockaddr_in rep_addr;
	int error = 0;

	INT32U server_ip = 0;

	memset(&rep_addr, 0, sizeof(rep_addr));
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if( sock <= 0 )
	{
		syslog(LOG_ERR, "[RCU] !! TCP socket(port# %d) open err\n", port);
		return -1;
	}

//	server_ip = BYTE_to_INT32B(ip_addr);
	server_ip = BYTE_to_INT32L(ip_addr);
//	printf("ip %x = %x %x %x %x \n", server_ip, ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);

	rep_addr.sin_family = AF_INET;      /* host byte order */
	rep_addr.sin_port = htons(port);    /* short, network byte order */
	rep_addr.sin_addr.s_addr = server_ip;

//	printf("_YCSO_ [tcp_Client_init] rep_addr.sin_addr.s_addr = %x \n",rep_addr.sin_addr.s_addr);

	if( (error = connect( sock, (struct sockaddr *)&rep_addr, sizeof(rep_addr))) < 0)
	{
		syslog(LOG_ERR, "[RCU] !! TCP socket connect fail (port# %d) err= %d\n", port,error);
		return -1;
	}

	fcntl( sock, F_SETFL, O_NONBLOCK );

	return sock;


}

///// YCSO added END

int udp_init( INT16U port )
{
	int sock;
	struct sockaddr_in rep_addr;	
	//int permission = 1;	
	//unsigned char optval = 0; // data (from my-self) is not received..
	//int bsize = 0, rn, rtn = 0;

	memset(&rep_addr, 0, sizeof(rep_addr));	
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // Create socket

	if( sock == -1 )
	{
		syslog(LOG_ERR, "[RCU] !! socket(port# %d) open err\n", port);
		return -1;
	}

	// broadcast enable	
#ifdef __Disable_Broadcast__
#else
	if(setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (void*)&permission, sizeof(permission)) < 0)
	{
		printf("[RCU] !! broadcast setsock opt fail, port# %d", port );
		syslog(LOG_ERR, "[RCU] !! broadcast setsock opt fail, port# %d", port );
	}
#endif	
	//-----------------------------------
	// below ??
	/////setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (void*)&permission, sizeof(permission));
	//-----------------------------------

	//-----------------------------------
	if(port == NODE_LoHost_PORT) // when localhost
	{
		rep_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		rep_addr.sin_family = AF_INET;
		rep_addr.sin_port = htons(port);
	}
	else
	{		
		rep_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		rep_addr.sin_family = AF_INET;
		rep_addr.sin_port = htons(port);
	}
	//-----------------------------------

	D_Printf(DEBUG_SYSTEM, "[RCU] %s/%d, sock(%d)", inet_ntoa(rep_addr.sin_addr), port, sock);

	//-----------------------------------
	// when broadcast
	//-----------------------------------
		
	////setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&optval, sizeof(optval));
	
	//-----------------------------------

	// Socket Buffer Max
	/*	
	if(service == 0)
	{
		system("echo 4000000 > /proc/sys/net/core/wmem_max");
		system("echo 4000000 > /proc/sys/net/core/rmem_max");

		rtn = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bsize, (socklen_t *)&rn);
		D_Printf(DEBUG_SYSTEM, "send buf size : %d, rtn = %d\n", bsize, rtn);

		bsize = bsize * 10;
		D_Printf(DEBUG_SYSTEM, "send buf size : %d, rtn = %d\n", bsize, rtn);
		rtn = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bsize, (socklen_t)rn);

		rtn = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bsize, (socklen_t *)&rn);
		D_Printf(DEBUG_SYSTEM, "send buf size : %d, rtn = %d\n", bsize, rtn);

		rtn = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bsize, (socklen_t *)&rn);
		D_Printf(DEBUG_SYSTEM, "recv buf size : %d, rtn = %d\n", bsize, rtn);

		bsize = bsize * 10;
		D_Printf(DEBUG_SYSTEM, "recv buf size : %d, rtn = %d\n", bsize, rtn);
		rtn = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bsize, (socklen_t)rn);

		rtn = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bsize, (socklen_t *)&rn);
		D_Printf(DEBUG_SYSTEM, "recv buf size : %d, rtn = %d\n", bsize, rtn);
	}
	*/

	if(bind(sock, (struct sockaddr*)&rep_addr, sizeof(rep_addr)) == -1)
	{
		 syslog(LOG_ERR, "[RCU] bind err : %s", strerror(errno));
		 system("lsof");
		 return -1;
	}
	fcntl( sock, F_SETFL, O_NONBLOCK );
	
	return sock;
}

void RecvCommDataQue( INT8U port )
{
	int max_len, rlen, udp_len =0;
	INT8U* pdata_start;


	if( comm_init[port].comm_fd<=0 ) 
	{
	//	printf( "\r\n!! RecvCommDataQue comm_init[%d].comm_fd %d\r\n\r\n", port, comm_init[port].comm_fd );
	//	syslog( LOG_CRIT, "\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!" );
	//	syslog( LOG_CRIT, "\r\n RecvCommDataQue comm_init[%d].comm_fd %d", port, comm_init[port].comm_fd );
	//	syslog( LOG_CRIT, "\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n" );		
		return;
	}

	while( 1 )
	{
		max_len = MAX_COMM_QUE - comm_que[port].front;

		pdata_start = &comm_que[port].data[comm_que[port].front];

		switch( comm_init[port].comm_type )
		{
			case COMM_TYPE_AT91_UART:
			case COMM_TYPE_2550_UART:
				rlen = read( comm_init[port].comm_fd, pdata_start, max_len );
				break;
				
			case COMM_TYPE_UDP:
//				rlen = recvfrom( comm_init[port].comm_fd, pdata_start, max_len, 0, (struct sockaddr*)&comm_que[port].recv_addr, &comm_que[port].recv_addr_size );
//				comm_que[port].recv_addr_size = sizeof(struct sockaddr_in); // Address Size 141209
//				rlen = recvfrom( comm_init[port].comm_fd, comm_que[port].udp_tmp_data, MAX_COMM_QUE, 0, (struct sockaddr*)&comm_que[port].recv_addr, &comm_que[port].recv_addr_size );

				rlen = GetUDPDataFromUDP_Que( port, comm_que[port].udp_tmp_data, (struct sockaddr*)&comm_que[port].recv_addr );

				udp_len = rlen;

				if( rlen>0 )
				{

					if( rlen>max_len )
					{						
						memcpy( pdata_start, comm_que[port].udp_tmp_data, max_len );
						rlen -= max_len;
						comm_que[port].front = 0;
						memcpy( &comm_que[port].data[0], &comm_que[port].udp_tmp_data[max_len], rlen );
						max_len = MAX_COMM_QUE;
					}
					else
					{
						memcpy( pdata_start, comm_que[port].udp_tmp_data, rlen );
					}
				}
				break;
			///// YCSO ADDED SIM TCP
			case COMM_TYPE_TCP:
				rlen = recv( comm_init[port].comm_fd,  pdata_start, max_len, 0 );
				break;

			///// YCSO added END
				
			default:
				rlen = 0;
				break;
		}
		if( rlen>0 )
		{			
			comm_que[port].front+=rlen;
			comm_que[port].front%=MAX_COMM_QUE;

			if( comm_init[port].comm_type == COMM_TYPE_UDP )
			{
				//D_Printf(DEBUG_SYSTEM, "recv udp[%d] %s (n:%08X, h:0x%08X) : len %d\n", 
				//	port, inet_ntoa(comm_que[port].recv_addr.sin_addr), 
				//	comm_que[port].recv_addr.sin_addr.s_addr, 
				//	ntohl(comm_que[port].recv_addr.sin_addr.s_addr),
				//	rlen );
					
				
				if( comm_que[port].comm_mode )
					Debug_Comm_Data( port, 0, 1, comm_que[port].udp_tmp_data, udp_len );
				else Debug_Comm_Data( port, 1, 1, comm_que[port].udp_tmp_data, udp_len ); // 처음 데이터

			}
			else if( comm_init[port].comm_type == COMM_TYPE_TCP )
			{
				//D_Printf(DEBUG_SYSTEM, "recv udp[%d] %s (n:%08X, h:0x%08X) : len %d\n", 
				//	port, inet_ntoa(comm_que[port].recv_addr.sin_addr), 
				//	comm_que[port].recv_addr.sin_addr.s_addr, 
				//	ntohl(comm_que[port].recv_addr.sin_addr.s_addr),
				//	rlen );
					
				if( comm_que[port].comm_mode )
					Debug_Comm_Data( port, 0, 1, pdata_start, rlen );
				else Debug_Comm_Data( port, 1, 1, pdata_start, rlen ); // 처음 데이터

			}

			else
			{
				if( comm_que[port].comm_mode )
					Debug_Comm_Data( port, 0, 1, pdata_start, rlen );
				else Debug_Comm_Data( port, 1, 1, pdata_start, rlen ); // 처음 데이터
			}

		}

		if( rlen<0 ) rlen = 0;

		if( max_len>rlen ) // 더받을 데이터 없다
			break;
	}
}

// baud_port : Serial 에서는 baud, UDP 는 Port
void Init_Comm_Lib( INT8U port, char* dev_fname, int udp_socket, INT32U comm_type, INT32U baud_port, PTR_GetFrame* pfunc, INT8U* preamble, INT8U preamble_len )
{
	COMM_TX_BUFF*	tx_buff;
	int res;

	if( comm_init[port].Mutex_Init_Flag != 0x140921 )
	{
		res = pthread_mutex_init(&Tx_Wait_Evt[port], NULL);
		if (res != 0) 
		{
			syslog( LOG_ERR, "Mutex initialization failed");
			exit(EXIT_FAILURE);
		}
		
		res = pthread_mutex_init(&UDP_Rx_Evt[port], NULL);
		if (res != 0) 
		{
			syslog( LOG_ERR, "Mutex initialization failed");
			exit(EXIT_FAILURE);
		}
		comm_init[port].Mutex_Init_Flag = 0x140921;
	}


	tx_buff = &comm_tx_buff[port];
	pGetFrame[port] = pfunc;

	// preamble 있으면, DMA송신버퍼에 Write
	if( preamble_len ) memcpy( tx_buff->data, preamble, preamble_len );
	// DMA송신버퍼에서 preamble제외한 실제 데이터 전송 시작 위치(인덱스) 는 preamble_len 부터가 됨.
	tx_buff->frame_start = preamble_len;
	tx_buff->data_start = &tx_buff->data[preamble_len]; // data 멤버중 preamble 다음위치가 실제데이터 시작. 이곳의 포인터를 저장.

	comm_init[port].comm_fd = -1;
	comm_init[port].comm_type = comm_type;

	switch( comm_type )
	{
		case COMM_TYPE_AT91_UART:
			if(dev_fname != NULL)
			{
				comm_init[port].comm_fd = AT91_UART_Init( dev_fname, baud_port, port );
				strcpy( comm_init[port].DevName, dev_fname );
			}
			break;
		case COMM_TYPE_2550_UART:
			if(dev_fname != NULL)
			{
				comm_init[port].comm_fd = AT91_UART_Init( dev_fname, baud_port, port );
				strcpy( comm_init[port].DevName, dev_fname );
			}
			break;
		case COMM_TYPE_UDP:
		case COMM_TYPE_TCP:	////// YCSO added TCP
			if(dev_fname == NULL)
			{
				comm_init[port].comm_fd = (int)udp_socket; // UDP DTU와 NODE Port 소프트적으로 구분 위해 별도 초기화
				comm_init[port].recv_ip = baud_port; // UDP DTU와 NODE Port 소프트적으로 구분 위해 별도 초기화
				
				udp_rx_que[port].front = 0;
				udp_rx_que[port].rear = 0;
				udp_rx_que[port].port = port;
			}

			break;
		
	}
	if( comm_init[port].comm_fd < -1 )
	{
		syslog( LOG_ERR, "Comm Init Fail %d", comm_init[port].comm_fd );
		exit(EXIT_FAILURE);
	}

	comm_que[port].port = port;
	Init_Que( &comm_que[port] );
}

void CheckUART_Error( INT32U port )
{
	(void)port;
/*
	switch( comm_init[port].comm_type )
	{
		case COMM_TYPE_AT91_UART:
		case COMM_TYPE_2550_UART:
			if( comm_init[port].comm_fd<=0 ) // File Close 상태
			{
//				comm_init[port].comm_fd = AT91_UART_Init( comm_init[port].DevName, baud_port );
//				Init_Que( &comm_que[port] );
				if( access( comm_init[port].DevName, F_OK)==0 )
				{
					printf("Device Open %s\n", comm_init[port].DevName );
					OSTimeDly( OS_DELAY_100msec );
					Init_Comm( port );
				}
				else
				{
					OSTimeDly( OS_DELAY_100msec );
				}
			}
			else
			{
				if( access( comm_init[port].DevName, F_OK)!=0 )
				{
					printf("Device Close %s\n", comm_init[port].DevName );
	        		close(comm_init[port].comm_fd);
	        		comm_init[port].comm_fd = -1;
				}
			}
			break;
		case COMM_TYPE_UDP:
			break;
	}
*/
}

INT32U CheckCommFail( void )
{
/*
	INT32U i;
	for( i=0; i<COMM_NUM; i++ )
	{
		if( UART_ERR_CO[i] > 10 )
		{
			UART_ERR_CO[i] = 0;

			if( comm_init[i].comm_fd!=-1 )
			{
				printf("Device Close %s\n", comm_init[i].DevName );
				close(comm_init[i].comm_fd);
				comm_init[i].comm_fd = -1;
			}
			Init_Comm( i );


			return 1;
		}
	}
*/
	return 0;
}

COMM_QUE* GetQUE( INT8U port )
{
	return &comm_que[port];
}

int Get_Comm_fd_Status(INT8U port)
{
	return comm_init[port].comm_fd;
}


/*
// SetPreamble Preamble 제정의
// BLUETOOTH
void SetPreamble( INT8U port, INT8U* preamble, INT8U preamble_len )
{
	COMM_TX_BUFF*	tx_buff;
	tx_buff = &comm_tx_buff[port];

	if( preamble_len ) memcpy( tx_buff->data, preamble, preamble_len );
	tx_buff->frame_start = preamble_len;
	tx_buff->data_start = &tx_buff->data[preamble_len];
}


// BT_수정
void UART_Set_BaudRate( INT8U port, INT32U baud )
{

	switch( comm_init[port].comm_type )
	{
		case COMM_TYPE_AT91_UART:
			Init_STM_UART( port, (USART_TypeDef*)comm_init[port].comm_addr, baud );
			break;
		case COMM_TYPE_2550_UART:
			#ifdef USE_2550_UART
			comm_que[port].dma = NULL;
			baud = ST2550_CLK_DIV/baud;
			Init_ST2550( port, baud, 0x03 );
			ST2550_EnableRx( port );
			#endif
			break;
	}

}
*/

//static INT8U UDP_Rx_Temp_Data[MAX_COMM_QUE];

void Add_UDP_Que( INT8U port, INT8U* r_data, int rlen, struct sockaddr_in* r_addr )
{
	INT16U front;

	pthread_mutex_lock(&UDP_Rx_Evt[port]);

	front = udp_rx_que[port].front;

	memcpy( udp_rx_que[port].data[front], r_data, rlen );
	udp_rx_que[port].recv_len[front] = rlen;
	memcpy( &udp_rx_que[port].recv_addr[front], r_addr, sizeof(struct sockaddr_in) );

	udp_rx_que[port].front ++;
	udp_rx_que[port].front %= MAX_UDP_RX_QUE;

	pipe_udp_push_write(port);	
	pthread_mutex_unlock(&UDP_Rx_Evt[port] );
}



//-------------------------------------------------------------------------------------------------




static INT8U UDP_RX_Data[COMM_NUM][MAX_COMM_QUE];

void RecvUDPDataQue_All(INT8U port)
{
    struct sockaddr_in recv_addr;
    INT32U recv_addr_size;
    int sock_fd;
	int rlen;		
	
	sock_fd = Get_UDP_Sock(port);

	//D_Printf(DEBUG_SYSTEM, "RecvUDPDataQue_All port %08X, sock_fd %d\n", port, sock_fd );

	if(sock_fd < 0) return;

	while( 1 )
	{
		recv_addr_size = sizeof(struct sockaddr_in);
		rlen = recvfrom(sock_fd, UDP_RX_Data[port], MAX_COMM_QUE, 0, (struct sockaddr*)&recv_addr, &recv_addr_size);

		if(rlen > 0)
		{
			/*
			D_Printf(DEBUG_SYSTEM, "recv %08X : %d\n", 
				recv_addr.sin_addr.s_addr, rlen );
				
			D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
				UDP_RX_Data[port][0], 
				UDP_RX_Data[port][1], 
				UDP_RX_Data[port][2],
				UDP_RX_Data[port][3] );
				
			D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
				UDP_RX_Data[port][4], 
				UDP_RX_Data[port][5], 
				UDP_RX_Data[port][6], 
				UDP_RX_Data[port][7] );
			*/

			Add_UDP_Que( port, UDP_RX_Data[port], rlen, &recv_addr );		
		}
		else break;		
	}
}
//-------------------------------------------------------------------------------------------------
/*
void RecvUDPDataQue( void )
{
    struct sockaddr_in recv_addr;
    INT32U recv_addr_size;
    int sock_fd;
	int rlen;
		
	sock_fd = Get_UDP_fd(COMM_UDP_NODE);

	if(sock_fd < 0) return;

	while( 1 )
	{
		recv_addr_size = sizeof(struct sockaddr_in); // Address Size 141209
		rlen = recvfrom( sock_fd, UDP_Rx_Temp_Data, MAX_COMM_QUE, 0, (struct sockaddr*)&recv_addr, &recv_addr_size );

		if(rlen > 0)
		{
			
			//D_Printf(DEBUG_SYSTEM, "recv %08X : %d\n", 
			//	recv_addr.sin_addr.s_addr, rlen );
			//D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
			//	UDP_Rx_Temp_Data[0], UDP_Rx_Temp_Data[1], UDP_Rx_Temp_Data[2], UDP_Rx_Temp_Data[3] );
			//D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
			//	UDP_Rx_Temp_Data[4], UDP_Rx_Temp_Data[5], UDP_Rx_Temp_Data[6], UDP_Rx_Temp_Data[7] );
			

			Add_UDP_Que( COMM_UDP_NODE, UDP_Rx_Temp_Data, rlen, &recv_addr );		
		}
		else break;		
	}
}
*/

/*
static INT8U REMS_Rx_Temp_Data[MAX_COMM_QUE];

void RecvUDPDataQue_REMS( void )
{
    struct sockaddr_in recv_addr;
    INT32U recv_addr_size;
    int sock_fd;
	int rlen;
		
	sock_fd = Get_UDP_fd(COMM_UDP_REMS);

	if(sock_fd < 0) return;

	while( 1 )
	{
		recv_addr_size = sizeof(struct sockaddr_in); // Address Size 141209
		rlen = recvfrom( sock_fd, REMS_Rx_Temp_Data, MAX_COMM_QUE, 0, (struct sockaddr*)&recv_addr, &recv_addr_size );

		if(rlen > 0)
		{
			
			//D_Printf(DEBUG_SYSTEM, "recv %08X : %d\n", 
			//	recv_addr.sin_addr.s_addr, rlen );
			//D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
			//	REMS_Rx_Temp_Data[0], REMS_Rx_Temp_Data[1], REMS_Rx_Temp_Data[2], REMS_Rx_Temp_Data[3] );
			//D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
			//	REMS_Rx_Temp_Data[4], REMS_Rx_Temp_Data[5], REMS_Rx_Temp_Data[6], REMS_Rx_Temp_Data[7] );
			

			Add_UDP_Que( COMM_UDP_REMS, REMS_Rx_Temp_Data, rlen, &recv_addr );		
		}
		else break;		
	}
}
*/
//--------------------------------------------------
static INT8U DTU_Rx_Temp_Data[MAX_COMM_QUE];

void RecvUDPDataQue_lohost( void )
{
    struct sockaddr_in recv_addr;
    INT32U recv_addr_size;
    int sock_fd;
	int rlen;
		
	sock_fd = Get_UDP_fd(COMM_UDP_DTU);

	if(sock_fd < 0) return;

	while( 1 )
	{
		recv_addr_size = sizeof(struct sockaddr_in); // Address Size 141209
		rlen = recvfrom( sock_fd, DTU_Rx_Temp_Data, MAX_COMM_QUE, 0, (struct sockaddr*)&recv_addr, &recv_addr_size );

		if(rlen > 0)
		{
			
			//D_Printf(DEBUG_SYSTEM, "recv %08X : %d\n", 
			//	recv_addr.sin_addr.s_addr, rlen );
			//D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
			//	REMS_Rx_Temp_Data[0], REMS_Rx_Temp_Data[1], REMS_Rx_Temp_Data[2], REMS_Rx_Temp_Data[3] );
			//D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", 
			//	REMS_Rx_Temp_Data[4], REMS_Rx_Temp_Data[5], REMS_Rx_Temp_Data[6], REMS_Rx_Temp_Data[7] );


			print_agent( DEBUG_SYSTEM, VT_BOLD_RED, "RecvUDPDataQue_lohost ");
			

			Add_UDP_Que( COMM_UDP_DTU, DTU_Rx_Temp_Data, rlen, &recv_addr );		
		}
		else break;		
	}
}

//--------------------------------------------------

/*
/sys/class/spi_master/spi0/spi0.2/iio:device0

in_voltage0_raw
in_voltage1_raw
in_voltage2_raw
in_voltage3_raw
in_voltage4_raw
in_voltage5_raw
in_voltage6_raw
in_voltage7_raw

*/ 


#define LISTEN_PORT    12345 
#define BUFSIZE            1024 



#define DSTADDR_SOCKOPT IP_RECVDSTADDR 
#define DSTADDR_DATASIZE (CMSG_SPACE(sizeof(struct in_addr))) 
#define dstaddr(x) (CMSG_DATA(x)) 

union control_data
{
    struct cmsghdr  cmsg; 
    u_char          data[DSTADDR_DATASIZE]; 
}; 

int pktinfo_get(struct msghdr *my_hdr, struct in_pktinfo *pktinfo)
{

	int res = -1;



	if (my_hdr->msg_controllen > 0) {

		struct cmsghdr *get_cmsg;

		for (get_cmsg = CMSG_FIRSTHDR(my_hdr); get_cmsg;

		     get_cmsg = CMSG_NXTHDR(my_hdr, get_cmsg)) {

			if (get_cmsg->cmsg_level == IPPROTO_IP &&

			    get_cmsg->cmsg_type == IP_PKTINFO) {

				struct in_pktinfo *get_pktinfo = (struct in_pktinfo *)CMSG_DATA(get_cmsg);

				memcpy(pktinfo, get_pktinfo, sizeof(*pktinfo));

				res = 0;

			} 
			else
			{
				D_Printf(DEBUG_SYSTEM, "Unknown ancillary data, len=%lu, level=%d, type=%d\n",
				       get_cmsg->cmsg_len, get_cmsg->cmsg_level, get_cmsg->cmsg_type);
			}

		}

	}

	return res;

}      


static INT8U UDP_LSH_Rx_Temp_Data[MAX_COMM_QUE];
void RecvUDPDataQue_LSH( void )
{
    struct sockaddr_in recv_addr;
    INT32U recv_addr_size;
    int sock_fd;
	int rlen;

	sock_fd = Get_UDP_fd(COMM_UDP_LS);

	if(sock_fd < 0) return;

	while( 1 )
	{
		recv_addr_size = sizeof(struct sockaddr_in); // Address Size 141209
		rlen = recvfrom( sock_fd, UDP_LSH_Rx_Temp_Data, MAX_COMM_QUE, 0, (struct sockaddr*)&recv_addr, &recv_addr_size );

		if(rlen > 0)
		{
			/*
			D_Printf(DEBUG_SYSTEM, "recv %08X : %d\n", recv_addr.sin_addr.s_addr, rlen );
			D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", UDP_LSH_Rx_Temp_Data[0], UDP_LSH_Rx_Temp_Data[1], UDP_LSH_Rx_Temp_Data[2], UDP_LSH_Rx_Temp_Data[3] );
			D_Printf(DEBUG_SYSTEM, "r %02X %02X %02X %02X\n", UDP_LSH_Rx_Temp_Data[4], UDP_LSH_Rx_Temp_Data[5], UDP_LSH_Rx_Temp_Data[6], UDP_LSH_Rx_Temp_Data[7] );
			*/

			Add_UDP_Que( COMM_UDP_LS, UDP_LSH_Rx_Temp_Data, rlen, &recv_addr );		
		}
		else break;		
	}
}


INT16U GetUDPDataFromUDP_Que( INT8U port, INT8U* data, struct sockaddr* p_recv_addr )
{
	INT16U rear, rlen;

	rlen = 0;

	pthread_mutex_lock(&UDP_Rx_Evt[port]);

	if( (udp_rx_que[port].front != udp_rx_que[port].rear) )// && (udp_rx_que[port].rear<MAX_UDP_RX_QUE) )
	{
		rear = udp_rx_que[port].rear;
		rlen = udp_rx_que[port].recv_len[rear];
		if( rlen )
		{
			memcpy( data, udp_rx_que[port].data[rear], rlen );
			memcpy( p_recv_addr, &udp_rx_que[port].recv_addr[rear], sizeof(struct sockaddr_in) );
		}

		udp_rx_que[port].rear ++;
		udp_rx_que[port].rear %= MAX_UDP_RX_QUE;
	}

	pthread_mutex_unlock(&UDP_Rx_Evt[port] );

	return rlen;
}





/* EOF */

