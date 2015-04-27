/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/examples/sdn-pub.cpp $
* $Id: sdn-pub.cpp 36365 2013-04-30 13:18:59Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API Example sdn-pub source file.
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

/** SdnPublisher function test code 
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
#include <semaphore.h>
#include <math.h>

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
#define	TEST_MODE_EVENT_PERFORMANCE		2

#define	MAX_SKIP_PACKET	10
#define	MAX_WAIT_PACKET	10

static int giTestMode = 0;
static int giPacketCount = 100;
static int giAsyncMode = 0;
static int giSendDelay = -1;
static int giEventTimeout = 0;
static int giTimeStampDelay = 0;
static int giTimeStampLead = 0;
static int gbFillContents = 1;
static int gbEnableLoopback = 0;
static int giSkipPacket[MAX_SKIP_PACKET+1] = {-1,};
static int giWaitDelay = 0;
static int giWaitPacket[MAX_WAIT_PACKET+1] = {-1,};

static int giEventTimeoutCount = 0;
static int giEventAckCount = 0;
static unsigned long* gpulLatency = NULL;

static char* gpDefConfigFile = NULL;
static char* gpUserConfigFile = NULL;

static pthread_mutex_t g_mutexSdnStatus; 
static pthread_cond_t g_condSdnStatus;	

void* publish_thread (void* param);
void* loopback_thread (void* param);
void CallbackSDNStatus(int is_ready);
void CallbackAckStatus(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec);
template <typename topic_metadata, typename topic_userdata> 
	SR_RET CallbackAsyncReceivePerfTest(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
void recordLatency(struct timespec* sendtime, struct timespec* roundtriptime, unsigned int iterations, 
	unsigned short sdnpacketsize, const char* outputfile);
void recordEventLatency(unsigned long* p_latency, unsigned int iterations, const char*outputfile);

static struct option options[] = {
	{"count", 1, 0, 'c'},
	{"delay", 1, 0, 'd'},
	{"timeout", 1, 0, 't'},
	{"timestamp_delay", 1, 0, 'p'},
	{"timestamp_lead", 1, 0, 'n'},
	{"skip", 1, 0, 's'},
	{"fill", 1, 0, 'f'},
	{"usec_for_wait", 1, 0, 'u'},
	{"wait", 1, 0, 'w'},
	{"loglevel", 1, 0, 'l'},
	{"async", 1, 0, 'a'},
	{"loopback", 1, 0, 'b'},
	{"register", 1, 0, 'r'},
	{"mode", 1, 0, 'm'},
	{"def_conf_file", 1, 0, 'x'},
	{"user_conf_file", 1, 0, 'y'},
	{"help", 0, 0, '?'},
	{"", 0, 0, 0}
};

static sem_t sem_perf_test;

#ifdef IMPROVE_ASYNC_PERFORMANCE
static struct timespec* p_sendtime;
static struct timespec* p_recvtime;
static int loop_count;
#endif


static void usage(void)
{
	printf("Usage: sdn-pub [options] <if-name> topic1(e)_test [topic2(e)_test topic3(e)_test]\n");
	printf(" * event topic: topic1e_test topic2e_test topic3e_test]\n");

	printf("  -c, --count             send packet count\n");
	printf("  -d, --delay             time delay in usec between packet transmission(default: -1(no delay))\n");
	printf("  -t, --timeout           timeout for event topic (usec)\n");
	printf("  -p, --timestamp_delay   add timestamp value (usec)\n");
	printf("  -n, --timestamp_lead    subtract timestamp value (usec)\n");
	printf("  -s, --skip              skip data packet for the specified sequence number (max 10)\n");
	printf("  -f, --fill              fill payload contents according to the pre-defined rule\n");
	printf("  -u, --usec_for_wait     delay in usec for wait\n");
	printf("  -w, --wait              wait before receive() for specified iteration count\n");
	printf("  -l, --loglevel          log level. 1(all) ~ 5\n");
	printf("  -a, --async             receive data in asynchronous mode(1) (valid only when test mode 1).\n");
	printf("  -b, --loopback          enable(1)/disable(0) multicast loopback (default:0).\n");
	printf("  -r, --register          enable(1)/disable(0) registering to SDN monitor node(default:1)\n");
	printf("  -x, --def_conf_file     default config filename\n");
	printf("  -y, --user_conf_file    user config filename\n");
	printf("  -m, --mode              test mode(default:0)\n");
	printf("                            - 0: normal test. all options are available\n");
	printf("                            - 1: loopback test and record result. available options(a:c:l).\n");
	printf("                                 parameter: topic[1,2,3]_test,topic[1,2,3]l_test\n");
	printf("                                 * topic1_test,topic1l_test: userdata size is 1 byte\n");
	printf("                                 * topic2_test,topic2l_test: userdata size is 1K bytes\n");
	printf("                                 * topic3_test,topic3l_test: userdata size is 8K bytes\n");
}

int main(int argc, char** argv)
{
	int skip_packet_index = 0;
	int wait_packet_index = 0;
	int topic_count = 0;
	SR_RET sr_ret;
	
	if ( argc > 1 )
	{
		int opt_index = 0;
		int opt = 0;
		
		while ((opt = getopt_long(argc, argv, "c:a:d:t:p:n:s:f:u:w:l:b:r:x:y:m:?", options, &opt_index)) != -1)
		{
			switch(opt)
			{
			case 'c':
				giPacketCount = atoi(optarg);
				break;
				
			case 'a':
				giAsyncMode = atoi(optarg);
				break;
				
			case 'd':
				giSendDelay = atoi(optarg);
				break;
					
			case 't':
				giEventTimeout = atoi(optarg);
				break;
					
			case 'p':
				giTimeStampDelay = atoi(optarg);
				break;
				
			case 'n':
				giTimeStampLead = atoi(optarg);
				break;

			case 's':
				if(skip_packet_index >= MAX_SKIP_PACKET)
				{
					printf("maximum skip count is %d\n", MAX_SKIP_PACKET);
					exit(1);
				}

				giSkipPacket[skip_packet_index++] = atoi(optarg);
				break;

			case 'f':
				gbFillContents = atoi(optarg);
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
		sr_ret = initializeSDN(gpDefConfigFile, gpUserConfigFile);
		if(sr_ret != SR_OK)
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
		
		sr_ret = initializeSDN(argv[optind++]);
		if(sr_ret != SR_OK)
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

	if((giTestMode == TEST_MODE_EVENT_PERFORMANCE) || (giTestMode == TEST_MODE_LOOPBACK_PERFORMANCE))
	{
		if(topic_count != 1)
		{
			SDNMSG(SM_FATAL, "performance test can be done for only 1 topic\n");
			sleep(1);	// wait for printing error message
			exit(1);
		}
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

	if((giTestMode == TEST_MODE_NORMAL) || (giTestMode == TEST_MODE_EVENT_PERFORMANCE))
	{
		pthread_t* piThreadId;
		char* pFilename = (char*)"";

		piThreadId = (pthread_t*)malloc(sizeof(pthread_t*) * topic_count);
		if(piThreadId == NULL)
		{
			SDNMSG(SM_FATAL, "out of memory\n");
			sleep(1);	// wait for printing error message
			exit(1);
		}

		if(giTestMode == TEST_MODE_EVENT_PERFORMANCE)
		{
			gpulLatency = (unsigned long*)calloc(1, giPacketCount*sizeof(unsigned long));
			pFilename = argv[optind];
			if(gpulLatency == NULL)
			{
				SDNMSG(SM_FATAL, "out of memory\n");
				sleep(1);	// wait for printing error message
				exit(1);
			}
		}

		for(int i=0; i<topic_count; i++)
		{
			/* Spawn sending thread for topics */
			SDNMSG(SM_LOG,"Spawning thread %d\n", i);
			pthread_create((pthread_t*)&piThreadId[i], NULL, &publish_thread, argv[optind++]);
		}

		for(int i=0; i<topic_count; i++)
		{
			pthread_join(piThreadId[i], NULL);
		}

		if((giEventAckCount != 0) || (giEventTimeoutCount != 0))
		{
			SDNMSG(SM_RESULT,"Number of acknowledged events: %d.\n", giEventAckCount);
			SDNMSG(SM_RESULT,"Number of timeout events: %d.\n", giEventTimeoutCount);
		}

		if(giTestMode == TEST_MODE_EVENT_PERFORMANCE)
		{
			if(giEventAckCount + giEventTimeoutCount > 1)
				recordEventLatency(gpulLatency+1, giEventAckCount+giEventTimeoutCount-1, pFilename);
			else
				SDNMSG(SM_ERR, "No records for event performance result.\n");
				
			free(gpulLatency);
		}
		
		free(piThreadId);
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

	SDNMSG(SM_INFO,"sdn-pub application end\n");

	terminateSDN();

	return 0;
}



