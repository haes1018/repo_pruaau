/********************************************************************************/
#ifndef __DOWNLOAD_H
#define __DOWNLOAD_H
/********************************************************************************/
#ifdef  DOWNLOAD_GLOBALS
#define DOWNLOAD_EXT
#else
#define DOWNLOAD_EXT extern
#endif
/**************************************************************/



/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
// 외부 호출 함수


DOWNLOAD_EXT INT16U rxStartDownload( PROTOCOL_FRAME* frame, PROTOCOL_FRAME* tx_frame, INT8U port );
DOWNLOAD_EXT INT16U rxDataDownload( PROTOCOL_FRAME* frame, PROTOCOL_FRAME* tx_frame, INT8U port );
DOWNLOAD_EXT INT16U rxFinishConfirm( PROTOCOL_FRAME* frame, PROTOCOL_FRAME* tx_frame, INT8U port );
DOWNLOAD_EXT INT16U rxReStartDownload( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port );
DOWNLOAD_EXT INT16U rxCancelDownload( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port );
DOWNLOAD_EXT void Check_DN_Image( void );
DOWNLOAD_EXT void* system_call_Task(void * arg);
#define __YCSO__TEST__CONFIRM__

/*----------------------------------------------------------*/
// 외부 참조 변수
//
DOWNLOAD_EXT void RCU_download_Rusume(void);
DOWNLOAD_EXT int rd_iro_version_info(INT8U is_test);
DOWNLOAD_EXT INT16U rxDNconfirm( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port );

/*----------------------------------------------------------*/


#define Code_NWV (0x11)
#define Code_WvE (0x06) // WaveElectro




typedef struct 
{
	INT8U made; 
	/*
	case PAU
		0x06 : WaveElectro
		0x10 : KRF		
		0x11 : NewWave
	*/ 
	INT8U ver[3];
	INT8U upgrade; // 1 : upgrade is needed
	INT8U percent;	
	INT8U reserve[2];
}VER_INFO;


enum
{
	Ver_Cur = 0,
	Ver_Zip,
	Ver_Max
};

typedef struct 
{
	char mod[32];
	char made[32];
	INT8U ver[4];
}LINE_INFO;


enum
{
	WEL = 0, // WaveElect
	NWV,
	AMP_MADE
};


// zip file version : version info file version
typedef struct 
{
	INT8U rcu[3];
	INT8U dtu[3];
	INT8U fpga[3];
	INT8U psu[3];
	INT8U amp[AMP_MADE][AMPU_4_MAX][3];
}MOD_VER_INFO;
DOWNLOAD_EXT MOD_VER_INFO m_ModVer;

// current version before download
DOWNLOAD_EXT VER_INFO info_Rcu;
DOWNLOAD_EXT VER_INFO info_Dtu;
DOWNLOAD_EXT VER_INFO info_FPGA;
DOWNLOAD_EXT VER_INFO info_AMP[AMPU_4_MAX];
DOWNLOAD_EXT VER_INFO info_PSU;



DOWNLOAD_EXT void Init_Ver_Info(void);
DOWNLOAD_EXT void Set_Ver_Info_Rcu( INT8U made, INT8U* ver );
DOWNLOAD_EXT void Set_Ver_Info_PSU( INT8U made, INT8U* ver );
DOWNLOAD_EXT void Set_Ver_Info_AMP( INT8U path4, INT8U made, INT8U* ver );


DOWNLOAD_EXT void Disp_Cur_All_Mod_Ver(void);
DOWNLOAD_EXT void Disp_Zip_All_Mod_Ver(void);

DOWNLOAD_EXT void Zip_Ver_Set( LINE_INFO* pZipInfo );
DOWNLOAD_EXT void Cmp_Version(void);


DOWNLOAD_EXT int Get_RCU_Dn_File_Name( INT8U rpt_maker, char* fileName, INT8U* ver );
DOWNLOAD_EXT int Get_PSU_Dn_File_Name( char* fileName );
DOWNLOAD_EXT int Get_AMP_Dn_File_Name( INT8U amp_maker, char* fileName );

