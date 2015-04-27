TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_INCLUDES += -I/usr/local/mdsplus/include
USR_INCLUDES += -I/usr/local/natinst/nidaqmxbase/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd

# For NIDAQmx
#USR_LDFLAGS         += -L/usr/local/lib -lnidaqmx
#nidaqmx_DIRS          = /usr/local/lib

# For NIDAQmxBase
#USR_LDFLAGS          += -L/usr/local/lib -lnidaqmxbase -lnidaqmxbaselv -llvrtdark
USR_LDFLAGS          += -L/usr/local/lib -lnidaqmxbase -lnidaqmxbaselv 
USR_DIRS             += /usr/local/lib

# For MDSplus
#USR_LDFLAGS         += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt
USR_LDFLAGS         += -L/usr/local/mdsplus/lib32 -lTreeShr -lTdiShr -lMdsLib -lMdsShr

#flags= -02 -m32
USR_CFLAGS			+= -m32
#USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
#USR_CFLAGS  += -D_NI_int8_DEFINED_
#USR_CFLAGS  += -D_NI_int16_DEFINED_
#USR_CFLAGS  += -D_NI_int32_DEFINED_
#USR_CFLAGS  += -D_NI_float32_DEFINED_
#USR_CFLAGS  += -D_NI_float64_DEFINED_

#sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#cltuDev_DIR         += $(EPICS_SITE)/lib/$(T_A)
#sfw add
sfwLib_DIR      += $(EPICS_SITE)/lib/$(T_A)



#=============================
# Build the IOC support library

LIBRARY_IOC += NIBASESupport

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
#DBD += xxxSupport.dbd

# Compile and add the code to the support library
#NIBASESupport_SRCS += xxxRecord.c
#NIBASESupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
#NIBASESupport_SRCS += dbSubExample.c
NIBASESupport_SRCS += NIBASEHello.c
#NIBASESupport_SRCS += initTrace.c

NIBASESupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = NIBASE
# NIBASE.dbd will be created and installed
DBD += NIBASE.dbd

# NIBASE.dbd will be made up from these files:
NIBASE_DBD += base.dbd
#NIBASE_DBD += xxxSupport.dbd
#NIBASE_DBD += dbSubExample.dbd
#NIBASE_DBD += initTrace.dbd
NIBASE_DBD += NIBASEHello.dbd
NIBASE_DBD += asyn.dbd
NIBASE_DBD += drvNIDAQbase.dbd
NIBASE_DBD += devNIDAQbase.dbd
NIBASE_DBD += statCommon.dbd
NIBASE_DBD += sfwDBD.dbd


# NIBASE_registerRecordDeviceDriver.cpp derives from NIBASE.dbd
NIBASE_SRCS += NIBASE_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
NIBASE_SRCS_DEFAULT += NIBASEMain.cpp
NIBASE_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#NIBASE_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary
NIBASE_SRCS += drvNIDAQbase.c 
NIBASE_SRCS += devNIDAQbase.c
NIBASE_SRCS += statCommon.c
NIBASE_SRCS += myMDSplus.c


# Link in the code from the support library
NIBASE_LIBS += NIBASESupport
NIBASE_LIBS += asyn
NIBASE_LIBS +=recTimeStamp
NIBASE_LIBS +=sfwLib

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into NIBASESupport
    sncExample_SNCFLAGS += +r
    NIBASE_DBD += sncExample.dbd
    NIBASESupport_SRCS += sncExample.stt
    NIBASESupport_SRCS += sncNIDAQbaseSeq.stt
    NIBASESupport_LIBS += seq pv
    NIBASE_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
NIBASE_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

