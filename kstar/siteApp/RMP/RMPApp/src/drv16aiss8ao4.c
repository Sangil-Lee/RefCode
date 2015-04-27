#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drv16aiss8ao4.h"
#include <math.h>
#include <errno.h>

#include "myMDSplus.h"

#include "drvRfm5565.h"
#include "rfm2g_api.h"

#include "waveformRecord.h"


static long drv16aiss8ao4_io_report(int level);
static long drv16aiss8ao4_init_driver();

//static void exit_16aiss8ao4_Task(void);


struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drv16aiss8ao4 = {
       2,
       drv16aiss8ao4_io_report,
       drv16aiss8ao4_init_driver
};

epicsExportAddress(drvet, drv16aiss8ao4);


#define OUT_BUF_SIZE  (400000) /* for 100 sec, all channel operation */
#define	ARRAY_ELEMENTS(a)	(sizeof((a))/sizeof((a)[0]))
static	__u32	_data_out[OUT_BUF_SIZE];
static	long	_loops;	// Number of times to send a block of data.
static	long	_qty;	// Number of samples to send per block.


static	int		_def_board		= -1;



int create_16aiss8ao4_taskConfig( ST_STD_device *pSTDdev)
{
	ST_16aiss8ao4* p16aiss8ao4;
	int		qty;
	int		errs=0;
	uint32  thresh, thresh_in;

	p16aiss8ao4 = (ST_16aiss8ao4 *)malloc(sizeof(ST_16aiss8ao4));
	if( !p16aiss8ao4) return WR_ERROR;

	pSTDdev->pUser = p16aiss8ao4;

	label_init(26);
	id_host();
	

	qty = count_boards();
	errs = select_1_board(qty, &_def_board);

	if ((qty <= 0) || (errs))
	{
		notify_error(1, ">>>>>>>>> ERROR: Can't find any device.");
		free(p16aiss8ao4);
		return WR_ERROR;
	}

	p16aiss8ao4->fd = dev_open(_def_board);
	if (p16aiss8ao4->fd == -1)
	{
		notify_error(1, ">>>>>>>>> ERROR: Unable to access device %d\n", _def_board);
		free(p16aiss8ao4);
		return WR_ERROR;
	}


	gsc_reset_pattern(p16aiss8ao4);
	

	p16aiss8ao4->u16GPIO = 0x0;


	thresh = (0x3FFFF) 						/* max threshold */
			- ((4L * 1000 * 1000) * (0.0025 * 2))	/* 2 clock ticks of 4M S/S  */
			- ((4L * 1000 * 1000) * (0.000030));	/* 30us for write() and ISR  */
	thresh = 256L * 1024 - thresh;

	thresh_in = (0x3FFFF)								// max threshold
			- ((16L * 1000 * 1000) * (0.0025 * 2))	// 2 clock ticks of 16M S/S
			- ((16L * 1000 * 1000) * (0.000030));	// 30us for read() and ISR

	p16aiss8ao4->out_ch_mask = AO_CH_ALL_ENABLE;
	/* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
	p16aiss8ao4->out_io_mode = DMA_DEMAND_MODE;
	/* OUT_MODE_REGISTER, OUT_MODE_BUFFERED */
	p16aiss8ao4->out_mode = OUT_MODE_BUFFERED; 	
	p16aiss8ao4->out_simultaneous = TRUE; /* TRUE, FALSE */
	/*OUT_CLK_INITIATOR_RATE_C, OUT_CLK_INITIATOR_HARDWARE*/
	p16aiss8ao4->out_clock_initiator_mode = OUT_CLK_INITIATOR_RATE_C; 
	/*OUTPUT_RANGE_10, OUTPUT_RANGE_2_5, OUTPUT_RANGE_5*/	
	p16aiss8ao4->out_range = OUTPUT_RANGE_10; 
	p16aiss8ao4->out_burst_enable = FALSE;
	p16aiss8ao4->out_clock_c = 40L * 1000 * 1000;	/* Master reference clock  */
	p16aiss8ao4->out_rate_c = INIT_TX_CLOCK;
	p16aiss8ao4->out_rate_gen_c_enable = TRUE;
	p16aiss8ao4->out_clock_enable = OUT_CLK_ENABLE; /*OUT_CLK_DISABLE, OUT_CLK_ENABLE */
	p16aiss8ao4->out_buffer_threshold = thresh;
	p16aiss8ao4->out_circular_buffer_enable = FALSE;
	p16aiss8ao4->out_period = 5;


	/* ENABLE_INPUT_0, ENABLE_INPUT_1, xxx, ENABLE_INPUT_ALL */
	p16aiss8ao4->in_ch_mask = ENABLE_INPUT_ALL; 
	/* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
	p16aiss8ao4->in_io_mode = DMA_DEMAND_MODE;
	/* AIM_DIFFERENTIAL, AIM_SINGLE_ENDED, xxx*/
	p16aiss8ao4->in_mode = AIM_DIFFERENTIAL;
	/*CLK_INITIATOR_OUTPUT, CLK_INITIATOR_INPUT*/
	p16aiss8ao4->in_clock_initiator_mode = CLK_INITIATOR_OUTPUT;
	/*INPUT_RANGE_10, INPUT_RANGE_2_5, INPUT_RANGE_5*/	
	p16aiss8ao4->in_range_a = INPUT_RANGE_10; /* 0,1,4,5 */
	p16aiss8ao4->in_range_b = INPUT_RANGE_10; /* 2,3,6,7 */
	p16aiss8ao4->in_burst_enable = FALSE;
	p16aiss8ao4->in_clock_a = 40L * 1000 * 1000;	/* Master reference clock  */
	p16aiss8ao4->in_rate_a = INIT_RX_CLOCK; /* --> 10000L */
	p16aiss8ao4->in_rate_gen_a_enable = TRUE;
	p16aiss8ao4->in_buffer_threshold = thresh_in;
	p16aiss8ao4->in_buffer_enable = TRUE;
	p16aiss8ao4->in_period = 5;


	p16aiss8ao4->input_format = DATA_FORMAT_OFFSET_BINARY;

	


	for( qty=0; qty < CNT_AI_CH; qty++) {
		p16aiss8ao4->fpRaw[qty] = NULL;
		p16aiss8ao4->d32CurValue[qty] = 0.0;
/*		printf(p16aiss8ao4->strTagName[qty], "\\B%d_CH%d:FOO", pSTDdev->BoardID, qty); */
	}

	p16aiss8ao4->u8_Link_PS = 0;
	p16aiss8ao4->u8_ip_fault = 0;
	p16aiss8ao4->u8_PCS_control = 0;

	p16aiss8ao4->f32MaxCurrent = INIT_MAX_CURRENT;
	p16aiss8ao4->u32CmdCurrentLimit = 4000;
	p16aiss8ao4->d32CurrentLimitDuration = 5;
 

	return WR_OK;
}

int init_my_MDS_tag( ST_STD_device *pSTDdev)
{
	ST_16aiss8ao4* p16aiss8ao4;

	p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	
/*
	for(i=0; i<CNT_AI_CH; i++) {
		sprintf(p16aiss8ao4->strTagName[i], "\\B%d_CH%d:FOO", pSTDdev->BoardID, qty);

	}
*/
#if 1
	sprintf(p16aiss8ao4->strTagName[CH_ID_BTM_I_AI], TAG_NAME_CH0);
	sprintf(p16aiss8ao4->strTagName[CH_ID_BTM_V_AI], TAG_NAME_CH1);

	sprintf(p16aiss8ao4->strTagName[CH_ID_MID_I_AI], TAG_NAME_CH2);
	sprintf(p16aiss8ao4->strTagName[CH_ID_MID_V_AI], TAG_NAME_CH3);

	sprintf(p16aiss8ao4->strTagName[CH_ID_TOP_I_AI], TAG_NAME_CH4);
	sprintf(p16aiss8ao4->strTagName[CH_ID_TOP_V_AI], TAG_NAME_CH5);
	
	sprintf(p16aiss8ao4->strTagName[6], TAG_NAME_CH6);
	sprintf(p16aiss8ao4->strTagName[7], TAG_NAME_CH7);

	sprintf(p16aiss8ao4->strTagCmd[0], TAG_CMD_CH0); /* Bottom */
	sprintf(p16aiss8ao4->strTagCmd[1], TAG_CMD_CH1); /* Middle */

	sprintf(p16aiss8ao4->strTagCmd[2], TAG_CMD_CH2); /* Top     */
	sprintf(p16aiss8ao4->strTagCmd[3], TAG_CMD_CH3);
#endif
#if 0
// temporary  request by ymjeon. 2012. 10. 05.
	sprintf(p16aiss8ao4->strTagName[CH_ID_BTM_I_AI], TAG_NAME_CH0);
	sprintf(p16aiss8ao4->strTagName[CH_ID_BTM_V_AI], TAG_NAME_CH1);

	sprintf(p16aiss8ao4->strTagName[CH_ID_MID_I_AI], TAG_NAME_CH4);
	sprintf(p16aiss8ao4->strTagName[CH_ID_MID_V_AI], TAG_NAME_CH5);

	sprintf(p16aiss8ao4->strTagName[CH_ID_TOP_I_AI], TAG_NAME_CH2);
	sprintf(p16aiss8ao4->strTagName[CH_ID_TOP_V_AI], TAG_NAME_CH3);
	
	sprintf(p16aiss8ao4->strTagName[6], TAG_NAME_CH6);
	sprintf(p16aiss8ao4->strTagName[7], TAG_NAME_CH7);

	sprintf(p16aiss8ao4->strTagCmd[0], TAG_CMD_CH0);
	sprintf(p16aiss8ao4->strTagCmd[1], TAG_CMD_CH2);

	sprintf(p16aiss8ao4->strTagCmd[2], TAG_CMD_CH1);
	sprintf(p16aiss8ao4->strTagCmd[3], TAG_CMD_CH3);
#endif
	


	return WR_OK;
	
}


int init_my_SFW_16aiss8ao4( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_16aiss8ao4_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_16aiss8ao4_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_16aiss8ao4_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_16aiss8ao4_SYS_RUN;
	pSTDdev->ST_Func._SAMPLING_RATE= func_16aiss8ao4_SAMPLING_RATE;
	pSTDdev->ST_Func._DATA_SEND = func_16aiss8ao4_DATA_SEND;

	pSTDdev->ST_Func._Exit_Call = clear_16aiss8ao4_taskConfig;


	if( make_16aiss8ao4_FG_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"user FG thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_16aiss8ao4_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
		
#if 1
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_16aiss8ao4_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif
	/* move here to get child device pointer */
	/* must be at this position              */
#if 0
	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_16aiss8ao4_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}
#endif

	return WR_OK;

}


