TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
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


#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = GCDS
# GCDS.dbd will be created and installed
DBD += GCDS.dbd

# GCDS.dbd will be made up from these files:
GCDS_DBD += base.dbd
GCDS_DBD += xxxSupport.dbd
GCDS_DBD += dbSubExample.dbd
GCDS_DBD += GCDSHello.dbd
GCDS_DBD += subHealthState.dbd 
GCDS_DBD += subIfDriver.dbd 

GCDS_DBD += timestampRecord.dbd
GCDS_DBD += asyn.dbd
GCDS_DBD += sysMon.dbd
GCDS_DBD += drvNIDAQ.dbd
GCDS_DBD += devNIDAQ.dbd
GCDS_DBD += statCommon.dbd
GCDS_DBD += sfwDBD.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
GCDS_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
GCDS_SRCS += GCDS_registerRecordDeviceDriver.cpp
GCDS_SRCS_DEFAULT += GCDSMain.cpp
GCDS_SRCS_vxWorks += -nil-

# Add locally compiled object code
GCDS_SRCS += dbSubExample.c
GCDS_SRCS += GCDSHello.c
GCDS_SRCS += drvNIDAQ.c
GCDS_SRCS += devNIDAQ.c
GCDS_SRCS += statCommon.c
GCDS_SRCS += myMDSplus.c
GCDS_SRCS += subHealthState.cpp
GCDS_SRCS += subIfDriver.cpp
GCDS_SRCS += userclass.cpp

# The following adds support from base/src/vxWorks
GCDS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

GCDS_LIBS += xxxSupport
GCDS_LIBS += asyn
GCDS_LIBS += recTimeStamp
GCDS_LIBS += sysMon
GCDS_LIBS += sfwLib 

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of GCDS
    GCDS_SNCFLAGS += +r
    GCDS_DBD += sncExample.dbd
    GCDS_SRCS += sncExample.stt
    GCDS_SRCS += sncGCDSSeq.stt
    GCDS_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

GCDS_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

