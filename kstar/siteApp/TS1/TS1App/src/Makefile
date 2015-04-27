TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES   += -I/usr/local/mdsplus/include
USR_INCLUDES   += -I/usr/local/epics/siteApp/CAEN/include
USR_LDFLAGS    += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt
USR_LDFLAGS    += -lCAENVME -lCAENqtpSDK

USR_CFLAGS  += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG -DLINUX

USR_INCLUDES    += -I/usr/local/include
USR_INCLUDES    += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS    += -I$(EPICS_PATH)/siteLib/dbd

cltuDev_DIR       += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR       += $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR  += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR        += $(EPICS_PATH)/siteLib/lib/$(T_A)


#==================================================
# Build an IOC support library


#=============================
# build an ioc application

PROD_IOC = TS1
# TS1.dbd will be created and installed
DBD += TS1.dbd

# TS1.dbd will be made up from these files:
TS1_DBD += base.dbd
TS1_DBD += initTrace.dbd
TS1_DBD += devV792.dbd
TS1_DBD += drvV792.dbd

TS1_DBD += sfwDBD.dbd
TS1_DBD += timestampRecord.dbd
TS1_DBD += sysMon.dbd
TS1_DBD += cltuBase.dbd
TS1_DBD += subIfDriver.dbd
TS1_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
TS1_DBD += pvCrawler.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
TS1_SRCS += TS1_registerRecordDeviceDriver.cpp
TS1_SRCS += subIfDriver.cpp
TS1_SRCS += subHealthState.cpp
TS1_SRCS += userclass.cpp
TS1_SRCS_DEFAULT += TS1Main.cpp
TS1_SRCS_vxWorks += -nil-

# Add locally compiled object code
TS1_SRCS += initTrace.c
TS1_SRCS += drvV792.c
TS1_SRCS += devV792.c
TS1_SRCS += myMDSplus.c


# The following adds support from base/src/vxWorks
TS1_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

TS1_LIBS += recTimeStamp
TS1_LIBS += sysMon
TS1_LIBS += cltuDev
TS1_LIBS += sfwLib
TS1_LIBS += $(EPICS_BASE_IOC_LIBS)

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of TS1
    TS1_SNCFLAGS += +r
    TS1_DBD += sncRuntimeSeq.dbd
    TS1_DBD += sncSeqTSSC.dbd
    TS1_SRCS += sncRuntimeSeq.stt
    TS1_SRCS += sncSeqTSSC.stt
    TS1_LIBS += seq pv

    # The following builds sncMdsCrEchTss as a standalone application
endif

TS1_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

