#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "CodacPcsRunStub.h"
#include "drvDensity.h"
#include "drvRTCORE.h"
/*#include "myMDSplus.h" */


static long drvDNST_io_report(int level);
static long drvDNST_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvDNST = {
       2,
       drvDNST_io_report,
       drvDNST_init_driver
};

epicsExportAddress(drvet, drvDNST);


int create_DNST_taskConfig( ST_STD_device *pSTDdev)
{
	ST_DNST* pDNST;
	unsigned int nval=0;
	int status;

	
	pDNST = (ST_DNST *)malloc(sizeof(ST_DNST));
	if( !pDNST) return WR_ERROR;
	pSTDdev->pUser = pDNST;





//	pDNST->rtCnt = 0;
	pDNST->cntDAQ = 0;
	pDNST->daqStepTime = 0.00001;
	pDNST->useScanIoRequest = 0;


	pDNST->clkStartTime = -15.0;

	pDNST->m_PID_BLKCNT = 100;

	return WR_OK;
}


int init_my_sfwFunc_DNST( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_DNST_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_DNST_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_DNST_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_DNST_SYS_RUN;
	pSTDdev->ST_Func._SAMPLING_RATE= func_DNST_SAMPLING_RATE;
	pSTDdev->ST_Func._DATA_SEND = func_DNST_DATA_SEND;
	pSTDdev->ST_Func._TEST_PUT = func_DNST_TEST_PUT;

	pSTDdev->ST_Func._SYS_RESET = func_DNST_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_DNST_taskConfig;


	/*********************************************************/
#if 0
	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_DNST_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif
#if 1
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_DNST_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif

#if 0
	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_DNST_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}
#endif



	return WR_OK;

}


void clear_DNST_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_DNST *pDNST = (ST_DNST *)pSTDdev->pUser;
	unsigned int nval=0;
	int status;

	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 


}

void threadFunc_DNST_DAQ(void *param)
{
	ST_STD_device *pSTDmy = (ST_STD_device*) param;
	ST_DNST *pDNST;
	ST_STD_device *pSTDcore = NULL;
	ST_RTcore *pRTcore;
	ST_MASTER *pMaster = NULL;
	int			got;
	int		send=0;
	int		sent=0;
	int nval;
	int nFirst = 1;
	epicsThreadId pthreadInfo;

	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDmy ) {
		printf("ERROR! threadFunc_DNST_DAQ() has null pointer of STD device.\n");
		return;
	} /* else {
		printf("MY RMchk: %p, eventID:%d\n", pSTDmy, pSTDmy->ST_DAQThread.threadEventId);
	}*/
	pDNST = (ST_DNST *)pSTDmy->pUser;
	if( !pDNST ) {
		printf("ERROR! threadFunc_DNST_DAQ() has null pointer of pDNST.\n");
		return;
	}
	pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	while( !pSTDcore ) { 
		epicsThreadSleep(0.5);
		printf("threadFunc_DNST_DAQ: wait %s pointer.\n", STR_DEVICE_TYPE_1);
		pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	}
	pRTcore = (ST_RTcore *)pSTDcore->pUser;
	while( !pRTcore ) {
		epicsThreadSleep(0.5);
		printf("threadFunc_DNST_DAQ: wait %s' user pointer.\n", STR_DEVICE_TYPE_1);
		pRTcore =(ST_RTcore *)pSTDcore->pUser;
        }

	
	pthreadInfo = epicsThreadGetIdSelf();
	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
	epicsThreadSetCPUAffinity( pthreadInfo, "2");
//	setPosixPriority(pthreadInfo, 91, SCHED_FIFO);
	epicsThreadSetPosixPriority(pthreadInfo, 91, "SCHED_FIFO");

	

	while(TRUE) {
		epicsEventWait( pSTDmy->ST_DAQThread.threadEventId);
//		pSTDmy->StatusDev |= TASK_WAIT_FOR_TRIGGER;
//		notify_refresh_master_status();
//		printf("%s: Got epics Run event.\n", pSTDmy->taskName);

		pDNST->cntDAQ++; 


		if( pDNST->useScanIoRequest )
			scanIoRequest(pSTDmy->ioScanPvt_userCall);

	}
}

