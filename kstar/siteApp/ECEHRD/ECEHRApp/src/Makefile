TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES        += -I/usr/local/mdsplus/include
USR_INCLUDES   	    += -I/home/dt100/PROJECTS/ACQ200/project/API/include

USR_CFLAGS 	+= -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
USR_INCLUDES    += -I/usr/local/include
USR_INCLUDES    += -I$(EPICS_PATH)/siteLib/include

USR_DBDFLAGS    += -I$(EPICS_PATH)/siteLib/dbd
USR_LDFLAGS     += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

cltuDev_DIR 	+= $(EPICS_PATH)/siteLib/lib/$(T_A)

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
sfwLib_DIR          += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

LIBRARY_IOC += ECEHRSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
ECEHRSupport_SRCS += xxxRecord.c
ECEHRSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
ECEHRSupport_SRCS += dbSubExample.c
ECEHRSupport_SRCS += ECEHRHello.c
ECEHRSupport_SRCS += initTrace.c

ECEHRSupport_SRCS += acqHTstream.c
ECEHRSupport_SRCS += drvACQ196.c
ECEHRSupport_SRCS += devACQ196.c
ECEHRSupport_SRCS += myMDSplus.c

ECEHRSupport_SRCS += statCommon.c

ECEHRSupport_SRCS += subIfDriver.cpp
ECEHRSupport_SRCS += subHealthState.cpp
ECEHRSupport_SRCS += userclass.cpp

ECEHRSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = ECEHR
# ECEHR.dbd will be created and installed
DBD += ECEHR.dbd

# ECEHR.dbd will be made up from these files:
ECEHR_DBD += base.dbd
ECEHR_DBD += xxxSupport.dbd
ECEHR_DBD += dbSubExample.dbd
ECEHR_DBD += ECEHRHello.dbd
ECEHR_DBD += initTrace.dbd

ECEHR_DBD += devACQ196.dbd
ECEHR_DBD += drvACQ196.dbd
ECEHR_DBD += statCommon.dbd
ECEHR_DBD += sfwDBD.dbd
ECEHR_DBD += timestampRecord.dbd
ECEHR_DBD += sysMon.dbd
ECEHR_DBD += subIfDriver.dbd
ECEHR_DBD += subHealthState.dbd
#ECEHR_DBD += pvCrawler.dbd

ECEHR_DBD += cltuBase.dbd

# ECEHR_registerRecordDeviceDriver.cpp derives from ECEHR.dbd
ECEHR_SRCS += ECEHR_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
ECEHR_SRCS_DEFAULT += ECEHRMain.cpp
ECEHR_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#ECEHR_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
ECEHR_LIBS += ECEHRSupport

ECEHR_LIBS += sfwLib
ECEHR_LIBS += recTimeStamp
ECEHR_LIBS += sysMon

ECEHR_LIBS += cltuDev

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into ECEHRSupport
    sncExample_SNCFLAGS += +r
    ECEHR_DBD += sncExample.dbd
    ECEHRSupport_SRCS += sncExample.stt
    ECEHRSupport_SRCS += sncECEHRSeq.stt 
    ECEHRSupport_SRCS += sncECEHRCalMode.stt 
#    ECEHRSupport_SRCS += sncECEHRCal.stt
    ECEHRSupport_LIBS += seq pv
    ECEHR_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
ECEHR_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

