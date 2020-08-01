/////////////////////////////
//	AUTO_CONFIG_PROC.C
/////////////////////////////
#define AUTO_CONFIG_PROC_GLOBALS
#include "includes.h"
/////////////////////////////


// ANTS Default IP/GateWay ----------------------------------------------
static INT8U Default_Ip_Bp[4]  = {192, 168, 0, 3};
static INT8U Default_GW_Bp[4]  = {192, 168, 0, 1};
//-----------------------------------------------
static INT8U Default_SM_Bp[4]  = {255, 255, 0, 0};
static INT8U Default_Mac_Bp[6] = {0x50, 0x5B, 0xC2, 0xE1, 0xC0, 0x25};


//===================================================================================================
OS_TIMER Send_Ip_Assign_Noti_time;
OS_TIMER Send_Init_Noti_time;
OS_TIMER Send_CnM_SW_Noti_time;

//---------------------------------------------------------------------------------------------------




static INT8U net_valid_chk_cnt = 0;	// 2019.05.03, add
void Clear_net_valid_chk_cnt(void)
{
	net_valid_chk_cnt = 0;
}


INT8U Is_Valid_MAC(INT8U* p_Mac)
{
	if(p_Mac[0] == 0xFF) return 0;
	if(p_Mac[0] == 0x00) return 0;
	
	return 1;
}

char* Get_REC_Str(INT8U RCE_Type)
{
	switch( RCE_Type )
	{		
		case _TYPE_LS_SS_: 
			return "SAMSUNG";
			
		case _TYPE_LS_ELG_:
		case _TYPE_LS_NKA_:
			return "ELS/NKA";
		
		default: 		
			return "Unknown";
	}
}

INT8U Get_REC_Idx(INT8U RCE_Type)
{
	switch( RCE_Type )
	{		
		case _TYPE_LS_SS_ : return LSH_SS;
		case _TYPE_LS_NKA_: return LSH_NK;
		case _TYPE_LS_ELG_: return LSH_ER;
		
		default: return LSH_Max;
	}
}



/*

VSS-REC type Check 전에, 동작
	--------------------------------------------------------
	MAC 
	---------
	(1) reboot : copy fused-MAC(AAU), Purchased-MAC(PRU) to file-system
	(2) APP start
	(3) Copy file/fused-MAC to m_Eep.Fused_MAC[6]
	
	(4) Read Current MAC to m_St.init_MAC[6]
	(5) when file-not-exist or m_Eep.Fused_MAC is invalid, 
	(5-1) [AAU]		
			(a) when reboot : (before Uptime-1-min)
				Save m_St.init_MAC[6] to m_Eep.Fused_MAC[6] (if same, not eep-save)
			(b) when app restart : do nothing(m_Eep.Fused_MAC[6] is now ready, but if not, reboot)
			   		  
	(5-2) [PRU] 
			(a) copy PRU-Fixed-MAC[6] to m_Eep.Fused_MAC[6] (if same, not eep-save)

	(6) MAC-Setting with m_Eep.Fused_MAC[6]
	(7) display m_Eep.Fused_MAC[6] (fused-MAC, purchased-MAC, or default-MAC)
	--------------------------------------------------------
	IP : 
	Setting default IP 192.168.0.3(PRU) 
	Setting default IP 192.168.0.3(AAU)+11 = 192.168.0.14
	--------------------------------------------------------

VSS-REC type Check 이후 동작
(6) when Rx VSS-BaseIP/BaseMAC : IP/MAC Set
(7) ID allocation : IP/MAC Set
--------------------------------------------------------
*/
void Set_System_Net_Info(void) // for default IP 192.168.0.3
{	
	INT8U Base_Ip[4]; 
	INT8U GW0[4]; 	
	INT8U NetMask0[4];
	INT32U uptime32;
	INT8U use_default;
	
	uptime32 = GetUptime();
	print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Init Net Set] Get Uptime %d[s]", uptime32);
	print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Init Net Set] Before Reading VSS(ip, MAC) ----");
	
	//=============================================
	// 2019.06.27 : default IP : 192.168.0.3 Start
	memcpy( Base_Ip,	Default_Ip_Bp, 4);
	memcpy( GW0,		Default_GW_Bp, 4);
	memcpy( NetMask0,	Default_SM_Bp, 4);
	//=============================================
	#ifdef __TYPE__PRU__
	if(Get_System_HW_Mac(m_Eep.Fused_MAC) != 0) // fail
	{	
		memcpy(m_St.def__MAC, Default_Mac_Bp, 6);
		use_default = 1;		
	}
	else
	{	
		if(Is_Valid_MAC(m_Eep.Fused_MAC))
		{
			memcpy(m_St.def__MAC, m_Eep.Fused_MAC, 6);
			use_default = 0;
		}
		else
		{
			memcpy(m_St.def__MAC, Default_Mac_Bp, 6);	
			use_default = 1;
		}
	}
	#else
	if(uptime32 < 40)
	{
		if(memcmp(m_Eep.Fused_MAC, m_St.init_MAC, 6)) // if not same
		{
			memcpy(m_Eep.Fused_MAC, m_St.init_MAC, 6);
			print_agent(DEBUG_SYSTEM, VT_CYAN, "Copy Fuse MAC");
		}
	}
	if(Is_Valid_MAC(m_Eep.Fused_MAC))
	{
		memcpy(m_St.def__MAC, m_Eep.Fused_MAC, 6);
		use_default = 0;
	}
	else
	{
		memcpy(m_St.def__MAC, Default_Mac_Bp, 6);	
		use_default = 1;
	}
	#endif	

	if(use_default)
	{
		print_agent(DEBUG_SYSTEM, VT_CYAN, "Use Def---MAC  %02X:%02X:%02X:%02X:%02X:%02X", 
				m_St.def__MAC[0], m_St.def__MAC[1], m_St.def__MAC[2], 
				m_St.def__MAC[3], m_St.def__MAC[4], m_St.def__MAC[5]);
	}
	else
	{
		print_agent(DEBUG_SYSTEM, VT_CYAN, "Use Fused-MAC  %02X:%02X:%02X:%02X:%02X:%02X", 
				m_St.def__MAC[0], m_St.def__MAC[1], m_St.def__MAC[2], 
				m_St.def__MAC[3], m_St.def__MAC[4], m_St.def__MAC[5]);
	}
	//=============================================

	
	//=============================================
	print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork SET : ---------"); 
	Net_Stop();		
	Net_Set_Info_Set( _MAC0_,       m_St.def__MAC);		
	Net_Set_Info_Set( _IPADDR0_,	Base_Ip);	
	sync();	
	Net_Start(2);
	Net_Set_Info_Set( _NETMASK0_,	NetMask0);
	Net_Set_Info_Set( _DEFAULT_GW_,	GW0);	
	//-----------------------
	Net_Get_Info_Update();
	//=============================================
}

void Proc__PRU_Set_MAC_Addr(void)
{
#ifdef __TYPE__PRU__
	if(m_Env.Ctrl_MAC_Req)
	{
		m_Env.Ctrl_MAC_Req = 0;
	
		Set_System_HW_Mac(m_Env.Ctrl_MAC);
		
		OSTimeDly(2*OS_DELAY_1sec);
		sync();
			
		if(Get_System_HW_Mac(m_Eep.Fused_MAC)==0) 
		{		
			print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork SET : ---------"); 
			Net_Stop();		
			Net_Set_Info_Set( _MAC0_, m_Eep.Fused_MAC);
			sync();	
			Net_Start(2);
			Net_Get_Info_Update();
		}
		else // fail
		{		
			print_agent(DEBUG_SYSTEM, VT_RED, "!! MAC-Change Fail");		
		}
	}
#endif
}







void Check_Alarm_Change(void)
{	
	Send_FX_ALM_INFO_NOTI(AufoConfigFlag.Recv_Ip, Get_Tx_Frame_SeqNo(COMM_UDP_LS));
}

void Check_CnM_Path_Change_Clear(void)
{
	AufoConfigFlag.CnM_Path_Sel_Flag = 0;
	AufoConfigFlag.CnM_Path_Sel_Cnt = 0;
	AufoConfigFlag.CnM_Path_Sel_Send_Cnt=0;
	
	OSTimerReset( &Send_CnM_SW_Noti_time );
}

void Check_CnM_Path_Change(void)
{
	if(m_Env.gREMS_Noti_Ready==0) // 2019.08.07
		return;
	
	// Compare 10 times
	if(m_St.d_VSS_Z_208__CnM_Path_Select[0][0] != AufoConfigFlag.CnM_Path_Sel_Val)
	{
		if(AufoConfigFlag.CnM_Path_Sel_Cnt++ > 10)
		{
			AufoConfigFlag.CnM_Path_Sel_Flag = 1;
			AufoConfigFlag.CnM_Path_Sel_Val = m_St.d_VSS_Z_208__CnM_Path_Select[0][0];
		}
	}

	// Send
	if(AufoConfigFlag.CnM_Path_Sel_Flag)
	{
		// Every 2sec
		if( OSTimerGet_msec(&Send_CnM_SW_Noti_time) > (2*TICK_CO_SEC) )
		{
			Send_FX_CnM_SW_NOTI(AufoConfigFlag.Recv_Ip, Get_Tx_Frame_SeqNo(COMM_UDP_LS));			

			if(AufoConfigFlag.CnM_Path_Sel_Send_Cnt++ > 2)
			{
				Check_CnM_Path_Change_Clear();
			}
		}
	}
}


