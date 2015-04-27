#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/Makefile $
# $Id: Makefile 34216 2013-02-20 09:00:23Z cesnikt $
#
# Project       : CODAC Core System
#
# Description   : Makefile
#
# Author        : Cosylab
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

SUBDIRS := \
	driver \
	lib \
	examples

%:
	@set -e; for ii in $(SUBDIRS); do \
		$(MAKE) $@ -C $$ii; \
	done

all:

