#!../../bin/linux-x86/TXPC

## You may have to change TXPC to something else
## everywhere it appears in this file

< envPaths
< sfwEnv


cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/TXPC.dbd"
TXPC_registerRecordDeviceDriver pdbbase

createMainTask("TXPC")
## DevType,   TaskName,  arg0,   arg1,  arg2
createDevice("Dummy", "DUMMY")


## Load record instances
#dbLoadRecords "db/dbSysMon.db", "SYS=TXPC"
dbLoadRecords "db/dbSFW.db", "SYS=TXPC"
#dbLoadRecords "db/sysTimeStamp.db", "SYS=TXPC"


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit



dbpf TXPC_OP_MODE 1

## Start any sequence programs
#seq sncExample, "user=rootHost"
