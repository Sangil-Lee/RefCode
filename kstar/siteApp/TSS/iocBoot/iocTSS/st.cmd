
## You may have to change TSS to something else
## everywhere it appears in this file

sysClkRateSet(1000)

cd "/usr/local/epics/siteApp/TSS/iocBoot/iocTSS"

< envPaths.fix

routeAdd("172.17.100.0", "172.17.101.1")
routeAdd("172.17.102.0", "172.17.101.1")
routeAdd("172.17.111.0", "172.17.101.1")
routeAdd("172.17.121.0", "172.17.101.1")
#routeAdd("172.17.251.0", "172.17.101.1")



#putenv "CLTU_IOC_DEBUG=2"

putenv("EPICS_TIMEZONE=KOR::-540:000000:000000")
putenv("EPICS_TS_NTP_INET=172.17.100.11")


cd topbin

ld < TSS.munch

cd top

## Register all support components
dbLoadDatabase "dbd/TSS.dbd"
TSS_registerRecordDeviceDriver pdbbase

iocsh

# arg0: system name
# arg1: device file
# arg2: version (0:old, 1:new)
# arg3: type (0:LTU, 1:CTU)
# arg4: id (only new version)
createTaskCLTU( "TSS", "none", "1",   "1", "100")
createTaskCLTU( "CTUV1", "none",  "0",   "1" )

## Load record instances
dbLoadTemplate "db/TSS_CLTU_CFG.template"

dbLoadRecords "db/SYS_CLTU.db", "TYPE=CTU, SYS=TSS"
dbLoadRecords "db/SYS_CLTU.db", "TYPE=CTU, SYS=CTUV1"

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit



iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"

dbpf("TSS_CTU_USE_MMAP_CTL", 1)

# This is not necessary because Old LTU does not use. 2013. 11. 05
# It provides 50MHz reference clock to LTU V.1
#dbpf("TSS_CTU_P7_SEC0_CLOCK", 50000000)
#dbpf("TSS_CTU_P7_PERM_CLK", 1)