template<typename T, typename U> int do_publish(char* topic_name)
{
	int send_count = 0;
	int err_num = 0;
	int pos = 0;
	int wait_pos = 0;
	int contents_count;
	SR_RET sr_ret;
	unsigned int seq_number = 0;

	int is_event = 0;
	int err_num_not_acknowledged;
	STATUS_ACK ack_status;
	
	setSDNStatusCallback(CallbackSDNStatus);

	/* Create a variable of type topic on which to publish*/
	T theMetaData;
	U theUserData;

	/* Create publisher object for the topic */
	SdnPublisher<T, U> thePublisher;

	/* Set publisher for the topic variable */
	sr_ret = thePublisher.setPublisher(theMetaData, gbEnableLoopback);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_ERR,"thePublisher.setPublisher() failed. sr_ret=%d\n", sr_ret);
		return -1; 
	}

	/* For packet delay test */
	if(giTimeStampDelay)
		thePublisher.m_usDelay = giTimeStampDelay;

	/* For packet delay test */
	if(giTimeStampLead)
		thePublisher.m_usDelay = -giTimeStampLead;

	if(theMetaData.isEvent())
	{
		is_event = 1;
		err_num_not_acknowledged = 0;

		if(giEventTimeout > 0)
			thePublisher.setEventAckTimeout(giEventTimeout);

		// CallbackAckStatus should be separated for each topic.
		// This program uses common callback for simple test.
		thePublisher.setEventAckCallback(CallbackAckStatus);
	}
	
	/* Wait until SDN status is ready */
	while(!isSDNStatusReady())
	{
		LOCK(g_mutexSdnStatus);
		pthread_cond_wait(&g_condSdnStatus, &g_mutexSdnStatus);
		UNLOCK(g_mutexSdnStatus);
	}

	/* Initiate publisher loop */
	/* copying payload in SdnPacket is in latency measure loop */
	/* coping data into payload itself is not in latency measure loop */

	for (int i=0; i<giPacketCount; ++i)
	{
		/* For packet loss test */
		if(giSkipPacket[pos] != -1)
		{
			if((i+1) == giSkipPacket[pos])
			{
				++pos;
				--send_count;
				thePublisher.m_noSend = 1;
			}
		}

		if(giWaitPacket[wait_pos] != -1)
		{
			if((i+1) == giWaitPacket[wait_pos])
			{
				++wait_pos;
				usleep(giWaitDelay);
			}
		}

		if(giSendDelay >= 0)
		{
			usleep(giSendDelay);
		}

		/* make payload different in each packet (for data sanity check) */
		if(gbFillContents)
		{
			contents_count = sizeof(theUserData.actuator1);
			for(int j=0; j<contents_count; ++j)
			{
				theUserData.actuator1[j] = (unsigned char)(i+j);
			}
		}

		if(is_event && (i != 0))
		{
			sr_ret = thePublisher.getEventAckStatus(seq_number, ack_status);
			if((sr_ret != SR_OK) || (ack_status != STATUS_ACK_ACKNOWLEDGED))
				++err_num_not_acknowledged;
		}
		
		/* Publish topic data */
		sr_ret = thePublisher.publish(theUserData, &seq_number);
		if(sr_ret != SR_OK)
			++err_num;

		/* For packet loss test */
		if(giSkipPacket[pos] != -1)
		{
			thePublisher.m_noSend = 0;
		}

		++send_count;

		// print every 1024 count to see if it is running (it is helpful when large iterations)
		if((send_count & 0x3ff) == 0)
			SDNMSG(SM_LOG,"count = %d\n", send_count);
	}

	if(!is_event)
	{
		if(err_num == 0)
			SDNMSG(SM_RESULT,"%d packets are published for %s successfully.\n", send_count, topic_name ? topic_name : "null");
	 	else
			SDNMSG(SM_RESULT,"%d packets are published for %s with %d errors.\n", send_count, topic_name ? topic_name : "null", err_num);
	}
	else
	{
		if(giSendDelay >= 0)
			usleep(giSendDelay*2);

		sr_ret = thePublisher.getEventAckStatus(seq_number, ack_status);
		if((sr_ret != SR_OK) || (ack_status != STATUS_ACK_ACKNOWLEDGED))
			++err_num_not_acknowledged;
		
		if((err_num == 0) && (err_num_not_acknowledged == 0))
			SDNMSG(SM_RESULT,"%d events are published for %s successfully.\n", send_count, topic_name ? topic_name : "null");
		else
			SDNMSG(SM_RESULT,"%d events are published for %s with publish_errors[%d], not acknowledged[%d] before next publish\n", 
				send_count, topic_name ? topic_name : "null", err_num, err_num_not_acknowledged);
	}

	thePublisher.unsetPublisher();

	unsetSDNStatusCallback();

	return err_num;
}



