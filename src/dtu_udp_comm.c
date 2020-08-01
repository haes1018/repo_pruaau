/////////////////////////////
//	DTU_UDP_COMM.C
/////////////////////////////
#define DIG_UDP_COMM_GLOBALS
#include "includes.h"
/////////////////////////////

















void Update_DTU_REMS(void);








//-----------------------------------------------











//---------------------------------------------------------------------------------------------------

//===================================================================================================


//----------------------------------------------







//===================================================================================

//===================================================================================













//-----------------------------------------------------------------------------------------




//-------------------------------------------------------------------------

/*
	--------------------------------------------------------------------------------------
	2. DPD Status check(FeedBack)
	
	<A>. ���� ���¿��� �˶����� üũ

		{abnormal ����} �ǹ� : (FWD-Dig-Pwr(DPD) - Dig-FB-Pwr) > 10dB 
		{Recovery ����} �ǹ� : �Ʒ� (1)~(3) ����.
		
		DPD-St(33) ���¿��� {abnormal ����}�� 10�� ���� �Ǹ� �Ʒ� (1)~(3)����
		(1) if (Fwd-Out-Att + FB-Att != 25dB) �̸�,   
		    FB-Att�� (25 - Fwd-Out-Att) ���� ����. ��, ATT-Pair-OFF �� Att ����
				
		(2) if (ATT-Pair == OFF) then, ON ����	
		(3) if ( FB-Att-Offset > 3) then, 3 ����

		(4) DPD Alarm(Feedback) �߻� ����.
			* (1)~(3) ���� ���� abnormal ���� 1�� �̻� ���� �� (90�� �̳��� )
			
		(5) Alarm ���¿��� 1�ð� ��, {Recovery ����} ���� (1�ð� ���� �ݺ� ���� ����).
		
	
	<B> ���� ���� : �Ʒ� (1) Ȥ�� (2)
		(1) DPD-St(33) ���� ���� ��. 
		(2) (FWD-Dig-Pwr(DPD) - Dig-FB-Pwr) < 7dB ] 30sec �̻� ����

	<C> AMP link fail : dpd alarm check ���� ����. (���� �߻��� �˶��� ����)
	<D> �Ʒ� ������ ���, dpd alarm check ���� ����. ( �˶��� �������� mask )
		(1) PAU �̻��.
		(2) FWD AMP OFF
		(3) AMP Device Fail alarm
		(4) AMP DC Fail alarm

	--------------------------------------------------------------------------------------
*/








//===================================================================================

//===================================================================================

//===================================================================================



//---------------------------------------------------------------------------------








//---------------------------------------------------------------


//-----------------------------------------------------------------------------


