/////////////////////////////
// rems.C
/////////////////////////////
#define REMS_GLOBALS
#include "includes.h"
/////////////////////////////

/*
///////////////////////////////////////////////////////////////////////////////////////

4. 통계보고 : 
- 설정 주기 : 30분, 1시간으로 변경
- 설정 주기 동안 통계 지원(default: 30분)
- 정각 기준으로 통계 검출 (매시 0~30, 30~60, 0~60)
- Traffic 분산을 위한 각 Node별 보고시간 분산 : PRU/AAU 미지원(각 Node별 기준시 다르게 설정)

5. 파형 조회에 대한 패킷 분할 ( 파인측과 별도 협의 후 공유 예정), (1024 : 서버의 DB 사이즈)
6. 파형감시 : 현재 local과 동일하게 처리 가능할 것으로 예상.(현 구현 상태 유지)

7. Suppression Alarm
알람 발생: 1분 10회 발생 시
알람 해제: 1분 0회 발생 시 (정상 상태 1분 이상 유지 시)

* 미사용 Command
- SFP Module Inventory 상태 조회
- local Time
///////////////////////////////////////////////////////////////////////////////////////
*/


/////////////////////////////
// Local Variables
/////////////////////////////

static PROTOCOL_FRAME	rems_auto_txf;
static pthread_mutex_t	Auto_Rpt_Q_Sem;
static pthread_mutex_t	Auto_Log_Q_Sem; // for alarm buffering when gREMS Comm.Not.Start

static pthread_mutex_t	gREMS_Pkt_1k_Sem;
static pthread_mutex_t	gREMS_Pkt_1k_Mea_Sem;
static pthread_mutex_t	gREMS_Pkt_1k_Spectrum;	//// YCSO added 191212

//--------------------------



static INT16U alm_que_co = 0;
static INT16U alm_que_st = 0;
static INT16U alm_que_ed = 0;
static INT16U alm_que_idx_super_cap_change = 0xFFFF;

// for alarm buffering when gREMS Comm.Not.Start
static INT16U log_que_co = 0;
static INT16U log_que_st = 0;
static INT16U log_que_ed = 0;


//--------------------------
//// YCSO added START 191212

static INT8U	REMS_specturm_buf[MAX_COM_BUFFER];

#define REMS_SPECTRUM_DATA_SIZE	(1000)

//// YCSO added END 191212

//--------------------------
#define ALM_BUF_SIZE (1024)


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////


INT8U Make_gREMS_Time_AID_Struct(INT8U* pData)
{
	INT8U i = 0;
	pData[i++] = 0xFF;
	pData[i++] = 0xFE;
	pData[i++] = 6;
	memcpy(&pData[i], m_St.Ru_Sys_Date, 6);
	i+=6;
	
	return i;
}
//----------------------------------------------------------------------
// 미사용 
INT16U Make_AID_Data( INT16U* pAid_list, INT16U max_num, INT8U* data )
{
	INT16U l = 0; // init
	INT16U k, idx;
	INT16U len = 0;

	for( k=0; k < max_num; k++ )
	{
		idx = pAid_list[k];
		if(idx != 0xFFFF)
		{
			len = Make_SKT_AID( idx, &data[l], 0 );
		}
	}
	return len;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define AID_DATA_Max_Len (4*2*4) // 4_byte x 2_optic x 4_layer 
typedef struct
{
	INT8U last_data[AID_DATA_Max_Len];
	INT32U f_change;
}CFnC_DATA;

static CFnC_DATA m_CFnC_Data[CFnC_IDX_MAX]; // CFG_nC_AID_idx[CFnC_IDX_MAX];
static OS_TIMER Timer_CFnC_Chk;

void Chk_Update_of_CF_nC( void )
{
	static INT8U init = 0;
	INT16U i, idx, cnt;
	INT8U l;
	INT8U data[100];

	//-----------------------------------
	if(init == 0)
	{
		init = 1;
		memset(m_CFnC_Data, 0, sizeof(CFnC_DATA)*CFnC_IDX_MAX);
		OSTimerReset(&Timer_CFnC_Chk);
		
		return;
	}
	//-----------------------------------
	if(OSTimerGet_msec(&Timer_CFnC_Chk) >= (5*TICK_CO_SEC))
	{
		OSTimerReset(&Timer_CFnC_Chk);

		for(cnt=0, i=0; i<CFG_nC_AID_num; i++)
		{
			idx = CFG_nC_AID_idx[i];
			if(idx == 0xFFFF) continue;
		
			l = Make_GUI_AID(idx, data, 0);
			
			if(l > AID_DATA_Max_Len) continue;
			if(l == 0) continue;
			
			if(memcmp(m_CFnC_Data[i].last_data, data, l)) // if not equal
			{
				memcpy(m_CFnC_Data[i].last_data, data, l);
				m_CFnC_Data[i].f_change = 1;
				cnt++;

				/*
				//print_agent( DEBUG_REMS, VT_WHITE, "[Chk_Update_of_CF_nC] CFG_nC(AID 0x%04X) Changed.", 
				//	Get__GUI_AID(idx));
				*/	
			}
		}
		//-----------------------------------
		if(cnt)
		{
			print_agent( DEBUG_REMS, VT_BOLD_GREEN, "[Chk_Update_of_CF_nC] CFG_nC Changed, %d.", cnt);
			
			m_Env.AutoRep_Req[_EMS_CFG_nC_] = 1;

			for(i=0; i<CFG_nC_AID_num; i++)
				m_CFnC_Data[i].f_change = 0; // init
		}
		//-----------------------------------
	}
}
//----------------------------------------------------------------------

void Disp_Stat_Time(INT8U type)
{
	type32 val32[2];	

	if(type == _start_) // stat section
	{
		val32[0].uD32 = m_Env.STATIS.time32[_start_];
		val32[1].uD32 = m_Env.STATIS.time32[_stop_];
		
		print_agent( DEBUG_REMS, VT_BOLD_YELLOW, " STAT time %02d:%02d:%02d ~ %02d:%02d:%02d\n",
			val32[0].uD8[2], val32[0].uD8[1], val32[0].uD8[0],
			val32[1].uD8[2], val32[1].uD8[1], val32[1].uD8[0]);
	}
	else
	{	
		val32[0].uD32 = m_Env.STATIS.time32[_rpt_];
		print_agent( DEBUG_REMS, VT_BOLD_YELLOW, " STAT Report @ %02d:%02d:%02d\n",
			val32[0].uD8[2], val32[0].uD8[1], val32[0].uD8[0]);
	}
}
//----------------------------------------------------------------------


//-----------------------------------------------------
// Response , 2019.06.11 : 0x02BD = 701 byte
INT16U tx_gREMS_CFG__C_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port)
{
	pthread_mutex_lock(&gREMS_Pkt_1k_Sem);	
	
	gRemsRspPkt_1k.use = 1;
	gRemsRspPkt_1k.seqNo16 = rxf->seqNo16;
	memcpy(gRemsRspPkt_1k.ip, r_ip, 4);
	gRemsRspPkt_1k.port16 = r_udp_port;
	gRemsRspPkt_1k.cmd = rxf->Type;
	print_agent(DEBUG_REMS, VT_BOLD_MAGENTA, "[tx_gREMS_CFG__C_Status] CMD 0x%X ---- ", gRemsRspPkt_1k.cmd);
	Make_REMS_SubData( CFG__C_AID_idx, CFG__C_AID_num, &gRemsRspPkt_1k, port );

	pthread_mutex_unlock(&gREMS_Pkt_1k_Sem);
	
	OSTimeDly(2*OS_DELAY_100msec); // 2019.09.05
	
	return 0;
}


INT16U tx_gREMS_Hidden_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port)
{
	pthread_mutex_lock(&gREMS_Pkt_1k_Sem);
	
	gRemsRspPkt_1k.use = 1;
	gRemsRspPkt_1k.seqNo16 = rxf->seqNo16;
	memcpy(gRemsRspPkt_1k.ip, r_ip, 4);
	gRemsRspPkt_1k.port16 = r_udp_port;
	gRemsRspPkt_1k.cmd = rxf->Type;
	
	Make_REMS_SubData( Hidden_AID_idx, Hidden_AID_num, &gRemsRspPkt_1k, port );

	pthread_mutex_unlock(&gREMS_Pkt_1k_Sem);	
	
	OSTimeDly(2*OS_DELAY_100msec); // 2019.09.05
	
	return 0;
}

//-----------------------------------------------------
// Response , 2019.06.11 : 0x018F = 399 byte
INT16U tx_gREMS_CFG_nC_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port)
{
	pthread_mutex_lock(&gREMS_Pkt_1k_Sem);
	
	gRemsRspPkt_1k.use = 1;
	gRemsRspPkt_1k.seqNo16 = rxf->seqNo16;
	memcpy(gRemsRspPkt_1k.ip, r_ip, 4);
	gRemsRspPkt_1k.port16 = r_udp_port;
	gRemsRspPkt_1k.cmd = rxf->Type;
	Make_REMS_SubData( CFG_nC_AID_idx, CFG_nC_AID_num, &gRemsRspPkt_1k, port );

	pthread_mutex_unlock(&gREMS_Pkt_1k_Sem);	

	OSTimeDly(2*OS_DELAY_100msec); // 2019.09.05
	
	return 0;
}

INT16U tx_gREMS_Mea_St2_Status(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port)
{
#if 0
	INT16U len;
	INT8U idx;
	INT16U i = 0;
	INT8U totPkt = 1, curPkt = 1;

	idx = Get_Pkt_Idx( rxf );
	gRemsRspPkt_1k_Mea.cmd = rxf->Type;
	if(idx == 1) Make_REMS_SubData( Meas___AID_idx, Meas___AID_num, &gRemsRspPkt_1k_Mea, port );
	len = gRemsRspPkt_1k_Mea.size[idx-1];
	gRemsRspPkt_1k_Mea.cur = idx;

	//----------------------
	// ALARM : 0x0001 AID
	//----------------------
	i = 0;
	txf->data[i++] = 0x00;
	txf->data[i++] = 0x01;
	txf->data[i++] = RU_ALARM_CNT*2;
	memcpy( &txf->data[i], RU_ARM_SUPP, RU_ALARM_CNT );
	i += RU_ALARM_CNT;
	memcpy( &txf->data[i], &m_St.Last_Alm, RU_ALARM_CNT );
	i += RU_ALARM_CNT;
	//----------------------
	memcpy( &txf->data[i], &gRemsRspPkt_1k_Mea.AID[0], 5 ); 
	i += 5;
	
	memcpy( &txf->data[i], &gRemsRspPkt_1k_Mea.data[(idx-1)* UDP_MAX_LEN], len );
	i += len;

	return Make_Frame( rxf->Type, rxf->Rcode, txf, i, rxf->seqNo16, totPkt, curPkt );	
#else
	pthread_mutex_lock(&gREMS_Pkt_1k_Mea_Sem);
	
	gRemsRspPkt_1k_Mea.use = 1;
	gRemsRspPkt_1k_Mea.seqNo16 = rxf->seqNo16;
	memcpy(gRemsRspPkt_1k_Mea.ip, r_ip, 4);
	gRemsRspPkt_1k_Mea.port16 = r_udp_port;
	gRemsRspPkt_1k_Mea.cmd = rxf->Type;
	Make_REMS_SubData( Meas___AID_idx, Meas___AID_num, &gRemsRspPkt_1k_Mea, port );

	pthread_mutex_unlock(&gREMS_Pkt_1k_Mea_Sem);
	return 0;
#endif
}



//----------------------------------------------
INT8U Is_Stat_Enable(void)
{
	return m_REMS_Env.stat_enable;
}
//----------------------------------------------
void Enable_Stat(void)
{
	m_REMS_Env.stat_enable = 1;
	//printf("[RCU] gREMS Statistics Start.\n");
}
//----------------------------------------------
void Stop_Stat(void)
{
	m_REMS_Env.stat_enable = 0;
	//printf("[RCU] gREMS Statistics Stop.\n");
}
//----------------------------------------------




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifdef __REMS_SUPPRESS_TEST__
#define SUP_ALM_GEN_CONTI_CNT (3)  // test
#define SUP_ALM_CLR_CNT       (30) // 30[s]
#else
#define SUP_ALM_GEN_CONTI_CNT (10) // 10[s]
#define SUP_ALM_CLR_CNT       (60) // 60[s]
#endif
//-------------------------------------------------------------------------------------
#define SUP_CNT_per_SEC (1) // 1초당 함수 호출되는 카운트 값.

static INT8U Supp_Alm_Gen_Timer_Cnt[RU_ALARM_CNT][8];

void Supp_Alm_Timer_Init( void )
{
	memset( Supp_Alm_Gen_Timer_Cnt, 0, RU_ALARM_CNT*8 );
}

void Supp_Alm_Timer_Inc( INT8U byte, INT8U bit ) 
{
	if( byte >= RU_ALARM_CNT ) return;
	if( bit  >= 8 ) return;

	Supp_Alm_Gen_Timer_Cnt[byte][bit]++;
}

// suppress alarm 1회 발생 된 이후의 시간 카운트.
// 1분을 cnt 함.
INT8U Get_Supp_Alm_Time_Cnt( INT8U byte, INT8U bit ) 
{
	if( byte >= RU_ALARM_CNT ) return 0;
	if( bit  >= 8 ) return 0;

	return Supp_Alm_Gen_Timer_Cnt[byte][bit];
}

void Supp_Alm_Timer_Clr( INT8U byte, INT8U bit ) 
{
	if( byte >= RU_ALARM_CNT ) return;
	if( bit  >= 8 ) return;

	Supp_Alm_Gen_Timer_Cnt[byte][bit] = 0;
}


static INT16U  SUP_Arm_Gen_Co[RU_ALARM_CNT][8]; // x 8 bit
static INT16U  SUP_Arm_Clr_Co[RU_ALARM_CNT][8]; // x 8 bit

