#!../../bin/linux-x86_64/HICS

## You may have to change HICS to something else
## everywhere it appears in this file

< envPaths
< sfwEnv

epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "HICS.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/HICS.dbd"
HICS_registerRecordDeviceDriver pdbbase

## printing debug messages to console
kuSetPrintLevel 3
kuSetLogFile ("/usr/local/epics/siteApp/HICS/iocBoot/iocHICS/HICS.log")

createMainTask("HICS")

## DevType,   Task(Dev)Name,  arg0,   arg1,  arg2 
createDevice("EC1_ANT",  "EC1_ANT",  "xxx")

## Load record instances
dbLoadRecords("db/dbSFW.db"       , "SYS=HICS")
dbLoadRecords("db/dbSysMon.db"    , "SYS=HICS")
dbLoadRecords("db/sysTimeStamp.db", "SYS=HICS")

dbLoadRecords ("db/HICS_EC1_ANT.db", "SYS=HICS, DEV=EC1_ANT")
dbLoadRecords ("db/HICS_ECH_PWR.db", "SYS=HICS, DEV=EC2")

# ------------------------------------------------
# KSTAR PV Archive
# ------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
< kupaEnv

cd ${TOP}/iocBoot/${IOC}
iocInit

#####################################
# Set CPU affinity and priority
#####################################

#epicsThreadSetCPUAffinityByName EC1_ANT 5
#epicsThreadSetPosixPriorityByName EC1_ANT 92 SCHED_FIFO
#epicsThreadShowAll 1

#####################################
# Set initial values
#####################################

#dbpf HICS_EC1_ANT_STS_SIM	1		# Simulation mode
#dbpf HICS_EC1_ANT_CMD_RATE	1000	# 1 KHz
#dbpf HICS_EC1_ANT_STS_RATE	1000	# 1 KHz
#dbpf HICS_EC1_ANT_CMD_RATE	100		# 100 Hz / 10 ms

dbpf HICS_ECH_PWR_CTRL_TORMAG       2.00
dbpf HICS_ECH_PWR_CTRL_DTIME        0.50
dbpf HICS_ECH_PWR_CTRL_TARGMANUAL   0.00
dbpf HICS_ECH_PWR_CTRL_TARGTEGAIN   2.00
dbpf HICS_ECH_PWR_CTRL_TARGPOS1     0.10
dbpf HICS_ECH_PWR_CTRL_TARGPOS2     0.20
dbpf HICS_ECH_PWR_CTRL_TARGPOS3     0.30
dbpf HICS_ECH_PWR_CTRL_TARGPOS4     0.40
dbpf HICS_ECH_PWR_CTRL_TARGPOS5     0.50
dbpf HICS_ECH_PWR_CTRL_TARGVAL1     2.50
dbpf HICS_ECH_PWR_CTRL_TARGVAL2     2.30
dbpf HICS_ECH_PWR_CTRL_TARGVAL3     2.00
dbpf HICS_ECH_PWR_CTRL_TARGVAL4     1.50
dbpf HICS_ECH_PWR_CTRL_TARGVAL5     1.00
dbpf HICS_ECH_PWR_CTRL_FLATXEON     0.00
dbpf HICS_ECH_PWR_CTRL_FLATXEVAL    1.00
dbpf HICS_ECH_PWR_CTRL_XECOEFA      1.00
dbpf HICS_ECH_PWR_CTRL_XECOEFB      3.00
dbpf HICS_ECH_PWR_CTRL_XECOEFC      6.00
dbpf HICS_ECH_PWR_CTRL_XECOEFD      9.00
dbpf HICS_ECH_PWR_CTRL_FLATNEVAL    2.50
dbpf HICS_ECH_PWR_CTRL_ECHPOS       0.20

#####################################
# IRMIS crawler
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################
