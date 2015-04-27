#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/src/Makefile $
# $Id: Makefile 33920 2013-02-05 15:34:20Z kimh12 $
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

CPP =	g++
CFLAGS += -g -fPIC -I../include/sdn_common -I../include/sdn 
LDFLAGS += -lrt -lpthread 

LIBNAME = sdn

TARGET_PATH = ../../../../target/lib
OBJDIR := $(TARGET_PATH)/obj

SRCS = sdn_node_info.cpp
HDRS = sdn_packet.h sdn_publisher.h sdn_subscriber.h sdn_node_info.h 
HDRS_CMN = log_buffer.h sdnmon_msg.h sdnmon_msg_reg.h 
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

all: staticlib dynamiclib

staticlib: $(OBJS)
	@mkdir -p $(TARGET_PATH)
	$(AR) $(ARFLAGS) $(TARGET_PATH)/lib$(LIBNAME).a $^

dynamiclib: $(OBJS)
	@mkdir -p $(TARGET_PATH)
	$(CPP) -shared $(LDFLAGS) -o $(TARGET_PATH)/lib$(LIBNAME).so $^

$(OBJS): $(OBJDIR)/%.o:%.cpp $(HDRS:%=../include/sdn/%) $(HDRS_CMN:%=../include/sdn_common/%)
	@mkdir -p $(OBJDIR)
	$(CPP) -c $(CFLAGS) -o $@ $<

clean:
	@rm -rf $(TARGET_PATH) 
