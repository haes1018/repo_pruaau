/////////////////////////////
// alarm.c
/////////////////////////////
#define ALARM_5G_GLOBALS
#include "includes.h"
/////////////////////////////
#include "ver_info.h"

extern int daemon_mode;

void init_con(void);
void Download_Mode_Init( void );

///// YCSO added START 190503
#define	EEP_TBL_AMP_TEMP_SIZE		32
#define EEP_TBL_AMP_TEMP_START		0		/////32 *8 

#define EEP_TBL_ATT_SIZE			128		///// 128 
#define EEP_TBL_ATT_START			0x100	///// 128 * 8

#define EEP_TBL_DETEC_POWER_SIZE	64	/////	64 * 8
#define EEP_TBL_DETEC_POWER_START	0x500	/////	64 * 8

// 2019.06.27 : ANTS-AAU-Temp-Table for Alpha only
#define EEP_TBL_AMP_TEMP_ALPHA_SIZE		32	/////	32 * 8, 
#define EEP_TBL_AMP_TEMP_ALPHA_START	0x700	/////	32* 8 

///// YCSO added END
/*
-----------------------------------------------------------------------
Load EEP, Save to EEP
-----------------------------------------------------------------------
*/
#define EEP0_ADDR_0	    (0)	     // m_Eep  시작 위치          ==> 일반 EEP

void LoadEEP( void )
{	
	V_EEP_Read( 0, EEP0_ADDR_0, (INT8U*)&m_Eep, sizeof(EEPDATASTR) );
}
void SaveEEP( void )
{
	if( m_Env.is_eep_write == EEP_WRITE_CODE )
	{
		m_Env.is_eep_write = 0;
		V_EEP_Write( 0, EEP0_ADDR_0, (INT8U*)&m_Eep, sizeof(EEPDATASTR) );
	}
}

///// YCSO added START 190503
#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU

void LoadEEPTable( void )
{
	int idx = 0;

	for( idx = 0; idx < 8; idx++)
	{
		V_EEP_Read( 1, EEP_TBL_AMP_TEMP_START 	 + (EEP_TBL_AMP_TEMP_SIZE * idx)	, (INT8U*)&m_AMP_tmp_tbl[idx]	, sizeof(Tbl_AMP_TEMP) );
		V_EEP_Read( 1, EEP_TBL_ATT_START		 + (EEP_TBL_ATT_SIZE * idx)			, (INT8U*)&m_ATT_AMP_tbl[idx]	, sizeof(Tbl_ATT) );
		V_EEP_Read( 1, EEP_TBL_DETEC_POWER_START + (EEP_TBL_DETEC_POWER_SIZE * idx)	, (INT8U*)&m_pwr_tbl[idx]		, sizeof(Tbl_DETEC_POWER) );
		// 2019.06.27 : ANTS-AAU-Alpha Table
		V_EEP_Read( 1, EEP_TBL_AMP_TEMP_ALPHA_START	+ (EEP_TBL_AMP_TEMP_ALPHA_SIZE * idx)	, (INT8U*)&m_AMP_tmp_tbl_Alpha[idx]	, sizeof(Tbl_AMP_TEMP) );		
	}

}

void Load_POWER_Table( INT8U idx )
{
	V_EEP_Read( 1, EEP_TBL_DETEC_POWER_START + (EEP_TBL_DETEC_POWER_SIZE*idx) , (INT8U*)&m_pwr_tbl[idx]	, sizeof(Tbl_DETEC_POWER) );
}
void Load_ATT_Table( INT8U idx )
{
	V_EEP_Read( 1, EEP_TBL_ATT_START + (EEP_TBL_ATT_SIZE*idx) , (INT8U*)&m_ATT_AMP_tbl[idx]	, sizeof(Tbl_ATT) );
}

void Load_AMP_TEMP_Table( INT8U idx )
{
	V_EEP_Read( 1, EEP_TBL_AMP_TEMP_START  + (EEP_TBL_AMP_TEMP_SIZE*idx) , (INT8U*)&m_AMP_tmp_tbl[idx]	, sizeof(Tbl_AMP_TEMP) );
}

// 2019.06.27 : ANTS-AAU-Alpha Table
void Load_AMP_TEMP_Alpha_Table( INT8U idx )
{
	V_EEP_Read( 1, EEP_TBL_AMP_TEMP_ALPHA_START  + (EEP_TBL_AMP_TEMP_ALPHA_SIZE*idx) , (INT8U*)&m_AMP_tmp_tbl_Alpha[idx]	, sizeof(Tbl_AMP_TEMP) );
}

void Save_POWER_Table( INT8U idx )
{
	V_EEP_Write( 1, EEP_TBL_DETEC_POWER_START + (EEP_TBL_DETEC_POWER_SIZE*idx), (INT8U*)&m_pwr_tbl[idx], sizeof(Tbl_DETEC_POWER) );
}

void Save_ATT_Table( INT8U idx )
{
	V_EEP_Write( 1, EEP_TBL_ATT_START + (EEP_TBL_ATT_SIZE*idx), (INT8U*)&m_ATT_AMP_tbl[idx], sizeof(Tbl_ATT) );
}

void Save_AMP_TEMP_Table( INT8U idx )
{
	V_EEP_Write( 1, EEP_TBL_AMP_TEMP_START + (EEP_TBL_AMP_TEMP_SIZE*idx), (INT8U*)&m_AMP_tmp_tbl[idx], sizeof(Tbl_AMP_TEMP) );
}

// 2019.06.27 : ANTS-AAU-Alpha Table
void Save_AMP_TEMP_Alpha_Table( INT8U idx )
{
	V_EEP_Write( 1, EEP_TBL_AMP_TEMP_ALPHA_START + (EEP_TBL_AMP_TEMP_ALPHA_SIZE*idx), (INT8U*)&m_AMP_tmp_tbl_Alpha[idx], sizeof(Tbl_AMP_TEMP) );
}

#endif

///// YCSO added END



void def_IP_Info_Set(void)
{	
	m_Env.lohost_ip[0] = 127;
	m_Env.lohost_ip[1] = 0;
	m_Env.lohost_ip[2] = 0;
	m_Env.lohost_ip[3] = 1;	
	
	m_Env.lohost_ip_32 = BYTE_to_INT32L( m_Env.lohost_ip );	
}


void Init_Status_struct(void)
{
	// 구조체 0 으로 초기화.
	memset(&m_Env,       0, sizeof(PGMEnvSTR));	
	memset(&m_St,        0, sizeof(ST_STR));
	memset(&f_Req,       0, sizeof(CTRL_REQ));
}


void Check_m_Eep_Init_Value(void)
{
	INT8U c, l;
	int delay_ns[2];

#ifdef __DO_Not_Save_LS_DELAY__ // 2019.09.17
	for(c=0; c<MAX_FA_PATH; c++)
	{
		m_Eep.LS_Delay_DL_nsec[c] = 0;
		m_Eep.LS_Delay_UL_nsec[c] = 0;
		
		for(l=0; l<MAX_DL; l++)
		{
			m_Eep.d_FPGA_Delay_ns_DL[c][l] = 0;
			m_Eep.d_FPGA_Delay_ns_UL[c][l] = 0;
		}
	}
#else
	// 2019.04.18
	if(m_Eep.Variable_Init_20190418 != 0x20190509)
	{
		m_Eep.Variable_Init_20190418 = 0x20190509;

		m_Eep.LS_Delay_DL_nsec[_100M] = 0;
		m_Eep.LS_Delay_DL_nsec[Alpha] = 0;
		m_Eep.LS_Delay_UL_nsec[_100M] = 0;
		m_Eep.LS_Delay_UL_nsec[Alpha] = 0;

		// 2019.05.09
		for(c=0; c<MAX_FA_PATH; c++)
		{
			for(l=0; l<MAX_DL; l++)
			{
				m_Eep.d_FPGA_Delay_ns_DL[c][l] = 0;
				m_Eep.d_FPGA_Delay_ns_UL[c][l] = 0;
			}
		}		
	}
	
	if(m_Eep.LS_Delay_DL_nsec[_100M] > 500000) // max 500.000[us]
		m_Eep.LS_Delay_DL_nsec[_100M] = 0;

	// 2019.04.18	
	if(m_Eep.LS_Delay_UL_nsec[Alpha] > 500000) // max 500.000[us]
		m_Eep.LS_Delay_UL_nsec[Alpha] = 0;	
#endif

	// 2019.04.06
	for(c=0; c<MAX_FA_PATH; c++)
	{
		for(l=0; l<MAX_DL; l++)
		{			
			if(m_Eep.d_Offset_DTU_Delay_DL[c][l] > 20000000)
				m_Eep.d_Offset_DTU_Delay_DL[c][l] = 0;
				
			if(m_Eep.d_Offset_DTU_Delay_UL[c][l] > 20000000)
				m_Eep.d_Offset_DTU_Delay_UL[c][l] = 0;
	
			delay_ns[DL] = Use_d_Delay_Ctrl(DL, c, l, _Not_Disp_); // 2019.05.13.
			delay_ns[UL] = Use_d_Delay_Ctrl(UL, c, l, _Not_Disp_); // 2019.05.13.

			if((delay_ns[DL] >= 0)&&(delay_ns[DL] >= 0))
			{
				// 2019.05.09. Added.
				Use_d_FPGA_Delay_Ctrl(DL, c, l, delay_ns[DL], _Not_Disp_);
				Use_d_FPGA_Delay_Ctrl(UL, c, l, delay_ns[UL], _Not_Disp_);
			}
		}
	}
	/*
	// m_St <--- m_Eep 는 무의미 해서, 이런 복사는 삭제예정. 단, m_Eep.xxx 의 초기값의 범위를 체크
    // 하는 것을 넣을 필요는 있음.
	// max / min check for init eep -value
	*/
}



const static char User_ID_Def[12] = "skt";
const static char User_PW_Def[12] = "skt123qwe";

