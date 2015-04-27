#include "sfwCommon.h"



#include "drvRTCORE.h"
#include "drvRMCHK.h"
/*#include "myMDSplus.h" */


static long drvRTCORE_io_report(int level);
static long drvRTCORE_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvRTCORE = {
       2,
       drvRTCORE_io_report,
       drvRTCORE_init_driver
};

epicsExportAddress(drvet, drvRTCORE);



int create_RTcore_taskConfig( ST_STD_device *pSTDdev)
{
	ST_RTcore* pRTcore;
	unsigned int nval=0;
	int status;

	
	pRTcore = (ST_RTcore *)malloc(sizeof(ST_RTcore));
	if( !pRTcore) return WR_ERROR;
	pSTDdev->pUser = pRTcore;

	pRTcore->fd_event = open("/dev/intLTU.0", O_RDWR|O_NDELAY);
	if (pRTcore->fd_event  < 0)
	{
		fprintf(stdout, "intLTU device open error !!!!!!\n");
		return WR_ERROR;
	}
	pRTcore->base0 = (char *)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, pRTcore->fd_event, 0);
	if( pRTcore->base0 == 0x0) {
		perror("mmap() error!");
		close( pRTcore->fd_event );
		return WR_ERROR;
	}
	status = ioctl(pRTcore->fd_event, IOCTL_CLTU_INTERRUPT_ENABLE, &nval);
	nval = READ32(pRTcore->base0 + 0x0);
	printf("0x0:   0x%x\n", (nval));

	nval = READ32(pRTcore->base0 + 0x4);
	printf("0x4:   0x%x\n", (nval));

	nval = READ32(pRTcore->base0 + 0x8);
	printf("0x8:   0x%x\n", (nval));

	nval = READ32(pRTcore->base0 + 0xc);
	printf("0xc:   0x%x\n", (nval));

     

//	pRTcore->cntDAQ = 0;
	pRTcore->cntDAQ_loop = 0;
	pRTcore->cntAccum = 0;
/*	pRTcore->daqStepTime = 0.00001; */
	pRTcore->useScanIoRequest = 0;


/*	pRTcore->clkStartTime = -15.0; */
	

	return WR_OK;
}


int init_my_sfwFunc_RTcore( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_RTcore_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_RTcore_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_RTcore_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_RTcore_SYS_RUN;
	pSTDdev->ST_Func._SAMPLING_RATE= func_RTcore_SAMPLING_RATE;

	pSTDdev->ST_Func._SYS_RESET = func_RTcore_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_RTcore_taskConfig;


	/*********************************************************/
#if 1
	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif
#if 1
	pSTDdev->ST_RTthread.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_RT;
	if(make_STD_RT_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"ST_RTthread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif

#if USE_RTCORE_DAQRING
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif


#if 0
	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}
#endif



	return WR_OK;

}

int init_DivThreads_RTcore( ST_STD_device *pSTDdev)
{
	ST_RTcore *pRTcore = NULL;

	pRTcore = (ST_RTcore *)pSTDdev->pUser;
#if USE_DIVIDER_A
	pRTcore->ST_DivThreadA.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_DivideA;
	sprintf(pRTcore->ST_DivThreadA.threadName, "%s_DivA", pSTDdev->taskName );
	pRTcore->ST_DivThreadA.threadEventId = epicsEventCreate(epicsEventEmpty);
	if ( ! pRTcore->ST_DivThreadA.threadEventId ) {
		return WR_ERROR;
	}
	epicsThreadCreate(pRTcore->ST_DivThreadA.threadName, 
			epicsThreadPriorityHigh, 
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) pRTcore->ST_DivThreadA.threadFunc,
			(void*)pRTcore);
#endif

#if USE_DIVIDER_B
	pRTcore->ST_DivThreadB.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_DivideB;
	sprintf(pRTcore->ST_DivThreadB.threadName, "%s_DivB", pSTDdev->taskName );
	pRTcore->ST_DivThreadB.threadEventId = epicsEventCreate(epicsEventEmpty);
	if ( ! pRTcore->ST_DivThreadB.threadEventId ) {
		return WR_ERROR;
	}
	epicsThreadCreate(pRTcore->ST_DivThreadB.threadName, 
			epicsThreadPriorityHigh, 
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) pRTcore->ST_DivThreadB.threadFunc,
			(void*)pRTcore);
