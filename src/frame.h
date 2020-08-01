/********************************************************************************/
#ifndef __FRAME_H
#define __FRAME_H
/**************************************************************/
#ifdef  FRAME_GLOBALS
#define FRAME_EXT
#else
#define FRAME_EXT extern
#endif
/**************************************************************/



/*----------------------------------------------------------*/
#define NO_DUMMY 0
#define HAVE_DUMMY 1

#define MAX_NMS_SYNC_SIZE		4

#ifdef __OLD_AID__
#define MAX_HEADER_SIZE			4 // old : TryNo(1) + Len(2) + ChkSum(1), old
#else
#define MAX_HEADER_SIZE			7 // @ 5G: SeqNo(2) + TotPktNum(1) + CurPktNum(1) + Len(2) + ChkSum(1)
#endif

#define MAX_BODY_HEADER_SIZE	7
#define MAX_PAU_HEADER_SIZE		5
#define MAX_CRC_SIZE			2
#define MAX_PAU_MSGID_SIZE		1
#define MAX_TAIL_SIZE			1
#define SYN						0x16

///// YCSO added 190419 COM_BUFFER_PAGE_SIZE
#define COM_BUFFER_PAGE_SIZE	(1024*3)
#define MAX_COM_BUFFER (2048*17) // 190418 YCSO size 4096*3 -> 4096*5 //// 190509 YCSO size 2048*17
#define MAX_COMM_QUE   (2048*17) // 190418 YCSO size 4096*3 -> 4096*5
///// YCSO added COMPACT SIZE
#define MAX_COM_BUFFER_COMPACT	(512) // 

//#pragma pack(1)

///// YCSO added START 190426
typedef struct __attribute__((aligned(1),packed))
{
#ifdef __OLD_AID__
	INT8U	seqNo16; // h_tryno;
#else // New AID @ 5G
	INT16U	seqNo16;
	INT8U	totPkt;
	INT8U	cur_Pkt;
#endif
    INT16U	h_len;
    INT8U	h_csum;
    //--------------------
    INT8U	SUB_ID[3];
    INT8U	Type;
    INT8U	Rcode;
    INT16U	len;
    INT8U	data[MAX_COM_BUFFER_COMPACT];
	INT8U   crc[2];
} PROTOCOL_FRAME_COMPACT;




typedef struct __attribute__((aligned(1),packed))
{
#ifdef __OLD_AID__
	INT8U	seqNo16; // h_tryno;
#else // New AID @ 5G
	INT16U	seqNo16;
	INT8U	totPkt;
	INT8U	cur_Pkt;
#endif
    INT16U	h_len;
    INT8U	h_csum;
    //--------------------
    INT8U	SUB_ID[3];
    INT8U	Type;
    INT8U	Rcode;
    INT16U	len;
    INT8U	data[MAX_COM_BUFFER];
	INT8U   crc[2];
} PROTOCOL_FRAME;

//#pragma pack()


typedef struct __attribute__((aligned(1),packed))
{
	INT8U	ScrAddr;
	INT8U	DesAddr;
	INT8U	Type;
	INT16U	len;
	INT8U	msg_id;
	INT8U	data[MAX_COM_BUFFER];
} PAU_PROTOCOL_FRAME;




typedef struct
{
    INT8U	port;

	INT16U	front;
    INT16U	rear;
    INT8U	sync_len;

    INT16U	len;
    INT8U   comm_mode;
    INT16U	crc_value;  // 계산된 CRC
    INT8U	recv_crc[2]; // 패킷으로 받은 CRC
    INT8U	data_recv;
    INT8U	disconnet_len;

    INT8U	asc_data_tmp[2]; // 패킷으로 받은 CRC
    INT8U	data[MAX_COMM_QUE];
    INT8U   udp_tmp_data[MAX_COMM_QUE];

    struct sockaddr_in recv_addr;
//    INT32U recv_addr_size;

} COMM_QUE;


// DMA 송신 버퍼 관련
typedef struct
{
	INT16U	len;
	INT16U	co;
	INT16U	frame_start;	// data[] 에 preamble 있는 경우, preamble 제외한 실제 데이터의 위치 인덱스.
	INT8U	*data_start;	// data[] 에 preamble 있는 경우, preamble 제외한 실제 데이터가 시작되는 곳의 주소.
	INT8U	data[MAX_COM_BUFFER]; // preamble + data +...
} COMM_TX_BUFF;


/*----------------------------------------------------------*/
// 외부 호출 함수
//

FRAME_EXT void Init_Que( COMM_QUE* pque );
FRAME_EXT void ADD_Que( COMM_QUE* pque, INT8U ch );
FRAME_EXT INT8U GetFrame( COMM_QUE* pque, PROTOCOL_FRAME* pframe ); // 0:no data, 1:Data 받는중, 2:Data 받기 완료

FRAME_EXT INT16U Make_Frame(INT8U type, INT8U rcode, PROTOCOL_FRAME* pframe, INT16U len, INT16U seqNo16, INT8U totPkt, INT8U curPkt);
FRAME_EXT INT16U Re_Make_Frame(  PROTOCOL_FRAME* pframe );

FRAME_EXT INT8U GetFrame_PAU( COMM_QUE* pque, PAU_PROTOCOL_FRAME* pframe ); // 0:no data, 1:Data 받는중, 2:Data 받기 완료
FRAME_EXT INT16U Make_Frame_PAU( INT8U type, PAU_PROTOCOL_FRAME* pframe, INT16U len, INT8U pau_idx );
FRAME_EXT INT16U Re_Make_Frame_PAU( PAU_PROTOCOL_FRAME* pframe, INT8U pau_idx );

FRAME_EXT void Init_Tx_Frame_SeqNo(INT8U port);
FRAME_EXT INT16U Get_Tx_Frame_SeqNo(INT8U port);
FRAME_EXT INT16U Get_Tx_Frame_SeqNo_Cur(INT8U port);


///// YCSO added 
FRAME_EXT INT16U Make_Frame_COMPACKT(INT8U type, INT8U rcode, PROTOCOL_FRAME_COMPACT* pframe, INT16U len, INT16U seqNo16, INT8U totPkt, INT8U curPkt);



/*----------------------------------------------------------*/
// 외부 참조 변수
//
/*----------------------------------------------------------*/

FRAME_EXT INT8U msg_id[20]; // 에러 안나게 임의로 많이 사용.

/********************************************************************************/
#endif
/********************************************************************************/







