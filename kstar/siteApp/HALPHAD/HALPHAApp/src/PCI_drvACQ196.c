#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvACQ196.h"
#include "acqHTstream.h"
#include "myMDSplus.h"


/*
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
*/



#define DTACQ_DEBUG		1
/*
static ST_MASTER          *pdrvACQ196Config = NULL;
*/

/*unsigned long long int trig_start, trig_stop, trig_interval; */

static int drvACQ196_set_defaultAcq196(ST_STD_device *pSTDdev); 



static int Is_All_Slave_DAQ_STOPED();


static long drvACQ196_io_report(int level);
static long drvACQ196_init_driver();


struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvACQ196 = {
       2,
       drvACQ196_io_report,
       drvACQ196_init_driver
};

epicsExportAddress(drvet, drvACQ196);



static int drvACQ196_set_defaultAcq196(ST_STD_device *pSTDdev)
{
	int i;
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	if( acq196_initialize(pSTDdev) == WR_ERROR ) return WR_ERROR;


	switch( pAcq196->nCardMode ) {
		case CARD_MODE_SINGLE: 			
			if( acq196_set_SingleMode( pSTDdev ) == WR_ERROR ) {
				epicsPrintf("\nERROR: acq196_set_SingleMode (%s)\n", pSTDdev->taskName );
				return WR_ERROR;
			}
			epicsPrintf(">>> %s: set standalone mode()... done\n", pSTDdev->taskName);
			break;
		case CARD_MODE_MASTER:
			if( acq196_set_MasterMode( pSTDdev ) == WR_ERROR ) {
				epicsPrintf("\nERROR: acq196_set_MasterMode (%s)\n", pSTDdev->taskName );
				return WR_ERROR;
			}
			epicsPrintf(">>> %s: set master mode()... done\n", pSTDdev->taskName);
			break;
		case CARD_MODE_SLAVE:
			if( acq196_set_SlaveMode( pSTDdev ) == WR_ERROR ) {	/* set to external clock */
				epicsPrintf("\nERROR: acq196_set_SlaveMode (%s)\n", pSTDdev->taskName );
				return WR_ERROR;
			}
			epicsPrintf(">>> %s: set slave mode()... done\n", pSTDdev->taskName);
			break;
		default:
			epicsPrintf("\nERROR: Current card mode: %s (%d)\n", pSTDdev->taskName, pAcq196->nCardMode  );
			return WR_ERROR;
		}


	for(i=0; i<96; i++) {
		pAcq196->channelOnOff[i] = 1;
		pAcq196->cTouchTag[i] = 0x0;
	}

	return WR_OK;
}

int create_acq196_taskConfig( ST_STD_device *pSTDdev)
{
	ST_ACQ196* pAcq196;

	pAcq196 = (ST_ACQ196 *)malloc(sizeof(ST_ACQ196));
	if( !pAcq196) return WR_ERROR;

	pSTDdev->pUser = pAcq196;

	pAcq196->slot = strtoul(pSTDdev->strArg0,NULL,0);
#if USE_FILE_POINTER
	pAcq196->fpRaw = NULL;
#else
	pAcq196->R_file = -1;
#endif
	
	pAcq196->nChannelMask = INIT_CH_MASK;
	pAcq196->clockSource = CLOCK_INTERNAL;
	pAcq196->gotDAQstopMsg = 0;
	pAcq196->clockDivider=1;
	pAcq196->nCardMode = CARD_MODE_SINGLE;


	pAcq196->rawDataSize = 0;
	pAcq196->needDataSize = 0;

	pAcq196->parsing_remained_cnt = 0;
	pAcq196->mdsplus_snd_cnt = 0;
	pAcq196->daqing_cnt = 0;


	/* configure for htstream.		2009. 3. 17 by woong */
//remove TG	sprintf(pAcq196->drv_bufAB[BUFFERA].id, "bufA");
//remove TG	sprintf(pAcq196->drv_bufAB[BUFFERB].id, "bufB");
//remove TG	pAcq196->drv_ops.ci.timeout = INITIAL_TIMEOUT;
	pAcq196->n32_maxdma = INIT_ACQ196_MAXDMA;


/*
	pAcq196->pchannelConfig = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pAcq196->pchannelConfig) return WR_ERROR;
	else ellInit(pAcq196->pchannelConfig);
*/
	drvACQ196_assign_pvname(pSTDdev);

	
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._SYS_ARMING = func_acq196_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_acq196_SYS_RUN;
	pSTDdev->ST_Func._OP_MODE = func_acq196_OP_MODE;
	pSTDdev->ST_Func._SHOT_NUMBER = func_acq196_SHOT_NUMBER;
//remove TG	pSTDdev->ST_Func._CREATE_LOCAL_SHOT = func_acq196_CREATE_LOCAL_SHOT;

	
	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_user_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}

	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_user_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buff_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_user_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}


	return WR_OK;
}

