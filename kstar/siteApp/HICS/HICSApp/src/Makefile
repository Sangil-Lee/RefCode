TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#RFM2G_LIBDIR=/usr/lib/rfm2g
RFM2G_LIBDIR=/usr/lib64/rfm2g

USR_INCLUDES	+= -I$(EPICS_SITE)/include
USR_INCLUDES    += -I$(EPICS_SITE)/intLtuLib/driver
USR_INCLUDES    += -I$(MDSPLUS_DIR)/include
USR_INCLUDES	+= -I/usr/local/include -I$(RFM2G_LIBDIR)

USR_DBDFLAGS	+= -I$(EPICS_SITE)/dbd
USR_LDFLAGS		+= -L$(RFM2G_LIBDIR) -lrfm2g 

USR_OBJS		+= $(RFM2G_LIBDIR)/close.o
USR_OBJS		+= $(RFM2G_LIBDIR)/open.o
USR_OBJS		+= $(RFM2G_LIBDIR)/config.o
USR_OBJS		+= $(RFM2G_LIBDIR)/event.o
USR_OBJS		+= $(RFM2G_LIBDIR)/rfm2g_errno.o
USR_OBJS		+= $(RFM2G_LIBDIR)/transfer.o
USR_OBJS		+= $(RFM2G_LIBDIR)/util.o

sfwLib_DIR		+= $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR+= $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR		+= $(EPICS_SITE)/lib/$(T_A)
kupa_DIR		+= $(EPICS_SITE)/lib/$(T_A)


#=============================
# Build the IOC support library


#=============================
# Build the IOC application

PROD_IOC = HICS
# HICS.dbd will be created and installed
DBD += HICS.dbd

# HICS.dbd will be made up from these files:
HICS_DBD += base.dbd
HICS_DBD += sfwDBD.dbd
HICS_DBD += kutil.dbd
HICS_DBD += dbdEC1_ANT.dbd
HICS_DBD += timestampRecord.dbd
HICS_DBD += sysMon.dbd
HICS_DBD += subIfDriver.dbd
HICS_DBD += subHealthState.dbd
HICS_DBD += pvCrawler.dbd
HICS_DBD += kupaLib.dbd

# HICS_registerRecordDeviceDriver.cpp derives from HICS.dbd
HICS_SRCS += HICS_registerRecordDeviceDriver.cpp

HICS_SRCS += kuRFM.cpp
HICS_SRCS += SerialImpl.cpp
HICS_SRCS += UdpImpl.cpp
HICS_SRCS += ControllerHandler.cpp
HICS_SRCS += RfmHandler.cpp
HICS_SRCS += devEC1_ANT.c
HICS_SRCS += drvEC1_ANT.c
HICS_SRCS += EchPowerCtrl.cpp

HICS_SRCS += subIfDriver.cpp
HICS_SRCS += subHealthState.cpp
HICS_SRCS += userclass.cpp

# Build the main IOC entry point on workstation OSs.
HICS_SRCS_DEFAULT += HICSMain.cpp
HICS_SRCS_vxWorks += -nil-

# Link in the code from the support library
#HICS_LIBS += HICSSupport
HICS_LIBS += sfwLib
HICS_LIBS += kutil
HICS_LIBS += kupa
HICS_LIBS += recTimeStamp
HICS_LIBS += sysMon
HICS_LIBS += pxi6259

# Finally link to the EPICS Base libraries
HICS_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

