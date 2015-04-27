#!../../bin/linux-x86/NBI_DAQ

## You may have to change NBI_DAQ to something else
## everywhere it appears in this file

epicsEnvSet("EPICS_CA_ADDR_LIST", "172.17.121.243 172.17.121.187 172.17.101.200 172.17.101.201 172.17.101.210 127.0.0.1")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "no")

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/NBI_DAQ.dbd")
NBI_DAQ_registerRecordDeviceDriver(pdbbase)

## Load record instances
#dbLoadRecords("db/dbSysMon.db","SYS=NBIDAQ")
dbLoadRecords("db/dbSysMon.db","SYS=NB1_DAQ")

## DTACQ HeartBeat
dbLoadRecords("db/NB1_DTACQ_HeartBeat.db")
dbLoadRecords("db/NB1_SHOT.db","PORT=L0,A=0")

dbLoadTemplate("db/devMdsGainA.template")
dbLoadTemplate("db/devMdsGainB.template")

## PNB node
dbLoadRecords("db/devMdsEtcGain.db")

dbLoadRecords("db/devAcq196_1.db","P=NB1_DTACQ1,PORT=L0,A=0")
dbLoadRecords("db/devAcq196_2.db","P=NB1_DTACQ2,PORT=L0,A=1")

dbLoadTemplate("db/devFilaGainAB.template")

## TC MDS
dbLoadRecords("db/devTCdatasave.db")

drvAsynAcq196Configure("DTACQ1", 0, 2)
drvAsynAcq196Configure("DTACQ2", 1, 3)

#drvAsynTCdatasaveConfigure("172.17.121.125", 0, 0)
#drvAsynTCdatasaveConfigure("172.17.121.126", 1, 0)
#drvAsynTCdatasaveConfigure("172.17.121.127", 2, 0)
#drvAsynTCdatasaveConfigure("172.17.121.128", 3, 0)
#drvAsynTCdatasaveConfigure("172.17.121.129", 4, 0)
#drvAsynTCdatasaveConfigure("172.17.121.48", 5, 0)
#drvAsynTCdatasaveConfigure("172.17.121.49", 6, 0)
drvAsynTCdatasaveConfigure("172.17.121.50", 7, 0)

drvMdsplusConfigure(2);

## mdsput -> 0 = NOT, 1 = PUT, 2 = Gain/Offset
# dtacqSetupChannel(int card, int ch, int mode, char *node)

dtacqSetupChannel(0, 0, 2, "NB11_VG1")
dtacqSetupChannel(0, 1, 2, "NB11_I0")
dtacqSetupChannel(0, 2, 1, "NB11_IG1A")
dtacqSetupChannel(0, 3, 2, "NB11_IG1")
dtacqSetupChannel(0, 4, 1, "NB11_IDK")
dtacqSetupChannel(0, 5, 2, "NB11_VG2")
dtacqSetupChannel(0, 6, 2, "NB11_IG2")
dtacqSetupChannel(0, 7, 1, "NB11_IR1")
dtacqSetupChannel(0, 8, 1, "NB11_IR2")
dtacqSetupChannel(0, 9, 2, "NB11_VG3")
dtacqSetupChannel(0, 10, 2, "NB11_IG3")
dtacqSetupChannel(0, 11, 2, "NB11_VARC")
dtacqSetupChannel(0, 12, 2, "NB11_IARC")
dtacqSetupChannel(0, 13, 2, "NB11_VF01")
dtacqSetupChannel(0, 14, 2, "NB11_VF02")
dtacqSetupChannel(0, 15, 2, "NB11_VF03")
dtacqSetupChannel(0, 16, 2, "NB11_VF04")
dtacqSetupChannel(0, 17, 2, "NB11_VF05")
dtacqSetupChannel(0, 18, 2, "NB11_VF06")
dtacqSetupChannel(0, 19, 2, "NB11_VF07")
dtacqSetupChannel(0, 20, 2, "NB11_VF08")
dtacqSetupChannel(0, 21, 2, "NB11_VF09")
dtacqSetupChannel(0, 22, 2, "NB11_VF10")
dtacqSetupChannel(0, 23, 2, "NB11_VF11")
dtacqSetupChannel(0, 24, 2, "NB11_VF12")
dtacqSetupChannel(0, 25, 2, "NB11_IF01")
dtacqSetupChannel(0, 26, 2, "NB11_IF02")
dtacqSetupChannel(0, 27, 2, "NB11_IF03")
dtacqSetupChannel(0, 28, 2, "NB11_IF04")
dtacqSetupChannel(0, 29, 2, "NB11_IF05")
dtacqSetupChannel(0, 30, 2, "NB11_IF06")
dtacqSetupChannel(0, 31, 2, "NB11_IF07")
dtacqSetupChannel(0, 32, 2, "NB11_IF08")
dtacqSetupChannel(0, 33, 2, "NB11_IF09")
dtacqSetupChannel(0, 34, 2, "NB11_IF10")
dtacqSetupChannel(0, 35, 2, "NB11_IF11")
dtacqSetupChannel(0, 36, 2, "NB11_IF12")
dtacqSetupChannel(0, 37, 1, "NB11_VBIAS")
dtacqSetupChannel(0, 38, 1, "NB11_IBIAS")
dtacqSetupChannel(0, 39, 2, "NB11_IFS")
dtacqSetupChannel(0, 40, 1, "NB11_IG2P")

