/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdn_subscriber.h $
* $Id: sdn_subscriber.h 36408 2013-05-02 08:49:35Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API SdnSubscriber header file.
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

/*! \file sdn_subscriber.h 
 *    \brief subscriber API
 */

#ifndef SDN_SUBSCRIBER_H
#define SDN_SUBSCRIBER_H

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
#include <netdb.h>

#include "../sdn_common/log_buffer.h"
#include "sdn_node_info.h"
#include "sdn_packet.h"

#undef SDNMSG_T
#define	SDNMSG_T(level, fmt, args...)	SDNMSG(level, ">%s< " fmt, m_csTopicMetaData_.getName(), ## args)


/*---------------- Template class SdnSubscriber declaration --------------------------------*/

/// class SdnSubscriber
/// is a template class for SDN topic subscription. 
///
/// The topic_metadata class and topic_userdata class on a topic are necessary as template parameters. 
/// This class holds communication parameters for the topic to be subscribed.
template <class topic_metadata, class topic_userdata>
class SdnSubscriber {
	public:
		SdnSubscriber()
		{
			m_nextPacketSequence_=1; 
			m_lostPackets_=0;
			m_sockfd_ = -1;
			m_bAsyncRecv_ = 0;
			m_fnCallbackReceive_ = NULL;
			m_threadIdAsyncRecv_ = (pthread_t)0;
			m_bCheckDelay_ = 0;

			m_sockEvent_ = -1;
			m_threadIdPubInfo_ = 0;
		}; 

		SR_RET setSubscriber(topic_metadata& theTopicMetaData); 
		SR_RET unsetSubscriber(); 

		SR_RET receive(topic_userdata& theTopicUserData, int timeout_usec=0); 

		SR_RET getAsyncUserData(topic_userdata& theTopicUserData); 
		SR_RET setReceiveCallback(SR_RET (*fnCallbackReceive)(topic_userdata&, SR_RET)); 
		SR_RET setReceiveCallback(SR_RET (*fnCallbackReceive)(SdnSubscriber<topic_metadata, topic_userdata>*, SR_RET)); 
		SR_RET unsetReceiveCallback();

		SR_RET enableCheckDelay(int usec);
		SR_RET disableCheckDelay();

		SR_RET getLatency(int& latency);
		SR_RET getPacketSequenceNumber(unsigned int& seqNumber);
		SR_RET getPacketSenderTime(struct timespec& senderTime);

		pthread_t getAsyncRecvThreadId();

	private:
		topic_metadata m_csTopicMetaData_;
		SdnPacket<topic_userdata> m_csSdnPacket_; 
		
		// socket information (set by setSubDgramSocket_())
		int m_sockfd_; 
		struct sockaddr_in m_sockaddr_; 
		char m_mcastIpAddr_[MAX_IP_ADDR_LEN]; 
		unsigned short  m_mcastPort_; 

		// async receive
		int m_bAsyncRecv_;
		SR_RET m_retAsyncRecv_;
		pthread_t m_threadIdAsyncRecv_;
		SR_RET (*m_fnCallbackReceive_)(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code);
		
		// check packet sequence number 
		unsigned int m_nextPacketSequence_; 
		unsigned int m_lostPackets_; 
		
		// check packet delay
		int m_bCheckDelay_;  		// for packet delay check enable
		int m_usDelayThreshold_;	// used as a delay threshold when m_bCheckDelay_ is true
		int m_usLatency_;			// save latency for the last packet when m_bCheckDelay_ is true

		// event management
		char m_isEvent_;
		int m_sockEvent_; 
		struct sockaddr_in m_sockaddrEvent_; 
		int m_lenSendAck_;
		char m_bufSendAck_[256];
		pthread_t m_threadIdPubInfo_;
		static void* threadPubInfo_(void* p_param);
		static void cleanupThreadPubInfo_(void* p_param);

		/*------------------- Internal function ----------------------------------------------*/
		SR_RET setSubDgramSocket_(char* multicast_ip, unsigned short multicast_port); 
		SR_RET unsetSubDgramSocket_(char* multicast_ip, unsigned short multicast_port);
		static void* threadAsyncReceive_(void* p_param);	// async receive thread
} ; 


/*---------- Template function declaration exposed as API --------------------------------*/

