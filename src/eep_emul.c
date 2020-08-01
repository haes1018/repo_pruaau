/////////////////////////////
//	eep_emul.C
/////////////////////////////
#include "includes.h"
/////////////////////////////


#define EEP_FILE_X_A     "/usr/app/sys/conf/app_data%02X_A.dat"
#define EEP_FILE_X_B     "/usr/app/sys/conf/app_data%02X_B.dat"

#define EEP_FILE_BACKUP	"/tmp/app/sys/conf/app_data*"
#define EEP_FILE_DIR	"/usr/app/sys/conf/"

#define EEP_TABLE_FILE_A	"/usr/app/sys/conf/table%02X_A.dat"
#define EEP_TABLE_FILE_B	"/usr/app/sys/conf/table%02X_B.dat"

#define EEP_TABLE_BACKUP	"/tmp/app/sys/conf/table*"

#define RST_INFO_FILE_A  "/usr/app/sys/conf/rst_info_A.dat"
#define RST_INFO_FILE_B  "/usr/app/sys/conf/rst_info_B.dat"

//#define DATA_FLASH_SEC_CO	1		// (2)4K -> 6(12K)
#ifdef __TYPE__AAU__		///// YCSO 
#define DATA_FLASH_SEC_CO	2		// (2)4K -> 6(12K)
#else
#define DATA_FLASH_SEC_CO	1		// (2)4K -> 6(12K)
#endif

#define FLASH_PAGE_SIZE		(0x4000) // 0x4000 16k 0x2000 : 8kB
#define DATA_FLASH_SIZE		(FLASH_PAGE_SIZE-2)	// CRC 2BYTE

#define RST_INFO_PAGE_SIZE		(64)		//64
#define RST_INFO_FLASH_SIZE		(RST_INFO_PAGE_SIZE-2)	// CRC 2BYTE

static INT8U	V_EEP_DATA[DATA_FLASH_SEC_CO][FLASH_PAGE_SIZE]; // sec : 0 ==> m_Eep, 1 ==> table

static INT8U	V_RST_INFO_DATA[RST_INFO_PAGE_SIZE]; // sec : 0 ==> m_Eep, 1 ==> table

static pthread_mutex_t	_mutex_Eep;



int CheckVFile( char* fname, INT8U* eep_data, int size )
{
	int fd, ret;
	INT16U crc = 0;

	fd = open( fname, O_RDONLY );

	if( fd<0 )
		return -1;	

	if( read( fd, eep_data, size )==size )
	{
		size -= 2; // CRC 위치
		crc = crcNByteCalc( 0, eep_data, size );
//		if( crc == *((INT16U*)&eep_data[size]) )
		if( crc == BYTE_to_INT16L(&eep_data[size])) 
			ret = 0;
		else 
			ret = -1;
	}
	else
		ret = -1;

	close( fd );
	
	printf("[RCU] %s, crc:%X/%X, ret %d\n", 
		fname, crc, BYTE_to_INT16L(&eep_data[size]), ret);
	
	return ret;
}

int SaveVFile( char* fname, INT8U* eep_data, int size )
{
	int fd, ret;

	fd = open( fname, O_CREAT|O_WRONLY|O_SYNC, 0644 );

	if( fd<0 )
	{
		return -1;
	}

	if( write( fd, eep_data, size )==size )
	{
		ret = 0;
	}
	else
	{
		ret = -1;
		D_Printf( DEBUG_SYSTEM, " SaveVFile Err %s.\n", strerror(errno) );
	}
	sync();
	close( fd );
	return ret;
}

