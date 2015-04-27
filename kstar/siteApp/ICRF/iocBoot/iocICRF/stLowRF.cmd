#! ../../bin/linux-x86/testIcrfDM

# set envirtonment
< envPaths

cd ${TOP}
dbLoadDatabase("dbd/testIcrfDM.dbd")
testIcrfDM_registerRecordDeviceDriver(pdbbase)

#dbLoadRecords("db/tune.db")
dbLoadRecords("db/2MW.db")
dbLoadRecords("db/lowRF.db")
var drvGenericSerialDebug 0

drvGenericSerialConfigure("L0", "125.125.125.15:1470", 0, 0)
drvGenericSerialConfigure("L6", "125.125.125.16:1470", 0, 0)
drvGenericSerialConfigure("L7", "125.125.125.17:1470", 0, 0)
#drvGenericSerialConfigure("L1", "125.125.125.15:1470", 0, 0)

#drvIcrfDM2_init(port_name, devTimeout, cbTimeout, scanInterval, mOde)
drvIcrfDM_init("L6", 3., 3., 0.2, 1)
drvIcrfDM_init("L0", 3., 3., 0.2, 0)
drvIcrfDM_init("L7", 3., 3., 0.2, 0)
#drvTPG262_init("L1", "Raw", 3., 3., .5)

cd ${TOP}/iocBoot/${IOC}
iocInit()
