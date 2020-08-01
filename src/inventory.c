/////////////////////////////
// INVENTORY.C
/////////////////////////////
#define INVENTORY_GLOBALS
#include "includes.h"
/////////////////////////////



/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////



//=====================================================================================================
// Inventory
//=====================================================================================================

void Inventory_Vaule_Set( INVENTORY_STR* pinv, INVENTORY_Ctrl_STR* pctrl );

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

static RHU_INVENTORY_STR *RHU_Inv_arry[MAX_INVENTORY];
static INT8U RHU_Max_Inv_Num;



//----------------------------------------------------------------------
void InventoryPrevUpdate( void )
{
	INT16U i;	
	
	for( i=0; i < RHU_Max_Inv_Num; i++ )
	{
		memcpy((char*)&RHU_Inventory_Prev[i].inv, (char*)&RHU_Inventory[i].inv, sizeof(INVENTORY_STR));
	}
}

INT16U CheckInvetoryUpdate( INT8U* sdu )
{
	INT16U l=0;
	INT16U i;

	INT8U chage_falg = 0;


	Update_SFP_Inventory(); // 5G SFP Inventory
	
	for(chage_falg=0, i=0; i < RHU_Max_Inv_Num; i++ )
	{
	#ifdef __TYPE__PRU__
		RHU_Inventory[i].inv.Rep_Type[0] = m_St.Type[0];
		RHU_Inventory[i].inv.Rep_Type[1] = Get_Repeater_Type();
	#else
		RHU_Inventory[i].inv.Rep_Type[0] = m_St.Type[0];
		RHU_Inventory[i].inv.Rep_Type[1] = m_St.Type[1];	
	#endif	

		if(memcmp((char*)&RHU_Inventory[i].inv, (char*)&RHU_Inventory_Prev[i].inv, sizeof(INVENTORY_STR))) // if different
		{
			chage_falg = 1;
		}
		memcpy((char*)&RHU_Inventory_Prev[i].inv, (char*)&RHU_Inventory[i].inv, sizeof(INVENTORY_STR));
	}
	
	if(chage_falg)
	{	
		for(i=0; i < RHU_Max_Inv_Num; i++)
		{
			memcpy(&sdu[l], &(RHU_Inventory[i].inv), sizeof(INVENTORY_STR));
			l+=sizeof(INVENTORY_STR);
		}
	}	
	
	return l;
}



