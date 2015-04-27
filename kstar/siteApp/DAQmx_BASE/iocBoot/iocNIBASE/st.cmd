#!../../bin/linux-x86/NIBASE

## You may have to change NIBASE to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet(EPICS_CA_ADDR_LIST,"172.17.101.200 172.17.101.201 172.17.102.175")
epicsEnvSet(EPICS_CA_ADDR_LIST,"172.17.101.200 172.17.101.201 172.17.101.210 172.17.100.107 127.0.0.1")
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST,"no")

epicsEnvSet(EPICS_CA_MAX_ARRAY_BYTES,"6553600")

< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/NIBASE.dbd"
NIBASE_registerRecordDeviceDriver pdbbase

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}

setPrintLevel 1

## logging debut message to file
setLogLevel 1
setLogFile("LOG_FUELDAQ.log")

createMainTask("FUEL_DAQ")

< initDriverNI_1.cmd
< initDriverNI_2.cmd

cd ${TOP}

epicsThreadSleep(1.)


## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=rootHost"

dbLoadTemplate("db/NIDAQ_READ_CH_DAQ.template")
dbLoadTemplate("db/NIDAQ_READ_CH_DAQ2.template")
dbLoadTemplate("db/NIDAQ_WRITE_CH_DAQ.template")
dbLoadTemplate("db/NIDAQ_WRITE_CH_DAQ2.template")
dbLoadRecords("db/NIDAQ_READ_CAL_PRESSURE.db")
dbLoadRecords("db/NIDAQ.db","SYS=FUEL_DAQ, DEV=NI6259")
dbLoadRecords("db/dbSFW.db", "SYS=FUEL_DAQ")
#dbLoadRecords("db/FUEL_DAQ_timeStamp.db")
#dbLoadRecords("db/dbSysMon.db","SYS=FUEL_DAQ")


## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
seq sncNIDAQbaseSeq

