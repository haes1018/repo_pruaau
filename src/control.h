/********************************************************************************/
#ifndef __CONTROL_5G_H
#define __CONTROL_5G_H
/********************************************************************************/
#ifdef  CONTROL_5G_GLOBALS
#define CONTROL_5G_EXT
#else
#define CONTROL_5G_EXT extern
#endif
/**************************************************************/


/*----------------------------------------------------------*/
// 외부 호출 함수
//

CONTROL_5G_EXT INT8U LocalCtrl( INT8U* data, INT16U tot_len, INT8U cmd );
CONTROL_5G_EXT void InitCtrl( void );
/*----------------------------------------------------------*/
// 외부 참조 변수
//
CONTROL_5G_EXT INT8U User_Ctrl_Req; // 사용자 제어 있었음.
/*----------------------------------------------------------*/

CONTROL_5G_EXT int Parse_AID2_STR(AIDSTR2* paid2, AID2_INFO* paid2_info);






CONTROL_5G_EXT void Use_FAN_Auto_Ctrl( INT8U idx, INT8U val );
CONTROL_5G_EXT void Use_PSU_Ext_FAN_Onoff_Ctrl(INT8U val);
CONTROL_5G_EXT void Use_PSU_INT_FAN_Onoff_Ctrl(INT8U val);

CONTROL_5G_EXT void Use_Ext_FAN_Speed_Ctrl( INT8U speed );
CONTROL_5G_EXT void PSU_Ext_FAN_Onoff_Ctrl( INT8U val ); // FAN_Onoff_Ctrl
CONTROL_5G_EXT void Use_FAN_On__Temp_Ctrl( INT8U idx, INT8S val );
CONTROL_5G_EXT void Use_FAN_Off_Temp_Ctrl( INT8U idx, INT8S val );
CONTROL_5G_EXT void Use_Fwd_Out_Pwr_Lmt_Set( INT8U p, INT8U upDn, INT16S lmt_x10);

CONTROL_5G_EXT void Use_AMP_Stdby_Mode_Time(INT8U idx, INT8U hour, INT8U min);
CONTROL_5G_EXT void Use_MIMO_Stdby_Mode_Time(INT8U idx, INT8U hour, INT8U min);
CONTROL_5G_EXT void Use_FAN_Mode_Time(INT8U idx, INT8U hour, INT8U min);


CONTROL_5G_EXT void Use_TempLmt( INT8S TempLmt );
CONTROL_5G_EXT void Use_TempLmt_x10( INT16S TempLmt_x10 );




CONTROL_5G_EXT void Use_PRU_PwrAmp_OnOff(INT8U path4, INT8U data );
CONTROL_5G_EXT void Use_PwrAmp_Reset( INT8U path4, INT8U data );

CONTROL_5G_EXT void Use_TestPort_CIM(INT8U cim_ctrl);
CONTROL_5G_EXT void Init_TestPort_CIM(void);

//----------------------------------------------------
CONTROL_5G_EXT void Use_d_HC_Threshold(INT8U path, INT16U level);
CONTROL_5G_EXT void Use_d_HC_Threshold_AAU(INT8U path, INT16U level); // 2019.09.20

CONTROL_5G_EXT void Use_d_RE_Channel_BandWidth(INT8U BW_100, INT8U BW___A, char call_info);
CONTROL_5G_EXT void RE_Channel_BandWidth(INT8U BW_100, INT8U BW___A);

CONTROL_5G_EXT void Use_d_Payload_OnOff(INT8U fa, INT8U path, INT8U OnOff); // 2019.04.13
CONTROL_5G_EXT void Use_d_Rx_Casc_Add_OnOff_UL(INT8U OnOff);
CONTROL_5G_EXT void Use_d_DPD_OnOff(INT8U path, INT8U onoff);
CONTROL_5G_EXT void DPD_OnOff_Ctrl(INT8U path, INT8U onoff);


CONTROL_5G_EXT void Use_d_CFR_OnOff(INT8U path, INT8U onoff);
CONTROL_5G_EXT void Use_d_CFR_Level(INT8U path, INT16U level_x10);
CONTROL_5G_EXT void Use_d_PAR_Level(INT8U path, INT16U level_x10);

///////CONTROL_5G_EXT INT16U Use_d_Gain_to_GainRegVal(INT8U path, INT8S Att);	

///////CONTROL_5G_EXT INT32U Use_d_Cal_VCO(INT32U freq);	
CONTROL_5G_EXT INT32U Use_d_Cal_VCO_PRU_1khz(INT8U fa, INT8U dl_ul, INT32U freq_x1000);
CONTROL_5G_EXT INT32U Use_d_Cal_VCO_AAU_1khz(INT32U freq_x1000);


