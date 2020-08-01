/********************************************************************************/
#ifndef __H_AAU_MMAP_PROC_H
#define __H_AAU_MMAP_PROC_H
/********************************************************************************/
#ifdef  H_AAU_MMAP_PROC_GLOBALS
#define H_AAU_MMAP_PROC_EXT
#else
#define H_AAU_MMAP_PROC_EXT extern
#endif
/**************************************************************/


///////////////////////////////////////////////////////////////////////////////////////////////////////
//def
#define MAP_BASE_DIGITAL		0xA0100000
#define MAP_BASE_RF				0xB0000000
#define MAP_SIZE_DIGITAL 		0x100000UL
#define MAP_SIZE_RF 			0x010000UL
//#define MAP_MASK 				(MAP_SIZE - 1)


#define H_AAU_MMAP_DEVICE  "/dev/uio2" // hfr aau mmap



///////////////////////////////////////////////////////////////////////////////////////////////////////
//E

enum
{
//	MMAP_UL_ATT_SEL 			= 0x00,
//	MMAP_UL_ATT_Data 			= 0x04,
//	MMAP_DL_ATT_SEL 			= 0x08,
//	MMAP_DL_ATT_Data 			= 0x0C,
	MMAP_UL_LNA_Ctrl 			= 0x10,
	MMAP_DL_AMP_Ctrl 			= 0x14,
	MMAP_UL_SW 					= 0x18,
	MMAP_DL1_Out_PWR_Det 		= 0x1C,
	MMAP_DL2_Out_PWR_Det 		= 0x20,
	MMAP_DL3_Out_PWR_Det 		= 0x24,
	MMAP_DL4_Out_PWR_Det 		= 0x28,
	
	MMAP_UL1_ATT_Data 			= 0x40,
	MMAP_UL2_ATT_Data 			= 0x44,
	MMAP_UL3_ATT_Data 			= 0x48,
	MMAP_UL4_ATT_Data 			= 0x4C,

	MMAP_DL1_ATT_Data 			= 0x50,
	MMAP_DL2_ATT_Data 			= 0x54,
	MMAP_DL3_ATT_Data 			= 0x58,
	MMAP_DL4_ATT_Data 			= 0x5C,

	
	MMAP_MAX,
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
//st

typedef struct __H_MMAP_Status
{
	INT8U DL_Att_x2[MAX_DL]; //
	INT8U UL_Att_x2[MAX_UL]; //

	//------------------------
	// DL En
	//------------------------
	union{
		INT8U Data;
		struct{
			INT8U AMP_1				:1;		//1:On 0:Off
			INT8U AMP_2 			:1;
			INT8U AMP_3 			:1;
			INT8U AMP_4 			:1;

			
			INT8U Mode 				:1;		//1:Manual 0:Auto Tdd
		}__attribute__((packed));
	}DL_En; // addr : 0x00
	//----------------------------------------			

	//------------------------
	// UL LNA En
	//------------------------
	union{
		INT8U Data;
		struct{
			INT8U LNA_1				:1;		//1:On 0:Off
			INT8U LNA_2 			:1;
			INT8U LNA_3 			:1;
			INT8U LNA_4 			:1;

			
			INT8U Mode 				:1;		//1:Manual 0:Auto Tdd
		}__attribute__((packed));
	}UL_LNA_En; // addr : 0x00
	//----------------------------------------	


	//------------------------
	// UL SW
	//------------------------
	union{
		INT16U Data;
		struct{

			//
			INT8U UL_Loop_1 		:1;		//1:Loop On 0:Loop Off
			INT8U UL_Loop_2 		:1;
			INT8U UL_Loop_3 		:1;
			INT8U UL_Loop_4 		:1;

			
			INT8U UL_SW_1			:1;		//1:DL 0:UL
			INT8U UL_SW_2			:1;
			INT8U UL_SW_3			:1;
			INT8U UL_SW_4			:1;
			

			//
			INT8U Mode 				:1;		//1:Manual 0:Auto Tdd

		}__attribute__((packed));
	}UL_SW; // addr : 0x00
	//----------------------------------------	



	INT16S DL_Out_Pwr_x10[MAX_DL];

}ST_H_MMAP_Status;


///////////////////////////////////////////////////////////////////////////////////////////////////////


H_AAU_MMAP_PROC_EXT void* H_AAU_Rfu_RW_Task(void *data);
H_AAU_MMAP_PROC_EXT int Write_H_AAU_Mmap(int addr, int data);
H_AAU_MMAP_PROC_EXT int Read_H_AAU_Mmap(int addr);
H_AAU_MMAP_PROC_EXT void Close_H_AAU_Mmap(void);


H_AAU_MMAP_PROC_EXT ST_H_MMAP_Status H_MMAP_Status;



/********************************************************************************/
#endif
/********************************************************************************/


