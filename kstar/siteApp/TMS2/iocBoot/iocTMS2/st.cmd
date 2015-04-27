#!../../bin/linux-x86/TMS2

## You may have to change TMS2 to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/TMS2.dbd")
TMS2_registerRecordDeviceDriver(pdbbase)

## Compact Field Point init
#drvGenericSerialConfigure "nicFP20x0Ctrl1" "172.17.101.22:6341" 0 0
#drvcFP20x0_init "nicFP20x0Ctrl1" 3. 3. 5. 2000


#epicsThreadSleep(5.)

## Autosave
< ${TOP}/iocBoot/${IOC}/save_restore.cmd


cd ${TOP}/iocBoot/${IOC}

< tms2initDriverPXIDev1.cmd
epicsThreadSleep(1.)
< tms2initDriverPXIDev2.cmd
epicsThreadSleep(1.)
< tms2initDriverPXIDev3.cmd
epicsThreadSleep(1.)
< tms2initDriverPXIDev4.cmd
epicsThreadSleep(1.)
< tms2initDriverPXIDev5.cmd

epicsThreadSleep(5.)


cd ${TOP}
## Load record instances
#dbLoadRecords("db/dbExample1.db","user=khkimHost")
#dbLoadRecords("db/dbExample2.db","user=khkimHost,no=1,scan=1 second")
#dbLoadRecords("db/dbExample2.db","user=khkimHost,no=2,scan=2 second")
#dbLoadRecords("db/dbExample2.db","user=khkimHost,no=3,scan=5 second")
#dbLoadRecords("db/dbSubExample.db","user=khkimHost")
#dbLoadRecords("db/dbPXISCXICernox.db")

#dbLoadTemplate("db/dbPXISCXICernox.template")
#dbLoadRecords("db/dbFanout.db")


#dbLoadTemplate("db/tmsCSStruct.template")
#dbLoadTemplate("db/tmsPFBusLine.template")
#dbLoadTemplate("db/tmsPFCccs.template")
#dbLoadTemplate("db/tmsPFStruct.template")
#dbLoadTemplate("db/tmsCernoxAll.template")

#dbLoadRecords("db/tmsMainValue.db")
#dbLoadRecords("db/tmsPT100.db")
#dbLoadRecords("db/tmsHall.db")
#dbLoadTemplate("db/tmsHall.template")
#dbLoadTemplate("db/tmsPT100.template")
dbLoadRecords("db/tmsTimeStamp.db","IOCNAME=IOC2")
dbLoadRecords("db/tmsInterlock_mbbi.db")
dbLoadRecords("db/dbSysMon.db","SYS=TMS_IOC2")

dbLoadTemplate("db/tmsStrain.template")
dbLoadTemplate("db/tmsStrain_HighSample.template")
dbLoadTemplate("db/tmsDisplacement_TFS.template")
dbLoadTemplate("db/tmsDisplacement_zero.template")
dbLoadTemplate("db/tmsInterlock_bo.template")
dbLoadTemplate("db/tmsInterlock_bi.template")
dbLoadTemplate("db/tmsInterlockMax.template")
dbLoadTemplate("db/tms2MainValue.template")
dbLoadRecords("db/tmsInterlock.db")

dbLoadRecords("db/tmsTaskChangeFanout.db")

dbLoadTemplate("db/tmsTaskChange.template")
dbLoadTemplate("db/stjSoftRvt.template")

## Set this to see messages from mySub
#var mySubDebug 1

##cd ${TOP}/iocBoot/${IOC}
##loadCernoxList cernoxList

##cd ${TOP}/cernoxData/TFStructure
##loadCernoxList("TFStructure.txt")
##cd ${TOP}/cernoxData/HeLine
##loadCernoxList("HeLine.txt")



cd ${TOP}/iocBoot/${IOC}

iocInit()

## Autosave
create_monitor_set("tms_autosave.req", 1)

## Start any sequence programs
#seq sncExample,"user=khkimHost"
seq sncTmsMain, "FNAME=tmsMain_Rad_Dis.tms"
seq sncTmsMain, "FNAME=tmsMain_Ver_Dis.tms"
seq sncTmsMain, "FNAME=tmsMain_Shr_Dis.tms"
#seq sncTmsIntLevel1
seq sncTmsIntLevel2
seq sncTmsIntLevel3
seq sncTmsIntLevel4
#seq sncTmsIntLevel5
#seq sncTmsHeartRun
seq sncTmsDhsZero,"FNAME=tmsDhsZero.tms"
seq sncTmsStrainZeroSet,"FNAME=tmsStrainZero.tms"
seq sncTmsWatchdog 

#########################
# IRMIS crawler [shbaek, 2010.01.07]
#########################
dbl > pvlist

#cd ${TOP}/pvcrawler
#system("./cmd.exe")
#cd ${TOP}/iocBoot/${IOC}
#########################
#cd ${TOP}/bin/linux-x86
#system("./caSysProc &")
#cd ${TOP}/iocBoot/${IOC}
