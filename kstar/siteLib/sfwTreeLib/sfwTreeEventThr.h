/****************************************************************************
 * sfwTreeEventThr.h
 * --------------------------------------------------------------------------
 * MDSplus C++ interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.09.14  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    sfwTreeEventThr.h
 * \ingroup sfwtree
 * \brief   노드 데이터 변경 이벤트를 처리하는 클래스인 sfwTreeEventThr 정의
 * 
 * 본 파일은 사용자 애플리케이션에서 MDSplus에 데이터를 저장한 뒤, 
 * 노드 데이터가 변경 되었음을 알리기 위한 이벤트 전송 요청을 처리하는 클래스인 sfwTreeEventThr 을 정의한다.
 */
#ifndef __SFW_TREE_EVENT_THR_H
#define __SFW_TREE_EVENT_THR_H

#include "kuThread.h"
#include "sfwTreeObj.h"

class sfwTree;
class sfwTreeNode;

/**
 * \class   sfwTreeEventThr
 * \brief   노드 데이터 변경 이벤트를 처리하는 클래스
 * 
 * 본 클래스는 사용자 애플리케이션으로부터 받은 이벤트 전송 요청을 처리하는 쓰레드를 위한 클래스로서,
 * 사용자 애플리케이션의 요청을 수행한다. \n
 * 사용자 애플리케이션은 C 함수 또는, C++ 클래스를 이용하여 데이터를 저장한 이후에
 * 데이터의 변경을 알리기 위한 이벤트 전송을 요청한다. \n
 * sfwTreeEventThr 는 이러한 이벤트 전송 요청 메시지를 수신하고, 그 요청에 대한 서비스를 수행한다. \n
 * \n
 * 본 쓰레드는, 컴파일 옵션에 따라 생성 되지 않을 수 있다.
 * 만일, 본 쓰레드가 생성되지 않으면 sfwTreeArchiveThr에서 MDS 이벤트 서버로의 이벤트 전송 작업을 수행한다. \n
 * 현재, sfwTreeEventThr를 사용하지 않고 있다. \n
 * \n
 * sfwTreeArchiveThr는, 쓰레드를 생성하기 위하여 kutil 라이브러리 내의 kuThread 클래스를 상속하였다.
 */
 
class sfwTreeEventThr : public kuThread
{
public :
	sfwTreeEventThr (const char *pszThrName, sfwTree *pSfwTree);
	~sfwTreeEventThr ();

	void *run ();

	const epicsMessageQueueId getQueueId () {
		if (NULL == m_queueId) {
			m_queueId	= createQueue ();
		}

		return (m_queueId);
	}

	const epicsUInt32 getQueueCapacity () {
		return (SFW_TREE_MQ_CAPACITY);
	}

	const epicsUInt32 getQueueMaxMsgSize () {
		return (SFW_TREE_MQ_MAX_SIZE);
	}

	sfwTree *getTree () { return (m_pSfwTree); }

private :
	sfwTree				*m_pSfwTree;	// Tree object
	epicsMessageQueueId	m_queueId;		// queue id for event notify thread

	const epicsBoolean init ();

	const epicsMessageQueueId createQueue ();
	void setQueueId (epicsMessageQueueId queueId) { m_queueId = queueId; }

	const sfwTreeErr_e processBeginArchiving (sfwTreeMsgHeader *pstHeader);
	const sfwTreeErr_e processEndArchiving (sfwTreeMsgHeader *pstHeader);
	const sfwTreeErr_e processUpdateArchiving (sfwTreeMsgHeader *pstHeader);
};

#endif // __SFW_TREE_EVENT_THR_H
