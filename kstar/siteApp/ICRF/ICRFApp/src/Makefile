TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
#USR_INCLUDES += -I/usr/local/mdsplus/include
#USR_LDFLAGS  += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
#USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
USR_DBDFLAGS += -I/usr/local/epics/siteLib/dbd

#USR_INCLUDES += -I/usr/local/mdsplus/include
#USR_LDFLAGS  += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr

asyn_DIR = $(EPICS_PATH)/siteLib/lib/$(T_A)
icrfDM_DIR = $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)

#==================================================
# Build an IOC support library

#LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
#DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
#xxxSupport_SRCS += xxxRecord.c
#xxxSupport_SRCS += devXxxSoft.c

#xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = ICRF
# ICRF.dbd will be created and installed
DBD += ICRF.dbd

# ICRF.dbd will be made up from these files:
ICRF_DBD += base.dbd
#ICRF_DBD += xxxSupport.dbd
#ICRF_DBD += dbSubExample.dbd
ICRF_DBD += ICRFHello.dbd
#ICRF_DBD += genSubFunction.dbd
ICRF_DBD += cltuBase.dbd
ICRF_DBD += sysMon.dbd
#ICRF_DBD += genSubFunction.dbd
ICRF_DBD += testIcrfDMInclude.dbd
ICRF_DBD += subIfDriver.dbd
ICRF_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.02.24]
ICRF_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ICRF_SRCS += ICRF_registerRecordDeviceDriver.cpp
ICRF_SRCS += subIfDriver.cpp
ICRF_SRCS += subHealthState.cpp
ICRF_SRCS += userclass.cpp
ICRF_SRCS_DEFAULT += ICRFMain.cpp
ICRF_SRCS_vxWorks += -nil-

# Add locally compiled object code
#ICRF_SRCS += dbSubExample.c
ICRF_SRCS += ICRFHello.c
#ICRF_SRCS += testIcrfDMMain.cpp
#ICRF_SRCS += testIcrfDMgenSub.c

# The following adds support from base/src/vxWorks
ICRF_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary
ICRF_LIBS += $(EPICS_BASE_IOC_LIBS)
#ICRF_LIBS += xxxSupport
ICRF_LIBS += asyn
ICRF_LIBS += icrfDM
#ICRF_LIBS += genSub
ICRF_LIBS += recTimeStamp
ICRF_LIBS += cltuDev
ICRF_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of ICRF
    ICRF_SNCFLAGS += +r
    ICRF_DBD += sncExample.dbd
    ICRF_SRCS += sncExample.stt
    ICRF_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

ICRF_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

