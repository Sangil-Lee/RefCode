TOP = ..
include $(TOP)/configure/CONFIG

PFEIFFERTPG262 = $(TOP)/pfeifferTPG262Lib

LIBRARY_IOC += pfeifferTPG262

SRC_DIRS += $(PFEIFFERTPG262)/drvTPG262
INC += drvTPG262.h
DBD += drvTPG262.dbd
pfeifferTPG262_SRCS += drvTPG262.c

SRC_DIRS += $(PFEIFFERTPG262)/devTPG262
DBD += devTPG262.dbd
pfeifferTPG262_SRCS += devTPG262.c


include $(TOP)/configure/RULES
