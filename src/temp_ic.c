/////////////////////////////
// CRC.C
/////////////////////////////
#define TEMP_IC_GLOBALS
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
	ANTS-PRU, ANTS-AAU
	TEMP-IC is "LM75"
	read temperature from "/sys/class/hwmon/hwmon0/temp1_input"
----------------------------------------------------------------------
*/

static int fd_LM75_Temp = (-1);

#define LM75_TEMP_Path "/sys/class/hwmon/hwmon0/temp1_input"

//---------------------------------------------
int Open_LM75_Temp(void)
{
	if(fd_LM75_Temp < 0) // if not opened
	{			
		fd_LM75_Temp = open( LM75_TEMP_Path, O_RDONLY );	
		
		if(fd_LM75_Temp < 0)
		{
			syslog( LOG_WARNING, "[RCU] No LM_75_TEMP_IC.\n");
			return -1;
		}
	}
	return fd_LM75_Temp;
}
//---------------------------------------------
void Close_LM75_Temp(void)
{
	if(fd_LM75_Temp > 0)
	{
		close(fd_LM75_Temp);
	}
}
//---------------------------------------------
INT16S Get_LM75_Temp(void)
{
	char temp_str[10];
	int i;
	int temp_x10;
	static int temp_x10_prev = 0;
	
	if(fd_LM75_Temp < 0)
	{
		D_Printf( DEBUG_SYSTEM, "[TEMP_LM75] !! fd_LM75_Temp < 0 \n");
		return temp_x10_prev;
	}
	
	memset(temp_str, 0, 10);
	lseek(fd_LM75_Temp, 0, SEEK_SET);
	read(fd_LM75_Temp, temp_str, 10);

	for(i = 0; i < 10; i++ )
	{
		if((temp_str[i] == 0x0d) || (temp_str[i] == 0x0a))
		{
			temp_str[i] = 0;
			break;
		}
	}	
	temp_x10 = atoi(temp_str)/100;
	
	//D_Printf( DEBUG_SYSTEM, "[TEMP_LM75] %s, %d \n", temp_str, temp_x10);

	temp_x10_prev = temp_x10;
	return temp_x10;
}
#endif







/* EOF */