void threadFunc_DNST_RingBuf(void *param)
{
	int i, ellCnt;
	__u32  dio_reg;

	ST_STD_device *pSTDmy = (ST_STD_device*) param;
	ST_DNST *pDNST;
	ST_STD_device *pSTDcore = NULL;
	ST_RTcore *pRTcore;
	ST_MASTER *pMaster = NULL;
	epicsThreadId pthreadInfo;

	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode;	
	
	pDNST = (ST_DNST *)pSTDmy->pUser;
	if( !pDNST ) {
		printf("ERROR! threadFunc_DNST_RingBuf() has null pointer of pDNST.\n");
		return;
	}


	pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	while( !pSTDcore ) { 
		epicsThreadSleep(0.5);
		printf("threadFunc_DNST_RingBuf: wait %s pointer.\n", STR_DEVICE_TYPE_1);
		pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	}
	pRTcore = (ST_RTcore *)pSTDcore->pUser;
	while( !pRTcore ) {
		epicsThreadSleep(0.5);
		printf("threadFunc_DNST_RingBuf: wait %s' user pointer.\n", STR_DEVICE_TYPE_1);
		pRTcore =(ST_RTcore *)pSTDcore->pUser;
        }



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
		ellAdd(pSTDmy->pList_BufferNode, &pbufferNode1->node);
	}
	epicsPrintf(" %s: create %d node (size:%dByte)\n", pSTDmy->ST_RingBufThread.threadName, 
																		MAX_RING_BUF_NUM,
																		sizeof(ST_buf_node) );
/*
	epicsPrintf("task launching: %s thread for %s task\n",pringThreadConfig->threadName, pSTDdev->taskName);
*/

	pthreadInfo = epicsThreadGetIdSelf();
	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
	epicsThreadSetCPUAffinity( pthreadInfo, "2");
//	setPosixPriority(pthreadInfo, 91, SCHED_FIFO);
	epicsThreadSetPosixPriority(pthreadInfo, 91, "SCHED_FIFO");

	while(TRUE) 
	{
		epicsMessageQueueReceive(pSTDmy->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));
#if 0	
		dio_reg = 0x101;
		ioctl(pRTcore->fd, IOCTL_DEVICE_WRITE_DIO_0, &dio_reg);
		dio_reg = 0x100;
		ioctl(pRTcore->fd, IOCTL_DEVICE_WRITE_DIO_0, &dio_reg);
#endif	
		pbufferNode = queueData.pNode;
		pDNST->density	= dnst_process(pDNST->m_PID_BLKCNT, pbufferNode->data);
		pDNST->raw_vltg1 = op_get_voltage(pbufferNode->data[0]);
		pDNST->raw_vltg2	= op_get_voltage(pbufferNode->data[1]);

		/* perform PID */	
		pDNST->pid_r = pid_getRefSigVoltage(pDNST->m_loop_cnt);
		pDNST->pid_e = pid_processError(pDNST->pid_r, pDNST->density);
		if( pid_get_enable() )
			pDNST->pid_u =  pid_get_enable_cnt(pDNST->m_loop_cnt) ? pid_update(pDNST->pid_e) : pDNST->pid_r;
		else 
			pDNST->pid_u = pDNST->pid_r;

		scanIoRequest(pSTDmy->ioScanPvt_userCall);
#if 1	
		dio_reg = 0x101;
		ioctl(pRTcore->fd, IOCTL_DEVICE_WRITE_DIO_0, &dio_reg);
		dio_reg = 0x100;
		ioctl(pRTcore->fd, IOCTL_DEVICE_WRITE_DIO_0, &dio_reg);
