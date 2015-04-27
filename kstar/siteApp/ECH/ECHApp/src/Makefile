TOP=../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS += -I$(EPICS_PATH)/siteLib/dbd
recTimeStamp_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
asyn_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
TPG256_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
cltuDev_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
devESP300_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
devDG535_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
etherIp_DIR += $(EPICS_PATH)/siteLib/lib/linux-x86
sfwLib_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
kutil_DIR        += $(EPICS_SITE)/lib/$(T_A)
kupa_DIR         += $(EPICS_SITE)/lib/$(T_A)

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#  You must twice compile.
#  One - "USR_LDFLAGS & nidaqmx_DIRS" Add after "make clean; make" Compile.
#  Two -"USR_LDFLAGS & nidaqmx_DIRS" remove after "make" Compile.
#=============================
USR_LDFLAGS += -L/usr/local/lib -lnidaqmx
USR_LDFLAGS += -lnidaqmx
USR_INCLUDES += -I/usr/local/include
USR_INCLUDES += -I/usr/local/mdsplus/include
USR_INCLUDES += -I/usr/local/epics/siteLib/include
USR_LDFLAGS += -L/usr/local/mdsplus/lib -lTreeShr -lMdsLib -lMdsShr -lc -lpopt
#nidaqmx_DIRS = /usr/local/lib

USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
USR_CFLAGS  += -D_NI_int32_DEFINED_


#==================================================
# Build an IOC support library
LIBRARY_IOC += xxxSupport
LIBRARY_IOC += recTimeStamp

# xxxRecord.h will be created from xxxRecord.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c
xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)


#=============================
# build an ioc application

PROD_IOC = ECH
# ECH.dbd will be created and installed
DBD += ECH.dbd

# ECH.dbd will be made up from these files:
ECH_DBD += base.dbd
ECH_DBD += xxxSupport.dbd
ECH_DBD += dbSubExample.dbd
ECH_DBD += ECHHello.dbd
# DAQ System SFW
ECH_DBD += drvNIDAQmx.dbd
ECH_DBD += devNIDAQmx.dbd
ECH_DBD += statCommon.dbd
ECH_DBD += sfwDBD.dbd

ECH_DBD += cltuBase.dbd

ECH_DBD += timestampRecord.dbd
ECH_DBD += asyn.dbd
ECH_DBD += drvAsynIPPort.dbd
ECH_DBD += devGpib.dbd
ECH_DBD += devTPG256.dbd
ECH_DBD += devESP300.dbd
ECH_DBD += devDG535.dbd
ECH_DBD += drvVxi11.dbd
ECH_DBD += sysMon.dbd

ECH_DBD += ether_ip.dbd
ECH_DBD += kutil.dbd
ECH_DBD += kupaLib.dbd 

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
ECH_DBD += pvCrawler.dbd
ECH_DBD += subIfDriver.dbd
ECH_DBD += subHealthState.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ECH_SRCS += ECH_registerRecordDeviceDriver.cpp
ECH_SRCS += subIfDriver.cpp
ECH_SRCS += subHealthState.cpp
ECH_SRCS += userclass.cpp

ECH_SRCS_DEFAULT += ECHMain.cpp
ECH_SRCS_vxWorks += -nil-

# Add locally compiled object code
ECH_SRCS += dbSubExample.c
ECH_SRCS += ECHHello.c

# DAQ PCIe-6259 AO, AI, DIO 
ECH_SRCS += drvNIDAQmx.c 
ECH_SRCS += statCommon.c
ECH_SRCS += devNIDAQmx.c
ECH_SRCS += myMDSplus.c

# The following adds support from base/src/vxWorks
ECH_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

ECH_LIBS += xxxSupport

ECH_LIBS += sfwLib 
ECH_LIBS += cltuDev

ECH_LIBS += recIoc
ECH_LIBS += softDevIoc
ECH_LIBS += testDevIoc

ECH_LIBS += recTimeStamp
ECH_LIBS += asyn
ECH_LIBS += devESP300 
ECH_LIBS += devTPG256
ECH_LIBS += devDG535
ECH_LIBS += sysMon
ECH_LIBS += kutil 
ECH_LIBS += kupa
ECH_LIBS += $(EPICS_BASE_IOC_LIBS)

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of ECH
    ECH_SNCFLAGS += +r
    ECH_DBD += sncExample.dbd
#    ECH_SRCS += sncExample.stt
    ECH_SRCS += sncECHAOSeq.stt 
    ECH_SRCS += sncPARAMZero.stt 
#    ECH_SRCS += sncECHDaqSeq.stt 
    ECH_SRCS += sncEchFaultList.stt
    ECH_SRCS += sncPermReady.stt 
#    ECH_SRCS += sncMdsPut.stt
    ECH_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

ECH_LIBS += $(EPICS_BASE_IOC_LIBS)
ECH_LIBS += ether_ip

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

