#!../../bin/linux-x86/VMS_VV
## You may have to change VMS_VV to something else
## everywhere it appears in this file

< envPaths
< ../envCommonIPTables

## prepare env variables and ports definitions
cd ${TOP}/iocBoot/${IOC}
< envPorts
< envVars

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/VMS_VV.dbd")
VMS_VV_registerRecordDeviceDriver(pdbbase)

drvEtherIP_init()

drvEtherIP_define_PLC("plc0", "172.17.101.14",0)

drvEtherIP_define_PLC("plc1", "172.17.101.41",0)

drvEtherIP_define_PLC("plc2", "172.17.101.18",0)

EIP_verbosity(2)
EIP_buffer_limit(494)



## Load record instances

###################################################
# HEATER                                          #
##################################################


dbLoadTemplate("db/VMS_VV_HTR_BI.template")
dbLoadTemplate("db/VMS_VV_HTR_AI.template")
dbLoadTemplate("db/VMS_VV_HTR_BO.template")
dbLoadTemplate("db/VMS_VV_HTR_AO.template")


###################################################
# RGA SUB                                         #
##################################################
 
 
dbLoadTemplate("db/VMS_VV_RGA_BI.template") 
dbLoadTemplate("db/VMS_VV_RGA_BO.template
dbLoadRecords("db/VMS_VV_RGA_BO_LOCAL.db")


###################################################
# SRSRGA #1                                       #
##################################################

dbLoadRecords("db/VMS_VV_RGA1_INIT.db")
dbLoadRecords("db/VMS_VV_RGA1_FL.db")
dbLoadTemplate("db/VMS_VV_RGA1_READ.template")

drvGenericSerialConfigure("L11","172.17.101.42:4001",0,0)

#########################################################



###################################################
# SRSRGA #2                                       #
##################################################
 
dbLoadRecords("db/VMS_VV_RGA2_INIT.db")
dbLoadRecords("db/VMS_VV_RGA2_FL.db")
dbLoadTemplate("db/VMS_VV_RGA2_READ.template")
 
drvGenericSerialConfigure("L13","172.17.101.49:4001",0,0)
 
######################################################### 



#The following command is for a serial line terminal concentrator

#asynOctetSetOutputEos (R0, 0, '\r\n')
#asynOctetSetInputEos (R0, 0, '\n\r')
#asynInterposeFlushConfig(R0,0,5)

#asynSetTraceFile("R0",-1,"")
#asynSetTraceMask("R0",-1,0x02)
#asynSetTraceIOMask("R0",-1,0x4)


## Set this to see messages from mySub
#var mySubDebug 1

# load record instance
cd ${TOP}/iocBoot/${IOC}
< dbLoads

# initialize drivers
cd ${TOP}/iocBoot/${IOC}
< initDrivers

# asSetFilename("accessSecurity")

iocInit()

epicsThreadSleep 2.

#var drvGenericSerialDebug 10


## db list writes to below file ##
#dbl > /root/dbl_VMS_VV.txt

## Start any sequence programs
#seq sncExample,"user=rootHost"

seq SRSRGA_ON
seq SRSRGA2_ON
seq CRPGV_ON
#< seqLoad

#########################
# IRMIS crawler [shbaek, 2010.01.22]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
