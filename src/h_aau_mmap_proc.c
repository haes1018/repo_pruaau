/////////////////////////////
//	H_AAU_MMAP_PROC.C
/////////////////////////////
#define H_AAU_MMAP_PROC_GLOBALS
#include "includes.h"
/////////////////////////////

//---------------------------------------------------------------------------------------------------
#ifdef __MAKER__HFR__ 
//---------------------------------------------------------------------------------------------------

int hfr_aau_mamp_fd = -1;
static void *map_base;


//---------------------------------------------------------------------------------------------------

int Open_H_AAU_Mmap(void)
{
	int map_size = MAP_SIZE_RF;


	hfr_aau_mamp_fd = open("/dev/uio2", O_RDWR | O_SYNC);


	printf("/dev/%s opened (map_size = 0x%x.\n", "uio2", map_size); 
	fflush(stdout);


 	map_base = mmap(NULL, 
				map_size /*MAP_SIZE*/, 
				PROT_READ | PROT_WRITE, MAP_SHARED, 
				hfr_aau_mamp_fd, 
				0 /*target & ~(off_t)MAP_MASK*/);
	
	//if (map_base == MAP_FAILED) FATAL; 

	D_Printf( DEBUG_SYSTEM, "[Open_H_AAU_Mmap] hfr_aau_mamp_fd 0x%08x, mapbase 0x%08x\n", hfr_aau_mamp_fd, map_base);


	return hfr_aau_mamp_fd;
}

int Read_H_AAU_Mmap(int addr)
{
	int read_result;
	void *virt_addr;

	virt_addr = (char*)map_base + (addr & (MAP_SIZE_RF-1)/*MAP_MASK*/);
	read_result = *((unsigned int *) virt_addr);


	//D_Printf( DEBUG_SYSTEM, "[Read_H_AAU_Mmap] addr 0x%08x read_result 0x%08x \n", addr, read_result);

	return read_result;
}

int Write_H_AAU_Mmap(int addr, int data)
{
	void *virt_addr;
	int map_size, read_result, rst = 0;

	//D_Printf( DEBUG_SYSTEM, "[Write_H_AAU_Mmap] addr 0x%08x, data 0x%08x \n", 
	//	addr, data);

	virt_addr = (char*)map_base + (addr & (MAP_SIZE_RF-1)/*MAP_MASK*/);

	*((unsigned int *) virt_addr) = data;
	read_result = *((unsigned int *) virt_addr);

	if(data == read_result) rst = 1;

	D_Printf( DEBUG_SYSTEM, "[Write_H_AAU_Mmap] addr 0x%08x write data 0x%08x read_result 0x%08x, rst  0x%08x\n", addr, data, read_result, rst);

	return rst;
}

void Close_H_AAU_Mmap(void)
{
	munmap(map_base, MAP_SIZE_RF /*MAP_SIZE*/);
	close(hfr_aau_mamp_fd);

}

//---------------------------------------------------------------------------------------------------

