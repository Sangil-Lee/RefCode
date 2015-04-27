#!../../bin/linux-x86/sysMonitor

< envPaths
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST,"NO")
epicsEnvSet(EPICS_CA_SERVER_PORT,"10001")

cd ${TOP}
dbLoadDatabase("dbd/sysMonitor.dbd")
sysMonitor_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/dbSysMon.db","SYS=CAGATEWAY6")

cd ${TOP}/iocBoot/${IOC}
iocInit()
