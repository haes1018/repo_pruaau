/////////////////////////////
// FRAME.C
/////////////////////////////
#define FRAME_GLOBALS
#include "includes.h"
#include <netinet/in.h>

/////////////////////////////






/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////


/////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////


static void rsStartCheck( COMM_QUE* pque )//0
{
	if( pque->data[pque->rear] == SYN )
	{
		if( ++pque->sync_len >= MAX_NMS_SYNC_SIZE )
		{
			pque->sync_len = 0;
			pque->comm_mode = 1;
			pque->len = 0;
			pque->data_recv = 0;
		    pque->crc_value = 0;
		}
	}
	else pque->sync_len = 0;
}
static void rsStartCheck_PAU( COMM_QUE* pque )//0
{
	if( pque->data[pque->rear] == SYN )
	{
		if( ++pque->sync_len >= MAX_NMS_SYNC_SIZE )
		{
			pque->sync_len = 0;
			pque->comm_mode = 1;
			pque->len = 0;
			pque->data_recv = 0;
		    pque->crc_value = 0;
		}
	}
	else pque->sync_len = 0;
}

static void rsHeaderCheck( COMM_QUE* pque, PROTOCOL_FRAME* pframe ) // 1
{
	INT8U *p = (INT8U *)pframe;
	INT8U sum;

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= MAX_HEADER_SIZE ) //3
	{
		pframe->h_len = Swap16( pframe->h_len );
		if( pframe->h_len > MAX_COM_BUFFER ) pframe->h_len = 0;

		//-------------------------------------
		#ifdef __OLD_AID__
		//-------------------------------------
		sum = pframe->seqNo16;
		sum += (INT8U)(pframe->h_len>>8);
		sum += (INT8U)(pframe->h_len); 
		//-------------------------------------
		#else
		//-------------------------------------
		pframe->seqNo16 = Swap16(pframe->seqNo16);
		
		sum = (INT8U)(pframe->seqNo16 >> 8) + (INT8U)pframe->seqNo16;
		sum += pframe->totPkt;
		sum += pframe->cur_Pkt;
		sum += (INT8U)(pframe->h_len>>8);
		sum += (INT8U)(pframe->h_len); 
		//-------------------------------------
		#endif		
		//-------------------------------------		

		if( sum != pframe->h_csum )
		{
			pque->comm_mode = 0;
		}
		else
		{
			if( pframe->h_len<9 )
				pque->comm_mode = 0;
			else pque->comm_mode = 2;
		}
		pque->len = 0;
	}
}

static void rsBodyHeaderCheck( COMM_QUE* pque, PROTOCOL_FRAME* pframe )
{
	INT8U *p = (INT8U *)pframe->SUB_ID;

	pque->crc_value = crcByteCalc( pque->crc_value, pque->data[pque->rear] );

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= MAX_BODY_HEADER_SIZE ) //3
	{
		pframe->len = Swap16( pframe->len );
		if( pframe->len>MAX_COM_BUFFER ) pframe->len = 0;
		pque->comm_mode = (pframe->len) ? 3:4;
		pque->len = 0;
	}
}

static void rsDataCheck( COMM_QUE* pque, PROTOCOL_FRAME* pframe )//3
{
	INT8U *p = (INT8U *)pframe->data;

	pque->crc_value = crcByteCalc( pque->crc_value, pque->data[pque->rear] );

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= pframe->len )
	{
		pque->len = 0;
		pque->comm_mode = 4;
	}
}




