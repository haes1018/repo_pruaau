
#include "includes.h"

// OS Time Delay Thread 관련 함수들



/*
void OSTimeDly( INT32U dly_1msec )
{
    struct timespec req, rem;

    req.tv_sec = (time_t)0;         //second
    req.tv_nsec = (long)1000000;    //1ms
    nanosleep(&req, &rem);
}
*/

void OSTimerReset( OS_TIMER *timer )		// 타이머 초기화
{
	clock_gettime( CLOCK_MONOTONIC, timer );
}


/*
INT32U OSTimerOverCheck_msec( OS_TIMER *timer, INT32U msec )
{
	OS_TIMER cur_time;
	INT32U d_time, d_time_usec;
	INT32U t_sec, t_msec;

	clock_gettime( CLOCK_MONOTONIC, &cur_time );
//	gettimeofday( &cur_time, NULL );

	if( cur_time.tv_sec < timer->tv_sec )
		return 0;

	d_time = cur_time.tv_sec - timer->tv_sec;
	t_sec = msec/1000;

	if( d_time>t_sec ) return 1;
	else if( d_time<t_sec ) return 0;
	else // d_time == t_sec
	{

		t_msec = msec%1000;
		d_time_usec = cur_time.tv_nsec/1000000;

		if( d_time_usec >= t_msec )
			return 1;
	}

	return 0;
}
*/
INT32U OSTimerOverCheck_msec( OS_TIMER *timer, INT32U msec )
{
	OS_TIMER cur_time;
	INT32U d_sec_time, d_nsec_time;
	INT32U t_sec, t_nsec;

	clock_gettime( CLOCK_MONOTONIC, &cur_time );
	//gettimeofday( &cur_time, NULL );


	if( cur_time.tv_sec < timer->tv_sec )
		return 0;

	d_sec_time = cur_time.tv_sec - timer->tv_sec; // 증가 시간 초
	t_sec = msec/1000;

	if( d_sec_time<t_sec ) return 0; // 시간이 지나지 않았음

	d_sec_time -= t_sec;

	if( d_sec_time>1 ) // 1초 초과 경과
         return 1;

	if( d_sec_time==1 ) // 초단위 1초 경과
	{
		if( cur_time.tv_nsec < timer->tv_nsec ) // 1초 이하
			d_nsec_time = (1000000000 - timer->tv_nsec) + cur_time.tv_nsec;
		else // 1초 이상
			return 1;
	}
	else // 초단위 0초 경과
	{
		if( cur_time.tv_nsec < timer->tv_nsec ) return 0;

		d_nsec_time = cur_time.tv_nsec - timer->tv_nsec;
	}

	t_nsec = msec%1000;
	t_nsec *= 1000000;

	if( d_nsec_time>= t_nsec) return 1;
	else return 0;
}

INT32U OSTimerGet_msec( OS_TIMER *start_time )
{
	OS_TIMER current_time;
	
	clock_gettime( CLOCK_MONOTONIC, &current_time );

	if(current_time.tv_sec < start_time->tv_sec) return 0;
	
	current_time.tv_sec -= start_time->tv_sec;
	
	if( current_time.tv_nsec > start_time->tv_nsec )
	{
		current_time.tv_nsec -= start_time->tv_nsec;
	}
	else
	{		
		if(current_time.tv_sec < 1) return 0;
		/*
		in the case of "tv_sec >= 1"
			tv_nsec 에 1초(1000000000)를 add, 
			instead, tv_sec 에 1초 minus.
		*/
		current_time.tv_sec --;
		current_time.tv_nsec = (1000000000) + current_time.tv_nsec - start_time->tv_nsec;
	}
	
	return ((int)current_time.tv_sec * 1000 + (int)(current_time.tv_nsec/1000000));
}


INT32U OSTimerOverCheck_Sec( OS_TIMER *timer, INT32U sec )
{
	OS_TIMER cur_time;
	INT32U d_time;

	clock_gettime( CLOCK_MONOTONIC, &cur_time );

	if( cur_time.tv_sec < timer->tv_sec )
		return 0;

	d_time = cur_time.tv_sec - timer->tv_sec;

	if( d_time>sec ) return 1;
	else if( d_time<sec ) return 0;
	else // d_time == t_sec
	{
		if( cur_time.tv_nsec >= timer->tv_nsec )
			return 1;
	}

	return 0;
}

void Thread_Priority_Init( Thread_ARG_Var* p_th_arg, int os_start_delay_en )
{
	struct sched_param sp;

	sp.sched_priority = p_th_arg->priority;

	if(sched_setscheduler (0, THREAD_SCHED_POLICY, &sp) == -1)
		syslog( LOG_ERR, "sched_setscheduler");

	if( os_start_delay_en ) OSTimeDly( (100-p_th_arg->priority)*OS_DELAY_1msec );

}


/*
void OSTimerReset( OS_TIMER *timer )		// 타이머 초기화
{
	gettimeofday( timer, NULL );
}


INT32U OSTimerOverCheck_msec( OS_TIMER *timer, INT32U msec )
{
	OS_TIMER cur_time;
	INT32U d_time, d_time_usec;
	INT32U t_sec, t_msec;

	gettimeofday( &cur_time, NULL );


	if( cur_time.tv_sec < timer->tv_sec )
		return 0;

	d_time = cur_time.tv_sec - timer->tv_sec;
	t_sec = msec/1000;

	if( d_time>t_sec ) return 1;
	else if( d_time<t_sec ) return 0;
	else // d_time == t_sec
	{
		t_msec = msec%1000;
		d_time_usec = cur_time.tv_usec/1000;

		if( d_time_usec >= t_msec )
			return 1;
	}

	return 0;
}

INT32U OSTimerOverCheck_Sec( OS_TIMER *timer, INT32U sec )
{
	OS_TIMER cur_time;
	INT32U d_time;

	gettimeofday( &cur_time, NULL );


	if( cur_time.tv_sec < timer->tv_sec )
		return 0;

	d_time = cur_time.tv_sec - timer->tv_sec;

	if( d_time>sec ) return 1;
	else if( d_time<sec ) return 0;
	else // d_time == t_sec
	{
		if( cur_time.tv_usec >= timer->tv_usec )
			return 1;
	}

	return 0;
}

*/



void InitTimeCheck( OS_TIMER* start_time )
{
	clock_gettime( CLOCK_MONOTONIC, start_time );
}

void PrintTimeCheck( char* msg, OS_TIMER* start_time )
{
	OS_TIMER current_time;
	clock_gettime( CLOCK_MONOTONIC, &current_time );

	current_time.tv_sec -= start_time->tv_sec;
	if( current_time.tv_nsec>start_time->tv_nsec )
	{
		current_time.tv_nsec-=start_time->tv_nsec;
	}
	else
	{
		current_time.tv_nsec = (1000000000) + current_time.tv_nsec - start_time->tv_nsec;
	}
	printf("%s : %d sec %d.%06d msec\n", msg, (int)current_time.tv_sec, (int)(current_time.tv_nsec/1000000), (int)(current_time.tv_nsec%1000000) );
}


/* EOF */


