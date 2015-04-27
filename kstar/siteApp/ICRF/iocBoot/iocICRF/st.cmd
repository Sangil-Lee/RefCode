#!../../bin/linux-x86/ICRF

## You may have to change ICRF to something else
## everywhere it appears in this file

< envPaths

#######
# kkh #
#######
epicsEnvSet("EPICS_CAS_INTF_ADDR_LIST", "172.17.121.180")
#epicsEnvSet("EPICS_CA_ADDR_LIST", "172.17.102.60")
#epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "YES")
epicsEnvSet("EPICS_CA_ADDR_LIST", "172.17.102.60 172.17.101.200 172.17.101.201 172.17.101.101 172.17.100.107","172.17.100.130")
#epicsEnvSet("EPICS_CA_ADDR_LIST", "172.17.102.60 172.17.101.200 172.17.101.201 172.17.101.101 172.17.100.107","172.17.100.130","172.17.121.180")
#epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "YES")
epicsEnvSet(CLTU_IOC_DEBUG, "0")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/ICRF.dbd")
ICRF_registerRecordDeviceDriver(pdbbase)

epicsEnvSet(CLTU_IOC_DEBUG, "0") /* 3 */

## Load record instances
dbLoadRecords("db/temper_levdet.db")
dbLoadRecords("db/tune.db")
dbLoadRecords("db/2MW.db")
dbLoadRecords("db/lowRF.db")
dbLoadRecords("db/interlock.db")
# MAXARR is limited by 100,000 at drv level.
# Maximum MAXARR is 4MByte of unsigned short
dbLoadRecords("db/daq_mcontrol.db")
dbLoadRecords("db/daq_central.db")
dbLoadRecords("db/daq_m21.db")
dbLoadRecords("db/daq_m22.db")
dbLoadRecords("db/daq_m31.db")
dbLoadRecords("db/daq_m32.db")
dbLoadRecords("db/daq_m32_temp.db")
dbLoadRecords("db/message.db")
dbLoadRecords("db/decoupler.db")
dbLoadRecords("db/ps_tok.db")
dbLoadRecords("db/ps_main.db")
dbLoadRecords("db/vftc.db")

dbLoadRecords("db/heartbeat.db")
dbLoadRecords("db/intercomm.db")

# ECH Protection form ICRF Cleaning RF -> Add TG.Lee 2010302
dbLoadRecords("db/ICRF_RF_Select.db")

# LTU and Sysmon Add Multi IOC -> One IOC TG.Lee 20090929
dbLoadRecords("db/dbSysMon.db","SYS=ICRH")

dbLoadRecords("db/SYS_CLTU.db", "TYPE=LTU, SYS=ICRH")
#dbLoadTemplate "db/ICRH_CLTU_CFG.template"
createTaskCLTU("ICRH", "/dev/tss/cltu.0", "0", "0", "999" )

## Set this to see messages from mySub
#var mySubDebug 1
var drvGenericSerialDebug 0
#var genSubDebug 6

# Temperature and Level detector controller
drvGenericSerialConfigure("L2", "192.168.10.12:1470", 0, 0)
# Tuner control
drvGenericSerialConfigure("L4", "125.125.125.14:5678", 0, 0)
# 2MW transmitter contrl
drvGenericSerialConfigure("L5", "192.168.10.15:1470", 0, 0)
# Low level waveform generator. Upgraded 20110613 for waveform upload
# Low level waveform generator. Upgraded 20110613 for monitor/control
drvGenericSerialConfigure("L6", "192.168.10.16:5678", 0, 0)
# Interlock analyzer
drvGenericSerialConfigure("L7", "192.168.10.17:1470", 0, 0)
# daq installed 20091113 for RF detection
# RFD matched line A and B DC 4ch
drvGenericSerialConfigure("L21", "192.168.10.21:5678", 0, 0)
# RFD resonant loop voltage 8ch
drvGenericSerialConfigure("L22", "192.168.10.22:5678", 0, 0)
# daq installed 2010 for generic acquisition
drvGenericSerialConfigure("L31", "125.125.125.31:5678", 0, 0)
drvGenericSerialConfigure("L32", "125.125.125.32:5678", 0, 0)
# Phase shifter in Tokamak hall, vftc_valve, decoupler cooling
drvGenericSerialConfigure("L35", "125.125.125.35:5678", 0, 0)
# Phase shifter in mainline(HDR)
drvGenericSerialConfigure("L34", "125.125.125.34:5678", 0, 0)
# vftc monitor and valve actuator
drvGenericSerialConfigure("L33", "125.125.125.33:5678", 0, 0)

#drvIcrfDM2_init(port_name, devTimeout, cbTimeout, scanInterval, mOde)
drvIcrfDM_init("L2", 3., 3., 0.2, 0)
drvIcrfDM_init("L4", 3., 3., 0.2, 4)
drvIcrfDM_init("L5", 3., 3., 0.2, 0)
drvIcrfDM_init("L6", 3., 3., 0.3, 1)
drvIcrfDM_init("L7", 3., 3., 0.2, 0)
drvIcrfDM_init("L21", 3., 3., 0.2, 3)
drvIcrfDM_init("L22", 3., 3., 0.2, 3)
drvIcrfDM_init("L31", 3., 3., 0.2, 4)
drvIcrfDM_init("L32", 3., 3., 0.2, 4)
drvIcrfDM_init("L35", 3., 3., 0.2, 4)
drvIcrfDM_init("L34", 3., 3., 0.2, 4)
drvIcrfDM_init("L33", 3., 3., 0.2, 4)

epicsThreadSleep(2.0)

cd ${TOP}/iocBoot/${IOC}
iocInit()

epicsThreadSleep(2.0)

#########################
# IRMIS crawler [shbaek, 2010.02.24]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################

## Start any sequence programs
#seq sncExample,"user=rootHost"