int drvACQ196_status_reset(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	pAcq196->boardConfigure = 0x0;

	set_STDdev_status_reset(pSTDdev);

	return WR_OK;
}


void threadFunc_user_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
/*	ST_threadCfg *pDAQST_threadConfig ; */

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}

	while(TRUE) {
/*		sem_wait( &DAQsemaphore ); */
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
#if PRINT_DAQ_DEBUG
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif
/*		pSTDdev->StatusDev &= ~TASK_DAQ_STOPED; */
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= DEV_IN_LOOP_HTSTREAM;

		scanIoRequest(pSTDdev->ioScanPvt_status);
// remove TG		acq196_htstream(pSTDdev);
	}
}

void threadFunc_user_RingBuf(void *param)
{
	int i, ellCnt;
/*	int show_info = 0; */

	ST_STD_device *pSTDdev = (ST_STD_device*) param;
//	ST_threadCfg *pST_BufThread;

	ST_User_Buff_node queueData;
	ST_buf_node *pbufferNode;	
	ST_ACQ196 *pAcq196;

//	while( !pSTDdev->pST_BuffThread) epicsThreadSleep(.1);
	
//	pST_BufThread = (ST_threadCfg*) pSTDdev->pST_BuffThread;
	pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

#if PRINT_DAQ_DEBUG
	epicsPrintf("\n %s: pSTDdev: %p, pAcq196: %p , pST_BuffThread: %p\n",pSTDdev->taskName, pSTDdev, pAcq196, pSTDdev->ST_RingBufThread );
#endif
/*	epicsPrintf("\n >>> %s has threadQueueId: %d\n", pST_BufThread->threadName, pSTDdev->ST_RingBufThread.threadQueueId );
*/

	for(i = 0; i< MAX_RING_BUF_NUM; i++)
	{
		ST_buf_node *pbufferNode1 = NULL;
		pbufferNode1 = (ST_buf_node*) malloc(sizeof(ST_buf_node));
		if(!pbufferNode1) {
			epicsPrintf("\n>>> threadFunc_user_RingBuf: malloc(sizeof(ST_buf_node))... fail\n");
			return;
		}
		ellAdd(pSTDdev->pList_BufferNode, &pbufferNode1->node);
	}
	epicsPrintf(" %s: create %d node (size:%dKB)\n", pSTDdev->ST_RingBufThread.threadName, 
																		MAX_RING_BUF_NUM,
																		sizeof(ST_buf_node)/1024 );
/*
	epicsPrintf("task launching: %s thread for %s task\n",pringThreadConfig->threadName, pSTDdev->taskName);
*/
#if 0
	epicsPrintf(">>> %s, ellCnt:%d\n", pringThreadConfig->threadName, 
												ellCount(pAcq196->pdrvBufferConfig->pbufferList));
#endif
	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));
	
		if( queueData.opcode & QUEUE_OPCODE_CLOSE_FILE) {
#if PRINT_DMA_NUM			
			printf(">>> [3]%s: got msg \"%s\" close\n", pSTDdev->taskName, pAcq196->dataFileName);
#endif
#if USE_FILE_POINTER
			if( pAcq196->fpRaw != NULL )
				fclose(pAcq196->fpRaw);
			pAcq196->fpRaw = NULL;
#else
			close(pAcq196->R_file);
			pAcq196->R_file = -1;
#endif
			
#if PRINT_DMA_NUM			
			printf(">>> [4]%s: DAQ stopted!(0x%X) \n", pSTDdev->taskName, pSTDdev->StatusDev);
#endif
/*			trig_stop = wf_getCurrentUsec();
			trig_interval = wf_intervalUSec(trig_start, trig_stop); */
#if PRINT_DMA_NUM
/*			printf(">>> [5]%s: Trig ON time is %lf sec\n", pSTDdev->taskName, (1.E-3 * (double)trig_interval)/1000.0 ); */
#endif

			/* it will have possible problem when Mater mode with internal clock */
			if( (pAcq196->nCardMode != CARD_MODE_MASTER) && (pAcq196->clockSource == CLOCK_INTERNAL) )  /* this is single op mode */
			{
				acq196_setDIO_falling(pSTDdev);				
			}
			
			pSTDdev->StatusDev &= ~DEV_CMD_DAQ_STOPED;

		} /* when opcode is file close command */
		else {
			pbufferNode = queueData.pNode;

			if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) 
			{	
				pSTDdev->StatusDev |= TASK_IN_PROGRESS;
				pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				scanIoRequest(pSTDdev->ioScanPvt_status);
				notify_refresh_master_status();
			}

#if USE_FILE_POINTER
			fwrite( pbufferNode->data, queueData.size, 1, pAcq196->fpRaw );
#else			
			write(pAcq196->R_file, pbufferNode->data, queueData.size);
#endif

			ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
			ellCnt = ellCount(pSTDdev->pList_BufferNode);
