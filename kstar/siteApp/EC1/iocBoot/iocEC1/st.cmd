#!../../../EC1/bin/linux-x86/EC1

## You may have to change EC1 to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

# 
kuSetLogLevel (3)
kuSetPrintLevel (4)
kuSetLogFile ("/usr/local/epics/siteApp/EC1/iocBoot/iocEC1/EC1_IOC.log")

## Register all support components
dbLoadDatabase("dbd/EC1.dbd")
EC1_registerRecordDeviceDriver pdbbase

# Initialize EtherIP driver, define PLCs
EIP_buffer_limit(50)
drvEtherIP_init()

EIP_verbosity(5)

drvEtherIP_define_PLC("PSPLC", "172.17.121.223", 0)
drvEtherIP_define_PLC("TLPLC", "172.17.121.224", 0)

#dbLoadRecords("../../db/nfri_db1.db")
dbLoadRecords("db/labview.db")
dbLoadRecords("db/dg645.db")
dbLoadRecords("db/ps_in.db", "PLC=PSPLC")
dbLoadRecords("db/ps_out.db", "PLC=PSPLC")
dbLoadRecords("db/tl_out.db", "PLC=TLPLC")
dbLoadRecords("db/tl_in.db", "PLC=TLPLC")

dbLoadRecords("db/EC1_MAIN.db","SYS=EC1")

## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=rootHost"

dbLoadRecords("db/dbSysMon.db","SYS=EC1")
dbLoadRecords("db/sysTimeStamp.db","SYS=EC1")

createTaskCLTU("EC1A", "/dev/tss/cltu.0", "1", "0", "107") 
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=EC1A" 
dbLoadTemplate "db/EC1A_CLTU_CFG.template"

## AUTO LTU MODE SET
dbLoadRecords "db/AUTO_LTU.db"

dbLoadRecords "db/EC1_PermReady.db", "SYS=EC1"

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

# ------------------------------------------------
# KSTAR PV Archive
# ------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
< kupaEnv

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
seq sncKupa
seq sncPermReady, "SYS=EC1"

#####################################
# IRMIS crawler
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe ")
cd ${TOP}/iocBoot/${IOC}
#####################################

