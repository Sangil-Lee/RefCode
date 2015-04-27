TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    	+= -I/usr/local/mdsplus/include
USR_INCLUDES    	+= -I../include
USR_LDFLAGS     	+= -L/usr/local/lib
USR_SYS_LIBS 		+= nidaqmx
USR_LDFLAGS     	+= -L/usr/local/mdsplus/lib
USR_SYS_LIBS		+= TreeShr TdiShr MdsLib MdsShr c popt

USR_CFLAGS			+= -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
USR_CFLAGS			+= -D_NI_int32_DEFINED_

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_SITE)/include
USR_DBDFLAGS        += -I$(EPICS_SITE)/dbd

sfwLib_DIR 	    	+= $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR          += $(EPICS_SITE)/lib/$(T_A)

#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = LHCD_DIAG
DBD += LHCD_DIAG.dbd

LHCD_DIAG_DBD += base.dbd
LHCD_DIAG_DBD += drvNI6254.dbd
LHCD_DIAG_DBD += devNI6254.dbd
LHCD_DIAG_DBD += statCommon.dbd

LHCD_DIAG_DBD += kutil.dbd
LHCD_DIAG_DBD += kupaLib.dbd
LHCD_DIAG_DBD += sfwTree.dbd
LHCD_DIAG_DBD += sfwDBD.dbd
LHCD_DIAG_DBD += timestampRecord.dbd
LHCD_DIAG_DBD += sysMon.dbd
LHCD_DIAG_DBD += subIfDriver.dbd
LHCD_DIAG_DBD += subHealthState.dbd
LHCD_DIAG_DBD += pvCrawler.dbd

# LHCD_DIAG_registerRecordDeviceDriver.cpp derives from LHCD_DIAG.dbd
LHCD_DIAG_SRCS += LHCD_DIAG_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
LHCD_DIAG_SRCS_DEFAULT += LHCD_DIAGMain.cpp

# Add locally compiled object code
LHCD_DIAG_SRCS += drvNI6254.c
LHCD_DIAG_SRCS += devNI6254.c
LHCD_DIAG_SRCS += statCommon.c
LHCD_DIAG_SRCS += myMDSplus.c

LHCD_DIAG_SRCS += subIfDriver.cpp
LHCD_DIAG_SRCS += subHealthState.cpp
LHCD_DIAG_SRCS += userclass.cpp

# Link in the code from the support library
LHCD_DIAG_LIBS += recTimeStamp
LHCD_DIAG_LIBS += sysMon
LHCD_DIAG_LIBS += sfwLib
LHCD_DIAG_LIBS += kutil
LHCD_DIAG_LIBS += ktree
LHCD_DIAG_LIBS += kupa

ifneq ($(SNCSEQ),)
	sncLHCD_DIAG_SNCFLAGS += +r
	LHCD_DIAG_DBD  += sncLHCD_DIAG.dbd
	LHCD_DIAG_SRCS += sncLHCD_DIAG.stt
	LHCD_DIAG_LIBS += seq pv
endif

# Finally link to the EPICS Base libraries
LHCD_DIAG_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

