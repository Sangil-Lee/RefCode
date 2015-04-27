#!../../bin/linux-ppc/TSSG

< envPaths

cd ${TOP}

dbLoadDatabase "dbd/TSSG.dbd"
TSSG_registerRecordDeviceDriver pdbbase

createTSSGTask("TSSG")

dbLoadRecords "db/TSSG.db", "SYS=TSSG"

#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

epicsThreadSleep 1
dbpf TSSG_STOP_PULSE 1
epicsThreadSleep 1
dbpf TSSG_STOP_PULSE 0

seq sncRuntimeSeq

