/********************************************************************************/
#ifndef __UL_PROC_H
#define __UL_PROC_H
/********************************************************************************/
#ifdef  UL_PROC_GLOBALS
#define UL_PROC_EXT
#else
#define UL_PROC_EXT extern
#endif
/**************************************************************/


/*----------------------------------------------------------*/



UL_PROC_EXT INT8U Is_Low_Alm_Mask(void);
UL_PROC_EXT void Mask_Low_Alm(void);
UL_PROC_EXT void Low_Alm_Mask_Start(void);
UL_PROC_EXT void Unmask_Low_Alm(void);

UL_PROC_EXT INT8U Is_Global_Alm_Mask(void);
UL_PROC_EXT void Proc_Lo_Alm_Mask(void);



/********************************************************************************/
#endif
/********************************************************************************/





