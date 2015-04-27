TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I. -I.. -I../../../dbd -I$(EPICS_PATH)/siteLib/dbd
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd
#==================================================
# Build an IOC support library
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)


#=============================
# build an ioc application

PROD_IOC = CCS_J
# j.dbd will be created and installed
DBD += CCS_J.dbd

# j.dbd will be made up from these files:
CCS_J_DBD += base.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
CCS_J_DBD += pvCrawler.dbd
CCS_J_DBD += sysMon.dbd
CCS_J_DBD += subHealthState.dbd
CCS_J_DBD += subIfDriver.dbd
#2013. 7. 9
CCS_J_DBD += ether_ip.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
CCS_J_SRCS += CCS_J_registerRecordDeviceDriver.cpp
CCS_J_SRCS += subHealthState.cpp
CCS_J_SRCS += subIfDriver.cpp
CCS_J_SRCS += userclass.cpp
CCS_J_SRCS_DEFAULT += CCS_JMain.cpp
CCS_J_SRCS_vxWorks += -nil-

# Add locally compiled object code

# The following adds support from base/src/vxWorks
CCS_J_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of j
    CCS_J_SNCFLAGS += +r
    CCS_J_DBD  += sncShotSeq.dbd
    CCS_J_SRCS += sncShotSeq.stt
    CCS_J_DBD += sncLongTermSeq.dbd
    CCS_J_SRCS += sncLongTermSeq.stt
    CCS_J_DBD += sncDiagnosticCheck.dbd
    CCS_J_SRCS += sncDiagnosticCheck.stt
    CCS_J_DBD += sncFaultCheck.dbd
    CCS_J_SRCS += sncFaultCheck.stt
    CCS_J_DBD += sncSoftInterlockCheck.dbd
    CCS_J_SRCS += sncSoftInterlockCheck.stt
    CCS_J_DBD += sncMDSTreeSeq.dbd
    CCS_J_SRCS += sncMDSTreeSeq.stt
# add 2012. 8. 29 move from CCS_T
	CCS_J_DBD += sncMPSWatchdog.dbd
    CCS_J_SRCS += sncMPSWatchdog.stt
    CCS_J_DBD += sncSoftLanding.dbd
    CCS_J_SRCS += sncSoftLanding.stt
    CCS_J_DBD += sncRuntimeMonitor.dbd
    CCS_J_SRCS += sncRuntimeMonitor.stt
    CCS_J_LIBS += seq pv

    # The following builds sncProgram as a standalone application
#    PROD_HOST += sncProgram
#    sncProgram_SNCFLAGS += +m
#    sncProgram_SRCS += sncProgram.st
#    sncProgram_LIBS += seq pv
#    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

CCS_J_LIBS += $(EPICS_BASE_IOC_LIBS)
CCS_J_LIBS += sysMon
CCS_J_LIBS += ether_ip

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

