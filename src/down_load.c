/////////////////////////////
//	DOWNLOAD.C
/////////////////////////////
#define DOWNLOAD_GLOBALS
#include "includes.h"
/////////////////////////////

/*
Use "_USE_ZIP_Down_" Always
*/

/////////////////////////////
// Local Variables
/////////////////////////////
static INT16U downReceivedBlock, downCurrentBlock, downTotalBlock, tot_crc;
static INT16U downTotalBlock_continue;	////// __YCSO__DN__CONTINUE__ added for block size check
//static INT16U DN_status = 0, DN_block= 0;
static INT32U down_addr;

static INT32U down_load_file_size;
static INT32U down_load_recv_size;
static INT32U rxFinishConfirm_cnt = 0;
static INT8U	amp_DN_status = 0;		/////	DN END flag
static INT8U	amp_DN_flag = 0;		/////	DN start flag
static INT8U	psu_DN_flag = 0;
static INT8U	psu_DN_status = 0;
static INT8U	amp_DN_confirm = 0;		/////	DN confirm flag
static INT8U	psu_DN_confirm = 0;
static OS_TIMER DN_wait_timer;
static INT32U	DN_wait_time;


pthread_mutex_t	dn_wait;

/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////
//----------------------------------------------------------
#define	EXT_CHKSUM       "chksum" // "xxx.chksum"
#define	EXT_Wait         "wait"   // "xxx.wait"


/*
개별 다운로드


"/tmp/RCU_DN.tar"
"/tmp/DN_TMP_INFO_FILE"






Package download
*/

//----------------------------------------------------------
// download temporary directory
//----------------------------------------------------------
#define DN_TEMP_DIR       "/tmp"
#define DN_TEMP_FILE      "/tmp/RCU_DN.tar"  // RCU download file or Server-zip-file

/////////#define __OLD__FS__SYSTEM__
#ifdef __OLD__FS__SYSTEM__
#define DN_TEMP_RUN_SH    "/tmp/DN_File/dn_copy.sh"
#else
#define DN_TEMP_RUN_SH    "upgrade_rcu_app.sh /tmp/DN_File/dn_copy.sh"
#endif
#define DN_COPY_SH        "/tmp/DN_File/dn_copy.sh" // not used


#define DN_TEMP_INFO_FILE "/tmp/DN_TMP_INFO_FILE"  // 다운로드 파일의 헤더에 있는 crc 정보를 해당 파일로 저장.
#define DN_File_Dir       "/tmp/DN_File"

//----------------------------------------------------------
// app directory
//----------------------------------------------------------
#define	RCU_DIR        "/usr/app/sys"
#define	DIG_DIR        "/usr/app/dtu"

#define	RCU_1_DIR      "/usr/app/sys_1"
#define	RCU_2_DIR      "/usr/app/sys_2"
#define	RCU_T_DIR      "/usr/app/sys_t"

#define	DIG_1_DIR      "/usr/app/dtu_1"
#define	DIG_2_DIR      "/usr/app/dtu_2"
#define	DIG_T_DIR      "/usr/app/dtu_t"



#define TMP_DIR		   "/usr/tmp"	///// YCSO added 2019.07.25
#define IMAGE_DIR  	   "/usr/app_dft/images"


#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
#define	MMC_Partition    "/dev/mmcblk0p2"
#define	MMC_Partition_3  "/dev/mmcblk0p3"
#endif

#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU	
#define	MMC_Partition    "/dev/mmcblk1p2"
#define	MMC_Partition_3  "/dev/mmcblk1p3"
#endif

#define MNT_DIR        "/mnt" // mount directory

//----------------------------------------------------------
// app name
//----------------------------------------------------------
#define	RCU__FILE       "RCU_App_DN"
#define	DTU__FILE       "pru_dtu" // ANTS-PRU-DTU-APP
#define	DPD__FILE       "dpd-smp" // ANTS-PRU-DPD-APP
#define KERNEL_FILE		"image.ub"

//--------------------

#define BITSTREAM_FILE	    "bitstream.bin"
#define BITSTREAM_FILE_1	"bitstream_1.bin"
#define BITSTREAM_FILE_2	"bitstream_2.bin"
#ifdef __TYPE__PRU__
#define	FPGA_FILE       "PRU_TOP.bit"
#else
#define	FPGA_FILE       "AAU_RU.bit"
#endif
//----------------------------------------------------------
#define WAIT_DN_FILE_CHK     "/usr/app/sys/Wait_Dn_File_chk.sh"
//----------------------------------------------------------


#define FPGA__DIR	   "/usr/app/fpga"
#define FPGA_1_DIR	   "/usr/app/fpga_1"
#define FPGA_2_DIR	   "/usr/app/fpga_2"
#define FPGA_T_DIR	   "/usr/app/fpga_t"


#define DTU_CP_ACTIVE  "dtu_app_mv_active.sh" // current rcu active 값의 dtu_x에 copy


/////  ycso START ------------------------------------------------
/*
장비명 (ASCII)	12
중계기 code 	2
bineary flage	1
제조사 code		1
Version			4	( 3 + 1(hidden) )
Binary size		4
Build Time		4
file type		4
HASH size		32
*/

typedef struct  __attribute__((packed))
{
	INT8U	equipment_name[12];
	INT8U	repeater_code[2];  
	INT8U	binary_flag; 
	INT8U	manufacturer_code; 
	INT8U	version[3];
	INT8U	version_hidden;
	INT8U	binary_size[4];		
	INT8U	build_time[4];
	INT8U	file_type[4];
	INT8U	hash[32];
} DN_ZIP_FILE_HEADER;

DN_ZIP_FILE_HEADER zip_header;
DN_ZIP_FILE_HEADER	pre_DN_file_header;

char zip_file_name[32];
char ftp_DN_file[64];
static char 	systemCall_buf[MAX_USER_CMD_BUF];

// '/usr/app/sys/PAU 에 copy된 2개 PAU 최종 file name 
static char		DN_PAU_file_name_NW[64]	= {0}; // newwave , 현재 package download 받은 pau.newwave의 file name
static char		DN_PAU_file_name_WE[64]	= {0}; // waveelec, 현재 package download 받은 pau.waveele의 file name

static INT8U	DN_PAU_file_name_size	= 0;
static char		DN_PSU_file_name[64]	= {0}; // '/usr/app/sys/PSU 에 copy된 현재 PSU 최종(4개중 최종1개) file name
static INT8U	DN_PSU_file_name_size	= 0;
static INT8U	zip_DN_flag_post		= 0;

static INT8U	DN_TASK_REQ_ftp		= 0; // ftp-down request ok 되면, SET.
static INT8U	DN_TASK_REQ_zip		= 0; // package download (GUI 0x5A(rxFinishConfirm) or FTP-down) 시, SET.
static INT8U	DN_TASK_REQ_Comfirm = 0; // reset 대기. 즉시이거나 0x5b command 내러오면 SET 됨.
static char		DN_cmd[300] = {0}; // 2019.07.03 : scyo
static INT8U	ftp_time[6] = {0}; // 2019.07.03 : scyo

#define MAX_FILE_NAME_SIZE	64 /// 임시 size.
#define ZIP_FILE_HASH_TMP	"/tmp/zip_file_hash"
#define ZIP_FILES_DIR		"/tmp/zip"
#define DN_TEMP_FILE_ZiP	"/tmp/DN_file.zip"
#define ZIP_FILE_PARSING	"/tmp/zip/parsing.txt"

//---------------------- package download 된 파일들을 제조사별로 (버전마다 달라지는)파일이름을 parsing.
// module 2원화시, package unzip 된 download file parsing
#define PSU_PARSE_AN_PSU	"/tmp/zip/psu_parse_ants_psu.txt"
#define PSU_PARSE_MC_PSU	"/tmp/zip/psu_parse_mcor_psu.txt" // 명코아콤
#define PSU_PARSE_AN_GPS	"/tmp/zip/psu_parse_ants_gps.txt"
#define PSU_PARSE_MC_GPS	"/tmp/zip/psu_parse_mcor_gps.txt" // 명코아콤
//----------------------
#define PAU_PARSE__NEWWA	"/tmp/zip/pau_parse_newwave.txt" // new_wave
#define PAU_PARSE__WAVEE	"/tmp/zip/pau_parse_waveele.txt" // wave_electics
//--------------------------------------------

#define PAU_PATH			"/usr/app/sys/PAU"
#define PSU_PATH			"/usr/app/sys/PSU"
#define TAR_ARCHIVE			"/tmp/TAR_ARCHIVE"

#define ZIP_FILE_PARSE_PSU	"/tmp/zip/parse_psu.txt"


#ifdef __TYPE__PRU__
const unsigned char  *PKG_Check_name = (unsigned char *)"PRU10-35G-4T";
#else
const unsigned char  *PKG_Check_name = (unsigned char *)"AAU10-35G-4T";
#endif

PROTOCOL_FRAME_COMPACT	ftp_txf;
INT8U	ftp_txf_port = 0;

// 2020.04.02
//// YCSO added 
enum
{
	ftp_cmd_ACK = 0,
	ftp_cmd_NACK,         // 1
	ftp_cmd_UDP_DL_NACK,  // 2
	fpt_cmd_sFTP_DL_NACK, // 3
	
	MAX_ftp_cmd_return
};
time_t ftp_dn_time; // 2020.04.02



///// ycso END -----------------------------------------------






/*
void Del_Old_FPGA_bitstream(void)
{
	char cmd[128];
	char file_name[128];
	FILE* fp;
	
	//------------------
	sprintf(file_name, "%s/%s", DIG_DIR, FPGA_FILE); 
	fp = fopen(file_name, "r");
	if( fp != NULL )
	{
		sprintf(cmd, "rm -f %s", file_name); 
		system(cmd);
		
		fclose( fp );
	}
	//------------------
	sync();
}
*/

/*
----------------------------------------------------------
Update 
	eMMC Partition-FPGA-Bit-Stream 
	with 
	New-download-FPGA-Bit-Stream
	
"mount -t ext4 /dev/mmcblk0p2 /mnt"
"cp -f ./PRU_TOP.bit /mnt"
"sync"
"umount /mnt"
----------------------------------------------------------
*/
///////////////// ycso ///////////////////
/*
//-------------------------------------------------
//	(1) cp file_name to /path
//-------------------------------------------------
*/

#include <libgen.h>

int update_pau_psu_file( char* pull_path_file_name, char* path)
{
	struct stat sb;
	char cmd[256] = {0};
	int fd = 0;
	char *name;
	
	if( stat( path, &sb ) < 0 )
	{
		sprintf(cmd, "mkdir %s\n", path );
		system( cmd );
		system( "sync" );
	}

	fd = open(pull_path_file_name , O_RDONLY);

	if( fd > 0 )
	{
		close(fd);

		name = basename(pull_path_file_name);
		if(name != NULL)
		{
			sprintf(cmd, "rm -rf %s/%s; sync", path, name );
			system( cmd );
		}		

		sprintf(cmd, "cp -f %s %s/\n", pull_path_file_name, path); // '-f' is --force 
		system( cmd );
		system( "sync" );
		return RET_SUCCESS;
	}
	return RET_Error;
}
/*
//-------------------------------------------------
//	(1) mkdir if dir is not exist 
//	(2) del /path/ *.*
//	(3) cp file_name to /path
//-------------------------------------------------
*/
int change_PAU_PSU_file( char* pull_path_file_name, char* path)
{
	struct stat sb;
	char cmd[256] = {0};
	int fd = 0;
	
	if( stat( path, &sb ) < 0 )
	{
		sprintf(cmd, "mkdir %s\n", path );
		system( cmd );
		system( "sync" );
	}

	fd = open(pull_path_file_name , O_RDONLY);

	if( fd > 0 )
	{
		close(fd);
		
		sprintf(cmd, "rm -rf %s/*\n", path );
		system( cmd );
		system( "sync" );

		sprintf(cmd, "cp %s %s/\n", pull_path_file_name, path);
		system( cmd );
		system( "sync" );
		return RET_SUCCESS;
	}
	return RET_Error;
}

int change_mmcblk_file( char* file_name, char* rm_file ,char* real_file_name)
{
	char cmd[128];
	FILE* fp;

	// copy
	//------------------
	fp = fopen(file_name, "r");
	if(fp != NULL)
	{
		fclose(fp);

		// mount
		sprintf(cmd, "mount -t ext4 %s %s", MMC_Partition, MNT_DIR); 
		system(cmd);

		sprintf(cmd, "rm -rf %s/%s\n",MNT_DIR, rm_file);
		system(cmd);

		if( real_file_name == NULL )
			sprintf(cmd, "cp -f %s %s/", file_name, MNT_DIR);
		else
			sprintf(cmd, "cp -f %s %s/%s", file_name, MNT_DIR, real_file_name); 
		system(cmd);		
		
		system("sync");

		// unmount
		sprintf(cmd, "umount %s", MNT_DIR); 
		system(cmd);

		/* ///// YCSO delete 190627
		파일 시스템의 3번 파티션은 건들지 않도록 변경.
		*/
			
		/*
		////sprintf(cmd, "mount -t ext4 %s %s", MMC_Partition_3, MNT_DIR); 
		////system(cmd);
		////
		////sprintf(cmd, "rm -rf %s/%s\n",MNT_DIR, rm_file);
		////system(cmd);
		////
		////if( real_file_name == NULL )
		////	sprintf(cmd, "cp -f %s %s/", file_name, MNT_DIR);
		////else
		////	sprintf(cmd, "cp -f %s %s/%s", file_name, MNT_DIR, real_file_name); 
		////system(cmd);		
		////
		////system("sync");
		////// unmount
		////sprintf(cmd, "umount %s", MNT_DIR); 
		////system(cmd);					
		*/
		return RET_SUCCESS;		
	}
	else
	{
		return RET_Error;
	}
}




/*
---------------------------------------------------------------

"/usr/app_dft/images/image.ub" file을 새로 생성하여 놓고, 

"/sbin/system_slice_check.sh" 을 수행하면 

'image.ub' 안의 kernel & rootfs image가 설치됨.

---------------------------------------------------------------
*/

int Update_image_ub( char* file_name, char* rm_file ,char* real_file_name)
{
	char cmd[128];
	FILE* fp;
	int result = 0;

	//------------------
	// copy
	//------------------
	fp = fopen(file_name, "r");
	if(fp == NULL)
	{
		return RET_Error;
	}
	else
	{
		fclose(fp);

		// mount
		snprintf(cmd, sizeof(cmd), "mv %s %s/image.ub",file_name,IMAGE_DIR);
		system(cmd);
		sync();
				
		result = system("system_slice_check.sh");

#ifdef __USE_KERNEL_ROOTFS_DN__CHECK__		//// YCSO
		// 2020.04.14 for Kernel, RootF/S download
		result = result>>8; // bit15~bit8 이 실제 result

		if( result & 0x04 ) // 0x04(Kernel & RootF/S), 0x02(FPGA)
		{
			sync();
			print_agent (DEBUG_SYSTEM, VT_BOLD_GREEN, "success Kernel & RootF/S Change = %d",  result);
		}
		else
		{
			print_agent (DEBUG_SYSTEM, VT_BOLD_YELLOW, "[%s] Kernel & RootF/S same file system_slice_check return = %d", cmd, result);
		}
		return RET_SUCCESS;
#else
		if( result == 0 )
		{
			print_agent (DEBUG_SYSTEM, VT_BOLD_YELLOW, "[%s] same(overwrite), update image_ub system_slice_check return = %d", cmd, result);
			return RET_Error;
		}
		else
		{
			sync();
			return RET_SUCCESS;
		}
#endif
	}
}


//////////////// ycso END ///////////////

/*----------------------------------------------------------------------
/usr/tmp/ *.bit 가 bitstream.bin 인지 확인.
*.bit 이면 return.

bitstream.bin 이면, 
1. file name을 'bitstream.bin' 로 변경.
2. copy bitstream_1.bin -> bitstream_2.bin
3. copy bitstream.bin   -> bitstream_1.bin
4. copy bitstream.bin   -> /usr/app/fpga_t/bitstream_1.bin
5. run "system_slice_check.sh" 하고 return 값 check




----------------------------------------------------------------------*/

