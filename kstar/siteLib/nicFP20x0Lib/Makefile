TOP = ..
include $(TOP)/configure/CONFIG

NICFP20X0 = $(TOP)/nicFP20x0Lib

LIBRARY_IOC += nicFP20x0

SRC_DIRS += $(NICFP20X0)/drvnicFP20x0
INC += drvnicFP20x0.h
DBD += drvnicFP20x0.dbd
nicFP20x0_SRCS += drvnicFP20x0.c
SRC_DIRS += $(NICFP20X0)/devnicFP20x0
DBD += devnicFP20x0.dbd
nicFP20x0_SRCS += devnicFP20x0.c

SRC_DIRS += $(NICFP20X0)/devnicFP20x0

include $(TOP)/configure/RULES
