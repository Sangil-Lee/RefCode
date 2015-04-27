#!../../bin/linux-x86/s7plcApp

## You may have to change hrs to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

dbLoadDatabase("dbd/s7plcApp.dbd")
s7plcApp_registerRecordDeviceDriver(pdbbase)

#var s7plcDebug <level>
#level=-1:  no output
#level=0:   errors only
#level=1:   startup messages
#level=2: + output record processing
#level=3: + inputput record processing
#level=4: + driver calls
#level=5: + io printout
#be careful using level>1 since many messages may introduce delays

var s7plcDebug 1 

#s7plcConfigure name,IPaddr,port,inSize,outSize,bigEndian,recvTimeout,sendIntervall
#connects to PLC <name> on address <IPaddr> port <port>
#<inSize>        : size of data bock PLC -> IOC [bytes]
#<outSize>       : size of data bock IOC -> PLC [bytes]
#<bigEndian>=1   : motorola format data (MSB first)
#<bigEndian>=0   : intel format data (LSB first)
#<recvTimeout>   : time to wait for input before disconnecting [ms]
#<sendIntervall> : time to wait before sending new data to PLC [ms]

# s7plcConfigure Testsystem:0,127.0.0.1,2000,80,80,1,2000,100
s7plcConfigure DB1PLC,100.100.100.10,2000,448,128,1,2000,100
# s7plcConfigure OPI19,127.0.0.1,2000,32,32,1,2000,100

# save_restore setup
# < ${TOP}/iocBoot/${IOC}/save_restore.cmd

# dbLoadRecords("db/example.db")
dbLoadRecords("db/status.db")
dbLoadTemplate("db/hrs_io.template")
# dbLoadRecords("db/hrs_mbbi.db", "SYS=HRS")
# dbLoadRecords("db/hrs_mbbo.db", "SYS=HRS")
dbLoadRecords("db/hrsTimeStamp.db", "SYS=HRS")
dbLoadRecords("db/dbSysMon.db", "SYS=HRS")

cd ${TOP}/iocBoot/${IOC}
iocInit()

# save
# create_monitor_set("hrs_savelist.req", 1)

#########################
# IRMIS crawler [shbaek, 2010.01.07]
#########################
dbl > pvlist

cd ${TOP}/pvcrawler
system("./cmd.exe")
cd ${TOP}/iocBoot/${IOC}
#########################
