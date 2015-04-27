TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library

USR_INCLUDES += -I/usr/local/epics/siteLib/include
USR_INCLUDES += -I/usr/local/epics/extensions/include
USR_DBDFLAGS += -I/usr/local/epics/siteLib/dbd

#=============================
# build an ioc application

PROD_IOC = CWF
# CWF.dbd will be created and installed
DBD += CWF.dbd

# CWF.dbd will be made up from these files:
CWF_DBD += base.dbd
CWF_DBD += ether_ip.dbd
CWF_DBD += timestampRecord.dbd
CWF_DBD += sysMon.dbd
CWF_DBD += pvCrawler.dbd
CWF_DBD += subIfDriver.dbd
CWF_DBD += subHealthState.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
CWF_SRCS += CWF_registerRecordDeviceDriver.cpp
CWF_SRCS_DEFAULT += CWFMain.cpp
CWF_SRCS += subIfDriver.cpp
CWF_SRCS += subHealthState.cpp
CWF_SRCS += userclass.cpp

CWF_SRCS_vxWorks += -nil-

# Add locally compiled object code
# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

CWF_LIBS += ether_ip
CWF_LIBS += recTimeStamp
CWF_LIBS += sysMon


CWF_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

