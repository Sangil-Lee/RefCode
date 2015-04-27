#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/examples/Makefile $
# $Id: Makefile 34216 2013-02-20 09:00:23Z cesnikt $
#
# Project       : CODAC Core System
#
# Description   : SDN Software - Communication API Makefile.
#
# Author        : Eui Jae LEE, Hyung Gi KIM (Mobiis Co., Ltd)
#
# Co-Author     : Mahajan Kirti
#
# Copyright (c) : 2010-2013 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
#
# This file is part of ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file ITER-LICENSE.TXT located in the top level directory
# of the distribution package.
#
#-======================================================================

CFLAGS = -Wall -O3 -Os -I ../include 

LDFLAGS = ../../../../target/lib/libsdn.a -lrt -lpthread
#LDFLAGS = -L ../../../../target/lib -lsdn -lrt -lpthread

TARGET = ../../../../target/main/c++/examples
TEST_DIR = ../../../../target/test

EXAMPLES = sdn-pub sdn-sub
	 

all:
	@mkdir -p $(TARGET)
	@for ii in $(EXAMPLES); do \
		g++ -o $(TARGET)/$$ii $$ii.cpp $(LDFLAGS) $(CFLAGS); \
	done

	@cp $(TARGET)/* $(TEST_DIR)/

clean:
	@rm -rf $(TARGET) 
