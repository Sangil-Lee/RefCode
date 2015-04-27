#!../../bin/linux-ppc/TSSC

< envPaths

< sfwEnv

cd ${TOP}

dbLoadDatabase "dbd/TSSC.dbd"
TSSC_registerRecordDeviceDriver pdbbase

createMainTask("TSSC")

##            DevType,  TaskName,  arg0,  arg1,        arg2, ...
createDevice("scVTX5", "SCFPGA", )

dbLoadRecords "db/dbTSSC.db", "SYS=TSSC"
dbLoadRecords "db/dbSFW.db", "SYS=TSSC"
dbLoadTemplate "db/dbGPIO_in.template"
dbLoadTemplate "db/dbGPIO_out.template"
dbLoadTemplate "db/dbGPIO_dir.template"
dbLoadRecords "db/dbSFW.db", "SYS=TSSC"


#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

seq sncSeqTSSC

epicsThreadSleep 1
dbpf TSSC_RESET 1
epicsThreadSleep 0.2
dbpf TSSC_RESET 0
dbpf TSSC_STOP_PULSE 0

epicsThreadSleep 0.1
dbpf TSSC_GPIO_DIR_CH0 0
epicsThreadSleep 0.1
dbpf TSSC_GPIO_DIR_CH1 0


