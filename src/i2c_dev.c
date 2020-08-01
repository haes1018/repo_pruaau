/////////////////////////////
//	i2c_dev.c
/////////////////////////////
#define I2C_DEV_GLOBALS
#include "includes.h"
/////////////////////////////


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static int fDev_i2cEEP = -1;

#ifdef __TYPE__PRU__
#define	I2C_DEV		"/dev/i2c-1" //--------------> Must be Changed !!
#endif
///////////////////////////////////////////////////////////////////////////
#ifdef __TYPE__AAU__
#define	I2C_DEV		"/dev/i2c-1" //--------------> Must be Changed !!
#endif


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/* Note: 10-bit addresses are NOT supported! */

/* This is the structure as used in the I2C_SMBUS ioctl call */
struct i2c_smbus_ioctl_data 
{
	char read_write;
	__u8 command;
	int size;
	union i2c_smbus_data *data;
};

/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data 
{
	struct i2c_msg *msgs;	/* pointers to i2c_msgs */
	int nmsgs;		/* number of i2c_msgs */
};


static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file,I2C_SMBUS,&args);
}


static inline __s32 i2c_smbus_write_quick(int file, __u8 value)
{
	return i2c_smbus_access(file,value,0,I2C_SMBUS_QUICK,NULL);
}
	
static inline __s32 i2c_smbus_read_byte(int file)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte(int file, __u8 value)
{
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,value,
	                        I2C_SMBUS_BYTE,NULL);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, 
                                              __u8 value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                        I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_read_word_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_WORD_DATA,&data))
		return -1;
	else
		return 0x0FFFF & data.word;
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, 
                                              __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                        I2C_SMBUS_WORD_DATA, &data);
}

static inline __s32 i2c_smbus_process_call(int file, __u8 command, __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	if (i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                     I2C_SMBUS_PROC_CALL,&data))
		return -1;
	else
		return 0x0FFFF & data.word;
}


/* Returns the number of read bytes */
static inline __s32 i2c_smbus_read_block_data(int file, __u8 command, __u8 *values)
{
	union i2c_smbus_data data;
	int i;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BLOCK_DATA,&data))
		return -1;
	else {
		for (i = 1; i <= data.block[0]; i++)
			values[i-1] = data.block[i];
		return data.block[0];
	}
}

static inline __s32 i2c_smbus_write_block_data(int file, __u8 command, __u8 length, __u8 *values)
{
	union i2c_smbus_data data;
	int i;
	if (length > I2C_SMBUS_BLOCK_MAX)
		length = I2C_SMBUS_BLOCK_MAX;
	for (i = 1; i <= length; i++)
		data.block[i] = values[i-1];
	data.block[0] = length;
	return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_DATA, &data);
}

/* Returns the number of read bytes */
static inline __s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 *values)
{
	union i2c_smbus_data data;
	int i;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command, I2C_SMBUS_I2C_BLOCK_DATA,&data))
		return -1;
	else {
		for (i = 1; i <= data.block[0]; i++)
			values[i-1] = data.block[i];
		return data.block[0];
	}
}

