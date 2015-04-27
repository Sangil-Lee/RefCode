TOP = ../

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# purify support (uncomment to enable)
#CXX := purify $(CXX)

#  Profiling doesn't work for threaded applications?
#arrput_CXXFLAGS = -p
#arrputCA_CXXFLAGS = -p

#COGENT=/usr/cogent
#USR_INCLUDES += -I$(COGENT)/include

#cogent_DIR += $(COGENT)/lib

#  Generate snc main programs
SNCFLAGS = +m

#  Products
PROD_HOST = GWInnerPVs

#kstarDataToMG_SRCS += kstarinterface.cpp

#  Libraries
PROD_LIBS += seq
seq_DIR    = $(INSTALL_LIB)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

