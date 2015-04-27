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

PROD_IOC = DDS2
# DDS2.dbd will be created and installed
DBD += DDS2.dbd

# DDS2.dbd will be made up from these files:
DDS2_DBD += base.dbd
#DDS2_DBD += xxxSupport.dbd
#DDS2_DBD += dbSubExample.dbd
#DDS2_DBD += DDS2Hello.dbd

DDS2_DBD += devM6802.dbd
DDS2_DBD += drvM6802.dbd
DDS2_DBD += timestampRecord.dbd
DDS2_DBD += asyn.dbd

#DDS2_DBD += devCLTU.dbd
#DDS2_DBD += drvCLTU.dbd
DDS2_DBD += cltuBase.dbd
# Add System Monitor by TG Lee 20080405
DDS2_DBD += sysMon.dbd
DDS2_DBD += subHealthState.dbd
DDS2_DBD += subIfDriver.dbd

# Next line was added for pvcrawler system command [shbaek, 2010.03.04]
DDS2_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
DDS2_SRCS += DDS2_registerRecordDeviceDriver.cpp
DDS2_SRCS_DEFAULT += DDS2Main.cpp
DDS2_SRCS_vxWorks += -nil-

# Add locally compiled object code
#DDS2_SRCS += dbSubExample.c
#DDS2_SRCS += DDS2Hello.c

DDS2_SRCS += devM6802.c
DDS2_SRCS += devMDSplus.c
DDS2_SRCS += drvM6802.c
DDS2_SRCS += drvFPDP.c
DDS2_SRCS += drvOzfunc.c

DDS2_SRCS += drvCLTU.c
DDS2_SRCS += devCLTU.c
DDS2_SRCS += subHealthState.cpp
DDS2_SRCS += subIfDriver.cpp
DDS2_SRCS += userclass.cpp

# The following adds support from base/src/vxWorks
DDS2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#DDS2_LIBS += xxxSupport
DDS2_LIBS += drvM6802
DDS2_LIBS += recTimeStamp
DDS2_LIBS += asyn
#Add System Monitor
DDS2_LIBS += sysMon


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of DDS2
    DDS2_SNCFLAGS += +r
    DDS2_DBD += sncExample.dbd
    DDS2_SRCS += sncDDS2BoProc.stt
    DDS2_SRCS += sncDDS2DaqSeq.stt
#    DDS2_SRCS += sncExample.stt
    DDS2_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

DDS2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

