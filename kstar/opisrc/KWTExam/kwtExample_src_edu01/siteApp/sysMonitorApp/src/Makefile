TOP=../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)

PROD_IOC = sysMonitor

DBD += sysMonitor.dbd

sysMonitor_DBD += base.dbd
sysMonitor_DBD += sysMon.dbd

sysMonitor_SRCS += sysMonitor_registerRecordDeviceDriver.cpp
sysMonitor_SRCS_DEFAULT += sysMonitorMain.cpp
sysMonitor_LIBS += sysMon
sysMonitor_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES

