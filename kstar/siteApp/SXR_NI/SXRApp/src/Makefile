TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE


USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
asyn_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
TPG256_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
cltuDev_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
devESP300_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
devDG535_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
etherIp_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
sfwLib_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)

#  You must twice compile.
#  One - "USR_LDFLAGS & nidaqmx_DIRS" Add after "make clean; make" Compile.
#  Two -"USR_LDFLAGS & nidaqmx_DIRS" remove after "make" Compile.
#=============================

USR_LDFLAGS += -L/usr/local/lib -lnidaqmx
USR_LDFLAGS += -lnidaqmx
USR_INCLUDES += -I/usr/local/include
USR_INCLUDES += -I/usr/local/mdsplus/include
USR_INCLUDES += -I/usr/local/epics/siteLib/include
USR_LDFLAGS += -L/usr/local/mdsplus/lib -lTreeShr -lMdsLib -lMdsShr -lc -lpopt
#nidaqmx_DIRS = /usr/local/lib

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
USR_CFLAGS  += -D_NI_int32_DEFINED_


#=============================
# Build the IOC support library

LIBRARY_IOC += SXRSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
SXRSupport_SRCS += xxxRecord.c
SXRSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
SXRSupport_SRCS += dbSubExample.c
SXRSupport_SRCS += SXRHello.c
SXRSupport_SRCS += initTrace.c

SXRSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = SXR
# SXR.dbd will be created and installed
DBD += SXR.dbd

# SXR.dbd will be made up from these files:
SXR_DBD += base.dbd
SXR_DBD += xxxSupport.dbd
SXR_DBD += dbSubExample.dbd
SXR_DBD += SXRHello.dbd
SXR_DBD += initTrace.dbd

# DAQ System SFW
SXR_DBD += drvNIDAQmx.dbd
SXR_DBD += devNIDAQmx.dbd
SXR_DBD += statCommon.dbd
SXR_DBD += sfwDBD.dbd

SXR_DBD += cltuBase.dbd

SXR_DBD += timestampRecord.dbd
SXR_DBD += asyn.dbd
SXR_DBD += sysMon.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
#SXR_DBD += pvCrawler.dbd
#SXR_DBD += subIfDriver.dbd
#SXR_DBD += subHealthState.dbd



# SXR_registerRecordDeviceDriver.cpp derives from SXR.dbd
SXR_SRCS += SXR_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
SXR_SRCS_DEFAULT += SXRMain.cpp
SXR_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#SXR_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# DAQ PCIe-6259 AO, AI, DIO
SXR_SRCS += drvNIDAQmx.c
SXR_SRCS += statCommon.c
SXR_SRCS += devNIDAQmx.c
SXR_SRCS += myMDSplus.c

# Link in the code from the support library
SXR_LIBS += SXRSupport

SXR_LIBS += sfwLib
SXR_LIBS += cltuDev
SXR_LIBS += recTimeStamp
SXR_LIBS += sysMon
SXR_LIBS += asyn
SXR_LIBS += $(EPICS_BASE_IOC_LIBS)


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into SXRSupport
    sncExample_SNCFLAGS += +r
    SXR_DBD += sncExample.dbd
    SXRSupport_SRCS += sncExample.stt
    SXRSupport_SRCS += sncSXRDAQSeq.stt
    SXRSupport_LIBS += seq pv
    SXR_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
SXR_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

