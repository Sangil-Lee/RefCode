/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdn_publisher.h $
* $Id: sdn_publisher.h 36408 2013-05-02 08:49:35Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API SdnPublisher header file.
*
* Author        : Eui Jae LEE, Hyung Gi KIM (Mobiis Co., Ltd)
*
* Co-Author     : Mahajan Kirti
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

/*! \file sdn_publisher.h 
 *  \brief publisher API
 */

#ifndef SDN_PUBLISHER_H
#define SDN_PUBLISHER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "../sdn_common/log_buffer.h"
#include "sdn_node_info.h"
#include "sdn_packet.h"


typedef enum {
	STATUS_ACK_UNKNOWN,
	STATUS_ACK_WAITING,
	STATUS_ACK_TIMEOUT,
	STATUS_ACK_ACKNOWLEDGED
} STATUS_ACK;



// Definitions for Internal Use
#undef SDNMSG_T
#define	SDNMSG_T(level, fmt, args...)	SDNMSG(level, "<%s> " fmt, m_csTopicMetaData_.getName(), ## args)

#define MAX_COUNT_ACKLIST_			10
#define	DEFAULT_TIMEOUT_ACK_US_		1000



/******************************************************************************
 *	Classes for Internal Use
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// class SubStatus 
/// describes subscriber information and its acknowledge status of event packet
class SubStatus {
	friend class EventAckList;
	
	char is_acknowledged;	//!< indicates if it is acknowledged
	std::string hostname;	//!< host name of the controller
	std::string appname;	//!< application name
};



////////////////////////////////////////////////////////////////////////////////
/// class EventAckList
/// manages subscriber list if the topic is for event
class EventAckList {
public:
	EventAckList()
	{
		m_indexAckList_ = 0;
		for(int i = 0; i < MAX_COUNT_ACKLIST_; i++)
		{
			m_AckList_[i].is_valid = 0;
			m_AckList_[i].ack_status = STATUS_ACK_UNKNOWN;
		}

		m_cbEventAck_ = NULL;
	};

	/** \fn int initializeSubStatusList(std::list<PeerInfo>& list_peer_info)
	 *	\brief Initialize AckList. 
	 *	@param[in] peer_info_list is the list of subscribers' PeerInfo.
	 */
	int initializeSubStatusList(std::list<PeerInfo>& list_peer_info)
	{
		SubStatus sub_status;
		
		for(int i = 0; i < MAX_COUNT_ACKLIST_; i++)
		{
			m_AckList_[i].list_substatus.clear();
		
			for(std::list<PeerInfo>::iterator iter_peer = list_peer_info.begin(); iter_peer != list_peer_info.end(); iter_peer++)
			{
				sub_status.is_acknowledged = 0;
				sub_status.hostname = (*iter_peer).m_nameHost;
				sub_status.appname = (*iter_peer).m_nameApp;
				m_AckList_[i].list_substatus.push_back(sub_status);
			}
		}
	
		return 0;
	};
	
	
	
	/** \fn int addNewEvent(unsigned int seq_num)
	 *	\brief Add new event to the AckList and then wait ack. 
	 *	@param[in] seq_num is the sequence number to be added to the acknowledgement waiting list.
	 */
	int addNewEvent(unsigned int seq_num)
	{
		std::list<SubStatus>::iterator iter;
		
		m_indexAckList_ = (m_indexAckList_ + 1) % MAX_COUNT_ACKLIST_;
	
		// It is assumed this event is already timed out.
		// For this to be assumed, publish() should not be called more than 
		//	(MAX_COUNT_ACKLIST_ - 2) times during the configured timeout.
		// It is recommended to configure the timeout less or equal than the control loop cycle.
		m_AckList_[m_indexAckList_].is_valid = 0;
	
		m_AckList_[m_indexAckList_].ack_status = STATUS_ACK_WAITING;
		m_AckList_[m_indexAckList_].seq_num = seq_num;
		gettimeofday(&m_AckList_[m_indexAckList_].sendtime, NULL);
		
		iter = m_AckList_[m_indexAckList_].list_substatus.begin();
		while(iter != m_AckList_[m_indexAckList_].list_substatus.end())
		{
			(*iter).is_acknowledged = 0;
			iter++;
		}
	
		m_AckList_[m_indexAckList_].is_valid = 1;
	
		// Make the next list invaild not to be conflicted between processing and addNewEvent().
		// This item will be free during a control loop.
		m_AckList_[(m_indexAckList_ + 1) % MAX_COUNT_ACKLIST_].is_valid = 0;
	
		return 0;
	};
	
	
	
	/** \fn int checkTimeoutEvent(int timeout)
	 *	\brief Remove old event, and then execute callback
	 *	@param[in] timeout is the timeout in micro-seconds in which event should acknowledged.
	 */
	int checkTimeoutEvent(int timeout)
	{
		int elapsed_usec;
		struct timeval curtime;
		int i;
		int new_timeout = 0;
	
		gettimeofday(&curtime, NULL);
		for(i = 0; i < MAX_COUNT_ACKLIST_; i++)
		{
			if((m_AckList_[i].is_valid == 0) 
				|| (m_AckList_[i].ack_status != STATUS_ACK_WAITING))
				continue;
			
			elapsed_usec = (curtime.tv_sec - m_AckList_[i].sendtime.tv_sec) * 1000000
							+ (curtime.tv_usec - m_AckList_[i].sendtime.tv_usec);
			if(elapsed_usec >= timeout)
			{
				new_timeout = 1;
				m_AckList_[i].ack_status = STATUS_ACK_TIMEOUT;
				if(m_cbEventAck_ != NULL)
					m_cbEventAck_(m_AckList_[i].seq_num, STATUS_ACK_TIMEOUT, elapsed_usec);
			}
		}
	
		return new_timeout;
	};
	
	
	
	/** \fn int processAckMessage(unsigned int seq_num, char* host_name, char* app_name, int timeout)
	 *	\brief Process acknowledgement message
	 *	@param[in] seq_num is the sequence number of the sent packet.
	 *	@param[in] host_name is the host name of the controller which sent packet.
	 *	@param[in] app_name is the application name which sent packet.
	 *	@param[in] timeout is the timeout in micro-seconds in which event should acknowledged.
	 */
	int processAckMessage(unsigned int seq_num, char* host_name, char* app_name, int timeout)
	{
		std::list<SubStatus>::iterator iter;
		char is_ack_all;
		int elapsed_usec;
		struct timeval curtime;
		STATUS_ACK ack_status_old;
	
		for(int i = 0; i < MAX_COUNT_ACKLIST_; i++)
		{
			if((m_AckList_[i].is_valid == 0)
				|| (m_AckList_[i].ack_status == STATUS_ACK_ACKNOWLEDGED)
				|| (m_AckList_[i].seq_num != seq_num))
				continue;
		
			is_ack_all = 1;
		
			iter = m_AckList_[i].list_substatus.begin();
			while(iter != m_AckList_[i].list_substatus.end())
			{
				if(((*iter).hostname.compare(host_name) == 0)
					&& ((*iter).appname.compare(app_name) == 0))
					(*iter).is_acknowledged = 1;
				
				if((*iter).is_acknowledged == 0)
					is_ack_all = 0;
	
				iter++;
			}
		
			if(is_ack_all)
			{
				ack_status_old = m_AckList_[i].ack_status;
				m_AckList_[i].ack_status = STATUS_ACK_ACKNOWLEDGED;
	
				gettimeofday(&curtime, NULL);
				elapsed_usec = (curtime.tv_sec - m_AckList_[i].sendtime.tv_sec) * 1000000
								+ (curtime.tv_usec - m_AckList_[i].sendtime.tv_usec);
				
				if((elapsed_usec > timeout) && (ack_status_old == STATUS_ACK_WAITING))
				{
					SDNMSG(SM_LOG, "Event Ack Timeout - 2\n");
					if(m_cbEventAck_ != NULL)
						m_cbEventAck_(seq_num, STATUS_ACK_TIMEOUT, elapsed_usec);
				}
	
				if(m_cbEventAck_ != NULL)
					m_cbEventAck_(seq_num, STATUS_ACK_ACKNOWLEDGED, elapsed_usec);
			}
	
			break;
		}
	
		return 0;
	};
	
	
	
	/** \fn int setCallbackEventAck(void (*callback_func)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec))
	 *	\brief Set callback function which should be called when received an acknoweledgement
	 *	@param[in] pointer of callback_func
	 */
	int setCallbackEventAck(void (*callback_func)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec))
	{
		m_cbEventAck_ = callback_func;
		return 0;
	};
	
	
	
	/** \fn int getEventAckStatus(unsigned int seq_num, STATUS_ACK& ack_status)
	 *	\brief Get acknowledgement status of the event whose sequence number is seq_num
	 *	@param[in] seq_num is the sequence number of event
	 *	@param[in] ack_status is the acknowledgement status of the event
	 */
	int getEventAckStatus(unsigned int seq_num, STATUS_ACK& ack_status)
	{
		int i;
		for(i = 0; i < MAX_COUNT_ACKLIST_; i++)
		{
			if((m_AckList_[i].is_valid == 1)
				&& (m_AckList_[i].seq_num == seq_num))
				break;
		}
	
		if(i >= MAX_COUNT_ACKLIST_)
			ack_status = STATUS_ACK_UNKNOWN;
		else
			ack_status = m_AckList_[i].ack_status;
		
		return 0;
	};
	

