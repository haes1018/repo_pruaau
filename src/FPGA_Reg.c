/////////////////////////////
// FPGA_REG.C
/////////////////////////////
#define FPGA_REG_GLOBALS
#include "includes.h"
/////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

OS_TIMER Test_timer;
static INT8U Test_timer_En = 0;

OS_TIMER Test_timer_Vss;
//static INT8U Test_timer_Vss_En = 0;

static INT16U task_cnt = 0;


int ants_axi_reg_fd = -1;
#define REGIO_DEVICE  "/dev/zynq_register_io_RCU" // ANTS PRU Register
#define REGIO_CMD     (0x000000FF)

typedef struct 
{	
	unsigned int addr;
	unsigned int data;
} axi_reg_io_t; // ANTS PRU Register

	
int Open_AXI_Reg(void)
{
	////system("insmod /usr/app/sys/register_io_RCU.ko");	
	
	ants_axi_reg_fd = open(REGIO_DEVICE, O_RDWR);
	
	return ants_axi_reg_fd;
}

void Close_AXI_Reg(void)
{
	if(ants_axi_reg_fd > 0)	
		close(ants_axi_reg_fd);
}

static pthread_mutex_t FPGA_RdWr_Sem; // 2019.04.21

void Lock__FPGA_RdWr_Sem(void)
{
	pthread_mutex_lock(&FPGA_RdWr_Sem);
}

void UnLock__FPGA_RdWr_Sem(void)
{
	pthread_mutex_unlock(&FPGA_RdWr_Sem);
}

/////////////////////////////
// Local Variables
/////////////////////////////

usr_cmd_tbl_t usr_cmd_tbl[] =
{
	{ "help"     , "               ", "help messages"          , cmd_help	},
	{ "quit"     , "               ", "quit ..."               , cmd_quit	},
	
	//---- AAU FPGA Register / PRU GPIO Register ------------------------------------------
	{ "regio"    , " address [data]", "read or write reg"      , cmd_regio	},
	//---- AAU FPGA Register / PRU GPIO Register ------------------------------------------

	//------------------------------------------------------------------------------------
	{ NULL, NULL, NULL, NULL},
	/* EOT */
	//------------------------------------------------------------------------------------

	
	//------------------------------------------------------------------------------------
	//{ "ver"      , "                     ",	"version"                , cmd_version	},
	//{ "md"       , "address [size]       ",	"dump memory"            , cmd_memdump	},
	//------------------------------------------------------------------------------------
};



typedef int (*reg_func_t)(int);

enum
{
	reg_rd = 1,
	reg_wr = 2,
	reg_rw = 3,
};

#define ____O____  (0x01)
#define _PRU_REG_  (0x01)
#define _AAU_REG_  (0x02)



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
 
INT32U Rd_AXI_Reg(INT32U addr32)
{
	axi_reg_io_t regio;
	unsigned int re = 0;	

	if (ants_axi_reg_fd < 0) return -1;

	Lock__FPGA_RdWr_Sem();

	addr32 &= 0x3FFFFFFF;
	addr32 |= 0x40000000;

	regio.addr = addr32;
	regio.data = 0;
	
	re = ioctl(ants_axi_reg_fd, REGIO_CMD, (unsigned long)&regio);

	UnLock__FPGA_RdWr_Sem();

	//D_Printf(DEBUG_SYSTEM, "addr=0x%08X, rd data=%p \n ", addr32, re);	
	return re;
}

INT32U Wr_AXI_Reg(INT32U addr32, INT32U data32, char* str)
{
	axi_reg_io_t regio;	
	
	if (ants_axi_reg_fd < 0) return -1;

	Lock__FPGA_RdWr_Sem();

	addr32 &= 0x3FFFFFFF;
	addr32 |= 0x80000000;
	
	regio.addr = addr32;
	regio.data = data32;
		
	ioctl(ants_axi_reg_fd, REGIO_CMD, (unsigned long)&regio);

	UnLock__FPGA_RdWr_Sem();

	//D_Printf(DEBUG_SYSTEM, "addr=0x%08X, wr data=%p \n ", addr32, data32); 

	#ifdef test_20190418	
		D_Printf(DEBUG_ETC, "[RCU] AXI WR : addr=0x%x data=0x%x rd_data=0x%x, %s \n", addr32, data32, regio.data, str);
		printf(VT_CYAN "[RCU] AXI WR : addr=0x%x data=0x%x rd_data=0x%x, %s \n" VT_END_CR, addr32, data32, regio.data, str);
	#endif

	if(addr32 == (Reg_Common_Addr[Reg_Common_RW_Reset]|0x80000000))
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Wr_AXI_Reg] Reg_RW_Reset 0x%X : addr=0x%x data=0x%x str=%s", 
				Reg_Common_RW_Reset, addr32, data32, str);
	}
	/*
	//else
	//{
	//	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[Wr_AXI_Reg] AXI WR : addr=0x%x data=0x%x rd_data=0x%x, str=%s", 
	//			addr32, data32, regio.data, str);
	//}
	*/
	
	return 0;
}
	
///// YCSO added 190529
#ifdef __TYPE__PRU__

INT32U Wr_AXI_Reg_DTU( int fd, INT32U addr32, INT32U data32)
{
	axi_reg_io_t regio;	
	
	if (fd < 0) return -1;

//	Lock__FPGA_RdWr_Sem();

	addr32 &= 0x3FFFFFFF;
	addr32 |= 0x80000000;
	
	regio.addr = addr32;
	regio.data = data32;
		
	ioctl(fd, 0x0000FF00, (unsigned long)&regio);

//	UnLock__FPGA_RdWr_Sem();

	//D_Printf(DEBUG_SYSTEM, "addr=0x%08X, wr data=%p \n ", addr32, data32); 

	return 0;

}

INT32U Rd_AXI_Reg_DTU(int fd, INT32U addr32)
{
	axi_reg_io_t regio;
	unsigned int re = 0;	

	if (fd < 0) return -1;

//	Lock__FPGA_RdWr_Sem();

	addr32 &= 0x3FFFFFFF;
	addr32 |= 0x40000000;

	regio.addr = addr32;
	regio.data = 0;
	
	re = ioctl(fd, 0x0000FF00, (unsigned long)&regio);

//	UnLock__FPGA_RdWr_Sem();

	//D_Printf(DEBUG_SYSTEM, "addr=0x%08X, rd data=%p \n ", addr32, re);	
	return re;
}

#endif

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

static int _get_max_cmd_len()
{
	int i;
	int len;

	len = 0;
	for(i=0; usr_cmd_tbl[i].cmd != NULL; i++)
	{
		if (len < strlen(usr_cmd_tbl[i].cmd))
			len = strlen(usr_cmd_tbl[i].cmd);
	}

	return len;
}

static int _get_max_opt_len()
{
	int i;
	int len;

	len = 0;
	for(i=0; usr_cmd_tbl[i].cmd != NULL; i++)
	{
		if (len < strlen(usr_cmd_tbl[i].opts))
			len = strlen(usr_cmd_tbl[i].opts);
	}

	return len;
}

int cmd_help(int argc, char *argv[])
{
	int  i,j;
	int cmd_len, opt_len;

	cmd_len = _get_max_cmd_len();
	opt_len = _get_max_opt_len();

	printf("Usages : \n");
	for(i=0; usr_cmd_tbl[i].cmd != NULL; i++)
	{
		printf("    %s", usr_cmd_tbl[i].cmd);
		for (j=0; j < (cmd_len - strlen(usr_cmd_tbl[i].cmd)); j++)
			putchar(' ');
		printf(" %s", usr_cmd_tbl[i].opts);
		for (j=0; j < (opt_len - strlen(usr_cmd_tbl[i].opts)); j++)
			putchar(' ');
		printf(" : %s\n", usr_cmd_tbl[i].help);
	}

	return 0;
}

int cmd_quit(int argc, char *argv[])
{
	return -1;
}

int cmd_version(int argc, char *argv[])
{
	printf(" %s Version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
/*
//	printf("            Compiled Date : %s\n", __DATE__);
//	printf("                 and Time : %s\n", __TIME__);
*/
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/*
int cmd_memdump(int argc, char *argv[])
{
	unsigned char *addr;
	int size;

	if (argc != 2 && argc != 3)
		return 0;

	addr = (unsigned char *)strtoul(argv[1], NULL, 16);
	if (argc == 3)
		size = strtol(argv[2], NULL, 10);
	else
		size = 32;

	////////////////////////////////////////// test only
	// hex_dump(addr, size);
	printf("[hex_dump] addr=%p size=%d\n", addr, size);
	//////////////////////////////////////////

	return 0;
}
*/


/*
	bwl : 1 (1byte), 2 (2byte), 3 (4byte)
*/

static void _regio_print_results(int rd, int bwl, int addr, int data)
{
	if (rd)
		printf("[regio] read:  ");
	else
		printf("[regio] write: ");

	printf("addr=%08x ", addr);

	if (bwl == 1)
		printf("data=0x%02x\n", data);
	else if (bwl == 2)
		printf("data=0x%04x\n", data);
	else
		printf("data=0x%08x\n", data);
}

/*
-----------------------------------------------------------
	PRU/AAU
	Register Read for RCU-APP
-----------------------------------------------------------	
*/

int cmd_regio(int argc, char *argv[])
{
	int bwl = 4; // default 4B I/O
	int base = 0;
	unsigned int addr;
	unsigned int data;

	if (argc < 2 || argc > 4)
	{
		printf("invalid args ...\n");
		return 0;
	}

	if (argv[1][0] == '-')
	{
		if (argv[1][1] == 'b')
			bwl = 1;
		else if (argv[1][1] == 'w')
			bwl = 2;
		base++;
		argc--;
	}

	if (argc != 2 && argc != 3)
	{
		printf("invalid addr or data ...\n");
		return 0;
	}

	addr = strtoul(argv[base + 1], NULL, 16);
	if (argc == 3) // Write
	{
		data = strtoul(argv[base + 2], NULL, 16);
	#ifdef __TYPE__PRU__
		Wr_AXI_Reg(addr, data, "cmd");
	#else
		Wr_SPI_FPGA(addr, data, "cmd");
	#endif
		_regio_print_results(0, bwl, addr, data);
	}
	else // Read
	{
	#ifdef __TYPE__PRU__
		data = Rd_AXI_Reg(addr);
	#else	
		data = Rd_SPI_FPGA(addr|0x80000000);		
	#endif	
		_regio_print_results(1, bwl, addr, data);
	}
	return 0;
}





//------------------------------------------------------------------
#ifdef _DO_NOT_FOLLOW_VSS_Table_When_TDD_Slot_
static INT8U default_tdd_slot_val[20] = // Test Value
{
	0x00,0x00,0x00,0x20,0x01, 0x00,0x00,0x00,0x20,0x01,
	0x00,0x00,0x00,0x20,0x01, 0x00,0x00,0x00,0x20,0x01
};
#endif

/*
------------------------------------- read/write -----
------------------------------------- general/axi -----
*/
// bit 31 ~ 30
#define r1_rw 0 // bit31(0), bit30(0)
#define r1_rd 0x02 // bit31(1), bit30(0)

/*
------------------------------------- General -----

bit 31 : (0 : wr enable), (1 : wr disable)
bit 30 : (0 : rd enable), (1 : rd disable)
bit 29 : 0 mode_general fix   ------- General -----
bit 28~25 : 
	0 : Common : bit24~21 (0~4 : common, cpri #0 ~ #3)
	1 : DSP    : bit24~21 (0~2 : common, FA #0 ~ #1)
	
	2 : VSS    : bit24~21 
	            0 ~ 1 : cpri dnward #0 ~ #1
	            2 ~ 3 : cpri upward #0 ~ #1
	            4 ~ 5 : cpri dnward #2 ~ #3
	            6 ~ 7 : cpri upward #2 ~ #3
	            
	3 : TDD  : bit24~21 (0 : Common)
-----------------------------------------------
bit 20~17 : MIMO
	0 : Common
	1 ~ 4 : MIMO #0 ~ #3
-----------------------------------------------
*/

//------------------
#define r2_gen 0	// bit 29, General
#define r2_axi 1	// bit 29, General

//------------------
#define r3g_comm 0 // bit 28 ~ 25 @ general
#define r3g__dsp 1 // bit 28 ~ 25 @ general
#define r3g__vss 2 // bit 28 ~ 25 @ general
#define r3g__tdd 3 // bit 28 ~ 25 @ general

#define r3g__rfu 4 // bit 28 ~ 25 @ general, AAU Only
#define r3g__alc 5 // bit 28 ~ 25 @ general, AAU Only
#define r3g_comp 6 // bit 28 ~ 25 @ general, compression

#define r3g__cfr 10 // bit 28 ~ 25 @ general, cfr
#define r3g__dpd 11 // bit 28 ~ 25 @ general, dpd

#define r3g__cpri 1 // bit 28 ~ 25 @ general


//------------------
enum // bit24~21 
{
	r4g_cpri_c = 0, // cpri common
	r4g_cpri_0,
	r4g_cpri_1,
	r4g_cpri_2,
	r4g_cpri_3,

	r4g_cpri_max
};
enum // bit24~21 
{
	r4g_dsp_comm = 0,
	r4g_dsp_fa_0,
	r4g_dsp_fa_1,

	r4g_dsp__max
};

enum // bit24~21 
{
	r4g_cpri_dn_0 = 0,
	r4g_cpri_dn_1,
	r4g_cpri_up_0,
	r4g_cpri_up_1,
	r4g_cpri_dn_2,
	r4g_cpri_dn_3,
	r4g_cpri_up_2,
	r4g_cpri_up_3,
};
enum // bit24~21 
{
	r4g_tdd_comm = 0,

	r4g_tdd_max
};

enum // bit24~21 , AAU : RFU_Ctrl, ALC
{
	r4g_aau_comm = 0,

	r4g_aau_max
};


enum // bit20~17 
{
	r5g_common = 0,
	r5g_mimo_0,
	r5g_mimo_1,
	r5g_mimo_2,
	r5g_mimo_3,

	r5g_mimo_max
};


/*
------------------------------------- Axi -----
bit 31 : R/W
bit 30 : 0 fix
bit 29 : 1 mode_axi fix  ------------ Axi -----
bit 28~25 : 
	1 : CPRI : bit24~21 (0~3 : cpri #0 ~ #3)
	2 : DPD  : bit24~21 (0~3 : dpd	#0 ~ #3)
	3 : CFR  : bit24~21 (0~3 : cfr	#0 ~ #3)
	4 : JESD : bit24~21 (0~7 : jesd #0 ~ #7)
-----------------------------------------------
*/

//------------------
#define r2_axi 1  // bit 29
//------------------
#define r3a_cpri 1 // bit 28 ~ 25 @ axi
#define r3a__dpd 2 // bit 28 ~ 25 @ axi
#define r3a__cfr 3 // bit 28 ~ 25 @ axi
#define r3a_jsed 4 // bit 28 ~ 25 @ axi
//------------------
enum // bit24~21 
{
	r4a_cpri_0 = 0,
	r4a_cpri_1,
	r4a_cpri_2,
	r4a_cpri_3,

	r4a_cpri_max
};
enum // bit24~21 
{
	r4a_dpd_0 = 0,
	r4a_dpd_1,
	r4a_dpd_2,
	r4a_dpd_3,

	r4a_dpd_max
};
enum // bit24~21 
{
	r4a_cfr_0 = 0,
	r4a_cfr_1,
	r4a_cfr_2,
	r4a_cfr_3,
	
	r4a_cfr_max
};
enum // bit24~21 
{
	r4a_jesd_0 = 0,
	r4a_jesd_1,
	r4a_jesd_2,
	r4a_jesd_3,

	r4a_jesd_max
};
//------------------------------------------------------------------






/*
-------------------------------------------------------
	Make
	Default (without R/W, burst)
	PRU/AAU FPGA Register Address
-------------------------------------------------------
*/

INT32U Make_Reg_Addr(INT8U mode, INT8U func, INT8U fa_port, INT8U mimo, INT8U addr8)
{
	REG_ADDR reg_addr;
	static int cnt=0;
	
	reg_addr.rdwr = 0; // default 0
	reg_addr.mode = mode; // bit 29
	reg_addr.func = func; // bit 28 ~ 25
	reg_addr.fa_port = fa_port; // bit 24 ~ 21
	reg_addr.mimo = mimo; // bit 20 ~ 17
	reg_addr.addr8 = addr8; // bit 16 ~ 9
	reg_addr.reserved = 0;  // bit 8 ~ 0

	cnt++;

	#if 0
	{
	// debug print -------------------------------------------------------------------------------
	D_Printf(DEBUG_SYSTEM, "[RCU %4d] 0x%08X : rw %d, mode %d, func 0x%2X, fa 0x%2X, mimo 0x%2X, addr 0x%03X, 0x%04X\n", 
		cnt,
		reg_addr.Data,
		reg_addr.rdwr,
		reg_addr.mode,
		reg_addr.func,
		reg_addr.fa_port,
		reg_addr.mimo,
		reg_addr.addr8,
		reg_addr.reserved);	
	}
	// debug print -------------------------------------------------------------------------------
	#endif

	return reg_addr.Data;
}

INT32U Make_Reg_Addr_AAU_Axi(INT8U mode, INT8U func, INT8U fa_port, INT8U mimo, INT8U addr8, INT8U addr0)
{
	REG_ADDR reg_addr;
	static int cnt=0;
	
	reg_addr.rdwr = 0; // default 0
	reg_addr.mode = mode; // bit 29
	reg_addr.func = func; // bit 28 ~ 25
	reg_addr.fa_port = fa_port; // bit 24 ~ 21
	reg_addr.mimo = mimo; // bit 20 ~ 17
	reg_addr.addr8 = addr8; // bit 16 ~ 9
	reg_addr.reserved = addr0;  // bit 8 ~ 0

	cnt++;

	#if 0
	// debug print -------------------------------------------------------------------------------
	D_Printf(DEBUG_SYSTEM, "[RCU %4d] 0x%08X : rw %d, mode %d, func 0x%2X, fa 0x%2X, mimo 0x%2X, addr 0x%03X, 0x%04X\n", 
		cnt,
		reg_addr.Data,
		reg_addr.rdwr,
		reg_addr.mode,
		reg_addr.func,
		reg_addr.fa_port,
		reg_addr.mimo,
		reg_addr.addr8,
		reg_addr.reserved);	
	// debug print -------------------------------------------------------------------------------
	#endif

	return reg_addr.Data;
}

/*
-------------------------------------------------------
	Calculate 
	of
	PRU/AAU FPGA Register Address
-------------------------------------------------------
*/
void Calculate_Reg_Address(void)
{
	int r, fa, m, c;	

	//------------------------------------------	
	for(r = 0; r < Reg_Common_MAX; r++)
	{
		Reg_Common_Addr[r] = Make_Reg_Addr(r2_gen, r3g_comm, r4g_cpri_c, r5g_common, r);
		Reg_Common_flag[r] = 0;
	}
	//------------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r = 0; r < Reg_Common_MIMO_MAX; r++)
		{
			Reg_Common_MIMO_Addr[c][r] = Make_Reg_Addr(r2_gen, r3g_comm, c+1, r5g_common, r);	
			Reg_Common_MIMO_flag[c][r] = 0;
		}
	}
	//------------------------------------------
	for(r = 0; r < Reg_DSP_Common_Max; r++)
	{
		Reg_DSP_Common_Addr[r] = Make_Reg_Addr(r2_gen, r3g__dsp, r4g_dsp_comm, r5g_common, r); // check
		Reg_DSP_Common_flag[r] = 0;
	}
	//------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_DSP_Common_MIMO_MAX; r++)
		{
			// 2019.04.08 : r5g_common -> r5g_mimo_0
			Reg_DSP_Common_MIMO_Addr[m][r] = Make_Reg_Addr(r2_gen, r3g__dsp, r4g_dsp_comm, m+r5g_mimo_0, r); // check
			Reg_DSP_Common_MIMO_flag[m][r] = 0;
		}
	}
	//------------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r = 0; r < Reg_DSP_FA_Common_MAX; r++)
		{
			Reg_DSP_FA_Common_Addr[fa][r] = Make_Reg_Addr(r2_gen, r3g__dsp, fa+r4g_dsp_fa_0, r5g_common, r);
			Reg_DSP_FA_Common_flag[fa][r] = 0;
		}
	}
	//------------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			for(r = 0; r < Reg_DSP_FA_MIMO_MAX; r++)
			{
				Reg_DSP_FA_MIMO_Addr[fa][m][r] = Make_Reg_Addr(r2_gen, r3g__dsp, fa+r4g_dsp_fa_0, m+r5g_mimo_0, r);
				Reg_DSP_FA_MIMO_flag[fa][m][r] = 0;
			}
		}
	}
	//------------------------------------------	
	for(r = 0; r < Reg_VSS_DL_MAX; r++)
	{
		Reg_VSS_DL_Addr[CPRI_0][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_dn_0, r5g_common, r);	// register 0
		Reg_VSS_DL_Addr[CPRI_1][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_dn_1, r5g_common, r);	// register 1	
		Reg_VSS_DL_Addr[CPRI_2][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_dn_2, r5g_common, r);	// register 4	
		Reg_VSS_DL_Addr[CPRI_3][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_dn_3, r5g_common, r);	// register 5	

		Reg_VSS_DL_flag[CPRI_0][r] = 0;	
		Reg_VSS_DL_flag[CPRI_1][r] = 0;
		Reg_VSS_DL_flag[CPRI_2][r] = 0;	
		Reg_VSS_DL_flag[CPRI_3][r] = 0;
	}
	//------------------------------------------	
	for(r = 0; r < Reg_VSS_UL_MAX; r++)
	{
		Reg_VSS_UL_Addr[CPRI_0][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_up_0, r5g_common, r);	// register 2	
		Reg_VSS_UL_Addr[CPRI_1][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_up_1, r5g_common, r);	// register 3	
		Reg_VSS_UL_Addr[CPRI_2][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_up_2, r5g_common, r);	// register 6	
		Reg_VSS_UL_Addr[CPRI_3][r] = Make_Reg_Addr(r2_gen, r3g__vss, r4g_cpri_up_3, r5g_common, r);	// register 7	

		Reg_VSS_UL_flag[CPRI_0][r] = 0;	
		Reg_VSS_UL_flag[CPRI_1][r] = 0;
		Reg_VSS_UL_flag[CPRI_2][r] = 0;	
		Reg_VSS_UL_flag[CPRI_3][r] = 0;
	}
	//------------------------------------------
	for(r = 0; r < Reg_TDD_MAX; r++)
	{
		Reg_TDD_Addr[r] = Make_Reg_Addr(r2_gen, r3g__tdd, r4g_tdd_comm, r5g_common, r);
		Reg_TDD_flag[r] = 0;	
	}
	//------------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r = 0; r < Reg_TDD2_MAX; r++)
		{
			Reg_TDD2_Addr[fa][r] = Make_Reg_Addr(r2_gen, r3g__tdd, fa+1, r5g_common, r);
			Reg_TDD2_flag[fa][r] = 0;	
		}
	}
	//------------------------------------------	
	for(m = 0; m < MAX_MIMO_PATH; m++)		
	{
		for(r = 0; r < Reg_RFU_CTRL_MAX; r++)
		{
			Reg_RFU_CTRL_Addr[m][r] = Make_Reg_Addr(r2_gen, r3g__rfu, r4g_aau_comm, m+1, r);
			Reg_RFU_CTRL_flag[m][r] = 0;
		}
			
		for(r = 0; r < Reg_ALC_MAX; r++)
		{
			Reg_ALC_Addr[m][r] = Make_Reg_Addr(r2_gen, r3g__alc, r4g_aau_comm, m+1, r);
			Reg_ALC_flag[m][r] = 0;
		}
	}
	//------------------------------------------	// add 0218
	for(m = 0; m < CPRI_PATH; m++)
	{
		for(r = 0; r < Reg_AXI_CPRI_MAX; r++)	
		{
			Reg_AXI_CPRI_Addr[m][r] = Make_Reg_Addr_AAU_Axi(r2_axi, r3g__cpri, m, 0, r, 0); // 2019.04.21
			Reg_AXI_CPRI_flag[m][r] = 0;
		}
	}
	//------------------------------------------	
	for(r = 0; r < CPRI_PATH; r++)
	{
		Reg_COMP_ONOFF_Addr[r] = Make_Reg_Addr(r2_gen, r3g_comp, r+1, 0, 0);
		Reg_COMP_ONOFF_flag[r] = 0;
	}
	//------------------------------------------	


//------------------------------------------
#if 1 // def __TYPE__PRU__
//------------------------------------------
	int path;	
	
	for(path = 0; path < MAX_DL; path++)
	{
		for(r = 0; r < Reg_CFR_MAX; r++)
		{
			Reg_CFR_Addr[path][r] = Make_Reg_Addr(r2_gen, r3g__cfr, path, 0, r);
			Reg_CFR_flag[path][r] = 0;
		}

		for(r = 0; r < Reg_DPD_MAX; r++)
		{
			Reg_DPD_Addr[path][r] = Make_Reg_Addr(r2_gen, r3g__dpd, path, 0, r);
			Reg_DPD_flag[path][r] = 0;
		}
	}
	// 2019.05.01
	for(r = 0; r < Reg_DPD2_MAX; r++)
	{
		Reg_DPD2_Addr[r] = Make_Reg_Addr(r2_gen, r3g__dpd, r, 0, 0x0B);
		Reg_DPD2_flag[r] = 0;
	}
//------------------------------------------	
#endif
//------------------------------------------
}






void Disp_Reg_Address(void)
{
#if 1
	int r, fa, m, c;	

	//------------------------------------------	
	for(r = 0; r < Reg_Common_MAX; r++)
	{		
		D_Printf(DEBUG_SYSTEM, "[Reg_Common] (0x%X) 0x%08X\n", 
			r,
			Reg_Common_Addr[r]);	
	}
	//------------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r = 0; r < Reg_Common_MIMO_MAX; r++)
		{			
			D_Printf(DEBUG_SYSTEM, "[Reg_Common_MIMO] %d (0x%X) 0x%08X\n", 
				c, r,
				Reg_Common_MIMO_Addr[c][r]);
		}
	}
	//------------------------------------------
	for(r = 0; r < Reg_DSP_Common_Max; r++)
	{
		D_Printf(DEBUG_SYSTEM, "[Reg_DSP_Common] (0x%X) 0x%08X\n", 
				r,
				Reg_DSP_Common_Addr[r]);
	}
	//------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_DSP_Common_MIMO_MAX; r++)
		{			
			D_Printf(DEBUG_SYSTEM, "[Reg_DSP_Common_MIMO] %d (0x%X) 0x%08X\n", 
				m, r,
				Reg_DSP_Common_MIMO_Addr[m][r]);
		}
	}
	//------------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r = 0; r < Reg_DSP_FA_Common_MAX; r++)
		{
			D_Printf(DEBUG_SYSTEM, "[Reg_DSP_FA_Common] %d (0x%X) 0x%08X\n", 
				fa, r,
				Reg_DSP_FA_Common_Addr[fa][r]);
		}
	}
	//------------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			for(r = 0; r < Reg_DSP_FA_MIMO_MAX; r++)
			{
				D_Printf(DEBUG_SYSTEM, "[Reg_DSP_FA_MIMO] %d %d (0x%X) 0x%08X\n", 
					fa, m, r,
					Reg_DSP_FA_MIMO_Addr[fa][m][r]);
			}
		}
	}
	//------------------------------------------	
	for(r = 0; r < Reg_VSS_DL_MAX; r++)
	{
		D_Printf(DEBUG_SYSTEM, "[Reg_VSS_DL] (0x%X) 0x%08X 0x%08X 0x%08X 0x%08X\n", 
			r,
			Reg_VSS_DL_Addr[CPRI_0][r],
			Reg_VSS_DL_Addr[CPRI_1][r],
			Reg_VSS_DL_Addr[CPRI_2][r],
			Reg_VSS_DL_Addr[CPRI_3][r]);
	}
	//------------------------------------------	
	for(r = 0; r < Reg_VSS_UL_MAX; r++)
	{
		D_Printf(DEBUG_SYSTEM, "[Reg_VSS_UL] (0x%X) 0x%08X 0x%08X 0x%08X 0x%08X\n", 
			r,
			Reg_VSS_UL_Addr[CPRI_0][r],
			Reg_VSS_UL_Addr[CPRI_1][r],
			Reg_VSS_UL_Addr[CPRI_2][r],
			Reg_VSS_UL_Addr[CPRI_3][r]);
	}
	//------------------------------------------
	for(r = 0; r < Reg_TDD_MAX; r++)
	{
		D_Printf(DEBUG_SYSTEM, "[Reg_TDD] (0x%X) 0x%08X\n", 
			r,
			Reg_TDD_Addr[r]);
	}
	//------------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r = 0; r < Reg_TDD2_MAX; r++)
		{
			D_Printf(DEBUG_SYSTEM, "[Reg_TDD2] %d (0x%X) 0x%08X\n", 
				fa, r,
				Reg_TDD2_Addr[fa][r]);
		}
	}
	//------------------------------------------	
	for(m = 0; m < MAX_MIMO_PATH; m++)		
	{
		for(r = 0; r < Reg_RFU_CTRL_MAX; r++)
		{			
			D_Printf(DEBUG_SYSTEM, "[Reg_RFU_CTRL] %d (0x%X) 0x%08X\n", 
				m, r,
				Reg_RFU_CTRL_Addr[m][r]);
		}
	}	
	for(m = 0; m < MAX_MIMO_PATH; m++)		
	{		
		for(r = 0; r < Reg_ALC_MAX; r++)
		{
			D_Printf(DEBUG_SYSTEM, "[Reg_ALC] %d (0x%X) 0x%08X\n", 
				m, r,
				Reg_ALC_Addr[m][r]);
		}
	}
	//------------------------------------------	// add 0218
	for(m = 0; m < CPRI_PATH; m++)
	{
		for(r = 0; r < Reg_AXI_CPRI_MAX; r++)	
		{
			D_Printf(DEBUG_SYSTEM, "[Reg_AXI_CPRI] %d (0x%X) 0x%08X\n", 
				m, r,
				Reg_AXI_CPRI_Addr[m][r]);
		}
	}
	//------------------------------------------	
	for(r = 0; r < CPRI_PATH; r++)
	{		
		D_Printf(DEBUG_SYSTEM, "[Reg_COMP_ONOFF] (0x%X) 0x%08X\n", 
			r,
			Reg_COMP_ONOFF_Addr[r]);
	}
	//------------------------------------------	

	int path;	
	
	for(path = 0; path < MAX_DL; path++)
	{
		for(r = 0; r < Reg_CFR_MAX; r++)
		{
			D_Printf(DEBUG_SYSTEM, "[Reg_CFR] (0x%X) 0x%08X\n", 
				r,
				Reg_CFR_Addr[path][r]);
		}

		for(r = 0; r < Reg_DPD_MAX; r++)
		{
			D_Printf(DEBUG_SYSTEM, "[Reg_DPD] %d (0x%X) 0x%08X\n", 
				path, r,
				Reg_DPD_Addr[path][r]);
		}
	}
	// 2019.05.01
	for(r = 0; r < Reg_DPD2_MAX; r++)
	{
		D_Printf(DEBUG_SYSTEM, "[Reg_DPD2] (0x%X) 0x%08X\n", 
			r,
			Reg_DPD2_Addr[r]);
	}
#endif

//------------------------------------------
}






/*
--------------------------------------------------------------------------------------------------
	Calculate 
	of
	PRU/AAU FPGA Register Variables' pointer

	Read Only Register's address 
	is 
	NULL
--------------------------------------------------------------------------------------------------
*/
void Calculate_Reg_Variable_Pointer(ST_DtuStatus* pDtuReg, int idx)
{
	int fa;
	int c, k, m, r;
	INT8U path;

	if(idx >= ST_CTL) return;

	// init -----------------------------------------------------------	
	for(r=0; r < Reg_Common_MAX; r++) 
		pReg_Common_Addr[r][idx] = NULL;	
	//----------------------------------------------------------------	
	pReg_Common_Addr[Reg_Common_RW_Reset][idx]    = &pDtuReg->RW_Reset.Data;

	pReg_Common_Addr[Reg_Common_RW_ETH_Path_Sel][idx]       = &pDtuReg->RW_ETH_Path_Sel.Data;
	pReg_Common_Addr[Reg_Common_RW_Mapper_Loopback][idx]    = &pDtuReg->RW_Mapper_Loopback.Data;
	pReg_Common_Addr[Reg_Common_RW_RF_Loopback][idx]        = &pDtuReg->RW_RF_Loopback.Data;
	pReg_Common_Addr[Reg_Common_RW_FAN_Control][idx]        = &pDtuReg->RW_FAN_Control.Data;
	pReg_Common_Addr[Reg_Common_RW_Recovery_Clock_Set][idx] = &pDtuReg->RW_Recovery_Clock_Set.Data;
	pReg_Common_Addr[Reg_Common_RW_Gain_Check_Mode][idx]    = &pDtuReg->Gain_Check_Mode.Data;
	
	pReg_Common_Addr[Reg_Common_RW_Interrupt_mask0][idx]   = &pDtuReg->RW_Interrupt_mask0.Data;
	pReg_Common_Addr[Reg_Common_RW_Interrupt_mask1][idx]   = &pDtuReg->RW_Interrupt_mask1.Data;
	pReg_Common_Addr[Reg_Common_RW_Interrupt_status0][idx] = &pDtuReg->RW_Interrupt_Status0.Data;
	pReg_Common_Addr[Reg_Common_RW_Interrupt_status1][idx] = &pDtuReg->RW_Interrupt_Status1.Data;


	pReg_Common_Addr[Reg_Common_RW_r_pattern_add_fa0][idx]  = &pDtuReg->RW_r_pattern_add_fa0.uD32;
	pReg_Common_Addr[Reg_Common_RW_r_pattern_i_fa0][idx]    = &pDtuReg->RW_r_pattern_i_fa0.uD32;
	pReg_Common_Addr[Reg_Common_RW_r_pattern_q_fa0][idx]    = &pDtuReg->RW_r_pattern_q_fa0.uD32;
	pReg_Common_Addr[Reg_Common_RW_r_pattern_wren_fa0][idx] = &pDtuReg->RW_r_pattern_wren_fa0.uD32;
	
	pReg_Common_Addr[Reg_Common_RW_r_pattern_add_fa1][idx]  = &pDtuReg->RW_r_pattern_add_fa1.uD32;
	pReg_Common_Addr[Reg_Common_RW_r_pattern_i_fa1][idx]    = &pDtuReg->RW_r_pattern_i_fa1.uD32;
	pReg_Common_Addr[Reg_Common_RW_r_pattern_q_fa1][idx]    = &pDtuReg->RW_r_pattern_q_fa1.uD32;
	pReg_Common_Addr[Reg_Common_RW_r_pattern_wren_fa1][idx] = &pDtuReg->RW_r_pattern_wren_fa1.uD32;	

	pReg_Common_Addr[Reg_Common_RW_PLL_Addr][idx] = &pDtuReg->RW_PLL_Addr.uD32; 
	pReg_Common_Addr[Reg_Common_RW_PLL_Data][idx] = &pDtuReg->RW_PLL_Data.uD32;		

	pReg_Common_Addr[Reg_Common_R__RE_Reset][idx]          = &pDtuReg->R_RE_Reset.Data;
	pReg_Common_Addr[Reg_Common_RW_RE_Reset_Clr][idx]      = &pDtuReg->RW_RE_Reset_Clr.Data;
	pReg_Common_Addr[Reg_Common_RW_VSS_Table_Debug][idx]   = &pDtuReg->RW_VSS_Tbl_Dbg.Data;	
	pReg_Common_Addr[Reg_Common_RW_PSU_Reset][idx]         = &pDtuReg->PSU_Reset.Data;
	pReg_Common_Addr[Reg_Common_RW_DTU_LED][idx]           = &pDtuReg->RW_DTU_LED.Data;
	pReg_Common_Addr[Reg_Common_AC_ALARM_INFO][idx]        = &pDtuReg->AC_Alarm_info.uD32;

	#ifdef __TYPE__AAU__ // AAU Only
	for(m = 0; m < 8; m++)
		pReg_Common_Addr[Reg_Common_RW_DTU_INV_INFO+m][idx] = &pDtuReg->RW_DTU_Inventory[m].uD32;

	pReg_Common_Addr[Reg_Common_RW_Det_Enable][idx]     = &pDtuReg->RW_Det_Enable.uD32;
	pReg_Common_Addr[Reg_Common_RW_Det_Start_Time][idx] = &pDtuReg->RW_Det_Start_Time.uD32;
	pReg_Common_Addr[Reg_Common_RW_Det_Range_Time][idx] = &pDtuReg->RW_Det_Range_Time.uD32;
	#endif

	
	pReg_Common_Addr[Reg_Common_RW_RE_Reset_ACK_SW][idx] = &pDtuReg->RW_RE_Reset_ACK_SW.Data;
	
//----------------------------------------
//		SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_Common_MIMO_MAX; r++) 
			pReg_Common_MIMO_Addr[c][r][idx] = NULL;	
		//----------------------------------------------------------------		
		pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_TX_Disable][idx] = &pDtuReg->Common_CPRI[c].RW_SFP_TX_DISABLE.Data;
		pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_Reset][idx]      = &pDtuReg->Common_CPRI[c].RW_SFP_Reset.Data;
		
		for(k = 0; k < 8; k++)
			pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_INV_INFO+k][idx] = &pDtuReg->Common_CPRI[c].R_SFP_Inventory[k].uD32;

		for(k = 0; k < 4; k++)
		{
			pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VENDOR_NAME+k][idx] = &pDtuReg->Common_CPRI[c].R_SFP_Vendor_Name[k].uD32;			
			pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VENDOR___PN+k][idx] = &pDtuReg->Common_CPRI[c].R_SFP_Vendor___PN[k].uD32;
			pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VENDOR___SN+k][idx] = &pDtuReg->Common_CPRI[c].R_SFP_Vendor___SN[k].uD32;
		}
		for(k = 0; k < 2; k++)	
			pReg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_DATE___CODE+k][idx] = &pDtuReg->Common_CPRI[c].R_SFP_Date___Code[k].uD32;
	}
//---------------------------------------------------------------------
// DSP-Delay
//---------------------------------------------------------------------
	// init ----------------------------------------------------------	
	for(r=0; r < Reg_DSP_Common_Max; r++) 
		pReg_DSP_Common_Addr[r][idx] = NULL; 
	//----------------------------------------------------------------	

	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_DL_Delay][idx] = &pDtuReg->RW_DL_DELAY.uD32;
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_Delay][idx] = &pDtuReg->RW_UL_DELAY.uD32;

	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_4T2R_Mode][idx] = &pDtuReg->RW_4T2R_Mode.Data;

	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_MUX][idx]     = &pDtuReg->RW_FFT_Mux.Data;	
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_Trigger][idx] = &pDtuReg->RW_FFT_Trigger.Data;	
	pReg_DSP_Common_Addr[Reg_DSP_Common_R_FFT_Done][idx]   	 = &pDtuReg->R_FFT_Done.Data;	
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_Addr][idx]    = &pDtuReg->RW_FFT_Address.Data;
	pReg_DSP_Common_Addr[Reg_DSP_Common_R_FFT_Result][idx]   = &pDtuReg->RW_FFT_Result.uD32;

#ifdef __TYPE__PRU__ // PRU Only, 2019.06.01
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_DL_Time][idx]   = &pDtuReg->RW_Capture_DL_Time.uD32;
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_UL_Time][idx]   = &pDtuReg->RW_Capture_UL_Time.uD32;
	
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_ULNoise_Time][idx]    = &pDtuReg->RW_Capture_ULNoise_Time.uD32;
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_Noise_RSSI_StartTime][idx] = &pDtuReg->RW_UL_Noise_RSSI_StartTime.uD32;
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_Noise_RSSI_StopTime][idx]  = &pDtuReg->RW_UL_Noise_RSSI_StopTime.uD32;
	
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_RSSI_StartTime][idx]  = &pDtuReg->RW_UL_RSSI_StartTime.uD32;
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_RSSI_StopTime][idx]   = &pDtuReg->RW_UL_RSSI_StopTime.uD32;
	pReg_DSP_Common_Addr[Reg_DSP_Common_RW_SG_Flat][idx]            = &pDtuReg->RW_SG_Flat.Data;
#endif	
//---------------------------------------------------------------------
// DSP-Common-MIMO
//---------------------------------------------------------------------
	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_DSP_Common_MIMO_MAX; r++) 
			pReg_DSP_Common_MIMO_Addr[m][r][idx] = NULL; 
		//----------------------------------------------------------------			

		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_Threshold][idx]= &pDtuReg->DSP_Common_MIMO[m].HC_Threshold.Data;
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_HC_CLR][idx]   = &pDtuReg->DSP_Common_MIMO[m].HC_Clr.Data;	
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_R_HC_COUNT][idx]  = &pDtuReg->DSP_Common_MIMO[m].HC_Count.Data;
	#ifdef __TYPE__AAU__ // AAU Only
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_CFR_OUT_GAIN][idx]= &pDtuReg->DSP_Common_MIMO[m].CFR_Out_Gain.Data;
	#else // PRU Only
		// 2019.04.06. PRU Only
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_UL_ALC_GAIN][idx]     = &pDtuReg->DSP_Common_MIMO[m].UL_ALC_GAIN.Data;
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_PAU_DL_Attn_Sync][idx]= &pDtuReg->DSP_Common_MIMO[m].PAU_DL_Attn_Sync.Data;	
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_DL_Sum_Gain][idx]     = &pDtuReg->DSP_Common_MIMO[m].RW_DL_Sum_Gain.uD32;
		pReg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_EQ_Common][idx]       = &pDtuReg->DSP_Common_MIMO[m].RW_EQ_Common.Data;		
	#endif		
	}
	
//------------------------------------------------------------
// General-Dsp-FA/Port- MIMO #0 ~ #3
//------------------------------------------------------------
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{	
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_DSP_FA_Common_MAX; r++) 
			pReg_DSP_FA_Common_Addr[fa][r][idx] = NULL; 
		//----------------------------------------------------------------	

	#ifdef __TYPE__PRU__
		pReg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_DL_NCO][idx] = &pDtuReg->General_FA[fa].RW_PRU_DL_NCO.uD32;
		pReg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_UL_NCO][idx] = &pDtuReg->General_FA[fa].RW_PRU_UL_NCO.uD32;
	#else // ANTS-AAU
		pReg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_NCO][idx]    = &pDtuReg->General_FA[fa].RW_NCO.uD32;
	#endif		
		pReg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_NCO_Valid][idx] = &pDtuReg->General_FA[fa].R_NCO_VALID.Data;		
	}	

	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{	
			// init ----------------------------------------------------------	
			for(r=0; r < Reg_DSP_FA_MIMO_MAX; r++) 
				pReg_DSP_FA_MIMO_Addr[fa][m][r][idx] = NULL; 
			//----------------------------------------------------------------	

			// DL -----
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_OnOff][idx]    = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_OnOff.Data;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_Gain][idx]     = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_Gain.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_CW_Level][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_CW_Level.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_CW_On][idx]    = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_CW_On.Data;
			
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_Inpahase_Cont][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_Inphase_Cont.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_Quad_Cont][idx]     = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_Quad_Cont.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_BW_Sel][idx]        = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_BW_Sel.Data;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_EQ_Coeff][idx]      = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_EQ_Coeff.uD32;
			
			
			
			
			// UL -----
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_OnOff][idx]         = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_OnOff.Data;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_Gain][idx]          = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_Gain.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_Inpahase_Cont][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_Inphase_Cont.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_Quad_Cont][idx]     = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_Quad_Cont.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_BW_Sel][idx]        = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_BW_Sel.Data;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_EQ_Coeff][idx]      = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_EQ_Coeff.uD32;	

			// 2019.04.06
			#ifdef __TYPE__PRU__
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_OUTPUT_ATT][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_OUTPUT_ATT.uD32;
			#else
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_EQ_1_Coeff][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_EQ_1_Coeff.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_EQ_1_Coeff][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_EQ_1_Coeff.uD32;
			#endif
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_DELAY][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_DELAY.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_DELAY][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_DELAY.uD32;			
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TEST_Pattern][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_TEST_Pattern.Data;			

			// 2019.07.01
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_LSH_Gain][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_DL_LSH_Gain.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_LSH_Gain][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_UL_LSH_Gain.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_Test_Pattern_Band_Gain][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_Test_Pattern_Band_Gain.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TSSI_SD_Level_short][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level_short.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TSSI_SD_Level__long][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level__long.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TSSI_SD_Level___Low][idx] = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level___Low.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_Height_Bal_Gain][idx]     = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_Height_Bal_Gain.uD32;
			pReg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_PAU_EQ_Status][idx]       = &pDtuReg->General_FA[fa].General_FA_MIMO[m].RW_PAU_EQ_St.Data;
		}
	}
	//------------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_VSS_DL_MAX; r++) 
			pReg_VSS_DL_Addr[c][r][idx] = NULL; 
		//----------------------------------------------------------------
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RECSERDESLOS][idx] = &pDtuReg->DL_CPRI[c].RECSERDESLOS.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RECLOFALARM][idx]  = &pDtuReg->DL_CPRI[c].RECLOFALARM.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RECTypeID][idx]    = &pDtuReg->DL_CPRI[c].RECTypeID.uD32;
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_CnM_Path_selection][idx] = &pDtuReg->DL_CPRI[c].CnM_Path_selection.uD32; 
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Reset][idx]      = &pDtuReg->DL_CPRI[c].RE_Reset.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Channel_BW][idx] = &pDtuReg->DL_CPRI[c].RE_Channel_BW.uD32;

		//----------------------------------------------------------------

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_4th_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP2_Address[3].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_4th_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP1_Address[3].uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_3rd_Byte][idx] = &pDtuReg->DL_CPRI[c].Base_Mac_Address[2].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_6th_Byte][idx] = &pDtuReg->DL_CPRI[c].Base_Mac_Address[5].uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_3rd_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP2_Address[2].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_3rd_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP1_Address[2].uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_2nd_Byte][idx] = &pDtuReg->DL_CPRI[c].Base_Mac_Address[1].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_5th_Byte][idx] = &pDtuReg->DL_CPRI[c].Base_Mac_Address[4].uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_2nd_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP2_Address[1].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_2nd_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP1_Address[1].uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_1st_Byte][idx] = &pDtuReg->DL_CPRI[c].Base_Mac_Address[0].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_4th_Byte][idx] = &pDtuReg->DL_CPRI[c].Base_Mac_Address[3].uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_1st_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP2_Address[0].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_1st_Byte][idx] = &pDtuReg->DL_CPRI[c].Noti_IP1_Address[0].uD32;

		//----------------------------------------------------------------

		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_OpenCPRIVersion][idx]      = &pDtuReg->DL_CPRI[c].OpenCPRIVersion.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_CPRICompressionOnOff][idx] = &pDtuReg->DL_CPRI[c].CPRICompressionOnOff.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Cascade_ID][idx]        = &pDtuReg->DL_CPRI[c].RE_Cascade_ID.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_OpticLOSDetectAlarm][idx]  = &pDtuReg->DL_CPRI[c].OpticLOSDetectAlarm.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Loopbackmode][idx]         = &pDtuReg->DL_CPRI[c].Loopbackmode.uD32;
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Delay_H][idx] = &pDtuReg->DL_CPRI[c].Delay_H.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Delay_L][idx] = &pDtuReg->DL_CPRI[c].Delay_L.uD32;

		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE1Reset][idx] = &pDtuReg->DL_CPRI[c].REx_Reset[0].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE2Reset][idx] = &pDtuReg->DL_CPRI[c].REx_Reset[1].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE3Reset][idx] = &pDtuReg->DL_CPRI[c].REx_Reset[2].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE4Reset][idx] = &pDtuReg->DL_CPRI[c].REx_Reset[3].uD32;
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE1TxDelay_H][idx] = &pDtuReg->DL_CPRI[c].RE1TxDelay_H.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE1TxDelay_L][idx] = &pDtuReg->DL_CPRI[c].RE1TxDelay_L.uD32;
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE1RxDelay_H][idx] = &pDtuReg->DL_CPRI[c].RE1RxDelay_H.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_RE1RxDelay_L][idx] = &pDtuReg->DL_CPRI[c].RE1RxDelay_L.uD32;

		for(k = 0; k < 20; k++)
			pReg_VSS_DL_Addr[c][k+Reg_VSS_DL_TDD_Slot_Number00][idx] = &pDtuReg->DL_CPRI[c].TDD_Slot_Number[k].uD32;
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_UL_GainBackOff_Onoff][idx] = &pDtuReg->DL_CPRI[c].UL_GainBackOff_Onoff.uD32; // 2019.05.13
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Cell_ID][idx]         = &pDtuReg->DL_CPRI[c].Cell_ID.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_1st_Byte][idx] = &pDtuReg->DL_CPRI[c].gNB_ID_1st.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_2nd_Byte][idx] = &pDtuReg->DL_CPRI[c].gNB_ID_2nd.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_3rd_Byte][idx] = &pDtuReg->DL_CPRI[c].gNB_ID_3rd.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_4th_Byte][idx] = &pDtuReg->DL_CPRI[c].gNB_ID_4th.uD32;

				
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_DUH_ID][idx]       = &pDtuReg->DL_CPRI[c].DUH_ID.uD32;
		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_PCI_1st_Byte][idx] = &pDtuReg->DL_CPRI[c].PCI_1st_Byte.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_PCI_2nd_Byte][idx] = &pDtuReg->DL_CPRI[c].PCI_2nd_Byte.uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_REC_Port_No][idx]  = &pDtuReg->DL_CPRI[c].REC_Port_No.uD32;


		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address1][idx] = &pDtuReg->DL_CPRI[c].Base_IP_Address[0].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address2][idx] = &pDtuReg->DL_CPRI[c].Base_IP_Address[1].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address3][idx] = &pDtuReg->DL_CPRI[c].Base_IP_Address[2].uD32;
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address4][idx] = &pDtuReg->DL_CPRI[c].Base_IP_Address[3].uD32;


		pReg_VSS_DL_Addr[c][Reg_VSS_DL_L1Reset][idx]   = &pDtuReg->DL_CPRI[c].L1Reset.Data;		
		pReg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Clear][idx] = &pDtuReg->DL_CPRI[c].BIP_Clear.Data;
	}
	//------------------------------------------	
	for(c = 0; c < CPRI_PATH; c++)
	{	
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_VSS_UL_MAX; r++) 
			pReg_VSS_UL_Addr[c][r][idx] = NULL; 
		//----------------------------------------------------------------
		
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RECSERDESLOS][idx]   = &pDtuReg->UL_CPRI[c].RE_SERDESLOS.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RECLOFALARM][idx]    = &pDtuReg->UL_CPRI[c].RE_LOFALARM.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_REFunctionFail][idx] = &pDtuReg->UL_CPRI[c].REFunctionFail.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Reset_Ack][idx]   = &pDtuReg->UL_CPRI[c].RE_Reset_Ack.uD32;		
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_ALIVE_ACK_00][idx]   = &pDtuReg->UL_CPRI[c].ALIVE_ACK_00.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_ALIVE_ACK_01][idx]   = &pDtuReg->UL_CPRI[c].ALIVE_ACK_01.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_Mismatch_Noti][idx]  = &pDtuReg->UL_CPRI[c].Mismatch_Noti.uD32;
		
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Cascade_ID_ACK][idx]      = &pDtuReg->UL_CPRI[c].RE_Cascade_ID_ACK.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE_OpticLOSDetectAlarm][idx] = &pDtuReg->UL_CPRI[c].RE_OpticLOSDetectAlarm.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Loopbackmode_ACK][idx]    = &pDtuReg->UL_CPRI[c].RE_Loopbackmode_ACK.uD32;

		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE1Reset][idx] = &pDtuReg->UL_CPRI[c].RE_Reset_ACK[0].uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE2Reset][idx] = &pDtuReg->UL_CPRI[c].RE_Reset_ACK[1].uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE3Reset][idx] = &pDtuReg->UL_CPRI[c].RE_Reset_ACK[2].uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE4Reset][idx] = &pDtuReg->UL_CPRI[c].RE_Reset_ACK[3].uD32;
		
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE1FunctionFail][idx] = &pDtuReg->UL_CPRI[c].RE_FunctionFail[0].uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE2FunctionFail][idx] = &pDtuReg->UL_CPRI[c].RE_FunctionFail[1].uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE3FunctionFail][idx] = &pDtuReg->UL_CPRI[c].RE_FunctionFail[2].uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE4FunctionFail][idx] = &pDtuReg->UL_CPRI[c].RE_FunctionFail[3].uD32;

		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Type_ID][idx]   = &pDtuReg->UL_CPRI[c].RE_Type_ID.uD32;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Vendor_ID][idx] = &pDtuReg->UL_CPRI[c].RE_Vendor_ID.uD32;		
		
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_L1_Reset_ACK][idx] = &pDtuReg->UL_CPRI[c].L1_Reset_ACK.Data;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Count][idx]    = NULL;
		pReg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Clear][idx]    = &pDtuReg->UL_CPRI[c].BIP_Clear.Data;
	}

//------------------------------------------------------------
// 'Mode General-TDD'
//------------------------------------------------------------		
	// init ----------------------------------------------------------	
	for(r=0; r < Reg_TDD_MAX; r++)  pReg_TDD_Addr[r][idx] = NULL; 
	//----------------------------------------------------------------

	pReg_TDD_Addr[Reg_TDD_RW_TDD_Mode][idx] = &pDtuReg->RW_TDD_Mode.Data;
	
	for(k = 0; k < 20; k++)
		pReg_TDD_Addr[Reg_TDD_RW_Slot00_Format+k][idx] = &pDtuReg->RW_Slot_Format[k].uD32;
		
	

	#ifdef __TYPE__PRU__
	pReg_TDD_Addr[Reg_TDD_RW_DL_ON_Guard_Time][idx] = &pDtuReg->DL_ON_Guard_Time.Data;
	pReg_TDD_Addr[Reg_TDD_RW_UL_ON_Guard_Time][idx] = &pDtuReg->UL_ON_Guard_Time.Data;	
	pReg_TDD_Addr[Reg_TDD_RW_DL_to_UL_Guard_Time][idx] = &pDtuReg->DL_to_UL_Guard_Time.Data;
	pReg_TDD_Addr[Reg_TDD_RW_UL_to_DL_Guard_Time][idx] = &pDtuReg->UL_to_DL_Guard_Time.Data;
	#else
	pReg_TDD_Addr[Reg_TDD_RW_DL_ON_Guard_Time][idx] = &pDtuReg->DL_ON_Guard_Time.Data;
	pReg_TDD_Addr[Reg_TDD_RW_UL_ON_Guard_Time][idx] = &pDtuReg->UL_ON_Guard_Time.Data; // 2019.05.14
	pReg_TDD_Addr[Reg_TDD_RW_DL_to_UL_Guard_Time][idx] = &pDtuReg->DL_to_UL_Guard_Time.Data; // 2019.05.14
	pReg_TDD_Addr[Reg_TDD_RW_UL_to_DL_Guard_Time][idx] = &pDtuReg->UL_to_DL_Guard_Time.Data;
	#endif
	
	

	pReg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time0][idx] = &pDtuReg->UL_GAIN_Guard_Time[0].uD32;
	pReg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time1][idx] = &pDtuReg->UL_GAIN_Guard_Time[1].uD32;
	pReg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time2][idx] = &pDtuReg->UL_GAIN_Guard_Time[2].uD32;
	pReg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time3][idx] = &pDtuReg->UL_GAIN_Guard_Time[3].uD32;
	pReg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time4][idx] = &pDtuReg->UL_GAIN_Guard_Time[4].uD32;
	
	pReg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Measure_Time][idx]= &pDtuReg->UL_GAIN_Measure_Time.uD32;	
	pReg_TDD_Addr[Reg_TDD_RW_TSYNC_Delay][idx] = &pDtuReg->TSYNC_Delay.uD32;
	
	// init ----------------------------------------------------------		
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r=0; r < Reg_TDD2_MAX; r++)  
			pReg_TDD2_Addr[fa][r][idx] = NULL; 
	}
	//----------------------------------------------------------------

//---------------------------------------------------------------------
// CFR : PRU & AAU
//---------------------------------------------------------------------
	for(path = 0; path < MAX_DL; path++)
	{
		// init
		for(r = 0; r < Reg_CFR_MAX; r++)
			pReg_CFR_Addr[path][r][idx] = NULL; 
			
		pReg_CFR_Addr[path][Reg_CFR_Version][idx]= &pDtuReg->CFR[path].Version.uD32;
		pReg_CFR_Addr[path][Reg_CFR_OnOff][idx]  = &pDtuReg->CFR[path].OnOff.Data;
		pReg_CFR_Addr[path][Reg_CFR_Level][idx]  = &pDtuReg->CFR[path].Level.uD32;
	}
//---------------------------------------------------------------------
// DPD : PRU Only
//---------------------------------------------------------------------	
	for(path = 0; path < MAX_DL; path++)
	{
		// init
		for(r = 0; r < Reg_CFR_MAX; r++)
			pReg_DPD_Addr[path][r][idx] = NULL; 

		pReg_DPD_Addr[path][Reg_DPD_Version][idx]= &pDtuReg->DPD[path].Version.uD32;
		pReg_DPD_Addr[path][Reg_DPD_OnOff][idx]  = &pDtuReg->DPD[path].OnOff.Data;
		
		pReg_DPD_Addr[path][Reg_DPD_St][idx]     = &pDtuReg->DPD[path].Status.uD32;
		pReg_DPD_Addr[path][Reg_DPD_Err_St][idx] = &pDtuReg->DPD[path].Err_St.uD32;
		pReg_DPD_Addr[path][Reg_DPD_Success_Cnt][idx]  = &pDtuReg->DPD[path].Success_Cnt.uD32;
		
		pReg_DPD_Addr[path][Reg_DPD_Input_Pwr][idx]    = &pDtuReg->DPD[path].Input_Pwr.uD32;
		pReg_DPD_Addr[path][Reg_DPD_Output_Pwr][idx]   = &pDtuReg->DPD[path].Output_Pwr.uD32;
		
		pReg_DPD_Addr[path][Reg_DPD_Eng_In_Pwr][idx]   = &pDtuReg->DPD[path].Eng_In_Pwr.uD32;
		pReg_DPD_Addr[path][Reg_DPD_Eng_Out_Pwr][idx]  = &pDtuReg->DPD[path].Eng_Out_Pwr.uD32;		
		pReg_DPD_Addr[path][Reg_DPD_FB_Eng_In_Pwr][idx]   = &pDtuReg->DPD[path].FB_Eng_In_Pwr.uD32;
		
		pReg_DPD_Addr[path][Reg_DPD_FB_In_Pwr][idx]    = &pDtuReg->DPD[path].FB_In_Pwr.uD32;
		pReg_DPD_Addr[path][Reg_DPD_RESET][idx]        = &pDtuReg->DPD[path].DPD_Reset.uD32;
	}

	// 2019.05.01
	pReg_DPD2_Addr[Reg_DPD_Core_RESET][idx] = &pDtuReg->DPD_Core_Reset.uD32;
	pReg_DPD2_Addr[Reg_DPD_All_RESET][idx]  = &pDtuReg->DPD_All_Reset.uD32;	

//------------------------------------------------------------
// RF : AAU Only
// 2019.05.03 : Both AAU and PRU Use below
//------------------------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{		
		// RF
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_RFU_CTRL_MAX; r++) 
			pReg_RFU_CTRL_Addr[m][r][idx] = NULL; 
		//----------------------------------------------------------------		
		pReg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_DL_ATT][idx] = &pDtuReg->RFU[m].DL_Att.Data;
		pReg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_UL_ATT][idx] = &pDtuReg->RFU[m].UL_Att.Data;
		pReg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_PA_OnOff][idx] = &pDtuReg->RFU[m].PA_OnOff.Data;
		pReg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_Layer_Info][idx] = &pDtuReg->RFU[m].LayerInfo.Data;

		// ALC
		// init ----------------------------------------------------------	
		for(r=0; r < Reg_ALC_MAX; r++) 
			pReg_ALC_Addr[m][r][idx] = NULL; 
		//----------------------------------------------------------------		
		pReg_ALC_Addr[m][Reg_ALC_RW_ALC_Threadhold][idx] = &pDtuReg->ALC[m].ALC_Threshold.Data;
		pReg_ALC_Addr[m][Reg_ALC_RW_Default_ATT][idx] = &pDtuReg->ALC[m].Def_Att.Data;
		pReg_ALC_Addr[m][Reg_ALC_RW_Mode][idx] = &pDtuReg->ALC[m].Mode.Data;
	}
	
	
//------------------------------------------------------------
// Axi add 0218
//------------------------------------------------------------
	// init ----------------------------------------------------------	
	for(m=0; m < CPRI_PATH; m++) 
		pReg_COMP_ONOFF_Addr[m][idx] = NULL; 
	//----------------------------------------------------------------		
	for(m=0; m < CPRI_PATH; m++) 
		pReg_COMP_ONOFF_Addr[m][idx] = &pDtuReg->COMP_ONOFF[m].Ctrl.Data;
		
	// init ----------------------------------------------------------	
	for(m=0; m < CPRI_PATH; m++) 
	{
		for(r = 0; r < Reg_AXI_CPRI_MAX; r++)	
			pReg_AXI_CPRI_Addr[m][r][idx] = NULL; 
	}
	//----------------------------------------------------------------		
#ifdef __TYPE__AAU__ // AAU Only
	for(m=0; m < CPRI_PATH; m++) 
	{
		for(r = 0; r < Reg_AXI_CPRI_MAX; r++)	
			pReg_AXI_CPRI_Addr[m][r][idx] = &pDtuReg->AXI[m].Cpri_Sts.Data;	
	}
#endif
}







/*
--------------------------------------------------------------------------------------------------
	Init All Reg Ctrl Flag
--------------------------------------------------------------------------------------------------
*/


void Init_All_Reg_Ctrl_Flag(void)
{
	int fa;
	int c, m, r;

	// init -----------------------------------------------------------	
	for(r=0; r < Reg_Common_MAX; r++) Reg_Common_flag[r] = 0;

	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r=0; r < Reg_Common_MIMO_MAX; r++) 	Reg_Common_MIMO_flag[c][r] = 0;
	}

	for(r=0; r < Reg_DSP_Common_Max; r++) Reg_DSP_Common_flag[r] = 0; 

	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r=0; r < Reg_DSP_Common_MIMO_MAX; r++) 
			Reg_DSP_Common_MIMO_flag[m][r] = 0; 
	}
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(r=0; r < Reg_DSP_FA_Common_MAX; r++) 
			Reg_DSP_FA_Common_flag[fa][r] = 0; 
	}

	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{	
			for(r=0; r < Reg_DSP_FA_MIMO_MAX; r++) 
				Reg_DSP_FA_MIMO_flag[fa][m][r] = 0; 
		}
	}

	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r=0; r < Reg_VSS_DL_MAX; r++) 
			Reg_VSS_DL_flag[c][r] = 0; 
	}
	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r=0; r < Reg_VSS_UL_MAX; r++) 
			Reg_VSS_UL_flag[c][r] = 0; 
	}

	for(r=0; r < Reg_TDD_MAX; r++)  Reg_TDD_flag[r] = 0; 
	
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r=0; r < Reg_TDD2_MAX; r++)  
			Reg_TDD2_flag[fa][r] = 0; 
	}

	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r=0; r < Reg_RFU_CTRL_MAX; r++) Reg_RFU_CTRL_flag[m][r] = 0; 
		for(r=0; r < Reg_ALC_MAX; r++) Reg_ALC_flag[m][r] = 0; 
	}

	for(m=0; m < CPRI_PATH; m++) 
	{
		Reg_COMP_ONOFF_flag[m] = 0; 
	}
	
	for(m=0; m < CPRI_PATH; m++) 
	{
		for(r = 0; r < Reg_AXI_CPRI_MAX; r++)	
			Reg_AXI_CPRI_flag[m][r] = 0; 
	}

	for(m = 0; m < MAX_DL; m++)
	{
		for(r = 0; r < Reg_CFR_MAX; r++)
			Reg_CFR_flag[m][r] = 0; 
	}

	for(m = 0; m < MAX_DL; m++)
	{
		for(r = 0; r < Reg_DPD_MAX; r++)
			Reg_DPD_flag[m][r] = 0; 
	}
	
	// 2019.05.01
	for(r = 0; r < Reg_DPD2_MAX; r++)
		Reg_DPD2_flag[r] = 0; 
}






/*
--------------------------------------------------------------------------------------------------
	Chk_Reg_Ctrl_Flag
--------------------------------------------------------------------------------------------------
*/

//#define _DISP__Chk_RegCtrlFlag_Status_
void Chk_RegCtrlFlag_Status(void)
{
	int c;
	int fa;
	int m, r;
	int path;

//----------------------------------------
//	Common
//----------------------------------------	
	for(r = 0; r < Reg_Common_MAX; r++)
	{		
		if(pReg_Common_Addr[r][FLG] != NULL) 
		{			
			if(*pReg_Common_Addr[r][FLG] > 0)
			{
				*pReg_Common_Addr[r][FLG] = 0;
				
				if(Reg_Common_flag[r]==0)
				{
					switch(r)
					{
						case Reg_Common_RW_DTU_LED:
							break;
							
						default:
							#ifdef _DISP__Chk_RegCtrlFlag_Status_
							D_Printf( DEBUG_SYSTEM, "[RCU] Reg_Common_Addr (0x%02X) 0x%08X, flag set.\n", 
								r, Reg_Common_Addr[r]);
							#endif		
							break;
					}
				}
				Reg_Common_flag[r] = 1;
			}		
		}
	}
//----------------------------------------
//	SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r = 0; r < Reg_Common_MIMO_MAX; r++)
		{			
			if(pReg_Common_MIMO_Addr[c][r][FLG] != NULL) 
			{	
				if(*pReg_Common_MIMO_Addr[c][r][FLG] > 0)
				{
					*pReg_Common_MIMO_Addr[c][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_Common_MIMO_flag[c][r]==0)
					{						
						D_Printf( DEBUG_SYSTEM, "[RCU] %d Reg_Common_MIMO_Addr (0x%02X) 0x%08X, flag set.\n", 
							c, r, Reg_Common_MIMO_Addr[c][r]);				
					}
					#endif
					Reg_Common_MIMO_flag[c][r] = 1;
				}				
			}
		}
	}
//---------------------------------------------------------------------
// DSP-Delay
//---------------------------------------------------------------------
	for(r = 0; r < Reg_DSP_Common_Max; r++)
	{
		if(pReg_DSP_Common_Addr[r][FLG] != NULL)
		{	
			if(*pReg_DSP_Common_Addr[r][FLG] > 0)
			{
				*pReg_DSP_Common_Addr[r][FLG] = 0;
				#ifdef _DISP__Chk_RegCtrlFlag_Status_
				if(Reg_DSP_Common_flag[r]==0)
				{
					D_Printf( DEBUG_SYSTEM, "[RCU] Reg_DSP_Common_Addr (0x%02X) 0x%08X, flag set.\n", 
						r, Reg_DSP_Common_Addr[r]);
				}
				#endif
				Reg_DSP_Common_flag[r] = 1;
			}
		}
	}
//---------------------------------------------------------------------
// DSP-Common-MIMO
//---------------------------------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_DSP_Common_MIMO_MAX; r++)
		{			
			if(pReg_DSP_Common_MIMO_Addr[m][r][FLG] != NULL)
			{				
				if(*pReg_DSP_Common_MIMO_Addr[m][r][FLG] > 0)
				{
					*pReg_DSP_Common_MIMO_Addr[m][r][FLG] = 0;
					if(Reg_DSP_Common_MIMO_flag[m][r]==0)
					{
						switch(r)
						{
							case Reg_DSP_Common_MIMO_RW_HC_CLR:
								break;

							default:
								#ifdef _DISP__Chk_RegCtrlFlag_Status_
								D_Printf( DEBUG_SYSTEM, "[RCU] mimo %d, Reg_DSP_Common_MIMO (0x%02X) 0x%08X, flag set.\n", 
									m, r, Reg_DSP_Common_MIMO_Addr[m][r]);
								#endif	
								break;
						}	
					}
					Reg_DSP_Common_MIMO_flag[m][r] = 1;
				}
			}
		}
	}
//------------------------------------------------------------
// General-Dsp-FA/Port- MIMO #0 ~ #3
//------------------------------------------------------------
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(r = 0; r < Reg_DSP_FA_Common_MAX; r++)
		{			
			if(pReg_DSP_FA_Common_Addr[fa][r][FLG] != NULL) 
			{
				if(*pReg_DSP_FA_Common_Addr[fa][r][FLG] > 0)
				{
					*pReg_DSP_FA_Common_Addr[fa][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_DSP_FA_Common_flag[fa][r]==0)
					{
						D_Printf( DEBUG_SYSTEM, "[RCU] fa %d, Reg_DSP_FA_Common_Addr (0x%02X) 0x%08X, flag set.\n", 
							fa, r, Reg_DSP_FA_Common_Addr[fa][r]);
					}
					#endif
					Reg_DSP_FA_Common_flag[fa][r]=1;
				}
			}
		}
	}
	
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			for(r = 0; r < Reg_DSP_FA_MIMO_MAX; r++)
			{				
				if(pReg_DSP_FA_MIMO_Addr[fa][m][r][FLG] != NULL) 
				{
					if(*pReg_DSP_FA_MIMO_Addr[fa][m][r][FLG] > 0)
					{
						*pReg_DSP_FA_MIMO_Addr[fa][m][r][FLG] = 0;
						#ifdef _DISP__Chk_RegCtrlFlag_Status_
						if(Reg_DSP_FA_MIMO_flag[fa][m][r]==0)
						{
							D_Printf( DEBUG_SYSTEM, "[RCU] fa %d, m %d, Reg_DSP_FA_MIMO_Addr (0x%02X) 0x%08X, flag set.\n", 
								fa, m, r, Reg_DSP_FA_MIMO_Addr[fa][m][r]);
						}
						#endif
						Reg_DSP_FA_MIMO_flag[fa][m][r] = 1;
					}
				}
			}
		}
	}


	
//------------------------------------------------------------
// Vss-CPRI-DL/UL
//------------------------------------------------------------
	for(c = CPRI_0; c < CPRI_PATH; c++)
	{
		//if((c == CPRI_0)||(c == CPRI_1))
		{			
			// UL ------------------------------------------
			for(r = 0; r < Reg_VSS_UL_MAX; r++)
			{
				if(pReg_VSS_UL_Addr[c][r][FLG] != NULL) 
				{	
					if(*pReg_VSS_UL_Addr[c][r][FLG] > 0)
					{
						*pReg_VSS_UL_Addr[c][r][FLG] = 0;
						if(Reg_VSS_UL_flag[c][r]==0)
						{
							switch(r)
							{
								case Reg_VSS_UL_ALIVE_ACK_00:								
								case Reg_VSS_UL_BIP_Clear:
								case Reg_VSS_UL_Mismatch_Noti:	
								case Reg_VSS_UL_RE_Reset_Ack:
								case Reg_VSS_UL_RE1Reset:
								case Reg_VSS_UL_RE2Reset:
								case Reg_VSS_UL_RE3Reset:
								case Reg_VSS_UL_RE4Reset:
									break;
									
								default:
									#ifdef _DISP__Chk_RegCtrlFlag_Status_
									D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_UL_Addr (0x%02X) 0x%08X, flag set.\n", 
										c, r, Reg_VSS_UL_Addr[c][r]);
									#endif	
									break;
							}
						}
						Reg_VSS_UL_flag[c][r] = 1;
					}					
				}
			}
		}		
		//else // CPRI_2, CPRI_3
		{
			// DL ------------------------------------------
			for(r = 0; r < Reg_VSS_DL_MAX; r++)
			{				
				if(pReg_VSS_DL_Addr[c][r][FLG] != NULL) 
				{
					if(*pReg_VSS_DL_Addr[c][r][FLG] > 0)
					{
						*pReg_VSS_DL_Addr[c][r][FLG] = 0;
						if(Reg_VSS_DL_flag[c][r]==0)
						{
							switch(r)
							{
								case Reg_VSS_DL_BIP_Clear:								
									break;
									
								default:
									#ifdef _DISP__Chk_RegCtrlFlag_Status_
									D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_DL_Addr (0x%02X) 0x%08X, flag set.\n", 
										c, r, Reg_VSS_DL_Addr[c][r]);
									#endif	
									break;
							}
						}
						Reg_VSS_DL_flag[c][r] = 1;
					}					
				}
			}
		}
	}
//------------------------------------------------------------
// 'Mode General-TDD'
//------------------------------------------------------------	
	for(r = 0; r < Reg_TDD_MAX; r++)
	{
		if(pReg_TDD_Addr[r][FLG] != NULL) 
		{	
			if(*pReg_TDD_Addr[r][FLG] > 0)
			{
				*pReg_TDD_Addr[r][FLG] = 0;
				#ifdef _DISP__Chk_RegCtrlFlag_Status_
				if(Reg_TDD_flag[r]==0)
				{
					D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD_Addr (0x%02X) 0x%08X, flag set.\n", 
						r, Reg_TDD_Addr[r]);
				}
				#endif
				Reg_TDD_flag[r] = 1;
			}
		}
	}

	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r = 0; r < Reg_TDD2_MAX; r++)
		{
			if(pReg_TDD2_Addr[fa][r][FLG] != NULL) 
			{	
				if(*pReg_TDD2_Addr[fa][r][FLG] > 0)
				{
					*pReg_TDD2_Addr[fa][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_TDD2_flag[fa][r]==0)
					{
						D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD2_Addr, fa %d, (0x%02X) 0x%08X, flag set.\n", 
							fa, r, Reg_TDD2_Addr[fa][r]);
					}
					#endif
					Reg_TDD2_flag[fa][r] = 1;
				}
			}
		}
	}

	//------------------------------------------------------------
	// Comp OnOff
	//------------------------------------------------------------
	for(m = 0; m < CPRI_PATH; m++) // ALC
	{
		if(pReg_COMP_ONOFF_Addr[m][FLG] != NULL)
		{
			if(*pReg_COMP_ONOFF_Addr[m][FLG] > 0)
			{
				*pReg_COMP_ONOFF_Addr[m][FLG] = 0;
				#ifdef _DISP__Chk_RegCtrlFlag_Status_
				if(Reg_COMP_ONOFF_flag[m]==0)
				{
					D_Printf( DEBUG_SYSTEM, "[RCU] Reg_COMP_ONOFF_Addr (0x%02X) 0x%08X, flag set.\n", 
						m, Reg_COMP_ONOFF_Addr[m]);
				}
				#endif
				Reg_COMP_ONOFF_flag[m] = 1;
			}			
		}
	}

	//---------------------------------------------------------------------
	// CFR : 2019.04.15 : PAU, AAU Common
	//---------------------------------------------------------------------
	for(path = 0; path < MAX_DL; path++)
	{
		for(r = 0; r < Reg_CFR_MAX; r++)
		{			
			if(pReg_CFR_Addr[path][r][FLG] != NULL)
			{				
				if(*pReg_CFR_Addr[path][r][FLG] > 0)
				{
					*pReg_CFR_Addr[path][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_CFR_flag[path][r]==0)
					{
						D_Printf( DEBUG_SYSTEM, "[RCU] path %d, Reg_CFR (0x%02X) 0x%08X, flag set.\n", 
							path, r, Reg_CFR_Addr[path][r]);
					}
					#endif
					Reg_CFR_flag[path][r] = 1;
				}
			}
		}
	}

//------------------------------------------------------------
// RF : AAU Only
//------------------------------------------------------------
#ifdef __TYPE__AAU__
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_RFU_CTRL_MAX; r++) // RF
		{						
			if(pReg_RFU_CTRL_Addr[m][r][FLG] != NULL)
			{				
				if(*pReg_RFU_CTRL_Addr[m][r][FLG] > 0)
				{
					*pReg_RFU_CTRL_Addr[m][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_RFU_CTRL_flag[m][r]==0)
					{
						D_Printf( DEBUG_SYSTEM, "[RFU_CTRL] m %d, Reg_RFU_CTRL_Addr (0x%02X) 0x%08X, flag set.\n", 
							m, r, Reg_RFU_CTRL_Addr[m][r]);
					}
					#endif
					Reg_RFU_CTRL_flag[m][r] = 1;
				}								
			}
		}
	}
	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_ALC_MAX; r++) // ALC
		{
			if(pReg_ALC_Addr[m][r][FLG] != NULL)
			{
				if(*pReg_ALC_Addr[m][r][FLG] > 0)
				{
					*pReg_ALC_Addr[m][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_ALC_flag[m][r]==0)
					{
						D_Printf( DEBUG_SYSTEM, "[RCU] m %d, Reg_ALC_Addr (0x%02X) 0x%08X, flag set.\n", 
							m, r, Reg_ALC_Addr[m][r]);
					}
					#endif
					Reg_ALC_flag[m][r] = 1;
				}
			}
		}
	}
#else // PRU	
	//---------------------------------------------------------------------
	// PRU
	// DPD
	//---------------------------------------------------------------------

	for(path = 0; path < MAX_DL; path++)
	{
		for(r = 0; r < Reg_DPD_MAX; r++)
		{			
			if(pReg_DPD_Addr[path][r][FLG] != NULL)
			{				
				if(*pReg_DPD_Addr[path][r][FLG] > 0)
				{
					*pReg_DPD_Addr[path][r][FLG] = 0;
					#ifdef _DISP__Chk_RegCtrlFlag_Status_
					if(Reg_DPD_flag[path][r]==0)
					{
						D_Printf( DEBUG_SYSTEM, "[RCU] path %d, Reg_DPD (0x%02X) 0x%08X, flag set.\n", 
							path, r, Reg_DPD_Addr[path][r]);
					}
					#endif					
					Reg_DPD_flag[path][r] = 1;
				}
			}
		}
	}

	// 2019.05.01
	for(r = 0; r < Reg_DPD2_MAX; r++)
	{			
		if(pReg_DPD2_Addr[r][FLG] != NULL)
		{				
			if(*pReg_DPD2_Addr[r][FLG] > 0)
			{
				*pReg_DPD2_Addr[r][FLG] = 0;
				#ifdef _DISP__Chk_RegCtrlFlag_Status_
				if(Reg_DPD2_flag[r]==0)
				{
					D_Printf( DEBUG_SYSTEM, "[RCU] Reg_DPD2 (0x%02X) 0x%08X, flag set.\n", 
						r, Reg_DPD2_Addr[r]);
				}
				#endif
				Reg_DPD2_flag[r] = 1;
			}
		}
	}
#endif
}
//------------------------------------------------


/////YCSO added START 190419
void Proc_RW_wave_Register(void)
{
	INT32U len = 0;
	INT32U sample_len = 0;
	INT32U read_data = 0;

/*	printf("**************************\n");
	printf("TEST POINT _YCSO_\n");
	printf("**************************\n");
*/
#ifdef __YCSO__SPECTRUM__DEBUG__
	printf("%d ms || Register Read Start here!! \n",OSTimerGet_msec(&spectrum_timer));
#endif

#ifdef __TYPE__PRU__			///// YCSO changed 190624
	if( m_Env.wave_type != DL ) sample_len = 4096;
	else sample_len = 8192;
#else
	sample_len = 4096;
#endif

	m_Reg_Flg.RW_FFT_Address.Data = 0x01;
	for( len = 0; len < sample_len; len++ )
	{
		Write_FPGA_Reg(Reg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_Addr], len,"Common_MIMO");		
		read_data = Read_FPGA_Reg(Reg_DSP_Common_Addr[Reg_DSP_Common_R_FFT_Result]);
	
	#ifdef __TYPE__AAU__ // AAU Only 
		// 2019.05.10 : I/Q changed
		sI[len] = (short)((read_data>>16)& 0x0000FFFF);
		sR[len] = (short)((read_data) & 0x0000FFFF);
		if( len % 500 == 0 )
			watchdog_live();
	#else
		sR[len] = (short)((read_data>>16)& 0x0000FFFF);
		sI[len] = (short)((read_data) & 0x0000FFFF);
		if( len % 500 == 0 )
			watchdog_live();
	#endif
	}

#ifdef __YCSO__SPECTRUM__DEBUG__
	printf("%d ms || Register Read END here!! \n",OSTimerGet_msec(&spectrum_timer));
#endif
}
///// YCSO added END 


/*
-------------------------------------------------------
	PRU/AAU
	FPGA Register Read
-------------------------------------------------------
*/
//----------------------------------------------


//----------------------------------------------
typedef struct __attribute__((packed))
{
	INT32U	val; // last stable value
	INT32U	prev; // previous value
	INT8U   cnt; // continuous count
	INT8U   reserved[3];
} VSS_Val;

static VSS_Val Vss_RE_Cascade_ID[CPRI_PATH];
static VSS_Val Vss_DL_Loopbackmode[CPRI_PATH];
static VSS_Val Vss_DL_RE_Channel_BW[CPRI_PATH];
static VSS_Val Vss_DL_TDD_Slot_Num[CPRI_PATH][20];
static VSS_Val Vss_DL_CPRICompressionOnOff[CPRI_PATH];
static VSS_Val Vss_DL_CnM_Path_Sel[CPRI_PATH];
static VSS_Val Vss_DL_REC_Port_No[CPRI_PATH];


//----------------------------------------------
INT32U Vss_Stable_Check(VSS_Val* pVss_Vall, INT32U curr32, INT16U reg_add16, INT8U cpri, INT8U idx)
{
	INT8U max_cnt;
	INT8U valid;
	char str[50];

	/*
	2019.08.08
	return received-value, if cpri is not CPRI_0/CPRI_1
	*/
	//if((cpri != CPRI_0)||(cpri != CPRI_1)) 
	//	return curr32;
	if((cpri != CPRI_0)&&(cpri != CPRI_1)) // 2019.09.16
		return curr32;

	switch(reg_add16)
	{
		case Reg_VSS_DL_RE_Channel_BW:
			max_cnt = 10;
			valid = 1;
			break;
			
		case Reg_VSS_DL_RE_Cascade_ID:
		case Reg_VSS_DL_REC_Port_No: // 2019.08.08
			max_cnt = 10; 
			valid = 1;
			break;
			
		case Reg_VSS_DL_Loopbackmode:
			max_cnt = 5; 
			if((curr32 != 0x33)&&(curr32 != 0xCC)&&(curr32 != 0xCA)) 
				valid = 0;
			else valid = 1;
			break;
		
		case Reg_VSS_DL_CPRICompressionOnOff: 
		case Reg_VSS_DL_CnM_Path_selection:		
		case Reg_VSS_DL_TDD_Slot_Number00:
			max_cnt = 10;
			valid = 1;
			break;		

		default:
			// init
			pVss_Vall->cnt = 0;
			pVss_Vall->prev = curr32;
			pVss_Vall->val = curr32;
			return curr32;
	}		
	
	if(pVss_Vall->prev == curr32)
	{
		if(valid)
			pVss_Vall->cnt++;
			
		if(pVss_Vall->cnt >= max_cnt)
		{
			pVss_Vall->cnt = 100; // stable			
			if(pVss_Vall->val != curr32)
			{
				pVss_Vall->val = curr32;

				switch(reg_add16)
				{
					case Reg_VSS_DL_RE_Channel_BW: sprintf(str, "RE_Channel_BW"); break;
					case Reg_VSS_DL_RE_Cascade_ID: sprintf(str, "RE_Cascade_ID"); break;
					case Reg_VSS_DL_REC_Port_No:   sprintf(str, "REC_Port_No  "); break;
					case Reg_VSS_DL_Loopbackmode:  sprintf(str, "Loopback-Mode"); break;
						
					case Reg_VSS_DL_CPRICompressionOnOff: sprintf(str, "CPRI Compress"); break;
					case Reg_VSS_DL_CnM_Path_selection  : sprintf(str, "CnM Path-Sel "); break;
					case Reg_VSS_DL_TDD_Slot_Number00   : sprintf(str, "TDD Slot-Num "); break;

					default: sprintf(str, "?? "); break;		
				}
				print_agent(DEBUG_SYSTEM, VT_GREEN, "CPRI%d VSS_DL_Reg(0x%X) %s : Stable Value 0x%X", 
					cpri, reg_add16 + idx, str, curr32);
			}
		}
	} 
	else pVss_Vall->cnt = 0;

	if(reg_add16 == Reg_VSS_DL_REC_Port_No)
	{		
		if(pVss_Vall->cnt < max_cnt)
		{
			print_agent(DEBUG_SYSTEM, VT_GREEN, "CPRI%d Reg_VSS_DL_REC_Port_No 0x%X, count %d", 
					cpri, curr32, pVss_Vall->cnt);
		}
	}
	
	pVss_Vall->prev = curr32;
	
	return pVss_Vall->val; 
}

void Vss_Stable_Init_All_Cnt(INT8U cpri)
{
	INT8U k;
	
	if(cpri >= CPRI_PATH) return;

	Vss_RE_Cascade_ID[cpri].cnt = 0;
	Vss_DL_Loopbackmode[cpri].cnt = 0;
	Vss_DL_RE_Channel_BW[cpri].cnt = 0;
	
	for(k = 0; k < 20; k++)
		Vss_DL_TDD_Slot_Num[cpri][k].cnt = 0;
		
	Vss_DL_CPRICompressionOnOff[cpri].cnt = 0;
	Vss_DL_CnM_Path_Sel[cpri].cnt = 0;
	Vss_DL_REC_Port_No[cpri].cnt = 0;
}

//----------------------------------------------
// 2019.06.22
void Init_Vss_Stable_Check(INT8U init)
{
	INT8U c, k;
	INT32U uVal32;
	
	for(c = 0; c < CPRI_PATH; c++)
	{
		// 2019.09.16 : delete below calling 'IS__CPRI_Lock()'
		////if(IS__CPRI_Lock(c)==0) continue; // 2019.09.09
		
		if(init) uVal32 = Read_FPGA_Reg(Reg_VSS_DL_Addr[c][Reg_VSS_DL_CnM_Path_selection]);
		else     uVal32 = Vss_DL_CnM_Path_Sel[c].val;
		m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32 = Vss_Stable_Check(&Vss_DL_CnM_Path_Sel[c], uVal32, 0xFFFF, 0, 0);	
		//--------------------------------
		if(init) uVal32 = Read_FPGA_Reg(Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Channel_BW]);
		else     uVal32 = Vss_DL_RE_Channel_BW[c].val;
		m_Reg_Sts.DL_CPRI[c].RE_Channel_BW.uD32 = Vss_Stable_Check(&Vss_DL_RE_Channel_BW[c], uVal32, 0xFFFF, 0, 0);
		//--------------------------------
		if(init) uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_CPRICompressionOnOff]);
		else     uVal32 = Vss_DL_CPRICompressionOnOff[c].val;
		m_Reg_Sts.DL_CPRI[c].CPRICompressionOnOff.uD32 = Vss_Stable_Check(&Vss_DL_CPRICompressionOnOff[c], uVal32, 0xFFFF, 0, 0);
		//--------------------------------
		if(init) uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Cascade_ID]);
		else     uVal32 = Vss_RE_Cascade_ID[c].val;
		m_Reg_Sts.DL_CPRI[c].RE_Cascade_ID.uD32 = Vss_Stable_Check(&Vss_RE_Cascade_ID[c], uVal32, 0xFFFF, 0, 0);
		//--------------------------------
		if(init) uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_REC_Port_No]);
		else     uVal32 = Vss_DL_REC_Port_No[c].val;
		m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD32 = Vss_Stable_Check(&Vss_DL_REC_Port_No[c], uVal32, 0xFFFF, 0, 0);		
		//--------------------------------	
		
		/*
		-----------------------------
		2019.10.08 : Do-Not use Vss_Stable_Check for 'Reg_VSS_DL_Loopbackmode'
		-----------------------------
		//if(init) uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Loopbackmode]);	
		//else     uVal32 = Vss_DL_Loopbackmode[c].val;
		//m_Reg_Sts.DL_CPRI[c].Loopbackmode.uD32 = Vss_Stable_Check(&Vss_DL_Loopbackmode[c], uVal32, 0xFFFF, 0, 0);
		*/		
		//--------------------------------
		for(k = 0; k < 20; k++)
		{		
			if(init) uVal32 = Read_FPGA_Reg(Reg_VSS_DL_Addr[c][k+Reg_VSS_DL_TDD_Slot_Number00]);
			else     uVal32 = Vss_DL_TDD_Slot_Num[c][k].val;
			m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[k].uD32 = Vss_Stable_Check(&Vss_DL_TDD_Slot_Num[c][k], uVal32, 0xFFFF, 0, 0);
		}
	}
}
//-----------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------
#ifdef __TYPE__AAU__ // AAU Only 	
void Read___AAU_Test_Pattern_Data(void)
{
	m_Reg_Sts.RW_r_pattern_add_fa0.uD32 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_add_fa0]);  
	m_Reg_Sts.RW_r_pattern_i_fa0.uD32   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_i_fa0]);    
	m_Reg_Sts.RW_r_pattern_q_fa0.uD32   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_q_fa0]);    
	
	m_Reg_Sts.RW_r_pattern_add_fa1.uD32 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_add_fa1]);  
	m_Reg_Sts.RW_r_pattern_i_fa1.uD32   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_i_fa1]);    
	m_Reg_Sts.RW_r_pattern_q_fa1.uD32   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_q_fa1]);

	m_St.d_AAU_Test_Pattern_addr[FA_0]      = (INT16U)m_Reg_Sts.RW_r_pattern_add_fa0.uD32;
	m_St.d_AAU_Test_Pattern_data[FA_0][_I_] = (INT16S)m_Reg_Sts.RW_r_pattern_i_fa0.uD32;
	m_St.d_AAU_Test_Pattern_data[FA_0][_Q_] = (INT16S)m_Reg_Sts.RW_r_pattern_q_fa0.uD32;
	
	m_St.d_AAU_Test_Pattern_addr[FA_1]      = (INT16U)m_Reg_Sts.RW_r_pattern_add_fa1.uD32;
	m_St.d_AAU_Test_Pattern_data[FA_1][_I_] = (INT16S)m_Reg_Sts.RW_r_pattern_i_fa1.uD32;
	m_St.d_AAU_Test_Pattern_data[FA_1][_Q_] = (INT16S)m_Reg_Sts.RW_r_pattern_q_fa1.uD32;
}

void Read___AAU_Test_Pattern_En(void)
{
	m_Reg_Sts.RW_r_pattern_wren_fa0.uD32= Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_wren_fa0]); 
	m_Reg_Sts.RW_r_pattern_wren_fa1.uD32= Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_r_pattern_wren_fa1]); 
	
	m_St.d_AAU_Test_Pattern_wrEn[FA_0]      = (INT8U)m_Reg_Sts.RW_r_pattern_wren_fa0.uD32;
	m_St.d_AAU_Test_Pattern_wrEn[FA_1]      = (INT8U)m_Reg_Sts.RW_r_pattern_wren_fa1.uD32;
}
#endif

#ifdef __TYPE__AAU__ // AAU Only
void Set___AAU_Test_Pattern_Addr(INT8U fa, INT16U addr11)
{
	INT32U val32;
	INT8U reg_add_8;
	
	if(fa >= MAX_FA_PATH) return;

	val32 = addr11 & 0x07FF; // 11bits
	
	if(fa == _100M) reg_add_8 = Reg_Common_RW_r_pattern_add_fa0;		
	else            reg_add_8 = Reg_Common_RW_r_pattern_add_fa1;

	Write_FPGA_Reg(Reg_Common_Addr[reg_add_8], val32, "Test_Pattern_Addr");
#if 0
	D_Printf(DEBUG_SYSTEM, "[RCU] Pattern <FA_%d> Addr 0x%04X(%d) : reg 0x%X, val 0x%X", 
		fa, addr11, addr11, Reg_Common_Addr[reg_add_8], val32);
		
	printf("[RCU] Pattern <FA_%d> Addr 0x%04X(%d) : reg 0x%X, val 0x%X\n", 
		fa, addr11, addr11, Reg_Common_Addr[reg_add_8], val32);
#endif		
}
#endif
#ifdef __TYPE__AAU__ // AAU Only
void Set___AAU_Test_Pattern_WrEn(INT8U fa, INT8U en)
{
	INT32U val32;
	INT8U reg_add_8;
	
	if(fa >= MAX_FA_PATH) return;

	val32 = en & 0x01;
	
	if(fa == _100M) reg_add_8 = Reg_Common_RW_r_pattern_wren_fa0;		
	else            reg_add_8 = Reg_Common_RW_r_pattern_wren_fa1;

	Write_FPGA_Reg(Reg_Common_Addr[reg_add_8], val32, "Test_Pattern_WrEn");

#if 0	
	D_Printf(DEBUG_SYSTEM, "[RCU] Pattern <FA_%d> WrEn 0x%04X : reg 0x%X, val 0x%X", 
		fa, en, Reg_Common_Addr[reg_add_8], val32);

	printf("[RCU] Pattern <FA_%d> WrEn 0x%04X : reg 0x%X, val 0x%X\n", 
		fa, en, Reg_Common_Addr[reg_add_8], val32);	
#endif		
}
#endif
#ifdef __TYPE__AAU__ // AAU Only
void Set___AAU_Test_Pattern_wr_iq(INT8U fa, INT8U iq, INT16S data16)
{
	INT8U reg_add_8;
	INT32U val32;
	
	if(fa >= MAX_FA_PATH) return;
	if(iq >= _IQ_) return;
	
	if(fa == _100M)
	{
		if(iq == _I_) reg_add_8 = Reg_Common_RW_r_pattern_i_fa0;
		else          reg_add_8 = Reg_Common_RW_r_pattern_q_fa0;
	}	
	else
	{
		if(iq == _I_) reg_add_8 = Reg_Common_RW_r_pattern_i_fa1;
		else          reg_add_8 = Reg_Common_RW_r_pattern_q_fa1;
	}
	val32 = (INT32U)data16;
	Write_FPGA_Reg(Reg_Common_Addr[reg_add_8], val32, "Pattern_wr_iq");

#if 0 
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "Pattern <FA_%d> IQ %d : reg 0x%X, val 0x%X(%d)", 
		fa, iq, Reg_Common_Addr[reg_add_8], val32, data16);	
#endif		
}
#endif


#ifdef __TYPE__AAU__ // AAU Only
#include "aau_pattern_40i.h"
#include "aau_pattern_40q.h"
#include "aau_pattern_60i.h"
#include "aau_pattern_60q.h"
#include "aau_pattern_80i.h"
#include "aau_pattern_80q.h"
#include "aau_pattern_100i.h"
#include "aau_pattern_100q.h"


extern INT16S AAU_T_Pattern_40i[2048];
extern INT16S AAU_T_Pattern_40q[2048];

extern INT16S AAU_T_Pattern_60i[2048];
extern INT16S AAU_T_Pattern_60q[2048];

extern INT16S AAU_T_Pattern_80i[2048];
extern INT16S AAU_T_Pattern_80q[2048];

extern INT16S AAU_T_Pattern_100i[2048];
extern INT16S AAU_T_Pattern_100q[2048];

static INT8U t_patten_fail[2][2048];
static INT16U t_patten_fail_cnt[2];
#endif


/*		
		write test pattern
		------------------------------
		*r_pattern_add_fa0 = i;
		*r_pattern_i_fa0 = test_pattern_value[i];
		*r_pattern_q_fa0 = test_pattern_value[i];
		*r_pattern_wren_fa0 = 1;
		*r_pattern_wren_fa0 = 0;
*/




	



#ifdef __TYPE__AAU__ // AAU Only

static OS_TIMER test_timer;

INT16S AAU_T_Pattern_100i_x_gain[2048];
INT16S AAU_T_Pattern_100q_x_gain[2048];

void Proc___AAU_Loading_100M_Test_Pattern(void)
{
	static INT8U init = 0;
	static INT16S t_gain_x10 = 0;
	INT16S* pData_i;
	INT16S* pData_q;
	INT16U addr;
	INT8U  fa, fail;
	INT8U  rec;
	static INT8U prev_LS = 0xFF;

	OSTimerReset( &test_timer );	

	fa = _100M;

	/*
	-----------------------------
	2019.08.23. AAU_Test_Pattern_Gain Added.
	-----------------------------	
	*/
	// +/- 10.0[dB] range
	for(rec=0; rec<LSH_Max; rec++)
	{
		if(m_Eep.d_t_pattern_gain_x10[_100M][rec] < (-100)) m_Eep.d_t_pattern_gain_x10[_100M][rec] = 0;
		if(m_Eep.d_t_pattern_gain_x10[_100M][rec] >    100) m_Eep.d_t_pattern_gain_x10[_100M][rec] = 0;		
	}

	// 2019.09.09 : test-pattern-gain for L/S
	//-------------------------------------
	switch(AufoConfigFlag.RECTypeID)
	{
		case _TYPE_LS_SS_ : rec = LSH_SS; break;
		case _TYPE_LS_ELG_: rec = LSH_ER; break;
		case _TYPE_LS_NKA_: rec = LSH_NK; break;
		
		///default: rec = LSH_SS; break; // 2019.09.25 : delete
		default: return;
	}
	if(prev_LS != AufoConfigFlag.RECTypeID)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[RCU] 100_Test_Pattern REC %d -> %d ", 
			prev_LS, AufoConfigFlag.RECTypeID);
			
		prev_LS = AufoConfigFlag.RECTypeID;
		init = 0; // for restart		
	}
	//-------------------------------------
	
	if(t_gain_x10 != m_Eep.d_t_pattern_gain_x10[_100M][rec]) // if user-change
	{		
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[RCU] 100_Test_Pattern gain %d -> %d ", 
			t_gain_x10, m_Eep.d_t_pattern_gain_x10[_100M][rec]);
			
		t_gain_x10 = m_Eep.d_t_pattern_gain_x10[_100M][rec];
		init = 0; // for restart
	}
	//-----------------------------	
	if(init == 0)
	{
		for(addr = 0; addr < 2048; addr++)	
		{
			AAU_T_Pattern_100i_x_gain[addr] = AAU_T_Pattern_100i[addr] * pow(10.0, (double)t_gain_x10/200.0);
			AAU_T_Pattern_100q_x_gain[addr] = AAU_T_Pattern_100q[addr] * pow(10.0, (double)t_gain_x10/200.0);
		}
		
		pData_i = AAU_T_Pattern_100i_x_gain; // pData_i = AAU_T_Pattern_100i;  
		pData_q = AAU_T_Pattern_100q_x_gain; // pData_q = AAU_T_Pattern_100q;
				
		t_patten_fail_cnt[fa] = 0; // init
		
		for(addr = 0; addr < 2048; addr++)
		{
			//--------------------------
			// 2019.07.12 : time delay
			//--------------------------
			if((addr%100)==0) OSTimeDly(3*OS_DELAY_10msec); 
			//--------------------------
			t_patten_fail[fa][addr] = 0; // init
			fail = 0; // init
			
			Set___AAU_Test_Pattern_Addr(fa, addr);					
			Set___AAU_Test_Pattern_wr_iq(fa, _I_, *pData_i);
			Set___AAU_Test_Pattern_wr_iq(fa, _Q_, *pData_q);
			Set___AAU_Test_Pattern_WrEn(fa, 1); // write '1'

			Read___AAU_Test_Pattern_Data();
			Read___AAU_Test_Pattern_En();

#if 0
			print_agent(DEBUG_SYSTEM, VT_BLINK_GREEN, "[RCU] Pattern <FA_%d> addr 0x%X : i %d, q %d, en %d \n", 
				fa, 
				m_St.d_AAU_Test_Pattern_addr[fa],
				m_St.d_AAU_Test_Pattern_data[fa][_I_],
				m_St.d_AAU_Test_Pattern_data[fa][_Q_],
				m_St.d_AAU_Test_Pattern_wrEn[fa]);
#endif
				

			if((m_St.d_AAU_Test_Pattern_addr[fa] == addr)&&
				(m_St.d_AAU_Test_Pattern_data[fa][_I_] == *pData_i)&&
				(m_St.d_AAU_Test_Pattern_data[fa][_Q_] == *pData_q)&&
				(m_St.d_AAU_Test_Pattern_wrEn[fa] == 1))
			{
				Set___AAU_Test_Pattern_WrEn(fa, 0); // write '0'
				Read___AAU_Test_Pattern_En();

				if(m_St.d_AAU_Test_Pattern_wrEn[fa] != 0)
				{
					fail = 1;
					printf(VT_BOLD_RED "[RCU] Pattern <FA_%d> addr 0x%X : fail \n" VT_END_CR, 
						fa, 
						m_St.d_AAU_Test_Pattern_addr[fa]);	
				}
			}
			else fail = 1;

			if(fail)
			{
				t_patten_fail[fa][addr] = 1;
				t_patten_fail_cnt[fa]++;
			}

			pData_i++;
			pData_q++;
		}
		init = 1;

		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[RCU] 100_Test_Pattern (%d)ms", OSTimerGet_msec(&test_timer));

		if(t_patten_fail_cnt[fa]==0)
			print_agent(DEBUG_SYSTEM, VT_BLINK_GREEN, "[RCU] <FA_%d> Test Pattern Success", fa);
		else
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RCU] <FA_%d> Test Pattern Fail(%d)", fa, t_patten_fail_cnt[fa]);		
	}
}
#endif

#ifdef __TYPE__AAU__ // AAU Only

INT16S AAU_T_Pattern_Alpha_i_x_gain[2048];
INT16S AAU_T_Pattern_Alpha_q_x_gain[2048];

void Proc___AAU_Loading_Test_Pattern(void)
{
	static INT8U init = 0;
	static INT8U bw_alpha = 0;
	INT8U pattern_load_req = 0;
	INT16S* pData_i;
	INT16S* pData_q;
	INT16U  addr;
	INT8U  fa, fail;
	INT8U  rec;
	static INT8U retry_en = 0;
	static INT8U cnt_proc = 0;
	static INT16S t_gain_x10 = 0;
	static INT8U prev_LS = 0xFF;
	
	OSTimerReset( &test_timer );

	/*
	-----------------------------
	2019.08.23. AAU_Test_Pattern_Gain Added.
	-----------------------------	
	*/
	// +/- 10.0[dB] range
	for(rec=0; rec<LSH_Max; rec++)
	{
		if(m_Eep.d_t_pattern_gain_x10[Alpha][rec] < (-100)) m_Eep.d_t_pattern_gain_x10[Alpha][rec] = 0;
		if(m_Eep.d_t_pattern_gain_x10[Alpha][rec] > 1000) m_Eep.d_t_pattern_gain_x10[Alpha][rec] = 0;
	}

	// 2019.09.09 : test-pattern-gain for L/S
	//-------------------------------------
	switch(AufoConfigFlag.RECTypeID)
	{
		case _TYPE_LS_SS_ : rec = LSH_SS; break;
		case _TYPE_LS_ELG_: rec = LSH_ER; break;
		case _TYPE_LS_NKA_: rec = LSH_NK; break;
		
		///default: rec = LSH_SS; break; // 2019.09.25 : delete
		default: return;
	}
	if(prev_LS != AufoConfigFlag.RECTypeID)
	{
		prev_LS = AufoConfigFlag.RECTypeID;
		init = 0; // for restart
	}
	//-------------------------------------
	
	if(t_gain_x10 != m_Eep.d_t_pattern_gain_x10[Alpha][rec]) // if user-change
		init = 0; // for restart
	
	if(bw_alpha != m_Eep.d_RE_Channel_BW[Alpha])	
		init = 0; // for restart	

	if(init == 0)
	{
		init = 1;
		pattern_load_req = 1;
		bw_alpha = m_Eep.d_RE_Channel_BW[Alpha];
		t_gain_x10 = m_Eep.d_t_pattern_gain_x10[Alpha][rec];
	}

	switch(m_Eep.d_RE_Channel_BW[Alpha])
	{
		case BW__40M : pData_i = AAU_T_Pattern_40i;  pData_q = AAU_T_Pattern_40q;  break;
		case BW__60M : pData_i = AAU_T_Pattern_60i;  pData_q = AAU_T_Pattern_60q;  break;
		case BW__80M : pData_i = AAU_T_Pattern_80i;  pData_q = AAU_T_Pattern_80q;  break;
		case BW_100M : pData_i = AAU_T_Pattern_100i; pData_q = AAU_T_Pattern_100q; break;
			
		default: pData_i = NULL; pData_q = NULL; break;
	}	
	// for Alpha --------------------------------------------------------------
	if(pattern_load_req)
	{
		pattern_load_req = 0;
		fa = Alpha; // alpha only
			
		if((pData_i != NULL)&&(pData_q != NULL))
		{
			/*
				2019.08.23. AAU_Test_Pattern_Gain Added.
			*/			
			for(addr = 0; addr < 2048; addr++)	
			{
				AAU_T_Pattern_Alpha_i_x_gain[addr] = (INT16S)(*pData_i * pow(10.0, (double)t_gain_x10/200.0));
				AAU_T_Pattern_Alpha_q_x_gain[addr] = (INT16S)(*pData_q * pow(10.0, (double)t_gain_x10/200.0));	

				pData_i++;
				pData_q++;
			}			
			
			t_patten_fail_cnt[fa] = 0; // init
			pData_i = AAU_T_Pattern_Alpha_i_x_gain;  
			pData_q = AAU_T_Pattern_Alpha_q_x_gain;	

			for(addr = 0; addr < 2048; addr++)
			{
				if((addr%100)==0) OSTimeDly(3*OS_DELAY_10msec); // 2019.07.12 : time delay 
				//--------------------------	
				
				t_patten_fail[fa][addr] = 0; // init
				fail = 0; // init
				
				Set___AAU_Test_Pattern_Addr(fa, addr);					
				Set___AAU_Test_Pattern_wr_iq(fa, _I_, *pData_i);
				Set___AAU_Test_Pattern_wr_iq(fa, _Q_, *pData_q);
				Set___AAU_Test_Pattern_WrEn(fa, 1); // write '1'

				Read___AAU_Test_Pattern_Data();
				Read___AAU_Test_Pattern_En();

				if((m_St.d_AAU_Test_Pattern_addr[fa] == addr)&&
					(m_St.d_AAU_Test_Pattern_data[fa][_I_] == *pData_i)&&
					(m_St.d_AAU_Test_Pattern_data[fa][_Q_] == *pData_q)&&
					(m_St.d_AAU_Test_Pattern_wrEn[fa] == 1))
				{
					Set___AAU_Test_Pattern_WrEn(fa, 0);  // write '0'
					Read___AAU_Test_Pattern_En();

					if(m_St.d_AAU_Test_Pattern_wrEn[fa] != 0)
						fail = 1;
				}
				else fail = 1;

				if(fail)
				{
					t_patten_fail[fa][addr] = 1;
					t_patten_fail_cnt[fa]++;
				}

				pData_i++;
				pData_q++;
			}

			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[RCU] A_Test_Pattern (%d)ms", OSTimerGet_msec(&test_timer));

			if(t_patten_fail_cnt[fa]==0)
				print_agent(DEBUG_SYSTEM, VT_BLINK_GREEN, "[RCU] <FA_%d>-Test Pattern Success", fa);
			else
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RCU] <FA_%d>-Test Pattern Fail(%d)", fa, t_patten_fail_cnt[fa]);			
		}	
	}
	//--------------------------------------------------------------

	//--------------------------------------------------------------
	// 2019.07.12 : fail 일때, 자주 retry 하지 않도록 .
	retry_en++;
	if(retry_en < 10) return;
	else retry_en = 0;

	cnt_proc++;
	//------------------------
	
	for(fa = 0; fa < MAX_Optic; fa++)
	{
		if(fa == _100M)
		{
			pData_i = AAU_T_Pattern_100i_x_gain;
			pData_q = AAU_T_Pattern_100q_x_gain;
		}
		else
		{
			switch(m_Eep.d_RE_Channel_BW[Alpha])
			{
				case BW__40M :
				case BW__60M :
				case BW__80M :
				case BW_100M :
					pData_i = AAU_T_Pattern_Alpha_i_x_gain;
					pData_q = AAU_T_Pattern_Alpha_q_x_gain;
					break;
					
				default: 
					pData_i = NULL; 
					pData_q = NULL; 
					break;
			}
		}

		if((pData_i == NULL)||(pData_q == NULL)) 
			continue;
		
		if(t_patten_fail_cnt[fa] > 0)
		{		
			if(cnt_proc > 5)
			{	
				cnt_proc = 10;
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RCU] <FA_%d> Test Pattern Fail(%d), Not-Retry.", fa, t_patten_fail_cnt[fa]);
				continue;
			}
			
			for(addr = 0; addr < 2048; addr++)
			{
				//--------------------------
				// 2019.07.12 : time delay
				//--------------------------
				if((addr%100)==0) OSTimeDly(3*OS_DELAY_10msec); 
				
				//--------------------------
				if(t_patten_fail[fa][addr])
				{					
					Set___AAU_Test_Pattern_Addr(fa, addr);
					Set___AAU_Test_Pattern_wr_iq(fa, _I_, *pData_i);
					Set___AAU_Test_Pattern_wr_iq(fa, _Q_, *pData_q);
					Set___AAU_Test_Pattern_WrEn(fa, 1);  // write '1'

					Read___AAU_Test_Pattern_Data();
					Read___AAU_Test_Pattern_En();

					if((m_St.d_AAU_Test_Pattern_addr[fa] == addr)&&
						(m_St.d_AAU_Test_Pattern_data[fa][_I_] == *pData_i)&&
						(m_St.d_AAU_Test_Pattern_data[fa][_Q_] == *pData_q)&&
						(m_St.d_AAU_Test_Pattern_wrEn[fa] == 1))
					{
						Set___AAU_Test_Pattern_WrEn(fa, 0);  // write '0'
						Read___AAU_Test_Pattern_En();

						if(m_St.d_AAU_Test_Pattern_wrEn[fa] == 0) // if same
						{
							t_patten_fail[fa][addr] = 0;
							t_patten_fail_cnt[fa]--;
						}
					}
					else
					{
						print_agent(DEBUG_SYSTEM, VT_YELLOW, "[RCU] retry Pattern <FA_%d> addr 0x%X : i %d, q %d, en %d \n" VT_END_CR, 
							fa, 
							m_St.d_AAU_Test_Pattern_addr[fa],
							m_St.d_AAU_Test_Pattern_data[fa][_I_],
							m_St.d_AAU_Test_Pattern_data[fa][_Q_],
							m_St.d_AAU_Test_Pattern_wrEn[fa]);	
					}
				}
				pData_i++;
				pData_q++;
			}	
				
			if(t_patten_fail_cnt[fa]==0)
			{				
				print_agent(DEBUG_SYSTEM, VT_BLINK_GREEN, "[RCU] <FA_%d>--Test Pattern Success", fa);
			}
			else
			{				
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RCU] <FA_%d>--Test Pattern Fail(%d)", fa, t_patten_fail_cnt[fa]);
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[RCU] A_Test_Pattern (%d)ms", OSTimerGet_msec(&test_timer));
			}			
		}	
	}	
}
#endif


/*
--------------------------------------------------------------------------
2019.09.16
LS 제조사별 0RB Signal SSB위치 상이하여, LS 제조사별 Detector 동작위치 Reigster 추가.
--------------------------------------------------------------------------
*/
void Proc___AAU_DL_Det_SSB(void)
{	
	INT16S DL_Det_SSB_Frame;
	INT16S DL_Det_SSB_Start;
	INT16S DL_Det_SSB_Lngth;
	INT8U rec;	
	//-------------------------------------
	switch(AufoConfigFlag.RECTypeID)
	{
		case _TYPE_LS_SS_ : rec = LSH_SS; break;
		case _TYPE_LS_ELG_: rec = LSH_ER; break;
		case _TYPE_LS_NKA_: rec = LSH_NK; break;
		
		default: rec = LSH_SS; break;
	}

	DL_Det_SSB_Frame = m_Eep.d_DL_Det_SSB_Frame[rec];
	DL_Det_SSB_Start = m_Eep.d_DL_Det_SSB_Start[rec];
	DL_Det_SSB_Lngth = m_Eep.d_DL_Det_SSB_Lngth[rec];

	if(DL_Det_SSB_Frame != m_Reg_Sts.RW_Det_Enable.uD16[0])
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Det_Enable], DL_Det_SSB_Frame, "SSB_En");
		
	if(DL_Det_SSB_Start != m_Reg_Sts.RW_Det_Start_Time.uD16[0])
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Det_Start_Time], DL_Det_SSB_Start, "SSB_Start");
		
	if(DL_Det_SSB_Lngth != m_Reg_Sts.RW_Det_Range_Time.uD16[0])
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Det_Range_Time], DL_Det_SSB_Lngth, "SSB_Range");
}
//-----------------------------------------------------------------------------------


/*
--------------------------------------------------------------------------
2019.09.25
	User가 CW ON 하면, 5분 후 자동으로 OFF
	User가 CW ON 하면, Gain Check mode, noise mode의 CW Level 제어 (0,40)이 동작하지 않음.
--------------------------------------------------------------------------
*/

	
	
static OS_TIMER timer_AAU_DL_CW_Off;
void Proc___AAU_DL_CW_Off(void)
{
	static INT8U init = 0;
	static INT8U cw_on = 0;	
	static INT8U cw_onoff_prev[MAX_FA_PATH][MAX_MIMO_PATH];
	static int count;
	INT8U c, l;
	INT8U cw_on_cnt;

	//------------------------------------
	if(init == 0)
	{		
		init = 1;
		cw_on = 0;
		count = 0;
		
		OSTimerReset(&timer_AAU_DL_CW_Off);
		for(c = 0; c < MAX_FA_PATH; c++)
		{
			for(l = 0; l < MAX_MIMO_PATH; l++)
			{	
				cw_onoff_prev[c][l] = m_St.User_DL_CW_OnOff[c][l];
			}
		}
		
		return;
	}
	//------------------------------------
	count++;
	if(count < 20) return; // every 2[s]
	else count = 0;
	//------------------------------------
	for(cw_on_cnt = 0, c = 0; c < MAX_FA_PATH; c++)
	{
		for(l = 0; l < MAX_MIMO_PATH; l++)
		{	
			if(cw_onoff_prev[c][l] != m_St.User_DL_CW_OnOff[c][l])
			{
				if(m_St.User_DL_CW_OnOff[c][l]) // if Off -> ON
				{
					cw_on = 1;
					cw_on_cnt++;
					OSTimerReset(&timer_AAU_DL_CW_Off);
				}
			}
			cw_onoff_prev[c][l] = m_St.User_DL_CW_OnOff[c][l];
		}
	}

	if(cw_on_cnt)
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[DL_CW_ON] CW-Auto-Off Timer Start");

	if(cw_on)
	{
		if(OSTimerGet_msec(&timer_AAU_DL_CW_Off) > (5*TICK_CO_MIN))		
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[DL_CW_ON] CW-Auto-Off TimeOut, All CW-Off-Control");
			OSTimerReset(&timer_AAU_DL_CW_Off);
			cw_on = 0;
			
			for(c = 0; c < MAX_FA_PATH; c++)
			{
				for(l = 0; l < MAX_MIMO_PATH; l++)
				{					
					m_St.User_DL_CW_OnOff[c][l] = 0; // '0' is off 
					Use_DL_CW_OnOff_Ctrl(c, l, 0); // '0' is off
				}
			}
		}
	}
	else
	{
		OSTimerReset(&timer_AAU_DL_CW_Off);
	}
}


void Update__SFP_Vendor_Info(void) // 2020.07.22
{
	INT8U cpri, k;
	type32 uVal32;
	INT8U  sfp_vendor[32];

	for(cpri = CPRI_0; cpri < CPRI_PATH; cpri++)
	{
		for(k = 0; k < 4; k++) 
		{
			uVal32.uD32 = htonl(m_Reg_Sts.Common_CPRI[cpri].R_SFP_Vendor_Name[k].uD32);
			memcpy(&sfp_vendor[4*k], uVal32.uD8, 4); // 4x4 byte
		}
		memcpy(m_St.d_SFP_Vendor_Name[cpri], sfp_vendor, 16);
		//-----------------------------------------
		for(k = 0; k < 4; k++) 
		{
			uVal32.uD32 = htonl(m_Reg_Sts.Common_CPRI[cpri].R_SFP_Vendor___PN[k].uD32);
			memcpy(&sfp_vendor[4*k], uVal32.uD8, 4); // 4x4 byte
		}
		memcpy(m_St.d_SFP_Vendor___PN[cpri], sfp_vendor, 16);
		//-----------------------------------------
		for(k = 0; k < 4; k++) 
		{
			uVal32.uD32 = htonl(m_Reg_Sts.Common_CPRI[cpri].R_SFP_Vendor___SN[k].uD32);
			memcpy(&sfp_vendor[4*k], uVal32.uD8, 4); // 4x4 byte
		}
		memcpy(m_St.d_SFP_Vendor___SN[cpri], sfp_vendor, 16);
		//-----------------------------------------
		for(k = 0; k < 2; k++)
		{
			uVal32.uD32 = htonl(m_Reg_Sts.Common_CPRI[cpri].R_SFP_Date___Code[k].uD32);
			memcpy(&sfp_vendor[4*k], uVal32.uD8, 4); // 4x2 byte
		}
		memcpy(m_St.d_SFP_Date___Code[cpri], sfp_vendor, 8);
	}
}

void Proc_Read_Fpga_Register(void)
{		
	int fa;
	int c, k, m;	
	INT32U uVal32;	
	INT8U cnt_2_en = 0;
	INT8U cnt_3_en = 0;
	INT8U cnt_5_en = 0;
	INT8U cnt_9_en = 0;

//--------------------------	
#ifdef __TYPE__AAU__ // AAU Only
//--------------------------	
	static INT8U cnt_2 = 10;
	static INT8U cnt_3 = 10;
	static INT8U cnt_5 = 10;
	static INT8U cnt_9 = 10;	
	//--------------------------
	cnt_2++;
	cnt_3++;
	cnt_5++;
	cnt_9++;
	if(cnt_2 >= 2)
	{
		cnt_2 = 0;
		cnt_2_en = 1;
	}
	//--------------------------	
	if(cnt_3 >= 3)
	{
		cnt_3 = 0;
		cnt_3_en = 1;
	}
	//--------------------------	
	if(cnt_5 >= 5)
	{
		cnt_5 = 0;
		cnt_5_en = 1;
	}
	//--------------------------	
	if(cnt_9 >= 10)
	{
		cnt_9 = 0;
		cnt_9_en = 1;	
	}
//--------------------------		
#else // PRU
//--------------------------
	cnt_2_en = 1;
	cnt_3_en = 1;
	cnt_5_en = 1;
	cnt_9_en = 1;
//--------------------------		
#endif
//--------------------------	
	if(cnt_5_en)
	{
		m_Reg_Sts.R_SW_Ver.uD32  = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_SW_Ver] );		
		m_Reg_Sts.R_HW_Ver.uD32	 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_HW_Ver] );
		/////m_Reg_Sts.RW_Test.uD32	 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Test] ); // deleted by duplication	
		
		m_Reg_Sts.R_App_MB_Cnter.uD32 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_App_MB_Counter] );
		m_Reg_Sts.R_FPGA_Counter.uD32 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_FPGA_Counter] );
		m_Reg_Sts.R_Board_INFO.uD32   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_Board_INFO] );			
		m_Reg_Sts.R_FPGA_Temp.uD32    = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_FPGA_Temp] );
		m_Reg_Sts.RW_Reset.Data       = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Reset] );	
	}
	m_Reg_Sts.RW_ETH_Path_Sel.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_ETH_Path_Sel] );
	
	m_Reg_Sts.RW_Mapper_Loopback.Data   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Mapper_Loopback] );
	m_Reg_Sts.RW_RF_Loopback.Data       = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_RF_Loopback] );
	
	m_Reg_Sts.RW_FAN_Control.Data        = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_FAN_Control] );
	m_Reg_Sts.RW_Recovery_Clock_Set.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Recovery_Clock_Set] );
	m_Reg_Sts.Gain_Check_Mode.Data       = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Gain_Check_Mode] );	

	if(cnt_9_en)
	{	
		m_Reg_Sts.RW_Interrupt_mask0.Data   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Interrupt_mask0] );
		m_Reg_Sts.RW_Interrupt_mask1.Data   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Interrupt_mask1] );
		m_Reg_Sts.RW_Interrupt_Status0.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Interrupt_status0] );
		m_Reg_Sts.RW_Interrupt_Status1.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_Interrupt_status1] );
	}
	
	m_Reg_Sts.R_RE_Reset.Data      = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R__RE_Reset] );
	m_Reg_Sts.RW_RE_Reset_Clr.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_RE_Reset_Clr] );	
	m_Reg_Sts.RW_VSS_Tbl_Dbg.Data  = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_VSS_Table_Debug] );
	m_Reg_Sts.PSU_Reset.Data       = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_PSU_Reset] );
	
	m_Reg_Sts.AC_Alarm_info.uD32   = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_AC_ALARM_INFO] ); // 2019.04.22

	if(cnt_5_en)
	{
	#ifdef __TYPE__AAU__ // AAU Only	
		for(k = 0; k < 8; k++)
			m_Reg_Sts.RW_DTU_Inventory[k].uD32 = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_DTU_INV_INFO+k]); // 2019.04.30

		m_Reg_Sts.RW_Det_Enable.uD32     = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Det_Enable]); // 2019.09.16
		m_Reg_Sts.RW_Det_Start_Time.uD32 = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Det_Start_Time]); // 2019.09.16
		m_Reg_Sts.RW_Det_Range_Time.uD32 = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Det_Range_Time]); // 2019.09.16
	#endif
	}
	m_Reg_Sts.RW_RE_Reset_ACK_SW.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_RE_Reset_ACK_SW] ); // 2019.05.17
	
//----------------------------------------
//		SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	if(cnt_5_en)
	{
		for(c = 0; c < CPRI_PATH; c++)
		{		
			m_Reg_Sts.Common_CPRI[c].R_SFP_LD_Power.uD32  = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_LD_Power] );
			m_Reg_Sts.Common_CPRI[c].R_SFP_PD_Power.uD32  = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_PD_Power] );
			m_Reg_Sts.Common_CPRI[c].R_SFP_VCC.uD32       = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VCC] );
			m_Reg_Sts.Common_CPRI[c].R_SFP_LD_Bias.sD32   = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_LD_Bias] );
			m_Reg_Sts.Common_CPRI[c].R_SFP_Temp.uD32      = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_Temp] );		
			m_Reg_Sts.Common_CPRI[c].R_SFP_wavelength.uD32= Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_WaveLen] );
			m_Reg_Sts.Common_CPRI[c].RW_SFP_TX_DISABLE.Data = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_TX_Disable]);
			m_Reg_Sts.Common_CPRI[c].RW_SFP_Reset.Data      = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_Reset] );
			m_Reg_Sts.Common_CPRI[c].R_SFP_BitRate.uD32     = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_BitRate] );		
			
			m_Reg_Sts.Common_CPRI[c].R_SFP_LOS_Alarm.Data = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_LOS_Alarm]); // 2019.05.21. Added.
			m_Reg_Sts.Common_CPRI[c].R_SFP_LD_Alarm.Data  = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_LD_Alarm]);  // 2019.05.21. Added.
		
			// 2019.04.06 : SFP inventory
			for(k = 0; k < 8; k++)
				m_Reg_Sts.Common_CPRI[c].R_SFP_Inventory[k].uD32 = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_INV_INFO+k]);

			for(k = 0; k < 4; k++)
			{
				m_Reg_Sts.Common_CPRI[c].R_SFP_Vendor_Name[k].uD32 = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VENDOR_NAME+k]);
				m_Reg_Sts.Common_CPRI[c].R_SFP_Vendor___PN[k].uD32 = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VENDOR___PN+k]);
				m_Reg_Sts.Common_CPRI[c].R_SFP_Vendor___SN[k].uD32 = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_VENDOR___SN+k]);
			}
			for(k = 0; k < 2; k++)
				m_Reg_Sts.Common_CPRI[c].R_SFP_Date___Code[k].uD32 = Read_FPGA_Reg( Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_R_SFP_DATE___CODE+k]);
		}

		Update__SFP_Vendor_Info();		
	}

//---------------------------------------------------------------------
// DSP-Delay
//---------------------------------------------------------------------
	if(cnt_3_en)
	{
		m_Reg_Sts.RW_DL_DELAY.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_DL_Delay]);
		m_Reg_Sts.RW_UL_DELAY.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_Delay]);
		//m_Reg_Sts.RW_4T2R_Mode.Data = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_4T2R_Mode]);
	}

	m_Reg_Sts.RW_FFT_Mux.Data = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_MUX]);
	m_Reg_Sts.RW_FFT_Trigger.Data = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_Trigger]);
	m_Reg_Sts.R_FFT_Done.Data = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_R_FFT_Done]);
	m_Reg_Sts.RW_FFT_Address.Data = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_FFT_Addr]);
	m_Reg_Sts.RW_FFT_Result.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_R_FFT_Result]);
	
#ifdef __TYPE__PRU__ // PRU Only, 2019.06.01
	m_Reg_Sts.RW_Capture_DL_Time.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_DL_Time]);
	m_Reg_Sts.RW_Capture_UL_Time.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_UL_Time]);
		
	m_Reg_Sts.RW_Capture_ULNoise_Time.uD32    = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_Capture_ULNoise_Time]);
	m_Reg_Sts.RW_UL_Noise_RSSI_StartTime.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_Noise_RSSI_StartTime]);
	m_Reg_Sts.RW_UL_Noise_RSSI_StopTime.uD32  = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_Noise_RSSI_StopTime]);
		
	m_Reg_Sts.RW_UL_RSSI_StartTime.uD32 = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_RSSI_StartTime]);
	m_Reg_Sts.RW_UL_RSSI_StopTime.uD32  = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_UL_RSSI_StopTime]);
	
	m_Reg_Sts.RW_SG_Flat.Data  = Read_FPGA_Reg( Reg_DSP_Common_Addr[Reg_DSP_Common_RW_SG_Flat]);
#endif

//---------------------------------------------------------------------
// DSP_Common_MIMO
//---------------------------------------------------------------------	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		m_Reg_Sts.DSP_Common_MIMO[m].HC_Threshold.Data = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_Threshold]);
		m_Reg_Sts.DSP_Common_MIMO[m].HC_Clr.Data       = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_HC_CLR]);
		m_Reg_Sts.DSP_Common_MIMO[m].HC_Count.Data     = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_R_HC_COUNT]);
		m_Reg_Sts.DSP_Common_MIMO[m].CFR_Out_Gain.Data = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_CFR_OUT_GAIN]);

		// 2019.04.06. PRU Only
	#ifdef __TYPE__PRU__
		m_Reg_Sts.DSP_Common_MIMO[m].UL_ALC_GAIN.Data      = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_UL_ALC_GAIN]);
		m_Reg_Sts.DSP_Common_MIMO[m].PAU_DL_Attn_Sync.Data = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_PAU_DL_Attn_Sync]);		
		m_Reg_Sts.DSP_Common_MIMO[m].R_UL_IN_RSSI.uD32   = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_UL_IN_RSSI_DET]);
		m_Reg_Sts.DSP_Common_MIMO[m].RW_DL_Sum_Gain.uD32 = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_DL_Sum_Gain]);
		m_Reg_Sts.DSP_Common_MIMO[m].RW_EQ_Common.Data   = Read_FPGA_Reg( Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_EQ_Common]);
	#endif	
	}	
//------------------------------------------------------------
// General-Dsp-FA/Port- MIMO #0 ~ #3
//------------------------------------------------------------
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{	
	#ifdef __TYPE__PRU__
		m_Reg_Sts.General_FA[fa].RW_PRU_DL_NCO.uD32 = Read_FPGA_Reg( Reg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_DL_NCO]);
		m_Reg_Sts.General_FA[fa].RW_PRU_UL_NCO.uD32 = Read_FPGA_Reg( Reg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_UL_NCO]);
	#else // ANTS-AAU
		m_Reg_Sts.General_FA[fa].RW_NCO.uD32      = Read_FPGA_Reg( Reg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_NCO]);
	#endif	
	
		m_Reg_Sts.General_FA[fa].R_NCO_VALID.Data = Read_FPGA_Reg( Reg_DSP_FA_Common_Addr[fa][Reg_DSP_FA_Common_RW_NCO_Valid]);
			
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{	
			// DL -----
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_OnOff.Data        = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_OnOff]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_Gain.uD32         = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_Gain]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_CW_Level.uD32     = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_CW_Level]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_CW_On.Data        = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_CW_On]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_DL_TSSI.uD32          = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_DL_TSSI]);			
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_BW_Sel.Data       = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_BW_Sel]);			

			if(cnt_3_en)
			{
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_Inphase_Cont.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_Inpahase_Cont]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_Quad_Cont.uD32    = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_Quad_Cont]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_EQ_Coeff.uD32    = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_EQ_Coeff]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_DL_RSSI_Peak.uD32    = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_DL_RSSI_Peak]);			
			}
			// UL -----
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_OnOff.Data           = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_OnOff]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_Gain.uD32            = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_Gain]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_RSSI.uD32             = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_UL_RSSI]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_BW_Sel.Data          = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_BW_Sel]);

			if(cnt_3_en)
			{
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_Inphase_Det.uD32	   = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_UL_Inpahase_Det]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_Quad_Det.uD32		   = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_UL_Quad_Det]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_Inphase_Cont.uD32    = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_Inpahase_Cont]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_Quad_Cont.uD32	   = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_Quad_Cont]);
				m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_EQ_Coeff.uD32    = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_EQ_Coeff]);
			}
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_RSSI_Peak.uD32        = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_UL_RSSI_Peak]);			

			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_FB_RSSI_RMS.uD32 	   = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_FB_RSSI_RMS]);			
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_FB_RSSI_Peak.uD32	   = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_FB_RSSI_Peak]);			
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_GC_RSSI_RMS.uD32	   = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_GC_RSSI_RMS]);			
						
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_RE_Reset.Data	= Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R_RE_Reset]);

			// 2019.04.06
		#ifdef __TYPE__PRU__
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_OUTPUT_ATT.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_OUTPUT_ATT]);
		#else
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_EQ_1_Coeff.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_EQ_1_Coeff]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_EQ_1_Coeff.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_EQ_1_Coeff]);
		#endif			
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_DELAY.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_DELAY]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_DELAY.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_DELAY]);			
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_TSSI_SD.Data = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R__TSSI_SD_St]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_TEST_Pattern.Data = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TEST_Pattern]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_NOISE_RMS.uD32 = Read_FPGA_Reg( Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_R__UL_NOISE_RMS]);
			
			// 2019.07.01
		#ifdef __TYPE__PRU__	
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_LSH_Gain.uD32 = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_LSH_Gain]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_LSH_Gain.uD32 = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_UL_LSH_Gain]);

			// 2019.07.01
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level_short.uD32 = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TSSI_SD_Level_short]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level__long.uD32 = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TSSI_SD_Level__long]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_TSSI_SD_Level___Low.uD32 = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_TSSI_SD_Level___Low]);

			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_Height_Bal_Gain.uD32     = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_Height_Bal_Gain]);
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_PAU_EQ_St.Data           = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_PAU_EQ_Status]);
		#endif	
			// 2019.07.01
			m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_Test_Pattern_Band_Gain.uD32 = Read_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_Test_Pattern_Band_Gain]);			
		}
	}
	//------------------------------------------	
	{
		for(c = 0; c < CPRI_PATH; c++)
		{
			if(IS__CPRI_Lock(c)==0) 
			{
				Vss_Stable_Init_All_Cnt(c); // 2019.09.16
				continue; // 2019.09.09
			}
			
			m_Reg_Sts.DL_CPRI[c].RECSERDESLOS.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECSERDESLOS]);
			m_Reg_Sts.DL_CPRI[c].RECLOFALARM.uD32  = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECLOFALARM]);
			m_Reg_Sts.DL_CPRI[c].RECTypeID.uD32    = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECTypeID]);
						
			// 2019.06.22
			uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_CnM_Path_selection]);
			m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32 = Vss_Stable_Check(&Vss_DL_CnM_Path_Sel[c], uVal32, Reg_VSS_DL_CnM_Path_selection, c, 0);	
			
			m_Reg_Sts.DL_CPRI[c].RE_Reset.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Reset]);

			// 2019.06.22			
			uVal32 = Read_FPGA_Reg(Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Channel_BW]);
			m_Reg_Sts.DL_CPRI[c].RE_Channel_BW.uD32 = Vss_Stable_Check(&Vss_DL_RE_Channel_BW[c], uVal32, Reg_VSS_DL_RE_Channel_BW, c, 0);

			//<--
			if(cnt_2_en)
			{
				m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[0].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_1st_Byte]);
				m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[1].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_2nd_Byte]);
				m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[2].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_3rd_Byte]);
				m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[3].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP1_4th_Byte]);
			
				m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[0].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_1st_Byte]);
				m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[1].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_2nd_Byte]);
				m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[2].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_3rd_Byte]);
				m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[3].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Noti_IP2_4th_Byte]);

				m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[0].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_1st_Byte]);
				m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[1].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_2nd_Byte]);
				m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[2].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_3rd_Byte]);
				m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[3].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_4th_Byte]);
				m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[4].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_5th_Byte]);
				m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[5].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Base_Mac_6th_Byte]);
			}			
			
			//<--			
			m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD32      = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_OpenCPRIVersion]);

			// 2019.06.22
			uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_CPRICompressionOnOff]);
			m_Reg_Sts.DL_CPRI[c].CPRICompressionOnOff.uD32 = Vss_Stable_Check(&Vss_DL_CPRICompressionOnOff[c], uVal32, Reg_VSS_DL_CPRICompressionOnOff, c, 0);

			// 2019.06.22
			uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE_Cascade_ID]);
			m_Reg_Sts.DL_CPRI[c].RE_Cascade_ID.uD32 = Vss_Stable_Check(&Vss_RE_Cascade_ID[c], uVal32, Reg_VSS_DL_RE_Cascade_ID, c, 0);
			
			m_Reg_Sts.DL_CPRI[c].OpticLOSDetectAlarm.uD32  = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_OpticLOSDetectAlarm]);

			// 2019.06.22	
			/*
			-----------------------------
			2019.10.08 : Do-Not use Vss_Stable_Check for 'Reg_VSS_DL_Loopbackmode'
			-----------------------------			
			//uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Loopbackmode]);	
			//m_Reg_Sts.DL_CPRI[c].Loopbackmode.uD32 = Vss_Stable_Check(&Vss_DL_Loopbackmode[c], uVal32, Reg_VSS_DL_Loopbackmode, c, 0);
			*/
			m_Reg_Sts.DL_CPRI[c].Loopbackmode.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Loopbackmode]);	
			

			m_Reg_Sts.DL_CPRI[c].REx_Reset[0].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE1Reset]);
			m_Reg_Sts.DL_CPRI[c].REx_Reset[1].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE2Reset]);
			m_Reg_Sts.DL_CPRI[c].REx_Reset[2].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE3Reset]);
			m_Reg_Sts.DL_CPRI[c].REx_Reset[3].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RE4Reset]);			
			
			for(k = 0; k < 20; k++)
			{
				// 2019.06.22			
				uVal32 = Read_FPGA_Reg(Reg_VSS_DL_Addr[c][k+Reg_VSS_DL_TDD_Slot_Number00]);
				m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[k].uD32 = Vss_Stable_Check(&Vss_DL_TDD_Slot_Num[c][k], uVal32, Reg_VSS_DL_TDD_Slot_Number00, c, k);
			}
			
			// 2019.05.13	
			m_Reg_Sts.DL_CPRI[c].UL_GainBackOff_Onoff.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_UL_GainBackOff_Onoff]);

			m_Reg_Sts.DL_CPRI[c].L1Reset.Data   = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_L1Reset]);
			m_Reg_Sts.DL_CPRI[c].Cell_ID.uD32    = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Cell_ID]);

			if(cnt_5_en)
			{
				m_Reg_Sts.DL_CPRI[c].gNB_ID_1st.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_1st_Byte]);
				m_Reg_Sts.DL_CPRI[c].gNB_ID_2nd.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_2nd_Byte]);		
				m_Reg_Sts.DL_CPRI[c].gNB_ID_3rd.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_3rd_Byte]);
				m_Reg_Sts.DL_CPRI[c].gNB_ID_4th.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_gNB_ID_4th_Byte]);		
				
				m_Reg_Sts.DL_CPRI[c].DUH_ID.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_DUH_ID]); // 2019.12.26
				
				m_Reg_Sts.DL_CPRI[c].PCI_1st_Byte.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_PCI_1st_Byte]);
				m_Reg_Sts.DL_CPRI[c].PCI_2nd_Byte.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_PCI_2nd_Byte]);
			}
			
			m_Reg_Sts.DL_CPRI[c].Base_IP_Address[0].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address1]);
			m_Reg_Sts.DL_CPRI[c].Base_IP_Address[1].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address2]);
			m_Reg_Sts.DL_CPRI[c].Base_IP_Address[2].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address3]);
			m_Reg_Sts.DL_CPRI[c].Base_IP_Address[3].uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Base_IP_Address4]);
			

			uVal32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_REC_Port_No]);
			m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD32 = Vss_Stable_Check(&Vss_DL_REC_Port_No[c], uVal32, Reg_VSS_DL_REC_Port_No, c, 0);	

			if(cnt_3_en)
			{
				m_Reg_Sts.DL_CPRI[c].CPRI_Version.uD32         = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_CPRI_Version_Z_2_0]);
				m_Reg_Sts.DL_CPRI[c].CPRI_Startup.uD32         = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_CPRI_Startup_Z_66_0]);
				m_Reg_Sts.DL_CPRI[c].CPRI_L1_Reset_LOS.uD32    = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_CPRI_L1_RST_LOS_Z_130_0]);
				m_Reg_Sts.DL_CPRI[c].Pointer_P.uD32            = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_Pointer_P_Z_194_0]);
			}

			m_Reg_Sts.DL_CPRI[c].L1Reset.Data     = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_L1Reset]);

			if(cnt_9_en)
			{
				m_Reg_Sts.DL_CPRI[c].BIP_Count.uD32   = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Count]);
				m_Reg_Sts.DL_CPRI[c].BIP_Count_1.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Count_1]);
				m_Reg_Sts.DL_CPRI[c].BIP_Count_2.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Count_2]);
				m_Reg_Sts.DL_CPRI[c].BIP_Count_3.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Count_3]);		
			}
			
			m_Reg_Sts.DL_CPRI[c].BIP_Clear.Data   = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Clear]);
			m_Reg_Sts.DL_CPRI[c].BIP_Err_Cnt.uD32 = Read_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_BIP_Err_Cnt]);		
		}
		//------------------------------------------	
		for(c = 0; c < CPRI_PATH; c++)
		{
			if(IS__CPRI_Lock(c)==0) continue; // 2019.09.09
			
			m_Reg_Sts.UL_CPRI[c].RE_SERDESLOS.uD32   = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RECSERDESLOS]);
			m_Reg_Sts.UL_CPRI[c].RE_LOFALARM.uD32    = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RECLOFALARM]);
			m_Reg_Sts.UL_CPRI[c].REFunctionFail.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_REFunctionFail]);
			
			m_Reg_Sts.UL_CPRI[c].RE_Reset_Ack.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Reset_Ack]);
			m_Reg_Sts.UL_CPRI[c].ALIVE_ACK_00.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_ALIVE_ACK_00]);
			m_Reg_Sts.UL_CPRI[c].Mismatch_Noti.uD32= Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_Mismatch_Noti]);				
			m_Reg_Sts.UL_CPRI[c].ALIVE_ACK_01.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_ALIVE_ACK_01]);
			
			m_Reg_Sts.UL_CPRI[c].RE_Cascade_ID_ACK.uD32      = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Cascade_ID_ACK]);
			m_Reg_Sts.UL_CPRI[c].RE_OpticLOSDetectAlarm.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE_OpticLOSDetectAlarm]);
			m_Reg_Sts.UL_CPRI[c].RE_Loopbackmode_ACK.uD32    = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Loopbackmode_ACK]);

			m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[0].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE1Reset]);
			m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[1].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE2Reset]);
			m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[2].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE3Reset]);
			m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[3].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE4Reset]);
			
			m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[0].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE1FunctionFail]);
			m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[1].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE2FunctionFail]);
			m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[2].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE3FunctionFail]);
			m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[3].uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE4FunctionFail]);

			if(cnt_3_en)
			{
				m_Reg_Sts.UL_CPRI[c].RE_Type_ID.uD32     = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Type_ID]);		
				m_Reg_Sts.UL_CPRI[c].RE_Vendor_ID.uD32   = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE_Vendor_ID]);

				m_Reg_Sts.UL_CPRI[c].CPRI_Version.uD32         = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_CPRI_Version_Z_2_0]);
				m_Reg_Sts.UL_CPRI[c].CPRI_Startup.uD32         = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_CPRI_Startup_Z_66_0]);
				m_Reg_Sts.UL_CPRI[c].CPRI_L1_Reset_LOS.uD32    = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_CPRI_L1_Reset_LOS_Z_130_0]);
				m_Reg_Sts.UL_CPRI[c].Pointer_P.uD32            = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_Pointer_P_Z_194_0]);
			}

			m_Reg_Sts.UL_CPRI[c].L1_Reset_ACK.Data = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_L1_Reset_ACK]);

			if(cnt_9_en)
			{
				m_Reg_Sts.UL_CPRI[c].BIP_Count.uD32   = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Count]);		
				m_Reg_Sts.UL_CPRI[c].BIP_Count_1.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Count_1]);
				m_Reg_Sts.UL_CPRI[c].BIP_Count_2.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Count_2]);
				m_Reg_Sts.UL_CPRI[c].BIP_Count_3.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Count_3]);
			}
			
			m_Reg_Sts.UL_CPRI[c].BIP_Clear.Data   = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Clear]);
			m_Reg_Sts.UL_CPRI[c].BIP_Err_Cnt.uD32 = Read_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_BIP_Err_Cnt]);
		}
	} // When VSS_Lock
//------------------------------------------------------------
// 'Mode General-TDD'
//------------------------------------------------------------	
	if(cnt_3_en) // check needed
	{	
		m_Reg_Sts.RW_TDD_Mode.Data = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_TDD_Mode]);
		
		for(k = 0; k < 20; k++)
			m_Reg_Sts.RW_Slot_Format[k].uD32 = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_Slot00_Format+k]);
	}
	if(cnt_5_en)
	{
		m_Reg_Sts.DL_ON_Guard_Time.Data    = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_DL_ON_Guard_Time]);
		m_Reg_Sts.UL_ON_Guard_Time.Data    = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_ON_Guard_Time]);	
		m_Reg_Sts.DL_to_UL_Guard_Time.Data = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_DL_to_UL_Guard_Time]);		
		m_Reg_Sts.UL_to_DL_Guard_Time.Data = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_to_DL_Guard_Time]);

		m_Reg_Sts.UL_GAIN_Guard_Time[0].uD32 = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time0]);
		m_Reg_Sts.UL_GAIN_Guard_Time[1].uD32 = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time1]);
		m_Reg_Sts.UL_GAIN_Guard_Time[2].uD32 = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time2]);
		m_Reg_Sts.UL_GAIN_Guard_Time[3].uD32 = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time3]);
		m_Reg_Sts.UL_GAIN_Guard_Time[4].uD32 = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Guard_Time4]);
		
		m_Reg_Sts.UL_GAIN_Measure_Time.uD32  = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_UL_GAIN_Measure_Time]);
	}
	
	if(cnt_3_en)
	{
		m_Reg_Sts.TSYNC_Delay.uD32    = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_TSYNC_Delay]);
		m_Reg_Sts.TSYNC_FAIL.Data     = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_R_TSYNC_Fail]);


		for(fa = 0; fa < MAX_Optic; fa++)	
		{
			m_Reg_Sts.DLY[fa].t_Offset.uD32       = Read_FPGA_Reg( Reg_TDD2_Addr[fa][Reg_TDD2_R_tOffset]);
			m_Reg_Sts.DLY[fa].TB_Delay_DL.uD32    = Read_FPGA_Reg( Reg_TDD2_Addr[fa][Reg_TDD2_R_TBDelayDL]);
			m_Reg_Sts.DLY[fa].TB_Delay_UL.uD32    = Read_FPGA_Reg( Reg_TDD2_Addr[fa][Reg_TDD2_R_TBDelayUL]);
			m_Reg_Sts.DLY[fa].Round_Trip_Dly.uD32 = Read_FPGA_Reg( Reg_TDD2_Addr[fa][Reg_TDD2_R_Round_Trip_Dly]);
			// 2019.05.23
			m_Reg_Sts.DLY[fa].T2A_Dly.uD32 = Read_FPGA_Reg( Reg_TDD2_Addr[fa][Reg_TDD2_R_T2A_Dly]);
			m_Reg_Sts.DLY[fa].TA3_Dly.uD32 = Read_FPGA_Reg( Reg_TDD2_Addr[fa][Reg_TDD2_R_TA3_Dly]);
		}
	}
//------------------------------------------------------------

	INT8U path;
	//------------------
	// CFR : PRU, AAU : 2019.04.15
	//------------------
	if(cnt_3_en)
	{
		m_Reg_Sts.CFR[0].Version.uD32 = Read_FPGA_Reg( Reg_CFR_Addr[0][Reg_CFR_Version]);

		for(path = 0; path < MAX_DL; path++)
		{
			m_Reg_Sts.CFR[path].OnOff.Data	= Read_FPGA_Reg( Reg_CFR_Addr[path][Reg_CFR_OnOff]);
			m_Reg_Sts.CFR[path].Level.uD32	= Read_FPGA_Reg( Reg_CFR_Addr[path][Reg_CFR_Level]);
		}
	}

//------------------------------------------------------------
#ifdef __TYPE__AAU__ // RF : AAU Only
//------------------------------------------------------------

	for(m = 0; m < MAX_MIMO_PATH; m++)
	{	
		// RF
		m_Reg_Sts.RFU[m].DL_Att.Data = Read_FPGA_Reg( Reg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_DL_ATT]);
		m_Reg_Sts.RFU[m].UL_Att.Data = Read_FPGA_Reg( Reg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_UL_ATT]);
		m_Reg_Sts.RFU[m].PA_OnOff.Data = Read_FPGA_Reg( Reg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_PA_OnOff]);
		
		// ALC		
		m_Reg_Sts.ALC[m].Def_Att.Data = Read_FPGA_Reg( Reg_ALC_Addr[m][Reg_ALC_RW_Default_ATT]); // UL AFE ATT
		m_Reg_Sts.ALC[m].ALC_Att.Data = Read_FPGA_Reg( Reg_ALC_Addr[m][Reg_ALC_R_ALC_ATT]);

		if(cnt_5_en)
		{
			m_Reg_Sts.ALC[m].ALC_Threshold.Data = Read_FPGA_Reg( Reg_ALC_Addr[m][Reg_ALC_RW_ALC_Threadhold]);
			m_Reg_Sts.ALC[m].Mode.Data    = Read_FPGA_Reg( Reg_ALC_Addr[m][Reg_ALC_RW_Mode]);
		}
	}

	// RF : AAU Only // Axi add0218
	//------------------------------------------------------------
	for(m = 0; m < CPRI_PATH; m++)
	{	
		m_Reg_Sts.AXI[m].Cpri_Sts.Data = Read_FPGA_Reg( Reg_AXI_CPRI_Addr[m][Reg_AXI_CPRI_R_Status]);
		m_Reg_Sts.AXI[m].Mscl_Status.Data = Read_FPGA_Reg( Reg_AXI_CPRI_Addr[m][Reg_AXI_CPRI_R_MiscAlm]);
	}

//------------------------------------------------------------	
#else // PRU
//------------------------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{	
		// RF
		m_Reg_Sts.RFU[m].PA_OnOff.Data = Read_FPGA_Reg( Reg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_PA_OnOff]); 
		m_Reg_Sts.RFU[m].LayerInfo.Data = Read_FPGA_Reg( Reg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_Layer_Info]); 		
	}
	//------------------
	// DPD : PRU Only
	//------------------
	m_Reg_Sts.DPD[0].Version.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[0][Reg_DPD_Version]);
	
	for(path = 0; path < MAX_DL; path++)
	{		
		m_Reg_Sts.DPD[path].OnOff.Data  = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_OnOff]);		
		m_Reg_Sts.DPD[path].Status.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_St]);
		m_Reg_Sts.DPD[path].Err_St.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Err_St]);
		m_Reg_Sts.DPD[path].Success_Cnt.uD32  = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Success_Cnt]);
		
		m_Reg_Sts.DPD[path].Input_Pwr.uD32  = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Input_Pwr]);
		m_Reg_Sts.DPD[path].Output_Pwr.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Output_Pwr]);		
		m_Reg_Sts.DPD[path].Eng_In_Pwr.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Eng_In_Pwr]);		
		m_Reg_Sts.DPD[path].Eng_Out_Pwr.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Eng_Out_Pwr]);	
		m_Reg_Sts.DPD[path].FB_Eng_In_Pwr.uD32 = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_FB_Eng_In_Pwr]);	
		
		m_Reg_Sts.DPD[path].FB_In_Pwr.uD32     = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_FB_In_Pwr]);
		m_Reg_Sts.DPD[path].DPD_Reset.uD32     = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_RESET]);		

		// 2019.05.20
		m_Reg_Sts.DPD[path].PeakExpansion.uD32  = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_PeakExpansion]);
		m_Reg_Sts.DPD[path].MSE.uD32        = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_MSE]);
		m_Reg_Sts.DPD[path].PRE_MSE.uD32    = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_PRE_MSE]);
		m_Reg_Sts.DPD[path].LUT_Value.uD32  = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_LUT_Value]);
		m_Reg_Sts.DPD[path].Counts.uD32     = Read_FPGA_Reg( Reg_DPD_Addr[path][Reg_DPD_Counts]);
	}
	
	// 2019.05.01
	m_Reg_Sts.DPD_Core_Reset.uD32 = Read_FPGA_Reg( Reg_DPD2_Addr[Reg_DPD_Core_RESET] );
	m_Reg_Sts.DPD_All_Reset.uD32  = Read_FPGA_Reg( Reg_DPD2_Addr[Reg_DPD_All_RESET] );

//------------------------------------------------------------
#endif
//------------------------------------------------------------
//------------------------------------------------------------
// Comp_OnOff add0218
//------------------------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
		m_Reg_Sts.COMP_ONOFF[m].Ctrl.Data = Read_FPGA_Reg( Reg_COMP_ONOFF_Addr[m]); // RF
}

INT8U IS_Alpha_Mhz_Existing(void)
{
	switch(m_Eep.d_RE_Channel_BW[Alpha])
	{
		case BW__40M :
		case BW__60M :
		case BW__80M :
		case BW_100M :
			return 1;
			
		default: 
			return 0;
	}
}

INT8U IS_Alpha_Mhz_Not_Existing(void)
{
	if(IS_Alpha_Mhz_Existing())
		return 0;
	else 
		return 1;
}


INT8U Get__BW_of_Alpha(void)
{
	INT8U bw_a;

	switch(m_Eep.d_RE_Channel_BW[Alpha])
	{	
		case BW_100M : bw_a = 100; break;
		case BW__40M : bw_a = 40; break;
		case BW__60M : bw_a = 60; break;
		case BW__80M : bw_a = 80; break;
		
		default : bw_a = 0; break; // not used.
	}
	return bw_a;
}


// 2020.01.29
INT8U IS__Vaild_CPRI__Main(void)
{
	if(Is_SFP_Inserted(CPRI_0)) return 1; // valid
	else return 0; // invalid
}

INT8U IS__Vaild_CPRI_Alpha(void)
{
	if(Is_SFP_Inserted(CPRI_1) && IS_Alpha_Mhz_Existing()) 
		return 1; // valid
	else return 0; // invalid
}


/*
----------------------------------------------------
	DL Main EQ0	11108	-
	DL Main EQ1	1110A	반전
	DL +a EQ0	12108	반전
	DL +a EQ1	1210A	-

	UL Main EQ0	11128	-
	UL Main EQ1	11133	반전
	UL +a EQ0	12128	반전
	UL +a EQ1	12133	-
----------------------------------------------------
*/
void AAU___EQ_Toggle_Set(void)
{
	m_St.EQ_Toggle[FA_0][DL][0] = 1;
	m_St.EQ_Toggle[FA_0][DL][1] = -1; // toggle
	m_St.EQ_Toggle[FA_1][DL][0] = -1; // toggle
	m_St.EQ_Toggle[FA_1][DL][1] = 1;
	
	m_St.EQ_Toggle[FA_0][UL][0] = 1;
	m_St.EQ_Toggle[FA_0][UL][1] = -1; // toggle
	m_St.EQ_Toggle[FA_1][UL][0] = -1; // toggle
	m_St.EQ_Toggle[FA_1][UL][1] = 1;	
}


// CPRI lock과 관계없이 Read
void Proc__Check_ClockFail(void)
{
	INT8U Pll_Lock;
	INT8U Pll_Init_Done;
	INT8U JESD_Lock;
	INT8U clock_alarm;
	INT8U cpri_lock_st;
	#ifdef __TYPE__AAU__ // AAU Only
	INT8U R_AFE_PLL_LD;
	#endif
	static INT8U cnt = 0;
	
	m_Reg_Sts.R_Board_Status.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R_Board_Status] );
	
	#ifdef __TYPE__AAU__ // AAU Only
	m_Reg_Sts.R_AFE_PLL_LD.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_R__AFE_PLL_LD] ); // 2019.05.29
	#endif	

	//--------------------------------------
	/*
	2019.04.03. v61.00.02(PRU)		
		SFP insert change : SFP Insert = fpga register '0' = AID 0xB43F (mount is '0')
	*/
	//--------------------------------------
	m_St.d_SFP_Insert_St[_100M][0] = (INT8U)(m_Reg_Sts.R_Board_Status.Optic1_Injection); // 2019.04.03. changed..
	m_St.d_SFP_Insert_St[_100M][1] = (INT8U)(m_Reg_Sts.R_Board_Status.Optic3_Injection); // 2019.04.03. changed..
	m_St.d_SFP_Insert_St[Alpha][0] = (INT8U)(m_Reg_Sts.R_Board_Status.Optic2_Injection); // 2019.04.03. changed..
	m_St.d_SFP_Insert_St[Alpha][1] = (INT8U)(m_Reg_Sts.R_Board_Status.Optic4_Injection); // 2019.04.03. changed..
	//------------------------------
	#ifdef __TYPE__PRU__
	if(m_St.AlmSt.A_0.AC == 0) // 2019.07.29
	#endif
	{
		if(m_St.AlmTest == 0) // 2019.09.02
		{
			m_St.AlmSt.A_3.SFP_Ins_0__ = Is_SFP_Inserted(CPRI_0)? 0:1;
		
			if(IS_Alpha_Mhz_Existing())
				m_St.AlmSt.A_3.SFP_Ins_0_a = Is_SFP_Inserted(CPRI_1)? 0:1;
			else
				m_St.AlmSt.A_3.SFP_Ins_0_a = 0; // 0(normal), MASK
		}
	}
	//------------------------------	
	
	
		
	
		

	/*
	//--------------------------------------
	// Check Clock Fail
	//--------------------------------------
	
	Register 0x07의 아래 1개라도 bit 값 0이면, 
	"fail 상태", 
	그렇지 않으면 "normal 상태"
	--------------------------	
		Pll_Lock	  (bit 0) 
		Pll_Init_Done (bit 1)
		JESD_Lock	  (bit 2)
	--------------------------	
	
	"fail 상태" 3s 유지 안되거나, "normal 상태" 3s 유지 안되면,
	Clock alarm은 변동 없음.('clock = alarm')

	"fail 상태" 3s 유지 시 
	{
		(1) Timer clear.
		(2) 순시 cpri lock 체크 
		lock 이면(만약 alpha 존재하면, alpha lock도 check 함) 최종 'clock = alarm'으로 변경.
		Unlock 이면, 다시 3s 측정 시작
	}	
	
	'clock = alarm' 상태에서, "normal 상태" 3s 유지 시, 
	{
		(1) Timer clear.
		(2) 'clock = normal' 로 변경 
	}
	
	*/
	
	#ifdef __TYPE__PRU__
	if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27
	#endif	
	{
		// 2019.12.24
		//------------------------------------------------------------------
			 if(m_Reg_Sts.R_Board_Status.Pll_Lock == 0)      clock_alarm = 1;
		else if(m_Reg_Sts.R_Board_Status.Pll_Init_Done == 0) clock_alarm = 1;
		else if(m_Reg_Sts.R_Board_Status.JESD_Lock == 0)     clock_alarm = 1;
		else 
		{
			clock_alarm = 0;
			#ifdef __TYPE__AAU__ // AAU Only
			if(m_Reg_Sts.R_AFE_PLL_LD.PLL_LD==0) clock_alarm = 1;
			#endif
		}
		/*
		last(m_St.AlmSt.A_4.Clock)
		---------+---------------------------+-------------------
		         |          Alarm            |     Normal 
		---------+---------------------------+-------------------
		3s Alarm | F/W : cpri 정상이면 A 설정 
		         |       cpri 알람이면 그대로  
                 +---------------------------+-------------------		         
		         | 동작: OK, Alarm 동작.     | OK (Alarm or Normal)
		---------+---------------------------+-------------------
		3s Normal| F/W : cpri 관계없이 Normal 설정 추가(2020.05.26)
		         | 즉, cpri alarm 인데, clock은 normal 일 수 있음.
		---------+---------------------------+-------------------
		
		*/
	
		// __2019_1226__Use_Alarm_Timer_Structure__	
		if(Chk_clock__alarm_normal_timeout(&m_St.AlmTime.A_4.Clock, m_St.AlmSt.A_4.Clock, clock_alarm ,3000, 3000)) // alarm
		{
			cpri_lock_st = Get_Instant_CPRI_Lock_St();

			/*
			2020.03.13
				정상 운영 중 clock fail 미발생
				현재 상태에서 CPRI가 알람 발생해도 Clock fail은 유지하도록
				
				[현 상태, 문제] : CPRI Lock 상태 때 clock fail 미발생, CPRI가 unlock 일때 clock fail 발생
				[수정필요]      : CPRI Lock 상태 때 Clock fail 발생해야함.  CPRI가 unlock 일때 Clock fail 유지
			*/

			if(Is_SFP_Inserted(CPRI_1) && IS_Alpha_Mhz_Existing()) // alpha 존재하면,
			{				
				if((cpri_lock_st & cbits[CPRI_0])&&(cpri_lock_st & cbits[CPRI_1])) // cpri0/1 을 체크. all cpri lock
				{
					m_St.AlmSt.A_4.Clock = 1;					
				}
			}
			else // alpha 존재(X)
			{				
				if(cpri_lock_st & cbits[CPRI_0]) // cpri0 을 체크. if cpri lock
				{
					m_St.AlmSt.A_4.Clock = 1;
				}
			}
		}
		else // normal
		{
			/*
			3s Normal : cpri 관계없이 Normal 설정 추가(2020.05.26)
		         즉, cpri alarm 인데, clock은 normal 일 수 있음.
			*/
			m_St.AlmSt.A_4.Clock = 0; // normal (2020.05.26)
		}

		#if 0
		cnt++;
		if(cnt > 20)
		{
			cnt = 0;
			print_agent(DEBUG_SYSTEM, VT_CYAN, "[Clock]-timer alm %d[ms]/clr %d[ms], last %d, tmp %d(reg 0x%X, 0x%X), Mask_at_Reboot %d)", 
						OSTimerGet_msec(&m_St.AlmTime.A_4.Clock.alm_ms), 
						OSTimerGet_msec(&m_St.AlmTime.A_4.Clock.clr_ms), 
						m_St.AlmSt.A_4.Clock,
						clock_alarm,
						m_Reg_Sts.R_Board_Status.Data,
						m_Reg_Sts.R_AFE_PLL_LD.Data,
						m_Env.CPRI_CLK_Alm_Mask_at_Reboot);
		}
		#endif
		if(m_Env.CPRI_CLK_Alm_Mask_at_Reboot) // When PRU Only
		{
			m_St.AlmSt.A_4.Clock = 0;
			Init__Alarm_Timer(&m_St.AlmTime.A_4.Clock);
		}
		//--------------------------------------------------------
		if(m_St.AlmTest && m_St.AlmFlag.A_4.Clock) // 2019.05.01
			m_St.AlmSt.A_4.Clock = m_St.AlmCtrl.A_4.Clock;
		else
		{
			if(m_St.AlmSt.A_4.Clock)
			{		
				cnt++;
				if(cnt >= 50) // every 100[ms] x 50
				{
					cnt = 0;
					
					Pll_Lock = m_Reg_Sts.R_Board_Status.Pll_Lock;
					Pll_Init_Done = m_Reg_Sts.R_Board_Status.Pll_Init_Done;
					JESD_Lock = m_Reg_Sts.R_Board_Status.JESD_Lock;				

			#ifdef __DEL__Unlock_Msg__
			// do nothing
			#else				
				#ifdef __TYPE__AAU__ // AAU Only
					R_AFE_PLL_LD = m_Reg_Sts.R_AFE_PLL_LD.PLL_LD;
					print_agent( BIT(1), VT_YELLOW,  "[RCU] Clock Fail (0 is fail) : pll(%d) pll_init_done(%d), jesd(%d), AFE_Pll_LD(%d).\n", 
							Pll_Lock, Pll_Init_Done, JESD_Lock, R_AFE_PLL_LD);				
				#else				
					print_agent( BIT(1), VT_YELLOW,  "[RCU] Clock Fail (0 is fail) : pll(%d) pll_init_done(%d), jesd(%d)\n", 
							Pll_Lock, Pll_Init_Done, JESD_Lock);					
				#endif		
			#endif				
				}		
			}
		}
	}
}
//-------------------------------------- 

/*
-------------------------------------------------------------------
2019.09.18 : PRU
	TSSI 레벨(Main/Alpha sum Power) ≥ -50dBm 경우에만 출력 하한 알람 Check
	그렇지 않으면 alarm masking
		(1) 100MHz Only 조건 시 100MHz TSSI만  참조
		(2) 100+αMHz 조건 시 100+α Total Power 진행
2019.09.20 : 5초유지 조건 추가.
2019.11.18 : 출력 하한 알람 : 5초유지 -> 10초유지로 변경
-------------------------------------------------------------------
*/
INT16S Get__PRU_TotPwr_x10_DL_TSSI(INT8U path4)
{
	double d_TSSI[MAX_Optic];
	INT16S tot_pwr_x10;

	if(Is_SFP_Inserted(CPRI_1)==0) 
		tot_pwr_x10 = m_St.d_DL_TSSI_dBm_x10[_100M][path4];
	else if(IS_Alpha_Mhz_Not_Existing())
		tot_pwr_x10 = m_St.d_DL_TSSI_dBm_x10[_100M][path4];
	else
	{	
		d_TSSI[_100M] = (double)(m_St.d_DL_TSSI_dBm_x10[_100M][path4])/10.0;
		d_TSSI[Alpha] = (double)(m_St.d_DL_TSSI_dBm_x10[Alpha][path4])/10.0;

		tot_pwr_x10 = 100*log10(pow(10.00, d_TSSI[_100M]/10.0)+pow(10.00, d_TSSI[Alpha]/10.0));
	}

	return tot_pwr_x10;
}

#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__
INT8U IS__Clock_CPRI_Alarm(void)
{
	INT8U cpri_alm = 0;	

	cpri_alm = m_St.AlmSt.A_2.CPRI_0__;	
	if(Is_SFP_Inserted(CPRI_1) && (IS_Alpha_Mhz_Existing()))
		cpri_alm |= m_St.AlmSt.A_2.CPRI_0_a;
	
	if(cpri_alm || m_St.AlmSt.A_4.Clock) return 1;
	else return 0;
}
#endif


/*
----------------------------------------------------------------
2019.11.29
(1) 출력 하한 알람 해제 조건에 TSSI 값 < -50dBm 조건은 제외하고 
(2)	하기 2가지 조건으로만 구현
	- "TSSI 하한알람" or "TX 출력 >= (출력하한설정+2)"
** "TX 출력 >= (출력하한설정+2)" 은 이미 기존에 구현됨.
----------------------------------------------------------------
*/
//////Hysterisis_Alm_Chk(&alarm, m_Eep.Fwd_Out_Lmt_x10[path4][LO], m_St.Fwd_Out_Pwr_x10[path4], 20 ); // Low limit
static OS_TIMER timer_PRU_Dl_Out_Low[AMPU_4_MAX];
INT8U Chk__PRU_DL_Out_Lower_Alarm(INT8U path4, INT8U alarm, INT8U alm_mask_req, INT8U chk_req)
{	
	INT16S tssi_x10;
	INT16S fwd_pwr_x10;
	static INT8U init[AMPU_4_MAX] = {0,0,0,0};
	INT8U alarm_100;
	INT8U alarm___A;
	INT8U alarm__tssi_low;

	if(path4 >= AMPU_4_MAX) return 0;

	if(init[path4] == 0)
	{
		init[path4] = 1;
		OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
	}
	// 2020.01.29 : 'AMP_Not_Use' -> 'AMP_Use' 시, 출력하한 알람 바로 뜨지 않도록.
	if(Check_PAU_Use(path4)==0)
	{
		OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
		return 0;
	}

	// 2020.01.29 :	amp on 되면서, 알람 발생하지 않도록 수정.
	if(AC_Fail_Ctrl_Req[path4] & _AC_F_AMP_REQ)
	{
		OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
		return 0;
	}
	
	#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__ // 2019.12.23, PRU
		if(IS__Clock_CPRI_Alarm())
			OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
	#endif
	//-------------------------------------------------------------------
	if(alm_mask_req)
	{
		OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
		return 0; // return normal
	}
	if(chk_req == 0)
	{
		OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
		return alarm; // return current status(alarm or normal)
	}
	//-------------------------------------------------------------------
	tssi_x10 = Get__PRU_TotPwr_x10_DL_TSSI(path4); // TSSI level
	fwd_pwr_x10 = m_St.Fwd_Out_Pwr_x10[path4];


	/* 
	-------------------------------------
	기본적으로 100M TSSI_Alarm을 참조하고, Alpha가 정상(Insert && BW_is_Normal)이면, Alpha 도 참조.
	//-------------------------------------
	*/
	
	alarm_100 = m_St.AlmSt.Path[path4].DL.TSSI_Lower__;
	alarm___A = m_St.AlmSt.Path[path4].DL.TSSI_Lower_a;
	
	if(IS__Vaild_CPRI_Alpha()) 
	{
		if(alarm_100 && alarm___A)
			alarm__tssi_low = 1;
		else
			alarm__tssi_low = 0;
	}
	else
	{
		alarm__tssi_low = alarm_100;
	}
	//-------------------------------------
	if(alarm) // now alarm
	{
		OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);		
		
		// 2020.01.29 : limit check 후에 tssi-low-alarm 비교를 해야 함.
		if(fwd_pwr_x10 >= (m_Eep.Fwd_Out_Lmt_x10[path4][LO]+20)) 
			return 0; // if "TX 출력 >= (출력하한설정+2)", normal
		else if(alarm__tssi_low)	
			return alarm; // 2020.01.29 : '출력하한 알람' 중, "TSSI 하한알람" 발생한 경우, '출력하한 알람' mask 하지 않도록 수정.	
		else 
			return 1; // alarm		
	}
	//-------------------------------------------------------------------
	else // now normal
	{
		if(tssi_x10 < (-46*10)) // 2020.01.29 : -50.0 -> -46.0
		{
			OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
			return 0; // normal
		}		
		else if(fwd_pwr_x10 < m_Eep.Fwd_Out_Lmt_x10[path4][LO])
		{
			/*
			PRU의 출력 하한 알람 유지 조건
			-------------------------------
			2019.11.18 : 5[s] -> 10[s]
			2020.02.19 : 10[s] -> 60[s]
			*/
			if(OSTimerGet_msec(&timer_PRU_Dl_Out_Low[path4]) > (60*TICK_CO_SEC)) 
				return 1; // alarm
			else
				return 0; // normal
		}
		else
		{
			OSTimerReset(&timer_PRU_Dl_Out_Low[path4]);
			return 0; // normal
		}
	}
	//-------------------------------------------------------------------
}




//-----------------------------------------


#ifdef __TYPE__AAU__ // AAU Only
static OS_TIMER timer_AAU_Dl_Out_Low[AMPU_4_MAX];
static OS_TIMER timer_AAU_Dl_Out_NRM[AMPU_4_MAX]; // Normal

INT8U Chk__AAU_DL_Out_Lower_Alarm(INT8U path4, INT8U alarm)
{	
	static INT8U init[AMPU_4_MAX] = {0,0,0,0};
	INT8U last_alm;
	INT8U temp_alm;

	if(path4 >= AMPU_4_MAX) return 0;

	if(init[path4] == 0)
	{
		init[path4] = 1;
		OSTimerReset(&timer_AAU_Dl_Out_Low[path4]);
		OSTimerReset(&timer_AAU_Dl_Out_NRM[path4]);
	}

	
#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__ // 2019.12.23, AAU
	if(IS__Clock_CPRI_Alarm())
	{
		OSTimerReset(&timer_AAU_Dl_Out_Low[path4]);
		OSTimerReset(&timer_AAU_Dl_Out_NRM[path4]);
	}
#endif

	last_alm = (m_St.AlmSt.Path[path4].DL.Out_Lower > 0)? 1:0;
	temp_alm = (alarm > 0)? 1:0;

	if(temp_alm == last_alm) // same status
	{
		OSTimerReset(&timer_AAU_Dl_Out_Low[path4]);
		OSTimerReset(&timer_AAU_Dl_Out_NRM[path4]);
		
		return temp_alm;
	}
	else if(temp_alm) // for last.normal -> last.alarm
	{
		OSTimerReset(&timer_AAU_Dl_Out_NRM[path4]);
		if(OSTimerGet_msec(&timer_AAU_Dl_Out_Low[path4]) > (10*TICK_CO_SEC))
		{
			OSTimerReset(&timer_AAU_Dl_Out_Low[path4]);
			return 1; // alarm
		}
		else 
		{
			return 0; // normal
		}
	}
	else // for last.alarm -> last.normal
	{
		OSTimerReset(&timer_AAU_Dl_Out_Low[path4]);
		if(OSTimerGet_msec(&timer_AAU_Dl_Out_NRM[path4]) > TICK_CO_SEC)
		{
			OSTimerReset(&timer_AAU_Dl_Out_NRM[path4]);
			return 0; // normal
		}
		else 
		{
			return 1; // alarm
		}
	}
}
#endif



#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__ // PRU/AAU
/*
INT8U IS__Clock_CPRI_Alm(INT8U cpri) // 미사용. 불필요. from 2020.02.17
{
	INT8U cpri_alm = 0;

	switch(cpri)
	{	
		case CPRI_0 : cpri_alm = m_St.AlmSt.A_2.CPRI_0__; break;
		case CPRI_1 : cpri_alm = m_St.AlmSt.A_2.CPRI_0_a; break;
		case CPRI_2 : cpri_alm = m_St.AlmSt.A_2.CPRI_1__; break;
		case CPRI_3 : cpri_alm = m_St.AlmSt.A_2.CPRI_1_a; break;

		default: return 0;		
	}
	
	if(cpri_alm || m_St.AlmSt.A_4.Clock) return 1;
	else return 0;
}
*/
#endif


/*
---------------------------------------
2019.12.04 : SFP LOS/LD 알람 보고시 PD/LD 파워 값이 정상인 오류 개선

	SFP LOS 알람 구현 변경 : LOS 알람이 2초 유지시 PD 값 참조
	SFP LD 알람 구현 변경 : LD 알람이 2초 유지시 LD 값 참조

--> 결국 알람 2초 유지되어야 실제 알람으로 판단하고, 
	당시의 PD/LD값을 참조 한다는 의미...
---------------------------------------
*/

enum
{
	A_LOS = 0,
	A__LD,	
	LOS_LD // Max
};

#define CPRI_MAX (4)
static OS_TIMER Timer_Keep_LosLd_Alm[CPRI_MAX][LOS_LD]; // alarm 유지시간.
INT8U Chk__SFP_Los_Ld_Alarm(INT8U losld, INT8U cpri, INT8U alm_temp, INT8U alm_curr)
{
	static INT8U init[CPRI_MAX] = {0,0,0,0};
	INT8U tmp_alm; 
	INT8U cur_alm;
	//----------------------------------------
	if(m_St.AlmTest) return 0;	
	if(cpri >= CPRI_MAX) return 0;
	if(losld >= LOS_LD) return 0;
	//----------------------------------------	
	tmp_alm = (alm_temp > 0)? 1:0;
	cur_alm = (alm_curr > 0)? 1:0;
	//----------------------------------------
	if(init[cpri] == 0)
	{
		init[cpri] = 1;		
		OSTimerReset(&Timer_Keep_LosLd_Alm[cpri][losld]);

		return cur_alm;
	}
	//----------------------------------------
	#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__ // PRU/AAU	
	/*
	2020.02.17 : 
	SFP LD/PD 알람 : 'clock alarm' or 'cpri alarm' 이면, 알람 clear 하는 조건에서 'cpri alarm' 조건은 삭제.	
	
	//if(IS__Clock_CPRI_Alm(cpri))
	//{
	//	OSTimerReset(&Timer_Keep_LosLd_Alm[cpri][losld]);
	//	return 0; // changed to normal, 2020.02.14
	//}
	*/
	if(m_St.AlmSt.A_4.Clock)
	{
		OSTimerReset(&Timer_Keep_LosLd_Alm[cpri][losld]);
		return 0; // changed to normal, 2020.02.14
	}
	#endif
	//----------------------------------------
	if(tmp_alm) // Now Alarm	
	{
		if(cur_alm) return cur_alm; // both alarm, not changed
		else // checking normal -> alarm
		{
			if(OSTimerGet_msec(&Timer_Keep_LosLd_Alm[cpri][losld]) > (2*TICK_CO_SEC))
			{
				OSTimerReset(&Timer_Keep_LosLd_Alm[cpri][losld]);
				return 1; // changed to alarm
			}
			else return cur_alm; // not changed, normal
		}
	}
	else // normal
	{
		OSTimerReset(&Timer_Keep_LosLd_Alm[cpri][losld]);
		return 0; // changed to normal
	}
	//----------------------------------------
}









//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
#define PRU_After_Filter_Offset_x10 (148) // +14.8[dB], 2019.05.28
#define PRU_Noise_Offset_x10 (-176) // -17.6[dB], 2019.05.28

void Proc_Conv_DTU_Reg_to_mSt(void) //
{
	INT32U RSSI_32, uVal32;
	INT32S sVal32;
	INT16S RSSI_dBFs_x10;
	INT8U  optic, path4;
	INT8U  path;
	INT8U  c, k, fa, m;
	INT8U  uVal8;
	INT16U uVal16;	
	double d_val;
	type32 Val_32;	
	INT16S pwr_x10;
	#ifdef __TYPE__AAU__	
	INT16S att_x10;	
	INT16S sVal16;	
	INT16S val_x10;
	#else	
	INT32S diff32;		
	INT16U bal_att_x10;	
	#endif
	
	
	
	
	//--------------------------------------------------------------
	// ANTS Register -> m_St.g__________
	//--------------------------------------------------------------

	Val_32.uD32 = htonl(m_Reg_Sts.R_SW_Ver.uD32);
	m_St.PRU_DTU_TempComp = Val_32.uD8[0] & cbits[0]; // 2019.12.18 : ON is 0 @ both AID and Register bit.24.
	memcpy(m_St.d_SW_VER_DTU,  &Val_32.uD8[1], 3); // 3byte only
		

	Val_32.uD32 = htonl(m_Reg_Sts.R_HW_Ver.uD32);
	memcpy(m_St.d_SW_VER_FPGA, &Val_32.uD8[1], 3); // 3byte only
	

	
	#ifdef __TYPE__PRU__	
	uVal16 = (INT16U)m_Reg_Sts.R_FPGA_Temp.uD32;
	uVal16 >>= 4;
	// temp = (reg * 503.9)/4095 - 273.1
	m_St.d_R_FPGA_Temp_x10 = (INT16S)(uVal16*5039/4095 - 2731);
	#else
	m_St.d_R_FPGA_Temp_x10 = (INT16S)(m_Reg_Sts.R_FPGA_Temp.sD32);
	#endif
	//----------------------------------------
	/*		
		m_Reg_Sts.R_App_MB_Cnter.uD32;
		m_Reg_Sts.R_FPGA_Counter.uD32;	
		m_Reg_Sts.R_Board_INFO.uD32;
		
		m_Reg_Sts.RW_ETH_Path_Sel.Data;		
		m_Reg_Sts.RW_Mapper_Loopback.Data;	
		m_Reg_Sts.RW_RF_Loopback.Data;	
		
		m_Reg_Sts.RW_Interrupt_mask0.Data;
		m_Reg_Sts.RW_Interrupt_mask1.Data;
		m_Reg_Sts.RW_Interrupt_Status0.Data;
		m_Reg_Sts.RW_Interrupt_Status1.Data;
	*/	
	// 2019.04.06 added
	#ifdef __TYPE__PRU__
	m_St.d_CPRI_Recovery_Path_St = m_Reg_Sts.RW_Recovery_Clock_Set.Data;
	#else // AAU : use bit31, 2019.08.24
	m_St.d_CPRI_Recovery_Path_St = (m_Reg_Sts.RW_Recovery_Clock_Set.Data & 0x80000000)? 1:0;
	#endif
	

	     if(m_Reg_Sts.Gain_Check_Mode.dl_mode_0 == 0) m_St.DL_Mode_En = 1; // OFF
	else if(m_Reg_Sts.Gain_Check_Mode.dl_mode_1 == 0) m_St.DL_Mode_En = 1; // OFF
	else                                              m_St.DL_Mode_En = 0; // ON
	//--------------------------------------

	
	/*	
	SFP Insert Status	
	0xB43F	S	Local	1	
	"Data type: 0x07(boolean)
	Data Step: 0x00(미사용)
	0x00: Mount, 0x01 : Unmount"
	
		[31:20] : Reserved [19:16] : Optic Injection, None = '0', Injection = '1' 			
		[19:16] : CPRI	#4 ~ #1 Optic 
		[15:3] : Reserved 
		[2] : JESD Lock, Lock ='1', Unlock ='0' 
		[1] : PLL/AFE Init Done = '1', NotDone = '0' 
		[0] : PLL Lock, Lock = '1', Fail = '0'"
	*/

	
	

	for(optic = 0; optic < MAX_Optic; optic++)	
	{
		for(path4 = 0; path4 < 4; path4++)
		{
		#ifdef __TYPE__PRU__ // ANTS-PRU, 2019.05.08
			bal_att_x10 = m_Reg_Flg.General_FA[optic].General_FA_MIMO[path4].RW_Height_Bal_Gain.uD32;
			m_St.d_DL_TSSI_BalAtt_x10[optic][path4] = bal_att_x10;
			
			m_St.d_Att_Dig_DL_x10[optic][path4] = (INT16U)m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_DL_Gain.uD32;
			
			uVal16 = (INT16U)m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32;
			m_St.d_Att_Dig_UL_x10[optic][path4] = uVal16 - m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4];
			m_St.d_Att_Dig_UL_x10[optic][path4] -= m_St.d_UL_GainBal_x10;

			//D_Printf(DEBUG_SYSTEM, "[RCU] optic %d, path %d, UL_ATT(%d) + ul-backoff(%d) = (%d)\n", 
			//	optic, path4, 
			//	m_St.d_Att_Dig_UL_x10[optic][path4],
			//	m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4],
			//	uVal16);

			m_St.d_Gain_DL_x10[optic][path4] = 0; // not used @ PRU
			m_St.d_Gain_UL_x10[optic][path4] = 0; // not used @ PRU			
		#else  // ANTS-AAU
			//--- DL
			uVal32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_DL_Gain.uD32;
			//----------------------------------------
			sVal16 = Get__AAU_DL_Gain_Att_x10(optic, path4);
			//----------------------------------------
			m_St.d_Att_Dig_DL_x10[optic][path4] = conv_Reg_Gain_to___Att_x10(DL, uVal32) - sVal16;			
			m_St.d_Gain_DL_x10[optic][path4] = Conv_Gain_from_Att(m_St.d_Att_Dig_DL_x10[optic][path4]);
			
			//--- UL
			//----------------------------------------
			uVal32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32;
			att_x10 = conv_Reg_Gain_to___Att_x10(UL, uVal32);			
			val_x10 = Get__AAU_UL_Gain_Att_x10(optic, path4);
			val_x10 -= m_Eep.d_Att_Dig_UL_Offs_x10[optic][path4]; // ???
			sVal16 = att_x10 - val_x10;			
			m_St.d_Att_Dig_UL_x10[optic][path4] = sVal16;			
			m_St.d_Gain_UL_x10[optic][path4] = Conv_Gain_from_Att(m_St.d_Att_Dig_UL_x10[optic][path4]);
		#endif
		}
	}
	//--------------------------------------------------------------
	
	
	//----------------------------------------
	for(optic = 0; optic < MAX_Optic; optic++)	
	{
		for(path4 = 0; path4 < 4; path4++)
		{	
			/*
			RSSI_dBFs_x10 
				= 100.0*log10(RSSI_32/pow(2.00, 31.0))
				= 100.0*log10(RSSI_32/2147483648)
				= 100.0*log10(RSSI_32) - 100.0*log10(2147483648)
				= 100.0*log10(RSSI_32) - 100.0*log10(2147483648)
				= 100.0*log10(RSSI_32) - 100.0*9.33
				= 100.0*log10(RSSI_32) - 933
			*/			

		//------------------------------------------------------------
		#ifdef __TYPE__PRU__
		//------------------------------------------------------------

			/*			
				2019.07.12 : PRU : TSSI DEC. 값 OFFSET 처리
				DTU에서 올려준 값에 "× 0.74"처리 요청(dBm 계산도 x0.74 처리 한 DEC. 값으로 계산 진행)
			*/
			// DL RMS -----------------------------------------------------			
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_DL_TSSI.uD32;
			d_val = 100.0*log10((double)RSSI_32*0.74);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;

			pwr_x10 = RSSI_dBFs_x10 + m_Eep.TSSI_Offset_x10[optic][path4]; // 2019.05.11
			uVal32 = (INT32U)((double)RSSI_32*0.74);

			if(optic == _100M)
			{
				if(pwr_x10 < -700) pwr_x10 = -900; // 무신호 시 레벨 표시, 2020.01.13 : -65.0 -> -70.0
				if(uVal32 < 50) uVal32 = 0; // 무신호 시 레벨 표시
			}
			else // Alpha
			{
				if(IS_Alpha_Mhz_Existing())
				{
					if(pwr_x10 < -700) pwr_x10 = -900; // 무신호 시 레벨 표시, 2020.01.13 : -65.0 -> -70.0
					if(uVal32 < 50) uVal32 = 0; // 무신호 시 레벨 표시
				}
				else
				{
					pwr_x10 = -900; // 무신호 시 레벨 표시
					uVal32 = 0; // 무신호 시 레벨 표시
				}
			}	
			m_St.d_DL_TSSI_dBm_x10[optic][path4] = pwr_x10;			
			m_St.d_DL_TSSI_dec[optic][path4] = uVal32; // PRU

			// UL RMS -----------------------------------------------------
			/*			
			RSSI(Filter 후)(0xB303) = 
				RSSI DET - 상수(+?dB) + DTU Atten(0x5206) 
				+ GUI Offset(0xB307)
				+ Gain Back OFF(0x5209) 
				+ Gain Balance(0x5209)
				+ after_filter_offset(14.8)	+ (PAU Atten(0xB224, 0xB225) - DTU Gain 보상(0xB304)) 			
			*/
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_UL_RSSI.uD32;			
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;
			RSSI_dBFs_x10 -= (370 - m_St.d_UL_Att_x10[optic][path4]); // 2019.04.25
			RSSI_dBFs_x10 += (m_Eep.RSSI_Offset_x10[optic][path4] + PRU_After_Filter_Offset_x10); // 2019.05.10
			RSSI_dBFs_x10 += m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4]; // 2019.06.09
			RSSI_dBFs_x10 += m_St.d_UL_GainBal_x10; // 2019.06.09
			/*
			2019.08.27 : PAU Att : ATTEN(LNA)(0xB224), ATTEN(LNA ALC)(0xB225), DTU Gain 보상(0xB304))
			*/
			RSSI_dBFs_x10 += 5*(m_St.Rvs_Att_x2[path4] + m_St.RX_ALC_Att_x2[path4]);
			RSSI_dBFs_x10 -= m_St.UL_DTU_Gain_Comp_x10[optic][path4];
			
			
			pwr_x10 = RSSI_dBFs_x10;
			m_St.d_UL_RSSI_dec[optic][path4] = RSSI_32;
			if(optic == _100M)
			{
				if(pwr_x10 < -920) pwr_x10 = -1000; // 무신호 시 레벨 표시, 2019.08.24 : -80.0 -> -92.0
			}
			else // Alpha
			{
				if(IS_Alpha_Mhz_Existing())
				{
					if(pwr_x10 < -920) pwr_x10 = -1000; // 무신호 시 레벨 표시, 2019.08.24 : -80.0 -> -92.0
				}
				else
				{
					pwr_x10 = -1000; // 무신호 시 레벨 표시
				}
			}
			m_St.d_UL_RSSI_dBm_x10[optic][path4] = pwr_x10;
			//---------------------------------------

			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_TSSI_SD_Level_short.uD32;
			m_St.d_TSSI_SD_Level_x10[optic][path4][0] = RSSI_32;
			//---------------------------------------
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_TSSI_SD_Level__long.uD32;
			m_St.d_TSSI_SD_Level_x10[optic][path4][1] = RSSI_32;
			//---------------------------------------
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_TSSI_SD_Level___Low.uD32;
			m_St.d_TSSI_SD_Level_x10[optic][path4][2] = RSSI_32;
		//------------------------------------------------------------ 
		#else // AAU
		//------------------------------------------------------------ 
			// DL RMS -----------------------------------------------------			
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_DL_TSSI.uD32;
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;

			pwr_x10 = RSSI_dBFs_x10 + m_Eep.TSSI_Offset_x10[optic][path4]; // 2019.05.11
			uVal32 = RSSI_32;

			if(optic == _100M)
			{
				if(pwr_x10 < -700) pwr_x10 = -900; // 무신호 시 레벨 표시, 2020.01.13 : -65.0 -> -70.0
				if(uVal32 < 50) uVal32 = 0; // 무신호 시 레벨 표시
			}
			else // Alpha
			{
				if(IS_Alpha_Mhz_Existing())
				{
					if(pwr_x10 < -700) pwr_x10 = -900; // 무신호 시 레벨 표시, 2020.01.13 : -65.0 -> -70.0
					if(uVal32 < 50) uVal32 = 0; // 무신호 시 레벨 표시
				}
				else
				{
					pwr_x10 = -900; // 무신호 시 레벨 표시
					uVal32 = 0; // 무신호 시 레벨 표시
				}
			}
			m_St.d_DL_TSSI_dBm_x10[optic][path4] = pwr_x10;
			m_St.d_DL_TSSI_dec[optic][path4] = uVal32;
			
			/*
			-------------------------------------------
				2019.08.23 : AAU, 
				'R_DL_RSSI_Peak' : TSSI-Decimal-before-EQ
			-------------------------------------------	
			*/
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_DL_RSSI_Peak.uD32; 
			sVal32 = RSSI_32 + m_Eep.TSSI_Dec_Offset[optic][path4]; // use offset @ AAU Only
			// 2019.09.18 : if minus value, do not use offset(AID 0xC06F)
			if(sVal32 < 0) 
				m_St.d_DL_TSSI_dec_Pre_EQ[optic][path4] = RSSI_32;
			else 
				m_St.d_DL_TSSI_dec_Pre_EQ[optic][path4] = sVal32;
			
			/*
			UL RMS 
			-----------------------------------------------------
			UL_RSSI_dBm 
				= 10log(reg_UL_RSSI) - 93.3 + RSS_Offset 
				+ Gain_backoff + Gain_Balance + UL_Att(0x5206 User) 
				- Gain_Compensation
				+ Att_Dig_UL_User // 2019.11.28 added
				- Curr_LS_Gain_Bal // 2019.11.28 added
				- UL_DTU_Gain_Compensation(0xB304)

				무신호시 레벨표시(-100.0dBm).
			-----------------------------------------------------	
			*/
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_UL_RSSI.uD32;
			m_St.d_UL_RSSI_dec[optic][path4] = RSSI_32;
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;
			RSSI_dBFs_x10 += m_Eep.RSSI_Offset_x10[optic][path4];
			RSSI_dBFs_x10 += m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4]; // 2019.06.09
			RSSI_dBFs_x10 += m_St.d_UL_GainBal_x10; // 2019.06.09
			RSSI_dBFs_x10 += m_Eep.d_Att_Dig_UL_User_x10[optic][path4]; // 2019.11.28
			RSSI_dBFs_x10 -= m_St.Curr_LS_Gain_Bal_x10[optic][UL][path4]; // 2019.11.28
			m_St.d_UL_RSSI_dBm_Alc_x10[optic][path4] = RSSI_dBFs_x10;
			
			RSSI_dBFs_x10 += conv_att_025_to_001(m_St.UL_Att[path4])/10; // 2019.07.18
			RSSI_dBFs_x10 -= m_St.UL_DTU_Gain_Comp_x10[optic][path4]; // 2019.07.18			
			pwr_x10 = RSSI_dBFs_x10;			
			
			if(optic == _100M)
			{
				if(pwr_x10 < -920) pwr_x10 = -1000; // 무신호 시 레벨 표시, 2019.08.24 : -80.0 -> -92.0
			}
			else // Alpha
			{
				if(IS_Alpha_Mhz_Existing())
				{
					if(pwr_x10 < -920) pwr_x10 = -1000; // 무신호 시 레벨 표시, 2019.08.24 : -80.0 -> -92.0
				}
				else
				{
					pwr_x10 = -1000; // 무신호 시 레벨 표시
				}
			}
			m_St.d_UL_RSSI_dBm_x10[optic][path4] = pwr_x10;
		#endif
		//------------------------------------------------------------ AAU
			


			m_St.d_Test_Pattern_Band_Gain[optic][path4] = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_Test_Pattern_Band_Gain.uD32;

			// gREMS Update
			Update_Stat( &m_Stat.d_DL_TSSI_dBm_x10[optic][path4], m_St.d_DL_TSSI_dBm_x10[optic][path4]);
			// 2019.08.12 : "DL_TSSI_Decimal" is not stat.			
			Update_Stat( &m_Stat.d_UL_RSSI_dBm_x10[optic][path4], m_St.d_UL_RSSI_dBm_x10[optic][path4] );
			
			// DL Peak -----------------------------------------------------			
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_DL_RSSI_Peak.uD32;
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;
			m_St.d_DL_TSSI_dBm_peak_x10[optic][path4] = RSSI_dBFs_x10;
			m_St.d_DL_TSSI_dec_peak_x10[optic][path4] = RSSI_32;			
			
			// UL Peak -----------------------------------------------------			
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_UL_RSSI_Peak.uD32;			
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;
			m_St.d_UL_RSSI_dBm_peak_x10[optic][path4] = RSSI_dBFs_x10;
			m_St.d_UL_RSSI_dec_peak_x10[optic][path4] = RSSI_32;

			// DL FB -----------------------------------------------------
			// 2019.04.29
			
		#ifdef __TYPE__PRU__
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_FB_RSSI_RMS.uD32;
		#else
			// 2019.09.05 : R_FB_RSSI_RMS -->> R_FB_RSSI_Peak
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_FB_RSSI_Peak.uD32; 
			//#ifdef __AAU_TEST__R_FB_RSSI_RMS__
			//RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_FB_RSSI_RMS.uD32;
			//#endif
		#endif
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;
			m_St.d_DL_FB_RSSI_x10[optic][path4] = RSSI_dBFs_x10 + m_Eep.DL_FB_Offset_x10[optic][path4];

			//-----------------------------------------------------			
			m_St.d_TSSI_SD_St[optic][path4] = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_TSSI_SD.Status;

			// UL Noise RMS -----------------------------------------------------
			// 2019.05.14			
		#ifdef __TYPE__PRU__
			/*			
			RSSI Noise(0xB306) = 
				RSSI DET - 상수(+?dB) + GUI Offset(0xC05C) 
				+( PAU Atten(0xB224, 0xB225) - DTU Gain 보상(0xB304) )
				+ DTU Atten(0x5206) + Gain Back OFF(0x5209) + Gain Balance(0x5209)
			*/
			RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_UL_NOISE_RMS.uD32;
			d_val = 100.0*log10((double)RSSI_32);
			RSSI_dBFs_x10 = (INT16S)d_val - 933;
			RSSI_dBFs_x10 += (m_Eep.UL_Noise_Offset_x10[optic][path4] + PRU_Noise_Offset_x10);
			/*
			2019.08.27 : PAU Att : ATTEN(LNA)(0xB224), ATTEN(LNA ALC)(0xB225)
			*/
			RSSI_dBFs_x10 += 5*(m_St.Rvs_Att_x2[path4] + m_St.RX_ALC_Att_x2[path4]);
			RSSI_dBFs_x10 -= m_St.UL_DTU_Gain_Comp_x10[optic][path4]; // DTU Gain 보상(0xB304)
			RSSI_dBFs_x10 += m_St.d_UL_Att_x10[optic][path4]; // DTU Atten(0x5206)
			RSSI_dBFs_x10 += m_Eep.RSSI_Offset_x10[optic][path4];
			RSSI_dBFs_x10 += PRU_After_Filter_Offset_x10; // 2019.05.10
			RSSI_dBFs_x10 += m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4];
			RSSI_dBFs_x10 += m_St.d_UL_GainBal_x10;


			if(optic == _100M) // 무신호 시 레벨 표시, 2019.08.27
			{
				if(RSSI_dBFs_x10 < -920) RSSI_dBFs_x10 = -1000; 
			}
			else // Alpha
			{
				if(IS_Alpha_Mhz_Existing())
				{
					if(RSSI_dBFs_x10 < -920) RSSI_dBFs_x10 = -1000; 
				}
				else
					RSSI_dBFs_x10 = -1000; // 무신호 시 레벨 표시				
			}
			m_St.d_UL_Noise_RMS_x10[optic][path4] = RSSI_dBFs_x10;	
		#else // AAU						
			if(m_Reg_Sts.Gain_Check_Mode.onoff) // UL Gain Check Mode
			{
				/*	// 2019.05.31 :
					 GC_RSSI_RMS = 레지스터값 + (44.0 - DL_Gain(출력-TSSI) + UL_Atten)
					 GC_RSSI_RMS +=  offset;
				*/
				// 2019.06.26 : register 분리 : Noise / UL GC Detect 측정값 Register 분리
				/*
				//RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_GC_RSSI_RMS.uD32;
				//d_val = 100.0*log10((double)RSSI_32);
				//RSSI_dBFs_x10 = (INT16S)d_val;				
				//RSSI_dBFs_x10 += 440; // 44.0[dB]
				//RSSI_dBFs_x10 -= (m_St.DL_OutPwr_Cal_x10[optic][path4] - m_St.d_DL_TSSI_dBm_x10[optic][path4]); // DL Gain(출력-TSSI)
				//RSSI_dBFs_x10 += m_St.d_UL_Att_x10[optic][path4];
				//RSSI_dBFs_x10 += m_Eep.UL_GC_RSSI_Offset_x10[_100M][path4]; // offset
				//m_St.d_R_GC_RSSI_RMS_x10[optic][path4] = RSSI_dBFs_x10;
				*/

				/*
				* 2019.08.09. 계산식 변경.
				GC_RSSI_RMS 레지스터 값 + 70 + DL ATTN(AID : 51 06) + UL ATTN(AID : B2 24)

				GC_RSSI_RMS = 레지스터값 + 70.0 + DL ATTN + UL_Atten
				GC_RSSI_RMS +=  offset;
				*/
				RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_GC_RSSI_RMS.uD32;
				d_val = 100.0*log10((double)RSSI_32);
				RSSI_dBFs_x10 = (INT16S)d_val - 933;
				RSSI_dBFs_x10 += 700; // 70.0[dB]

				/*
				2019.09.16 : 
					GC_RSSI_RMS 표시 값을 기존에 DL/UL RF Atten.을 더하여 표시 하였는데 
					Atten. 값을 더하는 부분 삭제
				---------------------------------------
				//uVal8 = conv_att_025_to_05(m_St.DL_Att[path4]);
				//RSSI_dBFs_x10 += conv_att_05_to_01(uVal8);				
				//uVal8 = conv_att_025_to_05(m_St.UL_Att[path4]);
				//RSSI_dBFs_x10 += conv_att_05_to_01(uVal8);
				*/				
				RSSI_dBFs_x10 += m_Eep.UL_GC_RSSI_Offset_x10[_100M][path4]; // offset
				/*
				------------------
				2019.12.04 : 
					GC_RSSI_RMS 계산에 Height Balance와 LS Gain Balance 를 추가.		
				2019.12.05 :
					GC_RSSI_RMS = "register Read" + "Ul_Gain_Low_Offset(0xC05D)" + "Height_Bal" - "L/S_Gain_Bal"
				2019.12.09 :
					GC_RSSI_RMS = "register Read" + "Ul_Gain_Low_Offset(0xC05D)" + "Height_Bal" - "L/S_Gain_Bal(DL Only)"
				------------------
				*/
				if(m_Eep.LS_Gain_Bal_OnOff) // 2019.12.09 : When LS_Gain_Bal_On only
				{
					RSSI_dBFs_x10 -= m_St.Curr_LS_Gain_Bal_x10[_100M][DL][path4];
				}
				RSSI_dBFs_x10 += m_St.d_DL_TSSI_BalAtt_x10[_100M][path4];
				//------------------
				m_St.d_R_GC_RSSI_RMS_x10[optic][path4] = RSSI_dBFs_x10;
			}
			else if(m_Reg_Sts.Gain_Check_Mode.noise_det_mode) // Noise Det Mode			
			{
				// 2019.06.26 : register 분리 : Noise / UL GC Detect 측정값 Register 분리
				RSSI_32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].R_UL_NOISE_RMS.uD32;
				d_val = 100.0*log10((double)RSSI_32);
				RSSI_dBFs_x10 = (INT16S)d_val - 933;
				m_St.d_UL_Noise_RMS_x10[optic][path4] = RSSI_dBFs_x10 + m_Eep.UL_Noise_Offset_x10[optic][path4];
			}			
		#endif
		} // PATH_4
	} // OPTIC
	
	
	for(path4 = 0; path4 < 4; path4++)
	{
	#ifdef __TYPE__PRU__
		RSSI_32 = m_Reg_Sts.DSP_Common_MIMO[path4].R_UL_IN_RSSI.uD32;
		d_val = 100.0*log10((double)RSSI_32);
		RSSI_dBFs_x10 = (INT16S)d_val - 933;
		m_St.d_UL_IN_RSSI_dBm_x10[path4] = RSSI_dBFs_x10;

		m_St.d_Att_DL_SUM[path4] = m_Reg_Sts.DSP_Common_MIMO[path4].RW_DL_Sum_Gain.uD32;
	#endif
	}	

	#ifdef __TYPE__AAU__
	Proc_DL_OutPwr_Cal(); // 2019.04.29
	#endif

	//------------------------------------------------------------
	// 'Mode General-TDD'
	//------------------------------------------------------------			
	m_St.d_RW_TDD_Mode = m_Reg_Sts.RW_TDD_Mode.Mode;

	if(m_Eep.RW_TDD_Mode != m_Reg_Sts.RW_TDD_Mode.Mode)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "m_Eep.RW_TDD_Mode %d != m_St.d_RW_TDD_Mode %d", 
			m_Eep.RW_TDD_Mode, m_St.d_RW_TDD_Mode);
	}

#ifdef __TYPE__AAU__ // 2019.04.17
	m_St.d_DL_ON_Guard_Time    = conv_Reg_GuardTime_to___usec(m_Reg_Sts.DL_ON_Guard_Time.Value);
	m_St.d_UL_ON_Guard_Time    = conv_AAU_Reg_GuardTime_to___usec(m_Reg_Sts.UL_ON_Guard_Time.Value);
	m_St.d_DL_to_UL_Guard_Time = conv_Reg_GuardTime_to___usec_AAU_dl_2_UL(m_Reg_Sts.DL_to_UL_Guard_Time.Value);
	m_St.d_UL_to_DL_Guard_Time = conv_Reg_GuardTime_to___usec_AAU_ul_2_DL(m_Reg_Sts.UL_to_DL_Guard_Time.Value);
#else
	m_St.d_DL_ON_Guard_Time    = conv_Reg_GuardTime_to___usec(m_Reg_Sts.DL_ON_Guard_Time.Value);
	m_St.d_UL_ON_Guard_Time    = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_ON_Guard_Time.Value);
	// 2019.04.11
	diff32 = m_Reg_Sts.DL_to_UL_Guard_Time.Value - m_Reg_Sts.UL_ON_Guard_Time.Value;
	m_St.d_DL_to_UL_Guard_Time = conv_Reg_GuardTime_to___usec(diff32);

	// 2019.04.11
	diff32 = m_Reg_Sts.UL_to_DL_Guard_Time.Value - m_Reg_Sts.DL_ON_Guard_Time.Value;
	m_St.d_UL_to_DL_Guard_Time = conv_Reg_GuardTime_to___usec(diff32);
#endif

	for(optic = 0; optic < MAX_Optic; optic++)	
	{
		// 2019.04.18 : 1:1 mapping, no conversion is needed
		m_St.d_Toffset[optic]       = m_Reg_Sts.DLY[optic].t_Offset.uD32;
		m_St.d_TBdelay_DL[optic]    = m_Reg_Sts.DLY[optic].TB_Delay_DL.uD32;
		m_St.d_TBdelay_UL[optic]    = m_Reg_Sts.DLY[optic].TB_Delay_UL.uD32;
		m_St.d_RoundTrip_Dly[optic] = m_Reg_Sts.DLY[optic].Round_Trip_Dly.uD32;		
		
		m_St.T2A_Dly__ns[optic] = m_Reg_Sts.DLY[optic].T2A_Dly.uD32;
		m_St.TA3_Dly__ns[optic] = m_Reg_Sts.DLY[optic].TA3_Dly.uD32;		
	}
	// 2019.05.23
	if(Is_SFP_Inserted(CPRI_2)==0)
	{
		m_St.d_TBdelay_DL[_100M]    = 0xFFFFFFFF;
		m_St.d_TBdelay_UL[_100M]    = 0xFFFFFFFF;
		m_St.d_RoundTrip_Dly[_100M] = 0xFFFFFFFF;
	}
	if(Is_SFP_Inserted(CPRI_3)==0)
	{
		m_St.d_TBdelay_DL[Alpha]    = 0xFFFFFFFF;
		m_St.d_TBdelay_UL[Alpha]    = 0xFFFFFFFF;
		m_St.d_RoundTrip_Dly[Alpha] = 0xFFFFFFFF;
	}	
	
	for(optic = 0; optic < MAX_Optic; optic++)	
	{
		uVal32 = m_St.d_RoundTrip_Dly[optic];
		if( uVal32 != 0xFFFFFFFF)
		{
			// 2020.07.08 : move position
			Update_Stat( &m_Stat.d_RoundTrip_Dly[optic], uVal32 );

			// 2020.07.08 : add statistics		
			Update_Stat( &m_Stat.d_Oneway_Dly[optic], uVal32/2);
		}
	}
	
	//----------------------------------------
	//		SFP Common : CPRI #0 ~ CPRI #3
	//----------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		if(Is_SFP_Inserted(c) == 0) 
		{
			// 2020.02.14 : SFP.not.insert 시, 0 표시. 
			m_St.d_SFP_LD_Pwr_x10[c] = 0; 
			m_St.d_SFP_PD_Pwr_x10[c] = 0; 
		}
		else
		{
			m_St.d_SFP_LD_Pwr_x10[c] = (INT16S)m_Reg_Sts.Common_CPRI[c].R_SFP_LD_Power.sD32;		
			pwr_x10 = (INT16S)m_Reg_Sts.Common_CPRI[c].R_SFP_PD_Power.sD32;
			if(pwr_x10 < -210) pwr_x10 = -400; // 무신호 시 레벨 표시
			
			m_St.d_SFP_PD_Pwr_x10[c] = pwr_x10;
		}
		
		m_St.d_SFP_Vcc_x10[c]    = (INT16S)m_Reg_Sts.Common_CPRI[c].R_SFP_VCC.sD32; 	
		m_St.d_SFP_LD_Bias_x10[c]= (INT16S)m_Reg_Sts.Common_CPRI[c].R_SFP_LD_Bias.sD32;		                                   
		m_St.d_SFP_Temp_x10[c]   = (INT16S)m_Reg_Sts.Common_CPRI[c].R_SFP_Temp.sD32;
		

		////m_St.d_SFP_WaveLen_x10[c]= (INT16U)m_Reg_Sts.Common_CPRI[c].R_SFP_wavelength.uD32;
		//---------------------------------------------
		// 2019.04.25 : WaveLength
		// [(upper 16bit).(lower 8bit)] x 10
		//---------------------------------
		uVal32 = m_Reg_Sts.Common_CPRI[c].R_SFP_wavelength.uD32; 		
		uVal8 = uVal32 & 0xFF; 
		uVal32 >>= 8;
		m_St.d_SFP_WaveLen_nm[c] = ((INT16U)uVal32 * 100 + uVal8)*10;
		
		/////D_Printf(DEBUG_SYSTEM, "[RCU] WaveLen_nm / C%d 0x%X\n", 
		//// c, m_Reg_Sts.Common_CPRI[c].R_SFP_wavelength.uD32);
		
		//---------------------------------------------		
		m_St.d_SFP_BitRate[c]       = (INT8U)m_Reg_Sts.Common_CPRI[c].R_SFP_BitRate.uD32; // rx val 1 is 0.1Gbps
		m_St.d_RW_SFP_TX_DISABLE[c] = m_Reg_Sts.Common_CPRI[c].RW_SFP_TX_DISABLE.EnDis;
		m_St.d_RW_SFP_Reset[c]      = (m_Reg_Sts.Common_CPRI[c].RW_SFP_Reset.Reset^1)&0x01; // 2019.06.21
	}	

	/*
	----------------------------------------------------
	2020.04.17 : SFP 관련 통계데이터 전송 data 순서.
		[cur] SFP 관련 통계데이터 전송 data 순서가, GUI Status Response 순서와 다름.
		[chg] GUI Status Response 순서대로 응답하도록 수정.
	----------------------------------------------------	
	*/
	for(c = 0; c < CPRI_PATH; c++)
	{
		     if(c == CPRI_1) m = Cpri1;
		else if(c == CPRI_2) m = Cpri2;
		else m = c;
		
		Update_Stat( &m_Stat.d_SFP_LD_Pwr_x10[m],  m_St.d_SFP_LD_Pwr_x10[c] );
		Update_Stat( &m_Stat.d_SFP_PD_Pwr_x10[m],  m_St.d_SFP_PD_Pwr_x10[c] );	
		Update_Stat( &m_Stat.d_SFP_Vcc_x10[m]   ,  m_St.d_SFP_Vcc_x10[c] );	
		Update_Stat( &m_Stat.d_SFP_LD_Bias_x10[m], m_St.d_SFP_LD_Bias_x10[c] );	
		Update_Stat( &m_Stat.d_SFP_Temp_x10[m]   , m_St.d_SFP_Temp_x10[c] );
	}


	// 2020.07.01 -----------------------------------------------------	
	if(m_St.d_SFP_PD_Pwr_x10[CPRI_0] >= m_Eep.PD_Pwr_Lmt_x10[_UP_][CPRI_0])
		 m_St.AlmSt.A_3.PD_Pwr_U_0__ = 1;
	else m_St.AlmSt.A_3.PD_Pwr_U_0__ = 0;

	if(m_St.d_SFP_PD_Pwr_x10[CPRI_1] >= m_Eep.PD_Pwr_Lmt_x10[_UP_][CPRI_1])
		 m_St.AlmSt.A_3.PD_Pwr_U_0_a = 1;
	else m_St.AlmSt.A_3.PD_Pwr_U_0_a = 0;

	if(m_St.d_SFP_PD_Pwr_x10[CPRI_2] >= m_Eep.PD_Pwr_Lmt_x10[_UP_][CPRI_2])
		 m_St.AlmSt.A_4.PD_Pwr_U_1__ = 1;
	else m_St.AlmSt.A_4.PD_Pwr_U_1__ = 0;

	if(m_St.d_SFP_PD_Pwr_x10[CPRI_3] >= m_Eep.PD_Pwr_Lmt_x10[_UP_][CPRI_3])
		 m_St.AlmSt.A_4.PD_Pwr_U_1_a = 1;
	else m_St.AlmSt.A_4.PD_Pwr_U_1_a = 0;
	
	// 2020.07.01 -----------------------------------------------------	
	if(m_St.d_SFP_PD_Pwr_x10[CPRI_0] <= m_Eep.PD_Pwr_Lmt_x10[_DN_][CPRI_0])
		 m_St.AlmSt.A_4.PD_Pwr_L_0__ = 1;
	else m_St.AlmSt.A_4.PD_Pwr_L_0__ = 0;

	if(m_St.d_SFP_PD_Pwr_x10[CPRI_1] <= m_Eep.PD_Pwr_Lmt_x10[_DN_][CPRI_1])
		 m_St.AlmSt.A_4.PD_Pwr_L_0_a = 1;
	else m_St.AlmSt.A_4.PD_Pwr_L_0_a = 0;

	if(m_St.d_SFP_PD_Pwr_x10[CPRI_2] <= m_Eep.PD_Pwr_Lmt_x10[_DN_][CPRI_2])
		 m_St.AlmSt.A_4.PD_Pwr_L_1__ = 1;
	else m_St.AlmSt.A_4.PD_Pwr_L_1__ = 0;

	if(m_St.d_SFP_PD_Pwr_x10[CPRI_3] <= m_Eep.PD_Pwr_Lmt_x10[_DN_][CPRI_3])
		 m_St.AlmSt.A_4.PD_Pwr_L_1_a = 1;
	else m_St.AlmSt.A_4.PD_Pwr_L_1_a = 0;	
	// -----------------------------------------------------	
					
					

				

	/*
	---------------------------------------
	2019.12.04 : SFP LOS/LD 알람 보고시 PD/LD 파워 값이 정상인 오류 개선
	---------------------------------------
	*/
	
	// 2019.05.21. Added.	
	//----------------------------------------	
	INT8U alm_los, alm__ld;
	if(m_St.AlmTest == 0)
	{
		#ifdef __TYPE__PRU__
		if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27
		#endif		
		{	
			if(Is_SFP_Inserted(CPRI_0)) 
			{
				alm_los = m_Reg_Sts.Common_CPRI[CPRI_0].R_SFP_LOS_Alarm.alarm; 
				alm__ld = m_Reg_Sts.Common_CPRI[CPRI_0].R_SFP_LD_Alarm.alarm;

				alm_los = Chk__SFP_Los_Ld_Alarm(A_LOS, CPRI_0, alm_los, m_St.AlmSt.A_2.LOS_SFP_0__);
				alm__ld = Chk__SFP_Los_Ld_Alarm(A__LD, CPRI_0, alm__ld, m_St.AlmSt.A_3.LD_SFP_0__);
			}
			else
			{
				alm_los = 0;	
				alm__ld = 0;	
			}
			m_St.AlmSt.A_2.LOS_SFP_0__ = alm_los;
			m_St.AlmSt.A_3.LD_SFP_0__  = alm__ld;
			//---------------------------------------- // 2019.05.21. Added.	
			if(Is_SFP_Inserted(CPRI_1) && (IS_Alpha_Mhz_Existing())) // 2019.07.17
			{
				alm_los = m_Reg_Sts.Common_CPRI[CPRI_1].R_SFP_LOS_Alarm.alarm;
				alm__ld = m_Reg_Sts.Common_CPRI[CPRI_1].R_SFP_LD_Alarm.alarm; 

				alm_los = Chk__SFP_Los_Ld_Alarm(A_LOS, CPRI_1, alm_los, m_St.AlmSt.A_2.LOS_SFP_0_a);
				alm__ld = Chk__SFP_Los_Ld_Alarm(A__LD, CPRI_1, alm__ld, m_St.AlmSt.A_3.LD_SFP_0_a);
			}
			else
			{
				alm_los = 0;	
				alm__ld = 0;	
			}
			m_St.AlmSt.A_2.LOS_SFP_0_a = alm_los;
			m_St.AlmSt.A_3.LD_SFP_0_a  = alm__ld;
			//---------------------------------------- // 2019.05.21. Added.
			if(Is_SFP_Inserted(CPRI_2)) 
			{
				alm_los = m_Reg_Sts.Common_CPRI[CPRI_2].R_SFP_LOS_Alarm.alarm;
				alm__ld = m_Reg_Sts.Common_CPRI[CPRI_2].R_SFP_LD_Alarm.alarm;

				// 2019.06.09. Added.	
				if(m_St.AlmSt.A_2.CPRI_0__) // if CPRI0 Alarm, masking Cascade-SFP-LD -larm
				{
					m_St.AlmSt.A_3.LD_SFP_1__  = 0;
					alm__ld = 0;
				}
				// 2019.12.11. Added.	
				if(m_St.d_RW_SFP_TX_DISABLE[CPRI_2] == ON)
				{
					m_St.AlmSt.A_3.LD_SFP_1__  = 0;
					alm__ld = 0;
				}	

				alm_los = Chk__SFP_Los_Ld_Alarm(A_LOS, CPRI_2, alm_los, m_St.AlmSt.A_2.LOS_SFP_1__);
				alm__ld = Chk__SFP_Los_Ld_Alarm(A__LD, CPRI_2, alm__ld, m_St.AlmSt.A_3.LD_SFP_1__);
			}
			else
			{		
				alm_los = 0;	
				alm__ld = 0;	
			}
			m_St.AlmSt.A_2.LOS_SFP_1__ = alm_los;
			m_St.AlmSt.A_3.LD_SFP_1__  = alm__ld;
			//---------------------------------------- // 2019.05.21. Added.
			if(Is_SFP_Inserted(CPRI_3) && (IS_Alpha_Mhz_Existing())) // 2019.07.17 
			{
				alm_los = m_Reg_Sts.Common_CPRI[CPRI_3].R_SFP_LOS_Alarm.alarm;
				alm__ld = m_Reg_Sts.Common_CPRI[CPRI_3].R_SFP_LD_Alarm.alarm; 

				
				// 2019.06.09. Added.	
				if(m_St.AlmSt.A_2.CPRI_0_a) // if CPRI0 Alarm, masking Cascade-SFP-LD -larm
				{
					m_St.AlmSt.A_3.LD_SFP_1_a  = 0;
					alm__ld = 0;
				}
				// 2019.12.11. Added.	
				if(m_St.d_RW_SFP_TX_DISABLE[CPRI_3] == ON)
				{
					m_St.AlmSt.A_3.LD_SFP_1_a  = 0;
					alm__ld = 0;
				}	

				alm_los = Chk__SFP_Los_Ld_Alarm(A_LOS, CPRI_3, alm_los, m_St.AlmSt.A_2.LOS_SFP_1_a);
				alm__ld = Chk__SFP_Los_Ld_Alarm(A__LD, CPRI_3, alm__ld, m_St.AlmSt.A_3.LD_SFP_1_a);
			}
			else
			{
				alm_los = 0;	
				alm__ld = 0;	
			}
			m_St.AlmSt.A_2.LOS_SFP_1_a = alm_los;
			m_St.AlmSt.A_3.LD_SFP_1_a  = alm__ld;			
			//-------------------------------------------------------------------------------- 
			/*
			---------------------------------------- 
			2019.11.27
			Update CPRI.LOS.SFP / CPRI.LD.SFP Alarm (alarm string)
			---------------------------------------- 
			*/
			if(m_St.AlmSt_prev.A_2.LOS_SFP_0__ != m_St.AlmSt.A_2.LOS_SFP_0__)
			{
				m_St.AlmSt_prev.A_2.LOS_SFP_0__ = m_St.AlmSt.A_2.LOS_SFP_0__;
				Update_Alm_Str_Lmt( 0x53, 0x02, 0, _100M, m_St.AlmSt.A_2.LOS_SFP_0__); // LOS(CPRI)LOS(SFP)
			}
			if(m_St.AlmSt_prev.A_2.LOS_SFP_0_a != m_St.AlmSt.A_2.LOS_SFP_0_a)
			{
				m_St.AlmSt_prev.A_2.LOS_SFP_0_a = m_St.AlmSt.A_2.LOS_SFP_0_a;
				Update_Alm_Str_Lmt( 0x53, 0x02, 0, Alpha, m_St.AlmSt.A_2.LOS_SFP_0_a); // LOS(CPRI)LOS(SFP)
			}
			if(m_St.AlmSt_prev.A_2.LOS_SFP_1__ != m_St.AlmSt.A_2.LOS_SFP_1__)
			{
				m_St.AlmSt_prev.A_2.LOS_SFP_1__ = m_St.AlmSt.A_2.LOS_SFP_1__;
				Update_Alm_Str_Lmt( 0x53, 0x02, 1, _100M, m_St.AlmSt.A_2.LOS_SFP_1__); // LOS(CPRI)LOS(SFP)
			}
			if(m_St.AlmSt_prev.A_2.LOS_SFP_1_a != m_St.AlmSt.A_2.LOS_SFP_1_a)
			{
				m_St.AlmSt_prev.A_2.LOS_SFP_1_a = m_St.AlmSt.A_2.LOS_SFP_1_a;
				Update_Alm_Str_Lmt( 0x53, 0x02, 1, Alpha, m_St.AlmSt.A_2.LOS_SFP_1_a); // LOS(CPRI)LOS(SFP)
			}
			//---------------------------------------- 
			if(m_St.AlmSt_prev.A_3.LD_SFP_0__ != m_St.AlmSt.A_3.LD_SFP_0__)
			{
				m_St.AlmSt_prev.A_3.LD_SFP_0__ = m_St.AlmSt.A_3.LD_SFP_0__;
				Update_Alm_Str_Lmt( 0x53, 0x04, 0, _100M, m_St.AlmSt.A_3.LD_SFP_0__); // LD(CPRI)LD(SFP)				
			}
			if(m_St.AlmSt_prev.A_3.LD_SFP_0_a != m_St.AlmSt.A_3.LD_SFP_0_a)
			{
				m_St.AlmSt_prev.A_3.LD_SFP_0_a = m_St.AlmSt.A_3.LD_SFP_0_a;
				Update_Alm_Str_Lmt( 0x53, 0x04, 0, Alpha, m_St.AlmSt.A_3.LD_SFP_0_a); // LD(CPRI)LD(SFP)
			}
			if(m_St.AlmSt_prev.A_3.LD_SFP_1__ != m_St.AlmSt.A_3.LD_SFP_1__)
			{
				m_St.AlmSt_prev.A_3.LD_SFP_1__ = m_St.AlmSt.A_3.LD_SFP_1__;
				Update_Alm_Str_Lmt( 0x53, 0x04, 1, _100M, m_St.AlmSt.A_3.LD_SFP_1__); // LD(CPRI)LD(SFP)
			}
			if(m_St.AlmSt_prev.A_3.LD_SFP_1_a != m_St.AlmSt.A_3.LD_SFP_1_a)
			{
				m_St.AlmSt_prev.A_3.LD_SFP_1_a = m_St.AlmSt.A_3.LD_SFP_1_a;
				Update_Alm_Str_Lmt( 0x53, 0x04, 1, Alpha, m_St.AlmSt.A_3.LD_SFP_1_a); // LD(CPRI)LD(SFP)
			}			
		}
		//---------------------------------------- 
			
		//----------------------------------------

		// 2019.06.03			
		m_St.LOS_SFP_St[CPRI_3] = m_St.AlmSt.A_2.LOS_SFP_1_a;
		m_St.LOS_SFP_St[CPRI_2] = m_St.AlmSt.A_2.LOS_SFP_1__;
		m_St.LOS_SFP_St[CPRI_1] = m_St.AlmSt.A_2.LOS_SFP_0_a;
		m_St.LOS_SFP_St[CPRI_0] = m_St.AlmSt.A_2.LOS_SFP_0__;
		
		m_St.LD_SFP_St[CPRI_3] = m_St.AlmSt.A_3.LD_SFP_1_a;
		m_St.LD_SFP_St[CPRI_2] = m_St.AlmSt.A_3.LD_SFP_1__;
		m_St.LD_SFP_St[CPRI_1] = m_St.AlmSt.A_3.LD_SFP_0_a;
		m_St.LD_SFP_St[CPRI_0] = m_St.AlmSt.A_3.LD_SFP_0__;		
	}
	//---------------------------------------------------------------------
	// DSP-Delay
	//---------------------------------------------------------------------
	for(fa = 0; fa < MAX_Optic; fa++) // fa #0/#1
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			uVal32 = conv_Reg_delay_us_to___nsec(m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_DELAY.uD32);			
		#ifdef __USE_ProcessDly_plus_Offset__
			sVal32 = uVal32 - m_Eep.d_Offset_DTU_Delay_DL[fa][m] - m_Env.Delay_Adjust_ns - m_Env.Delay_DL_Dpd_ns[m];
			if(sVal32 < 0) sVal32 = 0; // 2019.08.30
			m_St.d_FPGA_Delay_ns_DL[fa][m] = sVal32;
		#else
			m_St.d_FPGA_Delay_ns_DL[fa][m] = uVal32;
		#endif

			uVal32 = conv_Reg_delay_us_to___nsec(m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_DELAY.uD32); 
		#ifdef __USE_ProcessDly_plus_Offset__
			sVal32 = uVal32 - m_Eep.d_Offset_DTU_Delay_UL[fa][m] - m_Env.Delay_Adjust_ns;
			if(sVal32 < 0) sVal32 = 0; // 2019.08.30
			m_St.d_FPGA_Delay_ns_UL[fa][m] = sVal32;
		#else
			m_St.d_FPGA_Delay_ns_UL[fa][m] = uVal32;
		#endif
			// 2019.05.06
			m_St.d_Test_Pattern_OnOff[fa][m] = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_TEST_Pattern.OnOff ^ 1;
		}
	}
	
	m_St.d_RW_4T2R_Mode = m_Reg_Sts.RW_4T2R_Mode.Mode;
	
	#ifdef __TYPE__PRU__ // PRU Only, 2019.06.01
		m_St.d_Capture_DL_Time = conv_Reg_Capture_to___usec(m_Reg_Sts.RW_Capture_DL_Time.uD32);
		m_St.d_Capture_UL_Time = conv_Reg_Capture_to___usec(m_Reg_Sts.RW_Capture_UL_Time.uD32);			
		m_St.d_Capture_ULNoise_Time = conv_Reg_Capture_to___usec(m_Reg_Sts.RW_Capture_ULNoise_Time.uD32);
		
		m_St.d_UL_Noise_RSSI_StartTime = conv_Reg_RSSI_Time_to___usec(m_Reg_Sts.RW_UL_Noise_RSSI_StartTime.uD32);
		m_St.d_UL_Noise_RSSI_StopTime = conv_Reg_RSSI_Time_to___usec(m_Reg_Sts.RW_UL_Noise_RSSI_StopTime.uD32);
		
		m_St.d_UL_RSSI_StartTime = conv_Reg_RSSI_Time_to___usec(m_Reg_Sts.RW_UL_RSSI_StartTime.uD32);
		m_St.d_UL_RSSI_StopTime  = conv_Reg_RSSI_Time_to___usec(m_Reg_Sts.RW_UL_RSSI_StopTime.uD32);
	#endif

    // Hard Clipping Count
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		// PRU && AAU		
	#ifdef __TYPE__PRU__
		m_St.d_Hard_Clip_Lvl_x10[m] = m_Reg_Sts.DSP_Common_MIMO[m].HC_Threshold.value;
	#else
		uVal16 = m_Reg_Sts.DSP_Common_MIMO[m].HC_Threshold.value;
		if(uVal16 == 0x7FFF) 
			m_St.d_Hard_Clip_Lvl_x10[m] = m_Eep.d_Hard_Clip_Lvl_x10[m];
		else 
			m_St.d_Hard_Clip_Lvl_x10[m] = 10*(20.0*log10((double)uVal16)-70.0); 
			// 2019.09.20
			// level_x10 = 10*(20*log10(reg) - 70)
	#endif		
		
		m_St.d_Hard_Clip_Clear[m]   = m_Reg_Sts.DSP_Common_MIMO[m].HC_Clr.clear;
		m_St.d_Hard_Clip_Cnt_DL[m]  = m_Reg_Sts.DSP_Common_MIMO[m].HC_Count.value;
		
		m_St.d_CFR_Out_Gain[m] = conv_Reg_CfrGain_to___Gain(m_Reg_Sts.DSP_Common_MIMO[m].CFR_Out_Gain.value);
	}
               
	//------------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		m_St.d_Cascade_ID[c] = (INT16U)m_Reg_Sts.DL_CPRI[c].RE_Cascade_ID.uD32;

		m_St.d_DUH_ID[c] = (INT8U)m_Reg_Sts.DL_CPRI[c].DUH_ID.uD8[0];
		
		// PCI
		uVal16 = (INT8U)m_Reg_Sts.DL_CPRI[c].PCI_1st_Byte.uD32;
		uVal16 <<= 8;
		uVal16 |= (INT8U)m_Reg_Sts.DL_CPRI[c].PCI_2nd_Byte.uD32;		
		m_St.d_PCI[c] = uVal16;
		
		
		// cell ID
		m_St.d_Cell_ID[c] = (INT8U)m_Reg_Sts.DL_CPRI[c].Cell_ID.uD32;		

		// gNB-ID
		m_St.d_gNB_ID[c][0] = (INT8U)m_Reg_Sts.DL_CPRI[c].gNB_ID_1st.uD32;
		m_St.d_gNB_ID[c][1] = (INT8U)m_Reg_Sts.DL_CPRI[c].gNB_ID_2nd.uD32;
		m_St.d_gNB_ID[c][2] = (INT8U)m_Reg_Sts.DL_CPRI[c].gNB_ID_3rd.uD32;
		m_St.d_gNB_ID[c][3] = (INT8U)m_Reg_Sts.DL_CPRI[c].gNB_ID_4th.uD32;

		// Port_No : (Layer Splitter Port.No)	
		m_St.g_LS_Port_No[c] = (INT8U)m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD32;
				
		// BIP
		m_St.d_BIP_Cnt128[DL][c][0] = m_Reg_Sts.DL_CPRI[c].BIP_Count.uD32;
		m_St.d_BIP_Cnt128[DL][c][1] = m_Reg_Sts.DL_CPRI[c].BIP_Count_1.uD32;
		m_St.d_BIP_Cnt128[DL][c][2] = m_Reg_Sts.DL_CPRI[c].BIP_Count_2.uD32;
		m_St.d_BIP_Cnt128[DL][c][3] = m_Reg_Sts.DL_CPRI[c].BIP_Count_3.uD32;

		m_St.d_BIP_Cnt128[UL][c][0] = m_Reg_Sts.UL_CPRI[c].BIP_Count.uD32;
		m_St.d_BIP_Cnt128[UL][c][1] = m_Reg_Sts.UL_CPRI[c].BIP_Count_1.uD32;
		m_St.d_BIP_Cnt128[UL][c][2] = m_Reg_Sts.UL_CPRI[c].BIP_Count_2.uD32;
		m_St.d_BIP_Cnt128[UL][c][3] = m_Reg_Sts.UL_CPRI[c].BIP_Count_3.uD32;

		// 2019.04.23 : Instant Err Count
		if(Is_SFP_Inserted(c))
		{
			m_St.d_BIP_Count[DL][c] = m_Reg_Sts.DL_CPRI[c].BIP_Err_Cnt.uD32;
			m_St.d_BIP_Count[UL][c] = m_Reg_Sts.UL_CPRI[c].BIP_Err_Cnt.uD32;
		}
		else
		{
			m_St.d_BIP_Count[DL][c] = 0;
			m_St.d_BIP_Count[UL][c] = 0;
		}	

		m_St.d_VSS_Z__16__Rec___ReSerDesLOS[c][DL] = m_Reg_Sts.DL_CPRI[c].RECSERDESLOS.uD32;
		m_St.d_VSS_Z__16__Rec___ReSerDesLOS[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_SERDESLOS.uD32;
		
		m_St.d_VSS_Z__80__Rec___ReLOFAlarm[c][DL] = m_Reg_Sts.DL_CPRI[c].RECLOFALARM.uD32;
		m_St.d_VSS_Z__80__Rec___ReLOFAlarm[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_LOFALARM.uD32;

		m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[c][DL] = m_Reg_Sts.DL_CPRI[c].RECTypeID.uD32;
		m_St.d_VSS_Z_144__RecTypeID___ReFuncFail[c][UL] = m_Reg_Sts.UL_CPRI[c].REFunctionFail.uD32;

		m_St.d_VSS_Z_208__CnM_Path_Select[c][DL] = m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32; 
		m_St.d_VSS_Z_208__CnM_Path_Select[c][UL] = 0; // reserved

		m_St.d_VSS_Z__17__ReReset___ReResetAck[c][DL] = m_Reg_Sts.DL_CPRI[c].RE_Reset.uD32;
		m_St.d_VSS_Z__17__ReReset___ReResetAck[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Reset_Ack.uD32;

		m_St.d_VSS_Z__81__ReChannelBW___AliveAck[c][DL] = m_Reg_Sts.DL_CPRI[c].RE_Channel_BW.uD32;
		m_St.d_VSS_Z__81__ReChannelBW___AliveAck[c][UL] = m_Reg_Sts.UL_CPRI[c].ALIVE_ACK_00.uD32;

		m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[c][DL] = m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD32;
		m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[c][UL] = m_Reg_Sts.UL_CPRI[c].Mismatch_Noti.uD32;

		m_St.d_VSS_Z_149__CpriCompressOnOff[c][DL] = m_Reg_Sts.DL_CPRI[c].CPRICompressionOnOff.uD32;
		m_St.d_VSS_Z_149__CpriCompressOnOff[c][UL] = 0; // reserved

		m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[c][DL] = m_Reg_Sts.DL_CPRI[c].RE_Cascade_ID.uD32;
		m_St.d_VSS_Z_213__ReCascID___ReCascIdAck[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Cascade_ID_ACK.uD32;

		m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[c][DL] = m_Reg_Sts.DL_CPRI[c].Loopbackmode.uD32;
		m_St.d_VSS_Z__86__LoopbackModeConfig___Ack[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Loopbackmode_ACK.uD32;

		m_St.d_VSS_Z__23__Re1Reset___ACK[c][DL] = m_Reg_Sts.DL_CPRI[c].REx_Reset[0].uD32;
		m_St.d_VSS_Z__23__Re1Reset___ACK[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[0].uD32;
		
		m_St.d_VSS_Z__87__Re2Reset___ACK[c][DL] = m_Reg_Sts.DL_CPRI[c].REx_Reset[1].uD32;
		m_St.d_VSS_Z__87__Re2Reset___ACK[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[1].uD32;

		m_St.d_VSS_Z_151__Re3Reset___ACK[c][DL] = m_Reg_Sts.DL_CPRI[c].REx_Reset[2].uD32;
		m_St.d_VSS_Z_151__Re3Reset___ACK[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[2].uD32;
		
		m_St.d_VSS_Z_215__Re4Reset___ACK[c][DL] = m_Reg_Sts.DL_CPRI[c].REx_Reset[3].uD32;
		m_St.d_VSS_Z_215__Re4Reset___ACK[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[3].uD32;

		m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[c][DL] = 0; // reserved
		m_St.d_VSS_Z__24__Reserved___Re1FuncFail_Casc[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[0].uD32;
		
		m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[c][DL] = 0; // reserved
		m_St.d_VSS_Z__88__Reserved___Re2FuncFail_Casc[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[1].uD32;

		m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[c][DL] = 0; // reserved
		m_St.d_VSS_Z_152__Reserved___Re3FuncFail_Casc[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[2].uD32;
		
		m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[c][DL] = 0; // reserved
		m_St.d_VSS_Z_216__Reserved___Re4FuncFail_Casc[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[3].uD32;

		m_St.d_VSS_Z__30__Reserved___ReVendorID[c][DL] = m_Reg_Sts.DL_CPRI[c].UL_GainBackOff_Onoff.uD8[0]; // 2019.05.13		
		m_St.d_VSS_Z__30__Reserved___ReVendorID[c][UL] = m_Reg_Sts.UL_CPRI[c].RE_Vendor_ID.uD32;

		m_St.d_VSS_Z__60__Rec_PortNum[c][DL] = m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD32; // 2019.04.12 correct
		m_St.d_VSS_Z__60__Rec_PortNum[c][UL] = 0; // reserved

		for(k = 0; k < 20; k++)
			m_St.d_VSS_TDD_Slot_Num[c][k] = (INT8U)m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[k].uD32; // 2019.04.19

		#ifdef _DO_NOT_FOLLOW_VSS_Table_When_TDD_Slot_		
		for(k = 0; k < 20; k++)
			m_St.d_VSS_TDD_Slot_Num[c][k] = default_tdd_slot_val[k]; // 2019.06.14
		#else		
		for(k = 0; k < 20; k++)
			m_St.d_VSS_TDD_Slot_Num[c][k] = (INT8U)m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[k].uD32; // 2019.04.19
		#endif

			

		for(k = 0; k < 4; k++)
			m_St.d_VSS_Noti_IP_1[c][k] = (INT8U)m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[k].uD32; // main/alpha cpri DL

		for(k = 0; k < 4; k++)
			m_St.d_VSS_Noti_IP_2[c][k] = (INT8U)m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[k].uD32; // main/alpha cpri DL		
				

		m_St.d_VSS_Z__2__CPRI_Version[c][DL] = m_Reg_Sts.DL_CPRI[c].CPRI_Version.uD32; // 0xB441
		m_St.d_VSS_Z__2__CPRI_Version[c][UL] = m_Reg_Sts.UL_CPRI[c].CPRI_Version.uD32; // 0xB441
		
		m_St.d_VSS_Z_66__CPRI_Startup[c][DL] = m_Reg_Sts.DL_CPRI[c].CPRI_Startup.uD32; // 0xB442
		m_St.d_VSS_Z_66__CPRI_Startup[c][UL] = m_Reg_Sts.UL_CPRI[c].CPRI_Startup.uD32; // 0xB442
		
		m_St.d_VSS_Z_130_CPRI_L1_RST_LOS[c][DL] = m_Reg_Sts.DL_CPRI[c].CPRI_L1_Reset_LOS.uD32; // 0xB443
		m_St.d_VSS_Z_130_CPRI_L1_RST_LOS[c][UL] = m_Reg_Sts.DL_CPRI[c].CPRI_L1_Reset_LOS.uD32; // 0xB443
		
		m_St.d_VSS_Z_194_Pointer_P[c][DL] = m_Reg_Sts.DL_CPRI[c].Pointer_P.uD32;    // 0xB4x44  
		m_St.d_VSS_Z_194_Pointer_P[c][UL] = m_Reg_Sts.DL_CPRI[c].Pointer_P.uD32;    // 0xB4x44
	}
	//------------------------------------------	


	//------------------------------------------------------------
	// General-Dsp-FA/Port- MIMO #0 ~ #3
	//------------------------------------------------------------
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{	
	#ifdef __TYPE__PRU__
		m_St.d_NCO_PRU[fa][DL] = m_Reg_Sts.General_FA[fa].RW_PRU_DL_NCO.uD32;
		m_St.d_NCO_PRU[fa][UL] = m_Reg_Sts.General_FA[fa].RW_PRU_UL_NCO.uD32;		
	#else // ANTS-AAU
		m_St.d_NCO[fa] = m_Reg_Sts.General_FA[fa].RW_NCO.uD32;
	#endif
		m_St.d_NCO_VALID[fa] = m_Reg_Sts.General_FA[fa].R_NCO_VALID.Value;		
		

		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			// 2019.04.13.
			m_St.d_Payload_OnOff[fa][m] = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_OnOff.OnOff ^ 1; // ON is 1 @ FPGA
			m_St.d_Path_OnOff_DL[fa][m]	= m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_OnOff.OnOff ^ 1; // ON is 1 @ FPGA
			m_St.d_Path_OnOff_UL[fa][m]	= m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_OnOff.OnOff ^ 1; // ON is 1 @ FPGA
			m_St.d_Rx_Casc_Add_OnOff[fa][m]	= m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_OnOff.Cascade_OnOff ^ 1; // ON is 1 @ FPGA

			m_St.d_DL_CW_LEVEL[fa][m]	= m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_CW_Level.uD32;
			m_St.d_DL_CW_ON[fa][m]		= m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_CW_On.OnOff;
			
								
			m_St.d_RW_DL_BW_Sel[fa][m] = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_BW_Sel.BW;
			m_St.d_RW_UL_BW_Sel[fa][m] = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_BW_Sel.BW;			
			
			m_St.d_EQ_Coeff_DL[fa][m]  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_EQ_Coeff.uD16[0]*m_St.EQ_Toggle[fa][DL][0]; // 2019.08.01
			m_St.d_EQ_Coeff_UL[fa][m]  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_EQ_Coeff.uD16[0]*m_St.EQ_Toggle[fa][UL][0]; // 2019.08.01


			#ifdef __TYPE__PRU__			
			m_St.d_UL_Att_x10[fa][m]   = (INT16U)m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_OUTPUT_ATT.uD32;
			#else // AAU
			m_St.d_EQ_Coeff_1_DL[fa][m]  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_EQ_1_Coeff.uD16[0]*m_St.EQ_Toggle[fa][DL][1]; // 2019.08.01
			m_St.d_EQ_Coeff_1_UL[fa][m]  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_EQ_1_Coeff.uD16[0]*m_St.EQ_Toggle[fa][UL][1]; // 2019.08.01
			
			// AAU = Quick_ATT - Gain_Compensation + User_Att			
			sVal16 = m_St.RX_Qck_Att_x2[m]*5 - Get_ALC_Gain_Comp_x10(fa, m) + m_Eep.d_Att_Dig_UL_User_x10[fa][m];
			if(sVal16 < 0) sVal16 = 0;
			m_St.d_UL_Att_x10[fa][m] = sVal16;
			#endif
					
			/*
				//------------------------------------
				m_St.RW_DL_Inphase_Cont[fa][m] = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_Inphase_Cont.uD32
				m_St.RW_DL_Quad_Cont[fa][m]    = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_Quad_Cont.uD32									
				//------------------------------------				
				m_St.RW_UL_Inphase_Cont[fa][m]	  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_Inphase_Cont.uD32   
				m_St.RW_UL_Quad_Cont[fa][m] 	  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_Quad_Cont.uD32	  
				m_St.R_UL_Inphase_Det[fa][m]	  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_Inphase_Det.uD32	  
				m_St.R_UL_Quad_Det[fa][m]		  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_UL_Quad_Det.uD32						
				m_St.R_FB_RSSI_Peak[fa][m]		  = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].R_FB_RSSI_Peak.uD32 						
				//------------------------------------				
			*/
		}
		m_St.d_Channel_BW[fa] = m_St.d_RW_DL_BW_Sel[fa][0]; // DL & 1st is representitive	
	}
	m_St.d_Rx_Casc_Add_OnOff_UL = m_St.d_Rx_Casc_Add_OnOff[0][0]; // [0][0] is representitive	
	
	//---------------------------------------------------- 
	// CFR : PRU Only : 2019.04.15 : PAU, AAU Common
	//----------------------------------------------------
	m_St.d_SW_VER_CFR[0] = m_Reg_Sts.CFR[0].Version.uD8[1]; // littel & big
	m_St.d_SW_VER_CFR[1] = m_Reg_Sts.CFR[0].Version.uD8[0]; // littel & big
		
	for(path = 0; path < MAX_DL; path++)
	{
		m_St.d_CFR_OnOff[path]     = m_Reg_Sts.CFR[path].OnOff.Val; // AID ON = '0' = register CFR ON
		#ifdef __TYPE__PRU__
		m_St.d_CFR_Level_x10[path] = m_Reg_Sts.CFR[path].Level.uD32;
		#else // AAU
		m_St.d_CFR_Level_x10[path] = 10*m_Reg_Sts.CFR[path].Level.uD32;
		#endif
	}

//------------------------------------------------
#ifdef __TYPE__AAU__
//------------------------------------------------
	INT16S sTemp_x4;
	INT16S sTemp_x2;
	INT16S att___x4;
	INT8U reason_dis;
	INT8U reason__sd;
	INT8U is_sd_chk_enable;
	//INT8U rcu_reason;
	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{	
		// RF		
		//-----------------		
		sTemp_x2 = conv_signed_att_01_to_05(m_St.d_Temp_Att_Dig_DL_x10[COMMON][m]);
		sTemp_x4 = conv_signed_att_05_to_025(sTemp_x2);
		
		att___x4 = m_Reg_Sts.RFU[m].DL_Att.Val_x4 - sTemp_x4 - RF_Att_Accuracy_x4[DL][m];
		if(att___x4 < 0) att___x4 = 0;
		
		// 2019.06.26		
		m_St.DL_Att[m] = att___x4 - m_Eep.DL_Att_Offs[m];
		if(m_St.DL_Att[m] < m_Eep.DL_Att_User[m])
			m_St.DL_Att[m] = m_Eep.DL_Att_User[m];
		
		//-----------------		
		sTemp_x2 = conv_signed_att_01_to_05(m_St.d_Temp_Att_Dig_UL_x10[COMMON][m]);
		sTemp_x4 = conv_signed_att_05_to_025(sTemp_x2);

		att___x4 = m_Reg_Sts.RFU[m].UL_Att.Val_x4 - sTemp_x4 - RF_Att_Accuracy_x4[UL][m];
		if(att___x4 < 0) att___x4 = 0;
		m_St.UL_Att[m] = att___x4;
		// 2019.05.18 : Gain Backoff.
		//-----------------		
		// toggel, ON = Register'1' = GUI '0'
		

		if(Check_PAU_Use(m)) // 2020.02.03
		{	
			m_St.Pwr_Amp_En[m] = m_Reg_Sts.RFU[m].PA_OnOff.On ^ 0x01; 
			m_St.RVS_Amp_Onoff[m] = m_Reg_Sts.RFU[m].PA_OnOff.On ^ 0x01; 		

			/*
			// 2019.09.16 : 
			AAU : overpower shutdow 시, 
			disable alarm 이 아닌, shutdown alarm report 되도록 수정.
			*/
			if(m_St.Pwr_Amp_En[m] == ON)
			{
				if(m_St.AlmSt.Path[m].PAU.Disabled)
						print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "(AMP ON) Path[%d].Disabled = 0", m);

				m_St.AlmSt.Path[m].PAU.Disabled = 0; // When AAU Only
			
				if(Get__Reason_SD_5115_Ori(m) != Rsn_SD_UL_OverIn)
					m_St.AlmSt.Path[m].PAU.Shutdown = 0; // When AAU Only
			}
			else
			{
				reason_dis = Get__Reason_Dis_5117_Ori(m);
				reason__sd = Get__Reason_SD_5115_Ori(m);
						
				switch(reason_dis)
				{
					case Rsn_Dis_User___Ctrl:
						if(m_St.AlmSt.Path[m].PAU.Disabled == 0)
							print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "(D1) Path[%d].Disabled = 1, SD = 0", m);
							
						m_St.AlmSt.Path[m].PAU.Disabled = 1; // When AAU Only
						m_St.AlmSt.Path[m].PAU.Shutdown = 0; // When AAU Only 
						m_St.gREMS.Path[m].PAU.Shutdown = 0; // When AAU Only 
						
						break;
								
					case Rsn_Dis_System_Ctrl:
						if(m_St.AlmSt.Path[m].PAU.Shutdown == 0) // When AAU Only
						{						
							if(m_St.AlmSt.Path[m].PAU.Disabled == 0)
								print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "(D2) Path[%d].Disabled = 1", m);

							m_St.AlmSt.Path[m].PAU.Disabled = 1; // When AAU Only	
						}
						break;			
				}
				//----------------------------------------------
				// check S/D alarm
				is_sd_chk_enable = IS__SD_Check_Enabled(m);

				if(is_sd_chk_enable == 0) 
				{
					/*
					disable alarm 우선 순위에 밀려서, sd reason은 있는데, sd 정상이어서 sd reason이 필요 없는 상태에서,
					이후, sd alarm 이 되어, 원하는 reason 으로 갱신되어야 함.
					(reason overwrite 안되는 조건으로 이때, 원하는 reason 으로 갱신 안됨.)
											
					"SD alarm 정상" && "정상으로 이미 보고가 됨" 이면, Reason을 clear한다.
					*/
					if(reason__sd != 0)
					{
						if(m_St.AlmSt.Path[m].PAU.Shutdown == 0)
						{
							if(m_St.NMS_Alm.Path[m].PAU.Shutdown == 0)
							{
								if(m_St.Old_Alm.Path[m].PAU.Shutdown == 0)
								{
									Clr__Reason_SD_5115(m);
									Clr__Reason_SD_5115_Prev(m);

									print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Clr__Reason_SD_5115(%d)", m);
									print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "Clr__Reason_SD_5115_Prev(%d)", m);
								}
							}
						}					
					}
				}			
				else // is_sd_chk_enable > 0
				{											
					switch(reason__sd)
					{							
						case Rsn_SD_TSSI_Over : // by RCU
						case Rsn_SD_CPRI_Fail :
						case Rsn_SD_CLock_Alm :
						case Rsn_SD____by_UDA :
						case Rsn_SD_UL_OverIn :
						case Rsn_SD_Over__Pwr : // AAU FWD Shutdown
							if(is_sd_chk_enable == 1) // disable is normal
							{
								if(m_St.AlmSt.Path[m].PAU.Shutdown == 0)
									print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "(S1) Path[%d].Shutdown = 1", m);

								m_St.AlmSt.Path[m].PAU.Shutdown = 1;
							}
							else if(is_sd_chk_enable == 2) // disable is alarm but system-control
							{
								if(m_St.AlmSt.Path[m].PAU.Shutdown == 0)							
									print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "(S2) Path[%d].Shutdown = 1, DIS = 0", m);
									
								m_St.AlmSt.Path[m].PAU.Shutdown = 1;

								if(m_St.AlmSt.Path[m].PAU.Disabled)
									print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "(S2) Path[%d].Disabled = 0", m);
									
								m_St.AlmSt.Path[m].PAU.Disabled = 0;
							}							
							break;
									
						#if 0	
						/*
						활성화 하면, 
						30m 전에 disable alarm 이있었다면, 
						disable alarm이 clear되고, sd alarm 이 발생하는데,
						amp disable 상태에서 ul.over.in algorithm 이 동작 할 수 없으므로,
						아래를 활성화 해도 무방할 듯.
						*/ 
						case Rsn_SD_UL_OverIn_Skip:
							m_St.AlmSt.Path[m].PAU.Shutdown = 1;
							m_St.AlmSt.Path[m].PAU.Disabled = 0;
							break;
						#endif	
						
						//default	:
						//	m_St.AlmSt.Path[m].PAU.Shutdown = 0;
						//	break;
							
					}
				}	
			}
		}
		else // 2020.02.03 : 미사용시, alarm clear, Amp OFF 표시를 위해.
		{			
			m_St.Pwr_Amp_En[m] = OFF;
			m_St.RVS_Amp_Onoff[m] = OFF;

			m_St.AlmSt.Path[m].PAU.Disabled = 0;
			m_St.AlmSt.Path[m].PAU.Shutdown = 0;			
		}
		//-------------------------------------------------------------

		if(m_St.AlmTest && m_St.AlmFlag.Path[m].PAU.Disabled) // 2019.05.01
		{
			m_St.AlmSt.Path[m].PAU.Disabled = m_St.AlmCtrl.Path[m].PAU.Disabled;
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "m_St.AlmTest : Path[%d].Disabled = %d", 
				m, m_St.AlmSt.Path[m].PAU.Disabled);
		}
		
		// UL AFE ATT		
		m_St.UL_AFE_ATT_x1[m] = m_Reg_Sts.ALC[m].Def_Att.Val_x1;
	}	
//------------------------------------------------		
#else // PRU only
//------------------------------------------------	

	for(m = 0; m < MAX_MIMO_PATH; m++)
	{		
		m_St.d_Layer_Use[m] = m_Reg_Sts.RFU[m].LayerInfo.Use;
	}

	
	//------------------
	// DPD : PRU Only
	//------------------
	memcpy(m_St.d_SW_VER_DPD, &m_Reg_Sts.DPD[0].Version.uD8[1], 3);
	
	for(path = 0; path < MAX_DL; path++)
	{		
		m_St.d_DPD_OnOff[path]  = m_Reg_Sts.DPD[path].OnOff.Val;
		m_St.d_DPD_St[path]     = (INT16U)m_Reg_Sts.DPD[path].Status.sD32;		
		m_St.d_DPD_Err_St[path] = m_Reg_Sts.DPD[path].Err_St.uD32;
		
		
		// m_St.d_DPD_Reason[MAX_DL];
		m_St.d_DPD_Success_Cnt[path] = m_Reg_Sts.DPD[path].Success_Cnt.uD32; // 2019.05.20		
		m_St.d_DPD_In__Pwr_x10[path] = (INT16S)m_Reg_Sts.DPD[path].Input_Pwr.uD32;
		m_St.d_DPD_Out_Pwr_x10[path] = (INT16S)m_Reg_Sts.DPD[path].Output_Pwr.uD32 + m_Eep.d_Offset_DPD_x10[path]; // 2019.07.27

	#ifdef	__USE_DPD_9v0_Calculation__ // 2019.12.06, Test version
		m_St.d_DPD_Eng_In__Pwr_x10[path] = 100.0*log10(256.0*(double)m_Reg_Sts.DPD[path].Eng_In_Pwr.uD32);
		m_St.d_DPD_FB_Eng_In_Pwr_x10[path] = 100.0*log10(256.0*(double)m_Reg_Sts.DPD[path].FB_Eng_In_Pwr.uD32);	
	#else
		m_St.d_DPD_Eng_In__Pwr_x10[path] = 100.0*log10(256.0*(double)m_Reg_Sts.DPD[path].Eng_In_Pwr.uD32/pow(2.0, 30.0));		
		m_St.d_DPD_FB_Eng_In_Pwr_x10[path] = 100.0*log10(256.0*(double)m_Reg_Sts.DPD[path].FB_Eng_In_Pwr.uD32/pow(2.0, 30.0));	
	#endif

		m_St.d_DPD_Eng_Out_Pwr_x10[path] = (INT16S)m_Reg_Sts.DPD[path].Eng_Out_Pwr.uD32;
		m_St.d_DPD_FB_In__Pwr_x10[path]  = (INT16S)m_Reg_Sts.DPD[path].FB_In_Pwr.uD32;
		

		// 2019.05.20
		m_St.d_DPD_PeakExp_x10[path] = 100.0*log10((double)m_Reg_Sts.DPD[path].PeakExpansion.uD32);
		m_St.d_DPD_____MSE_x10[path] = 100.0*log10((double)m_Reg_Sts.DPD[path].MSE.uD32);
		m_St.d_DPD__RE_MSE_x10[path] = 100.0*log10((double)m_Reg_Sts.DPD[path].PRE_MSE.uD32);
		m_St.d_DPD_LUT_Value[path]   = m_Reg_Sts.DPD[path].LUT_Value.uD32;
		m_St.d_DPD_Counts[path]      = m_Reg_Sts.DPD[path].Counts.uD32;
	}
#endif	

	m_St.d_UL_Gain_Guard_time[0] = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_GAIN_Guard_Time[0].uD32);
	m_St.d_UL_Gain_Guard_time[1] = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_GAIN_Guard_Time[1].uD32);
	m_St.d_UL_Gain_Guard_time[2] = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_GAIN_Guard_Time[2].uD32);
	m_St.d_UL_Gain_Guard_time[3] = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_GAIN_Guard_Time[3].uD32);
	m_St.d_UL_Gain_Guard_time[4] = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_GAIN_Guard_Time[4].uD32);
	
	for(k = 0; k < 20; k++)
		m_St.RW_Slot_Format[k] = m_Reg_Sts.RW_Slot_Format[k].uD32;
	
	m_St.d_UL_Gain_Meas_time = conv_Reg_GuardTime_to___usec(m_Reg_Sts.UL_GAIN_Measure_Time.uD32);
	
	#ifdef __TYPE__PRU__	
	m_St.d_t_Sync_Delay = m_Reg_Sts.TSYNC_Delay.uD32*1000/307; // reg/0.307	 
	#else
	m_St.d_t_Sync_Delay = conv_Reg_TsyncDly_to___usec(m_Reg_Sts.TSYNC_Delay.uD32);
	#endif
	
	m_St.d_t_Sync_Fail = m_Reg_Sts.TSYNC_FAIL.Data;
//================================================================
}






/*
--------------------------------------------------------------------------------------------------
	AAU Only
	Registers
--------------------------------------------------------------------------------------------------
*/

INT8U Is_AAU_Only_Reg_Common(INT8U reg)
{
	switch(reg)
	{
		case Reg_Common_RW_FAN_Control: // AAU Only
		case Reg_Common_RW_Recovery_Clock_Set: // AAU Only
			return 1;

		default:
			return 0;
	}
}
/*
--------------------------------------------------------------------------------------------------
	AAU Only
	Registers
--------------------------------------------------------------------------------------------------
*/

INT8U Is_AAU_Only_Reg_DSP_FA_MIMO(INT8U reg)
{
	switch(reg)
	{
		case Reg_DSP_FA_MIMO_RW_DL_Inpahase_Cont: // AAU Only
		case Reg_DSP_FA_MIMO_RW_DL_Quad_Cont:     // AAU Only
		case Reg_DSP_FA_MIMO_R_DL_RSSI_Peak:      // AAU Only
		case Reg_DSP_FA_MIMO_RW_UL_Inpahase_Cont: // AAU Only
		case Reg_DSP_FA_MIMO_RW_UL_Quad_Cont:     // AAU Only
		case Reg_DSP_FA_MIMO_R_UL_Inpahase_Det:   // AAU Only
		case Reg_DSP_FA_MIMO_R_UL_Quad_Det:       // AAU Only
		case Reg_DSP_FA_MIMO_R_UL_RSSI_Peak:      // AAU Only
		case Reg_DSP_FA_MIMO_R_FB_RSSI_RMS:       // AAU Only
		case Reg_DSP_FA_MIMO_R_FB_RSSI_Peak:      // AAU Only
		case Reg_DSP_FA_MIMO_R_GC_RSSI_RMS:       // AAU Only
			return 1;

		default:
			return 0;
	}
}
/*
--------------------------------------------------------------------------------------------------
	AAU Only
	Registers
--------------------------------------------------------------------------------------------------
*/



/*
--------------------------------------------------------------------------------------------------
	AAU Only
	Registers
--------------------------------------------------------------------------------------------------
*/

INT8U Is_AAU_Only_Reg_DSP_Common(INT8U reg)
{
	switch(reg)
	{
		case Reg_DSP_Common_RW_4T2R_Mode: // AAU Only
			return 1;

		default:
			return 0;
	}
}



/*
--------------------------------------------------------------------------------------------------
	Compare 
	of
	PRU/AAU FPGA Register Stauts and Control Variables
--------------------------------------------------------------------------------------------------
*/
void Proc_Compare_Fpga_Register_VSS(void) // Compare and Ctrl
{
	int c;
	int r;
	int vss_fail[DL_UL];

	if(Get_Instant_CPRI_Lock_St() == 0) // if un-lock
		return;
//------------------------------------------------------------
// Vss-CPRI-DL/UL
//------------------------------------------------------------
	for(c = CPRI_0; c < CPRI_PATH; c++)
	{		
		vss_fail[DL] = 0;
		vss_fail[UL] = 0;

		if(IS__CPRI_Lock(c)==0) continue; // 2019.09.09
				
		// UL ------------------------------------------
		for(r = 0; r < Reg_VSS_UL_MAX; r++)
		{	
			if(pReg_VSS_UL_Addr[c][r][STS] != NULL) 
			{
				if(Reg_VSS_UL_flag[c][r] == 0) continue;
				
				if(*pReg_VSS_UL_Addr[c][r][STS] != *pReg_VSS_UL_Addr[c][r][CTL])
				{
					Write_FPGA_Reg(Reg_VSS_UL_Addr[c][r], *pReg_VSS_UL_Addr[c][r][CTL], "VSS_UL");
					vss_fail[UL]++;

					if(r == Reg_VSS_UL_RE_Reset_Ack)
					{
						if(c == CPRI_0)
						{
							if(Test_timer_En)
							{
								D_Printf( DEBUG_SYSTEM, VT_GREEN "[RCU] Test_timer %d[ms]\n",  OSTimerGet_msec(&Test_timer));
								printf(VT_GREEN "[RCU] Test_timer %d[ms]\n" VT_END_CR,  OSTimerGet_msec(&Test_timer));
								OSTimerReset( &Test_timer );
								Test_timer_En = 0;
							}
						}
					}


					switch(r)
					{
						case Reg_VSS_UL_ALIVE_ACK_00:
							break;
							
						case Reg_VSS_UL_RE_Loopbackmode_ACK:
							// TEST
							//----------------
							/*
							if(Test_timer_Vss_En)
							{
								printf(VT_GREEN "[RCU] LoopBack(cpri%d) : Time End %d[ms], TaskCnt %d\n" VT_END_CR, 
									c, OSTimerGet_msec(&Test_timer_Vss), task_cnt);
								
								OSTimerReset(&Test_timer_Vss);
								Test_timer_Vss_En = 0;
							}
							*/
							//----------------
							break;
							
						case Reg_VSS_UL_Mismatch_Noti:
						/*
							D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_UL_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X.  Reg_VSS_UL_Mismatch_Noti\n", 
							c, r, 
							Reg_VSS_UL_Addr[c][r],
							*pReg_VSS_UL_Addr[c][r][STS],
							*pReg_VSS_UL_Addr[c][r][CTL]);
						*/	
							break;							
							
						default:
						#ifdef _Show_Reg_Cmp_Debug_
						D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_UL_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
							c, r, 
							Reg_VSS_UL_Addr[c][r],
							*pReg_VSS_UL_Addr[c][r][STS],
							*pReg_VSS_UL_Addr[c][r][CTL]);
						#endif	
							break;
					}
				}
				else 
				{
					Reg_VSS_UL_flag[c][r] = 0;
					#if 0
					D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_UL_Addr (0x%02X) 0x%08X, 0x%08X == 0x%08X .\n", 
						c, r, 
						Reg_VSS_UL_Addr[c][r],
						*pReg_VSS_UL_Addr[c][r][STS],
						*pReg_VSS_UL_Addr[c][r][CTL]);
					#endif	
				}
			}
		}	
		// DL ------------------------------------------
		for(r = 0; r < Reg_VSS_DL_MAX; r++)
		{			
			if(pReg_VSS_DL_Addr[c][r][STS] != NULL) 
			{
				if(Reg_VSS_DL_flag[c][r] == 0) continue;
				
				if(*pReg_VSS_DL_Addr[c][r][STS] != *pReg_VSS_DL_Addr[c][r][CTL])
				{
					Write_FPGA_Reg(Reg_VSS_DL_Addr[c][r], *pReg_VSS_DL_Addr[c][r][CTL], "VSS_DL");
					vss_fail[DL]++;

					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_DL_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
						c, r, 
						Reg_VSS_DL_Addr[c][r],
						*pReg_VSS_DL_Addr[c][r][STS],
						*pReg_VSS_DL_Addr[c][r][CTL]);
					#endif	
				}
				else 
				{
					Reg_VSS_DL_flag[c][r] = 0;
					#if 0
					D_Printf( DEBUG_SYSTEM, "[RCU] c %d, Reg_VSS_DL_Addr (0x%02X) 0x%08X, 0x%08X == 0x%08X .\n", 
						c, r, 
						Reg_VSS_DL_Addr[c][r],
						*pReg_VSS_DL_Addr[c][r][STS],
						*pReg_VSS_DL_Addr[c][r][CTL]);
					#endif
				}
			}
		}
		Reg_VSS_DL__Cnt[c] = vss_fail[DL];		
		Reg_VSS_UL__Cnt[c] = vss_fail[UL];
	}	
}



/*
---------------------------------------------------------------------------
Initialize 
	"m_Reg_Ctl" 
	with "m_Reg_Sts"
---------------------------------------------------------------------------
*/
void Copy__RegSts__to__RegCtl(void)
{
	int c;
	int fa;
	int m, r;	

//----------------------------------------
//	Common
//----------------------------------------	
	for(r = 0; r < Reg_Common_MAX; r++)
	{	
		if(pReg_Common_Addr[r][CTL] == NULL) continue;
		*pReg_Common_Addr[r][CTL] = *pReg_Common_Addr[r][STS];
	}
//----------------------------------------
//	SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		for(r = 0; r < Reg_Common_MIMO_MAX; r++)
		{			
			if(pReg_Common_MIMO_Addr[c][r][CTL] == NULL) continue;
			*pReg_Common_MIMO_Addr[c][r][CTL] = *pReg_Common_MIMO_Addr[c][r][STS];
		}
	}
//---------------------------------------------------------------------
// DSP-Delay
//---------------------------------------------------------------------
	for(r = 0; r < Reg_DSP_Common_Max; r++)
	{		
		if(pReg_DSP_Common_Addr[r][CTL] == NULL) continue;
		*pReg_DSP_Common_Addr[r][CTL] = *pReg_DSP_Common_Addr[r][STS];
	}
//---------------------------------------------------------------------
// DSP-Common-MIMO
//---------------------------------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_DSP_Common_MIMO_MAX; r++)
		{			
			if(pReg_DSP_Common_MIMO_Addr[m][r][CTL] == NULL) continue;				
			*pReg_DSP_Common_MIMO_Addr[m][r][CTL] = *pReg_DSP_Common_MIMO_Addr[m][r][STS];
		}
	}
//------------------------------------------------------------
// General-Dsp-FA/Port- MIMO #0 ~ #3
//------------------------------------------------------------
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(r = 0; r < Reg_DSP_FA_Common_MAX; r++)
		{			
			if(pReg_DSP_FA_Common_Addr[fa][r][CTL] == NULL) continue;
			*pReg_DSP_FA_Common_Addr[fa][r][CTL] = *pReg_DSP_FA_Common_Addr[fa][r][STS];
		}
	}
	
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			for(r = 0; r < Reg_DSP_FA_MIMO_MAX; r++)
			{				
				if(pReg_DSP_FA_MIMO_Addr[fa][m][r][CTL] == NULL) continue;
				*pReg_DSP_FA_MIMO_Addr[fa][m][r][CTL] = *pReg_DSP_FA_MIMO_Addr[fa][m][r][STS];
			}
		}
	}

//------------------------------------------------------------
// Vss-CPRI-DL/UL
//------------------------------------------------------------	
	if(Get_Instant_CPRI_Lock_St()) // if lock
	{
		for(c = CPRI_0; c < CPRI_PATH; c++)
		{
			if(IS__CPRI_Lock(c)==0) continue; // 2019.09.09
			
			for(r = 0; r < Reg_VSS_UL_MAX; r++)
			{	
				if(pReg_VSS_UL_Addr[c][r][CTL] == NULL) continue;
				*pReg_VSS_UL_Addr[c][r][CTL] = *pReg_VSS_UL_Addr[c][r][STS];
			}
			for(r = 0; r < Reg_VSS_DL_MAX; r++)
			{			
				if(pReg_VSS_DL_Addr[c][r][CTL] == NULL) continue;
				*pReg_VSS_DL_Addr[c][r][CTL] = *pReg_VSS_DL_Addr[c][r][STS];
			}
		}
	}
//------------------------------------------------------------
// 'Mode General-TDD'
//------------------------------------------------------------	
	for(r = 0; r < Reg_TDD_MAX; r++)
	{
		if(pReg_TDD_Addr[r][CTL] == NULL) continue;
		*pReg_TDD_Addr[r][CTL] = *pReg_TDD_Addr[r][STS];
	}
	//----------------------------------------
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(r = 0; r < Reg_TDD2_MAX; r++)
		{
			if(pReg_TDD2_Addr[fa][r][CTL] == NULL) continue;
			*pReg_TDD2_Addr[fa][r][CTL] = *pReg_TDD2_Addr[fa][r][STS];
		}
	}
	//------------------------------------------------------------
	// Comp OnOff
	//------------------------------------------------------------
	for(m = 0; m < CPRI_PATH; m++)
	{
		if(pReg_COMP_ONOFF_Addr[m][CTL] == NULL) continue;
		*pReg_COMP_ONOFF_Addr[m][CTL] = *pReg_COMP_ONOFF_Addr[m][STS];
	}
	

	//---------------------------------------------------------------------
	// CFR
	//---------------------------------------------------------------------
	for(m = 0; m < MAX_DL; m++)
	{
		for(r = 0; r < Reg_CFR_MAX; r++)
		{			
			if(pReg_CFR_Addr[m][r][CTL] == NULL) continue;
			*pReg_CFR_Addr[m][r][CTL] = *pReg_CFR_Addr[m][r][STS];
		}
	}
#ifdef __TYPE__PRU__	
	//---------------------------------------------------------------------
	// PRU 
	//---------------------------------------------------------------------
	for(m = 0; m < MAX_DL; m++)
	{
		for(r = 0; r < Reg_DPD_MAX; r++)
		{			
			if(pReg_DPD_Addr[m][r][CTL] == NULL) continue;
			*pReg_DPD_Addr[m][r][CTL] = *pReg_DPD_Addr[m][r][STS];
		}		
	}
	
	for(r = 0; r < Reg_DPD2_MAX; r++)
	{			
		if(pReg_DPD2_Addr[r][CTL] == NULL) continue;
		*pReg_DPD2_Addr[r][CTL] = *pReg_DPD2_Addr[r][STS];
	}
#else // __AAU__
//------------------------------------------------------------
// RF : AAU Only
//------------------------------------------------------------	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_RFU_CTRL_MAX; r++) // RF
		{		
			if(pReg_RFU_CTRL_Addr[m][r][CTL] == NULL) continue;
			*pReg_RFU_CTRL_Addr[m][r][CTL] = *pReg_RFU_CTRL_Addr[m][r][STS];
		}
	}
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(r = 0; r < Reg_ALC_MAX; r++) // ALC
		{
			if(pReg_ALC_Addr[m][r][CTL] == NULL) continue;
			*pReg_ALC_Addr[m][r][CTL] = *pReg_ALC_Addr[m][r][STS];
		}
	}
#endif
}






void Proc_Compare_Fpga_Register(void) // Compare and Ctrl
{
	int c;
	int fa;
	int m, r;
	int fail;
	int tot_fail = 0;

	Chk_RegCtrlFlag_Status();


//----------------------------------------
//	Common
//----------------------------------------	
	for(fail = 0, r = 0; r < Reg_Common_MAX; r++)
	{	
		#ifdef __TYPE__PRU__
		if(Is_AAU_Only_Reg_Common(r)) continue;
		#endif		
		if(r == Reg_Common_RW_Interrupt_status0) continue;
		if(r == Reg_Common_RW_Interrupt_status1) continue;
		
		if(pReg_Common_Addr[r][STS] != NULL) 
		{			
			if(Reg_Common_flag[r] == 0) continue;
			
			if(*pReg_Common_Addr[r][STS] != *pReg_Common_Addr[r][CTL])
			{
				Write_FPGA_Reg(Reg_Common_Addr[r], *pReg_Common_Addr[r][CTL], "Common");	
				fail++;			

				switch(r)
				{
					case Reg_Common_RW_DTU_LED:
						break;
						
					default:								
						#ifdef _Show_Reg_Cmp_Debug_
						D_Printf( DEBUG_SYSTEM, "[RCU] Reg_Common_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
							r, Reg_Common_Addr[r], *pReg_Common_Addr[r][STS], *pReg_Common_Addr[r][CTL]);
						#endif	
						break;
				}				
			}			
			else
			{
				Reg_Common_flag[r] = 0;
				#if 0
				D_Printf( BIT(1), "[RCU] Reg (0x%02X) 0x%08X, 0x%08X == 0x%08X .\n", 
					r, Reg_Common_Addr[r],
					*pReg_Common_Addr[r][STS],
					*pReg_Common_Addr[r][CTL]);
				#endif
			}			
		}
	}
	Reg_Common__Cnt = fail;
	tot_fail += fail;
//----------------------------------------
//	SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	for(c = 0; c < CPRI_PATH; c++)
	{
		for(fail = 0, r = 0; r < Reg_Common_MIMO_MAX; r++)
		{
			// if 'r' is below, skip.
			//if(r == Reg_Common_MIMO_RW_SFP_Reset) continue; // 2019.06.21
			
			if(pReg_Common_MIMO_Addr[c][r][STS] != NULL) 
			{
				if(Reg_Common_MIMO_flag[c][r] == 0) continue;
				
				if(*pReg_Common_MIMO_Addr[c][r][STS] != *pReg_Common_MIMO_Addr[c][r][CTL])
				{
					Write_FPGA_Reg(Reg_Common_MIMO_Addr[c][r], *pReg_Common_MIMO_Addr[c][r][CTL], "Common_MIMO");
					fail++;

					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf( DEBUG_SYSTEM, "[RCU] %d Reg_Common_MIMO_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
						c, r, 
						Reg_Common_MIMO_Addr[c][r],
						*pReg_Common_MIMO_Addr[c][r][STS],
						*pReg_Common_MIMO_Addr[c][r][CTL]);
					#endif	
				}
				else Reg_Common_MIMO_flag[c][r] = 0;
			}
		}
		Reg_Common_MIMO__Cnt[c] = fail;
		tot_fail += fail;
	}
//---------------------------------------------------------------------
// DSP-Delay
//---------------------------------------------------------------------
	for(fail = 0, r = 0; r < Reg_DSP_Common_Max; r++)
	{
		#ifdef __TYPE__PRU__
		if(Is_AAU_Only_Reg_DSP_Common(r)) continue;
		#endif
		
		if(pReg_DSP_Common_Addr[r][STS] != NULL)
	 	{		  
	 		if(Reg_DSP_Common_flag[r] == 0) continue;
	 		
			if(*pReg_DSP_Common_Addr[r][STS] != *pReg_DSP_Common_Addr[r][CTL])
			{
				Write_FPGA_Reg(Reg_DSP_Common_Addr[r], *pReg_DSP_Common_Addr[r][CTL], "DSP_Common");
		  	    fail++;
				#ifdef _Show_Reg_Cmp_Debug_
		  	    D_Printf( DEBUG_SYSTEM, "[RCU] Reg_DSP_Common_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
					r, 
					Reg_DSP_Common_Addr[r],
					*pReg_DSP_Common_Addr[r][STS],
					*pReg_DSP_Common_Addr[r][CTL]);
				#endif	
		  	}
		  	else Reg_DSP_Common_flag[r] = 0;
		}
	}
	Reg_DSP_Common__Cnt = fail;
	tot_fail += fail;
	

//---------------------------------------------------------------------
// DSP-Common-MIMO
//---------------------------------------------------------------------
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(fail = 0, r = 0; r < Reg_DSP_Common_MIMO_MAX; r++)
		{			
			if(pReg_DSP_Common_MIMO_Addr[m][r][STS] != NULL)
			{		  
				if(Reg_DSP_Common_MIMO_flag[m][r] == 0) continue;
				
				if(*pReg_DSP_Common_MIMO_Addr[m][r][STS] != *pReg_DSP_Common_MIMO_Addr[m][r][CTL])
				{
					Write_FPGA_Reg(Reg_DSP_Common_MIMO_Addr[m][r], *pReg_DSP_Common_MIMO_Addr[m][r][CTL], "DSP_Common_MIMO");
					fail++;
					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf( DEBUG_SYSTEM, "[RCU] mimo %d, Reg_DSP_Common_MIMO (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
						m, r, 
						Reg_DSP_Common_MIMO_Addr[m][r],
						*pReg_DSP_Common_MIMO_Addr[m][r][STS],
						*pReg_DSP_Common_MIMO_Addr[m][r][CTL]);
					#endif	
				}
				else Reg_DSP_Common_MIMO_flag[m][r] = 0;
			}
		}
		Reg_DSP_Common_MIMO__Cnt[m] = fail;
		tot_fail += fail;
	}
	

//------------------------------------------------------------
// General-Dsp-FA/Port- MIMO #0 ~ #3
//------------------------------------------------------------
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(fail = 0, r = 0; r < Reg_DSP_FA_Common_MAX; r++)
		{			
			if(pReg_DSP_FA_Common_Addr[fa][r][STS] != NULL) 
			{
				if(Reg_DSP_FA_Common_flag[fa][r] == 0) continue;
				
				if(*pReg_DSP_FA_Common_Addr[fa][r][STS] != *pReg_DSP_FA_Common_Addr[fa][r][CTL])
				{
					Write_FPGA_Reg(Reg_DSP_FA_Common_Addr[fa][r], *pReg_DSP_FA_Common_Addr[fa][r][CTL], "DSP_FA_Common");
					fail++;

					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf( DEBUG_SYSTEM, "[RCU] fa %d, Reg_DSP_FA_Common_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
							fa, r, 
							Reg_DSP_FA_Common_Addr[fa][r],
							*pReg_DSP_FA_Common_Addr[fa][r][STS],
							*pReg_DSP_FA_Common_Addr[fa][r][CTL]);
					#endif		
				}
				else Reg_DSP_FA_Common_flag[fa][r] = 0;
			}
		}
		Reg_DSP_FA_Common__Cnt[fa] = fail;
		tot_fail += fail;
	}
	
	for(fa = 0; fa < 2; fa++) // fa #0/#1
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			for(fail = 0, r = 0; r < Reg_DSP_FA_MIMO_MAX; r++)
			{
				#ifdef __TYPE__PRU__
				if(Is_AAU_Only_Reg_DSP_FA_MIMO(r)) continue; 
				#endif
				
				if(pReg_DSP_FA_MIMO_Addr[fa][m][r][STS] != NULL) 
				{
					if(Reg_DSP_FA_MIMO_flag[fa][m][r] == 0) continue;
					
					if(*pReg_DSP_FA_MIMO_Addr[fa][m][r][STS] != *pReg_DSP_FA_MIMO_Addr[fa][m][r][CTL])
					{
						Write_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][r], *pReg_DSP_FA_MIMO_Addr[fa][m][r][CTL], "DSP_FA_MIMO");
						fail++;

						#ifdef _Show_Reg_Cmp_Debug_
						D_Printf( DEBUG_SYSTEM, "[RCU] fa %d, m %d, Reg_DSP_FA_MIMO_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
							fa, m, r, 
							Reg_DSP_FA_MIMO_Addr[fa][m][r],
							*pReg_DSP_FA_MIMO_Addr[fa][m][r][STS],
							*pReg_DSP_FA_MIMO_Addr[fa][m][r][CTL]);
						#endif	
					}
					else Reg_DSP_FA_MIMO_flag[fa][m][r] = 0;
				}
			}
			Reg_DSP_FA_MIMO__Cnt[fa][m] = fail;
			tot_fail += fail;
		}
	}
//------------------------------------------------------------
// Vss-CPRI-DL/UL
//------------------------------------------------------------
	Proc_Compare_Fpga_Register_VSS();	
//------------------------------------------------------------
// 'Mode General-TDD'
//------------------------------------------------------------	
	for(fail = 0, r = 0; r < Reg_TDD_MAX; r++)
	{
		if(pReg_TDD_Addr[r][STS] != NULL) 
		{		
			if(Reg_TDD_flag[r] == 0) continue; // 2019.03.06,fixed by haes
			
			if(*pReg_TDD_Addr[r][STS] != *pReg_TDD_Addr[r][CTL])
			{
				Write_FPGA_Reg(Reg_TDD_Addr[r], *pReg_TDD_Addr[r][CTL], "TDD");
				fail++;

				#ifdef _Show_Reg_Cmp_Debug_
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
					r, 
					Reg_TDD_Addr[r],
					*pReg_TDD_Addr[r][STS],
					*pReg_TDD_Addr[r][CTL]);
				#endif	
			}
			else Reg_TDD_flag[r] = 0;
		}		
	}
	Reg_TDD__Cnt = fail;
	tot_fail += fail;
	//----------------------------------------

	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(fail = 0, r = 0; r < Reg_TDD2_MAX; r++)
		{
			if(pReg_TDD2_Addr[fa][r][STS] != NULL) 
			{		
				if(Reg_TDD2_flag[fa][r] == 0) continue;
				
				if(*pReg_TDD2_Addr[fa][r][STS] != *pReg_TDD2_Addr[fa][r][CTL])
				{
					Write_FPGA_Reg(Reg_TDD2_Addr[fa][r], *pReg_TDD2_Addr[fa][r][CTL], "TDD2");
					fail++;

					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD2_Addr, fa %d (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
						fa, r, 
						Reg_TDD2_Addr[fa][r],
						*pReg_TDD2_Addr[fa][r][STS],
						*pReg_TDD2_Addr[fa][r][CTL]);
					#endif	
				}
				else Reg_TDD2_flag[fa][r] = 0;
			}			
		}
		Reg_TDD2__Cnt[fa] = fail;
		tot_fail += fail;
	}


	//------------------------------------------------------------
	// Comp OnOff
	//------------------------------------------------------------
	for(m = 0; m < CPRI_PATH; m++) // ALC
	{
		fail = 0;
		if(pReg_COMP_ONOFF_Addr[m][STS] != NULL)
		{
			if(Reg_COMP_ONOFF_flag[m] == 0) continue;
			
			if(*pReg_COMP_ONOFF_Addr[m][STS] != *pReg_COMP_ONOFF_Addr[m][CTL])
			{
				Write_FPGA_Reg(Reg_COMP_ONOFF_Addr[m], *pReg_COMP_ONOFF_Addr[m][CTL], "COMP_ON/OFF");
				fail++;
				
				#ifdef _Show_Reg_Cmp_Debug_
				D_Printf( DEBUG_SYSTEM, "[RCU] pReg_COMP_ONOFF_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
					m, 
					Reg_COMP_ONOFF_Addr[m],
					*pReg_COMP_ONOFF_Addr[m][STS],
					*pReg_COMP_ONOFF_Addr[m][CTL]);
				#endif	
			}
			else
			{
				Reg_COMP_ONOFF_flag[m] = 0;
				#if 0
				D_Printf( DEBUG_SYSTEM, "[RCU] pReg_COMP_ONOFF_Addr (0x%02X) 0x%08X, 0x%08X == 0x%08X .\n", 
					m, 
					Reg_COMP_ONOFF_Addr[m],
					*pReg_COMP_ONOFF_Addr[m][STS],
					*pReg_COMP_ONOFF_Addr[m][CTL]);
				#endif	
			}
		}
		Reg_COMP_ONOFF_Cnt[m] = fail;
		tot_fail += fail;
	}	


//---------------------------------------------------------------------
// PRU & AAU
// CFR
//---------------------------------------------------------------------
	for(m = 0; m < MAX_DL; m++)
	{
		for(fail = 0, r = 0; r < Reg_CFR_MAX; r++)
		{			
			if(pReg_CFR_Addr[m][r][STS] != NULL)
			{		  
				if(Reg_CFR_flag[m][r] == 0) continue;
				
				if(*pReg_CFR_Addr[m][r][STS] != *pReg_CFR_Addr[m][r][CTL])
				{
					Write_FPGA_Reg(Reg_CFR_Addr[m][r], *pReg_CFR_Addr[m][r][CTL], "CFR");
					fail++;
					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf( DEBUG_SYSTEM, "[RCU] path %d, Reg_CFR (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X.\n", 
						m, r, 
						Reg_CFR_Addr[m][r],
						*pReg_CFR_Addr[m][r][STS],
						*pReg_CFR_Addr[m][r][CTL]);
					#endif	
				}
				else Reg_CFR_flag[m][r] = 0;
			}
		}
		Reg_CFR__Cnt[m] = fail;
		tot_fail += fail;
	}

	//---------------------------------------------------------------------
	// PRU
	// DPD
	//---------------------------------------------------------------------
	for(m = 0; m < MAX_DL; m++)
	{
		for(fail = 0, r = 0; r < Reg_DPD_MAX; r++)
		{			
			if(pReg_DPD_Addr[m][r][STS] != NULL)
			{		  
				if(Reg_DPD_flag[m][r] == 0) continue;
				
				if(*pReg_DPD_Addr[m][r][STS] != *pReg_DPD_Addr[m][r][CTL])
				{
					Write_FPGA_Reg(Reg_DPD_Addr[m][r], *pReg_DPD_Addr[m][r][CTL], "DPD");
					fail++;
					#ifdef _Show_Reg_Cmp_Debug_
					D_Printf( DEBUG_SYSTEM, "[RCU] path %d, Reg_DPD (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X.\n", 
						m, r, 
						Reg_DPD_Addr[m][r],
						*pReg_DPD_Addr[m][r][STS],
						*pReg_DPD_Addr[m][r][CTL]);
					#endif	
					if(r == Reg_DPD_RESET)
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Reg_DPD_RESET] path %d, Sts 0x%08X != Ctl 0x%08X.", 
							m, 
							*pReg_DPD_Addr[m][r][STS],
							*pReg_DPD_Addr[m][r][CTL]);

						if(Check_PAU_Use(m)==0)
						{
							*pReg_DPD_Addr[m][r][CTL] = 1;
							
							print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[Reg_DPD_RESET] path %d Not_Used, Ctl 0x%08X.", 
								m, 
								*pReg_DPD_Addr[m][r][CTL]);
						}
					}
					/*
					// DPD core
					for(m = DL_0; m < MAX_DL; m++ )
					{
						m_Reg_Flg.DPD[m].DPD_Reset.uD32 = _SET_;
						m_Reg_Ctl.DPD[m].DPD_Reset.uD32 = 1; // DPD Normal
					}
					*/
				}
				else Reg_DPD_flag[m][r] = 0;
			}
		}
		Reg_DPD__Cnt[m] = fail;
		tot_fail += fail;
	}

	// 2019.05.01
	for(fail = 0, r = 0; r < Reg_DPD2_MAX; r++)
	{			
		if(pReg_DPD2_Addr[r][STS] != NULL)
		{		  
			if(Reg_DPD2_flag[r] == 0) continue;
			
			if(*pReg_DPD2_Addr[r][STS] != *pReg_DPD2_Addr[r][CTL])
			{
				Write_FPGA_Reg(Reg_DPD2_Addr[r], *pReg_DPD2_Addr[r][CTL], "DPD2");
				fail++;
				#ifdef _Show_Reg_Cmp_Debug_
				D_Printf( DEBUG_SYSTEM, "[RCU] Reg_DPD2 (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X.\n", 
					r, 
					Reg_DPD2_Addr[r],
					*pReg_DPD2_Addr[r][STS],
					*pReg_DPD2_Addr[r][CTL]);
				#endif	
			}
			else Reg_DPD2_flag[r] = 0;
		}
	}
	Reg_DPD2__Cnt = fail;
	tot_fail += fail;	

//----------------------------------------
//	Display Fail Count
//----------------------------------------		
	
#if 0
	if(tot_fail > 0)
	{
		D_Printf( BIT(1), "[RCU] Register Total Fail %d-----------.\n", tot_fail);
		if(Reg_Common__Cnt > 0) 
			D_Printf(DEBUG_SYSTEM, "[RCU] Reg_Common__Cnt Fail %d.\n", Reg_Common__Cnt);

		//	SFP Common : CPRI #0 ~ CPRI #3
		for(c = 0; c < CPRI_PATH; c++)
		{
			if(Reg_Common_MIMO__Cnt[c] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_Common_MIMO__Cnt[%d] Fail %d.\n", 
					c, Reg_Common_MIMO__Cnt[c]);
		}

		// DSP-Delay	
		if(Reg_DSP_Common__Cnt > 0) 
			D_Printf(DEBUG_SYSTEM, "[RCU] Reg_DSP_Common__Cnt Fail %d.\n", Reg_DSP_Common__Cnt);		

		// General-Dsp-FA/Port- MIMO #0 ~ #3
		for(fa = 0; fa < 2; fa++)
		{
			if(Reg_DSP_FA_Common__Cnt[fa] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_DSP_FA_Common__Cnt[%d] Fail %d.\n", 
					fa, Reg_DSP_FA_Common__Cnt[fa]);
		
			for(m = 0; m < MAX_MIMO_PATH; m++)
			{	
				if(Reg_DSP_FA_MIMO__Cnt[fa][m] > 0) 
					D_Printf(DEBUG_SYSTEM, "[RCU] Reg_DSP_FA_MIMO__Cnt[%d][%d] Fail %d.\n",
						fa, m, Reg_DSP_FA_MIMO__Cnt[fa][m]);
			}
		}

		// Vss-CPRI-DL/UL
		for(c = CPRI_0; c < CPRI_PATH; c++)
		{
			if(Reg_VSS_DL__Cnt[c] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_VSS_DL__Cnt[%d] Fail %d.\n", c, Reg_VSS_DL__Cnt[c]);
			if(Reg_VSS_UL__Cnt[c] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_VSS_UL__Cnt[%d] Fail %d.\n", c, Reg_VSS_UL__Cnt[c]);
		}

		// 'Mode General-TDD'	
		if(Reg_TDD__Cnt > 0) 
			D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD__Cnt Fail %d.\n", Reg_TDD__Cnt);

		if(Reg_TDD2__Cnt[0] > 0) 
			D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD2__Cnt, fa#1 Fail %d.\n", Reg_TDD2__Cnt[0]);	

		if(Reg_TDD2__Cnt[1] > 0) 
			D_Printf(DEBUG_SYSTEM, "[RCU] Reg_TDD2__Cnt, fa#2 Fail %d.\n", Reg_TDD2__Cnt[1]);
		
		// RF : AAU Only
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{	
			if(Reg_RFU_CTRL__Cnt[m] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_RFU_CTRL__Cnt[%d] Fail %d.\n", 
					m, Reg_RFU_CTRL__Cnt[m]);
				
			if(Reg_ALC__Cnt[m] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_ALC__Cnt[%d] Fail %d.\n", 
					m, Reg_ALC__Cnt[m]);
		}

		for(m = 0; m < CPRI_PATH; m++) // Compensation
		{
			if(Reg_COMP_ONOFF_Cnt[m] > 0) 
				D_Printf(DEBUG_SYSTEM, "[RCU] Reg_COMP_ONOFF_Cnt[%d] Fail %d.\n", 
					m, Reg_COMP_ONOFF_Cnt[m]);
		}
	}
#endif
}

void Proc_Compare_Fpga_Register_RF(void) // Compare and Ctrl
{
#ifdef __TYPE__AAU__
	int m, r;
	int fail;
	int tot_fail = 0;


//------------------------------------------------------------
// RF : AAU Only
//------------------------------------------------------------
	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(fail = 0,r = 0; r < Reg_RFU_CTRL_MAX; r++) // RF
		{
		#if 0
			if(r == Reg_RFU_CTRL_RW_PA_OnOff)
			{
				D_Printf( DEBUG_SYSTEM, "[RCU] m %d, Reg_RFU_CTRL_Addr (0x%02X) st 0x%08X ~ ctrl 0x%08X---------- .\n", 
					m, r, 
					*pReg_RFU_CTRL_Addr[m][r][STS],
					*pReg_RFU_CTRL_Addr[m][r][CTL]);
			}
		#endif	
		
			if(pReg_RFU_CTRL_Addr[m][r][STS] != NULL)
			{
				if(Reg_RFU_CTRL_flag[m][r] == 0) continue;

				if(r < Reg_RFU_CTRL_RW_PA_OnOff)
				{	
					if(*pReg_RFU_CTRL_Addr[m][r][STS] != ((*pReg_RFU_CTRL_Addr[m][r][CTL])&0x000000ff))		//triger bit clear
					{
						Write_FPGA_Reg(Reg_RFU_CTRL_Addr[m][r], *pReg_RFU_CTRL_Addr[m][r][CTL], "RFU_CTRL");
						fail++;
						
						#if 1
						D_Printf( DEBUG_SYSTEM, "[RFU_CTRL_ATT] m %d, Reg_RFU_CTRL_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
							m, r, 
							Reg_RFU_CTRL_Addr[m][r],
							*pReg_RFU_CTRL_Addr[m][r][STS],
							*pReg_RFU_CTRL_Addr[m][r][CTL]);
						#endif	
					}
					else Reg_RFU_CTRL_flag[m][r] = 0;
				}
				else
				{
					if(*pReg_RFU_CTRL_Addr[m][r][STS] != *pReg_RFU_CTRL_Addr[m][r][CTL])
					{
						Write_FPGA_Reg(Reg_RFU_CTRL_Addr[m][r], *pReg_RFU_CTRL_Addr[m][r][CTL], "RFU_CTRL");
						fail++;

						/*
						//D_Printf(DEBUG_SYSTEM, VT_BLINK_GREEN "[RFU_CTRL_PA On/off] RFU[%d] reg %d, ctrl 0x%X, eep %d\n", 
						//	m, r, 
						//	*pReg_RFU_CTRL_Addr[m][r][CTL],
						//	m_Eep.Pwr_Amp_En[m]);
						*/					
						
						#if 1
						D_Printf( DEBUG_SYSTEM, "[RFU_CTRL_Onoff] m %d, Reg_RFU_CTRL_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
							m, r, 
							Reg_RFU_CTRL_Addr[m][r],
							*pReg_RFU_CTRL_Addr[m][r][STS],
							*pReg_RFU_CTRL_Addr[m][r][CTL]);
						#endif	
					}
					else  Reg_RFU_CTRL_flag[m][r] = 0;
				}
			}
		}
		Reg_RFU_CTRL__Cnt[m] = fail;
		tot_fail += fail;
	}
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		for(fail = 0, r = 0; r < Reg_ALC_MAX; r++) // ALC
		{
			if(pReg_ALC_Addr[m][r][STS] != NULL)
			{
				if(Reg_ALC_flag[m][r] == 0) continue; // 2019.04.20 
				
				if(*pReg_ALC_Addr[m][r][STS] != *pReg_ALC_Addr[m][r][CTL])
				{
					Write_FPGA_Reg(Reg_ALC_Addr[m][r], *pReg_ALC_Addr[m][r][CTL], "ALC");
					fail++;
					
					#if 1
					D_Printf( DEBUG_SYSTEM, "[RCU] m %d, Reg_ALC_Addr (0x%02X) Fail 0x%08X, 0x%08X != 0x%08X .\n", 
						m, r, 
						Reg_ALC_Addr[m][r],
						*pReg_ALC_Addr[m][r][STS],
						*pReg_ALC_Addr[m][r][CTL]);
					#endif	
				}
				//else Reg_ALC_flag[m][r] = 0;
			}
		}
		Reg_ALC__Cnt[m] = fail;
		tot_fail += fail;
	}	
	
#endif
}





/*
-------------------------------------------------------
	Functions for RCU-APP
-------------------------------------------------------
*/

/*
-------------------------------------------------------
	Semaphore로 막아져있지 않으므로,
	FPGA Task에서 순차적으로 호출 되어야 함.
	다른 곳에서 임의로 호출 하면 안됨.
-------------------------------------------------------
*/ 

INT32U Read_FPGA_Reg(INT32U addr32)
{	
#ifdef __TYPE__PRU__	
	return Rd_AXI_Reg(addr32); // for read	
#else
	return Rd_SPI_FPGA(addr32|0x80000000); // read is MSB Set
#endif	
}

/*
-------------------------------------------------------
	Semaphore로 막아져있지 않으므로,
	FPGA Task에서 순차적으로 호출 되어야 함.
	다른 곳에서 임의로 호출 하면 안됨.
-------------------------------------------------------
*/ 
INT32U Write_FPGA_Reg(INT32U addr32, INT32U data32, char* str)
{	
#ifdef __TYPE__PRU__
	return Wr_AXI_Reg(addr32, data32, str); // for write
#else
	return Wr_SPI_FPGA(addr32, data32, str); // read is MSB Clear
#endif
}





void Write_Ctrl_Data_From_Eep(void)
{
	INT8U i, c;	
	INT16U att_x10;	
	INT8U j;
	#ifdef __TYPE__AAU__
	//INT16U att_2_x10;
	INT16S val_x10;
	#endif

	for (i = 0; i < AMPU_4_MAX; i++)
	{
	#ifdef __TYPE__AAU__
		Update__AAU_Temp_Att(i);
		
		AAU_RF_DL_Att_Set(i, m_Eep.DL_Att_User[i]/2);
		AAU_RF_UL_Att_Set(i, m_Eep.UL_Att[i]/2);

		if(m_Eep.Pwr_Amp_En[i] != ON)
			m_Eep.Pwr_Amp_En[i] = OFF;

		if(m_Eep.RVS_Amp_Onoff[i] != ON)
			m_Eep.RVS_Amp_Onoff[i] = OFF;

		if(m_Eep.UL_AFE_ATT_x1[i] > 20)
			m_Eep.UL_AFE_ATT_x1[i] = 0;
		AAU_UL_AFE_Att_Set(i, m_Eep.UL_AFE_ATT_x1[i]);
	#endif	
	}



	for( i = 0; i < MAX_FA_PATH; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_DL_EQ_Coeff.uD32 = 1;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_DL_EQ_Coeff.uD16[0] = m_Eep.d_EQ_Coeff_DL[i][j]*m_St.EQ_Toggle[i][DL][0];
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_DL_EQ_Coeff.uD16[1] = 0;
			

			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_UL_EQ_Coeff.uD32 = 1;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_EQ_Coeff.uD16[0] = m_Eep.d_EQ_Coeff_UL[i][j]*m_St.EQ_Toggle[i][UL][0];	
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_EQ_Coeff.uD16[1] = 0;

		#ifdef __TYPE__PRU__ // ANTS-PRU, digital Atten, 2019.05.08
			if(m_Eep.d_Att_Dig_DL_x10[i][j] > 10*10) // range is max 10dB
				m_Eep.d_Att_Dig_DL_x10[i][j] = 30; // default 3.0[dB], 2019.07.04
				
			att_x10 = Chk_Dig_Max_Att(m_Eep.d_Att_Dig_DL_x10[i][j]);
			m_Eep.d_Att_Dig_DL_x10[i][j] = att_x10;
			PRU_DL_Gain_Set_Req(i, j, att_x10);

			if(m_Eep.d_Att_Dig_UL_Offs_x10[i][j] > 20*10)
				m_Eep.d_Att_Dig_UL_Offs_x10[i][j] = 30; // default 3.0[dB], 2019.07.04			

			att_x10 = Chk_Dig_Max_Att(m_Eep.d_Att_Dig_UL_Offs_x10[i][j]);
			m_Eep.d_Att_Dig_UL_Offs_x10[i][j] = att_x10;
			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_UL_Gain.uD32 = 1;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_Gain.uD32 = att_x10; // @ Reboot, gain_backoff is 0dB
			
		#else //------------------------------------ ANTS-AAU
			att_x10 = Chk_Dig_Max_Att(m_Eep.d_Att_Dig_DL_x10[i][j]);
			m_Eep.d_Att_Dig_DL_x10[i][j] = att_x10;
			//------------------------------------------			
			val_x10 = Get__AAU_DL_Gain_Att_x10(i, j);
			//------------------------------------------
			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_DL_Gain.uD32 = 1;			
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_DL_Gain.uD32 = Get_GainRegVal(DL, (att_x10+val_x10)/5);

			//------------------------------------------------------------
			if(m_Eep.d_Att_Dig_UL_Offs_x10[i][j] > 40*10) 
				m_Eep.d_Att_Dig_UL_Offs_x10[i][j] = 0;
			else 
				m_Eep.d_Att_Dig_UL_Offs_x10[i][j] = m_Eep.d_Att_Dig_UL_Offs_x10[i][j];

			if(m_Eep.d_Att_Dig_UL_User_x10[i][j] > 40*10) 
				m_Eep.d_Att_Dig_UL_User_x10[i][j] = 0;
			else 
				m_Eep.d_Att_Dig_UL_User_x10[i][j] = m_Eep.d_Att_Dig_UL_User_x10[i][j];
			//------------------------------------------			
			val_x10 = Get__AAU_UL_Gain_Att_x10(i, j);
			//------------------------------------------
			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_UL_Gain.uD32 = 1;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_Gain.uD32 = Get_GainRegVal(UL, val_x10/5);
		#endif


			#ifdef __TYPE__PRU__ // ANTS-PRU, digital Atten	
			// 2019.04.18.
			if(m_Eep.d_UL_Att_x10[i][j] > 20*10) // max 20dB
				m_Eep.d_UL_Att_x10[i][j] = 0;
				
			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_UL_OUTPUT_ATT.uD32 = _F_SET_;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_OUTPUT_ATT.uD32 = m_Eep.d_UL_Att_x10[i][j];
			#else			
			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_DL_EQ_1_Coeff.uD32 = 1;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_DL_EQ_1_Coeff.uD16[0] = m_Eep.d_EQ_Coeff_1_DL[i][j]*m_St.EQ_Toggle[i][DL][1];
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_DL_EQ_1_Coeff.uD16[1] = 0;

			m_Reg_Flg.General_FA[i].General_FA_MIMO[j].RW_UL_EQ_1_Coeff.uD32 = 1;
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_EQ_1_Coeff.uD16[0] = m_Eep.d_EQ_Coeff_1_UL[i][j]*m_St.EQ_Toggle[i][UL][1];
			m_Reg_Ctl.General_FA[i].General_FA_MIMO[j].RW_UL_EQ_1_Coeff.uD16[1] = 0;
			#endif
		}
	}	

	for(i = 0; i < MAX_DL; i++) // 2019.04.25
	{		
	#ifdef __TYPE__PRU__
		// 2019.05.31 : Max/min limit 없음. user controll 대로.		
		Use_d_HC_Threshold(i, m_Eep.d_Hard_Clip_Lvl_x10[i]);
	#else	
		Use_d_HC_Threshold_AAU(i, m_Eep.d_Hard_Clip_Lvl_x10[i]);
	#endif	
	}	

	/*
	--------------------------------------------
	Max-min check is needed...
	--------------------------------------------
	*/	
	
	if(m_Eep.RW_TDD_Mode >= 0x04) m_Eep.RW_TDD_Mode = 0; // 2019.04.11: FDD(0x03) added.
	m_Reg_Flg.RW_TDD_Mode.Mode = 1;
	m_Reg_Ctl.RW_TDD_Mode.Mode = m_Eep.RW_TDD_Mode;
	m_Reg_Sts.RW_TDD_Mode.Data = Read_FPGA_Reg( Reg_TDD_Addr[Reg_TDD_RW_TDD_Mode]);
	
	//print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "RW_TDD_Mode REQ %d, read %d", 
	//	m_Reg_Ctl.RW_TDD_Mode.Mode, m_Reg_Sts.RW_TDD_Mode.Data);

	for(c = 0; c < MAX_Optic; c++)
	{
		if(m_Eep.d_Toffset[c] > 100) 
			m_Eep.d_Toffset[c] = 0;		
		m_Reg_Flg.DLY[c].t_Offset.uD32 = 1;
		m_Reg_Ctl.DLY[c].t_Offset.uD32 = conv_Reg_GuardTime_from_usec(m_Eep.d_Toffset[c]);

		if(m_Eep.d_TBdelay_DL[c] > 100) 
			m_Eep.d_TBdelay_DL[c] = 0;
		m_Reg_Flg.DLY[c].TB_Delay_DL.uD32 = 1;
		m_Reg_Ctl.DLY[c].TB_Delay_DL.uD32 = conv_Reg_GuardTime_from_usec(m_Eep.d_TBdelay_DL[c]);


		if(m_Eep.d_TBdelay_UL[c] > 100) 
			m_Eep.d_TBdelay_UL[c] = 0;
		m_Reg_Flg.DLY[c].TB_Delay_UL.uD32 = 1;
		m_Reg_Ctl.DLY[c].TB_Delay_UL.uD32 = conv_Reg_GuardTime_from_usec(m_Eep.d_TBdelay_UL[c]);
	}
	//------------------------------------------------------------------
	for( i = 0; i < 5; i++ )
	{
		if(m_Eep.d_UL_Gain_Guard_time[i] > 72) // max 72us
			m_Eep.d_UL_Gain_Guard_time[i] = 0;

		m_Reg_Flg.UL_GAIN_Guard_Time[i].uD32 = 1;
		m_Reg_Ctl.UL_GAIN_Guard_Time[i].uD32 = conv_Reg_GuardTime_from_usec(m_Eep.d_UL_Gain_Guard_time[i]);
	}
	//------------------------------------------------------------------
	/*
	// 2019.05.24.
	//if(m_Eep.d_UL_Gain_Meas_time > 500) // max 124us ??
	//	m_Eep.d_UL_Gain_Meas_time = 0;	
	//m_Reg_Flg.UL_GAIN_Measure_Time.uD32 = 1;
	//m_Reg_Ctl.UL_GAIN_Measure_Time.uD32 = conv_Reg_GuardTime_from_usec(m_Eep.d_UL_Gain_Meas_time);	
	*/
	
	//------------------------------------------------------------------
		
	// 2019.05.28 : PRU and AAU
	#ifdef __TYPE__PRU__
	Use_TSYNC_Delay_Set_PRU(m_Eep.d_t_Sync_Delay);
	#else
	Use_TSYNC_Delay_Set_AAU(m_Eep.d_t_Sync_Delay);
	#endif
	//------------------------------------------------------------------
	
	//-----------------------------------------------------------------
	// max/min check
	if(m_Eep.d_RE_Channel_BW[_100M] < BW__10M) m_Eep.d_RE_Channel_BW[_100M] = BW_100M; // vv
	if(m_Eep.d_RE_Channel_BW[_100M] > BW_100M) m_Eep.d_RE_Channel_BW[_100M] = BW_100M; // vv

	if(m_Eep.d_RE_Channel_BW[Alpha] < BW__10M) m_Eep.d_RE_Channel_BW[Alpha] = BW_100M; // vv
	if(m_Eep.d_RE_Channel_BW[Alpha] > BW_100M) m_Eep.d_RE_Channel_BW[Alpha] = BW_100M; // vv

	RE_Channel_BandWidth(m_Eep.d_RE_Channel_BW[_100M], m_Eep.d_RE_Channel_BW[Alpha]);
	Update_ARFCN_from_BW();	
	// after BW-Control
	for( i = 0; i < MAX_FA_PATH; i++ )
	{		
		//-------------------------------------
	#ifdef __TYPE__PRU__
		m_Reg_Flg.General_FA[i].RW_PRU_DL_NCO.uD32 = 1;
		m_Reg_Ctl.General_FA[i].RW_PRU_DL_NCO.uD32 = Use_d_Cal_VCO_PRU_1khz(i, DL, m_Eep.d_CenterFreq[i]);

		m_Reg_Flg.General_FA[i].RW_PRU_UL_NCO.uD32 = 1;
		m_Reg_Ctl.General_FA[i].RW_PRU_UL_NCO.uD32 = Use_d_Cal_VCO_PRU_1khz(i, UL, m_Eep.d_CenterFreq[i]);

		D_Printf(DEBUG_SYSTEM, "[Write_Ctrl_Data From Eep] fa(%d) RW_NCO 0x%08X,  0x%08X\n", 
			i, 
			m_Reg_Ctl.General_FA[i].RW_PRU_DL_NCO.uD32, 
			m_Reg_Ctl.General_FA[i].RW_PRU_UL_NCO.uD32);
	#else // ANTS-AAU
		m_Reg_Flg.General_FA[i].RW_NCO.uD32 = 1;
		m_Reg_Ctl.General_FA[i].RW_NCO.uD32 = Use_d_Cal_VCO_AAU_1khz(m_Eep.d_CenterFreq[i]);	

		D_Printf(DEBUG_SYSTEM, "[Write_Ctrl_Data From Eep] fa(%d) RW_NCO 0x%08X\n", 
			i, 
			m_Reg_Ctl.General_FA[i].RW_NCO.uD32);
		
		printf(VT_BOLD_YELLOW "[RCU] FA%d Fc %d by Init" VT_END_CR, i, m_Eep.d_CenterFreq[i]);
		D_Printf(DEBUG_SYSTEM, VT_GREEN "[ctrl] FA%d Fc %d by Init", i, m_Eep.d_CenterFreq[i]);			
	#endif		
	}
	//-----------------------------------------------------------------
	#ifdef __TYPE__PRU__	
	for(i = 0; i < MAX_DL; i++)
	{
		Use_d_DPD_OnOff(i, m_Eep.d_DPD_OnOff[i]);

		if(m_Eep.d_Att_DL_SUM[i] > 10*10) // range is max 10dB
			m_Eep.d_Att_DL_SUM[i] = 30; // default 3.0[dB], 2019.07.04
			
		Att_Digital_DL_SUM(i, m_Eep.d_Att_DL_SUM[i]);
	}
	//-----------------------------------------------------------------
	#else	

	//-----------------------------------------------	
	AAU__Gain_Check_Mode_Ctrl(0); // default UL Noise Mode
	

	if(m_Eep.DL_Mode_En == 1) // AID '1' is OFF 
	{
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_1 = 0;
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_0 = 0;
	}
	else // AID '0' is ON 
	{
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_1 = 1;
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_0 = 1;
	}	
	m_Reg_Flg.Gain_Check_Mode.onoff = _SET_;
	//-----------------------------------------------
	
	#endif
	Use_d_Rx_Casc_Add_OnOff_UL(m_Eep.d_Rx_Casc_Add_OnOff_UL); // AID 0xB435

	for(c = 0; c < MAX_Optic; c++) // 2019.04.18, PRU & AAU
	{
		for(i = 0; i < MAX_DL; i++)
		{
			Use_d_Payload_OnOff(c, i, m_Eep.d_Payload_OnOff[c][i]);
		}
	}

	// 2019.04.15 : PRU and AAU
	for(i = 0; i < MAX_DL; i++)
	{
		if(m_Eep.d_CFR_OnOff[i] != OFF) 
			m_Eep.d_CFR_OnOff[i] = ON;
		Use_d_CFR_OnOff(i, m_Eep.d_CFR_OnOff[i]);
		
		#ifdef __TYPE__PRU__
		Use_d_CFR_Level(i, m_Eep.d_CFR_Level_x10[i]);
		#else
		Use_d_PAR_Level(i, m_Eep.d_PAR_x10[i]);
		#endif
	}


	// 2019.04.17 added	
	for(c = 0; c < MAX_Optic; c++)
	{
		for(i = 0; i < MAX_MIMO_PATH; i++)
		{
			Use_DTU_DL_Path_Onoff(c, i, m_Eep.d_Path_OnOff_DL[c][i]);
			Use_DTU_UL_Path_Onoff(c, i, m_Eep.d_Path_OnOff_UL[c][i]);
		}
	}

	// 2019.04.17 added	
	Use_DL_ON_Guard_Time_Ctrl(m_Eep.d_DL_ON_Guard_Time);
	Use_UL_ON_Guard_Time_Ctrl(m_Eep.d_UL_ON_Guard_Time);
	
	Use_UL_to_DL_Guard_Time_Ctrl(m_Eep.d_UL_to_DL_Guard_Time);
	Use_DL_to_UL_Guard_Time_Ctrl(m_Eep.d_DL_to_UL_Guard_Time);

	// 2019.05.16
	for(c = 0; c < MAX_Optic; c++)
	{
		for(i = 0; i < MAX_MIMO_PATH; i++)
		{
			#ifdef __TYPE__PRU__ // 
			Use_DL_CW_Level_Ctrl(c, i, m_Eep.d_DL_CW_LEVEL[c][i]);
			Use_DL_CW_OnOff_Ctrl(c, i, 0); // '0' is OFF (both @AID and @register)
			#else
			Use_DL_CW_Level_Ctrl(c, i, 0); // AAU, default 0			
			Use_DL_CW_OnOff_Ctrl(c, i, 0); // '0' is OFF (both @AID and @register)
			m_St.User_DL_CW_OnOff[c][i] = 0; // 2019.09.25
			#endif			
		}
	}

	#ifdef __TYPE__PRU__ // 
	for(c = 0; c < MAX_Optic; c++)
	{
		for(i = 0; i < MAX_MIMO_PATH; i++)
		{	
			TSSI_SD_Level_Set(c, i, m_Eep.DL_TSSI_UP_x10[c][i]);
		}
	}
	#endif

	
	

	// 2019.05.16	
	VSS_UL_RE_Reset_ACK_Sw_Default(); // 2019.05.17

	Proc_Compare_Fpga_Register();
	Proc_Compare_Fpga_Register_RF();

	m_Env.is_eep_write = EEP_WRITE_CODE;
}


///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////


/*
BW 별 TSSI Input Power Balance ATT 조정은 아래 테이블과 같이 fix 된 값이 제어
제어시에는 현재 있는 DL_Gain Register에 sum하여 제어 
	
-----------------------
      Main-att  A-att　 
-----------------------     
100M 	0		0
140M	1.5		1.5
160M	2		2
180M	2.5		2.5
200M	3		3
-----------------------
*/

INT16U Get_Curr_TSSI_Input_Pwr_Bal_Att_x10(INT8U BW_A)
{
	INT16U att_x10;
	static INT8U bw_prev = 0xFF;
	
	switch(BW_A)
	{
		case 6 : att_x10 = 30; break; // 100M
		case 5 : att_x10 = 25; break; // 80M
		case 4 : att_x10 = 20; break; // 60M
		case 3 : att_x10 = 15; break; // 40M
		default: att_x10 = 0;  break; // 20M/10M/0
	}	

	if(bw_prev != BW_A)
	{
		bw_prev = BW_A;
		print_agent(DEBUG_SYSTEM, VT_MAGENTA, "[Get_Curr_TSSI_Input_Pwr_Bal_Att_x10] BW_A %d, att_x10 %d", 
						BW_A, att_x10);		
	}

	// 2019.08.06
	if(m_Eep.Height_Bal_Onoff == 0) // ON(1), OFF(0)
		att_x10 = 0;
	
	return att_x10;
}

void PRU_DL_Gain_Set_Req(INT8U fa, INT8U path, INT16U att_x10)
{
	INT16U bal_att_x10;
	INT8U bw_alpha;
	
	if(fa >= MAX_FA_PATH) return;
	if(path >= MAX_MIMO_PATH) return;
	
	m_Reg_Flg.General_FA[fa].General_FA_MIMO[path].RW_DL_Gain.uD32 = 1; 		
	m_Reg_Ctl.General_FA[fa].General_FA_MIMO[path].RW_DL_Gain.uD32 = att_x10;

	///////bw_alpha = m_St.d_RW_DL_BW_Sel[FA_1][MIMO_0]; // A Mhz path 1st
	bw_alpha = m_Eep.d_RE_Channel_BW[Alpha];
	bal_att_x10 = Get_Curr_TSSI_Input_Pwr_Bal_Att_x10(bw_alpha);
	Height_Bal_Gain_Set(fa, path, bal_att_x10);
}


void Proc__PRU_Bal_DTU_Att(void)
{
	static INT8U prev_bw = 0;
	static INT8U height_bal_onoff = 0xFF;
	INT8U bw_alpha;
	INT8U optic;
	INT8U path4;
	
	///////bw_alpha = m_St.d_RW_DL_BW_Sel[FA_1][MIMO_0]; // A Mhz path 1st
	bw_alpha = m_Eep.d_RE_Channel_BW[Alpha];
	
	if((prev_bw != bw_alpha)||(height_bal_onoff != m_Eep.Height_Bal_Onoff))
	{
		for(optic = DL_0; optic < MAX_Optic; optic++)
		{
			for(path4 = DL_0; path4 < MAX_DL; path4++)
			{
				PRU_DL_Gain_Set_Req(optic, path4, m_St.d_Att_Dig_DL_x10[optic][path4]);
			}
		}
	}
	prev_bw = bw_alpha;
	height_bal_onoff = m_Eep.Height_Bal_Onoff; // 2019.08.06
}


#ifdef __TYPE__AAU__ // ANTS-AAU

INT16S Get__AAU_DL_Gain_Att_x10(INT8U optic, INT8U path)
{
	INT16S val_x10 = 0;
	
	if(path >= MAX_PATH_4) return 0;
	
	val_x10 = Get_AAU_DTU_Alpha_Temp_Att_x10(optic, path, DL);
	val_x10 += m_St.d_DL_TSSI_BalAtt_x10[optic][path];
	val_x10 -= m_St.Curr_LS_Gain_Bal_x10[optic][DL][path]; // L/S Gain Balance

	/*
	2019.11.28 : 
		ATT 이지만 digital gain 제어와 관련 되므로, 
		음수이더라도 0으로 하면 안됨.
	///if(val_x10 < 0) val_x10 = 0;
	*/
	
	return val_x10;		
}

void AAU_DTU_ATT_Refresh(INT8U optic, INT8U path, INT8U dlul)
{
	INT16U att_x10;
	INT16S val_x10;
	INT32U cal_u32;
	
	if(optic >= MAX_Optic) return;
	if(path >= MAX_DL) return;
	if(dlul >= DL_UL) return;


	if(dlul == DL)
	{
		att_x10 = m_St.d_Att_Dig_DL_x10[optic][path];
		val_x10 = Get__AAU_DL_Gain_Att_x10(optic, path);
		
		cal_u32 = Get_GainRegVal(dlul, (att_x10 + val_x10)/5);
		m_Reg_Flg.General_FA[optic].General_FA_MIMO[path].RW_DL_Gain.uD32 = 1;
		m_Reg_Ctl.General_FA[optic].General_FA_MIMO[path].RW_DL_Gain.uD32 = cal_u32;
		
		//print_agent(DEBUG_SYSTEM, VT_YELLOW, "[AAU_DTU_ATT_Refresh] optic %d, path %d, DL reg 0x%X", 
		//		optic, path, cal_u32);
	}
	else
	{
		Set_ALC_Gain_Comp_x10(optic, path, m_St.AAU_ALC_Gain_Comp_x10[optic][path]);
		
		//print_agent(DEBUG_SYSTEM, VT_YELLOW, "[AAU_DTU_ATT_Refresh] optic %d, path %d, UL ALC Gain Comp %d/10", 
		//		optic, path, m_St.AAU_ALC_Gain_Comp_x10[optic][path]);
	}
}
#endif


static INT8U DTU_Reg_Link_Fail_Co = 0;

/*
-------------------------------------------------------
	Check 
	FPGA Register-Comm is Ready or not

	if Write and Read is Success, return 0
-------------------------------------------------------
*/

int Check_Fpga_Comm_Link(void)
{
	static INT32U increase_cnt = 1;
	static INT32U success__cnt = 0;
	INT32U read_cnt;

	/*
	"Reg_Common_R_App_MB_Counter" 사용.
	*/

	increase_cnt++;
	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Test], increase_cnt, "Common_RW_Test");
	read_cnt = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Test]);
	
	if(read_cnt == increase_cnt)
	{
		//D_Printf(DEBUG_SYSTEM, "[RCU] Reg_Common_RW_Test %d == %d.\n", read_cnt , increase_cnt);
		DTU_Reg_Link_Fail = 0;
		DTU_Reg_Link_Fail_Co = 0;
		
		success__cnt++;
		if(success__cnt >= 100)
			success__cnt = 100;
	}
	else
	{
		//print_agent(DEBUG_SYSTEM, VT_RED, "FPGA-Reg-Task Init. (Clock Normal)");
		print_agent(DEBUG_SYSTEM, VT_RED, "Reg_RW_Test Fail, read %d != req  %d.\n", read_cnt , increase_cnt);
		
		DTU_Reg_Link_Fail_Co++;
		success__cnt = 0;

		if( DTU_Reg_Link_Fail_Co >= 2 )
		{
			DTU_Reg_Link_Fail_Co = 2;
			DTU_Reg_Link_Fail = 1;
		}
	}	

	if(success__cnt >= 2) // coninuous 5 times
		return CHK_OK;
	else
		return CHK_FAIL;
}

INT8U Is_DTU_Reg_CommFail(void)
{
	if(DTU_Reg_Link_Fail) 
		return 10;			
	else return 0;
}

INT8U Is_DTU_Reg_CommFail_Co(void)
{
	if(DTU_Reg_Link_Fail)  
		return 10;
	else  
		return 0;	
}


void Proc_DTU_Alarm(void)
{
	INT8U  alarm;
	INT8U  path;
	INT16S power_x10;
	INT16S limit_x10;	
	
	for(path = DL_0; path < MAX_DL; path++)
	{
		// Check Alarm Test ----------------------------------------
		if(m_St.AlmTest) // 2019.05.01
		{		
		/*
			if(m_St.AlmFlag.Path[path].DL.TSSI_Upper__) // 2019.05.01		
				m_St.AlmSt.Path[path].DL.TSSI_Upper__ = m_St.AlmCtrl.Path[path].DL.TSSI_Upper__;		

			if(m_St.AlmFlag.Path[path].DL.TSSI_Lower__) // 2019.05.01			
				m_St.AlmSt.Path[path].DL.TSSI_Lower__ = m_St.AlmCtrl.Path[path].DL.TSSI_Lower__;		

			if(m_St.AlmFlag.Path[path].UL.RSSI_Lower__) // 2019.05.01			
				m_St.AlmSt.Path[path].UL.RSSI_Lower__ = m_St.AlmCtrl.Path[path].UL.RSSI_Lower__;	
		*/
			continue;
		}
		
		// Check Alarm Test ----------------------------------------	
		
			
		if(Check_PAU_Use(path)==0) // AMP Not Use, Alarm mask
		{
			m_St.AlmSt.Path[path].DL.TSSI_Upper__ = 0;
			m_St.AlmSt.Path[path].DL.TSSI_Lower__ = 0;
			m_St.AlmSt.Path[path].UL.RSSI_Lower__ = 0;

		#ifdef __2019_1226__Use_Alarm_Timer_Structure__
			Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper__);
			Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Lower__);
		#endif
			
			continue;
		}
		//-----------------------------------------
		// 2020.01.10 : added
		if(IS__TSSI_Not_Checking(path)) //// || m_Env.RU_Init_St_Reset) // 2020.01.28 : RU Init 시, TSSI Upper Alarm Clear
		{
			Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper__);
		}
		//-----------------------------------------
		
	//-----------------------
	// DL TSSI
	//-----------------------
		power_x10 = m_St.d_DL_TSSI_dBm_x10[_100M][path];
		
	#ifdef __TYPE__PRU__
		if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27
	#endif		
		{
		// DL TSSI upper limit ------------------------------------		
			limit_x10 = m_Eep.DL_TSSI_UP_x10[_100M][path];
			alarm = m_St.AlmSt.Path[path].DL.TSSI_Upper__;
			Hysterisis_Alm_Chk(&alarm, power_x10, limit_x10, 21); // high limit
		#ifdef __TYPE__PRU__
			alarm = m_St.d_TSSI_SD_St[_100M][path];
		#endif	

		#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__
			// Clock, Cpri알람 발생 시 타 알람의 알람 유지시간 초기화
			if(Is_SFP_Inserted(CPRI_0)) 
			{
				if(IS__Clock_CPRI_Alarm())
					Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper__);
			}
		#endif
		
		#ifdef __2019_1226__Use_Alarm_Timer_Structure__
			if(Is_SFP_Inserted(CPRI_0)) 
			{
				// TSSI 상한 알람/정상 : 3s
				if(Chk__alarm_normal_timeout(&m_St.AlmTime.Path[path].DL.TSSI_Upper__, m_St.AlmSt.Path[path].DL.TSSI_Upper__, alarm ,3000, 3000))
					m_St.AlmSt.Path[path].DL.TSSI_Upper__ = 1; // update
				else
					m_St.AlmSt.Path[path].DL.TSSI_Upper__ = 0; // update
			}
			else
			{
				Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper__);
				m_St.AlmSt.Path[path].DL.TSSI_Upper__ = 0; // update
			}
		#else
			if(Is_SFP_Inserted(CPRI_0)) 
				m_St.AlmSt.Path[path].DL.TSSI_Upper__ = alarm;
			else
				m_St.AlmSt.Path[path].DL.TSSI_Upper__ = 0;
		#endif		
		// DL TSSI lower limit ------------------------------------
			limit_x10 = m_Eep.DL_TSSI_DN_x10[_100M][path];
			alarm = m_St.AlmSt.Path[path].DL.TSSI_Lower__;
			Hysterisis_Alm_Chk(&alarm, limit_x10, power_x10, 20); // low limit

		#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__	
			// Clock, Cpri알람 발생 시 타 알람의 알람 유지시간 초기화
			if(Is_SFP_Inserted(CPRI_0)) 
			{				
				if(IS__Clock_CPRI_Alarm())
					Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Lower__);
			}
		#endif
		#ifdef __2019_1226__Use_Alarm_Timer_Structure__
			if(Is_SFP_Inserted(CPRI_0)) 
			{
				// TSSI 하한 알람/정상 : 3s
				if(Chk__alarm_normal_timeout(&m_St.AlmTime.Path[path].DL.TSSI_Lower__, m_St.AlmSt.Path[path].DL.TSSI_Lower__, alarm ,3000, 3000))
					m_St.AlmSt.Path[path].DL.TSSI_Lower__ = 1; // update
				else
					m_St.AlmSt.Path[path].DL.TSSI_Lower__ = 0; // update
			}
			else
			{
				Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Lower__);
				m_St.AlmSt.Path[path].DL.TSSI_Lower__ = 0; // update
			}
		#else	
			if(Is_SFP_Inserted(CPRI_0)) 
				m_St.AlmSt.Path[path].DL.TSSI_Lower__ = alarm; // update
			else
				m_St.AlmSt.Path[path].DL.TSSI_Lower__ = 0;
		#endif		
		}			
	}	

	
	for(path = DL_0; path < MAX_DL; path++)
	{
		// Check Alarm Test ----------------------------------------
		if(m_St.AlmTest) // 2019.05.01
		{
		/*
			if(m_St.AlmFlag.Path[path].DL.TSSI_Upper_a)
				m_St.AlmSt.Path[path].DL.TSSI_Upper_a = m_St.AlmCtrl.Path[path].DL.TSSI_Upper_a;	
				
			if(m_St.AlmFlag.Path[path].DL.TSSI_Lower_a)
				m_St.AlmSt.Path[path].DL.TSSI_Lower_a = m_St.AlmCtrl.Path[path].DL.TSSI_Lower_a;
				
			if(m_St.AlmFlag.Path[path].UL.RSSI_Lower_a)
				m_St.AlmSt.Path[path].UL.RSSI_Lower_a = m_St.AlmCtrl.Path[path].UL.RSSI_Lower_a;
		*/
			continue;
		}
		// Check Alarm Test ----------------------------------------	
		
		if(Check_PAU_Use(path)==0)
		{
			m_St.AlmSt.Path[path].DL.TSSI_Upper_a = 0;
			m_St.AlmSt.Path[path].DL.TSSI_Lower_a = 0;
			m_St.AlmSt.Path[path].UL.RSSI_Lower_a = 0;

		#ifdef __2019_1226__Use_Alarm_Timer_Structure__
			Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper_a);
			Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Lower_a);
		#endif
			
			continue;
		}

		//-----------------------------------------
		// 2020.01.10 : added
		if(IS__TSSI_Not_Checking(path))//// || m_Env.RU_Init_St_Reset) // 2020.01.28 : RU Init 시, TSSI Upper Alarm Clear
		{
			Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper_a);
		}
		//-----------------------------------------
		//-----------------------
		// DL TSSI, Alpha
		//-----------------------
		power_x10 = m_St.d_DL_TSSI_dBm_x10[Alpha][path];		

		#ifdef __TYPE__PRU__
		if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27
		#endif		
		{
		// DL TSSI upper limit, Alpha ------------------------------------		
			limit_x10 = m_Eep.DL_TSSI_UP_x10[Alpha][path];
			alarm = m_St.AlmSt.Path[path].DL.TSSI_Upper_a;
			Hysterisis_Alm_Chk(&alarm, power_x10, limit_x10, 21); // high limit
		#ifdef __TYPE__PRU__
			alarm = m_St.d_TSSI_SD_St[Alpha][path];
		#endif	
		
		#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__	
			// Clock, Cpri알람 발생 시 타 알람의 알람 유지시간 초기화			
			if(IS__Clock_CPRI_Alarm())
			{
				Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper_a);
			}
		#endif		
		
			if(IS__Vaild_CPRI_Alpha())
			{
			#ifdef __2019_1226__Use_Alarm_Timer_Structure__	
				// TSSI 상한 알람/정상 : 3s
				if(Chk__alarm_normal_timeout(&m_St.AlmTime.Path[path].DL.TSSI_Upper_a, m_St.AlmSt.Path[path].DL.TSSI_Upper_a, alarm ,3000, 3000))
					m_St.AlmSt.Path[path].DL.TSSI_Upper_a = 1; // update
				else
					m_St.AlmSt.Path[path].DL.TSSI_Upper_a = 0; // update
			#else
				m_St.AlmSt.Path[path].DL.TSSI_Upper_a = alarm; // update
			#endif	
			}			
			else // invalid alpha
			{
				Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Upper_a);
				m_St.AlmSt.Path[path].DL.TSSI_Upper_a = 0;
			}

		// DL TSSI lower limit, Alpha ------------------------------------					
			if(IS__Vaild_CPRI_Alpha())
			{
				limit_x10 = m_Eep.DL_TSSI_DN_x10[Alpha][path];
				alarm = m_St.AlmSt.Path[path].DL.TSSI_Lower_a;
				Hysterisis_Alm_Chk(&alarm, limit_x10, power_x10, 20); // low limit

			#ifdef __2019_1223__Clr_Some_Alm_Timer_When_Clk_or_Cpri_Alarm__	
				// Clock, Cpri알람 발생 시 타 알람의 알람 유지시간 초기화
				if(IS__Clock_CPRI_Alarm())
					Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Lower_a);
			#endif
				
			#ifdef __2019_1226__Use_Alarm_Timer_Structure__
				// TSSI 하한 알람/정상 : 3s
				if(Chk__alarm_normal_timeout(&m_St.AlmTime.Path[path].DL.TSSI_Lower_a, m_St.AlmSt.Path[path].DL.TSSI_Lower_a, alarm ,3000, 3000))
					m_St.AlmSt.Path[path].DL.TSSI_Lower_a = 1; // update
				else
					m_St.AlmSt.Path[path].DL.TSSI_Lower_a = 0; // update
			#else
				m_St.AlmSt.Path[path].DL.TSSI_Lower_a = alarm; // update
			#endif
			}
			else // invalid alpha
			{
				m_St.AlmSt.Path[path].DL.TSSI_Lower_a = 0;
				Init__Alarm_Timer(&m_St.AlmTime.Path[path].DL.TSSI_Lower_a);
			}
		}	
	}
	
	//----------------------------------------
	// 2019.11.27
	for(path = DL_0; path < MAX_DL; path++) 
	{
		//--------------------------- Upper, 100M
		if(m_St.AlmSt_prev.Path[path].DL.TSSI_Upper__ != m_St.AlmSt.Path[path].DL.TSSI_Upper__)
		{
			m_St.AlmSt_prev.Path[path].DL.TSSI_Upper__ = m_St.AlmSt.Path[path].DL.TSSI_Upper__;
			Update_Alm_Str_Lmt( 0x51, 0x01, path, _100M, m_St.AlmSt.Path[path].DL.TSSI_Upper__); // DL TSSI 상한
		}
		//--------------------------- Upper, Alpha
		if(m_St.AlmSt_prev.Path[path].DL.TSSI_Upper_a != m_St.AlmSt.Path[path].DL.TSSI_Upper_a)
		{
			m_St.AlmSt_prev.Path[path].DL.TSSI_Upper_a = m_St.AlmSt.Path[path].DL.TSSI_Upper_a;
			Update_Alm_Str_Lmt( 0x51, 0x01, path, Alpha, m_St.AlmSt.Path[path].DL.TSSI_Upper_a); // DL TSSI 상한, A
		}		
		//--------------------------- Lower, 100M
		if(m_St.AlmSt_prev.Path[path].DL.TSSI_Lower__ != m_St.AlmSt.Path[path].DL.TSSI_Lower__)
		{
			m_St.AlmSt_prev.Path[path].DL.TSSI_Lower__ = m_St.AlmSt.Path[path].DL.TSSI_Lower__;
			Update_Alm_Str_Lmt( 0x51, 0x02, path, _100M, m_St.AlmSt.Path[path].DL.TSSI_Lower__); // DL TSSI 하한
		}
		//--------------------------- Lower, Alpha
		if(m_St.AlmSt_prev.Path[path].DL.TSSI_Lower_a != m_St.AlmSt.Path[path].DL.TSSI_Lower_a)
		{
			m_St.AlmSt_prev.Path[path].DL.TSSI_Lower_a = m_St.AlmSt.Path[path].DL.TSSI_Lower_a;
			Update_Alm_Str_Lmt( 0x51, 0x02, path, Alpha, m_St.AlmSt.Path[path].DL.TSSI_Lower_a); // DL TSSI 하한, A
		}
	}
	//----------------------------------------
	// 2020.07.01
		if(m_St.AlmSt_prev.A_3.PD_Pwr_U_0__ != m_St.AlmSt.A_3.PD_Pwr_U_0__) 
		{
			m_St.AlmSt_prev.A_3.PD_Pwr_U_0__ = m_St.AlmSt.A_3.PD_Pwr_U_0__;
			Update_Alm_Str_Lmt( 0x53, 0x0C, 0, _100M, m_St.AlmSt.A_3.PD_Pwr_U_0__);
		}		
		if(m_St.AlmSt_prev.A_3.PD_Pwr_U_0_a != m_St.AlmSt.A_3.PD_Pwr_U_0_a) 
		{
			m_St.AlmSt_prev.A_3.PD_Pwr_U_0_a = m_St.AlmSt.A_3.PD_Pwr_U_0_a;
			Update_Alm_Str_Lmt( 0x53, 0x0C, 0, Alpha, m_St.AlmSt.A_3.PD_Pwr_U_0_a);
		}		
		if(m_St.AlmSt_prev.A_4.PD_Pwr_U_1__ != m_St.AlmSt.A_4.PD_Pwr_U_1__) 
		{
			m_St.AlmSt_prev.A_4.PD_Pwr_U_1__ = m_St.AlmSt.A_4.PD_Pwr_U_1__;
			Update_Alm_Str_Lmt( 0x53, 0x0C, 1, _100M, m_St.AlmSt.A_4.PD_Pwr_U_1__);
		}		
		if(m_St.AlmSt_prev.A_4.PD_Pwr_U_1_a != m_St.AlmSt.A_4.PD_Pwr_U_1_a) 
		{
			m_St.AlmSt_prev.A_4.PD_Pwr_U_1_a = m_St.AlmSt.A_4.PD_Pwr_U_1_a;
			Update_Alm_Str_Lmt( 0x53, 0x0C, 1, Alpha, m_St.AlmSt.A_4.PD_Pwr_U_1_a);
		}	

		// 2020.07.01
		if(m_St.AlmSt_prev.A_4.PD_Pwr_L_0__ != m_St.AlmSt.A_4.PD_Pwr_L_0__) 
		{
			m_St.AlmSt_prev.A_4.PD_Pwr_L_0__ = m_St.AlmSt.A_4.PD_Pwr_L_0__;
			Update_Alm_Str_Lmt( 0x53, 0x09, 0, _100M, m_St.AlmSt.A_4.PD_Pwr_L_0__);
		}		
		if(m_St.AlmSt_prev.A_4.PD_Pwr_L_0_a != m_St.AlmSt.A_4.PD_Pwr_L_0_a) 
		{
			m_St.AlmSt_prev.A_4.PD_Pwr_L_0_a = m_St.AlmSt.A_4.PD_Pwr_L_0_a;
			Update_Alm_Str_Lmt( 0x53, 0x09, 0, Alpha, m_St.AlmSt.A_4.PD_Pwr_L_0_a);
		}
		if(m_St.AlmSt_prev.A_4.PD_Pwr_L_1__ != m_St.AlmSt.A_4.PD_Pwr_L_1__) 
		{
			m_St.AlmSt_prev.A_4.PD_Pwr_L_1__ = m_St.AlmSt.A_4.PD_Pwr_L_1__;
			Update_Alm_Str_Lmt( 0x53, 0x09, 1, _100M, m_St.AlmSt.A_4.PD_Pwr_L_1__);
		}
		if(m_St.AlmSt_prev.A_4.PD_Pwr_L_1_a != m_St.AlmSt.A_4.PD_Pwr_L_1_a) 
		{
			m_St.AlmSt_prev.A_4.PD_Pwr_L_1_a = m_St.AlmSt.A_4.PD_Pwr_L_1_a;
			Update_Alm_Str_Lmt( 0x53, 0x09, 1, Alpha, m_St.AlmSt.A_4.PD_Pwr_L_1_a);
		}
}

/*
-------------------------------------------------------
VSS : 
	DL : CPRI 0,1 : Read Only
	DL : CPRI 2,3 : Read/Write

	UL : CPRI 0,1 : Read/Write
	UL : CPRI 2,3 : Read Only

-------------------------------------------------------
*/

void Proc_LS_Vss_Aive_Ack(void)
{
	static INT8U cnt = 0;
	static INT8U idx = 0;
	INT8U alive_ack[2] = {0xCC, 0x33};
	INT8U i;	

	//-------------------------------------
	if(Get_Instant_CPRI_Lock_St()==0) // 2019.05.14, Checked
		return;
	//-------------------------------------
	
	/*
	-----------------------------------------------
		Alive ACK for CPRI0/1 - UL
		every 500ms, 0xCC, 0x33
	-----------------------------------------------
	*/
	cnt++;
	if(cnt > 2) 
	{
		idx = ~idx;
		cnt = 0;	

		if(idx) i = 1;
		else i = 0;
		
		m_Reg_Flg.UL_CPRI[CPRI_0].ALIVE_ACK_00.uD32 = 1;
		m_Reg_Ctl.UL_CPRI[CPRI_0].ALIVE_ACK_00.uD32 = alive_ack[i];

		m_Reg_Flg.UL_CPRI[CPRI_1].ALIVE_ACK_00.uD32 = 1;
		m_Reg_Ctl.UL_CPRI[CPRI_1].ALIVE_ACK_00.uD32 = alive_ack[i];	
	}
}


void Proc_LS_VSS_Copy_CPRI(void)
{
	INT32U req32, cur32;
	INT8U c, i, l;
	//static INT8U cnt = 0;

	//------------------------------------
	// 2019.05.14
	if(Get_Instant_CPRI_Lock_St() == 0) // Checked 
		return;	
	//------------------------------------




	// DL copy
	for(c = CPRI_0; c <= CPRI_1; c++) // CPRI 0,1 only
	{
		if(c == CPRI_0) l = CPRI_2;
		else            l = CPRI_3; 

		if(IS__CPRI_Lock(c)==0) continue; // 2019.09.09

		/*
		------------------------------------------------------
		Do not Copy
		--------------		
			RECSERDESLOS
			RECLOFALARM  		
			RE_Cascade_ID		 
			OpticLOSDetectAlarm
			Loopbackmode		
						   
			Delay_H      (not used field)
			Delay_L 	 (not used field)
			RE1TxDelay_H (not used field)
			RE1TxDelay_L (not used field)			 
			RE1RxDelay_H (not used field)
			RE1RxDelay_L (not used field)	
										  
			BIP_Count   (ready only from cpri0/1)	
			BIP_Count_1 (ready only from cpri0/1)	
			BIP_Count_2 (ready only from cpri0/1)	
			BIP_Count_3 (ready only from cpri0/1)	
			BIP_Clear   (ready only from cpri0/1)	
			BIP_Err_Cnt (ready only from cpri0/1)	
		------------------------------------------------------
		*/

		//------------------------------------------------------
	#ifdef __TYPE__PRU__
	// below is done in the FPGA @ PRU
	#else // 2020.02.19 : DUH_ID @ AAU 		
		req32 = m_Reg_Sts.DL_CPRI[c].DUH_ID.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].DUH_ID.uD32;
		if(req32 != cur32)
		{			
			m_Reg_Flg.DL_CPRI[l].DUH_ID.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].DUH_ID.uD32 = req32;

			//D_Printf(DEBUG_SYSTEM, "[CPRI Copy] DUH_ID : c %d, req 0x%X, cur 0x%X\n", 
			//	c, req32, cur32);
		}
	#endif
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].RECTypeID.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].RECTypeID.uD32;
		if(req32 != cur32)
		{			
			m_Reg_Flg.DL_CPRI[l].RECTypeID.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].RECTypeID.uD32 = req32;

			//D_Printf(DEBUG_SYSTEM, "[CPRI Copy] RECTypeID : c %d, req 0x%X, cur 0x%X\n", 
			//	c, req32, cur32);
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].CnM_Path_selection.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].CnM_Path_selection.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].CnM_Path_selection.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].RE_Reset.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].RE_Reset.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].RE_Reset.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].RE_Reset.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].RE_Channel_BW.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].RE_Channel_BW.uD32;
		if(req32 != cur32)
		{			
			m_Reg_Flg.DL_CPRI[l].RE_Channel_BW.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].RE_Channel_BW.uD32 = req32;

			//D_Printf(DEBUG_SYSTEM, "[CPRI Copy] RE_Channel_BW : c %d, req 0x%X, cur 0x%X\n", 
			//	c, req32, cur32);
		}
		//else
		//{
		//	D_Printf(DEBUG_SYSTEM, "[CPRI Copy] RE_Channel_BW : c %d, req 0x%X = cur 0x%X\n", 
		//		c, req32, cur32);
		//}		
		//------------------------------------------------------
		for(i = 0; i < 4; i++)
		{
			//-----------------------------
			req32 = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[i].uD32;
			cur32 = m_Reg_Sts.DL_CPRI[l].Noti_IP1_Address[i].uD32;
			if(req32 != cur32)
			{
				m_Reg_Flg.DL_CPRI[l].Noti_IP1_Address[i].uD32 = _F_SET_;
				m_Reg_Ctl.DL_CPRI[l].Noti_IP1_Address[i].uD32 = req32;
			}
			//------------------------------
			req32 = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[i].uD32;
			cur32 = m_Reg_Sts.DL_CPRI[l].Noti_IP2_Address[i].uD32;
			if(req32 != cur32)
			{
				m_Reg_Flg.DL_CPRI[l].Noti_IP2_Address[i].uD32 = _F_SET_;
				m_Reg_Ctl.DL_CPRI[l].Noti_IP2_Address[i].uD32 = req32;
			}
		}
		//------------------------------------------------------
		for(i = 0; i < 6; i++)
		{			
			req32 = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[i].uD32;
			cur32 = m_Reg_Sts.DL_CPRI[l].Base_Mac_Address[i].uD32;
			if(req32 != cur32)
			{
				m_Reg_Flg.DL_CPRI[l].Base_Mac_Address[i].uD32 = _F_SET_;
				m_Reg_Ctl.DL_CPRI[l].Base_Mac_Address[i].uD32 = req32;
			}
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].OpenCPRIVersion.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].OpenCPRIVersion.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].OpenCPRIVersion.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].CPRICompressionOnOff.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].CPRICompressionOnOff.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].CPRICompressionOnOff.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].CPRICompressionOnOff.uD32 = req32;
		}		
		//------------------------------------------------------
		for(i = 0; i < 4; i++)
		{
			//-----------------------------
			req32 = m_Reg_Sts.DL_CPRI[c].REx_Reset[i].uD32;
			cur32 = m_Reg_Sts.DL_CPRI[l].REx_Reset[i].uD32;
			if(req32 != cur32)
			{
				m_Reg_Flg.DL_CPRI[l].REx_Reset[i].uD32 = _F_SET_;
				m_Reg_Ctl.DL_CPRI[l].REx_Reset[i].uD32 = req32;
			}
			//-----------------------------
			req32 = m_Reg_Sts.DL_CPRI[c].Base_IP_Address[i].uD32;
			cur32 = m_Reg_Sts.DL_CPRI[l].Base_IP_Address[i].uD32;
			if(req32 != cur32)
			{
				m_Reg_Flg.DL_CPRI[l].Base_IP_Address[i].uD32 = _F_SET_;
				m_Reg_Ctl.DL_CPRI[l].Base_IP_Address[i].uD32 = req32;

				//D_Printf(DEBUG_SYSTEM, "[CPRI Copy] Base_IP_Address(%d) : c %d, req 0x%X, cur 0x%X\n", 
				//	i, c, req32, cur32);
			}
		}		
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].Cell_ID.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].Cell_ID.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].Cell_ID.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].Cell_ID.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].gNB_ID_1st.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].gNB_ID_1st.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].gNB_ID_1st.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].gNB_ID_1st.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].gNB_ID_2nd.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].gNB_ID_2nd.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].gNB_ID_2nd.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].gNB_ID_2nd.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].gNB_ID_3rd.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].gNB_ID_3rd.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].gNB_ID_3rd.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].gNB_ID_3rd.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].gNB_ID_4th.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].gNB_ID_4th.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].gNB_ID_4th.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].gNB_ID_4th.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].PCI_1st_Byte.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].PCI_1st_Byte.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].PCI_1st_Byte.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].PCI_1st_Byte.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].PCI_2nd_Byte.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].PCI_2nd_Byte.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].PCI_2nd_Byte.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].PCI_2nd_Byte.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].REC_Port_No.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].REC_Port_No.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].REC_Port_No.uD32 = req32;
		}
		//------------------------------------------------------
		for(i = 0; i < 20; i++)
		{
			//-----------------------------
			req32 = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[i].uD32;
			cur32 = m_Reg_Sts.DL_CPRI[l].TDD_Slot_Number[i].uD32;
			if(req32 != cur32)
			{
				m_Reg_Flg.DL_CPRI[l].TDD_Slot_Number[i].uD32 = _F_SET_;
				m_Reg_Ctl.DL_CPRI[l].TDD_Slot_Number[i].uD32 = req32;
			}
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].CPRI_Version.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].CPRI_Version.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].CPRI_Version.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].CPRI_Version.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].CPRI_Startup.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].CPRI_Startup.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].CPRI_Startup.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].CPRI_Startup.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].CPRI_L1_Reset_LOS.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].CPRI_L1_Reset_LOS.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].CPRI_L1_Reset_LOS.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].CPRI_L1_Reset_LOS.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].Pointer_P.uD32;
		cur32 = m_Reg_Sts.DL_CPRI[l].Pointer_P.uD32;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].Pointer_P.uD32 = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].Pointer_P.uD32 = req32;
		}
		//------------------------------------------------------
		req32 = m_Reg_Sts.DL_CPRI[c].L1Reset.Data;
		cur32 = m_Reg_Sts.DL_CPRI[l].L1Reset.Data;
		if(req32 != cur32)
		{
			m_Reg_Flg.DL_CPRI[l].L1Reset.Data = _F_SET_;
			m_Reg_Ctl.DL_CPRI[l].L1Reset.Data = req32;
		}
		//------------------------------------------------------		
	}
	//------------------------------------------------------
	// UL copy
	//------------------------------------------------------
	for(c = CPRI_2; c <= CPRI_3; c++) // CPRI 2,3 only
	{
		if(c == CPRI_2) l = CPRI_0;
		else            l = CPRI_1; 

		if(IS__CPRI_Lock(c)==0) continue; // 2019.09.09
		
		/*
		------------------------------------------------------
		Do not Copy
			--------------			
			RE_SERDESLOS;
			RE_LOFALARM; 
			REFunctionFail;			
			RE_Reset_Ack;
			ALIVE_ACK_00; 
			ALIVE_ACK_01; 
			Mismatch_Noti;
			RE_Cascade_ID_ACK;
			RE_OpticLOSDetectAlarm;
			RE_Loopbackmode_ACK;    
			-------------------------------			   
			RE_Type_ID;
			RE_Vendor_ID;
			-------------------------------			   
			CPRI_Version;
			CPRI_Startup;
			CPRI_L1_Reset_LOS;
			Pointer_P;	  
			L1_Reset_ACK; 							
			-------------------------------
			BIP_Count   (ready only from cpri0/1)	
			BIP_Count_1 (ready only from cpri0/1)	
			BIP_Count_2 (ready only from cpri0/1)	
			BIP_Count_3 (ready only from cpri0/1)	
			BIP_Clear   (ready only from cpri0/1)	
			BIP_Err_Cnt (ready only from cpri0/1)	
		------------------------------------------------------
		*/	
		//------------------------------------------------------
		INT8U id = AufoConfigFlag.RE_Cascade_ID;					// 2019.05.08
	
		for(i = 0; i < 4; i++)
		{
			//-----------------------------
			if(i > id)
			{
				#ifdef __USE_VSS_UL_RE_RESET_ACK_SW__
				// 2019.05.17
				#else
				req32 = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[i].uD32;
				cur32 = m_Reg_Sts.UL_CPRI[l].RE_Reset_ACK[i].uD32;
				if(req32 != cur32)
				{
					m_Reg_Flg.UL_CPRI[l].RE_Reset_ACK[i].uD32 = _F_SET_;
					m_Reg_Ctl.UL_CPRI[l].RE_Reset_ACK[i].uD32 = req32;
				}				
				#endif
			}
		}
	}
	
	// 2019.07.09
	//---------------------------------------
	// UL
	//---------------------------------------
	c = CPRI_0;
	req32 = (m_St.d_CPRI_Alarm[c] & cbits[0])? 0xCC:0x33; // bit0(LOS)
	Write_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RECSERDESLOS], req32, "VSS_UL_RECSERDESLOS");
	
	req32 = (m_St.d_CPRI_Alarm[c] & cbits[1])? 0xCC:0x33; // bit1(LOF)
	Write_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RECLOFALARM] , req32, "VSS_UL_RECLOFALARM");	
	//---------------------------------------
	c = CPRI_1;
	req32 = (m_St.d_CPRI_Alarm[c] & cbits[0])? 0xCC:0x33; // bit0(LOS)
	Write_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RECSERDESLOS], req32, "VSS_UL_RECSERDESLOS");

	req32 = (m_St.d_CPRI_Alarm[c] & cbits[1])? 0xCC:0x33; // bit1(LOF)
	Write_FPGA_Reg( Reg_VSS_UL_Addr[c][Reg_VSS_UL_RECLOFALARM] , req32, "VSS_UL_RECLOFALARM");	
	//---------------------------------------
	// DL
	//---------------------------------------
	c = CPRI_2;
	req32 = (m_St.d_CPRI_Alarm[c] & cbits[0])? 0xCC:0x33; // bit0(LOS)
	Write_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECSERDESLOS], req32, "VSS_DL_RECSERDESLOS");

	req32 = (m_St.d_CPRI_Alarm[c] & cbits[1])? 0xCC:0x33; // bit1(LOF)
	Write_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECLOFALARM] , req32, "VSS_DL_RECLOFALARM");	
	//---------------------------------------
	c = CPRI_3;
	req32 = (m_St.d_CPRI_Alarm[c] & cbits[0])? 0xCC:0x33; // bit0(LOS)
	Write_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECSERDESLOS], req32, "VSS_DL_RECSERDESLOS");

	req32 = (m_St.d_CPRI_Alarm[c] & cbits[1])? 0xCC:0x33; // bit1(LOF)
	Write_FPGA_Reg( Reg_VSS_DL_Addr[c][Reg_VSS_DL_RECLOFALARM] , req32, "VSS_DL_RECLOFALARM");
	//------------------------------------------------------

	//------------------------------------------------------	
	req32 = (m_St.AlmSt.A_2.LOS_SFP_0__)? 0xCC:0x33; 
	Write_FPGA_Reg( Reg_VSS_UL_Addr[CPRI_0][Reg_VSS_UL_RE_OpticLOSDetectAlarm], req32, "VSS_UL_RE_OpticLOSDetectAlarm");
	
	req32 = (m_St.AlmSt.A_2.LOS_SFP_0_a)? 0xCC:0x33;
	Write_FPGA_Reg( Reg_VSS_UL_Addr[CPRI_1][Reg_VSS_UL_RE_OpticLOSDetectAlarm], req32, "VSS_UL_RE_OpticLOSDetectAlarm");
	
	req32 = (m_St.AlmSt.A_2.LOS_SFP_1__)? 0xCC:0x33;
	Write_FPGA_Reg( Reg_VSS_DL_Addr[CPRI_2][Reg_VSS_DL_OpticLOSDetectAlarm], req32, "VSS_DL_OpticLOSDetectAlarm");
	
	req32 =(m_St.AlmSt.A_2.LOS_SFP_1_a)?  0xCC:0x33;
	Write_FPGA_Reg( Reg_VSS_DL_Addr[CPRI_3][Reg_VSS_DL_OpticLOSDetectAlarm], req32, "VSS_DL_OpticLOSDetectAlarm");	
	//------------------------------------------------------
	
	#ifdef __USE_VSS_UL_RE_RESET_ACK_SW__
	Proc___VSS_UL_RE_Reset_ACK_Switch(); // 2019.05.17
	#endif
}



INT8U IS_BW_Valid(INT8U bw__all, INT8U bw_100, INT8U bw___A)
{
	INT8U valid;
	
	     if(bw__all == 0x00) valid = 0;
	else if(bw__all == 0x11) valid = 0;
	else if(bw__all == 0x22) valid = 0;
	else if(bw___A > 6) valid = 0;
	else if((bw_100 != 6)&&(bw_100 != 0)) valid = 0;
	else valid = 1;	

	return valid;
}



static OS_TIMER Loopback_Test_timer_Vss;
static OS_TIMER Loopback_timer_Vss;


void Proc_LS_VSS_Channel(void)
{
	int c, path4, i, k, c_id;
	static int cnt_bw=0;
	
	INT8U  data[10]; // temporary	
	INT32U uVal32;//, addr32;

	INT8U flag;
	INT8U flag1, flag2;
	INT8U idx1, idx2;

	INT8U  uVal8;
	INT8U  uVal8_req;
	INT32U uVal32_req;
	static INT8U cnt = 0;
	static INT32U cnt32 = 0;
	static INT16U cnt16 = 0;

	INT8U id = AufoConfigFlag.RE_Cascade_ID;				// 2019.05.08
	
	/*
	-----------------------------------------------------
	2019.05.14.
	   CPRI의 DL/UL BW Register Control
	   * when (SFP Mount) && (CPRI 알람) : 해당 CPRI BW Register를 0x01(10M)로 제어,
	   * when Lock 유지 :  VSS Table에 있는 값으로 다시 제어
	   * 100MHz, aMHz 각각 수행
	-----------------------------------------------------
	*/
	// vv	
	//------------------------------------
	Use_d_RE_Channel_BandWidth(m_Eep.d_RE_Channel_BW[_100M], m_Eep.d_RE_Channel_BW[Alpha], 'E'); // call before cpri-lock

	if(Get_Instant_CPRI_Lock_St() == 0) // Checked  
	{	
		return;	
	}
	//------------------------------------


	/*		
	--------------------------------------------------------------
	2019.07.17 : 
		Loopback On/Off는 제어는 안되고 상태만 표시하며, 
		VSS 필드의 Loopback 필드값이 0xCC, 0xCA인 경우 On, 
		이외 다른값은 Off로 처리
	--------------------------------------------------------------	
	*/
	INT8U vss_loopback;
	
	vss_loopback = (INT8U)m_Reg_Sts.DL_CPRI[m_St.Ref_CPRI_No].Loopbackmode.uD32;
	
	     if(vss_loopback == 0xCC) m_St.d_Loopback_OnOff = ON;
	else if(vss_loopback == 0xCA) m_St.d_Loopback_OnOff = ON;
	else                          m_St.d_Loopback_OnOff = OFF;
	//--------------------------------------------------------------
		

	

	
	
	/*
	-----------------------------------------------
	1.	Read DL-CPRI-Version and write @ CPRI-UL-"Mismatch-Noti"
		if CPRI Version is 0x01 : 
			write 0x00 @ CPRI-UL-"Mismatch-Noti", <CPRI 0,1> only
		else
			write 0x33 @ CPRI-UL-"Mismatch-Noti", <CPRI 0,1> only
	-----------------------------------------------
	*/


	cnt++;
	for(c = CPRI_0; c <= CPRI_1; c++) // CPRI 0,1 only
	{
		uVal8 = (INT8U)m_Reg_Sts.UL_CPRI[c].Mismatch_Noti.uD32;
		flag = 0; // init 0

		//D_Printf(DEBUG_SYSTEM, "[RCU] UL %d, Mismatch_Noti 0x%X\n", c, uVal8);
		
		/*
		if(cnt >= 10)
		{			
			D_Printf(DEBUG_SYSTEM, "[RCU] LS_Test c %d, ver 0x%X, 0x%X / MisNoti 0x%X, 0x%X, 0x%X\n", 
				c, 
				m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD32,
				(INT8U)m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD32,				
				
				m_Reg_Sts.UL_CPRI[c].Mismatch_Noti.uD32,
				(INT8U)m_Reg_Sts.UL_CPRI[c].Mismatch_Noti.uD32,
				uVal8);
		}
		*/

		if(m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD8[0] == 0x01) // ver 0x01
		{			
			if(uVal8 != 0x00) { flag = 1; uVal8_req = 0x00; }
		}
		else
		{
			if(uVal8 != 0x33) { flag = 1; uVal8_req = 0x33; }
		}
		if(flag)
		{
			m_Reg_Flg.UL_CPRI[c].Mismatch_Noti.uD32 = 1;
			m_Reg_Ctl.UL_CPRI[c].Mismatch_Noti.uD32 = uVal8_req;
			/*
			if(cnt >= 10)
			{			
				D_Printf(DEBUG_SYSTEM, "[RCU] flag c %d, req 0x%X\n", c, uVal8_req);
				D_Printf(DEBUG_SYSTEM, "[RCU] flag c %d, OpenCPRIVer 0x%X\n", c, (INT8U)m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD8[0]);
			}
			*/
		}
	}
	
	
	/*
	-----------------------------------------------
	2.	CPRI Compress
	CPRICompressionOn/Off
	if 0xCC : 
			write 0x01 @ CPRI-UL-"Register Map Comp on/off"
	else if 0x33 : 
			write 0x00 @ CPRI-UL-"Register Map Comp on/off"
	-----------------------------------------------
	*/	
	for(c = CPRI_0; c <= CPRI_1; c++) // CPRI 0,1 only
	{
		idx1 = c;		
		if(c == CPRI_0) idx2 = CPRI_2;
		else            idx2 = CPRI_3; 
		
		data[0] = (INT8U)m_Reg_Sts.COMP_ONOFF[idx1].Ctrl.Data;
		data[1] = (INT8U)m_Reg_Sts.COMP_ONOFF[idx2].Ctrl.Data;
			
		flag1 = 0; // init 0
		flag2 = 0; // init 0

		
			

		if((INT8U)m_Reg_Sts.DL_CPRI[c].CPRICompressionOnOff.uD32 == 0xCC)
		{
			if(data[0] != 0x01) { flag1 = 1; uVal8_req = 1; }
			if(data[1] != 0x01) { flag2 = 1; uVal8_req = 1; }
		}
		else
		{
			if(data[0] != 0x00) { flag1 = 1; uVal8_req = 0; }
			if(data[1] != 0x00) { flag2 = 1; uVal8_req = 0; }
		}
		if(flag1)
		{
			m_Reg_Flg.COMP_ONOFF[idx1].Ctrl.OnOff = 1;
			m_Reg_Ctl.COMP_ONOFF[idx1].Ctrl.OnOff = uVal8_req;
		}
		if(flag2)
		{
			m_Reg_Flg.COMP_ONOFF[idx2].Ctrl.OnOff = 1;
			m_Reg_Ctl.COMP_ONOFF[idx2].Ctrl.OnOff = uVal8_req;
		}
	}
	//--------------------------------------------
	

	/*
	-----------------------------------------------
	3.	VSD cascade ID
		VSS채널에서 Cascade ID를 Read. ID 부여만 확인
	-----------------------------------------------	
	*/	
	
	if(m_St.Ref_CPRI_No == CPRI_0)
		c_id = m_St.d_Cascade_ID[CPRI_0];
	else
		c_id = m_St.d_Cascade_ID[CPRI_1];
	        
	uVal8 = c_id + 1; // next
	
	// Set Next Cascade-ID	
	if(uVal8 != m_Reg_Sts.DL_CPRI[CPRI_2].RE_Cascade_ID.uD32)
	{
		m_Reg_Flg.DL_CPRI[CPRI_2].RE_Cascade_ID.uD32 = 1;
		m_Reg_Ctl.DL_CPRI[CPRI_2].RE_Cascade_ID.uD32 = uVal8;

		//D_Printf(DEBUG_SYSTEM, "[RCU] CPRI_1, Cascade ID 0x%X Set.\n", uVal8 );
	}
	if(uVal8 != m_Reg_Sts.DL_CPRI[CPRI_3].RE_Cascade_ID.uD32)
	{
		m_Reg_Flg.DL_CPRI[CPRI_3].RE_Cascade_ID.uD32 = 1;
		m_Reg_Ctl.DL_CPRI[CPRI_3].RE_Cascade_ID.uD32 = uVal8;

		//D_Printf(DEBUG_SYSTEM, "[RCU] CPRI_3, Cascade ID 0x%X Set.\n", uVal8 );
	}

	// Cascade-ID ACK to UL-CPRI
	if(m_Reg_Sts.UL_CPRI[CPRI_0].RE_Cascade_ID_ACK.uD32 != c_id)
	{
		m_Reg_Flg.UL_CPRI[CPRI_0].RE_Cascade_ID_ACK.uD32 = _F_SET_;
		m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Cascade_ID_ACK.uD32 = c_id;
	}	
	if(m_Reg_Sts.UL_CPRI[CPRI_1].RE_Cascade_ID_ACK.uD32 != c_id)
	{
		m_Reg_Flg.UL_CPRI[CPRI_1].RE_Cascade_ID_ACK.uD32 = _F_SET_;
		m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Cascade_ID_ACK.uD32 = c_id;
	}


	/*
	-----------------------------------------------
	DTU UL Gain Backoff
	2019.05.04
	-----------------------------------------------
	*/
	INT16U UL_GainBal_x10;
	INT8U ru_cnt;
	INT8U Vss_Z_30;
	static INT16U prev_gain_backoff_x10 = 0;
	ru_cnt = c_id+1; // include self-RU

	if(c_id < 3)
	{
		if(m_St.Ref_CPRI_No == CPRI_0) 
			 c = CPRI_2;
		else c = CPRI_3;	
		
		for(k = (c_id+1); k < 4; k++) // from next RU
		{			
			// 2019.05.07 : use "RE_Reset_ACK"
			if(m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[k].uD32 == 0xCC) ru_cnt++;
			if(m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[k].uD32 == 0x33) ru_cnt++;	
		}		
	}
	
	if(AufoConfigFlag.RE_Cascade_ID == 0) // 2019.05.13, Cascade-id 0
	{
		m_St.UL_Gain_A_BackOff = m_Eep.UL_Gain_A_BackOff;		
		UL_GainBal_x10 = m_Eep.d_UL_GainBal_x10[0][0];	
		
		if(m_St.UL_Gain_A_BackOff == ON)
		{
			m_St.d_RU_UL__Gain_Backoff_x10 = 100*log10(ru_cnt);	// when ru_cnt = 2, 100*log10(2) = 3*10[dB]
			if(ru_cnt == 3) m_St.d_RU_UL__Gain_Backoff_x10 = 48;
			Vss_Z_30 = 0xC0; // Z.30.0:	0xCC: 기능 On
			Vss_Z_30 |= (UL_GainBal_x10/10)&0x0F;
		}
		else
		{
			m_St.d_RU_UL__Gain_Backoff_x10 = 0;
			Vss_Z_30 = 0x30; // Z.30.0: 0x33: 기능 Off
			Vss_Z_30 |= (UL_GainBal_x10/10)&0x0F;
		}
	}
	else //-------------------------------------------Cascade-id 1~3
	{
		// copy simply VSS 
		// VSS.UL_GainBackOff_Onoff has backoff-onoff and gain-balance-att)
		if(m_St.Ref_CPRI_No == CPRI_0)
			Vss_Z_30 = m_Reg_Sts.DL_CPRI[CPRI_0].UL_GainBackOff_Onoff.uD8[0];
		else
			Vss_Z_30 = m_Reg_Sts.DL_CPRI[CPRI_1].UL_GainBackOff_Onoff.uD8[0];

		UL_GainBal_x10 = (Vss_Z_30 & 0x0F)*10;
		
		if((Vss_Z_30 & 0xF0) == 0xC0) // On, upper 4bits
		{
			m_St.UL_Gain_A_BackOff = ON; // 2019.05.13
			m_St.d_RU_UL__Gain_Backoff_x10 = 100*log10(ru_cnt);	// when ru_cnt = 2, 100*log10(2) = 3*10[dB]
			if(ru_cnt == 3) m_St.d_RU_UL__Gain_Backoff_x10 = 48;
		}
		else
		{
			m_St.UL_Gain_A_BackOff = OFF; // 2019.05.13
			m_St.d_RU_UL__Gain_Backoff_x10 = 0;
		}
	}
	
	//------------------------------------------------------
	// 2019.05.14 : Cacade_ID에 관계없이 하위 CPRI2/3 control				
	if(m_Reg_Sts.DL_CPRI[CPRI_2].UL_GainBackOff_Onoff.uD8[0] != Vss_Z_30)
	{
		m_Reg_Flg.DL_CPRI[CPRI_2].UL_GainBackOff_Onoff.uD32 = _F_SET_;
		m_Reg_Ctl.DL_CPRI[CPRI_2].UL_GainBackOff_Onoff.uD32 = Vss_Z_30;
	}
	if(m_Reg_Sts.DL_CPRI[CPRI_3].UL_GainBackOff_Onoff.uD8[0] != Vss_Z_30)
	{
		m_Reg_Flg.DL_CPRI[CPRI_3].UL_GainBackOff_Onoff.uD32 = _F_SET_;
		m_Reg_Ctl.DL_CPRI[CPRI_3].UL_GainBackOff_Onoff.uD32 = Vss_Z_30;
	}


	if((prev_gain_backoff_x10 != m_St.d_RU_UL__Gain_Backoff_x10)||
	   (m_St.d_UL_GainBal_x10 != UL_GainBal_x10))
	{	
		m_St.d_UL_GainBal_x10 = UL_GainBal_x10;	
		
		D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] UL Backoff %d/10 -> %d/10, UL Gain Bal %d/10 -> %d/10\n", 
			prev_gain_backoff_x10, m_St.d_RU_UL__Gain_Backoff_x10,
			m_St.d_UL_GainBal_x10, UL_GainBal_x10);

		for(c = 0; c < MAX_FA_PATH; c++)
		{
			for( k = 0; k < MAX_DL; k++)
			{
				m_St.d_UL_DTU_Gain_Backoff_GUI_x10[c][k] = m_St.d_RU_UL__Gain_Backoff_x10;
				
			#ifdef __TYPE__PRU__ // ANTS-PRU
				m_St.d_UL_DTU_Gain_Backoff_x10[c][k] = m_St.d_RU_UL__Gain_Backoff_x10;				
			#else				
				if(m_St.d_RU_UL__Gain_Backoff_x10 == 48)
					m_St.d_UL_DTU_Gain_Backoff_x10[c][k] = 45;
				else
					m_St.d_UL_DTU_Gain_Backoff_x10[c][k] = m_St.d_RU_UL__Gain_Backoff_x10;
			#endif		
			}
		}
		
		
		for(c = 0; c < MAX_FA_PATH; c++)
		{
			for( k = 0; k < MAX_DL; k++)
			#ifdef __TYPE__PRU__ // ANTS-PRU
				Set_PRU_UL_Gain_Backoff_x10(c, k, m_St.d_UL_DTU_Gain_Backoff_x10[c][k]);
			#else
				Set_ALC_Gain_Comp_x10(c, k, m_St.AAU_ALC_Gain_Comp_x10[c][k]);
			#endif		
		}		
	}
	m_St.d_UL_GainBal_x10 = UL_GainBal_x10;	
	prev_gain_backoff_x10 = m_St.d_RU_UL__Gain_Backoff_x10;
	
	//--------------------------------------------------
		

	/*	
	-----------------------------------------------
	Loopback mode
		REC <-> RE 간 Loopback 기능을 통해 선로 이상을 확인한다
		
	VSS DL Loopback Mode | Register Map Loopback (0x0000B) | VSS UL ACK
	-------------------------------------------------------+------------
		0xCC(mode 1)     | 0x0000 0010                     | 0xCC
		0xCA(mode 2)     | 0x0000 0001                     | 0xCA
		0x33(off)        | 0x0000 0000                     | 0x33
    -------------------------------------------------------+------------
	DL 에서 받은 mode 값을 UL 쪽을 ACK 함.	
	----------------------------------------------- 
	*/	
	INT32U Loopbackmode;
	INT32U Loopback_ack;
	INT8U  Reg_ctrl, cpri;
	INT8U  loopback_disp = 0; // 2019.11.11. Loop Back 관련 주기적으로 출력되는 로그 Off
	
	if(m_St.Ref_CPRI_No == CPRI_0) cpri = CPRI_0;
	else cpri = CPRI_1;
			
	Loopbackmode = m_Reg_Sts.DL_CPRI[cpri].Loopbackmode.uD32;
	Loopback_ack = m_Reg_Sts.UL_CPRI[cpri].RE_Loopbackmode_ACK.uD32;

	if(cnt32 == 0)
		OSTimerGet_msec(&Loopback_Test_timer_Vss);
	if(cnt16 == 0)
		OSTimerGet_msec(&Loopback_timer_Vss);
	//-------------------------------------------------			
	cnt32++;	
	if(cnt32 >= 100)
	{
		if(loopback_disp)
			printf(VT_GREEN "[RCU] LoopBack : cnt32(%d) time(%d [ms])" VT_END_CR, 
						cnt32, OSTimerGet_msec(&Loopback_Test_timer_Vss));

		OSTimerReset( &Loopback_Test_timer_Vss );
		OSTimerGet_msec(&Loopback_timer_Vss);
		cnt32 = 0;
		cnt16 = 0;
	}
	//-------------------------------------------------			
	if(Loopbackmode != Loopback_ack)
	{	
		Write_FPGA_Reg(Reg_VSS_UL_Addr[cpri][Reg_VSS_UL_RE_Loopbackmode_ACK], Loopbackmode, "VSS_UL_Loopback");
		
		cnt16++;

		if(loopback_disp)
			printf(VT_GREEN "[RCU] LoopBack(CPRI%d, Request 0x%02X, Current UL 0x%02X), Time %d[ms], cnt16 %d\n" VT_END_CR, 
					cpri, 
					Loopbackmode, 
					Loopback_ack,
					OSTimerGet_msec(&Loopback_timer_Vss),
					cnt16);
		/*
		// TEST
		//----------------
		if(Test_timer_Vss_En == 0)
		{
			OSTimerReset(&Test_timer_Vss);
			Test_timer_Vss_En = 1;		
			printf(VT_GREEN "[RCU] LoopBack(CPRI%d, Request 0x%02X, Current UL 0x%02X) : Test_time Start. %d[ms], TaskCnt %d \n" VT_END_CR, 
				cpri, 
				Loopbackmode, 
				Loopback_ack,
				OSTimerGet_msec(&Test_timer_Vss), 
				task_cnt);
		}
		*/
		//----------------
	}
	
 	     if(Loopbackmode == 0xCC) Reg_ctrl = 0x10;
	else if(Loopbackmode == 0xCA) Reg_ctrl = 0x01;
	else                          Reg_ctrl = 0x00;
			
	if(m_Reg_Sts.RW_Mapper_Loopback.Data != Reg_ctrl)
	{
		m_Reg_Flg.RW_Mapper_Loopback.Data = _F_SET_;
		m_Reg_Ctl.RW_Mapper_Loopback.Data = Reg_ctrl;

		// 2019.05.29
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Mapper_Loopback], m_Reg_Ctl.RW_Mapper_Loopback.Data, "Mapper_Loopback");
	}

	/*	
	-----------------------------------------------
		14. RE Reset, RE1 Reset, RE2 Reset, RE3 Reset, RE4 Reset
		VSS를 통해 RE의 Reset을 제어함. RU1번의 경우 RE Reset/RE1 Reset을 모두 사용
	-----------------------------------------------	
	*/		


	INT8U reset_req = 0;
	INT8U reset_req_a = 0;		
	
	
	if(m_St.Ref_CPRI_No == CPRI_0)
	{	
		if(id == 0)
		{
			if(m_Reg_Sts.R_RE_Reset.RE__Reset_M)
			{			
				reset_req |= cbits[0];
				
				// clear after read Register
				m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
				m_Reg_Ctl.R_RE_Reset.RE__Reset_M = 1; // clr
				
				// 2019.04.30 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_Ack.uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_Ack.uD32 = 0xCC;
				
				OSTimerReset( &Test_timer );
				Test_timer_En = 1;
				D_Printf( DEBUG_SYSTEM, VT_GREEN "[RCU] Test_timer start %d[ms]\n",  OSTimerGet_msec(&Test_timer));
				printf(VT_GREEN "[RCU] Test_timer start %d[ms]\n" VT_END_CR, OSTimerGet_msec(&Test_timer));			
			}
			else
			{			
				// 2019.05.07 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_Ack.uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_Ack.uD32 = m_Reg_Sts.DL_CPRI[CPRI_0].RE_Reset.uD32;			
			}
		}
		//----------------------------------------		
		if(m_Reg_Sts.R_RE_Reset.RE1_Reset_M)
		{
			if(id == 0)  // reset myself 
			{
				reset_req |= cbits[1];
				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[0].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[0].uD32 = 0xCC;
			}			
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE1_Reset_M = 1; // clr
		}
		else
		{
			if(id == 0) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[0].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[0].uD32 = m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[0].uD32;

				//printf( VT_GREEN "[RCU] id 0 %d Reset ACK, 0x%02X.\n" VT_END_CR, 
				//	id, m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[0].uD32);
			}
		}
		//-------------------		
		if(m_Reg_Sts.R_RE_Reset.RE2_Reset_M)
		{
			if(id == 1) // reset myself  
			{
				reset_req |= cbits[2];

				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[1].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[1].uD32 = 0xCC;
			}
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE2_Reset_M = 1; // clr
		}
		else
		{
			if(id == 1) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[1].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[1].uD32 = m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[1].uD32;

				//printf(VT_GREEN "[RCU] id 1 %d Reset ACK, 0x%02X.\n" VT_END_CR,
				//	id, m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[1].uD32);
			}
		}	
		//-------------------
		if(m_Reg_Sts.R_RE_Reset.RE3_Reset_M)
		{
			if(id == 2)  // reset myself 
			{
				reset_req |= cbits[3];
				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[2].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[2].uD32 = 0xCC;
			}
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE3_Reset_M = 1; // clr
		}
		else
		{
			if(id == 2) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[2].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[2].uD32 = m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[2].uD32;

				//printf(VT_GREEN "[RCU] id 2 %d Reset ACK, 0x%02X.\n" VT_END_CR, 
				//	id, m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[2].uD32);
			}
		}		
		//-------------------
		if(m_Reg_Sts.R_RE_Reset.RE4_Reset_M)
		{
			if(id == 3)  // reset myself 
			{
				reset_req |= cbits[4];
				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[3].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[3].uD32 = 0xCC;
			}
		
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE4_Reset_M = 1; // clr
		}
		else
		{
			if(id == 3) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_0].RE_Reset_ACK[3].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_0].RE_Reset_ACK[3].uD32 = m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[3].uD32;

				//printf(VT_GREEN "[RCU] id 3 %d Reset ACK, 0x%02X.\n" VT_END_CR, 
				//	id, m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[3].uD32);
			}
		}
		//-------------------
	}
	//------------------------------------------------ 
	// alpha 
	//------------------------------------------------
	else 
	{
		if(m_Reg_Sts.R_RE_Reset.RE__Reset_A)
		{
			if(id == 0) // reset myself 
			{
				reset_req_a |= cbits[0];
				
				// clear after read Register
				m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
				m_Reg_Ctl.R_RE_Reset.RE__Reset_A = 1; // clr

				// 2019.04.30 : Reset ACK with 0xCC
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_Ack.uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_Ack.uD32 = 0xCC;
			}
		}
		else
		{
			if(id == 0) 
			{
				// 2019.05.07 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_Ack.uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_Ack.uD32 = m_Reg_Sts.DL_CPRI[CPRI_1].RE_Reset.uD32;
			}
		}
		//----------------------------------------	
		if(m_Reg_Sts.R_RE_Reset.RE1_Reset_A)
		{
			if(id == 0) // reset myself 
			{
				reset_req_a |= cbits[1];

				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[0].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[0].uD32 = 0xCC;
			}
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE1_Reset_A = 1; // clr
		}
		else
		{
			if(id == 0) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[0].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[0].uD32 = m_Reg_Sts.DL_CPRI[CPRI_1].REx_Reset[0].uD32;
			}
		}
		//-------------------
		if(m_Reg_Sts.R_RE_Reset.RE2_Reset_A)
		{
			if(id == 1) // reset myself 
			{
				reset_req_a |= cbits[2];
				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[1].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[1].uD32 = 0xCC;
			}
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE2_Reset_A = 1; // clr
		}
		else
		{
			if(id == 1) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[1].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[1].uD32 = m_Reg_Sts.DL_CPRI[CPRI_1].REx_Reset[1].uD32;
			}
		}
		//-------------------
		if(m_Reg_Sts.R_RE_Reset.RE3_Reset_A)
		{			
			if(id == 2)  // reset myself
			{
				reset_req_a |= cbits[3];
				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[2].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[2].uD32 = 0xCC;
			}
		
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE3_Reset_A = 1; // clr
		}
		else
		{
			if(id == 2) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[2].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[2].uD32 = m_Reg_Sts.DL_CPRI[CPRI_1].REx_Reset[2].uD32;
			}
		}
		//-------------------
		if(m_Reg_Sts.R_RE_Reset.RE4_Reset_A)
		{			
			if(id == 3) // reset myself
			{
				reset_req_a |= cbits[4];
				// 2019.04.30 : Reset ACK
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[3].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[3].uD32 = 0xCC;
			}					
			// clear after read Register
			m_Reg_Flg.R_RE_Reset.Data = _F_SET_;
			m_Reg_Ctl.R_RE_Reset.RE4_Reset_A = 1; // clr
		}
		else
		{
			if(id == 3) 
			{
				// 2019.05.07 : Reset ACK				
				m_Reg_Flg.UL_CPRI[CPRI_1].RE_Reset_ACK[3].uD32 = _F_SET_;
				m_Reg_Ctl.UL_CPRI[CPRI_1].RE_Reset_ACK[3].uD32 = m_Reg_Sts.DL_CPRI[CPRI_1].REx_Reset[3].uD32;
			}
		}
		//-------------------
	}
	
	if(reset_req || reset_req_a)
	{
		syslog( LOG_WARNING, VT_YELLOW "[RCU] RCU Restart REQ by VSS, RE_Reset Register 0x%X \n" VT_END_CR, 
			 m_Reg_Sts.R_RE_Reset.Data);

		syslog( LOG_WARNING, VT_YELLOW "[RCU] RCU Restart REQ by VSS, VSS 0x%X / 0x%X, 0x%X, 0x%X, 0x%X\n" VT_END_CR, 
			m_Reg_Sts.DL_CPRI[CPRI_0].RE_Reset.uD32,
			m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[0].uD32,
			m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[1].uD32,
			m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[2].uD32,
			m_Reg_Sts.DL_CPRI[CPRI_0].REx_Reset[3].uD32); 			 
		
		// RCU Restart
		print_agent(DEBUG_SYSTEM, VT_YELLOW, "Restart REQ by VSS--Main/Alpha : 0x%X/0x%X", 
			reset_req, reset_req_a );

		for(i = 0; i < 3; i++)
			print_agent( DEBUG_SYSTEM, VT_BOLD_RED, "Restart REQ by VSS....");

		Wr__Event_to_LogFile("[VSS Reset  ] by VSS--Main/Alpha : 0x%X/0x%X", reset_req, reset_req_a);

		#ifdef __TYPE__PRU__	
			#ifdef __PSU_RESET_Cancel_When_VSS_RESET__
			// do nothing
			D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] PSU Reset is Canceled----.\n");
			syslog( LOG_WARNING, VT_YELLOW "[RCU] PSU Reset is Canceled----.\n" VT_END_CR);
			#else
			m_Env.psu_reset_req = 1; // power reset
			#endif	
		#else
			m_Env.is_reboot = 1;
			m_Env.is_down_load_me = 1;
		#endif
		SetResetInfo( Rst_By_Vss, 4594 ); // 2019.08.27 : etc -> Rst_By_Vss
	}

			
			
		

	/*
	CnM_Path_selection : CPRI 4개중, 어느것을 참조?
	-----------------------------------------------
	4.	Fast C&M Path Setting
	C&M Path Selection
		when 0xCC : Register Map "ETH Path Sel" 0x00000000 set : 100 Mhz Selection
		when 0x33 : Register Map "ETH Path Sel" 0x00010001 set : alpha Mhz Selection

	After Setting Register Map,
	Send Notifi msg to L/S via C&M
	-----------------------------------------------	
	*/	
	uVal32 = m_Reg_Sts.RW_ETH_Path_Sel.Data;
	flag = 0; // init 0

	if(m_St.Ref_CPRI_No == CPRI_0) 
		c = CPRI_0;
	else
		c = CPRI_1;

	#if 0
	D_Printf(DEBUG_SYSTEM, "[RCU] c(%d), VSS-C&M_Path_sel 0x%X / RW_ETH_Path_Sel 0x%X\n", 
			c, 
			m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32, 
			m_Reg_Sts.RW_ETH_Path_Sel.Data);
	#endif
	
	if((INT8U)m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32 == 0xCC)
	{			
		if(uVal32 != 0x00000000) { flag = 1; uVal32_req = 0x00000000; }
	}
	else if((INT8U)m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD32 == 0x33)
	{
		if(uVal32 != 0x00010001) { flag = 1; uVal32_req = 0x00010001; }
	}
	else
	{			
		if(uVal32 != 0x00000000) { flag = 1; uVal32_req = 0x00000000; }
	}
		
	if(flag)
	{
		m_Reg_Flg.RW_ETH_Path_Sel.Data = 1;
		m_Reg_Ctl.RW_ETH_Path_Sel.Data = uVal32_req;

		//D_Printf(DEBUG_SYSTEM, "[RCU] ETH_Path_Sel ctrl  0x%X\n\n", uVal32_req);
	}
	

	/*
	CPRI 4개중, 어느것을 참조?
	-----------------------------------------------
	5.	VSD Channel BW
	Setting Register Map
		DL_BW_SEL, UL_BW_SEL
	according to VSS-RE Channel BW
	
	C&M Path Selection
		when 0xCC : Register Map "ETH Path Sel" 0x00000000 set
		when 0x33 : Register Map "ETH Path Sel" 0x00010001 set

	After Setting Register Map,
	Send Notifi msg to L/S via C&M
	-----------------------------------------------	
	*/

	INT8U bw_100, bw___A, bw__all;	

	#if 0
	// 2019.09.09
	INT8U instant_lock;
	
	instant_lock = Get_Instant_CPRI_Lock_St();
	if(instant_lock & cbits[CPRI_0]) 
		c = CPRI_0;
	else
		c = CPRI_1;
	#else
	if(m_St.Ref_CPRI_No == CPRI_0) 
		c = CPRI_0;
	else
		c = CPRI_1;	
	#endif	
	
	// reference
	bw__all = (INT8U)m_Reg_Sts.DL_CPRI[c].RE_Channel_BW.uD32;
	bw_100  = bw__all & 0x0F;      // lower 4bit, 100_M
	bw___A  = (bw__all & 0xF0)>>4; // upper 4bit, Alpha

	// current
	data[0] = m_St.d_RW_DL_BW_Sel[FA_0][0]; // 100_M
	data[1] = m_St.d_RW_DL_BW_Sel[FA_1][0]; // Alpha	


	if(IS_BW_Valid(bw__all, bw_100, bw___A))
	{
		////////if((bw_100 != data[0])||(bw___A != data[1]))
		Use_d_RE_Channel_BandWidth(bw_100, bw___A, 'V'); // call when cpri-lock
		cnt_bw = 0;
	}
	else
	{
		if((cnt_bw % 10)==0)
		{
			print_agent(DEBUG_SYSTEM, VT_RED, " BW InValid : 100(%d) A(%d), all(0x%X), continue(%d)", 
						bw_100, bw___A, bw__all, cnt_bw);
			print_agent(DEBUG_SYSTEM, VT_RED, " BW Current : 100(%d) A(%d)", 
						data[0], data[1]);
		}
		cnt_bw++;
	}

		
	

	/*	
	-----------------------------------------------
	7.	REMS IP/5G EMS 설정	
	(1) Noti-1 은 Layer splitter 의 IP 이고, Noti-2 는 5G EMS IP의 의미인지요?
	(2) VSS - DL- CPRI_0 의 1개만 대표로 사용하면 되는지요?
	-----------------------------------------------
	*/

	if(m_St.Ref_CPRI_No == CPRI_0) 
		c = CPRI_0;
	else
		c = CPRI_1;	

	// 2019.11.11
	INT8U ip[4];
	
	if(m_Env.test_ls_ip[0] == 0) // if not controlled by user
	{
		/*
		// 2019.11.18 : delete
		//AufoConfigFlag.Recv_Ip[0] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[0].uD8[0];
		//AufoConfigFlag.Recv_Ip[1] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[1].uD8[0];
		//AufoConfigFlag.Recv_Ip[2] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[2].uD8[0];
		//AufoConfigFlag.Recv_Ip[3] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[3].uD8[0];
		*/

		// 2019.11.18		
		ip[0] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[0].uD8[0];
		ip[1] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[1].uD8[0];
		ip[2] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[2].uD8[0];
		ip[3] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[3].uD8[0];
		
		Update__Layer_IP(ip);
	}

	/*
	// 2019.11.11 : delete
	//AufoConfigFlag.Recv_Ip2[0] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[0].uD8[0];
	//AufoConfigFlag.Recv_Ip2[1] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[1].uD8[0];
	//AufoConfigFlag.Recv_Ip2[2] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[2].uD8[0];
	//AufoConfigFlag.Recv_Ip2[3] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[3].uD8[0];
	*/	
	
	ip[0] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[0].uD8[0];
	ip[1] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[1].uD8[0];
	ip[2] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[2].uD8[0];
	ip[3] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[3].uD8[0];
	
	Update__gREMS_IP(ip);


	/*
	-----------------------------------------------
	8.	REC Port ID
	VSS : REC Port ID : use as L/S-H/W-Port(0~15) number
	-----------------------------------------------
		- m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD32 is VSS : REC Port ID
		- m_St.g_LS_Port_No[0] is "AufoConfigFlag.REC_Port_No"
	*/

	if(m_St.Ref_CPRI_No == CPRI_0) 
		c = CPRI_0;
	else
		c = CPRI_1;
					
	AufoConfigFlag.REC_Port_No = m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD8[0];
	AufoConfigFlag.PCI_16 = m_St.d_PCI[c];

	/*	
	-----------------------------------------------
	VSS-UL-<CPRI 0,1> only
	-----------------------------------------------
	10. RE Vendor 확인
		- Write '0xAA' : 제조사 고유 code : ANTS
	11. RE Type ID 확인		
		- AAU 0x55 / PRU 0x66
	-----------------------------------------------
	*/
	for(c = CPRI_0; c <= CPRI_1; c++) // CPRI 0,1 only
	{
		//---------------------------
		if(m_Reg_Sts.UL_CPRI[c].RE_Vendor_ID.uD32 != 0xAA) // '0xAA' is 'ANTS'
		{
			m_Reg_Flg.UL_CPRI[c].RE_Vendor_ID.uD32 = 1;
			m_Reg_Ctl.UL_CPRI[c].RE_Vendor_ID.uD32 = 0xAA;
		}		
		//---------------------------
		#ifdef __TYPE__PRU__
		uVal8_req = 0x66;
		#else
		uVal8_req = 0x55;
		#endif
		if(m_Reg_Sts.UL_CPRI[c].RE_Type_ID.uD32 != uVal8_req)
		{
			m_Reg_Flg.UL_CPRI[c].RE_Type_ID.uD32 = 1;
			m_Reg_Ctl.UL_CPRI[c].RE_Type_ID.uD32 = uVal8_req;
		}
		//---------------------------
	}
	

	/*	
	-----------------------------------------------
	12.	Function Fail 확인
		(1) 모든 Tx Path의 Shutdown 알람(OverPower, HighTemp, Clockfail, DC Fail) 발생시
		(2) Reset Command 수신시 (VSS or C&M Channel)

	동작 -----	
	VSS 채널
		RE Function Fail, and RE#n Function Fail : normal 0x33, err 0xCC

	Function fail은 defalut가 0xCC로 시작, 이후 알람 체크 시작부터 상태에 맞게 변경되도록 수정
	-----------------------------------------------
	*/	
	
	// Check_VSS_Reset_Command	
	INT8U re_reset[2] = {0,0}; // init 0
	INT8U my_func_fail = 0x33; // normal init
	INT8U reset_val, tx_path_sd;
	INT32U refer__32, target_32;
	INT8U my_casc_id = AufoConfigFlag.RE_Cascade_ID; // 0 ~ 3, 2019.05.08

	// Get Reset Status
	for(i = CPRI_0; i <= CPRI_1; i++)
	{		
		if(my_casc_id == 0)
		{
			reset_val = (INT8U)m_Reg_Sts.DL_CPRI[i].RE_Reset.uD32;
			
			if(reset_val == 0xCC) re_reset[i] |= 1;
		}
		
		for(k = 0; k < 4; k++)
		{
			if(my_casc_id == k)
			{
				reset_val = (INT8U)m_Reg_Sts.DL_CPRI[i].REx_Reset[k].uD32;
				
				if(reset_val == 0xCC) re_reset[i] |= 1;
			}
		}
	}
	//--------------------------------------------------		
	// get my function fail	
	if(m_St.AlmSt.A_4.Clock)  my_func_fail = 0xCC; // clock alarm
	else if(re_reset[CPRI_0]) my_func_fail = 0xCC; // reset_req		
	else if(re_reset[CPRI_1]) my_func_fail = 0xCC; // reset_req		
	else
	{	
		my_func_fail = 0x33; // normal init
		
		for(c = 0, tx_path_sd = 0, path4 = 0; path4 < MAX_PATH_4; path4++)
		{
			if(Check_PAU_Use(path4))
			{
				c++; // use_count
				if(m_St.AlmSt.Path[path4].PAU.Shutdown) tx_path_sd++;
				else if(m_St.AlmSt.Path[path4].PAU.Disabled) tx_path_sd++;
			}
		}		
		if(c > 0) 
		{
			if(tx_path_sd == c) 
				my_func_fail = 0xCC; // alarm	
		}
		/*
			"PAU.Shutdown" includes 
			'Dev_Fail', 'DC_Fail', 'VSWR', 'High_Temp' and 'Over_Power'.
		*/
	}
	//D_Printf(DEBUG_SYSTEM, "[RCU] REFunctionFail,  my_func_fail 0x%X.\n", my_func_fail);
	//--------------------------------------------------	
	for(c = CPRI_0; c <= CPRI_1; c++) // CPRI 0,1 only
	{
		if(my_casc_id == 0)
		{
			if(m_Reg_Sts.UL_CPRI[c].REFunctionFail.uD32 != my_func_fail)
			{
				m_Reg_Flg.UL_CPRI[c].REFunctionFail.uD32 = 1;
				m_Reg_Ctl.UL_CPRI[c].REFunctionFail.uD32 = my_func_fail;
				Write_FPGA_Reg(Reg_VSS_UL_Addr[c][Reg_VSS_UL_REFunctionFail], m_Reg_Ctl.UL_CPRI[c].REFunctionFail.uD32, "REFunctionFail");

				//D_Printf(DEBUG_SYSTEM, "[RCU] CPRI%d UL REFunctionFail set 0x%X.\n", 
				//	c, my_func_fail);	
			}			
		}
		
		for(k = 0; k < 4; k++)
		{
			target_32 = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[k].uD32;			

			if(my_casc_id == k)
				refer__32 = my_func_fail;
			else
			{
				if(c == CPRI_0) refer__32 = m_Reg_Sts.UL_CPRI[CPRI_2].RE_FunctionFail[k].uD32;
				else            refer__32 = m_Reg_Sts.UL_CPRI[CPRI_3].RE_FunctionFail[k].uD32;
			}
			
			if(target_32 != refer__32)
			{
				m_Reg_Flg.UL_CPRI[c].RE_FunctionFail[k].uD32 = 1;
				m_Reg_Ctl.UL_CPRI[c].RE_FunctionFail[k].uD32 = refer__32;

				Write_FPGA_Reg(Reg_VSS_UL_Addr[c][Reg_VSS_UL_RE1FunctionFail+k], m_Reg_Ctl.UL_CPRI[c].RE_FunctionFail[k].uD32, "RE[x]FunctionFail");

				//D_Printf(DEBUG_SYSTEM, "[RCU] CPRI%d RE_FunctionFail[%d] set 0x%X, cascade_id %d\n", 
				//	c, k, refer__32, my_casc_id);
			}
		}
	}
	//--------------------------------------------------

	




		
	
	/*	
	-----------------------------------------------
	--- Message ID ---
	14. System Status : 항목 매핑 필요	
	15. Alarm Information : 항목 매핑 필요
	16.	Alarm Information Notification : 미구현	
	18. CPRI Status : 항목 매핑 필요
	
	17. Reset : Msg 수신은 되나 Reset은 미수행 -> Application 종료 후 
	PRU : PSU Reset 수행(방법 정의 필요)
	AAU : RCU S/W Reset 수행	
	*/
	
	
	if(cnt >= 10) 
	{
		cnt = 0;

		/*
		for(c=0; c<CPRI_PATH; c++)
		{
			D_Printf(DEBUG_SYSTEM, "[RCU] flag c %d, dl 0x%X, ul 0x%X\n",
				c,
				m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[c][DL],
				m_St.d_VSS_Z__85__OpenCpriVer___Mismatch_Noti[c][UL]);
		}
		
		Disp_IP_addr(AufoConfigFlag.Recv_Ip,  "LayerSplitter ");
		Disp_IP_addr(AufoConfigFlag.Recv_Ip2, "5G EMS ");
		D_Printf(DEBUG_SYSTEM, "[RCU] L/S REC-Port-No is %d,%d.\n", 
			AufoConfigFlag.REC_Port_No, m_St.g_LS_Port_No[0]);

		D_Printf(DEBUG_SYSTEM, "[RCU] RE_Type_ID 0x%X, 0x%X.\n", 
			m_Reg_Sts.UL_CPRI[0].RE_Type_ID.uD32,
			m_Reg_Sts.UL_CPRI[1].RE_Type_ID.uD32);	
		*/	
	}		

	/*
	-----------------------------------------------------------------
	TDD Slot Number
		CPRI Lock이 정상일 때, 
		VSS Register TDD Slot number 0~19번을 
		slot #0 ~19 에 write
	-----------------------------------------------------------------		
	*/
	
	c = m_St.Ref_CPRI_No; // 'CPRI_0' or 'CPRI_1'
	for(k = 0; k < 20; k++)
	{		
		#ifdef _DO_NOT_FOLLOW_VSS_Table_When_TDD_Slot_
		refer__32 = default_tdd_slot_val[k];
		#else
		refer__32 = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[k].uD32;
		#endif	
		
		target_32 = m_Reg_Sts.RW_Slot_Format[k].uD32;
		if(refer__32 != target_32)
		{		
			m_Reg_Flg.RW_Slot_Format[k].uD32 = _F_SET_;
			m_Reg_Ctl.RW_Slot_Format[k].uD32 = refer__32;
		
		}	
	}
}




INT8U Is_CRPI_Upper(INT8U cpri)
{
	if((cpri == CPRI_0)||(cpri == CPRI_1))
		return 1;
	else
		return 0;
}
INT8U Is_CRPI_Lower(INT8U cpri)
{
	if(Is_CRPI_Upper(cpri))
		return 0;
	else return 1;	
}

INT8U Is_CRPI_100Mhz(INT8U cpri)
{
	if((cpri == CPRI_0)||(cpri == CPRI_2))
		return 1;
	else
		return 0;
}
INT8U Is_CRPI_A_Mhz(INT8U cpri)
{
	if(Is_CRPI_100Mhz(cpri))
		return 0;
	else return 1;	
}



//-----------------------------------------------------------------
/*
	1. Check SFP Insert :CPRI_0/1
	2. Check lock & alarm of SFP-Insered CPRI.
	3. keeping normal for 2[s], then stable.
	4. access VSS-Channel Values
*/

static INT8U chk_cpri_st[CPRI_PATH] = {0,0,0,0}; // init 0(none)
enum
{
	Cpri_None = 0,
	Cpri_SFP_Insert, // Check insertion of SFP
	Cpri_Lock_Check, // check locking
	Cpri_Locked_Sts, // check change of insertion or lock

	Cpri_MAX_St 
};
static OS_TIMER Lock_time[CPRI_PATH];
static OS_TIMER UnLock_time[CPRI_PATH];

//-----------------------------------
void Chk_SFP_Insert__CPRI_Lock(INT8U cpri)
{
#ifdef __MAKER_ANTS__
	static INT8U insert_prev[CPRI_PATH] = {UnMount, UnMount, UnMount, UnMount};
	static INT8U init[CPRI_PATH] = {0, 0, 0, 0};
	INT8U insert_curr;
	INT8U  CPRI_Mscl_Lock = 0; // 2019.05.20
	static INT8U lock_1st[CPRI_PATH] = {0, 0, 0, 0};
	INT8U  lock_time = 10; // 10[s]
	INT8U  unlock_time = 2; // 2s

	if(cpri >= CPRI_PATH) return;

	// before 1st lock, lock time is 2[s]
	if(lock_1st[cpri] == 0)
		lock_time = 2; // 2[s]
	else
		lock_time = 10; // 10[s]

	//----------------------------------------------
	// 2019.05.20
	if(Is_CRPI_Upper(cpri))
	{
		if(CPRI_Mscl_Val[cpri] == 0) CPRI_Mscl_Lock = 1;
		else CPRI_Mscl_Lock = 0;
	}
	else // CPRU2/3, CPRI-Slave
	{
		if(CPRI_Mscl_Val[cpri] == 0x0080) CPRI_Mscl_Lock = 1;
		else CPRI_Mscl_Lock = 0;
	}
	//----------------------------------------------
	// 2019.05.20
	//if(Is_CRPI_100Mhz(cpri)) optic = _100M;
	//else optic = Alpha; // CPRU2/3

	INT8U _100_a = 0;
	INT8U idx = 0;
	
	switch(cpri)
	{
		case CPRI_0: _100_a = _100M; idx = 0; break;
		case CPRI_2: _100_a = _100M; idx = 1; break;
		
		case CPRI_1: _100_a = Alpha; idx = 0; break;		
		case CPRI_3: _100_a = Alpha; idx = 1; break;
	}
	//----------------------------------------------

	switch(chk_cpri_st[cpri])
	{
		//----------------------------------------------
		case Cpri_None:
			chk_cpri_st[cpri] = Cpri_SFP_Insert;
			OSTimerReset(&Lock_time[cpri]);
			OSTimerReset(&UnLock_time[cpri]);
			break;
		
		//----------------------------------------------
		case Cpri_SFP_Insert:
			if(init[cpri] == 0)
			{
				// start from lock status
				init[cpri] = 1;
				///m_Env.CPRI_Lock_St |= cbits[cpri]; // lock				
				// start from unlock status : 2019.07.01
				m_Env.CPRI_Lock_St &= nbits[cpri]; // unlock [v]
			}
			else
			{
				m_Env.CPRI_Lock_St &= nbits[cpri]; // unlock [v]
			}
			
			if(m_St.d_SFP_Insert_St[_100_a][idx] == __Mount) // CRPI0
			{	
				chk_cpri_st[cpri] = Cpri_Lock_Check; // next
				OSTimerReset(&Lock_time[cpri]);
				OSTimerReset(&UnLock_time[cpri]);
			}
			else
			{
				chk_cpri_st[cpri] = Cpri_SFP_Insert; // current
			}
			break;
		
		//----------------------------------------------
		case Cpri_Lock_Check:  // now unlock, check whether lock or not
			OSTimerReset(&UnLock_time[cpri]);
			if((CPRI_Lock_Val[cpri] == CPRI_LOCK_Val) && CPRI_Mscl_Lock) // 2019.05.20
			{
				#ifdef __2019_1226__Use_Alarm_Timer_Structure__
				// CPRI normal : when keeping normal for 3s
				lock_time = 3; // 3s
				#endif
				if(OSTimerGet_msec(&Lock_time[cpri]) > (lock_time*TICK_CO_SEC))
				{
					m_Env.CPRI_Lock_St |= cbits[cpri]; // lock
					chk_cpri_st[cpri] = Cpri_Locked_Sts; // next
					OSTimerReset(&Lock_time[cpri]);
					lock_1st[cpri] = 1;

					print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "CPRI%d Lock, %d[s], Lock_Reg(0x%X), Mscl_Reg(0x%04X)", 
						cpri, lock_time, CPRI_Lock_Val[cpri], CPRI_Mscl_Val[cpri]);	
				}				
			}
			else OSTimerReset(&Lock_time[cpri]);
		break;
		
		//----------------------------------------------
		case Cpri_Locked_Sts: // now lock, check whether unlock or not
			OSTimerReset(&Lock_time[cpri]);
			insert_curr = m_St.d_SFP_Insert_St[_100_a][idx]; // m_St.d_SFP_Insert_St[optic][0];
			//---------------------------			
			// if unlock
			if((CPRI_Lock_Val[cpri] == CPRI_LOCK_Val) && CPRI_Mscl_Lock)
			{
				OSTimerReset(&UnLock_time[cpri]);
			}
			else
			{			
				#ifdef __2019_1226__Use_Alarm_Timer_Structure__
				// CPRI Alarm : when keeping Alarm for 3s
				unlock_time = 3; // 3s
				#else 
				unlock_time = 2; // 2s
				#endif
				if(OSTimerGet_msec(&UnLock_time[cpri]) > (unlock_time*TICK_CO_SEC))
				{
					m_Env.CPRI_Lock_St &= nbits[cpri]; // unlock
					chk_cpri_st[cpri] = Cpri_Lock_Check;
					OSTimerReset(&UnLock_time[cpri]);
				
					print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "CPRI%d Unlock 2[s], Lock_Reg(0x%X), Mscl_Reg(0x%04X)", 
						cpri, CPRI_Lock_Val[cpri], CPRI_Mscl_Val[cpri]);				
				}
			}				
			
			if(insert_prev[cpri] != insert_curr)
			{
				if(insert_curr == UnMount) // if mount -> unmount
				{
					chk_cpri_st[cpri] = Cpri_SFP_Insert; // next
					
					if((cpri == CPRI_0)||(cpri == CPRI_1))
						print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "goto CPRI%d Cpri_SFP_Insert", cpri);
				}
			}
			//---------------------------	
			insert_prev[cpri] = insert_curr;
			break;
		
		//----------------------------------------------
		default:
			chk_cpri_st[cpri] = Cpri_SFP_Insert;
			break;
	}	
#endif
}

INT8U Get_CPRI_Lock(INT8U cpri)
{
	if(cpri >= CPRI_PATH) return 0;

	return (m_Env.CPRI_Lock_St & cbits[cpri]);
}

static INT8U cpri_reason[CPRI_PATH] = {0,0,0,0};

void Proc__SFP_Insert__CPRI_Lock(void)
{	
	INT8U cpri, c;
	INT8U Ref_CPRI_No = m_St.Ref_CPRI_No;
	INT8U prev_cpri_lock;
	INT8U instant_lock = 0;
	static INT8U instant_lock_prev = 0;
	
	

	prev_cpri_lock = m_St.Is_CPRI_Lock;

	for(cpri = 0; cpri < CPRI_PATH; cpri++)
		Chk_SFP_Insert__CPRI_Lock(cpri);

	if(m_Env.CPRI_Lock_St & cbits[CPRI_0])
	{		
		m_St.Ref_CPRI_No = CPRI_0;
		m_St.Is_CPRI_Lock = 1;
	}
	else if(m_Env.CPRI_Lock_St & cbits[CPRI_1])
	{
		m_St.Ref_CPRI_No = CPRI_1;
		m_St.Is_CPRI_Lock = 1;
	}
	else
	{	
		////m_St.Ref_CPRI_No = CPRI_0; // default, useless if unlock, delete 2019.09.19
		m_St.Is_CPRI_Lock = 0;
	}

	if(Ref_CPRI_No != m_St.Ref_CPRI_No)
	{
		Init_Vss_Stable_Check(0);
		print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "Ref_CPRI_No : %d ---> %d !! ----(Lock_St %d, %d)",
			Ref_CPRI_No, m_St.Ref_CPRI_No, Get_CPRI_Lock(CPRI_0), Get_CPRI_Lock(CPRI_1));
	}
	if(prev_cpri_lock != m_St.Is_CPRI_Lock)
	{
		Init_Vss_Stable_Check(0);
		print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "CPRI_Lock Changed : %d ------> %d, (0:unlock)",
			prev_cpri_lock, m_St.Is_CPRI_Lock);
	}
	
	instant_lock = Get_Instant_CPRI_Lock_St();
	if(instant_lock_prev != instant_lock)
	{
		Init_Vss_Stable_Check(0);
		print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "CPRI_Lock(Instance): %d ------> %d, (0:unlock)", instant_lock_prev, instant_lock);
		print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "Lock_Val (Instance): 100M 0x%X, Alpha 0x%X", CPRI_Lock_Val[CPRI_0], CPRI_Lock_Val[CPRI_1]);	
		print_agent( DEBUG_SYSTEM, VT_BOLD_MAGENTA, "Mscl_Val (Instance): 100M 0x%X, Alpha 0x%X", CPRI_Mscl_Val[CPRI_0], CPRI_Mscl_Val[CPRI_1]);
				
		instant_lock_prev = instant_lock;
	}

	// 2019.04.08
	//-----------------------------------------------
	INT8U CPRI_Alm[CPRI_PATH];
	INT8U CPRI_Alm_Prev[CPRI_PATH];

	CPRI_Alm_Prev[CPRI_0] = m_St.AlmSt.A_2.CPRI_0__;
	CPRI_Alm_Prev[CPRI_1] = m_St.AlmSt.A_2.CPRI_0_a;
	CPRI_Alm_Prev[CPRI_2] = m_St.AlmSt.A_2.CPRI_1__;
	CPRI_Alm_Prev[CPRI_3] = m_St.AlmSt.A_2.CPRI_1_a;
	
	if(m_St.AlmTest == 0) // 2019.05.02
	{
		CPRI_Alm[CPRI_0] = Get_CPRI_Lock(CPRI_0)? 0:1;
		CPRI_Alm[CPRI_1] = Get_CPRI_Lock(CPRI_1)? 0:1;
		CPRI_Alm[CPRI_2] = Get_CPRI_Lock(CPRI_2)? 0:1;
		CPRI_Alm[CPRI_3] = Get_CPRI_Lock(CPRI_3)? 0:1;

		for(c = CPRI_0; c < CPRI_PATH; c++)
		{
			if(Is_SFP_Inserted(c)==0)	
				CPRI_Alm[c] = 0;
				
			if(CPRI_Alm[c] == 0)
				m_St.d_CPRI_Alarm[c] = 0;
			else
				m_St.d_CPRI_Alarm[c] = cpri_reason[c];			
		}			

		if(IS_Alpha_Mhz_Existing() == 0) 
		{
			// 2019.07.17 : alarm masking of alpha
			CPRI_Alm[CPRI_1] = 0; 
			CPRI_Alm[CPRI_3] = 0;
		}

	#ifdef __TYPE__PRU__
		if(m_St.AlmSt.A_0.AC == 0) // 2019.07.27
	#endif
		{
			m_St.AlmSt.A_2.CPRI_0__ = CPRI_Alm[CPRI_0];
			m_St.AlmSt.A_2.CPRI_0_a = CPRI_Alm[CPRI_1];
			m_St.AlmSt.A_2.CPRI_1__ = CPRI_Alm[CPRI_2];
			m_St.AlmSt.A_2.CPRI_1_a = CPRI_Alm[CPRI_3];
		}

		for(c = CPRI_0; c < CPRI_PATH; c++)
		{
			if(CPRI_Alm_Prev[c] != CPRI_Alm[c])
			{
				printf(VT_YELLOW "[CPRI%d Alarm] %d -> %d (alarm is 1)" VT_END_CR, 
					c, CPRI_Alm_Prev[c], CPRI_Alm[c]);

				D_Printf( DEBUG_SYSTEM, VT_YELLOW "[CPRI%d Alarm] %d -> %d (alarm is 1)", 
					c, CPRI_Alm_Prev[c], CPRI_Alm[c]);	
			}
		}
	}
}



/*
1. SFP 실장상태를 확인 : 
- 미실장시 : 관련 알람 mask
- 실장시   : 
	(1) CPRI Lock및 CPRI 알람확인(CPRI Register 0x00, 0x01) 하여
	(2) 정상 조건이 2초간 유지시 Stable 상태로 간주, 이후부터 VSS 채널의 값을 억세스 

우선 순위 : CPRI0 > CPRI1 
*/
//---------------------------------------------------------------------------------------------------


/*
CPRI 알람
------------------
CPRI 0x00 번지가 0x0F 아니면 alarm / reason : Core unlock

CPRI 0x01 하위 16bit가 0x0000(slave) or 0x0080(master)이 아닌경우 alarm.
Reason		
0x01번지에 15번비트가 Set 된 경우 reason : LOF
0x01번지에 14번비트가 Set 된 경우 reason : LOS
0x01번지에 다른 비트들은 무시

AID 0x5301
------------------
0x00: 정상
0x01: LOS (CPRI)
0x02: LOF (CPRI)
0x04: LD (SFP)
0x08: PD (SFP)
0x10: CPRI Core Unlock
*/


void Reboot__CPRI_Alarm_Mask(void)
{
#ifdef __TYPE__PRU__  // ANTS-PRU		
	INT8U cpri;
	
	for(cpri = 0; cpri < CPRI_PATH; cpri++)
	{
		CPRI_Lock_Val[cpri] = CPRI_LOCK_Val;		
	
		if((cpri == CPRI_0)||(cpri == CPRI_1))
			CPRI_Mscl_Val[cpri] = 0;		
		else 
			CPRI_Mscl_Val[cpri] = 0x0080;
	}

	m_St.AlmSt.A_2.CPRI_0__ = 0;
	m_St.AlmSt.A_2.CPRI_0_a = 0;
	m_St.AlmSt.A_2.CPRI_1__ = 0;
	m_St.AlmSt.A_2.CPRI_1_a = 0;
#endif	
}


void Proc__Update_CPRI_Mscl_Status(void)
{
	INT8U i, c;	
	INT8U reason; 

#ifdef __TYPE__PRU__  // ANTS-PRU		
	for(i = 0; i < CPRI_PATH; i++)
	{
		A_MMAP_St[i].CPRI_Status = Read_A_PRU_Mmap(i, MMAP_CPRI_Status) & 0x00FF; // lower 8 bits
		A_MMAP_St[i].Mscl_Status.Data = Read_A_PRU_Mmap(i, MMAP_Mscl_Status); // lower 16bits
		
		CPRI_Lock_Val[i] = A_MMAP_St[i].CPRI_Status;
		CPRI_Mscl_Val[i] = (INT16U)A_MMAP_St[i].Mscl_Status.Data; // 2019.05.20

		if(m_Env.CPRI_CLK_Alm_Mask_at_Reboot) 
			Reboot__CPRI_Alarm_Mask(); // 2020.01.09
		//-------------------------------------------			
		A_MMAP_St[i].LoopBack_Ethernet_St = Read_A_PRU_Mmap(i, MMAP____LoopBack) & 0xFF; // lower 8 bits;	
		// 2019.05.20		
		A_MMAP_St[i].MMAP___0x0B = Read_A_PRU_Mmap(i, MMAP_____0x0b);
		A_MMAP_St[i].MMAP___0x0D = Read_A_PRU_Mmap(i, MMAP_____0x0d);				
	}
#else
	for(i = 0; i < CPRI_PATH; i++)
	{	
		m_Reg_Sts.AXI[i].Cpri_Sts.Data = Read_FPGA_Reg( Reg_AXI_CPRI_Addr[i][Reg_AXI_CPRI_R_Status]);
		m_Reg_Sts.AXI[i].Mscl_Status.Data = Read_FPGA_Reg( Reg_AXI_CPRI_Addr[i][Reg_AXI_CPRI_R_MiscAlm]);

		CPRI_Lock_Val[i] = m_Reg_Sts.AXI[i].Cpri_Sts.Alarm;
		CPRI_Mscl_Val[i] = (INT16U)m_Reg_Sts.AXI[i].Mscl_Status.Data; // 2019.05.20

		if(m_Env.CPRI_CLK_Alm_Mask_at_Reboot) 
			Reboot__CPRI_Alarm_Mask(); // 2020.01.09
	}
#endif

#ifdef __TYPE__PRU__  // ANTS-PRU
	for(i = 0; i < CPRI_PATH; i++)
	{
		reason = 0;

		if(CPRI_Lock_Val[i] != CPRI_LOCK_Val) reason |= cbits[4]; // 2019.04.25. AID 0x5301, CPRI Reason Added.
		if(A_MMAP_St[i].Mscl_Status.LOS_CPRI) reason |= cbits[0]; // bit 14, LOS 유지.(2019.06.22)
		if(A_MMAP_St[i].Mscl_Status.LOF_CPRI) reason |= cbits[1]; // bit 15, LOF 유지.(2019.06.22)		

		// 2019.06.22 : Main_AID (CPRI, 0x5301)의 LD/PD Reason 제거
		/*
		////if(A_MMAP_St[i].Mscl_Status.SFP_LD)	reason |= cbits[2]; // bit 11, 삭제.(2019.06.22)
		////if(A_MMAP_St[i].Mscl_Status.SFP_PD)	reason |= cbits[3]; // bit 0 , 삭제.(2019.06.22)
		*/		
		
		cpri_reason[i] = reason;
	}
#else // -------------------- AAU
	for(i = 0; i < CPRI_PATH; i++)
	{	
		reason = 0;

		if(CPRI_Lock_Val[i] != CPRI_LOCK_Val)     reason |= cbits[4]; // 2019.04.25. AID 0x5301, CPRI Reason Added.	
		if(m_Reg_Sts.AXI[i].Mscl_Status.LOS_CPRI) reason |= cbits[0];
		if(m_Reg_Sts.AXI[i].Mscl_Status.LOF_CPRI) reason |= cbits[1];

		// 2019.06.22 : Main_AID (CPRI, 0x5301)의 LD/PD Reason 제거
		/*
		////if(m_Reg_Sts.AXI[i].Mscl_Status.SFP_LD)   reason |= cbits[2]; // bit 11, 삭제.(2019.06.22)
		////if(m_Reg_Sts.AXI[i].Mscl_Status.SFP_PD)   reason |= cbits[3]; // bit 0 , 삭제.(2019.06.22)
		*/	

		cpri_reason[i] = reason;
	}	
	//---------------------------------------------------------	
#endif	
	//---------------------------------------------------------	
	/*
	(z.22.0)Optic LOS Detect/Alarm 구현
	AID : LOS(SFP) 0x5302 참조.  Alarm 발생 시 0xCC, 해제 시 0x33
	*/
	for(c = CPRI_0; c < CPRI_PATH; c++)
	{
		if(m_St.LOS_SFP_St[c]) // if alarm 0xCC, else 0x33
		{
			m_St.d_VSS_Z__22__OpticLosDetect___Alarm[c][DL] = 0xCC;
			m_St.d_VSS_Z__22__OpticLosDetect___Alarm[c][UL] = 0xCC;
		}
		else
		{
			m_St.d_VSS_Z__22__OpticLosDetect___Alarm[c][DL] = 0x33;
			m_St.d_VSS_Z__22__OpticLosDetect___Alarm[c][UL] = 0x33;
		}
	}	
	//---------------------------------------------------------	
#if 1 
	//-----------------------------------------------	
	/*	
	2019.07.16 : enable : 상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어
	2019.06.19 요청, 2019.06.22 : PRU/AAU all deleted
	2019.06.09
	상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어 및 LD 알람 Masking
		1. 레지스터 기준으로 CPRI#0[#1] 알람 발생시 CPRI#2[#3]의 SFP Tx disable 제어
		2. AAU 동일 적용
	*/
	//-----------------------------------------------
	
	
	if(m_St.AlmSt.A_2.CPRI_0__)
	{
		if(m_St.d_RW_SFP_TX_DISABLE[CPRI_2] == ON)
		{
			m_Reg_Flg.Common_CPRI[CPRI_2].RW_SFP_TX_DISABLE.EnDis = _SET_;
			m_Reg_Ctl.Common_CPRI[CPRI_2].RW_SFP_TX_DISABLE.EnDis = OFF;			
		}
	}
	else
	{
		if(m_St.d_RW_SFP_TX_DISABLE[CPRI_2] == OFF)
		{
			m_Reg_Flg.Common_CPRI[CPRI_2].RW_SFP_TX_DISABLE.EnDis = _SET_;
			m_Reg_Ctl.Common_CPRI[CPRI_2].RW_SFP_TX_DISABLE.EnDis = ON;
		}
	}	
	//-----------------------------------------------
	if(m_St.AlmSt.A_2.CPRI_0_a)
	{
		if(m_St.d_RW_SFP_TX_DISABLE[CPRI_3] == ON)
		{
			m_Reg_Flg.Common_CPRI[CPRI_3].RW_SFP_TX_DISABLE.EnDis = _SET_;
			m_Reg_Ctl.Common_CPRI[CPRI_3].RW_SFP_TX_DISABLE.EnDis = OFF;
		}
	}
	else
	{
		if(m_St.d_RW_SFP_TX_DISABLE[CPRI_3] == OFF)
		{
			m_Reg_Flg.Common_CPRI[CPRI_3].RW_SFP_TX_DISABLE.EnDis = _SET_;
			m_Reg_Ctl.Common_CPRI[CPRI_3].RW_SFP_TX_DISABLE.EnDis = ON;
		}
	}	
	//-----------------------------------------------	
#endif	
}


/*
----------------------------------------------------------------------------
VSS 직접적인 read/write 시에만 사용.

연속 stable count를 계속 check

2초유지에 해당하는 것을 가지고 알고리즘 동작.
----------------------------------------------------------------------------
*/
INT8U Get_Instant_CPRI_Lock_St(void)
{
	INT8U c;
	INT8U cpri_lock_st = 0;

	for(c = CPRI_0; c <= CPRI_1; c++) // Main
	{
	#ifdef __TYPE__PRU__  // PRU		
		A_MMAP_St[c].CPRI_Status = Read_A_PRU_Mmap(c, MMAP_CPRI_Status) & 0x00FF; // lower 8 bits
		A_MMAP_St[c].Mscl_Status.Data = Read_A_PRU_Mmap(c, MMAP_Mscl_Status); // lower 16bits
		
		CPRI_Lock_Val[c] = A_MMAP_St[c].CPRI_Status;
		CPRI_Mscl_Val[c] = (INT16U)A_MMAP_St[c].Mscl_Status.Data;
	#else // -------------------- AAU		
		m_Reg_Sts.AXI[c].Cpri_Sts.Data = Read_FPGA_Reg( Reg_AXI_CPRI_Addr[c][Reg_AXI_CPRI_R_Status]);
		m_Reg_Sts.AXI[c].Mscl_Status.Data = Read_FPGA_Reg( Reg_AXI_CPRI_Addr[c][Reg_AXI_CPRI_R_MiscAlm]);

		CPRI_Lock_Val[c] = m_Reg_Sts.AXI[c].Cpri_Sts.Alarm;
		CPRI_Mscl_Val[c] = (INT16U)m_Reg_Sts.AXI[c].Mscl_Status.Data;		
	#endif

		if(m_Env.CPRI_CLK_Alm_Mask_at_Reboot) 
			Reboot__CPRI_Alarm_Mask(); // 2020.01.09
		
		if((CPRI_Lock_Val[c] == CPRI_LOCK_Val) && (CPRI_Mscl_Val[c] == 0)) // lock
		{				
			cpri_lock_st |= cbits[c]; // lock
		}
	}

	return cpri_lock_st;
}

INT8U IS__CPRI_Lock(INT8U cpri)
{
	INT8U cpri_lock_st;

	cpri_lock_st = Get_Instant_CPRI_Lock_St();
	
	if((cpri == CPRI_0)||(cpri == CPRI_2))
		return (cpri_lock_st & cbits[CPRI_0]);
	else
		return (cpri_lock_st & cbits[CPRI_1]);
}

void reg_SFP_Tx_EnDisable(INT8U cpri, INT8U onoff) 
{
	INT32U reg_addr;
	INT32U reg_ctrl;

	if(cpri >= CPRI_PATH) return;	
	if((onoff != ON)&&(onoff != OFF)) return;	
		
	m_Reg_Ctl.Common_CPRI[cpri].RW_SFP_TX_DISABLE.EnDis = onoff;

	reg_addr = Reg_Common_MIMO_Addr[cpri][Reg_Common_MIMO_RW_SFP_TX_Disable];
	reg_ctrl = m_Reg_Ctl.Common_CPRI[cpri].RW_SFP_TX_DISABLE.Data;	

	if(onoff == ON)
		Write_FPGA_Reg(reg_addr, reg_ctrl, "TX Enable");
	else
		Write_FPGA_Reg(reg_addr, reg_ctrl, "TX Disable");		
}

void PHY__Reset_Setting(void)
{
#ifdef __TYPE__PRU__
	int re_cnt = 0; // init

	/*
	// phy chip reset
		mdio-tool w eth0 0x00 0x9140
		mdio-tool w eth0 0x00 0x1140
	*/	
	re_cnt = 0; // init
	if(system_agent("mdio-tool w eth0 0x00 0x9140") == 0) re_cnt++;
	if(system_agent("mdio-tool w eth0 0x00 0x1140") == 0) re_cnt++;

	if(re_cnt == 2) 
		print_agent(DEBUG_SYSTEM, VT_CYAN, "phy_chip_reset success"); 

	OSTimeDly(OS_DELAY_100msec);
	
	/*
		mdio-tool w eth0 0x0d 0x001f
		mdio-tool w eth0 0x0e 0x0033
		mdio-tool w eth0 0x0d 0x401f
		mdio-tool w eth0 0x0e 0x0003
	*/	
	re_cnt = 0; // init
	if(system_agent("mdio-tool w eth0 0x0d 0x001f") == 0) re_cnt++;
	if(system_agent("mdio-tool w eth0 0x0e 0x0033") == 0) re_cnt++;	
	if(system_agent("mdio-tool w eth0 0x0d 0x401f") == 0) re_cnt++;
	if(system_agent("mdio-tool w eth0 0x0e 0x0003") == 0) re_cnt++;
	
	if(re_cnt == 4) 
		print_agent(DEBUG_SYSTEM, VT_CYAN, "phy_chip_reg set1 success"); 

	OSTimeDly(OS_DELAY_100msec);	
	
	/*
		mdio-tool w eth0 0x0d 0x001f
		mdio-tool w eth0 0x0e 0x0033
		mdio-tool w eth0 0x0d 0x401f
		mdio-tool r eth0 0x0e
	*/
	re_cnt = 0; // init
	if(system_agent("mdio-tool w eth0 0x0d 0x001f") == 0) re_cnt++;
	if(system_agent("mdio-tool w eth0 0x0e 0x0033") == 0) re_cnt++;	
	if(system_agent("mdio-tool w eth0 0x0d 0x401f") == 0) re_cnt++;
	if(system_agent("mdio-tool r eth0 0x0e") == 0) re_cnt++;
	
	if(re_cnt == 4) 
		print_agent(DEBUG_SYSTEM, VT_CYAN, "phy_chip_reg set2 success"); 
		
#else
	/*
	-----------------------	
	AAU에서 GPIO를 통한 phy chip reset
	-----------------------	
	gpio_0_27 (number27) : Low(reset), high(ON)	
	*/

	AAU_PHY_RST();
	OSTimeDly(OS_DELAY_100msec);	
	AAU_PHY__ON();
	
	print_agent(DEBUG_SYSTEM, VT_CYAN, "phy_chip_reset success"); 	
#endif
}





/*
--------------------------------------------------------------------------------------
	SFP Recovery 기능 ( SFP Tx disable -> 100ms 지연 -> SFP Tx Enable)
	
	ii. CPRI unlock -> lock 된 상태에서 10초 동안 ARP를 5회 요청하여 3회 이상 응답이 오는지 체크(반복) : 
		every 2[s], ARP, 5 times during 10s

		fail : recovery 
		succuss : after 10[s], ping every 2[s] for 2 minutes.(if fail go to recovery, else every 10[s] ping)
		
	2.	every 10[s] ping : 
		fail : 연속 3회 이상 미 수신시 go to recovery
		success : continue...
	
	4.	복구 기능 수행 : 
		SFP Recovery기능 구행 및 
		Phy Chip Reset 후 Register 다시 제어
--------------------------------------------------------------------------------------	
*/

enum
{
	SFP_Rcvry_None = 0,
	SFP_Rcvry_unLock,
	SFP_Rcvry_Lock,	
	SFP_Rcvry_Start,
	SFP_Rcvry_Chk_5_ARP,// every 2[s], for 10[s]
	SFP_Rcvry_ARP__2s,  // every 2[s], for 2[m]
	SFP_Rcvry_ARP_Dly,  // delay for 10[s]
	SFP_Rcvry_ARP_10s, // every 10[s]
	SFP_Rcvry_TX_PHY,

	SFP_Rcvry_Max
};

static OS_TIMER sfp_recovery_time;

/*
	AAU 는 어떻게 ?? (phy reset...)
	TX disable cpri0~3 ??
	phy register setting의 의미.
*/

void Proc___SFP_Recovery(void)
{
	static INT8U rcvry_st = 0;
	static INT8U rcvry_prev = 0;
	static INT8U arp_send_cnt = 0;
	static INT8U arp_cnt = 0;
	static INT8U fail_cnt = 0;
	INT8U  ping_ip[4];	
	INT8U  cpri_lock;

	memcpy(ping_ip, AufoConfigFlag.Recv_Ip, 4);

	cpri_lock = Get_Instant_CPRI_Lock_St();

	// 동작 중, unlock 되면, 다시 초기 상태로...
	if(cpri_lock == 0)
	{
		switch(rcvry_st)
		{
			case SFP_Rcvry_Lock:	
			case SFP_Rcvry_Start:
			case SFP_Rcvry_Chk_5_ARP:
			case SFP_Rcvry_ARP__2s:
			case SFP_Rcvry_ARP_Dly:
			case SFP_Rcvry_ARP_10s:
			case SFP_Rcvry_TX_PHY:
				rcvry_st = SFP_Rcvry_None;	
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[SFP Recovery] Go to unLock-Step ---------"); 
				break;
		}		
	}

	switch(rcvry_st)
	{
		case SFP_Rcvry_None:
			OSTimerReset(&sfp_recovery_time);
			
			if(m_St.Is_CPRI_Lock) rcvry_st = SFP_Rcvry_Lock;
			else				  rcvry_st = SFP_Rcvry_unLock;
			break;

		case SFP_Rcvry_unLock:			
			if(cpri_lock)
				rcvry_st = SFP_Rcvry_Lock;
			break;		

		case SFP_Rcvry_Lock:			
			if(cpri_lock) 
			{
				if(rcvry_prev == SFP_Rcvry_unLock) // if unlock -> lock
					rcvry_st = SFP_Rcvry_Start;
			}
			break;
			
		case SFP_Rcvry_Start:	
			OSTimerReset(&sfp_recovery_time);
			arp_send_cnt = 0;
			arp_cnt = 0;
			fail_cnt = 0;
			rcvry_st = SFP_Rcvry_Chk_5_ARP;			
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[SFP Recovery] Start---------"); 
			break;
			
		case SFP_Rcvry_Chk_5_ARP:
			if(arp_send_cnt < 5)
			{
				if(OSTimerGet_msec(&sfp_recovery_time) > (2*TICK_CO_SEC))
				{
					OSTimerReset(&sfp_recovery_time);
					
					if(Ping_Test(ping_ip)) arp_cnt++; // ping_ok
					arp_send_cnt++;
					
					if(arp_send_cnt == 5)
					{
						if(arp_cnt >= 3)
						{
							print_agent(DEBUG_SYSTEM, VT_YELLOW, "[SFP Recovery] 3 times-ping OK---------"); 
							rcvry_st = SFP_Rcvry_ARP_Dly;
						}
						else
						{
							print_agent(DEBUG_SYSTEM, VT_RED, "[SFP Recovery] ping Fail(1) --------"); 
							rcvry_st = SFP_Rcvry_TX_PHY;	
						}						
					}
				}
			}
			break;

		case SFP_Rcvry_ARP_Dly: // 10[s] delay
			if(OSTimerGet_msec(&sfp_recovery_time) > (2*TICK_CO_SEC))
			{
				OSTimerReset(&sfp_recovery_time);
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[SFP Recovery] 10[s] delay end---------"); 

				arp_send_cnt = 0;
				fail_cnt = 0;
				arp_cnt = 0;
				rcvry_st = SFP_Rcvry_ARP__2s;
			}
			break;

		case SFP_Rcvry_ARP__2s:
			if(arp_send_cnt < 60) // 120[s]/2[s] = 60
			{
				if(OSTimerGet_msec(&sfp_recovery_time) > (2*TICK_CO_SEC))
				{
					OSTimerReset(&sfp_recovery_time);
					arp_send_cnt++;

					if(Ping_Test(ping_ip)) 
						 fail_cnt = 0; // fail count
					else fail_cnt++; // fail count
					
					if(fail_cnt >= 3) // continuous 3 times
					{
						print_agent(DEBUG_SYSTEM, VT_RED, "[SFP Recovery] 3 times ping Fail(2) ---------"); 
						rcvry_st = SFP_Rcvry_TX_PHY;	
					}
				}
			}
			else
			{
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[SFP Recovery] 120[s] ping OK ---------");
				OSTimerReset(&sfp_recovery_time);
				fail_cnt = 0;
				rcvry_st = SFP_Rcvry_ARP_10s;
			}
			break;	

		case SFP_Rcvry_ARP_10s:	
			if(OSTimerGet_msec(&sfp_recovery_time) > (10*TICK_CO_SEC))
			{
				OSTimerReset(&sfp_recovery_time);

				if(Ping_Test(ping_ip)) 
					 fail_cnt = 0; // fail count
				else fail_cnt++; // fail count
					
				if(fail_cnt >= 3) // continuous 3 times
				{
					print_agent(DEBUG_SYSTEM, VT_RED, "[SFP Recovery] 3 times ping Fail(3) ---------"); 
					rcvry_st = SFP_Rcvry_TX_PHY;
				}
			}
			break;

		case SFP_Rcvry_TX_PHY:
			print_agent(DEBUG_SYSTEM, VT_RED, "[SFP Recovery] TX Disable, PHY reset ---------"); 
			
			reg_SFP_Tx_EnDisable(CPRI_0, OFF);
			reg_SFP_Tx_EnDisable(CPRI_1, OFF);
			OSTimeDly(OS_DELAY_100msec);			
			reg_SFP_Tx_EnDisable(CPRI_0, ON);
			reg_SFP_Tx_EnDisable(CPRI_1, ON);

			PHY__Reset_Setting(); // internally 200[ms]
			
			rcvry_st = SFP_Rcvry_None;
			break;
	}
	rcvry_prev = rcvry_st;	
}


/*
--------------------------------------------------------------------------------------
	FAN Mode
	A.	일반모드
		- RPM Control according to temperature
		
	B.	저소음 모드		
		- when operation time :  FAN(외장) Max RPM(저소음 Mode) 값으로 RPM을 control
		- otherwise : 일반모드와 동일하게 동작
--------------------------------------------------------------------------------------
*/
enum
{
	AAU_FAN_OFF = 0,	
	AAU_FAN_LowNoise = 50,
	AAU_FAN_Normal = 100,
	
	AAU_FAN_Mode
};
//-----------------------------------------------------

				

static OS_TIMER aau_fan_timer;
void Proc_AAU_Ext_FAN_Ctrl( void ) // ANTS-AAU	
{
	INT8S cur_temp;
	INT8U fan_rpm; // 0 ~ 100
	static INT8S prev_fan_rpm = 100; // '100' is FAN-ON	
	INT8U cur_val, req_val;
	INT8U fan_mode;
	static INT8U init = 0;
	INT8U dbg_on = 0;

	if(init == 0)
	{
		init = 1;
		OSTimerReset(&aau_fan_timer);
	}

	if(OSTimerGet_msec(&aau_fan_timer) < (1*TICK_CO_SEC)) return;
	else OSTimerReset(&aau_fan_timer); 

	print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl ] --------------");

	// 2019.05.08
	switch(m_Eep.FAN_Mode_Onoff)
	{
		case FAN_Mode____Normal:
			m_St.FAN_Mode_St = FAN_Mode____Normal;
			break;
			
		case FAN_Mode_Low_Noise:
			if(Is_Ext_FAN_Mode_Time())
			{
				fan_mode = FAN_Mode_Low_Noise;
				if(dbg_on) print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl] low-noise, fan mode time");
			}
			else if(Is_Ext_FAN_Low_Noise_Mode_for_24Hour()) // 2019.04.17 : "FAN 저소음Mode" is always On.
			{
				fan_mode = FAN_Mode_Low_Noise;
				if(dbg_on) print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl] low-noise, fan mode 24h");
			}
			else
			{
				fan_mode = FAN_Mode____Normal;
				if(dbg_on) print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl] normal   , out of time");
			}

			/*
			2019.07.18 : Fan 저소음 Mode인 경우
				-> FPGA 온도 90도 이상시 Fan Mode : 일반 Mode로 변경  
				-> FPGA 온도 85도 이하시 Fan Mode : 저소음 Mode 복귀
			*/	
			if(fan_mode == FAN_Mode_Low_Noise)
			{
				cur_temp = m_St.d_R_FPGA_Temp_x10/10; // fpga temp
				
			         if(cur_temp >= 90) fan_mode = FAN_Mode____Normal;
				else if(cur_temp <= 85) fan_mode = FAN_Mode_Low_Noise;
				else                    fan_mode = m_St.FAN_Mode_St; // 2019.09.23 : keep previous status

				if(dbg_on) print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl 2] low-noise, temp %d, last mode %d (0:normal, 1:low)", cur_temp, fan_mode);
			}
			m_St.FAN_Mode_St = fan_mode;
			break;

		case FAN_Mode_OFF:
			m_St.FAN_Mode_St = FAN_Mode_OFF;
			break;
	}
	//----------------------------------------------
	if(m_St.FAN_Auto[EXT_FAN] == ON)
	{	
		if(m_St.FAN_Mode_St == FAN_Mode_Low_Noise)
		{
			fan_rpm = AAU_FAN_LowNoise;
		}
		else if(m_St.FAN_Mode_St == FAN_Mode_OFF) // 2019.06.18
		{
			fan_rpm = AAU_FAN_OFF;
		}
		else // Normal mode
		{
			cur_temp = m_St.d_R_FPGA_Temp_x10/10; // fpga temp

			//D_Printf( DEBUG_SYSTEM, "Max temp %d.\r\n", cur_temp );
				
			     if(cur_temp < m_St.FAN_OffTemp[EXT_FAN]) fan_rpm = AAU_FAN_OFF;
			else if(cur_temp > m_St.FAN_On_Temp[EXT_FAN]) fan_rpm = AAU_FAN_Normal;
			else fan_rpm = prev_fan_rpm;

			if(dbg_on) 
			{
				print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl 3-0, Auto] OFF temp %d, On Temp %d", 
													m_St.FAN_OffTemp[EXT_FAN], m_St.FAN_On_Temp[EXT_FAN]);
			}										
		}

		if(dbg_on) 
			print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl 3-1, Auto] mode %d (0:normal, 1:low), temp %d, rpm %d", 
													m_St.FAN_Mode_St, cur_temp, fan_rpm);
	}
	else // manual ctrl
	{
		if(m_Eep.Ext_FAN_Speed == 0) fan_rpm = AAU_FAN_OFF;
		else if(m_Eep.Ext_FAN_Speed == 50) fan_rpm = 50; // 2019.06.18 : '50' is AAU_FAN_LowNoise
		else fan_rpm = AAU_FAN_Normal;

		if(dbg_on) print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl 4-0, Manual] rpm %d", fan_rpm);
	}
	
	prev_fan_rpm = fan_rpm;
	m_St.AAU_Fan_Speed = fan_rpm;
	
	if(fan_rpm == AAU_FAN_OFF) 
		m_St.FAN_OnOff[EXT_FAN] = OFF;
	else
		m_St.FAN_OnOff[EXT_FAN] = ON;
	
	//----------------------------------------------
	cur_val = m_Reg_Sts.RW_FAN_Control.Data & 0x03;	// 2bits
	
	switch(fan_rpm)
	{		
		case AAU_FAN_LowNoise: 
			req_val = 0x02; // bit_1 High
			break;
			
		case AAU_FAN_Normal:   
			req_val = 0x01; // bit_0 High
			break;

		default : //  AAU_FAN_OFF
			req_val = 0;
			break;			
	}
	
	if(cur_val != req_val)
	{
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_FAN_Control], req_val, "FAN");
	}

	if(dbg_on) print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ AAU_FAN_Ctrl 5-0, Manual] cur %x, req %x", cur_val, req_val);
}


/*
---------------------------------------------------
	2019.04.13
	Get LOS Fail LOF Fail	
---------------------------------------------------
*/	

INT8U Is_CPRI_LOF_Fail(INT8U cpri)
{
	INT8U fa, idx;
	
	switch(cpri)
	{
		case CPRI_0: fa = _100M; idx = 0; break;
		case CPRI_2: fa = _100M; idx = 1; break;
		
		case CPRI_1: fa = Alpha; idx = 0; break;
		case CPRI_3: fa = Alpha; idx = 1; break;
		
		default:
			return 0;
	}

	if(m_St.d_SFP_Insert_St[fa][idx] == UnMount)
		return 0; // if not insert, return normal
		
	#ifdef __TYPE__AAU__ // ANTS-AAU
		return m_Reg_Sts.AXI[cpri].Mscl_Status.LOF_CPRI;
	#else //--------------- ANTS-PRU	
		return A_MMAP_St[cpri].Mscl_Status.LOF_CPRI;
	#endif	
}

INT8U Is_CPRI_LOS_Fail(INT8U cpri)
{
	INT8U fa, idx;

	switch(cpri)
	{
		case CPRI_0: fa = _100M; idx = 0; break;
		case CPRI_2: fa = _100M; idx = 1; break;
		
		case CPRI_1: fa = Alpha; idx = 0; break;
		case CPRI_3: fa = Alpha; idx = 1; break;
		
		default:
			return 0;
	}

	if(m_St.d_SFP_Insert_St[fa][idx] == UnMount)
		return 0; // if not insert, return normal
		
	#ifdef __TYPE__AAU__ // ANTS-AAU
		return m_Reg_Sts.AXI[cpri].Mscl_Status.LOS_CPRI;
	#else //--------------- ANTS-PRU	
		return A_MMAP_St[cpri].Mscl_Status.LOS_CPRI;
	#endif	
}

/*
---------------------------------------------------
	DTU Shutdown
	// 2019.04.13
	---------------------------------------------------
	- Shutdown : by doing "DL/UL Payload Off"
	- Auto recovery : if alarm clear, immediately

	
	1. Clock Fail : 
		Fail, When keeping Fail for 2[s]
		Normal, When keeping Normal for 2[s]
		
	2. CPRI(LOF, LOS) 	
		Fail, When keeping Fail for 2[s]
		Normal, When keeping Normal for 2[s]
		
	3. TSSI Upper-Limit	
		Fail, when TSSI Upper-limit 
		Noraml when hysteresis 2dB
---------------------------------------------------
-	CPRI장애 시, Shutdown 	

 	CPRI#0 장애 2초 유지 시 알람이 발생, 알람이 발생할 경우
		Layer 1~4의  DL100M Path OFF / UL100M Path OFF / Payload100M OFF 동작.
		PAU-AMP-FWD Off ?
	 	UL LNA 제어(X)
	 	
 	CPRI#1 도 위와 동일.
 	CPRI#2, #3의 경우 고려하지 않음.
*/

#define CPRI_SD_Max (3)
/*
index	
	// 0 : cpri_0
	// 1 : cpri_1
	// 2 : clock Fail	   
*/
static INT8U alm_st_prev[CPRI_SD_Max]; // 1 if fail, 0 if normal	
static INT8U cpri_sd_st[CPRI_SD_Max]; // 1 if shutdown, 0 if normal
static OS_TIMER chk_Timer[CPRI_SD_Max];


static INT8U f_DTU_Path_OnOff_prev[MAX_FA_PATH][DL_UL][MAX_MIMO_PATH]; // On(0), OFF(1)

enum
{
	DTU_SD_By_User = 0x80,
	DTU_SD_By_DIG = 0x40, // (== By CPRI_Unlock)
	DTU_SD_By_AMP = 0x20,
	DTU_SD_By_Standby = 0x10, // AMP-Standby, MIMO Standby, Batt.Extension
	DTU_SD_By_AMP_Not_Use = 0x08,
	DTU_SD_BY_AAU_FAN_TEMP = 0x04,
	DTU_SD_BY_RU_NOT_Init  = 0x02, // RU Initial Fail
	DTU_SD_By_AMP_Reason   = 0x01,
};




void Proc___DTU_SD__Check(void) // flag 만 Setting
{
	static INT8U proc_start = 0; // 1 if started
	INT8U cur_alm;
	INT8U i;
	INT8U fa, m;
	INT8U pau_amp_fwd_sd[MAX_PATH_4];
	//static INT8U disp_off_cnt = 0;
	
	
	// shutdown of 4-path of [100M/Alpha]
	INT8U path_sd[MAX_FA_PATH][MAX_MIMO_PATH];
	INT8U clock_sd[MAX_FA_PATH][MAX_MIMO_PATH];
	INT8U tssi_sd[MAX_FA_PATH][MAX_MIMO_PATH];

	// init ON (1 : Path ON)
	memset(path_sd, ON, 2*4); 
	memset(clock_sd, ON, 2*4); 	
	memset(tssi_sd, ON, 2*4); 

	//---------------------------
	if(m_St.AlmTest)
		return;
	//---------------------------	
	
	//----------------------------------------	
	if(proc_start == 0)
	{
		proc_start = 1;
		for(i=0; i<CPRI_SD_Max; i++)
		{
			alm_st_prev[i] = 0; 
			cpri_sd_st[i] = 0; 
			
			OSTimerReset(&chk_Timer[i]);
		}

		Set_Factory_Mode_Onoff(OFF); // 2019.08.06, default OFF	
		
		memset(f_DTU_Path_OnOff, 0, 2*2*4); // init zero
		
		return;
	}


/*
CPRI Alm : CPRI#2~#3 장애시 위 알고리즘 적용하지 않음. (Cascade 측 적용X)
-	CPRI#0 장애 발생 2초 유지 시 Alm 발생 하고 아래 동작 수행
	(DL Path OFF  /  UL Path OFF  / Payload OFF)

-	CPRI Alarm 발생시 PAU Shutdown 수행 : 선수 조건 1: CPRI 0, 1의 SFP mount 여부 확인

* SFP-0 설치 SFP-1 미설치 / SFP-0 미설치 SFP-1 설치
 : DL,UL, Payload Off, PAU Shutdown 
 
* SFP-0 설치 && SFP-1 설치
 : CPRI0,1 의 한 Port만 알람 발생시, 해당 패스의 DL,UL Payload off, PAU Shutdown 미수행
 : CPRI0,1 의 모든 Port만 알람 발생시, 해당 패스의 DL,UL Payload off, PAU Shutdown

*/
	//----------------------------------------
	// 2019.06.19 : (PRU) Clock Alarm 발생 시 Shutdown 알고리즘 수행.	
	for(i=0; i<CPRI_SD_Max; i++)
	{
		switch(i)
		{
			case 0 : cur_alm = m_St.AlmSt.A_2.CPRI_0__; break; // 100M,  if SFP-not installed, alarm clear.
			case 1 : cur_alm = m_St.AlmSt.A_2.CPRI_0_a; break; // Alpha, if SFP-not installed, alarm clear.			
			case 2 : cur_alm = m_St.AlmSt.A_4.Clock;   break;
		}
		//----------------------------------------	
		if(alm_st_prev[i] != cur_alm)
		{
			OSTimerReset(&chk_Timer[i]);
		}
		else if(cur_alm)
		{		
			if(OSTimerGet_msec(&chk_Timer[i]) >= 2000) // for 2[s]
			{
				cpri_sd_st[i] = 1; // Shutdown Req
				OSTimerReset(&chk_Timer[i]);
			}
			// else : keep previous
		}	
		else // now normal status
		{
			if(OSTimerGet_msec(&chk_Timer[i]) >= 2000) // for 2[s]		
			{
				cpri_sd_st[i] = 0; // Shutdown Not
				OSTimerReset(&chk_Timer[i]);
			}
			// else : keep previous
		}	
		alm_st_prev[i] = cur_alm;
	}

	/*
	---------------------------------------------------
	* 2019.08.06 : Factory mode @ AAU
	* 2019.08.26 : Factory mode @ PRU
	---------------------------------------------------
	*/	
	if(Get_Factory_Mode() == ON)
	{
		for(i=0; i<CPRI_SD_Max; i++)
		{
			cpri_sd_st[i] = 0;
			alm_st_prev[i] = 0;
		}
	}	
	
	//----------------------------------------
	// Check Clock Fail
	//----------------------------------------		
	// 2019.06.28 : clock fail 일때, payload off(not only AAU but also PRU)
	if(cpri_sd_st[2]) // if clock fail
	{		
		memset(clock_sd, OFF, 2*4); // shutdown : 'OFF' is Path-Off, MAX_FA_PATH*MAX_MIMO_PATH
	}	


	/*	
	2019.07.13
		BW 100+a에서 100Mhz 로 변경시 alpha의 Payload Off하도록 기능 추가
		(반대로 100Mhz -> 100+a로 변경될 경우에는 Payload 자동 On)
	*/
	if(IS_Alpha_Mhz_Existing()==0)
	{
		for(m=0; m < MAX_MIMO_PATH; m++)
			path_sd[FA_1][m] = OFF;
	}

		
	//----------------------------------------
	// TSSI Upper Limit
	//----------------------------------------	
	for(m=0; m < MAX_MIMO_PATH; m++)
	{
		// 2020.01.10 : added
		if(IS__TSSI_Not_Checking(m)) 
		{
			if(m_St.Last_Alm.Path[m].DL.TSSI_Upper__) // 100M
				tssi_sd[FA_0][m] = OFF;

			if(m_St.Last_Alm.Path[m].DL.TSSI_Upper_a) // alpha
				tssi_sd[FA_1][m] = OFF;
			continue;
		}
		
		if(m_St.AlmSt.Path[m].DL.TSSI_Upper__) // 100M
		{
			tssi_sd[FA_0][m] = OFF;
		}

		if(m_St.AlmSt.Path[m].DL.TSSI_Upper_a) // alpha
		{
			tssi_sd[FA_1][m] = OFF;
		}
	}	
	//----------------------------------------	
	
	
		
	/*
	----------------------------------------
	2019.04.25.
	* SFP-0 설치 SFP-1 미설치 / SFP-0 미설치 SFP-1 설치
		 : DL,UL, Payload Off, PAU Shutdown 
 
	* SFP-0 설치 && SFP-1 설치
		 : CPRI0,1 의 한 Port만 알람 발생시, 해당 패스의 DL,UL Payload off, PAU Shutdown 미수행
		 : CPRI0,1 의 모든 Port만 알람 발생시, 해당 패스의 DL,UL Payload off, PAU Shutdown
	----------------------------------------
	*/
	if(Is_SFP_Inserted(CPRI_0)==0) {cpri_sd_st[CPRI_0] = 0; OSTimerReset(&chk_Timer[0]);}
	if(Is_SFP_Inserted(CPRI_1)==0) {cpri_sd_st[CPRI_1] = 0; OSTimerReset(&chk_Timer[1]);}

	//--------------------------------------2020.06.22
	INT8U is_100Mh_exist = 0;
	INT8U is_alpha_exist = 0;
	INT8U amp_common_off = 0;

	     if(Is_SFP_Inserted(CPRI_1)==0) is_alpha_exist = 0;
	else if(IS_Alpha_Mhz_Existing()==0) is_alpha_exist = 0;
	else is_alpha_exist = 1;
	
	if(Is_SFP_Inserted(CPRI_0) == 0)  is_100Mh_exist = 0;
	else is_100Mh_exist = 1;

	if(is_100Mh_exist && is_alpha_exist)
	{
		if(cpri_sd_st[CPRI_0] && cpri_sd_st[CPRI_1]) 
			amp_common_off = 1;		
		else amp_common_off = 0;
	}
	else if(is_100Mh_exist) // 100 only
	{		
		if(cpri_sd_st[CPRI_0]) amp_common_off = 1;
		else amp_common_off = 0;
	}
	else if(is_alpha_exist) // alpha only
	{
		if(cpri_sd_st[CPRI_1]) amp_common_off = 1;
		else amp_common_off = 0;
	}
	else amp_common_off = 1;
	//--------------------------------------

	INT8U amp_sd_req[2] = {0,0}; // CPRI0/1
	INT8U common_cpri_fail = 0; // 2020.06.22
	if(Is_SFP_Inserted(CPRI_0) && Is_SFP_Inserted(CPRI_1)) 
	{
		if(cpri_sd_st[CPRI_0] && cpri_sd_st[CPRI_1])
		{			
			amp_sd_req[CPRI_0] = 1; // PAU Shutdown
			amp_sd_req[CPRI_1] = 1; // PAU Shutdown
			common_cpri_fail = 1; // 2020.06.22
		}
	}
	else if(Is_SFP_Inserted(CPRI_0)&&(Is_SFP_Inserted(CPRI_1)==0)) 
	{
		if(cpri_sd_st[CPRI_0])
		{
			amp_sd_req[CPRI_0] = 1; // PAU Shutdown
			common_cpri_fail = 1; // 2020.06.22
		}
	}
	else if(Is_SFP_Inserted(CPRI_1)&&(Is_SFP_Inserted(CPRI_0)==0)) 
	{
		if(cpri_sd_st[CPRI_1])
		{
			amp_sd_req[CPRI_1] = 1; // PAU Shutdown
			common_cpri_fail = 1; // 2020.06.22
		}
	}

	//----------------------------------------
	// Path Onoff Flag Set by DTU
	//----------------------------------------
	INT8U off_req;
	
	m_Env.Rvs_Spur_Chk = 0; // 2020.01.28
	
	for(fa=0; fa < MAX_FA_PATH; fa++)
	{
		for(m=0; m < MAX_MIMO_PATH; m++)	
		{
			//------------------------------------
			// 2019.12.20
			//------------------------------------			
			off_req = 0; // default	
						
			if(amp_sd_req[fa] || (path_sd[fa][m] == OFF)) off_req = 1;
			/*
			// 2020.06.22 삭제. : off_req 에 의해, Set__Reason_SD_5115(m, Rsn_SD_CPRI_Fail) 호출되므로 삭제. 
			all cpri_sd_st[0], cpri_sd_st[1] 이 shutdown이면 호출이 맞음.
			*/
			if(cpri_sd_st[fa])  off_req = 1;
			if(tssi_sd[fa][m] == OFF) off_req = 1;
			if(clock_sd[fa][m] == OFF) off_req = 1;
			
			if(Is_RVS_Spurious(fa, m))
			{
				if(off_req == 0) 
					m_Env.Rvs_Spur_Chk |= cbits[fa*MAX_MIMO_PATH + m];

				off_req = 1;	
			}
			//------------------------------------			
			if(off_req) // common off
			{
				     if(common_cpri_fail)       Set__Reason_SD_5115(m, Rsn_SD_CPRI_Fail); // 2020.06.22 : 'cpri_sd_st[fa]' -> 'common_cpri_fail'
				else if(clock_sd[fa][m] == OFF) Set__Reason_SD_5115(m, Rsn_SD_CLock_Alm); // 2019.12.20
				else if(tssi_sd[fa][m] == OFF)  Set__Reason_SD_5115(m, Rsn_SD_TSSI_Over); // 2019.12.20
				/*
				// 2020.01.28 : RVS Spurious 로는 payload만 off 하도록 수정.
				////else if(Is_RVS_Spurious(fa, m)) Set__Reason_SD_5115(m, Rsn_SD____by_UDA); // 2019.12.20
				*/
				/*
				alpha의 path_sd 부분의 경우, reason 설정 없음.
				*/				
				
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_DIG; // amp shutdown by cpri alarm
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_DIG; // amp shutdown by cpri alarm
				
				/*
				path_sd[fa][m] == OFF 인 경우, 아래 해당 안되므로, AMP_SD 안됨.(정상)
				*/
				//if(m_St.Ref_CPRI_No == fa) // if curren fa is reference-cpri		
				if(amp_common_off) // 2020.06.22
				{
					// 2020.01.28 : RVS Spurious 로는 payload만 off 하도록 수정.
					if(Is_RVS_Spurious(fa, m)==0)
					{
						Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
						Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
					}
				}	
			}
			else
			{
				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_DIG; // clear
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_DIG; // clear
			}
			
			#ifdef __TYPE__AAU__
			/*
			2019.08.01 : FAN & Temp Alarm : payload off, all-amp-off
			*/
			if(m_Env.AAU_Alarm_Fan_and_Temp)
			{
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_BY_AAU_FAN_TEMP;
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_BY_AAU_FAN_TEMP;
			}
			else
			{
				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_BY_AAU_FAN_TEMP; // clear
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_BY_AAU_FAN_TEMP; // clear
			}
			#endif
		}
	}
	m_Env.Rvs_Spur_OFF = m_Env.Rvs_Spur_Chk; // 2020.01.28
	//---------------------------------------------------------	
	// 2019.08.22.
	for(fa=0; fa < MAX_FA_PATH; fa++)
	{
		for(m=0; m < MAX_MIMO_PATH; m++)	
		{
			if(m_Env.AMP_Ctrl_Allowed == 0) // 2019.09.24
			{
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_BY_RU_NOT_Init;
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_BY_RU_NOT_Init;
			}
			else if(IS_RU_Init_Completed() || IS_User_AMP_ON_Control(m))
			{
				if(f_DTU_Path_OnOff[fa][DL][m] & DTU_SD_BY_RU_NOT_Init)
				{
					f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_BY_RU_NOT_Init;
					
					// 2019.12.19
					f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_AMP;
					
					// 2020.01.29 : delete below
					/*
					Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_CLR );
					Alarm_SD_Fwd(m);
					*/
				}
				if(f_DTU_Path_OnOff[fa][UL][m] & DTU_SD_BY_RU_NOT_Init)
				{				
					f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_BY_RU_NOT_Init;

					// 2019.12.19
					f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_AMP;

					// 2020.01.29 : delete below
					/*
					Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_CLR );
					Alarm_SD_Rev(m);
					*/
				}
			}
		}
	}

	//---------------------------------------------------------
	for(fa=0; fa < MAX_FA_PATH; fa++)
	{
		for(m=0; m < MAX_MIMO_PATH; m++)	
		{	
			//----------------------------------------------------
			if(Check_PAU_Use(m)==0)
			{
			/*
			* 2019.08.01
			* amp-not-use amp off, payload off
			*/					
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_AMP_Not_Use;
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_AMP_Not_Use;

				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_User;
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_User;
			}
			else
			{
				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_AMP_Not_Use;
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_AMP_Not_Use;
				
				if(m_Eep.d_Path_OnOff_DL[fa][m] == OFF) 
					f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_User;
			#ifdef __TYPE__AAU__
				else if(m_Eep.Pwr_Amp_En[m] == OFF) 
					f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_User;
			#endif	
				else
					f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_User;
				//----------------------------------------------------
				if(m_Eep.d_Path_OnOff_UL[fa][m] == OFF) 
					f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_User;
			#ifdef __TYPE__AAU__	
				else if(m_Eep.RVS_Amp_Onoff[m] == OFF) 
					f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_User;
			#endif
				else
					f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_User;
			}
		}
	}

	
	// 2019.04.25
	for(m=0; m < MAX_MIMO_PATH; m++)	
	{
		pau_amp_fwd_sd[m] = IS_PAU_TX_RX_All_Off(m);
	}		
	
	//----------------------------------------
	// Path Onoff Flag Set by PAU(PRU)/RFU(AAU)
	//----------------------------------------			
	for(fa=0; fa < MAX_FA_PATH; fa++)
	{
		for(m=0; m < MAX_MIMO_PATH; m++)	
		{			
			if(Fwd_Amp_Off_rsn[m] || Rvs_Amp_Off_rsn[m])
			{
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_AMP;
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_AMP;
			}			
			else if(pau_amp_fwd_sd[m])
			{
				#ifdef __TYPE__PRU__
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_AMP_Reason;
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_AMP_Reason;
				#else
				if(m_St.Pwr_Amp_Off_Reason_PAU[m] == rsn_Over_Pwr) // fwd shutdown
				{
					f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_AMP_Reason; // 2019.09.11
					f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_AMP_Reason;
				}
				#endif				
			}
			else	
			{				
				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_AMP;
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_AMP;

				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_AMP_Reason;
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_AMP_Reason;				
			}
			//--------------------------------------------------
			if(StdBy_Off_rsn[m])
			{
				f_DTU_Path_OnOff[fa][DL][m] |= DTU_SD_By_Standby;
				f_DTU_Path_OnOff[fa][UL][m] |= DTU_SD_By_Standby;
			}
			else
			{
				f_DTU_Path_OnOff[fa][DL][m] &= ~DTU_SD_By_Standby;
				f_DTU_Path_OnOff[fa][UL][m] &= ~DTU_SD_By_Standby;
			}
		}
	}
	//----------------------------------------	
	
	for(int cnt = 0, fa=0; fa < MAX_FA_PATH; fa++)
	{		
		for(m=0; m < MAX_MIMO_PATH; m++)	
		{					
			if(f_DTU_Path_OnOff_prev[fa][DL][m] != f_DTU_Path_OnOff[fa][DL][m])
			{	
				cnt++;
			#ifdef __disable_frequent_debug__	
			#else
				print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[ DL Payload_Off ] --------------");
			#endif	
				break;
			}
		}
		if(cnt) break;
	}
	//----------------------------------------
	#ifdef __disable_frequent_debug__	
	#else
	for(fa=0; fa < MAX_FA_PATH; fa++)
	{		
		for(m=0; m < MAX_MIMO_PATH; m++)	
		{					
			if(f_DTU_Path_OnOff_prev[fa][DL][m] != f_DTU_Path_OnOff[fa][DL][m])
			{	
				if(f_DTU_Path_OnOff[fa][DL][m])
				{
					print_agent( DEBUG_STDBY, VT_RED  , "[%d][%d] %#02x -> %#02x", 
						fa, m, f_DTU_Path_OnOff_prev[fa][DL][m], f_DTU_Path_OnOff[fa][DL][m]);
				}
				else
				{
					print_agent( DEBUG_STDBY, VT_GREEN, "[%d][%d] %#02x -> %#02x", 
						fa, m, f_DTU_Path_OnOff_prev[fa][DL][m], f_DTU_Path_OnOff[fa][DL][m]);
				}
				
				//print_agent( DEBUG_STDBY, VT_BOLD_GREEN, " m_St.Ref_CPRI_No : %d", m_St.Ref_CPRI_No);

				f_DTU_Path_OnOff_prev[fa][DL][m] = f_DTU_Path_OnOff[fa][DL][m];	
			}
		}
	}
	#endif
}






/*
-------------------------------------------------
- DTU Path onoff : 100M/Alpha - DL/UL - 4layer
-------------------------------------------------
*/

void Proc___Shutdown_Ctrl(void)
{
	INT8U fa, m;	
	INT8U onoff_req;
	INT8U onoff_cur;

	//---------------------------
	if(m_St.AlmTest)
		return;
	//---------------------------	
	
	//--------------------------------------
	INT8U is_100Mh_exist = 0;
	INT8U is_alpha_exist = 0;
	INT8U amp_common_off = 0;

	     if(Is_SFP_Inserted(CPRI_1)==0) is_alpha_exist = 0;
	else if(IS_Alpha_Mhz_Existing()==0) is_alpha_exist = 0;
	else is_alpha_exist = 1;
	
	if(Is_SFP_Inserted(CPRI_0) == 0)  is_100Mh_exist = 0;
	else is_100Mh_exist = 1;

	if(is_100Mh_exist && is_alpha_exist)
	{
		if(cpri_sd_st[CPRI_0] && cpri_sd_st[CPRI_1]) 
			amp_common_off = 1;
		else amp_common_off = 0;
	}
	else if(is_100Mh_exist) // 100 only
	{		
		if(cpri_sd_st[CPRI_0]) amp_common_off = 1;
		else amp_common_off = 0;
	}
	else if(is_alpha_exist) // alpha only
	{
		if(cpri_sd_st[CPRI_1]) amp_common_off = 1;
		else amp_common_off = 0;
	}
	else amp_common_off = 1;
	//--------------------------------------
	

	for(fa=0; fa < MAX_FA_PATH; fa++)
	{
		for(m=0; m < MAX_MIMO_PATH; m++)
		{	
			//------------------------------
			// DL
			//------------------------------
			if(f_DTU_Path_OnOff[fa][DL][m]) onoff_req = OFF;
			else onoff_req = ON;
			
			onoff_cur = m_St.d_Path_OnOff_DL[fa][m];
			
			if(onoff_req != onoff_cur)
			{
				DTU_DL_Path_Onoff(fa, m, onoff_req); // CHECK

				//if(onoff_req == ON) D_Printf( DEBUG_STDBY, "[RCU] DTU DL Path[%d][%d] On Ctrl.\n", fa, m);
				//else                D_Printf( DEBUG_STDBY, "[RCU] DTU DL Path[%d][%d] Off Ctrl.\n", fa, m);
			}
				
			if(f_DTU_Path_OnOff[fa][DL][m] & DTU_SD_By_AMP_Not_Use) // 2019.08.01
			{
				Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
			}
			else if(f_DTU_Path_OnOff[fa][DL][m] & DTU_SD_BY_RU_NOT_Init) // 2019.08.22
			{
				Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
			}
			else if(f_DTU_Path_OnOff[fa][DL][m] & DTU_SD_By_DIG)
			{
				////if(m_St.Ref_CPRI_No == fa) // if curren fa is reference-cpri		
				if(amp_common_off) // 2020.06.22
				{
				#if 1 // 2020.01.28 : RVS Spurious 로는 payload만 off 하도록 수정.
					if(Is_RVS_Spur_Only_Off(fa, m))
						Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_CLR );
					else
						Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
				#else
				Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
				#endif
				}
			}
			else
			{
				Fwd_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_CLR );
			}			
			
			
			//------------------------------
			// UL 
			//------------------------------
			if(f_DTU_Path_OnOff[fa][UL][m]) onoff_req = OFF;
			else onoff_req = ON;			
			
			onoff_cur = m_St.d_Path_OnOff_UL[fa][m];
			
			if(onoff_req != onoff_cur)
			{
				DTU_UL_Path_Onoff(fa, m, onoff_req);

				//if(onoff_req == ON) D_Printf( DEBUG_STDBY, "[RCU] DTU UL Path[%d][%d] On Ctrl.\n", fa, m);
				//else                D_Printf( DEBUG_STDBY, "[RCU] DTU UL Path[%d][%d] Off Ctrl.\n", fa, m);
			}

			
			if(f_DTU_Path_OnOff[fa][UL][m] & DTU_SD_By_DIG)
			{
				////if(m_St.Ref_CPRI_No == fa) // if curren fa is reference-cpri
				if(amp_common_off) // 2020.06.22
				{
				#if 1 // 2020.01.28 : RVS Spurious 로는 payload만 off 하도록 수정.
					if(Is_RVS_Spur_Only_Off(fa, m))
						Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_CLR );
					else
						Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
				#else
					Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
				#endif
				}	
			}
			else if(f_DTU_Path_OnOff[fa][UL][m] & DTU_SD_BY_RU_NOT_Init) // 2019.08.22
			{
				Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_SET );
			}
			else 
			{
				Rvs_AMP_SD_f_Ctrl( m, AMP_SD_DIG, f_CLR );
			}
			
		}
	}	
}


// 2019.04.08
#ifdef __TYPE__PRU__	
void PRU_Reg_LED_Red(INT8U onoff)
{
	m_Reg_Sts.RW_DTU_LED.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_DTU_LED] );
	m_Reg_Ctl.RW_DTU_LED.Data = m_Reg_Sts.RW_DTU_LED.Data;
	
	m_Reg_Flg.RW_DTU_LED.Red = _SET_;
	m_Reg_Ctl.RW_DTU_LED.Red = onoff;

	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_DTU_LED], m_Reg_Ctl.RW_DTU_LED.Data, "PRU LED");
}
void PRU_Reg_LED_Green(INT8U onoff)
{
	m_Reg_Sts.RW_DTU_LED.Data = Read_FPGA_Reg( Reg_Common_Addr[Reg_Common_RW_DTU_LED] );
	m_Reg_Ctl.RW_DTU_LED.Data = m_Reg_Sts.RW_DTU_LED.Data;
	
	m_Reg_Flg.RW_DTU_LED.Green = _SET_;
	m_Reg_Ctl.RW_DTU_LED.Green = onoff;

	Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_DTU_LED], m_Reg_Ctl.RW_DTU_LED.Data, "PRU LED");
}
#endif


#ifdef __TYPE__AAU__	
/*
----------------------------------------------------------
	Cascade ID | 0x2C	| 0x2D
	-----------+-------+-----
	0	       | 0x15B | 0xD0
	-----------+-------+-----
	1,2,3	   | 0x15B | 0xD4
	-----------+-------+-----
0x2D의 31bit가 '0' 인 경우에만 write가 가능 ; Write 전 Read 하여 31번트를 체크 
----------------------------------------------------------
*/
void Proc___AAU_Reg_PLL(void)
{
	INT32U data32;
	INT8U  req_val;
	INT8U  cur_val;
	
	INT32U  req_val32;
	INT32U  cur_val32;
 	INT16U  addr16;
 	INT8U   rec, id;
	

	//-----------------------------------------------
	// for debugging
	//-----------------------------------------------	
	if(m_Env.debug_data[dbg___AAU_PLL_Test][dbg_ctrl][dbg_en])
	{
		addr16 = dbg___AAU_PLL_Test;
		// address		
		cur_val32 = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Addr]);
		req_val32 = m_Env.debug_data[addr16][dbg_ctrl][dbg_d0] & 0x7FFFFFFF;

		m_Env.debug_data[addr16][dbg_data][0] = cur_val32;

		if(req_val32 != cur_val32)			
			Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Addr], req_val32, "PLL Addr");
		

		// data
		cur_val32 = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Data]);
		req_val32 = m_Env.debug_data[addr16][dbg_ctrl][dbg_d1] & 0x7FFFFFFF;
		
		m_Env.debug_data[addr16][dbg_data][1] = cur_val32 & 0x7FFFFFFF;

		if((cur_val32 & 0x80000000)==0) // if write permitted (when MSB(bit31) is '0')
		{
			if(req_val32 != cur_val32)
				Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Data], req_val32, "PLL Data");
		}
		return;
	}
	//-----------------------------------------------	
	id = AufoConfigFlag.RE_Cascade_ID;
	rec = Get_REC_Idx( AufoConfigFlag.RECTypeID );
	if((id < 4)&&(rec < LSH_Max))
	{
		/*
		2020.04.02 : Use AID 0xC090~92
		-----------------------------------
		////// if(AufoConfigFlag.RE_Cascade_ID == 0)
		//////	 req_val = 0xD0;
		////// else
		//////	 req_val = 0xD4;
		-----------------------------------	
		*/
		req_val = m_Eep.d_Reg_PLL_Set[rec][id];
		//-----------------------------------------------------------	
		
		data32 = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Data]);
		cur_val = (INT8U)data32;

		if(req_val != cur_val)
		{
			if((data32 & 0x80000000)==0) // if write permitted (when MSB(bit31) is '0')
			{		
				Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Addr], 0x15B, "PLL Addr");
				Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_PLL_Data], req_val, "PLL Data");
			}
		}
	}
}
#endif


// 2019.05.17
void VSS_UL_RE_Reset_ACK_Sw_Default(void)
{
	m_Reg_Flg.RW_RE_Reset_ACK_SW.Data = _F_SET_; 
	m_Reg_Ctl.RW_RE_Reset_ACK_SW.Data = 0; // 0 : FPGA Bypass
}

// 2019.05.17
/*
bypass : FPGA bypass
cpu access : enable cpu-setting-value
*/

void VSS_UL_RE_Reset_ACK_CPU_Access(INT8U optic, INT8U c_id)
{	
	INT32U req___ack_sw;
	INT32U curr__ack_sw;
	
	if(optic >= MAX_Optic) return;
	if(c_id >= 4) return;

	curr__ack_sw = m_Reg_Sts.RW_RE_Reset_ACK_SW.Data;
	req___ack_sw = 0; // 0 : FPGA Bypass default

	if(optic == _100M)
	{	
		if(c_id == 0 ) req___ack_sw |= cbits16[0];
		req___ack_sw |= cbits16[c_id+1];

		// 2019.07.02 Added.
		if(c_id == 0 ) req___ack_sw &= nbits16[8];
		req___ack_sw &= nbits16[c_id+9];
	}
	else // Alpha
	{			
		if(c_id == 0 ) req___ack_sw |= cbits16[8];
		req___ack_sw |= cbits16[c_id+9];

		// 2019.07.02 Added.
		if(c_id == 0 ) req___ack_sw &= nbits16[0];
		req___ack_sw &= nbits16[c_id+1];
	}

	if(curr__ack_sw != req___ack_sw)
	{
		m_Reg_Flg.RW_RE_Reset_ACK_SW.Data = _F_SET_;
		m_Reg_Ctl.RW_RE_Reset_ACK_SW.Data = req___ack_sw;

		if(optic == _100M)
			printf(VT_BLUE "[RCU] VSS-UL-RE-Reset_ACK_SW Changed : C-ID (0x%X) /100M\n" VT_END_CR, c_id);
		else
			printf(VT_BLUE "[RCU] VSS-UL-RE-Reset_ACK_SW Changed : C-ID (0x%X) /Alpha\n" VT_END_CR, c_id);
	}
}

void Proc___VSS_UL_RE_Reset_ACK_Switch(void)
{
	if(m_St.Ref_CPRI_No == CPRI_0)
		VSS_UL_RE_Reset_ACK_CPU_Access(_100M, m_St.d_Cascade_ID[CPRI_0]);
	else
		VSS_UL_RE_Reset_ACK_CPU_Access(Alpha, m_St.d_Cascade_ID[CPRI_1]);		
}




// 2019.04.08
/*
---------------------------------------------------
	DTU LED Display (ANTS-PRU)
	---------------------------------------------------
	when booting : default  all ON
	when App start : all Off		
		DTU Internal Shutdown : GREEN-OFF, RED-ON
		DTU Internal Alarm    : GREEN-OFF, RED-Blink(500ms)
		Normal : GREEN-ON, RED-OFF


	AAU LED Display (ANTS-AAU)		
	-----------------------+---------
					 RED   |    Blue
	-----------------------+---------
	* GUI Connet       X   |    Blink (No alarm & GUI Polling)
	-----------------------+---------
	* Shutdown 으로 
	  인한 알람        ON  |    OFF
	-----------------------+---------
	* 기타 alarm       ON  |    ON    (Purple)
	-----------------------|---------
	FAN 알람
	온도 상한
	TSSI 하한/RSSI 하한
	UDA 불요파/CPRI/Clock fail
	Disabled
	Gain 저하(UL)
	출력 상한/하한
	RSSI 하한/TSSI 하한
	-----------------------+---------
	* Normal          OFF  |   ON
	-----------------------+---------


---------------------------------------------------
2019.04.18.
 	when SFP is UnMount, do Masking.
 	CPRI#0~#3 Alarm ORing and LED Display

*/	

void Proc____PRU_LED(void)
{
//------------------------------------------------------
#ifdef __TYPE__PRU__	
//------------------------------------------------------

	static INT8U toggle = 0;
	static OS_TIMER PRU_Reg_LED_Timer;
	static INT8U init = 0;
	
	INT8U RED_LED_ON = 0;
	INT8U RED_LED_Blink = 0;
	INT8U path;
	//INT8U cpri;

	if(init == 0)
	{
		init = 1;
		m_St.ANTS_LED_Status = ANTS_LED_Max; // init value
	}
	
	//-----------------------------------------
	// every 500ms
	if(OSTimerGet_msec(&PRU_Reg_LED_Timer) >= 500)
	{
		OSTimerReset(&PRU_Reg_LED_Timer);
		toggle ^= 1;
	}
	
	// Check if RED ON or not -----------------------------------------	
	if(m_St.AlmSt.A_4.Clock) RED_LED_ON = 1; // clock fail		
	else
	{
		RED_LED_ON = 0; // init 0

		// cpri alarm, 2019.04.25
		if(m_St.AlmSt.A_2.CPRI_0__) RED_LED_ON = 1; 
		if(m_St.AlmSt.A_2.CPRI_0_a) RED_LED_ON = 1;

		if(m_St.AlmSt.A_2.LOS_SFP_0__) RED_LED_ON = 1; 
		if(m_St.AlmSt.A_2.LOS_SFP_0_a) RED_LED_ON = 1; 	
		
		if(m_St.AlmSt.A_3.LD_SFP_0__) RED_LED_ON = 1; 	 	
		if(m_St.AlmSt.A_3.LD_SFP_0_a) RED_LED_ON = 1; 

		if(m_St.AlmSt.A_3.SFP_Ins_0_a)  RED_LED_ON = 1;
		if(m_St.AlmSt.A_3.SFP_Ins_0__)  RED_LED_ON = 1;
		
		for(path=0; path < MAX_MIMO_PATH; path++)
		{
			// cpri alarm
			if(Check_PAU_Use(path))
			{	
				if(m_St.AlmSt.Path[path].DL.TSSI_Upper__) RED_LED_ON = 1; 
				if(m_St.AlmSt.Path[path].DL.TSSI_Upper_a) RED_LED_ON = 1;
			}
			if(RED_LED_ON == 1) break;
		}
		
		
	}
	// Check if RED Blink or not -----------------------------------------
	/*
	RED-LED Blick every 500[ms]
	--------------------------
		온도 상한 / 상태 감시 접속 / Door		
		TSSI 하한 / RSSI 하한     / PAU Link Fail
		UDA 불요파/ UDA PD 알람
	--------------------------	
	*/
	if(RED_LED_ON == 0)
	{
		if(m_St.AlmSt.A_0.Temp_Up) RED_LED_Blink = 1;
		/*
		// 2019.04.17 : GUI, Door는 제외됨.
		////else if(m_St.AlmSt.A_0.GUI)     RED_LED_Blink = 1;
		////else if(m_St.AlmSt.A_0.Door)    RED_LED_Blink = 1;
		*/
		else
		{
			RED_LED_Blink = 0; // init 0
			
			for(path=0; path < MAX_PATH_4; path++)
			{
				if(Check_PAU_Use(path))
				{
					if(m_St.AlmSt.Path[path].PAU.Link_Fail)   RED_LED_Blink = 1;
					if(m_St.AlmSt.Path[path].DL.TSSI_Lower__) RED_LED_Blink = 1;
					if(m_St.AlmSt.Path[path].DL.TSSI_Lower_a) RED_LED_Blink = 1;
					if(m_St.AlmSt.Path[path].DL.Out_Lower)    RED_LED_Blink = 1;
					if(m_St.AlmSt.Path[path].DL.Out_Upper)    RED_LED_Blink = 1;
				}
			}

			if(m_St.AlmSt.UDA.PD_CPRI)  RED_LED_Blink = 1;
			if(m_St.AlmSt.UDA.Spurious) RED_LED_Blink = 1;
			if(m_St.AlmSt.UDA.Clock)    RED_LED_Blink = 1; // 2019.09.04
			if(m_St.AlmSt.UDA.CPRI_RST_Suppr) RED_LED_Blink = 1; // 2019.09.04
			if(m_St.AlmSt.UDA.Clk__RST_Suppr) RED_LED_Blink = 1; // 2019.09.04
			if(m_St.AlmSt.ETC.SuperCap_Exch)  RED_LED_Blink = 1; // 2019.09.04
			if(m_St.AlmSt.ETC.Mismatch_ID)    RED_LED_Blink = 1; // 2019.09.04

			// cpri alarm, 2019.04.25
			if(m_St.AlmSt.A_2.CPRI_1__) RED_LED_Blink = 1; 
			if(m_St.AlmSt.A_2.CPRI_1_a) RED_LED_Blink = 1;

			if(m_St.AlmSt.A_2.LOS_SFP_1_a) RED_LED_Blink = 1;
			if(m_St.AlmSt.A_2.LOS_SFP_1__) RED_LED_Blink = 1;
			if(m_St.AlmSt.A_3.LD_SFP_1_a)  RED_LED_Blink = 1;
			if(m_St.AlmSt.A_3.LD_SFP_1__)  RED_LED_Blink = 1;
		}
	}
	
	// Control LED -----------------------------------------
	if(RED_LED_ON)
	{		
		PRU_Reg_LED_Green(_Reg_LED_OFF);
		PRU_Reg_LED_Red(_Reg_LED__ON);

		m_St.ANTS_LED_Status = ANTS_LED_R___ON;
	}
	else if(RED_LED_Blink)
	{		
		PRU_Reg_LED_Green(_Reg_LED_OFF);
		PRU_Reg_LED_Red(toggle);

		m_St.ANTS_LED_Status = ANTS_LED_R_Blnk;
	}
	else // normal
	{
		PRU_Reg_LED_Green(_Reg_LED__ON);		
		PRU_Reg_LED_Red(_Reg_LED_OFF);		

		m_St.ANTS_LED_Status = ANTS_LED_B___ON;
	}
//------------------------------------------------------
#endif	
}

//-----------------------------------------























#ifdef __TYPE__AAU__
static INT8U AAU_Polling = 0x00;
#endif

void AAU_Polling_Aivie(void)
{
#ifdef __TYPE__AAU__
	AAU_Polling++;
#endif
}

INT8U Is_AAU_Polling_Now(void)
{
#ifdef __TYPE__AAU__
	static INT8U Prev = 0;
	static INT8U cnt = 0;

	if(Prev != AAU_Polling)
	{
		Prev = AAU_Polling;
		cnt = 0;
		return 1;
	}
	else
	{
		cnt++;
		if(cnt >= 10)
		{
			cnt = 10;
			return 0;
		}
		else
		{
			return 1;
		}
	}	
#else
	return 0;
#endif
}



/*
-------------------------------------------------------------
	2019.08.21
	AAU only
	LED 자동 Off 0x5455 : Default: ON,  Off 시간: 5분

	2019.08.30 : 	
	LED 자동 Off/On 상태에서 상태 Polling시에는 이전과 동일하게 동작(blink),
	이후 상태감시 알람이 해제되면 다시 자동 Off 타이머가 동작하여 5분후에 Off
	
	----------------------------
	algorithm
	----------------------------
	LED 자동 Off 0x5455를 OFF 제어하면, m_St.AAU_LED_Disable = 0;
	LED 자동 Off 0x5455를 ON  제어하면, 
		5분 전, m_St.AAU_LED_Disable = 0
		5분 후, m_St.AAU_LED_Disable = 1

	m_St.AAU_LED_Disable : 
		0 : LED ON/OFF Control 가능. 
		1 : LED OFF Always	
-------------------------------------------------------------
*/

INT8U IS_AAU_LED_Disabled(void)
{
	return m_St.AAU_LED_Disable;
}

static OS_TIMER AAU_LED_Auto_Off_timer;

void Proc___AAU_LED_Auto_Off(void)
{
	static INT8U init = 0;
	static INT8U prev_st = 0;
	
	if(init == 0)
	{
		init = 1;
		m_St.AAU_LED_Auto_Off = ON; // default ON
		prev_st = ON; // default ON
		OSTimerReset(&AAU_LED_Auto_Off_timer);
		m_St.AAU_LED_Disable = 0;
	}
	
	if(prev_st != m_St.AAU_LED_Auto_Off)
	{
		prev_st = m_St.AAU_LED_Auto_Off;
		m_St.AAU_LED_Disable = 0;
		
		if(m_St.AAU_LED_Auto_Off == ON)
		{
			init = 0; // init for restart
			return; // for restart
		}
	}
	else if(m_St.AAU_LED_Auto_Off == ON)
	{
		/*
		2019.08.30 : 	
		상태 Polling시에는 이전과 동일하게 동작(blink),
		이후 상태감시 알람이 해제되면, 5분후에 Off
		*/
		if(Is_AAU_Polling_Now() || m_St.AlmSt.A_0.GUI)
		{
			OSTimerReset(&AAU_LED_Auto_Off_timer);
			m_St.AAU_LED_Disable = 0;
		}
		
		if(m_St.AAU_LED_Disable == 0) // if not disable
		{
			if(OSTimerGet_msec(&AAU_LED_Auto_Off_timer) >= (5*TICK_CO_MIN))
			{
				OSTimerReset(&AAU_LED_Auto_Off_timer);
				m_St.AAU_LED_Disable = 1;
				print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[AAU_LED_Auto_Off] 5min over, LED Disabled !!");
			}
		}
	}	
}		

void Proc____AAU_LED(void)
{
//------------------------------------------------------	
#ifdef __TYPE__AAU__ // AAU 	
//------------------------------------------------------
	static INT8U toggle = 0;
	static OS_TIMER LED_Timer;
	static INT8U init = 0;
	
	INT8U path;
	INT8U cpri;

	INT16U case_1 = 0; // blue X, Red O
	INT16U case_2 = 0; // blue O, Red O

	
	// 2019.08.21
	//----------------------------------------- 
	Proc___AAU_LED_Auto_Off();
	if(IS_AAU_LED_Disabled())
	{
		LED_RED__OFF();
		LED_BLUE_OFF();
		init = 0;
		return;
	}
	//----------------------------------------- 

	if(init == 0)
	{
		init = 1;
		m_St.ANTS_LED_Status = ANTS_LED_Max; // init value
	}

	// case 1 ----------------------------------------- 	
	     if(m_St.AlmSt.A_0.FAN1_Case) case_1 = cbits16[0];
	else if(m_St.AlmSt.A_0.Temp_Up)   case_1 = cbits16[1];
	else if(m_St.AlmSt.UDA.Spurious)  case_1 = cbits16[2];
	else if(m_St.AlmSt.UDA.PD_CPRI)   case_1 = cbits16[3];
	else if(m_St.AlmSt.UDA.Clock)     case_1 = cbits16[4];
	else if(m_St.AlmSt.A_0.DC)        case_1 = cbits16[5]; // dc fail	
	else if(m_St.Is_CPRI_Lock==0)     case_1 = cbits16[6]; // cpri unlock	
	else if(m_St.AlmSt.A_4.Clock)     case_1 = cbits16[7]; // clock fail	--------- ??
	else if(m_St.AlmSt.UDA.CPRI_RST_Suppr)  case_1 = cbits16[8]; // UDA CPRI Reset Suppression
	else if(m_St.AlmSt.UDA.Clk__RST_Suppr)  case_1 = cbits16[9]; // UDA Clock Reset Suppression
	else
	{
		case_1 = 0; // init 0
		
		for(cpri = 0; cpri < CPRI_PATH; cpri++)
		{			
			// cpri alarm, 2019.04.25
			if(m_St.AlmSt.A_2.CPRI_0__) case_1 = cbits16[8];
			if(m_St.AlmSt.A_2.CPRI_0_a) case_1 = cbits16[9]; 	
			if(m_St.AlmSt.A_2.CPRI_1__) case_1 = cbits16[8];
			if(m_St.AlmSt.A_2.CPRI_1_a) case_1 = cbits16[9]; 	

			// 2019.06.21
			if(m_St.AlmSt.A_2.LOS_SFP_1_a) case_1 = cbits16[8]; // LOS(SFP1)(+α) (OPTIC_TRANSCEIVER_RX_LOS)
			if(m_St.AlmSt.A_2.LOS_SFP_1__) case_1 = cbits16[8]; // LOS(SFP1)      (OPTIC_TRANSCEIVER_RX_LOS)
			if(m_St.AlmSt.A_2.LOS_SFP_0_a) case_1 = cbits16[8]; // LOS(SFP0)(+α) (OPTIC_TRANSCEIVER_RX_LOS)
			if(m_St.AlmSt.A_2.LOS_SFP_0__) case_1 = cbits16[8]; // LOS(SFP0)      (OPTIC_TRANSCEIVER_RX_LOS)

			if(m_St.AlmSt.A_3.LD_SFP_1_a) case_1 = cbits16[8]; // LD(SFP1)(+α) (OPTIC_TRANSCEIVER_TX_FAULT)
			if(m_St.AlmSt.A_3.LD_SFP_1__) case_1 = cbits16[8]; // LD(SFP1)      (OPTIC_TRANSCEIVER_TX_FAULT)
			if(m_St.AlmSt.A_3.LD_SFP_0_a) case_1 = cbits16[8]; // LD(SFP0)(+α) (OPTIC_TRANSCEIVER_TX_FAULT)
			if(m_St.AlmSt.A_3.LD_SFP_0__) case_1 = cbits16[8]; // LD(SFP0)      (OPTIC_TRANSCEIVER_TX_FAULT)
			
			if(case_1 > 0) break;
		}		
		
		for(path=0; path < MAX_MIMO_PATH; path++) // 100M
		{
			// cpri alarm
			if(Check_PAU_Use(path))
			{	
				if(m_St.AlmSt.Path[path].DL.TSSI_Upper__) case_1 = cbits16[10]; //
				if(m_St.AlmSt.Path[path].DL.TSSI_Upper_a) case_1 = cbits16[11]; //			
				if(m_St.AlmSt.Path[path].UL.Over_Input)   case_1 = cbits16[12]; //
				if(m_St.AlmSt.Path[path].UL.Gain_Low)     case_1 = cbits16[13]; //
				if(m_St.AlmSt.Path[path].PAU.Shutdown)    case_1 = cbits16[14]; // amp disable
				if(m_St.AlmSt.Path[path].PAU.Disabled)    case_1 = cbits16[15]; // amp disable
			}
			if(case_1 > 0) break;
		}
	}	
	// case 2 -----------------------------------------
	case_2 = 0; // init 0
	for(path=0; path < MAX_MIMO_PATH; path++)
	{
		if(Check_PAU_Use(path))
		{	
			if(m_St.AlmSt.Path[path].DL.TSSI_Lower__) case_2 = cbits16[0]; 
			if(m_St.AlmSt.Path[path].DL.TSSI_Lower_a) case_2 = cbits16[1];
			if(m_St.AlmSt.Path[path].DL.Out_Lower)    case_2 = cbits16[2];
			if(m_St.AlmSt.Path[path].DL.Out_Upper)    case_2 = cbits16[3];
		}
		if(case_2 > 0) break;
	}
	//-----------------------------------------------
	// every 500ms
	if(OSTimerGet_msec(&LED_Timer) >= 500)
	{
		OSTimerReset(&LED_Timer);
		toggle ^= 1;
	}
	//--------------------------
	if(Is_AAU_Polling_Now())
	{
		if(case_1 || case_2) // alarm
		{
			if(toggle) {LED_BLUE__ON(); LED_RED__OFF();}
			else       {LED_BLUE_OFF(); LED_RED___ON();}

			m_St.ANTS_LED_Status = ANTS_LED_Toggle;
		}
		else
		{
			LED_RED__OFF();
			if(toggle) LED_BLUE__ON();
			else       LED_BLUE_OFF();

			m_St.ANTS_LED_Status = ANTS_LED_B_Blnk;
		}
	}
	else
	{
		if(case_1 || case_2) // alarm
		{
			if(case_1)
			{
				LED_RED___ON();
				LED_BLUE_OFF();

				m_St.ANTS_LED_Status = ANTS_LED_R___ON;
			}			
			else if(case_2)
			{				
				LED_RED___ON();
				LED_BLUE__ON();

				m_St.ANTS_LED_Status = ANTS_LED_R_B_ON;
			}
		}
		else // normal
		{
			LED_RED__OFF();
			LED_BLUE__ON();

			m_St.ANTS_LED_Status = ANTS_LED_B___ON;
		}
	}
//------------------------------------------------------
#endif
}






/*
------------------------------------------------------------------------
	RVS-IN 
	ALC
	Algorithm 
------------------------------------------------------------------------
*/
// Get immediate digital rvs alc atten
INT8U Get_TI_ALC_Att_x2(INT8U path4)
{
	INT8U att_x2 = 0;

	if(path4 >= AMPU_4_MAX)
		return 0;
		
#ifdef __TYPE__AAU__ // ANTS-AAU
	////att_x2 = 2*m_Reg_Sts.ALC[path4].ALC_Att.Val_x1;
	att_x2 = m_St.RX_Qck_Att_x2[path4];
#endif
	return att_x2;
}


INT16S Get_Other_Company_Det_x10(INT8U path4)
{
	INT16S Pwr_dBFs_x10 = -1001; // -100.1dB	
	
	if(path4 >= AMPU_4_MAX)
		return -1000; // -100.0dB

#ifdef __TYPE__AAU__ // ANTS-AAU
	INT32U Reg_32;
	double d_val;	
			
	// Conversion -----------------------------------------------------			
	Reg_32 = m_Reg_Sts.ALC[path4].ALC_Att.Data;
	d_val = 100.0*log10((double)Reg_32);
	Pwr_dBFs_x10 = (INT16S)d_val - 790;	
#endif

	return Pwr_dBFs_x10;
}

//--------------------------------------------------------------
void AAU__Rvs_Alc_Att_Set(INT8U path4, INT8U quick_att_x2, INT8U alc_att_x2)
{
#ifdef __TYPE__AAU__
	INT8U user_att_x4; // 0.25dB step
	INT8U sum__att_x4; // 0.25dB step
	
	if(path4 >= AMPU_4_MAX)
		return;
		
	user_att_x4 = m_Eep.UL_Att[path4];	
	sum__att_x4 = user_att_x4 + (quick_att_x2 + alc_att_x2)*2;
	if(sum__att_x4 > (30*4)) 
		sum__att_x4 = (30*4);

	m_St.RX_ALC_Att_x2[path4] = alc_att_x2; // 2019.05.04
	
	AAU_RF_UL_Att_Set(path4, sum__att_x4/2);
#endif	
}
//--------------------------------------------------------------
/*
AID 0x5106 : m_St.TX_ALC_Att_x10 + m_Eep.DL_Att_User
AID 0xC036 : m_Eep.DL_Att_Offs
Register   : m_St.TX_ALC_Att_x10 + m_Eep.DL_Att_User + m_Eep.DL_Att_Offs + temp(?)
*/
void AAU__Fwd_Alc_Att_Set(INT8U path4, INT8U alc_att_x2)
{
#ifdef __TYPE__AAU__
	INT8U user_att_x4; // 0.25dB step
	INT16U sum__att_x4; // 0.25dB step, 2020.02.24 : INT8U -> INT16U, 단순보완 코드
	
	if(path4 >= AMPU_4_MAX)
		return;
		
	user_att_x4 = m_Eep.DL_Att_User[path4];	
	sum__att_x4 = user_att_x4 + alc_att_x2*2;
	if(sum__att_x4 > (30*4)) 
		sum__att_x4 = (30*4);

	m_St.TX_ALC_Att_x10[path4] = alc_att_x2*5;
	
	AAU_RF_DL_Att_Set(path4, sum__att_x4/2);
#endif	
}

//--------------------------------------------------------------
#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU
/*
	control att_x2(user + alc) 
	with att_offset + temp_offset + accuracy

	att_x2 is 'm_St.DL_Att'
*/
void AAU_RF_DL_Att_Set(INT8U path4, INT8U att_x2) // plus offset_att & temp_att
{
	INT16S sTemp_x4;
	INT16S sTemp_x2;
	INT16S att___x4;

	m_PAU_C_Req[path4].Fwd_Att_x2 = att_x2;
	
	sTemp_x2 = conv_signed_att_01_to_05(m_St.d_Temp_Att_Dig_DL_x10[COMMON][path4]);
	sTemp_x4 = conv_signed_att_05_to_025(sTemp_x2);
		
	att___x4 = (att_x2*2 + sTemp_x4); // 0.5 step --> 0.25 step
	att___x4 += m_Eep.DL_Att_Offs[path4]; // 2019.06.01
	
	if(att___x4 < 0) att___x4 = 0;
	// 2019.06.27 : max Att limit
	if(att___x4 > 126) att___x4 = 126; // 31.5 x 4 = 126

	RF_Att_Accuracy_x4[DL][path4] = 2*Get_AAU_RF_Att_Accuracy_x2(DL, path4, att___x4/2);
		
	m_Reg_Flg.RFU[path4].DL_Att.Data = _SET_;	
	m_Reg_Ctl.RFU[path4].DL_Att.Trig = 1;
	m_Reg_Ctl.RFU[path4].DL_Att.Val_x4 = att___x4 + RF_Att_Accuracy_x4[DL][path4];

	/*
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[RF DL Att] AMP%d : %2.1f(%2.1f + Temp %2.1f) + Accu(%2.1f) = tot (%2.1f, 0x%X)", 
			path4, 
			(double)att___x4/4.0, (double)att_x2/2.0, (double)sTemp_x4/4.0, 
			(double)RF_Att_Accuracy_x4[DL][path4]/4.0, 
			(double)(att___x4 + RF_Att_Accuracy_x4[DL][path4])/4.0,
			att___x4 + RF_Att_Accuracy_x4[DL][path4]);
	*/		
}
#endif
//--------------------------------------------------------------
#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU
void AAU_RF_UL_Att_Set(INT8U path4, INT8U att_x2)
{	
	INT16S sTemp_x4;
	INT16S sTemp_x2;
	INT16S att___x4;

	m_PAU_C_Req[path4].Rvs_Att_x2 = att_x2;

	sTemp_x2 = conv_signed_att_01_to_05(m_St.d_Temp_Att_Dig_UL_x10[COMMON][path4]);
	sTemp_x4 = conv_signed_att_05_to_025(sTemp_x2);

	att___x4 = att_x2*2 + sTemp_x4; // 0.5 step --> 0.25 step
	if(att___x4 < 0) att___x4 = 0;
	// 2019.06.27 : max Att limit
	if(att___x4 > 126) att___x4 = 126; // 31.5 x 4 = 126

	RF_Att_Accuracy_x4[UL][path4] = 2*Get_AAU_RF_Att_Accuracy_x2(UL, path4, att___x4/2);
		
	m_Reg_Flg.RFU[path4].UL_Att.Data = _SET_;	
	m_Reg_Ctl.RFU[path4].UL_Att.Trig = 1;
	m_Reg_Ctl.RFU[path4].UL_Att.Val_x4 = att___x4 + RF_Att_Accuracy_x4[UL][path4];
	
	/*
	print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[RF UL Att] AMP%d : %2.1f(%2.1f + Temp %2.1f) + Accu(%2.1f) = tot (%2.1f, 0x%X)", 
			path4, 
			(double)att___x4/4.0, (double)att_x2/2.0, (double)sTemp_x4/4.0, 
			(double)RF_Att_Accuracy_x4[UL][path4]/4.0, 
			(double)(att___x4 + RF_Att_Accuracy_x4[UL][path4])/4.0,
			att___x4 + RF_Att_Accuracy_x4[UL][path4]);	
	*/		
}

void AAU_UL_AFE_Att_Set(INT8U path4, INT8U att_x1)
{
	if(path4 >= AMPU_4_MAX) return;	
	
	if(att_x1 > 20)
		att_x1 = 0;

	//print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[AAU_UL_AFE_Att_Set] path %d : 0x%X", path4, att_x1);	

	m_Reg_Flg.ALC[path4].Def_Att.Data = _SET_;
	m_Reg_Ctl.ALC[path4].Def_Att.Val_x1 = att_x1;	
}

#endif
//--------------------------------------------------------------
#if defined(__MAKER_ANTS__) && defined(__TYPE__AAU__) // ANTS-AAU
void AAU__RF_AMP_OnOff(INT8U path4, INT8U onoff, INT16U idx)
{
	// 2019.09.24
	if(IS_RU_Init_Completed()==0)
	{
		if(m_Env.AMP_Ctrl_Allowed == 0)
			onoff = OFF;
		else if(m_Env.AMP_User_Ctrl[path4] == 0)
			onoff = OFF;
	}
	

	m_Reg_Ctl.RFU[path4].PA_OnOff.Data = Read_FPGA_Reg(Reg_RFU_CTRL_Addr[path4][Reg_RFU_CTRL_RW_PA_OnOff]);
		
	if(onoff == ON) m_Reg_Ctl.RFU[path4].PA_OnOff.On = 1;
	else            m_Reg_Ctl.RFU[path4].PA_OnOff.On = 0;

	Write_FPGA_Reg(Reg_RFU_CTRL_Addr[path4][Reg_RFU_CTRL_RW_PA_OnOff], m_Reg_Ctl.RFU[path4].PA_OnOff.Data, "PA OnOff");
	
	m_Reg_Flg.RFU[path4].PA_OnOff.Data = _SET_;

	AAU__RF_LNA_OnOff( path4,  onoff,  idx);
}

void AAU__RF_LNA_OnOff(INT8U path4, INT8U onoff, INT16U idx)
{	
#if 0
	m_Reg_Ctl.RFU[path4].PA_OnOff.Data = Read_FPGA_Reg(Reg_RFU_CTRL_Addr[path4][Reg_RFU_CTRL_RW_PA_OnOff]);
		
	if(onoff == ON) m_Reg_Ctl.RFU[path4].PA_OnOff.UL_LNA = 1;
	else            m_Reg_Ctl.RFU[path4].PA_OnOff.UL_LNA = 0;

	Write_FPGA_Reg(Reg_RFU_CTRL_Addr[path4][Reg_RFU_CTRL_RW_PA_OnOff], m_Reg_Ctl.RFU[path4].PA_OnOff.Data, "LNA OnOff");
	
	m_Reg_Flg.RFU[path4].PA_OnOff.Data = _SET_;
#endif	
}

#endif
//--------------------------------------------------------------




#ifdef __TYPE__AAU__
//--------------------------------------------------------------
// ANTS-AAU Only
/*
return val_x10 
	= att__temp_x10 + att__offs_x10 + att__user_x10 + gain_backoff_x10 + gain_balance_x10
	- ls_gain_bal_x10
*/
INT16S Get__AAU_UL_Gain_Att_x10(INT8U optic, INT8U path4)
{
	INT16S val_x10 = 0;
	INT16S att__offs_x10; // hidden offset
	INT16S att__user_x10;
	INT16S gain_backoff_x10;
	INT16S att__temp_x10 = 0;
	INT16S gain_balance_x10;
	INT16S ls_gain_bal_x10;
	
	if(path4 >= MAX_PATH_4) return 0;		
	
	att__temp_x10 = Get_AAU_DTU_Alpha_Temp_Att_x10(optic, path4, UL); // 2019.06.28 : Alpha_DTU_Temp_Att	
	att__offs_x10 = m_Eep.d_Att_Dig_UL_Offs_x10[optic][path4];
	att__user_x10 = m_Eep.d_Att_Dig_UL_User_x10[optic][path4];
	gain_backoff_x10 = m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4];
	gain_balance_x10 = m_St.d_UL_GainBal_x10;
	ls_gain_bal_x10 = m_St.Curr_LS_Gain_Bal_x10[optic][UL][path4]; // L/S Gain Balance

	val_x10 = att__temp_x10 + att__offs_x10 + att__user_x10 + gain_backoff_x10 + gain_balance_x10 - ls_gain_bal_x10;

	if(val_x10 < 0) val_x10 = 0;
	
	return val_x10;
}


void Set_ALC_Gain_Comp_x10(INT8U optic, INT8U path4, INT16S gain_x10) 
{
	INT16S val_x10 = 0;
	INT16S att__sum__x10;
	INT32U uVal32;
	INT16S gain__sum__x10;
	INT16S gain_user_x10;
	

	if(optic >= MAX_Optic) return;	
	if(path4 >= AMPU_4_MAX) return;		

	val_x10 = Get__AAU_UL_Gain_Att_x10(optic, path4);
	
	gain_user_x10 = Conv_Gain_from_Att(val_x10); // gain <- att

	m_St.AAU_ALC_Gain_Comp_x10[optic][path4] = gain_x10;
	
	gain__sum__x10 = gain_user_x10 + gain_x10;
	att__sum__x10 = Conv_Gain_to_Att(gain__sum__x10);	// att <- gain

	// register gain set
	
	if((att__sum__x10%5) > 0)
		uVal32 = Get_GainRegVal(UL, att__sum__x10/5+1);
	else
		uVal32 = Get_GainRegVal(UL, att__sum__x10/5);
	
	m_Reg_Flg.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32 = 1;
	m_Reg_Ctl.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32 = uVal32;
}
//--------------------------------------------------------------
// ANTS-AAU Only
INT16S Get_ALC_Gain_Comp_x10(INT8U optic, INT8U path4)
{
	INT16S att__conv_x10;
	INT16S gain_curr_x10;
	INT16S val_x10 = 0;
	INT16S gain_user_x10;	
	INT16S gain_comp_x10;		
	INT32U uVal32;

	if(optic >= MAX_Optic) return 0;	
	if(path4 >= AMPU_4_MAX) return 0;	
	
	uVal32 = m_Reg_Sts.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32;
	att__conv_x10 = conv_Reg_Gain_to___Att_x10(UL, uVal32);	

	val_x10 = Get__AAU_UL_Gain_Att_x10(optic, path4);
	
	gain_curr_x10 = Conv_Gain_from_Att(att__conv_x10); // gain <- att		
	gain_user_x10 = Conv_Gain_from_Att(val_x10); // gain <- att

	gain_comp_x10 = gain_curr_x10 - gain_user_x10;
	/*
	gain_comp_x10 (delta)
	= gain_curr_x10 - gain_user_x10
	= Conv_Gain_from_Att(att__conv_x10) - Conv_Gain_from_Att(att__offs_x10 + att__user_x10 + gain_backoff_x10)
	= 20-(att__conv_x10) - (20 - (att__offs_x10 + att__user_x10 + gain_backoff_x10))
	= att__offs_x10 + att__user_x10 + gain_backoff_x10 - att__conv_x10

	---------------------------------------
	if user_gain set '0' ( == 'att__offs_x10' is '20')
	if gain_backoff_x10 is '0'
	if att__user_x10 is '0'
	
	if compensation is +3, ==> att_conv 17
	(gain_backoff_x10 + att__offs_x10 + att__user_x10) - att__conv_x10
		= (0 + 20 + 0) - (17)
		= 3

	---------------------------------------
	if user_gain set '0' ( == 'att__offs_x10' is '20')
	if gain_backoff_x10 is '1'
	if att__user_x10 is '1.5'	

	digital att = 20 + 1 + 1.5 = 22.5
	22.5 --> conv --> digital gain '-2.5'

	comp = 
	(gain_backoff_x10 + att__offs_x10 + att__user_x10) - att__conv_x10
		= (1 + 20 + 1.5) - (22.5)
		= 0
	
	---------------------------------------
	if user_gain set '0' ( == 'att__offs_x10' is '20')
	if gain_backoff_x10 is '1'
	if att__user_x10 is '1.5'		

	digital att = 20 + 1 + 1.5 = 22.5
	22.5 --> conv --> digital gain '-2.5'
	
	if gain delta(compensation) is '3'
	total digital gain is '0.5'
	
	0.5 conv --> digital att '19.5'

	gain_backoff_x10 is '1'
	att__user_x10 is '1.5'
	digital att is '17'

	that is digital att 20 --> 17, compensation is 3.

	(gain_backoff_x10 + att__offs_x10 + att__user_x10) - att__conv_x10
		= (1 + 20 + 1.5) - (19.5)
		= 3
	
	*/
	//------------------------------
	if(gain_comp_x10 < 0) gain_comp_x10 = 0;
	
	return gain_comp_x10;
}
#endif

INT16S Get__AAU_User_Gain_x10(INT8U optic, INT8U path4)
{
	INT16S att__user_x10;
	INT16S gain_user_x10;	

	att__user_x10 = m_Eep.d_Att_Dig_UL_Offs_x10[optic][path4] + m_Eep.d_Att_Dig_UL_User_x10[optic][path4];	
	gain_user_x10 = Conv_Gain_from_Att(att__user_x10); // gain <- att
	
	return gain_user_x10;
}


//--------------------------------------------------------------

INT16S Get__AAU_TotPwr_x10_UL_RSSI(INT8U path4)
{
	// 2019.05.10
	double d_RSSI[MAX_Optic];
	INT16S tot_pwr_x10;
	
	d_RSSI[_100M] = (double)(m_St.d_UL_RSSI_dBm_Alc_x10[_100M][path4])/10.0;
	d_RSSI[Alpha] = (double)(m_St.d_UL_RSSI_dBm_Alc_x10[Alpha][path4])/10.0;

	tot_pwr_x10 = 100*log10(pow(10.00, d_RSSI[_100M]/10.0)+pow(10.00, d_RSSI[Alpha]/10.0));
	
	return tot_pwr_x10;
}

INT16S Get__AAU_TotPwr_x10_DL_TSSI(INT8U path4)
{
	// 2019.05.11
	double d_TSSI[MAX_Optic];
	INT16S tot_pwr_x10;
	
	d_TSSI[_100M] = (double)(m_St.d_DL_TSSI_dBm_x10[_100M][path4])/10.0;
	d_TSSI[Alpha] = (double)(m_St.d_DL_TSSI_dBm_x10[Alpha][path4])/10.0;

	tot_pwr_x10 = 100*log10(pow(10.00, d_TSSI[_100M]/10.0)+pow(10.00, d_TSSI[Alpha]/10.0));

	return tot_pwr_x10;
}



#define AAU_UL_Gain_Max_x10 (30*10)
void Proc___Calc_AAU_UL_Gain(INT8U path4)
{
// 2019.05.31
	INT16U user_att_x10;
	INT16U gain_backOff_x10;
	INT16U gain_balance_x10;
	
	if(path4 >= AMPU_4_MAX) return;
	// AMP-Off , UL gain Check 는 동작(Gain_Low 알람은 mask)
	/*	
	-----------------------------------------------------------------------------------------------
	2019.11.28 : Gain_현재값 수식 변경.		
	[Chagne]
		Gain(0x5207) = 30.0dB - [ User_ATTEN(0x5206) + Gain_BackOff(0xB426) + Gain_Balance(0x5209) ]

	[Old] m_St.UL_Gain_x10[path4] = AAU_UL_Gain_Max_x10 - m_St.d_UL_Att_x10[FA_0][path4];
		here, 
		m_St.d_UL_Att_x10[fa][m] 
			= Quick_ATT - Gain_Compensation + User_Att			
			= m_St.RX_Qck_Att_x2[m]*5 - Get_ALC_Gain_Comp_x10(fa, m) + m_Eep.d_Att_Dig_UL_User_x10[fa][m];

		0x5206 : m_Eep.d_Att_Dig_UL_User_x10
		m_Eep.UL_Att[path4] : RF Offset ATT.
	-----------------------------------------------------------------------------------------------
	*/
	user_att_x10 = m_Eep.d_Att_Dig_UL_User_x10[FA_0][path4];
	gain_backOff_x10 = m_St.d_UL_DTU_Gain_Backoff_x10[FA_0][path4];
	gain_balance_x10 = m_St.d_UL_GainBal_x10;
	
	m_St.UL_Gain_x10[path4] = AAU_UL_Gain_Max_x10 - (user_att_x10 + gain_backOff_x10 + gain_balance_x10);
}


/*
@ 5G
-	ALC ATT의 Range는 0~20.0dB범위내 동작한다.
-	Alarm 발생/해제 및 동작 Hysteresis는 2.0dB적용한다.
*/

#define Hystresis_Att_x2 (1*2)
#define ALC_Range_x2 (20*2)
#define UL_RF_Max_Att_x2 (30*2) // user_att + alc_att

#ifdef __TYPE__AAU__

INT8U Proc___AAU_RVS_In_ALC(INT8U path4)
{
	INT16S ctl_att_x2; // 2020.01.07 : INT8S -> INT16S
	INT8U  ALC__Onoff;
	INT16S level_x10;

	INT16S rvs_out_pwr_x10;	

	INT16S diff_x2;	
	//INT8S  user_att_x2; // user control att	
	
	INT16S  Gain_Comp_x2;	
	INT16S  rx_Qck_Att_x2;
	INT8S  max_alc_range_x2;

	INT8U  att_set = 0;	
	INT8U  ctrl_prev = 0;

	INT8S  cur_alc_att_x2;
	INT16S user_gain_x10; // 2019.05.07

	//#ifdef __USE_AAU_ALC_SPEED_CTRL__
	//#endif
	// 2019.09.17 : 
	static INT8U release_cnt[AMPU_4_MAX] = {0,0,0,0};
	
	
	//----------------------
	
	
	//----------------------	
	if(path4 >= AMPU_4_MAX) return 0;				
	//----------------------
	if(Check_PAU_Use(path4) == 0)
		return 0;	
		
	// Transciever level	
	// 5*m_Reg_Sts.ALC[path4].ALC_Threshold.Val_x2; 
	//----------------------
	ALC__Onoff = m_St.Rvs_IN_ALC_Onoff[path4];		
	level_x10  = m_St.Rvs_IN_ALC_x10[path4];
	//----------------------
	//user_att_x2 = m_Eep.UL_Att[path4]/2; // ATNS-AAU-RF-0.25dB Unit		
	rx_Qck_Att_x2 = Get_TI_ALC_Att_x2(path4);	

	Gain_Comp_x2 = Get_ALC_Gain_Comp_x10(_100M, path4)/5;	
	
	m_St.UL_DTU_Gain_Comp_x10[_100M][path4] = Gain_Comp_x2*5;	
	//// for quick-att-display
	//m_St.UL_DTU_Gain_Comp_x10[Alpha][path4] = Gain_Comp_x2*5;
	m_St.UL_DTU_Gain_Comp_x10[Alpha][path4] = m_St.RX_Qck_Att_x2[path4]*5;	
	
	//----------------------
	max_alc_range_x2 = ALC_Range_x2 - rx_Qck_Att_x2;
	if(max_alc_range_x2 < 0) max_alc_range_x2 = 0;
	//----------------------
	
	//----------------------
	cur_alc_att_x2 = m_St.RX_ALC_Att_x2[path4]; // 2019.05.04
	
	if(cur_alc_att_x2 < 0)
	{		
		cur_alc_att_x2 = 0;
		
		//D_Printf( DEBUG_STDBY, VT_RED "[RVS-ALC] <AMP_%d> Abnormal   : ALC_Att < 0 \n", path4);	
	}
	//----------------------	
	rvs_out_pwr_x10 = Get__AAU_TotPwr_x10_UL_RSSI(path4);
	
	m_St.RSSI_After__Filter_x10[_100M][path4] = rvs_out_pwr_x10;	// 0xB303, RSSI(Filter 후)
	m_St.RSSI_After__Filter_x10[Alpha][path4] = rvs_out_pwr_x10;	// 0xB303, RSSI(Filter 후)
	//----------------------
	m_St.Rvs_IN_Pwr_x10[path4] = rvs_out_pwr_x10 + 5*(m_St.RX_ALC_Att_x2[path4] + rx_Qck_Att_x2 - Gain_Comp_x2);
	//----------------------
	
	/*
	// if Rvs gain-Balace is used 
	//if(m_St.Rvs_Gain_Bal_On[path4] == ON)
	//	rvs_in_pwr_x10 += m_St.Rvs_Gain_Bal_x2[path4];		
	*/

	//-----------------------------------------------------
	// Rvs ALC Atten Range is 10dB from User-Att.
	//-----------------------------------------------------
	if(m_Reg_Sts.RFU[path4].PA_OnOff.On == 0) // if OFF
		return 0;
		
	// 2019.05.04
	if((m_Reg_Sts.General_FA[FA_0].General_FA_MIMO[path4].RW_UL_OnOff.OnOff == 0) &&
	   (m_Reg_Sts.General_FA[FA_1].General_FA_MIMO[path4].RW_UL_OnOff.OnOff == 0))
	{
		return 0;
	}
	//-----------------------------------------------------

	

	//-----------------------------------------------------

	if(ALC__Onoff == ON)
	{		
		diff_x2 = (rvs_out_pwr_x10 - level_x10)/5;
		att_set = 0; // init
		
		#if 0
		{			
			D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> --------\n", path4);
			D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Status : Out_Pwr (%d/10) - Limit (%d/10) = (%d/10), Max_Range (%d/2)\n", 
					path4,					
					rvs_out_pwr_x10, 
					level_x10,
					(rvs_out_pwr_x10 - level_x10),
					max_alc_range_x2);
					
			D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Status : User (%d/2) + RF-ALC (%d/2) + Quick_ALC (%d/2) -- Gain_Comp (%d/2)\n\n", 
					path4,
					user_att_x2, cur_alc_att_x2, rx_Qck_Att_x2, Gain_Comp_x2);					
		}
		#endif
		
		//------------------------------
		if(diff_x2 >= 0) // power >= level
		{	
			if(rvs_out_pwr_x10 == level_x10) 
				diff_x2 = 1; // 0.5dB
			
			/////////CtrlLock();
			ctrl_prev = Gain_Comp_x2;
			if(Gain_Comp_x2 > 0)
			{				
				Gain_Comp_x2 -= diff_x2; // for Gain Decrement ('diff_x2' is positive value)
				if(Gain_Comp_x2 < 0) Gain_Comp_x2 = 0; // ~ minimum 0 dB

				Set_ALC_Gain_Comp_x10(_100M, path4, 5*Gain_Comp_x2);
				Set_ALC_Gain_Comp_x10(Alpha, path4, 5*Gain_Comp_x2);

				if(ctrl_prev != Gain_Comp_x2) 
				{
					att_set = 1; // atten set
					//D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Over   : diff(%d/2),  Gain_Comp(%d/2) Set\n\n", 
					//	path4, diff_x2, Gain_Comp_x2);	
				}				
			}
			///else // 2019.05.04
			if(att_set == 0)  // 2019.05.04
			{		
				ctrl_prev = cur_alc_att_x2;
				
				ctl_att_x2 = diff_x2 + cur_alc_att_x2;
				if(ctl_att_x2 > max_alc_range_x2) // max 20.0 dB
					ctl_att_x2 = max_alc_range_x2;
				
				AAU__Rvs_Alc_Att_Set(path4, rx_Qck_Att_x2, ctl_att_x2);
				
				if(ctrl_prev != ctl_att_x2) 
				{
					att_set = 1; // atten set
					//D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Over   : diff(%d/2),  ALC_ATT (%d/2) Set.\n\n", 
					//	path4, diff_x2, ctl_att_x2);	
				}				
			}
			//#ifdef __USE_AAU_ALC_SPEED_CTRL__
			//#endif
			// 2019.09.17 : 
			release_cnt[path4] = 0;			
		}	
		//----------------------------------------------------------
		// Pwr is under 
		// if 'hysterisis 2.0dB' and alc-only-att is above 0[dB](that is cur_att is above user att) 
		//     then atten release
		//----------------------------------------------------------
		else if(diff_x2 <= (-Hystresis_Att_x2)) // power < level
		{
			//#ifdef __USE_AAU_ALC_SPEED_CTRL__	
			//#endif	
			//m_Env.AAU_alc_max_cnt = m_Eep.TempLmt_x10%10;
			//--------------------------
			// 2019.09.17 : fix with 8
			//--------------------------
			m_Env.AAU_alc_max_cnt = 8;
						
			release_cnt[path4]++;
			if(release_cnt[path4] >= m_Env.AAU_alc_max_cnt)
				release_cnt[path4] = 0;
			else
				return 1; // 1 : next-quick-alc not working		
			//--------------------------
			
			diff_x2 += Hystresis_Att_x2; // 2.0[dB]
			
			if(diff_x2 == 0) diff_x2 = -1; // 0.5dB			

			////////////CtrlLock();
			ctrl_prev = Gain_Comp_x2;
			if(rx_Qck_Att_x2 > 0) // Quick-Atten
			{				
				Gain_Comp_x2 -= diff_x2; // for Gain-Increment ( 'diff_x2' is negative value )
				if(Gain_Comp_x2 > rx_Qck_Att_x2) 
					Gain_Comp_x2 = rx_Qck_Att_x2; 

				user_gain_x10 = Get__AAU_User_Gain_x10(_100M, path4);

				////D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> user_gain_x10  (%d/10)\n", 
				////		path4, user_gain_x10);	

				// 2019.05.07 : limit max value
				// 2019.08.24 : 20dB -> 30dB
				if(Gain_Comp_x2 >= (30*10-user_gain_x10)/5)
					Gain_Comp_x2 = (30*10-user_gain_x10)/5;
				
				Set_ALC_Gain_Comp_x10(_100M, path4, 5*Gain_Comp_x2);				
				Set_ALC_Gain_Comp_x10(Alpha, path4, 5*Gain_Comp_x2);

				if(ctrl_prev != Gain_Comp_x2) 
				{
					att_set = 1; // atten set
					//D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Under  : diff(%d/2),  Gain_Comp(%d/2) Set\n\n", 
					//	path4, diff_x2, Gain_Comp_x2);	
				}
			}
			else if(rx_Qck_Att_x2 == 0) // Quick-Atten
			{	
				if(Gain_Comp_x2 > 0) 
					Gain_Comp_x2 = 0; 
				
				Set_ALC_Gain_Comp_x10(_100M, path4, 5*Gain_Comp_x2);
				Set_ALC_Gain_Comp_x10(Alpha, path4, 5*Gain_Comp_x2);

				if(ctrl_prev != Gain_Comp_x2) 
				{
					att_set = 1; // atten set
					//D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Under  : Gain_Comp(%d/2) Set\n\n", 
					//	path4, diff_x2, Gain_Comp_x2);	
				}
			}
			//else // 2019.05.04
			if(att_set == 0) // 2019.05.04 
			{
				ctrl_prev = cur_alc_att_x2;
				
				ctl_att_x2 = diff_x2 + cur_alc_att_x2;
				if(ctl_att_x2 < 0) 
					ctl_att_x2 = 0;			

				AAU__Rvs_Alc_Att_Set(path4, rx_Qck_Att_x2, ctl_att_x2);
				
				if(ctrl_prev != ctl_att_x2) 
				{
					att_set = 1; // atten set
					//D_Printf( DEBUG_STDBY, "[RVS-ALC] <AMP_%d> Under  : diff(%d/2),  ALC_ATT (%d/2) Set\n\n", 
					//	path4, diff_x2, ctl_att_x2);
				}
			}
			//////////////CtrlUnLock();
		}		
	}
	return att_set;
}
#endif
/*
-----------------------------------------------------
	Quick-ALC
	of
	Other Company RVS INPut
-----------------------------------------------------
*/

#define Q_Hystresis_Att_x2 (1*2) // 1dB
#ifdef __TYPE__AAU__
////#define __AAU_RVS_Quick_DEBUG__
INT8U Proc___AAU_RVS_Quick_ALC(INT8U path4)
{
	INT16S  ctl_att_x2; // 2020.01.07 : INT8S -> INT16S
	INT16S  cur_quick_att_x2; // 2020.01.07 : INT8S -> INT16S
	INT8U  ALC__Onoff;
	INT16S level_x10;
	INT16S rvs_out_pwr_x10;	
	INT16S diff_x2;	
	
#ifdef __AAU_RVS_Quick_DEBUG__
	INT8S  user_att_x2; // user control att	
#endif
	INT8S  max_alc_range_x2;		
	INT8U  att_set = 0;	
	INT8U  ctrl_prev = 0;
	//----------------------

	
	//----------------------	
	if(path4 >= AMPU_4_MAX) return 0;				
	//----------------------
	if(Check_PAU_Use(path4) == 0)
		return 0;
	
	//----------------------
	att_set = 0; // init
	ALC__Onoff = m_St.Rvs_IN_ALC_Onoff[path4];		
	////ALC__Onoff = m_Eep.Other_Rvs_IN_ALC_Onoff[path4];
		
	level_x10 = m_Eep.AFE_Rvs_ALC_x10[path4]; // 2019.06.13
	
	//----------------------
#ifdef __AAU_RVS_Quick_DEBUG__
	user_att_x2 = m_Eep.UL_Att[path4]/2; // ATNS-AAU-RF-0.25dB Unit				
#endif
	//----------------------
	cur_quick_att_x2 = m_St.RX_Qck_Att_x2[path4];

	max_alc_range_x2 = ALC_Range_x2 - m_St.RX_ALC_Att_x2[path4];
	if(max_alc_range_x2 < 0) max_alc_range_x2 = 0;	
	
	//----------------------
	// Other Campany Input Power -----------------------------------------------------
	rvs_out_pwr_x10 = Get_Other_Company_Det_x10(path4);	
	m_St.RSSI_Before_Filter_x10[path4] = rvs_out_pwr_x10 + m_Eep.Offset_RSSI_Before_Filter_x10[path4]; // 0xB302, RSSI(Filter 전)
	//-----------------------------------------------------
	if(m_Reg_Sts.RFU[path4].PA_OnOff.On == 0) // if OFF
		return 0;
	//-----------------------------------------------------
	if(ALC__Onoff == ON)
	{		
		diff_x2 = (rvs_out_pwr_x10 - level_x10)/5;
		att_set = 0; // init

		#ifdef __AAU_RVS_Quick_DEBUG__
		{			
			D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> -----\n", path4);					
			D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> Status : Out_Pwr (%d/10) - Limit (%d/10) = (%d/10), Range (%d/2)\n", 
					path4,					
					rvs_out_pwr_x10, 
					level_x10,
					(rvs_out_pwr_x10 - level_x10),
					max_alc_range_x2);
					
			D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> Status : User (%d/2) + quick_att (%d/2)\n", 
					path4, user_att_x2, cur_quick_att_x2);
		}
		#endif
		
		//------------------------------
		if(diff_x2 >= 0) // power >= level
		{	
			if(rvs_out_pwr_x10 == level_x10) 
				diff_x2 = 1; // 0.5dB
			
			/////////CtrlLock();
			
			ctrl_prev = cur_quick_att_x2;
			
			ctl_att_x2 = diff_x2 + cur_quick_att_x2;
			if(ctl_att_x2 > max_alc_range_x2) // max 20.0 dB
				ctl_att_x2 = max_alc_range_x2;
				
			m_St.RX_Qck_Att_x2[path4] = ctl_att_x2;
			AAU__Rvs_Alc_Att_Set(path4, ctl_att_x2, m_St.RX_ALC_Att_x2[path4]);
			
			if(ctrl_prev != ctl_att_x2) 
			{
				att_set = 1; // atten set			
			#ifdef __AAU_RVS_Quick_DEBUG__	
				D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> Over   : diff(%d/2),  quick_att (%d/2) Set\n\n", 
					path4, diff_x2, ctl_att_x2);
			#endif		
			}			
		
			////////CtrlUnLock();
		}	
		//----------------------------------------------------------
		// Pwr is under 
		// if 'hysterisis 2.0dB' and alc-only-att is above 0[dB](that is cur_att is above user att) 
		//     then atten release
		//----------------------------------------------------------				
		else if((diff_x2 <= (-Q_Hystresis_Att_x2)) && (cur_quick_att_x2 > 0)) // power < level
		{
			diff_x2 += Q_Hystresis_Att_x2; // 2.0[dB]
			
			if(diff_x2 == 0) diff_x2 = -1; // 0.5dB

			////////////CtrlLock();			
			
			ctrl_prev = cur_quick_att_x2;
			
			ctl_att_x2 = diff_x2 + cur_quick_att_x2;
			if(ctl_att_x2 < 0) 
				ctl_att_x2 = 0;			

			m_St.RX_Qck_Att_x2[path4] = ctl_att_x2;
			AAU__Rvs_Alc_Att_Set(path4, ctl_att_x2, m_St.RX_ALC_Att_x2[path4]);
			
			if(ctrl_prev != ctl_att_x2) 
			{
				att_set = 1; // atten set
			#ifdef __AAU_RVS_Quick_DEBUG__	
				D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> Under  : diff(%d/2),  quick_att (%d/2) Set\n\n", 
					path4, diff_x2, ctl_att_x2);
			#endif		
			}
	
			//////////////CtrlUnLock();
		}

		//if(att_set)
		/*
		{			
			D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> -----\n", path4);
			
					
			D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> Status : Out_Pwr (%d/10) - Limit (%d/10) = (%d/10), Range (%d/2)\n", 
					path4,					
					rvs_out_pwr_x10, 
					level_x10,
					(rvs_out_pwr_x10 - level_x10),
					max_alc_range_x2);
					
			D_Printf( DEBUG_STDBY, VT_YELLOW "[RVS-QUiCK] <AMP_%d> Status : User (%d/2) + quick_att (%d/2)\n\n", 
					path4, user_att_x2, ctl_att_x2);
		}
		*/
	}	
	//-----------------------------------------------------	
	return att_set; // changed or not
}


/*
----------------------------------------------------------
-	100M TSSI : A
-	100M DL Digital ATT(dB) : B

-	Alpha TSSI : C
-	Alpha DL Digital ATT(dB) : D
-   Temp ATT : F
-	Target PAR(dB) : E,  9.2dB(사용자 설정)


-  tssi[100] = 13000000 * 100 / 100 * payload_onoff(1 or 0)
-  tssi[A]   = 13000000 * BW  / 100 * payload_onoff(1 or 0) ; BW = 100,80,60,40,0
if payload off, tssi is 0

- BW_att = -10*log[13000000(tssi[100]+tssi[A])]
- L/S_att = 0

- Main  : 
tssi[100]*power(10,[20-B-F-BW_Att-L/S_att]/10)
- Alpha : 
tssi[A]*power(10,[20-D-F-BW_Att-L/S_att]/10) 

CFR threshold = round[ sqrt(Main + Alpha) *power(10, E/20) * 8594/5600]
----------------------------------------------------------
*/


#define TSSI_FIX (13000000.0)

INT16U Cal__AAU_BW_Path_Att_x10(INT8U path4)
{
	INT8U optic;
	double d_tssi[MAX_Optic];
	INT8U bw_a;
	INT8U payload_onoff[MAX_Optic];
	INT16U bw_att_x10;

	if(path4 >= MAX_DL) return 0;	

	for(optic = 0; optic < MAX_Optic; optic++)
	{
		if(m_St.d_Payload_OnOff[optic][path4]==OFF) 
			 payload_onoff[optic] = 0;
		else payload_onoff[optic] = 1;
	}
	bw_a = Get__BW_of_Alpha();	
	d_tssi[_100M] = 100 * TSSI_FIX/100 * payload_onoff[_100M];
	d_tssi[Alpha] = bw_a* TSSI_FIX/100 * payload_onoff[Alpha];

	bw_att_x10 = -100*log10(TSSI_FIX/(d_tssi[_100M] + d_tssi[Alpha]));
	
	return bw_att_x10;
}

typedef struct
{
	INT16S coeff;
	INT16S att_100;
	INT16S att___a[4]; // 40/60/80/100
}EQ_ATT;

#define EQ_MAX_Num 23

const static EQ_ATT EQ_Att_x10[EQ_MAX_Num] = 
{
	{ -5500,  25, { 34, 32, 29, 25}}, // 0
	{ -5000,  22, { 30, 28, 26, 22}}, 
	{ -4500,  20, { 27, 25, 23, 20}}, 
	{ -4000,  18, { 23, 22, 20, 18}},
	{ -3500,  15, { 20, 19, 17, 15}}, // 4
	{ -3000,  13, { 17, 16, 14, 13}}, 
	{ -2500,  11, { 14, 13, 12, 11}}, 
	{ -2000,   8, { 11, 10,  9,  8}}, 
	{ -1500,   6, {  8,  8,  7,  6}}, 
	{ -1000,   4, {  5,  5,  5,  4}}, // 9
	{ -500,    2, {  3,  2,  2,  2}}, 
	{    0,	   0, {  0,  0,  0,  0}}, 
	{  500,   -2, { -3, -2, -2, -2}}, 
	{ 1000,   -4, { -5, -5, -4, -4}}, 
	{ 1500,   -6, { -7, -7, -6, -6}}, // 14 
	{ 2000,   -8, {-10, -9, -8, -8}}, 
	{ 2500,  -10, {-12,-11,-10,-10}}, 
	{ 3000,  -11, {-14,-13,-12,-11}}, 
	{ 3500,  -13, {-16,-16,-14,-13}}, 
	{ 4000,  -15, {-18,-18,-16,-15}}, // 19 
	{ 4500,  -16, {-20,-20,-18,-16}}, 
	{ 5000,  -18, {-22,-21,-20,-18}}, 
	{ 5500,  -20, {-22,-23,-24,-20}} // 22
};

INT16S Get__EQ1_Att_x10(INT8U optic, INT8U path4)
{
	INT8U i, k=0;
	INT16S coeff;
	
	if(optic >= MAX_Optic) return 0;
	if(path4 >= MAX_DL) return 0;

	if(optic == Alpha)
	{
		if(IS_Alpha_Mhz_Existing()==0) // Alpha
			return 0;
	}

	coeff = m_St.d_EQ_Coeff_1_DL[optic][path4];

	for(i=0; i<EQ_MAX_Num; i++)
	{
		if(coeff <= EQ_Att_x10[i].coeff)
			break;		
	}
	if(i >= EQ_MAX_Num) i = EQ_MAX_Num - 1; // last

	if(optic == _100M)
		return EQ_Att_x10[i].att_100;
	else
	{
		switch(m_Eep.d_RE_Channel_BW[Alpha]) // Alpha
		{		
			case BW__40M : k = 0; break;
			case BW__60M : k = 1; break;
			case BW__80M : k = 2; break;
			case BW_100M : k = 3; break;

			default: return 0;
		}
		return EQ_Att_x10[i].att___a[k];
	}
}



static INT16S Aau_EQ_att_x10[MAX_Optic][MAX_DL];

// #define DBG_EN_AAU_DL_CFR_Adj

void Proc___AAU_DL_CFR_Adj(INT8U path4)
{
	INT16S temp_att_x10;
	INT8U optic;
	double d_cfr;
	double d_val[MAX_Optic];
	double d_att[MAX_Optic];
	double d_tssi[MAX_Optic];
	static INT16U cnt[MAX_DL] = {0,0,0,0};
	INT16S dl_att_x10; // can be negative value
	INT8U bw_a;
	
	INT8U payload_onoff[MAX_Optic];
	INT16U bw_att_x10;
	double d_PAR_x10;
	INT8U disp_req = 0;
	INT32U cfr_32;

	if(path4 >= MAX_DL) return;

	// 2019.09.17
	if(m_St.d_CFR_OnOff[path4] == OFF) 
	{
		cnt[path4] = 0;
		return;
	}

	cnt[path4]++;
	if(cnt[path4] % 10) return; // 10(every 1[s])
	if(cnt[path4] >= 50) cnt[path4] = 0; // init

#ifdef DBG_EN_AAU_DL_CFR_Adj
	if((m_Env.debug_path4 == path4)&&(cnt[path4] == 0))	
		disp_req = 1;
	else
		disp_req = 0;
#else
	disp_req = 0;
#endif
		

	for(optic = 0; optic < MAX_Optic; optic++)
	{
		if(m_St.d_Payload_OnOff[optic][path4]==OFF) 
			 payload_onoff[optic] = 0;
		else payload_onoff[optic] = 1;
	}

	d_tssi[_100M] = 100 * TSSI_FIX/100 * payload_onoff[_100M];
	bw_a = Get__BW_of_Alpha();	
	d_tssi[Alpha] = bw_a* TSSI_FIX/100 * payload_onoff[Alpha];
	bw_att_x10 = Cal__AAU_BW_Path_Att_x10(path4);
	
	if(disp_req)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "");
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[CFR] path(%d): d_tssi[_100M] %f, d_tssi[Alpha] %f", 
			path4, d_tssi[_100M], d_tssi[Alpha]);
	}	

	for(optic = 0; optic < MAX_Optic; optic++)
	{	
		temp_att_x10 = Get_AAU_DTU_Alpha_Temp_Att_x10(optic, path4, DL);	
					
		dl_att_x10 = 200 - bw_att_x10 - m_St.d_Att_Dig_DL_x10[optic][path4] - temp_att_x10;
		dl_att_x10 += m_St.Curr_LS_Gain_Bal_x10[optic][DL][path4]; // 2019.09.16, LS Gain Balance(AID 0xC062)
		dl_att_x10 -= Aau_EQ_att_x10[optic][path4]; // 2019.07.20
		
		d_att[optic] = (double)(dl_att_x10)/10.0; // att 음수 일 수 있음.			
		d_val[optic] = d_tssi[optic] * pow(10.0, d_att[optic]/10.0);
			
		if(disp_req)
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "---");
			print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[CFR FA%d.%d] d_att(%f) = 20 -bw_att(%f) -dig(%f) -temp(%f) + LS_Gain_Bal(%f) - eq1_att(%f)", 
				optic, path4, 
				d_att[optic],
				(double)bw_att_x10/10.0,
				(double)m_St.d_Att_Dig_DL_x10[optic][path4]/10.0,
				(double)temp_att_x10/10.0, 
				(double)m_St.Curr_LS_Gain_Bal_x10[optic][DL][path4]/10.0,
				(double)Aau_EQ_att_x10[optic][path4]/10.0);
				
			print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[CFR FA%d.%d] Payload(%d) x d_val(%f)", 
				optic, path4,  
				payload_onoff[optic], d_val[optic]);	
				
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[CFR FA%d.%d] d_val[%d] %f", 
				optic, path4, d_val[optic]);	
		}		
	}
	
	d_PAR_x10 = (double)(m_Eep.d_PAR_x10[path4] + m_Eep.d_CFR_Offset_x10[path4]); // 2019.09.16 : USE "CFR_Offset"
	d_cfr = sqrt(d_val[_100M] + d_val[Alpha]) * pow(10.0, d_PAR_x10/200.0) *8594.0/5600.0;		

	// 2020.02.19 : register limit value 0xffff 적용
	cfr_32 = (INT32U)d_cfr;
	if(cfr_32 > 0xffff) cfr_32 = 0xffff;
				
	m_Reg_Flg.CFR[path4].Level.uD32 = 1;
	m_Reg_Ctl.CFR[path4].Level.uD32 = cfr_32;
	
	if(disp_req)
	{
		print_agent(DEBUG_SYSTEM, VT_GREEN, "[CFR Path%d] sqrt(d_val[_100M] + d_val[Alpha]) %f",
			path4, sqrt(d_val[_100M] + d_val[Alpha]));
			
		print_agent(DEBUG_SYSTEM, VT_GREEN, "[CFR Path%d] pow(10.0, (double)m_Eep.d_PAR_x10[path4]/200.0) *4864.0/8259.0 %f",
			path4, pow(10.0, (double)m_Eep.d_PAR_x10[path4]/200.0)*4864.0/8259.0);

		print_agent(DEBUG_SYSTEM, VT_GREEN, "[CFR Path%d] d_cfr %f, CFR_Level %d",
				path4, d_cfr, cfr_32);
	}		
}
#endif




#ifdef __TYPE__AAU__
double AAU__Calc_filter_att(int *pattern_i,int *pattern_q, int coeff, int equ)
{
	double filter_coeff[3];

	if(equ > 1) return 0;
	
	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[AAU__Calc_filter_att] equ%d---", equ);

	if(equ == 0)
	{	
		filter_coeff[0] = coeff/32768.0;
		filter_coeff[1] = 1;
		filter_coeff[2] = -filter_coeff[0];
	}
	else
	{
		filter_coeff[0] = coeff/32768.0;
		filter_coeff[1] = 1;
		filter_coeff[2] = filter_coeff[0];
	}
	
	double rst_i[2048];
	double rst_q[2048];

	int i;
	double input_power;
	input_power = 0;

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "filter coeff : %f, %f, %f", 
			filter_coeff[0], filter_coeff[1], filter_coeff[2]);
	
	for(i=1;i<2047;i++)
	{
		input_power += pattern_i[i] * pattern_i[i] + pattern_q[i]*pattern_q[i];
	}

	if(equ == 0)
	{
		for(i=1;i<2047;i++)
		{
			rst_i[i] = -pattern_q[i-1] * filter_coeff[0] + pattern_i[i] * filter_coeff[1] - pattern_q[i+1] * filter_coeff[2];
			rst_q[i] =  pattern_i[i-1] * filter_coeff[0] + pattern_q[i] * filter_coeff[1] + pattern_i[i+1] * filter_coeff[2];
		}
	}
	else
	{
		for(i=1;i<2047;i++)
		{
			rst_i[i] = pattern_i[i-1] * filter_coeff[0] + pattern_i[i] * filter_coeff[1] + pattern_i[i+1] * filter_coeff[2];
			rst_q[i] = pattern_q[i-1] * filter_coeff[0] + pattern_q[i] * filter_coeff[1] + pattern_q[i+1] * filter_coeff[2];
		}
	}

	double out_power=0;
	for(i=1;i<2047;i++)
	{
		out_power += rst_i[i]*rst_i[i] + rst_q[i]*rst_q[i];
	}

	double equ_att;
	equ_att = -10*log10(out_power/input_power);
	//printf("filter att : %f, %f, %f\n",filter_att, out_power, input_power);

	return equ_att;
}
#endif

#ifdef __TYPE__AAU__

enum
{
	EQ_100 = 0,
	EQ__80, 
	EQ__60,
	EQ__40,
	Max_EQ
};
static int eq_pattern_i[Max_EQ][2048];
static int eq_pattern_q[Max_EQ][2048];

static INT8U eq_pattern_i_load[Max_EQ];
static INT8U eq_pattern_q_load[Max_EQ];


INT8U conv_BW_to_eq_bw(INT8U bw)
{
	INT8U eq_bw;
	
	switch(bw)
	{
		case BW__40M: eq_bw = EQ__40; break;
		case BW__60M: eq_bw = EQ__60; break;
		case BW__80M: eq_bw = EQ__80; break;
		case BW_100M: eq_bw = EQ_100; break;
		default     : eq_bw = Max_EQ; break;
	}
	return eq_bw;
}

int AAU___Loading_Get_EQ_Test_Pattern(INT8U bw)
{	
	FILE *i_fid,*q_fid;	
	int test_pattern_i[2048];
	int test_pattern_q[2048];	
	int i;
	INT8U eq_bw;
	
	
	if(bw == BW_100M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_100M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_100M_q.txt","r");
	}
	else if(bw == BW__80M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_80M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_80M_q.txt","r");
	}
	else if(bw == BW__60M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_60M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_60M_q.txt","r");
	}
	else if(bw == BW__60M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_40M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_40M_q.txt","r");
	}
	else return -1;

	eq_bw = conv_BW_to_eq_bw(bw);
	if(eq_bw == Max_EQ) return -1;
	
	if(i_fid == NULL || q_fid == NULL)
	{
		if(i_fid != NULL) fclose(i_fid);
		if(q_fid != NULL) fclose(q_fid);
		
		eq_pattern_i_load[eq_bw] = 0;
		eq_pattern_q_load[eq_bw] = 0;

		return -1;
	}	
	
	for(i=0;i<2048;i++)
	{
		fscanf(i_fid, "%d", &test_pattern_i[i]);
		fscanf(q_fid, "%d", &test_pattern_q[i]);
	}
	fclose(i_fid);
	fclose(q_fid);

	for(i=0;i<2048;i++)
	{
		eq_pattern_i[eq_bw][i] = test_pattern_i[i];
		eq_pattern_q[eq_bw][i] = test_pattern_q[i];
	}

	eq_pattern_i_load[eq_bw] = 1;
	eq_pattern_q_load[eq_bw] = 1;
	
	return 0;
}




INT16S AAU__Get_EQ_Att_x10(INT8U optic, INT8U path4)
{
	INT8U bw;
	int equ0_value, equ1_value;
	double fc,nco;
	
	if(optic >= MAX_Optic) return 0;
	if(path4 >= MAX_DL) return 0;

	if(optic == _100M)
	{
		fc = 3650.0;
		bw = BW_100M;
	}		
	else // Alpha
	{
		if(IS_Alpha_Mhz_Existing()==0)
			return 0;
			
		bw = m_Eep.d_RE_Channel_BW[Alpha];
		switch(m_Eep.d_RE_Channel_BW[Alpha]) // Alpha
		{		
			case BW__40M : fc = 3720.0; break;
			case BW__60M : fc = 3730.0; break;
			case BW__80M : fc = 3740.0; break;
			case BW_100M : fc = 3750.0; break;

			default: return 0;
		}		
	}
	
	equ0_value = m_St.d_EQ_Coeff_DL[optic][path4]  * m_St.EQ_Toggle[optic][DL][0];
	equ1_value = m_St.d_EQ_Coeff_1_DL[optic][path4]* m_St.EQ_Toggle[optic][DL][1];
	
	//----------------------------------------------
	
	int shifted_i[2048];
	int shifted_q[2048];
	int i;

	
	if(equ1_value > 32767)
		equ1_value = -65536 + equ1_value;

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[FA%d.Path%d] Fc : %f, BW : %d, eq_coeffi_0 : %d, eq_coeffi_1 : %d", 
			optic, path4,
			fc, bw, equ0_value, equ1_value);

//----------------------------------------------------------------
#ifdef __USE_Loading_AAU_EQ_Pattern__
//----------------------------------------------------------------
	INT8U eq_bw;
	
	if(optic == _100M)
		eq_bw = EQ_100;
	else
		eq_bw = conv_BW_to_eq_bw(m_Eep.d_RE_Channel_BW[Alpha]);


	if(eq_pattern_i_load[eq_bw] == 0 || eq_pattern_q_load[eq_bw] == 0)
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "EQ-Test-Pattern-File(%d) not Exist", eq_bw);
		return 0;
	}
		
	// test pattern frequency shift
	nco = fc - 3700;
	for(i=0; i<2048; i++)
	{
		shifted_i[i] = (int)(eq_pattern_i[eq_bw][i]*cos(2*M_PI*nco/245.76*i) - eq_pattern_q[eq_bw][i]*sin(2*M_PI*nco/245.76*i));
	    shifted_q[i] = (int)(eq_pattern_i[eq_bw][i]*sin(2*M_PI*nco/245.76*i) + eq_pattern_q[eq_bw][i]*cos(2*M_PI*nco/245.76*i));	
	}
//----------------------------------------------------------------	
#else	
//----------------------------------------------------------------
	FILE *i_fid,*q_fid;	
	int test_pattern_i[2048];
	int test_pattern_q[2048];
	
	if(bw == BW_100M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_100M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_100M_q.txt","r");
	}
	else if(bw == BW__80M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_80M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_80M_q.txt","r");
	}
	else if(bw == BW__60M)
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_60M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_60M_q.txt","r");
	}
	else // BW__40M
	{
		i_fid = fopen("/usr/app/sys.conf/equ_test_pattern_40M_i.txt","r");
		q_fid = fopen("/usr/app/sys.conf/equ_test_pattern_40M_q.txt","r");
	}

	if(i_fid == NULL || q_fid == NULL)
	{
		if(i_fid != NULL) fclose(i_fid);
		if(q_fid != NULL) fclose(q_fid);

		return 0;
	}

	for(i=0;i<2048;i++)
	{
		fscanf(i_fid,"%d", &test_pattern_i[i]);
		fscanf(q_fid,"%d", &test_pattern_q[i]);
		
	//	printf("%d %d\n", i,test_pattern_i[i]);
	}
	fclose(i_fid);
	fclose(q_fid);

	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "Load End : equ_test_pattern");

	// test pattern frequency shift
	nco = fc - 3700;
	for(i=0; i<2048; i++)
	{
		shifted_i[i] = (int)(test_pattern_i[i]*cos(2*M_PI*nco/245.76*i) - test_pattern_q[i]*sin(2*M_PI*nco/245.76*i));
	    shifted_q[i] = (int)(test_pattern_i[i]*sin(2*M_PI*nco/245.76*i) + test_pattern_q[i]*cos(2*M_PI*nco/245.76*i));	
	}
//----------------------------------------------------------------	
#endif	
//----------------------------------------------------------------

	double equ0_att,equ1_att,tot_equ_att;
	
	equ0_att = AAU__Calc_filter_att(shifted_i, shifted_q, equ0_value,0);
	equ1_att = AAU__Calc_filter_att(shifted_i, shifted_q, equ1_value,1);
	
	tot_equ_att = equ0_att + equ1_att;

	/*
	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN,"[EQU FA%d.Path%d] equ0/1 att : %f/%f, total_att : %f",
			optic, path4,	
			equ0_att, equ1_att, 
			tot_equ_att);
	*/
	print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN,"[EQU Tot FA%d.Path%d] %f",
			optic, path4,
			tot_equ_att);		

	return (INT16S)(tot_equ_att*10.0);
}
#endif


#ifdef __TYPE__AAU__
void Proc__AAU__Get_EQ_Att_x10(void)
{
	INT8U optic;
	INT8U path4;
	INT8U calc_req[MAX_Optic][MAX_DL];
	static INT8U BW_Alpha;
	static INT8U init = 0;
	static INT16S coeff_prev[MAX_Optic][MAX_DL][2];
	static INT8U  CFR_OnOff[MAX_DL];
	INT16S coeff[2]; 
	INT8U disp = 0;


	if(init == 0)
	{
		memset(calc_req, 1, 2*4); // MAX_Optic * MAX_DL;

		for(optic=0; optic < MAX_Optic; optic++)
		{
			for(path4=0; path4 < MAX_DL; path4++)
			{
				Aau_EQ_att_x10[optic][path4] = 0;
				
				coeff_prev[optic][path4][0] = m_St.d_EQ_Coeff_DL[optic][path4]  * m_St.EQ_Toggle[optic][DL][0];
				coeff_prev[optic][path4][1] = m_St.d_EQ_Coeff_1_DL[optic][path4]* m_St.EQ_Toggle[optic][DL][1];

				CFR_OnOff[path4] = m_St.d_CFR_OnOff[path4];
			}
		}
	#ifdef __USE_Loading_AAU_EQ_Pattern__
		AAU___Loading_Get_EQ_Test_Pattern(BW__40M);
		AAU___Loading_Get_EQ_Test_Pattern(BW__60M);
		AAU___Loading_Get_EQ_Test_Pattern(BW__80M);
		AAU___Loading_Get_EQ_Test_Pattern(BW_100M);
	#endif	
		BW_Alpha = m_Eep.d_RE_Channel_BW[Alpha];
		init = 1;
	}
	else
	{
		memset(calc_req, 0, 2*4); // MAX_Optic * MAX_DL;
	}

	
	for(optic=0; optic < MAX_Optic; optic++)
	{
		for(path4=0; path4 < MAX_DL; path4++)
		{			
			coeff[0] = m_St.d_EQ_Coeff_DL[optic][path4]  * m_St.EQ_Toggle[optic][DL][0];
			coeff[1] = m_St.d_EQ_Coeff_1_DL[optic][path4]* m_St.EQ_Toggle[optic][DL][1];
			
			if(coeff_prev[optic][path4][0] != coeff[0])
			{
				coeff_prev[optic][path4][0] = coeff[0];
				calc_req[optic][path4] = 1;
			}
			
			if(coeff_prev[optic][path4][1] != coeff[1])
			{
				coeff_prev[optic][path4][1] = coeff[1];
				calc_req[optic][path4] = 1;
			}
		}
	}		

	if(BW_Alpha != m_Eep.d_RE_Channel_BW[Alpha])
	{
		BW_Alpha = m_Eep.d_RE_Channel_BW[Alpha];
		
		for(path4=0; path4 < MAX_DL; path4++)
			calc_req[Alpha][path4] = 1;		
	}


	for(path4=0; path4 < MAX_DL; path4++)
	{
		if(CFR_OnOff[path4] != m_St.d_CFR_OnOff[path4])
		{
			if(m_St.d_CFR_OnOff[path4] == ON) // OFF -> ON
			{
				calc_req[_100M][path4] = 1;
				calc_req[Alpha][path4] = 1;
			}
			CFR_OnOff[path4] = m_St.d_CFR_OnOff[path4];
		}
	}

	for(optic=0; optic < MAX_Optic; optic++)
	{
		for(path4=0; path4 < MAX_DL; path4++)
		{			
			if(calc_req[optic][path4])
			{
				// 2019.09.17
				if(m_St.d_CFR_OnOff[path4] == ON) 
				{				
					Aau_EQ_att_x10[optic][path4] = AAU__Get_EQ_Att_x10(optic, path4);
					disp = 1;
				}
			}
		}
	}

	if(disp)
	{	
		for(optic=0; optic < MAX_Optic; optic++)
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "EQ_Att_x10 : FA%d : %3d/10, %3d/10, %3d/10, %3d/10",
				optic,
				Aau_EQ_att_x10[optic][0], 
				Aau_EQ_att_x10[optic][1], 
				Aau_EQ_att_x10[optic][2], 
				Aau_EQ_att_x10[optic][3]);
		}
	}
}
#endif







//------------------------------------------------------------------
/*
------------------------------------------------------------------------
	RVS-IN & FWD-IN
	Shutdown
	Algorithm 
---------------------------------------------------------------------------
*/

static OS_TIMER SD_Timer[AMPU_4_MAX];
static OS_TIMER SD_Off_Timer[AMPU_4_MAX];

static INT8U  SD_Cnt[AMPU_4_MAX];
static INT8U  SD_Sts[AMPU_4_MAX];
static INT8U  Pwr_Arm_St[AMPU_4_MAX];

#ifdef __TYPE__AAU__
// Start is 5[s]
static INT32U SD_RE_TIME_ms[5] = {5*TICK_CO_SEC, 5*TICK_CO_SEC, 5*TICK_CO_SEC, 5*TICK_CO_SEC, 5*TICK_CO_SEC}; 
#endif

/*
-----------------------------------------------------
-----------------------------------------------------
*/
void AAU_RVS_SD_Init_Ctrl(INT8U path4)
{
	if(path4 >= AMPU_4_MAX) return;

	if(m_St.AlmTest) // 2019.05.02
		return;	
	
	OSTimerReset(&SD_Timer[path4]);
	OSTimerReset(&SD_Off_Timer[path4]);

	SD_Cnt[path4] = 0; 
	SD_Sts[path4] = 0;
	Pwr_Arm_St[path4] = 0;	
	
	m_Env.Fwd_In_Over[path4] = 0;
	m_Env.Rvs_In_Over[path4] = 0;

	Rvs_Amp_SD[path4] &= (~AMP_REV_IN);		

	/*	
	when algorithm is restarted, Shutdown is cleared.
	*/	
	if(m_St.AlmSt.Path[path4].UL.Over_Input) // AAU
		m_Env.AAU_UL_Over_Input_Clr_Req = 1;
	
	if(Get__Reason_SD_5115_Ori(path4) == Rsn_SD_UL_OverIn) // 2019.12.26 : AAU
		m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
		
	if(m_St.Pwr_Amp_Off_Reason_PAU[path4] == rsn_RVS)
		m_St.Pwr_Amp_Off_Reason_PAU[path4] = 0;
}
//-----------------------------------------------------
enum // shutdown step
{
	SD_PWR_CHK = 0,
	SD_AMP_OFF,
	SD_30_min,
	SD_Total,
	
	SD_Max
};

/*
------------------------------------------------------------------------
	Shutdown Algorithm 
	@ 5G
	-	DL와 UL Path 중 하나의 Path에서 Shutdown 조건이 발생하는 경우에는 
	양쪽 Path를 동시에 Shutdown
---------------------------------------------------------------------------
*/


//-------------------------------------------
#ifdef __AAU_Shutdown__Use_Test_Time__
#define _SD_30min_Time_ (TICK_CO_SEC*60) // 60[s]
#else
#define _SD_30min_Time_ (TICK_CO_MIN*30) // 30[Min]
#endif
//-------------------------------------------


static INT8U init_SD_req[AMPU_4_MAX] = {1,1,1,1};	

void SD_Init_Req(INT8U path4)
{
	init_SD_req[path4] = 1;
}

void AAU_FWD_SD_Init_Req(INT8U path4)
{
	if(m_St.Pwr_Amp_Off_Reason_PAU[path4] == rsn_Over_Pwr)
		m_St.Pwr_Amp_Off_Reason_PAU[path4] = 0;	
}


INT8U Is__Rvs_SD_Total(INT8U path4)
{
//-----------------
#ifdef __TYPE__AAU__
//-----------------
	if(path4 >= AMPU_4_MAX) return 0;
	
	if(SD_Sts[path4] == SD_Total)
		return 1;
	else
		return 0;
//-----------------
#else // PRU
//-----------------
	(void)path4;
	return 1;
//-----------------	
#endif
//-----------------
}



#ifdef __TYPE__AAU__
void AAU_Ctrl__UL_Over_Input(INT8U path4, INT8U alarm)
{
	if(m_St.AlmSt.Path[path4].UL.Over_Input != alarm)
	{
		Update_Alm_Str_Lmt( 0x52, 0x10, path4, 0, m_St.AlmSt.Path[path4].UL.Over_Input); // UL Over IN, AAU
		
		if(alarm)
			D_Printf(DEBUG_STDBY, VT_BOLD_RED "[UL Over IN] <AMP_%d> AAU_TotPwr_x10 %d\n", 
				path4, Get__AAU_TotPwr_x10_UL_RSSI(path4));
		else
			D_Printf(DEBUG_STDBY, VT_BOLD_GREEN "[UL Over IN] <AMP_%d> AAU_TotPwr_x10 %d\n", 
				path4, Get__AAU_TotPwr_x10_UL_RSSI(path4));
	}

	m_St.AlmSt.Path[path4].UL.Over_Input = alarm; // AAU
	m_St.AlmSt_prev.Path[path4].UL.Over_Input = m_St.AlmSt.Path[path4].UL.Over_Input; // AAU, 무의미.
}
#endif

void Proc___AAU_Rvs_Shutdown(INT8U path4)
{
#ifdef __TYPE__PRU__
	(void)path4;
#else /// AAU 
	INT16S Out_Pwr_x10;
	INT16S SD__Lvl_x10;	// shutdown level
	//static int disp_cnt[AMPU_4_MAX] = {0,0,0,0};
	
	//INT8U prev_Alm_St;

	//-------------------------------------------
	if(m_St.AlmTest) // 2019.05.02
		return;	
	
	//-------------------------------------------
	if(path4 >= AMPU_4_MAX) return;	
	//-------------------------------------------	
	if((m_Eep.Auto_SD_Mode_En[path4] == OFF)||(m_Eep.Pwr_Amp_En[path4] == OFF))
	{	
		init_SD_req[path4] = 1; // for algorithm restart				
	}
	//-------------------------------------------
	if(m_Env.AAU_UL_Over_Input_Clr_Req)
	{
		if(OSTimerGet_msec(&SD_Off_Timer[path4])>= (TICK_CO_SEC*5))
		{
			OSTimerReset(&SD_Off_Timer[path4]);	
			m_Env.AAU_UL_Over_Input_Clr_Req = 0;
			
			if(m_St.AlmSt.Path[path4].UL.Over_Input) // AAU
				AAU_Ctrl__UL_Over_Input(path4, _CLR_);

			// 2019.12.26 : AAU
			if(m_St.AlmSt.Path[path4].PAU.Shutdown)
			{
				if(Get__Reason_SD_5115_Ori(path4) == Rsn_SD_UL_OverIn) // AAU
					m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
			}	
		}
	}	
	//-------------------------------------------
	if(init_SD_req[path4] == 1)	
	{
		init_SD_req[path4] = 0;
		AAU_Ctrl__UL_Over_Input(path4, _CLR_);
		AAU_RVS_SD_Init_Ctrl(path4); // for algorithm restart
		return;
	}		
	// FWD -------------------------------------------
	/*	
	//Out_Pwr_x10 = m_St.d_DL_FB_RSSI_Tot_x10[path4];
	//SD__Lvl_x10 = m_St.Rvs_IN_SD_x10[path4];	 // + 3dB ?? 
	//prev_Alm_St = Pwr_Arm_St[path4] & cbits[_FWD_];	
	//if(Out_Pwr_x10 >= SD__Lvl_x10)
	//	Pwr_Arm_St[path4] |= cbits[_FWD_];	
	//else if(Out_Pwr_x10 <= (SD__Lvl_x10 - 20)) // hysteresys 2.0[dB]	
	//	Pwr_Arm_St[path4] &= nbits[_FWD_];		
	//if(prev_Alm_St != (Pwr_Arm_St[path4] & cbits[_FWD_]))
	//{
	//	D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> FWD : Out_Pwr_x10 (%d/10) limit (%d/10).\n", 
	//		path4, Out_Pwr_x10, SD__Lvl_x10);	
	//}		
	*/
	// RVS -------------------------------------------	

	//-------------------------------------------
	// 2019.05.04	
	if(m_St.Rvs_IN_ALC_Onoff[path4] == ON)
	{
		if(m_St.Pwr_Amp_En[path4] == ON)
		{			
			if((m_St.RX_ALC_Att_x2[path4] + m_St.RX_Qck_Att_x2[path4]) < ALC_Range_x2) 
			{
				return;
			}
		}
	}
	//-------------------------------------------	
	Out_Pwr_x10 = Get__AAU_TotPwr_x10_UL_RSSI(path4);
	SD__Lvl_x10 = m_St.Rvs_IN_SD_x10[path4]; 

	//prev_Alm_St = Pwr_Arm_St[path4] & cbits[_RVS_];

	if(Out_Pwr_x10 >= SD__Lvl_x10)
		Pwr_Arm_St[path4] |= cbits[_RVS_];	
	else if(Out_Pwr_x10 <= (SD__Lvl_x10 - 20)) // hysteresys 2.0[dB]	
		Pwr_Arm_St[path4] &= nbits[_RVS_];

/*
	if(prev_Alm_St != (Pwr_Arm_St[path4] & cbits[_RVS_]))
	{
		D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> RVS : Out_Pwr_x10 (%d/10) limit (%d/10).\n", 
			path4, Out_Pwr_x10, SD__Lvl_x10);	
	}	
*/	

	//-------------------------------------------
	switch(SD_Sts[path4])
	{
		//-------------------------------------------
		// Check Now Normal or not
		// If over-power, increse shutdown count and Amp Off.
		//-------------------------------------------
		case SD_PWR_CHK :
			if(Pwr_Arm_St[path4] == 0) // if all normal
			{				
				OSTimerReset(&SD_Timer[path4]);

				// if previous Shutdown count exist 
				// and normla is over 10[s], then Normal.
				
				if((SD_Cnt[path4] > 0) && (OSTimerGet_msec(&SD_Off_Timer[path4])>= (TICK_CO_SEC*5))) // 5
				{
					OSTimerReset(&SD_Off_Timer[path4]);					
					
					SD_Cnt[path4] = 0; // count reset					
					m_Env.Fwd_In_Over[path4] = 0; // init
					m_Env.Rvs_In_Over[path4] = 0; // init

					D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> to Normal : SD Cnt 0, TotPwr_x10 %d.", 
							path4, Get__AAU_TotPwr_x10_UL_RSSI(path4));	

					// 2019.12.26 : if normal, clear alarm : AAU
					if(m_St.AlmSt.Path[path4].UL.Over_Input) // AAU
						AAU_Ctrl__UL_Over_Input(path4, _CLR_);
					
					// 2019.12.26 : AAU
					if(m_St.AlmSt.Path[path4].PAU.Shutdown)
					{
						if(Get__Reason_SD_5115_Ori(path4) == Rsn_SD_UL_OverIn) // AAU
							m_St.AlmSt.Path[path4].PAU.Shutdown = 0;
					}
				}

				/*
				//if((SD_Cnt[path4] > 0) && (OSTimerGet_msec(&SD_Off_Timer[path4])< (TICK_CO_SEC*5))) // 5
				//{
				//	disp_cnt[path4]++;
				//	if(disp_cnt[path4]%5 == 0)
				//		D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> TotPwr_x10 %d.", 
				//				path4, Get__AAU_TotPwr_x10_UL_RSSI(path4));	
				//}
				*/
			}
			else if(OSTimerGet_msec(&SD_Timer[path4]) >= SD_RE_TIME_ms[SD_Cnt[path4]])
			{
				/*
					Now if Over-Power
					try to amp-off if 'Amp-On' time is over
					SD_Count[i] : 0 ~ 4
				*/
				OSTimerReset(&SD_Timer[path4]);
				
				SD_Sts[path4] = SD_AMP_OFF; // mode is 1				
				SD_Cnt[path4]++; // next count..

				/*
				//if(Pwr_Arm_St[path4] & cbits[_FWD_])
				//{
				//	m_Env.Fwd_In_Over[path4] = 1; // 1 (shutdown)
				//	m_St.Pwr_Amp_Off_Reason_PAU[path4] = rsn_Over_Pwr;
				//	Set__Reason_SD_5115(m, Rsn_SD_Over__Pwr); // 2019.12.26 : AAU
				//}
				*/
				
				if(Pwr_Arm_St[path4] & cbits[_RVS_])
				{
					m_Env.Rvs_In_Over[path4] = 1; // 1 (shutdown)
					m_St.Pwr_Amp_Off_Reason_PAU[path4] = rsn_RVS;
					Set__Reason_SD_5115(path4, Rsn_SD_UL_OverIn_Skip); // 2019.12.26 : AAU

					Rvs_Amp_SD[path4] |= AMP_REV_IN;

					if(SD_Cnt[path4] == 1) // 2019.12.26 : if 1st Off : AAU
						AAU_Ctrl__UL_Over_Input(path4, _SET_);
				}
				//----------------------------------------
				    if( SD_Cnt[path4] >= 4) SD_Sts[path4] = SD_Total; // Total SD
				else if(SD_Cnt[path4] >= 3) SD_Sts[path4] = SD_30_min; // 30[min] SD
				//----------------------------------------
				
				//----------------------------------------
				/*
				//if(Pwr_Arm_St[path4] & cbits[_FWD_])
				//	D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> Shutdown By TX (cnt %d)\n\n", 
				//		path4, SD_Cnt[path4]);	
				*/
					
				if(Pwr_Arm_St[path4] & cbits[_RVS_])	
					D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> Shutdown By RX (cnt %d)\n\n", 
						path4, SD_Cnt[path4]);
				//----------------------------------------
				     if(SD_Sts[path4] == SD_Total)  D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> Total Shutdown.\n\n\n", path4);
				else if(SD_Sts[path4] == SD_30_min) D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> 30min Shutdown.\n\n\n", path4);				
				//----------------------------------------	
			}
			break;

		//-------------------------------------------
		// Check Amp Off is over 5[s]
		// try to Amp ON
		//-------------------------------------------
		case SD_AMP_OFF :			
			if( m_Env.Rvs_In_Over[path4]) Pwr_Arm_St[path4] |= cbits[_RVS_];
			////else if( m_Env.Fwd_In_Over[path4]) Pwr_Arm_St[path4] |= cbits[_FWD_];		

			if(OSTimerGet_msec(&SD_Timer[path4]) >= (TICK_CO_SEC*10)) // 2019.05.15 : 5[s] -> 10[s]
			{
				OSTimerReset(&SD_Timer[path4]);
				OSTimerReset(&SD_Off_Timer[path4]);
				
				SD_Sts[path4] = SD_PWR_CHK;
								
				if(m_St.Pwr_Amp_Off_Reason_PAU[path4] == rsn_RVS) // 2019.09.11
					m_St.Pwr_Amp_Off_Reason_PAU[path4] = 0;

				if(m_Env.Rvs_In_Over[path4])
				{
					// clear alarm
					Rvs_Amp_SD[path4] &= (~AMP_REV_IN);
					//////// m_St.AlmSt.Path[path4].UL.Over_Input = 0; // 2019.12.26 delete : AAU
				}

				D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> AMP ON. (cnt %d)\n\n\n", path4, SD_Cnt[path4]);	
			}
		break;

		//-------------------------------------------
		// Check Amp Off is over 30[minute]
		// try to Amp ON
		//-------------------------------------------
		case SD_30_min :			
			if(m_Env.Rvs_In_Over[path4]) Pwr_Arm_St[path4] |= cbits[_RVS_];
			//else if(m_Env.Fwd_In_Over[path4]) Pwr_Arm_St[path4] |= cbits[_FWD_];		
			
			if(OSTimerGet_msec(&SD_Timer[path4]) >= _SD_30min_Time_)
			{
				OSTimerReset(&SD_Timer[path4]);
				OSTimerReset(&SD_Off_Timer[path4]);

				SD_Sts[path4] = SD_PWR_CHK;

				if(m_St.Pwr_Amp_Off_Reason_PAU[path4] == rsn_RVS) // 2019.09.11
					m_St.Pwr_Amp_Off_Reason_PAU[path4] = 0;
				
				D_Printf( DEBUG_STDBY, "[TxRx_SD] <AMP_%d> AMP OFF 30min Over.\n\n\n", path4);	
			
				
				// clear alarm
				Rvs_Amp_SD[path4] &= (~AMP_REV_IN);
				//////m_St.AlmSt.Path[path4].UL.Over_Input = 0; // 2019.12.26 delete : AAU
			}
			else
			{	
				if(m_Env.Rvs_In_Over[path4])
				{				
					Rvs_Amp_SD[path4] |= AMP_REV_IN;
					/////m_St.AlmSt.Path[path4].UL.Over_Input = 1;  // 2019.12.26 delete : AAU
				}
				
				// 2019.12.26 : shutdown is @ 30m interval : AAU
				m_St.AlmSt.Path[path4].PAU.Shutdown = 1;
				Set__Reason_SD_5115(path4, Rsn_SD_UL_OverIn); // 2019.12.20, AAU
			}
			break;

		//-------------------------------------------
		// Total Shutdown 
		// Until User-AMP Controll is done
		//-------------------------------------------
		case SD_Total :			
			////if( m_Env.Fwd_In_Over[path4]) m_Env.Fwd_In_Over[path4] = 2; // total shutdown(2)
			if( m_Env.Rvs_In_Over[path4]) m_Env.Rvs_In_Over[path4] = 2; // total shutdown(2)
			break;
    }
#endif	 
}
//============================================================================


/*
---------------------------------------------------------------------------
2019.05.25 : shutdown off 만 하고, ON 하지 않음.
	shutdown,  when over shutdown-level and over max-fwd-alc-att(15dB)
---------------------------------------------------------------------------
*/
void Proc___AAU_Fwd_Shutdown(INT8U path4)
{
//	INT16S Out_Pwr_x10;
	INT16S SD__Lvl_x10;	// shutdown level
	INT16S FB__Pwr_x10;

	//-------------------------------------------
	if(path4 >= AMPU_4_MAX) return;	
	//-------------------------------------------
	if(m_St.AlmTest) // 2019.05.01
		return;	
	//-------------------------------------------
	// if user AMP-Off controlled
	/*
	D4_Printf(DEBUG_STDBY, path4, "[TxRx_SD] <AMP_%d> RVS : Auto_SD_Mode_En (%d) Pwr_Amp_En (%d).\n", 
			path4, 
			m_Eep.Auto_SD_Mode_En[path4],
			m_Eep.Pwr_Amp_En[path4]);
	*/
	//-------------------------------------------
	if((m_Eep.AAU_Fwd_SD__Onoff[path4] == OFF)
	 ||(m_Eep.Pwr_Amp_En[path4] == OFF))
	{		
		if(m_St.Pwr_Amp_Off_Reason_PAU[path4] == rsn_Over_Pwr)
			m_St.Pwr_Amp_Off_Reason_PAU[path4] = 0;
		return;
	}
	//-------------------------------------------
	//Out_Pwr_x10 = Get__AAU_TotPwr_x10_DL_TSSI(path4);
	FB__Pwr_x10 = m_St.d_DL_FB_RSSI_Tot_x10[path4];
	SD__Lvl_x10 = m_Eep.AAU_Fwd_SD__x10[path4]; // 2019.05.13
	//-------------------------------------------
	if((FB__Pwr_x10 >= SD__Lvl_x10) && (m_St.TX_ALC_Att_x10[path4] >= (15*10)))
	{	
		if(m_St.Pwr_Amp_Off_Reason_PAU[path4] != rsn_Over_Pwr)
		{
			m_St.Pwr_Amp_Off_Reason_PAU[path4] = rsn_Over_Pwr;
			Set__Reason_SD_5115(path4, Rsn_SD_Over__Pwr); // 2019.12.26 : AAU
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[FWD_SD] <AMP_%d> Rsn_SD_Over__Pwr\n", path4);

			if(m_Env.debug_path4 == path4)
			{			
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[FWD_SD] <AMP_%d> Off, FB__Pwr_x10 (%d/10) limit (%d/10), DL ALC %d/10 dB.\n", 
					path4, 
					FB__Pwr_x10, 
					SD__Lvl_x10, 
					m_St.TX_ALC_Att_x10[path4]);
			}
		}
	}
	/*
	---------------------------------------------------------------------------
	2019.05.25 : shutdown off 만 하고, ON 하지 않음.
	---------------------------------------------------------------------------	
	else if(FB__Pwr_x10 <= (SD__Lvl_x10 - 10)) // hysteresys 1.0[dB]
	{
		if(m_St.Pwr_Amp_Off_Reason_PAU[path4] != 0)
		{
			m_St.Pwr_Amp_Off_Reason_PAU[path4] = 0; // rsn_Over_Pwr;			

			D_Printf( DEBUG_STDBY, "[FWD_SD] <AMP_%d> ON, FB__Pwr_x10 (%d/10) limit (%d/10).\n", 
					path4, FB__Pwr_x10, SD__Lvl_x10);
		}
	}
	*/
}


/*
----------------------------------------------------------
AAU FWD ALC
----------------------------------------------------------
*/
//#define __AAU_FWD_ALC_DEBUG__
INT8U Proc___AAU_FWD_ALC(INT8U path4)
{
	INT16S ctl_att_x2;
	INT8U  ALC__Onoff;
	INT16S level_x10;

	INT16S out_pwr_x10;	
	INT16S diff_x2;	
#ifdef __AAU_FWD_ALC_DEBUG__	
	INT8S  user_att_x2; // user control att	
#endif
	INT8S  max_alc_range_x2;

	INT8U  att_set = 0;	
	INT8U  ctrl_prev = 0;

	INT8S  cur_alc_att_x2;
	//----------------------

	
	//----------------------	
	if(path4 >= AMPU_4_MAX) 
		return 0;
	if(Check_PAU_Use(path4) == 0)
		return 0;	
	if(m_Reg_Sts.RFU[path4].PA_OnOff.On == 0) // if OFF
		return 0;			
	//----------------------	
	ALC__Onoff = m_Eep.AAU_Fwd_ALC_Onoff[path4]; // 2019.05.13
	level_x10  = m_Eep.AAU_Fwd_ALC_x10[path4]; // 2019.05.13
#ifdef __AAU_FWD_ALC_DEBUG__	
	user_att_x2 = m_Eep.DL_Att_User[path4]/2; // ATNS-AAU-RF-0.25dB Unit		
#endif

	max_alc_range_x2 = ALC_Range_x2;
	if(max_alc_range_x2 < 0) max_alc_range_x2 = 0;
	//----------------------
	
	cur_alc_att_x2 = m_St.TX_ALC_Att_x10[path4]/5;
	out_pwr_x10 = m_St.d_DL_FB_RSSI_Tot_x10[path4];	
	if(cur_alc_att_x2 < 0)
	{		
		cur_alc_att_x2 = 0;
		
		D_Printf( DEBUG_STDBY, VT_RED "[FWD-ALC] <AMP_%d> Abnormal   : ALC_Att < 0 \n", path4);	
	}	
	//-----------------------------------------------------
	if(ALC__Onoff == ON)
	{		
		diff_x2 = (out_pwr_x10 - level_x10)/5;
		att_set = 0; // init
		
		#ifdef __AAU_FWD_ALC_DEBUG__
		{			
			D_Printf( DEBUG_STDBY, "[FWD-ALC] <AMP_%d> --------\n", path4);
			D_Printf( DEBUG_STDBY, "[FWD-ALC] <AMP_%d> Status : Out_Pwr (%d/10) - Limit (%d/10) = (%d/10), Max_Range (%d/2)\n", 
					path4,					
					out_pwr_x10, 
					level_x10,
					(out_pwr_x10 - level_x10),
					max_alc_range_x2);
					
			D_Printf( DEBUG_STDBY, "[FWD-ALC] <AMP_%d> Status : User (%d/2) + RF-ALC (%d/2)\n\n", 
					path4,
					user_att_x2, cur_alc_att_x2);					
		}
		#endif
		
		//------------------------------
		if(diff_x2 >= 0) // power >= level
		{	
			if(out_pwr_x10 == level_x10) 
				diff_x2 = 1; // 0.5dB
			
			/////////CtrlLock();			
				
			ctrl_prev = cur_alc_att_x2;
			
			ctl_att_x2 = diff_x2 + cur_alc_att_x2;
			if(ctl_att_x2 > max_alc_range_x2) // max 20.0 dB
				ctl_att_x2 = max_alc_range_x2;
			
			AAU__Fwd_Alc_Att_Set(path4, ctl_att_x2);
			
			if(ctrl_prev != ctl_att_x2) 
			{
				att_set = 1; // atten set
			#ifdef __AAU_FWD_ALC_DEBUG__
				D_Printf( DEBUG_STDBY, "[FWD-ALC] <AMP_%d> Over   : diff(%d/2),  ALC_ATT (%d/2) Set.\n\n", 
					path4, diff_x2, ctl_att_x2);	
			#endif		
			}
			////////CtrlUnLock();
		}	
		//----------------------------------------------------------
		// Pwr is under 
		// if 'hysterisis 2.0dB' and alc-only-att is above 0[dB](that is cur_att is above user att) 
		//     then atten release
		//----------------------------------------------------------
		else if(diff_x2 <= (-Hystresis_Att_x2)) // power < level
		{
			diff_x2 += Hystresis_Att_x2; // 2.0[dB]
			
			if(diff_x2 == 0) diff_x2 = -1; // 0.5dB			

			////////////CtrlLock();
	
			ctrl_prev = cur_alc_att_x2;
			
			ctl_att_x2 = diff_x2 + cur_alc_att_x2;
			if(ctl_att_x2 < 0) 
				ctl_att_x2 = 0;			
			
			AAU__Fwd_Alc_Att_Set(path4, ctl_att_x2);
			
			if(ctrl_prev != ctl_att_x2) 
			{
				att_set = 1; // atten set
			#ifdef __AAU_FWD_ALC_DEBUG__	
				D_Printf( DEBUG_STDBY, "[FWD-ALC] <AMP_%d> Under  : diff(%d/2),  ALC_ATT (%d/2) Set\n\n", 
					path4, diff_x2, ctl_att_x2);
			#endif		
			}
			//////////////CtrlUnLock();
		}		
	}
	return att_set;
}




/*
-------------------------------------------------------------------
2019.11.28 : Algorithm for Getting UL_Gain_Low_Offset 
-------------------------------------------------------------------
*/

INT8U IS__End_of_UL_Gain_Low_Offset(INT8U path4)
{
	if(path4 >= AMPU_4_MAX) return 0;
	
	if(m_Eep.f_UL_Gain_Low_Offset[path4] == 'F') // if end('F')
		return 1;
	else
		return 0;
}

void Set__End_of_UL_Gain_Low_Offset(INT8U path4)
{
	if(path4 >= AMPU_4_MAX) return;
	
	m_Eep.f_UL_Gain_Low_Offset[path4] = 'F'; // end('F')
	m_Env.is_eep_write = EEP_WRITE_CODE;
}

void Disp__AAU_GC_RSSI_RMS(INT8U path4)
{
	// Display GC_RSSI_RMS when UL.Gain.Check.Mode	
	static int co[AMPU_4_MAX] = {0,0,0,0};	

	if(path4 >= AMPU_4_MAX) return;

	if(m_Reg_Sts.Gain_Check_Mode.onoff)
	{		
		if(path4 == 0)
			print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[100M.Path_%d] (%d) GC_RSSI_RMS_x10 : %d/10",
						path4,
						co[path4],
						m_St.d_R_GC_RSSI_RMS_x10[_100M][path4]); 
		else
			print_agent(DEBUG_SYSTEM, VT_BOLD_WHITE, "[100M.Path_%d] (%d) GC_RSSI_RMS_x10 : %d/10",
						path4,
						co[path4],
						m_St.d_R_GC_RSSI_RMS_x10[_100M][path4]);
					
		co[path4]++;
		/*		
		print_agent(DEBUG_SYSTEM, VT_BOLD_CYAN, "[100M GC_RSSI_RMS_x10, %2d] %d/10, %d/10, %d/10, %d/10",
					co[path4],
					m_St.d_R_GC_RSSI_RMS_x10[_100M][0],
					m_St.d_R_GC_RSSI_RMS_x10[_100M][1],
					m_St.d_R_GC_RSSI_RMS_x10[_100M][2],
					m_St.d_R_GC_RSSI_RMS_x10[_100M][3]); 
		*/			
	}
	else
	{
		co[path4] = 0;
	}
}

enum
{
	Offset_St__None = 0,
	Offset_St__Delay,
	Offset_St__Chk_RSSI,  // "m_St.d_R_GC_RSSI_RMS_x10 < 25" 10회 조건 Check
	Offset_St__Sampling,  // calculate offset 5-candidate
	Offset_St__Chk_Range, // Check difference of max-candidate and min-candidate
	Offset_St__Chk_Retry, // Check retry count
	Offset_St__End,       // Set "Flag End"
	Offset_St__Exit,      // "Already Flag End" or "10-Retry End" or "Success"
	
	Offset_St__Max	
};

static char* str_Offset_St[Offset_St__Max] = 
{
	// enum 순서대로 되어야 함.!!!!!!!!!!!
	"St__None",
	"St__Chk_RSSI",
	"St__Sampling",
	"St__Chk_Range",
	"St__Chk_Retry",
	"St__End",
	"St__Exit",
};



#define Chk_RSSI_Max (4) // GC_RSSI_RMS Range Check for algorithm, 10 -> 4
#define GC_Offset_Sample_Max (4) // 2019.12.09 : 5 -> 4
#define GC_RSSI_Retry_Max (5) // 2019.12.11 : 10 -> 5

static OS_TIMER timer_sample[AMPU_4_MAX];

static INT8U ul_gain_st[AMPU_4_MAX] = {0,0,0,0}; // init "Offset_St__None(0)"

INT8U Proc___AAU_UL_Gain_Low_Offset(INT8U path4, INT8U* pSkipSt)
{	
	static INT8U cnt[AMPU_4_MAX] = {0,0,0,0}; // ~ GC_Offset_Sample_Max
	static INT16S sample[AMPU_4_MAX][GC_Offset_Sample_Max];	
	static INT8U st_pre[AMPU_4_MAX] = {0,0,0,0};
	static INT8U retry[AMPU_4_MAX] = {0,0,0,0}; // retry if fail
	static INT8U rss_low[AMPU_4_MAX]; // low rssi cnt
	static INT8U mode_prev[AMPU_4_MAX] = {0,0,0,0};
	
	INT16S RSSI_RMS_x10; // current value
	INT16S max_x10, min_x10;
	INT16S sum_x10, val_x10, avg_x10;
	INT8U  i;
	INT8U  skip = 0;
	INT8U cpri_lock_st;
	INT8U cur_cnt;	

	if(path4 >= AMPU_4_MAX) return Offset_St__None;
	
	//-----------------------------------------------------
	RSSI_RMS_x10 = m_St.d_R_GC_RSSI_RMS_x10[_100M][path4];	
	cpri_lock_st = Get_Instant_CPRI_Lock_St();
	skip = 0; // init with 0
	
	if(Is_SFP_Inserted(CPRI_0) == 0)               skip |= cbits[0];
	if((cpri_lock_st & cbits[CPRI_0]) == 0)        skip |= cbits[1];
	if(m_Reg_Sts.RFU[path4].PA_OnOff.On == 0)      skip |= cbits[2];
	if(m_St.RU_Init_Status != Init_St___Completed) skip |= cbits[3];
	if(m_Reg_Sts.Gain_Check_Mode.onoff == 0)       skip |= cbits[4]; // if not gain_check_mode

	*pSkipSt = skip;
	
	if(skip)
	{
		ul_gain_st[path4] = Offset_St__None;
		mode_prev[path4] = m_Reg_Sts.Gain_Check_Mode.onoff;		
		return ul_gain_st[path4];
	}

	// if mode OFF --> ON
	if(mode_prev[path4] != m_Reg_Sts.Gain_Check_Mode.onoff)
	{
		if(m_Reg_Sts.Gain_Check_Mode.onoff)
		{
			ul_gain_st[path4] = Offset_St__None;

			if(path4 == 0)
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] Gain Mode : ON Start."); 
		}
	}
	mode_prev[path4] = m_Reg_Sts.Gain_Check_Mode.onoff;
	//-----------------------------------
	switch(ul_gain_st[path4])
	{
		case Offset_St__None:
		case Offset_St__Exit:			
			break;
			
		default:
			if(OSTimerGet_msec(&timer_sample[path4]) >= 1500) // every 1.5s				
				break;
			else
				return ul_gain_st[path4];			
	}
	if(IS__End_of_UL_Gain_Low_Offset(path4)==0) // if end('F') 
		Disp__AAU_GC_RSSI_RMS(path4);
		
	OSTimerReset(&timer_sample[path4]);
	//-----------------------------------
	switch(ul_gain_st[path4])
	{
		case Offset_St__None:
			if(path4 == 0)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] flag_Offset(%c). (when End %c)",
							m_Eep.f_UL_Gain_Low_Offset[path4], 'F'); 
			}
						
			cnt[path4] = 0;
			retry[path4] = 0;
			
			if(IS__End_of_UL_Gain_Low_Offset(path4)) // if end
				ul_gain_st[path4] = Offset_St__Exit;
			else
				ul_gain_st[path4] = Offset_St__Delay;
				
			break;

		case Offset_St__Delay:
			cnt[path4]++;
			if(cnt[path4] > 4)
			{
				ul_gain_st[path4] = Offset_St__Chk_RSSI;
				cnt[path4] = 0; // init
				rss_low[path4] = 0; // init
			}
			else if(path4 == 0)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : Skip(%d).", 
						path4, cnt[path4]); 
			}			
			break;

		case Offset_St__Chk_RSSI: // GC_RSSI_RMS
			if(cnt[path4] < Chk_RSSI_Max)
			{
				cur_cnt = cnt[path4];
				cnt[path4]++; // total count
				
				if(RSSI_RMS_x10 < 250)
				{
					// 2019.12.09 : 4회 중 1회라도 조건이면, 보정 기능 동작.
					rss_low[path4]++;

					if(path4 == 0)
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s : [%d] GC_RSSI_RMS (%d/10) < 25.0", 
								path4, 								
								str_Offset_St[ul_gain_st[path4]],
								cur_cnt,
								RSSI_RMS_x10); 
					}	
				}				
			}
			else
			{
				if(path4 == 0)
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s : rssi_low_cnt %d", 
							path4, 
							str_Offset_St[ul_gain_st[path4]],
							rss_low[path4]); 
				}	
				if(rss_low[path4] == 0)
					ul_gain_st[path4] = Offset_St__End;
				else
					ul_gain_st[path4] = Offset_St__Chk_Retry;
			}			
			break;

		case Offset_St__Chk_Retry:
			// init
			for(i = 0; i < GC_Offset_Sample_Max; i++) 
				sample[path4][i] = 0;
			cnt[path4] = 0;
						
			if(retry[path4] < GC_RSSI_Retry_Max)
			{				
				if(path4 == 0)
				{
					if(retry[path4] >= 1)
					{
						print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s : Retry %d", 
							path4, 
							str_Offset_St[ul_gain_st[path4]],
							retry[path4]); 
					}
				}
				ul_gain_st[path4] = Offset_St__Sampling;
			}
			else
				ul_gain_st[path4] = Offset_St__End;
			break;		
			
		case Offset_St__Sampling:
			i = cnt[path4];
			if(i < GC_Offset_Sample_Max)
			{
				sample[path4][i] = 300 - RSSI_RMS_x10;
			}
			
			if(path4 == 0)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s : sample_%d (%d/10) = 30.0 - RMS(%d/10)", 
							path4, 
							str_Offset_St[ul_gain_st[path4]],
							i,
							sample[path4][i], 
							RSSI_RMS_x10); 
			}
				
			cnt[path4]++;
			if(cnt[path4] >= GC_Offset_Sample_Max)
				ul_gain_st[path4] = Offset_St__Chk_Range;
			break;

		case Offset_St__Chk_Range:
			max_x10 = sample[path4][0];
			min_x10 = sample[path4][0];
			sum_x10 = 0;
			
			for(i = 0; i < GC_Offset_Sample_Max; i++)
			{	
				val_x10 = sample[path4][i];				
				
				if(max_x10 < val_x10) max_x10 = val_x10;
				if(min_x10 > val_x10) min_x10 = val_x10;
				sum_x10 += val_x10;
			}

			if(path4 == 0)
			{			
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s : max %d/10, min %d/10", 
						path4, 
						str_Offset_St[ul_gain_st[path4]],
						max_x10, min_x10);
			}			
			
			if((max_x10 - min_x10) <= 40) // 2019.12.09 : below 4.0[dB] -> 3.0[dB] 
			{
				avg_x10 = sum_x10/GC_Offset_Sample_Max; // set average				

				if(path4 == 0)
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s : new (%d/10) = old (%d/10) + average (%d/10)", 
						path4, 
						str_Offset_St[ul_gain_st[path4]],
						m_Eep.UL_GC_RSSI_Offset_x10[_100M][path4] + avg_x10,						
						m_Eep.UL_GC_RSSI_Offset_x10[_100M][path4],
						avg_x10); 
				}
				m_Eep.UL_GC_RSSI_Offset_x10[_100M][path4] += avg_x10;
				m_Env.is_eep_write = EEP_WRITE_CODE;
				ul_gain_st[path4] = Offset_St__End;
			}
			else
			{
				retry[path4]++;
				ul_gain_st[path4] = Offset_St__Chk_Retry;
			}			
			break;

		case Offset_St__End:
			if(IS__End_of_UL_Gain_Low_Offset(path4)==0)
			{				
				Set__End_of_UL_Gain_Low_Offset(path4);				
			}
			ul_gain_st[path4] = Offset_St__Exit;
			break;	

		case Offset_St__Exit:
			break;		
	}

	if(st_pre[path4] != ul_gain_st[path4])
	{
		if(path4 == 0)
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[UL_Gain_Low_Offset] PATH_%d : %s --> %s", 
				path4,
				str_Offset_St[st_pre[path4]], 
				str_Offset_St[ul_gain_st[path4]]); 
		}		
		
		st_pre[path4] = ul_gain_st[path4];
	}	
	return ul_gain_st[path4];
}


/*
----------------------------------------------------------
UL Gain 저하.

* 1) Gain Check Mode 제어 
  (Reg 정의 필요) - A2K 
  2) GC_RSSI_RMS Detector 값 획득
  3) 설정 값 확인 후 이상 발생 시 
----------------------------------------------------------
*/
static OS_TIMER Timer_UL_Gain_Start_Delay[AMPU_4_MAX];
void Proc___AAU_UL_Gain_Low(INT8U path4)
{
	INT8U is_alarm = 0;
	INT8U cpri_lock_st;
	INT8U status;
	INT8U skipSt;

	static INT8U start[AMPU_4_MAX] = {0,0,0,0};
	static INT8U gain_chk_mode_prev[AMPU_4_MAX] = {0xFF,0xFF,0xFF,0xFF};
	static INT8U init[AMPU_4_MAX] = {0,0,0,0};

	if(init[path4] == 0) // 2020.01.07
	{
		init[path4] = 1;
		gain_chk_mode_prev[path4] = m_Reg_Sts.Gain_Check_Mode.onoff;
	}
			
	if(m_St.AlmTest) return;
	if(path4 >= AMPU_4_MAX) return;
	
	/*
	------------------------------------------------------
	2019.11.28 : Auto Calculation of "AAU_UL_Gain_Low_Offset"
	------------------------------------------------------	
	*/	
	
	status = Proc___AAU_UL_Gain_Low_Offset(path4, &skipSt);	
	if(status != Offset_St__Exit)
	{
		// 2020.02.19
		if(skipSt &= nbits[4]) // excluding gain_check_mode
		{
			m_St.AlmSt.Path[path4].UL.Gain_Low = 0;
		}
		// else : keep previous status

		/*
		If not gain_check_mode
		(that is, m_Reg_Sts.Gain_Check_Mode.onoff == 0), 
		then return
		*/

		// for time dealy
		start[path4] = 0;
		OSTimerReset(&Timer_UL_Gain_Start_Delay[path4]);
		gain_chk_mode_prev[path4] = m_Reg_Sts.Gain_Check_Mode.onoff;	
		return;
	}

	/*
	------------------------------------------------------
	2019.12.26 : Time delay 2s before algorithm-starting : AAU
	------------------------------------------------------	
	*/	
	if(gain_chk_mode_prev[path4] != m_Reg_Sts.Gain_Check_Mode.onoff)
	{
		start[path4] = 0;
		OSTimerReset(&Timer_UL_Gain_Start_Delay[path4]);
		print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[AAU_UL_Gain_Low] PATH_%d Gain_chk_mode Changed", path4); 
	}
	gain_chk_mode_prev[path4] = m_Reg_Sts.Gain_Check_Mode.onoff;
	
	if(start[path4] == 0)
	{
		if(OSTimerGet_msec(&Timer_UL_Gain_Start_Delay[path4]) > (2*TICK_CO_SEC))
		{
			OSTimerReset(&Timer_UL_Gain_Start_Delay[path4]);
			start[path4] = 1;
			print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[AAU_UL_Gain_Low] PATH_%d Delay 2s End", path4); 
		}
		return;
	}
	//---------------------------------------------------------
	if(m_Reg_Sts.Gain_Check_Mode.onoff) // if gain_check_mode
	{	
	/*
		2019.09.17 : 
		- UL-Gain-Low Alarm : '100M' Only
		- alarm mask when CPRI-0 not insert
		- alarm mask when CPRI-0 Unlock
		2019.09.09 : 
		- alarm mask when AMP-Off (UL gain Check 는 동작하되 알람은 mask)
	*/
		is_alarm = 0; // default no alarm
		if(Is_SFP_Inserted(CPRI_0))
		{	
			cpri_lock_st = Get_Instant_CPRI_Lock_St();
			
			if(cpri_lock_st & cbits[CPRI_0])
			{			
				if(m_Reg_Sts.RFU[path4].PA_OnOff.On) 
				{
					if(m_St.d_R_GC_RSSI_RMS_x10[_100M][path4] < m_Eep.UL_Gain_Low_x10[path4])
						is_alarm = 1;
				}
			}
		}
		m_St.AlmSt.Path[path4].UL.Gain_Low = is_alarm;
	}	
	else
	{	
		// 2019.09.16
		if(m_Reg_Sts.RFU[path4].PA_OnOff.On == 0) // if amp off
			m_St.AlmSt.Path[path4].UL.Gain_Low = 0;
	}	
}




#ifdef __TYPE__AAU__
void Proc___AAU_ALC_OFF_Check(void)
{
	static INT8U init = 0;
	static INT8U fwd_alc_prev[AMPU_4_MAX];
	static INT8U rvs_alc_prev[AMPU_4_MAX];
	INT8U path4;
	//INT16S sVal16;

	if(init == 0)
	{
		init = 1;
		for(path4 = 0; path4 < AMPU_4_MAX; path4++)
		{
			fwd_alc_prev[path4] = m_Eep.AAU_Fwd_ALC_Onoff[path4];
			rvs_alc_prev[path4] = m_Eep.Rvs_IN_ALC_Onoff[path4];
		}
	}
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{
		//------------------------------------------------------------
		// DL ALC On/Off
		if(fwd_alc_prev[path4] != m_Eep.AAU_Fwd_ALC_Onoff[path4])
		{
			if(m_Eep.AAU_Fwd_ALC_Onoff[path4] == OFF)
			{
				// 2020.02.24 : maybe user-att controll				
				AAU__Fwd_Alc_Att_Set(path4, 0);
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[ALC_OFF_Check] DL Path#%d OFF", path4+1);
			}
		}		
		fwd_alc_prev[path4] = m_Eep.AAU_Fwd_ALC_Onoff[path4];
		
		//------------------------------------------------------------
		// UL ALC On/Off
		if(rvs_alc_prev[path4] != m_Eep.Rvs_IN_ALC_Onoff[path4])
		{
			if(m_Eep.Rvs_IN_ALC_Onoff[path4] == OFF)
			{
				m_St.RX_Qck_Att_x2[path4] = 0;
				m_St.RX_ALC_Att_x2[path4] = 0;
				AAU__Rvs_Alc_Att_Set(path4, m_St.RX_Qck_Att_x2[path4], m_St.RX_ALC_Att_x2[path4]);
				
				Set_ALC_Gain_Comp_x10(_100M, path4, 0); // 0xC016 UL Gain Ctrl
				Set_ALC_Gain_Comp_x10(Alpha, path4, 0);	

				print_agent(DEBUG_SYSTEM, VT_YELLOW, "[ALC_OFF_Check] UL Path#%d OFF", path4+1);
			}
			rvs_alc_prev[path4] = m_Eep.Rvs_IN_ALC_Onoff[path4];
		}
	}
}
#endif






#ifdef __TYPE__AAU__

INT8S Get_AAU_RF_Att_Accuracy_x2(INT8U dl_ul, INT8U path4, INT8U att_x2)
{
	INT8U idx = 0;	
	
	if(path4 >= AMPU_4_MAX) return 0;
	if(dl_ul >= DL_UL) return 0;	

	idx = (dl_ul*4) + path4;

	// table 비어 있으면, 디폴트값 리턴.	
	if (m_ATT_AMP_tbl[idx].entry_cnt == 0)
		return 0;
	if (m_ATT_AMP_tbl[idx].entry_cnt > MAX_ATT_TBL_COUNT)
		return 0;
	
#ifdef __USE_AAU_RF_ATT_Accuracy__			
	if(att_x2 > 63) att_x2 = 63; // max 31.5[dB]
	// att_idx 0 is 31.5dB
	return m_ATT_AMP_tbl[idx].entry[63 - att_x2];
#else
	return 0;
#endif
}




/*
----------------------------------------------------------
AAU 온도 보상 
	onoff : AID 0xB434
	temp  : AID 0x0201 : 
	DL    : temp current AID 0xB422, ATT 0x5106
	UL    : temp current AID 0xB423, ATT 0x5206
----------------------------------------------------------
*/
INT8S Get_AAU_Curr_Table_TempAtt(INT16S temp, INT8U dl_ul, INT8U path4)
{
	INT8S cur_temp;
	
	if(path4 >= AMPU_4_MAX) return 0;
	if(dl_ul >= DL_UL) return 0;

	INT8U table_idx = 0;

	table_idx = (dl_ul*4) + path4;

	     if(temp > 127) cur_temp = 127;
	else if(temp < -127) cur_temp = -127;
	else cur_temp = temp;

	return Get_TempAtt_Tbl( cur_temp , table_idx );
}

INT8S Get_AAU_Curr_Table_Alpha_TempAtt(INT16S temp, INT8U dl_ul, INT8U path4)
{
	INT8S cur_temp;
	
	if(path4 >= AMPU_4_MAX) return 0;
	if(dl_ul >= DL_UL) return 0;

	INT8U table_idx = 0;

	table_idx = (dl_ul*4) + path4;

	     if(temp > 127) cur_temp = 127;
	else if(temp < -127) cur_temp = -127;
	else cur_temp = temp;

	return Get_TempAtt_Alpha_Tbl( cur_temp , table_idx );
}


INT16S Get_AAU_DTU_Alpha_Temp_Att_x10(INT8U optic, INT8U path, INT8U dlul)
{
	     if(path >= MAX_PATH_4) return 0;
	else if(dlul >= DL_UL)  return 0;
	else if(optic != Alpha) return 0;
	else 
	{
		if(dlul == DL)
			return m_St.d_Temp_Att_Dig_DL_x10[A_ONLY][path];	
		else
			return m_St.d_Temp_Att_Dig_UL_x10[A_ONLY][path];	
	}
}

static INT16S temp_prev_DL_x10[Optic_2][MAX_DL];
static INT16S temp_prev_UL_x10[Optic_2][MAX_DL];

void Update__AAU_Temp_Att(INT8U path4)
{
	static INT8U init[MAX_DL] = {0,0,0,0};

	if(path4 >= AMPU_4_MAX) return; 

	if(init[path4] == 0)
	{
		init[path4] = 1;		
		temp_prev_DL_x10[COMMON][path4] = -100; // -10.0[dB]
		temp_prev_UL_x10[A_ONLY][path4] = -100; // -10.0[dB]
	}
	
	if(m_Eep.TempComp_Onoff == ON)
	{
		// Get Temp-Atten according to current temp
		m_St.d_Temp_Att_Dig_DL_x10[COMMON][path4] = 5*Get_AAU_Curr_Table_TempAtt(m_St.Temp_x10/10, DL, path4);
		m_St.d_Temp_Att_Dig_UL_x10[COMMON][path4] = 5*Get_AAU_Curr_Table_TempAtt(m_St.Temp_x10/10, UL, path4);

		m_St.d_Temp_Att_Dig_DL_x10[A_ONLY][path4] = 5*Get_AAU_Curr_Table_Alpha_TempAtt(m_St.Temp_x10/10, DL, path4);
		m_St.d_Temp_Att_Dig_UL_x10[A_ONLY][path4] = 5*Get_AAU_Curr_Table_Alpha_TempAtt(m_St.Temp_x10/10, UL, path4);
	}
	else
	{
		m_St.d_Temp_Att_Dig_DL_x10[COMMON][path4] = 0;
		m_St.d_Temp_Att_Dig_UL_x10[COMMON][path4] = 0;

		m_St.d_Temp_Att_Dig_DL_x10[A_ONLY][path4] = 0;
		m_St.d_Temp_Att_Dig_UL_x10[A_ONLY][path4] = 0;
	}
}

void Proc___AAU_Temp_Att(INT8U path4)
{		
	if(path4 >= AMPU_4_MAX) return; 
	
	Update__AAU_Temp_Att(path4);

	// if atten changed or On <-> Off
	//------------------------------------------
	// 100M & Alpha Common
	//------------------------------------------
	if(temp_prev_DL_x10[COMMON][path4] != m_St.d_Temp_Att_Dig_DL_x10[COMMON][path4])
	{
		//D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] DL.Att[%d] %d", 
		//	path4, 
		//	m_Reg_Sts.RFU[path4].DL_Att.Val_x4/2*5 - temp_prev_DL_x10[COMMON][path4]);
			
		temp_prev_DL_x10[COMMON][path4] = m_St.d_Temp_Att_Dig_DL_x10[COMMON][path4];
		AAU_RF_DL_Att_Set(path4, m_St.DL_Att[path4]/2);
		
		D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] DL.Temp.Att[%d] %d Control", 
			path4, m_St.d_Temp_Att_Dig_DL_x10[COMMON][path4]);
	}	
	if(temp_prev_UL_x10[COMMON][path4] != m_St.d_Temp_Att_Dig_UL_x10[COMMON][path4])
	{
		//D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] UL.Att[%d] %d", 
		//	path4, 
		//	m_Reg_Sts.RFU[path4].UL_Att.Val_x4/2*5 - temp_prev_UL_x10[COMMON][path4]);
			
		temp_prev_UL_x10[COMMON][path4] = m_St.d_Temp_Att_Dig_UL_x10[COMMON][path4];
		AAU_RF_UL_Att_Set(path4, m_St.UL_Att[path4]/2);
		
		D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] UL.Temp.Att[%d] %d Control", 
			path4, 
			m_St.d_Temp_Att_Dig_UL_x10[COMMON][path4]);
	}
	
	//------------------------------------------
	// Alpha Only
	//------------------------------------------
	if(temp_prev_DL_x10[A_ONLY][path4] != m_St.d_Temp_Att_Dig_DL_x10[A_ONLY][path4])
	{			
		temp_prev_DL_x10[A_ONLY][path4] = m_St.d_Temp_Att_Dig_DL_x10[A_ONLY][path4];		

		AAU_DTU_ATT_Refresh(Alpha, path4, DL);
		
		D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] Alpha.DTU.DL.Temp.Att[%d] %d Control", 
			path4, m_St.d_Temp_Att_Dig_DL_x10[A_ONLY][path4]);
	}
	
	if(temp_prev_UL_x10[A_ONLY][path4] != m_St.d_Temp_Att_Dig_UL_x10[A_ONLY][path4])
	{			
		temp_prev_UL_x10[A_ONLY][path4] = m_St.d_Temp_Att_Dig_UL_x10[A_ONLY][path4];
		
		AAU_DTU_ATT_Refresh(Alpha, path4, UL);
		
		D_Printf(DEBUG_SYSTEM, VT_MAGENTA "[RCU] Alpha.DTU.UL.Temp.Att[%d] %d Control", 
			path4, 
			m_St.d_Temp_Att_Dig_UL_x10[A_ONLY][path4]);
	}
}

void Proc___AAU_BW_Att(void)
{
	static INT8U bw_a = 0xFF;
	static INT8U height_bal_onoff = 0xFF;
	INT8U bw_att_x10;
	INT8U path4;
	
	if((bw_a != m_Eep.d_RE_Channel_BW[Alpha])||(height_bal_onoff != m_Eep.Height_Bal_Onoff))
	{
		bw_att_x10 = Get_Curr_TSSI_Input_Pwr_Bal_Att_x10(m_Eep.d_RE_Channel_BW[Alpha]);

		for(path4=0; path4 < MAX_MIMO_PATH; path4++)
		{
			m_St.d_DL_TSSI_BalAtt_x10[_100M][path4] = bw_att_x10;
			m_St.d_DL_TSSI_BalAtt_x10[Alpha][path4] = bw_att_x10;
		}
		
		print_agent(DEBUG_SYSTEM, VT_YELLOW, "[Proc___AAU_BW_Att] BW %d -> %d", 
				bw_a, m_Eep.d_RE_Channel_BW[Alpha]);
		
		for(path4=0; path4 < MAX_MIMO_PATH; path4++)
		{
			AAU_DTU_ATT_Refresh(_100M, path4, DL);
			AAU_DTU_ATT_Refresh(Alpha, path4, DL);
		}
		bw_a = m_Eep.d_RE_Channel_BW[Alpha];		
		height_bal_onoff = m_Eep.Height_Bal_Onoff; // 2019.08.06
	}
}
#endif


/*
----------------------------------------------------------
PRU : 
	노란색 Debug message를 "VSS table reload"
	PRU의 경우 CPRI 연결후 간혹 VSS 테이블이 이상하게 읽히는 경우
	Open CPRI Version == 0x00 or 0xFF 상태가 2초간 유지 시, 
	PS Register 0x09번지에 20번 bit를 0으로 제어후 1로 제어
----------------------------------------------------------

static OS_TIMER timer_Chk_OpenCpriVer;
void Proc__Check__OpenCpriVersion(void)
{
	static INT8U init = 0;
	static INT8U chk_St = 0;
	INT8U  curr_open_cpri_ver;
	INT8U  cpri;

	cpri = CPRI_0;
	curr_open_cpri_ver = m_Reg_Sts.DL_CPRI[cpri].OpenCPRIVersion.uD8[0];
	
	if(init == 0)
	{
		init = 1;
		chk_St = 0;
		OSTimerReset(&timer_Chk_OpenCpriVer);
	}

	switch(chk_St)
	{
		case 0: //
			if((curr_open_cpri_ver != 0x00)&&(curr_open_cpri_ver != 0xFF))			
				OSTimerReset(&timer_Chk_OpenCpriVer);
			else
			{
				if(OSTimerGet_msec(&timer_Chk_OpenCpriVer) > 2000) // 2[s]
				{
					printf(VT_YELLOW "[RCU] VSS table reload (CPRI%d)\n" VT_END_CR, cpri);
					
					D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] 1.Read (VSS table): CPRI%d %d/%d\n", 
						cpri, m_Reg_Sts.RW_Reset.CPRI1, m_Reg_Sts.RW_Reset.CPRI2);					
					D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] VSS table reload (CPRI%d), %d[ms]\n", 
						cpri, OSTimerGet_msec(&timer_Chk_OpenCpriVer));
				
					m_Reg_Flg.RW_Reset.Data = _SET_;
					
					if(cpri == CPRI_0)
						m_Reg_Ctl.RW_Reset.CPRI1 = 0; // reset
					else
						m_Reg_Ctl.RW_Reset.CPRI2 = 0; // reset

					//printf(VT_YELLOW "[RCU] VSS table reload (CPRI%d), Reset Enable.\n" VT_END_CR, cpri);	
					//D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] VSS table reload (CPRI%d), Reset Enable.\n", cpri);	

					chk_St = 1; // check cpri reset
				}
			}
			break;
			
		case 1:	
			chk_St = 2;
			D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] 2.Read CPRI%d, %d/%d\n", 
				cpri, m_Reg_Sts.RW_Reset.CPRI1, m_Reg_Sts.RW_Reset.CPRI2);	

			if(m_Reg_Sts.RW_Reset.CPRI1 != 0)
			{
				m_Reg_Flg.RW_Reset.Data = _SET_;
				m_Reg_Ctl.RW_Reset.CPRI1 = 0; // reset
			}			
			break;
			
		case 2:
			D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] 3.Read CPRI%d,  %d/%d\n", 
				cpri, m_Reg_Sts.RW_Reset.CPRI1, m_Reg_Sts.RW_Reset.CPRI2);	
				
			m_Reg_Flg.RW_Reset.Data = _SET_;
			
			m_Reg_Ctl.RW_Reset.CPRI1 = 1; // reset disable
			m_Reg_Ctl.RW_Reset.CPRI2 = 1; // reset disable

			//printf(VT_YELLOW "[RCU] VSS table reload (CPRI%d), Reset Disable. %d[ms]\n" VT_END_CR, 
			//	cpri, OSTimerGet_msec(&timer_Chk_OpenCpriVer));
			//D_Printf(DEBUG_SYSTEM, VT_YELLOW "[RCU] VSS table reload (CPRI%d), Reset Disable. %d[ms].\n", 
			//	cpri, OSTimerGet_msec(&timer_Chk_OpenCpriVer));
			
			chk_St = 0;
			OSTimerReset(&timer_Chk_OpenCpriVer);
			break;
	}
}
*/



/*
----------------------------------------------------------
1. Count 읽은 후, init 제어.
2. GUI AID : Main(accumlated value)

BIP Only for CPRI_0/1 & DL (#0,#1 @ Register Document)
BIP Only for CPRI_2/3 & UL (#6,#7 @ Register Document)

----------------------------------------------------------
*/
void Proc___BIP__ErrCount(void)
{
	INT8U c;
	static INT8U cnt = 0;

	//-------------------------------------
	if(m_St.Is_CPRI_Lock==0) // Checked
		return;
	//-------------------------------------	

	cnt++;
	
	for(c = CPRI_0; c <= CPRI_1; c++) 
	{
		if(Is_SFP_Inserted(c) == 0)
		{
			// 2019.07.04
			m_St.d_BIP_ErrCnt[DL][c] = 0;
			m_St.d_BIP_Count[DL][c] = 0;
			
			continue;
		}
		
		if(m_Reg_Sts.DL_CPRI[c].BIP_Clear.Clear == 0)
		{
			if((cnt%10)==0)
			{
				m_St.d_BIP_ErrCnt[DL][c] += m_St.d_BIP_Count[DL][c];
				m_Reg_Flg.DL_CPRI[c].BIP_Clear.Data = 1;
				m_Reg_Ctl.DL_CPRI[c].BIP_Clear.Clear = 1;

//				D_Printf(DEBUG_SYSTEM, "[RCU] BIP DL(%d) Accum %d, delta %d\n", 
//					c, m_St.d_BIP_ErrCnt[DL][c], m_St.d_BIP_Count[DL][c]);
			}			
		}
		else
		{
			m_Reg_Flg.DL_CPRI[c].BIP_Clear.Data = 1;
			m_Reg_Ctl.DL_CPRI[c].BIP_Clear.Clear = 0;

			//D_Printf(DEBUG_SYSTEM, "[RCU] BIP Clr DL(%d)\n", c);
		}
	}
	
	for(c = CPRI_2; c <= CPRI_3; c++) 
	{
		if(Is_SFP_Inserted(c) == 0) 
		{			
			// 2019.07.04
			m_St.d_BIP_ErrCnt[UL][c] = 0;
			m_St.d_BIP_Count[UL][c] = 0;
			
			continue;
		}
		
		if(m_Reg_Sts.UL_CPRI[c].BIP_Clear.Clear == 0)
		{
			if((cnt%10)==0)
			{
				m_St.d_BIP_ErrCnt[UL][c] += m_St.d_BIP_Count[UL][c];
				m_Reg_Flg.UL_CPRI[c].BIP_Clear.Data = 1;
				m_Reg_Ctl.UL_CPRI[c].BIP_Clear.Clear = 1;
				
//				D_Printf(DEBUG_SYSTEM, "[RCU] BIP Ul(%d) Accum %d, delta %d\n", 
//					c, m_St.d_BIP_ErrCnt[UL][c], m_St.d_BIP_Count[UL][c]);
			}
		}
		else
		{
			m_Reg_Flg.UL_CPRI[c].BIP_Clear.Data = 1;
			m_Reg_Ctl.UL_CPRI[c].BIP_Clear.Clear = 0;

			//D_Printf(DEBUG_SYSTEM, "[RCU] BIP Clr Ul(%d)\n", c);
		}
	}	
	// 2019.06.29 : Use 0x530A of Accumulated Value
	// sequence : DL-CPRI0, UL-CPRI1, DL-CPRI1, UL-CPRI3	
	Update_Stat(&m_Stat.d_BIP_AccCnt[0], m_St.d_BIP_ErrCnt[DL][CPRI_0]);
	Update_Stat(&m_Stat.d_BIP_AccCnt[1], m_St.d_BIP_ErrCnt[UL][CPRI_2]);
	Update_Stat(&m_Stat.d_BIP_AccCnt[2], m_St.d_BIP_ErrCnt[DL][CPRI_1]);
	Update_Stat(&m_Stat.d_BIP_AccCnt[3], m_St.d_BIP_ErrCnt[UL][CPRI_3]);
	//-------------------------------------
	if(m_St.d_BIP_Clr[0]) // DL-CPRI0,UL-CPRI2
	{
		m_St.d_BIP_Clr[0] = 0;
		m_St.d_BIP_ErrCnt[UL][CPRI_2] = 0; // clear with 0
		m_St.d_BIP_ErrCnt[DL][CPRI_0] = 0; // clear with 0
	}
	//-------------------------------------
	if(m_St.d_BIP_Clr[1]) // DL-CPRI1,UL-CPRI3
	{
		m_St.d_BIP_Clr[1] = 0;
		m_St.d_BIP_ErrCnt[UL][CPRI_3] = 0; // clear with 0
		m_St.d_BIP_ErrCnt[DL][CPRI_1] = 0; // clear with 0
	}
	//-------------------------------------
	if(m_St.d_BIP_Clr[2]) // UL-CPRI2
	{
		m_St.d_BIP_Clr[2] = 0;
		m_St.d_BIP_ErrCnt[UL][CPRI_2] = 0; // clear with 0
	}
	//-------------------------------------
	if(m_St.d_BIP_Clr[3]) // UL-CPRI3
	{
		m_St.d_BIP_Clr[3] = 0;
		m_St.d_BIP_ErrCnt[UL][CPRI_3] = 0; // clear with 0
	}
	//-------------------------------------
}
/*
----------------------------------------------------------
1. Hard-clipping count 읽은 후, init 제어.
2. GUI AID : Main(accumlated value), Hidden(Instant count)
3. level control : dBm <-> hex Conversion is needed.
----------------------------------------------------------
*/
void Proc___Hard_Clipping(void)
{
	INT8U l;
	static INT8U cnt = 0;

	cnt++;

	if(m_St.d_Hard_Clip_Cnt_Init)
	{
		m_St.d_Hard_Clip_Cnt_Init = 0;

		for(l = 0; l < MAX_DL; l++) 
			m_St.d_Hard_Clip_Cnt_Accum[l] = 0;
	}
	//-----------------------------------------------------
	// 2019.04.23.
	// Accumulate and Clear
	//-----------------------------------------------------	
	for(l = 0; l < MAX_DL; l++)
	{
		if(m_St.d_Hard_Clip_Clear[l] == 0) // if normal, accumulate..
		{
			if((cnt%20)==0) // 2019.05.25
			{
				m_St.d_Hard_Clip_Cnt_Accum[l] += m_St.d_Hard_Clip_Cnt_DL[l];

				// 2019.06.29 : Use 0x5320 of Accumulated Value
				Update_Stat( &m_Stat.d_Hard_Clip_Cnt_DL[l], m_St.d_Hard_Clip_Cnt_Accum[l] );
					
				m_Reg_Flg.DSP_Common_MIMO[l].HC_Clr.clear = 1;
				m_Reg_Ctl.DSP_Common_MIMO[l].HC_Clr.clear = 1;
			}
		}
		else
		{
			m_Reg_Flg.DSP_Common_MIMO[l].HC_Clr.clear = 1;
			m_Reg_Ctl.DSP_Common_MIMO[l].HC_Clr.clear = 0;
		}
		//----------------------------------------------
		// 2019.05.30		
	#ifdef __TYPE__PRU__		
		if(m_Eep.d_Hard_Clip_OnOff[l] == OFF)
		{
			if(m_Reg_Sts.DSP_Common_MIMO[l].HC_Threshold.value != 0x7FFF)
			{			
				m_Reg_Flg.DSP_Common_MIMO[l].HC_Threshold.value = _F_SET_;
				m_Reg_Ctl.DSP_Common_MIMO[l].HC_Threshold.value = 0x7FFF;
			}
		}
		else
		{
			if(m_Reg_Sts.DSP_Common_MIMO[l].HC_Threshold.value != m_Eep.d_Hard_Clip_Lvl_x10[l])
			{			
				m_Reg_Flg.DSP_Common_MIMO[l].HC_Threshold.value = _F_SET_;
				m_Reg_Ctl.DSP_Common_MIMO[l].HC_Threshold.value = m_Eep.d_Hard_Clip_Lvl_x10[l];
			}
		}
	#else // AAU
		// do nothing
	#endif
		//----------------------------------------------
	}	
}



/*
----------------------------------------------------------
AAU : L/S Maker에 따른 LS Gain Offset Adjust
--------------------------------------
LS_Gain_Bal_OnOff : ON 이면, Adjust.
LS_Gain_Bal_OnOff : OFF 이면, 0dB Adjust.
----------------------------------------------------------
*/
//-----------------------------------------------
void Proc___LSH_Gain_Set(void)
{
	INT8U fa;
	INT8U m; 
	INT8U rec = 0;
	INT16U ref_gain_x10;
#ifdef __TYPE__PRU__	
	INT16U cur_gain_x10;
#endif
	static INT8U init = 0;
	

	if(init == 0)
	{
		init = 1;

		/*
		//if(m_Eep.LS_Gain_Bal_OnOff > 1)
		//	m_Eep.LS_Gain_Bal_OnOff = 0; // default OFF
		*/
		m_Eep.LS_Gain_Bal_OnOff = 1; // 2020.02.03 : default ON	
	}

	switch(AufoConfigFlag.RECTypeID)
	{
		case _TYPE_LS_SS_ : rec = LSH_SS; break;
		case _TYPE_LS_ELG_: rec = LSH_ER; break;
		case _TYPE_LS_NKA_: rec = LSH_NK; break;
		
		default: rec = LSH_SS; break;
	}
	
#ifdef __TYPE__PRU__
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			// DL ----------------------------------------------------			
		
			cur_gain_x10 = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_DL_LSH_Gain.uD32;			
			ref_gain_x10 = m_Eep.d_LS_gain_x10[rec][DL][fa][m];

			// 2020.01.21
			if(m_Eep.LS_Gain_Bal_OnOff == 0) 
				ref_gain_x10 = 0;
				
			if(cur_gain_x10 != ref_gain_x10)
				LSH_Gain_Set(rec, fa, m, DL, ref_gain_x10);
						
			m_St.Curr_LS_Gain_Bal_x10[fa][DL][m] = ref_gain_x10;
			
			// UL ----------------------------------------------------
			cur_gain_x10 = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_UL_LSH_Gain.uD32;
			ref_gain_x10 = m_Eep.d_LS_gain_x10[rec][UL][fa][m];

			// 2020.01.21
			if(m_Eep.LS_Gain_Bal_OnOff == 0)
				ref_gain_x10 = 0;
			
			if(cur_gain_x10 != ref_gain_x10)
				LSH_Gain_Set(rec, fa, m, UL, ref_gain_x10);			
				
			m_St.Curr_LS_Gain_Bal_x10[fa][UL][m] = ref_gain_x10;
		}
	}
#else
	for(fa = 0; fa < MAX_FA_PATH; fa++)
	{
		for(m = 0; m < MAX_MIMO_PATH; m++)
		{
			// DL ----------------------------------------------------			
			if(m_Eep.LS_Gain_Bal_OnOff)  
				ref_gain_x10 = m_Eep.d_LS_gain_x10[rec][DL][fa][m];
			else 
				ref_gain_x10 = 0; // if(OFF:0) 0dB set
		
			// L/S Gain Balance
			if(m_St.Curr_LS_Gain_Bal_x10[fa][DL][m] != ref_gain_x10)
			{
				m_St.Curr_LS_Gain_Bal_x10[fa][DL][m] = ref_gain_x10;
				AAU_DTU_ATT_Refresh(fa, m, DL);
			}			
			
			// UL ----------------------------------------------------					
			if(m_Eep.LS_Gain_Bal_OnOff) 
				ref_gain_x10 = m_Eep.d_LS_gain_x10[rec][UL][fa][m];
			else	
				ref_gain_x10 = 0; // if(OFF:0) 0dB set
		
			// L/S Gain Balance
			if(m_St.Curr_LS_Gain_Bal_x10[fa][UL][m] != ref_gain_x10)
			{
				m_St.Curr_LS_Gain_Bal_x10[fa][UL][m] = ref_gain_x10;
				AAU_DTU_ATT_Refresh(fa, m, UL);
			}
		}
	}
#endif	
}




/*
---------------------------------------------------------------------------------
Proc___Test_Pattern_Band_Gain
------------------
PRU Only
---------------------------------------------------------------------------------	
*/

void Proc___Test_Pattern_Band_Gain(void)
{	
	INT8U m;
	INT16U ref_gain_x10, cur_gain_x10;	
	INT8U onoff;
	
	for(m = 0; m < MAX_MIMO_PATH; m++)
	{
		onoff = m_St.d_Test_Pattern_OnOff[_100M][m]; // 100M is reference
		
		// 100M ------------------------------
		if(onoff == ON)
			ref_gain_x10 = m_Eep.d_Test_Pattern_Band_Gain[_100M][m];
		else
			ref_gain_x10 = 0;

		cur_gain_x10 = m_St.d_Test_Pattern_Band_Gain[_100M][m];
		if(cur_gain_x10 != ref_gain_x10)
			Test_Pattern_Band_Gain_Set(_100M, m, ref_gain_x10);		
			
		// Alpha ------------------------------
		if(onoff == ON)
			ref_gain_x10 = m_Eep.d_Test_Pattern_Band_Gain[Alpha][m];
		else
			ref_gain_x10 = 0;
		
		cur_gain_x10 = m_St.d_Test_Pattern_Band_Gain[Alpha][m];
		if(cur_gain_x10 != ref_gain_x10)
			Test_Pattern_Band_Gain_Set(Alpha, m, ref_gain_x10);
	}	
}

/*
---------------------------------------------------------------------------------
Proc___Off_Test_Pattern
------------------
Test pattern ON 후, 10 분 지나면 자동 OFF Control  
---------------------------------------------------------------------------------	
*/
static OS_TIMER test_pattern_time[MAX_DL];
void Proc___Off_Test_Pattern(void)
{
	static INT8U init = 0;
	INT8U i;
	INT8U off_req[MAX_DL];

	if(init == 0)
	{
		init = 1;
		for(i = 0; i < MAX_DL; i++)
		{
			OSTimerReset(&test_pattern_time[i]);
			off_req[i] = 1;
		}			
	}
	else
	{
		for(i = 0; i < MAX_DL; i++)
		{
			off_req[i] = 0; // init
			
			if(m_St.d_Test_Pattern_OnOff[_100M][i] == ON)
			{
				////print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[Off_Test_Pattern] Path[%d] %d[ms].", 
				////	i, OSTimerGet_msec(&test_pattern_time[i]));
			
				if(OSTimerGet_msec(&test_pattern_time[i]) > (10*TICK_CO_MIN))
				{
					OSTimerReset(&test_pattern_time[i]);
					off_req[i] = 1;
					print_agent(DEBUG_SYSTEM, VT_BOLD_BLUE, "[Off_Test_Pattern] Path[%d].Test_Pattern 10[min].", i);
				}
			}
			else
			{
				off_req[i] = 1;
				OSTimerReset(&test_pattern_time[i]);
			}
		}
	}

	for(i = 0; i < MAX_DL; i++)
	{
		if(off_req[i])
		{
			if(m_Reg_Sts.General_FA[_100M].General_FA_MIMO[i].RW_TEST_Pattern.OnOff != 0)
			{
				m_Reg_Flg.General_FA[_100M].General_FA_MIMO[i].RW_TEST_Pattern.OnOff = _SET_;
				m_Reg_Ctl.General_FA[_100M].General_FA_MIMO[i].RW_TEST_Pattern.OnOff = 0; // (0:Off)
				
				m_Reg_Flg.General_FA[Alpha].General_FA_MIMO[i].RW_TEST_Pattern.OnOff = _SET_;
				m_Reg_Ctl.General_FA[Alpha].General_FA_MIMO[i].RW_TEST_Pattern.OnOff = 0; // (0:Off)
			}
		}
	}
}

//-----------------------------------------------

//============================================================================

INT8U IS_Valid_VSS_REC_Type(INT8U RECTypeID)
{
	switch(RECTypeID)
	{
		case _TYPE_LS_SS_ :
		case _TYPE_LS_NKA_:
		case _TYPE_LS_ELG_:
			return 1;
			
		default:
			return 0;
	}
}










/*
OSM 동작과 관련하여 CPRI 초기화 관련 내용을 RCU APP에 추가 요청 드립니다.
*/
//============================================================================
// 
//============================================================================
void Proc__CPRI_Init_OSM_Ctrl(void)
{	
#ifdef _CPRI_Init_OSM_Ctrl_ // ANTS-PRU
	INT8U c;
	static INT8U init = 0;

	if(init == 1) return; // only 1 time
	
	// Board Status Reg Read, 
	// 0x07 : PLL LOCK check, [0] : PLL Lock, Lock = '1', Fail = '0'	

	m_Reg_Sts.R_Board_Status.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_R_Board_Status]);
	
	if(m_Reg_Sts.R_Board_Status.Pll_Init_Done) 
	{	
		
		printf(VT_GREEN "[RCU] CPRI Init : 1. Pll_Lock ----\n" VT_END_CR);
				
		printf(VT_GREEN "[RCU] CPRI Init : 2. CPRI1 0 Set ----\n" VT_END_CR);
		// CPRI Core Reset
		//-------------------------------------------
		//PSBUS_read(cpri_reset_addr, &data, DBG_ON)
		//BitRst(data, 20); // bit20 : set 0             
        //PSBUS_write(cpri_reset_addr, data, DBG_ON)         
        //-------------------------------------------        
		m_Reg_Sts.RW_Reset.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset]);
		m_Reg_Ctl.RW_Reset.Data = m_Reg_Sts.RW_Reset.Data;
		m_Reg_Ctl.RW_Reset.CPRI1 = 0; // bit20 : set 0
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "CPRI1 0 Set");

		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[m_Reg_Ctl.RW_Reset.Data 3] 0x%X", m_Reg_Ctl.RW_Reset.Data);

		// SFP OFF
		printf(VT_GREEN "[RCU] CPRI Init : 3. SFP Off ----\n" VT_END_CR);
		//-------------------------------------------	
        // PSBUS_write(0x1007, 0x00, DBG_ON);
        // PSBUS_write(0x2007, 0x00, DBG_ON);
        // PSBUS_write(0x3007, 0x00, DBG_ON);
        // PSBUS_write(0x4007, 0x00, DBG_ON);
		//-------------------------------------------
		for(c = 0; c < CPRI_PATH; c++)
			Write_FPGA_Reg(Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_Reset], 0x00, "SFP_Reset");

		//-------------------------------------------
		// sys_sleep(500);
		//-------------------------------------------
		printf(VT_GREEN "[RCU] CPRI Init : 4. Delay 500[ms] ----\n" VT_END_CR);
		OSTimeDly(OS_DELAY_500msec); 
		             
		// CPRI Core Clear
		printf(VT_GREEN "[RCU] CPRI Init : 5. CPRI1 1 Set ----\n" VT_END_CR);
		//-------------------------------------------
        // BitSet(data, 20);
        // PSBUS_write(cpri_reset_addr, data, DBG_ON);         
		//-------------------------------------------
        m_Reg_Sts.RW_Reset.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset]);
		m_Reg_Ctl.RW_Reset.Data = m_Reg_Sts.RW_Reset.Data;
		m_Reg_Ctl.RW_Reset.CPRI1 = 1; // bit20 : set 1
		Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "CPRI1 1 Set");

		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[m_Reg_Ctl.RW_Reset.Data 4] 0x%X", m_Reg_Ctl.RW_Reset.Data);


		printf(VT_GREEN "[RCU] CPRI Init : 6.0 mmap write 0x0 @ 0x0d  ----\n" VT_END_CR);
        for(c = 0; c < CPRI_PATH; c++) 
        	Write_A_PRU_Mmap(c, MMAP_____0x0d, 0x0);

  		//-------------------------------------------
        //CPRI_write( 0x0, 0x0b, 0x2A);                   // Fixed Value d(42)
        //CPRI_write( 0x1, 0x0b, 0x2A);                   // Fixed Value d(42)
        //CPRI_write( 0x2, 0x0b, 0x2A);                   // Fixed Value d(42)
        //CPRI_write( 0x3, 0x0b, 0x2A);                   // Fixed Value d(42)
        //-------------------------------------------
        printf(VT_GREEN "[RCU] CPRI Init : 6.1 mmap write 0x2A @ 0x0b  ----\n" VT_END_CR);
        for(c = 0; c < CPRI_PATH; c++) 
        	Write_A_PRU_Mmap(c, MMAP_____0x0b, 0x2A);


        printf(VT_GREEN "[RCU] CPRI Init : 6.2 mmap write 0x80 @ 0x0d  ----\n" VT_END_CR);
        for(c = 0; c < CPRI_PATH; c++) 
        	Write_A_PRU_Mmap(c, MMAP_____0x0d, 0x80);
 
		//-------------------------------------------
        //CPRI_write( 0x0, 0x0e, 0x80000000);                     // softreset        //2019.05.10 Park J.W
        //CPRI_write( 0x1, 0x0e, 0x80000000);                     // softreset
        //CPRI_write( 0x2, 0x0e, 0x80000000);                     // softreset
        //CPRI_write( 0x3, 0x0e, 0x80000000);                     // softreset
        //-------------------------------------------
        printf(VT_GREEN "[RCU] CPRI Init : 7. mmap write 0x80000000 @ 0x0e  ----\n" VT_END_CR);
        
        for(c = 0; c < CPRI_PATH; c++) 
        	Write_A_PRU_Mmap(c, MMAP____SW_Reset, 0x80000000);        
        
		//-------------------------------------------
        //CPRI_write( 0x0, 0x08, 0x03);
        //CPRI_write( 0x1, 0x08, 0x03);
        //CPRI_write( 0x2, 0x08, 0x03);
        //CPRI_write( 0x3, 0x08, 0x03);
        //-------------------------------------------
        printf(VT_GREEN "[RCU] CPRI Init : 8.0 mmap write 0x03 @ 0x08  ----\n" VT_END_CR);
        for(c = 0; c < CPRI_PATH; c++) 
        	Write_A_PRU_Mmap(c, MMAP____LoopBack, 0x03);


        printf(VT_GREEN "[RCU] CPRI Init : 8.1 mmap write 0x00 @ 0x08  ----\n" VT_END_CR);
        for(c = 0; c < CPRI_PATH; c++) 
        	Write_A_PRU_Mmap(c, MMAP____LoopBack, 0x00);	

        // SFP ON
		//-------------------------------------------	
        // PSBUS_write(0x1007, 0x01, DBG_ON);
        // PSBUS_write(0x2007, 0x01, DBG_ON);
        // PSBUS_write(0x3007, 0x01, DBG_ON);
        // PSBUS_write(0x4007, 0x01, DBG_ON);
		//-------------------------------------------
		printf(VT_GREEN "[RCU] CPRI Init : 9.0 Delay before SFP ON  ----\n" VT_END_CR);
		OSTimeDly(OS_DELAY_500msec); 
		
		printf(VT_GREEN "[RCU] CPRI Init : 9. SFP ON  ----\n" VT_END_CR);
		for(c = 0; c < CPRI_PATH; c++)
			Write_FPGA_Reg(Reg_Common_MIMO_Addr[c][Reg_Common_MIMO_RW_SFP_Reset], 0x01, "SFP ON");	

		// CPRI Core Reset
		printf(VT_GREEN "[RCU] CPRI Init : 10. CPRI Set : 0 & 1  ---- deleted...\n" VT_END_CR);
		/*
		printf(VT_GREEN "[RCU] CPRI Init : 10. CPRI Set : 0 & 1  ----\n" VT_END_CR);
		//-------------------------------------------
		//PSBUS_read(cpri_reset_addr, &data, DBG_ON);
        //BitRst(data, 20);
        //PSBUS_write(cpri_reset_addr, data, DBG_ON);         // CPRI Core Reset
        //BitSet(data, 20);
        //PSBUS_write(cpri_reset_addr, data, DBG_ON);         // CPRI Core Clear
		//-------------------------------------------
		 m_Reg_Sts.RW_Reset.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset]);
		 m_Reg_Ctl.RW_Reset.Data = m_Reg_Sts.RW_Reset.Data;
		 m_Reg_Ctl.RW_Reset.CPRI1 = 0; // bit20 : set 0
		 Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "CPRI1 0 Set");
		 m_Reg_Ctl.RW_Reset.CPRI1 = 1; // bit20 : set 1
		 Write_FPGA_Reg(Reg_Common_Addr[Reg_Common_RW_Reset], m_Reg_Ctl.RW_Reset.Data, "CPRI1 0 Set");
		 */
		

		
        //-------------------------------------------     
        // OSM ON
        //-------------------------------------------
		printf(VT_GREEN "[RCU] CPRI Init : 11. OSM ON ----\n" VT_END_CR);
//		OSM_On_Ctrl();	///// YCSO delete 190624

		init = 1; // only 1 time
		printf(VT_GREEN "[RCU] CPRI Init : End ----\n" VT_END_CR);
   }
#endif   
}
//============================================================================
//============================================================================


OS_TIMER cpri_alm_remain_time;




#ifdef __TYPE__PRU__ // ANTS-PRU
static INT8U Proc___PRU_Rvs_OtherCompany__Over_Input(INT8U path4);
#endif


/*
---------------------------------------------------------------------------
	(1) Every 100[ms]
	(2)
	Lock ~ Unlock 상태 변경시 표시, 
	Lock 상태 유지시 표시 X
	Unlock 상태 유지시 10초마다 표시
---------------------------------------------------------------------------	
*/
OS_TIMER cpri_alm_disp_time;
static INT8U CPRI_Lock_Prev = 1; // init 1 (lock)

void Proc__Disp_CPRI_Lock(void)
{
	static INT8U init = 0;
	//-------------------------------------
	// Display Lock Change
	//-------------------------------------

	if(init == 0)
	{
		init = 1;
		
	#ifdef __TYPE__AAU__ // ANTS-AAU
		print_agent( DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Initial Lock-St] CPRI0/1 0x%X/0x%X, Mscl_Status 0x%X/0x%X", 
				CPRI_Lock_Val[CPRI_0], 
				CPRI_Lock_Val[CPRI_1],
				(INT16U)m_Reg_Sts.AXI[CPRI_0].Mscl_Status.Data, 
				(INT16U)m_Reg_Sts.AXI[CPRI_1].Mscl_Status.Data);		
	#else //----------------- PRU
		print_agent( DEBUG_SYSTEM, VT_BOLD_YELLOW, "[Initial Lock-St] CPRI0/1 0x%X/0x%X, Mscl_Status 0x%X/0x%X", 
				CPRI_Lock_Val[CPRI_0], 
				CPRI_Lock_Val[CPRI_1],
				A_MMAP_St[CPRI_0].Mscl_Status.Data, 
				A_MMAP_St[CPRI_1].Mscl_Status.Data);		
	#endif			
	}

#ifdef __disable_frequent_debug__	
	OSTimerReset( &cpri_alm_disp_time );
#endif
	
	if(CPRI_Lock_Prev != m_St.Is_CPRI_Lock)  // Checked
	{
		OSTimerReset( &cpri_alm_disp_time ); // 2019.09.19: Added.
		
		if(m_St.Is_CPRI_Lock) // Checked
			print_agent( DEBUG_SYSTEM, VT_BOLD_GREEN, "CPRI Unlock --> Lock");
		else
			print_agent( DEBUG_SYSTEM, VT_MAGENTA, "CPRI Lock --> UnLock");
	}	
	else
	{
		if(m_St.Is_CPRI_Lock) // Checked
		{
			OSTimerReset( &cpri_alm_disp_time );
		}
		else // CPRI-Unlock
		{			
			//-------------------------------------------
			// every 10[s]
			if( OSTimerGet_msec(&cpri_alm_disp_time) > (10*TICK_CO_SEC) )
			{
				OSTimerReset( &cpri_alm_disp_time );

		#ifndef __DEL__Unlock_Msg__
				print_agent( DEBUG_SYSTEM,VT_YELLOW, "[Unlock] Unlock, CPRI0 0x%X / CPRI1 0x%X (if lock, 0x0F)", 
					CPRI_Lock_Val[CPRI_0], CPRI_Lock_Val[CPRI_1]);
					
			#ifdef __TYPE__AAU__ // ANTS-AAU
				print_agent( DEBUG_SYSTEM, VT_YELLOW, "[Unlock] CPRI Ref CPRI%d, Mscl_Status 0x%X/0x%X",
						m_St.Ref_CPRI_No,
						(INT16U)m_Reg_Sts.AXI[CPRI_0].Mscl_Status.Data,	
						(INT16U)m_Reg_Sts.AXI[CPRI_1].Mscl_Status.Data);		
			#else //----------------- PRU
				print_agent( DEBUG_SYSTEM, VT_YELLOW, "[Unlock] CPRI Ref CPRI%d, Mscl_Status 0x%X/0x%X",
						m_St.Ref_CPRI_No,
						A_MMAP_St[CPRI_0].Mscl_Status.Data,	
						A_MMAP_St[CPRI_1].Mscl_Status.Data);		
			#endif
		#endif
			}
		}
	}
	CPRI_Lock_Prev = m_St.Is_CPRI_Lock;
}
//---------------------------------------------------------------------------------
/*
---------------------------------------------------------------------------------
	2019.08.12 : 
	1. dpd_on  이면, register 0x80, register 0x04 모두 0x00으로 write
	2. dpd_off 이면,
		register 0x80(Reg_DSP_Common_MIMO_RW_EQ_Common)
		A. 알파 PAU EQ 정보가 ‘00’경우(기존 값 유지) : 0-path : 01, 1-Path :01, 2-Path: 02, 3-Path:01
		B. 알파 PAU EQ 정보가 ‘00’이 아닌 경우	   : 0-path : 01, 1-Path :02, 2-Path: 02, 3-Path:02
---------------------------------------------------------------------------------		
*/






#ifdef __TYPE__PRU__ // ANTS-PRU
void Proc____PRU_PAU_EQ_Status(void)
{	
#ifdef _gREMS_TEST_
#else
	INT8U fa;
	INT8U m;
	INT8U EQ_PAU[MAX_FA_PATH];
	INT8U EQ_Reg, req_eq;
	INT8U reg08_val[MAX_DL][2] = {{1,1},{1,2},{2,2},{1,2}}; // register 0x80
	INT16S dig_att_req_x10;
	static INT16U fail_cnt = 0;
	static INT16U fail_cnt2 = 0;
	
	for(m = 0; m < MAX_DL; m++)
	{		
		if(m_St.d_DPD_OnOff[m] == ON)
		{
			EQ_PAU[0] = 0;
			EQ_PAU[1] = 0;
		}
		else // OFF
		{
			EQ_PAU[0] = (m_St.Pwr_Amp_EQ[m] & 0xF0)>>4; // 100M
			EQ_PAU[1] = (m_St.Pwr_Amp_EQ[m] & 0x0F);    // Alpha
		}		
		
		for(fa = 0; fa < MAX_FA_PATH; fa++)
		{			
			EQ_Reg = m_Reg_Sts.General_FA[fa].General_FA_MIMO[m].RW_PAU_EQ_St.val;
			
			if(EQ_Reg != EQ_PAU[fa])
			{
				Write_FPGA_Reg(Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_PAU_EQ_Status], (INT32U)EQ_PAU[fa], "PAU EQ");
				fail_cnt++;
				
				if(fail_cnt > 1000)
				{
					fail_cnt = 0;
					print_agent( DEBUG_SYSTEM, VT_YELLOW, "[PRU_PAU_EQ_Status] FA%d.Path%d reg_val(0x%X) != req_val(0x%X)",
						fa, m, EQ_Reg, EQ_PAU[fa]);
				}
			}
			else fail_cnt = 0;

			if(fa == FA_1)
			{						
				if((m_St.d_DPD_OnOff[m] == OFF)&&(EQ_PAU[FA_1] > 0))
				{
					dig_att_req_x10 = m_Eep.d_Att_Dig_DL_x10[fa][m] - 10; // minus 1.0[dB]
					if(dig_att_req_x10 < 0)
						dig_att_req_x10 = 0;
				}
				else
					dig_att_req_x10 = m_Eep.d_Att_Dig_DL_x10[fa][m];				
			
				if(m_St.d_Att_Dig_DL_x10[fa][m] != dig_att_req_x10)
				{
					PRU_DL_Gain_Set_Req(FA_1, m, dig_att_req_x10);
				}
			}
		}
		/*
		2019.08.12
			A. 알파 PAU EQ 정보가 ‘00’경우(기존 값 유지) : 0-path : 01, 1-Path :01, 2-Path: 02, 3-Path:01
			B. 알파 PAU EQ 정보가 ‘00’이 아닌 경우       : 0-path : 01, 1-Path :02, 2-Path: 02, 3-Path:02
		*/
		if(m_St.d_DPD_OnOff[m] == ON)
		{
			req_eq = 0;
		}
		else // OFF
		{
			if(EQ_PAU[FA_1] == 0) req_eq = reg08_val[m][0];
			else                  req_eq = reg08_val[m][1];
		}
		
		if(m_Reg_Sts.DSP_Common_MIMO[m].RW_EQ_Common.val != req_eq)
		{
			Write_FPGA_Reg(Reg_DSP_Common_MIMO_Addr[m][Reg_DSP_Common_MIMO_RW_EQ_Common], (INT32U)req_eq, "EQ Common");
			fail_cnt2++;
				
			if(fail_cnt2 > 1000)
			{
				fail_cnt2 = 0;
				print_agent( DEBUG_SYSTEM, VT_YELLOW, "[PRU_PAU_EQ_Status] EQ_Common.Path%d reg_val(0x%X) != req_val(0x%X)",
					m, 
					m_Reg_Sts.DSP_Common_MIMO[m].RW_EQ_Common.val, req_eq);
			}
		}
		else fail_cnt2 = 0;
	}
#endif	
}
#endif

#ifdef __TYPE__AAU__
void AAU__RF_Max_Att(INT8U path)
{
	INT16U att_x4;
	
	if(path >= AMPU_4_MAX) return;

	// 2019.07.15 : RF_ATT Max
	att_x4 = 30*4;
	RF_Att_Accuracy_x4[DL][path] = 2*Get_AAU_RF_Att_Accuracy_x2(DL, path, att_x4/2); // 30dB
	att_x4 += RF_Att_Accuracy_x4[DL][path];
	m_Reg_Flg.RFU[path].DL_Att.Data = _SET_;	
	m_Reg_Ctl.RFU[path].DL_Att.Trig = 1;
	m_Reg_Ctl.RFU[path].DL_Att.Val_x4 = att_x4;
	Write_FPGA_Reg(Reg_RFU_CTRL_Addr[path][Reg_RFU_CTRL_RW_DL_ATT], m_Reg_Ctl.RFU[path].DL_Att.Val_x4, "RF DL ATT Max");

	att_x4 = 30*4;
	RF_Att_Accuracy_x4[UL][path] = 2*Get_AAU_RF_Att_Accuracy_x2(UL, path, att_x4/2); // 30dB
	att_x4 += RF_Att_Accuracy_x4[UL][path];
	m_Reg_Flg.RFU[path].UL_Att.Data = _SET_;	
	m_Reg_Ctl.RFU[path].UL_Att.Trig = 1;
	m_Reg_Ctl.RFU[path].UL_Att.Val_x4 = att_x4;
	Write_FPGA_Reg(Reg_RFU_CTRL_Addr[path][Reg_RFU_CTRL_RW_UL_ATT], m_Reg_Ctl.RFU[path].UL_Att.Val_x4, "RF UL ATT Max");
}
#endif



/*
---------------------------------------------------------------------------------
Factory mode
------------------
1. PRU: 
  - PAU Insert 상태 "실장" 처리  
  
2. 공통(AAU/PRU)
  - 부팅 시 기본값 OFF.
  - 데이터 저장은 없음.
  - RU Init status 절차 생략 (출력 ON 가능)
  - On시 CPRI or Clock Fail 등으로 Shutdown 이 되지 않도록 처리
  
---------------------------------------------------------------------------------	
*/
void Set_Factory_Mode_Onoff(INT8U onoff)
{
	if(onoff == ON) m_Env.Factory_Onoff = 1;
	else            m_Env.Factory_Onoff = 0;
}

INT8U Get_Factory_Mode(void)
{
	INT8U onoff;
	
	onoff = (m_Env.Factory_Onoff == 0)? OFF:ON;
	
	return onoff;
}





INT8U IS__Board_St_Normal(void)
{
	INT8U clock_st;
	
	m_Reg_Sts.R_Board_Status.Data = Read_FPGA_Reg(Reg_Common_Addr[Reg_Common_R_Board_Status]);
	
	/*
	2019.12.26 : Clock alarm : string 표시 항상 정상으로만 되는 것 수정.
	2020.02.19 : register를 읽을 값 mapping 변경 (m_Reg_Sts.R_Board_Status.Data <-> Clock_Status (AID 0xB41A))
	*/
	clock_st = 0; // init
	if((m_Reg_Sts.R_Board_Status.Data & cbits[2])==0) clock_st |= cbits[5]; // reg [2] : JESD Lock,   Lock ='1' : bit5 : ADC/DAC JESD
	if((m_Reg_Sts.R_Board_Status.Data & cbits[1])==0) clock_st |= cbits[4]; // reg [1] : PLL/AFE Init Done ='1' : bit4 : ADC/DAC
	if((m_Reg_Sts.R_Board_Status.Data & cbits[0])==0) clock_st |= cbits[6]; // reg [0] : PLL Lock,    Lock ='1' : bit6 : DCM(Main)	
	m_St.d_Clock_Status = clock_st; // 2020.02.19

	if(Get_Factory_Mode() == ON)	
	{
		return 1;
	}

	if(m_Reg_Sts.R_Board_Status.Pll_Init_Done && m_Reg_Sts.R_Board_Status.JESD_Lock)
		return 1;
	else
		return 0;
}





void AAU___AMP_Off_Max_Att(void)
{
#ifdef __TYPE__AAU__	
	INT8U path;
	
	for(path = 0; path < AMPU_4_MAX; path++)
	{
		AAU__RF_AMP_OnOff(path, OFF, 10161);	
		AAU__RF_LNA_OnOff(path, OFF, 10162);
		AAU__RF_Max_Att(path);		
		
		/*
		//DTU_DL_Path_Onoff(FA_0, path, OFF); // AAU X
		//DTU_UL_Path_Onoff(FA_0, path, OFF); // AAU X
		//DTU_DL_Path_Onoff(FA_1, path, OFF); // AAU X
		//DTU_UL_Path_Onoff(FA_1, path, OFF); // AAU X
		*/
	}
#endif
}

void AAU___Restore_AMP_Att(void)
{
/*
//#ifdef __TYPE__AAU__
//	INT8U i;
//	
//	for(i = 0; i < AMPU_4_MAX; i++)
//	{		
//		AAU_RF_DL_Att_Set(i, m_Eep.DL_Att_User[i]/2);
//		AAU_RF_UL_Att_Set(i, m_Eep.UL_Att[i]/2);		
//		AAU__RF_AMP_OnOff(i, m_Eep.Pwr_Amp_En[i], 12321);
//	}
//#endif		
*/
}



//-------------------------------------------
#ifdef __TYPE__PRU__
//-------------------------------------------

/*
------------------------------------------
2019.10.25
if "DPD_DCL reset" is requested..
------------------------------------------
*/

void Proc___DPD_DCL_Reset(void)
{
	INT8U path4;
	
	for(path4 = 0; path4 < AMPU_4_MAX; path4++)
	{		
		/* 2019.12.04 : 미사용 Layer에 DPD Reset 하지 않도록 수정. */
		if(Check_PAU_Use(path4)==0) 
		{
			m_Env.DPD_DCL_Rst_Req[path4] = 0;			

			if((m_Reg_Ctl.DPD[path4].DPD_Reset.uD32 & 0x01)==0)
			{
				m_Reg_Flg.DPD[path4].DPD_Reset.uD32 = _SET_;
				m_Reg_Ctl.DPD[path4].DPD_Reset.uD32 = 1; // DPD Normal

				Write_FPGA_Reg(Reg_DPD_Addr[path4][Reg_DPD_RESET], 1, "Reg_DPD_Normal");

				print_agent(DEBUG_STDBY, VT_BOLD_RED, "[PATH_%d Not Used] ---------------", path4);
				print_agent(DEBUG_STDBY, VT_BOLD_RED, "[PATH_%d Not Used] DPD_DCL Normal Set", path4);
				print_agent(DEBUG_STDBY, VT_BOLD_RED, "[PATH_%d Not Used] ---------------", path4);
			}
			
			continue;
		}
		
		if(m_Env.DPD_DCL_Rst_Req[path4])
		{
			m_Env.DPD_Rst_Normal_Chk_Cnt[path4] = DPD_Rst_Chk_MAX_Cnt;
			m_Env.DPD_DCL_Rst_Req[path4] = 0;
			Write_FPGA_Reg(Reg_DPD_Addr[path4][Reg_DPD_RESET], 0, "Reg_DPD_RESET");

			//m_Reg_Flg.DPD[path4].DPD_Reset.uD32 = _SET_;
			//m_Reg_Ctl.DPD[path4].DPD_Reset.uD32 = 0; // 0 : DPD Reset

			print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[DPD_DCL#%d_Reset] Request (%d)", 
				path4+1, m_Env.DPD_Rst_Normal_Chk_Cnt[path4]);
		}
	}
}
/*
------------------------------------------
2019.10.25
if current 'OFF' -> 'ON'
reset DPD_DCL
------------------------------------------
*/
void Check_DPD_DCL_Reset(INT8U path4, INT8U onoff)
{	
	if(path4 >= AMPU_4_MAX) return;
	if(onoff != ON) return;
	if(m_St.Pwr_Amp_En[path4] != OFF) return;
	
	/* 2019.11.29 : 미사용 Layer에 DPD Reset 하지 않도록 수정. */
	if(Check_PAU_Use(path4)==0) 
	{
		m_Env.DPD_DCL_Rst_Req[path4] = 0; // 2019.12.04
		return;
	}
	
	m_Env.DPD_DCL_Rst_Req[path4] = 1;

	//print_agent(DEBUG_STDBY, VT_BOLD_YELLOW, "[Check_DPD_DCL_Reset] #%d", path4+1);
}
#endif



void Proc___AAU_GainCheck_NoiseDet(void);

#define _INIT_Delay_ms_FPGA_ (5*OS_DELAY_1sec) // 2019.07.12 : time delay for FPGA Ready : 2[s] -> 5[s]
#define _MAX_DTU_Fail_Cnt_ (50) // 2019.09.20 : Reboot max 10회 -> max 50회 수정.

void* FPGA_Register_Task(void *data)
{	
	INT32S res;
	Thread_ARG_Var* p_th_arg;	
	INT8U path;
	INT8U init_task = 0;
	int cnt = 0;	
	OS_TIMER fpga_comm_time;
	OS_TIMER board_Sts_time;
	int fpga_comm_chk = 0; // 0(init), 1~ (fail, every 10[s])
	int board_Sts_chk = 0; // 0(init), 1~ (fail, every 10[s])
	
#ifdef __TYPE__AAU__
	INT8U fwd_alc_cnt = 0;
	/*
	------------------------------------		
	2019.07.26. : AAU RX ALC Timeing : 200[ms] -> 600[ms]
	2019.09.17. : rvs_alc_cnt 사용하지 않음.
	------------------------------------
	INT8U rvs_alc_cnt = 6; // 600[ms]
	*/
	INT8U att_set;
	INT8U alc_cnt[AMPU_4_MAX] = {0,0,0,0};
	INT8U quick_alc_cnt = 0;
	INT8S AMP_Init_End[AMPU_4_MAX] = {0,0,0,0};
	INT8U i;
	INT8U AAU_Init_AMP_Set[AMPU_4_MAX] = {0,0,0,0}; // 2020.01.10
#endif	
	
	p_th_arg = (Thread_ARG_Var*)data;

 	Thread_Priority_Init( p_th_arg, 1);

	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "FPGA_Register Task RUN" );

	res = pthread_mutex_init(&FPGA_RdWr_Sem, NULL);
	if (res != 0) 
	{
		syslog( LOG_ERR, "[RCU] FPGA_Register Task Mutex1 Init Fail");
		exit(EXIT_FAILURE);
	}	
	
	#ifdef __TYPE__PRU__ // ANTS-PRU Only
	Open_A_PRU_Mmap();
	Open_AXI_Reg();	
	#else // AAU
	AAU___EQ_Toggle_Set();
	
	for(i = 0; i <AMPU_4_MAX; i++)
	{
		RF_Att_Accuracy_x4[DL][i] = 0; // init
		RF_Att_Accuracy_x4[UL][i] = 0; // init
	}
	#endif		

	Init_All_Reg_Ctrl_Flag();
	Calculate_Reg_Address();	
	Calculate_Reg_Variable_Pointer(&m_Reg_Sts, STS);
	Calculate_Reg_Variable_Pointer(&m_Reg_Ctl, CTL);
	Calculate_Reg_Variable_Pointer(&m_Reg_Flg, FLG);

	//----------------------------------
	// DTU-Register Init
	//----------------------------------	
	DTU_Reg_Link_Fail = 0; // default

	OSTimerReset( &cpri_alm_remain_time );
	OSTimerReset( &cpri_alm_disp_time );

	OSTimerReset( &fpga_comm_time);
	OSTimerReset( &board_Sts_time);

	//----------------------------------
	OSTimeDly(_INIT_Delay_ms_FPGA_); 
	//----------------------------------	
	
	// 2019.04.08
#ifdef __TYPE__PRU__	
	PRU_Reg_LED_Red(_Reg_LED_OFF);
	PRU_Reg_LED_Green(_Reg_LED_OFF);
#else // AAU, AMP Default OFF	
	AAU___AMP_Off_Max_Att();
#endif
	//-------------------------------------------------------	
	init_task = 0;
	cnt = 0;
	m_St.Gain_Noise_Auto = 0; // default OFF Start
	m_St.d_Loopback_OnOff = OFF; // default OFF Start
	//-------------------------------------------------------
	// init status is un-lock
	m_St.Ref_CPRI_No = CPRI_0;
	m_St.Is_CPRI_Lock = 0;
	m_Env.CPRI_Lock_St = 0;	

	// 2019.07.18 : Loopback On/Off : default OFF
	m_St.d_Loopback_OnOff = OFF;

	// 2019.08.28
#ifdef __TYPE__PRU__
	m_Env.Delay_Adjust_ns = 87;
	m_Env.Tsync_Offset_ns = 500;
	m_Env.AAU_gain_chk_level = 0;
#else
	m_Env.Delay_Adjust_ns = 0;
	m_Env.Tsync_Offset_ns = 0;
	m_Env.AAU_gain_chk_level = 40; // 2019.09.16 : default 40 : CW Lv을 기존 '8' --> '40' 변경
#endif	

	//--------------------------------------------------------------


	#ifdef __TYPE__AAU__				
	for(path = 0; path < AMPU_4_MAX; path++)
	{
		if(IS__End_of_UL_Gain_Low_Offset(path)) // if end('F') 
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[End_of_UL_Gain_Low_Offset] Path_%d END"); 
	}
	#endif	
	
	//-------------------------------------------------------
	while( m_Env.task_flag[_TASK_FPGA_REGISTAER_] )		///// YCSO TASK END
	{	
		OSTimeDly(OS_DELAY_100msec); // 100[ms] period must
				
		Update__Task_Count(_TASK_FPGA_REGISTAER_);
		task_cnt++;
		
	#ifdef __AAU_WATCHDOG_TIME_TEST__
	#else
		if((task_cnt%5)==0) watchdog_live(); // 2019.09.10
	#endif
		
		/*		 
			{Test Address(0x02) Access가 비정상}인 경우 
			경고 Message : "DTU Initialize Fault: FPGA fail" 10초 주기로 출력(RED)
		
		*/
		if(Check_Fpga_Comm_Link() == CHK_FAIL)
		{			
			if(OSTimerGet_msec(&fpga_comm_time) > (10*TICK_CO_SEC))
			{
				OSTimerReset(&fpga_comm_time);
				fpga_comm_chk++;
				print_agent(DEBUG_SYSTEM, VT_RED, "DTU Initialize Fault: FPGA fail(cnt %d)", fpga_comm_chk);
				
			#ifdef __TYPE__AAU__				
				/*
				2019.09.20 : Reboot max 10회 -> max 50회 수정.
				*/				
				if(m_Eep.AAU_DTU_Comm_Fail_Cnt > _MAX_DTU_Fail_Cnt_) // max 50, if invalid
					m_Eep.AAU_DTU_Comm_Fail_Cnt = 0;
					
				if(m_Eep.AAU_DTU_Comm_Fail_Cnt < _MAX_DTU_Fail_Cnt_) // max 50
				{
					m_Eep.AAU_DTU_Comm_Fail_Cnt++;
					m_Env.is_eep_write = EEP_WRITE_CODE;
					
					print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "!!  RCU Reboot by FPGA-Fail : Cnt %d  !!", m_Eep.AAU_DTU_Comm_Fail_Cnt);
					print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

					SetResetInfo(Rst_By_Etc, 13582); // 2019.10.22 : user -> etc, 별의미는 없음.
					m_Env.is_reboot = 1;
				}
			#endif
			}
			//-----------
			continue;
			//-----------
		}
		else 
		{
			OSTimerReset(&fpga_comm_time);
			fpga_comm_chk = 0;
			
		#ifdef __TYPE__AAU__				
			if(m_Eep.AAU_DTU_Comm_Fail_Cnt > 0)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "!!  Clear AAU_DTU_Comm_Fail_Cnt  !!");
						
				m_Eep.AAU_DTU_Comm_Fail_Cnt = 0;
				m_Env.is_eep_write = EEP_WRITE_CODE;
			}				
		#endif
		}
		//--------------------------------------------
		/*
			{Test Address(0x02) Access}가 정상이나 
			{Board Status(0x07)의 Bit 1,2 가 비정상} 상태로 1분 경과 후 
			경고 Message : "DTU Initialize Fault: clock fail" 10초 주기로 출력(RED)
		*/
		
		if(IS__Board_St_Normal()==0)
		{
			//print_agent(DEBUG_SYSTEM, VT_RED, "DTU Board_St Abnormal --");
			//OSTimeDly(OS_DELAY_1sec);
			
			if( OSTimerGet_msec(&board_Sts_time) > (10*TICK_CO_SEC))
			{
				OSTimerReset(&board_Sts_time);
				board_Sts_chk++;
				
				if(board_Sts_chk >= 6)
					print_agent(DEBUG_SYSTEM, VT_RED, "DTU Initialize Fault: clock fail(cnt %d)", board_Sts_chk);				
			}
		}
		else 
		{
			OSTimerReset(&board_Sts_time);
			board_Sts_chk = 0;
		}
		//--------------------------------------------

		
		//------------------------------------
		Proc__Check_ClockFail();
	#ifdef __TYPE__PRU__  // ANTS-PRU			
		Proc____PRU_LED();
		Proc____PRU_PAU_EQ_Status();
	#else
		Proc____AAU_LED();
		Proc____AAU_Alarm_Fan_and_Temp();
	#endif
		//------------------------------------		
		// one-time control when start
		if(init_task == 0)
		{
			cnt++;
			if(cnt == 1) 
			{
				Init_Vss_Stable_Check(1);
			}
			Proc_Read_Fpga_Register();

			/*			
				{Test Address(0x02) Access로 판단} && {Board Status(0x07)의 Bit 1,2 정상 여부 확인} 추가
			*/
			if(IS__Board_St_Normal()&&(Check_Fpga_Comm_Link() == CHK_OK))
			{
				Copy__RegSts__to__RegCtl();
				Write_Ctrl_Data_From_Eep(); 
				init_task = 1;
				cnt = 0;
				print_agent(DEBUG_SYSTEM, VT_YELLOW, "FPGA-Reg-Task Init. (Pll_Init_Done, JESD_Lock)");				
			}
			else
			{
				if((cnt%100) == 0) // every 10[s]
				{
					if(IS__Board_St_Normal() == 0)
						print_agent(DEBUG_SYSTEM, VT_MAGENTA, "Board_Status 0x%X", m_Reg_Sts.R_Board_Status.Data);
					else if(Check_Fpga_Comm_Link() == CHK_FAIL)
						print_agent(DEBUG_SYSTEM, VT_MAGENTA, "Fpga_Comm_Link Fail");
				}
			}
			//-----------
			continue; 
			//-----------
		}
		else
		{		
		#ifdef __TYPE__AAU__  // ANTS-AAU
			if(m_Env.AMP_Ctrl_Allowed==0) // 2019.09.24
			{
				for(path = 0; path < AMPU_4_MAX; path++)
					AMP_Init_End[path] = 0; // for initialization
			}
			else
			{
				for(path = 0; path < AMPU_4_MAX; path++)
				{				
					if(Check_PAU_Use(path) == 0) continue;
					if(IS_RU_Init_Completed() || m_Env.AMP_User_Ctrl[path]) // or user on control
					{
						if(AMP_Init_End[path] == 0)
						{
							
							//---------------------------------------------------
							// 2020.01.10 : amp off reason set by user if user-amp-off controlled.
							if(AAU_Init_AMP_Set[path] == 0)
							{
								AAU_Init_AMP_Set[path] = 1;
								if(m_Eep.Pwr_Amp_En[path] == OFF)
									Set__Reason_Dis_5117(path, Rsn_Dis_User___Ctrl); 
							}
							
							AAU__RF_AMP_OnOff(path, m_Eep.Pwr_Amp_En[path], 12548);
							AMP_Init_End[path] = 1;
						}
					}
				}
			}			
		#endif			
		}
		//------------------------------------		
		Proc__Update_CPRI_Mscl_Status();
		Proc__SFP_Insert__CPRI_Lock();
	#ifdef __TYPE__PRU__  // ANTS-PRU			
		Proc_CPRI_Lock();		
	#endif
		Proc__Disp_CPRI_Lock();
		//------------------------------------			
		Chk_RegCtrlFlag_Status();
		Proc_LS_Vss_Aive_Ack();
		//------------------------------------		
	#ifdef __TYPE__AAU__  // ANTS-AAU
		Proc___AAU_Loading_100M_Test_Pattern();
		Proc___AAU_Loading_Test_Pattern();
	#endif
		//------------------------------------		
		if( m_Env.wave_req == _SET_)
		{
//			printf("check_YCSO ----------------------\n");
			Proc_RW_wave_Register();
			m_Env.wave_req = _CLR_;
			continue;
		}
		//------------------------------------
		Proc_Read_Fpga_Register();
		Proc__VSS_Monitor();
		Copy_Data_From_VSS();
		Proc__CPRI_Init_OSM_Ctrl(); // 2019.05.21
		Proc_Conv_DTU_Reg_to_mSt();
		Proc__Chk_and_Disp__TSSI_Level();
		//------------------------------------		
		Proc___Ctrl_from_LS();
		//------------------------------------		
		Proc___Hard_Clipping();
		Proc___BIP__ErrCount();		
	//-----------------------------------------------
	#ifdef __TYPE__AAU__ // ANTS-PRU
	//-----------------------------------------------		
		Proc___AAU_BW_Att();
		Proc_AAU_Ext_FAN_Ctrl();
		fwd_alc_cnt++;		
		//------------------------------------
		Proc__AAU__Get_EQ_Att_x10();
		quick_alc_cnt++;

		
		
		for(path = 0; path < MAX_MIMO_PATH; path++)
		{
			Get__AAU_TotPwr_x10_UL_RSSI(path); // offset TEST				
			Proc___AAU_RFU_Init(path); // 2019.04.21

			/*
			// 2019.09.17 : 
			"m_Env.AAU_alc_max_cnt = 8" fix.
			rvs_alc_cnt 사용하지 않음.
			
			//#ifdef __USE_AAU_ALC_SPEED_CTRL__
			//#else
			//if((quick_alc_cnt%rvs_alc_cnt)==0) 
			//#endif
			*/
			{
				att_set = Proc___AAU_RVS_In_ALC(path);
				
				if(att_set == 0)
				{
					m_St.RSSI_Before_Filter_x10[path] = Get_Other_Company_Det_x10(path);	
					
					alc_cnt[path]++;
					if(alc_cnt[path] & 0x01)
						Proc___AAU_RVS_Quick_ALC(path);
				}					
			}
			Proc___Calc_AAU_UL_Gain(path); // 2019.05.23			
			Proc___AAU_Rvs_Shutdown(path); // 2019.04.21
			Proc___AAU_UL_Gain_Low(path);  // 2019.05.11

			if((fwd_alc_cnt%5)==0)
			{
				Proc___AAU_FWD_ALC(path);      // 2019.05.11
				Proc___AAU_Fwd_Shutdown(path); // 2019.04.22
			}
			
			Proc___AAU_RFU(path);          // 2019.04.21
			Proc___AAU_Temp_Att(path);     // 2019.05.17
			Proc___AAU_DL_CFR_Adj(path);
		}
		Proc___LSH_Gain_Set();
		Proc___AAU_ALC_OFF_Check();
	//-----------------------------------------------		
	#else // PRU
	//-----------------------------------------------		
		Proc___LSH_Gain_Set();
		Proc___Test_Pattern_Band_Gain();
		Proc___DPD_St_DL_Delay_Offset();
		Proc___PRU_Tsync_Delay();
		Proc___PRU__BW_Gain_Compensation();
		
		for(path = 0; path < MAX_MIMO_PATH; path++)
			Proc___PRU_Rvs_OtherCompany__Over_Input(path);
	//-----------------------------------------------			
	#endif		
	//-----------------------------------------------		
		Proc___Off_Test_Pattern();
		//-----------------------------------------------
		Proc___DTU_SD__Check();
		for(path=0; path < AMPU_4_MAX; path++)
		{			
			Alarm_SD_Fwd(path);
			Alarm_SD_Rev(path);
		}		
		Proc___Shutdown_Ctrl(); // DTU Shutdown, 2019.04.19			
		//-----------------------------------------------			
		Proc_LS_VSS_Channel();
		Proc_LS_VSS_Copy_CPRI();
		
	//-----------------------------
	#ifdef __TYPE__PRU__ // ANTS-PRU
	//-----------------------------					
		Proc___PRU_DPD_Alarm();
		Proc___DPD_Reset_to_Normal(); // 2019.05.01
		Proc___DPD_DCL_Reset(); // 2019.10.25
		Proc__PRU_Bal_DTU_Att();
	//-----------------------------
	#else               // ANTS-AARU
	//-----------------------------
		Proc___AAU_Reg_PLL();
		Proc___AAU_GainCheck_NoiseDet(); // 2019.06.01
		Proc___AAU_DL_Det_SSB(); // 2019.09.25
		Proc___AAU_DL_CW_Off(); // 2019.09.25
	//-----------------------------
	#endif
	//-----------------------------		
		if(IS__Board_St_Normal())
			Proc_Compare_Fpga_Register(); // PRU, AAU (Write desire values)

	#ifdef __TYPE__AAU__ // ANTS-PRU
		Proc_Compare_Fpga_Register_RF();
	#endif
	
		Proc_DTU_Alarm();
	}

	// 2019.07.15 : AMP OFF, Att Max
#ifdef __TYPE__AAU__		
	for (path = 0; path < AMPU_4_MAX; path++)
	{
		AAU__RF_AMP_OnOff(path, OFF, 11673);	
		AAU__RF_LNA_OnOff(path, OFF, 11674);
		AAU__RF_Max_Att(path);
	}
#endif
	
	m_Env.task_END_status[_TASK_FPGA_REGISTAER_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}
//-----------------------------------------------------------------------------------------










//--------------------------------------------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU
INT16S Get__PRU_TotPwr_x10_UL_RSSI(INT8U path4)
{
	// 2019.05.10
	double d_RSSI[MAX_Optic];
	INT16S tot_pwr_x10;
	
	d_RSSI[_100M] = (double)(m_St.d_UL_RSSI_dBm_x10[_100M][path4])/10.0;
	d_RSSI[Alpha] = (double)(m_St.d_UL_RSSI_dBm_x10[Alpha][path4])/10.0;

	tot_pwr_x10 = 100*log10(pow(10.00, d_RSSI[_100M]/10.0)+pow(10.00, d_RSSI[Alpha]/10.0));
	
	return tot_pwr_x10;	
}
#endif
//--------------------------------------------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU
// DTU_UL = user offset + gain backoff
void Set_PRU_RVS_DTU_Att_x10(INT8U optic, INT8U path4, INT16S user_att_x10)
{
	INT16S gain_backoff_x10;
	INT16S gain_bal_x10;
	
	if(optic >= MAX_Optic) return;	
	if(path4 >= AMPU_4_MAX) return;		

	if(user_att_x10 > (10*10)) user_att_x10 = 10*10; // 10dB max ??
	
	m_Eep.d_Att_Dig_UL_Offs_x10[optic][path4] = user_att_x10;
	gain_backoff_x10 = m_St.d_UL_DTU_Gain_Backoff_x10[optic][path4];
	gain_bal_x10 = m_St.d_UL_GainBal_x10;
	
	m_Reg_Flg.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32 = 1;
	m_Reg_Ctl.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32 = (gain_backoff_x10 + user_att_x10 + gain_bal_x10);
}
#endif
//--------------------------------------------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU
// DTU_UL = user offset + gain backoff
void Set_PRU_UL_Gain_Backoff_x10(INT8U optic, INT8U path4, INT16S gain_backoff_x10)
{
	INT16S user_att_x10;
	INT16S gain_bal_x10;
	
	if(optic >= MAX_Optic) return;	
	if(path4 >= AMPU_4_MAX) return;
	
	user_att_x10 = m_Eep.d_Att_Dig_UL_Offs_x10[optic][path4];
	gain_bal_x10 = m_St.d_UL_GainBal_x10;
	
	m_Reg_Flg.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32 = 1;  // gain-backoff
	m_Reg_Ctl.General_FA[optic].General_FA_MIMO[path4].RW_UL_Gain.uD32 = (gain_backoff_x10 + user_att_x10 + gain_bal_x10);
}
#endif
//--------------------------------------------------------------


//--------------------------------------------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU
// 2019.05.08 : RVS 타사 과입력 PRU
void Set_PRU_RVS_Gain_Comp_x10(INT8U path4, INT16S gain_x10)
{
	/////////////////////////return; // 2019.09.04 TEST v09.04.99
	
	if(path4 >= AMPU_4_MAX) return; 	

	if(gain_x10 >= 20*10) gain_x10 = 20*10;	

	m_Reg_Flg.DSP_Common_MIMO[path4].UL_ALC_GAIN.Data = _F_SET_;
	m_Reg_Ctl.DSP_Common_MIMO[path4].UL_ALC_GAIN.value = gain_x10;
}
#endif
//--------------------------------------------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU
INT16S Get_PRU_RVS_Gain_Comp_x10(INT8U path4)
{
	INT16S gain_x10;
	
	gain_x10 = m_Reg_Sts.DSP_Common_MIMO[path4].UL_ALC_GAIN.value;

	if(gain_x10 < 0) gain_x10 = 0;
	
	return gain_x10;
}
#endif
//--------------------------------------------------------------
#ifdef __TYPE__PRU__ // ANTS-PRU
#define PRU_Q_Hystresis_Att_x10 (5) // 0.5dB
#define PRU_Before_Filter_Offset_x10 (-299) // -29.9[dB], 2019.05.28

INT8U Proc___PRU_Rvs_OtherCompany__Over_Input(INT8U path4)
{
	INT16S  ctl_gain_x10;
	INT16S  cur_comp_gain_delta_x10;
	
	INT8U  ALC__Onoff;
	INT16S level_x10;

	INT16S diff_x10;
	INT16S pau_rvs_alc_att_x10;
	INT16S max_comp_range_x10;
		
	INT8U  att_set = 0; 
	INT8U  ctrl_prev_x10 = 0;

	INT16S rssi__in_x10;
	INT16S rssi_out_x10;
	INT16S rssi_out_gui_x10;

	INT16S pwr_diff_x10;
	INT16S rssi_out_calc_x10;
	INT16S rssi__in_calc_x10;
	INT16S amp_rvs_att_x10;
	INT8U  skip_alc;

	static INT8U cnt[AMPU_4_MAX] = {0,0,0,0};

	
	static INT16S rssi_out_x10_pre = 0; 
	static INT8S max_comp_range_x10_pre = 0;
	static INT8S pau_rvs_alc_att_x10_pre = 0; 
	static INT16S cur_comp_gain_delta_x10_pre = 0;
	INT8U  disp = 0;
	INT16S dbg_diff_x10;

	//----------------------
	
	//----------------------	
	if(path4 >= AMPU_4_MAX) return 0;				
	//----------------------
	if(Check_PAU_Use(path4) == 0)
	{
		cnt[path4] = 0;
		return 0;
	}	
	//----------------------
	att_set = 0; // init

	// 2019.08.27 : Always on
	m_Eep.Other_Rvs_IN_ALC_Onoff[path4] = ON;
	ALC__Onoff = ON;
	level_x10  = (-465); // -46.5[dBm], 2019.08.30
	//----------------------
	pau_rvs_alc_att_x10 = 5*m_St.RX_ALC_Att_x2[path4]; // from ATNS-PRU-PAU
	
	cur_comp_gain_delta_x10 = Get_PRU_RVS_Gain_Comp_x10(path4);

	if(cur_comp_gain_delta_x10 < 0)
		Set_PRU_RVS_Gain_Comp_x10(path4, 0);
	
	m_St.UL_DTU_Gain_Comp_x10[_100M][path4] = Get_PRU_RVS_Gain_Comp_x10(path4); // PRU
	m_St.UL_DTU_Gain_Comp_x10[Alpha][path4] = Get_PRU_RVS_Gain_Comp_x10(path4); // PRU
	
	/*
	--------------------------------------------
	RSSI(Filter전, 0xB302) = RSSI DET + PAU Atten(0xB224, 0xB225) - 상수 + GUI Offset(0xC05B)
	타사 + 자사 신호 == PAU_ALC 결과 
	--------------------------------------------
	*/
	rssi__in_x10 = m_St.d_UL_IN_RSSI_dBm_x10[path4]; // include other-company-signal, before filter	
	rssi__in_x10 += 5*(m_St.Rvs_Att_x2[path4] + m_St.RX_ALC_Att_x2[path4]); // 2019.08.27 : PAU Att
	rssi__in_x10 += PRU_Before_Filter_Offset_x10;
	rssi__in_x10 += m_Eep.Offset_RSSI_Before_Filter_x10[path4];
	m_St.RSSI_Before_Filter_x10[path4] = rssi__in_x10;
	
	// 2019.08.26 : 대표값 
	if(m_St.RSSI_Before_Filter_x10[path4] < -920)
		m_St.RSSI_Before_Filter_x10[path4] = -1000;
	/*
	--------------------------------------------
	RSSI(Filter 후, 0xB303)
	자사 신호 = filter 후.
	--------------------------------------------
	*/
	m_St.RSSI_After__Filter_x10[_100M][path4] = m_St.d_UL_RSSI_dBm_x10[_100M][path4];	
	m_St.RSSI_After__Filter_x10[Alpha][path4] = m_St.d_UL_RSSI_dBm_x10[Alpha][path4];
	rssi_out_gui_x10 = Get__PRU_TotPwr_x10_UL_RSSI(path4); // after filter
	//-----------------------------------------------------
	max_comp_range_x10 = pau_rvs_alc_att_x10;
	if(max_comp_range_x10 < 0) max_comp_range_x10 = 0;		
	//-----------------------------------------------------		
	if((m_St.RVS_Amp_Onoff[path4] == OFF)||(ALC__Onoff == OFF)) // if OFF
	{
		cnt[path4] = 0;
		return 0;
	}
	//-----------------------------------------------------		
	// 2019.08.27
	amp_rvs_att_x10   = 5*(m_St.Rvs_Att_x2[path4] + m_St.RX_ALC_Att_x2[path4]);
	rssi__in_calc_x10 = rssi__in_x10 - amp_rvs_att_x10;
	rssi_out_calc_x10 = rssi_out_gui_x10 + cur_comp_gain_delta_x10 - amp_rvs_att_x10;

	INT16S rssi_out_calc_bf_x10 = rssi_out_calc_x10 - cur_comp_gain_delta_x10;
	//-----------------------------------------------------	
	// 타사 신호와 자사신호의 차이.
	// 타사 - 자사 => 즉, 타사가 얼마나 큰지.
	//-----------------------------------------------------		
	pwr_diff_x10 = rssi__in_calc_x10 - rssi_out_calc_bf_x10; // {타사+자사} - {자사}
	//-------------------------------------------------
	if(cur_comp_gain_delta_x10 > max_comp_range_x10)
	{
		ctl_gain_x10 = max_comp_range_x10;
		cnt[path4] = 0;
		
		Set_PRU_RVS_Gain_Comp_x10(path4, ctl_gain_x10);	
	
		if(Is_Debug_Path4(path4))
		{
			print_agent( DEBUG_STDBY, VT_BOLD_RED, "[RVS-COMP] <AMP_%d> gain_comp(%d/10) > amp_alc(%d/10), comp_gain(%d/10) Set", 
				path4, cur_comp_gain_delta_x10, max_comp_range_x10, ctl_gain_x10);
		}	
		return 0;
	}
	//-------------------------------------------------
	rssi_out_x10 = rssi_out_calc_x10;	
	diff_x10= rssi_out_x10 - level_x10; // after-filter, 즉, 자사신호를 ALC
	att_set = 0; // init
	
	if(Is_Debug_Path4(path4))
	{	
		disp = 0;		
		dbg_diff_x10 = rssi_out_x10_pre - rssi_out_x10;
		if((dbg_diff_x10 <(-2))||(dbg_diff_x10 >2))   disp = 1;
		if(max_comp_range_x10_pre != max_comp_range_x10) disp = 1;
		if(pau_rvs_alc_att_x10_pre != pau_rvs_alc_att_x10) disp = 1;
		if(cur_comp_gain_delta_x10_pre != cur_comp_gain_delta_x10) disp = 1;

		if(disp)		
		{
			print_agent( DEBUG_STDBY, VT_BOLD_YELLOW, "[RVS-COMP] <AMP_%d> -----", path4);					
			print_agent( DEBUG_STDBY, VT_BOLD_YELLOW, "[RVS-COMP] <AMP_%d> Status : rssi_out (%d/10) - Limit (%d/10) = (%d/10), max range (%d/10)", 
					path4,					
					rssi_out_x10, level_x10, (rssi_out_x10 - level_x10),
					max_comp_range_x10);
					
			print_agent( DEBUG_STDBY, VT_BOLD_YELLOW, "[RVS-COMP] <AMP_%d> Status : rvs_alc_att (%d/10), curr_comp_gain (%d/10)", 
					path4, pau_rvs_alc_att_x10, cur_comp_gain_delta_x10);
		}	

		rssi_out_x10_pre = rssi_out_x10; 
		max_comp_range_x10_pre = max_comp_range_x10;
		pau_rvs_alc_att_x10_pre = pau_rvs_alc_att_x10; 
		cur_comp_gain_delta_x10_pre = cur_comp_gain_delta_x10;
	}	
	
	//------------------------------	
	if(diff_x10 > 0) // power >= level, over power 이면, ALC를 함.
	{	
		cnt[path4] = 0;	
		
		ctrl_prev_x10 = cur_comp_gain_delta_x10;			
		ctl_gain_x10  = cur_comp_gain_delta_x10 - diff_x10;		
		
		if(ctl_gain_x10 < 0) ctl_gain_x10 = 0;
		//---------------
			
		m_St.UL_DTU_Gain_Comp_x10[_100M][path4] = ctl_gain_x10; // PRU
		m_St.UL_DTU_Gain_Comp_x10[Alpha][path4] = ctl_gain_x10; // PRU

		Set_PRU_RVS_Gain_Comp_x10(path4, ctl_gain_x10);
		
		if(ctrl_prev_x10 != ctl_gain_x10) 
		{
			att_set = 1; // atten set
		
			if(Is_Debug_Path4(path4))
			{
				print_agent( DEBUG_STDBY, VT_BOLD_WHITE, "[RVS-COMP] <AMP_%d> Over (-): diff(%d/10),  comp_gain(%d/10) Set", 
					path4, diff_x10, ctl_gain_x10);
			}		
		}
	}	
	//----------------------------------------------------------
	// Pwr is under 
	// if 'hysterisis 2.0dB' and alc-only-att is above 0[dB](that is cur_att is above user att) 
	//	   then atten release
	//----------------------------------------------------------				
	else if(diff_x10 < (-PRU_Q_Hystresis_Att_x10)) // power < level
	{
		//------------------------------
		/*		
		2019.08.28
		2019.08.30 : {자사+타사} ~ {타사} +/-1.0[dB] 조건을	+/-0.5[dB] 로 수정.		
		*/		
		     if((pwr_diff_x10>(-5))&&(pwr_diff_x10<5))     skip_alc = 1; // {타사} == {자사}	
		else if(rssi__in_calc_x10 <= rssi_out_calc_bf_x10)	skip_alc = 2; // {타사} < {자사}
		else if(m_St.RSSI_Before_Filter_x10[path4] <= -650) skip_alc = 3;
		else skip_alc = 0;

		
		if(Is_Debug_Path4(path4))
		{
			if(skip_alc == 1)
			{
				print_agent( DEBUG_STDBY, VT_BOLD_CYAN, "[RVS-COMP] <AMP_%d> Under(+) Skipped(%d) : diff(%d/10)", 
							path4, skip_alc, pwr_diff_x10);		
			}
			else if(skip_alc == 2)
			{
				print_agent( DEBUG_STDBY, VT_BOLD_CYAN, "[RVS-COMP] <AMP_%d> Under(+) Skipped(%d) : in_x10(%d/10), out_x10(%d/10)", 
							path4, skip_alc, rssi__in_calc_x10, rssi_out_calc_bf_x10);		
			}
			else if(skip_alc == 3)
			{
				print_agent( DEBUG_STDBY, VT_BOLD_CYAN, "[RVS-COMP] <AMP_%d> Under(+) Skipped(%d) : RSSI_Before_Filter_x10(%d/10)", 
							path4, skip_alc, m_St.RSSI_Before_Filter_x10[path4]);		
			}
		}			
		
		
		if(skip_alc)
		{
			cnt[path4] = 0;
			return 0;
		}		
		//------------------------------
		cnt[path4]++;		
					
		if(cnt[path4] < 3) return 0;
		else cnt[path4] = 0;
		//---------------
				
		diff_x10 += PRU_Q_Hystresis_Att_x10;	
		if(diff_x10 == 0) diff_x10 = -1; // 0.1dB
		
		ctrl_prev_x10 = cur_comp_gain_delta_x10;			
		ctl_gain_x10  = cur_comp_gain_delta_x10 - diff_x10; // gain increase
		
		if(ctl_gain_x10  > (ctrl_prev_x10+10)) // over 1dB, then 1dB step
			ctl_gain_x10 = (ctrl_prev_x10+5);
				
		if(ctl_gain_x10  > max_comp_range_x10) // max 20.0 dB
			ctl_gain_x10 = max_comp_range_x10;

		m_St.UL_DTU_Gain_Comp_x10[_100M][path4] = ctl_gain_x10; // PRU
		m_St.UL_DTU_Gain_Comp_x10[Alpha][path4] = ctl_gain_x10; // PRU
		
		Set_PRU_RVS_Gain_Comp_x10(path4, ctl_gain_x10);
		
		if(ctrl_prev_x10 != ctl_gain_x10) 
		{
			att_set = 1; // atten set
			
			if(Is_Debug_Path4(path4))
			{
				print_agent( DEBUG_STDBY, VT_BOLD_CYAN, "[RVS-COMP] <AMP_%d> Under(+): diff(%d/10), comp_gain(%d/10) Set", 
							path4, diff_x10, ctl_gain_x10);
			}			
		}
	}
	//----------------------------------------------------- 
	return att_set; // changed or not
}
#endif



//-----------------------------------------------------------------------------------------




// uVal8 : 1 is ON  (Gain Check ON,  Noise Mode Off)
// uVal8 : 0 is OFF (Gain Check off, Noise Mode ON)
void AAU__Gain_Check_Mode_Ctrl(INT8U uVal8)
{
	INT8U c, l;	
	INT8U req_gain;
	INT8U req_noise;
	INT16U req_level[MAX_Optic][MAX_DL];
	INT16U req_val16;

	// for TEST AAU_gain_chk_level
#ifdef __TEST_AAU__Use_Gain_Check_Level__
	if((m_Env.debug_optic == 0xFF)||(m_Env.debug_optic == 0))
		m_Env.debug_optic = m_Env.AAU_gain_chk_level;
	else
		m_Env.AAU_gain_chk_level = m_Env.debug_optic;
#endif	
	//---------------------------------------------
	if(uVal8) // Gain Check mode
	{
		req_gain = 1;
		req_noise = 0;	
		
		for( c=0; c < MAX_Optic; c++)
		{
			for( l=0; l < MAX_DL; l++)
				req_level[c][l] = m_Env.AAU_gain_chk_level; // AAU default 8
		}
	}
	else // Noise Det Mode
	{
		req_gain = 0;
		req_noise = 1;	
		
		for( c=0; c < MAX_Optic; c++)
		{
			for( l=0; l < MAX_DL; l++)
				req_level[c][l] = 0; // AAU default 0
		}
	}
	//---------------------------------------------
	if(m_Reg_Sts.Gain_Check_Mode.onoff != req_gain)
	{
		m_Reg_Ctl.Gain_Check_Mode.onoff = req_gain;
		m_Reg_Flg.Gain_Check_Mode.Data = _SET_;
	}
	if(m_Reg_Sts.Gain_Check_Mode.noise_det_mode != req_noise)
	{
		m_Reg_Ctl.Gain_Check_Mode.noise_det_mode = req_noise;
		m_Reg_Flg.Gain_Check_Mode.Data = _SET_;
	}
	//---------------------------------------------
	for( c=0; c < MAX_Optic; c++)
	{
		for( l=0; l < MAX_DL; l++)
		{
			/*
			2019.09.25 : if User-CW-ON, Do-Not CW-Level-Control
			*/
			if(m_St.User_DL_CW_OnOff[c][l])
				req_val16 = m_Eep.d_DL_CW_LEVEL[c][l];
			else
				req_val16 = req_level[c][l];				
			
			if(m_Reg_Sts.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 != req_val16)
			{
				m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 = 1;
				m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 = req_val16;
			}
		}
	}
}

void AAU___All_Off_GainChkMode_NoiseMode(void)
{
	INT8U c, l;	
	INT16U req_val;

	if(m_Reg_Sts.Gain_Check_Mode.onoff || m_Reg_Sts.Gain_Check_Mode.noise_det_mode)
	{
		m_Reg_Ctl.Gain_Check_Mode.onoff = 0;
		m_Reg_Ctl.Gain_Check_Mode.noise_det_mode = 0;
		m_Reg_Flg.Gain_Check_Mode.Data = _SET_;
	}
	//---------------------------------------------
	for( c=0; c < MAX_Optic; c++)
	{
		for( l=0; l < MAX_DL; l++)
		{
			/*
			2019.09.25 : if User-CW-ON, Do-Not CW-Level-Control
			*/
			if(m_St.User_DL_CW_OnOff[c][l])
				req_val = m_Eep.d_DL_CW_LEVEL[c][l];
			else
				req_val = 0;
			
			if(m_Reg_Sts.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 != req_val)
			{
				m_Reg_Flg.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 = 1;
				m_Reg_Ctl.General_FA[c].General_FA_MIMO[l].RW_DL_CW_Level.uD32 = req_val;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------
void AAU__Gain_Check_Mode_dl_mode_Ctrl(INT8U uVal8)
{
	m_Reg_Flg.Gain_Check_Mode.onoff = _SET_;
	
	if(uVal8) // 1 is OFF, 2019.05.31
	{
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_1 = 0; // OFF
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_0 = 0; // OFF
	}
	else  // 0 is ON, 2019.05.31
	{						
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_1 = 1; // ON
		m_Reg_Ctl.Gain_Check_Mode.dl_mode_0 = 1; // ON
	}
}

//-----------------------------------------------------------------------------------------
// 2019.06.01
static OS_TIMER ul_check_timer;

void Proc___AAU_GainCheck_NoiseDet(void) 
{
	static INT8U init = 0;
	static INT8U cnt = 0;
	INT32U time_ms;
	static INT8U wave_chk_prev = 0;
	
	static INT16U count_1s = 0;
	static INT16U gain__chk_req = 0;
	static INT16U noise_chk_req = 0;
	
	INT8U path4;

	if(init == 0) // 2019.08.12, ON(1)
	{
		init = 1;
		OSTimerReset(&ul_check_timer);
		
		if(m_Eep.AAU_CW_Auto_Onoff > 1)
			m_Eep.AAU_CW_Auto_Onoff = 0; // default OFF(0)
	}	

	if(OSTimerGet_msec(&ul_check_timer) >= 1000) // 1[s]
	{
		OSTimerReset(&ul_check_timer);	
		count_1s++;
		
		if(count_1s >= 300) // every 5[m] = 300[s], 2020.01.29
		{			
			noise_chk_req = 80; // 2019.12.09 : 20(20 x every 100[ms]) : 4 times --> 80(for 8s)
			gain__chk_req = 80; // 2019.12.09 : 20(20 x every 100[ms]) : 4 times --> 80(for 8s)	
			count_1s = 0;			
		
			if(m_Eep.AAU_CW_Auto_Onoff == 0x01) // "UL 감시 onoff" ON(1)
			{
				print_agent( DEBUG_STDBY, VT_BOLD_GREEN, "[Every 5m Check] Gain & Noise Start.");
				
				for(path4 = 0; path4 < AMPU_4_MAX; path4++)
				{				
					// if not ul_gain_offset End, gain check time is 90[s]
					if(Check_PAU_Use(path4)==0) continue;
					if(IS__End_of_UL_Gain_Low_Offset(path4)) continue;				
					
					gain__chk_req = 900; // 900 is 90.0s
					print_agent( DEBUG_STDBY, VT_YELLOW, "Gain__chk_req count is %d by Path_%d ---", 
								gain__chk_req, path4);					
				}
				/*
				--------------------------------------------------------
				2019.12.11 : 
					if not Init_St___Completed, do not enter into gain-check-mode
				--------------------------------------------------------	
				*/
				if(m_St.RU_Init_Status != Init_St___Completed)
				{					
					gain__chk_req = 0;
					print_agent( DEBUG_STDBY, VT_BOLD_MAGENTA, " NOT Init_St___Completed------------------");
					print_agent( DEBUG_STDBY, VT_BOLD_MAGENTA, " NOT Init_St___Completed, gain__chk_req = 0");
					print_agent( DEBUG_STDBY, VT_BOLD_MAGENTA, " NOT Init_St___Completed------------------");
				}
			}
			else
			{
				print_agent( DEBUG_STDBY, VT_BOLD_GREEN, "[Every 5m Check] Noise Only Start.");
				gain__chk_req = 0;
			}
		}	
	}
	
	//--------------------------------------------------------	

	if(m_St.Wave_Check != wave_chk_prev)
	{
		D_Printf( DEBUG_STDBY, VT_YELLOW "[m_St.Wave_Check] Changed <%d -> %d> \n", wave_chk_prev, m_St.Wave_Check);
	}	
	
	if((m_St.Gain_Noise_Auto == 0)||(m_St.Wave_Check))
	{
		cnt = 0;
		
		if(m_St.Wave_Check)
		{
			time_ms = Get_Spectrum_Timer_ms();
			if(time_ms > 3000)
			{
				m_St.Wave_Check = 0; // 2019.06.01			
				print_agent( DEBUG_STDBY, VT_YELLOW, "[m_St.Wave_Check] Changed OFF");
			}
		}
		else
		{
			if(noise_chk_req > 0) // 2019.08.12
			{				
				AAU__Gain_Check_Mode_Ctrl(0); // UL Noise Mode
				noise_chk_req--;
				////print_agent( DEBUG_STDBY, VT_YELLOW, "[AAU] UL Noise Mode");
			}
			else if(gain__chk_req > 0)
			{
				AAU__Gain_Check_Mode_Ctrl(1); // Gain Check
				gain__chk_req--;
				////print_agent( DEBUG_STDBY, VT_YELLOW, "[AAU] Gain-Check Mode");
			}
			else
			{
				AAU___All_Off_GainChkMode_NoiseMode(); // all off				
			}			
		}		
	}
	else
	{		
		gain__chk_req = 0;
		noise_chk_req = 0;
		count_1s = 0;
		
		cnt++;

		if(cnt <= 10)
			AAU__Gain_Check_Mode_Ctrl(0); // UL Noise Mode
		else
			AAU__Gain_Check_Mode_Ctrl(1); // Gain Check

		if(cnt >= 20)
			cnt = 0;
	}	
	wave_chk_prev = m_St.Wave_Check;	
}
//-----------------------------------------------------------------------------------------
void Init__BIP_ErrCnt_Accum(void)
{
	m_St.d_BIP_ErrCnt[DL][CPRI_0] = 0;
	m_St.d_BIP_ErrCnt[UL][CPRI_2] = 0;
	m_St.d_BIP_ErrCnt[DL][CPRI_1] = 0;
	m_St.d_BIP_ErrCnt[UL][CPRI_3] = 0;
}
//-----------------------------------------------------------------------------------------
void Init__HardClipCnt_Accum(void)
{
	m_St.d_Hard_Clip_Cnt_Accum[DL_0] = 0; // 0xB40E
	m_St.d_Hard_Clip_Cnt_Accum[DL_1] = 0; 
	m_St.d_Hard_Clip_Cnt_Accum[DL_2] = 0; 
	m_St.d_Hard_Clip_Cnt_Accum[DL_3] = 0; 
}
//-----------------------------------------------------------------------------------------

static INT8U VSS_DL[255];
static INT8U VSS_UL[255];

void Proc__VSS_Monitor(void)
{
	static INT8U init = 0;
	INT8U c;

	if(init == 0)
	{
		m_St.VSS_Monitor_OnOff = 1; // default CRPI_0
		init = 1;
	}

	switch(m_St.VSS_Monitor_OnOff)
	{
		case 1 : c = CPRI_0; break;
		case 2 : c = CPRI_1; break;
		case 3 : c = CPRI_2; break;
		case 4 : c = CPRI_3; break;
		
		default: return;
	}

	memset(VSS_DL, 0, 255);
	memset(VSS_UL, 0, 255);
	
/* 
----------------------------------------
DL : 수신 
----------------------------------------
*/	
	VSS_DL[2]   = m_Reg_Sts.DL_CPRI[c].CPRI_Version.uD8[0];
	VSS_DL[66]  = m_Reg_Sts.DL_CPRI[c].CPRI_Startup.uD8[0];
	VSS_DL[130] = m_Reg_Sts.DL_CPRI[c].CPRI_L1_Reset_LOS.uD8[0];
	VSS_DL[194] = m_Reg_Sts.DL_CPRI[c].Pointer_P.uD8[0];
	//-------------------------
	VSS_DL[16]  = m_Reg_Sts.DL_CPRI[c].RECSERDESLOS.uD8[0];
	VSS_DL[80]  = m_Reg_Sts.DL_CPRI[c].RECLOFALARM.uD8[0];
	VSS_DL[144] = m_Reg_Sts.DL_CPRI[c].RECTypeID.uD8[0];
	VSS_DL[208] = m_Reg_Sts.DL_CPRI[c].CnM_Path_selection.uD8[0];
	//-------------------------
	VSS_DL[17]  = m_Reg_Sts.DL_CPRI[c].RE_Reset.uD8[0];
	VSS_DL[81]  = m_Reg_Sts.DL_CPRI[c].RE_Channel_BW.uD8[0];
	VSS_DL[145] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[3].uD8[0];
	VSS_DL[209] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[3].uD8[0];
	//-------------------------
	VSS_DL[18]  = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[2].uD8[0];
	VSS_DL[82]  = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[5].uD8[0];
	VSS_DL[146] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[2].uD8[0];
	VSS_DL[210] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[2].uD8[0];
	//-------------------------
	VSS_DL[19]  = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[1].uD8[0];
	VSS_DL[83]  = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[4].uD8[0];
	VSS_DL[147] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[1].uD8[0];	
	VSS_DL[211] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[1].uD8[0];
	//-------------------------
	VSS_DL[20]  = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[0].uD8[0];	
	VSS_DL[84]  = m_Reg_Sts.DL_CPRI[c].Base_Mac_Address[3].uD8[0];	
	VSS_DL[148] = m_Reg_Sts.DL_CPRI[c].Noti_IP2_Address[0].uD8[0];
	VSS_DL[212] = m_Reg_Sts.DL_CPRI[c].Noti_IP1_Address[0].uD8[0];
	//-------------------------
	VSS_DL[21]  = m_Reg_Sts.DL_CPRI[c].BIP_Err_Cnt.uD8[0]; // BIP Data for DL ???????????????
	VSS_DL[85]  = m_Reg_Sts.DL_CPRI[c].OpenCPRIVersion.uD8[0];
	VSS_DL[149] = m_Reg_Sts.DL_CPRI[c].CPRICompressionOnOff.uD8[0];
	VSS_DL[213] = m_Reg_Sts.DL_CPRI[c].RE_Cascade_ID.uD8[0];
	//-------------------------
	VSS_DL[22]  = m_Reg_Sts.DL_CPRI[c].OpticLOSDetectAlarm.uD8[0];
	VSS_DL[86]  = m_Reg_Sts.DL_CPRI[c].Loopbackmode.uD8[0];		
	VSS_DL[150] = m_Reg_Sts.DL_CPRI[c].Delay_H.uD8[0];
	VSS_DL[214] = m_Reg_Sts.DL_CPRI[c].Delay_L.uD8[0];
	//-------------------------	
	VSS_DL[23]  = m_Reg_Sts.DL_CPRI[c].REx_Reset[0].uD8[0];
	VSS_DL[87]  = m_Reg_Sts.DL_CPRI[c].REx_Reset[1].uD8[0];
	VSS_DL[151] = m_Reg_Sts.DL_CPRI[c].REx_Reset[2].uD8[0];
	VSS_DL[215] = m_Reg_Sts.DL_CPRI[c].REx_Reset[3].uD8[0];
	//-------------------------	
	VSS_DL[24]  =  m_Reg_Sts.DL_CPRI[c].RE1TxDelay_H.uD8[0];
	VSS_DL[88]  = m_Reg_Sts.DL_CPRI[c].RE1TxDelay_L.uD8[0];		
	VSS_DL[152] = m_Reg_Sts.DL_CPRI[c].RE1RxDelay_H.uD8[0];
	VSS_DL[216] = m_Reg_Sts.DL_CPRI[c].RE1RxDelay_L.uD8[0];
	//-------------------------	
	VSS_DL[25]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[0].uD8[0];
	VSS_DL[89]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[1].uD8[0];
	VSS_DL[153] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[2].uD8[0];
	VSS_DL[217] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[3].uD8[0];
	//-------------------------
	VSS_DL[26]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[4].uD8[0];
	VSS_DL[90]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[5].uD8[0];
	VSS_DL[154] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[6].uD8[0];
	VSS_DL[218] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[7].uD8[0];
	//-------------------------
	VSS_DL[27]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[8].uD8[0];
	VSS_DL[91]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[9].uD8[0];
	VSS_DL[155] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[10].uD8[0];
	VSS_DL[219] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[11].uD8[0];
	//-------------------------
	VSS_DL[28]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[12].uD8[0];
	VSS_DL[92]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[13].uD8[0];
	VSS_DL[156] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[14].uD8[0];
	VSS_DL[220] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[15].uD8[0];
	//-------------------------
	VSS_DL[29]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[16].uD8[0];
	VSS_DL[93]  = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[17].uD8[0];
	VSS_DL[157] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[18].uD8[0];
	VSS_DL[221] = m_Reg_Sts.DL_CPRI[c].TDD_Slot_Number[19].uD8[0];
	//-------------------------
	// VSS_DL[30] // reserved
	// VSS_DL[94] // reserved
	// VSS_DL[158] // reserved
	// VSS_DL[222] // reserved
	//-------------------------
	// VSS_DL[31] // reserved
	// VSS_DL[95] // reserved
	// VSS_DL[159] // reserved
	VSS_DL[223] = m_Reg_Sts.DL_CPRI[c].DUH_ID.uD8[0];
	//-------------------------	
	VSS_DL[32]  = m_Reg_Sts.DL_CPRI[c].PCI_1st_Byte.uD8[0];
	VSS_DL[96]  = m_Reg_Sts.DL_CPRI[c].PCI_2nd_Byte.uD8[0];
	VSS_DL[160] = m_Reg_Sts.DL_CPRI[c].REC_Port_No.uD8[0];
	// VSS_DL[224] // reserved
	//-------------------------		
	VSS_DL[33]  = m_Reg_Sts.DL_CPRI[c].Base_IP_Address[0].uD8[0];
	VSS_DL[97]  = m_Reg_Sts.DL_CPRI[c].Base_IP_Address[1].uD8[0];
	VSS_DL[161] = m_Reg_Sts.DL_CPRI[c].Base_IP_Address[2].uD8[0];
	VSS_DL[225] = m_Reg_Sts.DL_CPRI[c].Base_IP_Address[3].uD8[0];
	//-------------------------	
	// VSS_DL[34] ~ : reserved
	// VSS_DL[35] ~ : reserved
	//-------------------------	
	

	//-------------------------		
	
	//-------------------------		
	/*
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].UL_GainBackOff_Onoff.uD8[0];	
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].Cell_ID.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].gNB_ID_1st.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].gNB_ID_2nd.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].gNB_ID_3rd.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].gNB_ID_4th.uD8[0];
	//VSS_DL[] = (INT8U)m_Reg_Sts.DL_CPRI[c].L1Reset.Data;
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].BIP_Count.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].BIP_Count_1.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].BIP_Count_2.uD8[0];
	//VSS_DL[] = m_Reg_Sts.DL_CPRI[c].BIP_Count_3.uD8[0];
	//VSS_DL[] = (INT8U)m_Reg_Sts.DL_CPRI[c].BIP_Clear.Data;
	*/
/* 
----------------------------------------
UL : 송신 
----------------------------------------
*/
	VSS_UL[2]   = m_Reg_Sts.UL_CPRI[c].CPRI_Version.uD8[0];
	VSS_UL[66]  = m_Reg_Sts.UL_CPRI[c].CPRI_Startup.uD8[0];
	VSS_UL[130] = m_Reg_Sts.UL_CPRI[c].CPRI_L1_Reset_LOS.uD8[0];
	VSS_UL[194] = m_Reg_Sts.UL_CPRI[c].Pointer_P.uD8[0];
	//-------------------------	
	VSS_UL[16]  = m_Reg_Sts.UL_CPRI[c].RE_SERDESLOS.uD8[0];
	VSS_UL[80]  = m_Reg_Sts.UL_CPRI[c].RE_LOFALARM.uD8[0];
	VSS_UL[144] = m_Reg_Sts.UL_CPRI[c].REFunctionFail.uD8[0];
	// VSS_UL[208] = 0; // reserved 
	//-------------------------	
	VSS_UL[17] = m_Reg_Sts.UL_CPRI[c].RE_Reset_Ack.uD8[0];
	VSS_UL[81] = m_Reg_Sts.UL_CPRI[c].ALIVE_ACK_00.uD8[0];		
	// VSS_UL[145] = 0; // reserved 
	// VSS_UL[209] = 0; // reserved 
	//-------------------------	
	VSS_UL[21] = m_Reg_Sts.UL_CPRI[c].BIP_Err_Cnt.uD8[0];
	VSS_UL[85] = m_Reg_Sts.UL_CPRI[c].ALIVE_ACK_01.uD8[0];
	// VSS_UL[149] = 0; // reserved 
	VSS_UL[213] = m_Reg_Sts.UL_CPRI[c].RE_Cascade_ID_ACK.uD8[0];
	//-------------------------		
	VSS_UL[22] = m_Reg_Sts.UL_CPRI[c].RE_OpticLOSDetectAlarm.uD8[0];
	VSS_UL[86] = m_Reg_Sts.UL_CPRI[c].RE_Loopbackmode_ACK.uD8[0];	
	// VSS_UL[150] = 0; // reserved 
	// VSS_UL[214] = 0; // reserved 
	//-------------------------	
	VSS_UL[23]  = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[0].uD8[0];
	VSS_UL[87]  = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[1].uD8[0];
	VSS_UL[151] = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[2].uD8[0];
	VSS_UL[215] = m_Reg_Sts.UL_CPRI[c].RE_Reset_ACK[3].uD8[0];
	//-------------------------	
	VSS_UL[24]  = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[0].uD8[0];
	VSS_UL[88]  = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[1].uD8[0];
	VSS_UL[152] = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[2].uD8[0];
	VSS_UL[216] = m_Reg_Sts.UL_CPRI[c].RE_FunctionFail[3].uD8[0];
	//-------------------------	
	VSS_UL[30] = m_Reg_Sts.UL_CPRI[c].RE_Type_ID.uD8[0];
	//-------------------------	
	VSS_UL[31] = m_Reg_Sts.UL_CPRI[c].RE_Vendor_ID.uD8[0];
	//-------------------------		
	

	/*
	//VSS_UL[] = m_Reg_Sts.UL_CPRI[c].Mismatch_Noti.uD8[0];
	//VSS_UL[] = (INT8U)m_Reg_Sts.UL_CPRI[c].L1_Reset_ACK.Data;		
	//VSS_UL[] = m_Reg_Sts.UL_CPRI[c].BIP_Count.uD8[0];
	//VSS_UL[] = m_Reg_Sts.UL_CPRI[c].BIP_Count_1.uD8[0];
	//VSS_UL[] = m_Reg_Sts.UL_CPRI[c].BIP_Count_2.uD8[0];
	//VSS_UL[] = m_Reg_Sts.UL_CPRI[c].BIP_Count_3.uD8[0];
	//VSS_UL[] = m_Reg_Sts.UL_CPRI[c].BIP_Clear.Clear;
	*/	

	/*
	2019.12.04
	---------------------------------------+----------------------		
				TX Val					   |	RX Val 
	---------------------------------------+----------------------	
	CPRI 0		2 : CPRI Main 상향		   |  0 : CPRI Main 하향
	CPRI 0 +a	3 : CPRI Alpha 상향 	   |  1 : CPRI Alpha 하향
	---------------------------------------+----------------------	
	CPRI 1		4 : CPRI Main Cascade 하향 |  6 : CPRI Main Cascade 상향
	CPRI 1 +a	5 : CPRI Alpha Cascade 하향|  7 : CPRI Alpha Cascade 상향
	---------------------------------------+----------------------	
	
	Reg_VSS_DL_Addr[CPRI_0] // register 0: CPRI Main 하향
	Reg_VSS_DL_Addr[CPRI_1] // register 1: CPRI Alpha 하향
	Reg_VSS_DL_Addr[CPRI_2] // register 4: CPRI Main Cascade 하향
	Reg_VSS_DL_Addr[CPRI_3] // register 5: CPRI Alpha Cascade 하향	
	
	Reg_VSS_UL_Addr[CPRI_0] // register 2: CPRI #Main 상향
	Reg_VSS_UL_Addr[CPRI_1] // register 3: CPRI #Alpha 상향 
	Reg_VSS_UL_Addr[CPRI_2] // register 6: CPRI Main Cascade 상향	
	Reg_VSS_UL_Addr[CPRI_3] // register 7: CPRI Alpha Cascade 상향
	
	---------------------------------------+----------------------		
				TX Val					   |	RX Val 
	---------------------------------------+----------------------	
	CPRI 0		Reg_VSS_UL_Addr[CPRI_0]	   |  Reg_VSS_DL_Addr[CPRI_0]
	CPRI 0 +a	Reg_VSS_UL_Addr[CPRI_1]    |  Reg_VSS_DL_Addr[CPRI_1]
	---------------------------------------+----------------------	
	CPRI 1		Reg_VSS_DL_Addr[CPRI_2]    |  Reg_VSS_UL_Addr[CPRI_2]
	CPRI 1 +a	Reg_VSS_DL_Addr[CPRI_3]    |  Reg_VSS_UL_Addr[CPRI_3]
	---------------------------------------+----------------------	
	*/

	
	switch(m_St.VSS_Monitor_OnOff)
	{
		case 1 : // CPRI 0
		case 2 : // CPRI 0 +a
			memcpy(m_St.VSS_Tx, VSS_UL, 255);
			memcpy(m_St.VSS_Rx, VSS_DL, 255);
			break; 
			
		case 3 : // CPRI 1
		case 4 : // CPRI 1 +a
			memcpy(m_St.VSS_Tx, VSS_DL, 255);
			memcpy(m_St.VSS_Rx, VSS_UL, 255);
			break;
		
		default: return;
	}

	//-------------------------		
	
	//-------------------------		
}
//---------------------------------------------




//-----------------------------------------------------------------------------
/* EOF */

