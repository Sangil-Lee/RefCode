TOP = ..
include $(TOP)/configure/CONFIG

DBD += devAgilent6652A.dbd
AGILENT6652A = $(TOP)/agilent6652ALib

LIBRARY_IOC += agilent6652A
agilent6652A_LIBS += $(EPICS_BASE_IOC_LIBS)

SRC_DIRS += $(AGILENT6652A)/devAgilent6652A
agilent6652A_SRCS += devAgilent6652A.c

include $(TOP)/configure/RULES

