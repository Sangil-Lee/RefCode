#!../../bin/linux-x86/IRVB_LTU

## You may have to change IRVB_LTU to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet(CLTU_IOC_DEBUG, "0") /* 3 */    /* don't care about it. */

cd ${TOP}

## Register all support components
dbLoadDatabase ("dbd/IRVB_LTU.dbd")
IRVB_LTU_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/dbSysMon.db","SYS=IRVB_LTU")
dbLoadRecords("db/sysTimeStamp.db", "SYS=IRVB_LTU")

#####################################
# LTU
#####################################
createTaskCLTU("IRVB", "/dev/tss/cltu.0", "1", "0", "126")
dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=IRVB")
dbLoadTemplate("db/IRVB_CLTU_CFG.template")
#####################################

cd ${TOP}/iocBoot/${IOC}
iocInit

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

