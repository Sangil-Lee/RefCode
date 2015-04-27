TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES        += -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS        += -I$(EPICS_PATH)/siteLib/dbd

cltuDev_DIR 	    += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR          += $(EPICS_PATH)/siteLib/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_PATH)/siteLib/lib/$(T_A)

#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = IRVB_LTU
# IRVB_LTU.dbd will be created and installed
DBD += IRVB_LTU.dbd

IRVB_LTU_DBD += base.dbd
IRVB_LTU_DBD += cltuBase.dbd
IRVB_LTU_DBD += timestampRecord.dbd
IRVB_LTU_DBD += sysMon.dbd
IRVB_LTU_DBD += subIfDriver.dbd
IRVB_LTU_DBD += subHealthState.dbd
IRVB_LTU_DBD += pvCrawler.dbd

# IRVB_LTU_registerRecordDeviceDriver.cpp derives from IRVB_LTU.dbd
IRVB_LTU_SRCS += IRVB_LTU_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
IRVB_LTU_SRCS_DEFAULT += IRVB_LTUMain.cpp
IRVB_LTU_SRCS_vxWorks += -nil-

# Add locally compiled object code
IRVB_LTU_SRCS += subIfDriver.cpp
IRVB_LTU_SRCS += subHealthState.cpp
IRVB_LTU_SRCS += userclass.cpp

# Add support from base/src/vxWorks if needed
#IRVB_LTU_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
IRVB_LTU_LIBS += cltuDev
IRVB_LTU_LIBS += sysMon
IRVB_LTU_LIBS += recTimeStamp

# Finally link to the EPICS Base libraries
IRVB_LTU_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

