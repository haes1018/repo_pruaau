/////////////////////////////
//	DEBUG_LIB.C
/////////////////////////////
#define DEBUG_LIB_GLOBALS
#include "includes.h"
/////////////////////////////


#define DEFAULT_TXT_LOG		(DEBUG_SYSTEM)
#define DBG_CFG_COMM_CNT 32
#define DBG_CFG_TEXT_CNT 32


#define _D_CMD_LOGOFF   0x00
#define _D_CMD_TXT_D_EN 0x01
#define _D_CMD_COM_D_EN 0x02
#define _D_CMD_LOG_EN   0x03

#define MAX_LOG_SIZE	0x100000	// 1 MByte

#define SERVER_FIFO "/tmp/kstd_fifo"
static int D_FIFO_Fd;

struct request
{
    unsigned int cmd_id;
    unsigned int cmd_val;
};

static pthread_mutex_t	_mutex_Debug;


//INT16U Dbg_CFG_TimeStamp = 0;
//INT16U Dbg_CFG_Text_File_Save = 1;
//INT16U Dbg_CFG_Comm_File_Save = 0;
INT32U Dbg_CFG_Text_En = DEFAULT_TXT_LOG;
INT32U Dbg_CFG_Comm_En = 0;

static FILE* txt_log_fp;
static FILE *comm_log_fp[DBG_CFG_COMM_CNT];
static FILE* text_log_fp[DBG_CFG_COMM_CNT];


/*
// Config File 형식
// Line No:Data
//       1:0 or 1 -- Time Stamp 유무 ( 0: none, 1:stamp 사용 )
//       2:0 or 1 -- Debug 파일 저장 유무 ( 0: Printf, 1:파일로 저장 )
//       3:0 or 1 -- 통신 Debug 파일 저장 유무 ( 0: Printf, 1:파일로 저장 )
//       4:0000 ~ FFFF -- Debug 표시 각 Bit별 사용 ( 0: Debug 표시 안함, 1:Debug 표시)
//       5:0000 ~ FFFF -- 통신 Tx Debug 표시 각 Bit별 사용 ( 0: Debug 표시 안함, 1:Debug 표시)
//       6:0000 ~ FFFF -- 통신 Rx Debug 표시 각 Bit별 사용 ( 0: Debug 표시 안함, 1:Debug 표시)


int ReadDebugConfigFile( const char *configFilename )
{
	int i;
    FILE *configfp;
    INT32U cfg_data[6];

    int config_file_fail;

    config_file_fail = 1;

    configfp = fopen(configFilename, "rt");
	if( configfp == NULL )
	{
//		printf("config file open error\n");
		return -1;
	}

	for( i=0; i<6; i++ )
	{
		if( fscanf( configfp, "%x", &cfg_data[i] )<0 ) break;
	}
	fclose(configfp);


	if( i==6 )
	{

		if( cfg_data[0] != 1 ) Dbg_CFG_TimeStamp = 0;
		else Dbg_CFG_TimeStamp = 1;

		if( cfg_data[1] != 1 ) Dbg_CFG_Text_File_Save = 0;
		else Dbg_CFG_Text_File_Save = 1;

		if( cfg_data[2] != 1 ) Dbg_CFG_Comm_File_Save = 0;
		else Dbg_CFG_Comm_File_Save = 1;

		Dbg_CFG_Text_En = cfg_data[3];
		Dbg_CFG_Tx_Comm_En = cfg_data[4];
		Dbg_CFG_Rx_Comm_En = cfg_data[5];
		config_file_fail = 0;

		if( Dbg_CFG_Text_File_Save )
		{
			if( txt_log_fp == NULL )
			{
				txt_log_fp = fopen( DEBUG_LOG_FILE, "a" );

				if( txt_log_fp != NULL )
					setbuf(txt_log_fp, NULL); // Disable stdio buffering

				D_Printf( DEBUG_SYSTEM, "Debug Log Save Start\n" );

			}
		}
		else
		{
			if( txt_log_fp != NULL )
			{
				fclose(txt_log_fp);
				txt_log_fp = NULL;
			}
		}

		D_Printf( DEBUG_SYSTEM, "Text Debug ");
		if( Dbg_CFG_Text_File_Save ) D_Printf(DEBUG_SYSTEM, "Log Save\n");
		else D_Printf(DEBUG_SYSTEM, "Console Out\n");

		D_Printf(DEBUG_SYSTEM, "Comm Debug ");
		if( Dbg_CFG_Comm_File_Save ) D_Printf(DEBUG_SYSTEM, "Log Save\n");
		else D_Printf(DEBUG_SYSTEM, "Console Out\n");

		D_Printf( DEBUG_SYSTEM, "Text Debug Cfg %04X\n", Dbg_CFG_Text_En );
		D_Printf( DEBUG_SYSTEM, "Comm Tx Debug Cfg %04X\n", Dbg_CFG_Tx_Comm_En );
		D_Printf( DEBUG_SYSTEM, "Comm Rx Debug Cfg %04X\n", Dbg_CFG_Rx_Comm_En );
	}

	return config_file_fail;
}
*/




