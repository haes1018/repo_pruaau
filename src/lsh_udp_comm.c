/////////////////////////////
//	LSH_UDP_COMM.C
/////////////////////////////
#define LSH_UDP_COMM_GLOBALS
#include "includes.h"
/////////////////////////////

static pthread_mutex_t	LSH_COMM_Evt;
////static pthread_mutex_t	LSH_Ctrl_Evt;



static LSH_PROTOCOL_FRAME	lsh_comm_txf;
static LSH_PROTOCOL_FRAME	lsh_comm_rxf;


//===================================================================================================
//rececive
static INT8U LSH_STX[3] = {0x10, 0x00, 0x00}; // 2019.01.11.
static void rsStartCheck_LSH( COMM_QUE* pque, LSH_PROTOCOL_FRAME* pframe )//0
{
	INT8U *p = (INT8U *)pframe; // STX - 3 byte
	
	p[pque->len++] = pque->data[pque->rear];

	if(p[0] == LSH_STX[0])
	{
		//D_Printf( DEBUG_LS_Comm, "[rsStartCheck_LSH] 1st %02x\n", p[0]);
		
		if( pque->len >= MAX_LSH_SYNC_SIZE ) //3
		{
			//D_Printf( DEBUG_LS_Comm, "[rsStartCheck_LSH] 2nd %02x\n", p[1]);
			
			if(p[1] == LSH_STX[1])
			{
				//D_Printf( DEBUG_LS_Comm, "[rsStartCheck_LSH] 3rd %02x\n", p[2]);
				
				if(p[2] == LSH_STX[2]) 
				{
					//D_Printf( DEBUG_LS_Comm, "[rsStartCheck_LSH] go command 1\n");
					
					pque->len = 0;
					pque->comm_mode = 1;
					pque->data_recv = 0;
				}
				else pque->len = 0;
			}
			else pque->len = 0;				
		}
	}
	else
	{
		pque->len = 0;
	}
}

static void rsHeaderCheck_LSH( COMM_QUE* pque, LSH_PROTOCOL_FRAME* pframe )//1
{
	INT8U *p = (INT8U *)&pframe->Type; // Command ~ SeqNum - 9 byte
	INT8U err = 0;

	p[pque->len++] = pque->data[pque->rear];

	//D_Printf( DEBUG_LS_Comm, "[rsHeaderCheck_LSH] len %02x, Data %02x\n", pque->len, p[pque->len]);

	if( pque->len >= MAX_LSH_HEADER_SIZE ) // 9
	{
#if 0
		for(INT8U i = 0; i < MAX_LSH_HEADER_SIZE; i++ )
		{
			D_Printf( DEBUG_LS_Comm, VT_CYAN "%d : %02x\n", i, p[i]);
		}
#endif

		if( err )
		{
			pque->comm_mode = 0;  

			//D_Printf( DEBUG_LS_Comm, VT_Color_Err "[rsHeaderCheck_LSH] error! pque->len %d\n", pque->len );
		}
		else
		{
			//D_Printf( DEBUG_LS_Comm, "[rsHeaderCheck_LSH] pframe->len %d\n", pframe->len );
			pframe->len = BYTE_to_INT32B((INT8U *) &p[1]);

			//no ie data
			if( (MAX_LSH_SYNC_SIZE+ MAX_LSH_HEADER_SIZE == pframe->len) )
			{	
				pque->data_recv = 1;
				pque->comm_mode = 5;
			}

			//D_Printf( DEBUG_LS_Comm, "[rsHeaderCheck_LSH] go 3! pframe->len %d\n", pframe->len );
			pque->comm_mode = 3;
		}		
		pque->len = 0;
	}
}

static void rsDataCheck_LSH( COMM_QUE* pque, LSH_PROTOCOL_FRAME* pframe )//3
{
	INT8U *p = (INT8U *)&pframe->data;
	
	p[pque->len++] = pque->data[pque->rear];

	//D_Printf( DEBUG_LS_Comm, "[rsDataCheck_LSH] pframe->len %d\n", pframe->len );

	if( pque->len + MAX_LSH_SYNC_SIZE+ MAX_LSH_HEADER_SIZE >= pframe->len )
	{	
		pque->data_recv = 1;
		pque->comm_mode = 5;
	}
}

INT8U GetFrame_LSH( COMM_QUE* pque, LSH_PROTOCOL_FRAME* pframe ) // 0:no data, 1:Data ?????, 2:Data ??? ???
{	
	RecvCommDataQue( pque->port );
	
	while( pque->front != pque->rear )
	{
		switch( pque->comm_mode )
		{
			case 0 : rsStartCheck_LSH( pque, pframe );  break;
			case 1 : rsHeaderCheck_LSH( pque, pframe ); break;
			case 3 : rsDataCheck_LSH( pque, pframe );   break;
				
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

			return 2;	// --> ??? ??? ??? ???
		}
	}
	if( pque->comm_mode ) return 1; // --> ??? ??? ??.

	return 0;	// --> ??? ??? ????, ??? ???..
}


//---------------------------------------------------------------------------------------------------
//Send
#if 1
INT16U Make_Frame_IE_Optic_Delay(INT8U * pframe, INT8U cpri_cnt)
{
	INT16U ie_len = 0;
	
	ST_IE_FRAME_EXT s_ie_frame;
	ST_Op_Delay_Value s_data[2];

	s_ie_frame.IE_Id.uD16 = IE_Optic_Delay;	// ID_Id
	ie_len = sizeof(ST_Op_Delay_Value) * cpri_cnt + MAX_LSH_IE_EXT_FRAME_SIZE;
	s_ie_frame.Item_Cnt.uD32 = htonl(cpri_cnt); // CPRI cnt
	s_ie_frame.IE_Len.uD16 = htons(ie_len); // ID_Len


	// 100M ---------------------------------------
	s_data[0].CPRI_Id.uD32 = htonl(0);		
	s_data[0].Toffset.uD32 = htonl(m_St.d_Toffset[_100M]);	
	if(Is_SFP_Inserted(CPRI_2)) 
	{
		s_data[0].OP_Delay.uD32 = htonl(m_St.d_RoundTrip_Dly[_100M]);		
		s_data[0].DL_Hopping_Delay.sD32 = htonl(m_St.d_TBdelay_DL[_100M]); // 2019.04.11
		s_data[0].UL_Hopping_Delay.sD32 = htonl(m_St.d_TBdelay_UL[_100M]); // 2019.04.11	
	}
	else
	{
		s_data[0].OP_Delay.uD32 = 0xFFFFFFFF;		
		s_data[0].DL_Hopping_Delay.sD32 = htonl(0x0000FFFF); // 2019.05.02
		s_data[0].UL_Hopping_Delay.sD32 = htonl(0x0000FFFF); // 2019.05.02
	}
	
	s_data[0].Tx_Delay.sD32 = htonl(m_Eep.LS_Delay_DL_nsec[_100M]); // 2019.04.30, value received from L/S
	s_data[0].Rx_Delay.sD32 = htonl(m_Eep.LS_Delay_UL_nsec[_100M]); // 2019.04.30, value received from L/S

	s_data[0].Tx_Delay_not_Ru_dly.sD32 = htonl(m_St.d_FPGA_Delay_ns_DL[_100M][0]); // 2019.05.10, Tx Delay (excluding RU proc.delay), 0~500000
	s_data[0].Rx_Delay_not_Ru_dly.sD32 = htonl(m_St.d_FPGA_Delay_ns_UL[_100M][0]); // 2019.05.10, Rx Delay (excluding RU proc.delay), 0~500000

	/*
	2019.09.24 : delay_set 단계가 지나지 않았으면 0으로 response
	*/
	if(m_St.RU_Init_Status < Init_St___Completed)
	{	
		s_data[0].Tx_Delay.sD32 = 0;
		s_data[0].Rx_Delay.sD32 = 0;

		s_data[0].Tx_Delay_not_Ru_dly.sD32 = 0;
		s_data[0].Rx_Delay_not_Ru_dly.sD32 = 0;
	}
	
	// Alpha---------------------------------------
	if(cpri_cnt == 2)
	{
		s_data[1].CPRI_Id.uD32 = htonl(1);
		s_data[1].Toffset.uD32 = htonl(m_St.d_Toffset[Alpha]);
		if(Is_SFP_Inserted(CPRI_3)) 
		{
			s_data[1].OP_Delay.uD32 = htonl(m_St.d_RoundTrip_Dly[Alpha]);
			s_data[1].DL_Hopping_Delay.sD32 = htonl(m_St.d_TBdelay_DL[Alpha]);
			s_data[1].UL_Hopping_Delay.sD32 = htonl(m_St.d_TBdelay_UL[Alpha]);
		}
		else
		{
			s_data[1].OP_Delay.uD32 = 0xFFFFFFFF;		
			s_data[1].DL_Hopping_Delay.sD32 = htonl(0x0000FFFF); // 2019.05.02
			s_data[1].UL_Hopping_Delay.sD32 = htonl(0x0000FFFF); // 2019.05.02
		}		
		s_data[1].Tx_Delay.sD32 = htonl(m_Eep.LS_Delay_DL_nsec[Alpha]); // 2019.04.30, value received from L/S
		s_data[1].Rx_Delay.sD32 = htonl(m_Eep.LS_Delay_UL_nsec[Alpha]); // 2019.04.30, value received from L/S
		
		s_data[1].Tx_Delay_not_Ru_dly.sD32 = htonl(m_St.d_FPGA_Delay_ns_DL[Alpha][0]); // 2019.05.10, Tx Delay (excluding RU proc.delay), 0~500000
		s_data[1].Rx_Delay_not_Ru_dly.sD32 = htonl(m_St.d_FPGA_Delay_ns_UL[Alpha][0]); // 2019.05.10, Rx Delay (excluding RU proc.delay), 0~500000

		
		/*
		2019.09.24 : delay_set 단계가 지나지 않았으면 0으로 response
		*/
		if(m_St.RU_Init_Status < Init_St___Completed)
		{	
			s_data[1].Tx_Delay.sD32 = 0;
			s_data[1].Rx_Delay.sD32 = 0;
		
			s_data[1].Tx_Delay_not_Ru_dly.sD32 = 0;
			s_data[1].Rx_Delay_not_Ru_dly.sD32 = 0;
		}
	}
	//---------------------------------------
	
	s_ie_frame.IF_Cnt.uD16 = htons(IF_OP_DELAY_VALUE_CNT); // if cnt		// 2019.05.03
	//if_len = ie_len - MAX_LSH_IE_EXT_FRAME_SIZE; // 2019.04.11
	//s_ie_frame.IF_Len.uD16 = htons(if_len); // if len
	s_ie_frame.IF_Len.uD16 = htons(sizeof(ST_Op_Delay_Value)); // if len

	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME_EXT));
	memcpy(pframe + sizeof(ST_IE_FRAME_EXT), (INT8U *) &s_data, sizeof(ST_Op_Delay_Value) * cpri_cnt);

	return (ie_len);	
}

INT16U Make_Frame_IE_Measured_Power(INT8U * pframe)
{
	//INT16U cnt_if = 0;
	INT16U ie_len = 0, if_len = 0;
	
	ST_IE_FRAME s_ie_frame;
	ST_Measured_Power s_data;


	s_ie_frame.IE_Id.uD16 = IE_Measured_Power;										//ie id
	ie_len = sizeof(ST_Measured_Power) + MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);											//ie len


	//imsi
	s_data.Path_A_TSSI_IQ_Level.uD32 = htonl(m_St.d_DL_TSSI_dBm_x10[_100M][DL_0]);
	s_data.Path_B_TSSI_IQ_Level.uD32 = htonl(m_St.d_DL_TSSI_dBm_x10[_100M][DL_1]);
	s_data.Path_C_TSSI_IQ_Level.uD32 = htonl(m_St.d_DL_TSSI_dBm_x10[_100M][DL_2]);
	s_data.Path_D_TSSI_IQ_Level.uD32 = htonl(m_St.d_DL_TSSI_dBm_x10[_100M][DL_3]);	

	s_data.Path_A_DL_Output_Power_Level.uD16 = htons(m_St.Fwd_Out_Pwr_x10[AMPU_4_1]);
	s_data.Path_B_DL_Output_Power_Level.uD16 = htons(m_St.Fwd_Out_Pwr_x10[AMPU_4_2]);
	s_data.Path_C_DL_Output_Power_Level.uD16 = htons(m_St.Fwd_Out_Pwr_x10[AMPU_4_3]);
	s_data.Path_D_DL_Output_Power_Level.uD16 = htons(m_St.Fwd_Out_Pwr_x10[AMPU_4_4]);	

	s_data.Path_A_RSSI_Level.uD16 = htons(m_St.d_UL_RSSI_dBm_x10[_100M][DL_0]);
	s_data.Path_B_RSSI_Level.uD16 = htons(m_St.d_UL_RSSI_dBm_x10[_100M][DL_1]);
	s_data.Path_C_RSSI_Level.uD16 = htons(m_St.d_UL_RSSI_dBm_x10[_100M][DL_2]);
	s_data.Path_D_RSSI_Level.uD16 = htons(m_St.d_UL_RSSI_dBm_x10[_100M][DL_3]);
	//
	
	s_ie_frame.IF_Cnt.uD16 = htons(IF_MEASURED_POWER_CNT);							//if cnt
	if_len = ie_len - MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IF_Len.uD16 = htons(if_len);											//if len


	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME));
	memcpy(pframe + sizeof(ST_IE_FRAME), (INT8U *) &s_data, (sizeof(ST_Measured_Power)));

	return (ie_len);
	
}

INT16U Make_Frame_IE_Time_Tick(INT8U * pframe)
{
	//INT16U cnt_if = 0;
	INT16U ie_len = 0, if_len = 0;
	
	ST_IE_FRAME s_ie_frame;
	ST_Time_Tick s_data;

	s_ie_frame.IE_Id.uD16 = IE_Time_Tick;											//ie id
	ie_len = sizeof(ST_Time_Tick) + MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);											//ie len
	
	s_data.Time_Tick.uD32 = htonl(m_St.UTC);
	s_data.Time_Tick_Offset.uD32 = htonl(m_Eep.Time_Tick_Offset);
		
	s_ie_frame.IF_Cnt.uD16 = htons(IF_TIME_TICK_CNT);								//if cnt
	if_len = ie_len - MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IF_Len.uD16 = htons(if_len);											//if len


	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME));
	memcpy(pframe + sizeof(ST_IE_FRAME), (INT8U *) &s_data, (sizeof(ST_Time_Tick)));

	return (ie_len);
	
}


