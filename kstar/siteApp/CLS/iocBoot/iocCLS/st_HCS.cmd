#!../../bin/linux-x86/CLS 

< envPaths 
 
cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/CLS.dbd")
CLS_registerRecordDeviceDriver(pdbbase)

scanOnceSetQueueSize(6000)

#######################
#                     #
#    AB PLC Driver    #
#                     #
#######################

drvEtherIP_init()
EIP_verbosity(0) 
#EIP_buffer_limit(502)

# CLS HCS PLC
drvEtherIP_define_PLC("plc1", "172.17.101.34",0)


#############
#CLS HCS DB
#############

dbLoadRecords("db/CLS_REMOTE.db")

dbLoadRecords("db/CLS_HCS_UTIL.db")

dbLoadTemplate("db/CLS_He_AI.template")
dbLoadTemplate("db/CLS_He_AI_P.template")
dbLoadTemplate("db/CLS_He_BI.template")
dbLoadTemplate("db/CLS_He_BO_PLC.template")
dbLoadTemplate("db/CLS_He_BO_MANU.template")
dbLoadTemplate("db/CLS_He_AO.template")
dbLoadTemplate("db/CLS_He_AO_SP.template")
dbLoadTemplate("db/CLS_He_AO_FLOW.template")
epicsThreadSleep(1.0)
#dbLoadTemplate("db/CLS_HCS_AI.template")
epicsThreadSleep(1.0)
#dbLoadTemplate("db/CLS_HCS_BI.template")
epicsThreadSleep(1.0)
#dbLoadTemplate("db/CLS_HCS_AO.template")
epicsThreadSleep(1.0)
#dbLoadTemplate("db/CLS_HCS_BO.template")
#dbLoadTemplate("db/CLS_HCS_BO_IN.template")
epicsThreadSleep(1.0)
#dbLoadTemplate("db/CLS_HCS_AI_ALARM.template")



## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

epicsThreadSleep(5.0)

#dbl > CLS_DB.txt
