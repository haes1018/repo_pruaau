/////////////////////////////
//	DOWNLOAD_ZIP.C
/////////////////////////////
#define DOWNLOAD_ZIP_GLOBALS
#include "includes.h"
/////////////////////////////


/////////////////////////////
// Local Variables
/////////////////////////////



/////////////////////////////
// Local Functions
/////////////////////////////


//---------------------- FTP ----------------------------------------------

#define DEFAULT_FTP_PORT			21

#define ANONYMOUS_FTPGET_STRING 	"ftpget -u anonymous -p iro@ -P %d %s %s %s"
#define ANONYMOUS_FTPPUT_STRING 	"ftpput -u anonymous -p iro@ -P %d %s %s %s"


// local_file : client 
// remote_file : server , remote file을 가져옴.
int do_anony_ftpget(char *server_ip_addr, char *ftp_file, char *local_file, char *remote_file)
{
	int re;
	char cmdbuf[512];

	
	if (ftp_file == NULL || local_file == NULL) return -1;
	if (remote_file == NULL ) return -1;
	
	re = chdir(ftp_file);
	if (re) return -1;

	snprintf(cmdbuf, sizeof(cmdbuf), ANONYMOUS_FTPGET_STRING, \
					DEFAULT_FTP_PORT,	\
					server_ip_addr, 	\
					local_file, 		\
					remote_file);

	re = system(cmdbuf);

	return re;
}


//---------------------- unZip ----------------------------------------------

int
is_file_exist(char *file_name)
{
	struct stat st;
	int re;

	re = stat(file_name, &st);

	if (re == 0 && st.st_size > 0)
		return 0;

	return -1;
}

// unzip (압축파일명).zip -d (압축 풀 폴더 경로)

int
extract_file(char *wdir, char *zip_name, char *extract_file) // wdir 에 있는 zip 에서 extract 함.
{
	int re;
	char cmd_buf[512];

	if (wdir == NULL || zip_name == NULL || extract_file == NULL)
		return -1;

	re = chdir(wdir);
	if (re)
		return -1;

	if (is_file_exist(zip_name) < 0)
		return -1;

	snprintf(cmd_buf, 512, "unzip %s %s", zip_name, extract_file);

	re = system(cmd_buf);

	system( "sync" );

	return re;
}

int clr_all_file(char *wdir) // wdir 에 있는 zip delete
{
	int re;
	char cmd_buf[512];

	if (wdir == NULL )
		return -1;

	re = chdir(wdir);
	if (re) return -1;
	
	snprintf(cmd_buf, 512, "rm -r *");
	re = system(cmd_buf);

	system( "sync" );
	
	if (re) return -1;
	else return 0;
}

int unzip_file_to_dir(char *src_dir_file, char *wdir)
{
	int re;
	char cmd_buf[512];

	if (src_dir_file == NULL || wdir == NULL) return -1;

	re = chdir(wdir);
	if (re) return -1;

	if (is_file_exist(src_dir_file) < 0) return -1;

	snprintf(cmd_buf, 512, "unzip %s", src_dir_file );
	re = system(cmd_buf);

	system( "sync" );

	return re;
}


/*
int extract_file_to_dir(char *wdir, char *zip_name, char *extract_file) // wdir 에 있는 zip 에서 extract 함.
{
	int re;
	char cmd_buf[512];

	if (wdir == NULL || zip_name == NULL || extract_file == NULL)
		return -1;

	re = chdir(wdir);
	if (re)
		return -1;

	if (is_file_exist(zip_name) < 0)
		return -1;

	snprintf(cmd_buf, 512, "unzip %s -d %s", zip_name, extract_file);

	re = system(cmd_buf);

	system( "sync" );

	return re;
}
*/


/////////////////////////////
/*
IRCS에서 받은 압축파일을 
IRCS_DIR_FILE 에 다운로드.

다운로드된 IRCS_DIR_FILE 를 Rename
*/ 


// same Dir and same file @server and client 
#define FTP_DIR_FILE "/usr/ftp/down/iro.zip"
#define FTP_DIR      "/usr/ftp/down"   // directory for getting file from ftp server, 
#define FTP_FILE     "rcu.zip"    // file name received from server

