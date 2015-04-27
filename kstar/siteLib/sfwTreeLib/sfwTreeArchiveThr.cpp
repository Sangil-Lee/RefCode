/****************************************************************************
 * sfwTreeArchiveThrArchiveThr.cpp
 * --------------------------------------------------------------------------
 * MDSplus C++ interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.08.17  yunsw        Initial revision
 ****************************************************************************/

#include "sfwTreeArchiveThr.h"

sfwTreeArchiveThr::sfwTreeArchiveThr (const char *pszThrName, sfwTree *pSfwTree) : kuThread(pszThrName)
{
	kuDebug (kuMON, "[sfwTreeArchiveThr::sfwTreeArchiveThr] constructor ... \n");

	init ();

	m_pSfwTree	= pSfwTree;

	createQueue ();
}

sfwTreeArchiveThr::~sfwTreeArchiveThr ()
{
	kuDebug (kuMON, "[sfwTreeArchiveThr::~sfwTreeArchiveThr] destructor ... \n");
}

const epicsBoolean sfwTreeArchiveThr::init ()
{
	m_queueId		= NULL;
	m_pSfwTree		= NULL;

	return (epicsTrue);
}

const epicsMessageQueueId sfwTreeArchiveThr::createQueue ()
{
	m_queueId = epicsMessageQueueCreate (getQueueCapacity(), getQueueMaxMsgSize());

	return (m_queueId);
}

sfwTreeNode *sfwTreeArchiveThr::getNode (const char *pszNodeName)
{
	return (getTree()->getNode (pszNodeName));
}

void *sfwTreeArchiveThr::run ()
{
	kuDebug (kuMON, "[sfwTreeArchiveThr::run] %s : started ...\n", getName());

	sfwTreeMsgHeader	stHeader;

	while (epicsTrue) {
		kuDebug (kuDEBUG, "[sfwTreeArchiveThr::run] %s : waiting message ...\n", getName());

		// receive message from DAQ task
		epicsMessageQueueReceive (m_queueId, &stHeader, sizeof(sfwTreeMsgHeader));

		kuDebug (kuDEBUG, "[sfwTreeArchiveThr::run] %s : received message ...\n", getName());

		switch (stHeader.uiOperCode) {
			case sfwTreeOperBegin :
				kuDebug (kuMON, "[sfwTreeArchiveThr::run] %s : begin : shot(%d)\n", getName(), stHeader.iTreeShotNo);

				processBeginArchiving (&stHeader);
				break;

			case sfwTreeOperPut :
				kuDebug (kuTRACE, "[sfwTreeArchiveThr::run] %s : put(%s)\n", getName(), stHeader.szNodeName);

				processReceivedData (&stHeader);
				break;

			case sfwTreeOperEnd :
				kuDebug (kuMON, "[sfwTreeArchiveThr::run] %s : end : shot(%d)\n", getName(), stHeader.iTreeShotNo);

				processEndArchiving (&stHeader);
				break;

			case sfwTreeOperEvent :
				kuDebug (kuINFO, "[sfwTreeArchiveThr::run] %s : event : shot(%d)\n", getName(), stHeader.iTreeShotNo);

				processUpdateArchiving (&stHeader);
				break;

			default :
				break;
		}
	}

	return (NULL);
}

const sfwTreeErr_e sfwTreeArchiveThr::processBeginArchiving (sfwTreeMsgHeader *pstHeader)
{
	sfwTree	*pSfwTree	 = getTree ();

#if 0
	processEndArchiving (pstHeader);
#endif

	// open local raw files
	openLocalDataFiles (pstHeader->iTreeShotNo);

	// connect to MDSplus
	if (sfwTreeOk != pSfwTree->connect ()) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::begin] %s : connect failed\n", getName());
		return (sfwTreeErr);
	}

	// open tree
	if (sfwTreeOk != pSfwTree->open ()) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::begin] %s : open failed\n", getName());
		return (sfwTreeErr);
	}

#if SFW_TREE_USE_EVENT && !SFW_TREE_USE_EVENT_THR
	// connect to MDSplus
	if (sfwTreeOk != pSfwTree->connectEvent ()) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::begin] %s : connectEvent failed\n", getName());
		return (sfwTreeErr);
	}
#endif

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeArchiveThr::processEndArchiving (sfwTreeMsgHeader *pstHeader)
{
	sfwTree	*pSfwTree	 = getTree ();

	// close local raw files
	closeLocalDataFiles (pstHeader->iTreeShotNo);

	// close tree
	if (sfwTreeOk != pSfwTree->close ()) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::end] %s : close failed\n", getName());
		return (sfwTreeErr);
	}

	// disconnect to MDSplus
	if (sfwTreeOk != pSfwTree->disConnect ()) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::end] %s : disconnect failed\n", getName());
		return (sfwTreeErr);
	}

#if SFW_TREE_USE_EVENT && !SFW_TREE_USE_EVENT_THR
	// disconnect to MDSplus
	if (sfwTreeOk != pSfwTree->disConnectEvent ()) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::end] %s : disconnect failed\n", getName());
		return (sfwTreeErr);
	}
