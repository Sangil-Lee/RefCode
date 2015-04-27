TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
#USR_DBDFLAGS += -I/usr/local/epics/asyn/dbd
USR_INCLUDES        += -I/usr/local/epics/siteLib/include
USR_DBDFLAGS += -I/usr/local/epics/siteLib/dbd


#ether_ip_DIR = /usr/local/epics/siteLib/lib/linux-x86
#autosave_DIR =/usr/local/epics/siteLib/lib/linux-x86
g2c_DIR = /usr/lib
#heProp_DIR = /usr/local/epics/siteLib/lib/linux-x86
#recTimeStamp_DIR = /usr/local/epics/siteLib/lib/linux-x86
seq_DIR = /usr/local/epics/siteLib/lib/linux-x86
pv_DIR = /usr/local/epics/siteLib/lib/linux-x86
#s7plc_DIR = /usr/local/epics/siteLib/lib/linux-x86
#sysMon_DIR   = /usr/local/epics/siteLib/lib/linux-x86
#devSRSRGA_DIR = /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIR = /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIR = /usr/local/epics/asyn/lib/linux-x86

#=============================
# build an ioc application

PROD_IOC = HDS
# SHDS.dbd will be created and installed

DBD += HDS.dbd

# SHDS.dbd will be made up from these files:

HDS_DBD += base.dbd
#SHDS_DBD += devSRSRGA.dbd
HDS_DBD += heProp.dbd
HDS_DBD += ether_ip.dbd
HDS_DBD += timestampRecord.dbd
HDS_DBD += s7plcBase.dbd
HDS_DBD += s7plcReg.dbd
HDS_DBD += s7plcCalcout.dbd
HDS_DBD += sysMon.dbd
#SHDS_DBD += asSupport.dbd
HDS_DBD += asyn.dbd
HDS_DBD += devGpib.dbd
HDS_DBD += drvAsynIPPort.dbd
#SHDS_DBD += drvAsynSerialPort.dbd
#SHDS_DBD += drvGenericSerial.dbd
HDS_DBD += devSRSRGA.dbd
#SHDS_DBD += sncSRSRGA.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
HDS_DBD += pvCrawler.dbd
HDS_DBD += subIfDriver.dbd
HDS_DBD += subHealthState.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
HDS_SRCS += HDS_registerRecordDeviceDriver.cpp
HDS_SRCS += subIfDriver.cpp
HDS_SRCS += subHealthState.cpp
HDS_SRCS += userclass.cpp
HDS_SRCS_DEFAULT += HDSMain.cpp

HDS_LIBS += devSRSRGA
#SHDS_LIBS += seq pv
HDS_LIBS += asyn
HDS_LIBS += heProp
HDS_LIBS += ether_ip
HDS_LIBS += recTimeStamp
#SHDS_LIBS += autosave
HDS_LIBS += s7plc
HDS_LIBS += g2c
HDS_LIBS += sysMon


HDS_SNCFLAGS += +r
HDS_DBD += sncSRSRGA.dbd
HDS_LIBS += seq pv
HDS_SRCS += sncSRSRGA.stt
HDS_SRCS += sncHDSHCS.stt


HDS_LIBS += $(EPICS_BASE_IOC_LIBS)
include $(TOP)/configure/RULES

