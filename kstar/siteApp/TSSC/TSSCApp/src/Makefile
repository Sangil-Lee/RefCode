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


#TSSCSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = TSSC
# TSSC.dbd will be created and installed
DBD += TSSC.dbd

# TSSC.dbd will be made up from these files:
TSSC_DBD += base.dbd
TSSC_DBD += initTrace.dbd
TSSC_DBD += dbdTSSC.dbd

TSSC_DBD += sfwDBD.dbd

# TSSC_registerRecordDeviceDriver.cpp derives from TSSC.dbd
TSSC_SRCS += TSSC_registerRecordDeviceDriver.cpp

TSSC_SRCS += initTrace.c
TSSC_SRCS += devTSSC.c
TSSC_SRCS += drvTSSC.c

# Build the main IOC entry point on workstation OSs.
TSSC_SRCS_DEFAULT += TSSCMain.cpp
#TSSC_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#TSSC_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#TSSC_LIBS += TSSCSupport
TSSC_LIBS += sfwLib

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into TSSCSupport
    TSSC_SNCFLAGS += +r
    TSSC_DBD += sncSeqTSSC.dbd
    TSSC_SRCS += sncSeqTSSC.stt
    TSSC_LIBS += seq pv

endif

# Finally link to the EPICS Base libraries
TSSC_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