////--------------------------------------------------------------------------------------
/*
static INT16U dig_down_tot_pkt = 0;
static INT16U dig_down_cur_pkt = 0;

INT16U DTU_Dnload_PassData( INT8U type, PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{	
	INT16U l;
	INT16U len = 0; // init
	INT8U Mod_SUB_ID[3];	
	INT8U dtu_type, gui_type;
	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16;

	pthread_mutex_lock(&DTU_COMM_Evt);
	

	// �ٿ�ε� ���(Ÿ��)�� PAU�� ����ϴ� Ÿ������ ����.
	switch( type )
	{	
		case 0xE8: dtu_type = 0x08; break;
		case 0xE0: dtu_type = 0x0B; break;
		case 0xE1: dtu_type = 0x0C; break;
		case 0xE2: dtu_type = 0x0D; break;
		case 0xE3: dtu_type = 0x0E; break;
	}

	Mod_SUB_ID[0] = rxf->data[0];
	Mod_SUB_ID[1] = rxf->data[1];
	Mod_SUB_ID[2] = rxf->data[2];	

	
	dn_txf.SUB_ID[0] = rxf->SUB_ID[0];
	dn_txf.SUB_ID[1] = rxf->SUB_ID[1];
	dn_txf.SUB_ID[2] = rxf->SUB_ID[2];
	

	//-----------------------------
	// cpy len, data
	// Module SUBID 3byte �� ��ŭ ����.	
	dn_txf.len = rxf->len - 3; 
	l = dn_txf.len;
	memcpy( dn_txf.data, &rxf->data[3], l);
	//-----------------------------	
	switch( type )
	{
		case 0xE0: m_Env.DnMode.Dig = _DN_RST_AUTO; break;
		case 0xE8: m_Env.DnMode.Dig = dn_txf.data[3]; break;
	}

	//--------------------------------------------------------
	switch( type )
	{
		case 0xE0:
			dig_down_tot_pkt = BYTE_to_INT16B( &dn_txf.data[1] );
			m_Env.Dig_Down_AMP_Off_Req = 0;
			break;

		case 0xE2:
			if(m_Env.DnMode.Dig == _DN_RST_AUTO)
			{
				dig_down_cur_pkt = BYTE_to_INT16B( &dn_txf.data[0] );				
				if( dig_down_tot_pkt <= (dig_down_cur_pkt + 50) )
				{
					m_Env.Dig_Down_AMP_Off_Req = 1;
				}
			}
			break;
	}
	//--------------------------------------------------------
	seqNo16 = Get_Tx_Frame_SeqNo(COMM_UDP_DTU);
	len = Make_Frame( dtu_type, _RCODE_, (PROTOCOL_FRAME*)&dn_txf, dn_txf.len, seqNo16, totPkt, curPkt );
	
	if( Send_DN_Data( COMM_UDP_DTU, (INT8U*)&dn_txf, len, (INT8U*)&dn_rxf, 200 ) )
	{
		if(dn_rxf.Type == dtu_type)
		{
			switch( dn_rxf.Type )
			{	
				case 0x08: gui_type = 0xE8; break;
				case 0x0B: gui_type = 0xE0; break;
				case 0x0C: gui_type = 0xE1; break;
				case 0x0D: gui_type = 0xE2; break;
				case 0x0E: gui_type = 0xE3; Start_Alm_Rpt_Pause_Timer(); break;
			}	

			if( gui_type == 0xE3 )
			{
				if(m_Env.DnMode.Dig == _DN_RST_WAIT_)
				{
					m_Env.is_down_load = 0;
				}
			}			
			
			l = dn_rxf.len;
			// SUBID 3byte ���ĺ��� ����.	
			memcpy ( &txf->data[3], dn_rxf.data, l );
			
			// Module SUBID 3byte �߰�.
			txf->data[0] = Mod_SUB_ID[0];
			txf->data[1] = Mod_SUB_ID[1];
			txf->data[2] = Mod_SUB_ID[2];
			l += 3;

			seqNo16 = Get_Tx_Frame_SeqNo(COMM_UDP_DTU);
			len = Make_Frame( gui_type, rxf->Rcode, txf, l, seqNo16, totPkt, curPkt );
			
			pthread_mutex_unlock(&DTU_COMM_Evt);
			return len;
		}
	}
	pthread_mutex_unlock(&DTU_COMM_Evt);
    return 0;
}
*/
//--------------------------------------------------------------------------------------

//--------------------------------------------------
enum
{
	D_RST_None = 0,
	D_RST_Send_AID,
	D_RST_Success,
	D_RST_Fail,
	D_RST_Max
};
#define DTU_MAX_RETRY (3)
//--------------------------------------------------




	
	




//=====================================================================

// Multi down

//=====================================================================






void Init_Dtu_MutiDn(void)
{
	DTU_Auto_DN* pAutoDn = &m_DTU_AutoDn;

	pAutoDn->req = 0;
	pAutoDn->fd = -1; // init
	pAutoDn->crc = 0; // init
	pAutoDn->crc = 0; // init
	pAutoDn->tot_frame = 0; // init
	pAutoDn->tot_size = 0; // init
	pAutoDn->result = Multi_Dn_None;
	pAutoDn->fw_apply_req = 0;
	
	memcpy( pAutoDn->str, "Dtu SS", 10);
}