private:	
	int m_indexAckList_;
	struct {
		char is_valid;
		STATUS_ACK ack_status;
		unsigned int seq_num;
		struct timeval sendtime;
		std::list<SubStatus> list_substatus;
	} m_AckList_[MAX_COUNT_ACKLIST_];

	void (*m_cbEventAck_)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec);
};






/*---------------- Template class SdnPublisher declaration --------------------------------*/

/// class SdnPublisher 
/// is a template class for SDN topic publishing.
///
/// The topic_metadata class and topic_userdata class on a topic are necessary as template parameters. 
/// This class holds communication parameters for the topic to be published.
template <class topic_metadata, class topic_userdata>
class SdnPublisher {
	public:
		SdnPublisher()
		{
			m_noSend = 0;
			m_usDelay = 0;
			m_sockfd_ = -1;

			m_sockEvent_ = -1;
			m_usTimeoutAck_ = DEFAULT_TIMEOUT_ACK_US_;
			m_threadIdAckRecv_ = (pthread_t)0;
		}

		// for function test
		int m_noSend;  	//!< packet processing (sequence number increment,...), but inhibit packet send (for packet loss test)
		int m_usDelay;	//!< make changes on timestamp value of send packet

		SR_RET setPublisher(topic_metadata& theTopicMetaData, int enable_loopback = 0); 
		SR_RET unsetPublisher();
		SR_RET publish(topic_userdata& theTopicUserData, unsigned int* pSeqNumber = NULL); 

