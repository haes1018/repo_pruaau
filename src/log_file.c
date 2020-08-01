/////////////////////////////
//	LOG_FILE.C
/////////////////////////////
#define LOG_FILE_GLOBALS
#include "includes.h"
#include <math.h>
/////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h> // malloc func
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>




/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#define	SINGLE_IO_SIZE				(4 * 1024)
#define	DEFAULT_FILE_MODE			0644

#define	Sym_Link__File  "/tmp/rcu.log"
#define	Sym_Link__PATH	"/usr/app/log"

#define	LOG_ALARM_PATH	"/usr/app/log/alm"
#define	LOG__CTRL_PATH	"/usr/app/log/cmd"
#define	LOG__REMS_PATH	"/usr/app/log/rems"
#define LOG_Alm_REMS_File  "rems_alm_rpt.log" // /usr/app/log/rems/rems_alm_rpt.log
#define LOG_Ctl_REMS_File  "rems_ctl_rpt.log" // /usr/app/log/rems/rems_ctl_rpt.log

#ifdef __ALARM_LOG_BACKUP__
#define Ext_old_chksum "old.chksum"
#define Ext_old        "old"
#define Ext_chksum     "chksum"
#endif

#ifdef __ALARM_LOG_BACKUP__	
static pthread_mutex_t	Sem_gREMS_Pktlog_Alm;
static pthread_mutex_t	Sem_gREMS_Pktlog_Ctl;
#endif


#define	MAX_LOG_ALARM_FILES		100 // 20200211 : 12 -> 100
#define	MAX_LOG_CTRL_CMD_FILES	200 // 2019.08.02


/////////////////////////////////////////////////////////////////////////


#ifdef _TEST____ALM_LOG_
#define CHUNK_MAX_Cnt     30  // 15[s] x 2 packet/s = 30 
//#define CHUNK_MAX_Tmp_Cnt 2
#else 
#define CHUNK_MAX_Cnt     (600) // 600 = 300[s] x 2 packet/s
//#define CHUNK_MAX_Tmp_Cnt (20)
#endif
#define CHUNK_Half_Cnt    (CHUNK_MAX_Cnt/2)
#define CHUNK_UNIT        (256) // 256byte unit

enum
{
	LOGGING_OFF = 0,
	LOGGING_1,
	LOGGING_2,
	LOGGING_3,
	
	LOGGING_Max
};

typedef struct
{
	int logging_st; // 1~3 : logging-on,  0: logging-off
	int chunk_size; // status response size, 256 byte step
	int immediate_start; // 1, when user logging off->on and sum-alarm
	int cnt_file_gen;
}ALM_LOG_INFO;

typedef struct
{
	int en;
	int cur_idx;   // 0 ~ (CHUNK_MAX_Cnt-1)
	int start_idx; // 0 ~ (CHUNK_MAX_Cnt-1)
	int end_idx;   // 0 ~ (CHUNK_MAX_Cnt-1)
	INT8U* ptr;    // mem-alloc
}ALM_LOG_BUF;

/////////////////////////////////////////////////////////////////////////


static int _get_error_code()
{
	int code;	

	code = -errno;
	if (code >= 0)
		code = -EIO; // EIO is '5'

	////print_agent(DEBUG_LOG, VT_BOLD_RED, "[_get_error_code] errno %d, EIO %d, return %d", errno, EIO, code);	

	return code;
}

static int _is_numeric_file_name(const struct dirent *p_de)
{
	if (isdigit(p_de->d_name[0]))
	{
		if (strstr(p_de->d_name, ".log"))
			return 1;
		if (strstr(p_de->d_name, ".logcmd"))
			return 1;
	}
	return 0;
}

static int _is_5m_log_file_name(const struct dirent *p_de)
{
#if 1
	if (strstr(p_de->d_name, ".log.tgz"))
		return 1;
#else		
	if (strstr(p_de->d_name, ".log"))
		return 1;
#endif		
	else
		return 0;
}

/*
---------------------------------------------------------------------
check if file count is max count or not.
if current file count is over max count, delete it.
---------------------------------------------------------------------
*/
static int _arrange_max_log_file_count(char *path, int max_count)
{
	int n;
	int i;
	struct dirent **pp_namelist;
	int file_count;
	int over_count;
	char filename[512];
	//int errcode;

	if(path == NULL || max_count <= 0)
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] log_file; path == NULL || max_count <= 0");
		return -1;
	}

	pp_namelist = NULL;
	file_count = 0;	
	n = scandir(path, &pp_namelist, _is_numeric_file_name, alphasort);	
	
	if(n >= 0)	
	{
		file_count = n;		
		if(file_count >= max_count)
		{
			over_count = file_count - max_count + 1;
			file_count = max_count;
		}
		else
		{
			over_count = 0;	// default 0
		}
		for(i=0; i<n; i++)
		{
			if(pp_namelist[i] == NULL)
				break;

			if(i < over_count)
			{
				snprintf(filename, 512, "%s/%s", path, pp_namelist[i]->d_name);
				print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] del log_file %s", filename);
				remove(filename);
				sync();
			}
			free(pp_namelist[i]);
		}
		if (pp_namelist)
			free(pp_namelist);
	}
	else
	{		
		//errcode = _get_error_code();
		//print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] path(%s) log_file count %d < 0, err code %d", 
		//	path, n, errcode);
	}
	return file_count;
}

/*
----------------------------------------------------------
* Get log file num of *.log, *.logcmd
----------------------------------------------------------
*/
static int _get_log_file_count(char *path ) // v
{
	int n;	
	struct dirent **pp_namelist;
	int file_count;	
	int errcode;

	if (path == NULL)
		return -1;

	pp_namelist = NULL;
	file_count = -1;

	n = scandir(path, &pp_namelist, _is_numeric_file_name, alphasort);
	if (n < 0)
	{
		errcode = _get_error_code();
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] [_get_log_file_count] error %d", errcode);
		return errcode;
	}
	else
	{
		file_count = n;
		
		if (pp_namelist)
			free(pp_namelist);
	}
	return file_count;
}


// len 를 4k 단위로 여러번에 걸쳐 write 함.
// O_TRUNC 기존 내용 삭제.

static int _write_log_file( char *pull_path_name, unsigned char *packet, int len, INT8U f_append )
{
	int fd;
	int count;
	int offset;
	int iosize;
	int flags;
	int errcode;

	if(f_append) flags = O_RDWR | O_APPEND;
	else         flags = O_CREAT| O_RDWR | O_TRUNC;	

	fd = open(pull_path_name, flags , DEFAULT_FILE_MODE);
	
	if (fd < 0)
	{
		errcode = _get_error_code();
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] [_write_log_file] errcode %d", errcode);
		return errcode; 
	}

	count = 0;
	iosize = 0;
	for (offset=0; offset < len; offset += iosize)
	{
		int resid;
		int iocnt;

		resid = len - offset;
		if (resid > SINGLE_IO_SIZE)
			iosize = SINGLE_IO_SIZE;
		else
			iosize = resid;

		iocnt = write(fd, (void *)&packet[offset], iosize);
		sync();
		if(iocnt < 0)
		{
			errcode = _get_error_code();
			print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] [_write_log_file] error %d", errcode);
			if (fd >= 0)
			{
				close(fd);
				sync();
			}
			return errcode;
		}
		else if (iocnt < iosize)
		{
			count += iocnt;
			break;
		}
		count += iocnt;
	}

	if (fd >= 0)
	{
		close(fd);
		sync();
	}

	return count;
}




//----------------------------------------------------------------------

enum
{
	_Log_Alm = 0,
	_Log_Cmd,
	_Log_Rst,
};


//----------------------------------------------------------------------
// 25 글자 이내이어야...
void Make_Log_File_Name( INT8U log_type, char* pfile_name, INT16U len )
{
	char date[20];

	sprintf( date, "%02d%02d%02d%02d%02d%02d",
		m_St.Ru_Sys_Date[_ye_],m_St.Ru_Sys_Date[_mo_],m_St.Ru_Sys_Date[_da_],
		m_St.Ru_Sys_Date[_ho_],m_St.Ru_Sys_Date[_mi_],m_St.Ru_Sys_Date[_se_]);

	if(log_type == _Log_Alm)
	{
		sprintf( pfile_name, "%s[%05d].log", date, len);
	}
	else if(log_type == _Log_Cmd)
	{
		sprintf( pfile_name, "%s.logcmd", date);
	}	
}

//----------------------------------------------------------------------

INT8U Is_InValid__SysTime(void)
{
#ifdef _TEST____ALM_LOG_
	return 0;
#endif

	if(m_St.Ru_Sys_Date[_ye_] < 19) return 1;
	if(m_St.Ru_Sys_Date[_mo_] < 1) return 1;
	if(m_St.Ru_Sys_Date[_mo_] > 12) return 1;

	if(m_St.Ru_Sys_Date[_da_] < 1) return 1;
	if(m_St.Ru_Sys_Date[_da_] > 31) return 1;

	if(m_St.Ru_Sys_Date[_ho_] > 23) return 1;
	if(m_St.Ru_Sys_Date[_mi_] > 59) return 1;
	if(m_St.Ru_Sys_Date[_se_] > 59) return 1;

	return 0; // valid
}




int NAND_Wr_Alarm_log( ALM_LOG_BUF* pAlmLog, int max, int chunk_size, char* pfile_name )
{
	int re;
	int offset;
	int idx;
	char pull_path_name[256];

	if(pAlmLog->ptr == NULL ) 
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] pAlmLog->ptr == NULL");
		return -EINVAL;
	}
	if(Is_InValid__SysTime()) 
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] system time is not valid");
		return -EINVAL;
	}	
	
	re = _arrange_max_log_file_count(LOG_ALARM_PATH, MAX_LOG_ALARM_FILES);
	if (re < 0)
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[wr_alarm_log] arrange_max_log_file_count fail (%d)", -re);
		return re;
	}

	// sprintf 처럼 쓰되 최대 256, 끝은 null 보장.
	snprintf(pull_path_name, 256, "%s/%s", LOG_ALARM_PATH, pfile_name); 
	
	idx = pAlmLog->start_idx;
	offset = idx * chunk_size;	
	
	re = _write_log_file(pull_path_name, &pAlmLog->ptr[offset], (max - idx)*chunk_size, 0);	
	if(re < 0)
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[wr_alarm_log] build failed ... (%d)\n", -re);
	}	
	else if(pAlmLog->start_idx > 0)
	{	
		re = _write_log_file(pull_path_name, pAlmLog->ptr, offset, 1);
		if (re < 0)
		{
			print_agent(DEBUG_LOG, VT_BOLD_RED, "[wr_alarm_log] build2 failed ... (%d)\n", -re);
		}
	}
	return re;
}
/////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------


