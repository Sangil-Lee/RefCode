#!../../bin/linux-x86/EC1_D1_DAQ

## You may have to change EC1_D1_DAQ to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "EC1_D1_DAQ.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/EC1_D1_DAQ.dbd")
EC1_D1_DAQ_registerRecordDeviceDriver pdbbase

## printing debug messages to console
kuSetPrintLevel (3)
kuSegLogLevel (2)
kuSetLogFile ("/usr/local/epics/siteApp/EC1_D1_DAQ/iocBoot/iocEC1_D1_DAQ/EC1_D1_DAQ_IOC.log")

# sleep times of DAQ task
var kDAQSleepMillis		1000

createMainTask("EC1_D1_DAQ")

######################################################################
#            DevType,  TaskName, DeviceName(arg0), TriggerChannel, SigType
######################################################################

createDevice("NI6220", "Dev1", "Dev1", "/Dev1/PFI0", "ai")

######################################################################
#            TaskName, ChannelID, InputType(arg0), TagName, TagName2
#              * InputType : 0(None), 1(Alpha), 2(Beta)
######################################################################

createChannel("Dev1",    "0",       "1",     "EC1_P_AMPL" , "EC1_P_ALPHA")
createChannel("Dev1",    "1",       "2",     "EC1_P_PHASE", "EC1_P_BETA" )

createChannel("Dev1",    "2",       "0",     "", "")
createChannel("Dev1",    "3",       "0",     "", "")
createChannel("Dev1",    "4",       "0",     "", "")
createChannel("Dev1",    "5",       "0",     "", "")
createChannel("Dev1",    "6",       "0",     "", "")
createChannel("Dev1",    "7",       "0",     "", "")
createChannel("Dev1",    "8",       "0",     "", "")
createChannel("Dev1",    "9",       "0",     "", "")
createChannel("Dev1",   "10",       "0",     "", "")
createChannel("Dev1",   "11",       "0",     "", "")
createChannel("Dev1",   "12",       "0",     "", "")
createChannel("Dev1",   "13",       "0",     "", "")

cd ${TOP}

## Load record instances
dbLoadTemplate("db/EC1_D1_DAQ_READ_CH.template")
dbLoadTemplate("db/EC1_D1_DAQ_POL_CH.template")
dbLoadTemplate("db/EC1_D1_DAQ_TC_CH.template")
dbLoadRecords("db/EC1_D1_DAQ.db","SYS=EC1_D1_DAQ, DEV=NI6220")
dbLoadRecords("db/EC1_D1_DAQ_STATE_Cfg.db","SYS=EC1_D1_DAQ, DEV=Dev1")

dbLoadRecords("db/dbSFW.db", "SYS=EC1_D1_DAQ")
dbLoadRecords("db/dbSysMon.db","SYS=EC1_D1_DAQ")
dbLoadRecords("db/sysTimeStamp.db", "SYS=EC1_D1_DAQ")

#########################################
##  add LTU routine. by woong 2012.7.20
createTaskCLTU("EC1B", "/dev/tss/cltu.0", "1", "0", "135") 
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=EC1B")
dbLoadTemplate "db/EC1B_CLTU_CFG.template" 

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()

iocInit

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

dbpf EC1_D1_DAQ_OP_MODE 2
dbpf EC1_D1_DAQ_SAMPLING_RATE 10000
dbpf EC1_D1_DAQ_AUTO_CREATE_LOCAL_TREE 1

#dbpf EC1B_LTU_P0_SEC0_T0 15
#dbpf EC1B_LTU_P0_SEC0_T1 26
#dbpf EC1B_LTU_setup_p0 1

# Sleep is required
#epicsThreadSleep (2)

#####################################

