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

PROD_IOC = FUEL_S_M
# FUEL_S_M.dbd will be created and installed
DBD += FUEL_S_M.dbd

# FUEL_S_M.dbd will be made up from these files:
FUEL_S_M_DBD += base.dbd
FUEL_S_M_DBD += ether_ip.dbd
FUEL_S_M_DBD += timestampRecord.dbd
FUEL_S_M_DBD += sysMon.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
FUEL_S_M_DBD += pvCrawler.dbd
FUEL_S_M_DBD += subIfDriver.dbd
FUEL_S_M_DBD += subHealthState.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
FUEL_S_M_SRCS += FUEL_S_M_registerRecordDeviceDriver.cpp
FUEL_S_M_SRCS += subIfDriver.cpp
FUEL_S_M_SRCS += subHealthState.cpp
FUEL_S_M_SRCS += userclass.cpp

FUEL_S_M_SRCS_DEFAULT += FUEL_S_MMain.cpp


# Add locally compiled object code

# The following adds support from base/src/vxWorks
FUEL_S_M_LIBS += ether_ip
FUEL_S_M_LIBS += recTimeStamp
FUEL_S_M_LIBS += seq pv
FUEL_S_M_LIBS += sysMon

FUEL_SNCFLAGS += +r
FUEL_S_M_DBD += sncFUEL.dbd
FUEL_S_M_SRCS += sncFUEL2.stt



FUEL_S_M_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

