TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES		+= -I/usr/local/mdsplus/include
USR_INCLUDES		+= -I../include
#USR_LDFLAGS		+= -L/usr/local/lib -lnidaqmx
USR_LDFLAGS			+= -L/usr/local/lib
USR_SYS_LIBS		+= nidaqmx
#USR_LDFLAGS		+= -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt
USR_LDFLAGS			+= -L/usr/local/mdsplus/lib
USR_SYS_LIBS		+= TreeShr TdiShr MdsLib MdsShr c popt
USR_CFLAGS			+= -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58: error: conflicting types for ‘int32’
USR_CFLAGS			+= -D_NI_int32_DEFINED_

USR_INCLUDES		+= -I/usr/local/include
USR_INCLUDES		+= -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS		+= -I$(EPICS_PATH)/siteLib/dbd

cltuDev_DIR			+= $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR			+= $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR	+= $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR			+= $(EPICS_PATH)/siteLib/lib/$(T_A)
kutilLib_DIR		+= $(EPICS_SITE)/lib/$(T_A)
kupaLib_DIR			+= $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library


#=============================
# Build the IOC application

PROD_IOC = NB1_LODAQ
# NB1_LODAQ.dbd will be created and installed
DBD += NB1_LODAQ.dbd

# NB1_LODAQ.dbd will be made up from these files:
# EPICS base : /usr/local/epics/base/dbd
NB1_LODAQ_DBD += base.dbd

# KSTAR Common : /usr/local/epics/siteLib/dbd
NB1_LODAQ_DBD += sfwDBD.dbd
NB1_LODAQ_DBD += timestampRecord.dbd
NB1_LODAQ_DBD += sysMon.dbd			# CSM
NB1_LODAQ_DBD += cltuBase.dbd		# CLTU (Control/Local Timining Unit)

# KSTAR Common : /usr/local/epics/siteApp/...
NB1_LODAQ_DBD += subIfDriver.dbd
NB1_LODAQ_DBD += subHealthState.dbd
NB1_LODAQ_DBD += pvCrawler.dbd
NB1_LODAQ_DBD += kutil.dbd
NB1_LODAQ_DBD += kupaLib.dbd

# IOC application specific
NB1_LODAQ_DBD += initTrace.dbd
NB1_LODAQ_DBD += devNI6254.dbd
NB1_LODAQ_DBD += drvNI6254.dbd

NB1_LODAQ_DBD += nbiCommon.dbd

# NB1_LODAQ_registerRecordDeviceDriver.cpp derives from NB1_LODAQ.dbd
NB1_LODAQ_SRCS += NB1_LODAQ_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
NB1_LODAQ_SRCS_DEFAULT += NB1_LODAQMain.cpp
NB1_LODAQ_SRCS_vxWorks += -nil-

# Add locally compiled object code
NB1_LODAQ_SRCS += initTrace.c
NB1_LODAQ_SRCS += drvNI6254.c
NB1_LODAQ_SRCS += devNI6254.c
NB1_LODAQ_SRCS += myMDSplus.c
NB1_LODAQ_SRCS += calAlarm.cpp

NB1_LODAQ_SRCS += subIfDriver.cpp
NB1_LODAQ_SRCS += subHealthState.cpp
NB1_LODAQ_SRCS += userclass.cpp

NB1_LODAQ_SRCS += nbiCommon.cpp

# Add support from base/src/vxWorks if needed
#NB1_LODAQ_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
NB1_LODAQ_LIBS += recTimeStamp
NB1_LODAQ_LIBS += sysMon
NB1_LODAQ_LIBS += cltuDev
NB1_LODAQ_LIBS += sfwLib
NB1_LODAQ_LIBS += kutil
NB1_LODAQ_LIBS += kupa

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
	# This builds sncProgram as a component of NB1_LODAQ
	sncNB1_LODAQ_SNCFLAGS += +r
	NB1_LODAQ_DBD += sncNB1_LODAQ.dbd
	NB1_LODAQ_SRCS += sncNB1_LODAQ.stt
	NB1_LODAQ_DBD += sncCalAlarm.dbd
	NB1_LODAQ_SRCS += sncCalAlarm.stt
	NB1_LODAQ_LIBS += seq pv

	# Build sncProgram as a standalone program
	#PROD_HOST += sncProgram
	#sncProgram_SNCFLAGS += +m
	#sncProgram_SRCS += sncProgram.st
	#sncProgram_LIBS += seq pv kutil
	#sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
NB1_LODAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

