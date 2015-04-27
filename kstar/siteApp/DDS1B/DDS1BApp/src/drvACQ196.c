#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h> /* open() */
#include <stdio.h>
#include <stdlib.h> /* itoa() */



#include "drvACQ196.h"
#include "myMDSplus.h" 
#include "acqHTstream.h" 


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



int create_ACQ196_taskConfig( ST_STD_device *pSTDdev)
{
	ST_ACQ196* pAcq196;

	pAcq196 = (ST_ACQ196 *)malloc(sizeof(ST_ACQ196));
	if( !pAcq196) return WR_ERROR;
	pSTDdev->pUser = pAcq196;


	pAcq196->slot = strtoul(pSTDdev->strArg0,NULL,0);
	sprintf(pAcq196->dataFileName, "%s/acq196.%d", pSTDdev->strArg1, pAcq196->slot);
//	pAcq196->nCardMode = strtoul(pSTDdev->strArg2,NULL,0);
	pAcq196->nCardMode = CARD_MODE_SINGLE; 

        pAcq196->fpRaw = NULL;

	pAcq196->nChannelMask = INIT_CH_MASK;
        pAcq196->clockSource = CLOCK_INTERNAL;
        pAcq196->gotDAQstopMsg = 0;
        pAcq196->clockDivider=1;
        pAcq196->rawDataSize = 0;
        pAcq196->needDataSize = 0;

        pAcq196->parsing_remained_cnt = 0;
        pAcq196->mdsplus_snd_cnt = 0;
        pAcq196->daqing_cnt = 0;

        /* configure for htstream.              2009. 3. 17 by woong */
        sprintf(pAcq196->drv_bufAB[BUFFERA].id, "bufA");
        sprintf(pAcq196->drv_bufAB[BUFFERB].id, "bufB");
        pAcq196->drv_ops.ci.timeout = INITIAL_TIMEOUT;
        pAcq196->n32_maxdma = INIT_ACQ196_MAXDMA;

//        drvACQ196_assign_pvname(pSTDdev);

	return WR_OK;
}


int init_my_sfwFunc_ACQ196( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_ACQ196_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_ACQ196_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_ACQ196_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_ACQ196_SYS_RUN;
//	pSTDdev->ST_Func._SAMPLING_RATE= func_ACQ196_SAMPLING_RATE;
	pSTDdev->ST_Func._DATA_SEND = func_ACQ196_DATA_SEND;
	pSTDdev->ST_Func._CREATE_LOCAL_TREE = func_acq196_CREATE_LOCAL_SHOT;

	pSTDdev->ST_Func._SYS_RESET = func_ACQ196_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_ACQ196_taskConfig;


	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ACQ196_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}

	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ACQ196_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buff_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ACQ196_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) {
		return WR_ERROR;
	}


	return WR_OK;

}


void clear_ACQ196_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 

	drvACQ196_save_file(pSTDdev);
}

void threadFunc_ACQ196_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
//	ST_ACQ196 *pAcq196;
//	ST_MASTER *pMaster = NULL;
	
//	pMaster = get_master();
//	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
//	pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

		

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= DEV_IN_LOOP_HTSTREAM;
		notify_refresh_master_status();

		acq196_htstream(pSTDdev);

#if 0
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif

//		db_put("ACQ196_SYS_RUN", "0");
//		epicsThreadSleep(1.0);
//		db_put("ACQ196_SYS_ARMING", "0");


//		if( pMaster->cUseAutoSave ) 
//			db_put("ACQ196_SEND_DATA", "1");
		
	}
}

void threadFunc_ACQ196_RingBuf(void *param)
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
	
		if( queueData.opcode == QUEUE_OPCODE_CLOSE_FILE) {
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
			printf("%s: DEV_CMD_DAQ_STOPED bit cleared.\n", pSTDdev->taskName);

		} /* when opcode is file close command */
		else {
			pbufferNode = queueData.pNode;

			if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) 
			{	
				pSTDdev->StatusDev |= TASK_IN_PROGRESS;
				pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				scanIoRequest(pSTDdev->ioScanPvt_status);
				notify_refresh_master_status();
				epicsEventSignal( pSTDdev->ST_CatchEndThread.threadEventId);
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

void threadFunc_ACQ196_CatchEnd(void *param)
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
		printf("%s: got End event! it will sleep %fsec. \n", pSTDdev->taskName, pSTDdev->ST_Base_fetch.dRunPeriod -2.0);
		epicsThreadSleep( pSTDdev->ST_Base_fetch.dRunPeriod -3.0 );
		printf(">>> %s: Caught the T1 stop trigger!\n", pSTDdev->taskName);

//		db_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
		pAcq196->gotDAQstopMsg = 1;
		pSTDdev->StatusDev |= DEV_CMD_DAQ_STOPED;
		epicsThreadSleep( 1.0 );
/*
		if( !drvACQ196_RUN_stop(pSTDdev) )
			continue;
*/
		drvACQ196_wait_RUN_stop_complete(pSTDdev);

//		db_put( pAcq196->pvName_DEV_ARMING, "0" );
//		epicsThreadSleep( 1.0 );


//		pMaster->StatusAdmin &= ~TASK_SYSTEM_IDLE;
//		pMaster->StatusAdmin |= TASK_POST_PROCESS;
//		scanIoRequest(pMaster->ioScanPvt_status);
		notify_refresh_master_status();
		if( drvACQ196_set_data_parsing( pSTDdev ) == WR_ERROR )
			continue;

		notify_refresh_master_status();
		scanIoRequest(pSTDdev->ioScanPvt_status);
		
//		if( pMaster->cUseAutoSave )
//		{

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
				printf("DDS1: Ready to Next Shot(%s)\n", pSTDdev->taskName);
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
//				DBproc_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
				db_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
				db_put(pMaster->strPvNames[NUM_SYS_ARMING], "0");
			}
		
