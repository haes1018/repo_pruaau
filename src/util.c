/////////////////////////////
// UTIL.C
/////////////////////////////
#define UTIL_GLOBALS
#include "includes.h"

#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//#include <netinet/in.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
//#include <net/if.h>
//#include <arpa/inet.h>


/////////////////////////////

#define isdigit(x)	(((x)>='0')&&('9'>=(x)))


/////////////////////////////
// Local Variables
/////////////////////////////


INT8U _table[17] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',0x20 };


/////////////////////////////
// Local Functions
/////////////////////////////

/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////

// 부호있는 수를 정수, 소수 모두 부호있게 바꿈.
void AID_Pwr( INT16S p, INT8U *d )
{
	d[0] = p/10;
	d[1] = p%10;
}

INT16S AID_Pwr_Ctrl( INT8U *d )
{
	INT16S p;

	p = ((INT8S)d[0]);
	p*=10;
	p+=((INT8S)d[1]);

	return p;
}


INT8S ROUND( INT16S val )
{
	INT8S mod;

	mod = val%10;

	val = val/10;

	if( mod>=5 ) val ++;
	if( mod<-5 ) val --;

	return val;
}

INT8S ROUND_05( INT16S val )
{
	INT8S mod;

	mod = val%5;
	val = val/5;

	if( mod>=3 ) val ++;
	if( mod<-3 ) val --;

	return val;
}
INT8U ROUND_05U( INT16S val )
{
	INT8S mod;
	INT8U ret;

	if( val>0 ) return 0;

	mod = val%5;
	val = val/5;

	if( mod>=3 ) val ++;
	if( mod<-3 ) val --;

	val = -val;
	ret = (INT8U)val;
	return ret;
}

INT8U ntoa(INT8U val)
{
    return _table[val%16];
}

INT8U atoh( INT8U v )
{
    if( isdigit(v) ) return (v -'0');
    else if( (v >= 'a') && (v <= 'f') ) return( 10 + v - 'a' );
    else if( (v >= 'A') && (v <= 'F') ) return( 10 + v - 'A' );
    return 0xFF;
}

INT8U AHex2Dig( INT8U *d )
{
	INT8U Value;
	Value = ( ( atoh( d[0] ) << 4 ) & 0xF0 ) | ( atoh( d[1] ) & 0x0F );
	return Value;
}

INT8U AHex2Dig2( INT8U *d )
{
	INT8U Value;
	Value = ( ( atoh( d[0] ) << 4 ) & 0x70 ) | ( atoh( d[1] ) & 0x0F );
	return Value;
}

INT16U AHex2Dig_INT16( INT8U *d )
{
	INT16U Value;
	Value = AHex2Dig( &d[0] );
	Value <<= 8;
	Value |= AHex2Dig( &d[2] );
	return Value;
}


void Dig2AHex( INT8U *h, INT8U d )
{
	h[0] = _table[(d>>4)&0x0F];
	h[1] = _table[d&0x0F];
}

void Dig2AHex_INT16U( INT8U *h, INT16U d )
{
	h[0] = _table[(d>>16)&0x0F];
	h[1] = _table[(d>>8)&0x0F];
	h[2] = _table[(d>>4)&0x0F];
	h[3] = _table[d&0x0F];
}

INT8U BCD2Dig( INT8U b )
{
	INT8U d;
	d = ((b>>4)&0x0F)*10 + (b&0x0F);
	return d;
}

INT8U Dig2BCD( INT8U d )
{
	INT8U b;
	b= ((d/10)<<4) + (d%10);
	return b;
}

void MakePwrValue( INT8U* v, INT16S p )
{
	INT8U sign;

	sign = 0; // Init
	if( p<0 )
	{
		sign = 0x40;
		p = -p;
	}

	v[0] = (p/10);
	v[0] |= sign;
	v[1] = (p%10);
}

void MakePnValue( INT8U* v, INT16U pn )
{
	v[0] = (INT8S)(pn>>6);
	v[1] = (INT8S)(pn&0x3F);
}

void Make_3G_Channel_Value( INT8U* v, INT16U pn )
{
	v[0] = (INT8S)((pn >> 12) & 0x0F);
	v[1] = (INT8S)((pn >> 6)  & 0x3F);
	v[2] = (INT8S)( pn        & 0x3F);
}

INT16U Swap16( INT16U val )
{
	INT8U h, l;

	h=(INT8U)(val>>8);
	l=(INT8U)(val);

	return ((l<<8)|h);
}




INT32U BYTE_to_INT32L( INT8U* psrc ) // little 
{
	return ((psrc[3]<<24)|(psrc[2]<<16)|(psrc[1]<<8)|psrc[0]);
}

INT32U BYTE_to_INT32B( INT8U* psrc ) // big endian
{
	return ((psrc[0]<<24)|(psrc[1]<<16)|(psrc[2]<<8)|psrc[3]);
}

void INT32B_to_BYTE( INT8U* pDst, INT32U Src32 )
{
	pDst[0] = (INT8U)(Src32 >>24); 
	pDst[1] = (INT8U)(Src32 >>16);
	pDst[2] = (INT8U)(Src32 >>8);
	pDst[3] = (INT8U)(Src32 >>0);
}

void INT32L_to_BYTE( INT8U* pDst, INT32U Src32 )
{
	pDst[3] = (INT8U)(Src32 >>24); 
	pDst[2] = (INT8U)(Src32 >>16);
	pDst[1] = (INT8U)(Src32 >>8);
	pDst[0] = (INT8U)(Src32 >>0);
}

INT16U BYTE_to_INT16L( INT8U* psrc )
{
	return ((psrc[1]<<8)|psrc[0]);
}

INT16U BYTE_to_INT16B( INT8U* psrc )
{
	return ((psrc[0]<<8)|psrc[1]);
}

void INT16B_to_BYTE( INT8U* pDst, INT16U Src16 )
{
	pDst[0] = (INT8U)(Src16 >>8); 
	pDst[1] = (INT8U)(Src16 >>0);
}
void INT16L_to_BYTE( INT8U* pDst, INT16U Src16 )
{
	pDst[1] = (INT8U)(Src16 >>8);
	pDst[0] = (INT8U)(Src16 >>0);
}


void date_to_a( INT8U *h, INT8U* date, INT8U len )
{
	INT8U i, k;
	INT8U val;

	// date[i] 값은 0 ~ 99 이어야 함.
	memset( h, '0', 2*len);
	
	for(k = 0, i = 0; i < len; i++)
	{		
		val = date[i];
		if(val > 99) val = 99;
		
		h[k++] += (val / 10);
		h[k++] += (val % 10);
	}
}




void calcBits( INT8U *Value, INT8U State, INT8U idx )
{
	if (ALM_JIG_MODE == ALM_JIG_MODE_CODE) return;
	
    if( State ) *Value |= cbits[idx];
    else        *Value &= nbits[idx];
}

void ctrlBits( INT8U *Value, INT8U State, INT8U idx )
{	
    if( State ) *Value |= cbits[idx];
    else        *Value &= nbits[idx];
}

void Hysterisis_Alm_Chk( INT8U* pAlm, INT16S a, INT16S b, INT16S h )  // a>=b 알람 발생
{	
	if(*pAlm > 0) // if now alarm
	{
		if((a+h) <= b) *pAlm = 0; // clear
	}
	else
	{
		if(a >= b) *pAlm = 1; // alarm set
	}
}

INT8U Hys_Alm_Update(INT8U alm, INT16S a, INT16S b, INT16S h)  // a>=b 알람 발생
{	
	INT8U ret_alm = alm; // init copy
	
	if(ret_alm) // if now alarm
	{
		if((a+h) <= b) ret_alm = 0; // clear
	}
	else
	{
		if(a >= b) ret_alm = 1; // alarm set
	}
	return ret_alm;
}



void HysAlarmCheck( INT8U* palm, INT8U bit, INT16S a, INT16S b, INT16S h )  // a>=b 알람 발생
{
//	if (ALM_JIG_MODE == ALM_JIG_MODE_CODE) return;
	
	if( (*palm)& cbits[bit] )
	{
		if( (a+h) <= b ) (*palm)&=nbits[bit];
	}
	else
	{
		if( a >= b ) (*palm)|=cbits[bit];
	}
}

// PHONE
INT8U MakeSMSInPowerVal( INT8S val )
{
	if( val>0 ) return 0;

	val = -val;

	val>>=1;

	return (INT8U)(val);
}
INT8S MakeGUIInPowerVal( INT8U val )
{
	if( val>63 ) val = 63;
	val<<=1;
	val = -val;
	return (INT8S)(val);
}
INT8U MakeSMSPowerVal( INT8S val )
{
	if( val>60 ) val = 60;
	if( val<-60 ) val = -60;

	if( val>0 ) return val;

	val = -val;
	return (val|0x40);
}

INT16S MakePwrValue_GUI( INT8U* v )
{
	INT16S p;
	INT8U sign;

	if( v[0]&0x40 ) sign = 1;
	else sign = 0;
	p = v[0]&0x3F;
	p*=10;
	p+=v[1];
	if( sign ) p=-p;

	return p;
}




/*----------------------------------------------------------
TblSize 의 크기를 갖는 테이블에서,
value 가 들어 있는 인덱스의 앞과 뒤 인덱스를 리턴.

compare : (* pTbl)[2] 이면, 포인터사이즈가 2이고, 0혹은1의 값만 올 수 있다.
{x, y} 에서 x를 비교하려면, 0 이 된다.

테이블은 현재, 큰값에서 작은 값의 순서로 sort 되어 있고, 이때, idx1이 큰 값쪽의 인덱스.

테이블의 각 항목은 16비트 부호 정수.
----------------------------------------------------------*/

INT8U Get_Item_Idx2(INT16S value, const INT16S(* pTbl)[2], INT16S TblSize, INT8U compare)
{
	unsigned char idx = 0; // 중간 인덱스, 최종 인덱스.
	unsigned char idx1 = 0; // 큰 쪽 인덱스
	unsigned char idx2 = (TblSize-1); // 뒤쪽 작은 값 인덱스
	signed short tbl_val;
	unsigned char CompareCnt = 0;

	//	signed short tbl_val2, tbl_val3;


	while ((idx1 + 1) != idx2)
	{
		CompareCnt++;

		idx = (idx1 + idx2)/2;

		tbl_val = pTbl[idx][compare]; // compare : 0 항목을 비교하려면 0.

		if(value == tbl_val) { return idx;}
		else if (value > tbl_val) idx2 = idx;
		else idx1 = idx;

		if ((idx1 + 1) == idx2)
		{
			return idx1;
		}
	}

	return idx;

}

