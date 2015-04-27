
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "sfwMDSplus.h"
#include "sfwCommon.h"
#include "myMDSplus.h"

#include "drvNIscope.h"
#include "drvNIfgen.h"



LOCAL long drvNIscope_io_report(int level);
LOCAL long drvNIscope_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvNIscope = {
       2,
       drvNIscope_io_report,
       drvNIscope_init_driver
};

epicsExportAddress(drvet, drvNIscope);




int create_ni5122_taskConfig( ST_STD_device *pSTDdev)
{
	sfwTreeErr_e	eReturn;
	ST_NISCOPE_dev *pNI5122 = NULL;
	pNI5122 = (ST_NISCOPE_dev*) malloc(sizeof(ST_NISCOPE_dev));
	if(!pNI5122) return WR_ERROR;

	pSTDdev->pUser = pNI5122;
	
/************************************* for NI-scope */
	pNI5122->vi = VI_NULL;

	strcpy(pNI5122->cResource, pSTDdev->strArg0);

	pNI5122->myStatusDev = 0x0;

	pNI5122->f64minSampleRate = INIT_SCOPE_SAMPLERATE;
	pNI5122->n32minRecordLength = INIT_SCOPE_RECD_LEN;
/*	pNI5122->f64refPosition = 50.0; */
	pNI5122->f64refPosition = 0.0; /* i don't want pretriggered samples, percentage */
	pNI5122->n32numRecords = 1;
	pNI5122->b8enforceRealtime = NISCOPE_VAL_TRUE;

	strcpy( pNI5122->triggerSource, INIT_SCOPE_TRIG_SRC);
	pNI5122->useMultiTrigger = INIT_USE_MULTI_TRIGGER;
/************************************* for MDSplus */

#if USE_SEGMENTED_ARCHIVING

	pNI5122->pSfwTree = NULL;
// 1. creates an object for Tree per each task (or device) : object & thread
	if (NULL == (pNI5122->pSfwTree = sfwTreeInit ("ERtreeHandler"))) {
		kuDebug (kuERR, "sfwTreeInit() failed\n");
		return WR_ERROR;
	}

	// 2. setup general configurations : at taskConfig() or arming
	eReturn = sfwTreeSetLocalFileHome (pNI5122->pSfwTree, NULL);

	if (sfwTreeOk != eReturn) {
		kuDebug (kuERR, "cannot set home directory for local data file\n");
		return WR_ERROR;
	}
	eReturn = sfwTreeSetPutType (pNI5122->pSfwTree, sfwTreePutSegment);
//	eReturn = sfwTreeSetSamplingRate (pNI5122->pSfwTree, TEST_SAMPLING_RATE); // move to "Arming".
#endif




/**********************************************************************/
/* almost standard function.......................  */
/**********************************************************************/

	pSTDdev->ST_Func._SYS_ARMING = func_ni5122_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_ni5122_SYS_RUN;
	pSTDdev->ST_Func._OP_MODE = func_ni5122_OP_MODE;

	pSTDdev->ST_Func._Exit_Call = clear_niscope_device;


	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ni5122_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		free(pSTDdev);
		return WR_ERROR;
	}



	return WR_OK;
}


