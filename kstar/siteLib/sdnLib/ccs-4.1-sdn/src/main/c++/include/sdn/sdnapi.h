/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdnapi.h $
* $Id: sdnapi.h 36311 2013-04-29 08:07:33Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API SDNAPI header file.
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

/*! \file sdnapi.h 
 *  \brief SDN API Header File
 *
 * [ Version History ]
 * CCS v4.0
 *	. release first version of SDN API
 * CCS v4.1
 *	. New Function
 *		- Registration to SDN monitoring node (new)
 *	. New API
 *		- SR_RET initializeSDN(char* def_conf_file, char* user_conf_file)
 *		- SR_RET setSDNStatusCallback(void (*fnCallbackSDNStatus)(int is_ready))
 *		- SR_RET unsetSDNStatusCallback()
 *		- int isSDNStatusReady()
 *		- pthread_t getLogThreadId();
 *		- SR_RET SdnPublisher::setEventAckTimeout(int usec)
 *		- SR_RET SdnPublisher::getEventAckTimeout(int& usec)
 *		- SR_RET SdnPublisher::getEventAckStatus(unsigned int seq_num, STATUS_ACK& ack_status)
 *		- SR_RET SdnPublisher::setEventAckCallback(void (*callback_func)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec))
 *		- pthread_t SdnPublisher::getAckRecvThreadId()
 *		- pthread_t SdnSubscriber::getAsyncRecvThreadId()
 *	. Changes of API
 *		- SdnSubscriber::setReceiveCallback(SR_RET (*fnCallbackReceive)(topic_userdata &, SR_RET)) is changed to
 *			SdnSubscriber::setReceiveCallback(SR_RET (*fnCallbackReceive)(SdnSubscriber<topic_metadata, topic_userdata>*, SR_RET))
 *			* Old API is obsolete, and it returns SR_ERROR for CCS v4.1. Old API will be removed from CCS v5.0
 *		- SdnSubscriber::getPacketSequenceNumber(unsigned short& seqNumber) is changed to 
 *			SdnSubscriber::getPacketSequenceNumber(unsigned int& seqNumber)
 *		- SdnSubscriber::receive(topic_userdata& theTopicUserData) is changed to
 *			SdnSubscriber::receive(topic_userdata& theTopicUserData, int timeout_usec=0)
 *		- SdnPublisher::publish(topic_userdata& theTopicUserData) is changed to
 *			SdnPublisher::publish(topic_userdata& theTopicUserData, unsigned int* pSeqNumber=NULL)
 *		- SdnPublisher::setPublisher(topic_metadata& theTopicMetaData) is changed to
 *			SdnPublisher::setPublisher(topic_metadata& theTopicMetaData, int enable_loopback=0) 
 */

#ifndef SDN_SDNAPI_H
#define SDN_SDNAPI_H

#include "sdn_node_info.h"
#include "sdn_publisher.h"
#include "sdn_subscriber.h"

#endif 
