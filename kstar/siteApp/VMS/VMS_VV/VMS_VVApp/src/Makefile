TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# set user defined 
USR_INCLUDES        += -I/usr/local/epics/siteLib/include
USR_DBDFLAGS        += -I/usr/local/epics/siteLib/dbd


seq_DIR                 += /usr/local/epics/siteLib/lib/$(T_A)
pv_DIR                  += /usr/local/epics/siteLib/lib/$(T_A)
asyn_DIR   		+= /usr/local/epics/siteLib/lib/linux-x86
#asyn_DIRS           = $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR   	+= /usr/local/epics/siteLib/lib/linux-x86
#recTimeStamp_DIRS   = $(EPICS_PATH)/siteLib/lib/$(T_A)
pfeifferTPG262_DIR 	+= /usr/local/epics/siteLib/lib/$(T_A)
devSRSRGA_DIR     	+= /usr/local/epics/siteLib/lib/$(T_A)
vgcGP307_DIR       	+= /usr/local/epics/siteLib/lib/$(T_A)
#nicFP20x0_DIRS      = $(EPICS_PATH)/siteLib/lib/$(T_A)
devPumpCRP_DIR     	+= /usr/local/epics/siteLib/lib/$(T_A)
devPumpTMP_DIR     	+= /usr/local/epics/siteLib/lib/$(T_A)
devPumpMBP_DIR     	+= /usr/local/epics/siteLib/lib/$(T_A)
ether_ip_DIR     	+= /usr/local/epics/siteLib/lib/$(T_A)
sysMon_DIR 		+= /usr/local/epics/siteLib/lib/$(T_A)

#=============================
# build an ioc application

PROD_IOC = VMS_VV
# <name>.dbd will be created from <name>Include.dbd
DBD += VMS_VV.dbd

VMS_VV_DBD += base.dbd
VMS_VV_DBD += timestampRecord.dbd
VMS_VV_DBD += devSRSRGA.dbd
VMS_VV_DBD += asyn.dbd
VMS_VV_DBD += drvGenericSerial.dbd
VMS_VV_DBD += devGpib.dbd
VMS_VV_DBD += drvTPG262.dbd
VMS_VV_DBD += devTPG262.dbd
VMS_VV_DBD += drvGP307.dbd
VMS_VV_DBD += devGP307.dbd
VMS_VV_DBD += devPumpCRP.dbd
VMS_VV_DBD += devPumpTMP.dbd
VMS_VV_DBD += devPumpMBP.dbd
VMS_VV_DBD += ether_ip.dbd
VMS_VV_DBD += sysMon.dbd
# Next line was added for pvcrawler system command [shbaek, 2010.01.22]
VMS_VV_DBD += pvCrawler.dbd
VMS_VV_DBD += subIfDriver.dbd
VMS_VV_DBD += subHealthState.dbd


# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
VMS_VV_SRCS += VMS_VV_registerRecordDeviceDriver.cpp
VMS_VV_SRCS_DEFAULT += VMS_VVMain.cpp
VMS_VV_SRCS_vxWorks += -nil-

# Add locally compiled object code
#VMS_VV_SRCS += dbSubExample.c

# The following adds support from base/src/vxWorks
#VMS_VV_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#VMS_VV_LIBS += asyn
VMS_VV_LIBS += recTimeStamp
VMS_VV_LIBS += seq pv
VMS_VV_LIBS += devSRSRGA
VMS_VV_LIBS += pfeifferTPG262
VMS_VV_LIBS += vgcGP307
#VMS_VV_LIBS += nicFP20x0
VMS_VV_LIBS += devPumpCRP
VMS_VV_LIBS += devPumpTMP
VMS_VV_LIBS += devPumpMBP
VMS_VV_LIBS += asyn
VMS_VV_LIBS += ether_ip
VMS_VV_LIBS += sysMon
VMS_VV_LIBS += $(EPICS_BASE_IOC_LIBS)


# NOTES:
#      1)It is not possible to build sncExample both as a component of VMS_VV
#        and standalone. You must choose only one.
#      2)To build sncExample SNCSEQ must be defined in <top>/configure/RELEASE

# The following builds sncExample as a component of VMS_VV

#VMS_VV_SRCS += sncTMPScan.stt
#VMS_VV_SRCS += sncCRPScan.stt
#VMS_VV_SRCS += sncCRPModeSelector.stt
#VMS_VV_SRCS += sncScenario.stt
#VMS_VV_LIBS += seq pv
VMS_VV_DBD += sncSRSRGA.dbd
VMS_VV_SRCS += sncSRSRGA_VV.stt
VMS_VV_SRCS += sncSRSRGA2_VV.stt
VMS_VV_SRCS += sncCRPGV.stt
VMS_VV_SRCS += subIfDriver.cpp
VMS_VV_SRCS += subHealthState.cpp
VMS_VV_SRCS += userclass.cpp
#VMS_VV_LIBS += $(EPICS_BASE_IOC_LIBS)


# The following builds sncExample as a standalone application
#PROD_HOST += sncExample
#sncExample_SNCFLAGS += +m
#sncExample_SRCS += sncExample.stt
#sncExample_LIBS += seq pv
#sncExample_LIBS += $(EPICS_BASE_HOST_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