#if 0
void threadFunc_DAQ_trigger(void *param)
{
	int waitCnt=0;
	int fetch_cnt = 0;
	int fetch_success_cnt = 0;
	
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_STD_channel *pSTDch = NULL;

/*	ST_threadCfg *pDAQST_threadConfig=NULL; */

	ST_NISCOPE_dev *pNI5122 = NULL;
	ST_NISCOPE_ch *pNI5122_ch = NULL;
/*	ST_MASTER *pAdminCfg = get_master(); */


	if( !pSTDdev ) {
		printf("ERROR! threadFunc_DAQ_trigger() has null pointer of STD device.\n");
		return;
	}
	pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;


	while(TRUE) 
	{
		epicsEventWait( pSTDdev->epicsEvent_DAQthread );
		waitCnt=0;

		while( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER )
		{
			epicsThreadSleep(1.0);
			epicsPrintf(" %X ", pSTDdev->StatusDev);
			if( (waitCnt%10) == 0 ) epicsPrintf("\n");
			if( pNI5122->myStatusDev & SCOPE_ABORT_ACQUSITION ) {
				epicsPrintf(">>> %s: Receive abort command. Bye!\n", pSTDdev->taskName );
				break;
			}
			if( ( drvNIscope_get_AcquisitionStatus(pSTDdev) ==  NISCOPE_VAL_ACQ_COMPLETE ) &&
				!(pNI5122->myStatusDev & SCOPE_ABORT_ACQUSITION) ) 
			{
				
				pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
				pSTDdev->StatusDev |= TASK_POST_PROCESS;
				pNI5122->myStatusDev &= ~SCOPE_BEGIN_ACQUSITION;
				scanIoRequest(pSTDdev->ioScanPvt_status);	
				notify_refresh_master_status();
#if 0
				pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
				while(pSTDch) {
					if( drvNIscope_memCheck_Fech(pSTDdev, pSTDch ) == WR_ERROR ) {
						printf("\"%s/%s\" mem check error to fetch\n", pSTDdev->taskName, pSTDch->chName );	
						break;
					}
					pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
				}
#endif
#if USE_AUTO_FETCH
				dbProc_call_Fetch_start(pSTDdev);

				epicsThreadSleep(2.0);

				fetch_cnt = 0;
				fetch_success_cnt = 0;
				pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
				while( 1) {
					pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;
					
					if( pNI5122_ch->myStatusCh & SCOPE_START_FETCH )
						fetch_cnt++;
					if( pNI5122_ch->myStatusCh & SCOPE_FETCH_SUCCESS )
						fetch_success_cnt++;

/*					epicsPrintf("%s: channelStatus: 0x%x\n", pSTDch->chName, pchannelConfig->channelStatus ); */
					pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
					pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;
					
					if( pNI5122_ch->myStatusCh & SCOPE_START_FETCH )
						fetch_cnt++;
					if( pNI5122_ch->myStatusCh & SCOPE_FETCH_SUCCESS )
						fetch_success_cnt++;

/*					epicsPrintf("%s: channelStatus: 0x%x\n", pSTDch->chName, pchannelConfig->channelStatus ); */
					pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);

					if( fetch_cnt > 0 ) {
						epicsPrintf(">>> %s: Wait Fetch complete. %d, %d\n", pSTDdev->taskName, fetch_cnt, fetch_success_cnt);
						epicsThreadSleep(1.0);
						fetch_cnt = 0;
					} 
					else 
						break;
				}

				pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;
				if( fetch_success_cnt == 2 ) {
					pNI5122_ch->myStatusCh &= ~SCOPE_FETCH_SUCCESS;
					pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
					pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;
					pNI5122_ch->myStatusCh &= ~SCOPE_FETCH_SUCCESS;
				}
#endif
				break;
			}
			
			waitCnt++;
			if( waitCnt> 120 ) /* 2 minute */
			{
				epicsPrintf("%s: wait time out... bye!!\n", pSTDdev->taskName);
				if( drvNIscope_abort_acquition(pSTDdev) == WR_OK) {
					epicsPrintf(">>> %s: Abort acquition.\n", pSTDdev->taskName );
				}
				break;
			}
		}
		pNI5122->myStatusDev &= ~SCOPE_BEGIN_ACQUSITION;
		pNI5122->myStatusDev &= ~SCOPE_ABORT_ACQUSITION;
		pNI5122->myStatusDev |= SCOPE_FINISH_ACQUISITION;
		pSTDdev->StatusDev &= ~TASK_POST_PROCESS;

		scanIoRequest(pSTDdev->ioScanPvt_status);

#if USE_AUTO_TREE_PUT		
		pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
		pNI5122->myStatusDev &= ~SCOPE_STORAGE_FINISH;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		notify_refresh_master_status();
		
		mds_Connect(pSTDdev);
		dbProc_call_TREEput_start(pSTDdev);
		mds_Disconnect(pSTDdev);

		pNI5122->myStatusDev |= SCOPE_STORAGE_FINISH;
		pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;	
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		scanIoRequest(pSTDdev->ioScanPvt_status);

		notify_refresh_master_status();
		
		if( drvNIscope_check_DataPutFinish() == WR_OK )	
			drvNIfgen_stop_all_Generation();
#endif
		
	}
	
}
#endif

