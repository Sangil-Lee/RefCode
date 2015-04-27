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
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>

#include "list_topic.h"
#include "list_node.h"
#include "sdn_common/log_buffer.h"

using namespace std; 


#define	TAG_DB				"sdn_topics_db"
#define	TAG_DB_VER			"schema_version"
#define	TAG_TOPIC			"topic"
#define	TAG_METADATA		"metadata"
#define	TAG_META_NAME		"name"
#define	TAG_META_DESC		"description"
#define	TAG_META_VER		"version"
#define	TAG_META_EVENT		"is_event"
#define	TAG_PUB				"publisher"
#define	TAG_SUB				"subscriber"
#define	TAG_HOSTNAME		"host_name"
#define	TAG_APPNAME			"app_name"		

#define	STAG(x)				"<"x">"
#define	ETAG(x)				"</"x">"


static int ExtractString(string& str_from, const char* tag, string& str_to, int remove=1);



int ParseConfig(const char* config_filename) 
{
	TopicInfo* p_topic_info;
	TopicPubsubInfo* p_sub_info;
	TopicPubsubInfo subinfo;

	ifstream config_file;
	string str_line;
	string str_topic;
	string str_topic_name;
	string str_metadata;
	string str_meta_name;
	string str_meta_desc;
	string str_meta_ver;
	string str_meta_event;
	string str_pub;
	string str_sub;
	string str_hostname;
	string str_appname;
	size_t pos;

	int found;
	int b_new_topic;

	config_file.open(config_filename, ios::in);
	if(!config_file.is_open())
	{
		SDNMSG(SM_ERR, "file[%s] open error.\n", config_filename);
		return -1;
	}

	// find start tag of TAG_DB
	found = 0;
	while(1)
	{
		// extract contents between TAG_DBs and save it to str_topic
		getline(config_file, str_line);
		if(str_line.empty())
			break;

		pos = str_line.find(STAG(TAG_DB));
		if(pos == string::npos)
			continue;

		found = 1;
		break;
	}

	if(!found)
	{
		SDNMSG(SM_ERR, "failed to find TAG_DB[%s] in the configuration file\n", STAG(TAG_DB));
		config_file.close();
		return -1;
	}

	// Parse all the topics
	while(1)
	{
		// extract contents between TAG_DBs and save it to str_topic
		getline(config_file, str_line);
		if(str_line.empty())
			break;

		pos = str_line.find(STAG(TAG_TOPIC));
		if(pos == string::npos)
			continue;

		// initialize all the strings
		str_topic.clear();
		str_topic_name.clear();
		str_metadata.clear();
		str_meta_name.clear();
		str_meta_desc.clear();
		str_meta_ver.clear();
		str_meta_event.clear();
		str_pub.clear();
		str_hostname.clear();
		str_appname.clear();
		
		str_topic = str_line.substr(pos + strlen(STAG(TAG_TOPIC)));

		found = 0;
		while(1)
		{
			getline(config_file, str_line);
			if(str_line.empty() || config_file.eof())
				break;
			
			pos = str_line.find(ETAG(TAG_TOPIC));
			if(pos == string::npos)
				str_topic += str_line;
			else
			{
				str_topic += str_line.substr(0, pos);
				found = 1;
				break;
			}
		}

		if(!found)
		{
			SDNMSG(SM_ERR, "failed to find TAG_TOPIC[%s] in the configuration file\n", ETAG(TAG_TOPIC));
			config_file.close();
			return -1;
		}

		// find topic name first (for debug messages)
		if(ExtractString(str_topic, TAG_META_NAME, str_topic_name, 0) != 0)
		{
			SDNMSG(SM_ERR, "cannot find topic_name in a topic\n");
			continue;
		}
		
		// process with str_topic
		ExtractString(str_topic, TAG_METADATA, str_metadata);
		ExtractString(str_topic, TAG_PUB, str_pub);
		
		// proess with str_metadata
		ExtractString(str_metadata, TAG_META_NAME, str_meta_name);
		ExtractString(str_metadata, TAG_META_VER, str_meta_ver);
		ExtractString(str_metadata, TAG_META_DESC, str_meta_desc);
		ExtractString(str_metadata, TAG_META_EVENT, str_meta_event);

		// process with str_pub
		ExtractString(str_pub, TAG_HOSTNAME, str_hostname);
		ExtractString(str_pub, TAG_APPNAME, str_appname);

		b_new_topic = 0;
		
		p_topic_info = g_csSdnTopicDb.getTopicInfo((char*)str_topic_name.c_str());
		if(p_topic_info == NULL)
		{
			b_new_topic = 1;
			// set TopicInfo field 	
			p_topic_info = new TopicInfo;
			if(p_topic_info == NULL)
			{
				SDNMSG(SM_ERR, "failed to create new class TopicInfo\n");
				config_file.close();
				return -1;
			}

			// set default value for new object
			p_topic_info->m_csMetadata.m_payloadSize = 0;
			p_topic_info->m_csMetadata.m_bEvent = 0;
		}


		if(!str_meta_name.empty())
			p_topic_info->m_csMetadata.m_strName = str_meta_name;

		if(!str_meta_desc.empty())
			p_topic_info->m_csMetadata.m_strDescription = str_meta_desc;

		if(!str_meta_ver.empty())
			p_topic_info->m_csMetadata.m_version = atoi(str_meta_ver.c_str());
		
		if(!str_meta_event.empty())
			p_topic_info->m_csMetadata.m_bEvent = atoi(str_meta_event.c_str());

		
		
		if(!str_hostname.empty())
			p_topic_info->m_csPubInfo.m_strHostName = str_hostname;

		if(!str_appname.empty())
			p_topic_info->m_csPubInfo.m_strAppName = str_appname;

		
		// add subscribers to the TopicInfo.m_vectorSubInfo
		while(1)
		{
			str_sub.clear();
			str_hostname.clear();
			str_appname.clear();

			ExtractString(str_topic, TAG_SUB, str_sub);
			if(str_sub.empty())
				break;
			
			ExtractString(str_sub, TAG_HOSTNAME, str_hostname);
			ExtractString(str_sub, TAG_APPNAME, str_appname);

			p_sub_info = new TopicPubsubInfo;
			if(p_sub_info == NULL)
			{
				SDNMSG(SM_ERR, "failed to create new class TopicPubsubInfo\n");
				config_file.close();
				return -1;
			}
			
			p_sub_info->m_strHostName = str_hostname;
			p_sub_info->m_strAppName = str_appname;
			p_topic_info->m_vectorSubInfo.push_back(p_sub_info);
		}

		// if p_topic_info is for new object, add it into the m_vectorTopicInfo 
		if(b_new_topic)
			g_csSdnTopicDb.m_vectorTopicInfo.push_back(p_topic_info);
	}

	config_file.close();

	return 0;
}



static int ExtractString(string& str_from, const char* tag, string& str_to, int remove) 
{
	size_t spos;
	size_t epos;

	size_t tag_spos;
	size_t tag_epos;

	string stag;
	string etag;

	stag = "<";
	stag += tag;
	stag += ">";

	etag = "</";
	etag += tag;
	etag += ">";

	tag_spos = str_from.find(stag);
	spos = tag_spos + stag.size();

	epos = str_from.find(etag);
	tag_epos = epos + etag.size();

	if((spos == string::npos) || (epos == string::npos) || 
		(tag_spos == string::npos) || (tag_epos == string::npos) || 
		(spos >= epos))
	{
		str_to = "";
		return -1;
	}

	str_to = str_from.substr(spos, epos - spos);
	if(remove)
		str_from.erase(tag_spos, tag_epos - tag_spos);

	return 0;
}