#if 0
			if( pSTDdev->ST_Base.nSamplingRate >= 200000) {
				if( (queueData.u32Cnt % 8 )==0 )
					show_info = 1;
			} else if( pSTDdev->ST_Base.nSamplingRate >= 100000) {
				if( (queueData.u32Cnt % 5 )==0 )
					show_info = 1;
			} else if( pSTDdev->ST_Base.nSamplingRate >= 50000) {
				if( (queueData.u32Cnt % 3 )==0 )
					show_info = 1;
			} else {
				show_info = 1;
			}
			if( show_info){
/*				printf("*%s, %dMB, %d, rcv:%lu\n", pringThreadConfig->threadName, queueData.size/1024/1024, ellCnt, queueData.u32Cnt );  */
				pAcq196->daqing_cnt++;
				scanIoRequest(pSTDdev->ioScanPvt_userCall);
				show_info = 0;
			}
#endif
			pAcq196->daqing_cnt++;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);


			if( ellCnt < MAX_CUTOFF_BUF_NODE) {
				printf("!!! %s, Danger! DMA must be Stopped !! >> Ell Counter: %d\n", 
																pSTDdev->ST_RingBufThread.threadName, 
																ellCnt);
/*				pSTDdev->StatusDev &= ~STATUS_SYS_RUN;
				drvDPIO2_set_DMAstop ( pAcq196);
*/
			}

		} /* whenever get a Data node  */

	} /* while(TRUE)  */

	return;
}

void threadFunc_user_CatchEnd(void *param)
{
	ST_ACQ196 *pAcq196;
	ST_MASTER *pMaster;
	ST_STD_device *pSTDdev = (ST_STD_device*) param;

	while (!pSTDdev)  {
		printf("ERROR! threadFunc_user_CatchEnd() has null pointer of STD device.\n");
		epicsThreadSleep(.1);
	}
	pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	pMaster = get_master();
	
	while(TRUE) 
	{
		epicsEventWait( pSTDdev->ST_CatchEndThread.threadEventId);
		printf("%s: got End event! it will sleep %fsec. \n", pSTDdev->taskName, pSTDdev->ST_Base_fetch.dT1[0] + 0.1);
		epicsThreadSleep( pSTDdev->ST_Base_fetch.dT1[0] + 0.1 );
		printf(">>> %s: Caught the T1 stop trigger!\n", pSTDdev->taskName);


		DBproc_put( pAcq196->pvName_DEV_RUN, "0" );
		epicsThreadSleep( 1.0 );
/*
		if( !drvACQ196_RUN_stop(pSTDdev) )
			continue;
*/
		drvACQ196_wait_RUN_stop_complete(pSTDdev);

		DBproc_put( pAcq196->pvName_DEV_ARMING, "0" );
		epicsThreadSleep( 1.0 );


		pMaster->StatusAdmin &= ~TASK_SYSTEM_IDLE;
		pMaster->StatusAdmin |= TASK_POST_PROCESS;
		scanIoRequest(pMaster->ioScanPvt_status);
		if( drvACQ196_set_data_parsing( pSTDdev ) == WR_ERROR )
			continue;

		notify_refresh_master_status();
		
		if( pMaster->cUseAutoSave )
		{

			pMaster->StatusAdmin |= TASK_DATA_TRANSFER;
			scanIoRequest(pMaster->ioScanPvt_status);

			if( admin_spinLock_mds_put_flag(pSTDdev) == WR_OK ) 
			{ 
				printf("\"%s\" start data tranfer to MDS tree.\n", pSTDdev->taskName );
				if( drvACQ196_set_send_data( pSTDdev) == WR_ERROR ) {
					printf("\"%s\" data send error!\n ", pSTDdev->taskName );
					pMaster->n8MdsPutFlag  = 0;
					continue;
				}
				printf("\"%s\" release mds put flag ", pSTDdev->taskName );
				/*epicsMutexLock(pST_Master->lock_mds);*/
				pMaster->n8MdsPutFlag  = 0; /* ok. I'm finished. release write process. */				
				/*epicsMutexUnlock(pST_Master->lock_mds);*/
				printf("... OK.\n");
				
			}
/*			notify_refresh_master_status(); */
/*
			epicsMutexLock(pST_Master->lock_mds);
			drvACQ196_set_send_data( pSTDdev);
			epicsMutexUnlock(pST_Master->lock_mds);
*/
			if( drvACQ196_check_AllStorageFinished() == WR_OK ) 
			{
				printf("\n######################################(%s)\n", pSTDdev->taskName);
				printf("HALPHA: Ready to Next Shot(%s)\n", pSTDdev->taskName);
#if 1				
				if( mds_notify_EndOfTreePut(pSTDdev) == WR_ERROR ) /* it will be good for Remote mode.		 2009-11-20 */	
				{
					printlog("%s, MDS event send error!.\n", pSTDdev->taskName);
				}
#endif
/*				wf_print_date();		*//* 2009-11-24 */
				printlog("---- \n");
				pMaster->StatusAdmin |= TASK_SYSTEM_IDLE;
				notify_refresh_master_status();
				printf("######################################(%s)\n", pSTDdev->taskName);
				DBproc_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
				DBproc_put(pMaster->strPvNames[NUM_SYS_ARMING], "0");
			}
		
		}	/* eof auto data put function */
		
		/* back to waiting for next shot start */
		
	}/* eof  While(TRUE) */

}

