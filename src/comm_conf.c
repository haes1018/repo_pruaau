/////////////////////////////
//	COMM_CONF.C
/////////////////////////////
#define COMM_CONF_GLOBALS
#include "includes.h"
/////////////////////////////




/////////////////////////////
// Local Variables
/////////////////////////////
static INT8U DUMMY_SYNC_DATA[4] = { 0x16, 0x16, 0x16, 0x16 };
static INT8U DUMMY_LSH_SYNC_DATA[3] = { 0x10, 0x00, 0x00 };


/////////////////////////////
// Local Functions
/////////////////////////////
static int udp_sock_fd[COMM_NUM];
static INT8U SIM_TCP_ip[4] = {0};





/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

void Init_UDP_fd(void)
{
	int i;
	
	for(i = 0; i < COMM_NUM; i++) 
	{
		udp_sock_fd[i] = -1;
		udp_port[i] = 0;
	}

	udp_port[COMM_UDP_NODE] = NODE_UDP_PORT;	
	udp_port[COMM_UDP_DTU]  = NODE_LoHost_PORT;
	udp_port[COMM_UDP_LS]   = NODE_LSH_PORT;
	udp_port[COMM_UDP_REMS] = NODE_REMS_PORT;
}

int Get_UDP_fd(int i)
{
	if(i < COMM_NUM)
		return udp_sock_fd[i];
	else return -1;	
}

void Close_UDP_fd(void)
{
	if(udp_sock_fd[COMM_UDP_NODE] > 0) close(udp_sock_fd[COMM_UDP_NODE]);
	if(udp_sock_fd[COMM_UDP_DTU] > 0)  close(udp_sock_fd[COMM_UDP_DTU]);
	if(udp_sock_fd[COMM_UDP_LS] > 0)   close(udp_sock_fd[COMM_UDP_LS]);
	if(udp_sock_fd[COMM_UDP_REMS] > 0) close(udp_sock_fd[COMM_UDP_REMS]);
}




void PreTxData( INT8U port )
{
	switch( port )
	{
		case COMM_TERM:
			break;			
		case COMM_UDP_NODE:
			break;
			
	#ifdef __TYPE__AAU__ // 5G-AAU-Only	
		case COMM_BT:
			break;
	#endif
	}
	OSTimeDly( OS_DELAY_1msec );
}

void EndTxData( INT8U port )
{
	switch( port )
	{
		case COMM_TERM:
			break;
			
	#ifdef __TYPE__AAU__ // 5G-AAU-Only	
		case COMM_BT:
			break;
	#endif
		
		default:
			return;
	}
}


// 하위 Unit에서 통신 데이터 받은 후.
void Recv_DN_Data( INT8U port, INT8U *ack )
{
	(void)ack;
	switch( port )
	{
		case COMM_UDP_NODE:
			break;

	#ifdef __TYPE__AAU__ // 5G-AAU-Only 
		case COMM_BT:
			break;
	#endif
	}
}

// TREE_2019_0521
// Terminal로부터의 패킷 처리.
INT16U Recv_UP_Data( INT8U port, INT8U *txf, INT8U* rxf, INT8U* udp_ip, INT16U udp_port )
{
	/*
	-------------------------------------------------
	2019.09.26
	if from gREMS-IP, and Init_status is 0, dismiss packet from gREMS
	-------------------------------------------------
	*/	
	if( port == COMM_UDP_NODE )
	{	
		if(memcmp(AufoConfigFlag.Recv_Ip2, udp_ip, 4)==0)
		{		
			if(AufoConfigFlag.Init_Status == 0)
			{				
				//print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UP_Data_Analysis] AufoConfigFlag %d.%d.%d.%d", 
				//	AufoConfigFlag.Recv_Ip2[0],	AufoConfigFlag.Recv_Ip2[1],	AufoConfigFlag.Recv_Ip2[2],	AufoConfigFlag.Recv_Ip2[3]);	
				//print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UP_Data_Analysis] r_ip %d.%d.%d.%d", 
				//	udp_ip[0],udp_ip[1],udp_ip[2],udp_ip[3]);	

				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "<Not Init-Noti-ACK> gREMS Packet Dismissed ----- !!");
				return 0;
			}
		}
	}


	
	switch( port )
	{
	#ifdef __TYPE__AAU__ 
		case COMM_BT:
	#endif
		case COMM_TERM:
		case COMM_UDP_NODE:
		#ifdef __DEL_UDP_REMS__
		#else
		case COMM_UDP_REMS:
		#endif
		case COMM_SIM_TCP:	//// YCSO added 190507	
			return UP_Data_Analysis( (PROTOCOL_FRAME*)rxf, (PROTOCOL_FRAME*)txf, port, udp_ip, udp_port );
	}
	return 0;
}
// END : TREE_2019_0521