/*----------------------------------------------------------
{x1, y1}, {x2, y2} @ table 에서, x에 해당하는 Y값을 리턴.
테이블의 각 항목은 16비트 부호 정수.
----------------------------------------------------------*/

signed short Get_Y_From_x( signed short (*pTbl)[2], signed short x)
{
	//double slope, b;
	signed short x1,x2,y1,y2;

	x1 = pTbl[0][0];
	y1 = pTbl[0][1];
	x2 = pTbl[1][0];
	y2 = pTbl[1][1];

	/*
	slope = (double)(y1-y2)/(x1-x2);
	y1 = slope*x1 + b;
	b = y1 - slope * x1;
	Y = slope * x + b;
	Y = slope * x + (y1 - slope * x1);
	Y = (signed short)(slope * (x - x1) + (double)y1);
	Y = (signed short)((double)(y1-y2)/(x1-x2) * (x - x1) + (double)y1);
	*/

	// slope라는 변수를 쓰지 않고 사용
	return (signed short)((y1-y2) * (x - x1)/(double)(x1-x2) + (double)y1);

}
/*----------------------------------------------------------*/
// 테이블 pPwr_x10 에서, num 갯수 만큼에서 최대값과 최소값의
// 차이를 절대값으로 리턴한다.
// 차이 및 파워 값은 x10 한 값이다.
/*----------------------------------------------------------*/
INT16U Get_Differenc(INT16S* pPwr_x10, INT8U num)
{
	INT8U i;
	INT16S max_x10, min_x10;


	max_x10 = pPwr_x10[0];
	min_x10 = pPwr_x10[0];

	for (i = 1; i < num; i++)
	{
		if (pPwr_x10[i] > max_x10) max_x10 = pPwr_x10[i];
		else if (pPwr_x10[i] < min_x10) min_x10 = pPwr_x10[i];
	}

	return (max_x10 - min_x10);
}

/*----------------------------------------------------------*/
//
/*----------------------------------------------------------*/



char* Get_OnOff_Str(INT8U onoff)
{
	if(onoff == ON)
		return "ON ";
	else	
		return "Off";
}

char* Get_Alarm_Str(INT8U alarm)
{
	if(alarm == 0)
		return "Normal";
	else	
		return "Alarm";
}




static char* str_lte_null = "LTE__";

static char* str_amp_1_null = "AMP_1";
static char* str_amp_2_null = "AMP_2";
static char* str_amp_3_null = "AMP_3";
static char* str_amp_4_null = "AMP_4";




static char* str_r5_null = "R5_?";
static char* str_r7_null = "r7_?";
static char* str_p10_null = "p10_??";


char* Get_P10_Str( INT8U p10 )
{	
	return str_p10_null;
}

char* Get_RP7_Str( INT8U r7 )
{	
	return str_r7_null;
}



char* Get_fpga_Str( INT8U fpga_idx )
{	
	return str_lte_null;
}



char* Get_path4_Str( INT8U path4 )
{	
	switch(path4)
	{	
		case AMPU_4_1: return str_amp_1_null;
		case AMPU_4_2: return str_amp_2_null;
		case AMPU_4_3: return str_amp_3_null;
		case AMPU_4_4: return str_amp_4_null;

		default : return str_lte_null;
	}

	return str_lte_null;
}

char* Get_RVS_5_Str( INT8U rvs_5 )
{	
	return str_r5_null;
}



char* Get_D10_Str( INT8U path10 )
{	
	return str_lte_null;
}



char* Get_SM_Str( INT8U path4 )
{
	return str_lte_null;
}



/*----------------------------------------------------------*/
//
/*----------------------------------------------------------*/


static char* str_xxhs = "_?HS_";


char* Get_Up_Dev_Str( INT8U dev )
{	
	return str_xxhs;
}

char* Get_Up_DevType_Str( INT8U up_dev_type )
{	
	return str_xxhs;
}
/*----------------------------------------------------------*/
//
/*----------------------------------------------------------*/

static char* str__cmd_alarm = "Alm_RPT";
static char* str__cmd_sup_alm = "SupprAlm";
static char* str__cmd_statistic = "STATIS";
static char* str__cmd_auto_Cfg_nC = "Auto_CFnC";
static char* str__cmd_auto_Cfg__C = "Auto_CF_C";
static char* str__cmd_xxxxx = "cmd_??";

char* Get_CMD_Str( INT8U cmd )
{	
	switch(cmd)
	{	
		case CMD2_ALARM_LOC_RPT  : return str__cmd_alarm;
		case CMD2_SUPP_ALARM_RPT : return str__cmd_sup_alm;
		case CMD2_STATISTIC_RPT  : return str__cmd_statistic;
		case CMD2_CFG_nC_AUTO_RPT: return str__cmd_auto_Cfg_nC;
		case CMD2_CFG__C_AUTO_RPT: return str__cmd_auto_Cfg__C;
		
		default : return str__cmd_xxxxx;
	}
}

/////////////////////////////////////////////////////////////////////////

static char* str_reason_user = "by_User";
static char* str_reason_down = "by_Dnld";
static char* str_reason__pwr = "by__Pwr";
static char* str_reason__etc = "by__etc";
static char* str_reason__prd = "by__Prd";
static char* str_reason__pwrctrl = "by_PwrCtrl";
static char* str_reason__none = "none";

// 2019.08.27 : add reset-reason
static char* str_reason__vss      = "by_VSS";
static char* str_reason__udp_pd   = "by_Uda_PD";
static char* str_reason__uda_link = "by_Uda_Link";
static char* str_reason__uda_clk  = "by_Uda_Clk";

char* Get_Rst_Reason_Str( INT8U reason )
{
	switch ( reason )
	{
		case Rst_By_User:     return str_reason_user;
		case Rst_By_Download: return str_reason_down;
		case Rst_By_PwrFail:  return str_reason__pwr;
		case Rst_By_Etc:      return str_reason__etc;

		case Rst_By_Period:   return str_reason__prd;
		case Rst_By_PwrCtrl:  return str_reason__pwrctrl;

		/*
			2019.08.27 : add reset-reason
		*/
		case Rst_By_Vss:         return str_reason__vss;
		case Rst_By_Uda_Pd_CPRI: return str_reason__udp_pd;
		case Rst_By_Uda_Link:    return str_reason__uda_link;
		case Rst_By_Uda_Clock:   return str_reason__uda_clk;
		
		default: return str_reason__none;
	}
}


/*
----------------------------------------------------------
gREMS Reset Reason String
----------------------------------------------------------
*/
static char* rems_str_reason_user = "운용자 제어";
static char* rems_str_reason_down = "F/W Download"; // "F/W Download";
static char* rems_str_reason__pwr = "전원이상";
static char* rems_str_reason__etc = "기타";
static char* rems_str_reason__prd = "Period";
static char* rems_str_reason__pwrctrl = "전원제어";
static char* rems_str_reason__vss      = "VSS";
static char* rems_str_reason__udp_pd   = "UDA_PD(CPRI)";
static char* rems_str_reason__uda_link = "UDA_Link";
static char* rems_str_reason__uda_clk  = "UDA_Clock";
static char* rems_str_reason__unknown  = "?";

char* Get__gREMS_Reset_Reason_Str(INT8U reason)
{
	switch (reason)
	{
		case Rst_By_User:     return rems_str_reason_user;
		case Rst_By_Download: return rems_str_reason_down;
		case Rst_By_PwrFail:  return rems_str_reason__pwr;
		case Rst_By_Etc:      return rems_str_reason__etc;
		case Rst_By_Period:   return rems_str_reason__prd;
		case Rst_By_PwrCtrl:  return rems_str_reason__pwrctrl;
		case Rst_By_Vss:         return rems_str_reason__vss;
		case Rst_By_Uda_Pd_CPRI: return rems_str_reason__udp_pd;
		case Rst_By_Uda_Link:    return rems_str_reason__uda_link;
		case Rst_By_Uda_Clock:   return rems_str_reason__uda_clk;
		
		default: return rems_str_reason__unknown;
	}
}



	


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



int
set_system_time(int year, int month, int day, int hour, int minute, int second)
{
	struct tm _tm;
	time_t _time;
	int re;

	memset(&_tm, 0, sizeof(struct tm));

	_tm.tm_year  = year + 2000 - 1900;
	_tm.tm_mon   = month -1;
	_tm.tm_mday  = day;

	_tm.tm_hour  = hour;
	_tm.tm_min   = minute;
	_tm.tm_sec   = second;

	if ( _tm.tm_year >= 116							/* >= 2016 */
	 && (_tm.tm_mon  >= 0 && _tm.tm_mon  <  12)
	 && (_tm.tm_mday >  0 && _tm.tm_mday <  32)
	 && (_tm.tm_hour >= 0 && _tm.tm_hour <  24)
	 && (_tm.tm_min  >= 0 && _tm.tm_min  <  60)
	 && (_tm.tm_sec  >= 0 && _tm.tm_sec  <= 60) )
	{
		_time = mktime(&_tm);
		re    = stime(&_time);
	}
	else
		re = -1;

	return re;
}



/*
//int get_system_time(void)
//{
//	struct tm _tm;
//	time_t _time;
//	int val;	
//
//	time(&_time);	
//	gmtime_r(&_time, &_tm);	// _tm 의 UTC로 변환.
//	
//	val = _tm.tm_year - 2000 + 1900;
//	if(val < 0) val = 0;
//	m_St.Ru_Sys_Date[0] = val;
//	
//	m_St.Ru_Sys_Date[1] = _tm.tm_mon + 1;	
//	m_St.Ru_Sys_Date[2] = _tm.tm_mday;
//	m_St.Ru_Sys_Date[3] = _tm.tm_hour;
//	m_St.Ru_Sys_Date[4] = _tm.tm_min;
//	m_St.Ru_Sys_Date[5] = _tm.tm_sec;
//
//	return 0;
//}
*/

int set_Ro_system_time(INT8U* time)
{
	return set_system_time( time[0], time[1], time[2], time[3], time[4], time[5]);
}













/*
-----------------------------------------------------------------------------------

	manipulating network file
	
-----------------------------------------------------------------------------------
*/

static char keyWord[20];
static char key_Val[50];


