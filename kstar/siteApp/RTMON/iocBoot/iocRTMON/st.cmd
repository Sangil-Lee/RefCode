#!../../bin/linux-x86_64/RTMON

## You may have to change RTMON to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/RTMON.dbd"
RTMON_registerRecordDeviceDriver pdbbase

createMainTask("RTMON")

## DevType,   Task(Dev)Name,  arg0,   arg1,  arg2 
createDevice("RTcoreDev",  "RTCORE",  "xxx")
createDevice("RFMdev",  "RMCHK",  "Dev5")


## Load record instances
dbLoadRecords "db/dbSFW.db", "SYS=RTMON"
dbLoadRecords "db/dbRTCORE.db", "SYS=RTMON, DEV=RTCORE"
dbLoadRecords "db/dbRTMON.db", "SYS=RTMON, DEV=RMCHK"
dbLoadRecords "db/dbKstarInfo.db"
dbLoadTemplate("db/dbRfmOffsetScan.template")
dbLoadTemplate("db/dbManualOffset.template")
dbLoadTemplate("db/dbRFM_TAG.template")

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"

#####################################
# IRMIS crawler [woong, 2013.06.10]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################


# PCS RT_MODE offset: 0x110
#dbpf RTMON_OFFSET3 110
dbpf RTMON_RT_CLOCK_RATE 10000
dbpf RTMON_SAMPLING_RATE 5000
dbpf RTMON_START_T0_SEC1 -15
dbpf RTMON_RUN_PERIOD  40

dbpf RTMON_OFFSET_ID0 0x3800008
dbpf RTMON_OFFSET_ID1 0x3800010
dbpf RTMON_OFFSET_ID2 0x3810000
dbpf RTMON_OFFSET_ID3 0x3810004


# epicsThreadSetCPUAffinityByName RMCHK_DAQ 5
# epicsThreadSetPosixPriorityByName RMCHK_DAQ 92 SCHED_FIFO
# epicsThreadShowAll 1

