#!../../bin/linux-x86/LTU

## You may have to change LTU to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet("EPICS_TIMEZONE", "KOR::-540:000000:000000")
#epicsEnvSet("EPICS_TS_NTP_INET", "172.17.100.119")
epicsEnvSet("EPICS_CA_ADDR_LIST", "")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "yes")

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/LTU.dbd"
LTU_registerRecordDeviceDriver pdbbase

# arg0: system name
# arg1: device file
# arg2: version (0:old, 1:new)
# arg3: type (0:LTU, 1:CTU)
# arg4: id ( only new version )

createTaskCLTU("XXX", "/dev/tss/cltu.0", "1", "0", "255") 
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=XXX"
dbLoadTemplate("db/XXX_CLTU_CFG.template")


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit


