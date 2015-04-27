TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
#sysMon_DIR += $(EPICS_SITE)/lib/$(T_A)
#recTimeStamp_DIR += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library


#=============================
# Build the IOC application

PROD_IOC = LTU
# LTU.dbd will be created and installed
DBD += LTU.dbd

# LTU.dbd will be made up from these files:
LTU_DBD += base.dbd
LTU_DBD += initTrace.dbd

LTU_DBD += cltuBase.dbd
#LTU_DBD += sysMon.dbd
#LTU_DBD += timestampRecord.dbd

# LTU_registerRecordDeviceDriver.cpp derives from LTU.dbd
LTU_SRCS += LTU_registerRecordDeviceDriver.cpp

LTU_SRCS += initTrace.c

# Build the main IOC entry point on workstation OSs.
LTU_SRCS_DEFAULT += LTUMain.cpp
LTU_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#LTU_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library

LTU_LIBS += cltuDev
#LTU_LIBS += sysMon
#LTU_LIBS += recTimeStamp

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into LTUSupport
    sncExample_SNCFLAGS += +r
    LTU_DBD += sncExample.dbd
    LTUSupport_SRCS += sncExample.stt
    LTUSupport_LIBS += seq pv
    LTU_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
LTU_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

