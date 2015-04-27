TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd
USR_CFLAGS      += -std=gnu89

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR += $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_SITE)/lib/$(T_A)

#==================================================
# Build an IOC support library

#=============================
# build an ioc application

PROD_IOC = NBI_MAIN
# NBI_MAIN.dbd will be created and installed
DBD += NBI_MAIN.dbd

# NBI_MAIN.dbd will be made up from these files:
NBI_MAIN_DBD += base.dbd
NBI_MAIN_DBD += ether_ip.dbd
NBI_MAIN_DBD += drvEtherIP.dbd
NBI_MAIN_DBD += subIfDriver.dbd
NBI_MAIN_DBD += subHealthState.dbd 
NBI_MAIN_DBD += drvHoneywell.dbd
NBI_MAIN_DBD += cltuBase.dbd
NBI_MAIN_DBD += sysMon.dbd
NBI_MAIN_DBD += timestampRecord.dbd
NBI_MAIN_DBD += pvCrawler.dbd
NBI_MAIN_DBD += kutil.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
NBI_MAIN_SRCS += NBI_MAIN_registerRecordDeviceDriver.cpp
NBI_MAIN_SRCS_DEFAULT += NBI_MAINMain.cpp
NBI_MAIN_SRCS_vxWorks += -nil-

# Add locally compiled object code
NBI_MAIN_SRCS += subIfDriver.cpp
NBI_MAIN_SRCS += subHealthState.cpp
NBI_MAIN_SRCS += userclass.cpp
NBI_MAIN_SRCS += drvHoneywell.c
NBI_MAIN_SRCS += modbus_tcp.c

# The following adds support from base/src/vxWorks
NBI_MAIN_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

NBI_MAIN_LIBS += cltuDev
NBI_MAIN_LIBS += sysMon
NBI_MAIN_LIBS += recTimeStamp
NBI_MAIN_LIBS += kutil

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of NBI_MAIN
    NBI_MAIN_SNCFLAGS += +r
    NBI_MAIN_DBD += sncExample.dbd
    NBI_MAIN_SRCS += sncExample.stt
    NBI_MAIN_DBD += sncBMInterlock.dbd
    NBI_MAIN_SRCS += sncBMInterlock.stt
    NBI_MAIN_DBD += sncBMInterlock2.dbd
    NBI_MAIN_SRCS += sncBMInterlock2.stt
    NBI_MAIN_DBD += sncParameter.dbd
    NBI_MAIN_SRCS += sncParameter.stt
    NBI_MAIN_DBD += sncPsOperation.dbd
    NBI_MAIN_SRCS += sncPsOperation.stt
    NBI_MAIN_DBD += sncPsOperation2.dbd
    NBI_MAIN_SRCS += sncPsOperation2.stt
    NBI_MAIN_DBD += sncParameter2.dbd
    NBI_MAIN_SRCS += sncParameter2.stt
    NBI_MAIN_DBD += sncGasFaultStatus.dbd
    NBI_MAIN_SRCS += sncGasFaultStatus.stt

    NBI_MAIN_DBD += sncVMEPervInterlock.dbd
    NBI_MAIN_SRCS += sncVMEPervInterlock.stt
    NBI_MAIN_DBD += sncVMEPervInterlock2.dbd
    NBI_MAIN_SRCS += sncVMEPervInterlock2.stt

    NBI_MAIN_DBD += sncPermReady.dbd
    NBI_MAIN_SRCS += sncPermReady.stt


    NBI_MAIN_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

NBI_MAIN_LIBS += ether_ip
NBI_MAIN_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

