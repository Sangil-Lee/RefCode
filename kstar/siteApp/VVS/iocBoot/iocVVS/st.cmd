#!../../bin/linux-x86/VVS

## You may have to change VVS to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","6553600" )
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.28 172.17.101.20 172.17.101.21")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")

< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/VVS.dbd"
VVS_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0 

## logging debut message to file
setLogLevel 0 
setLogFile("LOG_VVS.log")

createMainTask("VVS")

< initDriverNI_1.cmd

cd ${TOP}

epicsThreadSleep(1.)
## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=rootHost"

dbLoadTemplate("db/NIDAQ_READ_CH_DAQ.template")
dbLoadRecords("db/NIDAQ.db","SYS=VVS, DEV=NI6289")
dbLoadRecords("db/dbSFW.db", "SYS=VVS")
dbLoadRecords("db/VVS_timeStamp.db")
dbLoadRecords("db/dbSysMon.db","SYS=VVS")

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

#########################
# IRMIS crawler [shbaek, 2010.01.07]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

cd ${TOP}/bin/linux-x86
system("./caSysProc &")
cd ${TOP}/iocBoot/${IOC}

## Start any sequence programs
#seq sncExample, "user=rootHost"
seq sncVVSSeq
