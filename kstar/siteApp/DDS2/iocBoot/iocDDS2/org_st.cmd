#!../../bin/linux-ppc/m6802

epicsEnvSet(IOC_DEBUG,"0")
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.101")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST","NO")

epicsEnvSet(TOP,"/root/ioc/m6802")
epicsEnvSet(IOC,"iocm6802")
## Register all support components
dbLoadDatabase("../../dbd/m6802.dbd",0,0)
m6802_registerRecordDeviceDriver(pdbbase) 


cd ${TOP}/iocBoot/${IOC}


< initDriverDev1.cmd
< initDriverDev2.cmd
< initDriverDev3.cmd


cd ${TOP}

## Load record instances
dbLoadRecords("db/dds2.db")
#dbLoadRecords("db/DDS2_AO.db")
dbLoadTemplate("db/DDS2_AO.template")
dbLoadTemplate("db/DDS2_BO.template")
dbLoadTemplate("db/DDS2_BO_ZCAL.template")
#dbLoadRecords("db/DDS2_String.db")
dbLoadTemplate("db/DDS2_String.template")
dbLoadRecords("db/dds2TimeStamp.db")
# Delete SNL Create - Add DDS2_BI.db  at 20081107
#dbLoadRecords("db/DDS_SoftPV.vdb", "SYS=DDS2")
#dbLoadRecords("db/DDS_SoftPV_INTN.vdb", "SYS=DDS2")
dbLoadRecords("db/DDS2_BI.db", "SYS=DDS2")

dbLoadRecords("db/DDS2_LTU.db" )


iocInit()

cd ${TOP}/iocBoot/${IOC}
dbl > pvlist

## Start any sequence programs
#seq sncExample,"user=snl"
seq sncDDS2BoProc
seq sncDDS2DaqSeq

