/********************************************************************************/
#ifndef __LOG_EVT_H_
#define __LOG_EVT_H_
/********************************************************************************/
#ifdef  LOG_EVT_GLOBALS
#define LOG_EVT_EXT
#else
#define LOG_EVT_EXT extern
#endif
/**************************************************************/
LOG_EVT_EXT int Wr__Event_to_LogFile(char *fmt, ...);
LOG_EVT_EXT void Check__Eventlog_Directory(void);



/********************************************************************************/
#endif
/********************************************************************************/