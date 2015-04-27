#!../../bin/linux-x86/nbi

## You may have to change nbi to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/nbi.dbd")
nbi_registerRecordDeviceDriver(pdbbase)

####################################################################
## RGA
####################################################################
var drvGenericSerialDebug 10
drvGenericSerialConfigure("L11","172.17.121.52:4002",0,0)

dbLoadRecords ("db/NB1_LOCAL_RGA_FL.db", "SYS=NB1_LOCAL, LINK=L11")
dbLoadRecords ("db/NB1_LOCAL_RGA_INIT.db", "SYS=NB1_LOCAL, LINK=L11")
dbLoadTemplate("db/NB1_LOCAL_RGA_READ.template")

cd ${TOP}/iocBoot/${IOC}
iocInit()

####################################################################
## RGA
####################################################################
epicsThreadSleep 2.

seq SRSRGA_ON
####################################################################

