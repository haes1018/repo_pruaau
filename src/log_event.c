/////////////////////////////
//	log_event.c
/////////////////////////////
#define LOG_EVT_GLOBALS
#include "includes.h"
/////////////////////////////
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#define	SINGLE_IO_SIZE				(4 * 1024)
#define	DEFAULT_FILE_MODE			0644

#define	evt_MEM_PROCESS_BASE_SIZE	(4 * 1024)

#define	LOG_PATH_UP	        "/usr/app/log"
#define	LOG_EVENT_PATH		"/usr/app/log/evt"
#define	MAX_LOG_EVENT_COUNT	200
#define	LOG_EVENT_NAME		"evt.log"

#define _EVENT_AGENT_BUF_SIZE_ 	512

extern int syncfs(int fd);

//////////////////////////////////////////////////////////////////

#define	EVT_HEADER_STRING_LINES	\
	"-------------------------------------------\r\n" \
 	" Date		Time		Event               \r\n" \
	"-------------------------------------------\r\n"

/*	Example:
 *			" 2015-02-06	07:41:30	0006"
 *
 *	sprintf(buf, " %d-%02d-%02d\t\t%02d:%02d:%02d\t\t%04d\r\n", \
 *					year, month, day, hour, minute, second, event);
 */

#define	MAX_EVT_READLINE_BUF		512



void Check__Eventlog_Directory(void)
{
	struct stat sb;
		
	if(stat(LOG_PATH_UP, &sb ) < 0)
		system_agent_sync("mkdir %s", LOG_PATH_UP);
	
	if(stat(LOG_EVENT_PATH, &sb ) < 0)
		system_agent_sync("mkdir %s", LOG_EVENT_PATH);
}


static int
_is_evt_file_size(char *file_name)
{
	struct stat st;
	int re;

	re = stat(file_name, &st);

	if (re == 0 && st.st_size > 0)
		return st.st_size;

	return -1;
}

static int
_evt_readline(int fd, void *buf, size_t maxlen)
{
	int n;
	int count;
	char *cp;
	char line_buf[MAX_EVT_READLINE_BUF];

	if (buf == NULL || maxlen <= 0)
		return -1;

	if (maxlen > MAX_EVT_READLINE_BUF)
		n = MAX_EVT_READLINE_BUF;
	else
		n = maxlen;

	count = read(fd, line_buf, n);

	if (count == 0)
		return 0;
	else if (count < 0)
		return -1;

	cp = buf;

	for (n=0; n < count; n++)
	{
		*cp++ = line_buf[n];
		if (line_buf[n] == '\n')
		{
			n++;
			break;
		}
	}

	*cp = '\0';

	count -= n;
	if (count > 0)
		lseek(fd, -count, SEEK_CUR);

	return n;
}

static int
_is_evt_year_start_line(char *str)
{
	int len;
	int i;
	int year;

	year = 0;
	len = strlen(str);

	for (i=0; i<len; i++)
	{
		if (str[i] == ' ')
			continue;

		if (isdigit(str[i]))
			year = atoi(&str[i]);

		break;
	}

	return year;
}

static int
_evt_calc_year_start_lines(int fd)
{
	char buf[256];
	int lines;
	int cnt;
	int year;
 
	lines = 0;

	lseek(fd, 0, SEEK_SET);

	while ((cnt = _evt_readline(fd, buf, 256)) > 0)
	{
		year = _is_evt_year_start_line(buf);
		if (year < 2000)
			continue;
		lines++;
	}

	return lines;
}

static int
_set_evt_year_line_offset(int fd, int req_line)
{
	char buf[256];
	int offset;
	int cnt;
	int year;
	int line;
 
	offset = -1;
	line   = 0;

	lseek(fd, 0, SEEK_SET);

	while ((cnt = _evt_readline(fd, buf, 256)) > 0)
	{
		year = _is_evt_year_start_line(buf);
		if (year < 2000)
			continue;
		line++;
		if (line == req_line)
		{
			offset = lseek(fd, 0, SEEK_CUR);
			break;
		}
	}

	return offset;
}