#endif

	return WR_OK;

}


void clear_RTcore_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_RTcore *pRTcore = (ST_RTcore *)pSTDdev->pUser;
	unsigned int nval=0;
	int status;

	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 

	status = ioctl(pRTcore->fd_event, IOCTL_CLTU_INTERRUPT_DISABLE, &nval);
	printf("IntLTU interrupt disabled!\n");

	close( pRTcore->fd_event );
	printf("intLTU closed.\n");

}

volatile char rt_toggle = 1;
void threadFunc_RTcore_RT(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_RTcore *pRTcore;
//	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDRMchk = NULL;
	ST_RMCHK *pRMCHK;	

	epicsThreadId pthreadInfo;

	
	int got;
	int nval;
	int nFirst = 1;

	
//	pMaster = get_master();
//	if(!pMaster)	return;

	pRTcore = (ST_RTcore *)pSTDdev->pUser;
	if( !pRTcore ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of pRTcore.\n");
		return;
	}

	
/********************************************/
/* to get RFM, RMcheck device */
	pSTDRMchk = get_STDev_from_type(STR_DEVICE_TYPE_2);
	if( !pSTDRMchk ) {
		printf("ERROR! threadFunc_RTcore_RT() has null pointer of Target device.\n");
		return;
	} else {
		printf("target RMchk: %p, eventID:%d\n", pSTDRMchk, pSTDRMchk->ST_DAQThread.threadEventId );
	}
	pRMCHK = (ST_RMCHK *)pSTDRMchk->pUser;
	if( !pRMCHK ) {
		printf("ERROR! threadFunc_RTcore_RT() has null pointer of pRMCHK.\n");
		return;
	}
/*********************/

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RTCORE_RT);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RTCORE_RT, "SCHED_FIFO");
#endif

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_RTthread.threadEventId);
		nFirst = 1;
		rt_toggle = 1;
		printf("%s: Got epics RT Run event.\n", pSTDdev->taskName);
		
		while(TRUE) {
			got	= read(pRTcore->fd_event, &nval, 4);
#if USE_KERNEL_ACK_HIGH
			WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
#if USE_KERNEL_ACK_LOW
			WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif

			if( rt_toggle ) { /* Half clock,   10Khz, when 20Khz */
				rt_toggle = 0;
				epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId );
				epicsEventSignal( pSTDRMchk->ST_DAQThread.threadEventId );
			} 
			else rt_toggle = 1;

#if USE_DIVIDER_A
			if( (pSTDdev->stdCoreCnt % DIVIDER_A) == 0) { // value is 4, 5KHz if Mclk is 20Khz 
				epicsEventSignal( pRTcore->ST_DivThreadA.threadEventId );

			}
#endif
#if USE_DIVIDER_B
			if( (pSTDdev->stdCoreCnt % DIVIDER_B) == 0) { // value is 20, 1KHz if Mclk is 20Khz 
				epicsEventSignal( pRTcore->ST_DivThreadB.threadEventId );
			}
#endif
			pSTDdev->stdCoreCnt++; 
		/* not use scanIoRequest() */

			if( nFirst ) {
				send_master_stop_event();
				nFirst = 0;
			}

			if( !(pSTDdev->StatusDev & TASK_IN_PROGRESS ) )
			{
				pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				printf("system stop!.. bye RT thread!\n");
				break;
			}
		}
	}
}

void threadFunc_RTcore_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_RTcore *pRTcore;
	ST_MASTER *pMaster = NULL;

	ST_STD_device *pSTDRMchk = NULL;
	ST_RMCHK *pRMCHK;	
	
	epicsThreadId pthreadInfo;
	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode = NULL;

	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}

	pRTcore = (ST_RTcore *)pSTDdev->pUser;
	if( !pRTcore ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of pRTcore.\n");
		return;
	}
