#!../../bin/linux-x86/LHCD_MAIN

## You may have to change LHCD_MAIN to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet("IOCSH_PS1","LHCD_MAIN> ")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/LHCD_MAIN.dbd")
LHCD_MAIN_registerRecordDeviceDriver(pdbbase)

# Initialize EtherIP driver, define PLCs
# -------------------------------------
EIP_buffer_limit(450)
drvEtherIP_init()

# drvEtherIP_define_PLC <name>, <ip_addr>, <slot>
# The driver/device uses the <name> to indentify the PLC.
#
# <ip_addr> can be an IP address in dot-notation
# or a name that the IOC knows about (defined via hostAdd,
# see step 4).
# The IP address gets us to the ENET interface.
# To get to the PLC itself, we need the slot that
# it resides in. The first, left-most slot in the
# ControlLogix crate is slot 0.
drvEtherIP_define_PLC("plc1", "172.17.121.236", 0)

# 10 - Truckload of detailed messages down to the raw send/receive buffers
#  7 - Good value for operation
#  0 - Nothing except very severe problems
#EIP_verbosity=7

## Load record instances
dbLoadTemplate("db/devLHCD_AO.template")
dbLoadTemplate("db/devLHCD_BO.template")
dbLoadTemplate("db/devLHCD_BI.template")
dbLoadTemplate("db/devLHCD_AI.template")

dbLoadRecords("db/devLHCD_MBBI.db", "PV_NAME=LH1_DI1_ST, PV_LINK=LH1_DI1")
#dbLoadTemplate("db/devLHCD_MBBI.template")
dbLoadTemplate("db/devLHCD_MBBI_INP.template")
#dbLoadRecords("db/devLHCD_MBBI_INP.db.org","PV_NAME=LH1_DI1,DESC=DI1")
#dbLoadRecords("db/devLHCD_MBBI_INP.db","PV_NAME=LH1_DO1,DESC=DO1")

dbLoadRecords("db/dbSysMon.db", "SYS=LHCD")
dbLoadRecords("db/sysTimeStamp.db", "SYS=LHCD")

# add by woong 2012.4.19 for LTU
# arg0: system name
# arg1: device file
# arg2: version (0:old, 1:new)
# arg3: type (0:LTU, 1:CTU)
# arg4: id ( only new version )
createTaskCLTU("LHCD", "/dev/tss/cltu.0", "1", "0", "119")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=LHCD"
dbLoadTemplate("db/LHCD_CLTU_CFG.template")

# Heart Bit
dbLoadRecords("db/devLHCD_Heart.db", "P=LH1_MAIN")
# UTIL
dbLoadRecords("db/devLHCD_UTIL.db", "P=LH1")

#DAQ CALC
dbLoadRecords("db/devLHCD_CALC.db")

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
seq sncParameter
seq sncFaultCode
seq sncDCSET
seq sncPowParameter
seq sncOperationMode
seq sncPermission
seq sncFaultHvOff
seq sncPermissionLocal

dbpf LHCD_LTU_SET_FREE_RUN 1

dbpf LH1_RF_FEED_ON 0.01

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

