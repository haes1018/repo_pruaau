/////////////////////////////
// CRC.C
/////////////////////////////
#define ADC_IC_GLOBALS
#include "includes.h"
/////////////////////////////


/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef __MAKER_ANTS__

/*
----------------------------------------------------------------------
	ANTS-PRU
	ADC-IC is "adc128s052"
	8 channel
	read adc raw from /sys/class/spi_master/spi0/spi0.2/iio:device0/in_voltage[X]_raw 
	
	ANTS-AAU
	ADC-IC is "max11645"
	2 channel
	
	read adc raw from /dev/adc[x]_raw
	adc enable is /dev/adc[x]_en
----------------------------------------------------------------------
*/
#ifdef __TYPE__PRU__
#define ADC_PATH_1		"/sys/class/spi_master/spi0/spi0.2/iio:device0/in_voltage"
#define ADC_PATH_2		"_raw"
#define MAX_ADC_NUM		8
#else // __AAU__
#define ADC_PATH_1		"/dev/adc"
#define ADC_PATH_2		"_raw"
#define MAX_ADC_NUM		2
#endif

static int fd_ADC_ic_file[MAX_ADC_NUM] = {0};

//---------------------------------------------

int Open_ADC_ic_file( INT8U adc_num )
{
	char adc_path[128];
	
	if( adc_num >= MAX_ADC_NUM )
	{
		syslog( LOG_WARNING, "[RCU_ADC] No match number ADC %d\n",adc_num);
		return -1;
	}

	sprintf(adc_path, "%s%d%s",ADC_PATH_1,adc_num,ADC_PATH_2);

#ifdef __TYPE__AAU__

	system( "echo 1 > /dev/adc0_en" );
	system( "echo 1 > /dev/adc1_en" );
	
#endif

//	printf( "[RCU_ADC] %s success \n", adc_path);

	if( fd_ADC_ic_file[adc_num] <= 0 )
	{
		fd_ADC_ic_file[adc_num] = open( adc_path, O_RDONLY);

		if( fd_ADC_ic_file[adc_num] < 0 )
		{
//			D_Printf( DEBUG_SYSTEM, "[RCU_ADC] !! No ADC_PATH %d < 0 ", adc_num);
			syslog( LOG_WARNING, "[RCU_ADC] No ADC_PATH.\n");
			return -1;
		}
	}

//	D_Printf( DEBUG_SYSTEM, "[RCU_ADC] %s success ", adc_path);
	return fd_ADC_ic_file[adc_num];
	
}

void Close_ADC_ic_file( INT8U adc_num )
{
	if( fd_ADC_ic_file[adc_num] > 0)
	{
		close(fd_ADC_ic_file[adc_num]);
	}
}

void Close_ADC_ic_file_all( void )
{
	int x = 0;

	for( x = 0; x < MAX_ADC_NUM; x++ )
	{
		if( fd_ADC_ic_file[x] > 0)
		{
			close(fd_ADC_ic_file[x]);
		}
	}
}

INT16U Get_ADC_voltage( INT8U adc_num )
{	
	int bit_value = 0;	
	INT16U volt_mV;
	static INT16U volt_mV_prev[MAX_ADC_NUM];
	static INT8U init = 0;
	INT8U i;

	if(init==0)
	{
		init = 1;
		for(i = 0; i < MAX_ADC_NUM; i++)
			volt_mV_prev[i] = 0;
	}

	char temp_str[10];       // 2019.05.27 : 4 -> 10
	memset(temp_str, 0, 10); // 2019.05.27 : 4 -> 10
	
	Open_ADC_ic_file(adc_num); // 2019.04.24.By YCSo
	
	if(fd_ADC_ic_file[adc_num]  < 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Get_ADC_voltage] fd < 0 !!, Use previous %d[mV]", 
					adc_num, volt_mV_prev[adc_num]);
		
		return volt_mV_prev[adc_num]; // "return 0" 하지 않도록 수정.
	}
	
	lseek(fd_ADC_ic_file[adc_num], 0, SEEK_SET);
	read(fd_ADC_ic_file[adc_num], temp_str, 4);

	Close_ADC_ic_file(adc_num); // 2019.04.24.By YCSo
	fd_ADC_ic_file[adc_num] = 0; // 2019.04.24.By YCSo
	bit_value = atoi(temp_str);
	
	volt_mV = (bit_value)*3300/4096; // ((bit*3.3)/4096)*1000 = bit*3300/4096
	//D_Printf( DEBUG_SYSTEM, "[RCU_ADC] !! %s bit_value = %d ", temp_str, bit_value);	
	//D_Printf( DEBUG_SYSTEM, "[RCU_ADC] !! %s bit_value = %d && adc[mV] = %ld ", temp_str, bit_value, adc_value);	
	
	volt_mV_prev[adc_num] = volt_mV;
		
	return volt_mV;
}
//-------------------------------------------------------


//-------------------------------------------------------
void Open_ADC_ic_all( void )
{
	int x = 0;

	for( x = 0; x < MAX_ADC_NUM; x++)
		Open_ADC_ic_file(x);
	
}
//-------------------------------------------------------

#endif // End of ANTS-PRU/AAU



/* EOF */


