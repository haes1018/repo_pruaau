/////////////////////////////
//	uda_proc.c
/////////////////////////////
#define UDA_PROC_GLOBALS
#include "includes.h"
/////////////////////////////



/*
------------------------------------------------------------------
	UDA payload Suppress 
	//
	// @5G, UDA Spurious Suppress Not Defined...
	//
------------------------------------------------------------------
*/

static INT8U UDA_Alm_CntContinuous = 0;


/*
//--------------------------------------------------
//
// @ 5G, UDA-Spruious Suppress Alarm is NOT Define...
//
//--------------------------------------------------


#define UDA_ALM_MAX_Cnt (3)
#ifdef  __UDA_ALM_SUPPRESS_TEST__
#define UDA_ALM_SUPP_Time_ms (10*60*TICK_CO_SEC) // 10 [min]
#else
#define UDA_ALM_SUPP_Time_ms (120*60*TICK_CO_SEC) // 120 [min]
#endif



static INT8U suppress_chk_enable = 0;
static INT8U uda_payload_alm_prev = 0;
static OS_TIMER UDA_SuppressTimer;


void Proc_UDA_Suppress_Alm_Init(void)
{	
	OSTimerReset( &UDA_SuppressTimer );
	UDA_Alm_CntContinuous = 0;
	suppress_chk_enable = 0;
}

void Proc_UDA_Suppress_Alm(void)
{
	INT8U curr_uda_payload_alm = 0; // default 0

	if(_Alm_UDA_Spur_) 
		curr_uda_payload_alm = 1;	
	
	if(uda_payload_alm_prev != curr_uda_payload_alm)
	{		
		if(curr_uda_payload_alm) 
		{
			suppress_chk_enable = 1; // enable when 'normal' -> 'alarm'
			UDA_Alm_CntContinuous++;
			
			#ifdef  __UDA_ALM_SUPPRESS_TEST__
				D_Printf( DEBUG_UDA, " [UDA_ALM_SUPP] Enable, Cnt %d\r\n", UDA_Alm_CntContinuous );
			#endif
		}
		uda_payload_alm_prev = curr_uda_payload_alm; // update
	}
	
	if (m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_PayloadOFF)
	{
		if(suppress_chk_enable)
		{
			if(Is_UDA_PayloadOff_Supp_Alm() == 0)
			{
				if( OSTimerGet_msec(&UDA_SuppressTimer) >= UDA_ALM_SUPP_Time_ms ) 
				{
					if(UDA_Alm_CntContinuous >= UDA_ALM_MAX_Cnt)
					{
						////m_St.AlmSt.UDA.Spurious_suppress = _SET_; // AID NOT Exist @ 5G
						Proc_UDA_Suppress_Alm_Init();
						
						#ifdef  __UDA_ALM_SUPPRESS_TEST__
							D_Printf( DEBUG_UDA, " [UDA_ALM_SUPP] Cnt %d, End. Alarm Set.\r\n", UDA_Alm_CntContinuous );
						#endif
					}
				}
				else // time over
				{				
					#ifdef  __UDA_ALM_SUPPRESS_TEST__
						D_Printf( DEBUG_UDA, " [UDA_ALM_SUPP] Time Over. Cnt (%d) Reset.\r\n", UDA_Alm_CntContinuous );
					#endif
					Proc_UDA_Suppress_Alm_Init();
				}
			}
			else // if already suppress alarm.
				Proc_UDA_Suppress_Alm_Init();
		}
		else
			Proc_UDA_Suppress_Alm_Init();
	}
	else
	{
		if(suppress_chk_enable)
		{				
			#ifdef  __UDA_ALM_SUPPRESS_TEST__
				D_Printf( DEBUG_UDA, " [UDA_ALM_SUPP] Is not RS_PayloadOFF Mode, Return\r\n");
			#endif			
		}
		Proc_UDA_Suppress_Alm_Init();
		////m_St.AlmSt.UDA.Spurious_suppress = _CLR_; // AID NOT Exist @ 5G
	}
}
*/
//----------------------------------------------------------------------------------------------


	

void Init_UDA_SuppAlm(void)
{		
	#ifdef	__UDA_ALM_SUPPRESS_TEST__
	if(Is_UDA_PayloadOff_Supp_Alm())	
		print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA_ALM_SUPP] Init UDA Supp Alm.");	
	#endif

	//////m_St.AlmSt.UDA.Spurious_suppress = _CLR_; // AID NOT Exist @ 5G
	UDA_Alm_CntContinuous = 0;
}


//----------------------------------------------------------------------------------------------
INT8U Is_UDA_PayloadOff_Supp_Alm(void)
{
	//
	// @5G, No UDA-Spurious-Suppress is Defined...
	// return always 0.
	return 0; 
}
//-----------------------------------------------------------

static INT8U Payload_Int_OnOff[MAX_Optic][MAX_MIMO_PATH] = {{0,0,0,0},{0,0,0,0}}; // Payload On/Off, Internal Alarm, 2019.04.27

// UDA Spurious Alarm
void Update_Payload_Alm(void)
{
	INT8U fa, path4;
	INT8U cpri;
	INT8U payload = 0;

	for(fa = 0; fa < MAX_Optic; fa++)
	{
		if(fa == 0) cpri = CPRI_0;
		else        cpri = CPRI_1;

		if(Is_SFP_Inserted(cpri))
		{
			for(path4 = 0; path4 < MAX_PATH_4; path4++)
			{
				if (m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_Alm_Only)
					payload = Payload_Int_OnOff[fa][path4];
				else					
					payload = Is_RVS_Spurious(fa, path4);

				if(payload == OFF)
					break;
			}
			if(payload == OFF)
				break;
		}		
	}
	//-----------------------------------
	// all payload is ON	
	//-----------------------------------
	if(payload == OFF)
		m_St.AlmSt.UDA.Spurious = _ALM_SET_;
	else	
		m_St.AlmSt.UDA.Spurious = _ALM_CLR_;
	
	if(m_St.AlmFlag.UDA.Spurious)
		m_St.AlmSt.UDA.Spurious = m_St.AlmCtrl.UDA.Spurious;	
}
//---------------------------------------------------
/*
-----------------------------------------------------------
	return 1, if Payload off
	return 0, if Payload ON
-----------------------------------------------------------
*/
INT8U Get_Curr_Payload_Off_St(INT8U optic, INT8U path4)
{
	if(optic >= MAX_Optic) return 0;
	if(path4 >= AMPU_4_MAX) return 0;

	if (m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_Alm_Only)
	    return Payload_Int_OnOff[optic][path4];
	else if (m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_PayloadOFF)
		return m_St.d_Payload_OnOff[optic][path4];
	else 
		return 0;
}
//-----------------------------------------------------------
void Payload_St_Ctrl(INT8U optic, INT8U path4, INT8U onoff, INT8U dbg)
{
	(void)dbg;
		
	if(optic >= MAX_Optic) return;
	if(path4 >= MAX_MIMO_PATH) return;
	if(Check_PAU_Use(path4)==0) return; // 2020.01.29

	if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_Alm_Only)
	{
		Payload_Int_OnOff[optic][path4] = onoff; // 'OFF' is Alarm
	}
	else if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_PayloadOFF)
	{
		DTU_DL_Path_Onoff( optic, path4, onoff); // CHECK
		DTU_UL_Path_Onoff( optic, path4, onoff);

		print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[SPU] Payload_St_Ctrl, FA#%d-Path#%d onoff(0x%X)", optic+1, path4+1, onoff);
	}
}
//-----------------------------------------------------------
static INT8U Proc_idx[MAX_Optic][AMPU_4_MAX] = {{0,0,0,0},{0,0,0,0}};

void Payload_On_Def(INT8U optic, INT8U path4)
{
	if(optic >= MAX_Optic) return;
	if(path4 >= MAX_MIMO_PATH) return;
	
	Proc_idx[optic][path4]=0;
	
	Payload_St_Ctrl(optic, path4, ON, 1); // 0 is oN
}


