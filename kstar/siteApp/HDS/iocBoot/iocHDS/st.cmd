#!../../bin/linux-x86/HDS

## You may have to change SHDS to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/HDS.dbd")
HDS_registerRecordDeviceDriver(pdbbase)

#VPS(AB) PLC Loading

#######################
#		      #							
#    AB PLC Driver    #  
#                     # 
#######################
drvEtherIP_init()
drvEtherIP_define_PLC("plc0", "172.17.101.161",0)
EIP_verbosity(2)
EIP_buffer_limit(494)


#######################
#                     #                                                  
#    S7 PLC Driver    #   
#                     # 
#######################

s7plcConfigure("HCS_PLC","172.17.101.165",2000,8000,8000,1,5000,1000)

scanOnceSetQueueSize(10000)


#######################
#                     #
#    SRSRGA Driver    #
#                     #
#######################
 
drvAsynIPPortConfigure("L11","172.17.101.162:4001",0,0,0)
 
 
#asynOctetSetOutputEos ("L11", 0, '\r\n')
#asynOctetSetInputEos ("L11", 0, '\n\r')
#asynInterposeFlushConfig("L11",0,5)



#############
#HDS VPS DB
#############

dbLoadRecords("db/HDS_VPS_HEART.db")
dbLoadRecords("db/HDS_TIME.db")
dbLoadRecords("db/HDS_VPS_REMOTE.db")

dbLoadTemplate("db/HDS_VPS_AI.template")

dbLoadTemplate("db/HDS_VIB_AI_1.template")
dbLoadTemplate("db/HDS_VIB_AI_2.template")

dbLoadTemplate("db/HDS_VPS_AI_GAUGE.template")
dbLoadTemplate("db/HDS_VPS_GAUGE_AL.template")
dbLoadTemplate("db/HDS_VPS_AO.template")
dbLoadTemplate("db/HDS_VPS_BI.template")
dbLoadTemplate("db/HDS_VPS_BO.template")
dbLoadTemplate("db/HDS_VPS_MBBI_TPG256.template")
dbLoadTemplate("db/HDS_VPS_MBBI_TMP.template")

dbLoadRecords("db/dbSysMon.db","SYS=HDS")



#############
#HDS HCS DB
#############

dbLoadRecords("db/HDS_HCS_HEART.db")
dbLoadRecords("db/HDS_HCS_RESET.db")
############################################
dbLoadTemplate("db/HDS_HCS_BI.template")
############################################
dbLoadTemplate("db/HDS_HCS_AO_FLOW.template")
############################################
dbLoadTemplate("db/HDS_HCS_AI.template")
############################################
dbLoadTemplate("db/HDS_HCS_AI_SEL.template")
dbLoadTemplate("db/HDS_HCS_AI_ALARM.template")
dbLoadTemplate("db/HDS_HCS_AI_ALARM_1.template")
#############################################
dbLoadTemplate("db/HDS_HCS_BO.template")
dbLoadTemplate("db/HDS_HCS_AO_SEL.template")
dbLoadTemplate("db/HDS_HCS_AO_HRS2SHDS.template")
dbLoadTemplate("db/HDS_HCS_AO_TMS2SHDS.template")
dbLoadTemplate("db/HDS_HCS_BO_HRS2SHDS.template")
dbLoadTemplate("db/HDS_HCS_AO.template")

dbLoadTemplate("db/HDS_HCS_AI_T.template")

################################################
#Replaced by SNL Code

#dbLoadTemplate("db/SHDS_HCS_AI_P.template")
#dbLoadTemplate("db/SHDS_HCS_AO_P.template")

#dbLoadTemplate("db/SHDS_HCS_AI_QVV.template")
#dbLoadTemplate("db/SHDS_HCS_AO_QVV.template")
#dbLoadTemplate("db/SHDS_HCS_BI_QVV.template")
################################################


dbLoadRecords("db/HDS_HCS_Soft_PV.db")

dbLoadRecords("db/HDS_HCS_PLC_STAT.db")

#############
#HDS RGA DB
#############

dbLoadRecords("db/HDS_VPS_INIT.db")
dbLoadRecords("db/HDS_VPS_FL.db")
dbLoadTemplate("db/HDS_VPS_READ.template")


cd ${TOP}/iocBoot/${IOC}
iocInit()

dbpf HDS_HCS_W_EPICS_COMM_ERROR 0
dbpr HDS_HCS_W_EPICS_COMM_ERROR
epicsThreadSleep(5.0)

epicsThreadSleep(2.0)

seq SRSRGA_ON 
seq HDSHCS_ON

#########################
# IRMIS crawler [shbaek, 2010.01.25]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