INT8U ResetInfo_Write( INT8U* data, INT16U size )
{
//	INT16U* pcrc;

	if( size>=RST_INFO_FLASH_SIZE ) return 0;
	
	pthread_mutex_lock( &_mutex_Eep );
	
	if( memcmp( V_RST_INFO_DATA, data, size ) )
	{
		memcpy( V_RST_INFO_DATA, data, size );
		
		INT16L_to_BYTE( &V_RST_INFO_DATA[RST_INFO_FLASH_SIZE], crcNByteCalc( 0, V_RST_INFO_DATA, RST_INFO_FLASH_SIZE ) );
		
		if( SaveVFile( RST_INFO_FILE_A, V_RST_INFO_DATA, RST_INFO_PAGE_SIZE )<0 )
		{
			printf("eep_emul:file write fail rst info A");
			D_Printf( DEBUG_SYSTEM, " eep_emul:file write fail rst info A\n");
		}

		if( SaveVFile( RST_INFO_FILE_B, V_RST_INFO_DATA, RST_INFO_PAGE_SIZE )<0 )
		{
			printf("eep_emul:file write fail rst info B");
			D_Printf( DEBUG_SYSTEM, " eep_emul:file write fail rst info B\n");
		}
	}


	
	pthread_mutex_unlock( &_mutex_Eep );
	
	return 1;
}
INT8U ResetInfo_Read( INT8U* data, INT16U size )
{
	if( size>=RST_INFO_FLASH_SIZE ) return 0;
	
	pthread_mutex_lock( &_mutex_Eep );
	
	memcpy( data, V_RST_INFO_DATA, size );
	
	pthread_mutex_unlock( &_mutex_Eep );
	
	return 1;
}

void Refresh_V_EEP( INT8U sec )
{
//	INT16U* pcrc;
	char f_path[64];

//	pcrc = (INT16U*)&V_EEP_DATA[sec][DATA_FLASH_SIZE];
//	(*pcrc) = crcNByteCalc( 0, (INT8U*)V_EEP_DATA[sec], DATA_FLASH_SIZE );

	INT16L_to_BYTE( &V_EEP_DATA[sec][DATA_FLASH_SIZE], crcNByteCalc( 0, (INT8U*)V_EEP_DATA[sec], DATA_FLASH_SIZE ) );

	if( sec == 0 )
	{
		sprintf( f_path, EEP_FILE_X_A, sec );
		if( SaveVFile( f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE )<0 )
		{
			printf("eep_emul:file write fail A");
			D_Printf( DEBUG_SYSTEM, "eep_emul:file write fail A\r\n");
		}
	
		sprintf( f_path, EEP_FILE_X_B, sec );
		if( SaveVFile( f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE )<0 )
		{
			printf("eep_emul:file write fail B");
			D_Printf( DEBUG_SYSTEM, "eep_emul:file write fail B\r\n");
		}
	}

	else
	{
		sprintf( f_path, EEP_TABLE_FILE_A, sec );
		if( SaveVFile( f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE )<0 )
		{
			printf("eep_emul:file write fail table A");
			D_Printf( DEBUG_SYSTEM, "eep_emul:file write fail table A\r\n");
		}

		sprintf( f_path, EEP_TABLE_FILE_B, sec );
		if( SaveVFile( f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE )<0 )
		{
			printf("eep_emul:file write fail B");
			D_Printf( DEBUG_SYSTEM, "eep_emul:file write fail B\r\n");
		}
	
	}
}

INT8U V_EEP_Write( INT8U sec, INT16U addr, INT8U* data, INT16U size )
{
	// sec 와 쓰기할 데이터 크기 비교.
	if( sec>=DATA_FLASH_SEC_CO ) return 0;
	if( (addr+size)>=DATA_FLASH_SIZE ) return 0;
	
	pthread_mutex_lock( &_mutex_Eep );

	
	// 블럭만큼 비교해서 틀리면, 블럭 단위 write
	if( memcmp( &V_EEP_DATA[sec][addr], data, size ) )
	{
		if(m_Eep.Wr_Cnt32 >= 0xFFFFFFFF) m_Eep.Wr_Cnt32 = 0;
		m_Eep.Wr_Cnt32++;
		//printf( "[EEP] Wr(%d)\r\n", m_Eep.Wr_Cnt32);
		
		memcpy( &V_EEP_DATA[sec][addr], data, size );
		Refresh_V_EEP(sec);
	}
	
	pthread_mutex_unlock( &_mutex_Eep );
	
	return 1;
}