INT16U Make_Frame_IE_System_Sts(INT8U * pframe)
{
	INT16U ie_len = 0, if_len = 0;
	
	ST_IE_FRAME s_ie_frame;
	ST_System_Sts s_data;


	s_ie_frame.IE_Id.uD16 = IE_System_Sts;											//ie id
	ie_len = sizeof(ST_System_Sts) + MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);											//ie len

	//imsi
	s_data.RU_Init_Sts = AufoConfigFlag.Init_Status;
	s_data.Cascade_Id = AufoConfigFlag.RE_Cascade_ID;;
	s_data.FW_Mode = 0x00;
	s_data.System_Temp.uD32 = htonl((INT32S)m_St.Temp_x10); // 2019.04.11
	s_data.IF_Version.uD32 = htonl(0x01010101);
	s_data.Supported_OFR_Start_Frequency.uD32 = htonl(0xFFFFFFFF);		// 2019.05.03		미사용시 ffffffff
	s_data.Supported_OFR_Bandwidth.uD32 = htonl(0xFFFFFFFF);			// 2019.05.03
	s_data.Supported_IBW_Bandwidth.uD32 = htonl(0xFFFFFFFF);			// 2019.05.03

	// Voltage is 0.1 step
	#ifdef __TYPE__PRU__	
	s_data.DC_In_Volt.uD32 = htonl(0xFFFFFFFF); // 2019.05.03 PRU only, PRU is 0xFFFFFFFF fixed.
	s_data.DC_Out_Volt.uD32 = htonl(0xFFFFFFFF);// 2019.05.03 PRU only, PRU is 0xFFFFFFFF fixed.
	#else // AAU	
	s_data.DC_In_Volt.uD32 = htonl(m_St.d_Volt_DTU*100);	 // -48V, DC Fail Check 
	s_data.DC_Out_Volt.uD32 = htonl(m_St.d_Volt_DTU_Sub[0]*100); // RCU input 5.6V, FAN Alarm Voltage
	#endif
	
	s_data.Tx_Filter_Center_Frequency.uD32 = htonl(0xFFFFFFFF);
	s_data.Rx_Filter_Center_Frequency.uD32 = htonl(0xFFFFFFFF);
	s_data.Tx_Filter_Bandwidth = htonl(0xFF);
	s_data.Rx_Filter_Bandwidth = htonl(0xFF);

	
	#ifdef __TYPE__PRU__	
	// 2019.05.25
	if(m_St.Batt_Insert == 0) // not insert
		s_data.Battery_Equipment_Status.uD16 = htons(2); // 2 : not insert
	else // insert
		s_data.Battery_Equipment_Status.uD16 = htons(1); // 1 : insert
	#else // AAU
	// 2019.05.25
	s_data.Battery_Equipment_Status.uD16 = htons(0);
	#endif
	
	s_data.FAN_Equipment_Status.uD32 = htonl(1); // fix with 1, 2019.05.02

	s_data.FAN1_Speed.uD16 = htonl(0xFFFF);	//htons((INT16U)m_PSU.Fan_Speed), // 2019.05.03
	s_data.FAN2_Speed.uD16 = htonl(0xFFFF);	//htons((INT16U)m_PSU.Fan_Speed), // 2019.05.03

	s_data.FAN3_Speed_Reserved.uD16 = 0; // 2019.04.11
	s_data.FAN4_Speed_Reserved.uD16 = 0; // 2019.04.11

	s_data.Reserved1.uD32 = 0; // 2019.04.11


	#if 0
	s_data.Cascade_Delay.uD16 = htons(0xffff);
	s_data.Toffset.uD16 = htons(600);
	
	s_data.TBdelatDL.uD32 = htonl(0xffff);
	s_data.TBdelatUL.uD32 = htonl(0xffff);
	#endif
	//
	
	s_ie_frame.IF_Cnt.uD16 = htons(IF_SYSTEN_STAUS_CNT);								//if cnt
	if_len = ie_len - MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IF_Len.uD16 = htons(if_len);											//if len


	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME));
	memcpy(pframe + sizeof(ST_IE_FRAME), (INT8U *) &s_data, (sizeof(ST_System_Sts)));

	return (ie_len);
	
}

INT16U Make_Frame_IE_Alarm_Data_Vaild_Indicator(INT8U * pframe)
{
	INT16U ie_len = 0, if_len = 0;
	
	ST_IE_FRAME s_ie_frame;
	ST_ALM_Data_Vaild_Indicator s_data;


	s_ie_frame.IE_Id.uD16 = IE_ALM_Data_Vaild_Indicator;											//ie id
	ie_len = sizeof(ST_ALM_Data_Vaild_Indicator) + MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);											//ie len

	s_data.Vaild_Indicator.uD32 = htonl(AufoConfigFlag.Vaild_Indicator);
	
	s_ie_frame.IF_Cnt.uD16 = htons(IF_ALM_DATA_VALID_CNT);								//if cnt
	if_len = ie_len - MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IF_Len.uD16 = htons(if_len);											//if len


	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME));
	memcpy(pframe + sizeof(ST_IE_FRAME), (INT8U *) &s_data, (sizeof(ST_ALM_Data_Vaild_Indicator)));

	return (ie_len);
	
}

INT16U Make_Frame_IE_Alarm(INT8U * pframe)
{
	INT16U ie_len = 0, cnt = 0, alm_cnt = 0; // 2019.04.11
	//INT16U  if_len = 0, 
	
	ST_IE_FRAME_EXT s_ie_frame;
	ST_ALM s_data[100];

	memset(s_data, 0, sizeof(ST_ALM)*100); // 2019.05.22
	/*
	2019.04.30 : 
		Report Alarm Only
		That is, cleared-alarm is not reported...
		------------------------	
		Alarm Code
		Alarm Subcode Type
		Alarm Subcode
		Alarm Severity
		Alarm Flag
		Additional Information
		------------------------

		SPURIOUS_DETECTED
CPRI_FAILURE
Clock_FAILURE

	"UDA Id - 
	0: 역방향 불요파 검출 - 
	1: CPRI 다발 발생 - 
	2: Clock Fail 다발 발생"

	*/
	//-------------------------------------------------
	// UDA
	//-------------------------------------------------
	if(m_St.Last_Alm.UDA.Spurious)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_UDA);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_UDA);
		s_data[cnt].Alm_Subcode.uD32  = 0; // 0: 역방향 불요파 검출
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Warnig); // 2019.05.02
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x54, 0x40, 0x00, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}		
	if(m_St.Last_Alm.UDA.PD_CPRI)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_UDA);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_UDA);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000); // 1: CPRI 다발 발생 - 
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Warnig); // 2019.05.02
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x54, 0x43, 0x00, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.UDA.Clock)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_UDA);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_UDA);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000); // 2: Clock Fail 다발 발생"
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Warnig); // 2019.05.02
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x54, 0x42, 0x00, 0x00, 1, s_data[cnt].Add_Information);

		cnt++;
		alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	// Fan
	//-------------------------------------------------
	if(m_St.Last_Alm.A_0.FAN1_Case || m_St.Last_Alm.A_0.FAN2_PSU)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_FAN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_RUnRAU);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		if(m_St.Last_Alm.A_0.FAN1_Case)
			Make_Alm_Str(0x50, 0x14, 0x00, 0x00, 1, s_data[cnt].Add_Information);
		else if(m_St.Last_Alm.A_0.FAN2_PSU)
			Make_Alm_Str(0x50, 0x15, 0x00, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	

	//-------------------------------------------------
	// Temp High : 2019.05.01
	//-------------------------------------------------
	if(m_St.Last_Alm.A_0.Temp_Up)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_TEMPERATURE_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_RUnRAU);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x50, 0x13, 0x00, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++;
	}

	/*	
	// 2019.05.09 : Voltage High/Low 미지원으로 변경
	// VOLTAGE_HIGH		
	// VOLTAGE_LOW
	*/
	
	//-------------------------------------------------
	// CLOCK_FAIL
	//-------------------------------------------------
	if(m_St.Last_Alm.A_4.Clock) // 2019.05.02
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CLOCK_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_RUnRAU);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x00, 0x00, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	// CPRI_FAIL
	//-------------------------------------------------
	if(m_St.Last_Alm.A_2.CPRI_0__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x01, 0, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.A_2.CPRI_0_a)	
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x01, 0, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.A_2.CPRI_1__)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x01, 1, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.A_2.CPRI_1_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x01, 1, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	
/*
// 2020.07.01 ------------------------------------------------
	if(m_St.Last_Alm.A_3.PD_Pwr_U_0__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x0C, 0, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.A_3.PD_Pwr_U_0_a)	
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x0C, 0, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.A_4.PD_Pwr_U_1__)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x0C, 1, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.A_4.PD_Pwr_U_1_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x0C, 1, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	// 2020.07.01 ------------------------------------------------
	if(m_St.Last_Alm.A_4.PD_Pwr_L_0__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x09, 0, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.A_4.PD_Pwr_L_0_a)	
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x09, 0, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.A_4.PD_Pwr_L_1__)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x09, 1, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.A_4.PD_Pwr_L_1_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x09, 1, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
*/
	//-------------------------------------------------
	// E_ALM_DISABLED
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].PAU.Disabled)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DISABLED);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x17, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.Path[1].PAU.Disabled)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DISABLED);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x17, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.Path[2].PAU.Disabled)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DISABLED);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x17, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.Path[3].PAU.Disabled)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DISABLED);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x17, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	// E_ALM_LOW_GAIN
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].PAU.Dev_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_LOW_GAIN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x12, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[1].PAU.Dev_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_LOW_GAIN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x12, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[2].PAU.Dev_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_LOW_GAIN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x12, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[3].PAU.Dev_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_LOW_GAIN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x12, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	// E_ALM_OVERPOWER
	//-------------------------------------------------		
	if(m_St.Last_Alm.Path[0].PAU.Over_Power)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OVERPOWER);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x10, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[1].PAU.Over_Power)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OVERPOWER);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x10, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[2].PAU.Over_Power)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OVERPOWER);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x10, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[3].PAU.Over_Power)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OVERPOWER);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x10, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	

	//-------------------------------------------------
	// E_ALM_VSWR_FAIL
	//-------------------------------------------------		
	#ifdef __TYPE__PRU__
	if(m_St.Last_Alm.Path[0].PAU.VSWR) // PRU Only
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_VSWR_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x11, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	#endif
	#ifdef __TYPE__PRU__
	if(m_St.Last_Alm.Path[1].PAU.VSWR) // PRU Only
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_VSWR_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x11, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	#endif
	#ifdef __TYPE__PRU__
	if(m_St.Last_Alm.Path[2].PAU.VSWR) // PRU Only
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_VSWR_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x11, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	#endif
	#ifdef __TYPE__PRU__
	if(m_St.Last_Alm.Path[3].PAU.VSWR) // PRU Only
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_VSWR_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x11, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	#endif
	
	//-------------------------------------------------		
	// E_ALM_SHUTDOWN
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].PAU.Shutdown)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_SHUTDOWN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = 0;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x15, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}

	if(m_St.Last_Alm.Path[1].PAU.Shutdown)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_SHUTDOWN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x15, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}

	if(m_St.Last_Alm.Path[2].PAU.Shutdown)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_SHUTDOWN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x15, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}

	if(m_St.Last_Alm.Path[3].PAU.Shutdown)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_SHUTDOWN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x15, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	// E_ALM_DIGITE_ALM_INPUT_HIGH
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].DL.TSSI_Upper__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = 0x00000000;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x01, PATH_0, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[1].DL.TSSI_Upper__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x01, PATH_1, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[2].DL.TSSI_Upper__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x01, PATH_2, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[3].DL.TSSI_Upper__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x01, PATH_3, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].DL.TSSI_Upper_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x01, PATH_0, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[1].DL.TSSI_Upper_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x01, PATH_1, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[2].DL.TSSI_Upper_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32 = htonl(0x02010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32 = htonl(0);
		Make_Alm_Str(0x51, 0x01, PATH_2, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.Path[3].DL.TSSI_Upper_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32 = htonl(0x03010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32 = htonl(0);
		Make_Alm_Str(0x51, 0x01, PATH_3, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//-------------------------------------------------
	// E_ALM_RX_PATH_FAIL
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].UL.LNA_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = 0x00000000;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x12, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//----------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{
		// if(IS_Alpha_Mhz_Exist(0) && m_St.AlmSt.Path[0].UL.LNA_Fail) // 2019.05.07 : 운용중, VSS BW info 참조.
		if(m_St.Last_Alm.Path[0].UL.LNA_Fail_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x00010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x12, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}
	//----------------------------------
	if(m_St.Last_Alm.Path[1].UL.LNA_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x12, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//----------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{
		// if(IS_Alpha_Mhz_Exist(1) && m_St.AlmSt.Path[1].UL.LNA_Fail) // 2019.05.07 : 운용중, VSS BW info 참조.
		if(m_St.Last_Alm.Path[1].UL.LNA_Fail_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x01010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x12, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}
	//----------------------------------
	if(m_St.Last_Alm.Path[2].UL.LNA_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x12, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//----------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{
		// if(IS_Alpha_Mhz_Exist(2) && m_St.AlmSt.Path[2].UL.LNA_Fail) // 2019.05.07 : 운용중, VSS BW info 참조.
		if(m_St.Last_Alm.Path[2].UL.LNA_Fail_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x02010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x12, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}
	//----------------------------------
	if(m_St.Last_Alm.Path[3].UL.LNA_Fail)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x12, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//----------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{
		// if(IS_Alpha_Mhz_Exist(3) && m_St.AlmSt.Path[3].UL.LNA_Fail) // 2019.05.07 : 운용중, VSS BW info 참조.
		if(m_St.Last_Alm.Path[3].UL.LNA_Fail_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x03010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x12, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}
	//-------------------------------------------------
	// E_ALM_DIGITE_ALM_INPUT_LOW
	//-------------------------------------------------
	if(m_St.Last_Alm.Path[0].DL.TSSI_Lower__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = 0x00000000;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02, PATH_0, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[1].DL.TSSI_Lower__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02,  PATH_1, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[2].DL.TSSI_Lower__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02,  PATH_2, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[3].DL.TSSI_Lower__)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02, PATH_3, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}	
	//--------------------------------------------------------
	if(m_St.Last_Alm.Path[0].DL.TSSI_Lower_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02, PATH_0, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[1].DL.TSSI_Lower_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02, PATH_1, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[2].DL.TSSI_Lower_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02, PATH_2, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	if(m_St.Last_Alm.Path[3].DL.TSSI_Lower_a)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03010000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x51, 0x02, PATH_3, Alpha, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11
	}
	//--------------------------------------------------------
	// E_ALM_RSSI_HIGH
	//--------------------------------------------------------	
	if(m_St.Last_Alm.Path[0].UL.Over_Input)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = 0x00000000;
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x10, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//------------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{
		if(m_St.Last_Alm.Path[0].UL.Over_Input_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x00010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x10, PATH_0, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}
	//------------------------------------
	if(m_St.Last_Alm.Path[1].UL.Over_Input)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x10, PATH_1, 0x00, 1, s_data[cnt].Add_Information);
		
		cnt++; alm_cnt++; // 2019.04.11		
	}
	//------------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{		
		if(m_St.Last_Alm.Path[1].UL.Over_Input_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x01010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x10, PATH_1, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}
	//------------------------------------
	if(m_St.Last_Alm.Path[2].UL.Over_Input)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x10, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//------------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{		
		if(m_St.Last_Alm.Path[2].UL.Over_Input_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x02010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x10, PATH_2, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}	
	//------------------------------------
	if(m_St.Last_Alm.Path[3].UL.Over_Input)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x52, 0x10, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++; // 2019.04.11		
	}
	//------------------------------------
	if(Is_SFP_Inserted(CPRI_1)) // if Alpha-SFP is inserted
	{		
		if(m_St.Last_Alm.Path[3].UL.Over_Input_a)
		{
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(0x03010000);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
			Make_Alm_Str(0x52, 0x10, PATH_3, 0x00, 1, s_data[cnt].Add_Information);

			cnt++; alm_cnt++; // 2019.04.11
		}
	}	
	 
 	//--------------------------------------------------------
	// 2019.05.17
	/*	
	---------------------------------------------------------------
		OPTIC_TRANSCEIVER_RX_LOS
	---------------------------------------------------------------	
	*/	
	if(m_St.Last_Alm.A_2.LOS_SFP_0__) // 2019.05.17. LOS(SFP0)	  (OPTIC_TRANSCEIVER_RX_LOS)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x02, PATH_0, _100M, 1, s_data[cnt].Add_Information);
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_2.LOS_SFP_0_a)	// 2019.05.17. LOS(SFP0)(+α) (OPTIC_TRANSCEIVER_RX_LOS)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x02, PATH_0, Alpha, 1, s_data[cnt].Add_Information);
		 
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_2.LOS_SFP_1__)	// 2019.05.17. LOS(SFP1)	  (OPTIC_TRANSCEIVER_RX_LOS)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x02, PATH_1, _100M, 1, s_data[cnt].Add_Information);
		
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_2.LOS_SFP_1_a)  // 2019.05.17. LOS(SFP1)(+α) (OPTIC_TRANSCEIVER_RX_LOS)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x02, PATH_1, Alpha, 1, s_data[cnt].Add_Information);
		
		cnt++; alm_cnt++;
	}	

	/*
	---------------------------------------------------------------
		OPTIC_TRANSCEIVER_TX_FAULT
	---------------------------------------------------------------	
	*/
	if(m_St.Last_Alm.A_3.LD_SFP_0__)	// 2019.05.17. LD(SFP0)	     (OPTIC_TRANSCEIVER_TX_FAULT)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x04, PATH_0, _100M, 1, s_data[cnt].Add_Information);
		
		cnt++; alm_cnt++;
	}			
	if(m_St.Last_Alm.A_3.LD_SFP_0_a)	// 2019.05.17. LD(SFP0)(+α) (OPTIC_TRANSCEIVER_TX_FAULT)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x04, PATH_0, Alpha, 1, s_data[cnt].Add_Information);
		
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_3.LD_SFP_1__)	// 2019.05.17. LD(SFP1)	     (OPTIC_TRANSCEIVER_TX_FAULT)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x04, PATH_1, _100M, 1, s_data[cnt].Add_Information);

		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_3.LD_SFP_1_a)	// 2019.05.17. LD(SFP1)(+α) (OPTIC_TRANSCEIVER_TX_FAULT)
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(1); // 0x01 : Alarm
		Make_Alm_Str(0x53, 0x04, PATH_1, Alpha, 1, s_data[cnt].Add_Information);
		
		cnt++; alm_cnt++;
	}
	//--------------------------------------------------------	
	
	s_ie_frame.IE_Id.uD16 = IE_ALM;										//ie id
	ie_len = (sizeof(ST_ALM) * cnt) + MAX_LSH_IE_EXT_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);								//ie len
	
	s_ie_frame.Item_Cnt.uD32 = htonl(alm_cnt);					        // if cnt // 2019.04.11
	s_ie_frame.IF_Cnt.uD16 = htons(6);				// 2019.05.03		//if cnt // 2019.04.11		 
	s_ie_frame.IF_Len.uD16 = htons(sizeof(ST_ALM));	// 2019.05.03

	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME_EXT));	 
	memcpy(pframe + sizeof(ST_IE_FRAME_EXT), (INT8U *) &s_data, (sizeof(ST_ALM) * alm_cnt)); // 2019.05.02

	return (ie_len);
}

