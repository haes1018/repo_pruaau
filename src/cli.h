/********************************************************************************/
#ifndef __CLI_H
#define __CLI_H
/********************************************************************************/
#ifdef  CLI_GLOBALS
#define CLI_EXT
#else
#define CLI_EXT extern
#endif
/**************************************************************/

#define	PROGRAM_NAME			"cli_program"
#define	PROGRAM_VERSION			"0.01"

#define	PROMPT				"CLI>"

#define	MAX_ARGV_CNT		16
#define	MAX_USER_CMD_BUF	160


CLI_EXT void * alloc_memory(int size);
CLI_EXT void * realloc_memory(void *p_mem, int size);
CLI_EXT void free_memory(void *p_mem);
CLI_EXT void hex_dump();

////////////////////////////////////////////////////////////////////////////////////

CLI_EXT int CLI_Process( void );

/*----------------------------------------------------------*/


/********************************************************************************/
#endif
/********************************************************************************/

/* EOF */