void init_con( void )
{
	INT8U i, c;
	INT8U eep_save_req = 0;

	// hidden version
	m_St.RCU_Ver[0] = SW_R_VER1;
	m_St.RCU_Ver[1] = SW_R_VER2;
	m_St.RCU_Ver[2] = SW_R_VER3;	

	// main version
	m_St.RCU_Ver_Main[0] = SW_Main_VER1;
	m_St.RCU_Ver_Main[1] = SW_Main_VER2;
	m_St.RCU_Ver_Main[2] = SW_Main_VER3;	
	
	m_St.Int_Ver[0] = SW_VER1;
	m_St.Int_Ver[1] = SW_VER2;

	D_Printf( DEBUG_SYSTEM, "[RCU] Ver %02d.%02d.%02d ",SW_R_VER1, SW_R_VER2, SW_R_VER3 );

	

	// 2019.12.30
	
	Default_Setting();	

	if(m_Eep.Delay_Set_x10 >= 0xF000) 
		m_Eep.Delay_Set_x10 = 0;
	m_St.Delay_Set_x10 = m_Eep.Delay_Set_x10;

	if(m_Eep.Ext_FAN_Speed > 100)
		m_Eep.Ext_FAN_Speed = 100;
		
	//#ifdef __TYPE__PRU__
	//m_PSU.desire_Fan_Speed = m_Eep.Ext_FAN_Speed;
	//#endif
	
	
	Use_FAN_Off_Temp_Ctrl(EXT_FAN, m_Eep.FAN_OffTemp[EXT_FAN] );
	Use_FAN_On__Temp_Ctrl(EXT_FAN, m_Eep.FAN_On_Temp[EXT_FAN] );
	
	Use_FAN_Off_Temp_Ctrl(INT_FAN, m_Eep.FAN_OffTemp[INT_FAN] );
	Use_FAN_On__Temp_Ctrl(INT_FAN, m_Eep.FAN_On_Temp[INT_FAN] );

	Use_FAN_Auto_Ctrl(EXT_FAN, m_Eep.FAN_Auto[EXT_FAN] );	
	Use_FAN_Auto_Ctrl(INT_FAN, m_Eep.FAN_Auto[INT_FAN] );	

	Use_PSU_INT_FAN_Onoff_Ctrl(m_Eep.FAN_OnOff[INT_FAN]);

	for (i=0;i<AMPU_4_MAX;i++)
	{
		Use_Fwd_Out_Pwr_Lmt_Set( i, HI, m_Eep.Fwd_Out_Lmt_x10[i][HI] );
		Use_Fwd_Out_Pwr_Lmt_Set( i, LO, m_Eep.Fwd_Out_Lmt_x10[i][LO] );
	}
	//===============================================================
	if(m_Eep.MIMO_Stdby_Mode_Onoff != OFF) m_Eep.MIMO_Stdby_Mode_Onoff = ON; // default ON
	m_St.MIMO_Stdby_Enable = m_Eep.MIMO_Stdby_Mode_Onoff;	
	
	//===============================================================
	if (m_Eep.AMP_Stdby_Init_Use != 'G')
	{		
		m_Eep.AMP_Stdby_Mode_Onoff = OFF;		
		//
		m_Eep.AMP_Stdby_Start_Time[0] = 23;
		m_Eep.AMP_Stdby_Start_Time[1] = 0;
		//
		m_Eep.AMP_Stdby_Stop_Time[0] = 7;
		m_Eep.AMP_Stdby_Stop_Time[1] = 0;
		//
		m_Eep.AMP_Stdby_Path = 0xFF; // 0xFF : all not used
		//
		m_Eep.AMP_Stdby_Init_Use = 'G';
	}
	if (m_Eep.AMP_Stdby_Mode_Onoff != ON) m_Eep.AMP_Stdby_Mode_Onoff = OFF; // default off
	m_St.AMP_Stdby_Enable = m_Eep.AMP_Stdby_Mode_Onoff;
	//===============================================================
	// AMP Stdby mode와 Mimo Stdby mode는 동시에 on 안됨.
	// if both on, mimo-stdby is default.
	if ((m_Eep.AMP_Stdby_Mode_Onoff == ON) && (m_Eep.MIMO_Stdby_Mode_Onoff == ON))
	{
		m_Eep.AMP_Stdby_Mode_Onoff = OFF;
		m_St.AMP_Stdby_Enable = OFF;
	}
	//===============================================================
	Use_AMP_Stdby_Mode_Time( 0, m_Eep.AMP_Stdby_Start_Time[0], m_Eep.AMP_Stdby_Start_Time[1]);
	Use_AMP_Stdby_Mode_Time( 1, m_Eep.AMP_Stdby_Stop_Time[0],  m_Eep.AMP_Stdby_Stop_Time[1]);

	Use_MIMO_Stdby_Mode_Time( 0, m_Eep.MiMo_Stdby_Start_Time[0], m_Eep.MiMo_Stdby_Start_Time[1]);
	Use_MIMO_Stdby_Mode_Time( 1, m_Eep.MiMo_Stdby_Stop_Time[0],  m_Eep.MiMo_Stdby_Stop_Time[1]);

	Use_FAN_Mode_Time( 0, m_Eep.FAN_Mode_Start_Time[0], m_Eep.FAN_Mode_Start_Time[1]);
	Use_FAN_Mode_Time( 1, m_Eep.FAN_Mode_Stop__Time[0], m_Eep.FAN_Mode_Stop__Time[1]);

	m_St.AMP_Stdby_Path = m_Eep.AMP_Stdby_Path;

	Use_TempLmt_x10(m_Eep.TempLmt_x10);
	

#ifdef __TYPE__PRU__	
	if(m_Eep.Layer_Mode >= Oper_Max)
		m_Eep.Layer_Mode = Oper_SISO;	
#else		
		m_Eep.Layer_Mode = Oper_MIMO; // 2019.04.24 : AAU is "MIMO Mode" Only
#endif

	for (i=0;i<AMPU_4_MAX;i++)
	{
		if(m_Eep.LayerUseInfo[i] > 0x03) 
			m_Eep.LayerUseInfo[i] = AMP_USE; // default use
			
		m_St.LayerUseInfo[i] = m_Eep.LayerUseInfo[i] & 0x03; // 2020.02.03

		if(m_Eep.Auto_SD_Mode_En[i] > 1)
			m_Eep.Auto_SD_Mode_En[i] = ON;
		Use_Auto_SD_Mode_En(i, m_Eep.Auto_SD_Mode_En[i]);

		Use_ATT_Pair_Onoff(i, m_Eep.ATT_Pair_Onoff[i]);
		
		if(m_Eep.FB_ATT_x2[i] > (30*2)) 
			m_Eep.FB_ATT_x2[i] = 0;
		Use_FB_Att(i, m_Eep.FB_ATT_x2[i]);
	}

	Init_TestPort_CIM(); // 2019.04.06

	InitInventory(); // 2019.04.06
	Chk_BarCode(); // 2019.04.06	
	Chk_Node_Name(); // 2019.04.06	

	if(m_Eep.Node_Name_len > Node_Name_MAX)
		m_Eep.Node_Name_len = 1;
	

	// 2019.04.06
	// 2019.04.18 : AID deleted
	//if(m_Eep.System_Shape > PRU_2T2R)  // 0x5009, System 형상, PRU Only
	//	m_Eep.System_Shape = PRU_4T4R;

	//add 0313
	Check_m_Eep_Init_Value();	
	Use_OSM_Use(m_Eep.OSM_Use & 0x01);

	for (i=0;i<AMPU_4_MAX;i++)
	{
		if(m_Eep.d_Hard_Clip_OnOff[i] > OFF)
			m_Eep.d_Hard_Clip_OnOff[i] = OFF;

		// 2019.07.27	
		if(m_Eep.d_Offset_DPD_x10[i] >  100) m_Eep.d_Offset_DPD_x10[i] = 0;
		if(m_Eep.d_Offset_DPD_x10[i] < -100) m_Eep.d_Offset_DPD_x10[i] = 0;	
	}

#ifdef __TYPE__AAU__
	for (i=0;i<AMPU_4_MAX;i++)
	{
		// 2019.04.20
		//----------------------
		if(m_Eep.Rvs_IN_ALC_Onoff[i] != ON)
			m_Eep.Rvs_IN_ALC_Onoff[i] = OFF;
		m_St.Rvs_IN_ALC_Onoff[i] = m_Eep.Rvs_IN_ALC_Onoff[i];
		//----------------------
		if(m_Eep.AAU_Fwd_ALC_Onoff[i] != ON) // 0xC04B, AAU, 2019.05.13
			m_Eep.AAU_Fwd_ALC_Onoff[i] = OFF;
		//----------------------
		if(m_Eep.AAU_Fwd_SD__Onoff[i] != ON) // 0xC050, AAU, 2019.05.13
			m_Eep.AAU_Fwd_SD__Onoff[i] = OFF;
		//----------------------
		if(m_Eep.Rvs_IN_ALC_x10[i] > (-10*10)) // not yet fixed.
			m_Eep.Rvs_IN_ALC_x10[i] = (-40*10); // not yet fixed.
		m_St.Rvs_IN_ALC_x10[i] = m_Eep.Rvs_IN_ALC_x10[i];	
		//----------------------
		if(m_Eep.Rvs_IN_SD_x10[i] > (-10*10)) // not yet fixed.
			m_Eep.Rvs_IN_SD_x10[i] = (-40*10); // not yet fixed.
		m_St.Rvs_IN_SD_x10[i] = m_Eep.Rvs_IN_SD_x10[i];	
		//----------------------
		if(m_Eep.Rvs_Gain_Bal_On[i] != ON)
			m_Eep.Rvs_Gain_Bal_On[i] = OFF;
		m_St.Rvs_Gain_Bal_On[i] = m_Eep.Rvs_Gain_Bal_On[i];
		//----------------------
		if(m_Eep.Rvs_Gain_Bal_x2[i] > (5*2)) // not yet fixed.
			m_Eep.Rvs_Gain_Bal_x2[i] = 0; // not yet fixed.
		m_St.Rvs_Gain_Bal_x2[i] = m_Eep.Rvs_Gain_Bal_x2[i];	
		//----------------------		
		if(m_Eep.FWD_Pwr_Offset_x10[i] > 300) m_Eep.FWD_Pwr_Offset_x10[i] = 0;
		if(m_Eep.FWD_Pwr_Offset_x10[i] < -300) m_Eep.FWD_Pwr_Offset_x10[i] = 0;
		m_St.FWD_Pwr_Offset_x10[i] = m_Eep.FWD_Pwr_Offset_x10[i];
		//----------------------		
		if(m_Eep.Rvs_OUT_Pwr_Offset_x10[i] > 300) m_Eep.Rvs_OUT_Pwr_Offset_x10[i] = 0;
		if(m_Eep.Rvs_OUT_Pwr_Offset_x10[i] < -300) m_Eep.Rvs_OUT_Pwr_Offset_x10[i] = 0;
		m_St.Rvs_OUT_Pwr_Offset_x10[i] = m_Eep.Rvs_OUT_Pwr_Offset_x10[i];		
		//----------------------		
		if(m_Eep.Rvs_IN_Pwr_Offset_x10[i] > 300) m_Eep.Rvs_IN_Pwr_Offset_x10[i] = 0;
		if(m_Eep.Rvs_IN_Pwr_Offset_x10[i] < -300) m_Eep.Rvs_IN_Pwr_Offset_x10[i] = 0;
		m_St.Rvs_IN_Pwr_Offset_x10[i] = m_Eep.Rvs_IN_Pwr_Offset_x10[i];			
		//----------------------
		if(m_Eep.Over_Pwr_Thr_x10[i] > 500) // not yet
			m_Eep.Over_Pwr_Thr_x10[i] = 500;
		m_St.Over_Pwr_Thr_x10[i] = m_Eep.Over_Pwr_Thr_x10[i];
		//----------------------		
	}	

	
	if(m_Eep.DC_Fail_Volt_x10[DC__48V] > 480) 
		m_Eep.DC_Fail_Volt_x10[DC__48V] = 0;

	if(m_Eep.DC_Fail_Volt_x10[DC_Main] > 50) 
		m_Eep.DC_Fail_Volt_x10[DC_Main] = 0;
	
		
	if(m_Eep.UL_Gain_A_BackOff > OFF) 
		m_Eep.UL_Gain_A_BackOff = OFF;	

	// 2020.01.20
	m_St.UL_Gain_A_BackOff = m_Eep.UL_Gain_A_BackOff;		
#endif


	// 2019.05.06 : Temporary
	for (i=0;i<AMPU_4_MAX;i++)
	{		
		if(m_Eep.Other_Rvs_IN_ALC_Onoff[i] != ON)
			m_Eep.Other_Rvs_IN_ALC_Onoff[i] = OFF;
	}

	// 2019.05.09
	if(m_Eep.Threshold_UL_Spurious_x10 < -1000) 
		m_Eep.Threshold_UL_Spurious_x10 = 0;		
	if(m_Eep.Threshold_UL_Spurious_x10 > 1000) 
		m_Eep.Threshold_UL_Spurious_x10 = 0;

	// 2019.05.17
	if(m_Eep.Threshold_UL_Gain_Low_x10 < -1000) 
		m_Eep.Threshold_UL_Gain_Low_x10 = 0;		
	if(m_Eep.Threshold_UL_Gain_Low_x10 > 1000) 
		m_Eep.Threshold_UL_Gain_Low_x10 = 0;
		

	for(i=0;i<AMPU_4_MAX;i++)
	{
		m_Eep.UL_Gain_Low_x10[i] = m_Eep.Threshold_UL_Gain_Low_x10;
	}

	if(m_Eep.DL_Mode_En > 1)
		m_Eep.DL_Mode_En = 0;

	
	
	 
	#ifdef __TYPE__PRU__
	for (i=0;i<AMPU_4_MAX;i++)
	{ 
		if(m_Eep.Temp_Offset_x10[i] > 500) // 50.0
			m_Eep.Temp_Offset_x10[i] = 0;
		if(m_Eep.Temp_Offset_x10[i] < -500) // -50.0
			m_Eep.Temp_Offset_x10[i] = 0;	
	}
	#endif

	

	// 2019.05.10
	for (c=0;c<2;c++)	
	{
		for (i=0;i<AMPU_4_MAX;i++)	
		{
			if(m_Eep.RSSI_Offset_x10[c][i] > 500)
				m_Eep.RSSI_Offset_x10[c][i] = 0; 				
			if(m_Eep.RSSI_Offset_x10[c][i] < (-500))
				m_Eep.RSSI_Offset_x10[c][i] = 0;
			
			if(m_Eep.TSSI_Offset_x10[c][i] > 500) // 2019.05.10
				m_Eep.TSSI_Offset_x10[c][i] = 0; 				
			if(m_Eep.TSSI_Offset_x10[c][i] < (-500)) // 2019.05.10
				m_Eep.TSSI_Offset_x10[c][i] = 0;	

			// 2019.05.30
			if(m_Eep.d_UL_GainBal_x10[c][i] < 0) 
				m_Eep.d_UL_GainBal_x10[c][i] = 0;		
			if(m_Eep.d_UL_GainBal_x10[c][i] > 150) 
				m_Eep.d_UL_GainBal_x10[c][i] = 0;		
		}
	}
	
	// 2019.05.
	if(m_Eep.TempComp_Onoff != ON) // if not 'ON' then 'OFF', AAU Only
		m_Eep.TempComp_Onoff = OFF;

	
	if((m_Eep.AAU_48V_Limit_x10[_UP_] < 0)||(m_Eep.AAU_48V_Limit_x10[_DN_] < 0))
	{	
		m_Eep.AAU_48V_Limit_x10[_UP_] = 570;	
		m_Eep.AAU_48V_Limit_x10[_DN_] = 410;	
	}

	/*
	* 2019.08.01 : FAN Mode Init : from PSU Task -> Alarm Task
	*/
	#ifdef __TYPE__PRU__
	if(m_Eep.FAN_Mode_Onoff > FAN_Mode_OFF) // PRU & AAU
		m_Eep.FAN_Mode_Onoff = FAN_Mode____Normal;
	#else
	// 2019.08.29 : Always Low_Noise_Mode
	m_Eep.FAN_Mode_Onoff = FAN_Mode_Low_Noise;
	#endif
		

	
	#ifdef __TYPE__PRU__
	if(m_Eep.MIMO_Stdby_Level_x10[_IN_] < 0)
		m_Eep.MIMO_Stdby_Level_x10[_IN_] = 340;
	if(m_Eep.MIMO_Stdby_Level_x10[_OUT_] < 0)
		m_Eep.MIMO_Stdby_Level_x10[_OUT_] = 340;
	#else // ANTS-AAU-Only
	if(m_Eep.MIMO_Stdby_Level_x10[_IN_] < 0)
		m_Eep.MIMO_Stdby_Level_x10[_IN_] = 120;
	if(m_Eep.MIMO_Stdby_Level_x10[_OUT_] < 0)
		m_Eep.MIMO_Stdby_Level_x10[_OUT_] = 120;
	#endif	

	if(m_Eep.mask > 1) m_Eep.mask = 0; // 2019.05.01
	m_Eep.mask = 0;


	// 2019.07.10
	//-----------------------------------------------------------
	for(c=0, i=0; i<11; i++) // 범위밖 char를 0 으로 설정.
	{
		if(Is_Visible_Char(m_Eep.User_ID[i])==0) c=1;
		if(c) m_Eep.User_ID[i] = 0;
	}
	if(Is_Visible_Char(m_Eep.User_ID[0])==0)
	{
		memcpy( m_Eep.User_ID, User_ID_Def, 12);
	}
	//----------------------------------
	for(c=0, i=0; i<11; i++) // 범위밖 char를 0 으로 설정.
	{
		if(Is_Visible_Char(m_Eep.User_PW[i])==0) c=1;
		if(c) m_Eep.User_PW[i] = 0;
	}	
	if(Is_Visible_Char(m_Eep.User_PW[0])==0)
	{
		memcpy( m_Eep.User_PW, User_PW_Def, 12);
	}
	//---------------------------------------------------------
	memset(RU_ARM_SUPP, 0, RU_ALARM_CNT);	

	// 2019.08.28 : default ON	
	m_Eep.Height_Bal_Onoff = 1; // ON(1)	
	
	Set_Factory_Mode_Onoff(OFF); // default OFF

	/*
	2019.08.13 : 파형감시 DL/UL Offset
	*/
	Init___Spectrum_Offset();



	
	/*
	2020.04.02 : Use AID 0xC090~92	
	*/
	if(m_Eep.AAU_LS_PLL_def_set_20200402 != 0x20200402)
	{
		m_Eep.AAU_LS_PLL_def_set_20200402 = 0x20200402;
		eep_save_req = 1;
		
		for(c=0; c < LSH_Max; c++)
		{
			m_Eep.d_Reg_PLL_Set[c][0] = 0xD0;
			
			for(i=1; i<4; i++)
				m_Eep.d_Reg_PLL_Set[c][i] = 0xD4;
		}
	}

	if(eep_save_req)
	{
		m_Env.is_eep_write = EEP_WRITE_CODE;
	}
}