//----------------------------------------------------------------------------------------------
// 2014.06.2X : Payload off 알람. AMP 사용/미사용, Payload off 사용/미사용 적용된 최종 값을 리턴.
static OS_TIMER payload_oFF_Timer[MAX_Optic][AMPU_4_MAX]; // try to payload off check timer
static OS_TIMER payload_oN__Timer[MAX_Optic][AMPU_4_MAX]; // try to payload on(==release) timer

#define _min_ (0)
#define _max_ (1)

static INT16S Dig_Pwr_x10[MAX_Optic][AMPU_4_MAX][2]; // [min/max]
//static INT16S AMP_Pwr_x10[AMPU_4_MAX]; // debug할때 필요...

/*
------------------------------------------------------------
	UDA 불요파 Algorithm
	--------------------

	읽은 Dig_Pwr의 range : 
	A = Dig_Pwr - 3dB
	B = Dig_Pwr + 3dB

(1) 현재 payload on 인 상태
	다음 읽은 값이 A~B 사이이면, 시간 증가,
	아니면, 시간 다시 카운트, ragne 를 읽은 값에서 다시 계산.

	10분 A~B 유지되면, AMP_RVS_Pwr 와 비교, 
	C = A - 40dB
	AMP_RVS_Pwr 가 C 보다 낮으면 payload off 상태로 진입
	그렇지 않으면, 10분 카운트 다시.

(2) 현재 payload off 인 상태 
	읽은 Dig_Pwr 가 마지막 기준값 A보다 낮은 것을 1분 이상 유지 하면, payload on.
	아니면, 계속 off 상태.
---------------------------------------------------
*/

//----------------------------------------------------
// AID 0x544C
void Ctrl_UDA_Spurious_Layer(INT8U optic, INT8U path4, INT8U onoff)
{
	if(optic >= MAX_Optic) return;
	if(path4 >= AMPU_4_MAX) return;

	/*
	// 2019.09.18 : delete below "2019.09.17"
	// 2019.09.17
	////if(m_Eep.UDA_Oper_Mode[UDA_Spurious] != RS_PayloadOFF) 
	////return;
	*/

	if(onoff != ON) onoff = OFF;
	
	ctrlBits(&m_St.UDA_Spurious_Layer, onoff, (optic*AMPU_4_MAX + path4));
}


/*
	Bit 7: DL3(Alpha)
	Bit 6: DL2(Alpha)
	Bit 5: DL1(Alpha)
	Bit 4: DL0(Alpha)
	-------------------
	Bit 3: DL3
	Bit 2: DL2
	Bit 1: DL1
	Bit 0: DL0

	0: Payload On,
	1: Payload Off
*/

INT8U Is_RVS_Spurious(INT8U optic, INT8U path4)
{
	INT8U bit_posi;

	if(optic >= MAX_Optic) return 0; // not-off
	if(path4 >= AMPU_4_MAX) return 0; // not-off	
	
	if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_PayloadOFF)
	{
		bit_posi = optic*AMPU_4_MAX + path4;
		
		if(m_St.UDA_Spurious_Layer & cbits[bit_posi]) // off
			return 1; // off
		else return 0; // not-off	
	}
	else return 0; // not-off
}

// 2020.01.28 : RVS Spurious 만으로만 payload off 인 경우, 1 리턴.
INT8U Is_RVS_Spur_Only_Off(INT8U optic, INT8U path4)
{
	INT8U bit_posi;

	if(optic >= MAX_Optic) return 0; // not-off
	if(path4 >= AMPU_4_MAX) return 0; // not-off	
	
	if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_PayloadOFF)
	{
		bit_posi = optic*AMPU_4_MAX + path4;
		
		if(m_Env.Rvs_Spur_OFF & cbits[bit_posi]) // off
			return 1; // off
		else return 0; // not-off	
	}
	else return 0; // not-off
}





//------------------------------------------------------------
#ifdef _SHORTEN_TIME____PAYLOAD_
#define PAYLOAD_ON_TIME (2)  // 2min
#else
#define PAYLOAD_ON_TIME (10) // 10min 
#endif
//------------------------------------------------------------

static OS_TIMER Spurious_Proc_ms_Timer[MAX_Optic];


//#define RVS_DIG_PWR_Up_Limit_x10 (-20*10) // -20.0[dBm]
//#define PAYLOAD_Pwr_Diff_x10 (40*10) // 40[dB]

//----------------------------------------------------