void* publish_thread (void* param) 
{
	char* topic_name = (char*)param;
	
	SDNMSG(SM_LOG,"Publishing thread %s called\n", topic_name ? topic_name : "null");

	if(strcmp(topic_name, "topic1_test") == 0)
		do_publish<topic1_test_metadata, topic1_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic2_test") == 0)
		do_publish<topic2_test_metadata, topic2_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic3_test") == 0)
		do_publish<topic3_test_metadata, topic3_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic1e_test") == 0)
		do_publish<topic1e_test_metadata, topic1e_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic2e_test") == 0)
		do_publish<topic2e_test_metadata, topic2e_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic3e_test") == 0)
		do_publish<topic3e_test_metadata, topic3e_test_userdata>(topic_name);
	else if(strcmp(topic_name, "topic1_test_modified") == 0)
		do_publish<topic1_test_modified_metadata, topic1_test_modified_userdata>(topic_name);
	else
	{
		SDNMSG(SM_ERR,"no matched topic name [%s] found.\n", topic_name);
		pthread_exit(NULL); 
		return (void*)NULL; 
	}

	pthread_exit(NULL); 
	return (void*)NULL; 
}



template<typename T, typename U, typename LT, typename LU> int do_loopback(char* p_address)
{
	int send_count = 0;
	int publish_err_num = 0;
	int subscribe_err_num = 0;
	SR_RET sr_ret;
#ifndef IMPROVE_ASYNC_PERFORMANCE
	struct timespec* p_sendtime;
	struct timespec* p_recvtime;
	int loop_count;
#endif	
	
	T theMetaData;
	U theUserData;
	LT theMetaDatal;
	LU theUserDatal;
	SdnPublisher<T,U> thePublisher;
	SdnSubscriber<LT,LU> theSubscriberl;

	p_sendtime = (struct timespec*)calloc(1, giPacketCount*sizeof(struct timespec));
	if(p_sendtime == NULL)
	{
		SDNMSG(SM_FATAL,"Out of memory for p_sendtime.\n");
		return -1;
	}

	p_recvtime = (struct timespec*)calloc(1, giPacketCount*sizeof(struct timespec));
	if(p_recvtime == NULL)
	{
		SDNMSG(SM_FATAL,"Out of memory for p_recvtime.\n");
		free(p_sendtime);
		return -1;
	}
	
	setSDNStatusCallback(CallbackSDNStatus);

	/* Set for the topic variable */
	sr_ret = thePublisher.setPublisher(theMetaData, gbEnableLoopback);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_FATAL,"thePublisher.setPublisher() failed. sr_ret=%d\n", sr_ret);
		free(p_sendtime);
		free(p_recvtime);
		return -1; 
	}

	sr_ret = theSubscriberl.setSubscriber(theMetaDatal);
	if(sr_ret != SR_OK)
	{
		SDNMSG(SM_FATAL,"theSubscriberl.setSubscriber() failed. sr_ret=%d\n", sr_ret);
		free(p_sendtime);
		free(p_recvtime);
		return -1; 
	}

	if(giAsyncMode == 1)
	{
		sem_init(&sem_perf_test, 0, 0);
		theSubscriberl.setReceiveCallback(CallbackAsyncReceivePerfTest);
	}

	/* Wait until SDN status is ready */
	while(!isSDNStatusReady())
	{
		LOCK(g_mutexSdnStatus);
		pthread_cond_wait(&g_condSdnStatus, &g_mutexSdnStatus);
		UNLOCK(g_mutexSdnStatus);
	}

	/* Publish / Subscribe */
	for (loop_count=0; loop_count<giPacketCount; ++loop_count)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &p_sendtime[loop_count]);
		
		/* Publish topic data */
		sr_ret = thePublisher.publish(theUserData);
		if(sr_ret != SR_OK)
			++publish_err_num;

		/* Receive topic data */
		if(giAsyncMode != 1)
		{
			sr_ret = theSubscriberl.receive(theUserDatal); 
			if(sr_ret != SR_OK)
				++subscribe_err_num;
		}
		else
		{
			sem_wait(&sem_perf_test);
		}

