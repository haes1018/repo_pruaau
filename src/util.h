/********************************************************************************/
#ifndef __UTIL_H
#define __UTIL_H
/********************************************************************************/
#ifdef  UTIL_GLOBALS
#define UTIL_EXT
#else
#define UTIL_EXT extern
#endif
/**************************************************************/
#define C_BIT(x)	(0x01<<x)
#define N_BIT(x)	(~(0x01<<x))
#define BIT(x)	    (1<<(x))
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
// 외부 호출 함수
//
UTIL_EXT INT16U AHex2Dig_INT16( INT8U *d );
UTIL_EXT void Dig2AHex_INT16U( INT8U *h, INT16U d );

UTIL_EXT INT32U BYTE_to_INT32L( INT8U* psrc ); // little 
UTIL_EXT INT32U BYTE_to_INT32B( INT8U* psrc ); // big endian
UTIL_EXT void INT32L_to_BYTE( INT8U* pDst, INT32U Src32 ); // little 
UTIL_EXT void INT32B_to_BYTE( INT8U* pDst, INT32U Src32 ); // big endian
UTIL_EXT INT16U BYTE_to_INT16L( INT8U* psrc ); // little 
UTIL_EXT INT16U BYTE_to_INT16B( INT8U* psrc ); // big endian
UTIL_EXT void INT16L_to_BYTE( INT8U* pDst, INT16U Src16 ); // little 
UTIL_EXT void INT16B_to_BYTE( INT8U* pDst, INT16U Src16 ); // big endian


UTIL_EXT void AID_Pwr( INT16S p, INT8U *d );
UTIL_EXT void date_to_a( INT8U *h, INT8U* date, INT8U len );

UTIL_EXT INT16S AID_Pwr_Ctrl( INT8U *d );
UTIL_EXT INT8S ROUND( INT16S val );
UTIL_EXT INT8S ROUND_05( INT16S val );
UTIL_EXT INT8U ROUND_05U( INT16S val );
UTIL_EXT INT8U ntoa(INT8U val);
UTIL_EXT INT8U atoh( INT8U v );
UTIL_EXT INT8U AHex2Dig( INT8U *d );
UTIL_EXT INT8U AHex2Dig2( INT8U *d );
UTIL_EXT void Dig2AHex( INT8U *h, INT8U d );
UTIL_EXT INT8U BCD2Dig( INT8U b );
UTIL_EXT INT8U Dig2BCD( INT8U d );
UTIL_EXT void MakePwrValue( INT8U* v, INT16S p );
UTIL_EXT void MakePnValue( INT8U* v, INT16U pn );
UTIL_EXT void Make_3G_Channel_Value( INT8U* v, INT16U pn );
UTIL_EXT INT16U Swap16( INT16U val );
UTIL_EXT void calcBits( INT8U *Value, INT8U State, INT8U idx );
UTIL_EXT void ctrlBits( INT8U *Value, INT8U State, INT8U idx );
UTIL_EXT void HysAlarmCheck( INT8U* palm, INT8U bit, INT16S a, INT16S b, INT16S h );
UTIL_EXT void Hysterisis_Alm_Chk( INT8U* pAlm, INT16S a, INT16S b, INT16S h );
UTIL_EXT INT8U Hys_Alm_Update(INT8U alm, INT16S a, INT16S b, INT16S h);  // a>=b 알람 발생


UTIL_EXT INT16U Get_Differenc(INT16S* pPwr_x10, INT8U num);

UTIL_EXT char* Get_SM_Str( INT8U path4 );

// PHONE
UTIL_EXT INT8U MakeSMSInPowerVal( INT8S val );
UTIL_EXT INT8U MakeSMSPowerVal( INT8S val );
UTIL_EXT INT8S MakeGUIInPowerVal( INT8U val );
UTIL_EXT INT16S MakePwrValue_GUI( INT8U* v );