static INT8U rsCRCCheck( COMM_QUE* pque, PROTOCOL_FRAME* pframe )//4
{
	INT8U *p = (INT8U *)pque->recv_crc;
	INT8U ret = 0;

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= MAX_CRC_SIZE )
	{
		pframe->crc[0] = p[0];
		pframe->crc[1] = p[1];
		
		if( BYTE_to_INT16B(p) == pque->crc_value  )
		{
			pque->data_recv = 1;
		}
		else
		{
			D_Printf( DEBUG_SYSTEM, "CRC => %d\n", pque->port );
			pque->sync_len = 0;
			pque->comm_mode = 0;
			pque->len = 0;
			pque->data_recv = 0;
		    pque->crc_value = 0;
		    ret = 0xFF;

#ifdef _DOWN_LOAD_TEST2_
		    if( pque->port == COMM_IRCS )
		    {
				pque->data_recv = 1;
		    }
#endif
		}
		pque->comm_mode = 0;
	}
	
	return ret;
}

// AMP
static INT8U rsCRCCheck_PAU( COMM_QUE* pque )//4
{
	INT8U *p = (INT8U *)pque->recv_crc;
	INT8U ret = 0;

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= MAX_CRC_SIZE )
	{
//		if( *((INT16U*)p) == Swap16(pque->crc_value)  )
		if( BYTE_to_INT16B(p) == pque->crc_value  )
		{
			pque->data_recv = 1;
		}
		else
		{
			D_Printf( DEBUG_SYSTEM, "CRC => %d\n", pque->port );
			pque->sync_len = 0;
			pque->comm_mode = 0;
			pque->len = 0;
			pque->data_recv = 0;
		    pque->crc_value = 0;
		    ret = 0xFF;

#ifdef _DOWN_LOAD_TEST2_
		    if( pque->port == COMM_IRCS )
		    {
				pque->data_recv = 1;
		    }
#endif
		}
		pque->comm_mode = 0;
	}
	
	return ret;
}

////////////////////////////////////////////////////////////////



void Init_Que( COMM_QUE* pque )
{
	pque->front=0;
	pque->rear=0;
    pque->sync_len=0;
    pque->len=0;
    pque->comm_mode=0;
    pque->crc_value=0;
//    pque->sum_value=0;
    pque->data_recv=0;
}

void ADD_Que( COMM_QUE* pque, INT8U ch )
{
	pque->data[pque->front++] = ch;
	pque->front%=MAX_COMM_QUE;
}

//-------------------------------------------------------

static INT16U Tx_SeqNo16[COMM_NUM];

void Init_Tx_Frame_SeqNo(INT8U port)
{
	if(port < COMM_NUM)
		Tx_SeqNo16[port] = 0;
}

INT16U Get_Tx_Frame_SeqNo(INT8U port)
{
	INT16U seqNo16;
	
	if(port < COMM_NUM)
		seqNo16 = ++Tx_SeqNo16[port];
	else
		seqNo16 = 0;		

	return seqNo16;
}

INT16U Get_Tx_Frame_SeqNo_Cur(INT8U port)
{
	if(port < COMM_NUM)
		return Tx_SeqNo16[port];
	else
		return 0;
}

//-------------------------------------------------------


// packet 처리함수.
// pque : raw data 가 있고,
// pframe : 파싱된 데이터를 여기에 정렬한다.
INT8U GetFrame( COMM_QUE* pque, PROTOCOL_FRAME* pframe ) // 0:no data, 1:Data 받는중, 2:Data 받기 완료
{	
	INT8U crc_fail = 0;
	RecvCommDataQue( pque->port );

	// queue의 꼬리에서 하나씩 읽어서 파싱...
	while( pque->front != pque->rear )
	{
		switch( pque->comm_mode )
		{
			case 0 : rsStartCheck( pque );   break;
			case 1 : rsHeaderCheck( pque, pframe ); break;
			case 2 : rsBodyHeaderCheck( pque, pframe ); break;
			case 3 : rsDataCheck( pque, pframe ); break;
			case 4 : crc_fail = rsCRCCheck( pque, pframe ); break;

			default:
				pque->sync_len=0;
				pque->len=0;
				pque->comm_mode=0;
				break;
		}

//		Debug_Rx_Data( pque->port, pque->data[pque->rear] );

		pque->rear++;
		pque->rear%=MAX_COMM_QUE;

		if( pque->data_recv )
		{
			pque->data_recv = 0;

//			Debug_Rx_Data_End( pque->port);

			return 2;	// --> 패킷 하나 받기 완료
		}
	}
	if( pque->comm_mode ) return 1; // --> 패킷 받는 중.

	if(crc_fail == 0xFF) return 0xFF;
	return 0;	// --> 받는 중도 아니고, 완료도 아님..
}

