## Example vxWorks startup file

sysClkRateSet(1000)

## The following is needed if your board support package doesn't at boot time
## automatically cd to the directory containing its startup script
cd "/usr/local/epics/siteApp/CCS_T/iocBoot/iocCCS_T"

< cdCommands.fix

< ../nfsCommands

cd topbin
## You may have to change t to something else
## everywhere it appears in this file

putenv("EPICS_TIMEZONE=KOR::-540:000000:000000")
putenv("EPICS_TS_NTP_INET=172.17.100.11")
#putenv("EPICS_IOC_LOG_INET=172.17.101.201")
#putenv("TSdriverDebug=3")

# get TSS start signal   

#putenv("EPICS_CA_ADDR_LIST=172.17.100.192 172.17.100.193 172.17.100.199 127.0.0.1")
#putenv("EPICS_CA_AUTO_ADDR_LIST=no")




ld < CCS_T.munch

#errlogInit 10000
#iocLogInit

## This drvTS initializer is needed if the IOC has a hardware event system
#TSinit
#TSconfigure(0,10,1000,123,123,250,1)
#TSconfigure(0,0,0,0,0,0,1)
#TSinit

#
#
putenv "EPICS_CA_MAX_ARRAY_BYTES=16384"

## Register all support components
cd top
dbLoadDatabase("dbd/CCS_T.dbd",0,0)
CCS_T_registerRecordDeviceDriver(pdbbase)

## Load record instances
dbLoadTemplate("db/ccs_vxStats.template")
dbLoadTemplate("db/ccs_ClusterLine.template")
dbLoadTemplate("db/ccs_mainScreenSoftPVs.template")
dbLoadTemplate("db/ccs_watchdogSoftPVs.template")
dbLoadTemplate("db/ccs_MPSMeasured.template")
dbLoadTemplate("db/ccs_EIPtest.template")
#dbLoadTemplate("db/ccs_pv_relay.template")

dbLoadRecords("db/ccs_interlockOutputModule.vdb")
dbLoadRecords("db/ccs_interlockInputModule.vdb")
dbLoadRecords("db/ccs_interlockOutputModule_heating.db")
dbLoadRecords("db/ccs_pcsControlStatus.vdb")
dbLoadRecords("db/ccs_ShotNumber.db")
dbLoadRecords("db/ccs_pcsRTMode.db")
dbLoadRecords("db/sysTimeStamp.db", "SYS=CCS")

# 2013. 6. 4 move to CCS_J temporary
#dbLoadRecords("db/ccs_Interlock_T.db")
# 2013. 7. 9 move to CCS_J
#dbLoadTemplate("db/ccs_EIP_fast_interlock.template")

## Set this to see messages from mySub
#mySubDebug = 1

vmic2534_init(0, 0x0c00)
vmic2534_init(1, 0x0b00)


drvEtherIP_init()
drvEtherIP_define_PLC("INTL_PLC0", "172.17.101.211",0)
EIP_verbosity = 0


cd startup

iocsh

#vmivme5565_init(0, 0x88000000, 0x00010000, 2, 255) # disable 2013.08.14

#
# RFM partitioning for MPS<->PCS communication
#
#rfmPartition("RFM_reserved", 0x95fe0000, 0x0000, 0x100, 0x01abff00)
#rfmPartition("PCS_command",  0x95fe0000, 0x0100, 0x90, 0x01ab01ab)

#rfmPartition("MPS_PF1",      0x95fe0000, 0x300, 0x40, 0x01ab0001)
#rfmPartition("MPS_PF2",      0x95fe0000, 0x380, 0x40, 0x01ab0003)
#rfmPartition("MPS_PF3U",      0x95fe0000, 0x400, 0x40, 0x01ab0005)
#rfmPartition("MPS_PF3L",      0x95fe0000, 0x440, 0x40, 0x01ab0006)

#rfmPartition("MPS_PF4U",      0x95fe0000, 0x480, 0x40, 0x01ab0007)
#rfmPartition("MPS_PF4L",      0x95fe0000, 0x4C0, 0x40, 0x01ab0008)

