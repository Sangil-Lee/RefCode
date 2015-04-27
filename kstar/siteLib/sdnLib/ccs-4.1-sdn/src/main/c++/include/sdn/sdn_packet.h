/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdn_packet.h $
* $Id: sdn_packet.h 35799 2013-04-17 08:24:10Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API SdnPacket header file.
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

/*! \file sdn_packet.h 
 *  \brief defines SDN packet structure  
 */
#ifndef SDN_PACKET_H
#define SDN_PACKET_H

/** Packet signature 
 *  used for packet verification 
 */
#define MAGIC_NUMBER  0x5a6b7c8e


#include <time.h>
#include <iostream>

#include "sdn_typedef.h"
#include "../sdn_common/log_buffer.h"



/// class SdnPacket
/// A template class defines SDN packet format with topicname as a template parameter.  
/// SDN packet is UDP payload to be used in UDP multicast 
template <class topic_userdata>
class PACKED SdnPacket {
	public:
		SdnPacket()
		{
			m_sdnPacket.magicnumber = MAGIC_NUMBER; 
			m_sdnPacket.packetsequence = 0;
		};

		signed short checkPacket(); 
		void* getSdnPacket();
		int getSdnPacketLen();

		void setPayload(topic_userdata& payload); 
		void getPayload (topic_userdata& payload); 

		void setPacketSequence(unsigned int seqnumber); 
		void getPacketSequence(unsigned int& seqnumber); 
		void incrPacketSequence(); 

		void setSenderTime(int us_delay=0 /* option is used only for test */); 
		void getSenderTime(struct timespec& sendertime); 

		struct 
		{
			uint32_t magicnumber;
			uint32_t packetsequence;
			struct timespec sendertime;
			topic_userdata payload;
		} m_sdnPacket;
};



/*----------------- class SdnPacket implementation --------------------*/

/** \fn signed short SdnPacket<topic_userdata>::checkPacket()
 *	\brief Check the packet is valid by checking the magicnumber. 
 */
template <class topic_userdata>
signed short SdnPacket<topic_userdata>::checkPacket()
{
	if(m_sdnPacket.magicnumber == MAGIC_NUMBER)
		return 0;
	else 
		return -1;
}


/** \fn void* SdnPacket<topic_userdata>::getSdnPacket()
 *	\brief Get the pointer of current packet.
 */
template <class topic_userdata>
void* SdnPacket<topic_userdata>::getSdnPacket()
{
	return (void*)&m_sdnPacket;
}


/** \fn int SdnPacket<topic_userdata>::getSdnPacketLen()
 *	\brief Get the length of current packet. 
 */
template <class topic_userdata>
int SdnPacket<topic_userdata>::getSdnPacketLen()
{
	return sizeof(m_sdnPacket);
}


/** \fn void SdnPacket<topic_userdata>::setPayload(topic_userdata& topic)
 *	\brief Set payload of current packet. 
 *	@param[in] topic is the topic userdata which will be set to payload.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::setPayload(topic_userdata& topic)
{
	m_sdnPacket.payload=topic;
}


/** \fn void SdnPacket<topic_userdata>::getPayload (topic_userdata& payload)
 *	\brief Get payload of current packet. 
 *	@param[out] payload is the topic userdata which is the payload of SdnPacket.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::getPayload (topic_userdata& payload)
{
	payload = m_sdnPacket.payload;
}


/** \fn void SdnPacket<topic_userdata>::setPacketSequence(unsigned int seqnumber)
 *	\brief Set the sequence number of the packet. 
 *	@param[in] seqnumber is the sequence number of the packet.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::setPacketSequence(unsigned int seqnumber)
{
	m_sdnPacket.packetsequence=seqnumber;
}


/** \fn void SdnPacket<topic_userdata>::getPacketSequence(unsigned int& seqnumber)
 *	\brief Get the sequence number of the packet. 
 *	@param[out] seqnumber is the output parameter in which sequence number of the packet is returned.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::getPacketSequence(unsigned int& seqnumber)
{
	seqnumber = m_sdnPacket.packetsequence;
}


/** \fn void SdnPacket<topic_userdata>::incrPacketSequence()
 *	\brief Increment sequence number of the packet.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::incrPacketSequence()
{
	m_sdnPacket.packetsequence++;
}


/** \fn void SdnPacket<topic_userdata>::setSenderTime(int us_delay)
 *	\brief Set sendertime of the packet to the current system time. 
 *	@param[in] us_delay is used add or subtract from the real system time. Other values than 0 is used for test purpose in most cases.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::setSenderTime(int us_delay)
{
	clock_gettime(CLOCK_REALTIME, &m_sdnPacket.sendertime); 
	if(us_delay != 0)
	{
		if(us_delay > 0)
		{
			if(m_sdnPacket.sendertime.tv_nsec + (us_delay % 1000000) * 1000 > 1000000000)
			{
				m_sdnPacket.sendertime.tv_sec += ((us_delay / 1000000) + 1);
				m_sdnPacket.sendertime.tv_nsec = (m_sdnPacket.sendertime.tv_nsec + (us_delay % 1000000) * 1000) - 1000000000;
			}
			else
			{
				m_sdnPacket.sendertime.tv_sec += (us_delay / 1000000);
				m_sdnPacket.sendertime.tv_nsec += (us_delay % 1000000) * 1000;
			}
		}
		else
		{
			if(m_sdnPacket.sendertime.tv_nsec + (us_delay % 1000000) * 1000 < 0)
			{
				m_sdnPacket.sendertime.tv_sec += ((us_delay / 1000000) - 1);
				m_sdnPacket.sendertime.tv_nsec = (m_sdnPacket.sendertime.tv_nsec + (us_delay % 1000000) * 1000) + 1000000000;
			}
			else
			{
				m_sdnPacket.sendertime.tv_sec += (us_delay / 1000000);
				m_sdnPacket.sendertime.tv_nsec += (us_delay % 1000000) * 1000;
			}
		}
	}
	
}


/** \fn void SdnPacket<topic_userdata>::getSenderTime(struct timespec& sendertime)
 *	\brief Get sendertime of the packet. 
 *	@param[in] sendertime indicates the time when the packet is sent.
 */
template <class topic_userdata>
void SdnPacket<topic_userdata>::getSenderTime(struct timespec& sendertime)
{
	sendertime = m_sdnPacket.sendertime;
}
#endif 