static int
_write_evt_header_info(int fd)
{
	int cnt;

	lseek(fd, 0, SEEK_SET);

	ftruncate(fd, 0);

	cnt = write(fd, EVT_HEADER_STRING_LINES, strlen(EVT_HEADER_STRING_LINES));
	syncfs(fd);

	if (cnt <= 0)
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[write_header_info.event] file write error (%s)", strerror(errno));

	return cnt;
}

static int
_write_evt_body_info(int fd, char *buf, int size)
{
	int cnt;

	cnt = write(fd, buf, size);
	syncfs(fd);

	if (cnt <= 0)
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[write_body_info.event] file write error (%s)", strerror(errno));

	return cnt;
}

static int
_copy_evt_file(int dfd, int sfd)
{
	char *p_buf;
	int size;
	int rd_cnt;
	int wr_cnt;

	size = 0;
	p_buf = (char *)malloc(SINGLE_IO_SIZE);
	if (p_buf)
	{
		while ((rd_cnt = read(sfd, p_buf, SINGLE_IO_SIZE)) > 0)
		{
			wr_cnt = write(dfd, p_buf, rd_cnt);
			if (wr_cnt < rd_cnt)
			{
				size = -1;
				break;
			}
			size += wr_cnt;
		}
		if (rd_cnt < 0)
			size = -1;
		free(p_buf);
	}

	return size;
}

/*
copy sfd to name_buf(dfd)
*/
static int
_evt_copy_to_tmp_file(char *name_buf, int sfd)
{
	int dfd = -1;
	int cnt = -1;

	dfd = mkstemp(name_buf);
	if (dfd > 0)
	{
		lseek(sfd, 0, SEEK_SET);
		cnt = _copy_evt_file(dfd, sfd);
		if (cnt <= 0)
		{
			close(dfd);
			dfd = 0;
		}
	}

	return dfd;
}

//////////////////////////////////////////////////////////////////

static int
_append_evt_line_with_file(int fd, int file_line, char *str)
{
	int cnt;
	int len;

	if (file_line == 0)
		_write_evt_header_info(fd);
	else
		lseek(fd, 0, SEEK_END);

	len = strnlen(str, 256);

	cnt = write(fd, str, len);
	syncfs(fd);

	if (cnt <= 0)
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[append_line_with_file.event] file write error (%s) ...", strerror(errno));

	return cnt;
}

static int
_evt_remove_and_append_line_with_mem(int fd, int file_size, int file_line, char *str)
{
	char *p_mbuf;
	int offset;
	int cnt;
	int re;
	int req_line;
	int str_len;

	re = -1;

	req_line = file_line - MAX_LOG_EVENT_COUNT + 1;
	if (req_line < 1)
		req_line = 1;

	offset = _set_evt_year_line_offset(fd, req_line);
	str_len = sizeof(EVT_HEADER_STRING_LINES);
	if (offset < str_len)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[remove_and_append_line_with_mem.event] invalid file data ...");
		return re;
	}

	p_mbuf = (char *)malloc(file_size);
	if (p_mbuf)
	{
		cnt = read(fd, p_mbuf, file_size-offset);
		if (cnt > 0)
		{
			re = _write_evt_header_info(fd);
			if (re > 0)
			{
				re = _write_evt_body_info(fd, p_mbuf, cnt);
				if (re > 0)
					re = _append_evt_line_with_file(fd, file_line, str);
			}
		}
		else
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[remove_and_append_line_with_mem.event] file read error (%s) ...", strerror(errno));
		}
		free(p_mbuf);
	}

	return re;
}

