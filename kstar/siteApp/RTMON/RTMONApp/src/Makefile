TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

RFM2G_LIBDIR=/usr/lib64/rfm2g

USR_INCLUDES    += -I$(EPICS_SITE)/include
USR_INCLUDES    += -I$(EPICS_SITE)/intLtuLib/driver
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
USR_INCLUDES     += -I/usr/local/include -I$(RFM2G_LIBDIR)

USR_DBDFLAGS    += -I$(EPICS_SITE)/dbd

#USR_CFLAGS += -Wall -Werror 
USR_LDFLAGS += -L$(RFM2G_LIBDIR) -lrfm2g -lrdmacm -libverbs


USR_OBJS +=  $(RFM2G_LIBDIR)/close.o
USR_OBJS +=  $(RFM2G_LIBDIR)/open.o
USR_OBJS +=  $(RFM2G_LIBDIR)/config.o
USR_OBJS +=  $(RFM2G_LIBDIR)/event.o
USR_OBJS +=  $(RFM2G_LIBDIR)/rfm2g_errno.o
USR_OBJS +=  $(RFM2G_LIBDIR)/transfer.o
USR_OBJS +=  $(RFM2G_LIBDIR)/util.o


sfwLib_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)


#=============================
# Build the IOC support library


#=============================
# Build the IOC application

PROD_IOC = RTMON
# RTMON.dbd will be created and installed
DBD += RTMON.dbd

# RTMON.dbd will be made up from these files:
RTMON_DBD += base.dbd
#RTMON_DBD += xxxSupport.dbd
#RTMON_DBD += dbSubExample.dbd
#RTMON_DBD += RTMONHello.dbd
#RTMON_DBD += initTrace.dbd
RTMON_DBD += sfwDBD.dbd
RTMON_DBD += dbdRMCHK.dbd
RTMON_DBD += dbdRTCORE.dbd

# RTMON_registerRecordDeviceDriver.cpp derives from RTMON.dbd
RTMON_SRCS += RTMON_registerRecordDeviceDriver.cpp

RTMON_SRCS += devRTCORE.c
RTMON_SRCS += drvRTCORE.c
RTMON_SRCS += devRMCHK.c
RTMON_SRCS += drvRMCHK.c
RTMON_SRCS += common.c



# Build the main IOC entry point on workstation OSs.
RTMON_SRCS_DEFAULT += RTMONMain.cpp
RTMON_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#RTMON_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#RTMON_LIBS += RTMONSupport
RTMON_LIBS += sfwLib


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into RTMONSupport
    sncExample_SNCFLAGS += +r
    RTMON_DBD += sncExample.dbd
    RTMONSupport_SRCS += sncExample.stt
    RTMONSupport_LIBS += seq pv
    RTMON_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
RTMON_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

