#!../../bin/linux-x86/CCS_J

## You may have to change j to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST, "no")
#epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST, "yes")

# at 2012  
#epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.100.101 172.17.100.102 172.17.100.103 172.17.100.104 172.17.100.105 172.17.100.106 172.17.100.107 172.17.100.108 172.17.100.109 172.17.100.110 172.17.100.111 172.17.100.112 172.17.100.113 172.17.100.114 172.17.100.115 172.17.100.116 172.17.100.117 172.17.100.118 172.17.100.119 172.17.100.120 172.17.100.121 172.17.100.122 172.17.100.123 172.17.100.130 172.17.100.200 172.17.100.201 172.17.100.202 172.17.100.203 172.17.100.204 172.17.100.91 172.17.101.10 172.17.101.13 172.17.101.140 172.17.101.149 172.17.101.163 172.17.101.170 172.17.101.20 172.17.101.200 172.17.101.201 172.17.101.21 172.17.101.210 172.17.101.211 172.17.101.23 172.17.101.33 172.17.101.53 172.17.101.80 172.17.102.100 172.17.102.111 172.17.102.120 172.17.102.137 172.17.102.150 172.17.102.175 172.17.102.181 172.17.102.189 172.17.102.21 172.17.102.219 172.17.102.220 172.17.102.221 172.17.102.240 172.17.102.25 172.17.102.58 172.17.102.60 172.17.102.61 172.17.102.62 172.17.102.70 172.17.102.77 172.17.111.101 172.17.111.102 172.17.111.103 172.17.111.104 172.17.111.105 172.17.111.106 172.17.111.107 172.17.111.108 172.17.111.109 172.17.111.110 172.17.111.111 172.17.111.112 172.17.111.116 172.17.111.119 172.17.121.124 172.17.121.180 172.17.121.243 172.17.121.244 172.17.121.250 172.17.121.90 203.230.124.200")
# at 2013
epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.100.107 172.17.100.130 172.17.100.200 172.17.100.201 172.17.100.202 172.17.100.203 172.17.100.204 172.17.100.225 172.17.100.91 172.17.101.10 172.17.101.13 172.17.101.140 172.17.101.149 172.17.101.163 172.17.101.170 172.17.101.20 172.17.101.200 172.17.101.201 172.17.101.203 172.17.101.21 172.17.101.210 172.17.101.211 172.17.101.23 172.17.101.33 172.17.101.53 172.17.101.80 172.17.102.100 172.17.102.111 172.17.102.120 172.17.102.137 172.17.102.150 172.17.102.175 172.17.102.181 172.17.102.189 172.17.102.21 172.17.102.219 172.17.102.220 172.17.102.221 172.17.102.240 172.17.102.25 172.17.102.58 172.17.102.60 172.17.102.61 172.17.102.62 172.17.102.70 172.17.102.77 172.17.111.101 172.17.111.102 172.17.111.103 172.17.111.104 172.17.111.105 172.17.111.106 172.17.111.107 172.17.111.108 172.17.111.109 172.17.111.110 172.17.111.111 172.17.111.112 172.17.111.116 172.17.111.119 172.17.111.80 172.17.121.124 172.17.121.180 172.17.121.240 172.17.121.243 172.17.121.244 172.17.121.250 172.17.121.90 203.230.124.200")
#epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.100.199 172.17.100.192 172.17.100.193 172.17.100.107 172.17.100.200")
#epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.100.192 172.17.100.193 172.17.100.199")

## Register all support components
dbLoadDatabase("dbd/CCS_J.dbd")
CCS_J_registerRecordDeviceDriver(pdbbase)

## Load record instances
dbLoadTemplate("db/ccs_PF1TestSoftPVs_BI.template")
dbLoadTemplate("db/ccs_PF1TestSoftPVs_STRING.template")
dbLoadTemplate("db/ccs_PF1TestSoftPVs_LI.template")
dbLoadRecords("db/ccs_PF1TestSoftPVs_AI.db")
dbLoadRecords("db/ccs_PF1TestSoftPVs_AI2.db")
dbLoadRecords("db/dbSysMon.db","SYS=CCSCSL")
dbLoadRecords("db/ccs_pv_manipulate.db")

#2013. 6. 3 #############
dbLoadTemplate("db/ccs_Interlock_logic.template")  
# 2013. 6. 4 this is from CCS_T temporary
dbLoadRecords("db/ccs_Interlock_T.db")
dbLoadTemplate("db/ccs_EIP_fast_interlock.template")

## Set this to see messages from mySub
#var mySubDebug 1

### 2013. 7. 9 for xxx.121.xxx, xxx.111.xxx 
drvEtherIP_init()
drvEtherIP_define_PLC("INTL_PLC0", "172.17.101.211",0)
EIP_verbosity(4)
EIP_buffer_limit(494)




cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs

seq sncShotSeq
seq sncLongTermSeq
seq sncFaultCheck
seq sncSoftInterlockCheck
seq sncMDSTreeSeq
seq sncDiagnosticCheck
# 2012. 8. 28 copy from CCS_T
# 2013. 6. 12 remove. move to CCS_T again.
# 2013. 7. 9 copy back to CCS_T again.
seq sncMPSWatchdog
# 2013. 8. 6 for softlanding. request from GRPark
seq sncSoftLanding
# 2013. 8. 26  for plasma live status.
seq sncRuntimeMonitor

#########################
# IRMIS crawler [shbaek, 2010.01.07]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

