TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES        += -IC:/epics/base/include
USR_INCLUDES        += -IC:/epics/base/include/os/WIN32
USR_INCLUDES        += -IC:/epics/siteLib/include
USR_DBDFLAGS        += -IC:/epics/siteLib/dbd
USR_INCLUDES        += -IC:/"Program Files"/MDSplus/DEVTOOLS/include
#USR_INCLUDES        += -IC:\epics\siteApp\VSS\VSSApp\src
#USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
#USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

#USR_LDFLAGS         += -Lc:/"Program Files"/MDSplus/DEVTOOLS/lib/ -ltreeshr -ltdishr -lmdslib -lmdsshr -lc -lpopt
#USR_LDFLAGS         += -L"C:\Program Files\MDSplus\DEVTOOLS\lib" -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc -lpopt

MDSLib_DIR += C:\"Program Files"\MDSplus\DEVTOOLS\lib
MDSLib += ATSIFIOM32 treeshr tdishr mdslib mdsshr

#USR_INCLUDES += -IC:\Window\system32
#systemLib_DIR += C:\Window\system32
#systemLib += gdi32 comdlg32 oleaut32 ws2_32 user32 advapi32

#=============================
# Build the IOC support library

LIBRARY_IOC += VSSSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
VSSSupport_SRCS += xxxRecord.c
VSSSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.

VSSSupport_SRCS += dbSubExample.c
VSSSupport_SRCS += VSSHello.c
VSSSupport_SRCS += initTrace.c
VSSSupport_SRCS += SIFandSPE.c

VSSSupport_SRCS += dbSubNet.cpp
VSSSupport_SRCS += devSysMon.cpp
VSSSupport_SRCS += subHealthState.cpp
VSSSupport_SRCS += winPerfmon.cpp

#VSSSupport_SRCS += AndorSIF.c 
#VSSSupport_SRCS += PiSPE.c
VSSSupport_LIBS += $(EPICS_BASE_IOC_LIBS) $(MDSLib)


#=============================
# Build the IOC application

PROD_IOC = VSS
# VSS.dbd will be created and installed
DBD += VSS.dbd

# VSS.dbd will be made up from these files:
VSS_DBD += base.dbd
VSS_DBD += xxxSupport.dbd
VSS_DBD += dbSubExample.dbd
VSS_DBD += VSSHello.dbd
VSS_DBD += initTrace.dbd

VSS_DBD += dbSubNet.dbd
VSS_DBD += devSysMon.dbd
VSS_DBD += subHealthState.dbd
VSS_DBD += pvCrawler.dbd

# VSS_registerRecordDeviceDriver.cpp derives from VSS.dbd
VSS_SRCS += VSS_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
VSS_SRCS_DEFAULT += VSSMain.cpp
VSS_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#VSS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#VSS_LIBS += gdi32 comdlg32 oleaut32 imm32 winmm winspool ws2_32 ole32 uuid user32 advapi32
VSS_LIBS += VSSSupport



# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into VSSSupport
    sncExample_SNCFLAGS += +r
    VSS_DBD += sncExample.dbd
    VSSSupport_SRCS += sncExample.stt
    VSSSupport_SRCS += sncSIFConvert.stt
    VSSSupport_SRCS += sncSPEConvert.stt
    VSSSupport_LIBS += seq pv
    VSS_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
VSS_LIBS += $(EPICS_BASE_IOC_LIBS) 
#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