/*------------------- setsubscriber----------------------------------*/
/** \fn  SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setSubscriber(topic_metadata& theTopicMetaData)
 *  \brief This function needs a topic_metadata class as its parameter.
 *
 *  The topic related information, such as topic name, multicast address, and multicast port are filled from this parameter, 
 *  and socket is created with this information.
 *  @param[in] theTopicMetaData is the variable of type topic_metadata class defined in the topic definition file
 *  \warning After the SdnSubscriber object is created, setSubscriber()should be called first before any other member functions.
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setSubscriber(topic_metadata& theTopicMetaData) 
{
	SR_RET ret;
	
	SDNMSG(SM_LOG, ">%s< setSubscriber called.\n", theTopicMetaData.getName());

	if(g_NodeInfo.isRegistered() == 0)
	{
		SDNMSG(SM_ERR, ">%s< g_NodeInof is not initialized. initializeSDN() first.\n", theTopicMetaData.getName());
		return SR_ERR_NOT_INITIALIZED;
	}
	
	if(m_sockfd_ >= 0)
	{
		SDNMSG(SM_ERR, ">%s< topic is set already to this subscriber.\n", theTopicMetaData.getName());
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

	/* set datagram socket for the subscriber */
	ret = setSubDgramSocket_(m_mcastIpAddr_, m_mcastPort_);
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "setSubDgramSocket_() error. ret=0x%x\n", ret);
		return ret;
	}

	m_isEvent_ = m_csTopicMetaData_.isEvent();
	if(m_isEvent_)
	{
		// initialize send ack buffer
		// message format: "seq_num(4byte, little-endian) + host_name\0 + app_name\0"
		m_lenSendAck_ = 4 + strlen(g_NodeInfo.getHostName()) + 1 + strlen(g_NodeInfo.getAppName()) + 1;
		if(m_lenSendAck_ > (int)sizeof(m_bufSendAck_))
		{
			SDNMSG_T(SM_ERR, "invalid buffer size. host name or app name is too long.\n"); 
			unsetSubDgramSocket_(m_mcastIpAddr_, m_mcastPort_);
			return SR_ERROR;
		}
		
		memset(m_bufSendAck_, 0, m_lenSendAck_);
		strcpy(m_bufSendAck_ + 4, g_NodeInfo.getHostName());
		strcpy(m_bufSendAck_ + 4 + strlen(g_NodeInfo.getHostName()) + 1, g_NodeInfo.getAppName());

		// create thread for socket creation for send ack
		ret = pthread_create(&m_threadIdPubInfo_, NULL, &threadPubInfo_, this);
		if(ret != 0)
		{
			SDNMSG_T(SM_ERR, "pthread_create() failed.\n"); 
			unsetSubDgramSocket_(m_mcastIpAddr_, m_mcastPort_);
			return SR_ERROR;
		}
	}
		
	/* register to monitor node */
	ret = g_NodeInfo.registerPubsub(m_csTopicMetaData_.getName(), m_csTopicMetaData_.getDescription(),
			m_csTopicMetaData_.getVersion(), m_csTopicMetaData_.getPayloadsize(), 
			m_isEvent_, 0, m_mcastIpAddr_, m_mcastPort_);
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "registerPubsub() failed. ret=0x%x\n", ret);
		unsetSubDgramSocket_(m_mcastIpAddr_, m_mcastPort_);
		return ret;
	}
	
	SDNMSG_T(SM_INFO, "Subscriber initialized. Multicast addr [%s], Port [%d].\n", m_mcastIpAddr_, m_mcastPort_);
	
	return SR_OK;
}