static INT8U same_cnt = 0; // 2019.05.03
static INT8U base_prev[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // 2019.05.03

void Init_Base_IP_Mac_Chk(void)
{
	print_agent( DEBUG_SYSTEM, VT_BOLD_RED, "Init_Base_IP_Mac_Chk");
	AufoConfigFlag.Base_Is_Stable = 0;
}

void Disp_IP_Mac(INT8U cnt, INT8U* ip_mac, char* str)
{
	if(cnt == 6)
		print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Disp_IP_Mac] %02X:%02X:%02X:%02X:%02X:%02X, %s",
				ip_mac[0], ip_mac[1], ip_mac[2], ip_mac[3], ip_mac[4], ip_mac[5], str);
	else
		print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Disp_IP_Mac] %d.%d.%d.%d, %s",
				ip_mac[0], ip_mac[1], ip_mac[2], ip_mac[3], str);
}

void Chk___Base_IP_Mac(INT8U* base, INT8U cnt)
{	
	INT8U mac_ip_fail;
	INT8U invalid, i;
	static int fail_cnt = 0;
	
	if((cnt != 6)&&(cnt != 4)) 
	{		
		print_agent(DEBUG_SYSTEM, VT_RED, "[Chk___Base_IP_Mac] Cannot Check");
		return;	
	}

/*	
	------------------------------------------------
	// 2019.05.22.
	MAC Address 규칙
	Base MAC 02:03:02:01:00:0C
	------------------------------------------------
*/
#ifdef __USE_MAC_CHECK__
	if(cnt == 6) // SS
	{
		     if(base[1] != 0x03)          mac_ip_fail = 1; // 2번째 MAC Address가 0x03인 경우 에만 IP 설정
		else if((base[2] & cbits[1])==0)  mac_ip_fail = 2; // 3번째 1번 비트가 1이 아닌 경우 제외
		else if((base[5] & 0x0C) != 0x0C) mac_ip_fail = 3; // MAC 6번째의 2,3번 비트가 11 이 아닌 경우 제외
		else if(base[0] == 0x00)          mac_ip_fail = 4; // MAC Address 1번째 0x00인 경우 제외
		else if(base[0] == 0xFF)          mac_ip_fail = 5; // MAC Address 1번째 0xFF인 경우 제외
		else                              mac_ip_fail = 0;

		for(invalid=0, i = 0; i < 6; i++ ) // mac
		{
			if(base[i] == 0xff) invalid++;
			if(invalid >= 3) 
			{
				mac_ip_fail = 6;
			}
		}
	}
	else // IP
	{
		if(base[0] == 0) mac_ip_fail = 1;
		else if(base[0] != 192) mac_ip_fail = 1;	
		else mac_ip_fail = 0;
	}
	
	if(mac_ip_fail)
	{
		net_valid_chk_cnt = 0;
		if((fail_cnt%30)==0)
		{
			fail_cnt = 0;
			Disp_IP_Mac(cnt, base, "fail");
		}
		fail_cnt++;
		same_cnt = 0; // 2019.09.10.
	}	
	//----------------------------------------
	else
	{
		fail_cnt = 0;
		net_valid_chk_cnt++;
		if(net_valid_chk_cnt > 10) 
		{
			net_valid_chk_cnt = 11;
			if(Reg_Ready_Flag != 1)
			{
				Reg_Ready_Flag = 1;
				print_agent( DEBUG_SYSTEM, VT_BOLD_GREEN, "Reg_Ready_Flag");
			}
		}
	}
#endif	
	
	if(mac_ip_fail == 0)
	{	
		if(memcmp(base, base_prev, cnt)==0) // if same
		{
			if(base[0] != 0)
			{
				same_cnt++;
				if(same_cnt >= 10) 
				{
					if(same_cnt == 10)
					{
						print_agent(DEBUG_LS_Comm, VT_BOLD_GREEN, "[RCU] Base MAC(IP) is Stable, Count over 10.");
					}					
					same_cnt = 100; // max 10
				}
				else
				{
					if(cnt == 6) // Samsung
					{
						print_agent(DEBUG_LS_Comm, VT_YELLOW, "[RCU] Base MAC %02X:%02X:%02X:%02X:%02X:%02X (Same Count %d)", 
							base[0],base[1],base[2],base[3],base[4],base[5], same_cnt);		
					}
					else if(cnt == 4) // Ericson, Nokia
					{
						print_agent(DEBUG_LS_Comm, VT_YELLOW, "[RCU] Base IP %d.%d.%d.%d (Same Count %d)", 
							base[0],base[1],base[2],base[3], same_cnt);
					}
					
				}
			}
			else
			{
				print_agent(DEBUG_SYSTEM, VT_RED, "[Chk___Base_IP_Mac] base[0] == 0");
			}
		}
		else 
		{		
			same_cnt = 0;
			print_agent( DEBUG_SYSTEM, VT_BOLD_GREEN, "[Chk___Base_IP_Mac] same_cnt = 0");
			Disp_IP_Mac(cnt, base_prev, "prev");
			Disp_IP_Mac(cnt, base, "curr");
		}

		memcpy(base_prev, base, cnt);	
	}
	//-------------------------------------------------------	
	if(same_cnt >= 10)
	{
		AufoConfigFlag.Base_Is_Stable = 1;
		if(cnt == 6)
		{
			memcpy(AufoConfigFlag.Base_Mac_Vss_Prev, AufoConfigFlag.Base_Mac_Vss, 6);			
			memcpy(AufoConfigFlag.Base_Mac_Vss, base, 6);
			
			if(memcmp(AufoConfigFlag.Base_Mac_Vss_Prev, AufoConfigFlag.Base_Mac_Vss,6)) // if changed
			{
				AufoConfigFlag.Base_Mac_Changed = 1;
				AufoConfigFlag.Base_Mac_Changed2 = 1;
				Disp_IP_Mac(cnt, base, "changed");
			}
		}
		else if(cnt == 4)
		{
			memcpy(AufoConfigFlag.Base_Ip_Prev, AufoConfigFlag.Base_Ip, 4);
			memcpy(AufoConfigFlag.Base_Ip, base, 4);
			
			if(memcmp(AufoConfigFlag.Base_Ip_Prev, AufoConfigFlag.Base_Ip, 4)) // if changed
			{
				AufoConfigFlag.Base_Ip_Changed = 1;
				AufoConfigFlag.Base_Ip_Changed2 = 1;
				Disp_IP_Mac(cnt, base, "changed");
			}
		}
	}
}

