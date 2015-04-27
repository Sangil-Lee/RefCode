#!../../bin/linux-x86/sysMonitor

< envPaths

cd ${TOP}

dbLoadDatabase("dbd/sysMonitor.dbd")
sysMonitor_registerRecordDeviceDriver(pdbbase)

# dbLoadRecords("db/dbSysMon.db","SYS=OPI16")
# dbLoadRecords("db/led_contents.db")
dbLoadRecords("db/kwt_test.db", "no=1")
dbLoadRecords("db/kwt_test.db", "no=2")
dbLoadRecords("db/kwt_test.db", "no=3")
dbLoadRecords("db/kwt_test.db", "no=4")
dbLoadRecords("db/kwt_test.db", "no=5")
dbLoadRecords("db/alarm.vdb")
dbLoadRecords("db/sincalc.db")

cd ${TOP}/iocBoot/${IOC}
iocInit()