void threadFunc_ni5122_DAQ(void *param)
{
	int waitCnt=0;
	ViInt32 niStatus;
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_STD_channel *pSTDch = NULL;
	ST_NISCOPE_dev *pNI5122 = NULL;	
	ST_MASTER *pAdminCfg = NULL;
	int ltuCmd = 0;
	char cLtuCmd[4];

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_DAQ_trigger() has null pointer of STD device.\n");
		return;
	}
	pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	pAdminCfg = get_master();

	while(TRUE) 
	{
		epicsPrintf("%s waiting DAQ start.\n", pSTDdev->taskName );
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		waitCnt=0;
		printf("\"%s\" got start message.\n", pSTDdev->taskName );
		epicsThreadSleep(0.3 );
		
		while( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER || pSTDdev->StatusDev & TASK_IN_PROGRESS )
		{
			epicsThreadSleep(1.0);
			
			if( pNI5122->myStatusDev & SCOPE_ABORT_ACQUSITION ) {
				epicsPrintf(">>> %s: Receive abort command. Bye!\n", pSTDdev->taskName );
				break;
			}
			niStatus = drvNIscope_get_AcquisitionStatus(pSTDdev);
			if(  niStatus ==  NISCOPE_VAL_ACQ_COMPLETE  &&
				!(pNI5122->myStatusDev & SCOPE_ABORT_ACQUSITION) ) 
			{
/*		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER; */
				pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
				pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
				pSTDdev->StatusDev |= TASK_POST_PROCESS;
				scanIoRequest(pSTDdev->ioScanPvt_status);
				pNI5122->myStatusDev &= ~SCOPE_BEGIN_ACQUSITION;
				notify_refresh_master_status();


				pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
				while(pSTDch) 
				{
#if 0
					if( drvNIscope_memCheck_Fech(pSTDdev, pSTDch ) == WR_ERROR ) {
						printf("\"%s/%s\" mem check error to fetch\n", pSTDdev->taskName, pSTDch->chName );
						break;
					}
#endif
					if( drvNIscope_get_fetchData(pSTDdev, pSTDch) == WR_ERROR ) {
						epicsPrintf("ERROR! %s/%s: fetch data failed.\n", pSTDdev->taskName, pSTDch->chName );
						pSTDdev->ErrorDev = NISCOPE_ERROR_FETCH_DATA;
					}
					epicsThreadSleep(1.0);
					pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
				}

				pSTDdev->StatusDev &= ~TASK_POST_PROCESS;
				pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
				scanIoRequest(pSTDdev->ioScanPvt_status);
				notify_refresh_master_status();

				if( admin_spinLock_mds_put_flag(pSTDdev) == WR_OK ) 
				{ 
#if USE_NORMAL_ARCHIVING				
					mds_Connect(pSTDdev);
#endif
					pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);				
					while(pSTDch) {
						printf("\"%s/%s\" tree put data.\n", pSTDdev->taskName, pSTDch->chName );
						mds_sendDatatoTree(pSTDdev, pSTDch);
						pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
					}
#if USE_NORMAL_ARCHIVING
					mds_Disconnect(pSTDdev);
#endif

					printf("\"%s\" release mds put flag ", pSTDdev->taskName );
					/*epicsMutexLock(pST_MasterCfg->lock_mds);*/
					pAdminCfg->n8MdsPutFlag  = 0; /* ok. I'm finished. release write process. */				
					/*epicsMutexUnlock(pST_MasterCfg->lock_mds);*/
					printf("... OK.\n");
					
				}

				pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
				pSTDdev->StatusDev |= TASK_STANDBY;
				pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
/*				notify_refresh_master_status(); */

				break;
			}
			else if( niStatus ==  NISCOPE_VAL_ACQ_IN_PROGRESS ) 
			{
				if( !(pSTDdev->StatusDev & TASK_IN_PROGRESS) ) 
				{	
					epicsPrintf("%s: ACQ in progress\n", pSTDdev->taskName );
					pSTDdev->StatusDev |= TASK_IN_PROGRESS;
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					scanIoRequest(pSTDdev->ioScanPvt_status);
					notify_refresh_master_status();
				}
			} 
			else 
			{
				epicsPrintf(" waiting trigger 0x%X ", pSTDdev->StatusDev);
				if( (waitCnt%10) == 0 ) epicsPrintf("\n");
			}

			waitCnt++;
			if( waitCnt> 120 ) /* 2 minute */
			{
				epicsPrintf("%s: wait time out... bye!!\n", pSTDdev->taskName);
				if( drvNIscope_abort_acquition(pSTDdev) == WR_OK) {
					epicsPrintf(">>> %s: Abort acquition.\n", pSTDdev->taskName );
				}
				break;
			}

		}		

		pNI5122->myStatusDev &= ~SCOPE_BEGIN_ACQUSITION;
		pNI5122->myStatusDev &= ~SCOPE_ABORT_ACQUSITION;
		pNI5122->myStatusDev |= SCOPE_FINISH_ACQUISITION;
		
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		scanIoRequest(pSTDdev->ioScanPvt_status);

		epicsPrintf(">>> %s: finish DAQing.\n", pSTDdev->taskName );
