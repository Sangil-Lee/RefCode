#include <math.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "sfwCommon.h"
#include "sfwDriver.h"
#include "sfwMDSplus.h"
#include "sfwTree.h" /* segmented archiving */


#include "myMDSplus.h"




/* An example of Data readout using the MDSplus Data Access library (mdslib) */
/*
int dtype_Float = DTYPE_FLOAT; 
int dtype_Double = DTYPE_DOUBLE;
int dtype_Long = DTYPE_LONG; 
int dtype_ULong = DTYPE_ULONG; 
int dypte_Short = DTYPE_SHORT ;
int dypte_UShort = DTYPE_USHORT ;
*/

#if USE_NORMAL_ARCHIVING
int mds_sendDatatoTree(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	int _null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int dataDesc; /* Signal descriptor */
	int realRate_Desc, value_at_start_Desc;
	int id_end_Desc;
	int nval;
	int opmode;
	char buf[64];
	
	int dtype_Double = DTYPE_DOUBLE;
	int dtype_ULong = DTYPE_ULONG; 

	
	int cntPerRec=0, cntPerRec_1=0;  /* must int type */
	double dStartTime;
	double dRealTimeGap;

	ViReal64 *f64WaveformPtr_prev;

	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser; 
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;

/*********************************  for multi trigger calculation   */
	int total_cnt; /* must int type */
/*	unsigned int sec2_start_cnt, sec3_start_cnt;
	unsigned int sec4_start_cnt, sec5_start_cnt;
	unsigned int sec6_start_cnt, sec7_start_cnt;
	unsigned int sec8_start_cnt; */
/*********************************  for multi trigger calculation   */

		
/*************************************************************************/
/*	if( !mds_Connect_Open(pERefCfg) ) {
		epicsPrintf("\n>>> mds_Connect_Open(%s)... ERROR!!\n", pERefCfg->taskName);
		return WR_ERROR;
	}
*/
	epicsPrintf("\n");
	if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
		return WR_ERROR;
	}
	opmode = pSTDdev->ST_Base.opMode;
	
/*************************************************************************/
	cntPerRec= pNI5122->n32ActualRecordLength;
	dRealTimeGap = 1.0 / pNI5122->f64ActualSampleRate;

	if( pNI5122->useMultiTrigger ) 
/*		total_cnt = (pSTDdev->ST_Base_fetch.dT0[SIZE_CNT_MULTI_TRIG-1] - pSTDdev->ST_Base_fetch.dT0[0]) * pNI5122->f64ActualSampleRate + cntPerRec; */
		total_cnt = cntPerRec * pNI5122->n32numRecords;
	else 
		total_cnt = cntPerRec;

/*
	sec2_start_cnt = (pSTDdev->ST_Base_fetch.dT0[1] - pSTDdev->ST_Base_fetch.dT0[0]) * pNI5122->f64ActualSampleRate;
	sec3_start_cnt = (pSTDdev->ST_Base_fetch.dT0[2] - pSTDdev->ST_Base_fetch.dT0[1]) * pNI5122->f64ActualSampleRate + sec2_start_cnt;
	sec4_start_cnt = (pSTDdev->ST_Base_fetch.dT0[3] - pSTDdev->ST_Base_fetch.dT0[2]) * pNI5122->f64ActualSampleRate + sec3_start_cnt;
	sec5_start_cnt = (pSTDdev->ST_Base_fetch.dT0[4] - pSTDdev->ST_Base_fetch.dT0[3]) * pNI5122->f64ActualSampleRate + sec4_start_cnt;
	sec6_start_cnt = (pSTDdev->ST_Base_fetch.dT0[5] - pSTDdev->ST_Base_fetch.dT0[4]) * pNI5122->f64ActualSampleRate + sec5_start_cnt;
	sec7_start_cnt = (pSTDdev->ST_Base_fetch.dT0[6] - pSTDdev->ST_Base_fetch.dT0[5]) * pNI5122->f64ActualSampleRate + sec6_start_cnt;
	sec8_start_cnt = (pSTDdev->ST_Base_fetch.dT0[7] - pSTDdev->ST_Base_fetch.dT0[6]) * pNI5122->f64ActualSampleRate + sec7_start_cnt;
*/
	printf("%s: count per Record: %d, virtaul count: %d fTimeGap: %f\n", pSTDch->chName, cntPerRec, total_cnt, dRealTimeGap );
