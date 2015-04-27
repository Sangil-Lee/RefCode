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

/*! \file sdnmon_msg.h 
 *   \brief SDNMON Message Header File
 */

#ifndef SDNMON_MSG_H
#define SDNMON_MSG_H

#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>


///////////////////////////////////////////////////////////////////////////
// environment variables
// * format: SDN_MONITORING_NODE=host_ip_addr(or hostname):reg_port:ui_port
#define	SDN_MONITORING_NODE_ENVNAME				"SDN_MONITORING_NODE"

#define	DEFAULT_SDN_MONITOR_SERVER_PORT			10000
#define	DEFAULT_SDN_MONITOR_SERVER_PORT_REG		DEFAULT_SDN_MONITOR_SERVER_PORT
#define	DEFAULT_SDN_MONITOR_SERVER_PORT_UI		(DEFAULT_SDN_MONITOR_SERVER_PORT_REG+1)


/////////////////////////////////////////////////////////
// message delimeters
// eg1) PVER#1.0\n
// eg2) META#TNAME=TOPIC1;TDESC=This is topic1;TVER=1;PLSIZE=52;;\n
// eg3) SUBLIST#SUB=HOST:FC1,APP:TEST1,,;SUB=HOST:FC2,APP:TEST2,,;;\n
#define	DELIM_KEY_VALUE			"#"
#define	DELIM_KEY_LINE			"\n"
#define	DELIM_KEY_END			"\n\n"
#define	DELIM_PARAM_VALUE		"="
#define	DELIM_PARAM_LINE		";"
#define	DELIM_PARAM_END			";;"
#define	DELIM_SUB_PARAM_VALUE	":"
#define	DELIM_SUB_PARAM_LINE	","
#define	DELIM_SUB_PARAM_END		",,"


/////////////////////////////////////////////////////////
// Keys & Values for Message
#define	KEY_PROTOCOL_VERSION			"PVER"
	#define	VALUE_PROTOCOL_VERSION_MAJOR		1
	#define	VALUE_PROTOCOL_VERSION_MINOR		0
#define	KEY_MESSAGE_GROUP				"MGRP"
	#define	VALUE_MESSAGE_GROUP_REGISTER	"REG"
	#define	VALUE_MESSAGE_GROUP_UI			"UI"

	
////////////////////////////////////////////////////////////////////////////////
/// class KeyValue
/// String KEY and its VALUE
///	VALUE is set to the other list of KeyValue objects.
class KeyValue {
public:
	/** \fn void clean()
	 *	\brief Initialize member variables of KeyValue object. 
	 */
	void clean()
	{
		m_strKey = "";
		m_strValue = "";
		m_listKeyValue.clear();
	}

	
	/** \fn int setValue(const char* p_key, const char* p_value)
	 *	\brief Set the KeyValue object with a key and a value. 
	 *	The m_listKeyValue member variable is cleared.
	 *	@param[in] p_key is string pointer for key name.
	 *	@param[in] p_value is string pointer for key value.
	 */
	int setValue(const char* p_key, const char* p_value)
	{
		if((p_key == NULL) || (p_value == NULL))
			return -1;

		clean();
		m_strKey = p_key;
		m_strValue = p_value;

		return 0;
	}
	
	/** \fn int setList(const char* p_key, std::list<KeyValue>* p_list)
	 *	\brief Set the KeyValue object with a key and its sublist for other KeyValue objects. 
	 *	The m_strValue member variable is cleared.
	 *	@param[in] p_key is string pointer for key name.
	 *	@param[in] p_list is sublist for other KeyValue objects.
	 */
	int setList(const char* p_key, std::list<KeyValue>* p_list)
	{
		if((p_key == NULL) || (p_list == NULL))
			return -1;

		clean();
		m_strKey = p_key;
		m_listKeyValue = *p_list;

		return 0;
	}
	
	
	std::string m_strKey;
	std::string m_strValue;
	std::list<KeyValue> m_listKeyValue;
};