void Copy_Data_From_VSS(void)
{
	INT8U REC;
	INT8U base[6];
	INT8U Ref_CPRI_No;
	INT8U lock_cpri_no;
	INT8U i;
	INT8U instance_cpri_lock;

	//----------------------------------------------
	instance_cpri_lock = Get_Instant_CPRI_Lock_St();
	if(instance_cpri_lock == 0) // if un-lock
	{
		Reg_Ready_Flag = 0;
		net_valid_chk_cnt = 0;
		
		return; // 2019.06.21, Checked
	}
	//----------------------------------------------
	if(instance_cpri_lock & cbits[CPRI_0]) 
		 lock_cpri_no = CPRI_0; // 100M has high-priority to Alpha
	else lock_cpri_no = CPRI_1;
	Ref_CPRI_No = m_St.Ref_CPRI_No;

	//----------------------------------------------
	/*
	if((m_St.Ref_CPRI_No == CPRI_1)&&(lock_cpri_no == CPRI_0))
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "Ref_CPRI_No : CPRI_1 && lock_cpri_no : CPRI_0");
		print_agent(DEBUG_SYSTEM, VT_BOLD_MAGENTA, "CPRI0 : %02X:%02X:%02X:%02X:%02X:%02X",
				m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[0].uD8[0],
				m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[1].uD8[0],
				m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[2].uD8[0],
				m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[3].uD8[0],
				m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[4].uD8[0],
				m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[5].uD8[0]);
	}
	*/
	//----------------------------------------------			
	lock_cpri_no = m_St.Ref_CPRI_No; // 2019.09.10
	//----------------------------------------------
	
	REC = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].RECTypeID.uD8[0];
	
	if(IS_Valid_VSS_REC_Type(REC))
	{				
		if(AufoConfigFlag.RECTypeID != REC)
		{
			print_agent(DEBUG_LS_Comm, VT_BOLD_YELLOW, "RECTypeID  Changed 0x%X -> 0x%X", 
				AufoConfigFlag.RECTypeID, REC);
		}
		AufoConfigFlag.RECTypeID = REC;
		AufoConfigFlag.RE_Cascade_ID = m_Reg_Sts.DL_CPRI[m_St.Ref_CPRI_No].RE_Cascade_ID.uD8[0]; // stable value
		AufoConfigFlag.REC_Port_No   = m_Reg_Sts.DL_CPRI[m_St.Ref_CPRI_No].REC_Port_No.uD8[0];	 // stable value	
		m_St.LS_Maker = REC;	
		
		switch(REC)
		{
			case _TYPE_LS_SS_: // 2019.05.03		 	
				base[0] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[0].uD8[0];
				base[1] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[1].uD8[0];
				base[2] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[2].uD8[0];
				base[3] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[3].uD8[0];
				base[4] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[4].uD8[0];
				base[5] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_Mac_Address[5].uD8[0];

				Chk___Base_IP_Mac(base, 6);			
				
				AufoConfigFlag.NetMask0[0] = 255;
				AufoConfigFlag.NetMask0[1] = 0;
				AufoConfigFlag.NetMask0[2] = 0;
				AufoConfigFlag.NetMask0[3] = 0;
				break;
				
			case _TYPE_LS_ELG_: // Ericson
			case _TYPE_LS_NKA_: // Nokia				
				// Load origin 
				for(i=0;i<6;i++)
					AufoConfigFlag.Base_Mac[i] = m_St.def__MAC[i]; // 2019.06.02

				// 2019.05.03		 	
				base[0] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_IP_Address[0].uD8[0];
				base[1] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_IP_Address[1].uD8[0];
				base[2] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_IP_Address[2].uD8[0];
				base[3] = m_Reg_Sts.DL_CPRI[lock_cpri_no].Base_IP_Address[3].uD8[0];

				Chk___Base_IP_Mac(base, 4);
				
				AufoConfigFlag.NetMask0[0] = 255;
				AufoConfigFlag.NetMask0[1] = 255;
				AufoConfigFlag.NetMask0[2] = 0;
				AufoConfigFlag.NetMask0[3] = 0;
				break;

			default:			
				break;
		}
	}
	else
	{
		net_valid_chk_cnt = 0;
	}	

	INT8U ip[4];
	
	// noti ip
	if(m_Env.test_ls_ip[0] == 0) // if not controlled by user
	{
		/*
		// 2019.11.18 : delete
		//AufoConfigFlag.Recv_Ip[0] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[0].uD8[0];
		//AufoConfigFlag.Recv_Ip[1] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[1].uD8[0];
		//AufoConfigFlag.Recv_Ip[2] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[2].uD8[0];
		//AufoConfigFlag.Recv_Ip[3] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[3].uD8[0];
		*/
		
		// 2019.11.18
		ip[0] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[0].uD8[0];
		ip[1] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[1].uD8[0];
		ip[2] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[2].uD8[0];
		ip[3] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP1_Address[3].uD8[0];

		Update__Layer_IP(ip);
	}

	/*
	// 2019.11.11 : delete
	//AufoConfigFlag.Recv_Ip2[0] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[0].uD8[0];
	//AufoConfigFlag.Recv_Ip2[1] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[1].uD8[0];
	//AufoConfigFlag.Recv_Ip2[2] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[2].uD8[0];
	//AufoConfigFlag.Recv_Ip2[3] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[3].uD8[0];
	*/
	
	// 2019.11.11 : save gREMS IP when valid
	ip[0] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[0].uD8[0];
	ip[1] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[1].uD8[0];
	ip[2] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[2].uD8[0];
	ip[3] = m_Reg_Sts.DL_CPRI[Ref_CPRI_No].Noti_IP2_Address[3].uD8[0];

	Update__gREMS_IP(ip);	
}


// 2019.11.11

static OS_TIMER Timer_Disp_gREMS_IP;
void Update__gREMS_IP(INT8U* ip)
{	
	static INT8U prev_ip[4] = {0,0,0,0};
	static INT8U same_cnt = 0;
	INT8U max_same_cnt = 10;
	
	if(Is__Valid_IP(0, ip)) // valid
	{
		if(memcmp(prev_ip, ip, 4)==0) same_cnt++;
		else same_cnt = 0;

		memcpy(prev_ip, ip, 4);

		if(same_cnt >= max_same_cnt)
		{	
			same_cnt = max_same_cnt;
			
			memcpy(AufoConfigFlag.Recv_Ip2, ip, 4);
			
			if(memcmp(m_Eep.gREMS_Ip, ip, 4)) // if different and changed
			{
				print_agent(DEBUG_REMS, VT_BOLD_YELLOW, "[gREMS_IP] %d.%d.%d.%d --> %d.%d.%d.%d",		
					m_Eep.gREMS_Ip[0], 
					m_Eep.gREMS_Ip[1], 
					m_Eep.gREMS_Ip[2], 
					m_Eep.gREMS_Ip[3],
					ip[0], 
					ip[1], 
					ip[2], 
					ip[3]);	

				memcpy(m_Eep.gREMS_Ip, ip, 4);
				m_Env.is_eep_write = EEP_WRITE_CODE;
			}
		}
		OSTimerReset(&Timer_Disp_gREMS_IP);
	}
	else
	{
		Set__Defult_Noti_IP(AufoConfigFlag.Recv_Ip2); // 2019.11.30
		Set__Defult_Noti_IP(prev_ip);
		same_cnt = 0;
		
		if(OSTimerGet_msec(&Timer_Disp_gREMS_IP) > (15*TICK_CO_SEC))
		{
			OSTimerReset(&Timer_Disp_gREMS_IP);
			
			print_agent(DEBUG_REMS, VT_BOLD_RED, "[gREMS_IP] Mac fail error,  %d.%d.%d.%d",
				ip[0], ip[1], ip[2], ip[3]);
			print_agent(DEBUG_REMS, VT_BOLD_RED, "[gREMS_IP] Recv_Ip2,  %d.%d.%d.%d",
				AufoConfigFlag.Recv_Ip2[0], AufoConfigFlag.Recv_Ip2[1], AufoConfigFlag.Recv_Ip2[2], AufoConfigFlag.Recv_Ip2[3]);	
		}
	}
}


static OS_TIMER Timer_Disp_Layer_IP;
void Update__Layer_IP(INT8U* ip)
{	
	static INT8U prev_ip[4] = {0,0,0,0};
	static INT8U same_cnt = 0;
	INT8U max_same_cnt = 10;
	
	if(Is__Valid_IP(0, ip)) // valid
	{
		if(memcmp(prev_ip, ip, 4)==0) same_cnt++;
		else same_cnt = 0;

		memcpy(prev_ip, ip, 4);

		if(same_cnt >= max_same_cnt)
		{	
			same_cnt = max_same_cnt;
			memcpy(AufoConfigFlag.Recv_Ip, ip, 4);
		
			if(memcmp(m_Eep.Layer_Ip, ip, 4)) // if different and changed
			{
				print_agent(DEBUG_REMS, VT_BOLD_YELLOW, "[Layer_IP] %d.%d.%d.%d --> %d.%d.%d.%d",		
					m_Eep.Layer_Ip[0], 
					m_Eep.Layer_Ip[1], 
					m_Eep.Layer_Ip[2], 
					m_Eep.Layer_Ip[3],
					ip[0], 
					ip[1], 
					ip[2], 
					ip[3]); 

				memcpy(m_Eep.Layer_Ip, ip, 4);
				m_Env.is_eep_write = EEP_WRITE_CODE;
			}
		}
		OSTimerReset(&Timer_Disp_Layer_IP);
	}
	else
	{
		Set__Defult_Noti_IP(AufoConfigFlag.Recv_Ip); // 2019.11.30
		Set__Defult_Noti_IP(prev_ip);
		same_cnt = 0;
		
		if(OSTimerGet_msec(&Timer_Disp_Layer_IP) > (15*TICK_CO_SEC))
		{
			OSTimerReset(&Timer_Disp_Layer_IP);
			
			print_agent(DEBUG_REMS, VT_BOLD_RED, "[Layer_IP] Mac fail error,  %d.%d.%d.%d",
				ip[0], ip[1], ip[2], ip[3]);
		}
	}
}


void Set__Defult_Noti_IP(INT8U* noti_ip)
{
	/*
	2019.11.19 : REC 제조사 무관하게 default : NotiIP 1/2 모두 192.224.0.1
	*/
	INT8U def_ip[4] = {192,224, 0, 1}; // default IP
	
	memcpy(noti_ip, def_ip, 4);	
}

void Load__Noti_IP(void)
{	
	OSTimerReset(&Timer_Disp_gREMS_IP);
	OSTimerReset(&Timer_Disp_Layer_IP);	
	
	if(Is__Valid_IP(0, m_Eep.gREMS_Ip)==0) Set__Defult_Noti_IP(m_Eep.gREMS_Ip);
	if(Is__Valid_IP(0, m_Eep.Layer_Ip)==0) Set__Defult_Noti_IP(m_Eep.Layer_Ip);
	
	memcpy(AufoConfigFlag.Recv_Ip2, m_Eep.gREMS_Ip, 4);	
	memcpy(AufoConfigFlag.Recv_Ip , m_Eep.Layer_Ip, 4);	
}

	
	
		
	
	




