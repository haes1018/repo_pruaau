/********************************************************************************/
#ifndef __COMM_LIB_H
#define __COMM_LIB_H
/********************************************************************************/
#ifdef  COMM_LIB_GLOBALS
#define COMM_LIB_EXT
#else
#define COMM_LIB_EXT extern
#endif
/**************************************************************/

/////////////////////////////
//	COMM_LIB.C
/////////////////////////////
#define COMM_LIB_GLOBALS


typedef struct
{
	INT32S	comm_fd;
    INT32U	comm_type;
    INT32U	Mutex_Init_Flag;
//    struct sockaddr_in send_addr;
    char DevName[128];
    INT32U	recv_ip;
} COMM_INIT_TYTE;

typedef INT8U       PTR_GetFrame( COMM_QUE* pque, INT8U* pframe );

COMM_LIB_EXT INT8U Send_UP_Data( INT8U port, INT8U* send, INT16U len );
COMM_LIB_EXT int Get_Comm_fd_Status(INT8U port);
COMM_LIB_EXT void Comm_Que_Dbg(INT8U ComPort, INT8U type);

// ���� ��� ��Ž� ���
// port : Comm Port ��ȣ
// send : Tx Data
// len : Tx Data Len
// ack : ACK Data, NULL �̸� ������ ��ٸ��� ����
// delay : Ack ���� Time Out �ð� 10msec*delay
// Return Value : ACK ���Ž� 1, �ƴϸ� 0, Tx �������� Time Out�ÿ��� 0
COMM_LIB_EXT INT8U Send_DN_Data( INT8U port, INT8U* send, INT16U len, INT8U* ack, INT16U delay );
COMM_LIB_EXT INT8U Send_DN_Data_ASC( INT8U port, INT8U* send, INT16U len, INT8U* ack, INT16U delay );

// ���� ��������� Data Tx�� ���
// port : Comm Port ��ȣ
// send : Tx Data
// len : Tx Data Len
// Return Value : Tx �������� Time Out�ÿ� 0
COMM_LIB_EXT INT8U Send_UP_Data( INT8U port, INT8U* send, INT16U len );

COMM_LIB_EXT void Send_gREMS_Data( INT8U port, INT8U* dst_ip, INT16U port16, INT8U *buf, INT16U len );
// TREE_2019_0521
COMM_LIB_EXT void Send_UDP_Node_Data( INT8U port, INT8U* dst_ip, INT16U port_num, INT8U *buf, INT16U len );
// END : TREE_2019_0521

COMM_LIB_EXT void Send_UDP_Data_GUI( INT8U port, INT8U* dst_ip, INT8U *buf, INT16U len, INT16U port16 );




// ���� ��񿡼� Data �޾Ҵ��� �˻�
// port : Comm Port ��ȣ
// txf : Data Tx �� ���� ���� Address
// rxf : Data Rx �� ���� ���� Address
// delay : ������ preamble ������ ���� �Ϸ���� Time Out �ð� 10msec*delay
// Return Value : Tx �������� Time Out�ÿ� 0
COMM_LIB_EXT INT8U Check_UP_Data( INT8U port, INT8U* txf, INT8U* rxf, INT8U delay );

// Data ���Ž� Rx Que���� ������ ���� �ϱ� ���� �Լ� ����, �� Comm Port���� ���������� �߰��� �ٲ� ���
// port : Comm Port ��ȣ
// pfunc : Rx Que���� �����͸� �������ݿ� ������ ���� �ϱ� ���� �Լ�
COMM_LIB_EXT void Set_Frame_Func( INT8U port, PTR_GetFrame* pfunc );

// Comm Port �ʱ�ȭ�� ���� �Լ�
// port : Comm Port ��ȣ
// dev_addr : �ش� Comm Port �� Dev Address
// comm_type : �ش� Comm Port �� Dev Type ����  
// 				Define ����
//				COMM_TYPE_STM_UART : MCU ��� Comm Port ���
//				COMM_TYPE_2550_UART : ST2550 ����
//				COMM_TYPE_UDP : UDP ����
COMM_LIB_EXT void Init_Comm_Lib( INT8U port, char* dev_fname, int udp_socket, INT32U comm_type, INT32U baud_port, PTR_GetFrame* pfunc, INT8U* preamble, INT8U preamble_len );
COMM_LIB_EXT int udp_init( INT16U port );
COMM_LIB_EXT int udp_init_law( INT16U port );


// Comm Port Tx Fail �߻� �˻�
// Return Value : Comm Port ���� �������� 10ȸ �̻� Tx Fail �߻��� 1�� Return �ƴ� ��� 0
COMM_LIB_EXT INT32U CheckCommFail( void );
COMM_LIB_EXT void Close_UART_Comm( INT8U port );

// 16C255o ��� ���� Over ���� �߻��Ұ�� ó�� ���ؼ�
COMM_LIB_EXT void CheckUART_Error( INT32U port );

COMM_LIB_EXT COMM_QUE* GetQUE( INT8U port );
COMM_LIB_EXT void UDP_IP_Re_Setting( INT8U port, INT32U ip_32 );


COMM_LIB_EXT void SetPreamble( INT8U port, INT8U* preamble, INT8U preamble_len );

COMM_LIB_EXT void RecvCommDataQue( INT8U port );
COMM_LIB_EXT void SetSendIP_Address( INT8U comm_port, INT8U* ip, INT16U udp_port );

COMM_LIB_EXT INT8U Check_Node_Data( INT8U port, INT8U* rxf, INT16U delay, INT8U* recv_ip, INT16U* port_no);

COMM_LIB_EXT INT16U GetUDPDataFromUDP_Que( INT8U port, INT8U* data, struct sockaddr* p_recv_addr );
//COMM_LIB_EXT void RecvUDPDataQue( void );
//COMM_LIB_EXT void RecvUDPDataQue_REMS( void );
COMM_LIB_EXT void RecvUDPDataQue_lohost( void );

COMM_LIB_EXT void RecvUDPDataQue_All(INT8U port);

///// YCSO added 190429
COMM_LIB_EXT void Send_Tx_Only_Data(INT8U port, INT8U *buf, INT16U len );







COMM_LIB_EXT INT32U Get_IP32( INT8U comm_port );



COMM_LIB_EXT INT8U Check_LSH_Data( INT8U port, INT8U* rxf, INT16U delay, INT8U* recv_ip, INT16U* recv_port );
COMM_LIB_EXT void RecvUDPDataQue_LSH( void );
COMM_LIB_EXT void Send_LSH_Data( INT8U port, INT8U* dst_ip, INT16U dst_port, INT8U *buf, INT16U len );

COMM_LIB_EXT INT8U Send_Localhost_Data( INT8U port, INT8U* send, INT16U len, INT8U* ack, INT16U delay );

COMM_LIB_EXT void BT_Init(INT8U bt_req); // 2019.08.15(v08.14.99)

COMM_LIB_EXT int tcp_Client_init( INT16U port , INT8U* ip_addr);
COMM_LIB_EXT INT8U Check_Node_SIM_Data( INT8U port, INT8U* rxf, INT16U delay );
COMM_LIB_EXT void Send_TCP_Data_GUI(INT8U port, INT8U * dst_ip, INT8U * buf, INT16U len);

COMM_LIB_EXT void Set_BT_Name( INT8U BT_init_status ); // 2019.08.15(v08.14.99)

#ifdef __YCSO__BT__CMD__ //// YCSO added 191126
COMM_LIB_EXT void send_BT_mode_CMD(void);
#endif
#endif

/*EOF*/

