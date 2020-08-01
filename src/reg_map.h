
/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
typedef unsigned char  BOOLEAN;
typedef unsigned char  BOOL;
typedef unsigned char  INT8U;                    
typedef signed   char  INT8S;                    
typedef unsigned short INT16U;                   
typedef signed   short INT16S;                   
typedef unsigned int   INT32U;                   
typedef signed   int   INT32S;                   
typedef unsigned long long int   INT64U;         
typedef signed   long long int   INT64S;         
typedef float          FP32;                     
typedef double         FP64;                     

typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
*/

/********************************************************************************/
#ifndef __REG_MAP_H
#define __REG_MAP_H
/********************************************************************************/

//RW - bit 31
#define __REG_WRITE__					0
#define __REG_READ__					1

//Burst - bit 30
#define	__REG_BURST__					0

//Mode - bit 29
#define __REG_MODE_GENERAL__			0
#define __REG_MODE_AXI__				1

//Func - bit 28:25
#define __REG_FUNC_COMMON__				0
#define __REG_FUNC_DSP__				1
#define __REG_FUNC_VSS__				2
#define __REG_FUNC_TDD__				3

#define __REG_FUNC_CPRI__				1
#define __REG_FUNC_JESD204B__			2


//FA_Port - bit 24:21
#define __REG_FA_PORT_COMMON__			0

#define __REG_FA_PORT_CPRI0__			1
#define __REG_FA_PORT_CPRI1__			2
#define __REG_FA_PORT_CPRI2__			3
#define __REG_FA_PORT_CPRI3__			4

#define __REG_FA_PORT_FA0__				1
#define __REG_FA_PORT_FA1__				2


#define __REG_FA_PORT_CPRI0_DL__		0
#define __REG_FA_PORT_CPRI1_DL__		1
#define __REG_FA_PORT_CPRI0_UL__		2
#define __REG_FA_PORT_CPRI1_UL__		3
#define __REG_FA_PORT_CPRI2_DL__		0
#define __REG_FA_PORT_CPRI3_DL__		1
#define __REG_FA_PORT_CPRI2_UL__		2
#define __REG_FA_PORT_CPRI3_UL__		3

//MIMO - bit 20:17
#define __REG_MIMO_COMMON__				0
#define __REG_MIMO_MIMO0__				1
#define __REG_MIMO_MIMO1__				2
#define __REG_MIMO_MIMO2__				3
#define __REG_MIMO_MIMO3__				4

/*
------------------------------------------------------------------------------------
	Start 
	of
	AAU Address bit[16:9]
------------------------------------------------------------------------------------	
*/


/*
------------------------------------------------------------------------------------
	End 
	of 
	AAU Address bit[16:9]
------------------------------------------------------------------------------------	
*/


enum
{
	CPRI_0 = 0,
	CPRI_1,
	CPRI_2,
	CPRI_3,
	
	CPRI_MAX_PATH
};

enum
{
	FA_0 = 0,
	FA_1,
	MAX_FA_PATH
};