// command log file
static INT8U stx[4] = {0x16,0x16,0x16,0x16}; 
#ifdef _USE_wr_ctrl_cmd_log_
static char cmd_log_file[25];
#endif
#define CMD_LOG_Max (2000)
static INT8U cmd_log_buf[CMD_LOG_Max+48];


int wr_ctrl_cmd_log( PROTOCOL_FRAME* rxf )
{
#ifdef _USE_wr_ctrl_cmd_log_
	int re;	
	int len;
	INT8U swap;
	char pull_path_name[256];
	
	if( CMD_LOG_Max < rxf->len ) return -EINVAL;
	if( Is_InValid__SysTime() ) return -EINVAL;
	
	memcpy( cmd_log_buf, stx, 4);	
	memcpy( &cmd_log_buf[4], (INT8U*)rxf, rxf->len + 14); // header+data
	len = 4 + 14 + rxf->len; // stx(4) + header(14) + data
	cmd_log_buf[len++] = rxf->crc[0];
	cmd_log_buf[len++] = rxf->crc[1];
	cmd_log_buf[len++] = 0x03; // ETX

	//-------------------------------------------
	// swap : length, seq num...
	//-------------------------------------------
	swap = cmd_log_buf[4];  
	cmd_log_buf[4]  = cmd_log_buf[5];  
	cmd_log_buf[5] = swap;

	swap = cmd_log_buf[8];  
	cmd_log_buf[8]  = cmd_log_buf[9];  
	cmd_log_buf[9] = swap;
	
	swap = cmd_log_buf[16]; 
	cmd_log_buf[16] = cmd_log_buf[17]; 
	cmd_log_buf[17] = swap;
	//-------------------------------------------
	Make_Log_File_Name( _Log_Cmd, cmd_log_file, 0 );
	// sprintf 처럼 쓰되 최대 256, 끝은 null 보장.
	snprintf( pull_path_name, 256, "%s/%s", LOG__CTRL_PATH, cmd_log_file); 

	re = _arrange_max_log_file_count(LOG__CTRL_PATH, MAX_LOG_CTRL_CMD_FILES);
	if (re < 0)
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[wr_ctrl_cmd_log] arrange control command log failed ... (%d)\n", -re);
		return re;
	}
	re = _write_log_file( pull_path_name, cmd_log_buf, len, 0);
	if (re < 0)
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[wr_ctrl_cmd_log] control command log build failed ... (%d)\n", -re);

	return re;
#else
	(void)rxf;
	return 1;	
#endif	
}
//===============================================================================================















/*
===============================================================================================

Command 0xF4 : 
	Control log from gREMS 
	Alarm log to gREMS

===============================================================================================
*/

/*
----------------------------------------------
count  : address 0x00000000 ~ 0x00000003 : 
header : address 0x00000010 ~ 0x0000040F : 4byte x 256 : 1kB
body   : address 0x00000410 ~ 
----------------------------------------------
00000000  01 00 00 00 
00000010  30 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
*
00000410  
----------------------------------------------
*/ 

#define PKT_LOG_MAX_RANGE 256 // 16의 배수. 16byte 단위로 처리하고자 함.
#define PKT_LOG_MOV_MAX   200 // move count
#define PKT_LOG_MAX_CNT   100 // protocol max log count

typedef struct
{
	INT32U pkt_next; // next index
	INT32U reservd[3];
	INT32U pkt_size[PKT_LOG_MAX_RANGE];
}PKT_LOG_HEADER;
static PKT_LOG_HEADER  AlmPkt_Log_Header;
static PKT_LOG_HEADER  CtlPkt_Log_Header;
static PKT_LOG_HEADER  TmpPkt_Log_Header;


#define Temp_Alm_Pkt_Log_File "/tmp/tmp_pkt_log_tmp"

static pthread_mutex_t	Move_PktLog_Sem;

static int Move_PktLog_to_NewFile(int fd, char *full_name, PKT_LOG_HEADER* pLogInfo)
{
	int fd_t;
	int posi;
	int idx_start, cnt_copy;
	int size, i;
	static INT8U init = 0;

	if(init == 0)
	{
	 	if(pthread_mutex_init(&Move_PktLog_Sem, NULL) == 0) // normal when return is 0
			init = 1;

		if(init == 0)
		{			
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Move_PktLog_to_NewFile] fail to make mutex");
			return RET_Error; 
		}
	}

	fd_t = open(Temp_Alm_Pkt_Log_File, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);	
	
	if(fd_t < 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Move_PktLog_to_NewFile] fail to create temp-file (%s)", 
			Temp_Alm_Pkt_Log_File);

		return RET_Error;
	}
	//----------------------------------------------------------
	pthread_mutex_lock(&Move_PktLog_Sem);	
	//----------------------------------------------------------

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Move_PktLog_to_NewFile] %s", full_name);
	
	//----------------------------------------------------------
	// get last 100ea index from last
	//----------------------------------------------------------
	idx_start = pLogInfo->pkt_next - PKT_LOG_MAX_CNT;
	if(idx_start < 0) idx_start = 0;
	cnt_copy = pLogInfo->pkt_next - idx_start;

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Move_PktLog_to_NewFile] next %d, idx_start %d cnt_copy %d", 
			pLogInfo->pkt_next, idx_start, cnt_copy);

	//----------------------------------------------------------
	// get start-position to move
	//----------------------------------------------------------
	posi = sizeof(PKT_LOG_HEADER); // offset
	for(i=0; i < idx_start; i++)
		posi += pLogInfo->pkt_size[i]; // i:0~99
	
	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Move_PktLog_to_NewFile] offset %d, posi 0x%X", 
				sizeof(PKT_LOG_HEADER), posi);	
	//----------------------------------------------------------
	// mv header to new file
	//----------------------------------------------------------
	memset(&TmpPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
	
	for(i=0; i<cnt_copy; i++)
		TmpPkt_Log_Header.pkt_size[i] = pLogInfo->pkt_size[idx_start+i];
		
	TmpPkt_Log_Header.pkt_next = cnt_copy;	

	// write header info	
	memcpy(pLogInfo, &TmpPkt_Log_Header, sizeof(PKT_LOG_HEADER));
	lseek(fd_t, 0, SEEK_SET);
	write(fd_t, (INT8U*)pLogInfo, sizeof(PKT_LOG_HEADER));
	
	// move 100-alm-packet
	lseek(fd, posi, SEEK_SET);	
	size = _copy_file(fd_t, fd); // move fd to fd_t
	
	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Move_PktLog_to_NewFile] copied size 0x%X", size);	

	//----------------------------------------------------------
	close(fd_t);
	//----------------------------------------------------------

	//----------------------------------------------------------
	pthread_mutex_unlock(&Move_PktLog_Sem);	
	//----------------------------------------------------------
	if(size > 0)
		return RET_SUCCESS;
	else
		return RET_Error;
}

static int get_file_size(char *file_name)
{
	struct stat st;
	int re;

	re = stat(file_name, &st);

	if (re == 0 && st.st_size > 0)
		return st.st_size;

	return -1;
}



/*
--------------------------------------------------------------------
1. Read header-info (gREMS-alarm-log-info) from file
2. If file not exist, create it.(Initialized)
3. 'Pkt_Log_Header' has header-info @ file
--------------------------------------------------------------------
*/
void Rd_Pkt_Info_from_logfile(int fd, char *full_name, PKT_LOG_HEADER* pLogInfo)
{
	int len;
	int size = sizeof(PKT_LOG_HEADER);
	
	lseek(fd, 0, SEEK_SET);
	len = read(fd, (INT8U*)pLogInfo, size);
	if(len < size)
	{
		// init
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Rd_Pkt_Info_from_logfile] (%s) size fail, init", full_name);
		memset((INT8U*)pLogInfo, 0, size);
		ftruncate(fd, size); // 파일의 크기를 size byte로 변경, 파일 작다면 나머지 '\0'.
	}
}

/*
--------------------------------------------------------------------
1. Write header-info (gREMS-alarm-log-info) to file
2. if file not exist, create it.
3. return size to write
--------------------------------------------------------------------
*/

int Wr_Pkt_Info_to_logfile(int fd, char *full_name, PKT_LOG_HEADER* pLogInfo)
{
	int len;
	int size = sizeof(PKT_LOG_HEADER);

	// read header
	lseek(fd, 0, SEEK_SET);
	len = write(fd, (INT8U*)pLogInfo, size);
	
	if(len < size)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Wr_Pkt_Info_to_logfile] write file [%s] header (%d/%d)", 
					full_name, len, size);
	}
	sync();	
	
	return len;
}

/*
--------------------------------------------------------------------
1. Read the idx-th body(idx 0~ ) from file
2. If file not exist, create it.(Initialized)
3. 'Pkt_Log_Header' has header-info @ file
--------------------------------------------------------------------
*/

int Rd_Pkt_from_logfile(int fd, int idx, INT8U* pPkt, int max_size, PKT_LOG_HEADER* pLogInfo)
{
	INT32U posi;
	INT32U size;
	INT32U len, i;

	if(idx >= PKT_LOG_MOV_MAX) // 0 ~ (PKT_LOG_MOV_MAX-1)
		return 0;	

	posi = sizeof(PKT_LOG_HEADER); // offset
	for(i=0; i<idx; i++)
		posi += pLogInfo->pkt_size[i];
	
	size = pLogInfo->pkt_size[idx];
	if(size > max_size) size = max_size;		

	lseek(fd, posi, SEEK_SET);
	len = read(fd, pPkt, size);		

	return len;
}