/* get RFM device ******************************************* */
	pSTDRMchk = get_STDev_from_type(STR_DEVICE_TYPE_2);
	if( !pSTDRMchk ) {
		printf("ERROR! threadFunc_RTcore_DAQ() has null pointer of Dev2.\n");
		return;
	} else {
		printf("target RMchk: %p, eventID:%d\n", pSTDRMchk, pSTDRMchk->ST_DAQThread.threadEventId );
	}
	pRMCHK = (ST_RMCHK *)pSTDRMchk->pUser;
	if( !pRMCHK ) {
		printf("ERROR! threadFunc_RTcore_RT() has null pointer of pRMCHK.\n");
		return;
	}
/*********************************************************** */

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
/*	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid); */
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RTCORE_DAQ);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RTCORE_DAQ, "SCHED_FIFO");
#endif

#if USE_RTCORE_DAQRING
	epicsThreadSleep(1.0);
	pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
	ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);
#endif

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);

#if USE_RTCORE_DAQ_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
#if USE_RTCORE_DAQ_LOW
		WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif

#if USE_RTCORE_DAQRING
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 0] = pRMCHK->mapBuf[RM_PCS_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 1] = pRMCHK->mapBuf[RM_PF1_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 2] = pRMCHK->mapBuf[RM_PF2_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 3] = pRMCHK->mapBuf[RM_PF3U_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 4] = pRMCHK->mapBuf[RM_PF3L_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 5] = pRMCHK->mapBuf[RM_PF4U_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 6] = pRMCHK->mapBuf[RM_PF4L_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 7] = pRMCHK->mapBuf[RM_PF5U_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 8] = pRMCHK->mapBuf[RM_PF5L_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 9] = pRMCHK->mapBuf[RM_PF6U_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 10] = pRMCHK->mapBuf[RM_PF6L_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 11] = pRMCHK->mapBuf[RM_PF7_CNT/4];
		pbufferNode->data[pRTcore->cntDAQ_loop * LOOP_ELM + 12] = pRMCHK->mapBuf[RM_IVC_CNT/4];

		
//		pRTcore->cntDAQ++;

		pRTcore->cntDAQ_loop++;
		pRTcore->cntAccum += LOOP_ELM;


		if( pRTcore->cntDAQ_loop >= LOOP_CNT ) {
			queueData.pNode		= pbufferNode;
			queueData.timeStamp	= 0x0;
			epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

			pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
			ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);

			pRTcore->cntDAQ_loop = 0;

		}
#endif	
		pSTDdev->stdDAQCnt++;

		if( pRTcore->useScanIoRequest )
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
	}
}

void threadFunc_RTcore_RingBuf(void *param)
{
	int i, ellCnt;
	epicsThreadId pthreadInfo;

	ST_STD_device *pSTDdev = (ST_STD_device*) param;

	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode;	
	ST_RTcore *pRTcore;


	pRTcore = (ST_RTcore *)pSTDdev->pUser;

#if 0
	epicsPrintf("\n %s: pSTDdev: %p, p16aiss8ao4: %p , pST_BuffThread: %p\n",pSTDdev->taskName, pSTDdev, p16aiss8ao4, pST_BufThread );
#endif

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
#if 0
	epicsPrintf(">>> %s, ellCnt:%d\n", pringThreadConfig->threadName, 
												ellCount(p16aiss8ao4->pdrvBufferConfig->pbufferList));
#endif

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
/*	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid); */
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RTCORE_RING);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RTCORE_RING, "SCHED_FIFO");
#endif

	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

		pbufferNode = queueData.pNode;


/*		fwrite( pbufferNode->data,  sizeof(unsigned int), LOOP_CNT*LOOP_ELM, pRTcore->fp_raw); */
		
		
		ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
		ellCnt = ellCount(pSTDdev->pList_BufferNode);
/*		printf("RTcore_RingBuf, ellCnt: %d\n", ellCnt); */


	} /* while(TRUE)  */

	return;
}

void threadFunc_RTcore_DivideA(void *param)
{
	ST_RTcore *pRTcore = (ST_RTcore*) param;;
	epicsThreadId pthreadInfo;


#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
/*	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid); */
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RTCORE_DivA);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RTCORE_DivA, "SCHED_FIFO");
#endif

	epicsThreadSleep(1.0);

	while(TRUE) {
		epicsEventWait( pRTcore->ST_DivThreadA.threadEventId);

#if USE_RTCORE_DivA_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
#if USE_RTCORE_DivA_LOW
		WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif

	}
}

