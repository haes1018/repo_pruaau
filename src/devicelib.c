/////////////////////////////
//	DEVICELIB.C
/////////////////////////////
#define DEVICELIB_GLOBALS
#include "includes.h"

#include <errno.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
/////////////////////////////





// fd init value must be (-1)
//
static int fDev_spiADC = -1;
static int fDev_io_gpio = -1;
static int WDT_FD = -1;

//static const char *device = "/dev/spidev32766.0";
static INT8U spi_mode =0 ;
static INT8U spi_bits = 8;
static INT32U spi_speed = 1000000; // 1M
static INT16U spi_delay =0 ;


int watchdog_start( )
{
	WDT_FD = open("/dev/watchdog", O_RDWR|O_NDELAY);

	if( WDT_FD < 0 ) return -1;
	else return 0;
}

void watchdog_end( )
{
	if(WDT_FD < 0) return;
	
	write(WDT_FD, "V", 1);
	close(WDT_FD) ;
}

int watchdog_set_time( int time )
{
	if( WDT_FD < 0 ) return -1;
	ioctl( WDT_FD, WDIOC_SETTIMEOUT, &time );
	return 0;
}

int watchdog_get_time( )
{
	int	time ;
	if( WDT_FD < 0 ) return -1;
	ioctl( WDT_FD, WDIOC_GETTIMEOUT, &time );
	return time ;
}

static OS_TIMER watchdog_time;
static INT32U watchdog_live_time[3]; // [min/max/elapse]

int watchdog_live( void )
{
	static INT8U init = 0;
	INT32U elapse_time;
	
	if( WDT_FD < 0 ) return -1;
	ioctl( WDT_FD, WDIOC_KEEPALIVE, 0 );

	// 2019.09.11	
	//--------------------------------	
	if(init == 0)
	{
		init = 1;
		watchdog_live_time[0] = 0xFFFFFFFF; // min
		watchdog_live_time[1] = 0; // max		
	}
	else
	{
		elapse_time = OSTimerGet_msec(&watchdog_time);
		watchdog_live_time[2] = elapse_time;
		
		if(watchdog_live_time[0] > elapse_time) watchdog_live_time[0] = elapse_time;
		if(watchdog_live_time[1] < elapse_time) watchdog_live_time[1] = elapse_time;	
	}	
	OSTimerReset(&watchdog_time); 
	m_Env.watchdog_cnt++;	
	//--------------------------------
	return 0;
}

INT32U watchdog_remained_ms(void)
{
	INT32U elapse_time;

	elapse_time = OSTimerGet_msec(&watchdog_time);
	if(elapse_time < (10*TICK_CO_SEC))
		return (10*TICK_CO_SEC - elapse_time);
	else
		return 0;		
}

void watchdog_chk_remained(void)
{
	INT32U remained_time;
	
	remained_time = watchdog_remained_ms();
	if(remained_time < (5*TICK_CO_SEC))	
		print_agent(DEBUG_SYSTEM, VT_RED, "watchdog_time %d[ms]", remained_time);
}

INT32U watchdog_elapsed(void)
{	
	return watchdog_live_time[2]; // OSTimerGet_msec(&watchdog_time);
}

INT32U get_watchdog_live_time_min(void)
{
	return watchdog_live_time[0];
}

INT32U get_watchdog_live_time_max(void)
{
	return watchdog_live_time[1];
}






///////////////////////////////////////////////////////////

static int DrvOpen( char* dev_name, int* popen_err_co )
{
	int fd;
	fd = open(dev_name, O_RDWR);
	if( fd < 0)
	{
		syslog( LOG_ERR, "device_open : Could not open file `%s', : %s\n", dev_name, strerror(errno));
		(*popen_err_co)++;
	}
	return fd;
}

int Open_GPIO_Dev(void)
{
	int tot_err = 0; // must init

	fDev_io_gpio = DrvOpen( GPIO_DEVICE, &tot_err ); // 2019.02.20.
	if( fDev_io_gpio < 0 )
	{
		syslog( LOG_ERR, "Dev_io_gpio FAIL !!\n" );
		return -1 ;
	}
	//fDev_spiADC  = DrvOpen( "/dev/spidev32766.0", &tot_err );
	
	return 0;
}


void Close_GPIO_Dev(void)
{
	if( fDev_io_gpio > 0) close(fDev_io_gpio);
	if( fDev_spiADC  > 0) close(fDev_spiADC);

	Close_UART_Comm(COMM_TERM);

#ifdef __TYPE__PRU__
	Close_UART_Comm(COMM_UART_AMP_1);
	Close_UART_Comm(COMM_UART_AMP_1);
	Close_UART_Comm(COMM_UART_AMP_1);
	Close_UART_Comm(COMM_UART_AMP_1);
	Close_UART_Comm(COMM_UART___PSU);
#else // TYPE__AAU
	Close_UART_Comm(COMM_BT);
#endif 
}


////////////////////////////////////////////////////////////////////
// SPI ADC