		// for event topic
		SR_RET setEventAckTimeout(int usec); 
		SR_RET getEventAckTimeout(int& usec); 
		SR_RET getEventAckStatus(unsigned int seq_num, STATUS_ACK& ack_status); 
		SR_RET setEventAckCallback(void (*callback_func)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec)); 
		pthread_t getAckRecvThreadId();
		
	private:		
		topic_metadata m_csTopicMetaData_;
		SdnPacket<topic_userdata> m_csSdnPacket_;  

		// socket information (set by setPubDgramSocket_())
		int m_sockfd_; 
		struct sockaddr_in m_sockaddr_; 
		char m_mcastIpAddr_[MAX_IP_ADDR_LEN]; 
		unsigned short  m_mcastPort_; 

		// event management
		char m_isEvent_;
		int m_sockEvent_; 
		int m_usTimeoutAck_;
		class EventAckList m_eventAckList_;
		pthread_t m_threadIdAckRecv_;
		static void* threadAckRecv_(void* p_param);
		static void cleanupThreadAckRecv_(void* p_param);

		// other internal functions
		SR_RET setPubDgramSocket_(char* multicast_ip, unsigned short multicast_port, int enable_loopback); 
		SR_RET unsetPubDgramSocket_(char* multicast_ip, unsigned short multicast_port); 
};



