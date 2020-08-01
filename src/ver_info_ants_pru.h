/*
---------------------------------------------------
	ANTS PRU : version info
	prev 0.0.1
---------------------------------------------------	
*/
//------------------------------------------------------------
#define SW_VER1	0x10 // Internal Ver 1st
#define SW_VER2	0x00 // Internal Ver 2nd

/*------------------------------------------------------------
Hidden 
------------------------------------------------------------*/
#define SW_R_VER1  20 // 1st byte: Release year, 20
#define SW_R_VER2  7  // 2nd byte: Release number
#define SW_R_VER3  16 // 3rd byte: Patch number

/*------------------------------------------------------------
Main Release 
	19.01.04 : 2020.02.20 Now
	19.01.03 : 2020.02.14 Now
------------------------------------------------------------
TEST 
	99.01.03 : 2020.02.14 : gIRO DVT 기능시험 준비 건으로 UDA Test F/W
	99.01.05 : 2020.03.24 : 
------------------------------------------------------------*/
#define SW_Main_VER1 99 // 1st byte: Release year
#define SW_Main_VER2 1  // 2nd byte: Release number (ex) year 2015,1st Release FW. from 1
#define SW_Main_VER3 6  // 3rd byte: Patch number (Patch number in Release ver, from 0)
//------------------------------------------------------------

// H/W watchddg test, after Dip switch off
/*
# killall -1 watchdog
*/

/*
tcpdump -i eth0 -vvv -nn udp dst port 53
udp 속도 개선
tcpdump -i eth0 -vvnnXS port 53
tcpdump -i eth0 -Xq dst port 20013


sysctl -a | grep usb

# sysctl -w  net.core.wmem_max=262144



int a = -10, b = 11;
print_agent( DEBUG_STDBY, VT_RED  , " TEST %+d %+d %+04d %+05d", a, b, a, b);
// <RCU>  TEST -10 +11 -010 +0011




char cmd[128];
snprintf(cmd, sizeof(cmd), "mv %s %s/image.ub",file_name,IMAGE_DIR);
system(cmd);




-----------------------------
delay
-----------
1. L/S 응답용(Read only)
	Optic Delay(RoundTrip) : (AID 0x5405)
	T-Offset : (AID 0xB42B)
	TB-Delay-DL-Hoppong : (AID 0xB42C)
	TB-Delay-UL-Hoppong : (AID 0xB42D)
	
2. delay calculation (read/write)
	T2a(DL), T3a(UL) : (AID 0x5403), from L/S
	offset : RU (digital, AMP) 
	FPGA Delay (AID 0xB429)
-----------------------------

(1) 0xEA Command (PSU Reset) / by Power control
PRU : register 제어 후 "halt" 
AAU : "reboot"

(2) AID 0x5620 / by user control
PRU : register 제어 없이 "reboot" (2020.03.20)
AAU : "reboot"

(3) Download / by download
PRU : register 제어 후 "halt" 
AAU : "reboot"

(4) VSS restart / by VSS
PRU : register 제어 후 "halt" 
AAU : "reboot"

(5) UDA PD(CPRI)/Clock: by UDA
register 제어 후 "halt" (10s 지나도 안될 경우가 있다면 "reboot")
"reboot"

--------------------------------------------------------------

PRU "halt" 의 경우 
[1] 기존 
register(PSU Reset, register 0x33) 을 0x02 data 제어.
0x02 제어되면 "halt" 제어.

[2] 수정 : 2020.03.15
register(PSU Reset, register 0x33) 을 0x02 data 제어.
1초 후, 0x02 상태 값 변경 여부 관계없이 바로 "halt" 제어.


단, 위 [2] 수정 방식으로 모든 경우에 동일하게 적용하되, 
아래 "요청사항" 은 적용하지는 않음.

"요청사항" :  (Reset 수신 후 System이 종료 되지 않는 경우 timer 30초 경과 시 Halt 동작으로 System Reboot 진행


*/

/*
1. ftpget / ftpput
    - anonymous 로 login 하여 ftp 로 get 하는 부분과 put 하는 부분으로 되어 있습니다.
    
3. unzip
    - linux에서 zip file을 푸는 program이 "unzip" 인데, 이것을 이용하여 원하는 file만 푸는 것으로 가정하였습니다.
    - 그냥 shell 에서 사용할 때에는 다음과 같습니다.
         # unzip test.zip            # zip file 전체를 푸는 경우
         # unzip test.zip one_file # zip file에 one_file이 있을 경우, 그 file 만 푸는 경우
         # unzip -t test.zip         # zip file의 상태를 검사하고, 내부 file list 를 표시
         # unzip -l test.zip         # zip file의 내부 file list를 표시
*/




/*
-----------------------
RF ATT (ANTS)
-----------------------
	m_St.Fwd_Out_Att_x2[], m_St.Rvs_Att_x2[l]; // 0.5dB step, PRU-PAU
	m_St.DL_Att[l], m_St.UL_Att[l];     // 0.25dB step, AAU


INT8U Is_CRPI_Upper(INT8U cpri)
INT8U Is_CRPI_Lower(INT8U cpri)
INT8U Is_CRPI_100Mhz(INT8U cpri)
INT8U Is_CRPI_A_Mhz(INT8U cpri)

#warning "--------[RCU] @5G, When AC Fail, REMS-Report is Needed."
*/


/*

=====================================================
Check
=====================================================
2. task 실행 화면 표시 : download 시, update 여부 및 타임아웃 얼마?
	강제 동작시켜서 확인하는 방법.
3. SYSLOG 확인.얼마나 자주, 언제 ??
=====================================================
To do
=====================================================




Get_OnOff_Str(m_St.Pwr_Amp_En[path4]),


reboot 되면, eMMC write 동작 하지 않도록 수정 필요ㅛ


ru init 6단계 에서만 (5단게 10분) gREMS alarm report 가능하게	
===========================================
*/



