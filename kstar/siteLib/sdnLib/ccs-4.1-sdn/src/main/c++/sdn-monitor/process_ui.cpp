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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string>
#include <list>
#include <vector>

#include "sdn_common/log_buffer.h"
#include "sdn_common/sdnmon_msg.h"
#include "sdn_common/sdnmon_msg_ui.h"
#include "list_node.h"
#include "list_topic.h"


using namespace std; 


void* ThreadUiManage(void* p_param);


void* ThreadUiConnect(void* p_param)
{	
	int server_sock;
	struct sockaddr_in server_addr;
	unsigned short server_port;
	int opt = 1;
	int conn_sock;
	struct sockaddr_in conn_addr;
	pthread_t tid_ui_manage;
	int ret;
	char* p_ip_addr = (char*)p_param;
	char* p_server;
	char* p_next;

	SDNMSG(SM_LOG, "ThreadUiConnect(0x%x) thread start.\n", p_param);

	while(1)
	{
		server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(server_sock < 0)
		{
			SDNMSG(SM_FATAL, "socket() error\n");
			goto error_state;
		}

		if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		{
			SDNMSG(SM_ERR, "setsockopt(SO_REUSEADDR) fail.\n");
		}
		
		p_server = getenv(SDN_MONITORING_NODE_ENVNAME) ? getenv(SDN_MONITORING_NODE_ENVNAME) : (char*)"";
		strtok_r(p_server, ":", &p_next);
		if(p_next == NULL)
			server_port = DEFAULT_SDN_MONITOR_SERVER_PORT_UI;
		else if(strlen(p_next) == 0)
			server_port = DEFAULT_SDN_MONITOR_SERVER_PORT_UI;
		else
		{
			server_port = atoi(p_next) + 1;

			strtok_r(NULL, ":", &p_next);
			if(p_next != NULL)
			{
				if(strlen(p_next) > 0)
					server_port = atoi(p_next);
			}
		}

		memset (&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(server_port); 
		if(p_ip_addr == NULL)
			server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		else if(strlen(p_ip_addr) == 0)
			server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			server_addr.sin_addr.s_addr = inet_addr(p_ip_addr);

		if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
		{
			SDNMSG(SM_FATAL, "bind() error\n");
			close(server_sock);
			goto error_state;
		}

		if(listen(server_sock, 1) < 0) 
		{
			SDNMSG(SM_FATAL, "listen() error\n");
			close(server_sock);
			goto error_state;
		}

		while(1)
		{
			conn_sock = accept(server_sock, (struct sockaddr *)&conn_addr, (unsigned int*)&conn_addr);
			if(conn_sock < 0)
			{
				if((errno == EINTR) || (errno == EAGAIN))
				{
					SDNMSG(SM_WARN, "accept() returned errno %d\n", errno);
					continue;
				}

				SDNMSG(SM_FATAL, "accept() error. errno=[%d]\n", errno);
				break;
			}
			else
			{
				SDNMSG(SM_LOG, "New UI client connected. ip=[%s]\n", inet_ntoa(conn_addr.sin_addr));

				ret = pthread_create(&tid_ui_manage, NULL, &ThreadUiManage, (void*)conn_sock);
				if(ret != 0)
				{
					SDNMSG(SM_FATAL, "ThreadUiConnect thread failed.\n");
					goto error_state;
				}
			}
		}

		close(server_sock);

error_state:
		// never exit this loop while sdn monitoring node is alive
		SDNMSG(SM_FATAL, "Unrecoverable error with ui server socket. Now reinitializing the socket!!!!!!!!\n");
		sleep(3);
		continue;
	}
	
	SDNMSG(SM_LOG, "ThreadUiConnect thread end.\n");

	return (void*)0;
}



void* ThreadUiManage(void* p_param)
{
	int sock = (int)(long)p_param;

	SDNMSG(SM_LOG, "ThreadUiManage(0x%x) thread start.\n", p_param);

	// process UI message (nyi);
	

	close(sock);

	SDNMSG(SM_LOG, "ThreadUiManage thread end.\n");

	return (void*)0;
}