void clear_16aiss8ao4_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	
	dev_close(_def_board, p16aiss8ao4->fd);
	printf("%s: 16aiss8ao4 FD closed!\n", pSTDdev->taskName);

}


int make_16aiss8ao4_FG_thread(ST_STD_device *pSTDdev)
{
	ST_16aiss8ao4* p16aiss8ao4;
	char cName[SIZE_TASK_NAME];
	sprintf(cName, "%s_FG", pSTDdev->taskName );

	p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	p16aiss8ao4->epicsEvent_FGthread = epicsEventCreate(epicsEventEmpty);
	if ( ! p16aiss8ao4->epicsEvent_FGthread ) {
		return WR_ERROR;
	}

	epicsThreadCreate(cName, 
				epicsThreadPriorityHigh,
				epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadStackBig */
				threadFunc_16aiss8ao4_FG,
				(void*)pSTDdev); 
	return WR_OK;
}




void threadFunc_16aiss8ao4_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_16aiss8ao4 *p16aiss8ao4;
	ST_MASTER *pMaster = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
	p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	while(TRUE) {
/*		sem_wait( &DAQsemaphore ); */
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		notify_refresh_master_status();

#if 0
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif
		_io_read(pSTDdev);

/*		if( p16aiss8ao4->u8_ip_fault != 1 ) 
		{ */
			db_put("RMP_SYS_RUN", "0");
			epicsThreadSleep(3.0);
			db_put("RMP_SYS_ARMING", "0");
//			epicsThreadSleep(1.0);
//			notify_refresh_master_status();
/*		} */

#if 1	/*2012. 5. 31 don't need it. use auto save logic. */
		if( pMaster->cUseAutoSave ) 
			db_put("RMP_SEND_DATA", "1");
#endif
		
	}
}

void threadFunc_16aiss8ao4_FG(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	
	if( !p16aiss8ao4 ) {
		printf("ERROR! threadFunc_user_FG() has null pointer of STD device.\n");
		return;
	}

	while(TRUE) 
	{
		epicsEventWait( p16aiss8ao4->epicsEvent_FGthread);
#if 0
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif
		if( p16aiss8ao4->u8_PCS_control == 0 )
			_io_write(pSTDdev);
		
	}
}

void threadFunc_16aiss8ao4_RingBuf(void *param)
{
	int i, ellCnt;
/*	int show_info = 0; */

	ST_STD_device *pSTDdev = (ST_STD_device*) param;

	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode;	
	ST_16aiss8ao4 *p16aiss8ao4;

	ST_STD_device *pSTDdevRfm;
	ST_RFM5565 *pRFM5565;
	RFM2G_STATUS   result;
	RFM2G_UINT32   buffer;
	RFM2G_UINT32   buffer_mean[8];
	float rmp_vltg = 0.0;

	
//	pST_BufThread = (ST_threadCfg*) pSTDdev->pST_BuffThread;
	p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	pSTDdevRfm = (ST_STD_device *)get_STDev_from_type(STR_DEVICE_TYPE_2);
	while (!pSTDdevRfm->pUser) {
		printf("wait STD rfm pointer.\n");
		epicsThreadSleep(1.);
	}
	pRFM5565 = (ST_RFM5565 *)pSTDdevRfm->pUser;
/*	printf("\n\nRFM pointer: %p\n", pRFM5565); */



#if 0
	epicsPrintf("\n %s: pSTDdev: %p, p16aiss8ao4: %p , pST_BuffThread: %p\n",pSTDdev->taskName, pSTDdev, p16aiss8ao4, pST_BufThread );
#endif
/*	epicsPrintf("\n >>> %s has threadQueueId: %d\n", pST_BufThread->threadName, pST_BufThread->threadQueueId );
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
												ellCount(p16aiss8ao4->pdrvBufferConfig->pbufferList));
#endif
	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

		pbufferNode = queueData.pNode;

		for(i=0; i<8; i++)
			buffer_mean[i] = 0;

		for( i=0; i< BUFLEN; i+=8) {
			fwrite( &pbufferNode->data[i],     sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[0] );
			fwrite( &pbufferNode->data[i+1], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[1] );
			fwrite( &pbufferNode->data[i+2], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[2] );
			fwrite( &pbufferNode->data[i+3], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[3] );
			fwrite( &pbufferNode->data[i+4], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[4] );
			fwrite( &pbufferNode->data[i+5], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[5] );
			fwrite( &pbufferNode->data[i+6], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[6] );
			fwrite( &pbufferNode->data[i+7], sizeof(unsigned short), 1, p16aiss8ao4->fpRaw[7] );

			buffer_mean[0] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i];
			buffer_mean[1] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+1];
			buffer_mean[2] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+2];
			buffer_mean[3] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+3];
			buffer_mean[4] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+4];
			buffer_mean[5] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+5];
			buffer_mean[6] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+6];
			buffer_mean[7] += 0xffff & (RFM2G_UINT32)pbufferNode->data[i+7];
		}

		for(i=0; i<8; i++)
			buffer_mean[i] = buffer_mean[i]/10;
		
		ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
		ellCnt = ellCount(pSTDdev->pList_BufferNode);

		for( i=0; i<8; i++)
			p16aiss8ao4->d32CurValue[i] =  ((pbufferNode->data[i] & 0xffff) * 20.0 / 65535.0 ) - 10.0 ;
			

/*		printf("%s: ellCount:%d\n", pSTDdev->taskName, ellCnt); */
/*		for( i=0; i<CNT_AI_CH; i++)
			p16aiss8ao4->currAI[i] = 20.0 * ((double)(0xffff & pbufferNode->data[i]) / (double)0xffff) - 10.0;
*/ 
#if USE_SINGLE_RAW_VALUE_RFM

		buffer = 0xffff & (RFM2G_UINT32)pbufferNode->data[CH_ID_TOP_V_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_V, &buffer, sizeof(RFM2G_UINT32)*1 );
		buffer = 0xffff & (RFM2G_UINT32)pbufferNode->data[CH_ID_TOP_I_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_I, &buffer, sizeof(RFM2G_UINT32)*1 );

		/*****************/
		buffer = 0xffff & (RFM2G_UINT32)pbufferNode->data[CH_ID_MID_V_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_V, &buffer, sizeof(RFM2G_UINT32)*1 );
		buffer = 0xffff & (RFM2G_UINT32)pbufferNode->data[CH_ID_MID_I_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_I, &buffer, sizeof(RFM2G_UINT32)*1 );
		
		/*****************/

		buffer = 0xffff & (RFM2G_UINT32)pbufferNode->data[CH_ID_BTM_V_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_V, &buffer, sizeof(RFM2G_UINT32)*1 );
		buffer = 0xffff & (RFM2G_UINT32)pbufferNode->data[CH_ID_BTM_I_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_I, &buffer, sizeof(RFM2G_UINT32)*1 );
		
#else
		buffer = buffer_mean[CH_ID_TOP_V_AI];/* 2013. 8. 2 request by hshan */
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_V, &buffer, sizeof(RFM2G_UINT32) );
//		rmp_vltg = buffer_mean[CH_ID_TOP_V_AI] * 20.0 / 65535.0 - 10.0 ;
		//printf("TOP_V AI: %f\n", rmp_vltg);
//		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_V, &rmp_vltg, sizeof(rmp_vltg) );
		buffer = buffer_mean[CH_ID_TOP_I_AI]; 
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_I, &buffer, sizeof(RFM2G_UINT32) );
//		rmp_vltg = buffer_mean[CH_ID_TOP_I_AI] * 20.0 / 65535.0 - 10.0 ;
//		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_I, &rmp_vltg, sizeof(rmp_vltg) );

		/*****************/
		buffer = buffer_mean[CH_ID_MID_V_AI]; 
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_V, &buffer, sizeof(RFM2G_UINT32) );
//		rmp_vltg = buffer_mean[CH_ID_MID_V_AI] * 20.0 / 65535.0 - 10.0 ;
//		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_V, &rmp_vltg, sizeof(rmp_vltg) );

		buffer = buffer_mean[CH_ID_MID_I_AI]; 		
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_I, &buffer, sizeof(RFM2G_UINT32) );
//		rmp_vltg = buffer_mean[CH_ID_MID_I_AI] * 20.0 / 65535.0 - 10.0 ;
//		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_I, &rmp_vltg, sizeof(rmp_vltg) );

		/*****************/
		buffer = buffer_mean[CH_ID_BTM_V_AI];
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_V, &buffer, sizeof(RFM2G_UINT32) );
//		rmp_vltg = buffer_mean[CH_ID_BTM_V_AI] * 20.0 / 65535.0 - 10.0 ;
//		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_V, &rmp_vltg, sizeof(rmp_vltg) );

		buffer = buffer_mean[CH_ID_BTM_I_AI];		
		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_I, &buffer, sizeof(RFM2G_UINT32) );
//		rmp_vltg = buffer_mean[CH_ID_BTM_I_AI] * 20.0 / 65535.0 - 10.0 ;
//		result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_I, &rmp_vltg, sizeof(rmp_vltg) );

#endif

/* just for delay and synch test */
/*		drv16aiss8ao4_dio_out(pSTDdev, 0, 1);
		drv16aiss8ao4_dio_out(pSTDdev, 0, 0); */

/*                        end of 2012    */

	
		if( queueData.opcode & QUEUE_OPCODE_CLOSE_FILE)
		{
			printf("%s: file closeing...", pSTDdev->taskName);
			for( i=0; i<CNT_AI_CH; i++) 
			{
				if( p16aiss8ao4->fpRaw[i] ) {
					fclose(p16aiss8ao4->fpRaw[i]);
					p16aiss8ao4->fpRaw[i] = NULL;
				}
			}
			printf("OK.\n");
			printf("%s: received count: %d.\n", pSTDdev->taskName, queueData.u32Cnt);
			pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
			notify_refresh_master_status();
		}

	} /* while(TRUE)  */

	return;
}

void threadFunc_16aiss8ao4_CatchEnd(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;

	while (!pSTDdev)  {
		printf("ERROR! threadFunc_user_CatchEnd() has null pointer of STD device.\n");
		epicsThreadSleep(.1);
	}
	
	while(TRUE) 
	{
		epicsEventWait( pSTDdev->ST_CatchEndThread.threadEventId);
		printf("%s: got End event! it will sleep %fsec. \n", pSTDdev->taskName, pSTDdev->ST_Base_fetch.dT1[0] + 0.1);
		epicsThreadSleep( pSTDdev->ST_Base_fetch.dT1[0] + 0.1 );
		printf(">>> %s: Caught the T1 stop trigger!\n", pSTDdev->taskName);

		
		/* back to waiting for next shot start */
		
	}/* eof  While(TRUE) */

}


