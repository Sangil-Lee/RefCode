#!../../bin/linux-x86/FUEL_DAQ

## You may have to change FUEL_DAQ to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet(EPICS_CA_ADDR_LIST,"172.17.101.200 172.17.101.201 172.17.102.175")
epicsEnvSet(EPICS_CA_ADDR_LIST,"172.17.101.200 172.17.101.201 172.17.101.210 172.17.100.107 172.17.102.175 127.0.0.1")
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST,"no")

epicsEnvSet(EPICS_CA_MAX_ARRAY_BYTES,"6553600")

< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/FUEL_DAQ.dbd"
FUEL_DAQ_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0 

## logging debut message to file
setLogLevel 0
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
dbLoadRecords("db/FUEL_DAQ_timeStamp.db")
dbLoadRecords("db/dbSysMon.db","SYS=FUEL_DAQ")



# 2012. 5. 29  woong add PCI-X (cltu.1)  RM TG
createTaskCLTU("FUEL", "/dev/tss/cltu.1", "1", "0", "125")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=FUEL"
dbLoadTemplate "db/FUEL_CLTU_CFG.template"

# 2012. 7. 24 PCI-E (cltu.0)
createTaskCLTU("FUEL2", "/dev/tss/cltu.0", "1", "0", "134")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=FUEL2"
dbLoadTemplate "db/FUEL2_CLTU_CFG.template"





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

#cd ${TOP}/bin/linux-x86
#system("./caSysProc &")
#cd ${TOP}/iocBoot/${IOC}


## Start any sequence programs
#seq sncExample, "user=rootHost"
seq sncFUELDAQSeq

