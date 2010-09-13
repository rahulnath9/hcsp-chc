#!/bin/bash

CFG_FILE="CHC_LAN4.cfg"
MAKE_CFG="LAN4C"
DATA_DIR="../../ProblemInstances/HCSP/1024x32.mod/"

data[0]="A.u_c_hihi"
data[1]="A.u_c_hilo"
data[2]="A.u_c_lohi"
data[3]="A.u_c_lolo"
data[4]="A.u_i_hihi"
data[5]="A.u_i_hilo"
data[6]="A.u_i_lohi"
data[7]="A.u_i_lolo"
data[8]="A.u_s_hihi"
data[9]="A.u_s_hilo"
data[10]="A.u_s_lohi"
data[11]="A.u_s_lolo"
data[12]="B.u_c_hihi"
data[13]="B.u_c_hilo"
data[14]="B.u_c_lohi"
data[15]="B.u_c_lolo"
data[16]="B.u_i_hihi"
data[17]="B.u_i_hilo"
data[18]="B.u_i_lohi"
data[19]="B.u_i_lolo"
data[20]="B.u_s_hihi"
data[21]="B.u_s_hilo"
data[22]="B.u_s_lohi"
data[23]="B.u_s_lolo"

for i in {0..0}
do
	echo $CFG_FILE > Config.cfg
	echo "$DATA_DIR${data[i]}" >> Config.cfg
	echo "res/$MAKE_CFG.sol.txt" >> Config.cfg
	time(make $MAKE_CFG > res/A.u_c_hihi_$MAKE_CFG.log)        
done