static long drv16aiss8ao4_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_16aiss8ao4 *p16aiss8ao4 = NULL;
	ST_STD_device *pSTDdev = NULL;
	int errs;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) != 0 ) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if(create_16aiss8ao4_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_16aiss8ao4_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_SFW_16aiss8ao4(pSTDdev);
		
		init_my_MDS_tag(pSTDdev);
	
		p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
#if 1


		errs	+= dev_initialize(-1, p16aiss8ao4->fd);   /* don't need 'cause it was done when device open */

		errs	+= dev_input_format	(-1, p16aiss8ao4->fd, p16aiss8ao4->input_format	);

		drv16aiss8ao4_set_GPIO_OUT(pSTDdev, 0, 0); /* FALSE, default input */
		drv16aiss8ao4_set_GPIO_OUT(pSTDdev, 1, 0);


		errs	+= dev_io_timeout(-1, p16aiss8ao4->fd, READ_TIME_OUT);


		errs	+= dev_io_output_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->out_io_mode);
		errs	+= dev_output_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->out_mode);
		errs	+= dev_output_channels	(-1, p16aiss8ao4->fd, p16aiss8ao4->out_ch_mask);
		errs	+= dev_output_simultaneous	(-1, p16aiss8ao4->fd, p16aiss8ao4->out_simultaneous);
		errs	+= dev_output_clock_initiator_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->out_clock_initiator_mode);
		errs	+= dev_output_range(-1, p16aiss8ao4->fd, p16aiss8ao4->out_range);		
		errs	+= dev_output_burst_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->out_burst_enable);
		errs	+= dev_rate_gen_c_ndiv(-1, p16aiss8ao4->fd, 0, p16aiss8ao4->out_rate_c, p16aiss8ao4->out_clock_c);
		errs	+= dev_rate_gen_c_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->out_rate_gen_c_enable);
		errs	+= dev_output_clock_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->out_clock_enable);
		errs	+= dev_output_buffer_threshold(-1, p16aiss8ao4->fd, p16aiss8ao4->out_buffer_threshold);
		errs	+= dev_output_circular_buffer_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->out_circular_buffer_enable);
	/*	errs	+= dev_auto_calibrate(-1, p16aiss8ao4->fd); */
		errs	+= dev_output_buffer_clear(-1, p16aiss8ao4->fd);

		_init_data_2(p16aiss8ao4->out_ch_mask, p16aiss8ao4->out_rate_c, p16aiss8ao4->out_period);
	
#endif

#if 1
	
		errs	+= dev_input_buffer_fill(-1, p16aiss8ao4->fd, FALSE);
		errs	+= dev_input_buffer_overflow_detect(-1, p16aiss8ao4->fd, FALSE);
//		errs	+= dev_io_timeout(-1, p16aiss8ao4->fd, 10);
		errs	+= dev_io_input_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->in_io_mode);

//		val = FALSE;
//		ioctl(p16aiss8ao4->fd, IOCTL_DEVICE_SET_TRIGGER_INITIATOR_MODE, &val);

	
//		errs	+= dev_initialize					(-1, p16aiss8ao4->fd);
		errs	+= dev_input_channels(-1, p16aiss8ao4->fd, p16aiss8ao4->in_ch_mask);
		errs	+= dev_input_clock_initiator_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->in_clock_initiator_mode);
		errs	+= dev_input_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->in_mode);
		errs	+= dev_input_range_a(-1, p16aiss8ao4->fd, p16aiss8ao4->in_range_a);
		errs	+= dev_input_range_b(-1, p16aiss8ao4->fd, p16aiss8ao4->in_range_b);
		errs	+= dev_input_format	(-1, p16aiss8ao4->fd, DATA_FORMAT_OFFSET_BINARY);
		errs	+= dev_input_burst_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->in_burst_enable);
		errs	+= dev_rate_gen_a_ndiv(-1, p16aiss8ao4->fd, 0, p16aiss8ao4->in_rate_a, p16aiss8ao4->in_clock_a);
		errs	+= dev_rate_gen_a_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->in_rate_gen_a_enable);
		errs	+= dev_input_buffer_threshold(-1, p16aiss8ao4->fd, p16aiss8ao4->in_buffer_threshold);
		errs	+= dev_input_buffer_enable(-1, p16aiss8ao4->fd, p16aiss8ao4->in_buffer_enable);
//		errs	+= dev_auto_calibrate(-1, p16aiss8ao4->fd);
		errs	+= dev_input_buffer_clear (-1, p16aiss8ao4->fd);
#endif


		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(p16aiss8ao4) { */

	notify_refresh_master_status();


//	epicsAtExit((VOIDFUNCPTREXIT) exit_16aiss8ao4_Task, NULL);
	

	printf("****** 16aiss8ao4 local device init OK!\n");

	return 0;
}


static long drv16aiss8ao4_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_16aiss8ao4 *p16aiss8ao4;
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
		p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", p16aiss8ao4->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", p16aiss8ao4->adjCount_smplRate,
					                                                 p16aiss8ao4->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", p16aiss8ao4->adjCount_Gain,
					                                                   p16aiss8ao4->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(p16aiss8ao4->pchannelConfig)>0) print_channelConfig(p16aiss8ao4,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

#if 0
static void exit_16aiss8ao4_Task(void)
{
	printf("call get_first_STDdev()\n");
	ST_STD_device *pSTDdev = get_first_STDdev();
	if( !pSTDdev ) {
		printf("Hook: exit 16aiss8ao4  get STDdev failed!\n"); return;
		}
	
	printf("call exit 16aiss8ao4.. \n");
	while(pSTDdev) 
	{
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
		{
			clear_16aiss8ao4_taskConfig(pSTDdev, 0,0);
			free(pSTDdev->pUser);
		}
		
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	return;
}
#endif

int drv16aiss8ao4_AO_trigger(ST_STD_device *pSTDdev)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	dev_output_buffer_clear(-1, p16aiss8ao4->fd);
	epicsEventSignal( p16aiss8ao4->epicsEvent_FGthread);
	return WR_OK;
}
int drv16aiss8ao4_AI_trigger(ST_STD_device *pSTDdev)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	dev_input_buffer_clear (-1, p16aiss8ao4->fd);
	epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId );	
	return WR_OK;
}

