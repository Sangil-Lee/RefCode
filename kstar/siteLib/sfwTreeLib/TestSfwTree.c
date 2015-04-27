/**
 * \file	TestSfwTree.c
 * \ingroup sfwtree
 * \brief   Segmented archiving 구현을 위한 C 예제
 * \example TestSfwTree.c
 * 
 * 본 파일은, sfwTree 라이브러리를 이용한 Segmented archiving 구현을 위한 C 예제를 제공한다.
 */

#include "sfwTree.h"

#define TEST_NODE_NUM		2
#define TEST_SEGMENT_NUM	10
#define TEST_DATA_NUM		10
#define TEST_SAMPLING_RATE	10

#define TEST_TREE_NAME		"r_spectro"
#define TEST_SHOT_NUM		1
#define TEST_MDS_ADDR		"172.17.102.58:8000"
#define TEST_EVENT_NAME		"VBS"
#define TEST_EVENT_ADDR		"127.0.0.1:8000"
#define TEST_DATA_FILE_HOME	"/data/segdata"

int main (int argc, char **argv)
{
	sfwTreePtr		pSfwTree = NULL;
	sfwTreeNodePtr	pSfwTreeNode[10];
	sfwTreeNodePtr	pSfwTreeScaleNode[10];
	char			szNodeName[100];
	char			szFileName[100];
	char			szScaleNodeName[100];
	epicsFloat64	dScaleValue;
	epicsInt16		data[100];
	epicsInt32		i, j, k;
	sfwTreeErr_e	eReturn;

	kuSetPrintLevel (kuTRACE);

	// 1. creates an object for Tree per each task (or device) : object & thread
	if (NULL == (pSfwTree = sfwTreeInit ("MyTreeHandler"))) {
		kuDebug (kuERR, "sfwTreeInit() failed\n");
		return (-1);
	}

	epicsThreadSleep (2.0);

	// 2. setup general configurations : at taskConfig() or arming
	eReturn = sfwTreeSetLocalFileHome (pSfwTree, TEST_DATA_FILE_HOME);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot set home directory for local data file\n");
		return (-1);
	}

	eReturn	= sfwTreeSetPutType (pSfwTree, sfwTreePutSegment);
	eReturn	= sfwTreeSetSamplingRate (pSfwTree, TEST_SAMPLING_RATE);

	// 3-1. setup shot information for archiving : at arming
	eReturn = sfwTreeSetShotInfo (pSfwTree, TEST_SHOT_NUM, TEST_TREE_NAME, TEST_MDS_ADDR, TEST_EVENT_NAME, TEST_EVENT_ADDR);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot set shot information\n");
		return (-1);
	}

	sfwTreePrintInfo (pSfwTree);

	for (i = 0; i < TEST_NODE_NUM; i++) {
		sprintf (szNodeName, "\\TOR_VB%02d:RAW", i + 1);
		sprintf (szFileName, "CH%02d", i + 1);

		// 3-2. add nodes to Tree object
		pSfwTreeNode[i] = sfwTreeAddNode (pSfwTree, szNodeName, sfwTreeNodeWaveform, sfwTreeNodeInt16, szFileName);
		//pSfwTreeNode[i] = sfwTreeAddNodeWithSamplingRate (pSfwTree, szNodeName, sfwTreeNodeWaveform, sfwTreeNodeInt16, szFileName, 20);

		if (NULL == pSfwTreeNode[i]) {
			kuDebug (kuERR, "cannot create TreeNode object\n");
			return (-1);
		}

		sprintf (szScaleNodeName, "\\TOR_VB%02d:FBITS", i + 1);

		// 3-2-1. add nodes for scale to Tree object
		pSfwTreeScaleNode[i]	= sfwTreeAddNode (pSfwTree, szScaleNodeName, sfwTreeNodeValue, sfwTreeNodeFloat64, NULL);

		if (NULL == pSfwTreeScaleNode[i]) {
			kuDebug (kuERR, "cannot create TreeNode object\n");
			return (-1);
		}
	}

	sfwTreePrintNodes (pSfwTree);

	// 4-1. initialize archiving : creates connection and file
	eReturn = sfwTreeBeginArchive (pSfwTree);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot begin archiving\n");
		return (-1);
	}

	for (j = 0; j < TEST_NODE_NUM; j++) {
		//dScaleValue	= 20 / 65536.0;
		dScaleValue	= 123.456;

		// 4-2-0. archiving : scalar value : scale
		eReturn = sfwTreePutFloat64 (pSfwTreeScaleNode[j], dScaleValue);

		if (sfwTreeOk != eReturn) {
			kuDebug (kuERR, "cannot put record\n");
			//return (-1);
		}
	}

	epicsThreadSleep (2.0);

	kuDebug (kuMON, "---------------------------------------------------------\n");
	kuDebug (kuMON, "staring of putData() ... \n");

	for (i = 0; i < TEST_SEGMENT_NUM; i++) {
		for (j = 0; j < TEST_NODE_NUM; j++) {
			// test data
			for (k = 0; k < TEST_DATA_NUM; k++) {
				data[k] = i * TEST_DATA_NUM + k;
			}

			// 4-2. archiving : node, index, start time, samples, buffer
			eReturn = sfwTreePutData (pSfwTreeNode[j], i, i, TEST_DATA_NUM, data);

			if (sfwTreeOk != eReturn) {
				kuDebug (kuERR, "cannot put record\n");
				return (-1);
			}

			// checks whether data of a node requested for archiving has been sheduled for writing to MDS server
			//sfwTreeSyncNode (pSfwTreeNode[j], 0.1, 0.05);
		}

		sfwTreeUpdateArchive (pSfwTree);

		epicsThreadSleep (1.0);
	}

	// checks whether data of all nodes requested for archiving has been sheduled for writing to MDS server
	//sfwTreeSyncArchive (pSfwTree, SFW_TREE_SYNC_TIMEOUT, SFW_TREE_SYNC_PERIOD);	// 10s, 1s

	// 4-3. cleanups archiving : flush data, close tree and disconnect
	eReturn = sfwTreeEndArchive (pSfwTree, sfwTreeActionFlush);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot cleanup archiving\n");
	}

	// 5. releases tree object
	eReturn = sfwTreeDelete (&pSfwTree);

	sfwTreePrintNodes (pSfwTree);

	return (0);
}

