TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
#sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR   += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

LIBRARY_IOC += RBASupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
RBASupport_SRCS += xxxRecord.c
RBASupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
RBASupport_SRCS += dbSubExample.c
RBASupport_SRCS += RBAHello.c
RBASupport_SRCS += initTrace.c

RBASupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = RBA
# RBA.dbd will be created and installed
DBD += RBA.dbd

# RBA.dbd will be made up from these files:
RBA_DBD += base.dbd
RBA_DBD += sysMon.dbd
RBA_DBD += xxxSupport.dbd
RBA_DBD += dbSubExample.dbd
RBA_DBD += RBAHello.dbd
RBA_DBD += initTrace.dbd
RBA_DBD += cltuBase.dbd
RBA_DBD += subIfDriver.dbd
RBA_DBD += subHealthState.dbd


# Next line was added for pvcrawler system command [shbaek, 2010.02.12]
RBA_DBD += pvCrawler.dbd


# RBA_registerRecordDeviceDriver.cpp derives from RBA.dbd
RBA_SRCS += RBA_registerRecordDeviceDriver.cpp
RBA_SRCS += subIfDriver.cpp
RBA_SRCS += subHealthState.cpp
RBA_SRCS += userclass.cpp

# Build the main IOC entry point on workstation OSs.
RBA_SRCS_DEFAULT += RBAMain.cpp
RBA_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#RBA_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
RBA_LIBS += RBASupport

RBA_LIBS += cltuDev
RBA_LIBS += sysMon


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into RBASupport
    sncExample_SNCFLAGS += +r
    RBA_DBD += sncExample.dbd
    RBASupport_SRCS += sncExample.stt
    RBASupport_LIBS += seq pv
    RBA_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
RBA_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

