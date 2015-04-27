/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/examples/sdn-sub.cpp $
* $Id: sdn-sub.cpp 36365 2013-04-30 13:18:59Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API Example sdn-sub source file.
*
* Author        : Eui Jae LEE, Hyung Gi KIM (Mobiis Co., Ltd)
*
* Copyright (c) : 2010-2013 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

 /** SdnSubscriber function test code 
  *  Create an object of type TOPICNAME_userdata
  *  Create an object of type TOPICNAME_metatdata
  *  Create a SdnSubscriber object with TOPICNAME_userdata and TOPICNAME_metadata as a template argument 
  *  Call setSubscriber to set communication parameter
  *  Initiate loop to receive data 
  */
  
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <semaphore.h>

#include "sdn/sdnapi.h"

#include "sdn_topic_topic1_test.h"
#include "sdn_topic_topic1_test_modified.h"
#include "sdn_topic_topic2_test.h"
#include "sdn_topic_topic3_test.h"
#include "sdn_topic_topic1l_test.h"
#include "sdn_topic_topic2l_test.h"
#include "sdn_topic_topic3l_test.h"
#include "sdn_topic_topic1e_test.h"
#include "sdn_topic_topic2e_test.h"
#include "sdn_topic_topic3e_test.h"

using namespace std; 

#define	IMPROVE_ASYNC_PERFORMANCE	1

// parameter '-m'(test mode)
#define	TEST_MODE_NORMAL				0
#define	TEST_MODE_LOOPBACK_PERFORMANCE	1

#define	MAX_WAIT_PACKET	10

static int giTestMode = 0;
static int giPacketCount = 100;
static int giAsyncMode = 0;
static int gbEnableTimeCheck = 0;
static int giDelayThreshold = 0;
static int giTimeoutUsec = 0;
static int gbEnablePayloadCheck = 1;
static int gbEnableLoopback = 0;
static int giWaitDelay = 0;
static int giWaitPacket[MAX_WAIT_PACKET+1] = {-1,};
static char* gpDefConfigFile = NULL;
static char* gpUserConfigFile = NULL;

// delay statistics for asynchronous packets
static int giDelayedPackets1;
static int giMaxDelay1;
static int giMinDelay1;
static int giSumDelay1;
static int giDelayedPackets2;
static int giMaxDelay2;
static int giMinDelay2;
static int giSumDelay2;
static int giDelayedPackets3;
static int giMaxDelay3;
static int giMinDelay3;
static int giSumDelay3;
static int giDelayedPackets1e;
static int giMaxDelay1e;
static int giMinDelay1e;
static int giSumDelay1e;
static int giDelayedPackets2e;
static int giMaxDelay2e;
static int giMinDelay2e;
static int giSumDelay2e;
static int giDelayedPackets3e;
static int giMaxDelay3e;
static int giMinDelay3e;
static int giSumDelay3e;

static sem_t sem1, sem2, sem3, sem1e, sem2e, sem3e, sem_perf_test;

static pthread_mutex_t g_mutexSdnStatus; 
static pthread_cond_t g_condSdnStatus;	

void* subscriber_thread (void* param);
void* loopback_thread (void* param);
int receiveAsynchronous (int b_topic1, int b_topic2, int b_topic3, int b_topic1e, int b_topic2e, int b_topic3e);

void CallbackSDNStatus(int is_ready);

