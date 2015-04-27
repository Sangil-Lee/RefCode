TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES += -I/usr/local/mdsplus/include
USR_INCLUDES += -I/usr/local/include

USR_LDFLAGS += -L/usr/local/lib
USR_SYS_LIBS += nidaqmx
USR_LDFLAGS += -L/usr/local/mdsplus/lib
USR_SYS_LIBS += TreeShr TdiShr MdsLib MdsShr c popt

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#USR_CFLAGS += -D_NI_int32_DEFINED_

USR_INCLUDES    += -I/usr/local/epics/siteLib/include
USR_DBDFLAGS    += -I/usr/local/epics/siteLib/dbd

recTimeStamp_DIR+= $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR      += $(EPICS_PATH)/siteLib/lib/$(T_A)

#==================================================
# Build an IOC support library

#=============================
# build an ioc application

PROD_IOC = LHCD_DAQ
# LHCD_DAQ.dbd will be created and installed
DBD += LHCD_DAQ.dbd

# LHCD_DAQ.dbd will be made up from these files:
LHCD_DAQ_DBD += base.dbd
LHCD_DAQ_DBD += drvNI6220.dbd
LHCD_DAQ_DBD += drvNI6254.dbd
LHCD_DAQ_DBD += drvMdsplus.dbd
LHCD_DAQ_DBD += devNI6220.dbd
LHCD_DAQ_DBD += devNI6254.dbd
LHCD_DAQ_DBD += devMdsplus.dbd
LHCD_DAQ_DBD += drvRFCTRL.dbd
LHCD_DAQ_DBD += drvRFANT.dbd

LHCD_DAQ_DBD += timestampRecord.dbd
LHCD_DAQ_DBD += sysMon.dbd
LHCD_DAQ_DBD += subIfDriver.dbd
LHCD_DAQ_DBD += subHealthState.dbd
LHCD_DAQ_DBD += pvCrawler.dbd

LHCD_DAQ_DBD += devTCdatasave.dbd
LHCD_DAQ_DBD += drvTCdatasave.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
LHCD_DAQ_SRCS += LHCD_DAQ_registerRecordDeviceDriver.cpp
LHCD_DAQ_SRCS_DEFAULT += LHCD_DAQMain.cpp
LHCD_DAQ_SRCS_vxWorks += -nil-

LHCD_DAQ_SRCS += subIfDriver.cpp
LHCD_DAQ_SRCS += subHealthState.cpp
LHCD_DAQ_SRCS += userclass.cpp

# Add locally compiled object code
LHCD_DAQ_SRCS += drvNI6220.c
LHCD_DAQ_SRCS += drvNI6254.c
LHCD_DAQ_SRCS += devNI6220.c
LHCD_DAQ_SRCS += devNI6254.c
LHCD_DAQ_SRCS += lhcdMdsplus.c
LHCD_DAQ_SRCS += drvMdsplus.c
LHCD_DAQ_SRCS += nidaq_cmd.c
LHCD_DAQ_SRCS += devMdsplus.c
LHCD_DAQ_SRCS += drvRFCTRL.c
LHCD_DAQ_SRCS += drvRFANT.c

LHCD_DAQ_SRCS += devTCdatasave.c
LHCD_DAQ_SRCS += drvTCdatasave.c
LHCD_DAQ_SRCS += tcMdsplus.c

LHCD_DAQ_SRCS += tcp_comm.c

# The following adds support from base/src/vxWorks
LHCD_DAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

LHCD_DAQ_LIBS += recTimeStamp
LHCD_DAQ_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
endif

LHCD_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

