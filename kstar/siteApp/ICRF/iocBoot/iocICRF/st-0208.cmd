#! ../../bin/linux-x86/testIcrfDM1

# set envirtonment
< envPaths

cd ${TOP}
dbLoadDatabase("dbd/testIcrfDM1.dbd")
testIcrfDM1_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=0")
dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=1")
dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=2")
dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=3")
dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=4")
dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=5")
dbLoadRecords("db/testIcrfDM1-0208.db","PORT=L0,PARAM=val,CH=6")

#var drvGenericSerialDebug 3

drvGenericSerialConfigure("L0", "147.43.33.120:1470", 0, 0)
#drvGenericSerialConfigure("L1", "203.230.120.21:4002", 0, 0)

#drvIcrfDM1_init(port_name, unit_name, devTimeout, cbTimeout, scanInterval)
drvIcrfDM1_init("L0", "Raw", 3., 3., .1)
#drvTPG262_init("L1", "Raw", 3., 3., .5)

cd ${TOP}/iocBoot/${IOC}
iocInit()
