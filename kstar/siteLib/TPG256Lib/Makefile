TOP=..
include $(TOP)/configure/CONFIG
#=======================================



# Build devTPG256 as a library for an IOC:
LIBRARY_IOC += devTPG256

# Library Source files
devTPG256_SRCS += devTPG256.c

# Link with the asyn and base libraries
#devTPG256_LIBS += asyn
#devTPG256_LIBS += $(EPICS_BASE_IOC_LIBS)

# Install .dbd and .db files
DBD += devTPG256.dbd

#=======================================
include $(TOP)/configure/RULES
