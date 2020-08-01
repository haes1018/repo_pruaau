/********************************************************************************/
#ifndef __A_PRU_MMAP_PROC_H
#define __A_PRU_MMAP_PROC_H
/********************************************************************************/
#ifdef  A_PRU_MMAP_PROC_GLOBALS
#define A_PRU_MMAP_PROC_EXT
#else
#define A_PRU_MMAP_PROC_EXT extern
#endif
/**************************************************************/

/********************************************************************************/
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
/********************************************************************************/
#define ANTS_PRU_MMAP_DEV       "/dev/mem"
enum // offset
{
	MMAP_CPRI_Status 			= 0x0, // CPRI_Status
	MMAP_Mscl_Status 			= 0x4, // Miscellaneous Status
	MMAP____LoopBack 			= 4*0x8, // Transceiver Loopback and Ethernet Reset
	
	// 2019.05.20	
	MMAP_____0x0b = 4*0xB,
	MMAP_____0x0d = 4*0xD,
	MMAP____SW_Reset = 4*0xE,
};

typedef struct __A_MMAP_Status
{
	INT32U CPRI_Status; // CPRI_Status, 0 번지.	
		
	/*
		Miscellaneous Status	
		------------------------------
		15 Latched version of local loss of frame synchronization (LOF)
		14 Latched version of local loss of signal (LOS)
		11 Latched version of remote loss of signal (LOS)
		0  Local loss of signal(1)
	*/
	union{
		INT32U Data;
		struct
		{
			INT8U SFP_PD	    :1; // bit 0, SFP PD
			INT8U  				:7;	// bit 1 ~ 7 
			INT8U  				:3;	// bit 8 ~ 10 
			INT8U SFP_LD	    :1; // bit 11, SFP LD
			INT8U  				:2;	// bit 12 ~ 13
			INT8U LOS_CPRI		:1; // bit 14, LOS, local
			INT8U LOF_CPRI		:1; // bit 15, LOF, local
		}__attribute__((packed));
	}Mscl_Status;	// 1 번지.

	INT32U LoopBack_Ethernet_St; // Transceiver Loopback and Ethernet Reset
	
	// 2019.05.20
	INT32U MMAP___0x0D;
	INT32U MMAP___0x0B;	

}ST_A_MMAP_Status;



///////////////////////////////////////////////////////////////////////////////////////////////////////

A_PRU_MMAP_PROC_EXT ST_A_MMAP_Status A_MMAP_St[CPRI_PATH];  // ANTS-PRU-MMAP
A_PRU_MMAP_PROC_EXT int Open_A_PRU_Mmap(void);
A_PRU_MMAP_PROC_EXT int Read_A_PRU_Mmap(INT8U cpri, int offset_addr);
A_PRU_MMAP_PROC_EXT int Write_A_PRU_Mmap(INT8U cpri, int offset_addr, int data);
A_PRU_MMAP_PROC_EXT void Close_A_PRU_Mmap(void);
A_PRU_MMAP_PROC_EXT void A_PRU_Ctrl_Check(INT8U Boot_flag);
A_PRU_MMAP_PROC_EXT void Proc_CPRI_Lock(void);

///// YCSO added 190529
A_PRU_MMAP_PROC_EXT INT32U Read_A_PRU_Mmap_CMD(INT32U addr);
A_PRU_MMAP_PROC_EXT INT32U Write_A_PRU_Mmap_CMD(INT32U addr, INT32U data);


/********************************************************************************/
#endif // end of ANTS-PRU
/********************************************************************************/


#endif
/********************************************************************************/



