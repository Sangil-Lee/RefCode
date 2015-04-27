#!../../bin/linux-x86/SMBI_DAQ

## You may have to change SMBI_DAQ to something else
## everywhere it appears in this file

< envPaths
#epicsEnvSet("EPICS_CAS_INTF_ADDR_LIST", "172.17.102.60")
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.102.170 172.17.102.120 172.17.100.222")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")
epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "SMBI_DAQ.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")


#epicsEnvSet("EPICS_TIMEZONE", "KOR::-540:000000:000000")
#epicsEnvSet("EPICS_TS_NTP_INET", "172.17.100.21")
#< mdsplusEnv


< sfwEnv

cd ${TOP}
#system("/usr/bin/svn ci")
< svnAutoScript.cmd

## Register all support components
dbLoadDatabase "dbd/SMBI_DAQ.dbd"
SMBI_DAQ_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0

## logging debut message to file
setLogLevel 8 
setLogFile("SMBI_DAQ_Log.log")

createMainTask("SMBI_DAQ")

#           ( Type,   TaskName, DeviceName,TriggerChannel, SigType))
createDevice("NI6123", "Dev1", "Dev1", "/Dev1/PFI0", "ai")

#            ( TaskName, ChannelID, InputType, TagName, arg
#createChannel("Dev1",     "0",      "SMBI_DAQ",  "INTERFERO1", "Density_Calc")
createChannel("Dev1",     "0",      "SMBI_DAQ",  "SM_PCS_IN")
createChannel("Dev1",     "1",      "SMBI_DAQ",  "SM_TIME_IN")
createChannel("Dev1",     "2",      "SMBI_DAQ",  "SM_VAL_IN")
createChannel("Dev1",     "3",      "SMBI_DAQ",  "SM_VAL_OUT")
createChannel("Dev1",     "4",      "SMBI_DAQ",  "MG_CURRENT")
createChannel("Dev1",     "5",      "SMBI_DAQ",  "MG_VOLTAGE")
#createChannel("Dev1",     "6",      "SMBI_DAQ",  "ECE03")
#createChannel("Dev1",     "7",      "SMBI_DAQ",  "ECE04")

#createDevice("NI6123", "Dev2", "Dev2", "/Dev2/PFI0", "ai")

#            ( TaskName, ChannelID, InputType, TagName, arg
#createChannel("Dev2",     "0",      "SMBI_DAQ",  "ECE05")
#createChannel("Dev2",     "1",      "SMBI_DAQ",  "ECE06")
#createChannel("Dev2",     "2",      "SMBI_DAQ",  "ECE07"
#createChannel("Dev2",     "3",      "SMBI_DAQ",  "ECE08")
#createChannel("Dev2",     "4",      "SMBI_DAQ",  "ECE09")
#createChannel("Dev2",     "5",      "SMBI_DAQ",  "ECE10")
#createChannel("Dev2",     "6",      "SMBI_DAQ",  "ECE11")
#createChannel("Dev2",     "7",      "SMBI_DAQ",  "ECE12")

#createDevice("NI6123", "Dev3", "Dev3", "/Dev3/PFI0", "ai")
#createDevice("NI6123", "Dev4", "Dev4", "/Dev4/PFI0", "ai")


cd ${TOP}

## Load record instances
#dbLoadTemplate("db/userHost.substitutions")
#dbLoadRecords("db/dbSubExample.db", "user=rootHost")

dbLoadTemplate("db/SMBI_DAQ_READ_CH.template")
dbLoadTemplate("db/SMBI_DAQ_READ_CH_DEV2.template")
dbLoadTemplate("db/SMBI_DAQ_READ_CAL.template")
dbLoadTemplate("db/SMBI_DAQ_READ_WAVE.template")

dbLoadRecords("db/NI6123.db","SYS=SMBI_DAQ, DEV=NI6123")
dbLoadRecords("db/NI6123_AOCH.db","SYS=SMBI_DAQ, DEV=NI6123")
dbLoadRecords("db/dbSFW.db", "SYS=SMBI_DAQ")
#dbLoadRecords("db/dbSysMon.db","SYS=SMBI_DAQ")
#dbLoadRecords("db/sysTimeStamp.db", "SYS=SMBI_DAQ")


createTaskCLTU("SMBI", "/dev/tss/cltu.0", "0", "0", "999") 

dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=SMBI" 
#dbLoadTemplate "db/SMBI_CLTU_CFG.template"


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
#Not use seq
#seq sncSMBISeq

#####################################
# IRMIS crawler [linupark, 2012.07.17]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
#system("./caSysProc &")
cd ${TOP}/iocBoot/${IOC}
#####################################
