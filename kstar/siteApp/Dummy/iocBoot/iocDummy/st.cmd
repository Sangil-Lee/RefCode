#!../../bin/linux-x86/Dummy

## You may have to change Dummy to something else
## everywhere it appears in this file

< envPaths

< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/Dummy.dbd"
Dummy_registerRecordDeviceDriver pdbbase

createMainTask("Dummy")

initControl

## Load record instances
dbLoadRecords "db/dbSFW.db", "SYS=Dummy"

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