/* 

//-------------------------------------------
V20.07.15 (M_88.01.06) / PRU / 2020.07.28
V20.07.16 (M_99.01.06) / PRU / 2020.07.28
//-------------------------------------------
	1. Module Inventory : 상태응답 순서 변경(SFP 순서).

//-------------------------------------------
V20.07.14 (M_99.01.05) / PRU / 2020.07.27
//-------------------------------------------
	1. Module Inventory : 상태응답 순서 변경 
    * PRU : DTU - PSU -  PAU0/1/2/3 - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd
    * AAU : DTU - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd - RCU
    
//-------------------------------------------
V20.07.13 (M_99.01.05) / PRU / 2020.07.24
//-------------------------------------------
	1. Module download 2원화 수정 중.(Un-Define "__USE_MOD_DUALITY__")
	2. SFP Inventory (0x6A Command) 수정/확인.


//-------------------------------------------
V20.07.11 (M_99.01.05) / PRU / 2020.07.23 : not release
//-------------------------------------------
	1. Module download 2원화 수정 : 
	define "__USE_MOD_DUALITY__"

//-------------------------------------------
V20.07.10 (M_99.01.05) / PRU / 2020.07.23
//-------------------------------------------
	1. FPGA Reg.Map : SFP Vendor PN, Vendor Name, Vendor SN, Date Code 필드 추가
	2. GUI 상태등답에 SFP Vendor PN(AID 0x5316) 추가.
	3. SFP Inventory (0x6A Command) 추가.


//-------------------------------------------
V20.07.09 (M_99.01.05) / PRU / 2020.07.21 : without BOOT.BIN file
//-------------------------------------------
	1. gIRO-T 장비타입 : PSU 상태응답 giRO_T remote type : "0A" -> "0C" 로 변경.


//-------------------------------------------
V20.07.06 (M_99.01.05) / PRU / 2020.07.17 : with BOOT.BIN : 600Mhz
V20.07.07 (M_99.01.05) / PRU / 2020.07.17 : with BOOT.BIN : 800Mhz
V20.07.08 (M_99.01.05) / PRU / 2020.07.17 : without BOOT.BIN file
//-------------------------------------------
	1. DDR-Memory Clock info AID(0xC0A5) Added.

//-------------------------------------------
V20.07.05 (M_99.01.05) / PRU / 2020.07.15
//-------------------------------------------
	1. gIRO와 gIRO-T 장비타입 구분.

//-------------------------------------------
V20.07.04 (M_99.01.05) / PRU / 2020.07.15, with BOOT.BIN : 800Mhz
V60.07.04 (M_99.01.05) / PRU / 2020.07.15, with BOOT.BIN : 600Mhz
//-------------------------------------------
	1. BOOT.BIN install 처리 script 추가.
	2. gIRO와 gIRO-T 장비타입 구분 추가.(type을 eMMC에 저장)	


//-------------------------------------------
V20.07.03 (M_99.01.05) / PRU / 2020.07.14
//-------------------------------------------
	1.gIRO와 gIRO-T 장비타입 구분 추가.(type을 eMMC에 저장)	
	2.SFP Inventory (0x6A Command) 추가 중.


//-------------------------------------------
V20.07.02 (M_99.01.05) / PRU / 2020.07.08
//-------------------------------------------
	1. BIP와 Hard Clipping Count 초기화 
	[기존] 통계보고 후, 누적 초기화
	[수정] 통계보고 후, 계속 누적하도록 변경(초기화 하지 않음)
	
	2. 통계보고 AID 추가.	
	AID 0x5024 : 온도(PSU)
	AID 0x5025 : 전압(PSU)(Main DC)
	AID 0x5026 : 전압(PSU)(Sub DC)
	AID 0x511A : Return loss(DL)
	AID 0x5409 : Optic Delay (단방향)

//-------------------------------------------
V20.07.01 (M_99.01.05) / PRU / 2020.07.03
//-------------------------------------------
	1. "V20.06.08 (M_99.01.05) / PRU / 2020.07.02" 의 5,6번 수정.
	[수정] 
	define "__Send_Init_Alarm__When_0x500D_RuInit_is_6__"
	undef  "__Seperate_AlarmLog__From__AutoReport__"
	2. PD 상한/하한 알람 수정.
	


//-------------------------------------------
V20.06.08 (M_99.01.05) / PRU / 2020.07.02
//-------------------------------------------
1. AID 추가.
	- Hard Clipping count(0x5320) (Mea: 순시값 --> 누적값 변경)   
	- BIP Error 누적 Count 초기화 (0x530B) : (Mea: 순시값 --> 누적값 변경)
	- PD Power 상한(0x530C)
	- PD Power 하한(0x5309)
	- Retrun Loss(DL) (0x511A)
	- Optic Delay (단방향) (0x5409)
	- IP Address (중계기)  (0x5021)
	- IP Address (LS) (0x5022)
	- IP Address (gREMS) (0x5023)
	- 온도(PSU)  (0x5024)
	- 전압(PSU)(Main DC) (0x5025)
	- 전압(PSU)(Sub DC) (0x5026)
	- Hard Clipping Count 초기화(0x5322)
	
삭제--------------------	
	- Hard Clipping 누적 Count(DL)(0xB40E)
	- Hard Clipping 누적 Count 초기화(0xB40F)
	- BIP Error 누적 Count(DL)(0xB427)
	- BIP Error 누적 Count 초기화(0xB428)	
	- IP(0xB000) 삭제.
	- VSS(NOTI IP1) (0xB445) 삭제.
	- VSS(NOTI IP2) (0xB446) 삭제.

2. AID 0x530A
	[기존] 순시값(d_BIP_Count) 사용.
	[수정] 누적값(d_BIP_ErrCnt) 0xB427 을 중복하여 0x530A 에 사용.
	[통계] 원래대로 누적값 사용.(수정 사항 없음)
	
3. AID 0x5320
	[기존] 순시값(d_Hard_Clip_Cnt_DL) 사용.
	[수정] 누적값(d_Hard_Clip_Cnt_Accum) 0xB40E 을 중복하여 0x5320 에 사용.
	[통계] 원래대로 누적값 사용.(수정 사항 없음)

4. Measure 항목에 추가.
	- Retrun Loss(DL) (0x511A)
	- 온도(PSU)  (0x5024)
	- 전압(PSU)(Main DC) (0x5025)
	- 전압(PSU)(Sub DC) (0x5026)

5. gREMS와 통신이 되지 않는 상황에서 발생된 알람들을 내부에서 버퍼링하고 있다가 
	gREMS 통신이 정상화가 되면 버퍼에 쌓인 알람 데이터들 전송 기능
	코드상에 기능 on/off가 가능하도록 구현 필요.	
	[기존]
	gREMS.Ready 이면: 알람Queuing, Alarm Report, log Queuing, Log Write
	gREMS.Not-Ready : log Queuing, Log Write

	[수정] define "__Seperate_AlarmLog__From__AutoReport__"
	gREMS.Ready/Not-Ready 관계없이, 알람 및 log Queuing, Log write
	gREMS.Ready 이면, Alarm Report

6. 	RU Initalize가 초기화 완료시 혹은 5/6 Delay Setup Request waiting 상태로 10분 유지시
	결국 gREMS로 보고를 하기 시작하는 시점에 도달시 현재 발생된 알람은 gREMS에 보고
	[수정] define "__Send_Alarm__When_0x500D_RuInit_is_6__"

7. zModem 실행 후 timeout이 정상 동작 하지 않는 케이스가 있어 시리얼통신이 되지 않는 현상 존재.
[수정] 
	Process 모니터링 후 1분간 지속 중일 경우 auto kill 기능 추가
	sz 명령 실행후, timeout 30s 추가.


//-------------------------------------------
V20.06.07 (M_19.01.05) / PRU / 2020.06.22
//-------------------------------------------
	1. 100M/Alpha insert --> 100M 탈장 -> 100M 다시 장착 시, AMP Off 되는 현상.
	"V20.01.10 (M_19.01.02) / PRU / 2020.01.29" 부터 문제 O
	"V19.12.10 (M_19.01.01)" version 에서 문제 X

	"V20.01.10 (M_19.01.02)" 수정되면서, AMP off 시키는 조건부분 잘못된 부분 수정.
	"100M 다시 장착" 되어도 일정 시간 동안, 100M CPRI_Alarm이 유지되고 있음.
	이때, 100M/Alpha Common CPRI Alarm 이어야 amp off 인데, 
	100M CPRI_Alarm 으로 인해, amp off 됨. 
	100M/Alpha Common CPRI Alarm 일때, amp off 되도록 수정.

	2. UDA Reset : 
	[현상] Alarm only 일 때, gREMS 보고와 UDA Alarm Set 안됨.
	[수정] (다른 부분 수정없이) Alarm only 일 때, gREMS 보고와 UDA Alarm Set 되도록 수정.
	


//-------------------------------------------
V99.06.04 (M_99.01.05) / PRU / 2020.06.22
//-------------------------------------------
	1. TEST Time
		(1) UDA TEST Time : 10뿐(2분)
		(2) Battery Check : 24시간(2분), 2시간(1분)
		(3) Auto Reset
////////////#define _SHORTEN_TIME____PAYLOAD_  // payload TEST
////////////#define _SHORTEN_TIME____AUTO_RST_ // UDA RESET TIME : TEST Mode
////////////#define _SHORTEN_TIME____BATT_MON_

//-------------------------------------------
V20.06.04 (M_19.01.05) / PRU / 2020.06.12
//-------------------------------------------
	Version changed from "V20.06.03 (M_99.01.05) / PRU / 2020.06.11"


//-------------------------------------------
V20.06.03 (M_99.01.05) / PRU / 2020.06.11
//-------------------------------------------
	1. Log Count(AID :0x5454)
	(1) '88' 로 제어시에만 LS 등록번호 Null로 제어 후 저장
	(2) '0' 로 제어시에만 로그 초기화
	(3) 그외 제어시에는 아무것도 하지 않음.


//-------------------------------------------
V20.06.02 (M_99.01.05) / PRU / 2020.06.11
//-------------------------------------------
	1. LS 등록정보 기능 변경
	(1) Log Count(AID :0x5454)를 '88' 로 제어시 LS 등록번호 Null로 제어 후 저장

//-------------------------------------------
V20.06.01 (M_19.01.05) / PRU / 2020.06.10 :
//-------------------------------------------
	1. LS 등록정보 기능 변경
	2. ping test 기능 추가.

//-------------------------------------------
V20.04.20 (M_19.01.05) / PRU / 2020.05.27 :
//-------------------------------------------
	0. debug 표시 수정.
		(1) " IS_BW Not Valid : all(0x%X) 100(%d) A(%d)" : 너무자주 표시되어서, 연속 10회에 한번 표시하도록 수정.
		(2) DL payload onoff 상태 변경 표시 : string 표현 변경 : 
	    "DL_Payload_Off[0][0]" -> "[0][0]"

	1. clock fail 수정 : AAU 수정하면서 같이 수정. 테스트 필요.
	[현상] clock fail 정상으로 가지 않음.(optic 탈착 -> 장착 시)
	[수정] clock 정상처리 부분 누락. register 3s 정상되면, cpri lock에 무관하게 clock 정상되도록 수정.

	[최종]
	(1) 3s 유지 알람(register 값)인 경우,
	cpri(mmap crpi 상태) 읽은 값이 정상이면, 최종 알람상태(m_St.AlmSt.A_4.Clock)를 알람으로 설정.
	그렇지 않으면, 최종 알람상태(m_St.AlmSt.A_4.Clock)는 이전값 유지
	(2) 3s 유지 정상(register 값)인 경우, 최종 알람상태(m_St.AlmSt.A_4.Clock)를 정상으로 설정.
	


//-------------------------------------------
V20.04.19 (M_19.01.05) / PRU / 2020.04.17 : 
//-------------------------------------------
	1. SFP 관련 통계데이터 전송 data 순서.
		[cur] SFP 관련 통계데이터 전송 data 순서가, GUI Status Response 순서와 다름.
		[chg] GUI Status Response 순서대로 응답하도록 수정.(AID 0x5305/08/10/11/14)
	2. 알람 이력 저장 후 통신 정상화 시 전송 기능 제외.
		"__Seperate_AlarmLog__From__AutoReport__"	
	3. Package download (Root_F/S, Kernel) 제외.
		"__USE_KERNEL_ROOTFS_DN__CHECK__"


//-------------------------------------------
V20.04.18 (M_99.01.05) / PRU / 2020.04.14 : 
//-------------------------------------------
	1. Package download
		[Cur] Root_F/S, Kernel : downlaod 안됨.
		[Chg] downlaod 추가. "__USE_KERNEL_ROOTFS_DN__CHECK__" for PRU
		
	2. 알람 이력 저장 후 통신 정상화 시 전송 기능	
		[Cur] CPRI Alarm 전송 시도
		[Chg] 통신 정상화 시 전송 : 
		기존 'log & alarm 전송'용 공통 buffer 대신,
		'log용' 과 'alarm전송'용 2개 구별해서, alarm 은 통신 정상화 시 전송하도록 수정.
		"__Seperate_AlarmLog__From__AutoReport__"	


//-------------------------------------------
V20.04.16 (M_19.01.05) / PRU / 2020.04.13 : 
V20.04.17 (M_99.01.05) / PRU / 2020.04.13 : 
//-------------------------------------------
	1.	Battery Extension	
	[현상] SISO로 변경 시,  Layer #2, #3 #4 AMP 'ON'되지 않음.	
	[수정] Layer #2, #3 #4 AMP 'ON'되도록 수정.
	[현상] Batt Ext' 모드 해제 시, Layer #2, #3, #4 AMP 'ON'되지 않음
	[수정] Layer #2, #3, #4 AMP 'ON'되도록 수정.
	
	"AMP_Alarm_St()", "PAU_FWD_AMP_St()" --> "m_St.Pwr_Amp_En[]"

//-------------------------------------------
V20.04.15 (M_19.01.05) / PRU / 2020.04.13 : 
//-------------------------------------------
	1. battery extension 수정 중.

//-------------------------------------------
V20.04.14 (M_19.01.05) / PRU / 2020.04.12 : 
//-------------------------------------------
	1. battery extension : debug 추가.
	
//-------------------------------------------
V20.04.13 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. battery extension 수정.

//-------------------------------------------
V20.04.12 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. 모듈 이원화 다운로드 사항은 M19.01.05에 포함되지 않아, 원복.
		(undefine '__USE_MOD_DUALITY__' // module 2원화)
		
	2. AC Alarm '발생'보고
	[현상] Battery 실장 상태에서 AC 단절 시 AC Alarm 발생이 보고 이후,  AC복구 시, AC Alarm '발생'보고가 올라 감.
	[수정] AC Alarm 해제 보고.

//-------------------------------------------
V20.04.11 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. battery extension
	현상 : 	['AC fail 상태']에서, 'extension mode off시', 혹은 'Layer 운동모드를 MIMO -> SISO 변경시' AMP On 인되는 부분.
	
	

//-------------------------------------------
V20.04.10 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. UDA Alarm clear :
		현상 : Ru-init 6 단계에서, gREMS에 reset report 후, UDA Alarm(clock/cpri) clear/alarm report 함.
		수정 : uda alarm 유지 하도록 수정.( gREMS report 도 하지 않도록.)

//-------------------------------------------
V20.04.09 (M_19.01.05) / PRU / 2020.04.09 : 
//-------------------------------------------				
	1. UDA alarm 상태에서 UDA 해제가 안됨.
	(1) suppress 안된 상태(uda.reset mode)이며, uda reset 되기 전에, UDA alarm을 clear 했는데, 알람해제가 안됨.
	(2) suppress 안된 상태(uda.reset mode)이며, mode를 Alarm-Only/No-Action 변경했는데, UDA alarm을 clear 안됨.
	mode 변경되면, 알고리즘 초기화 필요.(UDA alarm 도 clear 필요)
	(3) suppress 안된 상태(uda.reset mode)이며, mode를 No-Action 변경했는데, UDA alarm을 상위로 계속 보고함.
	
		

//-------------------------------------------
V20.04.08 (M_19.01.05) / PRU / 2020.04.08 : 
//-------------------------------------------				
	1. UDA Reset Suppress 해제(35m 후)시, 
		만일 uda_clock 혹은 uda_pd_cpri alarm 상태이면, alarm clear 하도록 수정.

	2. Battery로 운영되고 있는 시점에서 장비를 Reset 했더니 AFE 및 Digital Block의 주요 기능들이 Off되는 부분 : 
	Battery 연동 상태에서, Reset 시, AC 알람이 발생되어 있고, 
	PSU와 통신이 이뤄지기 전 AC 알람에 대한 처리 로직이 동작하는 것으로 예상
	수정 : PSU와 통신이 5회 이상 유지가 된 조건에서 동작할 수 있도록 변경

	3. 현상 : GUI Serial port 로 연결해서 REMS로 sftp download test 할 때,
		RCU segmentation fault 띄우면서 재시작 되는 현상.
		수정 : 0x50 Command 의 경우, 처리함수 인자가 null 되는 경우의 처리 수정.
		(UP_Data_Analysis  함수에서 인자로 받는 r_ip pointer)
	
	4. Battery Extension Mode 오동작 건
		(1) "Layer 운영 모드 MIMO"에서 Battery Extension mode 진입시
			Layer 0 On, 3dB 적용 확인, Layer 1,2,3 Off  -> 정상 동작

			이때 Layer 운영 모드를 SISO로 변경하면 
			Layer 1,2,3에 3dB Att만 적용되고 PAU는 Off상태 유지 => On이 되도록 변경 필요

		(2) "Layer 운영 모드 SISO"에서 Battery Extension mode 진입시
			전체 3dB 적용 정상

			이때 Layer 운영모드를 MIMO로 변경하면 
			전체 3dB 유지, Layer 0 On, Layer 1,2,3 Off  -> 정상 동작(Att 감겨있는 것은 안풀어도 무관)

			이때 Battery Extension mode 를 Off하면 3dB ATT은 0dB로 변경되나 
			Off된 PAU는 On되지 않음 => On 되도록 개선 필요

	5. 모듈 이원화 정의
		F/W 다운로드시 "파일명의 모듈 제조사 코드"와 
		"인벤토리의 모듈 제조사" 일치 여부 확인 후 진행.


//-------------------------------------------
V20.04.07 (M_19.01.05) / PRU / 2020.04.07 : 
//-------------------------------------------				
	1. sftp command 수정 : [sshpass -p %s] -> [sshpass -p '%s'] for special characters
	2. UDA 에 의한 reset 수정 : 
		"reboot" command 전에, "halt" 를 먼저 실행하도록 수정.
		(이후, 10s 후, "reboot" command 실행.)


	
"sshpass -p %s sftp -P %s -o StrictHostKeyChecking=no -o ConnectTimeout=30 %s@%s:%s/%s /tmp/%s\n"
	-> 변경
	"sshpass -p '%s' sftp -P %s -o StrictHostKeyChecking=no -o ConnectTimeout=30 %s@%s:%s/%s /tmp/%s\n"
	
	기존도 문제는 없으나, 
	console에서 수동 제어 할때, 그리고, password 가 특수문자로 시작되는 경우,
	-p '%s' 로, -p option 다음에 '' 하지 않으면, 
	password 가 첫 특수문자가 사라짐.


//-------------------------------------------
V20.04.06 (M_19.01.05) / PRU / 2020.04.07 : for gIRO
//-------------------------------------------				
	1. Fan 자동제어(0xB617) == ON 이고, AID 0x5411(FAN_Mode_St) 가 저소음모드의 경우에만, 
	table rpm 에 Max RPM(저소음)(0xB618) limit을 제한되도록 수정.
	(기존, 2020.02.14 수정했던 사항을 원복함.)


//-------------------------------------------
V20.04.05 (M_19.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------				
	1. Version changed : Same with "V20.04.04 (M_99.01.05)"
	2. Ru init 이 안된 상태이고, CPRI alarm 상태에서 	
	아래 메시지 반복되지 않도록 수정.
		<RCU> [AMP_0] reason_SD_5117 : (0) -> (2)


	-------------------
	cpri alarm 이지만, 상위보고 하지는 않는 상태('m_Env.gREMS_Noti_Ready = 0')이므로, 
	reason clear 가 무의미 하므로 
	'Clr__Reason_SD_5115' 하는 부분 삭제.
	
	////path4 = (i-6)/5; // 0~3
	////if(j==4) {Clr__Reason_Dis_5117(path4); Clr__Reason_Dis_5117_Prev(path4);} // bit4
	////if(j==0) {Clr__Reason_SD_5115(path4); Clr__Reason_SD_5115_Prev(path4);} // bit0 

//-------------------------------------------
V20.04.04 (M_99.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------				
	1. Download Cancel 시, RCU Reboot 되지 않도록 수정.
	
//-------------------------------------------
V20.04.03 (M_99.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------				
	1. FTP 다운로드
		(1) FTP 다운로드에 대한 D/L 상태 미 변경
		(2) D/L 중에 다시 FTP 다운로드 명령 수신시 NACK 응답하도록 수정.
		(3) 다운로드 적용시간(from gREMS) 반영(시간 비교후 다운로드.)	

//-------------------------------------------
V20.04.02 (M_99.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------	
	1. AMP Pakcage download : 
		'AMP 사용/미사용'에 관계없이, 'AMP_HW_Insert' 여부만 참조하도록 수정.
		
	2. TSSI 상한 알람
	'100+a' 사용 중에 TSSI 상한알람 발생 상태에서 
	'100 only'로 변경되면, 알람이 해제가 되지 않는 경우 수정.

//-------------------------------------------
V20.04.01 (M_99.01.05) / PRU / 2020.04.01 : for gIRO
//-------------------------------------------	
	0. for gIRO : DL_SD_Alarm field 추가 : gIRO 일때만 적용되도록 define 추가.
		
	1. 다운로드 패킷 프레임수 500Byte로 원복 : 
		define "__USE_PSU_PAU_DN_SIZE_500__"
		
	2. DPD 알람 Reason 구분 필요
		현재 : DPD Error (33/-114/-110/255/256/258) 일 경우 DPD Error Alarm(0x01)으로 보고
		변경 : DPD Error 33d일 경우 Feedback Alarm(0x00)으로 보고 이외는 그대로 유지

	3. gIRO 다운로드 부분 수정(PRU 호환성 부분), PRU의 경우(PSU의 제조사 모드가 gIRO가 아닌 경우)
		Compatibility : 0x5A : .PSU 파일로 다운로드(.PSU 파일이 없는 경우.GPS 파일)
		Compatibility : 0xFF : .PSU 파일로 다운로드

	4. PAU DL Shutdown 상태일 때, PAU 'ON/OFF' 유저 제어 안되는 현상 수정
	
	5. AID 0x0100/0x0101(maker/supplier) 제어 되지 않도록 수정.

	6. 0xB2 Command : zmodem : 전송전에 2s 시간 지연 추가.


//-------------------------------------------
V20.03.10 (M_99.01.05) / PRU / 2020.03.31 : Not Release
//-------------------------------------------
	1. gIRO RCU FW(v20.22.13) 내용 : 
	DL_SD_Alarm field 추가 : un-define 함.
	#undef __USE__gIRO_DL_Shutdown_Field__

//-------------------------------------------
V20.03.09 (M_99.01.05) / PRU / 2020.03.31
//-------------------------------------------
	1. 버전 변경 : from "V20.03.08 (M_19.01.05) / PRU / 2020.03.30"


//-------------------------------------------
V20.03.08 (M_19.01.05) / PRU / 2020.03.30
//-------------------------------------------
	1. PSU Package 다운로드 속도 개선 : 전송 Frame 사이즈 수정(500byte -> 1000byte).
	(PSU 부분 누락되었음, 수정.)
	2. gIRO RCU FW(v20.22.13) 내용 Merge하여 M(19.01.05)로 배포	
	DL_SD_Alarm field 추가 : gIRO 일때만 적용되도록 define 추가.
	#define __USE__gIRO_DL_Shutdown_Field__

//-------------------------------------------
V20.03.07 (M_99.01.05) / PRU / 2020.03.26
//-------------------------------------------
1. CPRI Auto configuration 6/6단계 진입시 LS 제조사를 확인하여 아래 동작 추가
(Nokia는 3단계에서 Delay Setup이 바로 내려와 6/6으로 완료 되는 현상이 나옴.)
	(1) SamSung   : Register.Capture_DL_Time(0x10020)에 0x2d000
	(2) ELG/Nokia : Register.Capture_DL_Time(0x10020)에 0x12000


//-------------------------------------------
V20.03.06 (M_19.01.04) / PRU / 2020.03.26
//-------------------------------------------
1. CPRI Auto configuration 4/6단계 진입시 LS 제조사를 확인하여 아래 동작 추가
	(1) SamSung   : Register.Capture_DL_Time(0x10020)에 0x2d000
	(2) ELG/Nokia : Register.Capture_DL_Time(0x10020)에 0x12000


//-------------------------------------------
V20.03.05 (M_19.01.04) / PRU / 2020.03.25
//-------------------------------------------
	1. PAU/PSU Package 다운로드 속도 개선 : 전송 Frame 사이즈 수정(500byter -> 1000byte).
	2. '상태갑시접속' alarm : LogParset GUI 의 전용 Command의 경우, alarm 발생하지 않도록 수정.
	3. 'Clock fail' 발생 관련 debug : 표시 삭제.




//-------------------------------------------
V20.03.04 (M_19.01.04) / PRU / 2020.03.24
//-------------------------------------------
	1. Clock fail 발생 안되는 것 수정.
	(Clock fail 3s Check timer가 계속 clear되는 부분 수정.)


//-------------------------------------------
V20.03.03 (M_99.01.05) / PRU / 2020.03.24
//-------------------------------------------
	"V20.03.02 (M_19.01.04) / PRU / 2020.03.24"	와 동일버전.

//-------------------------------------------
V20.03.02 (M_19.01.04) / PRU / 2020.03.24
//-------------------------------------------
1. register 제어, "halt" 이후 10s 후 "reboot" 보완추가.(아래 (1)~(4))
	(1) 0xEA Command (PSU Reset) / by Power control
	(2) AID 0x5620 / by user control
	(3) Download / by download
	(4) VSS restart / by VSS
	(5) UDA PD(CPRI)/Clock: by UDA

2. LS 등록번호 : 
	(1) 자동생성시에는 EEP 저장하지 않도록 수정.
	(2) RCU Start 시, 자동생성 여부 check 하는 기능 추가.
	AID에 표시되는 LS 등록번호는 자동생성된것을 보여주고, 자동생성 되지 않았으면 기존의 값을 보여줌.

3. Log 파일에 저장된 상태 패킷의 Information Check sum의 오류 수정 

4. Clock fail : debug 표시 : 2s 마다.
	 "[Clock]-timer alm %d[ms]/clr %d[ms], last %d, tmp %d(reg 0x%X, 0x%X), Mask_at_Reboot %d)", 

5. 삼성전자 일때 LS ID 계산 방식 수정
	Samsung DUH 20 L/S ID 계산식 변경 : "(port_id/8) + (slot_id*2-2)" -> "(port_id) + (slot_id*12-12)"
	Range : 0~35, 
	연산후 0 보다 작을 경우나 35보다 클 경우 LS ID 99로 표시 후 
	"Invalid LS ID = [%d], slot ID =[%d], Port ID =[%d]"  Log 1회 출력


//-------------------------------------------
V20.03.01 (M_19.01.04) / PRU / 2020.03.18
//-------------------------------------------
1. 아래 (2) 변경
	(1) 0xEA Command (PSU Reset) / by Power control
	PRU : register 제어 후 "halt" 
	AAU : "reboot"

	(2) AID 0x5620 / by user control
	PRU : register 제어 후 "halt" --> register 제어 없이 "halt" 만.
	AAU : "reboot"

	(3) Download / by download
	PRU : register 제어 후 "halt" 
	AAU : "reboot"

	(4) VSS restart / by VSS
	PRU : register 제어 후 "halt" 
	AAU : "reboot"

	(5) UDA PD(CPRI)/Clock: by UDA
	register 제어 후 "halt" (10s 지나도 안될 경우가 있다면 "reboot")
	"reboot"



//-------------------------------------------
V20.03.00 (M_19.01.04) / PRU / 2020.03.13
//-------------------------------------------
	1. ADC : 정상 ADC Read 시, 정상 값 backup 하고 있다가, 
	(가령, reboot 도중 ADC Read 하는 부분에서) I2C의 파일 open 실패시 '0' 값으로 리턴하는 대신,	
	이전 정상 값을 return 하도록 수정.	

	2.	AID 0x5421~0x5424 : limit 추가. 
	(DL ON Guard Time/DL OFF Guard Time/DL to UL Guard Time/UL to DL Guard Time)

	3. DTU Reset
	AID 0x5620 수신시, system halt로 처리 
	(S/W Reboot으로 처리는 우선 보류), Reset 원인은 사용자 제어로 유지

	4. 0xEA Command (PSU Reset) 및 기타..
		(1) 0xEA Command (PSU Reset) / by Power control
		(2) AID 0x5620 / by user control
		(3) Download / by download
		(4) VSS restart / by VSS
		(5) UDA PD(CPRI)/Clock: by UDA
		모두 register 제어 후 "halt" 로 처리중. 
		--------------------------------------------------------------
		단, 
		[1] 기존 : 	register(PSU Reset, register 0x33) 을 0x02 data 제어. 0x02 제어되면 "halt" 제어.
		[2] 수정 : 	register(PSU Reset, register 0x33) 을 0x02 data 제어. 
			1초 후, 0x02 상태 값 변경 여부 관계없이 바로 "halt" 제어.
			
	5. 정상 운영 중 clock fail 미발생
	현 구현 상태  : CPRI Core Lock 상태 때 미발생이고, CPRI가 unlock 일때 발생한다.
	-> 수정필요 : CPRI 정상상태에서 Clock fail 이 발생하면 알람 처리, CPRI가 알람 발생해도 Clock fail은 유지
	

//-------------------------------------------
V20.02.14 (M_19.01.04) / PRU / 2020.03.02
//-------------------------------------------
	1. "AID 0x5620 Reset from gREMS"의 경우, gREMS 응답을 위한 지연(1.5s) 추가.
	2. PAU : DL_SD_Alarm field 추가 : gIRO 일때만 적용되도록 define 추가.
	#define __USE__gIRO_DL_Shutdown_Field__
	(V20.22.13, V20.02.13 은 DL_SD_Alarm field를 사용함)

//-------------------------------------------
V20.22.13 (M_19.01.04) / PRU / 2020.03.02
//-------------------------------------------
	0. same with "V20.02.13 (M_19.01.04) / PRU / 2020.02.27"
	1. BIP 통계 : bip 4개중, 첫번째 제외한 나머지 3개에 대해 초기화 안되어 있음.
		초기화 과정 수정.
	2. PAU : DL_SD_Alarm field 추가 : 
		PAU가, VSWR algorithm 진행 중, AMP On 하여 정상여부 check 중, 
		RCU에서 SD.Alarm 해제 되지 않도록, field 추가.

//-------------------------------------------
V20.02.13 (M_19.01.04) / PRU / 2020.02.27
//-------------------------------------------
	1. BIP 통계 : bip 4개중, 첫번째 제외한 나머지 3개에 대해 초기화 안되어 있음.
		초기화 과정 수정.
	2. PAU : DL_SD_Alarm field 추가 : 
		PAU가, VSWR algorithm 진행 중, AMP 하여 On 정상여부 check 중, 
		RCU에서 SD.Alarm 해제 되지 않도록, field 추가.

//-------------------------------------------
V20.02.12 (M_19.01.04) / PRU / 2020.02.24
//-------------------------------------------
	1. Command 0x5A : NACK 응답 시 
	  (1) 아래 변수 clear (tot_crc, downReceivedBlock, header)
	  (2) "/tmp/RCU_DN.tar" 도 지우도로 수정.
	2. Command 0x5B 미적용의 경우에만,
	  (1) 아래 변수 clear (tot_crc, downReceivedBlock, header)
	  (2) "/tmp/RCU_DN.tar" 도 지우도로 수정.
	  (3) "/tmp/DN_file.zip" 도 지우도로 수정.


//-------------------------------------------
V20.02.11 (M_19.01.04) / PRU / 2020.02.20
//-------------------------------------------
	1. ID Mismatch 수정 : debug 추가 및 Test alarm(mismatch) 수정.

//-------------------------------------------
V20.02.09 (M_99.01.03) / PRU / 2020.02.19
//-------------------------------------------
	1. TEST Time 원복	
		(1) UDA TEST Time
		(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
		(3) Battery Alarm (저전압)
		(4) payload Test Time
////////////#define _SHORTEN_TIME____PAYLOAD_  // payload TEST
////////////#define _SHORTEN_TIME____AUTO_RST_ // UDA RESET TIME : TEST Mode
////////////#define _WATCHDOG_TEST_TIME_5s_ // 2020.02.17 : PRU : S/W Watchdog Timer 10s -> 5s Test 시간 적용
////////////#define _SHORTEN_TIME____BATT_MON_


//-------------------------------------------
V20.02.08 (M_99.01.03) / PRU / 2020.02.19 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
//-------------------------------------------
	1. Mismatch Alarm/정상 : GUI는 즉시, gREMS Report는 10분 유지이지만,
		Test Alarm 으로 하는 경우, 10분울 10초로 변경.

	 

//-------------------------------------------
V20.02.07 (M_99.01.03) / PRU / 2020.02.19 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
//-------------------------------------------
	1. GUI Test Alarm으로 알람/정상 발생 시, Alarm report string(AID 부가정보)가 NULL이 되는 사항 수정.
		(1) 수정 AID 
			AID 0x5013 : Temp_Up
			AID 0x5017 : DC
			AID 0x5302 : LOS_SFP		
			AID 0x5304 : LD_SFP_	
			AID 0x5101 : DL.TSSI_Upper		
			AID 0x5102 : DL.TSSI_Lower
			AID 0x5104 : DL.Out_Upper
			AID 0x5105 : DL.Out_Lower
			AID 0x5210 : UL.Over_Input
		(2) AID 0x5215 : Shutdown : reason을 Test alarm으로 선택할 수 없어서, default로 "Rsn_SD__" 로 표시됨.
		
	2. Clock_Status (AID 0xB41A) <--> register를 읽은 값 mapping 변경.
	 reg [2] : JESD Lock,   Lock ='1' : bit5 : ADC/DAC JESD
	 reg [1] : PLL/AFE Init Done ='1' : bit4 : ADC/DAC
	 reg [0] : PLL Lock,    Lock ='1' : bit6 : DCM(Main)
	3. PRU의 출력 하한 알람 유지 조건 10초 -> 60초로 변경


//-------------------------------------------
V20.02.06 (M_99.01.03) / PRU / 2020.02.18 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
//-------------------------------------------
	1 UDA TEST Time (Suppress_Off 시간 변경)
		Suppress_Off:  35m(규격) ->  5m(Test Time)


//-------------------------------------------
V20.02.05 (M_99.01.03) / PRU / 2020.02.18 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
//-------------------------------------------
	1. giRO 관련 PSU MakerMode : "giRO"를 소문자로 만들어서 비교하도록 수정.


//-------------------------------------------
V20.02.04 (M_99.01.03) / PRU / 2020.02.17 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
//-------------------------------------------
	1. UDA TEST Time (Suppress_Off 시간 변경)
		Suppress_Off:  35m(규격) ->  2m(Test Time)
		SUPPRESS_ON : 120m(규격) -> 15m(Test Time)

//-------------------------------------------
V20.02.03 (M_99.01.03) / PRU / 2020.02.17 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
//-------------------------------------------
	1. L/S : alarm report : 'Alm_Subcode' 4byte little/big endian : 2020.01.14 버전으로 복귀.
	2. giRO 관련 PSU Compatibility : PSU protocol 관련 추가 및 적용.	
	3. Super CAP 장착 상태에서 AC 단절 시, FPGA 기능OFF 시, Ping 1초 유지 : 기존처럼 3~5초 되도록 수정.

	


//-------------------------------------------
V20.02.02 (M_99.01.03) / PRU / 2020.02.17 : Test Time version
//-------------------------------------------
	1, RCU App이 실행되고, 최초 AC 알람 발생 시에는 Reason이 없는 것 수정.
	2. SFP LD/PD 알람 : 'clock alarm' or 'cpri alarm' 이면, 알람 clear 하는 조건에서 'cpri alarm' 조건은 삭제.
	3. 외장Batt Low 알람 : AC fail 에 관계없이 외장Batt Low 알람 발생하도록 수정.
	4. TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test 시간 적용
	(3) Battery Alarm (저전압)
		- Test 시간 적용: ("_SHORTEN_TIME____BATT_MON_")
		- 24hour -> 2m, 2h -> 1m
	
	

//-------------------------------------------
V20.02.01 (M_99.01.03) / PRU / 2020.02.14, 
//-------------------------------------------			
	1. UDA TEST Time 원래대로 원복.


//-------------------------------------------
V20.02.00 (M_99.01.03) / PRU / 2020.02.14, <UDA TEST Time Version>
//-------------------------------------------			
	1. SFP_LD_Pwr(0x5305) : SFP insert 안되면 0.0 표시하도록 수정.
	2. SFP_PD_Pwr(0x5308) : SFP insert 안되면 0.0 표시하도록 수정.
		
	3. giRO 관련 PSU AID 추가 : 
		PSU Maker Mode(0xC080), PSU Compatibility(0xC081)
		
	4. package download : PSU 변경
		(1) "Compatibility"가 0x5A 이거나,"Maker Mode"가 "giRO" 이면 "*.GPS" file 사용.
		(2) 그외 : 기존대로 "*.PSU" file 사용.		

	5. FAN : Auto mode(0xB613) ON 의 경우, AID 0x5411(FAN_Mode_St)에 관계없이, 
		Max RPM 까지만 동작하도록 수정.

	6. UDA Test F/W : #define _SHORTEN_TIME____AUTO_RST_
		(1).CPRI,CLOCK UDA 발생조건
			A.30분 유지 -> 2분 유지
			B.30분 이내 20회 발생 -> 30분이내or2분이내 3회 발생
		(2).CPRI,CLOCK UDA 해제조건
			A.정상 상태 1분 유지

	7. UDA Test F/W : #define _SHORTEN_TIME____PAYLOAD_		
		(1).불요파 UDA
			A.발생조건 10분 -> 2분 : 
			B.해제조건 1분 (기존 동일)	

//-------------------------------------------
V20.99.32 (M_19.01.03) / PRU / 2020.02.13, TEST Version
//-------------------------------------------
	1. log 취득 기능 추가.
	(1) Command 0xB0 ~ 0xB5 추가.
	(2) 5m log file : tgz 압축하도록 수정 및 압축파일 전송.
	(3) AID 0xD454 Added.

//-------------------------------------------
V20.01.19 (M_19.01.03) / PRU / 2020.02.04
//-------------------------------------------
	version change from "V20.01.17 (M_19.01.00) / PRU / 2020.02.03"
//-------------------------------------------
V20.01.18 (M_99.01.00) / PRU / 2020.02.04
//-------------------------------------------
	version change from "V20.01.17 (M_19.01.00) / PRU / 2020.02.03"
//-------------------------------------------
V20.01.17 (M_19.01.00) / PRU / 2020.02.03
//-------------------------------------------
	1. Layer 미사용 설정 후 재 부팅 시, Disabled 알람이 발생 -> 마스킹 -> 발생 -> 마스킹 반복 됨 수정
	(재부팅 시에 미사용 Layer의 Disabled 상태값 (0x5117)이	‘ON’ 상태로 올라오지 않도록 수정)
	2. PAU OFF 상태에서 Layer 사용/미사용 설정 시,	Disabled(User) 알람이 발생/해제가 한번씩 더 올라가는 
	경우의 수정
	3. L/S alarm report : string ("User controlSyst"로 표시되는 부분)수정.
	4. Layer 사용 정보(0x5002) 표현 방법 수정
	 (기존) 0x0 : 미사용 0x1 : 사용
	 (수정) 0x0/2/3 : 미사용 0x1 : 사용


//-------------------------------------------
V20.01.11 (M_19.01.00) / PRU / 2020.02.03
//-------------------------------------------
	1. Layer Splitter 등록 번호(0x500E) 추가
	2. LS Gain Balance(0xc076) : '시스템 Reset시 항상 ON' 되도록 수정.
	3. layer 미사용 제어 시, PAU/LNA Off제어 수정.

	--  내용이 수정 반영된 AID 문서 송부 드리오니 F/W 및 GUI 구현 요청 드립니다~
	PRU 와 AAU의 배포 버전에 대한 협의를 진행 중이오니 아직 통합 버전은 이전과 같이 PRU/AAU 모두 19.01.00

//-------------------------------------------
V20.01.10 (M_19.01.02) / PRU / 2020.01.29
//-------------------------------------------	
	1. VSWR Alarm -> Normal 되는 순간의 VSWR update
	2. '출력하한 알람' 중, "TSSI 하한알람" 발생한 경우, '출력하한 알람' mask 하지 않도록 수정.	
	3. 'AMP_Not_Use' -> 'AMP_Use' 시, 출력하한 알람 바로 뜨지 않도록.
	4. Battery Extension 동작 : Disable 알람 발생 및 해제 안되도록 수정.


//-------------------------------------------
V20.01.08 (M_19.01.02) / PRU / 2020.01.29
//-------------------------------------------
	1. VSWR Alarm -> Normal 되는 순간의 VSWR update
	2. AC Fail : Alarm Report 2번 되지 않도록 수정.
	3. Layer 미사용 설정 조건 : DL payload on/off 제어가 바뀔 때 마다 아래처럼 노란색 디버그 표시 추가.
	4. 'AMP_Not_Use' -> 'AMP_Use' 시, 출력하한 알람 바로 뜨지 않도록.
	5. Battery Extension 동작 : Disable 알람 발생 및 해제 안되도록 수정.


//-------------------------------------------
V20.01.07 (M_19.01.02) / PRU / 2020.01.29
//-------------------------------------------
	1. Battery Extension 동작 : Disable 알람 발생 및 해제 안되도록 수정.
	2. 출력 하한 알람 TSSI 레벨 : (-50)dBm -> (-46)dBm 변경.
	3. Layer 미사용(AMP 미사용) 시, 3초마다 amp off, lna off 제어 내리도록 수정.
	4. AC Fail 시, AC 값(AID 0xB603) 표시 수정.



//-------------------------------------------
V20.01.06 (M_19.01.02) / PRU / 2020.01.28
//-------------------------------------------
	1. UDA 불요파 Payload OFF시, 출력 하한 알람 masking 추가.



//-------------------------------------------
V20.01.05 (M_19.01.02) / PRU / 2020.01.28
//-------------------------------------------
	1. DUH-ID(AID 0x5009) 추가.
	2. UDA 불요파로 인한 Payload off 시, DL/UL AMP 제어.
	[기존] Payload Off, DL AMP Off 하지만 UL AMP 제어 X.
	[수정] Payload 만 Off (DL/UL AMP 제어 X)
	3. 출력 상한 알람 발생/해제 유지시간 : 3초 	
	4. AMP ON의 경우, VSWR 값을 Update하고, VSWR alarm 시, 
	마지막 update 된 VSWR값을 Alarm Reason에 사용하도록 수정.
	5. TSSI log 출력 중.(__USE_TSSI_LOG__)


//-------------------------------------------
V20.01.04 (M_19.01.02) / PRU / 2020.01.23
//-------------------------------------------
	1. F/W 최초 다운로드시 설정 값 변경 건	
		(1) AID(0x5104) 출력상한 : 변경 요청 값(48dBm)
	2. "UDA 관련 L/S 보고할 때, String 변경"
		<변경>
		SPURIOUS-Clear/SPURIOUS-AlarmOnly/SPURIOUS-PayloadOff
		CPRI-Clear/CPRI-AlarmOnly/CPRI-Reset
		CLOCK-Clear/CLOCK-AlarmOnly/CLOCK-Reset




//-------------------------------------------
V20.01.03 (M_19.01.02) / PRU / 2020.01.21
//-------------------------------------------
	1. F/W 최초 다운로드시 설정 값 변경 건	
	(1) UL Gain 자동 backoff (AID 0x5450) : OFF
	(2) LS Gain Balance (AID 0xC076) : ON
	- 기존 : 'LS Gain Balance'가 항상 동작하고 있었음.
	- AAU처럼 on/off 상태/제어를 추가함.
	- AAU처럼 off 의 경우에는, LSH_Gain_Set (AID 0xC062~64/65~67) 의 값 대신 0의 값을 사용하도록 수정..
	


//-------------------------------------------
V20.01.02 (M_19.01.02) / PRU / 2020.01.20
//-------------------------------------------
	0. UL 과입력 S/D 알고리즘 동작 되도록 수정.
		(30분 S/D구간에서 S/D알람(Reason : UL과입력) 정상 확인) 
	1. L/S : alarm report : 'Alm_Subcode' 4byte little/big endian 수정.
	2. UL Gain 자동 Backoff	(AID 0x5450) : AAU 수정하면서 같이 수정.
	(1) 사용자 제어 수정.
		기존) cascade ID == 0 인 경우에만 저장.	
		상태는 cpri lock 되어서 routine이 동작해야만 update 됨.
		수정) cascade ID == 0 에 무관하게 저장가능하도록 수정.
		제어시, 상태도 바로 바뀌도록 수정.(cpri lock 여부에 관계없이)
	(2) reset 후 GUI 초기 상태값 수정.
		기존) default ON
		수정) EEP 저장되어 있는 값을 보여주도록 수정.
		(cascade ID 이 0이 아닌 경우,  
		상위 장비와 연동된 후에는, CPRI unlock 이 되어도 바로 전의 상위장비에서 받은 
		'UL Gain 자동 Backoff' 를 유지하는 것은 수정된 것이 없고, 
		다만, reset 후 GUI 초기 상태값이 EEP 저장되어 있는 값이 되는 것만 수정.)
		
	3. "2020.01.20 : v20.99.02" Test version의 'UL 과입력 S/D 알고리즘 동작' 확인 용 디버그(_PAU_1st_TEST_Debug_) 삭제.
	4. F/W 최초 다운로드시 설정 값 변경 건	
		(1) DL TSSI 하한/100MHz	Layer#0 ~ #3 (AID 0x5102)
		(2) DL TSSI 하한/+aMHz	Layer#0 ~ #3 (AID 0x5102)
		(3) DL 출력 하한	Layer#0 ~ #3 (AID 0x5105)
		(4) UL Gain backoff	Layer#0 ~ #3 (AID 0x5208)		
		(5) LS Gain Balance	 (AID 0xC076)
		(6) DL LSH Gain(N)(100+a) (AID 0xC063)
		(7) DL LSH Gain(E)(100+a) (AID 0xC064)


//-------------------------------------------
V20.01.01 (M_19.01.02) / PRU / 2020.01.14
//-------------------------------------------
	1. Version Release from "V19.12.26 (M_19.01.00) / PRU / 2020.01.14"
	2. TSSI Log 출력 유지 ("__USE_TSSI_LOG__")


//-------------------------------------------
V19.12.26 (M_19.01.00) / PRU / 2020.01.14
//-------------------------------------------
	1. RU Init 중에는, AMP-Stdby, Mimo-Stdby가 마치 시간이 out-of-time 인것 처럼, 
		exit 되도록 수정.(amp-off 로 exit 되도록 수정)
	2. TSSI Log 출력(TEST Version)


//-------------------------------------------
V19.12.25 (M_19.01.00) / PRU / 2020.01.14
//-------------------------------------------
	1. RuInit(6/6)되기 전, LNA ON/OFF 상태 toggle 되지 않도록 수정.
	2. TSSI Log 출력(TEST Version)


//-------------------------------------------
V19.12.24 (M_19.01.00) / PRU / 2020.01.14
//-------------------------------------------
	1. RU Reset 시 MIMO Standby Mode 동작 상태(AID 0x5431) On 표시 이상 
	: off 되도록 수정.
	2. TSSI Log 출력 방법 변경.
	3. RuInit(6/6)되기 전에 LNA가 ON 되어 있는 것 수정.
	RU Init Status (6/6) 되기 전에, AMP OFF & LNA OFF 상태 유지 하도록 수정.

//-------------------------------------------
V19.12.22 (M_19.01.00) / PRU / 2020.01.14
V19.12.23 (M_19.01.01) / PRU / 2020.01.14
//-------------------------------------------
	1. TSSI Log 출력 되도록 수정(define option 수정.)




//-------------------------------------------
V19.12.20 (M_19.01.00) / PRU / 2020.01.13
V19.12.21 (M_19.01.01) / PRU / 2020.01.13
//-------------------------------------------

Debug 추가 및 아래 추가

1. TSSI 대표값 -90dBm 처리 기준 
: -58dBm 이하 ' -70dBm 이하	PRU/AAU 공통이며 TSSI 레벨이 -70dBm 이하의 경우 -90dBm으로 대표값 처리
2. TSSI 하한 설정값 : 
: -60 ' -70dBm	TSSI 하한 설정 값을 f/w 다운로드 시 -70 dBm으로 default setting 되도록 수정




//-------------------------------------------
V19.12.18 (M_99.01.01) / PRU / 2020.01.10
V19.12.19 (M_19.01.01) / PRU / 2020.01.10
//-------------------------------------------
	1. RCU Restart 시, USER-AMP-OFF 이었으면, OFF Reason 설정.
	2. MIMO_Standby 
	(1) AMP 1ea user off 시, mimo standby 안되는 부분 수정.
		"PAU_OFF_Req[AMPU_4_MAX]"
	(2) shutdown alarm 해제 되지 않도록 수정 : 
	(3) mimo standby backup alarm 사용을 amp off 된 구간에만 사용하도록 수정.
	(4) TSSI 상한 check : mimo/amp standby 구간 동안에는 check 하지 않도록 수정.



//-------------------------------------------
V19.12.17 (M_19.01.01) / PRU / 2020.01.09
//-------------------------------------------
	1. L/S alarm 응답시, 무조건 정상으로 report 하는 부분 수정.

//-------------------------------------------
V19.12.16 (M_19.01.01) / PRU / 2020.01.09
//-------------------------------------------	
	1. RCU Restart 시, version 표시 전까지 CPRI/Clock alarm masking 추가.

//-------------------------------------------
V19.12.15 (M_19.01.01) / PRU / 2020.01.09
//-------------------------------------------	
1. Amp Off 원인 표시 수정
	0x5117 : Disabled Alarm
   	0x5115 : Shutdown Alarm

	disable alarm(by user) 이 sd alarm에 우선함.
	sd alarm 이 disable alarm(by system)에 우선함.
	disable alarm reason 있는 상태에서 overwrite 안됨.
	- Ru initial(AID 0x500D) 안되어 있고, factory off 의 상태에서 user 제어 못함.
	- sd (ul over in algorithm 중) amp off 하면, sd clear, disable alarm 발생.
	(AMP SD(VSWR, HighTemp, OverPwr, UlOverPwr, DC)의 경우, user amp on 제어 안됨.

2. 국소명 수정 : 2020.01.09
	SubData 1st byte가 기존에는 length 이었는데, 이 부분 삭제됨. 
	SubData 1st byte가 국소명 1st byte 가 됨.



//-------------------------------------------
V19.12.14 (M_19.01.01) / PRU / 2020.01.08
//-------------------------------------------	
	1. debug log clear	
		<RCU> [AMP#4] AMP_CMD_DIS Ctrl ---
		<RCU> [AMP#4] sendPAU_EnDis : Dis ---
	2. AufoConfigFlag.Init_Status == 0 이면, gREMS 보고 하지 않는 것 추가.
	3. AID 0x5117 Disable alarm/reason 안되는 부분 수정.



//========================================================================================
//-------------------------------------------
V19.12.13 (M_19.01.01) / PRU / 2020.01.08
//-------------------------------------------	
(*Not Release 사항포함.)
	1. CPRI, Clock, LOS, LOF : alarm(3s), normal(3s)
		("__2019_1226__Use_Alarm_Timer_Structure__")
	2. VSS Table의 Z.223(DUH_ID 필드) 추가 : VSS Table debug에 구현
		(d_DUH_ID[CPRI_PATH])
	3. Clock alarm : string 표시 항상 정상으로만 되는 것 수정.
	4. 알람 알고리즘 변경.
	Clock, Cpri알람 발생 시 타 알람의 알람 유지시간 초기화 : 출력하한 Alarm, SFP_LOS Alarm, SFP_LD Alarm 의 유지시간 초기화.
	5. gREMS alarm 2회 발생 : 수정, test 필요 (gREMS alarm 보고 시, 간혹 2번 보고하는 경우)
	6. Amp Off 원인 표시 수정
	   0x5117 : Disabled Alarm
	   0x5115 : Shutdown Alarm
	7. UL_Over_Input_Alarm(0x5210) : 
	  (1) GUI/gREMS/LS : 동일하게 알람발생/해제되도록 수정.
	  (2) alarm(PAU algorithm 진입시), normal(PAU algorithm 정상 복귀시)  
	8. SD_Alarm(0x5115) : 
	  (1) GUI : amp off 시, amp on 시 알람발생/해제
	  (2) gREMS/LS : AMP Protocol : Rx0 Shutdown alarm 적용.	
	9. BW Gain Comp.(AID 0xC077/78/79/7A) : signed value도 제어 되도록 수정. 

//-------------------------------------------
2020.01.07 : Not Release
//-------------------------------------------
	1. BW Gain Comp.(AID 0xC077/78/79/7A) : signed value도 제어 되도록 수정.

2019.12.26 : Not Release
//-------------------------------------------
	1. VSS Table의 Z.223(DUH_ID 필드) 추가 : VSS Table debug에 구현
		- d_DUH_ID[CPRI_PATH];
	2. Clock alarm : string 표시 항상 정상으로만 되는 것 수정.

2019.12.24 : Not Release
//-------------------------------------------	
	1. gREMS Alarm Report : Shutdown(DL)(AID 0x5115) Alarm String 추가.
	(Get__Str_SD_5115() 함수 수정.)

2019.12.23 : not release
//-------------------------------------------	
1. Amp Off 원인 표시 수정
	--------------------------------------+----------
	0x5117 : Disabled Alarm
	--------------------------------------+----------
	User에 의하여 Off 된 경우	          | User Control
	System Algorithm에 의하여 Off 된 경우 |	System Control
	--------------------------------------+----------
	0x5115 : Shutdown Alarm
	--------------------------------------+----------
	TSSI 상한 초과로 인한 SD	          | Overinput
	Over Power로 인한 SD	              | Overpower
	CPRI Unlock으로 인한 SD	              | CPRI
	Clock Unlock으로 인한 SD              |	Clock
	UL 과입력에 의한 SD	                  | UL Overinput
	UDA에 의한 SD	                      | UDA
	--------------------------------------+----------
//========================================================================================




//-------------------------------------------
V19.12.12 (M_19.01.01) / PRU / 2019.12.20
//-------------------------------------------	
	- Version Only Change from Test version ("V19.12.91")
	Test version ("V19.12.91")
	1. RU Init (by CPRI Unlock/Lock)이 다시 진행 되는 경우 출력하한 알람 발생 되는 것.
	[수정]
	- RU init 될때, Payload off 원인(flag_RU_init, flag_DTU_SD[FWD/RVS])이 활성화 됨.	
	- RU init 완료 후, FWD, RVS flag 모두 clear 되어야 payload on 됨.
	- RU init 완료 시, flag_RU_init 이 clear 될 때, flag_DTU_SD[FWD/RVS] 가 
	바로 clear 되지 않았던 부분(특히 RVS)을 수정.
	

//-------------------------------------------
V19.99.90 (M_19.01.01) / PRU / 2019.12.20
//-------------------------------------------
	0. "V19.12.12 (M_19.01.01) / PRU / 2019.12.20" 에서 수정 : not rlease
	1. DPDv9.0 api 적용관련 Test Version : 
	"__USE_DPD_9v0_Calculation__" Test Version 에만 추가.


	

//-------------------------------------------
V19.12.11 (M_19.01.01) / PRU / 2019.12.18
//-------------------------------------------
	1. DTU 온도보상 On/Off 구현 (상태만 표시, AID 0xB434)
	2. gREMS alarm 2회 발생 : 수정, test 필요.
	3. AMP ON : "system control" 표시 부분 보완.
	4. CPRI Unlock->Lock이 되어 RU Init이 다시 진행 되는 경우에
	AMP가 Turn ON 될 경우, Payload가 늦게 'ON'되면서 발생(출력하한알람)하는 증상
	AMP ON 시에 Payload가 늦게 ON 되는 원인 
	Payload off 이유중, RU Init 안된 경우에 대한 off flag가 삭제가 안된 것으로 판단됨.
	보통 즉시 off flag가 clear되어야 하나, 
	어떤 시간 지연 후에 off flag가 clear 됨.
	
	- 아래 항목(undef 이지만, 이후 사용하지 않도록 삭제.) 
		"__PRU_MMAP_ADDRESS_Changed__" 
		"__SKIP__MMAP__"
	- 변경
		"__DPD_9v0_TEST__" ---> "__USE_DPD_9v0_Calculation__"



//-------------------------------------------
//V19.12.10 (M_19.01.01) / PRU / 2019.12.13
//-------------------------------------------
//	1. 버전 요청 : 통합 19.01.00 / RCU 19.09.01	
//	2. 파형감시 프로토콜 기능 Disable (추후 사용, __USE_Packet_Division_for_WaveSpectrum__) 
//	3. CPRI Unlock 시 Auto configuration 기능 제거		
//		- [V19.10.04 (M_19.01.01) / 2019.10.28]
//		- 추후 사용, __Auto_Config_When_CPRI_UnLock_to_Lock__



-------------------------------------------
V19.12.10 (M_19.01.01) / PRU / 2019.12.13
-------------------------------------------
	1. gREMS 파형감시 응답 1kByte 분할 처리 추가.


-------------------------------------------
V84.10.05 (M_19.01.01) / PRU / 2019.12.11
-------------------------------------------
	1. low Alarm/Normal Algorithm 수정.
	2. Cascade SFP_TX_DISABLE 시에, cascade LD_SFP Alarm 을 mask 하는 것 추가.	
	3. Disable alarm 시, gREMS String Reason에 CPRI, Clock, TSSI 추가.


-------------------------------------------
V52.10.05 (M_19.01.01) / PRU / 2019.12.10
-------------------------------------------	
	#define __PRU_MMAP_ADDRESS_Changed__ 
	#define __DPD_9v0_TEST__ // 51.10.05 Test Version
	/// #define __SKIP__MMAP__
	
	1. MMAP Address Changed : 
	(0xB006_0000 ~ 0xB006_FFFF)를 (0xB007_0000 ~ 0xB007_FFFF)로 변경.



-------------------------------------------
V84.10.05 (M_19.01.01) / PRU / 2019.12.10
-------------------------------------------
	1. Shutdown Alarm gREMS 보고 : 예전 상태로 원복 
		[1] PAU에서 Shutdown Alarm 받은대로 gREMS 보고 하도록.
		
-------------------------------------------
V83.10.05 (M_19.01.01) / PRU / 2019.12.09
-------------------------------------------
	1. CPRI_2 (100M Cascade)의 'SFP(LOS)'알람 발생을 gREMS에 보고하지 않는 현상.
	[요청] JIg-RU1-RU2구성에서 , RU1 장비 SFP에서 광케이블 장착/탈착 반복, RU1이 SFP(LOS) 알람 상위 보고 하는 내용 관찰.
 	L/S 로의 보고는 정상, 알람 해제 보고는 gREMS와 L/S로 모두 정상 진행 됨.
 	[수정] gREMS alarm string을 alarm/Normal 용 2개를 사용하도록 수정.




-------------------------------------------
V51.10.05 (M_19.01.01) / PRU / 2019.12.06
-------------------------------------------
	#define __DPD_9v0_TEST__

	1. Test version from "V82.10.05 (M_19.01.01) / PRU / 2019.12.04"
	[1] mmap 처리 하지 않음.
	[2] 계산 수식 변경 : "10*log10(256*x)" 로 변경.
		- DPD Engine Input Power
		- DPD F/B Engine Input Power


-------------------------------------------
V82.10.05 (M_19.01.01) / PRU / 2019.12.04
-------------------------------------------
	1. SFP LOS/LD 알람 보고시 PD/LD 파워 값이 정상인 오류 개선
	[수정] SFP LOS/LD 알람 2초 유지될 때, 실제 알람으로 판단하고, 
	당시의 PD/LD값을 참조하도록 수정.
	
	2. Layer 미사용시 장비 Power Off/On 하면 미사용 Layer에 DPD Reset
	[수정] DPD Reset (Register 0 Set) 하는 부분 모두, 미사용 Layer여부 체크함.

	3. 출력 하한 알람 : "TSSI 하한알람" or "TX 출력 >= (출력하한설정+2)"
	[수정] "TSSI 하한알람" 일때, 알람이어야 하는데, 정상으로 잘못 되어 있었던 부분을 수정.

	4. PAU 로 부터의 AMP Off Reason이, VSWR, OverPwr의 경우,
		Shutdown 시간(30m) check 해서, 완전 shutdown을 계산해서, 
		SD Alarm(AID 0x5115)를 REMS Report 할 수 있도록 수정.
		(PAU 30분, 10초 off, 5초 on 이 맞아야 함. 시간이 틀어지면 안됨.)
	5. Shutdown Alarm이 완전 셧다운일 때, REMS 보고 되도록 수정.
	6. VSS Monitor 수정.		
	-----------+---------------------------+----------------------		
			   | TX Val					   |	RX Val 
	-----------+---------------------------+----------------------	
	CPRI 0		2 : CPRI Main 상향		   |  0 : CPRI Main 하향
	CPRI 0 +a	3 : CPRI Alpha 상향 	   |  1 : CPRI Alpha 하향
	---------------------------------------+----------------------	
	CPRI 1		4 : CPRI Main Cascade 하향 |  6 : CPRI Main Cascade 상향
	CPRI 1 +a	5 : CPRI Alpha Cascade 하향|  7 : CPRI Alpha Cascade 상향
	-----------+---------------------------+----------------------	
	
	Reg_VSS_DL_Addr[CPRI_0] // register 0: CPRI Main 하향
	Reg_VSS_DL_Addr[CPRI_1] // register 1: CPRI Alpha 하향
	Reg_VSS_DL_Addr[CPRI_2] // register 4: CPRI Main Cascade 하향
	Reg_VSS_DL_Addr[CPRI_3] // register 5: CPRI Alpha Cascade 하향	
	
	Reg_VSS_UL_Addr[CPRI_0] // register 2: CPRI #Main 상향
	Reg_VSS_UL_Addr[CPRI_1] // register 3: CPRI #Alpha 상향 
	Reg_VSS_UL_Addr[CPRI_2] // register 6: CPRI Main Cascade 상향	
	Reg_VSS_UL_Addr[CPRI_3] // register 7: CPRI Alpha Cascade 상향
	
	-----------+---------------------------+----------------------	
			   | TX Val					   |	RX Val 
	-----------+---------------------------+----------------------	
	CPRI 0		Reg_VSS_UL_Addr[CPRI_0]	   |  Reg_VSS_DL_Addr[CPRI_0]
	CPRI 0 +a	Reg_VSS_UL_Addr[CPRI_1]    |  Reg_VSS_DL_Addr[CPRI_1]
	---------------------------------------+----------------------	
	CPRI 1		Reg_VSS_DL_Addr[CPRI_2]    |  Reg_VSS_UL_Addr[CPRI_2]
	CPRI 1 +a	Reg_VSS_DL_Addr[CPRI_3]    |  Reg_VSS_UL_Addr[CPRI_3]
	---------------------------------------+----------------------	

-------------------------------------------
V81.10.05 (M_19.01.01) / PRU / 2019.12.03
-------------------------------------------
	1. (EverNode_11) Test Version
	Noti IP1/2 를 Invalid_IP로 변경 시, gREMS(192.224.0.1)로 패킷 전송관련.

-------------------------------------------
V80.10.05 (M_19.01.01) / PRU / 2019.11.30
-------------------------------------------
	1. (EverNode_27)	
	AID(0x5454) LogCount 제어 시, 알람 카운트가 '0'이 되지만, 
	알람 발생 시 로그카운트 '0'되기 전의 로크 카운트가 복구 됨.
	(1) 이전 backup log 파일이 재사용되지 않도록 chksum file 및 backup file을 삭제하도록 수정.
	(2) mutex(alarm, ctrl) Added.

	2. (EverNode_29)
	상위보고 String : F/W 알고리즘에 의하여 AMP가 'ON/OFF'될 때는, 
	'User Control'대신 'System Control' 사용하도록 수정.

	3.(EverNode_21) VSS Table Debug
	
	4.(EverNode_11) 
	Noti IP1/2 를 Invalid_IP로 변경 시, gREMS(192.224.0.1)로 패킷 전송하도록 수정.
	
-------------------------------------------
2019.11.29
-------------------------------------------
	1. Inventory 제어시, 자동으로 LS 등록번호 생성 후, 비휘발성메모리에 저장안되는 사항 수정.
		( Proc___Inventory_SN() 에서, flag-write 설정 추가)
		
	2. SFTP 접속 타임아웃 : 8[s] -> 30[s] 수정.
	 (Ericson L/S 의 경우, SFTP 접속 문제 간헐적 발생, timeout 변경 후, 정상 동작했다는 HFR 내용에 따라.)

	3. Layer 미사용시 장비 Power Off/On 하면 미사용 Layer에 DPD Reset을 계속 내리는 오류 수정.
		(EverNode_23, 미사용 Layer에 DPD Reset 하지 않도록 수정.)

	4. 출력 하한 알람 해제 조건에 TSSI 값 < -50dBm 조건은 제외하고 
		하기 2가지 조건으로만 구현(EverNode_24)
		- "TSSI 하한알람" or "TX 출력 >= (출력하한설정+2)"

-------------------------------------------
V99.10.05 (M_19.01.01) / PRU / 2019.11.28
-------------------------------------------
	11.18 ~ 11.27 사이 수정된 항목 
	//------------------------------------------------------------------------
	2019.11.27	
		1. 알람 보고시 Power와 관련항목은 알람 보고시 상태가 정상상태가 있어
			알람을 check 하는 시점의 Power와 관련항목(limit)을 사용하도록 수정.
			(알람 상태 변경 시, debug 출력 하도록 수정)
		2. alarm_log 2중화 추가.	

	2019.11.21 
		1. 국소명 제어/표시 수정.(2019.11.20 AAU 수정 시, 동일하게 수정) 
		Get_Mem_Usage() 함수에서, 아래 추가.( /proc F/S 가 없을 수 없지만, 문법상 추가.)
			if(fp == NULL) return 0; 

	2019.11.19
		1. Noti_IP_1/2 : invalid check
			[0] eMMC 에 저장된 Noti_IP_1/2 가 invalid 하면, default IP(192.224.0.1) 사용.
			VSS Channel 읽은 Noti_IP_1/2 가
			[1] 0.0.0.0 이거나 255.255.255.255 이면 invalid
			[2] Broadcast IP(255.0.0.0 or 255.255.0.0) 이면 invalid
			[3] 그외 valid		

	2019.11.18
		1. 통계보고 주기가 "보고안함" 일 때는, 1회/1일 gREMS 시간요청 하지 않는 부분 수정.
		2. GUI ID/PASS 오류 건 보완
			현재 필드에서 이미 변경된 상태라면 gREMS 연동 없이 현장 다운로드 진행시 변경되지 않습니다.
			하여 GUI ID/PASS 저장 영역을 새로 할당하여 진행하도록 변경 요청 드립니다.
		3. GUI ID/PASS, Local time 요청 주기 3[s] -> 5[s]로 변경
		4. 출력 하한 alarm : 출력 하한 설정 값 이하로 '5초' 유지 시 -> '10초'로 변경.
//------------------------------------------------------------------------	





//----------------- version history ---------------------
V98.10.05 (M_19.01.01) / PRU / 2019.11.11. (Test 버전)
	1. Tsync 수정 : 
	delay_dl_ns 
		+ T2A_offset 
		+ PAU_DL_Offset 
		+ Tsync_Offset(500ns fix)
		+ offset_ns(AID 0xB415)
		+ Delay_Adjust_ns(87[ns])
		
	2. TSSI(0x5100) Display Log(every 10s)삭제
		A. 시범국소에는 여전히 모니터링 중 이므로, 코드는 주석처리.
	3. "LoopBack ~" 관련 주기적으로 출력되는 로그 Off (AAU,PRU)
	4. gREMS IP 기억하여 변경된 값이 Valid(0.0.0.0 or 255.255.255.255)가 아닌 경우는 
		이전 DST IP 정보를 유지하여 기능 유지, 
		위 현상 발생시 Mac fail error 로그 출력 필요
	5. RCU Log 다발 - FPGA Delay 값 표시하는 부분, 로그 제거 (AAU,PRU)


V97.10.05 (M_19.01.01) / PRU / 2019.11.11. (Test 버전)
	1. "TSync(DL only) Register 설정 값 "은 아래와 같이 변경
		delay_dl_ns 
			+ T2A_offset 
			+ PAU_DL_Offset 
			+ Tsync_Offset(500ns fix)
			+ offset_ns(AID 0xB415)
	2. delay_dl_ns = DL_Delay(from L/S) -( PAU_DL_Offset + T2A_offset)
		delay_dl_ns < 0 의 경우,
		PRU && Factory_Mode 이면,
		delay_dl_ns = 0 으로 처리.		

V96.10.05 (M_19.01.01) / PRU / 2019.11.11. (Test 버전) : 중간 Test version.


V95.10.05 (M_19.01.01) / PRU / 2019.11.08, (Test 버전)
	1. "TSync(DL only) Register 설정 값 "은 아래와 같이 변경
		(이전) 
		FGPA_Register_DL_Delay + PAU_DL_Offset + T2A_offset + Tsync_Offset(500ns fix)
		(변경)
		DL_Delay(from L/S) + Tsync_Offset(500ns fix) + offset_ns(AID 0xB415)
	2. "AMP_OFF, DPD#0 Normal Set" 를 바로 처리/표시 하지 않고, 연속 5번 이후 처리/표시 하도록 수정.
	

V94.10.05 (M_19.01.01) / PRU / 2019.11.07, (Test 버전)
	1.	ARP 관련 수정 : F/W app 에서 network 설정할 때, broadcast 를 enable 하던 부분을 수정.
	(#define __Disable_Broadcast__)

	2. "TSync(DL only) Register 설정 값 "은 아래와 같이 변경
	(이전) 
	FPGA_Delay_ns_DL(GUI AID 0xB429) 
		+ T2A_offset 
		+ PAU_DL_Offset 
		+ Dly_Adjust(87ns fix) 
		+ Tsync_Offset(500ns fix)
	
	(변경)
	FGPA_Register_DL_Delay
		+ PAU_DL_Offset 
		+ T2A_offset 
		+ Tsync_Offset(500ns fix)

	3. AMP ON시 DPD Reset은 2초에서 3초로(TEST용)
	"DPD_Rst_Chk_MAX_Cnt" : 20(2[s]) -> 30(3[s])
	

V93.10.05 (M_19.01.01) / PRU / 2019.11.05, (Test 버전)
	1.	dpd off 시, DL Delay register 설정 할 때, 170[ns] 추가.
	2.	"시스템 Reset 후 AMP가 OFF 상태에서는 DPD Reset이 계속 내려 오는 경우" :  
	실제로 dpd reset 을 RCU가 제어 하는 경우, 
	"[DPD_DCL#1_Reset] Request(20)" 로그가 찍혀야 합니다.
	일단은, AMP가 OFF 상태이면서, Register 값이 DPD Reset 상태이면,
	"AMP_OFF, DPD#1 Normal Set" 이라는 log를 찍으면서, 
	dpd reset register 를 normal 로 제어하도록 수정.
	


V92.10.05 (M_19.01.01) / PRU / 2019.10.31, (Test 버전)
	1.	V91.10.05 (DPD_DCL_Reset 수정, reset low 2초 유지)에서 수정.
	2.	Delay 제어 수정.(AID는 DL 0xB415, UL 0xB416)
	3.	위1,2 확인 후, 정식버전으로 배포하겠습니다.


V19.10.05 (M_19.01.01) / PRU / 2019.10.30
1. CMD2_CFG_nC_AUTO_RPT(0x4B), CMD2_CFG__C_AUTO_RPT(0x4A) Command : 
	수신 받은 packet의 sub-data 크기가 100을 넘으면, packet 버리도록 수정.


V19.10.04 (M_19.01.01) / PRU / 2019.10.28
	1.	Path 별 AMP OFF->ON 제어 시, Path 별 DPD DCL Reset(0xB411) 수행.
		"Proc___DPD_DCL_Reset"
	2. CPRI unlock 시 LS 와 gREMS에 전송 중단 요청 : gREMS로 전송하지 않도록 수정.	
	3. Auto - configuration 조건 추가 : CPRI unlock 알람 -> CPRI Lock 복귀(알람 해제) 시
		debug 표시 : "[Auto_Config] by CPRI Unlock --> Lock"
	4. V19.10.03 버전 재수정 : gREMS IP가 0.0.0.0 이지만, 알람로그(공용 GUI로 보는 알람로그)는 쌓이도록 수정.


V19.10.03 (M_19.01.01) / PRU / 2019.10.22
	1. gREMS IP가 0.0.0.0 이면, 송신도 안하지만 알람로그(공용 GUI로 보는 알람로그)는 쌓이도록 수정.
	2. VSWR(AID 0xB207) type 변경 :  INT16S -> INT16U 
	3. RESET(AID 0x5620) 시, reset reason을 "user제어"로 변경(동작은 기존의 PSU Reset 처럼)


V19.10.02 (M_19.01.01) / PRU / 2019.10.18
1. ID/Password 의 data 오염이 발생하는 문제 수정.
	gREMS IP가 0.0.0.0 의 경우, RU에서 보내는 자동보고, 및 기타 요청(가령 GUI_ID_PW(0xEC))
	이 RU 자신에게 되돌아오게 되고, 이때, ID/Password 의 data 오염이 발생.
2. 수신된 packet의 data 길이가 맞지 않으면, 처리하지 하지 않도록 수정.
	Command LocalTime (0x69) : 길이가 6이 아니면, 처리 하지 않도록 수정.
	Command GUI_ID_PW (0xEC) : 길이가 22가 아니면, 처리 하지 않도록 수정.
3. gREMS IP가 0.0.0.0 이면, Packet 전송하지 않도록 수정.


V19.10.01 (M_19.01.01) / PRU / 2019.10.15
	1. TSSI (AID 0x5100) Debug display every 10[s].

V19.09.02 (M_19.01.01) / PRU / 2019.10.08
V19.09.03 (M_19.01.01) / PRU / 2019.10.08
V19.09.04 (M_19.01.01) / PRU / 2019.10.08
	1. 'Reg_VSS_DL_Loopbackmode' 에 대해서는 유효성 체크 하지 않음.
	(log 추가 : DL 값과 UL값이 다를때, 10초마다 처리 카운트 로그 표시.)
	

V19.09.01 (M_19.01.00) / PRU / 2019.09.30
	1. factory mode 시, AMP-ON 안되는 부분 수정.
	2. log count(AID 0x5454)를 기존의 log file 갯수가 아닌, Alarm/Ctrl(0xF4 Command) log count로 mapping

V09.26.03 (M_19.01.00) / PRU / 2019.09.26
	1. comm debug size check : debug on 된 중에 2Mbyte 넘으면 0으로 clear.
	2. gREMS Alarm report : VSWR string 수정 ":1" 추가.
	
V09.26.02 (M_19.01.00) / PRU / 2019.09.26
	1. L/S 에서 처럼, RU-ID 할당 및 Noti-ACK 받기 전까지는 gREMS 로 부터의 Packet을 처리하지 않도록 수정.
	
V09.26.01 (M_19.01.00) / PRU / 2019.09.26
	1. "V09.26.01" download 안됨 수정.
	2. Package download FW 적용 시간 표시 : CFnC 항목이라 down-count 표시하지는 않음.
	
V09.26.00 (M_19.01.00) / PRU / 2019.09.26
	1. L/S 에서 처럼, RU-ID 할당 및 Noti-ACK 받기 전까지는 gREMS 로 부터의 Packet을 처리하지 않도록 수정.
	2. delay T2A, TA3 값이 변경되면(10ns 이상), FPGA delay 를 재계산 하여 refresh 설정.
	
V09.25.01 (M_19.01.00) / PRU / 2019.09.25 : Not-Release
	1. L/S 에서 처럼, RU-ID 할당 및 Noti-ACK 받기 전까지는 gREMS 로 부터의 Packet을 처리하지 않도록 수정.

V09.25.00 (M_19.01.00) / PRU / 2019.09.25
	1. reboot 없이 무한 package download 시, tmpfs 용량이 다 차는 부분 수정 : 'make_DN_file_renew' 함수의 fclose()
	2. debug added when amp-multidownload.


V09.24.02 (M_19.01.00) / PRU / 2019.09.24
	1. "V08.24.00" 에서 
	"RU ID 할당 받으면, LS로 부터의 0x9B(RU ID 재할당, Noti-ACK) 에 대해, 응답/처리 하지 않도록" 부분이 
	실제로는 packet 처리가 되고 있어서, 처리하지 않도록 수정.

V09.24.01 (M_19.01.00) / PRU / 2019.09.24
	1.	AutoConfiguration 수행 시, 
		A.가지고 있는 DL/UL Delay 값은 유지. (excluding Delay도 동일)
		B.L/S 로부터 CPRI Request가 내려오면, 정상적인 Delay 값을 받기 전까지는 "0"으로 보고 
		(보고만 "0", 실제 가지고 있는 값은 유지)
		(단, gREMS로 보고되는 값은 실제로 가지고 있는 값을 그대로 보고 함)
	
	2.	Delay Setup이 내려오지 않아서 RU Init Status가 (5/6) 유지하고 있는 경우,
		A.	10분 Timer 돌고, gREMS 보고 시작 하는 시점에서 "AMP On/Off 제어"가 가능하도록 변경
		(단, 실제 가지고 있는 Delay가 정상 범위일 경우)
		(RCU Restart 되면, 실제 가지고 있는 Delay가 정상 범위가 아니고, 
		L/S만 Reset되면, 실제 가지고 있는 Delay가 정상 범위 일수 도 있음.)

V09.24.00 (M_19.01.00) / PRU / 2019.09.24
	1. AutoConfig 수행 시, DL Delay 및 UL Delay 값을 0x00 으로 Set
	2. text log size : 2MB -> 1MB


V09.20.02 (M_19.01.00) / PRU / 2019.09.20
	1.	Sftp 시도하기 전에 ssh key를 지우도록 추가
	2.	Amp 다운로드에 대한 보강작업
		2.1	link fail일 때 다운로드 시도 X
		2.2	Amp가 사용중이 아니라면 comfirm 시도 X
		2.3	PSU에도 같이 적용 함.
	3.	Pkg 다운로드 과정에 대한 DN fail 체크 부분 추가 및 DN fail시 여러 과정 초기화 함수 추가
	4.	Pkg 다운로드 시도하면 다운로드에 사용되었던 변수들 초기화 
	5.  pkg download : AMP confirm 10회 : time delay 300ms 추가


//----------------- version history ---------------------

V09.20.01 (M_19.09.19) / PRU / 2019.09.20
	1. TSSI 레벨 ≥ -50dBm 경우 출력 하한 알람 Check : “5초 유지 시 알람” 조건 추가.
	2. 'V09.20.00' 에서 gREMS-ALM-ACK 수정시, alarm log 저장안되는 것 수정.
	3. TestPort_CIM (AID 0x543F) : flash memory에 user 제어값 저장되게 수정.

V09.20.00 (M_19.09.19) / PRU / 2019.09.20
	1. gREMS-ALM-ACK 처리가 2번째 되는 부분 수정.(주로 Emulator에서 나오는 듯.)
	2. RU-Initial-status(AID 0x500D)가 complete가 아니면, 
		AMP ON 제어 안되게 수정(User 제어도 수정.(AID 0x5126, 0x5226, 0xB237))

V09.19.10 (M_19.09.19) / PRU / 2019.09.19
	1. 다운로드(Reset 대기로 )진행 중, Download 상태 표시 부분 'D/L 완료(Reset 대기) '되도록 수정.
	2. AMP-Standby-Mode-적용Path (AID 0x5437) : AID Data type 수정 : 0x02 -> 0x09(pre-defined)
	3. Log_On/Off(AID 0x5453) 이 ON 일때만, Alarm/Control log(Command 0xF4) 동작하도록 수정.
	4. Package download 후 reset reason 수정.
	5. SFTP download 시간 표시 추가.

V09.19.01 (M_19.09.05) / PRU / 2019.09.19
	1. AMP FW다운로드 시 Confirm하는 부분에서 재시도 횟수 10회로 변경.(chk_amp_FW_confirm)
	2. Ericson/Nokia : default gateway 설정하는 부분 수정.


V09.19.00 (M_19.09.05) / PRU / 2019.09.19
	1. 실제 압축은 풀려 있는 상태이고 적용 대기 상태로 유지 중이라 
	이런 케이스의 경우 D/L 완료(Reset 대기) 으로 변경되도록 수정.
	(DL_St_Rst_Wait -> DL_St_Complete, m_St.DL_State[DN_RCU][DL_ST])	

V09.18.02 (M_19.09.05) / PRU / 2019.09.18
	1. TSSI 레벨(Main/Alpha sum Power) ≥ -50dBm 경우에만 출력 하한 알람 Check

V09.18.01 (M_19.09.05) / PRU / 2019.09.18
	1. gREMS Comm Start 시, 이미 발생된 Alarm 을 Report 하지 않도록 "V08.29.01"를 다시 수정.

V09.18.00 (M_19.09.05) / PRU / 2019.09.18
	1."V09.17.02 (M_19.09.05)" 의 아래 사항을 그 이전 버전으로 원복.
	--------------------------------------------------------
	'UDA-불요파동작모드'(AID 0x5444)가 "Alarm-Only" 로 동작 중에,
	'UDA 불요파 동작 Layer'(AID 0x544C)는 OFF 로 표시가 되지 않도록 함.
	--------------------------------------------------------
	2. TSSI 레벨(Main/Alpha sum Power) ≥ -50dBm 경우에만 출력 하한 알람 Check
	그렇지 않으면 alarm masking
		(1) 100MHz Only 조건 시 100MHz TSSI만  참조
		(2) 100+αMHz 조건 시 100+α Total Power 진행
	3. VSS-DL REC_Port_Num 10 회 count가 안되고 있던 부분 수정.
	


V09.17.02 (M_19.09.05) / PRU / 2019.09.17
	1. DPD  알람 조건 중 TX Engine input Power(AID B408, DPD_ALM_PWR_x10) 레벨 변경 : -23dBm ==> -20dBm
	2. 출력 연산값 (AID 0x5108, DL_OutPwr_Cal_x10) : 음수일 때, 0 로 표시.
	3. 'UDA-불요파동작모드'(AID 0x5444)가 "Alarm-Only" 로 동작 중에,
	'UDA 불요파 동작 Layer'(AID 0x544C)는 OFF 로 표시가 되지 않도록 수정.
	4. 100MHz Only 설정 시 'Alpha 입력 하한'('TSSI_Lower_Alpha')이 발생 : masking 추가.	
	
V09.17.01 (M_19.09.05) / PRU / 2019.09.17
	1. Statis_Rpt_Prd(AID 0x5601) 이 0 이면, 통계보고 안하도록 추가(기존 30분 마다).
	2. RU-Init-Status(AID 0x500D) : 5/6 단계 아닐때 Delay 제어 받으면 에러 리턴(-101,Use of function is limited briefly)으로 수정.

V09.17.00 (M_19.09.05) / PRU / 2019.09.17
	1. L/S 로 부터 받는 Delay 설정 값을 F/W 비휘발성메모리에 Save 하지 않도록 수정.
	(RCU Start 되면, 값을 0으로 초기화하도록 수정)
	2. gREMS : Measure(0x44), CFG_nC(0x49), CFG__C(0x48) 응답에 0x0001 Alarm AID 추가.

	

V09.16.01 (M_19.09.05) / PRU / 2019.09.16
	1. MIMO_Std_AMP_Proc 에서 "m_St.Off_Reason____Prev" 은 참조하지 않도록 수정.

V09.16.00 (M_19.09.05) / PRU / 2019.09.16
	1. CPRI Status 대기 상태에서 Delay setup이 내려오더라도 제어는 처리 하되 
		RU Init status는 절차대로 진행 되도록 수정.
	2. VSS DL update 시, unlock 이면, VSS 10회 check count도 0으로 clear 하는 부분 추가.	
	3. 국소명 (AID0xB011) : 가변길이 AID : max 100으로 응답하지 않고, 가변형 크기로 update 되도록 수정.
	4. AMP Standby : AMP #1 ~ AMP #4 중, AMP #2 ~ AMP #4 가 복구 안되는 부분 수정.
	V08.31.00에서, 'Proc_AMP_Standby' 알고리즘과 AMP_onoff 제어 sync를 맞추기 위해, 
	AMP_Stdby_Sync 추가했는데, 
	모든 AMP 4개 path가 아닌, 첫번째 AMP 에만, 적용됨으로 인한 문제.
	모든 path에 적용되도록 수정.


V09.11.01 (M_19.09.05) / PRU / 2019.09.11
	1. AC Fail -> normal 시, reboot 동작 : 
	battery 미실장인 상태에서만 reboot 되도록 수정.
	2. Batt.extension mode 동작 중, AC Fail이 GUI에서 clear 되는 것 수정.

V09.10.01 (M_19.09.05) / PRU / 2019.09.10
	1. gREMS-packet log(alarm, control)
	(1) logCount(AID 0x5454) 제어시, file을 초기화 하도록 함.
	(2) /usr/app/log/rems/alm_report.log  변경-->  /usr/app/log/rems/rems_alm_rpt.log
	(3) /usr/app/log/rems/ctl_report.log  변경-->  /usr/app/log/rems/rems_ctl_rpt.log
	2. CPRI Recovery : 
	(1) 100-optic-off ,alpha-optic-ON 에서 100을 다시 on 할 때, 
	lock 된 100의 mac 이 00:00:00:00:00:00 이면, 10회 count 를 clear 하도록 수정.
	3. watchdog refresh를 FPGA Task에도 추가. 
	

V09.09.02 (M_19.09.05) / PRU / 2019.09.09
	1. "V09.09.01" 의 2번 CPRI ref 관련 원복.
	2. CPRI unlock 인 path는 VSS update/write 하지 않도록 수정.
	

V09.09.01 (M_19.09.05) / PRU / 2019.09.09
	1. 0xF4 log Command Added : gREMS-Alarm-packet-log, Control-packet-log
	2. CPRI_0(Unlock)/CPRI_1(lock) ---> CPRI_0(lock)/CPRI_1(lock) 의 경우, 
	CPRI ref가 CPRI_1 -> CPRI_0 로 변경 되지 않도록(즉, CPRI_1 유지하도록) 수정.	

V09.06.00 (M_19.09.05) / PRU / 2019.09.06
	1. alarm paket log 생성.(미완)

V09.05.02 (M_08.22.00) / PRU / 2019.09.05
	1. gREMS : CF/C, CF/nC 자동응답의 기지국 name 저장 부분 수정.(1글자 빠지는 것 수정.)

V09.05.01 (M_08.22.00) / PRU / 2019.09.05
	1. AMP쪽, 및 PSU 다운로드시에 retry 하는 부분 추가
	2. SFTP 다운로드 시에 sprint 를 snprintf로 변경

V09.05.00 (M_08.22.00) / PRU / 2019.09.05
	1. gREMS : PLD 응답 수정 : Measure 응답 처리를 별도로 구분하도록 수정.

V09.04.00 (M_08.22.00) / PRU / 2019.09.04
	1. inventory
	(1) report request : pau/psu polling 없이 바로 report 하게 수정.
	(2) auto report : pau/psu inventory polling 없었어도 report 하게 수정.
	2.  A_ID 변경
	  - FAN Mode 0x5410 표현 방법 변경:  0x07(boolean) ->  0x09(pre-defined)
	  - FAN Mode 동작 상태 0x5411 표현 방법 변경:  0x07(boolean) ->  0x09(pre-defined)
	3. A_ID 추가
	  - L/S 수신 DL/UL Delay Verification 0x5408
	4. gREMS protocol 변경 : 
		(1) ftp-download-port length 1byte -> 2byte ftp_get_pkg
		(2) DOWNLOAD 완료(Device -> EMS) : 0x52 -> 0x51
	5. L/S 등록번호(AID 0xB007)
	(1) 내부 변수 명 변경 : 'Serial_Num' -> 'LS_Regist_Num'
	(2) 자동 번호 생성 변경 : 
	- NULL 이거나, KST default : 자동생성.
	- Inventory 제어시 자동생성.
	- 그외 자동 생성 하지 않음.
	6. PRU LED 동작 : alarm 별로 추가.
	


V09.03.00 (M_08.22.00) / PRU / 2019.09.03
	1. gREMS 
		(1) Alarm Report : Alarm info(String) 한글 수정. 20byte 고정으로 응답하도록 수정.
		(2) alarm report 시, 시간 정보 사용.		
		(3) clock fail REMS 보고 안되는 것 수정.
		(4) gREMS에서 local time 응답 잘못하는 경우(연도 틀리거나 월,일,시,분,초가 범위 벗어날때), 
		 시간 설정 안되게 함.
		(5) header 통계보고 주기(0x5601) length 길이 수정 :  7 -> 8	
	2. UDA Clock/PD 로 reset 시, reset-reason 이 PowerCtrl 인것 수정.


V09.02.04 (M_08.22.00) / PRU / 2019.09.02
	1. UDA.Clock alarm : test alarm 되도록 수정.
	2. V09.02.02 에서 통계보고 안되던 것 수정.
	3. ID Mismatch 10분 타이머 표시 : 10분 마다 표시 되지 않도록 수정.
	
V09.02.02 (M_08.22.00) / PRU / 2019.09.02
	1. gREMS 통계 보고 : 0x5405 round_trip : layer x carrier = 1 x 2 이나 통계에는 2 x 2 로 되어 있는 것 수정.
	2. gREMS 자동 보고 : 응답에 시간 정보 AID 0xFFFE를 보고하지 않도록 삭제.


V09.02.01 (M_08.22.00) / PRU / 2019.09.02
	1. SFP 탈장/실장 알람 (SFP_Ins_0_a, SFP_Ins_0_) : GUI Alarm Test로 제어 되도록 수정.


V09.02.01 (M_08.22.00) / PRU / 2019.09.02
	1. gREMS 통계 보고 
	(1) header 의 ADI 0x5601, 0x5602 수정.
	(2) Packet Inforamtion : cur_packet, tot_packet 항목 : 0으로 보고되는 부분을 수정.


V09.02.00 (M_08.22.00) / PRU / 2019.09.02
	1. gREMS : 
	(1) _CF_CTRL_ 자동보고 시, CenterFreq(AID 0x500B) 항목 누락된 것을 수정.
	(2) LS 제조사(AID 0x500A) : _CF_nCTL_ 자동보고 되도록 수정.
	(3) 통계 보고 후, 'bip-err-누적', 'hard-clipping-누적' 을 clear.
	
	



V08.31.00 (M_08.22.00) / PRU / 2019.08.31
	1. AMP-Standby
	(1) 'Proc_AMP_Standby' 알고리즘과 AMP_onoff 제어 sync를 맞추기 위해, AMP_Stdby_Sync 추가.
	(2) debug 표시 변경.
	(3) gREMS 보고 되지 않도록 수정. 
		- AMP Standby Mode 진입 전, 알람상태(출력 상/하한 알람, disable 알람)
		- AMP Standby Mode 진입 -> 해제 시 : gREMS 보고 되는 현상.
	


V08.30.01 (M_08.22.00) / PRU / 2019.08.30
	1. BW Gain Comp. (AID 0xC077 ~ 0xC07A)수정.
	- 0[dB] 제어 안되는 부분 수정. 제어 반복 되는 것 수정.
	2. tsync offset 500[ns] add.
	3. Delay 설정 상태값 음수인 경우 0us로 표기(0xB429/B42A) 
		(register return value 보다 offset (0xB415/B416)값이 커서 음수가 되는 경우.)
	4. 타사과입력 : 	
	(1)	"{[(filter후 파워) >= (-47[dBm)]}" 조건을 
	    "[(filter후 파워) > (-47[dBm)]" 로 변경.
	(2)	"{[(filter후 파워)  - (-47[dBm)] < [-0.5[dB] }" 의 조건 안에서, 
	    {타사+ 자사} ~ {자사} 차이를 +/-0.5 에서 +/-0.4로 수정.
	(3) 제어 및 비교 단위를 0.5[dB] Step -> 0.1[dB] Step  변경.
	(4) ALC Level : (-47.0)[dBm] -> (-46.5)[dBm]
	5. PKG-다운로드
	(A) 요청 사항 : AAU 확인시 DL State에 F/W 전송 후, Reset 대기 상태에서 F/W 적용 눌러도 "압축 해제 중"으로 유지 중. 
		"0x04: Reset 준비중" 혹은 "D/L 완료(Reset 대기)" 로 가야 할 듯 함.
	(B) 수정 : PKG-다운로드시에 타이머 대기 상태가 되면 "DL_St_Rst_Wait" 로 상태 전환.



V08.30.00 (M_08.22.00) / PRU / 2019.08.30
	1. 타사과입력 : {자사+타사} ~ {타사} : +/-1.0[dB] 조건을  +/-0.5[dB] 로 수정.
	
2019.08.29---------------------
V08.29.02 (M_08.22.00)
	1. DPD Off 시, DL-Processing-delay에 +170[ns] 추가
	2. BW Gain Comp.(140MHz ~ 200MHz) 추가. (AID 0xC077 ~ 0xC07A)

2019.08.29---------------------
V08.29.01 (M_08.22.00)
	1. gREMS Comm Start 시, 초기 Alarm Report 되도록 수정.
	2. gREMS Reset Alarm Report : alarm string 추가.
	"운용자 제어", "F/W Download", "전원이상", "기타",
	"전원제어", "VSS", "UDA_PD(CPRI)", "UDA_Link", "UDA_Clock" 
	3. ID Mismatch REMS report 추가
	4. AC Fail 일때 Log : 2분30초만 저장하도록 수정.
	5. Inventory -> L/S 등록번호 : 변경된 format 으로 수정.
	6. Inventory S/N : format 이 맞지 않은 제어는 제어 안되도록 수정.
	



	

V08.28.01 (Main_08.22.00)
	1. 타사과입력 algorithm 수정 : algorithm 조건 수정.
	2. processing fpga delay offset 87[ns] Added (m_Env.Delay_Adjust_ns = 87;)
	3. t_Sync_Delay 제어 시에, delay offset 87[ns] 추가. (d_t_Sync_Delay)
		AID 0xC021 : PRU : Status Only(Control X)
	4. PAU status polling 시, PAU OFF시 Fwd_Out_Pwr_x10(AID 0x5103) 0[dBm] 으로 고정.
	5. Height_Bal_Onoff : RCU Restart 되면, 기본, ON 으로 되도록 수정.
	6. Test Pattern ON시 10분 후 자동 OFF되도록 기능 추가.
	

V08.28.00 (Main_08.22.00)
	1. GUI FW Version (AID 0x5503) : v08.22.00 고정.
	2. GUI FW Version (AID 0xB015) : hidden version 추가.(v08.28.00 표시.)
	3. RU_Init_Status (AID 0x500D) : 
		CPRI_Status 에 대한 설정은 현재 RU_Init_Status 상태가 CPRI_Status 보다 낮은 단계 일때 만, 갱신되도록 수정.
		Dly_Set_Req 에 대한 설정은 현재 RU_Init_Status 상태가 Dly_Set_Req 보다 낮은 단계 일때 만, 갱신되도록 수정.


V08.27.04
	1. 타사과입력 알고리즘 수정. Always ALC ON.
	

V08.27.03
	1. 통계주기(AID 0x5601, Statis_Rpt_Prd) 5분, 15분 추가. 
	2. UL RSSI 계산 수정.
		RSSI_Filter전(0xB302)
		RSSI_Filter후(0xB303)
		RSSI_Noise(0xB306)

V08.27.02
	1. MAC-GUI-Control : fileSystem version(08.15.00)으로 변경되면서 
		Set_System_HW_Mac()에서 사용하는 sh.script 변경.	
	2. reset reason 추가.(VSS, UDA-Clock/Cpri...)
	

V08.27.01
	1. 공용GUI, KST GUI 병행 하면서 다운로드 안되는 부분 수정.
	아래가 2019.08.09에 수정이 되었으나
	2019.08.10 부터 잘못 수정됨.
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_NODE, NULL, MAX_Pr--); // REMS-UDP, 2nd		
		->
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_REMS, NULL, MAX_Pr--); // REMS-UDP, 2nd		
	2. "Proc___PRU_Rvs_OtherCompany__Over_Input	" 계산시, AID 0xB302를 사용.
	3. 타사과입력 알고리즘 수정.
	4. RSSI Noise 대표값 수정.
	

V08.26.00
	1. Factory_mode 추가. (AAU와 동일하나, PAU_insert 만 다름)
	2. register 0x80(Reg_DSP_Common_MIMO_RW_EQ_Common) : 제어연속 100번 실패시, debug 표시 추가.
	3. 대표값 관련 : RSSI(filter 전, AID 0xB302) : -92dBm 미만시 -100dBm 표시.

V08.24.00
	1. IP setting 중복하는 것 수정.
	2. BIP error count : SFP 탈장시, 0 처리 수정.
	3. PAU : EQ 관련 DTU 제어 수정.
		(1) dpd_on  이면, register 0x80, register 0x04 모두 0x00으로 write
		(2) dpd_off 이면,
			register 0x80(Reg_DSP_Common_MIMO_RW_EQ_Common)
			A. 알파 PAU EQ 정보가 ‘00’경우(기존 값 유지) : 0-path : 01, 1-Path :01, 2-Path: 02, 3-Path:01
			B. 알파 PAU EQ 정보가 ‘00’이 아닌 경우	   : 0-path : 01, 1-Path :02, 2-Path: 02, 3-Path:02
	4. 대표값 일부 수정 : RSSI 기존 -80dBm미만 조건에서 -92dBm 미만 조건으로 변경
	5. RU ID 할당 받으면, LS로 부터의 RU ID 재할당, Noti-ACK 에 대해, 응답/처리 하지 않도록(packet 버림.)



V08.23.00 : Not Release
	1. AID 추가.	
		Delay 제어 수신 값(0xC071), 
		Delay 제어 수신 return 값(0xC075)
		Hidden Version(0xC072), 

V08.22.45 : 
	1. AID 0x500D RU Initial status 관련 수정
	2. Tsync offset 제어 요청 값과 register 제어된 값 : +/- 1 차이나면 정상으로 판단(반복 제어 하지 않도록 함)
		

V08.22.00 : Not Release
	1. AID 0x500D Added : 
		0x06(Completed)가 아니면, AMP/Payload OFF
	2. Inv.S/N 변경되면, Serial Number(AID 0xB007)도 같이 변경.

V08.21.00 : Not Release
	1. Optic Delay(Round Trip Delay) 0x5405 항목 개수 수정 2 --> 1

V08.19.91 : made ON 2019.08.21
	1. 분당 Test version : PAU Status 에서, EQ Stauts 항목을 뺌.


--------------------- FW_V08.14.99(FPAG download) added.

	
V08.19.90 : 
	1. Samsung L/S ID 계산식 변경.
	"(port_id/8) + (slot_id*2)"	-> "(port_id/8) + (slot_id*2-2)"



V08.19.00 : Not Release
	1. 초기 부팅 후 amp off 주장비사와 초기화 완료 후 amp on 하도록 변경
	2. Samsung L/S ID 계산식 변경 : "(port_id/8) + (slot_id*2)"	-> "(port_id/8) + (slot_id*2-2)"	
	3. console에서 실행파일 실행시, 버전만 표시하는 옵션 추가.

V08.17.00 : 
	1. CPRI unlock 표시 : console에 Instance 상태값 표시 추가.

V08.14.30 : 
	1. gREMS 

V08.13.00 : 
	1. delay 방식 제어 방법 변경("__USE_ProcessDly_plus_Offset__")
	2. 파형감시 offset : AID Added.
	4. "V08.10.00" 에서, S/N 제어 안되는 부분 수정.

V08.12.00
	1. gREMS stat : "DL_TSSI_Decimal" is not stat : AID is 0xB100
	2. PAUID : 0x20 -> 0x20/21/22/23
	3. AC Fail -> normal : system "halt" 로 PRU Restart되도록 수정.
	4. AID 0xB21D : "AMP Forward Input Power" Added.
	(PAU protocol added)
	5. EQ Status : PAU, Register Added (control, status)
	6. EQ Common Register Added.
	7. 	FAN(외장) 자동제어 : 
		RPM이 '40% -> 0%'로 되는 조건의 구현 상태는 40도 미만 
		-> 30으로 변경.(규격은 30도 미만이므로)

V08.10.00 : Not Release
	1. log Added : 	
		i.	제어 명령(0x01, 0x48, 0x4C), Download명령(0x57,58,5a,5b) FTP 다운로드(0x50,51), 전원 Reset(0xEA),
			Layer Splitter Delay 제어, VSS Reset 수신시 저장.
		ii.	L/S 프로토콜 중 RU ID 할당 완료 및 Init Noti 수신 패킷 및 시간정보 저장
		iii. 제어 이력 파일 1개에 패킷 저장 총 200개 저장
	2. Task End : all task 종료 확인하는 code 추가. task 별 max 1초 기다림.
		PRU : 실제로는 1초 이하 혹은 1.4초 소요. 시험용.

V08.09.01
	1. gREMS TX port : 50000 -> 20000

V08.09.00
	1. L/S : Delay 제어시 정상/비정상 케이스 log 출력 요청
	2. L/S Saumsung 일때, BASE_MAC = Syste_MAC 인 경우 수정.
	3. Alpha 미사용 시, L/S-ID 를 default 0으로 표시하도록 수정.


V08.08.00
	1. L/S UTC Setting 시, gREMS 시간 설정 시, RTC 설정. 
	이후, 10분 마다, RTC를 따라 system time 동기화.		
	2. vss데이터 접근전  cpri mmap을 읽도록 수정
	   (현재의 CPRI-lock 상태를 read 해서, VSS read/write 처리.)
	3. VSS MAC/IP 연속 check : "1초 x 10번" -> "100ms x 10회" 로 수정.
	4. VSS REC-Port-No : 연속 10회 stable check 추가.(cascade-id 는 이미 추가 되어 있음).

V08.07.02 : Not Release
	1. TEST RTC Added
	#define __USE_RTC__
	#define __TEST_RTC__

V08.07.01
	1. 'Alarm information Notification' 및 'C&M Path Switchover Notification' :
	RU ID 할당 되기 전에는 전송하지 않도록 수정.

V08.07.00 : 
	1. PRU ADC : 1.2V, 0.85V ADC에 대해, 전압분배 하지 않기로 함.('*2' 를 삭제.)
	2. TEST Alarm log 삭제(원래 기능으로 원복)
	3. gREMS 통계 : 통계 보고 후, 누적 된, HardClipCnt, BIP_ErrCnt 를 초기화 하지 않도록 함.
	4. gREMS : "GUI-Password/ID" Request, "Local-Time" Request added
	5. PRU : RCU restart 할 때, PHY reset Added.

V08.06.00 : Not Release
	1. gREMS Stat : init bip-err-accumulted, hard-clipping-accumulated
		Init__HardClipCnt_Accum(), Init__BIP_ErrCnt_Accum()
	2. AID 0xC06C : Height Balnance On/Off
	3. L/S Valid-indicator 0x01 되기 전까지는 gREMS Alarm Report 하지 않도록 수정.
	(L/S Valid-indicator 0x01 된 후, normal 로 alarm 변경 report 되는 것 수정.)
	

V08.02.01 : 
	1. L/S : "Initialization Information Acknowledge IE", "Lower node ID Configuration IE"
		[1]	message copy 할때, IE header 의 'IF Length' 만큼 copy 하도록 수정.
			length size 오류 : 4byte -> 2byte 수정.
		[2] samsung 의 경우, 'IF Length' 가 타사 보다 큰것애 대해 max 처리.
	2. alarm log added ( alarm, control) : Test Mode (5분 -> 15초)	
	 	

V08.01.00 : 
	1. L/S : "Initialization Information Acknowledge IE", "Lower node ID Configuration IE"
		message copy 할때, IE header 의 'IF Length' 만큼 copy 하도록 수정.
	2. FAN Mode Init : from PSU Task -> Alarm Task, AID 0x5410
	3. DTU_SD_By_AMP_Not_Use : AMP 미사용으로 인한, Payload off, AMP OFF Added.
	

V07.31.00 : 
	1. "V07.30.99" 적용 : 
	    V07.29.04 에서 __USE_Spectrum_LOW_Limit__가 define이 지워져 있어서 
		이 부분을 아예 없에고 전부 적용 하는 것으로 바꿈. 안 쓰이는 부분은 #if0으로 막음.

V07.29.04 : 
	1. "V07.27.00" 에서, 테스트를 위해 PAU르 항상 "사용"으로 했던 부분 삭제.	
	2. AC 알람 발생 시, 관련 alarm 처리 관련 아래 알람 추가.
	   [1] SFP 탈실장 알람(AID 0x5315), BATT LOW 알람(AID 0x5018) 처리 관련 추가.
	   [2] 출력 하한 알람(0x5105)상태에서 AC fail 일때, 알람 해제 보고 되는 것 수정.	   
	3. PSU Ext.FAN : 시스템 초기 부팅시 RPM 20%에서 시작 5초 유지 : Manual mode 수정.
	4. PSU External FAN : Auto : max temp of DTU and AMP 
	5. LS REC 가 NKA/ERS 의 경우, Base-IP 첫번째 byte 가 192인지 확인하는 조건 추가.
	6. LS ID 추출방법 변경 적용(L/S 가 Saumsung의 경우에만.)
	


V07.27.00 : 
	1. L/S : alarm 관련 string 변경. (AID 문서의 string 처럼 변경)
	2. PSU Ext.FAN : 시스템 초기 부팅시 RPM 20%에서 시작 5초 유지 
	3. UL output Atten(AID 0x5206) : 19.9dB까지만 제어되는 것을 20dB 까지 제어되도록 수정.
	4. DPD 출력 Offset(AID 0xB410) 구현 : DPD 출력(AID 0xB407)에 적용.
	5. AC 알람 발생 시, 관련 alarm 처리 
		[1] 관련 alarm : Clock Fail, TSSI 상한/하한, SFP LD,LOS 알람, CPRI Fail 알람, PAU 알람(Disable 포함)
		[2] 관련 alarm 상태가 변경 되지 않도록 수정(alarm 변경에 대해 gREMS에 보고하지 않음.)
		[3] gREMS/GUI 상태응답에는 alarm을 mask 하여 응답.
	   
	6. 무신호시 대표값 표현 
		[1] TSSI Detect	  : -90dBm, -65dBm 미만시 (Alpha : 100MHz Only 경우 -90)		
		[2] TSSI Detect(Dec.) : 0, 50미만 조건시(Alpha : 100MHz Only 경우 0)
		[3] RSSI Detect	  : -100dBm, -80dBm 미만시(Alpha : 100MHz Only 경우 -100) 	
		[4] PD Power : -40dBm, -21dBm 미만 조건

	7. AutoConfig-Restart, When,
		(1) REC Port Num 변경.
		(2) cascade-ID 변경.
		(3) base-MAC/base-IP 변경.

V07.25.00 : 
	1. L/S packet Debug : string display
	2. FPGA Download 수정. (file update 안될 수 있는 부분 수정).
	3. gREMS Suppress : test 3회 -> 기존 10회.


V07.20.01 : 
	1. PRU : Mac Control added.

V07.20.00 : 
	1. L/S Delay Setting : Num of CPRI 값이 "0"이거나 "2"를 초과할 경우 Error Return(-502, TBD).
	2. L/S : "Send_LSH_UDP_Data" 함수 pthread_mutex_lock 위치 변경.
	3. L/S : delay setting : cpri_num parsing 이 0으로 되는 부분 수정.
	4. Reset Reigster(0x09) : debug 표시.
	


V07.18.03 : 
	1. gREMS : Alarm String Added
	2. gREMS : Inventory Auto Report When SFP info is changed.
	3. AMP Standby Mode / MIMO Standby Mode : 동작 시에  'disable' 알람 마스킹
		(1) Mode 진입 전 알람은 유지하되, 진입 후에 발생되는 알람은 Mask 처리	
		 == 진입되면, 진입전 알람은 유지되고, 진입전에 정상이었던 것은 알람이 되지 않도록,
		 == mode 진입전 알람상태(normal or alarm)을 진입 후에도 계속 사용.
		(2) AMP 관련 alarm,  TSSI/RSSI, Power 상한/하한 알람 부분에 대해 처리.
	4. 	Initialzation Notification Acknowledge를 받기 전까지는 
	상태 요청 등에 대한 메시지에 대한 응답 X
	

V07.18.01 : 
	1. suppress 10 -> 3, clear time 60[s] -> 30[s]
	2. dtu board alarm : message every 10[s] 표시.
	3. clock alarm : payload off
	(register 0x07 pll 부분의 alarm이면, 현재 register 제어 안되나, clock alarm의 경우 payload off는 register 제어 가능하게 수정)


V07.17.03
	1. Loopback On/Off : (제어 X, 상태 Only)
	VSS 필드의 Loopback 필드값이 0xCC, 0xCA인 경우 On, 이외 Off로 처리
	2. Function fail : 
		(1) Layer 미사용 시 FuncFail ALM 발생하지 않는 부분 수정.
		(2) defalut가 0xCC로 시작, 이후 알람 체크 시작부터 상태에 맞게 변경 요청 :  
		(정상은 0x33, 그렇지 않으면, 0xCC로 이미 구현되어 있어서, 추가 수정부분은 없음.)
	3. PRU 및 AAU의 초기 부팅에 대한 DTU 정상 여부 판단 기능 변경
	4. L/S로 부터, ID 할당 받기 전에, BaseMac/BaseIP 변경되면, RCU MAC/IP 변경 추가.

	

V07.17.02
	1. PSU : PSU에서 internal FAN 자동제어 중일때는 
		RCU에서 FAN On/OFF로 인한 (Command 0x7C)제어를 PSU로 내리지 않도록 수정.
	2. VSS RE Channel BW (alpha) : 3~6 이 아닌 경우, 아래 alarm masking
	CPRI alarm, SFP_LOS Alarm, SFP_LD Alarm
		


V07.16.00
	1. 상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어
	2. CPRI alarm/SFP not insert : register 0M Control, but AID 0x5008은 VSS대로 표시.
	3. gREMS : RCU Start 시, 통계주기 부족한 1회에 대해, statistics 1회 report 하지 않음.
	4. AID 0x5620 : Reset : Command 0xEA 처럼 동작하게 수정.



V07.15.00
	1. Super Cap.교체(AID 0x5452) Added.	
	2.초기 부팅 후 알람 보고 시점은 L/S에서  
	(1) Initialization Information Acknowledge Message를 수신하거나
	(2) Valid indicator == 0x01(True)인 조건인 경우에 전송	
		Valid indicator는 0x01이 되는 조건
		(1) Init Noti 수신시
		(2) Init Noti를 처음 요청한 시점부터 1분 경과 후 Valid indicator를 자체적으로 0x01(True) 변경

V07.13.00
	1. VSS MAC Fail Check added : "__USE_MAC_CHECK__"
	2. PRU : Use Fused-MAC @ etc-partition
	3. BW 100+a에서 100Mhz 로 변경시 alpha의 Payload Off하도록 기능 추가
	(반대로 100Mhz -> 100+a로 변경될 경우에는 Payload 자동 On)	
	4. CPRI#1 은 SFP 탈실장 Alarm : 재확인 test 필요.

V07.12.02
	1. DTU 전압 모니터 OFFSET 처리 : 5.8V(+0.3)/1.8V(-0.1)/0.85V(-0.1)
	2. TSSI DEC. 값 OFFSET 처리 : DTU에서 올려준 값에 "× 0.74"처리
	   (dBm 계산도 0.74 처리 한 DEC. 값으로 계산 진행)
	3. Register 계산식 오류 수정.
	10*log10(RSSI/2^32) --> 10*log10(RSSI/2^31)
		UL INPUT RSSI DET (0x6)
		DL_TSSI_RMS  (0x04)
		DL_TSSI_PEAK (0x09)
		UL_RSSI_RMS  (0x22)
		UL_RSSI_PEAK (0x29)
	4. Register "TSSI_SD_Level" 제어 계산 식 변경.
		(1) remove tssi_offset from tssi_level
		(2) divide 0.74 to reg-control-value
		(3) 0xC052 Offset 제어해도 적용되도록 수정.
	


V07.10.00
	1. A_ID 표현 방법 : 이미 구현되어 있음.
		- TBdelayDL(DL Hopping Delay) 0xB42C 표현방법
		- TBdelayUL(UL Hopping Delay) 0xB42D 표현방법
	2. 자동보고 기준시(AID 0x5603
) Added
	3. GUI_ID (AID 0xB012), GUI_Password (AID 0xB013) Added
	4. Alarm Mask (AID 0xB014) Added
	5. TSSI_SD_Level_Set (AID 0x5101) : register control : 계산식 변경. 
	 


V07.09.00
	1. (z.16.0) 및 (z.80.0) 구현 : 
		Main_AID (CPRI, 0x5301) 의 LOS/LOF 발생 조건을 동일하게 참조.
	2. (z.22.0)Optic LOS Detect/Alarm 구현 : 
		Main_AID : LOS(SFP) 0x5302 참조.  Alarm(0xCC), 해제(0x33)



V07.08.00
	1. Masking alpha-SFP-alarm : "SFP_Ins_0_a" When BW-Alpha is not used	
	2. AID 0xB437 : ADC : 전압분배 후 측정 값이므로 X2하여 상태값 표현
	3. AID 0xB438 : ADC 
		(1) AID 응답하는 순서 변경 : 3.3/1.8/1.2/0.85V 순서로.
		(2) 전압분배 후 측정 값이므로 X2하여 상태값 표현
	

V07.04.03 : Not Release
	1. gREMS : CF/nC Auto Report Added (When AID Value is chnaged)


V07.04.02
	1. 초기값은 3dB(제어가 한번도 없었을 때)
		AID 0xB420(DL Atten), 
		AID 0xB421(UL Atten), 
		AID 0xB424(DL Sum Gain), 
	2. AID 0xB40D(Hard_Clip_Lvl) : 
		hard clipping off 일때는 이전의 사용자 제어값을 보이도록 수정.
	3. CPRI not insert 시, BIP count(순시/누적) 0으로 reset.
	4. AC Fail Check : "GPIO_PSU_AC_FAIL_I" 300[ms] -> 100[ms] 변경.	
		(1)	AC 알람 감지 조건 : 100ms 이상 유지시
		(2)	AC 알람에 대한 DTU 절전모드 조건 : AC 알람 300ms 이상 유지 시(현재 조건 동일)		
		(3)	AC 알람 발생 후 상위 보고시 PAU0의 Reset Pin toggle 구현(시험용)
			A.	AC 알람 감지시 REMS로 알람 보고시 Reset 핀을 High -> Low로 변경(1회)
			B.	2회 retry REMS로 알람 보고시 Reset 핀을 Low -> High로 변경
			C.	3회 retry REMS로 알람 보고시 Reset 핀을 High -> Low로 변경
	
	5. Continued_download : 공용GUI 에서 안되는 부분 수정.
	6. LSH : "FX_DELAY_SET_REQ" 의 경우, 'Number of CPRI' 항목을 참조하도록 수정.

V07.03.03
	1. TSSI : decimal 표시 오류 수정.(V07.01.01 에서 발생)
	2. TX 출력 연산 계산식을 수정 : 이중 적용되는 BW에 따른 계수는 삭제
	3. AID 0xB034 : DPD_Eng_Out_Pwr 대신 DPD_Eng_In__Pwr로 변경.
	4. 파형감시 변경. PRU AAU
	5. FTP 전송 시 fail 여부 전송하는 부분 추가
	6. 출력 연산 수정
		(1) 100M Only 경우 출력 연산 하지 않고 AMP 출력 레벨 그대로 Main 출력 연산 항목에 표시하도록.
		(2) AMP 출력이 0인 경우 출력 연산 값도 모두 0으로 표현
	7. BW 관련 수정.( alpha 10M/20M 는 0M 처럼 처리)

	

V07.03.00 : by ycSo
	1.	FTP 다운로드 수정
		FTP 다운로드시 대기 모드로만 동작 하도록 변경
		FTP 실패시 error code 보내도록 변경.
		FTP command (0x50) 일 경우에 error를 보내기 위한 ip 정보 저장.
	2.	박성진 차장 요청으로 파형감시 data 변경.
	3.	AAU GPIO_CONF_DAT_EN EXIT 추가


V07.02.01
	1. amp 관련 alarm 수정.
	(1) amp standby : alarm backup
	(2) mimo standby : amp off alarm : user amp on/off를 사용.

V07.02.00
	1. cpri unlock(alarm) timing check 오류 수정
	2. "SFP Not Insert" or "CPRI Alarm" 일때, BW 제어 변경 : 기존 10M 제어 -> 0M 제어


V07.01.03
	1.	Task 종료 처리 부분 추가
	2.	DL의 파형감시에 대해 별도 offset값 추가. 


V07.01.02
	1. reg "PA_OnOff.On" 를 참조하지 않도록 수정.
	2. AC Fail 300ms check : RCU init 시, timer 초기화 추가.
	3. APP Restart 시, cpri-ulock을 초기 상태로 함.
	초기 1st lock 까지는 2초, 이후는 10초 유지시, cpri-lock.

V07.01.01
	1. 
		AID 0xC062/63/64 : reg "DL LSH 연동" Gain Added.  
		AID 0xC065/66/67 : reg "UL LSH 연동 Gain"  Added.  
		AID 0xC068       : reg "Test Pattern Band Gain" Added.  
		
	2. TSSI 상한 알람 변경
	기존 : TSSI 레벨과 설정값을 비교
	변경 : reg "TSSI Shutdown Status" 사용.

	3. reg "DL Gain" 에 balance 사용하는 대신, reg "Height_Bal_Gain" 사용.
	4. reg "DL Sum Gain" added.
	5. AID 0xC052 : TSSI_Offset_x10 : GUI에서 제어가능하게 수정.
	

V07.01.00
	1. AID 0xC052 : TSSI_Offset_x10 : GUI에서 제어가능하게 수정.	
	2. 
		AID 0xC01D : TSSI SD LV(Short) Added.  
		AID 0xC01E : TSSI SD LV(long)  Added.  
		AID 0xC01F : TSSI SD LV(Low)   Added.  

V06.29.00
	1. gREMS
	(1) 자동보고에 대한 응답 수신시 국소명 저장(GUI 표시)
	(2) auto report(Alarm, 통계보고)의 시작은 L/S로 시간정보 수신 후 시작
	(L/S에서 조회/제어 요청시 응답은 처리)
	(3) Ethernet Packet Loopback added.
	(4) BIP Error Count : 누적데이터를 전송(최대/최소/평균에 모두 동일한 누적데이터 전송)
	(5) Hard Clipping count : 누적데이터를 전송(최대/최소/평균에 모두 동일한 누적데이터 전송)
	(6) Hidden Request/Response(0xE9) 
	(7) 통계보고 1024 packet 확인.
	
	2. Optic Switch Status0 
	(1) Optic Switch 설치(high)/미설치(low)
	(2) AID 0x501C : 기존 제어 -> 상태 핀 감시로 변경

V06.28.11
	1. SFP 탈실장 0x5315 Alarm 추가
	2. PAU On/Off(DL) 0x5116 속성 변경 Operation -> CF/C : 
		MIMO Standby/AMP Standby/Battery Ext : PAU 알람 마스크 적용시 PAU 상태 확인용.
	3. RU Initial status 0xB009 삭제 -> Main 이동  
	4. 국소명 0xB010 추가 	
	
V06.28.10
	1. RU Initial status 0x500D Added.
	2. Clock fail 일때, payload off 하도록 수정.
	3. LOS(SFP) 0x5302, LD(SFP) 0x5304 : alarm 으로만 사용.
	4. 아래 이미 수정되어 있음.
	 - Optic Delay (Round Trip Delay) 0x5405 "-" 표시 조건 추가
	 - TBdelayDL(DL Hopping Delay) 0x5401 "-" 표시 조건 추가
	 - TBdelayUL(UL Hopping Delay) 0x5402 "-" 표시 조건 추가
	 5. gREMS 통계 보고시,
	 - BIP Error Count(CPRI) 0x530A 는 누적값 적용.
	 - Hard Clipping Count(DL) 0x5320는 누적값 적용.
	 6. RU Initial status 0x500D
	 7. VSS : 10회 연속 수정 : BW 부분 잘못된 부분 수정.
	 8. AID 0xC01C (DL Input Power Balance Att) 수정.
	
V06.28.00
	1. AID 0xC01C (DL Input Power Balance Att) Added.	
	2. FPGA init Check 방법 변경.
		clock alarm (m_St.AlmSt.A_4.Clock) 대신, 
		m_Reg_Sts.R_Board_Status Register 의 2개 bit(Pll_Init_Done ,JESD_Lock)를 check.

	3. L/S : 
	(1) "Alarm_Data_Vaild_Indicator" 에 "Vaild_Indicator" 값 응답하도록 수정.
		(수정전 항상 1을 응답.)
	(2) "INIT_NOTI_ACK" 받으면, Vaild_Indicator 를 1로 설정하도록 수정.

	4. default IP : 항상 192.168.0.3 로 시작.(기존 : 전에 사용되었던 값을 사용.)
	5. FGPA Test Register 사용 : 연속 2회 Fail 이면, FPGA read/write 하지 않음.	
	6. CPRI lock과 CPRI Alarm을 같이 동작 : 해제 10초, 알람 2초.
	
	
	
	


		
	

V06.26.02
	1. gREMS report when AC Fail
	2. GPIO_OPT_SW_Status1 : default High Control
	3. gREMS_Auto_Report_Retry_Max : 4회.

V06.26.00
	1. PSU AC Fail IN Pin : 300ms monitor로 수정.

V06.24.00
	1. Restart 시, Inventory Auto Report to gREMS

V06.21.04 :
	1. When CPRI unlock : 
		(1)C&M 처리 X, VSS Read(REC Type 포함)/Write X
		다만, Register Read/Write (RCU Restart 시, EEP 초기화 포함.) 은 가능하도록 수정.
		(2)L/S ID 할당 안된 상태에서 lock -> CPRI Unlock -> lock 이면, ID 할당 재요청 할 수 있도록.
	2. FAN(외장) 자동제어 시 DTU 온도 조건을 참조하도록 수정.
	3. Hidden_AID 0xB432 "SFP On/Off" 제어 안되는 현상 수정 : Register 0x07
	4. RU ID Acqu' Noti  &  Ack  완료 전에 Initalization Noti Ack를 수신하지 않도록 구현.
	5. (z.22.0)Optic LOS Detect/Alarm 구현 : AID : LOS(SFP) 0x5302 참조.  Alarm 발생 시 0xCC, 해제 시 0x33
	6. VSS Table 중 시스템 기능과 연관이 있는 항목들 중 시스템 안정성 확보를 위해 변경 Data 유지 확인 후 적용하는 기능	
		RE Channel BW : 100ms 주기 10회 연속(0x1~0x6 이외 값 무시)
		Loopback mode : 100ms 주기 5회 연속(0x33, 0xCC, 0xCA 값 외에 무시)
		TDD slot number[20] : 100ms 주기, 10회 연속
		CPRI Compress : 100ms 주기, 10회 연속
		C&M Path Selection : 100ms 주기, 10회 연속
		RE Cascade ID : 100ms 주기, 5회 연속
	

V06.21.03 : by YCSo
	1.	파형감시 필터 적용시 부분 필터 계수 적용.
	2.	다운로드시 새로운 F/S에 맞게 변경 부분 수정.( 아직 정식 적용은 아닌 상태)


V06.20.00
	1. REMS response : SUBID 0xFFFFFF 로 변경.



V06.19.01
	1.	PRU & AAU register read / write 부분의 address 값을 0x200값을 곱해서 GUI에서 편하게 조회
	2.	APP 실행 후, RCU APP version 정보 표시 
	3. 	아래 내용 다시 삭제.
	"2019.06.09 : 상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어 및 LD 알람 Masking"
	4. Clock Alarm 발생 시 Shutdown 알고리즘 수행
	5. CPRI Alarm 발생->해제 시 10초 조건을 CPRi#2, CPRI#3에도 동일하게 적용.
	//-----------------------------------------------



V06.18.05 : 2019.06.18
	1. MIMO Standby 시작 동작 조건 변경 : all amp 동작조건 만족할 때 동작하도록 수정.
	2. Alpha BW 가 0/10/20 의 경우에도 BW 가 자동으로 설정되도록 수정.
	3. RCU Temp Offset Added : AID 0xB417
	4. AID 0xB421 UL Att 제어시 표시 오류 수정 : 제어시, Gain Balance(AID 0x5209)가 적용 안된 부분 수정.	
	5. FPGA Register Read/Write : 
		- download 중이 아니어도 동작되게 수정.
		- cpri Unlock 이어도 동작 되게 수정.
	
	6. 초기 부팅 후 알람 보고 시점 : L/S에서 수신되는 Alarm Acknowledge == 0x01(Acknowledge)인 조건인 경우
		when Acknowledge = 0x01 ?
		(1) Init Noti 수신시 Alarm Acknowledge가 0x01(Acknowledge)인 경우.
		(2) Init Noti를 처음 요청한 시점부터 1분 경과 후, 자체적으로 0x01(Acknowledge) 변경

	7. CPRI/Clock Fail 알람 해제 조건 10초 추가/ 기타 알람 발생/해제 시간 변경 할 수 있도록 구조 변경(PRU/AAU 공통)	
	8. VSS 필드 기능 미구현 분 구현(PRU/AAU 공통)
    	(1) LOS/LOF OPTIC LOS 알람 구현 필요
    	(2) Function fail 구현


V06.17.00
	1. RVS OVER PWR Shutdown alarm이라고 payload off 하지 않도록 수정.

V06.14.00
	1. L/S Delay 에러 응답시 byte order 부분 수정 : result.sD32 = htonl(rst)
	2. VSS UL : RE_Loopbackmode_ACK 즉시 응답하도록 수정.


V06.13.05
	1.  T2a(DL_Delay), T3a(UL_Delay) 제어 값에 대한 예외 처리 추가 
	2.	Center Frequency 제어 기능을 추가하면서 PRU에 RE Channel BW에 대해 다음과 같이 구현 요청 드립니다.(~06/13)
		a.	VSS Table 값 따라가도록 복원
		b.	BW 변경시 Main/Alpha에 대한 Defalut Center Frequency 적용 및 저장

V06.11.01	
	1. 0x5209 Gain Balance : max 15dB limit added, 소수점 수신시 내림 처리
	2. MIMO Standby 진입/해제 level : 0x543A/3B 추가.
	
	*. AID 0x0001 : Alarm : 27byte x 2 (1st 27byte에 alarm suppression, 2nd 27byte에 alarm )

V06.10.03
	1. 구 A_ID 항목 변경(구 A_ID --> 신규 A_ID로 변경)
		 - Reset  0x5620
		 - 통계보고 기준시 0x5600
		 - 통계보고 주기  0x5601
		 - 통계 시작 시간 0x5602
	2. 삭제 : LOF(CPRI) 0x5303, LOS(SFP) 0x5302 Mea 삭제, LD(SFP) 0x5304 Mea 삭제	 
	3. UL GainBalance 제어/상태 수정.
	4. AID : D/L State 0x5510 추가.
	5. App 실행 시 consol 에 version 표시 형식 변경
	6. 0x5012 (함체온도), 0x5013 (함체온도상한) : 표시 오류 수정.


V06.09.00
	1. UL Gain Balance의 Att : UL Gain Back-off on/off 상태와 무관하게 값 적용.
	2. RSSI display 식 수정
		PRU 변경안 : UL 입력(GUI의 RSSI) = RSSI(Digital) - (37dB - UL Att- Back-off Att - Gain Balance Att)
	3. 상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어 및 LD 알람 Masking
		레지스터 기준으로 CPRI#0[#1] 알람 발생시 CPRI#2[#3]의 SFP Tx disable 제어

	4. 구 A_ID 항목 변경(구 A_ID --> 신규 A_ID로 변경)
	 - 제조사 0x0100 -> 제조사 0x5500
	 - 공급사 0x0101 -> 공급사 0x5501
	 - 중계기 Type 0x0102 -> Type 0x5502
	 - S/W 버전(통합) 0x0149 ->  S/W 버전 0x5503
	 - 함체 온도 0x0201 ->함체 온도 0x5012
	 - 함체 온도 상한 0x0202 -> 함체 온도 상한 0x5013
	 - Battery 유무 0x0210 -> Battery 유무 0x501B
	 - OSM 설치 유무 0x0249 -> OSM 설치 유무 0x501C
	 - Battery Status 0x027D -> Battery Status 0x501A
	 - RCU D/L State 0x2B30 -> D/L State 0x5510
	 - Reset 0x0914 -> Reset  0x7020
	 - 통계보고 기준시 0x55 0x00
	 - 통계보고 주기 0x55 0x01
	 - 통계 시작 시간 0x55 0x02
		
V06.04.00 :	///// YCSO
	1. 파형감시 필터 계수 변경. 8192
	2. cmd 0x99 추가 
		- Register_inquire 함수 및 관련 함수 추가.
	3. App 실행 시 version 표시 추가.
		- FPGA, DTU, PSU, PAU 0 ~ 4 등등의 version 표시.

V06.03.00 : 
	1. Gain Balance Added to VSS-Z.30 UL-Gain-Backoff
	2. LOS(SFP) AID 0x5302
	3. LD(SFP)	AID 0x5304
	4. Center Freq Control Added.(by User-Control)



v06.01.00
1. AID Only Added
	Capture_DL_Time	0xC023
	Capture_UL_Time	0xC024
	Capture_ULNoise_Time	0xC025
	ULNoise_RSSI_StartTime	0xC026
	ULNoise_RSSI_StopTime	0xC027
	UL_RSSI_StartTime	0xC028
	UL_RSSI_StopTime	0xC029



v05.30.00
	1. AID 0x500B(Center Freq) : 0.00.1 -> 0.001Mhz step 변경.
	2. PRU 전용 NCO Register added.
	3. AID 수정.
		- T2a(DL Delay) 0x5403 항목개수 수정  4 -> 1
		- Ta3(UL Delay) 0x5404 항목개수 수정  4 -> 1
	4. 항목 삭제 : NG-ARFCN 0x5007
	5. Gain 저하(UL) Threshold 0xB305 Data type: 0x04 -> 0x03 변경	
	6. UL Gain Balance 0x5209 : AID 만 추가.
	7. Offset(UL Noise) 0xB309 : same with 0xC05C AID
	8. LNA 입력(UL) 0xB2 0x1C 추가 : same with 0xC05A
	9. MIMO Standby Mode(진입/해제 Level) 0x543A/3B


v05.29.10
	1. "Proc__CPRI_Init_OSM_Ctrl" : 전부 삭제.


v05.29.00
	1. MAC/IP Set with IOCTL
	2. "Proc__CPRI_Init_OSM_Ctrl" 수정.
	3. loopback register control : 시간상 register 다음에 VSS-loopback 제어이나, 
	VSS-loopback 제어전에, register 를 먼저(대략 100ms) 제어되게 수정.

v05.28.14
	1. UDA Spurious 동작 조건 : "threshold (+/-)3dB" -> "over threhold level"


v05.28.13
	1. RVS 타사 과입력 level 변경.: -17.0 -> -47.0 

v05.28.12
	1. CPRI INIT : "__20190528_FIX__" added.
	2. TSSI Offset : +8.0 --> +5.2	
	3. RSSI Offset
		Filter 후 : +14.8dB
		Filter 전 : -29.9dB
	4. Noise Offset : -17.6dB
	5. 파형감시 : span conversion

v05.28.03
	1. t-sync offset 제어 추가.(GUI unit : [us] -> [ns])
	2. AID 0xC04E/4F : T2A/T3A delay (Read Only) 용도 변경.

v05.27.00
	1. AID 0xC05A : PAU LNA In Power added.
	2. AID 0xC05B : Offset(FILTER 전) added.(alpha is not needed..)
	3. AID 0xC05C : Offset(UL Noise) added.
	4. 연속적이지 않은 VSS RE_CH_BW 처리 : 연속 10회 일때, 정상적인 값으로 처리.
	5. When Cascade-SFP-Not-Insert : TBDelay, RoundTrip Delay : 0xFFFFFFFF 처리.
	

v05.25.00 // not release
	1. Temp Alarm : hysteresis 수정. : 2 -> 5
	2. L/S : IE_System_Sts : Batt insert 정보 : AAU(0:미사용), PRU(1:insert, 2:not insert) added.
	3. AMP-Standby, Mimo-Standby, Fan-Mode : 
		- 동작 중, 시간이나 mode(적용 path)를 변경하면, mode off 제어되게 수정.


v05.24.03
	1. 앞의 PAU가 ALC on 이면, 이후, amp rvs atten 제어 안되는 부분 수정.
	(0xB224 (PAU RVS ATT) : ALC on 시, atten 제어 안할 때, break 대신 continue 수정.)
	2. PAU : RVS ATT : RCU Reset 초기 제어 되도록 수정.
	3. AID 0x5405 : round-trip-delay : 상태응답 flag 수정 : 0xC0 -> 0xF0
	4. FPGA : T2A delay, TA3 delay 적용 수정.
	5. UL_GAIN_Measure_Time : AID 없으므로, RCU Reset 시, 초기 제어 하지 않도록 함.
	

v05.23.0 // not release
	1. NOKIA L/S 연동 Added.
	2. register : "T2A delay", "TA3 Delay" added.

-------------------------------------------------------------

v05.22.05
	1. pkg download 수정

v05.22.04
	1. pkg download 수정 사항 merge.

v05.22.03
	1. CPRI init 원복

v05.22.02
	1. "Insert_GUI_IP_Table" bug fix
	


v05.22.00
	1. L/S Alarm Noti, Alarm Status : structure initial
	memset(s_data, 0, sizeof(ST_ALM)*100)
	2. L/S Alarm Noti : SFP LOS/LD Alarm : Send 후, alarm 저장하지 않아서 무한 send 하는 것 수정.
	기타...
	-. cpri alarm : 2sec 유지.
	-. CPRI init algorithm : OSM switch 만 제어하도록 요청

v05.21.16 : 
	v05.21.15 에서 download merge 만 제거.	

v05.21.15 :
	1. package downloatd 수정.(merge)
	2. cpri alarm : 2sec 유지.
	3. CPRI init algorithm : OSM switch 만 제어하도록 요청

v05.21.14 : CPRI init added -- reset register 잘못 사용하는 것 수정.

v05.21.13 -- XXXXX 문제이 있는 버전.-------------
	1. AID 0x5403/04 (T2a/Ta3 delay): GUI - Control 가능하게 수정.
	2. CPRI init algorithm
	3. spurious alarm : AID 0x544C (UDA_Spurious_Layer)를 사용하도록 수정.


v05.21.12
	1. TSSI offset : PRU : +8.0dB fixed.
	2. dpd algorithm added.
	3. DPD_Eng_In__Pwr, DPD_FB_Eng_In_Pwr : log 계산식 추가.
	

v05.21.10
	1. register : "SFP LOS Alarm","SFP LD Alarm" added

v05.21.08
	1. "__CPRI_SET_When_Reboot__" deleted (단, "Optic SWITCH ON" 제외)
	

v05.21.07
	1. "__CPRI_SET_When_Reboot__" deleted

v05.21.05
	1. tree
	2. "__CPRI_SET_When_Reboot__" added

v05.21.00
	1. L/S : Samsung IP Save.
	2. "Proc___PRU_before_OpticSw_ON" 삭제.
	   "MMAP____SW_Reset" 삭제.


v05.20.16
	1. LS ID Added : Samsung/Ericson(Nokia) : Base IP 에서 LS ID를 추출.	
	2. 	Hidden (0xb634)의 'batt status 3 (H/W Fault)'가 발생 시에,
		'Battery Status'의 Reason : '전압이상'  --> 'Battery Fault(HW)' 으로 수정 
		
	3. Temp Alarm : hysteresis 수정. : 5 -> 2
	4. TSSI upper Alarm : hysteresis 수정. : 2.0 -> 2.1
	5. Lock 판단 조건에 miscelaneous added.
		CPRI_0/1) if(CPRI_Mscl_16 == 0) CPRI_Mscl_Lock = 1;
		CPRU_2/3) if(CPRI_Mscl_16 == 0x0080) CPRI_Mscl_Lock = 1;
		
	6. register "DPD Successful Count" added.
	7. AID Change by Register Map
	   - m_St.d_DPD_St : 0xB405 -> 0xC032
	   - m_St.d_DPD_Err_St : 0xC032 -> 0xB405
	8. DPD 항목 추가.	
		DPD PeakExpansion
		DPD MSE
		DPD PRE MSE
		DPD LUT Value
		DPD Count
	9. "VSS table reload" 삭제.
	10. CRPI Lock : 100M, Alpha 모두 보도록 수정.
	11. "__EXCEPT_Alpha_at_CPRI_LOCK__" 삭제.
	12. "__USE_Spectrum_LOW_Limit__" : spectrum filer 적용된 값의 하한값 적용(-50.0)
	13. PAU : 0xB222/23/26 : temp att : signed 변경.
	14. PAU : VSWR : PAU 받은 대로 표시.
	
	

	


v05.18.02
	1. RVS 타사 과입력 ALC : Level -10.0[dBm] --> -17.0[dBm]

v05.18.01
	1. RVS Spurious Test 확인.
	2. CPRI1(alpha) not insert 시, BW 10M 제어.
	3. RVS 타사 과입력 ALC : 간이 Test 확인.
	4. Clock Alarm 발생 시에 Reg 0x07번지 로그로 출력 


v05.17.03
1. spectrum
	- "__SPECTRUM__OFFSET__" : Defined
	- 파형감시 filter 적용방식 변경.
	- filter : 계산식 부호 변경.
2. UDP download 속도 개선.
3. Vss_UL_RE_Reset_Ack 응답속도 개선. : "__USE_VSS_UL_RE_RESET_ACK_SW__"
4. PRU : 노란색 Debug message를 "VSS table reload"
	PRU의 경우 CPRI 연결후 간혹 VSS 테이블이 이상하게 읽히는 경우
	Open CPRI Version == 0x00 or 0xFF 상태가 2초간 유지 시, 
	PS Register 0x09번지에 20번 bit를 0으로 제어후 1로 제어
5. AID
	LS_ID[MAX_Optic](0x500C) ; ??
	alarm 변경.(항목만 추가)	
		LOS(SFP0)      (OPTIC_TRANSCEIVER_RX_LOS)	3	0x5302
		LOS(SFP0)(+α) (OPTIC_TRANSCEIVER_RX_LOS)	2	0x5302
		LOS(SFP1)      (OPTIC_TRANSCEIVER_RX_LOS)	1	0x5302
		LOS(SFP1)(+α) (OPTIC_TRANSCEIVER_RX_LOS)	0	0x5302
		LD(SFP0)       (OPTIC_TRANSCEIVER_TX_FAULT)	7	0x5304
		LD(SFP0)(+α)  (OPTIC_TRANSCEIVER_TX_FAULT)	6	0x5304
		LD(SFP1)       (OPTIC_TRANSCEIVER_TX_FAULT)	5	0x5304
		LD(SFP1)(+α)  (OPTIC_TRANSCEIVER_TX_FAULT)	4	0x5304


6. 
 - Toffset 0x5400 삭제, 히든 이동(0xB42B)
 - TBdelayDL(DL Hopping Delay) 0x5401 삭제, 히든 이동(0xB42C)
 - TBdelayUL(UL Hopping Delay) 0x5402 삭제, 히든 이동(0xB42D)
7. DPD Status(DL) 0xB405 LEN 변경 1 -> 2
8. Gain 저하(UL) Threshold  0xB305 추가
9. L/S : "OPTIC_TRANSCEIVER_RX_LOS", "OPTIC_TRANSCEIVER_TX_FAULT" Alarm added.
10. Battery Monitor : test time  -> 원래 시간으로 원복.







v05.16.02
	1. Download 이어받기.
	2. 파형감시 수정 : 속도 개선 중.
	3. tv.tv_usec = 10000; // 10[ms] 2019.05.16


v05.16.00
	1. TDD Sync (AID 0xC021) : register conversion 수정.( GUI에서 usec 단위로 제어하는 것으로 가정.)
	2. FPGA User Delay (AID 0xB429) 혹은 L/S로 인한 FGPA Delay 제어시, TDD Sync (AID 0xC021) 되도록 수정.
	3. loopback Mode : Response-Debug-Time is added in the console.
	4. DL CW Level, Onoff : Level EEP Save, CW Off when reset.


v05.15.04 (AAU)
1. TDD Sync
 기존
 [Reg_TDD_RW_UL_ON_Guard_Time]    <-- DL_to_UL_Guard_Time
 [Reg_TDD_RW_DL_to_UL_Guard_Time] <-- UL_ON_Guard_Time
 변경
 [Reg_TDD_RW_UL_ON_Guard_Time]    <-- UL_ON_Guard_Time
 [Reg_TDD_RW_DL_to_UL_Guard_Time] <-- DL_to_UL_Guard_Time

2. power offset added.
	(DL출력 FB)	0xC051, (DL TSSI) 0xC052
3. AAU RVS Shutdown : off time : 5[sec] --> 10[sec]
4. AAU FWD Shutdown : DL_RSSI_FB total power로 shutdown 되도록 수정.
5. Check and Get "Current MAC/IP" Every 3sec
6. UL-Gain-Backoff onoff : VSS-DL
 cascade ID 0,1,2,3 중, 
 cascade ID 1,2,3 의 경우, DL-CRPI0/1로 받은 'UL-Gain-Backoff onoff' 를 
 DL-CRPI2/3 으로 copy 하도록 수정.


v05.15.04 (PRU)	
1. UL-Gain-Backoff onoff : VSS-DL
	cascade ID 0,1,2,3 중, 
	cascade ID 1,2,3 의 경우, DL-CRPI0/1로 받은 'UL-Gain-Backoff onoff' 를 
	DL-CRPI2/3 으로 copy 하도록 수정.
2. Check and Get "Current MAC/IP" Every 3sec
3. DPD Alarm Algorithm 수정.


-------------------------------------------------------------
/////v28.00.04(AAU) : 2019.05.14.- not release
	1. AID 0xB007, 0xB008 : GUI 에서 제어시, null 다음 character 오더라도 적용 안 되도록 수정.
	2. CPRI Unlock 시, 
	- VSS 읽는 것도 하지 않도록 수정.
	- VSS_내용 Copy하지 않도록 수정.
	- VSS 관련 Register Write 하지 않도록 수정.
	- L/S : Auto_Config_Task 동작 안하게 수정.
	- Console 에 unlock message 표시(10초마다)


v26.00.04(PRU) : 2019.05.14.
	1.	CPRI의 DL/UL BW Register Control
	* when (SFP Mount) && (CPRI0/1 알람) : CPRI0/1 BW Register를 0x01(10M)로 제어,
	* when normal : VSS Table에 있는 값으로 다시 제어(AAU는 User Control)
	2. RE Channel BW : VSS 따라가도록 수정.(PRU), (AAU도 VSS 따라가고 있음)
	3. UL RSSI 하한 alarm 삭제 
	4. UDA_RVS Spurious : test time(payload off 10[min] -> 1[min]
	   AID 0xB306 : RSSI(UL Noise) added (Register added too)
	5. UL-Gain-Backoff : VSS-DL Control : Cacade_ID에 관계없이 하위 CPRI2/3 control
	

v28.00.04(PRU) : 2019.05.14.
	1. AID 0xB007, 0xB008 : GUI 에서 제어시, null 다음 character 오더라도 적용 안 되도록 수정.
	2. PRU Only : CPRI-Unlock check를 100M 만 하도록 수정.
	3. CPRI Unlock 시, 
	- VSS 읽는 것도 하지 않도록 수정.
	- VSS_내용 Copy하지 않도록 수정.
	- VSS 관련 Register Write 하지 않도록 수정.
	- L/S : Auto_Config_Task 동작 안하게 수정.
	- Console 에 unlock message 표시(10초마다)
	4. PRU : "AC_Fail" mask 하던 것 삭제.(원래대로 원복)
-------------------------------------------------------------



2019.05.13. v29.00.04(AAU) : 
	1. PRU : UL-Gain-Backoff : VSS-Z.30.0 추가.
		* when cascade-id == 0, after user-control, Control VSS-DL-CPRI2/3-VSS-Z.30.0
		* when cascade-id == 1~3, Read VSS-DL-CPRI0[ or CPRI1].
		** gain backoff 하는 부분수정안됨(현재 RF ATT 이고, Digital ATT으로 변경해야 함.)
	2. AAU_FAN_MODE 기능 활성화.
	3. PRU : "AC_Fail" mask.

2019.05.13. v29.00.04(PRU) : 
	1. PRU : UL-Gain-Backoff : VSS-Z.30.0 추가.
		* when cascade-id == 0, after user-control, Control VSS-DL-CPRI2/3-VSS-Z.30.0
		* when cascade-id == 1~3, Read VSS-DL-CPRI0[ or CPRI1].
	2. PRU : "AC_Fail" mask.
	
2019.05.13. v30.00.04(PRU) : TEST Version
	1. RE Channel BW를 User 제어가 아닌, L/S 에서 제어하는 BW 값을 따락 가도록 일시적으로 수정.
	("__Use_USER_RE_Channel_BW_CTRL__")
	2. PRU : "AC_Fail" mask.

2019.05.13. v31.00.04(AAU,PRU) :
	1. AAU/PRU : LayerUseInfo(AID 0x5002) GUI-Status-Response 시, flag 수정.(공용 GUI에서만 상태 표시 안되는 부분 수정.)
	2. PRU : DTU DL/UL Atten 초기 제어(RCU Re-Start 시) 안되는 부분 수정.
	3. PRU/AAU : FPGA Delay : AID (0xB429/2A) 제어 안되는 것 수정.
	4. PRU : "AC_Fail" mask.


2019.05.13. v32.00.04(AAU,PRU) :
	1. AAU  : "AFE_ALC_Level" added. (AID 0xC04A)
	2. AAU  : AID Added
		ALC On/Off(DL)	0xC04B
		ALC Level(DL)	0xC04C
		Shutdown Level(DL)	0xC04D
		Shutdown On/Off(DL)	0xC050
		
	3. AAU/PRU : AID Added	
		system Offset(DTU Delay DL)	0xC04E
		system Offset(DTU Delay UL)	0xC04F
	4. PRU : "__NOT_USE_AC_Fail__" : AC Fail Mask
	

2019.05.11. v33.00.04(AAU) :
	1. AAU  : Use 'DL_FB_RSSI' as calculation power (AID 0x5108)
	2. AAU  : Use 'DL_FB_RSSI total power' as outpu power (AID 0x5103)
	3. RSSI Offset 적용.
	4. AAU : "FWD-ALC" algorithm added.


2019.05.10. v33.00.04(PRU,AAU) :
	1. AAU Inventory : DTU use RCU's S/N and Date(RCU share S/N and Date with DTU)
	2. L/S 연동 protocol 수정.
		a. Alarm IE의 Number of IF, Number of Len 정보 추가
		b. Delay Value IE의 Number of CPRI, Number of IF, Number of Len 정보 추가
		c. Optic Delay IE의 Number of CPRI, Number of IF, Number of Len 정보 추가
		d. TSSI/RSSI IE의 Number of FA/Path, Number of IF, Number of Len 정보 추가
		e. Path IE의 Number of Path, Number of IF, Number of Len 정보 추가
	3. AAU : 파형 감심 : I/Q를 서로 뒤바꿔서 출력 함. 
	4. Offset(RSSI)	0xB307 적용 및 RSSI Total Power 적용.

2019.05.09. v34.00.04(PRU,AAU) : 	  
	1. PRU와 AAU의 SIM 기능이 추가
	2. AAU table이 정리
	3.	Frame의 size가 34Kbyte로 변경
	4.	TCP 통신하는 부분이 추가 

2019.05.09. v34.00.04(AAU) : 
	1. AID 0xB417 : "Offset(온도)"
	2. AAU 
		- ATTEN(LNA ALC)(UL) (0xB225) Added.
		- ATTEN(PAU)(DL)( 0xB220) added : "ATTEN(PAU)(DL)" : 0x5106 DL-ATTEN을 연결.
	    - ATTEN(PAU 온도보상)(DL)(0xB222) : AID만 추가. 온도보상은 추가 필요.
	    - ATTEN(LNA 온도보상)(UL)(0xB223) : AID만 추가. 온도보상은 추가 필요.
	3. PAU : Offset(PAU 온도) 0xB217 : added.

2019.05.09. v39.00.04(PRU/AAU) : 
	1. AID 0x5208 'UL_Gain_Backoff' : AID 0xB426 그대로 표시.
	2. AID 0x5315 'SFP 탈실장' : AID 0xB43F 그대로 표시.
	3. AID 0x5400 "Toffset" : AID 변경.
	4. AID 0xB308 "Threshold(UL 불요파)" Added.
	5. AID 0xB429/2A "Delay 설정값(FPGA)(DL)", "Delay 설정값(FPGA)(UL)" added.
	6. AID 0x530A : BIP 누적 count -> instant value
	   AID 0xB427 : "BIP 누적 count" 추가.	
	7. AID 0xB428 : "BIP Error 누적 Count 초기화" Added. 	
	8. Hard-Clipping Count
		AID 0x5320 : Hard-Clipping 누적 count --> Hard-Clipping Count
		AID 0xB40E : Hard-Clipping Count      --> Hard-Clipping 누적 count
	9. AID 0x544C : "UDA_불요파_동작 Layer" 표현 방법 수정 (100, +α분리)	


2019.05.08. v40.00.04(PRU) : Samsung L/S 연동 v41.00.04 merge

2019.05.08. v43.00.04(PRU)
	1. Register "UL ALC GAIN" : PRU RVS 타사 과입력 GAIN 보상으로 사용.
	2. Register "DL_GAIN" : Offset ATTEN으로 사용, 0.1dB ATTEN (AAU와 사용법 다름)
	3. Register "UL_GAIN" : Offset ATTEN + UL_Gain_Backoff로 사용, 0.1dB ATTEN (AAU와 사용법 다름)
	4. UL Gain_Backoff (AID 0xB426) Added.
	5. PRU RVS 타사 과입력 GAIN 보상.
	6. AAU : LowNoise Fan Mode Added.

2019.05.07. v44.00.04(PRU)
	1. L/S 관련 수정.


2019.05.07. v47.00.04(PRU)
	1. UDA Test Version.
	2. UL Casacade on/off (AID 0xB435) Added.
	3. L/S : status response 중, alarm noti 의 경우, packet 오염 수정.
	4. Measured_Power_IE 삭제 : delete "Make_Frame_IE_Measured_Power()"
	5. serial TX_data 부분 수정.

2019.05.07. v45.00.04(AAU)
	1. 타사 과입력
	2. UL Casacade on/off (AID 0xB435) Added.
	3. L/S : status response 중, alarm noti 의 경우, packet 오염 수정.
	4. Measured_Power_IE 삭제 : delete "Make_Frame_IE_Measured_Power()"


2019.05.06. v48.00.04(PRU)
	1. 타사 과입력
	2. UL Gain_Backoff (AID 0xB426) Added.
	3. 0xB430 : Test Pattern On/Off Added.


2019.05.04. v47.00.04(AAU)
	1. 타사 과입력, RVS ALC 수정.
	2. UL Gain_Backoff (AID 0xB426) Added.


2019.05.03. v49.00.04(PRU, AAU)
	1. UDA에 의한 Reset 시, Reset Reason을 "기타"로 수정.
	2. PRU : UL_IN_RSSI (Register) Added.

	m_St.d_Layer_Use[m] = m_Reg_Sts.RFU[m].LayerInfo.Use; 
	pReg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_Layer_Info][idx] = &pDtuReg->RFU[m].LayerInfo.Data;


2019.05.02. v50.00.04(PRU, AAU)
	1. Layer Splitter : "IE_Optic_Delay"에서, 하위 CPRI not insert 시, 
	 roundtrip 0xFFFFFFFF, Hopping Delay(tbDelay) DL/UL 0x0000FFFF로 전송.
	2. L/S : Alarm Noti에 대한 Alarm ACK를 무시하도록 함.
	3. AID 0x5403/04 : FPGA Delay(DL/UL) : FPGA read value 아닌, L/S에서 받은 값 표시로 수정.


2019.05.01. v53.00.04(PRU, AAU)
	1. PRU : AID 0xB411 : DPD Reset added.
	2. Test-Alarm Added.
	3. Alarm-mask Added ( When GUI-Control )
	4. VSD cascade ID : 100M에서 받은 Cascade-ID-ACK를 Alpha Mhs 에도 ACK 하도록 수정.
	(그 반대의 경우도)
	5. VSS : Loopback mode : alpha Mhz 부분도 추가.
	6. FWD Output calculation(AID 0x5108) : SFP(100M/Alpha)  Insert여부에 따른 표시 내용 추가.
	7. Layer Splitter : Alarm Status Response 에 "Temperature high alarm" Added.


2019.04.30. v54.00.04(PRU, AAU)
	1. AAU : DTU Invetory Register added. 
	2. System offset Delay added
		#define DL_SYSTEM_Offset_ns  (8340) //  8.34[us]
		#define UL_SYSTEM_Offset_ns (13230) // 13.23[us]
		FPGA Delay Set = L/S_Set_Value - (system_offset + delay_offset)
	3. LS : "Path Status Request/Response" added
	4. LS : "FA Status Request/Response" added
	5. RE_Reset에 대해, VSS UL ACK 하도록 수정.
	6. PRU : RE_RESET 되도록 comment 해제.

2019.04.29. v55.00.04(PRU)
	1. Proc__SuperCap_AC_Alarm_Report : 원복.


2019.04.29. v56.00.04(PRU)
	1. "Proc__SuperCap_AC_Alarm_Report" 동작 안되는 Test Version.

2019.04.29. v57.00.04(PRU,AAU)
	1. Inventory AAU-FPGA Added.
	2. output power calculation : AID 0x5108
	3. AAU : serial console 출력에 SPI log 삭제.
	4. AAU : LED 제어 일부 수정.
	5. AAU : 출력하한은 -100.0dB 로 변경.(이전 버전에서도 이미 적용.)
	6. PRU : Hard clipping/ BIP Error Counter 기능 다시 활성화 : __TEST__Do_Not_This__ // 2019.04.29


2019.04.26. v58.00.04(PRU)
	1. Register TSSI_Shutdown Status added. ( flag == 1 이면, off제어 하지 않음.)
	2. "PAU_EN_Ctrl" 에서, PAU algorithm 에 의한 fwd/rvs off 이면, ON제어는 내려가지 않도록 수정.
	


2019.04.25. v59.00.04(PRU)
- spectrum, package download log added.
- PAU AMP Reason 에 따라, DTU DL/UL Off
- LED alarm 수정,
- wavelength display 수정.


2019.04.25. v60.00.04(PRU, AAU)
 - clock fail로 인한 DTU payload off, PAU off 삭제.
 - PAU Reason에서 user 제어에 의한 부분은 제외.
  

2019.04.25. v65.00.04(PRU, AAU)
	1. d_Hard_Clip_Lvl_x10 : for loop 2 --> 4 변경.
	2. RED_LED ON Control When cpri alarm.
	3. UL RSSI Calculation : UL_RSSI(기존) - 37.0dB + DTU_UL_Atten


2019.04.25. v67.00.04(PRU, AAU)
- PRU에서, HARD Clipping, BIP 동작 중지.


2019.04.25. v68.00.04(PRU, AAU)
	1. "CPRI Alm" DL/UL path off, PAU Off ( SFP-0/SFP-1 설치여부 에 따라.)
	2. AID 0x5301, CPRI Reason Added.
	3. UL RSSI Calculation : UL_RSSI(기존) - 37.0dB - DTU_UL_Atten
	4. SuperCap_AC_Alarm_Report 시, Batt_Insert 정보 조건 추가.
	5. Hard Clipping Level 제어 추가.(PRU) : 8.0 ~ 14.0 dB range.
	6. Wavelength AID 변경. AID 0x5312.
	7. SFP not insert, PAU not insert : Inventory 0x7F
	8. Battery Extension : SISO/MIMO mode 모두 동작 하도록 수정.











	

2019.04.24. v70.00.04(PRU, AAU)
- AAU : DVT 정리용 release
- YCSo merge : spectrum 등. (package download, USB GUI ...)

 

2019.04.24. v75.00.04(AAU)
- AAU : DVT 정리용 release

2019.04.24. v79.00.04(PRU, AAU)
- ALC RVS : layer#2,#3,#4 : comment 해제.
- AAU is "MIMO Mode" Only		


2019.04.24. v80.00.04(PRU, AAU)
- ALC RVS (Quick)
- Hard Clipping Test

2019.04.23. v83.00.04(PRU, AAU)
- hard clipping added.
- BIP counter added.
- AMP-OFF 시, DTU DL/UL Off (단, PRU 이고, MIMO-Standby-Mode는 제외)


2019.04.22. v84.00.04(PRU, AAU)- not release
- MIMO-Mode 동작상태의 경우 : SISO-AMP Off 되어 있으면, Batt.Extension 동작 하지 않도록.
- SISO-Mode 동작상태의 경우 : MIMO-Standby 동작 하지 않도록 (MIMO-Standby 동작상태는 Off.)


2019.04.22. v85.00.04(PRU, AAU)
- AID 0x500B Center freq added.
- AAU : Fwd shutdown added.



2019.04.20. v86.00.04(PRU, AAU)
- "FPGA_RdWr_Sem" added
- AAU, 'Mscl_Status' bit 변경.
- Make_Reg_Addr_AAU_Axi 변경.




2019.04.20. v87.00.04(PRU, AAU)
- RVS ALC



2019.04.19. v88.00.04(PRU, AAU)
- AAU RVS ALC : Test needed
- if(Reg_ALC_flag[m][r] == 0) continue; // 2019.04.20 

2019.04.19. v91.00.04(PRU, AAU)
AID 0xB431 : Payload On/Off, AID size 변경. 4x1 -> 4x2



2019.04.18. v93.00.04(PRU, AAU)
- payload on/off : reset 시 초기화 하도록 수정.
- CW On/Off 제어/상태 추가.


2019.04.18. v98.00.04(PRU, AAU)
- layer splitter : 
	1.Delay Set 명령 수신시 AID 항목 FPGA Delay(DL)-0x5403, FPGA Delay(UL)-0x5404  상태/제어 반영(04/18)
	2.프로토콜 v0.8 기준 지원되는 알람 항목에 대한 Alarm Noti 구현(04/19)
- PSU 전력량 AID 추가.
- cascade ID : AID 0x5001 : data type 변경. 0x04 -> 0x02


2019.04.18. v11.00.02(PRU, AAU)
- AID 0x5009 : "System 형상" deleted.
- AID 0x5206 : UL_Att added (ANTS PRU Only, register)




2019.04.17. v12.00.02(PRU, AAU)
- package download 후, psu_reset 이 되도록 수정. m_Env.psu_reset_req = 1;
- DL/UL Guard time 관련 : PRU/AAU Register 11bit -> 12bit 변경.

- PRU : 
(1) "FAN 저소음Mode" : when (start-time == stop-stop), AID 0x5411 : "저소음 Mode" 표시
(2) FAN 저소음 모드 : PWM  온도 table 변경.
(2) DTU LED 제어에서, GUI, Door는 제외 하도록 수정.
(3) AID 0x5216 : LNA-on/off added. (PAU-RVS-AMP-Onoff 에 연결.)

- AAU : 
(1) GUI Main 
	(AID DL/UL Atten : RF ATTEN 수정.)
	(AMP-hidden LNA Atten : 연결 추가.)
(2) AID 0x5216 : LNA-on/off 에 대한 AAU-register는 없음.
(3) DL/UL Guard time 관련 : PRU와 달리 각각 제어.
--------------------------------------------------------------------







2019.04.16. v13.00.02(PRU, AAU)
- Batt.Extension, MIMO Standby : MIMO Mode일때만 동작하도록 수정.
- when "Batt.Ext.Mode == OFF" && "AC 알람" : mode 동작 하지 않도록 수정.
- Comm TX 에서, size check "MAX_COM_BUFFER" added. (size 4096 -> 4096x3))
- Layer mode AID 0x5433 : AID 변경.
- Battery 유무 AID	0x0210 (0x07 : 외장(50v))
- LS 제조사 AID 0x500A added.
- main LNA ON OFF 적용.
- USB/Bluetooth : tx size 4160 넘을 때, 통신 안 되는 부분 수정.
- AAU : S/N을 재 설정하면 Bluetooth가 re init 되면서 S/N을 가진 이름으로 비콘을 날림.
           B/T로 통신 중일 때 LED가 깜박거리도록 수정.

-SC.So Main AID merged.

- below AID : not used AID
LOS(CPRI)	0x5302
LOF(CPRI)	0x5303
LD(CPRI)	0x5304





2019.04.16. v14.00.02(PRU, AAU)
- merge version


2019.04.15. v15.00.02(PRU, AAU)
- AID 0xB437/38 : DTU Volgate (Main/Sub) Added
- FAN 저소음 모드 : (시작시간==종료시간) 항시 저소음 모드 On 되도록 수정.
- Bluetooth_re_Init (AAU)



2019.04.13. v17.00.02(PRU)
- correct "m_St.d_VSS_Z__60__Rec_PortNum[c][DL]" 
- Clock 장애 알람 : Shutdown 알고리즘
- CPRI 장애	알람 : Shutdown 알고리즘
- TSSI 입력 상한 : Shutdown 알고리즘
- LED 동작 added
- Super cap. 활용 AC 알람보고


2019.04.11. v18.00.02(PRU)
- when CPRI-Lock normal : copy "VSS Register TDD Slot number 0~19" to "slot #0 ~19"
- DL Off Guard Time <-> DL to UL Guard Time
- DL On Guard Time <-> UL to DL Guard Time
- AID 0x5420 (TDD_Mode) 에 FDD 항목 제어되도록 수정.

2019.04.11. v19.00.02(PRU), v20.00.02(AAU) : layer splitter 관련 수정.(Nokia 연동 부분 추가)

2019.04.10. v20.00.02(AAU, PRU)
- VSS Channel : Copy 
	DL(CPRI0/1) -> DL(CPRI2/3), UL(CPRI2/3) -> UL(CPRI0/1)
- loopback mode
- RE Reset Added : 
	PSU Power Reset 되도록 수정 및 DL(CPRI2/3) 으로 VSS (RE Reset) 전달.


2019.04.10. v22.00.02(AAU, PRU)
- DL/UL gain (0xC015/15) <--> DL/UL atten (0xB420/21) control/status added.

2019.04.10. v23.00.02(AAU, PRU)
- AC Fail
- watchdog time 10 -> 5 sec

2019.04.10. v24.00.02(AAU, PRU)
- AID  : 0x540A(EQ_Coeff), 0x540B(EQ_Coeff) 다른 값으로 변경.
- RE-Channel BW : user control 따라 가도록 수정.

2019.04.09. v25.00.02(AAU, PRU)
- cpri lock 후, initial setting 및 VSS write 하도록 수정.

2019.04.09. v27.00.02(AAU, PRU)
- "RE Channel BW" 변경 시, center freq 변경 되도록 수정.

2019.04.08. v28.00.02(AAU, PRU)
- RCU download *.RSM header 변경.


2019.04.08. v29.00.02(AAU, PRU)
- "Reg_Common_RW_DTU_LED" added.
- ARFCN calculation is Added. (AID 0x5007), AID 0xB41F deleted.
- register "PAU DL Attn Sync" 밀려서 제어 되는 것 수정.
- AID 0x05409 Center freq 삭제.(상태 /제어, same with AID 0xB412)
- cpri lock check 부분 수정.
	A_PRU_Status_Update_Mmap();
	A_AAU_Status_Update_Axi();





2019.04.06. v30.00.02(AAU, PRU)
- fpga delay(AID 0x5403/04) fpga address 변경, delay offset (AID 0xB415/16) 수정.
- AID 0x5106 PAU DL ATTEN 제어시 Register('PAU_DL_Attn_Sync') control added (ANTS PRU Only)
- inventory inquery added (control will be added later...)

2019.04.06. v36.00.02(AAU, PRU)
- package download 수정.
- ftp download added


2019.04.06. v37.00.02(AAU, PRU)
- AID 0x5206 UL ATT : digital 이나 ANTS-PRU 의 경우, 일시적으로 hidden atten AID 0xB224 와 동일하게.
- AID 0xB220 : PRU-PAU : TPL Atten 제어 부분 수정.
- AID 0xB224 : PRU-PAU : PAU RVS Atten 제어 : 0.5dB step 으로 수정.
- if Batt. not insert, mask-alarm of battery-low(bit6)
- temperature alarm hysteresis : bug fixed.
- AID 0xB419 (d_CPRI_Recovery_Path_St) : "CPRI Recovery Path Status" added
- inventory inquery added (control will be added later...)
- bar-code inquery/control added.

2019.04.06. v38.00.02(AAU, PRU)
- Test Port 출력 : 0x543F : F/W restart 시, off 로 시작되도록 수정
- System 형상 AID 0x5009 : added default 0:4T4R
- PSU_Reset Added
		1. FPGA Regiseter Control : 'PSU Reset'
		1. system("halt");
		1. for 'halt', RCU termination
		2. 5[s] delay after 'halt', ANTS-PRU-FPGA will do "PSU-RESET"


2019.04.06. v40.00.02(AAU, PRU)
- Battery Extension Mode (On/Off)	0x5438
- Battery Extension Mode 동작 상태	0x5439
- pkg download fix when AAU-pakage download
- when AC-Fail, do not send 'PSU_CMD_BattCharge_Ctrl'


2019.04.05. v50.00.02(AAU, PRU)
- package download added
- 

2019.04.04. v59.00.02(AAU, PRU)
- Nokia 연동 위한 AAU : layer splitter 수정.
- AAU V00.00.03 에서 수정된 AAU F/W를 V60.00.02에 merge)

2019.04.04. v60.00.02(AAU, PRU)
- VSS - REFunctionFail : Nokia 연동 위해, 일시적으로 0xCC 를 전송하도록 수정.
- MIMO Standby added ( siso layer mode 제외)
- AAU : RUN LED added.

2019.04.03. v61.00.02(PRU)
- FPGA Temp calculation added.
- SFP insert change : 
	SFP Insert = fpga register '0' = AID 0xB43F (mount is '0')


2019.04.03. v61.00.02(AAU)
- AID 0x5002 : LayerUseInfo : eeprom init is "use"
- AAU : Ext FAN Algorithm added according to temperature



2019.04.03. v62.00.02(AAU)
- layer 사용정보 부분 관련 수정: control시, status 값에도 반영하도록 .. 등.


2019.04.02. v63.00.02
- amp standby : basic operation : on/off according to time
- cpri alarm added.
- cascade ID : cpri2/3 에 1 증가.
- SFP (CPRI0, CPRI1) insert 에 따라 alarm mask 추가.
- Hard Clipping Count(DL) 0x5320 2 -> 1변경

2019.04.02. v64.00.02
2019.04.01. v65.00.02
- ether path selection, VSS-C&M 수정


2019.04.01. v66.00.02
- AAU : CRPI Alarm added.
- Vss : Alive ACK (CPRI0/1 UL) added : 0x33/0xCC
- bluetooth LED : Red/Blue : high <-> low, change


2019.04.01. v67.00.02
- AID 0x5106, 0x5206 : Atten 0.1_dB_Step (excel list)
- AID 0x5313 : register read 값 그대로 GUI 에. (excel list)
- AID 0xB618 : max RPM 제어/상태 수정. (excel list)
- mmap : cpri alarm bit 수정.
- nokia LS 연동 기능 수정 중.









2019.03.29. v77.00.02
- PRU power-reset added : check needed for io-pin
- AID 0x5106, 0x5206 : dl atten, ul atten, 0.01 -> 0.1 setp
- Fan mode 알고리즘 추가.
- Batt extension added
- PSU FAN RPM(AID 0xB616) 미지원으로 On/Off 변경 건
	RPM '0' = Off, RPM '100' = On 처리 
	(GUI에서 On 시 '100' 값으로 제어, Off시 '0'값으로 제어, Display 동일)
- AID 0x530A : BIP_Count --> BIP_ErrCnt
- PAU Reset(DL) 0x5119 added.
- Center Frequency 	0xB412 added
- CPRI	0x5301 added



2019.03.29. v78.00.02
- excel list document 


2019.03.28. v79.00.02
VSS Channel processing added

2019.03.27. v80.00.02
SFP_BitRate added.


2019.03.27. v81.00.02
- LS response 수정.
1.	System Status Response(H'1000 0011) Return Result 필드의 값이 "0x11(17)"으로 확인됩니다.
일단은 default로 "0x00 Normal Case" 올려주세요.

2.	CPRI Status Response(H'1000 001D)
 	해당 프로토콜의 Element중에 "Optic Delay IE(H'0034)"로 응답하도록 수정.


2019.03.26. v84.00.02
- OSM 설치 유무	0x0249 Added.
- Channel Bandwidth   0x5008
- DL 출력(연산) 0x5108


2019.03.25. v85.00.02
	- PRU-PAU H/W insert 처리 추가.: not insert 시, layer 미사용.(AID 0x5002)
	 H/W insert 시, user control에 따라, 표시
	 layer 미사용.(AID 0x5002) 시, alarm mask

	 PAU link fail added.

2019.03.25. v87.00.02
	- Layersplitter 연동 : port 별 응답 처리 추가.

2019.03.22. v90.00.02
	- GUI : udp comm, GUI port를 바꿀때, 20000 이외에도 응답하도록 수정.

2019.03.21. v91.00.02
	- AAU merge
	- clock fail added.
	- AAU : center freq, DL/UL Gain added as DL ATT, UL ATT(table)
	
	alarm 구분.
	m_St.AlmSt.Path[path4].PAU.Disabled = 0;
	m_St.AlmSt.Path[path4].PAU.Shutdown = 1;


2019.03.20. v92.00.02
	- AAU merge

Set_System_Net_Info

Set_Base_Info_From_VSS // not called

Remapping_Self_Ip // Nokia
Remapping_Self_Ip_SS // samsung

Remapping_Self_Ip_Use_RuId // IP change nokia
Remapping_Self_Ip_SS_Use_RuId // IP change samsung


2019.03.20. v93.00.02
	- BIP count : register 128bit added
	- BIP error count : register added
	- register : Rx/Cascade Add On/Off(UL) 제어 변경
		[7] : Alpha 3 Path
		[6] : Alpha 2 Path
		[5] : Alpha 1 Path
		[4] : Alpha 0 Path
		[3] : Main 3 Path
		[2] : Main 2 Path
		[1] : Main 1 Path
		[0] : Main 0 Path

	- PRU-PAU table : GUI display 구현.
	- PSU : alarm, status 몇몇 수정 및 추가.


2019.03.19. v97.00.02 ( Release is V00.00.02)
	- AID 0xB41E : ReChannel BW : DL/UL -> 100M/Alpha
	
2019.03.18. v99.00.02 ( Release is V00.00.02)
	- AID Hidden 0xB435 added
	( Rx/Cascade Add On/Off(UL) : [0] : RX Cascade On/Off)
	- cfr on/off, dpd on/off 반대로제어 되는 것 수정.

2019.03.18. v18.06.00
	- Select 방식 수정.
	- localhost 추가.
	

2019.03.15. v15.01.xx
- TSSI, RSSI 상한 하한 알람 추가.
- AID 추가 

- PCI 0x5003
- Cell ID 0x5004
- gNB ID 0x5005
- Port No 0x5006
- FAN Mode 0x5410
- FAN Mode 동작 상태 0x5411
- FAN Mode 동작 시간 0x5412


2019.03.15. v00.00.02 : Release.

2019.03.13.
	"Chk_Same_Process()" added : for terminating same-process-pid

v90.01.01 : 2019.03.06.
	CIM Test version and New AID2(with New GUI)

v89.01.01 : 2019.03.08.
	from v38.03.01 Test version
	and undefine belows 
*/

/* EOF */
