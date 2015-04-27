TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE


#USR_LDFLAGS += -L/usr/local/lib -lnidaqmx
#USR_LDFLAGS += -lnidaqmx
#USR_INCLUDES += -I/usr/local/include
#USR_INCLUDES += -I/usr/local/mdsplus/include
#USR_INCLUDES += -I/usr/local/epics/siteLib/include
#USR_LDFLAGS += -L/usr/local/mdsplus/lib -lTreeShr -lMdsLib -lMdsShr -lc -lpopt
#nidaqmx_DIRS = /usr/local/lib

#USR_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h:58 : error: conflicting types for 'int32'
#USR_CFLAGS  += -D_NI_int32_DEFINED_



USR_INCLUDES    	+= -I/usr/local/mdsplus/include
USR_INCLUDES    	+= -I../include
USR_LDFLAGS     	+= -L/usr/local/lib -lnidaqmx
USR_LDFLAGS			+= -lnidaqmx
#USR_SYS_LIBS 		+= -L/usr/local/natinst/nidaqmx
USR_LDFLAGS     	+= -L/usr/local/mdsplus/lib
#USR_SYS_LIBS		+= TreeShr TdiShr MdsLib MdsIpShr MdsShr MdsObjectsCppShr 
USR_SYS_LIBS		+= TreeShr TdiShr MdsLib MdsShr c popt

USR_CFLAGS			+= -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DDEBUG
#/usr/local/include/NIDAQmx.h 58 : error: conflicting types for 'int32'
USR_CFLAGS			+= -D_NI_int32_DEFINED_

USR_INCLUDES        += -I/usr/local/include
USR_INCLUDES        += -I$(EPICS_SITE)/include
USR_DBDFLAGS        += -I$(EPICS_SITE)/dbd

cltuDev_DIR 	    += $(EPICS_SITE)/lib/$(T_A)
sfwLib_DIR 	    	+= $(EPICS_SITE)/lib/$(T_A)
recTimeStamp_DIR    += $(EPICS_SITE)/lib/$(T_A)
sysMon_DIR          += $(EPICS_SITE)/lib/$(T_A)

#=============================
# Build the IOC support library

#=============================
# Build the IOC application

PROD_IOC = EC1_D1_DAQ
# EC1_D1_DAQ.dbd will be created and installed
DBD += EC1_D1_DAQ.dbd

EC1_D1_DAQ_DBD += base.dbd
EC1_D1_DAQ_DBD += drvNI6220.dbd
EC1_D1_DAQ_DBD += devNI6220.dbd
EC1_D1_DAQ_DBD += statCommon.dbd

EC1_D1_DAQ_DBD += kutil.dbd
EC1_D1_DAQ_DBD += sfwDBD.dbd
EC1_D1_DAQ_DBD += timestampRecord.dbd
EC1_D1_DAQ_DBD += sysMon.dbd
EC1_D1_DAQ_DBD += subIfDriver.dbd
EC1_D1_DAQ_DBD += subHealthState.dbd
EC1_D1_DAQ_DBD += pvCrawler.dbd
EC1_D1_DAQ_DBD += cltuBase.dbd

# EC1_D1_DAQ_registerRecordDeviceDriver.cpp derives from EC1_D1_DAQ.dbd
EC1_D1_DAQ_SRCS += EC1_D1_DAQ_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
EC1_D1_DAQ_SRCS_DEFAULT += EC1_D1_DAQMain.cpp

# Add locally compiled object code
EC1_D1_DAQ_SRCS += drvNI6220.c
EC1_D1_DAQ_SRCS += devNI6220.c
EC1_D1_DAQ_SRCS += statCommon.c
EC1_D1_DAQ_SRCS += myMDSplus.c

EC1_D1_DAQ_SRCS += subIfDriver.cpp
EC1_D1_DAQ_SRCS += subHealthState.cpp
EC1_D1_DAQ_SRCS += userclass.cpp

# Link in the code from the support library
EC1_D1_DAQ_LIBS += recTimeStamp
EC1_D1_DAQ_LIBS += sysMon
EC1_D1_DAQ_LIBS += sfwLib
EC1_D1_DAQ_LIBS += cltuDev
EC1_D1_DAQ_LIBS += kutil

# Finally link to the EPICS Base libraries
EC1_D1_DAQ_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