void init_key_val_str(void)
{
	memset( keyWord, 0, 20);
	memset( key_Val, 0, 50);
}



int replace_keywd_value( char* keywd, char* value )
{
	FILE* fp_src;
	FILE* fp_new;
	struct stat st;
	INT8U size;
	
	int re;
	char buf[512];

	char *p_chr;
	
	fp_src = fopen( INTERFACE_FILE, "r" );
	if( fp_src == NULL ) 
	{
		printf("file %s open failed (%s)\n", INTERFACE_FILE, strerror(errno));
		return 1;
	}

	fp_new = fopen( NEW_INTERFACE_FILE, "w" );
	if( fp_new == NULL ) 
	{
		printf("file %s open failed (%s)\n", NEW_INTERFACE_FILE, strerror(errno));
		return 1;
	}

	re = fstat( fileno(fp_src), &st );

	if (re < 0 || st.st_size <= 0)
		return 1;

	//printf("----file %s size is %ld\n", INTERFACE_FILE, st.st_size );

	/*	return value 
	 *		0 = success
	 *		1 = fail
	 */
	re = 1;

	while(1)
	{
		if( fgets( buf, 128, fp_src ) == NULL )
			break;

		//printf("--%s", buf );
		if( strstr( buf, keywd ) != NULL )
		{
			p_chr = strchr(buf, '=');
			if (p_chr)
			{
				size = (p_chr - buf);
				if ( size == strlen(keywd))
				{
					sprintf( buf, "%s=%s\n", keywd, value);
					//printf("--------%s", buf );
					re = 0;
				}
			}
		}

		fwrite( buf, strlen(buf), 1, fp_new );
		sync();
	};		

	fflush(fp_new);

	fclose(fp_src);
	fclose(fp_new);

	if (re == 0)
	{
		/*	rename
		 *		network     -> network.old
		 *		network.new -> network
		 */
		rename(INTERFACE_FILE, OLD_INTERFACE_FILE);
		rename(NEW_INTERFACE_FILE, INTERFACE_FILE);
	}
	else
		remove(NEW_INTERFACE_FILE);
	sync();

	return re;
}

int get_keywd_value( char* keywd, int* val )
{
	FILE* fp_src;	
	int re = 1; // init fail
	char buf[512];
	char *p_chr;
	char buf_str[10];
	
	fp_src = fopen( INTERFACE_FILE, "r" );
	if( fp_src == NULL ) 
	{
		printf("file %s open failed (%s)\n", INTERFACE_FILE, strerror(errno));
		return 1;
	}

	while(1)
	{
		if( fgets( buf, 128, fp_src ) == NULL )
			break;
		
		if( strstr( buf, keywd ) != NULL )
		{	
			p_chr = strchr(buf, '=');
			if (p_chr)
			{
				p_chr++;

				if(strcmp(keywd, "IPV4DHCP0") == 0)
				{					
					sscanf( p_chr, "%s\n", buf_str );
					
					if(strcmp(buf_str, "yes") == 0)      val[0] = 1;
					else if(strcmp(buf_str, "no") == 0 ) val[0] = 0; // no
					else                                 val[0] = 0xFF; // init
				}				
				else if(strcmp(keywd, "MAC0") == 0 || strcmp(keywd, "MAC1") == 0)
				{
					sscanf(p_chr, "%02X:%02X:%02X:%02X:%02X:%02X\n", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]);
				}				
				else
				{
					sscanf( p_chr, "%d.%d.%d.%d\n", &val[0], &val[1], &val[2], &val[3] );
				}
				re = 0; // success
				break;
			}			
		}
	};
	
	fclose(fp_src);	
	return re;
}

// below is for zynq

#ifdef __MAKER_ANTS__	//ANTS

#ifdef __TYPE__PRU__
#define  IPv4_ADDR_Str     "IPV4ADDR"
#define  IPv4_DefGW_Str    "IPV4_DEFAULT_GW"
#define  IPv4_Net_Mask_Str "IPV4NETMASK"
#define  IPv4_DHCP_Str     "IPV4DHCP"
#define  IPv4_INIT_Str     "IPV4INIT"
#define  IP_MACAddr_Str    "MAC"
#define  IPv4_BrdCast_Str  "IPV4BROADCAST"
#else // AAU ?????????????????????
#define  IPv4_ADDR_Str     "IPV4ADDR"
#define  IPv4_DefGW_Str    "IPV4_DEFAULT_GW"
#define  IPv4_Net_Mask_Str "IPV4NETMASK"
#define  IPv4_DHCP_Str     "IPV4DHCP"
#define  IPv4_INIT_Str     "IPV4INIT"
#define  IP_MACAddr_Str    "MAC"
#define  IPv4_BrdCast_Str  "IPV4BROADCAST"
#endif

#else	//HFR

#ifdef __TYPE__PRU__	//PRU

#else	//AAU

#define  IPv4_ADDR_Str     "Address"
#define  IPv4_DefGW_Str    "Gateway"
#define  IPv4_Net_Mask_Str "IPV4NETMASK"
#define  IPv4_DHCP_Str     "DHCP"
#define	 IPv4_Link		   "[Link]"
#define  IP_MACAddr_Str    "MACAddress"
#define  IPv4_BrdCast_Str  "IPV4BROADCAST"
#endif

#endif


void read_network_file(void)
{
	int num[10];
		
	get_keywd_value( IPv4_ADDR_Str, num);	  memcpy(m_NetFile.IP_ADDR,  num, 4);	
	get_keywd_value( IPv4_DefGW_Str, num);	  memcpy(m_NetFile.IP_Def_GW,num, 4);	
	get_keywd_value( IPv4_Net_Mask_Str, num); memcpy(m_NetFile.IP_Mask,  num, 4);
	get_keywd_value( IP_MACAddr_Str, num);    memcpy(m_NetFile.IP__MAC,  num, 6);
	get_keywd_value( IPv4_DHCP_Str, num); 	  m_NetFile.IP_DHCP = num[0];	
}


/*
--------------------------------------------------------------------------
int re = system(buf);

re 하위 1바이트 : 
	system 함수 자신의 리턴 값, 보통 0임. 
	0이 아닌 경우 실패인데, buf 의 실행이 permission 오류인 경우이고, 그때의 errno 을 보면됨.
	
re 2번째 바이트 :
	re 하위 1바이트가 정상 일 때, 의미 있음.
	2번째 바이트는 buf 안의 명령의 실행 후, exit code 이며, 1바이트 값(shell은 1바이트만 전달할 수 있음)
	buf 안의 명령이 실패했을 때의 errno은 전달 받을 수 없다.

	if(re & 0xff == 0) // buf 실행 성공
	{
		Byte exit_code = (re >> 8) & 0xff;
		Switch(exit_code)
		{
			Case 0 : … break;
			Case 1 : … break;
			Case 2 : … break;
			Case 3 : … break;
			Case 4 : … break;		
		}
		// buf 내부 명령어의 errno은 system() 함수를 call 해서 전달 받을 수 없다. 
	} 
	else
	{
		// buf 실행 실패.
		// errno을 확인
	} 
--------------------------------------------------------------------------
*/

#define _SYSTEM_AGENT_BUF_SIZE_         512

int
system_agent(char *fmt, ...)
{
    int re;
    int len;
    char buf[_SYSTEM_AGENT_BUF_SIZE_] = { 0 };
    va_list ap;

    len = 0;

    va_start(ap, fmt);
    vsnprintf (&buf[len], _SYSTEM_AGENT_BUF_SIZE_-len, fmt, ap);
    va_end(ap);

    re = system(buf);

    /*
    re 
    lower 1byte 
    	if 0 : system command success
    	else : system command fail

    higer 1byte 
    	exit code of command    	
    */

	
	// 2020.06.08 : fix...
	if((re & 0xff) == 0) // buf 실행 성공
	{
		int exit_code = (re >> 8) & 0xff;

		if(exit_code != 0)
		{
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[system] %s, exit(%d)" VT_END_CR, buf, exit_code);
			return exit_code;
		}

		// buf 내부 명령어의 errno은 system() 함수를 call 해서 전달 받을 수 없다. 
	} 
	else // buf 실행 실패.
	{		
		// system api 가 error 를 발생한 경우
		print_agent(DEBUG_SYSTEM, VT_BLINK_RED, "[errno %d] %s (%s).", errno, buf, strerror(errno));
	} 
    // printf("***[SYSTEM]*******************************(%d)=[%s]\n", re, buf);
    return re;
}


int
system_agent_sync(char *fmt, ...)
{
	int re;
	int len;
	char buf[_SYSTEM_AGENT_BUF_SIZE_] = { 0 };
	va_list ap;

	len = 0;

	va_start(ap, fmt);
	vsnprintf (&buf[len], _SYSTEM_AGENT_BUF_SIZE_-len, fmt, ap);
	va_end(ap);

	re = system(buf);

	// 2020.06.08 : fix...
	if((re & 0xff) == 0) // buf 실행 성공
	{
		system("sync");
		
		int exit_code = (re >> 8) & 0xff;

		if(exit_code != 0)
		{
			print_agent(DEBUG_SYSTEM, VT_YELLOW, "[system] %s, exit(%d)" VT_END_CR, buf, exit_code);			
		}
		
		// buf 내부 명령어의 errno은 system() 함수를 call 해서 전달 받을 수 없다. 
	} 
	else // buf 실행 실패.
	{		
		// system api 가 error 를 발생한 경우
		print_agent(DEBUG_SYSTEM, VT_BLINK_RED, "[errno %d] %s (%s).", errno, buf, strerror(errno));
	} 
	 	
	// printf("***[SYSTEM]*******************************(%d)=[%s]\n", re, buf);
	return re;
}


void print_agent(INT16U mask, char *color, char *fmt, ...)
{
    int len;
    char buf[_SYSTEM_AGENT_BUF_SIZE_] = { 0 };
    va_list ap;

    len = 0;

    va_start(ap, fmt);
    vsnprintf (&buf[len], _SYSTEM_AGENT_BUF_SIZE_-len, fmt, ap);
    va_end(ap);
	
	printf("%s<RCU> %s" VT_END_CR, color, buf);
	D_Printf(mask, "%s<RCU> %s", color, buf);    
}


#define	MAXBUF			512

