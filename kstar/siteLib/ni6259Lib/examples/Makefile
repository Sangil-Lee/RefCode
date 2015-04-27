#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/Makefile $
# $Id: Makefile 34216 2013-02-20 09:00:23Z cesnikt $
#
# Project       : CODAC Core System
#
# Description   : Makefile for examples
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

CFLAGS = -Wall -ggdb -I ../lib -I ../include -I /opt/codac/include
LDFLAGS =  -lm -lpxi6259 -L ../lib -lpthread -lrt

EXAMPLES =  sampleChannels \
			generateStaticSignal \
			generateWaveformOnOneChannel \
			generalPurposeCounterTest \
			testSampleRate \
			softwareTimedAcquisition \
			stressTest \
			continuousSample \
			testDIO \
			startStopDAQUsingTrigger \
			startAOUsingTrigger \
			sampleOnExternalTrigger \
			resetDevice \
			finiteSampling \
			finiteSamplingIterative\
			performance_ai_ao_loopback \
			testRead

EXCEPT_EXAMPLES = resetSegment \
			finiteSamplingExact 

all:	$(EXAMPLES)

%:	%.c
	gcc $(LDFLAGS) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(EXAMPLES)
        
install: all

