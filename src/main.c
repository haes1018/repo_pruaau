/////////////////////////////
//	MAIN.C
/////////////////////////////
#define MAIN_GLOBALS
#include "includes.h"

void Chk_Same_Process(void);


/////////////////////////////

/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

static Thread_ARG_Var	thread_arg[MAX_THREAD_NUM];
static pthread_t 		t_thread[MAX_THREAD_NUM];
static INT32U			thread_arg_index = 0;

static pthread_mutex_t	_mutex_Status;

int daemon_mode = 0;
int rcu_cli_mode = 0; // 1(cli, default), 2(cli, dtu)
int ver_disp_mode = 0;
int ver_return_mode = 0;


void DeInit_GPIO_ANTS( void );





void StatusSem_Init( void )
{
	if (pthread_mutex_init(&_mutex_Status, NULL))
	{
		syslog( LOG_ERR, "[RCU] !! Mutex Init Failed");
		exit(EXIT_FAILURE);
	}
}

void Status_Lock( void )
{
	pthread_mutex_lock( &_mutex_Status );
}
void Status_Unlock( void )
{
	pthread_mutex_unlock( &_mutex_Status );
}

void Free_MemAlloc(void) // v
{
	Free_Alm_Log_Buf();
}


static OS_TIMER task_end_timer;

void End_task(void)			///// YCSO TASK END
{
	int x = 0;
	int cnt, cnt_accum;
	INT8U task_cnt;

	for( x = (_TASK_MAX_NUM_-1); x >= 0; x--)
		m_Env.task_flag[x] = _CLR_;		///// YCSO TASK END
		
	OSTimerReset(&task_end_timer);	
	//memcpy(prev, m_Env.task_END_status, _TASK_MAX_NUM_);
#if 1
	cnt_accum = 0;
	for(x = (_TASK_MAX_NUM_-1); x > 0; x--)
	{
		cnt = 0;
		if(m_Env.task_END_status[x] == 0) 
		{
			//print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "- TASK %02d END, %d[ms]", x, OSTimerGet_msec(&task_end_timer));	
			continue;
		}
			
		if(m_Env.task_loop[x])
		{
			do
			{
				OSTimeDly(OS_DELAY_10msec);
				cnt++;
				cnt_accum++;
			}
			while(m_Env.task_END_status[x] && (cnt < 100));
		}
		print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "TASK %02d END, %d", x, cnt);	
	}
	print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "TASK END %d, %d[ms]", 
		cnt_accum, OSTimerGet_msec(&task_end_timer));

	task_cnt = 0;
	cnt = 0;
	for(x = (_TASK_MAX_NUM_-1); x > 0; x--)
	{	
		if(m_Env.task_loop[x])
		{
			task_cnt++;
			
			if(m_Env.task_END_status[x] == 0)
				cnt++;	
			else	
				print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "!! TASK %02d", x);	
		}
	}	
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "TASK cnt %d/%d", cnt, task_cnt); 
#else
	cnt_accum = 0;	
	do
	{
		OSTimeDly(OS_DELAY_100msec);
		cnt_accum++;
		task_cnt = 0;
		for(cnt = 0, x = (_TASK_MAX_NUM_-1); x >= 0; x--)
		{	
			if(m_Env.task_loop[x])
			{
				task_cnt++;
				if(m_Env.task_END_status[x] == 0)
				{
					cnt++;
					if(prev[x] != m_Env.task_END_status[x])
						print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "TASK %02d END", x); 
				}
			}
			prev[x] = m_Env.task_END_status[x];
		}
	}while((cnt < task_cnt)&&(cnt_accum < 150000));

	print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "TASK END %d cnt_accum, %d/%d", cnt_accum, cnt, task_cnt);

	for(x = (_TASK_MAX_NUM_-1); x >= 0; x--)
	{
		if(m_Env.task_END_status[x] != 0)
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "TASK %02d END Fail", x);
		
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "TASK END %d[ms]", OSTimerGet_msec(&task_end_timer));		
	}
#endif		
}

void End_Program(int a)
{
	(void)a;
	End_task();

#ifdef WATCH_DOG_EN
	watchdog_end();
#endif
	Close_UDP_fd();
	Free_MemAlloc();
	
#ifdef __MAKER_ANTS__	// PRU or AAU
	DeInit_GPIO_ANTS();
	Close_GPIO_Dev();
	i2c_device_close();
	Close_SPI_FPGA();	
	Close_AXI_Reg();	
	closelog ();
	Close_LM75_Temp(); // AAU/PRU of ANTS
	Close_ADC_ic_file_all(); // 2019.04.15 : ANTS PRU/AAU Only
	Close_System_Sock_fd(); // 2019.05.15
	#ifdef __TYPE__PRU__ // PRU
	Close_A_PRU_Mmap();
	#endif
#endif


	udp_rx_que_pipe_destroy();

	sync();
	syslog( LOG_WARNING, VT_BOLD_RED "[RCU] End Program." VT_END_CR);
	exit(0);
}