static long drvACQ196_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_ACQ196 *pAcq196 = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( create_acq196_taskConfig(pSTDdev) == WR_ERROR )
			return -1;
			
		pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
		drvACQ196_set_defaultAcq196(pSTDdev);

		
		switch(pSTDdev->BoardID){
			case 100: 
				sprintf(pAcq196->dataFileName, "%s/acq196.%d", STR_DEV1_RAW_DIR, pAcq196->slot);
	/*			sprintf(pAcq196->dataFileName, "/media/data1/acq196.%d", slot); */
				break;
			case 200:
				sprintf(pAcq196->dataFileName, "%s/acq196.%d", STR_DEV2_RAW_DIR, pAcq196->slot);
	/*			sprintf(pAcq196->dataFileName, "/media/data2/acq196.%d", slot); */
				break;
			case 300:
				sprintf(pAcq196->dataFileName, "%s/acq196.%d", STR_DEV3_RAW_DIR, pAcq196->slot);
	/*			sprintf(pAcq196->dataFileName, "/media/data3/acq196.%d", slot); */
				break;
			case 400:
				sprintf(pAcq196->dataFileName, "%s/acq196.%d", STR_DEV4_RAW_DIR, pAcq196->slot);
	/*			sprintf(pAcq196->dataFileName, "/root/data4/acq196.%d", slot); */
				break;
			default: break;
		}

		drvACQ196_status_reset(pSTDdev); /* add new function by woong  2009.7.15 */

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pAcq196) { */
	
/*	
	if( make_Admin_CA() == WR_ERROR )
		epicsPrintf("\n>>> drvACQ196_init_driver(): make_Admin_CA() ... ERROR! \n");
*/
/*	epicsAtExit((EPICSEXITFUNC) clearAllacq196Tasks, NULL); */

	printf("Local device init OK!\n");
/*	printlog("IOC:HALPHA start ++++++++++++++++++++++++++++++++++++"); */

	return 0;
}


static long drvACQ196_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pAcq196;
	ST_MASTER *pMaster = get_master();

	if(!pMaster) return 0;

	if(ellCount(pMaster->pList_DeviceTask))
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pMaster->pList_DeviceTask));

	if(level<1) return 0;

	while(pSTDdev) {
		pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, 
			    (unsigned int)pSTDdev, 
			    pSTDdev->StatusDev );
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
/*		
			epicsPrintf("   callback time: %fusec\n", pAcq196->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pAcq196->adjCount_smplRate,
					                                                 pAcq196->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pAcq196->adjCount_Gain,
					                                                   pAcq196->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pAcq196->pchannelConfig)>0) print_channelConfig(pAcq196,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

void drvACQ196_set_chanGain(ST_ACQ196 *pAcq196, int channel, double gain)
{

	return;
}

void drvACQ196_set_samplingRate(ST_STD_device *pSTDdev, int samplingRate)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
		

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("\n>>> drvACQ196_set_samplingRate : ADC must be stop \n");
	} else {
		pSTDdev->ST_Base.nSamplingRate = samplingRate; /* This is need for mdsplus time tag, unit: Hz */
/*		if( drvACQ196_check_AllStorageFinished() == WR_OK  ) {
			pSTDdev->mds_nSamplingRate = pSTDdev->nSamplingRate;
		} 
		else {
			epicsPrintf("ERROR! %s: current mds sample rate is %dHz \n", pSTDdev->taskName, pSTDdev->mds_nSamplingRate);
		}
*/
		if( pAcq196->clockSource == CLOCK_INTERNAL ) {
			acq196_set_internalClk_DO0(pSTDdev);
		}

		pAcq196->boardConfigure |= ACQ196_SET_CLOCK_RATE;
	}	
		
	
	return;
}

int drvACQ196_ARM_enable(ST_STD_device *pSTDdev)
{

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		printf("\n>>> drvACQ196_ARM_enable : ADC already run \n");
		return WR_ERROR;
	}

	acq196_set_ABORT(pSTDdev);
	epicsThreadSleep(0.1);

	acq196_set_ARM(pSTDdev);
	
	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_ARM_ENABLED;

	return WR_OK;
}