//		}	/* eof auto data put function */
		
		/* back to waiting for next shot start */
		
	}/* eof  While(TRUE) */

}



static long drvACQ196_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_ACQ196 *pACQ196 = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) != 0 ) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if(create_ACQ196_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_ACQ196_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_ACQ196(pSTDdev);
		
	
		pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;

		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pACQ196) { */

	notify_refresh_master_status();

	printf("****** ACQ196 local device init OK!\n");

	return 0;
}


static long drvACQ196_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pACQ196;
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
		pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pACQ196->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pACQ196->adjCount_smplRate,
					                                                 pACQ196->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pACQ196->adjCount_Gain,
					                                                   pACQ196->adjTime_Gain_Usec);
*/
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_ACQ196_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	char buf[64];
	ST_MASTER *pMaster = get_master();

	sprintf(buf, "%s_%s:CLOCK_MODE", pMaster->taskName, pSTDdev->taskName);
//	printf("%s\n", buf);
	
	if( (int)arg1 == OPMODE_REMOTE ) {
		/* to do here */
		db_put(buf, "1");

	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		/* to do here */
		db_put(buf, "0");
		
	}
}

void func_ACQ196_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_ACQ196_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);


}

void func_ACQ196_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;
/* to do here */
		if( drvACQ196_openFile(pSTDdev) == WR_ERROR) {
				printf("\n>>> %s: drvACQ196_openFile() ERROR", pSTDdev->taskName);
				return ;
			}
		pSTDdev->StatusDev &= ~DEV_STORAGE_FINISH;  /* 2009. 10. 16 : storage status clear for check end of saving signal...*/		
//		acq196_set_ABORT(pSTDdev);
//		epicsThreadSleep(0.1);

		acq196_set_ARM(pSTDdev);
	
			printf("\"%s\" armed.\n", pSTDdev->taskName );
			epicsThreadSleep(0.1);

			drvACQ196_reset_cnt(pSTDdev);
//			scanIoRequest(pSTDdev->ioScanPvt_userCall);



/*		flush_STDdev_to_MDSfetch( pSTDdev ); move to SFW 2013. 6. 19*/
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;	
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;

		ST_ACQ196 *pAcq196=NULL;
		pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
		
		if( pAcq196->fpRaw != NULL )
			fclose(pAcq196->fpRaw);
		pAcq196->fpRaw = NULL;
		
		acq196_set_ABORT(pSTDdev);
		
		printf("\"%s\" arm disabled.\n", pSTDdev->taskName );

		

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_ACQ196_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		/* to do here */
//		if( drvACQ196_RUN_start(pSTDdev) == WR_ERROR ) 
//				return;

		epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId);
//		epicsEventSignal( pSTDdev->ST_CatchEndThread.threadEventId);
	

//		epicsThreadSleep(0.3);



		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
//		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

	}
	else 
	{
/*		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;
*/
		pACQ196->gotDAQstopMsg = 1;
		pSTDdev->StatusDev |= DEV_CMD_DAQ_STOPED;
#if PRINT_DMA_NUM
		printf("\n>>> [1]%s: write gotDAQstop = 1 \n", pSTDdev->taskName);
#endif


		
/*		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS; */
	}
/*	scanIoRequest(pSTDdev->ioScanPvt_status); */

	
}
/*
void func_ACQ196_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	if( pACQ196->clockSource == CLOCK_INTERNAL ) {
		if( acq196_set_internalClk_DO0(pSTDdev) == WR_OK )
			pACQ196->boardConfigure |= ACQ196_SET_CLOCK_RATE;
		else 
			pACQ196->boardConfigure &= ~ACQ196_SET_CLOCK_RATE;
	}

	printf("%s: sampling rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}
*/
void func_ACQ196_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your MDS related functions */



	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();
}

void func_ACQ196_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your device reset functions */
	set_STDdev_status_reset(pSTDdev);
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();

}

void func_acq196_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

