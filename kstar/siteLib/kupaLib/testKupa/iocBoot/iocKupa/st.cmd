#!../../bin/linux-x86/Kupa

## You may have to change Kupa to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/Kupa.dbd"
Kupa_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadTemplate "db/userHost.substitutions"
dbLoadRecords "db/dbSubExample.db", "user=rootHost"

## Set this to see messages from mySub
#var mySubDebug 1

# ------------------------------------------------
# KSTAR PV Archive
# ------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
< kupaEnv

## Run this to trace the stages of iocInit
#traceIocInit

# ------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
iocInit

# ------------------------------------------------
# There is problem. (ex) PV.VAL
# ------------------------------------------------
#epicsThreadSleep 2.0
#kupaRunManager

## Start any sequence programs
seq sncExample, "user=rootHost"