/*
--------------------------------------------------------------------
1. Append the idx-th body(idx 0~ ) to file
2. return size of the idx-th body if success, else return 0.
--------------------------------------------------------------------
*/
int Append_Pkt_to_logfile(int fd, char *full_name, INT8U* pPkt, int size)
{
	int len;
	int file_size;
	
	file_size = get_file_size(full_name);

	if(file_size == (-1))
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Append_Pkt_to_logfile] fail, file '%s' size %d ", 
					full_name, file_size);
		return RET_Error;			
	}
	/*
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Append_Pkt_to_logfile] before append, file (%s) size %d ", 
				full_name, file_size);
	*/
	
	// append to end
	lseek(fd, 0, SEEK_END);
	len = write(fd, pPkt, size);
	sync();
	
	if(len == size)
	{
		/*
		file_size = get_file_size(full_name);		
		print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Append_Pkt_to_logfile] after append, file (%s) size %d ", 
					full_name, file_size);
		*/			
		return RET_SUCCESS;
	}
	else
	{		
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Append_Pkt_to_logfile] appending fail, file '%s' ftruncate with size %d", 
			full_name, file_size);

		// 파일을 원래 size로 변경
		ftruncate(fd, file_size); 
		sync();	
			
		return RET_Error;
	}
}




#ifdef __ALARM_LOG_BACKUP__
/*
---------------------------------------------------------------
delete all files
---------------------------------------------------------------
*/


void delete__log(char* pull_path_name)
{	
	system_agent_sync("rm -f %s", pull_path_name);
}
void delete__log_chksum(char* pull_path_name)
{	
	system_agent_sync("rm -f %s.%s", pull_path_name, Ext_chksum);
}
void delete__log_old(char* pull_path_name)
{	
	system_agent_sync("rm -f %s.%s", pull_path_name, Ext_old);
}
void delete__log_chksum_old(char* pull_path_name)
{
	system_agent_sync("rm -f %s.%s", pull_path_name, Ext_old_chksum);
}
#endif

#ifdef __ALARM_LOG_BACKUP__
void delete__log_all(char* pull_path_name)
{	
	delete__log(pull_path_name);	
	delete__log_chksum(pull_path_name);
	delete__log_old(pull_path_name);
	delete__log_chksum_old(pull_path_name);
}
#endif


	
/*
#include <unistd.h> 
#include <sys/types.h> 
int ftruncate(int fd, off_t length);

쓰기로 open된 fd에 대해서 파일 크기를 length 길이 만큼의 크기로 변경
length가 파일의 크기보다 작으면 length만큼의 크기로 파일 뒤쪽을 자릅니다. 
length가 파일의 크기보다 크면 파일의 크기를 length만큼으로 변경하고 0x00으로 채웁니다.
*/

void Flush_gREMS_Pktlog_File(void)
{
	int fd;
	char pull_path_name[256];	
	
	//----------------------------------------------
#ifdef __ALARM_LOG_BACKUP__	
	pthread_mutex_lock(&Sem_gREMS_Pktlog_Alm);
#endif	
	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Alm_REMS_File); 
	fd = open(pull_path_name, O_RDWR | O_SYNC);
	if(fd < 0)
		fd = open(pull_path_name, O_CREAT | O_RDWR | O_SYNC, DEFAULT_FILE_MODE);
	if(fd > 0)
	{	
		memset(&AlmPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &AlmPkt_Log_Header);
		ftruncate(fd, sizeof(PKT_LOG_HEADER));	
		close(fd);
	}	

#ifdef __ALARM_LOG_BACKUP__	
	/*
	2019.11.30 : 
		AID(0x5454) LogCount 제어 시, 알람 카운트가 '0'이 되지만, 
		알람 발생 시  로크 카운트가 복구 되는 현상
	*/
	if(Is__File_Existing(pull_path_name, Ext_chksum) == CHK_OK) // exist
		delete__log_chksum(pull_path_name);
		
	if(Is__File_Existing(pull_path_name, Ext_old) == CHK_OK) // exist
		delete__log_old(pull_path_name);

	if(Is__File_Existing(pull_path_name, Ext_old_chksum) == CHK_OK) // exist
		delete__log_chksum_old(pull_path_name);
#endif	
#ifdef __ALARM_LOG_BACKUP__
	pthread_mutex_unlock(&Sem_gREMS_Pktlog_Alm);
#endif	
	//----------------------------------------------
	

	//----------------------------------------------
#ifdef __ALARM_LOG_BACKUP__	
	pthread_mutex_lock(&Sem_gREMS_Pktlog_Ctl);
#endif	
	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Ctl_REMS_File); 
	fd = open(pull_path_name, O_RDWR | O_SYNC);
	if(fd < 0)
		fd = open(pull_path_name, O_CREAT | O_RDWR | O_SYNC, DEFAULT_FILE_MODE);
	if(fd > 0)
	{	
		memset(&CtlPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &CtlPkt_Log_Header);
		ftruncate(fd, sizeof(PKT_LOG_HEADER));	
		close(fd);
	}
#ifdef __ALARM_LOG_BACKUP__	
	/*
	2019.11.30 : 
		AID(0x5454) LogCount 제어 시, 알람 카운트가 '0'이 되지만, 
		알람 발생 시  로크 카운트가 복구 되는 현상
	*/

	if(Is__File_Existing(pull_path_name, Ext_chksum) == CHK_OK) // exist
		delete__log_chksum(pull_path_name);
		
	if(Is__File_Existing(pull_path_name, Ext_old) == CHK_OK) // exist
		delete__log_old(pull_path_name);

	if(Is__File_Existing(pull_path_name, Ext_old_chksum) == CHK_OK) // exist
		delete__log_chksum_old(pull_path_name);
#endif
#ifdef __ALARM_LOG_BACKUP__
	pthread_mutex_unlock(&Sem_gREMS_Pktlog_Ctl);
#endif	
	//----------------------------------------------
}

void Init__PktLog(void)
{
	char pull_path_name[256];	
	int fd;
	//-----------------------------------------------
	#ifdef __ALARM_LOG_BACKUP__
	check_abnormal_log(LOG__REMS_PATH, LOG_Alm_REMS_File);
	check_abnormal_log(LOG__REMS_PATH, LOG_Ctl_REMS_File);
	#endif

	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Alm_REMS_File); 
	fd = open(pull_path_name, O_RDWR);
	
	if(fd < 0)
	{		
		fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Init PktLog] create file (%s)", pull_path_name);
		memset(&AlmPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &AlmPkt_Log_Header);	
		sync();
	}
	else
	{
		Rd_Pkt_Info_from_logfile(fd, pull_path_name, &AlmPkt_Log_Header);
	}	
	close(fd);

	//-----------------------------------------------
	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Ctl_REMS_File); 
	fd = open(pull_path_name, O_RDWR);
	
	if(fd < 0)
	{		
		fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Init PktLog] create file (%s)", pull_path_name);
		memset(&CtlPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &CtlPkt_Log_Header);	
		sync();
	}
	else
	{
		Rd_Pkt_Info_from_logfile(fd, pull_path_name, &CtlPkt_Log_Header);
	}	
	close(fd);
	//-----------------------------------------------	
}


#ifdef __ALARM_LOG_BACKUP__
/*
---------------------------------------------------------------
make chksum file "pull_path_name.chksum" for "pull_path_name"
---------------------------------------------------------------
*/
void make_log_chksum(char* pull_path_name)
{
	char pull_path_name_chksum[256];
	
	snprintf(pull_path_name_chksum, 256, "%s.%s", pull_path_name, Ext_chksum); 

	system_agent_sync("sha1sum %s > %s", pull_path_name, pull_path_name_chksum);
}

/*
---------------------------------------------------------------
make file "pull_path_name.old" for "pull_path_name"
make file "pull_path_name.chksum.old" for "pull_path_name.chksum"
---------------------------------------------------------------
*/
void copy__log_old_from_current(char* pull_path_name)
{
	system_agent_sync("cp -f %s  %s.%s", 
		pull_path_name, 
		pull_path_name, Ext_old);
	
	system_agent_sync("cp -f %s.%s  %s.%s", 
		pull_path_name, Ext_chksum, 
		pull_path_name, Ext_old_chksum);
}

int Is__File_Existing(char* pull_path_name, char* ext_name)
{	
	char pull_path[256];

	if(strlen(ext_name) > 0)
		snprintf(pull_path, 256, "%s.%s", pull_path_name, ext_name);
	else
		snprintf(pull_path, 256, "%s", pull_path_name);
	
	if(_is_file_size(pull_path) <= 0) 
		return CHK_FAIL;
	else  
		return CHK_OK;
}
#endif

