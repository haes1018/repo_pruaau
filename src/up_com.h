/********************************************************************************/
#ifndef __UP_COM_H
#define __UP_COM_H
/********************************************************************************/
#ifdef  UP_COM_GLOBALS
#define UP_COM_EXT
#else
#define UP_COM_EXT extern
#endif
/**************************************************************/



/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
// 외부 호출 함수
//
//UP_COM_EXT void SendDebug( INT8U type, INT8U* data, INT16U l );
//UP_COM_EXT INT8U txSMSData( PROTOCOL_FRAME* txf );

UP_COM_EXT void *UP_Comm_Task(void *data);
//UP_COM_EXT void DN_Comm_Task(void *data);
//UP_COM_EXT INT8U txDebugData( INT8U port, PROTOCOL_FRAME* txf );
/*----------------------------------------------------------*/
// 외부 참조 변수
//
UP_COM_EXT void* UDP_Term_Comm_Task(void *data);
UP_COM_EXT INT16U PassUDP_TERM_Data( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
UP_COM_EXT INT8U Send_UDP_TERM_Ack_Check( PROTOCOL_FRAME* txf, INT16U len, PROTOCOL_FRAME* rxf, int ack_time_out );

/*----------------------------------------------------------*/


/********************************************************************************/
#endif
/********************************************************************************/