int drv16aiss8ao4_dio_out(ST_STD_device *pSTDdev, int ch, uint32 value)
{
	int state;
	uint16 chMask;
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	
	chMask = (0x1 << ch);

	if( value ) p16aiss8ao4->u16GPIO |= chMask;
	else  p16aiss8ao4->u16GPIO &= ~chMask;
	 	
	if( ch < 8) {
		state = gsc_dio_0_out(p16aiss8ao4->fd, p16aiss8ao4->u16GPIO & 0xFF);
	} 
	else if( ch < 16) {
		state = gsc_dio_1_out(p16aiss8ao4->fd, p16aiss8ao4->u16GPIO >> 8);
	} else { 
		printf(">>>>> ERROR. drv16aiss8ao4_dio_out() wrong channel: %d\n", ch );
		state = WR_ERROR;
		}
	return state;
}
#if 0
int drv16aiss8ao4_make_pattern(ST_STD_device *pSTDdev)
{

//	uint16 delta_y, delta_x;
//	float fVal, a;
	int i, x;
	int sec_ch0, sec_ch1, sec_ch2, sec_ch3;
	int total_cnt;
	double dT;
	uint32 hz;
	int32 prev_data0, prev_data1, prev_data2, prev_data3; 
	ST_16aiss8ao4 *pUser = (ST_16aiss8ao4 *)pSTDdev->pUser;

	hz = pUser->out_rate_c;
	dT = 1.0 / (double)hz; /* 0.001, 1KHz */
/*	total_cnt = pUser->out_period * hz; i don't know why it is here */
	
//	memset(_data_out, 0x8000, sizeof(_data_out));
	for (i = 0; i < OUT_BUF_SIZE; i++) _data_out[i]=0x8000;  /* offset binary */
	
	prev_data0 = prev_data1 = prev_data2 = prev_data3 = 0x8000;

	pUser->out_period = pUser->CH0[pUser->num_section-1].x2;
	printf("RMP section count: %d, duration: %d sec\n", pUser->num_section, pUser->out_period);

	total_cnt = pUser->out_period * hz; 

	gsc_pre_calc_pattern(pUser);

	printf("%s: clock rate:%dHz\n", pSTDdev->taskName, hz);
	printf("%s: total count:%d\n", pSTDdev->taskName, total_cnt);

//	for (i = 4, x= 1; x < total_cnt; i += 4, x++)
	for (i = 0, x= 0; x < total_cnt; i += 4, x++)		
	{
		sec_ch0 = drv16aiss8ao4_find_section(pUser, 0, x);
		sec_ch1 = drv16aiss8ao4_find_section(pUser, 1, x);
		sec_ch2 = drv16aiss8ao4_find_section(pUser, 2, x);
		sec_ch3 = drv16aiss8ao4_find_section(pUser, 3, x);

		if( (sec_ch0 < 0) ||(sec_ch1 < 0) ||(sec_ch2 < 0) ||(sec_ch3 < 0) ) {
			return WR_ERROR;
		}
#if 1
		_data_out[i+0] = (uint16)(prev_data0+ pUser->CH0[sec_ch0].delta_hex);
		_data_out[i+1] = (uint16)(prev_data1+ pUser->CH1[sec_ch1].delta_hex);
		_data_out[i+2] = (uint16)(prev_data2+ pUser->CH2[sec_ch2].delta_hex);
		_data_out[i+3] = (uint16)(prev_data3+ pUser->CH3[sec_ch3].delta_hex);

		prev_data0 = _data_out[i+0];
		prev_data1 = _data_out[i+1];
		prev_data2 = _data_out[i+2];
		prev_data3 = _data_out[i+3];
#endif		
				
#if 0
		_data_out[i+0] = prev_data0 + ( 0x8000 *  (( pUser->CH0[sec_ch0].a * (x*0.001)) / 10));
		_data_out[i+1] = (__u16)( 0x8000 *  (( pUser->CH1[sec_ch1].a * (x*0.001)) / 10));
		_data_out[i+2] = (__u16)( 0x8000 *  (( pUser->CH2[sec_ch2].a * (x*0.001)) / 10));
		_data_out[i+3] = (__u16)( 0x8000 *  (( pUser->CH3[sec_ch3].a * (x*0.001)) / 10));
#endif

	}

#if 0
	for (i = 0, x= 0; x < 5; i += 4, x++)
	{
		printf("_data1[%d]: 0x%x, dHex[0]:0x%x\n", i, _data_out[i], pUser->CH0[0].delta_hex);
//		printf("_data2[%d]: 0x%x\n", i+1, _data_out[i+1]);		
	}
	for (i = 3990, x= 0; x < 5; i += 4, x++)
	{
		printf("_data1[%d]: 0x%x, dHex[1]:0x%x\n", i, _data_out[i], pUser->CH0[1].delta_hex);
//		printf("_data2[%d]: 0x%x\n", i+1, _data_out[i+1]);		
	}
	for (i = 19960, x= 4990; x < total_cnt+15; i += 4, x++)
	{
		printf("_data1[%d]: 0x%x, dHex[2]:0x%x\n", i, _data_out[i], pUser->CH0[2].delta_hex);
//		printf("_data2[%d]: 0x%x\n", i+1, _data_out[i+1]);		
	}
#endif
	
	
	printf("%s: make pattern OK.\n", pSTDdev->taskName);
	return WR_OK;
}
#endif
int drv16aiss8ao4_make_pattern_2(ST_STD_device *pSTDdev)
{
//	uint16 delta_y, delta_x;
//	float fVal, a;
	int i, ch, x, x1[4];
	int sec_ch[4];
	int total_cnt;
	double dT;
	double T_curr[4];
	double V_curr[CNT_AO_CH];
	uint32 hz;
	uint32 run_time;

	uint8 prev_sect[4];
	ST_16aiss8ao4 *pUser = (ST_16aiss8ao4 *)pSTDdev->pUser;

	hz = pUser->out_rate_c;
	dT = 1.0 / (double)hz; /* 0.001, 1KHz */

	
//	memset(_data_out, 0x8000, sizeof(_data_out));
	for (i = 0; i < OUT_BUF_SIZE; i++) _data_out[i]=0x8000;  /* offset binary */
/*	
	pUser->out_period = pUser->CH0[pUser->num_section-1].x2;
	pUser->in_period = pUser->out_period + 1;
*/
	run_time = (unsigned int)pSTDdev->ST_Base.dRunPeriod;
//	printf("RMP section count: %d, out_period: %ds, in_period: %ds\n", pUser->num_section, pUser->out_period, pUser->in_period);
	printf("RMP section count: %d, out_period: %ds, in_period: %ds\n", pUser->num_section, run_time, run_time);

	total_cnt = run_time * hz; 


	_init_data_2(pUser->out_ch_mask, pUser->out_rate_c, run_time);
	

	gsc_pre_calc_pattern(pUser);

	printf("%s: clock rate:%dHz\n", pSTDdev->taskName, hz);
	printf("%s: total count:%d\n", pSTDdev->taskName, total_cnt);

	for(i=0; i<4; i++) {
		prev_sect[i] = 0;
		x1[i] = 0;
	}
//	for (i = 4, x= 1; x < total_cnt; i += 4, x++)
	for (i = 0, x= 0; x < total_cnt; i += 4, x++)		
	{
		sec_ch[0] = drv16aiss8ao4_find_section(pUser, 0, x);
		sec_ch[1] = drv16aiss8ao4_find_section(pUser, 1, x);
		sec_ch[2] = drv16aiss8ao4_find_section(pUser, 2, x);
#if USE_LAST_CH4_AO		
		sec_ch[3] = drv16aiss8ao4_find_section(pUser, 3, x);
#endif

		if( (sec_ch[0] < 0) ||(sec_ch[1] < 0) ||(sec_ch[2] < 0) ||(sec_ch[3] < 0) ) {
			return WR_ERROR;
		}

		for(ch=0; ch<4; ch++) {
			if( prev_sect[ch] != sec_ch[ch] )  x1[ch] = 0;

			T_curr[ch] = x1[ch]*dT;
		}
/*
		if( prev_sect[0] != sec_ch[0] )  x1[0] = 0;
		if( prev_sect[0] != sec_ch[0] )  x1[0] = 0;
		if( prev_sect[0] != sec_ch[0] )  x1[0] = 0;
		if( prev_sect[0] != sec_ch[0] )  x1[0] = 0;

		T_curr = x1*dT;
*/
		V_curr[0] = (sec_ch[0] == 0) ? pUser->CH0[sec_ch[0]].a * T_curr[0] : pUser->CH0[sec_ch[0]].a * T_curr[0] + pUser->CH0[sec_ch[0]-1].y2;
		V_curr[1] = (sec_ch[1] == 0) ? pUser->CH1[sec_ch[1]].a * T_curr[1] : pUser->CH1[sec_ch[1]].a * T_curr[1] + pUser->CH1[sec_ch[1]-1].y2;
		V_curr[2] = (sec_ch[2] == 0) ? pUser->CH2[sec_ch[2]].a * T_curr[2] : pUser->CH2[sec_ch[2]].a * T_curr[2] + pUser->CH2[sec_ch[2]-1].y2;
		V_curr[3] = (sec_ch[3] == 0) ? pUser->CH3[sec_ch[3]].a * T_curr[3] : pUser->CH3[sec_ch[3]].a * T_curr[3] + pUser->CH3[sec_ch[3]-1].y2;
		

		for(ch=0; ch<4; ch++) {
			prev_sect[ch] = sec_ch[ch];
			x1[ch]++; 
		}
		
		_data_out[i+0] = (uint16) (  ( V_curr[0] + 10. ) * (double)0xffff /20. ); /* 3276 Bottom  */
		_data_out[i+1] = (uint16) (  ( V_curr[1] + 10. ) * (double)0xffff /20. ); /* 3276 Middle   */
		_data_out[i+2] = (uint16) (  ( V_curr[2] + 10. ) * (double)0xffff /20. ); /* 3276 Top       */
		_data_out[i+3] = (uint16) (  ( V_curr[3] + 10. ) * (double)0xffff /20. ); /* 3276 --          */
		

	}

	printf("%s: make pattern OK.\n", pSTDdev->taskName);
	return WR_OK;
}

int drv16aiss8ao4_copy_data(int ch, int cnt,  unsigned short *data)
{
	int i, j;

	for(i=0, j=0; i< cnt; i++, j +=4) 
	{
		data[i] = (uint16)_data_out[j+ch];
	}

	return WR_OK;
}

int drv16aiss8ao4_copy_to_waveform(ST_STD_device *pSTDdev, int ch, void *vptr)
{
	unsigned int i;
	waveformRecord *pwf = (waveformRecord *)vptr;
	
//	scanIoRequest(pSTDdev->ioScanPvt_userCall1);
//	epicsPrintf("pwf->nelm: %ld\n", pwf->nelm );
	for( i=0; i< pwf->nelm; i++) {
		switch( pwf->ftvl ) {
			case DBF_DOUBLE:
//				pm_Mask[i] = (unsigned int)*( (double*)( pwf->bptr) + i);
				*( (double*)( pwf->bptr) + i) = i;
//				epicsPrintf("%d: %f \n", i, (double)*( (double*)( pwf->bptr) + i));
				break;
			case DBF_FLOAT:
				*( (float*)( pwf->bptr) + i) = (float)i;
//				printf("%d: %f \n", i, (float)*( (float*)( pwf->bptr) + i));
				break;
			default: epicsPrintf("pwf->ftvl(%d) not suppoerted!\n", pwf->ftvl ); break;
		}
/*		epicsThreadSleep(.05); */
/*		epicsPrintf("%d: %lf \n", nval, *( (double*)( pwf->bptr + nval ) ) ); */
	}

	return WR_OK;
}

int drv16aiss8ao4_copy_to_wf_pattern_x(ST_STD_device *pSTDdev, int psID, void *vptr)
{
	unsigned int i;
	ST_16aiss8ao4 *pUser = (ST_16aiss8ao4 *)pSTDdev->pUser;
	waveformRecord *pwf = (waveformRecord *)vptr;
	
//	scanIoRequest(pSTDdev->ioScanPvt_userCall1);
//	epicsPrintf("pwf->nelm: %ld\n", pwf->nelm );
	for( i=0; i< pwf->nelm; i++) {
		switch( pwf->ftvl ) {
			case DBF_DOUBLE:
//				pm_Mask[i] = (unsigned int)*( (double*)( pwf->bptr) + i);
				*( (double*)( pwf->bptr) + i) = i;
//				epicsPrintf("%d: %f \n", i, (double)*( (double*)( pwf->bptr) + i));
				break;
			case DBF_FLOAT: 
				if( psID == ID_PS_BOTTOM ) {
					*( (float*)( pwf->bptr) + i) = (float)pUser->CH0[i].x2 - TIME_FIX_LTU_START;
//				printf("%d: %f \n", i, (float)*( (float*)( pwf->bptr) + i));
				}
				else if	(psID == ID_PS_MIDDLE) {
					*( (float*)( pwf->bptr) + i) = (float)pUser->CH1[i].x2 - TIME_FIX_LTU_START;
				}
				else if (psID == ID_PS_TOP) {
					*( (float*)( pwf->bptr) + i) = (float)pUser->CH2[i].x2 - TIME_FIX_LTU_START;
				}
	
				break;
			default: epicsPrintf("pwf->ftvl(%d) not suppoerted!\n", pwf->ftvl ); break;
		}
/*		epicsThreadSleep(.05); */
/*		epicsPrintf("%d: %lf \n", nval, *( (double*)( pwf->bptr + nval ) ) ); */
	}

	return WR_OK;

}
int drv16aiss8ao4_copy_to_wf_pattern_y(ST_STD_device *pSTDdev, int psID, void *vptr)
{
	unsigned int i;
	ST_16aiss8ao4 *pUser = (ST_16aiss8ao4 *)pSTDdev->pUser;
	waveformRecord *pwf = (waveformRecord *)vptr;
	
	for( i=0; i< pwf->nelm; i++) {
		switch( pwf->ftvl ) {
			case DBF_DOUBLE:
//				pm_Mask[i] = (unsigned int)*( (double*)( pwf->bptr) + i);
				*( (double*)( pwf->bptr) + i) = i;
//				epicsPrintf("%d: %f \n", i, (double)*( (double*)( pwf->bptr) + i));
				break;
			case DBF_FLOAT: 
				if( psID == ID_PS_BOTTOM ) {
					*( (float*)( pwf->bptr) + i) = pUser->CH0[i].y2 * pUser->f32MaxCurrent / 10.0;
//				printf("%d: %f \n", i, (float)*( (float*)( pwf->bptr) + i));
				}
				else if	(psID == ID_PS_MIDDLE) {
					*( (float*)( pwf->bptr) + i) = pUser->CH1[i].y2 * pUser->f32MaxCurrent / 10.0;
				}
				else if (psID == ID_PS_TOP) {
					*( (float*)( pwf->bptr) + i) = pUser->CH2[i].y2 * pUser->f32MaxCurrent / 10.0;
				}
	
				break;
			default: epicsPrintf("pwf->ftvl(%d) not suppoerted!\n", pwf->ftvl ); break;
		}
	}

	return WR_OK;

}




int gsc_dio_0_out(int fd, char data)
{
	int		errs;
	uint32 val = data;
	errs	= ioctl(fd, IOCTL_DEVICE_WRITE_DIO_0, &val);
/*	printf("dio_0 write : 0x%x\n", val); */
	errs	= (errs == -1) ? WR_ERROR: WR_OK;
	return(errs);
}