typedef struct __DtuStatus
{
	type32 R_Version;							// 0x00, Logic Version 예) 1.0.0.5 -> 0x01000005
	type32 R_Date;								// 0x01, Logic Version 예) 1.0.0.5 -> 0x01000005
	type32 RW_Test;								// 0x02, Default value = 0x0000 0000
	type32 R_MB_Counter;						// 0x03, [31:0] Counter, M/B increase Counter per 1Sec
	type32 R_FPGA_Counter;						// 0x04, [31:0] Counter, M/B increase Counter per 1Sec

	//----------------------------------------	
	// [31:20] : Reserved 
	// [19:16] : Optic Injection, None = '0', Injection = '1'
    // [19:16] : CPRI #4 ~ #1 Optic
 	// [15:3] : Reserved
 	// [2] : JESD Lock, Lock ='1', Unlock ='0'
 	// [1] : PLL/AFE Init Done = '1', NotDone = '0'
 	// [0] : PLL Lock, Lock = '1', Fail = '0'
 	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Pll_Lock			:1; // 0	
			INT8U Pll_Inite_Done	:1; 	
			INT8U JESD_Lock			:1; 	
			INT8U 				    :1; 	
			INT16U				    :12; // 4~15			
			INT8U Optic1_Injection	:1;  // 16 
			INT8U Optic2_Injection	:1;  // 17 
			INT8U Optic3_Injection	:1;  // 18 
			INT8U Optic4_Injection	:1;  // 19 
			INT16U				    :12; // 20:31 
		}__attribute__((packed));
	}R_Board_Status; // Addr : 0x07

	//----------------------------------------	
	// [31:16] Reserved
	// [15:0] Termperature  예) -517 => -51.7 ℃
	type32 R_FPGA_Temp; // addr : 0x08
	
	//----------------------------------------	
	// Defaul value = 0xFFFF FFFF
	// bit : Normal = '1', Reset = '0'
	//----------------------------
	// [31] : DSP Reset 
	// [30] : M/B & PLL & AFE Reset
	// [29:24] : --
	// [23:20] : CPRI Reset of #4 ~ #1
	// [19:16] : --
	// [15:12] : AXI I/F Reset of #4 ~ #1
	// [11:2]  : --
	// [1]  : Eth Block Reset
	// [0]  : Eth Switch Reset
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Eth_Switch			:1; // 0
			INT8U Eth_Block				:1; // 1
			INT8U 						:2; // 2~3
			INT8U						:4; // 4
			INT8U						:4; // 8
			INT8U AXI_IF1				:1; // 9
			INT8U AXI_IF2				:1; // 10
			INT8U AXI_IF3				:1; // 11
			INT8U AXI_IF4				:1; // 12
			INT8U 						:4; // 16~19(#1~#4)
			INT8U CPRI1					:1; // 20(#1) 
			INT8U CPRI2					:1; // 21(#2)  
			INT8U CPRI3					:1; // 22(#3)  
			INT8U CPRI4					:1; // 23(#4)  
			INT8U						:4; // 24
			INT8U						:1; // 28
			INT8U						:1; // 29
			INT8U MB					:1; // 30
			INT8U DSP					:1; // 31
		}__attribute__((packed));
	}RW_Reset; // addr : 0x09, Reset 

	//----------------------------------------
	// Defaul value = 0x0000 0000
	//----------------------------------------	
	// [16] CPRI #1 = '0', CPRI #2 ='1'	
	// [0] CPRI #3 = '0', CPRI #4 ='1'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U CPRI3_4				:1; // 0	
			INT8U 						:7; 				
			INT8U 						:8; 	
			INT8U CPRI1_2				:1; // 16
			INT8U						:7; 
			INT8U 						:8; 
		}__attribute__((packed));
	}RW_ETH_Path_Sel; // addr : 0x0A,
	
	//----------------------------------------	
	// Defaul value = 0x0000 0000
	//----------------------------------------	
	// [31:1] : --
	// [0] : CPRI Mapper Loopback (On = '1', Off = '0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U OnOff				:1; // 0
		}__attribute__((packed));
	}RW_Mapper_Loopback; // addr : 0x0B

	//----------------------------------------	
	// Default value = 0x0000 0000
	// LoopbackOn = '1', LoopbackOff ='0'
	//----------------------------------------	
	// [1] : JESD Loopback
	// [0] : DSP Loopback
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U DSP_OnOff				:1; // 0	
			INT8U JESD_OnOff 			:1; // 1			
		}__attribute__((packed));
	}RW_RF_Loopback; // addr : 0x0C
		
	//----------------------------------------	
	// Default value = 0x0000 0000
	// ON ='1', OFF='0'
	//----------------------------------------
	// [31:2] : --
	// [1] : BLUE,  
	// [0] : RED, ON='1', OFF='0'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U RED_OnOff				:1; // 0	
			INT8U BLUE_OnOff 			:1; // 1			
		}__attribute__((packed));
	}RW_LED_Control; // addr : 0x0D

	//----------------------------------------	
	// Default value = 0x0000 0000	
	//----------------------------------------
	// [31:1] : --
	// [0] : PLL Lock (Mask='1', Unmask='0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Mask					:1; // 0
		}__attribute__((packed));
	}RW_Interrupt_mask0; // 0x20 : Interrupt Mask0 - PLL
	
	//----------------------------------------	
	// Default value = 0x0000 0000
	//----------------------------------------	
	// [31:30] : CPRI #1~#2 L1 Reset, Normal = '0', Reset = '1'
	// [30:20] : --
	// [19:16] : Optic Injection (None = '0', Injection = '1')
	//         [19:16] : CPRI #4 ~ #1 Optic
	// [15:12] : --
	// [11: 8] : Rx LOS, (Mask = '1', Unmask = '0')
	// [11: 8] : CPRI #4 ~ #1
	// [ 7: 4] : --
	// [ 3: 0] : Tx Fault (Mask = '1', Unmask = '0')
	// [ 3: 0] : CPRI #4 ~ #1, 
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Tx1_Fault				:1; // 0 
			INT8U Tx2_Fault				:1; // 1 
			INT8U Tx3_Fault				:1; // 2
			INT8U Tx4_Fault				:1; // 3
			INT8U 						:4; // 4~7
			INT8U Rx1_LOS				:1; // 8 
			INT8U Rx2_LOS				:1; // 9 
			INT8U Rx3_LOS				:1; // 10 
			INT8U Rx4_LOS				:1; // 11
			INT8U						:4; // 12~15
			INT8U Optic1_Injection		:1; // 16 
			INT8U Optic2_Injection		:1; // 17
			INT8U Optic3_Injection		:1; // 18 
			INT8U Optic4_Injection		:1; // 19			
			INT8U						:4; // 20~23
			INT8U						:4; // 24~27
			INT8U						:1; // 28 
			INT8U						:1; // 29 
			INT8U CPRI2_L1_Reset		:1; // 30 
			INT8U CPRI1_L1_Reset		:1; // 31			
		}__attribute__((packed));
	}RW_Interrupt_mask1; // 0x21 : Interrupt Mask0 - Optic module & L1 Reset

	//----------------------------------------	
	
	//----------------------------------------	
 	// Default value = 0xFFFF FFFF
 	//----------------------------------------	
 	// [31:1] : --
 	// [0] : PLL Lock, Lock = '1', Fail = '0'
	//----------------------------------------	
	// Write '1' to clear each bit
	//----------------------------------------	
	union{
		INT32U Data;
		struct{
			INT8U Sts				:1; // 0				
		}__attribute__((packed));
	}RW_Interrupt_Status0; // 0x22 : Interrupt Status0 - PLL
	
	//----------------------------------------	
 	// Defaul value = 0x3FFF FFFF
 	//----------------------------------------	
 	// [31:30] : CPRI #1~#2 L1 Reset, Normal = '0', Reset = '1'
 	// [30:20] : --
 	// [19:16] : Optic Injection, (None ='0', Injection ='1')
   	// [19:16] : CPRI #4 ~ #1 
 	// [15:12] : --
 	// [11: 8] : Rx Optic LOS (Mask = '1', Unmask = '0')
   	// [11: 8] : CPRI #4 ~ #1 
 	// [ 7: 4] : --
 	// [ 3: 0] : Tx Fault (Mask = '1', Unmask = '0')
    // [ 3: 0] : CPRI #4 ~ #1 
	//----------------------------------------	
	// Write '1' to clear each bit
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Tx1_Fault				:1; // 0 
			INT8U Tx2_Fault				:1; // 1 
			INT8U Tx3_Fault				:1; // 2 
			INT8U Tx4_Fault				:1; // 3 
			INT8U 						:4; // 4~7 
			INT8U Rx1_LOS				:1; // 8 
			INT8U Rx2_LOS				:1; // 9 
			INT8U Rx3_LOS				:1; // 10 
			INT8U Rx4_LOS				:1; // 11
			INT8U						:4; // 12~15 
			INT8U Optic1_Injection		:1; // 16 
			INT8U Optic2_Injection		:1; // 17
			INT8U Optic3_Injection		:1; // 18 
			INT8U Optic4_Injection		:1; // 19			
			INT8U						:4; // 20~23 
			INT8U						:4; // 24~27 
			INT8U						:1; // 28 
			INT8U						:1; // 29 
			INT8U CPRI2_L1_Reset		:1; // 30 
			INT8U CPRI1_L1_Reset		:1; // 31			
		}__attribute__((packed));
	}RW_Interrupt_Status1; // 0x23 : Interrupt Status1 - Optic module & L1 Reset

	
