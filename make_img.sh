#!/bin/sh

tar -zcvf RCU_DN.tar DN_File

case "$1" in
    ANTS)
        case "$2" in
		    PRU)
		 		./MakeSKT_File ./RCU_DN.tar ./header_PRU.txt ./src/ver_info_ants_pru.h ANPRU RSM #ANTS_PRU RSM
		 		cp ./*.RSM ../RSM
		 		rm ./*.RSM
		        ;;		   

		    AAU)
				./MakeSKT_File ./RCU_DN.tar ./header_AAU.txt ./src/ver_info_ants_aau.h ANAAU RSM #ANTS_AAU RSM
				cp ./*.RSM ../RSM
				rm ./*.RSM
		        ;;
		    *)
		    	echo "make_img.sh ANTS fail, neither PRU nor AAU"
		        exit 1
		        ;;
		esac
        ;;
    HFR)
    	case "$2" in
		    PRU)
		 		./MakeSKT_File ./RCU_DN.tar ./header.txt ./src/ver_info_hfr_pru.h HFR_PRU RSM       
		        ;;
		    AAU)
				./MakeSKT_File ./RCU_DN.tar ./header.txt ./src/ver_info_hfr_aau.h HFR_AAU RSM        
		        ;;
		    *)
		    	echo "make_img.sh HFR fail, neither PRU nor AAU"
		        exit 1
		        ;;
		esac
        exit 0
        ;;
    *)
    	echo "make_img.sh fail, neither ANTS nor HFR, neither PRU nor AAU"
        exit 1
        ;;
esac






# EOF