int gsc_dio_1_out(int fd, char data)
{
	int		errs;
	uint32 val = data;
	errs	= ioctl(fd, IOCTL_DEVICE_WRITE_DIO_1, &val);
/*	printf("dio_1 write : 0x%x\n", val); */
	errs	= (errs == -1) ? WR_ERROR: WR_OK;
	return(errs);
}

int gsc_dio_0_in(int fd, uint32 *data)
{
	int		errs;
	uint32 val;
	errs	= ioctl(fd, IOCTL_DEVICE_READ_DIO_0, &val);
	*data	= (uint32) (0xFF & val);
//	printf("gsc_dio_0_in : 0x%x\n", *data);
	errs	= (errs == -1) ? WR_ERROR: WR_OK;
	return(errs);
}

int gsc_dio_1_in(int fd, uint32 *data)
{
	int		errs;
	uint32 val;
	errs	= ioctl(fd, IOCTL_DEVICE_READ_DIO_1, &val);
/*	printf("dio_1 write : 0x%x\n", val); */
	*data	= (__u8) (0xFF & val);
	errs	= (errs == -1) ? WR_ERROR: WR_OK;
	return(errs);
}



/*
void _init_data(uint32 mask, long rate, int seconds)
{
	#define	PIE		(3.1415962)

	__u32*		dest;
	int			i;
	long		period_len;	// The length of the pattern in samples.
	long		period_tot;	// Pattern samples total for all channels.
	long		period;		// Pattern length in x units.
	int			samp;
	float		samp_dat;	// Total sample size of the data buffer.
	float		samp_sec;	// Samples Per Second for all active channels
	float		samp_tot;	// Samples Total for all active channels
	const char*	units;
	int chans;

//	FILE *fp;
//	char buf[64];

	chans	= ((mask & AO_CH_0_ENABLE) ? 1 : 0)
			+ ((mask & AO_CH_1_ENABLE) ? 1 : 0)
			+ ((mask & AO_CH_2_ENABLE) ? 1 : 0)
			+ ((mask & AO_CH_3_ENABLE) ? 1 : 0);
	

	label("Initializing Data");
	memset(_data_out, 0, sizeof(_data_out));
	samp_sec	= ((float) rate) * chans;
	samp_tot	= ((float) rate) * chans * seconds;
	samp_dat	= sizeof(_data_out) / 4;

	period_len	= (rate <= 100) ? rate				//      10-      100 ->    10-   100
				: (rate <= 1000) ? (rate / 2)		//     101-    1,000 ->    50-   500
				: (rate <= 10000) ? (rate / 5)		//   1,001-   10,000 ->   200- 2,000
				: (rate <= 100000) ? (rate / 10)	//  10,001-  100,000 -> 1,000-10,000
				: (rate / 100);						// 100,001-1,000,000 -> 1,000-10,000
	period_tot	= period_len * chans;
	period		= (long) (1000000000. / rate * period_len);
	units		= "ns";

	if ((period % 1000) == 0)
	{
		period	/= 1000;
		units	= "us";
	}

	if ((period % 1000) == 0)
	{
		period	/= 1000;
		units	= "ms";
	}

	if (samp_tot <= ARRAY_ELEMENTS(_data_out))
	{
		// We can get the entire image in the buffer.
		_loops	= 1;
		_qty	= (long) samp_tot;
	}
	else if (samp_sec <= ARRAY_ELEMENTS(_data_out))
	{
		// We can get one second of the image in the buffer.
		_loops	= seconds;
		_qty	= (long) samp_sec;
	}
	else if (period_tot <= ARRAY_ELEMENTS(_data_out))
	{
		// We can get one period of the image in the buffer.
		_loops	= seconds * (samp_sec / period_tot);
		_qty	= period_tot;
	}
	else
	{
		// We can get only a fraction of a period of the image in the buffer.
		_qty	= (samp_dat / chans) * chans;
		_loops	= (samp_tot + _qty - 1) / _qty;
	}

	// Initialize the data buffer.
	// The data is encoded in OFFSET BINARY format.
	dest	= _data_out;;

//	fp = fopen("output.txt", "w");
//	if( fp == NULL)  {
//		printf("'output.txt' open error!!!!!!\n");
//		return;
//	}

	printf("rate:%lu, period:%ds\n", rate, seconds);
	printf("_loops: %lu, _qty:%lu \n", _loops, _qty );

	for (i = 0, samp = 0; i < _qty; i += chans, samp++)
	{
		samp	= (samp < period_len) ? samp : 0;

//		sprintf(buf, "%d: ", samp);

		// 1st channel: sawtooth wave
		*dest++	= (__u16) (0xFFFF - (0xFFFF * samp / (period_len - 1)));

//		strcat(buf, "%d ", *dest

		// 2nd channel: ramp wave
		if (chans >= 2)
			*dest++	= (__u16) (0xFFFF * samp / (period_len - 1));

		// 3rd channel: square wave
		if (chans >= 3)
			*dest++	= (samp < (period_len / 2)) ? 0xFFFF : 0;

		// 4th channel: sine wave
		if (chans >= 3)
			*dest++	= ((long) 0x7FFF) * (1 + sin((2 * PIE / period_len) * samp));
	}

	printf("DONE   (pattern length: %ld samples, %ld %s)\n", period_len, period, units);
}
*/
void _init_data_2(uint32 mask, long rate, int seconds)
{
	long		period_len;	// The length of the pattern in samples.
	long		period_tot;	// Pattern samples total for all channels.
	long		period;		// Pattern length in x units.
	float		samp_dat;	// Total sample size of the data buffer.
	float		samp_sec;	// Samples Per Second for all active channels
	float		samp_tot;	// Samples Total for all active channels
	const char*	units;
	int chans;

	chans	= ((mask & AO_CH_0_ENABLE) ? 1 : 0)
			+ ((mask & AO_CH_1_ENABLE) ? 1 : 0)
			+ ((mask & AO_CH_2_ENABLE) ? 1 : 0)
			+ ((mask & AO_CH_3_ENABLE) ? 1 : 0);
	

	label("Initializing Data");

	samp_sec	= ((float) rate) * chans;
	samp_tot	= ((float) rate) * chans * seconds;
	samp_dat	= sizeof(_data_out) / 4;

	period_len	= (rate <= 100) ? rate				//      10-      100 ->    10-   100
				: (rate <= 1000) ? (rate / 2)		//     101-    1,000 ->    50-   500
				: (rate <= 10000) ? (rate / 5)		//   1,001-   10,000 ->   200- 2,000
				: (rate <= 100000) ? (rate / 10)	//  10,001-  100,000 -> 1,000-10,000
				: (rate / 100);						// 100,001-1,000,000 -> 1,000-10,000
	period_tot	= period_len * chans;
	period		= (long) (1000000000. / rate * period_len);
	units		= "ns";

	if ((period % 1000) == 0)
	{
		period	/= 1000;
		units	= "us";
	}

	if ((period % 1000) == 0)
	{
		period	/= 1000;
		units	= "ms";
	}

	if (samp_tot <= ARRAY_ELEMENTS(_data_out))
	{
		printf(" We can get the entire image in the buffer.\n");
		_loops	= 1;
		_qty	= (long) samp_tot;
	}
	else if (samp_sec <= ARRAY_ELEMENTS(_data_out))
	{
		printf("  We can get one second of the image in the buffer.\n");
		_loops	= seconds;
		_qty	= (long) samp_sec;
	}
	else if (period_tot <= ARRAY_ELEMENTS(_data_out))
	{
		printf("  We can get one period of the image in the buffer.\n");
		_loops	= seconds * (samp_sec / period_tot);
		_qty	= period_tot;
	}
	else
	{
		printf("  We can get only a fraction of a period of the image in the buffer.\n");
		_qty	= (samp_dat / chans) * chans;
		_loops	= (samp_tot + _qty - 1) / _qty;
	}
	
	printf("DONE   (pattern length: %ld samples, %ld %s), _loops:%ld, _qty:%ld\n", period_len, period, units, _loops, _qty);
}



/******************************************************************************
*
*	Function:	_output_buffer_status
*
*	Purpose:
*
*		Get the status of the output buffer.
*
*	Arguments:
*
*		fd			The handle for the board to access.
*
*		empty		Indicate here if the buffer is empty.
*
*		overflow	Indicate here if there was an overflow.
*
*	Returned:
*
*		>= 0		The number of errors seen.
*
******************************************************************************/

static int _output_buffer_status(int fd, int* empty, int* overflow)
{
	int		errs;
	int		status;
	__u32	val		= TRUE;

	status		= ioctl(fd, IOCTL_DEVICE_GET_OUT_BUFF_STATUS, &val);
	errs		= (status == -1) ? 1 : 0;
	empty[0]	= (val & 0x00001000) ? 1 : 0;
	overflow[0]	= (val & 0x00010000) ? 1 : 0;
	return(errs);
}

/******************************************************************************
*
*	Function:	_io_write
*
*	Purpose:
*
*		Write the data to the device.
*
*	Arguments:
*
*		fd		This is the handle used to access the device.
*
*		chans	The number of channels being used.
*
*		errs	This is the number of errors seen so far. We don't do the
*				work if errors have been see.
*
*	Returned:
*
*		>= 0	The number of errors seen.
*
******************************************************************************/

