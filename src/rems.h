/********************************************************************************/
#ifndef __REMS_H
#define __REMS_H
/********************************************************************************/
#ifdef  REMS_GLOBALS
#define REMS_EXT
#else
#define REMS_EXT extern
#endif
/**************************************************************/

/*
2019.06 gREMS Command
--------------------------
0x03	중계기 Alarm Mask

0x44:	Measurement Request/Response
0x45	중계기 Alarm 발생/해제 및 확인
0x46	중계기 Alarm Suppression 발생/해제 및 확인
0x47	Statistic Report/ACK(==0x17)
0x48:	CF/C 설정/조회
0x49:	CF/nC 조회
0x4A	CF/C Auto Report/ACK
0x4B	CF/nC Auto Report/ACK
0x4C:	PLD 설정
-----------------------------
0x60:	Module Inventory 상태 조회
0x61	Module Inventory 자동 보고
0x68	Local Time 요청
0x69	Local Time 요청 응답
0x6A	SFP Module Inventory 상태 조회
0x6B:	중계기 바코드 설정/조회
0xE9	Hidden Request/Response
0xEA	전원 Reset
0xEC	GUI ID/Password 조회
0xEF	Ethernet Packet Loopback
-----------------------------
0x57	Download 1,2차 통보
0x58	Download 통보 확인
0x59	Download Data
0x5A	Download 확인
0x5B	Download FW 적용
*/


REMS_EXT REMS_1024_Pkt gRemsRspPkt_1k;
REMS_EXT REMS_1024_Pkt gRemsRspPkt_1k_CFnC;
REMS_EXT REMS_1024_Pkt gRemsRspPkt_1k_Mea;
REMS_EXT REMS_1024_Pkt gRemsRspPkt_1k_Spectrum;	//// added YCSO 191212


REMS_EXT INT16U tx_gREMS_CFG__C_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port);
REMS_EXT INT16U tx_gREMS_Mea_St2_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port );
REMS_EXT INT16U tx_gREMS_CFG_nC_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port);
REMS_EXT INT16U tx_gREMS_Hidden_Status( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port);


REMS_EXT void Set__gREMS_local_Time_Request(void);
REMS_EXT void Clr__gREMS_local_Time_Request(void);
REMS_EXT INT8U IS___gREMS_local_Time_Request(void);
REMS_EXT void Send_LocalTime_Req(void);


REMS_EXT void Set__gREMS_GUI_ID_PW_Request(void);
REMS_EXT void Clr__gREMS_GUI_ID_PW_Request(void);
REMS_EXT INT8U IS___gREMS_GUI_ID_PW_Request(void);
REMS_EXT void Send_to_gREMS_GUI_ID_PW_Req(void);

REMS_EXT INT8U Is__Valid_IP(INT8U disp, INT8U* ipaddr);

REMS_EXT void Make_REMS_SubData( INT16U* pAid_list, INT16U max_num, REMS_1024_Pkt* pPkt, INT8U port );

REMS_EXT void* gREMS_Comm_Task(void *data);

REMS_EXT void Update_Stat( REMS_U32* pItem, INT32U uVal32 );
REMS_EXT void Chk_CFG_Ctrl_Auto_RPT_REQ( INT8U aid0, INT8U aid1 );
REMS_EXT INT8U Make_gREMS_Time_AID_Struct(INT8U* pData);



REMS_EXT INT8U Update_BTS_Name(INT8U* data, INT16U tot_len);


REMS_EXT void Supp_Alm_Timer_Init( void );
REMS_EXT void Supp_Alm_Timer_Inc( INT8U byte, INT8U bit );
REMS_EXT INT8U Get_Supp_Alm_Time_Cnt( INT8U byte, INT8U bit );
REMS_EXT void Supp_Alm_Timer_Clr( INT8U byte, INT8U bit );
REMS_EXT void Make_Suppress_Alarm( void );
REMS_EXT REMS_ST_STR m_Stat;
REMS_EXT REMS_ENV m_REMS_Env;

REMS_EXT void Set__alm_que_idx_SuperCapChange(void);
REMS_EXT INT8U Is__alm_que_idx_SuperCapChange(void);

//// YCSO added START 191212
REMS_EXT void tx_gREMS_spectrum_data_set(INT8U* data, INT32U size );
REMS_EXT void tx_gREMS_spectrum_set( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf, INT8U port, INT8U* r_ip, INT16U r_udp_port);
REMS_EXT void* Rems_Spectrum_Task( void *data );
//// YCSO added END 191212

REMS_EXT void Update_Only_Alm_Log(INT8U idx); // gREMS Comm 준비안되어도 log write는 함.


/*----------------------------------------------------------*/


/********************************************************************************/
#endif
/********************************************************************************/
