#ifndef _IRTV_DATA_CLIENT_H
#define _IRTV_DATA_CLIENT_H

#include "irtvDataService.h"

#include <map>
#include <string>

using namespace::std;

#if 1
#define	IRTV_SVR_IP_ADDR	"172.17.102.127"
#else
#define	IRTV_SVR_IP_ADDR	"127.0.0.1"
#endif

extern int createDataRecvThr (void *arg);

class IrtvSvcDataAssembler
{
public :
	IrtvSvcDataAssembler () { init (); };
	~IrtvSvcDataAssembler () {};

	void setTarget (GrabFrameData *pTargetData) {
		m_pTargetData	= pTargetData;
	}

	void getData (GrabFrameData &data) {
		memcpy ((char *)&data, (char *)&m_data, sizeof(GrabFrameData));
	}

	void addData (IrtvSvcUdpData &udpData) {
		if (1 == udpData.m_header.m_nMsgSeq) {
			resetData (false);
		}

		if ( m_nLastMsgSeq != (udpData.m_header.m_nMsgSeq - 1) ) {
			kLog (K_MON, "[IrtvSvcDataAssembler::addData] last(%d) curr(%d) \n", m_nLastMsgSeq, udpData.m_header.m_nMsgSeq);
		}

		m_nLastMsgSeq	= udpData.m_header.m_nMsgSeq;

		// data 조립
		int offset	= (udpData.m_header.m_nMsgSeq - 1) * IRTV_MTU;

		memcpy (m_pLocalData + offset, udpData.m_data, udpData.m_header.m_nMsgLen);

		kLog (K_DEL, "[IrtvSvcDataAssembler::addData] offset(%d) msgLen(%d) \n", offset, udpData.m_header.m_nMsgLen);

		// target에 data 복사
		if (1 == udpData.m_header.m_bLastMsg) {
			applyData ();
		}
	}

public :
	GrabFrameData	m_data;
	GrabFrameData	*m_pTargetData;
	char			*m_pLocalData;
	int				m_nLastMsgSeq;

	void init () {
		m_pLocalData	= (char *)&m_data;
		m_pTargetData	= NULL;
		m_nLastMsgSeq	= 0;

		resetData (true);
	}

	void resetData (const bool bResetData) {
		kLog (K_INFO, "[resetData]  ...\n");

		m_nLastMsgSeq	= 0;

		if (true == bResetData) {
			memset ((char *)&m_data, 0x00, sizeof(GrabFrameData));
		}
	}

	void applyData () {
		kLog (K_INFO, "[applyData]  ...\n");

		if (NULL != m_pTargetData) {
			//TODO
			memcpy ((char *)m_pTargetData, (char *)&m_data, sizeof(GrabFrameData));
		}
	}
};

#endif // _IRTV_DATA_CLIENT_H
