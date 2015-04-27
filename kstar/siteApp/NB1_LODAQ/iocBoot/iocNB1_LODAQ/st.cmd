#!../../bin/linux-x86/NB1_LODAQ

## You may have to change NB1_LODAQ to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/NB1_LODAQ.dbd")
NB1_LODAQ_registerRecordDeviceDriver pdbbase

## printing debug messages to console
setPrintLevel 3

# logging debug messages to file
setLogLevel 2
setLogFile("/usr/local/epics/siteApp/NB1_LODAQ/iocBoot/iocNB1_LODAQ/NB1_LODAQ.log")

# sleep times of DAQ task
var kDAQSleepMillis 		1000

# wait times for automatic starting of DAQ after stop of shot
var kAutoTrendRunWaitMillis	2000

createMainTask("NB1_LODAQ")

######################################################################
##           DevType,  TaskName,  DeviceName, TriggerChannel, SigType
######################################################################
createDevice("NI6254", "DT1",  "Dev1",     "/Dev1/PFI0",   "ai")
createDevice("NI6254", "DT2",  "Dev2",     "/Dev2/PFI0",   "ai")

###########################################################################################
##           TaskName, ChannelID, InputType,  TagName,     TermCfg, Flow, IS
###########################################################################################
createChannel("DT1",     "0",     "Delta-T", "NB11_ELDDT" , "RSE", "NB1_WAT_FLW_S25", "1")
createChannel("DT1",     "1",     "Delta-T", "NB11_FIL1DT", "RSE", "NB1_WAT_FLW_S28", "1")
createChannel("DT1",     "2",     "Delta-T", "NB11_FIL2DT", "RSE", "NB1_WAT_FLW_S29", "1")
createChannel("DT1",     "3",     "Delta-T", "NB11_ARC1DT", "RSE", "NB1_WAT_FLW_S26", "1")
createChannel("DT1",     "4",     "Delta-T", "NB11_ARC2DT", "RSE", "NB1_WAT_FLW_S27", "1")
createChannel("DT1",     "5",     "Delta-T", "NB11_G11DT" , "RSE", "NB1_WAT_FLW_S17", "1")
createChannel("DT1",     "6",     "Delta-T", "NB11_G12DT" , "RSE", "NB1_WAT_FLW_S18", "1")
createChannel("DT1",     "7",     "Delta-T", "NB11_G21DT" , "RSE", "NB1_WAT_FLW_S19", "1")
createChannel("DT1",     "8",     "Delta-T", "NB11_G22DT" , "RSE", "NB1_WAT_FLW_S20", "1")
createChannel("DT1",     "9",     "Delta-T", "NB11_G31DT" , "RSE", "NB1_WAT_FLW_S21", "1")
createChannel("DT1",    "10",     "Delta-T", "NB11_G32DT" , "RSE", "NB1_WAT_FLW_S22", "1")
createChannel("DT1",    "11",     "Delta-T", "NB11_G41DT" , "RSE", "NB1_WAT_FLW_S23", "1")
createChannel("DT1",    "12",     "Delta-T", "NB11_G42DT" , "RSE", "NB1_WAT_FLW_S24", "1")
createChannel("DT1",    "13",     "Delta-T", "NB11_SCRDT" , "RSE", "NB1_WAT_FLW_S05", "1")
createChannel("DT1",    "14",     "Delta-T", "NB11_N11DT" , "RSE", "NB1_WAT_FLW_S06", "1")
createChannel("DT1",    "15",     "Delta-T", "NB11_N12DT" , "RSE", "NB1_WAT_FLW_S07", "1")
createChannel("DT1",    "16",     "Delta-T", "NB11_N21DT" , "RSE", "NB1_WAT_FLW_R01", "1")
createChannel("DT1",    "17",     "Delta-T", "NB11_N22DT" , "RSE", "NB1_WAT_FLW_R02", "1")
createChannel("DT1",    "18",     "Delta-T", "NB1_BMSCDT" , "RSE", "NB1_WAT_FLW_S08", "1")
createChannel("DT1",    "19",     "Delta-T", "NB1_BMNIDDT", "RSE", "NB1_WAT_FLW_S10", "1")
createChannel("DT1",    "20",     "Delta-T", "NB1_ID1DT"  , "RSE", "NB1_WAT_FLW_S11", "1")
createChannel("DT1",    "21",     "Delta-T", "NB1_ID2DT"  , "RSE", "NB1_WAT_FLW_S12", "1")
createChannel("DT1",    "22",     "Delta-T", "NB1_ID3DT"  , "RSE", "NB1_WAT_FLW_S13", "1")
createChannel("DT1",    "23",     "Delta-T", "NB1_BLSCDT" , "RSE", "NB1_WAT_FLW_S14", "1")
createChannel("DT1",    "24",     "Delta-T", "NB1_CMCDT"  , "RSE", "NB1_WAT_FLW_S15", "1")
createChannel("DT1",    "25",     "Delta-T", "NB1_BDSCDT" , "RSE", "NB1_WAT_FLW_S16", "1")
#------------------------------------------------------------------------------------------
createChannel("DT1",    "26",     "Delta-T", "NB12_ELDDT" , "RSE", "NB1_WAT_FLW_S48", "2")
createChannel("DT1",    "27",     "Delta-T", "NB12_FIL1DT", "RSE", "NB1_WAT_FLW_S51", "2")
createChannel("DT1",    "28",     "Delta-T", "NB12_FIL2DT", "RSE", "NB1_WAT_FLW_S52", "2")
createChannel("DT1",    "29",     "Delta-T", "NB12_ARC1DT", "RSE", "NB1_WAT_FLW_S49", "2")
createChannel("DT1",    "30",     "Delta-T", "NB12_ARC2DT", "RSE", "NB1_WAT_FLW_S50", "2")
createChannel("DT1",    "31",     "Delta-T", "NB12_G11DT" , "RSE", "NB1_WAT_FLW_S40", "2")
#------------------------------------------------------------------------------------------
createChannel("DT2",    "0",      "Delta-T", "NB12_G12DT" , "RSE", "NB1_WAT_FLW_S41", "2")
createChannel("DT2",    "1",      "Delta-T", "NB12_G21DT" , "RSE", "NB1_WAT_FLW_S42", "2")
createChannel("DT2",    "2",      "Delta-T", "NB12_G22DT" , "RSE", "NB1_WAT_FLW_S43", "2")
createChannel("DT2",    "3",      "Delta-T", "NB12_G31DT" , "RSE", "NB1_WAT_FLW_S44", "2")
createChannel("DT2",    "4",      "Delta-T", "NB12_G32DT" , "RSE", "NB1_WAT_FLW_S45", "2")
createChannel("DT2",    "5",      "Delta-T", "NB12_G41DT" , "RSE", "NB1_WAT_FLW_S46", "2")
createChannel("DT2",    "6",      "Delta-T", "NB12_G42DT" , "RSE", "NB1_WAT_FLW_S47", "2")
createChannel("DT2",    "7",      "Delta-T", "NB12_SCRDT" , "RSE", "NB1_WAT_FLW_S35", "2")
createChannel("DT2",    "8",      "Delta-T", "NB12_N11DT" , "RSE", "NB1_WAT_FLW_S36", "2")
createChannel("DT2",    "9",      "Delta-T", "NB12_N12DT" , "RSE", "NB1_WAT_FLW_S37", "2")
createChannel("DT2",   "10",      "Delta-T", "NB12_N21DT" , "RSE", "NB1_WAT_FLW_S38", "2")
createChannel("DT2",   "11",      "Delta-T", "NB12_N22DT" , "RSE", "NB1_WAT_FLW_S39", "2")
createChannel("DT2",   "12",      "T/C"    , "NB1_FG1"    , "RSE")
createChannel("DT2",   "13",      "T/C"    , "NB11_FG3"   , "RSE")
createChannel("DT2",   "14",      "T/C"    , "NB12_FG3"   , "RSE")
createChannel("DT2",   "15",      "T/C"    , "NB1_FG4"    , "RSE")
createChannel("DT2",   "16",      "T/C"    , "NB1_DUCTDA" , "RSE")
#------------------------------------------------------------------------------------------

