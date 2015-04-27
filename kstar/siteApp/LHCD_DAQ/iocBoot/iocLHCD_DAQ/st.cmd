#!../../bin/linux-x86/LHCD_DAQ

## You may have to change LHCD_DAQ to something else
## everywhere it appears in this file


< envPaths

epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.210 172.17.121.240 172.17.121.100 172.17.121.101 127.0.0.1") 
#epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.210 172.17.121.240 172.17.121.100 172.17.121.101") 
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")
epicsEnvSet("IOCSH_PS1","LHCD_DAQ> ")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/LHCD_DAQ.dbd")
LHCD_DAQ_registerRecordDeviceDriver(pdbbase)

drvAsynMdsplusConfigure(1)

## NI PCI CARD
drvAsynNiPci6254Configure("Dev1/ai0:15", "/Dev1/PFI0", 0, 1, 1)
drvAsynNiPci6220Configure("Dev2/ai0:15", "/Dev2/PFI0", 0, 1, 1)

drvNIPCI6254Setup("6254", 0, "LH1_VCPS", 1, 1)
drvNIPCI6254Setup("6254", 1, "LH1_ICOL", 1, 1)
drvNIPCI6254Setup("6254", 2, "LH1_VAPS", 1, 1)
drvNIPCI6254Setup("6254", 3, "LH1_IANODE", 1, 1)
drvNIPCI6254Setup("6254", 4, "LH1_IZENER", 1, 1)
drvNIPCI6254Setup("6254", 5, "LH1_RFFWD", 1, 1)
drvNIPCI6254Setup("6254", 6, "LH1_RFREV", 1, 1)
drvNIPCI6254Setup("6254", 7, "LH1_RFDRV", 1, 1)
drvNIPCI6254Setup("6254", 8, "LH1_VACION1", 1, 1)
drvNIPCI6254Setup("6254", 9, "LH1_VACION2", 1, 1)

drvNIPCI6220Setup("6220", 0, "LH1_WINDOW", 1 ,1)
drvNIPCI6220Setup("6220", 1, "LH1_BODY", 1 ,1)
drvNIPCI6220Setup("6220", 2, "LH1_DUMMY1", 1 ,1)
drvNIPCI6220Setup("6220", 3, "LH1_DUMMY2", 1 ,1)

## Load record instances
dbLoadRecords("db/devMdsplus.db", "P=LH1_DAQ, PORT=L0, A=0")
dbLoadRecords("db/devNI6254.db", "P=LH1_DAQ_6254, PORT=L0, A=0")
dbLoadRecords("db/devNI6220.db", "P=LH1_DAQ_6220, PORT=L1, A=0")

dbLoadTemplate("db/devIntegral.template")
dbLoadTemplate("db/devPower.template")

dbLoadTemplate("db/devGain.template")
dbLoadTemplate("db/devOffset.template")

dbLoadRecords("db/devLHCD_UTIL.db", "P=LH1_DAQ")

dbLoadRecords("db/dbSysMon.db", "SYS=LH1_DAQ")
dbLoadRecords("db/sysTimeStamp.db", "SYS=LH1_DAQ")


####################################################
drvRFCTRL_Configure(1, "172.17.121.101", 50001)
drvRFANT_Configure(1, "172.17.121.100", 50001)

#drvAsynTCdatasaveConfigure("172.17.121.102", 0, 1)
#dbLoadRecords("db/devTCdatasave.db")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"

dbpf LH1_DAQ_VCPS_GAIN 10.0
dbpf LH1_DAQ_ICOL_GAIN 5.0
dbpf LH1_DAQ_VAPS_GAIN 10.0
dbpf LH1_DAQ_IANODE_GAIN 4.0
dbpf LH1_DAQ_IZENER_GAIN 2.0
dbpf LH1_DAQ_RFFWD_GAIN 1.0
dbpf LH1_DAQ_RFREV_GAIN 1.0

dbpf LH1_DAQ_RFDRV_GAIN  1.0
dbpf LH1_DAQ_VACION1_GAIN 0.0001
dbpf LH1_DAQ_VACION2_GAIN 0.0001

dbpf LH1_DAQ_WINDOW_GAIN 4.0
dbpf LH1_DAQ_BODY_GAIN 4.0
dbpf LH1_DAQ_DUMMY1_GAIN 4.0
dbpf LH1_DAQ_DUMMY2_GAIN 4.0

dbpf LH1_DAQ_VCPS_OFFSET 0.1
dbpf LH1_DAQ_ICOL_OFFSET 0
dbpf LH1_DAQ_VAPS_OFFSET 0.1
dbpf LH1_DAQ_IANODE_OFFSET 0
dbpf LH1_DAQ_IZENER_OFFSET 0
dbpf LH1_DAQ_RFFWD_OFFSET 0
dbpf LH1_DAQ_RFREV_OFFSET 0

dbpf LH1_DAQ_WINDOW_OFFSET 0
dbpf LH1_DAQ_BODY_OFFSET 0
dbpf LH1_DAQ_DUMMY1_OFFSET 0
dbpf LH1_DAQ_DUMMY2_OFFSET 0

dbpf LH1_DAQ_BEAM_PW 1.0
dbpf LH1_DAQ_WINDOW_LPM 10.0
dbpf LH1_DAQ_BODY_LPM 20.0
dbpf LH1_DAQ_DUMMY1_LPM 320.0
dbpf LH1_DAQ_DUMMY2_LPM 320.0

dbpf LH1_DAQ_WINDOW_T 15
dbpf LH1_DAQ_BODY_T 15
dbpf LH1_DAQ_DUMMY1_T 10
dbpf LH1_DAQ_DUMMY2_T 10

dbpf LH1_DAQ_6254_RATE 5000
dbpf LH1_DAQ_6220_RATE 20

dbpf LH1_DAQ_6220_T1 20
dbpf LH1_DAQ_6254_T1 10

dbpf LH1_TC_MDS_SHOT 100
dbpf LH1_TC_MDS_T0 0
dbpf LH1_TC_MDS_T1 40
dbpf LH1_TC_MDS_READY 1

system("/usr/local/epics/base/bin/linux-x86/caput LH1_RFK_DAQDURA_CMD 10")
epicsThreadSleep(0.5)
system("/usr/local/epics/base/bin/linux-x86/caput LH1_RFK_FWDLOSS_CMD 68.12")
epicsThreadSleep(0.5)
system("/usr/local/epics/base/bin/linux-x86/caput LH1_RFK_REVLOSS_CMD 67.9")
epicsThreadSleep(0.5)
system("/usr/local/epics/base/bin/linux-x86/caput LH1_RFANT_DAQ_DURA 10.0")
epicsThreadSleep(0.5)
system("/usr/local/epics/base/bin/linux-x86/caput LH1_RFK_DAQSAMP_CMD 2")
epicsThreadSleep(0.5)
system("/usr/local/epics/base/bin/linux-x86/caput LH1_RFANT_DAQSAMP_CMD 2")
epicsThreadSleep(0.5)
system("/usr/local/epics/base/bin/linux-x86/caput LH1_RF_DAQDURA_CMD 10.0")
epicsThreadSleep(0.5)


#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################
