#!../../bin/linux-x86/FUEL

## You may have to change FUEL to something else
## everywhere it appears in this file

< envPaths

cd $(TOP)

## Register all support components
dbLoadDatabase("dbd/FUEL.dbd")
FUEL_registerRecordDeviceDriver(pdbbase)

drvEtherIP_init()
drvEtherIP_define_PLC("plc0", "172.17.101.141",0)
EIP_verbosity(2)
EIP_buffer_limit(494)

## Load record instances

dbLoadRecords("db/FUEL_HEART.db")
dbLoadRecords("db/FUEL_TIME.db")
dbLoadRecords("db/FUEL_REMOTE.db")
dbLoadRecords("db/FUEL_PZ_EMO.db")
dbLoadRecords("db/dbSysMon.db","SYS=FUELSYS")

dbLoadTemplate("db/FUEL_AI.template")
dbLoadTemplate("db/FUEL_AO.template")

dbLoadTemplate("db/FUEL_BI.template")
dbLoadTemplate("db/FUEL_BO.template")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
seq FUEL_ON 

dbl > /root/FUEL_DB.txt

#########################
# IRMIS crawler [shbaek, 2010.01.25]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
