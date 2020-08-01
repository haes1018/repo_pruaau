/////////////////////////////
//	spi.c
/////////////////////////////
#define SPI_GLOBALS
#include "includes.h"
/////////////////////////////

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
#ifdef __TYPE__PRU__
#define	SPI_DEV_FPGA		"/dev/spidev0.0" //--------------> ANTS zynq
#define	SPI_DEV_FPGA1		"/dev/spidev0.1" //--------------> ANTS zynq
#define	SPI_DEV_FPGA2		"/dev/spidev0.2" //--------------> ANTS zynq, SPI for ADC(?)
#endif
///////////////////////////////////////////////////////////////////////////
#ifdef __TYPE__AAU__
#define	SPI_DEV_FPGA		"/dev/spidev1.0"
#endif
///////////////////////////////////////////////////////////////////////////

#define	SPI_DEV_BPW			8
#define	SPI_DEV_SPEED		8000000 // 10M(?)

///////////////////////////////////////////////////////////////////////////

static int fd_spi  = -1;

///////////////////////////////////////////////////////////////////////////

/*
// from "/linux/spi/spi.h"
//
//#define SPI_CPHA		0x01
//#define SPI_CPOL		0x02
//
//#define SPI_MODE_0		(0|0) // clock start low and rising sample
//#define SPI_MODE_1		(0|SPI_CPHA)
//#define SPI_MODE_2		(SPI_CPOL|0)
//#define SPI_MODE_3		(SPI_CPOL|SPI_CPHA)
*/


/*
---------------------------------------------------
	spi fpga open
---------------------------------------------------		
*/

#ifdef __TYPE__AAU__

static int _open_spi_fpga()
{
	int fd;
	int val;
	int re;

	fd = open(SPI_DEV_FPGA, O_RDWR);
	
	if (fd >= 0)
	{
		val = SPI_MODE_0;
		ioctl(fd, SPI_IOC_WR_MODE, &val);

		re = ioctl(fd, SPI_IOC_RD_MODE, &val);
		if (re < 0)
		{
			printf("[RCU: _open_spi_fpga] SPI MODE check failed ... \n");
			close(fd);
			return -1;
		}

		val = SPI_DEV_BPW;
		ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &val);

		re = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &val);
		if (re < 0)
		{
			printf("[RCU: _open_spi_fpga] SPI BITS_PER_WORD check failed ... \n");
			close(fd);
			return -1;
		}

		val = SPI_DEV_SPEED;
		ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &val);

		re = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &val);
		if (re < 0 || val != SPI_DEV_SPEED)
		{
			printf("[RCU: _open_spi_fpga] SPI SPEED_HZ check failed ... \n");
			close(fd);
			return -1;
		}
	}
	return fd;
}
#endif


int _rdwr_spi_dev(int fd, unsigned char *p_txbuf, unsigned char *p_rxbuf, int size, int speed_hz, int delay_usecs)
{
	struct spi_ioc_transfer xfer;
	int re;

	memset(&xfer, 0, sizeof(struct spi_ioc_transfer));

	xfer.tx_buf = (unsigned long)p_txbuf; // AAU is 4byte, PRU is 8byte
	xfer.rx_buf = (unsigned long)p_rxbuf;
	xfer.len    = size;

	xfer.bits_per_word = SPI_DEV_BPW;
	xfer.speed_hz      = speed_hz;
	xfer.delay_usecs   = delay_usecs;

	re = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);

	return re;
}

///////////////////////////////////////////////////////////////////////////

