TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_INCLUDES        += -I/usr/local/vxipnp/linux/include
USR_INCLUDES        += -I/usr/local/nivxi/include
USR_INCLUDES        += -I/usr/local/mdsplus/include
USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

# ============ we use C flags
CFLAGS             = -DVXILINUX
USR_LDFLAGS         += -L/usr/local/nivxi/lib -lnivxi
USR_LDFLAGS         += -L/usr/local/vxipnp/linux/lib -lvisa
USR_LDFLAGS         += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt
#USR_LDFLAGS         += -L/usr/local/lib -lnidaqmx
#nidaqmx_DIRS          = /usr/local/lib

#asyn_DIR            += $(EPICS_PATH)/siteLib/lib/$(T_A)
#nicFP20x0_DIR       += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR			+= $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)



#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport
LIBRARY_IOC += drvVXI1564a

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

DBD    	     += drvVXI1564a.dbd
DBD          += devVXI1564a.dbd
drvVXI1564a_SRCS += drvVXI1564a.c
drvVXI1564a_SRCS += devVXI1564a.c
drvVXI1564a_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = MC
# MC.dbd will be created and installed
DBD += MC.dbd

# MC.dbd will be made up from these files:
MC_DBD += base.dbd
MC_DBD += sysMon.dbd
MC_DBD += xxxSupport.dbd
MC_DBD += dbSubExample.dbd
MC_DBD += MCHello.dbd
MC_DBD += drvVXI1564a.dbd
MC_DBD += devVXI1564a.dbd
MC_DBD += timestampRecord.dbd
MC_DBD += subHealthState.dbd
MC_DBD += subIfDriver.dbd 
MC_DBD += cltuBase.dbd 

# Next line was added for pvcrawler system command [shbaek, 2010.01.07]
MC_DBD += pvCrawler.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
MC_SRCS += MC_registerRecordDeviceDriver.cpp
MC_SRCS += subHealthState.cpp
MC_SRCS += subIfDriver.cpp 
MC_SRCS += userclass.cpp 
MC_SRCS_DEFAULT += MCMain.cpp
MC_SRCS_vxWorks += -nil-

# Add locally compiled object code
MC_SRCS += dbSubExample.c
MC_SRCS += MCHello.c

# The following adds support from base/src/vxWorks
MC_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

MC_LIBS += xxxSupport
MC_LIBS += recTimeStamp
MC_LIBS += sysMon
MC_LIBS += cltuDev
MC_LIBS += drvVXI1564a

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of MC
    MC_SNCFLAGS += +r
    MC_DBD += sncExample.dbd
    MC_SRCS += sncExample.stt
    MC_SRCS += sncMCDaqSeq.stt 
    MC_SRCS += sncMCDaqStatus.stt 
    MC_SRCS += sncMCMasterSet.stt 
    MC_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
#    PROD_HOST += sncMdsPut
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
#    sncProgram_SRCS += sncMdsPut.stt
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

MC_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

