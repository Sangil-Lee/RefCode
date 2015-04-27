#!../../bin/linux-x86/RBA

epicsEnvSet(CLTU_IOC_DEBUG, "0")

## You may have to change RBA to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/RBA.dbd"
RBA_registerRecordDeviceDriver pdbbase

createTaskCLTU("RBA", "/dev/tss/cltu.0", "0", "0", "123")

## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
dbLoadRecords "db/RBA.db", "user=RBA"
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=RBA"
dbLoadRecords("db/dbSysMon.db","SYS=RBA")


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

#########################
# IRMIS crawler [shbaek, 2010.02.12]
#########################
# dbl > pvlist

# cd ${TOP}/pvcrawler
# system("./cmd.exe")
# cd ${TOP}/iocBoot/${IOC}
#########################


## Start any sequence programs
#seq sncExample, "user=RBA"


