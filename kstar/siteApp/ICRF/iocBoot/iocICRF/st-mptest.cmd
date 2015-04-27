#! ../../bin/linux-x86/testIcrfDM

# set envirtonment
< envPaths

cd ${TOP}
dbLoadDatabase("dbd/testIcrfDM.dbd")
testIcrfDM_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/mptest.db","MAXARR=100000")

cd ${TOP}/iocBoot/${IOC}
iocInit()