void Chk_CommLogSize( INT8U port )
{
	char log_file_name[DEBUG_COMM_LOG_FILE_BUF_SIZE];

	if(port >= DBG_CFG_COMM_CNT) return;
	
	if(comm_log_fp[port])
	{
		if( ftell(comm_log_fp[port]) > MAX_LOG_SIZE*2 )
		{
			printf("%s[Chk_Comm%d LogSize] size %d > %d", 
					VT_BOLD_MAGENTA, port, (int)ftell(comm_log_fp[port]), MAX_LOG_SIZE*2);
			printf("%s", VT_END_CR);

			sprintf(log_file_name, DEBUG_COMM_LOG_FILE, port); 
			truncate(log_file_name, 0);		
		}
	}
}


void CheckCommLogSize( INT8U port )
{
#ifdef __TEST_DEBUG__
	char log_file_name[DEBUG_COMM_LOG_FILE_BUF_SIZE];
	char log_file__old[DEBUG_COMM_LOG_FILE_BUF_SIZE];
#endif

	if(port >= DBG_CFG_COMM_CNT) return;
	
	if(comm_log_fp[port])
	{
		if( ftell(comm_log_fp[port]) > MAX_LOG_SIZE )
		{
			fclose(comm_log_fp[port]);
			comm_log_fp[port] = NULL;

			//printf("%s[CheckCommLogSize] size %d, %d", VT_BOLD_MAGENTA, port, (int)ftell(comm_log_fp[port]));
			//printf("%s", VT_END_CR);		
		}
	}
}



void CheckTextPortLogSize( INT8U port )
{
#ifdef __TEST_DEBUG__
	char log_file_name[DEBUG_TEXT_LOG_FILE_BUF_SIZE];
	char log_file__old[DEBUG_TEXT_LOG_FILE_BUF_SIZE];
#endif

	if(port >= DBG_CFG_COMM_CNT) return;

	if( text_log_fp[port] )
	{
		if(ftell(text_log_fp[port]) > MAX_LOG_SIZE )
		{
			fclose(text_log_fp[port]);
			text_log_fp[port] = NULL;	
		}
	}	
}


void OpenTextLogFile( void )
{
	txt_log_fp = fopen( DEBUG_LOG_FILE, "w" );

	if(txt_log_fp != NULL)
	{
		setbuf(txt_log_fp, NULL); // Disable stdio buffering
	}
}


void CheckTextLogSize( void )
{
	if(txt_log_fp != NULL)
	{
		if( ftell( txt_log_fp ) > MAX_LOG_SIZE )
		{
//			Dbg_CFG_Text_File_Save = 0;
			fclose(txt_log_fp);

			system( DEBUG_LOG_FILE_BACKUP );
			sync();
			OpenTextLogFile();
			// txt_log_fp = NULL;
//			printf("Log File Size Full\n" );
		}
	}
}

void OpenTextPortLogFile( void )
{
	char log_file_name[DEBUG_COMM_LOG_FILE_BUF_SIZE];
	int i;

	for( i=0; i<DBG_CFG_COMM_CNT; i++ )
	{
		if(Dbg_CFG_Text_En & cbits32[i])
		{
			if( text_log_fp[i] == NULL )
			{
				sprintf( log_file_name, DEBUG_TEXT_LOG_FILE, i );
				text_log_fp[i] = fopen( log_file_name, "a" );
				CheckTextPortLogSize( i );

				if( text_log_fp[i] != NULL )
					setbuf(text_log_fp[i], NULL); // Disable stdio buffering
			}
		}
		else
		{		
			CheckTextPortLogSize(i);			
			if( text_log_fp[i] != NULL )
			{	
				fclose(text_log_fp[i]);
				text_log_fp[i] = NULL;
			}
		}
	}
}



