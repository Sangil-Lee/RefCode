/****************************************************************************
 * sfwTree.c
 * --------------------------------------------------------------------------
 * C API for MDSplus segment archiving
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

#include "sfwTree.h"
#include "sfwTreeObj.h"

static sfwTree	*gpSfwTree	= NULL;

/**
 * \fn     sfwTree *getSfwTree (sfwTreePtr pSfwTreeHandle)
 * \brief  sfwTreePtr 데이터 유형의 주소로부터 sfwTree 데이터 유형의 트리 객체 포인터 얻기
 * \param  pSfwTreeHandle	sfwTreeInit()에 의해 생성된 트리 객체
 * \return sfwTree 클래스 유형의 트리 객체
 * \see    sfwTreeInit
 */
static sfwTree *getSfwTree (sfwTreePtr pSfwTreeHandle) 
{
	return ( (sfwTree *)pSfwTreeHandle );
}

/**
 * \fn     sfwTreeNode *getSfwTreeNode (sfwTreeNodePtr pSfwTreeNode)
 * \brief  sfwTreeNodePtr 데이터 유형의 주소로부터 sfwTreeNode 데이터 유형의 노드 객체 포인터 얻기
 * \param  pSfwTreeNode		sfwTreeAddNode()에 의해 생성된 노드 객체
 * \return sfwTreeNode 클래스 유형의 노드 객체
 * \see    sfwTreeAddNode
 */
static sfwTreeNode *getSfwTreeNode (sfwTreeNodePtr pSfwTreeNode) 
{
	return ( (sfwTreeNode *)pSfwTreeNode );
}

epicsShareFunc epicsBoolean epicsShareAPI sfwTreeEnabled ()
{
	return (sfwTree::isEnable());
}

// creates an object for Tree
epicsShareFunc sfwTreePtr epicsShareAPI sfwTreeInit (const char *pszHandleName)
{
	// Although sfwTree is not enabled, initialization is performed to prepare when sfwTree is enabled again
	//if (epicsTrue != sfwTreeEnabled ())	return (NULL);

	if (NULL == pszHandleName) {
		kuDebug (kuERR, "[sfwTreeInit] handler name is invalid\n");
		return (NULL);
	}

#if 0
	return (new sfwTree (pszHandleName));
#else
	//TODO
	if (NULL == gpSfwTree) {
		gpSfwTree	= new sfwTree (pszHandleName);
	}

	return (gpSfwTree);
#endif

}

epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetLocalFileHome (sfwTreePtr pSfwTree, const char *pszHomeDirName)
{
	return (getSfwTree(pSfwTree)->setLocalFileHome (pszHomeDirName));
}

epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetPutType (sfwTreePtr pSfwTree, sfwTreePutType_e eDataPutType)
{
	return (getSfwTree(pSfwTree)->setDataPutType (eDataPutType));
}

epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetSamplingRate (sfwTreePtr pSfwTree, epicsUInt32 uiSamplingRate)
{
	return (getSfwTree(pSfwTree)->setSamplingRate (uiSamplingRate));
}

epicsShareFunc epicsUInt32 epicsShareAPI sfwTreeGetSamplingRate (sfwTreePtr pSfwTree)
{
	return (getSfwTree(pSfwTree)->getSamplingRate ());
}

epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetNodeSamplingRate (sfwTreeNodePtr pSfwTreeNode, epicsUInt32 uiSamplingRate)
{
	return (getSfwTreeNode(pSfwTreeNode)->setSamplingRate (uiSamplingRate));
}

epicsShareFunc epicsUInt32 epicsShareAPI sfwTreeGetNodeSamplingRate (sfwTreeNodePtr pSfwTreeNode)
{
	return (getSfwTreeNode(pSfwTreeNode)->getSamplingRate ());
}

epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetShotInfo (sfwTreePtr pSfwTree, epicsInt32 iTreeShotNo,
	const char *pszTreeName, const char *pszTreeMdsAddr, const char *pszTreeEventName, const char *pszTreeEventAddr)
{
	if (NULL == pSfwTree) {
		kuDebug (kuERR, "[sfwTreeSetShotInfo] tree object is invalid\n");
		return (sfwTreeErr);
	}

	if (NULL == pszTreeName) {
		kuDebug (kuERR, "[sfwTreeSetShotInfo] tree name is invalid\n");
		return (sfwTreeErr);
	}

	return (getSfwTree(pSfwTree)->setShotInfo (iTreeShotNo, pszTreeName, pszTreeMdsAddr, pszTreeEventName, pszTreeEventAddr));
}

epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetShotNum (sfwTreePtr pSfwTree, epicsInt32 iTreeShotNo)
{
	if (NULL == pSfwTree) {
		kuDebug (kuERR, "[sfwTreeSetShotNum] tree object is invalid\n");
		return (sfwTreeErr);
	}

	return (getSfwTree(pSfwTree)->setShotNum (iTreeShotNo));
}

// addes a node to Tree object
epicsShareFunc sfwTreeNodePtr epicsShareAPI sfwTreeAddNode (sfwTreePtr pSfwTree,
	const char *pszNodeName, sfwTreeNodeType_e eNodeType, sfwTreeDataType_e	eNodeDataType, const char *pszFileName)
{
	// uses sampling rate of tree
	return ( sfwTreeAddNodeWithSamplingRate (pSfwTree, pszNodeName, eNodeType, eNodeDataType, pszFileName, 0));
}

// addes a node with own sampling rate to Tree object
epicsShareFunc sfwTreeNodePtr epicsShareAPI sfwTreeAddNodeWithSamplingRate (sfwTreePtr pSfwTree,
	const char *pszNodeName, sfwTreeNodeType_e eNodeType, sfwTreeDataType_e	eNodeDataType, const char *pszFileName, epicsUInt32 uiSamplingRate)
{
	if (epicsTrue != sfwTreeEnabled ())	return (NULL);

	if (!pSfwTree || !pszNodeName) {
		kuDebug (kuERR, "[sfwTreeAddNode] argument is invalid\n");
		return (NULL);
	}

	return (getSfwTree(pSfwTree)->addNode (pszNodeName, eNodeType, eNodeDataType, pszFileName, uiSamplingRate));
}

// initialize archiving : connect and open tree
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeBeginArchive (sfwTreePtr pSfwTree)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	return (getSfwTree(pSfwTree)->beginArchive ());
}

// puts data to a node
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreePutData (sfwTreeNodePtr pSfwTreeNode, 
		epicsInt32 iIndex, epicsFloat64 dStartTime, epicsUInt32 uiSamples, sfwTreeDataPtr pDataPtr)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	return (getSfwTreeNode(pSfwTreeNode)->putData (iIndex, dStartTime, uiSamples, pDataPtr));
}

// puts a float64 value to a node
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreePutFloat64 (sfwTreeNodePtr pSfwTreeNode, epicsFloat64 value)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	return (getSfwTreeNode(pSfwTreeNode)->putData (value));
}

// send event
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeUpdateArchive (sfwTreePtr pSfwTree)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	return (getSfwTree(pSfwTree)->updateArchive ());
}

// checking whether data of a node requested for archiving has been sheduled for writing to MDS server
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSyncNode (sfwTreeNodePtr pSfwTreeNode, 
		epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	sfwTreeNode	*pTreeNode	= getSfwTreeNode (pSfwTreeNode);

	return (pTreeNode->getTree()->waitForSync (pTreeNode, dTimeout, dCheckPeriod));
}

// checking whether data of all nodes requested for archiving has been sheduled for writing to MDS server
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSyncArchive (sfwTreePtr pSfwTree,
		epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	return (getSfwTree(pSfwTree)->waitForSync (dTimeout, dCheckPeriod));
}

// cleanup archiving : flush data, close tree and disconnect
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeEndArchive (sfwTreePtr pSfwTree, sfwTreeEndAction_e eAction)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	return (getSfwTree(pSfwTree)->endArchive (eAction));
}

// releases object for Tree
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeDelete (sfwTreePtr *pSfwTree)
{
	if (epicsTrue != sfwTreeEnabled ())	return (sfwTreeOk);

	//TODO

	*pSfwTree = NULL;

	return (sfwTreeOk);
}

// display information of Tree
epicsShareFunc void epicsShareAPI sfwTreePrintInfo (sfwTreePtr pSfwTree)
{
	if (NULL != pSfwTree) {
		getSfwTree(pSfwTree)->printInfo ();
	}
}

// display list of node
epicsShareFunc void epicsShareAPI sfwTreePrintNodes (sfwTreePtr pSfwTree)
{
	if (NULL != pSfwTree) {
		getSfwTree(pSfwTree)->printNodes ();
	}
}

// check whether connection is established
epicsShareFunc epicsBoolean epicsShareAPI sfwTreeConnected (sfwTreePtr pSfwTree)
{
	if (NULL == pSfwTree) {
		return (epicsFalse);
	}

	return (getSfwTree(pSfwTree)->isConnected ());
}

