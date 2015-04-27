TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES    	+= -I/usr/local/mdsplus/include
USR_INCLUDES    	+= -I../include
USR_LDFLAGS     	+= -L/usr/local/lib
USR_SYS_LIBS 		+= nidaqmx
USR_SYS_LIBS 		+= gpibapi
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

PROD_IOC = RBA
# RBA.dbd will be created and installed
DBD += RBA.dbd

RBA_DBD += base.dbd
RBA_DBD += drvNI6122.dbd
RBA_DBD += devNI6122.dbd
RBA_DBD += drvNI4882.dbd
RBA_DBD += devNI4882.dbd
RBA_DBD += statCommon.dbd

RBA_DBD += sfwDBD.dbd
RBA_DBD += timestampRecord.dbd
RBA_DBD += sysMon.dbd
RBA_DBD += subIfDriver.dbd
RBA_DBD += subHealthState.dbd
RBA_DBD += pvCrawler.dbd
RBA_DBD += cltuBase.dbd

# RBA_registerRecordDeviceDriver.cpp derives from RBA.dbd
RBA_SRCS += RBA_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
RBA_SRCS_DEFAULT += RBAMain.cpp
RBA_SRCS_vxWorks += -nil-

# Add locally compiled object code
RBA_SRCS += RBA.c
RBA_SRCS += drvNI6122.c
RBA_SRCS += devNI6122.c
RBA_SRCS += drvNI4882.c
RBA_SRCS += devNI4882.c
RBA_SRCS += statCommon.c
RBA_SRCS += myMDSplus.c

RBA_SRCS += subIfDriver.cpp
RBA_SRCS += subHealthState.cpp
RBA_SRCS += userclass.cpp

# Add support from base/src/vxWorks if needed
#RBA_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
RBA_LIBS += recTimeStamp
RBA_LIBS += sysMon
RBA_LIBS += sfwLib
RBA_LIBS += cltuDev

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into RBASupport
    RBA_SNCFLAGS += +r
    RBA_DBD += sncExample.dbd
    RBA_SRCS += sncExample.stt
    RBA_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
RBA_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