int
popen_agent(char *recvbuf, int maxsize, char *fmt, ...)
{
	int exit_code;
	char buf[MAXBUF] = { 0 };
	va_list ap;
	FILE *fp;
	int size;

	va_start(ap, fmt);
	vsnprintf (buf, MAXBUF, fmt, ap);
	va_end(ap);

	fp = popen(buf, "r"); // r : 표준출력을 읽기위한 pipe 

	if(fp == NULL)
		return -1;

	memset(recvbuf, 0, maxsize);

	size = fread(recvbuf, 1, maxsize, fp);
	
	exit_code = pclose(fp); // -1 if fail

	if(size == 0)
		print_agent(DEBUG_SYSTEM, VT_RED, "[popen_agent] size is 0, exit_code %#x, '%s'", exit_code, buf);

	if (exit_code & 0xff)
		return exit_code;
	else	
		return exit_code >> 8;
}




/*
ifconfig up 하기 전에 mac 먼저, 다음에 ip address
netmask 먼저, default gateway를 나중에.

netmask 를 하면, 
default gateway 가 지워짐.
*/
void Net_Set_Info_Set(INT8U net_cfg, INT8U* net)
{
	switch(net_cfg)	
	{
		case _IPADDR0_:
			system_agent("ifconfig eth0 %d.%d.%d.%d up", 
				net[0],net[1],net[2],net[3]);
			break;
			
		case _DEFAULT_GW_:
			system_agent("route add default gw %d.%d.%d.%d eth0",	
				net[0],net[1],net[2],net[3]);	
			break;	
		
		case _NETMASK0_:
			system_agent("ifconfig eth0 netmask %d.%d.%d.%d", 
				net[0],net[1],net[2],net[3]);
			break;
			
		case _MAC0_:
			system_agent("ifconfig eth0 hw ether %02X:%02X:%02X:%02X:%02X:%02X", 
				net[0],net[1],net[2],net[3],net[4],net[5]);	
			break;

		default:
			return;
	}	
}

void Net_Get_Info_Update(void)
{
	print_agent(DEBUG_SYSTEM, VT_GREEN, "NetWork Read IP/MAC : ---------"); 
	Get_system_mac_ip(m_St.Operate_Mac, m_St.Operate_Ip);
	
	print_agent(DEBUG_SYSTEM, VT_GREEN, "Operate_Ip  %d.%d.%d.%d", 
		m_St.Operate_Ip[0], 
		m_St.Operate_Ip[1], 
		m_St.Operate_Ip[2], 
		m_St.Operate_Ip[3]);
		
	print_agent(DEBUG_SYSTEM, VT_GREEN, "Operate_Mac %02X:%02X:%02X:%02X:%02X:%02X", 
		m_St.Operate_Mac[0], 
		m_St.Operate_Mac[1], 
		m_St.Operate_Mac[2], 
		m_St.Operate_Mac[3], 
		m_St.Operate_Mac[4], 
		m_St.Operate_Mac[5]);
}


/*
---- below is for zynq -----

# Network config file

HOST_NAME=zynqmp

MAC0=00:20:18:12:20:00
MTU0=1500

IPV4DHCP0=yes
IPV4INIT0=no
IPV4ADDR0=192.168.0.10
IPV4BROADCAST0=192.168.0.255
IPV4NETMASK0=255.255.255.0

IPV4_DEFAULT_GW=192.168.1.1

IPV6DHCP0=no
IPV6INIT0=yes

#IPV6ADDR0=2001:DB8:4:4::10/64
#IPV6_DEFAULT_GW=2001:DB8:4:4::1/128

# EOF

*/

void Net_Start( INT16U idx )
{	
	system( NETWORK_START );
	printf( VT_BOLD_YELLOW "[RCU] Net Start(#%d) :%s" VT_END_CR, idx, NETWORK_START );
	OSTimeDly(5*OS_DELAY_1sec);
}

void Net_Stop( void )
{
	system( NETWORK_STOP );
	printf( VT_BOLD_YELLOW "[RCU] Net Stop :%s" VT_END_CR, NETWORK_STOP );
	OSTimeDly( OS_DELAY_1sec );
}

void disp_time(INT8U* pTime)
{
	int val;	
	time_t the_time;
	
	time(&the_time); // get current time	
		 
	struct tm *tm_ptr; 
	tm_ptr = localtime(&the_time); 

/*
	D_Printf (DEBUG_SYSTEM, " time %d-%d-%d %d:%d:%d\n", 
		tm_ptr->tm_year+1900, 
		tm_ptr->tm_mon+1, 
		tm_ptr->tm_mday, 
		tm_ptr->tm_hour, 
		tm_ptr->tm_min, 
		tm_ptr->tm_sec);
*/
	val = tm_ptr->tm_year+1900 - 2000;
	if(val < 0) val = 0;
	
	pTime[0] = val;
	pTime[1] = tm_ptr->tm_mon + 1;
	pTime[2] = tm_ptr->tm_mday;
	pTime[3] = tm_ptr->tm_hour;
	pTime[4] = tm_ptr->tm_min;
	pTime[5] = tm_ptr->tm_sec;
}

INT32U Get_Cur_UTC_Time(INT8U* pTime)
{
	struct timeval tv;
	struct tm *utc_sec;
	INT32U year_32;
	INT8S val;

	gettimeofday(&tv, NULL); // 1970.1.1(UTC) 이후 현재 sec로 system time 변경.
	utc_sec = gmtime(&tv.tv_sec);

	year_32 = utc_sec->tm_year + 1900;

	val = year_32 - 2000;
	if(val < 0) val = 0;

	disp_time(pTime);

	return tv.tv_sec;
}

#ifdef __USE_RTC__
static INT8U systime[6];
#endif

void User_UTC_Set( INT32U utc_time_tick )
{
	struct timeval tv;
	//struct tm *tz;
	//time_t t;
	INT32S rtn = 0;

	tv.tv_usec = 0;	
	tv.tv_sec = utc_time_tick;	
	
	rtn = settimeofday(&tv, NULL);
	if(rtn != 0)
	{
		D_Printf( DEBUG_SYSTEM, "[RCU] User_UTC_Set Fail %d.\n", utc_time_tick); // DEBUG_SYSTEM, DEBUG_ALL
		D_Printf( DEBUG_LS_Comm, "[RCU] User_UTC_Set Fail %d.\n", utc_time_tick); // DEBUG_SYSTEM, DEBUG_ALL
		return;
	}

	D_Printf( DEBUG_LS_Comm, "[RCU] User_UTC_Set %d.\n", utc_time_tick); // DEBUG_SYSTEM, DEBUG_ALL
	
	#ifdef __USE_RTC__	
	disp_time(systime);
	if(IS___Valid_gREMS_Local_Time(systime))
	{
		system_agent("hwclock --systohc"); // 
		m_Env.IS_RTC_Init = 1;
		m_Env.rtc_refresh_cnt = 0;
	}
	#endif
	
	// GetUptime(); 
	//tz = gmtime(&tv.tv_sec);
	//t = timegm(tz);
	//tz = localtime(&t);

	/*
	local = change_utc_to_local(&lt);
	D_Printf(DEBUG_SYSTEM, "local time : %04d-%02d-%02d %02d:%02d:%02d\n",
			local->tm_year + 1900,
			local->tm_mon + 1,
			local->tm_mday,
			local->tm_hour,
			local->tm_min,
			local->tm_sec);	
	*/
}

/*
void GetUptime(void)
{
	struct sysinfo si;
	int re;	

	re = sysinfo(&si);
	if(re)
    {
		D_Printf(DEBUG_SYSTEM, "[dau] GetUptime(sysinfo) failed.\n");
        return;
    }	
}
*/
struct tm *change_utc_to_local(struct tm* p_tm)
{
	time_t t;
	struct tm *tz;

	t = timegm(p_tm);
	tz = localtime(&t);

	return tz;
}


INT8U IS___Valid_gREMS_Local_Time(INT8U* pTime)
{	
#ifdef	_TEST____ALM_LOG_
#else
	if(pTime[0] < 19) return 0;
	if((pTime[1] < 1)||(pTime[1] > 12)) return 0;
	if(pTime[2] > 31) return 0;
	if(pTime[3] > 23) return 0;
	if(pTime[4] > 59) return 0;
	if(pTime[5] > 59) return 0;
#endif
	return 1; // valid
}

int Change_Local_Time(INT8U* pTime)
{
	time_t m_time;
	struct tm tm_ptr; 

	// AID time -> utc
	if(IS___Valid_gREMS_Local_Time(pTime))
	{	
		tm_ptr.tm_year = pTime[0] + 2000 - 1900;
		tm_ptr.tm_mon  = pTime[1] - 1;
		tm_ptr.tm_mday = pTime[2];
		tm_ptr.tm_hour = pTime[3];
		tm_ptr.tm_min  = pTime[4];
		tm_ptr.tm_sec  = pTime[5];
		tm_ptr.tm_isdst = 0;

		m_time = mktime(&tm_ptr);
	    stime(&m_time);

	    #ifdef __USE_RTC__
		system_agent("hwclock --systohc");
		m_Env.IS_RTC_Init = 1;
		m_Env.rtc_refresh_cnt = 0;
		#endif
		
		print_agent (DEBUG_REMS, VT_BOLD_YELLOW, "LocalTime Response, hw clock set.");	
		print_agent (DEBUG_REMS, VT_BOLD_YELLOW, "Time : %04d-%02d-%02d %02d:%02d:%02d\n",
			tm_ptr.tm_year + 1900,
			tm_ptr.tm_mon + 1,
			tm_ptr.tm_mday,
			tm_ptr.tm_hour,
			tm_ptr.tm_min,
			tm_ptr.tm_sec);	
			
		system_agent("hwclock -r;date");
	    return 1;
	}
	else
		return 0;
}






/*
time_t rawtime; 
struct tm* local_timeinfo; 
struct tm* gm_timeinfo; 
time(&rawtime); 
local_timeinfo = localtime(&rawtime); 
printf("local time and date : %s\n", asctime(local_timeinfo)); 
gm_timeinfo = gmtime(&rawtime); 
printf("gm time and date : %s\n", asctime(gm_timeinfo));
*/



/*
-----------------------------------------------------
	Delay (1[ns], 0.01[us]=10[ns])
	Conversion
-----------------------------------------------------	
*/

// 1[ns] GUI -> 0.01[us] PAU
// 0.01[us] is 10[ns]
INT16U conv_delay_1ns_to_001us(INT32U delay_x1n)
{
	INT32U delay_x10n;

	delay_x10n = delay_x1n/10;	

	if(delay_x10n > 10000) 
		delay_x10n = 10000; // 100.00us
	
	return (INT16U)delay_x10n;
}