#ifdef __ALARM_LOG_BACKUP__
/*
----------------------------------------------------------------------------------
name
와
name.chksum
name.chksum.tmp
를 compare 해서 결과 return
----------------------------------------------------------------------------------
*/
int Is__log_chksum_Valid(char* pull_path_name, char* ext_old)
{
	char file_name[256];
	char file_chksum[256];
	char file_chksum_tmp[256];
	int len;
	int fd;

	INT8U chksum[50];
	INT8U chksum_tmp[50];

	memset(chksum, 0, 50);
	memset(chksum_tmp, 0, 50);

	snprintf(file_chksum_tmp, 256, "%s.%s.tmp", pull_path_name, Ext_chksum); 

	if(strlen(ext_old) > 0)
	{
		snprintf(file_chksum, 256, "%s.%s.%s", pull_path_name, ext_old, Ext_chksum); 
		snprintf(file_name, 256, "%s.%s", pull_path_name, ext_old); 
	}	
	else
	{
		snprintf(file_chksum, 256, "%s.%s", pull_path_name, Ext_chksum); 
		snprintf(file_name, 256, "%s", pull_path_name); 
	}		
	//-------------------------------------------------------------
	if(Is__File_Existing(file_chksum, "") == CHK_FAIL)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Is__log_chksum_Valid] file (%s) not exist", file_chksum);
		return CHK_FAIL;
	}	
		
	fd = open(file_chksum, O_RDWR);		
	if(fd < 0)
	{		
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Is__log_chksum_Valid] file (%s) open fail", file_chksum);
		return CHK_FAIL;
	}
	lseek(fd, 0, SEEK_SET);
	len = read(fd, chksum, 40);
	close(fd);

	if(len != 40)
	{		
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Is__log_chksum_Valid] file (%s) size fail", file_chksum);
		return CHK_FAIL;
	}
	//-------------------------------------------------------------	
	system_agent_sync("sha1sum %s > %s", file_name, file_chksum_tmp);
	fd = open(file_chksum_tmp, O_RDWR);	
	if(fd < 0)
	{		
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Is__log_chksum_Valid] file (%s) open fail", file_chksum_tmp);
		return CHK_FAIL;
	}	
	lseek(fd, 0, SEEK_SET);
	len = read(fd, chksum_tmp, 40);
	close(fd);

	if(len != 40)
	{		
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Is__log_chksum_Valid] file (%s) size fail", file_chksum_tmp);
		return CHK_FAIL;
	}	
	//-------------------------------------------------------------
	if(memcmp(chksum, chksum_tmp, 40)==0)
	{
		//print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[chksum_Valid] (%s)", chksum);
		//print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[chksum_Valid] (%s)", chksum_tmp);
		return CHK_OK;
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[chksum_InValid] file (%s) chksum mismatch", file_name);
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[chksum_InValid] (%s)", chksum);
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[chksum_InValid] (%s)", chksum_tmp);
		return CHK_FAIL;
	}	
}
#endif

#ifdef __ALARM_LOG_BACKUP__

/*
---------------------------------------------------------------
log.old, log.old.chksum에서
log, log.chksum 을 restore
---------------------------------------------------------------
*/
void copy__log_from_old(char* pull_path_name)
{	
	system_agent_sync("cp -f %s.old %s", pull_path_name, pull_path_name);
	system_agent_sync("cp -f %s.%s  %s.%s", 
		pull_path_name, Ext_old_chksum, 
		pull_path_name, Ext_chksum);
}
#endif



#ifdef __ALARM_LOG_BACKUP__

void make_log_chksum_test(int idx)
{
	char pull_path_name[256];
	
	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Alm_REMS_File); 

	switch(idx)
	{	
		case -1000 : make_log_chksum(pull_path_name); break;
		
		case -999 : delete__log(pull_path_name); break;
		case -998 : delete__log_chksum(pull_path_name); break;
		
		case -997 : delete__log_old(pull_path_name); break;
		case -996 : delete__log_chksum_old(pull_path_name); break;

		case -995 : Is__log_chksum_Valid(pull_path_name, ""); break;

		case -994 : copy__log_from_old(pull_path_name); break;	
		case -993 : copy__log_old_from_current(pull_path_name); break;	
	}
}
#endif

#ifdef __ALARM_LOG_BACKUP__

/*
-----------------------------------------------------------------------
file is log(원본파일)
-----------------------------------------------------------------------
*/
int restore_from_old(char* file)
{	
	int ret;
	
	if((Is__File_Existing(file, Ext_old) == CHK_OK) && (Is__File_Existing(file, Ext_old_chksum) == CHK_OK))
	{
		print_agent(DEBUG_LOG, VT_BOLD_GREEN, "[restore_from_old] old {%s.%s} and {%s.%s} exist", 
				file, Ext_old, 
				file, Ext_old_chksum);
		
		if(Is__log_chksum_Valid(file, Ext_old) == CHK_OK)
		{
			print_agent(DEBUG_LOG, VT_BOLD_GREEN, "[restore_from_old] old {%s.%s} copy__log_from_old", 
					file, Ext_old);
					
			copy__log_from_old(file);
			ret = RET_OK;
		}
		else
		{
			// delete all
			print_agent(DEBUG_LOG, VT_BOLD_RED, "[restore_from_old] old {%s.%s} delete OLDs", 
					file, Ext_old);
			delete__log_all(file);
			ret = RET_FAIL;
		}
	}
	else
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[restore_from_old] old {%s.%s} or {%s.%s} NOT exist, delete OLDs", 
				file, Ext_old, 
				file, Ext_old_chksum);
		// delete all
		delete__log_all(file);
		ret = RET_FAIL;
	}
	return ret;
}
#endif

#ifdef __ALARM_LOG_BACKUP__
/*
return 
RET_OK  (1) : log and log.chksum are ready
RET_None(-1) : log and log.chksum are NOT ready
*/

int check_abnormal_log(char* path, char* log_file)
{
	char file[256];
	char file_old[256];
	char file_chksum[256];
	char file_old_chksum[256];
	//int ret = 0; // default 0

	snprintf(file           , 256, "%s/%s"   , path, log_file); 
	snprintf(file_old       , 256, "%s/%s.%s", path, log_file, Ext_old); 
	snprintf(file_chksum    , 256, "%s/%s.%s", path, log_file, Ext_chksum); 
	snprintf(file_old_chksum, 256, "%s/%s.%s", path, log_file, Ext_old_chksum); 

	watchdog_live();

	if(Is__File_Existing(file, "") == CHK_OK) // log exist
	{
		if(Is__File_Existing(file, Ext_chksum) == CHK_OK) // log.chksum exist ??
		{
			print_agent(DEBUG_LOG, VT_BOLD_GREEN, "[chk_abnormal] {%s.%s} exist ", file, Ext_chksum);

			if(Is__log_chksum_Valid(file, "") == CHK_OK)
			{
				print_agent(DEBUG_LOG, VT_BOLD_GREEN, "[chk_abnormal] {%s} chksum valid ", file);
				
				if(Is__File_Existing(file, Ext_old) == CHK_OK)
					print_agent(DEBUG_LOG, VT_BOLD_GREEN, "[chk_abnormal] {%s.%s} exist ", file, Ext_old);
				else
					print_agent(DEBUG_LOG, VT_BOLD_RED, "[chk_abnormal] {%s.%s} NOT exist ", file, Ext_old);

				if(Is__File_Existing(file, Ext_old_chksum) == CHK_OK)
					print_agent(DEBUG_LOG, VT_BOLD_GREEN, "[chk_abnormal] {%s.%s} exist ", file, Ext_old_chksum);	
				else
					print_agent(DEBUG_LOG, VT_BOLD_RED, "[chk_abnormal] {%s.%s} NOT exist ", file, Ext_old_chksum);	
								
				copy__log_old_from_current(file);
				return RET_OK; // RET_OK : log and log.chksum, log.old and log.old.chksum all ready
			}
			else // log.chksum is invalid, log is crashed
			{
				print_agent(DEBUG_LOG, VT_BOLD_RED, "[chk_abnormal] chksum {%s} Invalid, restore_from_old", file);
				restore_from_old(file);
				return RET_None;
			}
		}
		else
		{
			print_agent(DEBUG_LOG, VT_BOLD_RED, "[chk_abnormal] {%s.%s} Not exist ", file, Ext_chksum);
			make_log_chksum(file);
			copy__log_old_from_current(file);
			return RET_OK; // RET_OK : log and log.chksum are ready
		}		
	}
	else // log NOT exist
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[chk_abnormal] Log {%s} NOT Exist ", file);			      
		restore_from_old(file);
		return RET_None;
	}
}
#endif

/*
--------------------------------------------------------------------
1. when RCU init, get current-header from file.
2. If Alarm changed, write packet, if success update current-header and write to file.
3. If body size is over, create new file.

body pakcet
protocol max is 100
file max is 256
if over 256, create new file.
--------------------------------------------------------------------
*/



//===============================================================================================

int Wr_Alm_Pkt_to_log( PROTOCOL_FRAME* txf )
{
	static INT8U init = 0;	
	int len;
	int len_date, len__etc, len__stx, offset;
	char pull_path_name[256];
	INT16U sub_data_len;
	INT8U redundancy;
	int fd;
	int idx;
	int re;

	if(m_Eep.Log_OnOff == OFF) // 2019.09.19
		return 0;

	sub_data_len = htons(txf->len);
	if( CMD_LOG_Max < sub_data_len ) 
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[Wr AlmPkt_to_log] CMD_LOG_Max < txf->len ");
		return -EINVAL;
	}
	if( Is_InValid__SysTime() ) 
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[Wr AlmPkt_to_log] Is_InValid__SysTime");
		return -EINVAL;
	}
	
#ifdef __ALARM_LOG_BACKUP__
	pthread_mutex_lock(&Sem_gREMS_Pktlog_Alm);
#endif		

	len_date = 6;	
	len__stx = 4; // 0x16161616
	len__etc = 14 + sub_data_len + 3; // header(14) + data + crc(2) + ETX(1)	
	
	len = len_date + len__stx + len__etc; // d_len(6) + stx(4) + len_except_stx
	redundancy = (16-len%16);

	memset(cmd_log_buf, 0, len+redundancy);

	offset = 0;
	memcpy(&cmd_log_buf[offset], m_St.Ru_Sys_Date, len_date); offset += len_date;
	memcpy(&cmd_log_buf[offset], stx, len__stx);              offset += len__stx;
	memcpy(&cmd_log_buf[offset], (INT8U*)txf, len__etc);      offset += len__etc;
	len += redundancy;

	// sprintf 처럼 쓰되 최대 256, 끝은 null 보장.
	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Alm_REMS_File); 

#ifdef __ALARM_LOG_BACKUP__
	/*-----------------------------------------------
	2019.11.14 : After alarm-update, update 'log.chksum'
	-----------------------------------------------*/		
	re = check_abnormal_log(LOG__REMS_PATH, LOG_Alm_REMS_File);