template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceive1(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceive2(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceive3(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceive1e(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceive2e(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceive3e(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);

template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceivePerfTest1(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceivePerfTest2(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
template <typename topic_metadata, typename topic_userdata>
	SR_RET CallbackAsyncReceivePerfTest3(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);


static struct option options[] = {
	{"count", 1, 0, 'c'},
	{"async", 1, 0, 'a'},
	{"timeout", 1, 0, 'o'},
	{"timecheck", 1, 0, 't'},
	{"payloadcheck", 1, 0, 'p'},
	{"usec_for_wait", 1, 0, 'u'},
	{"wait", 1, 0, 'w'},
	{"loglevel", 1, 0, 'l'},
	{"loopback", 1, 0, 'b'},
	{"register", 1, 0, 'r'},
	{"def_conf_file", 1, 0, 'x'},
	{"user_conf_file", 1, 0, 'y'},
	{"mode", 1, 0, 'm'},
	{"help", 0, 0, '?'},
	{"", 0, 0, 0}
};


static void usage(void)
{
	printf("Usage: sdn-sub [options] <if-name> topic1(e)_test [topic2(e)_test topic3(e)_test]\n");
	printf(" * event topic: topic1e_test topic2e_test topic3e_test]\n");

	printf("  -c, --count             receive packet count\n");
	printf("  -a, --async             receive data in asynchronous mode(1) or synchronous mode(0:default). \n");
	printf("  -o, --timeout           set timeout in micro-second for synchronous receive data\n");
	printf("  -t, --timecheck         check packet delay which is more than the argument value (usec)\n");
	printf("  -p, --payloadcheck      check payload contents according to the pre-defined rule\n");
	printf("  -u, --usec_for_wait     delay in usec for wait\n");
	printf("  -w, --wait              wait before receive() for specified iteration count\n");
	printf("  -l, --loglevel          log level. 1(all) ~ 5\n");
	printf("  -b, --loopback          enable(1)/disable(0) multicast loopback (default:0).\n");
	printf("  -r, --register          enable(1)/disable(0) registering to SDN monitor node(default:1)\n");
	printf("  -x, --def_conf_file     default config filename\n");
	printf("  -y, --user_conf_file    user config filename\n");
	printf("  -m, --mode              test mode(default:0)\n");
	printf("                            - 0: normal test. all options are available\n");
	printf("                            - 1: loopback test. available options(a:c:l).\n");
	printf("                                 parameter: topic[1,2,3]_test,topic[1,2,3]l_test\n");
	printf("                                 * topic1_test,topic1l_test: userdata size is 1 byte\n");
	printf("                                 * topic2_test,topic2l_test: userdata size is 1K bytes\n");
	printf("                                 * topic3_test,topic3l_test: userdata size is 8K bytes\n");
}



int main(int argc , char** argv)
{
	int topic_count = 0;
	int wait_packet_index = 0;
	SR_RET ret;
	
	if ( argc > 1 )
	{
		int opt_index = 0;
		int opt = 0;
		
		while ((opt = getopt_long(argc, argv, "c:a:o:t:p:u:w:l:b:r:x:y:m:?", options, &opt_index)) != -1)
		{
			switch(opt)
			{
			case 'c':
				giPacketCount = atoi(optarg);
				break;
				
			case 'a':
				giAsyncMode = atoi(optarg);
				break;
				
			case 'o':
				giTimeoutUsec = atoi(optarg);
				break;
			
			case 't':
				giDelayThreshold = atoi(optarg);
				gbEnableTimeCheck = 1;
				break;

			case 'p':
				gbEnablePayloadCheck = atoi(optarg);
				break;

			case 'u':
				giWaitDelay = atoi(optarg);
				break;

			case 'w':
				if(wait_packet_index >= MAX_WAIT_PACKET)
				{
					printf("maximum wait count is %d\n", MAX_WAIT_PACKET);
					exit(1);
				}

				giWaitPacket[wait_packet_index++] = atoi(optarg);
				break;
				

			case 'l':
				setSDNLogLevel(atoi(optarg));
				break;
				
			case 'b':
				gbEnableLoopback = atoi(optarg);
				break;
			
			case 'r':
				if(atoi(optarg) == 0)
					setenv("SDN_MONITORING_NODE", "", 1);
				break;
			
			case 'x':
				gpDefConfigFile = optarg;
				break;
			
			case 'y':
				gpUserConfigFile = optarg;
				break;
			
			case 'm':
				giTestMode = atoi(optarg);
				break;
			
			case '?':
			default:
				usage();
				exit(0);
			}
		}
	}
	else
	{
		usage();
		exit(0);
	}

	if((gpDefConfigFile != NULL) || (gpUserConfigFile != NULL))
	{
		ret = initializeSDN(gpDefConfigFile, gpUserConfigFile);
		if(ret != SR_OK)
		{
			printf("initializeSDN() for [%s][%s] failed.\n", 
				gpDefConfigFile ? gpDefConfigFile : "null",	gpUserConfigFile ? gpUserConfigFile : "null");
			exit(1);
		}
	}
	else
	{
		if(optind >= argc)
		{
			usage();
			exit(0);
		}
		
		ret = initializeSDN(argv[optind++]);
		if(ret != SR_OK)
		{
			printf("initializeSDN() for [%s] failed.\n", (optind > 0) ? argv[optind-1] : "null");
			exit(1);
		}
	}
	
	topic_count = argc - optind;
	if(topic_count <= 0)
	{
		SDNMSG(SM_FATAL, "no topic_name\n");
		sleep(1);	// wait for printing error message
		exit(1);
	}

	if(pthread_mutex_init(&g_mutexSdnStatus, NULL) != 0)
	{
		SDNMSG(SM_ERR, "pthread_mutex_init(g_mutexSdnStatus) failed.\n"); 
		return SR_ERROR;
	}

	if(pthread_cond_init(&g_condSdnStatus, NULL) != 0)
	{
		SDNMSG(SM_ERR, "pthread_mutex_init(g_condSdnStatus) failed.\n"); 
		return SR_ERROR;
	}

	if(giTestMode == TEST_MODE_NORMAL)
	{
		if(giAsyncMode == 0)
		{
			pthread_t* piThreadId;

			piThreadId = (pthread_t*)malloc(sizeof(pthread_t*) * topic_count);

			for(int i=0; i<topic_count; i++)
			{
				/* Spawn sending thread for topics */
				SDNMSG(SM_LOG,"Spawning thread %d\n", i);
				pthread_create((pthread_t*)&piThreadId[i], NULL, &subscriber_thread, argv[optind++]);
			}

			for(int i=0; i<topic_count; i++)
			{
				pthread_join(piThreadId[i], NULL);
			}

			free(piThreadId);

			SDNMSG(SM_INFO,"sdn-sub application end\n");
		}
		else
		{
			int b_topic1 = 0;
			int b_topic2 = 0;
			int b_topic3 = 0;
			int b_topic1e = 0;
			int b_topic2e = 0;
			int b_topic3e = 0;
			
			for(int i=0; i<topic_count; i++)
			{
				if(strcmp(argv[optind], "topic1_test") == 0)
					b_topic1 = 1;
				else if(strcmp(argv[optind], "topic2_test") == 0)
					b_topic2 = 1;
				else if(strcmp(argv[optind], "topic3_test") == 0)
					b_topic3 = 1;
				else if(strcmp(argv[optind], "topic1e_test") == 0)
					b_topic1e = 1;
				else if(strcmp(argv[optind], "topic2e_test") == 0)
					b_topic2e = 1;
				else if(strcmp(argv[optind], "topic3e_test") == 0)
					b_topic3e = 1;

				optind++;
			}

			receiveAsynchronous(b_topic1, b_topic2, b_topic3, b_topic1e, b_topic2e, b_topic3e);
		}
	}
	else if(giTestMode == TEST_MODE_LOOPBACK_PERFORMANCE)
	{
		pthread_t iThreadId;

		/* Spawn loopback thread for topics */
		SDNMSG(SM_LOG,"Spawning loopback thread\n");
		pthread_create(&iThreadId, NULL, &loopback_thread, argv[optind++]);
		
		pthread_join(iThreadId, NULL);
	}

	pthread_cond_destroy(&g_condSdnStatus);
	pthread_mutex_destroy(&g_mutexSdnStatus);

	terminateSDN();
	
	return 0;
}



template<typename T, typename U> int do_subscribe(char* topic_name)
{
	int recv_count = 0;
	int err_num = 0;
	int ret;
	int err_payload = 0;
	int contents_count;
	int wait_pos = 0;
	SR_RET sr_ret;

	// for packet delay statistics
	int delayed_packets = 0;
	int max_delay = 0;
	int min_delay = 0;
	int sum_delay = 0;
	int latency;

	// for timeout receive statistics 
	int timeout_packets = 0;
	
	setSDNStatusCallback(CallbackSDNStatus);

	/* create a variable of type topic for subscription */
	T theMetaData;
	U theUserData;

	/* Create subscriber object for the topic */
	SdnSubscriber<T, U> theSubscriber;

    /*Set subscriber for the topic variable */
	sr_ret = theSubscriber.setSubscriber(theMetaData);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_FATAL,"theSubscriber.setSubscriber() failed. sr_ret=%d\n", sr_ret);
		return -1;
	}

	/* Enable Delay Check (for function test) */
	if(gbEnableTimeCheck)
		theSubscriber.enableCheckDelay(giDelayThreshold);

	/* Wait until SDN status is ready */
	while(!isSDNStatusReady())
	{
		LOCK(g_mutexSdnStatus);
		pthread_cond_wait(&g_condSdnStatus, &g_mutexSdnStatus);
		UNLOCK(g_mutexSdnStatus);
	}

	/* Initiate receive loop */
	for (int i=0; i<giPacketCount; ++i) 
	{
		if(giWaitPacket[wait_pos] != -1)
		{
			if((i+1) == giWaitPacket[wait_pos])
			{
				++wait_pos;
				usleep(giWaitDelay);
			}
		}

		/* Receive topic data */
		ret = theSubscriber.receive(theUserData, giTimeoutUsec); 
		if(ret != 0)
		{
			++err_num;
			
			if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
			{
				theSubscriber.getLatency(latency);
				
				if((max_delay == 0) && (min_delay == 0))
				{
					max_delay = latency;
					min_delay = latency;
				}
				
				if(max_delay < latency)
					max_delay = latency;
				if(min_delay > latency)
					min_delay = latency;
				
				sum_delay += latency;
				delayed_packets++;
			}
			else if(ret == SR_ERR_RECV_TIMEOUT)
				timeout_packets++;
		}
		else
		{
			if(gbEnablePayloadCheck)
			{
				unsigned char value = theUserData.actuator1[0];
				
				contents_count = sizeof(theUserData.actuator1);
				for(int j=0; j<contents_count; ++j)
				{
					if(value != theUserData.actuator1[j])
					{
						// print 5 errors maximum (too much information is not helpful)
						if(err_payload < 5)
							SDNMSG(SM_ERR,"payload mismatch at offset %d. expect=%d, received=%d\n", j, value, theUserData.actuator1[j]);
						
						++err_payload;
						++err_num;
					}

					++value;
				}
			}
		}

		++recv_count;

		// print every 1024 count to see if it is running (it is helpful when large iterations)
		if((recv_count & 0x3ff) == 0)
			SDNMSG(SM_LOG,"count = %d\n", recv_count);
	}

	if(giTimeoutUsec != 0)
	{
		SDNMSG(SM_RESULT, "[Timeout check result]\n");
		SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
		SDNMSG(SM_RESULT, "timeout packets: %d\n", timeout_packets);
		SDNMSG(SM_RESULT, "timeout value: %d usec\n", giTimeoutUsec);
	}
	
	if(gbEnableTimeCheck)
	{
		SDNMSG(SM_RESULT, "[Time check result]\n");
		SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
		SDNMSG(SM_RESULT, "delayed packets: %d\n", delayed_packets);
		SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", max_delay);
		SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", min_delay);
		SDNMSG(SM_RESULT, "average delay time (usec): %d\n", delayed_packets ? (sum_delay / delayed_packets) : 0);
	}
		
	if(err_num == 0)
		SDNMSG(SM_RESULT,"%d packets are received for %s successfully.\n", recv_count, topic_name ? topic_name : "null");
 	else
 	{
		if(err_payload)
			SDNMSG(SM_RESULT,"number of payload error: %d\n", err_payload);

		SDNMSG(SM_RESULT,"%d packets are received for %s with %d errors.\n", recv_count, topic_name ? topic_name : "null", err_num);
 	}
		
	theSubscriber.unsetSubscriber();

	unsetSDNStatusCallback();

	return 0;
}



void* subscriber_thread (void* param) 
{
	char* topic_name = (char*)param;
	
	SDNMSG(SM_LOG,"Subscribing thread %s called\n", topic_name ? topic_name : "null");

	if(strcmp(topic_name, "topic1_test") == 0)
		do_subscribe<topic1_test_metadata, topic1_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic2_test") == 0)
		do_subscribe<topic2_test_metadata, topic2_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic3_test") == 0)
		do_subscribe<topic3_test_metadata, topic3_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic1e_test") == 0)
		do_subscribe<topic1e_test_metadata, topic1e_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic2e_test") == 0)
		do_subscribe<topic2e_test_metadata, topic2e_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic3e_test") == 0)
		do_subscribe<topic3e_test_metadata, topic3e_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic1_test_modified") == 0)
		do_subscribe<topic1_test_modified_metadata, topic1_test_modified_userdata>(topic_name);
	else
		SDNMSG(SM_ERR,"no matched topic name [%s] found.\n", topic_name);

	pthread_exit(NULL); 
	return (void*)NULL; 
}



int receiveAsynchronous (int b_topic1, int b_topic2, int b_topic3, int b_topic1e, int b_topic2e, int b_topic3e) 
{
	SR_RET sr_ret;
	
	SDNMSG(SM_LOG,"receiveAsynchronous(%d, %d, %d, %d, %d, %d) called\n", 
		b_topic1, b_topic2, b_topic3, b_topic1e, b_topic2e, b_topic3e);

	sem_init(&sem1, 0, 0);
	sem_init(&sem2, 0, 0);
	sem_init(&sem3, 0, 0);
	sem_init(&sem1e, 0, 0);
	sem_init(&sem2e, 0, 0);
	sem_init(&sem3e, 0, 0);

	/* create a variable of type topic for subscription */
	topic1_test_metadata theMetaData1;
	topic2_test_metadata theMetaData2;
	topic3_test_metadata theMetaData3;
	topic1e_test_metadata theMetaData1e;
	topic2e_test_metadata theMetaData2e;
	topic3e_test_metadata theMetaData3e;
	
	
	/* Create subscriber object for the topic */
	SdnSubscriber<topic1_test> theSubscriber1;
	SdnSubscriber<topic2_test> theSubscriber2;
	SdnSubscriber<topic3_test> theSubscriber3;
	SdnSubscriber<topic1e_test> theSubscriber1e;
	SdnSubscriber<topic2e_test> theSubscriber2e;
	SdnSubscriber<topic3e_test> theSubscriber3e;


	/**********************************************************/
	/* Set subscriber for the topic variable */
	/* Set subscriber asyncronous receive callback function */
	if(b_topic1)
	{
		sr_ret = theSubscriber1.setSubscriber(theMetaData1);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_ERR,"[receiveAsynchronous] theSubscriber1.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			return -1; 
		}
		
		if(gbEnableTimeCheck)
		{
			giDelayedPackets1 = 0;
			giMaxDelay1 = 0;
			giMinDelay1 = 0;
			giSumDelay1 = 0;

			theSubscriber1.enableCheckDelay(giDelayThreshold);
		}

		theSubscriber1.setReceiveCallback(CallbackAsyncReceive1);
	}
	
	if(b_topic2)
	{
		sr_ret = theSubscriber2.setSubscriber(theMetaData2);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_ERR,"[receiveAsynchronous] theSubscriber2.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			return -1; 
		}
		
		if(gbEnableTimeCheck)
		{
			giDelayedPackets2 = 0;
			giMaxDelay2 = 0;
			giMinDelay2 = 0;
			giSumDelay2 = 0;
		
			theSubscriber2.enableCheckDelay(giDelayThreshold);
		}
		
		theSubscriber2.setReceiveCallback(CallbackAsyncReceive2);
	}
	
	if(b_topic3)
	{
		sr_ret = theSubscriber3.setSubscriber(theMetaData3);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_ERR,"[receiveAsynchronous] theSubscriber3.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			return -1; 
		}
		
		if(gbEnableTimeCheck)
		{
			giDelayedPackets3 = 0;
			giMaxDelay3 = 0;
			giMinDelay3 = 0;
			giSumDelay3 = 0;
			
			theSubscriber3.enableCheckDelay(giDelayThreshold);
		}

		theSubscriber3.setReceiveCallback(CallbackAsyncReceive3);
	}

	if(b_topic1e)
	{
		sr_ret = theSubscriber1e.setSubscriber(theMetaData1e);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_ERR,"[receiveAsynchronous] theSubscriber1e.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			return -1; 
		}
		
		if(gbEnableTimeCheck)
		{
			giDelayedPackets1e = 0;
			giMaxDelay1e = 0;
			giMinDelay1e = 0;
			giSumDelay1e = 0;

			theSubscriber1e.enableCheckDelay(giDelayThreshold);
		}

		theSubscriber1e.setReceiveCallback(CallbackAsyncReceive1e);
	}
	
	if(b_topic2e)
	{
		sr_ret = theSubscriber2e.setSubscriber(theMetaData2e);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_ERR,"[receiveAsynchronous] theSubscriber2e.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			return -1; 
		}
		
		if(gbEnableTimeCheck)
		{
			giDelayedPackets2e = 0;
			giMaxDelay2e = 0;
			giMinDelay2e = 0;
			giSumDelay2e = 0;
		
			theSubscriber2e.enableCheckDelay(giDelayThreshold);
		}
		
		theSubscriber2e.setReceiveCallback(CallbackAsyncReceive2e);
	}
	
	if(b_topic3e)
	{
		sr_ret = theSubscriber3e.setSubscriber(theMetaData3e);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_ERR,"[receiveAsynchronous] theSubscriber3e.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			return -1; 
		}
		
		if(gbEnableTimeCheck)
		{
			giDelayedPackets3e = 0;
			giMaxDelay3e = 0;
			giMinDelay3e = 0;
			giSumDelay3e = 0;
			
			theSubscriber3e.enableCheckDelay(giDelayThreshold);
		}

		theSubscriber3e.setReceiveCallback(CallbackAsyncReceive3e);
	}


	/**********************************************************/
	/* Wait until all sem_post() are executed */
	if(b_topic1)
		sem_wait(&sem1);

	if(b_topic2)
		sem_wait(&sem2);

	if(b_topic3)
		sem_wait(&sem3);

	if(b_topic1e)
		sem_wait(&sem1e);

	if(b_topic2e)
		sem_wait(&sem2e);

	if(b_topic3e)
		sem_wait(&sem3e);


	/**********************************************************/
	/* Display results if time check mode */
	if(gbEnableTimeCheck)
	{
		if(b_topic1)
		{
			SDNMSG(SM_RESULT, "[Time check result for topic1_test]\n");
			SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
			SDNMSG(SM_RESULT, "delayed packets: %d\n", giDelayedPackets1);
			SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", giMaxDelay1);
			SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", giMinDelay1);
			SDNMSG(SM_RESULT, "average delay time (usec): %d\n", giDelayedPackets1 ? (giSumDelay1 / giDelayedPackets1) : 0);
		}

		if(b_topic2)
		{
			SDNMSG(SM_RESULT, "[Time check result for topic2_test]\n");
			SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
			SDNMSG(SM_RESULT, "delayed packets: %d\n", giDelayedPackets2);
			SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", giMaxDelay2);
			SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", giMinDelay2);
			SDNMSG(SM_RESULT, "average delay time (usec): %d\n", giDelayedPackets2 ? (giSumDelay2 / giDelayedPackets2) : 0);
		}

		if(b_topic3)
		{
			SDNMSG(SM_RESULT, "[Time check result for topic3_test]\n");
			SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
			SDNMSG(SM_RESULT, "delayed packets: %d\n", giDelayedPackets3);
			SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", giMaxDelay3);
			SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", giMinDelay3);
			SDNMSG(SM_RESULT, "average delay time (usec): %d\n", giDelayedPackets3 ? (giSumDelay3 / giDelayedPackets3) : 0);
		}

		if(b_topic1e)
		{
			SDNMSG(SM_RESULT, "[Time check result for topic1e_test]\n");
			SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
			SDNMSG(SM_RESULT, "delayed packets: %d\n", giDelayedPackets1e);
			SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", giMaxDelay1e);
			SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", giMinDelay1e);
			SDNMSG(SM_RESULT, "average delay time (usec): %d\n", giDelayedPackets1e ? (giSumDelay1e / giDelayedPackets1e) : 0);
		}
		
		if(b_topic2e)
		{
			SDNMSG(SM_RESULT, "[Time check result for topic2e_test]\n");
			SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
			SDNMSG(SM_RESULT, "delayed packets: %d\n", giDelayedPackets2e);
			SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", giMaxDelay2e);
			SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", giMinDelay2e);
			SDNMSG(SM_RESULT, "average delay time (usec): %d\n", giDelayedPackets2e ? (giSumDelay2e / giDelayedPackets2e) : 0);
		}
		
		if(b_topic3e)
		{
			SDNMSG(SM_RESULT, "[Time check result for topic3e_test]\n");
			SDNMSG(SM_RESULT, "total: %d\n", giPacketCount);
			SDNMSG(SM_RESULT, "delayed packets: %d\n", giDelayedPackets3e);
			SDNMSG(SM_RESULT, "maximum delay time (usec): %d\n", giMaxDelay3e);
			SDNMSG(SM_RESULT, "minimum delay time (usec): %d\n", giMinDelay3e);
			SDNMSG(SM_RESULT, "average delay time (usec): %d\n", giDelayedPackets3e ? (giSumDelay3e / giDelayedPackets3e) : 0);
		}
	}

		
	/**********************************************************/
	/* Unset subscriber asyncronous receive callback function */
	/* Unset subscriber for the topic variable */
	if(b_topic1)
	{
		theSubscriber1.unsetReceiveCallback();
		theSubscriber1.unsetSubscriber();
	}

	if(b_topic2)
	{
		theSubscriber2.unsetReceiveCallback();
		theSubscriber2.unsetSubscriber();
	}

	if(b_topic3)
	{
		theSubscriber3.unsetReceiveCallback();
		theSubscriber3.unsetSubscriber();
	}

	if(b_topic1e)
	{
		theSubscriber1e.unsetReceiveCallback();
		theSubscriber1e.unsetSubscriber();
	}

	if(b_topic2e)
	{
		theSubscriber2e.unsetReceiveCallback();
		theSubscriber2e.unsetSubscriber();
	}

	if(b_topic3e)
	{
		theSubscriber3e.unsetReceiveCallback();
		theSubscriber3e.unsetSubscriber();
	}

	return 0; 
}



