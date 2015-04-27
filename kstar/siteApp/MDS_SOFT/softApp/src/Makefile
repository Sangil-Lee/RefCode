TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES        += -I/usr/local/mdsplus/include
USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd
USR_LDFLAGS         += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

sysMon_DIR   += $(EPICS_PATH)/siteLib/lib/$(T_A)
#recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)


#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = soft
# soft.dbd will be created and installed
DBD += soft.dbd

# soft.dbd will be made up from these files:
soft_DBD += base.dbd
soft_DBD += xxxSupport.dbd
soft_DBD += dbSubExample.dbd
soft_DBD += softHello.dbd
soft_DBD += sysMon.dbd
soft_DBD += subIfDriver.dbd
soft_DBD += subHealthState.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
soft_SRCS += soft_registerRecordDeviceDriver.cpp
soft_SRCS += subIfDriver.cpp
soft_SRCS += subHealthState.cpp
soft_SRCS += userclass.cpp

soft_SRCS_DEFAULT += softMain.cpp
soft_SRCS_vxWorks += -nil-

# Add locally compiled object code
soft_SRCS += dbSubExample.c
soft_SRCS += softHello.c

# The following adds support from base/src/vxWorks
soft_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

soft_LIBS += xxxSupport
soft_LIBS += sysMon

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of soft
    soft_SNCFLAGS += +r
    soft_DBD += sncExample.dbd
    soft_SRCS += sncExample.stt
    soft_SRCS += sncKstarTreeCreateDone.stt
    soft_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)

#    PROD_HOST += sncMdsCrEchTss
#    sncMdsCrEchTss_SNCFLAGS += +m
##    sncMdsCrEchTss_SRCS += sncProgram.st
#    sncMdsCrEchTss_SRCS += sncMdsCrEchTss.stt
#    sncMdsCrEchTss_LIBS += seq pv
#    sncMdsCrEchTss_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncKstarTreeCreate
    sncKstarTreeCreate_SNCFLAGS += +m
    sncKstarTreeCreate_SRCS += sncKstarTreeCreate.stt
    sncKstarTreeCreate_LIBS += seq pv
    sncKstarTreeCreate_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsECHPut
    sncMdsECHPut_SNCFLAGS += +m
    sncMdsECHPut_SRCS += sncMdsECHPut.stt
    sncMdsECHPut_LIBS += seq pv
    sncMdsECHPut_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncKstarPutTSS
    sncKstarPutTSS_SNCFLAGS += +m
    sncKstarPutTSS_SRCS += sncKstarPutTSS.stt 
    sncKstarPutTSS_LIBS += seq pv
    sncKstarPutTSS_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsEventPutMC 
    sncMdsEventPutMC_SNCFLAGS += +m
    sncMdsEventPutMC_SRCS += sncMdsEventPutMC.stt
    sncMdsEventPutMC_LIBS += seq pv
    sncMdsEventPutMC_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsEventPutDDS1A 
    sncMdsEventPutDDS1A_SNCFLAGS += +m
    sncMdsEventPutDDS1A_SRCS += sncMdsEventPutDDS1A.stt
    sncMdsEventPutDDS1A_LIBS += seq pv
    sncMdsEventPutDDS1A_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsEventPutDDS1B 
    sncMdsEventPutDDS1B_SNCFLAGS += +m
    sncMdsEventPutDDS1B_SRCS += sncMdsEventPutDDS1B.stt
    sncMdsEventPutDDS1B_LIBS += seq pv
    sncMdsEventPutDDS1B_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsEventPutMMWI 
    sncMdsEventPutMMWI_SNCFLAGS += +m
    sncMdsEventPutMMWI_SRCS += sncMdsEventPutMMWI.stt
    sncMdsEventPutMMWI_LIBS += seq pv
    sncMdsEventPutMMWI_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsEventPutECEHR 
    sncMdsEventPutECEHR_SNCFLAGS += +m
    sncMdsEventPutECEHR_SRCS += sncMdsEventPutECEHR.stt
    sncMdsEventPutECEHR_LIBS += seq pv
    sncMdsEventPutECEHR_LIBS += $(EPICS_BASE_HOST_LIBS)
    PROD_HOST += sncMdsEventPutHALPHA 
    sncMdsEventPutHALPHA_SNCFLAGS += +m
    sncMdsEventPutHALPHA_SRCS += sncMdsEventPutHALPHA.stt
    sncMdsEventPutHALPHA_LIBS += seq pv
    sncMdsEventPutHALPHA_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

soft_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