int _io_write(ST_STD_device *pSTDdev)
{
	time_t	begin;
	int		empty;
	time_t	end;
	long	loop;
	int		over;
	char*	ptr;
	int		secs;
	int		send=0;
	int		sent=0;
	long	tmp;
	__u32	zero[4]	= { 0x8000, 0x8000, 0x8000, 0x8000 };
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	

	label("Writing Data");

	int fd, chans, errs=0;
	
	fd = p16aiss8ao4->fd;
	chans	= ((p16aiss8ao4->out_ch_mask & AO_CH_0_ENABLE) ? 1 : 0)
			+ ((p16aiss8ao4->out_ch_mask & AO_CH_1_ENABLE) ? 1 : 0)
			+ ((p16aiss8ao4->out_ch_mask & AO_CH_2_ENABLE) ? 1 : 0)
			+ ((p16aiss8ao4->out_ch_mask & AO_CH_3_ENABLE) ? 1 : 0);

	for (;;)	// A convenience loop.
	{
		if (errs)
		{
			errs	= 0;
			printf("SKIPPED   (due to errors)\n");
			break;
		}

		errs	= 0;
		begin	= time(NULL);

		// Write the data to the device.
		send	= chans * 4;
		tmp		= write(fd, zero, send);
		errs	+= (tmp != send) ? 1 : 0;

		for (loop = 0; loop < _loops; loop++)
		{
			send	= _qty * 4;
			ptr		= (char*) &_data_out;

			for (;;)
			{

				sent	= write(fd, ptr, send);


				if ((sent == -1) || (sent == send))
					break;

				ptr		+= sent;
				send	-= sent;
			}

			if (sent == -1)
				break;
		}

		send	= chans * 4;
		tmp		= write(fd, zero, send);
		errs	+= (tmp != send) ? 1 : 0;

		// Wait for the output buffer to empty.
#if 1
		for (;;)
		{
			errs	= _output_buffer_status(fd, &empty, &over);

			if ((errs) || (empty) || (over))
				break;
			epicsThreadSleep(0.2);
		}
#endif
		// Compute the results.

		end		= time(NULL);

		if (sent == -1)
		{
			errs	= 1;
			printf("FAIL <---  (write error)\n");
		}
		else if (over)
		{
			errs	= 1;
			printf("FAIL <---  (overflow)\n");
		}
		else
		{
			float	bps;
			float	bpcps;
			float	total;
			float	spcps;

			secs	= end - begin;
			total	= 4.0 * _loops * _qty;
			bps		= total / secs;
			bpcps	= bps / chans;
			spcps	= bpcps / 4;
			printf("PASS -- write. (%2d seconds, %.0f bytes, %.0f B/S, %.0f B/S/C, %.0f S/S/C)\n", secs, total, bps, bpcps, spcps);
		}

		break;
	}

	return(errs);
}

/******************************************************************************
*
*	Function:	_io_read
*
*	Purpose:
*
*		Read data from the device.
*
*	Arguments:
*
*		fd		This is the handle used to access the device.
*
*		chans	The number of enabled channels.
*
*		seconds	Read data for this many seconds.
*
*		errs	This is the number of errors seen so far. We don't do the
*				work if errors have been see.
*
*	Returned:
*
*		>= 0	The number of errors seen.
*
******************************************************************************/
//static	__u32	_data_in[128 * 1024];


int _io_read(ST_STD_device *pSTDdev)
{
	__u32		bcr;
	time_t		begin=0;
	int			get;
	int			got;
	time_t		now=0;
	long long	rx_total	= 0;	
	time_t		t_limit;
	int isFirst = 1;
	uint32 sndCnt = 0;

	int fd,  chans,  seconds,  errs=0;
	double cutoff_time = 0;

	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;


	ST_STD_device *pSTDdevRfm;
	ST_RFM5565 *pRFM5565;
	RFM2G_STATUS   result;
	RFM2G_UINT32   buffer[3];
	float vltg_cmd[3];
	uint32 val_binary[3];
	pSTDdevRfm = (ST_STD_device *)get_STDev_from_type(STR_DEVICE_TYPE_2);
	if(!pSTDdevRfm->pUser) {
		printf("can't get RFM node pointer! bye!.\n");
		return(errs);
	}
	pRFM5565 = (ST_RFM5565 *)pSTDdevRfm->pUser;
	


	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode = NULL;

	float conv = 10.0 / p16aiss8ao4->f32MaxCurrent;  /* INIT_MAX_CURRENT == 6KA*/

	
	fd = p16aiss8ao4->fd;
	chans	= ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_0) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_1) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_2) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_3) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_4) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_5) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_6) ? 1 : 0)
			+ ((p16aiss8ao4->in_ch_mask & ENABLE_INPUT_7) ? 1 : 0);
	
//	seconds = p16aiss8ao4->in_period;
	seconds = (int)pSTDdev->ST_Base.dRunPeriod;

	label("Reading Data");

	for (;;)	// A convenience loop.
	{
		if (errs)
		{
			errs	= 0;
			printf("SKIPPED   (due to errors)\n");
			break;
		}

		errs	= 0;
		isFirst = 1;

		// Read the data from the device.
		get		= sizeof(queueData.pNode->data);
		t_limit	= time(NULL) + seconds+2;

		printf(" sizeof(_datat_in): %d\n", get);
		printf(" %s: try to gethering data for %ds plus 2s\n", pSTDdev->taskName, seconds);
		dev_input_buffer_clear (-1, p16aiss8ao4->fd);
		
		for (;;)
		{
			pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
			ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);

			got	= read(fd, pbufferNode->data, get);

//			drv16aiss8ao4_dio_out(pSTDdev, 0, 1);
//			drv16aiss8ao4_dio_out(pSTDdev, 0, 0);

/* if LTU RX clock 10KHz, then 1KHz event generated. becase the BUFLEN is 80 (10 count per single return )*/
/* Is this function move to RingBuf thread ? */
			if( p16aiss8ao4->u8_PCS_control) 
			{
				/* read voltage command from RFM given by PCS, then write to the GSC register
				output voltage = conv * given value from RFM */

				result = RFM2gRead( pRFM5565->Handle, RM_RMP_ICMD_TOP, buffer, sizeof(RFM2G_UINT32)*3 );
				vltg_cmd[0] = (0xffff & buffer[0]) * conv; /* top */
				vltg_cmd[1] = (0xffff & buffer[1]) * conv; /* middle */
				vltg_cmd[2] = (0xffff & buffer[2]) * conv; /* bottom */

				if( p16aiss8ao4->u32CmdCurrentLimit < vltg_cmd[0] ||
					p16aiss8ao4->u32CmdCurrentLimit < vltg_cmd[1] ||
					p16aiss8ao4->u32CmdCurrentLimit < vltg_cmd[2] )
				{
					cutoff_time += (1.0 / (double)p16aiss8ao4->out_rate_c );
					if( cutoff_time > p16aiss8ao4->d32CurrentLimitDuration ) {
						kfwPrint(0, "Input command limit error!T:%d, M:%d, B: %d d-time: %f\n", vltg_cmd[0], vltg_cmd[1], vltg_cmd[2], cutoff_time);
						drv16aiss8ao4_dio_call_stop();
					}
				} else
					cutoff_time = 0;

				val_binary[0] = (unsigned int)(((vltg_cmd[2] + 10.0) * 0xffff) / 20.0 );
				val_binary[1] = (unsigned int)(((vltg_cmd[1] + 10.0) * 0xffff) / 20.0 );
				val_binary[2] = (unsigned int)(((vltg_cmd[0] + 10.0) * 0xffff) / 20.0 );

				reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_0_REG, val_binary[0]); /* bottom */
				reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_1_REG, val_binary[1]); /* middle */
				reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_2_REG, val_binary[2]); /* top */

				/* this is for command data saving to MDS node */
				/* command from PCS  has current value */
				if( sndCnt < (OUT_BUF_SIZE /4 -3) ) {
					_data_out[ (sndCnt*4) +0] = (0xffff & buffer[2]) ; /* bottom */
					_data_out[ (sndCnt*4) +1] = (0xffff & buffer[1]) ; /* middle */
					_data_out[ (sndCnt*4) +2] = (0xffff & buffer[0]) ; /* top */
					_data_out[ (sndCnt*4) +3] = 0 ;
				}
				
			}
			
			
			if( isFirst ) {
				begin	= time(NULL);
				t_limit	= time(NULL) + seconds + 2;
				isFirst = 0;
				pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				pSTDdev->StatusDev |= TASK_IN_PROGRESS;
				notify_refresh_master_status();
			}

			if (got < 0)
			{
				errs	= 1;
				printf("FAIL <---  (read() error, errno %d)\n", errno);
				break;
			}

			if (get != get)
			{
				errs	= 1;
				printf(	"FAIL <---  (read() error, requested %ld bytes"
						", got %ld)\n",
						(long) get,
						(long) got);
				break;
			}

			rx_total	+= got;
			sndCnt++;
			now			= time(NULL);

			queueData.pNode 	= pbufferNode;
			queueData.size	= got;
			queueData.u32Cnt = sndCnt;
			if (now >= t_limit) 
			{
				queueData.opcode	= QUEUE_OPCODE_CLOSE_FILE;
				epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));
				break;
			}
			else {
				queueData.opcode	= QUEUE_OPCODE_NORMAL;
				epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));
			}
		}

		// Check for I/O problems.
		errs	+= reg_gsc_read(fd, BOARD_CTRL_REG, &bcr);

		if (errs)
		{
			break;
		}
		else if (bcr & 0x8000)
		{
			errs	= 1;
			printf("FAIL <---  (overflow)\n");
			break;
		}
		else if (bcr & 0x800000)
		{
			errs	= 1;
			printf("FAIL <---  (underflow)\n");
			break;
		}

		// Report the results.
		{
			int			secs;
			float	bps;
			float	bpcps;
			float	total;
			float	spcps;

			secs	= now - begin;
			total	= rx_total;
			bps		= total / secs;
			bpcps	= bps / chans;
			spcps	= bpcps / 4;
			printf("chans: %d\n", chans);
			printf("PASS -- read. (%d seconds, %.0f bytes, %.0f B/S, %.0f B/S/C, %.0f S/S/C)\n", secs, total, bps, bpcps, spcps);
		}

		break;
	}

	printf("send count: %d\n", sndCnt);

	return(errs);
}

void func_16aiss8ao4_OP_MODE(void *pArg, double arg1, double arg2)
{
/*	ST_STD_device* pSTDdev = (ST_STD_device *)pArg; */

	if( (int)arg1 == OPMODE_REMOTE ) {
		db_put("RMP_16ADIO_AI_USE_EXT_CLK", "1");
		db_put("RMP_16ADIO_AO_USE_EXT_CLK", "1");
		db_put("RMP_RUN_PERIOD", "40");
		db_put("RMP_LTU_SET_FREE_RUN", "0");
	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		db_put("RMP_16ADIO_AI_USE_EXT_CLK", "0");
		db_put("RMP_16ADIO_AO_USE_EXT_CLK", "0");
		db_put("RMP_LTU_SET_FREE_RUN", "1");
	}
}

void func_16aiss8ao4_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_16aiss8ao4_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

	drv16aiss8ao4_make_pattern_2(pSTDdev);

}

void func_16aiss8ao4_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	char fname[1024];
	char strBuf[16];
	int i;

	ST_STD_device *pSTDdevRfm;
	ST_RFM5565 *pRFM5565;

	pSTDdevRfm = (ST_STD_device *)get_STDev_from_type(STR_DEVICE_TYPE_2);
	while (!pSTDdevRfm->pUser) {
		printf("wait STD rfm pointer.\n");
		epicsThreadSleep(1.);
	}
	pRFM5565 = (ST_RFM5565 *)pSTDdevRfm->pUser;


	

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		if( db_get("RMP_LTU_P0_SEC0_T0", strBuf) == WR_ERROR ) {
			notify_error(1, "'RMP_LTU_P0_SEC0_T0' get error!\n");
			return ;
		} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[0]);


		for( i=0; i<CNT_AI_CH; i++) 
		{
			sprintf(fname, "%s/raw%d_%d.dat", STR_CHANNEL_DATA_DIR, pSTDdev->BoardID, i );
			p16aiss8ao4->fpRaw[i] = fopen(fname, "wb");
			if( !p16aiss8ao4->fpRaw[i] ) {
				notify_error(1, "'%s' open error!\n", fname );
				return ;
			}
		}