void H_AAU_TDD_Mode_Check(void)
{
	INT8U tmp = 0;
	
	switch( m_St.d_RW_TDD_Mode )
	{
		// Status
		case 0x00:				//normal

			if((H_MMAP_Status.DL_En.Mode != 0) 
				|| (H_MMAP_Status.UL_LNA_En.Mode != 0)
				|| (H_MMAP_Status.UL_SW.Data != 0))
			{
				H_MMAP_Status.DL_En.Mode = 0;
				Write_H_AAU_Mmap(MMAP_DL_AMP_Ctrl, H_MMAP_Status.DL_En.Data);

				H_MMAP_Status.UL_LNA_En.Mode = 0;
				Write_H_AAU_Mmap(MMAP_UL_LNA_Ctrl, H_MMAP_Status.UL_LNA_En.Data);


				H_MMAP_Status.UL_SW.Data = 0;
				Write_H_AAU_Mmap(MMAP_UL_SW, H_MMAP_Status.UL_SW.Data);					
			}
				
			break;

		case 0x01:				//DL

			if((H_MMAP_Status.DL_En.Mode != 1) 
				|| (H_MMAP_Status.UL_LNA_En.Mode != 1)
				|| (H_MMAP_Status.UL_SW.Mode != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_1 != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_2 != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_3 != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_4 != 1)
				|| (H_MMAP_Status.UL_SW.UL_Loop_1 != 1)
				|| (H_MMAP_Status.UL_SW.UL_Loop_2 != 1)
				|| (H_MMAP_Status.UL_SW.UL_Loop_3 != 1)
				|| (H_MMAP_Status.UL_SW.UL_Loop_4 != 1)

				)
			{
				H_MMAP_Status.DL_En.Mode = 1;
				Write_H_AAU_Mmap(MMAP_DL_AMP_Ctrl, H_MMAP_Status.DL_En.Data);

				H_MMAP_Status.UL_LNA_En.Mode = 1;
				Write_H_AAU_Mmap(MMAP_UL_LNA_Ctrl, H_MMAP_Status.UL_LNA_En.Data);

				H_MMAP_Status.UL_SW.Mode = 1;
				H_MMAP_Status.UL_SW.UL_SW_1 = 1;
				H_MMAP_Status.UL_SW.UL_SW_2 = 1;
				H_MMAP_Status.UL_SW.UL_SW_3 = 1;
				H_MMAP_Status.UL_SW.UL_SW_4 = 1;
				H_MMAP_Status.UL_SW.UL_Loop_1 = 1;
				H_MMAP_Status.UL_SW.UL_Loop_2 = 1;
				H_MMAP_Status.UL_SW.UL_Loop_3 = 1;
				H_MMAP_Status.UL_SW.UL_Loop_4 = 1;

				Write_H_AAU_Mmap(MMAP_UL_SW, H_MMAP_Status.UL_SW.Data);					
			}
			break;

		case 0x02:				//UL		//Test   ing

			if((H_MMAP_Status.DL_En.Mode != 1) 
				|| (H_MMAP_Status.UL_LNA_En.Mode != 1)
				|| (H_MMAP_Status.UL_SW.Mode != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_1 != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_2 != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_3 != 1)
				|| (H_MMAP_Status.UL_SW.UL_SW_4 != 1)
				|| (H_MMAP_Status.UL_SW.UL_Loop_1 != 0)
				|| (H_MMAP_Status.UL_SW.UL_Loop_2 != 0)
				|| (H_MMAP_Status.UL_SW.UL_Loop_3 != 0)
				|| (H_MMAP_Status.UL_SW.UL_Loop_4 != 0)

				)
			{
				H_MMAP_Status.DL_En.Mode = 0;
				Write_H_AAU_Mmap(MMAP_DL_AMP_Ctrl, H_MMAP_Status.DL_En.Data);

				H_MMAP_Status.UL_LNA_En.Mode = 1;
				Write_H_AAU_Mmap(MMAP_UL_LNA_Ctrl, H_MMAP_Status.UL_LNA_En.Data);

				H_MMAP_Status.UL_SW.Mode = 0;
				H_MMAP_Status.UL_SW.UL_SW_1 = 0;
				H_MMAP_Status.UL_SW.UL_SW_2 = 0;
				H_MMAP_Status.UL_SW.UL_SW_3 = 0;
				H_MMAP_Status.UL_SW.UL_SW_4 = 0;
				H_MMAP_Status.UL_SW.UL_Loop_1 = 0;
				H_MMAP_Status.UL_SW.UL_Loop_2 = 0;
				H_MMAP_Status.UL_SW.UL_Loop_3 = 0;
				H_MMAP_Status.UL_SW.UL_Loop_4 = 0;

				Write_H_AAU_Mmap(MMAP_UL_SW, H_MMAP_Status.UL_SW.Data);					
			}
		

			break;


	}


}



