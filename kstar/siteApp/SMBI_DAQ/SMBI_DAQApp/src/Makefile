TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES    += -I/usr/local/mdsplus/include
USR_INCLUDES    += -I../include
USR_LDFLAGS     += -L/usr/local/lib
USR_SYS_LIBS 	+= nidaqmx
USR_LDFLAGS     += -L/usr/local/mdsplus/lib
USR_SYS_LIBS	+= TreeShr TdiShr MdsLib MdsShr c popt

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
USR_CFLAGS	+= -D_NI_int32_DEFINED_

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR 	    += $(EPICS_SITE)/lib/$(T_A)
#sfw add
sfwLib_DIR 	    += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

LIBRARY_IOC += SMBI_DAQSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd


# Compile and add the code to the support library
SMBI_DAQSupport_SRCS += xxxRecord.c
SMBI_DAQSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
SMBI_DAQSupport_SRCS += dbSubExample.c
SMBI_DAQSupport_SRCS += SMBI_DAQHello.c
#SMBI_DAQSupport_SRCS += initTrace.c

SMBI_DAQSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = SMBI_DAQ
# SMBI_DAQ.dbd will be created and installed
DBD += SMBI_DAQ.dbd

SMBI_DAQ_DBD += base.dbd
SMBI_DAQ_DBD += drvNI6123.dbd
SMBI_DAQ_DBD += devNI6123.dbd
SMBI_DAQ_DBD += statCommon.dbd
#SMBI_DAQ_DBD += initTrace.dbd
#SMBI_DAQ_DBD += devIT6322.dbd
#SMBI_DAQ_DBD += drvNI6259.dbd
#SMBI_DAQ_DBD += devNI6259.dbd
#SMBI_DAQ_DBD += drvIT6322.dbd

SMBI_DAQ_DBD += sfwDBD.dbd

#SMBI_DAQ_DBD += timestampRecord.dbd
#SMBI_DAQ_DBD += sysMon.dbd
#SMBI_DAQ_DBD += subIfDriver.dbd
#SMBI_DAQ_DBD += subHealthState.dbd
SMBI_DAQ_DBD += cltuBase.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
SMBI_DAQ_DBD += pvCrawler.dbd

# SMBI_DAQ.dbd will be made up from these files:
SMBI_DAQ_DBD += xxxSupport.dbd
SMBI_DAQ_DBD += dbSubExample.dbd
SMBI_DAQ_DBD += SMBI_DAQHello.dbd
#SMBI_DAQ_DBD += initTrace.dbd

# SMBI_DAQ_registerRecordDeviceDriver.cpp derives from SMBI_DAQ.dbd
SMBI_DAQ_SRCS += SMBI_DAQ_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
SMBI_DAQ_SRCS_DEFAULT += SMBI_DAQMain.cpp
SMBI_DAQ_SRCS_vxWorks += -nil-

# Add locally compiled object code
#SMBI_DAQ_SRCS += initTrace.c
SMBI_DAQ_SRCS += drvNI6123.c
#SMBI_DAQ_SRCS += drvNI6259.c
#SMBI_DAQ_SRCS += devNI6259.c
#SMBI_DAQ_SRCS += drvIT6322.c
#SMBI_DAQ_SRCS += devIT6322.c
SMBI_DAQ_SRCS += statCommon.c
SMBI_DAQ_SRCS += devNI6123.c
SMBI_DAQ_SRCS += myMDSplus.c

#delete for sfw add
#SMBI_DAQ_SRCS += sfwDBSeq.c
#SMBI_DAQ_SRCS += sfwCommon.c
#SMBI_DAQ_SRCS += sfwAdminHead.c
#SMBI_DAQ_SRCS += sfwAdminBody.c
#SMBI_DAQ_SRCS += sfwMDSplus.c
#SMBI_DAQ_SRCS += subIfDriver.cpp
#SMBI_DAQ_SRCS += subHealthState.cpp
#SMBI_DAQ_SRCS += userclass.cpp


# Add support from base/src/vxWorks if needed
#SMBI_DAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
SMBI_DAQ_LIBS += SMBI_DAQSupport
#SMBI_DAQ_LIBS += recTimeStamp
#SMBI_DAQ_LIBS += sysMon
SMBI_DAQ_LIBS += sfwLib
SMBI_DAQ_LIBS += cltuDev
SMBI_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into SMBI_DAQSupport
    sncExample_SNCFLAGS += +r
    SMBI_DAQ_DBD += sncExample.dbd
    SMBI_DAQSupport_SRCS += sncExample.stt
    #SMBI_DAQSupport_SRCS += sncSMBISeq.stt
    SMBI_DAQSupport_LIBS += seq pv
    SMBI_DAQ_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
SMBI_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