/*
------------------------------------------------------------


------------------------------------------------------------
*/

/*
void Proc_Termination(INT8U index)
{
	// 0. 그 동안 read, write 했던 내용 저장.
	SaveEEP();
	sync();

	// 1. DDTU 종료 처리 요청
	// 2. daemon으로 halt signal 전송 script 실행
	// 3. sys-app. 종료 : optional
	switch(index)
	{
		case Term_Rst_REQ:
			//Send_Term_To_DTU(1);
			Sys_Reboot(index);
			break;

		case Term_Fw_Apply:
			break;

		case Term_PSU_Rst_REQ:
			Sys_Halt(index);
			break;

		case Term_Super_Cap:
			//Send_Term_To_DTU(1);
			Sys_Halt(index);
			break;

		case Term_Push_Button:
			Sys_Halt(index);
			OSTimeDly(2 * OS_DELAY_1sec);
			break;
	}
}
*/


void *WDT_Task(void *arg)
{
	INT32U co, led;	
	#ifdef __TYPE__PRU__
	INT32U wdt_cnt=0;
	#endif

 	Thread_Priority_Init((Thread_ARG_Var*)arg, 1);

	co = 0;
	led = 0;
	//---------------------
	while( m_Env.task_flag[_TASK_WDT_] )		///// YCSO TASK END
	{
		DebugConfig();

		OSTimeDly( OS_DELAY_100msec );
		Update__Task_Count(_TASK_WDT_);
		Proc___Task_Working();		

		if(m_Env.is_down_load)
		{			
			if(OSTimerGet_msec( &m_Env.dl_timer) > m_Env.dl_time_out)
			{
				m_Env.is_down_load = 0;
			   	m_Env.is_down_load_me = 0;
			   	m_Env.Dig_Down_AMP_Off_Req = 0;
				
				if(m_St.DL_State[DN_RCU][DL_ST] == DL_St_Now)
				{
					m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;
				}
			}
		}
		
		SaveEEP();
		

		if(m_Env.app_exit_req)
		{
			m_Env.app_exit_req = 0;

			OSTimeDly( OS_DELAY_1sec * 2);  // GUI 요청에 응답할 시간을 위해.

			D_Printf( DEBUG_SYSTEM, "[RCU] make restart-tmp \n");

			//fp = fopen( RCU_RESTART_TMP, "wb" ); // save crc ascii 4byte
			//if( fp != NULL )
			//	fclose(fp);
			
			raise(SIGTERM);
		}

		if( m_Env.is_reboot )
		{
			/* 
			-- update needed for PRU --
			switch(rcu_reset_req)
			{
				case Rst_By_User:
				case Rst_By_XXX:
					break;
			}			
			syslog( LOG_WARNING, "reboot System %d", rcu_reset_req );			
			Proc_Termination(Term_Rst_REQ);
			-- update needed for PRU --
			*/
		
			watchdog_live();
			SaveEEP();
			
			syslog( LOG_WARNING, "[RCU] Reboot System %d", m_rst_info.reset_info );
			
			OSTimeDly( OS_DELAY_100msec*5 );
			sync();
			OSTimeDly( OS_DELAY_100msec*5 );
			OSTimeDly( OS_DELAY_500msec );
			system("reboot");
		}
		CheckCommFail();

	//--------------------------------
	#ifdef __MAKER_ANTS__	// PRU or AAU
	//--------------------------------
		co++;
		if( co >= 5 ) // every 500ms
		{
			#ifdef WATCH_DOG_EN
			watchdog_live();
			#endif
			co=0;
			led = ~led;
			
			// 2019.04.04 : AAU : RUN LED Added. 
			if( led ) RUN_LED__ON();
			else 	  RUN_LED_OFF();			
		}

		#ifdef __TYPE__PRU__
		wdt_cnt++;
		if(wdt_cnt & 0x01) WDT_Toggle_H();
		else               WDT_Toggle_L();
		#endif
	//--------------------------------	
	#endif	
	//--------------------------------
	
	}
	m_Env.task_END_status[_TASK_WDT_] = _CLR_;
	pthread_exit(NULL);
}