//-------------------------------------------------
//
OS_TIMER Local_Terminal_time;

void Reset_Local_Timer(void)
{
	OSTimerReset( &Local_Terminal_time );
}

void Init_Local_Termial(void)
{
	Local_Terminal = 0;
	BlueT_Terminal = 0;
	Reset_Local_Timer();
}

////// YCSO added START // 191204
#ifdef __TYPE__AAU__
OS_TIMER BT_Re_Init_Timer; // BT ReInit needed when timeout
void Reset_BT_Re_Init_Timer( void )
{
	OSTimerReset( &BT_Re_Init_Timer );
}
#endif

// Local GUI Connection Alarm
void Update_GUI_Alm(void)
{	
	if(Local_Terminal || BlueT_Terminal)
	{	
		if(OSTimerGet_msec(&Local_Terminal_time) > (60*TICK_CO_SEC))
		{
			Local_Terminal = 0; // disconnect with GUI
			BlueT_Terminal = 0;
			
		#ifdef __TYPE__AAU__
			printf("Re Init BT disconnect GUI \n");
			Set_Bt_req(Get_Bt_status()); // 2019.08.15(v08.14.99)
			//m_Env.Bluetooth_re_Init_Req = _SET_;
			Reset_BT_Re_Init_Timer(); // 2020.03.25 : for BT_Init every 10m
		#endif
		}
	#ifdef __TYPE__AAU__
		if(BlueT_Terminal == 0) // 2020.03.25 added
		{
			Reset_BT_Re_Init_Timer(); // 191204
		}
		else if(BlueT_Terminal == 1)
		{
			// When BT data used, BT_Re_Init_Timer is cleared...
			if(OSTimerGet_msec(&BT_Re_Init_Timer) > (20*TICK_CO_SEC))
			{
				/*
				------------------------------------
				2020.03.25 
				------------------------------------
				BT initialize 추가 (for BT-Re-Connection)
				(Serial GUI 연결과 관계없이) BT 데이터가 20s정도 수신되지 않는 경우 BT initialize.
				------------------------------------
				*/
				printf("Re Init BT by 20s disconnect \n");
				Set_Bt_req(Get_Bt_status());
				Reset_BT_Re_Init_Timer();
				BlueT_Terminal = 2; // '2' means disconneted for 20s but GUI.Alarm is not cleared(60s)
			}
		}
	#endif
	}	
#ifdef __TYPE__AAU__ // 191204
	else
	{
		if(OSTimerGet_msec(&BT_Re_Init_Timer) > (10*TICK_CO_MIN)) // every 10m
		{
			printf("Re Init BT 10 MIN disconnect GUI \n");
			Set_Bt_req(Get_Bt_status()); // 2019.08.15(v08.14.99)
			Reset_BT_Re_Init_Timer();
		}
	}
#endif

	if(m_St.AlmFlag.A_0.GUI) 
		m_St.AlmSt.A_0.GUI = m_St.AlmCtrl.A_0.GUI;	
	else
		m_St.AlmSt.A_0.GUI = (Local_Terminal | BlueT_Terminal);
}

////// YCSO added END 191204

//---------------------------------------------------------------------------
INT8U PAU_FWD_AMP_St( INT8U path4 )
{
	return AMP_Alarm_St(path4);
}
//---------------------------------------------------------------------------

/*
--------------------------------------+----------
0x5117 : Disabled Alarm
--------------------------------------+----------
User에 의하여 Off 된 경우			  | User Control
System Algorithm에 의하여 Off 된 경우 | System Control
--------------------------------------+----------
0x5115 : Shutdown Alarm
--------------------------------------+----------
TSSI 상한 초과로 인한 SD			  | Overinput
Over Power로 인한 SD				  | Overpower
CPRI Unlock으로 인한 SD 			  | CPRI
Clock Unlock으로 인한 SD			  | Clock
UL 과입력에 의한 SD 				  | UL Overinput
UDA에 의한 SD						  | UDA
--------------------------------------+----------
*/


void Disp_AMP_St(INT8U path4)
{
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Disp_AMP_St] AMP#%d Dis [%s], SD [%s], Amp_En [%s]",
				path4+1, 
				Get_Alarm_Str(m_St.AlmSt.Path[path4].PAU.Disabled),
				Get_Alarm_Str(m_St.AlmSt.Path[path4].PAU.Shutdown),
				Get_OnOff_Str(m_St.Pwr_Amp_En[path4])); 
}






INT8U AMP_Alarm_St(INT8U path4)
{	
	if( m_St.AlmSt.Path[path4].PAU.Disabled ) return OFF;
	if( m_St.AlmSt.Path[path4].PAU.Shutdown )
	{
		/*
		UL 과입력(AAU,PRU)의 경우,
		30m구간외에 일부 정상구간의 경우에도 Shutdown alarm(0x5115)가 발생해야 하므르, 
		Shutdown alarm(0x5115) 이 발생 했다고 해서, AMP_Off라고 판단하면 안됨.

		RVS.Over.IN alarm의 경우, SD alarm 인데, AMP-ON 의 경우가 있음.
		*/
		if(m_St.Pwr_Amp_En[path4] == ON) // 2019.12.26
			return ON;
		else
			return OFF;
	}
	else 
	{
		return ON;
		
		/*
		실제 off 인데, 
		alarm이 강제로 normal 처리 되어서, 
		on으로 응답될 경우가 문제가 될 수 있음.
		
		// 2020.04.13 : 아래가 맞을 것 같음.
		if(m_St.Pwr_Amp_En[path4] == ON)
			return ON;
		else
			return OFF;
		*/	
	}
}


void SetResetInfo( INT32U rst_info, int idx )
{
	print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[SetResetInfo] reason %d, line %d, %s.", 
			rst_info, idx, Get_Rst_Reason_Str(rst_info));
	
	m_rst_info.reset_info = rst_info;
	ResetInfo_Write((INT8U*)&m_rst_info, sizeof(DownInfoSTR));
	rcu_reset_req = rst_info;
}

//-------------------------------------------------



INT8U Check_PAU_Use(INT8U path4)
{
	if(m_St.LayerUseInfo[path4] == AMP_USE) 
		return 1;
	else return 0;
}
//----------------------------------------------------------------------
void Init_AMP_Install(void)
{
#ifdef __TYPE__AAU__ // AAU (ANTS, HFR)
	memset(m_St.AMP_HW_Insert, _PAU_Insert_, AMPU_4_MAX);
#else //--------------- PRU (ANTS, HFR)
	memset(m_St.AMP_HW_Insert, _PAU_Remove_, AMPU_4_MAX);
#endif	
}
//----------------------------------------------------------------------
INT8U Is_RU_SISO(void)
{
	switch(m_Eep.Layer_Mode)
	{
		case Oper_SISO:
			return 1;
			
		default:
			return 0;
	}
}
//----------------------------------------------------------------------
// Check self(path4) is MIMO-AMP or not
INT8U Is_MIMO_AMP(INT8U path4) 
{
	switch(m_Eep.Layer_Mode)
	{
		case Oper_MIMO:
			if(path4 == AMPU_4_1) return 0;
			else return 1;

		default:
			return 0;
	}
}
//----------------------------------------------------------------------
// Check self(path4) is SISO-AMP or not
INT8U Is_SISO_AMP(INT8U path4)
{
	return (Is_MIMO_AMP(path4)^1); // toggle
}
//----------------------------------------------------------------------
INT8U Get_AMP_SS_MM(INT8U path4)
{
	switch(m_Eep.Layer_Mode)
	{
		case Oper_MIMO:
			if(path4 == AMPU_4_1) return _SS_;
			else return _MM_;

		default:
			return _SS_;
	}
}
//----------------------------------------------------------------------
INT8U Get_AMP_Pair(INT8U path4)
{
	switch(m_Eep.Layer_Mode)
	{
		case Oper_MIMO:			
			if(path4 == AMPU_4_2) return AMPU_4_1;
			if(path4 == AMPU_4_3) return AMPU_4_1;
			if(path4 == AMPU_4_4) return AMPU_4_1;
			return AMPU_4_1;			

		default:
			return path4; // self, Oper_SISO
	}
}
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void Download_Mode_Init( void )
{	
	m_Env.DnMode.Rcu = _DN_RST_AUTO;
	m_Env.DnMode.Dig = _DN_RST_AUTO;
}


void RepeaterInit( void )
{	
	DEV_TYPE dev;	
	INT8U i;
	
	Download_Mode_Init();
		
	//-------------------------
	// EEP Load
	//-------------------------
	LoadEEP();

///// YCSO added START 190503
	#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-PRU	
	LoadEEPTable();
	#endif
///// YCSO added END
	
	m_St.Maker[0] = MAKER_ANTS;
	m_St.Maker[1] = MAKER_ANTS;
	

	Ro_IP_Set_Req=1; // 광정상되면, 체크하도록.	

	m_Eep.MY_SUBID[0] = 0xFF;
	m_Eep.MY_SUBID[1] = 0xFF;
	m_Eep.MY_SUBID[2] = 0xFF;

	SUB_ID_Check(m_Eep.MY_SUBID, &dev); 

	def_IP_Info_Set();

	init_con();

	// 2019.04.25 : when eep_A file is not, for generate it.
	m_Eep.reset_cnt++;
	m_Env.is_eep_write = EEP_WRITE_CODE;
	
	//-----------------------------------------------
#ifdef __TYPE__PRU__
	m_St.Type[0] = REPEATER_TYPE_1;
	m_St.Type[1] = REPEATER_TYPE_5G_PRU;
	
	m_St.Rep_Type = TYPE_5G_PRU;
#endif

	#ifdef __TYPE__AAU__ 
	// 2019.06.03
	m_Eep.AAU_Type[0] = REPEATER_TYPE_1;	
	if((m_Eep.AAU_Type[1] != REPEATER_TYPE_5G_AAU_C)&&(m_Eep.AAU_Type[1] != REPEATER_TYPE_5G_AAU_W))
	{
		m_Eep.AAU_Type[1] = REPEATER_TYPE_5G_AAU_C;
	}
	m_St.Type[0] = m_Eep.AAU_Type[0];	
	m_St.Type[1] = m_Eep.AAU_Type[1];
	m_St.Rep_Type = TYPE_5G_AAU;
	#endif

	////Chk_Auto_Reset_By_UDA_Suppress(); // 2019.05.03. delete and move
	//-------------------------------------------------------
	m_Ro.id = 0;

	memset(m_Env.task_flag, _SET_, _TASK_MAX_NUM_);		///// YCSO TASK END
	memset(m_Env.task_END_status, _SET_, _TASK_MAX_NUM_);		///// YCSO TASK END

	for(i=0; i < _TASK_MAX_NUM_; i++)
	{
		m_Env.task_loop[i] = 0;
		m_Env.task_cnt[i] = 0;
		m_Env.task_cnt_prev[i] = 0;
	}

	Get_KST_inventory();
}

void Update__Task_Count(INT8U i)
{
	if(i >= _TASK_MAX_NUM_) return;

	m_Env.task_loop[i] = 1;	
	m_Env.task_cnt_prev[i] = m_Env.task_cnt[i];
	m_Env.task_cnt[i]++;
}