// 1Body 짜리 Frame 을 만든다.
// data 부분은 이미 복사되었음.
// txf 포인터를 받아 여기에 인자로 넘겨받은 type, rcode 등 앞뒤 패킷 정보부분을 넣는다.

INT16U Make_Frame_COMPACKT(INT8U type, INT8U rcode, PROTOCOL_FRAME_COMPACT* pframe, INT16U len, INT16U seqNo16, INT8U totPkt, INT8U curPkt)
{
	INT8U* pcrc2;
	
	//------------------------------------
	#ifdef __OLD_AID__
	//------------------------------------
	(void)seqNo16; (void)totPkt; (void)curPkt;
	
	pframe->seqNo16 = 0x30;	
	pframe->h_len   = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size
	pframe->h_csum  = 0x30;
	pframe->h_csum += (INT8U)(pframe->h_len>>8);
	pframe->h_csum += (INT8U)(pframe->h_len);	
	//------------------------------------
	#else
	//------------------------------------
	pframe->seqNo16 = htons(seqNo16);
	pframe->totPkt  = totPkt;
	pframe->cur_Pkt = curPkt;	

	pframe->h_len  = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size
	pframe->h_csum = (INT8U)(pframe->seqNo16>>8) + (INT8U)pframe->seqNo16;
	pframe->h_csum += pframe->totPkt;
	pframe->h_csum += pframe->cur_Pkt;	
	pframe->h_csum += (INT8U)(pframe->h_len>>8);
	pframe->h_csum += (INT8U)(pframe->h_len);
	//------------------------------------
	#endif
	//------------------------------------	
	

	//------------------------------------	
	pframe->Type = type;
	pframe->Rcode = rcode;
	pframe->len = len;
	//------------------------------------

	pcrc2 = &pframe->data[len]; // crc 계산 후, 값을 저장할 CRC 위치의 포인터.
	pframe->data[len+2] = 0x03; // ETX 넣고..
	//------------------------------------
	pframe->h_len = Swap16(pframe->h_len);
	pframe->len = Swap16(pframe->len);

	INT16B_to_BYTE( pcrc2, crcNByteCalc( 0, &pframe->SUB_ID[0], len+MAX_BODY_HEADER_SIZE ) );

	return (len+MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE+MAX_HEADER_SIZE+MAX_TAIL_SIZE);
}
///// YCSO added END
INT16U Make_Frame(INT8U type, INT8U rcode, PROTOCOL_FRAME* pframe, INT16U len, INT16U seqNo16, INT8U totPkt, INT8U curPkt)
{
	INT8U* pcrc2;
	
	//------------------------------------
	#ifdef __OLD_AID__
	//------------------------------------
	(void)seqNo16; (void)totPkt; (void)curPkt;
	
	pframe->seqNo16 = 0x30;	
	pframe->h_len   = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size
	pframe->h_csum  = 0x30;
	pframe->h_csum += (INT8U)(pframe->h_len>>8);
	pframe->h_csum += (INT8U)(pframe->h_len);	
	//------------------------------------
	#else
	//------------------------------------
	pframe->seqNo16 = htons(seqNo16);
	pframe->totPkt  = totPkt;
	pframe->cur_Pkt = curPkt;	

	pframe->h_len  = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size
	pframe->h_csum = (INT8U)(pframe->seqNo16>>8) + (INT8U)pframe->seqNo16;
	pframe->h_csum += pframe->totPkt;
	pframe->h_csum += pframe->cur_Pkt;	
	pframe->h_csum += (INT8U)(pframe->h_len>>8);
	pframe->h_csum += (INT8U)(pframe->h_len);
	//------------------------------------
	#endif
	//------------------------------------	
	

	//------------------------------------	
	pframe->Type = type;
	pframe->Rcode = rcode;
	pframe->len = len;
	//------------------------------------

	pcrc2 = &pframe->data[len]; // crc 계산 후, 값을 저장할 CRC 위치의 포인터.
	pframe->data[len+2] = 0x03; // ETX 넣고..
	//------------------------------------
	pframe->h_len = Swap16(pframe->h_len);
	pframe->len = Swap16(pframe->len);

	INT16B_to_BYTE( pcrc2, crcNByteCalc( 0, &pframe->SUB_ID[0], len+MAX_BODY_HEADER_SIZE ) );

	return (len+MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE+MAX_HEADER_SIZE+MAX_TAIL_SIZE);
}