void Make_Suppress_Alarm( void )
{
	INT8U i, b_posi, bit, chg;
	INT8U arm_cur;

	INT8U* pLast_Arm = (INT8U*)&m_St.Last_Alm;
	INT8U* pPrev_Arm = (INT8U*)&m_St.Prev_Alm;	

	INT16U gen_max_cnt = SUP_ALM_GEN_CONTI_CNT * SUP_CNT_per_SEC; // 10회
	INT16U clr_max_cnt = SUP_ALM_CLR_CNT       * SUP_CNT_per_SEC; // 1분

	for( i=0; i<RU_ALARM_CNT; i++ )
	{
		chg = pPrev_Arm[i] ^ pLast_Arm[i];
		
		arm_cur = pLast_Arm[i];

		// update last-alarm to previous-alarm
		pPrev_Arm[i] = pLast_Arm[i]; 

		for( b_posi=0; b_posi<8; b_posi++ )
		{
			bit = cbits[b_posi];

			if(SUP_Arm_Gen_Co[i][b_posi] == 0)
			{
				Supp_Alm_Timer_Clr(i, b_posi);
			}
			else
			{
				Supp_Alm_Timer_Inc(i, b_posi); // timer increment

				/*
				// TEST
				//if((i == 0)&&(b_posi == 7))
				//{
				//	D_Printf( DEBUG_REMS, VT_BOLD_YELLOW "Alm[%d,%d] timer %d", 
				//		i, b_posi, Get_Supp_Alm_Time_Cnt(i, b_posi) );
				//}
				*/

				if(Get_Supp_Alm_Time_Cnt(i, b_posi) > 60)
				{
					Supp_Alm_Timer_Clr(i, b_posi);
					SUP_Arm_Gen_Co[i][b_posi] = 0;

					////D_Printf( DEBUG_REMS, VT_BOLD_YELLOW "Alm[%d,%d] SuppCnt Clr", i, b_posi );
				}
			}

			if(arm_cur & bit) // if alarm
			{
				if(chg & bit) // if generated...
				{
					if (RU_ARM_SUPP[i] & bit) // if suppressed, skip
						continue;
						
					// Not suppress_alarm				
					SUP_Arm_Gen_Co[i][b_posi]++; // count of alarm change
					if( SUP_Arm_Gen_Co[i][b_posi] >= gen_max_cnt )
					{
						RU_ARM_SUPP[i]  |= bit; // suppress alarm set
						m_St.Supp_Arm_f[i] |= bit; // flag set

						// init
						SUP_Arm_Clr_Co[i][b_posi] = 0;
						SUP_Arm_Gen_Co[i][b_posi] = 0;

						D_Printf( DEBUG_REMS, VT_BOLD_YELLOW "SuppAlm[%d,%d] Generate", i, b_posi );							
					}
				}
				SUP_Arm_Clr_Co[i][b_posi] = 0;
			}
			else // alarm/normal state is not changed
			{
				if (RU_ARM_SUPP[i] & bit) // if suppress_alarm			
				{
					SUP_Arm_Clr_Co[i][b_posi]++;

					if( SUP_Arm_Clr_Co[i][b_posi] >= clr_max_cnt ) // 1분
					{
						RU_ARM_SUPP[i] &= nbits[b_posi]; // suppress alarm clr
						m_St.Supp_Arm_f[i] |= bit; // flag set

						// init
						SUP_Arm_Clr_Co[i][b_posi] = 0;
						SUP_Arm_Gen_Co[i][b_posi] = 0;
						
						D_Printf( DEBUG_REMS, VT_BOLD_YELLOW "SuppAlm[%d,%d] Clear", i, b_posi );
					}
				}
			}
		}		
	}	
	Check_Suppress_Alm();
}
//-------------------------------------------------------------------------------------




//---------------------------------------------------------------------
// max, min, cnt, sum 값을 clear.
void Clr_REMS_Struct(void)
{
	INT16U i;
	INT16U cnt = sizeof(REMS_ST_STR) / sizeof(REMS_U32);
	REMS_U32* pItem = (REMS_U32*)&m_Stat;

	for (i = 0; i < cnt; i++)
	{
		pItem->cnt = 0; // clear
		pItem->max = 0; // clear
		pItem->min = 0; // clear
		pItem->sum = 0; // clear
		
		pItem++; // next
	}
}
//---------------------------------------------------------------------
INT8U Is_Time_In_Range_xx( INT32U start32, INT32U stop32, INT32U cur32 )
{
	INT8U in_range;

	if(start32 <= stop32) // 시작시간이 작은경우.
	{
		if((cur32 > start32) && (cur32 < stop32)) 
			 in_range = 1;
		else in_range = 0;
	}
	else // 시작시간이 더 클경우. 부등호 주의...해야.
	{				
		if((cur32 >= stop32) && (cur32 <= start32)) 
			 in_range = 0;
		else in_range = 1;
	}
	return in_range;
}
//------------------------------------------------
void Disp_Stat_St(REMS_U32* pItem)
{
	INT32U sum;

	if(pItem->cnt != 0)
	{
		if(pItem->sign)
		{
			if(pItem->s_sum < 0)
			{
				sum = (-1) * pItem->s_sum;
				sum /= pItem->cnt;
				//avr = sum * (-1);
			}
			else 
			{
				//avr = pItem->s_sum / pItem->cnt;
			}
			/*
			sprintf(dbgstr, "EMS32 (0x%02X%02X) %d/%d, cnt(%d) avg(%d)",\
				pItem->aid[0], pItem->aid[1], pItem->s_max, pItem->s_min, pItem->cnt, avr);
			*/
		}
		else
		{
			//avr = pItem->sum / pItem->cnt;
			/*
			sprintf(dbgstr, "EMS32 (0x%02X%02X) %d/%d, cnt(%d) avg(%d)",\
				pItem->aid[0], pItem->aid[1], pItem->s_max, pItem->s_min, pItem->cnt, avr);
			*/
		}
	}
}

//------------------------------------------------


void Update_Stat( REMS_U32* pItem, INT32U uVal )
{
	INT32S sVal;

	if(Is_Stat_Enable() == 0) 
		return;

	//-------------------------------------------
	/*
	2019.06.28
	-----------
		REMS_U32 d_BIP_Count[DL_UL][CPRI_PATH];// 0x530A ---> m_St.d_BIP_ErrCnt[DL][CPRI_PATH]
		REMS_U32 d_Hard_Clip_Cnt_DL[MAX_DL];   // 0x5320 ---> m_St.d_Hard_Clip_Cnt_Accum[MAX_DL]	
	*/
	if(pItem->aid_stat.aid[0] == 0x53)
	{
		if((pItem->aid_stat.aid[1] == 0x0A)||(pItem->aid_stat.aid[1] == 0x20))
		{
			pItem->sum = uVal;
			pItem->max = uVal;
			pItem->min = uVal;
			pItem->cnt = 1;
			
			return;
		}
	}
	//-------------------------------------------
	if(pItem->sign) // signed...
	{
		sVal = (INT32S)uVal;		

		if(pItem->cnt == 0)
		{
			pItem->s_sum = sVal;
			pItem->s_max = pItem->s_min = sVal;			
		}
		else
		{
			pItem->s_sum += sVal;
			
			if( pItem->s_max < sVal ) pItem->s_max = sVal;
			if( pItem->s_min > sVal ) pItem->s_min = sVal;
		}
	}
	else
	{
		if(pItem->cnt == 0)
		{
			pItem->sum = uVal;
			pItem->max = pItem->min = uVal;
		}
		else
		{
			pItem->sum += uVal;
			
			if( pItem->max < uVal ) pItem->max = uVal;
			if( pItem->min > uVal ) pItem->min = uVal;
		}
	}
	pItem->cnt++;


	#if 0 // TEST	
	pItem->min = pItem->cnt;
	#endif
}
//------------------------------------------------
void Calc_Stat_Average(void)
{
	REMS_U32* pItem = (REMS_U32*)&m_Stat;
	INT16U max_num = sizeof(REMS_ST_STR) / sizeof(REMS_U32);
	INT16U i;
	INT32U sum;	


	//------------------
	
	Stop_Stat();

	for (i=0; i< max_num; i++)
	{
		if(pItem->cnt > 0)
		{
			if(pItem->sign)
			{
			#if 0 // TEST
					sSum = pItem->s_sum;
					sAvr = sSum / pItem->cnt;
					pItem->s_avr = (INT32S)sAvr; 

					D_Printf( DEBUG_REMS, VT_Color_Err "[Calc_Stat_Average 0x%02X%02X] sum %d, avr %d,%d,%d, cnt %d.", 
						pItem->aid_stat.aid[0], pItem->aid_stat.aid[1],
						sSum, 
						sAvr, pItem->avr, pItem->s_avr, 
						pItem->cnt); 
			#else
				if(pItem->s_sum < 0)
				{		
					sum = (-1) * pItem->s_sum;
					sum /= pItem->cnt;
					pItem->s_avr = sum * (-1);				
				}
				else 
				{
					pItem->s_avr = pItem->s_sum / pItem->cnt;
				}
			#endif	
				
				pItem->prev_s_max = pItem->s_max;
				pItem->prev_s_min = pItem->s_min;
				pItem->prev_s_avr = pItem->s_avr;
				pItem->prev_s_sum = pItem->s_sum;				
			}
			else
			{
				pItem->avr = pItem->sum / pItem->cnt;

				pItem->prev_max = pItem->max;
				pItem->prev_min = pItem->min;
				pItem->prev_avr = pItem->avr;						
				pItem->prev_sum = pItem->sum;
			}
		}		
		else
		{
			if((pItem->aid_stat.aid[0] != 0)&&(pItem->aid_stat.aid[1] != 0))
			{
				//sprintf( dbgstr, "STATISTIC_zero (0x%02X %02X)", pItem->aid[0], pItem->aid[1]);
							
				if(pItem->sign)
				{
					pItem->s_max = pItem->prev_s_max;
					pItem->s_min = pItem->prev_s_min;
					pItem->s_avr = pItem->prev_s_avr;
					pItem->s_sum = pItem->prev_s_sum;				
				}
				else
				{
					pItem->max = pItem->prev_max;
					pItem->min = pItem->prev_min;
					pItem->avr = pItem->prev_avr;						
					pItem->sum = pItem->prev_sum;
				}
			}
		}
		
		pItem++; // next
	}

	Enable_Stat();
}
//------------------------------------------------


//--------------------------------------------------
void Insert_Stat(INT8U aid_0, INT8U aid_1, INT8U d_type, INT8U step, INT8U size, INT8U l, INT8U c, REMS_U32* pItem)
{
	STAT_AID stat_aid;
	INT8U num; // item count
	INT8U i;
	INT16U flag16;	

	memset(&stat_aid, 0, sizeof(STAT_AID)); // init

	//-------------------------------------
	num = l*c; // layer x carrier
	if((num == 0)||(num > 16)) // max 16
	{		
		D_Printf( DEBUG_REMS, VT_Color_Err "[RCU] Insert_Stat(%d) Fail.", num); 
		return;		
	}	
	//-------------------------------------
	for(flag16=0, i=0; i<num; i++) 
		flag16 |= cbits16[15-i];
	
	pItem->flags[0] = (INT8U)(flag16>>8);
	pItem->flags[1] = (INT8U)flag16;
	
	if(num <= 8) pItem->f_size = 1;
	else         pItem->f_size = 2;
	//-------------------------------------		
	switch(d_type)
	{
		case __INT8S:
		case _INT16S:
		case _INT32S:
			pItem->sign = 1; // minus
			break;
		
		default:
			pItem->sign = 0; // plus
			break;
	}
	//-------------------------------------		
	stat_aid.aid[0] = aid_0;
	stat_aid.aid[1] = aid_1;
	stat_aid.len = 6 + l*c*size*3 + pItem->f_size;
	stat_aid.type = d_type;
	stat_aid.step = step;
	stat_aid.reserved = 0;
	stat_aid.size = size;  // data 1 size
	stat_aid.layer = l; // layer num
	stat_aid.carrier = c; // carrier num

	memcpy(&pItem->aid_stat, &stat_aid, sizeof(STAT_AID));
	//-------------------------------------
	pItem->prev_s_max = pItem->s_max = 0;
	pItem->prev_s_min = pItem->s_min = 0;
	pItem->prev_s_avr = pItem->s_avr = 0;
	pItem->prev_s_sum = pItem->s_sum = 0;	
}

//--------------------------------------------------
// AID 를 Statistic 하기 위해 등록 및 초기화.
void gREMS_Stat_Insert_AID(void)
{	
	INT8U optic, path4, cpri, i;
	
	Insert_Stat(0x50,0x12, _INT16S, Step_____01, 2, 1, 1, (REMS_U32*)&m_Stat.Temp_x10);

	for(optic = 0; optic < MAX_Optic; optic++)
	{
		for(path4 = 0; path4 < CPRI_PATH; path4++)
		{
			Insert_Stat(0x51,0x00, _INT16S, Step_____01, 2, 4, 2, (REMS_U32*)&m_Stat.d_DL_TSSI_dBm_x10[optic][path4]);	
			// 2019.08.12 : "DL_TSSI_Decimal" is not stat.			
			Insert_Stat(0x51,0x08, _INT16S, Step_____01, 2, 4, 2, (REMS_U32*)&m_Stat.DL_OutPwr_Cal_x10[optic][path4]);
			Insert_Stat(0x52,0x00, _INT16S, Step_____01, 2, 4, 2, (REMS_U32*)&m_Stat.d_UL_RSSI_dBm_x10[optic][path4]);
		}
	}
	
	for(path4 = 0; path4 < MAX_DL; path4++)
		Insert_Stat(0x51,0x03, _INT16S, Step_____01, 2, 4, 1, (REMS_U32*)&m_Stat.Fwd_Out_Pwr_x10[path4]);	
	
	for(cpri = 0; cpri < CPRI_PATH; cpri++)
	{
		Insert_Stat(0x53,0x05, _INT16S, Step_____01, 2, 2, 2, (REMS_U32*)&m_Stat.d_SFP_LD_Pwr_x10[cpri]);	
		Insert_Stat(0x53,0x08, _INT16S, Step_____01, 2, 2, 2, (REMS_U32*)&m_Stat.d_SFP_PD_Pwr_x10[cpri]);
	}

	for(i = 0; i < 4; i++) // 2020.02.27 : loop added.
		Insert_Stat(0x53,0x0A, _INT32U, Step_No_Use, 4, 2, 2, (REMS_U32*)&m_Stat.d_BIP_AccCnt[i]);
		
	for(cpri = 0; cpri < CPRI_PATH; cpri++)
	{
		Insert_Stat(0x53,0x10, _INT16S, Step_____01, 2, 2, 2, (REMS_U32*)&m_Stat.d_SFP_Vcc_x10[cpri]);		
		Insert_Stat(0x53,0x11, _INT16S, Step_____01, 2, 2, 2, (REMS_U32*)&m_Stat.d_SFP_LD_Bias_x10[cpri]);
		Insert_Stat(0x53,0x14, _INT16S, Step_____01, 2, 2, 2, (REMS_U32*)&m_Stat.d_SFP_Temp_x10[cpri]);	
	}

	for(path4 = 0; path4 < MAX_DL; path4++)
		Insert_Stat(0x53,0x20, _INT32U, Step_No_Use, 4, 4, 1, (REMS_U32*)&m_Stat.d_Hard_Clip_Cnt_DL[path4]);

	for(optic = 0; optic < MAX_Optic; optic++)
		Insert_Stat(0x54,0x05, _INT32U, Step_No_Use, 4, 1, 2, (REMS_U32*)&m_Stat.d_RoundTrip_Dly[optic]);


	/*
	2020.07.08 : add statistics
	*/	
	Insert_Stat(0x50,0x24, _INT16S, Step_____01, 2, 1, 1, (REMS_U32*)&m_Stat.PSU_Temp_x10);
	Insert_Stat(0x50,0x25, _INT16S, Step_____01, 2, 1, 1, (REMS_U32*)&m_Stat.PSU_Main_DC_x10);
	Insert_Stat(0x50,0x26, _INT16S, Step_____01, 2, 1, 1, (REMS_U32*)&m_Stat.PSU_Sub__DC_x10);

	for(path4 = 0; path4 < MAX_DL; path4++)
		Insert_Stat(0x51,0x1A, _INT16S, Step_____01, 2, 4, 1, (REMS_U32*)&m_Stat.Ret_Loss_x10[path4]);

	for(optic = 0; optic < MAX_Optic; optic++)
		Insert_Stat(0x54,0x09, _INT32U, Step_No_Use, 4, 1, 2, (REMS_U32*)&m_Stat.d_Oneway_Dly[optic]);
}


