TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
# add by woong 2012.4.19 for LTU
USR_INCLUDES    += -I/usr/local/epics/siteLib/include
USR_DBDFLAGS    += -I/usr/local/epics/siteLib/dbd
cltuDev_DIR     += /usr/local/epics/siteLib/lib/linux-x86

recTimeStamp_DIR+= $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR      += $(EPICS_PATH)/siteLib/lib/$(T_A)

#==================================================
# Build an IOC support library

#=============================
# build an ioc application

PROD_IOC = LHCD_MAIN
# LHCD_MAIN.dbd will be created and installed
DBD += LHCD_MAIN.dbd

# LHCD_MAIN.dbd will be made up from these files:
LHCD_MAIN_DBD += base.dbd
LHCD_MAIN_DBD += devLHCD_AutoSet.dbd
LHCD_MAIN_DBD += ether_ip.dbd

LHCD_MAIN_DBD += timestampRecord.dbd
LHCD_MAIN_DBD += sysMon.dbd
LHCD_MAIN_DBD += subIfDriver.dbd
LHCD_MAIN_DBD += subHealthState.dbd
LHCD_MAIN_DBD += pvCrawler.dbd

# add by woong 2012.4.19 for LTU
LHCD_MAIN_DBD += cltuBase.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
LHCD_MAIN_SRCS += LHCD_MAIN_registerRecordDeviceDriver.cpp
LHCD_MAIN_SRCS_DEFAULT += LHCD_MAINMain.cpp
LHCD_MAIN_SRCS_vxWorks += -nil-

LHCD_MAIN_SRCS += subIfDriver.cpp
LHCD_MAIN_SRCS += subHealthState.cpp
LHCD_MAIN_SRCS += userclass.cpp

# Add locally compiled object code
LHCD_MAIN_SRCS += devLHCD_AutoSet.c

# The following adds support from base/src/vxWorks
LHCD_MAIN_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

LHCD_MAIN_LIBS += ether_ip
LHCD_MAIN_LIBS += recTimeStamp
LHCD_MAIN_LIBS += sysMon
LHCD_MAIN_LIBS += cltuDev

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncParameter as a component of LHCD_MAIN
    LHCD_MAIN_SNCFLAGS += +r
    LHCD_MAIN_DBD += sncParameter.dbd
    LHCD_MAIN_DBD += sncPowParameter.dbd
    LHCD_MAIN_DBD += sncFaultCode.dbd
    LHCD_MAIN_DBD += sncDCSET.dbd
    LHCD_MAIN_DBD += sncOperationMode.dbd
    LHCD_MAIN_DBD += sncPermission.dbd
    LHCD_MAIN_DBD += sncFaultHvOff.dbd
    LHCD_MAIN_DBD += sncPermissionLocal.dbd
    LHCD_MAIN_SRCS += sncParameter.stt
    LHCD_MAIN_SRCS += sncPowParameter.stt
    LHCD_MAIN_SRCS += sncFaultCode.stt
    LHCD_MAIN_SRCS += sncDCSET.stt
    LHCD_MAIN_SRCS += sncOperationMode.stt
    LHCD_MAIN_SRCS += sncPermission.stt
    LHCD_MAIN_SRCS += sncFaultHvOff.stt
    LHCD_MAIN_SRCS += sncPermissionLocal.stt
    LHCD_MAIN_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

LHCD_MAIN_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

