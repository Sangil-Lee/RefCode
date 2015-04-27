TOP = ../

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# purify support (uncomment to enable)
#CXX := purify $(CXX)

#  Profiling doesn't work for threaded applications?
#arrput_CXXFLAGS = -p
#arrputCA_CXXFLAGS = -p

MEMCACHED=/usr/local
#USR_INCLUDES += -I$(COGENT)/include

memcached_DIR += $(MEMCACHED)/lib
#iocsh_DIR += /usr/local/epics/base/lib/linux-x86/

USR_INCLUDES += -I/usr/local/epics/siteLib/include/
seq_DIR = /usr/local/epics/siteLib/lib/linux-x86/

#  Generate snc main programs
SNCFLAGS = +m

#  Products
#PROD_HOST = testCA pvtest arrput arrputCA
#PROD_HOST = kstarDataToMG mgDataToKSTAR
PROD_HOST = PV2MemCache

#  Libraries
#PROD_LIBS += seq cogent
#PROD_LIBS += seq memcached iocsh
PROD_LIBS += seq memcached


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

