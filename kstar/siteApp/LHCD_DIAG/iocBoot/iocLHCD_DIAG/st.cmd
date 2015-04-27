#!../../bin/linux-x86/LHCD_DIAG

## You may have to change LHCD_DIAG to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "LHCD_DIAG.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/LHCD_DIAG.dbd")
LHCD_DIAG_registerRecordDeviceDriver pdbbase

## printing debug messages to console
kuSetPrintLevel (4)
kuSetLogLevel (3)
kuSetLogFile ("/usr/local/epics/siteApp/LHCD_DIAG/iocBoot/iocLHCD_DIAG/LHCD_DIAG_IOC.log")

# sleep times of DAQ task
var kDAQSleepMillis		1000

createMainTask("LHCD_DIAG")

######################################################################
#            DevType,  TaskName, DeviceName(arg0), TriggerChannel, SigType
######################################################################

createDevice("NI6254", "Dev1", "Dev1", "/Dev1/PFI0", "ai")

######################################################################
#            TaskName, ChannelID, TagName
######################################################################

createChannel("Dev1",    "0",     "LH1_TL_PFWD1")
createChannel("Dev1",    "1",     "LH1_TL_PREV1" )
createChannel("Dev1",    "2",     "LH1_TL_REV1_I")
createChannel("Dev1",    "3",     "LH1_TL_REV1_Q")
createChannel("Dev1",    "4",     "LH1_LP1_VPROBE")
createChannel("Dev1",    "5",     "LH1_LP2_VPROBE")
createChannel("Dev1",    "6",     "LH1_LP3_VPROBE")
createChannel("Dev1",    "7",     "LH1_LP4_VPROBE")
createChannel("Dev1",    "8",     "LH1_LP1_VPS")
createChannel("Dev1",    "9",     "LH1_LP2_VPS")
createChannel("Dev1",   "10",     "LH1_LP3_VPS")
createChannel("Dev1",   "11",     "LH1_LP4_VPS")

cd ${TOP}

## Load record instances
dbLoadTemplate("db/LHCD_DIAG_READ_CH.template")
dbLoadRecords("db/LHCD_DIAG.db","SYS=LHCD_DIAG, DRV=NI6254, LSYS=EC1B")
dbLoadRecords("db/LHCD_DIAG_STATE_Cfg.db","SYS=LHCD_DIAG, DEV=Dev1, DRV=NI6254")

dbLoadRecords("db/dbSFW.db", "SYS=LHCD_DIAG")
dbLoadRecords("db/dbSysMon.db","SYS=LHCD_DIAG")
dbLoadRecords("db/sysTimeStamp.db", "SYS=LHCD_DIAG")

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()

iocInit

# Reserved for using later
#seq sncLHCD_DIAG

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

#####################################
# Set initial values
#####################################

epicsThreadSleep (1)

dbpf LHCD_DIAG_OP_MODE 2
dbpf LHCD_DIAG_SAMPLING_RATE 50000
#dbpf LHCD_DIAG_AUTO_CREATE_LOCAL_TREE 1

#dbpf EC1B_LTU_P0_SEC0_T0 15
#dbpf EC1B_LTU_P0_SEC0_T1 26
#dbpf EC1B_LTU_setup_p0 1

# Sleep is required
#epicsThreadSleep (2)

#####################################

