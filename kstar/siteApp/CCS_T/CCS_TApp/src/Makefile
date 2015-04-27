TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
USR_INCLUDEs += -I$(EPICS_PATH)/siteLib/include
vmivme2534_DIR = $(EPICS_PATH)/siteLib/lib/$(T_A)
#vmivme5565Rfm_DIR = $(EPICS_PATH)/siteLib/lib/$(T_A)
vxStats_DIR = $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)


#==================================================
# Build an IOC support library

PROD_IOC_vxWorks = CCS_T
LIBRARY_IOC_vxWorks = drvSysMon

# CCS_T.dbd will be created and installed
DBD += CCS_T.dbd

# t.dbd will be made up from these files:
CCS_T_DBD += base.dbd
CCS_T_DBD += devVmic2534.dbd
#CCS_T_DBD += rfmSupport.dbd
CCS_T_DBD += ether_ip.dbd
CCS_T_DBD += devStats.dbd
CCS_T_DBD += asyn.dbd
CCS_T_DBD += devGpib.dbd
CCS_T_DBD += drvGenericSerial.dbd
CCS_T_DBD += timestampRecord.dbd
CCS_T_DBD += drvnicFP20x0.dbd
CCS_T_DBD += devnicFP20x0.dbd
#CCS_T_DBD += rfmPartition.dbd
CCS_T_DBD += subHealthState.dbd

#CCS_T_DBD += apsEvent.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
CCS_T_SRCS += CCS_T_registerRecordDeviceDriver.cpp
CCS_T_SRCS_DEFAULT += CCS_TMain.cpp
CCS_T_SRCS_vxWorks += -nil-

# Add locally compiled object code
drvSysMon_SRCS += subHealthState.cpp

# The following adds support from base/src/vxWorks
CCS_T_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

CCS_T_LIBS += recTimeStamp
CCS_T_LIBS += asyn
CCS_T_LIBS += nicFP20x0
CCS_T_LIBS += vmivme2534
#CCS_T_LIBS += vmivme5565Rfm
CCS_T_LIBS += ether_ip
CCS_T_LIBS += vxStats
CCS_T_LIBS += drvSysMon
#CCS_T_LIBS += rfmPartition

#CCS_T_LIBS += apsEvent

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of CCS_T
    CCS_T_SNCFLAGS += +r

    CCS_T_DBD  += sncPCSWatchdog.dbd
    CCS_T_SRCS += sncPCSWatchdog.stt

    CCS_T_DBD  += sncMPSWatchdog.dbd
    CCS_T_SRCS += sncMPSWatchdog.stt
    
    CCS_T_SRCS += sncPF1Watchdog.stt
    CCS_T_SRCS += sncPF2Watchdog.stt
    CCS_T_SRCS += sncPF3LWatchdog.stt
    CCS_T_SRCS += sncPF3UWatchdog.stt
    CCS_T_SRCS += sncPF4LWatchdog.stt
    CCS_T_SRCS += sncPF4UWatchdog.stt
    CCS_T_SRCS += sncPF5LWatchdog.stt
    CCS_T_SRCS += sncPF5UWatchdog.stt
    CCS_T_SRCS += sncPF6LWatchdog.stt
    CCS_T_SRCS += sncPF6UWatchdog.stt
    CCS_T_SRCS += sncPF7Watchdog.stt
    CCS_T_SRCS += sncTFWatchdog.stt
    CCS_T_SRCS += sncIVCWatchdog.stt

    

#    CCS_T_DBD += sntpcClient.dbd
#    CCS_T_SRCS += sntpcClient.stt
    #CCS_T_SRCS += ntpClient.c

    CCS_T_LIBS += seq pv

    # The following builds sncProgram as a standalone application
#    PROD_HOST += sncProgram
#    sncProgram_SNCFLAGS += +m
#    sncProgram_SRCS += sncProgram.st
#    sncProgram_LIBS += seq pv
#    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

CCS_T_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