static inline __s32 i2c_smbus_write_i2c_block_data(int file, __u8 command, __u8 length, __u8 *values)
{
	union i2c_smbus_data data;
	int i;
	if (length > I2C_SMBUS_BLOCK_MAX)
		length = I2C_SMBUS_BLOCK_MAX;
		
	for (i = 1; i <= length; i++)
		data.block[i] = values[i-1];
		
	data.block[0] = length;
	
	return i2c_smbus_access(file,I2C_SMBUS_WRITE, command, I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

/* Returns the number of read bytes */
static inline __s32 i2c_smbus_block_process_call(int file, __u8 command, __u8 length, __u8 *values)
{
	union i2c_smbus_data data;
	int i;
	if (length > 32)
		length = 32;
	for (i = 1; i <= length; i++)
		data.block[i] = values[i-1];
	data.block[0] = length;
	if (i2c_smbus_access(file,I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_PROC_CALL, &data))
		return -1;
	else {
		for (i = 1; i <= data.block[0]; i++)
			values[i-1] = data.block[i];
		return data.block[0];
	}
}



/**************************************************************************
    Functions for EEPROM 24L64
***************************************************************************/

#define EEP_I2C_ADDR_BNANK 0x53 // 24LC64 (A2:0, A1:1, A0:1 ) 1010011
#define EEP_PAGE_LENGTH	31 // EEPROM 은 32 까지 되지만 Drv가 31 이 최대

unsigned char read_eep_byte( unsigned int address )
{
	unsigned int value ;
	int res ;

	if( fDev_i2cEEP<0 ) return 0xFF;


	if( ioctl( fDev_i2cEEP, I2C_SLAVE_FORCE, EEP_I2C_ADDR_BNANK )<0)
	{
	  	syslog( LOG_ERR, "Error: Could not set address to %d: %s\n",address, strerror(errno));
		return 0xFF;
	}

	/* 16 bit Address, 8 Bit Data => to 8 Bit Address + 16Bit Data Format */
//	address &= 0x1FFF ;	// Address Masking
	value = address&0xff;
	address = ( address >> 8 ) & 0x1f ; // 1F -> Address Masking

	res = i2c_smbus_write_byte_data(fDev_i2cEEP, address, value );	  /* Write Address for Read */
	res = i2c_smbus_read_byte(fDev_i2cEEP);

	if ( res < 0 )
		return 0xff;

	return (unsigned char)( res & 0xff ) ;
}

int write_eep_byte( unsigned int address, unsigned char eep_data )
{
	unsigned int value;
	int res;

	if( fDev_i2cEEP<0 ) return -1;

	if( ioctl( fDev_i2cEEP, I2C_SLAVE_FORCE, EEP_I2C_ADDR_BNANK )<0)
	{
	  	syslog( LOG_ERR, "Error: Could not set address to %d: %s\n",address, strerror(errno));
		return -1;
	}

	/* 16 bit Address, 8 Bit Data => to 8 Bit Address + 16Bit Data Format */
//	address = address & 0x1FFF ;	// Address Masking
//	value = eep_data ;

//	value = value *256 + ( address & 0xff );
	value = (eep_data<<8) | ( address & 0xff );
	address = ( address >> 8 ) & 0x1f ; // Address Masking


	res = i2c_smbus_write_word_data(fDev_i2cEEP, address, value);

	usleep( 1000 * 5 ); //5ms

	return res ;
}


int write_eep_page( unsigned int address, unsigned char* eep_data, int len )
{
	int res, i ;
	unsigned char i2c_data[ EEP_PAGE_LENGTH + 1 ] ;

	if( fDev_i2cEEP<0 ) return -1;
	if( len>EEP_PAGE_LENGTH ) return -1;

//	address = address & 0x1FE0 ;	// Address Masking


	if( ioctl( fDev_i2cEEP, I2C_SLAVE_FORCE, EEP_I2C_ADDR_BNANK )<0)
	{
	  	syslog( LOG_ERR, "Error: Could not set address to %d: %s\n",address, strerror(errno));
		return -1;
	}

	/* 16 bit Address => to 8 Bit Address + 8Bit Data Format */

	// res = i2c_smbus_write_word_data(fDev_i2cEEP, daddress, value);
	// S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P

	i2c_data[0] = ( address & 0xff );
	address = ( address >> 8 ) & 0x1f ;

	for( i=0; i<len; i++ )
		i2c_data[i+1] = eep_data[i] ;

	res = i2c_smbus_write_i2c_block_data( fDev_i2cEEP, address, len+1, i2c_data);

	return res ;
}

int i2c_device_open(void)
{
	fDev_i2cEEP = open(I2C_DEV, O_RDWR);
	
	if(fDev_i2cEEP < 0)
	{
		syslog( LOG_ERR, "i2c dev_open : Could not open file `%s', : %s\n", I2C_DEV, strerror(errno));
		syslog( LOG_ERR, "i2c_open FAIL !!\n" );

		return -1 ;
	}
	return fDev_i2cEEP;
}

void i2c_device_close(void)
{
	if( fDev_i2cEEP>0 ) close(fDev_i2cEEP);
}


////////////////////////////////////////////////



/* EOF */