#ifdef IMPROVE_ASYNC_PERFORMANCE
		if(giAsyncMode != 1)
			clock_gettime(CLOCK_MONOTONIC_RAW, &p_recvtime[loop_count]);
#else
		clock_gettime(CLOCK_MONOTONIC_RAW, &p_recvtime[loop_count]);
#endif

		++send_count;

		// print every 1024 count to see if it is running (it is helpful when large iterations)
		if((send_count & 0x3ff) == 0)
			SDNMSG(SM_LOG,"count = %d\n", send_count);
	}


	if(giAsyncMode == 1)
		theSubscriberl.unsetReceiveCallback();

	sr_ret = thePublisher.unsetPublisher();
	if(sr_ret != SR_OK)
		SDNMSG(SM_ERR,"thePublisher.unsetPublisher() failed. sr_ret=%d\n", sr_ret);
	
	sr_ret = theSubscriberl.unsetSubscriber();
	if(sr_ret != SR_OK)
		SDNMSG(SM_ERR,"theSubscriberl.unsetSubscriber() failed. sr_ret=%d\n", sr_ret);

	if((publish_err_num == 0) && (subscribe_err_num == 0))
		SDNMSG(SM_RESULT,"%d packets are looped back successfully.\n", send_count);
	else
		SDNMSG(SM_RESULT,"%d packets are looped back with %d(p), %d(s) errors.\n", send_count, publish_err_num, subscribe_err_num);

	unsetSDNStatusCallback();

	// discard the first result because it may take more time to resolve address
	recordLatency(p_sendtime + 1, p_recvtime + 1, giPacketCount - 1, theMetaData.getPayloadsize(), theMetaData.getName());

	free(p_sendtime);
	free(p_recvtime);
	
	return 0; 
}



