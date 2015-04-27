#!../../bin/linux-x86/sysMonitor

< envPaths

cd ${TOP}

dbLoadDatabase("dbd/MGApp.dbd")
sysMonitor_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/dbSysMon.db","SYS=MG")
dbLoadRecords("db/dbMGData.db","SYS=MG")


cd ${TOP}/iocBoot/${IOC}
iocInit()
