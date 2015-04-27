/****************************************************************************
 * sfwTreeObj.h
 * --------------------------------------------------------------------------
 * MDSplus C++ interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.23  yunsw        Initial revision
 ****************************************************************************/

#include "sfwTreeObj.h"

static epicsInt32	sfwTreeEnable	= epicsTrue;
epicsExportAddress (int, sfwTreeEnable);

// --------------------------------------------------------------------------
// sfwTreeType
// --------------------------------------------------------------------------

sfwTreeTypeInfo	sfwTreeType::m_pstSfwTreeType[]	= {
	{ sfwTreeNodeInt16	, DTYPE_SHORT	, sizeof(epicsInt16)	},
	{ sfwTreeNodeInt32	, DTYPE_LONG	, sizeof(epicsInt32)	},
	{ sfwTreeNodeFloat32, DTYPE_FLOAT	, sizeof(epicsFloat32)	},
	{ sfwTreeNodeFloat64, DTYPE_DOUBLE	, sizeof(epicsFloat64)	}
};

epicsInt32 sfwTreeType::getNumOfTypes ()
{
	return ( sizeof(m_pstSfwTreeType) / sizeof(m_pstSfwTreeType[0]) );
}

epicsInt32 sfwTreeType::getMdsType (const sfwTreeDataType_e eDataType)
{
	epicsInt32	iNum	= getNumOfTypes ();;

	for (epicsInt32 i = 0; i < iNum; i++) {
		if (eDataType == m_pstSfwTreeType[i].eSfwTreeDataType) {
			return (m_pstSfwTreeType[i].iMdsDescType);
		}
	}

	return (0);
}

epicsInt32 sfwTreeType::getTypeSize (const sfwTreeDataType_e eDataType)
{
	epicsInt32	iNum	= getNumOfTypes ();;

	for (epicsInt32 i = 0; i < iNum; i++) {
		if (eDataType == m_pstSfwTreeType[i].eSfwTreeDataType) {
			return (m_pstSfwTreeType[i].iDataTypeSize);
		}
	}

	return (0);
}

// --------------------------------------------------------------------------
// sfwTreeNode
// --------------------------------------------------------------------------

sfwTreeNode::sfwTreeNode (sfwTree *pSfwTree, const char *pszNodeName, const char *pszFileName,
		sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, epicsUInt32 uiSamplingRate)
{
	setNodeInfo (pSfwTree, pszNodeName, pszFileName, eNodeType, eNodeDataType, uiSamplingRate);

	m_fp	= NULL;
}

sfwTreeNode::sfwTreeNode (const sfwTreeNode &node)
{
	setNodeInfo (node.m_pSfwTree, node.m_strNodeName.c_str(), node.m_strFileName.c_str(), node.m_eNodeType, node.m_eDataType, node.m_uiSamplingRate);

	m_fp	= NULL;
}

sfwTreeNode::~sfwTreeNode ()
{
	clear ();
}

void sfwTreeNode::clear ()
{
	closeFile ();
}

void sfwTreeNode::setNodeInfo (sfwTree *pSfwTree, const char *pszNodeName, const char *pszFileName,
		sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, epicsUInt32 uiSamplingRate)
{
	setTree (pSfwTree);
	setName (pszNodeName);
	setFileName (pszFileName);
	setType (eNodeType);
	setDataType (eNodeDataType);
	setSamplingRate (uiSamplingRate);
}

const sfwTreeErr_e sfwTreeNode::setSamplingRate (const epicsUInt32 uiSamplingRate)
{
	m_uiSamplingRate	= uiSamplingRate;

	return (sfwTreeOk);
}

sfwTreeErr_e sfwTreeNode::push (sfwTreeRecord *pRecord)
{
	kuDebug (kuTRACE, "[sfwTreeNode::push] size of queue is %d\n", m_qRecQueue.size());

	lock ();

	m_qRecQueue.push (pRecord);

	unlock ();

	return (sfwTreeOk);
}

sfwTreeRecord *sfwTreeNode::pop ()
{
	lock ();

	sfwTreeRecord *pRecord = m_qRecQueue.front();
	m_qRecQueue.pop ();

	unlock ();

	kuDebug (kuTRACE, "[sfwTreeNode::pop] size of queue is %d\n", m_qRecQueue.size());

	return (pRecord);
}

const sfwTreeErr_e sfwTreeNode::putData (const epicsFloat64 dValue)
{
	return ( putData (0, 0, 1, &dValue) );
}

