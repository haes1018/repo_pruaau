/////////////////////////////
//	A_PRU_MMAP_PROC.C
/////////////////////////////
#define A_PRU_MMAP_PROC_GLOBALS
#include "includes.h"
/////////////////////////////


//---------------------------------------------------------------------------------------------------
#if defined(__MAKER_ANTS__) && defined(__TYPE__PRU__) // ANTS-PRU	
//---------------------------------------------------------------------------------------------------

#define MAP__CPRI_SIZE 		0x010000UL

#define BASE_CPRI__Main__ 	0x00B0010000
#define BASE_CPRI_Alpha__ 	0x00B0020000
#define BASE_CPRI__Main_C 	0x00B0050000
#define BASE_CPRI__Apha_C 	0x00B0060000

////// YCSO added 190604
#define CHECK_BASE_CPRI		0x00B00F0000
#define CHECK_OFFSET_CPRI	0x000000FFFF


static int ants_pru_mamp_fd = -1;
static void *map_base[CPRI_PATH] = {0,0,0,0};
//---------------------------------------------------------------------------------------------------


static unsigned long offset_cpri[CPRI_PATH] = 
{
	BASE_CPRI__Main__, // CRPI_0___Main__
	BASE_CPRI_Alpha__, // CRPI_1__Alpha__
	BASE_CPRI__Main_C, // CRPI_2___Main_C
	BASE_CPRI__Apha_C  // CRPI_3__Alpha_C
};

int Open_A_PRU_Mmap(void)
{
	int i;
	
	ants_pru_mamp_fd = open(ANTS_PRU_MMAP_DEV, O_RDWR | O_SYNC);

	printf("[RCU] PRU, %s opened (map_size = 0x%X)", ANTS_PRU_MMAP_DEV, (INT32U)MAP__CPRI_SIZE); 
	fflush(stdout);

	if(ants_pru_mamp_fd > 0)
	{
		for(i = 0; i < CPRI_PATH; i++)		
		{
		 	map_base[i] = mmap( NULL, 
						MAP__CPRI_SIZE,
						PROT_READ | PROT_WRITE, MAP_SHARED, 
						ants_pru_mamp_fd, 
						offset_cpri[i]);
		
			if(map_base[i] == MAP_FAILED)
			{
				D_Printf( DEBUG_SYSTEM, "[Open_A_PRU_Mmap] map_base[%d] fail.", i);
				printf("[Open_A_PRU_Mmap] map_base[%d] fail.", i);
			}
			else
			{
				D_Printf( DEBUG_SYSTEM, "[Open_A_PRU_Mmap] map_base[%d] 0x%08x\n", i, map_base[i]);
				printf("[Open_A_PRU_Mmap] map_base[%d] 0x%p\n", i, map_base[i]);
			}
		}
	}
	
	D_Printf( DEBUG_SYSTEM, "[Open_A_PRU_Mmap] fd 0x%08x\n", ants_pru_mamp_fd);
	printf("[Open_A_PRU_Mmap] fd 0x%08x\n", ants_pru_mamp_fd);

	return ants_pru_mamp_fd;
}

int Read_A_PRU_Mmap(INT8U cpri, int offset_addr)
{
	int read_result;
	void *virt_addr;

	if(map_base[cpri] == MAP_FAILED) return 0;

	virt_addr = (char*)map_base[cpri] + (offset_addr & (MAP__CPRI_SIZE-1)); // MAP_MASK
	read_result = *((unsigned int *)virt_addr);

	//D_Printf( DEBUG_SYSTEM, "[Read_A_PRU_Mmap] addr 0x%08x read_result 0x%08x ", addr, read_result);

	return read_result;
}

int Write_A_PRU_Mmap(INT8U cpri, int offset_addr, int data)
{
	void *virt_addr;
	int read_result, rst = 0;


	if(map_base[cpri] == MAP_FAILED) return 0;

	virt_addr = (char*)map_base[cpri] + (offset_addr & (MAP__CPRI_SIZE-1)); // MAP_MASK

	*((unsigned int *)virt_addr) = data;
	read_result = *((unsigned int *)virt_addr);

	if(data == read_result) rst = 1;

	D_Printf( DEBUG_SYSTEM, "[Write_A_PRU_Mmap] addr 0x%08x, write 0x%08x, read 0x%08x, rslt 0x%08x\n", 
		offset_addr, data, read_result, rst);

	return rst;
}

