//+============================================================================
// $HeadURL$
// $Id$
//
// Project       : CODAC Core System
//
// Description   : sdn-monitor program
//
// Author        : Hyung Gi KIM
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
//-============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <string>
#include <list>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h> 
#include <linux/netdevice.h> 
#include <netdb.h>

#include "sdn_common/sdnmon_msg.h"
#include "sdn_common/log_buffer.h"
#include "list_topic.h"
#include "list_node.h"

using namespace std; 


#define	SDNMON_CONFIG_DIRECTORY_ENVNAME		"SDNMON_CONFIG_DIRECTORY"
#define	DEFAULT_SDNMON_CONFIG_DIRECTORY		"/etc/opt/codac/sdn"

#define	SDNMON_CONFIG_FILENAME				"topic_configurations.xml"


class SdnTopicDb g_csSdnTopicDb;
class SdnNodeDb g_csSdnNodeDb;

SdnLogBuf theSdnLogBuf;
int g_iLogAuditLevel;


extern void* ThreadNodeConnect(void* p_param);
extern void* ThreadUiConnect(void* p_param);
extern int ProcessCommand();
extern int ParseConfig(const char* config_filename); 

static int getInterfaceIp(char* if_name, char* ip_addr, int ip_addr_len); 


static char addrIp[INET_ADDRSTRLEN]; 

static struct option options[] = {
	{"configfile", 1, 0, 'c'},
	{"interface", 1, 0, 'i'},
	{"loglevel", 1, 0, 'l'},
	{"help", 0, 0, '?'},
	{"", 0, 0, 0}
};


static void usage(void)
{
	printf("Usage: sdn-monitor [options] [config_file1 [config_file2 ...]]\n");
	printf("  -i, --interface    specify network interface for SDN monitoring. (default:all interface)\n");
	printf("  -l, --loglevel     log level. 1(all) ~ 5\n");
	printf("  -h, --help         \n");
}




int main(int argc, char* argv[]) 
{
	int ret;
	int app_ret = 0;
	pthread_t tid_node_connect;
	pthread_t tid_ui_connect;
	string config_files;
	char* p_config_file;

	// if addrIp is empty, SDN monitoring node accept connection from all interface card
	addrIp[0] = 0;
	
	if ( argc > 1 )
	{
		int opt_index = 0;
		int opt = 0;
		
		while ((opt = getopt_long(argc, argv, "l:i:h:?", options, &opt_index)) != -1)
		{
			switch(opt)
			{
			case 'l':
				theSdnLogBuf.setLogLevel(atoi(optarg));
				break;
				
			case 'i':
				if(getInterfaceIp(optarg, addrIp, INET_ADDRSTRLEN) != 0)
					addrIp[0] = 0;
				break;
			
			case '?':
			default:
				usage();
				exit(0);
			}
		}
	}

	// log thread start
	theSdnLogBuf.initialize();

	// initialize g_csSdnTopicDb from SDD configuration file
	if(optind < argc)
	{
		while(optind < argc)
		{
			p_config_file = argv[optind++];
			ret = ParseConfig(p_config_file);
			if(ret != 0)
				SDNMSG(SM_ERR, "config file[%s] parsing failed.\n", p_config_file);
		}
	}
	else
	{
		config_files = getenv(SDNMON_CONFIG_DIRECTORY_ENVNAME) ? getenv(SDNMON_CONFIG_DIRECTORY_ENVNAME) : "";
		if(strlen(config_files.c_str()) == 0)
			config_files = DEFAULT_SDNMON_CONFIG_DIRECTORY;
		config_files += "/";
		config_files += SDNMON_CONFIG_FILENAME;
		
		p_config_file = (char*)config_files.c_str();
		ret = ParseConfig(p_config_file);
		if(ret != 0)
		{
			SDNMSG(SM_ERR, "default config file[%s] is not valid.\n", p_config_file);
			app_ret = -1;
			goto app_exit;
		}
	}
		

	ret = pthread_create(&tid_node_connect, NULL, &ThreadNodeConnect, addrIp);
	if(ret != 0)
	{
		SDNMSG(SM_ERR, "ThreadNodeConnect thread failed.\n");
		app_ret = -1;
		goto app_exit;
	}

	ret = pthread_create(&tid_ui_connect, NULL, &ThreadUiConnect, addrIp);
	if(ret != 0)
	{
		SDNMSG(SM_ERR, "ThreadUiConnect thread failed.\n");
		app_ret = -1;
		goto app_exit;
	}

#if 1
	ProcessCommand();
#else
	pthread_join(tid_node_connect, NULL);
	pthread_join(tid_ui_connect, NULL);
#endif

app_exit:
	// wait until log buffer is empty
	theSdnLogBuf.deinitialize();
	
	return app_ret;
}



static int getInterfaceIp(char* if_name, char* ip_addr, int ip_addr_len) 
{ 
	int s; 
	struct ifconf ifconf; 
	struct ifreq ifr[100]; 
	int ifs; 
	int i; 
	char ip[INET_ADDRSTRLEN]; 
	struct sockaddr_in* s_in; 

	s = socket(AF_INET, SOCK_DGRAM, 0); 
	if (s < 0) 
	{ 
		SDNMSG(SM_ERR, "socket() failed.\n"); 
		return 1; 
	} 

	ifconf.ifc_buf = (char*)ifr; 
	ifconf.ifc_len = sizeof ifr; 

	if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) 
	{ 
		SDNMSG(SM_ERR, "ioctl() failed.\n"); 
		close(s); 
		return 2; 
	} 

	ifs = ifconf.ifc_len / sizeof(ifr[0]); 
	SDNMSG(SM_LOG, "interfaces = %d\n", ifs); 
	
	for (i = 0; i < ifs; i++) 
	{ 
		s_in = (struct sockaddr_in*)&ifr[i].ifr_addr; 

		if(strcmp(if_name, ifr[i].ifr_name) != 0)
			continue;
			
		if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip))) 
		{ 
			SDNMSG(SM_ERR, "inet_ntop() failed.\n"); 
			close(s); 
			return 3; 
		} 

		break;
	}

	close(s); 

	if(i >= ifs)
	{
		SDNMSG(SM_ERR, "no matched interface for [%s]\n", if_name); 
		return 4;
	}

	SDNMSG(SM_LOG, "matched: [%s] => [%s]\n", if_name, ip);

	if((int)strlen(ip) >= ip_addr_len)
	{
		SDNMSG(SM_LOG, "matched: [%s] => [%s]\n", if_name, ip);
	}
		
	strncpy(ip_addr, ip, ip_addr_len-1);

	return 0;
}