/*--------------------publish template function defintion -----------------*/

/*---------------- setPublisher function definition ---------------------------*/
/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::setPublisher(topic_metadata& theTopicMetaData, int enable_loopback)
 *  \brief This function needs a topic_metadata class as its parameter.
 *
 *  The topic related information, such as topic name, multicast address, and multicast port are filled from this parameter, 
 *  and socket is created with this information. Registration to the SDN monitor node on the publishing topic is done in this function.
 *  @param[in] theTopicMetaData is the variable of type topic_metadata class defined in the topic definition file.
 *  @param[in] enable_loopback is optional parameter that enables loopback of multicast packets.
 *				The default value is 0 not to affect the performance. Use this parameter only if it is necessary.
 *  \warning After the SdnPublisher object is created, setPublisher()should be called first before any other member functions.
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::setPublisher(topic_metadata& theTopicMetaData, int enable_loopback) 
{
	SR_RET ret;
	
	SDNMSG(SM_LOG, "<%s> setPublisher called.\n", theTopicMetaData.getName());

	if(g_NodeInfo.isRegistered() == 0)
	{
		SDNMSG(SM_ERR, "<%s> g_NodeInof is not initialized. initializeSDN() first.\n", theTopicMetaData.getName());
		return SR_ERR_NOT_INITIALIZED;
	}
	
	if(m_sockfd_ >= 0)
	{
		SDNMSG(SM_ERR, "<%s> topic is set already to this publisher.\n", theTopicMetaData.getName());
		return SR_ERR_ALREADY;
	}
	
	m_csTopicMetaData_ = theTopicMetaData;

	/* get multicast ipaddr  and port for the topic_metadata*/
	ret = g_NodeInfo.getMulticastAddress(m_csTopicMetaData_.getName(), m_mcastIpAddr_, &m_mcastPort_);
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "g_NodeInfo.getMulticastAddress() error. ret=0x%x\n", ret);
		return ret;
	}

	/* set datagram socket for the publihser */
	ret = setPubDgramSocket_(m_mcastIpAddr_, m_mcastPort_, enable_loopback);
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "setPubDgramSocket_() error. ret=0x%x\n", ret);
		return ret;
	}

	/* initialize packet sequence number to zero */
	m_csSdnPacket_.setPacketSequence(0);

	m_isEvent_ = m_csTopicMetaData_.isEvent();
	if(m_isEvent_)
	{
		ret = pthread_create(&m_threadIdAckRecv_, NULL, &threadAckRecv_, this);
		if(ret != 0)
		{
			SDNMSG(SM_ERR, "pthread_create() failed.\n"); 
			return SR_ERROR;
		}
	}
		

	/* register to monitor node	*/
	ret = g_NodeInfo.registerPubsub(m_csTopicMetaData_.getName(), m_csTopicMetaData_.getDescription(),
			m_csTopicMetaData_.getVersion(), m_csTopicMetaData_.getPayloadsize(), 
			m_isEvent_, 1, m_mcastIpAddr_, m_mcastPort_);
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "registerPubsub() failed. ret=0x%x\n", ret);
		unsetPubDgramSocket_(m_mcastIpAddr_, m_mcastPort_);
		return ret;
	}
	

	SDNMSG_T(SM_INFO, "Publisher initialized. Multicast addr [%s], Port [%d].\n", m_mcastIpAddr_, m_mcastPort_);
	
	return SR_OK;
}


/*------------Unset Publisher and close the sockets -----------------------------*/
/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::unsetPublisher()
 *  \brief This function should be called at the end of using SdnPublisher object. 
 *
 *  This function closes the socket and cleans up before terminating the application or 
 *	when no more data is required to be published. Deregistration from the SDN monitor node
 *	on the published topic is done.
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::unsetPublisher() 
{
	int ret = SR_OK;
	
	SDNMSG_T(SM_LOG, "unsetPublisher called.\n");

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}
	
	SDNMSG_T(SM_INFO, "Stop publishing on the topic.\n");

	if(m_isEvent_)
	{
		if(m_threadIdAckRecv_ != (pthread_t)0)
		{
			pthread_cancel(m_threadIdAckRecv_);
			m_threadIdAckRecv_ = (pthread_t)0;
		}
	}

    /* close socket */
	ret = unsetPubDgramSocket_(m_mcastIpAddr_, m_mcastPort_);
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "unsetPubDgramSocket_() error. ret=0x%x\n", ret);
		return ret;
	}

	/* unregister from monitor node	*/
	g_NodeInfo.unregisterPubsub(m_csTopicMetaData_.getName(), 1);

	return ret;
}