/*
		db_put(SYS_RUN, "0");
		db_put(SYS_ARMING, "0");
*/
		if( drvNIscope_check_DataPutFinish() == WR_OK )
		{
/*			drvNIfgen_stop_all_Generation();	*/ /* do in ARM 0 */

			db_put(pAdminCfg->strPvNames[NUM_SYS_RUN], "0");			
/*			db_put(SYS_RUN, "0");*/
			epicsThreadSleep(1.);
			db_put(pAdminCfg->strPvNames[NUM_SYS_ARMING], "0");
/*			db_put(SYS_ARMING, "0");*/
			epicsThreadSleep(1.);

			notify_refresh_master_status();
		}
		
		if( pAdminCfg->ST_Base.opMode == OPMODE_LOCAL ) {
			sprintf(cLtuCmd, "%d", ltuCmd);
			printf("%s: call LTU shot end!\n", pAdminCfg->taskName);
			db_put("ER_LTU_SHOT_END", cLtuCmd);
			ltuCmd = !ltuCmd;
		}
		
	}

}



int drvNIscope_memCheck_Fech(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = NULL;
	ST_NISCOPE_ch *pNI5122_ch = NULL;
	pNI5122 = (ST_NISCOPE_dev*)pSTDdev->pUser;
	pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;
	
	if( drvNIscope_get_SampleRate(pSTDdev) == WR_ERROR) {
		pSTDdev->ErrorDev = NISCOPE_ERROR_SMP_RATE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return WR_ERROR;
	}
	if( drvNIscope_get_RecordLength(pSTDdev) == WR_ERROR) {
		pSTDdev->ErrorDev = NISCOPE_ERROR_REC_LENGTH;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return WR_ERROR;
	}

	if (pNI5122_ch->wfmInfoPtr) {
		printf("call free ( %s wfmInfoPtr) ", pSTDch->chName);
		free (pNI5122_ch->wfmInfoPtr);
		printf("... OK\n");
	}
	pNI5122_ch->wfmInfoPtr = malloc(sizeof(struct niScope_wfmInfo) * pNI5122_ch->n32ActualNumWaveform );
	if (pNI5122_ch->wfmInfoPtr == NULL) {
		printf("\n>>> %s/%s: NISCOPE_ERROR_INSUFFICIENT_MEMORY (wfmInfoPtr)\n", pSTDdev->taskName, pSTDch->chName );
		pSTDdev->ErrorDev = NISCOPE_ERROR_MALLOC;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return WR_ERROR;
	}
	/*******************************************/
	if (pNI5122_ch->f64WaveformPtr) {
		printf("call free (%s f64WaveformPtr) ", pSTDch->chName);
		free( pNI5122_ch->f64WaveformPtr);
		printf("... OK\n");
	}
	pNI5122_ch->f64WaveformPtr = (ViReal64*) malloc (sizeof (ViReal64) * pNI5122->n32ActualRecordLength * pNI5122_ch->n32ActualNumWaveform );
	if (pNI5122_ch->f64WaveformPtr == NULL) {
		printf("\n>>> %s/%s: NISCOPE_ERROR_INSUFFICIENT_MEMORY (f64WaveformPtr)\n", pSTDdev->taskName, pSTDch->chName );
		pSTDdev->ErrorDev = NISCOPE_ERROR_MALLOC;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		free(pNI5122_ch->wfmInfoPtr);
		return WR_ERROR;
	}

	return WR_OK;
}

LOCAL long drvNIscope_init_driver(void)
{
	ViStatus error = VI_SUCCESS;

	ST_MASTER *pAdminConfig = NULL;	
	ST_STD_device *pSTDdev = NULL;
	ST_NISCOPE_dev *pNI5122 = NULL;
	ST_STD_channel *pSTDch = NULL;
	
	pAdminConfig = get_master();
	if(!pAdminConfig)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pAdminConfig->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) )
		{	
			if( create_ni5122_taskConfig(pSTDdev) == WR_ERROR )
				return -1;

			pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
			while(pSTDch) {
				pSTDch->pUser = create_ni5122_channel();
				if(!pSTDch->pUser) {
					printf("ERROR!	\"%s\" create_ni5122_channel() failed.\n", pSTDdev->taskName );
					free(pSTDdev);
					return -1;
				}
				/* Important...... just for NI device...... */
				sprintf(pSTDch->chName, "%d", pSTDch->channelId);
				
				pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
			}
			
			
			pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

			printf("call function: niScope_init( \"%s\" ) \n", pNI5122->cResource );
			error = niScope_init(pNI5122->cResource, NISCOPE_VAL_TRUE, NISCOPE_VAL_TRUE, &(pNI5122->vi));
			niScope_error(pSTDdev, error);
/*
			if( dbProc_Init_user(pSTDdev) == WR_ERROR )
				epicsPrintf("\n>>> drvNIscope_init_driver(): dbProc_Init() ... ERROR! \n");
*/
/*			epicsAtExit((VOIDFUNCPTREXIT) clear_niscope_device, (void *)pSTDdev); */
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pSTDdev) { */

	printf("Local NI Scope init OK!\n");
	
	return 0;
}

