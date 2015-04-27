#!../../bin/linux-x86/nbi

## You may have to change nbi to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet(CLTU_IOC_DEBUG, "0")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/nbi.dbd")
nbi_registerRecordDeviceDriver(pdbbase)

EIP_buffer_limit(50)
drvEtherIP_init()
EIP_verbosity(0)

dbLoadTemplate("db/NB1_HeartBeat.template")
####################################################################
##  Beamline PLC
####################################################################
drvEtherIP_define_PLC("plc1", "172.17.121.120", 0)
dbLoadTemplate("db/NB1_BL_AI.template")
dbLoadTemplate("db/NB1_BL_BI.template")
dbLoadTemplate("db/NB1_BL_AO.template")
dbLoadTemplate("db/NB1_BL_BO.template")
dbLoadTemplate("db/NB1_TC_to_PLC.template")

dbLoadTemplate("db/NB1_BL_CRP.template")
dbLoadRecords("db/NB1_INT_BO.db")
dbLoadRecords("db/NB1_BL_CALC.db")

####################################################################
##  Waterline PLC
####################################################################
drvEtherIP_define_PLC("plc2", "172.17.121.121", 0)
dbLoadTemplate("db/NB1_WATER_AI.template")
dbLoadTemplate("db/NB1_WATER_AO.template")
dbLoadTemplate("db/NB1_WATER_BI.template")
dbLoadTemplate("db/NB1_WATER_BO.template")
dbLoadTemplate("db/NB1_BL_DELTA.template")

####################################################################
##  Bending Magnet
####################################################################
drvAsynModbusConfigure("L0", "172.17.121.76", 0, 1)
drvAsynModbusConfigure("L1","172.17.121.130", 1, 1)
dbLoadTemplate("db/devBendingAI.template")
dbLoadTemplate("db/devBendingAO.template")
dbLoadTemplate("db/devBendingBO.template")
dbLoadTemplate("db/devBendingMBBI.template")

dbLoadRecords("db/devBendingCalc.db")

####################################################################
##  Etos Tcp
####################################################################
drvAsynEtosConfigure("L1", "172.17.121.123", 0, 1)
dbLoadTemplate("db/devEtosAI.template")
dbLoadTemplate("db/devEtosAO.template")
dbLoadTemplate("db/NB1_EPICS_to_PLC.template")

dbLoadRecords("db/devEtosCALC.db")

dbLoadRecords("db/NB1_BL_BG.db")

# TC Temp. under interlock
dbLoadRecords("db/NB1_TEMP_INT.db")

#dbLoadRecords("db/devMdsplus.db")

####################################################################
##  TC Modules
####################################################################
dbLoadTemplate("db/devTCAi.template")
dbLoadTemplate("db/devTCAi_Kelvin.template")

dbLoadRecords("db/NB1_TC_INTC.db")

drvAsynTcMinMaxConfigure("L0")
drvAsynTcMinMax2Configure("L1")
dbLoadRecords("db/devTcMinMax.db")

####################################################################
##  TC Modules
####################################################################
dbLoadRecords("db/NB1_DUMMY.db")

# TC HIGH Limit Setting
dbLoadTemplate("db/NB1_TC_INTC_AO.template")

# TC INTERLOCK CALC
dbLoadRecords("db/NB1_TC_INTC_CALC.db")

###################################################################
## WATER COOLING CALC
###################################################################
dbLoadRecords("db/NB1_WATER_CALC.db")

####################################################################
##  Time
####################################################################
dbLoadRecords("db/NB1_TIME_STAMP.db", "P=NB1_LOCAL")

dbLoadRecords("db/TestBo.db")

####################################################################
## ICP
####################################################################
dbLoadTemplate("db/devICP.template")
dbLoadRecords("db/ICP_HePAC.db")
drvAsynICPConfigure("I0", 1);

####################################################################
## RGA
####################################################################
#var drvGenericSerialDebug 10
drvGenericSerialConfigure("L11","172.17.121.52:4002",0,0)

dbLoadRecords ("db/NB1_LOCAL_RGA_FL.db", "SYS=NB1_LOCAL, LINK=L11")
dbLoadRecords ("db/NB1_LOCAL_RGA_INIT.db", "SYS=NB1_LOCAL, LINK=L11")
dbLoadTemplate("db/NB1_LOCAL_RGA_READ.template")

########################################################
# LTU module   2011.05.13 woong
createTaskCLTU("NBI1B", "/dev/tss/cltu.0", "1", "0", "106")
dbLoadRecords( "db/SYS_CLTU.db", "TYPE=LTU, SYS=NBI1B")
dbLoadTemplate( "db/NBI1B_CLTU_CFG.template")

####################################################################
## SysMon
####################################################################
dbLoadRecords("db/dbSysMon.db","SYS=NB1_LOCAL")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
seq sncExample

####################################################################
## RGA
####################################################################
epicsThreadSleep 2.

seq SRSRGA_ON
####################################################################

dbpf NBI1B_LTU_Trig_p0 1
dbpf NBI1B_LTU_Trig_p1 1
dbpf NBI1B_LTU_Trig_p2 1

dbpf NBI1B_LTU_P0_SEC0_T0 0
dbpf NBI1B_LTU_P0_SEC0_T1 0

dbpf NBI1B_LTU_P1_SEC0_T0 0
dbpf NBI1B_LTU_P1_SEC0_T1 0

dbpf NBI1B_LTU_P2_SEC0_T0 0
dbpf NBI1B_LTU_P2_SEC0_T1 0

dbpf NBI1B_LTU_P3_SEC0_T0 0
dbpf NBI1B_LTU_P3_SEC0_T1 0

dbpf NBI1B_LTU_P4_SEC0_T0 0
dbpf NBI1B_LTU_P4_SEC0_T1 0

dbpf NBI1B_LTU_SOFT_SETUP_P0 1
dbpf NBI1B_LTU_SOFT_SETUP_P1 1
dbpf NBI1B_LTU_SOFT_SETUP_P2 1
dbpf NBI1B_LTU_SOFT_SETUP_P3 1
dbpf NBI1B_LTU_SOFT_SETUP_P4 1

dbpf NBI1B_LTU_P0_ActiveLow 1
dbpf NBI1B_LTU_P1_ActiveLow 1
dbpf NBI1B_LTU_P3_ActiveLow 1
dbpf NBI1B_LTU_P4_ActiveLow 1

dbpf NBI1B_LTU_SET_FREE_RUN 1
dbpf NBI1B_LTU_P0_ENABLE 1
dbpf NBI1B_LTU_P1_ENABLE 1
dbpf NBI1B_LTU_P2_ENABLE 1
dbpf NBI1B_LTU_P3_ENABLE 1
dbpf NBI1B_LTU_P4_ENABLE 1

dbpf NB1_TC_OVER_VAL 200.0
dbpf NB1_TC_TEMP_AO  10.0

dbpf NB1_VAC_FG31_AI.HIGH "1.0e-2"
dbpf NB1_VAC_FG31_AI.HSV  "MAJOR"

dbpf NB1_VAC_FG1_AI.HIGH "1.0e-3"
dbpf NB1_VAC_FG1_AI.HSV  "MAJOR"

dbpf NB1_RGA_INTC_EN.OSV "MAJOR"

########################
# IRMIS crawler
########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
