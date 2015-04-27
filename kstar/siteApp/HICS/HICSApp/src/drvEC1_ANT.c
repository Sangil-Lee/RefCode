#include <fcntl.h>

#include "sfwCommon.h"

#include "drvEC1_ANT.h"

#include "ControllerHandler.h"
#include "RfmHandler.h"

int createRfmHandler (ST_STD_device *pSTDdev)
{
	epicsThreadCreate ("rfm_recv",
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) kstar_rfm_handler,
			(void*)pSTDdev);

	return WR_OK;
}

int createControllerHandler (ST_STD_device *pSTDdev)
{
	epicsThreadCreate ("ctrl_recv",
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) ctrl_comm_handler,
			(void*)pSTDdev);

	return WR_OK;
}

int initNI6259 (ST_EC1_ANT *pDev, int deviceNum, int channelNo)
{
	char	filename[64];

	kuDebug (kuMON, "[initNI6259] init ...\n");

	// open AI file descriptor
	sprintf (filename, "%s.%u.ai", STR_DIG_DEV_NAME, deviceNum);

	if (0 > (pDev->devFD = open (filename, O_RDWR))) {
		kuDebug (kuERR, "Failed to open device: %s\n", strerror(errno));
		return (kuNOK);
	}

	// initialize AI configuration
	pDev->aiConfig = pxi6259_create_ai_conf();

	// configure AI channels
	if (pxi6259_add_ai_channel (&pDev->aiConfig, channelNo, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
		kuDebug (kuERR, "Failed to configure channel %u\n", channelNo);
		return (kuNOK);
	}

	// configure AI sampling clock
	if (pxi6259_set_ai_sample_clk( &pDev->aiConfig, 20000000, 3, AI_SAMPLE_SELECT_PFI0, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
		kuDebug (kuERR, "Failed to configure AI sampling clock!\n");
		return (kuNOK);
	}

	// load AI configuration and let it apply
	if (pxi6259_load_ai_conf (pDev->devFD, &pDev->aiConfig)) {
		kuDebug (kuERR, "Failed to load configuration!\n");
		return (kuNOK);
	}

	// open file descriptor for each AI channel
	sprintf (filename, "%s.%u.ai.%u", STR_DIG_DEV_NAME, deviceNum, channelNo);

	if (0 > (pDev->channelFD = open (filename, O_RDWR | O_NONBLOCK))) {
		kuDebug (kuERR, "Failed to open channel %u: %s\n", channelNo, strerror(errno));
		return (kuNOK);
	}

	// start AI segment (data acquisition)
	if (pxi6259_start_ai (pDev->devFD)) {
		kuDebug (kuERR, "Failed to start data acquisition!\n");
		return (kuNOK);
	}

	epicsThreadSleep (0.0001);

	kuDebug (kuMON, "[initNI6259] init ok ...\n");

	return (kuOK);
}

int create_EC1_ANT_taskConfig (ST_STD_device *pSTDdev)
{
	ST_EC1_ANT * pEC1_ANT = NULL;
	
	pEC1_ANT = (ST_EC1_ANT *)calloc(1, sizeof(ST_EC1_ANT));
	if( !pEC1_ANT) return WR_ERROR;

	pEC1_ANT->channelFD		= -1;
	pEC1_ANT->channelNo		= 0;
	pEC1_ANT->devFD			= -1;
	pEC1_ANT->devNo			= 0;
	pEC1_ANT->dDbCmdRate	= 0;

	// initialize event for ECH Power Control thread
	pEC1_ANT->ctrlRunEventId	= epicsEventCreate (epicsEventEmpty);

	pSTDdev->pUser = pEC1_ANT;

	// initialize NI-6259 using CCS pxi6259 library
	initNI6259 (pEC1_ANT, pEC1_ANT->devNo, pEC1_ANT->channelNo);

	// creates a thread for RFM interface
	createRfmHandler (pSTDdev);

	// creates a thread for controller interface via serial(?)
	createControllerHandler (pSTDdev);

	kuDebug (kuMON, "[create_EC1_ANT_taskConfig] ...\n");

	return WR_OK;
}

void clear_EC1_ANT_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 

#if 0
#if USE_RFM_MMAP
//	result = RFM2gUnMapUserMemory(pEC1_ANT->Handle, (volatile void **)(&outbuffer), pEC1_ANT->Pages);
	result = RFM2gUnMapUserMemoryBytes(pEC1_ANT->Handle, (volatile void **)(&pEC1_ANT->mapBuf), pEC1_ANT->Bytes);
	if( result != RFM2G_SUCCESS )
	{
		printf( "ERROR: RFM2gUnMapUserMemory() failed.\n" );
		printf( "ERROR MSG: %s\n", RFM2gErrorMsg(result));
	}
#endif

	 /* Close the Reflective Memory device */
	RFM2gClose( &pEC1_ANT->Handle );

	printf("rfm2g closed.\n");
#endif
}

void threadFunc_EC1_ANT_RT (void *param)
{
	ST_STD_device *pSTDmy = (ST_STD_device*) param;
	ST_EC1_ANT *pEC1_ANT;
	ST_MASTER *pMaster = NULL;

	epicsThreadId pthreadInfo;
	
	//int nFirst = 1;
	//int toggle = 1;
	
	pMaster = get_master();
	if(!pMaster)	return;

	pEC1_ANT = (ST_EC1_ANT *)pSTDmy->pUser;
	if( !pEC1_ANT ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of pEC1_ANT.\n");
		return;
	}

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
	epicsThreadSetCPUAffinity( pthreadInfo, "5");
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_EC1_ANT, "SCHED_FIFO");
#endif

	int		num_samples = 1;
	float	read_buf[num_samples];
	int		scans_read;

	while(TRUE) {
#if 0
		epicsEventWait (pSTDmy->ST_RTthread.threadEventId);

		//		pSTDmy->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		//		notify_refresh_master_status();

		nFirst = 1;
		toggle = 1;
		printf("%s: Got epics RT Run event.\n", pSTDmy->taskName);
#endif

#if 0
		while(TRUE) {
			got	= read(pEC1_ANT->fd_event, &nval, 4);

			toggle = 0;

			pSTDmy->rtCnt++; 

			if( nFirst ) {
				send_master_stop_event();
				nFirst = 0;
			}
			
			if( pEC1_ANT->useScanIoRequest ) {
				scanIoRequest(pSTDmy->ioScanPvt_userCall);
			}

			if( !(pSTDmy->StatusDev & TASK_IN_PROGRESS ) )
			{
				pSTDmy->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				printf("system stop!.. bye RT thread!\n");
				break;
			}
		}
#else
		while (TRUE) {
			// read scaled samples for synchronization with PCSRT1
			if (0 > (scans_read = pxi6259_read_ai (pEC1_ANT->channelFD, read_buf, num_samples))) {
				epicsThreadSleep (0.00001);
				continue;
			}

			kuDebug (kuDEBUG, "[EC1_ANT_RT] schedule RfmHandler ...\n");

			// schedule RfmHandler
			epicsEventSignal (pEC1_ANT->ctrlRunEventId);
		}
#endif

		if( pMaster->cUseAutoSave ) {
			//db_put("HICS_EC1_ANT_SEND_DATA", "1");
		}
	}
}

