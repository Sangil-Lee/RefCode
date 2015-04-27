#--------------------------------------------------------------
# Tempral variables, should be removed after fix base/configure
# May 10, 2007 -khkim-
#--------------------------------------------------------------
FC = g77
FC_OPTIONS = -O3
#--------------------------------------------------------------

TOP = ../..
include $(TOP)/configure/CONFIG

g2c_DIR += /usr/local/lib
g2c_DIR += /usr/lib

heProp_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)

DBD += heProp.dbd

LIBRARY_IOC_solaris    += heProp
LIBRARY_IOC_Linux      += heProp 

heProp_SRCS += dbSubHep.c
heProp_SRCS += Heprop.f
heProp_LIBS += g2c


include $(TOP)/configure/RULES

