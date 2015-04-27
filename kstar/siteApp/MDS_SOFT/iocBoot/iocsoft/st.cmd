#!../../bin/linux-x86/soft

## You may have to change soft to something else
## everywhere it appears in this file

epicsEnvSet(EPICS_CA_ADDR_LIST,"172.17.101.200 172.17.101.201 172.17.100.107")

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/soft.dbd")
soft_registerRecordDeviceDriver(pdbbase)

## Load record instances
#dbLoadRecords("db/dbExample1.db","user=rootHost")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=1,scan=1 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=2,scan=2 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=3,scan=5 second")
#dbLoadRecords("db/dbSubExample.db","user=rootHost")
dbLoadRecords("db/mdsPVNodeSoft.db")
#dbLoadRecords("db/VSSsoftPV.db")
dbLoadRecords("db/dbSysMon.db","SYS=OPI07")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
seq sncKstarTreeCreateDone 