void func_EC1_ANT_OP_MODE(void *pArg, double arg1, double arg2)
{
	if( (int)arg1 == OPMODE_REMOTE ) {
		/* to do here */

	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		/* to do here */

	}
}

void func_EC1_ANT_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_EC1_ANT_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

}

void func_EC1_ANT_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if ((int)arg1) {
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;
/* to do here */

		//pSTDdev->rtCnt = 0;

		printf("Hz: %dHz, step time: %lf\n", pSTDdev->rtClkRate, pSTDdev->rtStepTime);

		flush_STDdev_to_MDSfetch( pSTDdev );

		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;	

		notify_error(ERR_SCN, "no error\n");
	}
	else {
		if (check_dev_release_condition(pSTDdev)  == WR_ERROR) {
			return;
		}

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;

		notify_error(ERR_SCN, "no error\n");
	}
}

void func_EC1_ANT_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

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

void func_EC1_ANT_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: event rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}

void func_RTMON_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your MDS related functions */



	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();
}

void func_EC1_ANT_SYS_RESET(void *pArg, double arg1, double arg2)
{
	//	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	/*	call your device reset functions */

}

int init_my_sfwFunc_EC1_ANT( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/

	pSTDdev->ST_Func._OP_MODE = func_EC1_ANT_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_EC1_ANT_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_EC1_ANT_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_EC1_ANT_SYS_RUN;
	pSTDdev->ST_Func._SAMPLING_RATE= func_EC1_ANT_SAMPLING_RATE;
	//pSTDdev->ST_Func._DATA_SEND= func_EC1_ANT_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET = func_EC1_ANT_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_EC1_ANT_taskConfig;

	/*********************************************************/

	pSTDdev->ST_RTthread.threadFunc = (EPICSTHREADFUNC)threadFunc_EC1_ANT_RT;

	if (make_STD_RT_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"ST_RTthread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}

	kuDebug (kuMON, "[init_my_sfwFunc_EC1_ANT] ...\n");

	return WR_OK;
}

static long drvEC1_ANT_init_driver (void)
{
	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		kuDebug (kuMON, "[drvEC1_ANT_init_driver] devType(%s) type(%s)\n", pSTDdev->devType, STR_DEVICE_TYPE_1);

		if (strcmp (pSTDdev->devType, STR_DEVICE_TYPE_1) != 0) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if (create_EC1_ANT_taskConfig (pSTDdev) == WR_ERROR) {
			printf ("ERROR!  \"%s\" create_EC1_ANT_taskConfig() failed.\n", pSTDdev->taskName);
			return 1;
		}
		
		init_my_sfwFunc_EC1_ANT (pSTDdev);
		
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	notify_refresh_master_status ();

	kuDebug (kuMON, "[drvEC1_ANT_init_driver] local device init OK!\n");

	return 0;
}

static long drvEC1_ANT_io_report (int level)
{
	ST_STD_device *pSTDdev;
	ST_MASTER *pMaster = get_master();

	if(!pMaster) return 0;

	if(ellCount(pMaster->pList_DeviceTask)) {
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
	}
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pMaster->pList_DeviceTask));

	if(level<1) return 0;

	while (pSTDdev) {
		if (level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if (level>3) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			epicsPrintf("\n");
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
}
drvEC1_ANT = {
	2,
	drvEC1_ANT_io_report,
	drvEC1_ANT_init_driver
};

epicsExportAddress (drvet, drvEC1_ANT);

