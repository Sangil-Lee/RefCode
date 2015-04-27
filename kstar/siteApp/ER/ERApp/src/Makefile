TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_LDFLAGS     += -lniFgen_32 -lniScope_32 

USR_INCLUDES    += -I/usr/local/mdsplus/include
USR_INCLUDES    += -I/usr/local/epics/siteApp/ZTEC/include
USR_INCLUDES	+= -I$(EPICS_SITE)/include

USR_LDFLAGS     += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc
#USR_LDFLAGS     += -L/usr/local/epics/siteApp/ZTEC/lib -lzbind -lztwavesim -lztwaveC
USR_LDFLAGS     += -L/usr/local/share/ZWaveC/SFP/lib -lzbind -lzcard -lztwavesim -lztwaveC
#USR_LDFLAGS	+= -L/usr/local/lib -lniFgen_32 -lniScope_32 

USR_DBDFLAGS	+= -I$(EPICS_SITE)/dbd

cltuDev_DIR += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR += $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR += $(EPICS_SITE)/lib/$(T_A)
kutil_DIR += $(EPICS_SITE)/lib/$(T_A)
ktree_DIR += $(EPICS_SITE)/lib/$(T_A)




#=============================
# Build the IOC support library



#=============================
# Build the IOC application

PROD_IOC = ER
# ER.dbd will be created and installed
DBD += ER.dbd
# ER.dbd will be made up from these files:
ER_DBD += base.dbd
ER_DBD += dbd_NIfgen.dbd
ER_DBD += dbd_ZT530.dbd
ER_DBD += dbd_NIscope.dbd

ER_DBD += sfwDBD.dbd
ER_DBD += initTrace.dbd
ER_DBD += cltuBase.dbd
ER_DBD += sysMon.dbd
ER_DBD += timestampRecord.dbd
ER_DBD += subIfDriver.dbd
ER_DBD += subHealthState.dbd
ER_DBD += kutil.dbd
ER_DBD += sfwTree.dbd

# ER_registerRecordDeviceDriver.cpp derives from ER.dbd
ER_SRCS += ER_registerRecordDeviceDriver.cpp
ER_SRCS += initTrace.c 
ER_SRCS += devNIfgen.c
ER_SRCS += drvNIfgen.c
ER_SRCS += devZT530.c
ER_SRCS += drvZT530.c
ER_SRCS += devNIscope.c
ER_SRCS += drvNIscope.c
ER_SRCS += myMDSplus.c


ER_SRCS += subIfDriver.cpp
ER_SRCS += subHealthState.cpp
ER_SRCS += userclass.cpp

# Build the main IOC entry point on workstation OSs.
ER_SRCS_DEFAULT += ERMain.cpp
ER_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#ER_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
ER_LIBS += cltuDev
ER_LIBS += sfwLib
ER_LIBS += sysMon
ER_LIBS += recTimeStamp
ER_LIBS += kutil
ER_LIBS += ktree

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into ERSupport
    ER_SNCFLAGS += +r
    ER_DBD += sncLogicSeq.dbd
    ER_SRCS += sncLogicSeq.stt
#    ERSupport_LIBS += seq pv
    ER_LIBS += seq pv

    # Build sncProgram as a standalone program
#    PROD_HOST += sncProgram
#    sncProgram_SNCFLAGS += +m
#    sncProgram_SRCS += sncProgram.st
#    sncProgram_LIBS += seq pv
#    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
ER_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