//---------------------------------------------------------------------
void gREMS_Stat_Init(void)
{
	memset((INT8U*)&m_Stat, 0, sizeof(REMS_ST_STR)); // all zeros.
	m_REMS_Env.stat_enable = 0;

	if(m_Eep.Statis_Rpt_Prd > 0x04) m_Eep.Statis_Rpt_Prd = 0;
	if(m_Eep.Statis_Rpt_Ref[0] >= 24) m_Eep.Statis_Rpt_Ref[0] = 0;
	if(m_Eep.Statis_Rpt_Ref[1] >= 60) m_Eep.Statis_Rpt_Ref[1] = 0;

	if(m_Eep.REMS_Auto_Rpt_Time[0] >= 24) m_Eep.REMS_Auto_Rpt_Time[0] = 0;
	if(m_Eep.REMS_Auto_Rpt_Time[1] >= 60) m_Eep.REMS_Auto_Rpt_Time[1] = 0;
	

	gREMS_Stat_Insert_AID();
}
//-------------------------------------------------------------------------------

INT8U Make_gREMS_AutoRpt_SubData( REMS_1024_Pkt* pPkt, INT8U* paid, INT8U len )
{
	INT16U l;
	INT8U cur;
	INT8U wr_ok = 0; // default

	//-----------------------------------------------------			
	cur = pPkt->cur;
	l = pPkt->l;
	
	if(pPkt->l != 0xFFFF)
	{
		//-----------------------------------------------------
		if ((l+len) <= MAX_LEN_STATIS)
		{					
			memcpy( &pPkt->data[UDP_MAX_LEN * cur + l], paid, len );
			l += len;
			wr_ok = 1;
		}
		else
		{	
			pPkt->size[cur++] = l;
			pPkt->tot = cur;
			l = 0; // init

			if (cur >= MAX_PKT_NUM) pPkt->l = 0xFFFF;
			else
			{
				memcpy( &pPkt->data[UDP_MAX_LEN * cur + l], paid, len );
				l += len;
				wr_ok = 1;
			}
		}
		pPkt->cur = cur;
		pPkt->l = l;
	}
	//sprintf( dbgstr, "Make_gREMS_AutoRpt_SubData len %d, cur %d, l %d", len, cur, l );
	
	return wr_ok;
}

//--------------------------------------------------------------------------------------------------------
#define MAX_STAT_AID_STR_Len (300)
static INT8U stat_data[MAX_STAT_AID_STR_Len];





INT8U Load_Stat_to_Buf( REMS_U32* pItem, REMS_1024_Pkt* pPkt )
{	
	INT16U i = 0;	
	INT8U k, item_num;
	INT8U byte_size;
	INT8U* data = stat_data;
	type16 val16;
	type32 val32;
		

	if( pItem->aid_stat.len > MAX_STAT_AID_STR_Len)
	{
		D_Printf( DEBUG_REMS, VT_Color_Err "Load_Stat_to_Buf size(%d) Fail.", 
			pItem->aid_stat.len);	
			
		return 0;
	}
	if(pItem == NULL)
	{
		D_Printf( DEBUG_REMS, VT_Color_Err "pItem NULL");	
			
		return 0;
	}
	//--------------------------------------------------	
	////////sprintf( dbgstr, "Load EMS Static (AID %02X%02X) %d, %d", pItem->aid[0], pItem->aid[1], i, size );
	//--------------------------------------------------
	data[i++] = pItem->aid_stat.aid[0];
	data[i++] = pItem->aid_stat.aid[1];
	data[i++] = pItem->aid_stat.len >> 8;
	data[i++] = (INT8U)pItem->aid_stat.len;
	data[i++] = pItem->aid_stat.type;
	data[i++] = pItem->aid_stat.step;
	data[i++] = pItem->aid_stat.reserved;
	data[i++] = pItem->aid_stat.size;
	data[i++] = pItem->aid_stat.layer;
	data[i++] = pItem->aid_stat.carrier;	
	data[i++] = pItem->flags[0];
	
	if(pItem->f_size > 1) 
		data[i++] = pItem->flags[1];
	//--------------------------------------------------	

	//sprintf( dbgstr, "Load_Stat_to_Buf (AID %02X%02X) s%d, t%d sign%d", pItem->aid[0], pItem->aid[1], size, type, sign );

	//--------------------------------------------------
	item_num = pItem->aid_stat.layer * pItem->aid_stat.carrier;
	byte_size = pItem->aid_stat.size;
	
	for (k = 0; k < item_num; k++)
	{
		if(byte_size == 1)
		{
			if(pItem->sign) data[i++] = (INT8S)pItem->s_max;
			else			data[i++] = pItem->max;
			
			if(pItem->sign) data[i++] = (INT8S)pItem->s_min;
			else			data[i++] = pItem->min;
			
			if(pItem->sign) data[i++] = (INT8S)pItem->s_avr;
			else			data[i++] = pItem->avr;			
		}
		else if(byte_size == 2)
		{
            if( pItem->sign) val16.uD16 = htons(pItem->s_max);
            else             val16.uD16 = htons(pItem->max);
			memcpy(&data[i], val16.uD8, 2); i+=2;

            if( pItem->sign) val16.uD16 = htons(pItem->s_min);
            else             val16.uD16 = htons(pItem->min);
			memcpy(&data[i], val16.uD8, 2); i+=2;
            
            if( pItem->sign) val16.uD16 = htons(pItem->s_avr);
            else             val16.uD16 = htons(pItem->avr);
			memcpy(&data[i], val16.uD8, 2); i+=2;
		}
		else if(byte_size == 4)
		{
            if( pItem->sign) val32.uD32 = htonl(pItem->s_max);
            else             val32.uD32 = htonl(pItem->max);            	
			memcpy(&data[i], val32.uD8, 4); i+=4;

            if( pItem->sign) val32.uD32 = htonl(pItem->s_min);
            else             val32.uD32 = htonl(pItem->min);
			memcpy(&data[i], val32.uD8, 4); i+=4;
            
            if( pItem->sign) val32.uD32 = htonl(pItem->s_avr);
            else             val32.uD32 = htonl(pItem->avr);
			memcpy(&data[i], val32.uD8, 4); i+=4;
		}
				
		pItem++;
	}	
	//--------------------------------------------------
	return Make_gREMS_AutoRpt_SubData( pPkt, data, i );
}
//--------------------------------------------------------------------------------------------------------
INT8U Make_Stat_Header( INT8U* data, INT8U cur, INT8U total, INT8U* pTime, INT8U period)
{
	INT16U i = 0;

	//------------------------------
	// AID : Legacy AID 	
	data[i++] = 0x00;
	data[i++] = 0x01;
	data[i++] = 2*RU_ALARM_CNT; // length 
	memcpy(&data[i], RU_ARM_SUPP, RU_ALARM_CNT); i+= RU_ALARM_CNT;
	memcpy(&data[i], &m_St.Last_Alm , RU_ALARM_CNT); i+= RU_ALARM_CNT;
	//------------------------------
	// AID : New AID : 통계보고 주기   0x5601
	data[i++] = 0x56;
	data[i++] = 0x01;
	data[i++] = 0; // length : Network_Byte_Order 
	data[i++] = 1+7; // length : Network_Byte_Order	
	data[i++] = _PrdDef;
	data[i++] = Step_No_Use;
	data[i++] = 0; // reserved
	data[i++] = 1; // size of data	
	data[i++] = 1; // layer
	data[i++] = 1; // carrier
	data[i++] = 0x80; // flag
	data[i++] = period;
	//------------------------------
	// AID : New AID : 통계 시작 시간  0x5602
	data[i++] = 0x56;
	data[i++] = 0x02;
	data[i++] = 0; // length : Network_Byte_Order, 
	data[i++] = 6+7; // length : Network_Byte_Order
	data[i++] = _PrdDef;
	data[i++] = Step_No_Use;
	data[i++] = 0; // reserved
	data[i++] = 6; // size of data	
	data[i++] = 1; // layer
	data[i++] = 1; // carrier
	data[i++] = 0x80; // flag
	memcpy( &data[i], pTime, 6); i+=6;	
	//------------------------------
	return i;
}
//-------------------------------------------------------------------------------
void Make_REMS_Stat_Pkt_Init( REMS_1024_Pkt* pPkt )
{
	INT8U l;

	pPkt->AID[0] = 0x23;
	pPkt->AID[1] = 0x23;
	pPkt->len = 2;	
	pPkt->cur = 0;
	pPkt->tot = 0;
	pPkt->l = 0;

	for( l=0; l < MAX_PKT_NUM; l++ ) pPkt->size[l] = 0; // init
}
//-------------------------------------------------------------------------------
void Make_STA_EMS_End( REMS_1024_Pkt* pPkt )
{
	INT16U l;
	INT8U cur;
	//INT8U len;
	//INT8U AID_0xFFFE[30];

	cur = pPkt->cur;
	l = pPkt->l;
	
	//-----------------------------------------------------

	#if 0
	/*
	// 2019.09.02 : delete
	//len = Make_gREMS_Time_AID_Struct(AID_0xFFFE); // time info added
    //
	//D_Printf( DEBUG_REMS, VT_Color_Evt "Make_STA_EMS_End, cur %d, l %d, len %d, max %d", 
	//	cur, l, len, MAX_LEN_SUB_DATA_ALM);
	//
	//if ((l+len) > MAX_LEN_SUB_DATA_ALM)	
	//{	
	//	pPkt->size[cur++] = l;
	//	pPkt->tot = cur;
	//	l = 0; // init
	//
	//	if (cur >= MAX_PKT_NUM) pPkt->l = 0xFFFF;
	//}
	//memcpy( &pPkt->data[UDP_MAX_LEN * cur + l], AID_0xFFFE, len );
	//l += len;
	*/
	#endif

	pPkt->size[cur++] = l;
	pPkt->cur = cur;
	pPkt->tot = cur;
	pPkt->l = l;	

	//D_Printf( DEBUG_REMS, VT_Color_Evt "Make_STA_EMS_End : cur %d, l %d", 
	//	cur, l);
}




//-------------------------------------------------------------------------------
static REMS_1024_Pkt Stat_Pkt;
static INT8U header[100];
void Make_Stat_Pkt( INT8U* pTime, INT8U period )
{
	INT16U k, len;
	INT16U size;
	
	//-------------------------
	Calc_Stat_Average();
	Make_REMS_Stat_Pkt_Init( &Stat_Pkt );
	//-------------------------

	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.Temp_x10, &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_DL_TSSI_dBm_x10[0][0], &Stat_Pkt );
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.Fwd_Out_Pwr_x10[0],      &Stat_Pkt );

	// 2019.08.12 : "DL_TSSI_Decimal" is not stat.	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.DL_OutPwr_Cal_x10[0][0], &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_UL_RSSI_dBm_x10[0][0], &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_SFP_LD_Pwr_x10[0],     &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_SFP_PD_Pwr_x10[0],     &Stat_Pkt );
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_BIP_AccCnt[0],         &Stat_Pkt );		
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_SFP_Vcc_x10[0],        &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_SFP_LD_Bias_x10[0],    &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_SFP_Temp_x10[0],       &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_Hard_Clip_Cnt_DL[0],   &Stat_Pkt );
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_RoundTrip_Dly[0],      &Stat_Pkt );

	/*
	2020.07.08 : add statistics
	*/
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.PSU_Temp_x10,    &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.PSU_Main_DC_x10, &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.PSU_Sub__DC_x10, &Stat_Pkt );	
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.Ret_Loss_x10[0], &Stat_Pkt );
	Load_Stat_to_Buf( (REMS_U32*)&m_Stat.d_Oneway_Dly[0], &Stat_Pkt );

	//-------------------------
	Make_STA_EMS_End( &Stat_Pkt );
	Clr_REMS_Struct();

	//-------------------------	
	Init_Statis_Q();
	for (k = 0; k < Stat_Pkt.tot; k++)
	{
		len = Stat_Pkt.size[k];
		Stat_Pkt.cur = k+1;

		size = Make_Stat_Header( header, Stat_Pkt.cur, Stat_Pkt.tot, pTime, period );
		
		if(k>0) size = 0; // exclude header from 2nd
		
		Make_Statis_Q( header, size, &Stat_Pkt.data[k*UDP_MAX_LEN], len, CMD2_STATISTIC_RPT, k, Stat_Pkt.cur, Stat_Pkt.tot);
	}	

	D_Printf( DEBUG_REMS ,"--- Make STATIS Pkt ---\r\n");
}




#if 0
//------------------------------------------------------------------


//-----------------------------------------------------------------------------
INT8U Is_Time_In_Range_ox( INT32U start32, INT32U stop32, INT32U cur32 )
{
	INT8U in_range;

	if(start32 <= stop32) // 시작시간이 작은경우.
	{
		if((cur32 >= start32) && (cur32 < stop32)) 
			 in_range = 1;
		else in_range = 0;
	}
	else // 시작시간이 더 클경우. 부등호 주의...해야.
	{				
		if((cur32 >= stop32) && (cur32 < start32)) 
			 in_range = 0;
		else in_range = 1;
	}
	return in_range;
}
//-----------------------------------------------------------------------------
INT8U Is_Time_In_Range_oo( INT32U start32, INT32U stop32, INT32U cur32 )
{
	INT8U in_range;

	if(start32 <= stop32) // 시작시간이 작은경우.
	{
		if((cur32 >= start32) && (cur32 <= stop32)) 
			 in_range = 1;
		else in_range = 0;
	}
	else // 시작시간이 더 클경우. 부등호 주의...해야.
	{				
		if((cur32 > stop32) && (cur32 < start32)) 
			 in_range = 0;
		else in_range = 1;
	}
	return in_range;
}
//-----------------------------------------------------------------------------
INT8U Is_Time_In_Range_xo( INT32U start32, INT32U stop32, INT32U cur32 )
{
	INT8U in_range;

	if(start32 <= stop32) // 시작시간이 작은경우.
	{
		if((cur32 > start32) && (cur32 <= stop32)) 
			 in_range = 1;
		else in_range = 0;
	}
	else // 시작시간이 더 클경우. 부등호 주의...해야.
	{				
		if((cur32 > stop32) && (cur32 <= start32)) 
			 in_range = 0;
		else in_range = 1;
	}
	return in_range;
}
//-----------------------------------------------------------------------------
#endif