/* 
"drvACQ196_ARM_disable()" never called by other function.....
don't need call this function              2009. 6. 26 by woong
will be erased in a while  it move to DAQ stop function....
*/
int drvACQ196_ARM_disable(ST_STD_device *pSTDdev)
{
#if 0
	if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
	{
		if( pSTDdev->StatusDev & DEV_CMD_DAQ_STOPED ) 
		{
/*			pSTDdev->StatusDev &= ~DEV_CMD_DAQ_STOPED; */
			/* wait untill ioctl(xxx) return. so get message DAQ STOP */
			epicsPrintf("\n>>> [6]%s: wait for DAQ Stop process done \n", pSTDdev->taskName);
			while( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) {
				epicsPrintf("."); 
				epicsThreadSleep(1.0) ;
			}
			epicsPrintf("\n"); 
		} 
		else if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) {
			epicsPrintf("\n %s: >>> drvACQ196_ARM_disable : need DAQ Stop first!! \n", pSTDdev->taskName);
			return WR_ERROR;
		}
	} else {
		epicsPrintf("\n>>> drvACQ196_ARM_disable : ADC not started \n");
		return WR_ERROR;
	}
	
	pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
	pSTDdev->StatusDev |= TASK_ARM_DISABLED;
	pSTDdev->StatusDev |= DEV_READY_TO_SAVE; /* if save PV put, then it cleared! */
	
	epicsPrintf(">>> [7]%s: ADC stopted!(0x%X) \n", pSTDdev->taskName, pSTDdev->StatusDev);
#endif

	epicsPrintf("\n>>> %s: ADC stop called \n", pSTDdev->taskName);

	return WR_OK;
}


/* 
	CAUTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  

	this function must be called slave first....
	master board be called at last time. 
	but all of master/slave set be use external clock... it don't care......  2009. 10. 17
*/
int drvACQ196_RUN_start(ST_STD_device *pSTDdev)
{
/*   move to int acq196_htstream(ST_STD_device *pSTDdev) function..... */
/*
	if( acq196_setDIO_rising(pSTDdev) == WR_ERROR ) {
		printf("\n>>> drvACQ196_RUN_start: acq196_setDIO_rising()... ERROR" );
		return WR_ERROR;
	}
*/

/*	pSTDdev->StatusDev |= DEV_IN_LOOP_HTSTREAM;  move to DAQ function */
	/*	sem_post( &DAQsemaphore );	*/
	epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId);
	epicsEventSignal( pSTDdev->ST_CatchEndThread.threadEventId);

#if PRINT_DAQ_DEBUG	
	printf(" sent signal %s\n", pSTDdev->taskName );
#endif
	return WR_OK;
}



int drvACQ196_RUN_stop(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if( !pAcq196 ) {
		printf("ERROR!!!! drvACQ196_RUN_stop() get pAcq196 fail.\n");
		return WR_ERROR;
	}
	
/*
	if( !(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) ) {
		printf("\n>>> drvACQ196_RUN_stop : DAQ not started \n");
		return WR_ERROR;
	}
*/
	pAcq196->gotDAQstopMsg = 1;
	pSTDdev->StatusDev |= DEV_CMD_DAQ_STOPED;
#if PRINT_DMA_NUM
	printf("\n>>> [1]%s: write gotDAQstop = 1 \n", pSTDdev->taskName);
#endif

#if 0
	while( pSTDdev->StatusDev & DEV_CMD_DAQ_STOPED ) {
		printf("."); 
		epicsThreadSleep(1.0) ;
	}

/******************************************** 2009. 09. 01*/
/*	if( (pAcq196->nCardMode == CARD_MODE_MASTER)  && (pAcq196->clockSource == CLOCK_EXTERNAL) ){  ....use in 2009 campaign */
#if 0
	if( (pSTDdev->BoardID == 0)  && (pAcq196->clockSource == CLOCK_EXTERNAL) ){
		printf(">>> [6]%s: wait for All slaves DAQ Stop process done \n", pSTDdev->taskName);
		while( Is_All_Slave_DAQ_STOPED() == WR_ERROR ) {
			printf("."); 
			epicsThreadSleep(1.0) ;
		}
		printf(">>> [7]%s: All slaves DAQ done! \n", pSTDdev->taskName);
	}
#endif		
/******************************************** */	
/*	pSTDdev->StatusDev |= TASK_DAQ_STOPED; */
	pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;

		
	/* it takes too long time when low speed sample clock. */
	/* so call it in this function  */			
	acq196_closeMapping(pAcq196->drv_bufAB+0);
	acq196_closeMapping(pAcq196->drv_bufAB+1);

	acq196_set_ABORT(pSTDdev);
	
	pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
/*	pSTDdev->StatusDev |= TASK_ARM_DISABLED; */
	
/*	pSTDdev->StatusDev |= DEV_READY_TO_SAVE;*/  /* if save PV put, then it cleared! */
/*	pSTDdev->StatusDev |= TASK_DAQ_DONE_OK; */  /*  2009. 09. 04 don't need */

	printf(">>> %s: DAQ done! \n", pSTDdev->taskName);
#endif

	return WR_OK;
}

