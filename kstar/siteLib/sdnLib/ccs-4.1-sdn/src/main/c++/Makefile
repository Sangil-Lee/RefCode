#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-iter-units-api/tags/CODAC-CORE-4.0.0/src/main/resources/newprog/TopMakefile $
# $Id: TopMakefile 34157 2013-02-15 15:51:55Z zagara $
#
# Project       : CODAC Core System
#
# Description   : C++ Makefile
#
# Author        : Anze Zagar, Cosylab
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

#SUBDIRS=$(dir $(wildcard */Makefile))
SUBDIRS=sdn-api sdn-monitor examples

all:

%:
	$(foreach dir, $(SUBDIRS), $(MAKE) -C $(dir) $@;)