#define TEST_STR_LEN (25)
INT16U Make_Frame_IE_Alarm_Change(INT8U * pframe)
{
	INT8U i = 0;
	INT16U ie_len = 0,  cnt = 0, alm_cnt = 0;
	// INT16U if_len = 0;
	
	ST_IE_FRAME_EXT s_ie_frame;
	ST_ALM s_data[100];
	ST_SubCode sub_code; // 2019.04.18
	char str[TEST_STR_LEN];

	memset(&sub_code.data, 0, 4); // 2019.04.18
	memset(s_data, 0, sizeof(ST_ALM)*100); // 2019.05.22

	// always "cnt == alm_cnt" why alm_cnt ??

	//Temp
	if(m_St.Last_Alm.A_0.Temp_Up != AufoConfigFlag.Alm_Prev.A_0.Temp_Up)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_TEMPERATURE_HIGH);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_RUnRAU);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.A_0.Temp_Up);
		Make_Alm_Str(0x50, 0x13, 0x00, 0x00, m_St.Last_Alm.A_0.Temp_Up, s_data[cnt].Add_Information);

		AufoConfigFlag.Alm_Prev.A_0.Temp_Up = m_St.Last_Alm.A_0.Temp_Up;

		cnt++;
		alm_cnt++;
	}

	// Fan
	if((m_St.Last_Alm.A_0.FAN1_Case != AufoConfigFlag.Alm_Prev.A_0.FAN1_Case)
		|| (m_St.Last_Alm.A_0.FAN2_PSU != AufoConfigFlag.Alm_Prev.A_0.FAN2_PSU))
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_FAN);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_RUnRAU);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.A_0.FAN1_Case);
		
		if(m_St.Last_Alm.A_0.FAN1_Case != AufoConfigFlag.Alm_Prev.A_0.FAN1_Case)
			Make_Alm_Str(0x50, 0x14, 0x00, 0x00, m_St.Last_Alm.A_0.FAN1_Case, s_data[cnt].Add_Information);
		else if(m_St.Last_Alm.A_0.FAN2_PSU != AufoConfigFlag.Alm_Prev.A_0.FAN2_PSU)
			Make_Alm_Str(0x50, 0x15, 0x00, 0x00, m_St.Last_Alm.A_0.FAN2_PSU, s_data[cnt].Add_Information);

		AufoConfigFlag.Alm_Prev.A_0.FAN1_Case = m_St.Last_Alm.A_0.FAN1_Case;
		AufoConfigFlag.Alm_Prev.A_0.FAN2_PSU = m_St.Last_Alm.A_0.FAN2_PSU;
		cnt++;
		alm_cnt++;
	}

	// Clock
	if(m_St.Last_Alm.A_4.Clock != AufoConfigFlag.Alm_Prev.A_4.Clock)
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CLOCK_FAIL);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_RUnRAU);
		s_data[cnt].Alm_Subcode.uD32 = htonl(0);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
		s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.A_4.Clock);
		
		Make_Alm_Str(0x53, 0x00, 0x00, 0x00, m_St.Last_Alm.A_4.Clock, s_data[cnt].Add_Information);
		AufoConfigFlag.Alm_Prev.A_4.Clock = m_St.Last_Alm.A_4.Clock;

		cnt++;
		alm_cnt++;
	}

	//-------------------------------------------------
	// UDA
	//-------------------------------------------------
	if(m_St.Last_Alm.UDA.Spurious != AufoConfigFlag.Alm_Prev.UDA.Spurious) // 2019.05.07. Added
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_UDA);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_UDA);
		s_data[cnt].Alm_Subcode.uD32  = 0; // 0: 역방향 불요파 검출
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Warnig); // 2019.05.02
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.UDA.Spurious);
		Make_Alm_Str(0x54, 0x40, 0x00, 0x00, m_St.Last_Alm.UDA.Spurious, s_data[cnt].Add_Information);
		
		AufoConfigFlag.Alm_Prev.UDA.Spurious = m_St.Last_Alm.UDA.Spurious;
		
		cnt++; alm_cnt++; // 2019.04.11
	}		
	if(m_St.Last_Alm.UDA.PD_CPRI != AufoConfigFlag.Alm_Prev.UDA.PD_CPRI) // 2019.05.07. Added
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_UDA);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_UDA);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000); // 1: CPRI 다발 발생 - 
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Warnig); // 2019.05.02
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.UDA.PD_CPRI);
		Make_Alm_Str(0x54, 0x43, 0x00, 0x00, m_St.Last_Alm.UDA.PD_CPRI, s_data[cnt].Add_Information);

		AufoConfigFlag.Alm_Prev.UDA.PD_CPRI = m_St.Last_Alm.UDA.PD_CPRI;
		cnt++; alm_cnt++; // 2019.04.11
	}	
	if(m_St.Last_Alm.UDA.Clock != AufoConfigFlag.Alm_Prev.UDA.Clock) // 2019.05.07. Added
	{
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_UDA);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_UDA);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000); // 2: Clock Fail 다발 발생"
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Warnig); // 2019.05.02
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.UDA.Clock);
		Make_Alm_Str(0x54, 0x42, 0x00, 0x00, m_St.Last_Alm.UDA.Clock, s_data[cnt].Add_Information);
		
		AufoConfigFlag.Alm_Prev.UDA.Clock = m_St.Last_Alm.UDA.Clock;
		
		cnt++;
		alm_cnt++; // 2019.04.11
	}

	for( i = 0; i < 4; i++ )
	{
		// TSS Upper
		if(m_St.Last_Alm.Path[i].DL.TSSI_Upper__ != AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Upper__)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = _100M; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_FAnSplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].DL.TSSI_Upper__);
			Make_Alm_Str(0x51, 0x01, i, _100M, m_St.Last_Alm.Path[i].DL.TSSI_Upper__, s_data[cnt].Add_Information);

			AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Upper__ = m_St.Last_Alm.Path[i].DL.TSSI_Upper__;

			cnt++;
			alm_cnt++;
		}

		if(m_St.Last_Alm.Path[i].DL.TSSI_Upper_a != AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Upper_a)
		{		
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = Alpha; 
			sub_code.data.uD8[3] = i;

			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_FAnSplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].DL.TSSI_Upper_a);
			Make_Alm_Str(0x51, 0x01, i, Alpha, m_St.Last_Alm.Path[i].DL.TSSI_Upper_a, s_data[cnt].Add_Information);

			AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Upper_a = m_St.Last_Alm.Path[i].DL.TSSI_Upper_a;

			cnt++;
			alm_cnt++;
		}

		//TSS Upper
		if(m_St.Last_Alm.Path[i].DL.TSSI_Lower__ != AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Lower__)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = _100M; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_FAnSplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].DL.TSSI_Lower__);
			Make_Alm_Str(0x51, 0x02, i, _100M, m_St.Last_Alm.Path[i].DL.TSSI_Lower__, s_data[cnt].Add_Information);

			AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Lower__ = m_St.Last_Alm.Path[i].DL.TSSI_Lower__;

			cnt++;
			alm_cnt++;
		}
		if(m_St.Last_Alm.Path[i].DL.TSSI_Lower_a != AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Lower_a)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = Alpha; 
			sub_code.data.uD8[3] = i;			

			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DIGITE_ALM_INPUT_LOW); // 2019.05.07 fix
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_FAnSplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].DL.TSSI_Lower_a);
			Make_Alm_Str(0x51, 0x02, i, Alpha, m_St.Last_Alm.Path[i].DL.TSSI_Lower_a, s_data[cnt].Add_Information);

			AufoConfigFlag.Alm_Prev.Path[i].DL.TSSI_Lower_a = m_St.Last_Alm.Path[i].DL.TSSI_Lower_a;

			cnt++;
			alm_cnt++;
		}

		

		// CPRI
		if(BitRead(m_St.Last_Alm.A_2.Data, (0x07-i)) != BitRead(AufoConfigFlag.Alm_Prev.A_2.Data, (0x07-i)))
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_CPRI_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32 = htonl(BitRead(m_St.Last_Alm.A_2.Data, (0x07-i)));
			switch(i)
			{
				case 0: Make_Alm_Str(0x53, 0x01, 0, _100M, BitRead(m_St.Last_Alm.A_2.Data, (0x07-i)), s_data[cnt].Add_Information); break;
				case 1: Make_Alm_Str(0x53, 0x01, 0, Alpha, BitRead(m_St.Last_Alm.A_2.Data, (0x07-i)), s_data[cnt].Add_Information); break;
				case 2: Make_Alm_Str(0x53, 0x01, 1, _100M, BitRead(m_St.Last_Alm.A_2.Data, (0x07-i)), s_data[cnt].Add_Information); break;
				case 3: Make_Alm_Str(0x53, 0x01, 1, Alpha, BitRead(m_St.Last_Alm.A_2.Data, (0x07-i)), s_data[cnt].Add_Information); break;
			}

			if(BitRead(m_St.Last_Alm.A_2.Data, (0x07-i))) BitSet(AufoConfigFlag.Alm_Prev.A_2.Data, (0x07-i));
			else								  BitRst(AufoConfigFlag.Alm_Prev.A_2.Data, (0x07-i));				
			cnt++;
			alm_cnt++;
		}


		// Disable
		if(m_St.Last_Alm.Path[i].PAU.Disabled != AufoConfigFlag.Alm_Prev.Path[i].PAU.Disabled)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;			
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_DISABLED);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].PAU.Disabled);
			Make_Alm_Str(0x51, 0x17, i, 0x00, m_St.Last_Alm.Path[i].PAU.Disabled, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].PAU.Disabled = m_St.Last_Alm.Path[i].PAU.Disabled;
		
			cnt++;
			alm_cnt++;
		}
		
		// LOW_GAIN
		if(m_St.Last_Alm.Path[i].PAU.Dev_Fail != AufoConfigFlag.Alm_Prev.Path[i].PAU.Dev_Fail)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;			
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_LOW_GAIN);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].PAU.Dev_Fail);
			Make_Alm_Str(0x51, 0x12, i, 0x00, m_St.Last_Alm.Path[i].PAU.Dev_Fail, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].PAU.Dev_Fail = m_St.Last_Alm.Path[i].PAU.Dev_Fail;
		
			cnt++;
			alm_cnt++;
		}

		// OVERPOWER
		if(m_St.Last_Alm.Path[i].PAU.Over_Power != AufoConfigFlag.Alm_Prev.Path[i].PAU.Over_Power)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;			
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OVERPOWER);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].PAU.Over_Power);
			Make_Alm_Str(0x51, 0x10, i, 0x00, m_St.Last_Alm.Path[i].PAU.Over_Power, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].PAU.Over_Power = m_St.Last_Alm.Path[i].PAU.Over_Power;
		
			cnt++;
			alm_cnt++;
		}
		
		//VSWR
		#ifdef __TYPE__PRU__
		if(m_St.Last_Alm.Path[i].PAU.VSWR != AufoConfigFlag.Alm_Prev.Path[i].PAU.VSWR)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_VSWR_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].PAU.VSWR);
			Make_Alm_Str(0x51, 0x11, i, 0x00, m_St.Last_Alm.Path[i].PAU.VSWR, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].PAU.VSWR = m_St.Last_Alm.Path[i].PAU.VSWR;
		
			cnt++;
			alm_cnt++;
		}
		#endif
	
	// Shutdown	
		if(m_St.Last_Alm.Path[i].PAU.Shutdown != AufoConfigFlag.Alm_Prev.Path[i].PAU.Shutdown)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_SHUTDOWN);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_Path);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Major);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].PAU.Shutdown);
			Make_Alm_Str(0x51, 0x15, i, 0x00, m_St.Last_Alm.Path[i].PAU.Shutdown, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].PAU.Shutdown = m_St.Last_Alm.Path[i].PAU.Shutdown;
		
			cnt++;
			alm_cnt++;
		}
	
		

		// RX Fail
		if(m_St.Last_Alm.Path[i].UL.LNA_Fail != AufoConfigFlag.Alm_Prev.Path[i].UL.LNA_Fail)
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);					// 2019.05.08  SC_Path->SC_PathFASplit
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor); // 2019.05.08
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].UL.LNA_Fail);
			Make_Alm_Str(0x52, 0x12, i, 0x00, m_St.Last_Alm.Path[i].UL.LNA_Fail, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].UL.LNA_Fail = m_St.Last_Alm.Path[i].UL.LNA_Fail;
		
			cnt++;
			alm_cnt++;
		}		
		//-----------------------------------------
		// 	
		
		if(m_St.Last_Alm.Path[i].UL.Over_Input != AufoConfigFlag.Alm_Prev.Path[i].UL.Over_Input)		
		{
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0; 
			sub_code.data.uD8[2] = 0; 
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit);
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].UL.Over_Input);
			Make_Alm_Str(0x52, 0x10, i, 0x00, m_St.Last_Alm.Path[i].UL.Over_Input, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].UL.Over_Input = m_St.Last_Alm.Path[i].UL.Over_Input;
		
			cnt++;
			alm_cnt++;
		}
			
		
		// 2019.05.07 : 운용중, VSS BW info 참조.
		// if(IS_Alpha_Mhz_Exist(i) && (m_St.AlmSt.Path[i].UL.LNA_Fail != AufoConfigFlag.Alm_Prev.Path[i].UL.LNA_Fail)) 
		if(m_St.Last_Alm.Path[i].UL.LNA_Fail_a != AufoConfigFlag.Alm_Prev.Path[i].UL.LNA_Fail_a)
		{			
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0;
			sub_code.data.uD8[2] = 1; // alpha
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RX_PATH_FAIL);
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit); // 2019.05.08
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor); // 2019.05.08
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].UL.LNA_Fail_a);
			Make_Alm_Str(0x52, 0x12, i, 0x00, m_St.Last_Alm.Path[i].UL.LNA_Fail_a, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].UL.LNA_Fail_a = m_St.Last_Alm.Path[i].UL.LNA_Fail_a;
		
			cnt++;
			alm_cnt++;
		}
		// 2019.05.07 : 운용중, VSS BW info 참조.
		// if(IS_Alpha_Mhz_Exist(i) && (m_St.AlmSt.Path[i].UL.LNA_Fail != AufoConfigFlag.Alm_Prev.Path[i].UL.LNA_Fail))		
		if(m_St.Last_Alm.Path[i].UL.Over_Input_a != AufoConfigFlag.Alm_Prev.Path[i].UL.Over_Input_a)
		{			
			sub_code.data.uD8[0] = 0; 
			sub_code.data.uD8[1] = 0;
			sub_code.data.uD8[2] = 1; // alpha
			sub_code.data.uD8[3] = i;
		
			s_data[cnt].Alm_Code.uD16 = htons(E_ALM_RSSI_HIGH); // 2019.05.08
			s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_PathFASplit); // 2019.05.08
			s_data[cnt].Alm_Subcode.uD32  = htonl(sub_code.data.uD32);
			s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor); // 2019.05.08
			s_data[cnt].Alm_Flag.uD32 = htonl(m_St.Last_Alm.Path[i].UL.Over_Input_a);
			Make_Alm_Str(0x52, 0x10, i, 0x00, m_St.Last_Alm.Path[i].UL.Over_Input_a, s_data[cnt].Add_Information);
		
			AufoConfigFlag.Alm_Prev.Path[i].UL.Over_Input_a = m_St.Last_Alm.Path[i].UL.Over_Input_a;
		
			cnt++;
			alm_cnt++;
		}
		//-----------------------------------------
	}
	//--------------------------------------------------------
	// 2019.05.17
	/*	
	---------------------------------------------------------------
		OPTIC_TRANSCEIVER_RX_LOS
	---------------------------------------------------------------	
	*/
	// 2019.05.17. LOS(SFP0)	  (OPTIC_TRANSCEIVER_RX_LOS)
	if(m_St.Last_Alm.A_2.LOS_SFP_0__ != AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_0__) 
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_2.LOS_SFP_0__);
		Make_Alm_Str(0x53, 0x02, 0, _100M, m_St.Last_Alm.A_2.LOS_SFP_0__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LOS_SFP_0__(%d)", m_St.Last_Alm.A_2.LOS_SFP_0__);
			
		AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_0__ = m_St.Last_Alm.A_2.LOS_SFP_0__;
		cnt++; alm_cnt++;		
	}	
	
	// 2019.05.17. LOS(SFP0)(+α) (OPTIC_TRANSCEIVER_RX_LOS)	
	if(m_St.Last_Alm.A_2.LOS_SFP_0_a != AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_0_a)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_2.LOS_SFP_0_a);
		Make_Alm_Str(0x53, 0x02, 0, Alpha, m_St.Last_Alm.A_2.LOS_SFP_0_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LOS_SFP_0_a(%d)", m_St.Last_Alm.A_2.LOS_SFP_0_a);
		
		AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_0_a = m_St.Last_Alm.A_2.LOS_SFP_0_a;
		cnt++; alm_cnt++;
	}
	// 2019.05.17. LOS(SFP1)	  (OPTIC_TRANSCEIVER_RX_LOS)
	if(m_St.Last_Alm.A_2.LOS_SFP_1__ != AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_1__)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_2.LOS_SFP_1__);
		Make_Alm_Str(0x53, 0x02, 1, _100M, m_St.Last_Alm.A_2.LOS_SFP_1__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LOS_SFP_1__(%d)", m_St.Last_Alm.A_2.LOS_SFP_1__);
		
		AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_1__ = m_St.Last_Alm.A_2.LOS_SFP_1__;
		cnt++; alm_cnt++;
	}
	// 2019.05.17. LOS(SFP1)(+α) (OPTIC_TRANSCEIVER_RX_LOS)	
	if(m_St.Last_Alm.A_2.LOS_SFP_1_a != AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_1_a)  
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_2.LOS_SFP_1_a);
		Make_Alm_Str(0x53, 0x02, 1, Alpha, m_St.Last_Alm.A_2.LOS_SFP_1_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LOS_SFP_1_a(%d)", m_St.Last_Alm.A_2.LOS_SFP_1_a);
		
		AufoConfigFlag.Alm_Prev.A_2.LOS_SFP_1_a = m_St.Last_Alm.A_2.LOS_SFP_1_a;
		
		cnt++; alm_cnt++;
	}	
	/*
	---------------------------------------------------------------
		OPTIC_TRANSCEIVER_TX_FAULT
	---------------------------------------------------------------	
	*/
	// 2019.05.17. LD(SFP0)	     (OPTIC_TRANSCEIVER_TX_FAULT)
	if(m_St.Last_Alm.A_3.LD_SFP_0__ != AufoConfigFlag.Alm_Prev.A_3.LD_SFP_0__)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_3.LD_SFP_0__);
		Make_Alm_Str(0x53, 0x04, 0, _100M, m_St.Last_Alm.A_3.LD_SFP_0__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LD_SFP_0__(%d)", m_St.Last_Alm.A_3.LD_SFP_0__);
		
		AufoConfigFlag.Alm_Prev.A_3.LD_SFP_0__ = m_St.Last_Alm.A_3.LD_SFP_0__;
		cnt++; alm_cnt++;
	}
	// 2019.05.17. LD(SFP0)(+α) (OPTIC_TRANSCEIVER_TX_FAULT)	
	if(m_St.Last_Alm.A_3.LD_SFP_0_a != AufoConfigFlag.Alm_Prev.A_3.LD_SFP_0_a)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_3.LD_SFP_0_a);
		Make_Alm_Str(0x53, 0x04, 0, Alpha, m_St.Last_Alm.A_3.LD_SFP_0_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LD_SFP_0_a(%d)", m_St.Last_Alm.A_3.LD_SFP_0_a);
		
		AufoConfigFlag.Alm_Prev.A_3.LD_SFP_0_a = m_St.Last_Alm.A_3.LD_SFP_0_a;
		cnt++; alm_cnt++;
	}
	// 2019.05.17. LD(SFP1)	     (OPTIC_TRANSCEIVER_TX_FAULT)
	if(m_St.Last_Alm.A_3.LD_SFP_1__ != AufoConfigFlag.Alm_Prev.A_3.LD_SFP_1__)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_3.LD_SFP_1__);
		Make_Alm_Str(0x53, 0x04, 1, _100M, m_St.Last_Alm.A_3.LD_SFP_1__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LD_SFP_1__(%d)", m_St.Last_Alm.A_3.LD_SFP_1__);
		
		AufoConfigFlag.Alm_Prev.A_3.LD_SFP_1__ = m_St.Last_Alm.A_3.LD_SFP_1__;
		cnt++; alm_cnt++;
	}
	// 2019.05.17. LD(SFP1)(+α) (OPTIC_TRANSCEIVER_TX_FAULT)
	if(m_St.Last_Alm.A_3.LD_SFP_1_a != AufoConfigFlag.Alm_Prev.A_3.LD_SFP_1_a)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_TX_FAULT);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_3.LD_SFP_1_a);
		Make_Alm_Str(0x53, 0x04, 1, Alpha, m_St.Last_Alm.A_3.LD_SFP_1_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] LD_SFP_1_a(%d)", m_St.Last_Alm.A_3.LD_SFP_1_a);
		
		AufoConfigFlag.Alm_Prev.A_3.LD_SFP_1_a = m_St.Last_Alm.A_3.LD_SFP_1_a;
		cnt++; alm_cnt++;
	}

