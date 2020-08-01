/********************************************************************************/
#ifndef DEVICELIB_H
#define DEVICELIB_H
/********************************************************************************/
#ifdef  DEVICELIB_GLOBALS
#define DEVICELIB_EXT
#else
#define DEVICELIB_EXT extern
#endif
/**************************************************************/

///////////////////////////////////
// GPIO
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
#define	GPIO_DEVICE				"/dev/io_gpio_k"  // 2019.02.20 @ 5G

#define	REGIO_IOCTL_MAGIC		'g'				  // 0x67, 2019.02.20 @ 5G

#define	GPIO_INIT					((REGIO_IOCTL_MAGIC << 24) | 0x00020000) // 2019.02.20 @ 5G
#define	GPIO_EXIT					((REGIO_IOCTL_MAGIC << 24) | 0x00020001) // 2019.02.20 @ 5G
#define	GPIO_DIR_OUT				((REGIO_IOCTL_MAGIC << 24) | 0x00020003) // 2019.02.20 @ 5G
#define	GPIO_DIR_IN					((REGIO_IOCTL_MAGIC << 24) | 0x00020004) // 2019.02.20 @ 5G
#define	GPIO_SET					((REGIO_IOCTL_MAGIC << 24) | 0x00020005) // 2019.02.20 @ 5G
#define	GPIO_GET					((REGIO_IOCTL_MAGIC << 24) | 0x00020006) // 2019.02.20 @ 5G

#define	GPIO_VALUE_HIGH				0x100 // 2019.02.20 @ 5G
#define	GPIO_VALUE_LOW				0     // 2019.02.20 @ 5G

typedef struct user_reg_io
{   
	int bwl;						// 1=byte(1B) 2=word(2B) 4=long_word(4B)
	void *addr;
	unsigned int data;
} user_reg_io_t; // 2019.02.20 @ 5G

// PRU GPIO
#define	REGIO_IOC_REG_READ			((REGIO_IOCTL_MAGIC << 24) | 0x00010001) // 2019.02.20 @ 5G
#define	REGIO_IOC_REG_WRITE			((REGIO_IOCTL_MAGIC << 24) | 0x00010002) // 2019.02.20 @ 5G
//--------------------------------------------------------------------------




//--------------------------------------------------------------------------

#define GPIO_Init(pin) (_init_gpio(pin))
#define GPIO_Exit(pin) (_close_gpio(pin))
#define GPIO_DIR_Mode_Out(pin,defout) (_init_gpio_out(pin, defout))
#define GPIO_DIR_Mode_In(pin) (_init_gpio_in(pin))
#define GPIO_Set(pin,out) (_set_gpio(pin, out))
#define GPIO_Get(pin) (_get_gpio(pin))

/*
-----------------------------------------------------------------------------
	GPIO - ANTS
-----------------------------------------------------------------------------
*/
#ifdef __MAKER_ANTS__
//-----------------------------------------------
#ifdef __TYPE__PRU__
//-----------------------------------------------
#define GPIO_PAU_3_RST_O	100 // MIO2
#define GPIO_PAU_3_RSV_O  	106 // MIO8
#define GPIO_PAU_3_En_O		107 // MIO9
#define GPIO_PAU_3_INS_I	109 // MIO11

#define GPIO_PAU_2_RST_O	104 // MIO6
#define GPIO_PAU_2_RSV_O 	102 // MIO4
#define GPIO_PAU_2_En_O		103 // MIO5
#define GPIO_PAU_2_INS_I	101 // MIO3

#define GPIO_PAU_1_RST_O	159 // MIO61
#define GPIO_PAU_1_RSV_O	157 // MIO59
#define GPIO_PAU_1_En_O		151 // MIO53
#define GPIO_PAU_1_INS_I	156 // MIO58

#define GPIO_PAU_0_RST_O	150 // MIO52
#define GPIO_PAU_0_RSV_O	164 // MIO66
#define GPIO_PAU_0_En_O		165 // MIO67
#define GPIO_PAU_0_INS_I	166 // MIO68

#define GPIO_PSU_AC_FAIL_I	172 // MIO74

