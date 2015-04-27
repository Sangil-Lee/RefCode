TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES	    += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS 	    += -I. -I.. -I../../../dbd -I$(EPICS_PATH)/siteLib/dbd
USR_DBDFLAGS 	    += -I$(EPICS_PATH)/siteLib/dbd


USR_LDFLAGS         += -L/usr/local/lib -lnidaqmx
nidaqmx_DIRS        = /usr/local/lib

asyn_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)
nicFP20x0_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)
# readline_DIR	+= /usr/local/lib
autosave_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)

#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport
LIBRARY_IOC += drvPXISCXI

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)


DBD		+= drvPXISCXI.dbd
DBD  		+= devPXISCXI.dbd
drvPXISCXI_SRCS += drvPXISCXI.c
drvPXISCXI_SRCS += devPXISCXI.c
drvPXISCXI_LIBS += $(EPICS_BASE_IOC_LIBS)




#=============================
# build an ioc application

PROD_IOC = TMS2 
# TMS2.dbd will be created and installed
DBD += TMS2.dbd

# TMS2.dbd will be made up from these files:
TMS2_DBD += base.dbd
TMS2_DBD += timestampRecord.dbd
TMS2_DBD += asyn.dbd
TMS2_DBD += drvGenericSerial.dbd
#TMS2_DBD += drvnicFP20x0.dbd
#TMS2_DBD += devnicFP20x0.dbd
TMS2_DBD += xxxSupport.dbd
TMS2_DBD += dbSubExample.dbd
TMS2_DBD += TMS2Hello.dbd
TMS2_DBD += drvPXISCXI.dbd
TMS2_DBD += devPXISCXI.dbd
TMS2_DBD += sysMon.dbd
TMS2_DBD += asSupport.dbd
TMS2_DBD += subIfDriver.dbd 
TMS2_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
TMS2_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
TMS2_SRCS += TMS2_registerRecordDeviceDriver.cpp
TMS2_SRCS_DEFAULT += TMS2Main.c
TMS2_SRCS_vxWorks += -nil-

# Add locally compiled object code
TMS2_SRCS += dbSubExample.c
TMS2_SRCS += TMS2Hello.c
TMS2_SRCS += subIfDriver.cpp 
TMS2_SRCS += subHealthState.cpp
TMS2_SRCS += userclass.cpp 

# The following adds support from base/src/vxWorks
TMS2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#TMS2_LIBS += nicFP20x0
TMS2_LIBS += recTimeStamp
TMS2_LIBS += asyn
TMS2_LIBS += drvPXISCXI
TMS2_LIBS += xxxSupport
TMS2_LIBS += sysMon 
TMS2_LIBS += autosave 

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of TMS2
    TMS2_SNCFLAGS += +r
    TMS2_DBD += sncExample.dbd
    TMS2_SRCS += sncExample.stt
    TMS2_SRCS += sncTmsMain.stt
    TMS2_SRCS += sncTmsIntLevel1.stt
    TMS2_SRCS += sncTmsIntLevel2.stt
    TMS2_SRCS += sncTmsIntLevel3.stt
    TMS2_SRCS += sncTmsIntLevel4.stt
    TMS2_SRCS += sncTmsIntLevel5.stt
    TMS2_SRCS += sncTmsHeartRun.stt
    TMS2_SRCS += sncTmsDhsZero.stt
    TMS2_SRCS += sncTmsStrainZeroSet.stt
    TMS2_SRCS += sncTmsWatchdog.stt
    TMS2_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

TMS2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

