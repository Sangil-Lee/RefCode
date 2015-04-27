#!../../bin/linux-x86/CWF 

< envPaths 
 
cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/CWF.dbd")
CWF_registerRecordDeviceDriver(pdbbase)

#######################
#                     #
#    AB PLC Driver    #
#                     #
#######################

drvEtherIP_init()

##############
# CWF PLC 
##############
drvEtherIP_define_PLC("plc0", "172.17.101.158",0)
drvEtherIP_define_PLC("plc1", "172.17.101.86",0)
drvEtherIP_define_PLC("plc2", "172.17.101.211",0)


EIP_verbosity(0) 
EIP_buffer_limit(494)



################ 
#CWF UTILITY DB 
################

#dbLoadRecords("db/CWF_TIME.db")
#dbLoadRecords("db/CWF_HEART.db")

#############
#CWF  DB
#############

#dbLoadTemplate("db/CWF_DB.template")
dbLoadTemplate("db/CWF_AI_TEMP.template")
dbLoadTemplate("db/CWF_DI.template")


#############
#PMS  DB
#############
dbLoadTemplate("db/PMS_AI.template")
dbLoadTemplate("db/PMS_BI.template")

#############
#PFC  DB
#############
dbLoadTemplate("db/PFC_AI_TEMP.template")

dbLoadRecords("db/dbSysMon.db","SYS=CWF_SYS")
dbLoadRecords("db/dbPFCCalc.db","SYS=CWF")


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

#dbl > CWF_DB.txt
