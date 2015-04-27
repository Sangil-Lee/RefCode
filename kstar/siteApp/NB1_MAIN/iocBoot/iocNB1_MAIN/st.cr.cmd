#!../../bin/linux-x86/NBI_MAIN

## You may have to change NBI_MAIN to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet(CLTU_IOC_DEBUG, "0")

#cd ../../
cd ${TOP}/


## Register all support components
dbLoadDatabase("dbd/NBI_MAIN.dbd")
NBI_MAIN_registerRecordDeviceDriver(pdbbase)

EIP_buffer_limit(50)
drvEtherIP_init()
EIP_verbosity(5)

####################################################################
##  Power Supply PLC
####################################################################
drvEtherIP_define_PLC("plc1", "172.17.101.225", 0)
dbLoadTemplate("db/NB1_PS_AI_G1.template")
dbLoadTemplate("db/NB1_PS_AI_G3.template")
dbLoadTemplate("db/NB1_PS_AI_ARC.template")
dbLoadTemplate("db/NB1_PS_AI_BIAS.template")
dbLoadTemplate("db/NB1_PS_AI_FILAM.template")
dbLoadTemplate("db/NB1_PS_AI_VME.template")

dbLoadTemplate("db/NB1_PS_AO_G1.template")
dbLoadTemplate("db/NB1_PS_AO_G3.template")
dbLoadTemplate("db/NB1_PS_AO_ARC.template")
dbLoadTemplate("db/NB1_PS_AO_BIAS.template")
dbLoadTemplate("db/NB1_PS_AO_FILAM.template")
dbLoadTemplate("db/NB1_PS_AO_VME.template")

dbLoadTemplate("db/NB1_PS_BI_G1.template")
dbLoadTemplate("db/NB1_PS_BI_G3.template")
dbLoadTemplate("db/NB1_PS_BI_ARC.template")
dbLoadTemplate("db/NB1_PS_BI_BIAS.template")
dbLoadTemplate("db/NB1_PS_BI_FILAM.template")
dbLoadTemplate("db/NB1_PS_BI_ETC.template")
dbLoadTemplate("db/NB1_PS_BI_VME.template")

dbLoadTemplate("db/NB1_PS_BO_G1.template")
dbLoadTemplate("db/NB1_PS_BO_G3.template")
dbLoadTemplate("db/NB1_PS_BO_ARC.template")
dbLoadTemplate("db/NB1_PS_BO_BIAS.template")
dbLoadTemplate("db/NB1_PS_BO_FILAM.template")
dbLoadTemplate("db/NB1_PS_BO_VME.template")

dbLoadTemplate("db/NB1_PS_AO_CALC.template")

dbLoadRecords("db/NB1_PS_BI_CALC.db")
dbLoadRecords("db/NB1_PS_AI_CALC.db")

dbLoadRecords("db/NB1_KST_INFO.db")

dbLoadRecords("db/NB1_LTU_ALL_SHOT.db")
#dbLoadRecords("db/dbSysMon.db","SYS=NBIPS")

## Power Supply To Operation Mode
dbLoadTemplate("db/NB1_OPR_MODE_BO.template")

## NBI-1 MAIN HeartBeat
dbLoadRecords("db/NB1_MAIN_HeartBeat.db")

drvAsynModbusConfigure("L0", "172.17.101.78", 0, 1)

###############################
#  load LTU  2011.5.13 woong
createTaskCLTU("NBI1A", "/dev/tss/cltu.0", "1", "0", "105")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=NBI1A"
dbLoadTemplate "db/NBI1A_CLTU_CFG.template"


## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"

#########################
# IRMIS crawler [shbaek, 2010.11.01]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
