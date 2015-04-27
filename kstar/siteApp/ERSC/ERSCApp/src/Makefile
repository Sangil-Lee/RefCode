TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
ifeq ($(T_A),linux-ppc)
USR_LDFLAGS     += -L/usr/local/mdsplus-ppc-4.0/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lMdsIpShr -lc
else 
USR_LDFLAGS     += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc
endif
USR_INCLUDES    += -I$(EPICS_SITE)/include 
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include 
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

sfwLib_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A) 
#asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
kutil_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
ktree_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)



#ERSCSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = ERSC
# ERSC.dbd will be created and installed
DBD += ERSC.dbd

# ERSC.dbd will be made up from these files:
ERSC_DBD += base.dbd
ERSC_DBD += initTrace.dbd
ERSC_DBD += dbdERSC.dbd

ERSC_DBD += sfwDBD.dbd
ERSC_DBD += kutil.dbd
ERSC_DBD += sfwTree.dbd

# ERSC_registerRecordDeviceDriver.cpp derives from ERSC.dbd
ERSC_SRCS += ERSC_registerRecordDeviceDriver.cpp

ERSC_SRCS += initTrace.c
ERSC_SRCS += devERSC.c
ERSC_SRCS += drvERSC.c
ERSC_SRCS += myMDSplus.c

# Build the main IOC entry point on workstation OSs.
ERSC_SRCS_DEFAULT += ERSCMain.cpp
#ERSC_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#ERSC_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#ERSC_LIBS += ERSCSupport
ERSC_LIBS += sfwLib
ERSC_LIBS += kutil
ERSC_LIBS += ktree


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into ERSCSupport
    ERSC_SNCFLAGS += +r
    ERSC_DBD += sncRuntimeSeq.dbd
    ERSC_SRCS += sncRuntimeSeq.stt
    ERSC_LIBS += seq pv

endif

# Finally link to the EPICS Base libraries
ERSC_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