void AutoConfig_Flag_Clear(void)
{
	//imsi
	AufoConfigFlag.AutoConfig_Step = AUTUCONFIG_STEP04;

	AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_READY;
	AufoConfigFlag.Ru_Ip_Noti_Cnt = 0;

	AufoConfigFlag.Init_Noti_Step = INIT_NOTI_READY;
	AufoConfigFlag.Init_Noti_Cnt = 0;

	AufoConfigFlag.Init_Status = 0;
	AufoConfigFlag.Vaild_Indicator = 0;
	
	AufoConfigFlag.RU_Id = 0;
	AufoConfigFlag.RU_Id_Alloc = 0; // 0x01 if RU_Id is Allocated. 2019.05.08

	//test
	AufoConfigFlag.AutoConfig_Default_Flag = 0;
	Reg_Ready_Flag = 0;	
	
	// 2019.05.20
	AufoConfigFlag.LS_ID = 0;

	//Backup Road - 4/2
	memcpy(AufoConfigFlag.LS_Regist_Num, m_St.LS_Regist_Num, SN_SIZE);	
	memcpy(AufoConfigFlag.RU_Board_Name, m_Eep.RU_Board_Name, BD_NANEMSIZE);


	Check_CnM_Path_Change_Clear();

	OSTimerReset( &Send_Ip_Assign_Noti_time );
	OSTimerReset( &Send_Init_Noti_time );

	Set___RU_Initial_Status(Init_St_System_Init);
}

void Init_Noti_Process(void)
{
	if(AufoConfigFlag.AutoConfig_Step != AUTUCONFIG_STEP05) return;
	
	switch( AufoConfigFlag.Init_Noti_Step )
	{		
		case INIT_NOTI_READY: 
			AufoConfigFlag.Init_Noti_Step = INIT_NOTI_START;
			break;

		case INIT_NOTI_START: 			// RU_IP_NOTI_REQ,				
			// Send Broadcast every 5sec
			Send_FX_INIT_NOTI(AufoConfigFlag.Recv_Ip, Get_Tx_Frame_SeqNo(COMM_UDP_LS));
			Set___RU_Initial_Status(Init_St_Init_Notify);
			Timer_start_LS_Valid_Indicator(); // 2019.06.26 
			AufoConfigFlag.Init_Noti_Step = INIT_NOTI_CHECK;
			
			print_agent( DEBUG_LS_Comm, VT_Color_TX, "[INIT_NOTI_START] Send_FX_INIT_NOTI to [%d.%d.%d.%d]", 
					AufoConfigFlag.Recv_Ip[0], AufoConfigFlag.Recv_Ip[1], 
					AufoConfigFlag.Recv_Ip[2], AufoConfigFlag.Recv_Ip[3] );
		
			break;

		case INIT_NOTI_CHECK: // RU_IP_NOTI_RSP_Check
			if( OSTimerGet_msec(&Send_Init_Noti_time) > (2*TICK_CO_SEC) ) // 2019.04.11 : 5 -> 2
			{			
				AufoConfigFlag.Init_Noti_Step = IP_ASSIGN_START;
				OSTimerReset( &Send_Init_Noti_time );
			}			
			break;
		
		case INIT_NOTI_PROCESSING: 		// IP Change
			print_agent( DEBUG_LS_Comm, VT_GREEN, "[Init_Noti_Process] INIT NOTI PROCESSING");
			AufoConfigFlag.Init_Noti_Step = INIT_NOTI_FINISH;					
			break;
		
		case INIT_NOTI_FINISH: 
			print_agent( DEBUG_LS_Comm, VT_GREEN, "[Init_Noti_Process] Auto CONFIG FINISH");
			AufoConfigFlag.Init_Noti_Step = INIT_NOTI_END;			
			
			if(m_St.RU_Init_Status < Init_St_CPRI_St_Req)
			{
				// 2019.08.28 : 'Init_St___Completed' 상태에서는 제어 되지 않도록.
				Set___RU_Initial_Status(Init_St_CPRI_St_Req);
			}
			break;

		case INIT_NOTI_END:
			break;
		//-------------------------------
		default: break;
		//-------------------------------
	}	
}

INT8U Calc__LS_ID(INT8U* ip)
{
	INT8U uVal8;
	INT8U slot_id;
	INT8U port_id;
	INT16S ls_id;
	static INT16S ls_id_prev = 0;

	/*
	------------------------------
		bit15 ~ 14 : slot_id
		bit13 ~  9 : port_id
		bit8       : x 
	------------------------------	
	*/	

	INT8U REC = AufoConfigFlag.RECTypeID;
	if(REC == _TYPE_LS_SS_)	
	{
		uVal8 = ip[2] >> 1;
		port_id = uVal8 & 0x1F;
		slot_id = (uVal8 >> 5) & 0x3;	

		/*
		----------------------------------------
		2020.03.24 
		LS_ID Changed when 'Samsung' (Samsung DUH L/S ID 계산식 변경)
			ls_id = (port_id/8) + (slot_id*2-2); // 2019.08.19
				---> 
			ls_id = port_id + (slot_id*12-12);

			Range : 0~35 ( 범위 벗어나면, LS ID 99로 표시)

			LS ID 99 : "Invalid LS ID = [%d], slot ID =[%d], Port ID =[%d]"  Log 1회 출력
		----------------------------------------	
		*/
		ls_id = port_id + (slot_id*12-12);
		
		if((ls_id < 0)||(ls_id > 35)) 
		{
			ls_id = 99;
			if(ls_id_prev != ls_id)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "Invalid LS ID = [%d], slot ID =[%d], Port ID =[%d]", 
								ls_id, slot_id, port_id); 
			}
		}
		ls_id_prev = ls_id;
	}
	else
	{
		uVal8 = ip[2];
		port_id = uVal8 & 0x1F;	
		ls_id = port_id;
	}
	return ls_id;
}


// nokia ericson
INT8U Remapping_Self_Ip(void)
{
	INT8U rst = 1; // init with 1, 2019.05.03	
	type32 temp_var_ip, temp_gw;
	
	//Base_IP[0], Base_IP[1], Base_IP[2], Base_IP[3]
	//ip mapping -> Base_IP[2] & RU_Id[1] 7:4	

	temp_var_ip.uD8[0] = AufoConfigFlag.Base_Ip[0];
	temp_var_ip.uD8[1] = AufoConfigFlag.Base_Ip[1];
	temp_var_ip.uD8[2] = AufoConfigFlag.Base_Ip[2];
	temp_var_ip.uD8[3] = AufoConfigFlag.Base_Ip[3] + 0x0B;

	if( temp_var_ip.uD8[0] == 0 )
		return 0;
	
	print_agent( DEBUG_LS_Comm, VT_Color_Evt, "IP Re-Map Base : RU-ID(%d) -> ip%d.%d.%d.%d", 
			AufoConfigFlag.RU_Id, 
			AufoConfigFlag.Base_Ip[0], AufoConfigFlag.Base_Ip[1], 
			AufoConfigFlag.Base_Ip[2], AufoConfigFlag.Base_Ip[3]+0x0B);

	temp_gw.uD8[0] = AufoConfigFlag.Base_Ip[0] & 0xFF;
	temp_gw.uD8[1] = AufoConfigFlag.Base_Ip[1] & 0xFF; // 2019.09.19 : 0x00 -> 0xFF
	temp_gw.uD8[2] = AufoConfigFlag.Base_Ip[2] & 0x00;
	temp_gw.uD8[3] = (AufoConfigFlag.Base_Ip[3] & 0x00) | 0x01;

	AufoConfigFlag.LS_ID = Calc__LS_ID(AufoConfigFlag.Base_Ip); // 2019.07.29
	m_St.LS_ID[_100M] = AufoConfigFlag.LS_ID;
	if(IS_Alpha_Mhz_Existing()) // 2019.08.09.
		m_St.LS_ID[Alpha] = AufoConfigFlag.LS_ID;
	else
		m_St.LS_ID[Alpha] = 0;

	//=============================================
	print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork SET : ---------"); 
	Net_Stop();	
	Net_Set_Info_Set( _MAC0_,    AufoConfigFlag.Base_Mac );	
	Net_Set_Info_Set( _IPADDR0_, &temp_var_ip.uD8[0] ); // Ericson, Nokia		
	sync();	
	Net_Start(4);		
	Net_Set_Info_Set( _NETMASK0_, AufoConfigFlag.NetMask0 );
	Net_Set_Info_Set( _DEFAULT_GW_, &temp_gw.uD8[0] ); // Ericson/Nokia
	//-----------------------
	Net_Get_Info_Update();
	//=============================================	
	return rst;
}

