#!../../bin/linux-x86/ESD

## You may have to change ESD to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/ESD.dbd")
ESD_registerRecordDeviceDriver(pdbbase)

## Load record instances
#dbLoadRecords("db/dbExample1.db","user=rootHost")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=1,scan=1 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=2,scan=2 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=3,scan=5 second")
#dbLoadRecords("db/dbSubExample.db","user=rootHost")
dbLoadRecords("db/ESD_TF.vdb")
dbLoadTemplate("db/ESD.template")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
