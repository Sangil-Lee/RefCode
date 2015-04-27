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
#g2c_DIR=/usr/local/epics/base/lib/linux-x86
#heProp_DIR = /usr/local/epics/siteLib/lib/linux-x86
#recTimeStamp_DIR = /usr/local/epics/siteLib/lib/linux-x86
#seq_DIR = /usr/local/epics/siteLib/lib/linux-x86
#pv_DIR = /usr/local/epics/siteLib/lib/linux-x86
#s7plc_DIR = /usr/local/epics/siteLib/lib/linux-x86
#sysMon_DIR   = /usr/local/epics/siteLib/lib/linux-x86
#devSRSRGA_DIR = /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIR = /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIR = /usr/local/epics/asyn/lib/linux-x86

#=============================
# build an ioc application

PROD_IOC = CLS
# CLS.dbd will be created and installed

DBD += CLS.dbd

# CLS.dbd will be made up from these files:

CLS_DBD += base.dbd
CLS_DBD += devSRSRGA.dbd
CLS_DBD += heProp.dbd
CLS_DBD += ether_ip.dbd
CLS_DBD += timestampRecord.dbd
#CLS_DBD += s7plcBase.dbd
#CLS_DBD += s7plcReg.dbd
#CLS_DBD += s7plcCalcout.dbd
CLS_DBD += sysMon.dbd
#CLS_DBD += asSupport.dbd
CLS_DBD += asyn.dbd
CLS_DBD += devGpib.dbd
CLS_DBD += drvAsynIPPort.dbd
CLS_DBD += drvAsynSerialPort.dbd
#CLS_DBD += drvGenericSerial.dbd
CLS_DBD += devSRSRGA.dbd
#CLS_DBD += sncSRSRGA.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
CLS_DBD += pvCrawler.dbd
CLS_DBD += subIfDriver.dbd 
CLS_DBD += subHealthState.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
CLS_SRCS += CLS_registerRecordDeviceDriver.cpp
CLS_SRCS += subIfDriver.cpp
CLS_SRCS += subHealthState.cpp
CLS_SRCS += userclass.cpp 
CLS_SRCS_DEFAULT += CLSMain.cpp

CLS_LIBS += devSRSRGA
#CLS_LIBS += seq pv
CLS_LIBS += asyn
CLS_LIBS += heProp
CLS_LIBS += ether_ip
CLS_LIBS += recTimeStamp
#CLS_LIBS += autosave
#CLS_LIBS += s7plc
CLS_LIBS += g2c
CLS_LIBS += sysMon


CLS_SNCFLAGS += +r
CLS_DBD += sncSRSRGA.dbd
CLS_LIBS += seq pv
CLS_SRCS += sncSRSRGA_PF.stt
CLS_SRCS += sncSRSRGA_TF.stt

CLS_LIBS += $(EPICS_BASE_IOC_LIBS)
include $(TOP)/configure/RULES