static int _read_spi_fpga_reg(int fd_spi, int addr)
#if 1
{
	unsigned char tx_buf[8];
	unsigned char rx_buf[8];
	int data;

	/*	tx_buf[0] : addr[31..24]
	 *	tx_buf[1] : addr[23..15]
	 *	tx_buf[2] : addr[14..8]
	 *	tx_buf[3] : addr[7..0]
	 *
	 *	rx_buf[0] : don't care
	 *	rx_buf[1] : don't care
	 *	rx_buf[2] : data[15..8] 
	 *	rx_buf[3] : data[7..0] 
	 */

	memset(tx_buf, 0, 8);
	memset(rx_buf, 0, 8);

	tx_buf[0] = 0x80 | ((addr >> 24) & 0x7f);		// 0x80 : read
	tx_buf[1] = ((addr >> 16) & 0xff);		
	tx_buf[2] = ((addr >> 8) & 0xff);
	tx_buf[3] = addr & 0xff;

	//_select_spi_fpga_reg_mode(fd_gpio, fd_spi);

	_rdwr_spi_dev(fd_spi, tx_buf, rx_buf, 8, SPI_DEV_SPEED, 0);

	data  = ((unsigned int)rx_buf[4] << 24) & 0xff000000;
	data |= ((unsigned int)rx_buf[5] << 16) & 0x00ff0000;
	data |= ((unsigned int)rx_buf[6] << 8) & 0x0000ff00;
	data |= (unsigned int)rx_buf[7] & 0x000000ff;
	
//	for(x = 0; x<8; x++)
//		printf("rxdata[%d] = %x \n", x ,rx_buf[x]);
	
	return data;
}
#else
{
	unsigned char tx_buf[8]; // @ ANTS PRU/AAU
	unsigned char rx_buf[8]; // @ ANTS PRU/AAU
	int data;

	/*	
	 *  tx_buf[0] : addr[31..24]
	 *	tx_buf[1] : addr[23..16]
	 *	tx_buf[2] : addr[15.. 8]
	 *	tx_buf[3] : addr[7 .. 0]
	 *
	 *  tx_buf[4] : data[31..24], dummy
	 *	tx_buf[5] : data[23..16], dummy
	 *	tx_buf[6] : data[15.. 8], dummy
	 *	tx_buf[7] : data[7 .. 0], dummy
	 */

	memset(tx_buf, 0, 8);
	memset(rx_buf, 0, 8);

	/*
	// legacy Code ----------------------------
	//tx_buf[0] = 0x80 | ((addr >> 8) & 0x7f); // 0x80 : read
	//tx_buf[1] = addr & 0xff;
	//_rdwr_spi_dev(...); 
	//data  = (rx_buf[2] << 8) & 0xff00;
	//data |= rx_buf[3] & 0x00ff;
	// legacy Code ----------------------------
	*/
	
	tx_buf[0] = ((addr >> 24) & 0x0000007F) | 0x80; // 0x80 : read
	tx_buf[1] = (addr >> 16) & 0x000000FF;
	tx_buf[2] = (addr >> 8) & 0x000000FF;
	tx_buf[3] = addr & 0x000000FF;

	_rdwr_spi_dev(fd_spi, tx_buf, rx_buf, 8, SPI_DEV_SPEED, 0);

	data  = (rx_buf[4] << 24) & 0xFF000000;
	data |= (rx_buf[5] << 16) & 0x00FF0000;
	data |= (rx_buf[6] << 8)  & 0x0000FF00;
	data |= rx_buf[7] & 0x000000FF;

	return data;
}
#endif 

