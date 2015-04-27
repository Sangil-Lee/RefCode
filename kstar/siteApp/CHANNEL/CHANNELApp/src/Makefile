TOP=../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_INCLUDES += -I$(EPICS_PATH)/extensions/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
#recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
Tools_DIR += $(EPICS_EXTENSIONS)/lib/$(T_A)

PROD_IOC = sysMonitor

DBD += sysMonitor.dbd

sysMonitor_DBD += base.dbd
sysMonitor_DBD += sysMon.dbd
#sysMonitor_DBD += timestampRecord.dbd
sysMonitor_DBD += subCHDataSize.dbd
sysMonitor_DBD += subIfDriver.dbd
sysMonitor_DBD += subHealthState.dbd

sysMonitor_SRCS += sysMonitor_registerRecordDeviceDriver.cpp
sysMonitor_SRCS += subCHDataSize.cpp
sysMonitor_SRCS += subIfDriver.cpp
sysMonitor_SRCS += subHealthState.cpp
sysMonitor_SRCS += userclass.cpp

ifneq ($(SNCSEQ),)
	sysMonitor_SNCFLAGS += +r
	sysMonitor_LIBS += seq pv
endif

sysMonitor_SRCS_DEFAULT += sysMonitorMain.cpp
sysMonitor_LIBS += sysMon
#sysMonitor_LIBS += recTimeStamp
sysMonitor_LIBS += $(EPICS_BASE_IOC_LIBS)
sysMonitor_LIBS += Tools

include $(TOP)/configure/RULES