INT8U IS__Task_Count_Update(INT8U i)
{
	if(i >= _TASK_MAX_NUM_) return 1; // no problem
	if(m_Env.task_loop[i] == 0) return 1; // no problem
	
	if(m_Env.task_cnt_prev[i] != m_Env.task_cnt[i])
		return 1;
	else
		return 0; // fail
}

//------------------------------------------------------------
static OS_TIMER Timer_Chk_Task_Working;
void Proc___Task_Working(void)
{
	static INT8U init = 0;	
	static int stop_cnt[_TASK_MAX_NUM_];
	INT8U i;

	if(init == 0)
	{
		init = 1;
		OSTimerReset(&Timer_Chk_Task_Working);		
		for(i=0; i < _TASK_MAX_NUM_; i++)
			stop_cnt[i] = 0;
	}

	// every 3[s]
	if(OSTimerGet_msec(&Timer_Chk_Task_Working) > 3*TICK_CO_SEC)
	{	
		OSTimerReset(&Timer_Chk_Task_Working);
		
		for(i=0; i < _TASK_MAX_NUM_; i++)
		{
			if(m_Env.task_loop[i] == 0)
				continue;
		
			if(IS__Task_Count_Update(i))
				stop_cnt[i] = 0;
			else
				stop_cnt[i]++;
						
			if(stop_cnt[i] > 1) // if(stop_cnt[i] >= 10)
			{
				print_agent(DEBUG_SYSTEM, VT_RED, "[Proc___Task_Working] %d, Cnt %d", 
						stop_cnt[i]);
			}
		}
	}
}
//------------------------------------------------------------


//------------------------------------------------------------
// 2019.08.15(v08.14.99)
void Disp_version_only(void)
{
	printf("%02d.%02d.%02d\n", SW_R_VER1, SW_R_VER2, SW_R_VER3);
}

void Disp_Version_Console(void)
{	
	printf(VT_BOLD_BLUE "-------------------------------------------" VT_END_CR);
	printf(VT_BOLD_BLUE " SYS-APP V%02d.%02d.%02d" VT_END_CR, SW_R_VER1, SW_R_VER2, SW_R_VER3);
	printf(VT_BOLD_BLUE " Main.Ver %02d.%02d.%02d" VT_END_CR, SW_Main_VER1, SW_Main_VER2, SW_Main_VER3);
///	printf(VT_BOLD_BLUE "		 Made On %s, %s\n"   VT_END_CR , __TIME__, __DATE__);	
	printf(VT_BOLD_BLUE "-------------------------------------------" VT_END_CR);
	
	#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
		printf(VT_BOLD_WHITE "ANTS-PRU Repeater." VT_END_CR);
	#endif

	#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU	
		printf(VT_BOLD_WHITE "ANTS-AAU Repeater." VT_END_CR);
	#endif
}
//----------------------------------------------------------------------

void Alarm_Mask_by_Service_Use( void )
{	
#if 0 // not used @ 5G
	INT8U path4;

	if(ALM_JIG_MODE == ALM_JIG_MODE_CODE) 
		return;
		
	// green mode 동작하면, DC Fail mask
	if(m_St.Green_Mode_Req == ON)  
	{		
		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
			m_St.AlmSt.Path[path4].PAU.DC_Fail = 0;
	}
#endif	
}

//----------------------------------------------------------------------

INT8U IS__Standby_Status(INT8U path4)
{
	INT8U need_adj = 0; // init

	if(path4 >= AMPU_4_MAX) return 0;

#if 0
	need_adj |= IS__Mimo_Standby_AMP_Off_flag(path4);
#else
	need_adj |= m_St.MIMO_Stdby_St[path4];
	need_adj |= m_St.Mimo_Stdby_St_Dly_REQ[path4];	
#endif
	
	need_adj |= m_St.AMP_Stdby_St[path4]; // if amp-standby 동작 중
	need_adj |= m_St.AMP_Stdby_St_Dly_REQ[path4];

	return need_adj;
}

INT8U IS__Standby_Dly_REQ(INT8U path4)
{
	INT8U need_adj = 0; // init

	if(path4 >= AMPU_4_MAX) return 0;	

	need_adj |= m_St.Mimo_Stdby_St_Dly_REQ[path4];	
	need_adj |= m_St.AMP_Stdby_St_Dly_REQ[path4];

	return need_adj;
}


INT8U IS__TSSI_Not_Checking(INT8U path4)
{
	static INT8U prev_val[AMPU_4_MAX] = {0,0,0,0};
	
	if((m_St.Pwr_Amp_En[path4] == OFF) && IS__Standby_Status(path4))	
	{
		prev_val[path4] = 1;
		return 1; // not checking	
	}
	else if(IS__Standby_Dly_REQ(path4))
	{
		prev_val[path4] = 1;
		return 1; // not checking
	}	
	else
	{
		if(prev_val[path4] == 1) 
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[AMP_%d] TSSI__Checking Enabled.", path4);
		
		prev_val[path4] = 0;
		return 0; // checking
	}
}


static ALM_ST last_Alm;
INT8U Chk_Sum_Arm( void )
{
	INT8U sum = 0;
	INT8U i;
	INT8U* pU8;

	//----------------------
	memcpy(&last_Alm, &m_St.AlmSt, RU_ALARM_CNT );
		
	if(ALM_JIG_MODE != ALM_JIG_MODE_CODE) // normal operation
	{
		Alarm_Mask_by_Service_Use();

		#ifdef __TYPE__AAU__
		AC_Extension_St = _AC_NORMAL_;
		#endif
		
		if(AC_Extension_St != _AC_NORMAL_) // if AC Alarm
		{
			AC_Alarm_Amp_Alarm_Adj(&last_Alm);			
		}
		else // if AC Normal		
		{	
			Get_Amp_stdby_st_Sum();
			for(i = 0; i < AMPU_4_MAX; i++)
			{
				if(Check_PAU_Use(i))
				{
					//if(IS__Standby_Status(i))
					if(IS__TSSI_Not_Checking(i))
					{
						AMP_Stby_Amp_Alarm_Adj(&last_Alm, i);
					}
				}
			}			
			/*
			// 2020.01.10 : delete below
			//if(MM_Std_St)
			//	Mimo_Stby_Amp_Alarm_Adj(&last_Alm);
			*/
		}
	}
	
	memcpy(&m_St.Last_Alm, &last_Alm, RU_ALARM_CNT );

	// sum alarm -------------------------------------
	pU8 = (INT8U*)&last_Alm;
	last_Alm.A_0.GUI = 0; // exclude GUI-alarm	
	
	for(sum=0, i=0; i< RU_ALARM_CNT; i++)
	{
		if(pU8[i] > 0) 
		{
			sum = 1;
			break;
		}
	}	
	return sum;
}
//----------------------------------------------------------------------
void Update_Door_Alm(void)
{
	INT8U alm = 0;	

	#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
	alm = GPIO_Get(GPIO_DOOR_ALARM) ^ 0x01; // toggle
	#endif

	if(m_St.AlmFlag.A_0.Door) 
		 m_St.AlmSt.A_0.Door = m_St.AlmCtrl.A_0.Door;	
	else m_St.AlmSt.A_0.Door = alm;	
}
/*
----------------------------------------------------------------------
	Update
	HW insertion of PAU @ PRU
----------------------------------------------------------------------
*/
void Update_PAU_HW_Insert(void)
{
#ifdef __TYPE__PRU__ // PRU	(ANTS, HFR)
	INT8U hw_install[AMPU_4_MAX]; //  '1' is not insert
	
	// ANTS-PRU	
	hw_install[AMPU_4_1] = GPIO_Get(GPIO_PAU_0_INS_I); // GPIO 'Low' is '_PAU_Insert_'
	hw_install[AMPU_4_2] = GPIO_Get(GPIO_PAU_1_INS_I); // GPIO 'Low' is '_PAU_Insert_'
	hw_install[AMPU_4_3] = GPIO_Get(GPIO_PAU_2_INS_I); // GPIO 'Low' is '_PAU_Insert_'
	hw_install[AMPU_4_4] = GPIO_Get(GPIO_PAU_3_INS_I); // GPIO 'Low' is '_PAU_Insert_'

	memcpy(m_St.AMP_HW_Insert, hw_install, AMPU_4_MAX);
#endif	
}
//-------------------------------------------------------------------------
/*
	2019.06.29
		Optic Switch Status0 
		(1) Optic Switch 설치(high)/미설치(low)
		(2) AID 0x501C : 기존 제어 -> 상태 핀 감시로 변경
*/
void Proc___PRU_Optic_Switch_St(void)
{
#ifdef __TYPE__PRU__ // ANTS-PRU Only
	INT8U hw_read;

	hw_read = GPIO_Get(GPIO_OPT_SW_Status0);
	
	if(hw_read) m_St.OSM_Use = 1; // installed
	else        m_St.OSM_Use = 0; // not installed
	
	/*
	//if(m_Eep.OSM_Use)
	//{
	//	if(hw_read) m_St.OSM_Use = 1; // installed
	//	else        m_St.OSM_Use = 0; // not installed
	//}
	//else m_St.OSM_Use = 0;	
	*/
#endif	
}

//----------------------------------------------------------------------
void Update_Temp_Alm(void)
{
	if(m_St.AlmSt.A_0.Temp_Up)
	{
		if((m_St.Temp_x10 + 50) <= m_Eep.TempLmt_x10) // 2019.05.25 : 2 -> 5
			m_St.AlmSt.A_0.Temp_Up = 0;
	}
	else
	{
		if( m_St.Temp_x10 >= m_Eep.TempLmt_x10 ) 
			m_St.AlmSt.A_0.Temp_Up = 1;
	}	

	if(m_St.AlmSt_prev.A_0.Temp_Up != m_St.AlmSt.A_0.Temp_Up)			
		Update_Alm_Str_Lmt( 0x50, 0x13, 0, 0, m_St.AlmSt.A_0.Temp_Up); // 함체 온도 상한	

	m_St.AlmSt_prev.A_0.Temp_Up = m_St.AlmSt.A_0.Temp_Up;

	//D_Printf( DEBUG_ETC, "[RCU] Update_Temp_Alm : temp %d limit %d alarm %d, (%d,%d)", 
	//	m_St.Temp_x10, m_Eep.TempLmt_x10, m_St.AlmSt.A_0.Temp_Up);
}


//----------------------------------------------------------------------
void Update_FAN_Alm(void)
{
#ifdef __TYPE__PRU__
	INT8U alm = 0; 
		
	//-------------------------------------------------
	// Ext. FAN Alarm
	//
	if (m_St.FAN_OnOff[EXT_FAN] == OFF)
	{	
		m_St.AlmSt.A_0.FAN1_Case = 0; // mask		
	}
	else
	{
		alm = Is_Ext_FAN_Alm();		
		m_St.AlmSt.A_0.FAN1_Case = alm;
	}
	if(m_St.AlmTest)
	{
	/*
		if(m_St.AlmFlag.A_0.FAN1_Case) // 2019.05.01
			m_St.AlmSt.A_0.FAN1_Case = m_St.AlmCtrl.A_0.FAN1_Case;	

		if(m_St.AlmFlag.A_0.FAN2_PSU) // 2019.05.01
			m_St.AlmSt.A_0.FAN2_PSU = m_St.AlmCtrl.A_0.FAN2_PSU;	

		if(m_St.AlmFlag.A_0.FAN1_Case) // 2019.05.01
			m_St.AlmSt.A_0.FAN1_Case = m_St.AlmCtrl.A_0.FAN1_Case;		
	*/
		return;	
	}
		
	//-------------------------------------------------
	// Int.(PSU) FAN Alarm
	//
	if(m_St.AlmSt.A_0.Door == 0) // door closed
	{		
		if (m_St.FAN_OnOff[INT_FAN] == OFF)
		{		
			// Int.PSU FAN mask
			m_St.AlmSt.A_0.FAN2_PSU = 0;
		}
		else // ON 제어시 알람 체크.
		{	
			alm = m_PSU.PSU_FAN_St;
			m_St.AlmSt.A_0.FAN2_PSU = alm;
		}
	}
	else // Door open
	{		
		// @ iRo : keep previous alarm
		// @ 5G  : alarm check by ksh@ants
		alm = m_PSU.PSU_FAN_St;
		m_St.AlmSt.A_0.FAN2_PSU = alm;
	}
	
	//-------------------------------------------------
#else
	INT8U alm = 0; 
	// 2019.04.22 : AAU FAN Alarm Added.	
	#if 0
	if (m_St.FAN_OnOff[EXT_FAN] == OFF)
	{
		m_St.AlmSt.A_0.FAN1_Case = 0; // mask
	}
	else
	{
		if(m_St.d_Volt_DTU_Sub[0] < 15) // below 1.5[V]
			m_St.AlmSt.A_0.FAN1_Case = 0; // normal, 2019.05.27
		else
			m_St.AlmSt.A_0.FAN1_Case = 1;
	}	
	#else // 2020.01.15 : fan alarm time delay : 미적용.
	if (m_St.FAN_OnOff[EXT_FAN] == OFF)
	{
		m_St.AlmSt.A_0.FAN1_Case = 0; // mask
		Init__Alarm_Timer(&m_St.AlmTime.A_0.FAN1_Case);
	}
	else
	{
		if(m_St.d_Volt_DTU_Sub[0] < 15) // below 1.5[V], 2019.05.27
			alm = 0;
		else
			alm = 1;

		if(Chk__alarm_normal_timeout(&m_St.AlmTime.A_0.FAN1_Case, m_St.AlmSt.A_0.FAN1_Case, alm ,3000, 1000)) // alarm 3s
			m_St.AlmSt.A_0.FAN1_Case = 1; // update
		else
			m_St.AlmSt.A_0.FAN1_Case = 0; // update	
	}
	#endif
#endif
}

