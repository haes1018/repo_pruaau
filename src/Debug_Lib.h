
/********************************************************************************/
#ifndef _DEBUG_LIB_H_
#define _DEBUG_LIB_H_
/********************************************************************************/
#ifdef  DEBUG_LIB_GLOBALS
#define DEBUG_LIB_EXT
#else
#define DEBUG_LIB_EXT extern
#endif
/**************************************************************/


#define DEBUG_ALL	    (INT32U)(0xFFFFFFFE)

#define DEBUG_SYSTEM	BIT(0)
#define DEBUG_PSU		BIT(1) // debug #1
#define DEBUG_LS_Comm	BIT(2) // debug #2
#define DEBUG_STDBY		BIT(3) // amp standby, mimo standby
#define DEBUG_UDA		BIT(4) // UDA
#define DEBUG_DPD		BIT(5) // DPD
#define DEBUG_REMS	    BIT(6)
#define DEBUG_LOG	    BIT(7) // LOG
#define DEBUG_ETC		BIT(15) // etc



#define	DEBUG_CONF_FILE		"/tmp/kst_log.conf"
#define	DEBUG_LOG_FILE		"/tmp/kst_log.txt"
#define	DEBUG_LOG_FILE_BACKUP	"mv -f /tmp/kst_log.txt /tmp/kst_log.txt.old"

#define	DEBUG_COMM_LOG_FILE	"/tmp/kst_comm_log_port%02d.txt"
#define	DEBUG_COMM_LOG__OLD	"/tmp/kst_comm_log_port%02d.old"


#define	DEBUG_TEXT_LOG_FILE	"/tmp/kst_text_log_port%02d.txt"
#define	DEBUG_TEXT_LOG__OLD	"/tmp/kst_text_log_port%02d.old"





#define DEBUG_COMM_LOG_FILE_BUF_SIZE (sizeof("/tmp/kst_comm_log_port%02d.txt")+2)
#define DEBUG_TEXT_LOG_FILE_BUF_SIZE (sizeof("/tmp/kst_text_log_port%02d.txt")+2)

DEBUG_LIB_EXT int ReadDebugConfigFile( const char *configFilename );

DEBUG_LIB_EXT void D_Printf( INT16U mask, char *form, ...);
DEBUG_LIB_EXT void D4_Printf(INT16U mask, INT8U path4, char *form, ...);
DEBUG_LIB_EXT void Do4_Printf(INT16U mask, INT8U optic, INT8U path4, char *form, ...);



DEBUG_LIB_EXT void Debug_Comm_Data( INT8U port, INT8U data_start, INT8U trx, INT8U*data, INT16U len ); // trx 0:tx, 1:rx

DEBUG_LIB_EXT void Debug_Init( void );
DEBUG_LIB_EXT void DebugConfig( void );

DEBUG_LIB_EXT void OSM_On_Ctrl(void);

#endif

/*EOF*/
