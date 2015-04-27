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

PROD_IOC = HALPHA
# HALPHA.dbd will be created and installed
DBD += HALPHA.dbd

# HALPHA.dbd will be made up from these files:
HALPHA_DBD += base.dbd
#HALPHA_DBD += xxxSupport.dbd
#HALPHA_DBD += dbSubExample.dbd
#HALPHA_DBD += HALPHAHello.dbd

HALPHA_DBD += devM6802.dbd
HALPHA_DBD += drvM6802.dbd
HALPHA_DBD += timestampRecord.dbd
HALPHA_DBD += asyn.dbd

HALPHA_DBD += cltuBase.dbd
HALPHA_DBD += sysMon.dbd
HALPHA_DBD += subHealthState.dbd
HALPHA_DBD += subIfDriver.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.03.04]
HALPHA_DBD += pvCrawler.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
HALPHA_SRCS += HALPHA_registerRecordDeviceDriver.cpp
HALPHA_SRCS_DEFAULT += HALPHAMain.cpp
HALPHA_SRCS_vxWorks += -nil-

# Add locally compiled object code
#HALPHA_SRCS += dbSubExample.c
#HALPHA_SRCS += HALPHAHello.c

HALPHA_SRCS += devM6802.c
HALPHA_SRCS += devMDSplus.c
HALPHA_SRCS += drvM6802.c
HALPHA_SRCS += drvFPDP.c
HALPHA_SRCS += drvOzfunc.c

HALPHA_SRCS += drvCLTU.c
HALPHA_SRCS += devCLTU.c
HALPHA_SRCS += subHealthState.cpp
HALPHA_SRCS += subIfDriver.cpp
HALPHA_SRCS += userclass.cpp

# The following adds support from base/src/vxWorks
HALPHA_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#HALPHA_LIBS += xxxSupport
HALPHA_LIBS += drvM6802
HALPHA_LIBS += recTimeStamp
HALPHA_LIBS += asyn
#Add System Monitor
HALPHA_LIBS += sysMon


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of HALPHA
    HALPHA_SNCFLAGS += +r
    HALPHA_DBD += sncExample.dbd
    HALPHA_SRCS += sncExample.stt
    HALPHA_SRCS += sncHALPHABoProc.stt
    HALPHA_SRCS += sncHALPHADaqSeq.stt
    HALPHA_SRCS += sncHALPHAChSelect.stt
    HALPHA_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

HALPHA_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

