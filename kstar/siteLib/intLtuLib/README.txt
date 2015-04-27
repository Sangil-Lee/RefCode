##### IMPORTANT!!!!!!!!!!!
# must build device driver first!!!!
#


## ////////////////////////////////////////////
# linux device driver loading when system boot up
# /etc/rc.d/rc.local : add here
cd /usr/local/epics/siteLib/cltuLib/driver; ./load-cltu



## ////////////////////////////////////////////////////
# ./Db : Makefile =================

DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += XXX_CLTU_CFG.template
# Important.!!!!!!
# must check system name and type.


## ////////////////////////////////////////////////////
# ./src : Makefile ==================

#================================================
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#================================================
USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR += $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_SITE)/lib/$(T_A)

#===================================
# Build the IOC application
#===================================
PROD_IOC = xxx
xxx_DBD += cltuBase.dbd
xxx_DBD += sysMon.dbd
xxx_DBD += timestampRecord.dbd

#==============================================
# Link in the code from the support library
#==============================================
xxx_LIBS += cltuDev
xxx_LIBS += sysMon
xxx_LIBS += recTimeStamp


## ////////////////////////////////////////////////////
# st.cmd

epicsEnvSet(CLTU_IOC_DEBUG, "0") /* 3 */    /* don't care about it. */
# putenv "CLTU_IOC_DEBUG=0" # if VxWorks...

# if VxWorks
# iocsh -> add it in this position  must before calling creatTaskCLTU(), and "arg1" could be device number
# arg0: system name
# arg1: device file       /* only for linux or find device number manually */
# arg2: version (0:old, 1:new)
# arg3: type (0:LTU, 1:CTU)
# arg4: id ( only new version )
createTaskCLTU("XXX", "/dev/tss/cltu.0", "1", "0", "999") 
dbLoadRecords "db/SYS_CLTU.db", "TYPE=LTU, SYS=XXX" 
dbLoadTemplate "db/XXX_CLTU_CFG.template"                 /* only NEW version */


###############################################
# openSUSE
#
# startup script

# openSUSE -> /etc/init.d/boot.local

cd /usr/local/epics/siteLib/cltuLib/driver; ./load-cltu
cd ~

# bash shell.....
# /etc/bash.bashrc





