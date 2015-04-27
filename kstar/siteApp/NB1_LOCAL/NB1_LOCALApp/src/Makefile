TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_INCLUDES    += -I/usr/local/mdsplus/include
USR_LDFLAGS     += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR += $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_SITE)/lib/$(T_A)
#heProp_DIR += $(EPICS_SITE)/lib/$(T_A)
asyn_DIR += $(EPICS_SITE)/lib/$(T_A)
devSRSRGA_DIR += $(EPICS_SITE)/lib/$(T_A)


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

PROD_IOC = nbi
# nbi.dbd will be created and installed
DBD += nbi.dbd

# nbi.dbd will be made up from these files:
nbi_DBD += base.dbd
nbi_DBD += xxxSupport.dbd
nbi_DBD += dbSubExample.dbd
nbi_DBD += nbiHello.dbd
nbi_DBD += ether_ip.dbd
nbi_DBD += drvEtherIP.dbd
nbi_DBD += drvModbus.dbd
nbi_DBD += devBending.dbd
nbi_DBD += timestampRecord.dbd
nbi_DBD += cltuBase.dbd
nbi_DBD += drvEtosTcp.dbd
nbi_DBD += devEtos.dbd
nbi_DBD += drvICP.dbd
nbi_DBD += devICP.dbd
nbi_DBD += subIfDriver.dbd
nbi_DBD += subHealthState.dbd
nbi_DBD += sysMon.dbd
nbi_DBD += pvCrawler.dbd
nbi_DBD += heProp.dbd
nbi_DBD += drvTcMinMax.dbd
nbi_DBD += devTcMinMax.dbd
nbi_DBD += drvTcMinMax2.dbd
nbi_DBD += devTcMinMax2.dbd
nbi_DBD += asyn.dbd
nbi_DBD += drvGenericSerial.dbd
nbi_DBD += devGpib.dbd
nbi_DBD += devSRSRGA.dbd
nbi_DBD += sncSRSRGA.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
nbi_SRCS += nbi_registerRecordDeviceDriver.cpp
nbi_SRCS_DEFAULT += nbiMain.cpp
nbi_SRCS_vxWorks += -nil-

# Add locally compiled object code
nbi_SRCS += dbSubExample.c
nbi_SRCS += nbiHello.c
nbi_SRCS += drvModbus.c
nbi_SRCS += modbus_tcp.c
nbi_SRCS += devBending.c
nbi_SRCS += drvEtosTcp.c
nbi_SRCS += devEtos.c
nbi_SRCS += drvICP.c
nbi_SRCS += devICP.c
nbi_SRCS += subIfDriver.cpp
nbi_SRCS += subHealthState.cpp
nbi_SRCS += userclass.cpp
nbi_SRCS += drvTcMinMax.c
nbi_SRCS += devTcMinMax.c
nbi_SRCS += drvTcMinMax2.c
nbi_SRCS += devTcMinMax2.c
nbi_SRCS += sncSRSRGA.stt

# The following adds support from base/src/vxWorks
nbi_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

nbi_LIBS += xxxSupport
nbi_LIBS += cltuDev
nbi_LIBS += sysMon
nbi_LIBS += recTimeStamp
nbi_LIBS += heProp
nbi_LIBS += asyn
nbi_LIBS += devSRSRGA


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of nbi
    nbi_SNCFLAGS += +r
    nbi_DBD += sncExample.dbd
    nbi_SRCS += sncExample.stt
    nbi_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

nbi_LIBS += ether_ip
nbi_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
