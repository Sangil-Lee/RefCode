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

LIBRARY_IOC += VVSSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
VVSSupport_SRCS += xxxRecord.c
VVSSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
VVSSupport_SRCS += dbSubExample.c
VVSSupport_SRCS += VVSHello.c
#VVSSupport_SRCS += initTrace.c

VVSSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = VVS
# VVS.dbd will be created and installed
DBD += VVS.dbd

# VVS.dbd will be made up from these files:
VVS_DBD += base.dbd
VVS_DBD += xxxSupport.dbd
VVS_DBD += dbSubExample.dbd
VVS_DBD += VVSHello.dbd
#VVS_DBD += initTrace.dbd

VVS_DBD += subHealthState.dbd
VVS_DBD += subIfDriver.dbd

VVS_DBD += timestampRecord.dbd
VVS_DBD += asyn.dbd
VVS_DBD += sysMon.dbd
VVS_DBD += drvNIDAQ.dbd
VVS_DBD += devNIDAQ.dbd
VVS_DBD += statCommon.dbd
VVS_DBD += sfwDBD.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
VVS_DBD += pvCrawler.dbd

# VVS_registerRecordDeviceDriver.cpp derives from VVS.dbd
VVS_SRCS += VVS_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
VVS_SRCS_DEFAULT += VVSMain.cpp
VVS_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#VVS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary
VVS_SRCS += dbSubExample.c
VVS_SRCS += drvNIDAQ.c
VVS_SRCS += devNIDAQ.c
VVS_SRCS += statCommon.c
VVS_SRCS += myMDSplus.c
VVS_SRCS += subHealthState.cpp
VVS_SRCS += subIfDriver.cpp
VVS_SRCS += userclass.cpp

# Link in the code from the support library
VVS_LIBS += VVSSupport
VVS_LIBS += asyn
VVS_LIBS += recTimeStamp
VVS_LIBS += sysMon
VVS_LIBS += sfwLib
# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into VVSSupport
    sncExample_SNCFLAGS += +r
    VVS_DBD += sncExample.dbd
    VVSSupport_SRCS += sncExample.stt
    VVS_SRCS += sncVVSSeq.stt
    VVSSupport_LIBS += seq pv
    VVS_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
VVS_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

