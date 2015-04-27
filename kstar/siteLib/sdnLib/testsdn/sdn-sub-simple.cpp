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

using namespace std; 

#define	IMPROVE_ASYNC_PERFORMANCE	1

// parameter '-m'(test mode)
#define	TEST_MODE_NORMAL				0

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

static pthread_mutex_t g_mutexSdnStatus; 
static pthread_cond_t g_condSdnStatus;	

void* subscriber_thread (void* param);

void CallbackSDNStatus(int is_ready);

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
	else
		SDNMSG(SM_ERR,"no matched topic name [%s] found.\n", topic_name);

	pthread_exit(NULL); 
	return (void*)NULL; 
}



void CallbackSDNStatus(int is_ready)
{
	SDNMSG(SM_INFO,"SDNStatusCallback : SDN status [%s].\n", is_ready ? "READY" : "NOT READY");

	LOCK(g_mutexSdnStatus);
	pthread_cond_broadcast(&g_condSdnStatus);
	UNLOCK(g_mutexSdnStatus);
}