//----------------------------------------
//		SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------
	struct
	{
		//----------------------------------------
		// [15:0] LD Power,  예) -517 => -51.7 dBm
		//----------------------------------------	
		type32 R_SFP_LD_Power; // Addr : 0x00 		
		//----------------------------------------	
		// [15:0] PD Power,  예) -517 => -51.7 dBm
		//----------------------------------------	
		type32 R_SFP_PD_Power; // Addr : 0x01 		
		//----------------------------------------	
		// [15:0] [15:0] VCC  예) 31 => 3.1V
		//----------------------------------------	
		type32 R_SFP_VCC; // Addr : 0x02 
		//----------------------------------------	
		// [15:0] VCC  예) 135 => 13.5mA
		//----------------------------------------	
		type32 R_SFP_LD_Bias; // Addr : 0x03 
		//----------------------------------------	
		// [15:0] Termperature  예) -517 => -51.7 ℃
		//----------------------------------------	
		type32 R_SFP_Temp; // Addr : 0x04 
		//----------------------------------------	
		// [15:0] wavelength  예) 1330 => 1330 nm
		//----------------------------------------	
		type32 R_SFP_wavelength; // Addr : 0x05 
		//----------------------------------------	
		// [0] : TX DISABLE (DISABLE='1',  ENABLE='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U EnDis				:1;				
			}__attribute__((packed));
		}RW_SFP_TX_DISABLE; // Addr : 0x06		
		//----------------------------------------	
		// [0] : Reset (Reset = '1',  Normal='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Reset				:1;				
			}__attribute__((packed));
		}RW_SFP_Reset; // Addr : 0x07 
		//----------------------------------------
	}Common_CPRI[CPRI_MAX_PATH]; // SFP Common	
