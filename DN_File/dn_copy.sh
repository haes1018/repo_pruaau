#!/bin/sh

##############################################################

SYS_DIR="sys"

if [ $# = 1 ]; then
    ACTIVE_ID=$1
    if [ "x$ACTIVE_ID" = "x1" ]; then
        SYS_DIR="sys_1"
    elif [ "x$ACTIVE_ID" = "x2" ]; then
        SYS_DIR="sys_2"
    fi
fi

echo "--- running 2020-dn_copy.sh"
##############################################################

MODEL=$(cat /Model)

if [ "x$MODEL" = "xANTS_PRU_0" ]; then    
    echo "--- PRU ---"
else
	echo "--- AAU ---"
	cp -f /tmp/DN_File/AAU/* /usr/app/sys.conf
fi




cp -f /tmp/DN_File/RCU_App_DN /usr/app/${SYS_DIR}/RCU_App_DN
cp -f /tmp/DN_File/RCU_App_DN.chksum /usr/app/${SYS_DIR}/RCU_App_DN.chksum

cp -f /tmp/DN_File/Wait_Dn_File_chk.sh /usr/app/${SYS_DIR}/Wait_Dn_File_chk.sh

cp -f /tmp/DN_File/0.show_txt_debug.sh /usr/app/${SYS_DIR}/0.show_txt_debug.sh
cp -f /tmp/DN_File/1.show_comm_debug.sh /usr/app/${SYS_DIR}/1.show_comm_debug.sh
#####################
if [ -f /usr/app/${SYS_DIR}/core ]; then
	rm -r /usr/app/${SYS_DIR}/core
fi	
#####################
if [ -f BOOT.bin ]; then
    mv ./BOOT.bin ./BOOT.BIN
    sync
fi

if [ -f boot.bin ]; then
   mv ./boot.bin ./BOOT.BIN
   sync
fi
#####################
echo `date`
chmod +x /tmp/DN_File/install_boot_n_image.sh
sleep 1
/tmp/DN_File/install_boot_n_image.sh
echo `date`
#####################


exit 0

# EOF
