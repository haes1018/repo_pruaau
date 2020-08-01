/********************************************************************************/
#ifndef __COMM_H_
#define __COMM_H_
/********************************************************************************/
#ifdef  COMM_GLOBALS
#define COMM_EXT
#else
#define COMM_EXT extern
#endif
/**************************************************************/

typedef  struct
{
	INT8U	dev_type;

    INT8S	id;		 // RO일때는 0~7, SHS/HUB는 MHS_BR와 같음.
    void*	dev_ptr;	// HRU의 경우, n 번째 HRU 상태구조체 포인터.
} DEV_TYPE;

enum
{
	DEV_TYPE_MINE = 0,
	// TREE_2019_0521
	DEV_TYPE__RU, // Other RU
	// END : TREE_2019_0521
	DEV_TYPE_NONE,
	DEV_TYPE_MAX

};

///// YCSO added 190529
enum
{
	REGISTER___PS_ = 0x00,
	REGISTER___PL_,
	REGISTER_CPRI_,
	REGISTER__AFE_,
	REGISTER__LMK_,
	REGISTER__MAX_
};
//------------------------------

#define DEV_TYPE_EMHS 1
#define DEV_TYPE_MHS  2
#define DEV_TYPE_SHS  3

// SUB ID Check 함수의 리턴 값.
#define _NORMAL_	(0)
#define _NO_SUBID_	(3)
#define _NO_DEV_	(4)

/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
// 외부 호출 함수
//
//TREE_2019_0521
COMM_EXT INT16U UP_Data_Analysis( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U udp_port );
// END : TREE_2019_0521

COMM_EXT INT8U SUB_ID_Check( INT8U* sub_id, DEV_TYPE *dev );
COMM_EXT void Get__gREMS_My_SUBID(INT8U* pSUBID);


