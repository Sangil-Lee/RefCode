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

using namespace std; 

// parameter '-m'(test mode)
#define	TEST_MODE_NORMAL				0

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

static char* gpDefConfigFile = NULL;
static char* gpUserConfigFile = NULL;

static pthread_mutex_t g_mutexSdnStatus; 
static pthread_cond_t g_condSdnStatus;	

void* publish_thread (void* param);
void CallbackSDNStatus(int is_ready);
void CallbackAckStatus(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec);

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
		pthread_t* piThreadId;

		piThreadId = (pthread_t*)malloc(sizeof(pthread_t*) * topic_count);
		if(piThreadId == NULL)
		{
			SDNMSG(SM_FATAL, "out of memory\n");
			sleep(1);	// wait for printing error message
			exit(1);
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

		free(piThreadId);
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

	do_publish<topic1_test_metadata, topic1_test_userdata>(topic_name);

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

		giEventTimeoutCount++;
	}
	else if(ack_status == STATUS_ACK_ACKNOWLEDGED)
	{
		giEventAckCount++;
	}
}