/*		flush_STDdev_to_MDSfetch( pSTDdev );  move to SFW 2013. 6. 19*/

#if 0 /* requested hyunsick ahan. 2013. 7. 2 */
		if( p16aiss8ao4->u8_Link_PS ) 
		{
			db_put("RMP_DIO24_DO_A0:BTM_RDY_ON", "1");
			db_put("RMP_DIO24_DO_B0:MID_RDY_ON", "1");
			db_put("RMP_DIO24_DO_C0:TOP_RDY_ON", "1");
			epicsThreadSleep(0.5);
			db_put("RMP_DIO24_DO_A0:BTM_RDY_ON", "0");
			db_put("RMP_DIO24_DO_B0:MID_RDY_ON", "0");
			db_put("RMP_DIO24_DO_C0:TOP_RDY_ON", "0");
		}
#endif
		/* write RMP coil connection value: n */
		drv16aiss8ao4_rfm_write_coil_mode(p16aiss8ao4, pRFM5565);
		
		/* write RMP current reset */
		drv16aiss8ao4_rfm_reset(pRFM5565);
				

		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;
		
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;
		
		while( pSTDdev->StatusDev & TASK_IN_PROGRESS ) {
			printf("%s: wait for processing done.\n", pSTDdev->taskName);
			epicsThreadSleep(1.0);
		}
		
		for( i=0; i<CNT_AI_CH; i++) 
		{
			if( p16aiss8ao4->fpRaw[i] ) {
				fclose(p16aiss8ao4->fpRaw[i]);
				p16aiss8ao4->fpRaw[i] = NULL;
			}
		}
#if 0
		if( p16aiss8ao4->u8_Link_PS ) 
		{
			db_put("RMP_DIO24_DO_A1:BTM_RDY_OFF", "1");
			db_put("RMP_DIO24_DO_B1:MID_RDY_OFF", "1");
			db_put("RMP_DIO24_DO_C1:TOP_RDY_OFF", "1");
			epicsThreadSleep(0.5);
			db_put("RMP_DIO24_DO_A1:BTM_RDY_OFF", "0");
			db_put("RMP_DIO24_DO_B1:MID_RDY_OFF", "0");
			db_put("RMP_DIO24_DO_C1:TOP_RDY_OFF", "0");
		}
#endif
		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}


}
void func_16aiss8ao4_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;


	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;
		if( p16aiss8ao4->u8_Link_PS ) 
		{
			db_put("RMP_DIO24_DO_A2:BTM_RUN", "1");
			db_put("RMP_DIO24_DO_B2:MID_RUN", "1");
			db_put("RMP_DIO24_DO_C2:TOP_RUN", "1");
			epicsThreadSleep(0.5);
			db_put("RMP_DIO24_DO_A2:BTM_RUN", "0");
			db_put("RMP_DIO24_DO_B2:MID_RUN", "0");
			db_put("RMP_DIO24_DO_C2:TOP_RUN", "0");
		}

		if( pSTDdev->ST_Base_fetch.opMode == OPMODE_LOCAL )
			epicsThreadSleep(0.5);

		if( p16aiss8ao4->u8_PCS_control == 0 )
			drv16aiss8ao4_AO_trigger(pSTDdev);
		drv16aiss8ao4_AI_trigger(pSTDdev);
		
		epicsThreadSleep(.3);
	}
	else {
/*		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return; */
		/* nothing to do */
		if( p16aiss8ao4->u8_Link_PS ) 
		{
			drv16aiss8ao4_dio_call_stop();
		}
		
	}

	
}

void func_16aiss8ao4_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	uint32 prev = p16aiss8ao4->in_rate_a;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;
	p16aiss8ao4->in_rate_a = pSTDdev->ST_Base.nSamplingRate;
	

	dev_rate_gen_a_ndiv(-1, p16aiss8ao4->fd, 0, p16aiss8ao4->in_rate_a, p16aiss8ao4->in_clock_a);

	printf("%s: sampling rate changed from %d to %d.\n", pSTDdev->taskName, prev, p16aiss8ao4->in_rate_a);
}
void func_16aiss8ao4_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
	notify_refresh_master_status();

	
	rmp_put_raw_value(pSTDdev);


	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();

}



int drv16aiss8ao4_find_section(ST_16aiss8ao4 *pUser, int ch, int step)
{
	uint32 hz;
	int section=-1, i;
	hz = pUser->out_rate_c;

//	printf("CH%d:  num of section:%d\n", ch, pUser->num_section);


	if( ch == 0 )
	{
		for( i=0; i < pUser->num_section; i++) 
		{
//			printf("CH%d: i:%d, x2:%.2f,    (step:%d < %f)\n", ch, i, pUser->CH0[i].x2, step, pUser->CH0[i].x2 * hz);
			
			if( step < pUser->CH0[i].x2 * hz ) 
			{ 
				section = i;
				break;
			}
		}
//		printf("CH%d:  %dHz, step:%d, section:%d\n", ch, hz, step, section);
	}
	else if ( ch == 1)
	{
		for( i=0; i < pUser->num_section; i++) 
		{
			if( step < pUser->CH1[i].x2 * hz ) 
			{ 
				section = i;
				break;
			}
		}
//		printf("CH%d:  %dHz, step:%d, section:%d\n", ch, hz, step, section);

	}
	else if ( ch == 2)
	{
		for( i=0; i < pUser->num_section; i++) 
		{
			if( step < pUser->CH2[i].x2 * hz ) 
			{ 
				section = i;
				break;
			}
		}
//		printf("CH%d:  %dHz, step:%d, section:%d\n", ch, hz, step, section);

	}
	else if ( ch == 3)
	{
		for( i=0; i < pUser->num_section; i++) 
		{
			if( step < pUser->CH3[i].x2 * hz ) 
			{ 
				section = i;
				break;
			}
		}
//		printf("CH%d:  %dHz, step:%d, section:%d\n", ch, hz, step, section);

	}
#if 1
	if( section < 0 ) {
		printf("ERROR. Couldn't find right section with given ch, step (%d, %d)\n", ch, step);
	}
#endif

	return section;
	
}

int drv16aiss8ao4_set_section_end_time(ST_STD_device *pSTDdev, int ch, int sect, float time)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	switch(ch) {
		case 0:
			p16aiss8ao4->CH0[sect].x2 = time + TIME_FIX_LTU_START;
			break;
		case 1:
			p16aiss8ao4->CH1[sect].x2 = time + TIME_FIX_LTU_START;
			break;
		case 2:
			p16aiss8ao4->CH2[sect].x2 = time + TIME_FIX_LTU_START;
			break;
		case 3:
			p16aiss8ao4->CH3[sect].x2 = time + TIME_FIX_LTU_START;
			break;
		default:
			printf("%s: ERROR.....set end time input channel (%d) wrong!!!\n", pSTDdev->taskName, ch );
			return WR_ERROR;
		}


	return WR_OK;
}

int drv16aiss8ao4_set_section_end_voltage(ST_STD_device *pSTDdev, int ch, int sect, float vltg)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	switch(ch) {
		case 0:
			p16aiss8ao4->CH0[sect].y2 = vltg;
			break;
		case 1:
			p16aiss8ao4->CH1[sect].y2 = vltg;
			break;
		case 2:
			p16aiss8ao4->CH2[sect].y2 = vltg;
			break;
		case 3:
			p16aiss8ao4->CH3[sect].y2 = vltg;
			break;
		default:
			printf("%s:ERROR....set end voltage input channel (%d) wrong!!!\n", pSTDdev->taskName, ch );
			return WR_ERROR;
		}


	return WR_OK;
}

int drv16aiss8ao4_set_section_end_current(ST_STD_device *pSTDdev, int ch, int sect, float cur)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}
	float conv = 10.0 / p16aiss8ao4->f32MaxCurrent;  /* INIT_MAX_CURRENT == 6KA*/

	switch(ch) {
		case 0:
			p16aiss8ao4->CH0[sect].y2 = cur * conv ;
			break;
		case 1:
			p16aiss8ao4->CH1[sect].y2 = cur * conv ;
			break;
		case 2:
			p16aiss8ao4->CH2[sect].y2 = cur  * conv ;
			break;
		case 3:
			p16aiss8ao4->CH3[sect].y2 = cur  * conv ;
			break;
		default:
			printf("%s:ERROR....set end current input channel (%d) wrong!!!\n", pSTDdev->taskName, ch );
			return WR_ERROR;
		}


	return WR_OK;
}


int drv16aiss8ao4_set_AI_clock_mode(ST_STD_device *pSTDdev, uint8 value)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}
	if( master_is_locked() ) {
		printf("%s: ERROR.....master something doing.\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	p16aiss8ao4->in_clock_initiator_mode = (value == 1) ? CLK_INITIATOR_INPUT : CLK_INITIATOR_OUTPUT ;

	dev_input_clock_initiator_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->in_clock_initiator_mode);

	return WR_OK;
}
int drv16aiss8ao4_set_AO_clock_mode(ST_STD_device *pSTDdev, uint8 value)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}
	if( master_is_locked() ) {
		printf("%s: ERROR.....master something doing.\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	p16aiss8ao4->out_clock_initiator_mode = (value == 1) ? OUT_CLK_INITIATOR_HARDWARE : OUT_CLK_INITIATOR_RATE_C ;

	dev_output_clock_initiator_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->out_clock_initiator_mode);

	return WR_OK;
}


int drv16aiss8ao4_set_AO_mode_registered(ST_STD_device *pSTDdev, uint8 value)
{
	uint32 prev_mode;
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}
	if( master_is_locked() ) {
		printf("%s: ERROR.....master something doing.\n", pSTDdev->taskName);
		return WR_ERROR;
	}
	prev_mode = p16aiss8ao4->out_mode;
	p16aiss8ao4->out_mode = (value == 1) ? OUT_MODE_REGISTER : OUT_MODE_BUFFERED;

	if( ( prev_mode == OUT_MODE_REGISTER) && ( p16aiss8ao4->out_mode == OUT_MODE_BUFFERED) ) {
		reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_0_REG, 0x8000);
		reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_1_REG, 0x8000);
		reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_2_REG, 0x8000);
		reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_3_REG, 0x8000);
	}
	
	dev_output_mode(-1, p16aiss8ao4->fd, p16aiss8ao4->out_mode);