void threadFunc_RTcore_DivideB(void *param)
{
	ST_RTcore *pRTcore = (ST_RTcore*) param;;
	epicsThreadId pthreadInfo;

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
/*	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid); */
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RTCORE_DivB);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RTCORE_DivB, "SCHED_FIFO");
#endif

	epicsThreadSleep(1.0);


	while(TRUE) {
		epicsEventWait( pRTcore->ST_DivThreadB.threadEventId);

#if USE_RTCORE_DivB_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
#if USE_RTCORE_DivB_LOW
		WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif

	}
}


static long drvRTCORE_init_driver(void)
{
	ST_MASTER *pMaster = NULL;	
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

		if(create_RTcore_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_RTcore_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_RTcore(pSTDdev);
		
	
		init_DivThreads_RTcore( pSTDdev);

		


		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pRTcore) { */

	notify_refresh_master_status();

	printf("****** RTcore local device init OK!\n");

	return 0;
}


static long drvRTCORE_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_RTcore *pRTcore;
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
		pRTcore = (ST_RTcore *)pSTDdev->pUser;
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
/*		
			epicsPrintf("   callback time: %fusec\n", pRTcore->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pRTcore->adjCount_smplRate,
					                                                 pRTcore->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pRTcore->adjCount_Gain,
					                                                   pRTcore->adjTime_Gain_Usec);
*/
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_RTcore_OP_MODE(void *pArg, double arg1, double arg2)
{
	if( (int)arg1 == OPMODE_REMOTE ) {
		/* to do here */

	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		/* to do here */

	}
}

void func_RTcore_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_RTcore_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

	


}

void func_RTcore_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_RTcore *pRTcore = (ST_RTcore *)pSTDdev->pUser;
	char fileName[40];

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;
/* to do here */


		pSTDdev->stdCoreCnt = 0;
		pSTDdev->stdDAQCnt = 0;

/*		pRTcore->clkStartTime = (double)pSTDdev->ST_Base.dT0[0];		*/
//		pRTcore->cntDAQ = 0;
		pRTcore->cntDAQ_loop = 0;
		pRTcore->cntAccum = 0;


#if 0		
		sprintf(fileName, "/mnt/ramdisk/%lu_raw.dat", pSTDdev->ST_Base.shotNumber);
//		sprintf(fileName, "/mnt/ramdisk/temp.txt");
//		sprintf(fileName, "temp.txt");
		if( (pRTcore->fp_raw= fopen(fileName, "wb")) == NULL )
		{
			printf("'%s'  open fp error!\n", fileName);
			return;
		}
#endif
		printf("Hz: %dHz, step time: %lf\n", pSTDdev->rtClkRate, pSTDdev->rtStepTime);


		flush_STDdev_to_MDSfetch( pSTDdev );
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;	
		notify_error(ERR_SCN, "no error\n");
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;
#if 0
		if( pRTcore->fp_raw != NULL )
			fclose(pRTcore->fp_raw);
/*		system("mv -f /mnt/ramdisk/* /home/data/rfmVal/ "); */

#endif

		kfwPrint(0, "stdCoreCnt: %d, stdDAQCnt: %d", pSTDdev->stdCoreCnt, pSTDdev->stdDAQCnt );

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		notify_error(ERR_SCN, "no error\n");
	}
}

void func_RTcore_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_RTcore *pRTcore = (ST_RTcore *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		/* to do here */
		epicsEventSignal( pSTDdev->ST_RTthread.threadEventId );



		pSTDdev->StatusDev |= TASK_IN_PROGRESS;
		notify_error(ERR_SCN, "no error\n");
	}
	else 
	{
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;



		
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		notify_error(ERR_SCN, "no error\n");
	}

	
}

void func_RTcore_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_RTcore *pRTcore = (ST_RTcore *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: event rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}


void func_RTcore_SYS_RESET(void *pArg, double arg1, double arg2)
{
//	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your device reset functions */




}




