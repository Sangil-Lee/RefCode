//+======================================================================
// $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-sample/tags/CODAC-CORE-4.0.0/src/main/c++/test-cps-rpc-pcf4/test-cps-rpc-pcf4.cpp $
// $Id: test-cps-rpc-pcf4.cpp 34157 2013-02-15 15:51:55Z zagara $
//
// Project       : CODAC Core System
//
// Description   : test-cps-rpc-pcf4 program
//
// Author        :
//
// Copyright (c) : 2010-2013 ITER Organization,
//                 CS 90 046
//                 13067 St. Paul-lez-Durance Cedex
//                 France
//
// This file is part of ITER CODAC software.
// For the terms and conditions of redistribution or use of this software
// refer to the file ITER-LICENSE.TXT located in the top level directory
// of the distribution package.
//
//-======================================================================

/** Subscription function test code 
 * Create an object of type test_cps_rpc_qval 
 * Create a SdnSubscriber object with test_cps_rpc_qval as a template argument 
 * Call setSubscriber to set communication parameter
 * Initiate loop to receive data
 */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <semaphore.h>
#include <string.h>

#include <sdn/sdnapi.h>

#include "sdn_topic_test_cps_rpc_qval.h"

using namespace std; 


int main(int argc , char **argv)
{
	long giPacketCount=10240;
	SR_RET sr_ret;
	string if_name;
	short int value1; 
	short int value2; 

	if(argc < 2)
	{
		if_name = getenv("SDN_INTERFACE_NAME") ? getenv("SDN_INTERFACE_NAME") : "";
		if(strlen(if_name.c_str()) == 0)
			if_name = "eth-sdn";
	}
	else
		if_name = argv[1];
	
	sr_ret = initializeSDN((char*)if_name.c_str());
	if(sr_ret != SR_OK) 
	{
		printf("initializeSDN() for [%s] failed.\n",  (char*)if_name.c_str());
		return (1);
	}

	/* create a variable of type topic for subscription */
	test_cps_rpc_qval_metadata theMetaData;
	test_cps_rpc_qval_userdata theUserData;

	/* print topic metadata */
	printTopicInformation(theMetaData, theUserData);

	/* Create subscriber object for the topic */
	SdnSubscriber<test_cps_rpc_qval> theSubscriber;

	/*Set subscriber for the topic variable */
	sr_ret = theSubscriber.setSubscriber(theMetaData);


	/* Initiate receive loop */
	long recv_count =0; 
	for (int i=0; i<giPacketCount; ++i)
	{
		/* Receive topic data */
		theSubscriber.receive(theUserData);

		/* process received qvalue */
		value1 = theUserData.converter1_delta; 
		value2 = theUserData.converter2_delta;

		++recv_count; 
		if((recv_count & 0x3ff) == 0)
			SDNMSG(SM_LOG,"count = %d\n", recv_count);
	}

	theSubscriber.unsetSubscriber();

	terminateSDN();
}

