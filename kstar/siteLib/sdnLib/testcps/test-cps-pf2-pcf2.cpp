//+======================================================================
// $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-sample/tags/CODAC-CORE-4.0.0/src/main/c++/test-cps-pf2-pcf2/test-cps-pf2-pcf2.cpp $
// $Id: test-cps-pf2-pcf2.cpp 34157 2013-02-15 15:51:55Z zagara $
//
// Project       : CODAC Core System
//
// Description   : test-cps-pf2-pcf2 program
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

/** For Publisher function  
 *  Create an object of type TOPICNAME_userdata
 *  Create an object of type TOPICNAME_metatdata
 *  Create a SdnPublisher object with TOPICNAME_userdata and TOPICNAME_metadata as a template argument 
 *  Call setPublisher to set communication parameter
 *  Initiate loop to send data 
 */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <string.h>

#include <sdn/sdnapi.h>

#include "sdn_topic_test_cps_pf2_qval.h"

using namespace std; 


int main(int argc, char **argv)
{
	long giPacketCount=10240; 
	int gbFillContents = 1; 
	int err_num=0; 
	SR_RET sr_ret;
	string if_name;

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

	/* Create a variable of type topic on which to publish*/
	test_cps_pf2_qval_metadata theMetaData;
	test_cps_pf2_qval_userdata theUserData;

	/* Create publisher object for the topic */
	SdnPublisher<test_cps_pf2_qval> thePublisher;

	/* Set publisher for the topic variable */
	sr_ret = thePublisher.setPublisher(theMetaData);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_ERR,"thePublisher.setPublisher() failed. sr_ret=%d\n", sr_ret);
		return (1) ; 
	}

	/* introduce delay so that other publihser can also be up */
	/* Needed for testing only */
	sleep(2);  

	/* Initiate publisher loop */
	long send_count = 0; 
	for (int i=0; i<giPacketCount; ++i)
	{
		/* make payload different in each packet (for data sanity check) */
		if(gbFillContents)
		{
			theUserData.converter3 = (unsigned char)(i);
			theUserData.xval = (unsigned char)(i+3);
		}

		/* Publish on the variable test_cps_pf2_qval */
		sr_ret = thePublisher.publish(theUserData);
		if(sr_ret != SR_OK)
			++err_num;

		++send_count;

		// print every 1024 count to see if it is running (it is helpful when large iterations)
		if((send_count & 0x3ff) == 0)
			SDNMSG(SM_LOG,"count = %d\n", send_count);

		/* for 1KHz update rate sleep here for 1000 usecs */
		usleep(1000); 
	}

	thePublisher.unsetPublisher();

	terminateSDN();
}

