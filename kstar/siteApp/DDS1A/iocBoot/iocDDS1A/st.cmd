#!../../bin/linux-x86/DDS1A

< envPaths
< sfwEnv



## Register all support components
dbLoadDatabase("../../dbd/DDS1A.dbd",0,0)
DDS1A_registerRecordDeviceDriver(pdbbase) 


cd ${TOP}/iocBoot/${IOC}

createMainTask("DDS1A")

#define CARD_MODE_SINGLE      1
#define CARD_MODE_MASTER      2
#define CARD_MODE_SLAVE       3
############ DevType,  Task name,  arg0(slot),   arg1(dir)
createDevice("ACQ196",   "B1",     "2",        "/root/rawData")
createDevice("ACQ196",   "B2",     "3",        "/media/data2")
createDevice("ACQ196",   "B3",     "5",        "/media/data3")
createDevice("ACQ196",   "B4",     "7",        "/media/data1")


cd ${TOP}

## Load record instances
dbLoadRecords "db/ACQ196_cfg.db","SYS=DDS1A, BX=B1"
dbLoadRecords "db/ACQ196_cfg.db","SYS=DDS1A, BX=B2"
dbLoadRecords "db/ACQ196_cfg.db","SYS=DDS1A, BX=B3"
dbLoadRecords "db/ACQ196_cfg.db","SYS=DDS1A, BX=B4"
dbLoadTemplate "db/dbB1_ch.template"
dbLoadTemplate "db/dbB2_ch.template"
dbLoadTemplate "db/dbB3_ch.template"
dbLoadTemplate "db/dbB4_ch.template"


dbLoadRecords "db/sysTimeStamp.db", "SYS=DDS1A"
dbLoadRecords "db/dbSysMon.db","SYS=DDS1A"
dbLoadRecords "db/dbSFW.db", "SYS=DDS1A"

createTaskCLTU("DDS1A", "/dev/tss/cltu.0", "1", "0", "113") 
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=DDS1A" 
dbLoadTemplate "db/DDS1A_CLTU_CFG.template" 


#traceIocInit()

errlogInit()
iocInit()

#        ( Task,   filter,      value)
setMDSplus("B4",  "REMOTE_IP",   "172.17.100.202:8000")
setMDSplus("B4",  "REMOTE_NAME", "r_electron")
setMDSplus("B4",  "LOCAL_IP",   "172.17.102.60:8000")
setMDSplus("B4",  "LOCAL_NAME", "electron")

#seq sncLogicSeq

#####################################
# IRMIS crawler [shbaek, 2010.01.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

dbpf DDS1A_B4:SAMPLING_RATE 100000
dbpf DDS1A_B4:START_TIME    (-0.5)
dbpf DDS1A_B4:STOP_TIME    (20)


