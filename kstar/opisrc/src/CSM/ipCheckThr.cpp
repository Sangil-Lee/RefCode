/*************************************************************************\
* Copyright (c) 2010 The National Fusion Research Institute
\*************************************************************************/
/*  
 *
 *         NFRI (National Fusion Research Institute)
 *    113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 */

/*  qtchaccessthr.cpp
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 *
 * Revision 1  2006-12-29
 * Author: Sangil Lee [silee]
 * Initial revision
 *
 * ---------------------------------------------------------------------------
 *  * DESCRIPTION *
 * ---------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------
 *  * System Include Files *
 * ---------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------
 *  * User Type Definitions *
 * ---------------------------------------------------------------------------
 */
#include <algorithm>
#include <set>
#include "icmpsocket.h"
#include "ipCheckThr.h"
#include "blinkLine.h"
/* ---------------------------------------------------------------------------
 *  * Imported Global Variables & Function Declarations *
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 *  * Exported Global Variables & Function Declarations *
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 *  * Module Variables & Function Declarations *
 * ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
IPCheckThr::IPCheckThr(ControlMonitorChannel *pattach, QObject *parent):QThread(parent),m_pattach(pattach)
{
	qDebug("**IPCheckThr Started**");
	start();
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
IPCheckThr::~IPCheckThr()
{
	qDebug("QThread Terminate");
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  QThread run
 * ---------------------------------------------------------------------------*/
void IPCheckThr::run()
{
	std::set<string> setrequestAddr, setreplyAddr, resultAddr;
	ICMPSocket pingSocket;
	ControlMonitorChannel::IPCheck *ipcheck;
	//qDebug("ControlLib:%p", m_pattach);
	m_pattach->hash_ipcheckiter = m_pattach->hash_ipcheck.constBegin();

	//qDebug("ControlLib-2");
	while ( m_pattach->hash_ipcheckiter != m_pattach->hash_ipcheck.constEnd() ) 
	{
		ipcheck = (ControlMonitorChannel::IPCheck*)&(m_pattach->hash_ipcheckiter.value());
		qDebug("IPAddr:%s",ipcheck->ipAddr.toStdString().c_str());
		setrequestAddr.insert(ipcheck->ipAddr.toStdString());
		m_pattach->hash_ipcheckiter++;
	};

	//qDebug("ControlLib-4");
	while (true)
	{
		try {
			//multiple host
			pingSocket.ping( &setrequestAddr, &setreplyAddr, resultAddr);
		} catch (Exception &sockErr) {
			printf("Exception %s\n", sockErr.getReason());
			pingSocket.close();
		};

		set<string>::iterator setRIter;
		size_t i = 0;
		for(setRIter = setreplyAddr.begin(); setRIter != setreplyAddr.end(); ++setRIter, i++)
		{
			QString strReSvr = setRIter->c_str();
			//qDebug("ReceivedIP:%s", strReSvr.toStdString().c_str());
			m_pattach->hash_ipcheckiter = m_pattach->hash_ipcheck.find(strReSvr);
			if ( m_pattach->hash_ipcheckiter != m_pattach->hash_ipcheck.constEnd() ) 
			{
				ipcheck = (ControlMonitorChannel::IPCheck*)&(m_pattach->hash_ipcheckiter.value());
				QString objName = ipcheck->pobj->metaObject()->className();
				BlinkLine *pLine = static_cast<BlinkLine*> (ipcheck->pobj);
#if 0
				pLine->setCount(0);
#endif
				pLine->setAlarm(BlinkLine::NormalState); 
			};
		};
		set<string>::iterator setIterFail;
		for( setIterFail= resultAddr.begin();setIterFail != resultAddr.end();++setIterFail)
		{
			QString strFailSvr = setIterFail->c_str();
			//qDebug("FailIP:%s", strFailSvr.toStdString().c_str());
			m_pattach->hash_ipcheckiter = m_pattach->hash_ipcheck.find(strFailSvr);
			if ( m_pattach->hash_ipcheckiter != m_pattach->hash_ipcheck.constEnd() ) 
			{
				ipcheck = (ControlMonitorChannel::IPCheck*)&(m_pattach->hash_ipcheckiter.value());
				QString objName = ipcheck->pobj->metaObject()->className();
				BlinkLine *pLine = static_cast<BlinkLine*> (ipcheck->pobj);
#if 0
				if(pLine->getCount() > 2)
#endif
				pLine->setAlarm(BlinkLine::AlarmState); 
#if 0
				pLine->incCount();
#endif
			};
		};
		setreplyAddr.clear();
		resultAddr.clear();
		sleep(5);
	};
}
