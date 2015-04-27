#!../../bin/win32-x86/VSS

## You may have to change VSS to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.100.107 172.17.102.58")

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/VSS.dbd"
VSS_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=xornHost"
#dbLoadRecords("db/dbSysMon.db","SYS=VSS")
dbLoadRecords "db/dbSubNet.db","SYS=VSS"

dbLoadRecords "db/VSS_BO.db"
dbLoadRecords "db/VSS_AI.db"
dbLoadRecords "db/VSS_STRING.db"

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

#########################
# IRMIS crawler 
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("pvcrawler.bat")
cd ${TOP}/iocBoot/${IOC}

## Start any sequence programs
#seq sncExample, "user=xornHost"
seq sncSIFConvert
seq sncSPEConvert