// 테이블 관련 함수
UTIL_EXT signed short Get_Y_From_x( signed short (*pTbl)[2], signed short x);
UTIL_EXT INT8U Get_Item_Idx2(INT16S value, const INT16S(* pTbl)[2], INT16S TblSize, INT8U compare);

/*----------------------------------------------------------*/
// 외부 참조 변수
//
#ifdef  UTIL_GLOBALS
/*                                bit0  bit1  bit2  bit3  bit4  bit5  bit6  bit7  */
UTIL_EXT const INT8U cbits[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
UTIL_EXT const INT8U nbits[8] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

UTIL_EXT const INT8U c2bits[4] = { 0x03, 0x0C, 0x30, 0xC0 };
UTIL_EXT const INT8U n2bits[4] = { 0xFC, 0xF3, 0xCF, 0x3F };

UTIL_EXT const INT16U cbits16[16] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
									 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 };
UTIL_EXT const INT16U nbits16[16] = { 0xFFFE, 0xFFFD, 0xFFFB, 0xFFF7, 0xFFEF, 0xFFDF, 0xFFBF, 0xFF7F,
									 0xFEFF, 0xFDFF, 0xFBFF, 0xF7FF, 0xEFFF, 0xDFFF, 0xBFFF, 0x7FFF };

UTIL_EXT const INT32U cbits32[32] = 
{ 
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 
	0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 
	0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 
	0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 
	0x10000000, 0x20000000, 0x40000000, 0x80000000
};

UTIL_EXT const INT32U nbits32[32] = 
{
	0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, 0xFFFFFFF7, 
	0xFFFFFFEF, 0xFFFFFFDF, 0xFFFFFFBF, 0xFFFFFF7F,
	0xFFFFFEFF, 0xFFFFFDFF, 0xFFFFFBFF, 0xFFFFF7FF, 
	0xFFFFEFFF, 0xFFFFDFFF, 0xFFFFBFFF, 0xFFFF7FFF,
	0xFFFEFFFF, 0xFFFDFFFF, 0xFFFBFFFF, 0xFFF7FFFF, 
	0xFFEFFFFF, 0xFFDFFFFF, 0xFFBFFFFF, 0xFF7FFFFF,
	0xFEFFFFFF, 0xFDFFFFFF, 0xFBFFFFFF, 0xF7FFFFFF, 
	0xEFFFFFFF, 0xDFFFFFFF, 0xBFFFFFFF, 0x7FFFFFFF
};

#else
UTIL_EXT const INT8U cbits[8];
UTIL_EXT const INT8U nbits[8];
UTIL_EXT const INT8U c2bits[4];
UTIL_EXT const INT8U n2bits[4];
UTIL_EXT const INT16U cbits16[16];
UTIL_EXT const INT16U nbits16[16];
UTIL_EXT const INT32U cbits32[32];
UTIL_EXT const INT32U nbits32[32];
#endif
/*----------------------------------------------------------*/

UTIL_EXT char* Get_fpga_Str( INT8U fpga_idx );
UTIL_EXT char* Get_path4_Str( INT8U path4 );
UTIL_EXT char* Get_D10_Str( INT8U path10 );
UTIL_EXT char* Get_Up_Dev_Str( INT8U dev );
UTIL_EXT char* Get_Up_DevType_Str( INT8U up_dev_type );
UTIL_EXT char* Get_RP7_Str( INT8U r7 );
UTIL_EXT char* Get_P10_Str( INT8U p10 );
UTIL_EXT char* Get_CMD_Str( INT8U cmd );
UTIL_EXT char* Get_RVS_5_Str( INT8U rvs_5 );



UTIL_EXT int set_Ro_system_time(INT8U* time);
UTIL_EXT int set_system_time(int year, int month, int day, int hour, int minute, int second);
UTIL_EXT char* Get_Rst_Reason_Str( INT8U reason );
UTIL_EXT int get_system_time(void);





