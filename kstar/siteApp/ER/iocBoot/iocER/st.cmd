#!../../bin/linux-x86/ER


< envPaths
< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/ER.dbd"
ER_registerRecordDeviceDriver pdbbase


createMainTask("ER")

############ DevType,   Task name,  arg0,   arg1,  arg3
createDevice("NI5412",  "5412_1",  "Dev11")
#createDevice("NI5412",  "5412_2",  "Dev12")
createDevice("ZT530",  "ZT530_1",  "PXI14::12::INSTR")
createDevice("NI5122",  "5122_1",  "Dev15")
createDevice("NI5122",  "5122_2",  "Dev14")
createDevice("NI5122",  "5122_3",  "Dev13")


############# DevName,     ID,    arg0,   arg1,   arg3
createChannel("5412_1",   "0")
#createChannel("5412_2",   "0")

createChannel("ZT530_1",   "0")
createChannel("ZT530_1",   "1")

createChannel("5122_1",   "0")
createChannel("5122_1",   "1")

createChannel("5122_2",   "0")
createChannel("5122_2",   "1")

createChannel("5122_3",   "0")
createChannel("5122_3",   "1")


## Load record instances
dbLoadRecords "db/niFgen.db", "SYS=ER, DEV=5412_1, CHX=0"
#dbLoadRecords "db/niFgen.db", "SYS=ER, DEV=5412_2, CHX=0"

dbLoadRecords "db/zt530.db", "SYS=ER, DEV=ZT530_1"
dbLoadRecords "db/zt530_ch.db", "SYS=ER, DEV=ZT530_1, CHX=0"
dbLoadRecords "db/zt530_ch.db", "SYS=ER, DEV=ZT530_1, CHX=1"

dbLoadRecords "db/niScope.db", "SYS=ER, DEV=5122_1"
dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5122_1, CHX=0, TAG=IF_QX"
dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5122_1, CHX=1, TAG=VCO_QX"

dbLoadRecords "db/niScope.db", "SYS=ER, DEV=5122_2"
dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5122_2, CHX=0, TAG=IF_VX"
dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5122_2, CHX=1, TAG=VCO_VX"

dbLoadRecords "db/niScope.db", "SYS=ER, DEV=5122_3"
dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5122_3, CHX=0, TAG=IF_WX"
dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5122_3, CHX=1, TAG=VCO_WX"

#dbLoadRecords "db/niScope.db", "SYS=ER, DEV=5124_1"
#dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5124_1, CHX=0"
#dbLoadRecords "db/niScope_ch.db", "SYS=ER, DEV=5124_1, CHX=1"


dbLoadRecords "db/dbSFW.db", "SYS=ER"
dbLoadRecords "db/soft_CCS_QUERY_READY.vdb", "SYS=ER"

dbLoadRecords "db/sysTimeStamp.db", "SYS=ER"
dbLoadRecords "db/dbSysMon.db", "SYS=ER"


createTaskCLTU("ER", "/dev/tss/cltu.0", "1", "0", "117")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=ER"
dbLoadTemplate "db/ER_CLTU_CFG.template"
dbLoadTemplate "db/ER_MTRIG_P0_CFG.template"
dbLoadTemplate "db/ER_MTRIG_P2_CFG.template"
dbLoadTemplate "db/ER_MTRIG_P3_CFG.template"


# Set this to see messages from mySub
#var mySubDebug 1

#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=rootHost"
#seq sncLogicSeq

dbpf ER_5412_1:ENABLE_OUTPUT       1
dbpf ER_ZT530_1_CH0:ENABLE_OUTPUT  1
dbpf ER_ZT530_1_CH1:ENABLE_OUTPUT  1

dbpf ER_ZT530_1_CH0:WF_FILENAME  "LE_Vround25.bin"
dbpf ER_ZT530_1_CH0:AMPLITUDE 20
dbpf ER_ZT530_1_CH0:ARB_RATE 40000

dbpf ER_ZT530_1_CH1:WF_FILENAME  "LE_Wround25.bin"
dbpf ER_ZT530_1_CH1:AMPLITUDE 20
dbpf ER_ZT530_1_CH1:ARB_RATE 40000


dbpf ER_5122_1_CH1:INPUT_RANGE 20
dbpf ER_5122_2_CH1:INPUT_RANGE 20
dbpf ER_5122_3_CH1:INPUT_RANGE 20

dbpf ER_5122_1_CH0:CUTOFF_FREQ 40000000
dbpf ER_5122_1_CH1:CUTOFF_FREQ 40000000
dbpf ER_5122_2_CH0:CUTOFF_FREQ 40000000
dbpf ER_5122_2_CH1:CUTOFF_FREQ 40000000
dbpf ER_5122_3_CH0:CUTOFF_FREQ 40000000
dbpf ER_5122_3_CH1:CUTOFF_FREQ 40000000

dbpf ER_5122_1:RECORD_LENGTH 65536
dbpf ER_5122_2:RECORD_LENGTH 65536
dbpf ER_5122_3:RECORD_LENGTH 65536


dbpf ER_LTU_P0_SEC0_T0 1.000
dbpf ER_LTU_P0_SEC0_T1 1.001
dbpf ER_LTU_P1_SEC0_T1 0.001
dbpf ER_LTU_P2_SEC0_T0 1.000
dbpf ER_LTU_P2_SEC0_T1 1.001
dbpf ER_LTU_P3_SEC0_T0 1.000
dbpf ER_LTU_P3_SEC0_T1 1.001



#####################################
# IRMIS crawler [shbaek, 2010.06.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################