//----------------------------------------------------------------------

void Update_PSU_Alm(void)
{
#ifdef __TYPE__PRU__	
	INT8U alm_DC;	
	INT8U batt_lo;
	INT8U i;

	//-------------------------------------------------
	if(m_St.AlmTest)
		return;		
	//-------------------------------------------------
	
	//-------------------------------------------------
	alm_DC = 0; // init
	for(i = 0; i < 2; i++) // 0 : main, 1 : sub, (2 : FAN 제외)
	{
		if(m_PSU.DC_UV_Recti[i]) alm_DC = 1;
		if(m_PSU.DC_OV_Recti[i]) alm_DC = 1;
		if(m_PSU.DC_OC_Recti[i]) alm_DC = 1;
	}
	if(m_PSU.Ripple_Alm_5v6) alm_DC = 1;
	if(m_PSU.PSU_St_Etc & cbits[7]) alm_DC = 1;		
	// BATT 알람이면, DC 알람 발생.
	if(m_PSU.Batt_Abnormal)  alm_DC = 1;
	//-----------------------
	// if AC, DC all alarm, clr DC alm
	if(m_St.AlmSt.A_0.AC && alm_DC) alm_DC = 0;		
	m_St.AlmSt.A_0.DC = alm_DC;

	// 2019.11.27
	if(m_St.AlmSt_prev.A_0.DC != m_St.AlmSt.A_0.DC)
	{
		m_St.AlmSt_prev.A_0.DC = m_St.AlmSt.A_0.DC;
		Update_Alm_Str_Lmt( 0x50, 0x17, 0, 0, m_St.AlmSt.A_0.DC); // DC
	}
	//-------------------------------------------------
	batt_lo = m_PSU.Battery_Low;
	// if BATT not used : clr BATT-Low-Alarm
	/*
	2020.02.17 : 외장Batt Low 알람 : 
		AC fail 에 관계없이 외장Batt Low 알람 발생하도록 수정.
	////if(m_St.AlmSt.A_0.AC == 0) // 2019.07.29
	*/
	{
		m_St.AlmSt.A_1.Batt_Low = batt_lo;	
	}
#endif	
}


/*
----------------------------------------------------------------------
2019.05.27
	adc_mV
	1. over  1.208 : return 60.0[V]
	2. below 0.684 : return 35.0[V]
	3. return 35.0[V] + (adc_mV - 0.684)/0.021 
		(ex) when adc_mV is 0.789, return 40.0[V]
----------------------------------------------------------------------
*/
INT16U AAU__Conv_ADC_to_48v_IN(INT16U adc_mV)
{
	     if(adc_mV <  684) return 350; // below 0.684[V]
	else if(adc_mV > 1208) return 600; // over  1.208[V]
	else
	{
		return 350 + 10*((double)(adc_mV - 684)/21.0);
	}
}

/*
----------------------------------------------------------------------
	AID 0xB437/38
	DTU Voltages
----------------------------------------------------------------------
*/
enum
{
	ANTS_PRU_DET__5V6 = 0, // 5.6V
	ANTS_PRU_DET__3V3,  // 3.3V
	ANTS_PRU_DET_0V85, // 0.85V
	ANTS_PRU_DET__1V2, // 1.2V
	ANTS_PRU_DET__1V8, // 1.8V
	ANTS_PRU_DET__0V9, // 0.9V
	
	ANTS_PRU_DET_Max
};
enum
{	
	ANTS_AAU_DET_FAN_Alm = 0, // FAN-Alarm_ADC, 2019.05.27, 0xB438
	ANTS_AAU_DET_48IN_DC, // 48V Input, DC Fail. 0xB437
	
	ANTS_AAU_DET_Max
};






/*
----------------------------------------------------------
2020.02.24 : 
	package download & reboot 되기전, 6~7회에 1회꼴로 
	"omap_i2c 44e0b000.i2c: controller timed out" 나오면서, DC alarm 발생.
	F/W 막기위한 방법으로 FW적용 command 내려오면, ADC Disable 하도록 수정.
----------------------------------------------------------	
*/
#ifdef __TYPE__AAU__
static INT8U aau_adc_en = _SET_; // default set
void Set__AAU_ADC_Chk_En(INT8U endis)
{
	aau_adc_en = endis;

	if(aau_adc_en == _CLR_)
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "adc (for dc fail) is disable by CMD_FW_UPDATE(0x5B)");
}

INT8U IS__AAU_ADC_Chk_En(void)
{
	if(aau_adc_en == _SET_) return 1;
	else return 0;
}
#endif		



void Update_DTU_Voltages(void)
{
	static INT8U cnt = 0;
	//-------------------------------------------------
	if(m_St.AlmTest) // 2019.05.02
	{
		return;
	}
	//-------------------------------------------------	

//-------------------------------------
#ifdef __TYPE__PRU__	
	/*			
	*	2019.07.12 : PRU : DTU 전압 모니터 OFFSET 처리 요청
	*		5.8V(+0.3),	3.3V(+0),  1.8V(-0.1),	1.2V(+0),  0.85V(-0.1)
	*
	*	2019.08.07 : PRU : DTU 전압 모니터 OFFSET 처리 요청
	*		0.85V(-0.05)
	*/

	m_St.d_Volt_DTU        = (2*Get_ADC_voltage(ANTS_PRU_DET__5V6)+300)/100; // '2*' is for voltage-divide, 2019.07.08
	m_St.d_Volt_DTU_Sub[0] = 2*Get_ADC_voltage(ANTS_PRU_DET__3V3)/100;       // '2*' is for voltage-divide, 2019.07.08 
	m_St.d_Volt_DTU_Sub[1] = (2*Get_ADC_voltage(ANTS_PRU_DET__1V8)-100)/100; // '2*' is for voltage-divide, 2019.07.08 
	m_St.d_Volt_DTU_Sub[2] = Get_ADC_voltage(ANTS_PRU_DET__1V2)/100;         // '2*' is deleted, 2019.08.07
	m_St.d_Volt_DTU_Sub[3] = (Get_ADC_voltage(ANTS_PRU_DET_0V85)-50)/100;   // '2*' is deleted, 2019.08.07

	cnt++;
	if(cnt>3)
	{
		cnt = 0;
		//D_Printf( DEBUG_SYSTEM, "[ADC] %d, %d, %d, %d, %d\n", 
		//	m_St.d_Volt_DTU,
		//	m_St.d_Volt_DTU_Sub[0],	m_St.d_Volt_DTU_Sub[1], m_St.d_Volt_DTU_Sub[2], m_St.d_Volt_DTU_Sub[3]);
	}
#else
	INT16U in__mV;
	INT16U fan_mV;
	INT16S sVal16;
	static INT8U dc_fail_cnt = 0;
	INT8U is_dc_fail;

	in__mV = Get_ADC_voltage(ANTS_AAU_DET_48IN_DC);
	fan_mV = Get_ADC_voltage(ANTS_AAU_DET_FAN_Alm);		

	m_St.d_Volt_DTU = AAU__Conv_ADC_to_48v_IN(in__mV) + m_Eep.AAU_48V_Offset_x10;
	m_St.d_Volt_DTU_Sub[0] = fan_mV/100;

	cnt++;
	if(cnt>3)
	{
		cnt = 0;
		//D_Printf(DEBUG_SYSTEM, VT_GREEN "[ADC] 48V_DC-ADC [%d/1000, %d = %d+%d], FAN-ADC [%d/1000]", 
		//	in__mV, 
		//	m_St.d_Volt_DTU, 
		//	AAU__Conv_ADC_to_48v_IN(in__mV), m_Eep.AAU_48V_Offset_x10,
		//	fan_mV);
	}
		
	
	if(m_Eep.AAU_48V_Limit_x10[_DN_] > m_Eep.AAU_48V_Limit_x10[_UP_])
	{
		// swap
		sVal16 = m_Eep.AAU_48V_Limit_x10[_UP_];
		m_Eep.AAU_48V_Limit_x10[_UP_] = m_Eep.AAU_48V_Limit_x10[_DN_];
		m_Eep.AAU_48V_Limit_x10[_DN_] = sVal16;
	}
	     if(m_St.d_Volt_DTU > m_Eep.AAU_48V_Limit_x10[_UP_]) is_dc_fail = 1; // alarm
	else if(m_St.d_Volt_DTU < m_Eep.AAU_48V_Limit_x10[_DN_]) is_dc_fail = 1; // alarm
	else                                                     is_dc_fail = 0; // normal

	/*
	----------------------------------------------------------
	2020.02.24 : 
		package download & reboot 되기전, 6~7회에 1회꼴로 
		"omap_i2c 44e0b000.i2c: controller timed out" 나오면서, DC alarm 발생.
		F/W 막기위한 방법으로 연속 3번 alarm 시, alarm 으로 판단하도록 수정.
	----------------------------------------------------------	
	*/
	if(IS__AAU_ADC_Chk_En())
	{
		if(is_dc_fail) // if dc fail 
		{
			dc_fail_cnt++;
			if(dc_fail_cnt >= 3)
			{
				dc_fail_cnt = 3;
				m_St.AlmSt.A_0.DC = 1;
			}		
		}
		else
		{	
			dc_fail_cnt = 0;
			m_St.AlmSt.A_0.DC = 0;
		}
	}
	//------------------------------------------------------
	
	
	//------------------------------------------------------
	// 2019.11.27
	if(m_St.AlmSt_prev.A_0.DC != m_St.AlmSt.A_0.DC)
	{
		m_St.AlmSt_prev.A_0.DC = m_St.AlmSt.A_0.DC;
		Update_Alm_Str_Lmt( 0x50, 0x17, 0, 0, m_St.AlmSt.A_0.DC); // DC
	}
	
#endif
//-------------------------------------
}






//----------------------------------------------------------------------
void Chk_Alarm_JIG_Timeout(void)
{
	if(ALM_JIG_MODE == ALM_JIG_MODE_CODE)
	{
		if( OSTimerGet_msec( &Alarm_JIG_Timer ) >= (3*TICK_CO_MIN) )
		{
			OSTimerReset(&Alarm_JIG_Timer);
			ALM_JIG_MODE = 0;
			
			// 2019.05.01
			m_St.AlmTest = 0; // clear, 2019.05.01
			memset(&m_St.AlmFlag, 0, RU_ALARM_CNT);
		}
	}
	else OSTimerReset(&Alarm_JIG_Timer);
}	
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void CheckAlarm( void )
{
	INT8U path4;	

	if(ALM_JIG_MODE == ALM_JIG_MODE_CODE)
	{
		m_St.Sum_Alarm = Chk_Sum_Arm();		
		return;
	}
	Chk_Alarm_JIG_Timeout();
	//----------------------------------------------
	
	Update_PSU_Alm();
	Update_Door_Alm();
	Update_Temp_Alm();
	Update_FAN_Alm();	
	Update_GUI_Alm();
	//-------------------------------------------------
	#ifdef __TYPE__AAU__
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4)==0)
			AMP_Not_Use_Alarm_Mask(path4); // for AAU
	}
	#endif		
	//-------------------------------------------------


	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		Update_Fwd_Pwr_Alm(path4);
	}
	Proc__Chk_and_Disp__Out_Upper_Alarm();
	Update_Payload_Alm(); // 2019.04.29
	
	//#if 0 // later @ 5G
	/*		
	//	Proc_Lo_Alm_Mask();
	//	Proc_LowAlm_Mask_at_AMP_Init();		
	*/	
	//#endif		


	//------------------------------------------------- 
	#ifdef __TYPE__PRU__
	Proc_AC_Fail_Batt_Save();	
	#else
	for(path4 = 0; path4 < AMPU_4_MAX; path4++) 
		AC_Fail_Ctrl_Req[path4] = 0;
	#endif
	//-------------------------------------------------	
		
	if(JIG_MODE != JIG_MODE_CODE)
		m_St.Sum_Alarm = Chk_Sum_Arm();	
}



void disp_fwd_out_pwr(INT8U path)
{
	if(m_St.Fwd_Out_Pwr_x10[path] >= 0) return;	
	if(m_Eep.Log_OnOff == OFF) return; // Log ON 일때(AID 0x5453) 만 debug
	/*	
	// 2020.05.26 : delete
	print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Layer[%d] Fwd_Out_Pwr = %.1f[dBm],  DLFB RSSI = %.1f[dBm], reg_FB_RSSI_PEAK = %08d/%08d, reg_FB_RSSI_RMS = %08d/%08d", 
				path, 
				
				(double)m_St.Fwd_Out_Pwr_x10[path]/10.0,
				(double)m_St.d_DL_FB_RSSI_Tot_x10[path]/10.0,

				m_Reg_Sts.General_FA[FA_0].General_FA_MIMO[path].R_FB_RSSI_Peak.uD32,
				m_Reg_Sts.General_FA[FA_1].General_FA_MIMO[path].R_FB_RSSI_Peak.uD32,
				
				m_Reg_Sts.General_FA[FA_0].General_FA_MIMO[path].R_FB_RSSI_RMS.uD32,
				m_Reg_Sts.General_FA[FA_1].General_FA_MIMO[path].R_FB_RSSI_RMS.uD32);
	*/
}

