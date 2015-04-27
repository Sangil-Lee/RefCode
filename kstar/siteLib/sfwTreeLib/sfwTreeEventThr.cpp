/****************************************************************************
 * sfwTreeEventThrArchiveThr.cpp
 * --------------------------------------------------------------------------
 * MDSplus C++ interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.09.14  yunsw        Initial revision
 ****************************************************************************/

#include "sfwTreeEventThr.h"

sfwTreeEventThr::sfwTreeEventThr (const char *pszThrName, sfwTree *pSfwTree) : kuThread(pszThrName)
{
	kuDebug (kuMON, "[sfwTreeEventThr::sfwTreeEventThr] constructor ... \n");

	init ();

	m_pSfwTree	= pSfwTree;

	createQueue ();
}

sfwTreeEventThr::~sfwTreeEventThr ()
{
	kuDebug (kuMON, "[sfwTreeEventThr::~sfwTreeEventThr] destructor ... \n");
}

const epicsBoolean sfwTreeEventThr::init ()
{
	m_queueId		= NULL;
	m_pSfwTree		= NULL;

	return (epicsTrue);
}

const epicsMessageQueueId sfwTreeEventThr::createQueue ()
{
	m_queueId = epicsMessageQueueCreate (getQueueCapacity(), getQueueMaxMsgSize());

	return (m_queueId);
}

void *sfwTreeEventThr::run ()
{
	kuDebug (kuMON, "[sfwTreeEventThr::run] %s : started ...\n", getName());

	sfwTreeMsgHeader	stHeader;

	while (epicsTrue) {
		kuDebug (kuDEBUG, "[sfwTreeEventThr::run] %s : waiting message ...\n", getName());

		// receive message from DAQ task
		epicsMessageQueueReceive (m_queueId, &stHeader, sizeof(sfwTreeMsgHeader));

		kuDebug (kuDEBUG, "[sfwTreeEventThr::run] %s : received message ...\n", getName());

		switch (stHeader.uiOperCode) {
			case sfwTreeOperBegin :
				kuDebug (kuMON, "[sfwTreeEventThr::run] %s : begin : shot(%d)\n", getName(), stHeader.iTreeShotNo);

				processBeginArchiving (&stHeader);
				break;

			case sfwTreeOperEnd :
				kuDebug (kuMON, "[sfwTreeEventThr::run] %s : end : shot(%d)\n", getName(), stHeader.iTreeShotNo);

				processEndArchiving (&stHeader);
				break;

			case sfwTreeOperEvent :
				kuDebug (kuINFO, "[sfwTreeEventThr::run] %s : event : shot(%d)\n", getName(), stHeader.iTreeShotNo);

				processUpdateArchiving (&stHeader);
				break;

			default :
				break;
		}
	}

	return (NULL);
}

const sfwTreeErr_e sfwTreeEventThr::processBeginArchiving (sfwTreeMsgHeader *pstHeader)
{
	sfwTree	*pSfwTree	 = getTree ();

	// connect to MDSplus Event
	if (sfwTreeOk != pSfwTree->connectEvent ()) {
		kuDebug (kuERR, "[sfwTreeEventThr::begin] %s : connect failed\n", getName());
		return (sfwTreeErr);
	}

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeEventThr::processEndArchiving (sfwTreeMsgHeader *pstHeader)
{
	sfwTree	*pSfwTree	 = getTree ();

	// disconnect from MDSplus Event
	if (sfwTreeOk != pSfwTree->disConnectEvent ()) {
		kuDebug (kuERR, "[sfwTreeEventThr::end] %s : disconnect failed\n", getName());
		return (sfwTreeErr);
	}

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeEventThr::processUpdateArchiving (sfwTreeMsgHeader *pstHeader)
{
	sfwTree	*pSfwTree	 = getTree ();

	pSfwTree->sendEvent ();

	return (sfwTreeOk);
}