/*	printf("%s: sec2 start cnt ID: %d, sec3 start cnt ID: %d\n", pSTDch->chName, sec2_start_cnt, sec3_start_cnt ); */
	
	
	f64WaveformPtr_prev = pNI5122_ch->f64WaveformPtr;
	for( nval=0; nval < pNI5122_ch->n32ActualNumWaveform; nval++) 
	{
		if ( pSTDdev->ST_Base.opMode == OPMODE_LOCAL) {
			if( pNI5122->useMultiTrigger ) 
				dStartTime = pSTDdev->ST_Base_fetch.dT0[nval];
			else
				dStartTime = 0.0;  /* Data Zero Start Time */
		}else
			dStartTime = pSTDdev->ST_Base_fetch.dT0[nval] - pSTDdev->ST_Base_fetch.fBlipTime;

		cntPerRec_1 = cntPerRec - 1; /* because array start with id 0 */
		
		dataDesc = descr(&dtype_Double, pNI5122_ch->f64WaveformPtr, &cntPerRec, &_null);
		realRate_Desc = descr(&dtype_Double, &dRealTimeGap, &_null);
		value_at_start_Desc = descr(&dtype_Double, &dStartTime, &_null);
		id_end_Desc = descr(&dtype_ULong, &cntPerRec_1, &_null);

		sprintf(buf, "%s%d", pSTDch->strTagName, nval);
		
		printf("shot \"%lu\": (%s, %s) tag:\"%s\" put .... ", pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->taskName, pSTDch->chName, buf);
		
		status = MdsPut(buf,
			"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
			&dataDesc, &id_end_Desc, &value_at_start_Desc, &realRate_Desc, &_null);

		printf("Done\n");
		
		pNI5122_ch->f64WaveformPtr = pNI5122_ch->f64WaveformPtr + pNI5122_ch->wfmInfoPtr[nval].actualSamples;
	}
	pNI5122_ch->f64WaveformPtr = f64WaveformPtr_prev;




	mds_Close_withFetch(pSTDdev);

	if( opmode == OPMODE_REMOTE ) 
		sprintf(buf, "Remote");
	else if( opmode == OPMODE_LOCAL) 
		sprintf(buf, "Local");
	else if( opmode == OPMODE_CALIBRATION) 
		sprintf(buf, "Calibration");
	else
		sprintf(buf, "N/A");

	printlog("%s %d: %s:%s(%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f) %.1fMHz,%dx%d %.2fMB\n", buf,
		pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->taskName, pSTDch->chName,
		pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT0[1], pSTDdev->ST_Base_fetch.dT0[2], 
		pSTDdev->ST_Base_fetch.dT0[3], pSTDdev->ST_Base_fetch.dT0[4], pSTDdev->ST_Base_fetch.dT0[5], 
		pSTDdev->ST_Base_fetch.dT0[6], pSTDdev->ST_Base_fetch.dT0[7],
		pNI5122->f64ActualSampleRate/1000000., 
		pNI5122->n32numRecords, cntPerRec,  (double)sizeof(double) * (double)total_cnt /1024./1024.);
	
	return WR_OK;
}
#endif

