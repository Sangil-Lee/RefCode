TOP=../..

include $(TOP)/configure/CONFIG


USR_CFLAGS += -D_CLTU_LINUX_
USR_CFLAGS += -I../../driver 
#USR_CFLAGS += -D_CLTU_VXWORKS_

#==================================================
# Build an IOC support library

LIBRARY_IOC += cltuDev

DBD += cltuBase.dbd

cltuDev_SRCS += devCLTU.c
cltuDev_SRCS += drvCLTU.c
cltuDev_SRCS += oz_cltu.c
cltuDev_SRCS += oz_xtu2.c

#=============================
# build an ioc application

#===========================

include $(TOP)/configure/RULES


