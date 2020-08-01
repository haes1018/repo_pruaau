/********************************************************************************/
#ifndef __INVENTORY_H
#define __INVENTORY_H
/********************************************************************************/
#ifdef  INVENTORY_GLOBALS
#define INVENTORY_EXT
#else
#define INVENTORY_EXT extern
#endif
/**************************************************************/

#ifdef __TYPE__PRU__ // PRU Only
enum // inventory Ctrl flag bit
{
	// 1st ------------------------
	inv_DTU   = 7, // 2020.07.28.
	inv_PSU   = 6, // 2020.07.28.
	inv_PAU_1 = 5, // 2020.07.28.
	inv_PAU_2 = 4, // 2020.07.28.
	inv_PAU_3 = 3, // 2020.07.28.
	inv_PAU_4 = 2, // 2020.07.28.
	inv_SFP_0 = 1, // 2020.07.28.
	inv_SFP_1 = 0, // 2020.07.28.
	// 2nd ------------------------
	inv_SFP_0_a=7, // 2020.07.28.
	inv_SFP_1_a=6, // 2020.07.28.	
};
#else // AAU
enum // inventory Ctrl flag bit
{
	// 1st ------------------------
	inv_DTU     = 7, // 2020.07.28.
	inv_SFP_0   = 6, // 2020.07.28.
	inv_SFP_1   = 5, // 2020.07.28.
	inv_SFP_0_a = 4, // 2020.07.28.
	inv_SFP_1_a = 3, // 2020.07.28.
	inv_RCU     = 2, // 2020.07.28.	
};
#endif

typedef struct
{
	INT8U mod_flag[2];
	INT8U flag[2];
	INVENTORY_STR inv;
} INVENTORY_Ctrl_STR;

INVENTORY_EXT INVENTORY_STR inv, inv_r;
INVENTORY_EXT INT8U Inv_Read_Needed( INT8U path4 );
INVENTORY_EXT void Clr_Inv_Read_Req( INT8U path4 );
INVENTORY_EXT void Clr_Psu_Inv_Read_Req(void);
INVENTORY_EXT INT8U Get_Inv_idx(INT8U path4);
INVENTORY_EXT void Update_SFP_Inventory(void);


INVENTORY_EXT void InventoryPrevUpdate( void );
INVENTORY_EXT INT16U CheckInvetoryUpdate( INT8U* sdu );

INVENTORY_EXT void Get_KST_inventory(void);


/*----------------------------------------------------------*/


/********************************************************************************/
#endif
/********************************************************************************/