void CallbackSDNStatus(int is_ready)
{
	SDNMSG(SM_INFO,"SDNStatusCallback : SDN status [%s].\n", is_ready ? "READY" : "NOT READY");

	LOCK(g_mutexSdnStatus);
	pthread_cond_broadcast(&g_condSdnStatus);
	UNLOCK(g_mutexSdnStatus);
}



template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceive1(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	static int err_num = 0;
	static int err_payload = 0;
	static int recv_count = 0;
	int ret = SR_OK;
	int contents_count;
	int latency;
	topic_userdata theUserData;

	if(err_code != SR_OK)
	{
		++err_num;
		ret = err_code;

		if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
		{
			p_subscriber->getLatency(latency);
			
			if((giMaxDelay1 == 0) && (giMinDelay1 == 0))
			{
				giMaxDelay1 = latency;
				giMinDelay1 = latency;
			}
			
			if(giMaxDelay1 < latency)
				giMaxDelay1 = latency;
			if(giMinDelay1 > latency)
				giMinDelay1 = latency;
			
			giSumDelay1 += latency;
			giDelayedPackets1++;
		}
	}
	else
	{
		p_subscriber->getAsyncUserData(theUserData);
		
		if(gbEnablePayloadCheck)
		{
			unsigned char value = theUserData.actuator1[0];
			
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				if(value != theUserData.actuator1[j])
				{
					// print 5 errors maximum (too much information is not helpful)
					if(err_payload < 5)
						SDNMSG(SM_ERR,"[topic1_test] payload mismatch at offset %d. expect=%d, received=%d\n", 
							j, value, theUserData.actuator1[j]);
					
					++err_payload;
					++err_num;
					ret = SR_ERROR;
				}

				++value;
			}
		}
	}
	
	++recv_count;

	// print every 1024 count to see if it is running (it is helpful when large iterations)
	if((recv_count & 0x3ff) == 0)
		SDNMSG(SM_LOG,"[topic1_test] count = %d\n", recv_count);

	if(recv_count >= giPacketCount)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"[topic1_test] %d packets are received asynchronously successfully.\n", recv_count);
		else
		{
			if(err_payload)
				SDNMSG(SM_RESULT,"[topic1_test] number of payload error: %d\n", err_payload);
				
			SDNMSG(SM_RESULT,"[topic1_test] %d packets are received with %d errors.\n", recv_count, err_num);
		}

		sem_post(&sem1);
	}

	return ret;
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceive2(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	static int err_num = 0;
	static int err_payload = 0;
	static int recv_count = 0;
	int ret = SR_OK;
	int contents_count;
	int latency;
	topic_userdata theUserData;
	
	if(err_code != SR_OK)
	{
		++err_num;
		ret = err_code;

		if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
		{
			p_subscriber->getLatency(latency);
			
			if((giMaxDelay2 == 0) && (giMinDelay2 == 0))
			{
				giMaxDelay2 = latency;
				giMinDelay2 = latency;
			}
			
			if(giMaxDelay2 < latency)
				giMaxDelay2 = latency;
			if(giMinDelay2 > latency)
				giMinDelay2 = latency;
			
			giSumDelay2 += latency;
			giDelayedPackets2++;
		}
	}
	else
	{
		p_subscriber->getAsyncUserData(theUserData);
		
		if(gbEnablePayloadCheck)
		{
			unsigned char value = theUserData.actuator1[0];
			
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				if(value != theUserData.actuator1[j])
				{
					// print 5 errors maximum (too much information is not helpful)
					if(err_payload < 5)
						SDNMSG(SM_ERR,"[topic2_test] payload mismatch at offset %d. expect=%d, received=%d\n", 
							j, value, theUserData.actuator1[j]);
					
					++err_payload;
					++err_num;
					ret = SR_ERROR;
				}

				++value;
			}
		}
	}
	
	++recv_count;

	// print every 1024 count to see if it is running (it is helpful when large iterations)
	if((recv_count & 0x3ff) == 0)
		SDNMSG(SM_LOG,"[topic2_test] count = %d\n", recv_count);

	if(recv_count >= giPacketCount)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"[topic2_test] %d packets are received asynchronously successfully.\n", recv_count);
		else
		{
			if(err_payload)
				SDNMSG(SM_RESULT,"[topic2_test] number of payload error: %d\n", err_payload);
				
			SDNMSG(SM_RESULT,"[topic2_test] %d packets are received with %d errors.\n", recv_count, err_num);
		}

		sem_post(&sem2);
	}

	return ret;
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceive3(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	static int err_num = 0;
	static int err_payload = 0;
	static int recv_count = 0;
	int ret = SR_OK;
	int contents_count;
	int latency;
	topic_userdata theUserData;
	
	if(err_code != SR_OK)
	{
		++err_num;
		ret = err_code;

		if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
		{
			p_subscriber->getLatency(latency);
			
			if((giMaxDelay3 == 0) && (giMinDelay3 == 0))
			{
				giMaxDelay3 = latency;
				giMinDelay3 = latency;
			}
			
			if(giMaxDelay3 < latency)
				giMaxDelay3 = latency;
			if(giMinDelay3 > latency)
				giMinDelay3 = latency;
			
			giSumDelay3 += latency;
			giDelayedPackets3++;
		}
	}
	else
	{
		p_subscriber->getAsyncUserData(theUserData);
		
		if(gbEnablePayloadCheck)
		{
			unsigned char value = theUserData.actuator1[0];
			
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				if(value != theUserData.actuator1[j])
				{
					// print 5 errors maximum (too much information is not helpful)
					if(err_payload < 5)
						SDNMSG(SM_ERR,"[topic3_test] payload mismatch at offset %d. expect=%d, received=%d\n", 
							j, value, theUserData.actuator1[j]);
					
					++err_payload;
					++err_num;
					ret = SR_ERROR;
				}

				++value;
			}
		}
	}
	
	++recv_count;

	// print every 1024 count to see if it is running (it is helpful when large iterations)
	if((recv_count & 0x3ff) == 0)
		SDNMSG(SM_LOG,"[topic3_test] count = %d\n", recv_count);

	if(recv_count >= giPacketCount)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"[topic3_test] %d packets are received asynchronously successfully.\n", recv_count);
		else
		{
			if(err_payload)
				SDNMSG(SM_RESULT,"[topic3_test] number of payload error: %d\n", err_payload);
				
			SDNMSG(SM_RESULT,"[topic3_test] %d packets are received with %d errors.\n", recv_count, err_num);
		}

		sem_post(&sem3);
	}

	return ret;
}