#define FTP_UNZIP_DIR "/usr/ftp/dwon/unZip"  // dir' for unzip-processing
#define IRCS_DIR_FILE "/usr/ftp/down/iro_tmp.zip"



int Dn_Zip_From_FTP_Svr( INT8U* ftp_ip )
{
	int re;
	char ip_str[20];

	clr_all_file(FTP_DIR);
	
	sprintf( ip_str, "%d.%d.%d.%d", ftp_ip[0], ftp_ip[1], ftp_ip[2], ftp_ip[3] );

	D_Printf( DEBUG_SYSTEM, "get ftp ip %s\r\n", ip_str);
		
	re = do_anony_ftpget( ip_str, FTP_DIR, FTP_FILE, FTP_FILE);

	return re;
}


int UnZip_DnZipFile( void )
{
	int re;

	clr_all_file(FTP_UNZIP_DIR);
	re = unzip_file_to_dir( FTP_DIR_FILE, FTP_UNZIP_DIR ); 
	return re;
}

/*
void test_ftp(void)
{
	int re;
	INT8U ftp_ip[4] = {192,168,16,31 };
	char ip_str[20];

	clr_all_file(FTP_DIR);	
	sprintf( ip_str, "%d.%d.%d.%d", ftp_ip[0], ftp_ip[1], ftp_ip[2], ftp_ip[3] );
	re = do_anony_ftpget(ip_str, FTP_DIR, FTP_FILE, FTP_FILE);
	printf("ftpget result is %d\n", re);
	
	
	clr_all_file(FTP_UNZIP_DIR);
	re = unzip_file_to_dir( FTP_DIR_FILE, FTP_UNZIP_DIR ); 
	printf("Extract File result %d\n", re);
}
*/











///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////


#define Missed_Frame_Max (50)
typedef struct
{
	//-----------------
	// From REMS
	//-----------------	
	INT32U tot_size;
	INT16U tot_frame_num; // total frame number, (1st :upper byte, 2nd : lower byte)
	INT16U tot_fw_crc; // total FW CRC
	INT8U  dn_mode; // download mode
	/*	
		0x00 : All Node  (All Node Download)
		0x01 : Branch Node 	( Node Download @ Same Branch)
		0x02 : Each Node 	 ( Target-Node Download )
	*/
	INT8U  stdby_mode; // FW standby mode,  (0 : Yes, 1 : No)
	INT8U  update_time[5]; // in order, year-month-day-hour-minute 

	// CMD : 0xA2
	
	INT16U rems_ack_frm; // frmae # received from REMS
	INT16U rems_frm; // frame # received from REMS
	INT16U miss_frame[Missed_Frame_Max]; // frame number 0 ~ (max-1), 270th (0x010E), 2byte x 100, 없을 때 0 or 0xFFFF

	INT16U seg_size; // max 1000 byte

	//-----------------
	// Internal
	//-----------------	
	
	INT32U dn_rx_size; // download fize from REMS
	INT16U dn_continuous_cnt; // last frame # which is continuous..
	INT16U dn_next_frm; // frm # which is going to be received..
	INT16U dn_crc; // cal crc when downloading
}ZIP_FW_DN;


static ZIP_FW_DN m_Zip_Dn;
#define ZIP_FW_STD_MODE (0)


static OS_TIMER  zip_dn_timeout_timer;





//----------------------------------------------------------------------------

// 0xAE05
void Get_Comp_Dn_Onfo_05( INT8U* pTarget, INT8U* pSubId )
{
	(void)pTarget; (void)pSubId;
	//*pTarget = pUpDev->Comp_Dn.info_AE05[0];
	//memcpy( pSubId, &pUpDev->Comp_Dn.info_AE05[1], 3);
}






//----------------------------------------------------------------------------

/*
ACK Message : 0, 2, 4, 6, 8, 10, 20, 30, 40, 50, 50*n
	NACK 경우 : ACK 간격은 이전 ACK 간격으로 감소, 이후 ACK 응답 시 상기 규칙으로 증가

NACK 메시지 : 누락된 번호(최대 50개) 포함.
REMS : NACK에 대해 누락된 프레임을 재전송, 
Dn-Server Module : 재전송된 프레임을 포함하여 초기화된 ACK 간격에 따라 응답 

ex)
ACK 간격 40, 전송 프레임 80~120, 누락 프레임 81,82,85 인 경우 
--> 다음 ACK 메시지는 147번 프레임 수신 시 보내게 된다.
(ACK간격은 30으로 감소, 누락(81,82,85)+121~147번 프레임 전송 후 ACK 메시지를 보낸다. )

*/

