#!../../bin/linux-x86/PMS 

< envPaths 
 
cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/PMS.dbd")
PMS_registerRecordDeviceDriver(pdbbase)

#######################
#                     #
#    AB PLC Driver    #
#                     #
#######################

drvEtherIP_init()

##############
# PMS PLC 
##############
drvEtherIP_define_PLC("plc0", "172.17.101.81",0)
drvEtherIP_define_PLC("plc1", "172.17.101.82",0)
drvEtherIP_define_PLC("plc2", "172.17.101.83",0)
drvEtherIP_define_PLC("plc3", "172.17.101.84",0)
drvEtherIP_define_PLC("plc4", "172.17.101.85",0)
drvEtherIP_define_PLC("plc5", "172.17.101.86",0)

##############
# CWF PLC
##############
drvEtherIP_define_PLC("CWF_PLC", "172.17.101.158",0)



EIP_verbosity(0) 
EIP_buffer_limit(494)



################ 
#PMS UTILITY DB 
################

dbLoadRecords("db/PMS_TIME.db")
dbLoadRecords("db/PMS_HEART.db")
dbLoadRecords("db/PMS_TEMP_UTIL.db")
dbLoadRecords("db/dbSysMon.db","SYS=PMS_SYS")


#############
#PMS  DB
#############

dbLoadTemplate("db/PMS_TEMP.template")
dbLoadTemplate("db/PMS_TEMP_NBI.template")
dbLoadTemplate("db/PMS_IVCC_TEMP.template")
dbLoadTemplate("db/PMS_MAX.template")
dbLoadTemplate("db/PMS_IVCC_MAX.template")
dbLoadTemplate("db/PMS_BI.template")
dbLoadTemplate("db/PMS_AI.template")

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

#dbl > PMS_DB.txt

seq PMS_Alarm

seq PMSMAX, "FNAME=PMS_OD_U.inp"
seq PMSMAX, "FNAME=PMS_CD_U.inp"

epicsThreadSleep(2.)
seq PMSMAX, "FNAME=PMS_ID_U.inp"
seq PMSMAX, "FNAME=PMS_OD_L.inp"

epicsThreadSleep(2.)
seq PMSMAX, "FNAME=PMS_CD_L.inp"
seq PMSMAX, "FNAME=PMS_ID_L.inp"
seq PMSMAX, "FNAME=PMS_IL_U.inp"

epicsThreadSleep(2.)
seq PMSMAX, "FNAME=PMS_PS_U.inp"
seq PMSMAX, "FNAME=PMS_PS_L.inp"
seq PMSMAX, "FNAME=PMS_IL_L.inp"

epicsThreadSleep(2.)
seq PMSMAX, "FNAME=PMS_NB_P.inp"
seq PMSMAX, "FNAME=PMS_NB_G.inp"
seq PMSMAX, "FNAME=PMS_PL_ALL.inp"
seq PMSMAX, "FNAME=PMS_IVCC_ALL.inp"
seq PMSMAX, "FNAME=PMS_IVCC_B.inp"
seq PMSMAX, "FNAME=PMS_IVCC_F.inp"

epicsThreadSleep(2.)

