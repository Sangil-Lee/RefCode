/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdn_node_info.h $
* $Id: sdn_node_info.h 36408 2013-05-02 08:49:35Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API SdnNodeInfo header file.
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

/*! \file sdn_node_info.h 
 *  \brief SDN intialization API
 */
#ifndef SDN_NODE_INFO_H
#define SDN_NODE_INFO_H

#include <list>
#include <string>

#include "sdn_typedef.h"


#define	TIMEOUT_NODE_CONNECTION		5
#define	TIMEOUT_NODE_MESSAGE		3

#define	LOCK(x)		pthread_mutex_lock(&x)
#define	UNLOCK(x)	pthread_mutex_unlock(&x)

///////////////////////////////////////////////////////////////////////////
// environment variables
#define SDN_CONFIG_DIRECTORY_ENVNAME			"SDN_CONFIG_DIRECTORY"
#define SDN_CONFIG_DIRECTORY_DEFAULT			"/etc/opt/codac/sdn"

#define	SDN_INTERFACE_NAME_ENVNAME				"SDN_INTERFACE_NAME"
#define	SDN_PON_HOSTNAME_ENVNAME				"SDN_PON_HOSTNAME"
#define	SDN_APP_NAME_ENVNAME					"SDN_APP_NAME"


///////////////////////////////////////////////////////////////////////////
// variable enum types
typedef enum {
	STATUS_SDN_NOT_READY,
	STATUS_SDN_READY,
} STATUS_SDN;


typedef enum {
	STATUS_NODE_INIT,
	STATUS_NODE_CONNECTING,
	STATUS_NODE_CONNECTED,
	STATUS_NODE_REGISTERING,
	STATUS_NODE_REGISTERED,
	STATUS_NODE_UNREGISTERING,
	STATUS_NODE_UNREGISTERED,
} STATUS_NODE;


typedef enum {
	STATUS_PUBSUB_INIT,
	STATUS_PUBSUB_REGISTERING,
	STATUS_PUBSUB_REGISTERED,
	STATUS_PUBSUB_UNREGISTERING,
	STATUS_PUBSUB_UNREGISTERED,
} STATUS_PUBSUB;


typedef enum {
	STATUS_RSP_INIT,
	STATUS_RSP_ACK,
	STATUS_RSP_NACK,
} STATUS_RSP;



/// class PeerInfo 
/// describes peer information which need acknowledge for event topics.
/// It includes hostname, appname, and ipaddr.  
/// It will be used for topicname configuration check.
class 	PeerInfo {
public:
	std::string m_nameHost;		//!< host name of the controller
	std::string m_nameApp;		//!< application name 
	std::string m_addrIp;		//!< IP address of the node
};


/// class PubsubInfo 
/// describes topicname metadata; name, version and length.  
/// It will be used for topicname configuration check.
class 	PubsubInfo {
public:
	char m_isPublisher;				//!< indicates if this object is for publisher or subscriber
	std::string m_nameTopic;		//!< topic name of metadata
	std::string m_descTopic;		//!< topic description of metadata
	unsigned short m_versionTopic; 	//!< topic version of metadata
	unsigned short m_sizePayload;	//!< payload size of the topic userdata
	char m_isEvent;					//!< indicates if topic is event or not
	std::string m_mcastIpAddr;		//!< multicast IP address
	unsigned short m_mcastPort;		//!< multicast IP port number
	STATUS_PUBSUB m_statusPubsub;	//!< status of the node registration
	STATUS_RSP m_statusPubsubRsp;	//!< status of the node registration response
	std::list<PeerInfo> m_listPeer; //!< list of peer node information
}; 


/// class SdnNodeInfo
/// describes node's publishing and subscription informations.
/// sdn network IP address, hostname,  process id, list of topicname on which the node is publishing and 
/// list of topicname on which the node is subscribing. 
///
class SdnNodeInfo {
public:
	pthread_mutex_t m_mutexSubList;	//!< mutex used set/get list of subscribers
	pthread_cond_t m_condSubList;	//!< condition variable used set/get list of subscribers
	std::list<PubsubInfo> m_listPubsub; //!< list of publishers and subscribers in this node

	SdnNodeInfo()
	{
		m_statusNode_ = STATUS_NODE_INIT;
		m_statusSdn_ = STATUS_SDN_NOT_READY;
		m_sockMonitoringNode_ = -1;
	};

	SR_RET initialize(char* if_name);
	SR_RET deinitialize();
	int isRegistered();

	SR_RET registerPubsub(const char* topic_name, const char* topic_desc, unsigned short topic_verion, 
			unsigned short payload_size, char is_event, char is_publisher,
			char* multicast_ip_addr, unsigned short multicast_port_number);
	SR_RET unregisterPubsub(const char* topic_name, char is_publisher);
	
	STATUS_SDN getStatusSdn();
	SR_RET getMulticastAddress(const char* topic_name, char* multicast_ip, unsigned short* multicast_port);

	char* getHostName();
	char* getAppName();
	char* getSdnIpAddr();

	int getPeerInfo(const char* topic_name, int is_publisher, int is_event, std::list<PeerInfo>& peer_info_list);
	
private:	
	char m_hostName_[128];
	char m_appName_[128];
	char m_sdnIpAddr_[MAX_IP_ADDR_LEN]; 
	pid_t m_pid_; 

	int m_useMonitoringNode_;
	pthread_mutex_t m_mutexNode_;
	int m_sockMonitoringNode_;
	STATUS_SDN m_statusSdn_;
	STATUS_NODE m_statusNode_;
	STATUS_RSP m_statusNodeRsp_;
	pthread_t m_idRecvThread_;

	int getInterfaceIp_(char* if_name, char* ip_addr, int ip_addr_len);
	int getProgramName_(char* buf, int buf_len);
	int registerNode_();
	int unregisterNode_();
	int sendNodeMessage_(char* p_send_message);
	int sendSimpleMessage_(const char* p_key, const char* p_value);
	int waitRsp_(STATUS_RSP* p_rsp);
	static void* threadRecv_(void* p_param);
	static void cleanupThreadRecv_(void* p_param);
	int processNodeMessage_(char* p_buf);
}; 



extern	SdnNodeInfo g_NodeInfo; 

extern SR_RET initializeSDN(char* if_name); 
extern SR_RET initializeSDN(char* def_conf_file, char* user_conf_file); 
extern SR_RET terminateSDN(); 
extern SR_RET setSDNLogLevel(int log_level);
extern SR_RET setSDNStatusCallback(void (*fnCallbackSDNStatus)(int is_ready));
extern SR_RET unsetSDNStatusCallback();
extern int isSDNStatusReady();
extern pthread_t getLogThreadId();

#endif 
