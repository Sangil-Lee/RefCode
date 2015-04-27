TOP = ..
include $(TOP)/configure/CONFIG

VGCGP307 =$(TOP)/vgcGP307Lib

LIBRARY_IOC += vgcGP307

SRC_DIRS += $(VGCGP307)/drvGP307
INC += drvGP307.h
DBD += drvGP307.dbd
vgcGP307_SRCS += drvGP307.c


SRC_DIRS += $(VGCGP307)/devGP307
DBD += devGP307.dbd
vgcGP307_SRCS += devGP307.c


include $(TOP)/configure/RULES