//-------------------------------------------------------------------
/*
year-mon-day-hour-min-sec // 0x237A, statistic start time, 

example) 
m_Eep.Statis_Rpt_Ref
statistic 기준시(0x5600) : 01시02분

m_Eep.Statis_Rpt_Prd
statistic 주기  (0x5601) : 15분

statistic start : 매시간 0분 기준 15분 간격 : 매시 0분 + [통계 주기 x N]
statistic stop  : start 시간 기준 15분 간격 
statistic report: 01시  02분 기준 15분 간격 

----- 5G PRU/AAU -----
통계보고 주기   0x5601
	0x00 : 통계 보고 안함
	0x01 : 5분
	0x02 : 15분
	0x03 : 30분
	0x04 : 60분

통계 시작 시간  0x5602, Hex,  YYMMDDhhmmss

*/
//-------------------------------------------------------------------


// inc_time32, cur_time32 : 0x020304 ==> 2시3분4초
// cur_time32 에서 inc_min + inc_sec 후의 시간을 계산.
INT32U Calc_Time_Offset( INT32U cur_time32, INT32U inc_min, INT16U inc_sec )
{
	INT32U time32;
	INT8U time[3]; // [hour/min/sec]
	INT8U quo; // quotient : 몫	


	//----------------
	time[0] = (cur_time32 >> 16) & 0xFF;
	time[1] = (cur_time32 >> 8 ) & 0xFF;
	time[2] = (cur_time32 & 0xFF) + inc_sec;
	//----------------
	quo = time[2] / 60;
	time[2] -= (quo * 60);
	//----------------
	time[1] += (inc_min + quo);
	quo = time[1] / 60;
	time[1] -= (quo * 60);
	//----------------
	time[0] += quo;
	time[0] %= 24; // 0~23	
	//----------------
	time32  = time[0]; time32 <<= 8;	
	time32 |= time[1]; time32 <<= 8;
	time32 |= time[2];
	//----------------
	return time32;
}


// hour/min/sec 를 INT32U로..
// 0x010203 is "01:02:03" 
INT32U Get_Cur_SysTime_3B(void)
{
	INT32U cur32;

	cur32 =  m_St.Ru_Sys_Date[3]; cur32 <<= 8;
	cur32 |= m_St.Ru_Sys_Date[4]; cur32 <<= 8;
	cur32 |= m_St.Ru_Sys_Date[5];

	return cur32;
}

// cur32 가 end32 시간과 그 이후 1분 사이에 들어오면, Report Time으로 판정.
INT8U Is_END_Time( INT32U cur32, INT32U end32 )
{
	INT32U start32;
	INT32U stop32;

	start32 = end32;
	stop32  = Calc_Time_Offset(end32, 0, 60); 

	if(start32 <= stop32) // 시작시간이 작은경우.
	{
		if((cur32 >= start32) && (cur32 < stop32)) 
			return 1;
		else return 0;

	}
	else // 시작시간이 더 클경우.
	{
		if((cur32 > stop32) && (cur32 <= start32))
			return 0;
		else return 1;
	}
}


// return mimutes
INT16U Get_Stat_Period(void)
{
	switch(m_Eep.Statis_Rpt_Prd)
	{
		case 1: return 5;  // 2019.08.27 : Added.
		case 2: return 15; // 2019.08.27 : Added.
		case 3: return 30;
		case 4: return 60;
	
		default: return 30; // default 30[m]
	}
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------


/*
date -d 'yesterday'		# 어제
date -d '1 day ago'		# 1일전 = 어제
date -d '2 day ago'		# 2일전
date -d '35 day ago'	# 20일전
*/
//-----------------------------------------------------------------------------
// pStart ~ pStop 에서, cur32에서 period_min 지난 다음 시간.
void Inc_Next_Time( INT32U* pStart, INT32U* pStop, INT32U cur32, INT16U period_min )
{
	INT32U start32, stop32;
	INT8U find;
	INT16U cnt = 0;
	INT16U max_cnt = 1;

	start32 = *pStart;
	stop32  = *pStop;

	if(period_min > 0)
	{
		max_cnt = 60*24/period_min;
		//D_Printf( DEBUG_REMS, "{Inc_Next_Time}: max_cnt %d\n", max_cnt);
	}

	do 
	{
		if(start32 <= stop32) // 시작시간이 작은경우.
		{
			if((cur32 >= start32) && (cur32 < stop32)) find = 1;
			else find = 0;
		}
		else // 시작시간이 더 클경우. 부등호 주의...해야.
		{				
			if((cur32 >= stop32) && (cur32 < start32)) find = 0;
			else find = 1;
		}

		if ( !find ) // next sect.
		{
			start32 = stop32;
			stop32 = Calc_Time_Offset(start32, period_min, 0);

			cnt++;
			if((cnt%50)==0) OSTimeDly( OS_DELAY_10msec );
		}
		else
		{
			*pStart = start32;
			*pStop = stop32;
		}
	} while ((find==0) && (cnt < max_cnt)); // 15분 단위일때는 96번이 max

	if(find==0)
		print_agent( DEBUG_REMS, VT_Color_Err, "Fail {Inc_Next_Time}: max_cnt %d", max_cnt);
}

//-----------------------------------------------------------------------------
// 통계 시작 시간(매시 0분 + [통계 주기 x N] )과 통계주기에 해당하는 구간(sect) 중에서,
// 현재 시간이 위치하는, sect를 찾는다.
void Inc_Next_Start_time(void)
{
	INT32U start32, cur32, stop_32;	
	INT16U period_min = Get_Stat_Period();

	//------------------
	// init start/stop
	cur32   = Get_Cur_SysTime_3B();	
	start32 = m_Env.STATIS.Time32_RepRef_Hour & 0xFF0000; // sec는 0으로.	
	stop_32 = Calc_Time_Offset(start32, period_min, 0);
	
	if(period_min != 0)
		Inc_Next_Time( &start32, &stop_32, cur32, period_min );

	// 2019.09.03
	#if 1
	type32 val32;
	val32.uD32 = start32;
	m_St.stat_start_time[0] = m_St.Ru_Sys_Date[0];
	m_St.stat_start_time[1] = m_St.Ru_Sys_Date[1];
	m_St.stat_start_time[2] = m_St.Ru_Sys_Date[2];	
	m_St.stat_start_time[3] = val32.uD8[2];
	m_St.stat_start_time[4] = val32.uD8[1];
	m_St.stat_start_time[5] = val32.uD8[0];
	#else
	memcpy(m_St.stat_start_time, m_St.Ru_Sys_Date, 6);	
	m_St.stat_start_time[5] = 0; // time sec는 0으로.
	#endif	

	m_Env.STATIS.time32[_start_] = start32;
	m_Env.STATIS.time32[_stop_]  = stop_32;	
	Disp_Stat_Time(_start_);
}
//-----------------------------------------------------------------------------
// 통계기준시간과 통계주기에 해당하는 구간(sect) 중에서,
// 현재 시간이 위치하는, sect를 찾는다.
void Inc_Next_Rpt_time(void)
{
	INT32U start32, cur32, stop_32;	
	INT16U period_min = Get_Stat_Period();

	//------------------	

	// 보고 시간 기준 , 통계보고 주기 마다 체크. 
	start32 = m_Env.STATIS.Time32_Rep_HourMin & 0xFFFF00; // 2015.12.15 : time sec는 0으로.
	stop_32 = Calc_Time_Offset(start32, period_min, 0);	

	cur32 = Get_Cur_SysTime_3B();	
	if(period_min != 0) // 통계보고 안함 외에...
		Inc_Next_Time( &start32, &stop_32, cur32, period_min );
		
	m_Env.STATIS.time32[_rpt_] = stop_32;
	Disp_Stat_Time(_rpt_);

	//-------------------------------------------------------------------------
	// rpt time 이 statistic range 안에 있으면, 다음 주기가 rpt time 이 되도록 수정 
	// stop 시간 미만일때 ( stop 시간과 같은 때는 rpt time을 변경하지 않음.)
	if( Is_Time_In_Range_xx( m_Env.STATIS.time32[_start_], m_Env.STATIS.time32[_stop_], m_Env.STATIS.time32[_rpt_] ))
	{
		D_Printf( DEBUG_REMS, "! Adjust RPT Time.\n");
		
		stop_32 = Calc_Time_Offset(m_Env.STATIS.time32[_rpt_], period_min, 0);

		m_Env.STATIS.time32[_rpt_] = stop_32;
		Disp_Stat_Time(_rpt_);
	}
	//-------------------------------------------------------------------------	
}
//-----------------------------------------------------------------------------




static INT8U prev_Sys_Time_Start = 0;

void Proc___gREMS_Stat(void)
{
	INT32U cur32, ref32;
	type32 time32;	
	
	//-------------------------------------------------------------------
	if(m_Eep.Statis_Rpt_Prd == 0) // 2019.09.17
	{
		Stop_Stat();
		return;
	}
	//-------------------------------------------------------------------	
	if(Is_Stat_Enable() == 0)
	{
		Clr_REMS_Struct();
		Enable_Stat();
	}
	//---------------------------
	// IF "system_time_start"  OR  "ref_time/period is changed"...
	// not start or "통계보고 주기/기준시" 바뀌면, 재시작.
	//---------------------------
	if( (prev_Sys_Time_Start == 0) || m_Env.STATIS.time_change )
	{		
		if(m_Env.STATIS.time_change)
		{
			print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "stat. user-ctrl : Ref %02d:%02d, Prd 0x%X", 
				m_Eep.Statis_Rpt_Ref[0], m_Eep.Statis_Rpt_Ref[1],
				m_Eep.Statis_Rpt_Prd);
		}
		
		prev_Sys_Time_Start = 1;
		m_Env.STATIS.time_change = 0;

		//------------------------------------------
		// start  time : statistic 기준시(0x5600)의 매시 0분 0초. 
		// stop   time : 매시 0분 0 초 + 통계주기
		// report time : statistic report 기준 시/분(0x5600) + 통계 주기 + ro 분산.
		m_Env.STATIS.Time32_RepRef_Hour = m_Eep.Statis_Rpt_Ref[0] << 16; // "hour" only
		
		ref32 = m_Eep.Statis_Rpt_Ref[0]; ref32 <<= 8;
		ref32 |= m_Eep.Statis_Rpt_Ref[1]; ref32 <<= 8;
		m_Env.STATIS.Time32_Rep_HourMin = ref32;		
		//------------------------------------------

		time32.uD32 = m_Env.STATIS.Time32_Rep_HourMin;		
		D_Printf( DEBUG_REMS, " STAT Report Refer %02d:%02d:%02d\n",
			time32.uD8[2], time32.uD8[1], time32.uD8[0]);

		Inc_Next_Start_time(); // every hour from 0		
		Inc_Next_Rpt_time();   // from report reference time
		//--------------------------
		// 1회 skip
		m_Env.STATIS.report_skip = 1;
		//--------------------------

		print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[REMS] stat. report will be skipped one-time.");
	}	
	//--------------------------------------------------------


	//--------------------------------------------------------
	// 현재의 구간을 벗어났는지 체크.
	//--------------------------------------------------------
	cur32 = Get_Cur_SysTime_3B();	
	//--------------------------------------------------------
	if(Is_END_Time(cur32, m_Env.STATIS.time32[_stop_]))
	{
		if(m_Env.STATIS.report_skip)
		{
			m_Env.STATIS.report_skip = 0;
			print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[REMS] stat. report skipped..");
		}
		else
			Make_Stat_Pkt( m_St.stat_start_time, m_Eep.Statis_Rpt_Prd );
		
		Inc_Next_Start_time();
		
		/*
		* 2019.08.06 : init bip-err-accumulted, hard-clipping-accumulated
		*/
		/*
		2020.07.08 : 통계보고 후, 누적 초기화 하지 않도록 수정.
		(BIP와 Hard Clipping Count : 통계 보고 후 초기화 했던 부분을 계속 누적하도록 변경 )
		////Init__HardClipCnt_Accum();
		////Init__BIP_ErrCnt_Accum();
		*/
	}
	/*
	else
	{
		time32.uD32 = cur32;
		time3B.uD32 = m_Env.STATIS.time32[_stop_];
		
		D_Printf( DEBUG_REMS, " STAT Section: curr %02d:%02d:%02d, end %02d:%02d:%02d\n",
			time32.uD8[2], time32.uD8[1], time32.uD8[0],
			time3B.uD8[2], time3B.uD8[1], time3B.uD8[0]);
	}
	*/
	//--------------------------------------------------------
	if(Is_END_Time( cur32, m_Env.STATIS.time32[_rpt_] ))
	{
		Insert_Statis_Q();		
		Inc_Next_Rpt_time();
	}
	/*
	else
	{
		time32.uD32 = cur32;
		time3B.uD32 = m_Env.STATIS.time32[_rpt_];
		
		D_Printf( DEBUG_REMS, " STAT Section: curr %02d:%02d:%02d, rpt %02d:%02d:%02d\n",
			time32.uD8[2], time32.uD8[1], time32.uD8[0],
			time3B.uD8[2], time3B.uD8[1], time3B.uD8[0]);
	}
	*/
	//--------------------------------------------------------
}


