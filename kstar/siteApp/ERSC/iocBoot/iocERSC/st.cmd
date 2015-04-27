#!../../bin/linux-ppc/ERSC

< envPaths

< sfwEnv

cd ${TOP}

dbLoadDatabase "dbd/ERSC.dbd"
ERSC_registerRecordDeviceDriver pdbbase

createMainTask("ERSC")

##            DevType,  TaskName,  arg0(LNKSYS),  arg1(PORT),        arg2, ...
createDevice("scFMC150", "FMC150", "ER",         "P0")

dbLoadRecords "db/dbERSC.db", "SYS=ERSC, DEV=FMC150, TAGA=IF_DX, TAGB=VCO_DX"
#dbLoadRecords "db/dbLTU_LINK.db", "SYS=ERSC, LNKSYS=ER, PORT=P0"
dbLoadRecords "db/dbSFW.db", "SYS=ERSC"
dbLoadTemplate "db/dbLTU_LINK.template"

#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

#epicsThreadSleep 1
#dbpf ERSC_STOP_PULSE 1
#epicsThreadSleep 1
#dbpf ERSC_STOP_PULSE 0

#seq sncRuntimeSeq