//----------------------------------------
//	End of 
//  SFP Common : CPRI #0 ~ CPRI #3
//----------------------------------------




//---------------------------------------------------------------------
// DSP-Delay
// DELAY = delay(uS) * 122.88(MSPS)
// Rang = 0 ~ 12531
//---------------------------------------------------------------------
	type32 RW_DL_DELAY; // addr 0x00
	type32 RW_UL_DELAY; // addr 0x01

//------------------------------------------------------------
// Start of DSP-FFT
//------------------------------------------------------------	
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------	
	// [2]   : DL_UL,   (DL='0', UL='1')
	// [1:0] : MIMO Num (1st ='0', 2nd ='1', 3rd ='2', 4th ='3')
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U MIMO_Num			:2;
			INT8U DLUL				:1;			
		}__attribute__((packed));
	}RW_FFT_Mux; // 0x10, not used
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [0] : Trigger   (En = '1', Dis='0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT8U Trigger			:1;			
		}__attribute__((packed));			
	}RW_FFT_Trigger; // 0x11, not used
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [0] : Done	(Done='1', NotDone='0')
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT8U Done				:1;			
		}__attribute__((packed));
	}R_FFT_Done;  // 0x12, not used
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [12:0] : Address
	//----------------------------------------
	union{
		INT32U Data;
		struct{			
			INT16U Address			:12;			
		}__attribute__((packed));
	}RW_FFT_Address; // 0x13, not used

	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [31:0] FFT Result
	//----------------------------------------
	type32 RW_FFT_Result;  // 0x14, not used
	
//------------------------------------------------------------
// End of DSP FFT
//------------------------------------------------------------





