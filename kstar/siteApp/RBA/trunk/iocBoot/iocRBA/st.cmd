#!../../bin/linux-x86/RBA

## You may have to change RBA to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "RBA.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/RBA.dbd")
RBA_registerRecordDeviceDriver pdbbase

## printing debug messages to console
setPrintLevel 3

# logging debug messages to file
setLogLevel 2
setLogFile("/usr/local/epics/siteApp/RBA/iocBoot/iocRBA/RBA_IOC.log")

# logging the input values of specific channel for each device
#setLogChNo 1

# sleep times of DAQ task
var kDAQSleepMillis		1000

createMainTask("RBA")

######################################################################
#            DevType,  TaskName, DeviceName(arg0), TriggerChannel, SigType
######################################################################

# -------------------------------
# Trigger for each digitizer
# -------------------------------
#createDevice("NI6122", "Dev1", "Dev1", "/Dev1/PFI0", "ai")
#createDevice("NI6122", "Dev2", "Dev2", "/Dev2/PFI0", "ai")

# -------------------------------
# Sharing trigger of master
#  : PFI0 and ai/StartTrigger both are ok
# -------------------------------
#createDevice("NI6122", "Dev1", "Dev1", "/Dev3/PFI0", "ai")
#createDevice("NI6122", "Dev2", "Dev2", "/Dev3/PFI0", "ai")

createDevice("NI6122", "Dev1", "Dev1", "/Dev3/ai/StartTrigger", "ai")
createDevice("NI6122", "Dev2", "Dev2", "/Dev3/ai/StartTrigger", "ai")
createDevice("NI6122", "Dev3", "Dev3", "/Dev3/PFI0", "ai")

######################################################################
#            TaskName, ChannelID, InputType(arg0), TagName, PV Name
######################################################################

createChannel("Dev1",    "0",       "RBA",     "RBACH01",   "RBA_CH01")
createChannel("Dev1",    "1",       "RBA",     "RBACH02",   "RBA_CH02")
createChannel("Dev1",    "2",       "RBA",     "RBACH03",   "RBA_CH03")
createChannel("Dev1",    "3",       "RBA",     "RBACH04",   "RBA_CH04")

createChannel("Dev2",    "0",       "RBA",     "RBACH05",   "RBA_CH05")
createChannel("Dev2",    "1",       "RBA",     "RBACH06",   "RBA_CH06")
createChannel("Dev2",    "2",       "RBA",     "RBACH07",   "RBA_CH07")
createChannel("Dev2",    "3",       "RBA",     "RBACH08",   "RBA_CH08")

createChannel("Dev3",    "0",       "RBA",     "RBACH09",   "RBA_CH09")
createChannel("Dev3",    "1",       "RBA",     "RBACH10",   "RBA_CH10")
createChannel("Dev3",    "2",       "RBA",     "RBACH11",   "RBA_CH11")
createChannel("Dev3",    "3",       "RBA",     "RBACH12",   "RBA_CH12")

cd ${TOP}

## Load record instances
dbLoadTemplate("db/RBA_READ_CH.template")
dbLoadRecords("db/RBA.db","SYS=RBA, DEV=NI6122")
dbLoadRecords("db/RBA_STATE_Cfg.db","SYS=RBA, DEV=Dev1")
dbLoadRecords("db/RBA_STATE_Cfg.db","SYS=RBA, DEV=Dev2")
dbLoadRecords("db/RBA_STATE_Cfg.db","SYS=RBA, DEV=Dev3")

dbLoadTemplate("db/RBA_AMP_CH.template")
dbLoadRecords("db/RBA_AMP.db","SYS=RBA, DEV=NI4882")

dbLoadRecords("db/dbSFW.db", "SYS=RBA")
dbLoadRecords("db/dbSysMon.db","SYS=RBA")
dbLoadRecords("db/sysTimeStamp.db", "SYS=RBA")

#########################################
##  add LTU routine. by woong 2012.7.20
createTaskCLTU("RBA", "/dev/tss/cltu.0", "1", "0", "133") 
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=RBA")
dbLoadTemplate "db/RBA_CLTU_CFG.template" 

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()

iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"

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

dbpf RBA_OP_MODE 2
dbpf RBA_SAMPLING_RATE 1000
dbpf RBA_LTU_P0_SEC0_T0 15
dbpf RBA_LTU_P0_SEC0_T1 31
dbpf RBA_LTU_setup_p0 1
dbpf RBA_AUTO_CREATE_LOCAL_TREE 1

# Gain 5000, Filter 200

# Sleep is required
epicsThreadSleep (2)

dbpf RBA_AMP_GAIN_GET 1
dbpf RBA_AMP_FLT_GET 1
dbpf RBA_AMP_T_GET 1
#####################################