/*------------------- unsetsubscriber----------------------------------*/
/** \fn  SR_RET SdnSubscriber<topic_metadata, topic_userdata>::unsetSubscriber()
 *	\brief This function should be called at the end of using SdnSubscriber object.
 *
 *	This function closes socket and cleans up before terminating the application or when no more data is required to be subscribed. 
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::unsetSubscriber() 
{
	SR_RET ret = SR_OK;
	
	SDNMSG_T(SM_LOG, "unsetSubscriber called.\n");

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "unsetSubscriber(): m_sockfd_ is invalid.\n");
		ret = SR_ERR_INVALID_SOCKET;
		return ret;
	}
	
    /* at the moment print udp dataloss here */
	SDNMSG_T(SM_INFO, "Stop subscriber. (data loss = %d packets)\n", m_lostPackets_);

	if(m_isEvent_)
		pthread_cancel(m_threadIdPubInfo_);

    /* Remove subscription request */
	ret = unsetSubDgramSocket_(m_mcastIpAddr_, m_mcastPort_); 
	if(ret != SR_OK)
	{
		SDNMSG_T(SM_ERR, "unsetSubDgramSocket_() error. ret=0x%x\n", ret);
		return ret;
	}

	/* unregister from monitor node	*/
	g_NodeInfo.unregisterPubsub(m_csTopicMetaData_.getName(), 0);

	return ret;
}


