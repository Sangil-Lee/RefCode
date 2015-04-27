TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_INCLUDES += -I/usr/local/mdsplus/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd

USR_LDFLAGS         += -L/usr/local/lib -lnidaqmx
nidaqmx_DIRS          = /usr/local/lib
#USR_LDFLAGS         += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt
USR_LDFLAGS         += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
USR_CFLAGS  += -D_NI_int32_DEFINED_


sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR         += $(EPICS_SITE)/lib/$(T_A)
#sfw add
sfwLib_DIR      += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

LIBRARY_IOC += FUEL_DAQSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
FUEL_DAQSupport_SRCS += xxxRecord.c
FUEL_DAQSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
FUEL_DAQSupport_SRCS += dbSubExample.c
FUEL_DAQSupport_SRCS += FUEL_DAQHello.c
#FUEL_DAQSupport_SRCS += initTrace.c

FUEL_DAQSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = FUEL_DAQ
# FUEL_DAQ.dbd will be created and installed
DBD += FUEL_DAQ.dbd

# FUEL_DAQ.dbd will be made up from these files:
FUEL_DAQ_DBD += base.dbd
FUEL_DAQ_DBD += xxxSupport.dbd
FUEL_DAQ_DBD += dbSubExample.dbd
FUEL_DAQ_DBD += FUEL_DAQHello.dbd
#FUEL_DAQ_DBD += initTrace.dbd

FUEL_DAQ_DBD += subHealthState.dbd
FUEL_DAQ_DBD += subIfDriver.dbd

FUEL_DAQ_DBD += timestampRecord.dbd
FUEL_DAQ_DBD += asyn.dbd
FUEL_DAQ_DBD += sysMon.dbd
FUEL_DAQ_DBD += drvNIDAQ.dbd
FUEL_DAQ_DBD += devNIDAQ.dbd
FUEL_DAQ_DBD += statCommon.dbd
FUEL_DAQ_DBD += sfwDBD.dbd
FUEL_DAQ_DBD += cltuBase.dbd

# Next line was added for pvcrawler system command 
FUEL_DAQ_DBD += pvCrawler.dbd

# FUEL_DAQ_registerRecordDeviceDriver.cpp derives from FUEL_DAQ.dbd
FUEL_DAQ_SRCS += FUEL_DAQ_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
FUEL_DAQ_SRCS_DEFAULT += FUEL_DAQMain.cpp
FUEL_DAQ_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#FUEL_DAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary
FUEL_DAQ_SRCS += dbSubExample.c
FUEL_DAQ_SRCS += drvNIDAQ.c
FUEL_DAQ_SRCS += devNIDAQ.c
FUEL_DAQ_SRCS += statCommon.c
FUEL_DAQ_SRCS += myMDSplus.c
FUEL_DAQ_SRCS += subHealthState.cpp
FUEL_DAQ_SRCS += subIfDriver.cpp
FUEL_DAQ_SRCS += userclass.cpp


# Link in the code from the support library
FUEL_DAQ_LIBS += FUEL_DAQSupport
FUEL_DAQ_LIBS += asyn
FUEL_DAQ_LIBS += recTimeStamp
FUEL_DAQ_LIBS += sysMon
FUEL_DAQ_LIBS += sfwLib
FUEL_DAQ_LIBS += cltuDev

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into FUEL_DAQSupport
    sncExample_SNCFLAGS += +r
    FUEL_DAQ_DBD += sncExample.dbd
    FUEL_DAQSupport_SRCS += sncExample.stt
    FUEL_DAQSupport_SRCS += sncFUELDAQSeq.stt 
    FUEL_DAQSupport_LIBS += seq pv
    FUEL_DAQ_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
FUEL_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

