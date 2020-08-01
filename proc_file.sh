#!/bin/sh
case "$1" in
    ANTS)
        case "$2" in
		    PRU)
				var=$(find ./DN_File/AAU  -iname "equ*" | wc -l)
				if [ $var -gt 0 ]; then
					echo "$var"
					rm ./DN_File/AAU/*	
				fi
		        ;;
		    AAU)				
				cp ./AAU/* ./DN_File/AAU
		        ;;
		    *)
		        exit 1
		        ;;
		esac
        ;;
    *)
        exit 1
        ;;
esac
# EOF
