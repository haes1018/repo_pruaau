/////////////////////////////
//	ul_proc.c
/////////////////////////////
#define UL_PROC_GLOBALS
#include "includes.h"
/////////////////////////////



#define DIG_Link_Fail_Max_CNT (5)
#define DIG_LINK_FAIL_CO	  (10)

/////////////////////////////
// Local Variables
/////////////////////////////



//*************************************************************************************
//
//*************************************************************************************


//--------------------------------------------

// RCS Reset 후, 90[s] 동안 low alarm mask

static INT8U Low_Alm_Mask_at_PD_Ref10M = 1; // default 1. At Reset, mask is default.
static OS_TIMER Low_Alm_Mask_Timer_ms; 

INT8U Is_Low_Alm_Mask(void)
{
	return Low_Alm_Mask_at_PD_Ref10M;
}
void Mask_Low_Alm(void)
{
	Low_Alm_Mask_at_PD_Ref10M = 1;
}
void Unmask_Low_Alm(void)
{
	Low_Alm_Mask_at_PD_Ref10M = 0;
}
//--------------------------------------------




#define LOW_ALM_Timeout_ms (90*1000) // 90[s]


static INT8U Global_Alm_Mask = 0; // default 0




void Global_Alm_Mask_Set(void)
{
	Global_Alm_Mask = 1;
}
void Global_Alm_Mask_Clr(void)
{
	Global_Alm_Mask = 0;
}

// QRO(CS,GSI)의 경우, RCU Reset 후, 90[s] 동안 all alarm mask.
INT8U Is_Global_Alm_Mask(void)
{
	return Global_Alm_Mask;
}

//----------------------------------------------



enum
{
	Lo_Alm_Mask_Start = 0,
	Lo_Alm_Mask_Now, // now mask status.
	Lo_Alm_After_Timeout, // timeout 후, unmask 여부를 DeciSion
	Lo_Alm_Mask_Desi, // lo alarm mask 할지 판단. 일반적인 상태. mask 여부 DeciSion.
	Lo_Alm_Mask_End
};

static INT8U Lo_Alm_Mask_St = Lo_Alm_Mask_Start; // when reset, start with low alamr mask. 

void Low_Alm_Mask_Start(void)
{
	Lo_Alm_Mask_St = Lo_Alm_Mask_Start;
}

void Proc_Lo_Alm_Mask(void) 
{
#if 0 // later @ 5G
	INT8U dbg_req = 0;
	INT32U time_ms;
	static INT8U cnt = 0;
		
	//---------------------------------------------------
	//
	// RCU Reset 후 90초 동안 low-out alarm mask 하기 위한 것.
	//
	//---------------------------------------------------
	switch(Lo_Alm_Mask_St)
	{
		// when RCU reset, 
		// start with low alamr mask.
		case Lo_Alm_Mask_Start: 			
			Mask_Low_Alm();
			OSTimerReset( &Low_Alm_Mask_Timer_ms );			
			Global_Alm_Mask_Set();
			Lo_Alm_Mask_St = Lo_Alm_Mask_Now;
			break;
			
		case Lo_Alm_Mask_Now:
			time_ms = OSTimerGet_msec(&Low_Alm_Mask_Timer_ms);
			if( time_ms >= LOW_ALM_Timeout_ms )
			{
				Lo_Alm_Mask_St = Lo_Alm_Mask_End;				
				Unmask_Low_Alm();
				Global_Alm_Mask_Clr();
				D_Printf( DEBUG_SYSTEM, "[Low_Alm] mask clr\r\n");
			}
			else
			{				
				if(dbg_req && (cnt == 0))
				{
					D_Printf( DEBUG_SYSTEM, "[Low_Alm] %d ms\r\n", time_ms );
				}
				cnt++;
				if(cnt >= 10) cnt = 0;
			}
			break;
			
		default : 
		case Lo_Alm_Mask_End:
			Lo_Alm_Mask_St = Lo_Alm_Mask_End;
			break;
	}
#endif	
}
//*************************************************************************************
//
//*************************************************************************************

/* EOF */



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

