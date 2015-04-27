TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# set user defined 
USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

asyn_DIR           = $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR   = $(EPICS_PATH)/siteLib/lib/$(T_A)
pfeifferTPG262_DIR = $(EPICS_PATH)/siteLib/lib/$(T_A)
vgcGP307_DIR       = $(EPICS_PATH)/siteLib/lib/$(T_A)
devPumpCRP_DIR     = $(EPICS_PATH)/siteLib/lib/$(T_A)
devPumpTMP_DIR     = $(EPICS_PATH)/siteLib/lib/$(T_A)
devPumpMBP_DIR     = $(EPICS_PATH)/siteLib/lib/$(T_A)
########################################################
devSRSRGA_DIR     = $(EPICS_PATH)/siteLib/lib/$(T_A)
########################################################
ether_ip_DIR       = $(EPICS_PATH)/siteLib/lib/$(T_A)
seq_DIR       = $(EPICS_PATH)/siteLib/lib/$(T_A)
pv_DIR       = $(EPICS_PATH)/siteLib/lib/$(T_A)
sysMon_DIR   = $(EPICS_PATH)/siteLib/lib/$(T_A)

#=============================
# build an ioc application

PROD_IOC = VMS_CR

# <name>.dbd will be created from <name>Include.dbd
DBD += VMS_CR.dbd

VMS_CR_DBD += base.dbd
VMS_CR_DBD += timestampRecord.dbd
VMS_CR_DBD += devSRSRGA.dbd
VMS_CR_DBD += asyn.dbd
VMS_CR_DBD += drvGenericSerial.dbd
VMS_CR_DBD += devGpib.dbd
VMS_CR_DBD += drvTPG262.dbd
VMS_CR_DBD += devTPG262.dbd
VMS_CR_DBD += drvGP307.dbd
VMS_CR_DBD += devGP307.dbd
VMS_CR_DBD += devPumpCRP.dbd
VMS_CR_DBD += devPumpTMP.dbd
VMS_CR_DBD += devPumpMBP.dbd
VMS_CR_DBD += ether_ip.dbd
VMS_CR_DBD += sysMon.dbd
# Next line was added for pvcrawler system co
VMS_CR_DBD += pvCrawler.dbd
VMS_CR_DBD += subHealthState.dbd
VMS_CR_DBD += subIfDriver.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
VMS_CR_SRCS += VMS_CR_registerRecordDeviceDriver.cpp
VMS_CR_SRCS_DEFAULT += VMS_CRMain.cpp
VMS_CR_SRCS_vxWorks += -nil-

# Add locally compiled object code
#VMS_CR_SRCS += dbSubExample.c

# The following adds support from base/src/vxWorks
#VMS_CR_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#VMS_CR_LIBS += asyn
VMS_CR_LIBS += recTimeStamp
VMS_CR_LIBS += pfeifferTPG262
VMS_CR_LIBS += vgcGP307
#VMS_CR_LIBS += nicFP20x0
VMS_CR_LIBS += devPumpCRP
VMS_CR_LIBS += devPumpTMP 
VMS_CR_LIBS += devPumpMBP
############################
VMS_CR_LIBS += devSRSRGA
############################
VMS_CR_LIBS += asyn
VMS_CR_LIBS += ether_ip
VMS_CR_LIBS += sysMon

# NOTES:
#      1)It is not possible to build sncExample both as a component of VMS_CR
#        and standalone. You must choose only one.
#      2)To build sncExample SNCSEQ must be defined in <top>/configure/RELEASE

# The following builds sncExample as a component of VMS_CR

#ifneq ($(SNCSEQ),)
#VMS_CR_SRCS += sncCRPScan.stt
#VMS_CR_SRCS += sncTMPScan.stt
#VMS_CR_SRCS += sncMBPScan.stt
#VMS_CR_SRCS += sncValveCheck.stt
#VMS_CR_SRCS += sncCRPModeSelector.stt
#VMS_CR_SRCS += sncScenario.stt
    #VMS_CR_SNCFLAGS += +r
    VMS_CR_DBD += sncSRSRGA.dbd
    VMS_CR_SRCS += sncSRSRGA.stt
    VMS_CR_SRCS += userclass.cpp
    VMS_CR_SRCS += subHealthState.cpp
    VMS_CR_SRCS += subIfDriver.cpp
    VMS_CR_LIBS += seq pv
#endif

VMS_CR_LIBS += $(EPICS_BASE_IOC_LIBS)

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