//----------------------------------------------------------------------
void InitialInvValue( INT8U inv_idx, INT8U def_type, INT8U is_install ) // iro
{
	RHU_Inventory[inv_idx].inv_idx = inv_idx;
	RHU_Inventory[inv_idx].type = def_type;
	
	if(is_install)	
	{
		RHU_Inventory[inv_idx].inv.Type = 0x4E;
		RHU_Inventory[inv_idx].is_read = 0;
	}
	else
	{
		RHU_Inventory[inv_idx].inv.Type = 0x7F;
		RHU_Inventory[inv_idx].is_read = 1;
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
// module (Digital, PAU 등 ) 에서 받은 inventory 응답을
// 해당 위치에 그대로 저장
// is_read 멤버 값을 1로 설정한다.
// 해당 모듈에서 inventory 조회 할지 여부를 결정.
void RecvModuleInventory( INT8U idx, INVENTORY_STR* inv )
{
	RHU_Inventory[idx].is_read = 1;
	memcpy( &(RHU_Inventory[idx].inv), inv, sizeof(INVENTORY_STR) );
}
//----------------------------------------------------------------------
// inventory 값을 읽어 왔는지 여부 체크.
INT8U CheckInvetoryRead( INT8U idx )
{
	return RHU_Inventory[idx].is_read;
}
//----------------------------------------------------------------------
// RCU, PSU, CASE 처럼, RCU가 inventory를 직접 save 하는 곳에,
// pctrl 로 넘겨 받은 inventory 설정 값을, 
// 플래그 설정 여부에 따라, pinv 에 save.(overwrite)
//
// Set pinv with pctrl
//
void Inventory_Vaule_Set( INVENTORY_STR* pinv, INVENTORY_Ctrl_STR* pctrl )
{
	if( pctrl->flag[0] & cbits[6] )	pinv->Type = pctrl->inv.Type;
	if( pctrl->flag[0] & cbits[5] )
	{
		pinv->Rep_Type[0] = pctrl->inv.Rep_Type[0];
		pinv->Rep_Type[1] = pctrl->inv.Rep_Type[1];
	}
	if( pctrl->flag[0] & cbits[4] )	pinv->supply = pctrl->inv.supply;
	if( pctrl->flag[0] & cbits[3] )	pinv->maker = pctrl->inv.maker;
	if( pctrl->flag[0] & cbits[2] )	pinv->Ver = pctrl->inv.Ver;
	if( pctrl->flag[0] & cbits[1] )
		memcpy( pinv->sn, pctrl->inv.sn, 4 );
	
	if( pctrl->flag[0] & cbits[0] )
	{
		pinv->date[0] = pctrl->inv.date[0];
		pinv->date[1] = pctrl->inv.date[1];
		pinv->date[2] = pctrl->inv.date[2];
	}

	if( pctrl->flag[1] & cbits[6] )
		memcpy( pinv->ERP_code, pctrl->inv.ERP_code, 15 );
	
	if( pctrl->flag[1] & cbits[5] )
		memcpy( pinv->Maker_Mode, pctrl->inv.Maker_Mode, 4 );
	//----
}

//----------------------------------------------------------
INT8U Get_Inv_idx(INT8U path4)
{	
#ifdef __TYPE__PRU__
	switch(path4)
	{
		case AMPU_4_1: return INV_IDX_PAU_1;
		case AMPU_4_2: return INV_IDX_PAU_2;
		case AMPU_4_3: return INV_IDX_PAU_3;
		case AMPU_4_4: return INV_IDX_PAU_4;
	}	
#endif	
	return 0;
}
//----------------------------------------------------------
void InvetoryReadClear_mod( INT8U idx ) // iro
{
	if(idx < MAX_INVENTORY)
	{
		RHU_Inventory[idx].is_read = 0;
	}
}
//----------------------------------------------------------

OS_TIMER Inv_rd_en_timer[AMPU_4_MAX];
static INT8U Inv_rd_req[AMPU_4_MAX] = {0,0,0,0};

void Clr_Inv_Read_Req( INT8U path4 )
{
	Inv_rd_req[path4] = 0;
}
//----------------------------------------------------------
void PAU_Inv_Read_Req(INT8U path4 )
{	
	InvetoryReadClear_mod( Get_Inv_idx(path4));

	Inv_rd_req[path4] = 1;
	OSTimerReset( &Inv_rd_en_timer[path4] );
}
//----------------------------------------------------------
static INT8U Inv_Psu_rd_req = 0;
void Clr_Psu_Inv_Read_Req(void)
{
	Inv_Psu_rd_req = 0;
}
//----------------------------------------------------------
void Psu_Inv_Read_Req(void)
{	
#ifdef __TYPE__PRU__
	Inv_Psu_rd_req = 0;
	InvetoryReadClear_mod(INV_IDX_R_PSU);
#endif	
}
//----------------------------------------------------------
INT8U Inv_Read_Needed( INT8U path4 )
{
	if( Inv_rd_req[path4] )
	{
		if( OSTimerOverCheck_msec( &Inv_rd_en_timer[path4], 30*TICK_CO_SEC ) )
		{
			Inv_rd_req[path4] = 0;
		}		
	}	
	return ( Inv_rd_req[path4] );
}
//----------------------------------------------------------------------
void InitInventory( void )
{	
/*
---------------------
0x05 : Digital Module
---------------------
SFP
---------------------
0xCC: 10G(1km)(Tx-1270) : TR (CRPI2,CPR3)
0xCD: 10G(1km)(Tx-1330) : IR (CRPI0,CPR1)
---------------------
PAU
---------------------
0x4E: HPA(5G(3.5G) + LNA)
---------------------
*/

//-------------------------------------------------------------
#ifdef __TYPE__PRU__ // PRU Only
//-------------------------------------------------------------
	InitialInvValue( INV_IDX_R_DTU, 	 0x05, 1 ); // 0x05 : DTU
	InitialInvValue( INV_IDX_SFP_CPRI_0, 0xCD, 1 ); // SFP
	InitialInvValue( INV_IDX_SFP_CPRI_1, 0xCD, 1 ); // SFP
	InitialInvValue( INV_IDX_SFP_CPRI_2, 0xCC, 1 ); // SFP
	InitialInvValue( INV_IDX_SFP_CPRI_3, 0xCC, 1 ); // SFP

	InitialInvValue( INV_IDX_R_PSU, 0x16, 1 ); // 0x16 : Remote PSU			
	InitialInvValue( INV_IDX_PAU_1,	0x4E, 1 ); // Check_PAU_Use(AMPU_4_1));
	InitialInvValue( INV_IDX_PAU_2,	0x4E, 1 ); // Check_PAU_Use(AMPU_4_2));
	InitialInvValue( INV_IDX_PAU_3,	0x4E, 1 ); // Check_PAU_Use(AMPU_4_3));	
	InitialInvValue( INV_IDX_PAU_4, 0x4E, 1 ); // Check_PAU_Use(AMPU_4_4));	
	
	// Inventory 응답에 사용하기 위해 응답 순서대로 배열.	
	//
    //
    /* 2020.07.27 : Inventory Status Sequence 변경
     * PRU : DTU - PSU -  PAU0/1/2/3 - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd
     * */
	RHU_Max_Inv_Num=0;
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_R_DTU];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_R_PSU];	
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_PAU_1];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_PAU_2];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_PAU_3];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_PAU_4];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_0];	
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_2];	
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_1];	
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_3];
	
	// RCU(DTU) : read 완료로 초기화. 
	RHU_Inventory[INV_IDX_R_DTU].is_read = 1;
	RHU_Inventory[INV_IDX_R_DTU].inv.Type = RHU_Inventory[INV_IDX_R_DTU].type;
	RHU_Inventory[INV_IDX_R_DTU].inv.Rep_Type[0] = m_St.Type[0];
	RHU_Inventory[INV_IDX_R_DTU].inv.Rep_Type[1] = Get_Repeater_Type();
	RHU_Inventory[INV_IDX_R_DTU].inv.supply = m_St.Maker[0];
	RHU_Inventory[INV_IDX_R_DTU].inv.maker = m_St.Maker[1];
	
	RHU_Inventory[INV_IDX_R_DTU].inv.Ver = m_St.Int_Ver[0];
	RHU_Inventory[INV_IDX_R_DTU].inv.Ver = 0x10; // temporary

	
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.sn, m_Eep.sn, 4 );
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.date, m_Eep.date, 3 );
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.ERP_code, m_Eep.ERP_code, 15 );
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.Maker_Mode, m_Eep.Maker_Mode, 4 );
	