/*	
--------------------------------------------------------
	//--------------------------------------------------------
	// 2020.07.01
	if(m_St.Last_Alm.A_3.PD_Pwr_U_0__ != AufoConfigFlag.Alm_Prev.A_3.PD_Pwr_U_0__) 
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_3.PD_Pwr_U_0__);
		Make_Alm_Str(0x53, 0x0C, 0, _100M, m_St.Last_Alm.A_3.PD_Pwr_U_0__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_U_0__(%d)", m_St.Last_Alm.A_3.PD_Pwr_U_0__);
			
		AufoConfigFlag.Alm_Prev.A_3.PD_Pwr_U_0__ = m_St.Last_Alm.A_3.PD_Pwr_U_0__;
		cnt++; alm_cnt++;		
	}	
	if(m_St.Last_Alm.A_3.PD_Pwr_U_0_a != AufoConfigFlag.Alm_Prev.A_3.PD_Pwr_U_0_a)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_3.PD_Pwr_U_0_a);
		Make_Alm_Str(0x53, 0x0C, 0, Alpha, m_St.Last_Alm.A_3.PD_Pwr_U_0_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_U_0_a(%d)", m_St.Last_Alm.A_3.PD_Pwr_U_0_a);
		
		AufoConfigFlag.Alm_Prev.A_3.PD_Pwr_U_0_a = m_St.Last_Alm.A_3.PD_Pwr_U_0_a;
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_4.PD_Pwr_U_1__ != AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_U_1__)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_4.PD_Pwr_U_1__);
		Make_Alm_Str(0x53, 0x0C, 1, _100M, m_St.Last_Alm.A_4.PD_Pwr_U_1__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_U_1__(%d)", m_St.Last_Alm.A_4.PD_Pwr_U_1__);
		
		AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_U_1__ = m_St.Last_Alm.A_4.PD_Pwr_U_1__;
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_4.PD_Pwr_U_1_a != AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_U_1_a)  
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_4.PD_Pwr_U_1_a);
		Make_Alm_Str(0x53, 0x0C, 1, Alpha, m_St.Last_Alm.A_4.PD_Pwr_U_1_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_U_1_a(%d)", m_St.Last_Alm.A_4.PD_Pwr_U_1_a);
		
		AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_U_1_a = m_St.Last_Alm.A_4.PD_Pwr_U_1_a;
		
		cnt++; alm_cnt++;
	}
	//--------------------------------------------------------
	// 2020.07.01
	if(m_St.Last_Alm.A_4.PD_Pwr_L_0__ != AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_0__) 
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x00000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_4.PD_Pwr_L_0__);
		Make_Alm_Str(0x53, 0x09, 0, _100M, m_St.Last_Alm.A_4.PD_Pwr_L_0__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_L_0__(%d)", m_St.Last_Alm.A_4.PD_Pwr_L_0__);
			
		AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_0__ = m_St.Last_Alm.A_4.PD_Pwr_L_0__;
		cnt++; alm_cnt++;		
	}	
	if(m_St.Last_Alm.A_4.PD_Pwr_L_0_a != AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_0_a)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x01000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_4.PD_Pwr_L_0_a);
		Make_Alm_Str(0x53, 0x09, 0, Alpha, m_St.Last_Alm.A_4.PD_Pwr_L_0_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_L_0_a(%d)", m_St.Last_Alm.A_4.PD_Pwr_L_0_a);
		
		AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_0_a = m_St.Last_Alm.A_4.PD_Pwr_L_0_a;
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_4.PD_Pwr_L_1__ != AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_1__)	
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x02000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_4.PD_Pwr_L_1__);
		Make_Alm_Str(0x53, 0x09, 1, _100M, m_St.Last_Alm.A_4.PD_Pwr_L_1__, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_L_1__(%d)", m_St.Last_Alm.A_4.PD_Pwr_L_1__);
		
		AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_1__ = m_St.Last_Alm.A_4.PD_Pwr_L_1__;
		cnt++; alm_cnt++;
	}
	if(m_St.Last_Alm.A_4.PD_Pwr_L_1_a != AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_1_a)  
	{	
		s_data[cnt].Alm_Code.uD16 = htons(E_ALM_OPTIC_TRANSCEIVER_RX_LOS);
		s_data[cnt].Alm_Subcode_Type.uD16 = htons(SC_CPRI);
		s_data[cnt].Alm_Subcode.uD32  = htonl(0x03000000);
		s_data[cnt].Alm_Severity.uD32 = htonl(SV_Minor);
		s_data[cnt].Alm_Flag.uD32     = htonl(m_St.Last_Alm.A_4.PD_Pwr_L_1_a);
		Make_Alm_Str(0x53, 0x09, 1, Alpha, m_St.Last_Alm.A_4.PD_Pwr_L_1_a, s_data[cnt].Add_Information);

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] PD_Pwr_L_1_a(%d)", m_St.Last_Alm.A_4.PD_Pwr_L_1_a);
		
		AufoConfigFlag.Alm_Prev.A_4.PD_Pwr_L_1_a = m_St.Last_Alm.A_4.PD_Pwr_L_1_a;
		
		cnt++; alm_cnt++;
	}	
--------------------------------------------------------
*/
	
	//--------------------------------------------------------
	s_ie_frame.IE_Id.uD16 = IE_ALM; // ie id
	ie_len = (sizeof(ST_ALM) * cnt) + MAX_LSH_IE_EXT_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len); // ie len	
	s_ie_frame.Item_Cnt.uD32 = htonl(alm_cnt); // if cnt
	s_ie_frame.IF_Cnt.uD16 = htons(6); // if cnt
	s_ie_frame.IF_Len.uD16 = htons(sizeof(ST_ALM));	// 2019.05.03

	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME_EXT));	
	memcpy(pframe + sizeof(ST_IE_FRAME_EXT), (INT8U *) &s_data, (sizeof(ST_ALM) * cnt)); ////????

	if(ie_len <= MAX_LSH_IE_EXT_FRAME_SIZE) // ??????? 2019.05.02
		ie_len = 0; // ???? 2019.05.02
	

	if(alm_cnt > 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Make_LS_Alarm] ie_len(%d), cnt(%d)", ie_len, cnt);

		for(i = 0; i < alm_cnt; i++)
		{
			memset(str, 0, TEST_STR_LEN);
			memcpy(str, s_data[i].Add_Information, 20);
			print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[LS_Alm %d] %s", i, str);
		}
	}

	return (ie_len);	
}