/*
--------------------------------------------------------
2019.11.18 : Proc___gREMS_Stat() 에서 1_time_1_day 부분망 분리.
--------------------------------------------------------
*/
void Proc___Chk_gREMS_1_time_1_day(void)
{	
	static INT8U auto_rpt_after_reset = 0;
	INT8U rpt_req;
	INT32U cur32;	
	INT32U start32, stop_32;	// "0x0시분초"

	cur32 = Get_Cur_SysTime_3B();	
	//--------------------------------------------------------
	// 1회/1일 : 제어 설정값 자동보고, config 값 자동보고.
	//--------------------------------------------------------
	// 2019.07.10 : 자동보고 기준시 added
	start32 = m_Eep.REMS_Auto_Rpt_Time[0]; start32 <<= 8;
	start32 |= m_Eep.REMS_Auto_Rpt_Time[1]; start32 <<= 8;
	
	stop_32 = Calc_Time_Offset(start32, 0, 20); // 0분20초 안이면, 자동보고.

	if(start32 <= stop_32) // 시작시간이 작은경우.
	{
		if((start32 <= cur32) && (cur32 < stop_32)) rpt_req = 1;
		else rpt_req = 0;
	}
	else // 시작시간이 더 클경우.
	{
		if((cur32 > stop_32) && (cur32 <= start32)) rpt_req = 0;
		else rpt_req = 1;
	}
	if( rpt_req )
	{		
		if(m_Env.flag_1_time_1_day == 0)
		{
			m_Env.flag_1_time_1_day = 1;
			m_Env.AutoRep_Req[_EMS_CFG__C_] = 1;
			m_Env.AutoRep_Req[_EMS_CFG_nC_] = 1;

			Set__gREMS_local_Time_Request();
			Set__gREMS_GUI_ID_PW_Request();

			print_agent( DEBUG_REMS, VT_BLUE, "[CF_C/CF_nC] auto-rpt one time a day.");
		}
	}
	else m_Env.flag_1_time_1_day = 0;
	
	//--------------------------------------------------
	// REMS : when reset, one-time-auto-report
	//--------------------------------------------------
	if(auto_rpt_after_reset == 0)
	{
		auto_rpt_after_reset = 1;
		m_Env.AutoRep_Req[_EMS_CFG__C_] = 1;
		m_Env.AutoRep_Req[_EMS_CFG_nC_] = 1;
		m_Env.CF_nC_Chk_REQ = 1;
		
		print_agent( DEBUG_REMS, VT_BLUE, "[CF_C/CF_nC] auto_rpt_after_reset.");		
	}
	//--------------------------------------------------------
}

//----------------------------------------------------------------------


#define MAX_BTS_NAME (100) // 2019.11.20 : 30 -> 100
static INT8U str_Bts_Name[MAX_BTS_NAME];

#define REMS_RSP_MAX (150)



// return : 0 실패, non-zero : 해당 AID 의 데이터 length.
/*
INT8U Update_BTS_Name(INT8U* data, INT16U tot_len)
{
	INT16U cnt;
	INT8U end;
	INT8U name_len;	
	
	name_len = data[0];	
	if(name_len > 0)
	{
		memset(str_Bts_Name, 0, MAX_BTS_NAME);
		
		if(name_len > MAX_BTS_NAME)
			name_len = MAX_BTS_NAME;			
		
		cnt = 0; end = 0;
		do
		{		
			// 2019.09.05 : cnt+1 -> cnt
			// 2019.11.20 : cnt -> cnt+1
			str_Bts_Name[cnt] = data[cnt+1]; 

			if(str_Bts_Name[cnt] == 0) // if NULL
			{
				end = 1;
				Wr_Node_Name(str_Bts_Name, cnt+1); // 2019.11.20 : "+1" is null char
				m_Env.is_eep_write = EEP_WRITE_CODE;			
			}
			cnt++;
			if( cnt >= name_len) end = 1;
		}while(end == 0);
		return cnt;
	}
	return 0;
}
*/

/*
국소명 수정 : 2020.01.09
	SubData 1st byte가 기존에는 length 이었는데, 이 부분 삭제됨. 
	SubData 1st byte가 국소명 1st byte 가 됨.
*/
INT8U Update_BTS_Name(INT8U* data, INT16U tot_len)
{
	INT16U cnt;
	INT8U end;
	INT8U name_len;	
	
	name_len = tot_len;	// 2020.01.09 : SubData 1st byte가 국소명 1st byte 가 됨.
	
	if(name_len > 0)
	{
		memset(str_Bts_Name, 0, MAX_BTS_NAME);
		
		if(name_len > MAX_BTS_NAME)
			name_len = MAX_BTS_NAME;			
		
		cnt = 0; end = 0;
		do
		{
			str_Bts_Name[cnt] = data[cnt]; // 2020.01.09 : SubData 1st byte가 국소명 1st byte 가 됨.
			
			if(str_Bts_Name[cnt] == 0) // if NULL
			{
				end = 1;
				Wr_Node_Name(str_Bts_Name, cnt+1); // 2019.11.20 : "+1" is null char
				m_Env.is_eep_write = EEP_WRITE_CODE;			
			}
			cnt++;
			if( cnt >= name_len) end = 1;
		}while(end == 0);
		return cnt;
	}
	return 0;
}



//=================================   USE REMS	=========================================

//-------------------------------------------------------------------------------
// Auto Report 할 packet을 큐에 넣는다.
void Set__alm_que_idx_SuperCapChange(void)
{
	alm_que_idx_super_cap_change = alm_que_st;
}

INT8U Is__alm_que_idx_SuperCapChange(void)
{
	if(alm_que_idx_super_cap_change != 0xFFFF)
	{
		if(alm_que_idx_super_cap_change == alm_que_ed)
		{
			alm_que_idx_super_cap_change = 0xFFFF; // init
			return 1;
		}
	}	

	return 0;
}

void Insert_Auto_Rpt_Q( INT8U* data, INT16U data_len, INT8U cmd, INT8U cur, INT8U tot )
{
	INT16U len = 0; // must 0.

#ifdef __Seperate_AlarmLog__From__AutoReport__
	/*
	cmd 종류(CMD2_ALARM_LOC_RPT, CMD2_CFG__C_AUTO_RPT, CMD2_CFG_nC_AUTO_RPT)에 
	관계없이
	*/
	if(m_Env.gREMS_Noti_Ready == 0) return;
#endif

	if( alm_que_co >= (MAX_AUTO_RPT_QUE-1) ) return;

	//print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[RCU] Insert_Auto_Rpt_Q--(CMD 0x%X)", cmd);

	pthread_mutex_lock(&Auto_Rpt_Q_Sem);

	// Make Alarm data
	len = 0;
	if(data != NULL)
	{
		memcpy( &Auto_Rep_Pkt[alm_que_st].data[len], data, data_len );
		len += data_len;
	}
	//------------------------------------------------
	Auto_Rep_Pkt[alm_que_st].len = len;
	Auto_Rep_Pkt[alm_que_st].cmd = cmd;	
	Auto_Rep_Pkt[alm_que_st].cur = cur;
	Auto_Rep_Pkt[alm_que_st].tot = tot;
	Auto_Rep_Pkt[alm_que_st].seq16 = Auto_Rep_Pkt_Seq;	
	//------------------------------------------------
	

	print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[RCU] Insert_Auto_Rpt_Q : alm_que_st %d, len %d, CMD 0x%02X, Auto_Rep_Pkt_Seq %d", 
				alm_que_st, len, cmd, Auto_Rep_Pkt_Seq);

	alm_que_st++;
	alm_que_co++;
	alm_que_st %= MAX_AUTO_RPT_QUE; 

	if(cur == tot) 
	{
		Auto_Rep_Pkt_Seq++;
		print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[RCU] Inc Auto_Rep_Pkt_Seq %d", Auto_Rep_Pkt_Seq);
	}

	pthread_mutex_unlock(&Auto_Rpt_Q_Sem);
}

void Insert_Auto_Log_Q( INT8U* data, INT16U data_len, INT8U cmd, INT8U cur, INT8U tot )
{
#ifdef __Seperate_AlarmLog__From__AutoReport__
	INT16U len = 0; // must 0.

	if( log_que_co >= (MAX_AUTO_LOG_QUE-1) ) return;

	//print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[RCU] Insert_Auto_Log_Q--(CMD 0x%X)", cmd);

	pthread_mutex_lock(&Auto_Log_Q_Sem);

	// Make Alarm data
	len = 0;
	if(data != NULL)
	{
		memcpy( &Auto_Wr__Log[log_que_st].data[len], data, data_len );
		len += data_len;
	}
	//------------------------------------------------
	Auto_Wr__Log[log_que_st].len = len;
	Auto_Wr__Log[log_que_st].cmd = cmd;	
	Auto_Wr__Log[log_que_st].cur = cur;
	Auto_Wr__Log[log_que_st].tot = tot;
	Auto_Wr__Log[log_que_st].seq16 = Auto_Wr__Log_Seq;	
	//------------------------------------------------
	
	print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[RCU] Insert_Auto_Log_Q : alm_que_st %d, len %d, CMD 0x%02X, Auto_Wr__Log_Seq %d", 
				log_que_st, len, cmd, Auto_Wr__Log_Seq);

	log_que_st++;
	log_que_co++;
	log_que_st %= MAX_AUTO_LOG_QUE; 

	if(cur == tot) 
	{
		Auto_Wr__Log_Seq++;
		print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "[RCU] Inc Auto_Wr__Log_Seq %d", Auto_Wr__Log_Seq);
	}

	pthread_mutex_unlock(&Auto_Log_Q_Sem);
#else
	(void)data; (void)data_len; (void)cmd; (void)cur; (void)tot;
#endif
}

//-------------------------------------------------------------------------------
#define MAX_STATIS_Q_NUM (3)

static AUTO_REP_PKT Statis_Q[MAX_STATIS_Q_NUM];
// Auto Report 할 packet을 큐에 넣는다.
void Init_Statis_Q(void)
{
	INT8U i;

	for (i = 0; i<MAX_STATIS_Q_NUM; i++)
	{
		Statis_Q[i].use = 0;
	}	
}

void Make_Statis_Q( INT8U* data_1st, INT16U len_1st, INT8U* data_2nd, INT16U len_2nd, INT8U cmd, INT8U idx, INT8U cur, INT8U tot )
{	
	INT16U len = 0; // must 0.

	if(idx >= MAX_STATIS_Q_NUM) return;
	
	if(data_1st != NULL)
	{
		memcpy( &Statis_Q[idx].data[len], data_1st, len_1st );
		len = len_1st;
		Statis_Q[idx].use = 1;
	}

	if(data_2nd != NULL)
	{
		memcpy( &Statis_Q[idx].data[len], data_2nd, len_2nd );
		len += len_2nd;
		Statis_Q[idx].use = 1;
	}

	Statis_Q[idx].len = len;
	Statis_Q[idx].cmd = cmd;	

	Statis_Q[idx].cur = cur;
	Statis_Q[idx].tot = tot; 
}


// 자동보고 시간이 되어, Statis_Q를 자동보고 Q에 넣는다.
INT8U Insert_Statis_Q(void)
{		
	INT8U i;

	if( alm_que_co >= (MAX_AUTO_RPT_QUE-1) ) 
	{
		D_Printf(DEBUG_REMS, "!! MAX_AUTO_RPT_STAT_QUE.\n");
		return 0;
	}

	pthread_mutex_lock(&Auto_Rpt_Q_Sem);
	
	for(i = 0; i < MAX_STATIS_Q_NUM; i++)
	{
		if (Statis_Q[i].use == 0) 
		{
			// print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "!! STAT_Q 0");
			continue;
		}
		
		memcpy( (INT8U*)&Auto_Rep_Pkt[alm_que_st], (INT8U*)&Statis_Q[i], sizeof(AUTO_REP_PKT) );
		Statis_Q[i].use = 0;

		alm_que_st++;
		alm_que_co++;
		alm_que_st %= MAX_AUTO_RPT_QUE;
	}
	pthread_mutex_unlock(&Auto_Rpt_Q_Sem);

	print_agent( DEBUG_REMS, VT_BOLD_YELLOW, "Insert STAT_Q(alm_que_co %d)", alm_que_co);

	return 1;
}
//-------------------------------------------------------------------------------
// 다음 Auto Report 할 packet을 큐에서 꺼내 pUp_data 에 넣고, 길이를 리턴.
INT16U Get_Next_AutoRpt_From_Que( INT8U* pUp_data, INT8U* pCmd, INT8U* cur, INT8U* tot, INT16U* seq16 )
{	
	INT16U len=0;

	pthread_mutex_lock(&Auto_Rpt_Q_Sem);

	if( alm_que_st != alm_que_ed )
	{		
		memcpy( pUp_data, Auto_Rep_Pkt[alm_que_ed].data, Auto_Rep_Pkt[alm_que_ed].len );
		len = Auto_Rep_Pkt[alm_que_ed].len;
		*pCmd = Auto_Rep_Pkt[alm_que_ed].cmd;

		*cur   = Auto_Rep_Pkt[alm_que_ed].cur; 
		*tot   = Auto_Rep_Pkt[alm_que_ed].tot; 
		*seq16 = Auto_Rep_Pkt[alm_que_ed].seq16; 

		alm_que_ed++;
		alm_que_co--;
		alm_que_ed %= MAX_AUTO_RPT_QUE;
	}

	pthread_mutex_unlock(&Auto_Rpt_Q_Sem);

	return len;
}

INT16U Get_Next_AutoLog_From_Que( INT8U* pUp_data, INT8U* pCmd, INT8U* cur, INT8U* tot, INT16U* seq16 )
{	
	INT16U len=0;

	pthread_mutex_lock(&Auto_Log_Q_Sem);

	if( log_que_st != log_que_ed )
	{		
		memcpy( pUp_data, Auto_Wr__Log[log_que_ed].data, Auto_Wr__Log[log_que_ed].len );
		len = Auto_Wr__Log[log_que_ed].len;
		*pCmd = Auto_Wr__Log[log_que_ed].cmd;

		*cur   = Auto_Wr__Log[log_que_ed].cur; 
		*tot   = Auto_Wr__Log[log_que_ed].tot; 
		*seq16 = Auto_Wr__Log[log_que_ed].seq16; 

		log_que_ed++;
		log_que_co--;
		log_que_ed %= MAX_AUTO_LOG_QUE;
	}

	pthread_mutex_unlock(&Auto_Log_Q_Sem);

	return len;
}