#if USE_SEGMENTED_ARCHIVING
int mds_sendDatatoTree(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	int _null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int dataDesc; /* Signal descriptor */
	int realRate_Desc, value_at_start_Desc;
	int id_end_Desc;
	int nval;
	int opmode;
	char buf[64];
	
	int dtype_Double = DTYPE_DOUBLE;
	int dtype_ULong = DTYPE_ULONG; 

	
	int cntPerRec=0, cntPerRec_1=0;  /* must int type */
	double dStartTime;
	double dRealTimeGap;

	ViReal64 *f64WaveformPtr_prev;

	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser; 
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;

/*********************************  for multi trigger calculation   */
	int total_cnt; /* must int type */
/*********************************  for multi trigger calculation   */

/********************* segment archiving 2013. 7. 25 */
//	sfwTreePtr		pSfwTree = NULL;
//	sfwTreeNodePtr	pSfwTreeNode[10];
//	sfwTreeNodePtr	pSfwTreeScaleNode[10];
//	char			szNodeName[100];
//	char			szFileName[100];
//	char			szScaleNodeName[100];
//	epicsFloat64	dScaleValue;
//	epicsInt16		data[100];
//	epicsInt32		i, j, k;
	sfwTreeErr_e	eReturn;
/********************************************************/
	


#if 1
	opmode = pSTDdev->ST_Base.opMode;

	// 1. creates an object for Tree per each task (or device) : object & thread
//	if (NULL == (pNI5122->pSfwTree = sfwTreeInit ("ERtreeHandler"))) {
//		kuDebug (kuERR, "sfwTreeInit() failed\n");
//		return;
//	}

//	epicsThreadSleep (2.0);

	// 2. setup general configurations : at taskConfig() or arming
//	eReturn = sfwTreeSetLocalFileHome (pSfwTree, TEST_DATA_FILE_HOME);

//	if (sfwTreeOk != eReturn) {
//		kuDebug (kuERR, "cannot set home directory for local data file\n");
//		return;
//	}

//	eReturn = sfwTreeSetPutType (pSfwTree, sfwTreePutSegment);
//	eReturn = sfwTreeSetSamplingRate (pSfwTree, TEST_SAMPLING_RATE);

	// 3-1. setup shot information for archiving : at arming
//	eReturn = sfwTreeSetShotInfo (pSfwTree, TEST_SHOT_NUM, TEST_TREE_NAME, TEST_MDS_ADDR, TEST_EVENT_NAME, TEST_EVENT_ADDR);
//	eReturn = sfwTreeSetShotInfo (pSfwTree,pSTDdev->ST_Base_fetch.shotNumber, pSTDch->strTagName, pSTDdev->ST_mds.treeIPport[opmode],pSTDdev->ST_mds.treeEventArg, pSTDdev->ST_mds.treeEventIP);
//	if (sfwTreeOk != eReturn) {
//		kuDebug (kuERR, "cannot set shot information\n");
//		return;
//	}

//	sfwTreePrintInfo (pSfwTree);
/*
	for (i = 0; i < TEST_NODE_NUM; i++) {
		sprintf (szNodeName, "\\TOR_VB%02d:FOO", i + 1);
		sprintf (szFileName, "CH%02d", i + 1);

		// 3-2. add nodes to Tree object
		pSfwTreeNode[i] = sfwTreeAddNode (pSfwTree, szNodeName, sfwTreeNodeWaveform, sfwTreeNodeInt16, szFileName);

		if (NULL == pSfwTreeNode[i]) {
			kuDebug (kuERR, "cannot create TreeNode object\n");
			return;
		}

		sprintf (szScaleNodeName, "\\TOR_VB%02d:SCALE", i + 1);

		// 3-2-1. add nodes for scale to Tree object
		pSfwTreeScaleNode[i]	= sfwTreeAddNode (pSfwTree, szScaleNodeName, sfwTreeNodeValue, sfwTreeNodeFloat64, NULL);

		if (NULL == pSfwTreeScaleNode[i]) {
			kuDebug (kuERR, "cannot create TreeNode object\n");
			return;
		}
	}
*/
//	sfwTreePrintNodes (pNI5122->pSfwTree);


	cntPerRec= pNI5122->n32ActualRecordLength;

	// 4-1. initialize archiving : creates connection and file
	eReturn = sfwTreeBeginArchive (pNI5122->pSfwTree);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot begin archiving\n");
	}