#endif

	fd = open(pull_path_name, O_RDWR);
	if(fd < 0)
	{		
		fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Wr AlmPkt_to_log] create file (%s)", pull_path_name);
		memset(&AlmPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &AlmPkt_Log_Header);	
		sync();
	}

	if(init == 0)
	{
		init = 1;
		Rd_Pkt_Info_from_logfile(fd, pull_path_name, &AlmPkt_Log_Header);
	}
#ifdef __ALARM_LOG_BACKUP__	
	if(re == RET_None)	
		Rd_Pkt_Info_from_logfile(fd, pull_path_name, &AlmPkt_Log_Header); // read 만 하고, fd 자체는 변하지 않았음.
#endif

	idx = AlmPkt_Log_Header.pkt_next;

	if(idx >= PKT_LOG_MAX_RANGE)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Wr AlmPkt_to_log] abnormal file, delete (%s)", pull_path_name);
		memset(&AlmPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &AlmPkt_Log_Header);			
		ftruncate(fd, sizeof(PKT_LOG_HEADER));
		sync();
		idx = AlmPkt_Log_Header.pkt_next;
	}
	
	if(idx >= PKT_LOG_MOV_MAX) // last index
	{		
		if(Move_PktLog_to_NewFile(fd, pull_path_name, &AlmPkt_Log_Header) == RET_SUCCESS)
		{
			close(fd);
			system_agent("rm -rf %s\n", pull_path_name);
			sync(); // 2020.01.23
			
			system_agent("mv %s %s\n", Temp_Alm_Pkt_Log_File, pull_path_name);
			sync(); // 2020.01.23
			
			fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
			idx = AlmPkt_Log_Header.pkt_next;
		}
		////else {}
	}	
	
	if(fd > 0) 
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Wr AlmPkt_to_log] idx to write %d", idx);	
		
		if(Append_Pkt_to_logfile(fd, pull_path_name, cmd_log_buf, len) == RET_SUCCESS)
		{
			AlmPkt_Log_Header.pkt_size[idx] = len;
			AlmPkt_Log_Header.pkt_next = ++idx;			
			// update header
			Wr_Pkt_Info_to_logfile(fd, pull_path_name, &AlmPkt_Log_Header);
		}
		// else : do nothing
		
		close(fd);
	}

#ifdef __ALARM_LOG_BACKUP__
	/*-----------------------------------------------
	2019.11.14 : After alarm-update, update 'log.chksum'
	-----------------------------------------------*/
	make_log_chksum(pull_path_name);
	//-----------------------------------------------
#endif

#ifdef __ALARM_LOG_BACKUP__
	pthread_mutex_unlock(&Sem_gREMS_Pktlog_Alm);
#endif
	return 0;
}




#define WR_CMD_LOG_Max (2000)
static INT8U wr_cmd_log_buf[WR_CMD_LOG_Max+48];

int Wr_Ctl_Pkt_to_log( PROTOCOL_FRAME* rxf )
{
	static INT8U init = 0;	
	int len;
	int len_date, len__etc, len__stx, offset;
	char pull_path_name[256];
	INT16U sub_data_len;
	INT8U redundancy;
	int fd;
	int idx;
	INT8U swap;
	int re;

	if(m_Eep.Log_OnOff == OFF) // 2019.09.19
		return 0;

	sub_data_len = rxf->len;
	if( WR_CMD_LOG_Max < sub_data_len ) 
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[Wr CtlPkt_to_log] WR_CMD_LOG_Max < txf->len ");
		return -EINVAL;
	}
	if( Is_InValid__SysTime() ) 
	{
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[Wr CtlPkt_to_log] Is_InValid__SysTime");
		return -EINVAL;
	}

	len_date = 6;	
	len__stx = 4;
	len__etc = 14 + sub_data_len + 3; // header(14) + data + crc(2) + ETX(1)	
	
	len = len_date + len__stx + len__etc; // d_len(6) + stx(4) + len_except_stx
	redundancy = (16-len%16);

	memset(wr_cmd_log_buf, 0, len+redundancy);

	offset = 0;
	memcpy(&wr_cmd_log_buf[offset], m_St.Ru_Sys_Date, len_date); offset += len_date;
	memcpy(&wr_cmd_log_buf[offset], stx, len__stx);              offset += len__stx;
	memcpy(&wr_cmd_log_buf[offset], (INT8U*)rxf, (len__etc-3));  offset += (len__etc-3);

	wr_cmd_log_buf[offset++] = rxf->crc[0];
	wr_cmd_log_buf[offset++] = rxf->crc[1];
	wr_cmd_log_buf[offset++] = 0x03; // ETX
	len += redundancy;

	//-------------------------------------------
	// swap : length, seq num...
	//-------------------------------------------
	offset = len_date;
	swap = wr_cmd_log_buf[offset+4];	
	wr_cmd_log_buf[offset+4] = wr_cmd_log_buf[offset+5];  
	wr_cmd_log_buf[offset+5] = swap;

	swap = wr_cmd_log_buf[offset+8];	
	wr_cmd_log_buf[offset+8] = wr_cmd_log_buf[offset+9];  
	wr_cmd_log_buf[offset+9] = swap;
	
	swap = wr_cmd_log_buf[offset+16]; 
	wr_cmd_log_buf[offset+16] = wr_cmd_log_buf[offset+17]; 
	wr_cmd_log_buf[offset+17] = swap;
	////////////////////

	// sprintf 처럼 쓰되 최대 256, 끝은 null 보장.
	snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Ctl_REMS_File); 
#ifdef __ALARM_LOG_BACKUP__
	/*-----------------------------------------------
	2019.11.14 : After alarm-update, update 'log.chksum'
	-----------------------------------------------*/		
	re = check_abnormal_log(LOG__REMS_PATH, LOG_Ctl_REMS_File);
#endif

	fd = open(pull_path_name, O_RDWR);
	if(fd < 0)
	{		
		fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Wr CtlPkt_to_log] create file (%s)", pull_path_name);
		memset(&CtlPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &CtlPkt_Log_Header);	
		sync();
	}
	
	if(init == 0)
	{
		init = 1;
		Rd_Pkt_Info_from_logfile(fd, pull_path_name, &CtlPkt_Log_Header);
	}
#ifdef __ALARM_LOG_BACKUP__	
	if(re == RET_None)
		Rd_Pkt_Info_from_logfile(fd, pull_path_name, &CtlPkt_Log_Header);
#endif

	idx = CtlPkt_Log_Header.pkt_next;

	if(idx >= PKT_LOG_MAX_RANGE)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Wr CtlPkt_to_log] abnormal file, delete (%s)", pull_path_name);
		memset(&CtlPkt_Log_Header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, &CtlPkt_Log_Header);			
		ftruncate(fd, sizeof(PKT_LOG_HEADER));
		sync();
		idx = CtlPkt_Log_Header.pkt_next;
	}

	if(idx >= PKT_LOG_MOV_MAX) // last index
	{		
		if(Move_PktLog_to_NewFile(fd, pull_path_name, &CtlPkt_Log_Header) == RET_SUCCESS)
		{
			close(fd);
			system_agent("rm -rf %s\n", pull_path_name);
			system_agent("mv %s %s\n", Temp_Alm_Pkt_Log_File, pull_path_name);
			fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
			idx = CtlPkt_Log_Header.pkt_next;
		}
	}
	
	if(fd > 0) 
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[Wr CtlPkt_to_log] idx to write %d", idx);	

		if(Append_Pkt_to_logfile(fd, pull_path_name, wr_cmd_log_buf, len) == RET_SUCCESS)
		{
			CtlPkt_Log_Header.pkt_size[idx] = len;
			CtlPkt_Log_Header.pkt_next = ++idx;			
			// update header
			Wr_Pkt_Info_to_logfile(fd, pull_path_name, &CtlPkt_Log_Header);
		}
		close(fd);
	}

#ifdef __ALARM_LOG_BACKUP__
	/*-----------------------------------------------
	2019.11.14 : After alarm-update, update 'log.chksum'
	-----------------------------------------------*/
	make_log_chksum(pull_path_name);
	//-----------------------------------------------
#endif	
	return 0;
}

void Update_Log_Alm_Ctl_Cnt(void)
{
	INT8U data[10]; 
	INT16U cnt_alm;
	INT16U cnt_ctl;
	INT16U tot_ctl;	
	static INT8U cnt = 0;

	// every 1.5[s]
	//------------------
	cnt++;
	if(cnt < 3) return;
	cnt = 0;
	//------------------
	
	cnt_alm = Get_Pkt_from_log(LOG_ALM, data, 0xFF);
	cnt_ctl = Get_Pkt_from_log(LOG_CTL, data, 0xFF);

	tot_ctl = cnt_alm + cnt_ctl;
	if(tot_ctl >= 256) tot_ctl = 255;

	m_St.Log_Alm_Ctl_Cnt = (INT8U)tot_ctl;	
}