INT8U Remapping_Self_Ip_Use_RuId(void) // Ericson, Nokia
{
	INT8U rst = 0;
	INT8U temp_var = 0;

	type32 temp_var_ip, temp_gw;

	// Base_IP[0], Base_IP[1], Base_IP[2], Base_IP[3]
	// ip mapping -> Base_IP[2] & RU_Id[1] 7:4	

	temp_var_ip.uD8[0] = AufoConfigFlag.Base_Ip[0];
	temp_var_ip.uD8[1] = AufoConfigFlag.Base_Ip[1];
	temp_var_ip.uD8[2] = AufoConfigFlag.Base_Ip[2];
	temp_var_ip.uD8[3] = AufoConfigFlag.Base_Ip[3];

	temp_var = (AufoConfigFlag.RU_Id << 2);
	temp_var_ip.uD8[3] = (temp_var_ip.uD8[3] & 0xF3) | temp_var;
	
	print_agent( DEBUG_LS_Comm, VT_Color_Evt, "IP Re-Map RU_ID %d -> ip %d.%d.%d.%d\n", 
		AufoConfigFlag.RU_Id, temp_var_ip.uD8[0], temp_var_ip.uD8[1], temp_var_ip.uD8[2], temp_var_ip.uD8[3]);

	temp_gw.uD8[0] = AufoConfigFlag.Base_Ip[0] & 0xFF;
	temp_gw.uD8[1] = AufoConfigFlag.Base_Ip[1] & 0xFF; // 2019.09.19 : 0x00 -> 0xFF
	temp_gw.uD8[2] = AufoConfigFlag.Base_Ip[2] & 0x00;
	temp_gw.uD8[3] = (AufoConfigFlag.Base_Ip[3] & 0x00) | 0x01;

	//=============================================
	print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork SET : ---------"); 
	Net_Stop();	
	Net_Set_Info_Set( _MAC0_,    &AufoConfigFlag.Base_Mac[0] );
	Net_Set_Info_Set( _IPADDR0_, &temp_var_ip.uD8[0] ); // Ericson, Nokia	
	sync();	
	Net_Start(5);
	Net_Set_Info_Set( _NETMASK0_, &AufoConfigFlag.NetMask0[0] );
	Net_Set_Info_Set( _DEFAULT_GW_, &temp_gw.uD8[0] ); // Ericson/Nokia
	//-----------------------
	Net_Get_Info_Update();
	//=============================================
	
	return rst;
}





// samsung
INT8U Remapping_Self_Ip_SS(void)
{
#ifdef __TYPE__PRU__
	type64_Q temp_var_mac, temp_var_mac_After;
#else
	type64_T temp_var_mac, temp_var_mac_After;
#endif
	type32 temp_var_ip, temp_gw;
	INT8U i, rst = 1;

	//base mac copy
	temp_var_mac.uD64 = 0x0000000000000000;
	for(i = 0; i<6; i++) temp_var_mac.uD8[5-i] =  AufoConfigFlag.Base_Mac_Vss[i];

	//base mac >> 2 bit
	temp_var_mac_After.uD64 = temp_var_mac.uD64 >> 2;

	print_agent(DEBUG_SYSTEM, VT_GREEN, "[Remapping_Self_Ip_SS] BaseMac [%02X:%02X:%02X:%02X:%02X:%02X] >> 2 = [%02X:%02X:%02X:%02X:%02X:%02X]", 
		AufoConfigFlag.Base_Mac_Vss[0], AufoConfigFlag.Base_Mac_Vss[1], AufoConfigFlag.Base_Mac_Vss[2], 
		AufoConfigFlag.Base_Mac_Vss[3], AufoConfigFlag.Base_Mac_Vss[4], AufoConfigFlag.Base_Mac_Vss[5], 
		temp_var_mac_After.uD8[5], temp_var_mac_After.uD8[4], temp_var_mac_After.uD8[3], 
		temp_var_mac_After.uD8[2], temp_var_mac_After.uD8[1], temp_var_mac_After.uD8[0]);	

	AufoConfigFlag.Base_Ip[0] = temp_var_ip.uD8[0] = temp_var_mac_After.uD8[3]; // lschk
	AufoConfigFlag.Base_Ip[1] = temp_var_ip.uD8[1] = temp_var_mac_After.uD8[2];
	AufoConfigFlag.Base_Ip[2] = temp_var_ip.uD8[2] = temp_var_mac_After.uD8[1];
	AufoConfigFlag.Base_Ip[3] = temp_var_ip.uD8[3] = temp_var_mac_After.uD8[0];

	
	for(i=0;i<6;i++)
		AufoConfigFlag.Base_Mac[i] = AufoConfigFlag.Base_Mac_Vss[i];

	AufoConfigFlag.LS_ID = Calc__LS_ID(AufoConfigFlag.Base_Ip); // 2019.07.29
	m_St.LS_ID[_100M] = AufoConfigFlag.LS_ID;
	if(IS_Alpha_Mhz_Existing()) // 2019.08.09.
		m_St.LS_ID[Alpha] = AufoConfigFlag.LS_ID;
	else
		m_St.LS_ID[Alpha] = 0;

	if(temp_var_ip.uD8[0] == 0)
		return 0;

	temp_gw.uD8[0] = temp_var_ip.uD8[0] & 0xFF;
	temp_gw.uD8[1] = temp_var_ip.uD8[1] & 0x00; 
	temp_gw.uD8[2] = temp_var_ip.uD8[2] & 0x00;
	temp_gw.uD8[3] = (temp_var_ip.uD8[3] & 0x00) | 0x01;

	print_agent(DEBUG_SYSTEM, VT_GREEN, "[Remapping_Self_Ip_SS] Finish --- Calc IP - [%d.%d.%d.%d]", 
			temp_var_ip.uD8[0], temp_var_ip.uD8[1], temp_var_ip.uD8[2], temp_var_ip.uD8[3]);				

	//=============================================		
	print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork SET : ---------"); 
	Net_Stop();
	Net_Set_Info_Set( _MAC0_,    &AufoConfigFlag.Base_Mac[0] );
	Net_Set_Info_Set( _IPADDR0_, &temp_var_ip.uD8[0] ); // samsung		
	sync();
	Net_Start(6);
	Net_Set_Info_Set( _NETMASK0_, &AufoConfigFlag.NetMask0[0] );
	Net_Set_Info_Set( _DEFAULT_GW_, &temp_gw.uD8[0] ); // Samsung
	//-----------------------	
	Net_Get_Info_Update();
	//=============================================	

	return rst;
}



INT8U Remapping_Self_Ip_SS_Use_RuId(void) // Samsung
{
#ifdef __TYPE__PRU__
	type64_Q temp_var_mac, temp_var_mac_After;
#else
	type64_T temp_var_mac, temp_var_mac_After;
#endif
	type32 temp_var_ip, temp_gw;
	INT8U temp_var_s = 0;
	INT8U i, rst = 1; // init with '1' 2019.05.03


	// base mac copy
	temp_var_mac.uD64 = 0x0000000000000000;
	for(i = 0; i<6; i++) temp_var_mac.uD8[5-i] = AufoConfigFlag.Base_Mac_Vss[i];

	// base mac >> 2 bit
	temp_var_mac_After.uD64 = temp_var_mac.uD64 >> 2; 

	print_agent(DEBUG_SYSTEM, VT_WHITE, "[Remapping_Self_Ip_SS_Use_RuId] BaseMac [%02X:%02X:%02X:%02X:%02X:%02X] >> 2 [%02X:%02X:%02X:%02X:%02X:%02X]", 
			AufoConfigFlag.Base_Mac_Vss[0], AufoConfigFlag.Base_Mac_Vss[1], AufoConfigFlag.Base_Mac_Vss[2], 
			AufoConfigFlag.Base_Mac_Vss[3], AufoConfigFlag.Base_Mac_Vss[4], AufoConfigFlag.Base_Mac_Vss[5], 
			temp_var_mac_After.uD8[5], temp_var_mac_After.uD8[4], temp_var_mac_After.uD8[3], 
			temp_var_mac_After.uD8[2], temp_var_mac_After.uD8[1], temp_var_mac_After.uD8[0]	);

	AufoConfigFlag.Base_Ip[0] = temp_var_ip.uD8[0] = temp_var_mac_After.uD8[3];
	AufoConfigFlag.Base_Ip[1] = temp_var_ip.uD8[1] = temp_var_mac_After.uD8[2];
	AufoConfigFlag.Base_Ip[2] = temp_var_ip.uD8[2] = temp_var_mac_After.uD8[1];
	AufoConfigFlag.Base_Ip[3] = temp_var_ip.uD8[3] = temp_var_mac_After.uD8[0];

	temp_gw.uD8[0] = temp_var_ip.uD8[0] & 0xFF;
	temp_gw.uD8[1] = temp_var_ip.uD8[1] & 0x00; 
	temp_gw.uD8[2] = temp_var_ip.uD8[2] & 0x00;
	temp_gw.uD8[3] = (temp_var_ip.uD8[3] & 0x00) | 0x01;

	print_agent(DEBUG_SYSTEM, VT_WHITE, "[Remapping_Self_Ip_SS_Use_RuId] RU ID[%d] Change Array [%02X:%02X:%02X:%02X]" , 
			AufoConfigFlag.RU_Id,
			temp_var_ip.uD8[0], temp_var_ip.uD8[1], temp_var_ip.uD8[2], temp_var_ip.uD8[3]);		


	//////////////////////////////////////////////////////////////////////
	// Make IP/Mac with RU_Id from L/S
	//////////////////////////////////////////////////////////////////////
	
	temp_var_s = (AufoConfigFlag.RU_Id << 3);
	temp_var_ip.uD8[3] = (temp_var_ip.uD8[3] & 0xE7) | temp_var_s;

	for(i=0;i<6;i++)
		AufoConfigFlag.Base_Mac_Cal[i] = AufoConfigFlag.Base_Mac_Vss[i];

	temp_var_s = 0;
	temp_var_s |= (AufoConfigFlag.RU_Id << 5);
	AufoConfigFlag.Base_Mac_Cal[5] = (AufoConfigFlag.Base_Mac_Cal[5] & 0x9F) | temp_var_s;

	print_agent(DEBUG_SYSTEM, VT_GREEN, "[Remapping_Self_Ip_SS_Use_RuId] Finish --- RU ID[%d] Calculated IP[MAC] : %d.%d.%d.%d [%02X:%02X:%02X:%02X]", 
			AufoConfigFlag.RU_Id,
			temp_var_ip.uD8[0], temp_var_ip.uD8[1], temp_var_ip.uD8[2], temp_var_ip.uD8[3],
			AufoConfigFlag.Base_Mac_Cal[0], AufoConfigFlag.Base_Mac_Cal[1], AufoConfigFlag.Base_Mac_Cal[2], 
			AufoConfigFlag.Base_Mac_Cal[3], AufoConfigFlag.Base_Mac_Cal[4], AufoConfigFlag.Base_Mac_Cal[5] );

	//=============================================
	print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork SET : ---------"); 
	Net_Stop();
	Net_Set_Info_Set( _MAC0_,    AufoConfigFlag.Base_Mac_Cal );
	Net_Set_Info_Set( _IPADDR0_, temp_var_ip.uD8 );	// samsung with RU_ID
	sync();
	Net_Start(7);		
	Net_Set_Info_Set( _NETMASK0_ ,  AufoConfigFlag.NetMask0 );
	Net_Set_Info_Set( _DEFAULT_GW_, temp_gw.uD8 ); // Samsung
	//-----------------------
	Net_Get_Info_Update();
	//=============================================
	
	return rst;
}



