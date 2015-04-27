#!../../bin/linux-x86/ECH_DAQ

## You may have to change ECH_DAQ to something else
## everywhere it appears in this file

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","6553600" )
#epicsEnvSet(CLTU_IOC_DEBUG, "0")

< envPaths

epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.101.201 172.17.101.200 172.17.101.210 172.17.100.107 172.17.101.101 127.0.0.1 172.17.121.90")
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST, "NO")

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "ECH_DAQ.log")

< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/ECH_DAQ.dbd"
ECH_DAQ_registerRecordDeviceDriver pdbbase


cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0

## logging debut message to file
setLogLevel 0
setLogFile("LOG_ECH_DAQ.log")

createMainTask("ECH")

#< initDriverNI6123_1.cmd
< initDriverNI_1.cmd

cd ${TOP}

epicsThreadSleep(1.)

## Add_For_DAQ SFW and MDSplus Put By TG.Lee - 20120201
## Remove DAQ to New server - 20130213

#dbLoadTemplate("db/ECH_READ_CH_DAQ.template")
#dbLoadTemplate("db/NI6123_AOCH.template")
#dbLoadRecords("db/NI6123.db","SYS=ECH, DEV=NI6123")
#dbLoadRecords("db/dbSFW.db", "SYS=ECH")
#dbLoadRecords("db/ECH_MDS_PARAM.db")
#dbLoadRecords("db/ECH_timeStamp_DAQ.db")

dbLoadRecords("db/NIDAQ.db","SYS=ECH, DEV=NI6220")
dbLoadTemplate("db/ECH_DAQ_AOCH.template")
dbLoadTemplate("db/NIDAQ_READ_CH_DAQ.template")
dbLoadTemplate("db/NIDAQ_WRITE_CH_DAQ.template")
dbLoadRecords("db/dbSFW.db", "SYS=ECH")
dbLoadRecords("db/ECH_MDS_PARAM.db")
dbLoadRecords("db/ECH_timeStamp_DAQ.db")
dbLoadRecords("db/dbSysMon.db","SYS=ECH_DAQ")


## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=rootHost"

# ------------------------------------------------
# KSTAR PV Archive
# ------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
< kupaEnv

cd ${TOP}

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
seq sncECHSeq
seq sncECHCArchive

#########################
# IRMIS crawler [shbaek, 2010.01.25]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