// 0.01[us](=10n PAU -> 1[ns] GUI
// 0.01[us] is 10[ns]
INT32U conv_delay_001us_to_1ns(INT16U delay_x10n)
{
	INT32U delay_x1n;

	delay_x1n = delay_x10n*10;
	
	if(delay_x1n > 100000) 
		delay_x1n = 100000; // 100000ns = 100us
	
	return delay_x1n;
}


/*
-----------------------------------------------------
	ATTEN (0.01dB, 0.5dB, 0.25dB)
	Conversion
-----------------------------------------------------	
*/
INT8U conv_att_001_to_05(INT16U att_x100)
{
	INT16U att_x2;

	att_x2 = att_x100/50;
	
	if((att_x2 == 0)&&(att_x100 > 0))
		return 1; // 0.5dB
	
	if(att_x2 > 63) 
		att_x2 = 63; // 31.5dB
	
	return (INT8U)att_x2;
}

INT8U conv_att_01_to_05(INT16U att_x10)
{
	INT16U att_x2;

	att_x2 = att_x10/5;
	
	if((att_x2 == 0)&&(att_x10 > 0))
		return 1; // 0.5dB
	
	if(att_x2 > 63) 
		att_x2 = 63; // 31.5dB
	
	return (INT8U)att_x2;
}


INT8S conv_signed_att_01_to_05(INT16S att_x10)
{
	INT16S att_x2;

	att_x2 = att_x10/5;
	
	if((att_x2 == 0)&&(att_x10 > 0))
		return 1; // 0.5dB
	
	if(att_x2 > 63) 
		att_x2 = 63; // 31.5dB
	
	return (INT8S)att_x2;
}

//-----------------------------------------------------
INT16U conv_att_001_to_025(INT16U att_x100)
{
	INT16U att_x4; // 0.25dB

	att_x4 = att_x100/25;
	
	if((att_x4 == 0)&&(att_x100 > 0))
		return 1; // 0.25dB
	
	if(att_x4 > 126) 
		att_x4 = 126; // '126/4' is 31.5dB
	
	return att_x4;
}
//-----------------------------------------------------
//-----------------------------------------------------
INT8U conv_att_025_to_05(INT16U att_x4)
{
	INT16U att_x2;

	att_x2 = att_x4/2;
	
	if((att_x2 == 0)&&(att_x4 > 0))
		return 1; // 0.5dB
	
	if(att_x2 > 63) 
		att_x2 = 63; // 31.5dB
	
	return (INT8U)att_x2;
}
//-----------------------------------------------------
INT16U conv_att_05_to_001(INT8U att_x2)
{
	INT16U att_x100;

	att_x100 = att_x2*50;

	if(att_x100 > 3150) att_x100 = 3150; // 31.5dB	
	return att_x100;
}
//-----------------------------------------------------
INT16U conv_att_05_to_01(INT8U att_x2)
{
	INT16U att_x10;

	att_x10 = att_x2*5;

	if(att_x10 > 315) att_x10 = 315; // 31.5dB	
	return att_x10;
}
//-----------------------------------------------------
INT16U conv_att_025_to_001(INT16U att_x4)
{
	INT16U att_x100;

	att_x100 = att_x4*25;
	
	if(att_x100 > 3150) att_x100 = 3150; // 31.5dB	
	return att_x100;
}
//-----------------------------------------------------
INT16U conv_att_05_to_025(INT8U att_x2)
{
	INT16U att_x4;

	att_x4 = att_x2*2;	
	
	if(att_x4 > 126) att_x4 = 126; // '126/4' is 31.5dB	
	return att_x4;
}
//-----------------------------------------------------
INT16S conv_signed_att_05_to_025(INT8S att_x2)
{
	INT16S att_x4;

	att_x4 = att_x2*2;	
	
	if(att_x4 > 126) att_x4 = 126; // '126/4' is 31.5dB	
	return att_x4;
}

//-----------------------------------------------------

void Init_Debug_Path4(void)
{
	m_Env.debug_path4 = 0xFF;
	m_Env.debug_optic = 0xFF;
}

INT8U Is_Debug_Path4(INT8U path4)
{
	if(m_Env.debug_path4 == path4)
		return 1;
	else 
		return 0;
}

INT8U Is_Debug_Optic_Path(INT8U optic, INT8U path4)
{
	     if(m_Env.debug_optic != optic) return 0; 
	else if(m_Env.debug_path4 != path4) return 0;
	else return 1;
}



/*----------------------------------------------------------*/
//
/*----------------------------------------------------------*/
#include <sys/types.h>
#include <unistd.h>

void Chk_Same_Process(void)
{
	char pstr[10] = "RCU_App_";
	FILE *fp = NULL;
	char cmd[20];
	memset(cmd, 0x00, 20);

	int self_pid = getpid();
	int i, rtn, cnt;
	char pid_str[10][10];
	int  pid[10];	
	
	memset(pid_str, 0, 10*10);

	//sprintf(cmd, "pgrep -x %s", pstr);
	sprintf(cmd, "pgrep %s", pstr);
	fp = popen(cmd, "r");

	for(cnt = 0, i=0; i<10; i++)
	{
		if(fscanf(fp, "%s", pid_str[cnt++]) < 0) 
			break;
	}

	if(cnt > 0)
	{
		printf("[RCU] Chk Same Process, self_pid is %d\n", self_pid);	
		for(i=0; i<cnt; i++)
		{
			pid[i] = atoi(pid_str[i]);
			if((pid[i] > 0)&&(pid[i] != self_pid))
			{
				rtn = kill(pid[i], SIGTERM);				

				if(rtn >= 0)	
				{					
					OSTimeDly(3*OS_DELAY_1sec);
					printf("[RCU] Same Process %d is Terminated.\n", pid[i]);	
				}
			}
		}
	}	
	pclose(fp);
}



/*
SKT_5G-PRU:[sys]# cat /proc/meminfo | grep Mem
MemTotal:        1018892 kB
MemFree:          976080 kB
MemAvailable:     946080 kB
*/
#define MAX_LEN_LINE (100)
INT16U Get_Mem_Usage(void)
{
	FILE *fp = NULL;
	char cmd[50];
	memset(cmd, 0x00, 50);
	int size[2];

	int i, cnt;
	char size_str[2][MAX_LEN_LINE];
	INT16U usage_x10 = 0;
	
	memset(size_str, 0, 2*MAX_LEN_LINE);
	
	sprintf(cmd, "cat /proc/meminfo | grep Mem");
	fp = popen(cmd, "r");

	if(fp == NULL) return 0; // 2019.11.21
	
	for(cnt = 0, i=0; i<2; i++)
	{
		if(fscanf(fp, "%*s %s %*s", size_str[cnt]) < 0) 
			break;
		cnt++;
	}
	if(cnt > 0)
	{
		size[0] = atoi(size_str[0]);
		size[1] = atoi(size_str[1]);
		
		usage_x10 = (size[0]-size[1])*1000/size[0];
		//printf("[RCU] Mem_Usage, %d - %d => %d/10\n", size[0], size[1], usage_x10);
	}	
	pclose(fp);

	return usage_x10;
}


/*
---------------------------------------------------------------

rc = system("timeout -t 2 ping -c 1 192.168.1.151");
    # rc == 0 : success 
    # rc != 0 : fail


---------------------------------------------------------------
*/


#define PING_TEST_FILE "/tmp/ping_test"
#define PING__PID_FILE "/tmp/ping__pid"
#define PING____RESULT "/tmp/ping_result"




void Proc___Ping_Test(void)
{
	static INT8U init = 0;
	static INT8U is_test_on = 0; 
	INT8U  re_system;
	INT8U  re_cmd;
	char result[255], result_f = 0;

	memset(result, 0, sizeof(result));		

	if(init == 0)
	{
		init = 1;

		memset(&m_ping_test, 0, sizeof(m_ping_test));
		m_ping_test.on = 0; // '0' is OFF
		result[0] = '-';
		result_f = 1;

		is_test_on = 0; // off		

		m_ping_test.ip[0] = 192;
		m_ping_test.ip[1] = 168;
		m_ping_test.ip[2] = 0;
		m_ping_test.ip[3] = 100;
	}
	//-----------------------------------------	
	if(m_ping_test.cnt16 == 0) m_ping_test.cnt16 = 3;
	
	if(m_ping_test.size16 > 65000) m_ping_test.size16 = 65000;
	if(m_ping_test.size16 == 0)    m_ping_test.size16 = 56;	
	//-----------------------------------------	

	int rc;
	char cmd[256];

	
	
	if(m_ping_test.on == 0) // off(==stop), from 1(on) or 2(on going)
	{
		if(is_test_on == 2) // in progress
		{
			system("kill -2 `pidof ping`"); // '2' is 'SIGINT'
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[ping test] stop request..");	
		}		
	}
	else if(m_ping_test.on == 1) // start requested
	{		
		if(m_ping_test.ip[0] != 192)
		{
			sprintf(cmd, "[ping test] ip %d.%d.%d.%d error, test fail", 
						m_ping_test.ip[0], m_ping_test.ip[1], m_ping_test.ip[2], m_ping_test.ip[3]);
						
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "%s", cmd);	
			strcpy(result, cmd);
			result_f = 1;
			
			m_ping_test.on = 0; // '0' off			
			is_test_on = 0;
		}
		else if(is_test_on == 0) // not yet start
		{
			sprintf(cmd, "touch %s %s %s", PING_TEST_FILE, PING__PID_FILE, PING____RESULT);
			rc = system_agent(cmd);			

			re_system = rc & 0xff;
			re_cmd = (rc >> 8) & 0xff;

			if(re_cmd == 0)			
			{				
				///////sprintf(cmd, "timeout -t 100 ping -c %d -s %d %d.%d.%d.%d", 
				sprintf(cmd, "ping -c %d -s %d %d.%d.%d.%d > %s 2>&1 &", 
								m_ping_test.cnt16, 
								m_ping_test.size16,
								m_ping_test.ip[0], m_ping_test.ip[1], m_ping_test.ip[2], m_ping_test.ip[3],
								PING_TEST_FILE);				
				system_agent(cmd);		
				
				OSTimeDly( OS_DELAY_1sec );				
				
				m_ping_test.on = 2; // on going
				is_test_on = 2;
				print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[ping test] in progress..");
				strcpy(result, "in progress");
				result_f = 1;
			}
			else
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "%s", cmd);	
				strcpy(result, cmd);
				result_f = 1;

				m_ping_test.on = 0; // '0' off
				is_test_on = 0;				
			}			
		}		
	}
	//--------------------------------------------------------
	if(is_test_on == 2) // on going	
	{
		/*
		sprintf(cmd, "pidof ping > %s", PING__PID_FILE);
		rc = system(cmd);
		*/
		OSTimeDly( OS_DELAY_1sec );
		
		rc = system("pidof ping");		
		re_system = rc & 0xff;
		re_cmd = (rc >> 8) & 0xff;
				
		if(re_system == 0)
		{
			if(re_cmd == 0)
			{
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[ping test] in progress, pid of ping exist, %d", rc);
			}
			else
			{				
				m_ping_test.on = 0; // '0' off
				is_test_on = 0;

				char buf[MAXBUF];
				int exit_code;

				memset(buf, 0, sizeof(buf));

				sprintf(cmd, "tail -n 3 %s > %s", PING_TEST_FILE, PING____RESULT);
				system_agent(cmd);
				OSTimeDly( OS_DELAY_1sec );

				exit_code = popen_agent(buf, MAXBUF, "cat %s", PING____RESULT);
				OSTimeDly( OS_DELAY_500msec );
								
				if(exit_code >= 0)			
				{
					print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[ping test] end, size(%d/%d), exit_code %#x.\n[result]\n%s", 
								strlen(buf), sizeof(buf), exit_code, buf);
					
					if(strlen(buf) < sizeof(buf))
					{
						memcpy(result, buf, strlen(buf));
						result_f = 1;
					}	
				}				
			}
		}
		else
		{
			m_ping_test.on = 0; // '0' off
			is_test_on = 0;

			sprintf(cmd, "[ping test] fail of system(pidof ping), %#x", rc);
			print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "%s", cmd);
			strcpy(result, cmd);
			result_f = 1;
		}
	}
	if(result_f)
	{
		memcpy(m_ping_test.result, result, sizeof(m_ping_test.result));
		////print_agent(DEBUG_SYSTEM, VT_BOLD_GREEN, "[0xC0A4, 1] %s, %d", m_ping_test.result, strlen(m_ping_test.result));
	}
	//--------------------------------------------------------
}


