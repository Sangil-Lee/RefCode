#!../../bin/linux-x86_64/MarteShm

## You may have to change MarteShm to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase ("dbd/MarteShm.dbd")
MarteShm_registerRecordDeviceDriver (pdbbase)

# ------------------------------------------------
# Set debug level to display on standard output
# ------------------------------------------------
#  -> 0(OFF)  1(FATAL) 2(ERROR) 3(WARN) 4(MON) 
#  -> 5(INFO) 6(DEBUG) 7(TRACE) 8(DATA) 9(ALL)
# ------------------------------------------------

msuSetPrintLevel (4)

# ------------------------------------------------
# iShmReadKey, iShmWriteKey, iShmSize(number of int), iInterval(msec)
# ------------------------------------------------

#drvMarteShmConfigure (0x1001, 0x1002, 1000, 1)
drvMarteShmConfigure (0x1001, 0x1002, 1000, 100)

cd ${TOP}

## Load record instances
dbLoadTemplate ("db/MarteShm.template")
dbLoadRecords  ("db/MarteShmSetup.db")
#dbLoadRecords  ("db/MarteShmTest.db")

cd ${TOP}/iocBoot/${IOC}
iocInit

seq sncMarteShm

# ------------------------------------------------
# Changes default setting
# ------------------------------------------------

#dbpf MARTE_SHM_WR_PERIOD	0.001
dbpf MARTE_SHM_WR_PERIOD	0.1

