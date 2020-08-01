/*	spi_test.c	*/

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>




///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#define	SPI_DEV_FPGA		"/dev/spidev1.0"
#define	SPI_DEV_BPW			8
#define	SPI_DEV_SPEED		12000000
#define	SPI_DEV_SPEED_PLL	12000000

///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////

#define	FPGA_IO_BUF_SIZE		(4 * 1024)					// 4KB
#define	FPGA_SINGLE_IO_SIZE		(FPGA_IO_BUF_SIZE - 1)

#define	MAX_WAITING_COUNT		(10 * 1000)			// 1usec * 1000 * 10 = 10sec

///////////////////////////////////////////////////////////////////////////

#define	PLL_READBACK_ADDR		0x0000001F
#define	PLL_RB_ADDR_SHIFT		16
#define	PLL_MASK_RB_LOCK		0x00000020			// (1 << 5) : bit[5]
#define	PLL_WIRE_UNLOCK			PLL_READBACK_ADDR

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////

static int
_open_spi_fpga()
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
			printf("[_open_spi_fpga] SPI MODE check failed ... \n");
			close(fd);
			return -1;
		}

		val = SPI_DEV_BPW;
		ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &val);

		re = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &val);
		if (re < 0)
		{
			printf("[_open_spi_fpga] SPI BITS_PER_WORD check failed ... \n");
			close(fd);
			return -1;
		}

		val = SPI_DEV_SPEED;
		ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &val);

		re = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &val);
		if (re < 0 || val != SPI_DEV_SPEED)
		{
			printf("[_open_spi_fpga] SPI SPEED_HZ check failed ... \n");
			close(fd);
			return -1;
		}
	}

	return fd;
}

int
_rdwr_spi_dev(int fd, unsigned char *p_txbuf, unsigned char *p_rxbuf, int size, int speed_hz, int delay_usecs)
{
	struct spi_ioc_transfer xfer;
	int re;

	memset(&xfer, 0, sizeof(struct spi_ioc_transfer));

	xfer.tx_buf = (unsigned long)p_txbuf;
	xfer.rx_buf = (unsigned long)p_rxbuf;
	xfer.len    = size;

	xfer.bits_per_word = SPI_DEV_BPW;
	xfer.speed_hz      = speed_hz;
	xfer.delay_usecs   = delay_usecs;

	re = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);

	return re;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



static int
_read_spi_fpga_reg( int fd_spi, int addr)
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


	_rdwr_spi_dev(fd_spi, tx_buf, rx_buf, 8, SPI_DEV_SPEED, 0);

	data  = ((unsigned int)rx_buf[4] << 24) & 0xff000000;
	data |= ((unsigned int)rx_buf[5] << 16) & 0x00ff0000;
	data |= ((unsigned int)rx_buf[6] << 8) & 0x0000ff00;
	data |= (unsigned int)rx_buf[7] & 0x000000ff;
	

	
	return data;
}

static int
_write_spi_fpga_reg( int fd_spi, int addr, int data)
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

	

	_rdwr_spi_dev(fd_spi, tx_buf, NULL, 8, SPI_DEV_SPEED, 0);

	rd_data = _read_spi_fpga_reg( fd_spi, addr);

	return rd_data;
}




static int
_main_read_spi_fpga_reg( int fd_spi, int addr)
{
	int data;
	
	data = _read_spi_fpga_reg( fd_spi, addr);

	printf("[read_spi_fpga_reg] addr=0x%x data=0x%x\n", addr, data);

	return data;
}

static int
_main_write_spi_fpga_reg(int fd_spi, int addr, int data)
{
	int rd_data;
	
	rd_data = _write_spi_fpga_reg( fd_spi, addr, data);

	printf("[write_spi_fpga_reg] addr=0x%x data=0x%x rd_data=0x%x\n", addr, data, rd_data);

	return 0;
}


static void
_close_files( int fd_spi)
{
	if (fd_spi >= 0)
		close(fd_spi);
}

void
_print_usages(char *name)
{
	printf("%s [-f fpga_image_file] [spi_fpga_addr [data]] [-p pll_data_addr]\n", name);	
}

int
main(int ac, char *av[])
{
	int cmd;
	int opt;
	
	unsigned int reg_addr;
	unsigned int reg_data;
	int re;

	
	int fd_spi  = -1;

	cmd = 0;	
	reg_addr = 0;
	reg_data = 0;


	printf("\n %s: spi device ------ \n", SPI_DEV_FPGA);
		
	if (cmd == 0)
	{
		/*	arrange arg position
		 */
		ac -= optind - 1;
		av += optind - 1;

		if (ac == 2)
		{
			reg_addr = strtoul(av[1], NULL, 16);
			cmd = 2;
		}
		else if (ac == 3)
		{
			reg_addr = strtoul(av[1], NULL, 16);
			reg_data = strtoul(av[2], NULL, 16);
			cmd = 3;
		}
	}

	re = 0;
	if (cmd <= 0)
	{
		_print_usages(av[0]);
		re = 1;
	}
	else
	{		
		fd_spi = _open_spi_fpga();
		if (fd_spi < 0)
		{
			_close_files(fd_spi);
			printf("%s: spi device open failed (%s)\n", av[0], strerror(errno));
			return 1;
		}

		/*	do request ios ...
		 */
		switch (cmd)
		{
			case 2:			// read fpga reg
				re = _main_read_spi_fpga_reg( fd_spi, (int)reg_addr);
				break;
			case 3:			// write fpga reg
				re = _main_write_spi_fpga_reg( fd_spi, (int)reg_addr, (int)reg_data);
				break;
			
			default:
				printf("%s: invalid command options ...\n", av[0]);
				_print_usages(av[0]);
				re = 1;
				break;
		}

		/*	close used devices
		 */
		_close_files(fd_spi);
	}

	return re;
}

/* EOF */