#endif		
	
		pDNST->m_loop_cnt++;
			
		pbufferNode = queueData.pNode;
		ellAdd(pSTDmy->pList_BufferNode, &pbufferNode->node);
		ellCnt = ellCount(pSTDmy->pList_BufferNode);
		if( ellCnt < MAX_CUTOFF_BUF_NODE )
			printf("%s: Ring buffer node count errror!. cnt:%d\n", pSTDmy->taskName, ellCnt );

	} /* while(TRUE)  */

	return;
}




static long drvDNST_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_DNST *pDNST = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) != 0 ) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if(create_DNST_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_DNST_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_DNST(pSTDdev);

		init_controller ();
		show_internal_value();
		
	
		pDNST = (ST_DNST *)pSTDdev->pUser;


		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pDNST) { */

	notify_refresh_master_status();

	printf("****** DNST local device init OK!\n");

	return 0;
}


static long drvDNST_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_DNST *pDNST;
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
		pDNST = (ST_DNST *)pSTDdev->pUser;
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
/*		
			epicsPrintf("   callback time: %fusec\n", pDNST->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pDNST->adjCount_smplRate,
					                                                 pDNST->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pDNST->adjCount_Gain,
					                                                   pDNST->adjTime_Gain_Usec);
*/
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_DNST_OP_MODE(void *pArg, double arg1, double arg2)
{
	if( (int)arg1 == OPMODE_REMOTE ) {
		/* to do here */

	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		/* to do here */

	}
}

void func_DNST_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_DNST_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

	


}

void func_DNST_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_DNST *pDNST = (ST_DNST *)pSTDdev->pUser;
	char fileName[40];
	char fileNameB[40];

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;
/* to do here */

		
		pid_set_sectEndTime(0, -2.001);
		pid_set_sectEndDensity(0, 0);
		
		pid_set_sectEndTime(1, -2.000);
		pid_set_sectEndDensity(1, 1.5);
		
		pid_set_sectEndTime(2, -1.0);
		pid_set_sectEndDensity(2, 1.5);
		
		pid_set_sectEndTime(3, -0.999);
		pid_set_sectEndDensity(3, 0);
		
		pid_set_sectEndTime(4, 0);
		pid_set_sectEndDensity(4, 0);
			
		pid_set_sectEndTime(5, 2);
		pid_set_sectEndDensity(5, 3);
		
		pid_set_sectEndTime(6, 4);
		pid_set_sectEndDensity(6, 4);
		
		pid_set_sectEndTime(7, 5);
		pid_set_sectEndDensity(7, 4);
		
		pid_set_sectEndTime(8, 5.001);
		pid_set_sectEndDensity(8, 0);
		
		pid_set_sectEndTime(9, 10);
		pid_set_sectEndDensity(9, 0);
		
		pid_set_sectCounter(10);
		
		op_set_startTime(-4.999);
		op_set_stopTime(10);
		pid_set_startTime(2);
		pid_set_stopTime(5);

		// kp, ki, kd, error_thresh, step_time	
		pid_initialize(2.5, 0.0, 0.0, 100000, 0.001);
		pid_set_enable(1);
		
		pid_make_refPattern();
		
		dnst_initialize(C_A1, C_B1, C_C1, C_D1, C_A2, C_B2, C_C2, C_D2, LMT_U, LMT_L);


		show_internal_value();


		pDNST->m_loop_cnt = 0;


		flush_STDdev_to_MDSfetch( pSTDdev );
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;	
		notify_error(ERR_SCN, "no error\n");
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;


		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		notify_error(ERR_SCN, "no error\n");
	}
}

void func_DNST_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_DNST *pDNST = (ST_DNST *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		/* to do here */


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

void func_DNST_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_DNST *pDNST = (ST_DNST *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: event rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}
void func_DNST_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your MDS related functions */



	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();
}

void func_DNST_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your device reset functions */


}

void func_DNST_TEST_PUT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_DNST *pDNST = (ST_DNST *)pSTDdev->pUser;

/*	call your device reset functions */
	show_internal_value();


}