void OpenCommLogFile( void )
{
	char log_file_name[DEBUG_COMM_LOG_FILE_BUF_SIZE];
	int i;

	for( i=0; i<DBG_CFG_COMM_CNT; i++ )
	{
		if(Dbg_CFG_Comm_En & cbits32[i])
		{
			//printf("%s[OpenCommLogFile] flag on  %d" VT_END_CR, VT_BOLD_YELLOW, i);
						
			if( comm_log_fp[i] == NULL )
			{
				sprintf( log_file_name, DEBUG_COMM_LOG_FILE, i );
				comm_log_fp[i] = fopen( log_file_name, "a" );				

				if( comm_log_fp[i] != NULL )
					setbuf(comm_log_fp[i], NULL); // Disable stdio buffering
			}
		}
		else
		{			
			//printf("%s[OpenCommLogFile] flag off -- %d" VT_END_CR, VT_BOLD_YELLOW, i);
			
			if( comm_log_fp[i] != NULL )
			{				
				printf("%s[OpenCommLogFile] size %d, %d", VT_BOLD_MAGENTA, i, (int)ftell(comm_log_fp[i]));
				printf("%s", VT_END_CR);
				
				fclose(comm_log_fp[i]);
				comm_log_fp[i] = NULL;
			}
		}
	}
}




/*
1.show_comm_debug.sh 0

_D_CMD_LOGOFF 
with Dbg_CFG_Comm_En = 0;

_D_CMD_COM_D_EN 
with Dbg_CFG_Comm_En = cmd_val;

*/

void SetDebugCmd( INT32U cmd_id, INT32U cmd_val )
{
	pthread_mutex_lock( &_mutex_Debug );
	
	switch( cmd_id )
	{
		case _D_CMD_LOGOFF:
			Dbg_CFG_Text_En = 0; // DEBUG_SYSTEM;
			Dbg_CFG_Comm_En = 0;
			//Dbg_CFG_Text_File_Save = 0;
			OpenTextLogFile();
			OpenCommLogFile();
			break;
			
		case _D_CMD_TXT_D_EN:
			Dbg_CFG_Text_En = cmd_val;
			OpenTextPortLogFile();
			break;
			
		case _D_CMD_COM_D_EN:
			Dbg_CFG_Comm_En = cmd_val;
			OpenCommLogFile();			
			break;
			
		case _D_CMD_LOG_EN:
			//Dbg_CFG_Text_File_Save = cmd_val;
			//OpenTextLogFile();
			break;
	}
	pthread_mutex_unlock( &_mutex_Debug );
}


void DebugConfig( void )
{
    struct request req;

	if( read(D_FIFO_Fd, &req, sizeof(struct request)) == sizeof(struct request) )
	{
		//D_Printf(DEBUG_SYSTEM, "[RCU] Read FIFO 0x%04X, 0x%04X\n", req.cmd_id, req.cmd_val );
		SetDebugCmd( req.cmd_id, req.cmd_val );
	}
}



void Debug_Init( void )
{
	int res;

	res = pthread_mutex_init(&_mutex_Debug, NULL);

	if(res != 0) // if fail
	{
		syslog(LOG_ERR, "[RCU] !! Mutex Init Fail.");
		exit(EXIT_FAILURE);
	}
	umask(0); // So we get the permissions we want

	if(mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) < 0)
	{
		syslog(LOG_ERR, "[RCU] !! dbg mkfifo" );
	}

	D_FIFO_Fd = open(SERVER_FIFO, O_RDONLY | O_NONBLOCK );
	if(D_FIFO_Fd == -1)  // if fail
	{
		syslog(LOG_ERR, "[RCU] !! Open dbg fifo");
	}

	txt_log_fp = NULL;

	for(res=0; res<DBG_CFG_COMM_CNT; res++)
		comm_log_fp[res] = NULL;
	for(res=0; res<DBG_CFG_TEXT_CNT; res++)
		text_log_fp[res] = NULL;
	
	
	Dbg_CFG_Text_En = DEFAULT_TXT_LOG;
	Dbg_CFG_Comm_En = 0;
	OpenTextLogFile();
}


