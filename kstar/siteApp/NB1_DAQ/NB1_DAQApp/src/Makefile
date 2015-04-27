TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library
USR_INCLUDES    += -I/usr/local/mdsplus/include
USR_INCLUDES    += -I/usr/local/epics/siteLib/acqDrv
USR_LDFLAGS     += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

USR_CFLAGS		+= -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
sysMon_DIR		+= $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR+= $(EPICS_PATH)/siteLib/lib/$(T_A)

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

PROD_IOC = NBI_DAQ
# NBI_DAQ.dbd will be created and installed
DBD += NBI_DAQ.dbd

# NBI_DAQ.dbd will be made up from these files:
NBI_DAQ_DBD += base.dbd
NBI_DAQ_DBD += sysMon.dbd
NBI_DAQ_DBD += xxxSupport.dbd
NBI_DAQ_DBD += dbSubExample.dbd
NBI_DAQ_DBD += NBI_DAQHello.dbd
NBI_DAQ_DBD += subIfDriver.dbd
NBI_DAQ_DBD += subHealthState.dbd
NBI_DAQ_DBD += drvAcq196.dbd
NBI_DAQ_DBD += devAcq196.dbd
NBI_DAQ_DBD += timestampRecord.dbd
NBI_DAQ_DBD += pvCrawler.dbd

NBI_DAQ_DBD += drvTCdatasave.dbd
NBI_DAQ_DBD += devTCdatasave.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
NBI_DAQ_SRCS += NBI_DAQ_registerRecordDeviceDriver.cpp
NBI_DAQ_SRCS_DEFAULT += NBI_DAQMain.cpp
NBI_DAQ_SRCS_vxWorks += -nil-

# Add locally compiled object code
NBI_DAQ_SRCS += dbSubExample.c
NBI_DAQ_SRCS += NBI_DAQHello.c
NBI_DAQ_SRCS += subIfDriver.cpp
NBI_DAQ_SRCS += subHealthState.cpp
NBI_DAQ_SRCS += userclass.cpp
NBI_DAQ_SRCS += drvAcq196.cpp
NBI_DAQ_SRCS += devAcq196.cpp
NBI_DAQ_SRCS += acqMdsPlus.c
NBI_DAQ_SRCS += htstream.c

NBI_DAQ_SRCS += tcMdsplus.c
NBI_DAQ_SRCS += drvTCdatasave.c
NBI_DAQ_SRCS += devTCdatasave.c

# The following adds support from base/src/vxWorks
NBI_DAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

NBI_DAQ_LIBS += xxxSupport
NBI_DAQ_LIBS += sysMon
NBI_DAQ_LIBS += recTimeStamp

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of NBI_DAQ
    NBI_DAQ_SNCFLAGS += +r
    NBI_DAQ_DBD += sncExample.dbd
    NBI_DAQ_DBD += sncAutoArm.dbd
    NBI_DAQ_SRCS += sncExample.stt
    NBI_DAQ_SRCS += sncAutoArm.stt
    NBI_DAQ_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

NBI_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