void Proc___UDP_Spurious(INT8U optic) // chk
{	
	INT8U path4;
	INT16S R_Dig_In_Pwr_x10;	
	INT8U is_out_range = 0;
	INT8U is_Now_payload_off;	
	INT8U cpri;

	static INT8U init[MAX_Optic] = {0, 0};
	static INT8U in_range[MAX_Optic][AMPU_4_MAX];
	static INT8U prev_operation_mode[MAX_Optic] = {RS_No_Action, RS_No_Action};
	
	switch(optic)
	{
		case _100M: 
		case Alpha:
			if(optic == _100M) cpri = CPRI_0;
			else               cpri = CPRI_1;
			
			if(Is_SFP_Inserted(cpri)==0)
			{	
				for(path4 = 0; path4 < AMPU_4_MAX; path4++)		
				{
					Proc_idx[optic][path4]=0;
					////Ctrl_UDA_Spurious_Layer(optic, path4, OFF);
					////Payload_St_Ctrl(optic, path4, OFF, 4); // 1 is off
					in_range[optic][path4]=0;
				}
				init[optic] = 0;				
				return;
			}
			break;		

		default:
			return;
	}

	if(init[optic] == 0)
	{
		init[optic] = 1;
		memset(in_range[optic], 0, AMPU_4_MAX);
		memset(Payload_Int_OnOff[optic], 0, AMPU_4_MAX);
		return; // for next step
	}

	//------------------------------------
	// every 1[s]
	if( OSTimerGet_msec(&Spurious_Proc_ms_Timer[optic]) < 1000 ) 
		return;
	OSTimerReset( &Spurious_Proc_ms_Timer[optic] );
	//------------------------------------



	//------------------------------------
	// Payload algorithm oFF
	//------------------------------------
	if(m_Eep.UDA_Oper_Mode[UDA_Spurious] == RS_No_Action)
	{	
		for(path4 = 0; path4 < AMPU_4_MAX; path4++) 
		{
			Payload_On_Def(optic, path4);
			Ctrl_UDA_Spurious_Layer(optic, path4, ON);
			in_range[optic][path4]=0;
		}
		prev_operation_mode[optic] = m_Eep.UDA_Oper_Mode[UDA_Spurious];

		/*
		----------------------------------------
		// 2019.05.03 : UDP-Payload-Off-Suppression is not Defined @ 5G
		//Init_UDA_SuppAlm();
		----------------------------------------
		*/		
		return; //----------- return
	}
	//------------------------------------
	// payload_off or alarm_only..
	//------------------------------------
	else 
	{
		if(prev_operation_mode[optic] != m_Eep.UDA_Oper_Mode[UDA_Spurious]) // if changed..
		{
			for(path4 = 0; path4 < AMPU_4_MAX; path4++) 
			{
				Payload_On_Def(optic, path4);
				Ctrl_UDA_Spurious_Layer(optic, path4, ON);
				in_range[optic][path4]=0;
			}
		}		
	}
	prev_operation_mode[optic] = m_Eep.UDA_Oper_Mode[UDA_Spurious];

	/*
	----------------------------------------
	// 2019.05.03 : UDP-Payload-Off-Suppression is not Defined @ 5G
	//if(Is_UDA_PayloadOff_Supp_Alm()) return;
	----------------------------------------
	*/
	
	//------------------------------------
	// Payload algorithm ON
	//------------------------------------
	for (path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4) == 0)
		{
			Payload_On_Def(optic, path4);
			Ctrl_UDA_Spurious_Layer(optic, path4, ON);
			in_range[optic][path4]=0;

			continue;
		}
		//----------------------------------------------------
		// Get DIG RVS In Pwr
		//----------------------------------------------------
		R_Dig_In_Pwr_x10 = m_St.d_UL_Noise_RMS_x10[optic][path4]; // 2019.05.14
		//----------------------------------------------------
		// 2019.05.18
		Dig_Pwr_x10[optic][path4][_min_] = m_Eep.Threshold_UL_Spurious_x10 - 30;
		Dig_Pwr_x10[optic][path4][_max_] = m_Eep.Threshold_UL_Spurious_x10 + 30;
		
		if(Proc_idx[optic][path4]==0) // if init
		{			
			OSTimerReset(&payload_oFF_Timer[optic][path4]);
			OSTimerReset(&payload_oN__Timer[optic][path4]);

			Proc_idx[optic][path4]=1;
							
			Do4_Printf(DEBUG_UDA, optic, path4, "[SPU] FA#%d-Path#%d 1st PL : 1st : dig_pwr(%d), max(%d), min(%d)\n",
				optic+1, path4+1, 
				R_Dig_In_Pwr_x10, 
				Dig_Pwr_x10[optic][path4][_max_], 
				Dig_Pwr_x10[optic][path4][_min_] );
			
			continue;
		}
		//----------------------------------------------------			
		if(Get_Curr_Payload_Off_St(optic, path4) == OFF)
			 is_Now_payload_off = 1;
		else is_Now_payload_off = 0;		
		//--------------------------------------------------------------------------------------
		
		
		//--------------------------------------------------------------------------------------
		if( is_Now_payload_off ) // Now off. on 조건을 체크
		{	
			// now off, clr off timer
			OSTimerReset(&payload_oFF_Timer[optic][path4]);
			
			/*
			------------------------------------------
				Normal, if keeping below,
					(R_Dig_In_Pwr_x10 <= (Dig_Pwr_x10_mininum - 1*10))
				for 1[min]
			------------------------------------------	
			*/
			
			if(R_Dig_In_Pwr_x10 > Dig_Pwr_x10[optic][path4][_min_]) // time reset if over than lower-limit
			//if(R_Dig_In_Pwr_x10 > (Dig_Pwr_x10[optic][path4][_min_] - 1*10)) // time reset if over than [lower-limit below 1dB]
			{
				// keeping off
				OSTimerReset(&payload_oN__Timer[optic][path4]);
			}

			if( OSTimerGet_msec(&payload_oN__Timer[optic][path4] ) >= (1*60*TICK_CO_SEC) )
			{
				OSTimerReset( &payload_oN__Timer[optic][path4] );
				Payload_St_Ctrl(optic, path4, ON, 3); // 0 is oN
				Ctrl_UDA_Spurious_Layer(optic, path4, ON);
				
				in_range[optic][path4]=0;
			}

			Do4_Printf(DEBUG_UDA, optic, path4, "[SPU] FA#%d-Path#%d PL-OFF now, dig_pwr(%d), min(%d), on-timer(%d)\n",
				optic+1, path4+1, 
				R_Dig_In_Pwr_x10, 
				Dig_Pwr_x10[optic][path4][_min_], 
				OSTimerGet_msec(&payload_oN__Timer[optic][path4]));

		}
		//--------------------------------------------------------------------------------------
		else // now on, off 조건을 체크...
		{
			// now on, clr on timer
			OSTimerReset( &payload_oN__Timer[optic][path4] );

			/*
			     if (R_Dig_In_Pwr_x10 < Dig_Pwr_x10[optic][path4][_min_]) is_out_range = 1;
			else if (R_Dig_In_Pwr_x10 > Dig_Pwr_x10[optic][path4][_max_]) is_out_range = 1;
			else    is_out_range = 0;
			*/
			if (R_Dig_In_Pwr_x10 >= m_Eep.Threshold_UL_Spurious_x10) 
				 is_out_range = 0; // in-range
			else is_out_range = 1;

			if(is_out_range)
			{
				in_range[optic][path4] = 0;
				// update range
				Dig_Pwr_x10[optic][path4][_min_] = R_Dig_In_Pwr_x10 - 30;
				Dig_Pwr_x10[optic][path4][_max_] = R_Dig_In_Pwr_x10 + 30;

				OSTimerReset( &payload_oFF_Timer[optic][path4] ); // off timer restart.

				Do4_Printf(DEBUG_UDA, optic, path4, "[SPU] FA#%d-Path#%d PL-On Now, dig_pwr(%d), max(%d), min(%d)\n",
					optic+1, path4+1, 
					R_Dig_In_Pwr_x10, 
					Dig_Pwr_x10[optic][path4][_max_], 
					Dig_Pwr_x10[optic][path4][_min_]);
			}
			else // in range Timer Check : every 10 min..
			{
				in_range[optic][path4]=1;
				
				Do4_Printf(DEBUG_UDA, optic, path4, "[SPU] FA#%d-Path#%d PL-On Now, In-Range, dig_pwr(%d), off-timer(%d)\n",
						optic+1, path4+1, 
						R_Dig_In_Pwr_x10, 
						OSTimerGet_msec(&payload_oFF_Timer[optic][path4]));
				
				if( OSTimerGet_msec(&payload_oFF_Timer[optic][path4] ) >= (PAYLOAD_ON_TIME*60*TICK_CO_SEC) )
				{
					OSTimerReset( &payload_oFF_Timer[optic][path4] ); // off timer restart.
					
					Do4_Printf(DEBUG_UDA, optic, path4, "[SPU] FA#%d-Path#%d ReStart 10 min\n", optic+1, path4+1);

					// 2019.05.14 : not use AMP-Power
					Payload_St_Ctrl(optic, path4, OFF, 4); // 1 is off
					Ctrl_UDA_Spurious_Layer(optic, path4, OFF);
					//----------------------------------------------------
				}
			} // off check timer
		}
	} // for loop	
}

void Proc___RVS_Spurious_Payload(void) // chk
{
	Proc___UDP_Spurious(_100M);
	Proc___UDP_Spurious(Alpha);
}




















/*
//---------------------------------------------------------
	if( OSTimerGet_msec(&MIMO_Standby_Timer_ms) < 500 ) return; // every 500[ms]
	OSTimerReset( &MIMO_Standby_Timer_ms );
//---------------------------------------------------------	
*/

//---------------------------------------------------------
#ifdef _SHORTEN_TIME____AUTO_RST_
//---------------------------------------------------------
#define CNT_Ref_ms		(2*60*1000)
#define Normal__Keep_ms (2*60*1000)
#define DIG_KEEP_Ref_ms (2*60*1000)
//------------------------------
#define Suppress_Off_ms (5*60*1000) // 5min -> 2min(2019.06.02), 2min -> 5min(2020.02.18)
#define SUPPRESS_ON__ms (15*60*1000)
//------------------------------
const INT8U Alm_Cnt_Max[UDA_MAX] = {3,3,3,3}; // alarm count during CNT_Ref_ms, 3회 발생 시 알람.
//---------------------------------------------------------
#else
//---------------------------------------------------------
#define CNT_Ref_ms      (30*60*1000) // Ref 시간동안의 횟수 체크.30min
#define Normal__Keep_ms (30*60*1000) // Normal 유지 시간. 30min
#define DIG_KEEP_Ref_ms (30*60*1000) // Dig alarm 유지 시간.30min
//------------------------------
#define Suppress_Off_ms (35*60*1000)  //  35[min] continuous
#define SUPPRESS_ON__ms (120*60*1000) // 120[min]
//------------------------------
const INT8U Alm_Cnt_Max[UDA_MAX] = {20,20,20,20}; // alarm count during CNT_Ref_ms, 20회 발생 시 알람.
//---------------------------------------------------------
#endif
//---------------------------------------------------------

static OS_TIMER Suppress_ON__ms_Timer[UDA_MAX]; // 120[min] timer for alarm 
static OS_TIMER Suppress_OFF_ms_Timer[UDA_MAX]; //  30[min] timer for alarm-clear

