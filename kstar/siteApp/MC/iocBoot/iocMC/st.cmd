#!../../bin/linux-x86/MC

## You may have to change MC to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/MC.dbd")
MC_registerRecordDeviceDriver(pdbbase)

cd ${TOP}/iocBoot/${IOC}

< initDriverPCIDev1.cmd
< initDriverPCIDev2.cmd
< initDriverPCIDev3.cmd
< initDriverPCIDev4.cmd
< initDriverPCIDev5.cmd
< initDriverPCIDev6.cmd
< initDriverPCIDev7.cmd
< initDriverPCIDev8.cmd
< initDriverPCIDev9.cmd
< initDriverPCIDev10.cmd
#< initDriverPCIDev11.cmd
#< initDriverPCIDev12.cmd

cd ${TOP}

## Load record instances
#dbLoadRecords("db/dbExample1.db","user=rootHost")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=1,scan=1 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=2,scan=2 second")
#dbLoadRecords("db/dbExample2.db","user=rootHost,no=3,scan=5 second")
#dbLoadRecords("db/dbSubExample.db","user=rootHost")
#dbLoadRecords("db/MC_DAQ_SOFTAO.db")

dbLoadRecords("db/dbSysMon.db","SYS=MC")

dbLoadRecords("db/MC_DAQ_BIO_CAL.db")
dbLoadRecords("db/MC_DAQ_SOFT_MASTER_MBBO.db")
dbLoadRecords("db/MC_DAQ_SOFT_MASTER_STRING.db")
dbLoadRecords("db/MC_DAQ_timeStamp.db")

#dbLoadTemplate("db/MC_DAQ_AO.template")
#dbLoadTemplate("db/MC_DAQ_AI.template")
#dbLoadTemplate("db/MC_DAQ_BO.template")
#dbLoadTemplate("db/MC_DAQ_MBBO.template")
#dbLoadTemplate("db/MC_DAQ_MBBO2.template")
#dbLoadTemplate("db/MC_DAQ_SOFT_AO.template")
#dbLoadTemplate("db/MC_DAQ_SOFT_MASTER.template")
#dbLoadTemplate("db/MC_DAQ_MDSNODE_STRING.template")
#dbLoadTemplate("db/MC_DAQ_AO_8S.template")
#dbLoadTemplate("db/MC_DAQ_AI_8S.template")
#dbLoadTemplate("db/MC_DAQ_BO_8S.template")
#dbLoadTemplate("db/MC_DAQ_MBBO_8S.template")
#dbLoadTemplate("db/MC_DAQ_MBBI_8S.template")
#dbLoadTemplate("db/MC_DAQ_MBBO2_8S.template")

dbLoadTemplate("db/MC_DAQ_AO_10S.template")
dbLoadTemplate("db/MC_DAQ_AI_10S.template")
dbLoadTemplate("db/MC_DAQ_BO_10S.template")
dbLoadTemplate("db/MC_DAQ_MBBO_10S.template")
dbLoadTemplate("db/MC_DAQ_MBBI_10S.template")
dbLoadTemplate("db/MC_DAQ_MBBO2_10S.template")
dbLoadTemplate("db/MC_DAQ_SOFT_MASTER_AO.template")
dbLoadTemplate("db/MC_DAQ_SOFT_MASTER_BO.template")
dbLoadTemplate("db/MC_DAQ_MDSNODE_STRING_10S.template")
## Set this to see messages from mySub
#var mySubDebug 1

# add by woong 2011. 5. 27
createTaskCLTU("MC", "/dev/tss/cltu.0", "1", "0", "110")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=MC"
dbLoadTemplate "db/MC_CLTU_CFG.template"


cd ${TOP}/iocBoot/${IOC}
iocInit()


## Start any sequence programs
#seq sncExample,"user=rootHost"
seq sncMCDaqSeq 
seq sncMCDaqStatus 
seq sncMCMasterSet

#########################
# IRMIS crawler [shbaek, 2010.07.14]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
######################### 
