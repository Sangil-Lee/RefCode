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

PROD_IOC = TS2
# TS2.dbd will be created and installed
DBD += TS2.dbd

# TS2.dbd will be made up from these files:
TS2_DBD += base.dbd
TS2_DBD += initTrace.dbd
TS2_DBD += devV792.dbd
TS2_DBD += drvV792.dbd

TS2_DBD += sfwDBD.dbd
TS2_DBD += timestampRecord.dbd
TS2_DBD += sysMon.dbd
TS2_DBD += cltuBase.dbd
TS2_DBD += subIfDriver.dbd
TS2_DBD += subHealthState.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
TS2_DBD += pvCrawler.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
TS2_SRCS += TS2_registerRecordDeviceDriver.cpp
TS2_SRCS += subIfDriver.cpp
TS2_SRCS += subHealthState.cpp
TS2_SRCS += userclass.cpp
TS2_SRCS_DEFAULT += TS2Main.cpp
TS2_SRCS_vxWorks += -nil-

# Add locally compiled object code
TS2_SRCS += initTrace.c
TS2_SRCS += drvV792.c
TS2_SRCS += devV792.c
TS2_SRCS += myMDSplus.c


# The following adds support from base/src/vxWorks
TS2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

TS2_LIBS += recTimeStamp
TS2_LIBS += sysMon
TS2_LIBS += cltuDev
TS2_LIBS += sfwLib
TS2_LIBS += $(EPICS_BASE_IOC_LIBS)

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of TS2
    TS2_SNCFLAGS += +r
    TS2_DBD += sncRuntimeSeq.dbd
    TS2_SRCS += sncRuntimeSeq.stt
    TS2_LIBS += seq pv

    # The following builds sncMdsCrEchTss as a standalone application
endif

TS2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

