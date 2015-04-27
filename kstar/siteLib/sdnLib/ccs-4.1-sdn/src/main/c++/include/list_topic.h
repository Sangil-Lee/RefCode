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

#ifndef LIST_TOPIC_H
#define LIST_TOPIC_H

#define	TOPIC_SCHEMA_VERSION_MAJOR	1
#define	TOPIC_SCHEMA_VERSION_MINOR	0

#include <string>
#include <vector>


typedef enum {
	STATUS_SDN_NOT_READY,
	STATUS_SDN_READY
} STATUS_SDN_MONITOR;
	
typedef enum {
	STATUS_TOPIC_NOT_READY,
	STATUS_TOPIC_READY
} STATUS_TOPIC;

typedef enum {
	STATUS_TOPIC_PUBSUB_NOT_REGISTERED,
	STATUS_TOPIC_PUBSUB_REGISTERED,
} STATUS_TOPIC_PUBSUB;


class Metadata {
public:	
	std::string m_strName;
	std::string m_strDescription;
	unsigned short m_version;
	unsigned short m_payloadSize;
	char m_bEvent;
};


class TopicPubsubInfo {
public:
	TopicPubsubInfo() : m_statusTopicPubsub(STATUS_TOPIC_PUBSUB_NOT_REGISTERED) {};

	std::string m_strHostName;
	std::string m_strAppName;
	std::string m_strIpAddr;

	STATUS_TOPIC_PUBSUB getStatusTopicPubsub()
	{
		return m_statusTopicPubsub;
	}

	void setStatusTopicPubsub(STATUS_TOPIC_PUBSUB status)
	{
		m_statusTopicPubsub = status;
		return;
	}

private:
	STATUS_TOPIC_PUBSUB m_statusTopicPubsub;
};


/// class TopicInfo
/// Manages Topic related information, which are metadata, publisher, and list of subscribers. 
///
class TopicInfo {
public:
	TopicInfo() : m_statusTopic(STATUS_TOPIC_NOT_READY) {};

	Metadata m_csMetadata;
	TopicPubsubInfo m_csPubInfo;
	std::vector<TopicPubsubInfo*> m_vectorSubInfo;

	
	TopicPubsubInfo* getSubInfo(char* host_name, char* app_name)
	{
		for(std::vector<TopicPubsubInfo*>::iterator iter=m_vectorSubInfo.begin(); iter != m_vectorSubInfo.end(); iter++)
		{
			if(((*iter)->m_strHostName.compare(host_name) == 0)
				&& ((*iter)->m_strAppName.compare(app_name) == 0))
			{
				return (*iter);
			}
		}

		return NULL;
	}


	STATUS_TOPIC getStatusTopic()
	{
		return m_statusTopic;
	}


	int getNumberOfRegisteredPubsub()
	{
		int count = 0;
		
		if(m_csPubInfo.getStatusTopicPubsub() == STATUS_TOPIC_PUBSUB_REGISTERED)
			count++;
		
		for(std::vector<TopicPubsubInfo*>::iterator iter=m_vectorSubInfo.begin(); iter != m_vectorSubInfo.end(); iter++)
		{
			if((*iter)->getStatusTopicPubsub() == STATUS_TOPIC_PUBSUB_REGISTERED)
				count++;
		}

		return count;
	}

	
	int updateStatusTopic()
	{
		int old_status_topic = m_statusTopic;
		int status_not_ready = 0;
		
		if(m_csPubInfo.getStatusTopicPubsub() != STATUS_TOPIC_PUBSUB_REGISTERED)
		{
			status_not_ready = 1;
		}
		else
		{
			for(std::vector<TopicPubsubInfo*>::iterator iter=m_vectorSubInfo.begin(); iter != m_vectorSubInfo.end(); iter++)
			{
				if((*iter)->getStatusTopicPubsub() != STATUS_TOPIC_PUBSUB_REGISTERED)
				{
					status_not_ready = 1;
					break;
				}
			}
		}

		if(status_not_ready)
			m_statusTopic = STATUS_TOPIC_NOT_READY;
		else
			m_statusTopic = STATUS_TOPIC_READY;
		
		if(old_status_topic != m_statusTopic)
			return 1;
		
		return 0;
	}


private:
	STATUS_TOPIC m_statusTopic;
};


/// class SdnTopicDb
/// Manages TopicInfo list. 
///
class SdnTopicDb {
public:
	SdnTopicDb() : m_statusSdn(STATUS_SDN_NOT_READY) {};
	
	std::vector<TopicInfo*> m_vectorTopicInfo;


	TopicInfo* getTopicInfo(char* topic_name)
	{
		for(std::vector<TopicInfo*>::iterator iter = m_vectorTopicInfo.begin(); iter != m_vectorTopicInfo.end(); iter++)
		{
			if((*iter)->m_csMetadata.m_strName.compare(topic_name) == 0) 
			{
				return (*iter);
			}
		}

		return NULL;
	}


	void setStatusSdn(STATUS_SDN_MONITOR status)
	{
		m_statusSdn = status;
	}


	STATUS_SDN_MONITOR getStatusSdn()
	{
		return m_statusSdn;
	}


private:
	STATUS_SDN_MONITOR m_statusSdn;	
};


extern class SdnTopicDb g_csSdnTopicDb;

#endif
