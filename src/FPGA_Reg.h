/********************************************************************************/
#ifndef __FPGA_REG_H
#define __FPGA_REG_H
/********************************************************************************/
#ifdef  FPGA_REG_GLOBALS
#define FPGA_REG_EXT
#else
#define FPGA_REG_EXT extern
#endif
/**************************************************************/

#include "FPGA_Map.h"

#define CPRI_LOCK_Val (0x0F) // 2019.04.08

FPGA_REG_EXT void* FPGA_Register_Task(void *data);


typedef struct __attribute__((packed))
{
	INT32U ver;
	INT32U date;
	INT32U test;
} DTU_REG;



FPGA_REG_EXT DTU_REG m_Dtu_Reg;

typedef int  (*ifunc_t)();

typedef struct _usr_cmd_tbl {
	char *cmd;
	char *opts;
	char *help;
	ifunc_t func;
} usr_cmd_tbl_t;

FPGA_REG_EXT usr_cmd_tbl_t usr_cmd_tbl[];

FPGA_REG_EXT int cmd_help(int argc, char *argv[]);
FPGA_REG_EXT int cmd_quit(int argc, char *argv[]);
FPGA_REG_EXT int cmd_version(int argc, char *argv[]);

//FPGA_REG_EXT int cmd_memdump(int argc, char *argv[]);
FPGA_REG_EXT int cmd_regio(int argc, char *argv[]);
FPGA_REG_EXT int cmd_aau_regio(int argc, char *argv[]);


FPGA_REG_EXT ST_DtuStatus m_Reg_Sts;
FPGA_REG_EXT ST_DtuStatus m_Reg_Ctl;
FPGA_REG_EXT ST_DtuStatus m_Reg_Flg; // flag

FPGA_REG_EXT INT8U Reg_Ready_Flag; // 1 when CPRI-Lock
FPGA_REG_EXT void Test_Bit_Struct(void);

FPGA_REG_EXT INT32U Read_FPGA_Reg(INT32U addr32);
FPGA_REG_EXT INT32U Write_FPGA_Reg(INT32U addr32, INT32U data32, char* str);

FPGA_REG_EXT INT8U DTU_Reg_Link_Fail;
FPGA_REG_EXT INT8U Is_DTU_Reg_CommFail_Co(void);
FPGA_REG_EXT INT8U Is_DTU_Reg_CommFail(void);


FPGA_REG_EXT int Open_AXI_Reg(void);
FPGA_REG_EXT void Close_AXI_Reg(void);


FPGA_REG_EXT void Update_Vss_To_m_St(void);
FPGA_REG_EXT void Proc__SFP_Insert__CPRI_Lock(void);


FPGA_REG_EXT INT8U CPRI_Lock_Val[CPRI_PATH]; // 0x0F is cpri-lock, // 2019.04.18
FPGA_REG_EXT INT16U CPRI_Mscl_Val[CPRI_PATH];



FPGA_REG_EXT void AAU_RVS_SD_Init_Ctrl(INT8U path4);

FPGA_REG_EXT void Proc___AAU_Rvs_Shutdown(INT8U path4);

FPGA_REG_EXT INT8U Get_CPRI_Lock(INT8U cpri); // 2019.04.18





FPGA_REG_EXT void Disp_Reg_Address(void);

FPGA_REG_EXT void Proc___Shutdown_Ctrl(void);
FPGA_REG_EXT INT8U Proc___AAU_RVS_In_ALC(INT8U path4);

#ifdef __TYPE__AAU__ // ANTS-AAU
FPGA_REG_EXT void AAU_RF_DL_Att_Set(INT8U path4, INT8U att_x2);
FPGA_REG_EXT void AAU_RF_UL_Att_Set(INT8U path4, INT8U att_x2);
FPGA_REG_EXT void AAU__RF_AMP_OnOff(INT8U path4, INT8U onoff, INT16U idx);
FPGA_REG_EXT void AAU__RF_LNA_OnOff(INT8U path4, INT8U onoff, INT16U idx);
FPGA_REG_EXT void AAU_Polling_Aivie(void);
FPGA_REG_EXT INT16S Get_AAU_DTU_Alpha_Temp_Att_x10(INT8U optic, INT8U path, INT8U dlul);
#endif

#ifdef __TYPE__PRU__ // ANTS-PRU
FPGA_REG_EXT void Set_PRU_RVS_DTU_Att_x10(INT8U optic, INT8U path4, INT16S user_att_x10);
FPGA_REG_EXT void Set_PRU_UL_Gain_Backoff_x10(INT8U optic, INT8U path4, INT16S gain_backoff_x10);
#endif

FPGA_REG_EXT void Lock__FPGA_RdWr_Sem(void);
FPGA_REG_EXT void UnLock__FPGA_RdWr_Sem(void);
FPGA_REG_EXT void SD_Init_Req(INT8U path4);
FPGA_REG_EXT void AAU_FWD_SD_Init_Req(INT8U path4);

