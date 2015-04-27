TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

USR_CFLAGS += -D_CLTU_VXWORKS_
#USR_CFLAGS += -D_CLTU_LINUX_

#sysMon_DIR += $(EPICS_SITE)/lib/$(T_A)
#recTimeStamp_DIR += $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = TSS
# TSS.dbd will be created and installed
DBD += TSS.dbd

# TSS.dbd will be made up from these files:
TSS_DBD += base.dbd
#TSS_DBD += initTrace.dbd
TSS_DBD += cltuBase.dbd

# TSS_registerRecordDeviceDriver.cpp derives from TSS.dbd
TSS_SRCS += TSS_registerRecordDeviceDriver.cpp
#TSS_SRCS += initTrace.c

TSS_SRCS += devCLTU.c
TSS_SRCS += drvCLTU.c
TSS_SRCS += oz_cltu.c
TSS_SRCS += oz_xtu2.c


# Build the main IOC entry point on workstation OSs.
TSS_SRCS_DEFAULT += TSSMain.cpp
TSS_SRCS_vxWorks += -nil-
#TSS_SRCS_vxWorks += TSS_registerRecordDeviceDriver.cpp 

# Add support from base/src/vxWorks if needed
#TSS_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into TSSSupport

    # Build sncProgram as a standalone program
endif

# Finally link to the EPICS Base libraries
TSS_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