/*
------------------------------------------------------------


------------------------------------------------------------
*/
static void APP_Task_Creat( void* (*task)(void *p_arg), INT32U arg_v, void *p_arg, INT32U pri )
{
	int res;

	if(thread_arg_index >= MAX_THREAD_NUM)
	{
		syslog(LOG_ERR, "[RCU] !! Thread Count Max");
		return;
	}

	thread_arg[thread_arg_index].priority = pri;
	thread_arg[thread_arg_index].Arg_v = arg_v;
	thread_arg[thread_arg_index].pArg = p_arg;

	res = pthread_create(&t_thread[thread_arg_index], NULL, task, (void *)&thread_arg[thread_arg_index]);

	if (res != 0) 
	{
		syslog(LOG_ERR, "[RCU] !! Thread Creation Failed.");
		exit(EXIT_FAILURE);
	}
	thread_arg_index++;
}

/*
------------------------------------------------------------
	INIT GPIO
	ANTS
------------------------------------------------------------
*/
#ifdef __MAKER_ANTS__
void Init_GPIO_ANTS( void )
{	
#ifdef __TYPE__PRU__
	//----------------------------------------------------
	// pin - output
	//----------------------------------------------------
	GPIO_Init( GPIO_RUN_LED );
	GPIO_Init( GPIO_PS_WDOG_CLK );	

	GPIO_Init( GPIO_PAU_0_RST_O );
	GPIO_Init( GPIO_PAU_0_RSV_O );
	GPIO_Init( GPIO_PAU_0_En_O );
	GPIO_Init( GPIO_PAU_1_RST_O	);
	GPIO_Init( GPIO_PAU_1_RSV_O );
	GPIO_Init( GPIO_PAU_1_En_O );
	GPIO_Init( GPIO_PAU_2_RST_O	);
	GPIO_Init( GPIO_PAU_2_RSV_O	);
	GPIO_Init( GPIO_PAU_2_En_O );
	GPIO_Init( GPIO_PAU_3_RST_O	);
	GPIO_Init( GPIO_PAU_3_RSV_O	);
	GPIO_Init( GPIO_PAU_3_En_O );	
	GPIO_Init( GPIO_OPT_SW_DC_EN );	
	GPIO_Init( GPIO_OPT_SW_Status0 );		
	GPIO_Init( GPIO_OPT_SW_Status1 );		
	GPIO_Init( GPIO_CIM_DL_0_CPL_PS );
	GPIO_Init( GPIO_CIM_DL_1_CPL_PS	);
	GPIO_Init( GPIO_CIM_DL_2_CPL_PS	);
	GPIO_Init( GPIO_CIM_DL_3_CPL_PS	);		
	GPIO_Init( GPIO_PS_TO_PL_RESET);
	GPIO_Init( GPIO_BOARD_RESET);

	GPIO_DIR_Mode_Out( GPIO_PS_WDOG_CLK, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_RUN_LED, GPIO_VALUE_HIGH );	
	
	// PAU RST, Reserved, Enable : defult OUT High
	GPIO_DIR_Mode_Out( GPIO_PAU_0_RST_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_0_RSV_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_0_En_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_1_RST_O, GPIO_VALUE_HIGH);
	GPIO_DIR_Mode_Out( GPIO_PAU_1_RSV_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_1_En_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_2_RST_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_2_RSV_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_2_En_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_3_RST_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_3_RSV_O, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_PAU_3_En_O, GPIO_VALUE_HIGH );	

	//----------------------------------
	// 2019.04.29 : Default High(ON) Control
	// @ uboot, must be LOW
	//----------------------------------	
	GPIO_DIR_Mode_Out( GPIO_OPT_SW_DC_EN, GPIO_VALUE_HIGH );	
	
	/*	
	// 2019.05.20 : delete below	
	GPIO_DIR_Mode_Out( GPIO_OPT_SW_Status1, GPIO_VALUE_HIGH );			
	*/

	
	// 2019.04.06 : default Low Control
	GPIO_DIR_Mode_Out( GPIO_CIM_DL_0_CPL_PS, GPIO_VALUE_LOW );
	GPIO_DIR_Mode_Out( GPIO_CIM_DL_1_CPL_PS, GPIO_VALUE_LOW );
	GPIO_DIR_Mode_Out( GPIO_CIM_DL_2_CPL_PS, GPIO_VALUE_LOW );
	GPIO_DIR_Mode_Out( GPIO_CIM_DL_3_CPL_PS, GPIO_VALUE_LOW );		
	GPIO_DIR_Mode_Out( GPIO_OPT_SW_Status1 , GPIO_VALUE_HIGH);///// YCSO added 190624, 2019.06.26 default High
	
	GPIO_DIR_Mode_Out( GPIO_PS_TO_PL_RESET, GPIO_VALUE_HIGH );	
	GPIO_DIR_Mode_Out( GPIO_BOARD_RESET, GPIO_VALUE_HIGH );
	
	//----------------------------------------------------
	// pin - input
	//----------------------------------------------------
	GPIO_Init( GPIO_PAU_0_INS_I );
	GPIO_Init( GPIO_PAU_1_INS_I );
	GPIO_Init( GPIO_PAU_2_INS_I );
	GPIO_Init( GPIO_PAU_3_INS_I );		
	GPIO_Init( GPIO_PSU_AC_FAIL_I );	
	GPIO_Init( GPIO_DOOR_ALARM );

	GPIO_DIR_Mode_In( GPIO_PAU_0_INS_I );
	GPIO_DIR_Mode_In( GPIO_PAU_1_INS_I );
	GPIO_DIR_Mode_In( GPIO_PAU_2_INS_I );
	GPIO_DIR_Mode_In( GPIO_PAU_3_INS_I );		
	GPIO_DIR_Mode_In( GPIO_PSU_AC_FAIL_I );	
	GPIO_DIR_Mode_In( GPIO_DOOR_ALARM );

	GPIO_DIR_Mode_In( GPIO_OPT_SW_Status0 );///// YCSO added 190624
	//----------------------------------------------------
#endif	

#ifdef __TYPE__AAU__
	GPIO_Init( GPIO_LED_R );
	GPIO_Init( GPIO_LED_B );
	GPIO_Init( GPIO_BT_RESET );
	GPIO_Init( GPIO_RUN_LED  ); // 2019.04.04. Added.
	GPIO_Init( GPIO_CONF_DAT_EN ); // 2019.06.28
	
	GPIO_DIR_Mode_Out( GPIO_LED_R, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_LED_B, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_BT_RESET, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_RUN_LED, GPIO_VALUE_HIGH  ); // 2019.04.04. Added.
	GPIO_DIR_Mode_Out( GPIO_CONF_DAT_EN, GPIO_VALUE_HIGH); // 2019.07.02 : Low -> High
	
	#if 0
	GPIO_Init( GPIO_BLUE_RST );
	GPIO_Init( GPIO_FPGA_RST );
	GPIO_DIR_Mode_Out( GPIO_BLUE_RST, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_FPGA_RST, GPIO_VALUE_HIGH );
	//----------------------------
	GPIO_Init( GPIO_GPIO_2 );
	GPIO_Init( GPIO_GPIO_4 );
	GPIO_Init( GPIO_GPIO_1 );
	GPIO_Init( GPIO_GPIO_3 );	
	GPIO_DIR_Mode_Out( GPIO_GPIO_2, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_GPIO_4, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_GPIO_1, GPIO_VALUE_HIGH );
	GPIO_DIR_Mode_Out( GPIO_GPIO_3, GPIO_VALUE_HIGH );
	#endif
#endif
}


