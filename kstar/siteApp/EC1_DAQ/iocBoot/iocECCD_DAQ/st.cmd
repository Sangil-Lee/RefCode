#!../../bin/linux-x86/ECCD_DAQ

## You may have to change ECCD_DAQ to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/ECCD_DAQ.dbd",0,0)
ECCD_DAQ_registerRecordDeviceDriver(pdbbase) 

#
# portName, extMode, trigMode
#

drvAsynMdsplusConfigure(1)
drvAsynNiPci6254Configure("NIDAQ1", 0, 1)
drvAsynNiPci6220Configure("NIDAQ2", 0, 1)
drvAsynPoloidalScanConfigure(1)

## Load record instances
dbLoadRecords("db/devNiPci6254.db", "P=EC1_DAQ_6254, PORT=L0, A=0")
dbLoadRecords("db/devNiPci6220.db", "P=EC1_DAQ_6220, PORT=L0, A=0")
dbLoadRecords("db/devMdsplus.db", "P=EC1_DAQ, PORT=L0, A=0")

dbLoadTemplate("db/devGain.template")
dbLoadTemplate("db/devOffset.template")

dbLoadTemplate("db/devIntegral.template")
dbLoadTemplate("db/devPower.template")

dbLoadRecords("db/devPoloScan.db", "P=EC1_DAQ, PORT=L0, A=0")

dbLoadRecords("db/devLabviewTC.db")

dbLoadRecords("db/dbSysMon.db","SYS=EC1_DAQ")
dbLoadRecords("db/sysTimeStamp.db","SYS=EC1_DAQ")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}

iocInit()

## Start any sequence programs
#seq sncExample,"user=root"

dbpf EC1_DAQ_VAPS_GAIN "10000"
dbpf EC1_DAQ_IAPS_GAIN "0.004"
dbpf EC1_DAQ_VBPS_GAIN "10000"
dbpf EC1_DAQ_IBPS_GAIN "0.01"
dbpf EC1_DAQ_VCPS_GAIN "10000"
dbpf EC1_DAQ_ICPS_GAIN "50.0"
dbpf EC1_DAQ_OCAPS_GAIN 1
dbpf EC1_DAQ_RFFWD1_GAIN 1
dbpf EC1_DAQ_RFFWD2_GAIN 1
dbpf EC1_DAQ_RFREF1_GAIN 1
dbpf EC1_DAQ_RFREF2_GAIN 1

dbpf EC1_DAQ_VAPS_OFFSET 0
dbpf EC1_DAQ_IAPS_OFFSET 0
dbpf EC1_DAQ_VBPS_OFFSET 0
dbpf EC1_DAQ_IBPS_OFFSET 0
dbpf EC1_DAQ_VCPS_OFFSET 0
dbpf EC1_DAQ_ICPS_OFFSET 0
dbpf EC1_DAQ_OCAPS_OFFSET 0
dbpf EC1_DAQ_RFFWD1_OFFSET 0
dbpf EC1_DAQ_RFFWD2_OFFSET 0
dbpf EC1_DAQ_RFREF1_OFFSET 0
dbpf EC1_DAQ_RFREF2_OFFSET 0

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe ")
cd ${TOP}/iocBoot/${IOC}
#####################################

