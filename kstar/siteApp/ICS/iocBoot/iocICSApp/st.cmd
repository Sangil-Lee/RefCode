#!../../bin/linux-x86/ICSApp

< envPaths

cd ${TOP}

dbLoadDatabase("dbd/ICSApp.dbd")
ICSApp_registerRecordDeviceDriver(pdbbase)

#epicsEnvSet("EPICS_TS_NTP_INET", "172.17.101.201")
#var TSdriverDebug 3
#TSconfigure(1,0,1000,0,0,0,1)
#TSinit("")

drvGenericSerialConfigure("SYSMON1", "172.17.101.208:6341", 0, 0)
drvGenericSerialConfigure("SYSMON2", "172.17.101.209:6341", 0, 0)
drvcFP20x0_init("SYSMON1", 3., 3., 4., 2000)
drvcFP20x0_init("SYSMON2", 3., 3., 4., 2000)

dbLoadRecords("db/dbSysMon.db","SYS=ICS")
dbLoadRecords("db/dbPing.vdb", "user=ICS")
dbLoadTemplate("db/dbicsPT100.template", "")
dbLoadTemplate("db/dbicsDI.template", "")

#dbLoadRecords("db/dbESDPV.db","SYS=ICS_ESD")
dbLoadRecords("db/ESD_TF.vdb","SYS=ICS_ESD")

cd ${TOP}/iocBoot/${IOC}
iocInit()

seq sncMonsvrstatus
seq sncCurTimeSeq, "SYS=ICS"

#seq sncGADBUpdateSeq(stand alone)
seq sncSetResetTimeToShot
