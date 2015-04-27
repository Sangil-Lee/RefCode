TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
#USR_LIBS += TreeShr TdiShr MdsLib MdsShr 

USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR       += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR      += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = TXPC
# TXPC.dbd will be created and installed
DBD += TXPC.dbd

# TXPC.dbd will be made up from these files:
TXPC_DBD += base.dbd
TXPC_DBD += initTrace.dbd
#TXPC_DBD += timestampRecord.dbd
#TXPC_DBD += sysMon.dbd
#TXPC_DBD += cltuBase.dbd
TXPC_DBD += sfwDBD.dbd
TXPC_DBD += dbdDummy.dbd


# TXPC_registerRecordDeviceDriver.cpp derives from TXPC.dbd
TXPC_SRCS += TXPC_registerRecordDeviceDriver.cpp
TXPC_SRCS += initTrace.c

TXPC_SRCS += drvDummy.c
TXPC_SRCS += devDummy.c


# Build the main IOC entry point on workstation OSs.
TXPC_SRCS_DEFAULT += TXPCMain.cpp
TXPC_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#TXPC_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#TXPC_LIBS += TXPCSupport

#TXPC_LIBS += cltuDev
#TXPC_LIBS += recTimeStamp
#TXPC_LIBS += sysMon
TXPC_LIBS += sfwLib

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into TXPCSupport
    sncExample_SNCFLAGS += +r
    TXPC_DBD += sncExample.dbd
    TXPCSupport_SRCS += sncExample.stt
    TXPCSupport_LIBS += seq pv
    TXPC_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
TXPC_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