// ANTS PRU & AAU ----
#define INTERFACE_FILE	"/etc/sysconfig/network"
#define NETWORK_STOP    "ifconfig eth0 down"
#define NETWORK_START   "ifconfig eth0 up"

#define OLD_INTERFACE_FILE		"/etc/sysconfig/network.old"
#define NEW_INTERFACE_FILE		"/etc/sysconfig/network.new"
//-------------------------------------------------

enum
{
	_MAC0_ = 0,	
	_MTU0_,	
	_DHCP0_,
	_IPADDR0_,	
	_BROADCAST0_,	
	_NETMASK0_,	
	_DEFAULT_GW_,
	_INIT0_,
	_NET_COFG_MAX
};



typedef struct  __attribute__((packed))
{
	INT8U IP_ADDR[4];
	INT8U IP_Def_GW[4];
	INT8U IP_Mask[4];
	INT8U IP__MAC[6];
	INT8U IP_DHCP;
}Network_File;

UTIL_EXT Network_File m_NetFile;


UTIL_EXT void read_network_file(void);

UTIL_EXT void User_UTC_Set( INT32U utc_time_tick );
UTIL_EXT INT32U Get_Cur_UTC_Time(INT8U* pTime);


UTIL_EXT int Init_Net_Value( INT8U* Base_Ip , INT8U* GW0 , INT8U* Base_Mac , INT8U* NetMask0);
UTIL_EXT void Chk_Same_Process(void);



typedef struct  __attribute__((packed))
{
	INT8U  ip[4];
	INT16U cnt16;
	INT16U size16;
	char  result[255];
	INT8U  on;
}PING_TEST;
UTIL_EXT PING_TEST m_ping_test;
UTIL_EXT void Proc___Ping_Test(void);


/*
-----------------------------------------------------
	ATTEN (0.01dB, 0.5dB, 0.25dB)
	Conversion
-----------------------------------------------------	
*/
UTIL_EXT INT8U  conv_att_001_to_05(INT16U att_x100);
UTIL_EXT INT8U  conv_att_01_to_05(INT16U att_x100);
UTIL_EXT INT16U conv_att_001_to_025(INT16U att_x100);
UTIL_EXT INT8U  conv_att_025_to_05(INT16U att_x4);
UTIL_EXT INT16U conv_att_025_to_001(INT16U att_x4);
UTIL_EXT INT16U conv_att_05_to_025(INT8U att_x2);
UTIL_EXT INT16U conv_att_05_to_001(INT8U att_x2);
UTIL_EXT INT16U conv_att_05_to_01(INT8U att_x2);;


UTIL_EXT INT16U conv_delay_1ns_to_001us(INT32U delay_x1n);
UTIL_EXT INT32U conv_delay_001us_to_1ns(INT16U delay_x10n);

//-----------------------------------------------------



UTIL_EXT void* CPU_Usage_Task(void *data);
UTIL_EXT void Disp_IP_addr(INT8U* rx_ip, char* str);
UTIL_EXT void Make_IP_addr_str(INT8U* rx_ip, char* str);




/* 
-----------------------------------------------------------------
	Debug
	Foreground 
	Colours 
-----------------------------------------------------------------
*/
#define VT_BOLD              "\x1b[1m"
#define VT_BLINK             "\x1b[5m"

#define VT_BLACK             "\x1b[30m"
#define VT_RED               "\x1b[31m"  
#define VT_CYAN              "\x1b[36m"   
#define VT_GREEN             "\x1b[32m"   

#define VT_YELLOW            "\x1b[33m"   

#define VT_BLUE              "\x1b[34m"
#define VT_MAGENTA           "\x1b[35m" // pink
#define VT_WHITE             "\x1b[37m"

