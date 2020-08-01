/////////////////////////////
//	alarm_iro.c
/////////////////////////////

#define ALARM_GLOBALS
#include "includes.h"
/////////////////////////////

#include <math.h>

#define ALM_LOOP_DELAY	OS_DELAY_10msec
extern int daemon_mode;

/////////////////////////////
// Local Variables
/////////////////////////////





/////////////////////////////
// Local Variables
/////////////////////////////



void AMP_Not_Use_Pwr_Chk(void);


//----------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------
INT8U AlarmCnt_Ref_Time_Chk_En = 0;
//----------------------------------------------------------------------------------------
#define AMP_OFF_TIME_30_Min	1800000 // 30분 = 30 * 60 * TICK_CO_SEC
//----------------------------------------------------------------------------------------

INT8U IS___AmpStd_UnChk_AMP___Alarm(void);
INT8U IS__AMP_Std_Exit_Possible(void);


/////////////////////////////
// Local Functions
/////////////////////////////


void init_con_iro(void);
void CheckDI( void );

void AMP_All_Off( INT8U sd_rsn );
void FWD_AMP_OFF_Reason_Proc( void );
void Debug_BTS_Use(void);
//----------------------------------------------


int Read_RootFS_Version( char* Ver_RootFS );

static INT8U prev_amp_use[AMPU_4_MAX];


OS_TIMER ArmChkTimer;
static OS_TIMER Timer_dbg; 
static OS_TIMER Timer_Broadcast; // every 3[s]
static OS_TIMER _2sec_Timer;




void Alarm_Process_IRO( void )
{
	INT8U co = 50;
	INT8U path4;

	Init_Ver_Info();
	Set_Ver_Info_Rcu( 0, m_St.RCU_Ver );
	

	OSTimerReset( &ArmChkTimer );
	OSTimerReset( &_2sec_Timer );
	OSTimerReset( &Cw_AutoChk_Timer );
	Init_NMS_Alm_Proc();	
	AC_Fail_St = 0;	
	
	memset( prev_amp_use, 0, AMPU_4_MAX);
		
	Start_Alm_Rpt_Pause_Timer();	
	
	m_St.Sum_AMP_Stdby_St = OFF;
	

    while (1)
	{
		OSTimeDly( OS_DELAY_10msec);
		
		if( ++co>=100 )
		{
			co = 0;						
			Proc_G_Timer();
			Proc_UDA_Suppress_Alm();
		}		
		AC_Normal_Std_Mode_Restart_Chk();
	}
}

static OS_TIMER Ro_sys_Timer;






//----------------------------------------------------------------------------------------------
// 사용, 미사용에 따른 알람 마스크
// 모듈 사용 여부 등에 따라 알람 마스크한다.


//=================================================================================================================================

#define RootFS_File_Path "/Version"
int Read_RootFS_Version( char* Ver_RootFS )
{
	int fd;
	struct stat st;
	int fileSize = -1;
	int re = -1;
	char ver_str[10];
	int i;
	
		
	fd = open( RootFS_File_Path, O_RDONLY );
	
	if( fd < 0 )
	{
		syslog( LOG_WARNING, "[iRO] no RootFS Ver.\n");
		return re;
	}
	
	if(stat( RootFS_File_Path, &st) == 0)
	{
		fileSize = st.st_size;		
	}
	
	if(fileSize <= 0)
	{
		syslog( LOG_WARNING, "[iRO] RootFS_Ver 0.\n");
		close(fd);
		return re;
	}
	else if(fileSize < 10)
	{
		memset( ver_str, 0, 10);
		read( fd, ver_str, fileSize );

		for(i = 0; i < fileSize; i++ )
		{
			if((ver_str[i] == 0x0d) || (ver_str[i] == 0x0a)) 
			{
				ver_str[i] = 0;
			}
		}
		memcpy( Ver_RootFS, ver_str, fileSize );
		re = 1;
	}	
	else
	{	
		syslog( LOG_WARNING, "[iRO] rd fail(1).\n");
		re = -1;
	}
	
	close(fd);
	return re;
}

static INT8U  SKIP_REMS_ALM_Mask[RU_ALARM_CNT];
#define AMP_RST_SKIP_ALM_NUM (2)


