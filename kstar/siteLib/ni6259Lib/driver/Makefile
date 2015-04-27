#+======================================================================
#
# The PXI-6259 driver.
# Author: Janez Golob (Cosylab)
# Copyright (C) 2010-2013 ITER Organization
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file COPYING in the main directory of the Linux
# distribution for more details.
#
#+======================================================================

namebase := pxi6259

# Only build for current kernel
kernel_this := $(shell uname -r)
#kernel_list := $(shell rpm -qa kernel-devel kernel-rt-devel | xargs rpm -q --qf "%{VERSION}-%{RELEASE}.%{ARCH}\n")
kernel_list := $(shell uname -r)
current_dir := $(shell pwd)
#target_dir := $(current_dir)/../../..
target_dir := $(current_dir)

headers := $(shell ls ../include/*.h)


.PHONY: all clean test

all: $(kernel_list)

$(kernel_list):
	@echo "Building for $@"
	-@ln -s -f $(headers) .
	make -C /lib/modules/$@/build M=$(current_dir) modules
	@mkdir -p $(target_dir)/bin/$@/
	@cp $(namebase).ko $(target_dir)/bin/$@
	@rm -f $(notdir $(headers))
	 
clean:
	@rm -f *.o *.ko *~ Module.* *.mod.c .*.cmd
	@rm -rf .tmp_versions

install: all

module_install:
	cp $(namebase).ko /lib/modules/$(kernel_this)/extra
