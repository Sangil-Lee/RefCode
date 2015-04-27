TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

SDNLIB_HOME		= ../../sdn

USR_INCLUDES    += -I$(SDNLIB_HOME)/src/main/c++/include
USR_LDFLAGS     += -L$(SDNLIB_HOME)/target/lib
USR_SYS_LIBS	+= sdn rt pthread

#=============================
# Build the IOC application

PROD_IOC += test-cps-sub

#test-cps-sub_SRCS_DEFAULT += test-cps-sub.cpp
#test-cps-sub_SRCS += test-cps-sub.cpp
#test-my-sdnVBS_LIBS += $(EPICS_BASE_IOC_LIBS)

PROD_IOC += test-cps-pub

#test-cps-sub_SRCS += test-cps-sub.cpp
#test-cps-pf1-pcf1VBS_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

