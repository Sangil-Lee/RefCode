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
 *   \brief Header File for SDNMON Register Message  
 */

#ifndef SDNMON_MSG_REG_H
#define SDNMON_MSG_REG_H


/////////////////////////////////////////////////////////
// Keys
/////////////////////////////////////////////////////////

#define	KEY_CMD				"CMD"
#define	KEY_TOPICNAME		"TNAME"				// valid when VAL_CMD__PUB(SUB)_UNREG, NOTIFY_SUB_LIST, NOTIFY_PUB_INFO, NOTIFY_TOPIC_STATUS
#define KEY_TOPICSTATUS		"TOPIC_STS"			// valid when VAL_CMD__NOTIFY_TOPIC_STATUS
#define KEY_SDNSTATUS		"SDN_STS"			// valid when VAL_CMD__NOTIFY_SDN_STATUS
#define	KEY_META			"META"				// valid when VAL_CMD__PUB(SUB)_REG
	#define	KEY_META_TOPICNAME			"TNAME"
	#define	KEY_META_TOPICDESC			"TDESC"
	#define	KEY_META_TOPICVER			"TVER"
	#define	KEY_META_PAYLOADSIZE		"PLSIZE"
	#define	KEY_META_ISEVENT			"ISEVENT"
#define	KEY_SUBLIST			"SUBLIST"			// valid when VAL_CMD__NOTIFY_SUB_LIST
	#define	KEY_SUBLIST_SUB				"SUB"		// can be duplicated
		#define	KEY_SUBLIST_SUB_HOSTNAME			"HOST"			
		#define	KEY_SUBLIST_SUB_APPNAME				"APP"			
		#define	KEY_SUBLIST_SUB_IP					"IP"
#define	KEY_PARAM			"PARAM"
	// KEY_CMD is VAL_CMD__NODE_REG, NOTIFY_PUB_INFO
	#define	KEY_PARAM_HOSTNAME			"HOST"
	#define	KEY_PARAM_APPNAME			"APP"
	#define	KEY_PARAM_IP				"IP"


/////////////////////////////////////////////////////////
// Values
/////////////////////////////////////////////////////////

// values for KEY_CMD
#define	VAL_CMD__NODE_REG				"NODE_REG"
#define	VAL_CMD__NODE_REG_ACK			"NODE_REG_ACK"
#define	VAL_CMD__NODE_REG_NACK			"NODE_REG_NACK"
#define	VAL_CMD__NODE_UNREG				"NODE_UNREG"
#define	VAL_CMD__NODE_UNREG_ACK			"NODE_UNREG_ACK"
#define	VAL_CMD__NODE_UNREG_NACK		"NODE_UNREG_NACK"
#define	VAL_CMD__PUB_REG				"PUB_REG"
#define	VAL_CMD__PUB_REG_ACK			"PUB_REG_ACK"
#define	VAL_CMD__PUB_REG_NACK			"PUB_REG_NACK"
#define	VAL_CMD__SUB_REG				"SUB_REG"
#define	VAL_CMD__SUB_REG_ACK			"SUB_REG_ACK"
#define	VAL_CMD__SUB_REG_NACK			"SUB_REG_NACK"
#define	VAL_CMD__PUB_UNREG				"PUB_UNREG"
#define	VAL_CMD__PUB_UNREG_ACK			"PUB_UNREG_ACK"
#define	VAL_CMD__PUB_UNREG_NACK			"PUB_UNREG_NACK"
#define	VAL_CMD__SUB_UNREG				"SUB_UNREG"
#define	VAL_CMD__SUB_UNREG_ACK			"SUB_UNREG_ACK"
#define	VAL_CMD__SUB_UNREG_NACK			"SUB_UNREG_NACK"
#define	VAL_CMD__NOTIFY_TOPIC_STATUS	"NOTIFY_TOPIC_STATUS"
#define	VAL_CMD__NOTIFY_SDN_STATUS		"NOTIFY_SDN_STATUS"
#define	VAL_CMD__NOTIFY_PUB_INFO		"NOTIFY_PUB_INFO"
#define	VAL_CMD__NOTIFY_SUB_LIST		"NOTIFY_SUB_LIST"
#define	VAL_CMD__ALIVE					"ALIVE"
#define	VAL_CMD__ALIVE_ACK				"ALIVE_ACK"

// values for KEY_META_ISEVENT
#define	VAL_META_ISEVENT__TRUE			"1"
#define	VAL_META_ISEVENT__FALSE			"0"

// values for KEY_SDNSTATUS, KEY_TOPICSTATUS
#define	VAL_STATUS__READY			"READY"
#define	VAL_STATUS__NOT_READY		"NOT READY"

#endif