## Load record instances
dbLoadTemplate("db/NB1_LODAQ_DeltaT.substitutions")
#dbLoadTemplate("db/NB1_LODAQ_TC.substitutions")

dbLoadRecords("db/NB1_LODAQ.db","SYS=NB1_LODAQ, DEV=NI6254")
dbLoadRecords("db/NB1_LODAQ_Cfg.db","SYS=NB1_LODAQ, DEV=DT1")
dbLoadRecords("db/NB1_LODAQ_Cfg.db","SYS=NB1_LODAQ, DEV=DT2")

dbLoadRecords("db/dbSFW.db","SYS=NB1_LODAQ")
dbLoadRecords("db/dbSysMon.db","SYS=NB1_LODAQ")
dbLoadRecords("db/sysTimeStamp.db","SYS=NB1_LODAQ")

# for testing with self NB1_DTACQ_LocalShot / NB1_BEAM_INJECT_KSTAR / NB1_BEAM_INJECT_DUMMY
#dbLoadRecords("db/NB1_LODAQ_Test.db")

# 2012. 5. 23 woong 
createTaskCLTU("NBI1C", "/dev/tss/cltu.0", "1", "0", "122") 
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=NBI1C")
dbLoadTemplate("db/NBI1C_CLTU_CFG.template")

