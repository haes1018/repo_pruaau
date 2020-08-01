/********************************************************************************/
#ifndef __LOG_RST_H_
#define __LOG_RST_H_
/********************************************************************************/
#ifdef  LOG_RST_GLOBALS
#define LOG_RST_EXT
#else
#define LOG_RST_EXT extern
#endif
/**************************************************************/

LOG_RST_EXT int Wr__Reset_Reason_to_LogFile( INT8U reason );
LOG_RST_EXT void Check__Resetlog_Directory(void);
LOG_RST_EXT int _copy_file(int dfd, int sfd);

#ifdef __ALARM_LOG_BACKUP__
LOG_RST_EXT int _is_file_size(char *file_name);
#endif

/*
LOG_RST_EXT int write_reset_event_log(char *rstevt_str);
LOG_RST_EXT int _readline(int fd, void *buf, size_t maxlen);
LOG_RST_EXT int _is_year_start_line(char *str);
LOG_RST_EXT int _calc_year_start_lines(int fd);
LOG_RST_EXT int _set_year_line_offset(int fd, int req_line);
LOG_RST_EXT int _write_header_info(int fd);
LOG_RST_EXT int _write_body_info(int fd, char *buf, int size);
LOG_RST_EXT int _copy_to_tmp_file(char *name_buf, int sfd);
LOG_RST_EXT int _append_line_with_file(int fd, int file_line, char *str);
LOG_RST_EXT int _remove_and_append_line_with_mem(int fd, int file_size, int file_line, char *str);
LOG_RST_EXT int _remove_and_append_line_with_tmpfile(int fd, int file_line, char *str);
*/




/********************************************************************************/
#endif
/********************************************************************************/