INT16U Make_Frame_IE_Lower_Node_Id_Config(INT8U * pframe)
{
	INT16U ie_len = 0, if_len = 0;

	ST_IE_FRAME s_ie_frame;
	ST_Lower_Node_Id_Config s_data;


	s_ie_frame.IE_Id.uD16 = IE_Lower_Node_Id_Config;								//ie id
	ie_len = sizeof(ST_Lower_Node_Id_Config) + MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);											//ie len

	#if 1

	s_data.RU_Id.uD16 = htons(0);

	s_data.Cascade_Id.uD16 = htons(AufoConfigFlag.RE_Cascade_ID);
	
	s_data.Base_IP.uD8[0] = AufoConfigFlag.Base_Ip[0];
	s_data.Base_IP.uD8[1] = AufoConfigFlag.Base_Ip[1];
	s_data.Base_IP.uD8[2] = AufoConfigFlag.Base_Ip[2];
	s_data.Base_IP.uD8[3] = AufoConfigFlag.Base_Ip[3];

	memcpy(s_data.Seial_Number, AufoConfigFlag.LS_Regist_Num, SN_SIZE);
	
	s_data.Dummy.uD16 = htons(0);
	
	memcpy(s_data.RU_Board_Name, AufoConfigFlag.RU_Board_Name, BD_NANEMSIZE);
	#endif
	
	s_ie_frame.IF_Cnt.uD16 = htons(IF_LOWER_NODE_ID_CONFIG_CNT);

	if_len = ie_len - MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IF_Len.uD16 = htons(if_len);
	
	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME));
	memcpy(pframe + sizeof(ST_IE_FRAME), (INT8U *) &s_data, sizeof(ST_Lower_Node_Id_Config));

	return (ie_len);	
}


INT16U Make_Frame_IE_Initial_Information(INT8U * pframe)
{
	INT16U cnt_if = 0;
	INT16U ie_len = 0, if_len = 0;

	ST_IE_FRAME s_ie_frame;
	ST_Initial_Information s_data;


	s_ie_frame.IE_Id.uD16 = IE_Initial_Information;									//ie id
	ie_len = sizeof(ST_Initial_Information) + MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IE_Len.uD16 = htons(ie_len);											//ie len

		
	s_data.FW_Mode.uD16 = htons(0);
	cnt_if++;	

	s_data.Log_Avail.uD16 = htons(0);
	cnt_if++;	

	s_data.Cascade_Id.uD32 = htonl(AufoConfigFlag.RE_Cascade_ID);
	cnt_if++;	
	
	s_data.FW_Version.uD8[0] = 0; // 2019.04.30
	memcpy(&s_data.FW_Version.uD8[1], m_St.RCU_Ver, 3);
	cnt_if++;
	
	s_data.IF_Version.uD32 = htonl(0x01010101);
	cnt_if++;

	s_ie_frame.IF_Cnt.uD16 = htons(IF_INITIAL_INFORMATION_CNT);											//if cnt

	if_len = ie_len - MAX_LSH_IE_FRAME_SIZE;
	s_ie_frame.IF_Len.uD16 = htons(if_len);											//if len

	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME));
	memcpy(pframe + sizeof(ST_IE_FRAME), (INT8U *) &s_data, sizeof(ST_Initial_Information));
	
	return (ie_len);
}

// 2019.04.30, by haes
INT16U Make_Frame_IE_TSSI_RSSI(INT8U * pframe, INT8U cnt)
{
	INT8U fa, path, i;
	INT16U ie_len = 0;
	INT32U uVal32;
	
	ST_IE_FRAME_EXT s_ie_frame;
	ST_TSSI_RSSI_Value s_data[8]; // 2(100M/alpha) x 4(layer)

	s_ie_frame.IE_Id.uD16 = IE_TSSI_RSSI;	// ID_Id
	ie_len = (sizeof(ST_TSSI_RSSI_Value) * cnt) + MAX_LSH_IE_EXT_FRAME_SIZE;
	s_ie_frame.Item_Cnt.uD32 = htonl(cnt); // cnt				// 2019.05.03 : / 8
	s_ie_frame.IE_Len.uD16 = htons(ie_len); // ID_Len			// 2019.05.03 : / 8

	for(fa = 0; fa < MAX_Optic; fa++)
	{
		if((fa == 1)&&(cnt < 8)) continue;
		
		for(path = 0; path < MAX_PATH_4; path++)
		{
			i = fa * MAX_PATH_4 + path;
			//---------------------------
			s_data[i].FA_Id.uD16    = htons(fa);
			s_data[i].Split_Id.uD16 = 0; // fixed with 0
			s_data[i].Path_Id.uD16  = htons(path);
			s_data[i].RSSI_dBm.uD16 = htons(m_St.d_UL_RSSI_dBm_x10[fa][path]); // 0.1dB step
			//--------------------
			#ifdef __TYPE__PRU__
			uVal32 = htonl(m_St.d_DL_TSSI_dec[fa][path]);
			#else			
			uVal32 = htonl(m_St.d_DL_TSSI_dec_Pre_EQ[fa][path]); // 2019.08.23 : AAU, 
			#endif
			s_data[i].TSSI_IQ.uD32 = uVal32; // DL-TSSI-Decimal
			//--------------------
			s_data[i].RSSI_IQ.uD32  = htonl(m_St.d_UL_RSSI_dec[fa][path]);  // Decimal value
		}
	}
	
	s_ie_frame.IF_Cnt.uD16 = htons(IF_TSSI_RSSI_VALUE_CNT); // if cnt		// 2019.05.03
	////if_len = ie_len - MAX_LSH_IE_EXT_FRAME_SIZE; // 2019.04.11
	////s_ie_frame.IF_Len.uD16 = htons(if_len); // if len
	s_ie_frame.IF_Len.uD16 = htons(sizeof(ST_TSSI_RSSI_Value)); // if len					// 2019.05.3

	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME_EXT));
	memcpy(pframe + sizeof(ST_IE_FRAME_EXT), (INT8U *) &s_data, (sizeof(ST_TSSI_RSSI_Value)) * cnt);

	return (ie_len);	
}


// 2019.04.30, by haes
INT16U Make_Frame_IE_Path_Status(INT8U * pframe, INT8U cnt)
{
	INT8U path, i;
	INT16U ie_len = 0; ////, if_len = 0;
	
	ST_IE_FRAME_EXT s_ie_frame;
	ST_Path_Status_Value s_data[MAX_PATH_4];  //  4(layer)

	s_ie_frame.IE_Id.uD16 = IE_Path_Status;	// ID_Id
	ie_len = ((sizeof(ST_Path_Status_Value) * cnt) + MAX_LSH_IE_EXT_FRAME_SIZE);
	s_ie_frame.Item_Cnt.uD32 = htonl(cnt); //  cnt						// 2019.05.03 : / 4
	s_ie_frame.IE_Len.uD16 = htons(ie_len); // ID_Len					// 2019.05.03 : / 4

	//---------------------------------------
	// 2019.05.03 : count 8 --> 4	
	for(path = 0; path < MAX_PATH_4; path++)
	{
		i = path;
		//---------------------------
		s_data[i].Path_Id.uD16	   = htons(i);
		s_data[i].Ref_Sig_Num.uD16 = 0xFFFF; // 0xFFFF : not used
		s_data[i].Temp.uD16	       = htons(m_St.Temp_x10);
		s_data[i].Tx_RF_Pwr.uD16   = htons(m_St.Fwd_Out_Pwr_x10[path]);
		s_data[i].RU_Gain.uD16	   = htons(0xEFFF); // 미지원. 0 fixed				// 2019.05.03 : 0->htons(0xEFFF)
	#ifdef __TYPE__PRU__
		s_data[i].Ret_Loss.uD16	   = htons(m_St.Ret_Loss_x10[path]);	// PRU only
	#else
		s_data[i].Ret_Loss.uD16	   = htons(9999); // AAU Not Supported 		// 2019.05.03 0->htons(9999)
	#endif
	}
	
	//---------------------------------------
	////if_len = ie_len - MAX_LSH_IE_EXT_FRAME_SIZE; // 2019.04.11
	////s_ie_frame.IF_Len.uD16 = htons(if_len); // if len
	s_ie_frame.IF_Cnt.uD16 = htons(IF_Path_Status_VALUE_CNT); // 2019.05.03
	s_ie_frame.IF_Len.uD16 = htons(sizeof(ST_Path_Status_Value)); 	// 2019.05.03

	memcpy(pframe, (INT8U *) &s_ie_frame, sizeof(ST_IE_FRAME_EXT));
	memcpy(pframe + sizeof(ST_IE_FRAME_EXT), (INT8U *) &s_data, (sizeof(ST_Path_Status_Value)) * cnt);

	return (ie_len);	
}

INT16U Make_Frame_LSH(INT8U type, LSH_PROTOCOL_FRAME* pframe, INT8U*data, INT16U len, INT32U seqnum)
{
	INT16U tLen = 0;

	pframe->Type = type;			// message id

	tLen = MAX_LSH_SYNC_SIZE+MAX_LSH_HEADER_SIZE+len;

	//D_Printf( DEBUG_LS_Comm, "[Make_Frame_LSH] Len %d,  tLen %d, seqnum %d\n", len, tLen, seqnum);
		
	pframe->len = 0xffff0000 & htonl((INT32U)tLen);	
	pframe->SeqNum = htonl(seqnum);
	
	memcpy( pframe->data, data, len );

	return (tLen);
}

