#!../../bin/linux-x86/pxi

## You may have to change pxi to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/pxi.dbd")
pxi_registerRecordDeviceDriver(pdbbase)

## Compact Field Point init
drvGenericSerialConfigure "nicFP20x0Ctrl1" "172.17.101.22:6341" 0 0
drvcFP20x0_init "nicFP20x0Ctrl1" 3. 3. 5. 2000


epicsThreadSleep(5.)


cd ${TOP}/iocBoot/${IOC}

< initDriverPXIDev1.cmd
< initDriverPXIDev2.cmd
< initDriverPXIDev3.cmd
< initDriverPXIDev4.cmd
< initDriverPXIDev5.cmd

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
#dbLoadTemplate("db/tmsCSStruct.template")
#dbLoadTemplate("db/tmsPFBusLine.template")
#dbLoadTemplate("db/tmsPFCccs.template")
#dbLoadTemplate("db/tmsPFStruct.template")
dbLoadTemplate("db/tmsCernoxAll.template")
dbLoadTemplate("db/tmsCernoxAllRaw.template")

#dbLoadRecords("db/tmsMainValue.db")
#dbLoadRecords("db/tmsPT100.db")
dbLoadTemplate("db/tmsMainCDCTLevel.template")
dbLoadTemplate("db/tmsMainValue.template")
dbLoadTemplate("db/tmsHall.template")
dbLoadTemplate("db/tmsPT100.template")


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
cd ${TOP}/cernoxData/AllSensor_20070530
loadCernoxList("AllCernoxSensor.txt")

cd ${TOP}/iocBoot/${IOC}

iocInit()

## Start any sequence programs
#seq sncExample,"user=khkimHost"
seq sncTmsMain, "FNAME=tmsMainTFC_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFB_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainTFBUS_TINT.tms"
seq sncTmsMain, "FNAME=tmsMainTFBUS_THR.tms"
seq sncTmsMain, "FNAME=tmsMainTFBUS_TINT_THR.tms"
seq sncTmsMain, "FNAME=tmsMainPFC01_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC02_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC03_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC04_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC05_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC06_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC07_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC15_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainPFC67_TOUT.tms"
seq sncTmsMainLevel, "FNAME=tmsMainCoilTmaxLevel.tms"
seq sncTmsMain, "FNAME=tmsMainTFCPFC_TIN.tms"
seq sncTmsMain, "FNAME=tmsMainTFPFCS_ST.tms"
seq sncTmsMain, "FNAME=tmsMainTFSHL_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMainMaxMax.tms"
seq sncTmsMainCon
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0116_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0104_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0508_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0912_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC1316_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFC0116_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_CLSTFBUS_BHL.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFBHR_BHL.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFBUS_INT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFB_THRICB.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0104_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0508_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0912_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS1316_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0116_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0104_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0508_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0912_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS1316_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_TFS0116_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC1_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC2_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC3_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC4_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC5_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC6_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC7_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC17_TINT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC1_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC2_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC3_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC4_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC5_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC6_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC7_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFC17_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB1_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB2_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB3_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB4_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB5_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB6_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB7_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFB17_TOUT.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS14_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS5_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS6_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS7_TSURF.tms"
seq sncTmsMain, "FNAME=tmsMain_Max_PFS17_TSURF.tms"