void* loopback_thread (void* param) 
{
	char* p_address = (char*)param;
	char* topic_name[2];
	char* next_ptr;

	SDNMSG(SM_LOG,"Publishing loopback thread %s called\n", p_address ? p_address : "null");

	topic_name[0] = strtok_r(p_address, ",", &next_ptr);
	if(topic_name[0] == NULL)
	{
		SDNMSG(SM_FATAL,"topic_name is invalid.\n");
		pthread_exit(NULL); 
		return (void*)NULL; 
	}
		
	topic_name[1] = strtok_r(NULL, ",", &next_ptr);
	if(topic_name[1] == NULL)
	{
		SDNMSG(SM_FATAL,"2nd topic_name is invalid.\n");
		pthread_exit(NULL); 
		return (void*)NULL; 
	}

	if((strcmp(topic_name[0], "topic1_test") == 0) && 
		(strcmp(topic_name[1], "topic1l_test") == 0))
	{
		do_loopback<topic1_test_metadata, topic1_test_userdata, topic1l_test_metadata, topic1l_test_userdata>((char*)param);
	}
	else if((strcmp(topic_name[0], "topic2_test") == 0) && 
		(strcmp(topic_name[1], "topic2l_test") == 0))
	{
		do_loopback<topic2_test_metadata, topic2_test_userdata, topic2l_test_metadata, topic2l_test_userdata>((char*)param);
	}
	else if((strcmp(topic_name[0], "topic3_test") == 0) && 
		(strcmp(topic_name[1], "topic3l_test") == 0))
	{
		do_loopback<topic3_test_metadata, topic3_test_userdata, topic3l_test_metadata, topic3l_test_userdata>((char*)param);
	}
	else
	{
		SDNMSG(SM_FATAL,"Invalid parameter. last parameter format should be \"topic[1,2,3]_test,topic[1l,2l,3l]_test\".\n");
	}
		
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



template <typename topic_metadata, typename topic_userdata>
SR_RET CallbackAsyncReceivePerfTest(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)
{
	int ret = SR_OK;
	
#ifdef IMPROVE_ASYNC_PERFORMANCE
	clock_gettime(CLOCK_MONOTONIC_RAW, &p_recvtime[loop_count]);
#endif

	sem_post(&sem_perf_test);

	if(err_code != SR_OK)
	{
		ret = err_code;
		SDNMSG(SM_ERR,"[CallbackAsyncReceivePerfTest] error code [%d]\n", err_code);
	}

	return ret;
}


void CallbackAckStatus(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec)
{
	if(ack_status == STATUS_ACK_TIMEOUT)
	{
		if(giEventTimeoutCount < 10)
			SDNMSG(SM_ERR,"Timeout for event acknowledgement for seqnumber[%d], elapsed[%d]\n", seq_num, elapsed_usec);
		else 
		{
			if(giEventTimeoutCount == 10)
				SDNMSG(SM_ERR,"++++++++++++++++++++++++++++++++++++++++++++++++\nMore than 10 timeout events will be printed as SM_LOG level.\n++++++++++++++++++++++++++++++++++++++++++++++++\n");
			
			SDNMSG(SM_LOG,"Timeout for event acknowledgement for seqnumber[%d], elapsed[%d]\n", seq_num, elapsed_usec);
		}

		if(giTestMode == TEST_MODE_EVENT_PERFORMANCE)
			gpulLatency[giEventTimeoutCount + giEventAckCount] = elapsed_usec;
		
		giEventTimeoutCount++;
	}
	else if(ack_status == STATUS_ACK_ACKNOWLEDGED)
	{
		if(giTestMode == TEST_MODE_EVENT_PERFORMANCE)
			gpulLatency[giEventTimeoutCount + giEventAckCount] = elapsed_usec;
		
		giEventAckCount++;
	}
}


void recordLatency(struct timespec* sendtime, struct timespec* roundtriptime, unsigned int iterations, 
	unsigned short sdnpacketsize, const char*outputfile)
{

	/* Record latency data into a file */
	char result_filename[512]; 
	float sum=0.0; 
	float latency=0; 
	long sec;
	FILE* fp; 
	float min=0;
	float max=0;
	float square_sum=0.0; 

	sprintf(result_filename, "/tmp/perf_%s_%d.result", outputfile, sdnpacketsize);

	SDNMSG(SM_RESULT,"result filename is %s\n", result_filename );
	if((fp = fopen(result_filename, "w+"))==NULL)
	{
		SDNMSG(SM_ERR,"file open error\n");
		return;
	}

	fprintf(fp, "SDN packet size %dbytes\n", sdnpacketsize);
	fprintf(fp, "Send Time(s)\tSendTime(ns)\tRTT(s)\tRTT(ns)\tLatency(us)(RTT-SendTime)/2\n");

	for (unsigned int i=0; i<iterations; i++) {

		sec = roundtriptime->tv_sec - sendtime->tv_sec;
		latency = (float)((sec*1000000000 + roundtriptime->tv_nsec - sendtime->tv_nsec)) / 2000 ;
		fprintf(fp, "%ld\t%09ld\t%ld\t%09ld\t%.04f\n", sendtime->tv_sec, sendtime->tv_nsec, 
				roundtriptime->tv_sec, roundtriptime->tv_nsec, latency);

		sum=sum + latency; 
		square_sum = square_sum + (latency*latency);

		roundtriptime++; 
		sendtime++; 

		if(i == 0)
		{
			min = latency;
			max = latency;
		}
		else
		{
			if(latency < min)
				min = latency;

			if(latency > max)
				max = latency;
		}
	}
	
	fprintf(fp, "average latency is %f usec\n", sum / iterations); 
	fprintf(fp, "  - min(%f), max(%f), sdev(%f)\n", min, max, sqrt(fabsf((square_sum / iterations) - (sum / iterations)*(sum / iterations)))); 
	fclose(fp);
}


void recordEventLatency(unsigned long* p_latency, unsigned int iterations, const char*outputfile)
{
	/* Record latency data into a file */
	char result_filename[512]; 
	unsigned long sum=0.0; 
	unsigned long latency=0; 
	FILE* fp; 
	unsigned long min=0;
	unsigned long max=0;
	unsigned long square_sum=0.0; 

	sprintf(result_filename, "/tmp/perf_%s.result", outputfile);

	SDNMSG(SM_RESULT,"result filename is %s\n", result_filename );
	if((fp = fopen(result_filename, "w+"))==NULL)
	{
		SDNMSG(SM_ERR,"file open error\n");
		return;
	}

	fprintf(fp, "Latency(us)\n");

	for (unsigned int i=0; i<iterations; i++) 
	{
		latency = p_latency[i];
		sum=sum + latency; 
		square_sum = square_sum + (latency*latency);

		fprintf(fp, "%ld\n", latency);

		if(i == 0)
		{
			min = latency;
			max = latency;
		}
		else
		{
			if(latency < min)
				min = latency;

			if(latency > max)
				max = latency;
		}
	}
	
	fprintf(fp, "average latency is %f usec\n", (float)sum / iterations); 
	fprintf(fp, "  - min(%d), max(%d), sdev(%f)\n", (int)min, (int)max, (float)sqrt(fabsf((square_sum / iterations) - (sum / iterations)*(sum / iterations)))); 
	fclose(fp);
}