/*
----------------------------------------------------------------------
	"Send_LSH_UDP_Data()" 
	is called 
	in the "Send_FX_......()"
----------------------------------------------------------------------
*/



/*
----------------------------------------------------------------------

-------------------------
LS	  |   RU
-------------------------
0xXXXX -> 0x2000 
0xXXXX <- 0x2000 
//	H'10000010 System Status Request
//	H'10000028 Alarm information Request
//	H'10000034 Reset Request
//	H'1000005C Delay Setup Request
//	H'1000001C CPRI Status Request

-------------------------
LS	  |   RU
-------------------------
0x5662 <- 0x2000 : Notify
0x5662 -> 0x2000
//	H'1000 009A RU ID Acquisition Notification
//	H'1000 0002 Initialization Notification
//	H'1000 0098 C&M Path Switchover Notification

-------------------------
LS	  |   RU
-------------------------
0x5071 <- 0x2000 : Notify
0x5071 -> 0x2000
//	H'10000026 Alarm information Notification

----------------------------------------------------------------------
*/

INT16U Get_LS_PortNo(INT8U msgID_8)
{
	INT16U port16;
	
	switch(msgID_8)
	{		
		case FX_RUID_ACQU_NOTI:
		case FX_INIT_NOTI:
		case FX_CnM_SW_NOTI:
			port16 = LSH_Port_0x5662;
			break;

		case FX_ALM_INFO_NOTI:
			port16 = LSH_Port_0x5071;
			break;	

		default:
			port16 = 0;
			break;
	}
	return port16;
}

// 2019.05.07 : status response 중, alarm noti 의 경우, packet 오염 수정.

static LSH_PROTOCOL_FRAME LS_Comm_txf; // 2019.05.07.

INT8U Send_LSH_UDP_Data(INT8U type, INT8U*data, INT16U len, INT8U* ip, INT16U rx_port16, INT32U seqnum)
{
	INT16U port16;
		
	if(len > MAX_COM_BUFFER) return 0;	

	pthread_mutex_lock(&LSH_COMM_Evt); // 2019.07.20

	len = Make_Frame_LSH(type, (LSH_PROTOCOL_FRAME*)&LS_Comm_txf, data, len, seqnum);

	#ifdef __LS_TEST_Board_Name__
	memcpy(&LS_Comm_txf.data[len], AufoConfigFlag.RU_Board_Name, BD_NANEMSIZE);
	len += BD_NANEMSIZE;
	#endif

	port16 = Get_LS_PortNo(type);	
	if(port16 == 0) port16 = rx_port16;

	//print_agent( DEBUG_LS_Comm, VT_Color_TX, "[Send_LSH_UDP_Data] TX MSG ID (0x%02X), port16 (%d), IP 0x%X\n", 
	//	type, port16, *(INT32U*)ip);

	Send_LSH_Data( COMM_UDP_LS, ip, port16, (INT8U*)&LS_Comm_txf.Type, len );
		
	pthread_mutex_unlock(&LSH_COMM_Evt);

	return 1;	
}

//---------------------------------------------------------------------------------

//send noti RCU->LSH

INT8U Send_FX_SYSTEM_STS_RSP( INT8U rst, INT8U* lsh_ip, INT16U port16, INT32U seqnum)
{
	INT16U len;
	type32 result;

	result.uD32 = 0;
	result.uD8[3] = rst;	

	memcpy((INT8U *)&lsh_comm_txf.data, (INT8U *) &result, sizeof(result)); //tx

	len = sizeof(result);
	len += Make_Frame_IE_System_Sts((INT8U *)&lsh_comm_txf.data + len);
	len += Make_Frame_IE_Time_Tick((INT8U *)&lsh_comm_txf.data + len);
	// 2019.05.07 : Measured_Power_IE 삭제
	/////len += Make_Frame_IE_Measured_Power((INT8U *)&lsh_comm_txf.data + len);
	return Send_LSH_UDP_Data(FX_SYSTEM_STS_RSP, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, port16, seqnum);
}

//Common use noti, response.

INT8U Send_FX_ALM_INFO(INT8U type, INT8U* lsh_ip, INT16U port16, INT32U seqnum)
{
	INT16U len;
	type32 result; // 2019.04.30 : add return result with 0x0000
	
	result.uD32 = 0;

	memcpy((INT8U*)&lsh_comm_txf.data, (INT8U*) &result, sizeof(result));	 //tx
	len = sizeof(result);	

	// 2019.04.30 : add return result with 0x0000
	len += Make_Frame_IE_Alarm_Data_Vaild_Indicator((INT8U*)&lsh_comm_txf.data[len]);	
	len += Make_Frame_IE_Alarm((INT8U *)&lsh_comm_txf.data + len);
	return Send_LSH_UDP_Data(type, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, port16, seqnum);
}

INT8U Send_FX_RUID_ACQU_NOTI(INT8U* lsh_ip, INT32U seqnum)
{
	INT16U len;

	len = Make_Frame_IE_Lower_Node_Id_Config((INT8U *)&lsh_comm_txf.data);	 //tx
	return Send_LSH_UDP_Data(FX_RUID_ACQU_NOTI, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, 0, seqnum);
}

INT8U Send_FX_INIT_NOTI(INT8U* lsh_ip, INT32U seqnum)
{
	INT16U len;

	len = Make_Frame_IE_Initial_Information((INT8U *)&lsh_comm_txf.data);  //tx
	return Send_LSH_UDP_Data(FX_INIT_NOTI, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, 0, seqnum);
}

INT8U Send_FX_CnM_SW_NOTI(INT8U* lsh_ip, INT32U seqnum)
{
	return Send_LSH_UDP_Data(FX_CnM_SW_NOTI, NULL, 0, lsh_ip, 0, seqnum);	
}

/*
INT8U Send_FX_ALM_INFO_NOTI(INT8U* lsh_ip, INT32U seqnum)
{
	INT16U len, tmp;

	len = Make_Frame_IE_Alarm_Data_Vaild_Indicator((INT8U *)&lsh_comm_txf.data); //tx

	tmp = Make_Frame_IE_Alarm_Change((INT8U *)&lsh_comm_txf.data + len);

	if(tmp == 0)
	{
		return 0;
	}
	else
	{
		len += tmp;		
		return Send_LSH_UDP_Data(FX_ALM_INFO_NOTI, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, 0, seqnum);
	}
}
*/

// 2019.05.07 : status response 중, alarm noti 의 경우, packet 오염 수정.
#define LS_ALM_NOTI_Max_Len (4096)
static INT8U LS_Comm_Alarm_Data[LS_ALM_NOTI_Max_Len]; // name change for ALARM_ACK_Retry
INT8U Send_FX_ALM_INFO_NOTI(INT8U* lsh_ip, INT32U seqnum)
{
	INT16U len, tmp;

	len = Make_Frame_IE_Alarm_Data_Vaild_Indicator(LS_Comm_Alarm_Data); // tx

	tmp = Make_Frame_IE_Alarm_Change(&LS_Comm_Alarm_Data[len]);

	if(tmp == 0)
	{
		return 0;
	}
	else
	{
		len += tmp;
		if(m_Env.gREMS_Noti_Ready) // 2019.08.07
			return Send_LSH_UDP_Data(FX_ALM_INFO_NOTI, LS_Comm_Alarm_Data, len, lsh_ip, 0, seqnum);
		else
			return 0;
	}
}



INT8U Send_FX_ReturnResult(INT8U type, int rst, INT8U* lsh_ip, INT16U port16, INT32U seqnum)
{
	type32 result;

	result.sD32 = htonl(rst); // 2019.06.14, Delay 에러 응답시 byte order 부분 수정 	

	return Send_LSH_UDP_Data(type, (INT8U *) &result, sizeof(result), lsh_ip, port16, seqnum);	
}

INT8U Send_FX_CPRI_STS_RSP(INT8U type, INT8U cpri_cnt, INT8U* lsh_ip, INT16U port16, INT32U seqnum)
{
	INT16U len;
	type32 result;
	result.uD32 = 0;

	memcpy((INT8U *)&lsh_comm_txf.data, (INT8U *) &result, sizeof(result)); // tx
	
	len = sizeof(result);	

	print_agent( DEBUG_LS_Comm, VT_RED, "[Send_FX_CPRI_STS_RSP] len %d, cpri_cnt %d", len, cpri_cnt);
	if(cpri_cnt > 2) cpri_cnt = 2;

	len += Make_Frame_IE_Optic_Delay(&lsh_comm_txf.data[len], cpri_cnt);

	if(len > MAX_COM_BUFFER)
		print_agent( DEBUG_LS_Comm, VT_RED, "[Send_FX_CPRI_STS_RSP] Length ?? %d", len);
		
	return Send_LSH_UDP_Data(type, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, port16, seqnum);	
}


/*
-------------------------------------------------------------------------------------
	// 2019.04.30
	
	FA Status Response
-------------------------------------------------------------------------------------
*/
INT8U Send_FX_FA_STS_RSP(INT8U type, INT8U cnt, INT8U* lsh_ip, INT16U port16, INT32U seqnum)
{
	INT16U len; 
	type32 result;
	
	result.uD32 = 0;

	memcpy((INT8U *)&lsh_comm_txf.data, (INT8U *) &result, sizeof(result)); //tx
	
	len = sizeof(result);	
	len += Make_Frame_IE_TSSI_RSSI((INT8U *)&lsh_comm_txf.data[len], cnt);

	return Send_LSH_UDP_Data(type, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, port16, seqnum);
}

/*
-------------------------------------------------------------------------------------
	// 2019.04.30
	
	Path Status Response
-------------------------------------------------------------------------------------
*/

INT8U Send_FX_PATH_STS_RSP(INT8U type, INT8U cnt, INT8U* lsh_ip, INT16U port16, INT32U seqnum)
{
	INT16U len; 
	
	type32 result;
	result.uD32 = 0;

	memcpy((INT8U *)&lsh_comm_txf.data, (INT8U *) &result, sizeof(result)); //tx
	
	len = sizeof(result);	
	len += Make_Frame_IE_Path_Status((INT8U *)&lsh_comm_txf.data[len], cnt);
	
	return Send_LSH_UDP_Data(type, (INT8U *)&lsh_comm_txf.data, len, lsh_ip, port16, seqnum);
}





//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//
//recv
INT8U Recv_FX_SYSTEM_STS_REQ(LSH_PROTOCOL_FRAME* rxf)
{
	memcpy((INT8U *)&m_LSH_Sts.SystemTopology, &rxf->data[0], sizeof(ST_System_Topology));

	return 1;
}


INT8U Recv_FX_TIME_SYNC_REQ(LSH_PROTOCOL_FRAME* rxf)
{

	memcpy((INT8U *)&m_LSH_Sts.rTimeTick, &rxf->data[MAX_LSH_IE_FRAME_SIZE], sizeof(ST_Time_Tick));

	return 1;
}



