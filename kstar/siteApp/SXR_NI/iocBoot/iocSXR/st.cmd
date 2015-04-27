#!../../bin/linux-x86/SXR

## You may have to change SXR to something else
## everywhere it appears in this file

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","6553600" )
epicsEnvSet(CLTU_IOC_DEBUG, "0")


< envPaths

epicsEnvSet(EPICS_CA_ADDR_LIST, "172.17.101.201 172.17.101.200 172.17.101.210 172.17.100.107 172epicsEnvSet(EPICS_CA_AUTO_ADDR_LIST, "NO")

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "SXR_DAQ.log")

< sfwEnv


cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/SXR.dbd"
SXR_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}
## printingdebut messages to console
setPrintLevel 0 
## logging debut message to file
setLogLevel 0
setLogFile("LOG_SXRDAQAI.log")

# Add AO Digitizer by TG 20130617

createMainTask("SXR_AI")

# Important create order if you use AO channel
#< initDriverNIDAQmx_AO.cmd
#< initDriverNIDAQmx_AI.cmd
< initDriverNIDAQmx_AI1_B7.cmd 
< initDriverNIDAQmx_AI2_B6.cmd
< initDriverNIDAQmx_AI3_B5.cmd
< initDriverNIDAQmx_AI4_B4.cmd
< initDriverNIDAQmx_AI5_B3.cmd
< initDriverNIDAQmx_AI6_B2.cmd
< initDriverNIDAQmx_AI7_B1.cmd
< initDriverNIDAQmx_AI8_B9.cmd

cd ${TOP}
epicsThreadSleep(1.)

## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=rootHost"

## New AO DAQ - 20130506
dbLoadTemplate("db/NIDAQmx_AI.template")
dbLoadTemplate("db/NIDAQmx_BO_SOFT.template")
dbLoadRecords("db/NIDAQmx.db","SYS=SXR_AI")
dbLoadTemplate("db/NIDAQmx_Dev.template")

dbLoadRecords("db/sysTimeStamp.db","SYS=SXR")
dbLoadRecords("db/dbSFW.db", "SYS=SXR_AI")
#dbLoadRecords("db/dbSysMon.db","SYS=SXR")


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
seq sncSXRDAQSeq


#####################################
# IRMIS crawler [shbaek, 2010.01.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
#system("./caSysProc &")
cd ${TOP}/iocBoot/${IOC}
#####################################
