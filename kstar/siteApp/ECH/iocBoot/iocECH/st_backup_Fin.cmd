#!../../bin/linux-x86/ech

## You may have to change ech to something else
## everywhere it appears in this file
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","6553600" )

< envPaths


epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST, "YES")


cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/ech.dbd")
ech_registerRecordDeviceDriver(pdbbase)

cd ${TOP}/iocBoot/${IOC}

< initDriverPCIDev1.cmd

cd ${TOP}

## Add_For_DAQ PCI6220 and MDSplus Put By TG.Lee - 2009/08/11
dbLoadTemplate("db/ECH_AO_DAQ.template")
dbLoadTemplate("db/ECH_BO_DAQ.template")
dbLoadTemplate("db/ECH_WAVE_DAQ.template")
dbLoadTemplate("db/ECH_MDSNODE_STRING.template")
dbLoadTemplate("db/ECH_MDSNODE_STRING2.template")
dbLoadRecords("db/ECH_BIO_CAL_DAQ.db")

## Load record instances
## Change By HMT - 2009/06/12
#dbLoadRecords("db/ech.db","user=rootHost")

##########################################
#dbLoadRecords CLTU DB    20071114   TGLee
##########################################
dbLoadRecords("db/dbSysMon.db","SYS=ECH")
dbLoadRecords("db/ECH_LTU.db")

###############
# MDSPlus PUT  add by TGLee    Remove - line 2##
################
#dbLoadRecords("db/mdsPVNode.db")
#dbLoadTemplate("db/mdsPVNode.template")

################
#    TPG256
################
dbLoadTemplate("db/TPG256.template")

################
#    ESP300
################
dbLoadTemplate("db/ESP300_SI.template")
dbLoadTemplate("db/ESP300_AI.template")
dbLoadTemplate("db/ESP300_AO.template")
dbLoadTemplate("db/ESP300_BI.template")
dbLoadTemplate("db/ESP300_BO.template")
dbLoadRecords("db/ESP300_AI_TB.db")

################
#    VAC ION
################
dbLoadRecords("db/VACION_AI.db")


################
#    DG535
################
dbLoadRecords("db/DG535_Read.db","PORT=L4,A=1")
dbLoadRecords("db/DG535_Write.db","PORT=L4,A=1")

######################################
#                                    #
#    Instrument Driver Configure     #
#                                    #
######################################

################
#  ESP300
################
drvAsynIPPortConfigure("L0", "172.17.101.96:4001", 0, 0, 0)

#asynOctetSetOutputEos (L0, 0, "\r")
#asynOctetSetInputEos (L0, 0, "\r\n")
#asynInterposeFlushConfig(L0,0,3)

#asynSetTraceMask("L0",0,0x02)
#asynSetTraceIOMask("L0",0,0x09)


################
#  TPG256
################
drvAsynIPPortConfigure("L1", "172.17.101.96:4002", 0, 0, 0)


################
#  ION PUMP1
################
drvAsynIPPortConfigure("L2", "172.17.101.96:4003", 0, 0, 0)

################
#  ION PUMP2 
################
drvAsynIPPortConfigure("L3", "172.17.101.96:4004", 0, 0, 0)

#asynSetTraceMask("L3",0,0x02)
#asynSetTraceIOMask("L3",0,0x9)

################
#    DG535
################
vxi11Configure("L4", "172.17.101.99", 0,0.0,"gpib0",0,0)

###########################################
# AB CompactLogix PLC
###########################################
EIP_buffer_limit(50)
drvEtherIP_init()
drvEtherIP_define_PLC("plc1", "172.17.101.231", 0)
EIP_verbosity(0)

dbLoadTemplate("db/ECH_BO.template")
dbLoadTemplate("db/ECH_AI.template")
dbLoadTemplate("db/ECH_AI_CALC.template")
dbLoadTemplate("db/ECH_AO_INP.template")
dbLoadTemplate("db/ECH_LO_INP.template")
dbLoadTemplate("db/ECH_AO.template")
dbLoadTemplate("db/ECH_BI.template")

#asynSetTraceMask("L3",0,0x09)
#asynSetTraceIOMask("L3",0,0xff)

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
seq sncECHDaqSeq
#seq sncMdsPut