int Update_Fpga_Bitstream(void)
{
	char cmd[256] = {0};
	char file_name[128] = {0};
	int result = 0;
	FILE* fp = NULL; // 2019.07.25

	//----------------------------------------------------------------------
	// copy
	//------------------
	sprintf(file_name, "%s/%s", TMP_DIR, FPGA_FILE); //// YCSO change 190628
	// for checking '*.bit' is '*.betstream' or '*.bit'
	snprintf(cmd , sizeof(cmd), "dumpimage -l %s", file_name);
	result = system( cmd );
	
	/*
	result 
	--------
	0	: bitstream file . change file name *.bit --> *.bin
	etc : bit file. cannot change file name & mount patition block 2 for change bit file AAU / PRU .BIT
	*/
	if(result == 0)		///// bitstream file
	{
		sync();
		
		// change file name *.bit --> *.bin
		snprintf(cmd, sizeof(cmd), "mv %s %s/%s\n", file_name, TMP_DIR, BITSTREAM_FILE);
		printf("%s",cmd);
		system(cmd);
		sync();
		memset( file_name, 0, sizeof(file_name) );	
		sprintf(file_name, "%s/%s", TMP_DIR, BITSTREAM_FILE); // file_name chagned to 'bitstream.bin'
		
		////printf("__YCSO__ HERE BITSTREAM FIEL\n");
	}
	else // file_name is not bitstream.bin but real *.bit, So return.
	{
		D_Printf( DEBUG_SYSTEM, "[RCU] not mkimage bitstream file [%s]", file_name);
		return RET_Error;
	}
	//----------------------------------------------------------------------
	
	fp = fopen(file_name, "r"); // file_name is 'bitstream.bin'
	
	printf("%s\n", file_name);	
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, "Update_Fpga_Bitstream [%s]", file_name);
	
	if(fp == NULL)	
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RCU] Update_Fpga_Bitstream open fail.");
		return RET_Error;
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "Update_Fpga_Bitstream open success.");
		
		fclose(fp);

	#ifdef __OLD__TYPE__ROOTFS__ // *.bit 라면, 앞에서 return 되었음.
		// mount
		snprintf(cmd, sizeof(cmd), "mount -t ext4 %s %s", MMC_Partition, MNT_DIR); // 2019.07.25
		D_Printf( DEBUG_SYSTEM, "[%s]", cmd);
		result = system(cmd); // 2019.07.25

		if( result != 0 ) // 2019.07.25
		{
			printf("MOUNT FAIL !!!!!!\n");
			return RET_Error;
		}

		////printf("_YCSO_ CHECK HERE????? !!!!!!\n");

		snprintf(cmd, sizeof(cmd), "rm -f %s/*.bit", MNT_DIR);
		system(cmd);
		sync();
		
		sprintf(cmd, "cp -f %s %s", file_name, MNT_DIR); 
		D_Printf( DEBUG_SYSTEM, "[%s]", cmd);
		system(cmd);		
		
		sync();

		// unmount
		sprintf(cmd, "umount %s", MNT_DIR); 
		D_Printf( DEBUG_SYSTEM, "[%s]", cmd);
		system(cmd);
	#endif
		//--------------------------
		// copying
		//-------------
		// /usr/app_dft/images/bitstream_1.bin -> bitstream_2.bin
		// /usr/tmp/bitstream.bin              -> bitstream_1.bin
		
		snprintf(cmd, sizeof(cmd), "mv %s/%s %s/%s", IMAGE_DIR,BITSTREAM_FILE_1, 
											         IMAGE_DIR,BITSTREAM_FILE_2);
		system(cmd);
		sync();
		//--------------------------		
		snprintf(cmd, sizeof(cmd), "cp %s %s/%s", file_name, 
		                                          IMAGE_DIR,BITSTREAM_FILE_1);
		system(cmd);
		sync();
		result = system("system_slice_check.sh");

#ifdef __USE_KERNEL_ROOTFS_DN__CHECK__		//// YCSO
		
		// 2020.04.14 : 0x04(Kernel & RootF/S), 0x02(FPGA)
		result = result>>8; // bit15~bit8 이 실제 result

		if( result & 0x02 ) // fpga 이든 아니든 success
	   {
		   sync();
			print_agent (DEBUG_SYSTEM, VT_BOLD_GREEN, "[%s] success, FPGA same file system_slice_check return = %d", cmd, result);
	   }
	   else // overwrite, success
	   {
			print_agent (DEBUG_SYSTEM, VT_BOLD_YELLOW, "[%s] same file, FPGA same file system_slice_check return = %d", cmd, result);
	   }
	   return RET_SUCCESS;
	#else // 0 or not : kernel & rootfs 이면, 0x04이어서, fpga가 아닌데, success.
		if( result == 0 )
		{
			print_agent (DEBUG_SYSTEM, VT_BOLD_YELLOW, "[%s] same, FPGA same file system_slice_check return = %d", cmd, result);
			return RET_Error;
		}
		else // 0x02(FPGA)
		{
			print_agent (DEBUG_SYSTEM, VT_BOLD_GREEN, "[%s] success, FPGA same file system_slice_check return = %d", cmd, result);
			sync();
			return RET_SUCCESS;
		}
#endif
	}
}



int Make_App_File_Total_Path(char* dir, char* file, char* ext1, char* ext2, char* tot_path)
{
	char file_tmp[128];
	char file_end[128];

	if((dir == NULL)||(file == NULL)) return RET_Error;
	
	sprintf(file_tmp, "%s/%s", dir, file); 
	sprintf(file_end, "%s", file_tmp); 

	if(ext1 != NULL)
	{		
		sprintf(file_tmp, "%s.%s", file_end, ext1);
		sprintf(file_end, "%s", file_tmp);

		if(ext2 != NULL)
		{		
			sprintf(file_tmp, "%s.%s", file_end, ext2);
			sprintf(file_end, "%s", file_tmp);
		}		
	}
	
	sprintf(tot_path, "%s", file_end);	
	return RET_SUCCESS;
}


static INT8U Proc_ZimImg_Req = 0;
static INT8U Proc_ZimImg_Req_Fw_Wait = 0;



/*
directory 없을 경우, 생성하도록.
*/



INT8U sw_side;
INT8U dn_version[2]; // 141007