// 프레임을 조사하여 Body2있는지 확인하고, 길이 등을 갱신하고,  Body1, Body2의 CRC 계산하여
// 프레임에 저장.
// pFrame  : 전달할 패킷.
// return : STX ~ EXT 전체 길이.
// h_len, seqNo16, len : host order --> network order
INT16U Re_Make_Frame( PROTOCOL_FRAME* pframe )
{
	INT16U len;
	INT16U tot_len;
	INT8U* pcrc2;

	len = pframe->len;

	// 헤더 갱신 pframe->len 이 변경 되는 경우 대비
	//------------------------------------
	#ifdef __OLD_AID__
	//------------------------------------
	pframe->seqNo16 = 0x30;
	pframe->h_len   = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size
	pframe->h_csum  = 0x30 + ((INT8U)(pframe->h_len>>8)) + ((INT8U)pframe->h_len);
	//------------------------------------
	#else
	//------------------------------------	
	pframe->h_csum = (INT8U)(pframe->seqNo16>>8) + (INT8U)pframe->seqNo16;
	pframe->h_csum += pframe->totPkt;
	pframe->h_csum += pframe->cur_Pkt;	
	pframe->h_csum += (INT8U)(pframe->h_len>>8);
	pframe->h_csum += (INT8U)pframe->h_len;
// TREE_2019_0521
	pframe->seqNo16 = Swap16(pframe->seqNo16);
// END : TREE_2019_0521

	//------------------------------------
	#endif
	//------------------------------------

	tot_len = pframe->h_len + MAX_HEADER_SIZE + MAX_TAIL_SIZE;
	pframe->h_len = Swap16(pframe->h_len);
	pframe->len = Swap16(pframe->len);
	
	pcrc2 = &pframe->data[len];
	INT16B_to_BYTE( pcrc2, crcNByteCalc( 0, &pframe->SUB_ID[0], len+MAX_BODY_HEADER_SIZE ) );
	pframe->data[len+2] = 0x03;


	return tot_len;
	
}

/////////////////////////////////////////////////////////////////////////////




// PAU 해당 프로토콜에 맞게 프레임 만든다.
INT16U Make_Frame_PAU( INT8U type, PAU_PROTOCOL_FRAME* pframe, INT16U len, INT8U pau_idx )
{
//	INT16U* pcrc; 
	INT8U* pcrc; 

	msg_id[pau_idx]++;	
	msg_id[pau_idx] &= 0x7F; // 0~0x7F

	pframe->Type = type;
	pframe->len = len+3; // PAU 프로토콜은 msg_id,CRC 까지 길이 포함
	pframe->msg_id = msg_id[pau_idx];

	pcrc = &pframe->data[len];
	pframe->data[len+2] = 0xF5;

	pframe->len = Swap16(pframe->len);

//	(*pcrc) = Swap16(crcNByteCalc( 0, (INT8U*)pframe, len+MAX_PAU_HEADER_SIZE+MAX_PAU_MSGID_SIZE ));	

	INT16B_to_BYTE( pcrc, crcNByteCalc( 0, (INT8U*)pframe, len+MAX_PAU_HEADER_SIZE+MAX_PAU_MSGID_SIZE ) );


	return (len+MAX_PAU_HEADER_SIZE+MAX_CRC_SIZE+MAX_PAU_MSGID_SIZE+MAX_TAIL_SIZE);
}

INT16U Re_Make_Frame_PAU( PAU_PROTOCOL_FRAME* pframe, INT8U pau_idx )
{
	INT16U len;
//	INT16U* pcrc; 
	INT8U* pcrc; 

	msg_id[pau_idx]++;
	msg_id[pau_idx]&=0x7F;

	len = pframe->len-3;

	pcrc = &pframe->data[len];
	pframe->data[len+2] = 0xF5;

	pframe->len = Swap16(pframe->len);

//	(*pcrc) = Swap16(crcNByteCalc( 0, (INT8U*)pframe, len+MAX_PAU_HEADER_SIZE+MAX_PAU_MSGID_SIZE ));
	INT16B_to_BYTE( pcrc, crcNByteCalc( 0, (INT8U*)pframe, len+MAX_PAU_HEADER_SIZE+MAX_PAU_MSGID_SIZE ) );

	return (len+MAX_PAU_HEADER_SIZE+MAX_CRC_SIZE+MAX_PAU_MSGID_SIZE+MAX_TAIL_SIZE);
}




static void rsHeaderCheck_PAU( COMM_QUE* pque, PAU_PROTOCOL_FRAME* pframe )//1
{
	INT8U *p = (INT8U *)pframe;


	pque->crc_value = crcByteCalc( pque->crc_value, pque->data[pque->rear] );

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= MAX_PAU_HEADER_SIZE ) //3
	{
		pframe->len = Swap16( pframe->len )-3; // MSG, CRC  뺀 길이
		//pframe->len = Swap16( pframe->len );
		if( pframe->len>MAX_COM_BUFFER ) pframe->len = 0;
		//		pque->comm_mode = (pframe->len) ? 2:3;
		pque->comm_mode = 2;
		pque->len      = 0;
	}
}



static void rsDataCheck_PAU( COMM_QUE* pque, PAU_PROTOCOL_FRAME* pframe )//3
{
	INT8U *p = (INT8U *)&pframe->msg_id;

	pque->crc_value = crcByteCalc( pque->crc_value, pque->data[pque->rear] );

	p[pque->len++] = pque->data[pque->rear];

	if( pque->len >= (pframe->len+1) )
	{
		pque->len      = 0;
		pque->comm_mode = 3;
	}
}

INT8U GetFrame_PAU( COMM_QUE* pque, PAU_PROTOCOL_FRAME* pframe ) // 0:no data, 1:Data 받는중, 2:Data 받기 완료
{
	INT8U ret = 0;
	
	RecvCommDataQue( pque->port );

	while( pque->front != pque->rear )
	{
		switch( pque->comm_mode )
		{
			case 0 : rsStartCheck_PAU( pque );   break;
			case 1 : rsHeaderCheck_PAU( pque, pframe ); break;
			case 2 : rsDataCheck_PAU( pque, pframe );    break;
			case 3 : ret = rsCRCCheck_PAU( pque );     break;
			default:
				pque->sync_len=0;
				pque->len=0;
				pque->comm_mode=0;
				break;
		}
//		Debug_Rx_Data( pque->port, pque->data[pque->rear] );
		pque->rear++;
		pque->rear%=MAX_COMM_QUE;

		if( pque->data_recv )
		{
			pque->data_recv = 0;
			return 2;
		}
		else // 2016.10.28. by Haes
		{
			if(ret == 0xFF) return 3;
		}
	}
	if( pque->comm_mode ) return 1;

	return 0;
}





/* EOF */



