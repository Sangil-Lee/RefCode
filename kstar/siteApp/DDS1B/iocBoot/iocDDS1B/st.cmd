#!../../bin/linux-x86/DDS1B

< envPaths
< sfwEnv



## Register all support components
dbLoadDatabase("../../dbd/DDS1B.dbd",0,0)
DDS1B_registerRecordDeviceDriver(pdbbase) 


cd ${TOP}/iocBoot/${IOC}

createMainTask("DDS1B")

#define CARD_MODE_SINGLE      1
#define CARD_MODE_MASTER      2
#define CARD_MODE_SLAVE       3
############ DevType,  Task name,  arg0(slot),   arg1(dir)
createDevice("ACQ196",   "B5",     "2",        "/media/data1")
createDevice("ACQ196",   "B6",     "3",        "/media/data2")


cd ${TOP}

## Load record instances
dbLoadRecords "db/ACQ196_cfg.db","SYS=DDS1B, BX=B5"
dbLoadRecords "db/ACQ196_cfg.db","SYS=DDS1B, BX=B6"
dbLoadTemplate "db/dbB5_ch.template"
dbLoadTemplate "db/dbB6_ch.template"


dbLoadRecords "db/sysTimeStamp.db", "SYS=DDS1B"
dbLoadRecords "db/dbSysMon.db","SYS=DDS1B"
dbLoadRecords "db/dbSFW.db", "SYS=DDS1B"

createTaskCLTU("DDS1B", "/dev/tss/cltu.0", "1", "0", "114") 
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=DDS1B" 
dbLoadTemplate "db/DDS1B_CLTU_CFG.template" 


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
cd ${TOP}/iocBoot/${IOC}
#####################################


