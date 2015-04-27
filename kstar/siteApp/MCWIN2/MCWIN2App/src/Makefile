TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# No optimization, we want debugging info:
HOST_OPT=NO
CROSS_OPT=NO

USR_INCLUDES        += -IC:/epics/base/include
USR_INCLUDES        += -IC:/epics/base/include/os/WIN32
USR_INCLUDES        += -IC:/epics/siteLib/include
USR_DBDFLAGS        += -IC:/epics/siteLib/dbd
USR_INCLUDES        += -IC:/"Program Files"/MDSplus/DEVTOOLS/include
USR_INCLUDES        += -IC:/"Program Files"/"IVI Foundation"/VISA/WinNT/include

MDSLib_DIR += C:\"Program Files"\MDSplus\DEVTOOLS\lib
MDSLib += treeshr tdishr mdslib mdsshr 

VISALib_DIR += C:\"Program Files"\"IVI Foundation"\VISA\WinNT\lib\msc
VISALib += visa32 hpe1564

#=============================
# Build the IOC support library

LIBRARY_IOC += MCWIN2Support
LIBRARY_IOC += drvVXI1564a

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
MCWIN2Support_SRCS += xxxRecord.c
MCWIN2Support_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
MCWIN2Support_SRCS += dbSubExample.c
MCWIN2Support_SRCS += MCWIN2Hello.c
MCWIN2Support_SRCS += initTrace.c

MCWIN2Support_SRCS += dbSubNet.cpp
MCWIN2Support_SRCS += devSysMon.cpp
MCWIN2Support_SRCS += subHealthState.cpp
MCWIN2Support_SRCS += winPerfmon.cpp

MCWIN2Support_LIBS += $(EPICS_BASE_IOC_LIBS) $(MDSLib) $(VISALib)

DBD    	     += drvVXI1564a.dbd
DBD          += devVXI1564a.dbd
drvVXI1564a_SRCS += drvVXI1564a.c
drvVXI1564a_SRCS += devVXI1564a.c
drvVXI1564a_LIBS += $(EPICS_BASE_IOC_LIBS) $(MDSLib) $(VISALib)

#=============================
# Build the IOC application

PROD_IOC = MCWIN2
# MCWIN2.dbd will be created and installed
DBD += MCWIN2.dbd

# MCWIN2.dbd will be made up from these files:
MCWIN2_DBD += base.dbd
MCWIN2_DBD += xxxSupport.dbd
MCWIN2_DBD += dbSubExample.dbd
MCWIN2_DBD += MCWIN2Hello.dbd
MCWIN2_DBD += initTrace.dbd
MCWIN2_DBD += drvVXI1564a.dbd
MCWIN2_DBD += devVXI1564a.dbd

#MCWIN2_DBD += timestampRecord.dbd

MCWIN2_DBD += dbSubNet.dbd
MCWIN2_DBD += devSysMon.dbd
MCWIN2_DBD += subHealthState.dbd
MCWIN2_DBD += pvCrawler.dbd

# MCWIN2_registerRecordDeviceDriver.cpp derives from MCWIN2.dbd
MCWIN2_SRCS += MCWIN2_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
MCWIN2_SRCS_DEFAULT += MCWIN2Main.cpp
MCWIN2_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#MCWIN2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
MCWIN2_LIBS += MCWIN2Support
#MCWIN2_LIBS += recTimeStamp
MCWIN2_LIBS += drvVXI1564a

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into MCWIN2Support
    sncExample_SNCFLAGS += +r
    MCWIN2_DBD += sncExample.dbd
    MCWIN2Support_SRCS += sncExample.stt
    MCWIN2Support_SRCS += sncMCDaqSeq.stt 
    MCWIN2Support_SRCS += sncMCDaqStatus.stt 
    MCWIN2Support_SRCS += sncMCMasterSet.stt 
    MCWIN2Support_LIBS += seq pv
    MCWIN2_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
MCWIN2_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

