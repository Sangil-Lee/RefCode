#!../../bin/linux-x86/sysMonitor

< envPaths

cd ${TOP}

dbLoadDatabase("dbd/sysMonitor.dbd")
sysMonitor_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/dbSysMon.db","SYS=OPI13")


cd ${TOP}/iocBoot/${IOC}
iocInit()
