#!../../bin/linux-x86/GCDS

## You may have to change GCDS to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","6553600" )
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.210 172.17.101.142 172.17.101.145 172.17.100.222 127.0.0.1")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")
epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "GCDS.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")



< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/GCDS.dbd")
GCDS_registerRecordDeviceDriver(pdbbase)

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0 

## logging debut message to file
setLogLevel 0 
setLogFile("LOG_GCDS.log")

createMainTask("GCDS")

< initDriverNI6133_1.cmd

cd ${TOP}

epicsThreadSleep(1.)
## Add_For_DAQ SFW and MDSplus Put By TG.Lee - 20120217
dbLoadTemplate("db/NIDAQ_READ_CH_DAQ.template")
dbLoadRecords("db/NIDAQ.db","SYS=GCDS, DEV=NI6133")
dbLoadRecords("db/dbSFW.db", "SYS=GCDS")
dbLoadRecords("db/GCDS_timeStamp.db")
dbLoadRecords("db/dbSysMon.db", "SYS=GCDS")


## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
seq sncGCDSSeq

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
