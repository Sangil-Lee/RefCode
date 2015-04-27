//+======================================================================
// $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-sample/tags/CODAC-CORE-4.0.0/src/main/c++/test-cps-mcs-pcf3/test-cps-mcs-pcf3.cpp $
// $Id: test-cps-mcs-pcf3.cpp 34157 2013-02-15 15:51:55Z zagara $
//
// Project       : CODAC Core System
//
// Description   : test-cps-mcs-pcf3 program
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

/** For subscriber function  
 *  Create an object of type test_cps_pf1_userdata
 *  Create an object of type test_cps_pf1_metatdata
 *  Create a SdnPublisher object with test_cps_pf1_userdata and test_cps_pf1_metadata as a template argument 
 *  Call setSubscriber to set communication parameter
 *  Call setReceiveCallback to define functional handler for the data reception
 */

/** Repeat subscriber function for other topic - test_cps_pf2_userdata
 *
 */

/** For Publisher function  
 *  Create an object of type test_cps_rpc_userdata
 *  Create an object of type test_cps_rpc_metatdata
 *  Create a SdnPublisher object with test_cps_rpc_userdata and test_cps_rpc_metadata as a template argument 
 *  Call setPublisher to set communication parameter
 *  Initiate loop to send data 
 */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <semaphore.h>
#include <string.h>

#include <sdn/sdnapi.h>

#include "sdn_topic_test_cps_pf1_qval.h"
#include "sdn_topic_test_cps_pf2_qval.h"
#include "sdn_topic_test_cps_rpc_qval.h"

using namespace std; 


template <typename topic_userdata>
SR_RET pf1_qval_handler(topic_userdata &theTopicUserdata, SR_RET err_code); 

template <typename topic_userdata>
SR_RET pf2_qval_handler(topic_userdata &theTopicUserdata, SR_RET err_code); 

sem_t pf1_qval_sem, pf2_qval_sem; 


int main(int argc, char **argv)
{
	long giPacketCount=10240; 
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

	/* Initialize semaphore to synchronize reception and publishing */
	sem_init(&pf1_qval_sem, 0, 0); 
	sem_init(&pf2_qval_sem, 0, 0); 


	/* Create a variable of type test_cps_pf1_qval on which to subscribe*/
	test_cps_pf1_qval_metadata cps_pf1_qval_metadata;
	test_cps_pf1_qval_userdata cps_pf1_qval_userdata;

	/* Create subscriber object for the topic */
	SdnSubscriber<test_cps_pf1_qval> cps_pf1_qval_subscriber;

	/* Set subscriber for the topic test_cps_pf1_qval */
	sr_ret = cps_pf1_qval_subscriber.setSubscriber(cps_pf1_qval_metadata);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_ERR,"test_cps_pf1_qval_subscriber failed. sr_ret=%d\n", sr_ret);
		return (1) ; 
	}

	/* Define asynchronous callback handler */
	cps_pf1_qval_subscriber.setReceiveCallback(pf1_qval_handler); 

	/* Create a variable of type test_cps_pf2_qval on which to subscribe*/
	test_cps_pf2_qval_metadata cps_pf2_qval_metadata;
	test_cps_pf2_qval_userdata cps_pf2_qval_userdata;

	/* Create subscriber object for the topic */
	SdnSubscriber<test_cps_pf2_qval> cps_pf2_qval_subscriber;

	/* Set subscriber for the topic test_cps_pf2_qval*/
	sr_ret = cps_pf2_qval_subscriber.setSubscriber(cps_pf2_qval_metadata);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_ERR,"test_cps_pf2_qval_subscriber failed. sr_ret=%d\n", sr_ret);
		return (1) ; 
	}

	/* Define asynchronous callback handler */
	cps_pf2_qval_subscriber.setReceiveCallback(pf2_qval_handler); 

	/* Create variable for publishing */
	test_cps_rpc_qval_metadata cps_rpc_qval_metadata;
	test_cps_rpc_qval_userdata cps_rpc_qval_userdata;

	/* Create publisher object for the topic test_cps_prc_qval*/
	SdnPublisher<test_cps_rpc_qval> cps_rpc_qval_publisher;

	/* Set publisher for the topic variable */
	sr_ret = cps_rpc_qval_publisher.setPublisher(cps_rpc_qval_metadata);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_ERR,"cps.rpc.qval_publisher.setPublisher() failed. sr_ret=%d\n", sr_ret);
		return (1) ;
	}

	/* Initiate publisher loop */
	long send_count =0;
	for (int i=0; i<giPacketCount; ++i)
	{
		if ((sem_wait(&pf1_qval_sem)==0) && (sem_wait(&pf2_qval_sem)==0))
		{
			/* data from both pf1 and pf2 are available */
			/* read data */
			cps_pf1_qval_subscriber.getAsyncUserData(cps_pf1_qval_userdata); 
			cps_pf2_qval_subscriber.getAsyncUserData(cps_pf2_qval_userdata); 
			
			/* perform computation and publish the result as rpc_qval */
			cps_rpc_qval_userdata.converter1_delta= cps_pf1_qval_userdata.converter1 * cps_pf2_qval_userdata.xval; 
			cps_rpc_qval_userdata.converter2_delta= cps_pf1_qval_userdata.converter2 * cps_pf2_qval_userdata.xval; 
			cps_rpc_qval_userdata.firing_angle =    cps_pf2_qval_userdata.converter3 * cps_pf2_qval_userdata.xval; 

			/* Publish on the variable test_cps_rpc_qval */
			sr_ret = cps_rpc_qval_publisher.publish(cps_rpc_qval_userdata);
			if(sr_ret != SR_OK)
				++err_num;
		}

		++send_count;

		// print every 1024 count to see if it is running (it is helpful when large iterations)
		if((send_count & 0x3ff) == 0)
			SDNMSG(SM_LOG,"count = %d\n", send_count);
	}

	cps_rpc_qval_publisher.unsetPublisher();
	cps_pf1_qval_subscriber.unsetSubscriber();
	cps_pf2_qval_subscriber.unsetSubscriber();

	terminateSDN();
}


template <typename topic_userdata>
SR_RET pf1_qval_handler(topic_userdata &theTopicrUserdata, SR_RET err_code) 
{
	sem_post(&pf1_qval_sem); 
	return SR_OK; 
}

template <typename topic_userdata>
SR_RET pf2_qval_handler(topic_userdata &theTopicrUserdata, SR_RET err_code) 
{
	sem_post(&pf2_qval_sem); 
	return SR_OK; 
}

