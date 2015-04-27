TOP=../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
#asyn_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
#TPG256_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
#cltuDev_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
#devESP300_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
#devDG535_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
#etherIp_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
sfwLib_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

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

LIBRARY_IOC += ECH_DAQSupport
#LIBRARY_IOC += recTimeStamp

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
ECH_DAQSupport_SRCS += xxxRecord.c
ECH_DAQSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
ECH_DAQSupport_SRCS += dbSubExample.c
ECH_DAQSupport_SRCS += ECH_DAQHello.c
ECH_DAQSupport_SRCS += initTrace.c

ECH_DAQSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = ECH_DAQ
# ECH_DAQ.dbd will be created and installed
DBD += ECH_DAQ.dbd

# ECH_DAQ.dbd will be made up from these files:
ECH_DAQ_DBD += base.dbd
ECH_DAQ_DBD += xxxSupport.dbd
ECH_DAQ_DBD += dbSubExample.dbd
ECH_DAQ_DBD += ECH_DAQHello.dbd
ECH_DAQ_DBD += initTrace.dbd

# DAQ System SFW
ECH_DAQ_DBD += drvNIDAQ.dbd
ECH_DAQ_DBD += devNIDAQ.dbd
ECH_DAQ_DBD += statCommon.dbd
ECH_DAQ_DBD += kutil.dbd
ECH_DAQ_DBD += kupaLib.dbd
ECH_DAQ_DBD += sfwDBD.dbd

ECH_DAQ_DBD += timestampRecord.dbd
ECH_DAQ_DBD += asyn.dbd
ECH_DAQ_DBD += sysMon.dbd


# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
ECH_DAQ_DBD += pvCrawler.dbd
ECH_DAQ_DBD += subIfDriver.dbd
ECH_DAQ_DBD += subHealthState.dbd

ECH_DAQ_SRCS += subIfDriver.cpp
ECH_DAQ_SRCS += subHealthState.cpp
ECH_DAQ_SRCS += userclass.cpp


# DAQ PCI 6220  System       20090723 TGLee
ECH_DAQ_SRCS += drvNIDAQ.c
ECH_DAQ_SRCS += statCommon.c
ECH_DAQ_SRCS += devNIDAQ.c
ECH_DAQ_SRCS += myMDSplus.c

# ECH_DAQ_registerRecordDeviceDriver.cpp derives from ECH_DAQ.dbd
ECH_DAQ_SRCS += ECH_DAQ_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
ECH_DAQ_SRCS_DEFAULT += ECH_DAQMain.cpp
ECH_DAQ_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#ECH_DAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
ECH_DAQ_LIBS += ECH_DAQSupport
ECH_DAQ_LIBS += sfwLib
ECH_DAQ_LIBS += kupa 
ECH_DAQ_LIBS += kutil 

ECH_DAQ_LIBS += recTimeStamp
ECH_DAQ_LIBS += asyn
ECH_DAQ_LIBS += sysMon


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into ECH_DAQSupport
    sncExample_SNCFLAGS += +r
    ECH_DAQ_DBD += sncExample.dbd
    ECH_DAQSupport_SRCS += sncExample.stt
    ECH_DAQSupport_SRCS += sncECHSeq.stt 
    ECH_DAQSupport_SRCS += sncECHCArchive.stt 
    ECH_DAQSupport_LIBS += seq pv
    ECH_DAQ_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
ECH_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