//------------------------------------------------------------
// Start of 'Mode General-Dsp-Common'
//------------------------------------------------------------
	struct
	{
		//----------------------------------------
		// NCO = target_frequency(MHz)/245.76(MHz)*2^32
		//----------------------------------------
		type32 RW_NCO; // addr 0x00

		//----------------------------------------
		// Default Value = 0x0000 0000
		//----------------------------------------
		// [0] [ Valid = '1' , Invalid = '0']
		//----------------------------------------
		//* NCO Setting sequence
		//----------------------------------------
		// 1. write NCO value
		// 2. write NCO_VALID 1
		// 3. write NCO_VALID 0
		//----------------------------------------
		union{
			INT32U Data;
			struct{				
				INT8U Value				:1;				
			}__attribute__((packed));
		}R_NCO_VALID; // 0x01
		
		
	//------------------------------------------------------------
	// Start of 
	// Mode General - Dsp - FA/Port - MIMO #0~#3
	//------------------------------------------------------------
		struct
		{		
			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U OnOff			:1;					
				}__attribute__((packed));
			}RW_DL_OnOff; // 0x00
			//----------------------------------------
			// Default Value = 0x0000 4000
			//----------------------------------------
			// [15:0] Gain
			// Att 변환식, Att = 20*log10(GAIN/0x4000)
			// Min step = 0.5dB
			//----------------------------------------
			type32 RW_DL_Gain; // 0x01

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [15:0] Level
			//----------------------------------------
			type32 RW_DL_CW_Level; // 0x02

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
///// YCSO change 190408 type32 --> union
//			type32 RW_DL_CW_On; // 0x03
			union{
				INT32U Data;
				struct{					
					INT8U OnOff			:1;					
				}__attribute__((packed));
			}RW_DL_CW_On; // 0x03
///// YCSO change END
			//----------------------------------------
			// [31:0] TSSI 
			//----------------------------------------
			// TSSI = Mean(I^2 + Q^2)
			// dBFs 변환식, dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_DL_TSSI; // 0x04

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Inphase Ctrl Value, 
			//        Value = cos(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_DL_Inphase_Cont; // 0x05

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Quadrature Ctrl Value 
			// Value = sin(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_DL_Quad_Cont; // 0x06

			//----------------------------------------
			// Default Value = 0x0000 0005
			//----------------------------------------
			// [2:0] BW
			// 10MHz='0', 20MHz='1', 40MHz='2', 
			// 60MHz='3', 80MHz='4', 100MHz='5'
			//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U BW			:3;					
				}__attribute__((packed));
			}RW_DL_BW_Sel; // 0x07

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [15:0] EQUAIZER_COEFF
			//----------------------------------------
			type32 RW_DL_EQ_Coeff; // 0x08

			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [0] ON = '1', OFF = '0'
			//----------------------------------------
			union{
				INT32U Data;
				struct{
					INT8U Itself_OnOff				:1;
					INT8U Cascade_OnOff				:1;	
				}__attribute__((packed));
			}RW_UL_OnOff; // 0x20

			//----------------------------------------
			// Default Value = 0x0000 4000
			//----------------------------------------
			// [15:0] Gain
			//----------------------------------------
			// Att 변환식, Att = 20*log10(GAIN/0x4000)
			// Min step = 0.5dB
			//----------------------------------------
			type32 RW_UL_Gain; // 0x21

			//----------------------------------------
			// [31:0] RSSI 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_UL_RSSI; // 0x22

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Inphase Ctrl Value 
			//       Value = cos(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_UL_Inphase_Cont; // 0x23

			//----------------------------------------
			// Default Value = 0x0000 7FFF
			//----------------------------------------
			// [15:0] Quadrature Ctrl Value 
			//        Value = sin(theta) x 0x7FFF
			//----------------------------------------
			type32 RW_UL_Quad_Cont; // 0x24
						
			//----------------------------------------
			// [15:0] Inphase Det
			//----------------------------------------
			type32 R_UL_Inphase_Det; // 0x25 
			
			//----------------------------------------
			// [15:0] UL_Quad_Det
			//----------------------------------------
			type32 R_UL_Quad_Det; // 0x26 

			//----------------------------------------
			// Default Value = 0x0000 0005			
			//----------------------------------------
			// [2:0] BW
			//----------------------------------------
		  	// 10MHz='0', 20MHz='1', 40MHz='2'
		  	// 60MHz='3', 80MHz='4', 100MHz='5'
		  	//----------------------------------------
			union{
				INT32U Data;
				struct{					
					INT8U BW			:3;					
				}__attribute__((packed));
			}RW_UL_BW_Sel; // 0x27
			
			//----------------------------------------
			// Default Value = 0x0000 0000
			//----------------------------------------
			// [15:0] EQUAIZER_COEFF
			//----------------------------------------
			type32 RW_UL_Equalizer_Coeff; // 0x28
			
			//----------------------------------------
			// [31:0] RSSI 
			//----------------------------------------			
			// dBFs = 10*log10(RSSI/2^31)
			//----------------------------------------
			type32 R_DL_RSSI; // 0x30
			
		}General_FA_MIMO[CPRI_MAX_PATH];
	
	//------------------------------------------------------------
	// End of 
	// Mode General - Dsp - FA/Port - MIMO #0~#3
	//------------------------------------------------------------
	}General_FA[MAX_FA_PATH];
	