//---------------------------------------------------
#ifdef __MAKER_ANTS__
CONTROL_5G_EXT INT32U Get_GainRegVal(INT8U dlul, INT8U Att_x2);
CONTROL_5G_EXT INT16U Chk_Dig_Max_Att(INT16U Att_x10);
CONTROL_5G_EXT INT16U conv_Reg_Gain_to___Att_x10(INT8U dlul, INT32U regVal);
CONTROL_5G_EXT INT16S Conv_Gain_to_Att(INT16S gain_x10);

#endif
CONTROL_5G_EXT INT16U roundoff_05(INT16U Att_x10);
CONTROL_5G_EXT INT8S step_01__round__step_05(INT16S step_x10);
CONTROL_5G_EXT void Use_OSM_Use(INT8U use);


CONTROL_5G_EXT INT16U conv_Reg_GuardTime_to___usec(INT16U regVal);
CONTROL_5G_EXT INT16U conv_Reg_GuardTime_from_usec(INT16U usec);


CONTROL_5G_EXT INT16U conv_Reg_TsyncDly_to___usec(INT16U regVal);
CONTROL_5G_EXT INT16U conv_Reg_TsyncDly_from_usec(INT16U usec);


CONTROL_5G_EXT INT16U conv_AAU_Reg_GuardTime_to___usec(INT16U regVal);
CONTROL_5G_EXT INT16U conv_AAU_Reg_GuardTime_from_usec(INT16U usec);


CONTROL_5G_EXT INT16U conv_Reg_GuardTime_to___usec_AAU_dl_2_UL(INT16U regVal);
CONTROL_5G_EXT INT16U conv_Reg_GuardTime_from_usec_AAU_dl_2_UL(INT16U usec);
CONTROL_5G_EXT INT16U conv_Reg_GuardTime_to___usec_AAU_ul_2_DL(INT16U regVal);
CONTROL_5G_EXT INT16U conv_Reg_GuardTime_from_usec_AAU_ul_2_DL(INT16U usec);



CONTROL_5G_EXT INT16U conv_Reg_CfrGain_to___Gain(INT16U regVal);
CONTROL_5G_EXT INT16U conv_Reg_CfrGain_from_Gain(INT16U dBm_x10);

CONTROL_5G_EXT void Use_Auto_SD_Mode_En(INT8U path, INT8U SD_Mode);
CONTROL_5G_EXT void Fwd_Out_Att_Set_Req(INT8U path4, INT8U data);
CONTROL_5G_EXT void UseFwd_Out_Att(INT8U path4, INT8U data);
CONTROL_5G_EXT void Fwd_Out_Att_Set_Req(INT8U path4, INT8U data);

CONTROL_5G_EXT void ATT_Pair_Onoff_Req(INT8U path4, INT8U onoff);
CONTROL_5G_EXT void Use_FB_Att( INT8U path4, INT8U att_x2 );
CONTROL_5G_EXT void Use_ATT_Pair_Onoff(INT8U path4, INT8U onoff);

CONTROL_5G_EXT void FB_Offset_Att_Req( INT8U path4, INT8U offset_x2 );
CONTROL_5G_EXT void FB_Att_Req( INT8U path4, INT8U att_x2 );


CONTROL_5G_EXT void Wr_BarCode( INT8U* pBarCode_Info );
CONTROL_5G_EXT void Chk_BarCode(void);
CONTROL_5G_EXT void Chk_Node_Name(void);
CONTROL_5G_EXT void Wr_Node_Name(INT8U* pName, INT8U len);

//---------------------------------------------------
CONTROL_5G_EXT INT32U conv_Reg_Capture_to___usec(INT32U reg32);
CONTROL_5G_EXT INT32U conv_Reg_Capture_from_usec(INT32U usec32);
//---------------------------------------------------
CONTROL_5G_EXT INT32U conv_Reg_RSSI_Time_to___usec(INT32U reg32);
CONTROL_5G_EXT INT32U conv_Reg_RSSI_Time_from_usec(INT32U usec32);
//---------------------------------------------------


CONTROL_5G_EXT INT32U conv_Reg_delay_us_to___nsec(INT32U usec);
CONTROL_5G_EXT INT32U conv_Reg_delay_us_from_nsec(INT32U nsec);
CONTROL_5G_EXT int Use_d_Delay_Ctrl(INT8U dlul, INT8U fa, INT8U path4, INT8U disp_en);
CONTROL_5G_EXT INT8U conv_Reg_Clk_Recovery_to__AID(INT8U clock_st);

CONTROL_5G_EXT void Update_ARFCN_from_BW(void);
CONTROL_5G_EXT void Update_CenterFreq_from_BW(void);
CONTROL_5G_EXT INT16S Conv_Gain_from_Att(INT16S att_x10);