void Ip_Assign_Process(void)
{
	INT8U ret;
	INT8U REC;
	static int ruid_acqu_noti_cnt = 0;
			
	if(AufoConfigFlag.AutoConfig_Step != AUTUCONFIG_STEP04) return;

	REC = AufoConfigFlag.RECTypeID;
		
	switch( AufoConfigFlag.Ip_Assign_Step )
	{
		//-------------------------------		
		case IP_ASSIGN_READY: 
			if(AufoConfigFlag.Base_Is_Stable) // 2019.05.03
			{
				// 2019.08.24 : Base_MAC/IP 설정 중복 막기위함.
				if(AufoConfigFlag.Base_Mac_Changed || AufoConfigFlag.Base_Ip_Changed)
				{
					AufoConfigFlag.Base_Mac_Changed = 0;
					AufoConfigFlag.Base_Ip_Changed = 0;
				}
				print_agent(DEBUG_LS_Comm, VT_GREEN, "[Ip_Assign_Process.Ready] Base MAC/IP Stable. Goto Noti-Step.");
				
				     if(REC == _TYPE_LS_SS_)  ret = Remapping_Self_Ip_SS();
				else if(REC == _TYPE_LS_ELG_) ret = Remapping_Self_Ip();
				else if(REC == _TYPE_LS_NKA_) ret = Remapping_Self_Ip();	
				else ret = 0;
					
				if(ret == 0)
					AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_READY;
				else
					AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_START;
			}
			break;

		case IP_ASSIGN_START:
			if(AufoConfigFlag.Base_Mac_Changed || AufoConfigFlag.Base_Ip_Changed)
			{
				// 2019.07.17 : Noti Step에서 IP/MAC 변경되면 'IP_ASSIGN_READY' step으로.
				print_agent(DEBUG_LS_Comm, VT_YELLOW, "[Ip_Assign_Process.Noti] Base_Mac/IP Changed, Goto Ready-Step");
				
				AufoConfigFlag.Base_Mac_Changed = 0;
				AufoConfigFlag.Base_Ip_Changed = 0;				
				AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_READY;								
			}
			else
			{	
				ruid_acqu_noti_cnt++;
				if((ruid_acqu_noti_cnt%10)==0)
					print_agent(DEBUG_LS_Comm, VT_YELLOW, "[Ip_Assign_Process.Noti] Send_FX_RUID_ACQU_NOTI(%d)", ruid_acqu_noti_cnt);
					
				Send_FX_RUID_ACQU_NOTI(AufoConfigFlag.Recv_Ip, Get_Tx_Frame_SeqNo(COMM_UDP_LS));
				Set___RU_Initial_Status(Init_St_RE_ID_Acqui);
				
				AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_CHECK;				
			}
			break;

		case IP_ASSIGN_CHECK: 			// RU_IP_NOTI_RSP_Check,	
			if( OSTimerGet_msec(&Send_Ip_Assign_Noti_time) > (2*TICK_CO_SEC) )
			{
				AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_START;
				OSTimerReset( &Send_Ip_Assign_Noti_time );
			}
			break;
		
		case IP_ASSIGN_PROCESSING: 		/// IP Change,	
			if(REC != _TYPE_LS_SS_)
				Remapping_Self_Ip_Use_RuId(); // nokia, ericson
			else  // samsung			
				Remapping_Self_Ip_SS_Use_RuId();

			AufoConfigFlag.Ip_Assign_Step = IP_ASSIGN_FINISH;
			break;
		
		case IP_ASSIGN_FINISH: 		
			AufoConfigFlag.AutoConfig_Step = AUTUCONFIG_STEP05;
			//D_Printf( DEBUG_LS_Comm, "[Ip_Assign_Process] Finish\n" );
			break;
				
		//-------------------------------
		default: break;
		//-------------------------------
	}	
}





void AutoConfig_Proc(void)
{
	switch( AufoConfigFlag.AutoConfig_Step )
	{
		//-------------------------------		
		case AUTUCONFIG_STEP04: 		// IP_Assin_Process,	
			Ip_Assign_Process();
		break;
		
		case AUTUCONFIG_STEP05: 		// Initial_Noti_Procerdure,
			Init_Noti_Process();
		break;		
		
		case AUTUCONFIG_FINISH: 
		break;
				
		//-------------------------------
		default: break;
		//-------------------------------
	}
}



/*
초기 부팅 후 알람 보고 시점은 
L/S에서 수신되는 Alarm Acknowledge == 0x01(Acknowledge)인 조건인 경우에 전송(PRU/AAU 공통)

Acknowledge는 0x01이 되는 조건
- Init Noti 수신시 Alarm Acknowledge가 0x01(Acknowledge)로 수신되거나
- Init Noti를 처음 요청한 시점부터 1분 경과 후 Alarm Acknowledge를 자체적으로 0x01(Acknowledge) 변경
*/

static OS_TIMER timer_ls_valid_indicator;
static INT8U timer_start_ls_valid_indicator = 0;

void Timer_start_LS_Valid_Indicator(void)
{
	if(timer_start_ls_valid_indicator == 0)
	{
		OSTimerReset(&timer_ls_valid_indicator);
		timer_start_ls_valid_indicator = 1;		

		print_agent(DEBUG_LS_Comm, VT_BOLD_YELLOW, "[Timer_start_LS_Valid_Indicator] 60[s] Timer Start");		
	}
}
void Proc__LS_Valid_Indicator(void)
{
	if(AufoConfigFlag.Vaild_Indicator == 0)
	{	
		if(timer_start_ls_valid_indicator)
		{
			if(OSTimerGet_msec(&timer_ls_valid_indicator) >(60*TICK_CO_SEC))
			{
				AufoConfigFlag.Vaild_Indicator = 1;

				print_agent(DEBUG_LS_Comm, VT_BOLD_GREEN, "[Proc__LS_Valid_Indicator] Timer End.");
			}
		}
	}
}


//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------

/*
---------------------------------------------------------------------------------------------------

*gREMS Report
발생 report : Cacade_ID와 Layer Splitter 수신 ID 상이한 상태 10분 이상 유지시  
해제 report : Cacade_ID와 Layer Splitter 수신 ID 동일한 상태 10분 이상 유지시

*GUI Alarm : 
알람 발생/해제 즉시 표시.
---------------------------------------------------------------------------------------------------
*/
static OS_TIMER ID_Mismatch_timer;
static INT8U ID_Mismatch_timeout = 0;
static INT8U ID_Mismatch_TimeChk_En = 0;


#ifdef __ID_MisMatch_ALARM_TEST__
#define ID_MISMATCH_TIME_ms (2*TICK_CO_MIN)
#else
#define ID_MISMATCH_TIME_ms (10*TICK_CO_MIN)
#endif