/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::publish(topic_userdata& theTopicUserData, unsigned int* pSeqNumber)
 *	\brief Transmit SDN packet on the topic.
 *
 *  The SDN packet is composed of SDN packet header and the user data payload. 
 *  The packet format of the user data payload is defined in the topic_userdata. 
 *  The SDN packet header is added to this payload. The SDN packet header is managed 
 *  in the SdnPublisher object using the internal SdnPacket object.
 *  The optional parameter pSeqNumber is used to get the sequence number of the sent packet, 
 *  which is necessary to check the acknowledgement status of the sent packet when it is event topic.
 *	@param[in]	theTopicUserData is the variable of type topic_userdata class defined in the topic definition file
 *	@param[out]	pSeqNumber is the optional parameter which returns the sequence number of sent packet
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::publish(topic_userdata& theTopicUserData, unsigned int* pSeqNumber)
{
	int len = 0;
	int ret = SR_OK;
	unsigned int seq_num;
	
	SDNMSG_T(SM_TEST, "publish() start.\n");

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}
	
	m_csSdnPacket_.incrPacketSequence();
	m_csSdnPacket_.setPayload(theTopicUserData);
	m_csSdnPacket_.setSenderTime(m_usDelay);

	m_csSdnPacket_.getPacketSequence(seq_num);

	if(pSeqNumber != NULL)
		*pSeqNumber = seq_num;

	if(m_noSend)
		return ret;
	
	if(m_isEvent_)
		m_eventAckList_.addNewEvent(seq_num);
	
	/* Send SdnPacket */
 	len = sendto(m_sockfd_, &m_csSdnPacket_.m_sdnPacket, m_csSdnPacket_.getSdnPacketLen(), 0, 
 			(struct sockaddr*)&m_sockaddr_, sizeof(struct sockaddr));
	if(len < 0)
	{
		SDNMSG_T(SM_ERR, "Sending datagram message error\n");
		ret = SR_ERR_SOCKET;
		return ret;
	}
	else if(len == 0)
	{
		SDNMSG_T(SM_ERR, "Sending datagram message error. ret=0\n");
		ret = SR_ERR_SEND;
		return ret;
	}

	SDNMSG_T(SM_TEST, "publish() end. len=%d, ret=%d\n", len, ret);

	return ret;
}



/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::setEventAckTimeout(int usec)
 *  \brief Sets timeout in micro-seconds for acknowledgement of event.
 *
 *  Timeout for acknowledgement of event is set to 1000 micro-second as default.  
 *  This function is used to change the default timeout.
 *  If the event is not acknowledged before the timeout, getEventAckStatus() returns STATUS_ACK_TIMEOUT,
 *  and the callback function is called if set by setEventAckCallback()
 *  @param[in] usec is the timeout in micro-seconds for acknowledgement of event
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::setEventAckTimeout(int usec)
{
	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}
	
	if(!m_isEvent_)
	{
		SDNMSG_T(SM_ERR, "topic is not event.\n");
		return SR_ERROR;
	}
	
	m_usTimeoutAck_ = usec;
	return SR_OK;
}



/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::getEventAckTimeout(int& usec)
 *  \brief Gets timeout in micro-seconds for acknowledgement of event.
 *
 *  This function is used to get the timeout in micro-seconds for acknowledgement of event.
 *  @param[out] usec is the timeout in micro-seconds for acknowledgement of event
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::getEventAckTimeout(int& usec)
{
	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}
	
	if(!m_isEvent_)
	{
		SDNMSG_T(SM_ERR, "topic is not event.\n");
		return SR_ERROR;
	}
	
	usec = m_usTimeoutAck_;
	return SR_OK;
}



