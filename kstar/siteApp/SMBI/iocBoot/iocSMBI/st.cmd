#!../../bin/linux-x86/SMBI

## You may have to change SMBI to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/SMBI.dbd")
SMBI_registerRecordDeviceDriver(pdbbase)

#linupark[[
drvEtherIP_init()
drvEtherIP_define_PLC("plc1", "172.17.101.148",0)
EIP_verbosity(2)
EIP_buffer_limit(494)
#]]
## Load record instances

#linupark[[
dbLoadRecords("db/SMBI_MGI_PULSE.db")
dbLoadRecords("db/dbSysMon.db","SYS=SMBI")
#]]

#linupark[[
dbLoadTemplate("db/SMBI_MGI_AI.template")
dbLoadTemplate("db/SMBI_FILD_AI.template")
dbLoadTemplate("db/SMBI_MGI_AO.template")
dbLoadTemplate("db/SMBI_MGI_BI.template")
dbLoadTemplate("db/SMBI_MGI_BO.template")
dbLoadTemplate("db/SMBI_MGI_BO_SW.template")
#linupark, secound[[
dbLoadRecords("db/SMBI_MGI_HEART.db")
dbLoadRecords("db/SMBI_MGI_TIME.db")
#]]
#]]

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
#Added by linupark[[
seq FUEL_ON2 
#]]

dbl > /root/FUEL_DB.txt

#########################
# IRMIS crawler [linupark, 2012.07.17]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