#define PRU_AAU_Reset_OK_Code (0x4ADE5928)

//-----------------------------------------------------
void UDA_Auto_Reset_EEP_Save( void )
{	
	m_Uda.Reset_Code = PRU_AAU_Reset_OK_Code;	
	
	memcpy(&m_rst_info.UDA, &m_Uda, sizeof(UDA_STR));

	// 2019.08.27 : etc -> Rst_By_Uda_Pd_CPRI/Rst_By_Uda_Clock/Rst_By_Uda_Link
	////SetResetInfo(Rst_By_Etc, 105 );
	
	     if(m_Uda.UDA_Alarm_f & cbits[UDA_PD_CP]) SetResetInfo( Rst_By_Uda_Pd_CPRI, 4594 ); 
	else if(m_Uda.UDA_Alarm_f & cbits[UDA_Clock]) SetResetInfo( Rst_By_Uda_Clock, 4594 );
	else if(m_Uda.UDA_Alarm_f & cbits[UDA_Link])  SetResetInfo( Rst_By_Uda_Link, 4594 );
		
	sync();
}
//-----------------------------------------------------------------------
char Get_UDA_Char(INT8U Uda)
{
	     if(Uda == UDA_Link)  return 'L';
	else if(Uda == UDA_Clock) return 'C';
	else if(Uda == UDA_PD_CP) return 'P';
	else                      return '?';
}
//-----------------------------------------------------
void Set_UDA_Alarm__flag( INT8U bit, INT8U alm_set ) // chk
{
	if(bit >= UDA_MAX) return;
	
	if (alm_set)
	{	
		m_Uda.UDA_Alarm__Code = PRU_AAU_Reset_OK_Code;
		m_Uda.UDA_Alarm_f |= cbits[bit];
	}
	else
	{
		m_Uda.UDA_Alarm_f &= nbits[bit];

		if((m_Uda.UDA_Alarm_f & 0x0F) == 0)
			m_Uda.UDA_Alarm__Code = 0;
	}	
}
//-----------------------------------------------------
// reset 전의 값이 Warm reset 되고 나서도 그대로 유지 되었는지 확인. 
//
INT8U Valid_UDA_Alm_Code(void)
{
	if( m_Uda.UDA_Alarm__Code != PRU_AAU_Reset_OK_Code ) 
		return 0;	
	else return 1;
}
//-----------------------------------------------------
// UDA Auto Reset flag를 set or clear
//
void Set_UDA_RST_Suppr_f( INT8U bit, INT8U alm_set )
{	
	if(bit >= UDA_MAX) return;

	if (alm_set)
	{
		m_Uda.RST_Suppr__Code = PRU_AAU_Reset_OK_Code;
		m_Uda.RST_Suppr_f |= cbits[bit];
	}
	else
	{
		m_Uda.RST_Suppr_f &= nbits[bit];

		if((m_Uda.RST_Suppr_f & 0x0F) == 0)
			m_Uda.RST_Suppr__Code = 0;
	}	
}
//-----------------------------------------------------
// reset 전의 값이 Warm reset 되고 나서도 그대로 유지 되었는지 확인. 
//
INT8U Valid_AutoRst_Flag_Code(void)
{
	if( m_Uda.RST_Suppr__Code != PRU_AAU_Reset_OK_Code ) 
		return 0;
	else return 1;
}
//-----------------------------------------------------





//-----------------------------------------------------
// 30분 20회 혹은 30분 30회에 대한 타이머, 알람카운터.
static OS_TIMER Alm_Cnt_ms_Timer[UDA_MAX];  // timer for alarm-generation-count
static OS_TIMER Alm_Keep_ms_Timer[UDA_MAX]; // timer for alarm-keeping-time
static INT8U Alm_Cnt[UDA_MAX];

static OS_TIMER Alm_Clr_ms_Timer[UDA_MAX];  // timer for alarm-clear when Alarm only.

//-------------------------------------------
void UDA_Auto_Rst_All_Init(INT8U bUda)
{
	if(bUda < UDA_MAX)
	{
		OSTimerReset( &Alm_Cnt_ms_Timer[bUda] );
		OSTimerReset( &Alm_Keep_ms_Timer[bUda] );
		
		UDA_Auto_Rst_Cnt_Clr(bUda);
	}
}
//-------------------------------------------
void UDA_Auto_Rst_Cnt_Clr(INT8U rst_idx)
{
	if(rst_idx >= UDA_MAX) 
		return;
	
	Alm_Cnt[rst_idx] = 0;
}

//---------------------------------------------------------
void UDA_Alm_Inc(INT8U Uda, char c)
{
	if(Uda >= UDA_MAX) return;

	Alm_Cnt[Uda]++;
	
	if (Alm_Cnt[Uda] >= Alm_Cnt_Max[Uda])
		Alm_Cnt[Uda] = Alm_Cnt_Max[Uda];
	
	print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] Alm Cnt : %d", c, Alm_Cnt[Uda]);
}

//----------------------------------------------------------------------


/*
-------------------------------------------------------
	UDA Alarm 
	
	!! It is NOT UDA_Reset_Suppress
-------------------------------------------------------
*/
void UDA_Alarm_Set(INT8U UDA, INT8U alm) // UDA Alarm Set/Clear
{	
	////if(ALM_JIG_MODE == ALM_JIG_MODE_CODE) 
	////	return;	
	//if(m_St.AlmTest) // 2019.09.02
	//	return;	

	switch(UDA)
	{
		case UDA_Clock :
			if(alm) 
			{ 
				if(m_St.AlmSt.UDA.Clock==0)
				{
					print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA] UDA_Clock Alm Set.");
					
					if(m_Eep.UDA_Oper_Mode[UDA] == AR_Reset) 
						Set_UDA_Alarm__flag(UDA, _F_SET_);
				}				
				m_St.AlmSt.UDA.Clock = 1;

				// 2019.09.03
				if((m_Uda.UDA_Alarm_f & cbits[UDA])==0) 
				{
					if(m_Eep.UDA_Oper_Mode[UDA] == AR_Reset) 
						Set_UDA_Alarm__flag(UDA, _F_SET_);
				}
			}
			else
			{ 
				if(m_St.AlmSt.UDA.Clock)
				{
					print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA] UDA_Clock Alm Clr.");
					Set_UDA_Alarm__flag(UDA, _F_CLR_);
				}
				m_St.AlmSt.UDA.Clock = 0;
				
				// 2019.09.03
				if(m_Uda.UDA_Alarm_f & cbits[UDA])
					Set_UDA_Alarm__flag(UDA, _F_CLR_);
			}
						
			if(m_St.AlmFlag.UDA.Clock)
				m_St.AlmSt.UDA.Clock = m_St.AlmCtrl.UDA.Clock;
								
			m_St.Last_Alm.UDA.Clock = m_St.AlmSt.UDA.Clock;	
			break;
		//-----------------------------------------------
		case UDA_PD_CP :		
			if(alm) 
			{ 
				if(m_St.AlmSt.UDA.PD_CPRI==0)
				{
					print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA] UDA_PD_CPRI Alarm.");
					
					if(m_Eep.UDA_Oper_Mode[UDA] == AR_Reset) 
						Set_UDA_Alarm__flag(UDA, _F_SET_);
				}
				m_St.AlmSt.UDA.PD_CPRI = 1;

				// 2019.09.03
				if((m_Uda.UDA_Alarm_f & cbits[UDA])==0) 
				{
					if(m_Eep.UDA_Oper_Mode[UDA] == AR_Reset) 
						Set_UDA_Alarm__flag(UDA, _F_SET_);
				}
			}
			else
			{				
				if(m_St.AlmSt.UDA.PD_CPRI)
				{
					print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA] UDA_Clock Clear.");
					Set_UDA_Alarm__flag(UDA, _F_CLR_);
				}	
				m_St.AlmSt.UDA.PD_CPRI = 0;

				// 2019.09.03
				if(m_Uda.UDA_Alarm_f & cbits[UDA])
					Set_UDA_Alarm__flag(UDA, _F_CLR_);
			}
						
			if(m_St.AlmFlag.UDA.PD_CPRI)
				m_St.AlmSt.UDA.PD_CPRI = m_St.AlmCtrl.UDA.PD_CPRI;
			
			m_St.Last_Alm.UDA.PD_CPRI = m_St.AlmSt.UDA.PD_CPRI;
			break;

		//-----------------------------------------------	
		default:	
		case UDA_Link:
			// AID is NOT defined @ 5G
			break;	
	}	
}

