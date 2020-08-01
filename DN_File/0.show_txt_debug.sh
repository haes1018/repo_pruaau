#!/bin/sh
echo "show text debug data"
is_valid=0
port_val=0
WDIR=/usr/app/sys

if [ $# = 1 ]
then
        if [ $1 -ge 0 -a $1 -le 6 ]
        then
                is_valid=1
        else
                is_valid=0
        fi

        if [ $1 = 15 ]
        then
                is_valid=1
        fi

fi


if [ $is_valid = 0 ]
then
        echo ""
        echo "---- selection ----"
        echo ""
        echo " 0 : ALL"
        echo " 1 : PSU"
        echo " 2 : L/S"
        echo " 3 : Standby"
        echo " 4 : UDA"
		echo " 5 : DPD"        
		echo " 6 : REMS"
		echo " 15 : etc"
        
        echo ""
else
        echo "init txt data log"
        trap "echo; echo \"hide txt data\"; ${WDIR}/kstd txt_0000; exit 1 " INT
        ${WDIR}/kstd txt_0000

        sleep 1

        case $1 in
        0)
                port_val=FFFF;
                echo "0: ALL";;

        1)
                port_val=0002;
                echo "1: PSU";;
        2)
                port_val=0004;
                echo "2 : L/S C&M";;
        3)
                port_val=0008;
                echo "3 : Standby(AMP,MIMO)";;
        4)
                port_val=0010;
                echo "4 : UDA";;
		5)
                port_val=0020;
                echo "5 : DPD";;
		6)
                port_val=0040;
                echo "6 : REMS";;                
        15)
                port_val=8000;
                echo "15 : etc";;
        *)
                is_valid = 0;
                echo "???";;
        esac
fi

if [ $is_valid = 1 ]
then
        ${WDIR}/kstd txt_$port_val
        sleep 1
        tail -n 500 -f /tmp/kst_log.txt
fi
#EOF