/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::getEventAckStatus(unsigned int seq_num, STATUS_ACK& ack_status)
 *  \brief Gets acknowledgement status of the event corresponding to the seq_num.
 *
 *  This function is used to check the acknowledgement status of the event whose sequence number is seq_num.
 *  seq_num is returned through the optional output parameter of publish() API. 
 *  Acknowledgement status is returned through ack_status.
 *  @param[in] seq_num is the sequence number of the event
 *  @param[out] ack_status STATUS_ACK_UNKNOWN, STATUS_ACK_WAITING, STATUS_ACK_TIMEOUT, or STATUS_ACK_ACKNOWLEDGED
 *    STATUS_ACK_UNKNOWN - not found in the acknowledgement waiting queue because it is very old one or is not sent
 *    STATUS_ACK_WAITING - not acknowledged and timeout is not reached
 *    STATUS_ACK_TIMEOUT - not acknowledged and timeout expired
 *    STATUS_ACK_ACKNOWLEDGED - acknowledged
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::getEventAckStatus(unsigned int seq_num, STATUS_ACK& ack_status)
{
	int ret;

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}
	
	if(!m_isEvent_)
	{
		SDNMSG_T(SM_ERR, "topic is not event.\n");
		return SR_ERROR;
	}
	
	ret = m_eventAckList_.getEventAckStatus(seq_num, ack_status);
	if(ret != 0)
		return SR_ERROR;

	return SR_OK;
}



/** \fn SR_RET SdnPublisher<topic_metadata, topic_userdata>::setEventAckCallback(void (*callback_func)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec))
 *  \brief Sets the callback function which should be called when the acknowledgement status is changed.
 *
 *  User is notified by the callback function when the acknowledgement status is changed to STATUS_ACK_TIMEOUT or STATUS_ACK_ACKNOWLEDGED.
 *  If the callback function is not set or set to NULL, the change of acknowledgement is not notified.
 *  seq_num is the sequence number of the event whose status is changed.
 *  ack_status is the current status of the acknowledgement for the seq_num.
 *  elapsed_usec is elapsed time in micro-seconds after the event is sent.
 *  @param[in] callback_func is the callback function which should be called when the acknowledgement status is changed
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::setEventAckCallback(void (*callback_func)(unsigned int seq_num, STATUS_ACK ack_status, int elapsed_usec))
{
	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}
	
	if(!m_isEvent_)
	{
		SDNMSG_T(SM_ERR, "topic is not event.\n");
		return SR_ERROR;
	}
	
	m_eventAckList_.setCallbackEventAck(callback_func);
	return SR_OK;
}



/** \fn pthread_t SdnPublisher<topic_metadata, topic_userdata>::getAckRecvThreadId()
 *  \brief Gets the thread ID of acknowledgement receive thread.
 *
 *	The thread ID is used to change thread attribute such as scheduling policy and priority.
 *	Acknowledgement receive thread is SdnPublisher internal thread which receives acknowledgements
 *	from subscribers for event topics.
 */
template <class topic_metadata, class topic_userdata>
pthread_t SdnPublisher<topic_metadata, topic_userdata>::getAckRecvThreadId()
{
	return m_threadIdAckRecv_;
};



/*------------------- Internal function ----------------------------------------------*/