//-------------------------------------------------------------	
#else // ANTS-AAU	
//-------------------------------------------------------------
	InitialInvValue( INV_IDX_RCU, 0x00, 1 ); // 0x00 : RCU, ANTS-AAU Only
	InitialInvValue( INV_IDX_R_DTU, 	 0x05, 1 ); // 0x05 : DTU
	InitialInvValue( INV_IDX_SFP_CPRI_0, 0xCD, 1 ); // SFP
	InitialInvValue( INV_IDX_SFP_CPRI_1, 0xCD, 1 ); // SFP
	InitialInvValue( INV_IDX_SFP_CPRI_2, 0xCC, 1 ); // SFP
	InitialInvValue( INV_IDX_SFP_CPRI_3, 0xCC, 1 ); // SFP
	// Inventory 응답에 사용하기 위해 응답 순서대로 배열.	
	//
    /* 2020.07.27 : Inventory Status Sequence 변경
     * AAU : DTU - SFP_100 - SFP_100_2nd - SFP_Alpha- SFP_Alpha_2nd - RCU
     * */
	RHU_Max_Inv_Num=0;	
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_R_DTU];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_0];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_1];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_2];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_SFP_CPRI_3];
	RHU_Inv_arry[RHU_Max_Inv_Num++] = &RHU_Inventory[INV_IDX_RCU];
	//--------	
	// RCU 기본 inventory init
	//--------
	RHU_Inventory[INV_IDX_RCU].is_read = 1;
	RHU_Inventory[INV_IDX_RCU].inv.Type = RHU_Inventory[INV_IDX_RCU].type;
	RHU_Inventory[INV_IDX_RCU].inv.Rep_Type[0] = m_St.Type[0];
	RHU_Inventory[INV_IDX_RCU].inv.Rep_Type[1] = m_St.Type[1];
	RHU_Inventory[INV_IDX_RCU].inv.supply = m_St.Maker[0];
	RHU_Inventory[INV_IDX_RCU].inv.maker = 0xA0; // 0xA0 : KST
	RHU_Inventory[INV_IDX_RCU].inv.Ver = m_St.Int_Ver[0];
	
	memcpy( RHU_Inventory[INV_IDX_RCU].inv.sn, m_Eep.sn, 4 );
	memcpy( RHU_Inventory[INV_IDX_RCU].inv.date, m_Eep.date, 3 );
	memcpy( RHU_Inventory[INV_IDX_RCU].inv.ERP_code, m_Eep.ERP_code, 15 );
	memcpy( RHU_Inventory[INV_IDX_RCU].inv.Maker_Mode, m_Eep.Maker_Mode, 4 );
	