void clear_niscope_device(void *pArg, double arg1, double arg2)
{
	ST_STD_device *pSTDdev = NULL;
	ST_NISCOPE_dev *pNI5122 = NULL;
	sfwTreeErr_e	eReturn;
	
	pSTDdev = (ST_STD_device *)pArg;
	if( !pSTDdev ) {
		printf("Hook: exit niscope  get STDdev failed!\n"); 
		return;
	}
	printf("%s: call niscope device clear!\n", pSTDdev->taskName);
	
	pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	if (pNI5122->vi) {
		niScope_Abort(pNI5122->vi);
		niScope_close(pNI5122->vi);
	}
	printf("%s: niscope device clear!\n", pSTDdev->taskName);
#if 0
	if( !pSTDdev->pList_Channel ) {
		printf("Hook: exit niscope  get pList_Channel failed!\n"); 
		return;
		}
	pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
	while(pSTDch) {
		clear_niscope_channel(pSTDch);
		
		pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
	}
#endif

	// 5. releases tree object
#if USE_SEGMENTED_ARCHIVING	
	eReturn = sfwTreeDelete (&pNI5122->pSfwTree);
#endif

	return;
}

void clear_niscope_channel(void *pArg, double arg1, double arg2)
{
	ST_STD_channel *pSTDch;
	pSTDch = (ST_STD_channel *)pArg;
	

	ST_NISCOPE_ch *pNI5122_ch = NULL;
	pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;
	
	if( pNI5122_ch->f64WaveformPtr )
		free( pNI5122_ch->f64WaveformPtr );
#if USE_BINARY_WFM
	if( pNI5122_ch->binaryWfmPtr )
		free( pNI5122_ch->binaryWfmPtr );
#endif
	if( pNI5122_ch->wfmInfoPtr )
		free( pNI5122_ch->wfmInfoPtr );
	

	printf("%s: niscope channel clear!\n", pSTDch->chName);


	return;
}


ST_NISCOPE_ch* create_ni5122_channel()
{
	ST_NISCOPE_ch *pNI5122_ch = NULL;
	pNI5122_ch = (ST_NISCOPE_ch*) malloc(sizeof(ST_NISCOPE_ch));
	if(!pNI5122_ch) return pNI5122_ch;

/********* NI scope **************/
	pNI5122_ch->myStatusCh = 0x0;


	pNI5122_ch->wfmInfoPtr = NULL;
	pNI5122_ch->f64WaveformPtr = NULL;
	pNI5122_ch->binaryWfmPtr = NULL;
	pNI5122_ch->f64Timeout = 0.0;
	
	pNI5122_ch->f64VRange = INIT_SCOPE_V_RANGE;
	pNI5122_ch->f64VOffset = INIT_SCOPE_OFFSET;
	pNI5122_ch->n32VCoupling = NISCOPE_VAL_DC;
	pNI5122_ch->f64probeAttenuation = INIT_SCOPE_PROBE_ATTENU;
	pNI5122_ch->f64InputImpedance = INIT_SCOPE_IMPEDANCE_50;
	pNI5122_ch->f64maxInputFrequency = INIT_SCOPE_MAX_FREQ;


	pNI5122_ch->filterType = NISCOPE_FILTER_LOWPASS;
	pNI5122_ch->cutoffFrequency = INIT_SCOPE_FILTER_CUTOFF;
	pNI5122_ch->centerFrequency = INIT_SCOPE_FILTER_CENTER;
	pNI5122_ch->bandpassWidth = INIT_SCOPE_FILTER_BNDWIDTH;
	
	return pNI5122_ch;
}

LOCAL long drvNIscope_io_report(int level)
{

	
	return 0;
}

void drvNIscope_status_reset( ST_STD_device *pSTDdev)
{	
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	if( !pNI5122) {
		printf(" can't get pNI5122 ptr,  %s\n", pSTDdev->taskName);
		return;
	}
	pNI5122->myStatusDev = 0x0;
	set_STDdev_status_reset(pSTDdev);
	
	return ;
}