void TimeStampOut( FILE* fp )
{
#ifdef DEBUG_TIME_STAMP_DATE_EN
    const char *TIMESTAMP_FMT = "%F %X";        /* = YYYY-MM-DD HH:MM:SS */
#define TS_BUF_SIZE sizeof("YYYY-MM-DD HH:MM:SS")       /* Includes '\0' */
#else
    const char *TIMESTAMP_FMT = "%X";        /* = YYYY-MM-DD HH:MM:SS */
#define TS_BUF_SIZE sizeof("HH:MM:SS")       /* Includes '\0' */
#endif

    char timestamp[TS_BUF_SIZE];
    time_t t;
    struct tm *loc;

	t = time(NULL);
	loc = localtime(&t);
	if(loc == NULL || strftime(timestamp, TS_BUF_SIZE, TIMESTAMP_FMT, loc) == 0)
	{
		fprintf( fp, "???Unknown time????" );
	}

	fprintf(fp, "%s: ", timestamp);
}









// "DEBUG_SYSTEM"
void D_Printf( INT16U mask, char *form, ...)
{
    va_list argptr;    

	// Debug 선택이 안되었음
    if((mask & Dbg_CFG_Text_En) == 0) return;

	pthread_mutex_lock( &_mutex_Debug );

//	if( (Dbg_CFG_Text_File_Save!=0) && (txt_log_fp!=NULL) ) // 파일 저장시만 Stamp
	TimeStampOut( txt_log_fp );

    va_start(argptr, form);

	if( txt_log_fp > 0 )
	{
		vfprintf( txt_log_fp, form, argptr);
		vfprintf( txt_log_fp, VT_END_CR, argptr);
		CheckTextLogSize();
	}
	//else vprintf( form, argptr);

    va_end(argptr);

	pthread_mutex_unlock(&_mutex_Debug);
}


void D4_Printf(INT16U mask, INT8U path4, char *form, ...)
{
    va_list argptr;    

	// Debug 선택이 안되었음
	if(Is_Debug_Path4(path4)==0) return;
    if((mask & Dbg_CFG_Text_En) == 0) return;

	pthread_mutex_lock( &_mutex_Debug );

//	if( (Dbg_CFG_Text_File_Save!=0) && (txt_log_fp!=NULL) ) // 파일 저장시만 Stamp
	TimeStampOut( txt_log_fp );

    va_start(argptr, form);

	if( txt_log_fp > 0 )
	{
		vfprintf( txt_log_fp, form, argptr);
		vfprintf( txt_log_fp, VT_END_CR, argptr);
		CheckTextLogSize();
	}
	//else vprintf( form, argptr);

    va_end(argptr);

	pthread_mutex_unlock(&_mutex_Debug);
}

void Do4_Printf(INT16U mask, INT8U optic, INT8U path4, char *form, ...)
{
    va_list argptr;    

	// Debug 선택이 안되었음
	if(Is_Debug_Optic_Path(optic, path4)==0) return;
    if((mask & Dbg_CFG_Text_En) == 0) return;

	pthread_mutex_lock( &_mutex_Debug );

//	if( (Dbg_CFG_Text_File_Save!=0) && (txt_log_fp!=NULL) ) // 파일 저장시만 Stamp
	TimeStampOut( txt_log_fp );

    va_start(argptr, form);

	if( txt_log_fp > 0 )
	{
		vfprintf( txt_log_fp, form, argptr);
		vfprintf( txt_log_fp, VT_END_CR, argptr);
		CheckTextLogSize();
	}
	//else vprintf( form, argptr);

    va_end(argptr);

	pthread_mutex_unlock(&_mutex_Debug);
}


typedef struct
{
	INT8U idxMsgId;
	char* strMsgId;
}LS_PKT_STR;

#define MAX_LS_PKT_STR (22)
static LS_PKT_STR MsgId[MAX_LS_PKT_STR] = 
{
	{ 0x9A, "RU ID NOTI"    },
	{ 0x9B, "RU ID NOTI ACK"},
	{ 0x02, "INIT NOTI"     },
	{ 0x03, "INIT NOTI ACK" },
	{ 0x10, "SYSTEM STATUS REQ" },
	{ 0x11, "SYSTEM STATUS RSP"},
	{ 0x26, "ALM INFO NOTI"},
	{ 0x27, "ALM INFO ACK" },
	{ 0x28, "ALM INFO REQ" },
	{ 0x29, "ALM INFO RSP"},
	{ 0x34, "RESET REQ"   },
	{ 0x35, "RESET RSP"   },
	{ 0x5C, "DELAY SETUP REQ"  },
	{ 0x5D, "DELAY SETUP RSP" },
	{ 0x98, "C&M PATH NOTI"    },
	{ 0x99, "C&M PATH NOTI ACK"},
	{ 0x1C, "CPRI STATUS REQ"  },
	{ 0x1D, "CPRI STATUS RSP" },
	{ 0x14, "FA STATUS REQ"    },
	{ 0x15, "FA STATUS RSP"   },
	{ 0x18, "PATH STATUS REQ"  },
	{ 0x19, "PATH STATUS RSP" }
};

