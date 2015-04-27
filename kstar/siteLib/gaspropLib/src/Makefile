TOP = ../..
include $(TOP)/configure/CONFIG

g2c_DIR += /usr/local/lib
g2c_DIR += /usr/lib

gasprop_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)

DBD += dbSubGasp.dbd

LIBRARY_IOC_solaris    += dbSubGasp
LIBRARY_IOC_Linux      += dbSubGasp

dbSubGasp_SRCS += dbSubGasp.c
dbSubGasp_LIBS += gasprop
dbSubGasp_LIBS += g2c


include $(TOP)/configure/RULES