dtacqSetupChannel(1, 0, 2, "NB12_VG1")
dtacqSetupChannel(1, 1, 2, "NB12_I0")
dtacqSetupChannel(1, 2, 1, "NB12_IG1A")
dtacqSetupChannel(1, 3, 2, "NB12_IG1")
dtacqSetupChannel(1, 4, 1, "NB12_IDK")
dtacqSetupChannel(1, 5, 2, "NB12_VG2")
dtacqSetupChannel(1, 6, 2, "NB12_IG2")
dtacqSetupChannel(1, 7, 1, "NB12_IR1")
dtacqSetupChannel(1, 8, 1, "NB12_IR2")
dtacqSetupChannel(1, 9, 2, "NB12_VG3")
dtacqSetupChannel(1, 10, 2, "NB12_IG3")
dtacqSetupChannel(1, 11, 2, "NB12_VARC")
dtacqSetupChannel(1, 12, 2, "NB12_IARC")
dtacqSetupChannel(1, 13, 2, "NB12_VF01")
dtacqSetupChannel(1, 14, 2, "NB12_VF02")
dtacqSetupChannel(1, 15, 2, "NB12_VF03")
dtacqSetupChannel(1, 16, 2, "NB12_VF04")
dtacqSetupChannel(1, 17, 2, "NB12_VF05")
dtacqSetupChannel(1, 18, 2, "NB12_VF06")
dtacqSetupChannel(1, 19, 2, "NB12_VF07")
dtacqSetupChannel(1, 20, 2, "NB12_VF08")
dtacqSetupChannel(1, 21, 2, "NB12_VF09")
dtacqSetupChannel(1, 22, 2, "NB12_VF10")
dtacqSetupChannel(1, 23, 2, "NB12_VF11")
dtacqSetupChannel(1, 24, 2, "NB12_VF12")
dtacqSetupChannel(1, 25, 2, "NB12_IF01")
dtacqSetupChannel(1, 26, 2, "NB12_IF02")
dtacqSetupChannel(1, 27, 2, "NB12_IF03")
dtacqSetupChannel(1, 28, 2, "NB12_IF04")
dtacqSetupChannel(1, 29, 2, "NB12_IF05")
dtacqSetupChannel(1, 30, 2, "NB12_IF06")
dtacqSetupChannel(1, 31, 2, "NB12_IF07")
dtacqSetupChannel(1, 32, 2, "NB12_IF08")
dtacqSetupChannel(1, 33, 2, "NB12_IF09")
dtacqSetupChannel(1, 34, 2, "NB12_IF10")
dtacqSetupChannel(1, 35, 2, "NB12_IF11")
dtacqSetupChannel(1, 36, 2, "NB12_IF12")
dtacqSetupChannel(1, 37, 1, "NB12_VBIAS")
dtacqSetupChannel(1, 38, 1, "NB12_IBIAS")
dtacqSetupChannel(1, 39, 2, "NB12_IFS")
dtacqSetupChannel(1, 40, 1, "NB12_IG2P")

## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncExample,"user=rootHost"
seq sncAutoArm

########################
# IRMIS crawler
########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

dbpf NB1_DTACQ1_VG1_A 20
dbpf NB1_DTACQ1_IG1_A 10
dbpf NB1_DTACQ1_I0_A  10
dbpf NB1_DTACQ1_VG2_A 10
dbpf NB1_DTACQ1_IG2_A 0.01 
dbpf NB1_DTACQ1_VG3_A 0.8
dbpf NB1_DTACQ1_IG3_A 5
dbpf NB1_DTACQ1_VARC_A 34
dbpf NB1_DTACQ1_IARC_A 125
dbpf NB1_DTACQ1_IF01_A 100
dbpf NB1_DTACQ1_VF01_A 5
dbpf NB1_DTACQ1_IFS_A  400
dbpf NB1_DTACQ1_PNB_B 0.8

dbpf NB1_DTACQ2_VG1_A 20
dbpf NB1_DTACQ2_IG1_A 10
dbpf NB1_DTACQ2_I0_A  10
dbpf NB1_DTACQ2_VG2_A 15
dbpf NB1_DTACQ2_IG2_A 0.01
dbpf NB1_DTACQ2_VG3_A 0.8
dbpf NB1_DTACQ2_IG3_A 5
dbpf NB1_DTACQ2_VARC_A 34
dbpf NB1_DTACQ2_IARC_A 125
dbpf NB1_DTACQ2_IF01_A 100
dbpf NB1_DTACQ2_VF01_A 10
dbpf NB1_DTACQ2_IFS_A  600
dbpf NB1_DTACQ2_PNB_B 0.7

dbpf NB1_DTACQ1_SetClock 3
dbpf NB1_DTACQ2_SetClock 3