//-------------------------------------------------------------
#endif
//-------------------------------------------------------------
}
//----------------------------------------------------------------------
// 2019.04.06 : SFP inventory
void Update_SFP_Inventory(void)
{
	INT32U* pInv;
	INT8U cpri, k;
	INT8U idx;
		
	//----------------------------------
	// DTU When AAU-ANTS
	// 2019.04.29 : Inventory AAU-FPGA Added.
	//----------------------------------
#ifdef __TYPE__AAU__ // ANTS-AAU
	/*
	(1) Copy FPGA.32byte.Reg to Inv[R_DTU]
	(2) Copy RCU's_S/N ~ MakerCode to Inv[R_DTU] : share RCU's_S/N ~ MakerCode with Inv[R_DTU]
	*/
	pInv = (INT32U*)&RHU_Inventory[INV_IDX_R_DTU].inv.Type;	
	
	for(k = 0; k < 8; k++) // block copy
		pInv[k] = htonl(m_Reg_Sts.RW_DTU_Inventory[k].uD32);

	// 2019.05.10 : DTU use RCU's S/N and Date.
	// RCU share S/N and Date with DTU
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.sn,   RHU_Inventory[INV_IDX_RCU].inv.sn,   4 );
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.date, RHU_Inventory[INV_IDX_RCU].inv.date, 3 );	
	
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.ERP_code,   RHU_Inventory[INV_IDX_RCU].inv.ERP_code, 15 );
	memcpy( RHU_Inventory[INV_IDX_R_DTU].inv.Maker_Mode, RHU_Inventory[INV_IDX_RCU].inv.Maker_Mode, 4 );