int drvACQ196_wait_RUN_stop_complete(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
#if PRINT_DMA_NUM
	/* wait untill ioctl(xxx) return in Buffer Thread function. so get message DAQ STOP */
	printf(">>> [2]%s: wait for DAQ Stop process done \n", pSTDdev->taskName);
#endif

	while( pSTDdev->StatusDev & DEV_CMD_DAQ_STOPED ) {
/*		printf(".");  */
		epicsThreadSleep(1.0) ;
	}
	
	/* it takes too long time when low speed sample clock. */
	/* so call it in this function  */			
	acq196_closeMapping(pAcq196->drv_bufAB+0);
	acq196_closeMapping(pAcq196->drv_bufAB+1);

	pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
	
	printf(">>> %s: DAQ done! \n", pSTDdev->taskName);

	return WR_OK;
}


static int Is_All_Slave_DAQ_STOPED()
{
	int cnt = 0;
	int brd_cnt=0;
	ST_MASTER *pMaster = get_master();
	ST_STD_device*pSTDdev = (ST_STD_device*) ellLast(pMaster->pList_DeviceTask);
	while(pSTDdev) {
		if( drvACQ196_is_Master(pSTDdev) == WR_ERROR ) 
		{
			if( !(pSTDdev->StatusDev & TASK_IN_PROGRESS) ) {
/*				printf("%s daq stoped!\n", pSTDdev->taskName ); */
				cnt++;
			}
			brd_cnt++;
/*			printf("cnt: %d, brd_cnt: %d\n", cnt, brd_cnt); */
		}
		
		pSTDdev = (ST_STD_device*) ellPrevious(&pSTDdev->node);
	}

	if( cnt == brd_cnt ) return WR_OK; /* 1*/

	return WR_ERROR; /* 0 */
}


void drvACQ196_set_channelMask(ST_STD_device *pSTDdev, int cnt)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("\n>>> drvACQ196_set_channelMask : ADC must be stop \n");
	} else {
	
		if( cnt)
/* TG  pci-Acq196 type	sprintf(strCmd, "acqcmd -b %d setChannelMask %d", pAcq196->slot, pAcq196->nChannelMask);  */
			sprintf(strCmd, "acqcmd -s %d set.channelBlockMask %d", pAcq196->slot, pAcq196->nChannelMask);
		
		pfp=popen( strCmd, "r");
		if(!pfp) {
			epicsPrintf("\n>>> drvACQ196_set_channelMask ... failed\n");
			return ;
		}
		pAcq196->nChannelMask = cnt;
		while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
			epicsPrintf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
		} pclose(pfp);
	}
}


int drvACQ196_set_cardMode(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("\n>>> drvACQ196_set_cardMode : %s: ADC must be stop \n", pSTDdev->taskName);
		return WR_ERROR;
	} else {
		if( pAcq196->nCardMode == CARD_MODE_SINGLE ) {
			acq196_set_SingleMode(pSTDdev );

		} else if( pAcq196->nCardMode == CARD_MODE_MASTER ) {
			acq196_set_MasterMode(pSTDdev);

		} else if( pAcq196->nCardMode == CARD_MODE_SLAVE) {
			acq196_set_SlaveMode(pSTDdev);
		} else {
			epicsPrintf("\nERROR: drvACQ196_set_cardMode : %s: card op mode is not pre-defined!(%d) \n", pSTDdev->taskName, pAcq196->nCardMode);
			return WR_ERROR;
		}		
	}
	return WR_OK;
}


int drvACQ196_openFile(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
#if USE_FILE_POINTER
	pAcq196->fpRaw = fopen(pAcq196->dataFileName, "wb");
	if (pAcq196->fpRaw == NULL){
		printf("WR_ERROR: failed to open file %s\n",
      			       pAcq196->dataFileName);		
		perror("");
		return WR_ERROR;
	} 
#else
	pAcq196->R_file = open(pAcq196->dataFileName, O_WRONLY|O_CREAT);
	if (pAcq196->R_file < 0){
		printf("WR_ERROR: failed to open file %s\n",
      			       pAcq196->dataFileName);		
		perror("");
		return WR_ERROR;
	} 
#endif
	
/*	epicsPrintf("file: %s\n", pAcq196->dataFileName);	 */
	pAcq196->daqing_cnt = 0;
	pAcq196->rawDataSize = 0;
	pAcq196->needDataSize = (unsigned long int)((pSTDdev->ST_Base.dT1[0] - pSTDdev->ST_Base.dT0[0])*(float)pSTDdev->ST_Base.nSamplingRate*192.0); /* unit: byte */


	return WR_OK;	
}

int drvACQ196_is_Master(ST_STD_device *pSTDdev)
{
	if( strcmp(pSTDdev->taskName, MASTER_DEV_NAME) != 0)
		return WR_ERROR;
		
	return WR_OK;
}

