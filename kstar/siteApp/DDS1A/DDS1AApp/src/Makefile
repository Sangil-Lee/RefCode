TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

USR_INCLUDES    += -I$(EPICS_PATH)/siteApp/dtacq/HOSTDRV
USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG

sfwLib_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)



#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = DDS1A
# DDS1A.dbd will be created and installed
DBD += DDS1A.dbd

# DDS1A.dbd will be made up from these files:
DDS1A_DBD += base.dbd
DDS1A_DBD += initTrace.dbd

DDS1A_DBD += sfwDBD.dbd
DDS1A_DBD += sysMon.dbd
DDS1A_DBD += cltuBase.dbd
DDS1A_DBD += timestampRecord.dbd
DDS1A_DBD += subIfDriver.dbd
DDS1A_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
DDS1A_DBD += pvCrawler.dbd

DDS1A_DBD += dbdACQ196.dbd



# DDS1A_registerRecordDeviceDriver.cpp derives from DDS1A.dbd
DDS1A_SRCS += DDS1A_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
DDS1A_SRCS_DEFAULT += DDS1AMain.cpp
DDS1A_SRCS_vxWorks += -nil-

DDS1A_SRCS += initTrace.c
DDS1A_SRCS += acqHTstream.c
DDS1A_SRCS += drvACQ196.c
DDS1A_SRCS += devACQ196.c
DDS1A_SRCS += myMDSplus.c

DDS1A_SRCS += subIfDriver.cpp
DDS1A_SRCS += subHealthState.cpp
DDS1A_SRCS += userclass.cpp


# Add support from base/src/vxWorks if needed
#DDS1A_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

DDS1A_LIBS += sfwLib
DDS1A_LIBS += cltuDev
DDS1A_LIBS += recTimeStamp
DDS1A_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into DDS1ASupport
    sncExample_SNCFLAGS += +r
    DDS1A_DBD += sncExample.dbd
    DDS1ASupport_SRCS += sncExample.stt
    DDS1ASupport_LIBS += seq pv
    DDS1A_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
DDS1A_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