INT8U V_EEP_Read( INT8U sec, INT16U addr, INT8U* data, INT16U size )
{
	if( sec>=DATA_FLASH_SEC_CO ) return 0;
	if( (addr+size)>=DATA_FLASH_SIZE ) return 0;
	
	pthread_mutex_lock( &_mutex_Eep );
	
	memcpy( data, &V_EEP_DATA[sec][addr], size );
	
	pthread_mutex_unlock( &_mutex_Eep );
	
	return 1;
}
///// YCSO added 190627
void V_EEP_Copy_Backup( INT8U sec )
{
	char cmd[128] = {0};
	
	syslog( LOG_WARNING, "[RCU] !! COPY EEP DATA BACK UP %d !!!\n", sec);

	if( sec == 0 )
		sprintf( cmd , "cp %s %s",EEP_FILE_BACKUP	, EEP_FILE_DIR);
	else
		sprintf( cmd , "cp %s %s",EEP_TABLE_BACKUP	, EEP_FILE_DIR);

	system(cmd);
	
}

INT8U V_EEP_Read_All( INT8U sec )
{	
	char f_path[64];
	INT8U cnt = 0;
	///// YCSO changed 190503 START

	syslog( LOG_WARNING, "[RCU] V_EEP_Read_All %d\n", sec);

	if( sec == 0 )
	{
		printf("[RCU] sec %d, A\n", sec);

		for( cnt = 0; cnt < 2; cnt++ )
		{
			if( cnt == 1 )
				V_EEP_Copy_Backup( sec );
			
			sprintf(f_path, EEP_FILE_X_A, sec);
			if(CheckVFile(f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE)==0) // Data OK
				return 1;
			else 		
				syslog( LOG_WARNING, "[RCU] !! EEP Sec%d_A Fail %d \n", sec, cnt);


			syslog( LOG_WARNING, "[RCU] sec %d, B\n", sec);
			
			sprintf(f_path, EEP_FILE_X_B, sec);
			if(CheckVFile(f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE)==0) // Data OK		
				return 1;
			else 
				syslog( LOG_WARNING, "[RCU] !! EEP Sec%d_B Fail %d \n", sec, cnt);
		}

		memset( V_EEP_DATA[sec], 0xFF, FLASH_PAGE_SIZE );
	}
	else
	{
		for( cnt = 0; cnt < 2; cnt ++ )
		{
			if( cnt == 1 )
				V_EEP_Copy_Backup( sec );
			
			sprintf(f_path, EEP_TABLE_FILE_A, sec);
			if(CheckVFile(f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE)==0) // Data OK
				return 1;
			else 		
				syslog( LOG_WARNING, "[RCU] !! EEP Table Sec%d_A Fail %d \n", sec, cnt);
			
			sprintf(f_path, EEP_TABLE_FILE_A, sec);
			if(CheckVFile(f_path, V_EEP_DATA[sec], FLASH_PAGE_SIZE)==0) // Data OK		
				return 1;
			else 
				syslog( LOG_WARNING, "[RCU] !! EEP Table Sec%d_B Fail %d \n", sec, cnt);
		}

		memset( V_EEP_DATA[sec], 0xFF, FLASH_PAGE_SIZE );
	}

////// YCSO change end
	return 0;
}

INT8U V_Read_Rst_Info_All( void )
{
	if(CheckVFile(RST_INFO_FILE_A, V_RST_INFO_DATA, RST_INFO_PAGE_SIZE)==0) // Data OK
		return 1;
	else 
		syslog( LOG_WARNING, "[RCU] !! EEP Rst-Info A Fail.\n");

	if(CheckVFile( RST_INFO_FILE_B, V_RST_INFO_DATA, RST_INFO_PAGE_SIZE)==0) // Data OK		
		return 1;
	else 
		syslog( LOG_WARNING, "[RCU] !! EEP Rst-Info B Fail.\n");

	memset(V_RST_INFO_DATA, 0xFF, RST_INFO_PAGE_SIZE);

	return 0;
}

void InitEEP( void )
{
	int i;

	printf("[RCU] InitEEP.\n");

	if(pthread_mutex_init(&_mutex_Eep, NULL))
	{
		syslog(LOG_ERR, "[RCU] !! Mutex Init Fail.");
		exit(EXIT_FAILURE);
	}

	pthread_mutex_lock(&_mutex_Eep);

	V_Read_Rst_Info_All();

	for( i=0; i<DATA_FLASH_SEC_CO; i++ )
		V_EEP_Read_All( i );

	pthread_mutex_unlock( &_mutex_Eep );
}


/* EOF */

