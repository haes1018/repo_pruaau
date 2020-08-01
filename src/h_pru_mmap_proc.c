/////////////////////////////
//	H_PRU_MMAP_PROC.C
/////////////////////////////
#define H_PRU_MMAP_PROC_GLOBALS
#include "includes.h"
/////////////////////////////

//---------------------------------------------------------------------------------------------------
int Read_H_PRU_Mmap(int addr)
{
	int fd;
	void *ptr;
	unsigned page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	int value = -1;

	if(addr == -1)
	{
		printf("%s(%d) invalid word address\n", __FUNCTION__, __LINE__);
	}
	else
	{
		fd=open("/dev/mem",O_RDONLY);
		if(fd<1) {
			perror(__FUNCTION__);
		}
		else
		{
			page_addr=(addr & ~(page_size-1));
			page_offset=addr-page_addr;

			ptr=mmap(NULL,page_size,PROT_READ,MAP_SHARED,fd,(addr & ~(page_size-1)));
			if(ptr==NULL) {
				printf("%s(%d) mmap failed\n", __FUNCTION__, __LINE__);
			}
			else
			{
				value = *((unsigned *)(ptr+page_offset));
				/* printf("%s(%d) 0x%08x = 0x%08x, \n", __FUNCTION__, __LINE__, addr, value); */
				munmap(ptr, page_size);
			}

			close(fd);
		}
	}
	return value;
}

int Write_H_PRU_Mmap(int addr, int data)
{
	int fd;
	void *ptr;
	unsigned page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);

	if(addr == -1)
	{
		printf("%s(%d) invalid word address\n", __FUNCTION__, __LINE__);
	}
	else
	{
		fd=open("/dev/mem",O_RDWR);
		if(fd<1) {
			perror(__FUNCTION__);
		}
		else
		{
			page_addr=(addr & ~(page_size-1));
			page_offset=addr-page_addr;

			ptr=mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(addr & ~(page_size-1)));
			if(ptr==NULL) {
				printf("%s(%d) mmap failed\n", __FUNCTION__, __LINE__);
			}
			else
			{
				*((unsigned *)(ptr+page_offset)) = data;
				/* printf("%s(%d) 0x%08x = 0x%08x, \n", __FUNCTION__, __LINE__, value, addr); */
				munmap(ptr, page_size);
			}

			close(fd);
		}
	}
	return data;
}

//---------------------------------------------------------------------------------------------------

void H_PRU_Status_Update(void)
{

	H_PRU_MMAP_Status.IO_Sts.Data = Read_H_PRU_Mmap(DTU_INTF_BASE+DTU_STATUS_OFFSET);

	
	D_Printf( DEBUG_SYSTEM, "[H_Status_Update_Mmap] [%08x] PAU0 Insert %02x, PAU2 Insert %02x, PAU3 Insert %02x, PAU4 Insert %02x\n", 
		H_PRU_MMAP_Status.IO_Sts.Data,
		H_PRU_MMAP_Status.IO_Sts.PAU0_Ins, H_PRU_MMAP_Status.IO_Sts.PAU1_Ins,
		H_PRU_MMAP_Status.IO_Sts.PAU2_Ins, H_PRU_MMAP_Status.IO_Sts.PAU3_Ins
	);

	D_Printf( DEBUG_SYSTEM, "[H_Status_Update_Mmap] PSU_Reset_H %02x, PSU_Reset_L %02x, PSU_Fail %02x\n", 
		H_PRU_MMAP_Status.IO_Sts.PSU_Reset_H, H_PRU_MMAP_Status.IO_Sts.PSU_Reset_L, H_PRU_MMAP_Status.IO_Sts.PSU_Fail
	);

	D_Printf( DEBUG_SYSTEM, "[H_Status_Update_Mmap] Door_Open %02x, Op_Sw_Sts0 %02x, Op_Sw_Sts1 %02x\n", 
		H_PRU_MMAP_Status.IO_Sts.Door_Open, H_PRU_MMAP_Status.IO_Sts.Op_Sw_Sts0, H_PRU_MMAP_Status.IO_Sts.Op_Sw_Sts0
	);

	


}


//---------------------------------------------------------------------------------------------------
#if defined(__MAKER__HFR__) && defined(__TYPE__PRU__) // HFR-PRU	
void* H_PRU_Rfu_RW_Task(void *data)
{	
	Thread_ARG_Var* p_th_arg;	
	INT8U task_path;


	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	task_path = (INT8U)p_th_arg->Arg_v;



	//D_Printf( DEBUG_LS_Comm, "[RCU] H_PRU_Rfu_RW_Task RUN %d\n", task_path );
	OSTimeDly( OS_DELAY_1msec * 3 );


    while (1)
	{	


		H_PRU_Status_Update();

	
		OSTimeDly( OS_DELAY_1sec );

		//D_Printf( DEBUG_LS_Comm, "LSH comm Task Recv Run\n");
	}
	pthread_exit(NULL);
}
#endif
//---------------------------------------------------------------------------------------------------


/* EOF */