/*
--------------------------------------------------------------------------------------
* alm 이 속한 path가 amp/mimo stdby 상태이면, return 1.
* alm mask 용.																					   
--------------------------------------------------------------------------------------
*/
/*
INT8U Is_Standby_State(INT8U path4)
{
	if( path4 >= AMPU_4_MAX ) return 0;

	if( m_St.MIMO_Stdby_St[path4] ) return 1;
	if( m_St.Mimo_Stdby_St_Dly_REQ[path4] ) return 1;
	
	if( m_St.AMP_Stdby_St[sm][path4] ) return 1;
	if( m_St.AMP_Stdby_St_Dly_REQ[sm][path4] ) return 1;
	
	return 0;
}
*/

INT8U Pause_Alm_Report( INT8U byte, INT8U bit )
{
	return 0;
	
#if 0 // later used when 5G-AAU/PRU
    INT8U path4;
	INT8U* pAlm;

	if(byte >= RU_ALARM_CNT) return 0;
	if(bit  >= 8) return 0;	
			
	pAlm = alm_____DPD[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));
			
	pAlm = alm_OverPwr[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm_Hi_Temp[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm____VSWR[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm_AMP_OFF[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm_AmpLink[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm_DC_Fail[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm_DevFail[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));
			
	pAlm = alm_Hi__Alm[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));

	pAlm = alm_Low_Alm[path4];
	if((pAlm[0] == byte) && (pAlm[1] == bit)) return (Is_Standby_State(path4));
	
	return 0;
#endif
}



NMS_ALARM_EXT INT8U Is_Low_Alm_AID( INT8U byte_p, INT8U bit_p );

//---------------------------------------------------------------------
// check if fwd-low-limit-alarm AID 
//
INT8U Is_Low_Alm_AID( INT8U byte_p, INT8U bit_p )
{
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if((alm_Low_Alm[path4][0] == byte_p) && (alm_Low_Alm[path4][1] == bit_p))
		{
			return 1;
		}
	}			
	return 0;
}




//----------------------------------------------------------------------------------------------





/*
------------------------------
DL1 DL2 DL3 DL4
------------------------------
 5, 10, 15, 20
 6, 11, 16, 21
 7, 12, 17, 22
 8, 13, 18, 23
 9  14, 19, 24
 ------------------------------
*/

/*
#ifdef  NMS_ALARM_GLOBALS
NMS_ALARM_EXT INT8U alm_____DPD[AMPU_4_MAX][2] = {{ 7, 7},{12, 7},{17, 7},{22, 7}}; // PRU, DPD
//---------------------
NMS_ALARM_EXT INT8U alm_OverPwr[AMPU_4_MAX][2] = {{ 6, 7},{11, 7},{16, 7},{21, 7}}; // PRU, Over-Power
NMS_ALARM_EXT INT8U alm_Hi_Temp[AMPU_4_MAX][2] = {{ 6, 6},{11, 6},{16, 6},{21, 6}}; // PRU, high-Temp 
NMS_ALARM_EXT INT8U alm____VSWR[AMPU_4_MAX][2] = {{ 6, 5},{11, 5},{16, 5},{21, 5}}; // PRU, VSWR
NMS_ALARM_EXT INT8U alm_AMP_OFF[AMPU_4_MAX][2] = {{ 6, 4},{11, 4},{16, 4},{21, 4}}; // PRU/AAU, AMP Off
NMS_ALARM_EXT INT8U alm_AmpLink[AMPU_4_MAX][2] = {{ 6, 3},{11, 3},{16, 3},{21, 3}}; // PRU, AMP Link fail
NMS_ALARM_EXT INT8U alm_DC_Fail[AMPU_4_MAX][2] = {{ 6, 2},{11, 2},{16, 2},{21, 2}}; // PRU, DC Fail
NMS_ALARM_EXT INT8U alm_DevFail[AMPU_4_MAX][2] = {{ 6, 1},{11, 1},{16, 1},{21, 1}}; // PRU/AAU, Device Fail
NMS_ALARM_EXT INT8U alm_Shut_Dn[AMPU_4_MAX][2] = {{ 6, 0},{11, 0},{16, 0},{21, 0}}; // PRU/AAU, Shutdown
//---------------------
NMS_ALARM_EXT INT8U alm_Hi__Alm[AMPU_4_MAX][2] = {{ 5, 5},{10, 5},{15, 5},{20, 5}}; // PRU/AAU, DL 출력 상한
NMS_ALARM_EXT INT8U alm_Low_Alm[AMPU_4_MAX][2] = {{ 5, 4},{10, 4},{15, 4},{20, 4}}; // PRU/AAU, DL 출력 하한.
//---------------------
NMS_ALARM_EXT INT8U alm_Main__DL_TSSI_Hi[AMPU_4_MAX][2] = {{ 5, 7},{10, 7},{15, 7},{20, 7}}; // PRU/AAU, DL TSSI 상한, Main
NMS_ALARM_EXT INT8U alm_Alpha_DL_TSSI_Hi[AMPU_4_MAX][2] = {{ 5, 6},{10, 6},{15, 6},{20, 6}}; // PRU/AAU, DL TSSI 상한, Alpha
NMS_ALARM_EXT INT8U alm_Main__DL_TSSI_Lo[AMPU_4_MAX][2] = {{ 5, 3},{10, 3},{15, 3},{20, 3}}; // PRU/AAU, DL TSSI 하한, Main
NMS_ALARM_EXT INT8U alm_Alpha_DL_TSSI_Lo[AMPU_4_MAX][2] = {{ 5, 2},{10, 2},{15, 2},{20, 2}}; // PRU/AAU, DL TSSI 하한, Alpha
//---------------------
NMS_ALARM_EXT INT8U alm_Main__UL_TSSI_Hi[AMPU_4_MAX][2] = {{ 8, 7},{13, 7},{18, 7},{23, 7}}; // PRU/AAU, UL TSSI 상한, Main
NMS_ALARM_EXT INT8U alm_Alpha_UL_TSSI_Hi[AMPU_4_MAX][2] = {{ 8, 6},{13, 6},{18, 6},{23, 6}}; // PRU/AAU, UL TSSI 상한, Alpha
NMS_ALARM_EXT INT8U alm_Main__UL_TSSI_Lo[AMPU_4_MAX][2] = {{ 8, 3},{13, 3},{18, 3},{23, 3}}; // PRU/AAU, UL TSSI 하한, Main
NMS_ALARM_EXT INT8U alm_Alpha_UL_TSSI_Lo[AMPU_4_MAX][2] = {{ 8, 2},{13, 2},{18, 2},{23, 2}}; // PRU/AAU, UL TSSI 하한, Alpha
//---------------------
NMS_ALARM_EXT INT8U alm__R_Over[AMPU_4_MAX][2]  = {{ 8, 5},{13, 5},{18, 5},{23, 5}}; // PRU/AAU, RVS IN Over
NMS_ALARM_EXT INT8U alm_LNA_Fail[AMPU_4_MAX][2] = {{ 8, 4},{13, 4},{18, 4},{23, 4}}; // PRU, LNA Fail
NMS_ALARM_EXT INT8U RVS_AMP_OFF[AMPU_4_MAX][2]  = {{ 8, 4},{13, 4},{18, 4},{23, 4}}; // PRU, RVS AMP OFF, same with LNA-Fail, maybe
#else
NMS_ALARM_EXT INT8U alm_____DPD[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_OverPwr[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Hi_Temp[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm____VSWR[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_AMP_OFF[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_AmpLink[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_DC_Fail[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_DevFail[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Shut_Dn[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Hi__Alm[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Low_Alm[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Main__DL_TSSI_Hi[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Alpha_DL_TSSI_Hi[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Main__DL_TSSI_Lo[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Alpha_DL_TSSI_Lo[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Main__UL_TSSI_Hi[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Alpha_UL_TSSI_Hi[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Main__UL_TSSI_Lo[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_Alpha_UL_TSSI_Lo[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm__R_Over[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U alm_LNA_Fail[AMPU_4_MAX][2];
NMS_ALARM_EXT INT8U RVS_AMP_OFF[AMPU_4_MAX][2];
#endif
*/





/* EOF */

