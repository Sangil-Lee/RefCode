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
#include "sdn_common/sdnmon_msg_reg.h"
#include "list_node.h"
#include "list_topic.h"

using namespace std; 


#define	SOCKET_SELECT_TIMEOUT		3	
#define	MAX_RECV_BUF_SIZE			1500



static void* ThreadNodeManage(void* p_param);
static int ProcessRegisterMessage(NodeInfo* p_node_info, char* message);
static int SendNodeMessage(int sock, char* p_send_message);
static int SendOneKeyRegisterMessage(int sock, const char* p_key, const char* p_value);
static int SendTwoKeysRegisterMessage(int sock, const char* p_key1, const char* p_value1, const char* p_key2, const char* p_value2);
static int SendPublisherInfo(int sock, TopicInfo* p_topic_info);
static int SendSubscriberList(int sock, TopicInfo* p_topic_info);
static int SendTopicStatus(int sock, TopicInfo* p_topic_info);
static int unregisterNodeInfoInTopicDb(NodeInfo* p_node_info);
static int updateStatusSdn();


void* ThreadNodeConnect(void* p_param)
{	
	int server_sock;
	struct sockaddr_in server_addr;
	unsigned short server_port;
	int opt = 1;

	int conn_sock;
	struct sockaddr_in conn_addr;
	unsigned int conn_addr_size;
	char* p_ip_addr = (char*)p_param;
	pthread_t tid_node_manage;
	int ret;
	char* p_server;
	char* p_next;

	SDNMSG(SM_LOG, "ThreadNodeConnect(0x%x) thread start.\n", p_param);

	ret = pthread_create(&tid_node_manage, NULL, &ThreadNodeManage, NULL);
	if(ret != 0)
	{
		SDNMSG(SM_ERR, "ThreadNodeManage thread failed. ThreadNodeConnect exits.\n");
		return (void*)0;
	}


	while(1)
	{
		server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(server_sock < 0)
		{
			SDNMSG(SM_ERR, "socket() error\n");
			goto error_state;
		}

		if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		{
			SDNMSG(SM_ERR, "setsockopt(SO_REUSEADDR) fail.\n");
		}
		
		p_server = getenv(SDN_MONITORING_NODE_ENVNAME) ? getenv(SDN_MONITORING_NODE_ENVNAME) : (char*)"";
		strtok_r(p_server, ":", &p_next);
		if(p_next == NULL)
			server_port = DEFAULT_SDN_MONITOR_SERVER_PORT_REG;
		else if(strlen(p_next) == 0)
			server_port = DEFAULT_SDN_MONITOR_SERVER_PORT_REG;
		else
			server_port = atoi(p_next);
			
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
			SDNMSG(SM_ERR, "bind() error\n");
			close(server_sock);
			goto error_state;
		}

		if(listen(server_sock, 1) < 0) 
		{
			SDNMSG(SM_ERR, "listen() error\n");
			close(server_sock);
			goto error_state;
		}

		while(1)
		{
			conn_addr_size = sizeof(conn_addr);
			conn_sock = accept(server_sock, (struct sockaddr *)&conn_addr, &conn_addr_size);
			if(conn_sock < 0)
			{
				if((errno == EINTR) || (errno == EAGAIN))
				{
					SDNMSG(SM_WARN, "accept() returned errno %d\n", errno);
					continue;
				}

				SDNMSG(SM_ERR, "accept() error. errno=[%d]\n", errno);
				break;
			}
			else
			{
				SDNMSG(SM_INFO, "New node connected. ip=[%s]\n", inet_ntoa(conn_addr.sin_addr));
				
				g_csSdnNodeDb.addNodeInfo(conn_sock);
			}
		}

		close(server_sock);

error_state:
		// never exit this loop while sdn monitoring node is alive
		SDNMSG(SM_FATAL, "Unrecoverable error with register server socket. Now reinitializing the socket!!!!!!!!\n");
		sleep(3);
		continue;
	}
	
	SDNMSG(SM_LOG, "ThreadNodeConnect thread end.\n");

	return (void*)0;
}