///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////
/*
static void init_DN( void )
{
	down_addr = 0;
	down_load_recv_size = 0;
	tot_crc = 0;
	downReceivedBlock = 0;
	rxFinishConfirm_cnt = 0;

}
*/
INT16U rxStartDownload( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{
	INT8U len = 0; // init 0
	FILE* fp;
	INT8U totPkt = 1, curPkt = 1;

	(void)port;
///// YCSO block 190515
///// __YCSO__DN__CONTINUE__ START	--> delete init point
/*
	down_addr = 0;
	down_load_recv_size = 0;
	tot_crc = 0;
	downReceivedBlock = 0;
	rxFinishConfirm_cnt = 0;
*/	
///// __YCSO__DN__CONTINUE__ END

	downTotalBlock = (rxf->data[1]<<8)|rxf->data[2];

	len = 0;
///// YCSO change 190515
///// __YCSO__DN__CONTINUE__	--> file open option change
//	fp = fopen( DN_TEMP_FILE, "wb" );
	fp = fopen( DN_TEMP_FILE, "ab" );
	if( fp!=NULL )
	{
		txf->data[len++] = 0x00;
		fclose( fp );
	}
	else txf->data[len++] = 0x01;	

	txf->data[len++] = rxf->data[0];
	txf->data[len++] = rxf->data[1];
	txf->data[len++] = rxf->data[2];

	//-----------------------------------------
	// REMS	
	m_St.DL_State[DN_RCU][DL_ST] = DL_St_Now;
	m_St.DL_State[DN_RCU][DL_CNT] = 0;

	if(rxf->Type == 0x08)
	{
		m_Env.DnMode.Rcu = rxf->data[3];
		m_Env.DnMode.pkg = rxf->data[3];

		txf->data[len++] = rxf->data[3];
	}
	else
	{	
		m_Env.DnMode.Rcu = _DN_RST_AUTO;
//		m_Env.DnMode.pkg = _DN_RST_AUTO;
		m_Env.DnMode.pkg = rxf->data[3];
		txf->data[len++] = rxf->data[3];
	}

	D_Printf(DEBUG_SYSTEM, " [Down] CMD (0x%02X) Mode(%d) \r\n", rxf->Type, m_Env.DnMode.Rcu);
	printf(" [Down] CMD (0x%02X) Mode(%d) \r\n", rxf->Type, m_Env.DnMode.pkg);

	
	Proc_ZimImg_Req = 0;
	Proc_ZimImg_Req_Fw_Wait = 0;

	return Make_Frame( rxf->Type, rxf->Rcode, txf, len, rxf->seqNo16, totPkt, curPkt );
}

//-------------------------------------------------------------

INT16U rxReStartDownload( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{
	INT8U totPkt = 1, curPkt = 1;
	(void)port;	

	txf->data[0] = 0x00;

	Proc_ZimImg_Req = 0;
	Proc_ZimImg_Req_Fw_Wait = 0;

	return Make_Frame( rxf->Type, rxf->Rcode, txf, 1, rxf->seqNo16, totPkt, curPkt );
}

//-------------------------------------------------------------
// 32byte header 중 일부에 있는 파일의 size : down_load_file_size 에 update
// 32byte header 중 일부에 있는 파일의 crc  : /tmp/DN_TMP_INFO_FILE 에 update
int SaveInfoFile( unsigned char *_Dat )
{
	FILE* fp;
	int i;

	
	for( i=0; i<12; i++ )
	{
		// conv small char --> large char
		if( (('a'<=_Dat[i])&&( _Dat[i]<='f')) )
		{
			_Dat[i] -= ('a'-'A');
		}

		// neither char nor num, return -1
		if( !((('0'<=_Dat[i])&&( _Dat[i]<='9')) || (('A'<=_Dat[i])&&( _Dat[i]<='F')))  )
		{
			return -1;
		}
	}

	// _Dat is 12byte ascii = 4byte-crc-ascii + 8byte-length-ascii
	//

	down_load_file_size = 0;

	for( i=0; i<8; i+=2 )
	{
		down_load_file_size <<= 8;
		down_load_file_size |= AHex2Dig( &_Dat[4+i] );
	}


	fp = fopen( DN_TEMP_INFO_FILE, "wb" ); // save crc ascii 4byte
	if( fp == NULL )
	{
		syslog( LOG_ERR, "DN_TMP_FILE Open Err(1)\n");
		return -1;
	}

	if( fwrite( _Dat, 1, 8, fp )<8 ) // 1byte unit, 8 byte write, 
	{
		fclose(fp);
		return -1;
	}
	sync();
	fclose(fp);
	return 0;
}

/*-------------------------------------------------------------
*.rsm, *.dtu, *.fpga 에서 header 32byte와 file 끝 file.name 분리된 
dn_file은 '/tmp/RCU_DN.tar'.(순수 tar file)

header 32byte 에서 만들어진 
info(info_file) 에서 crc 추출하고,
'/tmp/RCU_DN.tar'(dn_file)  의 실제 crc 계산해서
위 2개의 crc를 비교. 

return  1 if success, 
return -1 if fail

'ver' 는 사용하지 않으나, NULL 아니면, 
info(info_file)의 추출된 CRC : 4byte ascii -> 2byte hex
-------------------------------------------------------------*/
int CheckDN_File( char* dn_file, char* info_file, INT8U* ver )
{
	FILE* fp;
	INT8U tmp[32];
	INT16U crc, tot_crc_x;
	INT8U tmp_ver[4];
	int i;

	// Get CRC 2byte form tmp[0:3], 4byte ascii
	if( info_file != NULL )
	{
		fp = fopen( info_file, "rb" );
		if( fp == NULL )
		{
			syslog( LOG_ERR, "DN_TMP_FILE Open Err(2)\n");
			return -1;
		}
		if( fread( tmp, 1, 12, fp ) < 8 ) // info file 정보는 8.
		{
			fclose(fp);
			return -1;
		}
		fclose(fp);

		for( i=0; i<8; i++ )
		{
			// convert to uppercase character
			if( (('a'<=tmp[i])&&( tmp[i]<='f')) )
			{
				tmp[i] -= ('a'-'A');
			}
			// neither char nor num, return -1
			if( !((('0'<=tmp[i])&&( tmp[i]<='9')) || (('A'<=tmp[i])&&( tmp[i]<='F')))  ) // if not 0~9, A~F
			{
				return -1;
			}
		}
		// tmp[0:3], 4byte ascii -> 2byte crc
		tmp_ver[0] = AHex2Dig( &tmp[0] );
		tmp_ver[1] = AHex2Dig( &tmp[2] );

		crc = (tmp_ver[0]<<8) | tmp_ver[1];
	}

	//InitTimeCheck( &time1 );

	// download 된 파일이 존재하는지 check
	fp = fopen( dn_file, "rb" ); // *.tar(dn_file)
	if( fp == NULL )
	{
		syslog( LOG_ERR, "DN_TMP_FILE Open Err(3)\n");
		return -1;
	}
	// download 된 파일의  total crc 계산.
	if( info_file != NULL )
	{
		tot_crc_x = 0;
		while( 1 )
		{
			i = fread( tmp, 1, 32, fp );
			if( i>0 )
			{
				tot_crc_x = crcNByteCalc( tot_crc_x, tmp, i );
			}
			if( i<32 ) break;
		}
	}
	fclose(fp);
	/////////////////////////////////////////////////////////////////////////
	// CRC Check Time 81KByte->14.7msec
	//PrintTimeCheck( "Check DN CRC File:", &time1 );

	if( info_file != NULL )
	{
		D_Printf( DEBUG_SYSTEM, "[Down] Download File Check CRC %04x : %04x\n", tot_crc_x, crc );

		// info file의 crc와 download 된 파일을 crc 계산 한 것을 서로 비교.
		if( tot_crc_x == crc )
		{
			if( ver!= NULL )
			{
				ver[0] = tmp_ver[0];
				ver[1] = tmp_ver[1];
			}
			//// return tot_crc_x; success 인데, 'tot_crc_x' 값이 음수이면, 결과가 fail 이 됨
			return 1; // 2020.07
		}
		else
		{
			if( ver!= NULL )
			{
				ver[0] = 0xFF;
				ver[1] = 0xFF;
			}
			return -1;
		}	
	}
	else
	{
		D_Printf( DEBUG_SYSTEM, "[Down] Zip file Download File Check\n" );
		return 1;
	}
}

//-------------------------------------------------------------

#define Invalid_Image (1)
#define Image_OK      (0)

#define ONE_Image   (1)     // /tmp/RCU_DN.tar, rcu/dtu/dpd/fpga 단독 downloading, single image
#define Zip_Image   (2)     // == package file is *.zip file
#define Wrong_Image (3)		
static INT8U dn_img_kind = ONE_Image; // RCU,DTU,FPGA @ 5G, rcu(app,dtu,dpd,fpga) 단독인지, zip(==package) 인지.
// dn_img_kind : GUI single, GUI package, ftp package, gREMS package : package image or single image





#define	DEFAULT_FILE_MODE			0644

int Is_File_Exist( char *pull_path_name )
{
	int fd;
	
	fd = open( pull_path_name, O_RDONLY , DEFAULT_FILE_MODE);
	
	if (fd < 0) return -1;

	close(fd);
	
	return 1;
}


//--------------------------------------------------------------------------------------------- 			
//------------------------------------------------------------------


/*
// kill process....
# pgrep -l RCU
# pids=$(pgrep RCU_App) or pids=$(pidof -s RCU_App_1)
# echo $pids
# kill -9 $pids
*/
int Kill_Dig_App(char* app_name)
{
	char cmd[128];
	//int result;
	
	sprintf(cmd, "pids=$(pidof -s %s)", app_name);
	printf("[RCU] %s\n", cmd);
	system(cmd);
	//result = system(cmd);
	/////if(result != 0) return result; // fail
	
	system("echo $pids");
	system("kill -9 $pids");
	
/*
	sprintf(cmd, "kill -9 `pidof %s`", app_name);
	system( cmd );
*/
	return 0;
}



int Chk_Kill_Dig_App(char* app_name)
{
	char cmd[128];
	int result;
	
	sprintf(cmd, "pidof -s %s", app_name);
	printf("[RCU] %s\n", cmd);
	result = system(cmd);
	
	if(result == 1) 
		return RET_SUCCESS; // 1 : No program was found with the requested name.	
	else return RET_Error;
}


/*--------------------------------------------------------
code_image_idx 가 
	RCU Image 외 ( 즉, FPGA, DTU, DPD 의 경우) 다운로드 시, 호출.
	
즉, 
	'code_image_idx'가 Code_Img_FPGA, Code_Img_DTU 일때만.
	(Code_Img_DPD 경우 없음.)
		
	1. file name : tar -tf /tmp/RCU_DN.tar  > /tmp/TAR_ARCHIVE
	2. extract   : tar -xvf /tmp/RCU_DN.tar -C /tmp
	3. run 'dn_copy.sh' for copying files
	4. "touch /tmp/CHANGE_SYS_APP" : 
	5. upgrade_dtu_app_mv.sh /tmp/pru_dtu
	   or 
	   upgrade_dtu_app_mv.sh /tmp/dpd-smp	
--------------------------------------------------------*/
int CopyImage_x( INT8U code_image_idx )
{
	char cmd[128];	

	char *pSrcPath = NULL;
	char *pDstPath = NULL;

	char  file_name[64] = {0};
	int   fd = 0;
	int   file_name_len = 0;

	system( "sync" );

	//--------------------------------------------------------
	if((dn_img_kind == ONE_Image)||(dn_img_kind == Zip_Image))
	{	
		pSrcPath = DN_TEMP_FILE;		
		pDstPath = DN_TEMP_DIR;
		//------------------------------
		// 1. get file name from *.tar
		//------------------------------
		// -t : tar 묶음 풀기전, 묶인 경로를 보여줌.
		sprintf( cmd, "tar -tf %s > %s", pSrcPath, TAR_ARCHIVE); // 압축 파일 아카이브로 이름 찾기.
		system( cmd );
		fd = open(TAR_ARCHIVE, O_RDONLY);
		if( fd < 0 )
			return -1;

		file_name_len = lseek( fd, 0, SEEK_END ) -1 ; // -1 : 맨 뒤에 0x0a가 붙어 있어서,
		lseek( fd, 0, SEEK_SET ); //// 맨 뒤에 0x0a가 붙어 있음.
		read(  fd, file_name, file_name_len );
		close(fd);

		//------------------------------
		// 2. extract image
		//------------------------------		
		sprintf( cmd, "tar -zxvf %s -C %s", pSrcPath, pDstPath ); // 압축 파일 풀기.
		if( system( cmd ) ) 
		{
			sprintf( cmd, "tar -xvf %s -C %s", pSrcPath, pDstPath ); // 묶인 파일 풀기.(압축 x)
			if( system( cmd ) ) return -1;
		}
		D_Printf( DEBUG_SYSTEM, "[DOWN confirm] CopyImage %s \r\n", file_name );
		
		/*
		------------------------------------------------------------
		3. tar 압축 해제하고, 앞에서 구한 /tmp/file_name 을 아래 name으로 copy.
		----------------------
		when fpga : 
			"/usr/tmp/AAU_RU.bit" or "/usr/tmp/PRU_TOP.bit" 에 copy.
			
		when dtu  : 
			"/tmp/pru_dtu" 에 copy.
			touch /tmp/CHANGE_DTU_APP
			chmod +x /tmp/pru_dtu
			upgrade_dtu_app_mv.sh /tmp/pru_dtu

		when dpd  :
			"/tmp/dpd-smp" 에 copy.
			touch /tmp/CHANGE_DTU_APP
			chmod +x /tmp/dpd-smp
			upgrade_dtu_app_mv.sh /tmp/dpd-smp			
		------------------------------------------------------------	
		*/		
		if( code_image_idx == Code_Img_FPGA )
		{
			sprintf( cmd, "cp /tmp/%s %s/%s\n", file_name, TMP_DIR, FPGA_FILE); // 2019.06.28 : DIG_DIR -> TMP_DIR
			if( system( cmd ) ) return -1;	
		}
		else if( code_image_idx == Code_Img_DTU )
		{
		#ifdef __OLD__FS__SYSTEM__
			sprintf( cmd, "cp /tmp/%s %s/%s\n", file_name, DIG_DIR, DTU__FILE);
			if( system( cmd ) ) return -1;	
		#else
			sprintf(cmd, "cp /tmp/%s /tmp/%s", file_name, DTU__FILE);
			system( cmd );
			
			system("touch /tmp/CHANGE_DTU_APP");
			sync();
			
			sprintf(cmd, "chmod +x /tmp/%s", DTU__FILE);
			system( cmd );
			sync();
			
	
			sprintf(cmd, "upgrade_dtu_app_mv.sh /tmp/%s", DTU__FILE);
			if( system( cmd ) ) return -1;
		#endif
		}
		else if( code_image_idx == Code_Img_DPD )
		{
			sprintf(cmd, "cp /tmp/%s /tmp/%s", file_name, DPD__FILE);
			system( cmd );
			
			system("touch /tmp/CHANGE_DTU_APP");
			sync();
			
			sprintf(cmd, "chmod +x /tmp/%s", DPD__FILE);
			system(cmd);
			sync();

	
			sprintf(cmd, "upgrade_dtu_app_mv.sh /tmp/%s", DPD__FILE);
			if( system( cmd ) ) return -1;
		}		
		system( "sync" );
		OSTimeDly( OS_DELAY_1sec );

		/*------------------------------------------------------------
		4. auto-reset or reset-wait에 따라 처리
		----------------------
		auto-reset : 		
			dtu, dpd : kill App
			fpga     : 'Update_Fpga_Bitstream' 호출

		reset-wait  :
		------------------------------------------------------------*/		

		if(m_Env.DnMode.Rcu == _DN_RST_AUTO)
		{
			SetResetInfo( Rst_By_Download, 688 );		

			switch(code_image_idx)
			{
				case Code_Img_DTU:
					if(Chk_Kill_Dig_App(DTU__FILE)==RET_SUCCESS)
						printf( "[RCU] %s is terminated.\n", DTU__FILE);
					else
						printf( "[RCU] %s is exist.\n", DTU__FILE);
					break;
					
				case Code_Img_DPD:
					Kill_Dig_App(DPD__FILE);
					break;
					
				case Code_Img_FPGA:
					if(Update_Fpga_Bitstream() == RET_SUCCESS)
						print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, "FPGA Update Success.");
					else
					{
						print_agent( DEBUG_SYSTEM, VT_Color_Err, "!! FPGA Update Same file OR fail.");
						////m_Env.is_reboot = 0;
						////m_Env.is_down_load_me = 0;
					}
					break;
			}			
		}
		else if(m_Env.DnMode.Rcu == _DN_RST_WAIT_)
		{
		#ifdef __OLD__FS__SYSTEM__
			char  tot_path[128];
			char  tot_path_wait[128];
			char  str_file[32];
			
			switch(code_image_idx)
			{
				case Code_Img_DTU:
				case Code_Img_DPD:
					if(code_image_idx == Code_Img_DTU)
						snprintf(str_file, sizeof(str_file), "%s", DTU__FILE);
					else
						snprintf(str_file, sizeof(str_file), "%s", DPD__FILE);
						
					Make_App_File_Total_Path(DIG_DIR, str_file, NULL,     NULL, tot_path);
					Make_App_File_Total_Path(DIG_DIR, str_file, EXT_Wait, NULL, tot_path_wait);
					sprintf(cmd, "mv -f %s %s; sync", tot_path, tot_path_wait ); 
					system(cmd);
					
					Make_App_File_Total_Path(DIG_DIR, str_file, EXT_CHKSUM, NULL,     tot_path);
					Make_App_File_Total_Path(DIG_DIR, str_file, EXT_CHKSUM, EXT_Wait, tot_path_wait);
					sprintf(cmd, "mv -f %s %s; sync", tot_path, tot_path_wait );
					system(cmd);
					break;		

				default: // Code_Img_FPGA
					return 0;
			}
		#endif
		}
		//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] CopyImage, upgrade_req \r\n" );
	}
	return 0;
}
/*--------------------------------------------------------
RCU Image Only ( 즉, FPGA, DTU, DPD 의 경우 호출 안됨.) 개별 다운로드 시, 호출.

즉, 
'code_image_idx'가 Code_Img_RCU 일때만.


1. extract rcu image : tar -xvf /tmp/RCU_DN.tar -C /tmp
2. run 'dn_copy.sh' for copying files
3. "touch /tmp/CHANGE_SYS_APP"
--------------------------------------------------------*/
int CopyImage( INT8U code_image_idx )
{
	char cmd[128];	

	char *pSrcPath = NULL;
	char *pDstPath = NULL;

	system( "sync" );

	D_Printf( DEBUG_SYSTEM, "[DOWN confirm] CopyImage %d \r\n", code_image_idx );

	//--------------------------------------------------------
	if((dn_img_kind == ONE_Image)||(dn_img_kind == Zip_Image))
	{	
		pSrcPath = DN_TEMP_FILE;	
		pDstPath = DN_TEMP_DIR;

		//------------------------------
		// 1. extract rcu image
		//------------------------------
		sprintf( cmd, "tar -zxvf %s -C %s", pSrcPath, pDstPath ); // 압축 파일 풀기.
		if( system( cmd ) ) // fail
		{
			sprintf( cmd, "tar -xvf %s -C %s", pSrcPath, pDstPath ); // 묶인 파일 풀기.(압축 x)
			if( system( cmd ) ) return -1;
		}
		system( "sync" );
		
		//------------------------------
		// 2. run 'dn_copy.sh' 로 해당 active에 file copy
		//------------------------------		
		D_Printf( DEBUG_SYSTEM, "[DOWN confirm] DN_TEMP_RUN_SH \r\n", code_image_idx );
		if( system( DN_TEMP_RUN_SH ) ) return -1;
		//------------------------------
		// 3. "touch /tmp/CHANGE_SYS_APP"
		//------------------------------	
	#ifdef __OLD__FS__SYSTEM__
	// do nothing
	#else
		system("touch /tmp/CHANGE_SYS_APP");
	#endif	
		////printf("_YCSO_ TEST POINT DN 2\n");		
		system( "sync" );
		OSTimeDly( OS_DELAY_1sec );

		//------------------------------
		// 4. 
		//------------------------------
		if(m_Env.DnMode.Rcu == _DN_RST_AUTO)
		{
			////printf("_YCSO_ TEST POINT DN 2\n");	

			SetResetInfo( Rst_By_Download, 688 );
		}
		else if(m_Env.DnMode.Rcu == _DN_RST_WAIT_)
		{
			if(code_image_idx == Code_Img_RCU) sync();			
		}
		//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] CopyImage, upgrade_req \r\n" );
	}
	return 0;
}



/*
디렉토리이름을 제외한 파일이름만 가져온다.
#include <libgen.h>
char *basename(char *path);

#include <unistd.h>

ssize_t readlink(const char *path, char *buf, size_t bufsiz);
path가 symbolic link일 경우에 symbolic link의 origianl 파일명을 buf에 저장합니다. 
buf에는 null-terminated string으로 저장되지 않기 때문에 
buf를 memset(3)으로 초기화가 필요합니다.

path
     - symbolic link 파일에 대한 상대 path 또는 절대 path
     - 반드시 symbolic link 파일이어야 합니다.
buf
    - path가 가리키고 있는 원본 파일명 또는 디렉토리명을 저장할 buffer
    - buf에 저장될 때에 파일명 또는 디렉토리명은 null-termincated string이 아니므로 
      memset(3)을 하고 사용하는 것이 좋습니다.

*/

//-------------------------------------------------------------

#ifdef __MAKER_ANTS__
int check_zip_DN_flag( void )
{
	/*	// 2019.04.16.deleted
	if( zip_DN_flag_post & 0xE0 )
	{
		if( zip_header.binary_flag == zip_DN_flag_post )
		{
			zip_DN_flag_post = 0;
			return 1;
		}
	}

	else if( zip_DN_flag_post & cbits[Zip_flag_RCU_App] )
	{
		if( zip_header.binary_flag == zip_DN_flag_post )
		{
			zip_DN_flag_post = 0;
			return 2;
		}
	}
	*/
	///// added YCSO 190510
	if(  zip_header.binary_flag & cbits[Zip_flag_PAU] )
	{
		if( (zip_DN_flag_post & cbits[Zip_flag_PAU]) == 0x00 )
		{
			if( amp_DN_status != 0x00 )
			{
				if( amp_DN_flag == amp_DN_confirm )
				{
					set_zip_DN_flag( Zip_flag_PAU );
					syslog( LOG_CRIT, VT_BOLD_GREEN " PAU file confirm success \n" VT_END_CR);
				}
			}
		}
	}

	if(  zip_header.binary_flag & cbits[Zip_flag_PSU] )
	{
		if( (zip_DN_flag_post & cbits[Zip_flag_PSU]) == 0x00 )
		{
			if( psu_DN_status != 0x00 )
			{
				if( psu_DN_flag == psu_DN_confirm )
				{
					set_zip_DN_flag( Zip_flag_PSU );
					syslog( LOG_CRIT, VT_BOLD_GREEN " PSU file confirm success \n" VT_END_CR);
				}

			}
		}
	
	}
	// 2019.04.16.Added
	if( zip_DN_flag_post != 0x00 )
	{
		if( zip_DN_flag_post == zip_header.binary_flag )
		{
			m_St.DL_State[DN_RCU][DL_ST] = DL_St_Rst_Wait; // 2019.09.19

			return 1;
		}
	}
		
	return 0;
}

void set_zip_DN_flag( INT8U flag_num )
{
	calcBits(&zip_DN_flag_post, 1 , flag_num );
}
///// YCSO added START 190515
/*
AMP의 다운로드 시작을 알려 주는 flag
*/
void set_amp_DN_flag( INT8U amp_num, INT8U flag)
{
	pthread_mutex_lock(&dn_wait);

	calcBits(&amp_DN_flag, flag, amp_num);
//	syslog( LOG_CRIT, VT_BOLD_GREEN "_YCSO_ [set_amp_DN_sataus] flag = %x \n" VT_END_CR, amp_DN_flag);
	D_Printf(DEBUG_SYSTEM, " [set_amp_DN_sataus] amp %d flag = %x \n" ,amp_num+1, amp_DN_flag);

	pthread_mutex_unlock(&dn_wait);

}
// 2019.09.20
INT8U check_amp_DN_flag( INT8U amp_num )
{
	if( amp_DN_flag & (0x01 << amp_num) )
		return 1;
	else
		return 0;
}
/*
AMP의 다운로드 상태를 알려주는 flag 1이 되면 완료.
*/
void set_amp_DN_sataus( INT8U amp_num, INT8U flag)
{
	pthread_mutex_lock(&dn_wait);
	calcBits(&amp_DN_status, flag, amp_num);
//	printf("_YCSO_ [set_amp_DN_sataus] flag = %x \n",amp_DN_status);
//	syslog( LOG_CRIT, VT_BOLD_GREEN "_YCSO_ [set_amp_DN_sataus] amp %d status = %x \n" VT_END_CR, amp_num+1,amp_DN_status);
	D_Printf(DEBUG_SYSTEM, " [set_amp_DN_sataus] amp %d status = %x \n" , amp_num+1,amp_DN_status);
	pthread_mutex_unlock(&dn_wait);

}
//// confirm이 제대로 되었는지 확인해 주는 flag
void set_amp_DN_confirm_END( INT8U amp_num, INT8U flag)
{
	pthread_mutex_lock(&dn_wait);

	calcBits(&amp_DN_confirm, flag, amp_num);

	syslog( LOG_CRIT, VT_BOLD_GREEN "AMP DN %d confirm \n" VT_END_CR, amp_num+1 );
	D_Printf(DEBUG_SYSTEM, " [set_amp_DN_sataus] amp %d confirm = %x \n" ,amp_num+1, amp_DN_confirm);
	pthread_mutex_unlock(&dn_wait);

}

void set_psu_DN_flag( INT8U flag)
{
	psu_DN_flag = flag;
//	syslog( LOG_CRIT, VT_BOLD_GREEN "_YCSO_ [set_psu_DN_flag] PSU flag = %x \n" VT_END_CR, psu_DN_flag);
	D_Printf(DEBUG_SYSTEM, " [set_psu_DN_flag] PSU flag = %x \n" , psu_DN_flag);

}
/*
AMP의 다운로드 상태를 알려주는 flag 1이 되면 완료.
*/
void set_psu_DN_sataus( INT8U flag)
{
	psu_DN_status = flag;
//	printf("_YCSO_ [set_amp_DN_sataus] flag = %x \n",psu_DN_status);
//	syslog( LOG_CRIT, VT_BOLD_GREEN "_YCSO_ [set_psu_DN_sataus] PSU status = %x \n" VT_END_CR, psu_DN_status);
	D_Printf(DEBUG_SYSTEM, " [set_psu_DN_sataus] PSU status = %x \n", psu_DN_status);
}

void set_psu_DN_confirm_END( INT8U flag)
{
	psu_DN_confirm = flag;
	syslog( LOG_CRIT, VT_BOLD_GREEN " PSU DN confirm \n" VT_END_CR);
	D_Printf(DEBUG_SYSTEM,  " [set_psu_DN_sataus] confirm = %x \n", psu_DN_confirm);
}

/*
------------------------------------------------------------------
"__USE_MOD_DUALITY__" 에서만 사용됨.

'file_parsing' file을 열어서,  
파일안에 있는 파일 이름을 
file_name 에 copy
------------------------------------------------------------------
*/
int Get_Name_and_Length_of_file(char* file_parsing, char* file_name, int max, int* file_name_length)
{
	int fd;
	int name_length;
	int re = RET_Error;

	fd = open( file_parsing, O_RDONLY);
	
	if( fd < 0 )
	{
		D_Printf( DEBUG_SYSTEM, "[Get_Name_and_Length_of_file] open fail, %s", file_parsing);
		return re;
	}
	
	name_length = lseek( fd, 0, SEEK_END ) - 1; /// 뒤에 EOF가 붙는 듯....

	if(name_length <= 0) 
	{
		*file_name_length = 0;
		re = RET_Error;
	}
	else if(name_length < max)
	{
		*file_name_length = name_length;
		lseek( fd, 0, SEEK_SET );
		read(fd, file_name, name_length );
		re = RET_SUCCESS;
	}
	else 
	{
		*file_name_length = max;
		re = RET_Error;
	}
	
	close(fd);

	return re;
}


//--------------------------------------------------
// return 0 : success
// when file_type is PAU, PSU, Kernel, FileSystem
//--------------------------------------------------
int file_copy( INT8U file_type )
{
#ifdef __USE_MOD_DUALITY__ // module 2원화
	char file_name[256] = {0}; // "/tmp/zip/" 이 포함됨.		
	int file_pull_path_len = 0;
	int result = 0;
	int result2 = 0;	
	char* pFile_PSU;

	// for Kernel, FileSystem
	int file_name_length = 0;	
	int fd;
	INT8U is_psu_giro, is_psu_compatible;

	/*
		file_name[] 에 "/tmp/zip/" 이 포함됨.		
		file_name[sizeof(ZIP_FILES_DIR)] 부터 순수한 file_name Only 
	*/	
	
	switch( file_type )
	{
		case Zip_flag_PAU: // '/usr/app/sys/PAU' directory에 unzip된 2개의 pau file 을 copy. 순수 name도 갱신
			memset(DN_PAU_file_name_NW, 0, sizeof(DN_PAU_file_name_NW));
			memset(DN_PAU_file_name_WE, 0, sizeof(DN_PAU_file_name_WE));
			
			//------------------for new wave pau
			memset(file_name, 0, sizeof(file_name));
			result = Get_Name_and_Length_of_file(PAU_PARSE__NEWWA, file_name, 255, &file_pull_path_len);
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] file_name %s/%d", file_name, file_pull_path_len);	

			if(result == RET_SUCCESS)
			{
				result = update_pau_psu_file( file_name, PAU_PATH ); // *.pau download file을 dir에 copy.
				if( result == RET_SUCCESS )
				{
					DN_PAU_file_name_size = file_pull_path_len - sizeof(ZIP_FILES_DIR);

					if( DN_PAU_file_name_size < 32 )
					{
						memcpy(DN_PAU_file_name_NW, &file_name[sizeof(ZIP_FILES_DIR)], DN_PAU_file_name_size);
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "PAU NewWave File : %s", DN_PAU_file_name_NW);
					}
				}
			}			
			//------------------for wave elec pau
			memset(file_name, 0, sizeof(file_name));
			result2 = Get_Name_and_Length_of_file(PAU_PARSE__WAVEE, file_name, 255, &file_pull_path_len);
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] file_name %s/%d", file_name, file_pull_path_len);	

			if(result2 == RET_SUCCESS)
			{
				result2 = update_pau_psu_file( file_name, PAU_PATH );
				if( result2 == RET_SUCCESS )
				{
					DN_PAU_file_name_size = file_pull_path_len - sizeof(ZIP_FILES_DIR);

					if( DN_PAU_file_name_size < 32 )
					{
						memcpy(DN_PAU_file_name_WE, &file_name[sizeof(ZIP_FILES_DIR)], DN_PAU_file_name_size);
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "PAU WaveEle File : %s", DN_PAU_file_name_WE);
					}
				}
			}
			//------------------------------------
			if((result == RET_SUCCESS)||(result2 == RET_SUCCESS)) 
				result = RET_SUCCESS; 
			else
				result = RET_Error;
			//------------------------------------	
			break;			

		case Zip_flag_PSU:	// '/usr/app/sys/PSU' directory에 unzip된 4개중, 1개의 최종 psu file 을 copy. 순수 name도 갱신
			is_psu_giro = IS__PSU_giRO();
			is_psu_compatible = IS__PSU_Compatible();
			
			if( is_psu_giro || is_psu_compatible ) // ext is '.gps'
			{	
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] ext is *.gps : giro (%d), compatible (%d)", is_psu_giro, is_psu_compatible);	
				
				if(PSU_Info.maker == 0x26) pFile_PSU = PSU_PARSE_AN_GPS; // ants
				else                       pFile_PSU = PSU_PARSE_MC_GPS;
			}
			else // ext is '.psu'
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] ext is *.psu");	
				
				if(PSU_Info.maker == 0x26) pFile_PSU = PSU_PARSE_AN_PSU; // ants
				else                       pFile_PSU = PSU_PARSE_MC_PSU;
			}			
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] PSU_Info.maker %#x (0x26 is ants)", PSU_Info.maker);	
			
		#if 1
			memset(file_name, 0, sizeof(file_name));
			if( Get_Name_and_Length_of_file(PSU_PARSE_AN_GPS, file_name, 255, &file_pull_path_len) == RET_SUCCESS )
				 print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[file_copy] %s [%s]", PSU_PARSE_AN_GPS, file_name);				

			memset(file_name, 0, sizeof(file_name));
			if( Get_Name_and_Length_of_file(PSU_PARSE_MC_GPS, file_name, 255, &file_pull_path_len) == RET_SUCCESS )
				print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[file_copy] %s [%s]", PSU_PARSE_MC_GPS, file_name);		

			memset(file_name, 0, sizeof(file_name));
			if( Get_Name_and_Length_of_file(PSU_PARSE_AN_PSU, file_name, 255, &file_pull_path_len) == RET_SUCCESS )
				print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[file_copy] %s [%s]", PSU_PARSE_AN_PSU, file_name);

			memset(file_name, 0, sizeof(file_name));	
			if( Get_Name_and_Length_of_file(PSU_PARSE_MC_PSU, file_name, 255, &file_pull_path_len) == RET_SUCCESS )
				print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[file_copy] %s [%s]", PSU_PARSE_MC_PSU, file_name);		
		#endif		
			
			memset(file_name, 0, sizeof(file_name));
			result = Get_Name_and_Length_of_file(pFile_PSU, file_name, 255, &file_pull_path_len);
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] file_name %s, len %d", file_name, file_pull_path_len);	
			
			if( result == RET_SUCCESS )
			{
				result = update_pau_psu_file( file_name, PSU_PATH );			
			
				if( result == RET_SUCCESS )
				{
					DN_PSU_file_name_size = file_pull_path_len - sizeof(ZIP_FILES_DIR);

					if( DN_PSU_file_name_size < 32 )
					{
						memset(DN_PSU_file_name, 0, sizeof(DN_PSU_file_name));
						memcpy(DN_PSU_file_name, &file_name[sizeof(ZIP_FILES_DIR)], DN_PSU_file_name_size);
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "PSU File : %s", DN_PSU_file_name);
					}
				}
			}	
			break;

		case Zip_flag_filesystem:
		case Zip_flag_kernel:
			//-------------------------------------
			fd = open( ZIP_FILE_PARSING, O_RDONLY);
			if( fd < 0 )
			{
				D_Printf( DEBUG_SYSTEM, "[file_copy] check_point 1 \r\n");
				return 1;
			}
			lseek( fd, 0, SEEK_SET );
			file_name_length = lseek( fd, 0, SEEK_END ) - 1; /// 뒤에 EOF가 붙는 듯....
			read(fd, file_name, file_name_length );
			close(fd);
			//-------------------------------------
			if(file_type ==  Zip_flag_filesystem)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] Zip_flag : filesystem, %s", file_name);
				result = change_mmcblk_file(file_name, "*emmcfs*", NULL);
				system("touch /emmc/FORMAT_THIS_FILE_SYSTEM");
			}
			else			
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[file_copy] Zip_flag : kernel, %s", file_name);
				result = Update_image_ub(file_name, KERNEL_FILE, KERNEL_FILE); 
			}
			break;			
	
		default :
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[file_copy] invalid file type");
			return 1;
	}
	return result;