//------------------------------------------------------------
// End of 'Mode General-Dsp-Common'
//         Mode General - Dsp - FA/Port - MIMO
//------------------------------------------------------------




//------------------------------------------------------------
// Start of 'Mode General-VSS-CPRI'
//         Mode General - Dsp - FA/Port - MIMO
//------------------------------------------------------------	

	//------------------------
	// Down-Ward CPRI #0 ~ #3
	//------------------------
	struct{		
		type32 R_RECSERDESLOS; // addr : 0x00	
		type32 R_RECLOFALARM;  // addr : 0x01
		type32 R_RECTypeID;    // addr : 0x02
		type32 R_CnM_Path_selection; // addr : 0x03
		type32 R_RE_Reset;           // addr : 0x04
		type32 R_RE_Channel_BW;      // addr : 0x05
		type32 R_Base_IP_Address[4]; // addr 0x06-0x09-0x08-0x07

		type32 R_OpenCPRIVersion;     // addr : 0x0A
		type32 R_CPRICompressionOnOff;// addr : 0x0B
		type32 R_RE_Cascade_ID;       // addr : 0x0C
		type32 R_OpticLOSDetectAlarm; // addr : 0x0D			
		type32 R_Loopbackmode;        // addr : 0x0E
		
		type32 R_Delay_H; // MSB, addr : 0x0F
		type32 R_Delay_L; // LSB, addr : 0x10
		
		type32 R_REx_Reset[4]; // RE1_RESET ~ RE4_RESET, addr : 0x11 ~ 0x14
		
		type32 R_RE1TxDelay_H; // MSB, addr : 0x15
		type32 R_RE1TxDelay_L; // LSB, addr : 0x16
		
		type32 R_RE1RxDelay_H; // MSB, addr : 0x17
		type32 R_RE1RxDelay_L; // LSB, addr : 0x18
		
		type32 R_TDD_Slot_Number[20]; // Number_00 ~ Number_19, addr : 0x19 ~ 0x2C

		//----------------------------------------	
		// bit : Alarm = '1', Clear = '0'
		//----------------------------
		// [4] : LOF
		// [3] : LOS
		// [2] : SDI
		// [1] : RAI
		// [0] : Reset (Reset='1', NoReset='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Reset					:1; // 0
				INT8U RAI					:1; // 1
				INT8U SDI					:1; // 2
				INT8U LOS					:1; // 3
				INT8U LOF					:1; // 4
			}__attribute__((packed));
		}R_L1Reset; // addr : 0x40
		//----------------------------------------
		// [15:0] BIP Counter
		//----------------------------------------
		type32 R_BIP_Count;  // addr : 0x41
		//----------------------------------------
		// [0] Clear = '1', Normal = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U Clear				:1; // 0
			}__attribute__((packed));			
		}RW_BIP_Clear; // addr : 0x42	
		//----------------------------------------
	}DN_CPRI[CPRI_MAX_PATH]; // Down-Ward CPRI

	//------------------------
	// Up-Ward CPRI #0 ~ #3
	//------------------------
	struct{
		type32 R_RECSERDESLOS; // addr 0x00
		type32 R_RECLOFALARM;  // addr 0x01
		type32 R_REFunctionFail; // addr 0x02

		type32 R_RE_Reset_Ack; // addr 0x04
		
		type32 R_ALIVE_ACK_00; // addr 0x05
		type32 R_ALIVE_ACK_01; // addr 0x0A		

		type32 R_RE_Cascade_ID_ACK; // addr 0x0C
		type32 R_RE_OpticLOSDetectAlarm; // addr 0x0D
		type32 R_RE_Loopbackmode_ACK; // addr 0x0E

		type32 R_RE_Reset_ACK[4]; // RE1_RESET_ACK ~ RE4_RESET_ACK, addr : 0x11 ~ 0x14

		type32 R_RE_FunctionFail[4]; // RE1_FucnFail ~ RE4_FucnFail, addr 0x15 ~ 0x18

		type32 R_L1_Reset; 				
		//----------------------------------------	
		// bit : Alarm = '1', Clear = '0'
		//----------------------------
		// [4] : LOF
		// [3] : LOS
		// [2] : SDI
		// [1] : RAI
		// [0] : Reset (Reset='1', NoReset='0')
		//----------------------------------------	
		union{
			INT32U Data;
			struct{
				INT8U Reset					:1; // 0
				INT8U RAI					:1; // 1
				INT8U SDI					:1; // 2
				INT8U LOS					:1; // 3
				INT8U LOF					:1; // 4
			}__attribute__((packed));
		}R_L1_Reset_ACK; // addr : 0x40
		//----------------------------------------
		// [15:0] BIP Counter
		//----------------------------------------
		type32 R_BIP_Count;  // addr : 0x41
		//----------------------------------------
		// [0] Clear = '1', Normal = '0'
		//----------------------------------------
		union{
			INT32U Data;
			struct{
				INT8U Clear				:1; // 0
			}__attribute__((packed));
		}RW_BIP_Clear; // addr : 0x42	
		//----------------------------------------			
	}UP_CPRI[CPRI_MAX_PATH]; // Up-Ward CPRI