static void* ThreadNodeManage(void* p_param)
{	
	int max_fd;
	int cur_fd;
	fd_set fd_recv;
	struct timeval timeout;
	int ret;
	NodeInfo* p_node_info;
	list<NodeInfo*>::iterator node_info_iter;
	list<NodePubsubInfo*>::iterator node_pubsub_info_iter;
	char recv_buf[MAX_RECV_BUF_SIZE];

	SDNMSG(SM_LOG, "ThreadNodeManage(0x%x) thread start.\n", p_param);

	while(1)
	{
		FD_ZERO(&fd_recv);
		max_fd = -1;
		
		node_info_iter = g_csSdnNodeDb.m_listNodeInfo.begin();
		while(node_info_iter != g_csSdnNodeDb.m_listNodeInfo.end())
		{
			cur_fd = (*node_info_iter)->m_socket;

			FD_SET(cur_fd, &fd_recv);

			if(max_fd < cur_fd)
				max_fd = cur_fd;

			node_info_iter++;
		}

		// no connection
		if(max_fd < 0)
		{
			sleep(1);
			continue;
		}
		
		timeout.tv_sec = SOCKET_SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		ret = select(max_fd+1, &fd_recv, NULL, NULL, &timeout);
		if(ret < 0)
		{
			if((errno == EINTR) || (errno == EAGAIN))
			{
				SDNMSG(SM_WARN, "select() returned errno %d\n", errno);
				continue;
			}

			// never exit this loop while sdn monitoring node is alive
			SDNMSG(SM_FATAL, "select() error. errno=[%d]!!!!!!!!!!!!!\n", errno);
			sleep(3);
			continue;
		}
		else if (ret == 0)
		{
			continue;
		}
		else
		{
			node_info_iter = g_csSdnNodeDb.m_listNodeInfo.begin();
			while(node_info_iter != g_csSdnNodeDb.m_listNodeInfo.end())
			{
				p_node_info = *node_info_iter;
				cur_fd = p_node_info->m_socket;
				
				if(FD_ISSET(cur_fd, &fd_recv))
				{
					ret = recv(cur_fd, recv_buf, MAX_RECV_BUF_SIZE, 0);
					if(ret <= 0)	// socket closed or error
					{
						if((errno == EINTR) || (errno == EAGAIN))
						{
							SDNMSG(SM_LOG, "recv() interrupted. errno=%d\n", errno);
							node_info_iter++;
							continue;
						}
						
						close(cur_fd);

						SDNMSG(SM_INFO, "Connection closed. Unregister node. host[%s], app[%s], ip[%s]\n", 
							p_node_info->m_strHostName.c_str(), p_node_info->m_strAppName.c_str(), p_node_info->m_strIpAddr.c_str());
						
						unregisterNodeInfoInTopicDb(p_node_info);

						p_node_info->removeAllNodePubsubInfo();
						delete(p_node_info);
						node_info_iter = g_csSdnNodeDb.m_listNodeInfo.erase(node_info_iter);

						continue;
					}
					else
					{
						char* cur_ptr;
						char* next_ptr;
						char* end_ptr;

						SDNMSG(SM_LOG, "[RECV(%d)]\n%s", strlen(recv_buf), recv_buf);
						
						cur_ptr = recv_buf;
						end_ptr = recv_buf + strlen(recv_buf);

						while(cur_ptr < end_ptr)
						{
							next_ptr = strstr(cur_ptr, DELIM_KEY_END);
							if(next_ptr == NULL)
								break;

							if(next_ptr + 1 < end_ptr)
							{
								*(next_ptr + 1) = 0;
								// Parse and process message
								ProcessRegisterMessage(p_node_info, recv_buf);
							}

							cur_ptr = next_ptr + 2;
						}
					}
				}

				node_info_iter++;
			}
		}
	}

	
	SDNMSG(SM_LOG, "ThreadNodeManage thread end.\n");

	return (void*)0;
}



