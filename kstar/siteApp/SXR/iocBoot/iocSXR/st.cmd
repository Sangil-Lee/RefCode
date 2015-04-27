#!../../bin/linux-x86/SXR

< envPaths
#epicsEnvSet("EPICS_CAS_INTF_ADDR_LIST", "172.17.102.60") FILD-102.38
epicsEnvSet("EPICS_CA_ADDR_LIST","172.17.101.200 172.17.101.201 172.17.101.210 172.17.102.38 172.17.102.120 127.0.0.1")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")
epicsEnvSet("EPICS_IOC_LOG_FILE_NAME", "SXR.log")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","3276800")


#epicsEnvSet("EPICS_TIMEZONE", "KOR::-540:000000:000000")
#epicsEnvSet("EPICS_TS_NTP_INET", "172.17.100.119")

< mdsplusEnv

## Register all support components
dbLoadDatabase("../../dbd/SXR.dbd",0,0)
SXR_registerRecordDeviceDriver(pdbbase) 


cd ${TOP}/iocBoot/${IOC}


createAdminTask("Admin_SXR")

#           ( Type,   Name,  Slot, Channel Name))
createDevice("NI6123", "Dev7", "Dev7/ai0", "/Dev7/PFI0", "CH01")
createDevice("NI6123", "Dev6", "Dev6/ai0", "/Dev6/PFI0", "CH09")
createDevice("NI6123", "Dev5", "Dev5/ai0", "/Dev5/PFI0", "CH17")
createDevice("NI6123", "Dev4", "Dev4/ai0", "/Dev4/PFI0", "CH25")
createDevice("NI6123", "Dev3", "Dev3/ai0", "/Dev3/PFI0", "CH33")
createDevice("NI6123", "Dev2", "Dev2/ai0", "/Dev2/PFI0", "CH41")
createDevice("NI6123", "Dev1", "Dev1/ai0", "/Dev1/PFI0", "CH49")
createDevice("NI6123", "Dev9", "Dev9/ai0", "/Dev9/PFI0", "CH57")

createDevice("IT6322", "IT6322", "", "CH24")
createDevice("NI6259", "NI6259", "/Dev8/ai0", "/Dev8/PFI0", "BLOCK1")
#cd ${TOP}

## Load record instances
dbLoadRecords("../../db/NI6123.db","SYS=SXR, DEV=NI6123")
dbLoadRecords("../../db/IT6322.db","SYS=SXR, DEV=NI6123")

dbLoadRecords("../../db/NI6259.db","SYS=SXR, DEV=BLOCK1")
dbLoadRecords("../../db/NI6259.db","SYS=SXR, DEV=BLOCK2")
dbLoadRecords("../../db/NI6259.db","SYS=SXR, DEV=BLOCK3")
dbLoadRecords("../../db/NI6259.db","SYS=SXR, DEV=BLOCK4")

dbLoadRecords("../../db/dbAdmin.db", "SYS=SXR")

#TG_dbLoadRecords("../../db/dbSysMon.db","SYS=SXR")
dbLoadRecords("../../db/sysTimeStamp.db", "SYS=SXR")

cd ${TOP}

#createTaskCLTU("SXR", "/dev/tss/cltu.0", "1", "0", "109") 

#dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=SXR" 
#dbLoadTemplate "db/SXR_CLTU_CFG.template"

#traceIocInit()

errlogInit()
iocInit()

#seq sncLogicSeq

#####################################
# IRMIS crawler [shbaek, 2010.01.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
#system("./caSysProc &")
cd ${TOP}/iocBoot/${IOC}
#####################################