/* -----------------------receive ------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::receive(topic_userdata& theTopicUserData, int timeout_usec)
 *  \brief  Receive SDN packet on the topic.
 *
 *  The SDN packet is composed of SDN packet header and the user data payload. 
 *  The SDN packet header is managed in the SdnSubscriber object using the internal SdnPacket object, and then removed. 
 *  Packet loss, packet delay, and data integrity are checked in this routine, and the result is indicated with a return value. 
 *  Only user data payload is delivered to the user. The packet format of the user data payload is defined in the topic_userdata. 
 *  @param[out] theTopicUserData is the variable of type topic_userdata class defined in the topic definition file 
 *  @param[in] timeout_usec is optional parameter to define timeout in micro-seconds
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::receive(topic_userdata& theTopicUserData, int timeout_usec)
{
	unsigned int seqnumber; 
	signed short status; 
	int len = 0;
	struct sockaddr_in sockaddr = m_sockaddr_; 
	socklen_t socklen = (socklen_t)sizeof(struct sockaddr_in); 
	long sec;
	struct timespec curtime;
	fd_set fds;
	struct timeval timeout;
	int ret;
	int send_len;
	unsigned int* p_buf;
	
	SDNMSG_T(SM_TEST, "receive() start.\n");

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "receive(): m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}

	if(timeout_usec > 0)
	{
		timeout.tv_sec = timeout_usec / 1000000;
		timeout.tv_usec = timeout_usec % 1000000;

		FD_ZERO(&fds);
		FD_SET(m_sockfd_, &fds);
		ret = select(m_sockfd_+1, &fds, NULL, NULL, &timeout);
		if(ret < 0)
		{
			SDNMSG_T(SM_ERR, "select() error\n");
			return SR_ERR_SOCKET;
		}
		else if(ret == 0)
		{
			return SR_ERR_RECV_TIMEOUT;
		}
	}

	len = recvfrom(m_sockfd_, &m_csSdnPacket_.m_sdnPacket, m_csSdnPacket_.getSdnPacketLen(), 0, 
			(struct sockaddr*)&sockaddr, &socklen);	
	if(len < 0)
	{
		SDNMSG_T(SM_ERR, "Reading datagram message error\n");
		return SR_ERR_SOCKET;
	}
	else if(len == 0)
	{
		SDNMSG_T(SM_ERR, "Reading datagram message error. ret=0.\n");
		return SR_ERR_RECV;
	}
	else
	{
		status= m_csSdnPacket_.checkPacket();
		if (status ==0)
		{
			m_csSdnPacket_.getPacketSequence(seqnumber);

			if(m_isEvent_)
			{
				// send event acknowledge packet
				// message format: "seq_num(4byte, little-endian) + host_name\0 + app_name\0"
				if(m_sockEvent_ >= 0)
				{
					p_buf = (unsigned int*)m_bufSendAck_;
					*p_buf = seqnumber;
					send_len = sendto(m_sockEvent_, m_bufSendAck_, m_lenSendAck_, 0, 
							(struct sockaddr*)&m_sockaddrEvent_, sizeof(m_sockaddrEvent_));

					SDNMSG(SM_TEST, "send ack(%d)\n", seqnumber);

					if(send_len != m_lenSendAck_)
						SDNMSG(SM_WARN, "acknowledge send is incomplete\n");
				}
				else
					SDNMSG_T(SM_WARN, "socket for acknowledge is invalid\n");
			}

			if (seqnumber != m_nextPacketSequence_) 
			{
				SDNMSG_T(SM_ERR, "packet lost for sequence number %d.\n", m_nextPacketSequence_);
				m_lostPackets_++;
				m_nextPacketSequence_=seqnumber+1;
				return SR_ERR_PACKET_LOST;
			}
			else 
			{
				m_csSdnPacket_.getPayload(theTopicUserData); 
				m_nextPacketSequence_++; 
			}

			if(m_bCheckDelay_)
			{
				clock_gettime(CLOCK_REALTIME, &curtime);
			
				sec = curtime.tv_sec - m_csSdnPacket_.m_sdnPacket.sendertime.tv_sec;
				m_usLatency_ = (int)((sec*1000000000 + curtime.tv_nsec - m_csSdnPacket_.m_sdnPacket.sendertime.tv_nsec)) / 1000 ;
				SDNMSG(SM_TEST, "%ld, %ld, %ld, %ld, latency=%ld\n", curtime.tv_sec, curtime.tv_nsec, 
					m_csSdnPacket_.m_sdnPacket.sendertime.tv_sec, m_csSdnPacket_.m_sdnPacket.sendertime.tv_nsec, m_usLatency_);
				
				if(m_usLatency_ > m_usDelayThreshold_)
					return SR_ERR_PACKET_DELAY;
			}
		}
		else 
		{
			SDNMSG_T(SM_ERR, "datagram validity check error.\n");
			return SR_ERR_DATA_VALIDITY;
		}
	}

	SDNMSG_T(SM_TEST, "receive() end. len=%d\n", len);
	
	return SR_OK;
}


/* -----------------------getAsyncUserData------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getAsyncUserData(topic_userdata& theTopicUserData)
 *  \brief Get the SDN user data on the topic which is received asynchronously.
 *
 *  This function is valid only when the mode is set to asynchronous receive mode by setReceiveCallback(). 
 *  The most recently received user data payload is delivered. The packet format of the user data payload is 
 *	defined in the topic_userdata. 
 *  @param[out] theTopicUserData is the variable of type topic_userdata class defined in the topic definition file 
 *  \warning This function should be called as soon as the callback function set by the setReceiveCallback() is called. 
 *  The getAsyncUserData() retrieves user data payload from the most recently received SDN packet, 
 *  and the SDN packets which were not taken before this packet will be lost.
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getAsyncUserData(topic_userdata& theTopicUserData)
{
	m_csSdnPacket_.getPayload(theTopicUserData); 
	return m_retAsyncRecv_;
}


/* -----------------------setReceiveCallback------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setReceiveCallback(SR_RET (*fnCallbackReceive)(topic_userdata&, SR_RET))
 *  \brief obsolete API 
 *
 *  This function is obsolete API from CCS v4.1, and always returns SR_ERROR.
 *  Use setReceiveCallback(SR_RET (*fnCallbackReceive)(SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber, SR_RET err_code)).
 *  \warning This function will be removed from CCS v5.0 
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setReceiveCallback(SR_RET (*fnCallbackReceive)(topic_userdata&, SR_RET)) 
{
		SDNMSG_T(SM_ERR, "setReceiveCallback(SR_RET (*fn)(toic_userdata&, SR_RET)) is obsolete.\n");
		return SR_ERROR;
}


/* -----------------------setReceiveCallback------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setReceiveCallback(SR_RET (*fnCallbackReceive)(SdnSubscriber<topic_metadata, topic_userdata>*, SR_RET))
 *  \brief Set SdnSubscriber object to asynchronous receive mode. 
 *
 *  When the SdnSubscriber object is in asynchronous receive mode, user shall retrieve the user data payload 
 *  using fnCallbackReceive() or getAsyncUserData(), instead of receive()
 *  @param[in] fnCallbackReceive is the callback function called when the SDN packet on the corresponding topic is available 
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setReceiveCallback(
		SR_RET (*fnCallbackReceive)(SdnSubscriber<topic_metadata, topic_userdata>*, SR_RET)) 
{
	int i_ret;

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "setReceiveCallback(): m_sockfd_ is invalid.\n");
		return SR_ERR_INVALID_SOCKET;
	}

	if(m_bAsyncRecv_)
	{
		SDNMSG_T(SM_ERR, "Subscriber is already running in async receive mode. unsetReceiveCallback() first.\n");
		return SR_ERR_ALREADY;
	}

	m_bAsyncRecv_ = 1;
	m_fnCallbackReceive_ = fnCallbackReceive;

	SDNMSG_T(SM_LOG, "Spawning async receive thread\n");
	i_ret = pthread_create((pthread_t*)&m_threadIdAsyncRecv_, NULL, &threadAsyncReceive_, this);
	if(i_ret != 0)
	{
		SDNMSG_T(SM_ERR, "Create threadAsyncReceive_ thread error\n");
		m_bAsyncRecv_ = 0;
		m_fnCallbackReceive_ = NULL;
		return SR_ERROR;
	}

	return SR_OK;
}


/* -----------------------unsetReceiveCallback------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::unsetReceiveCallback()
 *  \brief Set SdnSubscriber object to synchronous receive mode.
 *
 *  When the SdnSubscriber object is in synchronous receive mode, which is default mode when the SdnSubscriber object is created, 
 *  user shall retrieve the user data payload using receive() function.
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::unsetReceiveCallback() 
{
	int i_ret;
	
	if(!m_bAsyncRecv_)
	{
		SDNMSG_T(SM_ERR, "unsetReceiveCallback() error. Subscriber is not running in async receive mode.\n");
		return SR_ERROR;
	}

	if(m_threadIdAsyncRecv_)
	{
		i_ret = pthread_cancel(m_threadIdAsyncRecv_);
		if (i_ret != 0)
			SDNMSG_T(SM_WARN, "unsetReceiveCallback: pthread_cancel() error.\n");

		m_threadIdAsyncRecv_ = (pthread_t)0;
	}

	m_bAsyncRecv_ = 0;

	return SR_OK;
}


/* -----------------------enableCheckDelay ------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::enableCheckDelay(int usec)
 *  \brief Enable packet delay inspection.
 *
 *  In this mode, receive() function compares time flag of each packet which is set to the system time of the publisher 
 *  to the system time of the subscriber. If the time difference is greater than the usec parameter, 
 *  receive() function returns with error code SR_ERR_PACKET_DELAY.
 *  @param[in] usec is the packet delay threshold in micro-seconds  
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::enableCheckDelay(int usec)
{
	m_bCheckDelay_ = 1;
	m_usDelayThreshold_ = usec;

	return SR_OK;
}


/* -----------------------disableCheckDelay ------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::disableCheckDelay()
 *  \brief Disable packet delay inspection 
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::disableCheckDelay()
{
	m_bCheckDelay_ = 0;

	return SR_OK;
}


/* -----------------------getLatency ------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getLatency(int& latency)
 *  \brief Get latency between send to receive.
 *
 *  This function is valid only when delay check is enabled by enableCheckDelay()
 *  @param[out] latency	 latency for the last packet  
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getLatency(int& latency)
{
	latency = m_usLatency_;

	if(m_bCheckDelay_)
		return SR_ERROR;
	
	return SR_OK;
}



/* -----------------------getPacketSequenceNumber------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getPacketSequenceNumber(unsigned int& seqNumber)
 *  \brief Get the sequence number of SDN packet header.
 *
 *  @param[out] seqNumber is the sequence number of the last SDN packet header 
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getPacketSequenceNumber(unsigned int& seqNumber)
{
	m_csSdnPacket_.getPacketSequence(seqNumber); 
	return SR_OK;
}


/* -----------------------getPacketSenderTime------------------------------*/
/** \fn SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getPacketSenderTime(struct timespec& senderTime)
 *  \brief Get the sendertime of SDN packet header.
 *
 *  The sendertime is set to system time of the publisher by the SDN software
 *  @param[out] senderTime is the sendertime of the last SDN packet header 
 */
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::getPacketSenderTime(struct timespec& senderTime)
{
	m_csSdnPacket_.getSenderTime(senderTime); 
	return SR_OK;
}