////#define _USE_CPU_AVR_
#define USG_AVR_MAX 3
void* CPU_Usage_Task(void *data)
{	
	FILE *fp;
	int a[4], b[4];
	long long load;
	long long aa1, bb1;
	long long aa2, bb2;
	INT16U CPU_Usage;
	INT16U Usage_prev = 0;
	INT16U Usage_Max = 0;
	
	Thread_Priority_Init((Thread_ARG_Var*)data, 1);
	print_agent(DEBUG_LOG, VT_BOLD_WHITE, "CPU_Usage_Task");
		
	
	OSTimeDly( 5*OS_DELAY_1sec );

	
	fp = fopen("/proc/stat","r");
	if(fp != NULL) // 2019.11.21
	{
		fscanf(fp,"%*s %d %d %d %d", &a[0], &a[1], &a[2], &a[3]);
		fclose(fp);
	}

	//memset( avr, 0, USG_AVR_MAX);
	
	while( m_Env.task_flag[_TASK_CPU_USAGE_] )		///// YCSO TASK END
	{		
		sleep(2); // sleep 2[s]
		Update__Task_Count(_TASK_CPU_USAGE_);

		//---------------------
		// 2020.06.09
		Proc___Ping_Test();
		//---------------------
		
		fp = fopen("/proc/stat","r");
		if(fp == NULL) continue; // 2019.11.21
		fscanf(fp,"%*s %d %d %d %d", &b[0], &b[1], &b[2], &b[3]);
		fclose(fp);

		aa1 = a[0] + a[1] + a[2];
		bb1 = b[0] + b[1] + b[2];
		
		aa2 = aa1 + a[3];
		bb2 = bb1 + b[3];

		//D_Printf( DEBUG_SYSTEM, "%d + %d + %d + %d = %d\r\n", b[0], b[1], b[2], b[3], bb2);
		//D_Printf( DEBUG_SYSTEM, "%d + %d + %d + %d = %d\r\n", a[0], a[1], a[2], a[3], aa2);
		//D_Printf( DEBUG_SYSTEM, "%d - %d \r\n", (bb1 - aa1), (bb2 - aa2));

		/*
		-- IRO
		load = ((bb1 - aa1) * 100000) / (bb2 - aa2);
//		idle = 100000 - load;

		if(load < 1000)
			CPU_Usage = 1;
		else 
			CPU_Usage = load/1000;
		*/
		
		load = ((bb1 - aa1) * 100000) / (bb2 - aa2);
		//idle = 100000 - load;

		if(load < 1000)
			CPU_Usage = 1;
		else 
			CPU_Usage = load/100;

		//D_Printf( DEBUG_SYSTEM, "[CPU] %lld.%03d [IDLE] %lld.%03d\r\n\r\n", CPU_Usage, load%1000, idle/1000, idle%1000);
				
		a[0] = b[0];
		a[1] = b[1];
		a[2] = b[2];
		a[3] = b[3];

		m_St.CPU_Usage = CPU_Usage;
		m_St.Mem_Usage = Get_Mem_Usage();	

		//-----------------------------------
		if(Usage_Max < m_St.CPU_Usage)
		{
			Usage_Max = m_St.CPU_Usage;
			if(Usage_Max > 500)
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "Max CPU_Usage %d", Usage_Max/10);
		}
		if(Usage_prev < m_St.CPU_Usage)
		{
			if(m_St.CPU_Usage > 500)
				print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "CPU_Usage %d -> %d", 
						Usage_prev/10, m_St.CPU_Usage/10);
		}
		Usage_prev = m_St.CPU_Usage;
		//-----------------------------------
	}
	
	m_Env.task_END_status[_TASK_CPU_USAGE_] = _CLR_;		///// YCSO TASK END

	pthread_exit(NULL);
}


void Disp_IP_addr(INT8U* rx_ip, char* str)
{
	struct sockaddr_in addr;	
	addr.sin_addr.s_addr = *(INT32U*)rx_ip;
	
	D_Printf(DEBUG_SYSTEM, "[RCU] %s ip is %s\n", 
		str,
		inet_ntoa(addr.sin_addr));
}


/*
	INT8U ip[4] = {1,2,3,4};
	Make_IP_addr_str(ip, str);
	D_Printf(DEBUG_SYSTEM, "[RCU] test ip {%s}\n", str);
*/

void Make_IP_addr_str(INT8U* rx_ip, char* str)
{
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = *(INT32U*)rx_ip;	
	sprintf(str, "%s", inet_ntoa(addr.sin_addr));
}





/*----------------------------------------------------------*/


#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/////////////////////////////////////////////////////////////////

static int sock_fd = -1; // init

void Close_System_Sock_fd(void)
{
	if(sock_fd > 0) 
		close(sock_fd);
}

int Proc__Get_System_MAC_IP(void)
{
	struct ifreq ifr;

	if(sock_fd < 0)
	{
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if(sock_fd < 0)
		{
			sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
			perror("socket fail");
			D_Printf(DEBUG_SYSTEM, "[Get_system_mac_ip] socket fail\n"); 
			return -1;
		}
	}
	//----------------------------------------
	else
	{		
		strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
		if(ioctl(sock_fd, SIOCGIFHWADDR, &ifr) < 0)
		{
			perror("[Get_system_mac] ioctl fail");
			D_Printf(DEBUG_SYSTEM, "[Get_system_mac] ioctl fail\n"); 
			return -1;
		}
		memcpy(m_St.Operate_Mac, ifr.ifr_hwaddr.sa_data, 6);
		
		//printf("%02X:%02X:%02X:%02X:%02X:%02X\n", s_mac[0], s_mac[1], s_mac[2], s_mac[3], s_mac[4], s_mac[5]);
		//----------------------------------------
		if(ioctl(sock_fd, SIOCGIFADDR, &ifr) < 0)
		{
			perror("[Get_system_ip] ioctl fail2");
			D_Printf(DEBUG_SYSTEM, "[Get_system__ip] ioctl fail2\n"); 
			return -1;
		}
		
		struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
		//printf("IP address: %s, 0x%X\n",inet_ntoa(ipaddr->sin_addr), ipaddr->sin_addr.s_addr);
	
		m_St.Operate_Ip32 = (INT32U)ipaddr->sin_addr.s_addr;
		//printf("IP address: %d.%d.%d.%d\n", s_ip[0],s_ip[1],s_ip[2],s_ip[3]);		
	}
	
	return 0;
}



/*
-----------------------------------------------------------------
PRU :
----------------
eMMC의 6번 partition 을 additional information 을 저장하는 영역으로 정의, 
이 영역을 이용하여 mac file을 저장
 
# setup_mac_5g_repeater_info.sh SAVED_MAC_ADDRESS 
 

AAU : 
----------------
Chip에 Fusing 되어 있는 MAC 주소를 읽는 기능
    onchip_eth0_macid.sh
    onchip_eth1_macid.sh
 
# setup_mac_5g_repeater_info.sh `onchip_eth0_macid.sh`
(참고로 ` 는 reverse single quotation(or back quote) mark 입니다.)


setup_mac_5g_repeater_info.sh 12:34:56:78:AB:CD
setup_mac_5g_repeater_info.sh 50:5B:C2:E1:C0:25


-----------------------------------------------------------------
*/


int Set_System_HW_Mac(INT8U* Mac) // PRU Only
{
	int re;
	
	re = system_agent("info_slice_mac.sh %02X:%02X:%02X:%02X:%02X:%02X", 
				Mac[0], Mac[1], Mac[2],	Mac[3], Mac[4], Mac[5]);
			
	if(re != 0) // if fail
	{
		re = system_agent("setup_mac_5g_repeater_info.sh %02X:%02X:%02X:%02X:%02X:%02X", 
					Mac[0], Mac[1], Mac[2],	Mac[3], Mac[4], Mac[5]);	
	}

	return re;
}

#define Fused_MAC_Path "/tmp/app/MAC_5G_REPEATER.ini"