void DeInit_GPIO_ANTS( void )
{
#ifdef __TYPE__PRU__	
	GPIO_Exit( GPIO_RUN_LED );
	GPIO_Exit( GPIO_PS_WDOG_CLK );

	GPIO_Exit( GPIO_PAU_0_RST_O );
	GPIO_Exit( GPIO_PAU_0_RSV_O );
	GPIO_Exit( GPIO_PAU_0_En_O );
	GPIO_Exit( GPIO_PAU_1_RST_O	);
	GPIO_Exit( GPIO_PAU_1_RSV_O );
	GPIO_Exit( GPIO_PAU_1_En_O );
	GPIO_Exit( GPIO_PAU_2_RST_O	);
	GPIO_Exit( GPIO_PAU_2_RSV_O	);
	GPIO_Exit( GPIO_PAU_2_En_O );
	GPIO_Exit( GPIO_PAU_3_RST_O	);
	GPIO_Exit( GPIO_PAU_3_RSV_O	);
	GPIO_Exit( GPIO_PAU_3_En_O );	
	GPIO_Exit( GPIO_OPT_SW_DC_EN );	
	GPIO_Exit( GPIO_OPT_SW_Status0 );		
	GPIO_Exit( GPIO_OPT_SW_Status1 );		
	GPIO_Exit( GPIO_CIM_DL_0_CPL_PS );
	GPIO_Exit( GPIO_CIM_DL_1_CPL_PS	);
	GPIO_Exit( GPIO_CIM_DL_2_CPL_PS	);
	GPIO_Exit( GPIO_CIM_DL_3_CPL_PS	);		
	GPIO_Exit( GPIO_PS_TO_PL_RESET);	
	GPIO_Exit( GPIO_BOARD_RESET);

	GPIO_Exit( GPIO_PAU_0_INS_I );
	GPIO_Exit( GPIO_PAU_1_INS_I );
	GPIO_Exit( GPIO_PAU_2_INS_I );
	GPIO_Exit( GPIO_PAU_3_INS_I );		
	GPIO_Exit( GPIO_PSU_AC_FAIL_I );	
	GPIO_Exit( GPIO_DOOR_ALARM );
#endif


#ifdef __TYPE__AAU__
	GPIO_Exit( GPIO_LED_R );
	GPIO_Exit( GPIO_LED_B );
	GPIO_Exit( GPIO_BT_RESET );
	GPIO_Exit( GPIO_RUN_LED  ); // 2019.04.04. Added.
	GPIO_Exit( GPIO_CONF_DAT_EN	); ////// YCSO added 190702

	#if 0
	GPIO_Exit( GPIO_BLUE_RST );
	GPIO_Exit( GPIO_FPGA_RST );

	GPIO_Exit( GPIO_GPIO_2 );
	GPIO_Exit( GPIO_GPIO_4 );
	GPIO_Exit( GPIO_GPIO_1 );
	GPIO_Exit( GPIO_GPIO_3 );
	#endif
#endif
}

