/*
//##############################################################################
//				Maker Definition
//					&
//				Repeater-Type Definition 
//##############################################################################
*/

#define __MAKER_ANTS__ 		
#define __TYPE__AAU__  		// '__TYPE__PRU__'  or '__TYPE__AAU__'


/////////////#define __USE__RuInit_6_of_6_Cnt__ // 2020.07.08


////#undef __DEL__Unlock_Msg__
#define __Disable_Broadcast__
#define __USE_ProcessDly_plus_Offset__ 







/*
--------------------------------------------------------------------------------------
TEST 
--------------------------------------------------------------------------------------
*/

#ifdef __TYPE__PRU__ // -----------PRU
#define __USE_TSSI_LOG__ // 2020.01.14 : TSSI log display
//#undef _PAU_1st_TEST_Debug_
#define __USE_PSU_PAU_DN_SIZE_500__ // PAU/PSU AutoDown PktSize is 500
//#define __USE_MOD_DUALITY__ // module 2원화, 2020.04.10

#else // --------------------------AAU
#define __USE_TSSI_LOG__ // 2020.03.26 : TSSI log display

//#undef __AAU_Shutdown__Use_Test_Time__
#endif
//------------------------------------


//---------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU-------------------------------	
//////#define _SHORTEN_TIME____PAYLOAD_  // payload TEST
//////#define _SHORTEN_TIME____AUTO_RST_ // UDA RESET TIME : TEST Mode
//////#define _SHORTEN_TIME____BATT_MON_

//////////#define _WATCHDOG_TEST_TIME_5s_ // 2020.02.17 : PRU : S/W Watchdog Timer 10s -> 5s Test 시간 적용

//////////////////////////////////////////////////---------------------------
////#undef _CPRI_Init_OSM_Ctrl_
////#undef __Use_USER_RE_Channel_BW_CTRL__ // 2019.06.03

////#undef __SPECTRUM__OFFSET__ // 2019.05.17--> spectrum.c로 옮김. 
////#undef __CPRI_SET_When_Reboot__
#endif

#ifdef __TYPE__AAU__ // ANTS-AAU----------------------------------	
////#undef _DO_NOT_FOLLOW_VSS_Table_When_TDD_Slot_
////#undef _SHORTEN_TIME____AUTO_RST_
////#undef _SHORTEN_TIME____PAYLOAD_ // payload TEST
////#undef test_20190418
#endif
//---------------------------


//--------------------------------
// Test
//--------------------------------
//#undef __ID_MisMatch_ALARM_TEST__  // 10분 ------------> 2분 
//#undef _TEST____ALM_LOG_
//#undef __REMS_SUPPRESS_TEST__
//#undef __YCSO__DBUG__
//#undef _20190609_When_CPRI01_Alarm_CPRI23_TX_DISABLE_
//#undef __TEST_LS_MODE__
//#undef __LS_TEST_Board_Name__ 2019.05.28
//#undef __AC_Fail_TEST_PAU0_RST_PIN_Toggle__

//--------------------------------
// Shortening Test-Time
//--------------------------------
//#undef __PSU_RESET_Cancel_When_VSS_RESET__ // PRU
//#undef __20190429_TEST_REQUEST__ // PRU, v56.00.04

//#undef _SHORTEN_TIME____BATT_MON_

// Shortening Test-Time
//#undef _SHORTEN_TIME____FAN_LTE_SERVICE_
//#undef _SHORTEN_TIME____DPD_ALARM_
//#undef _SHORTEN_TIME____NOISE_SPUR_ // RVS Noise Alarm 
//#undef _SHORTEN_TIME____RVS_LOCAL_FWD_AMP_OFF_
//#undef _Show_Reg_Cmp_Debug_
//#undef __AAU_MU_DUMMY_APP__
//-------------------------------------------------------







/*
--------------------------------------------------------------------------------------
Defines which are used...
--------------------------------------------------------------------------------------
*/
#define __USE_VSS_UL_RE_RESET_ACK_SW__ // Vss_UL_RE_Reset_Ack 응답속도 개선.
#define __PRU_Temporary_SKIP__ // it will be used later @ PRU
#define WATCH_DOG_EN
#define __USE_MAC_CHECK__
#define __INVENTORY_UPDATE_CHECK_20190718__
#define __Use_ifconfig__
#define _AID_DELETE_NEEDED__ // 2019.05.17
#define __USE_AAU_RF_ATT_Accuracy__
#define __DEL_UDP_REMS__ // use this 2019.09.05.
#define __DO_Not_Save_LS_DELAY__
#define __CHECK_BTS_SIZE__
//--------------------------------------------------------------------------------------

