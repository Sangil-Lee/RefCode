###################################################
#  add several db file in ./Db/Makefile
#
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/dbSFW.db
DB += $(EPICS_SITE)/db/asynRecord.db


##################################################
# modify Makefile which placed on ./src directory
#------------------------------------------      
#  Makefile

#================================================
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#================================================
USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

cltuDev_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
sfwLib_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)

# add your dependency, ex)
# USR_LDFLAGS += -L$(GSC_DIR)/utils -l16aiss8ao4_utils
# USR_INCLUDES += -I$(GSC_DIR)/driver -I$(GSC_DIR)/utils -I$(GSC_DIR)/docsrc


#===================================
# Build the IOC application
#===================================
PROD_IOC = xxx
xxx_DBD += sfwDBD.dbd
xxx_DBD += cltuBase.dbd
xxx_DBD += sysMon.dbd
xxx_DBD += timestampRecord.dbd
xxx_DBD += drvAsynIPPort.dbd
xxx_DBD += drvAsynSerialPort.dbd
XXX_DBD += dbdXXX.dbd

#===================================
# add your source file
#===================================
# follow is example of make master task as a controller.
# you need file copy: cp $EPICS_SITE/sfwLib/src/appExample.c ./     
xxx_SRCS += appExample.c
xxx_SRCS += devXXX.c
xxx_SRCS += drvXXX.c

#==============================================
# Link in the code from the support library
#==============================================
xxx_LIBS += sfwLib
xxx_LIBS += cltuDev
xxx_LIBS += sysMon
xxx_LIBS += recTimeStamp
xxx_LIBS += asyn

###############################################
# modify start up script to run IOC
# ----------------------------------
# st.cmd

# Copy "sfwEnv" to "/iocBoot/iocXXX" directory and load it.
< envPaths

< sfwEnv

## after Register all support components


## required task spawn with system name
createMainTask("XXX")

##          DevType,   Task(Dev)Name,  arg0,   arg1,  arg2 
createDevice("X_Type",  "X_Name",     "Dev5")

## do this if you need it 
##        Task(Dev)Name,   ID,    arg0,   arg1,   arg3 
createChannel("X_Name",    "0") 
createChannel("X_Name",    "1")

# Activate master control mechanism, It's your choice
initControl


## Load record instances
dbLoadRecords "db/dbSFW.db", "SYS=XXX" 
dbLoadRecords "db/dbXXX.db", "SYS=XXX, DEV=X_Name" 

# load asynRecord, if you want
dbLoadRecords "db/asynRecord.db","P=XXX,R=_asyn,PORT=L0,ADDR=0,OMAX=0,IMAX=0"

## load device dependant records

~
iocInit

~

## IRMIS execute



###############################################
# openSUSE
#
# startup script

# openSUSE -> /etc/init.d/boot.local
cd /usr/local/epics/siteLib/cltuLib/driver
./load-cltu
cd ~

# bash shell.....
# /etc/bash.bashrc