static int
_evt_remove_and_append_line_with_tmpfile(int fd, int file_line, char *str)
{
	int offset;
	int re;
	int req_line;
	int tmpfd;
	char tmp_file_name[32];
	int str_len;

	re = -1;

	sprintf(tmp_file_name, "/tmp/evt.XXXXXX");

	tmpfd = _evt_copy_to_tmp_file(tmp_file_name, fd);
	if (tmpfd <= 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[remove_and_append_line_with_tmpfile.event] mktemp file failed ..");
		return re;
	}

	req_line = file_line - MAX_LOG_EVENT_COUNT + 1;
	if (req_line < 1)
		req_line = 1;

	offset = _set_evt_year_line_offset(tmpfd, req_line);
	str_len = sizeof(EVT_HEADER_STRING_LINES);
	if (offset < str_len)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[remove_and_append_line_with_tmpfile.event] invalid file data ...");
		return re;
	}

	re = _write_evt_header_info(fd);
	if (re > 0)
	{
		re = _copy_evt_file(fd, tmpfd);
		if (re > 0)
			re = _append_evt_line_with_file(fd, file_line, str);
	}

	if (tmpfd > 0)
	{
		close(tmpfd);
		remove(tmp_file_name);
	}

	return re;
}

//////////////////////////////////////////////////////////////////

int
write_event_log(char *evt_str)
{
	int fd;
	int re;
	int file_line;
	char full_name[_EVENT_AGENT_BUF_SIZE_];

	if (evt_str == NULL)
		return -1;

	re = strnlen(evt_str, _EVENT_AGENT_BUF_SIZE_);
	if (re <= 0)
		return -1;

	re = -1;
	sprintf(full_name,"%s/%s", LOG_EVENT_PATH, LOG_EVENT_NAME);

	/* open & calc lines ...
	 */
	fd = open(full_name, O_RDWR);
	if (fd > 0)
		file_line = _evt_calc_year_start_lines(fd);
	else
	{
		fd = open(full_name, O_CREAT | O_RDWR, DEFAULT_FILE_MODE);
		file_line = 0;
	}

	if (fd < 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[write_reset_event_log.event] open failed (%s) ...", strerror(errno));
	}
	else if (file_line < MAX_LOG_EVENT_COUNT)
	{
		/* if lines are below base line count, just append.
		 */
		re = _append_evt_line_with_file(fd, file_line, evt_str);
	}
	else
	{
		int size;

		size = _is_evt_file_size(full_name);
		if (size > 0)
		{
			if (size <= evt_MEM_PROCESS_BASE_SIZE)
			{
				/* if log file size is small, use memory.
				 * remove the first year line and append.
				 */
				re = _evt_remove_and_append_line_with_mem(fd, size, file_line, evt_str);
			}
			else
			{
				/* if log file size is not small, use temporary file.
				 * remove the first year line and append.
				 */
				re = _evt_remove_and_append_line_with_tmpfile(fd, file_line, evt_str);
			}
		}
		else
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[write_event_log] log file is unstable (%s) ...", strerror(errno));
	}

	if (fd > 0)
	{
		close(fd);
		sync();
	}

	return re;
}

/*

-------------------------------------------
 Date		Time		Event
-------------------------------------------
 2000-01-01	11:00:33	0002
*/


int Wr__Event_to_LogFile(char *fmt, ...)
{
    //int re;
    int len;
    char buf[_EVENT_AGENT_BUF_SIZE_] = { 0 };
    va_list ap;

	sprintf(buf, " %d-%02d-%02d\t%02d:%02d:%02d\t",
			2000+m_St.Ru_Sys_Date[_ye_],
			m_St.Ru_Sys_Date[_mo_],
			m_St.Ru_Sys_Date[_da_],
			m_St.Ru_Sys_Date[_ho_],
			m_St.Ru_Sys_Date[_mi_],
			m_St.Ru_Sys_Date[_se_]);
			
    len = strlen(buf);
    va_start(ap, fmt);
    vsnprintf (&buf[len], _EVENT_AGENT_BUF_SIZE_-len, fmt, ap);    
    va_end(ap);

    len = strlen(buf);
    if(len < (_EVENT_AGENT_BUF_SIZE_-1))
	    buf[len] = '\n';	
    
	return write_event_log(buf);
}
    


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

/*
int
main(int argc, char *argv[])
{
	int re;
	char buf[256];

	if (argc < 2)
	{
		printf("invalid args\n");
		printf("usage: %s reset_event_string\n", argv[0]);
		return 1;
	}

	sprintf(buf, "%s\r\n", argv[1]);

	
	printf("result=%d\n", re);

	return 0;
}
*/

/* EOF */


