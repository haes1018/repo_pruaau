// OS Time Delay Thread 관련 함수들

#ifndef  _OS_UTIL_H_
#define  _OS_UTIL_H_

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity */
typedef signed   short INT16S;                   /* Signed   16 bit quantity */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity */

typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

//
typedef unsigned long long   INT64U_T;
typedef signed long long  INT64S_T;


/*
----------------------------------------------------------------------------
@ zynq
----------------------------------------------------------------------------
*/
typedef unsigned long   INT64U;                   /* Signed   64 bit quantity*/
typedef signed   long   INT64S;                   /* UnSigned  64 bit quantity*/
/*
----------------------------------------------------------------------------
@ zynq
----------------------------------------------------------------------------
*/


typedef float          FP32;                     /* Single precision floating point*/
typedef double         FP64;                     /* Double precision floating point*/


/*
----------------------------------------------------------------------------
@ 5G AAU/PRU
----------------------------------------------------------------------------
*/

typedef union{
	INT8U uD8;
	INT8S sD8;
} type8;

typedef union{
	INT8U uD8[2];
	INT8S sD8[2];

	INT16U uD16;
	INT16S sD16;
} type16;

typedef union{
	INT8U uD8[4];
	INT8U sD8[4];
	
	INT16U uD16[2];
	INT16S sD16[2];

	INT32U uD32;
	INT32S sD32;
} type32;


//TI
typedef union{
	INT8U uD8[8];
	INT8U sD8[8];
	
	INT16U uD16[4];
	INT16S sD16[4];

	INT32U uD32[2];
	INT32S sD32[2];

	INT64U_T uD64;
	INT64S_T sD64;

} type64_T;

//zynq
typedef union{
	INT8U uD8[8];
	INT8U sD8[8];
	
	INT16U uD16[4];
	INT16S sD16[4];

	INT32U uD32[2];
	INT32S sD32[2];

	INT64U uD64;
	INT64S sD64;

} type64_Q;


////////////////////////////////////////////////////////////////////////////////
// Common utility macros
//

#define	BitSet(a,b)				a = a|(1<<b)
#define	BitRst(a,b)				a = a&(~(1<<b))
#define	BitRead(a,b)			((a >> b) & 1)

#define hiword(x)		((unsigned short int)((x) >> 16))
#define	loword(x)		((unsigned short int)(x))
#define	hibyte(x)		(((x) >> 8) & 0xff)
#define	lobyte(x)		((x) & 0xff)

/*
----------------------------------------------------------------------------
@ 5G AAU/PRU
----------------------------------------------------------------------------
*/



#define MAX_THREAD_PRIORITY	90
#define MAX_THREAD_NUM	80 // MAX_THREAD_PRIORITY 보다 적어야..
#define THREAD_SCHED_POLICY	SCHED_FIFO
//#define THREAD_SCHED_POLICY	SCHED_RR

typedef struct
{
	INT32U priority;
	INT32U Arg_v;
	void* pArg;
} Thread_ARG_Var;



#define OS_DELAY_1msec			1000
#define OS_DELAY_10msec			10000
#define OS_DELAY_100msec		100000
#define OS_DELAY_500msec		500000
#define OS_DELAY_1sec			1000000
#define OS_DELAY_10sec			10000000

//extern void OSTimeDly( INT32U dly_1msec ); // UCSOS에서 처럼 쓸려고 만든 Delay 함수
#define OSTimeDly usleep
// 128 Hz 가 기준 7.8125
////////////////////////////////////////////
// Timer
//typedef struct timeval OS_TIMER;                     /* Double precision floating point                    */
typedef struct timespec OS_TIMER;                     /* Double precision floating point                    */

extern void OSTimerReset( OS_TIMER *timer );		// 타이머 초기화
extern INT32U OSTimerOverCheck_Sec( OS_TIMER *timer, INT32U sec );		// 타이머 함수 1초단위 - 1msec 단위에 비해 빠름
extern INT32U OSTimerOverCheck_msec( OS_TIMER *timer, INT32U msec );	// 타이머 함수 1msec단위
extern void Thread_Priority_Init( Thread_ARG_Var* p_th_arg, int os_start_delay_en );

extern INT32U OSTimerGet_msec( OS_TIMER *timer );

extern void InitTimeCheck( OS_TIMER* start_time );
extern void PrintTimeCheck( char* msg, OS_TIMER* start_time );

#endif
