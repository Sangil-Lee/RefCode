TOP=../..

include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

ifeq ($(OS_CLASS),WIN32)
	USR_INCLUDES    += -I$(EPICS_PATH)/mdsplus/include
	USR_LDFLAGS     += /LIBPATH:$(EPICS_PATH)/mdsplus/lib
	USR_LDFLAGS     += TreeShr.lib TdiShr.lib MdsLib.lib MdsShr.lib

	USR_LDFLAGS     += /LIBPATH:$(EPICS_BASE)/lib/win32-x86
	USR_LDFLAGS     += recIoc.lib softDevIoc.lib miscIoc.lib rsrvIoc.lib dbtoolsIoc.lib
	USR_LDFLAGS     += asIoc.lib dbIoc.lib registryIoc.lib dbStaticIoc.lib ca.lib Com.lib 

	USR_CFLAGS	+= -DWIN32
	USR_CXXFLAGS	+= -DWIN32
else
ifeq ($(T_A),linux-ppc)
        USR_LDFLAGS     += -L/usr/local/mdsplus-ppc-4.0/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lMdsIpShr -lc
        USR_INCLUDES    += -I/usr/local/mdsplus-ppc-4.0/include
else
	USR_INCLUDES    += -I/usr/local/mdsplus/include
        USR_LDFLAGS     += -L/usr/local/mdsplus/lib -lTreeShr -lTdiShr -lMdsLib -lMdsShr -lc
endif
endif

#==================================================
# Build an IOC support library

LIBRARY_IOC += sfwLib

DBD += sfwDBD.dbd
INC += sfwCommon.h
INC += sfwDriver.h
INC += sfwGlobalDef.h
INC += sfwInitControl.h
INC += sfwMDSplus.h

sfwLib_SRCS += sfwSequence.c
sfwLib_SRCS += sfwCommon.c
sfwLib_SRCS += sfwDevice.c
sfwLib_SRCS += sfwDriver.c
sfwLib_SRCS += sfwMDSplus.c

#=============================
# build an ioc application

#===========================

include $(TOP)/configure/RULES