/*
----------------------------------------------------------------------
2019.11.30
log Read 때에는 
Sem_gREMS_Pktlog_Alm, Sem_gREMS_Pktlog_Ctl
를 사용하지 않아도 될듯.
----------------------------------------------------------------------
*/
int Get_Pkt_from_log(INT8U log_idx, INT8U* pPkt, int i)
{	
	int len, idx;
	char pull_path_name[256];	
	int fd;
	int max_cnt;
	INT8U ret_req;
	PKT_LOG_HEADER* pPktLog_header;
	int num;


	switch(log_idx)
	{
		case LOG_ALM:
			pPktLog_header = &AlmPkt_Log_Header;
			num = AlmPkt_Log_Header.pkt_next;
			snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Alm_REMS_File);	
			break;
			
		case LOG_CTL:
			pPktLog_header = &CtlPkt_Log_Header;
			num = CtlPkt_Log_Header.pkt_next;
			snprintf(pull_path_name, 256, "%s/%s", LOG__REMS_PATH, LOG_Ctl_REMS_File);	
			break;

		default:
			return 0;
	}

	if(i == 0xFF)
	{
		if(num > PKT_LOG_MAX_CNT)
		num = PKT_LOG_MAX_CNT;

		return num;
	}
	//-------------------------------------------
	fd = open(pull_path_name, O_RDONLY); // 2019.11.30 : 'O_RDWR' --> 'O_RDONLY'
	if(fd < 0)
	{
		// file 없을때는 return만 하고, 아래처럼 생성하지는 않아도 될듯.
		fd = open(pull_path_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Get_Pkt_from_log] create file (%s)", pull_path_name);
		memset(pPktLog_header, 0, sizeof(PKT_LOG_HEADER));
		Wr_Pkt_Info_to_logfile(fd, pull_path_name, pPktLog_header);	
		sync();
		
		return 0;
	}	
	else
	{
		ret_req = 0;
		
		if((i < 1)||(i > PKT_LOG_MAX_CNT))
			ret_req = 1;
		
		if(pPktLog_header->pkt_next == 0) // alarm not exist
			ret_req = 1;
		
		if(pPktLog_header->pkt_next < PKT_LOG_MAX_CNT)
			max_cnt = pPktLog_header->pkt_next;
		else
			max_cnt = PKT_LOG_MAX_CNT;

		if(i > max_cnt)	ret_req = 1;			
	}
	if(ret_req)	
	{
		close(fd);	
		return 0;
	}

	
	if(pPktLog_header->pkt_next < PKT_LOG_MAX_CNT)
		idx = max_cnt - i;
	else
		idx = pPktLog_header->pkt_next - i;	
	
	len = Rd_Pkt_from_logfile(fd, idx, pPkt, 2048, pPktLog_header);
	close(fd);
	
	return len;
}

/*
===============================================================================================
End of Command 0xF4 :

	Control log from gREMS 
	Alarm log to gREMS
===============================================================================================
*/




/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

typedef struct __attribute__((aligned(1),packed))
{
	INT8U   stx[4];
	//---------------
#ifdef __OLD_AID__
	INT8U	seqNo16; // h_tryno;
#else // New AID @ 5G
	INT16U	seqNo16;
	INT8U	totPkt;
	INT8U	cur_Pkt;
#endif
    INT16U	h_len;
    INT8U	h_csum;
    //---------------
    INT8U	SUB_ID[3];
    INT8U	Type;
    INT8U	Rcode;
    INT16U	len;
    INT8U	data[MAX_COM_BUFFER];
    //---------------
} ALM_LOG_PROTOCOL_FRAME;

INT16U Make_Alm_log_Frame( ALM_LOG_PROTOCOL_FRAME* pframe, INT16U len, INT16U seqNo16, INT8U totPkt, INT8U curPkt )
{
	INT8U* pcrc2;

	memset( pframe->stx, 0x16, 4);

	//------------------------------------
	#ifdef __OLD_AID__
	//------------------------------------
	pframe->seqNo16 = 0x30; // TEST cnt++;
	pframe->h_len   = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size
	pframe->h_csum  = 0x30 + ((INT8U)(pframe->h_len>>8)) + ((INT8U)pframe->h_len);
	//------------------------------------
	#else
	//------------------------------------		
	pframe->totPkt	= totPkt; // 2020.03.20 : add
	pframe->cur_Pkt = curPkt; // 2020.03.20 : add	
	
	pframe->seqNo16 = htons(seqNo16);
	pframe->h_len  = len + MAX_BODY_HEADER_SIZE+MAX_CRC_SIZE; // Body Size		
	pframe->h_csum = (INT8U)(pframe->seqNo16>>8) + (INT8U)pframe->seqNo16;
	pframe->h_csum += pframe->totPkt;
	pframe->h_csum += pframe->cur_Pkt;	
	pframe->h_csum += (INT8U)(pframe->h_len>>8);
	pframe->h_csum += (INT8U)pframe->h_len;		
	//------------------------------------	
	#endif
	//------------------------------------

	pframe->Type = 0x00; // type
	pframe->Rcode = _RCODE_;
	pframe->len = len;

	memcpy( pframe->SUB_ID, m_Eep.MY_SUBID, 3);

	pcrc2 = &pframe->data[len]; // crc 계산 후, 값을 저장할 CRC 위치의 포인터.

	pframe->data[len+2] = 0x03; // ETX 넣고..

	pframe->h_len = Swap16(pframe->h_len);
	pframe->len = Swap16(pframe->len);

	INT16B_to_BYTE( pcrc2, crcNByteCalc( 0, &pframe->SUB_ID[0], len+MAX_BODY_HEADER_SIZE ) );

	return (MAX_NMS_SYNC_SIZE + MAX_BODY_HEADER_SIZE + MAX_HEADER_SIZE + len + MAX_CRC_SIZE+MAX_TAIL_SIZE );
}

//--------------------------------------------------

static ALM_LOG_PROTOCOL_FRAME alm_log_frame;

INT16U Make_Current_Status_Pkt(void)
{
	INT16U len = 0;
	static INT16U seqNo = 0;
	INT8U totPkt, curPkt;
	
	//#ifdef _TEST____ALM_LOG_
	//memcpy( &alm_log_frame.data[0],  RU_ARM_SUPP, RU_ALARM_CNT ); len = RU_ALARM_CNT;		
	//memcpy( &alm_log_frame.data[len], &m_St.Last_Alm, RU_ALARM_CNT ); len += RU_ALARM_CNT;	
	//#else
	len = GetStatus(alm_log_frame.data, COMM_TERM);
	//#endif
	
	len += Make_Tree_Info( &alm_log_frame.data[len], 1 ); // make self-RU status

	totPkt = 1;
	curPkt = 1;
	return Make_Alm_log_Frame( &alm_log_frame, len, seqNo++, totPkt, curPkt);
}

//--------------------------------------------------



/*
int
main()
{
	int ch;
	int i;
	int cnt;
	char name[128];
	unsigned char buf[4096*3];

	for (i=0; ; i++)
	{
		printf("   Check [Y/n] : ");
		fflush(stdout);
		ch = getchar();
		if (ch == 'n' || ch == 'N')
			break;
	}
	return 0;
}
*/

/*
해제 free(ptr);
a = (struct A *)malloc(sizeof(struct A));
*/








static ALM_LOG_INFO alm_log;
static ALM_LOG_BUF alm_log_buf;
static ALM_LOG_BUF log_tmp;


//------------------------------------------------------
void Init_Alm_Log_Info(void)
{
	alm_log.logging_st = LOGGING_OFF;	
	alm_log.chunk_size = 0;	
	alm_log.immediate_start = 0;
	alm_log.cnt_file_gen = 0;

	alm_log_buf.en = 0;
	alm_log_buf.cur_idx = 0;
	alm_log_buf.ptr = NULL;	

	log_tmp.en = 0;
	log_tmp.cur_idx = 0;
	log_tmp.ptr = NULL;
}
//------------------------------------------------------
void Calc_Chunk_Size(void)
{
	int max_size = CHUNK_UNIT*22; // from 5632 byte(5396) @ 5G
	INT16U pkt_len;
	
	if( alm_log.chunk_size == 0 )
	{
		pkt_len = Make_Current_Status_Pkt();
		
		do{
			if( pkt_len > max_size ) 
			{
				max_size += CHUNK_UNIT;
			}
		}while(pkt_len > max_size);		
		
		alm_log.chunk_size = max_size + CHUNK_UNIT; // plus CHUNK_UNIT for packet-header-size

		print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] size (pkt : %d, chunk : %d)", pkt_len, alm_log.chunk_size);
	}
}



//------------------------------------------------------

static INT8U pre_alm[RU_ALARM_CNT];
void User_alarm_log_on(void) // v
{	
	print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] alm-log-ON-Request.");

	memcpy( pre_alm, &m_St.Last_Alm, RU_ALARM_CNT); // init
	
	MemAlloc_Alm_Log();
	
	if(m_Eep.Log_OnOff != m_St.Log_Prev)
	{
		system_agent_sync("rm -r %s/*", LOG_ALARM_PATH);
		
		m_St.Log_Cnt = _get_log_file_count(LOG_ALARM_PATH);		
		
		/*
		//re = chdir(LOG_ALARM_PATH);
		//if (re == 0)
		//{		
		//	snprintf(cmd_buf, 50, "rm -r *");
		//	re = system(cmd_buf);
		//	system( "sync" );
		//}
		*/
	}

	// alarm 상태에서 log_off --> log_on 되면, 
	//
	if(alm_log_buf.en && (alm_log.logging_st == LOGGING_OFF))
	{
		alm_log.logging_st = LOGGING_1;
		alm_log_buf.cur_idx = 0;
		
		if(m_St.Sum_Alarm)
			alm_log.immediate_start = 1;
		else
			alm_log.immediate_start = 0;

		print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] alm-log on (alm %d).", m_St.Sum_Alarm);
	}
	alm_log.cnt_file_gen = 0;
}
//------------------------------------------------------
void User_alarm_log_off(void)
{
	Free_Alm_Log_Buf();
}
//------------------------------------------------------

