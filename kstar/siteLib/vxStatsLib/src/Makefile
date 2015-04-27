TOP = ../..

include $(TOP)/configure/CONFIG

LIBRARY_IOC_vxWorks += vxStats

#DBDINC += 
DBD += devStats.dbd

vxStats_SRCS += devVxStats.c
vxStats_SRCS += read_ca_stats.c
vxStats_SRCS += BSlib.c
vxStats_SRCS += vx_stats.c
vxStats_SRCS += PVSvx.c
vxStats_SRCS += reboot.c
vxStats_SRCS += tsSubr.o

include $(TOP)/configure/RULES

