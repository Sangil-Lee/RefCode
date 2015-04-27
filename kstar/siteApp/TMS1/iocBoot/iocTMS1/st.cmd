#!../../bin/linux-x86/TMS1

## You may have to change TMS1 to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/TMS1.dbd")
TMS1_registerRecordDeviceDriver(pdbbase)

## Compact Field Point init
drvGenericSerialConfigure "nicFP20x0Ctrl1" "172.17.101.22:6341" 0 0
drvcFP20x0_init "nicFP20x0Ctrl1" 3. 3. 1. 1000
#drvcFP20x0_init "nicFP20x0Ctrl1" 3. 3. 5. 2000
#drvcFP20x0_init "nicFP20x0Ctrl2" 3. 3. 5. 2000


drvGenericSerialConfigure "nicFP20x0Ctrl2" "172.17.101.64:6341" 0 0
drvcFP20x0_init "nicFP20x0Ctrl2" 3. 3. 1. 1000

epicsThreadSleep(5.)

## Autosave
# < ${TOP}/iocBoot/${IOC}/save_restore.cmd

cd ${TOP}/iocBoot/${IOC}

< initDriverPXIDev1.cmd
< initDriverPXIDev2.cmd
< initDriverPXIDev3.cmd
< initDriverPXIDev4.cmd
< initDriverPXIDev5.cmd
< initDriverPXIDev6.cmd
< initDriverPXIDev7.cmd
< initDriverPXIDev8.cmd
< initDriverPXIDev9.cmd
< initDriverPXIDev10.cmd

epicsThreadSleep(5.)


cd ${TOP}
## Load record instances
#dbLoadRecords("db/dbExample1.db","user=khkimHost")
#dbLoadRecords("db/dbExample2.db","user=khkimHost,no=1,scan=1 second")
#dbLoadRecords("db/dbExample2.db","user=khkimHost,no=2,scan=2 second")
#dbLoadRecords("db/dbExample2.db","user=khkimHost,no=3,scan=5 second")
#dbLoadRecords("db/dbSubExample.db","user=khkimHost")
#dbLoadRecords("db/dbPXISCXICernox.db")

dbLoadTemplate("db/dbPXISCXICernox.template")
dbLoadRecords("db/dbFanout.db")

dbLoadRecords("db/tmsInterlock.db")
dbLoadRecords("db/tmsTimeStamp.db","IOCNAME=IOC1")
dbLoadRecords("db/dbSysMon.db","SYS=TMS_IOC1")
#dbLoadTemplate("db/tmsCSStruct.template")
#dbLoadTemplate("db/tmsPFBusLine.template")
#dbLoadTemplate("db/tmsPFCccs.template")
#dbLoadTemplate("db/tmsPFStruct.template")
dbLoadTemplate("db/tmsCernoxAll.template")
dbLoadTemplate("db/tmsCernoxAllRM.template")
dbLoadTemplate("db/tmsCernoxAllRaw.template")

#dbLoadRecords("db/tmsMainValue.db")
#dbLoadRecords("db/tmsPT100.db")
dbLoadTemplate("db/tmsMainCDCTLevel.template")
dbLoadTemplate("db/tmsMaxValue.template")
dbLoadTemplate("db/tmsMainMaxValue.template")
dbLoadTemplate("db/tmsHall.template")
#dbLoadTemplate("db/tmsVHall.template")
#dbLoadTemplate("db/tmsVVHall.template")
dbLoadTemplate("db/tmsPT100.template")
dbLoadTemplate("db/tmsPT100_V2.template")
dbLoadTemplate("db/tmsRelay.template")
dbLoadTemplate("db/tmsHallRelay.template")

dbLoadTemplate("db/tmsPFBLineMasterSelect.template")


## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
loadCernoxList cernoxList