#endif





/*
------------------------------------------------------------


------------------------------------------------------------
*/

/*
-------------------
PRU : ZynqMP Reset Reason Register (TRM p1125) 
----------------------------
*/
enum
{
	ZYNQMP_EXTERNAL_POR     = 0x0100,
	ZYNQMP_INTERNAL_POR     = 0x0200,
	ZYNQMP_PMU_SYS_RESET    = 0x0400,
	ZYNQMP_PSONLY_RESET_REQ = 0x0800,
	ZYNQMP_SRST             = 0x1000,
	ZYNQMP_SOFT             = 0x2000,
	ZYNQMP_DEBUG_SYS        = 0x4000,
};

/*
-------------------
AAU : TI AM335X PRM_RSTST (TRM p1428)
-------------------
*/
enum
{
    AM335X_COLD_RESET     = 0x010000,
    AM335X_WARM_SW_RESET  = 0x020000,
    AM335X_WDT1_RESET     = 0x040000,
    AM335X_EXT_WARM_RESET = 0x080000,
    AM335X_ICE_PICK_RESET = 0x100000,
};

/*		

// H/W watchddg, after Dip switch off
# killall -1 watchdog


PRU : 
CPU-WDT : 구분
나머지 Power-ON, GPIO에 의한 reboot처리 & EXT-WDT를 구분 X

AAU : 모든 상태를 구분

Detailed Information Strings:
			PRU 						
			--------------------------- 
 Power-ON : 0x0100 : ZYNQMP_EXTERNAL_POR
  EXT-WDT : 0x0100 : ZYNQMP_EXTERNAL_POR 		
SW-Reboot : 0x0100 : ZYNQMP_EXTERNAL_POR  (GPIO) 
			0x2000 : ZYNQMP_SOFT (Zynq Internal)
  CPU-WDT : 0x0400 : ZYNQMP_PMU_SYS_RESET {v}

		   AAU
		   ---------------------------
Power-ON  : 0x010000 : AM335X_COLD_RESET 	
SW-Reboot : 0x020000 : AM335X_WARM_SW_RESET		   
 CPU-WDT  : 0x040000 : AM335X_WDT1_RESET
 EXT-WDT  : 0x080000 : AM335X_EXT_WARM_RESET	
*/