int Recv_FX_DELAY_SET_REQ(LSH_PROTOCOL_FRAME* rxf)
{
	// 2019.04.18 : changed
	// 2019.05.08 : 'MAX_LSH_IE_DELAY_REQ_FRAME_SIZE' -> 'MAX_LSH_IE_EXT_FRAME_SIZE'
	/*
	2019.05.08
	Delay Value IE 항목 중  Number of CPRI Length 변경 (2byte-> 4byte)
	Delay Value IE의 Number of CPRI, Number of IF, Number of Len 정보 추가
	*/

	INT8U l;
	INT8U fa;	
	int delay_ns;
	INT8U fail_cnt = 0;
	INT8U num_of_cpri;

	INT32U LS_Delay_DL_nsec[MAX_Optic]; // Delay Setting Value from LayerSplitter 
	INT32U LS_Delay_UL_nsec[MAX_Optic]; // Delay Setting Value from LayerSplitter 

	INT32U LS_Delay_DL_nsec_prev[MAX_Optic]; // Delay Setting Value from LayerSplitter 
	INT32U LS_Delay_UL_nsec_prev[MAX_Optic]; // Delay Setting Value from LayerSplitter 

	//------------------------------------------------------------------
	ST_IE_OPTIC_DELAY_FRAME DlyValueIE;

	// 2019.07.04
	//---------------------
	memcpy((INT8U*)&DlyValueIE, rxf->data, MAX_LSH_IE_EXT_FRAME_SIZE); 
	num_of_cpri = ntohl(DlyValueIE.CPRI_Cnt.uD32);

	print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] IE_Id  0x%X --- ", ntohs(DlyValueIE.IE_Id.uD16) );
	print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] IE_Len 0x%X --- ", ntohs(DlyValueIE.IE_Len.uD16) );
	print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] num_cpri %d --- ", num_of_cpri );
	print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] IF_Cnt 0x%X --- ", ntohs(DlyValueIE.IF_Cnt.uD16) );
	print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] IF_Len 0x%X --- ", ntohs(DlyValueIE.IF_Len.uD16) );
	
	
	if((num_of_cpri > 2)||(num_of_cpri == 0))
	{
		print_agent( DEBUG_LS_Comm, VT_BOLD_RED, "[Recv_FX_DELAY_SET_REQ] !! num_of_cpri %d, return---", num_of_cpri);
		return -1;
	}
	//---------------------

	memcpy((INT8U *)&m_LSH_Sts.DelayValue, &rxf->data[MAX_LSH_IE_EXT_FRAME_SIZE], sizeof(ST_Delay_Value)*num_of_cpri); // 2019.05.08

	LS_Delay_DL_nsec[_100M] = ntohl(m_LSH_Sts.DelayValue[_100M].DL_Delay.sD32);
	LS_Delay_UL_nsec[_100M] = ntohl(m_LSH_Sts.DelayValue[_100M].UL_Delay.sD32);
	LS_Delay_DL_nsec[Alpha] = ntohl(m_LSH_Sts.DelayValue[Alpha].DL_Delay.sD32);
	LS_Delay_UL_nsec[Alpha] = ntohl(m_LSH_Sts.DelayValue[Alpha].UL_Delay.sD32);
	
	// 2019.08.23
	m_St.LS_Dly_Ctrl_ns[_100M][DL] = LS_Delay_DL_nsec[_100M];
	m_St.LS_Dly_Ctrl_ns[_100M][UL] = LS_Delay_UL_nsec[_100M];
	m_St.LS_Dly_Ctrl_ns[Alpha][DL] = LS_Delay_DL_nsec[Alpha];
	m_St.LS_Dly_Ctrl_ns[Alpha][UL] = LS_Delay_UL_nsec[Alpha];


	print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] DLY_100 0x%X(%d) 0x%X(%d) --- ", 
		LS_Delay_DL_nsec[_100M], LS_Delay_DL_nsec[_100M], LS_Delay_UL_nsec[_100M], LS_Delay_UL_nsec[_100M]);
	if(num_of_cpri > 1)
	{
		print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] DLY___A 0x%X(%d) 0x%X(%d) --- ", 
			LS_Delay_DL_nsec[Alpha], LS_Delay_DL_nsec[Alpha], LS_Delay_UL_nsec[Alpha], LS_Delay_UL_nsec[Alpha]);
	}

	Wr__Event_to_LogFile("[L/S Control] DELAY_SET_REQ : cpri num(%d), [100_M] %d, %d [Alpha] %d, %d", 
		num_of_cpri, 
		LS_Delay_DL_nsec[_100M], LS_Delay_UL_nsec[_100M], 
		LS_Delay_DL_nsec[Alpha], LS_Delay_UL_nsec[Alpha]);
	
	
	if(LS_Delay_DL_nsec[_100M] >= 120000) return 0;
	if(LS_Delay_UL_nsec[_100M] >= 120000) return 0;

	if(num_of_cpri > 1)
	{
		if(LS_Delay_DL_nsec[Alpha] >= 120000) return 0;
		if(LS_Delay_UL_nsec[Alpha] >= 120000) return 0;
	}

	// Cal Delay Set ----------------------------

	LS_Delay_DL_nsec_prev[_100M] = m_Eep.LS_Delay_DL_nsec[_100M];
	LS_Delay_UL_nsec_prev[_100M] = m_Eep.LS_Delay_UL_nsec[_100M];	
	LS_Delay_DL_nsec_prev[Alpha] = m_Eep.LS_Delay_DL_nsec[Alpha];
	LS_Delay_UL_nsec_prev[Alpha] = m_Eep.LS_Delay_UL_nsec[Alpha];

	m_Eep.LS_Delay_DL_nsec[_100M] = LS_Delay_DL_nsec[_100M];
	m_Eep.LS_Delay_UL_nsec[_100M] = LS_Delay_UL_nsec[_100M];	

	if(num_of_cpri > 1)
	{
		m_Eep.LS_Delay_DL_nsec[Alpha] = LS_Delay_DL_nsec[Alpha];
		m_Eep.LS_Delay_UL_nsec[Alpha] = LS_Delay_UL_nsec[Alpha];
	}
	
	for(fa=0; fa<num_of_cpri; fa++) // 2019.07.04
	{	
		// 2019.07.03
		//---------------------
		if(fa==Alpha)
		{
			if(IS_Alpha_Mhz_Existing()==0) continue;
		}
		//---------------------
		for(l=0; l<MAX_DL; l++)
		{
			delay_ns = Use_d_Delay_Ctrl(DL, fa, l, _Not_Disp_);
			if(delay_ns < 0) fail_cnt++;
		
			delay_ns = Use_d_Delay_Ctrl(UL, fa, l, _Not_Disp_);
			if(delay_ns < 0) fail_cnt++;
		}
	}

	if(fail_cnt == 0)
	{		
	#ifdef __DO_Not_Save_LS_DELAY__		
	#else
		m_Env.is_eep_write = EEP_WRITE_CODE;
	#endif
		return 1;
	}
	else
	{
		m_Eep.LS_Delay_DL_nsec[_100M] = LS_Delay_DL_nsec_prev[_100M];
		m_Eep.LS_Delay_UL_nsec[_100M] = LS_Delay_UL_nsec_prev[_100M];	
		m_Eep.LS_Delay_DL_nsec[Alpha] = LS_Delay_DL_nsec_prev[Alpha];
		m_Eep.LS_Delay_UL_nsec[Alpha] = LS_Delay_UL_nsec_prev[Alpha];

		print_agent( DEBUG_LS_Comm, VT_YELLOW, "[Recv_FX_DELAY_SET_REQ] fail cnt %d, return --- ", fail_cnt );
		
		return 0;
	}	
}






	


////////////////////////////////////////////////////////////////////////




INT8U Recv_FX_RUID_ACQU_NOTI_ACK(LSH_PROTOCOL_FRAME* rxf)
{	
	ST_Lower_Node_Id_Config temp_data;
	char str[100];
	INT16U IF_Len; // 2019.08.01	
	ST_IE_FRAME ID_ACQU_NOTI_ACK_IE; // 2019.08.01

	// 2019.08.01
	memcpy((INT8U*)&ID_ACQU_NOTI_ACK_IE, rxf->data, MAX_LSH_IE_FRAME_SIZE); 
	IF_Len = ntohs(ID_ACQU_NOTI_ACK_IE.IF_Len.uD16);
	
	/*
	* 2019.08.02
	* samsung has over 36 byte length, so limit the length, 36
	*/
	if(IF_Len > sizeof(ST_Lower_Node_Id_Config))
	{
		print_agent( DEBUG_LS_Comm, VT_BOLD_YELLOW, "[FX_RUID_ACQU_NOTI_ACK] IF_Len(%d) is Over %d", 
			IF_Len, sizeof(ST_Lower_Node_Id_Config));
			
		IF_Len = sizeof(ST_Lower_Node_Id_Config);
	}
	

	// 2019.08.01
	#if 1
	memset((INT8U *)&temp_data, 0, sizeof(ST_Lower_Node_Id_Config));
	memcpy((INT8U *)&temp_data, &rxf->data[MAX_LSH_IE_FRAME_SIZE], IF_Len);
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] IF_Len : %d", IF_Len );
	#else	
	memcpy((INT8U *)&temp_data, &rxf->data[MAX_LSH_IE_FRAME_SIZE], sizeof(ST_Lower_Node_Id_Config));
	#endif
	
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] Received Data." );
	
	//Cascad ID cehck.
	if(AufoConfigFlag.RE_Cascade_ID != temp_data.Cascade_Id.uD8[1]) 
	{
		print_agent( DEBUG_LS_Comm, VT_Color_Evt, "[FX_RUID_ACQU_NOTI_ACK] Cascade_ID %02x != %02x, Dismissed.", 
				AufoConfigFlag.RE_Cascade_ID, temp_data.Cascade_Id.uD8[1] );
		return 0;
	}

	memset(str, 0, 100);
	memcpy(str,(char*)temp_data.Seial_Number, SN_SIZE);
	if(strncmp((char*)temp_data.Seial_Number, (char*)AufoConfigFlag.LS_Regist_Num, SN_SIZE) != 0) 
	{
		print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] SerialNum mis-match : %s, Dismissed", str);
		return 0;
	}
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] SerialNum : %s, Match.", str);
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] Cascade_ID %02x, Match.", temp_data.Cascade_Id.uD8[1] );	
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] RU_ID : %d, Cascade_Id : %d", 
		Swap16(temp_data.RU_Id.uD16), Swap16(temp_data.Cascade_Id.uD16));	
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_RUID_ACQU_NOTI_ACK] Base_IP : %d.%d.%d.%d", 
		temp_data.Base_IP.uD8[0], temp_data.Base_IP.uD8[1], temp_data.Base_IP.uD8[2], temp_data.Base_IP.uD8[3]);

	memset(str, 0, 100);
	memcpy(str,(char*)temp_data.RU_Board_Name, 16);
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "RU_Board_Name : %s", str);

	AufoConfigFlag.RU_Id = temp_data.RU_Id.uD8[1];
	AufoConfigFlag.RU_Id_Alloc = 0x01; // 0x01 if RU_Id is Allocated. 2019.05.08

	Wr__Event_to_LogFile("[L/S RSP_ACK] RUID_ACQU_NOTI_ACK : RU_Id(%d), RU_Cascade_ID(%d)", 
		AufoConfigFlag.RU_Id, AufoConfigFlag.RE_Cascade_ID);
		
	return 1;
}

INT8U Recv_FX_INIT_NOTI_ACK(LSH_PROTOCOL_FRAME* rxf)
{
	ST_Initial_Information_Ack temp_data;
	INT16U IF_Len; // 2019.08.01	
	ST_IE_FRAME INIT_NOTI_ACK_IE; // 2019.08.01

	// 2019.08.01
	memcpy((INT8U*)&INIT_NOTI_ACK_IE, rxf->data, MAX_LSH_IE_FRAME_SIZE); 
	IF_Len = ntohs(INIT_NOTI_ACK_IE.IF_Len.uD16);
	
	/*
	* 2019.08.02
	* if has over byte length, then limit the length.
	*/
	if(IF_Len > sizeof(ST_Initial_Information_Ack))
	{
		print_agent( DEBUG_LS_Comm, VT_BOLD_YELLOW, "[FX_INIT_NOTI_ACK] IF_Len(%d) is Over %d", 
			IF_Len, sizeof(ST_Initial_Information_Ack));
			
		IF_Len = sizeof(ST_Initial_Information_Ack);
	}

	// 2019.08.01
	#if 1
	memset((INT8U *)&temp_data, 0, sizeof(ST_Initial_Information_Ack));
	memcpy((INT8U *)&temp_data, &rxf->data[MAX_LSH_IE_FRAME_SIZE], IF_Len);
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_INIT_NOTI_ACK] IF_Len : %d", IF_Len );
	#else
	memcpy((INT8U *)&temp_data, &rxf->data[MAX_LSH_IE_FRAME_SIZE], sizeof(ST_Initial_Information_Ack));
	#endif
	
	AufoConfigFlag.Init_Status = 1;		// Received
	AufoConfigFlag.Vaild_Indicator = 1; // it means "packet is received"
	AufoConfigFlag.Alarm_Acknowledge = temp_data.Alm_Ack.uD8[1];
	
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_INIT_NOTI_ACK] Received Data." );	
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_INIT_NOTI_ACK] Time_Tick   : Host-Order 0x%08X(%d)", 			
			ntohl(temp_data.Time_Tick.uD32), ntohl(temp_data.Time_Tick.uD32));
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_INIT_NOTI_ACK] System_Id   : %#x", Swap16(temp_data.System_Id.uD16) );
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_INIT_NOTI_ACK] Alm_Ack     : %d", Swap16(temp_data.Alm_Ack.uD16) );
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[FX_INIT_NOTI_ACK] Tick_Offset : %d", (INT32S)ntohl(temp_data.Time_Tick_Offset.sD32));

	// 2019.05.23
	m_Eep.Time_Tick_Offset = ntohl(temp_data.Time_Tick_Offset.sD32);
	m_Env.is_eep_write = EEP_WRITE_CODE;

	User_UTC_Set(ntohl(temp_data.Time_Tick.uD32));

	Wr__Event_to_LogFile("[L/S RSP_ACK] INIT_NOTI_ACK : Time_Tick(%d), Tick_Offset(%d)", 
		ntohl(temp_data.Time_Tick.uD32), m_Eep.Time_Tick_Offset);
		
	////// YCSO added 190725
	///// YCSO delete 190813
	//// Set_BT_Name();
	return 1;
}

INT8U Recv_System_Topology(LSH_PROTOCOL_FRAME* rxf)
{
	ST_System_Topology temp_data;
	
	memcpy((INT8U *)&temp_data, &rxf->data[MAX_LSH_IE_FRAME_SIZE], sizeof(ST_System_Topology));

	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[Recv_System_Topology] Received Data." );	
	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[Recv_System_Topology] Split_Hub_Status - Sts: %#x / Type : %#x", temp_data.Split_Hub_Status.Bit.Sts, temp_data.Split_Hub_Status.Bit.Type);

	for(INT8U i=0; i<16; i++)
	{
		print_agent( DEBUG_LS_Comm, VT_Color_RX, "[Recv_System_Topology] RE_BR[%d] - Sts: %#x / Type : %#x", i, temp_data.RE_BR[i].Bit.Sts, temp_data.RE_BR[i].Bit.Type);
	}
	return 1;
}

