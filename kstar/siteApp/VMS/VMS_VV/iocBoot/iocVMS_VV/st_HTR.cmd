#!../../bin/linux-x86/vacuumVVP
## You may have to change vacuumVVP to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/vacuumVVP.dbd")
vacuumVVP_registerRecordDeviceDriver(pdbbase)

drvEtherIP_init()
drvEtherIP_define_PLC("plc1", "172.17.101.41",0)
EIP_verbosity(2)
EIP_buffer_limit(494)


# load record instance
cd ${TOP}

#dbLoadTemplate("db/VMS_VV_HTR.template")

dbLoadTemplate("db/VMS_VV_HTR_BI.template")
dbLoadTemplate("db/VMS_VV_HTR_AI.template")
dbLoadTemplate("db/VMS_VV_HTR_BO.template")
dbLoadTemplate("db/VMS_VV_HTR_AO.template")


iocInit()

epicsThreadSleep 2.