CONTROL_5G_EXT void Use_DL_ON_Guard_Time_Ctrl(INT16U usec);
CONTROL_5G_EXT void Use_UL_to_DL_Guard_Time_Ctrl(INT16U usec);
CONTROL_5G_EXT void Use_UL_ON_Guard_Time_Ctrl(INT16U usec);
CONTROL_5G_EXT void Use_DL_to_UL_Guard_Time_Ctrl(INT16U usec);

CONTROL_5G_EXT void Use_DTU_DL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff);
CONTROL_5G_EXT void Use_DTU_UL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff);

CONTROL_5G_EXT void DTU_DL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff);
CONTROL_5G_EXT void DTU_UL_Path_Onoff(INT8U fa, INT8U mimo, INT8U onoff);
CONTROL_5G_EXT void DTU_UL_Cascade_Onoff(INT8U fa, INT8U mimo, INT8U onoff);




CONTROL_5G_EXT INT8U aid_user_ctrl;

CONTROL_5G_EXT void DPD_Reset_Ctrl(INT8U AID_Idx);
CONTROL_5G_EXT void Proc___DPD_Reset_to_Normal(void);

CONTROL_5G_EXT void Proc____GUI_Test_Alarm(void);

CONTROL_5G_EXT void Use_d_FPGA_Delay_Ctrl(INT8U dlul, INT8U fa, INT8U path4, INT32U delay_ns, INT8U disp_en);
CONTROL_5G_EXT void Use_TSYNC_Delay_Set_PRU(INT32U usec);
CONTROL_5G_EXT void Use_TSYNC_Delay_Set_AAU(INT32U usec);
CONTROL_5G_EXT void Proc___PRU_Tsync_Delay(void);
CONTROL_5G_EXT void Proc___DPD_St_DL_Delay_Offset(void);
CONTROL_5G_EXT void Proc___PRU__BW_Gain_Compensation(void);




CONTROL_5G_EXT void Use_DL_CW_Level_Ctrl(INT8U fa, INT8U l, INT16U level);
CONTROL_5G_EXT void DL_CW_Level_Ctrl(INT8U c, INT8U l, INT16U level);

CONTROL_5G_EXT void Use_DL_CW_OnOff_Ctrl(INT8U fa, INT8U l, INT8U onoff);

CONTROL_5G_EXT void User_DTU_Reset(INT8U reset);

enum
{
	REG_RST______DSP = 0x01, // DTU				
	REG_RST_Main_Brd = 0x02, // FPGA	
	REG_RST_CpriCore = 0x03, // CPRI Core	
	REG_RST_Ethernet = 0x04, // Ethernet	
	REG_RST__JESD204 = 0x05, // JESD204	
	REG_RST_Delay__B = 0x06, // Delay Block	-------( X )
	REG_RST_Filter_B = 0x07, // Filter Block		-------( X )
	REG_RST____L2_SW = 0x08, // L2 SW	
	REG_RST_CalBlock = 0x09, // Calibration Block	-------( X )
	REG_RST_Max
};


CONTROL_5G_EXT INT8U Proc__Cpri_Alarm_Keep_Xsec(INT8U cpri, INT8U alarm);
CONTROL_5G_EXT INT8U Proc__Clock_Normal_Keep_Xsec(INT8U alarm);
CONTROL_5G_EXT void Proc____AAU_Alarm_Fan_and_Temp(void);



CONTROL_5G_EXT INT8U LSH_Gain_Set(INT8U REC, INT8U fa, INT8U mimo, INT8U dlul, INT16U gain_x10);
CONTROL_5G_EXT INT8U Use_LSH_Gain_Set(INT8U REC, INT8U fa, INT8U mimo, INT8U dlul, INT16U gain_x10);
CONTROL_5G_EXT INT8U Test_Pattern_Band_Gain_Set(INT8U fa, INT8U mimo, INT16U gain_x10);
CONTROL_5G_EXT INT8U Use_Test_Pattern_Band_Gain_Set(INT8U fa, INT8U mimo, INT16U gain_x10);

CONTROL_5G_EXT INT8U TSSI_SD_Level_Set(INT8U fa, INT8U mimo, INT16S level_x10);

CONTROL_5G_EXT INT8U Att_Digital_DL_SUM(INT8U path4, INT16U att_x10);
CONTROL_5G_EXT INT8U Use_Att_Digital_DL_SUM(INT8U path4, INT16U att_x10);
CONTROL_5G_EXT INT8U Height_Bal_Gain_Set(INT8U fa, INT8U m, INT16U gain_x10);
CONTROL_5G_EXT INT16U AID_Control( void* pAID, INT8U cmd );
CONTROL_5G_EXT void Default_Setting(void);

CONTROL_5G_EXT INT8U Ping_Test(INT8U* pDst_IP);


/********************************************************************************/
#endif
/********************************************************************************/