int Get_UDP_Sock(INT8U port)
{
	return udp_sock_fd[port];
}
///// YCSO ADDED TCP 
void Init_SIM_TCP_fd( INT16U port, INT8U* ip )
{
	int i = 0;
	udp_sock_fd[COMM_SIM_TCP] = -1;

	udp_port[COMM_SIM_TCP] = port;

	for( i = 0; i < 4; i++)
		SIM_TCP_ip[i] = ip[i];
}

void Close_SIM_TCP_fd(void)
{
	if(udp_sock_fd[COMM_SIM_TCP] > 0) close(udp_sock_fd[COMM_SIM_TCP]);
}

int Get_SIM_TCP_sock(INT8U port)
{
	return udp_sock_fd[port];
}

void Init_Comm( INT8U port )
{
	switch( port )
	{
		case COMM_TERM: // with GUI, WRCS
			Init_Comm_Lib( port, UART_TERM_Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame, DUMMY_SYNC_DATA, 4 );
			break;	
			
		case COMM_UDP_NODE:
		#ifdef __DEL_UDP_REMS__
		#else
		case COMM_UDP_REMS:
		#endif
			if( udp_sock_fd[port] < 0 )
			{
				if(port == COMM_UDP_NODE)
					udp_sock_fd[port] = udp_init(udp_port[port]);
				else
					udp_sock_fd[COMM_UDP_REMS] = udp_sock_fd[COMM_UDP_NODE];
			}
			Init_Comm_Lib( port, NULL, udp_sock_fd[port], COMM_TYPE_UDP, 0, (PTR_GetFrame*)GetFrame, DUMMY_SYNC_DATA, 4 );
			break;		
			
		case COMM_UDP_DTU:
			if( udp_sock_fd[port] < 0 ) udp_sock_fd[port] = udp_init(udp_port[port]);
			Init_Comm_Lib( port, NULL, udp_sock_fd[port], COMM_TYPE_UDP, m_Env.lohost_ip_32, (PTR_GetFrame*)GetFrame, DUMMY_SYNC_DATA, 4 );
			break;

		case COMM_UDP_LS:
			if( udp_sock_fd[port]<0 ) udp_sock_fd[port] = udp_init(udp_port[port]);
			Init_Comm_Lib( port, NULL, udp_sock_fd[port], COMM_TYPE_UDP, 0, (PTR_GetFrame*)GetFrame_LSH, DUMMY_LSH_SYNC_DATA, 3 );
			break;

//-------------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only 
//-------------------------------------------------
		case COMM_UART_AMP_1:
			Init_Comm_Lib( port, UART_AMP1_Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame_PAU, DUMMY_SYNC_DATA, 4 );
			break;	
		case COMM_UART_AMP_2:
			Init_Comm_Lib( port, UART_AMP2_Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame_PAU, DUMMY_SYNC_DATA, 4 );
			break;		
		case COMM_UART_AMP_3:
			Init_Comm_Lib( port, UART_AMP3_Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame_PAU, DUMMY_SYNC_DATA, 4 );
			break;		
		case COMM_UART_AMP_4:
			Init_Comm_Lib( port, UART_AMP4_Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame_PAU, DUMMY_SYNC_DATA, 4 );
			break;
		case COMM_UART___PSU:
			Init_Comm_Lib( port, UART_PSU__Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame_PAU, DUMMY_SYNC_DATA, 4 );
			break;	
		///// YCSO added SIM
		case COMM_SIM_TCP:
			///// IP 값 받아 오는거 아직 안됨.
			if( udp_sock_fd[port] < 0 ) udp_sock_fd[port] = tcp_Client_init(udp_port[port], SIM_TCP_ip);
			Init_Comm_Lib( port, NULL, udp_sock_fd[port], COMM_TYPE_TCP, 0, (PTR_GetFrame*)GetFrame, DUMMY_SYNC_DATA, 4 );
			break;
		///// YCSO added SIM END
//-------------------------------------------------
#else  // __TYPE__AAU__ : 5G-PRU-Only 
//-------------------------------------------------
		case COMM_BT:
			Init_Comm_Lib( port, COMM_Blue_Dev, -1, COMM_TYPE_AT91_UART, 115200, (PTR_GetFrame*)GetFrame, DUMMY_SYNC_DATA, 4 );			
			break;
//-------------------------------------------------
#endif  // __TYPE__PRU__ : 5G-PRU-Only 
//-------------------------------------------------
	}
}






/* EOF */

