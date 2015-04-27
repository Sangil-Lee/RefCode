#!../../bin/win32-x86/MCWIN1

## You may have to change MCWIN1 to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.102.222 172.17.102.70")

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/MCWIN1.dbd"
MCWIN1_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

< initDriverPCIDev1.cmd
< initDriverPCIDev2.cmd
< initDriverPCIDev3.cmd
#< initDriverPCIDev4.cmd
< initDriverPCIDev5.cmd
< initDriverPCIDev6.cmd
#< initDriverPCIDev7.cmd
#< initDriverPCIDev8.cmd
#< initDriverPCIDev9.cmd
#< initDriverPCIDev10.cmd
##< initDriverPCIDev11.cmd
##< initDriverPCIDev12.cmd

cd ${TOP}

## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=kstarHost"

#dbLoadRecords "db/dbSysMon.db","SYS=MC"
#dbLoadRecords "db/MC_DAQ_timeStamp.db"
dbLoadRecords "db/dbSubNet.db","SYS=MC"

dbLoadRecords "db/MC_DAQ_BIO_CAL.db"
epicsThreadSleep(1.0)
dbLoadRecords "db/MC_DAQ_SOFT_MASTER_MBBO.db"
epicsThreadSleep(1.0)
dbLoadRecords "db/MC_DAQ_SOFT_MASTER_STRING.db"
epicsThreadSleep(1.0)
dbLoadTemplate "db/MC_DAQ_AO_10S.template"
epicsThreadSleep(1.0)
dbLoadTemplate "db/MC_DAQ_AI_10S.template"
epicsThreadSleep(1.0)
#dbLoadTemplate "db/MC_DAQ_BO_10S.template"
dbLoadTemplate "db/MC_DAQ_BO_S1.template"
dbLoadTemplate "db/MC_DAQ_BO_S2.template"
dbLoadTemplate "db/MC_DAQ_BO_S3.template"
epicsThreadSleep(1.0)
#dbLoadTemplate "db/MC_DAQ_BO_S4.template"
dbLoadTemplate "db/MC_DAQ_BO_S5.template"
dbLoadTemplate "db/MC_DAQ_BO_S6.template"
epicsThreadSleep(1.0)
dbLoadTemplate "db/MC_DAQ_MBBO_10S.template"
epicsThreadSleep(1.0)
dbLoadTemplate "db/MC_DAQ_MBBI_10S.template"
epicsThreadSleep(1.0)
#dbLoadTemplate "db/MC_DAQ_MBBO2_10S.template"
dbLoadTemplate "db/MC_DAQ_MBBO2_S1.template"
dbLoadTemplate "db/MC_DAQ_MBBO2_S2.template"
dbLoadTemplate "db/MC_DAQ_MBBO2_S3.template"
epicsThreadSleep(1.0)
#dbLoadTemplate "db/MC_DAQ_MBBO2_S4.template"
dbLoadTemplate "db/MC_DAQ_MBBO2_S5.template"
dbLoadTemplate "db/MC_DAQ_MBBO2_S6.template"
epicsThreadSleep(1.0)
#dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_10S.template"
dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_S1.template"
dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_S2.template"
dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_S3.template"
epicsThreadSleep(1.0)
#dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_S4.template"
dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_S5.template"
dbLoadTemplate "db/MC_DAQ_MDSNODE_STRING_S6.template"

epicsThreadSleep(1.0)
dbLoadTemplate "db/MC_DAQ_SOFT_MASTER_AO.template"
epicsThreadSleep(1.0)
dbLoadTemplate "db/MC_DAQ_SOFT_MASTER_BO.template"
epicsThreadSleep(1.0)

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=kstarHost"
epicsThreadSleep(2.0)
seq sncMCDaqStatus 
epicsThreadSleep(5.0)
seq sncMCMasterSet
epicsThreadSleep(5.0)
seq sncMCDaqSeq 
epicsThreadSleep(5.0)
#########################
# IRMIS crawler 
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("pvcrawler.bat")
cd ${TOP}/iocBoot/${IOC}