//------------------------------------------------------
void Free_Alm_Log_Buf(void) // v
{
	if(alm_log_buf.ptr != NULL) 
	{
		free(alm_log_buf.ptr);
		alm_log_buf.ptr = NULL; // must
		print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] free mem-alm-log.");
	}	
	alm_log_buf.en = 0;

	/*
	if(log_tmp.ptr != NULL) 
	{
		free(log_tmp.ptr);
		log_tmp.ptr = NULL; // must
		print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] free mem-tmp-log.\r\n");
	}
	log_tmp.en = 0;
	*/
	
	alm_log.logging_st = LOGGING_OFF;
	alm_log.cnt_file_gen = 0;
}
//------------------------------------------------------
void MemAlloc_Alm_Log(void) // v
{
	int size;
	
	if(alm_log.chunk_size == 0) 
	{
		alm_log_buf.en = 0;
		log_tmp.en = 0;
		print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] mem-alloc not init, ret.");
		return;
	}
	
	if(alm_log_buf.ptr == NULL)
	{
		size = alm_log.chunk_size * CHUNK_MAX_Cnt;
		
		alm_log_buf.ptr = (INT8U*)malloc(size);
		
		if(alm_log_buf.ptr != NULL)
		{
			alm_log_buf.en = 1;
			memset(alm_log_buf.ptr, 0, size);
			print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] mem-alloc init %d, %d", 
					alm_log.chunk_size, size);
		}
		else print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] mem-alloc null.");
	}	

	/*
	if(log_tmp.ptr == NULL)
	{
		size = alm_log.chunk_size * CHUNK_MAX_Tmp_Cnt;
		
		log_tmp.ptr = (INT8U*)malloc(size);
		
		if(log_tmp.ptr != NULL)
		{
			log_tmp.en = 1;
			memset( log_tmp.ptr, 0, size);
			print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] mem-alloc init %d.", size);
		}		
		else print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] mem-alloc null.");
	}
	*/
}
//------------------------------------------------------
void Buf_Wr_Current_Status(int idx) // v
{
	INT16U len;
	int buf_idx;
	int chunk;

	chunk = alm_log.chunk_size;

	if(idx < CHUNK_MAX_Cnt)
	{
		len = Make_Current_Status_Pkt();

		buf_idx = idx * chunk;

		memset( &alm_log_buf.ptr[buf_idx], 0, chunk );
		memcpy( &alm_log_buf.ptr[buf_idx], (unsigned char *)&alm_log_frame, len );

		//if(idx % 20 == 0)
		//{
		//	D_Printf( DEBUG_SYSTEM,  "[LOG %d] cnt %d (buf_idx %d)\r\n", alm_log.logging_st, idx, buf_idx);
		//}
		
		//if(idx == (CHUNK_MAX_Cnt-1))
		//	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[LOG] st(%d), Max Roll-Back.", alm_log.logging_st);		
	}
}
//------------------------------------------------------

static char alm_log_file_name[50];

//------------------------------------------------------



/*
알람 Log는 알람 발생시 발생 전 2.5분, 발생 후 2.5분 총 5분 동안의 Log가 저장됨
*/
	

static INT8U cur_Alm[RU_ALARM_CNT];
enum
{
	_ALM_BKUP_REQ = 0,
	_ALM_CHK__REQ,
};
void Chk_Alm_generating( INT8U check_req ) // v
{
	INT8U i;
	INT8U log_gen = 0; // init
	ALM_ST* pAlmSt;

	memcpy( cur_Alm, &m_St.Last_Alm, RU_ALARM_CNT);

	pAlmSt = (ALM_ST*)cur_Alm;
	pAlmSt->A_0.GUI = 0;
	//--------------------------------

	if(check_req == _ALM_BKUP_REQ)
	{
		memcpy( pre_alm, cur_Alm, RU_ALARM_CNT);
	}
	else // check request
	{
		if(alm_log.immediate_start)
		{
			log_gen = 1;			
		}
		else
		{
			for(log_gen=0, i=0; i < RU_ALARM_CNT; i++)
			{
				if(pre_alm[i] ^ cur_Alm[i]) // if alarm changed..
				{
					log_gen = 1;
					print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] alarm byte[%d] 0x%02X --> 0x%02X changed", 
							i, pre_alm[i], cur_Alm[i] );
				}	
				if(log_gen) break;
			}
		}

		if(log_gen)
		{
			if(alm_log.logging_st == LOGGING_1) 
			{
				print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] Logging after alarm-change");
				alm_log.logging_st = LOGGING_2;
			}
		}		
		memcpy( pre_alm, cur_Alm, RU_ALARM_CNT);
	}
}

//------------------------------------------------------





// alarm log on 상태에서, 연속 아래 횟수 이면, log off 시킴.
#define MAX_FILE_GEN_NUM (100)

void Proc_alarm_log(void)
{
	int idx_start, idx__stop;
	int cur_idx;
	int re;
	int offset;
	INT8U complete;
	int chunk_max_cnt;	
	char pull_path_name[256];
	
	
	

	cur_idx = alm_log_buf.cur_idx;

	if(cur_idx >= CHUNK_MAX_Cnt)
		cur_idx = 0;
		
		
	switch(alm_log.logging_st)
	{	
		/*
		* When user alarm-log-on : logging_off -> logging_1
		* Waiting until alarm-change
		* If alarm-change, goto logging_2
		*/		
		case LOGGING_1:	
			Buf_Wr_Current_Status(cur_idx);
			Chk_Alm_generating(_ALM_CHK__REQ);
			break;
		
		case LOGGING_2:
			Buf_Wr_Current_Status(cur_idx);
			/*
			* calculate start_index from cur_idx
			* calculate end_index from start_index
			* 없는 것은 0으로 채워져 있음.
			*/
			offset = CHUNK_Half_Cnt; // start idx is 599
			idx_start = cur_idx - offset;
			idx__stop = idx_start - 1;
			
			if(idx_start < 0) idx_start += CHUNK_MAX_Cnt;
			if(idx__stop < 0) idx__stop += CHUNK_MAX_Cnt;
			//---------------------------------
			alm_log_buf.start_idx = idx_start;
			alm_log_buf.end_idx = idx__stop;
			alm_log.immediate_start = 0;
			//---------------------------------
			alm_log.logging_st = LOGGING_3;			
			Make_Log_File_Name( _Log_Alm, alm_log_file_name, alm_log.chunk_size );
			
			print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] ring idx : start(%d) curr(%d) end(%d)",
				idx_start, cur_idx, idx__stop);
			break;

		/*
		* if logging is completed, write to eMMC/NAND flash
		*/
		case LOGGING_3:
			Buf_Wr_Current_Status(cur_idx);
			Chk_Alm_generating(_ALM_BKUP_REQ);	

			if(cur_idx == alm_log_buf.end_idx) 
				 complete = 1; // if logging completed			
			else complete = 0;			

			#ifdef __TYPE__PRU__ // 2019.08.29 : when AC Fail
			if(m_St.AlmSt.A_0.AC) complete = 1; // logging completed by AC-Fail
			#endif
			
			if(complete) // if logging completed
			{	
				chunk_max_cnt = CHUNK_MAX_Cnt;
			#ifdef __TYPE__PRU__ // 2019.08.29 : when AC Fail
				if(m_St.AlmSt.A_0.AC)
				{
					print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] AC-Fail, Immediately Write to File");
					chunk_max_cnt = CHUNK_MAX_Cnt/2;
				}			
			#endif			
				re = NAND_Wr_Alarm_log( &alm_log_buf, chunk_max_cnt, alm_log.chunk_size, alm_log_file_name );
				if(re >= 0) // success
				{
					print_agent(DEBUG_LOG, VT_BOLD_CYAN, "[LOG] write to File (idx %d)", cur_idx);
					m_St.Log_Cnt = _get_log_file_count(LOG_ALARM_PATH);
					alm_log.cnt_file_gen++;

					snprintf(pull_path_name, 256, "%s/%s", LOG_ALARM_PATH, alm_log_file_name); 
					
					system_agent_sync("tar -zcvf %s.tgz %s", pull_path_name, pull_path_name);
					system_agent_sync("rm -rf %s", pull_path_name);					
				}
				else
				{
					print_agent(DEBUG_LOG, VT_BOLD_RED, "[LOG] fail to write to File (idx %d)", cur_idx);
				}
				
				if( m_St.Log_Cnt >= MAX_LOG_ALARM_FILES)
					m_St.Log_Cnt = MAX_LOG_ALARM_FILES;
				
				alm_log.logging_st = LOGGING_1;
			}
			break;

		case LOGGING_OFF:
			alm_log.cnt_file_gen = 0;
			break;
			
		default: 
			alm_log.logging_st = LOGGING_OFF; 
			break;
	}

	if(alm_log.logging_st > LOGGING_OFF)
	{
		cur_idx++;
		alm_log_buf.cur_idx = cur_idx;		
	}
	/*
	LOGGING_OFF -> LOGGING_1 되면,
	alm_log_buf.cur_idx = 0 됨.
	*/

	if(m_Eep.Log_OnOff == ON)
	{
		if(alm_log.cnt_file_gen >= MAX_FILE_GEN_NUM )
		{	
			m_St.Log_Prev = m_Eep.Log_OnOff;
			m_Eep.Log_OnOff = OFF;
			User_alarm_log_off();
			m_Env.is_eep_write = EEP_WRITE_CODE;
		}
	}
}


int Remove_all_AlmLog_Files(void)
{
	int cnt;

	// 2019.09.10 : remove gREMS alarm-log file, ctrl-log-file too.
	//----------------------------------------
	Flush_gREMS_Pktlog_File();	
	//----------------------------------------
	
	m_St.Log_Cnt = _get_log_file_count(LOG_ALARM_PATH); 
	
	system_agent_sync("rm -f %s/*", LOG_ALARM_PATH);

	cnt = _get_log_file_count(LOG_ALARM_PATH);	
	
	if(cnt > MAX_LOG_ALARM_FILES) 
		cnt = MAX_LOG_ALARM_FILES;
		
	return cnt;
}

void Check__Alarmlog_Directory(void)
{
	struct stat sb;
		
	if(stat(Sym_Link__PATH, &sb ) < 0)
		system_agent_sync("mkdir %s", Sym_Link__PATH);
	
	if(stat(LOG_ALARM_PATH, &sb ) < 0)
		system_agent_sync("mkdir %s", LOG_ALARM_PATH);

	if(stat(LOG__CTRL_PATH, &sb ) < 0)
		system_agent_sync("mkdir %s", LOG__CTRL_PATH);

	if(stat(LOG__REMS_PATH, &sb ) < 0)
		system_agent_sync("mkdir %s", LOG__REMS_PATH);
}


/*
-----------------------------------------------------------------------------------
1010.02.12 : Get Log File (alarm 발생시, alarm 발생 전후 5분의 GUI 상태응답 packet)
-----------------------------------------------------------------------------------
*/
typedef struct
{
	char file_name[MAX_LOG_ALARM_FILES][128];
	int  file_co;
}LOG_FILE_INFO;
static LOG_FILE_INFO m_LogFile_Info;

