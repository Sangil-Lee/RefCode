TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
USR_INCLUDES    += -I$(EPICS_PATH)/siteApp/dtacq/HOSTDRV
#USR_INCLUDES    += -I/home/dt100/PROJECTS/ACQ200/project/API
USR_INCLUDES    += -I/home/dt100/PROJECTS/ACQ200/project/API/include
#USR_LDFLAGS    += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR       += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR       += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

LIBRARY_IOC += HALPHASupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
HALPHASupport_SRCS += xxxRecord.c
HALPHASupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
HALPHASupport_SRCS += dbSubExample.c
HALPHASupport_SRCS += HALPHAHello.c
HALPHASupport_SRCS += initTrace.c
HALPHASupport_SRCS += acqHTstream.c 
HALPHASupport_SRCS += drvACQ196.c
HALPHASupport_SRCS += devACQ196.c
HALPHASupport_SRCS += myMDSplus.c

HALPHASupport_SRCS += statCommon.c

HALPHASupport_SRCS += subIfDriver.cpp 
HALPHASupport_SRCS += subHealthState.cpp
HALPHASupport_SRCS += userclass.cpp

HALPHASupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = HALPHA
# HALPHA.dbd will be created and installed
DBD += HALPHA.dbd

# HALPHA.dbd will be made up from these files:
HALPHA_DBD += base.dbd
HALPHA_DBD += xxxSupport.dbd
HALPHA_DBD += dbSubExample.dbd
HALPHA_DBD += HALPHAHello.dbd
HALPHA_DBD += initTrace.dbd

HALPHA_DBD += devACQ196.dbd 
HALPHA_DBD += drvACQ196.dbd
HALPHA_DBD += statCommon.dbd
HALPHA_DBD += sfwDBD.dbd
HALPHA_DBD += timestampRecord.dbd
HALPHA_DBD += sysMon.dbd
HALPHA_DBD += subIfDriver.dbd
HALPHA_DBD += subHealthState.dbd
#HALPHA_DBD += pvCrawler.dbd 


# HALPHA_registerRecordDeviceDriver.cpp derives from HALPHA.dbd
HALPHA_SRCS += HALPHA_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
HALPHA_SRCS_DEFAULT += HALPHAMain.cpp
HALPHA_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#HALPHA_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
HALPHA_LIBS += HALPHASupport
HALPHA_LIBS += sfwLib 
HALPHA_LIBS += recTimeStamp 
HALPHA_LIBS += sysMon 

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into HALPHASupport
    sncExample_SNCFLAGS += +r
    HALPHA_DBD += sncExample.dbd
    HALPHASupport_SRCS += sncExample.stt
    HALPHASupport_SRCS += sncHALPHADSeq.stt 
    HALPHASupport_LIBS += seq pv
    HALPHA_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
HALPHA_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

