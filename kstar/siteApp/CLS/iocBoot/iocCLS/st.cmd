#!../../bin/linux-x86/CLS 

< envPaths 
 
cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/CLS.dbd")
CLS_registerRecordDeviceDriver(pdbbase)

#######################
#                     #
#    AB PLC Driver    #
#                     #
#######################

drvEtherIP_init()

##############
# CLS VPS PLC
##############
drvEtherIP_define_PLC("plc0", "172.17.101.31",0)

#############
# CLS HCS PLC
#############

drvEtherIP_define_PLC("plc1", "172.17.101.34",0)

EIP_verbosity(0) 
EIP_buffer_limit(494)

scanOnceSetQueueSize(20000)

#######################
#                     #
#    SRSRGA Driver    #
#                     #
#######################
##S to LAN for PF
drvAsynIPPortConfigure("L11","172.17.101.38:4001",0,0,0)

asynOctetSetOutputEos ("L11", 0, '\r\n')
asynOctetSetInputEos ("L11", 0, '\n\r')
asynInterposeFlushConfig("L11",0,5)

##S to LAN for TF
drvAsynIPPortConfigure("L12","172.17.101.50:4001",0,0,0)

asynOctetSetOutputEos ("L12", 0, '\r\n')
asynOctetSetInputEos ("L12", 0, '\n\r')
asynInterposeFlushConfig("L12",0,5)

## Load record instances

################ 
#CLS UTILITY DB 
################

dbLoadRecords("db/CLS_HEART.db")
dbLoadRecords("db/CLS_TIME.db")
dbLoadRecords("db/CLS_REMOTE.db")
dbLoadRecords("db/CLS_HCS_UTIL.db")
dbLoadRecords("db/CLS_HCS_RESET.db")

dbLoadRecords("db/dbSysMon.db","SYS=CLS_SYS")


#############
#CLS HCS DB
#############

dbLoadTemplate("db/CLS_HCS_AI.template")
dbLoadTemplate("db/CLS_HCS_BI.template")
dbLoadTemplate("db/CLS_HCS_AO.template")
dbLoadTemplate("db/CLS_HCS_BO.template")
dbLoadTemplate("db/CLS_HCS_BO_IN.template")
dbLoadTemplate("db/CLS_HCS_AI_ALARM.template")

dbLoadTemplate("db/CLS_He_AI.template")
dbLoadTemplate("db/CLS_He_AI_P.template")
dbLoadTemplate("db/CLS_He_BI.template")
dbLoadTemplate("db/CLS_He_BO_PLC.template")
dbLoadTemplate("db/CLS_He_BO_MANU.template")
dbLoadTemplate("db/CLS_He_AO.template")
dbLoadTemplate("db/CLS_He_AO_SP.template")

dbLoadTemplate("db/CLS_He_AO_FLOW.template")
dbLoadTemplate("db/CLS_He_AO_FLOW_1.template")

dbLoadTemplate("db/CLS_He_AI_REF.template")

dbLoadTemplate("db/CLS_AO_TEMP.template")

#############
# HMT OMY
#############
dbLoadTemplate("db/NEW_CLS_HCS_AO.template")


#############
#CLS VPS DB
#############

dbLoadTemplate("db/CLS_VPS_AI.template")
dbLoadTemplate("db/CLS_VPS_AI_TMP_I.template")
dbLoadTemplate("db/CLS_VPS_AI_GAUGE.template")
dbLoadTemplate("db/CLS_VPS_GAUGE_AL.template")
dbLoadTemplate("db/CLS_VPS_AO.template")
dbLoadTemplate("db/CLS_VPS_BI.template")
dbLoadTemplate("db/CLS_VPS_BI_ACCEL.template")
dbLoadTemplate("db/CLS_VPS_BO.template")
dbLoadTemplate("db/CLS_VPS_MBBI_TPG256.template")
dbLoadTemplate("db/CLS_VPS_MBBI_TMP.template")

epicsThreadSleep(1.0)


#############
#CLS RGA DB
#############

dbLoadRecords("db/CLS_PF_RGA_INIT.db")
dbLoadRecords("db/CLS_PF_RGA_FL.db")
dbLoadTemplate("db/CLS_PF_RGA_READ.template")

dbLoadRecords("db/CLS_TF_RGA_INIT.db")
dbLoadRecords("db/CLS_TF_RGA_FL.db")
dbLoadTemplate("db/CLS_TF_RGA_READ.template")




## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

epicsThreadSleep(2.0)

#########################
# IRMIS crawler [shbaek, 2010.01.25]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

## Start any sequence programs
seq PF_SRSRGA_ON
seq TF_SRSRGA_ON

#dbl > CLS_DB.txt
