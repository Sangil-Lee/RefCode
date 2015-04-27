TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#USR_INCLUDES    += -I$(EPICS_SITE)/include
#USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

USR_CFLAGS += -D_CLTU_VXWORKS_
#USR_CFLAGS += -D_CLTU_LINUX_



#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = DDS1AB_LTU
# DDS1AB_LTU.dbd will be created and installed
DBD += DDS1AB_LTU.dbd

# DDS1AB_LTU.dbd will be made up from these files:
DDS1AB_LTU_DBD += base.dbd
DDS1AB_LTU_DBD += initTrace.dbd
DDS1AB_LTU_DBD += cltuBase.dbd

# DDS1AB_LTU_registerRecordDeviceDriver.cpp derives from DDS1AB_LTU.dbd
DDS1AB_LTU_SRCS += DDS1AB_LTU_registerRecordDeviceDriver.cpp
DDS1AB_LTU_SRCS += initTrace.c

DDS1AB_LTU_SRCS += devCLTU.c
DDS1AB_LTU_SRCS += drvCLTU.c
DDS1AB_LTU_SRCS += oz_cltu.c
DDS1AB_LTU_SRCS += oz_xtu2.c


# Build the main IOC entry point on workstation OSs.
DDS1AB_LTU_SRCS_DEFAULT += DDS1AB_LTUMain.cpp
DDS1AB_LTU_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#DDS1AB_LTU_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into DDS1AB_LTUSupport
    sncExample_SNCFLAGS += +r
    DDS1AB_LTU_DBD += sncExample.dbd
    DDS1AB_LTUSupport_SRCS += sncExample.stt
    DDS1AB_LTUSupport_LIBS += seq pv
    DDS1AB_LTU_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
DDS1AB_LTU_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

