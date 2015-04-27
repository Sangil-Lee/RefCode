TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_DBDFLAGS += -I$(EPICS)/siteLib/dbd

ether_ip_DIR = /usr/local/epics/siteLib/lib/$(T_A)
recTimeStamp_DIR = /usr/local/epics/siteLib/lib/$(T_A)
seq_DIR = /usr/local/epics/siteLib/lib/$(T_A)
sysMon_DIR   = /usr/local/epics/siteLib/lib/linux-x86




#==================================================
# Build an IOC support library


#=============================
# build an ioc application

PROD_IOC = SMBI
# SMBI.dbd will be created and installed
DBD += SMBI.dbd

# SMBI.dbd will be made up from these files:
SMBI_DBD += base.dbd
SMBI_DBD += ether_ip.dbd
SMBI_DBD += timestampRecord.dbd
SMBI_DBD += sysMon.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
SMBI_DBD += pvCrawler.dbd
SMBI_DBD += subIfDriver.dbd
SMBI_DBD += subHealthState.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
SMBI_SRCS += SMBI_registerRecordDeviceDriver.cpp
SMBI_SRCS += subIfDriver.cpp
SMBI_SRCS += subHealthState.cpp
SMBI_SRCS += userclass.cpp

SMBI_SRCS_DEFAULT += SMBIMain.cpp


# Add locally compiled object code

# The following adds support from base/src/vxWorks
SMBI_LIBS += ether_ip
SMBI_LIBS += recTimeStamp
SMBI_LIBS += seq pv
SMBI_LIBS += sysMon

SMBI_SNCFLAGS += +r
SMBI_DBD += sncFUEL.dbd
SMBI_SRCS += sncFUEL2.stt



SMBI_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