FPGA_REG_EXT void Clear_net_valid_chk_cnt(void);



FPGA_REG_EXT void VSS_UL_RE_Reset_ACK_Sw_Default(void);
FPGA_REG_EXT void Proc___VSS_UL_RE_Reset_ACK_Switch(void);

FPGA_REG_EXT INT8U IS_Valid_VSS_REC_Type(INT8U RECTypeID);

FPGA_REG_EXT void AAU__Gain_Check_Mode_Ctrl(INT8U uVal8);
FPGA_REG_EXT void AAU__Gain_Check_Mode_dl_mode_Ctrl(INT8U uVal8);
FPGA_REG_EXT INT16S Get_ALC_Gain_Comp_x10(INT8U optic, INT8U path4);

///// YCSO added 190529
#ifdef __TYPE__PRU__
FPGA_REG_EXT INT32U Wr_AXI_Reg_DTU( int fd, INT32U addr32, INT32U data32);
FPGA_REG_EXT INT32U Rd_AXI_Reg_DTU(int fd, INT32U addr32);
FPGA_REG_EXT void PRU_DL_Gain_Set_Req(INT8U fa, INT8U path, INT16U att_x10);
#endif

FPGA_REG_EXT INT8U IS_BW_Valid(INT8U bw__all, INT8U bw_100, INT8U bw___A);

FPGA_REG_EXT INT16U Get_Curr_TSSI_Input_Pwr_Bal_Att_x10(INT8U BW_A);



FPGA_REG_EXT void Set_ALC_Gain_Comp_x10(INT8U optic, INT8U path4, INT16S gain_x10);
FPGA_REG_EXT INT8U IS_Alpha_Mhz_Existing(void);
FPGA_REG_EXT INT8U IS_Alpha_Mhz_Not_Existing(void);

FPGA_REG_EXT INT8U Get__BW_of_Alpha(void);
FPGA_REG_EXT INT16U Cal__AAU_BW_Path_Att_x10(INT8U path4);
FPGA_REG_EXT void AAU__RF_Max_Att(INT8U path4);
FPGA_REG_EXT void Update__AAU_Temp_Att(INT8U path4);

FPGA_REG_EXT void Init__HardClipCnt_Accum(void);
FPGA_REG_EXT void Init__BIP_ErrCnt_Accum(void);
FPGA_REG_EXT INT8U Get_Instant_CPRI_Lock_St(void);
FPGA_REG_EXT void AAU_UL_AFE_Att_Set(INT8U path4, INT8U att_x1);

FPGA_REG_EXT INT8S Get_AAU_RF_Att_Accuracy_x2(INT8U dl_ul, INT8U path4, INT8U att_x2);
FPGA_REG_EXT INT8S RF_Att_Accuracy_x4[DL_UL][AMPU_4_MAX];



FPGA_REG_EXT void Set_Factory_Mode_Onoff(INT8U onoff);
FPGA_REG_EXT INT8U Get_Factory_Mode(void);
FPGA_REG_EXT INT16S Get__AAU_DL_Gain_Att_x10(INT8U optic, INT8U path);
FPGA_REG_EXT INT16S Get__AAU_UL_Gain_Att_x10(INT8U optic, INT8U path);

FPGA_REG_EXT INT8U IS__CPRI_Lock(INT8U cpri);
FPGA_REG_EXT INT16S Get__PRU_TotPwr_x10_DL_TSSI(INT8U path4);
FPGA_REG_EXT INT8U Chk__PRU_DL_Out_Lower_Alarm(INT8U path4, INT8U alarm, INT8U alm_mask_req, INT8U chk_req);

FPGA_REG_EXT INT8U Chk__AAU_DL_Out_Lower_Alarm(INT8U path4, INT8U alarm);

FPGA_REG_EXT void Proc__VSS_Monitor(void);
FPGA_REG_EXT INT8U Is__Rvs_SD_Total(INT8U path4);


FPGA_REG_EXT INT8U f_DTU_Path_OnOff[MAX_FA_PATH][DL_UL][MAX_MIMO_PATH]; // On(0), OFF(1)
FPGA_REG_EXT void Check_DPD_DCL_Reset(INT8U path4, INT8U onoff);

#define DPD_Rst_Chk_MAX_Cnt (30) // 2019.11.07 : 20(2[s]) -> 30(3[s])


FPGA_REG_EXT INT8U IS__Clock_CPRI_Alarm(void);

FPGA_REG_EXT INT8U IS__Vaild_CPRI__Main(void);
FPGA_REG_EXT INT8U IS__Vaild_CPRI_Alpha(void);


/********************************************************************************/
#endif
/********************************************************************************/

/* EOF */

