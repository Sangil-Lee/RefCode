TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build the IOC application

PROD_IOC = MarteShm

# MarteShm.dbd will be created and installed
DBD += MarteShm.dbd

# MarteShm.dbd will be made up from these files:
MarteShm_DBD += base.dbd
MarteShm_DBD += MarteShmLib.dbd

# MarteShm_registerRecordDeviceDriver.cpp derives from MarteShm.dbd
MarteShm_SRCS += MarteShm_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
MarteShm_SRCS_DEFAULT += MarteShmMain.cpp
MarteShm_SRCS_vxWorks += -nil-

MarteShm_LIBS += MarteShmLib

# Finally link to the EPICS Base libraries
MarteShm_LIBS += $(EPICS_BASE_IOC_LIBS)

ifneq ($(SNCSEQ),)
	sncMarteShm_SNCFLAGS += +r
	MarteShm_DBD  += sncMarteShm.dbd
	MarteShm_SRCS += sncMarteShm.stt
	MarteShm_LIBS += seq pv
endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