static int _write_spi_fpga_reg(int fd_spi, int addr, int data)
#if 1
{
	unsigned char tx_buf[8];
	int rd_data;

	/*	tx_buf[0] : addr[14..8]
	 *	tx_buf[1] : addr[7..0]
	 *	tx_buf[2] : data[15..8] 
	 *	tx_buf[3] : data[7..0] 
	 */

	memset(tx_buf, 0, 8);

	tx_buf[0] = 0x00 | ((addr >> 24) & 0x7f);		// 0x00 : write
	tx_buf[1] = ((addr >> 16) & 0xff);		
	tx_buf[2] = ((addr >> 8) & 0xff);
	tx_buf[3] = addr & 0xff;

////////////// ycso TEST////////////
	tx_buf[4] = (data >> 24) & 0xff;
	tx_buf[5] = (data >> 16) & 0xff;
	tx_buf[6] = (data >> 8) & 0xff;
	tx_buf[7] = data & 0xff;

	//_select_spi_fpga_reg_mode(fd_gpio, fd_spi);

	_rdwr_spi_dev(fd_spi, tx_buf, NULL, 8, SPI_DEV_SPEED, 0);

	rd_data = _read_spi_fpga_reg(fd_spi, addr);

	return rd_data;
}
#else
{
	unsigned char tx_buf[8]; // @ ANTS PRU/AAU
	int rd_data;

	 /*	
	 *  tx_buf[0] : addr[31..24]
	 *	tx_buf[1] : addr[23..16]
	 *	tx_buf[2] : addr[15.. 8]
	 *	tx_buf[3] : addr[7 .. 0]
	 *
	 *  tx_buf[4] : data[31..24]
	 *	tx_buf[5] : data[23..16]
	 *	tx_buf[6] : data[15.. 8]
	 *	tx_buf[7] : data[7 .. 0]
	 */

	memset(tx_buf, 0, 8);

	/*
	// legacy Code ----------------------------
	//tx_buf[0] = 0x00 | ((addr >> 8) & 0x7f); // 0x00 : write
	//tx_buf[1] = addr & 0xff;
	//
	//tx_buf[2] = (data >> 8) & 0xff;
	//tx_buf[3] = data & 0xff;
	// legacy Code ----------------------------
	*/

	tx_buf[0] = ((addr >> 24) & 0x0000007F); // MSB '0' is for Write
	tx_buf[1] = (addr >> 16) & 0x000000FF;
	tx_buf[2] = (addr >> 8) & 0x000000FF;
	tx_buf[3] = addr & 0x000000FF;

	tx_buf[4] = (data >> 24) & 0x000000FF;
	tx_buf[5] = (data >> 16) & 0x000000FF;
	tx_buf[6] = (data >> 8) & 0x000000FF;
	tx_buf[7] = data & 0x000000FF;	

	_rdwr_spi_dev(fd_spi, tx_buf, NULL, 8, SPI_DEV_SPEED, 0);

	rd_data = _read_spi_fpga_reg(fd_spi, addr);

	return rd_data;
}
#endif

/*
---------------------------------------------------
	SPI Comm with FPGA 
		: 	Open fd and Close fd
---------------------------------------------------		
*/
int Open_SPI_FPGA(void)
{
#ifdef __TYPE__AAU__
	fd_spi = _open_spi_fpga();	
	
	if (fd_spi < 0) // fail
	{		
		printf("spi device open failed (%s)\n", strerror(errno));
		return 1;
	}
#endif	
	return 0; // success
}

void Close_SPI_FPGA(void)
{		
	if (fd_spi >= 0) 
		close(fd_spi);
}

/*
---------------------------------------------------------------------------------------------------
	AAU
	FPGA SPI Register 
	Direct Read / Write
---------------------------------------------------------------------------------------------------	
*/
int Rd_SPI_FPGA(int addr)
{
	int data;	

	if(fd_spi < 0) return 0xFFFFFFFF;
	
	
	Lock__FPGA_RdWr_Sem();

	data = _read_spi_fpga_reg(fd_spi, addr);
	
	UnLock__FPGA_RdWr_Sem();
	//D_Printf(DEBUG_SYSTEM, "[RCU] rd_spi_fpga_reg : addr=0x%x data=0x%x\n", addr, data);

	return data;
}

int Wr_SPI_FPGA(int addr, int data, char* str)
{
	int rd_data;

	if(fd_spi < 0) return 0xFFFFFFFF;

	Lock__FPGA_RdWr_Sem();

	rd_data = _write_spi_fpga_reg(fd_spi, addr, data);	


	if(addr == Reg_Common_Addr[Reg_Common_RW_Reset])
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[RCU] wr_spi_fpga_reg(Reg_RW_Reset 0x%X) : addr=0x%x data=0x%x str=%s", 
				Reg_Common_RW_Reset, addr, data, str);
	}


	if(addr == Reg_TDD_Addr[Reg_TDD_RW_TDD_Mode])
	{
		print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "Reg_TDD_RW_TDD_Mode control 0x%X", data);
	}

	