#endif

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeArchiveThr::processUpdateArchiving (sfwTreeMsgHeader *pstHeader)
{
	sfwTree	*pSfwTree	 = getTree ();

#if SFW_TREE_USE_EVENT && !SFW_TREE_USE_EVENT_THR
	pSfwTree->sendEvent ();
#endif

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeArchiveThr::processReceivedData (sfwTreeMsgHeader *pstHeader)
{
	sfwTree		*pSfwTree	= getTree();
	sfwTreeNode	*pTreeNode	= getNode(pstHeader->szNodeName);

	// get data from internal queue
	sfwTreeRecord *pRecord	= pTreeNode->pop ();

	// write data to file
	FILE		*fp			= pTreeNode->getFilePointer();
	char		*pData		= (char *)pRecord->getDataBuffer();
	epicsUInt32	uiMsgLen	= pRecord->getSize();

	if (NULL == pData) {
		kuDebug (kuERR, "[sfwTreeArchiveThr::processReceivedData] %s : data is invalid\n", pTreeNode->getName());
		goto clean_process;
	}

	if (NULL != fp) {
		kuDebug (kuINFO, "[sfwTreeArchiveThr::processReceivedData] %s write : size(%d) \n", pTreeNode->getName(), uiMsgLen);

		if (0 > fwrite (pData, 1, uiMsgLen, fp)) {
			kuDebug (kuERR, "[sfwTreeArchiveThr::processReceivedData] %s : write failed\n", pTreeNode->getName());
			goto clean_process;
		}
	}

	kuDebug (kuINFO, "[sfwTreeArchiveThr::processReceivedData] %s send : size(%d) rate(%d/%d) stime(%f)\n",
			pTreeNode->getName(), uiMsgLen, pSfwTree->getSamplingRate(), pSfwTree->getSamplingRate(pTreeNode), pRecord->getStartTime());

	// send data to MDSplus server
	if (sfwTreePutSegment == pSfwTree->getDataPutType()) {
		if (sfwTreeNodeWaveform == pTreeNode->getType()) {
			epicsUInt32	uiSamples	= pRecord->getSize() / pTreeNode->getDataTypeSize();
			pSfwTree->writeSegment (pTreeNode, pRecord->getIndex(), pRecord->getStartTime(), uiSamples, pData);
		}
		else if (sfwTreeNodeValue == pTreeNode->getType()) {
			if (sfwTreeNodeFloat64 == pTreeNode->getDataType()) {
				epicsFloat64 *pdValue = (epicsFloat64 *)pData;
				pSfwTree->write (pTreeNode->getName(), *pdValue);
			}
		}
	}
	else {
		//TODO
		//pSfwTree->write (pTreeNode->getName(), pTreeNode->getType(), pData, pSfwTree->getSamplingRate(pTreeNode), uiSamples, pRecord->getStartTime());
	}

	kuDebug (kuINFO, "[sfwTreeArchiveThr::processReceivedData] %s : size(%d) OK!!! \n", pTreeNode->getName(), uiMsgLen);

	delete (pRecord);

	return (sfwTreeOk);

clean_process :

	delete (pRecord);

	return (sfwTreeErr);
}

const char *sfwTreeArchiveThr::getLocalHomeDir (const char *pszFilePath, const int iShotNo, char *pszHomeDir)
{
	if ( (NULL != pszFilePath) && (NULL != pszHomeDir) ) {
		sprintf (pszHomeDir, "%s/S%09d", pszFilePath, iShotNo);
	}

	return (pszHomeDir);
}

const sfwTreeErr_e sfwTreeArchiveThr::openLocalDataFiles (const epicsInt32 iTreeShotNo)
{
	sfwTree	*pSfwTree	= getTree();

#if 0	//TODO
	if (pSfwTree->getShotNo() != iTreeShotNo) {
		return (sfwTreeErr);
	}
#endif

	sfwTreeNodeMap				*pTreeNodeMap	= getTree()->getNodeMap ();
	sfwTreeNodeMap::iterator    pos;

	if( strlen( pSfwTree->getLocalFileHome() ) <= 0 ) /* 2013. 8. 12 by woong */
	{
		kuDebug (kuDEBUG, "[sfwTreeArchiveThr::openLocalDataFiles] Don't use LocalFileHome\n");
		return (sfwTreeOk);
	}

	// create directory for current shot
	char	szHomeDir[128];
	getLocalHomeDir (pSfwTree->getLocalFileHome(), pSfwTree->getShotNo(), szHomeDir);

	if (kuTRUE != kuFile::isExist (szHomeDir)) {
		if (kuOK != kuFile::makeDir (szHomeDir)) {
			kuDebug (kuERR, "[sfwTreeArchiveThr::openLocalDataFiles] %s : create failed\n", szHomeDir);
			return (sfwTreeErr);
		}
	}

	sfwTreeNode	*pSwTreeNode	= NULL;

	for (pos = pTreeNodeMap->begin(); pos != pTreeNodeMap->end(); ++pos) {
		pSwTreeNode	= pos->second;
		pSwTreeNode->openFile (szHomeDir);

		kuDebug (kuTRACE, "[sfwTreeArchiveThr::openLocalDataFiles] %s \n", pos->first.c_str());
	}

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeArchiveThr::closeLocalDataFiles (const epicsInt32 iTreeShotNo)
{
#if 0
	sfwTree	*pSfwTree	= getTree();

	if (pSfwTree->getShotNo() != iTreeShotNo) {
		return (sfwTreeErr);
	}
#endif

	sfwTreeNodeMap				*pTreeNodeMap	= getTree()->getNodeMap ();
	sfwTreeNodeMap::iterator    pos;

	sfwTreeNode	*pSwTreeNode	= NULL;

	for (pos = pTreeNodeMap->begin(); pos != pTreeNodeMap->end(); ++pos) {
		pSwTreeNode	= pos->second;
		pSwTreeNode->closeFile ();

		kuDebug (kuTRACE, "[sfwTreeArchiveThr::closeLocalDataFiles] %s \n", pos->first.c_str());
	}

	return (sfwTreeOk);
}

