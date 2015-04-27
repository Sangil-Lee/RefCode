TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_INCLUDES    += -I/usr/local/include
USR_INCLUDES    += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS    += -I$(EPICS_PATH)/siteLib/dbd

asyn_DIR        += $(EPICS_PATH)/siteLib/lib/$(T_A)
nicFP20x0_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR        += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR      += $(EPICS_PATH)/siteLib/lib/$(T_A)


#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = DLS
# DLS.dbd will be created and installed
DBD += DLS.dbd

# DLS.dbd will be made up from these files:
DLS_DBD += base.dbd
DLS_DBD += timestampRecord.dbd
DLS_DBD += asyn.dbd
DLS_DBD += drvGenericSerial.dbd
DLS_DBD += drvnicFP20x0.dbd
DLS_DBD += devnicFP20x0.dbd
DLS_DBD += xxxSupport.dbd
DLS_DBD += dbSubExample.dbd
DLS_DBD += DLSHello.dbd
DLS_DBD += sysMon.dbd
DLS_DBD += subIfDriver.dbd
DLS_DBD += subHealthState.dbd


# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
DLS_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
DLS_SRCS += DLS_registerRecordDeviceDriver.cpp
DLS_SRCS += subIfDriver.cpp
DLS_SRCS += subHealthState.cpp
DLS_SRCS += userclass.cpp
DLS_SRCS_DEFAULT += DLSMain.cpp
DLS_SRCS_vxWorks += -nil-

# Add locally compiled object code
DLS_SRCS += dbSubExample.c
DLS_SRCS += DLSHello.c

# The following adds support from base/src/vxWorks
DLS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

DLS_LIBS += nicFP20x0
DLS_LIBS += recTimeStamp
DLS_LIBS += asyn
DLS_LIBS += xxxSupport
DLS_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of DLS
    DLS_SNCFLAGS += +r
    DLS_DBD += sncExample.dbd
    DLS_SRCS += sncExample.stt
    DLS_SRCS += sncDlsStop.stt
    DLS_SRCS += sncDlsInterlock.stt
    DLS_SRCS += sncDlsRBAStatus.stt
    DLS_SRCS += sncDlsCSTA.stt 
    DLS_SRCS += sncDlsCSTD.stt
    DLS_SRCS += sncDlsCSTI.stt
    DLS_SRCS += sncDlsCSTM.stt
    DLS_SRCS += sncDlsECEI.stt
    DLS_SRCS += sncDlsECEI2.stt
    DLS_SRCS += sncDlsTSC.stt
    DLS_SRCS += sncDlsTSE.stt
    DLS_SRCS += sncDlsTSG.stt
    DLS_SRCS += sncDlsXCSB.stt
    DLS_SRCS += sncDlsXCSB1.stt
    DLS_SRCS += sncDlsXCSB2.stt
    DLS_SRCS += sncDlsXPCF.stt
    DLS_SRCS += sncDlsXCSF.stt
    DLS_SRCS += sncDlsXCSF1.stt
    DLS_SRCS += sncDlsXCSF2.stt
    DLS_SRCS += sncDlsXCSF3.stt
    DLS_SRCS += sncDlsTHA1.stt
    DLS_SRCS += sncDlsPHA1.stt
    DLS_SRCS += sncDlsPHA2.stt
    DLS_SRCS += sncDlsVSS1.stt
    DLS_SRCS += sncDlsVSS2.stt
    DLS_SRCS += sncDlsProctect.stt 
    DLS_SRCS += sncECEIProctect.stt 
    DLS_SRCS += sncDlsTV01.stt
    DLS_SRCS += sncDlsTV02.stt
    DLS_SRCS += sncDlsAllShutterMon.stt	
    DLS_SRCS += sncDlsLIBEAM.stt

    DLS_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

DLS_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