#endif
//---------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------
INT16U LSH_Data_Analysis( LSH_PROTOCOL_FRAME* rxf, LSH_PROTOCOL_FRAME* txf, INT8U* r_ip, INT16U port16 )
{
	INT8U cpri_num;
	int re, result;

	print_agent( DEBUG_LS_Comm, VT_Color_RX, "[LSH_Data_Analysis] Message-ID 0x%02X\n", rxf->Type);

	/*	
	-------------------------------
	2019.07.18 : 
	Initialzation Notification Acknowledge를 받기 전까지는 
	상태 요청 등에 대한 메시지에 대한 응답 X
	-------------------------------	
	*/
	switch(rxf->Type) // Message-ID
	{
		case FX_SYSTEM_STS_REQ:		
		case FX_ALM_INFO_ACK:
		case FX_ALM_INFO_REQ:
		case FX_RESET_REQ:
		case FX_DELAY_SET_REQ:
		case FX_TIME_SYNC_REQ:		
		case FX_CnM_SW_NOTI_ACK:
		case FX_CPRI_STS_REQ:
		case FX_FA_STS_REQ:		
		case FX_PATH_STS_REQ:
			if(AufoConfigFlag.Init_Status == 0)
			{
				print_agent(DEBUG_LS_Comm, VT_BOLD_YELLOW, "<No Init-Noti-ACK> Message-ID Dismissed ----- !!");
				return 0;
			}
			break;
	}
	//-------------------------------
	// 2019.08.24
	if(AufoConfigFlag.RU_Id_Alloc)
	{
		switch( rxf->Type ) // Message-ID
		{	
			case FX_RUID_ACQU_NOTI_ACK:	
			////////case FX_INIT_NOTI_ACK: // 2019.09.24 : delete, RU ID ACK 에 대해서만.
				print_agent(DEBUG_LS_Comm, VT_BOLD_MAGENTA, "[RX MSG ID] RU-ID Already Allocated.");
				print_agent(DEBUG_LS_Comm, VT_BOLD_MAGENTA, "[RX MSG ID] (0x%02X) Canceled.", rxf->Type);
				return 0; // 2019.09.24 : break -> return 0
		}
	}
	//-------------------------------	

	switch( rxf->Type ) // Message-ID
	{
		//-------------------------------
		case FX_RUID_ACQU_NOTI_ACK:			
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_RUID_ACQU_NOTI_ACK", rxf->Type);			
			memcpy((INT8U *)&AufoConfigFlag.Recv_Ip, r_ip, 4);			
			if(Recv_FX_RUID_ACQU_NOTI_ACK(rxf))
			{
				//Flag set : Auto_Config_Task -> AutoConfig_Proc -> Ip_Assign_Process
				AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_PROCESSING;
			}
			return 1;

		case FX_INIT_NOTI_ACK: 			
			/*
			2019.06.22 : RU ID Acqu' Noti  &  Ack  완료 전에 Initalization Noti Ack를 수신하지 않도록 구현.
			*/			
			if((AufoConfigFlag.Ip_Assign_Step == IP_ASSIGN_FINISH)&&(AufoConfigFlag.AutoConfig_Step == AUTUCONFIG_STEP05))
			{		
				print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_INIT_NOTI_ACK \n", rxf->Type);

				//Flag set : Auto_Config_Task -> AutoConfig_Proc -> INIT_NOTI_Process
				AufoConfigFlag.Init_Noti_Step = INIT_NOTI_PROCESSING;
				Recv_FX_INIT_NOTI_ACK(rxf);		
				return 1;
			}
			else return 0;
			
		case FX_SYSTEM_STS_REQ: 
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_SYSTEM_STS_REQ", rxf->Type);
			Recv_FX_SYSTEM_STS_REQ(rxf);
			Send_FX_SYSTEM_STS_RSP(0, r_ip, port16, rxf->SeqNum); // 2019.04.11
			return 1;

		case FX_ALM_INFO_REQ:
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_ALM_INFO_REQ \n", rxf->Type);
			Send_FX_ALM_INFO(FX_ALM_INFO_RSP, r_ip, port16, rxf->SeqNum);
			return 1;
		
		case FX_DELAY_SET_REQ: 
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_DELAY_SET_REQ", rxf->Type);

			/*
			2019.09.17 : RU-Init-Status(AID 0x500D)
				5/6 단계 아닐때 Delay 제어 받으면 
				에러 리턴(-101,Use of function is limited briefly)
			*/
			if(Get___RU_Initial_Status() < Init_St_Dly_Set_Req)
			{
				result = -101;
			}
			else
			{			
				re = Recv_FX_DELAY_SET_REQ(rxf);
				
				     if(re ==  0) result = -105;
				else if(re == -1) result = -502;
				else              result = 0;
			}

			m_St.LS_Dly_Ret32 = result;
			Wr__Event_to_LogFile("[L/S Control] DELAY_SET_REQ : result %d", result);	
			
			/*
			* 2019.08.09 : Delay 제어시 정상/비정상 케이스 log 출력 요청
			*/
			if(result == 0)
			{
				Set___RU_Initial_Status(Init_St___Completed);
				print_agent( DEBUG_LS_Comm, VT_BOLD_CYAN,  "[TX MSG ID] (0x%02X), FX_DELAY_SET_RSP(%d)", 
					FX_DELAY_SET_RSP, result);
			}
			else
			{
				print_agent( DEBUG_LS_Comm, VT_BOLD_YELLOW,"[TX MSG ID] (0x%02X), FX_DELAY_SET_RSP(%d)", 
					FX_DELAY_SET_RSP, result);				
			}
							
			Send_FX_ReturnResult(FX_DELAY_SET_RSP, result, r_ip, port16, rxf->SeqNum); // 2019.04.11
			return 1;
		
		#if 0	//close 190219
		//case FX_TIME_SYNC_REQ: 
		//	D_Printf( DEBUG_LS_Comm, VT_Color_RX "[RX MSG ID] (0x%02X), FX_TIME_SYNC_REQ \n", rxf->Type);
		//	Recv_FX_TIME_SYNC_REQ(rxf);
		//	Send_FX_ReturnResult(FX_TIME_SYNC_RSP, 1, r_ip, port16, rxf->SeqNum);		
		//	return 1;
		#endif
			
		case FX_RESET_REQ: 
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_RESET_REQ", rxf->Type);

			Send_FX_ReturnResult(FX_RESET_RSP, 0, r_ip, port16, rxf->SeqNum); // 2019.04.11

			OSTimeDly( OS_DELAY_100msec * 5 );
			sync();
			OSTimeDly( OS_DELAY_1sec * 3 );
			system("reboot");
			
			return 1;

		case FX_CPRI_STS_REQ:
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_CPRI_STS_REQ", rxf->Type);
			
			if(IS_Alpha_Mhz_Existing()) cpri_num = 2;
			else cpri_num = 1;
				
			Send_FX_CPRI_STS_RSP(FX_CPRI_STS_RSP, cpri_num, r_ip, port16, rxf->SeqNum); // Optic Delay IE의 Number of CPRI : 2

			if(m_St.RU_Init_Status < Init_St_Dly_Set_Req)
			{
				// 2019.08.28 : 'Init_St___Completed' 상태에서는 제어 되지 않도록.
				Set___RU_Initial_Status(Init_St_Dly_Set_Req);
			}
			return 1;

		case FX_CnM_SW_NOTI_ACK:
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_CnM_SW_NOTI_ACK", rxf->Type);
			Check_CnM_Path_Change_Clear();
			return 1; // 

		case FX_ALM_INFO_ACK:  // 2019.04.11
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_ALM_INFO_ACK", rxf->Type);
			return 1;

		// 2019.04.30
		case FX_FA_STS_REQ:
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_FA_STS_REQ", rxf->Type);
			
			if(IS_Alpha_Mhz_Existing()) cpri_num = 2;
			else cpri_num = 1;
			
			Send_FX_FA_STS_RSP(FX_FA_STS_RSP, (4*cpri_num), r_ip, port16, rxf->SeqNum); // Number of FA/Path is 8
			return 1;	
			
		// 2019.04.30
		case FX_PATH_STS_REQ:
			print_agent( DEBUG_LS_Comm, VT_Color_RX, "[RX MSG ID] (0x%02X), FX_PATH_STS_REQ", rxf->Type);
			Send_FX_PATH_STS_RSP(FX_PATH_STS_RSP, 4, r_ip, port16, rxf->SeqNum);	// 2019.05.03, // Number of Path is 4
			return 1;
			
		//-------------------------------
		default: return 0;
		//-------------------------------
	}
}

/*
--------------------------------------------------------------

	Control Procedure 
	of
	All Control 
	from
	Layter Splitter 

--------------------------------------------------------------
*/
void Proc___Ctrl_from_LS(void) // 2019.04.18
{
	INT8U l;
	INT8U fa;
	
	INT8U  fwd_change[AMPU_4_MAX] = {0,0,0,0};
	INT8U  rvs_change[AMPU_4_MAX] = {0,0,0,0};
	
	static INT32S T2A_ns[MAX_Optic] = {0,0}; // 2019.09.26
	static INT32S TA3_ns[MAX_Optic] = {0,0}; // 2019.09.26

	int diff_T2A_ns[MAX_Optic];  // 2019.09.26
	int diff_TA3_ns[MAX_Optic];  // 2019.09.26
	
	
	#ifdef __TYPE__PRU__
	static INT32U pau_fwd_offset[AMPU_4_MAX] = {0,0,0,0};
	static INT32U pau_rvs_offset[AMPU_4_MAX] = {0,0,0,0};	
	INT32U offset_ns;
	#else
	static INT8U CFR_OnOff_Prev[MAX_DL] = {0xFF,0xFF,0xFF,0xFF}; // 0xFF for control when restart.
	#endif
	int delay_ns;

	#ifdef __TYPE__PRU__
	for(l=0; l<AMPU_4_MAX; l++)
	{
		offset_ns = 10*m_St.FWD_Dly_Offset_x100[l];
		if(pau_fwd_offset[l] != offset_ns)
		{
			fwd_change[l] = 1;
			pau_fwd_offset[l] = offset_ns;
		}

		offset_ns = 10*m_St.RVS_Dly_Offset_x100[l];
		if(pau_rvs_offset[l] != offset_ns)
		{
			rvs_change[l] = 1;
			pau_rvs_offset[l] = offset_ns;
		}
	}
	#else // AAU
	for(l=0; l<AMPU_4_MAX; l++)
	{
		if(CFR_OnOff_Prev[l] != m_St.d_CFR_OnOff[l]) 
		{
			CFR_OnOff_Prev[l] = m_St.d_CFR_OnOff[l];
			fwd_change[l] = 1;
			rvs_change[l] = 1;
		}
	}
	#endif		

	//--------------------------------------------------------
	// 2019.09.26 : if T2A or T3A is changed, refresh FPGA Delay Setting
	
	for(fa=0; fa<MAX_Optic; fa++)
	{	
		if(fa==Alpha)
		{
			if(IS_Alpha_Mhz_Existing()==0) continue;
		}
		
		diff_T2A_ns[fa] = T2A_ns[fa] - m_St.T2A_Dly__ns[fa];
		diff_TA3_ns[fa] = TA3_ns[fa] - m_St.TA3_Dly__ns[fa];

		if(diff_T2A_ns[fa] < 0) diff_T2A_ns[fa] *= (-1);
		if(diff_TA3_ns[fa] < 0) diff_TA3_ns[fa] *= (-1);

		if((diff_T2A_ns[fa] >= 10)||(diff_TA3_ns[fa] >= 10))
		{
			for(l=0; l<MAX_DL; l++) 
			{
				fwd_change[l] = 1;
				rvs_change[l] = 1;
			}
			T2A_ns[fa] = m_St.T2A_Dly__ns[fa];
			TA3_ns[fa] = m_St.TA3_Dly__ns[fa];
		}
	}
	

	// Delay Setting ----------------------------
	for(fa=0; fa<MAX_Optic; fa++)
	{
		if(fa==Alpha)
		{
			if(IS_Alpha_Mhz_Existing()==0) continue;
		}
		//--------------------------------------------------------
		if(m_Eep.LS_Delay_DL_nsec[fa] != m_St.LS_Delay_DL_nsec[fa])
		{
			if(m_Eep.LS_Delay_DL_nsec[fa] < 120000)
			{
				m_St.LS_Delay_DL_nsec[fa] = m_Eep.LS_Delay_DL_nsec[fa];
				for(l=0; l<MAX_DL; l++) fwd_change[l] = 1;
			}
		}
		if(m_Eep.LS_Delay_UL_nsec[fa] != m_St.LS_Delay_UL_nsec[fa])
		{
			if(m_Eep.LS_Delay_UL_nsec[fa] < 120000)
			{
				m_St.LS_Delay_UL_nsec[fa] = m_Eep.LS_Delay_UL_nsec[fa];
				for(l=0; l<MAX_DL; l++) rvs_change[l] = 1;
			}
		}
		//--------------------------------------------------------
		for(l=0; l<MAX_DL; l++)
		{
			if(fwd_change[l])
			{
				delay_ns = Use_d_Delay_Ctrl(DL, fa, l, _Not_Disp_);
				if(delay_ns >= 0)
				{
					Use_d_FPGA_Delay_Ctrl(DL, fa, l, delay_ns, _Not_Disp_);
					D_Printf( DEBUG_LS_Comm, VT_YELLOW "[LS] Delay Set : FA%d - DL %d[ns]\n", 
						fa, m_Eep.LS_Delay_DL_nsec[fa]);
				}
			}
			if(rvs_change[l])
			{
				delay_ns = Use_d_Delay_Ctrl(UL, fa, l, _Not_Disp_);
				if(delay_ns >= 0)
				{
					Use_d_FPGA_Delay_Ctrl(UL, fa, l, delay_ns, _Not_Disp_);
					D_Printf( DEBUG_LS_Comm, VT_YELLOW "[LS] Delay Set : FA%d - UL %d[ns]\n", 
						fa, m_Eep.LS_Delay_UL_nsec[fa]);
				}
			}
		}
		//--------------------------------------------------------
	}
	// next control from LS ----------------------------
}



//---------------------------------------------------------------------------------------------------

			






//---------------------------------------------------------------------------------------------------
void Proc_LSH_Node_Pkt(void)
{
	INT8U ip[4];
	INT16U port16;

	D_Printf( DEBUG_LS_Comm, "Proc_LSH_Node_Pkt.\n");
	
	if(Check_LSH_Data(COMM_UDP_LS, (INT8U*)&lsh_comm_rxf, 100, ip, &port16))
	{			
		lsh_comm_rxf.SeqNum = ntohl(lsh_comm_rxf.SeqNum);		
		
		/*
			struct sockaddr_in addr;	
			memset(&addr, 0, sizeof(addr));	
			addr.sin_addr.s_addr = *(INT32U*)ip;
			
			D_Printf(DEBUG_LS_Comm, "[RCU] L/S Recv ip %s\n", inet_ntoa(addr.sin_addr));
			D_Printf( DEBUG_LS_Comm, "Seq Num %d\n", lsh_comm_rxf.SeqNum);
		*/
#ifdef __TEST_LS_MODE__
		LSH_Data_Analysis(&lsh_comm_rxf, &lsh_comm_txf, ip, port16);
		Proc___Ctrl_from_LS(); 
#else
		if(m_St.Is_CPRI_Lock) // 2019.05.14
		{		
			LSH_Data_Analysis(&lsh_comm_rxf, &lsh_comm_txf, ip, port16);
			// 2019.04.18
			Proc___Ctrl_from_LS(); 
		}
		else // 2019.05.14
		{
			D_Printf( DEBUG_LS_Comm, VT_BOLD_RED "!! Receive L/S Packet, But CPRI-unLock.\n");
			printf(VT_BOLD_RED "!! Receive L/S Packet, But CPRI-unLock.\n" VT_END_CR);
		}
#endif		
	}	
}

//---------------------------------------------------------------------------------------------------
/* EOF */