/** \fn pthread_t SdnSubscriber<topic_metadata, topic_userdata>::getAsyncRecvThreadId()
 *  \brief Gets the threadID of asynchronous data receive thread.
 *
 *	The threadID is used to change thread attribute such as scheduling policy and priority.
 *	Asynchronous data receive thread is SdnPublisher internal thread which receives 
 *	topic data from publisher asynchronously.
 *	Asynchronous receive is enabled by setReceiveCallback() API
 */
template <class topic_metadata, class topic_userdata>
pthread_t SdnSubscriber<topic_metadata, topic_userdata>::getAsyncRecvThreadId()
{
	return m_threadIdAsyncRecv_;
};



/*------------------- Internal Functions ----------------------------------*/

/*------------------- setSubDramSocket----------------------------------*/
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::setSubDgramSocket_(char* multicast_ip, unsigned short multicast_port)
{
	struct ip_mreq subGroup ; 
	char sdnipaddr[MAX_IP_ADDR_LEN]; 

	strncpy(sdnipaddr, g_NodeInfo.getSdnIpAddr(), MAX_IP_ADDR_LEN); 

	/* Create a datagram socket on which to receive. */
	m_sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "Opening datagram socket error\n");
		return SR_ERR_INVALID_SOCKET;
	}
	else
	{
		SDNMSG_T(SM_LOG, "Opening datagram socket....OK.\n");
	}

	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	{
		int reuse = 1;

		if(setsockopt(m_sockfd_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
		{
			SDNMSG_T(SM_ERR, "Setting SO_REUSEADDR error\n");
			close(m_sockfd_);
			m_sockfd_ = -1;
			return SR_ERR_SOCKET;
		}
		else
		{
			SDNMSG_T(SM_LOG, "Setting SO_REUSEADDR...OK.\n");
		}
	}

	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char*)&m_sockaddr_, 0, sizeof(m_sockaddr_));
	m_sockaddr_.sin_family = AF_INET;
	m_sockaddr_.sin_port = htons(multicast_port);
	m_sockaddr_.sin_addr.s_addr = inet_addr(multicast_ip);
	if(bind(m_sockfd_, (struct sockaddr*)&m_sockaddr_, sizeof(m_sockaddr_)) != 0)
	{
		SDNMSG_T(SM_ERR, "Binding datagram socket error\n");
		close(m_sockfd_);
		m_sockfd_ = -1;
		return SR_ERR_SOCKET;
	}

	/* Join the multicast group on the local interface */
 	/* Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	subGroup.imr_multiaddr.s_addr = inet_addr(multicast_ip);
	subGroup.imr_interface.s_addr = inet_addr(sdnipaddr);
	if(setsockopt(m_sockfd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&subGroup, sizeof(subGroup)) < 0)
	{
		SDNMSG_T(SM_ERR, "Adding multicast group error\n");
		close(m_sockfd_);
		m_sockfd_ = -1;
		return SR_ERR_SOCKET;
	}

	//int opt = 0;
	//setsockopt(m_sockfd_, IPPROTO_IP, IP_MULTICAST_ALL, &opt, sizeof(opt));

	SDNMSG_T(SM_LOG, "Adding multicast group...OK.\n");
	
	return SR_OK;
}


/*---------- unsubscribe from the topic and close the socket -----------------------*/
template <class topic_metadata, class topic_userdata>
SR_RET SdnSubscriber<topic_metadata, topic_userdata>::unsetSubDgramSocket_(char* multicast_ip, unsigned short multicast_port)
{
	struct ip_mreq subGroup ; 

    subGroup.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    subGroup.imr_interface.s_addr = inet_addr(g_NodeInfo.getSdnIpAddr());

	if(m_sockfd_ < 0)
	{
		SDNMSG_T(SM_ERR, "Error! socket is not opened\n");
		return SR_ERR_INVALID_SOCKET;
	}
	else
	{
		if(setsockopt(m_sockfd_, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&subGroup, sizeof(subGroup)) < 0)
		{
			SDNMSG_T(SM_ERR, "unsubscribe multicast group error\n");
		}
	}

	close(m_sockfd_); 
	m_sockfd_ = -1;

	return SR_OK;
}