#endif

	//----------------------------------
	// SFP 
	//----------------------------------
	// MAKER_ANTS
	for(cpri = 0; cpri < CPRI_PATH; cpri++)	
	{
		     if(cpri == CPRI_1) idx = INV_IDX_SFP_CPRI_1;
		else if(cpri == CPRI_2) idx = INV_IDX_SFP_CPRI_2;
		else                    idx = INV_IDX_SFP_CPRI_0 + cpri;
		
		pInv = (INT32U*)&RHU_Inventory[idx].inv.Type;		

		if(Is_SFP_Inserted(cpri) == 0) // if not insert
		{
			// 2019.04.25
			memset( &(RHU_Inv_arry[idx]->inv), 0, sizeof(INVENTORY_STR));
			RHU_Inv_arry[idx]->inv.Type = 0x7F; // 0x7F is No-device
			continue;
		}
		
		for(k = 0; k < 8; k++) // block copy
			pInv[k] = htonl(m_Reg_Sts.Common_CPRI[cpri].R_SFP_Inventory[k].uD32);
	}
}
//----------------------------------------------------------------------
// 5분마다 inventory를 다시 읽기 위해, is_read 멤버 변수를 클리어해서, 
// inventory 읽지않음 상태로 둔다.
void InvetoryReadClear( void ) // iro
{
#ifdef __TYPE__PRU__
	RHU_Inventory[INV_IDX_R_PSU].is_read = 0;
	RHU_Inventory[INV_IDX_PAU_1].is_read = 0;
	RHU_Inventory[INV_IDX_PAU_2].is_read = 0;
	RHU_Inventory[INV_IDX_PAU_3].is_read = 0;
	RHU_Inventory[INV_IDX_PAU_4].is_read = 0;
#endif	
}


// RCU reset 후, auto inventory 1회 report
//
// inven 조회가 다 되었는지 체크.
INT8U Auto_Inv_Ready( void )
{
#ifdef __TYPE__PRU__
#if 1
	return 1; // all read, 2019.09.04
#else	
	INT8U path4;
	INT8U inv;
	
	if(RHU_Inventory[INV_IDX_R_PSU].is_read == 0) return 0; 
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		if(Check_PAU_Use(path4))
		{
			inv = Get_Inv_idx(path4);			
			if(RHU_Inventory[inv].is_read == 0)
				return 0;
		}
	}
	return 1; // all read
#endif	
#else //------------------------- AAU
	return 1; // all read
#endif	
}

//----------------------------------------------------------------------
// 모든 함체/모듈의 인벤터리 항목들을 sdu에 복사해 넣는다. 
// 전체 길이를 리턴한다.
INT16U Get_Inventory( INT8U* sdu )
{
	INT16U l=0;
	INT16U i;

	Update_SFP_Inventory(); // 5G SFP Inventory
		
	// 2019.04.25
	#ifdef __TYPE__PRU__
	for( i=0; i < RHU_Max_Inv_Num; i++ )
	{		
		switch(i)
		{
			case INV_IDX_PAU_1:
			case INV_IDX_PAU_2:
			case INV_IDX_PAU_3:
			case INV_IDX_PAU_4: 
				if(Is_Amp_Install(i-INV_IDX_PAU_1)==0) // if not insert
				{
					memset( &(RHU_Inv_arry[i]->inv), 0, sizeof(INVENTORY_STR));
					RHU_Inv_arry[i]->inv.Type = 0x7F; // 0x7F is No-device			
				}
				break;				
		}
	}
	#endif
	//------------------------------------
	for( i=0; i < RHU_Max_Inv_Num; i++ )
	{	
	#ifdef __TYPE__PRU__
		RHU_Inv_arry[i]->inv.Rep_Type[0] = m_St.Type[0];
		RHU_Inv_arry[i]->inv.Rep_Type[1] = Get_Repeater_Type();
	#else
		RHU_Inv_arry[i]->inv.Rep_Type[0] = m_St.Type[0];
		RHU_Inv_arry[i]->inv.Rep_Type[1] = m_St.Type[1];
	#endif
		
		memcpy( &sdu[l], &(RHU_Inv_arry[i]->inv), sizeof(INVENTORY_STR) );
		l+=sizeof(INVENTORY_STR);
	}

	// __INVENTORY_UPDATE_CHECK_20190718__
	InventoryPrevUpdate();
	
	return l;
}


