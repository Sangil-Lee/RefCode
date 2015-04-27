#!../../bin/linux-x86_64/ECEHR

## You may have to change ECEHR to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/ECEHR.dbd"
ECEHR_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0 

## logging debut message to file
setLogLevel 0 
setLogFile("LOG_ECEHR.log")


createMainTask("ECEHR")

< initACQ196_1.cmd 
< initACQ196_2.cmd 
< initACQ196_3.cmd 


cd ${TOP}
## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=dt100Host"

dbLoadTemplate("db/ACQ196_READ_CH_DAQ.template")
dbLoadRecords("db/ACQ196_DAQ.db","SYS=ECEHR, DEV=ACQ196")
dbLoadTemplate("db/ACQ196_WRITE_CH_DAQ.template")
dbLoadRecords("db/ECEHRCal.db")
dbLoadRecords("db/ACQ196_READ_WAVE.db")


dbLoadRecords("db/sysTimeStamp.db", "SYS=ECEHR")
dbLoadRecords("db/dbSFW.db", "SYS=ECEHR")
dbLoadRecords("db/dbSysMon.db", "SYS=ECEHR")

createTaskCLTU("ECEHR", "/dev/tss/cltu.0", "1", "0", "130")
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=ECEHR")
dbLoadTemplate("db/ECEHR_CLTU_CFG.template")


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()
iocInit


## Start any sequence programs
#seq sncExample, "user=dt100Host"
seq sncECEHRSeq
seq sncECEHRCalMode
#####################################
# IRMIS crawler 2012.08.22
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################