void H_AAU_Ctrl_Check(INT8U Boot_flag)
{
	INT8U i;
	

	for( i = 0; i < AMPU_4_MAX; i++ )
	{
		//FPGA 1 En 0 Dis <-> GUI 0 En 1 Dis
		//DL EN
		if((Boot_flag) || (BitRead(m_St.Pwr_Amp_En[i], 0) == BitRead(H_MMAP_Status.DL_En.Data, i)))
		{
			if(m_St.Pwr_Amp_En[i])	BitRst(H_MMAP_Status.DL_En.Data, i);
			else					BitSet(H_MMAP_Status.DL_En.Data, i);

		
			D_Printf( DEBUG_SYSTEM, "[H_AAU_Backup_Load_Set] idx %02x / Pwr_Amp_En 0x%02x RVS_Amp_Onoff 0x%08x DL_Att_x2 0x%08x, UL_Att_x2	0x%08x\n", 
				i, m_St.Pwr_Amp_En[i], m_St.RVS_Amp_Onoff[i], m_St.DL_Att[i], m_St.UL_Att[i]);


		
			Write_H_AAU_Mmap(MMAP_DL_AMP_Ctrl, H_MMAP_Status.DL_En.Data);
		}
		
		//UL EN
		if((Boot_flag) || (BitRead(m_St.RVS_Amp_Onoff[i], 0) == BitRead(H_MMAP_Status.UL_LNA_En.Data, i)))
		{
			if(m_St.RVS_Amp_Onoff[i])	BitRst(H_MMAP_Status.UL_LNA_En.Data, i);
			else						BitSet(H_MMAP_Status.UL_LNA_En.Data, i);

		
			Write_H_AAU_Mmap(MMAP_UL_LNA_Ctrl, H_MMAP_Status.UL_LNA_En.Data);
		}

		//Attn
		if((Boot_flag) || (m_St.DL_Att[i] != H_MMAP_Status.DL_Att_x2[i]))
		{

			Write_H_AAU_Mmap(MMAP_DL1_ATT_Data + (i*0x04), m_St.DL_Att[i]);
		}
		if((Boot_flag) || (m_St.UL_Att[i] != H_MMAP_Status.UL_Att_x2[i]))
		{
		
			Write_H_AAU_Mmap(MMAP_UL1_ATT_Data + (i*0x04), m_St.UL_Att[i]);
		}



		
	}


	H_AAU_TDD_Mode_Check();



	if(Boot_flag) Boot_flag = 0;


}
		
void H_AAU_Backup_Load_Set(void)
{
	INT8U i;

	for( i = 0; i < AMPU_4_MAX; i++ )
	{
		m_St.d_RW_TDD_Mode 			= m_Eep.RW_TDD_Mode;

	
		m_St.Pwr_Amp_En[i] 			= m_Eep.Pwr_Amp_En[i];
		m_St.RVS_Amp_Onoff[i] 		= m_Eep.RVS_Amp_Onoff[i];

		
		m_St.DL_Att[i] = m_Eep.DL_Att[i];
		m_St.UL_Att[i] = m_Eep.UL_Att[i];



		H_AAU_Ctrl_Check(1);

		D_Printf( DEBUG_SYSTEM, "[H_AAU_Backup_Load_Set] idx %02x / Pwr_Amp_En 0x%02x RVS_Amp_Onoff 0x%08x DL_Att_x2 0x%08x, UL_Att_x2  0x%08x\n", 
			i, m_St.Pwr_Amp_En[i], m_St.RVS_Amp_Onoff[i], m_St.DL_Att[i], m_St.UL_Att[i]);

	}

}


