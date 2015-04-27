TOP = ..
include $(TOP)/configure/CONFIG

USR_INCLUDES += -I/usr/local/mdsplus/include

LIBRARY_IOC += kupa

INC  += kupaLib.h
INC  += kupaMDS.h
DBD  += kupaLib.dbd

kupa_SRCS += kupaLib.cpp
kupa_SRCS += kupaManager.cpp
kupa_SRCS += kupaPV.cpp
kupa_SRCS += kupaCollectPV.cpp
kupa_SRCS += kupaCollectStream.cpp
kupa_SRCS += kupaExportCSV.cpp
kupa_SRCS += kupaExportFile.cpp
kupa_SRCS += kupaExportMDS.cpp
kupa_SRCS += kupaCA.cpp
kupa_SRCS += kupaDB.c
kupa_SRCS += kupaMDS.c

include $(TOP)/configure/RULES
