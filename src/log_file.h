/********************************************************************************/
#ifndef __LOG_FILE_H_
#define __LOG_FILE_H_
/********************************************************************************/
#ifdef  LOG_FILE_GLOBALS
#define LOG_FILE__EXT
#else
#define LOG_FILE__EXT extern
#endif
/**************************************************************/


LOG_FILE__EXT int wr_ctrl_cmd_log( PROTOCOL_FRAME* rxf);

LOG_FILE__EXT int Wr_Alm_Pkt_to_log( PROTOCOL_FRAME* txf);
LOG_FILE__EXT int Wr_Ctl_Pkt_to_log( PROTOCOL_FRAME* rxf );


LOG_FILE__EXT int Get_Pkt_from_log(INT8U log_idx, INT8U* pPkt, int idx);



LOG_FILE__EXT void* LOG_Task(void *data);
LOG_FILE__EXT void* zModem_Task(void *data);


LOG_FILE__EXT void Init_Alm_Log_Info(void);
LOG_FILE__EXT void Calc_Chunk_Size(void);

LOG_FILE__EXT void User_alarm_log_on(void);
LOG_FILE__EXT void User_alarm_log_off(void);
LOG_FILE__EXT void Free_Alm_Log_Buf(void);

LOG_FILE__EXT void MemAlloc_Alm_Log(void);

LOG_FILE__EXT void Proc_alarm_log(void);
LOG_FILE__EXT int Remove_all_AlmLog_Files(void);
LOG_FILE__EXT void Update_Log_Alm_Ctl_Cnt(void);



#ifdef __ALARM_LOG_BACKUP__
LOG_FILE__EXT void make_log_chksum(char* pull_path_name);
LOG_FILE__EXT void copy__log_old_from_current(char* pull_path_name);
LOG_FILE__EXT int Is__File_Existing(char* pull_path_name, char* ext_name);
LOG_FILE__EXT int Is__log_chksum_Valid(char* pull_path_name, char* ext_name);
LOG_FILE__EXT void copy__log_from_old(char* pull_path_name);
LOG_FILE__EXT void delete__log_all(char* pull_path_name);

LOG_FILE__EXT void make_log_chksum_test(int idx);
LOG_FILE__EXT int check_abnormal_log(char* path, char* log_file);
#endif


LOG_FILE__EXT int   get__list_of_5m_log_file(void);
LOG_FILE__EXT char* get__name_of_5m_log_file(int n);

LOG_FILE__EXT INT8U Set__send_5m_log_file_via_sz(char* file_name, INT8U port);
LOG_FILE__EXT void Proc__Check_zModem_Timeout(void);
LOG_FILE__EXT void TEST__send_5m_log_file_via_sz(void);


/********************************************************************************/
#endif
/********************************************************************************/