int Get_System_HW_Mac(INT8U* Mac) // ANTS Only
{
	FILE *configfp;
	INT8U i;
	int re;
	int mac[6];	

	configfp = fopen(Fused_MAC_Path, "r");

	if(configfp == NULL)
	{	
		#ifdef __TYPE__PRU__ // ANTS_AAU Only
		print_agent(DEBUG_SYSTEM, VT_RED, "Read MAC (MAC_5G_REPEATER) Fail");
		re = -1;
		#else // ANTS_AAU Only
		print_agent(DEBUG_SYSTEM, VT_RED, "Read Fused-MAC (MAC_5G_REPEATER) Fail, Retry");
		system_agent("setup_mac_5g_repeater_info.sh `onchip_eth0_macid.sh`");
		sync();
		OSTimeDly(3*OS_DELAY_1sec);
		
		configfp = fopen(Fused_MAC_Path, "r");		
		if(configfp == NULL)
		{		
			print_agent(DEBUG_SYSTEM, VT_RED, "Read Fused-MAC (MAC_5G_REPEATER) Fail");
			re = -1;
		}		
		#endif
	}
	else
	{
		// 50:5B:C2:E1:C0:25
		if(fscanf(configfp, "%X:%X:%X:%X:%X:%X",
					&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) > 0)
		{
			print_agent(DEBUG_SYSTEM, VT_CYAN, "Read MAC (MAC_5G_REPEATER) %02X:%02X:%02X:%02X:%02X:%02X", 
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

			for(i=0; i<6; i++) Mac[i] = mac[i];
			re = 0;
		}
		else re = -1;
		
		fclose(configfp);
		
		return 0;
	}
	return re;
}



#include <net/if_arp.h>
/*
int Set_system_MAC(INT8U* s_mac)
{
	struct ifreq ifr;
	INT8U idx = 100;

	#ifdef __Use_ifconfig__
	Net_Set_Info_Set(_MAC0_, s_mac);
	return 0;
	#endif	

	if(sock_fd < 0)
	{
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if(sock_fd < 0)
		{
			sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
			perror("socket fail");
			D_Printf(DEBUG_SYSTEM, "[Set_system_mac] socket fail\n"); 
			
			printf(VT_BOLD_RED "[Set_system_mac] socket fail\n" VT_END_CR);
			return -1;
		}
	}
	//----------------------------------------
	
	{		
		strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
		memcpy(ifr.ifr_hwaddr.sa_data, s_mac, 6);
		
		D_Printf(DEBUG_SYSTEM, "[Set_system_MAC] Req : %02X:%02X:%02X:%02X:%02X:%02X",
		        ifr.ifr_hwaddr.sa_data[0],
		        ifr.ifr_hwaddr.sa_data[1],
		        ifr.ifr_hwaddr.sa_data[2],
		        ifr.ifr_hwaddr.sa_data[3],
		        ifr.ifr_hwaddr.sa_data[4],
		        ifr.ifr_hwaddr.sa_data[5]);	

		printf(VT_GREEN "\n\n[Set_system_MAC] Req : %02X:%02X:%02X:%02X:%02X:%02X" VT_END_CR,
		        ifr.ifr_hwaddr.sa_data[0],
		        ifr.ifr_hwaddr.sa_data[1],
		        ifr.ifr_hwaddr.sa_data[2],
		        ifr.ifr_hwaddr.sa_data[3],
		        ifr.ifr_hwaddr.sa_data[4],
		        ifr.ifr_hwaddr.sa_data[5]);        	
		
		system( NETWORK_STOP );
		printf("[RCU] Net Stop(#%d).\n", idx );		
		OSTimeDly(OS_DELAY_500msec);
		
		

		ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;		
		if(ioctl(sock_fd, SIOCSIFHWADDR, &ifr) < 0)
		{
			perror("[Set_system_mac]ioctl fail 1");
			D_Printf(DEBUG_SYSTEM, "[Set_system_mac] ioctl fail 1\n"); 
			printf(VT_BOLD_RED "[Set_system_mac] ioctl fail 1\n" VT_END_CR);

			system( NETWORK_START );
			printf( "[RCU] Net Start(#%d).\n", idx );
			return -1;
		}
		
		memcpy(m_St.Operate_Mac, ifr.ifr_hwaddr.sa_data, 6);	
		if(ioctl(sock_fd, SIOCGIFHWADDR, &ifr) < 0)
		{
			perror("[Set_system_mac]ioctl fail2");
			D_Printf(DEBUG_SYSTEM, "[Set_system_mac] ioctl fail 2\n"); 
			printf(VT_BOLD_RED "[Set_system_mac] ioctl fail 2\n" VT_END_CR);

			system( NETWORK_START );
			printf( "[RCU] Net Start(#%d).\n", idx );
			return -1;
		}
		memcpy(m_St.Operate_Mac, ifr.ifr_hwaddr.sa_data, 6);

		D_Printf(DEBUG_SYSTEM, "[Get_Sys_Mac] Success : %02X:%02X:%02X:%02X:%02X:%02X",
		        m_St.Operate_Mac[0],
		        m_St.Operate_Mac[1],
		        m_St.Operate_Mac[2],
		        m_St.Operate_Mac[3],
		        m_St.Operate_Mac[4],
		        m_St.Operate_Mac[5]);	

		printf(VT_GREEN "\n\n[Get_Sys_Mac] Success : %02X:%02X:%02X:%02X:%02X:%02X" VT_END_CR,
		        m_St.Operate_Mac[0],
		        m_St.Operate_Mac[1],
		        m_St.Operate_Mac[2],
		        m_St.Operate_Mac[3],
		        m_St.Operate_Mac[4],
		        m_St.Operate_Mac[5]);                
	}	
	
	system( NETWORK_START );
	printf( "[RCU] Net Start(#%d).\n", idx );
	return 0;
}
*/


#include <stddef.h>
#define ifreq_offsetof(x)  offsetof(struct ifreq, x)
/*
int Set_system_IP(INT8U* s_ip)
{
	struct ifreq ifr;	
	char ip_char[50];
	INT8U idx = 101;


	#ifdef __Use_ifconfig__
	Net_Set_Info_Set( _IPADDR0_, s_ip );
	return 0;
	#endif

	if(sock_fd < 0)
	{
		// Create a channel to the NET kernel.
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if(sock_fd < 0)
		{
			sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
			perror("socket fail");
			D_Printf(DEBUG_SYSTEM, "[Set_system_ip] socket fail\n"); 
			
			syslog( LOG_DEBUG, VT_BOLD_RED "[Set_system_IP] socket fail\n" VT_END_CR);
			return -1;
		}
	}
	
	D_Printf(DEBUG_SYSTEM, "[Set_system_IP] Req : %d.%d.%d.%d\n",
			s_ip[0], s_ip[1], s_ip[2], s_ip[3]);	
	printf(VT_GREEN "[Set_system_IP] Req : %d.%d.%d.%d\n" VT_END_CR,
			s_ip[0], s_ip[1], s_ip[2], s_ip[3]);	
	
	// get interface name
	//strncpy(ifr.ifr_name, IFNAME, IFNAMSIZ);
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
	//----------------------------------------	
	system( NETWORK_STOP );
	printf( "[RCU] Net Stop(#%d).\n", idx );
	OSTimeDly(OS_DELAY_500msec);	

	

	////in_addr_t in_addr;
	////in_addr = inet_addr("192.168.101.17");	
	////sai.sin_addr.s_addr = in_addr;
	////sai.sin_addr.s_addr = s_ip; 
	
	struct sockaddr_in sai;	
	char *p;

	memset(&sai, 0, sizeof(struct sockaddr));
	sai.sin_family = AF_INET;
	sai.sin_port = 0;		
	sprintf(ip_char, "%d.%d.%d.%d", s_ip[0], s_ip[1], s_ip[2], s_ip[3]);
	sai.sin_addr.s_addr = inet_addr(ip_char);
	p = (char *) &sai;
	memcpy((((char *)&ifr + ifreq_offsetof(ifr_addr))), p, sizeof(struct sockaddr));
	
	//----------------------------------------
	if(ioctl(sock_fd, SIOCSIFADDR, &ifr) < 0) // xx
	{
		perror("Set_system_IP");
		D_Printf(DEBUG_SYSTEM, "[Set_system_IP] ioctl fail 1\n"); 
		syslog( LOG_DEBUG, VT_BOLD_RED "[Set_system_IP] ioctl fail 1\n" VT_END_CR);

		system( NETWORK_START );
		printf( "[RCU] Net Start(#%d).\n", idx );
		return -1;
	}		

	if(ioctl(sock_fd, SIOCGIFADDR, &ifr) < 0)
	{
		perror("[Set_system_IP]");
		D_Printf(DEBUG_SYSTEM, "[Set_system_IP] ioctl fail 2\n"); 
		syslog( LOG_DEBUG, VT_BOLD_RED "[Set_system_IP] ioctl fail 2\n" VT_END_CR);

		system( NETWORK_START );
		printf( "[RCU] Net Start(#%d).\n", idx );
		return -1;
	}
	
	struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	m_St.Operate_Ip32 = (INT32U)ipaddr->sin_addr.s_addr;	
	//----------------------------------------	
	D_Printf(DEBUG_SYSTEM, "[Get_Sys_IP] Success : %d.%d.%d.%d\n",
	        m_St.Operate_Ip[0],
	        m_St.Operate_Ip[1],
	        m_St.Operate_Ip[2],
	        m_St.Operate_Ip[3]);	

	printf(VT_GREEN "[Get_Sys_IP] Success : %d.%d.%d.%d\n" VT_END_CR,
	        m_St.Operate_Ip[0],
	        m_St.Operate_Ip[1],
	        m_St.Operate_Ip[2],
	        m_St.Operate_Ip[3]);

	//----------------------------------------	
	ioctl(sock_fd, SIOCGIFFLAGS, &ifr); 
	ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
	ioctl(sock_fd, SIOCSIFFLAGS, &ifr);	

	//system( NETWORK_START );
	printf( "[RCU] Net Start(#%d).\n", idx );	
	//----------------------------------------	
	
	return 0;
}
*/


int Get_system_mac_ip(INT8U* s_mac, INT8U* s_ip)
{
	struct ifreq ifr;
	int fd;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if(fd < 0)
	{
		perror("Get_system_mac_ip fail"); // perror("socket fail");
		D_Printf(DEBUG_SYSTEM, "[Get_system_mac_ip] socket fail\n"); 
		return -1;
	}
	//----------------------------------------
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
	if(ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		perror("Get_system_mac_ip fail"); // perror("ioctl fail");
		D_Printf(DEBUG_SYSTEM, "[Get_system_mac_ip] ioctl fail\n"); 
		return -1;
	}
	memcpy(s_mac, ifr.ifr_hwaddr.sa_data, 6);
	
	//printf("%02X:%02X:%02X:%02X:%02X:%02X\n", s_mac[0], s_mac[1], s_mac[2], s_mac[3], s_mac[4], s_mac[5]);
	//----------------------------------------
	if(ioctl(fd, SIOCGIFADDR, &ifr) < 0)
	{
		perror("Get_system_mac_ip fail"); // perror("ioctl fail");
		D_Printf(DEBUG_SYSTEM, "[Get_system_mac_ip] ioctl fail2\n"); 
		return -1;
	}
	
	//struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	struct sockaddr_in *ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;

	//printf("IP address: %s, 0x%X\n",inet_ntoa(ipaddr->sin_addr), ipaddr->sin_addr.s_addr);

	*(INT32U*)s_ip = (INT32U)ipaddr->sin_addr.s_addr;
	
	//printf("IP address: %d.%d.%d.%d\n", s_ip[0],s_ip[1],s_ip[2],s_ip[3]);
		
	close(fd);
	
	return 0;
}


#include <sys/sysinfo.h>
INT32U GetUptime(void)
{
	struct sysinfo si;
	int re;	

	re = sysinfo(&si);
	if(re)
    {
    	perror("[RCU] GetUptime(sysinfo)");
		D_Printf(DEBUG_SYSTEM, "[RCU] GetUptime(sysinfo) failed.\n");
        return 0;
    }
	return si.uptime;
}



/*----------------------------------------------------------
App Timer
----------------------------------------------------------*/
void Init__App_Timer(APP_TIMER* pApp_Timer)
{
	pApp_Timer->start = 0;
	pApp_Timer->end = 0;
	pApp_Timer->enable = 0;	
	OSTimerReset(&pApp_Timer->timer_ms);
}

void Start__App_Timer(APP_TIMER* pApp_Timer, INT32U timeout_ms)
{
	pApp_Timer->start = 1;
	pApp_Timer->end = 0;
	pApp_Timer->enable = 1;
	
	OSTimerReset(&pApp_Timer->timer_ms);
	pApp_Timer->timeout_ms = timeout_ms;	
}

INT8U IsEnd__App_Timer(APP_TIMER* pApp_Timer)
{		
	if(pApp_Timer->start)
	{	
		if( OSTimerGet_msec( &pApp_Timer->timer_ms ) >= pApp_Timer->timeout_ms )	
		{
			pApp_Timer->start = 0;
			pApp_Timer->end = 1;
			OSTimerReset(&pApp_Timer->timer_ms);
		}
	}
	
	if(pApp_Timer->enable && pApp_Timer->end)
	{
		pApp_Timer->enable = 0;
		pApp_Timer->end = 0;
			
		return 1;
	}
	else return 0;
}






/*
//--------------------------------------------------------
// 2020.07.15 : DDR-Memory Clock info AID(0xC0A5) Added
//--------------------------------------------------------
*/
#define PROC_CMDLINE_FILE "/proc/cmdline"
#include <string.h>

void Get__PRU_DDR_Memory_Clk_Info(void)
{	
#ifdef __TYPE__PRU__ // PRU Only
	char info[20];
	char buf[256];
	char *p_chr;
	int clk;
	
	FILE* fp_src = fopen( PROC_CMDLINE_FILE, "r" );

	memset(info, 0, 20);
	
	if(fp_src != NULL)
	{		
		if( fgets( buf, 256, fp_src ) != NULL )
		{
			print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[%s] %s", PROC_CMDLINE_FILE, buf );
			
			p_chr = strstr( buf, "ddr_clk" );
			if( p_chr != NULL )
			{
				memcpy(info, p_chr, 20); // max 20byte
				
				if(sscanf(info, "ddr_clk=%dMhz", &clk) > 0)
				{
					sprintf(info, "ddr_clk=%dMhz", clk);
					print_agent(DEBUG_SYSTEM, VT_BOLD_YELLOW, "[DDR_Memory_Clk] %s", info );
				}
				print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[DDR_Memory_Clk] parsing fail for clk");
			}
			else print_agent(DEBUG_SYSTEM, VT_BOLD_RED, "[DDR_Memory_Clk] finding fail for ddr_clk");
		}
		fclose(fp_src);	
	}
	//-------------------------------------	
	memcpy(m_St.DDR_Mem_Clk_Info, info, 20);
#endif	
}


#if 0
#define SYSTEM_FUSED_MAC_FILE	"/tmp/fused_mac"
void Get_System_Fused_MAC(INT8U* pMAC)
{
	FILE* fp_src;	
	int re = 1; // init fail
	char buf[512];
	char *p_chr;
	char buf_str[10];
	
	fp_src = fopen( SYSTEM_FUSED_MAC_FILE, "r" );
	if( fp_src == NULL ) 
	{
		printf("file %s open failed (%s)\n", INTERFACE_FILE, strerror(errno));
		return 1;
	}

	while(1)
	{
		if( fgets( buf, 128, fp_src ) == NULL )
			break;
		
		if( strstr( buf, keywd ) != NULL )
		{	
			p_chr = strchr(buf, '=');
			if (p_chr)
			{
				p_chr++;

				if(strcmp(keywd, "IPV4DHCP0") == 0)
				{					
					sscanf( p_chr, "%s\n", buf_str );
					
					if(strcmp(buf_str, "yes") == 0)      val[0] = 1;
					else if(strcmp(buf_str, "no") == 0 ) val[0] = 0; // no
					else                                 val[0] = 0xFF; // init
				}				
				else if(strcmp(keywd, "MAC0") == 0 || strcmp(keywd, "MAC1") == 0)
				{
					sscanf(p_chr, "%02X:%02X:%02X:%02X:%02X:%02X\n", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]);
				}				
				else
				{
					sscanf( p_chr, "%d.%d.%d.%d\n", &val[0], &val[1], &val[2], &val[3] );
				}
				re = 0; // success
				break;
			}			
		}
	};
	
	fclose(fp_src);	
	return re;
}
#endif
//
/*----------------------------------------------------------*/