#ifdef test_20190418
	switch(addr)
	{
		case 0x4400a00:
		case 0x4600a00:
		case 0x6000: // RE_Reset 
		case 0x2020200: // HC_CLR
		case 0x2040200: // HC_CLR
		case 0x2060200: // HC_CLR
		case 0x2080200: // HC_CLR		
		case 0x400ca00: // VSS_DL
		case 0x420ca00: // VSS_DL
		case 0x4c0ca00: // VSS_DL
		case 0x4e0ca00: // VSS_DL
		case 0x480c000: // VSS_DL
		case 0x4a0c000: // VSS_DL
		case 0x4403c00: // VSS_UL
		case 0x4404000: // VSS_UL
		case 0x4404400: // VSS_UL
		case 0x4604000: // VSS_UL
		case 0x4604400: // VSS_UL		
			break;

		case 0x2220600:
		case 0x2240600:
		case 0x2260600:
		case 0x2280600:		
			D_Printf(DEBUG_ETC, VT_BOLD_RED "[RCU] wr_spi_fpga_reg : DL CW Ctrl addr 0x%X \n", addr);
			D_Printf(DEBUG_ETC, VT_BOLD_RED "[RCU] wr_spi_fpga_reg : addr=0x%x data=0x%x rd_data=0x%x\n", addr, data, rd_data);
			
			printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : DL CW Ctrl addr 0x%X \n" VT_END_CR, addr);
			printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : addr=0x%x data=0x%x rd_data=0x%x\n" VT_END_CR, addr, data, rd_data);
			break;	
			
		
		case 0x02200000:
		case 0x02400000:
			D_Printf(DEBUG_ETC, VT_BOLD_RED "[RCU] wr_spi_fpga_reg : NCO, 0x%X \n", addr);
			D_Printf(DEBUG_ETC, VT_BOLD_RED "[RCU] wr_spi_fpga_reg : addr=0x%x data=0x%x rd_data=0x%x\n", addr, data, rd_data);

			printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : NCO, 0x%X \n" VT_END_CR, addr);
			printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : addr=0x%x data=0x%x rd_data=0x%x\n" VT_END_CR, addr, data, rd_data);
			break;
			
		default:	
			D_Printf(DEBUG_ETC, "[RCU] SPI WR : addr=0x%x data=0x%x rd_data=0x%x, %s\n", addr, data, rd_data, str);
			printf(VT_CYAN "[RCU] SPI WR : addr=0x%x data=0x%x rd_data=0x%x, %s\n" VT_END_CR, addr, data, rd_data, str);
			break;
	}

	if(addr == 0xA020600)
	{		
		printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : Addr 0xA020600 \n" VT_END_CR);
		printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : Addr 0xA020600 \n" VT_END_CR);
		printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : Addr 0xA020600 \n" VT_END_CR);
		printf(VT_BOLD_RED "[RCU] wr_spi_fpga_reg : Addr 0xA020600 \n" VT_END_CR);
	}

	INT32U addr32;
	INT8U fa, m;
	for(fa=0;fa<MAX_FA_PATH;fa++)
	{
		for(m=0;m<MAX_MIMO_PATH;m++)
		{
			addr32 = Reg_DSP_FA_MIMO_Addr[fa][m][Reg_DSP_FA_MIMO_RW_DL_CW_On];
			if(addr32 == addr)
			{
				D_Printf(DEBUG_SYSTEM, VT_BOLD_RED "[CW ON/OFF Ctrl] fa %d, m %d, onoff %d, user(%d)\n", fa, m, data, aid_user_ctrl);
				printf(VT_BOLD_RED "[CW ON/OFF Ctrl] fa %d, m %d, onoff %d, user(%d)\n" VT_END_CR, fa, m, data, aid_user_ctrl);
			
}
		}
	}
	if(aid_user_ctrl > 0) aid_user_ctrl = 0;
#endif

	UnLock__FPGA_RdWr_Sem();

	return rd_data;
}
/*
---------------------------------------------------------------------------------------------------
	End of 
	AAU FPGA SPI Register 
	Direct Read / Write
---------------------------------------------------------------------------------------------------	
*/






/* EOF */

