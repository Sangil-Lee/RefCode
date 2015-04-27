#!../../bin/linux-x86/RMP

## You may have to change RMP to something else
## everywhere it appears in this file


< envPaths
< sfwEnv

#epicsEnvSet(CLTU_IOC_DEBUG, "2")

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/RMP.dbd"
RMP_registerRecordDeviceDriver pdbbase


## system name
createMainTask("RMP")

## DevType,   TaskName,  arg0,   arg1,  arg2
createDevice("16aiss8ao4", "16ADIO")
createDevice("Dio24", "DIO24")
createDevice("Rfm5565", "RFM", "/dev/rfm2g0")

## Load record instances
dbLoadRecords("db/dbRfm5565.vdb", "SYS=RMP, BX=RFM")
dbLoadRecords("db/db16aiss8ao4.vdb", "SYS=RMP, BX=16ADIO")
#####dbLoadTemplate("db/dbGPIO_OUT.template") #don't use it
dbLoadTemplate("db/dbGPIO_IN.template")
dbLoadTemplate("db/dbPattern.template")
#dbLoadTemplate("db/dbWaveform_aio.template") # it makes fault in IOC...2012.09.14
dbLoadRecords("db/dbDio24.vdb", "SYS=RMP, BX=DIO24")
dbLoadTemplate("db/db16AI_rt.template")

dbLoadRecords("db/dbSFW.db", "SYS=RMP")
dbLoadRecords("db/sysTimeStamp.db", "SYS=RMP")
dbLoadRecords("db/dbSysMon.db","SYS=RMP") # wrong in RT patch!


## sysName, devFile, version, mode, id
createTaskCLTU("RMP", "/dev/tss/cltu.0", "1", "0", "128");
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=RMP"
dbLoadTemplate("db/RMP_CLTU_CFG.template")


## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
seq sncRuntimeSeq

# just test about unstability of caGateway. 2013. 7. 10
#seq sncMPSWatchdog



dbpf RMP_16ADIO_CHANNEL_MASK 0xff
#dbpf RMP_SAMPLING_RATE 1000  --> changed at 2012. 2. 6  
dbpf RMP_SAMPLING_RATE 10000

dbpf RMP_16ADIO_AI_USE_EXT_CLK 1
dbpf RMP_16ADIO_AO_USE_EXT_CLK 1

# make 16ADIO GPIO output mode only for timestamp test
#dbpf RMP_16ADIO_GPIO_0_MODE 1
#dbpf RMP_16ADIO_GPIO_1_MODE 1


# make DIO24 all output mode
dbpf RMP_DIO24_DIR_PORT_A 1
dbpf RMP_DIO24_DIR_PORT_B 1
dbpf RMP_DIO24_DIR_PORT_C 1

dbpf RMP_T_CFG_A 2
dbpf RMP_T_CFG_B 0
dbpf RMP_T_CFG_C 0
dbpf RMP_T_CFG_D 2

dbpf RMP_M_CFG_A 2
dbpf RMP_M_CFG_B 2
dbpf RMP_M_CFG_C 0
dbpf RMP_M_CFG_D 0

dbpf RMP_B_CFG_A 0
dbpf RMP_B_CFG_B 2
dbpf RMP_B_CFG_C 2
dbpf RMP_B_CFG_D 0


#####################################
# IRMIS crawler [woong, 2011.05.03]
#####################################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#####################################
# TX setup. 1KHz, clock, F
dbpf RMP_LTU_P0_ENABLE  1
dbpf RMP_LTU_P0_ActiveLow  1
dbpf RMP_LTU_P0_MODE  0
dbpf RMP_LTU_P0_SEC0_T0_BLIP  (-14) 
dbpf RMP_LTU_P0_SEC0_T1_BLIP  150
dbpf RMP_LTU_P0_SEC0_CLOCK  1000
dbpf RMP_LTU_SOFT_SETUP_P0  1

# RX setup. 10KHz, clock, F
dbpf RMP_LTU_P1_ENABLE  1
dbpf RMP_LTU_P1_ActiveLow  1
dbpf RMP_LTU_P1_MODE  0
dbpf RMP_LTU_P1_SEC0_T0_BLIP  (-14) 
dbpf RMP_LTU_P1_SEC0_T1_BLIP  150 
dbpf RMP_LTU_P1_SEC0_CLOCK  10000 
dbpf RMP_LTU_SOFT_SETUP_P1  1




