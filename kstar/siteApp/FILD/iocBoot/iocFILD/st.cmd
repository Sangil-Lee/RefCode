#!../../bin/linux-x86/FILD

## You may have to change FILD to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.210 172.17.102.170 127.0.0.1")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/FILD.dbd"
FILD_registerRecordDeviceDriver pdbbase

## printing debug messages to console
kuPrintLevel 3

# logging debug messages to file
kuLogLevel 2
kuLogFile("/usr/local/epics/siteApp/FILD/iocBoot/iocFILD/FILD_IOC.log")

cd ${TOP}

## Load record instances
dbLoadRecords("db/FILD.db", "user=FILD")
dbLoadRecords("db/dbSysMon.db","SYS=FILD")
dbLoadRecords("db/sysTimeStamp.db", "SYS=FILD")

#####################################
# arg0: system name
# arg1: device file
# arg2: version (0:old, 1:new)
# arg3: type (0:LTU, 1:CTU)
# arg4: id ( only new version )
# -----------------------------------

createTaskCLTU("FILD", "/dev/tss/cltu.0", "1", "0", "131")
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=FILD")
dbLoadTemplate("db/FILD_CLTU_CFG.template")
#####################################

#####################################
# TPG261
#------------------------------------

#var drvGenericSerialDebug 1

# port name, tty name, priority, reopen only after disconnect
drvGenericSerialConfigure ("TPG261CTRL0", "172.17.102.36:4001", 0, 0)

# port name, unit name, devTimeout, cbTimeout, scanInterval
drvTPG262_init ("TPG261CTRL0", "mBar", 5.0, 5.0, 1.0)

dbLoadTemplate("db/FILD_TPG261.template")
#####################################

#####################################
# MASTER-K80S
#------------------------------------

#var drvGenericSerialDebug 1

# port name, tty name, priority, reopen only after disconnect
drvGenericSerialConfigure ("L0", "172.17.102.36:4002", 0, 0)

# port name, devTimeout, cbTimeout, scanInterval
drvMK80S_init ("L0", 5.0, 5.0, 0.5)

dbLoadRecords("db/FILD_MK80S.db", "DTYPE=MK80S")
# Test PLC 
#dbLoadRecords("db/FILD_MK80S.db", "DTYPE=Soft Channel")
#####################################

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
seq sncFILD
# Test PLC
#seq sncFILDTestPLC

#####################################
# Set initial values
#------------------------------------
# Test PLC
#dbpf FILD_TEST_PLC_ENABLE 1
#dbpf FILD_REMOTE_CTRL 1
#dbpf FILD_CMD_EMG_STOP_RELEASE 1
#####################################

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

