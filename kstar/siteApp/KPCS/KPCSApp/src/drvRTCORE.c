#include "sfwCommon.h"



#include "drvRTCORE.h"
#include "drvDensity.h"

/*#include "myMDSplus.h" */


static long drvRTcore_io_report(int level);
static long drvRTcore_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvRTcore = {
       2,
       drvRTcore_io_report,
       drvRTcore_init_driver
};

epicsExportAddress(drvet, drvRTcore);


static  int             _def_board              = -1;


int create_RTcore_taskConfig( ST_STD_device *pSTDdev)
{
	ST_RTcore* pRTcore;
	unsigned int nval=0;
	int status;

	int             qty;
	int             errs=0;
	uint32  thresh, thresh_in;


	
	pRTcore = (ST_RTcore *)malloc(sizeof(ST_RTcore));
	if( !pRTcore) return WR_ERROR;
	pSTDdev->pUser = pRTcore;
    
	label_init(26);
	id_host();

	qty = count_boards();
	errs = select_1_board(qty, &_def_board);
	if ((qty <= 0) || (errs))
	{
		notify_error(1, ">>>>>>>>> ERROR: Can't find any device.");
		free(pRTcore);
		return WR_ERROR;
	}
	 pRTcore->fd = dev_open(_def_board);
	 if (pRTcore->fd == -1)
	 {
		 notify_error(1, ">>>>>>>>> ERROR: Unable to access device %d\n", _def_board);
		 free(pRTcore);
		 return WR_ERROR;
	  }
	 pRTcore->u16GPIO = 0x0;
	 thresh = (0x3FFFF)                                              /* max threshold */
             - ((4L * 1000 * 1000) * (0.0025 * 2))   /* 2 clock ticks of 4M S/S  */
             - ((4L * 1000 * 1000) * (0.000030));    /* 30us for write() and ISR  */
         thresh = 256L * 1024 - thresh;

	 thresh_in = (0x3FFFF)                                                           // max threshold
            - ((16L * 1000 * 1000) * (0.0025 * 2))  // 2 clock ticks of 16M S/S
            - ((16L * 1000 * 1000) * (0.000030));   // 30us for read() and ISR

	pRTcore->out_ch_mask = AO_CH_ALL_ENABLE;
	/* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
	pRTcore->out_io_mode = DMA_DEMAND_MODE;
	/* OUT_MODE_REGISTER, OUT_MODE_BUFFERED */
	pRTcore->out_mode = OUT_MODE_BUFFERED;
	pRTcore->out_simultaneous = TRUE; /* TRUE, FALSE */
	/*OUT_CLK_INITIATOR_RATE_C, OUT_CLK_INITIATOR_HARDWARE*/
	pRTcore->out_clock_initiator_mode = OUT_CLK_INITIATOR_RATE_C;
	/*OUTPUT_RANGE_10, OUTPUT_RANGE_2_5, OUTPUT_RANGE_5*/
	pRTcore->out_range = OUTPUT_RANGE_10;
	pRTcore->out_burst_enable = FALSE;
	pRTcore->out_clock_c = 40L * 1000 * 1000;   /* Master reference clock  */
	pRTcore->out_rate_c = INIT_TX_CLOCK;
	pRTcore->out_rate_gen_c_enable = TRUE;
	pRTcore->out_clock_enable = OUT_CLK_ENABLE; /*OUT_CLK_DISABLE, OUT_CLK_ENABLE */
	pRTcore->out_buffer_threshold = thresh;
	pRTcore->out_circular_buffer_enable = FALSE;
	pRTcore->out_period = 5;

	/* ENABLE_INPUT_0, ENABLE_INPUT_1, xxx, ENABLE_INPUT_ALL */
/*	pRTcore->in_ch_mask = ENABLE_INPUT_ALL;*/
	pRTcore->in_ch_mask = ENABLE_INPUT_0 + ENABLE_INPUT_1;
	/* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
	pRTcore->in_io_mode = DMA_DEMAND_MODE;
	/* AIM_DIFFERENTIAL, AIM_SINGLE_ENDED, xxx*/
	pRTcore->in_mode = AIM_DIFFERENTIAL;
	/*CLK_INITIATOR_OUTPUT, CLK_INITIATOR_INPUT*/
	pRTcore->in_clock_initiator_mode = CLK_INITIATOR_INPUT;
	/*INPUT_RANGE_10, INPUT_RANGE_2_5, INPUT_RANGE_5*/
	pRTcore->in_range_a = INPUT_RANGE_10; /* 0,1,4,5 */
	pRTcore->in_range_b = INPUT_RANGE_10; /* 2,3,6,7 */
	pRTcore->in_burst_enable = FALSE;
	pRTcore->in_clock_a = 40L * 1000 * 1000;    /* Master reference clock  */
	pRTcore->in_rate_a = INIT_RX_CLOCK; /* --> 10000L */
	pRTcore->in_rate_gen_a_enable = TRUE;
	pRTcore->in_buffer_threshold = thresh_in;
	pRTcore->in_buffer_enable = TRUE;
	pRTcore->in_period = 10;
	pRTcore->input_format = DATA_FORMAT_OFFSET_BINARY;

	errs    += dev_input_buffer_fill(-1, pRTcore->fd, FALSE);
	errs    += dev_input_buffer_overflow_detect(-1, pRTcore->fd, FALSE);
	errs    += dev_io_timeout(-1, pRTcore->fd, 20);
	errs    += dev_io_input_mode(-1, pRTcore->fd, pRTcore->in_io_mode);
	errs    += dev_input_channels(-1, pRTcore->fd, pRTcore->in_ch_mask);
	errs    += dev_input_clock_initiator_mode(-1, pRTcore->fd, pRTcore->in_clock_initiator_mode);
	errs    += dev_input_mode(-1, pRTcore->fd, pRTcore->in_mode);
	errs    += dev_input_range_a(-1, pRTcore->fd, pRTcore->in_range_a);
	errs    += dev_input_range_b(-1, pRTcore->fd, pRTcore->in_range_b);
	errs    += dev_input_format     (-1, pRTcore->fd, DATA_FORMAT_OFFSET_BINARY);
	errs    += dev_input_burst_enable(-1, pRTcore->fd, pRTcore->in_burst_enable);
	errs    += dev_rate_gen_a_ndiv(-1, pRTcore->fd, 0, pRTcore->in_rate_a, pRTcore->in_clock_a);
	errs    += dev_rate_gen_a_enable(-1, pRTcore->fd, pRTcore->in_rate_gen_a_enable);
	errs    += dev_input_buffer_threshold(-1, pRTcore->fd, pRTcore->in_buffer_threshold);
	errs    += dev_input_buffer_enable(-1, pRTcore->fd, pRTcore->in_buffer_enable);
	errs    += dev_input_buffer_clear (-1, pRTcore->fd);

	nval = TRUE;
	ioctl( pRTcore->fd, IOCTL_DEVICE_SET_DIO_OUTPUT_0, &nval);
	ioctl( pRTcore->fd, IOCTL_DEVICE_SET_DIO_OUTPUT_1, &nval);



	pRTcore->cntDAQ = 0;
	pRTcore->daqStepTime = 0.00001;
	pRTcore->useScanIoRequest = 0;


	pRTcore->clkStartTime = -15.0;
	

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
#if 0
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

#if 0
	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RTcore_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}
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
/*
	status = ioctl(pRTcore->fd_event, IOCTL_CLTU_INTERRUPT_DISABLE, &nval);
	printf("IntLTU interrupt disabled!\n");

	close( pRTcore->fd_event );
	printf("intLTU closed.\n");
*/
	dev_close(_def_board, pRTcore->fd);
	printf("%s: 16aiss8ao4 FD closed!\n", pSTDdev->taskName);
}
void threadFunc_RTcore_RT(void *param)
{
	ST_STD_device *pSTDmy = (ST_STD_device*) param;
	ST_RTcore *pRTcore;
//	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdnst = (ST_STD_device*) param;
//	ST_DNST *pDNST;


	epicsThreadId pthreadInfo;

	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode = NULL;


	
	int got, get;
	int nval;
	int nFirst = 1;
	unsigned int data[800];
	__u32  dio_reg;

//	pMaster = get_master();
//	if(!pMaster)	return;

	pRTcore = (ST_RTcore *)pSTDmy->pUser;
	if( !pRTcore ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of pRTcore.\n");
		return;
	}

	
	pSTDdnst = get_STDev_from_type(STR_DEVICE_TYPE_2);
	while( !pSTDdnst ) { 
		epicsThreadSleep(0.5);
		printf("%S: wait %s pointer.\n", pSTDmy->taskName, STR_DEVICE_TYPE_2);
		pSTDdnst = get_STDev_from_type(STR_DEVICE_TYPE_2);
	}
//	pDNST = (ST_DNST *)pSTDdnst->pUser;
//	while( !pDNST ) {
//		epicsThreadSleep(0.1); printf(". ");
//		pDNST =(ST_DNST *)pSTDdnst->pUser;
//	}


	
	pthreadInfo = epicsThreadGetIdSelf();
	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
	epicsThreadSetCPUAffinity( pthreadInfo, "2");
//	setPosixPriority(pthreadInfo, 91, SCHED_FIFO);
	epicsThreadSetPosixPriority(pthreadInfo, 91, "SCHED_FIFO");
	
//	get = sizeof(__u32)* 2 * 100;
	get = sizeof(queueData.pNode->data);

	printf("RT core : read size: %d\n", get);

	while(TRUE) {
		epicsEventWait( pSTDmy->ST_RTthread.threadEventId);
		nFirst = 1;
		printf("%s: Got epics RT Run event.\n", pSTDmy->taskName);
		
		while(TRUE) {
//			got	= read(pRTcore->fd_event, &nval, 4);
			pbufferNode = (ST_buf_node *)ellFirst(pSTDdnst->pList_BufferNode);
			ellDelete(pSTDdnst->pList_BufferNode, &pbufferNode->node);
			
			got = read(pRTcore->fd, pbufferNode->data, get);
//			got	= read(pRTcore->fd, &data, get);

#if 0
			dio_reg = 0x101;
			ioctl(pRTcore->fd, IOCTL_DEVICE_WRITE_DIO_0, &dio_reg);
			dio_reg = 0x100;
			ioctl(pRTcore->fd, IOCTL_DEVICE_WRITE_DIO_0, &dio_reg);
#endif
//			if( (pSTDmy->rtCnt % 2) == 0) {
//			if( toggle ) {
//				epicsEventSignal( pSTDmy->ST_DAQThread.threadEventId );
//				epicsEventSignal( pSTDRMchk->ST_DAQThread.threadEventId );
//				WRITE32(pRTcore->base0 + 0x4, 0x1);
			queueData.opcode	= 1;
			queueData.pNode         = pbufferNode;
			epicsMessageQueueSend(pSTDdnst->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

#if 0
			fprintf( pRTcore->fp_cnt, "%d %lf : %d , %d %d | %d %d | %d %d | %d %d | %d %d | %d %d %08X %x | %x %x %x\n", pSTDmy->rtCnt,  (double)pSTDmy->rtCnt * pSTDmy->rtStepTime + pRTcore->clkStartTime, 
				pRTMON->mapBuf[RM_PCS_CNT/4], pRTMON->mapBuf[RM_PF1_CNT/4], pRTMON->mapBuf[RM_PF2_CNT/4],
				pRTMON->mapBuf[RM_PF3U_CNT/4], pRTMON->mapBuf[RM_PF3L_CNT/4],
				pRTMON->mapBuf[RM_PF4U_CNT/4], pRTMON->mapBuf[RM_PF4L_CNT/4],
				pRTMON->mapBuf[RM_PF5U_CNT/4], pRTMON->mapBuf[RM_PF5L_CNT/4],
				pRTMON->mapBuf[RM_PF6U_CNT/4], pRTMON->mapBuf[RM_PF6L_CNT/4],
				pRTMON->mapBuf[RM_PF7_CNT/4], pRTMON->mapBuf[RM_IVC_CNT/4],
				pRTMON->mapBuf[RM_PCS_FAULT/4],
				pRTMON->mapBuf[RM_PCS_FAST_INTLOCK/4],
				pRTMON->mapBuf[RM_RMP_BTM_I/4],
				pRTMON->mapBuf[RM_RMP_MID_I/4],
				pRTMON->mapBuf[RM_RMP_TOP_I/4]
			);
#endif
//				WRITE32(pRTcore->base0 + 0x4, 0x0);

//			}
			
//			WRITE32(pRTcore->base0 + 0x4, 0x0);
			pSTDmy->rtCnt++; 

//			WRITE32(pRTcore->base0 + 0x4, 0x0);
			if( nFirst ) {
				send_master_stop_event();
				nFirst = 0;
			}
			
			if( pRTcore->useScanIoRequest )
				scanIoRequest(pSTDmy->ioScanPvt_userCall);

			if( !(pSTDmy->StatusDev & TASK_IN_PROGRESS ) )
			{
				pSTDmy->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
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
	int			got;
	int		send=0;
	int		sent=0;
	int nval;
	int nFirst = 1;
//	epicsThreadId pthreadInfo;

	
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
//	pthreadInfo = epicsThreadGetIdSelf();
//	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
//	epicsThreadSetCPUAffinity( pthreadInfo, "2");

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
//		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
//		notify_refresh_master_status();
//		printf("%s: Got epics Run event.\n", pSTDdev->taskName);

//		WRITE32(pRTcore->base0 + 0x4, 0x0);
		pRTcore->cntDAQ++; 


//		WRITE32(pRTcore->base0 + 0x4, 0x0);


		if( pRTcore->useScanIoRequest )
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

	}
}

static long drvRTcore_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_RTcore *pRTcore = NULL;
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
		
	
		pRTcore = (ST_RTcore *)pSTDdev->pUser;


		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pRTcore) { */

	notify_refresh_master_status();

	printf("****** RTcore local device init OK!\n");

	return 0;
}


static long drvRTcore_io_report(int level)
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


		pSTDdev->rtCnt = 0;
		pRTcore->clkStartTime = (double)pSTDdev->ST_Base.dT0[0];

		dev_input_buffer_clear (-1, pRTcore->fd);

#if 0		
		sprintf(fileName, "/mnt/ramdisk/%lu_rt.txt", pSTDdev->ST_Base.shotNumber);
//		sprintf(fileName, "/mnt/ramdisk/temp.txt");
//		sprintf(fileName, "temp.txt");
		if( (pRTcore->fp_cnt = fopen(fileName, "w")) == NULL )
		{
			printf("'%s'  open fp error!\n", fileName);
			return;
		}
		printf("RT core system Arming...\n");
		pSTDdev->rtCnt = 0;
		pSTDdev->rtStepTime = 1.0 /(double)pSTDdev->rtClkRate;
		pRTcore->clkStartTime = (double)pSTDdev->ST_Base.dT0[0];



		fprintf(pRTcore->fp_cnt, "Real Time monitor - counter check.\n");
		fprintf(pRTcore->fp_cnt, "Hz: %dHz, step time: %lf\n", pSTDdev->rtClkRate, pSTDdev->rtStepTime);
		fprintf(pRTcore->fp_cnt, "rtCnt time(s) :  PCS,  PF1	PF2  (3U 3L) (4U 4L) (5U 5L) (6U 6L) PF7 IVC  PCS_fault Fast_INTL |BI MI TI \n");
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
		if( pRTcore->fp_cnt != NULL )
			fclose(pRTcore->fp_cnt);

		system("mv -f /mnt/ramdisk/* /home/data/rfmVal/ ");
#endif

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		notify_error(ERR_SCN, "no error\n");
	}
}

void func_RTcore_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_RTcore *pRTcore = (ST_RTcore *)pSTDdev->pUser;

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
	ST_RTcore *pRTcore = (ST_RTcore *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: event rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}


void func_RTcore_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your device reset functions */




}




