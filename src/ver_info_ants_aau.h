/*
---------------------------------------------------
	ANTS AAU : version info
---------------------------------------------------branch subdir01	
*/

//------------------------------------------------------------
#define SW_VER1	0x10 // Internal Ver 1st
#define SW_VER2	0x00 // Internal Ver 2nd

/*------------------------------------------------------------
Hidden 
------------------------------------------------------------*/
#define SW_R_VER1 20 // 1st byte: Release year
#define SW_R_VER2 7  // 2nd byte: Release number
#define SW_R_VER3 0  // 3rd byte: Patch number

/*------------------------------------------------------------
Main Release 
	19.01.04 : 2020.02.20 Now
	19.01.03 : 2020.02.14 Now
------------------------------------------------------------
TEST 
	99.01.03 : 2020.02.14 : gIRO DVT 기능시험 준비 건으로 UDA Test F/W
------------------------------------------------------------*/
#define SW_Main_VER1 99 // 1st byte: Release year
#define SW_Main_VER2  1 // 2nd byte: Release number (ex) year 2015,1st Release FW. from 1
#define SW_Main_VER3  6 // 3rd byte: Patch number (Patch number in Release ver, from 0)
//------------------------------------------------------------


/*
---------------------------------
IP : L/S 에 따라 IP 다름 : samsung 3/11/19/27
----------------------------------


//------------------------------------------- 
V20.07.00 (M_99.01.06) / AAU / 2020.07.31 
//-------------------------------------------
	-- 2020.06.12 이후, PRU 수정사항 AAU에 추가.

	1. Module Inventory : 상태응답 순서 변경 
	    * AAU : DTU - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd - RCU

	2. FPGA Reg.Map : SFP Vendor PN, Vendor Name, Vendor SN, Date Code 필드 추가

	3. GUI 상태등답에 SFP Vendor PN(AID 0x5316) 추가.

	4. SFP Inventory (0x6A Command) 추가.

	5. BIP와 Hard Clipping Count 초기화 
	[기존] 통계보고 후, 누적 초기화
	[수정] 통계보고 후, 계속 누적하도록 변경(초기화 하지 않음)
	
	6. 통계보고 AID 추가.	
	AID 0x5409 : Optic Delay (단방향)

	7. PD 상한/하한 알람 수정.	

	8. AID 추가.
		- Hard Clipping count(0x5320) (Mea: 순시값 --> 누적값 변경)   
		- BIP Error 누적 Count 초기화 (0x530B) : (Mea: 순시값 --> 누적값 변경)
		- PD Power 상한(0x530C)
		- PD Power 하한(0x5309)
		- Optic Delay (단방향) (0x5409)
		- IP Address (중계기)  (0x5021)
		- IP Address (LS) (0x5022)
		- IP Address (gREMS) (0x5023)
		- Hard Clipping Count 초기화(0x5322)	
	삭제--------------------	
		- Hard Clipping 누적 Count(DL)(0xB40E)
		- Hard Clipping 누적 Count 초기화(0xB40F)
		- BIP Error 누적 Count(DL)(0xB427)
		- BIP Error 누적 Count 초기화(0xB428)	
		- IP(0xB000) 삭제.
		- VSS(NOTI IP1) (0xB445) 삭제.
		- VSS(NOTI IP2) (0xB446) 삭제.

	9. AID 0x530A
		[기존] 순시값(d_BIP_Count) 사용.
		[수정] 누적값(d_BIP_ErrCnt) 0xB427 을 중복하여 0x530A 에 사용.
		[통계] 원래대로 누적값 사용.(수정 사항 없음)
		
	10. AID 0x5320
		[기존] 순시값(d_Hard_Clip_Cnt_DL) 사용.
		[수정] 누적값(d_Hard_Clip_Cnt_Accum) 0xB40E 을 중복하여 0x5320 에 사용.
		[통계] 원래대로 누적값 사용.(수정 사항 없음)
		
	11.	RU Initalize가 초기화 완료시 혹은 5/6 Delay Setup Request waiting 상태로 10분 유지시
		결국 gREMS로 보고를 하기 시작하는 시점에 도달시 현재 발생된 알람은 gREMS에 보고
		define "__Send_Init_Alarm__When_0x500D_RuInit_is_6__"
		undef  "__Seperate_AlarmLog__From__AutoReport__"

	12. zModem 실행 후 timeout이 정상 동작 하지 않는 케이스가 있어 시리얼통신이 되지 않는 현상 존재.
		[수정] 
		Process 모니터링 후 1분간 지속 중일 경우 auto kill 기능 추가
		sz 명령 실행후, timeout 30s 추가.

	13. 100M/Alpha insert --> 100M 탈장 -> 100M 다시 장착 시, AMP Off 되는 현상.
		"V20.01.10 (M_19.01.02) / PRU / 2020.01.29" 부터 문제 O
		"V19.12.10 (M_19.01.01)" version 에서 문제 X

		"V20.01.10 (M_19.01.02)" 수정되면서, AMP off 시키는 조건부분 잘못된 부분 수정.
		"100M 다시 장착" 되어도 일정 시간 동안, 100M CPRI_Alarm이 유지되고 있음.
		이때, 100M/Alpha Common CPRI Alarm 이어야 amp off 인데, 
		100M CPRI_Alarm 으로 인해, amp off 됨. 
		100M/Alpha Common CPRI Alarm 일때, amp off 되도록 수정.

	14. UDA Reset : 
		[현상] Alarm only 일 때, gREMS 보고와 UDA Alarm Set 안됨.
		[수정] (다른 부분 수정없이) Alarm only 일 때, gREMS 보고와 UDA Alarm Set 되도록 수정.


//-------------------------------------------
V20.06.04 (M_99.01.06) / AAU / 2020.06.12
//-------------------------------------------
	1. LS 등록정보 기능 변경
	2. ping test 기능 추가.
	3. Log Count(AID :0x5454)
		(1) '88' 로 제어시에만 LS 등록번호 Null로 제어 후 저장
		(2) '0' 로 제어시에만 로그 초기화
		(3) 그외 제어시에는 아무것도 하지 않음.


//-------------------------------------------
V20.05.02 (M_99.01.06) / AAU / 2020.05.26
//-------------------------------------------
	1. clock fail 수정
	[현상] clock fail 정상으로 가지 않음.(optic 탈착 -> 장착 시)
	[수정] clock 정상처리 부분 누락. register 3s 정상되면, cpri lock에 무관하게 clock 정상되도록 수정.	
	[최종]
	(1) 3s 유지 알람(register 값)인 경우,
	cpri(mmap crpi 상태) 읽은 값이 정상이면, 최종 알람상태(m_St.AlmSt.A_4.Clock)를 알람으로 설정.
	그렇지 않으면, 최종 알람상태(m_St.AlmSt.A_4.Clock)는 이전값 유지
	(2) 3s 유지 정상(register 값)인 경우, 최종 알람상태(m_St.AlmSt.A_4.Clock)를 정상으로 설정.

	2. debug 표시 삭제.('void disp_fwd_out_pwr(INT8U path)')
	"Layer[%d] Fwd_Out_Pwr = %.1f[dBm],  DLFB RSSI = %.1f[dBm], reg_FB_RSSI_PEAK = %08d/%08d, reg_FB_RSSI_RMS = %08d/%08d"



//-------------------------------------------
V20.05.01 (M_99.01.05) / AAU / 2020.05.13
V20.04.02 (M_99.01.05) / AAU / 2020.04.__ : Not Release
//-------------------------------------------	
	1. sftp command 수정 : [sshpass -p %s] -> [sshpass -p '%s'] for special characters
	
	2. UDA Reset Suppress 해제(35m 후)시, 
		만일 uda_clock 혹은 uda_pd_cpri alarm 상태이면, alarm clear 하도록 수정.

	3. 현상 : GUI Serial port 로 연결해서 REMS로 sftp download test 할 때,
		RCU segmentation fault 띄우면서 재시작 되는 현상.
		수정 : 0x50 Command 의 경우, 처리함수 인자가 null 되는 경우의 처리 수정.
		(UP_Data_Analysis  함수에서 인자로 받는 r_ip pointer)

	4. UDA Alarm clear :
		현상 : Ru-init 6 단계에서, gREMS에 reset report 후, UDA Alarm(clock/cpri) clear/alarm report 함.
		수정 : uda alarm 유지 하도록 수정.( gREMS report 도 하지 않도록.)

	5. Package download
		[Cur] Root_F/S, Kernel : downlaod 안됨.
		[Chg] downlaod 추가. "__USE_KERNEL_ROOTFS_DN__CHECK__" for PRU

	6. SFP 관련 통계데이터 전송 data 순서.
		[cur] SFP 관련 통계데이터 전송 data 순서가, GUI Status Response 순서와 다름.
		[chg] GUI Status Response 순서대로 응답하도록 수정.(AID 0x5305/08/10/11/14)

	7. 알람 이력 저장 후 통신 정상화 시 전송 기능 --<PRU확인 안된 기능이라서, 기능을 disable 함>
		[Cur] CPRI Alarm 전송 시도
		[Chg] 통신 정상화 시 전송 : 
		기존 'log & alarm 전송'용 공통 buffer 대신,
		'log용' 과 'alarm전송'용 2개 구별해서, alarm 은 통신 정상화 시 전송하도록 수정.
		"__Seperate_AlarmLog__From__AutoReport__"		

//-------------------------------------------
V20.04.01 (M_99.01.05) / AAU / 2020.04.02
//-------------------------------------------	
	1. 주장비사 별 PLL 설정값 AID 추가(0xC090/91/92)

	2. FTP 다운로드
		(1) FTP 다운로드에 대한 D/L 상태 미 변경
		(2) D/L 중에 다시 FTP 다운로드 명령 수신시 NACK 응답하도록 수정.
		(3) 다운로드 적용시간(from gREMS) 반영(시간 비교후 다운로드.)

	3. TSSI 상한 알람
		'100+a' 사용 중에 TSSI 상한알람 발생 상태에서 
		'100 only'로 변경되면, 알람이 해제가 되지 않는 경우 수정.

	4. AAU 앱 실행후 Version 출력시 DTU S/W 부분(Register의 버전)도 표시 추가
	5. AID 0x0100/0x0101(maker/supplier) 제어 되지 않도록 수정.
	6. Download Cancel 시, RCU Reboot 되지 않도록 수정.
	7. Ru init 이 안된 상태이고, CPRI alarm 상태에서 	
	아래 메시지 반복됨.
		<RCU> [AMP_0] reason_SD_5117 : (0) -> (2)
		<RCU> [AMP_1] reason_SD_5117 : (0) -> (2)
		<RCU> [AMP_3] reason_SD_5117 : (0) -> (2)

	반복되지 않도록 수정.	
	-------------------
	cpri alarm 이지만, 상위보고 하지는 않는 상태('m_Env.gREMS_Noti_Ready = 0')이므로, 
	reason clear 가 무의미 하므로 
	'Clr__Reason_SD_5115' 하는 부분 삭제.
	
	////path4 = (i-6)/5; // 0~3
	////if(j==4) {Clr__Reason_Dis_5117(path4); Clr__Reason_Dis_5117_Prev(path4);} // bit4
	////if(j==0) {Clr__Reason_SD_5115(path4); Clr__Reason_SD_5115_Prev(path4);} // bit0 


//-------------------------------------------
V20.03.07 (M_19.01.04) / AAU / 2020.03.30
//-------------------------------------------	
	1. debug Test 추가 : AAU의 PLL 관련 설정을 변경 하며 시험.

//-------------------------------------------
V20.03.06 (M_19.01.04) / AAU / 2020.03.27
//-------------------------------------------	
	1. debug Test 추가 : AAU의 PLL 관련 설정을 변경 하며 시험.
	

//-------------------------------------------
V20.03.05 (M_19.01.04) / AAU / 2020.03.26
//-------------------------------------------	
	1. LogParset GUI 의 전용 Command의 경우, '상태갑시접속' alarm 발생하지 않도록 수정.
	2. BT initialize 추가 (for BT-Re-Connection) : 
		(Serial GUI 연결과 관계없이) BT 데이터가 20s정도 수신되지 않는 경우 BT initialize.
	3. TSSI 로그 출력기능 활성화(PRU와 동일) : ("__USE_TSSI_LOG__")
	


//-------------------------------------------
V20.03.04 (M_19.01.04) / AAU / 2020.03.24
//-------------------------------------------
	1. Clock fail 발생 안되는 것 수정(debug 표시 삭제)
	(Clock fail 3s Check timer가 계속 clear되는 부분 수정.)
	
	2. AAU 통계보고시 DL 출력값 이상 debug를 위한 출력표시(fwd out pwr < 0 의 경우에만)
		부분에 Log ON 일때(AID 0x5453) 만 debug 표시 하도록 수정.

//-------------------------------------------
V20.03.03 (M_99.01.05) / AAU / 2020.03.24
//-------------------------------------------
1. 삼성전자 일때 LS ID 계산 방식 수정
	Samsung DUH 20 L/S ID 계산식 변경 : "(port_id/8) + (slot_id*2-2)" -> "(port_id) + (slot_id*12-12)"
	Range : 0~35, 
	연산후 0 보다 작을 경우나 35보다 클 경우 LS ID 99로 표시 후 
	"Invalid LS ID = [%d], slot ID =[%d], Port ID =[%d]"  Log 1회 출력


//-------------------------------------------
V20.03.02 (M_19.01.04) / AAU / 2020.03.23
//-------------------------------------------
1. AAU 통계보고시 DL 출력값 이상 debug를 위한 출력표시(fwd out pwr < 0 의 경우에만)
2. LS 등록번호 : 
	(1) 자동생성시에는 EEP 저장하지 않도록 수정.
	(2) RCU Start 시, 자동생성 여부 check 하는 기능 추가.
	AID에 표시되는 LS 등록번호는 자동생성된것을 보여주고, 자동생성 되지 않았으면 기존의 값을 보여줌.
3. Log 파일에 저장된 상태 패킷의 Information Check sum의 오류 수정 
4. Clock fail : debug 표시 : 2s 마다.
	 "[Clock]-timer alm %d[ms]/clr %d[ms], last %d, tmp %d(reg 0x%X, 0x%X), Mask_at_Reboot %d)", 





//-------------------------------------------
V20.03.01 (M_19.01.04) / AAU / 2020.03.18
//-------------------------------------------
	1. AAU  DC Fail 알람 : I2C의 파일 open 실패시 디버그 추가.
	 Red Color 표시 : "[Get_ADC_voltage] fd < 0 !!, Use previous xxx[mV]"


//-------------------------------------------
V20.03.00 (M_19.01.04) / AAU / 2020.03.13
//-------------------------------------------
	0. AAU  DC Fail 알람 :  
	reboot 도중 ADC Read 하는 부분에서, I2C의 파일 open 실패시 '0' 값으로 리턴하는 원인.
	정상 ADC Read 시, 정상 값 backup 하고 있다가, 
	I2C의 파일 open 실패시, 0 대신 이전 값을 return 하도록 수정.
	
	1. AAU UL Atten 상한 : AID 0x5206 : control limit 20.0 dB 추가 
		(m_Eep.d_Att_Dig_UL_User_x10)
		
	2. 	AID 0x5421~0x5424 : limit 추가.	
	(DL ON Guard Time/DL OFF Guard Time/DL to UL Guard Time/UL to DL Guard Time)

	3. LogData 조회 시간 개선 필요: 
	GUI 버전에 대한 최신 버전 GUI로 할 경우 속도가 느리며, 타사도 동일 현상
	BT Thread 주기 변경 : 500ms -> 30ms

	4. 정상 운영 중 clock fail 미발생
	현 구현 상태  : CPRI Core Lock 상태 때 미발생이고, CPRI가 unlock 일때 발생한다.
	-> 수정필요 : CPRI 정상상태에서 Clock fail 이 발생하면 알람 처리, CPRI가 알람 발생해도 Clock fail은 유지

//-------------------------------------------
V20.02.16 (M_19.01.04) / AAU / 2020.02.27 : Not Release
//-------------------------------------------
	1. BIP 통계 : bip 4개중, 첫번째 제외한 나머지 3개에 대해 초기화 안되어 있음.
		초기화 과정 수정.


//-------------------------------------------
V20.02.15 (M_19.01.04) / AAU / 2020.02.24
//-------------------------------------------
	1. Command 0x5A : NACK 응답 시
	 (1) 아래 변수 clear (tot_crc, downReceivedBlock, header)
	 (2) "/tmp/RCU_DN.tar" 도 지우도로 수정.
	2. Command 0x5B 미적용의 경우에만,
	 (1) 아래 변수 clear (tot_crc, downReceivedBlock, header)
	 (2) "/tmp/RCU_DN.tar" 도 지우도로 수정.
	 (3) "/tmp/DN_file.zip" 도 지우도로 수정.

//-------------------------------------------
V20.02.10 (M_19.01.04) / AAU / 2020.02.24
//-------------------------------------------
	1. Package download & reboot 되기전, 6~7회에 1회꼴로 
	"omap_i2c 44e0b000.i2c: controller timed out" 나오면서, DC alarm 발생.
	(1) 연속 3번 dc alarm 시, alarm 으로 판단하도록 수정.
	(2) FW적용 command 내려오면, ADC (for dc fail) Disable 하도록 수정.
	2. Command 0x5A : NACK 응답 시 혹은 Command 0x5B 적용시 
		아래 변수 clear (tot_crc, downReceivedBlock)
	3. dl output low recovery 기능 
		: dl out low alarm시, 30분체크 및 3회 연속 리셋 기능 추가.




//-------------------------------------------
V20.02.09 (M_19.01.04) / AAU / 2020.02.20
//-------------------------------------------
	1. ID Mismatch 수정 : debug 추가 및 Test alarm(mismatch) 수정.

//-------------------------------------------
V20.02.08 (M_99.01.03) / AAU / 2020.02.19
//-------------------------------------------
	1. Mismatch Alarm/정상 : GUI는 즉시, gREMS Report는 10분 유지이지만,
		Test Alarm 으로 하는 경우, 10분울 10초로 변경.


//-------------------------------------------
V20.02.07 (M_99.01.03) / AAU / 2020.02.19
//-------------------------------------------
	1. CFR 계산 값 Register 적용 시, 16bit(0xffff) 넘지 않도록 수정.
		(PAR(AID 0xB40B) 과 CFR_Offset(AID 0xC07B)으로 계산되는 CFR 값)



//-------------------------------------------
V20.02.06 (M_99.01.03) / AAU / 2020.02.19
//-------------------------------------------
	1. Clock_Status (AID 0xB41A) <--> register를 읽은 값 mapping 변경.
	 reg [2] : JESD Lock,   Lock ='1' : bit5 : ADC/DAC JESD
	 reg [1] : PLL/AFE Init Done ='1' : bit4 : ADC/DAC
	 reg [0] : PLL Lock,    Lock ='1' : bit6 : DCM(Main)	


//-------------------------------------------
V20.02.05 (M_99.01.03) / AAU / 2020.02.19
//-------------------------------------------
	1. Clock_Status (AID 0xB41A) 는 '0' 이 Normal, register 는 읽은 값이 '1' 이면, Normal 이므로,
		register를 읽을 값을 0 <-> 1 toggle 시켜서 표시하도록 수정.
		
	2. Gain_Check_Mode (AID 0xC054) 가 off 될 때, UL.Gain_Low.Alarm이 clear 되는 현상 수정.
		UL gain 저하 offset 체크 algorithm이 Gain_Check_Mode (AID 0xC054) 가 off일때,
		완료모드가 아닌 것으로 되면서, UL.Gain_Low.Alarm이 clear 되었음. 
		이 부분을 수정.


//-------------------------------------------
V20.02.04 (M_99.01.03) / AAU / 2020.02.19
//-------------------------------------------
	1. GUI Test Alarm으로 알람/정상 발생 시, 
		Alarm report string(AID 부가정보)가 NULL이 되는 사항 수정.
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
			
		(2) AID 0x5215 : Shutdown : 
			reason을 Test alarm으로 선택할 수 없어서, default로 "Rsn_SD__" 로 표시됨.


//-------------------------------------------
V20.02.03 (M_19.01.03) / AAU / 2020.02.19
//-------------------------------------------
	1. DUH ID 하위 Cascade 로 복사(Register 0x2003F 값을 0x2403F 에 Write)

//-------------------------------------------
V20.02.03 (M_19.01.03) / AAU / 2020.02.14, Not Release
//-------------------------------------------
	1. L/S alarm report 'Alm_Subcode' 4byte little/big endian : 
		"2020.01.14" 버전으로 복귀.
	2. SFP LD/PD 알람 : 
		'clock alarm' or 'cpri alarm' 이면, 알람 clear 하는 조건에서 'cpri alarm' 조건은 삭제.

//-------------------------------------------
V20.02.02 (M_19.01.03) / AAU / 2020.02.14, Not Release
//-------------------------------------------
	1. SFP_LD_Pwr(0x5305) : SFP insert 안되면 0.0 표시하도록 수정.
	2. SFP_PD_Pwr(0x5308) : SFP insert 안되면 0.0 표시하도록 수정.

//-------------------------------------------
V20.99.32 (M_19.01.03) / AAU / 2020.02.14, TEST Version
V20.99.31 (M_19.01.03) / AAU / 2020.02.13, TEST Version
//-------------------------------------------
	1. AID 0xC07F 추가 : UL Att

//-------------------------------------------
V20.99.30 (M_19.01.03) / AAU / 2020.02.13, TEST Version
//-------------------------------------------
	1. log 취득 기능 추가.
	(1) Command 0xB0 ~ 0xB5 추가.
	(2) 5m log file : tgz 압축하도록 수정 및 압축파일 전송.
	(3) AID 0xD454 Added.


//-------------------------------------------
V20.02.01 (M_19.01.03) / AAU / 2020.02.04
//-------------------------------------------
	- Version change from "V20.01.12 (M_19.01.01) / AAU / 2020.02.04"

//-------------------------------------------
V20.01.12 (M_19.01.01) / AAU / 2020.02.04
//-------------------------------------------
	1. Default Setting 항목 추가 : LS 제조사별 Test pattern gain(100/+a)(AID 0xC074)
	2. AMP OFF -> Layer '미사용' 설정 시 Disabled Alm Clear 된 후,
	   다시 Layer '사용' 설정 시, Disabled 알람이 발생하지 않음(비정상) 수정.

 //-------------------------------------------
 V20.01.12 (M_19.01.01) / AAU / 2020.02.04
 //-------------------------------------------
	 1. Layer 사용 정보(0x5002) 표현 방법 수정
	  (기존) 0x0 : 미사용
	  (수정) 0x0/2/3 : 미사용
	 2. LS Gain Balance(0xc076) : '시스템 Reset시 항상 ON' 되도록 수정.
	 3. Layer Splitter 등록 번호(0x500E) 추가 
	 4. L/S alarm report : string ("User controlSyst"로 표시되는 부분)수정.  
	 5. 기타 : PRU 관련 수정
	 (1) AMP : layer 미사용시, Amp Off Alarm 즉시 갱신되지 않도록 수정.	

//-------------------------------------------
V20.01.10 (M_19.01.01) / AAU / 2020.01.29
//-------------------------------------------
	1. same with V20.01.09 (M_19.99.99)
	(출력 하한 Alarm이 발생 되지 않아야 하는 조건에서 출력 하한 Alarm이 발생 되며, 
	UL Gain Check Mode가 ON 상태에서 UL Gain Check는 동작 되지 않습니다.)
//-------------------------------------------
V20.01.09 (M_19.99.99) / AAU / 2020.01.29
//-------------------------------------------


//-------------------------------------------
V20.01.08 (M_19.99.99) / AAU / 2020.01.29
//-------------------------------------------
	1. UL 감시 onoff(AID 0x5451)
	(1) Off : Noise는 5분 마다 check 하도록 수정.(8s 동안)
	(2) On  : 5분 마다 Gain/Noise check (Noise 8s 먼저 check 후, Gain 90s check)

//-------------------------------------------
V20.01.07 (M_19.99.99) / AAU / 2020.01.29
//-------------------------------------------
	1. UDA 불요파 Payload OFF시
	(1) 출력 하한 알람 masking 추가.
	(2) UDA 발생 시 동작이 Payload OFF 일 때 Amp/LNA는 OFF 하지 않도록 수정
	2.	TSSI 하한 Alarm 발생 시 DL 출력 하한 Alarm Masking.


//-------------------------------------------
V20.01.06 (M_19.99.99) / AAU / 2020.01.28
//-------------------------------------------
	1. DUH-ID(AID 0x5009) 추가.
	2. UDA 불요파로 인한 Payload off 시, DL/UL AMP 제어.
	[기존] Payload Off, DL AMP Off 하지만 UL AMP 제어 X.
	[수정] Payload 만 Off (DL/UL AMP 제어 X)
	

//-------------------------------------------
V20.01.05 (M_19.99.99) / AAU / 2020.01.23
//-------------------------------------------
	1. 최초 Download 후 Default 값 설정/변경.
		(1) 통계 보고 기준시(AID 0x5600) : 03시 00분 변경 --> 04시 00분
		(2) 자동 보고 기준시(AID 0x5603) : 04시 00분 변경 --> 03시 00분
		(3) UL Gain 자동 Back Off(AID 0x5450) :	"OFF" 추가.
	2. 	UL Gain Check ON/OFF와는 무관하게 RSSI Noise(A_ID 0xB306) 값 갱신되도록 수정.
	3. "UDA 관련 L/S 보고할 때, String 변경"
		<변경>
		SPURIOUS-Clear/SPURIOUS-AlarmOnly/SPURIOUS-PayloadOff
		CPRI-Clear/CPRI-AlarmOnly/CPRI-Reset
		CLOCK-Clear/CLOCK-AlarmOnly/CLOCK-Reset
	


//-------------------------------------------
V20.01.04 (M_19.99.99) / AAU / 2020.01.21
//-------------------------------------------
	1. 최초 Download 후 Default 값 설정변경.	
        (12) DL LSH Gain(S) 100/+a(AID 0xC062) : 2.0


//-------------------------------------------
V20.01.03 (M_19.99.99) / AAU / 2020.01.21
//-------------------------------------------
	1. 최초 Download 후 Default 값 설정변경.	
		( 2) 출력 상한(AID 0x5104)     +25dBm 
        ( 3) 출력 하한(AID 0x5105)     +0dBm
        ( 8) TDD Guard Time    (AID 0x5421 ~ 24) : 5/5/5/5
        (11) DL LSH Gain(E) 100/+a(AID 0xC064) : 4.0

//-------------------------------------------
V20.01.02 (M_19.99.99) / AAU / 2020.01.20
//-------------------------------------------
	0. Version changed from '2020.01.20 : v20.99.03'
	1. L/S : alarm report : 'Alm_Subcode' 4byte little/big endian 수정.
	2. CW Auto On/Off(AID 0xC070) : 설정 값이 저장 되도록 수정.			
	3. UL Gain 자동 Backoff	(AID 0x5450) : 
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
	4. 최초 Download 후 Default 값 설정	변경.	
		( 1) TSSI 하한(AID 0x5102)	-60dBm -->	-70dBm
		( 2) 출력 상한(AID 0x5104)	+28dBm -->	+25dBm
		( 3) 출력 하한(AID 0x5105)	+4dBm  -->	+0dBm		
		( 4) AMP_Standby 적용 Path(AID 0x5437)	미적용/적용/적용/적용  -->	미적용/미적용/미적용/미적용
		( 5) Log(AID 0x5453) ON --> OFF
		( 6) MIMO_Standby 진입 Level(AID  0x543A) :	+11dBm
		( 7) MIMO_Standby 해제 Level(AID  0x543B) :	+15dBm		
		( 8) TDD Guard Time	(AID 0x5421) : 5/5/5/5
		( 9) CW Auto On/Off (AID 0xC070) : OFF
		(10) DL LSH Gain(N) 100/+a(AID 0xC063) : 8.5
		(11) DL LSH Gain(E) 100/+a(AID 0xC064) : 4.0
	
	5. "V20.01.99 (M_19.01.01) / AAU / 2020.01.15 : Test version"	
	(1) Fan alarm time delay 3s 적용.
	(2) log 삭제. "[AMP_0] reason_SD_5117 : (2) -> (3), ctrl req"
	


//-------------------------------------------
V20.01.99 (M_19.01.01) / AAU / 2020.01.15 : Test version
//-------------------------------------------
	1. Fan alarm time delay 3s 적용.
	2. log 삭제. "[AMP_0] reason_SD_5117 : (2) -> (3), ctrl req"


//-------------------------------------------
V20.01.01 (M_19.01.01) / AAU / 2020.01.14
//-------------------------------------------
	1. Version Release from "V19.12.53 (M_19.01.01) / AAU / 2020.01.14 : TEST Version"
	2. UL.Over.In algorithm 30m 구간 test 1m : 원래의 30m으로 원복.
	3. RU Init 중에는, AMP-Stdby, Mimo-Stdby가 마치 시간이 out-of-time 인것 처럼, 
	exit 되도록 수정.(amp-off 로 exit 되도록 수정)
	4. RU Reset 시 MIMO Standby Mode 동작 상태(AID 0x5431) On 표시 이상 
	: off 되도록 수정.
	5. TSSI 대표값 -90dBm 처리 기준 
	6. TSSI 하한 설정값 :  f/w 다운로드 시 -70 dBm으로 default setting 되도록 수정.


	V19.12.53 (M_19.01.01) / AAU / 2020.01.14 : TEST Version
	//-------------------------------------------
		1. RU Init 중에는, AMP-Stdby, Mimo-Stdby가 마치 시간이 out-of-time 인것 처럼, 
		exit 되도록 수정.(amp-off 로 exit 되도록 수정)


	//-------------------------------------------
	V19.12.52 (M_19.01.01) / AAU / 2020.01.14 : TEST Version
	//-------------------------------------------
		1. RU Reset 시 MIMO Standby Mode 동작 상태(AID 0x5431) On 표시 이상 
		: off 되도록 수정.
	

//-------------------------------------------
V19.12.24 (M_19.01.01) / AAU / 2020.01.10
//-------------------------------------------
	1. RCU Restart 시, USER-AMP-OFF 이었으면, OFF Reason 설정.
	2. MIMO_Standby 
	(1) AMP 1ea user off 시, mimo standby 안되는 부분 수정.
		"PAU_OFF_Req[AMPU_4_MAX]"
	(2) shutdown alarm 해제 되지 않도록 수정 : 
	(3) mimo standby backup alarm 사용을 amp off 된 구간에만 사용하도록 수정.
	(4) TSSI 상한 check : mimo/amp standby 구간 동안에는 check 하지 않도록 수정.


//-------------------------------------------
V19.12.23 (M_19.01.01) / AAU / 2020.01.09
//-------------------------------------------
	1. L/S alarm 응답시, 무조건 정상으로 report 하는 부분 수정.
	2. UL.Over.In algorithm : user on 시, UL.Over.In alarm 해제되도록 수정.

//-------------------------------------------
V19.12.22 (M_19.01.01) / AAU / 2020.01.09
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
V19.12.21 (M_19.01.01) / AAU / 2020.01.08
//-------------------------------------------	
	1. DC Alarm String : 전압(DC Fail 설정값)(48v)(0xB608) 로 변경.
	2. Gain-Backoff (AID 0x5208, 0xB426) : GUI Display용과 내부 계산용 구분.
	(4.8dB의 경우, 내부 계산은 4.5dB 로 하도록 수정)
	3. AufoConfigFlag.Init_Status == 0 이면, gREMS 보고 하지 않는 것 추가.

//-------------------------------------------
V19.12.20 (M_19.01.01) / AAU / 2020.01.08
//-------------------------------------------	
	1. Time delay 2s before UL.Gain.Low Alarm algorithm-starting
	: 2s 지연이 안되는 부분 수정.


//-------------------------------------------
V19.12.19 (M_19.01.01) / AAU / 2020.01.07
//-------------------------------------------	
	1. CPRI Alarm 해제시, alarm log 저장할 수 있도록 수정.
	define "__2020_0107_Saving_AlarmLog_When_CPRI_Lock__"
	2. UL_Gain_Low_Offset flag 지정 위치 변경 및 설정 상태 디버그 표시.
	('f_UL_Gain_Low_Offset')
	3. ALC algorithm에서, ALC Att의 type 변경 : INT8S -> INT16S
	clock alarm 시, fwd power가 너무 낮은 값으로 될때, alc atten overflow 수정.
	(1) FWD_ALC algorithm에서
	(2) AAU_RVS_Quick_ALC 함수에서 
	(3)	AAU_RVS_In_ALC 함수에서	

//-------------------------------------------
V19.12.16 (M_99.01.01) / AAU / 2020.01.06
//-------------------------------------------	
	- same with "V19.12.15 (M_19.01.01) / AAU / 2019.12.30"
	
	
//-------------------------------------------
V19.12.15 (M_19.01.01) / AAU / 2019.12.30
//-------------------------------------------	
	1. 하기 정의된 default 값으로 세팅되도록 수정.
	-------------------------------------------------------
	통계보고 기준시			5600	03시 00분
	자동보고 기준시			5603	04시 00분
	통계보고 주기			5601	15분
	TSSI 상한(100/alpha)	5101	-15dBm
	TSSI 하한(100/alpha)	5102	-60dBm
	출력 상한				5104	+28dBm
	출력 하한				5105	+4dBm
	UDA CPRI				5447	Alarm만 보고
	UDA Clock				5446	Alarm만 보고
	UDA 불요파				5444	Alarm만 보고
	-------------------------------------------------------
	AMP_Stdby 시작시간		5436	23시 00분
	AMP_Stdby 종료시간		5436	07시 00분
	------------------------------------------
	MIMO Stdby 시작 시간	5432	23시 00분
	MIMO Stdby 종료 시간	5432	07시 00분
	-------------------------------------------------------
	AMP Stdby 적용Path	    5437	미적용/적용/적용/적용
	Log	                    5453	On
	-------------------------------------------------------
	(1) 설정제어 값(4byte)에 value가 0x20191230이 아니면, 
	설정제어 값(4byte)에 0x20191230를 저장하고, 
	하기 정의된 default 값으로 세팅되도록 수정.
	(2) 0x5013 TempLimit을 "-99.9" 로 제어하면, 
	temp limit은 제어되지는 않고, 다만, 	
	설정제어 값(4byte)에 0으로 설정되어, reboot 후, 
	하기 정의된 default 값으로 세팅Test 를 다시 할 수 있음.
	(0x5013 TempLimit을 "-99.9" 설정테스트는, 잠시 test 용도로서,
	이후 버전이 바뀌면, 다른 용도로 변경될 수 있음.)

//-------------------------------------------
V19.12.14 (M_19.01.01) / AAU / 2019.12.26
//-------------------------------------------	
	1. "__2019_1226__Use_Alarm_Timer_Structure__" : CPRI, Clock, 
		LOS, LOF, alarm(3s), normal(3s)
		
	2. UL_Over_Input_Alarm(0x5210) : 
		(1) GUI/gREMS/LS : 
		(2) alarm(algorithm 진입시), normal(algorithm 초기화시, 정상 level 복귀시)
		
	3. SD_Alarm(0x5115) : 
		(1) GUI : amp off 시, amp on 시.
		(2) gREMS/LS : 30m 구간 진입시 alarm, normal(algorithm 초기화시, 정상 level 복귀시)
		
	4. UL_Gain_Low algorithm start 전에 2s 시간 지연
	- update 되지 않은 RSSI_RMS 값으로 인해 UL Gain 저하 알람이 뜨지 않도록.
	
	5. VSS Table의 Z.223(DUH_ID 필드) 추가 : VSS Table debug에 구현
	- d_DUH_ID[CPRI_PATH];

	6. Clock alarm : string 표시 항상 정상으로만 되는 것 수정.


//-------------------------------------------
V19.12.xx (M_19.01.01) / AAU / 2019.12.24 
//-------------------------------------------	
	1. gREMS Alarm Report : Shutdown(DL)(AID 0x5115) Alarm String 추가.
	(Get__Str_SD_5115() 함수 수정.)


//-------------------------------------------
V19.12.13 (M_19.01.01) / AAU / 2019.12.23 
//-------------------------------------------	
	- Update_Alm_Str_etc() 삭제 : 미사용으로 인한 삭제.

//-------------------------------------------
V19.12.13 (M_19.01.01) / AAU / 2019.12.20 : Not Release
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
2. gREMS alarm 2회 발생 : 수정, test 필요.

3.	알람 알고리즘 변경.
	A.	Clock, Cpri알람 발생 시 타 알람의 알람 유지시간 초기화
	



-------------------------------------------
V19.12.10 (M_19.01.01) / AAU / 2019.12.13
-------------------------------------------
	1. Bluetooth reset 주기 추가.
	2. gREMS 파형감시 응답 1kByte 분할 처리 추가.


-------------------------------------------
V97.10.05 (M_19.01.01) / AAU / 2019.12.11
-------------------------------------------
	1. Cascade SFP_TX_DISABLE 시에, cascade LD_SFP Alarm 을 mask 하는 것 추가.	
	
	2. GC_RSSI Offset
		[1] 완료 안된 경우, Check 시간을 90s 로 변경.
		[2] retry 횟수 : 10 -> 5
		[3] If not Init_St___Completed, do not enter into gain-check-mode
		[4] GC_RSSI_RMS 값을 처음 4번 skip.
		
	3. GUI : 0xB210/12/13  Offset (DL 출력/UL 출력/UL 입력) 제어 오류 수정.
		[수정] big endian <-> little endian 변환 추가.
		
	4. Fwd_Out_Pwr, Fwd_Out_Pwr(연산)의 갱신을 가능한 같은 시간에 하도록 수정.
		(m_St.Fwd_Out_Pwr_x10, m_St.DL_OutPwr_Cal_x10)
		
	5. Disable alarm 시, gREMS String Reason에 CPRI, Clock, TSSI 추가.	



-------------------------------------------
V96.10.05 (M_19.01.01) / AAU / 2019.12.10 / Not Release
-------------------------------------------
	1. RVS Shutdown : 완전 shutdown 이 아닌 30m 구간에서 gREMS 상위 보고하도록 수정.


-------------------------------------------
V95.10.05 (M_19.01.01) / AAU / 2019.12.09
-------------------------------------------
	1. RVS Shutdown 30m : test 15s -> 30m 시간 변경.
	2. GC_RSSI_RMS Detector 게산식 변경 요청.
	- "register 읽은 값" + "UL Gain 저하 Offset(0xC05D)값 (A)" + "Height Balance" - "LS Gain Balance(DL)"
	- LS Gain Balance가 Off로 설정 되면, LS Gain Balance 값이 0으로 적용
	3. GC_RSSI Offset
	[1] 20회/100ms(Total 2s) ->	4회/1.5s(Total 6s, max 8s)
	[2] GC_RSSI_RMS 조건 (<25dB) : 4회 중 1회라도 조건이면, 보정 기능 동작.
	[3] UL Gain 저하 Offset 보상 조건 : 
		4회 결과값의 최대/최소 편차가 3dB 이내일 때 동작.
		편차가 3dB 보다 클 때 위 동작을 10회 반복 후 완료.
	[4] GC_RSSI Offset algorithm 완료 후, UL Gain 저하 알람 algorithm 동작.
	[5] Algorithm(GC_RSSI Offset 및 UL Gain 저하 알람) : gain mode가 off -> on 된 시점부터 1.5s 마다.
	[6] 기존 '5분 20회'를 8초를 위해, '5분 80회'로.
	noise mode 역시 '3분 20회'를 8초를 위해, '3분 80회'로 수정.

-------------------------------------------
V94.10.05 (M_19.01.01) / AAU / 2019.12.05
-------------------------------------------
	1. UL Gain 저하	
		[1] GC_RSSI_RMS 계산식.		
			GC_RSSI_RMS = "register 읽은 값" + "UL Gain 저하 Offset(0xC05D)값 (A)" + "Height Balance" - "LS Gain Balance"

		[2] "Adjusting UL_Gain_Offset"
			if(SFP_CPRI_0_Not_Inserted) skip;
			if(CPRI_0_Unlock) skip;
			if(AMP Off)       skip;
			if(RU_Init_Status Not 'Completed') skip;
			if(Gain_Check_Mode is 0) skip;
			
			(a) 위의 skip 상태에서는 
			"Adjusting UL_Gain_Offset" 하지 않음. algorithm은 'None' 상태로 있음.
			"UL Gain 저하 알람 체크" 알고리즘도 수행 하지 않음.

			(b) 위의 skip이 해제되면, 
			"UL Gain Check Mode 1회" 상태가 됨.
			GC_RSSI_RMS 값을 2번 버림.(GUI Display만 함, 1번 버리는 것을 1번 더.)

			(c) GC_RSSI_RMS 값을 2번 버린 다음 단계.
			이전에 "Adjusting UL_Gain_Offset" 가 완료 되었다면, "UL Gain 저하 알람 체크" 알고리즘 수행 
			그렇지 않으면, "UL Gain Check Mode 2회" 의 동작 수행.

			(d) "UL Gain Check Mode 2회" .... 가 모두 끝나면, 
			"UL Gain 저하 알람 체크" 알고리즘 수행 

			(e) 
			위 (a)~(d) 단계중 skip 상태가 나오면,  
			(a)~(d)를 수행하지 안혹, skip이 해제될 때까지 wait.
			skip이 해제되면, (a)~(d) 수행.

			(f) skip 되었다가 다시, skip이 해제되어 (a)~(d) 다시 수행하더라도,
			이전에 "Adjusting UL_Gain_Offset" 가 완료 되었다면, 
			"Adjusting UL_Gain_Offset"를 하지는 않음.
	


-------------------------------------------
V93.10.05 (M_19.01.01) / AAU / 2019.12.04
-------------------------------------------
	1. UL Gain 저하
		[1] GC_RSSI_RMS 측정 값을 Print.
		[2] Gain check mode 가 "off" 이었다가 "on" 으로 바뀌게 되면, 
		"UL Gain Check Mode 1회"의 경우로 다시 시작 되는 것이고,
		이때의 GC_RSSI_RMS 값은 측정값으로만 사용하도록 수정.
		[3] GC_RSSI_RMS 계산식.
			기존 : GC_RSSI_RMS = "register 읽은 값" + "UL Gain 저하 Offset(0xC05D)값 (A)" 
			수정 : GC_RSSI_RMS = "register 읽은 값" + "UL Gain 저하 Offset(0xC05D)값 (A)" - "Height Balance" - "LS Gain Balance"
		
	2. SFP LOS/LD 알람 보고시 PD/LD 파워 값이 정상인 오류 개선
		[수정] SFP LOS/LD 알람 2초 유지될 때, 실제 알람으로 판단하고, 
		당시의 PD/LD값을 참조하도록 수정.
	3. Shutdown Alarm이 완전 셧다운일 때, REMS 보고 되도록 수정.
	4. VSS Monitor 수정.		
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



---------------------------------------
V92.10.05 (M_19.01.01) / AAU / 2019.11.30
---------------------------------------
	0. Bluetooth Task : 500[ms]
	
	1. (EverNode_27)	
	AID(0x5454) LogCount 제어 시, 알람 카운트가 '0'이 되지만, 
	알람 발생 시 로그카운트 '0'되기 전의 로크 카운트가 복구 됨.
	(1) 이전 backup log 파일이 재사용되지 않도록 chksum file 및 backup file을 삭제하도록 수정.
	(2) mutex(alarm, ctrl) Added.

	2. (EverNode_29)
	상위보고 String : F/W 알고리즘에 의하여 AMP가 'ON/OFF'될 때는, 
	'User Control'대신 'System Control' 사용하도록 수정.

	3. (EverNode_28)
	UL 과입력 Shutdown 알고리즘 완전 Shutdown 될 때만 Shutdown 상위보고 하도록 수정.
	(gREMS Alarm Report 및 L/S Alarm Report, L/S Status Report 에 적용)

	4.(EverNode_21) VSS Table Debug
	
	5.(EverNode_11) 
	Noti IP1/2 를 Invalid_IP로 변경 시, gREMS(192.224.0.1)로 패킷 전송하도록 수정.

	6. AAU UL Gain 저하 Offset 보정 기능
	- GC_RSSI_RMS 읽어온 값이 -20dB 보다 작으면, 25dB 미만을 확인 하는 부분을 재 수행 하도록 수정.
	- Register의 UL Gain mode on/off 가 1(UL Gain check mode) 일 때만, 
		"AAU UL Gain 저하 Offset 보정 알고리즘" 을 체크하도록 수정.

	7. "UL Gain 저하 알고리즘"
	- Register의 UL Gain mode 가 1 일 때만, 알고리즘 체크하도록 수정.		
	- Register의 UL Gain mode : 1(UL Gain check mode), 0(noise check mode)

	8. Inventory 제어시, 자동으로 LS 등록번호 생성 후, 비휘발성메모리에 저장안되는 사항 수정.
		( Proc___Inventory_SN() 에서, flag-write 설정 추가)
		
	9. SFTP 접속 타임아웃 : 8[s] -> 30[s] 수정.
	 (Ericson L/S 의 경우, SFTP 접속 문제 간헐적 발생, timeout 변경 후, 정상 동작했다는 HFR 내용에 따라.)

---------------------------------------
V30.10.05 (M_19.01.01) / AAU / 2019.11.29
---------------------------------------
	1. Bluetooth Task : 500[ms] -> 30[ms] Test 수정버전 (from "V57.10.05 (M_19.01.01) / AAU / 2019.11.28")	
	
	2. Inventory 제어시, 자동으로 LS 등록번호 생성 후, 비휘발성메모리에 저장안되는 사항 수정.
		( Proc___Inventory_SN() 에서, flag-write 설정 추가)
	3. SFTP 접속 타임아웃 : 8[s] -> 30[s] 수정.
	 (Ericson L/S 의 경우, SFTP 접속 문제 간헐적 발생, timeout 변경 후, 정상 동작했다는 HFR 내용에 따라.)

---------------------------------------
V57.10.05 (M_19.01.01) / AAU / 2019.11.28
---------------------------------------
 	1. AID 0xB220, 0x5206 : 상태응답 같은 기능, User ATT 으로변경. 
		(m_St.DL_Att -> m_Eep.DL_Att_User)
		
	2. AID 0xB224 상태응답 : User ATT 으로 변경.
		(m_St.UL_Att -> m_Eep.UL_Att) 

	3. Gain_현재값(m_St.UL_Gain_x10) 수식 변경.		
		Gain(0x5207) = 30.0dB - [ User_ATTEN(0x5206) + Gain_BackOff(0xB426) + Gain_Balance(0x5209) ]
		(User_ATTEN(0x5206) is 'm_Eep.d_Att_Dig_UL_User_x10')

		[Old] 
		Gain(0x5207)
			= AAU_UL_Gain_Max_x10 - m_St.d_UL_Att_x10;
			= AAU_UL_Gain_Max_x10 - (Quick_ATT - Gain_Compensation + User_Att)			
			= AAU_UL_Gain_Max_x10 - (m_St.RX_Qck_Att_x2[m]*5 - Get_ALC_Gain_Comp_x10 + m_Eep.d_Att_Dig_UL_User_x10)

			
	4. Height.Bal.Att 이 0 일 때,(Alpha BW 가 40M보다 작을 때), L/S.GainBal 제어가 안되는 현상.
		계산식에 "Height.Bal.Att - L/S.GainBal" 부분이 음수일 때, 음수대신 0으로 사용 하던 부분을 삭제.
		즉, 계산식이 ATT 이지만 digital gain 제어와 관련 되므로, 음수이더라도 0으로 하면 안됨.
		"Get__AAU_DL_Gain_Att_x10():

	5. UL.User.Att(AID 0x5206) : 상태응답을 UL.User.Att 제어값을 표시하도록 수정.
	
	6. 계산식 변경 : UL.RSSI(AID 0x5200) 및 UL_ALC_RSSI
		- UL.User.Att(AID 0x5206), LS_Gain_Bal를 추가.
		- [기존 계산식] + [Att_Dig_UL_User] - [Curr_LS_Gain_Bal]

		UL.RSSI(AID 0x5200) : m_St.d_UL_RSSI_dBm_Alc_x10
		UL_ALC_RSSI : m_St.d_UL_RSSI_dBm_x10

		UL.User.Att(AID 0x5206) : m_Eep.d_Att_Dig_UL_User_x10
		LS_Gain_Bal : m_St.Curr_LS_Gain_Bal_x10
		
	7. UL Gain 저하 offset : 자동화 algorithm 추가.

---------------------------------------
2019.11.27 / not release
---------------------------------------
	1. Bluetooth 수정: Set bluetooth.paring.mode, every 60[s].
	(define "__YCSO__BT__CMD__", send_BT_mode_CMD() 실행.)
	2. 알람 보고시 Power와 관련항목은 알람 보고시 상태가 정상상태가 있어
		알람을 check 하는 시점의 Power와 관련항목(limit)을 사용하도록 수정.
		(알람 상태 변경 시, debug 출력 하도록 수정)
	3. alarm_log 2중화 추가.	
	* "__ALARM_LOG_BACKUP__" eanble : disable 해야 할지는 재 결정.

---------------------------------------
2019.11.06
---------------------------------------
//	1. bluetooth task : task delay : 500[ms] -> 30[ms] 수정 : comment.


---------------------------------------
V91.10.05 (M_19.01.01) / AAU / 2019.11.20
---------------------------------------
	1. 국소명 제어/표시 수정.
	

V90.10.05 (M_19.01.01) / AAU / 2019.11.19
	1. Noti_IP_1/2 : invalid check
		[0] eMMC 에 저장된 Noti_IP_1/2 가 invalid 하면, default IP(192.224.0.1) 사용.
		VSS Channel 읽은 Noti_IP_1/2 가
		[1] 0.0.0.0 이거나 255.255.255.255 이면 invalid
		[2] Broadcast IP(255.0.0.0 or 255.255.0.0) 이면 invalid
		[3] 그외 valid
	2. 출력 상한 alarm : FWD_ALC_Att > Max (15.0dB) 의 상태에서 Alarm Check 하도록 수정.
	3. 출력 하한 alarm : 출력 하한 설정 값 이하로 10초간 유지 시	
	4. 통계보고 주기가 "보고안함" 일 때는, 1회/1일 gREMS 시간요청 하지 않는 부분 수정.
	5. GUI ID/PASS 오류 건 보완
		현재 필드에서 이미 변경된 상태라면 gREMS 연동 없이 현장 다운로드 진행시 변경되지 않습니다.
		하여 GUI ID/PASS 저장 영역을 새로 할당하여 진행하도록 변경 요청 드립니다.
	6. GUI ID/PASS, Local time 요청 주기 3[s] -> 5[s]로 변경
	
	* "__ALARM_LOG_BACKUP__" disable : 완성안되어서 미사용으로 함.


	


V19.10.05 (M_19.01.01) / AAU / 2019.10.30
1. CMD2_CFG_nC_AUTO_RPT(0x4B), CMD2_CFG__C_AUTO_RPT(0x4A) Command : 
	수신 받은 packet의 sub-data 크기가 100을 넘으면, packet 버리도록 수정.


V19.10.04 (M_19.01.01) / AAU / 2019.10.28
	1. CPRI unlock 시 LS 와 gREMS에 전송 중단 요청 : gREMS로 전송하지 않도록 수정.	
	2. Auto - configuration 조건 추가 : CPRI unlock 알람 -> CPRI Lock 복귀(알람 해제) 시
		debug 표시 : "[Auto_Config] by CPRI Unlock --> Lock"
	3. V19.10.03 버전 재수정 : gREMS IP가 0.0.0.0 이지만, 알람로그(공용 GUI로 보는 알람로그)는 쌓이도록 수정.	


V19.10.03 (M_19.01.01) / AAU / 2019.10.22
	1. gREMS IP가 0.0.0.0 이면, 송신도 안하지만 알람로그(공용 GUI로 보는 알람로그)는 쌓이도록 수정.
	2. 전원 제어 CMD 수신 시, Reset Reason : 'By_User' -> 'By_PwrCtrl'


V19.10.02 (M_19.01.01) / AAU / 2019.10.18
	1. ID/Password 의 data 오염이 발생하는 문제 수정.
		gREMS IP가 0.0.0.0 의 경우, RU에서 보내는 자동보고, 및 기타 요청(가령 GUI_ID_PW(0xEC))
		이 RU 자신에게 되돌아오게 되고, 이때, ID/Password 의 data 오염이 발생.
	2. 수신된 packet의 data 길이가 맞지 않으면, 처리하지 하지 않도록 수정.
		Command LocalTime (0x69) : 길이가 6이 아니면, 처리 하지 않도록 수정.
		Command GUI_ID_PW (0xEC) : 길이가 22가 아니면, 처리 하지 않도록 수정.
	3. gREMS IP가 0.0.0.0 이면, Packet 전송하지 않도록 수정.


V19.10.01 (M_19.01.01) / AAU / 2019.10.15
	1. TSSI (AID 0x5100) Debug display every 10[s].

V19.09.02 (M_19.01.01) / AAU / 2019.10.08 : Not Release
	1. 'Reg_VSS_DL_Loopbackmode' 에 대해서는 유효성 체크 하지 않음.


V19.09.01 (M_19.01.00) / AAU / 2019.09.30
	1. factory mode 시, AMP-ON 안되는 부분 수정.
	2. log count(AID 0x5454)를 기존의 log file 갯수가 아닌, Alarm/Ctrl(0xF4 Command) log count로 mapping

V09.27.00 (M_19.01.00) / AAU / 2019.09.27 : Not Release
	1. comm debug size check : debug on 된 중에 2Mbyte 넘으면 0으로 clear.

V09.26.00 (M_19.01.00) / AAU / 2019.09.26
	1. L/S 에서 처럼, RU-ID 할당 및 Noti-ACK 받기 전까지는 gREMS 로 부터의 Packet을 처리하지 않도록 수정.
	2. Package download FW 적용 시간 표시 : CFnC 항목이라 down-count 표시하지는 않음.

V09.25.01 (M_19.01.00) / AAU / 2019.09.25
	1. RCU Start 시, test pattern을 SPI로 loading 시, 2번 loading 됨. 1번만 loading 되도록 수정.
	2. L/S 에서 처럼, RU-ID 할당 및 Noti-ACK 받기 전까지는 gREMS 로 부터의 Packet을 처리하지 않도록 수정.

V09.25.00 (M_19.01.00) / AAU / 2019.09.25
	1. reboot 없이 무한 package download 시, tmpfs 용량이 다 차는 부분 수정 : 'make_DN_file_renew' 함수의 fclose()
	2. LS 제조사 별, DL Detector(Frame,Start,Length) : 기능 적용이 안되었던 부분 적용되도록 추가.
	3. User CW ON 제어시, User 제어한 CW Level 유지
		(1) Gain Check mode, Noise Mode는 동작하지 않음.
		(2) User 가 마지막 CW-ON 제어한 시점 부터, 5분 후, 모든 path CW-OFF 제어 추가.

V09.24.02 (M_19.01.00) / AAU / 2019.09.24
	1. "V08.24.00" 에서 
	"RU ID 할당 받으면, LS로 부터의 0x9B(RU ID 재할당, Noti-ACK)에 대해, 응답/처리 하지 않도록" 부분이 
	실제로는 packet 처리가 되고 있어서, 처리하지 않도록 수정.


V09.24.01 (M_19.01.00) / AAU / 2019.09.24
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


V09.24.00 (M_19.01.00) / AAU / 2019.09.24
	1. AutoConfig 수행 시, DL Delay 및 UL Delay 값을 0x00 으로 Set
	2. text log size : 2MB -> 1MB
	

V09.23.00 (M_19.01.00) / AAU / 2019.09.23
	1.	FAN Mode : 
	FAN Mode가 저소음 모드이고, 24시간 저소음 동작조건에서 고온(90')이 되어서 일반모드로 갔는데,
	90('C)보다 작게되면, 다시 저소음 모드로 가는 것 수정.
	온도 85 ~ 90 에서는 이전의 일반모드를 유지하도록 수정.(Proc_AAU_Ext_FAN_Ctrl)
	2. gREMS : Alarm String
		(1) BIP count 0 일 때, 소수점 "0.00000" 으로 표시되지 않도록 수정.
		(2) 음수값일 때, 앞에 '+' 표시 되지 않도록 수정.

V09.20.03 (M_19.01.00) / AAU / 2019.09.20
	1.	Sftp 시도하기 전에 ssh key를 지우도록 추가
	2.	Pkg 다운로드 과정에 대한 DN fail 체크 부분 추가 및 DN fail시 여러 과정 초기화 함수 추가
	3.	Pkg 다운로드 시도하면 다운로드에 사용되었던 변수들 초기화 

V09.20.02 (M_19.09.19) / AAU / 2019.09.20
	1. 'V09.20.01' 에서 gREMS-ALM-ACK 수정시, alarm log 저장안되는 것 수정.
	2. DTU Comm Fail 되면, 10초 후 Reboot max 20회 -> max 50회 수정.

V09.20.01 (M_19.09.19) / AAU / 2019.09.20
	1. AAU Hard Clipping Level control 기능 변경
		(1) DTU_reg(0x10100) = power(10,(level+70)/20)
		(2) Clipping OFF 시, DTU_reg = 0x7FFF
		(3) 소수점 까지 계산되게 수정
		(4) 20.0 dB까지만 제어되도록 수정.
	2. gREMS-ALM-ACK 처리가 2번째 되는 부분 수정.(주로 Emulator에서 나오는 듯.)
	3. DTU Comm Fail 되면, 10초 후 Reboot max 10회 -> max 20회 수정.
	4. RU-Initial-status(AID 0x500D)가 complete가 아니면, 
		AMP ON 제어 안되게 수정(User 제어도 수정.(AID 0x5126, 0x5226, 0xB237))

V09.19.10 (M_19.09.19) / AAU / 2019.09.19
	1. AMP-Standby-Mode-적용Path (AID 0x5437) : AID Data type 수정 : 0x02 -> 0x09(pre-defined)
	2. Log_On/Off(AID 0x5453) 이 ON 일때만, Alarm/Control log(Command 0xF4) 동작하도록 수정.
	3. Package download 후 reset reason 수정.
	4. SFTP download 시간 표시 추가.

V19.09.19 (M_19.09.19) / AAU / 2019.09.19
	1. 다운로드(Reset 대기로 )진행 중, Download 상태 표시 부분 'D/L 완료(Reset 대기) '되도록 수정.

V09.19.01 (M_19.09.05) / AAU / 2019.09.19
	1. Ericson/Nokia : default gateway 설정하는 부분 수정.

V09.19.00 (M_19.09.05) / AAU / 2019.09.19
	1. 실제 압축은 풀려 있는 상태이고 적용 대기 상태로 유지 중이라 
	이런 케이스의 경우 D/L 완료(Reset 대기) 으로 변경되도록 수정.
	(DL_St_Rst_Wait -> DL_St_Complete, m_St.DL_State[DN_RCU][DL_ST])

V09.18.02 (M_19.09.05) / AAU / 2019.09.18
	1. FPGA Read 회수를 조절 : 
		CPU 점유율 낮추기 위해, VSS 가 아닌 Register 중, timing 에 많이 관련없는 것 
		('Proc_Read_Fpga_Register')
	2. 정상적인 경우 DTU-Comm-Fail Count 초기화 될 때, debug 표시 추가 함.
	

V09.18.01 (M_19.09.05) / AAU / 2019.09.18
	1. DL-TSSI (Display @ AAU only) 
	(1) TSSI_Dec_Pre_EQ(AID 0xB100) = DTU_Reg(DL_RSSI_PEAK 0x11109) + Offset(AID 0xC06F), L/S Report 용도
	(2) TSSI_Dec(AID 0xC073)        = DTU_Reg(DL_TSSI_RMS  0x11104)
	2. "V09.17.02 (M_19.09.05)" 의 아래 사항을 그 이전 버전으로 원복.
	--------------------------------------------------------
	'UDA-불요파동작모드'(AID 0x5444)가 "Alarm-Only" 로 동작 중에,
	'UDA 불요파 동작 Layer'(AID 0x544C)는 OFF 로 표시가 되지 않도록 함.
	--------------------------------------------------------
	3. DTU Comm Fail 되면, 10초 후 Reboot Added. max 10회 까지만.
	4. TSSI_Dec_Pre_EQ(AID 0xB100) 값이 음수이면, Offset(AID 0xC06F)을 적용하지 않음.
	5. VSS-DL REC_Port_Num 10 회 count가 안되고 있던 부분 수정.
	6. gREMS Comm Start 시, 이미 발생된 Alarm 을 Report 하지 않도록 "V08.29.00"를 다시 수정.


V09.17.02 (M_19.09.05) / AAU / 2019.09.17
	1. 'UDA-불요파동작모드'(AID 0x5444)가 "Alarm-Only" 로 동작 중에,
	'UDA 불요파 동작 Layer'(AID 0x544C)는 OFF 로 표시가 되지 않도록 수정.
	2. 100MHz Only 설정 시 'Alpha 입력 하한'('TSSI_Lower_Alpha')이 발생 : masking 추가.
	3. User가 AAU-Type(AID 0xB010) 변경 제어 하면, 'LS 등록번호' 자동 생성 되도록 수정.
	4. RVS ALC Timing Fix : 
	(1) 800ms Fix : "m_Env.AAU_alc_max_cnt = 8" 	
	(2) undefine and delete "__USE_AAU_ALC_SPEED_CTRL__"
	(3) "__USE_AAU_ALC_SPEED_CTRL__" define 되기 이전, 즉, 기존의 rvs_alc_cnt 사용하지 않음.
			
			
	

V09.17.01 (M_19.09.05) / AAU / 2019.09.17
	1. Statis_Rpt_Prd(AID 0x5601) 이 0 이면, 통계보고 안하도록 추가(기존 30분 마다).
	2. UL Gain 저하 알람 : 	
		- '100M' 만 check 하도록 수정.
		- Alarm mask when 'CPRI0-Not-Insert' or 'CPRI0-Unlock'
	3. d_CFR_OnOff이 OFF이면, CFR 관련 연산 하지 않도록 수정.
	4. RU-Init-Status(AID 0x500D) : 5/6 단계 아닐때 Delay 제어 받으면 에러 리턴(-101,Use of function is limited briefly)으로 수정.
	5. 출력 연산값 (AID 0x5108, DL_OutPwr_Cal_x10) : 음수일 때, 0 로 표시.

V09.17.00 (M_19.09.05) / AAU / 2019.09.17
	1. L/S 로 부터 받는 Delay 설정 값을 F/W 비휘발성메모리에 Save 하지 않도록 수정.
	(RCU Start 되면, 값을 0으로 초기화하도록 수정)
	2. gREMS : Measure(0x44), CFG_nC(0x49), CFG__C(0x48) 응답에 0x0001 Alarm AID 추가.
	3. L/S 별 test_pattern_gain(AID 0xC074) : GUI 제어순서 맞지 않음 수정



V09.16.03 (M_19.09.05) / AAU / 2019.09.16
	1. MIMO_Std_AMP_Proc 에서 "m_St.Off_Reason____Prev" 은 참조하지 않도록 수정.


V09.16.02 (M_19.09.05) / AAU / 2019.09.16
	1. 국소명 (AID0xB011) : 가변길이 AID : max 100으로 응답하지 않고, 가변형 크기로 update 되도록 수정.
	2. AID Data-type 수정 : 0x00 -> 0x07(boolean)
		(1) FWD ALC on/off (AID 0xB23B)
		(2) FWD Auto-SD-on/off(AID 0xB23C)
	3. AMP Standby : AMP #1 ~ AMP #4 중, AMP #2 ~ AMP #4 가 복구 안되는 부분 수정.
	V08.31.00에서, 'Proc_AMP_Standby' 알고리즘과 AMP_onoff 제어 sync를 맞추기 위해, 
	AMP_Stdby_Sync 추가했는데, 모든 AMP 4개 path가 아닌, 
	첫번째 AMP 에만, 적용됨으로 인한 문제.
	모든 path에 적용되도록 수정.
	4. UL Gain감시(AID 0x5451)를 CW Auto On/Off(AID 0xC070) 와 같이 동작 되게 수정.

V09.16.01 (M_19.09.05) / AAU / 2019.09.16
	1. AMP Off 시, UL-Gain 저하 알람 mask : UL_Gain_Chk_Mode 가 아니어도, 동작 되게 수정.
	2. CFR 계산 식 변경.
		(1) LS Gain Balance(AID 0xC062) 항목 추가.(Register 현재 값을 사용)	
		(2) DL_TSSI_BalAtt (AID 0xC01C) 항목 추가.
		(3) CFR Offset (AID 0xC07B) 추가 : CFR Level(AID 0xB40B) 사용할 때 SUM 하여 사용.		
	3. DL Detector SSB AID 추가.
		(1) DL Detector SSB Frame (0xC07C) 추가
		(2) DL Detector SSB Start (0xC07D) 추가
		(3) DL Detector SSB Length(0xC07E) 추가		
	4. AAU_gain_chk_level : default 40 (CW Lv을 기존 '8' --> '40' 변경)
	5. GC_RSSI_RMS 표시 : (AID 0xC053, d_R_GC_RSSI_RMS_x10) : 계산 식에서 DL/UL RF Atten부분 삭제
	6. PAU.Over_Power alarm 삭제.(gRMES는 보고 되지는 않으나, protocol 미정의 항목이므로 KST GUI에 표시되지 않도록.)
	


V09.16.00 (M_19.09.05) / AAU / 2019.09.16
	1. Overpower shutdown 시, Disable alarm 이 아닌, Shutdown alarm 되도록 수정.
	2. CPRI Status 대기 상태에서 Delay setup이 내려오더라도 제어는 처리 하되 
		RU Init status는 절차대로 진행 되도록 수정.
	3. VSS DL update 시, unlock 이면, VSS 10회 check count도 0으로 clear 하는 부분 추가.


V09.11.00 (M_19.09.05) / AAU / 2019.09.11
	1. FWD Shutdown 수정 : 
	RVS Shutdown off 시, FWD Shutdown 상태가 반복되는 것 수정.
	2. CW level : test 용으로 level 조절 되도록 수정.


V09.10.01 (M_19.09.05) / AAU / 2019.09.10
	1. "V09.10.00" 의 CPRI Recovery 디버그 많이 표시 되는 것 삭제.
	2. watchdog refresh를 FPGA Task에도 추가. 

V09.10.00 (M_19.09.05) / AAU / 2019.09.10
	1. gREMS-packet log(alarm, control)
	(1) logCount(AID 0x5454) 제어시, file을 초기화 하도록 함.
	(2) /usr/app/log/rems/alm_report.log  변경-->  /usr/app/log/rems/rems_alm_rpt.log
	(3) /usr/app/log/rems/ctl_report.log  변경-->  /usr/app/log/rems/rems_ctl_rpt.log
	2. CPRI Recovery : 
	(1) 100-optic-off ,alpha-optic-ON 에서 100을 다시 on 할 때, 
	lock 된 100의 mac 이 00:00:00:00:00:00 이면, 10회 count 를 clear 하도록 수정.
	(2) debug 표시 추가.

V09.09.02 (M_19.09.05) / AAU / 2019.09.09
	1. "V09.09.01" 의 5번 CPRI ref 관련 원복.
	2. CPRI unlock 인 path는 VSS update/write 하지 않도록 수정.

V09.09.01 (M_19.09.05) / AAU / 2019.09.09
	1. AMP-Not-Used 일때, AMP Off 되도록 수정.
	2. AMP Off 시, 
		(1) UL-Gain 저하 알람을 mask 하도록 추가.(UL gain Check 는 동작)
		(2) UL-ALC ATT(RF ATT) max 25dB Setting.
	3. LS REC Type ID에 따른 Test Pattern Gain 조정 기능 추가
	(AID 0xC074 기능을 확장함 : 기존(공통) -> 수정(L/S 3개)
	4. 0xF4 log Command Added : gREMS-Alarm-packet-log, Control-packet-log
	5. CPRI_0(Unlock)/CPRI_1(lock) ---> CPRI_0(lock)/CPRI_1(lock) 의 경우, 
	CPRI ref가 CPRI_1 -> CPRI_0 로 변경 되지 않도록(즉, CPRI_1 유지하도록) 수정.	
	

V09.06.00 (M_19.09.05) / AAU / 2019.09.06
	1. TSSI Decimal Offset(AID 0xC06F) 변경 : layer x carrier 잘못된 것 수정.

V09.05.03 (M_08.31.00) / AAU / 2019.09.05	
	1.	현재 AAU 입력 전압 표시에 사용되는 AID는 0xB437(전압(DTU))
	--> AID 0xB608(전압(DC 48V)) 으로 수정.

V09.05.02 (M_08.31.00) / AAU / 2019.09.05
	1. gREMS : CF/C, CF/nC 자동응답의 기지국 name 저장 부분 수정.(1글자 빠지는 것 수정.)

V09.05.01 (M_08.31.00) / AAU / 2019.09.05
	1. gREMS : PLD 응답 수정 : Measure 응답 처리를 별도로 구분하도록 수정.
	
V09.05.00 (M_08.31.00) / AAU / 2019.09.05
	1. inventory date : S/N 형식에 맞지 않으면, inven.date 제어 안되는 부분 수정.
	2. DL/UL ALC Off 시 ALC에 의하여 동작 되었던 Atten.을 모두 초기화(0dB) 
		(1) DL ALC On/Off(0xB23B : 	DL RF Atten.(0x5106)
		(2) UL ALC On/Off(0xB230:
			UL RF Atten.	    (0xB224)	자사/타사 ALC
			UL DTU Gain/Atten.	(0xC016)	타사 과입력 Gain 보상
			UL User Atten.	    (0x5206)	자사 과입력 Quick Atten.(?)
	3. 기존 FB_RSSI_RMS 값을 표시 하는 것에서 FB_RSSI_PEAK로 변경 

	
V09.04.00 (M_08.31.00) / AAU / 2019.09.04
	1.  A_ID 변경
	  - FAN Mode 0x5410 표현 방법 변경:  0x07(boolean) ->  0x09(pre-defined)
	  - FAN Mode 동작 상태 0x5411 표현 방법 변경:  0x07(boolean) ->  0x09(pre-defined)
	2. A_ID 추가
	  - L/S 수신 DL/UL Delay Verification 0x5408
	3. gREMS protocol 변경 : 
		(1) ftp-download-port length 1byte -> 2byte ftp_get_pkg
		(2) DOWNLOAD 완료(Device -> EMS) : 0x52 -> 0x51
	4. L/S 등록번호(AID 0xB007)
	(1) 내부 변수 명 변경 : 'Serial_Num' -> 'LS_Regist_Num'
	(2) 자동 번호 생성 변경 : 
	- NULL 이거나, KST default : 자동생성.
	- Inventory 제어시 자동생성.
	- 그외 자동 생성 하지 않음.	


V09.03.04 (M_08.31.00) / AAU / 2019.09.03
	1. UDA Clock/PD 로 reset 시, reset-reason 이 PowerCtrl 인것 수정.
	2. CFR 계산식 관련 file 처리 하여서 계산 하도록 수정.
	

V09.03.03 (M_08.31.00) / AAU / 2019.09.03
	1. gREMS
	(1) alarm report 시, 시간 정보 사용.

V09.03.02 (M_08.31.00) / AAU / 2019.09.03
	1. ATTEN Accuracy Table 동작수정.
	2. LS Gain Balance 기능 동작 안되는 부분 수정.
	3. gREMS
		(1) UL Gain 감시(AID 0x5451) 추가 : 기존 UL_Gain_Chk_Mode (AID 0xC054)에 기능을 연결함.
		(2) Alarm Report : Alarm info(String) 한글 수정. 20byte 고정으로 응답하도록 수정.
		(3) clock fail REMS 보고 안되는 것 수정.
		(4) gREMS에서 local time 응답 잘못하는 경우(연도 틀리거나 월,일,시,분,초가 범위 벗어날때), 
		 시간 설정 안되게 함.
		(5) header 통계보고 주기(0x5601) length 길이 수정 :  7 -> 8

V09.02.01 (M_08.31.00) / AAU / 2019.09.02
	1. UDA.Clock alarm : test alarm 되도록 수정.	
	2. ID Mismatch 10분 타이머 표시 : 10분 마다 표시 되지 않도록 수정.
	3. gREMS 통계 보고 : 0x5405 round_trip : layer x carrier = 1 x 2 이나 통계에는 2 x 2 로 되어 있는 것 수정.
	4. gREMS 자동 보고 : 응답에 시간 정보 AID 0xFFFE를 보고하지 않도록 삭제.


V09.02.00 (M_08.31.00) / AAU / 2019.09.02
	1. main version V08.31.00 로 변경.
	2. ATTEN Accuracy Table 동작 : User(DL)/ALC(UL) Atten 에 대한 accuracy 가 적용되고 있음.
		- register 에 제어되는 전체 Att에 대한 accuracy 로 수정.
	3. LS Gain Balance 기능 동작 : DL/UL Gain(register 0x11101/0x11121) 에 적용되도록 수정.
	4. gREMS : 
		(1) _CF_CTRL_ 자동보고 시, CenterFreq(AID 0x500B) 항목 누락된 것을 수정.
		(2) LS 제조사(AID 0x500A) : _CF_nCTL_ 자동보고 되도록 수정.
		(3) 통계 보고 후, 'bip-err-누적', 'hard-clipping-누적' 을 clear.
	5. gREMS 통계 보고 
		(1) header 의 ADI 0x5601, 0x5602 수정.
		(2) Packet Inforamtion : cur_packet, tot_packet 항목 : 0으로 보고되는 부분을 수정.
	6. SFP 탈장/실장 알람 (SFP_Ins_0_a, SFP_Ins_0_) : 
		GUI Alarm Test로 제어 되도록 수정.




	
	




	


V08.31.00 (M_08.22.00) / AAU / 2019.08.31
	1. AMP-Standby
	(1) 'Proc_AMP_Standby' 알고리즘과 AMP_onoff 제어 sync를 맞추기 위해, AMP_Stdby_Sync 추가.
	(2) debug 표시 변경.
	(3) gREMS 보고 되지 않도록 수정. 
		- AMP Standby Mode 진입 전, 알람상태(출력 상/하한 알람, disable 알람)
		- AMP Standby Mode 진입 -> 해제 시 : gREMS 보고 되는 현상.

	2. LS 제조사별 Gain 보정 기능 추가. 
		(1) Register 0x11134(DL LSH 연동 Gain)
		(2) Register 0x11135(UL LSH 연동 Gain)

	3. TB 시험 중 UL ALC 동작 시 흔들림 발생 : ALC 푸는 동작을 조정 할 수 있도록 요청 
		- quick ALC 가 아닌, 출력 쪽의 ALC 푸는 것에 대해 수정.
		- 온도상한 AID(0x5013) 의 정수를 제외한 소숫 값 1~9 를 max counter 로 사용.(1:100[ms] ~ 9:900[ms])
		- ALC 푸는 동작 : counter 를 증가해서, max counter 가 되면, ALC 풀고, counter 초기화.
		- ALC 감는 동작 : counter 를 초기화.
		- 이전의 600[ms] 마다 ALC 했던 부분은 삭제.

	4. RF Atten Accuracy 추가.







2019.08.30---------------------
V08.30.00 (Main_08.22.00) / AAU
	1. LED 동작 수정 요청
		LED 자동 Off/On 상태에서 상태 Polling시에는 이전과 동일하게 동작(blink),
		이후 상태감시 알람이 해제되면 다시 자동 Off 타이머가 동작하여 5분후에 Off		
	2. BT Name 잘못 init 되던 부분을 수정(개수가 잘못됨) : 
		RU ID 할당 전의 이름에서 RU_ID 할당 후 BT Name 으로 갱신 안되던 현상.
	3. PKG-다운로드
	(A) 요청 사항 : AAU 확인시 DL State에 F/W 전송 후, Reset 대기 상태에서 F/W 적용 눌러도 "압축 해제 중"으로 유지 중. 
		"0x04: Reset 준비중" 혹은 "D/L 완료(Reset 대기)" 로 가야 할 듯 함.
	(B) 수정 : PKG-다운로드시에 타이머 대기 상태가 되면 "DL_St_Rst_Wait" 로 상태 전환.
	4. Delay 설정 상태값 음수인 경우 0us로 표기(0xB429/B42A) 
	(register return value 보다 offset (0xB415/B416)값이 커서 음수가 되는 경우.)


2019.08.29---------------------
V08.29.01 (Main_08.22.00) : 
	1. ID Mismatch REMS report 추가
	2. Layer 운용 모드(AID 0x5433, Layer_Mode) SISO 제어 될수 있게 다시 수정.
	3. Inventory -> L/S 등록번호 : 변경된 format 으로 수정.
	4. Inventory S/N : format 이 맞지 않은 제어는 제어 안되도록 수정.


2019.08.29---------------------
V08.29.00 (Main_08.22.00) :
	1. gREMS Comm Start 시, 초기 Alarm Report 되도록 수정.
	2. gREMS Reset Alarm Report : alarm string 추가.
	"운용자 제어", "F/W Download", "전원이상", "기타",
	"전원제어", "VSS", "UDA_PD(CPRI)", "UDA_Link", "UDA_Clock" 	
	3. RCU reset 되면, 항상 Low_Noise_Mode : AID 0x5410


V08.28.01 (Main_08.22.00) : 
	1. Bitstream file 다운로드 / pkg 다운로드 시에 bitstream file을 교체하는 sys shell과의 연동이 반대로 되어 있던 부분 수정. 
	2. Bitstream  file 다운 시, 교체 결과가 실패일 경우 reset 안 하는걸 일단 cpu reset  하도록 변경( 혹은 위에서 내려온 대로)
	3. Bluetooth 이름 형식 변경 및 변경 시점 변경.
	("5GX-AAU_ANTS" -> "AAU", RU_Init_Status 가 complete 일 경우)
	4. KST AAU RCU Board : KST-Inventory-AID 추가. 0xCF01, 0xCF02	
		{ 0xCF,0x01, __INT8U, Step_No_Use, 4, 1, 1}, // kst_inventory_SN
		{ 0xCF,0x02, __INT8U, Step_No_Use, 3, 1, 1}, // kst_inventory_date
	5. Height_Bal_Onoff : RCU Restart 되면, 기본, ON 으로 되도록 수정.	
	6. Test Pattern ON시 10분 후 자동 OFF되도록 기능 추가.

V08.28.00 (Main_08.22.00) :
	1. GUI FW Version (AID 0x5503) : v08.22.00 고정.
	2. GUI FW Version (AID 0xB015) : hidden version 추가.(v08.28.00 표시.)
	3. RU_Init_Status (AID 0x500D) : 
		CPRI_Status 에 대한 설정은 현재 RU_Init_Status 상태가 CPRI_Status 보다 낮은 단계 일때 만, 갱신되도록 수정.
		Dly_Set_Req 에 대한 설정은 현재 RU_Init_Status 상태가 Dly_Set_Req 보다 낮은 단계 일때 만, 갱신되도록 수정.

V08.24.00 :
	1. CPRI_Recovery_Path_St : AAU의 경우, register "RW_Recovery_Clock_Set" bit31을 참조.
	2. RVS ALC : Gain compensation range : 20dB -> 30dB : Gain_Comp_x2
	3. GUI Main UL Att 표시 : 음수일 때, 0으로 표시되도록 수정.(d_UL_Att_x10)
	4. 기존 ["processing-delay"] -> ["processing-delay" + "system offset"]
		현재 "system offset"은 0으로 사용중. 일단, PRU 처럼 동작.
		Tsync 제어부분도 같은 방법으로 수정.
	5. AAU 구현 상태 변경으로 CPRI Recovervy Path status 에 대한 F/W 처리 추가 : 31번비트 확인.
	6. 대표값 일부 수정 : RSSI 기존 -80dBm미만 조건에서 -92dBm 미만 조건으로 변경
	7. RU ID 할당 받으면, LS로 부터의 RU ID 재할당, Noti-ACK 에 대해, 응답/처리 하지 않도록(packet 버림.)
	

V08.23.00 : Not Release
	1. Test Pattern Level 조정 수정 및 range +/- 10.0[dB].
	2. LS 상태응답 : AID 0xB100 대신, TSSI_dec_Pre_EQ(AID 0xC073) 사용. 
	3. AID 추가.		
		Delay 제어 수신 값(0xC071), 
		Delay 제어 수신 return 값(0xC075)
		Hidden Version(0xC072), 
		TSSI EQ전 Decimal(0xC073)
	4. "__AAU_GAIN_RANGE_60dB__" : Digital-Gain-Range 40dB -> 60dB.

V08.22.00 : Not Release
	1. AID 0x500D Added : 0x06(Completed)가 아니면, AMP/Payload OFF
	2. Inv.S/N 변경되면, Serial Number(AID 0xB007)도 같이 변경.(AAU type 변경시에도 적용.)	
	3. Test Pattern Level 조정 건 : GUI 수정 필요.
	4. EQ 기능 변경 : BW 가변 시 Gain 편차가 발생 되어 DTU Bit File 변경에 따른 수정

V08.21.00 : Not Release
	1. Optic Delay(Round Trip Delay) 0x5405 항목 개수 수정 2 --> 1
	2. LED 자동 Off 0x5455 :  Default: ON,  Off 시간: 5분


V08.19.90 : 
	1. Samsung L/S ID 계산식 변경 : "(port_id/8) + (slot_id*2)"	-> "(port_id/8) + (slot_id*2-2)"

V08.19.00 : Not Release
	1. 초기 부팅 후 amp off 주장비사와 초기화 완료 후 amp on 하도록 변경
	2. Samsung L/S ID 계산식 변경 : "(port_id/8) + (slot_id*2)"	-> "(port_id/8) + (slot_id*2-2)"
	3. AAU 생산 버전(v08.14.99) merged.
	4. console에서 실행파일 실행시, 버전만 표시하는 옵션 추가.


V08.13.01 : 
	1. AFE UL ATTEN
	2. Factory Mode : BW 제어되도록 수정. pll_alarm이어도 Register 제어 가능하게 수정.
	3 파형감시 offset : AID Added.
	4. "V08.10.00" 에서, S/N 제어 안되는 부분 수정.

V08.12.00 : Not Release
	1. 중복 AID 0xC05F "48V DC Fail limit" 삭제 : AID 0xB60A/0B 의 중복으로 인함.
	2. gREMS stat : "DL_TSSI_Decimal" is not stat : AID is 0xB100
	3. TSSI Decimal Offset (AID 0xC06F) Added.

V08.10.00 : Not Release
	1. log Added : 	
		i.	제어 명령(0x01, 0x48, 0x4C), Download명령(0x57,58,5a,5b) FTP 다운로드(0x50,51), 전원 Reset(0xEA),
			Layer Splitter Delay 제어, VSS Reset 수신시 저장.
		ii.	L/S 프로토콜 중 RU ID 할당 완료 및 Init Noti 수신 패킷 및 시간정보 저장
		iii. 제어 이력 파일 1개에 패킷 저장 총 200개 저장

V08.09.04
	1. UL Gain Check 계산식 수정
		GC_RSSI_RMS 레지스터 값 + 70 + DL ATTN(AID : 51 06) + UL ATTN(AID : B2 24)		
	2.	UL Noise 체크 동작 시간 :  3분마다 주기적으로 체크
	3.	UL Check 체크 동작 시간 :  5분마다 주기적으로 체크
	4.  Height-balance on/off, factory onoff // ON(1), OFF(0) 로 수정.(반대로 되어 있던 것 수정.)

V08.09.01
	1. L/S : Delay 제어시 정상/비정상 케이스 log 출력 요청
	2. L/S Saumsung 일때, BASE_MAC = Syste_MAC 인 경우 수정.
	3. Alpha 미사용 시, L/S-ID 를 default 0으로 표시하도록 수정.
	4. gREMS TX port : 50000 -> 20000


V08.08.10 :
	1. vss데이터 접근전  cpri mmap을 읽도록 수정
	   (현재의 CPRI-lock 상태를 read 해서, VSS read/write 처리.)
	2. VSS MAC/IP 연속 check : "1초 x 10번" -> "100ms x 10회" 로 수정.
	3. VSS REC-Port-No : 연속 10회 stable check 추가.(cascade-id 는 이미 추가 되어 있음).
	4. "2019.08.01 : Restart 시, TDD Mode DL Only" 부분 삭제.
	5. 전원 reset 시, TDD Mode 제어 안되는 부분 수정.

V08.07.00 :
	1. TEST Alarm log 삭제(원래 기능으로 원복)
	2. gREMS 통계 : 통계 보고 후, 누적 된, HardClipCnt, BIP_ErrCnt 를 초기화 하지 않도록 함.
	3. gREMS : "GUI-Password/ID" Request, "Local-Time" Request added
	

V08.06.00 : Mot Release
	1. gREMS Stat : init bip-err-accumulted, hard-clipping-accumulated
		Init__HardClipCnt_Accum(), Init__BIP_ErrCnt_Accum()
	2. AID 0xC06C : Height Balnance On/Off
	3. AID 0xC0C0 : Factory Mode On/Off
	4. L/S Valid-indicator 0x01 되기 전까지는 gREMS Alarm Report 하지 않도록 수정.
	(L/S Valid-indicator 0x01 된 후, normal 로 alarm 변경 report 되는 것 수정.)

V08.02.01 :
	1. L/S : "Initialization Information Acknowledge IE", "Lower node ID Configuration IE"
		[1] message copy 할때, IE header 의 'IF Length' 만큼 copy 하도록 수정.
			length size 오류 : 4byte -> 2byte 수정.
		[2] samsung 의 경우, 'IF Length' 가 타사 보다 큰것애 대해 max 처리.
	2. alarm log added ( alarm, control) : Test Mode (5분 -> 15초)	


V08.01.02 :
	1. When FAN & Temp Alarm, payload off, all-amp-off.
	
V08.01.01 :
	1. DTU_SD_By_AMP_Not_Use : AMP 미사용으로 인한, Payload off, AMP OFF Added.
	2. Reset 시, FAN Mode 가 항상 Normal 로 되는 부분 수정.
		(FAN Mode Init : from PSU Task -> Alarm Task)
	3. DTU 초기 설정 값(RCU 제어가 없는 경우) 정의
		[Added] TDD Mode DL Only
		[Added] Payload Off
		[Already] Amp Off
		[Already] Atten Max
	4. DTU Sub-Voltage(AID 0xB438) : 상태응답 시, 미사용 항목(2,3,4번째) 0으로 default set.
	

V07.31.00 : Not Release
	1. "V07.30.99" 적용 : 
	    V07.29.04 에서 __USE_Spectrum_LOW_Limit__가 define이 지워져 있어서 
		이 부분을 아예 없에고 전부 적용 하는 것으로 바꿈. 안 쓰이는 부분은 #if0으로 막음.


V07.29.00 : Not Release
	1. 전압(DC 48V) 상한/하한(0xB60A/0B) added : AID 0xC05F 와 유사. 
	2. 전압(DTU Main) 상한/하한	(0xB60C/0D) : AAU에 해당사항 없어서, 추가 하지 않음.
	3. LS ID 추출방법 변경 적용(L/S 가 Saumsung의 경우에만.)
	4. LS REC 가 NKA/ERS 의 경우, Base-IP 첫번째 byte 가 192인지 확인하는 조건 추가.



V07.27.00 : Not Release
	1. L/S : alarm 관련 string 변경. (AID 문서의 string 처럼 변경)
	2. 무신호시 대표값 표현 
	[1] TSSI Detect   : -90dBm, -65dBm 미만시 (Alpha : 100MHz Only 경우 -90)		
	[2] TSSI Detect(Dec.) : 0, 50미만 조건시(Alpha : 100MHz Only 경우 0)
	[3] RSSI Detect   : -100dBm, -80dBm 미만시(Alpha : 100MHz Only 경우 -100)	
	[4] PD Power : -40dBm, -21dBm 미만 조건



V07.26.04 : 
	1. AutoConfig-Restart, When,
		(1) REC Port Num 변경.
		(2) cascade-ID 변경.
		(3) base-MAC/base-IP 변경.
	2. register : Pll Addr/Data setting.


V07.26.03 : 
	1. AAU RX ALC Timeing : 200[ms] -> 600[ms]
	2. local GUI 연결이 꾾어 졌을때에 bluetooth 재 init
	3. 통신보고 알람이 꺼졌을 경우 블루투스 reinit
	4. AAU 파형감시 offset inband 대역 변경.


V07.25.02 : 
	1. L/S 이후에 Bluetooth 이름 설정 및 재 init


V07.25.00 : 
	1. L/S packet Debug : string display
	2. FPGA Download 수정. (file update 안될 수 있는 부분 수정).
	3. gREMS Suppress : test 3회 -> 기존 10회.


V07.23.00 : 
	ALC Timing TEST

V07.20.02 : 
	1. CFR Calculation 방법 변경 : EQ1 Added.

V07.20.01 : 
	1. L/S Delay Setting : Num of CPRI 값이 "0"이거나 "2"를 초과할 경우 Error Return(-502, TBD).

V07.20.00 : 
	1. L/S : "Send_LSH_UDP_Data" 함수 pthread_mutex_lock 위치 변경.
	2. L/S : delay setting : cpri_num parsing 이 0으로 되는 부분 수정.



V07.18.06 : 
	1. gREMS : Alarm String Added
	2. gREMS : Inventory Auto Report When SFP info is changed.
	3. AMP Standby Mode / MIMO Standby Mode : 동작 시에  'disable' 알람 마스킹
		(1) Mode 진입 전 알람은 유지하되, 진입 후에 발생되는 알람은 Mask 처리	
		 == 진입되면, 진입전 알람은 유지되고, 진입전에 정상이었던 것은 알람이 되지 않도록,
		 == mode 진입전 알람상태(normal or alarm)을 진입 후에도 계속 사용.
		(2) AMP 관련 alarm,  TSSI/RSSI, Power 상한/하한 알람 부분에 대해 처리.
	4. 	Initialzation Notification Acknowledge를 받기 전까지는 
	상태 요청 등에 대한 메시지에 대한 응답 X

V07.18.05
	1. suppress 10 -> 3, clear time 60[s] -> 30[s]
	2. dtu board alarm : message every 10[s] 표시.
	3. clock alarm : payload off
	(register 0x07 pll 부분의 alarm이면, 현재 register 제어 안되나, clock alarm의 경우 payload off는 register 제어 가능하게 수정)

V07.18.04
	1. Fan 저소음 Mode인 경우
	-> FPGA 온도 90도 이상시 Fan Mode : 일반 Mode로 변경  
	-> FPGA 온도 85도 이하시 Fan Mode : 저소음 Mode 복귀

V07.18.03
	1.	UL RSSI 계산식 변경.
	계산식 변경은 GUI/LayerSpliter/REMS 통계등에만 사용.
	ALC, Shutdown은 계산식 변경 이전대로 사용.

V07.18.00
	1. UL RSSI 계산식 변경.
	2. bluetooth 관련	
		(1) AID 0xB00C // Bluetooth Device ID ( Status ) 추가 
		(2)	AID 0xB00D // Bluetooth Pairing Code (Status / Control) 추가 
		(이 부분이 control 되면 Bluetooth reInit)
	3. 파형감시 관련.
	(1)	AID 0xC06A/6B : AAU 파형감시 DL/UL Inband offset (Status / Control) 추가.
	(2)	파형감시 AAU inband 필터 적용.
	(3) 파형감시 I / Q data dump 기능 추가. Dump된 파일들은 전부 /tmp아래에 저장됨. 
	
	
V07.17.00 : Not Release
	1. Loopback On/Off : (제어 X, 상태 Only)
	VSS 필드의 Loopback 필드값이 0xCC, 0xCA인 경우 On, 이외 Off로 처리
	2. Function fail : 
		(1) Layer 미사용 시 FuncFail ALM 발생하지 않는 부분 수정.
		(2) defalut가 0xCC로 시작, 이후 알람 체크 시작부터 상태에 맞게 변경 요청 :  
		(정상은 0x33, 그렇지 않으면, 0xCC로 이미 구현되어 있어서, 추가 수정부분은 없음.)
	3. PRU 및 AAU의 초기 부팅에 대한 DTU 정상 여부 판단 기능 변경
	4. L/S로 부터, ID 할당 받기 전에, BaseMac/BaseIP 변경되면, RCU MAC/IP 변경 추가.
	
V07.16.08
	1. CPRI alarm/SFP not insert : register 0M Control, but AID 0x5008은 VSS대로 표시.
	
	
V07.16.05
	1. height balance att : dtu digital DL att 에 추가.
	2. CPRI alarm/SFP not insert : register 0M Control, but AID 0x5008은 VSS대로 표시.
	3. gREMS : RCU Start 시, 통계주기 부족한 1회에 대해, statistics 1회 report 하지 않음.
	4. AID 0x5620 : Reset : Command 0xEA 처럼 동작하게 수정.
	5. 상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어
	6. Test Time 
: test 시간 원복.
		#undef _SHORTEN_TIME____AUTO_RST_
		#undef _SHORTEN_TIME____PAYLOAD_

V07.15.01
	1. CFR 계산 식 변경.
	2. EQ (AID 0xC014/19/1A/1B) : unsigned -> signed 변경 : GUI 수정도 필요.
	3. height balance att : dtu digital DL att 에 추가.
	4. RCU Restart 시, 초기에 rf att (DL, UL) 30dB 제어 추가.

V07.13.00
	1. VSS MAC Fail Check added : "__USE_MAC_CHECK__"
	2. Test pattern : 5회이상은 retry 하지 않고, Message만 Print.
	3. BW 100+a에서 100Mhz 로 변경시 alpha의 Payload Off하도록 기능 추가
	(반대로 100Mhz -> 100+a로 변경될 경우에는 Payload 자동 On)
	4. CPRI#1 은 SFP 탈실장 Alarm : 재확인 test 필요.


V07.12.02
	1. CFR 계산 변경.(log 삭제)
	2. Register 계산식 오류 수정.
	10*log10(RSSI/2^32) --> 10*log10(RSSI/2^31)
		UL INPUT RSSI DET (0x6)
		DL_TSSI_RMS  (0x04)
		DL_TSSI_PEAK (0x09)
		UL_RSSI_RMS  (0x22)
		UL_RSSI_PEAK (0x29)
		FB_RSSI_RMS  (0x30)
		UL_NOISE_RMS (0x38)
	단,
	FB_RSSI_PEAK (0x31) : 현재 Register 미사용 상태.
	GC_RSSI_RMS (0x32) : 실제 계산에 2^31 부분 없음. 

V07.10.00
	1. A_ID 표현 방법 : 이미 구현되어 있음.
		- TBdelayDL(DL Hopping Delay) 0xB42C 표현방법
		- TBdelayUL(UL Hopping Delay) 0xB42D 표현방법
	2. 자동보고 기준시(AID 0x5603) Added
	3. GUI_ID (AID 0xB012), GUI_Password (AID 0xB013) Added
	4. Alarm Mask (AID 0xB014) Added


V07.09.00
	1. (z.16.0) 및 (z.80.0) 구현 : 
		Main_AID (CPRI, 0x5301) 의 LOS/LOF 발생 조건을 동일하게 참조.
	2. (z.22.0)Optic LOS Detect/Alarm 구현 : 
		Main_AID : LOS(SFP) 0x5302 참조.  Alarm(0xCC), 해제(0x33)

V07.08.01 : not Release
	1. AAU Test pattern을 위한 Register가 추가 

V07.08.00
	1. Masking alpha-SFP-alarm : "SFP_Ins_0_a" When BW-Alpha is not used
	2. FPGA Delay when CFR On/Off (CFR On/Off 시, 1.262u 만큼 변경/제어)
	- FPGA 설정 Delay가 50[us] : CFR Off시 FPGA 설정을 51.262[us]로 제어
	


V07.04.07
	1. CFR threshold 보정관련 수정
		AID 0xB404 : CFR --> PAR
		AID 0xC069 : PAR --> CFR Result Decimal
	2. gREMS : CF/nC Auto Report Added (When AID Value is chnaged)
	3. Wave__DL_Offset (-35*10)
	
V07.04.06
	1. AID 0xB40D(Hard_Clip_Lvl) : 
		hard clipping off 일때는 이전의 사용자 제어값을 보이도록 수정.
	2. CPRI not insert 시, BIP count(순시/누적) 0으로 reset.
	3. CFR threshold 보정관련 수정 및 Register 적용.
	4. Continued_download : 공용GUI 에서 안되는 부분 수정.

V07.03.00
	1. CFR threshold 보정 추가.
	2. 파형감시 변경. PRU AAU
	3. FTP 전송 시 fail 여부 전송하는 부분 추가
	4. BW 관련 수정.( alpha 10M/20M 는 0M 처럼 처리)

V07.02.06
	1. cpri unlock(alarm) timing check 오류 수정
	2. "SFP Not Insert" or "CPRI Alarm" 일때, BW 제어 변경 : 기존 10M 제어 -> 0M 제어
	3. FAN 알람 && 온도 상한 알람 동시 발생 : DSP와 AFE reset, 해제시 reboot.
	4. GPIO_DIR_Mode_Out( GPIO_CONF_DAT_EN, GPIO_VALUE_HIGH); // 2019.07.02 : Low -> High
	

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

V06.28.11
	1. SFP 탈실장 0x5315 Alarm 추가
	2. 국소명 0xB010 추가 	
	3. RU Initial status 0xB009 삭제 -> Main 이동  

V06.28.10
	1. FPGA Data line OFF 기능을 지원하는 GPIO 추가(AAU 신규 보드 경우)
	2. FPGA 다운로드 문제 해결 : FPGA 다운로드시 임시 파일 경로 변경하는 방법사용.
	3. RU Initial status 0x500D
	4. VSS : 10회 연속 수정 : BW 부분 잘못된 부분 수정.

V06.28.01
	1. Temp Atten for Alpha-DTU added.
	2. CPRICompressionOnOff : AAU
	3. AAU : RF ATT(DL/UL) : max Att 31.5dB limit
	4. RF_LNA_OnOff : 제어 안함. FPGA 제어 안됨.
	5. L/S : 
	(1) "Alarm_Data_Vaild_Indicator" 에 "Vaild_Indicator" 값 응답하도록 수정.
		(수정전 항상 1을 응답.)
	(2) "INIT_NOTI_ACK" 받으면, Vaild_Indicator 를 1로 설정하도록 수정.

	6. default IP : 항상 192.168.0.3 로 시작.(기존 : 전에 사용되었던 값을 사용.)
	7. FGPA Test Register 사용 : 연속 2회 Fail 이면, FPGA read/write 하지 않음.	
	8. CPRI lock과 CPRI Alarm을 같이 동작 : 해제 10초, 알람 2초.
	

V06.28.00
	1. FPGA init Check 방법 변경.
		clock alarm (m_St.AlmSt.A_4.Clock) 대신, 
		m_Reg_Sts.R_Board_Status Register 의 2개 bit(Pll_Init_Done ,JESD_Lock)를 check.
	


V06.26.01
	1. spectrum offset 변경.	
	#define Wave__DL_Offset (-25*10) // 2019.06.26.
	#define Wave__UL_Offset (-80*10) // 2019.06.26.
	#define Wave_etc_Offset (-80*10) // 2019.06.26.
	
V06.26.00
	1.	Noise / UL GC Detect 측정값 Register 분리
		A. UL GC(UL Gain Check) - 0Path기준 address : 0x11132
		B. UL Noise 측정 - 0Path기준 address : 0x11138



	

V06.25.00 : by YCSo, NOT Released.
	1.	PRU에서 GPIO_OPT_SW_Status0 ,  GPIO_OPT_SW_Status1 에 대한 변경.
	2.	AAU에서 DL 파형 감시 시 8K size -> 4K size로 변경
	3.	PRU의 파형감시의 필터 값 조절
	4.	다운로드시에 수행되는 쉘의 변경.


V06.21.01
	1. LED Alarm( blue X, Red O)에 아래 추가.	
		UDA_CPRI_Reset_Suppression, UDA_Clock_Reset_Suppression
	2. LED Alarm( blue O, Red O)에 아래 추가.	
		LOS_SFP_1_a, LOS_SFP_1__, LOS_SFP_0_a, LOS_SFP_0__
		LD_SFP_1_a,  LD_SFP_1__,  LD_SFP_0_a,  LD_SFP_0__	
	3. Hidden_AID 0xB432 "SFP On/Off" 제어 안되는 현상 수정 : Register 0x07
	4. RU ID Acqu' Noti  &  Ack  완료 전에 Initalization Noti Ack를 수신하지 않도록 구현.

	

V06.21.00 : by YCSo, NOT Released.
	1.	파형감시 필터 적용시 부분 필터 계수 적용.
	2.	다운로드시 새로운 F/S에 맞게 변경 부분 수정.( 아직 정식 적용은 아닌 상태)

V06.20.00
	1. Register 0x30, RE RESET : 동작 되게 수정.

V06.19.00
	1.	PRU & AAU register read / write 부분의 address 값을 0x200값을 곱해서 GUI에서 편하게 조회
	2.	APP 실행 후, RCU APP version 정보 표시 
	3. 	AAU 전원 리셋 동작을 RCU Reboot과 동일한 방법으로 수행.
	4.  CPRI Alarm 발생->해제 시 10초 조건을 CPRi#2, CPRI#3에도 동일하게 적용.

V06.18.05
	1. MIMO Standby 시작 동작 조건 변경 : all amp 동작조건 만족할 때 동작하도록 수정.
	2. Alpha BW 가 0/10/20 의 경우에도 BW 가 자동으로 설정되도록 수정.
	3. RCU Temp Offset Added : AID 0xB417
	4. FAN Off Mode  구현
		(1) FAN 저소음 모드시 RPM 50%로 표시
		(2) 자동 제어 Off 후 RPM 50% 제어시 저소음 모드 속도로 FAN 제어
		(3) FAN Off Mode 설정시 상태 동일하게 변경 및 RPM 0%로 제어
	5. FPGA Register Read/Write : 
		- download 중이 아니어도 동작되게 수정.
		- cpri Unlock 이어도 동작 되게 수정.
	6. AAU 전원 Reset Command 구현
	7. 초기 부팅 후 알람 보고 시점 : L/S에서 수신되는 Alarm Acknowledge == 0x01(Acknowledge)인 조건인 경우
		when Acknowledge = 0x01 ?
		(1) Init Noti 수신시 Alarm Acknowledge가 0x01(Acknowledge)인 경우.
		(2) Init Noti를 처음 요청한 시점부터 1분 경과 후, 자체적으로 0x01(Acknowledge) 변경
	8. CPRI/Clock Fail 알람 해제 조건 10초 추가/ 기타 알람 발생/해제 시간 변경 할 수 있도록 구조 변경(PRU/AAU 공통)	
	9. VSS 필드 기능 미구현 분 구현(PRU/AAU 공통)
    	(1) LOS/LOF OPTIC LOS 알람 구현 필요
    	(2) Function fail 구현

V06.14.01
	1. TEST Version : "_DO_NOT_FOLLOW_VSS_Table_When_TDD_Slot_" : 삭제.
	2. VSS UL : RE_Loopbackmode_ACK 즉시 응답하도록 수정.

V06.14.00
	1. L/S Delay 에러 응답시 byte order 부분 수정 : result.sD32 = htonl(rst)
	2. TEST Version : "_DO_NOT_FOLLOW_VSS_Table_When_TDD_Slot_"
	3. VSS UL : RE_Loopbackmode_ACK 즉시 응답하도록 수정.

V06.13.05
	1. UL Gain Back Off 3단 Cascade 연결시 : 4.5dB --> 4.8dB 로 변경	
	2.	평상시는 C.W Level을 "0"으로 설정하며, UL Gain Chech Mode 일 경우만 
		C.W Level을 "8"로 설정 후 측정. 측정 완료 시 다시 C.W Level "0"으로 설정  
	3. 	Center Frequency 제어 기능을 추가하면서 PRU에 RE Channel BW에 대해 다음과 같이 구현 요청 드립니다.(~06/13)
		a.	VSS Table 값 따라가도록 복원
		b.	BW 변경시 Main/Alpha에 대한 Defalut Center Frequency 적용 및 저장
	4. T2a(DL_Delay), T3a(UL_Delay) 제어 값에 대한 예외 처리 추가 

V06.11.01
	1. 0x5209 Gain Balance : max 15dB limit added, 소수점 수신시 내림 처리
	2. "Vss_UL_RE_Reset_Ack 응답속도 개선" added to AAU
	3. MIMO Standby 진입/해제 level : 0x543A/3B 추가.
	
V06.10.02
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

V06.09.01
	1. RSSI Display = RSSI_dBFs_x10(DTU) + RSSI_Offset + UL_DTU_Gain_Backoff + UL_GainBal

V06.09.00 // not release
	1. UL Gain Balance의 Att : UL Gain Back-off on/off 상태와 무관하게 값 적용.
	2. RSSI display 식 수정
		AAU 변경안 : UL 입력(GUI의 RSSI) = RSSI(Digital) - (30dB - UL Offset - UL Att- Back-off Att - Gain Balance Att)
	3. 상위 CPRI 알람 발생(100MHz, aMHz 개별)시 하위 SFP Tx disable로 제어 및 LD 알람 Masking
		레지스터 기준으로 CPRI#0[#1] 알람 발생시 CPRI#2[#3]의 SFP Tx disable 제어

	4. 구 A_ID 항목 변경(구 A_ID --> 신규 A_ID로 변경)
	 - 제조사 0x0100 -> 제조사 0x5500
	 - 공급사 0x0101 -> 공급사 0x5501
	 - 중계기 Type 0x0102 -> Type 0x5502
	 - S/W 버전(통합) 0x0149 ->  S/W 버전 0x5503
	 - 함체 온도 0x0201 ->함체 온도 0x5012
	 - 함체 온도 상한 0x0202 -> 함체 온도 상한 0x5013
	 - RCU D/L State 0x2B30 -> D/L State 0x5510
	 - Reset 0x0914 -> Reset  0x7020
	 - 통계보고 기준시 0x55 0x00
	 - 통계보고 주기 0x55 0x01
	 - 통계 시작 시간 0x55 0x02

V06.04.00 : /////YCSO
	1. App 실행 시 5초 후에 FPGA 버전 consol에 표시하도록 변경.
	2. cmd 0x99 Register Read Write 기능 추가. 

V06.02.15 : 
	1. Gain Balance Added to VSS-Z.30 UL-Gain-Backoff	
	2. LOS(SFP)	AID 0x5302
	3. LD(SFP)	AID 0x5304
	4. AAU : Clock Fail 일때 payload all off.

V06.02.10/11/12/13/14
	1. AID 0xB010 : AAU Type : Control Added.

V06.02.04
	1. debug : syslog -> printf , 일부
	2. NOKIA/Ericson : MAC Address : fused MAC 사용하도록 수정.(PRU의 경우, mac 구입 저장 필요)
	3. LED Alarm : CPRI cascade alarm added
	4. gain backoff : off -> ON 변경시, 설정 추가.


V06.01.05
	1. FAN Alarm 수정.
	2. Gain Check Mode/Noise Check Mode



V06.01.00
	1.	Equalizer 관련 Register : 온도상한 값이 짝수이면, 기존, 홀수 이면, 아래 추가된 register
	2.	RF ATT : user att, offset att 구분
	3.	파형감시 register 0x1E : DL(0x06), Noise(0x08) 구분 제어 추가. 


v05.31.02
	1. UL ATT 표현 변경/추가 : AID 0x5216을 기존 RF ATT에서 Digital Att으로 변경.
	2. GUI 버전에 따른 DL-Mode-Enable 변경 : 최종 GUI(v84)에 맞춤
	3. GC_RSSI_RMS = 레지스터값 + 44 -DL Gain(출력-TSSI) + UL Atten(AID 0x5206)
	4. AID 0x5207 UL Gain : 계산 변경 : 30 - UL Atten(AID 0x5206)

v05.30.01
	1. AID 수정.
		- T2a(DL Delay) 0x5403 항목개수 수정  4 -> 1
		- Ta3(UL Delay) 0x5404 항목개수 수정  4 -> 1
	2. 항목 삭제 : NG-ARFCN 0x5007
	3. Gain 저하(UL) Threshold 0xB305 Data type: 0x04 -> 0x03 변경
	4. ALC On/Off(DL) 0xB23B (기존 AID 0xC04B를 변경)  
	5. Auto Shutdown Mode On/Off(DL)  0xB23C (기존 AID 0xC050을 변경)
	6. UL Gain Balance 0x5209 : AID 만 추가.
	7. Offset(UL Noise) 0xB309 : same with 0xC05C AID
	8. MIMO Standby Mode(진입/해제 Level) 0x543A/3B
	9. Type AID 0xB010 추가
	10. Offset(DL 출력) 0xB210 추가
	

v05.30.00
	1. AAU_UL_NoiseDet_Mode : AID 0xC060 Added.
	2. AID 0x500B(Center Freq) : 0.00.1 -> 0.001Mhz step 변경.
	3. 파형 감시 할 때, "AAU_UL_NoiseDet_Mode" 추가 : register에 data 기존 0x07 -> 0x0E 제어.
	4. PAU/LNA Off 시, payload off added.
	5. UL Cascade on/off : reset 시 초기 제어.

v05.29.01
	1. MAC/IP Set with IOCTL --> 원래대로 수정.

v05.29.00
	1. AFE PLL 알람 정보 : Register 추가(0x3F)
	2. loopback register control : 시간상 register 다음에 VSS-loopback 제어이나, 
	VSS-loopback 제어전에, register 를 먼저(대략 100ms) 제어되게 수정.
	3. MAC/IP Set with IOCTL

v05.28.01 // not release
	1. AID 0xC04E/4F : T2A/T3A delay (Read Only) 용도 변경.
	2. UDA Spurious 동작 조건 : "threshold (+/-)3dB" -> "over threhold level"
	
v05.28.00
	1. 48V-DC-Fail-limit (AID 0xC05F) 적용.
	2. t-sync-offset : reset 후 init value 적용되도록.

v05.27.01 // not release
	1. 48V-In-Voltage-Offset(AID 0xC05E) added.
	2. 48V-DC-Fail-limit	(AID 0xC05F) added.
	3. When Cascade-SFP-Not-Insert : TBDelay, RoundTrip Delay : 0xFFFFFFFF 처리.

v05.27.00
	1. FAN Alarm : alarm when adc is below 1.5
	2. 48V-In-Volt Conversion Added.
	3. UDA : Test Time
	4. 연속적이지 않은 VSS RE_CH_BW 처리 : 연속 10회 일때, 정상적인 값으로 처리.
	5. UL Gain Check : Gain 값에 대한 Offset 값 추가
	6. Register : T2A, T3A : delay 계산에 적용.
	7. RF Delay : DL(8ns), UL(13ns) 적용.

v05.25.00
	1. AAU_Fwd_Shutdown : when over shutdown-level and over max-fwd-alc-att(15dB)
	2. MIMO_Standby_Pwr_Level : -34.0[dBm] -> +12.0 [dBm]
	3. Temp Alarm : hysteresis 수정. : 2 -> 5
	4. L/S : IE_System_Sts : 
		- Batt insert 정보 : AAU(0:미사용), PRU(1:insert, 2:not insert) added.
		- DC_In_Volt : -48V, DC Fail Check (AAU)
		- DC_Out_Volt : RCU input 5.6V (AAU)
	5. L/S : alarm : AAU : VSWR 제외.
	6. AMP-Standby, Mimo-Standby, Fan-Mode : 
		- 동작 중, 시간이나 mode(적용 path)를 변경하면, mode off 제어되게 수정.


v05.24.00
	1. AID 0x5405 : round-trip-delay : 상태응답 flag 수정 : 0xC0 -> 0xF0
	2. UL_GAIN_Measure_Time : AID 없으므로, RCU Reset 시, 초기 제어 하지 않도록 함.
	3. m_St.T2A_Dly__ns[optic], m_St.TA3_Dly__ns[optic] : AAU는 적용 안됨.
	4. AID 0x5207 : UL Gain added.
	5. alarm LED 수정 
	

v05.23.11
	1. AID 0xB429 FPGA Delay (DL/UL) : register-conversion-value changed.
	2. AID 0xC058 : UL-Gain-저하-Threshold added.
	3. AID 0xC059 : DL-Mode-Enable Added.
	4. NOKIA L/S 연동 Added.
	5. RCU reset 시 초기값 제어
	-	T sync delay : 추가.
	-	TDD Guard time : 상한/하한 비교하는 부분 삭제.
	

v05.22.00 // not release
	1. UL Gain backoff : 'RF_Att' --> 'UL_Gain' 이동.
	2. 

v05.21.00 // not release
	1. register : "SFP LOS Alarm","SFP LD Alarm" added
	2. AID 0x5403/04 (T2a/Ta3 delay): GUI - Control 가능하게 수정.


v05.20.02
	1. LS ID Added : Samsung/Ericson(Nokia) : Base IP 에서 LS ID를 추출.
	2. LED Alarm 수정.
	3. Temp Alarm : hysteresis 수정. : 5 -> 2
	4. TSSI upper Alarm : hysteresis 수정. : 2.0 -> 2.1
	5. Lock 판단 조건에 miscelaneous added.
		CPRI_0/1) if(CPRI_Mscl_16 == 0) CPRI_Mscl_Lock = 1;
		CPRU_2/3) if(CPRI_Mscl_16 == 0x0080) CPRI_Mscl_Lock = 1;



v05.18.01 // not release
1. RVS Spurious Test 확인.
2. CPRI1(alpha) not insert 시, BW 10M 제어.


v05.17.03
1. spectrum -----> AAU Check Needed !!!!
	- "__SPECTRUM__OFFSET__" : Not defined
	- 파형감시 filter 적용방식 변경.
	- filter : 계산식 부호 변경.
	
2. UDP download 속도 개선.
3. ATTEN(ALC)(DL)	0xB22A added.
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
7. Vss_UL_RE_Reset_Ack 응답속도 개선. : "__USE_VSS_UL_RE_RESET_ACK_SW__"
8. Gain 저하(UL) Threshold  0xB305 추가
9. L/S : "OPTIC_TRANSCEIVER_RX_LOS", "OPTIC_TRANSCEIVER_TX_FAULT" Alarm added.
10. 온도보상 AID Added ; temp-att is not implemented.







v05.16.02 // not release
	1. Download 이어받기.
	2. 파형감시 수정.

v05.16.01
	1. TDD Sync (AID 0xC021) : register conversion 수정.( GUI에서 usec 단위로 제어하는 것으로 가정.)
	2. FPGA User Delay (AID 0xB429) 혹은 L/S로 인한 FGPA Delay 제어시, TDD Sync (AID 0xC021) 되도록 수정.
	3. AID GC_RSSI_RMS_x10(0xC053), UL_Gain_Chk_Mode(0xC054) Added.
	4. Guard Time offset : register 제어시 1228 offset 삭제.
	5. DL CW Level, Onoff : Level EEP Save, CW Off when reset.
*/

/* EOF */
