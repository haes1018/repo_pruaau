/////////////////////////////
// lo_host_comm.c
/////////////////////////////
#define LO_HOST_COMM_GLOBALS
#include "includes.h"
/////////////////////////////










/////////////////////////////
// Local Variables
/////////////////////////////


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////


void Disp_Flag_Data(INT16U d_len, INT16U f_len, INT8U* pData, char* str, INT16U max_buf )
{
	INT16U flag_data_size = d_len + f_len;
	char  hex_str[10];
	INT16U i;
	
	if(flag_data_size < (max_buf/3)) 
	{
		memset(str, 0, max_buf);	
		
		for(i = 0; i < f_len; i++)
		{
			sprintf(hex_str, " %02X", pData[i]);
			strcat(str, hex_str);
		}
		
		strcat(str, " +");

		for(i = 0; i < d_len; i++)
		{
			sprintf(hex_str, " %02X", pData[i+f_len]);
			strcat(str, hex_str);
		}		
	}
}

//------------------------------------------------------------------------
#if 0 // Not Used @ ANTS
#define AID_DTU_DISP_BUF (512)
static char aid_dtu_disp_data[AID_DTU_DISP_BUF];

static INT16U DIG_UDP_AID_Recv(void* pAID)
{
	AIDSTR*  paid  = (AIDSTR*)pAID;
	AIDSTR2* paid2 = (AIDSTR2*)pAID;
	AID2_INFO aid2_info;
	INT16U tot__len;
	INT16S len; // data_only_len (without flag)
	int re;
	
	
	INT8U flag_len; // default 1
	INT8U AID[2];		
	
	//INT32U* pVal32 = NULL;
	INT16U* pVal16 = NULL;
	INT8U* pVal8 = NULL;

	//INT32U uVal32;	
	INT16U uVal16;
	//INT16S sVal16;	
	INT8U  uVal8;
		
	INT8U  l, i;
	INT32U flag32;
	//INT8S  flag_msb; // flag_index

	AID[0] = paid->AID[0];
	AID[1] = paid->AID[1];


	//D_Printf(DEBUG_SYSTEM, "[RCU] DIG_UDP_AID_Recv.\n"); 

	if(Is_AID2_Range(AID[0])) 
	{	
		re = Parse_AID2_STR(paid2, &aid2_info);
		if(re == RET_Error)
			return aid2_info.tot__len;	
			
		if(aid2_info.aid2_len == 0) return 0;
		
		len = aid2_info.data_len; // data_only		
		tot__len = aid2_info.tot__len;
		
		flag_len = aid2_info.flag_len;
		
		/*
		INT8U size,	layer, carrier;	
		INT16U aid2_len;    // size of "type ~ data"
		
		aid2_len = aid2_info.aid2_len;
		size	 = aid2_info.size;
		layer	 = aid2_info.layer;
		carrier  = aid2_info.carrier;
		*/

		for(flag32=0, i = 0; i < flag_len; i++)
		{
			flag32 <<= 8;
			flag32 |= paid2->data[i];
		}
		//flag_msb = 7 + 8*(flag_len-1);
		//----------------------------------------
		pVal8  = &paid2->data[flag_len];
		pVal16 = (INT16U*)&paid2->data[flag_len];
		//pVal32 = (INT32U*)&paid2->data[flag_len];
		//----------------------------------------


		#if 1 // for debug display

		Print_AID2("[DTU_AID]", paid2, &aid2_info, aid_dtu_disp_data, AID_DTU_DISP_BUF);

		/*		
		D_Printf(DEBUG_SYSTEM, "[DTU AID Recv] flag_len %d, flag 0x%08X, flag_msb 0x%08X\n", 
			flag_len, flag32, cbits32[flag_msb]);
		*/
		#endif
	}
	else
	{
		len = paid->len; // data_only
		tot__len = len + 3;

		if(len == 0) return 0;
	}

	switch(AID[0])
	{
		//---------------------------------------------------------------
		case 0x01:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x00:
				case 0x01:
					//D_Printf(DEBUG_SYSTEM, "[DATA0] 0x%02X\n", paid->data[0]);
					break;
				//---------------
				default:break;
				//---------------
			}
			break;
		//---------------------------------------------------------------
		case 0x50:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x00: 
					if(pVal8 == NULL) break;
					for(l = 0; l < 2; l++)
					{
						uVal8 = *pVal8++;
						D_Printf(DEBUG_SYSTEM, "[lohost DATA1] 0x%02X\n", uVal8);
					}
					break;		

				case 0x02: // Layer 사용 정보	
					if(pVal8 == NULL) break;
					for(l = 0; l < MAX_DL; l++)
					{
						uVal8 = *pVal8++;
						//D_Printf(DEBUG_SYSTEM, "[DATA2] 0x%02X\n", uVal8);
					}
					break;

				//---------------
				default:break;
				//---------------
			}
			break;		
		//---------------------------------------------------------------
		case 0xB4:
		//---------------------------------------------------------------
			switch(AID[1])
			{
				case 0x0B: // CFR level
					for(i = 0; i < MAX_DL; i++)
					{						
						uVal16 = ntohs(*pVal16++); 
						D_Printf(DEBUG_SYSTEM, "[DATA3] %d\n", uVal16);
					}
					break;
					
				//--------------
				default: break;
				//--------------
			}
			break;					

		//---------------------------------------------------------------
		default: break;
		//---------------------------------------------------------------
	}
	return tot__len;
}
#endif
//---------------------------------------------------------------------------------









////--------------------------------------------------------------------------------------




/* EOF */