void Proc___Mismatch_ID_Alarm(void)
{
	static INT8U init = 0;
	static INT8U timer_restart = 0;
	static INT8U alarm_prev = 0;
	INT32U timeout_ms;

	int time_s;
	static int time_s_pre = 0;
	
	if(init == 0)
	{
		init = 1;
		OSTimerReset(&ID_Mismatch_timer);
		m_St.AlmSt.ETC.Mismatch_ID = 0;
		alarm_prev = m_St.AlmSt.ETC.Mismatch_ID;
		ID_Mismatch_timeout = 0;
		ID_Mismatch_TimeChk_En = 0;
		timer_restart = 0;	
		return;
	}	
	if(m_Env.gREMS_Noti_Ready==0)
	{
		return; // return...
	}
	//---------------------------------------------------------
	if(m_St.AlmTest) // 2019.05.01
	{
		timeout_ms = 10000; // 10s

	/*
		if(m_St.AlmSt.ETC.Mismatch_ID)
			timer_restart = (alarm_prev == 0)? 1:0;
		else // ID is matched.
			timer_restart = (alarm_prev != 0)? 1:0;
	*/
		if(alarm_prev != m_St.AlmSt.ETC.Mismatch_ID)
			timer_restart = 3;	
	}
	//---------------------------------------------------------
	else
	{		
		timeout_ms = ID_MISMATCH_TIME_ms;
		
		switch(m_St.RU_Init_Status)
		{
			case Init_St_System_Init:
			case Init_St_RE_ID_Acqui:
				init = 0;
				alarm_prev = m_St.AlmSt.ETC.Mismatch_ID;
				return; // return...
				
			default:
				break;
		}		

		if(AufoConfigFlag.RU_Id != AufoConfigFlag.RE_Cascade_ID)
		{
			if(m_St.AlmSt.ETC.Mismatch_ID == 0) // if normal
			{
				timer_restart = 1;				
				m_St.AlmSt.ETC.Mismatch_ID = 1;
			}
		}
		else // ID is matched.
		{
			if(m_St.AlmSt.ETC.Mismatch_ID) // if alarm
			{
				timer_restart = 2;				
				m_St.AlmSt.ETC.Mismatch_ID = 0;
			}
		}
	}
	//---------------------------------------------------------
	if(timer_restart)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[ID_Mismatch] timer_restart(%d)", 
					timer_restart);

		ID_Mismatch_TimeChk_En = 1;
		ID_Mismatch_timeout = 0;
		OSTimerReset(&ID_Mismatch_timer);
		
		timer_restart = 0;
	}
	
	if(ID_Mismatch_TimeChk_En)
	{
		if(ID_Mismatch_timeout == 0)
		{
			if(OSTimerGet_msec(&ID_Mismatch_timer) >= timeout_ms)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[ID_Mismatch] Over 10[min], Is alarm(%d)", 
							m_St.AlmSt.ETC.Mismatch_ID);
				
				ID_Mismatch_timeout = 1;
				ID_Mismatch_TimeChk_En = 0;
				OSTimerReset(&ID_Mismatch_timer);
			}
			else
			{
				time_s = OSTimerGet_msec(&ID_Mismatch_timer)/1000;
				if(time_s_pre != time_s)
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[ID_Mismatch] timer %d[s], Is alarm(%d)", 
								time_s, m_St.AlmSt.ETC.Mismatch_ID);
				}
				time_s_pre = time_s;
			}			
		}
	}
	alarm_prev = m_St.AlmSt.ETC.Mismatch_ID;
}


INT8U IS__TimeOver_Id_Mismatch_Alm(void)
{
	return ID_Mismatch_timeout;
}
void Clr__TimeOver_Id_Mismatch_Alm(void)
{
	ID_Mismatch_timeout = 0;

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[ID_Mismatch] clear 10[min] flag");
}

/*
---------------------------------------------------------------------------------------------------
AID 0x500D Ru Initial Status

---------------------------------------------------------------------------------------------------
*/

INT8U Get___RU_Initial_Status(void)
{
	return m_St.RU_Init_Status;
}

void Set___RU_Initial_Status(INT8U st)
{
	if(st >= Init_St_Max) return;

	/*				
	2019.09.16 : CPRI Status 대기 상태에서 Delay setup이 내려오더라도 제어는 처리 하되 
		RU Init status는 절차대로 진행 되도록 수정.
	*/ 
	if(st == Init_St___Completed)
	{
		if(m_St.RU_Init_Status != Init_St_Dly_Set_Req)
		 return;
	}	
	
	// 2020.01.07 : 
	// AID 0x500D : init is requested when 'Init_St___Completed' or 'Init_St_Dly_Set_Req'
	if(st == Init_St_System_Init)
	{
		if(m_St.RU_Init_Status == Init_St___Completed)
		{
			m_Env.RU_Init_St_Reset = 1;
			m_Env.RU_Init_Cnt = 1;
			print_agent( DEBUG_SYSTEM, VT_BOLD_CYAN, "[Set___RU_Initial_St] Init");
		}
		else if((m_St.RU_Init_Status == Init_St_Dly_Set_Req) && m_Env.gREMS_Noti_Ready) 
		{
			m_Env.RU_Init_St_Reset = 1;	
			m_Env.RU_Init_Cnt = 1;
			print_agent( DEBUG_SYSTEM, VT_BOLD_CYAN, "[Set___RU_Initial_St] Init.2");
		}
	}
		
	m_St.RU_Init_Status = st;	
}


/*
---------------------------------------------------------------------------------------------------
0x04: Delay Setup Request waiting, 
	Time-out: 10분
	10분 이후에도 미수신시, gREMS연동 시작

0x05: (6/6) 
	Initialization Complete, 유효한 Delay설정값 수신시
	출력 방사, gREMS Alarm 보고 시작
---------------------------------------------------------------------------------------------------
*/



static OS_TIMER LS_Dly_Time_Out_timer;


INT8U IS_User_AMP_ON_Control(INT8U path4)
{
	if(path4 >= AMPU_4_MAX) return 0;
	
	if(m_Env.AMP_User_Ctrl[path4] == 0) return 0;
	else if(m_Eep.Pwr_Amp_En[path4] == OFF) return 0;
	else return 1;
}

INT8U IS_RU_Init_Completed(void)
{
	if(Get_Factory_Mode()==ON)
	{
		m_Env.gREMS_Noti_Ready = 1;
		m_Env.AMP_Ctrl_Allowed = 1; // 2019.09.30
		return 1;
	}

	if(m_St.RU_Init_Status == Init_St___Completed)
		return 1;
	else
		return 0;
}

INT8U Is_LS_Delay_Normal(void)
{
	INT8U abnormal = 0; // init 0
	
	if(m_Eep.LS_Delay_DL_nsec[_100M] == 0)      abnormal |= cbits[0];
	if(m_Eep.LS_Delay_DL_nsec[_100M] >= 120000) abnormal |= cbits[0];

	if(m_Eep.LS_Delay_UL_nsec[_100M] == 0)      abnormal |= cbits[1];
	if(m_Eep.LS_Delay_UL_nsec[_100M] >= 120000) abnormal |= cbits[1];

	// Alpha
	if(IS_Alpha_Mhz_Existing() && Is_SFP_Inserted(CPRI_1)) 	
	{
		if(m_Eep.LS_Delay_DL_nsec[Alpha] == 0)      abnormal |= cbits[2];
		if(m_Eep.LS_Delay_DL_nsec[Alpha] >= 120000) abnormal |= cbits[2];
		
		if(m_Eep.LS_Delay_UL_nsec[Alpha] == 0)      abnormal |= cbits[3];
		if(m_Eep.LS_Delay_UL_nsec[Alpha] >= 120000) abnormal |= cbits[3];
	}

	if(abnormal) return 0; // LS Delay is AbNormal
	else return 1; // LS Delay is Normal
}

/*
1. CPRI Auto configuration 6/6단계 진입시 LS 제조사를 확인하여 아래 동작 추가
	(1) SamSung   : Register.Capture_DL_Time(0x10020)에 0x2d000
	(2) ELG/Nokia : Register.Capture_DL_Time(0x10020)에 0x12000
*/
void Set__PRU_Reg_Capture_DL_Time(void)
{
#ifdef __TYPE__PRU__
	INT32U cur_32;
	INT32U req_32;

	cur_32 = m_Reg_Sts.RW_Capture_DL_Time.uD32;
	
	switch(AufoConfigFlag.RECTypeID)
	{
		case _TYPE_LS_SS_ : req_32 = 0x2D000; break;
		case _TYPE_LS_ELG_: req_32 = 0x12000; break;
		case _TYPE_LS_NKA_: req_32 = 0x12000; break;

		default:
			return;
	}
	if(req_32 != cur_32)
	{
		m_Reg_Flg.RW_Capture_DL_Time.uD32 = 1;
		m_Reg_Ctl.RW_Capture_DL_Time.uD32 = req_32;
								
		Write_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_DL_Time], req_32, "Capture_DL_Time");
		print_agent(DEBUG_LS_Comm, VT_GREEN, "[Capture_DL_Time] Write 0x%X", req_32);
	}					
#endif
}

