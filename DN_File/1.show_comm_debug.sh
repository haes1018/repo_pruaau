#!/bin/sh

is_valid=0
port_val=0
WDIR=/usr/app/sys

if [ $# = 1 ]
then
	if [ $1 -ge 0 -a $1 -le 9 ]
	then
		is_valid=1
	fi
fi

if [ $is_valid = 0 ]
then 
	echo ""
	echo "---- selection ----"
	echo " 0 : Term"
	echo " 1 : UDP Node"
	echo " 2 : localhost dtu"
	echo " 3 : L/S C&M"
	echo " 4 : PAU-#1 or B/T"
	echo " 5 : PAU-#2"
	echo " 6 : PAU-#3"
	echo " 7 : PAU-#4"
	echo " 8 : PSU"	
	echo " 9 : gREMS TX"	
	echo ""
else

	trap "echo; echo \"hide comm data\"; ${WDIR}/kstd com_0000; exit 1 " INT

	echo "init comm data log"

	${WDIR}/kstd com_0000

	sleep 1

	case $1 in
	0)
		port_val=0001;
		echo "0: Trem";;
	1)
		port_val=0002;
		echo "1: UDP Node";;
	2)
		port_val=0004;
		echo "2 : localhost";;
	3)
		port_val=0008;
		echo "3 : L/S C&M";;
	4)
		port_val=0010;
		echo "4 : PAU-#1 or B/T";;
	5)
		port_val=0020;
		echo "5 : PAU-#2";;
	6)
		port_val=0040;
		echo "6 : PAU-#3";;
	7)
		port_val=0080;
		echo "7 : PAU-#4";;
	8)
		port_val=0100;
		echo "8 : PSU";;	
	9)
		port_val=0200;
		echo "9 : gREMS";;	
	10)
		port_val=0400;
		echo "10 : ";;
	11)
		port_val=0800;
		echo "11 : ";;	
	
	*)
		is_valid = 0;
		echo "???";;
	esac
#------------------------------------------
fi

if [ $is_valid = 1 ]
then 
	if [ $1 -le 9 ]
	then
		rm -f /tmp/kst_comm_log_port0$1.txt
	else
		rm -f /tmp/kst_comm_log_port$1.txt
	fi

	

	${WDIR}/kstd com_$port_val

	echo "show comm data udp:"
#	echo $1 "--->" $port_val

	sleep 1

	
	if [ $1 -le 9 ]
	then
		tail -f /tmp/kst_comm_log_port0$1.txt
	else
		tail -f /tmp/kst_comm_log_port$1.txt
	fi

	${WDIR}/kstd com_0000
fi

#EOF