static int ProcessRegisterMessage(NodeInfo* p_node_info, char* message)
{
	ParseMessage parse_message;
	ConstructMessage construct_message;

	KeyValue* p_kv;
	KeyValue* p_kv_command;
	KeyValue kv;
	list<KeyValue> list_kv;

	TopicInfo* p_topic_info;
	TopicPubsubInfo* p_topic_pubsub_info;

	char* cur_ptr;
	char* next_ptr;
	int version_major;
	int version_minor;


	if((p_node_info == NULL) || (message == NULL))
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}
	
	parse_message.parse(message);

	// check protocol version
	p_kv = parse_message.getKey(NULL, KEY_PROTOCOL_VERSION);
	if(p_kv == NULL)
	{
		SDNMSG(SM_ERR, "no KEY_PROTOCOL_VERSION in the message.\n");
		return -1;
	}

	cur_ptr = strtok_r((char*)p_kv->m_strValue.c_str(), ".", &next_ptr);
	version_major = (cur_ptr == NULL) ? 0 : atoi(cur_ptr);	
	version_minor = (next_ptr == NULL) ? 0 : atoi(next_ptr);
	if(version_major != VALUE_PROTOCOL_VERSION_MAJOR)
	{
		SDNMSG(SM_ERR, "protocol version(%d.%d) does not consistent with current version(%d.%d)\n",
			version_major, version_minor, VALUE_PROTOCOL_VERSION_MAJOR, VALUE_PROTOCOL_VERSION_MINOR);
		return -1;
	}

	// check if message is for register protocol
	p_kv = parse_message.getKey(NULL, KEY_MESSAGE_GROUP);
	if(p_kv == NULL)
	{
		SDNMSG(SM_ERR, "no KEY_MESSAGE_GROUP in the message.\n");
		return -1;
	}

	if(p_kv->m_strValue.compare(VALUE_MESSAGE_GROUP_REGISTER) != 0)
	{
		SDNMSG(SM_ERR, "KEY_MESSAGE_GROUP is not VALUE_MESSAGE_GROUP_REGISTER.\n");
		return -1;
	}

	// process register message
	p_kv_command = parse_message.getKey(NULL, KEY_CMD);

	if(p_kv_command == NULL)
	{
		SDNMSG(SM_ERR, "KEY_CMD does not exist in the register message.\n");
		return -1;
	}

	if(p_kv_command->m_strValue.compare(VAL_CMD__NODE_REG) == 0)
	{
		if(p_node_info->getStatusNode() == STATUS_NODE_CONNECTED)
		{
			KeyValue* p_kv_param;
			KeyValue* p_kv_host_name;
			KeyValue* p_kv_app_name;
			KeyValue* p_kv_ip;
			
			p_kv_param = parse_message.getKey(NULL, KEY_PARAM);
			if(p_kv_param == NULL)
			{
				SDNMSG(SM_ERR, "KEY_PARAM does not exist in VAL_CMD__NODE_REG message.\n");
				return -1;
			}
			
			p_kv_host_name = parse_message.getKey(&p_kv_param->m_listKeyValue, KEY_PARAM_HOSTNAME);
			p_kv_app_name = parse_message.getKey(&p_kv_param->m_listKeyValue, KEY_PARAM_APPNAME);
			p_kv_ip = parse_message.getKey(&p_kv_param->m_listKeyValue, KEY_PARAM_IP);
			if((p_kv_host_name == NULL) || (p_kv_app_name == NULL) || (p_kv_ip == NULL))
			{
				SDNMSG(SM_ERR, "invalid null parameter for NODE_REG. hostname, appname and ip are mandatory.\n");
				return -1;
			}
			
			// set hostname, appname and ip_addr in NodeInfo
			p_node_info->m_strHostName = p_kv_host_name->m_strValue;
			p_node_info->m_strAppName = p_kv_app_name->m_strValue;
			p_node_info->m_strIpAddr = p_kv_ip->m_strValue;

			p_node_info->setStatusNode(STATUS_NODE_REGISTERED);

			SendOneKeyRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__NODE_REG_ACK);

			SDNMSG(SM_INFO, "Node is registered. host[%s], app[%s], ip[%s] \n", 
				p_node_info->m_strHostName.c_str(), p_node_info->m_strAppName.c_str(), p_node_info->m_strIpAddr.c_str());
		}
		else
		{
			SDNMSG(SM_ERR, "NODE_REG failed. Node is not in connect state. current state=%d\n", p_node_info->getStatusNode());
			SendOneKeyRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__NODE_REG_NACK);
			return -1;
		}
	}
	else if(p_kv_command->m_strValue.compare(VAL_CMD__NODE_UNREG) == 0)
	{
		if(p_node_info->getStatusNode() == STATUS_NODE_REGISTERED)
		{
			SDNMSG(SM_INFO, "Node is unregistered. host[%s], app[%s], ip[%s] \n", p_node_info->m_strHostName.c_str(),
				p_node_info->m_strAppName.c_str(), p_node_info->m_strIpAddr.c_str());

			unregisterNodeInfoInTopicDb(p_node_info);
			p_node_info->removeAllNodePubsubInfo();

			p_node_info->setStatusNode(STATUS_NODE_CONNECTED);

			SendOneKeyRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__NODE_UNREG_ACK);

			updateStatusSdn();
		}
		else
		{
			SDNMSG(SM_ERR, "NODE_UNREG failed. Node is not registered\n");
			SendOneKeyRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__NODE_UNREG_NACK);
			return -1;
		}
	}
	else if(p_kv_command->m_strValue.compare(VAL_CMD__PUB_REG) == 0)
	{
		KeyValue* p_kv_metadata;
		
		KeyValue* p_kv_topicname;
		KeyValue* p_kv_topicdesc;
		KeyValue* p_kv_topicver;
		KeyValue* p_kv_payloadsize;
		KeyValue* p_kv_isevent;
		int is_event;
		
		p_kv_metadata = parse_message.getKey(NULL, KEY_META);
		if(p_kv_metadata == NULL)
		{
			SDNMSG(SM_ERR, "KEY_META does not exist in VAL_CMD__PUB_REG message.\n");
			return -1;
		}
		
		p_kv_topicname = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_TOPICNAME);
		p_kv_topicdesc = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_TOPICDESC);
		p_kv_topicver = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_TOPICVER);
		p_kv_payloadsize = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_PAYLOADSIZE);
		if((p_kv_topicname == NULL) || (p_kv_topicdesc == NULL) || (p_kv_topicver == NULL) || (p_kv_payloadsize == NULL))
		{
			SDNMSG(SM_ERR, "invalid null parameter for metadata. tname, tdesc, tver, plsize and event are mandatory.\n");
			return -1;
		}

		p_kv_isevent = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_ISEVENT);
		if(p_kv_isevent != NULL)
			is_event = atoi(p_kv_isevent->m_strValue.c_str());
		else 
			is_event = 0;
		
		if(p_node_info->getStatusNode() == STATUS_NODE_REGISTERED)
		{
			// Check if register information is valid
			p_topic_info = g_csSdnTopicDb.getTopicInfo((char*)p_kv_topicname->m_strValue.c_str());
			if(p_topic_info == NULL)
			{
				SDNMSG(SM_ERR, "PUB_REG failed. Topic[%s] is not registered\n", p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			if(p_topic_info->m_csPubInfo.getStatusTopicPubsub() == STATUS_TOPIC_PUBSUB_REGISTERED)
			{
				SDNMSG(SM_ERR, "PUB_REG failed. Publisher is already registered\n");
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			// set p_topic_info->m_csMetadata.m_payloadSize if this is the first registered node in the topic list
			if(p_topic_info->getNumberOfRegisteredPubsub() == 0)
				p_topic_info->m_csMetadata.m_payloadSize = atoi(p_kv_payloadsize->m_strValue.c_str());

			if((p_topic_info->m_csPubInfo.m_strHostName.compare(p_node_info->m_strHostName.c_str()) != 0)
				|| (p_topic_info->m_csPubInfo.m_strAppName.compare(p_node_info->m_strAppName.c_str()) != 0)
				|| (p_topic_info->m_csMetadata.m_version != atoi(p_kv_topicver->m_strValue.c_str()))
				|| (p_topic_info->m_csMetadata.m_bEvent != is_event)
				|| (p_topic_info->m_csMetadata.m_payloadSize != atoi(p_kv_payloadsize->m_strValue.c_str())))
			{
				SDNMSG(SM_ERR, "PUB_REG failed. Received info is different from registered.\n");
				SDNMSG(SM_ERR, "DB for [%s]:\n  host_name[%s], app_name[%s], version[%d], payload_size[%d], b_event[%d]\n",
					p_kv_topicname->m_strValue.c_str(), 
					p_topic_info->m_csPubInfo.m_strHostName.c_str(), p_topic_info->m_csPubInfo.m_strAppName.c_str(),
					p_topic_info->m_csMetadata.m_version, p_topic_info->m_csMetadata.m_payloadSize, p_topic_info->m_csMetadata.m_bEvent);
				SDNMSG(SM_ERR, "Received:\n  host_name[%s], app_name[%s], version[%d], payload_size[%d], b_event[%d]\n",
					p_node_info->m_strHostName.c_str(), p_node_info->m_strAppName.c_str(),
					atoi(p_kv_topicver->m_strValue.c_str()), atoi(p_kv_payloadsize->m_strValue.c_str()), is_event);
				
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
			
			// add pubsub info in g_csSdnNodeDb
			if(p_node_info->addNodePubsubInfo((char*)p_kv_topicname->m_strValue.c_str(), 1) != 0)
			{
				SDNMSG(SM_ERR, "PUB_REG failed. addNodePubsubInfo() failed.\n");
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
			
			// update pubsub info in g_csSdnTopicDb
			p_topic_info->m_csPubInfo.m_strIpAddr = p_node_info->m_strIpAddr;
			p_topic_info->m_csPubInfo.setStatusTopicPubsub(STATUS_TOPIC_PUBSUB_REGISTERED);
			
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_REG_ACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());

			SDNMSG(SM_INFO, "Publisher is registered. host[%s], app[%s], ip[%s] \n", p_topic_info->m_csPubInfo.m_strHostName.c_str(),
				p_topic_info->m_csPubInfo.m_strAppName.c_str(), p_topic_info->m_csPubInfo.m_strIpAddr.c_str());

			updateStatusSdn();
		}
		else
		{
			SDNMSG(SM_ERR, "PUB_REG failed. Node is not registered\n");
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
			return -1;
		}
	}
	else if(p_kv_command->m_strValue.compare(VAL_CMD__SUB_REG) == 0)
	{
		KeyValue* p_kv_metadata;
		
		KeyValue* p_kv_topicname;
		KeyValue* p_kv_topicdesc;
		KeyValue* p_kv_topicver;
		KeyValue* p_kv_payloadsize;
		KeyValue* p_kv_isevent;
		int is_event;
		
		p_kv_metadata = parse_message.getKey(NULL, KEY_META);
		if(p_kv_metadata == NULL)
		{
			SDNMSG(SM_ERR, "KEY_META does not exist in VAL_CMD__SUB_REG message.\n");
			return -1;
		}
		
		p_kv_topicname = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_TOPICNAME);
		p_kv_topicdesc = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_TOPICDESC);
		p_kv_topicver = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_TOPICVER);
		p_kv_payloadsize = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_PAYLOADSIZE);
		if((p_kv_topicname == NULL) || (p_kv_topicdesc == NULL) || (p_kv_topicver == NULL) || (p_kv_payloadsize == NULL))
		{
			SDNMSG(SM_ERR, "invalid null parameter for metadata. tname, tdesc, tver, plsize and event are mandatory.\n");
			return -1;
		}
		
		p_kv_isevent = parse_message.getKey(&p_kv_metadata->m_listKeyValue, KEY_META_ISEVENT);
		if(p_kv_isevent != NULL)
			is_event = atoi(p_kv_isevent->m_strValue.c_str());
		else 
			is_event = 0;
		
		if(p_node_info->getStatusNode() == STATUS_NODE_REGISTERED)
		{
			// Check if register information is valid
			p_topic_info = g_csSdnTopicDb.getTopicInfo((char*)p_kv_topicname->m_strValue.c_str());
			if(p_topic_info == NULL)
			{
				SDNMSG(SM_ERR, "SUB_REG failed. Topic[%s] is not in SDD DB\n", p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			p_topic_pubsub_info = p_topic_info->getSubInfo((char*)p_node_info->m_strHostName.c_str(), 
									(char*)p_node_info->m_strAppName.c_str());
			if(p_topic_pubsub_info == NULL)
			{
				SDNMSG(SM_ERR, "SUB_REG failed. Host[%s], App[%s] for Topic[%s] are not in SDD DB\n", 
					p_node_info->m_strHostName.c_str(), p_node_info->m_strAppName.c_str(), p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			if(p_topic_pubsub_info->getStatusTopicPubsub() == STATUS_TOPIC_PUBSUB_REGISTERED)
			{
				SDNMSG(SM_ERR, "SUB_REG failed. This subscriber[%s][%s] for topic[%s] is already registered\n",
					p_node_info->m_strHostName.c_str(), p_node_info->m_strAppName.c_str(), p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			// set p_topic_info->m_csMetadata.m_payloadSize if this is the first registered node in the topic list
			if(p_topic_info->getNumberOfRegisteredPubsub() == 0)
				p_topic_info->m_csMetadata.m_payloadSize = atoi(p_kv_payloadsize->m_strValue.c_str());

			if((p_topic_info->m_csMetadata.m_version != atoi(p_kv_topicver->m_strValue.c_str()))
				|| (p_topic_info->m_csMetadata.m_bEvent != is_event)
				|| (p_topic_info->m_csMetadata.m_payloadSize != atoi(p_kv_payloadsize->m_strValue.c_str())))
			{
				SDNMSG(SM_ERR, "SUB_REG failed. Metadata is different.\n");
				SDNMSG(SM_ERR, "DB for [%s]: version[%d], payload_size[%d], b_event[%d]\n",
					p_kv_topicname->m_strValue.c_str(), p_topic_info->m_csMetadata.m_version, 
					p_topic_info->m_csMetadata.m_payloadSize, p_topic_info->m_csMetadata.m_bEvent);
				SDNMSG(SM_ERR, "Received: version[%d], payload_size[%d], b_event[%d]\n",
					atoi(p_kv_topicver->m_strValue.c_str()), atoi(p_kv_payloadsize->m_strValue.c_str()), is_event);
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			// add pubsub info in g_csSdnNodeDb
			if(p_node_info->addNodePubsubInfo((char*)p_kv_topicname->m_strValue.c_str(), 0) != 0)
			{
				SDNMSG(SM_ERR, "SUB_REG failed. addNodePubsubInfo() failed.\n");
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
			
			// update pubsub info in g_csSdnTopicDb
			p_topic_pubsub_info->m_strIpAddr = p_node_info->m_strIpAddr;
			p_topic_pubsub_info->setStatusTopicPubsub(STATUS_TOPIC_PUBSUB_REGISTERED);

			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_ACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());

			SDNMSG(SM_INFO, "Subscriber is registered. host[%s], app[%s], ip[%s] \n", p_topic_pubsub_info->m_strHostName.c_str(),
				p_topic_pubsub_info->m_strAppName.c_str(), p_topic_pubsub_info->m_strIpAddr.c_str());

			updateStatusSdn();
		}
		else
		{
			SDNMSG(SM_ERR, "SUB_REG failed. Node is not registered\n");
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_REG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
			return -1;
		}
	}
	else if(p_kv_command->m_strValue.compare(VAL_CMD__PUB_UNREG) == 0)
	{
		KeyValue* p_kv_topicname;
		
		p_kv_topicname = parse_message.getKey(NULL, KEY_TOPICNAME);
		if(p_kv_topicname == NULL)
		{
			SDNMSG(SM_ERR, "KEY_TOPICNAME does not exist in VAL_CMD__PUB_UNREG message.\n");
			return -1;
		}
		
		if(p_node_info->getStatusNode() == STATUS_NODE_REGISTERED)
		{
			if(p_node_info->removeNodePubsubInfo((char*)p_kv_topicname->m_strValue.c_str(), 1) != 0)
			{
				SDNMSG(SM_WARN, "PUB_UNREG failed. removeNodePubsubInfo() failed.\n");

				// do not return here and continue to remove from topic_db (error case)
			}
			
			p_topic_info = g_csSdnTopicDb.getTopicInfo((char*)p_kv_topicname->m_strValue.c_str());
			if(p_topic_info == NULL)
			{
				SDNMSG(SM_ERR, "PUB_UNREG failed. Topic[%s] is not registered\n", p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
			
			if(p_topic_info->m_csPubInfo.getStatusTopicPubsub() != STATUS_TOPIC_PUBSUB_REGISTERED)
			{
				SDNMSG(SM_ERR, "PUB_UNREG failed. Publisher is not registered\n");
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
		
			SDNMSG(SM_INFO, "Publisher is unregistered. host[%s], app[%s], ip[%s] \n", p_topic_info->m_csPubInfo.m_strHostName.c_str(),
				p_topic_info->m_csPubInfo.m_strAppName.c_str(), p_topic_info->m_csPubInfo.m_strIpAddr.c_str());

			p_topic_info->m_csPubInfo.setStatusTopicPubsub(STATUS_TOPIC_PUBSUB_NOT_REGISTERED);
			
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_UNREG_ACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
			
			updateStatusSdn();
		}
		else
		{
			SDNMSG(SM_ERR, "PUB_UNREG failed. Node is not registered\n");
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__PUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
			return -1;
		}
	}
	else if(p_kv_command->m_strValue.compare(VAL_CMD__SUB_UNREG) == 0)
	{
		KeyValue* p_kv_topicname;
		
		p_kv_topicname = parse_message.getKey(NULL, KEY_TOPICNAME);
		if(p_kv_topicname == NULL)
		{
			SDNMSG(SM_ERR, "KEY_TOPICNAME does not exist in VAL_CMD__SUB_UNREG message.\n");
			return -1;
		}
		
		if(p_node_info->getStatusNode() == STATUS_NODE_REGISTERED)
		{
			if(p_node_info->removeNodePubsubInfo((char*)p_kv_topicname->m_strValue.c_str(), 0) != 0)
			{
				SDNMSG(SM_WARN, "SUB_UNREG failed. removeNodePubsubInfo() failed.\n");

				// do not return here and continue to remove from topic_db (error case)
			}
			
			p_topic_info = g_csSdnTopicDb.getTopicInfo((char*)p_kv_topicname->m_strValue.c_str());
			if(p_topic_info == NULL)
			{
				SDNMSG(SM_ERR, "PUB_UNREG failed. Topic[%s] is not registered\n", p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
			
			p_topic_pubsub_info = p_topic_info->getSubInfo((char*)p_node_info->m_strHostName.c_str(), 
									(char*)p_node_info->m_strAppName.c_str());
			if(p_topic_pubsub_info == NULL)
			{
				SDNMSG(SM_ERR, "SUB_UNREG failed. Host[%s], App[%s] for Topic[%s] are not in SDD DB\n", 
					p_node_info->m_strHostName.c_str(), p_node_info->m_strAppName.c_str(), p_kv_topicname->m_strValue.c_str());
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}

			if(p_topic_pubsub_info->getStatusTopicPubsub() != STATUS_TOPIC_PUBSUB_REGISTERED)
			{
				SDNMSG(SM_ERR, "SUB_UNREG failed. This subscriber is not registered\n");
				SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
				return -1;
			}
		
			SDNMSG(SM_INFO, "Subscriber is unregistered. host[%s], app[%s], ip[%s] \n", p_topic_pubsub_info->m_strHostName.c_str(),
				p_topic_pubsub_info->m_strAppName.c_str(), p_topic_pubsub_info->m_strIpAddr.c_str());

			p_topic_pubsub_info->setStatusTopicPubsub(STATUS_TOPIC_PUBSUB_NOT_REGISTERED);
			
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_UNREG_ACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
			
			updateStatusSdn();
		}
		else
		{
			SDNMSG(SM_ERR, "SUB_UNREG failed. Node is not registered\n");
			SendTwoKeysRegisterMessage(p_node_info->m_socket, KEY_CMD, VAL_CMD__SUB_UNREG_NACK, KEY_TOPICNAME, p_kv_topicname->m_strValue.c_str());
			return -1;
		}
	}
	
	return 0;
}



static int SendNodeMessage(int sock, char* p_send_message)
{
	int ret;
	
	if(p_send_message == NULL)
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}

	SDNMSG(SM_LOG, "[SEND(%d)]\n%s", strlen(p_send_message), p_send_message);

	ret = send(sock, p_send_message, strlen(p_send_message), 0);
	if(ret != (int)strlen(p_send_message))
	{
		SDNMSG(SM_ERR, "send(%d) returned %d.\n", strlen(p_send_message), ret);
		return -1;
	}

	return 0;
}



static int SendOneKeyRegisterMessage(int sock, const char* p_key, const char* p_value)
{
	KeyValue kv;
	list<KeyValue> list_kv;
	char* p_send_msg;
	ConstructMessage construct_message;
	int ret;

	if((p_key == NULL) || (p_value == NULL))
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}

	list_kv.clear();
	kv.setValue(p_key, p_value);
	list_kv.push_back(kv);
	p_send_msg = construct_message.construct(VALUE_MESSAGE_GROUP_REGISTER, &list_kv);
	if(p_send_msg == NULL)
	{
		SDNMSG(SM_ERR, "construct() returned NULL.\n");
		return -1;
	}
	
	ret = SendNodeMessage(sock, p_send_msg);

	return ret;
}



static int SendTwoKeysRegisterMessage(int sock, const char* p_key1, const char* p_value1, const char* p_key2, const char* p_value2)
{
	KeyValue kv;
	list<KeyValue> list_kv;
	char* p_send_msg;
	ConstructMessage construct_message;
	int ret;

	if((p_key1 == NULL) || (p_value1 == NULL) || (p_key2 == NULL) || (p_value2 == NULL))
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}

	list_kv.clear();
	kv.setValue(p_key1, p_value1);
	list_kv.push_back(kv);
	kv.setValue(p_key2, p_value2);
	list_kv.push_back(kv);
	p_send_msg = construct_message.construct(VALUE_MESSAGE_GROUP_REGISTER, &list_kv);
	if(p_send_msg == NULL)
	{
		SDNMSG(SM_ERR, "construct() returned NULL.\n");
		return -1;
	}
	
	ret = SendNodeMessage(sock, p_send_msg);

	return ret;
}



static int SendPublisherInfo(int sock, TopicInfo* p_topic_info)
{
	KeyValue kv;
	list<KeyValue> list_kv;
	list<KeyValue> list_sub_kv;
	char* p_send_msg;
	ConstructMessage construct_message;
	int ret;
	
	if(p_topic_info == NULL)
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}

	list_kv.clear();
	kv.setValue(KEY_CMD, VAL_CMD__NOTIFY_PUB_INFO);
	list_kv.push_back(kv);
	kv.setValue(KEY_TOPICNAME, p_topic_info->m_csMetadata.m_strName.c_str());
	list_kv.push_back(kv);
	
	list_sub_kv.clear();
	kv.setValue(KEY_PARAM_HOSTNAME, p_topic_info->m_csPubInfo.m_strHostName.c_str());
	list_sub_kv.push_back(kv);
	kv.setValue(KEY_PARAM_APPNAME, p_topic_info->m_csPubInfo.m_strAppName.c_str());
	list_sub_kv.push_back(kv);
	kv.setValue(KEY_PARAM_IP, p_topic_info->m_csPubInfo.m_strIpAddr.c_str());
	list_sub_kv.push_back(kv);
	
	kv.setList(KEY_PARAM, &list_sub_kv);
	list_kv.push_back(kv);

	p_send_msg = construct_message.construct(VALUE_MESSAGE_GROUP_REGISTER, &list_kv);
	if(p_send_msg == NULL)
	{
		SDNMSG(SM_ERR, "construct() returned NULL.\n");
		return -1;
	}

	ret = SendNodeMessage(sock, p_send_msg);

	return ret;
}



static int SendSubscriberList(int sock, TopicInfo* p_topic_info)
{
	KeyValue kv;
	list<KeyValue> list_kv;
	list<KeyValue> list_sub_kv;
	list<KeyValue> list_sub_sub_kv;
	char* p_send_msg;
	ConstructMessage construct_message;
	int ret;
	
	if(p_topic_info == NULL)
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}

	list_kv.clear();
	kv.setValue(KEY_CMD, VAL_CMD__NOTIFY_SUB_LIST);
	list_kv.push_back(kv);
	kv.setValue(KEY_TOPICNAME, p_topic_info->m_csMetadata.m_strName.c_str());
	list_kv.push_back(kv);

	list_sub_kv.clear();
	for(vector<TopicPubsubInfo*>::iterator iter = p_topic_info->m_vectorSubInfo.begin(); iter != p_topic_info->m_vectorSubInfo.end(); iter++)
	{
		list_sub_sub_kv.clear();
		kv.setValue(KEY_SUBLIST_SUB_HOSTNAME, (*iter)->m_strHostName.c_str());
		list_sub_sub_kv.push_back(kv);
		kv.setValue(KEY_SUBLIST_SUB_APPNAME, (*iter)->m_strAppName.c_str());
		list_sub_sub_kv.push_back(kv);
		kv.setValue(KEY_SUBLIST_SUB_IP, (*iter)->m_strIpAddr.c_str());
		list_sub_sub_kv.push_back(kv);

		kv.setList(KEY_SUBLIST_SUB, &list_sub_sub_kv);
		list_sub_kv.push_back(kv);
	}
	kv.setList(KEY_SUBLIST, &list_sub_kv);
	list_kv.push_back(kv);

	p_send_msg = construct_message.construct(VALUE_MESSAGE_GROUP_REGISTER, &list_kv);
	if(p_send_msg == NULL)
	{
		SDNMSG(SM_ERR, "construct() returned NULL.\n");
		return -1;
	}

	ret = SendNodeMessage(sock, p_send_msg);

	return ret;
}



static int SendTopicStatus(int sock, TopicInfo* p_topic_info)
{
	KeyValue kv;
	list<KeyValue> list_kv;
	char* p_send_msg;
	ConstructMessage construct_message;
	int ret;
	
	if(p_topic_info == NULL)
	{
		SDNMSG(SM_ERR, "invalid NULL parameter.\n");
		return -1;
	}

	list_kv.clear();
	kv.setValue(KEY_CMD, VAL_CMD__NOTIFY_TOPIC_STATUS);
	list_kv.push_back(kv);
	kv.setValue(KEY_TOPICNAME, p_topic_info->m_csMetadata.m_strName.c_str());
	list_kv.push_back(kv);
	kv.setValue(KEY_TOPICSTATUS, (p_topic_info->getStatusTopic() == STATUS_TOPIC_READY) ? VAL_STATUS__READY : VAL_STATUS__NOT_READY);
	list_kv.push_back(kv);

	p_send_msg = construct_message.construct(VALUE_MESSAGE_GROUP_REGISTER, &list_kv);
	if(p_send_msg == NULL)
	{
		SDNMSG(SM_ERR, "construct() returned NULL.\n");
		return -1;
	}

	ret = SendNodeMessage(sock, p_send_msg);

	return ret;
}



static int unregisterNodeInfoInTopicDb(NodeInfo* p_node_info)
{
	TopicInfo* p_topic_info;
	TopicPubsubInfo* p_topic_pubsub_info;
	NodePubsubInfo* p_node_pubsub_info;
	list<NodePubsubInfo*>::iterator node_pubsub_info_iter;
	
	if(p_node_info == NULL)
		return -1;
	
	// update status in the g_csSdnTopicDb
	node_pubsub_info_iter = p_node_info->m_listNodePubsubInfo.begin();
	while(node_pubsub_info_iter != p_node_info->m_listNodePubsubInfo.end())
	{
		p_node_pubsub_info = *node_pubsub_info_iter;
		
		p_topic_info = g_csSdnTopicDb.getTopicInfo((char*)p_node_pubsub_info->m_strTopicName.c_str());
		if(p_topic_info != NULL)
		{
			if(p_node_pubsub_info->m_bPublisher)	// publisher
			{
				p_topic_info->m_csPubInfo.setStatusTopicPubsub(STATUS_TOPIC_PUBSUB_NOT_REGISTERED);
			}
			else	// subscriber
			{
				p_topic_pubsub_info = p_topic_info->getSubInfo((char*)p_node_info->m_strHostName.c_str(), 
					(char*)p_node_info->m_strAppName.c_str());
				if(p_topic_pubsub_info != NULL)
				{
					p_topic_pubsub_info->setStatusTopicPubsub(STATUS_TOPIC_PUBSUB_NOT_REGISTERED);
				}
			}
		}
		
		node_pubsub_info_iter++;
	}
	
	updateStatusSdn();

	return 0;
}



static int updateStatusSdn()
{
	list<NodeInfo*>::iterator node_info_iter;
	vector<TopicInfo*>::iterator topic_info_iter;

	STATUS_SDN_MONITOR old_sdn_status;
	STATUS_SDN_MONITOR current_sdn_status;
	int topic_status_ready;

	int is_publish_exist;
	int is_subscriber_exist;

	int ret;


	old_sdn_status = g_csSdnTopicDb.getStatusSdn();

	topic_status_ready = 1;

	topic_info_iter = g_csSdnTopicDb.m_vectorTopicInfo.begin();
	while(topic_info_iter != g_csSdnTopicDb.m_vectorTopicInfo.end())
	{
		ret = (*topic_info_iter)->updateStatusTopic();
		if(ret)
		{
			// send VAL_CMD__NOTIFY_TOPIC_STATUS to all nodes which contain the corresponding topic pubsub
			node_info_iter = g_csSdnNodeDb.m_listNodeInfo.begin();
			while(node_info_iter != g_csSdnNodeDb.m_listNodeInfo.end())
			{
				if((*node_info_iter)->getNodePubsubInfo((*topic_info_iter)->m_csMetadata.m_strName.c_str(), 1) != NULL)
					is_publish_exist = 1;
				else
					is_publish_exist = 0;

				if((*node_info_iter)->getNodePubsubInfo((*topic_info_iter)->m_csMetadata.m_strName.c_str(), 0) != NULL)
					is_subscriber_exist = 1;
				else
					is_subscriber_exist = 0;
				
				if(is_publish_exist || is_subscriber_exist)
				{
					// when the topic is event and it changes to ready state, 
					//	send VAL_CMD__NOTIFY_PUB_INFO and VAL_CMD__NOTIFY_SUB_LIST
					if((*topic_info_iter)->m_csMetadata.m_bEvent
						&& ((*topic_info_iter)->getStatusTopic() == STATUS_TOPIC_READY))
					{
						if(is_publish_exist)
							SendSubscriberList((*node_info_iter)->m_socket, *topic_info_iter);
					
						if(is_subscriber_exist)
							SendPublisherInfo((*node_info_iter)->m_socket, *topic_info_iter);
					}

					// send VAL_CMD__NOTIFY_TOPIC_STATUS
					SendTopicStatus((*node_info_iter)->m_socket, *topic_info_iter);
				}
				
				node_info_iter++;
			}
		}
		
		if((*topic_info_iter)->getStatusTopic() != STATUS_TOPIC_READY) 
			topic_status_ready = 0;

		topic_info_iter++;
	}

	if(topic_status_ready)
		current_sdn_status = STATUS_SDN_READY;
	else
		current_sdn_status = STATUS_SDN_NOT_READY;

	if(current_sdn_status != old_sdn_status)
	{
		SDNMSG(SM_INFO, "SDN Status is changed to [%s]\n", (current_sdn_status == STATUS_SDN_READY) ? VAL_STATUS__READY : VAL_STATUS__NOT_READY);

		g_csSdnTopicDb.setStatusSdn(current_sdn_status);
		
		if(current_sdn_status == STATUS_SDN_READY)
		{
			// wait until subscribers are prepared to receive 
			sleep(1);
		}

		node_info_iter = g_csSdnNodeDb.m_listNodeInfo.begin();
		while(node_info_iter != g_csSdnNodeDb.m_listNodeInfo.end())
		{
			SendTwoKeysRegisterMessage((*node_info_iter)->m_socket, KEY_CMD, VAL_CMD__NOTIFY_SDN_STATUS, 
				KEY_SDNSTATUS, (current_sdn_status == STATUS_SDN_READY) ? VAL_STATUS__READY : VAL_STATUS__NOT_READY); 
			node_info_iter++;
		}
	}
	
	return 0;
}

