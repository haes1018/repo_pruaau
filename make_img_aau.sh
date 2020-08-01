#!/bin/sh

tar -zcvf RCU_DN.tar DN_File
#./MakeSKT_File ./RCU_DN.tar ./header.txt ./src/ver_info.h ANTS_AAU RSM
./MakeSKT_File ./RCU_DN.tar ./header.txt ./src/ver_info.h ANAAU RSM

# EOF