void Proc___gREMS_Comm_Start(void)
{
	INT8U path4;
	static INT8U init = 0;
//	static INT8U prev_st = 0;

	if(init == 0)
	{
		init = 1;		
		m_St.RU_Init_Status = Init_St_System_Init;
//		prev_st = Init_St_System_Init;
	}

	// 2019.09.30
	if(Get_Factory_Mode()==ON) 
	{
		m_Env.gREMS_Noti_Ready = 1;
		m_Env.AMP_Ctrl_Allowed = 1;
		OSTimerReset(&LS_Dly_Time_Out_timer);
		memset(m_Env.AMP_User_Ctrl, 0, AMPU_4_MAX);

		return;
	}

	switch(m_St.RU_Init_Status)
	{
		case Init_St_System_Init:
		case Init_St_RE_ID_Acqui:
		case Init_St_Init_Notify:		
			m_Env.gREMS_Noti_Ready = 0;
			m_Env.AMP_Ctrl_Allowed = 0; // 2019.09.24
			memset(m_Env.AMP_User_Ctrl, 0, AMPU_4_MAX);
			OSTimerReset(&LS_Dly_Time_Out_timer);
			
			// 2019.12.20
			for(path4 = 0; path4 < AMPU_4_MAX; path4++)
				Set__Reason_Dis_5117(path4, Rsn_Dis_System_Ctrl);
			break;
			
		case Init_St_CPRI_St_Req:
			m_Env.gREMS_Noti_Ready = 0;			
			OSTimerReset(&LS_Dly_Time_Out_timer);
		
			m_Env.AMP_Ctrl_Allowed = 0; // 2019.09.24
			memset(m_Env.AMP_User_Ctrl, 0, AMPU_4_MAX);			
			break;
			 
		case Init_St_Dly_Set_Req:
			if(m_Env.gREMS_Noti_Ready == 0)
			{
				if(OSTimerGet_msec(&LS_Dly_Time_Out_timer) >= (10*TICK_CO_MIN))			
				{
					m_Env.gREMS_Noti_Ready = 1;
					m_Env.RU_Init_St_Reset = 0;
					print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Proc___gREMS_Comm_Start] Over 10[min], gREMS_Noti_Ready");
				}
			}
			// 2019.09.24
			if(m_Env.gREMS_Noti_Ready)
			{
				if(Is_LS_Delay_Normal()) // 실제 가지고 있는 Delay가 정상 범위일 경우
				{
					if(m_Env.AMP_Ctrl_Allowed == 0)
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Proc___gREMS_Comm_Start] Is_LS_Delay_Normal");
						m_Env.AMP_Ctrl_Allowed = 1; 
					}						
				}
			}			
			break;
			
		case Init_St___Completed:
			m_Env.AMP_Ctrl_Allowed = 1; // 2019.09.24
			Set__PRU_Reg_Capture_DL_Time();
			if(m_Env.gREMS_Noti_Ready == 0)
			{
				m_Env.gREMS_Noti_Ready = 1;
				m_Env.RU_Init_St_Reset = 0;
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[Proc___gREMS_Comm_Start] gREMS_Noti_Ready");
			}
			break;
	}
}




//---------------------------------------------------------------------------------------------------
void* Auto_Config_Task(void *data)
{	
	INT8U task_path;
	Thread_ARG_Var* p_th_arg;	
	INT8U prev_lock_st = 0;	 // init 0
	INT8U prev_RE_Cascade_ID = 0;
	INT8U auto_cfg_restart_need = 0; // init 0
	INT8U prev_REC_Port_No = 0;
	INT16U cnt = 0;
	//INT16U co = 0;
	
	#if 0
	INT8U *p = (INT8U *)&lsh_comm_rxf;
	INT8U test_buff[4] = {0x01, 0x02, 0x03, 0x04};
	#endif

	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	task_path = (INT8U)p_th_arg->Arg_v;

	while( m_Env.UDP_Init[COMM_UDP_LS] == 0 )
		OSTimeDly( OS_DELAY_1sec ); // UDP 초기화 대기

	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "Auto_Config_Task RUN %d", task_path );

	//test.uD64 = 0x1234567887654321;
	//D_Printf( DEBUG_SYSTEM, "[RCU]  type64 %d, test : %llx %lld\n", sizeof(type64), test.uD64, test.uD64 );


	Init___PRU_PYH_IC(); // 2019.08.07
	OSTimeDly( OS_DELAY_1sec *3 ); // UDP 초기화 대기	

	Set_System_Net_Info(); // Network Setting Before Read VSS-BaseIP/MAC
	
	AutoConfig_Flag_Clear();
	Init_Base_IP_Mac_Chk(); // 2019.05.03

	OSTimerReset( &Send_Ip_Assign_Noti_time );
	OSTimerReset( &Send_Init_Noti_time );

	OSTimeDly( OS_DELAY_1sec *3 ); // UDP 초기화 대기
	
	AufoConfigFlag.gREMS_Port16 = 20000; // gREMS 2000 Fix

	auto_cfg_restart_need = 0; // init

	Load__Noti_IP(); // 2011.11.11

	while( m_Env.task_flag[_TASK_AUTO_CONFIG_] )		///// YCSO TASK END
	{
		cnt = 0;
		do
		{		
			OSTimeDly(OS_DELAY_10msec);
			cnt++;
		}while(m_Env.task_flag[_TASK_AUTO_CONFIG_] && (cnt <= 50)); // 500[ms]
		Update__Task_Count(_TASK_AUTO_CONFIG_);

		Proc___gREMS_Comm_Start(); // 2019.08.21
		//-----------------------------------------
		// 2019.05.03
		// if "lock" to "unlock" and ID is not allocated.
		// below is canceled. 2019.05.07 : unlock 시, ip 할당 다시 받지는 않음.
		//-----------------------------------------		
		if(prev_lock_st != m_St.Is_CPRI_Lock) // Checked
		{
			if(m_St.Is_CPRI_Lock == 0) // 1 -> 0, Checked
			{
				if(AufoConfigFlag.RU_Id_Alloc == 0) // if RU_ID is not allocated
				{
					AutoConfig_Flag_Clear();
					Clear_net_valid_chk_cnt();
					Init_Base_IP_Mac_Chk();
					
					print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Auto_Config] CPRI Un-Lock : AutoConfig Restart...");
				}
			}
		}
		/*
		---------------------------------------------
		AutoConfig-Restart 
		When,
			1. REC port num 변경.
			2. cascade-ID 변경.
			3. base-MAC/base-IP 변경.
			4. CPRI unlock 알람 -> CPRI Lock 복귀(알람 해제) 시 : 2019.10.28
		---------------------------------------------	
		*/
		if(prev_REC_Port_No != AufoConfigFlag.REC_Port_No)
		{			
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Auto_Config] by REC_Port_No-Changed");

			auto_cfg_restart_need = 1;
			prev_REC_Port_No = AufoConfigFlag.REC_Port_No;
		}
		if(AufoConfigFlag.Base_Mac_Changed2 || AufoConfigFlag.Base_Ip_Changed2)
		{
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Auto_Config] by BaseMac/IP-Changed");

			auto_cfg_restart_need = 1;
			AufoConfigFlag.Base_Mac_Changed2 = 0;
			AufoConfigFlag.Base_Ip_Changed2 = 0;			
		}
		if(prev_RE_Cascade_ID != AufoConfigFlag.RE_Cascade_ID)
		{
			prev_RE_Cascade_ID = AufoConfigFlag.RE_Cascade_ID;
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Auto_Config] by Cascade-ID-Changed");
			auto_cfg_restart_need = 1;
		}
		/*
		----------------------------------------------------
		2019.10.28 : AutoConfig-Restart when CPRI unlock 알람 -> CPRI Lock 복귀
		----------------------------------------------------
		*/
	#ifdef __Auto_Config_When_CPRI_UnLock_to_Lock__
		if(prev_lock_st != m_St.Is_CPRI_Lock) // Changed
		{
			if(m_St.Is_CPRI_Lock)
			{
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Auto_Config] by CPRI Unlock --> Lock");
				auto_cfg_restart_need = 1;
			}
		}
	#endif
		//------------------------------------------------
		if(auto_cfg_restart_need)
		{
			auto_cfg_restart_need = 0;
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Auto_Config] Restart...");
			AutoConfig_Flag_Clear();
			Clear_net_valid_chk_cnt();
		}
		prev_lock_st = m_St.Is_CPRI_Lock;
		//-----------------------------------------
		// 2019.05.14
		if(m_St.Is_CPRI_Lock) // if lock
		{		
			Proc__LS_Valid_Indicator(); // 2019.06.18

			if(Reg_Ready_Flag)	// when CPRI_Lock
			{
				AutoConfig_Proc();
				Check_CnM_Path_Change();
				Check_Alarm_Change();
			}
			Proc___Mismatch_ID_Alarm();
		}
		else
		{
			continue;
		}
		//-----------------------------------------
	}
	m_Env.task_END_status[_TASK_AUTO_CONFIG_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}

//---------------------------------------------------------------------------------------------------



/* EOF */