//----------------------------------------------------------------------
// 0x2323 의 Pkt index 들어가는 SUB Data를 만든다.
void Make_REMS_SubData( INT16U* pAid_list, INT16U max_num, REMS_1024_Pkt* pPkt, INT8U port  )
{
	INT16U l=0; // init
	INT16U k, idx;
	INT8U* sdu;
	INT16U len_aid;
	INT8U  cur = 0;
	INT16U max_subdata_len;
	//-----------------------------------------------------
	pPkt->AID[0] = 0x23;
	pPkt->AID[1] = 0x23;
	pPkt->len = 2;
	pPkt->cur = 1;
	pPkt->tot = 1;
	pPkt->aid_cnt = 0;

	for(k=0; k < MAX_PKT_NUM; k++)
		pPkt->size[k] = 0; // init
	
	sdu = pPkt->data;
	//-----------------------------------------------------
	switch(pPkt->cmd)
	{
		case CMD2_ST2_MEA_RPT:
		case CMD_REMS_HIDDEN:
			max_subdata_len = MAX_LEN_SUB_DATA; // MAX_ST2_LEN_SUB_DATA; 2019.09.17.
			break;

		case CMD2_CFG__C_AUTO_RPT:
		case CMD2_CFG_nC_AUTO_RPT:
		case CMD2_CFG_nC_REQ:
		case CMD2_CFG__C_SET_REQ:
		case CMD2_PLD_SET_REQ:
			max_subdata_len = MAX_LEN_SUB_DATA;
			break;
			
		default:
			pPkt->use = 0;
			return;
	}		
	//----------------------
	l=0; // init
	//----------------------
	// 2019.09.17
	//----------------------
		switch(pPkt->cmd)
		{
			case CMD2_ST2_MEA_RPT:
			case CMD2_CFG_nC_REQ:
			case CMD2_CFG__C_SET_REQ:			
				sdu[l++] = 0x00;
				sdu[l++] = 0x01;
				sdu[l++] = RU_ALARM_CNT*2;
				memcpy(&sdu[l], RU_ARM_SUPP,	RU_ALARM_CNT); l += RU_ALARM_CNT;
				memcpy(&sdu[l], &m_St.Last_Alm, RU_ALARM_CNT); l += RU_ALARM_CNT;			
				break;
		}
	//----------------------
	//----------------------

	//-----------------------------------------------------	
	for( k=0; k < max_num; k++ ) // AID max count
	{
		idx = pAid_list[k];

		if ( idx == 0xFFFF ) break;
		if ( cur >= MAX_PKT_NUM ) break;

		len_aid = Make_SKT_AID( idx, &sdu[l], port );

		//sprintf( dbgstr, "len_aid(%d, %d, %d)", len_aid, l, cur );		

		if ((l + len_aid) <= max_subdata_len)
		{
			l += len_aid;
			pPkt->aid_cnt++;
		}
		else
		{
			D_Printf(DEBUG_REMS, "cur(%d), l(%d)", cur, l );			

			pPkt->size[cur++] = l;
			l = 0; // init
			pPkt->tot = cur;			

			sdu = &pPkt->data[UDP_MAX_LEN * cur];				
			k--; // k 번째 retry 를 위해.

			// from 2nd packet and for below commands
			if((pPkt->cmd == CMD2_ST2_MEA_RPT)||(pPkt->cmd == CMD_REMS_HIDDEN))
				max_subdata_len = MAX_LEN_SUB_DATA;
		}
	}	

	if(l > 0)
	{
		pPkt->size[cur++] = l;
		pPkt->tot = cur;
		l = 0; // init	
	}

	//sprintf( dbgstr, "aid_cnt(%d/%d)", pPkt->aid_cnt, max_num );
}





static PROTOCOL_FRAME rems_txf;
INT16U Proc___gREMS_RSP_1k(void)
{
	INT16U len, l;
	INT16U p;
	INT8U totPkt, curPkt;
	INT16U size; // size of subdata;

	
	pthread_mutex_lock(&gREMS_Pkt_1k_Sem);	

	if(gRemsRspPkt_1k.use)
	{
		for(p=0; p<gRemsRspPkt_1k.tot; p++)
		{
			size = gRemsRspPkt_1k.size[p];	
			l = 0; // init
			/*
			//if((gRemsRspPkt_1k.cmd == CMD2_ST2_MEA_RPT)||(gRemsRspPkt_1k.cmd == CMD_REMS_HIDDEN))
			//{
			//	if(p==0)
			//	{
			//		//----------------------
			//		// ALARM : 0x0001 AID
			//		//----------------------			
			//		rems_txf.data[l++] = 0x00;
			//		rems_txf.data[l++] = 0x01;
			//		rems_txf.data[l++] = RU_ALARM_CNT*2;
			//		memcpy( &rems_txf.data[l], RU_ARM_SUPP, RU_ALARM_CNT );
			//		l += RU_ALARM_CNT;
			//		memcpy( &rems_txf.data[l], &m_St.Last_Alm, RU_ALARM_CNT );
			//		l += RU_ALARM_CNT;
			//		//----------------------					
			//	}
			//}
			*/
			totPkt = gRemsRspPkt_1k.tot;
			curPkt = p+1;
			
			print_agent(DEBUG_REMS, VT_BOLD_MAGENTA, "Pkt(%d/%d), CMD 0x%X ---- ", curPkt, totPkt, gRemsRspPkt_1k.cmd);
			
			memcpy(&rems_txf.data[l], &gRemsRspPkt_1k.data[p*UDP_MAX_LEN], size);
			l += size;

			Get__gREMS_My_SUBID(rems_txf.SUB_ID);
			len = Make_Frame( gRemsRspPkt_1k.cmd, _RCODE_, &rems_txf, l, gRemsRspPkt_1k.seqNo16, totPkt, curPkt);
			
			Send_UDP_Node_Data( COMM_UDP_NODE, gRemsRspPkt_1k.ip, gRemsRspPkt_1k.port16, (INT8U*)&rems_txf, len);
			OSTimeDly(OS_DELAY_100msec);
		}		
		gRemsRspPkt_1k.use = 0;
		gRemsRspPkt_1k.tot = 0;
	}

	pthread_mutex_unlock(&gREMS_Pkt_1k_Sem);	
	return 0;
}



static PROTOCOL_FRAME rems_Mea_txf;
INT16U Proc___gREMS_RSP_1k_Mea(void)
{
	INT16U len, l;
	INT16U p;
	INT8U totPkt, curPkt;
	INT16U size; // size of subdata;

	
	pthread_mutex_lock(&gREMS_Pkt_1k_Sem);	

	if(gRemsRspPkt_1k_Mea.use)
	{
		for(p=0; p<gRemsRspPkt_1k_Mea.tot; p++)
		{
			size = gRemsRspPkt_1k_Mea.size[p];	
			l = 0; // init
			
			totPkt = gRemsRspPkt_1k_Mea.tot;
			curPkt = p+1;
			
			print_agent(DEBUG_REMS, VT_BOLD_MAGENTA, "Pkt(%d/%d), CMD 0x%X ---- ", curPkt, totPkt, gRemsRspPkt_1k_Mea.cmd);
			
			memcpy(&rems_Mea_txf.data[l], &gRemsRspPkt_1k_Mea.data[p*UDP_MAX_LEN], size);
			l += size;

			Get__gREMS_My_SUBID(rems_Mea_txf.SUB_ID);
			len = Make_Frame( gRemsRspPkt_1k_Mea.cmd, _RCODE_, &rems_Mea_txf, l, gRemsRspPkt_1k_Mea.seqNo16, totPkt, curPkt);
			
			Send_UDP_Node_Data( COMM_UDP_NODE, gRemsRspPkt_1k_Mea.ip, gRemsRspPkt_1k_Mea.port16, (INT8U*)&rems_Mea_txf, len);
			OSTimeDly(OS_DELAY_100msec);
		}		
		gRemsRspPkt_1k_Mea.use = 0;
		gRemsRspPkt_1k_Mea.tot = 0;
	}

	pthread_mutex_unlock(&gREMS_Pkt_1k_Sem);	
	return 0;
}

//----------------------------------------------------------------------





//----------------------------------------------------------------------

static OS_TIMER ACK_timer; 
static INT8U send_retry_cnt = 0;
#define gREMS_Auto_Report_Retry_Max (4) // 2019.06.26
static INT8U f_Send_REP = 0;

void Init_Auto_Rpt_Flag(void)
{
	send_retry_cnt = 0;
	f_Send_REP = 0;
}


// REMS Auto Report 에 사용. 

void Send_gREMS_AutoReport_Data( INT8U* alm_data, INT16U alm_len, INT8U cmd, INT8U cur, INT8U tot, INT16U seq16, INT8U is_retry )
{
	INT16U len=0;	
	INT16U seqNo16;
	static INT16U seqNo16_prev = 0xFFFF;
#ifdef __Seperate_AlarmLog__From__AutoReport__ // 
// do nothing
#else	
	static INT16U seqNo16_p = 0xFFFF;
#endif

	Get__gREMS_My_SUBID(rems_auto_txf.SUB_ID);
	
	// Make Alarm Data
	memcpy( &rems_auto_txf.data[len], alm_data , alm_len);

	if(seqNo16_prev != seq16)	
		seqNo16 = Get_Tx_Frame_SeqNo(COMM_REMS); // inc and return
	else 	
		seqNo16 = Get_Tx_Frame_SeqNo_Cur(COMM_REMS); // return current

	D_Printf (DEBUG_REMS, VT_CYAN "[Send_Auto_Report] len %d, cmd 0x%X, %d/%d/%d", 
		alm_len, cmd,
		seqNo16_prev, seq16, seqNo16 );

	D_Printf (DEBUG_REMS, VT_CYAN "[Send_Auto_Report] REMS IP/port %d.%d.%d.%d/%d", 
		AufoConfigFlag.Recv_Ip2[0],
		AufoConfigFlag.Recv_Ip2[1],
		AufoConfigFlag.Recv_Ip2[2],
		AufoConfigFlag.Recv_Ip2[3],
		AufoConfigFlag.gREMS_Port16);

	seqNo16_prev = seq16;
	
	//-----------------------------------------------------------
	switch(cmd)
	{
		case CMD2_CFG_nC_AUTO_RPT:
		case CMD2_CFG__C_AUTO_RPT:
			D_Printf(DEBUG_REMS, "[->REMS] %s(0x%2X)", Get_CMD_Str(cmd), cmd);
			m_AutoRpt_Info.cmd = cmd;
			m_AutoRpt_Info.pkt_num = cur; // init
			m_AutoRpt_Info.pkt_tot = tot; // init
			memcpy( m_AutoRpt_Info.subid, rems_auto_txf.SUB_ID, 3);
			//m_Alm_Rpt_Info.cmd = 0; // init
			break;

		case CMD2_ALARM_LOC_RPT:
		case CMD2_SUPP_ALARM_RPT:
		case CMD2_STATISTIC_RPT:
			D_Printf(DEBUG_REMS, "[->REMS] %s(0x%2X)", Get_CMD_Str(cmd), cmd);
			m_Alm_Rpt_Info.cmd = cmd;
			m_Alm_Rpt_Info.pkt_num = cur; // init
			m_Alm_Rpt_Info.pkt_tot = tot; // init
			memcpy( m_Alm_Rpt_Info.subid, rems_auto_txf.SUB_ID, 3);
			//m_AutoRpt_Info.cmd = 0;
			break;

		default: 			
			//m_AutoRpt_Info.cmd = 0;
			//m_Alm_Rpt_Info.cmd = 0;
			break;
	}

	//-----------------------------------------------------------
	len = Make_Frame( cmd, _RCODE_, (PROTOCOL_FRAME*)&rems_auto_txf, alm_len, seqNo16, tot, cur );	
	//-----------------------------------------------------------
#ifdef __Seperate_AlarmLog__From__AutoReport__ // 
	// do nothing
#else
	switch(cmd)
	{
		case CMD2_ALARM_LOC_RPT:
		case CMD2_SUPP_ALARM_RPT:
			if(seqNo16_p != seqNo16)
			{
				if(is_retry == 0) // if not retry
				{
					print_agent(DEBUG_REMS, VT_CYAN, "[Send_Auto_Report] Wr AlmPkt_to_log, %d/%d", 
							seqNo16_p, seqNo16);
					Wr_Alm_Pkt_to_log(&rems_auto_txf);
				}
				seqNo16_p = seqNo16;
			}
			break;
	}
#endif	
	// 2019.09.20 : 'm_Alm_Rpt_Info', 'm_AutoRpt_Info' 값 갱신 후, Send 하도록 수정.
	Send_gREMS_Data( COMM_UDP_REMS, AufoConfigFlag.Recv_Ip2, AufoConfigFlag.gREMS_Port16, (INT8U*)&rems_auto_txf, len );		
	//-----------------------------------------------------------
}

/*
-----------------------------------------------------------
2019.11.19
Noti_IP_1/2 모두 해당

[0] eMMC 에 저장된 Noti_IP_1/2 가 invalid 하면, default IP(192.224.0.1) 사용.

VSS Channel 읽은 Noti_IP_1/2 가
[1] 0.0.0.0 이거나 255.255.255.255 이면 invalid
[2] Broadcast IP 이면 invalid
[3] 그외 valid
-----------------------------------------------------------
*/
INT8U Is__Valid_IP(INT8U disp, INT8U* ipaddr)
{
	INT8U ip_00[4] = {0,0,0,0};
	INT8U ip_FF[4] = {255,255,255,255};
	INT8U ip_broadcast_ss[4] = {255,0,0,0}; // SamSung
	INT8U ip_broadcast_ne[4] = {255,255,0,0}; // Nokia/Ericson
	INT8U valid;
	
	     if(memcmp(ipaddr, ip_00, 4)==0) valid = 0; // invalid
	else if(memcmp(ipaddr, ip_FF, 4)==0) valid = 0; // invalid	 
	else if(memcmp(ipaddr, ip_broadcast_ss, 4)==0) valid = 0; // invalid	 
	else if(memcmp(ipaddr, ip_broadcast_ne, 4)==0) valid = 0; // invalid	 
	else valid = 1; // valid
	
	if(disp && (valid == 0)) // invalid
	{
		print_agent(DEBUG_REMS, VT_BOLD_RED, "[Check IP] IP(%d.%d.%d.%d) Invaild, (%d)",		
				ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], disp);	
	}

	return valid;
}



/*
Module Inventory 자동 보고
----------------------------
중계기 Reset 제어 후 ON 시, 
관리 대상 Module 교체 시
*/
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Send_Auto_Inventory( void )
{
	INT16U len=0;	

	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16;
	
	Get__gREMS_My_SUBID(rems_auto_txf.SUB_ID);	
	len = Get_Inventory(rems_auto_txf.data);	
	seqNo16 = Get_Tx_Frame_SeqNo(COMM_UDP_REMS);	
	len = Make_Frame(CMD_AUTO_INVENTORY, _RCODE_, (PROTOCOL_FRAME*)&rems_auto_txf, len, seqNo16, totPkt, curPkt);	
	
	Send_gREMS_Data(COMM_UDP_REMS, AufoConfigFlag.Recv_Ip2, AufoConfigFlag.gREMS_Port16, (INT8U*)&rems_auto_txf, len );
	print_agent(DEBUG_SYSTEM, VT_CYAN, "Auto Inventory.\n");
}


