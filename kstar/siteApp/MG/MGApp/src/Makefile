TOP=../..

include $(TOP)/configure/CONFIG

COGENT=/usr/cogent
USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_INCLUDES += -I$(EPICS_PATH)/extensions/include
USR_INCLUDES += -I$(COGENT)/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
#recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#nicFP20x0_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
#apsEvent_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
Tools_DIR += $(EPICS_EXTENSIONS)/lib/$(T_A)
cogent_DIR += $(COGENT)/lib

PROD_IOC = MGApp

DBD += MGApp.dbd

MGApp_DBD += base.dbd
MGApp_DBD += sysMon.dbd
#MGApp_DBD += timestampRecord.dbd
#MGApp_DBD += asyn.dbd
#MGApp_DBD += devGpib.dbd
#MGApp_DBD += drvGenericSerial.dbd
#MGApp_DBD += drvnicFP20x0.dbd
#MGApp_DBD += devnicFP20x0.dbd
#MGApp_DBD += apsEvent.dbd
MGApp_DBD += subIfDriver.dbd
MGApp_DBD += subHealthState.dbd

MGApp_SRCS += MGApp_registerRecordDeviceDriver.cpp
MGApp_SRCS += subIfDriver.cpp
MGApp_SRCS += subHealthState.cpp
MGApp_SRCS += userclass.cpp

ifneq ($(SNCSEQ),)
	MGApp_SNCFLAGS += +r
	MGApp_DBD  += sncMGInterface.dbd 
	MGApp_SRCS += sncMGInterface.stt
	MGApp_LIBS += seq pv

endif

MGApp_SRCS_DEFAULT += MGAppMain.cpp
MGApp_LIBS += sysMon
#MGApp_LIBS += asyn
#MGApp_LIBS += recTimeStamp
#MGApp_LIBS += nicFP20x0
#MGApp_LIBS += apsEvent
MGApp_LIBS += $(EPICS_BASE_IOC_LIBS)
MGApp_LIBS += Tools
MGApp_LIBS += cogent

include $(TOP)/configure/RULES