void Init_Fpga_MutiDn(void)
{
	DTU_Auto_DN* pAutoDn = &m_FPGA_AutoDn; 
	
	pAutoDn->req = 0;
	pAutoDn->fd = -1; // init
	pAutoDn->crc = 0; // init
	pAutoDn->crc = 0; // init
	pAutoDn->tot_frame = 0; // init
	pAutoDn->tot_size = 0; // init
	pAutoDn->result = Multi_Dn_None;
	pAutoDn->fw_apply_req = 0;

	memcpy( pAutoDn->str, "Fpga SS", 10);
}


// Dtu, Fpga
//
/*
int Get_Multi_Dn_Tot_Frame_Crc_Dig( DTU_Auto_DN* pAutoDn, INT16U* pTotFrame, MOD_TYPE* pMod )
{
	char file[128];
	int size = 0;
	int frame_co;	
	struct stat st;	

	if(pMod->type == Mod_DTU) 
	{
	     Get_DTU_Dn_File_Name( m_St.Maker[0], file, m_ModVer.dtu );
	     D_Printf( DEBUG_SYSTEM, "[Multi] Dtu file %s \n", file);
	}
	else if(pMod->type == Mod_FPGA) 
	{
		Get_FPGA_Dn_File_Name( m_St.Maker[0], file, m_ModVer.fpga );
		D_Printf( DEBUG_SYSTEM, "[Multi] Fpga file %s \n", file);
	}
	else return -1;	
					
						
	pAutoDn->fd = open(file, O_RDONLY);
			
	if( pAutoDn->fd < 0 )
	{
		syslog( LOG_WARNING, "[iRO] no %s File(1).\n", pAutoDn->str);
		size = 0;
	}
	else
	{
		if(stat(file, &st) == 0)
		{
			size = st.st_size;		
		}			
		if(size <= 0)
		{
			syslog( LOG_WARNING, "[iRO] no %s File(2).\n",pAutoDn->str);
			size = 0;
			close( pAutoDn->fd) ;
			pAutoDn->fd = -1; // init
		}
	}
	
	frame_co = size / D_FRAME_DATA_MAX;
	if((size % D_FRAME_DATA_MAX) > 0) frame_co += 1;
	
	
	*pTotFrame = frame_co;	

	D_Printf( DEBUG_SYSTEM, "[Multi] [%s] Tot Frame %d \r\n", pAutoDn->str, frame_co);

	return size;
}
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

int Dig_Get_Multi_Dn_Data( INT8U* data, INT16U cur_frame, DTU_Auto_DN* pAutoDn )
{
	int idx;
	INT16U len;
	int re;
	static INT32S prev_frame = -1;	
	INT16U tot_frame;
	int tot_size;

	tot_frame = pAutoDn->tot_frame;
	tot_size = pAutoDn->tot_size;

	idx = cur_frame*D_FRAME_DATA_MAX;
	
	if( cur_frame < (tot_frame-1) )
	{		
		len = D_FRAME_DATA_MAX;				
	}
	else
	{
		len = tot_size - idx;
	}

	if( cur_frame == 0 ) // if start
	{
		pAutoDn->crc = 0;
		prev_frame = -1;
	}


	if((prev_frame+1) == cur_frame)
	{	
		re = read( pAutoDn->fd, data, len );
		
		if( re != len )
		{	
			syslog( LOG_WARNING, "[iRO] rd %s fail(1) %d != %d, total size %d, cur frame %d.\n", pAutoDn->str, len, re, tot_size, cur_frame);
			D_Printf( DEBUG_SYSTEM, "[Multi] rd %s fail(1).\n", pAutoDn->str);

			lseek( pAutoDn->fd, -1*re, SEEK_CUR );

			OSTimeDly( OS_DELAY_100msec );

			syslog( LOG_WARNING, "[iRO] back and retry.\r\n");

			re = read( pAutoDn->fd, data, len );	

			if( re < len )
			{
				syslog( LOG_WARNING, "[iRO] rd %s fail(2) %d != %d, rd etc \n", pAutoDn->str, len, re);
				len -= re;
				re = read( pAutoDn->fd, &data[re], len );

				if( re != len )
				{
					syslog( LOG_WARNING, "[iRO] rd-retry fail.\r\n");
					
					return -1;
				}
			}			
		}
		
		if( re == len )
		{	
			pAutoDn->crc = crcLByteCalc( pAutoDn->crc, data, len );			
		}
	}
	else 
	{
		D_Printf( DEBUG_SYSTEM, "[Multi] Down retry frame %d\r\n", cur_frame);
	}
	
	prev_frame = cur_frame;

	pAutoDn->percent = cur_frame *100 / tot_frame;
	
	return len;
}
//-----------------------------------------------------------------------------




//---------------------------------------------------------------------





#define DIG_DN_NOTI_FW (0x08)
#define DIG_DN_CHK     (0x0C)
#define DIG_DN_DATA    (0x0D)
#define DIG_DN_END     (0x0E)


void Rx_Dig_AutoDown( INT8U rx_type, INT8U* data, DTU_Auto_DN* pAutoDn ) 
{
	INT16U uVal16;
	INT8U nack = 0; // default
	
	switch( rx_type )
	{	
		case DIG_DN_NOTI_FW: // 1,2�� �뺸, FW ����.
		case DIG_DN_CHK:			
		case DIG_DN_DATA:	
		case DIG_DN_END:
			if(data[0] != 0) 
			{
				nack = 1;
				pAutoDn->retry++;
				D_Printf( DEBUG_SYSTEM, "[Multi] [%s] rx nak [0x%02X] \r\n", pAutoDn->str, rx_type);
				return;
			}
			break;
	}	
	
	
	switch( rx_type )
	{	
		case DIG_DN_NOTI_FW: // 1,2�� �뺸, FW ����.			
			
			// total frame
			uVal16 = data[2]; uVal16 <<= 8; uVal16 |= data[3];			
			if(uVal16 != pAutoDn->tot_frame) 
			{
				D_Printf( DEBUG_SYSTEM, "[Multi] [%s] nak tot frame %d != %d \r\n", pAutoDn->str, uVal16, pAutoDn->tot_frame);
				nack = 1;
			}
						
			if(nack == 0)
			{
				if(pAutoDn->st == Auto_Dn_Noti_1) 
					 pAutoDn->st = Auto_Dn_Chk;
				else pAutoDn->st = Auto_Dn_Data;
			}
			break;

		case DIG_DN_CHK: // �뺸 Ȯ��.
			if(nack == 0) pAutoDn->st = Auto_Dn_Noti_2;
			break;
			
		case DIG_DN_DATA: // �ٿ�ε�
			// cur frame
			uVal16 = data[1]; uVal16 <<= 8; uVal16 |= data[2];
			if(uVal16 != pAutoDn->cur_frame) 
			{
				D_Printf( DEBUG_SYSTEM, "[Multi] [%s] nak cur frame %d != %d \r\n", pAutoDn->str, uVal16, pAutoDn->cur_frame);
				nack = 1;
			}
			
			if(nack == 0)
			{				
				if(uVal16 >= (pAutoDn->tot_frame-1)) // if last frame
				{
					pAutoDn->st = Auto_Dn_End;
					D_Printf( DEBUG_SYSTEM, "[Multi] [%s] last rx %d \r\n", pAutoDn->str, uVal16);
				}
				else pAutoDn->cur_frame++;
			}
			break;
	
		case DIG_DN_END: // Ȯ��
			// tot frame
			uVal16 = data[1]; uVal16 <<= 8; uVal16 |= data[2];
			if(uVal16 != pAutoDn->tot_frame) 
			{
				nack = 1;
				D_Printf( DEBUG_SYSTEM, "[Multi] [%s] nak tot frame %d != %d \r\n", pAutoDn->str, uVal16, pAutoDn->tot_frame);
			}
			
			if(nack == 0) 
			{
				pAutoDn->st = Auto_Dn_Success;
				D_Printf( DEBUG_SYSTEM, "[Multi] [%s] down success \r\n", pAutoDn->str);
			}
			break;
	}	

	if(nack) pAutoDn->retry++;
	else	 pAutoDn->retry = 0;
}


/*
INT8U AutoDown_Dig_Send_Data( DTU_Auto_DN* pAutoDn, INT8U type, INT8U* data, INT16U l )
{	
	INT16U len = 0; // init
	INT8U comm_port;
	INT32U dtu_ip_32, rx_ip_32;
	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16;
	
	pthread_mutex_lock(&DTU_COMM_Evt);


	Get__gREMS_My_SUBID(dn_txf.SUB_ID);
	//-----------------------------
	dn_txf.len = l;
	
	memcpy( dn_txf.data, data, l);

	seqNo16 = Get_Tx_Frame_SeqNo(comm_port);
	len = Make_Frame( type, _RCODE_, (PROTOCOL_FRAME*)&dn_txf, dn_txf.len, seqNo16, totPkt, curPkt );

	//-----------------------------
	comm_port = COMM_UDP_DTU;
	dtu_ip_32 = m_Env.lohost_ip_32;	
	
	if( Send_DN_Data( comm_port, (INT8U*)&dn_txf, len, (INT8U*)&dn_rxf, 300 ) )
	{		
		rx_ip_32 = Get_IP32(comm_port);
		
		if((dn_rxf.Type == type) && ( rx_ip_32 == dtu_ip_32 ))
		{
			Rx_Dig_AutoDown( type, dn_rxf.data, pAutoDn );
			
			pthread_mutex_unlock(&DTU_COMM_Evt);
			return 1;
		}		
		else if( rx_ip_32 != dtu_ip_32 )
		{
			D_Printf( DEBUG_SYSTEM, "[Multi] [%s] rx IP fail 0x%08X != 0x%08X\r\n",\
			pAutoDn->str, rx_ip_32, dtu_ip_32);
		}
		else		
		{
			D_Printf( DEBUG_SYSTEM, "[Multi] [%s] rx CMD fail 0x%02X != 0x%02X\r\n", pAutoDn->str, dn_rxf.Type, type);
		}
	}
	else
	{
		D_Printf( DEBUG_SYSTEM, "[Multi] [!] [%s] Dn 0x%02X\r\n", pAutoDn->str, type );
	}
	pthread_mutex_unlock(&DTU_COMM_Evt);
    return 0;
}
*/





