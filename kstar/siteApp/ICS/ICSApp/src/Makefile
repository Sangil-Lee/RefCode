TOP=../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_INCLUDES += -I$(EPICS_PATH)/extensions/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
nicFP20x0_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
asyn_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
apsEvent_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
Tools_DIR += $(EPICS_EXTENSIONS)/lib/$(T_A)
sybdb_DIR += /usr/local/lib

PROD_IOC = ICSApp

DBD += ICSApp.dbd

ICSApp_DBD += base.dbd
ICSApp_DBD += sysMon.dbd
ICSApp_DBD += timestampRecord.dbd
ICSApp_DBD += asyn.dbd
ICSApp_DBD += devGpib.dbd
ICSApp_DBD += drvGenericSerial.dbd
ICSApp_DBD += drvnicFP20x0.dbd
ICSApp_DBD += devnicFP20x0.dbd
ICSApp_DBD += apsEvent.dbd
ICSApp_DBD += ICMPSubProc.dbd
ICSApp_DBD += subNextShotTime.dbd
ICSApp_DBD += subIfDriver.dbd
ICSApp_DBD += subHealthState.dbd
ICSApp_DBD += subCTRLRoomTemp.dbd
ICSApp_DBD += subSVRRoomTemp.dbd

ICSApp_SRCS += ICSApp_registerRecordDeviceDriver.cpp
ICSApp_SRCS += icmpsocket.cpp
ICSApp_SRCS += ICMPSubProc.cpp
ICSApp_SRCS += subNextShotTime.cpp
ICSApp_SRCS += subIfDriver.cpp
ICSApp_SRCS += subHealthState.cpp
ICSApp_SRCS += subSVRRoomTemp.cpp
ICSApp_SRCS += subCTRLRoomTemp.cpp
ICSApp_SRCS += userclass.cpp

ICSApp_SRCS_DEFAULT += ICSAppMain.cpp
ICSApp_LIBS += sysMon
ICSApp_LIBS += asyn
ICSApp_LIBS += recTimeStamp
ICSApp_LIBS += nicFP20x0
ICSApp_LIBS += apsEvent
ICSApp_LIBS += $(EPICS_BASE_IOC_LIBS)
ICSApp_LIBS += Tools
ICSApp_LIBS += sybdb


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
# This builds sncExample as a component of tr1
	ICSApp_SNCFLAGS += +r
	ICSApp_DBD += sncMonsvrstatus.dbd
	ICSApp_DBD += sncCurTimeSeq.dbd
	ICSApp_DBD += sncSetResetTimeToShot.dbd
#ICSApp_DBD += sncCWFSet.dbd
	ICSApp_SRCS += sncMonsvrstatus.stt
	ICSApp_SRCS += sncCurTimeSeq.stt
	ICSApp_SRCS += sncSetResetTimeToShot.stt
#ICSApp_SRCS += sncCWFSet.stt
	ICSApp_LIBS += seq pv
	ICSApp_LIBS += $(EPICS_BASE_HOST_LIBS)

	PROD_HOST += sncRDBUpdateSeq
	sncRDBUpdateSeq_SNCFLAGS += +m
	sncRDBUpdateSeq_DBD  += sncRDBUpdateSeq.dbd
	sncRDBUpdateSeq_SRCS += sncRDBUpdateSeq.stt
	sncRDBUpdateSeq_LIBS += seq pv sybdb
	sncRDBUpdateSeq_LIBS += $(EPICS_BASE_HOST_LIBS)

	PROD_HOST += sncCASchedulerSeq
	sncCASchedulerSeq_SNCFLAGS += +m
	sncCASchedulerSeq_DBD  += sncCASchedulerSeq.dbd
	sncCASchedulerSeq_SRCS += sncCASchedulerSeq.stt
	sncCASchedulerSeq_LIBS += seq pv
	sncCASchedulerSeq_LIBS += $(EPICS_BASE_HOST_LIBS)

	PROD_HOST += sncCWFSet
	sncCWFSet_SNCFLAGS += +m
	sncCWFSet_DBD  += sncCWFSet.dbd
	sncCWFSet_SRCS += sncCWFSet.stt
	sncCWFSet_LIBS += seq pv
	sncCWFSet_LIBS += $(EPICS_BASE_HOST_LIBS)

endif
include $(TOP)/configure/RULES