/*
	for (j = 0; j < TEST_NODE_NUM; j++) {
		//dScaleValue	= 20 / 65536.0;
		dScaleValue = 123.456;

		// 4-2-0. archiving : scalar value : scale
		eReturn = sfwTreePutFloat64 (pSfwTreeScaleNode[j], dScaleValue);

		if (sfwTreeOk != eReturn) {
			kuDebug (kuERR, "cannot put record\n");
			//return;
		}
	}
*/
//	epicsThreadSleep (2.0);


//	kuDebug (kuMON, "---------------------------------------------------------\n");
//	kuDebug (kuMON, "staring of putData() ... \n");
	f64WaveformPtr_prev = pNI5122_ch->f64WaveformPtr;
	for (nval = 0; nval < pNI5122_ch->n32ActualNumWaveform; nval++) {
//		for (j = 0; j < TEST_NODE_NUM; j++) {
			// test data
//			for (k = 0; k < TEST_DATA_NUM; k++) {
//				data[k] = i * TEST_DATA_NUM + k;
//			}

			if ( opmode == OPMODE_LOCAL) {
				if( pNI5122->useMultiTrigger ) 
					dStartTime = pSTDdev->ST_Base_fetch.dT0[nval];
				else
					dStartTime = 0.0;  /* Data Zero Start Time */
			}else
				dStartTime = pSTDdev->ST_Base_fetch.dT0[nval] - pSTDdev->ST_Base_fetch.fBlipTime;

			// 4-2. archiving : node, index, start time, samples, buffer
			eReturn = sfwTreePutData (pNI5122_ch->pSfwTreeNode, nval, dStartTime, cntPerRec, pNI5122_ch->f64WaveformPtr);

			if (sfwTreeOk != eReturn)
				kuDebug (kuERR, "cannot put record\n");
//		}

//		sfwTreeUpdateArchive (pNI5122->pSfwTree);

		epicsThreadSleep (0.1);
		pNI5122_ch->f64WaveformPtr = pNI5122_ch->f64WaveformPtr + pNI5122_ch->wfmInfoPtr[nval].actualSamples;

	}
	pNI5122_ch->f64WaveformPtr = f64WaveformPtr_prev;
	sfwTreeUpdateArchive (pNI5122->pSfwTree);

	// 4-3. cleanups archiving : flush data, close tree and disconnect
	eReturn = sfwTreeEndArchive (pNI5122->pSfwTree, sfwTreeActionFlush);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot cleanup archiving\n");
	}

	// 5. releases tree object
//	eReturn = sfwTreeDelete (&pSfwTree);

//	sfwTreePrintNodes (pNI5122->pSfwTree);
#endif


	if( opmode == OPMODE_REMOTE ) 
		sprintf(buf, "Remote");
	else if( opmode == OPMODE_LOCAL) 
		sprintf(buf, "Local");
	else if( opmode == OPMODE_CALIBRATION) 
		sprintf(buf, "Calibration");
	else
		sprintf(buf, "N/A");

	printlog("%s %d: %s:%s(%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f) %.1fMHz,%dx%d %.2fMB\n", buf,
		pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->taskName, pSTDch->chName,
		pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT0[1], pSTDdev->ST_Base_fetch.dT0[2], 
		pSTDdev->ST_Base_fetch.dT0[3], pSTDdev->ST_Base_fetch.dT0[4], pSTDdev->ST_Base_fetch.dT0[5], 
		pSTDdev->ST_Base_fetch.dT0[6], pSTDdev->ST_Base_fetch.dT0[7],
		pNI5122->f64ActualSampleRate/1000000., 
		pNI5122->n32numRecords, cntPerRec,  (double)sizeof(double) * (double)total_cnt /1024./1024.);
	
	return WR_OK;
}

#endif