#else
	char file_name[256] = {0};
	int file_name_length = 0;
	int fd;
	int result = 0;
	int file_zip_path_size = 0;
	
	fd = open( ZIP_FILE_PARSING, O_RDONLY);

	if( fd < 0 )
	{
		D_Printf( DEBUG_SYSTEM, "[file_copy] check_point 1 \r\n");
		return 1;
	}

	file_name_length = lseek( fd, 0, SEEK_END ) - 1; /// 뒤에 EOF가 붙는 듯....

	lseek( fd, 0, SEEK_SET );
	read(fd, file_name, file_name_length );
	close(fd);
	//-------------------------------------

	file_zip_path_size = sizeof(ZIP_FILES_DIR) + 1; // size for "/tmp/zip/"

	switch( file_type )
	{
		case Zip_flag_PAU:
			result = change_PAU_PSU_file( file_name, PAU_PATH );
			if( result == RET_SUCCESS )
			{
				DN_PAU_file_name_size = file_name_length - file_zip_path_size +1;

				if( DN_PAU_file_name_size < 32 )
				{
//					D_Printf( DEBUG_SYSTEM, "[file_copy] DN_PAU_file_name_size =%d \n",DN_PAU_file_name_size);
					memset(DN_PAU_file_name_NW, 0, sizeof(DN_PAU_file_name_NW));
					memcpy(DN_PAU_file_name_NW, &file_name[file_zip_path_size-1], DN_PAU_file_name_size);
					
					memset(DN_PAU_file_name_WE, 0, sizeof(DN_PAU_file_name_WE));
					memcpy(DN_PAU_file_name_WE, &file_name[file_zip_path_size-1], DN_PAU_file_name_size);
				}
			}
			break;

		case Zip_flag_PSU:
			result = change_PAU_PSU_file( file_name, PSU_PATH );
			if( result == RET_SUCCESS )
			{
				DN_PSU_file_name_size = file_name_length - file_zip_path_size +1;

				if( DN_PSU_file_name_size < 32 )
				{
					memset(DN_PSU_file_name, 0, sizeof(DN_PSU_file_name));
					memcpy(DN_PSU_file_name, &file_name[file_zip_path_size-1], DN_PSU_file_name_size);
				}
			}
			break;

		case Zip_flag_filesystem:
			D_Printf( DEBUG_SYSTEM, "[file_copy] Zip_flag_filesystem = %s\n",file_name);
			result = change_mmcblk_file(file_name, "*emmcfs*", NULL);
			system("touch /emmc/FORMAT_THIS_FILE_SYSTEM");
			break;
			
		case Zip_flag_kernel:
			D_Printf( DEBUG_SYSTEM, "[file_copy] Zip_flag_kernel = %s\n",file_name);
			result = Update_image_ub(file_name, KERNEL_FILE, KERNEL_FILE); 
			break;

		default :
			break;
	}
	return result;
#endif
}

INT8U check_header_crc( void )
{
	INT16U dn_crc = 0;
	INT16U check_crc = 0;

	dn_crc = (INT16U)zip_header.file_type[2];
	dn_crc = (dn_crc<<8) + zip_header.file_type[3];

	check_crc = crcNByteCalc( check_crc, &zip_header.equipment_name[0], sizeof(DN_ZIP_FILE_HEADER)-34 );

	if( dn_crc != check_crc )
	{
		D_Printf( DEBUG_SYSTEM, "[check_header_crc] dn_crc = %x, check_crc = %x, \n", dn_crc, check_crc);
		return 1;
	}

	return 0;
		
}
//////	__YCSO__DN__CONTINUE__ START --->> DN file compare same file
INT8U compare_Header( INT8U* data )
{
	INT8U* p_pre_headr;
	INT8U size = sizeof(DN_ZIP_FILE_HEADER);
	int x, result = 0;

	p_pre_headr = (INT8U*)&pre_DN_file_header;

	for( x = 0; x < size; x++ )
	{
		if( data[x] != p_pre_headr[x] )
		{
			result = 1;
			D_Printf( DEBUG_SYSTEM, "[compare_Header] return point 1 \n"); // 2019.09.20
			return result;
		}
	}
/*
frame 개수에 따른 문제가 생겨 전체 block 비교 하는 부분 추가됨.
__YCSO__DN__CONTINUE__
---> if header same.. but block size not same...
EX) first DN file is 1000 size block. but next DN first block is 2000 size block. --->> not same file. restart DN.
*/
	if( downTotalBlock_continue != downTotalBlock )
	{
		D_Printf( DEBUG_SYSTEM, "[compare_Header] return point 2 \n"); // 2019.09.20
		return 1;
	}

	result = 0;

	return result;
	
}

//////	__YCSO__DN__CONTINUE__ END
/*
parsing hedader of zip file 
update file size,
checking package name,

success : return zip header size
fail    : return 0
*/
INT8U Parsing_Header( INT8U* data )
{
	INT8U zip_header_size = sizeof(DN_ZIP_FILE_HEADER);
	int x = 0;
	INT8U result = 0;
	
	memset( &zip_header, 0, zip_header_size );
	//------------------------------------------
	memcpy( &zip_header.equipment_name[0], &data[0], zip_header_size );

	down_load_file_size = 0;

	// binary_size 4byte을 읽어서 down_load_file_size 에 갱신 : bigendia
	for( x=0; x<4; x++ )
		down_load_file_size += (INT32U)zip_header.binary_size[x] << ((3-x)*8);

	down_load_file_size += MAX_FILE_NAME_SIZE;
	//------------------------------------------	
	
	for( x = 0; x < 12; x++ )
	{
		if( zip_header.equipment_name[x] != PKG_Check_name[x] )
		{
			syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! Pakage File NOT %s file !!!!!!\n" VT_END_CR, zip_header.equipment_name);
			return 0;
		}
	}
	//------------------------------------------
	INT8U check_code[3] = {0x02, 0x2A, 0xAA};
	
	     if( zip_header.repeater_code[0]  != check_code[0] ) result = 1;
	else if( zip_header.repeater_code[1]  != check_code[1] ) result = 1;
	else if( zip_header.manufacturer_code != check_code[2] ) result = 1;
	else result = 0;

	if( result )
	{
		syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! Pakage File header Code error !!!!!!\n" VT_END_CR);
		return 0;
	}
	if( check_header_crc() )
	{
		syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! Pakage File header crc error !!!!!!\n" VT_END_CR);
		return 0;
	}
	return zip_header_size;
}

/*
------------------------------------------------------------
'/tmp/zip' directory에 
'/tmp/DN_file.zip' 압축해제.

return 0 : noraml
return 1 : fail
------------------------------------------------------------
*/
INT8U unzip_Zip_FILE( void )
{
	INT8U result = 0;
	char system_buf[256] = {0};
	struct stat sb; 

	if( stat(ZIP_FILES_DIR, &sb) < 0 )
	{
		mkdir(ZIP_FILES_DIR, 777);
	}

	system("rm -rf /tmp/zip/*");

	memset( system_buf, 0, sizeof(system_buf) );

	sprintf( system_buf, "unzip %s -d %s -o \n",DN_TEMP_FILE_ZiP, ZIP_FILES_DIR );

	result = system( system_buf );

	return result;

}
/*
------------------------------------------------------------

/tmp/DN_file.zip 에서 sha256sum 사용해서, hash file "/tmp/zip_file_hash" 만들고,
읽어서 zip_hash_tmp 에 복사.

zip_hash_tmp 를 zip_header.hash 와 비교.

return 0 : noraml
return 1 : fail
------------------------------------------------------------
*/
int HASH_Check_Zip_FILE( void )
{
	int result = 0;
	INT8U zip_hash_tmp[64] = {0};
	INT8U zip_hash[32] = {0};
	int hash_tmp_fd = 0;
	int x = 0;

	memset( systemCall_buf, 0, sizeof(systemCall_buf) );
	
	sprintf( systemCall_buf, "sha256sum %s > %s\n", DN_TEMP_FILE_ZiP, ZIP_FILE_HASH_TMP );
	
	result = system( systemCall_buf );

	if( result < 0 )
	{
		return 1;
	}

	hash_tmp_fd = open( ZIP_FILE_HASH_TMP , O_RDONLY);
	
	if( hash_tmp_fd < 0 )
	{
		D_Printf( DEBUG_SYSTEM, "[HASH_Check_Zip_FILE] open hash_tmp_fd fail \n");
		return 1;
	}

	read( hash_tmp_fd, zip_hash_tmp, 64 );

	
	for( x = 0; x < 32; x++ )
	{
		zip_hash[x] = (atoh(zip_hash_tmp[x*2]) << 4) + atoh(zip_hash_tmp[(x*2)+1]);

		if( zip_hash[x] != zip_header.hash[x] )
		{
			result = 1;
			break;
		}
	}

	close( hash_tmp_fd );	

	return result;
	
}

/*
------------------------------------------------------------
	rename file /tmp/RCU_DN.tar -> /tmp/DN_File.zip

return 0 : success
return 1 : fail
------------------------------------------------------------	
*/
INT8U check_Zip_FILE( void )
{
	int fd = 0;
	unsigned int bin_size = 0;

	fd = open( DN_TEMP_FILE, O_RDWR );

	if( fd <= 0 )
	{
		D_Printf( DEBUG_SYSTEM, "[DOWN check_Zip_FILE] DN_TEMP_FILE open fail %d \n", fd);
		syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! DN_TEMP_FILE open fail !!!!!!\n" VT_END_CR);
		close(fd);
		
		return 1;
	}

	bin_size = down_load_file_size - MAX_FILE_NAME_SIZE;

	lseek( fd, 0, SEEK_SET );
	ftruncate( fd, bin_size );
	close( fd );
	
	sprintf(systemCall_buf, "mv %s %s\n", DN_TEMP_FILE, DN_TEMP_FILE_ZiP );
	system( systemCall_buf );

	memset( systemCall_buf, 0, sizeof(systemCall_buf) );

	sync();

	return 0;
}

#endif
//////////// ycso  END ===========================================


//-------------------------------------------------------------

