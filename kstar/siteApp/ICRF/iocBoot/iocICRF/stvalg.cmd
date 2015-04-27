#! /usr/bin/valgrind ../../bin/linux-x86/testIcrfDM

# set envirtonment
< envPaths
#######
# kkh #
#######
epicsEnvSet("EPICS_CAS_INTF_ADDR_LIST", "172.17.101.180")
epicsEnvSet("EPICS_CA_ADDR_LIST", "")
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "YES")
cd ${TOP}
dbLoadDatabase("dbd/testIcrfDM.dbd")
testIcrfDM_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/tune.db")
dbLoadRecords("db/2MW.db")
dbLoadRecords("db/lowRF.db")
dbLoadRecords("db/interlock.db")
# MAXARR is limited by 100,000 at drv level.
# Maximum MAXARR is 4MByte of unsigned short
dbLoadRecords("db/daqRfd.db","MAXARR=100000")
dbLoadRecords("db/heartbeat.db")
dbLoadRecords("db/intercomm.db")

#var drvGenericSerialDebug 10
var genSubDebug 1

# RF detector dsp module 1
drvGenericSerialConfigure("L1", "125.125.125.11:1470", 0, 0)
# Tuner control
drvGenericSerialConfigure("L4", "125.125.125.14:1470", 0, 0)
# 2MW transmitter contrl
drvGenericSerialConfigure("L5", "125.125.125.15:1470", 0, 0)
# Low level waveform generator
drvGenericSerialConfigure("L6", "125.125.125.16:1470", 0, 0)
# Interlock analyzer
drvGenericSerialConfigure("L7", "125.125.125.17:1470", 0, 0)

#drvIcrfDM2_init(port_name, devTimeout, cbTimeout, scanInterval, mOde)
drvIcrfDM_init("L1", 3., 3., 0.2, 2)
drvIcrfDM_init("L4", 3., 3., 0.2, 0)
drvIcrfDM_init("L5", 3., 3., 0.2, 0)
drvIcrfDM_init("L6", 3., 3., 0.2, 1)
drvIcrfDM_init("L7", 3., 3., 0.2, 0)
cd ${TOP}/iocBoot/${IOC}
iocInit()
