TOP=..

include $(TOP)/configure/CONFIG

# library

LIBRARY_IOC = MarteShmLib

INC  += drvMarteShm.h
INC  += MarteShmUtil.h

SRCS += drvMarteShm.c
SRCS += devMarteShm.c
SRCS += MarteShmUtil.cpp

DBD  += MarteShmLib.dbd
DBD  += MarteShmUtil.dbd

include $(TOP)/configure/RULES
