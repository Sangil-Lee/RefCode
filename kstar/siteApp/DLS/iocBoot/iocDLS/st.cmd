#!../../bin/linux-x86/DLS

## You may have to change DLS to something else
## everywhere it appears in this file

epicsEnvSet(EPICS_CA_ADDR_LIST,"172.17.121.90 172.17.121.180 172.17.101.140 172.17.101.200 172.17.101.201")
epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST,"no")

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/DLS.dbd")
DLS_registerRecordDeviceDriver(pdbbase)

## Compact Field Point init
# Remove 20130219 Field Point
#drvGenericSerialConfigure "nicFP20x0Ctrl1" "172.17.101.171:6341" 0 0
#drvcFP20x0_init "nicFP20x0Ctrl1" 3. 3. 5. 50

drvGenericSerialConfigure "nicFP20x0Ctrl2" "172.17.101.172:6341" 0 0
drvcFP20x0_init "nicFP20x0Ctrl2" 3. 3. 5. 50

drvGenericSerialConfigure "nicFP20x0Ctrl3" "172.17.101.173:6341" 0 0
drvcFP20x0_init "nicFP20x0Ctrl3" 3. 3. 5. 50

# chage init from 1000 to 50 2011.07.01
drvGenericSerialConfigure "nicFP20x0Ctrl4" "172.17.101.174:6341" 0 0
drvcFP20x0_init "nicFP20x0Ctrl4" 3. 3. 5. 50

#drvGenericSerialConfigure "test" "172.17.101.208:6341" 0 0
#drvcFP20x0_init "test" 3. 3. 5. 50

## Load record instances
#dbLoadRecords("db/dbExample1.db","user=rootHost")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=1,scan=1 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=2,scan=2 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=3,scan=5 second")
#dbLoadRecords("db/dbSubExample.db","user=rootHost")
#dbLoadRecords("db/dlsShutter2_bo.db")
#dbLoadRecords("db/dlsShutter2_bi.db")
#dbLoadTemplate("db/dlsIntLockInterFace.template")

dbLoadRecords("db/dlsTimeStamp.db","IOCNAME=IOC")
dbLoadRecords("db/dlsHeartBeat.db")
dbLoadRecords("db/dbSysMon.db","SYS=DLS")

dbLoadRecords("db/dlsShutter2_mbbo.db")
dbLoadRecords("db/dlsShutterSoft_bo.db")

#dbLoadTemplate("db/dlsShutter.template")
#dbLoadTemplate("db/dlsShutter_p.template")

# Remove 20130219 Field Point
#dbLoadTemplate("db/dlsShutter1_bo.template")
#dbLoadTemplate("db/dlsShutter1p_bo.template")

dbLoadTemplate("db/dlsShutter2_bo.template")
dbLoadTemplate("db/dlsShutter2_bi.template")
dbLoadTemplate("db/dlsShutter2_mbbi.template")

dbLoadTemplate("db/dlsShutter4_bo.template")
dbLoadTemplate("db/dlsShutter4_bo_soft.template")
dbLoadTemplate("db/dlsShutter4_bi.template")
dbLoadTemplate("db/dlsShutter4_mbbi.template")

dbLoadTemplate("db/dlsPT100_3.template")
dbLoadTemplate("db/dlsShutter3_bo.template")
dbLoadTemplate("db/dlsShutter3p_bo.template")

#< ${TOP}/iocBoot/${IOC}/dlsInterlockInterface.cmd

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

#########################
# IRMIS crawler [shbaek, 2010.01.25]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

## Start any sequence programs
#seq sncExample,"user=rootHost"
seq sncDlsStop,"FNAME=dlsAllClosed.dls"
#remove 20120912 because I will do not use this function
#seq sncDlsInterlock,"FNAME=dlsSncInterlock.dls"
seq sncDlsRBAStatus 
epicsThreadSleep(1.0)
seq sncDlsCSTA 
seq sncDlsCSTD
seq sncDlsCSTI
epicsThreadSleep(1.0)
seq sncDlsCSTM
seq sncDlsECEI
seq sncDlsECEI2
seq sncDlsTSC
seq sncDlsTSE
epicsThreadSleep(1.0)
seq sncDlsTSG
seq sncDlsXPCF 
seq sncDlsXCSB 
seq sncDlsXCSB1 
seq sncDlsXCSB2 
epicsThreadSleep(1.0)
seq sncDlsXCSF
seq sncDlsXCSF1
seq sncDlsXCSF2
seq sncDlsXCSF3
seq sncDlsTHA1
epicsThreadSleep(1.0)
seq sncDlsPHA1
seq sncDlsPHA2
seq sncDlsVSS1
seq sncDlsVSS2
epicsThreadSleep(1.0)
seq sncDlsLIBEAM
seq sncDlsProctect,"FNAME=dlsProtection.dls"
seq sncECEIProctect
epicsThreadSleep(1.0)
seq sncDlsTV01
seq sncDlsTV02
seq sncDlsAllShutterMon,"FNAME=dlsAllShutterStatus.dls"