int SaveDnDataFile( unsigned char *_Dat, INT16U len, INT8U img_kind )
{
	FILE* fp;
	unsigned int remain_size;
	
	
	if( down_load_recv_size>=down_load_file_size ) return 0; // 파일 다운 종료
	remain_size = down_load_file_size - down_load_recv_size;
	if( len>remain_size ) len = remain_size;
	
	
	down_load_recv_size += len;

	fp = fopen( DN_TEMP_FILE, "ab" );
	if( fp == NULL )
	{
		syslog( LOG_ERR, "DN_TMP_FILE Open Err(4)\n");
		return -1;
	}

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
//-------------------------------------------------------------
// if normal return 0, else return 1.
// zip image 아닌 상태에서, RCU/FGPA/DTU/DPD header check
// 'code_image_idx' 를 update
//-------------------------------------------------------------
*/
unsigned char Check_Data_Image_All(unsigned char *_Dat)
{
	unsigned char i;
	INT8U cnt[Code_Img_Max];
	INT8U max_size = 20; // header 20bytes

	memset(cnt, 0, Code_Img_Max);
	
	for(i = 0; i < max_size; i++) 
	{
		if(RCU_CODE_IMAGE[i] == _Dat[i]) cnt[Code_Img_RCU]++;  // RCU  Code Image New
		if(FPG_CODE_IMAGE[i] == _Dat[i]) cnt[Code_Img_FPGA]++; // FPGA Code Image 
		if(DTU_CODE_IMAGE[i] == _Dat[i]) cnt[Code_Img_DTU]++;  // DTU  Code Image 
		if(DPD_CODE_IMAGE[i] == _Dat[i]) cnt[Code_Img_DPD]++;  // DPD  Code Image 
	}

	for(i = 0; i < Code_Img_Max; i++) 
	{
		if(cnt[i] == max_size)
		{
			code_image_idx = i;
			return Image_OK;
		}
	}
	code_image_idx = Code_Img_Max; // fail
	return Invalid_Image;
}


const unsigned char Zip__Image[4] = { 0x50, 0x4B, 0x03, 0x04 };

/*
----------------------------------------------------
 Downlaod image 1st packet(frame)의 
 Header에 있는 image 종류를 판단.

 'dn_img_kind' 이 RCU 인지, 압축인지, wrong 인지 확인 
 ftp download 에서는 'dn_img_kind' 가 무조건 압축이미지.
---------------------------------------------------- 
Determine 'ONE_Image' or 'Zip_Image'
---------------------------------------------------- 
*/
unsigned char Check_Data_Image( unsigned char *_Dat )
{
	unsigned char i;
	unsigned char zip_invalid = 0;
	unsigned int size = sizeof(DN_ZIP_FILE_HEADER);

	//-----------------------------------------
	// Use "_USE_ZIP_Down_" Always	
	// check if zip-image or not.
	//-----------------------------------------
	D_Printf( DEBUG_SYSTEM, "[ZIP] Image 0x%02X%02X%02X%02X\r\n", _Dat[size+0],_Dat[size+1],_Dat[size+2],_Dat[size+3] );
	
	for( i = 0; i < 4; i++ ) // 4 : zip header
	{
		if( Zip__Image[i] != _Dat[size+i] )
		{
			zip_invalid = 1; // Invalid
		}
	}
	if(zip_invalid == 0) // is zip image
	{
		D_Printf( DEBUG_SYSTEM, "[ZIP] Zip_Image\r\n");
		dn_img_kind = Zip_Image;
		return Image_OK; 
	}
	
	//-----------------------------------------
	// zip image 아님 
	//---------------------------
	// check if rcu-image or not.
	// 20 : 32byte에서 CRC 4BYTE, len 8BYTE 제외
	if(Check_Data_Image_All(_Dat) == Invalid_Image)
	{
		dn_img_kind = Wrong_Image;
		return Invalid_Image;
	}
	else if(SaveInfoFile(&_Dat[20]) < 0) // Chk info file
	{
		dn_img_kind = Wrong_Image;
		return Invalid_Image;
	}
	else
	{
		dn_img_kind = ONE_Image;
		return Image_OK;
	}
}

/////////////ycso START ////////////////////

/*
---------------------------------------------------------------------------
'file_re_name_to_DN' is called 

when file_type is, Only

	Zip_flag_DTU_App
	Zip_flag_RCU_App
	Zip_flag_FPGA

confirm_zip_files() 함수에서 호출.

/tmp/zip/parsing.txt 에 들어있는 실제 RCU/DTU/FPGA file name을 
open 하고, 읽어서, 
32byte header로 file 종류(code_image_idx 를 구하고)
32byte 내용을 체크.

// 32byte header 중 일부에 있는 파일의 size : down_load_file_size 에 update
// 32byte header 중 일부에 있는 파일의 crc  : /tmp/DN_TMP_INFO_FILE 에 update

// 각각의 실제 download file(32byte + 압축파일)에 대해 
header 32byte를 제거 후, /tmp/RCU_DN.tar 로 저장.
/tmp/RCU_DN.tar 에 파일 끝에 있는 file name 만큼을 truncate 함.
---------------------------------------------------------------------------
요약,
package download 시,

file *.RSM (*.DTU, *.FPGA) 의 
	(1) 32byte header check
	(2) file 끝의 file.name 을 truncate
	(3) 중간 tar 를 /tmp/RCU_DN.tar 로 분리함.
---------------------------------------------------------------------------	
*/
INT8U file_re_name_to_DN( INT8U file_type )
{
	int fd_file = 0;
	int fd_parsing = 0;
	int fd_tar = 0;
	INT8U file_name_length = 0;
	char file_name_with_path[128] = {0};
	char cmd[256];
	INT8U check_data[32] = {0};
	INT32U tar_file_len = 0;
	struct stat sb;

	//---------------------------
	switch(file_type)
	{
		case Zip_flag_DTU_App:
		case Zip_flag_RCU_App:
		case Zip_flag_FPGA:
			break;
			
		default:
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[file_re_name_to_DN] invalid file type");
			return 1;
	}
	//---------------------------
	fd_parsing = open( ZIP_FILE_PARSING, O_RDONLY);

	if( fd_parsing < 0 )
	{
		D_Printf( DEBUG_SYSTEM, "[file_re_name_to_DN] check_point 1 \r\n");

		return 1;
	}
	//---------------------------
	file_name_length = lseek( fd_parsing, 0, SEEK_END ) - 1; /// 뒤에 EOF가 붙는 듯....

	lseek( fd_parsing, 0, SEEK_SET );
	read(fd_parsing, file_name_with_path, file_name_length );

	close(fd_parsing);
	//---------------------------

	//sprintf(file_name_with_path , "%s/%s\n",ZIP_FILES_DIR,file_name);

	/////// file in zip open
	fd_file = open(file_name_with_path, O_RDWR);

	D_Printf( DEBUG_SYSTEM, "[file_re_name_to_DN] file_name_with_path = %s \r\n",file_name_with_path);
	
	if( fd_file < 0 )
	{
		D_Printf( DEBUG_SYSTEM, "[file_re_name_to_DN] check_point 2 \r\n");

		return 1;
	}
	
	read( fd_file, check_data, 32); // for reading header 32byte of RCU/DTU/FPGA

	if ( Check_Data_Image_All(check_data) ) // if 32byte header is invalid
	{
		close(fd_file);
		D_Printf( DEBUG_SYSTEM, "[file_re_name_to_DN] check_point 3 \r\n");

		return 1;
	}

	if(SaveInfoFile(&check_data[20]) < 0)
	{
		close(fd_file);
		return Invalid_Image;
	}
	
	if( stat( DN_TEMP_FILE, &sb ) == 0 )
	{
		sprintf(cmd,"rm -rf %s\n", DN_TEMP_FILE);
		system(cmd);
		system("sync");
	}
	close(fd_file);
	

	switch( file_type )
	{
		// 각가그이 download file에 대해 header 32byte를 제거하고, /tmp/RCU_DN.tar 로 저장.
		// /tmp/RCU_DN.tar 에 파일 끝에 있는 file name 만큼을 truncate 함.
		case Zip_flag_DTU_App:
		case Zip_flag_FPGA:
		case Zip_flag_RCU_App:
			sprintf(cmd,"dd if=%s of=%s bs=32 skip=1\n", file_name_with_path, DN_TEMP_FILE);
			system( cmd );
			system( "sync" );

			fd_tar = open(DN_TEMP_FILE, O_RDWR);

			if( fd_tar < 0 )
			{
				D_Printf( DEBUG_SYSTEM, "[file_re_name_to_DN] check_point 2 \r\n");
				break;
			}
			// tar file end에 file name 있음.
			// file name 제거된 tar file size 필요.
			// (full-path-name) 길이 - (디렉토리(/tmp/zip) length + 1('/'))) = 실제 파일이름 길이
			// '실제 파일이름 길이' 는 basename 함수를 쓰는 것이 더 좋을 듯.
			tar_file_len = lseek( fd_tar, 0, SEEK_END );
			tar_file_len = tar_file_len - (file_name_length - strlen(ZIP_FILES_DIR)-1);
			lseek( fd_tar, 0, SEEK_SET );
			ftruncate( fd_tar, tar_file_len );			
			close(fd_tar);
			break;
	}
	return 0;
}

/*
----------------------------------------------------
/tmp/zip directory에서,
unzip 된 파일 중에서, 

zip_header.binary_flag 중,
PAU, PSU 대한 처리가 필요한 경우에 대한 처리
---------------------------------------------------- 
*/

INT8U check_zip_files_PSU_PAU(void)
{
	char cmd[128];
	int result = 0;
	int zip_flag = 0;

	int path4 = 0;

	system("/usr/app_dft/sys/change_upper_lower.sh"); // /tmp/zip 안의 파일이름을 모두 lower character 로.
	system("sync" );

	////D_Printf( DEBUG_SYSTEM, "[DOWN confirm] CopyImage %d \r\n", code_image_idx );

	//--------------------------------------------------------
	if( dn_img_kind == Zip_Image ) // package download
	{
		zip_DN_flag_post = 0;
		
		for( zip_flag = 0; zip_flag < Zip_flag_Max ; zip_flag++ )
		{
			if( zip_header.binary_flag & cbits[zip_flag] )
			{
				result = 1;
				switch(zip_flag)
				{
					case Zip_flag_PSU:
					#ifdef __USE_MOD_DUALITY__ // module 2원화
						sprintf(cmd , "ls %s/*anpsu*.psu > %s\n", ZIP_FILES_DIR, PSU_PARSE_AN_PSU ); system( cmd );
						sprintf(cmd , "ls %s/*mcpsu*.psu > %s\n", ZIP_FILES_DIR, PSU_PARSE_MC_PSU ); system( cmd );
						sprintf(cmd , "ls %s/*anpsu*.gps > %s\n", ZIP_FILES_DIR, PSU_PARSE_AN_GPS ); system( cmd );
						sprintf(cmd , "ls %s/*mcpsu*.gps > %s\n", ZIP_FILES_DIR, PSU_PARSE_MC_GPS ); system( cmd );
					#else
						if(IS___file_ext___psu()) sprintf(cmd , "ls %s/*.psu > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						else                      sprintf(cmd , "ls %s/*.gps > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						system( cmd );						
					#endif
						system( "sync" );
						
						D_Printf(DEBUG_SYSTEM, "[check_zip_files_PSU] Zip_flag : PSU\n" );

						// 2020.02.14 : *.GPS PSU file added
						if(IS__PSU_Compatible()) D_Printf(DEBUG_SYSTEM, "[PSU] IS__PSU_Compatible."); 
						if(IS__PSU_giRO()) D_Printf(DEBUG_SYSTEM, "[PSU] giRO Maker Mode.");

						result = file_copy(Zip_flag_PSU);

						if( result == 0 )
						{
							////////////// 기존 'check_zip_files()' 에서는 아래 없음.
							psu_DN_flag = 0;
							psu_DN_confirm = 0;
							psu_DN_status = 0;
							syslog( LOG_CRIT, VT_BOLD_GREEN " PSU file check success \n" VT_END_CR);
							m_St.DL_State[DN_RCU][DL_ST] = DL_St_Inside_DL_Now;
							//////////////
							info_PSU.upgrade = 1;
						}
						break;
						
					//----------------------
					case Zip_flag_PAU:
					#ifdef __USE_MOD_DUALITY__ // module 2원화						
						sprintf(cmd , "ls %s/*nw*.50a > %s\n", ZIP_FILES_DIR, PAU_PARSE__NEWWA ); system( cmd ); // ANPRUNW35A190101.50A
						sprintf(cmd , "ls %s/*we*.50a > %s\n", ZIP_FILES_DIR, PAU_PARSE__WAVEE ); system( cmd ); // ANPRUWE35A190101.50A
					#else
						sprintf(cmd , "ls %s/*.50a > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING ); system( cmd );						
					#endif							
						system( "sync" );
						D_Printf(DEBUG_SYSTEM, "[check_zip_files_PAU] Zip_flag : AMP\n" );
						
						result = file_copy(Zip_flag_PAU);

						if( result == 0 )
						{
							amp_DN_status = 0;
							amp_DN_flag = 0;
							amp_DN_confirm  = 0; // 기존 'check_zip_files()' 에서는 없음.	
							syslog( LOG_CRIT, VT_BOLD_GREEN " PAU file check success \n" VT_END_CR);
							for(path4 = 0; path4 < AMPU_4_MAX; path4++)
								info_AMP[path4].upgrade = 1;
								
							////////////// 기존 'check_zip_files()' 에서는 아래 없음.	
							m_St.DL_State[DN_RCU][DL_ST] = DL_St_Inside_DL_Now;
							//////////////
						}
						break;					
					//----------------------
					default:
						break;
				}
			}
		}
	}
	return 0;
}



#ifdef __YCSO__TEST__CONFIRM__
// package down reset 대기 timeout 된후 최종 check
// package down confirm 여부 확인.
INT8U confirm_zip_files(void)
{
	char cmd[128];
	int result = 0;
	int zip_flag = 0;

	int path4 = 0;

	system("/usr/app_dft/sys/change_upper_lower.sh");
	system( "sync" );

	////D_Printf( DEBUG_SYSTEM, "[DOWN confirm] CopyImage %d \r\n", code_image_idx );
	

	//--------------------------------------------------------
	if( dn_img_kind == Zip_Image )
	{
		zip_DN_flag_post = 0;
		
		for( zip_flag = 0; zip_flag < Zip_flag_Max ; zip_flag++ )
		{
			if( zip_header.binary_flag & cbits[zip_flag] )
			{
				result = 1;
				
				switch(zip_flag)
				{
					//------------------------------------
					case Zip_flag_PSU:
						if( psu_DN_flag != 0 )
						{
							printf("_YCSO_ [confirm_zip_files] check PSU !!!! \n");
							if( psu_DN_flag == psu_DN_status )
							{
								m_Env.PSU_confirm_DL = _SET_;
								result = 0;		//// added YCSO 190920 // 2019.09.20
							}
						}
						break;
					//------------------------------------
					case Zip_flag_PAU:
						if( amp_DN_flag != 0 )
						{
							printf("_YCSO_ [confirm_zip_files] check PAU !!!! \n");
							if( amp_DN_flag == amp_DN_status )
							{
								for( path4 = 0; path4 < 4; path4++)
									m_Env.PAU_confirm_DL[path4] = _SET_;

								result = 0;		//// added YCSO 190920 // 2019.09.20
							}
						}
						break;
					//------------------------------------
					case Zip_flag_DTU_App:
						sprintf(cmd , "ls %s/*.dtu> %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						system( cmd );
						system( "sync" );
						
						file_re_name_to_DN(Zip_flag_DTU_App);
						D_Printf(DEBUG_SYSTEM, "[confirm_zip_files] Zip_flag : DTU\n" );

						// 이시점에서 '/tmp/RCU_DN.tar' 은 file 끝에 file name도 제거된, 순수한 tar file
						if( CheckDN_File( DN_TEMP_FILE, DN_TEMP_INFO_FILE, NULL ) > 0 )
						{
							if( code_image_idx == Code_Img_DTU )
							{
								result = CopyImage_x(code_image_idx);
								if( result == 0 ) // 2019.09.20
								{
									set_zip_DN_flag(Zip_flag_DTU_App);
									syslog( LOG_CRIT, VT_BOLD_GREEN " DTU file check success \n" VT_END_CR);
								}
							}
						}
						break;
					//------------------------------------	
					case Zip_flag_RCU_App:
						sprintf(cmd , "ls %s/*.rsm > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						system( cmd );
						system( "sync" );
						
						file_re_name_to_DN(Zip_flag_RCU_App);
						D_Printf(DEBUG_SYSTEM, "[confirm_zip_files] Zip_flag : RCU\n" );

						// 이시점에서 '/tmp/RCU_DN.tar' 은 file 끝에 file name도 제거된, 순수한 tar file
						if( CheckDN_File( DN_TEMP_FILE, DN_TEMP_INFO_FILE, NULL ) > 0 )
						{
							if( code_image_idx == Code_Img_RCU )
							{
								result = CopyImage(Code_Img_RCU);
								if( result == 0 ) // 2019.09.20
								{
									set_zip_DN_flag(Zip_flag_RCU_App);
									syslog( LOG_CRIT, VT_BOLD_GREEN " RCU file check success \n" VT_END_CR);
								}
							}
						}
						break;
					//------------------------------------
					case Zip_flag_filesystem:
						sprintf(cmd , "ls %s/*emmcfs* > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						system( cmd );
						system( "sync" );
						
						D_Printf(DEBUG_SYSTEM, "[confirm_zip_files] Zip_flag : filesystem\n" );
						result = file_copy(Zip_flag_filesystem);
						if( result == 0 )
						{
							set_zip_DN_flag(Zip_flag_filesystem);
							syslog( LOG_CRIT, VT_BOLD_GREEN " F/S file check success \n" VT_END_CR);
						}
						break;
					//------------------------------------
					case Zip_flag_kernel:
						sprintf(cmd , "ls %s/*.ub > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						system( cmd );
						system( "sync" );
						
						D_Printf(DEBUG_SYSTEM, "[confirm_zip_files] Zip_flag : kernel\n" );
						result = file_copy(Zip_flag_kernel);
						if( result == 0 )
						{
							set_zip_DN_flag(Zip_flag_kernel);
							syslog( LOG_CRIT, VT_BOLD_GREEN " kernel file check success \n" VT_END_CR);
						}
						break;
					//------------------------------------
					case Zip_flag_FPGA:
						sprintf(cmd , "ls %s/*.fpga > %s\n", ZIP_FILES_DIR, ZIP_FILE_PARSING );
						system( cmd );
						system( "sync" );
						
						D_Printf(DEBUG_SYSTEM, "[confirm_zip_files] Zip_flag : FPGA\n" );
						file_re_name_to_DN(Zip_flag_FPGA);
						
						// 이시점에서 '/tmp/RCU_DN.tar' 은 file 끝에 file name도 제거된, 순수한 tar file
						if( CheckDN_File( DN_TEMP_FILE, DN_TEMP_INFO_FILE, NULL ) > 0 )
						{
							if( code_image_idx == Code_Img_FPGA )
							{
								result = CopyImage_x(code_image_idx);
								if( result == 0 )
								{
									set_zip_DN_flag(Zip_flag_FPGA);
									syslog( LOG_CRIT, VT_BOLD_GREEN " FPGA file check success \n" VT_END_CR);
								}
							}
						}
						break;
					//------------------------------------
					case Zip_flag_reserve:
					default:
						break;
				}

				if( result != 0 )
				{
					D_Printf(DEBUG_SYSTEM, "fail file change %d result = %d\n", zip_flag, result );
					m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920 // 2019.09.20
				}

				D_Printf(DEBUG_SYSTEM, "check_DN_flag %x \n",zip_DN_flag_post  );						
			}
		}
	}
	return 0;
}
#endif


INT8U check_zip_files(void)
{
	return 0;
}
/////////////////////// YCSO END
//-------------------------------------------------------------

///// __YCSO__DN__CONTINUE__ !!! NEED Check make_DN_file_renew function same???????
/*
return 
	0 : *.tar 존재함.
	1 : *.tar 존재하지 않음.
*/
static INT8U make_DN_file_renew( void)
{
	FILE* fp;

	fp = fopen( DN_TEMP_FILE, "wb" );

	if(fp == NULL)
	{		
		// 2019.09.25 : delete fclose()
		return 1; // fail when return 1
	}
	
	fclose( fp ); // 2019.09.25
	return 0;

}

INT16U rxDataDownload( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{
	INT8U result = 0, Start_Inx=0;
	INT16U i;	
	int percent = 0; // init 0
	int re;
	INT8U totPkt = 1, curPkt = 1;
	
	(void)port;
	
	m_Env.is_down_load_me = 1;
	downCurrentBlock = (rxf->data[0]<<8) | rxf->data[1];
	
	if( downCurrentBlock == 0 ) // if 1st packet is received...
	{
		result = Check_Data_Image( &rxf->data[2] );

		if( dn_img_kind == Zip_Image )	///// is PKG file
		{
			result = compare_Header( &rxf->data[2] );			
			
			if( result )	///// __YCSO__DN__CONTINUE__ not same file. init DN status.
			{
				down_addr = 0;
				down_load_recv_size = 0;
				tot_crc = 0;
				downReceivedBlock = 0;
				rxFinishConfirm_cnt = 0;
				D_Printf(DEBUG_SYSTEM, "download check point Block = %d \n", downCurrentBlock );
				downTotalBlock_continue = downTotalBlock;
				result = make_DN_file_renew();	///// DN file RENEW. 
				if( result )					///// DN file RENEW fail. up data is NAK .
				{
					txf->data[0] = 0x01; //// NACK
					txf->data[1] = 0xFF; //Frame high
					txf->data[2] = 0xFF; //Frame low
					D_Printf(DEBUG_SYSTEM, "download check point Block file fail = %d \n", downCurrentBlock );
					return Make_Frame( rxf->Type, rxf->Rcode, txf, 3, rxf->seqNo16, totPkt, curPkt );
				}
			}
			else	////// same DN file. continue DN. 
			{
				txf->data[0] = 0x01; //// NACK
				txf->data[1] = ((downReceivedBlock - 1) >> 8    ); //Frame high
				txf->data[2] = ((downReceivedBlock - 1) & 0xFF  ); //Frame low
				D_Printf(DEBUG_SYSTEM, "download check point Block = %d not new file \n", downCurrentBlock-1 );
				////downReceivedBlock++;
				return Make_Frame( rxf->Type, rxf->Rcode, txf, 3, rxf->seqNo16, totPkt, curPkt );				
			}
		}
		else		//// NOT PKG file. init status 
		{
			down_addr = 0;
			down_load_recv_size = 0;
			tot_crc = 0;
			downReceivedBlock = 0;
			rxFinishConfirm_cnt = 0;
			downTotalBlock_continue = downTotalBlock;
			result = make_DN_file_renew();
		}
	}
	//-----------------------------------------------------------
	if(downReceivedBlock==downCurrentBlock)
	{
		if(downCurrentBlock == 0)
		{
			result = Check_Data_Image( &rxf->data[2] );

			if(dn_img_kind == ONE_Image)
			{
				result = make_DN_file_renew();
				Start_Inx = 32;
			}
			else if( dn_img_kind == Zip_Image )
			{
				Start_Inx = Parsing_Header( &rxf->data[2] );
				if( Start_Inx == 0 ) // zip-file-header parsing fail
					result = 0x01;
				else
					memcpy((INT8U*)&pre_DN_file_header,(INT8U*)&zip_header, sizeof(DN_ZIP_FILE_HEADER));
			}
			else
			{
				D_Printf( DEBUG_SYSTEM, "[rxDataDownload] dn_img_kind error %d \r\n", dn_img_kind);
				txf->data[0] = 0x01; //// NACK
				result = 0x01;
			}
		}

		if( result == 0 )
		{
			i = rxf->len-2-Start_Inx;
			
			re = SaveDnDataFile( &rxf->data[Start_Inx+2], i, dn_img_kind );
			
			if( re ==0 )
			{
				down_addr+=i;
				tot_crc = crcNByteCalc( tot_crc, &rxf->data[2], rxf->len-2 );
				downReceivedBlock++;
			}
			else result = 0x01;
		}
	}
	//-----------------------------------------------------------
	downCurrentBlock = downReceivedBlock-1;
	
	if(downTotalBlock > 0)
	{
		percent = downReceivedBlock*100;
		percent/= downTotalBlock; 
	}

	// 2016. Jan. : REMS Added
	m_St.DL_State[DN_RCU][DL_ST] = DL_St_Now;
	m_St.DL_State[DN_RCU][DL_CNT] = percent;
	
	Proc_ZimImg_Req = 0;
	Proc_ZimImg_Req_Fw_Wait = 0;

	txf->data[0] = result; // ACK
	txf->data[1] = (downCurrentBlock >> 8    ); //Frame high
	txf->data[2] = (downCurrentBlock & 0xFF  ); //Frame low

	return Make_Frame( rxf->Type, rxf->Rcode, txf, 3, rxf->seqNo16, totPkt, curPkt );
}


void Clear_dir__tmp_DN_File(void)
{
	char cmd[32];
	
	sprintf(cmd,"rm -rf %s/*\n", DN_File_Dir);
	system(cmd);
	system("sync");
}

//-------------------------------------------------------------
INT16U rxFinishConfirm( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{
	INT8U result = 0; // 0 is success
	INT16U crc;
	char* info_file = NULL; // init
	INT8U totPkt = 1, curPkt = 1;
	
	(void)port;

	//---------------------------------
	// 'rxFinishConfirm' 함수 연속 2번 금지처리.
	if(rxFinishConfirm_cnt > 0)
		return 0;
	rxFinishConfirm_cnt++;
	//---------------------------------	
	downTotalBlock = (rxf->data[0]<<8) | rxf->data[1];

	if(downTotalBlock!=downReceivedBlock)
	{
		result = 0x01;
	}
	crc = (rxf->data[2]<<8)|rxf->data[3];
	if( tot_crc != crc ) 
	{
		result = 0x01;
		D_Printf( DEBUG_SYSTEM, "[DOWN] crc : rx 0x%X != cal 0x%X\r\n", crc, tot_crc );
	}

	if( result == 0 )
	{
		if( dn_img_kind == ONE_Image ) // single download
		{
			info_file = DN_TEMP_INFO_FILE;

			if( CheckDN_File( DN_TEMP_FILE, info_file, NULL ) > 0 )
			{
				if( code_image_idx == Code_Img_RCU )
					CopyImage(Code_Img_RCU);
				else 
					CopyImage_x(code_image_idx);
			}
			else result = 1;
		}
		else if( dn_img_kind == Zip_Image ) // package download
		{
			DN_TASK_REQ_zip = _SET_; //	system_call_Task 로 처리
		}
		else
		{
			result = 1;
		}
	}

	printf(" [Down] CMD (0x%02X) Mode(%d) \r\n", rxf->Type, m_Env.DnMode.pkg); // 2019.09.20

	/*
	2020.02.24 : NACK이면, 아래 변수 clear
	*/
	if(result != 0) // if NACK
	{
		tot_crc = 0;
		downReceivedBlock = 0;
		memset( &pre_DN_file_header.equipment_name[0], 0, sizeof(DN_ZIP_FILE_HEADER));
		make_DN_file_renew(); // not needed
	}
	else
	{
		m_Eep.AAU_Reset_Cnt_by_Low_Alarm = 0;
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}

	txf->data[0] = result; // ACK
	txf->data[1] = (downReceivedBlock >> 8    ); //Frame high
	txf->data[2] = (downReceivedBlock & 0xFF  ); //Frame low
	txf->data[3] = (tot_crc>>8); //crc high
	txf->data[4] = (tot_crc& 0xFF); //crc low

	Proc_ZimImg_Req = 0;
	Proc_ZimImg_Req_Fw_Wait = 0;

	//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] %d, %d, %d \r\n", result, dn_img_kind, m_Env.DnMode.Rcu );

	if( result == 0 ) // if success
	{		
		m_Env.DnMode.down_load_Size = down_addr;
		
		if( dn_img_kind == Zip_Image )
		{	
			m_Env.is_down_load = 0;
			m_Env.is_down_load_me = 0;			
			
			if(m_Env.DnMode.Rcu == _DN_RST_AUTO)
			{	
				Proc_ZimImg_Req = 1;
				Proc_ZimImg_Req_Fw_Wait = 0;
				////m_St.DL_State[DN_RCU][DL_ST] = DL_St_Rst_Wait; // 2019.09.19

				//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] 11 \r\n" );
			}
			else if(m_Env.DnMode.Rcu == _DN_RST_WAIT_)
			{
				Proc_ZimImg_Req = 0;
				Proc_ZimImg_Req_Fw_Wait = 1;
				////m_St.DL_State[DN_RCU][DL_ST] = DL_St_Complete; // 2019.09.19

				//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] 12 \r\n" );
			}
		}
		else // single download
		{	
			if(m_Env.DnMode.Rcu == _DN_RST_AUTO)
			{
				if(code_image_idx == Code_Img_RCU)
				{
					// temporary skipping
					m_Env.app_exit_req = 1;
				}
				else
				{
				   	SetResetInfo( Rst_By_Download, 1102 );
					m_Env.is_reboot = 1;
					m_Env.is_down_load_me = 1;
				}				

				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Rst_Wait;
				m_St.DL_State[DN_RCU][DL_CNT] = 0;

				//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] 21 \r\n" );
		    }
			else if(m_Env.DnMode.Rcu == _DN_RST_WAIT_)
			{	
				m_Env.is_down_load = 0;
				m_Env.is_down_load_me = 0;
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Complete;

				//D_Printf( DEBUG_SYSTEM, "[DOWN confirm] 22 \r\n" );
			}
		}
	}	
	else // fail
	{
		m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;
	}
	return Make_Frame( rxf->Type, rxf->Rcode, txf, 5, rxf->seqNo16, totPkt, curPkt );
}

//-------------------------------------------------------------
// tx_FW_Update( rxf, txf );
INT16U rxDNconfirm( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port )
{
	INT8U totPkt = 1, curPkt = 1;
	INT8U confirm_FW = 0;
	INT8U wait_min = 0;
	char cmd[256];	
	struct stat sb;
	
	confirm_FW	= rxf->data[0];
	wait_min	= rxf->data[1];

	if( confirm_FW == 0x00 ) // F/W upgrade start
	{
		if( stat( DN_TEMP_FILE_ZiP, &sb ) == 0 )		
		{
			DN_TASK_REQ_Comfirm = _SET_;
			DN_wait_time = wait_min * TICK_CO_MIN;

			// 2019.09.26
			if(DN_wait_time > 255000) DN_wait_time = 255000;
			
			OSTimerReset(&DN_wait_timer);
			txf->data[0] = 0x00; // (0:ACK)

		#ifdef __TYPE__AAU__
			/*
			----------------------------------------------------------
			2020.02.24 : 
				package download & reboot 되기전, 6~7회에 1회꼴로 
				"omap_i2c 44e0b000.i2c: controller timed out" 나오면서, DC alarm 발생.
				F/W 막기위한 방법으로 FW적용 command 내려오면, ADC Disable 하도록 수정.
			----------------------------------------------------------	
			*/
			Set__AAU_ADC_Chk_En(_CLR_);

			m_Eep.AAU_Reset_Cnt_by_Low_Alarm = 0;
			m_Env.is_eep_write = EEP_WRITE_CODE;
		#endif	
		}
		else // if not exist
		{
			txf->data[0] = 0x01; // (1:NACK)
		}
	}
	else if( confirm_FW == 0x01 ) //// upgrade cancle 
	{
		m_Env.PSU_confirm_DL	= 0x02;
		m_Env.PAU_confirm_DL[0] = 0x02;
		m_Env.PAU_confirm_DL[1] = 0x02;
		m_Env.PAU_confirm_DL[2] = 0x02;			
		m_Env.PAU_confirm_DL[3] = 0x02;

		// 2020.04.02
		if( stat( DN_TEMP_FILE_ZiP, &sb ) == 0 ) // if exist
			txf->data[0] = 0x00; // (0:ACK)
		else
			txf->data[0] = 0x01; // (1:NACK)	
	}
	else
	{
		txf->data[0] = 0x01; // (1:NACK)
	}

	/*
	2020.02.24 : clear below
	*/	
	if( confirm_FW == 0x01 )		//// cancle 
	{
		tot_crc = 0;
		downReceivedBlock = 0;
		memset( &pre_DN_file_header.equipment_name[0], 0, sizeof(DN_ZIP_FILE_HEADER));
		make_DN_file_renew();
		
		if(stat( DN_TEMP_FILE_ZiP, &sb ) == 0)
		{
			sprintf(cmd,"rm -rf %s\n", DN_TEMP_FILE_ZiP);
			system(cmd);
			system("sync");
		}
		//-------------------------------
		// 2020.04.02
		//-------------------------------
		zip_DN_flag_post = 0x00;
		DN_TASK_REQ_Comfirm = _CLR_;
		DN_TASK_REQ_zip = _CLR_;	
		DN_TASK_REQ_ftp = _CLR_;		
		memset( &zip_header.equipment_name[0], 0, sizeof(DN_ZIP_FILE_HEADER));			
		m_St.DL_State[DN_RCU][DL_ST] = DL_St_IDLE;		
		//-------------------------------
	}	
	
	return Make_Frame( rxf->Type, rxf->Rcode, txf, 1, rxf->seqNo16, totPkt, curPkt );
}

void RCU_download_Rusume(void)
{	
	INT8U ret_fail;
	
	if( Proc_ZimImg_Req_Fw_Wait )
	{
		Proc_ZimImg_Req_Fw_Wait = 0;
		Proc_ZimImg_Req = 1;
	}
	else
	{	
		ret_fail = system( WAIT_DN_FILE_CHK );
		
		if( ret_fail )
		{
			m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;
			D_Printf( DEBUG_SYSTEM, "[DOWN] Rusume Fail !\n" );
		}
		else
		{
			SetResetInfo( Rst_By_Download, 1152 );
			m_Env.is_reboot = 1;
		}
	}
}

#ifdef __MAKER_ANTS__

/*
static INT8U check_ftp_setting(char *cmd)
{
	if(cmd == NULL )
		return 1;

	if( cmd[0] == '\0')
		return 1;

	return 0;
}
*/

static INT8U* get_words( char* cmd, INT8U *start_ptr,  int max_len)
{
	int x = 0;
	INT32U copy_len = 0;


	for( x = 0; x < max_len; x++)
	{
		if( start_ptr[x] == ',' )
		{
			copy_len = x;
			memcpy( cmd, start_ptr, copy_len);
			break;
		}
	}

	D_Printf( DEBUG_SYSTEM, "[get_words] len %s \n", cmd);

	return 	&start_ptr[x+1];

}

// 2020.04.02
//// YCSO added
int check_ftp_DN_cmd_ACK( void )
{
	int result;

	if( DN_TASK_REQ_ftp == _SET_ )
		return (int)fpt_cmd_sFTP_DL_NACK;

	switch( m_St.DL_State[DN_RCU][DL_ST] )
	{
		case DL_St_IDLE:
		case DL_St_Complete:
		case DL_St_Fail:
			result = ftp_cmd_ACK;
			break;

		default : // under processing..
			result = ftp_cmd_UDP_DL_NACK;
			break;
	}

	return (int)result;
}

void set_ftp_time_t( void )
{
	INT16U year = 0;
	struct tm ftp_tm_st;

	/*	
	---------------------------------------
	년(2byte) ,월,일,시,분
	년은 MSB를 제외한 14Bit만 사용
	2014년12월11일14시10분 
		=> 0x0F5E,0x0c,0x0b,0x0e,0x0a
	---------------------------------------
	*/
	year = (INT16U)ftp_time[0];
	year = year<<7;
	year +=  (INT16U)(ftp_time[1] & 0x7F);
	
	ftp_tm_st.tm_year = year - 1900;
	ftp_tm_st.tm_mon  = ftp_time[2] - 1;
	ftp_tm_st.tm_mday = ftp_time[3];
	ftp_tm_st.tm_hour = ftp_time[4];
	ftp_tm_st.tm_min  = ftp_time[5];
	ftp_tm_st.tm_sec  = 0;
	ftp_tm_st.tm_isdst= 0;

	ftp_dn_time = mktime(&ftp_tm_st);

	printf("sftp: %d-%d-%d %d:%d:%d\n", 
		ftp_tm_st.tm_year+1900, ftp_tm_st.tm_mon+1, ftp_tm_st.tm_mday,	
		ftp_tm_st.tm_hour, ftp_tm_st.tm_min, ftp_tm_st.tm_sec);
}
//// YCSO added 200402 END



INT16U ftp_get_pkg( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip )
{
	char user[32] = {0};
	char password[32] = {0};
	char file[32] = {0};
	char path[32] = {0};
	char ip[32] = {0};
	char ftp_port[8] = {0};
	int result = 0;
	INT8U *start_ptr = NULL;
	INT8U totPkt = 1, curPkt = 1;
	INT16U* pVal16;

/*
	2 byte		Port		접속 Port(Default 21 -> 0x15), 2019.09.04 : ftp-download-port : 1byte -> 2byte
	N Byte	IP Address		접속IP:220.103.235.12 char string => '220.2.2.2'
	1 Byte		','
	N Byte		ID			FTP 접속 ID, char string => 'test'
	1 Byte		','
	N Byte	Passwd			FTP 접속 Passwd => 'test'
	1 Byte		','
	N Byte	File Path		다운로드 파일 Path
	1 Byte		','
	N Byte	파일명			다운로드 파일명(확장자 포함, Max 14)
	1 Byte		','
	6 Byte	다운로드시간	년(2byte) ,월,일,시,분
							년은 MSB를 제외한 14Bit만 사용
							2014년12월11일14시10분 
							=> 0x0F5E,0x0c,0x0b,0x0e,0x0a

○ 접속 ID와 Passwd의 마지막에 ','를 포함시켜 끝을 판단할 수 있게 한다.
○ 접속 ID, Passwd, 다운르도 파일 Path의 길이는 30 byte를 넘어서는 안된다.
							
*/	
	
	// 2019.09.04 : port length 1byte -> 2byte	
	//-------------------------
	pVal16 = (INT16U*)rxf->data; 
	snprintf(ftp_port, 8, "%d", ntohs(*pVal16)); // 2019.09.05		
	start_ptr = &rxf->data[2];	
	//-------------------------		
	start_ptr = get_words(ip		, start_ptr, 32);
	start_ptr = get_words(user		, start_ptr, 32);
	start_ptr = get_words(password	, start_ptr, 32);
	start_ptr = get_words(path		, start_ptr, 32);
	start_ptr = get_words(file		, start_ptr, 32);
	//-------------------------

	ftp_txf_port = port;	

	printf("ftp_get_pkg port = %d\n",port);
	ftp_txf.seqNo16 = rxf->seqNo16;
	memcpy(&ftp_txf.data[1], start_ptr, 6);//// download_time
	memcpy(ftp_time, start_ptr, 6);			//// ftp DN time added 190702 YCSO
	ftp_txf.Type = rxf->Type;
	ftp_txf.Rcode = rxf->Rcode;
	memcpy(&ftp_txf.SUB_ID[0], &rxf->SUB_ID[0], 3);

	zip_DN_flag_post = 0;
/*
	result = check_ftp_setting( ip );
	result = check_ftp_setting( user );	
	result = check_ftp_setting( password );
	result = check_ftp_setting( path );
	result = check_ftp_setting( file );
*/
//// ftpget -u [user] -p [password] -P [port] [ip] [get file name] [path server & file] 

//	sprintf(port, "%d", rxf->data[0]);

//	ptr = strtok(&rxf->data[1], ",");
	

	printf("user: %s \n"	, user			);
	printf("password: %s \n", password		);
	printf("file: %s \n"	, file	);
	printf("path: %s \n"	, path  );
	printf("ip : %s \n"		, ip	);
	printf("port : %s \n"	, ftp_port	);	
//	sprintf(cmd, "ftpget -u %s -p %s -P %s %s /tmp/%s %s/%s\n", user , password, ftp_port, ip, file, path, file);

	// 2020.04.02
	result = check_ftp_DN_cmd_ACK(); ///// YCSO added 200330

	if( result == ftp_cmd_ACK )
	{
		set_ftp_time_t();	//// YCSO added // 2020.04.02

		snprintf(ftp_DN_file, 64 ,"/tmp/%s", file); // 2019.09.05
		memset(DN_cmd, 0, sizeof(DN_cmd));
		/*
		///// YCSO 190808 ftp down --> sftp down 변경.
		///// PRU의 경우 sshpass가 없어서 rootfs 변경과 함께 테스트 필요.
		//	sprintf(DN_cmd, "ftpget -u %s -p %s -P %s %s /tmp/%s %s/%s\n", user , password, ftp_port, ip, file, path, file);
		*/

		/*
		// 2019.09.05
		// 2019.11.29 : timeout 8 -> 30 변경.
		// 2020.04.07 : [sshpass -p %s] -> [sshpass -p '%s'] for special characters
		*/
		snprintf(DN_cmd, 300, "sshpass -p '%s' sftp -P %s -o StrictHostKeyChecking=no -o ConnectTimeout=30 %s@%s:%s/%s /tmp/%s\n", 
			password , ftp_port, user, ip, path, file, file);
			
//		sprintf(DN_cmd, "sshpass -p [password] sftp -P [port] -o StrictHostKeyChecking=no -o ConnectTimeout=10 [user]@[ip]:[path]/[file] /tmp/[file]\n", password , ftp_port, user, ip, path, file, file);
//      sshpass -p user.peta scp -P 22 -o StrictHostKeyChecking=no -o ConnectTimeout=10 user@192.168.0.5:Share/C.test/PRU_TOP.bit /tmp/PRU_TOP.bit
		D_Printf( DEBUG_SYSTEM, VT_YELLOW "[ftp_get_pkg] %s \n", DN_cmd);
		DN_TASK_REQ_ftp = _SET_;
	}

	printf( "ftp_cmd_aCK = %d \n", result);
	
	txf->data[0] = result; // ACK

	return Make_Frame( rxf->Type, rxf->Rcode, txf, 1, rxf->seqNo16, totPkt, curPkt );
}
#endif
//-------------------------------------------------------------



#define MAX_VER_NUM (20)

static LINE_INFO m_Info[MAX_VER_NUM];

int rd_iro_version_info( INT8U is_test )
{
	int fd;
	char* ver_str[MAX_VER_NUM];
	INT8U idx = 0;
	struct stat st;
	int fileSize = -1;
	char *p_buf = NULL;
	int re = -1;
	char *str;
	INT8U max_idx;
	int ver[3];
	

	memset( (INT8U*)&m_ModVer, 0, sizeof(MOD_VER_INFO));
	
	
	
	fd = open(Ver_INFO_PATH, O_RDONLY);
	
	if( fd < 0 )
	{
		syslog( LOG_WARNING, "[iRO] no VerInfo File.\n");
		return re;
	}

	if(stat(Ver_INFO_PATH, &st) == 0)
	{
		fileSize = st.st_size;		
	}

	if(fileSize <= 0)
	{
		syslog( LOG_WARNING, "[iRO] VerInfo size 0.\n");
		return re;
	}
	else
	{
		p_buf = (char *)malloc(fileSize+1);
		
		if (p_buf)
		{			
			memset( p_buf, 0, fileSize+1);
			if( read( fd, p_buf, fileSize ) > 0 )
			{	
				strtok( p_buf, "\r\n" ); // 1st line
				idx = 0;
				do
				{					
					ver_str[idx] = strtok( NULL, "\r\n" );
					
					if(ver_str[idx] == NULL) break;					
					
					idx++;
					
				}while( idx < MAX_VER_NUM );				
			}
			else
			{
				syslog( LOG_WARNING, "[iRO] VerInfo rd fail.\n");
				re = -1;
			}
		}
		else
		{
			syslog( LOG_WARNING, "[iRO] VerInfo malloc fail.\n");
			re = -1;
		}
	}


	max_idx = idx;
	for( idx = 0; idx < max_idx; idx++ )
	{
		//printf("%s\r\n", ver_str[idx]);

		str = strtok(ver_str[idx], ",");  
		//printf("%s\r\n", str );
		strcpy( m_Info[idx].mod, str);
						
		str = strtok(NULL, ",");  
		//printf("%s\r\n", str );
		strcpy( m_Info[idx].made, str);
						
		str = strtok(NULL, ",");
		//printf("%s\r\n", str );	
		sscanf(str, "%d.%d.%d", &ver[0], &ver[1], &ver[2]);
		m_Info[idx].ver[0] = ver[0];
		m_Info[idx].ver[1] = ver[1];
		m_Info[idx].ver[2] = ver[2];
		//printf("v%d.%d.%d\r\n", m_Info[idx].ver[0], m_Info[idx].ver[1], m_Info[idx].ver[2] );

		Zip_Ver_Set( &m_Info[idx] );
	}

	Disp_Cur_All_Mod_Ver();

	if(is_test==0) Cmp_Version();
	
	Disp_Zip_All_Mod_Ver();


	close(fd);
	if (p_buf != NULL) free(p_buf);	

	return re;
}

//------------------------
#define Mod__RCU "RCU"
#define Mod__DTU "DTU_CPU"
#define Mod_FPGA "DTU_FPGA"
#define Mod_L800 "PAU_CDMA/LTE0.8"
#define Mod_L18G "PAU_LTE1.8"
#define Mod_L21G "PAU_WCDMA/LTE2.1"
#define Mod_L26G "PAU_LTE2.6"
#define Mod__PSU "PSU"
//------------------------
#define Made_KST "KST"
#define Made_DSP "DSPONE"
#define Made_KRF "KRF"		// 0x10
#define Made_NWV "NEWWAVE"  // 0x11
#define Made_WvE "WaveElc"	// 0x10
#define Made_EGT "EGT"

void Set_AMP_Zip_Ver( LINE_INFO* pZipInfo, INT8U path4 )
{
	if(memcmp( pZipInfo->made, Made_WvE, sizeof(Made_WvE)) == 0)
	{
		memcpy ( m_ModVer.amp[WEL][path4], pZipInfo->ver, 3);
	}
	if(memcmp( pZipInfo->made, Made_NWV, sizeof(Made_NWV)) == 0)
	{
		memcpy ( m_ModVer.amp[NWV][path4], pZipInfo->ver, 3);
	}
}


void Zip_Ver_Set( LINE_INFO* pZipInfo )
{
	//INT8U path4;
	
	if(memcmp( pZipInfo->mod, Mod__RCU, sizeof(Mod__RCU)) == 0)
	{
		memcpy ( m_ModVer.rcu, pZipInfo->ver, 3);
	}
	if(memcmp( pZipInfo->mod, Mod__PSU, sizeof(Mod__PSU)) == 0)
	{
		memcpy ( m_ModVer.psu, pZipInfo->ver, 3);
	}
	if(memcmp( pZipInfo->mod, Mod__DTU, sizeof(Mod__DTU)) == 0)
	{
		memcpy ( m_ModVer.dtu, pZipInfo->ver, 3);
	}
	if(memcmp( pZipInfo->mod, Mod_FPGA, sizeof(Mod_FPGA)) == 0)
	{
		memcpy ( m_ModVer.fpga, pZipInfo->ver, 3);
	}

	if(memcmp( pZipInfo->mod, Mod_L800, sizeof(Mod_L800)) == 0)
	{	
		Set_AMP_Zip_Ver( pZipInfo, AMPU_4_2 );
	}
	if(memcmp( pZipInfo->mod, Mod_L18G, sizeof(Mod_L18G)) == 0)
	{	
		Set_AMP_Zip_Ver( pZipInfo, AMPU_4_3 );
	}
	if(memcmp( pZipInfo->mod, Mod_L21G, sizeof(Mod_L21G)) == 0)
	{	
		Set_AMP_Zip_Ver( pZipInfo, AMPU_4_1 );
	}
	if(memcmp( pZipInfo->mod, Mod_L26G, sizeof(Mod_L26G)) == 0)
	{	
		Set_AMP_Zip_Ver( pZipInfo, AMPU_4_4 );
	}
}

void Init_Ver_Info(void)
{
	INT8U path4;
		
	memset(&info_Rcu,  0, sizeof(VER_INFO));		
	memset(&info_Dtu,  0, sizeof(VER_INFO));
	memset(&info_FPGA, 0, sizeof(VER_INFO));
		
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		memset(&info_AMP[path4], 0, sizeof(VER_INFO));
	}	
	memset(&info_PSU, 0, sizeof(VER_INFO));
}


void Set_Ver_Info_Rcu( INT8U made, INT8U* ver )
{	
	info_Rcu.made = made;
	memcpy ( info_Rcu.ver, ver, 3);
}

void Set_Ver_Info_PSU( INT8U made, INT8U* ver )
{
	info_PSU.made = made;
	memcpy ( info_PSU.ver, ver, 3);
}




void Set_Ver_Info_AMP(INT8U path4, INT8U made, INT8U* ver)
{
	if(path4 >= AMPU_4_MAX) return;

	info_AMP[path4].made = made;
	memcpy (info_AMP[path4].ver, ver, 3);	
}



void Disp_Cur_All_Mod_Ver(void)
{
	VER_INFO* ptr;	
	INT8U path4;	

	ptr = &info_Rcu;
	D_Printf( DEBUG_SYSTEM, "[Ver] RCU : v%d.%d.%d\r\n", ptr->ver[0],ptr->ver[1],ptr->ver[2]);

	ptr = &info_PSU;
	D_Printf( DEBUG_SYSTEM, "[Ver] PSU : v%d.%d.%d\r\n", ptr->ver[0],ptr->ver[1],ptr->ver[2]);
	
	ptr = &info_Dtu;
	D_Printf( DEBUG_SYSTEM, "[Ver] DTU : v%d.%d.%d\r\n", ptr->ver[0],ptr->ver[1],ptr->ver[2]);

	ptr = &info_FPGA;
	D_Printf( DEBUG_SYSTEM, "[Ver] FPGA: v%d.%d.%d\r\n", ptr->ver[0],ptr->ver[1],ptr->ver[2]);

	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		ptr = &info_AMP[path4];
		D_Printf( DEBUG_SYSTEM, "[Ver] AMP : v%d.%d.%d ( %s made 0x%02X)\r\n",\
			ptr->ver[0],ptr->ver[1],ptr->ver[2],\
			Get_path4_Str(path4), ptr->made );
	}	
}

void Disp_Zip_All_Mod_Ver(void)
{	
	MOD_VER_INFO* ptr;	
	INT8U path4;
	INT8U made;

	ptr = &m_ModVer;
	
	D_Printf( DEBUG_SYSTEM, "[Ver] zip RCU : v%d.%d.%d\r\n", ptr->rcu[0],ptr->rcu[1],ptr->rcu[2]);
	D_Printf( DEBUG_SYSTEM, "[Ver] zip PSU : v%d.%d.%d\r\n", ptr->psu[0],ptr->psu[1],ptr->psu[2]);
	D_Printf( DEBUG_SYSTEM, "[Ver] zip DTU : v%d.%d.%d\r\n", ptr->dtu[0],ptr->dtu[1],ptr->dtu[2]);
	D_Printf( DEBUG_SYSTEM, "[Ver] zip FPGA: v%d.%d.%d\r\n", ptr->fpga[0],ptr->fpga[1],ptr->fpga[2]);


	for(made = 0; made < 2; made++)
	{
		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		{
			D_Printf( DEBUG_SYSTEM, "[Ver] zip AMP(%d) %s : v%d.%d.%d \r\n", made, Get_path4_Str(path4), \
			ptr->amp[made][path4][0],ptr->amp[made][path4][1],ptr->amp[made][path4][2]);
		}
	}	
}

INT8U Is_InValid_Version( INT8U* ver )
{
	if((ver[0] == 0) && (ver[1] == 0) && (ver[2] == 0)) 
		 return 1;
	else return 0;
}

void Upgrade_Decision( INT8U* zipVer, VER_INFO* pInfo )
{
	if( Is_InValid_Version( zipVer ) ) return;
	
	if( memcmp( zipVer, pInfo->ver, 3)) // ver changed
	{
		pInfo->upgrade = 1;
	}
}

void Cmp_Version(void)
{
	INT8U path4;
	INT8U amp_made;

	// init 
	info_Rcu.upgrade = 0; 
	info_PSU.upgrade = 0; 	
	info_Dtu.upgrade = 0;
	info_FPGA.upgrade = 0;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		info_AMP[path4].upgrade = 0;
	}
	
	
	Upgrade_Decision( m_ModVer.rcu, &info_Rcu );
	if(info_Rcu.upgrade) D_Printf( DEBUG_SYSTEM, "[Ver]  Upgrade RCU.\r\n");

	if(info_Rcu.upgrade == 0) // RCU 먼저 reset 되게 하기 위함.
	{	
		Upgrade_Decision( m_ModVer.psu, &info_PSU );
		if(info_PSU.upgrade) D_Printf( DEBUG_SYSTEM, "[Ver]  Upgrade PSU.\r\n");
					
		Upgrade_Decision( m_ModVer.dtu, &info_Dtu );
		if(info_Dtu.upgrade) D_Printf( DEBUG_SYSTEM, "[Ver]  Upgrade Dtu.\r\n");
		
		Upgrade_Decision(m_ModVer.fpga, &info_FPGA);
		if(info_FPGA.upgrade) D_Printf( DEBUG_SYSTEM, "[Ver]  Upgrade FPGA.\r\n");

		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		{
			     if(info_AMP[path4].made == Code_WvE) amp_made = WEL;
			else if(info_AMP[path4].made == Code_NWV) amp_made = NWV;
			else break;
			
			Upgrade_Decision(m_ModVer.amp[amp_made][path4], &info_AMP[path4] );
			
			if(info_AMP[path4].upgrade) D_Printf( DEBUG_SYSTEM, "[Ver]  Upgrade AMP(%s).\r\n",\
				Get_path4_Str(path4));
		}		
	}
}



#define CS_AMP "CSIDR"
#define GS_AMP "GSIDR"

#define KRF_AMP "KR"
#define NWV_AMP "NW"

#define LTE08_AMP "CLP"
#define LTE18_AMP "LAP"
#define LTE21_AMP "WLE"
#define LTE26_AMP "ELA"

// AID 0x0100
#define RPT_MAKER_CS (0x0A)
#define RPT_MAKER_GS (0x0B)

// make pull-path-file-name
int Get_RCU_Dn_File_Name( INT8U rpt_maker, char* fileName, INT8U* ver )
{
	char* str_rpt_maker;

	     if( rpt_maker == RPT_MAKER_CS ) str_rpt_maker = "CSIDR";
	else if( rpt_maker == RPT_MAKER_GS ) str_rpt_maker = "GSIDR";
	else return -1;
		
	sprintf( fileName, "%s/%s%02d%02d%02d.RSM", UnZip_DIR_RCU, str_rpt_maker, ver[0], ver[1], ver[2] );
	return 1;
}


// make pull-path-file-name
int Get_PSU_Dn_File_Name( char* fileName )
{
	sprintf( fileName, "%s/", PSU_PATH);
	strncat( fileName, DN_PSU_file_name, DN_PSU_file_name_size );

	print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Get_PSU_Dn_File_Name] %s", fileName);

	return 1;
}


int Get_AMP_Dn_File_Name( INT8U amp_maker, char* fileName)
{
	sprintf( fileName, "%s/", PAU_PATH);
	
	switch( amp_maker ) // maker_hex
	{
		case Code_NWV: 
			strncat( fileName, DN_PAU_file_name_NW, DN_PAU_file_name_size );			
			break;
			
		case Code_WvE: 
			strncat( fileName, DN_PAU_file_name_WE, DN_PAU_file_name_size );
			break;
			
		default:
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Get_AMP_Dn_File_Name] invalid amp maker");
			return -1;		
	}	

	if(fileName[0] == 0) 
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Get_AMP_Dn_File_Name] invalid file name");
		return -1;		
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Get_AMP_Dn_File_Name] %s", fileName);
		return 1;
	}
}



// RCU Reset 후, multidown 필요한 지 check
typedef struct
{
	INT8U en;
	INT8U dly_start;
	INT8U reserve[2];
	OS_TIMER dly_timer;	
}M_DN_CHK;

//static M_DN_CHK m_MultiDnChk;

/*
압축 다운로드로 rcu reset 후, amp/psu 개별 다운로드를 위해
일정 시간 후, version 정보 파일 읽어서,
amp/psu 개별 다운로드를 결정.
*/
/*
void Init_MultiDnChk(void)
{	
	m_MultiDnChk.en = 0;
	OSTimerReset( &m_MultiDnChk.dly_timer );

	if(m_Eep.MultiDown_Need == MUTI_DOWN_NEED)
	{
		m_MultiDnChk.en = 1;

		m_Eep.MultiDown_Need = 0;
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}
}

void Proc_MultiDnChk(void)
{
	INT32U elapseTime;
	
	if(m_MultiDnChk.en)
	{
		elapseTime = OSTimerGet_msec(&m_MultiDnChk.dly_timer);
		if(elapseTime >= 30000) // 30[s]
		{
			m_MultiDnChk.en = 0;
			rd_iro_version_info(0);
		}
		else
		{
			D_Printf( DEBUG_SYSTEM, "[Multi] %d \r\n", elapseTime);
		}
	}
	else
	{
		m_MultiDnChk.en = 0;
		OSTimerReset( &m_MultiDnChk.dly_timer );
	}
}
*/
int check_PSU_PAU_DN(void) //// added YCSO 190920
{
	int re = 0;
	int check_pase = 0;

	if( zip_header.binary_flag & cbits[Zip_flag_PAU] )
		check_pase |= cbits[Zip_flag_PAU] ;
	
	if( zip_header.binary_flag & cbits[Zip_flag_PSU] )
		check_pase |= cbits[Zip_flag_PSU] ;

	switch( check_pase )
	{
		case 0x02: //// PSU ONLY
			if( psu_DN_flag != 0 )
			{
				if( psu_DN_flag == psu_DN_status )
					re = 1;
			}
			break;
			
		case 0x04: //// PAU ONLY
			if( amp_DN_flag != 0 )
			{
				if( amp_DN_flag == amp_DN_status )
					re = 1;
			}
			break;

		case 0x06: //// PSU PAU 
			if( ( amp_DN_flag != 0 ) && ( psu_DN_flag != 0 ) )
			{
				if( ( amp_DN_flag == amp_DN_status ) && ( psu_DN_flag == psu_DN_status ))
					re = 1;
			}
			break;

		default:
			break;
	}

	return re;

}

// 2020.04.02
//// YCSO added 200402
INT8U check_ftp_time( void )
{
	time_t  time_now;
	double diff_time;

	time(&time_now);

	diff_time = difftime(time_now , ftp_dn_time );

	if( diff_time < 0 )
		return 0;

	else
		return 1;

}
//// YCSO added END

//// added YCSO 190920 // 2019.09.20
void DL_St_Fail_handle( void )
{
	char system_buf[256] = {0};
	struct stat sb[2];

	if( stat(ZIP_FILES_DIR, &sb[0] ) == 0 )
	{
		snprintf(system_buf, 256, "rm -rf %s", ZIP_FILES_DIR);
		system(system_buf);
		sync();
	}
	
	if( stat(DN_TEMP_FILE_ZiP, &sb[1] ) == 0 )
	{
		snprintf(system_buf, 256, "rm -rf %s", DN_TEMP_FILE_ZiP);
		system(system_buf);
		sync();
	}

	DN_TASK_REQ_zip = _CLR_;
	DN_TASK_REQ_Comfirm = _CLR_;
	DN_TASK_REQ_ftp = _CLR_;
	downTotalBlock_continue = 0;
	memset( &zip_header.equipment_name[0], 0, sizeof(DN_ZIP_FILE_HEADER));
//	memset( &pre_DN_file_header.equipment_name[0], 0, sizeof(DN_ZIP_FILE_HEADER));

}

void* system_call_Task(void *arg)
{
	Thread_ARG_Var* p_th_arg;	
	
	p_th_arg = (Thread_ARG_Var*)arg;
 	Thread_Priority_Init(p_th_arg, 1);

	int result = 0;
	int fd = 0;
	INT8U Start_Inx = 0;
	INT8U zip_header_ftp[64] = {0};
	char system_cmd[128] = {0};
	INT8U check_zip_reset = 0;
	INT16U len = 0;
	INT8U totPkt = 1, curPkt = 1;
	INT8U version_cnt = 0;
	static INT8U version_check = 1;
	INT8U serial[10] = {0};
	static INT8U downloading_flag = 0;		//// added YCSO 190920 // 2019.09.20


	result = pthread_mutex_init(&dn_wait, NULL);
	if (result != 0) 
	{
		syslog(LOG_ERR, "[RCU] System_Call Mutex Init Fail.");
		exit(EXIT_FAILURE);
	}
	
	//---------------------
	// 2020.01.09
	#ifdef __TYPE__PRU__
	m_Env.CPRI_CLK_Alm_Mask_at_Reboot = 1;
	print_agent( DEBUG_SYSTEM, VT_BOLD_RED, "!! START of CPRI/CLK Alarm Masking at_Reboot....\n");
	#endif
	//---------------------
	
	while( m_Env.task_flag[_TASK_SYSTEM_CALL_] )		///// YCSO TASK END
	{
		OSTimeDly( OS_DELAY_500msec );
		Update__Task_Count(_TASK_SYSTEM_CALL_);
		//------------------------------------------------------------
		if( version_check )
		{
			if( version_cnt >= 60 )////// 10 --> 60 YCSO 19.06.05
			{
				version_check = 0;
				
			#ifdef __TYPE__PRU__ // 2020.01.09
				m_Env.CPRI_CLK_Alm_Mask_at_Reboot = 0;
				print_agent( DEBUG_SYSTEM, VT_BOLD_GREEN, "!! END of Masking CPRI/CLK Alarm....\n");
			#endif
				
				/*
				PRU[Serial Number][Cascade ID]-version info. DTU App [xx.xx.xx], FPGA [xx.xx.xx] PSU [xx.xx] 
				PAU0 [xx.xx] PAU1 [xx.xx] PAU2 [xx.xx] PAU3 [xx.xx]
				Serial Number][Cascade ID]-version info. DTU App [xx.xx.xx], FPGA [xx.xx.xx]m_PSU.PSU_I_Ver
				*/
			#ifdef __TYPE__PRU__
				memcpy(serial, m_St.LS_Regist_Num, 10);
				printf("PRU[%s][%d]-vsersion info. ", serial			, AufoConfigFlag.RE_Cascade_ID	);
				printf("RCU App [%d.%d.%d] ", m_St.RCU_Ver[0]			, m_St.RCU_Ver[1]				, m_St.RCU_Ver[2]);				
				printf("DTU App [%d.%d.%d] ", m_St.d_SW_VER_DTU[0]		, m_St.d_SW_VER_DTU[1]			, m_St.d_SW_VER_DTU[2]);
				printf("FPGA [%d.%d.%d] ", m_St.d_SW_VER_FPGA[0]		, m_St.d_SW_VER_FPGA[1]			, m_St.d_SW_VER_FPGA[2]);
				printf("PSU [%d.%d.%d] ", m_PSU.PSU_I_Ver[0]			, m_PSU.PSU_I_Ver[1]			, m_PSU.PSU_I_Ver[2]);
				printf("PAU1 [%d.%d.%d] ", m_St.PAU_I_Ver[AMPU_4_1][0]	, m_St.PAU_I_Ver[AMPU_4_1][1]	, m_St.PAU_I_Ver[AMPU_4_1][2]);
				printf("PAU2 [%d.%d.%d] ", m_St.PAU_I_Ver[AMPU_4_2][0]	, m_St.PAU_I_Ver[AMPU_4_2][1]	, m_St.PAU_I_Ver[AMPU_4_2][2]);
				printf("PAU3 [%d.%d.%d] ", m_St.PAU_I_Ver[AMPU_4_3][0]	, m_St.PAU_I_Ver[AMPU_4_3][1]	, m_St.PAU_I_Ver[AMPU_4_3][2]);
				printf("PAU4 [%d.%d.%d] \n", m_St.PAU_I_Ver[AMPU_4_4][0], m_St.PAU_I_Ver[AMPU_4_4][1]	, m_St.PAU_I_Ver[AMPU_4_4][2]);				
			#else
				memcpy(serial, m_St.LS_Regist_Num, 10);
				printf("RCU App [%d.%d.%d] ", m_St.RCU_Ver[0]		 , m_St.RCU_Ver[1]		, m_St.RCU_Ver[2]);				
				printf("DTU App [%d.%d.%d] ", m_St.d_SW_VER_DTU[0]	 , m_St.d_SW_VER_DTU[1]	, m_St.d_SW_VER_DTU[2]); // 2020.04.02
				printf("FPGA	[%d.%d.%d] \n", m_St.d_SW_VER_FPGA[0], m_St.d_SW_VER_FPGA[1], m_St.d_SW_VER_FPGA[2]);
			#endif
			}
			else
			{
				version_cnt++;
			}
		}
		//------------------------------------------------------------
		if( DN_TASK_REQ_ftp )
		{	
			if( check_ftp_time() )	//// YCSO added // 2020.04.02
			{
				printf( VT_YELLOW "[DN_TASK_REQ_ftp] FTP cmd START \n");  // 2019.09.19
				
				system( "rm -rf /emmc/etc/ssh/.ssh/*");
				sync();									
				
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Now;	///// added YCSO 2020.04.02
	
				result = system( DN_cmd );
				printf( VT_YELLOW "[DN_TASK_REQ_ftp] FTP cmd END \n");  // 2019.09.19
								
				downloading_flag = _SET_;  // 2019.09.20

				if( result == 0)
				{
					D_Printf( DEBUG_SYSTEM, VT_YELLOW "[DN_TASK_REQ_ftp] FTP GET : success & finish %s\n", ftp_DN_file);
					dn_img_kind = Zip_Image;
	
					fd = open(ftp_DN_file, O_RDONLY);
					if( fd < 0 )
					{
						D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[DN_TASK_REQ_ftp] fail opne %s\n", ftp_DN_file);
						DN_TASK_REQ_ftp = _CLR_;
						result = RET_Error;
					}

					if( result == RET_SUCCESS )
					{
						memset( zip_header_ftp, 0, sizeof(zip_header_ftp));
	
						read(fd, zip_header_ftp, sizeof(zip_header_ftp));
						close(fd);
						
						Start_Inx = Parsing_Header( zip_header_ftp );
	
						if( Start_Inx == sizeof(DN_ZIP_FILE_HEADER))
						{
							sprintf(system_cmd,"dd if=%s of=%s bs=64 skip=1\n", ftp_DN_file, DN_TEMP_FILE);
							result = system( system_cmd );
	
							if( result == 0 )
							{
								DN_TASK_REQ_zip = _SET_;
								m_Env.DnMode.pkg = _DN_RST_WAIT_; ///// DN 후 wait 하도록 변경.// 2019.07.03 : scyo
							}
							else result = RET_Error;
						}
						else result = RET_Error;
					}
					// 2019.07.03 : scyo
					if( result == RET_SUCCESS )
					{
						ftp_txf.data[0] = 0x00;						
					}
					else
					{
						ftp_txf.data[0] = 0x01;
						m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;
						ftp_txf.data[7] = gREMS_Err_FTP_Conn;
					}
				}
				else
				{
					ftp_txf.data[0] = 0x01;
					result = result/256;
					////printf("sftp result = %d \n",result);
					
					if( result == 1 )
						ftp_txf.data[7]	= gREMS_Err_FTP_File;
					else
						ftp_txf.data[7]	= gREMS_Err_FTP_Conn;
	
					m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920  // 2019.09.20
	
					////SendError(PROTOCOL_FRAME * txf, INT8U Rcode, INT8U err, INT8U port)
				}

				memcpy(&ftp_txf.data[1], ftp_time, 6);
				
				// 2019.09.04 : DOWNLOAD 완료(Device -> EMS) : 0x52 -> 0x51
				len = Make_Frame_COMPACKT(CMD2_FTP_COMPLETE, ftp_txf.Rcode, &ftp_txf, 8, ftp_txf.seqNo16, totPkt, curPkt);
				hex_dump((unsigned char * )&ftp_txf, len );
				////printf("_YCSO_FTP_Check port = %d \n",ftp_txf_port);
				
				Send_Tx_Only_Data(ftp_txf_port, (INT8U *)&ftp_txf, len);
				/*
				//if( r_ip )					
				//	Send_UDP_Data_GUI(ftp_txf_port, INT8U * dst_ip, INT8U * buf, INT16U len, INT16U port16)
				//Send_UP_Data( ftp_txf_port, (INT8U *)&ftp_txf, len);
				*/
	 		    ////SendData(ftp_txf_port, INT8U * send,len, INT8U * ack, INT16U delay)
				
				DN_TASK_REQ_ftp = _CLR_;
			}
			else // 2020.04.02
			{
				OSTimeDly( OS_DELAY_500msec );
			}
		}
		//------------------------------------------------------------
		if( DN_TASK_REQ_zip )
		{
			downloading_flag = _SET_;		//// added YCSO 190920  // 2019.09.20
			DN_TASK_REQ_Comfirm = _CLR_;

			//// added YCSO 190920 START  // 2019.09.20
			down_addr = 0;
			down_load_recv_size = 0;
			tot_crc = 0;
			downReceivedBlock = 0;
			rxFinishConfirm_cnt = 0;
			memset( &pre_DN_file_header.equipment_name[0], 0, sizeof(DN_ZIP_FILE_HEADER));
			//// added YCSO 190920 END  // 2019.09.20
			
			m_St.DL_State[DN_RCU][DL_ST] = DL_St_Compression_Check;

			if( check_Zip_FILE() > 0 )	/// tar로 저장된 파일을 zip으로 바구고, file name을 가져온다.
			{
				D_Printf( DEBUG_SYSTEM, "[DOWN rxFinishConfirm] check_Zip_FILE fail \n");
				syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! worng ZIP file !!!!!!\n" VT_END_CR);

				DN_TASK_REQ_zip = _CLR_;
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920  // 2019.09.20

				continue;				
			}

			if( HASH_Check_Zip_FILE() > 0 )
			{
				D_Printf( DEBUG_SYSTEM, "[DOWN rxFinishConfirm] HASH_Check_Zip_FILE fail \n");
				syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! file HASH error !!!!!!\n" VT_END_CR);
				DN_TASK_REQ_zip = _CLR_;
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920 // 2019.09.20

				continue;
			}

			m_St.DL_State[DN_RCU][DL_ST] = DL_St_Compression_Now;

			if( unzip_Zip_FILE() > 0 )
			{
				D_Printf( DEBUG_SYSTEM, "[DOWN rxFinishConfirm] unzip_Zip_FILE fail \n");
				syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! file unzip fail !!!!!!\n" VT_END_CR);
				DN_TASK_REQ_zip = _CLR_;
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;		//// added YCSO 190920 // 2019.09.20
				
				continue;				
			}
			
		#ifdef __YCSO__TEST__CONFIRM__
			check_zip_files_PSU_PAU(); // 'DL_St_Inside_DL_Now' 처리를 위해.			
		#else
			if( check_zip_files( ) > 0 )
			{
				D_Printf( DEBUG_SYSTEM, "[DOWN rxFinishConfirm] PSU/PAU file down fail \n");
				syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! file update fail !!!!!!\n" VT_END_CR);
				DN_TASK_REQ_zip = _CLR_;
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Fail;

				continue;
			}			
		#endif
		
			DN_TASK_REQ_zip = _CLR_;
		
			if(m_Env.DnMode.pkg == _DN_RST_AUTO)
			{
				DN_TASK_REQ_Comfirm = _SET_;
				DN_wait_time = 0;
				////printf(VT_BOLD_GREEN "\n!!!!!! check point pkg auto!!!!!!\n" VT_END_CR);
			}
		}
		//------------------------------------------------------------
	#ifdef __YCSO__TEST__CONFIRM__
		if( DN_TASK_REQ_Comfirm ) // package download 진행요청(package down 즉시 or 0x5b Command)
		{
			if( zip_header.binary_flag & cbits[Zip_flag_PSU] )
			{
				if( psu_DN_flag == 0x00 || psu_DN_flag != psu_DN_status ) // psu download 중.
					continue;
			}

			if( zip_header.binary_flag & cbits[Zip_flag_PAU] )
			{
				if( amp_DN_flag == 0x00 || amp_DN_flag != amp_DN_status ) // pau download 중.
					continue;
			}

			if( OSTimerGet_msec(&DN_wait_timer) > DN_wait_time )
			{
				if ( confirm_zip_files() > 0 ) // return value always 0
				{
					////////
				}
				else
				{
					DN_TASK_REQ_Comfirm = _CLR_;
				}
			}
			else // reset 대기 상태
			{
				m_St.DL_State[DN_RCU][DL_ST] = DL_St_Rst_Wait; 
				m_St.DL_State[DN_RCU][DL_CNT] = DN_wait_time/1000; // [ms] -> [s]
			}
		}
		else  // 2019.09.19 : package download 적용이 내려오기 이전 상태.
		{
			if( m_St.DL_State[DN_RCU][DL_ST] != DL_St_IDLE)
			{
			#ifdef __TYPE__PRU__
				if( m_St.DL_State[DN_RCU][DL_ST] == DL_St_Inside_DL_Now )
				{
					if( check_PSU_PAU_DN() ) //// psu pau check  // 2019.09.19
						m_St.DL_State[DN_RCU][DL_ST] = DL_St_Complete;
				}
			#endif
				if( m_St.DL_State[DN_RCU][DL_ST] == DL_St_Compression_Now )
					m_St.DL_State[DN_RCU][DL_ST] = DL_St_Complete;

				//// added YCSO 190920  // 2019.09.20
				if( (m_St.DL_State[DN_RCU][DL_ST] == DL_St_Fail) && (downloading_flag == _SET_) )
				{
					DL_St_Fail_handle();
					downloading_flag = _CLR_;
				}
			}
		}
	#endif

		check_zip_reset = check_zip_DN_flag();
		if( check_zip_reset )
		{
			D_Printf( DEBUG_SYSTEM, "[RCU] Reset TEST \n" );
			if( check_zip_reset == 1 )
			{
				OSTimeDly( OS_DELAY_1sec );

				///// reset이 아닌 psu reset으로 변경해 달라는 요청에 따른 변경.
			#ifdef __TYPE__PRU__
				m_Env.psu_reset_req = 1; // 2019.04.17: psu reset으로 변경요청.
			#else // AAU for reboot after download
				m_Env.is_reboot = 1;
			#endif
				SetResetInfo( Rst_By_Download, 5000 ); // 2019.09.19
				syslog( LOG_CRIT, VT_BOLD_RED "\n!!!!!! PKG download success & REBOOT !!!!!!\n" VT_END_CR);
			}
			else
				m_Env.app_exit_req = 1;
		}
	}
	m_Env.task_END_status[_TASK_SYSTEM_CALL_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}


/* EOF */