/*----------------------setPubDgramSocket_()------------------------------------*/
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::setPubDgramSocket_(char* multicast_ip, unsigned short multicast_port, 
		int enable_loopback)
{
	struct in_addr localInterface;

	/* Open datagram socket for publisher */
	m_sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "Opening datagram socket error\n");
		return SR_ERR_SOCKET;
	}
	else
	{
		SDNMSG_T(SM_LOG, "Opening the datagram socket...OK\n");
	}

	memset((char*) &m_sockaddr_, 0, sizeof(m_sockaddr_));
	m_sockaddr_.sin_family = AF_INET;
	m_sockaddr_.sin_addr.s_addr = inet_addr(multicast_ip);
	m_sockaddr_.sin_port = htons(multicast_port);

	if(!enable_loopback)	
	{
		char loopch = 0;

		if(setsockopt(m_sockfd_, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
		{
			SDNMSG(SM_ERR,"Setting IP_MULTICAST_LOOP error\n");
			return SR_ERROR;
		}
		else
			SDNMSG(SM_LOG,"Disabling the loopback...OK.\n");
	}
	else
		SDNMSG(SM_LOG,"Enabling the loopback...OK.\n");
		
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr(g_NodeInfo.getSdnIpAddr());
	if(setsockopt(m_sockfd_, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localInterface, sizeof(localInterface)) < 0)
	{
		SDNMSG_T(SM_ERR, "Setting local interface error\n");
		return SR_ERROR;
	}
	else
	{
		SDNMSG_T(SM_LOG, "Setting the local interface...OK\n");
	}

	return SR_OK;
}


/*---------- unsubscribe from the topic and close the socket -----------------------*/
template <class topic_metadata, class topic_userdata>
SR_RET SdnPublisher<topic_metadata, topic_userdata>::unsetPubDgramSocket_(char* multicast_ip, unsigned short multicast_port)
{
	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "Error! socket is not opened\n");
		return SR_ERR_INVALID_SOCKET;
	}

	close(m_sockfd_); 
	m_sockfd_ = -1;

	return SR_OK;
}