int niScope_error(ST_STD_device *pSTDdev, ViStatus error)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	if(error != VI_SUCCESS) {
		ViChar errMsg[MAX_ERROR_DESCRIPTION] = " ";
		ViChar errorSource[MAX_FUNCTION_NAME_SIZE];
		niScope_errorHandler(pNI5122->vi, error, errorSource, errMsg);
		epicsPrintf("\nError %x: %s\n", (unsigned int)error, errMsg);
		return WR_ERROR;
	}
	pSTDdev->ErrorDev = 0;
	return WR_OK;
}

int drvNIscope_check_DataPutFinish()
{
	int cnt, cnt2;

	ST_STD_device *pSTDdev = NULL;
	ST_MASTER *pAdminCfg = get_master();

	cnt = cnt2 =0;
		
	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev) {
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) )
		{
			cnt++;
			if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) cnt2++;
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	if( cnt == cnt2 ) return WR_OK;

	return WR_ERROR;
}

/* NI scope   *************************************************************************/
int drvNIscope_auto_setup(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ViStatus error = niScope_AutoSetup(pNI5122->vi);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_get_RecordLength(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ViStatus error = niScope_ActualRecordLength(pNI5122->vi, &(pNI5122->n32ActualRecordLength) );
	return niScope_error(pSTDdev, error);
}
int drvNIscope_get_SampleRate(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViStatus error = niScope_SampleRate(pNI5122->vi, &(pNI5122->f64ActualSampleRate) );
	return niScope_error(pSTDdev, error);
}
int drvNIscope_get_NumOfWfm(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_ActualNumWfms(pNI5122->vi, pSTDch->chName, &(pNI5122_ch->n32ActualNumWaveform) );
	return niScope_error(pSTDdev, error);
}

int drvNIscope_get_readData(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	epicsPrintf("ch name: %s, f64Timeout:%f, n32ActualRecordLength:%lu\n", pSTDch->chName,
		pNI5122_ch->f64Timeout, 
		pNI5122->n32ActualRecordLength);

	ViStatus error = niScope_Read(pNI5122->vi, pSTDch->chName,
			pNI5122_ch->f64Timeout, pNI5122->n32ActualRecordLength, pNI5122_ch->f64WaveformPtr,	pNI5122_ch->wfmInfoPtr);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_v_Cfg_board(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_ConfigureVertical(pNI5122->vi, pSTDch->chName, 
			pNI5122_ch->f64VRange, pNI5122_ch->f64VOffset, pNI5122_ch->n32VCoupling, pNI5122_ch->f64probeAttenuation,
			NISCOPE_VAL_TRUE);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_v_Cfg_ChanCharacteristics(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_ConfigureChanCharacteristics(pNI5122->vi, pSTDch->chName, 
			pNI5122_ch->f64InputImpedance, pNI5122_ch->f64maxInputFrequency);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_h_Cfg_board(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViStatus error = niScope_ConfigureHorizontalTiming(pNI5122->vi,
			pNI5122->f64minSampleRate, pNI5122->n32minRecordLength, pNI5122->f64refPosition, pNI5122->n32numRecords,
			pNI5122->b8enforceRealtime);
	return niScope_error(pSTDdev, error);
}


int drvNIscope_set_trigImmediate(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViStatus error = niScope_ConfigureTriggerImmediate(pNI5122->vi);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_start_acquisition(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViStatus error = niScope_InitiateAcquisition(pNI5122->vi);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_abort_acquition(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViStatus error = niScope_Abort(pNI5122->vi);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_get_AcquisitionStatus(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViInt32 niStatus;
	ViStatus error = niScope_AcquisitionStatus(pNI5122->vi, &niStatus);
	niScope_error(pSTDdev, error);
/*	switch(niStatus){
		case NISCOPE_VAL_ACQ_COMPLETE :
			epicsPrintf("%s: ACQ Complete(%d)\n", pSTDdev->taskName, niStatus);
			break;
		case NISCOPE_VAL_ACQ_IN_PROGRESS  :
			epicsPrintf("%s: ACQ IN Progress(%d)\n", pSTDdev->taskName, niStatus);
			break;
		case NISCOPE_VAL_ACQ_STATUS_UNKNOWN:
			epicsPrintf("%s: ACQ Status Unknown(%d)\n", pSTDdev->taskName, niStatus);
			break;
		default:
			epicsPrintf("ERROR(%s): return with %d\n", pSTDdev->taskName, niStatus);
			break;
	}
*/
	return niStatus;
}

int drvNIscope_get_fetchData(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_Fetch (pNI5122->vi, pSTDch->chName, 
			pNI5122_ch->f64Timeout, pNI5122->n32ActualRecordLength,  pNI5122_ch->f64WaveformPtr, pNI5122_ch->wfmInfoPtr);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_get_fetchBinary(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_FetchBinary16(pNI5122->vi, pSTDch->chName, 
			pNI5122_ch->f64Timeout, pNI5122->n32ActualRecordLength,  (ViInt16*)pNI5122_ch->binaryWfmPtr, pNI5122_ch->wfmInfoPtr);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_cfg_triggerEdge(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	ViStatus error = niScope_ConfigureTriggerEdge(pNI5122->vi, 
		pNI5122->triggerSource, 
		INIT_SCOPE_TRIG_LEVEL, 
		NISCOPE_VAL_POSITIVE, /* slope */
		NISCOPE_VAL_DC,  /* coupling */
		INIT_SCOPE_TRIG_HOLDOFF,
		INIT_SCOPE_TRIG_DELAY );	
	return niScope_error(pSTDdev, error);	
}

int drvNIscope_set_filterType(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_SetAttributeViInt32(pNI5122->vi, pSTDch->chName, 
			NISCOPE_ATTR_MEAS_FILTER_TYPE,  pNI5122_ch->filterType);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_set_filterCutoffFreq(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_SetAttributeViReal64(pNI5122->vi, pSTDch->chName, 
			NISCOPE_ATTR_MEAS_FILTER_CUTOFF_FREQ,  pNI5122_ch->cutoffFrequency );
	return niScope_error(pSTDdev, error);
}
int drvNIscope_set_filterCenterFreq(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_SetAttributeViReal64(pNI5122->vi, pSTDch->chName, 
			NISCOPE_ATTR_MEAS_FILTER_CENTER_FREQ,  pNI5122_ch->centerFrequency);
	return niScope_error(pSTDdev, error);
}
int drvNIscope_set_filterBandWidth(ST_STD_device *pSTDdev, ST_STD_channel *pSTDch)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	ViStatus error = niScope_SetAttributeViReal64(pNI5122->vi, pSTDch->chName, 
			NISCOPE_ATTR_MEAS_FILTER_WIDTH,  pNI5122_ch->bandpassWidth);
	return niScope_error(pSTDdev, error);
}

int drvNIscope_set_multi_trigger(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE )
		pNI5122->useMultiTrigger = 1;
	else
		pNI5122->useMultiTrigger = 0;
	
	return WR_OK;
}
int drvNIscope_is_multi_trigger(ST_STD_device *pSTDdev)
{
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	return pNI5122->useMultiTrigger;
}

/*************************************************
	user add here
**************************************************/
int drvNIscope_get_Multi_Trig_Info(void *ptr)
{
	ST_STD_device *pSTDdev = (ST_STD_device*)ptr;
	int nval;
	char strBuf[16];

	char strT0[64], strT1[64];

	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) 
	{
		sprintf(strT0, "ER_LTU_P2_SEC%d_T0", nval);
		if( db_get(strT0, strBuf) == WR_ERROR ) {
			notify_error(1, "'%s' get error!\n", strT0);
			return WR_ERROR;
		} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[nval] );

		sprintf(strT1, "ER_LTU_P2_SEC%d_T1", nval);
		if( db_get(strT1, strBuf) == WR_ERROR ) {
			notify_error(1, "'%s' get error!\n", strT1);
			return WR_ERROR;
		} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT1[nval] );
	}
		
#if 0
	if( db_get("ER_LTU_mT0_sec0", strBuf) == WR_ERROR ) {
		notify_error(1, "'ER_LTU_mT0_sec0' get error!\n");
		return WR_ERROR;
	} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[0] );

	if( db_get("ER_LTU_mT1_sec0", strBuf) == WR_ERROR ) {
		notify_error(1, "'ER_LTU_mT1_sec0' get error!\n");
		return WR_ERROR;
	} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT1[0] );
/******************************************/
	if( db_get("ER_LTU_mT0_sec1", strBuf) == WR_ERROR ) {
		notify_error(1, "'ER_LTU_mT0_sec1' get error!\n");
		return WR_ERROR;
	} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[1] );

	if( db_get("ER_LTU_mT1_sec1", strBuf) == WR_ERROR ) {
		notify_error(1, "'ER_LTU_mT1_sec1' get error!\n");
		return WR_ERROR;
	} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT1[1] );
/******************************************/
	if( db_get("ER_LTU_mT0_sec2", strBuf) == WR_ERROR ) {
		notify_error(1, "'ER_LTU_mT0_sec2' get error!\n");
		return WR_ERROR;
	} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[2] );

	if( db_get("ER_LTU_mT1_sec2", strBuf) == WR_ERROR ) {
		notify_error(1, "'ER_LTU_mT1_sec2' get error!\n");
		return WR_ERROR;
	} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT1[2] );
