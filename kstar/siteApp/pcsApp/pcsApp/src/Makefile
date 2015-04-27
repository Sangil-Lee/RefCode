TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library


PROD_HOST += pcsGateway
pcsGateway_SNCFLAGS += +m
pcsGateway_SRCS += pcsGateway.st
pcsGateway_LIBS += seq pv
pcsGateway_LIBS += pcsMessages
pcsGateway_LIBS += $(EPICS_BASE_HOST_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

