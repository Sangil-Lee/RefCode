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
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

cltuDev_DIR 	    += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR 	    	+= $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)

#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = VBS
# VBS.dbd will be created and installed
DBD += VBS.dbd

VBS_DBD += base.dbd
VBS_DBD += drvNI6250.dbd
VBS_DBD += devNI6250.dbd
VBS_DBD += statCommon.dbd
VBS_DBD += kutil.dbd
VBS_DBD += kupaLib.dbd
VBS_DBD += sfwTree.dbd

VBS_DBD += sfwDBD.dbd
VBS_DBD += timestampRecord.dbd
VBS_DBD += sysMon.dbd
VBS_DBD += subIfDriver.dbd
VBS_DBD += subHealthState.dbd
VBS_DBD += pvCrawler.dbd
VBS_DBD += cltuBase.dbd

# VBS_registerRecordDeviceDriver.cpp derives from VBS.dbd
VBS_SRCS += VBS_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
VBS_SRCS_DEFAULT += VBSMain.cpp
VBS_SRCS_vxWorks += -nil-

# Add locally compiled object code
VBS_SRCS += drvNI6250.c
VBS_SRCS += statCommon.c
VBS_SRCS += devNI6250.c
VBS_SRCS += myMDSplus.c

VBS_SRCS += subIfDriver.cpp
VBS_SRCS += subHealthState.cpp
VBS_SRCS += userclass.cpp

# Add support from base/src/vxWorks if needed
#VBS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
VBS_LIBS += recTimeStamp
VBS_LIBS += sysMon
VBS_LIBS += sfwLib
VBS_LIBS += cltuDev

# TODO : SFW_TREE
VBS_LIBS += kutil
VBS_LIBS += sfwTree
VBS_LIBS += kupa

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
	# This builds sncProgram as a component of IOC
    sncVBS_SNCFLAGS += +r
    VBS_DBD += sncVBS.dbd
    VBS_SRCS += sncVBS.stt
    VBS_DBD += sncKupa.dbd
    VBS_SRCS += sncKupa.stt
    VBS_LIBS += seq pv
endif

# Finally link to the EPICS Base libraries
VBS_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