/*---------- asynchronous receive thread created by setReceiveCallback() -----------------------*/
template <class topic_metadata, class topic_userdata>
void* SdnSubscriber<topic_metadata, topic_userdata>::threadAsyncReceive_(void* p_param)
{
	unsigned int seqnumber; 
	signed short status; 
	int len;
	struct sockaddr_in sockaddr; 
	socklen_t socklen; 
	int i_ret;
	unsigned short packet_len;
	long sec;
	struct timespec curtime;
	int send_len;
	unsigned int* p_buf;
	SdnSubscriber<topic_metadata, topic_userdata>* p_subscriber = (SdnSubscriber<topic_metadata, topic_userdata>*)p_param;
	
	if(p_subscriber->m_sockfd_ < 0)
	{
		SDNMSG(SM_ERR, ">%s< m_sockfd_ is invalid.\n", p_subscriber->m_csTopicMetaData_.getName());
		p_subscriber->m_retAsyncRecv_ = SR_ERR_INVALID_SOCKET;
		return (void*)1;
	}

	packet_len = p_subscriber->m_csSdnPacket_.getSdnPacketLen();
	
	i_ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (i_ret != 0)
		SDNMSG(SM_ERR, ">%s< pthread_setcancelstate(PTHREAD_CANCEL_ENABLE) failed.\n", p_subscriber->m_csTopicMetaData_.getName());
	
	while(p_subscriber->m_bAsyncRecv_ && (p_subscriber->m_sockfd_ >= 0))
	{
		sockaddr = p_subscriber->m_sockaddr_;
		socklen = (socklen_t)sizeof(struct sockaddr);
		
		len = recvfrom(p_subscriber->m_sockfd_, &p_subscriber->m_csSdnPacket_.m_sdnPacket, packet_len, 0, 
				(struct sockaddr*)&sockaddr, &socklen);	
		if(len < 0)
		{
			SDNMSG(SM_ERR, ">%s< Reading datagram message error. ret<0\n", p_subscriber->m_csTopicMetaData_.getName());
			p_subscriber->m_retAsyncRecv_ = SR_ERR_SOCKET; 
		}
		else if(len == 0)
		{
			SDNMSG(SM_ERR, ">%s< Reading datagram message error. ret=0.\n", p_subscriber->m_csTopicMetaData_.getName());
			p_subscriber->m_retAsyncRecv_ = SR_ERR_RECV; 
		}
		else
		{
			p_subscriber->m_retAsyncRecv_ = SR_OK;
			
			status = p_subscriber->m_csSdnPacket_.checkPacket();
			if (status == 0)
			{
				p_subscriber->m_csSdnPacket_.getPacketSequence(seqnumber);
				if(p_subscriber->m_isEvent_)
				{
					// send event acknowledge packet
					// message format: "seq_num(4byte, little-endian) + host_name\0 + app_name\0"
					if(p_subscriber->m_sockEvent_ >= 0)
					{
						p_buf = (unsigned int*)p_subscriber->m_bufSendAck_;
						*p_buf = seqnumber;
						send_len = sendto(p_subscriber->m_sockEvent_, p_subscriber->m_bufSendAck_, p_subscriber->m_lenSendAck_, 0, 
								(struct sockaddr*)&p_subscriber->m_sockaddrEvent_, sizeof(p_subscriber->m_sockaddrEvent_));

						if(send_len != p_subscriber->m_lenSendAck_)
							SDNMSG(SM_WARN, "acknowledge send is incomplete\n");
					}
					else
						SDNMSG(SM_WARN, "socket for acknowledge is invalid\n");
				}

				if (seqnumber != p_subscriber->m_nextPacketSequence_) 
				{
					SDNMSG(SM_ERR, ">%s< packet lost for sequence number %d.\n", p_subscriber->m_csTopicMetaData_.getName(), p_subscriber->m_nextPacketSequence_);
					p_subscriber->m_lostPackets_++;
					p_subscriber->m_nextPacketSequence_ = seqnumber + 1;
					p_subscriber->m_retAsyncRecv_ = SR_ERR_PACKET_LOST; 
				}
				else 
				{
					p_subscriber->m_nextPacketSequence_++; 
				}

				if(p_subscriber->m_bCheckDelay_)
				{
					clock_gettime(CLOCK_REALTIME, &curtime);
				
					sec = curtime.tv_sec - p_subscriber->m_csSdnPacket_.m_sdnPacket.sendertime.tv_sec;
					p_subscriber->m_usLatency_ = (int)((sec*1000000000 + curtime.tv_nsec 
													- p_subscriber->m_csSdnPacket_.m_sdnPacket.sendertime.tv_nsec)) / 1000 ;
					SDNMSG(SM_TEST, "%ld, %ld, %ld, %ld, latency=%ld\n", curtime.tv_sec, 
						curtime.tv_nsec, p_subscriber->m_csSdnPacket_.m_sdnPacket.sendertime.tv_sec, 
						p_subscriber->m_csSdnPacket_.m_sdnPacket.sendertime.tv_nsec, p_subscriber->m_usLatency_);
					
					if(p_subscriber->m_usLatency_ > p_subscriber->m_usDelayThreshold_)
						p_subscriber->m_retAsyncRecv_ = SR_ERR_PACKET_DELAY; 
				}
			}
			else 
			{
				SDNMSG(SM_ERR, ">%s< datagram validity check error.\n", p_subscriber->m_csTopicMetaData_.getName());
				p_subscriber->m_retAsyncRecv_ = SR_ERR_DATA_VALIDITY; 
			}
		}

		if(p_subscriber->m_fnCallbackReceive_ == NULL)
		{
			SDNMSG(SM_ERR, ">%s< m_fnCallbackReceive_ pointer is NULL\n", p_subscriber->m_csTopicMetaData_.getName());
			break;
		}
			
		p_subscriber->m_fnCallbackReceive_(p_subscriber, p_subscriber->m_retAsyncRecv_);
	}

	p_subscriber->m_threadIdAsyncRecv_ = (pthread_t)0;
	
	return (void*)0;
}



