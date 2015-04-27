#!../../bin/linux-x86/TS2

< envPaths

< sfwEnv

## Register all support components
dbLoadDatabase("../../dbd/TS2.dbd",0,0)
TS2_registerRecordDeviceDriver(pdbbase) 


cd ${TOP}/iocBoot/${IOC}


createMainTask("TS2")

############ DevType,  Task name,  arg0(addr),   arg1,  arg2
createDevice("V792N",   "B0",      "0xaa00")
createDevice("V792N",   "B1",      "0xbb00")
createDevice("V792N",   "B2",      "0xcc00")
createDevice("V792N",   "B3",      "0xdd00")

#createTaskCLTU("TS2", "/dev/tss/cltu.0", "0")
#createTaskCLTU("TS2", "/dev/tss/cltu.0", "1", "1", "23")

cd ${TOP}
 
## Load record instances

dbLoadTemplate "db/ts_value_B0.substitutions"
dbLoadTemplate "db/ts_value_B1.substitutions"
dbLoadTemplate "db/ts_value_B2.substitutions"
dbLoadTemplate "db/ts_value_B3.substitutions"
dbLoadTemplate "db/ts_tag_B0.substitutions"
dbLoadTemplate "db/ts_tag_B1.substitutions"
dbLoadTemplate "db/ts_tag_B2.substitutions"
dbLoadTemplate "db/ts_tag_B3.substitutions"
#dbLoadTemplate "db/ts_result.substitutions"

dbLoadRecords("db/TS_cfg.db","SYS=TS2, BX=B0")
dbLoadRecords("db/TS_cfg.db","SYS=TS2, BX=B1")
dbLoadRecords("db/TS_cfg.db","SYS=TS2, BX=B2")
dbLoadRecords("db/TS_cfg.db","SYS=TS2, BX=B3")
dbLoadRecords("db/TS_get_TS1.db","SYS=TS2")
dbLoadRecords("db/TS_shot.db","SYS=TS2")

dbLoadRecords("db/sysTimeStamp.db", "SYS=TS2")
dbLoadRecords("db/dbSysMon.db","SYS=TS2")

dbLoadRecords("db/dbSFW.db", "SYS=TS2")




#traceIocInit()

#errlogInit()
iocInit()


#seq sncRuntimeSeq

#setup initial channel mask.
dbpf TS2_B0_CHANNEL_MASK 0x7fff
dbpf TS2_B1_CHANNEL_MASK 0x7fff
dbpf TS2_B2_CHANNEL_MASK 0x7fff
dbpf TS2_B3_CHANNEL_MASK 0x7fff

dbpf TS2_B0_SET_PEDESTAL 65
dbpf TS2_B1_SET_PEDESTAL 65
dbpf TS2_B2_SET_PEDESTAL 65
dbpf TS2_B3_SET_PEDESTAL 65

################################
# very important... to calculate time stamp of MDS data
dbpf TS2_SAMPLING_RATE 100

#####################################
# IRMIS crawler [shbaek, 2010.01.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