#rfmPartition("MPS_PF5U",      0x95fe0000, 0x500, 0x40, 0x01ab0009)
#rfmPartition("MPS_PF5L",      0x95fe0000, 0x540, 0x40, 0x01ab000a)

#rfmPartition("MPS_PF6U",      0x95fe0000, 0x580, 0x40, 0x01ab000b)
#rfmPartition("MPS_PF6L",      0x95fe0000, 0x5C0, 0x40, 0x01ab000c)

#rfmPartition("MPS_PF7",      0x95fe0000, 0x600, 0x40, 0x01ab000d)

#rfmPartition("MPS_TF",       0x95fe0000, 0x680, 0x20, 0x01ab000f)

#rfmPartition("MPS_IVC",       0x95fe0000, 0x6A0, 0x20, 0x01cd0001)

#rfmPartition("MPS_Reserved0", 0x95fe0000, 0x0900, 0x400, 0x02bc1122)
#rfmPartition("RFM_Reserved1", 0x95fe0000, 0x0e00, 0x1ff200, 0x02bc1111)

#rfmPartition("Heating",      0x95fe0000, 0x00200000, 0x200000, 0x02bc1112)
#rfmPartition("Fueling",      0x95fe0000, 0x00400000, 0x200000, 0x02bc1113)
#rfmPartition("Diag",         0x95fe0000, 0x00600000, 0x800000, 0x02bc1114)
#rfmPartition("RFM_Reserved2", 0x95fe0000, 0x00e00000, 0x02800000, 0x02bc1115)
#rfmPartition("Timing",        0x95fe0000, 0x03600000, 0x00200000, 0x02bc1116)
#rfmPartition("CCS",           0x95fe0000, 0x03800000, 0x00200000, 0x01abfe01)
#rfmPartition("PCS_Share",     0x95fe0000, 0x04000000, 0x00100000, 0xabdcabdc)
#rfmPartition("PCS_Internal",  0x95fe0000, 0x04100000, 0x03d00000, 0x01abcdef)
#rfmPartition("TCP/IP",        0x95fe0000, 0x07e00000, 0x00200000, 0xfedbadea)

#
# RFM partitioning for PCS Internal
#
#rfmPartition("PCS_share",    0x95fe0000, 0x04000000, 0x01000000, 0xabcdabcd)

#
# RFM partitioning for TCP/IP over the rfm
#
#rfmPartition("TCP/IP",       0x95fe0000, 0x07f00000, 0x00100000, 0xfedbadea)

#drvGenericSerialConfigure("TMON0", "172.17.101.208:6341", 0, 0)
#drvGenericSerialConfigure("TMIN1", "172.17.101.209:6431", 0, 0)

#drvcFP20x0_init("TMON0", 3., 3., 5., 100)
#drvcFP20x0_init("TMON1", 3., 3., 5., 100)

iocInit()

## Start any sequence programs
#seq &sncExample,"user=khkim"



seq &sncPCSWatchdog
# 2012. 8. 29 move to CCS_J
# 2013. 6. 12 return from CCS_J
# 2013. 7. 9 move to CCS_J again
#seq &sncMPSWatchdog

seq &sncPF1Watchdog
seq &sncPF2Watchdog
seq &sncPF3UWatchdog
seq &sncPF3LWatchdog
seq &sncPF4UWatchdog
seq &sncPF4LWatchdog
seq &sncPF5UWatchdog
seq &sncPF5LWatchdog
seq &sncPF6UWatchdog
seq &sncPF6LWatchdog
seq &sncPF7Watchdog
seq &sncTFWatchdog
seq &sncIVCWatchdog

#seq &sncNTPUpdate

dbpf("CCS:R_CA", 1)
dbpf("CCS:R_FD", 1)
dbpf("CCS:R_Load", 1)
dbpf("CCS:R_Mem", 1)

## 2011. 6. 13.  normal high
dbpf("CCS_INTL_ECCD_STOP", 1)
dbpf("CCS_INTL_ECH_STOP", 1)
dbpf("CCS_INTL_ICRH_STOP", 1)
dbpf("CCS_INTL_NBI1_STOP", 1)

## 2013. 7. 11 for pcs connection
dbpf("PCS_LS_STATUS", 1)


dbl > pvlist


