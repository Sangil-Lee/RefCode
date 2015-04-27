TOP = ..
include $(TOP)/configure/CONFIG

ifeq ($(T_A),linux-ppc)
USR_INCLUDES    += -I/usr/local/mdsplus-ppc-4.0/include
USR_LDFLAGS     += -L/usr/local/mdsplus-ppc-4.0/lib
else
USR_INCLUDES	+= -I/usr/local/mdsplus/include
USR_LDFLAGS		+= -L/usr/local/mdsplus/lib
endif
USR_SYS_LIBS	+= TreeShr TdiShr MdsLib MdsShr MdsIpShr

INC				+= sfwTree.h
INC				+= sfwTreeObj.h
DBD				+= sfwTree.dbd

LIBRARY_IOC		+= sfwTree
sfwTree_SRCS	+= sfwTree.cpp
sfwTree_SRCS	+= sfwTreeObj.cpp
sfwTree_SRCS	+= sfwTreeArchiveThr.cpp
sfwTree_SRCS	+= sfwTreeEventThr.cpp

PROD_IOC			+= TestSfwTreeObj
TestSfwTreeObj_SRCS	+= TestSfwTreeObj.cpp
TestSfwTreeObj_LIBS	+= kutil
TestSfwTreeObj_LIBS	+= sfwTree

PROD_IOC			+= TestSfwTree
TestSfwTree_SRCS	+= TestSfwTree.c
TestSfwTree_LIBS	+= kutil
TestSfwTree_LIBS	+= sfwTree

# Finally link to the EPICS Base libraries
TestSfwTreeObj_LIBS	+= $(EPICS_BASE_IOC_LIBS)
TestSfwTree_LIBS	+= $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
