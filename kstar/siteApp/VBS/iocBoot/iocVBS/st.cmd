#!../../bin/linux-x86/VBS

## You may have to change VBS to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "VBS.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/VBS.dbd")
VBS_registerRecordDeviceDriver pdbbase

## printing debug messages to console
setPrintLevel 3
kuSetPrintLevel 3

# logging debug messages to file
setLogLevel 2
setLogFile("/usr/local/epics/siteApp/VBS/iocBoot/iocVBS/VBS_IOC.log")

# logging the input values of specific channel for each device
#setLogChNo 1

# sleep times of DAQ task
var kDAQSleepMillis		1000

# sfwTree Enable or Disable
var sfwTreeEnable	1

createMainTask("VBS")

######################################################################
#            DevType,  TaskName, DeviceName,TriggerChannel, SigType
######################################################################

# Trigger for each digitizer
createDevice("NI6250", "Dev1", "SC1Mod1", "/Dev1/PFI0", "ai")
createDevice("NI6250", "Dev2", "SC1Mod2", "/Dev2/PFI0", "ai")
createDevice("NI6250", "Dev3", "SC1Mod3", "/Dev3/PFI0", "ai")
#createDevice("NI6250", "Dev4", "SC1Mod4", "/Dev4/PFI0", "ai")

# Sharing trigger of master : sharing is not stable sometime
#createDevice("NI6250", "Dev1", "SC1Mod1", "/Dev1/PFI0", "ai")
#createDevice("NI6250", "Dev2", "SC1Mod2", "/Dev2/PFI0", "ai")
#createDevice("NI6250", "Dev3", "SC1Mod3", "/Dev1/ai/StartTrigger", "ai")
#createDevice("NI6250", "Dev4", "SC1Mod4", "/Dev1/ai/StartTrigger", "ai")

######################################################################
#            ( TaskName, ChannelID, InputType, TagName, arg
######################################################################

createChannel("Dev1",    "0",       "VBS",     "TOR_VB01")
createChannel("Dev1",    "1",       "VBS",     "TOR_VB02")
createChannel("Dev1",    "2",       "VBS",     "TOR_VB03")
createChannel("Dev1",    "3",       "VBS",     "TOR_VB04")
createChannel("Dev1",    "4",       "VBS",     "TOR_VB05")
createChannel("Dev1",    "5",       "VBS",     "TOR_VB06")
createChannel("Dev1",    "6",       "VBS",     "TOR_VB07")
createChannel("Dev1",    "7",       "VBS",     "TOR_VB08")

createChannel("Dev2",    "0",       "VBS",     "TOR_VB09")
createChannel("Dev2",    "1",       "VBS",     "TOR_VB10")
createChannel("Dev2",    "2",       "VBS",     "POL_VB01")
createChannel("Dev2",    "3",       "VBS",     "POL_VB02")
createChannel("Dev2",    "4",       "VBS",     "POL_VB03")
createChannel("Dev2",    "5",       "VBS",     "POL_VB04")
createChannel("Dev2",    "6",       "VBS",     "POL_VB05")
createChannel("Dev2",    "7",       "VBS",     "POL_VB06")

createChannel("Dev3",    "0",       "VBS",     "POL_VB07")
#createChannel("Dev3",    "1",       "VBS",     "TOR_VB18")
#createChannel("Dev3",    "2",       "VBS",     "TOR_VB19")
#createChannel("Dev3",    "3",       "VBS",     "TOR_VB20")
#createChannel("Dev3",    "4",       "VBS",     "TOR_VB21")
#createChannel("Dev3",    "5",       "VBS",     "TOR_VB22")
#createChannel("Dev3",    "6",       "VBS",     "TOR_VB23")
#createChannel("Dev3",    "7",       "VBS",     "TOR_VB24")

#createChannel("Dev4",    "0",       "VBS",     "TOR_VB25")
#createChannel("Dev4",    "1",       "VBS",     "TOR_VB26")

cd ${TOP}

## Load record instances
dbLoadTemplate("db/VBS_READ_CH.template")

dbLoadRecords("db/VBS.db","SYS=VBS, DEV=NI6250")
dbLoadRecords("db/VBS_STATE_Cfg.db","SYS=VBS, DEV=Dev1")
dbLoadRecords("db/VBS_STATE_Cfg.db","SYS=VBS, DEV=Dev2")
dbLoadRecords("db/VBS_STATE_Cfg.db","SYS=VBS, DEV=Dev3")
#dbLoadRecords("db/VBS_STATE_Cfg.db","SYS=VBS, DEV=Dev4")

dbLoadRecords("db/dbSFW.db", "SYS=VBS")
dbLoadRecords("db/dbSysMon.db","SYS=VBS")
dbLoadRecords("db/sysTimeStamp.db", "SYS=VBS")

#########################################
##  add LTU routine. by woong 2012.7.20
createTaskCLTU("VBS", "/dev/tss/cltu.0", "1", "0", "132") 
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=VBS")
dbLoadTemplate "db/VBS_CLTU_CFG.template" 

# ------------------------------------------------
# KSTAR PV Archive
# ------------------------------------------------

#cd ${TOP}/iocBoot/${IOC}
#< kupaEnv

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit()

iocInit

## Start any sequence programs
seq sncVBS
#seq sncKupa

#####################################
# Set initial values
#####################################
epicsThreadSleep 2

#dbpf VBS_MIN_VOLT -0.02
#dbpf VBS_MAX_VOLT 0.02
#dbpf VBS_GAIN 500
dbpf VBS_INPUT_RANGE 3
dbpf VBS_SAMPLING_RATE 10000
dbpf VBS_LTU_P0_SEC0_T0 15.5
dbpf VBS_LTU_P0_SEC0_T1 30.0
dbpf VBS_LTU_setup_p0 1
dbpf VBS_OP_MODE 2
dbpf VBS_ENABLE_IP_MIN 0

#< test.cmd

#####################################

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

