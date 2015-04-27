TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
GSC_DIR=/home/woong/16aiss8ao4

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_INCLUDES    += -I$(EPICS_SITE)/intLtuLib/driver
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include

USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

USR_LDFLAGS += -L$(GSC_DIR)/utils -l16aiss8ao4_utils
USR_LDFLAGS += -L$(GSC_DIR)/docsrc -l16aiss8ao4_dsl
USR_INCLUDES += -I$(GSC_DIR)/driver -I$(GSC_DIR)/utils -I$(GSC_DIR)/docsrc



sfwLib_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)


#=============================
# Build the IOC support library


#=============================
# Build the IOC application

PROD_IOC = KPCS
# KPCS.dbd will be created and installed
DBD += KPCS.dbd

# KPCS.dbd will be made up from these files:
KPCS_DBD += base.dbd
#KPCS_DBD += xxxSupport.dbd
#KPCS_DBD += dbSubExample.dbd
#KPCS_DBD += KPCSHello.dbd
#KPCS_DBD += initTrace.dbd
KPCS_DBD += sfwDBD.dbd
KPCS_DBD += dbdRTCORE.dbd
KPCS_DBD += dbdDNST.dbd

# KPCS_registerRecordDeviceDriver.cpp derives from KPCS.dbd
KPCS_SRCS += KPCS_registerRecordDeviceDriver.cpp

KPCS_SRCS += devRTCORE.c
KPCS_SRCS += drvRTCORE.c

KPCS_SRCS += CodacPcsRun.cpp
KPCS_SRCS += CodacPcsRunStub.cpp
KPCS_SRCS += devDensity.c
KPCS_SRCS += drvDensity.c


# Build the main IOC entry point on workstation OSs.
KPCS_SRCS_DEFAULT += KPCSMain.cpp
KPCS_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#KPCS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#KPCS_LIBS += KPCSSupport
KPCS_LIBS += sfwLib


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into KPCSSupport
    sncExample_SNCFLAGS += +r
    KPCS_DBD += sncExample.dbd
    KPCSSupport_SRCS += sncExample.stt
    KPCSSupport_LIBS += seq pv
    KPCS_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
KPCS_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