/*--- thread to receive event acknowledge from subscribers (only created when the topic is event) ---*/
template <class topic_metadata, class topic_userdata>
void* SdnPublisher<topic_metadata, topic_userdata>::threadAckRecv_(void* p_param)
{
#define	MAX_RECV_BUF_SIZE			1500
	int ret;
	int len;
	int offset;
	int ack_len;
	int new_timeout;
	fd_set fds;
	int reuse;
	char* p_host_name;
	char* p_app_name;
	unsigned int seq_num;
	char* p_buf;
	struct sockaddr_in addr;
	socklen_t socklen = (socklen_t)sizeof(struct sockaddr_in); 
	struct timeval timeout;
	SdnPublisher<topic_metadata, topic_userdata>* p_pub = (SdnPublisher<topic_metadata, topic_userdata>*)p_param;
	std::list<PeerInfo> list_peer_info;
	char recv_buf[MAX_RECV_BUF_SIZE];

	if(p_pub == NULL)
	{
		SDNMSG(SM_ERR, "invalid null parameter\n");
		return (void*)1;
	}

	pthread_cleanup_push(p_pub->cleanupThreadAckRecv_, p_pub);

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (ret != 0)
		SDNMSG(SM_WARN,"pthread_setcancelstate(PTHREAD_CANCEL_ENABLE) failed.\n");

	while(1)
	{
		LOCK(g_NodeInfo.m_mutexSubList);

		pthread_cond_wait(&g_NodeInfo.m_condSubList, &g_NodeInfo.m_mutexSubList);

		// [AckList] check subscriber list for this topic, and exit loop if it is available
		if(g_NodeInfo.getPeerInfo(p_pub->m_csTopicMetaData_.getName(), 1, p_pub->m_isEvent_, list_peer_info) == 0)
		{
			if(!list_peer_info.empty())
			{
				p_pub->m_eventAckList_.initializeSubStatusList(list_peer_info);
				UNLOCK(g_NodeInfo.m_mutexSubList);
				break;
			}
		}

		UNLOCK(g_NodeInfo.m_mutexSubList);
	}
	
	p_pub->m_sockEvent_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(p_pub->m_sockEvent_ < 0)
	{
		SDNMSG(SM_ERR, "socket() error\n");
		return (void*)1;
	}

	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	reuse = 1;
	if(setsockopt(p_pub->m_sockEvent_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
	{
		SDNMSG(SM_ERR, "Setting SO_REUSEADDR error\n");
		close(p_pub->m_sockEvent_);
		p_pub->m_sockEvent_ = -1;
		return (void*)1;
	}

	memset(&addr, 0, socklen);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(p_pub->m_mcastPort_ + 1);
	addr.sin_addr.s_addr = inet_addr(g_NodeInfo.getSdnIpAddr());
	if(bind(p_pub->m_sockEvent_, (struct sockaddr*)&addr, socklen) != 0)
	{
		SDNMSG(SM_ERR, "Binding datagram socket error\n");
		close(p_pub->m_sockEvent_);
		p_pub->m_sockEvent_ = -1;
		return (void*)1;
	}

	// recv loop
	// message format: "seq_num(4byte, little-endian) + host_name\0 + app_name\0"
	while(1)
	{
		// [AckList] remove old event, and then execute callback
		new_timeout = p_pub->m_eventAckList_.checkTimeoutEvent(p_pub->m_usTimeoutAck_);
		if(new_timeout)
			SDNMSG(SM_LOG, "Event Ack Timeout for topic [%s]\n", p_pub->m_csTopicMetaData_.getName());

		// check ack status every half time of timeout
		timeout.tv_sec = (p_pub->m_usTimeoutAck_ / 2) / 1000000;
		timeout.tv_usec = (p_pub->m_usTimeoutAck_ / 2) % 1000000;

		// not to make timeout as 0
		if((timeout.tv_sec == 0) && (timeout.tv_usec == 0) && (p_pub->m_usTimeoutAck_ != 0))
			timeout.tv_usec = 1;
	
		FD_ZERO(&fds);
		FD_SET(p_pub->m_sockEvent_, &fds);
		ret = select(p_pub->m_sockEvent_+1, &fds, NULL, NULL, &timeout);
		if(ret < 0)
		{
			SDNMSG(SM_ERR, "select() error\n");
			break;
		}
		else if(ret == 0)
			continue;

		len = recvfrom(p_pub->m_sockEvent_, recv_buf, MAX_RECV_BUF_SIZE, 0, (struct sockaddr*)&addr, &socklen);
		if(len <= 0)	// socket closed or error
		{
			if((errno == EINTR) || (errno == EAGAIN))
			{
				SDNMSG(SM_WARN, "recvfrom() returned errno %d\n", errno);
				continue;
			}

			SDNMSG(SM_ERR,"recvfrom() returned %d\n", len);
			break;
		}
		else
		{
			offset = 0;
			while(len > 0)
			{
				p_buf = recv_buf + offset;
				
				seq_num = *(unsigned int*)p_buf;
				p_host_name = p_buf + 4;
				p_app_name = p_host_name + strlen(p_host_name) + 1;
				ack_len = (int)((unsigned long)p_app_name + strlen(p_app_name) + 1 - (unsigned long)p_buf);
				if(ack_len > len)
				{
					SDNMSG(SM_WARN, "invalid ack message(msglen[%d], ack_len[%d], seqnum[%d], hostnam[%s], appname[%s]\n", 
						len, ack_len, seq_num, p_host_name, p_app_name);
					break;
				}

				// [AckList] process ack message
				p_pub->m_eventAckList_.processAckMessage(seq_num, p_host_name, p_app_name, p_pub->m_usTimeoutAck_);

				if(len > ack_len)
					SDNMSG(SM_INFO, "message contains more than a ack message(len[%d], ack_len[%d]\n", len, ack_len);

				offset += ack_len;
				len -= ack_len;
			}
		}
	}

	pthread_cleanup_pop(1);

	return (void*)0;
}



template <class topic_metadata, class topic_userdata>
void SdnPublisher<topic_metadata, topic_userdata>::cleanupThreadAckRecv_(void* p_param)
{
	SdnPublisher<topic_metadata, topic_userdata>* p_pub = (SdnPublisher<topic_metadata, topic_userdata>*)p_param;

	UNLOCK(g_NodeInfo.m_mutexSubList);

	if(p_pub == NULL)
	{
		SDNMSG(SM_ERR, "invalid null parameter\n");
		return;
	}

	if(p_pub->m_sockEvent_ >= 0)
	{
		close(p_pub->m_sockEvent_);
		p_pub->m_sockEvent_ = -1;
	}

	p_pub->m_threadIdAckRecv_ = (pthread_t)0;

	return;
}

#endif
