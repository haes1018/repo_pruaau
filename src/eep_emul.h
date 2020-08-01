/********************************************************************************/
#ifndef __EEP_EMUL_H_
#define __EEP_EMUL_H_
/********************************************************************************/

extern INT8U ResetInfo_Write( INT8U* data, INT16U size );
extern INT8U ResetInfo_Read( INT8U* data, INT16U size );
extern INT8U V_EEP_Write( INT8U sec, INT16U addr, INT8U* data, INT16U size );
extern INT8U V_EEP_Read( INT8U sec, INT16U addr, INT8U* data, INT16U size );
extern void InitEEP( void );


/********************************************************************************/
#endif
/********************************************************************************/

