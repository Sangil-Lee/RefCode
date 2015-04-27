TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES        += -I../include
USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I/usr/local/mdsplus/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

USR_CFLAGS          += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
USR_LDFLAGS         += -L/usr/local/lib

recTimeStamp_DIR    += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR          += $(EPICS_SITE)/lib/$(T_A)
cltuDev_DIR         += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR          += $(EPICS_SITE)/lib/$(T_A)
asyn_DIR            += $(EPICS_SITE)/lib/$(T_A)
pfeifferTPG262_DIR	+= $(EPICS_SITE)/lib/$(T_A)
MK80S_DIR           += $(EPICS_SITE)/lib/$(T_A)
kutil_DIR           += $(EPICS_SITE)/lib/$(T_A)

#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = FILD

# FILD.dbd will be created and installed
DBD += FILD.dbd

# FILD.dbd will be made up from these files:
FILD_DBD += base.dbd
#FILD_DBD += initTrace.dbd

FILD_DBD += sfwDBD.dbd
FILD_DBD += timestampRecord.dbd
FILD_DBD += sysMon.dbd
FILD_DBD += subIfDriver.dbd
FILD_DBD += subHealthState.dbd
FILD_DBD += pvCrawler.dbd
FILD_DBD += cltuBase.dbd
FILD_DBD += asyn.dbd
FILD_DBD += drvGenericSerial.dbd

FILD_DBD += drvTPG262.dbd
FILD_DBD += devTPG262.dbd
FILD_DBD += drvMK80S.dbd
FILD_DBD += devMK80S.dbd
FILD_DBD += kutil.dbd

# FILD_registerRecordDeviceDriver.cpp derives from FILD.dbd
FILD_SRCS += FILD_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
FILD_SRCS_DEFAULT += FILDMain.cpp
FILD_SRCS_vxWorks += -nil-

#FILD_SRCS += initTrace.c
FILD_SRCS += subIfDriver.cpp
FILD_SRCS += subHealthState.cpp
FILD_SRCS += userclass.cpp

# Link in the code from the support library
FILD_LIBS += recTimeStamp
FILD_LIBS += sysMon
FILD_LIBS += sfwLib
FILD_LIBS += cltuDev
FILD_LIBS += asyn
FILD_LIBS += pfeifferTPG262
FILD_LIBS += MK80S
FILD_LIBS += kutil

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
	# This builds sncProgram as a component of FILD
    sncFILD_SNCFLAGS += +r
    FILD_DBD += sncFILD.dbd
    FILD_SRCS += sncFILD.stt
    FILD_LIBS += seq pv
	# Test PLC
	FILD_DBD += sncFILDTestPLC.dbd
    FILD_SRCS += sncFILDTestPLC.stt

    # Build sncProgram as a standalone program
	#PROD_HOST += sncProgram
	#sncProgram_SNCFLAGS += +m
	#sncProgram_SRCS += sncProgram.st
	#sncProgram_LIBS += seq pv kutil
	#sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
FILD_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

