/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdn_typedef.h $
* $Id: sdn_typedef.h 36311 2013-04-29 08:07:33Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API sdn_typedef header file.
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

/*! \file sdn_typedef.h 
 *  \brief defines SDN typedefs   
 */
#ifndef SDN_TYPEDEF_H
#define SDN_TYPEDEF_H

#include <stdio.h>
#include <stdint.h>

/** \brief pack the classes to make the code portable between different endianess machines
 */
#define PACKED __attribute__((__packed__)) 

/* SDN Return Value */
#define	SR_RET			int
	#define	SR_OK			0
	#define	SR_ERROR		1
	// socket error
	#define SR_ERR_INVALID_SOCKET	0x101
	#define	SR_ERR_SOCKET			0x102
	// send/receive error
	#define SR_ERR_RECV 			0x201
	#define	SR_ERR_SEND				0x202
	#define	SR_ERR_PACKET_LOST		0x203
	#define	SR_ERR_PACKET_LENGTH	0x204
	#define	SR_ERR_DATA_VALIDITY	0x205
	#define SR_ERR_PACKET_DELAY		0x206
	#define SR_ERR_RECV_TIMEOUT		0x207
	// status error
	#define SR_ERR_ALREADY			0x301
	#define SR_ERR_NOT_INITIALIZED	0x302

/* Topic Definition Module */
#define	MAX_TOPIC_NAME_LEN				40
#define	MAX_TOPIC_DESCRIPTION_LEN		80

#define	MAX_IP_ADDR_LEN				26

// userdata type definition
// 	-- predefined userdata type: float, double
#define	uint8_t		unsigned char
#define	uint16_t	unsigned short
#define	uint32_t	unsigned int
#define	uint64_t	unsigned long
#define	int8_t		char
#define	int16_t		short
#define	int32_t		int
#define	int64_t		long


#endif 
