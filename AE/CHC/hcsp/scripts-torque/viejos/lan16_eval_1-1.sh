#!/bin/bash

# Nombre del trabajo
#PBS -N ae_lan16_eval1-1

# Requerimientos
#PBS -l nodes=16,walltime=20:00:00

# Cola
#PBS -q publica

# Working dir
#PBS -d /home/siturria/AE/trunk/AE/CHC/hcsp/ejecuciones/

# Correo electronico
#PBS -M siturria@fing.edu.uy

# Email
#PBS -m abe
# n: no mail will be sent.
# a: mail is sent when the job is aborted by the batch system.
# b: mail is sent when the job begins execution.
# e: mail is sent when the job terminates.

# Output path
#PBS -e /home/siturria/AE/trunk/AE/CHC/hcsp/ejecuciones/evaluacion/lan16-1/
#PBS -o /home/siturria/AE/trunk/AE/CHC/hcsp/ejecuciones/evaluacion/lan16-1/

#PBS -V

echo Job Name: $PBS_JOBNAME
echo Working directory: $PBS_O_WORKDIR
echo Queue: $PBS_QUEUE
echo Cantidad de tasks: $PBS_TASKNUM
echo Home: $PBS_O_HOME
echo Puerto del MOM: $PBS_MOMPORT
echo Nombre del usuario: $PBS_O_LOGNAME
echo Idioma: $PBS_O_LANG
echo Cookie: $PBS_JOBCOOKIE
echo Offset de numero de nodos: $PBS_NODENUM
echo Shell: $PBS_O_SHELL
#echo JobID: $PBS_O_JOBID
echo Host: $PBS_O_HOST
echo Cola de ejecucion: $PBS_QUEUE
echo Archivo de nodos: $PBS_NODEFILE
echo Path: $PBS_O_PATH

echo
cd $PBS_O_WORKDIR
echo Current path: 
pwd
echo
echo Nodos:
cat $PBS_NODEFILE
echo
# Define number of processors
echo Cantidad de nodos:
NPROCS=`wc -l < $PBS_NODEFILE`
echo $NPROCS
echo

EXEC="/home/siturria/bin/mpich2-1.2.1p1/bin/mpiexec.hydra -rmk pbs /home/siturria/AE/trunk/AE/CHC/hcsp/MainLan"
#EXEC="mpiexec -mpich-p4-no-shmem ../MainLan"

data[0]="u_c_hihi.0"
data[1]="u_c_hilo.0"
data[2]="u_c_lohi.0"
data[3]="u_c_lolo.0"
data[4]="u_s_hihi.0"
data[5]="u_s_hilo.0"
data[6]="u_s_lohi.0"
data[7]="u_s_lolo.0"
data[8]="u_i_hihi.0"
data[9]="u_i_hilo.0"
data[10]="u_i_lohi.0"
data[11]="u_i_lolo.0"

for i in {0..0}
do
	CfgFile="/home/siturria/AE/trunk/AE/CHC/hcsp/ejecuciones/scripts_evaluacion/chc_lan16-1.cfg"
	DataFile="/home/siturria/AE/trunk/AE/ProblemInstances/HCSP/Braun_et_al.CPrio/${data[i]}"
	OutputFile="/home/siturria/AE/trunk/AE/CHC/hcsp/ejecuciones/evaluacion/lan16-1/Braun_et_al/${data[i]}"
	PesosFile="/home/siturria/AE/trunk/AE/CHC/hcsp/ejecuciones/pesos_fijos.txt"
	
	echo "Datos $DataFile"
	echo "CfgFile $CfgFile"
	cat $CfgFile
		
	echo "${CfgFile}" > Config_LAN16_eval1-1.cfg
	echo "${DataFile}" >> Config_LAN16_eval1-1.cfg
	echo "${OutputFile}.sol" >> Config_LAN16_eval1-1.cfg
	echo "${PesosFile}" >> Config_LAN16_eval1-1.cfg
	
	time($EXEC Config_LAN16_eval1-1.cfg > $OutputFile.log) 
done
