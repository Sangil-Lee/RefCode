TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
#USR_INCLUDES += -I/usr/local/natinst/nidaqmx/include
USR_INCLUDES += -I/usr/local/mdsplus/include
#USR_LDFLAGS += -L/usr/local/natinst/nidaqmx -lnidaqmx
USR_SYS_LIBS += nidaqmx

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
USR_LDFLAGS += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)

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

PROD_IOC = ECCD_DAQ
# ECCD_DAQ.dbd will be created and installed
DBD += ECCD_DAQ.dbd

# ECCD_DAQ.dbd will be made up from these files:
ECCD_DAQ_DBD += base.dbd
ECCD_DAQ_DBD += drvNiPci6254.dbd
ECCD_DAQ_DBD += drvNiPci6220.dbd
ECCD_DAQ_DBD += drvMdsplus.dbd
ECCD_DAQ_DBD += devNiPci6254.dbd
ECCD_DAQ_DBD += devNiPci6220.dbd
ECCD_DAQ_DBD += devMdsplus.dbd
ECCD_DAQ_DBD += devPoloScan.dbd
ECCD_DAQ_DBD += drvPoloidalScan.dbd

ECCD_DAQ_DBD += timestampRecord.dbd
ECCD_DAQ_DBD += sysMon.dbd         # CSM
ECCD_DAQ_DBD += subIfDriver.dbd
ECCD_DAQ_DBD += subHealthState.dbd
ECCD_DAQ_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ECCD_DAQ_SRCS += ECCD_DAQ_registerRecordDeviceDriver.cpp
ECCD_DAQ_SRCS_DEFAULT += ECCD_DAQMain.cpp
ECCD_DAQ_SRCS_vxWorks += -nil-

# Add locally compiled object code
ECCD_DAQ_SRCS += drvNiPci6254.cpp
ECCD_DAQ_SRCS += devNiPci6254.c
ECCD_DAQ_SRCS += drvNiPci6220.cpp
ECCD_DAQ_SRCS += drvMdsplus.cpp
ECCD_DAQ_SRCS += devNiPci6220.c
ECCD_DAQ_SRCS += devMdsplus.c
ECCD_DAQ_SRCS += daqMdsplus.c
ECCD_DAQ_SRCS += nidaq_cmd.c
ECCD_DAQ_SRCS += drvPoloidalScan.cpp
ECCD_DAQ_SRCS += devPoloScan.c

ECCD_DAQ_SRCS += subIfDriver.cpp
ECCD_DAQ_SRCS += subHealthState.cpp
ECCD_DAQ_SRCS += userclass.cpp

# The following adds support from base/src/vxWorks
ECCD_DAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

ECCD_DAQ_LIBS += xxxSupport
ECCD_DAQ_LIBS += recTimeStamp
ECCD_DAQ_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of ECCD_DAQ
    ECCD_DAQ_SNCFLAGS += +r
    ECCD_DAQ_DBD += sncExample.dbd
    ECCD_DAQ_SRCS += sncExample.stt
    ECCD_DAQ_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

ECCD_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

