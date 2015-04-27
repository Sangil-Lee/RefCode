#!../../bin/linux-x86/hrs

## You may have to change hrs to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/hrs.dbd")
hrs_registerRecordDeviceDriver(pdbbase)

## Load record instances
dbLoadRecords("db/dbExample1.db","user=rootHost")
dbLoadRecords("db/dbExample2.db","user=rootHost,no=1,scan=1 second")
dbLoadRecords("db/dbExample2.db","user=rootHost,no=2,scan=2 second")
dbLoadRecords("db/dbExample2.db","user=rootHost,no=3,scan=5 second")
dbLoadRecords("db/dbSubExample.db","user=rootHost")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
