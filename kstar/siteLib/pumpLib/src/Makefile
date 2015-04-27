TOP = ../..
include $(TOP)/configure/CONFIG


devPumpCRP_DIR += $(EPICS_PATH)/siteLib/lib/$(T_A)
devPumpTMP_DIR += $(EPICS_PATH)/siteLIb/lib/$(T_A)

DBD += devPumpCRP.dbd
DBD += devPumpTMP.dbd
DBD += devPumpMBP.dbd

LIBRARY_IOC    += devPumpCRP
LIBRARY_IOC    += devPumpTMP
LIBRARY_IOC    += devPumpMBP

devPumpCRP_SRCS += devPumpCRP.c
devPumpTMP_SRCS += devPumpTMP.c
devPumpMBP_SRCS += devPumpMBP.c


include $(TOP)/configure/RULES

