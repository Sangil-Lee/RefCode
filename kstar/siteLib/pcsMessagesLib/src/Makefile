TOP = ../..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -DLINUXOS

USR_LDFLAGS += -L../../../lib/$(T_A)


LIBRARY_IOC_Linux += pcsMessages

#DBDINC += 
#DBD += devStats.dbd

pcsMessages_SRCS += socketbasic.c
pcsMessages_SRCS += socketcom.c
pcsMessages_SRCS += swap_functions.c
pcsMessages_SRCS += readwriteit.c
pcsMessages_SRCS += MemDebug.c
pcsMessages_SRCS += get_processor.c


include $(TOP)/configure/RULES

