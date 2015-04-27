TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
RFM2G_LIBDIR=/usr/lib/rfm2g
GSC_DIR=/root/download/gsc/16aiss8ao4
DIO24_DIR=/root/download/gsc/dio24

USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
#USR_LIBS += TreeShr TdiShr MdsLib MdsShr 
USR_LDFLAGS += -L$(RFM2G_LIBDIR) -lrfm2g 
USR_LDFLAGS += -L$(GSC_DIR)/utils -l16aiss8ao4_utils
USR_LDFLAGS += -L$(GSC_DIR)/docsrc -l16aiss8ao4_dsl 
#USR_LDFLAGS += -L/usr/lib/rfm2g -lrfm2g 
#USR_CFLAGS += -Wall -I/usr/lib/rfm2g -DRFM2G_LINUX 
#USR_LDFLAGS += -L$(DIO24_DIR)/dio24/docsrc/linux/gcc -ldio24_dsl 
USR_LDFLAGS += -L/usr/local/lib/ -ldio24_api 

USR_INCLUDES     += -I/usr/local/include -I$(RFM2G_LIBDIR)
USR_INCLUDES     += -I$(GSC_DIR)/driver -I$(GSC_DIR)/utils -I$(GSC_DIR)/docsrc
USR_INCLUDES     += -I$(DIO24_DIR)/dio24/api -I$(DIO24_DIR)/common/api -I$(DIO24_DIR)/dio24/docsrc

USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
cltuDev_DIR       += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR	+= $(EPICS_SITE)/lib/$(T_A)

#=============================
# Build the IOC support library



#=============================
# Build the IOC application

PROD_IOC = RMP
# RMP.dbd will be created and installed
DBD += RMP.dbd

# RMP.dbd will be made up from these files:
RMP_DBD += base.dbd
RMP_DBD += initTrace.dbd

RMP_DBD += dbd16aiss8ao4.dbd
RMP_DBD += dbdRfm5565.dbd
RMP_DBD += dbdDio24.dbd

RMP_DBD += timestampRecord.dbd
RMP_DBD += sysMon.dbd
RMP_DBD += subIfDriver.dbd
RMP_DBD += subHealthState.dbd
RMP_DBD += pvCrawler.dbd
RMP_DBD += sfwDBD.dbd
RMP_DBD += cltuBase.dbd


# RMP_registerRecordDeviceDriver.cpp derives from RMP.dbd
RMP_SRCS += RMP_registerRecordDeviceDriver.cpp
RMP_SRCS += initTrace.c

RMP_SRCS += drv16aiss8ao4.c
RMP_SRCS += dev16aiss8ao4.c
RMP_SRCS += drvDio24.c
RMP_SRCS += devDio24.c
RMP_SRCS += drvRfm5565.c
RMP_SRCS += devRfm5565.c
RMP_SRCS += myMDSplus.c

RMP_SRCS += subIfDriver.cpp
RMP_SRCS += subHealthState.cpp
RMP_SRCS += userclass.cpp

# Build the main IOC entry point on workstation OSs.
RMP_SRCS_DEFAULT += RMPMain.cpp
RMP_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#RMP_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library

RMP_LIBS += recTimeStamp
RMP_LIBS += sysMon
RMP_LIBS += sfwLib
RMP_LIBS += cltuDev


USR_OBJS +=  $(RFM2G_LIBDIR)/close.o
USR_OBJS +=  $(RFM2G_LIBDIR)/open.o
USR_OBJS +=  $(RFM2G_LIBDIR)/config.o
USR_OBJS +=  $(RFM2G_LIBDIR)/event.o
USR_OBJS +=  $(RFM2G_LIBDIR)/rfm2g_errno.o
USR_OBJS +=  $(RFM2G_LIBDIR)/transfer.o
USR_OBJS +=  $(RFM2G_LIBDIR)/util.o
#USR_OBJS +=  $(TOP)/GSC/utils/util_count.o
#USR_OBJS +=  $(TOP)/GSC/utils/util_select.o


# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into RMPSupport
    RMP_SNCFLAGS += +r
    RMP_DBD += sncRuntimeSeq.dbd
    RMP_SRCS += sncRuntimeSeq.stt

    RMP_DBD += sncMPSWatchdog.dbd
    RMP_SRCS += sncMPSWatchdog.stt
    
    RMP_LIBS += seq pv

    # Build sncProgram as a standalone program
#    PROD_HOST += sncProgram
#    sncProgram_SNCFLAGS += +m
#    sncProgram_SRCS += sncProgram.st
#    sncProgram_LIBS += seq pv
#    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
RMP_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

