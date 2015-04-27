TOP = ../..
include $(TOP)/configure/CONFIG
#

USR_INCLUDES 	+= -I/usr/local/include
USR_INCLUDES	+= -I$(EPICS_PATH)/siteLib/include
USR_DBDFLAGS	+= -I$(EPICS_PATH)/siteLib/dbd

USR_LDFLAGS	+= -L/usr/local/lib -lnidaqmx
nidaqmx_DIRS	+= /usr/local/lib
#

LIBRARY_IOC += niPXISCXI

#
DBD 		+= drvPXISCXI.dbd
DBD 		+= devPXISCXI.dbd

#
niPXISCXI_SRCS 		+= drvPXISCXI.c
niPXISCXI_SRCS 		+= devPXISCXI.c
niPXISCXI_LIBS 		+= $(EPICS_BASE_IOC_LIB)


#
include $(TOP)/configure/RULES

