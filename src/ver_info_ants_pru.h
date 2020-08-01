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
	99.01.03 : 2020.02.14 : gIRO DVT ��ɽ��� �غ� ������ UDA Test F/W
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
udp �ӵ� ����
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
1. L/S �����(Read only)
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
PRU : register ���� �� "halt" 
AAU : "reboot"

(2) AID 0x5620 / by user control
PRU : register ���� ���� "reboot" (2020.03.20)
AAU : "reboot"

(3) Download / by download
PRU : register ���� �� "halt" 
AAU : "reboot"

(4) VSS restart / by VSS
PRU : register ���� �� "halt" 
AAU : "reboot"

(5) UDA PD(CPRI)/Clock: by UDA
register ���� �� "halt" (10s ������ �ȵ� ��찡 �ִٸ� "reboot")
"reboot"

--------------------------------------------------------------

PRU "halt" �� ��� 
[1] ���� 
register(PSU Reset, register 0x33) �� 0x02 data ����.
0x02 ����Ǹ� "halt" ����.

[2] ���� : 2020.03.15
register(PSU Reset, register 0x33) �� 0x02 data ����.
1�� ��, 0x02 ���� �� ���� ���� ������� �ٷ� "halt" ����.


��, �� [2] ���� ������� ��� ��쿡 �����ϰ� �����ϵ�, 
�Ʒ� "��û����" �� ���������� ����.