void H_AAU_Status_Update_Mmap(void)
{
	INT8U i;


	H_MMAP_Status.DL_En.Data			= Read_H_AAU_Mmap(MMAP_DL_AMP_Ctrl);
	H_MMAP_Status.UL_LNA_En.Data		= Read_H_AAU_Mmap(MMAP_UL_LNA_Ctrl);

		

	H_MMAP_Status.DL_Att_x2[AMPU_4_1]	= Read_H_AAU_Mmap(MMAP_DL1_ATT_Data);
	H_MMAP_Status.DL_Att_x2[AMPU_4_2]	= Read_H_AAU_Mmap(MMAP_DL2_ATT_Data);
	H_MMAP_Status.DL_Att_x2[AMPU_4_3]	= Read_H_AAU_Mmap(MMAP_DL3_ATT_Data);
	H_MMAP_Status.DL_Att_x2[AMPU_4_4]	= Read_H_AAU_Mmap(MMAP_DL4_ATT_Data);

	H_MMAP_Status.UL_Att_x2[AMPU_4_1]	= Read_H_AAU_Mmap(MMAP_UL1_ATT_Data);
	H_MMAP_Status.UL_Att_x2[AMPU_4_2]	= Read_H_AAU_Mmap(MMAP_UL2_ATT_Data);
	H_MMAP_Status.UL_Att_x2[AMPU_4_3]	= Read_H_AAU_Mmap(MMAP_UL3_ATT_Data);
	H_MMAP_Status.UL_Att_x2[AMPU_4_4]	= Read_H_AAU_Mmap(MMAP_UL4_ATT_Data);

	H_MMAP_Status.UL_SW.Data			= Read_H_AAU_Mmap(MMAP_UL_SW);



	#if 0
	for( i = 0; i < AMPU_4_MAX; i++ )
	{
		m_St.Pwr_Amp_En[i] 			= BitRead(H_MMAP_Status.DL_En.Data, i);
		m_St.RVS_Amp_Onoff[i]		= BitRead(H_MMAP_Status.UL_LNA_En.Data, i);
	}
	#endif	
	
	H_MMAP_Status.DL_Out_Pwr_x10[DL_0] 	= Read_H_AAU_Mmap(MMAP_DL1_Out_PWR_Det) & 0xfff0;
	H_MMAP_Status.DL_Out_Pwr_x10[DL_1] 	= Read_H_AAU_Mmap(MMAP_DL2_Out_PWR_Det) & 0xfff0;
	H_MMAP_Status.DL_Out_Pwr_x10[DL_2] 	= Read_H_AAU_Mmap(MMAP_DL3_Out_PWR_Det) & 0xfff0;
	H_MMAP_Status.DL_Out_Pwr_x10[DL_3] 	= Read_H_AAU_Mmap(MMAP_DL4_Out_PWR_Det) & 0xfff0;


	#if 0
	D_Printf( DEBUG_LS_Comm, "[H_MMAP_Status] DL_Out_Pwr_x10[DL_0] %d, %d, %f\n", 
		H_MMAP_Status.DL_Out_Pwr_x10[DL_0], (H_MMAP_Status.DL_Out_Pwr_x10[DL_0]*3)/4096, (H_MMAP_Status.DL_Out_Pwr_x10[DL_0]*3)/4096 );
	D_Printf( DEBUG_LS_Comm, "[H_MMAP_Status] DL_Out_Pwr_x10[DL_1] %d, %d, %f\n", 
		H_MMAP_Status.DL_Out_Pwr_x10[DL_1], (H_MMAP_Status.DL_Out_Pwr_x10[DL_1]*3)/4096, (H_MMAP_Status.DL_Out_Pwr_x10[DL_1]*3)/4096 );
	D_Printf( DEBUG_LS_Comm, "[H_MMAP_Status] DL_Out_Pwr_x10[DL_2] %d, %d, %f\n", 
		H_MMAP_Status.DL_Out_Pwr_x10[DL_2], (H_MMAP_Status.DL_Out_Pwr_x10[DL_1]*3)/4096, (H_MMAP_Status.DL_Out_Pwr_x10[DL_0]*3)/4096 );
	D_Printf( DEBUG_LS_Comm, "[H_MMAP_Status] DL_Out_Pwr_x10[DL_3] %d, %d, %f\n", 
		H_MMAP_Status.DL_Out_Pwr_x10[DL_3], (H_MMAP_Status.DL_Out_Pwr_x10[DL_1]*3)/4096, (H_MMAP_Status.DL_Out_Pwr_x10[DL_0]*3)/4096 );
	#endif


	H_AAU_Ctrl_Check(0);

}


//---------------------------------------------------------------------------------------------------


void* H_AAU_Rfu_RW_Task(void *data)
{	
	Thread_ARG_Var* p_th_arg;	
	INT8U task_path;


	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	task_path = (INT8U)p_th_arg->Arg_v;



	//D_Printf( DEBUG_LS_Comm, "[RCU] H_AAU_Rfu_RW_Task RUN %d\n", task_path );

	Open_H_AAU_Mmap();

	OSTimeDly( OS_DELAY_1msec * 3 );

	H_AAU_Backup_Load_Set();


    while (1)
	{	

		H_AAU_Status_Update_Mmap();

		OSTimeDly( OS_DELAY_1sec );

		//D_Printf( DEBUG_LS_Comm, "LSH comm Task Recv Run\n");
	}
	pthread_exit(NULL);
}

//---------------------------------------------------------------------------------------------------
#endif // End of HRF
//---------------------------------------------------------------------------------------------------


/* EOF */


