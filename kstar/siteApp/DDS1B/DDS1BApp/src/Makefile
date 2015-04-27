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

PROD_IOC = DDS1B
# DDS1B.dbd will be created and installed
DBD += DDS1B.dbd

# DDS1B.dbd will be made up from these files:
DDS1B_DBD += base.dbd
DDS1B_DBD += initTrace.dbd

DDS1B_DBD += sfwDBD.dbd
DDS1B_DBD += sysMon.dbd
DDS1B_DBD += cltuBase.dbd
DDS1B_DBD += timestampRecord.dbd
DDS1B_DBD += subIfDriver.dbd
DDS1B_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
DDS1B_DBD += pvCrawler.dbd

DDS1B_DBD += dbdACQ196.dbd



# DDS1B_registerRecordDeviceDriver.cpp derives from DDS1B.dbd
DDS1B_SRCS += DDS1B_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
DDS1B_SRCS_DEFAULT += DDS1BMain.cpp
DDS1B_SRCS_vxWorks += -nil-

DDS1B_SRCS += initTrace.c
DDS1B_SRCS += acqHTstream.c
DDS1B_SRCS += drvACQ196.c
DDS1B_SRCS += devACQ196.c
DDS1B_SRCS += myMDSplus.c

DDS1B_SRCS += subIfDriver.cpp
DDS1B_SRCS += subHealthState.cpp
DDS1B_SRCS += userclass.cpp


# Add support from base/src/vxWorks if needed
#DDS1B_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

DDS1B_LIBS += sfwLib
DDS1B_LIBS += cltuDev
DDS1B_LIBS += recTimeStamp
DDS1B_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into DDS1BSupport
    sncExample_SNCFLAGS += +r
    DDS1B_DBD += sncExample.dbd
    DDS1BSupport_SRCS += sncExample.stt
    DDS1BSupport_LIBS += seq pv
    DDS1B_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
DDS1B_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