#endif
#if 0	
	for( nval=0; nval<SIZE_CNT_MULTI_TRIG; nval++) {
		printf("Section%d :  %lf -> %lf\n", nval, pSTDdev->ST_Base.dT0[nval], pSTDdev->ST_Base.dT1[nval] );
	}
#endif
	return WR_OK;
}

/*************************************************
	End of user add
**************************************************/

void func_ni5122_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	sfwTreeErr_e	eReturn;
	unsigned short opmode;
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	opmode = pSTDdev->ST_Base.opMode;
	if( (int)arg1 == 1 ) /* in case of arming  */
	{
//		if( admin_check_Arming_condition()  == WR_ERROR) 
//			return;
//		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
//			return;

//		if( drvNIscope_is_multi_trigger(pSTDdev) )  2012. 10. 24 always get time information.
//		{
			drvNIscope_get_Multi_Trig_Info( pSTDdev );
//		}

#if USE_SEGMENTED_ARCHIVING
		eReturn = sfwTreeSetSamplingRate (pNI5122->pSfwTree, (epicsUInt32)pNI5122->f64ActualSampleRate);
// 3-1. setup shot information for archiving : at arming
//	eReturn = sfwTreeSetShotInfo (pSfwTree, TEST_SHOT_NUM, TEST_TREE_NAME, TEST_MDS_ADDR, TEST_EVENT_NAME, TEST_EVENT_ADDR);
		eReturn = sfwTreeSetShotInfo (pNI5122->pSfwTree,
								pSTDdev->ST_Base.shotNumber, 
								pSTDdev->ST_mds.treeName[opmode], 
								pSTDdev->ST_mds.treeIPport[opmode],
								pSTDdev->ST_mds.treeEventArg, 
								pSTDdev->ST_mds.treeEventIP[opmode]);
		if (sfwTreeOk != eReturn) {
			kuDebug (kuERR, "cannot set shot information\n");
		}
		sfwTreePrintInfo (pNI5122->pSfwTree);
#endif

		
/*		flush_STDdev_to_MDSfetch( pSTDdev ); move to SFW 2013. 6. 19*/
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;
	}
	else
	{
		/* nothing to do  */
		
		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;			
	}