//	drvACQ196_status_reset(pSTDdev);
	scanIoRequest(pSTDdev->ioScanPvt_status);
}
/*
void func_acq196_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	pAcq196->boardConfigure |= ACQ196_SET_SHOT_NUM;
	drvACQ196_notify_InitCondition( pSTDdev );
}
*/
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






/*************************************************************/




int drvACQ196_wait_RUN_stop_complete(ST_STD_device *pSTDdev)
{
	float tmp;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
#if PRINT_DMA_NUM
	/* wait untill ioctl(xxx) return in Buffer Thread function. so get message DAQ STOP */
	printf(">>> [2]%s: wait for DAQ Stop process done \n", pSTDdev->taskName);
#endif
	tmp = (float)pSTDdev->BoardID + 0.1;
	while( pSTDdev->StatusDev & DEV_CMD_DAQ_STOPED ) {
//		printf("%s: wait(%f) DEV_CMD_DAQ_STOPED bit clear.\n", pSTDdev->taskName, tmp);
		
		epicsThreadSleep(tmp) ;
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
			sprintf(strCmd, "acqcmd -b %d setChannelMask %d", pAcq196->slot, pAcq196->nChannelMask);
		
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
//	pAcq196->needDataSize = (unsigned long int)((pSTDdev->ST_Base.dT1[0] - pSTDdev->ST_Base.dT0[0])*(float)pSTDdev->ST_Base.nSamplingRate*192.0); /* unit: byte */
	pAcq196->needDataSize = (unsigned long int)(pSTDdev->ST_Base.dRunPeriod * (float)pSTDdev->ST_Base.nSamplingRate * 192.0); /* unit: byte */


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
	printf("%s: start data parsing...", pSTDdev->taskName );
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
	printf("  OK." );

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


/*
int drvACQ196_assign_pvname(ST_STD_device *pSTDdev)
{
	int noffset;

	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;
	
	if(!strcmp(pMaster->taskName, "DDS1A") ) noffset = 1;
	else if( !strcmp(pMaster->taskName, "DDS1B") ) noffset = 5;
	else return WR_ERROR;

	sprintf(pAcq196->pvName_DEV_ARMING, "%s_B%d_ARMING", pMaster->taskName, pSTDdev->BoardID+noffset);
	sprintf(pAcq196->pvName_DEV_RUN, "%s_B%d_RUN", pMaster->taskName, pSTDdev->BoardID+noffset);
	
	return WR_OK;
}
*/
int drvACQ196_reset_cnt(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pAcq196->parsing_remained_cnt = 0;
	pAcq196->mdsplus_snd_cnt = 0;
	pAcq196->daqing_cnt = 0;
	
	return WR_OK;
}


void drvACQ196_init_file( ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	int i;
	FILE *fp;
	char filename[64];
	char bufLine[64];
	int chid[96], on[96];	
	char tagname[96][60];
	char mask[4];
	char pvname[60];

	sprintf(filename, "%s/iocBoot/%s/%s_%s.cfg", pSTDdev->pST_parentAdmin->strTOP, 
		pSTDdev->pST_parentAdmin->strIOC, pSTDdev->pST_parentAdmin->taskName, 
		pSTDdev->taskName);
//	printf("%s\n", filename);

	if( (fp = fopen(filename, "r")) == NULL ) {
		printf("'%s' can't open!\n", filename);
		return;
	}

	for( i=0; i<96; i++){
		if( fgets(bufLine, 64, fp) == NULL ) { fclose(fp); return;}
//		sscanf(bufLine, "%d %s %s", &chid, mask, tagname);
		sscanf(bufLine, "%d %d %s", &chid[i], &on[i], tagname[i]);
	}
	fclose(fp);

	for(i=0; i<96; i++) {
		
		sprintf(pvname, "%s_%s_CH%d:TAG_NAME", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, i);
		db_put(pvname, tagname[i]);
//		printf("%s  %s\n", pvname, tagname[i]);
		
		sprintf(pvname, "%s_%s_CH%d:ENABLE", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, i);
//		itoa(on[i], mask, 10);
		sprintf(mask, "%d", on[i]);
		db_put(pvname, mask);
//		printf("%s  %s\n", pvname, mask);
	}

//	fclose(fp);

}

void drvACQ196_save_file( ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	int i;
	FILE *fp;
	char filename[64];

	sprintf(filename, "%s/iocBoot/%s/%s_%s.cfg", pSTDdev->pST_parentAdmin->strTOP, 
		pSTDdev->pST_parentAdmin->strIOC, pSTDdev->pST_parentAdmin->taskName, 
		pSTDdev->taskName);
//	printf("%s\n", filename);

	if( (fp = fopen(filename, "w")) == NULL ) {
		printf("'%s' can't open!\n", filename);
		return;
	}

	for( i=0; i<96; i++){
		fprintf(fp, "%d\t%d\t%s\n", i, pAcq196->channelOnOff[i], pAcq196->strTagName[i]);
	}

	fclose(fp);

}