template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceive1e(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	static int err_num = 0;
	static int err_payload = 0;
	static int recv_count = 0;
	int ret = SR_OK;
	int contents_count;
	int latency;
	topic_userdata theUserData;

	if(err_code != SR_OK)
	{
		++err_num;
		ret = err_code;

		if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
		{
			p_subscriber->getLatency(latency);
			
			if((giMaxDelay1e == 0) && (giMinDelay1e == 0))
			{
				giMaxDelay1e = latency;
				giMinDelay1e = latency;
			}
			
			if(giMaxDelay1e < latency)
				giMaxDelay1e = latency;
			if(giMinDelay1e > latency)
				giMinDelay1e = latency;
			
			giSumDelay1e += latency;
			giDelayedPackets1e++;
		}
	}
	else
	{
		p_subscriber->getAsyncUserData(theUserData);
		
		if(gbEnablePayloadCheck)
		{
			unsigned char value = theUserData.actuator1[0];
			
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				if(value != theUserData.actuator1[j])
				{
					// print 5 errors maximum (too much information is not helpful)
					if(err_payload < 5)
						SDNMSG(SM_ERR,"[topic1e_test] payload mismatch at offset %d. expect=%d, received=%d\n", 
							j, value, theUserData.actuator1[j]);
					
					++err_payload;
					++err_num;
					ret = SR_ERROR;
				}

				++value;
			}
		}
	}
	
	++recv_count;

	// print every 1024 count to see if it is running (it is helpful when large iterations)
	if((recv_count & 0x3ff) == 0)
		SDNMSG(SM_LOG,"[topic1e_test] count = %d\n", recv_count);

	if(recv_count >= giPacketCount)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"[topic1e_test] %d packets are received asynchronously successfully.\n", recv_count);
		else
		{
			if(err_payload)
				SDNMSG(SM_RESULT,"[topic1e_test] number of payload error: %d\n", err_payload);
				
			SDNMSG(SM_RESULT,"[topic1e_test] %d packets are received with %d errors.\n", recv_count, err_num);
		}

		sem_post(&sem1e);
	}

	return ret;
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceive2e(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	static int err_num = 0;
	static int err_payload = 0;
	static int recv_count = 0;
	int ret = SR_OK;
	int contents_count;
	int latency;
	topic_userdata theUserData;
	
	if(err_code != SR_OK)
	{
		++err_num;
		ret = err_code;

		if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
		{
			p_subscriber->getLatency(latency);
			
			if((giMaxDelay2e == 0) && (giMinDelay2e == 0))
			{
				giMaxDelay2e = latency;
				giMinDelay2e = latency;
			}
			
			if(giMaxDelay2e < latency)
				giMaxDelay2e = latency;
			if(giMinDelay2e > latency)
				giMinDelay2e = latency;
			
			giSumDelay2e += latency;
			giDelayedPackets2e++;
		}
	}
	else
	{
		p_subscriber->getAsyncUserData(theUserData);
		
		if(gbEnablePayloadCheck)
		{
			unsigned char value = theUserData.actuator1[0];
			
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				if(value != theUserData.actuator1[j])
				{
					// print 5 errors maximum (too much information is not helpful)
					if(err_payload < 5)
						SDNMSG(SM_ERR,"[topic2e_test] payload mismatch at offset %d. expect=%d, received=%d\n", 
							j, value, theUserData.actuator1[j]);
					
					++err_payload;
					++err_num;
					ret = SR_ERROR;
				}

				++value;
			}
		}
	}
	
	++recv_count;

	// print every 1024 count to see if it is running (it is helpful when large iterations)
	if((recv_count & 0x3ff) == 0)
		SDNMSG(SM_LOG,"[topic2e_test] count = %d\n", recv_count);

	if(recv_count >= giPacketCount)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"[topic2e_test] %d packets are received asynchronously successfully.\n", recv_count);
		else
		{
			if(err_payload)
				SDNMSG(SM_RESULT,"[topic2e_test] number of payload error: %d\n", err_payload);
				
			SDNMSG(SM_RESULT,"[topic2e_test] %d packets are received with %d errors.\n", recv_count, err_num);
		}

		sem_post(&sem2e);
	}

	return ret;
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceive3e(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	static int err_num = 0;
	static int err_payload = 0;
	static int recv_count = 0;
	int ret = SR_OK;
	int contents_count;
	int latency;
	topic_userdata theUserData;
	
	if(err_code != SR_OK)
	{
		++err_num;
		ret = err_code;

		if(gbEnableTimeCheck && (ret == SR_ERR_PACKET_DELAY))
		{
			p_subscriber->getLatency(latency);
			
			if((giMaxDelay3e == 0) && (giMinDelay3e == 0))
			{
				giMaxDelay3e = latency;
				giMinDelay3e = latency;
			}
			
			if(giMaxDelay3e < latency)
				giMaxDelay3e = latency;
			if(giMinDelay3e > latency)
				giMinDelay3e = latency;
			
			giSumDelay3e += latency;
			giDelayedPackets3e++;
		}
	}
	else
	{
		p_subscriber->getAsyncUserData(theUserData);
		
		if(gbEnablePayloadCheck)
		{
			unsigned char value = theUserData.actuator1[0];
			
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				if(value != theUserData.actuator1[j])
				{
					// print 5 errors maximum (too much information is not helpful)
					if(err_payload < 5)
						SDNMSG(SM_ERR,"[topic3e_test] payload mismatch at offset %d. expect=%d, received=%d\n", 
							j, value, theUserData.actuator1[j]);
					
					++err_payload;
					++err_num;
					ret = SR_ERROR;
				}

				++value;
			}
		}
	}
	
	++recv_count;

	// print every 1024 count to see if it is running (it is helpful when large iterations)
	if((recv_count & 0x3ff) == 0)
		SDNMSG(SM_LOG,"[topic3e_test] count = %d\n", recv_count);

	if(recv_count >= giPacketCount)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"[topic3e_test] %d packets are received asynchronously successfully.\n", recv_count);
		else
		{
			if(err_payload)
				SDNMSG(SM_RESULT,"[topic3e_test] number of payload error: %d\n", err_payload);
				
			SDNMSG(SM_RESULT,"[topic3e_test] %d packets are received with %d errors.\n", recv_count, err_num);
		}

		sem_post(&sem3e);
	}

	return ret;
}



