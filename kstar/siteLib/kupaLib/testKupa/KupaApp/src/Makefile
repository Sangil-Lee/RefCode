TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# for MDSplus
USR_INCLUDES += -I/usr/local/mdsplus/include
USR_LDFLAGS  += -L/usr/local/mdsplus/lib
USR_SYS_LIBS += TreeShr TdiShr MdsLib MdsShr c popt

#=============================
# Build the IOC support library

LIBRARY_IOC += KupaSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
KupaSupport_SRCS += xxxRecord.c
KupaSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
KupaSupport_SRCS += dbSubExample.c
KupaSupport_SRCS += KupaHello.c
KupaSupport_SRCS += initTrace.c

KupaSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = Kupa
# Kupa.dbd will be created and installed
DBD += Kupa.dbd

# Kupa.dbd will be made up from these files:
Kupa_DBD += base.dbd
Kupa_DBD += xxxSupport.dbd
Kupa_DBD += dbSubExample.dbd
Kupa_DBD += KupaHello.dbd
Kupa_DBD += initTrace.dbd

# Kupa_registerRecordDeviceDriver.cpp derives from Kupa.dbd
Kupa_SRCS += Kupa_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Kupa_SRCS_DEFAULT += KupaMain.cpp
Kupa_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Kupa_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
Kupa_LIBS += KupaSupport

# -------------------------------
# TODO: Kupa library test
# -------------------------------
USR_DBDFLAGS+= -I$(EPICS_SITE)/dbd

kupa_DIR  += $(EPICS_SITE)/lib/$(T_A)
kutil_DIR+= $(EPICS_SITE)/lib/$(T_A)

Kupa_DBD  += kupaLib.dbd
Kupa_LIBS += kupa

Kupa_DBD  += kutil.dbd
Kupa_LIBS += kutil
# -------------------------------

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into KupaSupport
    sncExample_SNCFLAGS += +r
    Kupa_DBD += sncExample.dbd
    KupaSupport_SRCS += sncExample.stt
    KupaSupport_LIBS += seq pv
    KupaSupport_LIBS += kupa kutil
    Kupa_LIBS += seq pv
    Kupa_LIBS += kupa kutil

    # Build sncProgram as a standalone program
#PROD_HOST += sncProgram
#sncProgram_SNCFLAGS += +m
#sncProgram_SRCS += sncProgram.st
#sncProgram_LIBS += seq pv
#sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
Kupa_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