# ------------------------------------------------
# KSTAR PV Archive
# ------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
< kupaEnv

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
errlogInit

iocInit

## Start any sequence programs
seq sncNB1_LODAQ
seq sncCalAlarm

#####################################
# IRMIS crawler
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe ")
cd ${TOP}/iocBoot/${IOC}
#####################################

dbpf NB11_WAT_ELD_DT_REF 0.60
dbpf NB11_WAT_ARC1_DT_REF 50
dbpf NB11_WAT_ARC2_DT_REF 50
dbpf NB11_WAT_G11_DT_REF 0.50
dbpf NB11_WAT_G12_DT_REF 0.50
dbpf NB11_WAT_G21_DT_REF 0.15
dbpf NB11_WAT_G22_DT_REF 0.15
dbpf NB11_WAT_G31_DT_REF 0.07
dbpf NB11_WAT_G32_DT_REF 0.07
dbpf NB11_WAT_G41_DT_REF 0.17
dbpf NB11_WAT_G42_DT_REF 0.17
dbpf NB11_WAT_SCR_DT_REF 0.45
dbpf NB11_WAT_N11_DT_REF 2.50
dbpf NB11_WAT_N12_DT_REF 2.50
dbpf NB11_WAT_N21_DT_REF 3.10
dbpf NB11_WAT_N22_DT_REF 3.10

dbpf NB12_WAT_ELD_DT_REF 1.10
dbpf NB12_WAT_ARC1_DT_REF 65
dbpf NB12_WAT_ARC2_DT_REF 65
dbpf NB12_WAT_G11_DT_REF 0.50
dbpf NB12_WAT_G12_DT_REF 0.50
dbpf NB12_WAT_G21_DT_REF 0.15
dbpf NB12_WAT_G22_DT_REF 0.15
dbpf NB12_WAT_G31_DT_REF 0.15
dbpf NB12_WAT_G32_DT_REF 0.15
dbpf NB12_WAT_G41_DT_REF 0.25
dbpf NB12_WAT_G42_DT_REF 0.25
dbpf NB12_WAT_SCR_DT_REF 0.70
dbpf NB12_WAT_N11_DT_REF 3.50
dbpf NB12_WAT_N12_DT_REF 5.50
dbpf NB12_WAT_N21_DT_REF 6.00
dbpf NB12_WAT_N22_DT_REF 6.00

dbpf NB1_WAT_BMSC_DT_REF 5.30
dbpf NB1_WAT_BMID_DT_REF 0.70
dbpf NB1_WAT_ID1_DT_REF 29.00
dbpf NB1_WAT_ID2_DT_REF 2.60
dbpf NB1_WAT_ID3_DT_REF 0.20
dbpf NB1_WAT_BLSC_DT_REF 0.70
dbpf NB1_WAT_BDSC_DT_REF 1.35

dbpf NB1_LODAQ_START_T0_SEC1 2.0
dbpf NB1_LODAQ_STOP_T1_SEC1 120.0
dbpf NBI1C_LTU_P2_SEC0_T0 2.0
dbpf NBI1C_LTU_P2_SEC0_T1 2.1

# For test
#dbpf NBI1C_LTU_SHOT_END	1
#dbpf NB1_LODAQ_STOP_T1_SEC1 10.0
#dbpf NB1_LODAQ_SAMPLING_RATE_TYPE 1
#dbpf NB11_BEAM_PULSE 1.0
#dbpf NB11_INT_STIME 1.0
#dbpf NB12_BEAM_PULSE 1.0
#dbpf NB12_INT_STIME 1.0