void Debug_Comm_Data( INT8U port, INT8U data_start, INT8U trx, INT8U*data, INT16U len ) // trx 0:tx, 1:rx
{
	INT32U i, k, idx;
	INT8U ls_msg_id;
	int l;
	LSH_PROTOCOL_FRAME* pLS;
	INT32U val32;

	if((Dbg_CFG_Comm_En & cbits32[port]) == 0) return;


	pthread_mutex_lock( &_mutex_Debug );

	
	Chk_CommLogSize( port ); // 2019.09.26
	
	if( comm_log_fp[port] )
	{		
		if( data_start )
		{			
			if(trx == 0)
				 fprintf( comm_log_fp[port], VT_Color_TX "\n<Tx %d> " VT_END_CR, port );
			else fprintf( comm_log_fp[port], VT_Color_RX "\n<Rx %d> " VT_END_CR, port );
			
			TimeStampOut( comm_log_fp[port] );
			fprintf( comm_log_fp[port], "\n");
		}

		switch(port)
		{
			case COMM_UDP_LS:
				pLS = (LSH_PROTOCOL_FRAME*)data;
				val32 = *(INT32U*)data;
				l = ntohl(pLS->len);
				
				ls_msg_id = (INT8U)ntohl(val32);

				for(idx=0; idx < MAX_LS_PKT_STR; idx++)
				{
					if(ls_msg_id == MsgId[idx].idxMsgId)
						break;
				}

				if(idx < MAX_LS_PKT_STR)
					fprintf( comm_log_fp[port], "MSG-ID  : 0x%08X, %s\n", ntohl(val32), MsgId[idx].strMsgId);
				else
					fprintf( comm_log_fp[port], "MSG-ID  : 0x%08X, ?? \n", ntohl(val32));
					
				fprintf( comm_log_fp[port], "MSG-Len : 0x%08X \n", ntohl(pLS->len));
				fprintf( comm_log_fp[port], "MSG-Seq#: 0x%08X \n", ntohl(pLS->SeqNum));
				fprintf( comm_log_fp[port], "MSG-Data: (%d byte)\n\n", l);

				if(l > MAX_COM_BUFFER) 
				{
					fprintf( comm_log_fp[port], VT_BLINK_RED "Data Over !! ----- \n" VT_END_CR);
					l = 0; // limit max length
				}
							
				if(l > 0)
				{				
					for(i=0,k=0; i<l; i++)
					{				
						if(trx == 0)
							fprintf( comm_log_fp[port], VT_Color_TX "%02X ", data[i] );
						else
							fprintf( comm_log_fp[port], VT_Color_RX "%02X ", data[i] );
						k++;
						if(k>=32) 
						{
							fprintf( comm_log_fp[port], "\n");
							k=0;
						}
					}
				}
				break;
			
		
			case COMM_UDP_NODE:
			case COMM_UDP_DTU:
		#ifdef __DEL_UDP_REMS__
		#else	
			case COMM_UDP_REMS:
		#endif	
			case COMM_TERM:
			#ifdef TYPE__AAU 
			case COMM_BT:
			#endif		
				for(i=0,k=0; i<len; i++)
				{
					if(trx == 0)
						fprintf( comm_log_fp[port], VT_Color_TX "%02X ", data[i] );
					else
						fprintf( comm_log_fp[port], VT_Color_RX "%02X ", data[i] );
					k++;
					if(k>=32) 
					{
						fprintf( comm_log_fp[port], "\n");
						k=0;
					}
				}
				break;
			
			default: // AMP/PSU
				for(i=0,k=0; i<len; i++)
				{
					if(trx == 0)
						fprintf( comm_log_fp[port], VT_Color_TX "%02X ", data[i] );
					else
						fprintf( comm_log_fp[port], VT_Color_RX "%02X ", data[i] );
					k++;
					if(k>=32) 
					{
						fprintf( comm_log_fp[port], "\n");
						k=0;
					}
				}
				break;
		}		
		
		fprintf( comm_log_fp[port], "\n" VT_END_CR);
	}
	pthread_mutex_unlock( &_mutex_Debug );
}



/* EOF */



