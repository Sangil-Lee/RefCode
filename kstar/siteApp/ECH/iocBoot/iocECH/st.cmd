#!../../bin/linux-x86/ECH

## You may have to change ECH to something else
## everywhere it appears in this file

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","6553600" )
epicsEnvSet(CLTU_IOC_DEBUG, "0")

< envPaths


epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.101.201 172.17.101.200 172.17.101.210 172.17.100.107 172.17.101.101 127.0.0.1")
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST, "NO")

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "ECH.log")

< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/ECH.dbd")
ECH_registerRecordDeviceDriver(pdbbase)

cd ${TOP}/iocBoot/${IOC}

## printingdebut messages to console
setPrintLevel 0
## logging debut message to file
setLogLevel 0 
setLogFile("LOG_ECHAO.log")

# Add AO Digitizer by TG 20130617

createMainTask("ECH_AO")

# Important create order if you use AO channel
< initDriverNIDAQmx_AO.cmd
< initDriverNIDAQmx_AI.cmd


cd ${TOP}

epicsThreadSleep(1.)

## New AO DAQ - 20130506
#dbLoadTemplate("db/NIDAQmx_AI.template")

dbLoadTemplate("db/NIDAQmx_BO_SOFT.template")
# Calc the data from ECH Power(kV : 0kV ~ 70kV) to AO volt(0~9Volt)
dbLoadTemplate("db/NIDAQmx_AO_CH0_Pow.template")
# Foward Analog output value(calc) from channel 0 to channel 1 on Real-time control
dbLoadTemplate("db/NIDAQmx_AO_CH1_FWD.template")

dbLoadTemplate("db/NIDAQmx_AO_CH0_Val.template")
dbLoadTemplate("db/NIDAQmx_AO_CH1_Val.template")
dbLoadTemplate("db/NIDAQmx_AO_CH0_Time.template")

dbLoadTemplate("db/NIDAQmx_AO_CH1_Time.template")

#dbLoadTemplate("db/NIDAQmx_AO_CH2_FWD.template")
#dbLoadTemplate("db/NIDAQmx_AO_CH2_Val.template")
#dbLoadTemplate("db/NIDAQmx_AO_CH2_Time.template")

dbLoadTemplate("db/NIDAQmx_AO_CH_PATTERNtoPWR.template")
# Make Real Time PV
dbLoadTemplate("db/NIDAQmx_AO_RTIME.template")
# Foward Analog output value(calc) from channel 0 to channel 1 on Real-time control
dbLoadRecords("db/NIDAQmx_AO_RTIME_CAL.db")

dbLoadRecords("db/NIDAQmx_BO.db")
dbLoadRecords("db/NIDAQmx.db","SYS=ECH_AO, DEV=NI6259")

dbLoadRecords("db/ECH_timeStamp_DAQ.db")
dbLoadRecords("db/dbSFW.db", "SYS=ECH_AO")


##########################################
#dbLoadRecords CLTU DB    20071114   TGLee
##########################################
dbLoadRecords("db/dbSysMon.db","SYS=ECH")
createTaskCLTU("ECH", "/dev/tss/cltu.0", "1", "0", "101")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=ECH"
dbLoadTemplate "db/ECH_CLTU_CFG.template"


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
drvAsynIPPortConfigure("L0", "172.17.121.96:4001", 0, 0, 0)


################
#  TPG256
################
drvAsynIPPortConfigure("L1", "172.17.121.96:4002", 0, 0, 0)


################
#    DG535
################
vxi11Configure("L4", "172.17.121.99", 0,0.0,"gpib0",0,0)

###########################################
# AB CompactLogix PLC
###########################################
EIP_buffer_limit(400)
drvEtherIP_init()
# TL PLC
drvEtherIP_define_PLC("plc1", "172.17.121.231", 0)
# PS PLC
drvEtherIP_define_PLC("plc2", "172.17.121.89", 0)
EIP_verbosity(0)

dbLoadTemplate("db/ECH_BO.template")
dbLoadTemplate("db/ECH_AI.template")
dbLoadTemplate("db/ECH_AI_ARM.template")
dbLoadTemplate("db/ECH_AI_CALC.template")
dbLoadTemplate("db/ECH_AO_INP.template")
dbLoadTemplate("db/ECH_LO_INP.template")
dbLoadTemplate("db/ECH_AO.template")
dbLoadTemplate("db/ECH_BI.template")
dbLoadTemplate("db/ECH_AI_ALARM.template")
dbLoadRecords("db/ECH_FAULT_LIST.db")

## ECH Information
dbLoadRecords("db/ECH_INFO.db")

## ECH Physics operator control permission 
dbLoadRecords("db/ECH_PermReady.db","SYS=ECH")

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
seq sncEchFaultList
seq sncECHAOSeq

epicsThreadSleep(2)

seq sncPARAMZero, "FNAME=ECH_AO_PATTERN_ZERO.inp"
seq sncPermReady, "SYS=ECH"


#########################
# IRMIS crawler [shbaek, 2010.01.25]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
