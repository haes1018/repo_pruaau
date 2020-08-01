/********************************************************************************/
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__
/********************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <semaphore.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <termios.h>
#include <signal.h>

#include <stdarg.h>
#include <syslog.h>
#include <math.h>


#include "os_util.h"
#include "sys_definition.h"
#include "protocol.h"
#include "def.h"
#include "alarm.h"


#include "cli.h"
#include "spi.h"
#include "frame.h"
#include "crc.h"
#include "comm_conf.h"
#include "comm_lib.h"
#include "up_com.h"
#include "PAU_comm.h"
#include "psu_comm.h"

#include "FPGA_Reg.h"
#include "inventory.h"
#include "iro_proc.h"
#include "rems.h"
#include "uda_proc.h"
#include "ul_proc.h"
#include "util.h"
#include "nms_alarm.h"

#include "Debug_Lib.h"
#include "comm.h"
#include "devicelib.h"
#include "BT_Comm.h"
#include "control.h"
#include "down_load.h"
#include "down_load_zip.h"

#include "eep_emul.h"
#include "udp_comm.h"
#include "dtu_udp_comm.h"
#include "SIM.h"
#include "log_file.h"
#include "log_reset.h"
#include "log_event.h"
#include "i2c_dev.h"

#include "lsh_udp_comm.h"
#include "lo_host_comm.h"
#include "auto_cofig_proc.h"
#include "a_pru_mmap_proc.h"
#include "temp_ic.h"
#include "algorithm_rf.h"
#include "ADC_ic.h" // 2019.04.15
#include "spectrum.h"	///// YCSO added 190420
#include "uda_proc.h"
#include "pru_dpd.h"
/********************************************************************************/
#endif
/********************************************************************************/
