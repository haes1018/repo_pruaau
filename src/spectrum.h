///// YCSO added START 190419
/********************************************************************************/
#ifndef __SPECTRUM_H
#define __SPECTRUM_H
/********************************************************************************/
#ifdef  SPECTRUM_GLOBALS
#define SPECTRUM_EXT
#else
#define SPECTRUM_EXT extern
#endif
/**************************************************************/


SPECTRUM_EXT INT32U Get_Spectrum_Timer_ms(void);


/*----------------------------------------------------------*/
// 외부 호출 함수
//
/*
YCSO 190624
PRU 와 AAU 일때 DL의 sample 개수가 다름.
*/
#ifdef __TYPE__PRU__
#define DL_SPECTRUM_SAMPLE_SIZE		8192
#else
#define DL_SPECTRUM_SAMPLE_SIZE		4096
#endif 
#define UL_SPECTRUM_SAMPLE_SIZE		4096

#ifdef __TYPE__PRU__
#define DL_BW	400 // 491, 2019.05.28
#define UL_BW	240 // 245, 2019.05.28
#else
#define DL_BW	240
#define UL_BW	240
#endif

#define FFT_8192		8192
#define FFT_8192_3_4	( (FFT_8192 * 3) / 4)

INT16S	sR[FFT_8192];
INT16S	sI[FFT_8192];
float wn_FFT[FFT_8192_3_4];		// for twiddle factor
short br_FFT[FFT_8192];			// for bit reversa

#define __YCSO__SPECTRUM__DEBUG__	///// debug for spectrum.

OS_TIMER spectrum_timer;

/*float	xP[FFT_8192] = {0};
float	Wave_data[FFT_8192] = {0};
INT16U	send_Wave_data[FFT_8192] = {0};
*/
#ifdef __MAKER_ANTS__ // AAU/PRU of ANTS
enum
{
	_DL0 = 0x00,
	_DL1,
	_DL2,
	_DL3,
	_UL0 = 0x10,
	_UL1,
	_UL2,
	_UL3,
	_UL0_NOISE = 0x20,
	_UL1_NOISE,
	_UL2_NOISE,
	_UL3_NOISE,
	_DL0_DUMP = 0xA0,
	_DL1_DUMP,
	_DL2_DUMP,
	_DL3_DUMP,
	_UL0_DUMP = 0xB0,
	_UL1_DUMP,
	_UL2_DUMP,
	_UL3_DUMP,
	_UL0_NOISE_DUMP = 0xC0,
	_UL1_NOISE_DUMP,
	_UL2_NOISE_DUMP,
	_UL3_NOISE_DUMP,
	_MAX_SPECTRUM_Type

};
#endif

/*----------------------------------------------------------*/
// 외부 참조 변수
//
SPECTRUM_EXT INT16U rxSpectrumRequest( PROTOCOL_FRAME* rxf, PROTOCOL_FRAME* txf );
SPECTRUM_EXT INT16U rxSpectrumReq_SIM(PROTOCOL_FRAME * rxf, PROTOCOL_FRAME * txf);
SPECTRUM_EXT void Init___Spectrum_Offset(void);

/*----------------------------------------------------------*/


/********************************************************************************/
#endif
/********************************************************************************/


//#EOF
///// YCSO added END 190419