const sfwTreeErr_e sfwTreeNode::putData (const epicsInt32 iIndex,
		const epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf)
{
	if (NULL == m_pSfwTree) {
		kuDebug (kuERR, "[sfwTreeNode::putData] tree is not initialized ...\n");
		return (sfwTreeErrInvalid);
	}

	if ( (0 == uiSamples) || (NULL == pDataBuf) ) {
		return (sfwTreeErrInvalid);
	}

	const epicsUInt32	uiDataSize	= uiSamples * getDataTypeSize();

	// put data to internal queue
	sfwTreeRecord *pRecord = new sfwTreeRecord (getShotNo(), iIndex, dStartTime, uiDataSize, pDataBuf);

	if (NULL != pRecord) {
		if (sfwTreeOk == push (pRecord)) {
			// send event to thread
			return ( m_pSfwTree->putData (getName(), iIndex, dStartTime, uiDataSize, pDataBuf) );
		}
	}

	return (sfwTreeErr);
}
const sfwTreeErr_e sfwTreeNode::waitForSync (epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
{
	return (getTree()->waitForSync (this, dTimeout, dCheckPeriod));
}

char *sfwTreeNode::makeFilePath (const char *pszHomeDir, char *pszFilePath)
{
	if (NULL != pszFilePath) {
		sprintf (pszFilePath, "%s/%s", pszHomeDir, getFileName());
	}

	return (pszFilePath);
}

const sfwTreeErr_e sfwTreeNode::openFile (const char *pszHomeDir)
{
	closeFile ();

	if ("" == getFileNameStr()) {
		return (sfwTreeOk);
	}

	char	szFilePath[128];

	m_strFilePath	= makeFilePath (pszHomeDir, szFilePath);

	if (NULL == (m_fp = fopen (m_strFilePath.c_str(), "w+"))) {
		kuDebug (kuERR, "[sfwTreeNode::openFile] %s : create failed\n", szFilePath);
		return (sfwTreeErr);
	}

	kuDebug (kuDEBUG, "[sfwTreeNode::openFile] %s : created\n", szFilePath);

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTreeNode::closeFile ()
{
	if (NULL != m_fp) {
		fclose (m_fp);
		m_fp	= NULL;

		kuDebug (kuDEBUG, "[sfwTreeNode::closeFile] %s : closed %s\n", getFileName(), getFilePath());
	}

	return (sfwTreeOk);
}

const epicsInt32 sfwTreeNode::getShotNo ()
{
	return (m_pSfwTree->getShotNo());
}

// --------------------------------------------------------------------------
// sfwTree
// --------------------------------------------------------------------------

const epicsBoolean sfwTree::isEnable ()
{
	return ( sfwTreeEnable ? epicsTrue : epicsFalse );
}

sfwTree::sfwTree (const char *pszHandleName)
{
	kuDebug (kuTRACE, "[sfwTree::sfwTree] constructor ... \n");

	init ();

	createArchiveThr (pszHandleName);

#if SFW_TREE_USE_EVENT && SFW_TREE_USE_EVENT_THR
	createEventThr (pszHandleName);
#endif
}

sfwTree::~sfwTree ()
{
	kuDebug (kuTRACE, "[sfwTree::~sfwTree] destructor ... \n");
}

const sfwTreeErr_e sfwTree::init ()
{
	kuDebug (kuTRACE, "[sfwTree::init] hi ... \n");

	m_iSocket			= INVALID_SOCKET;
	m_bOpened			= epicsFalse;
	m_iStatus			= 1;
	m_iShotNo			= 0;
	m_pArchiveThr		= NULL;

	m_iEventSocket		= INVALID_SOCKET;
	m_pEventThr			= NULL;

	return (sfwTreeOk);
}

const epicsMessageQueueId sfwTree::getQueueId ()
{
	return ( m_pArchiveThr ? m_pArchiveThr->getQueueId() : NULL );
}

const epicsMessageQueueId sfwTree::getEventQueueId ()
{
	return ( m_pEventThr ? m_pEventThr->getQueueId() : NULL );
}

const sfwTreeErr_e sfwTree::createArchiveThr (const char *pszThrName)
{
	kuDebug (kuTRACE, "[sfwTree::createArchiveThr] hi ... \n");

	char	szThrName[64] = "sfwTree";

	if (NULL != pszThrName) {
		sprintf (szThrName, "%s_Tree", pszThrName);
	}

	lock ();

	if (NULL == m_pArchiveThr) {
		if (NULL != (m_pArchiveThr = new sfwTreeArchiveThr (szThrName, this))) {
			if (NULL != m_pArchiveThr->start ()) {
				unlock ();
				return (sfwTreeOk);
			}
		}
	}

	unlock ();

	return (sfwTreeErr);
}

const sfwTreeErr_e sfwTree::createEventThr (const char *pszThrName)
{
	kuDebug (kuTRACE, "[sfwTree::createEventThr] hi ... \n");

	char	szThrName[64] = "SfwTreeEvent";

	if (NULL != pszThrName) {
		sprintf (szThrName, "%s_TreeEvent", pszThrName);
	}

	lock ();

	if (NULL == m_pEventThr) {
		if (NULL != (m_pEventThr = new sfwTreeEventThr (szThrName, this))) {
			if (NULL != m_pEventThr->start ()) {
				unlock ();
				return (sfwTreeOk);
			}
		}
	}

	unlock ();

	return (sfwTreeErr);
}

sfwTreeNode *sfwTree::addNode (const char *pszNodeName, sfwTreeNodeType_e eNodeType,
		sfwTreeDataType_e eNodeDataType, const char *pszFileName, epicsUInt32 uiSamplingRate)
{
	if (NULL == pszNodeName) {
		return (NULL);
	}

	lock ();

	sfwTreeNodeMap::iterator pos	= m_mapSfwTreeNode.find (pszNodeName);

	if (m_mapSfwTreeNode.end() != pos) {
		pos->second->setNodeInfo (this, pszNodeName, pszFileName, eNodeType, eNodeDataType, uiSamplingRate);

		unlock ();
		return (pos->second);
	}

	sfwTreeNode	*pTreeNode	= new sfwTreeNode (this, pszNodeName, pszFileName, eNodeType, eNodeDataType, uiSamplingRate);

	if (NULL != pTreeNode) {
		m_mapSfwTreeNode[pszNodeName] = pTreeNode;
	}

	unlock ();

	return (pTreeNode);
}

void sfwTree::printNodes ()
{
	sfwTreeNodeMap::iterator	pos;

	kuDebug (kuMON, "[sfwTree::printNodes] size(%d)\n", m_mapSfwTreeNode.size());

	for (pos = m_mapSfwTreeNode.begin(); pos != m_mapSfwTreeNode.end(); ++pos) {
		kuDebug (kuMON, "[sfwTree::printNodes] %s \n", pos->first.c_str());
	}
}

sfwTreeNode *sfwTree::getNode (const char *pszNodeName)
{
	sfwTreeNodeMap::iterator	pos = m_mapSfwTreeNode.find (pszNodeName);

	if (m_mapSfwTreeNode.end() != pos) {
		kuDebug (kuDEBUG, "[sfwTree::getNode] %s : found \n", pos->first.c_str());
		return (pos->second);
	}
	else {
		kuDebug (kuDEBUG, "[sfwTree::getNode] %s : not found \n", pszNodeName);
		return (NULL);
	}

}

const sfwTreeErr_e sfwTree::setShotInfo (const epicsInt32 iShotNo, const char *pszTreeName,
		const char *pszMdsAddr, const char *pszEventName, const char *pszEventAddr)
{
	if (NULL == pszTreeName) {
		kuDebug (kuMON, "[sfwTree::setShotInfo] tree name is invalid\n");
		return (sfwTreeErrInvalid);
	}

	m_strTreeName		= pszTreeName;
	m_strMdsAddr		= pszMdsAddr;
	m_strEventName		= pszEventName;
	m_strEventAddr		= pszEventAddr;
	m_iShotNo			= iShotNo;

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::setShotNum (const epicsInt32 iShotNo)
{
	m_iShotNo	= iShotNo;

	return (sfwTreeOk);
}

void sfwTree::printInfo ()
{
	kuDebug (kuMON, "[sfwTree::printInfo] tree(%s) addr(%s) event(%s:%s)\n",
			m_strTreeName.c_str(), m_strMdsAddr.c_str(), m_strEventAddr.c_str(), m_strEventName.c_str());
}

const sfwTreeErr_e sfwTree::setLocalFileHome (const char *pszPathName)
{
	if (NULL == pszPathName) { /* 2013. 8. 12 by woong */
/*		m_strLocalFileHome	= pszPathName; */
		kuDebug (kuDEBUG, "[sfwTree::setsetLocalFileHome] Don't use LocalFileHome\n");
		return (sfwTreeOk);
	}

	if (kuTRUE != kuFile::isExist (pszPathName)) {
		kuDebug (kuERR, "[sfwTree::setsetLocalFileHome] %s not exist\n", pszPathName);
		return (sfwTreeErrInvalid);
	}

	m_strLocalFileHome	= pszPathName;

	kuDebug (kuDEBUG, "[sfwTree::setsetLocalFileHome] LocalFileHome : %s\n", m_strLocalFileHome.c_str());

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::setDataPutType (const sfwTreePutType_e eDataPutType)
{
	if ( (sfwTreePutBulk != eDataPutType) && (sfwTreePutSegment != eDataPutType) ) {
		kuDebug (kuERR, "[sfwTree::setDataPutType] DataPutType(%d) is invalid\n", eDataPutType);
		return (sfwTreeErrInvalid);
	}

	m_eDataPutType	= eDataPutType;

	return (sfwTreeOk);
}

const epicsUInt32 sfwTree::getSamplingRate (sfwTreeNode *pSfwTreeNode)
{
	if ( (NULL != pSfwTreeNode) && (0 < pSfwTreeNode->getSamplingRate()) ) {
		return (pSfwTreeNode->getSamplingRate());
	}
	else {
		return (m_uiSamplingRate);
	}
}

const sfwTreeErr_e sfwTree::setSamplingRate (const epicsUInt32 uiSamplingRate)
{
	m_uiSamplingRate	= uiSamplingRate;

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::sendMessage (const sfwTreeOperCode_e operCode, const char *pszTreeNodeName,
		const epicsInt32 iIndex, const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf)
{
	sfwTreeMsgHeader	stHeader;

	// copy data to message buffer
	stHeader.uiOperCode		= operCode;
	stHeader.uiMsgLen		= uiDataSize;
	stHeader.iTreeShotNo	= getShotNo();
	stHeader.iIndex			= iIndex;
	stHeader.dStartTime		= dStartTime;

	if (pszTreeNodeName) {
		strcpy (stHeader.szNodeName, pszTreeNodeName);
	}

	// send data to message queue for MDSplus
	epicsMessageQueueSend (getQueueId(), &stHeader, sizeof(sfwTreeMsgHeader));

#if SFW_TREE_USE_EVENT && SFW_TREE_USE_EVENT_THR
	if (epicsTrue == isEventServer ()) {
		if (sfwTreeOperPut != operCode) {
			// send data to message queue for MDSplus event
			epicsMessageQueueSend (getEventQueueId(), &stHeader, sizeof(sfwTreeMsgHeader));
		}
	}
#endif

	kuDebug (kuDEBUG, "[sfwTree::sendMessage] operCode(%d) size(%d) shot(%d) idx(%d) time(%f)\n",
			stHeader.uiOperCode, stHeader.uiMsgLen, stHeader.iTreeShotNo, stHeader.iIndex, stHeader.dStartTime);

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::putData (sfwTreeNode *pSfwTreeNode, const epicsInt32 iIndex, 
		const epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf)
{
	if (NULL == pSfwTreeNode) {
		return (sfwTreeErrInvalid);
	}

	epicsUInt32	uiDataSize	= uiSamples * pSfwTreeNode->getDataTypeSize();

	return (putData (pSfwTreeNode->getName(), iIndex, dStartTime, uiDataSize, pDataBuf));
}

const sfwTreeErr_e sfwTree::putData (const char *pszTreeNodeName, const epicsInt32 iIndex, 
		const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf)
{
	if (NULL == pszTreeNodeName) {
		return (sfwTreeErrInvalid);
	}

	return ( sendMessage (sfwTreeOperPut, pszTreeNodeName, iIndex, dStartTime, uiDataSize, pDataBuf) );
}

const sfwTreeErr_e sfwTree::beginArchive ()
{
	kuDebug (kuTRACE, "[sfwTree::beginArchive] ...\n");

	// send begin message to thread
	return ( sendMessage (sfwTreeOperBegin, NULL, -1, 0, 0, NULL) );
}

const sfwTreeErr_e sfwTree::updateArchive ()
{
	kuDebug (kuTRACE, "[sfwTree::updateArchive] ...\n");

	// send envent message to thread
	return ( sendMessage (sfwTreeOperEvent, NULL, -1, 0, 0, NULL) );
}

const sfwTreeErr_e sfwTree::endArchive (sfwTreeEndAction_e eAction)
{
	kuDebug (kuTRACE, "[sfwTree::endArchive] ...\n");

	// send end message to thread
	return ( sendMessage (sfwTreeOperEnd, NULL, -1, 0, 0, NULL) );
}

const sfwTreeErr_e sfwTree::waitForSync (epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
{
	return ( waitForSync (NULL, dTimeout, dCheckPeriod) );
}

const sfwTreeErr_e sfwTree::waitForSync (sfwTreeNode *pSfwTreeNode, epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
{
	kuDebug (kuTRACE, "[sfwTree::waitForSync] timeout(%f) checkPeriod(%f)\n", dTimeout, dCheckPeriod);

	epicsFloat64	dTimeoutSec		= (dTimeout > 0) ? dTimeout : SFW_TREE_SYNC_TIMEOUT;
	epicsFloat64	dSleepSec		= (dCheckPeriod > 0) ? dCheckPeriod : SFW_TREE_SYNC_PERIOD;
	int				nTryCount		= (int)(dTimeoutSec / dSleepSec);
	int				nRecNumInQueue	= 0;

	sfwTreeNodeMap::iterator	pos;

	for (int i = 0; i <= nTryCount; i++) {
		nRecNumInQueue	= 0;

		if (NULL != pSfwTreeNode) {
			// checks a specific node
			nRecNumInQueue	+= pSfwTreeNode->size();
		}
		else {
			// checks all nodes
			for (pos = m_mapSfwTreeNode.begin(); pos != m_mapSfwTreeNode.end(); ++pos) {
				nRecNumInQueue	+= pos->second->size();
			}
		}

		if (0 == nRecNumInQueue) {
			kuDebug (kuINFO, "[sfwTree::waitForSync] there is no pending data in queue\n");
			return (sfwTreeOk);
		}
		else {
			kuDebug (kuDEBUG, "[sfwTree::waitForSync] %d/%d : there are %d pending records in queue\n", i, nTryCount, nRecNumInQueue);
		}

		if (i < nTryCount) {
			epicsThreadSleep (dSleepSec);
		}
	}

	kuDebug (kuMON, "[sfwTree::waitForSync] timeout : there are %d pending records in queue\n", nRecNumInQueue);

	return (sfwTreeErr);
}

const sfwTreeErr_e sfwTree::connect ()
{
	if (epicsTrue == isConnected ()) {
		return (sfwTreeOk);
	}

	kuDebug (kuTRACE, "[sfwTree::connect] mdsAddr(%s) : connecting ...\n", m_strMdsAddr.c_str());

	//m_iSocket = MdsConnect ((char *)m_strMdsAddr.c_str());
	m_iSocket = ConnectToMds ((char *)m_strMdsAddr.c_str());
	setSocket (m_iSocket);

	if (INVALID_SOCKET == m_iSocket) {
		kuDebug (kuERR, "[sfwTree::connect] MdsConnect() failed for %s\n", m_strMdsAddr.c_str());
		return (sfwTreeErr);
	}

	kuDebug (kuMON, "[sfwTree::connect] mdsAddr(%s) : connected. socket (%d)\n", m_strMdsAddr.c_str(), m_iSocket);

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::disConnect ()
{
	lock ();

	if (epicsTrue == isConnected ()) {
		if (epicsTrue == isOpened ()) {
			close ();
		}

		kuDebug (kuMON, "[sfwTree::disConnect] mdsAddr(%s) socket(%d): disconnecting ...\n", m_strMdsAddr.c_str(), m_iSocket);

		disConnectSocket (m_iSocket);

		m_iSocket	= INVALID_SOCKET;
	}

	unlock ();

	return (sfwTreeOk);	
}

const sfwTreeErr_e sfwTree::connectEvent ()
{
	if (epicsTrue == isEventConnected ()) {
		return (sfwTreeOk);
	}

	if (0 == strcmp (m_strMdsAddr.c_str(), m_strEventAddr.c_str())) {
		// uses same connection
		return (sfwTreeOk);
	}

	kuDebug (kuTRACE, "[sfwTree::connectEvent] mdsAddr(%s) : connecting ...\n", m_strEventAddr.c_str());

	//m_iEventSocket = MdsConnect ((char *)m_strEventAddr.c_str());
	m_iEventSocket = ConnectToMds ((char *)m_strEventAddr.c_str());

	if (INVALID_SOCKET == m_iEventSocket) {
		kuDebug (kuERR, "[sfwTree::connectEvent] MdsConnect() failed for %s\n", m_strEventAddr.c_str());
		return (sfwTreeErr);
	}

	kuDebug (kuMON, "[sfwTree::connectEvent] mdsAddr(%s) : connected. socket (%d)\n", m_strEventAddr.c_str(), m_iEventSocket);

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::disConnectEvent ()
{
	lock ();

	if (epicsTrue == isEventConnected ()) {
		kuDebug (kuMON, "[sfwTree::disConnectEvent] mdsAddr(%s) socket(%d) : disconnecting ...\n", m_strEventAddr.c_str(), m_iEventSocket);

		disConnectSocket (m_iEventSocket);

		m_iEventSocket	= INVALID_SOCKET;
	}

	unlock ();

	return (sfwTreeOk);	
}

const sfwTreeErr_e sfwTree::disConnectSocket (epicsInt32 iSocket)
{
#if 1
	if (INVALID_SOCKET != iSocket) {
#if 0
		epicsInt32	iOldSocket	= MdsSetSocket (&iSocket);
		MdsDisconnect ();

		if ( (INVALID_SOCKET != iOldSocket) && (iSocket != iOldSocket) ) {
			MdsSetSocket (&iOldSocket);
		}
#else
		setSocket (iSocket);
		MdsDisconnect ();
#endif
	}
#else
	MdsDisconnect ();
#endif

	return (sfwTreeOk);	
}

const sfwTreeErr_e sfwTree::open ()
{
	epicsInt32	iShotNo	= getShotNo();

	if (epicsTrue == isOpened ()) {
		return (sfwTreeOk);
	}

	if (epicsTrue != isConnected ()) {
		kuDebug (kuERR, "[sfwTree::open] mdsAddr(%s) : not connected\n", m_strMdsAddr.c_str());
		return (sfwTreeErr);
	}

	useDataSocket ();

	if (epicsTrue != isMdsOK (MdsOpen((char *)m_strTreeName.c_str(), (int *)&iShotNo))) {
		kuDebug (kuERR, "[sfwTree::open] mdsAddr(%s) tree(%s) shot(%d) : open failed\n",
					m_strMdsAddr.c_str(), m_strTreeName.c_str(),iShotNo);
		disConnect ();
		return (sfwTreeErr);
	}

	setOpened (epicsTrue);

	kuDebug (kuMON, "[sfwTree::open] mdsAddr(%s) tree(%s) shot(%d) OK\n", m_strMdsAddr.c_str(), m_strTreeName.c_str(), getShotNo());
		
	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::close ()
{
	if (epicsTrue == isOpened ()) {
		useDataSocket ();

		if (epicsTrue != isMdsOK (MdsClose ((char *)m_strTreeName.c_str(), &m_iShotNo))) {
			kuDebug (kuERR, "[sfwTree::close] mdsAddr(%s) tree(%s) shot(%d) : close failed\n",
						m_strMdsAddr.c_str(), m_strTreeName.c_str(), getShotNo());
			//TODO
			setOpened (epicsFalse);
			return (sfwTreeErr);
		}
	}

	setOpened (epicsFalse);

	kuDebug (kuMON, "[sfwTree::close] mdsAddr(%s) tree(%s) shot(%d) OK\n", m_strMdsAddr.c_str(), m_strTreeName.c_str(), getShotNo());
		
	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::createTree (const epicsInt32 iShotNo)
{
	int		null = 0; /* Used to mark the end of the argument list */
	int		tstat, len;
	char	buf[128];
	int		dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
	int		idesc = descr(&dtype_Long, &tstat, &null);

	kuDebug (kuMON, "[sfwTree::createTree] Create new tree(%d)... \n", iShotNo);

	if (epicsTrue != isConnected ()) {
		kuDebug (kuERR, "[sfwTree::createTree] MDS tree not connected\n");
		return (sfwTreeErr);
	}

	useDataSocket ();

	sprintf(buf, "TCL(\"set tree %s/shot=-1\")", m_strTreeName.c_str());

	if (epicsTrue != isMdsOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuERR, "[sfwTree::createTree] Error TCL command: %s.\n", getStatusMsg());
		disConnect();
		return (sfwTreeErr);
	}

	sprintf(buf, "TCL(\"create pulse %d\")", iShotNo);

	if (epicsTrue != isMdsOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuERR, "[sfwTree::createTree] Error TCL command: create pulse: %s.\n", getStatusMsg());
		disConnect();
		return (sfwTreeErr);
	}

	sprintf(buf, "TCL(\"close\")");
	MdsValue(buf, &idesc, &null, &len);

	kuDebug (kuMON, "[sfwTree::createTree] Create new tree(%d)... OK\n", iShotNo);

	return (sfwTreeOk);	
}

const sfwTreeErr_e sfwTree::writeSegment (sfwTreeNode *pTreeNode,
		const epicsInt32 iIndex, epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf)
{
	int			iDataType		= sfwTreeType::getMdsType (pTreeNode->getDataType());
	epicsUInt32	uiSamplingRate	= getSamplingRate (pTreeNode);
	int			iEndIdx			= uiSamples - 1;
	double		dTimeRate		= 1.0 / (double)uiSamplingRate;
	double		dEndTime		= dStartTime + iEndIdx * dTimeRate;

	int			_null = 0;		// Used to mark the end of the argument list
	int			tstat;

	// descriptor types
	int			dtype_Long		= DTYPE_LONG;
	int			dtype_Double	= DTYPE_DOUBLE;

	int			startTimeDesc	= descr (&dtype_Double	, &dStartTime		, &_null);	// T0 - Blip
	int			endTimeDesc		= descr (&dtype_Double	, &dEndTime			, &_null);	//
	int			endIdxDesc		= descr (&dtype_Long	, &iEndIdx			, &_null);
	int			timeRateDesc	= descr (&dtype_Double	, &dTimeRate		, &_null);
	int			valueDesc		= descr (&iDataType		, (void *)pDataBuf	, (int *)&uiSamples, &_null);
	int			statDesc		= descr (&dtype_Long	, &tstat			, &_null);

	kuDebug (kuTRACE, "[sfwTree::writeSegment] %s : shot(%d) idx(%d) samples(%d) stime(%f) etime(%f) eidx(%d) timegap(%g)\n",
			pTreeNode->getName(), getShotNo(), iIndex, uiSamples, dStartTime, dEndTime, iEndIdx, dTimeRate);

#if 1
	if (sfwTreeOk != writeSegmentNoPut (pTreeNode->getName(), iIndex, startTimeDesc, endTimeDesc, endIdxDesc, timeRateDesc, valueDesc, statDesc)) {
		kuDebug (kuERR, "[sfwTree::writeSegment] %s : shot(%d) idx(%d) samples(%d) stime(%f) etime(%f) eidx(%d) timegap(%g) NOK\n",
				pTreeNode->getName(), getShotNo(), iIndex, uiSamples, dStartTime, dEndTime, iEndIdx, dTimeRate);
		return (sfwTreeErr);
	}
#else
	// for test
	if (sfwTreeOk != writeSegmentWithPut (pTreeNode->getName(), iIndex, startTimeDesc, endTimeDesc, endIdxDesc, timeRateDesc, valueDesc, statDesc)) {
		kuDebug (kuERR, "[sfwTree::writeSegment] %s : shot(%d) idx(%d) samples(%d) stime(%f) etime(%f) eidx(%d) timegap(%g) NOK\n",
				pTreeNode->getName(), getShotNo(), iIndex, uiSamples, dStartTime, dEndTime, iEndIdx, dTimeRate);
		return (sfwTreeErr);
	}
#endif

	kuDebug (kuMON, "[sfwTree::writeSegment] %s : shot(%d) idx(%d) stime(%f) samples(%d) socket(%d) OK\n",
			pTreeNode->getName(), getShotNo(), iIndex, dStartTime, uiSamples, getSocket());

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::writeSegmentNoPut (const char *pszTagName, const epicsInt32 iIndex, 
		int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
{
	if (sfwTreeOk != beginSegment (pszTagName, iIndex, startTimeDesc, endTimeDesc, endIdxDesc, timeRateDesc, valueDesc, statDesc)) {
		return (sfwTreeErr);
	}

#if 0	// PutSegment() is not required. The result is same with non PutSegment
	if (sfwTreeOk != putSegment (pszTagName, iIndex, startTimeDesc, endTimeDesc, endIdxDesc, timeRateDesc, valueDesc, statDesc)) {
		return (sfwTreeErr);
	}
#endif

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::writeSegmentWithPut (const char *pszTagName, const epicsInt32 iIndex, 
		int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
{
	if (0 == iIndex) {
		if (sfwTreeOk != beginSegment (pszTagName, iIndex, startTimeDesc, endTimeDesc, endIdxDesc, timeRateDesc, valueDesc, statDesc)) {
			return (sfwTreeErr);
		}
	} 

	if (sfwTreeOk != putSegment (pszTagName, iIndex, startTimeDesc, endTimeDesc, endIdxDesc, timeRateDesc, valueDesc, statDesc)) {
		return (sfwTreeErr);
	}

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::beginSegment (const char *pszTagName, const epicsInt32 iIndex, 
		int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
{
	int		_null = 0;	/* Used to mark the end of the argument list */
	int		status;		/* Will contain the status of the data access operation */
	int		len;
	char	buf[1024];

	useDataSocket ();

	sprintf (buf, "BeginSegment(%s,$1,$2,MAKE_DIM(*,MAKE_RANGE($1,$2,$3)),$4,%d)", pszTagName, -1);	// index is always -1
	status = MdsValue (buf, &startTimeDesc, &endTimeDesc, &timeRateDesc, &valueDesc, &statDesc, &_null, &len);

#if 0
	// OK : origin expression
	sprintf (buf, "BeginSegment(%s,$,$,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))),$,%d)", pszTagName, -1);	// index is always -1
	status = MdsValue (buf, &startTimeDesc, &endTimeDesc, &endIdxDesc, &startTimeDesc, &timeRateDesc, &valueDesc, &statDesc, &_null, &len);

	// OK
	sprintf (buf, "BeginSegment(%s,$,$,MAKE_DIM(*,MAKE_RANGE($,$,$)),$,%d)", pszTagName, -1);	// index is always -1
	status = MdsValue (buf, &startTimeDesc, &endTimeDesc, &startTimeDesc, &endTimeDesc, &timeRateDesc, &valueDesc, &statDesc, &_null, &len);

	// Error
	sprintf (buf, "BeginSegment(%s,$1,$2,MAKE_DIM(*, $1 : $2 : $3)),$4,%d)", pszTagName, -1);	// index is always -1
	status = MdsValue (buf, &startTimeDesc, &endTimeDesc, &timeRateDesc, &valueDesc, &statDesc, &_null, &len);
#endif

	if (epicsTrue != isMdsOK (status)) {
		kuDebug (kuERR, "[sfwTree::beginSegment] %s : Error(%s)\n", pszTagName, getStatusMsg());
		return (sfwTreeErr);
	} 

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::putSegment (const char *pszTagName, const epicsInt32 iIndex, 
		int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
{
	int		_null = 0;	/* Used to mark the end of the argument list */
	int		status;		/* Will contain the status of the data access operation */
	int		len;
	char	buf[1024];

    sprintf (buf, "PutSegment(%s,%d, \
		BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\")))))", pszTagName, -1 /* iIndex */ );

	useDataSocket ();
	status = MdsValue (buf, &valueDesc, &timeRateDesc, &endIdxDesc, &startTimeDesc, &timeRateDesc, &statDesc, &_null, &len);

	if (epicsTrue != isMdsOK (status)) {
		kuDebug (kuERR, "[sfwTree::putSegment] %s : Error(%s)\n", pszTagName, getStatusMsg());
		return (sfwTreeErr);
	} 

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::write (const char *szTagName, const void *pDatabuf, epicsUInt32 uiSamplingRate, epicsUInt32 uiSamples, epicsFloat64 dStartTime)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	int 	dtype_Long		= DTYPE_LONG;
	int 	dtype_Float		= DTYPE_FLOAT;
	int 	dtype_Double	= DTYPE_DOUBLE;
	int		iEndIdx			= uiSamples - 1;
	double	dTimeRate		= 1.0 / (double)uiSamplingRate;

	int		startTimeDesc	= descr(&dtype_Double, &dStartTime, &_null);	// T0 - Blip
	int		endIdxDesc		= descr(&dtype_Long  , &iEndIdx, &_null);
	int		timeRateDesc	= descr(&dtype_Double, &dTimeRate, &_null);
	int		valueDesc		= descr(&dtype_Float , (void *)pDatabuf, (int *)&uiSamples, &_null);

	kuDebug (kuMON, "[sfwTree::write] tag(%s) rate(%d) samples(%d) startTime(%f) ...\n", szTagName, uiSamplingRate, uiSamples, dStartTime);

	useDataSocket ();
	status = MdsPut ((char *)szTagName,
		"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&valueDesc, &endIdxDesc, &startTimeDesc, &timeRateDesc, &_null);

	if (epicsTrue != isMdsOK (status)) {
		kuDebug (kuERR, "[sfwTree::write] Error tag(%s) : %s\n", szTagName, getStatusMsg());
		return (sfwTreeErr);
	} 

	kuDebug (kuMON, "[sfwTree::write] tag(%s) rate(%d) samples(%d) startTime(%f) OK\n", szTagName, uiSamplingRate, uiSamples, dStartTime);

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::write (const char *szTagName, char *pszValue)
{
	if ( (NULL == szTagName) || (NULL == pszValue) ) {
		return (sfwTreeErr);
	}

	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	kuDebug (kuTRACE, "[sfwTree::write] %s : val(%s) ...\n", szTagName, pszValue);

	useDataSocket ();
	status = MdsPut ((char *)szTagName, pszValue, &_null);

	if (epicsTrue != isMdsOK (status)) {
		kuDebug (kuERR, "[sfwTree::write] %s : Error(%s)\n", szTagName, getStatusMsg());
		return (sfwTreeErr);
	} 

	kuDebug (kuMON, "[sfwTree::write] %s : val(%s) OK\n", szTagName, pszValue);

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::write (const char *szTagName, const epicsFloat64 dValue)
{
	char	szValueStr[64];
	sprintf (szValueStr, "%g", dValue);

	return ( write (szTagName, szValueStr) );
}

const sfwTreeErr_e sfwTree::sendEvent ()
{
	if (m_strEventAddr.empty() || m_strEventName.empty()) {
		return (sfwTreeErr);
	}

	kuDebug (kuTRACE, "[sfwTree::sendEvent] tree(%s) addr(%s) event(%s) \n",
			m_strTreeName.c_str(), m_strEventAddr.c_str(), m_strEventName.c_str());

	int		null = 0; /* Used to mark the end of the argument list */
	int		tstat, len;
	char	buf[128];
	int		dtype_Long = DTYPE_LONG;
	int		idesc = descr(&dtype_Long, &tstat, &null);

	sprintf(buf, "TCL(\"setevent %s\")", m_strEventName.c_str());

	useEventSocket ();
	if (epicsTrue != isMdsOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuFATAL, "[sfwTree::sendEvent] Error TCL(%s): %s.\n", buf, getStatusMsg());
		return (sfwTreeErr);
	}

	kuDebug (kuMON, "[sfwTree::sendEvent] tree(%s) addr(%s) event(%s) socket(%d) ok\n",
			m_strTreeName.c_str(), m_strEventAddr.c_str(), m_strEventName.c_str(), getSocket());

	return (sfwTreeOk);
}

const sfwTreeErr_e sfwTree::sendEventWithConnection ()
{
	kuDebug (kuMON, "[sfwTree::sendEventWithConnect] tree(%s) addr(%s) event(%s) connecting ...\n",
			m_strTreeName.c_str(), m_strEventAddr.c_str(), m_strEventName.c_str());

	int	socket = MdsConnect ((char *)m_strEventAddr.c_str());

	if (-1 == socket) {
		kuDebug (kuFATAL, "[sfwTree::sendEventWithConnect] MdsConnect() failed for %s\n", m_strEventAddr.c_str());
		return (sfwTreeErr);
	}

	sendEvent ();

	MdsDisconnect();

	kuDebug (kuMON, "[sfwTree::sendEventWithConnect] tree(%s) addr(%s) event(%s) ok\n",
			m_strTreeName.c_str(), m_strEventAddr.c_str(), m_strEventName.c_str());

	return (sfwTreeOk);
}

void sfwTree::setSocket (epicsInt32 iSocket)
{
	MdsSetSocket (&iSocket);
}

void sfwTree::useDataSocket ()
{
	setSocket (m_iSocket);
}

void sfwTree::useEventSocket ()
{
	if (epicsTrue == isEventConnected ()) {
		setSocket (m_iEventSocket);
	}
}

