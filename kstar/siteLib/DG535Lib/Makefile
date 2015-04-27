TOP=..
include $(TOP)/configure/CONFIG
#=======================================



# Build devDG535 as a library for an IOC:
LIBRARY_IOC += devDG535

# Library Source files
devDG535_SRCS += devDG535.c

# Link with the asyn and base libraries
#devDG535_LIBS += asyn
#devDG535_LIBS += $(EPICS_BASE_IOC_LIBS)

# Install .dbd and .db files
DBD += devDG535.dbd

#=======================================
include $(TOP)/configure/RULES
