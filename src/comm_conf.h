/********************************************************************************/
#ifndef __COMM_CONF_H
#define __COMM_CONF_H
/********************************************************************************/
#ifdef  COMM_CONF_GLOBALS
#define COMM_CONF_EXT
#else
#define COMM_CONF_EXT extern
#endif
/**************************************************************/


enum
{
	COMM_TYPE_AT91_UART = 0,
	COMM_TYPE_2550_UART,
	COMM_TYPE_UDP,
	COMM_TYPE_TCP,
	COMM_TYPE_NUM
};

enum
{
	COMM_TERM = 0,	
	COMM_UDP_NODE,	// 1, GUI-UDP-Communication	
	COMM_UDP_DTU,   // 2, Comm UDP with local-host
	COMM_UDP_LS,	// 3, Comm UDP with Layer Splitter
//----------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only	
//----------------------------------------------	
	COMM_UART_AMP_1 = 4,
	COMM_UART_AMP_2, // 5
	COMM_UART_AMP_3, // 6
	COMM_UART_AMP_4, // 7
	COMM_UART___PSU, // 8
//----------------------------------------------
#else // TYPE__AAU   // 5G-AAU-Only
//----------------------------------------------
	COMM_BT       = 4,         // 4, bluetooth		
//----------------------------------------------
#endif
#ifdef __DEL_UDP_REMS__
	COMM_REMS = 9,
#else
	COMM_UDP_REMS = 9,
#endif	
	COMM_SIM_TCP = 10,		//// YCSO added 
	
	COMM_NUM
};
//------------------------------------------------

#ifdef __DEL_UDP_REMS__
#define COMM_UDP_REMS (COMM_UDP_NODE)
#endif




//---------------------------------------------------------------
#ifdef __MAKER_ANTS__ 		              // ANTS
//---------------------------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only	
#define UART_TERM_Dev "/dev/ttyPS1" // COMM_TERM
//
#define UART_AMP1_Dev "/dev/ttyUL4" // COMM_UART_AMP_1
#define UART_AMP2_Dev "/dev/ttyUL3" // COMM_UART_AMP_2
#define UART_AMP3_Dev "/dev/ttyUL2" // COMM_UART_AMP_3
#define UART_AMP4_Dev "/dev/ttyUL0" // COMM_UART_AMP_4
//
#define UART_PSU__Dev "/dev/ttyUL5" // COMM_UART___PSU
#else // ---------------------------- AAU
#define UART_TERM_Dev "/dev/ttyO4"  // COMM_TERM
#define COMM_Blue_Dev "/dev/ttyO1"  // COMM_BT
#endif
//---------------------------------------------------------------
#else 			                           // HRF
//---------------------------------------------------------------
#ifdef __TYPE__PRU__ // 5G-PRU-Only	
#define UART_TERM_Dev "/dev/ttyPS1" // COMM_TERM
//
#define UART_AMP1_Dev "/dev/ttyUL1" // COMM_UART_AMP_1
#define UART_AMP2_Dev "/dev/ttyUL2" // COMM_UART_AMP_2
#define UART_AMP3_Dev "/dev/ttyUL3" // COMM_UART_AMP_3
#define UART_AMP4_Dev "/dev/ttyUL4" // COMM_UART_AMP_4
//
#define UART_PSU__Dev "/dev/ttyUL5" // COMM_UART___PSU
#else // ---------------------------- AAU
#define UART_TERM_Dev "/dev/ttyO4"  // COMM_TERM
#define COMM_Blue_Dev "/dev/ttyO1"  // COMM_BT
#endif
//---------------------------------------------------------------
#endif
//---------------------------------------------------------------




//------------------------------------------------
#define NODE_UDP_PORT    20000
#define NODE_LSH_PORT    0x2000 // Layer-Splitter 
#define NODE_REMS_PORT   20000

#define NODE_LoHost_PORT 10000 // Self/10000 <-- DTU
#define DTU__LoHost_PORT 21010 // Self --> DTU/21010





COMM_CONF_EXT INT16U udp_port[COMM_NUM];


COMM_CONF_EXT void Init_UDP_fd(void);
COMM_CONF_EXT int Get_UDP_fd(int i);
COMM_CONF_EXT void Close_UDP_fd(void);


COMM_CONF_EXT void PreTxData( INT8U port );
COMM_CONF_EXT void EndTxData( INT8U port );
COMM_CONF_EXT void Recv_DN_Data( INT8U port, INT8U *ack );
// TREE_2019_0521
COMM_CONF_EXT INT16U Recv_UP_Data( INT8U port, INT8U *txf, INT8U* rxf, INT8U* udp_ip, INT16U udp_port );
// END : TREE_2019_0521
COMM_CONF_EXT void Init_Comm( INT8U port );
COMM_CONF_EXT int Get_UDP_Sock(INT8U port);

COMM_CONF_EXT void Close_SIM_TCP_fd(void);
COMM_CONF_EXT void Init_SIM_TCP_fd( INT16U port, INT8U* ip );



/********************************************************************************/
#endif
/********************************************************************************/



/* EOF */