INT8U IS__UDA_Alarm(INT8U UDA)
{
	switch(UDA)
	{
		case UDA_Clock : return m_St.AlmSt.UDA.Clock;				
		case UDA_PD_CP : return m_St.AlmSt.UDA.PD_CPRI;
		
		default: return 0;
	}	
}

/*
-----------------------------------------------------------------
	After CPU_Reset,
	Restore Alarm-Status in RAM(or nonVolatile Memory)
	to the Status-structure, like 'm_St'.

	If CPU Reset is by power-on-rese or user-reset,
	Do Not restore but init.
-----------------------------------------------------------------
*/
static INT8U UDA_RST_Suppr_f_Prev = 0;

INT8U Chk_Auto_Reset_By_UDA_Suppress(void) // when RCU init, Check UDA-Auto-Reset
{
	//INT8U byte, bit;
	INT8U Uda;
			
	if( Valid_UDA_Alm_Code() == 0)
		m_Uda.UDA_Alarm_f = 0;
	
	if( Valid_AutoRst_Flag_Code() == 0)
		m_Uda.RST_Suppr_f = 0;

	// loading previous value which is before reset
	// clr 알람 올라가도록.
	UDA_RST_Suppr_f_Prev = m_Uda.RST_Suppr_f; 
	
	printf("[m_Uda] UDA__Alarm_f : 0x%X \n", m_Uda.UDA_Alarm_f);
	printf("[m_Uda] Reset_Supp_f : 0x%X \n", m_Uda.RST_Suppr_f);
	printf("[m_Uda] Reset_Code   : 0x%X \n", m_Uda.Reset_Code);
	//-------------------------------------------------------

	// Loading UDA-Alarm and Suppress-Alarm 
		
	/*
	// UDA_Link: // UDA_Link is not defined.
	//	if(m_Uda.RST_Suppr_f & cbits[Uda])	// Resotre UDA Suppress
	//	{
	//		m_St.AlmSt.UDA.Link_RST_Suppr = 1;
	//		m_St.NMS_Alm.UDA.Link_RST_Suppr = 1;
	//		m_St.Old_Alm.UDA.Link_RST_Suppr = 1;
	//	}
	//	break;
	*/
		
	// UDA_Clock --------------------------------------------
	if(m_Uda.UDA_Alarm_f & cbits[UDA_Clock]) // UDA_Alarm
	{
		if(m_St.AlmFlag.UDA.Clock)
			m_St.AlmSt.UDA.Clock = m_St.AlmCtrl.UDA.Clock;		
		else m_St.AlmSt.UDA.Clock = 1;		
	}
	m_St.NMS_Alm.UDA.Clock = m_St.AlmSt.UDA.Clock;
	m_St.Old_Alm.UDA.Clock = m_St.AlmSt.UDA.Clock;
	
	if(m_Uda.RST_Suppr_f & cbits[UDA_Clock]) // Resotre Suppress Alarm
	{
		if(m_St.AlmFlag.UDA.Clk__RST_Suppr)
			m_St.AlmSt.UDA.Clk__RST_Suppr = m_St.AlmCtrl.UDA.Clk__RST_Suppr;		
		else m_St.AlmSt.UDA.Clk__RST_Suppr = 1;		
	}
	m_St.NMS_Alm.UDA.Clk__RST_Suppr = m_St.AlmSt.UDA.Clk__RST_Suppr;
	m_St.Old_Alm.UDA.Clk__RST_Suppr = m_St.AlmSt.UDA.Clk__RST_Suppr;
	
	// UDA_PD_CP --------------------------------------------
	if(m_Uda.UDA_Alarm_f & cbits[UDA_PD_CP]) // UDA_Alarm
	{
		if(m_St.AlmFlag.UDA.PD_CPRI)
			m_St.AlmSt.UDA.PD_CPRI = m_St.AlmCtrl.UDA.PD_CPRI;		
		else m_St.AlmSt.UDA.PD_CPRI = 1;
	}
	m_St.NMS_Alm.UDA.PD_CPRI = m_St.AlmSt.UDA.PD_CPRI;
	m_St.Old_Alm.UDA.PD_CPRI = m_St.AlmSt.UDA.PD_CPRI;
	
	if(m_Uda.RST_Suppr_f & cbits[UDA_PD_CP]) // Resotre Suppress Alarm
	{
		if(m_St.AlmFlag.UDA.CPRI_RST_Suppr)
			m_St.AlmSt.UDA.CPRI_RST_Suppr = m_St.AlmCtrl.UDA.CPRI_RST_Suppr;
		else m_St.AlmSt.UDA.CPRI_RST_Suppr = 1;
	}
	m_St.NMS_Alm.UDA.CPRI_RST_Suppr = m_St.AlmSt.UDA.CPRI_RST_Suppr;
	m_St.Old_Alm.UDA.CPRI_RST_Suppr = m_St.AlmSt.UDA.CPRI_RST_Suppr;
		
	//-------------------------------------------------------
	if(m_Uda.Reset_Code == PRU_AAU_Reset_OK_Code)
	{
		printf("[m_Uda] Reset by Auto RCU Reset \n");
		printf("[m_Uda] Time_backup %d, %d, %d [ms] \n", 
			m_Uda.time_backup_ms[UDA_Clock],
			m_Uda.time_backup_ms[UDA_PD_CP],
			m_Uda.time_backup_ms[UDA_Link]);
			
		m_Uda.Reset_Code = 0; // init

		return 1; // reset by auto rcu reset
	}
	else 
	{
		/*
			If CPU Reset is by power-on-rese or user-reset,
			Do Not restore but init.
			
			All init.
		*/
		
		printf("[m_Uda] All Init \n");
		
		for(Uda = UDA_Link; Uda < UDA_MAX; Uda++)
		{	
			Set_UDA_Alarm__flag(Uda, 0);
			Set_UDA_RST_Suppr_f(Uda, 0);
			
			m_Uda.UDA_Rst_Cnt[Uda] = 0;
			m_Uda.Rst_Suppress[Uda] = 0;
			m_Uda.time_backup_ms[Uda] = 0;
		}
		m_Uda.Reset_Code = 0; // init
		
		return 0;
	}	
}
//-----------------------------------------------------


//----------------------------------------------------------------------------------------
// UDA Suppression Alarm Set or Clr
//
void UDA_Suppr_Alm_Ctrl(INT8U UDA, INT8U alm)
{
	switch(UDA)
	{		
		case UDA_Clock:
			if(m_St.AlmFlag.UDA.Clk__RST_Suppr)
				m_St.AlmSt.UDA.Clk__RST_Suppr = m_St.AlmCtrl.UDA.Clk__RST_Suppr;			
			else m_St.AlmSt.UDA.Clk__RST_Suppr = alm;
			
			m_St.Last_Alm.UDA.Clk__RST_Suppr = m_St.AlmSt.UDA.Clk__RST_Suppr;
			break;
			
		case UDA_PD_CP:
			if(m_St.AlmFlag.UDA.CPRI_RST_Suppr)
				m_St.AlmSt.UDA.CPRI_RST_Suppr = m_St.AlmCtrl.UDA.CPRI_RST_Suppr;
			else m_St.AlmSt.UDA.CPRI_RST_Suppr = alm;

			m_St.Last_Alm.UDA.CPRI_RST_Suppr = m_St.AlmSt.UDA.CPRI_RST_Suppr;
			break;		
	}
}