DOWNLOAD_EXT int Proc_ZipImage( void );



#define RCU_RESTART_TMP      "/tmp/rcu_restart"


// zip file download ----------------------------
#define ZIP_FILE       "rcu.zip"    // file name received from server
#define ZIP_DIR_FILE   "/usr/ftp/down/iro.zip"
#define ZIP_DIR        "/usr/ftp/down/zip"   // directory for getting file from ftp server, 
#define OLD_ZIP_DIR    "/usr/ftp/down/zip_old"

#define UNZIP_DIR      "/usr/ftp/down/fw"  // dir' for unzip-processing

#define Ver_INFO_PATH  "/usr/ftp/down/fw/version_info.csv"

#define UnZip_DIR_RCU     "/usr/ftp/down/fw/RCU"
#define UnZip_DIR_DTU     "/usr/ftp/down/fw/DTU" // DTU, FPGA
#define UnZip_DIR_PAU_KR  "/usr/ftp/down/fw/PAU_KR"
#define UnZip_DIR_PAU_NW  "/usr/ftp/down/fw/PAU_NW"
// 2019.04.05. Package download
#define UnZip_DIR_PSU     "/usr/app/sys/PSU"

enum
{
	Code_Img_RCU = 0,
	Code_Img_FPGA,
	Code_Img_DTU, // PRU Only
	Code_Img_DPD, // PRU Only	
	
	Code_Img_Max
};
DOWNLOAD_EXT INT8U code_image_idx; // zip(package) 아닌 경우의 단독 download image 종류 : 0(RCU),1(FPGA),2(DTU),3(DPD),4(MAX)



//DOWNLOAD_EXT void Init_MultiDnChk(void);
//DOWNLOAD_EXT void Proc_MultiDnChk(void);

//////ycso added 
DOWNLOAD_EXT void* system_call_Task(void *data);
DOWNLOAD_EXT void set_zip_DN_flag(INT8U flag_num);
DOWNLOAD_EXT int check_zip_DN_flag(void);
//DOWNLOAD_EXT INT16U ftp_get_pkg(PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port);
DOWNLOAD_EXT INT16U ftp_get_pkg( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip );

///// YCSO added 190510
DOWNLOAD_EXT void set_amp_DN_flag( INT8U amp_num, INT8U flag);

DOWNLOAD_EXT void set_amp_DN_sataus( INT8U amp_num, INT8U flag);
DOWNLOAD_EXT void set_amp_DN_confirm_END( INT8U amp_num, INT8U flag);

DOWNLOAD_EXT void set_psu_DN_flag( INT8U flag);
DOWNLOAD_EXT void set_psu_DN_sataus( INT8U flag);
DOWNLOAD_EXT void set_psu_DN_confirm_END( INT8U flag);
DOWNLOAD_EXT INT8U check_amp_DN_flag(INT8U amp_num); // 2019.09.20



enum
{
	Mod_RCU,
	Mod_DTU,
	Mod_FPGA,
	Mod_AMP,
	Mod_PSU,
	Mod_MAX
};


enum
{
	Auto_Dn_None = 0,
	Auto_Dn_Start,
	Auto_Dn_Noti_1,	
	Auto_Dn_Noti_2,
	Auto_Dn_Chk,	
	Auto_Dn_Data,
	Auto_Dn_End,
	Auto_Dn_Success,
	Auto_Dn_Max
};

enum
{
	Multi_Dn_None = 0,
	Multi_Dn_Progress,
	Multi_Dn_Fail,
	Multi_Dn_Success,
	Multi_Dn_Max
};

enum
{
	Zip_flag_reserve = 0,
	Zip_flag_PSU,
	Zip_flag_PAU,
	Zip_flag_DTU_App,
	Zip_flag_RCU_App,
	Zip_flag_filesystem,
	Zip_flag_kernel,
	Zip_flag_FPGA,
	
	Zip_flag_Max // PRU-dpd는 없음.
};	

DOWNLOAD_EXT int Pro__Chk_Active_Dir(void);
DOWNLOAD_EXT INT8U Current_Active(void);



/********************************************************************************/
#endif
/********************************************************************************/