/*
--------------------------------------------------------------
2019.08.29
Check Inventory.Sn is valid or not
--------------------------------------------------------------
*/	
INT8U IS__Valid_InvSN(INT8U* inv_sn)
{
	INT8U val;
	INT8U year;

	print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[Control Inv.SN] 0x%02X 0x%02X 0x%02X 0x%02X",
		inv_sn[0],inv_sn[1],inv_sn[2],inv_sn[3]);

	// year --------------
	val = inv_sn[0]>>4;
	if(val < 1) return 0;
	if(val > 4) return 0;
	
	val = inv_sn[0]&0x0F;
	if(val > 9) return 0;

	year = (inv_sn[0]>>4)*10 + (inv_sn[0]&0x0F);
	if(year < 19) return 0;
	if(year > 44) return 0;

	// month --------------
	val = inv_sn[1]>>4;
	if(val < 1) return 0;
	if(val > 12) return 0;

	// S/N --------------
	/*
	val = inv_sn[1]&0x0F;
	if(val > 9) return 0;
	
	val = inv_sn[2]>>4;
	if(val > 9) return 0;
	
	val = inv_sn[2]&0x0F;
	if(val > 9) return 0;
	
	val = inv_sn[3]>>4;
	if(val > 9) return 0;
	
	val = inv_sn[3]&0x0F;
	if(val > 9) return 0;
	*/

	return 1;
}