// 2019.11.14. Alarm/Control-Log-Duplexing
#define __ALARM_LOG_BACKUP__ 


//--------------------------------------------------------------------------------------
#ifdef __TYPE__PRU__
//--------------------------------------------------------------------------------------
#define __USE_RTC__
#define __20191107__PRU_Tsync_Calc_Change__     // 2019.11.07

#define __USE_Packet_Division_for_WaveSpectrum__ // 2019.12. : gREMS WaveSpectrum Packet Division
#define __Auto_Config_When_CPRI_UnLock_to_Lock__ // 2019.10.28 : 

#define __2019_1226__Use_Alarm_Timer_Structure__ // 2020.01.08 added
#define __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__ // 2020.01.08 added
#define __USE__gIRO_DL_Shutdown_Field__          // -----------PRU gIRO

//////#undef __USE_DPD_9v0_Calculation__ // 51.10.05 Test Version, TEST Version for NEW_DPD_Version


///////#undef __USE_KERNEL_ROOTFS_DN__CHECK__         // 2020.04.14 for "Kernel, RootF/S download"
///////#undef __Seperate_AlarmLog__From__AutoReport__   // 2020.04.14 seperate log.write from alarm.auto.report
#define __Send_Init_Alarm__When_0x500D_RuInit_is_6__ // 2020.07.02 : Sending Init Alarm @ AID 0x500D is 6/6.


//--------------------------------------------------------------------------------------
#else // AAU
//--------------------------------------------------------------------------------------
#define __YCSO__BT__CMD__ // 2019.11.26 : bluetooth 수정: every 60[s], send_BT_mode_CMD() 실행.
#define __TEST_AAU__Use_Gain_Check_Level__
#define __USE_Loading_AAU_EQ_Pattern__
#define __USE_Packet_Division_for_WaveSpectrum__ // 2019.12. : gREMS WaveSpectrum Packet Division
#define __Auto_Config_When_CPRI_UnLock_to_Lock__ // 2019.10.28 : 

#define __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__
#define __2019_1226__Use_Alarm_Timer_Structure__ // AAU먼저 CPRI, Clock 등 timer 변경.

#define __2020_0107_Saving_AlarmLog_When_CPRI_Lock__


////#undef __AAU_WATCHDOG_TIME_TEST__
////#undef __USE_AAU_ALC_SPEED_CTRL__	// deleted 2019.09.17
////#undef __TEST_DEBUG__

////#define __USE_KERNEL_ROOTFS_DN__CHECK__ // 2020.04.14 for "Kernel, RootF/S download"

//--------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------





/*
//##############################################################################
*/












/////////////#undef __OLD_AID__ // old AID

// when basic test for MIMO Standby
////#warning __YCSO__ need parsing string


/*
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
#endif
*/


/*
------------------------------------------------------------
// debug 는 define or undefine
------------------------------------------------------------
#define _Debug____SIM_
#define _Debug____NOISE_SPUR_
#define _Debug____MULTI_DOWN_ST_  // multidown 시, module의 download 상태를 기존의 AID 통해서 보게 할지 말지.
------------------------------------------------------------
*/



/*
//---------------------------------------------------------------------------------------------				
#if 0 // later used when 5G-AAU/PRU
//------------------------------------------------------------------

//---------------------------------------------------------------------------------------------				
#endif // later used when 5G-AAU/PRU
//------------------------------------------------------------------
(void); (void);


#if 0 // later, when PRU/AAU

------------------------------------------------------------


Reg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_PA_OnOff]
m_Reg_Ctl.RFU[amp].PA_OnOff.Data = Read_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff]);
Write_FPGA_Reg(Reg_RFU_CTRL_Addr[amp][Reg_RFU_CTRL_RW_PA_OnOff], m_Reg_Ctl.RFU[amp].PA_OnOff.Data, "PA OnOff");



const INT8U unicast_aid_list[][2] = 

*/







/*
-------------------------------------------------------
// TEST
-------------------------------------------------------
#undef  __UDA_ALM_SUPPRESS_TEST__ // UDA Alarm Suppress

#undef _TEST____PAYLOAD_OFF_

#define USE_DLY_DEBUG
*/





//##############################################################################
				
