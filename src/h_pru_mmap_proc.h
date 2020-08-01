/********************************************************************************/
#ifndef __H_PRU_MMAP_PROC_H
#define __H_PRU_MMAP_PROC_H
/********************************************************************************/
#ifdef  H_PRU_MMAP_PROC_GLOBALS
#define H_PRU_MMAP_PROC_EXT
#else
#define H_PRU_MMAP_PROC_EXT extern
#endif
/**************************************************************/


///////////////////////////////////////////////////////////////////////////////////////////////////////
//def
#define DTU_INTF_BASE				0x80005000
#define DTU_STATUS_OFFSET			0x00
#define DTU_PAU_RESET_OFFSET		0x04
#define DTU_PAU_AMPEN_OFFSET		0x08
#define DTU_RESERVED0_OFFSET		0x0C
#define DTU_RESERVED1_OFFSET		0x10
#define DTU_OPTI_SW_BYPASS_OFFSET	0x14




///////////////////////////////////////////////////////////////////////////////////////////////////////
//E


///////////////////////////////////////////////////////////////////////////////////////////////////////
//st

typedef struct __H_PRU_MMAP_Status
{


	//------------------------
	// IO_Sts
	//------------------------
	union{
		INT32U Data;
		struct{
			//
			INT8U PAU0_Ins			:1;	
			INT8U PAU1_Ins 			:1;	
			INT8U PAU2_Ins			:1;
			INT8U PAU3_Ins 			:1;

			INT8U PSU_Reset_H		:1;
			INT8U PSU_Reset_L 		:1;
			INT8U PSU_Fail			:1;
			INT8U Door_Open 		:1;

			//
			INT8U Op_Sw_Sts0 		:1;
			INT8U Op_Sw_Sts1		:1;

		}__attribute__((packed));
	}IO_Sts; // addr : 0x00
	//----------------------------------------	



	//------------------------
	// PAU_Reset
	//------------------------
	union{
		INT8U Data;
		struct{
			INT8U PAU0_Reset			:1; 
			INT8U PAU1_Reset			:1; 
			INT8U PAU2_Reset			:1;
			INT8U PAU3_Reset			:1;

		}__attribute__((packed));
	}PAU_Reset; // addr : 0x04
	//----------------------------------------			

	//------------------------
	// PAU_En
	//------------------------
	union{
		INT8U Data;
		struct{
			INT8U PAU0_En			:1; 
			INT8U PAU1_En			:1; 
			INT8U PAU2_En			:1;
			INT8U PAU3_En			:1;

		}__attribute__((packed));
	}PAU_En; // addr : 0x08
	//----------------------------------------	

	//------------------------
	// Op_Sw_Bypass
	//------------------------
	union{
		INT8U Data;
		struct{
			INT8U En			:1; 

		}__attribute__((packed));
	}Op_Sw_En; // addr : 0x14
	//----------------------------------------	

}ST_H_PRU_MMAP_Status;


///////////////////////////////////////////////////////////////////////////////////////////////////////


H_PRU_MMAP_PROC_EXT void* H_PRU_Rfu_RW_Task(void *data);
H_PRU_MMAP_PROC_EXT int Write_H_PRU_Mmap(int addr, int data);
H_PRU_MMAP_PROC_EXT int Read_H_PRU_Mmap(int addr);


H_PRU_MMAP_PROC_EXT ST_H_PRU_MMAP_Status H_PRU_MMAP_Status;



/********************************************************************************/
#endif
/********************************************************************************/