/*
----------------------------------------------------------------------
	Calculation
	of
	DL-Outpower 
	
	-----------------------------------
	Tx AMP Output : Z dBm
	100M  TX_Out_Cal = 10logX + Z
	Alpha TX_Out_Cal = 10logY + Z
	-----------------------------------
	10logX = x - 10log(10^(x/10) + 10^(y/10))
	10logY = y - 10log(10^(x/10) + 10^(y/10))
	-----------------------------------
	x = TSSI(_100) - AMP_Tx_Atten + 10log(BW_100/[BW_100 + BW___A])
	y = TSSI(__A_) - AMP_Tx_Atten + 10log(BW___A/[BW_100 + BW___A])
----------------------------------------------------------------------	
*/
void Proc_DL_OutPwr_Cal(void)
{
	INT8U path;
	INT16S pwr_x10;
#ifdef __TYPE__PRU__
	INT16S Z_x10;
	double x, y;	
	double _10logX, _10logY;
#else	
	double d_TSSI[MAX_Optic];
	INT16S tot_pwr_x10;
#endif

	#ifdef __TYPE__AAU__
	// MIMO Standby에 사용되는 FWD Power
	for(path = 0; path < AMPU_4_MAX; path++)
	{
		d_TSSI[_100M] = (double)(m_St.d_DL_FB_RSSI_x10[_100M][path])/10.0;
		d_TSSI[Alpha] = (double)(m_St.d_DL_FB_RSSI_x10[Alpha][path])/10.0;
	
		tot_pwr_x10 = 100*log10(pow(10.00, d_TSSI[_100M]/10.0)+pow(10.00, d_TSSI[Alpha]/10.0));
	
		m_St.d_DL_FB_RSSI_Tot_x10[path] = tot_pwr_x10;		
	}
	#endif

	
	

	// 100M Only Insert
	if(Is_SFP_Inserted(CPRI_0)) 
	{
		if((IS_Alpha_Mhz_Existing()==0) || (Is_SFP_Inserted(CPRI_1)==0)) // 2019.07.03
		{
			for(path = 0; path < AMPU_4_MAX; path++)
			{
				m_St.DL_OutPwr_Cal_x10[Alpha][path] = 0;
				//--------------------------------------
				if(Check_PAU_Use(path)==0)
				{
					#ifdef __TYPE__AAU__
					m_St.Fwd_Out_Pwr_x10[path] = 0;
					#endif
					m_St.DL_OutPwr_Cal_x10[_100M][path] = 0;
				}
				else
				{
					#ifdef __TYPE__AAU__	
					m_St.Fwd_Out_Pwr_x10[path] = m_St.d_DL_FB_RSSI_Tot_x10[path] + m_St.FWD_Pwr_Offset_x10[path];
					disp_fwd_out_pwr(path);
					#endif		
					if(m_St.Fwd_Out_Pwr_x10[path] > 0)
						m_St.DL_OutPwr_Cal_x10[_100M][path] = m_St.Fwd_Out_Pwr_x10[path];
					else // 2019.07.03
						m_St.DL_OutPwr_Cal_x10[_100M][path] = 0;
				}			
				
				// 2019.12.11 : SFP(100 only or Alpha only)의 경우, 통계 update 추가.
				#ifdef __TYPE__AAU__
				if(m_St.Fwd_Out_Pwr_x10[path] < 0) m_St.Fwd_Out_Pwr_x10[path] = 0;
				
				Update_Stat(&m_Stat.Fwd_Out_Pwr_x10[path], m_St.Fwd_Out_Pwr_x10[path]);	
				#endif
				Update_Stat( &m_Stat.DL_OutPwr_Cal_x10[_100M][path], m_St.DL_OutPwr_Cal_x10[_100M][path] );
				Update_Stat( &m_Stat.DL_OutPwr_Cal_x10[Alpha][path], m_St.DL_OutPwr_Cal_x10[Alpha][path] );
			}
			return; // return...
		}
	}

	// 2019.05.01
	if((Is_SFP_Inserted(CPRI_0)==0) && Is_SFP_Inserted(CPRI_1)) // Alpha Only Insert
	{
		for(path = 0; path < AMPU_4_MAX; path++)
		{
			m_St.DL_OutPwr_Cal_x10[_100M][path] = 0;
			//----------------------------------
			if(Check_PAU_Use(path)==0)
			{
				#ifdef __TYPE__AAU__
				m_St.Fwd_Out_Pwr_x10[path] = 0;
				#endif
				m_St.DL_OutPwr_Cal_x10[Alpha][path] = 0;
			}
			else
			{				
				#ifdef __TYPE__AAU__	
				m_St.Fwd_Out_Pwr_x10[path] = m_St.d_DL_FB_RSSI_Tot_x10[path] + m_St.FWD_Pwr_Offset_x10[path];
				disp_fwd_out_pwr(path);
				#endif		
				if(m_St.Fwd_Out_Pwr_x10[path] > 0)
					m_St.DL_OutPwr_Cal_x10[Alpha][path] = m_St.Fwd_Out_Pwr_x10[path];
				else // 2019.07.03
					m_St.DL_OutPwr_Cal_x10[Alpha][path] = 0;
			}
			#ifdef __TYPE__AAU__
			if(m_St.Fwd_Out_Pwr_x10[path] < 0) m_St.Fwd_Out_Pwr_x10[path] = 0;
			
			Update_Stat(&m_Stat.Fwd_Out_Pwr_x10[path], m_St.Fwd_Out_Pwr_x10[path]);	
			#endif
			// 2019.12.11 : SFP(100 only or Alpha only)의 경우, 통계 update 추가.
			Update_Stat( &m_Stat.DL_OutPwr_Cal_x10[_100M][path], m_St.DL_OutPwr_Cal_x10[_100M][path] );
			Update_Stat( &m_Stat.DL_OutPwr_Cal_x10[Alpha][path], m_St.DL_OutPwr_Cal_x10[Alpha][path] );
		}
		return;
	}


		
	


	// Below : 100M, Alpha All SFP inserted...
	for(path = 0; path < AMPU_4_MAX; path++)
	{		
		if(Check_PAU_Use(path)==0)
		{	
			#ifdef __TYPE__AAU__
			m_St.Fwd_Out_Pwr_x10[path] = 0;
			#endif
			m_St.DL_OutPwr_Cal_x10[_100M][path] = 0;
			m_St.DL_OutPwr_Cal_x10[Alpha][path] = 0;
		}
		else
		{
		//-----------------------------
		#ifdef __TYPE__PRU__
		//-----------------------------
			Z_x10 = m_St.Fwd_Out_Pwr_x10[path];
			if(m_St.Fwd_Out_Pwr_x10[path] == 0)
			{
				m_St.DL_OutPwr_Cal_x10[_100M][path] = 0;
				m_St.DL_OutPwr_Cal_x10[Alpha][path] = 0;
			}
			else // 2019.07.03
			{
				// 2019.07.03
				x = (double)(m_St.d_DL_TSSI_dBm_x10[_100M][path] - 5*m_St.Fwd_Out_Att_x2[path])/10.0;
				y = (double)(m_St.d_DL_TSSI_dBm_x10[Alpha][path] - 5*m_St.Fwd_Out_Att_x2[path])/10.0;

				_10logX = x - 10*log10(pow(10.00, x/10)+pow(10.00, y/10));
				_10logY = y - 10*log10(pow(10.00, x/10)+pow(10.00, y/10));

				pwr_x10 = 10*_10logX + Z_x10;
				if(pwr_x10 < 0) pwr_x10 = 0; // 2019.09.17
				m_St.DL_OutPwr_Cal_x10[_100M][path] = pwr_x10;
				
				pwr_x10 = 10*_10logY + Z_x10;
				if(pwr_x10 < 0) pwr_x10 = 0; // 2019.09.17			
				m_St.DL_OutPwr_Cal_x10[Alpha][path] = pwr_x10;
			}		
		//-----------------------------
		#else // @ AAU 
		//----------------------------- 
			#ifdef __TYPE__AAU__	
			m_St.Fwd_Out_Pwr_x10[path] = m_St.d_DL_FB_RSSI_Tot_x10[path] + m_St.FWD_Pwr_Offset_x10[path];
			disp_fwd_out_pwr(path);			
			#endif	
			// 2019.05.11 : use 'DL_FB_RSSI' as calculation power
			pwr_x10 = m_St.d_DL_FB_RSSI_x10[_100M][path];
			if(pwr_x10 < 0) pwr_x10 = 0; // 2019.09.17
			m_St.DL_OutPwr_Cal_x10[_100M][path] = pwr_x10;
			
			pwr_x10 = m_St.d_DL_FB_RSSI_x10[Alpha][path];
			if(pwr_x10 < 0) pwr_x10 = 0; // 2019.09.17			
			m_St.DL_OutPwr_Cal_x10[Alpha][path] = pwr_x10;
		//-----------------------------
		#endif
		//-----------------------------
		}
		#ifdef __TYPE__AAU__
		if(m_St.Fwd_Out_Pwr_x10[path] < 0) m_St.Fwd_Out_Pwr_x10[path] = 0;
		
		Update_Stat(&m_Stat.Fwd_Out_Pwr_x10[path], m_St.Fwd_Out_Pwr_x10[path]);	
		#endif
		Update_Stat( &m_Stat.DL_OutPwr_Cal_x10[_100M][path], m_St.DL_OutPwr_Cal_x10[_100M][path] );
		Update_Stat( &m_Stat.DL_OutPwr_Cal_x10[Alpha][path], m_St.DL_OutPwr_Cal_x10[Alpha][path] );

		////D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[RCU] Z_x10 = %d/10.", Z_x10);
		////D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[RCU] 10*log(d_bw_ratio___a) (%f), 10*log(d_bw_ratio___a) (%f)", 
		////	10.0*log10(d_bw_ratio_100), 10.0*log10(d_bw_ratio___a));
		////D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[RCU] x (%f), y (%f)", 
		////	x, y);
		////D_Printf( DEBUG_SYSTEM, VT_BOLD_RED "[RCU] _10logX (%f), _10logY (%f)", 
		////	_10logX, _10logY);
	}	
}

/*
----------------------------------------------------------------------
	PSU Reset
	PRU Only
----------------------------------------------------------------------	
*/
void PRU_PSU_Reset_Reg_Ctrl(void)
{
	m_Reg_Flg.PSU_Reset.Data = 1;
	m_Reg_Ctl.PSU_Reset.Data = 0x2; // '0x2' Reset		
	
	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PSU_Reset], m_Reg_Ctl.PSU_Reset.Data, "psu reset"); // 2020.03.15 : add direct control 		
	OSTimeDly(OS_DELAY_500msec);
	
	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PSU_Reset], m_Reg_Ctl.PSU_Reset.Data, "psu reset"); // 2020.03.15 : add direct control 		
	OSTimeDly(OS_DELAY_500msec);
}

void Proc_PSU_Reset(void)
{
	#ifdef __TYPE__PRU__ // ANTS-PRU	
	INT8U psu_reset_req;
	
	if(m_Env.psu_reset_req)
	{
		//--------------------
		// PSU Reset : ANTS-PRU
		//--------------------
		psu_reset_req = 0x2; // '0x2' Reset		
		if ( m_Reg_Sts.PSU_Reset.Data != psu_reset_req )
		{
			// 2020.03.02 : "AID 0x5620 Reset from gREMS"의 경우, gREMS로 응답되게 하기 위한 지연 추가.
			OSTimeDly(OS_DELAY_500msec*3);
			
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "!! PSU Reset REQ(%d) to PRU-FPGA.", m_Env.psu_reset_req);
			
			m_Env.psu_reset_req++;

			/*
			----------------------------------------------
			2020.03.13 : DTU Reset
				AID 0x5620 수신시, system halt로만 처리 
				(S/W Reboot으로 처리는 우선 보류), Reset 원인은 사용자 제어로 유지			
			----------------------------------------------	
			*/
			PRU_PSU_Reset_Reg_Ctrl();
		
			/*
			----------------------------------------------
			2020.03.13 : register 제어 1s 후에는, result 비교 없이, "halt" 실행.
			----------------------------------------------
			*/
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "!! PSU Reset : System Halt(2).");

			if(m_rst_info.reset_info == 0) // previous-control-reason 이 없으면, by-power-control
				SetResetInfo( Rst_By_PwrCtrl, 1869 );
				
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "!! Reset Reason : %s(2)", Get_Rst_Reason_Str(m_rst_info.reset_info));
			m_Env.psu_reset_req = 0;
			system("halt");		
			//----------------------------------
			// 2020.03.24 : waiting reset
			OSTimeDly( 10*OS_DELAY_1sec); 
			print_agent(DEBUG_UDA, VT_BOLD_RED, "[PSU Reset] !! by reboot(2)");								
			m_Env.is_reboot = 1;
			//----------------------------------
		}
		else
		{	
			print_agent( DEBUG_SYSTEM, VT_BOLD_RED, "!! PSU Reset : System Halt.");
			
			if(m_rst_info.reset_info == 0) // 2019.09.19 : previous-control-reason 이 없으면, by-power-control
				SetResetInfo( Rst_By_PwrCtrl, 749 );

			// 2019.10.22 : display reason
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "!! Reset Reason : %s", Get_Rst_Reason_Str(m_rst_info.reset_info));				
			
			m_Env.psu_reset_req = 0;
			system("halt");
			/*
			--------------------------------
			1. for 'halt', RCU termination
			2. 5[s] delay after halt, ANTS-PRU-FPGA will do "PSU-RESET"
			--------------------------------
			*/
			//----------------------------------
			// 2020.03.24 : waiting reset
			OSTimeDly( 10*OS_DELAY_1sec); 
			print_agent(DEBUG_UDA, VT_BOLD_RED, "[PSU Reset] !! by reboot");								
			m_Env.is_reboot = 1;
			//----------------------------------
		}
	}
	#endif	
}

