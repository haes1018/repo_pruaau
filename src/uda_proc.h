/********************************************************************************/
#ifndef __UDA_PROC_H
#define __UDA_PROC_H
/********************************************************************************/
#ifdef  UDA_PROC_GLOBALS
#define UDA_PROC_EXT
#else
#define UDA_PROC_EXT extern
#endif
/**************************************************************/

UDA_PROC_EXT void Proc___RVS_Spurious_Payload(void); // chk
UDA_PROC_EXT void Proc___Auto_Rst_UDA(void);

UDA_PROC_EXT void UDA_Alarm_Set(INT8U UDA, INT8U alm);
UDA_PROC_EXT INT8U auto_rst_req[UDA_MAX]; // 1 : under reset now 

UDA_PROC_EXT void Init_UDA_Proc(void);
UDA_PROC_EXT void UDA_Auto_Rst_Cnt_Clr(INT8U rst_idx);

UDA_PROC_EXT void Use_UDA_Oper_Mode_Ctrl(INT8U Uda, INT8U Mode);
UDA_PROC_EXT INT8U Chk_Auto_Reset_By_UDA_Suppress(void);
UDA_PROC_EXT void Proc___Reset_By_UDA(void);
UDA_PROC_EXT void UDA_Auto_Rst_All_Init(INT8U bUda);

UDA_PROC_EXT void Update_Payload_Alm(void);

UDA_PROC_EXT INT8U Is_RVS_Spurious(INT8U optic, INT8U path4);

UDA_PROC_EXT INT8U Is_Alm_Gen( INT8U Prev, INT8U Curr, INT8U bit_posi );
UDA_PROC_EXT INT8U Is_Alm_Now( INT8U Curr, INT8U bit_posi );
UDA_PROC_EXT INT8U Is_RVS_Spur_Only_Off(INT8U optic, INT8U path4);


/********************************************************************************/
#endif
/********************************************************************************/