/*
-----------------------------------------------------

	Procedure 
	for 
	Reset by UDA

-----------------------------------------------------
*/
static OS_TIMER UDA_Rst_Dly_Timer;

void Proc___Reset_By_UDA(void)
{
	INT8U Uda;
	INT8U chg;
	//INT8U ret, i;

	switch(m_Env.UDA_Reset_En)
	{
		case 0:
			for( Uda = 0; Uda < UDA_MAX; Uda++)
			{
				if(Uda == UDA_Spurious) continue;
				if(Uda == UDA_Link) continue;

				/*
				/////////////if(m_Uda.UDA_Rst_Cnt[Uda] >= SUPPRESS_MAX)
				*/
				{
					if(auto_rst_req[Uda] == 1) 
					{
						m_Env.UDA_Reset_En = 1;
						OSTimerReset( &UDA_Rst_Dly_Timer );
						
						print_agent(DEBUG_UDA, VT_BOLD_RED, "[UDA] PSU RST Req (0x%02X)", Uda );
					}
				}
			}
			break;
		
		case 1:	
			if( OSTimerGet_msec(&UDA_Rst_Dly_Timer) >= 1000 ) // 5000 -> 1000
			{
				UDA_Auto_Reset_EEP_Save();
				m_Env.UDA_Reset_En = 2;
				OSTimerReset( &UDA_Rst_Dly_Timer );
			}
			break;
	
		case 2:		
			if( OSTimerGet_msec(&UDA_Rst_Dly_Timer) >= 2000 ) // 3000 -> 2000
			{					
			#ifdef __TYPE__PRU__
				m_Env.psu_reset_req = 1;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "!! PSU Reset REQ to PRU-FPGA / by UDA");
				PRU_PSU_Reset_Reg_Ctrl();
			#else // AAU 		
				m_Env.is_reboot = 1;
			#endif
			
				print_agent(DEBUG_UDA, VT_BOLD_RED, "!!!!!! Reset By UDA !!!!!!");
				//syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! Reset By UDA !!!!!!\n" VT_END_CR);

				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "!! PSU Reset : System Halt, by UDA.");
				system("halt");

				// PSU Reset Fail ----------------------------------
			#ifdef __TYPE__PRU__
				OSTimeDly( 10*OS_DELAY_1sec); // waiting reset
				
				m_Env.UDA_Reset_En = 3;
				
				print_agent(DEBUG_UDA, VT_BOLD_RED, "[UDA] ! PSU Reset Fail");
				//syslog( LOG_CRIT, VT_BOLD_RED "[UDA] ! PSU Reset Fail\n" VT_END_CR);
				
				OSTimeDly(OS_DELAY_100msec);
								
				m_Env.is_reboot = 1;
			#endif	
				//--------------------------------
			}
			break;
	}		
	//------------------------------------------------------------	
	for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
	{
		if(Uda == UDA_Link) continue;
		
		if(auto_rst_req[Uda])
		{
			UDA_Alarm_Set(Uda, _SET_);
			Chk_UDA_Alm_gREMS();
		}
	}		
	//-------------------------------------------------------------
	// suppress
	if(ALM_JIG_MODE != ALM_JIG_MODE_CODE) 
	{
		// Do below after DTU(Register) is Ready @ 5G	
		chg = UDA_RST_Suppr_f_Prev ^ m_Uda.RST_Suppr_f;

		// suppress alarm :
		if(chg)
		{
			for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
			{
				if(Uda == UDA_Link) continue;
				
				if(chg & cbits[Uda])
				{
					if(m_Uda.RST_Suppr_f & cbits[Uda])
						UDA_Suppr_Alm_Ctrl( Uda, 1 );
					else
						UDA_Suppr_Alm_Ctrl( Uda, 0 );
				}
			}
			Chk_UDA_Alm_gREMS();
		}
		UDA_RST_Suppr_f_Prev = m_Uda.RST_Suppr_f;
	}
}
//---------------------------------------------------------------------------



//----------------------------------------------------------------
// 알람 정상 --> 발생된 경우 1 리턴, 변동 없으면 0 리턴.
INT8U Is_Alm_Gen( INT8U Prev, INT8U Curr, INT8U bit_posi )
{
	if( (Prev & cbits[bit_posi]) != (Curr & cbits[bit_posi]) )
	{
		if(Curr & cbits[bit_posi]) return 1;
	}
	return 0;
}
//----------------------------------------------------------------

//-------------------------------------------
INT8U Is_Alm_Now( INT8U Curr, INT8U bit_posi )
{
	if( Curr & cbits[bit_posi] ) return 1;
	else return 0;
}
//----------------------------------------------------------------





/*-----------------------------------------------------------------------
* UDA Reset 목적 : 
	다발 발생/유지 되는 국소 --> 자체 reset  --> 정상 복귀 목적 

* UDA Auto reset : 
  -> 알람 30분(Link는 10분) 유지 or 20회/30분 이상 발생 : Reset

* UDA Reset : Auto Reset Suppression
	- Auto Reset 3회/120분 넘으면 : suppression (== Auto Reset 기능 Off)
	- 30분 이상, 정상 상태 유지 : suppression 해제



(1) After Reset, UDA_alarm_flag가 있으면, UDA Alarm(clock/cpri) = 1 로 시작.
	(Chk_Auto_Reset_By_UDA_Suppress() 함수에서 처리됨.	)
(2) 2020.04.10 : gREMS에 reset.reason 을 report 한후, 	
	UDA Alarm(clock/cpri) = 0 하고, clear report 했던 것을 
	하지 않도록(즉, uda alarm 유지 하도록) 수정.
(3) 30m 정상 유지 시, UDA clear 되도록 함.(2020.04.09)


//-----------------------------------------------------------------------*/


static OS_TIMER Rst_Proc_ms_Timer; // Algorithm 주기관련.
static INT8U UDA_Oper_Mode_Prev[UDA_MAX];