void Chk_ResetReason( void )
{
	int reason = 0;
	INT8U rst_reason = 0;

	//------------------------
	// Read Reset Info
	ResetInfo_Read( (INT8U*)&m_rst_info, sizeof(DownInfoSTR) );
	
	memcpy( (INT8U*)&m_Uda, (INT8U*)&m_rst_info.UDA, sizeof(UDA_STR) );
	memset( (INT8U*)&m_rst_info.UDA, 0, sizeof(UDA_STR) ); // init
	
	//-------------------------
	#ifdef __MAKER_ANTS__
	//-------------------------
	int fd;
	char buf[256];
	
	
	memset(buf, 0, 256);
	
	fd = open("/dev/reset_reason", O_RDWR);
	if (fd < 0)
	{
		printf("[RCU] {/dev/reset_reason} open failed ...\n");
		reason = 0;
	}
	else
	{
		reason = ioctl(fd, 0, 0);
		close(fd);

	}
	//-------------------------	
	#endif
	//-------------------------

	
	#ifdef __TYPE__PRU__
	switch(reason)
	{
		// CPU-WDT
		case ZYNQMP_PMU_SYS_RESET: 			
			rst_reason = Rst_By_WDT_RST;
			break;
		/*
		// Power-ON, EXT-WDT, SW-Reboot
		case ZYNQMP_EXTERNAL_POR: rst_reason = Rst_By_PwrFail; break;
		
		case ZYNQMP_INTERNAL_POR:
		case ZYNQMP_PSONLY_RESET_REQ:
		case ZYNQMP_SRST:
		case ZYNQMP_SOFT:
		case ZYNQMP_DEBUG_SYS:
			reason = 0;
			break;
		*/	

		default:
			switch(m_rst_info.reset_info)
			{
				case Rst_By_User:
				case Rst_By_Download:
				case Rst_By_PwrCtrl:
				/*
				2019.08.27 : add reset-reason
				*/
				case Rst_By_Vss:        
				case Rst_By_Uda_Pd_CPRI:
				case Rst_By_Uda_Link:
				case Rst_By_Uda_Clock:
				case Rst_By_Etc:
					rst_reason = m_rst_info.reset_info;
					break;

				default:
					rst_reason = Rst_By_PwrFail;
					break;
			}
			break;
	}
	#endif	

	#ifdef __TYPE__AAU__
	switch(reason)
	{
	    case AM335X_COLD_RESET:
	    case AM335X_ICE_PICK_RESET:
	    	rst_reason = Rst_By_PwrFail;
	    	break;
	    	
		case AM335X_WARM_SW_RESET:
			rst_reason = Rst_By_User;
			break;
			
		case AM335X_WDT1_RESET:
		case AM335X_EXT_WARM_RESET:
			rst_reason = Rst_By_WDT_RST;
			break;
		
		default:
			switch(m_rst_info.reset_info)
			{
				case Rst_By_User:
				case Rst_By_Download:
				case Rst_By_PwrCtrl:				
				/*
				2019.08.27 : add reset-reason
				*/
				case Rst_By_Vss:        
				case Rst_By_Uda_Pd_CPRI:
				case Rst_By_Uda_Link:
				case Rst_By_Uda_Clock:
				case Rst_By_Etc:
					rst_reason = m_rst_info.reset_info;
					break;

				default:
					rst_reason = Rst_By_PwrFail;
					break;
			}
			break;
	}
	#endif	

	//--------------------------
	// 2019.05.03
	//--------------------------
	/*
	// deleted on 2019.08.27 by adding-reset-reason
	//if(Chk_Auto_Reset_By_UDA_Suppress()) // reset by UDA
	//{
	//	rst_reason = Rst_By_Etc; 
	//}
	*/
	/*
	2019.08.27 : add reset-reason
		Instead of Rst_By_Etc 
		Use Rst_By_Uda_Pd_CPRI, Rst_By_Uda_Link, or Rst_By_Uda_Clock.				
	*/
	Chk_Auto_Reset_By_UDA_Suppress(); // reset by UDA
	//--------------------------
	
	m_St.ResetReason = rst_reason;
	m_St.ResetReason_cpy = rst_reason;
	m_St.ResetReason_log = rst_reason;
	
	syslog(LOG_WARNING, "[RCU] Start, reg (0x%x), reason(0x%x) %s.", 
			reason, rst_reason, Get_Rst_Reason_Str(rst_reason) );	

	SetResetInfo( Rst_None, 649 );
	m_Env.is_reboot = Rst_None;


	// SW-WDT 일때 0x404를 읽음.
	print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[ZYNQMP] reset reason 0x%X", reason); 



	
	
	// TEST
	//print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "reason string test : %s", Get__gREMS_Reset_Reason_Str(Rst_By_User)); 
	//print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "reason string test : %s", Get__gREMS_Reset_Reason_Str(Rst_By_PwrFail)); 
	//print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "reason string test : %s", Get__gREMS_Reset_Reason_Str(Rst_By_Etc)); 
	//print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "reason string test : %s", Get__gREMS_Reset_Reason_Str(Rst_By_PwrCtrl));
}

/*
------------------------------------------------------------


------------------------------------------------------------
*/
static int MAX_Pr; // thread Priority 전역 변수