#define GPIO_OPT_SW_DC_EN		160 // MIO62
#define GPIO_OPT_SW_Status0		161 // MIO63
#define GPIO_OPT_SW_Status1		163 // MIO65
#define GPIO_CIM_DL_0_CPL_PS	108 // MIO10
#define GPIO_CIM_DL_1_CPL_PS	98  // MIO0
#define GPIO_CIM_DL_2_CPL_PS	123 // MIO25
#define GPIO_CIM_DL_3_CPL_PS	122 // MIO24

#define GPIO_DOOR_ALARM			173 // MIO75
#define GPIO_PS_WDOG_CLK		158 // MIO60
#define GPIO_PS_TO_PL_RESET		162 // MIO64
#define GPIO_RUN_LED			167 // MIO69
#define GPIO_BOARD_RESET		170 // MIO72
#define GPIO_CASE_INVEN_DATA	171 // MIO73

//#define GPIO_PSU_RESET_H		// EMIO?
//#define GPIO_PSU_RESET_L		// EMIO?
//#define GPIO_RESET_L2SW		// EMIO?
//------------------------------
#define RUN_LED__ON()	(GPIO_Set( GPIO_RUN_LED, GPIO_VALUE_LOW ))
#define RUN_LED_OFF()	(GPIO_Set( GPIO_RUN_LED, GPIO_VALUE_HIGH ))
#define WDT_Toggle_H()	(GPIO_Set( GPIO_PS_WDOG_CLK, GPIO_VALUE_HIGH ))
#define WDT_Toggle_L()	(GPIO_Set( GPIO_PS_WDOG_CLK, GPIO_VALUE_LOW ))

#define CIM_0_Ctrl_H()	(GPIO_Set( GPIO_CIM_DL_0_CPL_PS, GPIO_VALUE_HIGH ))
#define CIM_0_Ctrl_L()	(GPIO_Set( GPIO_CIM_DL_0_CPL_PS, GPIO_VALUE_LOW ))

#define CIM_1_Ctrl_H()	(GPIO_Set( GPIO_CIM_DL_1_CPL_PS, GPIO_VALUE_HIGH ))
#define CIM_1_Ctrl_L()	(GPIO_Set( GPIO_CIM_DL_1_CPL_PS, GPIO_VALUE_LOW ))

#define CIM_2_Ctrl_H()	(GPIO_Set( GPIO_CIM_DL_2_CPL_PS, GPIO_VALUE_HIGH ))
#define CIM_2_Ctrl_L()	(GPIO_Set( GPIO_CIM_DL_2_CPL_PS, GPIO_VALUE_LOW ))

#define CIM_3_Ctrl_H()	(GPIO_Set( GPIO_CIM_DL_3_CPL_PS, GPIO_VALUE_HIGH ))
#define CIM_3_Ctrl_L()	(GPIO_Set( GPIO_CIM_DL_3_CPL_PS, GPIO_VALUE_LOW ))

#define PSU_RST_Ctrl_H()	(GPIO_Set( GPIO_BOARD_RESET, GPIO_VALUE_HIGH ))
#define PSU_RST_Ctrl_L()	(GPIO_Set( GPIO_BOARD_RESET, GPIO_VALUE_LOW ))


//-----------------------------------------------
#else // ANTS-AAU
//-----------------------------------------------
// 'GPIO1_0' is 'GPIO_32'

#define GPIO_PHY__RST		     27 // 32*0 + num, GPIO0_27, phy reset(high ON, low Reset)
#define GPIO_BLUE_RST		     22 // 32*0 + num, GPIO0_22, bluetooth reset
#define GPIO_FPGA_RST		    113 // 32*3 + num, GPIO3_17, FPGA reset
#define GPIO_RUN_LED		    115 // 32*3 + num, GPIO3_19, RUN LED // 2019.04.04

#define GPIO_GPIO_2		         26 // 32*0 + num, GPIO0_26, GPIO2
#define GPIO_GPIO_4		         28 // 32*0 + num, GPIO0_28, GPIO4
#define GPIO_GPIO_1		         44 // 32*1 + num, GPIO1_12, GPIO1
#define GPIO_GPIO_3		         45 // 32*1 + num, GPIO1_13, GPIO3

#define GPIO_LED_R			     46 // 32*1 + num, GPIO1_14, LED_Red
#define GPIO_LED_B			     47 // 32*1 + num, GPIO1_15, LED_Blue