void Send_Auto_Inventory_By_Update( void )
{
	INT16U len=0;	

	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16;


	len = CheckInvetoryUpdate(rems_auto_txf.data);	
	if(len <= 0) return;
	
	Get__gREMS_My_SUBID(rems_auto_txf.SUB_ID);	
	seqNo16 = Get_Tx_Frame_SeqNo(COMM_UDP_REMS);	
	len = Make_Frame(CMD_AUTO_INVENTORY, _RCODE_, (PROTOCOL_FRAME*)&rems_auto_txf, len, seqNo16, totPkt, curPkt);	
	
	Send_gREMS_Data(COMM_UDP_REMS, AufoConfigFlag.Recv_Ip2, AufoConfigFlag.gREMS_Port16, (INT8U*)&rems_auto_txf, len );
	print_agent(DEBUG_SYSTEM, VT_CYAN, "Auto Inventory by update. %d\n", len);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static INT8U gREMS_local_time_req = 1;

// local-time request one-time a day
void Set__gREMS_local_Time_Request(void)
{
	gREMS_local_time_req = 1;
}
void Clr__gREMS_local_Time_Request(void)
{
	gREMS_local_time_req = 0;
}
INT8U IS___gREMS_local_Time_Request(void)
{
	return gREMS_local_time_req;
}
void Send_LocalTime_Req(void)
{
	INT16U len=0;	

	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16;
	
	Get__gREMS_My_SUBID(rems_auto_txf.SUB_ID);	
	seqNo16 = Get_Tx_Frame_SeqNo(COMM_UDP_REMS);	
	len = Make_Frame(CMD2_LocalTime_REQ, _RCODE_, (PROTOCOL_FRAME*)&rems_auto_txf, 0, seqNo16, totPkt, curPkt);		
	Send_gREMS_Data(COMM_UDP_REMS, AufoConfigFlag.Recv_Ip2, AufoConfigFlag.gREMS_Port16, (INT8U*)&rems_auto_txf, len);
	//print_agent(DEBUG_SYSTEM, VT_CYAN, "Send_LocalTime_Req");
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static INT8U gREMS_GUI_Id_Pw_req = 1;

// local-time request one-time a day
void Set__gREMS_GUI_ID_PW_Request(void)
{
	gREMS_GUI_Id_Pw_req = 1;
}
void Clr__gREMS_GUI_ID_PW_Request(void)
{
	gREMS_GUI_Id_Pw_req = 0;
}
INT8U IS___gREMS_GUI_ID_PW_Request(void)
{
	return gREMS_GUI_Id_Pw_req;
}
void Send_to_gREMS_GUI_ID_PW_Req(void)
{
	INT16U len=0;	

	INT8U totPkt = 1, curPkt = 1;
	INT16U seqNo16;
	
	Get__gREMS_My_SUBID(rems_auto_txf.SUB_ID);	
	seqNo16 = Get_Tx_Frame_SeqNo(COMM_UDP_REMS);	
	len = Make_Frame(CMD2_GUI_ID_PW, _RCODE_, (PROTOCOL_FRAME*)&rems_auto_txf, 0, seqNo16, totPkt, curPkt);		
	Send_gREMS_Data(COMM_UDP_REMS, AufoConfigFlag.Recv_Ip2, AufoConfigFlag.gREMS_Port16, (INT8U*)&rems_auto_txf, len);
	//print_agent(DEBUG_SYSTEM, VT_CYAN, "Send_GUI_ID_PW_Req");
}






//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=




	
static INT8U up_pkt[BUF_SIZE+20]; // 알람 패킷 데이터 크기보다 크게.  
static INT8U up_pkt_cmd;
static INT16U  up_len16;

void Send_Auto_Report(void)
{
	INT16U  ACK_Timeout_ms;
	static  INT8U cmd = 0;
	INT8U   init_req = 0;
	static INT8U   cur, tot;
	static INT16U  seq16;	
	INT8U   i, chkcnt = 0;

	//----------------------------------------------------
	// Send next packet from Q	
	if ((send_retry_cnt==0)||(send_retry_cnt > gREMS_Auto_Report_Retry_Max))
	{	
		// init
		//-----------------------
		for(i=0; i<ALM_RPT_Chk_Max; i++)
			m_Alm_Rpt_Info.aid16[i] = 0xFFFF;
		//-----------------------
		// Super Cap. 교체
		if(Is__alm_que_idx_SuperCapChange() && (chkcnt < ALM_RPT_Chk_Max))
		{			
			m_Alm_Rpt_Info.aid16[chkcnt++] = 0x5452; 
		}
		up_len16 = Get_Next_AutoRpt_From_Que(up_pkt, &cmd, &cur, &tot, &seq16);
		if(up_len16)
		{
			up_pkt_cmd = cmd;
			send_retry_cnt=1;
			f_Send_REP=1;

			Send_gREMS_AutoReport_Data( up_pkt, up_len16, up_pkt_cmd, cur, tot, seq16, 0); // retry cnt is 0
			OSTimerReset( &ACK_timer);

			if(up_pkt_cmd == CMD2_ALARM_LOC_RPT)
				print_agent (DEBUG_REMS, VT_BOLD_YELLOW, "[Send_Auto_Report] Alarm [seqNo.%d] Send REQ.", seq16);			
		}				
		else init_req = 1;
	}
	else // retry check
	{
		if(f_Send_REP)
		{
			/*		
			Battery 미실장시 AC 알람 발생시에는 알람 전송 이후 ACK 대기시간을 1초로 합니다.
			즉, 전원 차단시 Super Cap으로 동작시에는 ACK가 미수신되면 1초 주기로 AC 알람을 보고 합니다.
			*/			
			if(up_pkt_cmd == CMD2_ALARM_LOC_RPT) 
			{
				if((m_St.Batt_Insert == 0)&&(m_St.AlmSt.A_0.AC))
					ACK_Timeout_ms = 1000;
				else ACK_Timeout_ms = 5000;				
			}
			else 
			{
				ACK_Timeout_ms = 5000;
			}

			if( OSTimerGet_msec(&ACK_timer) >= ACK_Timeout_ms ) 
			{							
				send_retry_cnt++;
				if(send_retry_cnt > gREMS_Auto_Report_Retry_Max) init_req = 1;
				else
				{				
					print_agent (DEBUG_REMS, VT_BLINK_RED, "CMD %s[seqNo.%d] retry count %d.", 
							Get_CMD_Str(up_pkt_cmd), seq16, send_retry_cnt); // cmd -> up_pkt_cmd
							
					Send_gREMS_AutoReport_Data(up_pkt, up_len16, up_pkt_cmd, cur, tot, seq16, send_retry_cnt); // retry cnt

				#ifdef __TYPE__PRU__
					#ifdef __AC_Fail_TEST_PAU0_RST_PIN_Toggle__
					if(m_St.AlmSt.A_0.AC)
					{
						AC_Fail_TEST_PAU0_RST_PIN_Toggle_Count++;
						if(AC_Fail_TEST_PAU0_RST_PIN_Toggle_Count & 0x1)
							GPIO_Set( GPIO_PAU_0_RST_O, GPIO_VALUE_LOW ); // when odd number
						else
							GPIO_Set( GPIO_PAU_0_RST_O, GPIO_VALUE_HIGH); // when even number
					}
					#endif
				#endif
				}
				OSTimerReset( &ACK_timer);
			}
		}
		else init_req = 1;
	}
	//----------------------------------------------------
	if(init_req)
	{
		f_Send_REP = 0;
		send_retry_cnt = 0;
		OSTimerReset( &ACK_timer); // 2019.09.20 Added
	}
}
//----------------------------------------------------------------------
/*
	2019.10.28
		'CMD2_CFG__C_AUTO_RPT' and 'CMD2_CFG_nC_AUTO_RPT' are discarded..
*/

static INT8U up_pkt_alm_only[BUF_SIZE+20]; // 알람 패킷 데이터 크기보다 크게.  

void Update_Only_Alm_Log(INT8U c) // gREMS Comm 준비안되어도 log write는 함.
{
	INT8U cmd = 0;	
	INT8U cur, tot;
	INT16U auto_q_seq;
	INT16U seqNo16;

#ifdef __Seperate_AlarmLog__From__AutoReport__
	up_len16 = Get_Next_AutoLog_From_Que(up_pkt_alm_only, &cmd, &cur, &tot, &auto_q_seq);
#else
	up_len16 = Get_Next_AutoRpt_From_Que(up_pkt_alm_only, &cmd, &cur, &tot, &auto_q_seq);	
	if((cmd == CMD2_SUPP_ALARM_RPT) && (c == 'A')) // AAU only, skip if supp-alarm
		return;
#endif	

	if(up_len16 > 0)
	{
		switch(cmd)
		{
			case CMD2_ALARM_LOC_RPT:
			case CMD2_SUPP_ALARM_RPT:
				Get__gREMS_My_SUBID(rems_auto_txf.SUB_ID);
				memcpy(rems_auto_txf.data, up_pkt_alm_only, up_len16);
				
				seqNo16 = Get_Tx_Frame_SeqNo(COMM_REMS);
				Make_Frame(cmd, _RCODE_, (PROTOCOL_FRAME*)&rems_auto_txf, up_len16, seqNo16, tot, cur);				
				
				print_agent (DEBUG_REMS, VT_BOLD_YELLOW, "[Update_Only_Alm_Log] Alarm [auto_seq.%d], %c.", auto_q_seq, c);
				Wr_Alm_Pkt_to_log(&rems_auto_txf);

			#ifdef __Seperate_AlarmLog__From__AutoReport__
				// do nothing
			#else	
				f_Send_REP = 0;
				send_retry_cnt = 0;
			#endif
				break;
		}
	}
}
//-------------------------------------------------------
static REMS_1024_Pkt REMS_Cfg_Auto_Pkt;

void Chk_Make_Auto_Report( void )
{
	INT16U len;
	INT8U k;

	//-----------------------------
	if(m_Env.gREMS_Noti_Ready==0)	
		return;
	//----------------------------- 

	//--------------------------------------------------------
	if( m_Env.AutoRep_Req[_EMS_CFG__C_] )
	{		
		m_Env.AutoRep_Req[_EMS_CFG__C_] = 0;
		
		REMS_Cfg_Auto_Pkt.cmd = CMD2_CFG__C_AUTO_RPT;
		Make_REMS_SubData( CFG__C_AID_idx, CFG__C_AID_num, &REMS_Cfg_Auto_Pkt, 0 );

		for (k = 0; k < REMS_Cfg_Auto_Pkt.tot; k++)
		{
			len = REMS_Cfg_Auto_Pkt.size[k];
			REMS_Cfg_Auto_Pkt.cur = k+1;
			Insert_Auto_Rpt_Q( &REMS_Cfg_Auto_Pkt.data[k*UDP_MAX_LEN], len, CMD2_CFG__C_AUTO_RPT, REMS_Cfg_Auto_Pkt.cur, REMS_Cfg_Auto_Pkt.tot );
		}		
	}
	//--------------------------------------------------------
	if( m_Env.AutoRep_Req[_EMS_CFG_nC_] )
	{
		m_Env.AutoRep_Req[_EMS_CFG_nC_] = 0;
		
		REMS_Cfg_Auto_Pkt.cmd = CMD2_CFG_nC_AUTO_RPT;
		Make_REMS_SubData( CFG_nC_AID_idx, CFG_nC_AID_num, &REMS_Cfg_Auto_Pkt, 0 );
		
		for (k = 0; k < REMS_Cfg_Auto_Pkt.tot; k++)
		{
			len = REMS_Cfg_Auto_Pkt.size[k];
			REMS_Cfg_Auto_Pkt.cur = k+1;
			Insert_Auto_Rpt_Q( &REMS_Cfg_Auto_Pkt.data[k*UDP_MAX_LEN], len, CMD2_CFG_nC_AUTO_RPT, REMS_Cfg_Auto_Pkt.cur, REMS_Cfg_Auto_Pkt.tot );
		}
	}
	//--------------------------------------------------------
}
//-------------------------------------------------------





//-------------------------------------------------------


//-------------------------------------------------------

static INT8U Auto_Inv_Rpt_Req = 10; // inventory auto report


static OS_TIMER Timer_stat_Xsec;
void* gREMS_Comm_Task(void *data)
{
	INT32S res;	
	Thread_ARG_Var* p_th_arg;
	////INT8U uda_alarm_send = 0; // 2020.04.10, deleted
	INT16U send_check_cnt = 0;
	INT8U cnt_3s = 0;
	INT8U skip_gREMS; // 2019.10.28
	INT16U disp_cnt = 0;

	
	p_th_arg = (Thread_ARG_Var*)data;
 	Thread_Priority_Init( p_th_arg, 1);		


 	res = pthread_mutex_init(&gREMS_Pkt_1k_Sem, NULL);
 	if (res != 0) 
	{
		syslog(LOG_ERR, "[RCU] REMS_Pkt_1k_Sem Init Fail.");
		exit(EXIT_FAILURE);
	}	
	res = pthread_mutex_init(&gREMS_Pkt_1k_Mea_Sem, NULL);
 	if (res != 0) 
	{
		syslog(LOG_ERR, "[RCU] REMS_Pkt_1k_Sem_Mea Init Fail.");
		exit(EXIT_FAILURE);
	}	
	res = pthread_mutex_init(&Auto_Rpt_Q_Sem, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "Mutex Auto_Rpt_Q_Sem failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_mutex_init(&Auto_Log_Q_Sem, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "Mutex Auto_Log_Q_Sem failed");
		exit(EXIT_FAILURE);
	}
	

	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "gREMS_Comm_Task RUN.");
	OSTimerReset(&Timer_stat_Xsec);	
	
	
	gREMS_Stat_Init();
	Supp_Alm_Timer_Init();
	OSTimeDly(3*OS_DELAY_1sec); // time delay for ready of other-task
	
	//---------------------------------------------------
	while ( m_Env.task_flag[_TASK_gREMS_COMM_] )
	{
		OSTimeDly(OS_DELAY_100msec);
		Update__Task_Count(_TASK_gREMS_COMM_);

	#ifdef __Seperate_AlarmLog__From__AutoReport__ // 2020.07.03
		Update_Only_Alm_Log('S');
	#endif

		/*
		2019.10.18 : if gREMS IP is invalid, Do not Send to gREMS
		2019.10.28 : if CPRI Unlock, Do not Send to gREMS
		*/		
		//-----------------------------------------------------
		skip_gREMS = 0; // init
				
		if(m_St.Is_CPRI_Lock == 0) skip_gREMS |= cbits[1];
		if(Is__Valid_IP(0, AufoConfigFlag.Recv_Ip2)==0) skip_gREMS |= cbits[2]; // default IP 사용으로 인해, 무의미 함.
		if(AufoConfigFlag.Init_Status == 0) skip_gREMS |= cbits[3]; // 2020.01.08
		//-----------------------------------------------------
		if(m_Env.gREMS_Test) skip_gREMS = 0; // init, used when debug_test
		//-----------------------------------------------------
		if(skip_gREMS) // alarm log when unlock
		{
			disp_cnt++;
			if(disp_cnt > 200) // 100[ms] x 200 = 20[s]
			{
				disp_cnt = 0;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, " 'REMS-IP invalid(b2)','cpri-unlock(b1)','AutoCfg.Init_St(b3)', %#x", 
						skip_gREMS);
			}
			Update_Only_Alm_Log('O');
						
			continue; // skip gREMS procedure
		}
		disp_cnt = 0; // init
		//-----------------------------------------------------
		
		Proc___gREMS_RSP_1k();
		Proc___gREMS_RSP_1k_Mea();
		// ALARM SEND
		Send_Auto_Report();

		if( OSTimerGet_msec( &Timer_stat_Xsec ) >= (1*TICK_CO_SEC) )
		{
			OSTimerReset(&Timer_stat_Xsec);
			//-----------------------------
			if(m_Env.gREMS_Noti_Ready) 
			{
				/*------------------------------------------------------
				2019.11.18 : GUI ID/PASS, Local time 요청 주기 3[s] -> 5[s]로 변경
				------------------------------------------------------*/
				cnt_3s++;				
				if(cnt_3s >= 5) // 2019.11.18 : 3[s] -> 5[s]
				{
					cnt_3s = 0;
					
					if(IS___gREMS_local_Time_Request())
						Send_LocalTime_Req();

					if(IS___gREMS_GUI_ID_PW_Request())
						Send_to_gREMS_GUI_ID_PW_Req();
				}
				
				Chk_RebootAlarm_gREMS();
				
				Proc___gREMS_Stat();
				Proc___Chk_gREMS_1_time_1_day();
				
				Chk_Make_Auto_Report();
				
				Chk_Update_of_CF_nC();

				// After sending reset_reason 
				/*
				-------------------------------------------------
				2020.04.10
				(1) After Reset, UDA_alarm_flag가 있으면, UDA Alarm(clock/cpri) = 1 로 시작.
					(Chk_Auto_Reset_By_UDA_Suppress() 함수에서 처리됨.	)
				(2) 2020.04.10 : gREMS에 reset.reason 을 report 한후,	
					UDA Alarm(clock/cpri) = 0 하고, clear report 했던 것을 
					하지 않도록(즉, uda alarm 유지 하도록) 수정.
				(3) 30m 정상 유지 시, UDA clear 되도록 함.(2020.04.09)
				-------------------------------------------------
				2020.04.10
				////if(m_St.ResetReason == 0)
				////{
				////	if(uda_alarm_send == 0)
				////	{						
				////		// alarm이 올라갔다면, 해제를 바로 올린다.
				////		UDA_Alarm_Set(UDA_PD_CP , 0);
				////		UDA_Alarm_Set(UDA_Clock, 0);
				////		Chk_UDA_Alm_gREMS();
				////		uda_alarm_send = 1;
				////	}
				////}	
				*/				
				//--------------------------------------
				// inventory auto repport
				// when RCU reststart, auto inventory 1회 report
				//
				if(Auto_Inv_Rpt_Req > 0)
				{
					if(Auto_Inv_Ready()) // inventory 모두 준비되면 바로 보냄.
					{	
						Send_Auto_Inventory();
						Auto_Inv_Rpt_Req = 0;
					}					
				}
				//--------------------------------------
				// __INVENTORY_UPDATE_CHECK_20190718__
				if(Auto_Inv_Rpt_Req == 0)
				{
					//every 10[s]
					if(send_check_cnt++ > 10)
					{
						Send_Auto_Inventory_By_Update();
						send_check_cnt = 0;
					}
				}				
			}
			else cnt_3s = 0;
			//-----------------------------------------
		}				
	}
	m_Env.task_END_status[_TASK_gREMS_COMM_] = _CLR_;
	pthread_exit(NULL);
}
//-------------------------------------------------------