////////////////////////////////////////////////////////////////////////////////
/// class ConstructMessage
///	Makes a text string message from the list of KeyValue objects
///	The text string message follows the format defined in SDNMON communication protocol
class ConstructMessage {
public:
	ConstructMessage() {};

	std::string m_strMessage;


	/** \fn char* construct(const char* message_group, std::list<KeyValue>* p_list_kv)
	 *	\brief Makes a text string message from the list of KeyValue objects
	 *	It returns the constructed text string message.
	 *	@param[in] message_group is VALUE_MESSAGE_GROUP_REGISTER or VALUE_MESSAGE_GROUP_UI.
	 *	@param[in] p_list_kv is list of KeyValue objects.
	 */
	char* construct(const char* message_group, std::list<KeyValue>* p_list_kv)
	{
		if((message_group == NULL) || (p_list_kv == NULL))
			return NULL;
		
		m_strMessage = "";

		m_strMessage += KEY_PROTOCOL_VERSION;
		m_strMessage += DELIM_KEY_VALUE;
		m_strMessage += (char)('0' + VALUE_PROTOCOL_VERSION_MAJOR);
		m_strMessage += '.';
		m_strMessage += (char)('0' + VALUE_PROTOCOL_VERSION_MINOR);
		m_strMessage += DELIM_KEY_LINE;

		m_strMessage += KEY_MESSAGE_GROUP;
		m_strMessage += DELIM_KEY_VALUE;
		m_strMessage += message_group;
		m_strMessage += DELIM_KEY_LINE;

		for(std::list<KeyValue>::iterator iter = p_list_kv->begin(); iter != p_list_kv->end(); iter++)
		{
			if(iter->m_listKeyValue.empty())
			{
				m_strMessage += iter->m_strKey;
				m_strMessage += DELIM_KEY_VALUE;
				m_strMessage += iter->m_strValue;
				m_strMessage += DELIM_KEY_LINE;
			}
			else
			{
				m_strMessage += iter->m_strKey;
				m_strMessage += DELIM_KEY_VALUE;

				for(std::list<KeyValue>::iterator param_iter = iter->m_listKeyValue.begin(); 
					param_iter != iter->m_listKeyValue.end(); param_iter++)
				{
					if(param_iter->m_listKeyValue.empty())
					{
						m_strMessage += param_iter->m_strKey;
						m_strMessage += DELIM_PARAM_VALUE;
						m_strMessage += param_iter->m_strValue;
						m_strMessage += DELIM_PARAM_LINE;
					}
					else
					{
						m_strMessage += param_iter->m_strKey;
						m_strMessage += DELIM_PARAM_VALUE;
						
						for(std::list<KeyValue>::iterator sub_param_iter = param_iter->m_listKeyValue.begin(); 
							sub_param_iter != param_iter->m_listKeyValue.end(); sub_param_iter++)
						{
							m_strMessage += sub_param_iter->m_strKey;
							m_strMessage += DELIM_SUB_PARAM_VALUE;
							m_strMessage += sub_param_iter->m_strValue;
							m_strMessage += DELIM_SUB_PARAM_LINE;
						}

						m_strMessage += DELIM_SUB_PARAM_LINE;
						m_strMessage += DELIM_PARAM_LINE;
					}
				}

				m_strMessage += DELIM_PARAM_LINE;
				m_strMessage += DELIM_KEY_LINE;
			}
		}

		m_strMessage += DELIM_KEY_LINE;

		return (char*)m_strMessage.c_str();
	}
};



////////////////////////////////////////////////////////////////////////////////
/// class ParseMessage
///	Makes a list of KeyValue objects from the text string message.
///	The text string message follows the format defined in SDNMON communication protocol
class ParseMessage {
public:
	ParseMessage() {};

	std::list<KeyValue> m_listKeyValue;