//---------------------------------------------------------
void ClearFwdSD_Mode( INT8U path4 )
{
	Fwd_Amp_SD[path4] = 0 ;
}

void AMP_Not_Use_Pwr_Chk(void)
{		
	INT8U i;
	
	for(i = 0; i < AMPU_4_MAX; i++)
	{
		if(Check_PAU_Use(i)==0) 
		{
			m_St.Fwd_Out_Pwr_x10[i] = 0;			
		}
	}
}


/*
----------------------------------------------------
	if SFP Inserted, return 1
	else return 0
----------------------------------------------------	
*/
INT8U Is_SFP_Inserted(INT8U cpri)
{
	INT8U _100_a = 0;
	INT8U idx = 0;
	
	switch(cpri)
	{
		case CPRI_0: _100_a = _100M; idx = 0; break;
		case CPRI_2: _100_a = _100M; idx = 1; break;
		
		case CPRI_1: _100_a = Alpha; idx = 0; break;		
		case CPRI_3: _100_a = Alpha; idx = 1; break;

		default: return 0;
	}

	if(m_St.d_SFP_Insert_St[_100_a][idx] == __Mount)
		return 1;
	else return 0;		
}


/*
	부팅 후 RCU APP에서 Optic switch ON 이전에 
	하기와 같이 Register 세팅 추가 요청.

	1.	SFP Disable
	2.	Sleep(500msec)
	
	3.	CPRI IP0~3  0xD  0x00  write      AXI
	4.	CPRI IP0~3  0xB  0x2A  write      AXI
	5.	CPRI IP0~3  0xD  0x80  write      AXI      
	6.	CPRI IP0~3  0x8  0x03  write      AXI
	7.	CPRI IP0~3  0x8  0x00  write      AXI
	
	8.	SFP Enable	
	9.	Sleep(500msec)
	
	10.	OSM ON
*/
void Proc___PRU_before_OpticSw_ON(void)
{
/*
#ifdef  __CPRI_SET_When_Reboot__ // ANTS-PRU	

	INT8U i;
	
	printf(VT_GREEN "[RCU] CPRI IP Set ----\ VT_END_CR);
	OSTimeDly(OS_DELAY_1sec); // for Other TASK Ready
	
	// 1.	SFP Disable	
	printf(VT_GREEN "[RCU] CPRI IP Set 1. SFP Disable : reg addr 0x%X, val 0x%X" VT_END_CR, Reg_Common_MIMO_RW_SFP_Reset, 0x00);
	for(i = CPRI_0; i <= CPRI_3; i++ )
		Write_FPGA_Reg(Reg_Common_MIMO_Addr[i][Reg_Common_MIMO_RW_SFP_Reset], 0x00, "SFR_Disable");

	// 2.	Sleep(500msec)		
	printf(VT_GREEN "[RCU] CPRI IP Set 2. Sleep 500[ms]" VT_END_CR);
	OSTimeDly(OS_DELAY_500msec);

	// 3.	CPRI IP0~3  0xD  0x00  write
	printf(VT_YELLOW "[RCU] CPRI IP Set 3. CPRI IP0~3 : mmap addr 0x%X, val 0x%X" VT_END_CR, MMAP_____0x0d/4, 0x00);
	for(i = 0; i < CPRI_PATH; i++) Write_A_PRU_Mmap(i, MMAP_____0x0d, 0x00);

	// 4.	CPRI IP0~3  0xB  0x2A  write
	printf(VT_YELLOW "[RCU] CPRI IP Set 4. CPRI IP0~3 : mmap addr 0x%X, val 0x%X" VT_END_CR, MMAP_____0x0b/4, 0x2A);
	for(i = 0; i < CPRI_PATH; i++) Write_A_PRU_Mmap(i, MMAP_____0x0b, 0x2A);

	// 5.	CPRI IP0~3  0xD  0x80  write
	printf(VT_YELLOW "[RCU] CPRI IP Set 5. CPRI IP0~3 : mmap addr 0x%X, val 0x%X" VT_END_CR, MMAP_____0x0d/4, 0x80);
	for(i = 0; i < CPRI_PATH; i++) Write_A_PRU_Mmap(i, MMAP_____0x0d, 0x80);

	// 6.	CPRI IP0~3  0x8  0x03  write
	printf(VT_YELLOW "[RCU] CPRI IP Set 6. CPRI IP0~3 : mmap addr 0x%X, val 0x%X" VT_END_CR, MMAP____LoopBack/4, 0x03);
	for(i = 0; i < CPRI_PATH; i++) Write_A_PRU_Mmap(i, MMAP____LoopBack, 0x03);
	
	// 7.	CPRI IP0~3  0x8  0x00  write
	printf(VT_YELLOW "[RCU] CPRI IP Set 7. CPRI IP0~3 : mmap addr 0x%X, val 0x%X" VT_END_CR, MMAP____LoopBack/4, 0x00);
	for(i = 0; i < CPRI_PATH; i++) Write_A_PRU_Mmap(i, MMAP____LoopBack, 0x00);
	
	// 8.	SFP Enable	
	printf(VT_GREEN "[RCU] CPRI IP Set 8. SFP Enable : reg addr 0x%X, val 0x%X" VT_END_CR, Reg_Common_MIMO_RW_SFP_Reset, 0x01);
	for(i = CPRI_0; i <= CPRI_3; i++ )
		Write_FPGA_Reg(Reg_Common_MIMO_Addr[i][Reg_Common_MIMO_RW_SFP_Reset], 0x01, "SFR_Enable");
		
	// 9.	Sleep(500msec)
	printf(VT_GREEN "[RCU] CPRI IP Set 9. Sleep 500[ms]" VT_END_CR);
	OSTimeDly(OS_DELAY_500msec);
#endif

	// 10.	OSM ON
	printf(VT_GREEN "[RCU] CPRI IP Set 10. OSM ON ----" VT_END_CR);
	OSM_On_Ctrl();
*/	
}







/*
------------------------------------------------------------------
* 2019.08.06
* 초기 부팅시 PHY 초기화 부분을 추가 
------------------------------------------------------------------
*/
void Init___PRU_PYH_IC(void)
{
#ifdef __TYPE__PRU__
	int re;
	int cnt;

	/*
	*	phy chip reset
	*	mdio-tool w eth0 0x00 0x9140 // S/W Reset
	*	mdio-tool w eth0 0x00 0x1140 // Auto nego enable
	*	mdio-tool r eth0 0x01        // link up 확인
	*/	

	//-------------------------------------------
	cnt = 0;
	do
	{	
		re = system_agent("mdio-tool w eth0 0x00 0x9140");
		cnt++;
		OSTimeDly(OS_DELAY_500msec);
	}while((re != 0)&&(cnt < 3));
	if(cnt >= 3)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "phy_chip_reset fail @ Init"); 
		return;
	}	
	//-------------------------------------------
	cnt = 0;
	do
	{	
		re = system_agent("mdio-tool w eth0 0x00 0x1140");
		cnt++;
		OSTimeDly(OS_DELAY_500msec);
	}while((re != 0)&&(cnt < 3));
	if(cnt >= 3)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "phy_chip auto-nego-enable fail @ Init"); 
		return;
	}
	//-------------------------------------------
	cnt = 0;
	do
	{	
		re = system_agent("mdio-tool r eth0 0x01");
		cnt++;
		OSTimeDly(OS_DELAY_500msec);
	}while((re != 0)&&(cnt < 3));
	if(cnt >= 3)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "phy_chip link-up fail @ Init"); 
		return;
	}
	//-------------------------------------------
	system_agent("mdio-tool w eth0 0x0d 0x001f");
	system_agent("mdio-tool w eth0 0x0e 0x0033");	
	system_agent("mdio-tool w eth0 0x0d 0x401f");
	system_agent("mdio-tool w eth0 0x0e 0x0003");

	system_agent("mdio-tool w eth0 0x0d 0x001f");
	system_agent("mdio-tool w eth0 0x0e 0x0033");
	system_agent("mdio-tool w eth0 0x0d 0x401f");
	system_agent("mdio-tool r eth0 0x0e");

	// success
	print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "phy_chip init-success @ Init"); 	
	
#endif
}


/*
--------------------------------------------	 
	inv.sn[0] = 0x19; // year
	inv.sn[1] = 0x12; // month
	inv.sn[0] = 0x34; // s/n-1
	inv.sn[0] = 0x56; // s/n-2
--------------------------------------------	 	
*/

INT8U Get_ASCII(INT8U val)
{
	if(val < 10) val += '0';
	else		 val += ('A'-10);

	return val;
}

void Make_SN_from_InvSN(INT8U* pInvSn, INT8U* pSN)
{
	INT8U sn[2*SN_SIZE];
	INT8U month;
	INT8U year;
	
	
	memset(sn, 0, 2*SN_SIZE);	
	
#ifdef __TYPE__PRU__ 
	sn[0] = 'P'; // PRU
#else // AAU
	if(m_Eep.AAU_Type[1] == REPEATER_TYPE_5G_AAU_C)
		 sn[0] = 'C'; // Ceiling
	else sn[0] = 'W'; // Wall, REPEATER_TYPE_5G_AAU_W
#endif
	// year---------------
	year = (pInvSn[0]>>4)*10 + (pInvSn[0]&0x0F);
	     if(year < 19) sn[1] = '?';
	else if(year > 44) sn[1] = '?';
	else		       sn[1] = year - 19 + 'A';
	
	// month---------------
	month = pInvSn[1]>>4;	
	     if(month <  1) sn[2] = '?';
	else if(month < 10) sn[2] = month + '0';	
	else if(month <=12) sn[2] = month - 10 + 'A';	
	else		        sn[2] = '?';
	
	// S/N ---------------
	sn[3] = Get_ASCII(pInvSn[1]&0x0F);
	sn[4] = Get_ASCII(pInvSn[2]>>4);
	sn[5] = Get_ASCII(pInvSn[2]&0x0F);
	sn[6] = Get_ASCII(pInvSn[3]>>4);
	sn[7] = Get_ASCII(pInvSn[3]&0x0F);
	
	// Maker ---------------
	sn[8] = 'A'; // 'A' : ANTS	
	sn[9] = 0; // null	

	memcpy(pSN, sn, SN_SIZE);
}
/*
void Proc___Inventory_SN(void)
{
	static INT8U init = 0;
	static INT8U sn_prev[4];
	INT8U regist_auto_num[SN_SIZE];
	INT8U auto_generate_req = 0; 
#ifdef __TYPE__AAU__
	static INT8U AAU_Type[2];
#endif	

	if(init == 0)
	{
		init = 1;
		memcpy(m_St.LS_Regist_Num, m_Eep.LS_Regist_Num, SN_SIZE);
		memcpy(sn_prev, m_Eep.sn, 4);
	#ifdef __TYPE__AAU__	
		AAU_Type[0] = m_Eep.AAU_Type[0];
		AAU_Type[1] = m_Eep.AAU_Type[1];
	#endif	
	}

	if(memcmp(m_St.LS_Regist_Num, "KST", 3)==0) // If KST default
		auto_generate_req = 1;
	else if(m_St.LS_Regist_Num[0] == 0) // If Null
		auto_generate_req = 1;		
	else if(memcmp(sn_prev, m_Eep.sn, 4)) // If user change inventory-S/N
	{
		memcpy(sn_prev, m_Eep.sn, 4);
		auto_generate_req = 1;
	}	
	else 
	{
		auto_generate_req = 0;	
		
	#ifdef __TYPE__AAU__ // 2019.09.17
		if(AAU_Type[0] != m_Eep.AAU_Type[0]) // If Changed	
			auto_generate_req = 1;
		if(AAU_Type[1] != m_Eep.AAU_Type[1]) // If Changed	
			auto_generate_req = 1;	
	#endif		
	}
#ifdef __TYPE__AAU__
	AAU_Type[0] = m_Eep.AAU_Type[0];
	AAU_Type[1] = m_Eep.AAU_Type[1];
#endif
	
	if(auto_generate_req)
	{
		Make_SN_from_InvSN(m_Eep.sn, regist_auto_num);
		
		memcpy(m_St.LS_Regist_Num, regist_auto_num, SN_SIZE);
		m_St.LS_Regist_Num[SN_SIZE-1] = 0;		

		memcpy(AufoConfigFlag.LS_Regist_Num, m_St.LS_Regist_Num, SN_SIZE);
		AufoConfigFlag.LS_Regist_Num[SN_SIZE-1] = 0;		

		print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[S/N] Changed : %s", m_St.LS_Regist_Num);
	}
}
*/


/*
------------------------------------------------------
	If User control inventory.s/n,
	AID 0xB007 LS_Regist_Num is changed.

2020.06.08 기능 변경
-----------------

1. User 인벤토리 변경 시  : 인벤토리 변경 후 인벤토리 eMMC 저장
(1) eMMC 저장된 값이 "KST" or NULL이면, LS 등록번호 자동생성.
	(자동생성 자동생성값은 eMMC 저장하지 않음)
(2) 자동생성하지 않음.


2. User LS 등록번호 제어시 : 
(1) "KST" or NULL 제어 시: 
	 User 제어대로 LS 등록번호 eMMC 저장
	 자동생성 기능 동작.(자동생성 자동생성값은 eMMC 저장하지 않음)
(2) 그외 제어
	User 제어대로 LS 등록번호 eMMC 저장하고, 자동생성 동작하지 않음.

------------------------------------------------------
*/