/* EOF */


/*
1. 리눅스 시스템 현재 시간 알아보는 예제(1970년 부터 흐른시간)
#include <time.h> 
#include <stdio.h> 
#include <unistd.h> 
int main() { 
 int i; 
 time_t the_time; 
 time(&the_time); printf("현재시간은 %d\n", the_time);
}

2. 시스템 시간 설정 (stime 함수)
2.1. 사용법
#include <time.h>
int stime(time_t *t);

2.2. 설명
stime 은 시스템의 시간과 날짜를 설정한다. 
지정하는 시간 5는 00:00:00 GMT January 1, 1970 이후로 부터 계산되는 초이다. 
stime() 함수는 단지 root 유저 만이 사용할수 있다. 

2.3. 반환값
성공할경우 0을 실패했을경우에는 -1을 반환하며, 적당한 errno 값을 설정한다. 

2.4. 에러
EPERM: 호출 프로세스의 유저가 슈퍼 사용자가 아니다. 

2.5. 예제
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
int main(int argc, char **argv) {
    // 사용법 : ./stime "2002 2 16 9 9 9" 
    //                   년  월 일 시 분 초
    char buff[6][5];
    struct tm tm_ptr;
    time_t m_time;
    sscanf(argv[1], "%s %s %s %s %s %s", buff[0], buff[1],
                                         buff[2], buff[3],
                                         buff[4], buff[5]);
    tm_ptr.tm_year  = atoi(buff[0]) - 1900;
    tm_ptr.tm_mon   = atoi(buff[1]) - 1;
    tm_ptr.tm_mday  = atoi(buff[2]);
    tm_ptr.tm_hour  = atoi(buff[3]);
    tm_ptr.tm_min   = atoi(buff[4]);
    tm_ptr.tm_sec   = atoi(buff[5]);
    tm_ptr.tm_isdst = 0;
    m_time = mktime(&tm_ptr);
    stime(&m_time);
}
   
위의 프로그램에 시간을 주고 실행시키면 시스템 시간이 변경된다. 
[root@coco test]# ./stime "2018 04 15 19 45 12"

3. 리눅스 시간을 보기 좋은 형식으로 바꾸기 (gmtime 함수)
3.1. 사용법
#include <time.h>
struct tm *gmtime(const time_t *timep);

3.2. 설명
timep 를 입력받아서 시간 값을 돌려준다. 시간 값은 tm 구조체에 들어간다. 
tm 구조체는 다음과 같은 멤버 변수들을 포함한다. 
struct tm {
        int     tm_sec;         
        int     tm_min;         
        int     tm_hour;        
        int     tm_mday;        
        int     tm_mon;         
        int     tm_year;        
        int     tm_wday;        
        int     tm_yday;        
        int     tm_isdst;       
};
   
월은 0부터 시작한다. 그러므로 프로그램상에서 제대로 표현하려면 +1을 해주어야 한다. 
또한 년의 경우에는 +1900 해주어야 한다. 
아규먼트로 들어가는 timeptr 은 1970년 1월 1일 00:00:00 부터의 계산하고픈 때까지의 
시간의 차이를 초로 환산한 값이다. 
gmtime 은 UTC(Universal Time Coordinated) 협정 세계시를 되돌려준다. 
각 지방시는 이것을 기준으로 결정이 된다. 
이 세계시는 그리니치 평균시의 시간체계를 12시간 앞당겨 자정부터 시작하도록 조정되어 있다. 

3.3. 반환값
tm 구조체를 넘겨준다. 

3.4. 예제
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
void swaptime(time_t, char *);
int main() {
    char buffer[255];
    time_t the_time;
    // time 함수를 이용해서 현재 시간을 얻어온다. 
    time(&the_time);
    memset(buffer, 0x00, 255);
    swaptime(the_time, buffer);
    printf("%s\n", buffer);
}

void swaptime(time_t org_time, char *time_str) {
    struct tm *tm_ptr;
    tm_ptr = gmtime(&org_time);
    sprintf(time_str, "%d-%d-%d %d:%d:%d", tm_ptr->tm_year+1900,
                    tm_ptr->tm_mon+1,
                    tm_ptr->tm_mday,
                    tm_ptr->tm_hour,
                    tm_ptr->tm_min,
                    tm_ptr->tm_sec);
}
   
위 프로그램을 컴파일한다음 실행시키면 다음과 같은 결과를 보여줄것이다. 
[root@localhost test]# ./gmtime
2018-4-15 18:18:32

*/


