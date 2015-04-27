TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

#cltuDev_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
sfwLib_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#sysMon_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)


#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = Dummy
# Dummy.dbd will be created and installed
DBD += Dummy.dbd

# Dummy.dbd will be made up from these files:
Dummy_DBD += base.dbd
Dummy_DBD += initTrace.dbd
Dummy_DBD += sfwDBD.dbd

# Dummy_registerRecordDeviceDriver.cpp derives from Dummy.dbd
Dummy_SRCS += Dummy_registerRecordDeviceDriver.cpp

Dummy_SRCS += initTrace.c
Dummy_SRCS += appExample.c

# Build the main IOC entry point on workstation OSs.
Dummy_SRCS_DEFAULT += DummyMain.cpp
Dummy_SRCS_vxWorks += -nil-

# Link in the code from the support library
Dummy_LIBS += sfwLib

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into DummySupport
    sncExample_SNCFLAGS += +r
    Dummy_DBD += sncExample.dbd
    DummySupport_SRCS += sncExample.stt
    DummySupport_LIBS += seq pv
    Dummy_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
Dummy_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