void Proc___Auto_Rst_UDA(void) // UDA_RESET_Suppression
{	
	static INT8U init = 0;
	static INT8U UDA_Alm_Prev = 0;	
	INT8U Curr_Alm = 0;
	INT8U rst_f[UDA_MAX] = {0,0,0,0};
	INT8U init_req=0;
	INT8U Uda;
	INT8U c;
	INT32U time_msec;
	
	if(init == 0) 
	{
		init = 1;
		for(Uda = UDA_Spurious; Uda < UDA_MAX; Uda++)					
			UDA_Oper_Mode_Prev[Uda] = m_Eep.UDA_Oper_Mode[Uda];
		
			
		OSTimerReset( &Rst_Proc_ms_Timer);
	}
	
	//--------------------------------------------------------------
	// every 1sec..
	//--------------------------------------------------------------
	if( OSTimerGet_msec(&Rst_Proc_ms_Timer) <= 1000 ) return;
	OSTimerReset( &Rst_Proc_ms_Timer);

	//--------------------------------------------------------------
	// if not-use & alarm, clear alarm.
	//--------------------------------------------------------------	
	for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
	{
		if(Uda == UDA_Spurious) continue;
		if(Uda == UDA_Link) continue;
		
		if(m_Eep.UDA_Oper_Mode[Uda] == AR_No_Action)
		{
			if(IS__UDA_Alarm(Uda)) 
			{
				UDA_Alarm_Set(Uda, _CLR_);
				Chk_UDA_Alm_gREMS(); // 2020.04.08
			}
			
			UDA_Suppr_Alm_Ctrl(Uda, _CLR_);
		}
	}	
	//--------------------------------------------------------------


	//--------------------------------------------------------------
	// init
	//--------------------------------------------------------------
	for(Uda = UDA_Link; Uda < UDA_MAX; Uda++)
	{	
		if(Uda == UDA_Spurious) continue;
		if(Uda == UDA_Link) continue;
		
		if(UDA_Oper_Mode_Prev[Uda] != m_Eep.UDA_Oper_Mode[Uda]) init_req = 1;
		else if(m_Eep.UDA_Oper_Mode[Uda] == AR_No_Action)     init_req = 2;
		else init_req = 0; // init zero

		UDA_Oper_Mode_Prev[Uda] = m_Eep.UDA_Oper_Mode[Uda];
		//---------------------------
		if( init_req )
		{			
			//D_Printf( DEBUG_UDA, "@@ (Uda: %d) init_req(%d) : mode[%d]\n ",
			//	Uda, init_req, m_Eep.UDA_Oper_Mode[Uda]);
				
			m_Uda.UDA_Rst_Cnt[Uda] = 0;
			m_Uda.Rst_Suppress[Uda] = 0;	

			UDA_Alm_Prev &= nbits[Uda]; 
			UDA_Auto_Rst_All_Init(Uda);

			if(IS__UDA_Alarm(Uda)) 
			{
				UDA_Alarm_Set(Uda, _CLR_);
				Chk_UDA_Alm_gREMS(); // 2020.04.08
			}
			
			OSTimerReset(&Alm_Clr_ms_Timer[Uda]); // @ AR_Alm_Only 일때 사용.

			auto_rst_req[Uda] = 0;
			
			m_Uda.time_backup_ms[Uda] = 0;
		}		
	}

	for(Uda = UDA_Link; Uda < UDA_MAX; Uda++)
	{		
		if(Uda == UDA_Link) continue;
		
		if(auto_rst_req[Uda]) // init, if under reset...
		{
			UDA_Alm_Prev &= nbits[Uda];
		}
	}
	//================================================================
			
	//================================================================
	// Get Current Alarm
	//==================
	Curr_Alm = 0; // init
	if(Is_SFP_Inserted(CPRI_0) && m_St.AlmSt.A_2.CPRI_0__) 
		Curr_Alm |= cbits[UDA_PD_CP];
	if(Is_SFP_Inserted(CPRI_1) && m_St.AlmSt.A_2.CPRI_0_a) 
		Curr_Alm |= cbits[UDA_PD_CP];	
	if(m_St.AlmSt.A_4.Clock)  
		Curr_Alm |= cbits[UDA_Clock];	

	if((Is_SFP_Inserted(CPRI_0)==0) && (Is_SFP_Inserted(CPRI_1)==0))  // if both not installed
	{
		Curr_Alm &= nbits[UDA_Link];
		Curr_Alm &= nbits[UDA_PD_CP];
		Curr_Alm &= nbits[UDA_Clock];
	}	

	//================================================================
	
	//D_Printf( DEBUG_UDA, "@@ Cur : dig[%d], pd[%d]\r\n", (Curr_Alm & cbits[UDA_Clock]), (Curr_Alm & cbits[UDA_PD_CP]) );
	//D_Printf( DEBUG_UDA, "@@ rst_req : dig[%d], pd[%d] \r\n ", auto_rst_req[UDA_Clock], auto_rst_req[UDA_PD_CP] );
	//D_Printf( DEBUG_UDA, "@@ mode : dig[%d], pd[%d]\r\n", m_Eep.Auto_Rst_Onoff[UDA_PD_CP], m_Eep.UDA_Oper_Mode[UDA_Clock] );
	//D_Printf( DEBUG_UDA, "@@ sfp : m[%d], e[%d]\r\n", sfp_ins[0], sfp_ins[1] );
	
	//================================================================
	for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
	{	
		if(Uda == UDA_Link) continue;
		
		if(auto_rst_req[Uda] > 0) // reset 확정안된 경우만 처리.
			continue;
			
		if(m_Eep.UDA_Oper_Mode[Uda] == AR_No_Action)
			continue;
		
		//---------------------------------------------
		// 1. Check alarm count is over 20 or not for 30[min]
		//---------------------------------------------
		if(Alm_Cnt[Uda]==0) // if no alarm, init.
			OSTimerReset( &Alm_Cnt_ms_Timer[Uda] ); // Start When Alm is Generated.

		c = Get_UDA_Char(Uda);
		
		if(Is_Alm_Gen(UDA_Alm_Prev, Curr_Alm, Uda))
		{
			print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] UDA_Alm_Inc. %02X != %02X, %d", c, UDA_Alm_Prev, Curr_Alm, Uda);
			UDA_Alm_Inc(Uda, c);
		}		

		if(OSTimerGet_msec(&Alm_Cnt_ms_Timer[Uda]) >= CNT_Ref_ms) // check alarm count is over or not during 30m(every 30m)
		{		
			print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] 30 [min] End. Re-Start.", c);

			if(Alm_Cnt[Uda] >= Alm_Cnt_Max[Uda])
			{
				rst_f[Uda] = 1;
				print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA_%c] Max Cnt", c);

				// 2020.06.22 : Alarm Only에서 alarm set, gREMS 안되는 부분 수정.
				if(m_Eep.UDA_Oper_Mode[Uda] == AR_Alm_Only)
				{
					print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA_%c] Count Mode, Alarm Mode, Alarm set/gEMS", c);
					UDA_Alarm_Set(Uda, _SET_);
					Chk_UDA_Alm_gREMS();
				}
			}
			
			// restart
			OSTimerReset( &Alm_Cnt_ms_Timer[Uda] );
			Alm_Cnt[Uda]=0;
		}
		//---------------------------------------------
		// 2. Check whether Alarm is keeping for 30[mim] / 10[mim]
		//---------------------------------------------
		if(Is_Alm_Now(Curr_Alm, Uda))
		{	
			OSTimerReset( &Alm_Clr_ms_Timer[Uda] );
			
			if(OSTimerGet_msec(&Alm_Keep_ms_Timer[Uda]) >= DIG_KEEP_Ref_ms)
			{			
				rst_f[Uda] = 1;
				OSTimerReset( &Alm_Keep_ms_Timer[Uda] ); // restart
				print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA_%c] Alarm maintain Over!", c);
				
				UDA_Alarm_Set(Uda, _SET_);
				UDA_Auto_Rst_All_Init(Uda);

				// 2020.06.22 : Alarm Only에서 alarm set, gREMS 안되는 부분 수정.
				if(m_Eep.UDA_Oper_Mode[Uda] == AR_Alm_Only)
				{
					print_agent(DEBUG_UDA, VT_BOLD_YELLOW, "[UDA_%c] Time Mode, Alarm Mode, Alarm set/gEMS", c);
					UDA_Alarm_Set(Uda, _SET_);
					Chk_UDA_Alm_gREMS();
				}
			}			
		}
		else 
		{
			OSTimerReset( &Alm_Keep_ms_Timer[Uda] );
			
			if(IS__UDA_Alarm(Uda)) 
			{
				if( OSTimerGet_msec(&Alm_Clr_ms_Timer[Uda]) >= Normal__Keep_ms ) 
				{
					OSTimerReset( &Alm_Clr_ms_Timer[Uda] );

					UDA_Alarm_Set(Uda, _CLR_);
					Chk_UDA_Alm_gREMS(); // 2020.04.08
				}
			}
			else
				OSTimerReset( &Alm_Clr_ms_Timer[Uda] );
		}		
	}
	//------------------------------------------------------------------
	for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
	{
		// Update	
		ctrlBits( &UDA_Alm_Prev, (Curr_Alm & cbits[Uda]), Uda);

		// if not "AR_Reset" 아니면, clear flag..	
		if(m_Eep.UDA_Oper_Mode[Uda] != AR_Reset) 
			rst_f[Uda] = 0;
	}
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	// Reset-decision according to priority 
	// Alarm only 이어도 아래 코드 실행되므로, 주의..
	if(rst_f[UDA_Clock] || rst_f[UDA_PD_CP] || rst_f[UDA_Link]) 
	{			
		//-------------------------
		for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
			if(rst_f[Uda]) UDA_Auto_Rst_All_Init(Uda);
		
		//-------------------------	
		// Decide Reset according to priority : Clock > PD_CPRI > Link
		//-------------------------
		if((m_Uda.Rst_Suppress[UDA_Clock] == 0) && rst_f[UDA_Clock])
		{
			// clear other flag
			rst_f[UDA_PD_CP] = 0; 
			rst_f[UDA_Link] = 0;
		}
		if(( m_Uda.Rst_Suppress[UDA_PD_CP] == 0) && rst_f[UDA_PD_CP])
		{ 			
			// clear other flag
			rst_f[UDA_Link] = 0; 
		}		
		//-------------------------
		print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA] flag : clock[%d], pd-cpri[%d], link[%d]", 
			rst_f[UDA_Clock], rst_f[UDA_PD_CP], rst_f[UDA_Link] );
			
		//-------------------------
			 if (rst_f[UDA_Clock]) { Uda = UDA_Clock; c = Get_UDA_Char(UDA_Clock);}
		else if (rst_f[UDA_PD_CP]) { Uda = UDA_PD_CP; c = Get_UDA_Char(UDA_PD_CP);}
		else                       { Uda = UDA_Link;  c = Get_UDA_Char(UDA_Link); }		
		//-------------------------
		if( rst_f[Uda] )
		{		
			if(m_Uda.Rst_Suppress[Uda]) // if already suppressed, skip reset.
			{
				OSTimerReset( &Suppress_OFF_ms_Timer[Uda] ); // re-check
				print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] Reset rejected; Already Suppressed.", c);
			}
			else // suppress 아니어야 리셋가능, ( == Not-suppress State Now)
			{				
				auto_rst_req[Uda] = 1;
				m_Uda.UDA_Rst_Cnt[Uda]++;
				m_Uda.time_backup_ms[Uda] += OSTimerGet_msec(&Suppress_ON__ms_Timer[Uda]);

				print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] Reset cnt(%d), backup_time %d [ms]", 
					c, m_Uda.UDA_Rst_Cnt[Uda], m_Uda.time_backup_ms[Uda]);
			}
		}		
	}		
	//================================================================
	// Suppress check	
	INT8U Rst_Suppress_Req;
	INT8U Time_Restart_Req;
	
	for(Uda = UDA_Link; Uda <= UDA_PD_CP; Uda++)
	{
		Rst_Suppress_Req = 0; // init 0
		Time_Restart_Req = 0; // init 0
		
		c = Get_UDA_Char(Uda);
		
		     if(m_Eep.UDA_Oper_Mode[Uda] != AR_Reset) init_req=1;
		else if(m_Uda.UDA_Rst_Cnt[Uda] == 0)          init_req=1;
		else                                          init_req=0;		
				
		//----------------------------------------------[ reset cnt is 0]
		if( init_req )
		{
			m_Uda.Rst_Suppress[Uda] = 0;
			OSTimerReset( &Suppress_ON__ms_Timer[Uda] ); // re-check
			OSTimerReset( &Suppress_OFF_ms_Timer[Uda] ); // clr

			Set_UDA_Alarm__flag(Uda, _F_CLR_);
			Set_UDA_RST_Suppr_f(Uda, _F_CLR_);			
			m_Uda.time_backup_ms[Uda] = 0;

			continue;
		}
		//----------------------------------------------[ reset cnt >= 1]	
		//----------------------------------------------< not suppress state >
		if(m_Uda.Rst_Suppress[Uda] == 0)
		{
			//------------------------------------------
			// Check Over 120[min] or not
			//------------------------------------------
			time_msec = OSTimerGet_msec(&Suppress_ON__ms_Timer[Uda]);								
			time_msec += m_Uda.time_backup_ms[Uda];

			if(m_Uda.UDA_Rst_Cnt[Uda] >= SUPPRESS_MAX)
				Rst_Suppress_Req = 0x01;
							
			if(time_msec >= SUPPRESS_ON__ms)
				Time_Restart_Req = 1; // Over 120[min]			
			//--------------------------------
			if(Rst_Suppress_Req)
			{					
				m_Uda.Rst_Suppress[Uda] = 1;

				OSTimerReset( &Suppress_OFF_ms_Timer[Uda] ); // 해제시간 시작.
				OSTimerReset( &Suppress_ON__ms_Timer[Uda] ); // init

				m_Uda.time_backup_ms[Uda] = 0;
				
				Set_UDA_Alarm__flag( Uda, _CLR_); // clear
				Set_UDA_RST_Suppr_f( Uda, _SET_); // set
				
				print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] Suppressed Now (%d)", c, Rst_Suppress_Req );
			}
			else if(Time_Restart_Req)
			{					
				m_Uda.UDA_Rst_Cnt[Uda] = 0;
				
				OSTimerReset( &Suppress_ON__ms_Timer[Uda] ); // init 					
				
				m_Uda.time_backup_ms[Uda] = 0;
				
				Set_UDA_Alarm__flag( Uda, _CLR_);
				Set_UDA_RST_Suppr_f( Uda, _CLR_);
				
				print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] Suppress 120 min Re-Start..", c );
				
				Time_Restart_Req = 0;
			}
		}
		//----------------------------------------------< now suppress State >
		else
		{
			if( OSTimerGet_msec(&Suppress_OFF_ms_Timer[Uda]) >= Suppress_Off_ms ) // 해제되면....
			{
				m_Uda.Rst_Suppress[Uda] = 0;
				m_Uda.UDA_Rst_Cnt[Uda] = 0; // suppress 해제되어야 global cnt clear
				
				Set_UDA_RST_Suppr_f(Uda , _CLR_); // clear
				UDA_Suppr_Alm_Ctrl(Uda, _CLR_); // 2019.05.07

				print_agent(DEBUG_UDA, VT_BOLD_WHITE, "[UDA_%c] Suppression Is Cleared.", c );

				// 2020.04.08 : suppress 해제되었는데, 기존 uda_clock/uda_pd_cpri alarm 남아 있으면 alarm clear

				if(IS__UDA_Alarm(Uda))
				{
					UDA_Alarm_Set(Uda, _CLR_);
					Chk_UDA_Alm_gREMS();
				}
			}
		} // < now suppress State >	
	} // for loop
	//================================================================		
}
//----------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
// user 제어일때만 호출되어야 함. 

