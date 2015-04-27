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
#seq_DIR = /usr/local/epics/siteLib/lib/linux-x86
#pv_DIR = /usr/local/epics/siteLib/lib/linux-x86
#s7plc_DIR = /usr/local/epics/siteLib/lib/linux-x86
#sysMon_DIR   = /usr/local/epics/siteLib/lib/linux-x86
#devSRSRGA_DIR = /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIR = /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIR = /usr/local/epics/asyn/lib/linux-x86

#=============================
# build an ioc application

PROD_IOC = PMS
# PMS.dbd will be created and installed

DBD += PMS.dbd

# PMS.dbd will be made up from these files:

PMS_DBD += base.dbd
#PMS_DBD += devSRSRGA.dbd
#PMS_DBD += heProp.dbd
PMS_DBD += ether_ip.dbd
PMS_DBD += timestampRecord.dbd
#PMS_DBD += s7plcBase.dbd
#PMS_DBD += s7plcReg.dbd
#PMS_DBD += s7plcCalcout.dbd
PMS_DBD += sysMon.dbd
#PMS_DBD += asSupport.dbd
#PMS_DBD += asyn.dbd
#PMS_DBD += devGpib.dbd
#PMS_DBD += drvAsynIPPort.dbd
#PMS_DBD += drvAsynSerialPort.dbd
#PMS_DBD += drvGenericSerial.dbd
#PMS_DBD += devSRSRGA.dbd
#PMS_DBD += sncSRSRGA.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.25]
PMS_DBD += pvCrawler.dbd
PMS_DBD += subIfDriver.dbd
PMS_DBD += subHealthState.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
PMS_SRCS += PMS_registerRecordDeviceDriver.cpp
PMS_SRCS += subIfDriver.cpp
PMS_SRCS += subHealthState.cpp
PMS_SRCS += userclass.cpp
PMS_SRCS_DEFAULT += PMSMain.cpp

#PMS_LIBS += devSRSRGA
#PMS_LIBS += seq pv
#PMS_LIBS += asyn
#PMS_LIBS += heProp
PMS_LIBS += ether_ip
PMS_LIBS += recTimeStamp
#PMS_LIBS += autosave
#PMS_LIBS += s7plc
#PMS_LIBS += g2c
PMS_LIBS += sysMon


PMS_SNCFLAGS += +r
PMS_DBD += sncPMSMAX.dbd
PMS_LIBS += seq pv
PMS_SRCS += sncPMSMAX.stt
PMS_SRCS += sncPMSInterlock.stt 
#PMS_SRCS += sncPMSAL.stt
#PMS_SRCS += sncTmsMain.stt


PMS_LIBS += $(EPICS_BASE_IOC_LIBS)
include $(TOP)/configure/RULES

