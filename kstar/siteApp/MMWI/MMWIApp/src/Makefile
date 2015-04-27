TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES    += -I/usr/local/mdsplus/include
USR_INCLUDES    += -I../include
#USR_LDFLAGS     += -L/usr/lib
USR_LDFLAGS     += -L/usr/local/lib
USR_SYS_LIBS 	+= nidaqmx
USR_LDFLAGS     += -L/usr/local/mdsplus/lib
USR_SYS_LIBS	+= TreeShr TdiShr MdsLib MdsIpShr MdsShr MdsObjectsCppShr 
#USR_SYS_LIBS	+= TreeShr TdiShr MdsLib MdsShr MdsIpShr c popt
#MdsObjectsCppShr_DIR   = /usr/local/mdsplus/lib
#USR_SYS_LIBS   += -L/usr/local/mdsplus/lib -lMdsObjectsCppShr -fexceptions -Wl,-E

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

LIBRARY_IOC += MMWISupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd


# Compile and add the code to the support library
MMWISupport_SRCS += xxxRecord.c
MMWISupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
MMWISupport_SRCS += dbSubExample.c
MMWISupport_SRCS += MMWIHello.c
#MMWISupport_SRCS += initTrace.c

MMWISupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = MMWI
# MMWI.dbd will be created and installed
DBD += MMWI.dbd

MMWI_DBD += base.dbd
MMWI_DBD += drvNI6123.dbd
MMWI_DBD += devNI6123.dbd
MMWI_DBD += statCommon.dbd
#MMWI_DBD += initTrace.dbd
#MMWI_DBD += devIT6322.dbd
#MMWI_DBD += drvNI6259.dbd
#MMWI_DBD += devNI6259.dbd
#MMWI_DBD += drvIT6322.dbd

MMWI_DBD += sfwDBD.dbd

MMWI_DBD += timestampRecord.dbd
MMWI_DBD += sysMon.dbd
#MMWI_DBD += subIfDriver.dbd
#MMWI_DBD += subHealthState.dbd
MMWI_DBD += cltuBase.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
MMWI_DBD += pvCrawler.dbd

# MMWI.dbd will be made up from these files:
MMWI_DBD += xxxSupport.dbd
MMWI_DBD += dbSubExample.dbd
MMWI_DBD += MMWIHello.dbd
#MMWI_DBD += initTrace.dbd

# MMWI_registerRecordDeviceDriver.cpp derives from MMWI.dbd
MMWI_SRCS += MMWI_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
MMWI_SRCS_DEFAULT += MMWIMain.cpp
MMWI_SRCS_vxWorks += -nil-

# Add locally compiled object code
#MMWI_SRCS += initTrace.c
MMWI_SRCS += drvNI6123.c
#MMWI_SRCS += drvNI6259.c
#MMWI_SRCS += devNI6259.c
#MMWI_SRCS += drvIT6322.c
#MMWI_SRCS += devIT6322.c
MMWI_SRCS += statCommon.c
MMWI_SRCS += devNI6123.c
MMWI_SRCS += myMDSplus.c
#MMWI_SRCS += myMDSObjCpp.cpp 

#delete for sfw add
#MMWI_SRCS += sfwDBSeq.c
#MMWI_SRCS += sfwCommon.c
#MMWI_SRCS += sfwAdminHead.c
#MMWI_SRCS += sfwAdminBody.c
#MMWI_SRCS += sfwMDSplus.c
#MMWI_SRCS += subIfDriver.cpp
#MMWI_SRCS += subHealthState.cpp
#MMWI_SRCS += userclass.cpp


# Add support from base/src/vxWorks if needed
#MMWI_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
MMWI_LIBS += MMWISupport
MMWI_LIBS += recTimeStamp
MMWI_LIBS += sysMon
MMWI_LIBS += sfwLib
MMWI_LIBS += cltuDev
MMWI_LIBS += $(EPICS_BASE_IOC_LIBS)

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into MMWISupport
    sncExample_SNCFLAGS += +r
    MMWI_DBD += sncExample.dbd
    MMWISupport_SRCS += sncExample.stt
    MMWISupport_SRCS += sncMMWISeq.stt
    MMWISupport_LIBS += seq pv
    MMWI_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
MMWI_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

