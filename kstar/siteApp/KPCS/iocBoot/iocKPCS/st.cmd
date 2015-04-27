#!../../bin/linux-x86_64/KPCS

## You may have to change KPCS to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/KPCS.dbd"
KPCS_registerRecordDeviceDriver pdbbase

createMainTask("KPCS")

## DevType,   Task(Dev)Name,  arg0,   arg1,  arg2 
createDevice("RTcoreDev",  "RTCORE",  "xxx")
createDevice("DNSTdev",  "DCal",  "Dev5")


## Load record instances
dbLoadRecords "db/dbSFW.db", "SYS=KPCS"
dbLoadRecords "db/dbRTCORE.db", "SYS=KPCS, DEV=RTCORE"
dbLoadRecords "db/dbDensityCal.db", "SYS=KPCS, DEV=DCal"


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"

# epicsThreadSetCPUAffinityByName RMCHK_DAQ 5
# epicsThreadSetPosixPriorityByName RMCHK_DAQ 91 SCHED_FIFO
# epicsThreadShowAll 1