/*--- thread to set socket for send event acknowledge received by nodeinfo (only created when the topic is event) ---*/
template <class topic_metadata, class topic_userdata>
void* SdnSubscriber<topic_metadata, topic_userdata>::threadPubInfo_(void* p_param)
{
#define	MAX_RECV_BUF_SIZE			1500
	int ret;
	SdnSubscriber<topic_metadata, topic_userdata>* p_sub = (SdnSubscriber<topic_metadata, topic_userdata>*)p_param;
	std::list<PeerInfo> list_peer_info;
	std::list<PeerInfo>::iterator iter_peer;

	if(p_sub == NULL)
	{
		SDNMSG(SM_ERR, "invalid null parameter\n");
		return (void*)1;
	}

	pthread_cleanup_push(p_sub->cleanupThreadPubInfo_, p_sub);

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (ret != 0)
		SDNMSG(SM_WARN,"pthread_setcancelstate(PTHREAD_CANCEL_ENABLE) failed.\n");

	while(1)
	{
		LOCK(g_NodeInfo.m_mutexSubList);

		pthread_cond_wait(&g_NodeInfo.m_condSubList, &g_NodeInfo.m_mutexSubList);

		// [AckList] check publisher info for this topic, and exit loop if it is available
		if(g_NodeInfo.getPeerInfo(p_sub->m_csTopicMetaData_.getName(), 0, p_sub->m_isEvent_, list_peer_info) == 0)
		{
			if(!list_peer_info.empty())
			{
				UNLOCK(g_NodeInfo.m_mutexSubList);
				break;
			}
		}

		UNLOCK(g_NodeInfo.m_mutexSubList);
	}

	iter_peer = list_peer_info.begin(); 
	if(iter_peer != list_peer_info.end())
	{
		p_sub->m_sockEvent_ = socket(AF_INET, SOCK_DGRAM, 0);
		if(p_sub->m_sockEvent_ < 0)
		{
			SDNMSG(SM_ERR, "Opening datagram socket error\n");
			return (void*)-1;
		}

		memset((char*) &p_sub->m_sockaddrEvent_, 0, sizeof(p_sub->m_sockaddrEvent_));
		p_sub->m_sockaddrEvent_.sin_family = AF_INET;
		p_sub->m_sockaddrEvent_.sin_port = htons(p_sub->m_mcastPort_ + 1);
		p_sub->m_sockaddrEvent_.sin_addr.s_addr = inet_addr((*iter_peer).m_addrIp.c_str());
	}

	pthread_cleanup_pop(1);

	return (void*)0;
}



template <class topic_metadata, class topic_userdata>
void SdnSubscriber<topic_metadata, topic_userdata>::cleanupThreadPubInfo_(void* p_param)
{
	UNLOCK(g_NodeInfo.m_mutexSubList);
	return;
}



#endif
