TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE


TSSGSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = TSSG
# TSSG.dbd will be created and installed
DBD += TSSG.dbd

# TSSG.dbd will be made up from these files:
TSSG_DBD += base.dbd
TSSG_DBD += initTrace.dbd
TSSG_DBD += devTSSG.dbd

# TSSG_registerRecordDeviceDriver.cpp derives from TSSG.dbd
TSSG_SRCS += TSSG_registerRecordDeviceDriver.cpp

TSSG_SRCS += initTrace.c
TSSG_SRCS += devTSSG.c

# Build the main IOC entry point on workstation OSs.
TSSG_SRCS_DEFAULT += TSSGMain.cpp
#TSSG_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#TSSG_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#TSSG_LIBS += TSSGSupport

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into TSSGSupport
    TSSG_SNCFLAGS += +r
    TSSG_DBD += sncRuntimeSeq.dbd
    TSSG_SRCS += sncRuntimeSeq.stt
    TSSG_LIBS += seq pv

endif

# Finally link to the EPICS Base libraries
TSSG_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

