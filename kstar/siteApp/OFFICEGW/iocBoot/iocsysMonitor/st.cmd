#!../../bin/linux-x86/sysMonitor

< envPaths

cd ${TOP}
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST,"NO")
epicsEnvSet(EPICS_CA_SERVER_PORT,"10001")
epicsEnvSet(EPICS_CAS_INTF_ADDR_LIST,"172.17.101.199")
epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.101.255")
dbLoadDatabase("dbd/sysMonitor.dbd")
sysMonitor_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/dbSysMon.db","SYS=OFFICEGW")


cd ${TOP}/iocBoot/${IOC}
iocInit()