#ifdef IMPROVE_ASYNC_PERFORMANCE
static int recv_loop_count;
static int publish_loop_err_num;

static topic1l_test_metadata theMetaData1lLoop;
static topic1l_test_userdata theUserData1lLoop;
static SdnPublisher<topic1l_test> thePublisher1lLoop;

static topic2l_test_metadata theMetaData2lLoop;
static topic2l_test_userdata theUserData2lLoop;
static SdnPublisher<topic2l_test> thePublisher2lLoop;

static topic3l_test_metadata theMetaData3lLoop;
static topic3l_test_userdata theUserData3lLoop;
static SdnPublisher<topic3l_test> thePublisher3lLoop;
#endif


void* loopback_thread (void* param) 
{
	int subscribe_err_num = 0;
	char* p_address = (char*)param;
	char* topic_name[2];
	char* next_ptr;
	int topic_mode;
	SR_RET sr_ret;
	
	topic1_test_metadata theMetaData1;
	topic1_test_userdata theUserData1;
	SdnSubscriber<topic1_test> theSubscriber1;

	topic2_test_metadata theMetaData2;
	topic2_test_userdata theUserData2;
	SdnSubscriber<topic2_test> theSubscriber2;

	topic3_test_metadata theMetaData3;
	topic3_test_userdata theUserData3;
	SdnSubscriber<topic3_test> theSubscriber3;

#ifndef IMPROVE_ASYNC_PERFORMANCE
	int recv_loop_count;
	int publish_loop_err_num;

	topic1l_test_metadata theMetaData1lLoop;
	topic1l_test_userdata theUserData1lLoop;
	SdnPublisher<topic1l_test> thePublisher1lLoop;

	topic2l_test_metadata theMetaData2lLoop;
	topic2l_test_userdata theUserData2lLoop;
	SdnPublisher<topic2l_test> thePublisher2lLoop;

	topic3l_test_metadata theMetaData3lLoop;
	topic3l_test_userdata theUserData3lLoop;
	SdnPublisher<topic3l_test> thePublisher3lLoop;
#endif

	SDNMSG(SM_LOG,"Subscribing thread %s called\n", p_address ? p_address : "null");

	recv_loop_count = 0;
	publish_loop_err_num = 0;
	
	topic_name[0] = strtok_r(p_address, ",", &next_ptr);
	if((topic_name[0] == NULL) || (next_ptr == NULL))
	{
		SDNMSG(SM_FATAL,"topic_name is invalid.\n");
		pthread_exit(NULL); 
		return (void*)NULL; 
	}
		
	topic_name[1] = strtok_r(NULL, ",", &next_ptr);
	if(topic_name[1] == NULL)
	{
		SDNMSG(SM_FATAL,"topic_name is invalid.\n");
		pthread_exit(NULL); 
		return (void*)NULL; 
	}
		
	if((strcmp(topic_name[0], "topic1_test") == 0) && 
		(strcmp(topic_name[1], "topic1l_test") == 0))
		topic_mode = 1;
	else if((strcmp(topic_name[0], "topic2_test") == 0) && 
		(strcmp(topic_name[1], "topic2l_test") == 0))
		topic_mode = 2;
	else if((strcmp(topic_name[0], "topic3_test") == 0) && 
		(strcmp(topic_name[1], "topic3l_test") == 0))
		topic_mode = 3;
	else
	{
		SDNMSG(SM_FATAL,"Invalid parameter. last parameter format should be \"topic[1,2,3]_test,topic[1l,2l,3l]_test\".\n");
		pthread_exit(NULL); 
		return (void*)NULL; 
	}
			
	setSDNStatusCallback(CallbackSDNStatus);
			
	if(topic_mode == 2)
	{
		/* Set for the topic variable */
		sr_ret = theSubscriber2.setSubscriber(theMetaData2);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_FATAL,"theSubscriber2.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			goto exit_loopback_thread;
		}

		if(giAsyncMode == 1)
		{
			sem_init(&sem_perf_test, 0, 0);
			theSubscriber2.setReceiveCallback(CallbackAsyncReceivePerfTest2);
		}

		sr_ret = thePublisher2lLoop.setPublisher(theMetaData2lLoop, gbEnableLoopback);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_FATAL,"thePublisher2lLoop.setPublisher() failed. sr_ret=%d\n", sr_ret);
			goto exit_loopback_thread;
		}

		/* Publish / Subscribe */
		for (int i=0; i<giPacketCount; ++i)
		{
			/* Receive topic data */
			if(giAsyncMode == 0)
			{
				sr_ret = theSubscriber2.receive(theUserData2); 
				if(sr_ret != SR_OK)
					++subscribe_err_num;

				/* Publish topic data */
				sr_ret = thePublisher2lLoop.publish(theUserData2lLoop);
				if(sr_ret != SR_OK)
					++publish_loop_err_num;
			}
			else
			{
				sem_wait(&sem_perf_test);

#ifndef IMPROVE_ASYNC_PERFORMANCE
				/* Publish topic data */
				sr_ret = thePublisher2lLoop.publish(theUserData2lLoop);
				if(sr_ret != SR_OK)
					++publish_loop_err_num;
#endif					
			}

			++recv_loop_count;

			// print every 1024 count to see if it is running (it is helpful when large iterations)
			if((recv_loop_count & 0x3ff) == 0)
				SDNMSG(SM_LOG,"count = %d\n", recv_loop_count);
		}

		if(giAsyncMode == 1)
			theSubscriber2.unsetReceiveCallback();

		sr_ret = theSubscriber2.unsetSubscriber();
		if(sr_ret != SR_OK)
			SDNMSG(SM_ERR,"theSubscriber2.unsetSubscriber() failed. sr_ret=%d\n", sr_ret);
		
		sr_ret = thePublisher2lLoop.unsetPublisher();
		if(sr_ret != SR_OK)
			SDNMSG(SM_ERR,"thePublisher2lLoop.unsetPublisher() failed. sr_ret=%d\n", sr_ret);
	}
	else if(topic_mode == 3)
	{
		/* Set for the topic variable */
		sr_ret = theSubscriber3.setSubscriber(theMetaData3);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_FATAL,"theSubscriber3.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			goto exit_loopback_thread;
		}

		if(giAsyncMode == 1)
		{
			sem_init(&sem_perf_test, 0, 0);
			theSubscriber3.setReceiveCallback(CallbackAsyncReceivePerfTest3);
		}

		sr_ret = thePublisher3lLoop.setPublisher(theMetaData3lLoop, gbEnableLoopback);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_FATAL,"thePublisher3lLoop.setPublisher() failed. sr_ret=%d\n", sr_ret);
			goto exit_loopback_thread;
		}

		/* Publish / Subscribe */
		for (int i=0; i<giPacketCount; ++i)
		{
			/* Receive topic data */
			if(giAsyncMode == 0)
			{
				sr_ret = theSubscriber3.receive(theUserData3); 
				if(sr_ret != SR_OK)
					++subscribe_err_num;

				/* Publish topic data */
				sr_ret = thePublisher3lLoop.publish(theUserData3lLoop);
				if(sr_ret != SR_OK)
					++publish_loop_err_num;
			}
			else
			{
				sem_wait(&sem_perf_test);

#ifndef IMPROVE_ASYNC_PERFORMANCE
				/* Publish topic data */
				sr_ret = thePublisher3lLoop.publish(theUserData3lLoop);
				if(sr_ret != SR_OK)
					++publish_loop_err_num;
#endif
			}
			
			++recv_loop_count;

			// print every 1024 count to see if it is running (it is helpful when large iterations)
			if((recv_loop_count & 0x3ff) == 0)
				SDNMSG(SM_LOG,"count = %d\n", recv_loop_count);
		}

		if(giAsyncMode == 1)
			theSubscriber3.unsetReceiveCallback();

		sr_ret = theSubscriber3.unsetSubscriber();
		if(sr_ret != SR_OK)
			SDNMSG(SM_ERR,"theSubscriber3.unsetSubscriber() failed. sr_ret=%d\n", sr_ret);

		sr_ret = thePublisher3lLoop.unsetPublisher();
		if(sr_ret != SR_OK)
			SDNMSG(SM_ERR,"thePublisher3lLoop.unsetPublisher() failed. sr_ret=%d\n", sr_ret);
	}
	else // default (topic_mode == 1)
	{
		/* Set for the topic variable */
		sr_ret = theSubscriber1.setSubscriber(theMetaData1);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_FATAL,"theSubscriber1.setSubscriber() failed. sr_ret=%d\n", sr_ret);
			goto exit_loopback_thread;
		}

		if(giAsyncMode == 1)
		{
			sem_init(&sem_perf_test, 0, 0);
			theSubscriber1.setReceiveCallback(CallbackAsyncReceivePerfTest1);
		}

		sr_ret = thePublisher1lLoop.setPublisher(theMetaData1lLoop, gbEnableLoopback);
		if(sr_ret != SR_OK)
		{
			SDNMSG(SM_FATAL,"thePublisher1lLoop.setPublisher() failed. sr_ret=%d\n", sr_ret);
			goto exit_loopback_thread;
		}

		/* Publish / Subscribe */
		for (int i=0; i<giPacketCount; ++i)
		{
			/* Receive topic data */
			if(giAsyncMode == 0)
			{
				sr_ret = theSubscriber1.receive(theUserData1); 
				if(sr_ret != SR_OK)
					++subscribe_err_num;
			
				/* Publish topic data */
				sr_ret = thePublisher1lLoop.publish(theUserData1lLoop);
				if(sr_ret != SR_OK)
					++publish_loop_err_num;
			}
			else
			{
				sem_wait(&sem_perf_test);
			
#ifndef IMPROVE_ASYNC_PERFORMANCE
				/* Publish topic data */
				sr_ret = thePublisher1lLoop.publish(theUserData1lLoop);
				if(sr_ret != SR_OK)
					++publish_loop_err_num;
#endif
			}

			++recv_loop_count;

			// print every 1024 count to see if it is running (it is helpful when large iterations)
			if((recv_loop_count & 0x3ff) == 0)
				SDNMSG(SM_LOG,"count = %d\n", recv_loop_count);
		}

		if(giAsyncMode == 1)
			theSubscriber1.unsetReceiveCallback();

		sr_ret = theSubscriber1.unsetSubscriber();
		if(sr_ret != SR_OK)
			SDNMSG(SM_ERR,"theSubscriber1.unsetSubscriber() failed. sr_ret=%d\n", sr_ret);

		sr_ret = thePublisher1lLoop.unsetPublisher();
		if(sr_ret != SR_OK)
			SDNMSG(SM_ERR,"thePublisher1lLoop.unsetPublisher() failed. sr_ret=%d\n", sr_ret);
	}
	
	if((publish_loop_err_num == 0) && (subscribe_err_num == 0))
		SDNMSG(SM_RESULT,"%d packets are looped back successfully.\n", recv_loop_count);
 	else
		SDNMSG(SM_RESULT,"%d packets are looped back with %d(s), %d(p) errors.\n", recv_loop_count, subscribe_err_num, publish_loop_err_num);