	/** \fn std::list<KeyValue>* parse(char* message)
	 *	\brief Makes a list of KeyValue objects from the text string message. 
	 *	Resulted list of key_value is saved to m_listKeyValue member variables,
	 *	and then returns the pointer of m_listKeyValue as a return value
	 *	@param[in] message is the text string message of SDNMON communication protocol.
	 */
	std::list<KeyValue>* parse(char* message)
	{
		char* cur_ptr;
		char* next_ptr;
		char* tmp;
		char* key;
		char* value;
		char* param_cur_ptr;
		char* param_next_ptr;
		char* param_tmp;
		char* param_key;
		char* param_value;
		char* sub_param_cur_ptr;
		char* sub_param_next_ptr;
		char* sub_param_tmp;
		char* sub_param_key;
		char* sub_param_value;
		KeyValue kv;
		KeyValue param_kv;
		KeyValue sub_param_kv;

		m_listKeyValue.clear();
		
		cur_ptr = message;
		tmp = strtok_r(cur_ptr, DELIM_KEY_LINE, &next_ptr);
		while(tmp != NULL)
		{
			key = strtok_r(tmp, DELIM_KEY_VALUE, &value);
			if((key == NULL) || (value == NULL))
				break;

			kv.clean();

			if(strpbrk(value, DELIM_PARAM_VALUE))
			{
				kv.m_strKey = key;

				param_cur_ptr = value;
				param_tmp = strtok_r(param_cur_ptr, DELIM_PARAM_LINE, &param_next_ptr);
				while(param_tmp != NULL)
				{
					param_key = strtok_r(param_tmp, DELIM_PARAM_VALUE, &param_value);
					if((param_key == NULL) || (param_value == NULL))
						break;

					param_kv.clean();

					if(strpbrk(param_value, DELIM_SUB_PARAM_VALUE))
					{
						param_kv.m_strKey = param_key;
						
						sub_param_cur_ptr = param_value;
						sub_param_tmp = strtok_r(sub_param_cur_ptr, DELIM_SUB_PARAM_LINE, &sub_param_next_ptr);
						while(sub_param_tmp != NULL)
						{
							sub_param_key = strtok_r(sub_param_tmp, DELIM_SUB_PARAM_VALUE, &sub_param_value);
							if((sub_param_key == NULL) || (sub_param_value == NULL))
								break;
						
							sub_param_kv.clean();

							sub_param_kv.m_strKey = sub_param_key;
							sub_param_kv.m_strValue = sub_param_value;
						
							param_kv.m_listKeyValue.push_back(sub_param_kv);

							sub_param_tmp = strtok_r(NULL, DELIM_SUB_PARAM_LINE, &sub_param_next_ptr);
						}
					}
					else
					{
						param_kv.m_strKey = param_key;
						param_kv.m_strValue = param_value;
					}

					kv.m_listKeyValue.push_back(param_kv);

					param_tmp = strtok_r(NULL, DELIM_PARAM_LINE, &param_next_ptr);
				}

			}
			else
			{
				kv.m_strKey = key;
				kv.m_strValue = value;
			}

			m_listKeyValue.push_back(kv);

			tmp = strtok_r(NULL, DELIM_KEY_LINE, &next_ptr);
		}

		return &m_listKeyValue;
	}


	/** \fn KeyValue* getKey(std::list<KeyValue>* p_list_kv, const char* key, int index = 0)
	 *	\brief Searches key in the KeyValue list and returns its popinter. 
	 *	Starting index in the list can be specified by optional parameter index.
	 *	@param[in] p_list_kv is the list of KeyValue in which the key is searched.
	 *	@param[in] key is the key name.
	 *	@param[in] index is the start point of searching in the list.
	 */
	// if p_list_kv parameter is NULL, key will be searched in the m_listKeyValue (1st depth)
	KeyValue* getKey(std::list<KeyValue>* p_list_kv, const char* key, int index = 0)
	{
		int count = 0;
		
		if(p_list_kv == NULL)
			p_list_kv = &m_listKeyValue;
		
		for(std::list<KeyValue>::iterator iter = p_list_kv->begin(); iter != p_list_kv->end(); iter++)
		{
			if(iter->m_strKey.compare((const char*)key) == 0)
			{
				if(index == count)
					return (&(*iter));
				else
				{
					count++;
					continue;
				}
			}
		}

		return NULL;
	}
};

#endif

