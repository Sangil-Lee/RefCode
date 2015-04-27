TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES	    += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS 	    += -I$(EPICS_PATH)/siteLib/dbd

USR_LDFLAGS         += -L/usr/local/lib -lnidaqmx
nidaqmx_DIRS          = /usr/local/lib

asyn_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)
nicFP20x0_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR	    += $(EPICS_PATH)/siteLib/lib/$(T_A)
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
DBD             += menuConvert.dbd
DBD             += bptPT100.dbd

drvPXISCXI_SRCS += drvPXISCXI.c
drvPXISCXI_SRCS += devPXISCXI.c
drvPXISCXI_LIBS += $(EPICS_BASE_IOC_LIBS)




#=============================
# build an ioc application

PROD_IOC = TMS1
# TMS1.dbd will be created and installed
DBD += TMS1.dbd

# TMS1.dbd will be made up from these files:
TMS1_DBD += base.dbd
TMS1_DBD += timestampRecord.dbd
TMS1_DBD += asyn.dbd
TMS1_DBD += drvGenericSerial.dbd
TMS1_DBD += drvnicFP20x0.dbd
TMS1_DBD += devnicFP20x0.dbd
TMS1_DBD += xxxSupport.dbd
TMS1_DBD += dbSubExample.dbd
TMS1_DBD += TMS1Hello.dbd
TMS1_DBD += drvPXISCXI.dbd
TMS1_DBD += devPXISCXI.dbd
TMS1_DBD += bptPT100.dbd
TMS1_DBD += sysMon.dbd
TMS1_DBD += asSupport.dbd
TMS1_DBD += subIfDriver.dbd
TMS1_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
TMS1_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
TMS1_SRCS += TMS1_registerRecordDeviceDriver.cpp
TMS1_SRCS_DEFAULT += TMS1Main.c
TMS1_SRCS_vxWorks += -nil-

# Add locally compiled object code
TMS1_SRCS += dbSubExample.c
TMS1_SRCS += TMS1Hello.c
TMS1_SRCS += subIfDriver.cpp
TMS1_SRCS += subHealthState.cpp
TMS1_SRCS += userclass.cpp

# The following adds support from base/src/vxWorks
pxi_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

TMS1_LIBS += nicFP20x0
TMS1_LIBS += recTimeStamp
TMS1_LIBS += asyn
TMS1_LIBS += drvPXISCXI
TMS1_LIBS += xxxSupport
TMS1_LIBS += sysMon
TMS1_LIBS += autosave

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of pxi
    TMS1_SNCFLAGS += +r
    TMS1_DBD += sncExample.dbd
    TMS1_SRCS += sncExample.stt
    TMS1_SRCS += sncTmsMax.stt
    TMS1_SRCS += sncTmsMain.stt
    TMS1_SRCS += sncTmsMainLevel.stt
    TMS1_SRCS += sncTmsMainCon.stt
    TMS1_SRCS += sncTmsHallReverse.stt
    TMS1_SRCS += sncTmsHallZero.stt
    TMS1_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

TMS1_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