int ADC_SPI_Init( void )
{
	int ret = 0;

	if( fDev_spiADC<0 ) return -1;


	/*
	 * spi mode
	 */
	ret = ioctl(fDev_spiADC, SPI_IOC_WR_MODE, &spi_mode);
	if (ret == -1)
		syslog( LOG_ERR, "can't set spi mode");

	ret = ioctl(fDev_spiADC, SPI_IOC_RD_MODE, &spi_mode);
	if (ret == -1)
		syslog( LOG_ERR, "can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fDev_spiADC, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);
	if (ret == -1)
		syslog( LOG_ERR, "can't set bits per word");

	ret = ioctl(fDev_spiADC, SPI_IOC_RD_BITS_PER_WORD, &spi_bits);
	if (ret == -1)
		syslog( LOG_ERR, "can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fDev_spiADC, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1)
		syslog( LOG_ERR, "can't set max speed hz");

	ret = ioctl(fDev_spiADC, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1)
		syslog( LOG_ERR, "can't get max speed hz");

	return ret;
}

int ADC_SPI_Comm( INT8U* tx_buf, INT8U* rx_buf )
{
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx_buf,
		.rx_buf = (unsigned long)rx_buf,
		.len = 0,
		.delay_usecs = spi_delay,
		.speed_hz = spi_speed,
		.bits_per_word = spi_bits,
	};

	if( fDev_spiADC<0 ) return -1;

	tr.len = 4;

	ret = ioctl(fDev_spiADC, SPI_IOC_MESSAGE(1), &tr);

	if (ret == 1)
		syslog( LOG_ERR, "can't send spi message");

	return ret;
}

/*
//--------------------------------------------------
// GPIO 5G
//--------------------------------------------------
*/
int _init_gpio(int pin)
{
	int re;
	
	if(fDev_io_gpio < 0) return 0;
	
	re = ioctl(fDev_io_gpio, GPIO_INIT, pin);

	if(re < 0) 
		syslog( LOG_ERR, "_init_gpio : fail , %s\n", strerror(errno));

	return re; 
}

int _set_gpio(int pin, int value)
{
	if(fDev_io_gpio < 0) return 0;
	
	return ioctl(fDev_io_gpio, GPIO_SET, pin | value);
}

int _get_gpio(int pin)
{
	if(fDev_io_gpio < 0) return 0;
	
	return ioctl(fDev_io_gpio, GPIO_GET, pin);
}

int _init_gpio_out(int pin, int initial_value)
{
	if(fDev_io_gpio < 0) return 0;	
	return ioctl(fDev_io_gpio, GPIO_DIR_OUT, pin | initial_value);
}

int _init_gpio_in(int pin)
{
	if(fDev_io_gpio < 0) return 0;
	
	return ioctl(fDev_io_gpio, GPIO_DIR_IN, pin);
}

void _close_gpio(int pin)
{   
	if(fDev_io_gpio < 0) 
	{
		syslog( LOG_ERR, "[RCU] _close_gpio %d fail.\n", pin);
		return;
	}
	
	ioctl(fDev_io_gpio, GPIO_EXIT, pin);
}
/*
//--------------------------------------------------
// GPIO 5G
//--------------------------------------------------
*/


/*		///// YCSO delete 190624
void OSM_On_Ctrl(void)
{
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
	GPIO_DIR_Mode_Out( GPIO_OPT_SW_Status1, GPIO_VALUE_HIGH );
	GPIO_Set( GPIO_OPT_SW_Status1, GPIO_VALUE_HIGH );
#endif
}
*/

//--------------------------------------------------





////////////////////////////////////////////////
/*
void Get_RTC_Time( unsigned char* ST_RTC )
{
	//time_t curr_time = time(NULL) + (9 * 3600);
	time_t curr_time = time(NULL);
	struct tm * tm_ptr = gmtime(&curr_time);

	if( tm_ptr != NULL )
	{
		ST_RTC[0] = (unsigned char)((tm_ptr->tm_year + 1900) % 100);
		ST_RTC[1] = (unsigned char)(tm_ptr->tm_mon + 1);
		ST_RTC[2] = (unsigned char)(tm_ptr->tm_mday);
		ST_RTC[3] = (unsigned char)(tm_ptr->tm_hour);
		ST_RTC[4] = (unsigned char)(tm_ptr->tm_min);
		ST_RTC[5] = (unsigned char)(tm_ptr->tm_sec);
	}





}

void Set_RTC_Time( unsigned char* SET_RTC )
{

	struct tm tm_ptr;
    time_t m_time;

	tm_ptr.tm_year  = SET_RTC[0]+100;
	tm_ptr.tm_mon   = SET_RTC[1]-1;
    tm_ptr.tm_mday  = SET_RTC[2];
    tm_ptr.tm_hour  = SET_RTC[3];
    tm_ptr.tm_min   = SET_RTC[4];
    tm_ptr.tm_sec   = SET_RTC[5];
    tm_ptr.tm_isdst = 0;

    m_time = mktime(&tm_ptr);
    stime(&m_time);

}
*/

/* EOF */