/* 2013. 6. 30. This is error function.  It makes dragged on of output signal.
	p16aiss8ao4->out_simultaneous = (value == 1) ? TRUE : FALSE ;
	dev_output_simultaneous	(-1, p16aiss8ao4->fd, p16aiss8ao4->out_simultaneous);
*/

	return WR_OK;
}


int gsc_pre_calc_pattern(ST_16aiss8ao4 *pUser)
{
	int i;
	double dT;

	dT = 1.0 / (double)pUser->out_rate_c; /* 0.001 = 1KHz*/

/* calculate incline */
	pUser->CH0[0].a = (pUser->CH0[0].y2 - 0.0 ) / (pUser->CH0[0].x2 - 0.0 ) ;
	pUser->CH1[0].a = (pUser->CH1[0].y2 - 0.0 ) / (pUser->CH1[0].x2 - 0.0 ) ;
	pUser->CH2[0].a = (pUser->CH2[0].y2 - 0.0 ) / (pUser->CH2[0].x2 - 0.0 ) ;
	pUser->CH3[0].a = (pUser->CH3[0].y2 - 0.0 ) / (pUser->CH3[0].x2 - 0.0 ) ;
		
	for (i = 1; i < pUser->num_section; i++)
	{
		pUser->CH0[i].a = (pUser->CH0[i].y2 - pUser->CH0[i-1].y2 ) / (pUser->CH0[i].x2 - pUser->CH0[i-1].x2 ) ;
		pUser->CH1[i].a = (pUser->CH1[i].y2 - pUser->CH1[i-1].y2 ) / (pUser->CH1[i].x2 - pUser->CH1[i-1].x2 ) ;
		pUser->CH2[i].a = (pUser->CH2[i].y2 - pUser->CH2[i-1].y2 ) / (pUser->CH2[i].x2 - pUser->CH2[i-1].x2 ) ;
		pUser->CH3[i].a = (pUser->CH3[i].y2 - pUser->CH3[i-1].y2 ) / (pUser->CH3[i].x2 - pUser->CH3[i-1].x2 ) ;
	}

	for (i = 0; i < pUser->num_section; i++)
	{
		pUser->CH0[i].delta_v= pUser->CH0[i].a * dT;
		pUser->CH1[i].delta_v= pUser->CH1[i].a * dT;
		pUser->CH2[i].delta_v= pUser->CH2[i].a * dT;
		pUser->CH3[i].delta_v= pUser->CH3[i].a * dT;

		pUser->CH0[i].delta_hex=(int16)(0x8000*pUser->CH0[i].delta_v/10);
		pUser->CH1[i].delta_hex=(int16)(0x8000*pUser->CH1[i].delta_v/10);
		pUser->CH2[i].delta_hex=(int16)(0x8000*pUser->CH2[i].delta_v/10);
		pUser->CH3[i].delta_hex=(int16)(0x8000*pUser->CH3[i].delta_v/10);
	}
	
	return WR_OK;

}

int gsc_reset_pattern(ST_16aiss8ao4 *p16aiss8ao4)
{
	int i;
	
	p16aiss8ao4->num_section = 3;
	for(i=0; i<MAX_NUM_SECTION; i++){
		p16aiss8ao4->CH0[i].y2= 0.0;
		p16aiss8ao4->CH0[i].x2= 0.0;
		p16aiss8ao4->CH0[i].delta_hex = 0x0;
		p16aiss8ao4->CH0[i].delta_v = 0x0;
	}
	p16aiss8ao4->CH0[0].y2= 5.0;
	p16aiss8ao4->CH0[1].y2= 5.0;	
	p16aiss8ao4->CH0[2].y2= 0.0;
	p16aiss8ao4->CH0[0].x2= 1.0;
	p16aiss8ao4->CH0[1].x2= 3.0;
	p16aiss8ao4->CH0[2].x2= 5.0;

	p16aiss8ao4->CH1[0].y2= 3.0;
	p16aiss8ao4->CH1[1].y2= 3.0;	
	p16aiss8ao4->CH1[2].y2= 0.0;
	p16aiss8ao4->CH1[0].x2= 1.0;
	p16aiss8ao4->CH1[1].x2= 3.0;
	p16aiss8ao4->CH1[2].x2= 5.0;

	p16aiss8ao4->CH2[0].y2= 7.0;
	p16aiss8ao4->CH2[1].y2= 7.0;	
	p16aiss8ao4->CH2[2].y2= 0.0;
	p16aiss8ao4->CH2[0].x2= 1.0;
	p16aiss8ao4->CH2[1].x2= 2.0;
	p16aiss8ao4->CH2[2].x2= 5.0;

	p16aiss8ao4->CH3[0].y2= 3.0;
	p16aiss8ao4->CH3[1].y2= 8.0;	
	p16aiss8ao4->CH3[2].y2= 0.0;
	p16aiss8ao4->CH3[0].x2= 1.0;
	p16aiss8ao4->CH3[1].x2= 4.0;
	p16aiss8ao4->CH3[2].x2= 5.0;

	return WR_OK;
}

/* 
	description: select GPIO input or output
	which: upper or lower
	inout: input or output
*/
int drv16aiss8ao4_set_GPIO_OUT(ST_STD_device *pSTDdev, int which, int inout)
{
	uint32 val ;

	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( p16aiss8ao4 == NULL) {
		printf("%s: ERROR.....no User structure pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	if( inout )
	{
		val		= TRUE;
		if( which ) {
			ioctl(p16aiss8ao4->fd, IOCTL_DEVICE_SET_DIO_OUTPUT_1, &val);
			gsc_dio_1_out(p16aiss8ao4->fd, 0x0);
			p16aiss8ao4->GPIO_1_IO = inout;
		}
		else {
			ioctl(p16aiss8ao4->fd, IOCTL_DEVICE_SET_DIO_OUTPUT_0, &val); /* TRUE */
			gsc_dio_0_out(p16aiss8ao4->fd, 0x0);
			p16aiss8ao4->GPIO_0_IO = inout;
		}
		
	} 
	else {
		val		= FALSE;
		if( which) {
			ioctl(p16aiss8ao4->fd, IOCTL_DEVICE_SET_DIO_OUTPUT_1, &val);
			p16aiss8ao4->GPIO_1_IO = 0;
			}
		else { 
			ioctl(p16aiss8ao4->fd, IOCTL_DEVICE_SET_DIO_OUTPUT_0, &val); /* FALSE */
			p16aiss8ao4->GPIO_0_IO = 0;
			}
	}
	return WR_OK;
	
}

int drv16aiss8ao4_update_coilMode(ST_STD_device *pSTDdev)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	unsigned int  n1000, n100, n10, n1;
	int i;
	
#if 0
	/* before 2012. 9. 28 */
	for( i=0; i<3; i++) {
		n1000 = p16aiss8ao4->u32Mode_a[i] == 0 ? 3 : p16aiss8ao4->u32Mode_a[i] ;
		n100 = p16aiss8ao4->u32Mode_b[i] == 0 ? 3 : p16aiss8ao4->u32Mode_b[i] ;
		n10 = p16aiss8ao4->u32Mode_c[i] == 0 ? 3 : p16aiss8ao4->u32Mode_c[i] ;
		n1 = p16aiss8ao4->u32Mode_d[i] == 0 ? 3 : p16aiss8ao4->u32Mode_d[i] ;
		p16aiss8ao4->u32Mode_PCS[i] = (n1000 * 1000) + (n100 * 100) + (n10*10) + n1;

		p16aiss8ao4->u32Mode_tag[i] = (p16aiss8ao4->u32Mode_a[i] * 1000) + (p16aiss8ao4->u32Mode_b[i] * 100) + (p16aiss8ao4->u32Mode_c[i]*10) + p16aiss8ao4->u32Mode_d[i];
	}
#endif
/* after 2012. 9. 28 */
	for( i=0; i<3; i++) {
		n1000 = p16aiss8ao4->u32Mode_a[i] + 1 ;
		n100 = p16aiss8ao4->u32Mode_b[i] + 1;
		n10 = p16aiss8ao4->u32Mode_c[i] + 1 ;
		n1 = p16aiss8ao4->u32Mode_d[i] + 1;
		p16aiss8ao4->u32Mode_PCS[i] = (n1000 * 1000) + (n100 * 100) + (n10*10) + n1;  /* this value will be write in RFM */

		p16aiss8ao4->u32Mode_tag[i] = (p16aiss8ao4->u32Mode_a[i] * 1000) + (p16aiss8ao4->u32Mode_b[i] * 100) + (p16aiss8ao4->u32Mode_c[i]*10) + p16aiss8ao4->u32Mode_d[i];
	}


	return WR_OK;
}

int drv16aiss8ao4_rfm_reset(void *ptr)
{
	ST_RFM5565 *pRFM5565 = (ST_RFM5565 *)ptr;
	RFM2G_STATUS   result;
	RFM2G_INT32   buffer;

	buffer = 0x8000;
	result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_I, &buffer, sizeof(RFM2G_UINT32)*1 );
	result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_I, &buffer, sizeof(RFM2G_UINT32)*1 );
	result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_I, &buffer, sizeof(RFM2G_UINT32)*1 );
	printf(" ----> RMP current reset to zero \n");
	
	return WR_OK;
}

int drv16aiss8ao4_rfm_write_coil_mode(void *ptr1, void *ptr2)
{
	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)ptr1;
	ST_RFM5565 *pRFM5565 = (ST_RFM5565 *)ptr2;
	RFM2G_STATUS   result;
	RFM2G_UINT32   buffer;
	
	
	buffer = p16aiss8ao4->u32Mode_PCS[ID_PS_TOP];
	result = RFM2gWrite( pRFM5565->Handle, RM_RMP_TOP_MODE, &buffer, sizeof(RFM2G_UINT32)*1 );
	buffer = 0;
	buffer = p16aiss8ao4->u32Mode_PCS[ID_PS_MIDDLE];
	result = RFM2gWrite( pRFM5565->Handle, RM_RMP_MID_MODE, &buffer, sizeof(RFM2G_UINT32)*1 );
	buffer = 0;
	buffer = p16aiss8ao4->u32Mode_PCS[ID_PS_BOTTOM];
	result = RFM2gWrite( pRFM5565->Handle, RM_RMP_BTM_MODE, &buffer, sizeof(RFM2G_UINT32)*1 );

	return WR_OK;
}

void drv16aiss8ao4_dio_call_stop()
{
	db_put("RMP_DIO24_DO_A3:BTM_STOP", "1");
	db_put("RMP_DIO24_DO_B3:MID_STOP", "1");
	db_put("RMP_DIO24_DO_C3:TOP_STOP", "1");
	epicsThreadSleep(0.5);
	db_put("RMP_DIO24_DO_A3:BTM_STOP", "0");
	db_put("RMP_DIO24_DO_B3:MID_STOP", "0");
	db_put("RMP_DIO24_DO_C3:TOP_STOP", "0");
}