//----------------------------------------------------------------------------

#define ACK_INTER_MAX (10)
static INT8U ack_idx = 0;
static INT8U ack_step[ACK_INTER_MAX] = { 0, 2, 4, 6, 8, 10, 20, 30, 40, 50};
static INT16U next_ack_frame = 0;

INT16U cal_next_ack_frame( INT16U cur_ack_frame, INT8U cur_ack_idx, INT8U miss_cnt )
{
	INT16U next_frame;
	INT8U ack_idx;
	INT8U max = ACK_INTER_MAX-1;

	if( miss_cnt )
	{
		if(cur_ack_idx > 0) ack_idx = cur_ack_idx - 1;
		else				ack_idx = 0;
	}
	else
	{			
		if(cur_ack_idx < max) ack_idx = cur_ack_idx + 1;
		else                  ack_idx = max;		
	}

	next_frame = cur_ack_frame + ack_step[ack_idx];
	
	if( next_frame > miss_cnt) next_frame -= miss_cnt;	

	return next_frame;
}

//---------------------------------------------------

void Insert_miss_frame( INT16U frame_num )
{
	INT8U i;

	for( i = 0; i < Missed_Frame_Max; i++ )
	{
		if(m_Zip_Dn.miss_frame[i] == frame_num) return;
	}
	
	for( i = 0; i < Missed_Frame_Max; i++ )
	{
		if(m_Zip_Dn.miss_frame[i] == 0xFFFF)
		{
			m_Zip_Dn.miss_frame[i] = frame_num;			
			return;
		}
	}
}
//---------------------------------------------------
void Del_miss_frame( INT16U frame_num )
{
	INT8U i;

	for( i = 0; i < Missed_Frame_Max; i++ )
	{
		if(m_Zip_Dn.miss_frame[i] == frame_num)
		{
			m_Zip_Dn.miss_frame[i] = 0xFFFF;
			
			return;
		}
	}
}
//---------------------------------------------------
INT8U Get_Cnt_miss_frame(void)
{
	INT8U i, cnt;

	for(cnt = 0, i = 0; i < Missed_Frame_Max; i++ )
	{
		if(m_Zip_Dn.miss_frame[i] != 0xFFFF)
		{
			cnt++;
		}
	}
	return cnt;
}

//----------------------------------------------------------------------------------
// 임의 file 위치에 frame 저장.
//
int Save_DnData_File_zip( unsigned char *_Dat, INT16U len, INT16U frm_num )
{
	FILE* fp;	
	//INT16U posi;
	(void)frm_num;
	fp = fopen( IRCS_DIR_FILE, "ab" );
	if( fp == NULL )
	{
		syslog( LOG_ERR, "DN_TMP_FILE Open Err(5)\n");
		return -1;
	}

	//posi = frm_num * m_Zip_Dn.seg_size;
	// posi 에 len 만큼 write

	if( fwrite( _Dat, 1, len, fp )<len )
	{
		fclose(fp);
		return -1;
	}
	sync();
	fclose(fp);	
	return 0;
}






/*
1. ftpget / ftpput
    - anonymous 로 login 하여 ftp 로 get 하는 부분과 put 하는 부분으로 되어 있습니다.
    
3. unzip
    - linux에서 zip file을 푸는 program이 "unzip" 인데, 이것을 이용하여 원하는 file만 푸는 것으로 가정하였습니다.
    - 그냥 shell 에서 사용할 때에는 다음과 같습니다.
         # unzip test.zip            # zip file 전체를 푸는 경우
         # unzip test.zip one_file # zip file에 one_file이 있을 경우, 그 file 만 푸는 경우
         # unzip -t test.zip         # zip file의 상태를 검사하고, 내부 file list 를 표시
         # unzip -l test.zip         # zip file의 내부 file list를 표시

*/


/* EOF */



//--------------------------------------------------------------------------------------------------------------



