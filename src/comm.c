/////////////////////////////
//	COMM.C
/////////////////////////////
#define COMM_GLOBALS
#include "includes.h"
/////////////////////////////


INT16U txInventry( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U cmd );
INT16U txSFP_Inventry( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
INT16U tx_FW_Update( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
INT16U txStatus_REMS_Hidden( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port );
INT16U GUI_ID_PW_Chk( INT8U* data, INT16U tot_len );


//-----------------------------------------------------
// Response for GUI-Status
INT16U txStatus( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, DEV_TYPE* dev, INT8U port, INT8U tree_req )
{
	INT16U len=0;
	INT8U totPkt, curPkt;
	
	totPkt = 1; // Response for GUI-Status
	curPkt = 1; // Response for GUI-Status

	//D_Printf(DEBUG_SYSTEM, " [txStatus] --\r\n");

	switch( dev->dev_type )
	{
		case DEV_TYPE_MINE:
			len = GetStatus(txf->data, port);
			break;
			
		default: return 0;
	}

	if(len > MAX_COM_BUFFER)
	{
		printf(VT_BOLD_RED "\n!!!!! TX SIZE MAX !!!!" VT_END_CR);
		return 0;
	}

	if(rxf->Type == CMD_STATUS)
	{
	#ifdef __TYPE__PRU__
		if((port == COMM_TERM)||(port == COMM_UDP_NODE))
		{
			if(GUI_ID_PW_Chk(rxf->data, rxf->len) == 0)
				return Make_Frame( 0xFF, rxf->Rcode, txf, 0, rxf->seqNo16, totPkt, curPkt);
		}
	#else
		if((port == COMM_TERM)||(port == COMM_UDP_NODE)||(port == COMM_BT))
		{
			if(GUI_ID_PW_Chk(rxf->data, rxf->len) == 0)
				return Make_Frame( 0xFF, rxf->Rcode, txf, 0, rxf->seqNo16, totPkt, curPkt);
		}
	#endif
	}

	// TREE_2019_0521
	if((dev->dev_type == DEV_TYPE_MINE)&&(rxf->Rcode == _RCODE_))
	{
		len += Make_Tree_Info( &txf->data[len], 1 ); // make self-RU status
	}
	else if((dev->dev_type != DEV_TYPE_MINE)&&(rxf->Rcode != _RCODE_)) // ?? 필요없을듯.
	{
		len += Make_Tree_Info( &txf->data[len], 1 ); // ?? 필요없을듯.
	}
	// END : TREE_2019_0521

	return Make_Frame(rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);
}

//-----------------------------------------------------------------------------------------------------
// TREE_2019_0521
INT8U Get__My_RU_ID(void)
{
	#if 1 // 2019.05.21 : cascade-id 기준.
	return AufoConfigFlag.RE_Cascade_ID;
	#else
	if(AufoConfigFlag.RU_Id_Alloc)
		return AufoConfigFlag.RU_Id;
	else
		return 0;
	#endif	
}

void Get__gREMS_My_SUBID(INT8U* pSUBID)
{
/*
	pSUBID[0] = 0;
	pSUBID[1] = 0xFF;
	pSUBID[2] = Get__My_RU_ID();
*/	
	// (2019.06.20) 1. REMS response : SUBID 0xFFFFFF 로 변경.
	pSUBID[0] = 0xFF;
	pSUBID[1] = 0xFF;
	pSUBID[2] = 0xFF;
}
// END : TREE_2019_0521


INT8U SUB_ID_Check( INT8U* sub_id, DEV_TYPE *dev )
{
// TREE_2019_0521
	// 2019.05.18
	INT8U self_ru_id;

	self_ru_id = Get__My_RU_ID();
	
	if((sub_id[0] == 0xFF)&&(sub_id[1] == 0xFF)&&(sub_id[2] == 0xFF))
	{
		dev->dev_type = DEV_TYPE_MINE;
		return _NORMAL_;
	}	
	else if((sub_id[2] == 0xFF)||(sub_id[2] == self_ru_id))
	{
		dev->dev_type = DEV_TYPE_MINE;
		return _NORMAL_;
	}
	else if(sub_id[2] <= 0x03)
	{
		dev->dev_type = DEV_TYPE__RU;
		return _NORMAL_;
	}
	else
	{
		dev->dev_type = DEV_TYPE_NONE;
		return _NO_SUBID_;
	}
// END : TREE_2019_0521
}

// TREE_2019_0521
// 2019.05.18
int Cal_Node_IP_Addr(INT8U* sub_id, INT8U* ip)
{	
	INT8U  self_ru_id;
	INT16S base = 0;
	
	if(sub_id[2] > 0x03) return RET_Error;
	
	self_ru_id = Get__My_RU_ID();
	memcpy(ip, m_St.Operate_Ip, 4);	

	if(AufoConfigFlag.RECTypeID == _TYPE_LS_SS_)
	{
		base = ip[3] - self_ru_id*8;
		ip[3] = 8*sub_id[2]+base;
	}
	else
	{
		base = ip[3] - self_ru_id*4;
		ip[3] = 4*sub_id[2]+base;
	}
	if(base < 0) return RET_Error;

	D_Printf(DEBUG_SYSTEM, VT_BLINK_GREEN "[Cal_Node_IP_Addr] self_ru_id %d, sbuid %d, %d.%d.%d.%d ", 
		self_ru_id, 
		sub_id[2], ip[0],ip[1],ip[2],ip[3]);

	return RET_SUCCESS;
}

typedef struct __attribute__ ((packed))
{
	INT8U  IpAddr[4];
	INT16U Port16;	
	INT16U Seq_No;
}GUI_IP_Table;

#define MAX_Bypass_GUI (10)
static GUI_IP_Table  m_Bypass_GUI[MAX_Bypass_GUI];
static INT8U idx_gui_ip_table = 0;

void Init_GUI_IP_Table(void)
{
	INT8U i;

	idx_gui_ip_table = 0;
	
	for(i=0; i<MAX_Bypass_GUI; i++)
		memset(&m_Bypass_GUI[i], 0, sizeof(GUI_IP_Table));
}

void Insert_GUI_IP_Table(INT8U* ip, INT16U port16, INT16U seqNo16)
{	
	if(idx_gui_ip_table >= MAX_Bypass_GUI)
		idx_gui_ip_table = 0;

	if(ip == NULL) return;

	memcpy(m_Bypass_GUI[idx_gui_ip_table].IpAddr, ip, 4);
	m_Bypass_GUI[idx_gui_ip_table].Port16 = port16;
	m_Bypass_GUI[idx_gui_ip_table].Seq_No = seqNo16;

	D_Printf(DEBUG_SYSTEM, VT_BLINK_GREEN "[Insert_GUI_IP] %d : seqNo16 %02x(%d), %d.%d.%d.%d/%d ", 
		idx_gui_ip_table, seqNo16, seqNo16, 
		ip[0],ip[1],ip[2],ip[3], port16);

	idx_gui_ip_table++;
}

int Find_GUI_IP(INT16U seqNo16, INT8U* ip, INT16U* pPort16)
{	
	INT8U i;
	
	D_Printf(DEBUG_SYSTEM, "[RCU] Find_GUI_IP %d\n", seqNo16);
	
	for(i=0; i<MAX_Bypass_GUI; i++)
	{
		if(m_Bypass_GUI[i].Seq_No == seqNo16)
		{
			memcpy(ip, m_Bypass_GUI[i].IpAddr, 4);
			*pPort16 = m_Bypass_GUI[i].Port16;

			
			D_Printf(DEBUG_SYSTEM, "[RCU] Find_GUI_IP : RET_SUCCESS, ip %d.%d.%d.%d/ %d\n", 
				ip[0],ip[1],ip[2],ip[3], *pPort16);
				
			return RET_SUCCESS;
		}		
	}

	D_Printf(DEBUG_SYSTEM, "[RCU] Find_GUI_IP : fail\n");
	return RET_Error;
}
// END : TREE_2019_0521

//--------------------------------------------------------------------------------
// 오류응답 command 0xFF : not defined @ gRMES Protocol

INT16U SendError( PROTOCOL_FRAME* txf, INT8U Rcode, INT8U err, INT8U port )
{
	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16 = Get_Tx_Frame_SeqNo(port);
	
	txf->data[0] = err;
	return Make_Frame( 0xFF, Rcode, txf, 1, seqNo16, totPkt, curPkt );
}
//--------------------------------------------------------------------------------
INT16U Make_JIg_Data( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT16U len=0;
	INT8U totPkt = 1, curPkt = 1;
	
	memcpy(txf->data, Jig_Data, Max_Jig_Data);
	len = Max_Jig_Data;

	return Make_Frame(rxf->Type, _RCODE_GUI, txf, len, rxf->seqNo16, totPkt, curPkt);
}
//--------------------------------------------------------------------------------

// 2020.04.02
INT8U IS__Download_Allowed(void)
{
	return 1;
/*
	switch(m_St.DL_State[DN_RCU][DL_ST])
	{
		case DL_St_IDLE:
		case DL_St_Complete:
		case DL_St_Fail:
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "!! Download is Not IDLE(DL_State 0x%X) ---- ", 
						m_St.DL_State[DN_RCU][DL_ST]);
			return 1;
			
		default: return 0;	
	}
*/	
}


/*
-------------------------------------------------------
2020.04.02 : 
	download 진행 중으로 5m 이상되면 fail로 가도록.
	download는 재시도 되도록.
-------------------------------------------------------
*/
//static OS_TIMER dl_now_timer;
void Proc__Check_Download_Now(void)
{
/*
	static INT8U init = 0;

	if(init == 0)
	{
		init = 1;
		OSTimerGet_msec( &dl_now_timer );
	}
	
	if(m_St.DL_State[DN_RCU][DL_ST] == DL_St_Now)
	{
		if(OSTimerGet_msec( &dl_now_timer) > TICK_CO_SEC*60*30) // 30m
		{
			OSTimerGet_msec( &dl_now_timer );
			m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;

			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "!! Download_Now over 3 min. Cleared to download fail ----");
		}
	}
	else
	{
		OSTimerGet_msec( &dl_now_timer );
	}
*/	
}




void DownLoadDataCheck( INT8U Type )
{	
	switch( Type )
	{
		// module download
		case 0xE8:
		case 0xE0:
		case 0xE1:
		case 0xE2:
			m_Env.dl_time_out = TICK_CO_SEC*30;
			OSTimerReset( &m_Env.dl_timer );
			m_Env.is_down_load = 1;			
        	break;

        case 0xE3:        	
			m_Env.dl_time_out = TICK_CO_SEC*5;
			OSTimerReset( &m_Env.dl_timer );
			m_Env.is_down_load = 1;			
        	break;
        	
        //-------------------------  		
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x08:
        //--------
        case 0x57:
        case 0x58:
        case 0x59:
			m_Env.dl_time_out = TICK_CO_SEC*30; // 2020.04.02 : 60s -> 30s
			OSTimerReset( &m_Env.dl_timer );
			m_Env.is_down_load = 1;				
        	break;
        	
        case 0x0E:
		case 0x0F: // Cancel Download
		case 0x5A:			
			m_Env.dl_time_out = TICK_CO_SEC*3;
			OSTimerReset( &m_Env.dl_timer );
			m_Env.is_down_load = 1;
        	break;
	}
}
//------------------------------------------------------------------------


INT16U Mod_Download( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{	
	INT16U ret_16 = 0;

	//-------------------------
	// Chk Module-SUBID
	/*
		rxf->data[0] ~ [2] 
		is 
		Module-SUBID[0] ~ [2]
		0  : DTU
		1~4: amp
		5  : PSU
	*/
	if(rxf->data[0] != 0x00) return 0;	
	if(rxf->data[1] != 0x01) return 0;	
	if(rxf->data[2] > 0x05) return 0;
	
	//-------------------------
	if(rxf->data[2] == 0x00) // DTU
	{					
		//ret_16 = DTU_Dnload_PassData( rxf->Type, rxf, txf );
	
		if(rxf->Type == 0xE3)
		{			
			if(m_Env.DnMode.Dig == _DN_RST_AUTO)			
			{
				m_Env.Dig_Down_AMP_Off_Req = 0;
			}			
		}
		return ret_16;
	}
	#ifdef __TYPE__PRU__	
	else 
	{
		if(rxf->data[2] == 0x05)
			return PSU_DownLoad_PassData( rxf->Type, rxf, txf ); // if PSU SUBID
		else 
			return PAU_DownLoad_PassData( rxf->Type, rxf, txf ); // if AMP SUBID
	}
	#else
	return 0; // AAU : Module download X
	#endif
}
//-----------------------------------------------------
// 미사용
INT16U txStatus_REMS_Hidden( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{
	INT16U len=0;	
	INT8U totPkt = 1, curPkt = 1;
	
	len = GetStatus_REMS_Hidden( txf->data, port );

	//if(port == COMM_TERM)
	//	len += Make_Tree_Info( &txf->data[len], 1 ); 
	
	return Make_Frame( rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt );
}
//------------------------------------------------------------------

INT16U Register_inquire( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )	
{
	INT8U totPkt = 1, curPkt = 1;

	INT8U	len;
	INT8U	type;
	INT32U	Addr;
	INT32U	Data;
	INT32U	W_and_R;

	#ifdef __TYPE__PRU__
	int fd;
	#endif


	txf->data[0] = rxf->data[0];

	memcpy( &txf->data[1] , &rxf->data[1], 4);
	
	type = rxf->data[0] & 0x07;
	
	
	if( rxf->data[0] & 0x80 )	///// write
		W_and_R = _SET_;
	else						///// read
		W_and_R = _CLR_;

	Addr = BYTE_to_INT32B(&rxf->data[1]);
	Data = BYTE_to_INT32B(&rxf->data[5]);

	#ifdef __YCSO__TEST__
	printf("_YCSO_ type = %d W_and_R = %d,  Addr = %x, Data = %x \n",type,W_and_R,Addr,Data);
	#endif
	switch( type )
	{
		case REGISTER___PS_:
			Addr *= 0x200; // 2019.06.19 : address 값을 0x200값을 곱해서 GUI에서 편하게 조회
			if( W_and_R )
			{
				Write_FPGA_Reg(Addr , Data, "cmd 0x99\n");
				#ifdef __YCSO__TEST__
				printf("_YCSO_ PS Write = %d,  Addr = %x, Data = %x \n",W_and_R,Addr,Data);
				#endif
			}
			Data = Read_FPGA_Reg(Addr);
			#ifdef __YCSO__TEST__
			printf("_YCSO_ PS Read = %d,  Addr = %x, Data = %x \n",W_and_R,Addr,Data);
			#endif
			break;

	#ifdef __TYPE__PRU__
		case REGISTER___PL_:
			fd = open("/dev/zynq_register_io_DTU", O_RDWR);

			if( fd > 0 )
			{
				Addr *= 0x200; // 2019.06.19 : address 값을 0x200값을 곱해서 GUI에서 편하게 조회
				if( W_and_R )
				{
					Wr_AXI_Reg_DTU( fd, Addr, Data );
					#ifdef __YCSO__TEST__
					printf("_YCSO_ PL Write = %d,  Addr = %x, Data = %x \n",W_and_R,Addr,Data);
					#endif
				}
				Data = Rd_AXI_Reg_DTU( fd, Addr );
				#ifdef __YCSO__TEST__
				printf("_YCSO_ PL Read = %d,  Addr = %x, Data = %x \n",W_and_R,Addr,Data);
				#endif
			}
			else
			{
				D_Printf(DEBUG_SYSTEM, "fail open /dev/zynq_register_io_DTU fd = %d \n", fd);
				Data = 0x00;
			}
			close(fd);			

			break;

		case REGISTER_CPRI_:
			
			if( W_and_R )
			{
				Write_A_PRU_Mmap_CMD( Addr, Data );
				#ifdef __YCSO__TEST__
				printf("_YCSO_ CPRI Write = %d,  Addr = %x, Data = %x \n",W_and_R,Addr,Data);
				#endif
			}
			
			Data = Read_A_PRU_Mmap_CMD( Addr );
			#ifdef __YCSO__TEST__
			printf("_YCSO_ CPRI Read = %d,  Addr = %x, Data = %x \n",W_and_R,Addr,Data);
			#endif
			break;
	#endif
	#ifdef __YCSO__TEST__
		case REGISTER__AFE_:
			fd = open("/dev/spidev0.0");
			if( fd > 0)
			{


			}
			else
			{
				D_Printf(DEBUG_SYSTEM, "fail open /dev/spidev0.0 fd = %d \n", fd);
				Data = 0x00;
			}

			close(fd);
			break;

		case REGISTER__LMK_:
			fd = open("/dev/spidev0.1");
			if( fd > 0)
			{


			}
			else
			{
				D_Printf(DEBUG_SYSTEM, "fail open /dev/spidev0.1 fd = %d \n", fd);
				Data = 0x00;
			}

			close(fd);

			break;
	#endif
		default:
			break;
	}

	INT32B_to_BYTE( &txf->data[5], Data);

	len = 1 + 4 + 4;
	
	return Make_Frame(rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);

}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-PRU	
INT16U Copy_POWER_Table( Tbl_DETEC_POWER* pdest, Tbl_DETEC_POWER* psrc )
{
	INT16U i;


	D_Printf(DEBUG_SYSTEM, "[CopyPowerTable] psrc->tbl_num [%d], psrc->init_val [%d], psrc->entry_cnt [%d]", 
		psrc->tbl_num, psrc->init_val, psrc->entry_cnt ); 

	if( psrc->entry_cnt>MAX_PWR_TBL_COUNT ) return 0;
	pdest->tbl_num	= psrc->tbl_num;
	pdest->init_val = psrc->init_val;
	pdest->entry_cnt = psrc->entry_cnt;

	for( i=0; i<psrc->entry_cnt; i++ )
	{
		pdest->entry[i] = Swap16(psrc->entry[i]);
		D_Printf(DEBUG_SYSTEM, "[%d] %02x ", i, pdest->entry[i]);
	}
	return ((pdest->entry_cnt*2)+3);
}

INT16U Copy_AMP_TEMP_Table( Tbl_AMP_TEMP* pdest, Tbl_AMP_TEMP* psrc )
{
	if( psrc->entry_cnt>MAX_AMP_TEMP_COUNT ) return 0;
	pdest->tbl_num	= psrc->tbl_num;
	pdest->init_val = psrc->init_val;
	pdest->entry_cnt = psrc->entry_cnt;

	memcpy( pdest->entry, psrc->entry, psrc->entry_cnt );

	return (pdest->entry_cnt+3);
}

INT16U Copy_ATT_Table( Tbl_ATT* pdest, Tbl_ATT* psrc )
{
	if( psrc->entry_cnt>MAX_ATT_TBL_COUNT ) return 0;
	pdest->tbl_num		= psrc->tbl_num;
	pdest->init_val		= psrc->init_val;
	pdest->entry_cnt	= psrc->entry_cnt;

	memcpy( pdest->entry, psrc->entry, psrc->entry_cnt );

	return (pdest->entry_cnt+3);
}

INT8S Get_TempAtt_Tbl(INT8S temp, INT8U idx)
{
	INT8U  tblCount=0; // table 안의 데이터 항목의 수.
	INT8S *pTable=0;  // data 부분의 포인터.
	INT8S  retValue=0, i;
	INT8S high_temp = 0;

	// table 비어 있으면, 디폴트값 리턴.

	high_temp = m_AMP_tmp_tbl[idx].init_val;
	
	if (m_AMP_tmp_tbl[idx].entry_cnt == 0)
		return 0;
	if (m_AMP_tmp_tbl[idx].entry_cnt > MAX_AMP_TEMP_COUNT)
		return 0;	
		
	pTable = &m_AMP_tmp_tbl[idx].entry[0];
	tblCount = m_AMP_tmp_tbl[idx].entry_cnt;	

	if( temp > high_temp )
	{
		retValue = pTable[0];
	}
	else if( temp <= (high_temp - 5*(tblCount-1)) )
	{
		retValue = pTable[tblCount-1];
	}
	else
	{
		for( i = 1; i < tblCount; i++ )
		{
			if( temp >= (high_temp - (5*i)) )
			{
				retValue = pTable[i-1];
				break;
			}
		}
	}
	return retValue;
}

INT8S Get_TempAtt_Alpha_Tbl(INT8S temp, INT8U idx)
{
	INT8U  tblCount=0; // table 안의 데이터 항목의 수.
	INT8S *pTable=0;  // data 부분의 포인터.
	INT8S  retValue=0, i;
	INT8S high_temp = 0;

	// table 비어 있으면, 디폴트값 리턴.

	high_temp = m_AMP_tmp_tbl_Alpha[idx].init_val;
	
	if (m_AMP_tmp_tbl_Alpha[idx].entry_cnt == 0)
		return 0;
	if (m_AMP_tmp_tbl_Alpha[idx].entry_cnt > MAX_AMP_TEMP_COUNT)
		return 0;	
		
	pTable = &m_AMP_tmp_tbl_Alpha[idx].entry[0];
	tblCount = m_AMP_tmp_tbl_Alpha[idx].entry_cnt;	

	if( temp > high_temp )
	{
		retValue = pTable[0];
	}
	else if( temp <= (high_temp - 5*(tblCount-1)) )
	{
		retValue = pTable[tblCount-1];
	}
	else
	{
		for( i = 1; i < tblCount; i++ )
		{
			if(temp >= (high_temp - (5*i)))
			{
				retValue = pTable[i-1];
				break;
			}
		}
	}
	return retValue;
}


INT8S Get_MaxMin_Temp_TempAtt_Tbl(INT8U idx, INT8S* max_temp, INT8S* min_temp)
{
	INT8S max = 0;
	INT8S min = 0;

	if(idx > 7) return RET_Error;	
	if(m_AMP_tmp_tbl[idx].entry_cnt <= 1) return RET_Error;	
	if(m_AMP_tmp_tbl[idx].entry_cnt > MAX_AMP_TEMP_COUNT) return RET_Error;	

	max = m_AMP_tmp_tbl[idx].init_val;	
	min = max - (m_AMP_tmp_tbl[idx].entry_cnt - 1)*5;

	*max_temp = max;
	*min_temp = min;

	D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] MaxMin TempAtt_Tbl[%d] %d/%d", idx, max, min);
	
	return RET_SUCCESS;
}
#endif
//-------------------------------------------------------------------------------------------------------------------
INT16U Table_PassData( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{	
	switch(rxf->data[0]) // tbl_idx
	{			
		case Tbl_ATT_AMP1_FWD_OUT_ATT:
		case Tbl_ATT_AMP2_FWD_OUT_ATT:
		case Tbl_ATT_AMP3_FWD_OUT_ATT:
		case Tbl_ATT_AMP4_FWD_OUT_ATT:

		case Tbl_ATT_AMP1_RVS_ATT:
		case Tbl_ATT_AMP2_RVS_ATT:
		case Tbl_ATT_AMP3_RVS_ATT:
		case Tbl_ATT_AMP4_RVS_ATT:

		case Tbl_FWD_AMP1_TEMP:
		case Tbl_FWD_AMP2_TEMP:
		case Tbl_FWD_AMP3_TEMP:
		case Tbl_FWD_AMP4_TEMP:

		case Tbl_RVS_AMP1_TEMP:
		case Tbl_RVS_AMP2_TEMP:
		case Tbl_RVS_AMP3_TEMP:
		case Tbl_RVS_AMP4_TEMP:
	
		case Tbl_FB0_AMP1_TEMP:
		case Tbl_FB0_AMP2_TEMP:
		case Tbl_FB0_AMP3_TEMP:
		case Tbl_FB0_AMP4_TEMP:
			return PAU_Table_ATT_PassData( rxf, txf );
			
		default : return 0;
	}	
}

///// YCSO added START 190503
#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU	

void Table_Write( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port  )	
{

	INT8U tbl_num;
	INT8U tbl_idx;
	//txf->data[0] = RFU_num = rxf->data[0];
	tbl_num = rxf->data[0];

	switch( tbl_num )
	{
		case Tbl_FWD_AMP1_TEMP: 	 
		case Tbl_FWD_AMP2_TEMP: 	 
		case Tbl_FWD_AMP3_TEMP: 	 
		case Tbl_FWD_AMP4_TEMP: 	 
		case Tbl_RVS_AMP1_TEMP: 	 
		case Tbl_RVS_AMP2_TEMP: 	 
		case Tbl_RVS_AMP3_TEMP: 	 
		case Tbl_RVS_AMP4_TEMP:
			tbl_idx = tbl_num;
			Copy_AMP_TEMP_Table(&m_AMP_tmp_tbl[tbl_idx], (Tbl_AMP_TEMP*)&rxf->data[0]);
			Save_AMP_TEMP_Table(tbl_idx);
			break;


		case Tbl_ATT_AMP1_FWD_OUT_ATT:
		case Tbl_ATT_AMP2_FWD_OUT_ATT:	
		case Tbl_ATT_AMP3_FWD_OUT_ATT:	
		case Tbl_ATT_AMP4_FWD_OUT_ATT:	
		case Tbl_ATT_AMP1_RVS_ATT:	
		case Tbl_ATT_AMP2_RVS_ATT:		
		case Tbl_ATT_AMP3_RVS_ATT:		
		case Tbl_ATT_AMP4_RVS_ATT:
			tbl_idx = tbl_num - 8;
			Copy_ATT_Table(&m_ATT_AMP_tbl[tbl_idx], (Tbl_ATT*)&rxf->data[0]);
			Save_ATT_Table(tbl_idx);
			break;


    	case Tbl_DL0_OUT_PWR_DET:		// Pwr
    	case Tbl_DL1_OUT_PWR_DET:
    	case Tbl_DL2_OUT_PWR_DET:
    	case Tbl_DL3_OUT_PWR_DET:
    	case Tbl_UL0_IN_PWR_DET:
    	case Tbl_UL1_IN_PWR_DET:
    	case Tbl_UL2_IN_PWR_DET:
    	case Tbl_UL3_IN_PWR_DET:
			tbl_idx = tbl_num - 20;
			Copy_POWER_Table(&m_pwr_tbl[tbl_idx], ( Tbl_DETEC_POWER*)&rxf->data[0]);
			Save_POWER_Table(tbl_idx);
    		break;

		// 2019.06.27 : AAU-Temp-Att Alpha
		case Tbl_DL0_TMP_ALPHA_ONLY: 	 
		case Tbl_DL1_TMP_ALPHA_ONLY: 	 
		case Tbl_DL2_TMP_ALPHA_ONLY: 	 
		case Tbl_DL3_TMP_ALPHA_ONLY: 	 
		case Tbl_UL0_TMP_ALPHA_ONLY: 	 
		case Tbl_UL1_TMP_ALPHA_ONLY: 	 
		case Tbl_UL2_TMP_ALPHA_ONLY: 	 
		case Tbl_UL3_TMP_ALPHA_ONLY:
			tbl_idx = tbl_num - 32;
			Copy_AMP_TEMP_Table(&m_AMP_tmp_tbl_Alpha[tbl_idx], (Tbl_AMP_TEMP*)&rxf->data[0]);
			Save_AMP_TEMP_Alpha_Table(tbl_idx);
			break;
			
		default:
			break;

	}

}

//Sts - Table sts
INT16U Table_Read( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port  )	
{
	INT8U totPkt = 1, curPkt = 1;
//	INT16U seqNo16 = Get_Tx_Frame_SeqNo(port);

	INT16U len;
	INT8U tbl_num;
	INT8U tbl_idx;

	//txf->data[0] = RFU_num = rxf->data[0];
	tbl_num = rxf->data[0];

	switch( tbl_num )
	{
		case Tbl_FWD_AMP1_TEMP: 	 
		case Tbl_FWD_AMP2_TEMP: 	 
		case Tbl_FWD_AMP3_TEMP: 	 
		case Tbl_FWD_AMP4_TEMP: 	 
		case Tbl_RVS_AMP1_TEMP: 	 
		case Tbl_RVS_AMP2_TEMP: 	 
		case Tbl_RVS_AMP3_TEMP: 	 
		case Tbl_RVS_AMP4_TEMP:
			tbl_idx = tbl_num;
			Load_AMP_TEMP_Table(tbl_idx);
			len = Copy_AMP_TEMP_Table( (Tbl_AMP_TEMP*)&txf->data[0], &m_AMP_tmp_tbl[tbl_idx]);
 			break;

		case Tbl_ATT_AMP1_FWD_OUT_ATT:
		case Tbl_ATT_AMP2_FWD_OUT_ATT:	
		case Tbl_ATT_AMP3_FWD_OUT_ATT:	
		case Tbl_ATT_AMP4_FWD_OUT_ATT:	
		case Tbl_ATT_AMP1_RVS_ATT:	
		case Tbl_ATT_AMP2_RVS_ATT:		
		case Tbl_ATT_AMP3_RVS_ATT:		
		case Tbl_ATT_AMP4_RVS_ATT:		
			tbl_idx = tbl_num-8;
			Load_ATT_Table(tbl_idx);
			len = Copy_ATT_Table( (Tbl_ATT*)&txf->data[0], &m_ATT_AMP_tbl[tbl_idx]);
 			break;


		case Tbl_DL0_OUT_PWR_DET:		// Pwr
		case Tbl_DL1_OUT_PWR_DET:
		case Tbl_DL2_OUT_PWR_DET:
		case Tbl_DL3_OUT_PWR_DET:
		case Tbl_UL0_IN_PWR_DET:
		case Tbl_UL1_IN_PWR_DET:
		case Tbl_UL2_IN_PWR_DET:
		case Tbl_UL3_IN_PWR_DET:
			tbl_idx = tbl_num-20;
			Load_POWER_Table(tbl_idx);
			len = Copy_POWER_Table( (Tbl_DETEC_POWER*)&txf->data[0], &m_pwr_tbl[tbl_idx]);
 			break;

		// 2019.07.27 : AAU-Temp-Att for Alpha
		case Tbl_DL0_TMP_ALPHA_ONLY:		// Pwr
		case Tbl_DL1_TMP_ALPHA_ONLY:
		case Tbl_DL2_TMP_ALPHA_ONLY:
		case Tbl_DL3_TMP_ALPHA_ONLY:
		case Tbl_UL0_TMP_ALPHA_ONLY:		// Pwr
		case Tbl_UL1_TMP_ALPHA_ONLY:
		case Tbl_UL2_TMP_ALPHA_ONLY:
		case Tbl_UL3_TMP_ALPHA_ONLY:
			tbl_idx = tbl_num-32;
			Load_AMP_TEMP_Alpha_Table(tbl_idx);
			len = Copy_AMP_TEMP_Table( (Tbl_AMP_TEMP*)&txf->data[0], &m_AMP_tmp_tbl_Alpha[tbl_idx]);
 			break;

		default : return 0;

	}
	return Make_Frame( rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt );
}
#endif
///// YCSO added END
//-------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------
INT16U MaskSet( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT8U totPkt = 1, curPkt = 1;
	
	m_Eep.mask = rxf->data[0];
	txf->data[0] = rxf->data[0];
	
	/*	
		0x01: 알람 Masking 설정 
		0x00: 알람 Masking 해제
	*/
	
	m_Env.is_eep_write = EEP_WRITE_CODE;

	return Make_Frame( rxf->Type, rxf->Rcode, txf, 1, rxf->seqNo16, totPkt, curPkt );
}
//------------------------------------------------------------------------------------------------
INT16U Power_Reset_Ctrl( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT8U totPkt = 1, curPkt = 1;
	
#ifdef __TYPE__PRU__	
	m_Env.psu_reset_req = 1;	
	SetResetInfo( Rst_By_PwrCtrl, 861 ); // 2020.03.15 : psr reset 방식(바로 halt제어)바뀌면서 추가.
#else // 2019.06.19
	SetResetInfo( Rst_By_PwrCtrl, 1212 ); // 2019.10.22 : 'Rst_By_User' -> 'Rst_By_PwrCtrl'
	m_Env.is_reboot = 1; 
#endif

	return Make_Frame(rxf->Type, rxf->Rcode, txf, 0, rxf->seqNo16, totPkt, curPkt);
}

//------------------------------------------------------------------------------------------------

INT16U Get_LogStatus( INT8U* sdu, INT8U port )
{
	return Get_GUI_AID_Val( LOG___AID_idx, LOG___AID_num, sdu, port, 0 );
}

INT16U tx_LogStatus(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, DEV_TYPE* dev, INT8U port)
{
	INT16U len=0;
	INT8U totPkt, curPkt;
	
	totPkt = 1; // Response for GUI-Status
	curPkt = 1; // Response for GUI-Status
		
	m_St.Log_Cnt = get__list_of_5m_log_file();	// get log file list
	
	len = Get_LogStatus(txf->data, port);		

	return Make_Frame(CMD_RCU_LOG_File_RSP, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);
}
//------------------------------------------------------------------------------------------------
INT16U tx_LogFile(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, DEV_TYPE* dev, INT8U port)
{
	INT16U l=0;
	INT8U totPkt, curPkt;
	char file_name[128];
	
	
	totPkt = 1; // Response for GUI-Status
	curPkt = 1; // Response for GUI-Status

	memset(file_name, 0, 128);
	
	if(rxf->len < 128)
	{
		memcpy(file_name, rxf->data, rxf->len);		
		print_agent(DEBUG_LOG, VT_BOLD_CYAN, "0xB2 Command : %s, len %d", file_name, strlen(file_name));
	}
	
	if(Set__send_5m_log_file_via_sz(file_name, port))	
	{
		txf->data[l++] = 0x01; // 파일유무(0x01:있음)
		txf->data[l++] = 0x01; // 파일전송(0x01:ready)
	}	
	else
	{
		if(strlen(file_name) == 0)
		{
			txf->data[l++] = 0x01; // 파일유무(0x01:있음)
			txf->data[l++] = 0x00;
		}
		else
		{
			txf->data[l++] = 0x00;
			txf->data[l++] = 0x00;
		}
	}

	return Make_Frame(CMD_RCU_LOG_File_GET_RSP, rxf->Rcode, txf, l, rxf->seqNo16, totPkt, curPkt);
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
INT16U tx_LogFileList(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, DEV_TYPE* dev, INT8U port)
{
	INT16U l=0;
	INT8U totPkt, curPkt;
	INT16U i;
	INT16U size;
	char file_name[128];
	
	totPkt = 1; // Response for GUI-Status
	curPkt = 1; // Response for GUI-Status
	
	txf->data[l++] = 0x5A;
	txf->data[l++] = 0x5A;

	for(i=0; i < m_St.Log_Cnt; i++) // max 500
	{
		strcpy(file_name, get__name_of_5m_log_file(i));
		size = strlen(file_name);

	////print_agent(DEBUG_LOG, VT_BOLD_CYAN, "%3d (%3d byte) %s", i, size, get__name_of_5m_log_file(i));
		
		memcpy(&txf->data[l], file_name, size);
		l+=size;		
	
		txf->data[l++] = 0x0D;
		txf->data[l++] = 0x0A;
	}

	return Make_Frame(CMD_RCU_LOG_File_LIST_RSP, rxf->Rcode, txf, l, rxf->seqNo16, totPkt, curPkt);
}
//------------------------------------------------------------------------------------------------



INT8U Is__Command_of_LogParser_GUI(INT8U command)
{
	switch(command)
	{	
		case CMD_GUI_LOG_File_REQ:
		case CMD_RCU_LOG_File_RSP:
		case CMD_GUI_LOG_File_GET_REQ:
		case CMD_RCU_LOG_File_GET_RSP:
		case CMD_GUI_LOG_File_LIST_REQ:
		case CMD_RCU_LOG_File_LIST_RSP:
			return _TRUE_;
			
		default:
			return _FALSE_;
	}
}
//------------------------------------------------------------------------------------------------
/*
PROTOCOL_FRAME *rxf 
	rxf->seqNo16 : host order byte
	rxf->h_len : host order byte
	rxf->len : host order byte

*/
INT16U UP_Data_Analysis( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port )
{
	INT8U err;
	DEV_TYPE dev;	
	INT8U tree_req=0;
	static INT8U init = 0;
	INT8U i;
	

	
	//-------------------------------------------------
	if(init == 0)
	{
		Init_GUI_IP_Table();
		init = 1;
	}

	if((rxf->SUB_ID[0] == 0xFF)&&(rxf->SUB_ID[1] == 0xFF)&&(rxf->SUB_ID[2] == 0xFF))
	{
		txf->SUB_ID[0] = rxf->SUB_ID[0];
		txf->SUB_ID[1] = rxf->SUB_ID[1];
		txf->SUB_ID[2] = rxf->SUB_ID[2];
	}
	else
	{
		txf->SUB_ID[0] = rxf->SUB_ID[0];
		txf->SUB_ID[1] = rxf->SUB_ID[1];
		txf->SUB_ID[2] = Get__My_RU_ID();
	}
	err = SUB_ID_Check( rxf->SUB_ID, &dev );


	if(err) 
	{
		return SendError( txf, rxf->Rcode, gREMS_Err____SUBID, port );
	}	

	//---------------------------------------------------
	// ACK from Server : Alarm report, Auto Report
	if((port == COMM_UDP_REMS)||(port == COMM_UDP_NODE))
	{		
		if(dev.dev_type == DEV_TYPE_MINE)
		{
			// SUBID is 0xFFFFFF, 2019.06.20			
			switch( rxf->Type )
			{
				case CMD2_ALARM_LOC_RPT  :
				case CMD2_SUPP_ALARM_RPT :
				case CMD2_STATISTIC_RPT  :
					print_agent(DEBUG_REMS, VT_Color_RX, "[from REMS] %s", Get_CMD_Str(rxf->Type));					
					if(m_Alm_Rpt_Info.cmd == rxf->Type)
					{
						////if(memcmp( m_Alm_Rpt_Info.subid, rxf->SUB_ID, 3) == 0)
						//{							
							Init_Auto_Rpt_Flag();
							m_Alm_Rpt_Info.cmd = 0; // init
							
							for(i=0; i<ALM_RPT_Chk_Max; i++)
							{
								if(m_Alm_Rpt_Info.aid16[i] == 0x5452) // Super Cap. 교체
								{
									print_agent(DEBUG_REMS, VT_Color_TX, "[REMS Alm ACK] Clear Super-Cap-ExCH Alarm"); 
									m_St.AlmSt.ETC.SuperCap_Exch = 0;
								}
							}							
						//}
						//else
						//{
						//	D_Printf(DEBUG_REMS, "SUBID 0x%02X%02X%02X != 0x%02X%02X%02X\n",
						//		m_Alm_Rpt_Info.subid[0], m_Alm_Rpt_Info.subid[1], m_Alm_Rpt_Info.subid[2],
						//		rxf->SUB_ID[0], rxf->SUB_ID[1], rxf->SUB_ID[2]); 
						//}
					}
					else
					{
						print_agent(DEBUG_REMS, VT_Color_RX, "[from REMS] 0x%02X != CMD(x%02X)", 
							rxf->Type, m_Alm_Rpt_Info.cmd); 
					}
					break;
					
				case CMD2_CFG_nC_AUTO_RPT:
				case CMD2_CFG__C_AUTO_RPT:
					// 2019.10.30 : if length over 100, invalid packet
					if(rxf->len > Node_Name_MAX) 
					{
						print_agent (DEBUG_REMS, VT_BOLD_RED, "[CFG_C/CFG_nC AUTO_RPT(0x%X)] rx len(%d) is over %d, skip", 
								rxf->Type, rxf->len, Node_Name_MAX);
								
						return 0;
					}
					else
					{					
						print_agent(DEBUG_REMS, VT_Color_RX, "[from REMS] %s", Get_CMD_Str(rxf->Type)); 
						if(m_AutoRpt_Info.cmd == rxf->Type)
						{
							Update_BTS_Name(rxf->data, rxf->len);
							Init_Auto_Rpt_Flag();
							m_AutoRpt_Info.cmd = 0; // init						
						}
						else
						{
							print_agent(DEBUG_REMS, VT_Color_RX, "[from REMS] 0x%02X != CMD(x%02X)", 
								rxf->Type, m_AutoRpt_Info.cmd); 
						}
					}
					break;

				case CMD2_LocalTime_RSP: // 2019.08.06
					return rx_LocalTime(rxf);
					
				case CMD2_GUI_ID_PW	:						
					return rx_GUI_ID_PW(rxf);

				default: break;
			}
		}		
	}	
	//---------------------------------------------------	

	/*	
	2020.03.25
		Log Paser GUI 연결 시, 상태 감시접속 알람이 발생하지 않도록 요청.
	*/
	

	//---------------------------------------------------	
	if(port == COMM_TERM)
	{		
		Reset_Local_Timer(); // reset GUI-timer

		if(Is__Command_of_LogParser_GUI(rxf->Type)== _FALSE_) // 2020.03.25 : if not from LogGUI
		{
			Local_Terminal = 1; // GUI connection Alarm
		}

		/*
		//------------------------
		// 2020.03.25
		//------------------------
		BT initialize 추가 (for BT-Re-Connection)
		(Serial GUI 연결과 관계없이) BT 데이터가 20s정도 수신되지 않는 경우 BT initialize.
		//------------------------
		// 2020.03.25 : below is deleted..		
		//------------------------
		//#ifdef __TYPE__AAU__
		//Reset_BT_Re_Init_Timer(); // 191204
		//#endif
		*/
	}	

#ifdef __TYPE__AAU__ //---------- AAU
	if(port == COMM_BT)
	{				
		Reset_Local_Timer(); // reset GUI-timer
		if(Is__Command_of_LogParser_GUI(rxf->Type)== _FALSE_) // 2020.03.25 : if not from LogGUI
		{
			BlueT_Terminal = 1; // GUI connection Alarm
		}
		Reset_BT_Re_Init_Timer(); // 191204
	}
#endif	

	if(dev.dev_type == DEV_TYPE_MINE)
	{
		/*
		//// switch( rxf->Type )
		//// {
		//// 	case 0xE8: // module download
		//// 	case 0xE0:
	    ////     case 0x0B:
	    ////     case 0x08:	    
	    ////     case 0x57:
	    ////     	if(IS__Download_Allowed()==0)
	    ////     		return 0;
	    ////     	break;
		//// }
		*/
		DownLoadDataCheck( rxf->Type );		

		switch( rxf->Type )
		{
			//-------------------------------
			// Status
			case CMD_STATUS:
				return txStatus(rxf, txf, &dev, port, tree_req);
			//-------------------------------
			// Ctrl
			case CMD_CONTROL:
				if( rcu_reset_req ) return 0;
				
				wr_ctrl_cmd_log(rxf);
				Wr_Ctl_Pkt_to_log(rxf);
				if(LocalCtrl(rxf->data, rxf->len, rxf->Type))
				{					
					return txStatus( rxf, txf, &dev, port, tree_req );
				}
				else return 0;
								
			//-------------------------------
			// Mask
			//-------------------------------
			case CMD_ALARM_MASK: 
				return MaskSet( rxf, txf );

			//-------------------------------
			// Download
			//-------------------------------
			case 0x50:
				wr_ctrl_cmd_log(rxf);
				Wr__Event_to_LogFile("[FTP Down   ] Command 0x%02X", rxf->Type);
				
				// 2019.07.03 : scyo
				if(r_ip == NULL) memset(&comm_info[port].ip[0], 0x00,    4); // 2020.04.08
				else             memcpy(&comm_info[port].ip[0], &r_ip[0],4);
				
				comm_info[port].port = r_udp_port;
				return ftp_get_pkg( rxf, txf, port, r_ip ); 

			case 0x08:
			case 0x0B:
			case 0x57:
				wr_ctrl_cmd_log(rxf);
				Wr__Event_to_LogFile("[DOWN Start ] Command 0x%02X", rxf->Type);
				return rxStartDownload( rxf, txf, port );
				
			case 0x0C:
			case 0x58:
				wr_ctrl_cmd_log(rxf);
				Wr__Event_to_LogFile("[DOWN Start2] Command 0x%02X", rxf->Type);
				return rxReStartDownload( rxf, txf, port );
				
			case 0x0D:
			case 0x59:
				D_Printf(DEBUG_SYSTEM, "[rxDataDownload] seqNo16 %d \n", rxf->seqNo16);
				return rxDataDownload ( rxf, txf, port );
				
			case 0x0E:
			case 0x5A:
				wr_ctrl_cmd_log(rxf);
				Wr__Event_to_LogFile("[DOWN END   ] Command 0x%02X", rxf->Type);
				return rxFinishConfirm( rxf, txf, port );

			case CMD_FW_UPDATE: // 0x5B
				wr_ctrl_cmd_log(rxf);
				Wr__Event_to_LogFile("[FW_UPDATE  ] Command 0x%02X", rxf->Type);
				return rxDNconfirm( rxf, txf, port );

			///// YCSO added START 191212
		#ifdef __USE_Packet_Division_for_WaveSpectrum__
			case CMD_WAVE_Spectrum: //// 0x5f
				tx_gREMS_spectrum_set( rxf, txf, port, r_ip, r_udp_port );
				return rxSpectrumRequest( rxf, txf );
		#endif	
			case 0x63:
				return rxSpectrumRequest( rxf, txf );
			///// YCSO added END 191212	

			//-------------------------------
			// Module Download
			//-------------------------------
			case 0xE8:
			case 0xE0:
			case 0xE1:
			case 0xE2:
			case 0xE3:
				return Mod_Download( rxf, txf );

			//-------------------------------
			// Table
			//----------
		#ifdef __TYPE__PRU__
			case 0x75:
			case 0x76:			
				return Table_PassData( rxf, txf, port );
		#else // ANTS-AAU
			case 0x75:
				Table_Write( rxf, txf, port );
			case 0x76:
				return Table_Read( rxf, txf, port );
		#endif		
			
			//-------------------------------
			// PSU Reset
			//----------
			case CMD_PSU_RESET_REQ:
				wr_ctrl_cmd_log(rxf);
				Wr_Ctl_Pkt_to_log(rxf);
				Wr__Event_to_LogFile("[PSU Reset  ] Command 0x%02X", rxf->Type);
				return Power_Reset_Ctrl( rxf, txf );

			//-------------------------------
			// Inventory
			case CMD_INVENTORY_SET: // Inventory 설정 
				SetInventory( rxf->data, rxf->len );
				return txInventry( rxf, txf, CMD_INVENTORY_SET );
				
			case CMD_Get_INVENTORY: // Inventory 조회
				InvetoryReadClear();	
				return txInventry( rxf, txf, CMD_Get_INVENTORY );

			case CMD2_SFP_Mod_Inv_St:
				return txSFP_Inventry( rxf, txf );
				
			case CMD2_BAR_Code:
				return txBarCode( rxf, txf );

			//// SIM req
			case 0xF0:		//// SIM YCSO
				return SIM_task_req( rxf, txf );

			case 0xF2:		//// SIM YCSO
				return rxSpectrumReq_SIM( rxf, txf );
			
			case 0x99:
				return Register_inquire( rxf, txf);				
				
			// Hidden
			case CMD_REMS_HIDDEN:
				return tx_gREMS_Hidden_Status( rxf, txf, port, r_ip, r_udp_port );
				
			//-------------------------------
			case CMD2_ST2_MEA_RPT:
					return tx_gREMS_Mea_St2_Status( rxf, txf, port, r_ip, r_udp_port );

			case CMD2_CFG__C_SET_REQ:
			case CMD2_CFG_nC_REQ:
			case CMD2_PLD_SET_REQ:				
				if(rxf->len) 
				{
					if((rxf->Type==CMD2_PLD_SET_REQ)||(rxf->Type==CMD2_CFG__C_SET_REQ)) 
					{
						wr_ctrl_cmd_log(rxf);
						Wr_Ctl_Pkt_to_log(rxf);
					}
					
					LocalCtrl( rxf->data, rxf->len, rxf->Type );
				}
			
				if( rxf->Type == CMD2_CFG_nC_REQ )
					return tx_gREMS_CFG_nC_Status( rxf, txf, port, r_ip, r_udp_port );
				else // CF_C, PLD
				{
					print_agent(DEBUG_REMS, VT_BOLD_MAGENTA, "CMD 0x%X ---- ", rxf->Type);
					return tx_gREMS_CFG__C_Status( rxf, txf, port, r_ip, r_udp_port );
				}
			
			case CMD2_EMS_LOOPBACK:
				return tx_EMS_Loopback( rxf, txf );

			case CMD_GUI_LOG_REQ:
				return tx_GUI_Log(rxf, txf);
			
			//-------------------------------
			// Module Download
			//-------------------------------
			case 0x29: // 지그모드.			
				JIG_MODE = JIG_MODE_CODE;
				return Make_JIg_Data( rxf, txf );

			//-------------------------------
			// Log
			//-------------------------------
			case CMD_GUI_LOG_File_REQ: 
				return tx_LogStatus( rxf, txf, &dev, port);
				
			case CMD_GUI_LOG_File_GET_REQ: 
				return tx_LogFile( rxf, txf, &dev, port);
				
			case CMD_GUI_LOG_File_LIST_REQ: 
				return tx_LogFileList( rxf, txf, &dev, port);

			//-------------------------------
			default: return 0;
			//-------------------------------
		}
	}
// TREE_2019_0521
	else if(dev.dev_type == DEV_TYPE__RU) // 2019.05.18
	{
		return Pass_Node_Data( port, &dev, rxf, r_ip, r_udp_port );
	}
// END : TREE_2019_0521
	return 0;
}



// TREE_2019_0521
void PassAckData_ToGUI( INT8U recv_rcode, PROTOCOL_FRAME* rxf )
{
	INT16U len;
	INT8U comm_port = COMM_TERM;
	INT8U ip[4];
	INT16U udp_port16 = 0;	
	INT8U debug_on = 0;
	
	rxf->Rcode = _RCODE_;	

	if(debug_on)
		D_Printf(DEBUG_SYSTEM, "[RCU] PassAckData_ToGUI : recv_rcode 0x%X, 0x%X\n", recv_rcode, rxf->seqNo16);

	switch( recv_rcode )
	{	
		case _RCODE_GUI:
			comm_port = COMM_TERM;
			udp_port16 = 0;			
			break;
			
		case _RCODE_UDP:
			comm_port = COMM_UDP_NODE;
			if(Find_GUI_IP(rxf->seqNo16, ip, &udp_port16) == RET_Error)
				return;
			break;
			
		#ifdef __TYPE__AAU__
		case _RCODE_USB: // bluetooth
			comm_port = COMM_BT;
			udp_port16 = 0;			
			break;
		#endif

		default:
			return;
	}

	if(debug_on)	
	{
		D_Printf(DEBUG_SYSTEM, "[RCU] before Re_Make_Frame : seqNo 0x%X(%d), h_len 0x%X(%d), len 0x%X(%d)", 
			rxf->seqNo16, rxf->seqNo16, 
			rxf->h_len, rxf->h_len,
			rxf->len, rxf->len);
	}
	
	len = Re_Make_Frame(rxf);	

	if(debug_on)	
	{
		D_Printf(DEBUG_SYSTEM, "[RCU] after Re_Make_Frame : seqNo 0x%X(%d), h_len 0x%X(%d), len 0x%X(%d), crc 0x%02X%02X\n", 
			rxf->seqNo16, rxf->seqNo16, 
			rxf->h_len, rxf->h_len,
			rxf->len, rxf->len,
			rxf->crc[0], rxf->crc[1]);	
	}	
	Send_UDP_Data_GUI(comm_port, ip, (INT8U*)rxf, len, udp_port16);		
}
// END : TREE_2019_0521



// TREE_2019_0521
INT16U NodeDataAck( PROTOCOL_FRAME* rxf, INT8U* r_ip ) // Node 간 UDP 에서 받은 데이터 선처리.
{
	INT8U err;
	DEV_TYPE dev;
	INT8U recv_rcode;
	INT8U tree_len;

	(void)r_ip;

	recv_rcode = rxf->Rcode & 0xF0;

	switch( recv_rcode )
	{
		case _RCODE_GUI: // GUI 에서 수신 (GUI 보고용)
		case _RCODE_USB: // USB 에서 수신 (USB 보고용)
		case _RCODE_UDP:	

			err = SUB_ID_Check( rxf->SUB_ID, &dev );
			if( err ) return 0;

			// DEV_TYPE_MINE 이면 UP_Data_Analysis 에서 처리
			if( dev.dev_type == DEV_TYPE_MINE ) return 0; 
				
			switch( rxf->Type )
			{
				case 0x00:
				case 0x01:
					tree_len = Make_Tree_Info( &rxf->data[rxf->len], 1 ); 
					rxf->len += tree_len;
					rxf->h_len += tree_len;
					break;
			}
			PassAckData_ToGUI( recv_rcode, rxf );
			return 1;
			
		default: 
			return 0;
	}
}
// END : TREE_2019_0521




INT16U Make_Alive_Data( PROTOCOL_FRAME* txf, INT8U port )
{
	INT16U len=0;
	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16 = Get_Tx_Frame_SeqNo(port);

	Get__gREMS_My_SUBID(txf->SUB_ID);

#if 0 // later, when PRU/AAU
	len = MakeAliveStatus( txf->data );
#endif
	
	if( len == 0 ) return 0;
	
	return Make_Frame( CMD_RO_ALIVE, _RCODE_, txf, len, seqNo16, totPkt, curPkt );
	
}

//================================================================================
INT8U Get_Pkt_Idx( PROTOCOL_FRAME* rxf )
{
	AIDSTR* paid = (AIDSTR*)rxf->data;

	if((paid->AID[0] == 0x23) && (paid->AID[1] == 0x23))
	{
		if( paid->data[0] == 0 )
			return 1; // default
		else
			return paid->data[0];
	}
	return 1; // default
}




//---------------------------------------------------------------




//==========================================================================


//-----------------------------------------------------

//-----------------------------------------------------
INT8U Login_Info_Valid(INT8U* pID_Str, INT8U* pPW_Str)
{
	     if(memcmp(m_Eep.User_ID, pID_Str, 10)) return 0;
	else if(memcmp(m_Eep.User_PW, pPW_Str, 10)) return 0;	
	else return 1;
}

/*
[TX] 
16 16 16 16 00 01 01 01 00 35 38 FF FF FF 00 50 00 2C 
B0 12 00 12 08 00 00 0B 01 01 80 31 32 00 00 00 00 00 00 00 00 00 
B0 13 00 12 08 00 00 0B 01 01 80 31 32 00 00 00 00 00 00 00 00 00 
EB 65 03 
[RX] 16 16 16 16 00 01 01 01 00 09 0C FF FF FF FF 50 00 00 BE 0C 03 

[TX] 
16 16 16 16 00 02 01 01 00 35 39 FF FF FF 00 50 00 2C 
B0 12 00 12 08 00 00 0B 01 01 80 73 6B 74 00 00 00 00 00 00 00 00 
B0 13 00 12 08 00 00 0B 01 01 80 73 6B 74 31 32 33 71 77 65 00 00 
E8 BD 03
*/

INT16U GUI_ID_PW_Chk(INT8U* data, INT16U tot_len)
{
	INT16U cnt=0;	

	m_Env.gui_id_req = 0;
	m_Env.gui_pw_req = 0;

	while(cnt < tot_len)
	{
		cnt += AID_Control( &data[cnt], CMD_STATUS );
	}

	if(m_Env.gui_id_req && m_Env.gui_pw_req)
	{
		return Login_Info_Valid(m_Env.User_ID, m_Env.User_PW);
	}

	return 1; // aid가 없으면, 평소대로.
}
//-----------------------------------------------------
INT16U txInventry( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U cmd )
{
	INT16U len=0;
	INT8U totPkt = 1, curPkt = 1;

// 요청시, 직접 PAU, PSU 읽으려면, 아래를 활성화.
#ifdef __TYPE__PRU__ // PRU Only	
#if 0 // 2019.09.04
	INT8U path4;
	
	if (cmd == CMD_Get_INVENTORY)
	{
		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		{
			if(Check_PAU_Use(path4))
				sendPAU_GetInfo(path4);
		}
		PSU_Get_Info();
	}
#endif	
#endif

	len = Get_Inventory(txf->data);
	return Make_Frame(rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);
}
//-----------------------------------------------------

INT16U txSFP_Inventry( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT16U len=0;
	INT8U totPkt = 1, curPkt = 1;
	INT8U optic, updn, port_num, cpri;
		
	for(optic = _100M; optic < MAX_Optic; optic++)
	{
		for(updn = 0; updn < 2; updn++)
		{
			port_num = 2*optic+updn; // port_num
			
			/*			
				port_num 0 = SFP0 100M 상위 = CPRI_0
				port_num 1 = SFP1 100M 하위 = CPRI_2
				port_num 2 = SFP0 +α  상위 = CPRI_1
				port_num 3 = SFP1 +α  하위 = CPRI_3
			*/
			     if(port_num == 1) cpri = CPRI_2;
			else if(port_num == 2) cpri = CPRI_1;
			else                   cpri = port_num;
			
			txf->data[len++] = port_num;			
			txf->data[len++] = Is_SFP_Inserted(cpri); // SFP 설치 유무 : 0(미설치), 1(설치)
			txf->data[len++] = RHU_Inventory[cpri + INV_IDX_SFP_CPRI_0].inv.supply; // 공급사
						
			memset(&txf->data[len], 0, 5); len+=5; // reserved, 5byte

			// 2020.07.22
			memcpy(&txf->data[len], m_St.d_SFP_Vendor_Name[cpri], 16); len+=16;
			memcpy(&txf->data[len], m_St.d_SFP_Vendor___PN[cpri], 16); len+=16;
			memcpy(&txf->data[len], m_St.d_SFP_Vendor___SN[cpri], 16); len+=16;
			memcpy(&txf->data[len], m_St.d_SFP_Date___Code[cpri],  8); len+=8;
		}	
	}
	return Make_Frame(rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);
}
//-----------------------------------------------------
INT16U txBarCode( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT16U len=0;
	INT8U ascii_len;
	INT8U totPkt = 1, curPkt = 1;
	
	if(rxf->len > 0) // Bar-Code Control
	{
		Wr_BarCode( rxf->data );
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}	
	//------------------------------
	txf->data[len++] = m_St.Maker[0];
	txf->data[len++] = m_St.Maker[1];	
#ifdef __TYPE__PRU__
	txf->data[len++] = m_St.Type[0];
	txf->data[len++] = Get_Repeater_Type();
#else
	txf->data[len++] = m_St.Type[0];
	txf->data[len++] = m_St.Type[1]; 
#endif	
	//------------------------------
	// barCode 
	ascii_len = m_Eep.BarCode_len;
	if(ascii_len > BarCode_Name_MAX) 
		ascii_len = BarCode_Name_MAX;
	txf->data[len++] = ascii_len; // Length	
	memcpy(&txf->data[len], m_Eep.BarCode_Name, ascii_len); // barCode (ASCII) 
	len += ascii_len;
	//------------------------------
	// BTS Name(중계기 국소명) 
	ascii_len = m_Eep.Node_Name_len;
	if(ascii_len > Node_Name_MAX) ascii_len = Node_Name_MAX;
	txf->data[len++] = ascii_len; // len	
	memcpy(&txf->data[len], m_Eep.Node_Name, ascii_len); // BTS Name (ASCII)
	len += ascii_len;
	//------------------------------	
	return Make_Frame(rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);
}

INT16U tx_GUI_Log( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT16U len=0, size;
	INT8U totPkt = 1, curPkt = 1;
	INT8U type;
	INT8U index, count;
	INT8U log_type;
	

	/*
	type
	----------------------
		0 : Alarm log(Reset Log 포함)		
		1 : Control log

	index	
	----------------------
		0xFF  : 현재 저장된 log 개수 요청				
		1~100 : 저장된 log index (낮은 숫자: 제일 최근 발생한 log index)				
	*/
	
	// Request ------------------------------	
	type  = rxf->data[0];
	index = rxf->data[1];
	
	// Response ------------------------------
	len = 0; // init
	txf->data[len++] = type;

	switch(type)
	{
		case 0 : log_type = LOG_ALM; break;
		case 1 : log_type = LOG_CTL; break;
		default:
			return 0;
	}	
	
	if(index == 0xFF) 
	{
		count = Get_Pkt_from_log(log_type, &txf->data[len], 0xFF);
		txf->data[len++] = count;
	}
	else 
	{
		txf->data[len++] = index;
		size = Get_Pkt_from_log(log_type, &txf->data[len], index);
		len += size;
	}
	
	//------------------------------	
	return Make_Frame(rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt);
}

INT16U rx_LocalTime( PROTOCOL_FRAME* rxf )
{
	INT8U rx_time[6];

	if(rxf->len != 6) // 2019.10.18 : length must 6
	{
		print_agent (DEBUG_REMS, VT_BOLD_RED, "[rx_LocalTime] len(%d) != 6, skip", rxf->len);
		return 0;
	}

	memcpy(rx_time, rxf->data, 6);
	
	if(Change_Local_Time(rx_time))
	{
		print_agent (DEBUG_REMS, VT_BOLD_YELLOW, "[gREMS] rx LocalTime %d.%d.%d %d:%d:%d", 
			2000+rx_time[0], 
			rx_time[1], rx_time[2],
			rx_time[3], rx_time[4], rx_time[5]);
			
		memcpy(m_St.Ru_Sys_Date, rx_time, 6);
		Clr__gREMS_local_Time_Request();
	}	
	else
	{
		print_agent (DEBUG_REMS, VT_BOLD_RED, "[gREMS] rx LocalTime Fail %d.%d.%d %d:%d:%d", 
			2000+rx_time[0], 
			rx_time[1], rx_time[2],
			rx_time[3], rx_time[4], rx_time[5]);
	}	
	return 0;
}


INT16U tx_EMS_Loopback(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf)
{
	INT8U totPkt = 1, curPkt = 1;
	
	memcpy(txf->data, rxf->data, rxf->len);
	return Make_Frame(rxf->Type, rxf->Rcode, txf, rxf->len, rxf->seqNo16, totPkt, curPkt );
}
	

INT16U rx_GUI_ID_PW(PROTOCOL_FRAME* rxf)
{
	INT8U size;
	INT8U* pStr;

	if(rxf->len != 22) // 2019.10.18
	{
		print_agent (DEBUG_REMS, VT_BOLD_RED, "[rx_GUI_ID_PW] len(%d) != 22, skip", rxf->len);
		return 0;
	}

	size = 11; // ID
	pStr = &rxf->data[0];
	if(memcmp(m_Eep.User_ID, pStr, size)) // if different
	{
		memset(m_Eep.User_ID, 0, 12);
		memcpy(m_Eep.User_ID, pStr, size);
		
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}

	size = 11; // PW
	pStr = &rxf->data[11];
	if(memcmp(m_Eep.User_PW, pStr, size)) // if different
	{
		memset(m_Eep.User_PW, 0, 12);
		memcpy(m_Eep.User_PW, pStr, size);
		
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}


	Clr__gREMS_GUI_ID_PW_Request();
	
	return 0;
}
//---------------------------------------------------------------------------------------------				






//---------------------------------------------------------------------------------------------				
#if 0 // later used when 5G-AAU/PRU
//------------------------------------------------------------------


//==========================================================================
// return 1, if DTU and All Amp are in the idle state.
INT8U Is_All_Idle(void)
{
	INT8U path4;

	if(m_St.DL_State[DN_RCU][DL_ST] != DL_St_IDLE) return 0;
	if(m_St.DL_State[DN_DTU][DL_ST] != DL_St_IDLE) return 0;
	if(m_St.DL_State[DN_FPGA][DL_ST]!= DL_St_IDLE) return 0;
	if(m_St.DL_State[DN_PSU][DL_ST] != DL_St_IDLE) return 0;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use( path4 ))
		{
			if(m_St.AMP_DL_St[path4][DL_ST] != DL_St_IDLE) return 0;
		}
	}
	
	return 1;
}
//---------------------------------------------------------------
// return 1, if DTU or any Amp is in the XXXX state.
INT8U Is_DL_DTU_or_AMP(INT8U dl_xx_st)
{
	INT8U path4;

	if(m_St.DL_State[DN_RCU][DL_ST] == dl_xx_st) return 1;
	if(m_St.DL_State[DN_DTU][DL_ST] == dl_xx_st) return 1;
	if(m_St.DL_State[DN_FPGA][DL_ST]== dl_xx_st) return 1;
	if(m_St.DL_State[DN_PSU][DL_ST] == dl_xx_st) return 1;
		
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use( path4 ))
		{				
			if(m_St.AMP_DL_St[path4][DL_ST] == dl_xx_st) return 1;
		}
	}	
	return 0;
}

void Proc_FW_Update(void) 
{
	INT32U timeout;
	INT32U time_elapse;
	INT8U ack, ret, cnt;
	INT8S path4;
	
	//------------------------------------------------------
		
	if(m_fw_update.forward_cmd_amp_dtu)
	{	
		if(Is_AMP_MultiDown()) return;
		if(Is_PSU_MultiDown()) return;
		if(Is_Dig_MultiDown()) return;
			
		// 1st : All AMPs
		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		{				
			if( Check_PAU_Use( path4 ) )
			{
				cnt = 0;
				do
				{
					cnt++;
					ret = PAU_FW_APPLY( path4, m_fw_update.data, m_fw_update.len, &ack );
				} while ((ret == 0) && (cnt < 3));
			}
		}

		// 2nd : PSU						
		cnt = 0;
		do
		{
			cnt++;
			ret = PSU_FW_APPLY( m_fw_update.data, m_fw_update.len, &ack );
		} while ((ret == 0) && (cnt < 3));				
						

		// 3rd : DIG
		Send_DIG_Command( CMD_FW_UPDATE, m_fw_update.data, m_fw_update.len );		
		
		m_fw_update.forward_cmd_amp_dtu = 0;
	}
	//------------------------------------------------------
	if(m_fw_update.cmd_req)
	{
		if(m_Env.DnMode.Rcu != _DN_RST_WAIT_)
		{
			m_fw_update.cmd_req = 0; // timer 도중에 즉시 다운로드의 경우.
		}
		else if(m_St.DL_State[DN_RCU][DL_ST] == DL_St_Rst_Wait)
		{
			if(m_fw_update.dly_min > MAX_FW_DLY_TIME)
			{
				m_fw_update.dly_min = MAX_FW_DLY_TIME;
			}

			timeout = m_fw_update.dly_min * TICK_CO_MIN;
			time_elapse = OSTimerGet_msec(&m_fw_update.timer);

			if( time_elapse >= timeout )
			{			
				m_St.DL_State[DN_RCU][DL_CNT] = 0;
			
				m_fw_update.cmd_req = 0;
				D_Printf(DEBUG_SYSTEM, "RCU_download_Rusume.\r\n");
				RCU_download_Rusume();
			}
			else
			{
				m_St.DL_State[DN_RCU][DL_CNT] = (timeout - time_elapse)/1000;
			}
		}
	}
}

// 적용 or 취소
INT16U tx_FW_Update( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf )
{
	INT8U NACK; // nack if 1
	INT8U direct_NACK_RSP = 0; // 1 : DTU, AMP로 FW 적용 메시지를 보내지 않고, 상위장비로 NACK 응답
	INT8U totPkt = 1, curPkt = 1;

	D_Printf(DEBUG_SYSTEM, "Rx tx_FW_Update : (%d)\r\n", rxf->data[0]);

	//-------------------------------------------
	// RCPU : download 후 즉시 reset 이면...
	//-------------------------------------------
	if(m_Env.DnMode.Rcu != _DN_RST_WAIT_)
	{
		m_fw_update.cmd_req = 0;
		OSTimerReset( &m_fw_update.timer);
		m_St.DL_State[DN_RCU][DL_ST] = DL_St_IDLE;
		m_St.DL_State[DN_RCU][DL_CNT] = 0;

		D_Printf(DEBUG_SYSTEM, " [Down] _DN_RST_AUTO \r\n");
	}
	//-------------------------------------------


	//-------------------------------------------
	m_fw_update.cancel = rxf->data[0];

	if ( m_fw_update.cancel ) // cancel
	{
		// 모두 idle 이면, DTU/AMP에 command 전달하지 않고, 바로 NACK 보냄.
		if( Is_All_Idle() ) direct_NACK_RSP = 1;
		else				direct_NACK_RSP = 0;

		if(direct_NACK_RSP == 0)
		{
			// RCPU
			switch(m_St.DL_State[DN_RCU][DL_ST])
			{
				case DL_St_IDLE:
				case DL_St_Now:
					m_fw_update.cmd_req = 0;
					break;

				case DL_St_Fail:
				case DL_St_Complete:
				case DL_St_Rst_Wait:
					m_fw_update.cmd_req = 0;
					m_St.DL_State[DN_RCU][DL_ST] = DL_St_IDLE;
					m_St.DL_State[DN_RCU][DL_CNT] = 0;
					break;
			}
		}
	}
	else // FW 적용
	{
		// DTU와 모든 AMP의 D/L State를 확인
		// 아래의 경우, DTU, AMP로 FW 적용 메시지를 보내지 않고, 상위장비로 NACK 응답
		// 1. DTU, AMP 모두 idle
		// 2. DTU, AMP 하나라도 D/L fail or Reset 준비 중 ( 어떤 것은 D/L 완료, Reset 준비 중)
			 if( Is_All_Idle() )					direct_NACK_RSP = 1;
		else if( Is_DL_DTU_or_AMP(DL_St_Fail) ) 	direct_NACK_RSP = 1;
		else if( Is_DL_DTU_or_AMP(DL_St_Rst_Wait) ) direct_NACK_RSP = 1;
		else										direct_NACK_RSP = 0;


		// RCU
		if(direct_NACK_RSP == 0)
		{
			D_Printf(DEBUG_SYSTEM, "[iRo]  RCU DL_State : %d ", m_St.DL_State[DN_RCU][DL_ST]);
			
			switch(m_St.DL_State[DN_RCU][DL_ST])
			{
				case DL_St_Fail: ////
					m_fw_update.cmd_req = 0;
					OSTimerReset( &m_fw_update.timer);  // 시간 카운트 초기화
					break;

				case DL_St_Rst_Wait:
					break;

				case DL_St_Complete:
					m_St.DL_State[DN_RCU][DL_ST] = DL_St_Rst_Wait;
					
					m_St.DL_State[DN_RCU][DL_CNT] = 0;
					
					m_fw_update.cmd_req = 1;
					OSTimerReset( &m_fw_update.timer);  // 시간 카운트 초기화
					m_fw_update.dly_min = rxf->data[1];// 지연 요청 값 갱신.

					// D_Printf(DEBUG_SYSTEM, "tx_FW_Update: cance(%d), dly_min(%d).\r\n", m_fw_update.cancel, m_fw_update.dly_min);
					
					break;

				case DL_St_IDLE:
				case DL_St_Now:
					m_fw_update.cmd_req = 0;
					OSTimerReset( &m_fw_update.timer);  // 시간 카운트 초기화
					break;
			}
		}
	}
	//------------------------------------


	//------------------------------------
	if(direct_NACK_RSP)
	{
		NACK = 1; // NACK 응답.
		m_fw_update.forward_cmd_amp_dtu = 0;  // DTU/AMP에 command 전달 X.

		D_Printf(DEBUG_SYSTEM, "[iRo]  tx_FW_Update() : direct_NACK_RSP.");
	}
	else
	{
		NACK = 0; // ACK 응답.
		m_fw_update.forward_cmd_amp_dtu = 1; // DTU/AMP에 command 전달 O.
		memcpy( m_fw_update.data, rxf->data, 2);
		m_fw_update.len = rxf->len;

		if (m_fw_update.len > 3) m_fw_update.len = 3;
	}

	txf->data[0] = NACK;
	
	return Make_Frame( rxf->Type, rxf->Rcode, txf, 1, rxf->seqNo16, totPkt, curPkt );
}
//-----------------------------------------------------
#endif	
//-----------------------------------------------------


/* EOF */