int get__list_of_5m_log_file(void)
{
	int n;	
	struct dirent **pp_namelist = NULL;
	
	n = scandir(LOG_ALARM_PATH, &pp_namelist, _is_5m_log_file_name, alphasort);

////print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[get__list_of_5m_log_file] count is %d", n); 
	
	if (n < 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[get__list_of_5m_log_file] count is %d", n); 
		print_agent(DEBUG_LOG, VT_BOLD_RED, "[get_log_file_cnt] error %d", _get_error_code());
		m_LogFile_Info.file_co = 0;
	}
	else
	{
		if(n > MAX_LOG_ALARM_FILES)
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[get__list_of_5m_log_file] count is %d", n); 
			print_agent(DEBUG_LOG, VT_BOLD_RED, "log file count : %d > %d", n, MAX_LOG_ALARM_FILES);
			n = MAX_LOG_ALARM_FILES;
		}
		m_LogFile_Info.file_co = n;
		
		while (n--) 
		{
			strcpy(m_LogFile_Info.file_name[n], pp_namelist[n]->d_name);			
		////print_agent(DEBUG_LOG, VT_BOLD_GREEN, "%03d : %s", n, m_LogFile_Info.file_name[n]);
			
			free(pp_namelist[n]);
		}
		
		if(pp_namelist)
			free(pp_namelist);			
	}
	
	return m_LogFile_Info.file_co;
}

char* get__name_of_5m_log_file( int n )
{
	if(n < m_LogFile_Info.file_co)
		return m_LogFile_Info.file_name[n];
	else
		return '\0';
}
//------------------------------------------------------------------------------------------------
typedef struct{
	char file_name[128];
	INT8U en;
	INT8U port;
}SEND_SZ_INFO;

static SEND_SZ_INFO m_Send_sz_Info;
//--------------------------------------
INT8U Set__send_5m_log_file_via_sz(char* file_name, INT8U port)
{
#ifdef __TYPE__PRU__ // PRU
	if(port != COMM_TERM)
	{
		m_Send_sz_Info.en = 0;
		return 0;
	}
#else // AAU
	if((port != COMM_TERM)&&(port != COMM_BT))
	{
		m_Send_sz_Info.en = 0;
		return 0;
	}	
#endif
	m_Send_sz_Info.port = port;		
	strcpy(m_Send_sz_Info.file_name, file_name);

	if(strlen(file_name) == 0)
		return 0;

	m_Send_sz_Info.en = 1;
	
	return 1;
}

static APP_TIMER zModem_Timer;

INT8U Is_Existing_sz(void)
{
	int re = system("pidof sz");			
    /*
    re lower 1byte 
    	if 0 : system command success
    	else : system command fail
    	
    re higer 1byte 
    	exit code of command    	
    */

	if((re & 0xff00) == 0) // 'pidof sz' is exist
		return 1; // exist
	else return 0; // not-exist	
}

/*
----------------------------------------------------------------
- console은 console대로 실행.
- console 아닌 장비의 serial port (AAU는 BT 도 있음) 와 기존 PC GUI 연동 중,
PC GUI 를 shut-down 하고, sz전용GUI를 연결해서 sz 전송하기 위한 것.

busybox 에서는 아래처럼 해야 함.
"timeout -t 10 -s KILL /usr/bin/sz"
ubuntu 같은 경우는 "timeout -s SIGKILL 10s command"
----------------------------------------------------------------
*/

#define __USE__sz_with_timeout__

#ifdef __USE__sz_with_timeout__
#define SZ_CMD "timeout -t 30 -s KILL /usr/bin/sz"
#else
#define SZ_CMD "/usr/bin/sz"
#endif

// 서버(PRU/AAU) <----> PC
// sz : zmodem 사용하는 명령어, 서버(PRU/AAU) 입장에서 sz(send zmodem), rz(recieve zmodem).

void Proc__send_5m_log_file_via_sz(void)
{
	static INT8U init = 0;
	char cmd[256];

	if(init == 0)
	{
		init = 1;
		m_Send_sz_Info.en = 0;
		Init__App_Timer(&zModem_Timer);
	}	

	if(m_Send_sz_Info.en) 
	{
		/*-------------------------------------------------
		GUI 실행 중인, PC serial port로 sz로 전송.
		sz_실행_GUI가 serial port(평소 serial GUI가 실행되는 port)에 연결됨.
		--------
		PC(console port가 아닌 serial port)에서 zmodem receive 상태가 된 후에, 
		아래 send 가 실행되어야 함.
		------------------------------
		$ killall -9 sz
		$ sz file_name 1> /dev/ttyPS1 0< /dev/ttyPS1
		-------------------------------------------------*/

		// pidof check 가 필요함. 이후에 killall 여부.
		if(Is_Existing_sz())
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "trying to kill sz ----");
			system_agent("killall -9 sz; usleep 10000;"); // console에 실행.
			OSTimeDly(OS_DELAY_100msec*20);
		}
		print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "sz start ----");
	
		if(m_Send_sz_Info.port == COMM_TERM) 
			sprintf(cmd, "%s %s/%s 1> %s 0< %s", 
					SZ_CMD, LOG_ALARM_PATH, m_Send_sz_Info.file_name, UART_TERM_Dev, UART_TERM_Dev);
	#ifdef __TYPE__AAU__
		else if(m_Send_sz_Info.port == COMM_BT)   
		    sprintf(cmd, "%s %s/%s 1> %s 0< %s", 
		    		SZ_CMD, LOG_ALARM_PATH, m_Send_sz_Info.file_name, COMM_Blue_Dev, COMM_Blue_Dev);
	#endif
		else return;

		Start__App_Timer(&zModem_Timer, 60*1000); // 1m = 60x1000[ms]

		print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "sz start {%s}", cmd);
		system_agent(cmd);

		print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "sz end ----");
		m_Send_sz_Info.en = 0;	
	}
}

void Proc__Check_zModem_Timeout(void)
{
	int i = 0;	
	
	if(IsEnd__App_Timer(&zModem_Timer))
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[zModem] %d[s] timeout !!", zModem_Timer.timeout_ms/1000);
		Init__App_Timer(&zModem_Timer);

		do
		{
		    if(Is_Existing_sz()) // 'pidof sz' is exist	    
		    {
		    	print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "sz exist, kill sz, retry %d", i);
		    	system("killall -9 sz"); // or system("kill -9 `pidof sz`");
		    }
		    else 
		    {
		    	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "sz not exist");
		    	break;
		    }
			i++;
			OSTimeDly( 10*OS_DELAY_1sec );
	    }while(1); // 'pidof sz' is exist	    
	}
}



void TEST__send_5m_log_file_via_sz(void)
{	
	system("killall -9 sz; usleep 10000;");		
	system_agent("sz %s/2a2.log", LOG_ALARM_PATH);
}

//-------------------------------------------------------------------------------



void* LOG_Task(void *data)
{
	Thread_Priority_Init((Thread_ARG_Var*)data, 1);
	
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "LOG_Task RUN");

#ifdef __ALARM_LOG_BACKUP__	
	INT32S res;
	res = pthread_mutex_init(&Sem_gREMS_Pktlog_Alm, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] Sem_gREMS_Pktlog_Alm Init Fail.");
		exit(EXIT_FAILURE);
	}
	res = pthread_mutex_init(&Sem_gREMS_Pktlog_Ctl, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] Sem_gREMS_Pktlog_Ctl Init Fail.");
		exit(EXIT_FAILURE);
	}
#endif
				
	OSTimeDly( 5*OS_DELAY_1sec );	
	
	if((m_Eep.Log_OnOff != ON) && (m_Eep.Log_OnOff != OFF))
	{
		m_Eep.Log_OnOff = OFF;
	}	

	Check__Alarmlog_Directory();
	Check__Resetlog_Directory();
	Check__Eventlog_Directory();

	m_St.Log_Cnt = _get_log_file_count(LOG_ALARM_PATH);	
	if(m_St.Log_Cnt > MAX_LOG_ALARM_FILES) m_St.Log_Cnt = MAX_LOG_ALARM_FILES;	
		
	Init_Alm_Log_Info();

	OSTimeDly( 5*OS_DELAY_1sec );
	OSTimeDly( 5*OS_DELAY_1sec );
	
	Calc_Chunk_Size();
	
	if(m_Eep.Log_OnOff == ON)
	{
		m_St.Log_Prev = ON; // alarm log 지우지 않게 하도록.
		User_alarm_log_on();
	}
	else
	{
		m_St.Log_Prev = m_Eep.Log_OnOff;
	}

	memcpy( pre_alm, &m_St.Last_Alm, RU_ALARM_CNT); // init

	Init__PktLog(); // init gREMS-alarm-log
	
	while( m_Env.task_flag[_TASK_LOG_] )
	{		
		OSTimeDly( OS_DELAY_500msec );
		Update__Task_Count(_TASK_LOG_);
		Update_Log_Alm_Ctl_Cnt();

		if(IS___Valid_gREMS_Local_Time(m_St.Ru_Sys_Date))
		{
			if(m_St.ResetReason_cpy > 0)
			{
				Wr__Reset_Reason_to_LogFile(m_St.ResetReason_cpy);
				m_St.ResetReason_cpy = 0;
			}
			Proc_alarm_log();
		}
	}
	m_Env.task_END_status[_TASK_LOG_] = _CLR_;
	pthread_exit(NULL);
}

void* zModem_Task(void *data)
{
	Thread_Priority_Init((Thread_ARG_Var*)data, 1);
	
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "zModem_Task RUN");
				
	OSTimeDly( 5*OS_DELAY_1sec );	
		
	while( m_Env.task_flag[_TASK_zMODEM_] )
	{		
		OSTimeDly( OS_DELAY_500msec );
		Update__Task_Count(_TASK_zMODEM_);
		
		//--------------------------------------
		//OSTimeDly( 3*OS_DELAY_1sec );
		//print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "zModem_Task ----");
		
		Proc__send_5m_log_file_via_sz();		
	}
	m_Env.task_END_status[_TASK_zMODEM_] = _CLR_;
	pthread_exit(NULL);
}


/* EOF */









