# Makefile
TOP = ../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# No optimization, we want debugging info:
HOST_OPT=NO
CROSS_OPT=NO

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_LDFLAGS         += -L/usr/local/mdsplus/lib
USR_SYS_LIBS        += TreeShr TdiShr MdsLib MdsShr c popt
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

cltuDev_DIR         += $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
kutil_DIR			+= $(EPICS_SITE)/lib/$(T_A)
kupa_DIR			+= $(EPICS_SITE)/lib/$(T_A)

#=============================
# Build the IOC application

PROD_IOC = EC1
# EC1.dbd will be created and installed
DBD += EC1.dbd

# EC1.dbd will be made up from these files:
EC1_DBD += base.dbd

# KSTAR Common : /usr/local/epics/siteLib/dbd
EC1_DBD += timestampRecord.dbd
EC1_DBD += sysMon.dbd         # CSM
EC1_DBD += cltuBase.dbd       # CLTU (Control/Local Timining Unit)
EC1_DBD += kutil.dbd          # KSTAR Utility
EC1_DBD += kupaLib.dbd        # KSTAR Utility for PV Archiver

# KSTAR Common : /usr/local/epics/siteApp/...
EC1_DBD += subIfDriver.dbd
EC1_DBD += subHealthState.dbd
EC1_DBD += pvCrawler.dbd
EC1_DBD += ether_ip.dbd

EC1_SRCS += EC1_registerRecordDeviceDriver.cpp

# create munch file for dynamic loading will install in <bin>
EC1_SRCS_DEFAULT += EC1Main.c
EC1_vxWorks += -nil-

EC1_SRCS += dl_list.c
EC1_SRCS += ether_ip.c
EC1_SRCS += drvEtherIP.c
EC1_SRCS += devEtherIP.c
EC1_SRCS += drvEtherIPRegister.cpp

EC1_SRCS += subIfDriver.cpp
EC1_SRCS += subHealthState.cpp
EC1_SRCS += userclass.cpp

# Link in the code from the support library
EC1_LIBS += recTimeStamp
EC1_LIBS += sysMon
EC1_LIBS += cltuDev
EC1_LIBS += kutil
EC1_LIBS += kupa

EC1_LIBS += Com

ifneq ($(SNCSEQ),)
	# This builds sncProgram as a component of IOC
	sncEC1_SNCFLAGS += +r
	EC1_DBD  += sncKupa.dbd
	EC1_SRCS += sncKupa.stt
	EC1_DBD  += sncPermReady.dbd
	EC1_SRCS += sncPermReady.stt
	EC1_LIBS += seq pv
	EC1_LIBS += kutil kupa
endif

# Finally link to the EPICS Base libraries
EC1_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

