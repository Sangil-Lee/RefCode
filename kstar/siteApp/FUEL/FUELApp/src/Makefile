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

PROD_IOC = FUEL
# FUEL.dbd will be created and installed
DBD += FUEL.dbd

# FUEL.dbd will be made up from these files:
FUEL_DBD += base.dbd
FUEL_DBD += ether_ip.dbd
FUEL_DBD += timestampRecord.dbd
FUEL_DBD += sysMon.dbd
#FUEL_DBD += 
# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
FUEL_DBD += pvCrawler.dbd
FUEL_DBD += subIfDriver.dbd
FUEL_DBD += subHealthState.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
FUEL_SRCS += FUEL_registerRecordDeviceDriver.cpp
FUEL_SRCS += subIfDriver.cpp
FUEL_SRCS += subHealthState.cpp
FUEL_SRCS += userclass.cpp

FUEL_SRCS_DEFAULT += FUELMain.cpp

# Add locally compiled object code

# The following adds support from base/src/vxWorks

FUEL_LIBS += ether_ip
FUEL_LIBS += recTimeStamp
FUEL_LIBS += seq pv
FUEL_LIBS += sysMon

FUEL_SNCFLAGS += +r
FUEL_DBD += sncFUEL.dbd
FUEL_SRCS += sncFUEL.stt



FUEL_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