int main(int argc, char *argv[])
{
	struct sched_param sp;

	//---------------------------------------------
	daemon_mode = 0;
	rcu_cli_mode = 0;
	ver_disp_mode = 0;
	ver_return_mode = 0;

	if(argc == 2)
	{
		     if(strcmp(argv[1], "daemon")==0)	 daemon_mode = 1;
		else if(strcmp(argv[1], "--cli")==0)	 rcu_cli_mode = 1;
		else if(strcmp(argv[1], "--version")==0) ver_disp_mode = 1;
		else if(strcmp(argv[1], "--V")==0)       ver_return_mode = 1;
		else exit(EXIT_SUCCESS);
	}
	else if(argc == 1) 
	{	
		daemon_mode = 1;
	}
	else 
	{
		exit(EXIT_SUCCESS);	
	}
	//---------------------------------------------
	if(daemon_mode || rcu_cli_mode || ver_disp_mode )
	{
		Disp_Version_Console();
	}
	if( ver_return_mode )
	{
		Disp_version_only();
		exit(EXIT_SUCCESS);
		return 0;
	}
	if(daemon_mode)
	{
		printf("\[RCU] daemon mode.\n");
		Chk_Same_Process();
		daemon(0,1);		
		/* 
		daemon(int nochdir, int noclose)
			Fork_init_for_daemon 역할하는 lib-function
			Fork_init_for_daemon();
		*/
	}
	else if(rcu_cli_mode)
	{	
		printf("\[RCU] CLI Mode.\n");
	}
	else if(ver_disp_mode)
	{
		exit(EXIT_SUCCESS);
		return 0;
	}
	//---------------------------------------------

	
	
	openlog ("[RCU_APP]", LOG_CONS|LOG_PERROR, LOG_USER );

#ifdef WATCH_DOG_EN
	if(watchdog_start() < 0)
		syslog(LOG_ERR, "[RCU] !! Watchdog Init Fail");
	else 
		printf("\n[RCU] Watchdog Init, OK.\r\n ");
		
///// YCSO change 190410 watchdog time 10 --> 5 //// 190422 박성진 차장 요청으로 5 -> 10 변경.
#ifdef _WATCHDOG_TEST_TIME_5s_
	watchdog_set_time(5); // Time-Out is 5s
#else
	watchdog_set_time(10); // Time-Out is 10[s]
#endif

	
#endif

#ifdef __MAKER_ANTS__ 	
	Open_SPI_FPGA();
	if(Open_GPIO_Dev() != 0) // if fail
	{
		syslog(LOG_ERR, "[RCU] !! CPU_Dev_Open Fail");
		Close_GPIO_Dev();
		exit(-1) ;
	}
///// YCSO TEST 중....
/*
*	SPI 통신 시 통신이 죽는 이슈가 있어서 OPEN을 전부 하지 않고, 읽을 때마다 오픈하도록 변경.
	//Open_ADC_ic_all(); // 2019.04.15 : ANTS PRU/AAU Only	
*/
	Open_LM75_Temp(); // AAU/PRU of ANTS
#endif
	
	signal(SIGTERM, End_Program);
	signal(SIGINT , End_Program);
	signal(SIGQUIT, End_Program);
	
	Debug_Init();	

	if(rcu_cli_mode)
	{
		MAX_Pr = MAX_THREAD_PRIORITY;

	   	sp.sched_priority = MAX_Pr;

		if(sched_setscheduler(0, THREAD_SCHED_POLICY, &sp) == -1) // current is the 1st priority thread.
		{
			syslog( LOG_ERR, "[RCU-CLI] (sched_setscheduler)");
		}

		MAX_Pr--; // 숫자 높을 수록 높은 순위.
		printf("\n[RCU-CLI] Making Threads...\n");	
		m_Env.task_flag[_TASK_WDT_] = _SET_;		///// YCSO TASK END

		APP_Task_Creat( WDT_Task, 0, NULL, MAX_Pr-- );
		
		#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
		Open_AXI_Reg();
		#endif
		

		/*----------------------------------------------
		3. Infinite Loop
		----------------------------------------------*/		
		CLI_Process();
	}
	else // normal daemon mode
	{
		Init_GPIO_ANTS();	
		StatusSem_Init();

		InitEEP();
		Init_Status_struct();
		Chk_ResetReason();		

		/*----------------------------------------------
			1. Thread 생성 전 초기화	
		----------------------------------------------*/
		
		#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU : TI CPU	
		printf("\n[RCU] Repeater Init. EEP --- %d\n", sizeof(EEPDATASTR));
		#else
		printf("\n[RCU] Repeater Init. EEP --- %ld\n", sizeof(EEPDATASTR));
		#endif	
		
		
		RepeaterInit();		
		Init_UDP_fd();

		/*------------------------------------------
		2020.03.02 : 
			Init/Update LS_Regist_Num  for initial start
			Must Be Called after 'Init_Status_struct' and 'RepeaterInit'
		------------------------------------------*/
		Proc___Inventory_SN(); 
		
		/*----------------------------------------------
			2. Thread 생성
		----------------------------------------------*/	
		MAX_Pr = MAX_THREAD_PRIORITY;
	   	sp.sched_priority = MAX_Pr;

		if(sched_setscheduler(0, THREAD_SCHED_POLICY, &sp) == -1) // current is the 1st priority thread.
		{
			syslog( LOG_ERR, "[RCU] (sched_setscheduler)");
		}		

		MAX_Pr--; // 숫자 높을 수록 높은 순위.
		printf("\n[RCU] Making Threads...\n");			
		//----------------------------------------------
		
//----------------------------------------------
#ifdef __AAU_MU_DUMMY_APP__
//----------------------------------------------
		APP_Task_Creat( WDT_Task,     0, NULL, MAX_Pr--); // Watchdog	
		APP_Task_Creat( UP_Comm_Task, COMM_TERM, NULL, MAX_Pr--); // GUI-Serial		
		APP_Task_Creat( UDP_Sel_Task, 0, NULL, MAX_Pr-- );		
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_NODE, NULL, MAX_Pr--); // GUI-UDP				
		APP_Task_Creat( UDP_DTU_Rx_Task,    0, NULL, MAX_Pr--); // UDP Common
		APP_Task_Creat( system_call_Task, 0, NULL, MAX_Pr-- );
		
		Alarm_Process();
//----------------------------------------------		
#else
//----------------------------------------------
		//============================================================================
		#ifdef __MAKER_ANTS__ 
		//============================================================================
		
		APP_Task_Creat( WDT_Task,     0, NULL, MAX_Pr--); // Watchdog			
		APP_Task_Creat( UP_Comm_Task, COMM_TERM, NULL, MAX_Pr--); // GUI-Serial		
		//-------------------------------------
		// new select method
		APP_Task_Creat( UDP_Sel_Task, 0, NULL, MAX_Pr-- );		
		/*
		2019.08.09
			GUI_UDP와 REMS TX port를 20000을 같이 사용하는 것 때문에,
			APP_Task_Creat() 시, 
			COMM_UDP_NODE 를 COMM_UDP_REMS 보다 먼저 해야 함.
		*/
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_NODE, NULL, MAX_Pr--); // GUI-UDP, 1st
	#ifdef __DEL_UDP_REMS__
	#else	
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_REMS, NULL, MAX_Pr--); // REMS-UDP, 2nd		
	#endif
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_LS  , NULL, MAX_Pr--); // Layer-Splitter

		APP_Task_Creat( gREMS_Comm_Task, 0, NULL, MAX_Pr--); // gREMS				
		APP_Task_Creat( UDP_DTU_Rx_Task,    0, NULL, MAX_Pr--); // UDP Common		
		//-------------------------------------			
		APP_Task_Creat( Auto_Config_Task,   0, NULL, MAX_Pr--);	  // AutoConfiguration
		APP_Task_Creat( FPGA_Register_Task, 0, NULL, MAX_Pr--);   // for DTU Register, ANTS				
		//----------------------------------------------		
	#ifdef __TYPE__PRU__
		APP_Task_Creat( PAU_Comm_Task, AMPU_4_1, NULL, MAX_Pr-- );
		APP_Task_Creat( PAU_Comm_Task, AMPU_4_2, NULL, MAX_Pr-- );
		APP_Task_Creat( PAU_Comm_Task, AMPU_4_3, NULL, MAX_Pr-- );
		APP_Task_Creat( PAU_Comm_Task, AMPU_4_4, NULL, MAX_Pr-- );			
		APP_Task_Creat( Psu_Comm_Task, 0, NULL, MAX_Pr-- );
	#else // AAU		
		APP_Task_Creat( BlueTooth_Comm_Task, COMM_BT, NULL, MAX_Pr-- );
	#endif
		//----------------------------------------------		
		APP_Task_Creat( CPU_Usage_Task, 0, NULL, MAX_Pr-- );
		APP_Task_Creat( system_call_Task, 0, NULL, MAX_Pr-- );
		APP_Task_Creat( Rems_Spectrum_Task,0, NULL, MAX_Pr-- );	///// YCSO added 191212
		//============================================================================
		#endif		
		APP_Task_Creat( SIM_Task ,COMM_SIM_TCP, NULL, MAX_Pr-- );	
		APP_Task_Creat( LOG_Task, 0, NULL, MAX_Pr--);
		APP_Task_Creat( zModem_Task, 0, NULL, MAX_Pr--);
		/*----------------------------------------------
		3. Infinite Loop
		----------------------------------------------*/
		Alarm_Process();
//----------------------------------------------		
#endif
//----------------------------------------------
	}
	exit(EXIT_SUCCESS);
	return 0;
}


//-------------------------------------------------------------------------------------------------------
/* EOF */



