#!../../bin/linux-x86/MMWI

## You may have to change MMWI to something else
## everywhere it appears in this file

< envPaths
#epicsEnvSet("EPICS_CAS_INTF_ADDR_LIST", "172.17.102.60")
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.210 172.17.102.170 172.17.102.62 172.17.102.120 172.17.100.222 127.0.0.1")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")
epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "MMWI.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")


#epicsEnvSet("EPICS_TIMEZONE", "KOR::-540:000000:000000")
#epicsEnvSet("EPICS_TS_NTP_INET", "172.17.100.21")
#< mdsplusEnv


< sfwEnv

cd ${TOP}
#system("/usr/bin/svn ci")
#< svnAutoScript.cmd

## Register all support components
dbLoadDatabase "dbd/MMWI.dbd"
MMWI_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0 

## logging debut message to file
setLogLevel 0 
setLogFile("MMWI_Log.log")

createMainTask("MMWI")

#           ( Type,   TaskName, DeviceName,TriggerChannel, SigType, ClockChannel))
createDevice("NI6123", "Dev1", "Dev1", "/Dev1/PFI0", "ai", "/Dev1/PFI12")

#            ( TaskName, ChannelID, InputType, TagName, arg
createChannel("Dev1",     "0",      "MMWI",  "INTERFERO1", "Density_Calc")
createChannel("Dev1",     "1",      "MMWI",  "INTERFERO2")


# Ready FIR System 2012.4.17
createDevice("NI6123", "Dev2", "Dev2", "/Dev2/PFI0", "ai", "/Dev1/PFI12")
createChannel("Dev2",     "0",      "MMWI",  "INTERFERO3", "Density_Calc")
createChannel("Dev2",     "1",      "MMWI",  "INTERFERO4")



cd ${TOP}

## Load record instances
#dbLoadTemplate("db/userHost.substitutions")
#dbLoadRecords("db/dbSubExample.db", "user=rootHost")

dbLoadTemplate("db/MMWI_READ_CH.template")
dbLoadTemplate("db/MMWI_READ_CH_DEV2.template")
dbLoadTemplate("db/MMWI_READ_CAL.template")
dbLoadTemplate("db/MMWI_READ_CAL_DEV2.template")
dbLoadTemplate("db/MMWI_READ_WAVE.template")
dbLoadTemplate("db/MMWI_READ_WAVE_DEV2.template")

dbLoadRecords("db/NI6123.db","SYS=MMWI, DEV=NI6123")
dbLoadRecords("db/NI6123_DEV2.db","SYS=MMWI, DEV=NI6123")
dbLoadRecords("db/NI6123_AOCH.db","SYS=MMWI, DEV=NI6123")
dbLoadRecords("db/NI6123_AOCH_DEV2.db","SYS=MMWI, DEV=NI6123")
dbLoadRecords("db/dbSFW.db", "SYS=MMWI")
dbLoadRecords("db/dbSysMon.db","SYS=MMWI")
dbLoadRecords("db/sysTimeStamp.db", "SYS=MMWI")


createTaskCLTU("MMWI", "/dev/tss/cltu.0", "1", "0", "115") 
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=MMWI" 
dbLoadTemplate "db/MMWI_CLTU_CFG.template" 

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
seq sncMMWISeq

#####################################
# IRMIS crawler [shbaek, 2010.01.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
#system("./caSysProc &")
cd ${TOP}/iocBoot/${IOC}
#####################################