//------------------------------------------------------------------------------------
void Chk_CFG_Ctrl_Auto_RPT_REQ( INT8U aid0, INT8U aid1 )
{
	INT16U k;
	INT16U idx;
	INT8U  ls_aid0, ls_aid1;

	if(m_Env.AutoRep_Req[_EMS_CFG__C_] == 0)
	{
		for (k = 0; k < CFG__C_AID_num; k++ )
		{
			idx = CFG__C_AID_idx[k];
			if ( idx == 0xFFFF ) break;

			Get_AID( idx, &ls_aid0, &ls_aid1 );

			if((ls_aid0 == aid0)&&(ls_aid1 == aid1))
			{
				m_Env.AutoRep_Req[_EMS_CFG__C_] = 1;
				break;
			}
		}		
	}
}
//----------------------------------------------------------------------





//-------------------------------------------------------
void REMS_St_Update(void)
{	
	//------------------------
	/*
	//Update_Stat( &m_Stat.AMP_Temp_x10[path4], m_St.AMP_Temp_x10[path4] );
	//Update_Stat( &m_Stat.VSWR_x100[path4]   , m_St.VSWR_x100[path4] ); 
	//Update_Stat( &m_Stat.Fwd_Out_Pwr_x10[path4] , m_St.Fwd_Out_Pwr_x10[path4] );
	//Update_Stat( &m_Stat.Fwd_Out_Att_x2[path4]  , m_St.Fwd_Out_Att_x2[path4] );	
	//Update_Stat( &m_Stat.Fwd_AMP_Var_Att_x2[path4] , m_St.Fwd_AMP_Var_Att_x2[path4] );
	//Update_Stat( &m_Stat.Ret_Loss_x10[path4], sVal16 );
	//Update_Stat( &m_Stat.Rvs_IN_Pwr_x10[path4], m_St.Rvs_IN_Pwr_x10[path4] );
	//Update_Stat( &m_Stat.Rvs_Att_x2[path4], m_St.Rvs_Att_x2[path4] );
	//Update_Stat( &m_Stat.RX_ALC_Att_x2[path4], m_St.RX_ALC_Att_x2[path4] );	
	*/
}
//----------------------------------------------------------------------

//// YCSO START added 191212
//// ***********************************************************************************

static PROTOCOL_FRAME rems_spectrum_txf;
void Send_gREMS_Spectrum_Data( void )
{
	INT16U len, l;
	INT16U p;
	INT8U totPkt, curPkt;
	INT16U size; // size of subdata;

	
	pthread_mutex_lock(&gREMS_Pkt_1k_Spectrum);	

	if(gRemsRspPkt_1k_Spectrum.use)
	{
		for(p=0; p<gRemsRspPkt_1k_Spectrum.tot; p++)
		{
//			size = gRemsRspPkt_1k_Spectrum.size[p];	
			l = 0; // init
			
			totPkt = gRemsRspPkt_1k_Spectrum.tot;
			curPkt = p+1;

			if( p == (totPkt - 1) )
				size = gRemsRspPkt_1k_Spectrum.size[0];
			else
				size = REMS_SPECTRUM_DATA_SIZE;
			
			print_agent(DEBUG_REMS, VT_BOLD_MAGENTA, "Pkt(%d/%d), CMD 0x%X ---- ", curPkt, totPkt, gRemsRspPkt_1k_Spectrum.cmd);
			
			memcpy(&rems_spectrum_txf.data[0], &REMS_specturm_buf[p*REMS_SPECTRUM_DATA_SIZE], size);

			l += size;

			Get__gREMS_My_SUBID(rems_spectrum_txf.SUB_ID);
			len = Make_Frame( gRemsRspPkt_1k_Spectrum.cmd, _RCODE_, &rems_spectrum_txf, l, gRemsRspPkt_1k_Spectrum.seqNo16, totPkt, curPkt);
			
			Send_UDP_Node_Data( COMM_UDP_NODE, gRemsRspPkt_1k_Spectrum.ip, gRemsRspPkt_1k_Spectrum.port16, (INT8U*)&rems_spectrum_txf, len);
			OSTimeDly(OS_DELAY_100msec);
		}		
		gRemsRspPkt_1k_Mea.use = 0;
		gRemsRspPkt_1k_Mea.tot = 0;
	}

	pthread_mutex_unlock(&gREMS_Pkt_1k_Spectrum);	

}


void tx_gREMS_spectrum_set( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port)
{
	pthread_mutex_lock(&gREMS_Pkt_1k_Spectrum);
	
//	gRemsRspPkt_1k_Spectrum.use = 1;
	gRemsRspPkt_1k_Spectrum.seqNo16 = rxf->seqNo16;
	memcpy(gRemsRspPkt_1k_Spectrum.ip, r_ip, 4);
	gRemsRspPkt_1k_Spectrum.port16 = r_udp_port;
	gRemsRspPkt_1k_Spectrum.cmd = rxf->Type;
	
//	Make_REMS_SubData( Hidden_AID_idx, Hidden_AID_num, &gRemsRspPkt_1k, port );

	pthread_mutex_unlock(&gREMS_Pkt_1k_Spectrum);	
	
}

void tx_gREMS_spectrum_data_set(INT8U* data, INT32U size )
{

	int tot = 0;

	pthread_mutex_lock(&gREMS_Pkt_1k_Spectrum);

	tot = ( size/REMS_SPECTRUM_DATA_SIZE ) + 1;
	gRemsRspPkt_1k_Spectrum.size[0] = size%REMS_SPECTRUM_DATA_SIZE;

	memset(REMS_specturm_buf, 0, MAX_COM_BUFFER);

	memcpy(REMS_specturm_buf, data, size);

	gRemsRspPkt_1k_Spectrum.tot = tot;

	gRemsRspPkt_1k_Spectrum.use = 1;

	pthread_mutex_unlock(&gREMS_Pkt_1k_Spectrum);	

}

void* Rems_Spectrum_Task(void *data)
{
//	INT8U* txf = NULL;
//	INT8U* rxf = NULL;
//	INT8U port;
	Thread_ARG_Var* p_th_arg;

	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

//	port = (INT8U)p_th_arg->Arg_v;
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "Rems_Specturm_Task RUN");

	m_Env.gREMS_Spectrum_req = _CLR_;
//	Init_Tx_Frame_SeqNo(port);	
	OSTimeDly( OS_DELAY_1sec );

	while( m_Env.task_flag[_TASK_Rems_Spectrum_] )		///// YCSO TASK END
	{
		OSTimeDly( OS_DELAY_100msec*2 );
		Update__Task_Count(_TASK_Rems_Spectrum_);
		
		if( m_Env.gREMS_Spectrum_req )
		{
			Send_gREMS_Spectrum_Data();
			m_Env.gREMS_Spectrum_req = _CLR_;
		}

	}
	m_Env.task_END_status[_TASK_Rems_Spectrum_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}


//// ***********************************************************************************
//// YCSO END added 191212


//-----------------------------------------------------------
#if 0 // it will be used later....

static INT8U node_name[100];
static INT8U eep__name[100];

void Proc_Node_Name_Change( INT8U* rxNodeName, INT8U rx_size )
{
	//INT8U start = 0;
	INT8U i, n, m;
	INT8U null_last, _null;
	INT8U ascii_rx = 0;
	INT8U size;
	INT8U c;
	INT8U update_req = 0;


	if(rx_size == 0) return;

	size = rx_size;
	if(rx_size > 99) size = 99;

	memset(node_name, 0, 100);
	memset(eep__name, 0, 100);

	//-------------------------------
	// 처음에 ascii 아니어도 check.
	ascii_rx = 0;
	null_last = 0;
	for(n=0, i=0; i<size; i++)
	{
		c = rxNodeName[i];

		if(Is_Visible_Char(c)) {_null = 0; ascii_rx = 1;}
		else 	               {_null = 1;}

		if( null_last ) ascii_rx = 0;
		if( ascii_rx && _null ) {ascii_rx = 0; null_last = 1;}
		if( ascii_rx ) node_name[n++] = c;
	}
	//-------------------------------
	// 처음에 ascii 아니어도 check.
	ascii_rx = 0;
	null_last = 0;
	size = m_Eep.Node_Name_len;
	for(m=0, i=0; i<size; i++)
	{
		c = m_Eep.Node_Name[i];

		if(Is_Visible_Char(c)) {_null = 0; ascii_rx = 1;}
		else 	               {_null = 1;}

		if( null_last ) ascii_rx = 0;
		if( ascii_rx && _null ) {ascii_rx = 0; null_last = 1;}
		if( ascii_rx ) eep__name[n++] = c;
	}
	//-------------------------------
	if(m!=n) update_req = 1;
	else
	{
		if(memcmp( node_name, eep__name, n) != 0) 
			update_req = 1;
		else
			update_req = 0;
	}

	if(update_req)
	{
		Wr_Node_Name( node_name, n+1 ); // "+1" is null char
		m_Env.is_eep_write = EEP_WRITE_CODE;
		//m_Env.eep_idx[20]++;
		//Debug_PutString((INT8U*)"[!]Eep6", 1);
	}
}



//------------------------------------------------------------------------------------
// 2016. Jan. : REMS Added
// aid0, aid1 에 해당되는 것을 찾아 max_len 만큼만 pData에 복사.
// return : 0 실패, non-zero : 해당 AID 의 데이터 length.
INT16U Find_AID( INT8U* data, INT16U tot_len, INT8U aid0, INT8U aid1, INT8U* pData )
{	
	AIDSTR*  paid;
	AIDSTR2* paid2;
	INT8U is_aid2_type;
	
	INT16U cnt=0;
	INT8U max_len=30;

	while(cnt < tot_len)
	{
		//-------------------------------
		if(Is_AID2_Range(data[cnt]))
			paid2 = (AIDSTR2*)&data[cnt];
		else			
			paid = (AIDSTR*)&data[cnt];
		//-------------------------------	

		if((paid->AID[0] == aid0) && (paid->AID[1] == aid1))
		{
			if(max_len > (paid->len)) max_len = paid->len;
			memcpy( pData, paid->data, max_len);

			return max_len;
		}		
		if(Is_AID2_Range(paid->AID[0]))
			cnt += (paid->len + AID2__AID__LEN__Size);
		else
			cnt += (paid->len + 3);
	}
	return 0;
}

// 다른 node에서 응답이 오지 않을 때, 링크 알람으로 GUI 처리 하려 할때 사용.
INT16U Make_Alarm_AID( INT8U* pSUBID, INT8U* pData ) // -----------------?? iro에서 불필요 할듯...
{
	pData[0] = 0x00;
	pData[1] = 0x01;
	pData[2] = RU_ALARM_CNT*2;
	memset( &pData[3], 0x00, RU_ALARM_CNT*2);

	// Link Alarm Set	
	pData[3+RU_ALARM_CNT+19] |= cbits[7]; // 19-cbits[7]

	return (3+ RU_ALARM_CNT*2);
}
#endif


/* EOF */

///////////////////////////////////////////////////////////////////////////////////////

