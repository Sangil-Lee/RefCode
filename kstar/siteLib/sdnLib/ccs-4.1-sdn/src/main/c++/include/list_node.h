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

#ifndef LIST_NODE_H
#define LIST_NODE_H

#include <string>
#include <list>


typedef enum {
	STATUS_NODE_NOT_CONNECTED,	// node of this status will be removed from the list
	STATUS_NODE_CONNECTED,
	STATUS_NODE_REGISTERED,
} STATUS_NODE;


class NodePubsubInfo {
public:
	int m_bPublisher;
	std::string m_strTopicName;
};


/// class NodeInfo
/// Manages Node Connection list. 
///
class NodeInfo {
public:
	NodeInfo() : m_statusNode(STATUS_NODE_NOT_CONNECTED) {};

	int m_socket;

	std::string m_strHostName;
	std::string m_strAppName;
	std::string m_strIpAddr;

	time_t m_timeConnected;
	time_t m_timeLastAlive;

	std::list<NodePubsubInfo*> m_listNodePubsubInfo;


	STATUS_NODE getStatusNode()
	{
		return m_statusNode;
	}

	void setStatusNode(STATUS_NODE status)
	{
		m_statusNode = status;
		return;
	}

	int addNodePubsubInfo(char* topic_name, int b_publish)
	{
		NodePubsubInfo* p_node_pubsub_info;

		if(topic_name == NULL)
			return -1;

		p_node_pubsub_info = new NodePubsubInfo;
		if(!p_node_pubsub_info)
			return -1;

		p_node_pubsub_info->m_strTopicName = topic_name;
		p_node_pubsub_info->m_bPublisher = b_publish;

		m_listNodePubsubInfo.push_back(p_node_pubsub_info);

		return 0;
	}


	int removeNodePubsubInfo(char* topic_name, int b_publish)
	{
		if(topic_name == NULL)
			return -1;

		for(std::list<NodePubsubInfo*>::iterator iter = m_listNodePubsubInfo.begin(); 
			iter != m_listNodePubsubInfo.end(); iter++)
		{
			if(((*iter)->m_strTopicName.compare(topic_name) == 0)
				&& ((*iter)->m_bPublisher == b_publish))
			{
				delete(*iter);
				m_listNodePubsubInfo.erase(iter);
				return 0;
			}
		}

		return -1;
	}


	NodePubsubInfo* getNodePubsubInfo(const char* topic_name, int b_publish)
	{
		if(topic_name == NULL)
			return NULL;
		
		for(std::list<NodePubsubInfo*>::iterator iter = m_listNodePubsubInfo.begin(); 
			iter != m_listNodePubsubInfo.end(); iter++)
		{
			if(((*iter)->m_strTopicName.compare(topic_name) == 0)
				&& ((*iter)->m_bPublisher == b_publish))
			{
				return (*iter);
			}
		}

		return NULL;
	}


	int removeAllNodePubsubInfo()
	{
		std::list<NodePubsubInfo*>::iterator iter = m_listNodePubsubInfo.begin(); 
		while(iter != m_listNodePubsubInfo.end())
		{
			delete(*iter);
			iter = m_listNodePubsubInfo.erase(iter);
		}

		return 0;
	}

private:
	STATUS_NODE m_statusNode;
};


class SdnNodeDb {
public:
	SdnNodeDb() {};
	
	std::list<NodeInfo*> m_listNodeInfo;


	// add node when connected. hostname, appname, ipaddr will be set when registered
	int addNodeInfo(int sock, char* ip_addr = NULL)
	{
		NodeInfo* p_node_info = new NodeInfo;

		if(!p_node_info)
			return -1;

		p_node_info->m_socket = sock;

		if(ip_addr != NULL)
			p_node_info->m_strIpAddr = ip_addr;

		time(&p_node_info->m_timeConnected);
		p_node_info->m_timeLastAlive = p_node_info->m_timeConnected;

		p_node_info->setStatusNode(STATUS_NODE_CONNECTED);

		m_listNodeInfo.push_back(p_node_info);

		return 0;
	}


	int removeNodeInfo(int sock)
	{
		for(std::list<NodeInfo*>::iterator iter=m_listNodeInfo.begin(); iter != m_listNodeInfo.end(); iter++)
		{
			if((*iter)->m_socket == sock)
			{
				(*iter)->removeAllNodePubsubInfo();
				delete(*iter);
				m_listNodeInfo.erase(iter);
				return 0;
			}
		}

		return -1;
	}


	NodeInfo* getNodeInfo(int sock)
	{
		for(std::list<NodeInfo*>::iterator iter=m_listNodeInfo.begin(); iter != m_listNodeInfo.end(); iter++)
		{
			if((*iter)->m_socket == sock)
			{
				return (*iter);
			}
		}

		return NULL;
	}


	NodeInfo* getNodeInfo(char* host_name, char* app_name)
	{
		if((host_name == NULL) || (app_name == NULL))
			return NULL;

		for(std::list<NodeInfo*>::iterator iter=m_listNodeInfo.begin(); iter != m_listNodeInfo.end(); iter++)
		{
			if(((*iter)->m_strHostName.compare(host_name) == 0)
				&& ((*iter)->m_strAppName.compare(app_name) == 0))
			{
				return (*iter);
			}
		}

		return NULL;
	}


	int removeAllNodeInfo()
	{
		std::list<NodeInfo*>::iterator iter=m_listNodeInfo.begin();
		while(iter != m_listNodeInfo.end())
		{
			(*iter)->removeAllNodePubsubInfo();
			delete(*iter);
			iter = m_listNodeInfo.erase(iter);
		}

		return 0;
	}
};


extern class SdnNodeDb g_csSdnNodeDb;

#endif