exit_loopback_thread:		
	unsetSDNStatusCallback();
	pthread_exit(NULL); 

	return (void*)NULL; 
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceivePerfTest1(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	int ret = SR_OK;
	
#ifdef IMPROVE_ASYNC_PERFORMANCE
	/* Publish topic data */
	if(thePublisher1lLoop.publish(theUserData1lLoop) != SR_OK)
		++publish_loop_err_num;
#endif

	sem_post(&sem_perf_test);

	if(err_code != SR_OK)
	{
		ret = err_code;
		SDNMSG(SM_ERR,"[CallbackAsyncReceivePerfTest] error code [%d]\n", err_code);
	}

	return ret;
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceivePerfTest2(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	int ret = SR_OK;
	
#ifdef IMPROVE_ASYNC_PERFORMANCE
		/* Publish topic data */
		if(thePublisher2lLoop.publish(theUserData2lLoop) != SR_OK)
			++publish_loop_err_num;
#endif
	
	sem_post(&sem_perf_test);

	if(err_code != SR_OK)
	{
		ret = err_code;
		SDNMSG(SM_ERR,"[CallbackAsyncReceivePerfTest] error code [%d]\n", err_code);
	}

	return ret;
}


template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceivePerfTest3(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	int ret = SR_OK;
	
#ifdef IMPROVE_ASYNC_PERFORMANCE
		/* Publish topic data */
		if(thePublisher3lLoop.publish(theUserData3lLoop) != SR_OK)
			++publish_loop_err_num;
#endif
	
	sem_post(&sem_perf_test);

	if(err_code != SR_OK)
	{
		ret = err_code;
		SDNMSG(SM_ERR,"[CallbackAsyncReceivePerfTest] error code [%d]\n", err_code);
	}

	return ret;
}