#cd ${TOP}/cernoxData/TFStructure
#loadCernoxList("TFStructure.txt")
#cd ${TOP}/cernoxData/HeLine
#loadCernoxList("HeLine.txt")
#cd ${TOP}/cernoxData/AddSensor
#loadCernoxList("AddSensor.txt")
#cd ${TOP}/cernoxData/AllSensor_20070423
#loadCernoxList("AllCernoxSensor.txt")
#cd ${TOP}/cernoxData/AllSensor_20071123
#loadCernoxList("AllCernoxSensor_update_20071123.txt")
cd ${TOP}/cernoxData/AllSensor_20120425
loadCernoxList("AllCernoxSensor_update_20120425.txt")

cd ${TOP}/iocBoot/${IOC}

iocInit()

## Autosave request file
# create_monitor_set("tms_autosave.req", 1)

#########################
# IRMIS crawler [shbaek, 2010.01.07]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
cd ${TOP}/bin/linux-x86
system("./caSysProc &")
cd ${TOP}/iocBoot/${IOC}

epicsThreadSleep(5.)

## Start any #sequence programs
##seq sncExample,"user=khkimHost"
seq sncTmsMax, "FNAME=tmsMaxTFC_TOUT.tms"
seq sncTmsMax, "FNAME=tmsMaxPFC_TOUT.tms"
seq sncTmsMax, "FNAME=tmsMaxPFB_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMax, "FNAME=tmsMaxTFBUS_TINT.tms"
epicsThreadSleep(1.)
seq sncTmsMax, "FNAME=tmsMaxTFBUS_THR.tms"
seq sncTmsMax, "FNAME=tmsMaxTFBUS_TINT_THR.tms"
epicsThreadSleep(2.)
seq sncTmsMax, "FNAME=tmsMaxPFC01_TOUT.tms"
epicsThreadSleep(1.)
seq sncTmsMax, "FNAME=tmsMaxPFC02_TOUT.tms"
seq sncTmsMax, "FNAME=tmsMaxPFC03_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMax, "FNAME=tmsMaxPFC04_TOUT.tms"
epicsThreadSleep(1.)
seq sncTmsMax, "FNAME=tmsMaxPFC05_TOUT.tms"
seq sncTmsMax, "FNAME=tmsMaxPFC06_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMax, "FNAME=tmsMaxPFC07_TOUT.tms"
seq sncTmsMax, "FNAME=tmsMaxPFC15_TOUT.tms"
epicsThreadSleep(1.)
seq sncTmsMax, "FNAME=tmsMaxPFC67_TOUT.tms"
#seq sncTmsMainLevel, "FNAME=tmsMainCoilTmaxLevel.tms"
seq sncTmsMax, "FNAME=tmsMaxTFCPFC_TIN.tms"
seq sncTmsMax, "FNAME=tmsMaxTFPFCS_ST.tms"
seq sncTmsMax, "FNAME=tmsMaxTFSHL_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0116_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0104_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0508_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0912_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC1316_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0116_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_CLSTFBUS_BHL.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFBHR_BHL.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFBUS_INT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_TFB_THRICB.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0104_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0508_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0912_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS1316_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0116_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0104_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0508_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0912_TSURF.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_TFS1316_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0116_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC1_TINT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFC2_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC3_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC4_TINT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFC5_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC6_TINT.tms"
epicsThreadSleep(5.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFC7_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC17_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC1_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFC2_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC3_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFC4_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC5_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC6_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFC7_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC17_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFB1_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB2_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB3_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFB4_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB5_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB6_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFB7_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB17_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFS14_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS5_TSURF.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFS6_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS7_TSURF.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFS57_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFCU1_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFCL1_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFCU2_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFCL2_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFCU3_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMain, "FNAME=tmsMain_Max_PFCL3_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFCU4_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFCL4_TOUT.tms"
epicsThreadSleep(2.)
seq sncTmsMax, "FNAME=tmsMaxMaxMax.tms"
epicsThreadSleep(2.)
seq sncTmsMainCon
seq sncTmsHallReverse, "FNAME=tmsHallRelayReverse.tms"
seq sncTmsHallZero, "FNAME=tmsHallZero.tms"