#define GPIO_BT_RESET			116 // BT Reset

#define GPIO_CONF_DAT_EN		53	// 32*1 + num, GPIO1_21, CONF_DAT_EN, 2019.06.28.AAU

//------------------------------

#define LED_RED___ON()	(GPIO_Set( GPIO_LED_R, GPIO_VALUE_HIGH ))
#define LED_RED__OFF()	(GPIO_Set( GPIO_LED_R, GPIO_VALUE_LOW ))

#define LED_BLUE__ON()	(GPIO_Set( GPIO_LED_B, GPIO_VALUE_HIGH ))
#define LED_BLUE_OFF()	(GPIO_Set( GPIO_LED_B, GPIO_VALUE_LOW ))

#define BT_SET()	(GPIO_Set( GPIO_BT_RESET, GPIO_VALUE_LOW ))
#define BT_RESET()	(GPIO_Set( GPIO_BT_RESET, GPIO_VALUE_HIGH ))

#define RUN_LED__ON()	(GPIO_Set( GPIO_RUN_LED, GPIO_VALUE_LOW ))
#define RUN_LED_OFF()	(GPIO_Set( GPIO_RUN_LED, GPIO_VALUE_HIGH ))

#define AAU_PHY_RST()	(GPIO_Set( GPIO_PHY__RST, GPIO_VALUE_LOW )) // (high ON, low Reset)
#define AAU_PHY__ON()	(GPIO_Set( GPIO_PHY__RST, GPIO_VALUE_HIGH)) // (high ON, low Reset)


//-----------------------------------------------
#endif
#else // HFR (PRU,AAU)
//-----------------------------------------------
//HFR
#define BT_SET()			NULL
#define BT_RESET()			NULL

#endif
/*
-----------------------------------------------------------------------------
	END of
	GPIO - ANTS
-----------------------------------------------------------------------------
*/





//---------------------------------------------------------------------------
// Used later...: 
//
// Do not delete
//
//-----------------------------------------
#define GPIO_SIM_LED	167
//-----------------------------------------
#define GPIO_SIM_LED_R	GPIO_SIM_LED
#define GPIO_SIM_LED_B	GPIO_SIM_LED
#define GPIO_SIM_LED_G	GPIO_SIM_LED
#define GPIO_SIM_SW		GPIO_SIM_LED
//---------------------------------------------------------------------------




DEVICELIB_EXT int Open_GPIO_Dev(void);
DEVICELIB_EXT void Close_GPIO_Dev(void);

// Watch Dog
DEVICELIB_EXT int watchdog_start( void );
DEVICELIB_EXT void watchdog_end( void );
DEVICELIB_EXT int watchdog_set_time( int time );
DEVICELIB_EXT int watchdog_get_time( void );
DEVICELIB_EXT int watchdog_live( void );
DEVICELIB_EXT void watchdog_chk_remained(void);
DEVICELIB_EXT INT32U watchdog_remained_ms(void);
DEVICELIB_EXT INT32U watchdog_elapsed(void);
DEVICELIB_EXT INT32U get_watchdog_live_time_min(void);
DEVICELIB_EXT INT32U get_watchdog_live_time_max(void);



// spi ADC
DEVICELIB_EXT int ADC_SPI_Init( void );
DEVICELIB_EXT int ADC_SPI_Comm( INT8U* tx_buf, INT8U* rx_buf );

// i2c eep
DEVICELIB_EXT unsigned char read_eep_byte( unsigned int address );
DEVICELIB_EXT int write_eep_byte( unsigned int address, unsigned char eep_data );
DEVICELIB_EXT int write_eep_page( unsigned int address, unsigned char* eep_data, int len );



// GPIO
DEVICELIB_EXT int _init_gpio(int pin);
DEVICELIB_EXT int _set_gpio(int pin, int value);
DEVICELIB_EXT int _get_gpio(int pin);
DEVICELIB_EXT int _init_gpio_out(int pin, int initial_value);
DEVICELIB_EXT int _init_gpio_in(int pin);
DEVICELIB_EXT void _close_gpio(int pin);



//////////////////////////////////
#endif /* DEVICELIB_H */