//	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_ni5122_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	if( (int)arg1 == 1 ) /* in case of run  */
	{
//		if( admin_check_Run_condition()  == WR_ERROR) 
//			return;
//		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
//			return;
		
		if( drvNIscope_start_acquisition(pSTDdev) == WR_ERROR) 
		{
			pSTDdev->ErrorDev |= NISCOPE_ERROR_GEN;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;
		}

		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId );

	}
	else 
	{
//		if( admin_check_Stop_condition()  == WR_ERROR) 
//			return;
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;

	
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
	}
			
//	scanIoRequest(pSTDdev->ioScanPvt_status);

}

void func_ni5122_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
/*	drvNIscope_status_reset(pSTDdev);
	scanIoRequest(pSTDdev->ioScanPvt_status);
*/

	if( pSTDdev->ST_Base.opMode  == OPMODE_REMOTE ) {

		db_put("ER_LTU_SET_FREE_RUN", "0");
		epicsThreadSleep(0.1);
		db_put("ER_LTU_SHOT_END", "1");
		epicsThreadSleep(0.1);
		db_put("ER_LTU_P0_ENABLE", "1");
		db_put("ER_LTU_P1_ENABLE", "1");
		db_put("ER_LTU_P2_ENABLE", "1");
		db_put("ER_LTU_P3_ENABLE", "1");
	} 
	else 
	{
		db_put("ER_LTU_SET_FREE_RUN", "1");
		epicsThreadSleep(0.1);
		db_put("ER_LTU_SHOT_END", "1");
		db_put("ER_LTU_P0_ENABLE", "1");
		db_put("ER_LTU_P1_ENABLE", "1");
		db_put("ER_LTU_P2_ENABLE", "1");
		db_put("ER_LTU_P3_ENABLE", "1");

	}
}