int drvACQ196_notify_InitCondition(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if( (pAcq196->boardConfigure & ACQ196_SET_CLOCK_RATE) &&
		(pAcq196->boardConfigure & ACQ196_SET_R_BITMASK) &&
		(pAcq196->boardConfigure & ACQ196_SET_M_BITMASK) &&
		(pAcq196->boardConfigure & ACQ196_SET_L_BITMASK) &&
		(pAcq196->boardConfigure & ACQ196_SET_T0) &&
		(pAcq196->boardConfigure & ACQ196_SET_T1) &&
		(pAcq196->boardConfigure & ACQ196_SET_TAG) )
/*		(pAcq196->boardConfigure & ACQ196_SET_SHOT_NUM) )  */
	{
		if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE )
			pSTDdev->StatusDev |= TASK_STANDBY;
		else {
			if( pAcq196->boardConfigure & ACQ196_SET_SHOT_NUM) {
				pSTDdev->StatusDev |= TASK_STANDBY;
/*				printf(" %s: ready, %x\n", pSTDdev->taskName, pSTDdev->StatusDev ); */
			} else {
				pSTDdev->StatusDev &= ~TASK_STANDBY;
/*				printf(" %s: not ready, %x\n", pSTDdev->taskName, pSTDdev->StatusDev ); */
				}
		}
	} else{
		pSTDdev->StatusDev &= ~TASK_STANDBY;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);

/*	notify_sys_ready(); */
	notify_refresh_master_status();
	
	return WR_OK;
}

int drvACQ196_check_AllReadyToSave()
{
	int cnt_done = 0;
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) {
		if(pSTDdev->StatusDev & DEV_READY_TO_SAVE ) 
			cnt_done++;
		
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	if( cnt_done == pMaster->s32DeviceNum )
		return WR_OK;

	return WR_ERROR;
}

int drvACQ196_check_AllStorageFinished()
{
	int cnt_done = 0;
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) {
		if(pSTDdev->StatusDev & DEV_STORAGE_FINISH ) 
			cnt_done++;
		
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	if( cnt_done == pMaster->s32DeviceNum  )
		return WR_OK;
	
	return WR_ERROR;
}

int drvACQ196_set_data_parsing(ST_STD_device *pSTDdev)
{
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_POST_PROCESS;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	printf("%s: start data parsing!\n", pSTDdev->taskName );
	if( !proc_dataChannelization( pSTDdev ) )
	{
		epicsPrintf("\n>>> drvACQ196_set_data_parsing(%s) : proc_dataChannelization() ... fail\n", pSTDdev->taskName);
		pSTDdev->StatusDev &= ~TASK_POST_PROCESS;
		pSTDdev->StatusDev |= DEV_CHANNELIZATION_FAIL;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		pMaster->ErrorAdmin |= ERROR_AFTER_SHOT_PROCESS;
		scanIoRequest(pMaster->ioScanPvt_status);
		return WR_ERROR;
	}
	pSTDdev->StatusDev &= ~TASK_POST_PROCESS;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;

	scanIoRequest(pSTDdev->ioScanPvt_status);

	pSTDdev->StatusDev |= DEV_READY_TO_SAVE; /* if save PV put, then it cleared! */
	return WR_OK;
}

int drvACQ196_set_send_data(ST_STD_device *pSTDdev)
{
	unsigned long long int start, stop, _interval;
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	
	start = wf_getCurrentUsec();
	if( proc_sendData2Tree( pSTDdev ) == WR_ERROR)
	{
		epicsPrintf("\n>>> drvACQ196_set_send_data(%s) : proc_sendData2Tree() ... fail\n", pSTDdev->taskName);
		pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
		pSTDdev->StatusDev |= DEV_STORAGE_FAIL;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		pMaster->ErrorAdmin |= ERROR_DATA_PUT_STORAGE;
		scanIoRequest(pMaster->ioScanPvt_status);
		epicsPrintf("\n>>> %s: Status(0x%X) \n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR ;
	}
	stop = wf_getCurrentUsec();
	
	pSTDdev->StatusDev &= ~DEV_READY_TO_SAVE;
	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
	pSTDdev->StatusDev |= DEV_STORAGE_FINISH;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;

	pSTDdev->StatusDev |= TASK_STANDBY; /* always ready after put finished */ /* 2010.8.11 woong */
	scanIoRequest(pSTDdev->ioScanPvt_status);

	_interval = wf_intervalUSec(start, stop);
	printf("%s: Data put time: %d sec\n", pSTDdev->taskName, (int)(1.E-3 * (double)_interval)/1000 ); 

/*	drvACQ196_notify_InitCondition(pSTDdev); */   /* remove it ........ 2010.8.11 woong */
	
	return WR_OK ;
}

int drvACQ196_check_Run_condition(ST_STD_device *pSTDdev)
{
	if( !(pSTDdev->StatusDev & TASK_ARM_ENABLED) )
	{
		notify_error(1, "%s: DAQ needs arming! \n", pSTDdev->taskName);
		return WR_ERROR;
	}
	if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev & TASK_IN_PROGRESS )
	{
		notify_error(1, "%s: DAQ aleady running! \n", pSTDdev->taskName);
		return WR_ERROR;
	}

	return WR_OK;	
}