//----------------------------------------------------------------------
// GUI에서 Inventory 제어 및 저장.
// inventory set : sdu 값을 Control_Inventory 함수에 넘기고, 이때
// 제어대상이 되는 모듈을 플래그를 보고 결정한다. 
//
void SetInventory( INT8U* sdu, INT16U len )
{	
	INVENTORY_Ctrl_STR* pctrl;
    RHU_INVENTORY_STR* pinv;
	INT16U l=0;
	INT8U valid_sn;
	
	#ifdef __TYPE__PRU__ // PRU Only	
	INT8U inv;	
	INT8U path4;
	INT8U req_amp = 0;
	#endif
	
	
	if((len%sizeof(INVENTORY_Ctrl_STR)) != 0) 
		return;

	pctrl = (INVENTORY_Ctrl_STR*)&sdu[l];

//--------------------------------------------	
#ifdef __TYPE__PRU__ // PRU Only	
//--------------------------------------------
	//-----------------------
	// RCU
	//-----------------------
	if( pctrl->mod_flag[0] & cbits[inv_DTU]) 
	{
        pinv = &RHU_Inventory[INV_IDX_R_DTU];
		pctrl->flag[0]&=0x3F;
		pctrl->flag[1]&=0x60;

		if( pctrl->flag[0] || pctrl->flag[1] )
		{			
			pctrl->flag[0] &= nbits[5];
			pctrl->flag[0] &= nbits[4];
			pctrl->flag[0] &= nbits[3]; // 제조사
			pctrl->flag[0] &= nbits[2]; // version

			Inventory_Vaule_Set( &pinv->inv, pctrl );

			/*			
				flag[0].Bit1 : Serial Number
				flag[0].Bit0 : 제조 년/월/일
				
				flag[1].Bit6 : ERP Code
				flag[1].Bit5 : 제조사 Mode
			*/

			// 2019.08.29 : check Inv.SN is valid or not			
			if(pctrl->flag[0] & cbits[1])
			{
				if(IS__Valid_InvSN(pctrl->inv.sn)) 
				     valid_sn = 1;
				else valid_sn = 0;
			}
			else valid_sn = 1;
			
			if(valid_sn)
				memcpy( m_Eep.sn, pinv->inv.sn, 4 );
			else
				print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[Inventory] S/N is Invalid !!");
			
			memcpy( m_Eep.date, pinv->inv.date, 3 );
			memcpy( m_Eep.ERP_code, pinv->inv.ERP_code, 15 );
			memcpy( m_Eep.Maker_Mode, pinv->inv.Maker_Mode, 4 );
		
			m_Env.is_eep_write = EEP_WRITE_CODE;
		}
	}
	//-----------------------
	// PAU
	//-----------------------	
	else if(pctrl->mod_flag[0] & cbits[inv_PAU_1]) { req_amp = 1; path4 = AMPU_4_1; inv = INV_IDX_PAU_1; }
	else if(pctrl->mod_flag[0] & cbits[inv_PAU_2]) { req_amp = 1; path4 = AMPU_4_2; inv = INV_IDX_PAU_2; }
	else if(pctrl->mod_flag[0] & cbits[inv_PAU_3]) { req_amp = 1; path4 = AMPU_4_3; inv = INV_IDX_PAU_3; }
	else if(pctrl->mod_flag[0] & cbits[inv_PAU_4]) { req_amp = 1; path4 = AMPU_4_4; inv = INV_IDX_PAU_4; }	
	//-----------------------
	// PSU
	//-----------------------	
	else if(pctrl->mod_flag[0] & cbits[inv_PSU])
	{
		// @ iRo
		pinv = &RHU_Inventory[INV_IDX_R_PSU];
		pctrl->flag[0]&=0x7F;
		pctrl->flag[1]&=0x60;
		pctrl->inv.maker&=0xFF;
		if( pctrl->flag[0] || pctrl->flag[1] )
		{
			sendPsu_INFO_Ctrl( &pctrl->flag[0], &pctrl->inv);			
		}
	}

	if(req_amp)
	{
		pinv = &RHU_Inventory[inv];
		pctrl->flag[0]&=0x7F;
		pctrl->flag[1]&=0x60;
		pctrl->inv.maker&=0xFF;
		if( pctrl->flag[0] || pctrl->flag[1] )
		{
			sendPAU_INFO_Ctrl(path4, &pctrl->flag[0], &pctrl->inv);			
		}
	}
//--------------------------------------------		
#else // AAU
//--------------------------------------------	
	//-----------------------
	// RCU
	//-----------------------
	// DTU is RCU @ AAU.
	// RCU is controlled @ AAU
	if( pctrl->mod_flag[0] & cbits[inv_DTU]) 
	{
		pinv = &RHU_Inventory[INV_IDX_RCU]; 
		pctrl->flag[0]&=0x3F;
		pctrl->flag[1]&=0x60;

		if( pctrl->flag[0] || pctrl->flag[1] )
		{			
			pctrl->flag[0] &= nbits[5];
			pctrl->flag[0] &= nbits[4];
			pctrl->flag[0] &= nbits[3]; // 제조사
			pctrl->flag[0] &= nbits[2]; // version

			Inventory_Vaule_Set( &pinv->inv, pctrl );
			
			// 2019.08.29 : check Inv.SN is valid or not			
			if(pctrl->flag[0] & cbits[1])
			{
				if(IS__Valid_InvSN(pctrl->inv.sn)) 
				     valid_sn = 1;
				else valid_sn = 0;
			}
			else valid_sn = 1;
			
			if(valid_sn)
				memcpy( m_Eep.sn, pinv->inv.sn, 4 );
			else
				print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "[Inventory] S/N is Invalid !!");
			
			memcpy( m_Eep.date, pinv->inv.date, 3 );
			memcpy( m_Eep.ERP_code, pinv->inv.ERP_code, 15 );
			memcpy( m_Eep.Maker_Mode, pinv->inv.Maker_Mode, 4 );

			m_Env.is_eep_write = EEP_WRITE_CODE;
		}
	}

//--------------------------------------------	
#endif	
//--------------------------------------------	 
}


//// YCSO added 190728 START
#define KST_INVENTORY_FILE		"/tmp/app/inventory.dat"


void Get_KST_inventory( void )
{
#ifdef __TYPE__AAU__

	INVENTORY_Ctrl_STR kst_inventory;
	int fd;
	int re = 0;

	fd = open( KST_INVENTORY_FILE , O_RDONLY);

	if( fd < 0 )
	{
		printf("file read inventory file\n");
		re = 1;
	}

	else
	{
		re = read(fd, (INT8U*)&kst_inventory.mod_flag[0], sizeof(INVENTORY_Ctrl_STR));

		if( re < sizeof(INVENTORY_Ctrl_STR))
		{
			printf("file read inventory file\n");
			return;
		}

		close(fd);
		
		memcpy(m_St.kst_inventory_SN	, kst_inventory.inv.sn	, 4);
		memcpy(m_St.kst_inventory_date	, kst_inventory.inv.date, 3);
	}

#endif

}

//// YCSO added 190728 END
//----------------------------------------------------------------------

/* EOF */


