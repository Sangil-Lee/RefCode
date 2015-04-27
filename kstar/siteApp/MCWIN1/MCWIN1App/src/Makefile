TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# No optimization, we want debugging info:
#HOST_OPT=NO
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

LIBRARY_IOC += MCWIN1Support
LIBRARY_IOC += drvVXI1564a

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
MCWIN1Support_SRCS += xxxRecord.c
MCWIN1Support_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
MCWIN1Support_SRCS += dbSubExample.c
MCWIN1Support_SRCS += MCWIN1Hello.c
MCWIN1Support_SRCS += initTrace.c

MCWIN1Support_SRCS += dbSubNet.cpp
MCWIN1Support_SRCS += devSysMon.cpp
MCWIN1Support_SRCS += subHealthState.cpp
MCWIN1Support_SRCS += winPerfmon.cpp

MCWIN1Support_LIBS += $(EPICS_BASE_IOC_LIBS)

DBD    	     += drvVXI1564a.dbd
DBD          += devVXI1564a.dbd
drvVXI1564a_SRCS += drvVXI1564a.c
drvVXI1564a_SRCS += devVXI1564a.c
drvVXI1564a_LIBS += $(EPICS_BASE_IOC_LIBS) $(MDSLib) $(VISALib)

#=============================
# Build the IOC application

PROD_IOC = MCWIN1
# MCWIN1.dbd will be created and installed
DBD += MCWIN1.dbd

# MCWIN1.dbd will be made up from these files:
MCWIN1_DBD += base.dbd
MCWIN1_DBD += xxxSupport.dbd
MCWIN1_DBD += dbSubExample.dbd
MCWIN1_DBD += MCWIN1Hello.dbd
MCWIN1_DBD += initTrace.dbd
MCWIN1_DBD += drvVXI1564a.dbd
MCWIN1_DBD += devVXI1564a.dbd

#MCWIN1_DBD += timestampRecord.dbd

MCWIN1_DBD += dbSubNet.dbd
MCWIN1_DBD += devSysMon.dbd
MCWIN1_DBD += subHealthState.dbd
MCWIN1_DBD += pvCrawler.dbd


# MCWIN1_registerRecordDeviceDriver.cpp derives from MCWIN1.dbd
MCWIN1_SRCS += MCWIN1_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
MCWIN1_SRCS_DEFAULT += MCWIN1Main.cpp
MCWIN1_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#MCWIN1_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
MCWIN1_LIBS += MCWIN1Support
#MCWIN1_LIBS += recTimeStamp
MCWIN1_LIBS += drvVXI1564a

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into MCWIN1Support
    sncExample_SNCFLAGS += +r
    MCWIN1_DBD += sncExample.dbd
    MCWIN1Support_SRCS += sncExample.stt
    MCWIN1Support_SRCS += sncMCDaqSeq.stt 
    MCWIN1Support_SRCS += sncMCDaqStatus.stt 
    MCWIN1Support_SRCS += sncMCMasterSet.stt 
    MCWIN1Support_LIBS += seq pv
    MCWIN1_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
MCWIN1_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