// =========================================>> merge 시, AID 연결 필요.(control.c)

void Use_UDA_Oper_Mode_Ctrl(INT8U Uda, INT8U Mode)
{	
	if(Uda >= UDA_MAX) return;		
	if(Uda == UDA_Spurious) return;
	if(Uda == UDA_Link) return;	

	if(Mode >= AR_MAX) return;
	
	//if(m_Eep.UDA_Oper_Mode[Uda] == Mode) // if already same
	//	return;

	m_Eep.UDA_Oper_Mode[Uda] = Mode;

	m_Uda.UDA_Rst_Cnt[Uda] = 0;
	m_Uda.Rst_Suppress[Uda] = 0;

	Set_UDA_Alarm__flag(Uda, 0);		
	UDA_Auto_Rst_All_Init(Uda);		
	auto_rst_req[Uda] = 0;
		
	m_Uda.time_backup_ms[Uda] = 0;
}

void Init_UDA_Proc(void)
{
	INT8U Uda;

	for(Uda = UDA_Spurious; Uda < UDA_MAX; Uda++)
	{
		if(m_Eep.UDA_Oper_Mode[Uda] > AR_Alm_Only) 
			m_Eep.UDA_Oper_Mode[Uda] = AR_No_Action;

		UDA_Oper_Mode_Prev[Uda] = m_Eep.UDA_Oper_Mode[Uda];

		// rcu 리셋되면, suppress off : 무조건 다시 시작.
		OSTimerReset(&Suppress_OFF_ms_Timer[Uda]);

		UDA_Auto_Rst_All_Init(Uda);
		auto_rst_req[Uda] = 0;
	}	
	OSTimerReset( &Rst_Proc_ms_Timer );
	OSTimerReset( &Spurious_Proc_ms_Timer[_100M] );
	OSTimerReset( &Spurious_Proc_ms_Timer[Alpha] );
}


//----------------------------------------------------------------------------------------

/* EOF */

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

