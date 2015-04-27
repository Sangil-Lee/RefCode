#!../../bin/linux-x86/VMS_CR

## You may have to change VMS_CR to something else
## everywhere it appears in this file

< envPaths
< ../envCommonIPTables

## prepare env variables and ports definitions
cd ${TOP}/iocBoot/${IOC}
< envPorts
< envVars

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/VMS_CR.dbd")
VMS_CR_registerRecordDeviceDriver(pdbbase)


## Set this to see messages from mySub
#var mySubDebug 1
epicsThreadSleep 2.

#PLC Loading
drvEtherIP_init()
drvEtherIP_define_PLC("plc0", "172.17.101.11",0)
EIP_verbosity(2)
EIP_buffer_limit(494)


###################################################
# SRSRGA                                          #
##################################################

dbLoadRecords("db/VMS_CR_INIT.db")
dbLoadRecords("db/VMS_CR_FL.db")
dbLoadTemplate("db/VMS_CR_READ.template")


drvGenericSerialConfigure("L11","172.17.101.42:4002",0,0)

#########################################################



# load record instance
cd ${TOP}/iocBoot/${IOC}
< dbLoads

# initialize drivers
cd ${TOP}/iocBoot/${IOC}
< initDrivers

# asSetFilename("accessSecurity")

iocInit()

epicsThreadSleep 2.


## db list writes to below file ##
#dbl > VMS_CR.txt

## Start any sequence programs
seq SRSRGA_ON 

#< seqLoads


#########################
# IRMIS crawler [shbaek, 2010.01.22]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
