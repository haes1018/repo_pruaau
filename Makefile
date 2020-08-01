#!/usr/bin/make


#-----------------------------
# Select Maker : 'ANTS', 'HFR'
#-----------------------------
MAKER = ANTS

#-----------------------------
# Select Type : 'PRU' or 'AAU'
#-----------------------------
PRUAAU = AAU

#-----------------------------
# Select DBG : 'dbg' or 'none'
#-----------------------------
DBG = none

#-----------------------------
# used when zynq 
#-----------------------------
ifeq ($(PRUAAU),PRU)
CROSS_COMPILE=aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
endif

#-----------------------------
# used when Ti33XX
#-----------------------------
ifeq ($(PRUAAU),AAU)
AM3352_CC = arm-poky-linux-gnueabi-
CC = $(AM3352_CC)GCC
LD = $(AM3352_CC)ld
STRIP = $(AM3352_CC)strip
endif

#-----------------------------
# used when zynq  
#-----------------------------
ifeq ($(MAKER),HFR)
CROSS_COMPILE=aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
endif


###################################

RM = /bin/rm -f

.SUFFIXES : .c .o .h .d

###################################
## add flags here





#-----------------------------
ifeq ($(DBG),dbg)
CFLAGS = -Wall -g  # '-g' is When making core
#####else ifeq ($(DBG),none)	
else
CFLAGS = -Wall
endif
#-----------------------------

## add library here

STDLIBS = -lpthread -lm # '-lm' is for math library

###################################
## target Application rename here

TARGETS = RCU_App_DN
EXESRCS =	main.c \
			cli.c \
			up_com.c \
			comm.c \
			comm_conf.c \
			comm_lib.c \
			Debug_Lib.c \
			frame.c \
			crc.c \
			util.c \
			os_util.c \
			devicelib.c \
			BT_Comm.c \
			down_load.c \
			eep_emul.c \
			udp_comm.c \
			lo_host_comm.c \
			lsh_udp_comm.c \
			auto_cofig_proc.c \
			a_pru_mmap_proc.c \
			PAU_comm.c \
			psu_comm.c \
			alarm.c \
			algorithm_rf.c \
			protocol.c \
			control.c \
			spi.c \
			FPGA_Reg.c \
			temp_ic.c \
			dtu_udp_comm.c \
			inventory.c \
			nms_alarm.c \
			log_file.c \
			i2c_dev.c \
			log_reset.c \
			log_event.c \
			SIM.c \
			ADC_ic.c \
			uda_proc.c \
			pru_dpd.c \
			rems.c \
			spectrum.c
################################# form main.c ~ to inventory.c

################### don't touch below this line #############################


LDFLAGS = -Wl,-Map=${TARGETS}.map


vpath %.c src

OBJPATH=./obj

LIBSRC=
LIBOBJS = ${LIBSRC:.c=.o}

OBJECTS = $(patsubst %.c, $(OBJPATH)/%.o, $(EXESRCS))

DEPENDENCIES = $(patsubst %.c, $(OBJPATH)/%.d, $(EXESRCS))

all: ${OBJPATH} ${LIBOBJS} ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJECTS} ${LIBOBJS} -o ${TARGETS} ${STDLIBS}
#	${CC} ${CFLAGS} ${LDFLAGS} ${OBJECTS} ${LIBOBJS} -o ${TARGETS} $(BUILDAGENTLIBS) -lrt -lpthread
#	cp ${TARGETS} ${TARGETS}.symbol


#--------------------------------------
# Before ${STRIP}, do below
ifeq ($(PRUAAU),PRU)
	${CROSS_COMPILE}nm ${TARGETS} | sort > ${TARGETS}.nm
	@echo '---------------'
	@echo '[PRU] nm and sort'
	@echo '---------------'	
else ifeq ($(PRUAAU),AAU)
	${AM3352_CC}nm ${TARGETS} | sort > ${TARGETS}.nm
	@echo '---------------'
	@echo '[AAU] nm and sort'
	@echo '---------------'	
else
	@echo '---------------'
	@echo '[AAU nor PRU] !!'
	@echo '---------------'
endif
#--------------------------------------




#--------------------------------------
ifeq ($(DBG),dbg)
	@echo '---------------'
	@echo 'do not strip symbol and use -g '
	@echo '---------------'
####else ifeq ($(DBG),none)	
else
	@echo '---------------'
	@echo 'strip symbol file'
	@echo '---------------'
	${STRIP} ${TARGETS}
#	sudo chown root ${TARGETS} && sudo chmod u+s ${TARGETS}	
endif
#--------------------------------------




ifneq ($(CC),gcc)
#	tar -cvf IRO_DN.tar dn_copy.sh ${TARGETS}
#	./MakeDN_Img ./RCU_DN.tar ./header.txt ./src/ver_info.h
	sha1sum ${TARGETS} > ./DN_File/${TARGETS}.chksum
	./proc_file.sh $(MAKER) $(PRUAAU)
	cp ${TARGETS} ./DN_File

# compress
	./make_img.sh $(MAKER) $(PRUAAU)	
endif

ifeq ($(PRUAAU),PRU)
	cp ./${TARGETS} /tftpboot
endif




.PHONY : clean Info
clean :
	${RM} ${OBJECTS} ${DEPENDENCIES} ${LIBOBJS} ${TARGETS} 
#	${RM} ${OBJECTS} ${DEPENDENCIES} ${LIBOBJS} ${TARGETS} core tags

$(OBJPATH)/%.d: %.c
	test -d $(OBJPATH) || mkdir $(OBJPATH) ; \
	rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)/%.o : %.c $(OBJPATH)/%.d
	$(CC) $(CFLAGS)  -c $< -o $@

Info:
	echo Default goal is $(.DEFAULT_GOAL)
	echo include DIR is $(.INCLUDE_DIRS)

-include $(DEPENDENCIES)

# EOF







