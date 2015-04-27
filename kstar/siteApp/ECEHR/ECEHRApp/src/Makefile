TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
PRJ_HOME  = /Vmetro/dpio2-drv-src-m5000-linux
CROSS_DIR = /opt/eldk/ppc_74xx/usr/src/cwcec-kernel-source-2.6.16.13

USR_INCLUDES    += -I/usr/local/mdsplus-ppc/include -I$(CROSS_DIR)/include -I/Vmetro/dpio2-drv-src-m5000-linux/include
USR_LDFLAGS     += -L/usr/local/mdsplus-ppc/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc
USR_OBJS += $(PRJ_HOME)/lib/ppc-74xx-linux-2.6.x/dpio2-drv.o

USR_CFLAGS += -D VMOSA -D VM_LINUX -D VMOSA_M5000 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64


USR_INCLUDES        += -I/opt/eldk/ppc_74xx/usr/local/epics/siteLib/include
USR_DBDFLAGS        += -I/opt/eldk/ppc_74xx/usr/local/epics/siteLib/dbd

asyn_DIR            += /opt/eldk/ppc_74xx/usr/local/epics/siteLib/lib/$(T_A)
recTimeStamp_DIR    += /opt/eldk/ppc_74xx/usr/local/epics/siteLib/lib/$(T_A)
# Add System Monitor by TG Lee  20080405
sysMon_DIR          += /opt/eldk/ppc_74xx/usr/local/epics/siteLib/lib/$(T_A)

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)

#==================================================
# Build an IOC support library

#LIBRARY_IOC += xxxSupport
LIBRARY_IOC += drvM6802

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
#DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
#xxxSupport_SRCS += xxxRecord.c
#xxxSupport_SRCS += devXxxSoft.c

#xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)
drvM6802_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = ECEHR
# ECEHR.dbd will be created and installed
DBD += ECEHR.dbd

# ECEHR.dbd will be made up from these files:
ECEHR_DBD += base.dbd
#ECEHR_DBD += xxxSupport.dbd
#ECEHR_DBD += dbSubExample.dbd
#ECEHR_DBD += ECEHRHello.dbd

ECEHR_DBD += devM6802.dbd
ECEHR_DBD += drvM6802.dbd
ECEHR_DBD += timestampRecord.dbd
ECEHR_DBD += asyn.dbd

ECEHR_DBD += cltuBase.dbd
# Add System Monitor by TG Lee 20080405
ECEHR_DBD += sysMon.dbd
ECEHR_DBD += subHealthState.dbd
ECEHR_DBD += subIfDriver.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.03.04]
ECEHR_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ECEHR_SRCS += ECEHR_registerRecordDeviceDriver.cpp
ECEHR_SRCS_DEFAULT += ECEHRMain.cpp
ECEHR_SRCS_vxWorks += -nil-

# Add locally compiled object code
#ECEHR_SRCS += dbSubExample.c
#ECEHR_SRCS += ECEHRHello.c

ECEHR_SRCS += devM6802.c
ECEHR_SRCS += devMDSplus.c
ECEHR_SRCS += drvM6802.c
ECEHR_SRCS += drvFPDP.c
ECEHR_SRCS += drvOzfunc.c

ECEHR_SRCS += subHealthState.cpp
ECEHR_SRCS += subIfDriver.cpp
ECEHR_SRCS += userclass.cpp

# The following adds support from base/src/vxWorks
ECEHR_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#ECEHR_LIBS += xxxSupport
ECEHR_LIBS += drvM6802
ECEHR_LIBS += recTimeStamp
ECEHR_LIBS += asyn
#Add System Monitor
ECEHR_LIBS += sysMon

ECEHR_LIBS += cltuDev


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of ECEHR
    ECEHR_SNCFLAGS += +r
    ECEHR_DBD += sncExample.dbd
    ECEHR_SRCS += sncECEHRBoProc.stt
    ECEHR_SRCS += sncECEHRDaqSeq.stt
    ECEHR_SRCS += sncECEHRChSelect.stt 
#    ECEHR_SRCS += sncExample.stt
    ECEHR_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

ECEHR_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