//------------------------------------------------------------
// End of 'Mode General-VSS-CPRI'
//         Mode General - Dsp - FA/Port - MIMO
//------------------------------------------------------------		

	
	



//------------------------------------------------------------
// Start of 'Mode General-TDD'
//------------------------------------------------------------	
	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [2] : UL_TDD_SEL  UL_TSYNC ='0', DL_TSYNC='1'
	// [1:0] : Mode  
	//       Normal='0', DL_ONLY='1', UL_ONLY='2', FDD='3'
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT8U Mode				:2;
			INT8U Sel				:1;			
		}__attribute__((packed));
	}RW_TDD_Mode; // 0x00

	//----------------------------------------	
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [7:0] : Slot Format
	//----------------------------------------
	type32 RW_Slot_Format[20]; // addr 0x01 ~ 0x14, Format_00 ~ Format_19

	//----------------------------------------
	// Default Value = 0x0000 0000
	//----------------------------------------
	// [10:0] Value    delay(uS) =  Value / 122.88(MSPS)
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value:11;
		}__attribute__((packed));
	}DL_ON_Guard_Time; // 0x15
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:11;			
		}__attribute__((packed));
	}UL_ON_Guard_Time; // 0x16
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:11;			
		}__attribute__((packed));
	}DL_to_UL_Guard_Time; // 0x17
	//----------------------------------------
	union{
		INT32U Data;
		struct{
			INT16U Value				:11;			
		}__attribute__((packed));
	}UL_to_DL_Guard_Time; // 0x18
	//----------------------------------------
//------------------------------------------------------------
// End of 'Mode General-TDD'
//------------------------------------------------------------	
}ST_DtuStatus;


typedef union{
	union{
		INT32U Data;
		struct{
			INT16U reserved		:9; // 0~8 
			INT16U addr8		:8; // 9~16
			INT8U  mimo			:4; // 17~20
			INT8U  fa_port		:4; // 21~24
			INT8U  func			:4; // 25~28
			INT8U  mode			:1; // 29
			INT8U  rdwr			:2; // 30~31
		}__attribute__((packed));
	};
}REG_ADDR;


/********************************************************************************/
#endif
/********************************************************************************/

/* EOF */

