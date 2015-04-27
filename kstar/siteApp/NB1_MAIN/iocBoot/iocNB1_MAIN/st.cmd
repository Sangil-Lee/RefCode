#!../../bin/linux-x86/NBI_MAIN

## You may have to change NBI_MAIN to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet(CLTU_IOC_DEBUG, "0")

## CCS, CCS Host, NB1_MAIN, NB1_DTACQ, NB1_LOCAL, NB1_LODAQ
epicsEnvSet("EPICS_CA_ADDR_LIST", "172.17.101.200 172.17.101.201 172.17.101.210 172.17.121.243 172.17.121.244 172.17.121.124 172.17.121.187")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "no")

cd ../../

## Register all support components
dbLoadDatabase("dbd/NBI_MAIN.dbd")
NBI_MAIN_registerRecordDeviceDriver(pdbbase)

EIP_buffer_limit(100)
drvEtherIP_init()
EIP_verbosity(0)

####################################################################
##  Power Supply PLC
####################################################################
drvEtherIP_define_PLC("plc1", "172.17.121.225", 0)
drvEtherIP_define_PLC("plc2", "172.17.121.176", 0)

dbLoadTemplate("db/NB1_PS_AI_G1.template")
dbLoadTemplate("db/NB1_PS_AI_G3.template")
dbLoadTemplate("db/NB1_PS_AI_ARC.template")
dbLoadTemplate("db/NB1_PS_AI_BIAS.template")
dbLoadTemplate("db/NB1_PS_AI_FILAM.template")
dbLoadTemplate("db/NB1_PS_AI_VME.template")
dbLoadTemplate("db/NB1_PS_AI_MOD.template")

dbLoadTemplate("db/NB1_PS_AO_G1.template")
dbLoadTemplate("db/NB1_PS_AO_G3.template")
dbLoadTemplate("db/NB1_PS_AO_ARC.template")
dbLoadTemplate("db/NB1_PS_AO_BIAS.template")
dbLoadTemplate("db/NB1_PS_AO_FILAM.template")
dbLoadTemplate("db/NB1_PS_AO_VME.template")
dbLoadTemplate("db/NB1_PS_AO_MOD.template")

dbLoadTemplate("db/NB1_PS_BI_G1.template")
dbLoadTemplate("db/NB1_PS_BI_G3.template")
dbLoadTemplate("db/NB1_PS_BI_ARC.template")
dbLoadTemplate("db/NB1_PS_BI_BIAS.template")
dbLoadTemplate("db/NB1_PS_BI_FILAM.template")
dbLoadTemplate("db/NB1_PS_BI_ETC.template")
dbLoadTemplate("db/NB1_PS_BI_VME.template")

dbLoadTemplate("db/NB1_PS_BO_G1.template")
dbLoadTemplate("db/NB1_PS_BO_G3.template")
dbLoadTemplate("db/NB1_PS_BO_ARC.template")
dbLoadTemplate("db/NB1_PS_BO_BIAS.template")
dbLoadTemplate("db/NB1_PS_BO_FILAM.template")
dbLoadTemplate("db/NB1_PS_BO_VME.template")

# woong add 2013. 6. 28
dbLoadTemplate("db/NB1_PS_BO_ETC.template")
dbLoadRecords("db/NB1_PS_RGA.db")


dbLoadTemplate("db/NB1_PS_AO_CALC.template")

### Beamline PLC to Power Supply PLC , LN2
dbLoadTemplate("db/NB1_PS_AI_LN2.template")

dbLoadRecords("db/NB1_PS_BI_CALC.db")
dbLoadRecords("db/NB1_PS_AI_CALC.db")

### Beamline PLC to Power Supply PLC , Interlock
dbLoadRecords("db/NB1_BL_PS_INT.db")

dbLoadRecords("db/NB1_KST_INFO.db")

### BM interlock
dbLoadRecords("db/NB1_BM_FUNC.db")
dbLoadRecords("db/NB1_BM_FUNC2.db")

### FILA VCMD Total 2012.08.07 ###
#dbLoadRecords("db/NB1_PS_AO_FILA_VCMD.db")
### FILA TF Total 2012.08.07 ###
#dbLoadRecords("db/NB1_PS_AO_FILA_TF.db")

dbLoadRecords("db/NB1_LTU_ALL_SHOT.db")
#dbLoadRecords("db/dbSysMon.db","SYS=NBIPS")
dbLoadRecords("db/dbSysMon.db","SYS=NB1_MAIN")
dbLoadRecords("db/sysTimeStamp.db","SYS=NB1_MAIN")

## Power Supply To Operation Mode
dbLoadTemplate("db/NB1_OPR_MODE_BO.template")

## NBI-1 MAIN HeartBeat
dbLoadRecords("db/NB1_MAIN_HeartBeat.db")

## NBI-1 FILA P2 RUN
dbLoadRecords("db/NB1_PS_P2FILA_T.db")

## NBI-1 Parameter Apply
dbLoadRecords("db/NB1_PARAM_BO.db","P=NB1_PARAM")
dbLoadRecords("db/NB1_PARAM_BO.db","P=NB1_PARAM2")

## NBI-1 Power Supply Operation Mode (2012.07.04)
dbLoadRecords("db/NB1_PS_OPER_MODE.db", "P=NB1_OPER1")
dbLoadRecords("db/NB1_PS_OPER_MODE.db", "P=NB1_OPER2")
dbLoadRecords("db/NB1_PS_OPER_MODE.db", "P=NB1_OPER3")

## NBI-1 UTIL (2012.07.04)
dbLoadRecords("db/NB1_UTIL.db")

## NBI-1 DUMMY,KSTAR MODE select
dbLoadRecords("db/NB1_KSTAR_DUMMY.db")


## NBI-1 Integrated EMG,SAFETY(2012.08.09)
dbLoadRecords("db/NB1_PS_CALC_ETC.db")

## GAS Detect
#drvAsynModbusConfigure("L0", "172.17.121.78", 0, 1)

###############################
#  load LTU  2011.5.13 woong
createTaskCLTU("NBI1A", "/dev/tss/cltu.0", "1", "0", "105")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=NBI1A"
dbLoadTemplate "db/NBI1A_CLTU_CFG.template"

#  load LTU  2012.5.23 woong
createTaskCLTU("NBI1D", "/dev/tss/cltu.1", "1", "0", "123")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=NBI1D"
dbLoadTemplate "db/NBI1D_CLTU_CFG.template"

# Permission & Ready
dbLoadRecords "db/NB1_PermReady.db", "SYS=NB11, ISMODE=NB1_OPER_IS1_MODE"
dbLoadRecords "db/NB1_PermReady.db", "SYS=NB12, ISMODE=NB1_OPER_IS2_MODE"

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
seq sncBMInterlock
seq sncBMInterlock2
seq sncParameter
seq sncParameter2
seq sncPsOperation
seq sncPsOperation2
seq sncGasFaultStatus
seq sncVMEPervInterlock
seq sncVMEPervInterlock2

# Permission & Ready
seq sncPermReady, "SYS=NB11, PULSEON=NBI1A_LTU_P5_SEC0_T0, FAULT=NB1_OPER_IS1_MODE"
seq sncPermReady, "SYS=NB12, PULSEON=NBI1D_LTU_P5_SEC0_T0, FAULT=NB1_OPER_IS2_MODE"

#########################
# IRMIS crawler
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")

cd ${TOP}/iocBoot/${IOC}
#########################
