
## You may have to change TSS to something else
## everywhere it appears in this file

sysClkRateSet(1000)

cd "/usr/local/epics/siteApp/DDS1AB_LTU/iocBoot/iocDDS1AB_LTU"

< envPaths.fix

routeAdd("172.17.100.0", "172.17.102.1")
#routeAdd("172.17.101.0", "172.17.102.1")


#putenv "CLTU_IOC_DEBUG=2"

putenv("EPICS_TIMEZONE=KOR::-540:000000:000000")
putenv("EPICS_TS_NTP_INET=172.17.100.21")


cd topbin

ld < DDS1AB_LTU.munch

cd top

## Register all support components
dbLoadDatabase "dbd/DDS1AB_LTU.dbd"
DDS1AB_LTU_registerRecordDeviceDriver pdbbase

iocsh

# arg0: system name
# arg1: device file
# arg2: version (0:old, 1:new)
# arg3: type (0:LTU, 1:CTU)
# arg4: id (only new version)
#createTaskCLTU( "DDS1A", "1", "0",   "0", "1" )
#createTaskCLTU( "DDS1B", "2", "0",   "0", "2" )
createTaskCLTU( "DDS1A", "none", "1",   "0", "113" )
#createTaskCLTU( "DDS1B", "2", "1",   "0", "114" )

dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=DDS1A"
#dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=DDS1B"

## Load record instances
dbLoadTemplate "db/DDS1A_CLTU_CFG.template"
#dbLoadTemplate "db/DDS1B_CLTU_CFG.template"

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit


iocInit


dbl > pvlist

