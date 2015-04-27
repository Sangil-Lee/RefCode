TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES	+= -I/usr/local/mdsplus/include
USR_INCLUDES	+= -I$(EPICS_PATH)/siteApp/dtacq/HOSTDRV
USR_LDFLAGS     += -L/usr/local/lib -lnidaqmx
USR_LDFLAGS	+= -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#USR_CFLAGS += -D_NI_int32_DEFINED_

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR       += $(EPICS_SITE)/lib/$(T_A)


#==================================================
# Build an IOC support library


#=============================
# build an ioc application

PROD_IOC = SXR
# SXR.dbd will be created and installed
DBD += SXR.dbd

# SXR.dbd will be made up from these files:
SXR_DBD += base.dbd
SXR_DBD += drvNI6123.dbd
SXR_DBD += initTrace.dbd
SXR_DBD += devNI6123.dbd
SXR_DBD += devIT6322.dbd
SXR_DBD += drvNI6259.dbd
SXR_DBD += devNI6259.dbd
SXR_DBD += drvIT6322.dbd

SXR_DBD += sfwAdmin.dbd
SXR_DBD += timestampRecord.dbd
SXR_DBD += sysMon.dbd
SXR_DBD += cltuBase.dbd
SXR_DBD += subIfDriver.dbd
SXR_DBD += subHealthState.dbd


# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
SXR_DBD += pvCrawler.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
SXR_SRCS += SXR_registerRecordDeviceDriver.cpp
SXR_SRCS_DEFAULT += SXRMain.cpp
SXR_SRCS_vxWorks += -nil-

# Add locally compiled object code
SXR_SRCS += initTrace.c
SXR_SRCS += drvNI6123.c
SXR_SRCS += drvNI6259.c
SXR_SRCS += devNI6259.c
SXR_SRCS += drvIT6322.c
SXR_SRCS += devIT6322.c
SXR_SRCS += devNI6123.c
SXR_SRCS += myMDSplus.c

SXR_SRCS += sfwDBSeq.c
SXR_SRCS += sfwCommon.c
SXR_SRCS += sfwAdminHead.c
SXR_SRCS += sfwAdminBody.c
SXR_SRCS += sfwMDSplus.c
SXR_SRCS += subIfDriver.cpp
SXR_SRCS += subHealthState.cpp
SXR_SRCS += userclass.cpp


# The following adds support from base/src/vxWorks
SXR_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

SXR_LIBS += cltuDev
SXR_LIBS += recTimeStamp
SXR_LIBS += sysMon
SXR_LIBS += $(EPICS_BASE_IOC_LIBS)

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of SXR
    SXR_SNCFLAGS += +r
    SXR_DBD += sncLogicSeq.dbd
    SXR_SRCS += sncLogicSeq.stt
    SXR_LIBS += seq pv

endif

SXR_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

