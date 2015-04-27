
############################################################################
#
# Makefile
#
#
############################################################################

# Cross compile
# make build ARCH=ppc KBUILD_VERBOSE=1 CROSS_COMPILE=/opt/eldk/usr/bin/ppc-linux-


##############################################################
# enable for normal module compile...... ( not CROSS COMPILE )
##############################################################
CROSS_COMPILE =
LINUX_SRC = /lib/modules/$(shell uname -r)/source
LINUX_BUILD = /lib/modules/$(shell uname -r)/build

##########################################
# Only for DDS2 system (CROSS COMPILE )
# call :   "ppcmake"  (alias  'make ARCH=ppc CROSS_COMPILE=ppc-linux-')
#         or   make ARCH=ppc CROSS_COMPILE=ppc-linux-
##########################################
#LINUX_SRC = /opt/eldk/ppc_74xx/usr/src/cwcec-kernel-source-2.6.16.13

#############################################
# Only for PCS system Kernel-2.4.xx and something wrong....
##############################################
#cc -D__KERNEL__ -DMODULE -I/usr/src/linux-2.4.20-8.1.kstarcustom/include -c cltu_driver.c -o cltu_driver.o


TOP=../..

HERE = $(PWD)
PRJ_HOME = $(HERE)
LIB_DIR = $(PRJ_HOME)/lib/$(TARGET_PLATFORM)

MODULE_NAME = cltu_driver

MODULE_OBJS = $(MODULE_NAME).o


obj-m  := $(MODULE_OBJS)
$(MODULE_NAME)-objs := drv_xtu2.o cltu_main.o 

EXTRA_CFLAGS += -I$(src)/../src -I$(LINUX_SRC)/include/linux -I$(LINUX_SRC)/include/asm
KDIR   := $(LINUX_BUILD)
PWD    :=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) V=$(V) modules

clean:
	rm	-rf *.o \
		*.mod.* \
		.*.cmd \
		*.ko *~ modules.* Module.*


ensure_dir:
	@if test ! -d $(DIR) ; then \
	echo "Create" $(DIR);     \
	mkdir -p $(DIR); \
	fi
install: default
	cp  -f *.h $(TOP)/include/


