#!../../bin/linux-x86/TS1

< envPaths

< sfwEnv

## Register all support components
dbLoadDatabase("../../dbd/TS1.dbd",0,0)
TS1_registerRecordDeviceDriver(pdbbase) 


cd ${TOP}/iocBoot/${IOC}


createMainTask("TS1")

############ DevType,  Task name,  arg0(addr),   arg1,  arg2
createDevice("V792",   "B0",      "0xaa00")
createDevice("V792",   "B1",      "0xbb00")
createDevice("V792",   "B2",      "0xcc00")
createDevice("V792",   "B3",      "0xdd00")


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

dbLoadRecords("db/TS_cfg.db","SYS=TS1, BX=B0")
dbLoadRecords("db/TS_cfg.db","SYS=TS1, BX=B1")
dbLoadRecords("db/TS_cfg.db","SYS=TS1, BX=B2")
dbLoadRecords("db/TS_cfg.db","SYS=TS1, BX=B3")
dbLoadRecords("db/TS_shot.db","SYS=TS1")

dbLoadRecords("db/sysTimeStamp.db", "SYS=TS1")
dbLoadRecords("db/dbSysMon.db","SYS=TS1")

dbLoadRecords("db/dbSFW.db", "SYS=TS1")

createTaskCLTU("TS", "/dev/tss/cltu.0", "1", "0", "127")
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=TS"
dbLoadTemplate "db/TS_CLTU_CFG.template" 


#traceIocInit()

#errlogInit()
iocInit()


#seq sncRuntimeSeq
#seq sncSeqTSSC

#setup initial channel mask.
dbpf TS1_B0_CHANNEL_MASK 0x7fff7fff
dbpf TS1_B1_CHANNEL_MASK 0x7fff7fff
dbpf TS1_B2_CHANNEL_MASK 0x7fff7fff
dbpf TS1_B3_CHANNEL_MASK 0x7fff7fff

dbpf TS1_B0_SET_PEDESTAL 65
dbpf TS1_B1_SET_PEDESTAL 65
dbpf TS1_B2_SET_PEDESTAL 65
dbpf TS1_B3_SET_PEDESTAL 65

################################
# very important... to calculate time stamp of MDS data
dbpf TS1_SAMPLING_RATE 100



#####################################
# IRMIS crawler [shbaek, 2010.01.25]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################