int drvACQ196_check_Stop_condition(ST_STD_device *pSTDdev)
{
/*	if( !(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) ) {
		printf("%s: DAQ not started \n", pSTDdev->taskName);
		return WR_ERROR;
	}
*/
	if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev & TASK_IN_PROGRESS ) {
		return WR_OK;
	}
	else 
		printf("%s: DAQ not started \n", pSTDdev->taskName);
	
	return WR_ERROR;
}

int drvACQ196_assign_pvname(ST_STD_device *pSTDdev)
{
	int noffset;

	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;
	
	if(!strcmp(pMaster->taskName, "HALPHA") ) noffset = 1;
	else if( !strcmp(pMaster->taskName, "SXR") ) noffset = 5;
	else return WR_ERROR;

	sprintf(pAcq196->pvName_DEV_ARMING, "%s_B%d_ARMING", pMaster->taskName, pSTDdev->BoardID+noffset);
	sprintf(pAcq196->pvName_DEV_RUN, "%s_B%d_RUN", pMaster->taskName, pSTDdev->BoardID+noffset);
	
	return WR_OK;
}

int drvACQ196_reset_cnt(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pAcq196->parsing_remained_cnt = 0;
	pAcq196->mdsplus_snd_cnt = 0;
	pAcq196->daqing_cnt = 0;
	
	return WR_OK;
}





void func_acq196_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if( (int)arg1 == 1 ) /* in case of arming  */
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		/* direct call to sub-device.. for status notify immediately */
//		while(pSTDdev) 
//		{
			if( drvACQ196_openFile(pSTDdev) == WR_ERROR) {
				printf("\n>>> %s: drvACQ196_openFile() ERROR", pSTDdev->taskName);
				return ;
			}
			pSTDdev->StatusDev &= ~DEV_STORAGE_FINISH;  /* 2009. 10. 16 : storage status clear for check end of saving signal...*/		
//			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
//		}
//		pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
//		while(pSTDdev) 
//		{
			if( drvACQ196_ARM_enable( pSTDdev ) == WR_ERROR ) {
				epicsPrintf("\n>>> dev_BO_SYS_ARMING : drvACQ196_ARM_enable... failed\n");
				return ;
			}
			printf("\"%s\" armed.\n", pSTDdev->taskName );
			epicsThreadSleep(TIME_GAP_ARMING);
			flush_STDdev_to_MDSfetch( pSTDdev );
//			scanIoRequest(pSTDdev->ioScanPvt_status);

			drvACQ196_reset_cnt(pSTDdev);
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

//			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
//		}
	}
	else /* release arming condition */
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;
		
//		while(pSTDdev) 
//		{
			ST_ACQ196 *pAcq196=NULL;
			pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
			
			if( pAcq196->fpRaw != NULL )
				fclose(pAcq196->fpRaw);
			pAcq196->fpRaw = NULL;
			
			acq196_set_ABORT(pSTDdev);
			
			printf("\"%s\" arm disabled.\n", pSTDdev->taskName );
	
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	
//			scanIoRequest(pSTDdev->ioScanPvt_status);
//			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
//		}

	}
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
		
	
}
void func_acq196_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	if( (int)arg1 == 1 ) /* command to run  */
	{
		if( check_dev_run_condition(pSTDdev)  == WR_ERROR) 
			return;

		/* direct call to sub-device.. for status notify immediately */
//		while(pSTDdev) 
//		{
			if( drvACQ196_RUN_start(pSTDdev) == WR_ERROR ) 
				return;

//			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
//		}

		epicsThreadSleep(0.3);
	}
	else 
	{
#if 0	/* do nothing... 'cause aleady done in sub devices */	
		if( check_dev_stop_condition(pSTDdev)  == WR_ERROR) 
			return;

		/* do something, if you need */
		while(pSTDdev) 
		{
			

			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
#endif
	}
	

}
void func_acq196_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	drvACQ196_status_reset(pSTDdev);
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_acq196_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	pAcq196->boardConfigure |= ACQ196_SET_SHOT_NUM;
	drvACQ196_notify_InitCondition( pSTDdev );
}

void func_acq196_CREATE_LOCAL_SHOT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if( !strcmp(pSTDdev->taskName, "B4") )
	{
		mds_Connect( pSTDdev );
		mds_createNewShot( pSTDdev );
		mds_Disconnect(pSTDdev );
	}

}