///// YCSO added 190529

INT32U Read_A_PRU_Mmap_CMD(INT32U addr)
{
	INT8U cpri_num = 0;
	int offset = 0;

	switch( addr & CHECK_BASE_CPRI )
	{
		case BASE_CPRI__Main__:
			cpri_num = 0;
			break;

		case BASE_CPRI_Alpha__:
			cpri_num = 1;
			break;

		case BASE_CPRI__Main_C:
			cpri_num = 2;
			break;

		case BASE_CPRI__Apha_C:
			cpri_num = 3;
			break;

		default:
			return 0;
			break;
	}
	
	offset =(int)( addr & CHECK_OFFSET_CPRI);

//	printf("_YCSO_ cpri_num = %d, addr = %x , offset = %x\n", cpri_num, addr,offset);

	return (INT32U)Read_A_PRU_Mmap(cpri_num , offset*4);

}

INT32U Write_A_PRU_Mmap_CMD(INT32U addr, INT32U data)
{
	INT8U cpri_num = 0;
	int offset = 0;

	switch( addr & CHECK_BASE_CPRI )
	{
		case BASE_CPRI__Main__:
			cpri_num = 0;
			break;

		case BASE_CPRI_Alpha__:
			cpri_num = 1;
			break;

		case BASE_CPRI__Main_C:
			cpri_num = 2;
			break;

		case BASE_CPRI__Apha_C:
			cpri_num = 3;
			break;

		default:
			return 0;
	}
	
	offset =(int)( addr & CHECK_OFFSET_CPRI);

	return (INT32U)Write_A_PRU_Mmap(cpri_num , offset*4, (int)data);

}

////// YCSO added END

void Close_A_PRU_Mmap(void)
{
	if(ants_pru_mamp_fd > 0)
	{
		munmap(map_base[0], MAP__CPRI_SIZE);
		munmap(map_base[1], MAP__CPRI_SIZE);
		munmap(map_base[2], MAP__CPRI_SIZE);
		munmap(map_base[3], MAP__CPRI_SIZE);
		
		close(ants_pru_mamp_fd);
	}
}








static int cpri_lock_reset_req[CPRI_PATH] = {0,0,0,0};

void Proc_CPRI_Lock(void)
{
	INT8U i;
	INT8U curr;
	INT8U prev;	
	static INT8U lock_st_prev = 0; // 0 is unlock
	
	for(i = 0; i < CPRI_PATH; i++)
	{
		//------------------------------------		
		curr = m_Env.CPRI_Lock_St & cbits[i];
		prev = lock_st_prev & cbits[i];
		if(curr != prev)
		{
			if(curr > 0) // if locked
				cpri_lock_reset_req[i] = 2; 
		}
	}
	lock_st_prev = m_Env.CPRI_Lock_St;	
	//------------------------------------		
	for(i = 0; i < CPRI_PATH; i++)
	{
		if(cpri_lock_reset_req[i] == 0) 
			continue;

		cpri_lock_reset_req[i]--;	
		
		Write_A_PRU_Mmap(i, MMAP____LoopBack, 0x03);
		print_agent( DEBUG_SYSTEM, VT_GREEN, "[CPRI%d], Write LoopBack", i);

		/*
		#ifdef  __CPRI_SET_When_Reboot__ // ANTS-PRU	
		if((i == CPRI_2)&&(i == CPRI_3))
		{
			Write_A_PRU_Mmap(i, MMAP____SW_Reset, 0x80000000);
		}
		#endif
		*/
	}	
}
//---------------------------------------------------------------------------------------------------





//---------------------------------------------------------------------------------------------------
#endif // End of ANTS-PRU
//---------------------------------------------------------------------------------------------------


/* EOF */