void Proc___Inventory_SN(void)
{
	static INT8U init = 0;
	static INT8U sn_prev[4] = {0,0,0,0};
	static INT8U LS_Regist_Num[20];
	INT8U regist_auto_num[SN_SIZE];
	INT8U auto_generate_req = 0; // init
#ifdef __TYPE__AAU__
	static INT8U AAU_Type[2] = {0,0};
#endif	

	if(init == 0)
	{
		init = 1;
		memcpy(m_St.LS_Regist_Num, m_Eep.LS_Regist_Num, SN_SIZE);

		memset(LS_Regist_Num, 0, SN_SIZE);
		memcpy(LS_Regist_Num, m_Eep.LS_Regist_Num, SN_SIZE);
		
		memcpy(sn_prev, m_Eep.sn, 4);
	}
	//---------------------------------------
	INT8U is__Null;
	if((memcmp(m_Eep.LS_Regist_Num, "KST", 3)==0) || (m_Eep.LS_Regist_Num[0] == 0))
		is__Null = 1;
	else
		is__Null = 0;
	//---------------------------------------
	auto_generate_req = 0; // init 0	

	if(memcmp(sn_prev, m_Eep.sn, 4)) // If inventory-S/N changed
	{
		memcpy(sn_prev, m_Eep.sn, 4);
		
		if(is__Null)
		{
			auto_generate_req |= cbits[0]; // auto request
			print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[L/S Regist] auto_generate, inventory-S/N changed"); 
		}
	}
	
	if(memcmp(LS_Regist_Num, m_Eep.LS_Regist_Num, SN_SIZE))
	{
		memcpy(LS_Regist_Num, m_Eep.LS_Regist_Num, SN_SIZE);
		
		if(is__Null)
		{
			auto_generate_req |= cbits[1]; // auto request
			print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[L/S Regist] auto_generate, User Regist_Num is Null"); 
		}
	}

	#ifdef __TYPE__AAU__
	if((AAU_Type[0] != m_Eep.AAU_Type[0])||(AAU_Type[1] != m_Eep.AAU_Type[1]))
	{				
		AAU_Type[0] = m_Eep.AAU_Type[0];
		AAU_Type[1] = m_Eep.AAU_Type[1];

		if(is__Null)
		{
			auto_generate_req |= cbits[2]; // auto request
			print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[L/S Regist] auto_generate, AAU type changed"); 
		}
	}
	#endif

	if((memcmp(m_St.LS_Regist_Num, "KST", 3)==0) || (m_St.LS_Regist_Num[0] == 0))
	{
		auto_generate_req |= cbits[7]; // auto request
		print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[L/S Regist] auto_generate, Regist_Num Sts Null"); 
	}
	
	/*
	// 자동생성(inventory-S/N 에 따라)되지만, 
	// eep 저장을 하지 않음.
	*/
	if(auto_generate_req > 0)
	{			
		Make_SN_from_InvSN(m_Eep.sn, regist_auto_num);
		
		memcpy(m_St.LS_Regist_Num,           regist_auto_num, SN_SIZE);
		memcpy(AufoConfigFlag.LS_Regist_Num, regist_auto_num, SN_SIZE);

		m_St.LS_Regist_Num[SN_SIZE-1] = 0;
		AufoConfigFlag.LS_Regist_Num[SN_SIZE-1] = 0;		

		print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[LS Regist-Num] Changed -> %s, req by %#x", 
			m_St.LS_Regist_Num, auto_generate_req);		
	}
}


static OS_TIMER Timer_3sec; // every 3[s]

void Alarm_Process( void )
{
	INT8U co = 50;
	INT8U co_100ms = 0;
	INT32U watchdog_live_max = 0;
	INT32U watchdog_cnt = 0;
	
	Init_Ver_Info();
	Set_Ver_Info_Rcu( 0, m_St.RCU_Ver );
	Init_Local_Termial();

	Get_Total_AID_LIST();      // 2019.02.07. by bkn
		
	m_Eep.reset_cnt++;
	
	m_Env.is_eep_write = EEP_WRITE_CODE;	

	rcu_reset_req = 0; // init

	/*
	RU Reset 시 MIMO Standby Mode 동작 상태 On 표시 이상 수정.
	*/
	m_St.Sum_MIMO_Stdby_St = OFF;

	//--------------------------------------------------------	
	read_network_file();

	print_agent(DEBUG_SYSTEM, VT_MAGENTA, "%s IP :  %d.%d.%d.%d", INTERFACE_FILE,
		m_NetFile.IP_ADDR[0], 
		m_NetFile.IP_ADDR[1], 
		m_NetFile.IP_ADDR[2], 
		m_NetFile.IP_ADDR[3]);

	print_agent(DEBUG_SYSTEM, VT_MAGENTA, "%s MAC : %02X-%02X-%02X-%02X-%02X-%02X", 
		INTERFACE_FILE,
		m_NetFile.IP__MAC[0], m_NetFile.IP__MAC[1], m_NetFile.IP__MAC[2], 
		m_NetFile.IP__MAC[3], m_NetFile.IP__MAC[4], m_NetFile.IP__MAC[5]);
		
	//--------------------------------------------------------	
	Get_system_mac_ip(m_St.init_MAC, m_St.init__IP);

	print_agent(DEBUG_SYSTEM, VT_CYAN, "Read IP (ioctl)  %d.%d.%d.%d", 
			m_St.init__IP[0], m_St.init__IP[1], m_St.init__IP[2], m_St.init__IP[3]);

	print_agent(DEBUG_SYSTEM, VT_CYAN, "Read MAC (ioctl) %02X:%02X:%02X:%02X:%02X:%02X", 
			m_St.init_MAC[0], m_St.init_MAC[1], m_St.init_MAC[2], 
			m_St.init_MAC[3], m_St.init_MAC[4], m_St.init_MAC[5]);
	//--------------------------------------------------------



				
	//--------------------------------------------------------
	
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UDP_DTU %d ", COMM_UDP_DTU);	
#ifdef __TYPE__PRU__ // PRU Only
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UART_AMP_1 %d ", COMM_UART_AMP_1);
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UART_AMP_2 %d ", COMM_UART_AMP_2);
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UART_AMP_3 %d ", COMM_UART_AMP_3);
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UART_AMP_4 %d ", COMM_UART_AMP_4);
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UART___PSU %d ", COMM_UART___PSU);
#endif
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UDP_NODE %d ", COMM_UDP_NODE);
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UDP_LS %d ",   COMM_UDP_LS);
	print_agent( DEBUG_SYSTEM, VT_BOLD_WHITE, " COMM_UDP_REMS %d ", COMM_UDP_REMS);	

	// RF Algorithm
	Init_AMP_Standby_Mode_Proc();
	Init_MIMO_Std_Mode();
	Init_RU_Proc();	
	Init_AMP_Install();	
	Init_UDA_Proc();
	
	// sync with Layter Splitter : int sync
	Is_RU_Time_Sync = 1; 
	OSTimerReset(&Timer_3sec);

	
	#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
	Proc___PRU_before_OpticSw_ON();
	#endif

	Init_Debug_Path4();
	Init__Alm_Str_Lmt(); // 2019.11.27
	Init__All_Alarm_Timer(); // 2019.12.24	
	

	// add alias to "$HOME/.profile"
	system("touch /tmp/.profile; sync");	
	system("echo  \"alias app='cd /usr/app/sys'\" > /tmp/.profile ; sync");
	
	Get__PRU_DDR_Memory_Clk_Info();
		
    while (1)
	{
		//--------------------------------
		OSTimeDly(OS_DELAY_10msec);
		//--------------------------------
		if(JIG_MODE == JIG_MODE_CODE)
		{
			OSTimeDly(OS_DELAY_500msec);
			co++;
			if (co >= 10)
			{				
				co = 0;				
				SaveEEP(); // inventory ctrl을 위해.
			}
			continue;
		}
		//--------------------------------		
		Proc__SuperCap_AC_Alarm_Report(); // Super-Capacitor AC Fail @ PRU
		//--------------------------------
		co_100ms++;
		if(co_100ms >= 100) // every 1[s]
		{				
			co_100ms = 0;				
			m_St.UTC = Get_Cur_UTC_Time(m_St.Ru_Sys_Date);
			watchdog_chk_remained();			
		}
		
		#ifdef __USE_RTC__		
		/*
		//if(m_Env.IS_RTC_Init == 0) // always use
		//{
		//	m_Env.IS_RTC_Init = 1;
		//	m_Env.rtc_refresh_cnt = 60000;
		//}
		*/
		if(m_Env.IS_RTC_Init) 
		{
			m_Env.rtc_refresh_cnt++;
			// every 1 hour = 10ms * 100 * 60 * 60 = 360000
			// every 10 min = 10ms * 100 * 60 * 60 / 6 = 360000/6 = 60000
			if(m_Env.rtc_refresh_cnt > 60000) 
			{
				m_Env.rtc_refresh_cnt = 0;
				system_agent("hwclock --hctosys");
			}
		}
		else 
		{
			m_Env.rtc_refresh_cnt = 0;
		}
		#endif
		//--------------------------------
		if( OSTimerGet_msec( &Timer_3sec ) >= (3*TICK_CO_SEC) )
		{			
			OSTimerReset(&Timer_3sec);
			Update_DTU_Voltages(); // 2019.04.15
			Proc__Get_System_MAC_IP();
			Proc___Inventory_SN();

			if(watchdog_live_max < get_watchdog_live_time_max())
			{
				print_agent( DEBUG_SYSTEM, VT_BOLD_CYAN, "watchdog (%d,%d)", 
					get_watchdog_live_time_min(), get_watchdog_live_time_max());
			}
			watchdog_live_max = get_watchdog_live_time_max();

			if(m_Env.watchdog_cnt == watchdog_cnt)
			{
				print_agent(DEBUG_SYSTEM, VT_WHITE, "watchdog_cnt, %d", m_Env.watchdog_cnt);			
			}
			watchdog_cnt = m_Env.watchdog_cnt;
			
			/*
			D_Printf( DEBUG_SYSTEM, VT_GREEN "[RCU] UL_User: %d, UL_Offs: %d , Backoff %d ",
					m_Eep.d_Att_Dig_UL_User_x10[0][0],
					m_Eep.d_Att_Dig_UL_Offs_x10[0][0],
					m_St.d_RU_UL__Gain_Backoff_x10);

			D_Printf( DEBUG_SYSTEM, VT_GREEN "[RCU] RW_UL_Gain.uD32: %d, conv_x10: %d , Backoff %d ",
					m_Reg_Sts.General_FA[0].General_FA_MIMO[0].RW_UL_Gain.uD32,
					conv_Reg_Gain_to___Att_x10(UL, m_Reg_Sts.General_FA[0].General_FA_MIMO[0].RW_UL_Gain.uD32));
			
			for(i=0;i<4;i++)
			{
				uVal8 = conv_att_025_to_05(m_St.DL_Att[i]);
				uVal16 = conv_att_05_to_01(uVal8);

				D_Printf( DEBUG_SYSTEM, VT_GREEN "[DL_Att[%d] %d, %d, %d, temp %d/ DL_Att.Val_x4 : %d\n",
					i, m_St.DL_Att[i],
					uVal8,
					uVal16,
					m_St.d_Temp_Att_Dig_DL_x10[COMMON][i],
					m_Reg_Sts.RFU[i].DL_Att.Val_x4);
			}
			*/	
		}		
		//--------------------------------
		if( ++co>=100 )
		{			
			co = 0;
			
			Update_PAU_HW_Insert();
			AMP_Not_Use_Pwr_Chk();
			Proc__PRU_Set_MAC_Addr();

			if(Proc_MIMO_Standby())
			{
				m_St.MIMO_Stdby_Sync = 0x0F; // for sync
			}
			Mimo_Stdby_AMP_Ctrl_Chk();

			Get_Amp_stdby_st_Sum();
			AMP_Stdby_AMP_Ctrl_Chk();
			if(Proc_AMP_Standby()) 
			{
				m_St.AMP_Stdby_Sync = 0x0F; // for sync
			}
			
			Proc___Reset_By_UDA();
			Proc___Auto_Rst_UDA(); // 2019.04.29
			Proc___RVS_Spurious_Payload(); // 2019.04.29
			Proc___PRU_Optic_Switch_St(); // 2019.06.29
			
			#ifdef __MAKER_ANTS__ // ANTS-PRU, AAU	
			m_St.Temp_x10 = m_Eep.RCU_Temp_Offset_x10 + Get_LM75_Temp(); // 2019.06.18
			Update_Stat( &m_Stat.Temp_x10, m_St.Temp_x10 );
			m_St.d_DTU_Temp_x10 = m_St.Temp_x10;
			#endif
						
			Proc__Sending_Init_Alarm__When_RuInit_End();			
			CheckAlarm();			
			////Proc__Check_Download_Now();
		}		
		
		Chk_n_Make_NMS_Alm();
		//--------------------------------
		Proc_PSU_Reset(); // @ PRU, AAU		
		Proc__Check_zModem_Timeout(); // 2020.06.30 
		//--------------------------------
		#ifdef __USE_TSSI_LOG__
		Proc___Display_TSSI(); // 2019.10.15
		#endif
	}
}


/* EOF */