#define VT_BOLD_RED          "\x1b[1;31m"
#define VT_BOLD_GREEN        "\x1b[1;32m"
#define VT_BOLD_BLUE         "\x1b[1;34m"
#define VT_BOLD_YELLOW       "\x1b[1;33m"
#define VT_BOLD_MAGENTA      "\x1b[1;35m" // pink
#define VT_BOLD_CYAN         "\x1b[1;36m"
#define VT_BOLD_WHITE        "\x1b[1;37m"



#define VT_BLINK_RED         "\x1b[5;31m"
#define VT_BLINK_GREEN       "\x1b[5;32m"
#define VT_BLINK_BLUE        "\x1b[5;34m"

#define VT_END               "\x1b[0m"   // reset(Clear) attributes
#define VT_END_CR            "\x1b[0m\n"
//-----------------------------------------
#define VT_Color_RX         VT_GREEN  // RCU RX
#define VT_Color_TX         VT_CYAN   // RCU TX

#define VT_Color_Evt        VT_YELLOW // RCU TX
#define VT_Color_Err        VT_BOLD_RED    // Error, Fail

// RCU Alert, Warning, Error



//Printf(VT_RED "AlmSendPtr not memalloc \n" VT_END_CR);



UTIL_EXT int Get_system_mac_ip(INT8U* mac, INT8U* ip);
UTIL_EXT int Proc__Get_System_MAC_IP(void);
UTIL_EXT void Close_System_Sock_fd(void);

UTIL_EXT INT8U conv_att_001_to_05(INT16U att_x100);
UTIL_EXT INT8U conv_att_01_to_05(INT16U att_x10);
UTIL_EXT INT8S conv_signed_att_01_to_05(INT16S att_x10);
UTIL_EXT INT16U conv_att_001_to_025(INT16U att_x100);
UTIL_EXT INT16S conv_signed_att_05_to_025(INT8S att_x2);

UTIL_EXT void Init_Debug_Path4(void);
UTIL_EXT INT8U Is_Debug_Path4(INT8U path4);
UTIL_EXT INT8U Is_Debug_Optic_Path(INT8U optic, INT8U path4);


UTIL_EXT int Set_system_MAC(INT8U* s_mac);
UTIL_EXT int Set_system_IP(INT8U* s_ip);
UTIL_EXT INT32U GetUptime(void);
UTIL_EXT void print_agent(INT16U mask, char *color, char *fmt, ...);
UTIL_EXT void Net_Set_Info_Set(INT8U net_cfg, INT8U* net);
UTIL_EXT void Net_Get_Info_Update(void);
UTIL_EXT int Get_System_HW_Mac(INT8U* Mac);
UTIL_EXT int Set_System_HW_Mac(INT8U* Mac); // PRU Only

UTIL_EXT int system_agent(char *fmt, ...);
UTIL_EXT int system_agent_sync(char *fmt, ...);
UTIL_EXT int Change_Local_Time(INT8U* pTime);
UTIL_EXT INT8U IS___Valid_gREMS_Local_Time(INT8U* pTime);
UTIL_EXT char* Get__gREMS_Reset_Reason_Str(INT8U reason);


UTIL_EXT char* Get_OnOff_Str(INT8U onoff);
UTIL_EXT char* Get_Alarm_Str(INT8U alarm);


UTIL_EXT int popen_agent(char *recvbuf, int maxsize, char *fmt, ...);






typedef struct __attribute__((packed))
{
	INT8U start;
	INT8U end;
	INT8U enable;
	INT8U reseved;
	
	INT32U timeout_ms;
	OS_TIMER timer_ms;
} APP_TIMER;

UTIL_EXT void Init__App_Timer(APP_TIMER* pApp_Timer);
UTIL_EXT void Start__App_Timer(APP_TIMER* pApp_Timer, INT32U timeout_ms);
UTIL_EXT INT8U IsEnd__App_Timer(APP_TIMER* pApp_Timer);


UTIL_EXT void Get__PRU_DDR_Memory_Clk_Info(void);


/********************************************************************************/
#endif
/********************************************************************************/