//------------------------------------------------------------------------------------


INT8U m_dn_data[DTU_DN_DATA_MAX];



#define Retry_Max (3)


void Dig_Down_Fail( DTU_Auto_DN* pAutoDn )
{	
	pAutoDn->retry = 0;
	pAutoDn->result = Multi_Dn_Fail;
	pAutoDn->st = Auto_Dn_None;
	pAutoDn->req = 0; // end
	
	D_Printf( DEBUG_SYSTEM, "[Multi] [%s] down fail \r\n", pAutoDn->str );
}





typedef struct
{
	OS_TIMER timer;
	INT8U en;
}SISO_FW_APPLY_DLY;
static SISO_FW_APPLY_DLY fw_apply_dly;


void Fw_Apply_Dly_Req(void)
{
	fw_apply_dly.en = 0;
}

void Update_DIG_DL_State(void)
{	
#ifdef _Debug____MULTI_DOWN_ST_	
	if(m_DTU_AutoDn.req)
	{
		m_St.DL_State[DN_DTU][1] = m_DTU_AutoDn.percent;
		m_St.DL_State[DN_DTU][DL_ST] = DL_St_Now;			
	}
	
	if(m_DTU_AutoDn.fw_apply_req)
	{
		m_St.DL_State[DN_DTU][1] = 100;
		m_St.DL_State[DN_DTU][DL_ST] = DL_St_Complete;
	}
	
	if(m_FPGA_AutoDn.req)
	{
		m_St.DL_State[DN_FPGA][1] = m_FPGA_AutoDn.percent;
		m_St.DL_State[DN_FPGA][DL_ST] = DL_St_Now;			
	}
	
	if(m_FPGA_AutoDn.fw_apply_req)
	{
		m_St.DL_State[DN_FPGA][1] = 100;
		m_St.DL_State[DN_FPGA][DL_ST] = DL_St_Complete;
	}
#endif	
}

INT8U Is_Dig_MultiDown(void)
{
	if( m_DTU_AutoDn.req || m_FPGA_AutoDn.req )
	{
		return 1;
	}

	if( info_FPGA.upgrade || info_Dtu.upgrade )
	{
		return 1;
	}
	
	else return 0;
}


/* EOF */