"��û����" :  (Reset ���� �� System�� ���� ���� �ʴ� ��� timer 30�� ��� �� Halt �������� System Reboot ����


*/

/*
1. ftpget / ftpput
    - anonymous �� login �Ͽ� ftp �� get �ϴ� �κа� put �ϴ� �κ����� �Ǿ� �ֽ��ϴ�.
    
3. unzip
    - linux���� zip file�� Ǫ�� program�� "unzip" �ε�, �̰��� �̿��Ͽ� ���ϴ� file�� Ǫ�� ������ �����Ͽ����ϴ�.
    - �׳� shell ���� ����� ������ ������ �����ϴ�.
         # unzip test.zip            # zip file ��ü�� Ǫ�� ���
         # unzip test.zip one_file # zip file�� one_file�� ���� ���, �� file �� Ǫ�� ���
         # unzip -t test.zip         # zip file�� ���¸� �˻��ϰ�, ���� file list �� ǥ��
         # unzip -l test.zip         # zip file�� ���� file list�� ǥ��
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
2. task ���� ȭ�� ǥ�� : download ��, update ���� �� Ÿ�Ӿƿ� ��?
	���� ���۽��Ѽ� Ȯ���ϴ� ���.
3. SYSLOG Ȯ��.�󸶳� ����, ���� ??
=====================================================
To do
=====================================================




Get_OnOff_Str(m_St.Pwr_Amp_En[path4]),


reboot �Ǹ�, eMMC write ���� ���� �ʵ��� ���� �ʿ��


ru init 6�ܰ� ������ (5�ܰ� 10��) gREMS alarm report �����ϰ�	
===========================================
*/



/* 

//-------------------------------------------
V20.07.15 (M_88.01.06) / PRU / 2020.07.28
V20.07.16 (M_99.01.06) / PRU / 2020.07.28
//-------------------------------------------
	1. Module Inventory : �������� ���� ����(SFP ����).

//-------------------------------------------
V20.07.14 (M_99.01.05) / PRU / 2020.07.27
//-------------------------------------------
	1. Module Inventory : �������� ���� ���� 
    * PRU : DTU - PSU -  PAU0/1/2/3 - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd
    * AAU : DTU - SFP_100 - SFP_100_2nd - SFP_Alpha - SFP_Alpha_2nd - RCU
    
//-------------------------------------------
V20.07.13 (M_99.01.05) / PRU / 2020.07.24
//-------------------------------------------
	1. Module download 2��ȭ ���� ��.(Un-Define "__USE_MOD_DUALITY__")
	2. SFP Inventory (0x6A Command) ����/Ȯ��.


//-------------------------------------------
V20.07.11 (M_99.01.05) / PRU / 2020.07.23 : not release
//-------------------------------------------
	1. Module download 2��ȭ ���� : 
	define "__USE_MOD_DUALITY__"

//-------------------------------------------
V20.07.10 (M_99.01.05) / PRU / 2020.07.23
//-------------------------------------------
	1. FPGA Reg.Map : SFP Vendor PN, Vendor Name, Vendor SN, Date Code �ʵ� �߰�
	2. GUI ���µ�信 SFP Vendor PN(AID 0x5316) �߰�.
	3. SFP Inventory (0x6A Command) �߰�.


//-------------------------------------------
V20.07.09 (M_99.01.05) / PRU / 2020.07.21 : without BOOT.BIN file
//-------------------------------------------
	1. gIRO-T ���Ÿ�� : PSU �������� giRO_T remote type : "0A" -> "0C" �� ����.


//-------------------------------------------
V20.07.06 (M_99.01.05) / PRU / 2020.07.17 : with BOOT.BIN : 600Mhz
V20.07.07 (M_99.01.05) / PRU / 2020.07.17 : with BOOT.BIN : 800Mhz
V20.07.08 (M_99.01.05) / PRU / 2020.07.17 : without BOOT.BIN file
//-------------------------------------------
	1. DDR-Memory Clock info AID(0xC0A5) Added.

//-------------------------------------------
V20.07.05 (M_99.01.05) / PRU / 2020.07.15
//-------------------------------------------
	1. gIRO�� gIRO-T ���Ÿ�� ����.

//-------------------------------------------
V20.07.04 (M_99.01.05) / PRU / 2020.07.15, with BOOT.BIN : 800Mhz
V60.07.04 (M_99.01.05) / PRU / 2020.07.15, with BOOT.BIN : 600Mhz
//-------------------------------------------
	1. BOOT.BIN install ó�� script �߰�.
	2. gIRO�� gIRO-T ���Ÿ�� ���� �߰�.(type�� eMMC�� ����)	


//-------------------------------------------
V20.07.03 (M_99.01.05) / PRU / 2020.07.14
//-------------------------------------------
	1.gIRO�� gIRO-T ���Ÿ�� ���� �߰�.(type�� eMMC�� ����)	
	2.SFP Inventory (0x6A Command) �߰� ��.


//-------------------------------------------
V20.07.02 (M_99.01.05) / PRU / 2020.07.08
//-------------------------------------------
	1. BIP�� Hard Clipping Count �ʱ�ȭ 
	[����] ��躸�� ��, ���� �ʱ�ȭ
	[����] ��躸�� ��, ��� �����ϵ��� ����(�ʱ�ȭ ���� ����)
	
	2. ��躸�� AID �߰�.	
	AID 0x5024 : �µ�(PSU)
	AID 0x5025 : ����(PSU)(Main DC)
	AID 0x5026 : ����(PSU)(Sub DC)
	AID 0x511A : Return loss(DL)
	AID 0x5409 : Optic Delay (�ܹ���)

//-------------------------------------------
V20.07.01 (M_99.01.05) / PRU / 2020.07.03
//-------------------------------------------
	1. "V20.06.08 (M_99.01.05) / PRU / 2020.07.02" �� 5,6�� ����.
	[����] 
	define "__Send_Init_Alarm__When_0x500D_RuInit_is_6__"
	undef  "__Seperate_AlarmLog__From__AutoReport__"
	2. PD ����/���� �˶� ����.
	


//-------------------------------------------
V20.06.08 (M_99.01.05) / PRU / 2020.07.02
//-------------------------------------------
1. AID �߰�.
	- Hard Clipping count(0x5320) (Mea: ���ð� --> ������ ����)   
	- BIP Error ���� Count �ʱ�ȭ (0x530B) : (Mea: ���ð� --> ������ ����)
	- PD Power ����(0x530C)
	- PD Power ����(0x5309)
	- Retrun Loss(DL) (0x511A)
	- Optic Delay (�ܹ���) (0x5409)
	- IP Address (�߰��)  (0x5021)
	- IP Address (LS) (0x5022)
	- IP Address (gREMS) (0x5023)
	- �µ�(PSU)  (0x5024)
	- ����(PSU)(Main DC) (0x5025)
	- ����(PSU)(Sub DC) (0x5026)
	- Hard Clipping Count �ʱ�ȭ(0x5322)
	
����--------------------	
	- Hard Clipping ���� Count(DL)(0xB40E)
	- Hard Clipping ���� Count �ʱ�ȭ(0xB40F)
	- BIP Error ���� Count(DL)(0xB427)
	- BIP Error ���� Count �ʱ�ȭ(0xB428)	
	- IP(0xB000) ����.
	- VSS(NOTI IP1) (0xB445) ����.
	- VSS(NOTI IP2) (0xB446) ����.

2. AID 0x530A
	[����] ���ð�(d_BIP_Count) ���.
	[����] ������(d_BIP_ErrCnt) 0xB427 �� �ߺ��Ͽ� 0x530A �� ���.
	[���] ������� ������ ���.(���� ���� ����)
	
3. AID 0x5320
	[����] ���ð�(d_Hard_Clip_Cnt_DL) ���.
	[����] ������(d_Hard_Clip_Cnt_Accum) 0xB40E �� �ߺ��Ͽ� 0x5320 �� ���.
	[���] ������� ������ ���.(���� ���� ����)

4. Measure �׸� �߰�.
	- Retrun Loss(DL) (0x511A)
	- �µ�(PSU)  (0x5024)
	- ����(PSU)(Main DC) (0x5025)
	- ����(PSU)(Sub DC) (0x5026)

5. gREMS�� ����� ���� �ʴ� ��Ȳ���� �߻��� �˶����� ���ο��� ���۸��ϰ� �ִٰ� 
	gREMS ����� ����ȭ�� �Ǹ� ���ۿ� ���� �˶� �����͵� ���� ���
	�ڵ�� ��� on/off�� �����ϵ��� ���� �ʿ�.	
	[����]
	gREMS.Ready �̸�: �˶�Queuing, Alarm Report, log Queuing, Log Write
	gREMS.Not-Ready : log Queuing, Log Write

	[����] define "__Seperate_AlarmLog__From__AutoReport__"
	gREMS.Ready/Not-Ready �������, �˶� �� log Queuing, Log write
	gREMS.Ready �̸�, Alarm Report

6. 	RU Initalize�� �ʱ�ȭ �Ϸ�� Ȥ�� 5/6 Delay Setup Request waiting ���·� 10�� ������
	�ᱹ gREMS�� ���� �ϱ� �����ϴ� ������ ���޽� ���� �߻��� �˶��� gREMS�� ����
	[����] define "__Send_Alarm__When_0x500D_RuInit_is_6__"

7. zModem ���� �� timeout�� ���� ���� ���� �ʴ� ���̽��� �־� �ø�������� ���� �ʴ� ���� ����.
[����] 
	Process ����͸� �� 1�а� ���� ���� ��� auto kill ��� �߰�
	sz ��� ������, timeout 30s �߰�.


//-------------------------------------------
V20.06.07 (M_19.01.05) / PRU / 2020.06.22
//-------------------------------------------
	1. 100M/Alpha insert --> 100M Ż�� -> 100M �ٽ� ���� ��, AMP Off �Ǵ� ����.
	"V20.01.10 (M_19.01.02) / PRU / 2020.01.29" ���� ���� O
	"V19.12.10 (M_19.01.01)" version ���� ���� X

	"V20.01.10 (M_19.01.02)" �����Ǹ鼭, AMP off ��Ű�� ���Ǻκ� �߸��� �κ� ����.
	"100M �ٽ� ����" �Ǿ ���� �ð� ����, 100M CPRI_Alarm�� �����ǰ� ����.
	�̶�, 100M/Alpha Common CPRI Alarm �̾�� amp off �ε�, 
	100M CPRI_Alarm ���� ����, amp off ��. 
	100M/Alpha Common CPRI Alarm �϶�, amp off �ǵ��� ����.

	2. UDA Reset : 
	[����] Alarm only �� ��, gREMS ����� UDA Alarm Set �ȵ�.
	[����] (�ٸ� �κ� ��������) Alarm only �� ��, gREMS ����� UDA Alarm Set �ǵ��� ����.
	


//-------------------------------------------
V99.06.04 (M_99.01.05) / PRU / 2020.06.22
//-------------------------------------------
	1. TEST Time
		(1) UDA TEST Time : 10��(2��)
		(2) Battery Check : 24�ð�(2��), 2�ð�(1��)
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
	(1) '88' �� ����ÿ��� LS ��Ϲ�ȣ Null�� ���� �� ����
	(2) '0' �� ����ÿ��� �α� �ʱ�ȭ
	(3) �׿� ����ÿ��� �ƹ��͵� ���� ����.


//-------------------------------------------
V20.06.02 (M_99.01.05) / PRU / 2020.06.11
//-------------------------------------------
	1. LS ������� ��� ����
	(1) Log Count(AID :0x5454)�� '88' �� ����� LS ��Ϲ�ȣ Null�� ���� �� ����

//-------------------------------------------
V20.06.01 (M_19.01.05) / PRU / 2020.06.10 :
//-------------------------------------------
	1. LS ������� ��� ����
	2. ping test ��� �߰�.

//-------------------------------------------
V20.04.20 (M_19.01.05) / PRU / 2020.05.27 :
//-------------------------------------------
	0. debug ǥ�� ����.
		(1) " IS_BW Not Valid : all(0x%X) 100(%d) A(%d)" : �ʹ����� ǥ�õǾ, ���� 10ȸ�� �ѹ� ǥ���ϵ��� ����.
		(2) DL payload onoff ���� ���� ǥ�� : string ǥ�� ���� : 
	    "DL_Payload_Off[0][0]" -> "[0][0]"

	1. clock fail ���� : AAU �����ϸ鼭 ���� ����. �׽�Ʈ �ʿ�.
	[����] clock fail �������� ���� ����.(optic Ż�� -> ���� ��)
	[����] clock ����ó�� �κ� ����. register 3s ����Ǹ�, cpri lock�� �����ϰ� clock ����ǵ��� ����.

	[����]
	(1) 3s ���� �˶�(register ��)�� ���,
	cpri(mmap crpi ����) ���� ���� �����̸�, ���� �˶�����(m_St.AlmSt.A_4.Clock)�� �˶����� ����.
	�׷��� ������, ���� �˶�����(m_St.AlmSt.A_4.Clock)�� ������ ����
	(2) 3s ���� ����(register ��)�� ���, ���� �˶�����(m_St.AlmSt.A_4.Clock)�� �������� ����.
	


//-------------------------------------------
V20.04.19 (M_19.01.05) / PRU / 2020.04.17 : 
//-------------------------------------------
	1. SFP ���� ��赥���� ���� data ����.
		[cur] SFP ���� ��赥���� ���� data ������, GUI Status Response ������ �ٸ�.
		[chg] GUI Status Response ������� �����ϵ��� ����.(AID 0x5305/08/10/11/14)
	2. �˶� �̷� ���� �� ��� ����ȭ �� ���� ��� ����.
		"__Seperate_AlarmLog__From__AutoReport__"	
	3. Package download (Root_F/S, Kernel) ����.
		"__USE_KERNEL_ROOTFS_DN__CHECK__"


//-------------------------------------------
V20.04.18 (M_99.01.05) / PRU / 2020.04.14 : 
//-------------------------------------------
	1. Package download
		[Cur] Root_F/S, Kernel : downlaod �ȵ�.
		[Chg] downlaod �߰�. "__USE_KERNEL_ROOTFS_DN__CHECK__" for PRU
		
	2. �˶� �̷� ���� �� ��� ����ȭ �� ���� ���	
		[Cur] CPRI Alarm ���� �õ�
		[Chg] ��� ����ȭ �� ���� : 
		���� 'log & alarm ����'�� ���� buffer ���,
		'log��' �� 'alarm����'�� 2�� �����ؼ�, alarm �� ��� ����ȭ �� �����ϵ��� ����.
		"__Seperate_AlarmLog__From__AutoReport__"	


//-------------------------------------------
V20.04.16 (M_19.01.05) / PRU / 2020.04.13 : 
V20.04.17 (M_99.01.05) / PRU / 2020.04.13 : 
//-------------------------------------------
	1.	Battery Extension	
	[����] SISO�� ���� ��,  Layer #2, #3 #4 AMP 'ON'���� ����.	
	[����] Layer #2, #3 #4 AMP 'ON'�ǵ��� ����.
	[����] Batt Ext' ��� ���� ��, Layer #2, #3, #4 AMP 'ON'���� ����
	[����] Layer #2, #3, #4 AMP 'ON'�ǵ��� ����.
	
	"AMP_Alarm_St()", "PAU_FWD_AMP_St()" --> "m_St.Pwr_Amp_En[]"

//-------------------------------------------
V20.04.15 (M_19.01.05) / PRU / 2020.04.13 : 
//-------------------------------------------
	1. battery extension ���� ��.

//-------------------------------------------
V20.04.14 (M_19.01.05) / PRU / 2020.04.12 : 
//-------------------------------------------
	1. battery extension : debug �߰�.
	
//-------------------------------------------
V20.04.13 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. battery extension ����.

//-------------------------------------------
V20.04.12 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. ��� �̿�ȭ �ٿ�ε� ������ M19.01.05�� ���Ե��� �ʾ�, ����.
		(undefine '__USE_MOD_DUALITY__' // module 2��ȭ)
		
	2. AC Alarm '�߻�'����
	[����] Battery ���� ���¿��� AC ���� �� AC Alarm �߻��� ���� ����,  AC���� ��, AC Alarm '�߻�'���� �ö� ��.
	[����] AC Alarm ���� ����.

//-------------------------------------------
V20.04.11 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. battery extension
	���� : 	['AC fail ����']����, 'extension mode off��', Ȥ�� 'Layer ���带 MIMO -> SISO �����' AMP On �εǴ� �κ�.
	
	

//-------------------------------------------
V20.04.10 (M_19.01.05) / PRU / 2020.04.10 : 
//-------------------------------------------
	1. UDA Alarm clear :
		���� : Ru-init 6 �ܰ迡��, gREMS�� reset report ��, UDA Alarm(clock/cpri) clear/alarm report ��.
		���� : uda alarm ���� �ϵ��� ����.( gREMS report �� ���� �ʵ���.)

//-------------------------------------------
V20.04.09 (M_19.01.05) / PRU / 2020.04.09 : 
//-------------------------------------------				
	1. UDA alarm ���¿��� UDA ������ �ȵ�.
	(1) suppress �ȵ� ����(uda.reset mode)�̸�, uda reset �Ǳ� ����, UDA alarm�� clear �ߴµ�, �˶������� �ȵ�.
	(2) suppress �ȵ� ����(uda.reset mode)�̸�, mode�� Alarm-Only/No-Action �����ߴµ�, UDA alarm�� clear �ȵ�.
	mode ����Ǹ�, �˰��� �ʱ�ȭ �ʿ�.(UDA alarm �� clear �ʿ�)
	(3) suppress �ȵ� ����(uda.reset mode)�̸�, mode�� No-Action �����ߴµ�, UDA alarm�� ������ ��� ������.
	
		

//-------------------------------------------
V20.04.08 (M_19.01.05) / PRU / 2020.04.08 : 
//-------------------------------------------				
	1. UDA Reset Suppress ����(35m ��)��, 
		���� uda_clock Ȥ�� uda_pd_cpri alarm �����̸�, alarm clear �ϵ��� ����.

	2. Battery�� ��ǰ� �ִ� �������� ��� Reset �ߴ��� AFE �� Digital Block�� �ֿ� ��ɵ��� Off�Ǵ� �κ� : 
	Battery ���� ���¿���, Reset ��, AC �˶��� �߻��Ǿ� �ְ�, 
	PSU�� ����� �̷����� �� AC �˶��� ���� ó�� ������ �����ϴ� ������ ����
	���� : PSU�� ����� 5ȸ �̻� ������ �� ���ǿ��� ������ �� �ֵ��� ����

	3. ���� : GUI Serial port �� �����ؼ� REMS�� sftp download test �� ��,
		RCU segmentation fault ���鼭 ����� �Ǵ� ����.
		���� : 0x50 Command �� ���, ó���Լ� ���ڰ� null �Ǵ� ����� ó�� ����.
		(UP_Data_Analysis  �Լ����� ���ڷ� �޴� r_ip pointer)
	
	4. Battery Extension Mode ������ ��
		(1) "Layer � ��� MIMO"���� Battery Extension mode ���Խ�
			Layer 0 On, 3dB ���� Ȯ��, Layer 1,2,3 Off  -> ���� ����

			�̶� Layer � ��带 SISO�� �����ϸ� 
			Layer 1,2,3�� 3dB Att�� ����ǰ� PAU�� Off���� ���� => On�� �ǵ��� ���� �ʿ�

		(2) "Layer � ��� SISO"���� Battery Extension mode ���Խ�
			��ü 3dB ���� ����

			�̶� Layer ���带 MIMO�� �����ϸ� 
			��ü 3dB ����, Layer 0 On, Layer 1,2,3 Off  -> ���� ����(Att �����ִ� ���� ��Ǯ� ����)

			�̶� Battery Extension mode �� Off�ϸ� 3dB ATT�� 0dB�� ����ǳ� 
			Off�� PAU�� On���� ���� => On �ǵ��� ���� �ʿ�

	5. ��� �̿�ȭ ����
		F/W �ٿ�ε�� "���ϸ��� ��� ������ �ڵ�"�� 
		"�κ��丮�� ��� ������" ��ġ ���� Ȯ�� �� ����.


//-------------------------------------------
V20.04.07 (M_19.01.05) / PRU / 2020.04.07 : 
//-------------------------------------------				
	1. sftp command ���� : [sshpass -p %s] -> [sshpass -p '%s'] for special characters
	2. UDA �� ���� reset ���� : 
		"reboot" command ����, "halt" �� ���� �����ϵ��� ����.
		(����, 10s ��, "reboot" command ����.)


	
"sshpass -p %s sftp -P %s -o StrictHostKeyChecking=no -o ConnectTimeout=30 %s@%s:%s/%s /tmp/%s\n"
	-> ����
	"sshpass -p '%s' sftp -P %s -o StrictHostKeyChecking=no -o ConnectTimeout=30 %s@%s:%s/%s /tmp/%s\n"
	
	������ ������ ������, 
	console���� ���� ���� �Ҷ�, �׸���, password �� Ư�����ڷ� ���۵Ǵ� ���,
	-p '%s' ��, -p option ������ '' ���� ������, 
	password �� ù Ư�����ڰ� �����.


//-------------------------------------------
V20.04.06 (M_19.01.05) / PRU / 2020.04.07 : for gIRO
//-------------------------------------------				
	1. Fan �ڵ�����(0xB617) == ON �̰�, AID 0x5411(FAN_Mode_St) �� ����������� ��쿡��, 
	table rpm �� Max RPM(������)(0xB618) limit�� ���ѵǵ��� ����.
	(����, 2020.02.14 �����ߴ� ������ ������.)


//-------------------------------------------
V20.04.05 (M_19.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------				
	1. Version changed : Same with "V20.04.04 (M_99.01.05)"
	2. Ru init �� �ȵ� �����̰�, CPRI alarm ���¿��� 	
	�Ʒ� �޽��� �ݺ����� �ʵ��� ����.
		<RCU> [AMP_0] reason_SD_5117 : (0) -> (2)


	-------------------
	cpri alarm ������, �������� ������ �ʴ� ����('m_Env.gREMS_Noti_Ready = 0')�̹Ƿ�, 
	reason clear �� ���ǹ� �ϹǷ� 
	'Clr__Reason_SD_5115' �ϴ� �κ� ����.
	
	////path4 = (i-6)/5; // 0~3
	////if(j==4) {Clr__Reason_Dis_5117(path4); Clr__Reason_Dis_5117_Prev(path4);} // bit4
	////if(j==0) {Clr__Reason_SD_5115(path4); Clr__Reason_SD_5115_Prev(path4);} // bit0 

//-------------------------------------------
V20.04.04 (M_99.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------				
	1. Download Cancel ��, RCU Reboot ���� �ʵ��� ����.
	
//-------------------------------------------
V20.04.03 (M_99.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------				
	1. FTP �ٿ�ε�
		(1) FTP �ٿ�ε忡 ���� D/L ���� �� ����
		(2) D/L �߿� �ٽ� FTP �ٿ�ε� ��� ���Ž� NACK �����ϵ��� ����.
		(3) �ٿ�ε� ����ð�(from gREMS) �ݿ�(�ð� ���� �ٿ�ε�.)	

//-------------------------------------------
V20.04.02 (M_99.01.05) / PRU / 2020.04.02 : for gIRO
//-------------------------------------------	
	1. AMP Pakcage download : 
		'AMP ���/�̻��'�� �������, 'AMP_HW_Insert' ���θ� �����ϵ��� ����.
		
	2. TSSI ���� �˶�
	'100+a' ��� �߿� TSSI ���Ѿ˶� �߻� ���¿��� 
	'100 only'�� ����Ǹ�, �˶��� ������ ���� �ʴ� ��� ����.

//-------------------------------------------
V20.04.01 (M_99.01.05) / PRU / 2020.04.01 : for gIRO
//-------------------------------------------	
	0. for gIRO : DL_SD_Alarm field �߰� : gIRO �϶��� ����ǵ��� define �߰�.
		
	1. �ٿ�ε� ��Ŷ �����Ӽ� 500Byte�� ���� : 
		define "__USE_PSU_PAU_DN_SIZE_500__"
		
	2. DPD �˶� Reason ���� �ʿ�
		���� : DPD Error (33/-114/-110/255/256/258) �� ��� DPD Error Alarm(0x01)���� ����
		���� : DPD Error 33d�� ��� Feedback Alarm(0x00)���� ���� �ܴ̿� �״�� ����

	3. gIRO �ٿ�ε� �κ� ����(PRU ȣȯ�� �κ�), PRU�� ���(PSU�� ������ ��尡 gIRO�� �ƴ� ���)
		Compatibility : 0x5A : .PSU ���Ϸ� �ٿ�ε�(.PSU ������ ���� ���.GPS ����)
		Compatibility : 0xFF : .PSU ���Ϸ� �ٿ�ε�

	4. PAU DL Shutdown ������ ��, PAU 'ON/OFF' ���� ���� �ȵǴ� ���� ����
	
	5. AID 0x0100/0x0101(maker/supplier) ���� ���� �ʵ��� ����.

	6. 0xB2 Command : zmodem : �������� 2s �ð� ���� �߰�.


//-------------------------------------------
V20.03.10 (M_99.01.05) / PRU / 2020.03.31 : Not Release
//-------------------------------------------
	1. gIRO RCU FW(v20.22.13) ���� : 
	DL_SD_Alarm field �߰� : un-define ��.
	#undef __USE__gIRO_DL_Shutdown_Field__

//-------------------------------------------
V20.03.09 (M_99.01.05) / PRU / 2020.03.31
//-------------------------------------------
	1. ���� ���� : from "V20.03.08 (M_19.01.05) / PRU / 2020.03.30"


//-------------------------------------------
V20.03.08 (M_19.01.05) / PRU / 2020.03.30
//-------------------------------------------
	1. PSU Package �ٿ�ε� �ӵ� ���� : ���� Frame ������ ����(500byte -> 1000byte).
	(PSU �κ� �����Ǿ���, ����.)
	2. gIRO RCU FW(v20.22.13) ���� Merge�Ͽ� M(19.01.05)�� ����	
	DL_SD_Alarm field �߰� : gIRO �϶��� ����ǵ��� define �߰�.
	#define __USE__gIRO_DL_Shutdown_Field__

//-------------------------------------------
V20.03.07 (M_99.01.05) / PRU / 2020.03.26
//-------------------------------------------
1. CPRI Auto configuration 6/6�ܰ� ���Խ� LS �����縦 Ȯ���Ͽ� �Ʒ� ���� �߰�
(Nokia�� 3�ܰ迡�� Delay Setup�� �ٷ� ������ 6/6���� �Ϸ� �Ǵ� ������ ����.)
	(1) SamSung   : Register.Capture_DL_Time(0x10020)�� 0x2d000
	(2) ELG/Nokia : Register.Capture_DL_Time(0x10020)�� 0x12000


//-------------------------------------------
V20.03.06 (M_19.01.04) / PRU / 2020.03.26
//-------------------------------------------
1. CPRI Auto configuration 4/6�ܰ� ���Խ� LS �����縦 Ȯ���Ͽ� �Ʒ� ���� �߰�
	(1) SamSung   : Register.Capture_DL_Time(0x10020)�� 0x2d000
	(2) ELG/Nokia : Register.Capture_DL_Time(0x10020)�� 0x12000


//-------------------------------------------
V20.03.05 (M_19.01.04) / PRU / 2020.03.25
//-------------------------------------------
	1. PAU/PSU Package �ٿ�ε� �ӵ� ���� : ���� Frame ������ ����(500byter -> 1000byte).
	2. '���°�������' alarm : LogParset GUI �� ���� Command�� ���, alarm �߻����� �ʵ��� ����.
	3. 'Clock fail' �߻� ���� debug : ǥ�� ����.




//-------------------------------------------
V20.03.04 (M_19.01.04) / PRU / 2020.03.24
//-------------------------------------------
	1. Clock fail �߻� �ȵǴ� �� ����.
	(Clock fail 3s Check timer�� ��� clear�Ǵ� �κ� ����.)


//-------------------------------------------
V20.03.03 (M_99.01.05) / PRU / 2020.03.24
//-------------------------------------------
	"V20.03.02 (M_19.01.04) / PRU / 2020.03.24"	�� ���Ϲ���.

//-------------------------------------------
V20.03.02 (M_19.01.04) / PRU / 2020.03.24
//-------------------------------------------
1. register ����, "halt" ���� 10s �� "reboot" �����߰�.(�Ʒ� (1)~(4))
	(1) 0xEA Command (PSU Reset) / by Power control
	(2) AID 0x5620 / by user control
	(3) Download / by download
	(4) VSS restart / by VSS
	(5) UDA PD(CPRI)/Clock: by UDA

2. LS ��Ϲ�ȣ : 
	(1) �ڵ������ÿ��� EEP �������� �ʵ��� ����.
	(2) RCU Start ��, �ڵ����� ���� check �ϴ� ��� �߰�.
	AID�� ǥ�õǴ� LS ��Ϲ�ȣ�� �ڵ������Ȱ��� �����ְ�, �ڵ����� ���� �ʾ����� ������ ���� ������.

3. Log ���Ͽ� ����� ���� ��Ŷ�� Information Check sum�� ���� ���� 

4. Clock fail : debug ǥ�� : 2s ����.
	 "[Clock]-timer alm %d[ms]/clr %d[ms], last %d, tmp %d(reg 0x%X, 0x%X), Mask_at_Reboot %d)", 

5. �Ｚ���� �϶� LS ID ��� ��� ����
	Samsung DUH 20 L/S ID ���� ���� : "(port_id/8) + (slot_id*2-2)" -> "(port_id) + (slot_id*12-12)"
	Range : 0~35, 
	������ 0 ���� ���� ��쳪 35���� Ŭ ��� LS ID 99�� ǥ�� �� 
	"Invalid LS ID = [%d], slot ID =[%d], Port ID =[%d]"  Log 1ȸ ���


//-------------------------------------------
V20.03.01 (M_19.01.04) / PRU / 2020.03.18
//-------------------------------------------
1. �Ʒ� (2) ����
	(1) 0xEA Command (PSU Reset) / by Power control
	PRU : register ���� �� "halt" 
	AAU : "reboot"

	(2) AID 0x5620 / by user control
	PRU : register ���� �� "halt" --> register ���� ���� "halt" ��.
	AAU : "reboot"

	(3) Download / by download
	PRU : register ���� �� "halt" 
	AAU : "reboot"

	(4) VSS restart / by VSS
	PRU : register ���� �� "halt" 
	AAU : "reboot"

	(5) UDA PD(CPRI)/Clock: by UDA
	register ���� �� "halt" (10s ������ �ȵ� ��찡 �ִٸ� "reboot")
	"reboot"



//-------------------------------------------
V20.03.00 (M_19.01.04) / PRU / 2020.03.13
//-------------------------------------------
	1. ADC : ���� ADC Read ��, ���� �� backup �ϰ� �ִٰ�, 
	(����, reboot ���� ADC Read �ϴ� �κп���) I2C�� ���� open ���н� '0' ������ �����ϴ� ���,	
	���� ���� ���� return �ϵ��� ����.	

	2.	AID 0x5421~0x5424 : limit �߰�. 
	(DL ON Guard Time/DL OFF Guard Time/DL to UL Guard Time/UL to DL Guard Time)

	3. DTU Reset
	AID 0x5620 ���Ž�, system halt�� ó�� 
	(S/W Reboot���� ó���� �켱 ����), Reset ������ ����� ����� ����

	4. 0xEA Command (PSU Reset) �� ��Ÿ..
		(1) 0xEA Command (PSU Reset) / by Power control
		(2) AID 0x5620 / by user control
		(3) Download / by download
		(4) VSS restart / by VSS
		(5) UDA PD(CPRI)/Clock: by UDA
		��� register ���� �� "halt" �� ó����. 
		--------------------------------------------------------------
		��, 
		[1] ���� : 	register(PSU Reset, register 0x33) �� 0x02 data ����. 0x02 ����Ǹ� "halt" ����.
		[2] ���� : 	register(PSU Reset, register 0x33) �� 0x02 data ����. 
			1�� ��, 0x02 ���� �� ���� ���� ������� �ٷ� "halt" ����.
			
	5. ���� � �� clock fail �̹߻�
	�� ���� ����  : CPRI Core Lock ���� �� �̹߻��̰�, CPRI�� unlock �϶� �߻��Ѵ�.
	-> �����ʿ� : CPRI ������¿��� Clock fail �� �߻��ϸ� �˶� ó��, CPRI�� �˶� �߻��ص� Clock fail�� ����
	

//-------------------------------------------
V20.02.14 (M_19.01.04) / PRU / 2020.03.02
//-------------------------------------------
	1. "AID 0x5620 Reset from gREMS"�� ���, gREMS ������ ���� ����(1.5s) �߰�.
	2. PAU : DL_SD_Alarm field �߰� : gIRO �϶��� ����ǵ��� define �߰�.
	#define __USE__gIRO_DL_Shutdown_Field__
	(V20.22.13, V20.02.13 �� DL_SD_Alarm field�� �����)

//-------------------------------------------
V20.22.13 (M_19.01.04) / PRU / 2020.03.02
//-------------------------------------------
	0. same with "V20.02.13 (M_19.01.04) / PRU / 2020.02.27"
	1. BIP ��� : bip 4����, ù��° ������ ������ 3���� ���� �ʱ�ȭ �ȵǾ� ����.
		�ʱ�ȭ ���� ����.
	2. PAU : DL_SD_Alarm field �߰� : 
		PAU��, VSWR algorithm ���� ��, AMP On �Ͽ� ���󿩺� check ��, 
		RCU���� SD.Alarm ���� ���� �ʵ���, field �߰�.

//-------------------------------------------
V20.02.13 (M_19.01.04) / PRU / 2020.02.27
//-------------------------------------------
	1. BIP ��� : bip 4����, ù��° ������ ������ 3���� ���� �ʱ�ȭ �ȵǾ� ����.
		�ʱ�ȭ ���� ����.
	2. PAU : DL_SD_Alarm field �߰� : 
		PAU��, VSWR algorithm ���� ��, AMP �Ͽ� On ���󿩺� check ��, 
		RCU���� SD.Alarm ���� ���� �ʵ���, field �߰�.

//-------------------------------------------
V20.02.12 (M_19.01.04) / PRU / 2020.02.24
//-------------------------------------------
	1. Command 0x5A : NACK ���� �� 
	  (1) �Ʒ� ���� clear (tot_crc, downReceivedBlock, header)
	  (2) "/tmp/RCU_DN.tar" �� ���쵵�� ����.
	2. Command 0x5B �������� ��쿡��,
	  (1) �Ʒ� ���� clear (tot_crc, downReceivedBlock, header)
	  (2) "/tmp/RCU_DN.tar" �� ���쵵�� ����.
	  (3) "/tmp/DN_file.zip" �� ���쵵�� ����.


//-------------------------------------------
V20.02.11 (M_19.01.04) / PRU / 2020.02.20
//-------------------------------------------
	1. ID Mismatch ���� : debug �߰� �� Test alarm(mismatch) ����.

//-------------------------------------------
V20.02.09 (M_99.01.03) / PRU / 2020.02.19
//-------------------------------------------
	1. TEST Time ����	
		(1) UDA TEST Time
		(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
		(3) Battery Alarm (������)
		(4) payload Test Time
////////////#define _SHORTEN_TIME____PAYLOAD_  // payload TEST
////////////#define _SHORTEN_TIME____AUTO_RST_ // UDA RESET TIME : TEST Mode
////////////#define _WATCHDOG_TEST_TIME_5s_ // 2020.02.17 : PRU : S/W Watchdog Timer 10s -> 5s Test �ð� ����
////////////#define _SHORTEN_TIME____BATT_MON_


//-------------------------------------------
V20.02.08 (M_99.01.03) / PRU / 2020.02.19 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
//-------------------------------------------
	1. Mismatch Alarm/���� : GUI�� ���, gREMS Report�� 10�� ����������,
		Test Alarm ���� �ϴ� ���, 10�п� 10�ʷ� ����.

	 

//-------------------------------------------
V20.02.07 (M_99.01.03) / PRU / 2020.02.19 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
//-------------------------------------------
	1. GUI Test Alarm���� �˶�/���� �߻� ��, Alarm report string(AID �ΰ�����)�� NULL�� �Ǵ� ���� ����.
		(1) ���� AID 
			AID 0x5013 : Temp_Up
			AID 0x5017 : DC
			AID 0x5302 : LOS_SFP		
			AID 0x5304 : LD_SFP_	
			AID 0x5101 : DL.TSSI_Upper		
			AID 0x5102 : DL.TSSI_Lower
			AID 0x5104 : DL.Out_Upper
			AID 0x5105 : DL.Out_Lower
			AID 0x5210 : UL.Over_Input
		(2) AID 0x5215 : Shutdown : reason�� Test alarm���� ������ �� ���, default�� "Rsn_SD__" �� ǥ�õ�.
		
	2. Clock_Status (AID 0xB41A) <--> register�� ���� �� mapping ����.
	 reg [2] : JESD Lock,   Lock ='1' : bit5 : ADC/DAC JESD
	 reg [1] : PLL/AFE Init Done ='1' : bit4 : ADC/DAC
	 reg [0] : PLL Lock,    Lock ='1' : bit6 : DCM(Main)
	3. PRU�� ��� ���� �˶� ���� ���� 10�� -> 60�ʷ� ����


//-------------------------------------------
V20.02.06 (M_99.01.03) / PRU / 2020.02.18 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
//-------------------------------------------
	1 UDA TEST Time (Suppress_Off �ð� ����)
		Suppress_Off:  35m(�԰�) ->  5m(Test Time)


//-------------------------------------------
V20.02.05 (M_99.01.03) / PRU / 2020.02.18 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
//-------------------------------------------
	1. giRO ���� PSU MakerMode : "giRO"�� �ҹ��ڷ� ���� ���ϵ��� ����.


//-------------------------------------------
V20.02.04 (M_99.01.03) / PRU / 2020.02.17 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
//-------------------------------------------
	1. UDA TEST Time (Suppress_Off �ð� ����)
		Suppress_Off:  35m(�԰�) ->  2m(Test Time)
		SUPPRESS_ON : 120m(�԰�) -> 15m(Test Time)

//-------------------------------------------
V20.02.03 (M_99.01.03) / PRU / 2020.02.17 : Test Time version
	TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
//-------------------------------------------
	1. L/S : alarm report : 'Alm_Subcode' 4byte little/big endian : 2020.01.14 �������� ����.
	2. giRO ���� PSU Compatibility : PSU protocol ���� �߰� �� ����.	
	3. Super CAP ���� ���¿��� AC ���� ��, FPGA ���OFF ��, Ping 1�� ���� : ����ó�� 3~5�� �ǵ��� ����.

	


//-------------------------------------------
V20.02.02 (M_99.01.03) / PRU / 2020.02.17 : Test Time version
//-------------------------------------------
	1, RCU App�� ����ǰ�, ���� AC �˶� �߻� �ÿ��� Reason�� ���� �� ����.
	2. SFP LD/PD �˶� : 'clock alarm' or 'cpri alarm' �̸�, �˶� clear �ϴ� ���ǿ��� 'cpri alarm' ������ ����.
	3. ����Batt Low �˶� : AC fail �� ������� ����Batt Low �˶� �߻��ϵ��� ����.
	4. TEST Time
	(1) UDA TEST Time
	(2) S/W Watchdog Timer 10s -> 5s Test �ð� ����
	(3) Battery Alarm (������)
		- Test �ð� ����: ("_SHORTEN_TIME____BATT_MON_")
		- 24hour -> 2m, 2h -> 1m
	
	

//-------------------------------------------
V20.02.01 (M_99.01.03) / PRU / 2020.02.14, 
//-------------------------------------------			
	1. UDA TEST Time ������� ����.


//-------------------------------------------
V20.02.00 (M_99.01.03) / PRU / 2020.02.14, <UDA TEST Time Version>
//-------------------------------------------			
	1. SFP_LD_Pwr(0x5305) : SFP insert �ȵǸ� 0.0 ǥ���ϵ��� ����.
	2. SFP_PD_Pwr(0x5308) : SFP insert �ȵǸ� 0.0 ǥ���ϵ��� ����.
		
	3. giRO ���� PSU AID �߰� : 
		PSU Maker Mode(0xC080), PSU Compatibility(0xC081)
		
	4. package download : PSU ����
		(1) "Compatibility"�� 0x5A �̰ų�,"Maker Mode"�� "giRO" �̸� "*.GPS" file ���.
		(2) �׿� : ������� "*.PSU" file ���.		

	5. FAN : Auto mode(0xB613) ON �� ���, AID 0x5411(FAN_Mode_St)�� �������, 
		Max RPM ������ �����ϵ��� ����.

	6. UDA Test F/W : #define _SHORTEN_TIME____AUTO_RST_
		(1).CPRI,CLOCK UDA �߻�����
			A.30�� ���� -> 2�� ����
			B.30�� �̳� 20ȸ �߻� -> 30���̳�or2���̳� 3ȸ �߻�
		(2).CPRI,CLOCK UDA ��������
			A.���� ���� 1�� ����

	7. UDA Test F/W : #define _SHORTEN_TIME____PAYLOAD_		
		(1).�ҿ��� UDA
			A.�߻����� 10�� -> 2�� : 
			B.�������� 1�� (���� ����)	

//-------------------------------------------
V20.99.32 (M_19.01.03) / PRU / 2020.02.13, TEST Version
//-------------------------------------------
	1. log ��� ��� �߰�.
	(1) Command 0xB0 ~ 0xB5 �߰�.
	(2) 5m log file : tgz �����ϵ��� ���� �� �������� ����.
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
	1. Layer �̻�� ���� �� �� ���� ��, Disabled �˶��� �߻� -> ����ŷ -> �߻� -> ����ŷ �ݺ� �� ����
	(����� �ÿ� �̻�� Layer�� Disabled ���°� (0x5117)��	��ON�� ���·� �ö���� �ʵ��� ����)
	2. PAU OFF ���¿��� Layer ���/�̻�� ���� ��,	Disabled(User) �˶��� �߻�/������ �ѹ��� �� �ö󰡴� 
	����� ����
	3. L/S alarm report : string ("User controlSyst"�� ǥ�õǴ� �κ�)����.
	4. Layer ��� ����(0x5002) ǥ�� ��� ����
	 (����) 0x0 : �̻�� 0x1 : ���
	 (����) 0x0/2/3 : �̻�� 0x1 : ���


//-------------------------------------------
V20.01.11 (M_19.01.00) / PRU / 2020.02.03
//-------------------------------------------
	1. Layer Splitter ��� ��ȣ(0x500E) �߰�
	2. LS Gain Balance(0xc076) : '�ý��� Reset�� �׻� ON' �ǵ��� ����.
	3. layer �̻�� ���� ��, PAU/LNA Off���� ����.

	--  ������ ���� �ݿ��� AID ���� �ۺ� �帮���� F/W �� GUI ���� ��û �帳�ϴ�~
	PRU �� AAU�� ���� ������ ���� ���Ǹ� ���� ���̿��� ���� ���� ������ ������ ���� PRU/AAU ��� 19.01.00

//-------------------------------------------
V20.01.10 (M_19.01.02) / PRU / 2020.01.29
//-------------------------------------------	
	1. VSWR Alarm -> Normal �Ǵ� ������ VSWR update
	2. '������� �˶�' ��, "TSSI ���Ѿ˶�" �߻��� ���, '������� �˶�' mask ���� �ʵ��� ����.	
	3. 'AMP_Not_Use' -> 'AMP_Use' ��, ������� �˶� �ٷ� ���� �ʵ���.
	4. Battery Extension ���� : Disable �˶� �߻� �� ���� �ȵǵ��� ����.


//-------------------------------------------
V20.01.08 (M_19.01.02) / PRU / 2020.01.29
//-------------------------------------------
	1. VSWR Alarm -> Normal �Ǵ� ������ VSWR update
	2. AC Fail : Alarm Report 2�� ���� �ʵ��� ����.
	3. Layer �̻�� ���� ���� : DL payload on/off ��� �ٲ� �� ���� �Ʒ�ó�� ����� ����� ǥ�� �߰�.
	4. 'AMP_Not_Use' -> 'AMP_Use' ��, ������� �˶� �ٷ� ���� �ʵ���.
	5. Battery Extension ���� : Disable �˶� �߻� �� ���� �ȵǵ��� ����.


//-------------------------------------------
V20.01.07 (M_19.01.02) / PRU / 2020.01.29
//-------------------------------------------
	1. Battery Extension ���� : Disable �˶� �߻� �� ���� �ȵǵ��� ����.
	2. ��� ���� �˶� TSSI ���� : (-50)dBm -> (-46)dBm ����.
	3. Layer �̻��(AMP �̻��) ��, 3�ʸ��� amp off, lna off ���� �������� ����.
	4. AC Fail ��, AC ��(AID 0xB603) ǥ�� ����.



//-------------------------------------------
V20.01.06 (M_19.01.02) / PRU / 2020.01.28
//-------------------------------------------
	1. UDA �ҿ��� Payload OFF��, ��� ���� �˶� masking �߰�.



//-------------------------------------------
V20.01.05 (M_19.01.02) / PRU / 2020.01.28
//-------------------------------------------
	1. DUH-ID(AID 0x5009) �߰�.
	2. UDA �ҿ��ķ� ���� Payload off ��, DL/UL AMP ����.
	[����] Payload Off, DL AMP Off ������ UL AMP ���� X.
	[����] Payload �� Off (DL/UL AMP ���� X)
	3. ��� ���� �˶� �߻�/���� �����ð� : 3�� 	
	4. AMP ON�� ���, VSWR ���� Update�ϰ�, VSWR alarm ��, 
	������ update �� VSWR���� Alarm Reason�� ����ϵ��� ����.
	5. TSSI log ��� ��.(__USE_TSSI_LOG__)


//-------------------------------------------
V20.01.04 (M_19.01.02) / PRU / 2020.01.23
//-------------------------------------------
	1. F/W ���� �ٿ�ε�� ���� �� ���� ��	
		(1) AID(0x5104) ��»��� : ���� ��û ��(48dBm)
	2. "UDA ���� L/S ������ ��, String ����"
		<����>
		SPURIOUS-Clear/SPURIOUS-AlarmOnly/SPURIOUS-PayloadOff
		CPRI-Clear/CPRI-AlarmOnly/CPRI-Reset
		CLOCK-Clear/CLOCK-AlarmOnly/CLOCK-Reset




//-------------------------------------------
V20.01.03 (M_19.01.02) / PRU / 2020.01.21
//-------------------------------------------
	1. F/W ���� �ٿ�ε�� ���� �� ���� ��	
	(1) UL Gain �ڵ� backoff (AID 0x5450) : OFF
	(2) LS Gain Balance (AID 0xC076) : ON
	- ���� : 'LS Gain Balance'�� �׻� �����ϰ� �־���.
	- AAUó�� on/off ����/��� �߰���.
	- AAUó�� off �� ��쿡��, LSH_Gain_Set (AID 0xC062~64/65~67) �� �� ��� 0�� ���� ����ϵ��� ����..
	


//-------------------------------------------
V20.01.02 (M_19.01.02) / PRU / 2020.01.20
//-------------------------------------------
	0. UL ���Է� S/D �˰��� ���� �ǵ��� ����.
		(30�� S/D�������� S/D�˶�(Reason : UL���Է�) ���� Ȯ��) 
	1. L/S : alarm report : 'Alm_Subcode' 4byte little/big endian ����.
	2. UL Gain �ڵ� Backoff	(AID 0x5450) : AAU �����ϸ鼭 ���� ����.
	(1) ����� ���� ����.
		����) cascade ID == 0 �� ��쿡�� ����.	
		���´� cpri lock �Ǿ routine�� �����ؾ߸� update ��.
		����) cascade ID == 0 �� �����ϰ� ���尡���ϵ��� ����.
		�����, ���µ� �ٷ� �ٲ�� ����.(cpri lock ���ο� �������)
	(2) reset �� GUI �ʱ� ���°� ����.
		����) default ON
		����) EEP ����Ǿ� �ִ� ���� �����ֵ��� ����.
		(cascade ID �� 0�� �ƴ� ���,  
		���� ���� ������ �Ŀ���, CPRI unlock �� �Ǿ �ٷ� ���� ������񿡼� ���� 
		'UL Gain �ڵ� Backoff' �� �����ϴ� ���� ������ ���� ����, 
		�ٸ�, reset �� GUI �ʱ� ���°��� EEP ����Ǿ� �ִ� ���� �Ǵ� �͸� ����.)
		
	3. "2020.01.20 : v20.99.02" Test version�� 'UL ���Է� S/D �˰��� ����' Ȯ�� �� �����(_PAU_1st_TEST_Debug_) ����.
	4. F/W ���� �ٿ�ε�� ���� �� ���� ��	
		(1) DL TSSI ����/100MHz	Layer#0 ~ #3 (AID 0x5102)
		(2) DL TSSI ����/+aMHz	Layer#0 ~ #3 (AID 0x5102)
		(3) DL ��� ����	Layer#0 ~ #3 (AID 0x5105)
		(4) UL Gain backoff	Layer#0 ~ #3 (AID 0x5208)		
		(5) LS Gain Balance	 (AID 0xC076)
		(6) DL LSH Gain(N)(100+a) (AID 0xC063)
		(7) DL LSH Gain(E)(100+a) (AID 0xC064)


//-------------------------------------------
V20.01.01 (M_19.01.02) / PRU / 2020.01.14
//-------------------------------------------
	1. Version Release from "V19.12.26 (M_19.01.00) / PRU / 2020.01.14"
	2. TSSI Log ��� ���� ("__USE_TSSI_LOG__")


//-------------------------------------------
V19.12.26 (M_19.01.00) / PRU / 2020.01.14
//-------------------------------------------
	1. RU Init �߿���, AMP-Stdby, Mimo-Stdby�� ��ġ �ð��� out-of-time �ΰ� ó��, 
		exit �ǵ��� ����.(amp-off �� exit �ǵ��� ����)
	2. TSSI Log ���(TEST Version)


//-------------------------------------------
V19.12.25 (M_19.01.00) / PRU / 2020.01.14
//-------------------------------------------
	1. RuInit(6/6)�Ǳ� ��, LNA ON/OFF ���� toggle ���� �ʵ��� ����.
	2. TSSI Log ���(TEST Version)


//-------------------------------------------
V19.12.24 (M_19.01.00) / PRU / 2020.01.14
//-------------------------------------------
	1. RU Reset �� MIMO Standby Mode ���� ����(AID 0x5431) On ǥ�� �̻� 
	: off �ǵ��� ����.
	2. TSSI Log ��� ��� ����.
	3. RuInit(6/6)�Ǳ� ���� LNA�� ON �Ǿ� �ִ� �� ����.
	RU Init Status (6/6) �Ǳ� ����, AMP OFF & LNA OFF ���� ���� �ϵ��� ����.

//-------------------------------------------
V19.12.22 (M_19.01.00) / PRU / 2020.01.14
V19.12.23 (M_19.01.01) / PRU / 2020.01.14
//-------------------------------------------
	1. TSSI Log ��� �ǵ��� ����(define option ����.)




//-------------------------------------------
V19.12.20 (M_19.01.00) / PRU / 2020.01.13
V19.12.21 (M_19.01.01) / PRU / 2020.01.13
//-------------------------------------------

Debug �߰� �� �Ʒ� �߰�

1. TSSI ��ǥ�� -90dBm ó�� ���� 
: -58dBm ���� ' -70dBm ����	PRU/AAU �����̸� TSSI ������ -70dBm ������ ��� -90dBm���� ��ǥ�� ó��
2. TSSI ���� ������ : 
: -60 ' -70dBm	TSSI ���� ���� ���� f/w �ٿ�ε� �� -70 dBm���� default setting �ǵ��� ����




//-------------------------------------------
V19.12.18 (M_99.01.01) / PRU / 2020.01.10
V19.12.19 (M_19.01.01) / PRU / 2020.01.10
//-------------------------------------------
	1. RCU Restart ��, USER-AMP-OFF �̾�����, OFF Reason ����.
	2. MIMO_Standby 
	(1) AMP 1ea user off ��, mimo standby �ȵǴ� �κ� ����.
		"PAU_OFF_Req[AMPU_4_MAX]"
	(2) shutdown alarm ���� ���� �ʵ��� ���� : 
	(3) mimo standby backup alarm ����� amp off �� �������� ����ϵ��� ����.
	(4) TSSI ���� check : mimo/amp standby ���� ���ȿ��� check ���� �ʵ��� ����.



//-------------------------------------------
V19.12.17 (M_19.01.01) / PRU / 2020.01.09
//-------------------------------------------
	1. L/S alarm �����, ������ �������� report �ϴ� �κ� ����.

//-------------------------------------------
V19.12.16 (M_19.01.01) / PRU / 2020.01.09
//-------------------------------------------	
	1. RCU Restart ��, version ǥ�� ������ CPRI/Clock alarm masking �߰�.

//-------------------------------------------
V19.12.15 (M_19.01.01) / PRU / 2020.01.09
//-------------------------------------------	
1. Amp Off ���� ǥ�� ����
	0x5117 : Disabled Alarm
   	0x5115 : Shutdown Alarm

	disable alarm(by user) �� sd alarm�� �켱��.
	sd alarm �� disable alarm(by system)�� �켱��.
	disable alarm reason �ִ� ���¿��� overwrite �ȵ�.
	- Ru initial(AID 0x500D) �ȵǾ� �ְ�, factory off �� ���¿��� user ���� ����.
	- sd (ul over in algorithm ��) amp off �ϸ�, sd clear, disable alarm �߻�.
	(AMP SD(VSWR, HighTemp, OverPwr, UlOverPwr, DC)�� ���, user amp on ���� �ȵ�.

2. ���Ҹ� ���� : 2020.01.09
	SubData 1st byte�� �������� length �̾��µ�, �� �κ� ������. 
	SubData 1st byte�� ���Ҹ� 1st byte �� ��.



//-------------------------------------------
V19.12.14 (M_19.01.01) / PRU / 2020.01.08
//-------------------------------------------	
	1. debug log clear	
		<RCU> [AMP#4] AMP_CMD_DIS Ctrl ---
		<RCU> [AMP#4] sendPAU_EnDis : Dis ---
	2. AufoConfigFlag.Init_Status == 0 �̸�, gREMS ���� ���� �ʴ� �� �߰�.
	3. AID 0x5117 Disable alarm/reason �ȵǴ� �κ� ����.



//========================================================================================
//-------------------------------------------
V19.12.13 (M_19.01.01) / PRU / 2020.01.08
//-------------------------------------------	
(*Not Release ��������.)
	1. CPRI, Clock, LOS, LOF : alarm(3s), normal(3s)
		("__2019_1226__Use_Alarm_Timer_Structure__")
	2. VSS Table�� Z.223(DUH_ID �ʵ�) �߰� : VSS Table debug�� ����
		(d_DUH_ID[CPRI_PATH])
	3. Clock alarm : string ǥ�� �׻� �������θ� �Ǵ� �� ����.
	4. �˶� �˰��� ����.
	Clock, Cpri�˶� �߻� �� Ÿ �˶��� �˶� �����ð� �ʱ�ȭ : ������� Alarm, SFP_LOS Alarm, SFP_LD Alarm �� �����ð� �ʱ�ȭ.
	5. gREMS alarm 2ȸ �߻� : ����, test �ʿ� (gREMS alarm ���� ��, ��Ȥ 2�� �����ϴ� ���)
	6. Amp Off ���� ǥ�� ����
	   0x5117 : Disabled Alarm
	   0x5115 : Shutdown Alarm
	7. UL_Over_Input_Alarm(0x5210) : 
	  (1) GUI/gREMS/LS : �����ϰ� �˶��߻�/�����ǵ��� ����.
	  (2) alarm(PAU algorithm ���Խ�), normal(PAU algorithm ���� ���ͽ�)  
	8. SD_Alarm(0x5115) : 
	  (1) GUI : amp off ��, amp on �� �˶��߻�/����
	  (2) gREMS/LS : AMP Protocol : Rx0 Shutdown alarm ����.	
	9. BW Gain Comp.(AID 0xC077/78/79/7A) : signed value�� ���� �ǵ��� ����. 

//-------------------------------------------
2020.01.07 : Not Release
//-------------------------------------------
	1. BW Gain Comp.(AID 0xC077/78/79/7A) : signed value�� ���� �ǵ��� ����.

2019.12.26 : Not Release
//-------------------------------------------
	1. VSS Table�� Z.223(DUH_ID �ʵ�) �߰� : VSS Table debug�� ����
		- d_DUH_ID[CPRI_PATH];
	2. Clock alarm : string ǥ�� �׻� �������θ� �Ǵ� �� ����.

2019.12.24 : Not Release
//-------------------------------------------	
	1. gREMS Alarm Report : Shutdown(DL)(AID 0x5115) Alarm String �߰�.
	(Get__Str_SD_5115() �Լ� ����.)

2019.12.23 : not release
//-------------------------------------------	
1. Amp Off ���� ǥ�� ����
	--------------------------------------+----------
	0x5117 : Disabled Alarm
	--------------------------------------+----------
	User�� ���Ͽ� Off �� ���	          | User Control
	System Algorithm�� ���Ͽ� Off �� ��� |	System Control
	--------------------------------------+----------
	0x5115 : Shutdown Alarm
	--------------------------------------+----------
	TSSI ���� �ʰ��� ���� SD	          | Overinput
	Over Power�� ���� SD	              | Overpower
	CPRI Unlock���� ���� SD	              | CPRI
	Clock Unlock���� ���� SD              |	Clock
	UL ���Է¿� ���� SD	                  | UL Overinput
	UDA�� ���� SD	                      | UDA
	--------------------------------------+----------
//========================================================================================




//-------------------------------------------
V19.12.12 (M_19.01.01) / PRU / 2019.12.20
//-------------------------------------------	
	- Version Only Change from Test version ("V19.12.91")
	Test version ("V19.12.91")
	1. RU Init (by CPRI Unlock/Lock)�� �ٽ� ���� �Ǵ� ��� ������� �˶� �߻� �Ǵ� ��.
	[����]
	- RU init �ɶ�, Payload off ����(flag_RU_init, flag_DTU_SD[FWD/RVS])�� Ȱ��ȭ ��.	
	- RU init �Ϸ� ��, FWD, RVS flag ��� clear �Ǿ�� payload on ��.
	- RU init �Ϸ� ��, flag_RU_init �� clear �� ��, flag_DTU_SD[FWD/RVS] �� 
	�ٷ� clear ���� �ʾҴ� �κ�(Ư�� RVS)�� ����.
	

//-------------------------------------------
V19.99.90 (M_19.01.01) / PRU / 2019.12.20
//-------------------------------------------
	0. "V19.12.12 (M_19.01.01) / PRU / 2019.12.20" ���� ���� : not rlease
	1. DPDv9.0 api ������� Test Version : 
	"__USE_DPD_9v0_Calculation__" Test Version ���� �߰�.


	

//-------------------------------------------
V19.12.11 (M_19.01.01) / PRU / 2019.12.18
//-------------------------------------------
	1. DTU �µ����� On/Off ���� (���¸� ǥ��, AID 0xB434)
	2. gREMS alarm 2ȸ �߻� : ����, test �ʿ�.
	3. AMP ON : "system control" ǥ�� �κ� ����.
	4. CPRI Unlock->Lock�� �Ǿ� RU Init�� �ٽ� ���� �Ǵ� ��쿡
	AMP�� Turn ON �� ���, Payload�� �ʰ� 'ON'�Ǹ鼭 �߻�(������Ѿ˶�)�ϴ� ����
	AMP ON �ÿ� Payload�� �ʰ� ON �Ǵ� ���� 
	Payload off ������, RU Init �ȵ� ��쿡 ���� off flag�� ������ �ȵ� ������ �Ǵܵ�.
	���� ��� off flag�� clear�Ǿ�� �ϳ�, 
	� �ð� ���� �Ŀ� off flag�� clear ��.
	
	- �Ʒ� �׸�(undef ������, ���� ������� �ʵ��� ����.) 
		"__PRU_MMAP_ADDRESS_Changed__" 
		"__SKIP__MMAP__"
	- ����
		"__DPD_9v0_TEST__" ---> "__USE_DPD_9v0_Calculation__"



//-------------------------------------------
//V19.12.10 (M_19.01.01) / PRU / 2019.12.13
//-------------------------------------------
//	1. ���� ��û : ���� 19.01.00 / RCU 19.09.01	
//	2. �������� �������� ��� Disable (���� ���, __USE_Packet_Division_for_WaveSpectrum__) 
//	3. CPRI Unlock �� Auto configuration ��� ����		
//		- [V19.10.04 (M_19.01.01) / 2019.10.28]
//		- ���� ���, __Auto_Config_When_CPRI_UnLock_to_Lock__



-------------------------------------------
V19.12.10 (M_19.01.01) / PRU / 2019.12.13
-------------------------------------------
	1. gREMS �������� ���� 1kByte ���� ó�� �߰�.


-------------------------------------------
V84.10.05 (M_19.01.01) / PRU / 2019.12.11
-------------------------------------------
	1. low Alarm/Normal Algorithm ����.
	2. Cascade SFP_TX_DISABLE �ÿ�, cascade LD_SFP Alarm �� mask �ϴ� �� �߰�.	
	3. Disable alarm ��, gREMS String Reason�� CPRI, Clock, TSSI �߰�.


-------------------------------------------
V52.10.05 (M_19.01.01) / PRU / 2019.12.10
-------------------------------------------	
	#define __PRU_MMAP_ADDRESS_Changed__ 
	#define __DPD_9v0_TEST__ // 51.10.05 Test Version
	/// #define __SKIP__MMAP__
	
	1. MMAP Address Changed : 
	(0xB006_0000 ~ 0xB006_FFFF)�� (0xB007_0000 ~ 0xB007_FFFF)�� ����.



-------------------------------------------
V84.10.05 (M_19.01.01) / PRU / 2019.12.10
-------------------------------------------
	1. Shutdown Alarm gREMS ���� : ���� ���·� ���� 
		[1] PAU���� Shutdown Alarm ������� gREMS ���� �ϵ���.
		
-------------------------------------------
V83.10.05 (M_19.01.01) / PRU / 2019.12.09
-------------------------------------------
	1. CPRI_2 (100M Cascade)�� 'SFP(LOS)'�˶� �߻��� gREMS�� �������� �ʴ� ����.
	[��û] JIg-RU1-RU2�������� , RU1 ��� SFP���� �����̺� ����/Ż�� �ݺ�, RU1�� SFP(LOS) �˶� ���� ���� �ϴ� ���� ����.
 	L/S ���� ����� ����, �˶� ���� ����� gREMS�� L/S�� ��� ���� ���� ��.
 	[����] gREMS alarm string�� alarm/Normal �� 2���� ����ϵ��� ����.




-------------------------------------------
V51.10.05 (M_19.01.01) / PRU / 2019.12.06
-------------------------------------------
	#define __DPD_9v0_TEST__

	1. Test version from "V82.10.05 (M_19.01.01) / PRU / 2019.12.04"
	[1] mmap ó�� ���� ����.
	[2] ��� ���� ���� : "10*log10(256*x)" �� ����.
		- DPD Engine Input Power
		- DPD F/B Engine Input Power


-------------------------------------------
V82.10.05 (M_19.01.01) / PRU / 2019.12.04
-------------------------------------------
	1. SFP LOS/LD �˶� ����� PD/LD �Ŀ� ���� ������ ���� ����
	[����] SFP LOS/LD �˶� 2�� ������ ��, ���� �˶����� �Ǵ��ϰ�, 
	����� PD/LD���� �����ϵ��� ����.
	
	2. Layer �̻��� ��� Power Off/On �ϸ� �̻�� Layer�� DPD Reset
	[����] DPD Reset (Register 0 Set) �ϴ� �κ� ���, �̻�� Layer���� üũ��.

	3. ��� ���� �˶� : "TSSI ���Ѿ˶�" or "TX ��� >= (������Ѽ���+2)"
	[����] "TSSI ���Ѿ˶�" �϶�, �˶��̾�� �ϴµ�, �������� �߸� �Ǿ� �־��� �κ��� ����.

	4. PAU �� ������ AMP Off Reason��, VSWR, OverPwr�� ���,
		Shutdown �ð�(30m) check �ؼ�, ���� shutdown�� ����ؼ�, 
		SD Alarm(AID 0x5115)�� REMS Report �� �� �ֵ��� ����.
		(PAU 30��, 10�� off, 5�� on �� �¾ƾ� ��. �ð��� Ʋ������ �ȵ�.)
	5. Shutdown Alarm�� ���� �˴ٿ��� ��, REMS ���� �ǵ��� ����.
	6. VSS Monitor ����.		
	-----------+---------------------------+----------------------		
			   | TX Val					   |	RX Val 
	-----------+---------------------------+----------------------	
	CPRI 0		2 : CPRI Main ����		   |  0 : CPRI Main ����
	CPRI 0 +a	3 : CPRI Alpha ���� 	   |  1 : CPRI Alpha ����
	---------------------------------------+----------------------	
	CPRI 1		4 : CPRI Main Cascade ���� |  6 : CPRI Main Cascade ����
	CPRI 1 +a	5 : CPRI Alpha Cascade ����|  7 : CPRI Alpha Cascade ����
	-----------+---------------------------+----------------------	
	
	Reg_VSS_DL_Addr[CPRI_0] // register 0: CPRI Main ����
	Reg_VSS_DL_Addr[CPRI_1] // register 1: CPRI Alpha ����
	Reg_VSS_DL_Addr[CPRI_2] // register 4: CPRI Main Cascade ����
	Reg_VSS_DL_Addr[CPRI_3] // register 5: CPRI Alpha Cascade ����	
	
	Reg_VSS_UL_Addr[CPRI_0] // register 2: CPRI #Main ����
	Reg_VSS_UL_Addr[CPRI_1] // register 3: CPRI #Alpha ���� 
	Reg_VSS_UL_Addr[CPRI_2] // register 6: CPRI Main Cascade ����	
	Reg_VSS_UL_Addr[CPRI_3] // register 7: CPRI Alpha Cascade ����
	
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
	Noti IP1/2 �� Invalid_IP�� ���� ��, gREMS(192.224.0.1)�� ��Ŷ ���۰���.

-------------------------------------------
V80.10.05 (M_19.01.01) / PRU / 2019.11.30
-------------------------------------------
	1. (EverNode_27)	
	AID(0x5454) LogCount ���� ��, �˶� ī��Ʈ�� '0'�� ������, 
	�˶� �߻� �� �α�ī��Ʈ '0'�Ǳ� ���� ��ũ ī��Ʈ�� ���� ��.
	(1) ���� backup log ������ ������� �ʵ��� chksum file �� backup file�� �����ϵ��� ����.
	(2) mutex(alarm, ctrl) Added.

	2. (EverNode_29)
	�������� String : F/W �˰��� ���Ͽ� AMP�� 'ON/OFF'�� ����, 
	'User Control'��� 'System Control' ����ϵ��� ����.

	3.(EverNode_21) VSS Table Debug
	
	4.(EverNode_11) 
	Noti IP1/2 �� Invalid_IP�� ���� ��, gREMS(192.224.0.1)�� ��Ŷ �����ϵ��� ����.
	
-------------------------------------------
2019.11.29
-------------------------------------------
	1. Inventory �����, �ڵ����� LS ��Ϲ�ȣ ���� ��, ���ֹ߼��޸𸮿� ����ȵǴ� ���� ����.
		( Proc___Inventory_SN() ����, flag-write ���� �߰�)
		
	2. SFTP ���� Ÿ�Ӿƿ� : 8[s] -> 30[s] ����.
	 (Ericson L/S �� ���, SFTP ���� ���� ������ �߻�, timeout ���� ��, ���� �����ߴٴ� HFR ���뿡 ����.)

	3. Layer �̻��� ��� Power Off/On �ϸ� �̻�� Layer�� DPD Reset�� ��� ������ ���� ����.
		(EverNode_23, �̻�� Layer�� DPD Reset ���� �ʵ��� ����.)

	4. ��� ���� �˶� ���� ���ǿ� TSSI �� < -50dBm ������ �����ϰ� 
		�ϱ� 2���� �������θ� ����(EverNode_24)
		- "TSSI ���Ѿ˶�" or "TX ��� >= (������Ѽ���+2)"

-------------------------------------------
V99.10.05 (M_19.01.01) / PRU / 2019.11.28
-------------------------------------------
	11.18 ~ 11.27 ���� ������ �׸� 
	//------------------------------------------------------------------------
	2019.11.27	
		1. �˶� ����� Power�� �����׸��� �˶� ����� ���°� ������°� �־�
			�˶��� check �ϴ� ������ Power�� �����׸�(limit)�� ����ϵ��� ����.
			(�˶� ���� ���� ��, debug ��� �ϵ��� ����)
		2. alarm_log 2��ȭ �߰�.	

	2019.11.21 
		1. ���Ҹ� ����/ǥ�� ����.(2019.11.20 AAU ���� ��, �����ϰ� ����) 
		Get_Mem_Usage() �Լ�����, �Ʒ� �߰�.( /proc F/S �� ���� �� ������, ������ �߰�.)
			if(fp == NULL) return 0; 

	2019.11.19
		1. Noti_IP_1/2 : invalid check
			[0] eMMC �� ����� Noti_IP_1/2 �� invalid �ϸ�, default IP(192.224.0.1) ���.
			VSS Channel ���� Noti_IP_1/2 ��
			[1] 0.0.0.0 �̰ų� 255.255.255.255 �̸� invalid
			[2] Broadcast IP(255.0.0.0 or 255.255.0.0) �̸� invalid
			[3] �׿� valid		

	2019.11.18
		1. ��躸�� �ֱⰡ "�������" �� ����, 1ȸ/1�� gREMS �ð���û ���� �ʴ� �κ� ����.
		2. GUI ID/PASS ���� �� ����
			���� �ʵ忡�� �̹� ����� ���¶�� gREMS ���� ���� ���� �ٿ�ε� ����� ������� �ʽ��ϴ�.
			�Ͽ� GUI ID/PASS ���� ������ ���� �Ҵ��Ͽ� �����ϵ��� ���� ��û �帳�ϴ�.
		3. GUI ID/PASS, Local time ��û �ֱ� 3[s] -> 5[s]�� ����
		4. ��� ���� alarm : ��� ���� ���� �� ���Ϸ� '5��' ���� �� -> '10��'�� ����.
//------------------------------------------------------------------------	





//----------------- version history ---------------------
V98.10.05 (M_19.01.01) / PRU / 2019.11.11. (Test ����)
	1. Tsync ���� : 
	delay_dl_ns 
		+ T2A_offset 
		+ PAU_DL_Offset 
		+ Tsync_Offset(500ns fix)
		+ offset_ns(AID 0xB415)
		+ Delay_Adjust_ns(87[ns])
		
	2. TSSI(0x5100) Display Log(every 10s)����
		A. �ù����ҿ��� ������ ����͸� �� �̹Ƿ�, �ڵ�� �ּ�ó��.
	3. "LoopBack ~" ���� �ֱ������� ��µǴ� �α� Off (AAU,PRU)
	4. gREMS IP ����Ͽ� ����� ���� Valid(0.0.0.0 or 255.255.255.255)�� �ƴ� ���� 
		���� DST IP ������ �����Ͽ� ��� ����, 
		�� ���� �߻��� Mac fail error �α� ��� �ʿ�
	5. RCU Log �ٹ� - FPGA Delay �� ǥ���ϴ� �κ�, �α� ���� (AAU,PRU)


V97.10.05 (M_19.01.01) / PRU / 2019.11.11. (Test ����)
	1. "TSync(DL only) Register ���� �� "�� �Ʒ��� ���� ����
		delay_dl_ns 
			+ T2A_offset 
			+ PAU_DL_Offset 
			+ Tsync_Offset(500ns fix)
			+ offset_ns(AID 0xB415)
	2. delay_dl_ns = DL_Delay(from L/S) -( PAU_DL_Offset + T2A_offset)
		delay_dl_ns < 0 �� ���,
		PRU && Factory_Mode �̸�,
		delay_dl_ns = 0 ���� ó��.		

V96.10.05 (M_19.01.01) / PRU / 2019.11.11. (Test ����) : �߰� Test version.


V95.10.05 (M_19.01.01) / PRU / 2019.11.08, (Test ����)
	1. "TSync(DL only) Register ���� �� "�� �Ʒ��� ���� ����
		(����) 
		FGPA_Register_DL_Delay + PAU_DL_Offset + T2A_offset + Tsync_Offset(500ns fix)
		(����)
		DL_Delay(from L/S) + Tsync_Offset(500ns fix) + offset_ns(AID 0xB415)
	2. "AMP_OFF, DPD#0 Normal Set" �� �ٷ� ó��/ǥ�� ���� �ʰ�, ���� 5�� ���� ó��/ǥ�� �ϵ��� ����.
	

V94.10.05 (M_19.01.01) / PRU / 2019.11.07, (Test ����)
	1.	ARP ���� ���� : F/W app ���� network ������ ��, broadcast �� enable �ϴ� �κ��� ����.
	(#define __Disable_Broadcast__)

	2. "TSync(DL only) Register ���� �� "�� �Ʒ��� ���� ����
	(����) 
	FPGA_Delay_ns_DL(GUI AID 0xB429) 
		+ T2A_offset 
		+ PAU_DL_Offset 
		+ Dly_Adjust(87ns fix) 
		+ Tsync_Offset(500ns fix)
	
	(����)
	FGPA_Register_DL_Delay
		+ PAU_DL_Offset 
		+ T2A_offset 
		+ Tsync_Offset(500ns fix)

	3. AMP ON�� DPD Reset�� 2�ʿ��� 3�ʷ�(TEST��)
	"DPD_Rst_Chk_MAX_Cnt" : 20(2[s]) -> 30(3[s])
	

V93.10.05 (M_19.01.01) / PRU / 2019.11.05, (Test ����)
	1.	dpd off ��, DL Delay register ���� �� ��, 170[ns] �߰�.
	2.	"�ý��� Reset �� AMP�� OFF ���¿����� DPD Reset�� ��� ���� ���� ���" :  
	������ dpd reset �� RCU�� ���� �ϴ� ���, 
	"[DPD_DCL#1_Reset] Request(20)" �αװ� ������ �մϴ�.
	�ϴ���, AMP�� OFF �����̸鼭, Register ���� DPD Reset �����̸�,
	"AMP_OFF, DPD#1 Normal Set" �̶�� log�� �����鼭, 
	dpd reset register �� normal �� �����ϵ��� ����.
	


V92.10.05 (M_19.01.01) / PRU / 2019.10.31, (Test ����)
	1.	V91.10.05 (DPD_DCL_Reset ����, reset low 2�� ����)���� ����.
	2.	Delay ���� ����.(AID�� DL 0xB415, UL 0xB416)
	3.	��1,2 Ȯ�� ��, ���Ĺ������� �����ϰڽ��ϴ�.


V19.10.05 (M_19.01.01) / PRU / 2019.10.30
1. CMD2_CFG_nC_AUTO_RPT(0x4B), CMD2_CFG__C_AUTO_RPT(0x4A) Command : 
	���� ���� packet�� sub-data ũ�Ⱑ 100�� ������, packet �������� ����.


V19.10.04 (M_19.01.01) / PRU / 2019.10.28
	1.	Path �� AMP OFF->ON ���� ��, Path �� DPD DCL Reset(0xB411) ����.
		"Proc___DPD_DCL_Reset"
	2. CPRI unlock �� LS �� gREMS�� ���� �ߴ� ��û : gREMS�� �������� �ʵ��� ����.	
	3. Auto - configuration ���� �߰� : CPRI unlock �˶� -> CPRI Lock ����(�˶� ����) ��
		debug ǥ�� : "[Auto_Config] by CPRI Unlock --> Lock"
	4. V19.10.03 ���� ����� : gREMS IP�� 0.0.0.0 ������, �˶��α�(���� GUI�� ���� �˶��α�)�� ���̵��� ����.


V19.10.03 (M_19.01.01) / PRU / 2019.10.22
	1. gREMS IP�� 0.0.0.0 �̸�, �۽ŵ� �������� �˶��α�(���� GUI�� ���� �˶��α�)�� ���̵��� ����.
	2. VSWR(AID 0xB207) type ���� :  INT16S -> INT16U 
	3. RESET(AID 0x5620) ��, reset reason�� "user����"�� ����(������ ������ PSU Reset ó��)


V19.10.02 (M_19.01.01) / PRU / 2019.10.18
1. ID/Password �� data ������ �߻��ϴ� ���� ����.
	gREMS IP�� 0.0.0.0 �� ���, RU���� ������ �ڵ�����, �� ��Ÿ ��û(���� GUI_ID_PW(0xEC))
	�� RU �ڽſ��� �ǵ��ƿ��� �ǰ�, �̶�, ID/Password �� data ������ �߻�.
2. ���ŵ� packet�� data ���̰� ���� ������, ó������ ���� �ʵ��� ����.
	Command LocalTime (0x69) : ���̰� 6�� �ƴϸ�, ó�� ���� �ʵ��� ����.
	Command GUI_ID_PW (0xEC) : ���̰� 22�� �ƴϸ�, ó�� ���� �ʵ��� ����.
3. gREMS IP�� 0.0.0.0 �̸�, Packet �������� �ʵ��� ����.


V19.10.01 (M_19.01.01) / PRU / 2019.10.15
	1. TSSI (AID 0x5100) Debug display every 10[s].

V19.09.02 (M_19.01.01) / PRU / 2019.10.08
V19.09.03 (M_19.01.01) / PRU / 2019.10.08
V19.09.04 (M_19.01.01) / PRU / 2019.10.08
	1. 'Reg_VSS_DL_Loopbackmode' �� ���ؼ��� ��ȿ�� üũ ���� ����.
	(log �߰� : DL ���� UL���� �ٸ���, 10�ʸ��� ó�� ī��Ʈ �α� ǥ��.)
	

V19.09.01 (M_19.01.00) / PRU / 2019.09.30
	1. factory mode ��, AMP-ON �ȵǴ� �κ� ����.
	2. log count(AID 0x5454)�� ������ log file ������ �ƴ�, Alarm/Ctrl(0xF4 Command) log count�� mapping

V09.26.03 (M_19.01.00) / PRU / 2019.09.26
	1. comm debug size check : debug on �� �߿� 2Mbyte ������ 0���� clear.
	2. gREMS Alarm report : VSWR string ���� ":1" �߰�.
	
V09.26.02 (M_19.01.00) / PRU / 2019.09.26
	1. L/S ���� ó��, RU-ID �Ҵ� �� Noti-ACK �ޱ� �������� gREMS �� ������ Packet�� ó������ �ʵ��� ����.
	
V09.26.01 (M_19.01.00) / PRU / 2019.09.26
	1. "V09.26.01" download �ȵ� ����.
	2. Package download FW ���� �ð� ǥ�� : CFnC �׸��̶� down-count ǥ�������� ����.
	
V09.26.00 (M_19.01.00) / PRU / 2019.09.26
	1. L/S ���� ó��, RU-ID �Ҵ� �� Noti-ACK �ޱ� �������� gREMS �� ������ Packet�� ó������ �ʵ��� ����.
	2. delay T2A, TA3 ���� ����Ǹ�(10ns �̻�), FPGA delay �� ���� �Ͽ� refresh ����.
	
V09.25.01 (M_19.01.00) / PRU / 2019.09.25 : Not-Release
	1. L/S ���� ó��, RU-ID �Ҵ� �� Noti-ACK �ޱ� �������� gREMS �� ������ Packet�� ó������ �ʵ��� ����.

V09.25.00 (M_19.01.00) / PRU / 2019.09.25
	1. reboot ���� ���� package download ��, tmpfs �뷮�� �� ���� �κ� ���� : 'make_DN_file_renew' �Լ��� fclose()
	2. debug added when amp-multidownload.


V09.24.02 (M_19.01.00) / PRU / 2019.09.24
	1. "V08.24.00" ���� 
	"RU ID �Ҵ� ������, LS�� ������ 0x9B(RU ID ���Ҵ�, Noti-ACK) �� ����, ����/ó�� ���� �ʵ���" �κ��� 
	�����δ� packet ó���� �ǰ� �־, ó������ �ʵ��� ����.

V09.24.01 (M_19.01.00) / PRU / 2019.09.24
	1.	AutoConfiguration ���� ��, 
		A.������ �ִ� DL/UL Delay ���� ����. (excluding Delay�� ����)
		B.L/S �κ��� CPRI Request�� ��������, �������� Delay ���� �ޱ� �������� "0"���� ���� 
		(���� "0", ���� ������ �ִ� ���� ����)
		(��, gREMS�� ����Ǵ� ���� ������ ������ �ִ� ���� �״�� ���� ��)
	
	2.	Delay Setup�� �������� �ʾƼ� RU Init Status�� (5/6) �����ϰ� �ִ� ���,
		A.	10�� Timer ����, gREMS ���� ���� �ϴ� �������� "AMP On/Off ����"�� �����ϵ��� ����
		(��, ���� ������ �ִ� Delay�� ���� ������ ���)
		(RCU Restart �Ǹ�, ���� ������ �ִ� Delay�� ���� ������ �ƴϰ�, 
		L/S�� Reset�Ǹ�, ���� ������ �ִ� Delay�� ���� ���� �ϼ� �� ����.)

V09.24.00 (M_19.01.00) / PRU / 2019.09.24
	1. AutoConfig ���� ��, DL Delay �� UL Delay ���� 0x00 ���� Set
	2. text log size : 2MB -> 1MB


V09.20.02 (M_19.01.00) / PRU / 2019.09.20
	1.	Sftp �õ��ϱ� ���� ssh key�� ���쵵�� �߰�
	2.	Amp �ٿ�ε忡 ���� �����۾�
		2.1	link fail�� �� �ٿ�ε� �õ� X
		2.2	Amp�� ������� �ƴ϶�� comfirm �õ� X
		2.3	PSU���� ���� ���� ��.
	3.	Pkg �ٿ�ε� ������ ���� DN fail üũ �κ� �߰� �� DN fail�� ���� ���� �ʱ�ȭ �Լ� �߰�
	4.	Pkg �ٿ�ε� �õ��ϸ� �ٿ�ε忡 ���Ǿ��� ������ �ʱ�ȭ 
	5.  pkg download : AMP confirm 10ȸ : time delay 300ms �߰�


//----------------- version history ---------------------

V09.20.01 (M_19.09.19) / PRU / 2019.09.20
	1. TSSI ���� �� -50dBm ��� ��� ���� �˶� Check : ��5�� ���� �� �˶��� ���� �߰�.
	2. 'V09.20.00' ���� gREMS-ALM-ACK ������, alarm log ����ȵǴ� �� ����.
	3. TestPort_CIM (AID 0x543F) : flash memory�� user ��� ����ǰ� ����.

V09.20.00 (M_19.09.19) / PRU / 2019.09.20
	1. gREMS-ALM-ACK ó���� 2��° �Ǵ� �κ� ����.(�ַ� Emulator���� ������ ��.)
	2. RU-Initial-status(AID 0x500D)�� complete�� �ƴϸ�, 
		AMP ON ���� �ȵǰ� ����(User ��� ����.(AID 0x5126, 0x5226, 0xB237))

V09.19.10 (M_19.09.19) / PRU / 2019.09.19
	1. �ٿ�ε�(Reset ���� )���� ��, Download ���� ǥ�� �κ� 'D/L �Ϸ�(Reset ���) '�ǵ��� ����.
	2. AMP-Standby-Mode-����Path (AID 0x5437) : AID Data type ���� : 0x02 -> 0x09(pre-defined)
	3. Log_On/Off(AID 0x5453) �� ON �϶���, Alarm/Control log(Command 0xF4) �����ϵ��� ����.
	4. Package download �� reset reason ����.
	5. SFTP download �ð� ǥ�� �߰�.

V09.19.01 (M_19.09.05) / PRU / 2019.09.19
	1. AMP FW�ٿ�ε� �� Confirm�ϴ� �κп��� ��õ� Ƚ�� 10ȸ�� ����.(chk_amp_FW_confirm)
	2. Ericson/Nokia : default gateway �����ϴ� �κ� ����.


V09.19.00 (M_19.09.05) / PRU / 2019.09.19
	1. ���� ������ Ǯ�� �ִ� �����̰� ���� ��� ���·� ���� ���̶� 
	�̷� ���̽��� ��� D/L �Ϸ�(Reset ���) ���� ����ǵ��� ����.
	(DL_St_Rst_Wait -> DL_St_Complete, m_St.DL_State[DN_RCU][DL_ST])	

V09.18.02 (M_19.09.05) / PRU / 2019.09.18
	1. TSSI ����(Main/Alpha sum Power) �� -50dBm ��쿡�� ��� ���� �˶� Check

V09.18.01 (M_19.09.05) / PRU / 2019.09.18
	1. gREMS Comm Start ��, �̹� �߻��� Alarm �� Report ���� �ʵ��� "V08.29.01"�� �ٽ� ����.

V09.18.00 (M_19.09.05) / PRU / 2019.09.18
	1."V09.17.02 (M_19.09.05)" �� �Ʒ� ������ �� ���� �������� ����.
	--------------------------------------------------------
	'UDA-�ҿ��ĵ��۸��'(AID 0x5444)�� "Alarm-Only" �� ���� �߿�,
	'UDA �ҿ��� ���� Layer'(AID 0x544C)�� OFF �� ǥ�ð� ���� �ʵ��� ��.
	--------------------------------------------------------
	2. TSSI ����(Main/Alpha sum Power) �� -50dBm ��쿡�� ��� ���� �˶� Check
	�׷��� ������ alarm masking
		(1) 100MHz Only ���� �� 100MHz TSSI��  ����
		(2) 100+��MHz ���� �� 100+�� Total Power ����
	3. VSS-DL REC_Port_Num 10 ȸ count�� �ȵǰ� �ִ� �κ� ����.
	


V09.17.02 (M_19.09.05) / PRU / 2019.09.17
	1. DPD  �˶� ���� �� TX Engine input Power(AID B408, DPD_ALM_PWR_x10) ���� ���� : -23dBm ==> -20dBm
	2. ��� ���갪 (AID 0x5108, DL_OutPwr_Cal_x10) : ������ ��, 0 �� ǥ��.
	3. 'UDA-�ҿ��ĵ��۸��'(AID 0x5444)�� "Alarm-Only" �� ���� �߿�,
	'UDA �ҿ��� ���� Layer'(AID 0x544C)�� OFF �� ǥ�ð� ���� �ʵ��� ����.
	4. 100MHz Only ���� �� 'Alpha �Է� ����'('TSSI_Lower_Alpha')�� �߻� : masking �߰�.	
	
V09.17.01 (M_19.09.05) / PRU / 2019.09.17
	1. Statis_Rpt_Prd(AID 0x5601) �� 0 �̸�, ��躸�� ���ϵ��� �߰�(���� 30�� ����).
	2. RU-Init-Status(AID 0x500D) : 5/6 �ܰ� �ƴҶ� Delay ���� ������ ���� ����(-101,Use of function is limited briefly)���� ����.

V09.17.00 (M_19.09.05) / PRU / 2019.09.17
	1. L/S �� ���� �޴� Delay ���� ���� F/W ���ֹ߼��޸𸮿� Save ���� �ʵ��� ����.
	(RCU Start �Ǹ�, ���� 0���� �ʱ�ȭ�ϵ��� ����)
	2. gREMS : Measure(0x44), CFG_nC(0x49), CFG__C(0x48) ���信 0x0001 Alarm AID �߰�.

	

V09.16.01 (M_19.09.05) / PRU / 2019.09.16
	1. MIMO_Std_AMP_Proc ���� "m_St.Off_Reason____Prev" �� �������� �ʵ��� ����.

V09.16.00 (M_19.09.05) / PRU / 2019.09.16
	1. CPRI Status ��� ���¿��� Delay setup�� ���������� ����� ó�� �ϵ� 
		RU Init status�� ������� ���� �ǵ��� ����.
	2. VSS DL update ��, unlock �̸�, VSS 10ȸ check count�� 0���� clear �ϴ� �κ� �߰�.	
	3. ���Ҹ� (AID0xB011) : �������� AID : max 100���� �������� �ʰ�, ������ ũ��� update �ǵ��� ����.
	4. AMP Standby : AMP #1 ~ AMP #4 ��, AMP #2 ~ AMP #4 �� ���� �ȵǴ� �κ� ����.
	V08.31.00����, 'Proc_AMP_Standby' �˰���� AMP_onoff ���� sync�� ���߱� ����, 
	AMP_Stdby_Sync �߰��ߴµ�, 
	��� AMP 4�� path�� �ƴ�, ù��° AMP ����, ��������� ���� ����.
	��� path�� ����ǵ��� ����.


V09.11.01 (M_19.09.05) / PRU / 2019.09.11
	1. AC Fail -> normal ��, reboot ���� : 
	battery �̽����� ���¿����� reboot �ǵ��� ����.
	2. Batt.extension mode ���� ��, AC Fail�� GUI���� clear �Ǵ� �� ����.

V09.10.01 (M_19.09.05) / PRU / 2019.09.10
	1. gREMS-packet log(alarm, control)
	(1) logCount(AID 0x5454) �����, file�� �ʱ�ȭ �ϵ��� ��.
	(2) /usr/app/log/rems/alm_report.log  ����-->  /usr/app/log/rems/rems_alm_rpt.log
	(3) /usr/app/log/rems/ctl_report.log  ����-->  /usr/app/log/rems/rems_ctl_rpt.log
	2. CPRI Recovery : 
	(1) 100-optic-off ,alpha-optic-ON ���� 100�� �ٽ� on �� ��, 
	lock �� 100�� mac �� 00:00:00:00:00:00 �̸�, 10ȸ count �� clear �ϵ��� ����.
	3. watchdog refresh�� FPGA Task���� �߰�. 
	

V09.09.02 (M_19.09.05) / PRU / 2019.09.09
	1. "V09.09.01" �� 2�� CPRI ref ���� ����.
	2. CPRI unlock �� path�� VSS update/write ���� �ʵ��� ����.
	

V09.09.01 (M_19.09.05) / PRU / 2019.09.09
	1. 0xF4 log Command Added : gREMS-Alarm-packet-log, Control-packet-log
	2. CPRI_0(Unlock)/CPRI_1(lock) ---> CPRI_0(lock)/CPRI_1(lock) �� ���, 
	CPRI ref�� CPRI_1 -> CPRI_0 �� ���� ���� �ʵ���(��, CPRI_1 �����ϵ���) ����.	

V09.06.00 (M_19.09.05) / PRU / 2019.09.06
	1. alarm paket log ����.(�̿�)

V09.05.02 (M_08.22.00) / PRU / 2019.09.05
	1. gREMS : CF/C, CF/nC �ڵ������� ������ name ���� �κ� ����.(1���� ������ �� ����.)

V09.05.01 (M_08.22.00) / PRU / 2019.09.05
	1. AMP��, �� PSU �ٿ�ε�ÿ� retry �ϴ� �κ� �߰�
	2. SFTP �ٿ�ε� �ÿ� sprint �� snprintf�� ����

V09.05.00 (M_08.22.00) / PRU / 2019.09.05
	1. gREMS : PLD ���� ���� : Measure ���� ó���� ������ �����ϵ��� ����.

V09.04.00 (M_08.22.00) / PRU / 2019.09.04
	1. inventory
	(1) report request : pau/psu polling ���� �ٷ� report �ϰ� ����.
	(2) auto report : pau/psu inventory polling ����� report �ϰ� ����.
	2.  A_ID ����
	  - FAN Mode 0x5410 ǥ�� ��� ����:  0x07(boolean) ->  0x09(pre-defined)
	  - FAN Mode ���� ���� 0x5411 ǥ�� ��� ����:  0x07(boolean) ->  0x09(pre-defined)
	3. A_ID �߰�
	  - L/S ���� DL/UL Delay Verification 0x5408
	4. gREMS protocol ���� : 
		(1) ftp-download-port length 1byte -> 2byte ftp_get_pkg
		(2) DOWNLOAD �Ϸ�(Device -> EMS) : 0x52 -> 0x51
	5. L/S ��Ϲ�ȣ(AID 0xB007)
	(1) ���� ���� �� ���� : 'Serial_Num' -> 'LS_Regist_Num'
	(2) �ڵ� ��ȣ ���� ���� : 
	- NULL �̰ų�, KST default : �ڵ�����.
	- Inventory ����� �ڵ�����.
	- �׿� �ڵ� ���� ���� ����.
	6. PRU LED ���� : alarm ���� �߰�.
	


V09.03.00 (M_08.22.00) / PRU / 2019.09.03
	1. gREMS 
		(1) Alarm Report : Alarm info(String) �ѱ� ����. 20byte �������� �����ϵ��� ����.
		(2) alarm report ��, �ð� ���� ���.		
		(3) clock fail REMS ���� �ȵǴ� �� ����.
		(4) gREMS���� local time ���� �߸��ϴ� ���(���� Ʋ���ų� ��,��,��,��,�ʰ� ���� �����), 
		 �ð� ���� �ȵǰ� ��.
		(5) header ��躸�� �ֱ�(0x5601) length ���� ���� :  7 -> 8	
	2. UDA Clock/PD �� reset ��, reset-reason �� PowerCtrl �ΰ� ����.


V09.02.04 (M_08.22.00) / PRU / 2019.09.02
	1. UDA.Clock alarm : test alarm �ǵ��� ����.
	2. V09.02.02 ���� ��躸�� �ȵǴ� �� ����.
	3. ID Mismatch 10�� Ÿ�̸� ǥ�� : 10�� ���� ǥ�� ���� �ʵ��� ����.
	
V09.02.02 (M_08.22.00) / PRU / 2019.09.02
	1. gREMS ��� ���� : 0x5405 round_trip : layer x carrier = 1 x 2 �̳� ��迡�� 2 x 2 �� �Ǿ� �ִ� �� ����.
	2. gREMS �ڵ� ���� : ���信 �ð� ���� AID 0xFFFE�� �������� �ʵ��� ����.


V09.02.01 (M_08.22.00) / PRU / 2019.09.02
	1. SFP Ż��/���� �˶� (SFP_Ins_0_a, SFP_Ins_0_) : GUI Alarm Test�� ���� �ǵ��� ����.


V09.02.01 (M_08.22.00) / PRU / 2019.09.02
	1. gREMS ��� ���� 
	(1) header �� ADI 0x5601, 0x5602 ����.
	(2) Packet Inforamtion : cur_packet, tot_packet �׸� : 0���� ����Ǵ� �κ��� ����.


V09.02.00 (M_08.22.00) / PRU / 2019.09.02
	1. gREMS : 
	(1) _CF_CTRL_ �ڵ����� ��, CenterFreq(AID 0x500B) �׸� ������ ���� ����.
	(2) LS ������(AID 0x500A) : _CF_nCTL_ �ڵ����� �ǵ��� ����.
	(3) ��� ���� ��, 'bip-err-����', 'hard-clipping-����' �� clear.
	
	



V08.31.00 (M_08.22.00) / PRU / 2019.08.31
	1. AMP-Standby
	(1) 'Proc_AMP_Standby' �˰���� AMP_onoff ���� sync�� ���߱� ����, AMP_Stdby_Sync �߰�.
	(2) debug ǥ�� ����.
	(3) gREMS ���� ���� �ʵ��� ����. 
		- AMP Standby Mode ���� ��, �˶�����(��� ��/���� �˶�, disable �˶�)
		- AMP Standby Mode ���� -> ���� �� : gREMS ���� �Ǵ� ����.
	


V08.30.01 (M_08.22.00) / PRU / 2019.08.30
	1. BW Gain Comp. (AID 0xC077 ~ 0xC07A)����.
	- 0[dB] ���� �ȵǴ� �κ� ����. ���� �ݺ� �Ǵ� �� ����.
	2. tsync offset 500[ns] add.
	3. Delay ���� ���°� ������ ��� 0us�� ǥ��(0xB429/B42A) 
		(register return value ���� offset (0xB415/B416)���� Ŀ�� ������ �Ǵ� ���.)
	4. Ÿ����Է� : 	
	(1)	"{[(filter�� �Ŀ�) >= (-47[dBm)]}" ������ 
	    "[(filter�� �Ŀ�) > (-47[dBm)]" �� ����.
	(2)	"{[(filter�� �Ŀ�)  - (-47[dBm)] < [-0.5[dB] }" �� ���� �ȿ���, 
	    {Ÿ��+ �ڻ�} ~ {�ڻ�} ���̸� +/-0.5 ���� +/-0.4�� ����.
	(3) ���� �� �� ������ 0.5[dB] Step -> 0.1[dB] Step  ����.
	(4) ALC Level : (-47.0)[dBm] -> (-46.5)[dBm]
	5. PKG-�ٿ�ε�
	(A) ��û ���� : AAU Ȯ�ν� DL State�� F/W ���� ��, Reset ��� ���¿��� F/W ���� ������ "���� ���� ��"���� ���� ��. 
		"0x04: Reset �غ���" Ȥ�� "D/L �Ϸ�(Reset ���)" �� ���� �� �� ��.
	(B) ���� : PKG-�ٿ�ε�ÿ� Ÿ�̸� ��� ���°� �Ǹ� "DL_St_Rst_Wait" �� ���� ��ȯ.



V08.30.00 (M_08.22.00) / PRU / 2019.08.30
	1. Ÿ����Է� : {�ڻ�+Ÿ��} ~ {Ÿ��} : +/-1.0[dB] ������  +/-0.5[dB] �� ����.
	
2019.08.29---------------------
V08.29.02 (M_08.22.00)
	1. DPD Off ��, DL-Processing-delay�� +170[ns] �߰�
	2. BW Gain Comp.(140MHz ~ 200MHz) �߰�. (AID 0xC077 ~ 0xC07A)

2019.08.29---------------------
V08.29.01 (M_08.22.00)
	1. gREMS Comm Start ��, �ʱ� Alarm Report �ǵ��� ����.
	2. gREMS Reset Alarm Report : alarm string �߰�.
	"����� ����", "F/W Download", "�����̻�", "��Ÿ",
	"��������", "VSS", "UDA_PD(CPRI)", "UDA_Link", "UDA_Clock" 
	3. ID Mismatch REMS report �߰�
	4. AC Fail �϶� Log : 2��30�ʸ� �����ϵ��� ����.
	5. Inventory -> L/S ��Ϲ�ȣ : ����� format ���� ����.
	6. Inventory S/N : format �� ���� ���� ����� ���� �ȵǵ��� ����.
	



	

V08.28.01 (Main_08.22.00)
	1. Ÿ����Է� algorithm ���� : algorithm ���� ����.
	2. processing fpga delay offset 87[ns] Added (m_Env.Delay_Adjust_ns = 87;)
	3. t_Sync_Delay ���� �ÿ�, delay offset 87[ns] �߰�. (d_t_Sync_Delay)
		AID 0xC021 : PRU : Status Only(Control X)
	4. PAU status polling ��, PAU OFF�� Fwd_Out_Pwr_x10(AID 0x5103) 0[dBm] ���� ����.
	5. Height_Bal_Onoff : RCU Restart �Ǹ�, �⺻, ON ���� �ǵ��� ����.
	6. Test Pattern ON�� 10�� �� �ڵ� OFF�ǵ��� ��� �߰�.
	

V08.28.00 (Main_08.22.00)
	1. GUI FW Version (AID 0x5503) : v08.22.00 ����.
	2. GUI FW Version (AID 0xB015) : hidden version �߰�.(v08.28.00 ǥ��.)
	3. RU_Init_Status (AID 0x500D) : 
		CPRI_Status �� ���� ������ ���� RU_Init_Status ���°� CPRI_Status ���� ���� �ܰ� �϶� ��, ���ŵǵ��� ����.
		Dly_Set_Req �� ���� ������ ���� RU_Init_Status ���°� Dly_Set_Req ���� ���� �ܰ� �϶� ��, ���ŵǵ��� ����.


V08.27.04
	1. Ÿ����Է� �˰��� ����. Always ALC ON.
	

V08.27.03
	1. ����ֱ�(AID 0x5601, Statis_Rpt_Prd) 5��, 15�� �߰�. 
	2. UL RSSI ��� ����.
		RSSI_Filter��(0xB302)
		RSSI_Filter��(0xB303)
		RSSI_Noise(0xB306)

V08.27.02
	1. MAC-GUI-Control : fileSystem version(08.15.00)���� ����Ǹ鼭 
		Set_System_HW_Mac()���� ����ϴ� sh.script ����.	
	2. reset reason �߰�.(VSS, UDA-Clock/Cpri...)
	

V08.27.01
	1. ����GUI, KST GUI ���� �ϸ鼭 �ٿ�ε� �ȵǴ� �κ� ����.
	�Ʒ��� 2019.08.09�� ������ �Ǿ�����
	2019.08.10 ���� �߸� ������.
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_NODE, NULL, MAX_Pr--); // REMS-UDP, 2nd		
		->
		APP_Task_Creat( UDP_Sel_Proc_Task, COMM_UDP_REMS, NULL, MAX_Pr--); // REMS-UDP, 2nd		
	2. "Proc___PRU_Rvs_OtherCompany__Over_Input	" ����, AID 0xB302�� ���.
	3. Ÿ����Է� �˰��� ����.
	4. RSSI Noise ��ǥ�� ����.
	

V08.26.00
	1. Factory_mode �߰�. (AAU�� �����ϳ�, PAU_insert �� �ٸ�)
	2. register 0x80(Reg_DSP_Common_MIMO_RW_EQ_Common) : ����� 100�� ���н�, debug ǥ�� �߰�.
	3. ��ǥ�� ���� : RSSI(filter ��, AID 0xB302) : -92dBm �̸��� -100dBm ǥ��.

V08.24.00
	1. IP setting �ߺ��ϴ� �� ����.
	2. BIP error count : SFP Ż���, 0 ó�� ����.
	3. PAU : EQ ���� DTU ���� ����.
		(1) dpd_on  �̸�, register 0x80, register 0x04 ��� 0x00���� write
		(2) dpd_off �̸�,
			register 0x80(Reg_DSP_Common_MIMO_RW_EQ_Common)
			A. ���� PAU EQ ������ ��00�����(���� �� ����) : 0-path : 01, 1-Path :01, 2-Path: 02, 3-Path:01
			B. ���� PAU EQ ������ ��00���� �ƴ� ���	   : 0-path : 01, 1-Path :02, 2-Path: 02, 3-Path:02
	4. ��ǥ�� �Ϻ� ���� : RSSI ���� -80dBm�̸� ���ǿ��� -92dBm �̸� �������� ����
	5. RU ID �Ҵ� ������, LS�� ������ RU ID ���Ҵ�, Noti-ACK �� ����, ����/ó�� ���� �ʵ���(packet ����.)



V08.23.00 : Not Release
	1. AID �߰�.	
		Delay ���� ���� ��(0xC071), 
		Delay ���� ���� return ��(0xC075)
		Hidden Version(0xC072), 

V08.22.45 : 
	1. AID 0x500D RU Initial status ���� ����
	2. Tsync offset ���� ��û ���� register ����� �� : +/- 1 ���̳��� �������� �Ǵ�(�ݺ� ���� ���� �ʵ��� ��)
		

V08.22.00 : Not Release
	1. AID 0x500D Added : 
		0x06(Completed)�� �ƴϸ�, AMP/Payload OFF
	2. Inv.S/N ����Ǹ�, Serial Number(AID 0xB007)�� ���� ����.

V08.21.00 : Not Release
	1. Optic Delay(Round Trip Delay) 0x5405 �׸� ���� ���� 2 --> 1

V08.19.91 : made ON 2019.08.21
	1. �д� Test version : PAU Status ����, EQ Stauts �׸��� ��.


--------------------- FW_V08.14.99(FPAG download) added.

	
V08.19.90 : 
	1. Samsung L/S ID ���� ����.
	"(port_id/8) + (slot_id*2)"	-> "(port_id/8) + (slot_id*2-2)"



V08.19.00 : Not Release
	1. �ʱ� ���� �� amp off ������� �ʱ�ȭ �Ϸ� �� amp on �ϵ��� ����
	2. Samsung L/S ID ���� ���� : "(port_id/8) + (slot_id*2)"	-> "(port_id/8) + (slot_id*2-2)"	
	3. console���� �������� �����, ������ ǥ���ϴ� �ɼ� �߰�.

V08.17.00 : 
	1. CPRI unlock ǥ�� : console�� Instance ���°� ǥ�� �߰�.

V08.14.30 : 
	1. gREMS 

V08.13.00 : 
	1. delay ��� ���� ��� ����("__USE_ProcessDly_plus_Offset__")
	2. �������� offset : AID Added.
	4. "V08.10.00" ����, S/N ���� �ȵǴ� �κ� ����.

V08.12.00
	1. gREMS stat : "DL_TSSI_Decimal" is not stat : AID is 0xB100
	2. PAUID : 0x20 -> 0x20/21/22/23
	3. AC Fail -> normal : system "halt" �� PRU Restart�ǵ��� ����.
	4. AID 0xB21D : "AMP Forward Input Power" Added.
	(PAU protocol added)
	5. EQ Status : PAU, Register Added (control, status)
	6. EQ Common Register Added.
	7. 	FAN(����) �ڵ����� : 
		RPM�� '40% -> 0%'�� �Ǵ� ������ ���� ���´� 40�� �̸� 
		-> 30���� ����.(�԰��� 30�� �̸��̹Ƿ�)

V08.10.00 : Not Release
	1. log Added : 	
		i.	���� ���(0x01, 0x48, 0x4C), Download���(0x57,58,5a,5b) FTP �ٿ�ε�(0x50,51), ���� Reset(0xEA),
			Layer Splitter Delay ����, VSS Reset ���Ž� ����.
		ii.	L/S �������� �� RU ID �Ҵ� �Ϸ� �� Init Noti ���� ��Ŷ �� �ð����� ����
		iii. ���� �̷� ���� 1���� ��Ŷ ���� �� 200�� ����
	2. Task End : all task ���� Ȯ���ϴ� code �߰�. task �� max 1�� ��ٸ�.
		PRU : �����δ� 1�� ���� Ȥ�� 1.4�� �ҿ�. �����.

V08.09.01
	1. gREMS TX port : 50000 -> 20000

V08.09.00
	1. L/S : Delay ����� ����/������ ���̽� log ��� ��û
	2. L/S Saumsung �϶�, BASE_MAC = Syste_MAC �� ��� ����.
	3. Alpha �̻�� ��, L/S-ID �� default 0���� ǥ���ϵ��� ����.


V08.08.00
	1. L/S UTC Setting ��, gREMS �ð� ���� ��, RTC ����. 
	����, 10�� ����, RTC�� ���� system time ����ȭ.		
	2. vss������ ������  cpri mmap�� �е��� ����
	   (������ CPRI-lock ���¸� read �ؼ�, VSS read/write ó��.)
	3. VSS MAC/IP ���� check : "1�� x 10��" -> "100ms x 10ȸ" �� ����.
	4. VSS REC-Port-No : ���� 10ȸ stable check �߰�.(cascade-id �� �̹� �߰� �Ǿ� ����).

V08.07.02 : Not Release
	1. TEST RTC Added
	#define __USE_RTC__
	#define __TEST_RTC__

V08.07.01
	1. 'Alarm information Notification' �� 'C&M Path Switchover Notification' :
	RU ID �Ҵ� �Ǳ� ������ �������� �ʵ��� ����.

V08.07.00 : 
	1. PRU ADC : 1.2V, 0.85V ADC�� ����, ���кй� ���� �ʱ�� ��.('*2' �� ����.)
	2. TEST Alarm log ����(���� ������� ����)
	3. gREMS ��� : ��� ���� ��, ���� ��, HardClipCnt, BIP_ErrCnt �� �ʱ�ȭ ���� �ʵ��� ��.
	4. gREMS : "GUI-Password/ID" Request, "Local-Time" Request added
	5. PRU : RCU restart �� ��, PHY reset Added.

V08.06.00 : Not Release
	1. gREMS Stat : init bip-err-accumulted, hard-clipping-accumulated
		Init__HardClipCnt_Accum(), Init__BIP_ErrCnt_Accum()
	2. AID 0xC06C : Height Balnance On/Off
	3. L/S Valid-indicator 0x01 �Ǳ� �������� gREMS Alarm Report ���� �ʵ��� ����.
	(L/S Valid-indicator 0x01 �� ��, normal �� alarm ���� report �Ǵ� �� ����.)
	

V08.02.01 : 
	1. L/S : "Initialization Information Acknowledge IE", "Lower node ID Configuration IE"
		[1]	message copy �Ҷ�, IE header �� 'IF Length' ��ŭ copy �ϵ��� ����.
			length size ���� : 4byte -> 2byte ����.
		[2] samsung �� ���, 'IF Length' �� Ÿ�� ���� ū�;� ���� max ó��.
	2. alarm log added ( alarm, control) : Test Mode (5�� -> 15��)	
	 	

V08.01.00 : 
	1. L/S : "Initialization Information Acknowledge IE", "Lower node ID Configuration IE"
		message copy �Ҷ�, IE header �� 'IF Length' ��ŭ copy �ϵ��� ����.
	2. FAN Mode Init : from PSU Task -> Alarm Task, AID 0x5410
	3. DTU_SD_By_AMP_Not_Use : AMP �̻������ ����, Payload off, AMP OFF Added.
	

V07.31.00 : 
	1. "V07.30.99" ���� : 
	    V07.29.04 ���� __USE_Spectrum_LOW_Limit__�� define�� ������ �־ 
		�� �κ��� �ƿ� ������ ���� ���� �ϴ� ������ �ٲ�. �� ���̴� �κ��� #if0���� ����.

V07.29.04 : 
	1. "V07.27.00" ����, �׽�Ʈ�� ���� PAU�� �׻� "���"���� �ߴ� �κ� ����.	
	2. AC �˶� �߻� ��, ���� alarm ó�� ���� �Ʒ� �˶� �߰�.
	   [1] SFP Ż���� �˶�(AID 0x5315), BATT LOW �˶�(AID 0x5018) ó�� ���� �߰�.
	   [2] ��� ���� �˶�(0x5105)���¿��� AC fail �϶�, �˶� ���� ���� �Ǵ� �� ����.	   
	3. PSU Ext.FAN : �ý��� �ʱ� ���ý� RPM 20%���� ���� 5�� ���� : Manual mode ����.
	4. PSU External FAN : Auto : max temp of DTU and AMP 
	5. LS REC �� NKA/ERS �� ���, Base-IP ù��° byte �� 192���� Ȯ���ϴ� ���� �߰�.
	6. LS ID ������ ���� ����(L/S �� Saumsung�� ��쿡��.)
	


V07.27.00 : 
	1. L/S : alarm ���� string ����. (AID ������ string ó�� ����)
	2. PSU Ext.FAN : �ý��� �ʱ� ���ý� RPM 20%���� ���� 5�� ���� 
	3. UL output Atten(AID 0x5206) : 19.9dB������ ����Ǵ� ���� 20dB ���� ����ǵ��� ����.
	4. DPD ��� Offset(AID 0xB410) ���� : DPD ���(AID 0xB407)�� ����.
	5. AC �˶� �߻� ��, ���� alarm ó�� 
		[1] ���� alarm : Clock Fail, TSSI ����/����, SFP LD,LOS �˶�, CPRI Fail �˶�, PAU �˶�(Disable ����)
		[2] ���� alarm ���°� ���� ���� �ʵ��� ����(alarm ���濡 ���� gREMS�� �������� ����.)
		[3] gREMS/GUI �������信�� alarm�� mask �Ͽ� ����.
	   
	6. ����ȣ�� ��ǥ�� ǥ�� 
		[1] TSSI Detect	  : -90dBm, -65dBm �̸��� (Alpha : 100MHz Only ��� -90)		
		[2] TSSI Detect(Dec.) : 0, 50�̸� ���ǽ�(Alpha : 100MHz Only ��� 0)
		[3] RSSI Detect	  : -100dBm, -80dBm �̸���(Alpha : 100MHz Only ��� -100) 	
		[4] PD Power : -40dBm, -21dBm �̸� ����

	7. AutoConfig-Restart, When,
		(1) REC Port Num ����.
		(2) cascade-ID ����.
		(3) base-MAC/base-IP ����.

V07.25.00 : 
	1. L/S packet Debug : string display
	2. FPGA Download ����. (file update �ȵ� �� �ִ� �κ� ����).
	3. gREMS Suppress : test 3ȸ -> ���� 10ȸ.


V07.20.01 : 
	1. PRU : Mac Control added.

V07.20.00 : 
	1. L/S Delay Setting : Num of CPRI ���� "0"�̰ų� "2"�� �ʰ��� ��� Error Return(-502, TBD).
	2. L/S : "Send_LSH_UDP_Data" �Լ� pthread_mutex_lock ��ġ ����.
	3. L/S : delay setting : cpri_num parsing �� 0���� �Ǵ� �κ� ����.
	4. Reset Reigster(0x09) : debug ǥ��.
	


V07.18.03 : 
	1. gREMS : Alarm String Added
	2. gREMS : Inventory Auto Report When SFP info is changed.
	3. AMP Standby Mode / MIMO Standby Mode : ���� �ÿ�  'disable' �˶� ����ŷ
		(1) Mode ���� �� �˶��� �����ϵ�, ���� �Ŀ� �߻��Ǵ� �˶��� Mask ó��	
		 == ���ԵǸ�, ������ �˶��� �����ǰ�, �������� �����̾��� ���� �˶��� ���� �ʵ���,
		 == mode ������ �˶�����(normal or alarm)�� ���� �Ŀ��� ��� ���.
		(2) AMP ���� alarm,  TSSI/RSSI, Power ����/���� �˶� �κп� ���� ó��.
	4. 	Initialzation Notification Acknowledge�� �ޱ� �������� 
	���� ��û � ���� �޽����� ���� ���� X
	

V07.18.01 : 
	1. suppress 10 -> 3, clear time 60[s] -> 30[s]
	2. dtu board alarm : message every 10[s] ǥ��.
	3. clock alarm : payload off
	(register 0x07 pll �κ��� alarm�̸�, ���� register ���� �ȵǳ�, clock alarm�� ��� payload off�� register ���� �����ϰ� ����)


V07.17.03
	1. Loopback On/Off : (���� X, ���� Only)
	VSS �ʵ��� Loopback �ʵ尪�� 0xCC, 0xCA�� ��� On, �̿� Off�� ó��
	2. Function fail : 
		(1) Layer �̻�� �� FuncFail ALM �߻����� �ʴ� �κ� ����.
		(2) defalut�� 0xCC�� ����, ���� �˶� üũ ���ۺ��� ���¿� �°� ���� ��û :  
		(������ 0x33, �׷��� ������, 0xCC�� �̹� �����Ǿ� �־, �߰� �����κ��� ����.)
	3. PRU �� AAU�� �ʱ� ���ÿ� ���� DTU ���� ���� �Ǵ� ��� ����
	4. L/S�� ����, ID �Ҵ� �ޱ� ����, BaseMac/BaseIP ����Ǹ�, RCU MAC/IP ���� �߰�.

	

V07.17.02
	1. PSU : PSU���� internal FAN �ڵ����� ���϶��� 
		RCU���� FAN On/OFF�� ���� (Command 0x7C)��� PSU�� ������ �ʵ��� ����.
	2. VSS RE Channel BW (alpha) : 3~6 �� �ƴ� ���, �Ʒ� alarm masking
	CPRI alarm, SFP_LOS Alarm, SFP_LD Alarm
		


V07.16.00
	1. ���� CPRI �˶� �߻�(100MHz, aMHz ����)�� ���� SFP Tx disable�� ����
	2. CPRI alarm/SFP not insert : register 0M Control, but AID 0x5008�� VSS��� ǥ��.
	3. gREMS : RCU Start ��, ����ֱ� ������ 1ȸ�� ����, statistics 1ȸ report ���� ����.
	4. AID 0x5620 : Reset : Command 0xEA ó�� �����ϰ� ����.



V07.15.00
	1. Super Cap.��ü(AID 0x5452) Added.	
	2.�ʱ� ���� �� �˶� ���� ������ L/S����  
	(1) Initialization Information Acknowledge Message�� �����ϰų�
	(2) Valid indicator == 0x01(True)�� ������ ��쿡 ����	
		Valid indicator�� 0x01�� �Ǵ� ����
		(1) Init Noti ���Ž�
		(2) Init Noti�� ó�� ��û�� �������� 1�� ��� �� Valid indicator�� ��ü������ 0x01(True) ����

V07.13.00
	1. VSS MAC Fail Check added : "__USE_MAC_CHECK__"
	2. PRU : Use Fused-MAC @ etc-partition
	3. BW 100+a���� 100Mhz �� ����� alpha�� Payload Off�ϵ��� ��� �߰�
	(�ݴ�� 100Mhz -> 100+a�� ����� ��쿡�� Payload �ڵ� On)	
	4. CPRI#1 �� SFP Ż���� Alarm : ��Ȯ�� test �ʿ�.

V07.12.02
	1. DTU ���� ����� OFFSET ó�� : 5.8V(+0.3)/1.8V(-0.1)/0.85V(-0.1)
	2. TSSI DEC. �� OFFSET ó�� : DTU���� �÷��� ���� "�� 0.74"ó��
	   (dBm ��굵 0.74 ó�� �� DEC. ������ ��� ����)
	3. Register ���� ���� ����.
	10*log10(RSSI/2^32) --> 10*log10(RSSI/2^31)
		UL INPUT RSSI DET (0x6)
		DL_TSSI_RMS  (0x04)
		DL_TSSI_PEAK (0x09)
		UL_RSSI_RMS  (0x22)
		UL_RSSI_PEAK (0x29)
	4. Register "TSSI_SD_Level" ���� ��� �� ����.
		(1) remove tssi_offset from tssi_level
		(2) divide 0.74 to reg-control-value
		(3) 0xC052 Offset �����ص� ����ǵ��� ����.
	


V07.10.00
	1. A_ID ǥ�� ��� : �̹� �����Ǿ� ����.
		- TBdelayDL(DL Hopping Delay) 0xB42C ǥ�����
		- TBdelayUL(UL Hopping Delay) 0xB42D ǥ�����
	2. �ڵ����� ���ؽ�(AID 0x5603
) Added
	3. GUI_ID (AID 0xB012), GUI_Password (AID 0xB013) Added
	4. Alarm Mask (AID 0xB014) Added
	5. TSSI_SD_Level_Set (AID 0x5101) : register control : ���� ����. 
	 


V07.09.00
	1. (z.16.0) �� (z.80.0) ���� : 
		Main_AID (CPRI, 0x5301) �� LOS/LOF �߻� ������ �����ϰ� ����.
	2. (z.22.0)Optic LOS Detect/Alarm ���� : 
		Main_AID : LOS(SFP) 0x5302 ����.  Alarm(0xCC), ����(0x33)



V07.08.00
	1. Masking alpha-SFP-alarm : "SFP_Ins_0_a" When BW-Alpha is not used	
	2. AID 0xB437 : ADC : ���кй� �� ���� ���̹Ƿ� X2�Ͽ� ���°� ǥ��
	3. AID 0xB438 : ADC 
		(1) AID �����ϴ� ���� ���� : 3.3/1.8/1.2/0.85V ������.
		(2) ���кй� �� ���� ���̹Ƿ� X2�Ͽ� ���°� ǥ��
	

V07.04.03 : Not Release
	1. gREMS : CF/nC Auto Report Added (When AID Value is chnaged)


V07.04.02
	1. �ʱⰪ�� 3dB(��� �ѹ��� ������ ��)
		AID 0xB420(DL Atten), 
		AID 0xB421(UL Atten), 
		AID 0xB424(DL Sum Gain), 
	2. AID 0xB40D(Hard_Clip_Lvl) : 
		hard clipping off �϶��� ������ ����� ����� ���̵��� ����.
	3. CPRI not insert ��, BIP count(����/����) 0���� reset.
	4. AC Fail Check : "GPIO_PSU_AC_FAIL_I" 300[ms] -> 100[ms] ����.	
		(1)	AC �˶� ���� ���� : 100ms �̻� ������
		(2)	AC �˶��� ���� DTU ������� ���� : AC �˶� 300ms �̻� ���� ��(���� ���� ����)		
		(3)	AC �˶� �߻� �� ���� ����� PAU0�� Reset Pin toggle ����(�����)
			A.	AC �˶� ������ REMS�� �˶� ����� Reset ���� High -> Low�� ����(1ȸ)
			B.	2ȸ retry REMS�� �˶� ����� Reset ���� Low -> High�� ����
			C.	3ȸ retry REMS�� �˶� ����� Reset ���� High -> Low�� ����
	
	5. Continued_download : ����GUI ���� �ȵǴ� �κ� ����.
	6. LSH : "FX_DELAY_SET_REQ" �� ���, 'Number of CPRI' �׸��� �����ϵ��� ����.

V07.03.03
	1. TSSI : decimal ǥ�� ���� ����.(V07.01.01 ���� �߻�)
	2. TX ��� ���� ������ ���� : ���� ����Ǵ� BW�� ���� ����� ����
	3. AID 0xB034 : DPD_Eng_Out_Pwr ��� DPD_Eng_In__Pwr�� ����.
	4. �������� ����. PRU AAU
	5. FTP ���� �� fail ���� �����ϴ� �κ� �߰�
	6. ��� ���� ����
		(1) 100M Only ��� ��� ���� ���� �ʰ� AMP ��� ���� �״�� Main ��� ���� �׸� ǥ���ϵ���.
		(2) AMP ����� 0�� ��� ��� ���� ���� ��� 0���� ǥ��
	7. BW ���� ����.( alpha 10M/20M �� 0M ó�� ó��)

	

V07.03.00 : by ycSo
	1.	FTP �ٿ�ε� ����
		FTP �ٿ�ε�� ��� ���θ� ���� �ϵ��� ����
		FTP ���н� error code �������� ����.
		FTP command (0x50) �� ��쿡 error�� ������ ���� ip ���� ����.
	2.	�ڼ��� ���� ��û���� �������� data ����.
	3.	AAU GPIO_CONF_DAT_EN EXIT �߰�


V07.02.01
	1. amp ���� alarm ����.
	(1) amp standby : alarm backup
	(2) mimo standby : amp off alarm : user amp on/off�� ���.

V07.02.00
	1. cpri unlock(alarm) timing check ���� ����
	2. "SFP Not Insert" or "CPRI Alarm" �϶�, BW ���� ���� : ���� 10M ���� -> 0M ����


V07.01.03
	1.	Task ���� ó�� �κ� �߰�
	2.	DL�� �������ÿ� ���� ���� offset�� �߰�. 


V07.01.02
	1. reg "PA_OnOff.On" �� �������� �ʵ��� ����.
	2. AC Fail 300ms check : RCU init ��, timer �ʱ�ȭ �߰�.
	3. APP Restart ��, cpri-ulock�� �ʱ� ���·� ��.
	�ʱ� 1st lock ������ 2��, ���Ĵ� 10�� ������, cpri-lock.

V07.01.01
	1. 
		AID 0xC062/63/64 : reg "DL LSH ����" Gain Added.  
		AID 0xC065/66/67 : reg "UL LSH ���� Gain"  Added.  
		AID 0xC068       : reg "Test Pattern Band Gain" Added.  
		
	2. TSSI ���� �˶� ����
	���� : TSSI ������ �������� ��
	���� : reg "TSSI Shutdown Status" ���.

	3. reg "DL Gain" �� balance ����ϴ� ���, reg "Height_Bal_Gain" ���.
	4. reg "DL Sum Gain" added.
	5. AID 0xC052 : TSSI_Offset_x10 : GUI���� ������ϰ� ����.
	

V07.01.00
	1. AID 0xC052 : TSSI_Offset_x10 : GUI���� ������ϰ� ����.	
	2. 
		AID 0xC01D : TSSI SD LV(Short) Added.  
		AID 0xC01E : TSSI SD LV(long)  Added.  
		AID 0xC01F : TSSI SD LV(Low)   Added.  

V06.29.00
	1. gREMS
	(1) �ڵ����� ���� ���� ���Ž� ���Ҹ� ����(GUI ǥ��)
	(2) auto report(Alarm, ��躸��)�� ������ L/S�� �ð����� ���� �� ����
	(L/S���� ��ȸ/���� ��û�� ������ ó��)
	(3) Ethernet Packet Loopback added.
	(4) BIP Error Count : ���������͸� ����(�ִ�/�ּ�/��տ� ��� ������ ���������� ����)
	(5) Hard Clipping count : ���������͸� ����(�ִ�/�ּ�/��տ� ��� ������ ���������� ����)
	(6) Hidden Request/Response(0xE9) 
	(7) ��躸�� 1024 packet Ȯ��.
	
	2. Optic Switch Status0 
	(1) Optic Switch ��ġ(high)/�̼�ġ(low)
	(2) AID 0x501C : ���� ���� -> ���� �� ���÷� ����

V06.28.11
	1. SFP Ż���� 0x5315 Alarm �߰�
	2. PAU On/Off(DL) 0x5116 �Ӽ� ���� Operation -> CF/C : 
		MIMO Standby/AMP Standby/Battery Ext : PAU �˶� ����ũ ����� PAU ���� Ȯ�ο�.
	3. RU Initial status 0xB009 ���� -> Main �̵�  
	4. ���Ҹ� 0xB010 �߰� 	
	
V06.28.10
	1. RU Initial status 0x500D Added.
	2. Clock fail �϶�, payload off �ϵ��� ����.
	3. LOS(SFP) 0x5302, LD(SFP) 0x5304 : alarm ���θ� ���.
	4. �Ʒ� �̹� �����Ǿ� ����.
	 - Optic Delay (Round Trip Delay) 0x5405 "-" ǥ�� ���� �߰�
	 - TBdelayDL(DL Hopping Delay) 0x5401 "-" ǥ�� ���� �߰�
	 - TBdelayUL(UL Hopping Delay) 0x5402 "-" ǥ�� ���� �߰�
	 5. gREMS ��� �����,
	 - BIP Error Count(CPRI) 0x530A �� ������ ����.
	 - Hard Clipping Count(DL) 0x5320�� ������ ����.
	 6. RU Initial status 0x500D
	 7. VSS : 10ȸ ���� ���� : BW �κ� �߸��� �κ� ����.
	 8. AID 0xC01C (DL Input Power Balance Att) ����.
	
V06.28.00
	1. AID 0xC01C (DL Input Power Balance Att) Added.	
	2. FPGA init Check ��� ����.
		clock alarm (m_St.AlmSt.A_4.Clock) ���, 
		m_Reg_Sts.R_Board_Status Register �� 2�� bit(Pll_Init_Done ,JESD_Lock)�� check.

	3. L/S : 
	(1) "Alarm_Data_Vaild_Indicator" �� "Vaild_Indicator" �� �����ϵ��� ����.
		(������ �׻� 1�� ����.)
	(2) "INIT_NOTI_ACK" ������, Vaild_Indicator �� 1�� �����ϵ��� ����.

	4. default IP : �׻� 192.168.0.3 �� ����.(���� : ���� ���Ǿ��� ���� ���.)
	5. FGPA Test Register ��� : ���� 2ȸ Fail �̸�, FPGA read/write ���� ����.	
	6. CPRI lock�� CPRI Alarm�� ���� ���� : ���� 10��, �˶� 2��.
	
	
	
	


		
	

V06.26.02
	1. gREMS report when AC Fail
	2. GPIO_OPT_SW_Status1 : default High Control
	3. gREMS_Auto_Report_Retry_Max : 4ȸ.

V06.26.00
	1. PSU AC Fail IN Pin : 300ms monitor�� ����.

V06.24.00
	1. Restart ��, Inventory Auto Report to gREMS

V06.21.04 :
	1. When CPRI unlock : 
		(1)C&M ó�� X, VSS Read(REC Type ����)/Write X
		�ٸ�, Register Read/Write (RCU Restart ��, EEP �ʱ�ȭ ����.) �� �����ϵ��� ����.
		(2)L/S ID �Ҵ� �ȵ� ���¿��� lock -> CPRI Unlock -> lock �̸�, ID �Ҵ� ���û �� �� �ֵ���.
	2. FAN(����) �ڵ����� �� DTU �µ� ������ �����ϵ��� ����.
	3. Hidden_AID 0xB432 "SFP On/Off" ���� �ȵǴ� ���� ���� : Register 0x07
	4. RU ID Acqu' Noti  &  Ack  �Ϸ� ���� Initalization Noti Ack�� �������� �ʵ��� ����.
	5. (z.22.0)Optic LOS Detect/Alarm ���� : AID : LOS(SFP) 0x5302 ����.  Alarm �߻� �� 0xCC, ���� �� 0x33
	6. VSS Table �� �ý��� ��ɰ� ������ �ִ� �׸�� �� �ý��� ������ Ȯ���� ���� ���� Data ���� Ȯ�� �� �����ϴ� ���	
		RE Channel BW : 100ms �ֱ� 10ȸ ����(0x1~0x6 �̿� �� ����)
		Loopback mode : 100ms �ֱ� 5ȸ ����(0x33, 0xCC, 0xCA �� �ܿ� ����)
		TDD slot number[20] : 100ms �ֱ�, 10ȸ ����
		CPRI Compress : 100ms �ֱ�, 10ȸ ����
		C&M Path Selection : 100ms �ֱ�, 10ȸ ����
		RE Cascade ID : 100ms �ֱ�, 5ȸ ����
	

V06.21.03 : by YCSo
	1.	�������� ���� ����� �κ� ���� ��� ����.
	2.	�ٿ�ε�� ���ο� F/S�� �°� ���� �κ� ����.( ���� ���� ������ �ƴ� ����)


V06.20.00
	1. REMS response : SUBID 0xFFFFFF �� ����.



V06.19.01
	1.	PRU & AAU register read / write �κ��� address ���� 0x200���� ���ؼ� GUI���� ���ϰ� ��ȸ
	2.	APP ���� ��, RCU APP version ���� ǥ�� 
	3. 	�Ʒ� ���� �ٽ� ����.
	"2019.06.09 : ���� CPRI �˶� �߻�(100MHz, aMHz ����)�� ���� SFP Tx disable�� ���� �� LD �˶� Masking"
	4. Clock Alarm �߻� �� Shutdown �˰��� ����
	5. CPRI Alarm �߻�->���� �� 10�� ������ CPRi#2, CPRI#3���� �����ϰ� ����.
	//-----------------------------------------------



V06.18.05 : 2019.06.18
	1. MIMO Standby ���� ���� ���� ���� : all amp �������� ������ �� �����ϵ��� ����.
	2. Alpha BW �� 0/10/20 �� ��쿡�� BW �� �ڵ����� �����ǵ��� ����.
	3. RCU Temp Offset Added : AID 0xB417
	4. AID 0xB421 UL Att ����� ǥ�� ���� ���� : �����, Gain Balance(AID 0x5209)�� ���� �ȵ� �κ� ����.	
	5. FPGA Register Read/Write : 
		- download ���� �ƴϾ ���۵ǰ� ����.
		- cpri Unlock �̾ ���� �ǰ� ����.
	
	6. �ʱ� ���� �� �˶� ���� ���� : L/S���� ���ŵǴ� Alarm Acknowledge == 0x01(Acknowledge)�� ������ ���
		when Acknowledge = 0x01 ?
		(1) Init Noti ���Ž� Alarm Acknowledge�� 0x01(Acknowledge)�� ���.
		(2) Init Noti�� ó�� ��û�� �������� 1�� ��� ��, ��ü������ 0x01(Acknowledge) ����

	7. CPRI/Clock Fail �˶� ���� ���� 10�� �߰�/ ��Ÿ �˶� �߻�/���� �ð� ���� �� �� �ֵ��� ���� ����(PRU/AAU ����)	
	8. VSS �ʵ� ��� �̱��� �� ����(PRU/AAU ����)
    	(1) LOS/LOF OPTIC LOS �˶� ���� �ʿ�
    	(2) Function fail ����


V06.17.00
	1. RVS OVER PWR Shutdown alarm�̶�� payload off ���� �ʵ��� ����.

V06.14.00
	1. L/S Delay ���� ����� byte order �κ� ���� : result.sD32 = htonl(rst)
	2. VSS UL : RE_Loopbackmode_ACK ��� �����ϵ��� ����.


V06.13.05
	1.  T2a(DL_Delay), T3a(UL_Delay) ���� ���� ���� ���� ó�� �߰� 
	2.	Center Frequency ���� ����� �߰��ϸ鼭 PRU�� RE Channel BW�� ���� ������ ���� ���� ��û �帳�ϴ�.(~06/13)
		a.	VSS Table �� ���󰡵��� ����
		b.	BW ����� Main/Alpha�� ���� Defalut Center Frequency ���� �� ����

V06.11.01	
	1. 0x5209 Gain Balance : max 15dB limit added, �Ҽ��� ���Ž� ���� ó��
	2. MIMO Standby ����/���� level : 0x543A/3B �߰�.
	
	*. AID 0x0001 : Alarm : 27byte x 2 (1st 27byte�� alarm suppression, 2nd 27byte�� alarm )

V06.10.03
	1. �� A_ID �׸� ����(�� A_ID --> �ű� A_ID�� ����)
		 - Reset  0x5620
		 - ��躸�� ���ؽ� 0x5600
		 - ��躸�� �ֱ�  0x5601
		 - ��� ���� �ð� 0x5602
	2. ���� : LOF(CPRI) 0x5303, LOS(SFP) 0x5302 Mea ����, LD(SFP) 0x5304 Mea ����	 
	3. UL GainBalance ����/���� ����.
	4. AID : D/L State 0x5510 �߰�.
	5. App ���� �� consol �� version ǥ�� ���� ����
	6. 0x5012 (��ü�µ�), 0x5013 (��ü�µ�����) : ǥ�� ���� ����.


V06.09.00
	1. UL Gain Balance�� Att : UL Gain Back-off on/off ���¿� �����ϰ� �� ����.
	2. RSSI display �� ����
		PRU ����� : UL �Է�(GUI�� RSSI) = RSSI(Digital) - (37dB - UL Att- Back-off Att - Gain Balance Att)
	3. ���� CPRI �˶� �߻�(100MHz, aMHz ����)�� ���� SFP Tx disable�� ���� �� LD �˶� Masking
		�������� �������� CPRI#0[#1] �˶� �߻��� CPRI#2[#3]�� SFP Tx disable ����

	4. �� A_ID �׸� ����(�� A_ID --> �ű� A_ID�� ����)
	 - ������ 0x0100 -> ������ 0x5500
	 - ���޻� 0x0101 -> ���޻� 0x5501
	 - �߰�� Type 0x0102 -> Type 0x5502
	 - S/W ����(����) 0x0149 ->  S/W ���� 0x5503
	 - ��ü �µ� 0x0201 ->��ü �µ� 0x5012
	 - ��ü �µ� ���� 0x0202 -> ��ü �µ� ���� 0x5013
	 - Battery ���� 0x0210 -> Battery ���� 0x501B
	 - OSM ��ġ ���� 0x0249 -> OSM ��ġ ���� 0x501C
	 - Battery Status 0x027D -> Battery Status 0x501A
	 - RCU D/L State 0x2B30 -> D/L State 0x5510
	 - Reset 0x0914 -> Reset  0x7020
	 - ��躸�� ���ؽ� 0x55 0x00
	 - ��躸�� �ֱ� 0x55 0x01
	 - ��� ���� �ð� 0x55 0x02
		
V06.04.00 :	///// YCSO
	1. �������� ���� ��� ����. 8192
	2. cmd 0x99 �߰� 
		- Register_inquire �Լ� �� ���� �Լ� �߰�.
	3. App ���� �� version ǥ�� �߰�.
		- FPGA, DTU, PSU, PAU 0 ~ 4 ����� version ǥ��.

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
	1. AID 0x500B(Center Freq) : 0.00.1 -> 0.001Mhz step ����.
	2. PRU ���� NCO Register added.
	3. AID ����.
		- T2a(DL Delay) 0x5403 �׸񰳼� ����  4 -> 1
		- Ta3(UL Delay) 0x5404 �׸񰳼� ����  4 -> 1
	4. �׸� ���� : NG-ARFCN 0x5007
	5. Gain ����(UL) Threshold 0xB305 Data type: 0x04 -> 0x03 ����	
	6. UL Gain Balance 0x5209 : AID �� �߰�.
	7. Offset(UL Noise) 0xB309 : same with 0xC05C AID
	8. LNA �Է�(UL) 0xB2 0x1C �߰� : same with 0xC05A
	9. MIMO Standby Mode(����/���� Level) 0x543A/3B


v05.29.10
	1. "Proc__CPRI_Init_OSM_Ctrl" : ���� ����.


v05.29.00
	1. MAC/IP Set with IOCTL
	2. "Proc__CPRI_Init_OSM_Ctrl" ����.
	3. loopback register control : �ð��� register ������ VSS-loopback �����̳�, 
	VSS-loopback ��������, register �� ����(�뷫 100ms) ����ǰ� ����.

v05.28.14
	1. UDA Spurious ���� ���� : "threshold (+/-)3dB" -> "over threhold level"


v05.28.13
	1. RVS Ÿ�� ���Է� level ����.: -17.0 -> -47.0 

v05.28.12
	1. CPRI INIT : "__20190528_FIX__" added.
	2. TSSI Offset : +8.0 --> +5.2	
	3. RSSI Offset
		Filter �� : +14.8dB
		Filter �� : -29.9dB
	4. Noise Offset : -17.6dB
	5. �������� : span conversion

v05.28.03
	1. t-sync offset ���� �߰�.(GUI unit : [us] -> [ns])
	2. AID 0xC04E/4F : T2A/T3A delay (Read Only) �뵵 ����.

v05.27.00
	1. AID 0xC05A : PAU LNA In Power added.
	2. AID 0xC05B : Offset(FILTER ��) added.(alpha is not needed..)
	3. AID 0xC05C : Offset(UL Noise) added.
	4. ���������� ���� VSS RE_CH_BW ó�� : ���� 10ȸ �϶�, �������� ������ ó��.
	5. When Cascade-SFP-Not-Insert : TBDelay, RoundTrip Delay : 0xFFFFFFFF ó��.
	

v05.25.00 // not release
	1. Temp Alarm : hysteresis ����. : 2 -> 5
	2. L/S : IE_System_Sts : Batt insert ���� : AAU(0:�̻��), PRU(1:insert, 2:not insert) added.
	3. AMP-Standby, Mimo-Standby, Fan-Mode : 
		- ���� ��, �ð��̳� mode(���� path)�� �����ϸ�, mode off ����ǰ� ����.


v05.24.03
	1. ���� PAU�� ALC on �̸�, ����, amp rvs atten ���� �ȵǴ� �κ� ����.
	(0xB224 (PAU RVS ATT) : ALC on ��, atten ���� ���� ��, break ��� continue ����.)
	2. PAU : RVS ATT : RCU Reset �ʱ� ���� �ǵ��� ����.
	3. AID 0x5405 : round-trip-delay : �������� flag ���� : 0xC0 -> 0xF0
	4. FPGA : T2A delay, TA3 delay ���� ����.
	5. UL_GAIN_Measure_Time : AID �����Ƿ�, RCU Reset ��, �ʱ� ���� ���� �ʵ��� ��.
	

v05.23.0 // not release
	1. NOKIA L/S ���� Added.
	2. register : "T2A delay", "TA3 Delay" added.

-------------------------------------------------------------

v05.22.05
	1. pkg download ����

v05.22.04
	1. pkg download ���� ���� merge.

v05.22.03
	1. CPRI init ����

v05.22.02
	1. "Insert_GUI_IP_Table" bug fix
	


v05.22.00
	1. L/S Alarm Noti, Alarm Status : structure initial
	memset(s_data, 0, sizeof(ST_ALM)*100)
	2. L/S Alarm Noti : SFP LOS/LD Alarm : Send ��, alarm �������� �ʾƼ� ���� send �ϴ� �� ����.
	��Ÿ...
	-. cpri alarm : 2sec ����.
	-. CPRI init algorithm : OSM switch �� �����ϵ��� ��û

v05.21.16 : 
	v05.21.15 ���� download merge �� ����.	

v05.21.15 :
	1. package downloatd ����.(merge)
	2. cpri alarm : 2sec ����.
	3. CPRI init algorithm : OSM switch �� �����ϵ��� ��û

v05.21.14 : CPRI init added -- reset register �߸� ����ϴ� �� ����.

v05.21.13 -- XXXXX ������ �ִ� ����.-------------
	1. AID 0x5403/04 (T2a/Ta3 delay): GUI - Control �����ϰ� ����.
	2. CPRI init algorithm
	3. spurious alarm : AID 0x544C (UDA_Spurious_Layer)�� ����ϵ��� ����.


v05.21.12
	1. TSSI offset : PRU : +8.0dB fixed.
	2. dpd algorithm added.
	3. DPD_Eng_In__Pwr, DPD_FB_Eng_In_Pwr : log ���� �߰�.
	

v05.21.10
	1. register : "SFP LOS Alarm","SFP LD Alarm" added

v05.21.08
	1. "__CPRI_SET_When_Reboot__" deleted (��, "Optic SWITCH ON" ����)
	

v05.21.07
	1. "__CPRI_SET_When_Reboot__" deleted

v05.21.05
	1. tree
	2. "__CPRI_SET_When_Reboot__" added

v05.21.00
	1. L/S : Samsung IP Save.
	2. "Proc___PRU_before_OpticSw_ON" ����.
	   "MMAP____SW_Reset" ����.


v05.20.16
	1. LS ID Added : Samsung/Ericson(Nokia) : Base IP ���� LS ID�� ����.	
	2. 	Hidden (0xb634)�� 'batt status 3 (H/W Fault)'�� �߻� �ÿ�,
		'Battery Status'�� Reason : '�����̻�'  --> 'Battery Fault(HW)' ���� ���� 
		
	3. Temp Alarm : hysteresis ����. : 5 -> 2
	4. TSSI upper Alarm : hysteresis ����. : 2.0 -> 2.1
	5. Lock �Ǵ� ���ǿ� miscelaneous added.
		CPRI_0/1) if(CPRI_Mscl_16 == 0) CPRI_Mscl_Lock = 1;
		CPRU_2/3) if(CPRI_Mscl_16 == 0x0080) CPRI_Mscl_Lock = 1;
		
	6. register "DPD Successful Count" added.
	7. AID Change by Register Map
	   - m_St.d_DPD_St : 0xB405 -> 0xC032
	   - m_St.d_DPD_Err_St : 0xC032 -> 0xB405
	8. DPD �׸� �߰�.	
		DPD PeakExpansion
		DPD MSE
		DPD PRE MSE
		DPD LUT Value
		DPD Count
	9. "VSS table reload" ����.
	10. CRPI Lock : 100M, Alpha ��� ������ ����.
	11. "__EXCEPT_Alpha_at_CPRI_LOCK__" ����.
	12. "__USE_Spectrum_LOW_Limit__" : spectrum filer ����� ���� ���Ѱ� ����(-50.0)
	13. PAU : 0xB222/23/26 : temp att : signed ����.
	14. PAU : VSWR : PAU ���� ��� ǥ��.
	
	

	


v05.18.02
	1. RVS Ÿ�� ���Է� ALC : Level -10.0[dBm] --> -17.0[dBm]

v05.18.01
	1. RVS Spurious Test Ȯ��.
	2. CPRI1(alpha) not insert ��, BW 10M ����.
	3. RVS Ÿ�� ���Է� ALC : ���� Test Ȯ��.
	4. Clock Alarm �߻� �ÿ� Reg 0x07���� �α׷� ��� 


v05.17.03
1. spectrum
	- "__SPECTRUM__OFFSET__" : Defined
	- �������� filter ������ ����.
	- filter : ���� ��ȣ ����.
2. UDP download �ӵ� ����.
3. Vss_UL_RE_Reset_Ack ����ӵ� ����. : "__USE_VSS_UL_RE_RESET_ACK_SW__"
4. PRU : ����� Debug message�� "VSS table reload"
	PRU�� ��� CPRI ������ ��Ȥ VSS ���̺��� �̻��ϰ� ������ ���
	Open CPRI Version == 0x00 or 0xFF ���°� 2�ʰ� ���� ��, 
	PS Register 0x09������ 20�� bit�� 0���� ������ 1�� ����
5. AID
	LS_ID[MAX_Optic](0x500C) ; ??
	alarm ����.(�׸� �߰�)	
		LOS(SFP0)      (OPTIC_TRANSCEIVER_RX_LOS)	3	0x5302
		LOS(SFP0)(+��) (OPTIC_TRANSCEIVER_RX_LOS)	2	0x5302
		LOS(SFP1)      (OPTIC_TRANSCEIVER_RX_LOS)	1	0x5302
		LOS(SFP1)(+��) (OPTIC_TRANSCEIVER_RX_LOS)	0	0x5302
		LD(SFP0)       (OPTIC_TRANSCEIVER_TX_FAULT)	7	0x5304
		LD(SFP0)(+��)  (OPTIC_TRANSCEIVER_TX_FAULT)	6	0x5304
		LD(SFP1)       (OPTIC_TRANSCEIVER_TX_FAULT)	5	0x5304
		LD(SFP1)(+��)  (OPTIC_TRANSCEIVER_TX_FAULT)	4	0x5304


6. 
 - Toffset 0x5400 ����, ���� �̵�(0xB42B)
 - TBdelayDL(DL Hopping Delay) 0x5401 ����, ���� �̵�(0xB42C)
 - TBdelayUL(UL Hopping Delay) 0x5402 ����, ���� �̵�(0xB42D)
7. DPD Status(DL) 0xB405 LEN ���� 1 -> 2
8. Gain ����(UL) Threshold  0xB305 �߰�
9. L/S : "OPTIC_TRANSCEIVER_RX_LOS", "OPTIC_TRANSCEIVER_TX_FAULT" Alarm added.
10. Battery Monitor : test time  -> ���� �ð����� ����.







v05.16.02
	1. Download �̾�ޱ�.
	2. �������� ���� : �ӵ� ���� ��.
	3. tv.tv_usec = 10000; // 10[ms] 2019.05.16


v05.16.00
	1. TDD Sync (AID 0xC021) : register conversion ����.( GUI���� usec ������ �����ϴ� ������ ����.)
	2. FPGA User Delay (AID 0xB429) Ȥ�� L/S�� ���� FGPA Delay �����, TDD Sync (AID 0xC021) �ǵ��� ����.
	3. loopback Mode : Response-Debug-Time is added in the console.
	4. DL CW Level, Onoff : Level EEP Save, CW Off when reset.


v05.15.04 (AAU)
1. TDD Sync
 ����
 [Reg_TDD_RW_UL_ON_Guard_Time]    <-- DL_to_UL_Guard_Time
 [Reg_TDD_RW_DL_to_UL_Guard_Time] <-- UL_ON_Guard_Time
 ����
 [Reg_TDD_RW_UL_ON_Guard_Time]    <-- UL_ON_Guard_Time
 [Reg_TDD_RW_DL_to_UL_Guard_Time] <-- DL_to_UL_Guard_Time

2. power offset added.
	(DL��� FB)	0xC051, (DL TSSI) 0xC052
3. AAU RVS Shutdown : off time : 5[sec] --> 10[sec]
4. AAU FWD Shutdown : DL_RSSI_FB total power�� shutdown �ǵ��� ����.
5. Check and Get "Current MAC/IP" Every 3sec
6. UL-Gain-Backoff onoff : VSS-DL
 cascade ID 0,1,2,3 ��, 
 cascade ID 1,2,3 �� ���, DL-CRPI0/1�� ���� 'UL-Gain-Backoff onoff' �� 
 DL-CRPI2/3 ���� copy �ϵ��� ����.


v05.15.04 (PRU)	
1. UL-Gain-Backoff onoff : VSS-DL
	cascade ID 0,1,2,3 ��, 
	cascade ID 1,2,3 �� ���, DL-CRPI0/1�� ���� 'UL-Gain-Backoff onoff' �� 
	DL-CRPI2/3 ���� copy �ϵ��� ����.
2. Check and Get "Current MAC/IP" Every 3sec
3. DPD Alarm Algorithm ����.


-------------------------------------------------------------
/////v28.00.04(AAU) : 2019.05.14.- not release
	1. AID 0xB007, 0xB008 : GUI ���� �����, null ���� character ������ ���� �� �ǵ��� ����.
	2. CPRI Unlock ��, 
	- VSS �д� �͵� ���� �ʵ��� ����.
	- VSS_���� Copy���� �ʵ��� ����.
	- VSS ���� Register Write ���� �ʵ��� ����.
	- L/S : Auto_Config_Task ���� ���ϰ� ����.
	- Console �� unlock message ǥ��(10�ʸ���)


v26.00.04(PRU) : 2019.05.14.
	1.	CPRI�� DL/UL BW Register Control
	* when (SFP Mount) && (CPRI0/1 �˶�) : CPRI0/1 BW Register�� 0x01(10M)�� ����,
	* when normal : VSS Table�� �ִ� ������ �ٽ� ����(AAU�� User Control)
	2. RE Channel BW : VSS ���󰡵��� ����.(PRU), (AAU�� VSS ���󰡰� ����)
	3. UL RSSI ���� alarm ���� 
	4. UDA_RVS Spurious : test time(payload off 10[min] -> 1[min]
	   AID 0xB306 : RSSI(UL Noise) added (Register added too)
	5. UL-Gain-Backoff : VSS-DL Control : Cacade_ID�� ������� ���� CPRI2/3 control
	

v28.00.04(PRU) : 2019.05.14.
	1. AID 0xB007, 0xB008 : GUI ���� �����, null ���� character ������ ���� �� �ǵ��� ����.
	2. PRU Only : CPRI-Unlock check�� 100M �� �ϵ��� ����.
	3. CPRI Unlock ��, 
	- VSS �д� �͵� ���� �ʵ��� ����.
	- VSS_���� Copy���� �ʵ��� ����.
	- VSS ���� Register Write ���� �ʵ��� ����.
	- L/S : Auto_Config_Task ���� ���ϰ� ����.
	- Console �� unlock message ǥ��(10�ʸ���)
	4. PRU : "AC_Fail" mask �ϴ� �� ����.(������� ����)
-------------------------------------------------------------



2019.05.13. v29.00.04(AAU) : 
	1. PRU : UL-Gain-Backoff : VSS-Z.30.0 �߰�.
		* when cascade-id == 0, after user-control, Control VSS-DL-CPRI2/3-VSS-Z.30.0
		* when cascade-id == 1~3, Read VSS-DL-CPRI0[ or CPRI1].
		** gain backoff �ϴ� �κм����ȵ�(���� RF ATT �̰�, Digital ATT���� �����ؾ� ��.)
	2. AAU_FAN_MODE ��� Ȱ��ȭ.
	3. PRU : "AC_Fail" mask.

2019.05.13. v29.00.04(PRU) : 
	1. PRU : UL-Gain-Backoff : VSS-Z.30.0 �߰�.
		* when cascade-id == 0, after user-control, Control VSS-DL-CPRI2/3-VSS-Z.30.0
		* when cascade-id == 1~3, Read VSS-DL-CPRI0[ or CPRI1].
	2. PRU : "AC_Fail" mask.
	
2019.05.13. v30.00.04(PRU) : TEST Version
	1. RE Channel BW�� User ��� �ƴ�, L/S ���� �����ϴ� BW ���� ���� ������ �Ͻ������� ����.
	("__Use_USER_RE_Channel_BW_CTRL__")
	2. PRU : "AC_Fail" mask.

2019.05.13. v31.00.04(AAU,PRU) :
	1. AAU/PRU : LayerUseInfo(AID 0x5002) GUI-Status-Response ��, flag ����.(���� GUI������ ���� ǥ�� �ȵǴ� �κ� ����.)
	2. PRU : DTU DL/UL Atten �ʱ� ����(RCU Re-Start ��) �ȵǴ� �κ� ����.
	3. PRU/AAU : FPGA Delay : AID (0xB429/2A) ���� �ȵǴ� �� ����.
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
	3. RSSI Offset ����.
	4. AAU : "FWD-ALC" algorithm added.


2019.05.10. v33.00.04(PRU,AAU) :
	1. AAU Inventory : DTU use RCU's S/N and Date(RCU share S/N and Date with DTU)
	2. L/S ���� protocol ����.
		a. Alarm IE�� Number of IF, Number of Len ���� �߰�
		b. Delay Value IE�� Number of CPRI, Number of IF, Number of Len ���� �߰�
		c. Optic Delay IE�� Number of CPRI, Number of IF, Number of Len ���� �߰�
		d. TSSI/RSSI IE�� Number of FA/Path, Number of IF, Number of Len ���� �߰�
		e. Path IE�� Number of Path, Number of IF, Number of Len ���� �߰�
	3. AAU : ���� ���� : I/Q�� ���� �ڹٲ㼭 ��� ��. 
	4. Offset(RSSI)	0xB307 ���� �� RSSI Total Power ����.

2019.05.09. v34.00.04(PRU,AAU) : 	  
	1. PRU�� AAU�� SIM ����� �߰�
	2. AAU table�� ����
	3.	Frame�� size�� 34Kbyte�� ����
	4.	TCP ����ϴ� �κ��� �߰� 

2019.05.09. v34.00.04(AAU) : 
	1. AID 0xB417 : "Offset(�µ�)"
	2. AAU 
		- ATTEN(LNA ALC)(UL) (0xB225) Added.
		- ATTEN(PAU)(DL)( 0xB220) added : "ATTEN(PAU)(DL)" : 0x5106 DL-ATTEN�� ����.
	    - ATTEN(PAU �µ�����)(DL)(0xB222) : AID�� �߰�. �µ������� �߰� �ʿ�.
	    - ATTEN(LNA �µ�����)(UL)(0xB223) : AID�� �߰�. �µ������� �߰� �ʿ�.
	3. PAU : Offset(PAU �µ�) 0xB217 : added.

2019.05.09. v39.00.04(PRU/AAU) : 
	1. AID 0x5208 'UL_Gain_Backoff' : AID 0xB426 �״�� ǥ��.
	2. AID 0x5315 'SFP Ż����' : AID 0xB43F �״�� ǥ��.
	3. AID 0x5400 "Toffset" : AID ����.
	4. AID 0xB308 "Threshold(UL �ҿ���)" Added.
	5. AID 0xB429/2A "Delay ������(FPGA)(DL)", "Delay ������(FPGA)(UL)" added.
	6. AID 0x530A : BIP ���� count -> instant value
	   AID 0xB427 : "BIP ���� count" �߰�.	
	7. AID 0xB428 : "BIP Error ���� Count �ʱ�ȭ" Added. 	
	8. Hard-Clipping Count
		AID 0x5320 : Hard-Clipping ���� count --> Hard-Clipping Count
		AID 0xB40E : Hard-Clipping Count      --> Hard-Clipping ���� count
	9. AID 0x544C : "UDA_�ҿ���_���� Layer" ǥ�� ��� ���� (100, +��и�)	


2019.05.08. v40.00.04(PRU) : Samsung L/S ���� v41.00.04 merge

2019.05.08. v43.00.04(PRU)
	1. Register "UL ALC GAIN" : PRU RVS Ÿ�� ���Է� GAIN �������� ���.
	2. Register "DL_GAIN" : Offset ATTEN���� ���, 0.1dB ATTEN (AAU�� ���� �ٸ�)
	3. Register "UL_GAIN" : Offset ATTEN + UL_Gain_Backoff�� ���, 0.1dB ATTEN (AAU�� ���� �ٸ�)
	4. UL Gain_Backoff (AID 0xB426) Added.
	5. PRU RVS Ÿ�� ���Է� GAIN ����.
	6. AAU : LowNoise Fan Mode Added.

2019.05.07. v44.00.04(PRU)
	1. L/S ���� ����.


2019.05.07. v47.00.04(PRU)
	1. UDA Test Version.
	2. UL Casacade on/off (AID 0xB435) Added.
	3. L/S : status response ��, alarm noti �� ���, packet ���� ����.
	4. Measured_Power_IE ���� : delete "Make_Frame_IE_Measured_Power()"
	5. serial TX_data �κ� ����.

2019.05.07. v45.00.04(AAU)
	1. Ÿ�� ���Է�
	2. UL Casacade on/off (AID 0xB435) Added.
	3. L/S : status response ��, alarm noti �� ���, packet ���� ����.
	4. Measured_Power_IE ���� : delete "Make_Frame_IE_Measured_Power()"


2019.05.06. v48.00.04(PRU)
	1. Ÿ�� ���Է�
	2. UL Gain_Backoff (AID 0xB426) Added.
	3. 0xB430 : Test Pattern On/Off Added.


2019.05.04. v47.00.04(AAU)
	1. Ÿ�� ���Է�, RVS ALC ����.
	2. UL Gain_Backoff (AID 0xB426) Added.


2019.05.03. v49.00.04(PRU, AAU)
	1. UDA�� ���� Reset ��, Reset Reason�� "��Ÿ"�� ����.
	2. PRU : UL_IN_RSSI (Register) Added.

	m_St.d_Layer_Use[m] = m_Reg_Sts.RFU[m].LayerInfo.Use; 
	pReg_RFU_CTRL_Addr[m][Reg_RFU_CTRL_RW_Layer_Info][idx] = &pDtuReg->RFU[m].LayerInfo.Data;


2019.05.02. v50.00.04(PRU, AAU)
	1. Layer Splitter : "IE_Optic_Delay"����, ���� CPRI not insert ��, 
	 roundtrip 0xFFFFFFFF, Hopping Delay(tbDelay) DL/UL 0x0000FFFF�� ����.
	2. L/S : Alarm Noti�� ���� Alarm ACK�� �����ϵ��� ��.
	3. AID 0x5403/04 : FPGA Delay(DL/UL) : FPGA read value �ƴ�, L/S���� ���� �� ǥ�÷� ����.


2019.05.01. v53.00.04(PRU, AAU)
	1. PRU : AID 0xB411 : DPD Reset added.
	2. Test-Alarm Added.
	3. Alarm-mask Added ( When GUI-Control )
	4. VSD cascade ID : 100M���� ���� Cascade-ID-ACK�� Alpha Mhs ���� ACK �ϵ��� ����.
	(�� �ݴ��� ��쵵)
	5. VSS : Loopback mode : alpha Mhz �κе� �߰�.
	6. FWD Output calculation(AID 0x5108) : SFP(100M/Alpha)  Insert���ο� ���� ǥ�� ���� �߰�.
	7. Layer Splitter : Alarm Status Response �� "Temperature high alarm" Added.


2019.04.30. v54.00.04(PRU, AAU)
	1. AAU : DTU Invetory Register added. 
	2. System offset Delay added
		#define DL_SYSTEM_Offset_ns  (8340) //  8.34[us]
		#define UL_SYSTEM_Offset_ns (13230) // 13.23[us]
		FPGA Delay Set = L/S_Set_Value - (system_offset + delay_offset)
	3. LS : "Path Status Request/Response" added
	4. LS : "FA Status Request/Response" added
	5. RE_Reset�� ����, VSS UL ACK �ϵ��� ����.
	6. PRU : RE_RESET �ǵ��� comment ����.

2019.04.29. v55.00.04(PRU)
	1. Proc__SuperCap_AC_Alarm_Report : ����.


2019.04.29. v56.00.04(PRU)
	1. "Proc__SuperCap_AC_Alarm_Report" ���� �ȵǴ� Test Version.

2019.04.29. v57.00.04(PRU,AAU)
	1. Inventory AAU-FPGA Added.
	2. output power calculation : AID 0x5108
	3. AAU : serial console ��¿� SPI log ����.
	4. AAU : LED ���� �Ϻ� ����.
	5. AAU : ��������� -100.0dB �� ����.(���� ���������� �̹� ����.)
	6. PRU : Hard clipping/ BIP Error Counter ��� �ٽ� Ȱ��ȭ : __TEST__Do_Not_This__ // 2019.04.29


2019.04.26. v58.00.04(PRU)
	1. Register TSSI_Shutdown Status added. ( flag == 1 �̸�, off���� ���� ����.)
	2. "PAU_EN_Ctrl" ����, PAU algorithm �� ���� fwd/rvs off �̸�, ON����� �������� �ʵ��� ����.
	


2019.04.25. v59.00.04(PRU)
- spectrum, package download log added.
- PAU AMP Reason �� ����, DTU DL/UL Off
- LED alarm ����,
- wavelength display ����.


2019.04.25. v60.00.04(PRU, AAU)
 - clock fail�� ���� DTU payload off, PAU off ����.
 - PAU Reason���� user ��� ���� �κ��� ����.
  

2019.04.25. v65.00.04(PRU, AAU)
	1. d_Hard_Clip_Lvl_x10 : for loop 2 --> 4 ����.
	2. RED_LED ON Control When cpri alarm.
	3. UL RSSI Calculation : UL_RSSI(����) - 37.0dB + DTU_UL_Atten


2019.04.25. v67.00.04(PRU, AAU)
- PRU����, HARD Clipping, BIP ���� ����.


2019.04.25. v68.00.04(PRU, AAU)
	1. "CPRI Alm" DL/UL path off, PAU Off ( SFP-0/SFP-1 ��ġ���� �� ����.)
	2. AID 0x5301, CPRI Reason Added.
	3. UL RSSI Calculation : UL_RSSI(����) - 37.0dB - DTU_UL_Atten
	4. SuperCap_AC_Alarm_Report ��, Batt_Insert ���� ���� �߰�.
	5. Hard Clipping Level ���� �߰�.(PRU) : 8.0 ~ 14.0 dB range.
	6. Wavelength AID ����. AID 0x5312.
	7. SFP not insert, PAU not insert : Inventory 0x7F
	8. Battery Extension : SISO/MIMO mode ��� ���� �ϵ��� ����.











	

2019.04.24. v70.00.04(PRU, AAU)
- AAU : DVT ������ release
- YCSo merge : spectrum ��. (package download, USB GUI ...)

 

2019.04.24. v75.00.04(AAU)
- AAU : DVT ������ release

2019.04.24. v79.00.04(PRU, AAU)
- ALC RVS : layer#2,#3,#4 : comment ����.
- AAU is "MIMO Mode" Only		


2019.04.24. v80.00.04(PRU, AAU)
- ALC RVS (Quick)
- Hard Clipping Test

2019.04.23. v83.00.04(PRU, AAU)
- hard clipping added.
- BIP counter added.
- AMP-OFF ��, DTU DL/UL Off (��, PRU �̰�, MIMO-Standby-Mode�� ����)


2019.04.22. v84.00.04(PRU, AAU)- not release
- MIMO-Mode ���ۻ����� ��� : SISO-AMP Off �Ǿ� ������, Batt.Extension ���� ���� �ʵ���.
- SISO-Mode ���ۻ����� ��� : MIMO-Standby ���� ���� �ʵ��� (MIMO-Standby ���ۻ��´� Off.)


2019.04.22. v85.00.04(PRU, AAU)
- AID 0x500B Center freq added.
- AAU : Fwd shutdown added.



2019.04.20. v86.00.04(PRU, AAU)
- "FPGA_RdWr_Sem" added
- AAU, 'Mscl_Status' bit ����.
- Make_Reg_Addr_AAU_Axi ����.




2019.04.20. v87.00.04(PRU, AAU)
- RVS ALC



2019.04.19. v88.00.04(PRU, AAU)
- AAU RVS ALC : Test needed
- if(Reg_ALC_flag[m][r] == 0) continue; // 2019.04.20 

2019.04.19. v91.00.04(PRU, AAU)
AID 0xB431 : Payload On/Off, AID size ����. 4x1 -> 4x2



2019.04.18. v93.00.04(PRU, AAU)
- payload on/off : reset �� �ʱ�ȭ �ϵ��� ����.
- CW On/Off ����/���� �߰�.


2019.04.18. v98.00.04(PRU, AAU)
- layer splitter : 
	1.Delay Set ��� ���Ž� AID �׸� FPGA Delay(DL)-0x5403, FPGA Delay(UL)-0x5404  ����/���� �ݿ�(04/18)
	2.�������� v0.8 ���� �����Ǵ� �˶� �׸� ���� Alarm Noti ����(04/19)
- PSU ���·� AID �߰�.
- cascade ID : AID 0x5001 : data type ����. 0x04 -> 0x02


2019.04.18. v11.00.02(PRU, AAU)
- AID 0x5009 : "System ����" deleted.
- AID 0x5206 : UL_Att added (ANTS PRU Only, register)




2019.04.17. v12.00.02(PRU, AAU)
- package download ��, psu_reset �� �ǵ��� ����. m_Env.psu_reset_req = 1;
- DL/UL Guard time ���� : PRU/AAU Register 11bit -> 12bit ����.

- PRU : 
(1) "FAN ������Mode" : when (start-time == stop-stop), AID 0x5411 : "������ Mode" ǥ��
(2) FAN ������ ��� : PWM  �µ� table ����.
(2) DTU LED �����, GUI, Door�� ���� �ϵ��� ����.
(3) AID 0x5216 : LNA-on/off added. (PAU-RVS-AMP-Onoff �� ����.)

- AAU : 
(1) GUI Main 
	(AID DL/UL Atten : RF ATTEN ����.)
	(AMP-hidden LNA Atten : ���� �߰�.)
(2) AID 0x5216 : LNA-on/off �� ���� AAU-register�� ����.
(3) DL/UL Guard time ���� : PRU�� �޸� ���� ����.
--------------------------------------------------------------------







2019.04.16. v13.00.02(PRU, AAU)
- Batt.Extension, MIMO Standby : MIMO Mode�϶��� �����ϵ��� ����.
- when "Batt.Ext.Mode == OFF" && "AC �˶�" : mode ���� ���� �ʵ��� ����.
- Comm TX ����, size check "MAX_COM_BUFFER" added. (size 4096 -> 4096x3))
- Layer mode AID 0x5433 : AID ����.
- Battery ���� AID	0x0210 (0x07 : ����(50v))
- LS ������ AID 0x500A added.
- main LNA ON OFF ����.
- USB/Bluetooth : tx size 4160 ���� ��, ��� �� �Ǵ� �κ� ����.
- AAU : S/N�� �� �����ϸ� Bluetooth�� re init �Ǹ鼭 S/N�� ���� �̸����� ������ ����.
           B/T�� ��� ���� �� LED�� ���ڰŸ����� ����.

-SC.So Main AID merged.

- below AID : not used AID
LOS(CPRI)	0x5302
LOF(CPRI)	0x5303
LD(CPRI)	0x5304





2019.04.16. v14.00.02(PRU, AAU)
- merge version


2019.04.15. v15.00.02(PRU, AAU)
- AID 0xB437/38 : DTU Volgate (Main/Sub) Added
- FAN ������ ��� : (���۽ð�==����ð�) �׽� ������ ��� On �ǵ��� ����.
- Bluetooth_re_Init (AAU)



2019.04.13. v17.00.02(PRU)
- correct "m_St.d_VSS_Z__60__Rec_PortNum[c][DL]" 
- Clock ��� �˶� : Shutdown �˰���
- CPRI ���	�˶� : Shutdown �˰���
- TSSI �Է� ���� : Shutdown �˰���
- LED ���� added
- Super cap. Ȱ�� AC �˶�����


2019.04.11. v18.00.02(PRU)
- when CPRI-Lock normal : copy "VSS Register TDD Slot number 0~19" to "slot #0 ~19"
- DL Off Guard Time <-> DL to UL Guard Time
- DL On Guard Time <-> UL to DL Guard Time
- AID 0x5420 (TDD_Mode) �� FDD �׸� ����ǵ��� ����.

2019.04.11. v19.00.02(PRU), v20.00.02(AAU) : layer splitter ���� ����.(Nokia ���� �κ� �߰�)

2019.04.10. v20.00.02(AAU, PRU)
- VSS Channel : Copy 
	DL(CPRI0/1) -> DL(CPRI2/3), UL(CPRI2/3) -> UL(CPRI0/1)
- loopback mode
- RE Reset Added : 
	PSU Power Reset �ǵ��� ���� �� DL(CPRI2/3) ���� VSS (RE Reset) ����.


2019.04.10. v22.00.02(AAU, PRU)
- DL/UL gain (0xC015/15) <--> DL/UL atten (0xB420/21) control/status added.

2019.04.10. v23.00.02(AAU, PRU)
- AC Fail
- watchdog time 10 -> 5 sec

2019.04.10. v24.00.02(AAU, PRU)
- AID  : 0x540A(EQ_Coeff), 0x540B(EQ_Coeff) �ٸ� ������ ����.
- RE-Channel BW : user control ���� ������ ����.

2019.04.09. v25.00.02(AAU, PRU)
- cpri lock ��, initial setting �� VSS write �ϵ��� ����.

2019.04.09. v27.00.02(AAU, PRU)
- "RE Channel BW" ���� ��, center freq ���� �ǵ��� ����.

2019.04.08. v28.00.02(AAU, PRU)
- RCU download *.RSM header ����.


2019.04.08. v29.00.02(AAU, PRU)
- "Reg_Common_RW_DTU_LED" added.
- ARFCN calculation is Added. (AID 0x5007), AID 0xB41F deleted.
- register "PAU DL Attn Sync" �з��� ���� �Ǵ� �� ����.
- AID 0x05409 Center freq ����.(���� /����, same with AID 0xB412)
- cpri lock check �κ� ����.
	A_PRU_Status_Update_Mmap();
	A_AAU_Status_Update_Axi();





2019.04.06. v30.00.02(AAU, PRU)
- fpga delay(AID 0x5403/04) fpga address ����, delay offset (AID 0xB415/16) ����.
- AID 0x5106 PAU DL ATTEN ����� Register('PAU_DL_Attn_Sync') control added (ANTS PRU Only)
- inventory inquery added (control will be added later...)

2019.04.06. v36.00.02(AAU, PRU)
- package download ����.
- ftp download added


2019.04.06. v37.00.02(AAU, PRU)
- AID 0x5206 UL ATT : digital �̳� ANTS-PRU �� ���, �Ͻ������� hidden atten AID 0xB224 �� �����ϰ�.
- AID 0xB220 : PRU-PAU : TPL Atten ���� �κ� ����.
- AID 0xB224 : PRU-PAU : PAU RVS Atten ���� : 0.5dB step ���� ����.
- if Batt. not insert, mask-alarm of battery-low(bit6)
- temperature alarm hysteresis : bug fixed.
- AID 0xB419 (d_CPRI_Recovery_Path_St) : "CPRI Recovery Path Status" added
- inventory inquery added (control will be added later...)
- bar-code inquery/control added.

2019.04.06. v38.00.02(AAU, PRU)
- Test Port ��� : 0x543F : F/W restart ��, off �� ���۵ǵ��� ����
- System ���� AID 0x5009 : added default 0:4T4R
- PSU_Reset Added
		1. FPGA Regiseter Control : 'PSU Reset'
		1. system("halt");
		1. for 'halt', RCU termination
		2. 5[s] delay after 'halt', ANTS-PRU-FPGA will do "PSU-RESET"


2019.04.06. v40.00.02(AAU, PRU)
- Battery Extension Mode (On/Off)	0x5438
- Battery Extension Mode ���� ����	0x5439
- pkg download fix when AAU-pakage download
- when AC-Fail, do not send 'PSU_CMD_BattCharge_Ctrl'


2019.04.05. v50.00.02(AAU, PRU)
- package download added
- 

2019.04.04. v59.00.02(AAU, PRU)
- Nokia ���� ���� AAU : layer splitter ����.
- AAU V00.00.03 ���� ������ AAU F/W�� V60.00.02�� merge)

2019.04.04. v60.00.02(AAU, PRU)
- VSS - REFunctionFail : Nokia ���� ����, �Ͻ������� 0xCC �� �����ϵ��� ����.
- MIMO Standby added ( siso layer mode ����)
- AAU : RUN LED added.

2019.04.03. v61.00.02(PRU)
- FPGA Temp calculation added.
- SFP insert change : 
	SFP Insert = fpga register '0' = AID 0xB43F (mount is '0')


2019.04.03. v61.00.02(AAU)
- AID 0x5002 : LayerUseInfo : eeprom init is "use"
- AAU : Ext FAN Algorithm added according to temperature



2019.04.03. v62.00.02(AAU)
- layer ������� �κ� ���� ����: control��, status ������ �ݿ��ϵ��� .. ��.


2019.04.02. v63.00.02
- amp standby : basic operation : on/off according to time
- cpri alarm added.
- cascade ID : cpri2/3 �� 1 ����.
- SFP (CPRI0, CPRI1) insert �� ���� alarm mask �߰�.
- Hard Clipping Count(DL) 0x5320 2 -> 1����

2019.04.02. v64.00.02
2019.04.01. v65.00.02
- ether path selection, VSS-C&M ����


2019.04.01. v66.00.02
- AAU : CRPI Alarm added.
- Vss : Alive ACK (CPRI0/1 UL) added : 0x33/0xCC
- bluetooth LED : Red/Blue : high <-> low, change


2019.04.01. v67.00.02
- AID 0x5106, 0x5206 : Atten 0.1_dB_Step (excel list)
- AID 0x5313 : register read �� �״�� GUI ��. (excel list)
- AID 0xB618 : max RPM ����/���� ����. (excel list)
- mmap : cpri alarm bit ����.
- nokia LS ���� ��� ���� ��.









2019.03.29. v77.00.02
- PRU power-reset added : check needed for io-pin
- AID 0x5106, 0x5206 : dl atten, ul atten, 0.01 -> 0.1 setp
- Fan mode �˰��� �߰�.
- Batt extension added
- PSU FAN RPM(AID 0xB616) ���������� On/Off ���� ��
	RPM '0' = Off, RPM '100' = On ó�� 
	(GUI���� On �� '100' ������ ����, Off�� '0'������ ����, Display ����)
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
- LS response ����.
1.	System Status Response(H'1000 0011) Return Result �ʵ��� ���� "0x11(17)"���� Ȯ�ε˴ϴ�.
�ϴ��� default�� "0x00 Normal Case" �÷��ּ���.

2.	CPRI Status Response(H'1000 001D)
 	�ش� ���������� Element�߿� "Optic Delay IE(H'0034)"�� �����ϵ��� ����.


2019.03.26. v84.00.02
- OSM ��ġ ����	0x0249 Added.
- Channel Bandwidth   0x5008
- DL ���(����) 0x5108


2019.03.25. v85.00.02
	- PRU-PAU H/W insert ó�� �߰�.: not insert ��, layer �̻��.(AID 0x5002)
	 H/W insert ��, user control�� ����, ǥ��
	 layer �̻��.(AID 0x5002) ��, alarm mask

	 PAU link fail added.

2019.03.25. v87.00.02
	- Layersplitter ���� : port �� ���� ó�� �߰�.

2019.03.22. v90.00.02
	- GUI : udp comm, GUI port�� �ٲܶ�, 20000 �̿ܿ��� �����ϵ��� ����.

2019.03.21. v91.00.02
	- AAU merge
	- clock fail added.
	- AAU : center freq, DL/UL Gain added as DL ATT, UL ATT(table)
	
	alarm ����.
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
	- register : Rx/Cascade Add On/Off(UL) ���� ����
		[7] : Alpha 3 Path
		[6] : Alpha 2 Path
		[5] : Alpha 1 Path
		[4] : Alpha 0 Path
		[3] : Main 3 Path
		[2] : Main 2 Path
		[1] : Main 1 Path
		[0] : Main 0 Path

	- PRU-PAU table : GUI display ����.
	- PSU : alarm, status ��� ���� �� �߰�.


2019.03.19. v97.00.02 ( Release is V00.00.02)
	- AID 0xB41E : ReChannel BW : DL/UL -> 100M/Alpha
	
2019.03.18. v99.00.02 ( Release is V00.00.02)
	- AID Hidden 0xB435 added
	( Rx/Cascade Add On/Off(UL) : [0] : RX Cascade On/Off)
	- cfr on/off, dpd on/off �ݴ������ �Ǵ� �� ����.

2019.03.18. v18.06.00
	- Select ��� ����.
	- localhost �߰�.
	

2019.03.15. v15.01.xx
- TSSI, RSSI ���� ���� �˶� �߰�.
- AID �߰� 

- PCI 0x5003
- Cell ID 0x5004
- gNB ID 0x5005
- Port No 0x5006
- FAN Mode 0x5410
- FAN Mode ���� ���� 0x5411
- FAN Mode ���� �ð� 0x5412


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
