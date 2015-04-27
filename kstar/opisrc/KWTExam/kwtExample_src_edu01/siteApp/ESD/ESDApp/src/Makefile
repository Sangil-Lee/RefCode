TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = ESD
# ESD.dbd will be created and installed
DBD += ESD.dbd

# ESD.dbd will be made up from these files:
ESD_DBD += base.dbd
ESD_DBD += xxxSupport.dbd
ESD_DBD += dbSubExample.dbd
ESD_DBD += ESDHello.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ESD_SRCS += ESD_registerRecordDeviceDriver.cpp
ESD_SRCS_DEFAULT += ESDMain.cpp
ESD_SRCS_vxWorks += -nil-

# Add locally compiled object code
ESD_SRCS += dbSubExample.c
ESD_SRCS += ESDHello.c

# The following adds support from base/src/vxWorks
ESD_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

ESD_LIBS += xxxSupport

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of ESD
    ESD_SNCFLAGS += +r
    ESD_DBD += sncExample.dbd
    ESD_SRCS += sncExample.stt
    ESD_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

ESD_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