COMM_EXT INT16U Make_Alive_Data( PROTOCOL_FRAME* txf, INT8U port );
COMM_EXT INT16U NodeDataAck( PROTOCOL_FRAME* rxf, INT8U* r_ip );
COMM_EXT INT16U Power_Reset_Ctrl( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
// TREE_2019_0521
COMM_EXT INT8U Get__My_RU_ID(void);
COMM_EXT int Cal_Node_IP_Addr(INT8U* sub_id, INT8U* ip);
// END : TREE_2019_0521


/*----------------------------------------------------------*/

COMM_EXT void PLC_Rx_Data_Anal( PROTOCOL_FRAME* rxf );


COMM_EXT INT8U Calc_RO_Mac( const INT8U* pSUBID, INT8U* pMac );
COMM_EXT INT8U Calc_RO_Mac_From_IP( INT8U* pIP, INT8U* pMac );
//----------------------------------------------------------
// 외부 참조 변수
//
#define Max_Jig_Data (21)
COMM_EXT INT8U Jig_Data[Max_Jig_Data+5];
//----------------------------------------------------------

COMM_EXT INT16U tx_EMS_Loopback( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
COMM_EXT INT16U tx_GUI_Log( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
COMM_EXT INT16U rx_GUI_ID_PW(PROTOCOL_FRAME* rxf);


COMM_EXT INT16U txBarCode( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
COMM_EXT INT16U rx_LocalTime( PROTOCOL_FRAME* rxf );
COMM_EXT INT16U SendError( PROTOCOL_FRAME* txf, INT8U Rcode, INT8U err, INT8U port );



#define MAX_FW_DLY_TIME (4) // 4 min
typedef struct __attribute__((packed))
{
	INT8U	cmd_req; // 1 : command 받음. 처리 필요.
	INT8U	cancel;  // 0 : apply/reset, 1 : update cancel.
	INT8U	dly_min; // delay min : 0 ~ 10
	INT8U	forward_cmd_amp_dtu; // 1 : FW Apply Cmd를 AMP, DTU에 전달.
	INT8U	data[3];
	INT8U   len;
	OS_TIMER  timer;
} FW_UPDATE_STR;

COMM_EXT FW_UPDATE_STR m_fw_update;

COMM_EXT void Proc_FW_Update(void);


// TREE_2019_0521
COMM_EXT void Init_GUI_IP_Table(void);
COMM_EXT void Insert_GUI_IP_Table(INT8U* ip, INT16U port16, INT16U seqNo16);
COMM_EXT int Find_GUI_IP(INT16U seqNo16, INT8U* ip, INT16U* pPort16);
// END : TREE_2019_0521


/********************************************************************************/
//5G Table

// protocol占쏙옙 占쏙옙.
/*
#define MAX_PWR_TABLE_COUNT		(62) // 58(EEP_PWR_TABLE_SIZE-4)/2 // TRIO-L
#define MAX_ATT_TABLE_COUNT		(70) // 0~31.5dB
#define MAX_TMP_TABLE_COUNT     (30) // -35 ~ +80
#define PROTO_TMP_TBL_TEMP      (80) // table 첫占쏙옙째 占승듸옙.
*/


#define MAX_PWR_TABLE_COUNT		(62) // 58(EEP_PWR_TABLE_SIZE-4)/2 // TRIO-L
#define MAX_ATT_TABLE_COUNT		(70) // 0~31.5dB
#define MAX_TMP_TABLE_COUNT     (30) // -35 ~ +80
#define PROTO_TMP_TBL_TEMP      (80) // table 첫占쏙옙째 占승듸옙.


typedef struct __attribute__((packed))
{
	INT8U	tbl_num;
	INT8S   init_val;
	INT8U   entry_cnt;
	INT16U  entry[MAX_PWR_TABLE_COUNT-3];
} PWRTABLESTR;

typedef struct __attribute__((packed))
{
	INT8U	tbl_num;
	INT8S   init_val;
	INT8U   entry_cnt;
	INT8S  entry[MAX_ATT_TABLE_COUNT-3];
} ATTTABLESTR;

typedef struct __attribute__((packed))
{
	INT8U	tbl_num;
	INT8S   init_val;
	INT8U   entry_cnt;
	INT8S  entry[MAX_TMP_TABLE_COUNT-3];
} TMPTABLESTR;



typedef struct __attribute__((packed))
{
//         INT8U   RFU_num;
           INT8U   tbl_num;
           INT8S    init_val;
           INT8U   entry_cnt;
           INT8S    entry[ MAX_TMP_TABLE_COUNT -3 ];            
}TEMP_TABLE_STR;                                                                          // eep size 140

typedef struct __attribute__((packed))
{
//         INT8U   RFU_num;
           INT8U   tbl_num;
           INT8S    init_val;
           INT8U   entry_cnt;
           INT8S    entry[ MAX_TMP_TABLE_COUNT -3 ];                 
}TEMP_ATT_TABLE_STR;                                                                    // eep size 140


typedef struct __attribute__((packed))
{
//         INT8U   RFU_num;
	       INT8U   tbl_num;
           INT8S    init_val;
           INT8U   entry_cnt;
           INT8S    entry[ MAX_ATT_TABLE_COUNT -3 ];
}ATT_TABLE_STR;                                                                 // eep size 100


typedef struct __attribute__((packed))
{
//         INT8U   RFU_num;
	       INT8U   tbl_num;
           INT8S    init_val;
           INT8U   entry_cnt;
           INT16U  entry[ MAX_PWR_TABLE_COUNT -3 ];
}DETEC_POWER_TABLE_STR;                                         

typedef struct __attribute__((packed))
{
		INT8U   comm_port;
		INT8U   ip[4];
		INT16U  port;
}COM_PORT_INFO;                                         

COMM_EXT COM_PORT_INFO comm_info[COMM_NUM];

 //// for ANTS
//COMM_EXT INT16S getTblValue( INT8U idx, INT16U key );
COMM_EXT INT8S Get_TempAtt_Tbl(INT8S temp, INT8U idx);
COMM_EXT INT8S Get_MaxMin_Temp_TempAtt_Tbl(INT8U idx, INT8S* max_temp, INT8S* min_temp);
COMM_EXT INT8S Get_TempAtt_Alpha_Tbl(INT8S temp, INT8U idx);
//COMM_EXT signed char Get_Att_Tbl( unsigned char Inx, signed char Att );


COMM_EXT void Proc__Check_Download_Now(void);


// gREMS Error Code
enum
{
	gREMS_Err_CRC_Fail = 0x01,	// CRC 오류	
	gREMS_Err____Frame = 0x02,	// 무효 프레임 	정의되어 있지 않은 프레임
	gREMS_Err____SUBID = 0x03,	// 범위 초과	SUB_ID 범위 초과
	gREMS_Err_FTP_Conn = 0x12,	// FTP 접속 불가	FTP Download 시 Server에 FTP로 접속이 되지 않음
	gREMS_Err_FTP_File = 0x13,	// FTP File 없음	FTP Download 시 Server에 Download File이 존재하지 않음
	gREMS_Err____ID_PW = 0x14,	// ID/Password Mismatch	수신된 ID 및 Password가 일치 하지 않음 (Local Only)
};
#endif

/********************************************************************************/

