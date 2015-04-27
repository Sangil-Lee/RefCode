#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drvRMCHK.h"
#include "drvRTCORE.h"
/*#include "myMDSplus.h" */



static long drvRMCHK_io_report(int level);
static long drvRMCHK_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvRTMON = {
       2,
       drvRMCHK_io_report,
       drvRMCHK_init_driver
};

epicsExportAddress(drvet, drvRTMON);



int create_RMCHK_taskConfig( ST_STD_device *pSTDdev)
{
	ST_RMCHK* pRMCHK;
	RFM2G_STATUS   result;                 /* Return codes from RFM2Get API calls  */
	RFM2G_UINT32   dmaThreshold;

	
	pRMCHK = (ST_RMCHK *)malloc(sizeof(ST_RMCHK));
	if( !pRMCHK) return WR_ERROR;
	pSTDdev->pUser = pRMCHK;

	
	/* Open the Reflective Memory device */
	result = RFM2gOpen( "/dev/rfm2g0", &pRMCHK->Handle );
	if( result != RFM2G_SUCCESS )
	{
		printf( "ERROR: RFM2gOpen() failed.\n" );
		printf( "ERROR MSG: %s\n", RFM2gErrorMsg(result));
		return(WR_ERROR);
	}

	
	/* Tell the user the NodeId of this device */
	result = RFM2gNodeID( pRMCHK->Handle, &pRMCHK->NodeId );
	if( result != RFM2G_SUCCESS )
	{
		printf( "Could not get the Node ID.\n" );
		printf( "Error: %s.\n\n",  RFM2gErrorMsg(result));
		RFM2gClose( &pRMCHK->Handle );
		return(WR_ERROR);
	}
	printf("rfm2g:  NodeID = 0x%02x\n", pRMCHK->NodeId);

	dmaThreshold = CFG_DMA_THRESHOLD; /* 4096 byte */
	RFM2gSetDMAThreshold(pRMCHK->Handle, dmaThreshold);

	/* Get the DMA Threshold */
	result = RFM2gGetDMAThreshold(pRMCHK->Handle, &dmaThreshold);
	if( result != RFM2G_SUCCESS )
	{
		printf( "Could not get the DMA Threshold.\n" );
		printf( "Error: %s.\n\n",  RFM2gErrorMsg(result));
		RFM2gClose( &pRMCHK->Handle );
		return(WR_ERROR);
	}
	printf("\nRFM2g DMA Test (DMA Threshold is %uByte)\n", dmaThreshold);

	pRMCHK->Offset = INIT_RFM_MAP_OFFSET;
	pRMCHK->Pages = INIT_RFM_MAP_PAGE;
	pRMCHK->Bytes = INIT_RFM_MAP_BYTES;

#if USE_RFM_MMAP
//	result = RFM2gUserMemory(pRMCHK->Handle, (volatile void **)(&outbuffer), pRMCHK->Offset, pRMCHK->Pages);
	result = RFM2gUserMemoryBytes(pRMCHK->Handle, (volatile void **)(&pRMCHK->mapBuf), pRMCHK->Offset, pRMCHK->Bytes);
	if( result != RFM2G_SUCCESS )
	{
		printf( "ERROR: RFM2gUserMemory() failed.\n" );
		printf( "ERROR MSG: %s\n", RFM2gErrorMsg(result));
		/* Close the Reflective Memory device */
		RFM2gClose( &pRMCHK->Handle );
		return(WR_ERROR);
	}
#else
	pRMCHK->mapBuf = (unsigned int *)malloc(INIT_RFM_MAP_BYTES);
	if( pRMCHK->mapBuf == NULL ) printf("------- mapBuf allocation error!\n");
#endif



//	pRMCHK->rtCnt = 0;
//	pRMCHK->cntDAQ = 0;
	pSTDdev->stdDAQCnt = 0;

/*	pRMCHK->daqStepTime = 0.00001; */
	pRMCHK->useScanIoRequest = 0;


/*	pRMCHK->clkStartTime = -15.0; */
	

	return WR_OK;
}


int init_my_sfwFunc_RMCHK( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_RMCHK_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_RMCHK_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_RMCHK_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_RMCHK_SYS_RUN;
//	pSTDdev->ST_Func._SAMPLING_RATE= func_RMCHK_SAMPLING_RATE;
	pSTDdev->ST_Func._DATA_SEND = func_RMCHK_DATA_SEND;

	pSTDdev->ST_Func._SYS_RESET = func_RMCHK_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_RMCHK_taskConfig;


	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RMCHK_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}

#if 1
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RMCHK_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif

#if 0
	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_RMCHK_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}
#endif



	return WR_OK;

}


void clear_RMCHK_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_RMCHK *pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	RFM2G_STATUS   result;                 /* Return codes from RFM2Get API calls  */

	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 

	

#if USE_RFM_MMAP
//	result = RFM2gUnMapUserMemory(pRMCHK->Handle, (volatile void **)(&outbuffer), pRMCHK->Pages);
	result = RFM2gUnMapUserMemoryBytes(pRMCHK->Handle, (volatile void **)(&pRMCHK->mapBuf), pRMCHK->Bytes);
	if( result != RFM2G_SUCCESS )
	{
		printf( "ERROR: RFM2gUnMapUserMemory() failed.\n" );
		printf( "ERROR MSG: %s\n", RFM2gErrorMsg(result));
	}
#endif

	 /* Close the Reflective Memory device */
	RFM2gClose( &pRMCHK->Handle );
	printf("rfm2g closed.\n");

}

void threadFunc_RMCHK_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_RMCHK *pRMCHK;
	ST_STD_device *pSTDcore = NULL;
	ST_RTcore *pRTcore;
	ST_MASTER *pMaster = NULL;
	epicsThreadId pthreadInfo;
	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode = NULL;
	unsigned int nval;
	int index;
	float fval[12];
	double dval[12];

	RFM2G_STATUS   result;                 /* Return codes from RFM2Get API calls  */
//	RFM2GTRANSFER rfm2gTransfer;
	RFM2G_UINT32   inbuffer[16*14];  /* Data read from RFM area              */
	
//	unsigned short u16Temp, u16Temp1;
//	int int32Tmp, int32Tmp1;
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_RMCHK_DAQ() has null pointer of STD device.\n");
		return;
	} /* else {
		printf("MY RMchk: %p, eventID:%d\n", pSTDdev, pSTDdev->ST_DAQThread.threadEventId);
	}*/
	pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	if( !pRMCHK ) {
		printf("ERROR! threadFunc_RMCHK_DAQ() has null pointer of pRMCHK.\n");
		return;
	}

	pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	while( !pSTDcore ) { 
		epicsThreadSleep(0.1); printf(". ");
		pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	}
	pRTcore = (ST_RTcore *)pSTDcore->pUser;
	while( !pRTcore ) {
                epicsThreadSleep(0.1); printf(". ");
                pRTcore =(ST_RTcore *)pSTDcore->pUser;
        }

#if 0
	test_mmap_working();	
#endif

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
	printf("%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RMCHK_DAQ);
//	setPosixPriority(pthreadInfo, 91, SCHED_FIFO);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RMCHK_DAQ, "SCHED_FIFO");
#endif

	epicsThreadSleep(1.0);
	pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
	ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		
#if USE_RMCHK_ACK_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
#if USE_RMCHK_ACK_LOW
		WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif

/*
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 0] = pRMCHK->mapBuf[RM_PCS_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 1] = pRMCHK->mapBuf[RM_PF1_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 2] = pRMCHK->mapBuf[RM_PF2_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 3] = pRMCHK->mapBuf[RM_PF3U_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 4] = pRMCHK->mapBuf[RM_PF3L_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 5] = pRMCHK->mapBuf[RM_PF4U_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 6] = pRMCHK->mapBuf[RM_PF4L_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 7] = pRMCHK->mapBuf[RM_PF5U_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 8] = pRMCHK->mapBuf[RM_PF5L_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 9] = pRMCHK->mapBuf[RM_PF6U_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 10] = pRMCHK->mapBuf[RM_PF6L_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 11] = pRMCHK->mapBuf[RM_PF7_CNT/4];
		pbufferNode->data[pRMCHK->cntDAQ_loop * LOOP_ELM + 12] = pRMCHK->mapBuf[RM_IVC_CNT/4];
*/

#if USE_RMCHK_DAQ_PRE_IO_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
#if 0
//		for(nval=0; nval < pRMCHK->cntTag; nval++)
		for(nval=0; nval < 16*14; nval++)
		{
/*			addr = pRMCHK->tagAddr[nval];
			pbufferNode->data[pRMCHK->cntDAQ_loop * pRMCHK->cntTag + nval] = pRMCHK->mapBuf[addr/4]; 
			pbufferNode->data[nval] = pRMCHK->mapBuf[ pRMCHK->tagAddr[nval] /4]; */
			inbuffer[nval] = pRMCHK->mapBuf[ pRMCHK->tagAddr[10] /4];
		}
#else
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF3U, &inbuffer, 40 );
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF1, &pbufferNode->RM_PF_return[0], sizeof(PF_return)*1 );
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF2, &pbufferNode->RM_PF_return[1], sizeof(PF_return)*1 );
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF3U, &pbufferNode->RM_PF_return[2], sizeof(PF_return)*8 );
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF7, &pbufferNode->RM_PF_return[10], sizeof(PF_return)*1 );
		memcpy( &pbufferNode->RM_PF_return[0], &pRMCHK->mapBuf[RFM_ID_PF1/4] , sizeof(PF_return));
		memcpy( &pbufferNode->RM_PF_return[1], &pRMCHK->mapBuf[RFM_ID_PF2/4] , sizeof(PF_return));
		memcpy( &pbufferNode->RM_PF_return[2], &pRMCHK->mapBuf[RFM_ID_PF3U/4] , sizeof(PF_return)*8);
		memcpy( &pbufferNode->RM_PF_return[10], &pRMCHK->mapBuf[RFM_ID_PF7/4] , sizeof(PF_return));
/*
		pRMCHK->mapBuf[0] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[1] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[2] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[3] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[4] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[5] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[6] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[7] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[8] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[9] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[10] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[11] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[12] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[13] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[14] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[15] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[16] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[17] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[18] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[19] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[20] = pSTDdev->stdDAQCnt;
		pRMCHK->mapBuf[21] = pSTDdev->stdDAQCnt;
*/

//		 if( result != RFM2G_SUCCESS )
//	        {
//	            printf( "\nERROR: Could not read data from Reflective Memory.\n" );
//	        }
#endif
/*		
		pRMCHK->cntDAQ_loop++;
		pRMCHK->cntAccum += pRMCHK->cntTag;

		if( pRMCHK->cntDAQ_loop >= LOOP_CNT ) {
			queueData.pNode		= pbufferNode;
			queueData.timeStamp	= 0x0;
			epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

			pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
			ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);

			pRMCHK->cntDAQ_loop = 0;

		}
*/

		queueData.pNode		= pbufferNode;
		queueData.timeStamp	= pSTDdev->stdDAQCnt;
		epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

		pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
		ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);

			
		pSTDdev->stdDAQCnt++;

#if 0
		result = RFM2gRead( pRMCHK->Handle, RFM_PCS_TO_PF, (void *)&pRMCHK->RM_PCS_PF, sizeof(PCS_cmd_PF) );
//		result = RFM2gRead( pRMCHK->Handle, RFM_PCS_TO_IVC, (void *)&pRMCHK->RM_PCS_IVC, sizeof(PCS_cmd_IVC) );
//		result = RFM2gRead( pRMCHK->Handle, RFM_CCS_SHARE, (void *)&pRMCHK->RM_CCS_SHARE, sizeof(CCS_from_PCS) );
//		result = RFM2gRead( pRMCHK->Handle, RFM_CCS_SHARE, (void *)&pRMCHK->RM_CCS_SHARE, sizeof(CCS_from_PCS) );
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF1, (void *)&pRMCHK->RM_PF_return, 896 );
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_TF, (void *)&pRMCHK->RM_TF_return, sizeof(TF_return));
//		result = RFM2gRead( pRMCHK->Handle, RFM_ID_IVC, (void *)&pRMCHK->RM_IVC_return, sizeof(IVC_return));
#endif
#if 1  /* true RTMON job ********************************************** RTMON task */
		fval[0] = pRMCHK->mapBuf[ RM_RMP_TOP_I / 4] * 20.0 / 65535.0 - 10.0 ;

		fprintf( pRMCHK->fp_cnt, "%d %lu %lf : %d | %08X %s %02x %02x | %x %x %x | T:%f v\n", 
				pSTDcore->stdCoreCnt, pSTDdev->stdDAQCnt, (double)pSTDdev->stdDAQCnt * pSTDdev->ST_Base_fetch.fStepTime + pSTDdev->ST_Base_fetch.dT0[0], 
				pRMCHK->mapBuf[RM_PCS_CNT/4], 
				pRMCHK->mapBuf[RM_PCS_FAULT_CODE/4],
				pRMCHK->mapBuf[RM_PCS_FAST_INTLOCK/4]?"H":"L",
				pRMCHK->mapBuf[RM_PCS_FORCE_ABORT/4],
				pRMCHK->mapBuf[RM_PCS_PFC_FAULT/4],
				pRMCHK->mapBuf[RM_RMP_ICMD_TOP/4],
				pRMCHK->mapBuf[RM_RMP_ICMD_MID/4],
				pRMCHK->mapBuf[RM_RMP_ICMD_BTM/4],
				fval[0]
			);
#endif
#if 0  /* temporary for RTSCOP 2013. 7. 29 */
		result = RFM2gRead( pRMCHK->Handle, RM_SCOPE_BUF_INDEX, &index, sizeof(int) ); 
		result = RFM2gRead( pRMCHK->Handle, RM_SCOPE_BUF_DATA, fval, sizeof(float)*10 ); 
		for(nval=0; nval<10; nval++)
			dval[nval] = -1. * fval[nval] * 1.0e-6;
		fprintf( pRMCHK->fp_cnt, "%d %lu %lf : ID: %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f \n", 
				pSTDcore->stdCoreCnt, pSTDdev->stdDAQCnt, (double)pSTDdev->stdDAQCnt * pSTDdev->ST_Base_fetch.fStepTime + pSTDdev->ST_Base_fetch.dT0[0], 
				index,
				dval[0], dval[1], dval[2], dval[3], dval[4], dval[5], dval[6], dval[7], dval[8], dval[9] 
			);

#endif

#if 0  /* for ECCD control test  */
//		u16Temp = (pRMCHK->mapBuf[RM_ECCD_x_A/4] >> 16  )& 0xffff; 
		fprintf( pRMCHK->fp_cnt, "%lu %lu %lf : 0x%x, %d\n", pSTDcore->stdCoreCnt, pSTDdev->stdDAQCnt, 
				(double)pSTDdev->stdDAQCnt * pSTDdev->ST_Base_fetch.fStepTime + pSTDdev->ST_Base_fetch.dT0[0], 
				pRMCHK->mapBuf[0x200 /4], pRMCHK->mapBuf[0x200 /4]  );
#endif

//		sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PCS_CNT/4]), 4*80);

#if USE_RMCHK_DAQ_POST_IO_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif
//		epicsThreadSleep(0.00001);
#if USE_RMCHK_DAQ_POST_IO_LOW
		WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif

		if( pRMCHK->useScanIoRequest )
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

	}
}

void threadFunc_RMCHK_RingBuf(void *param)
{
	int i, ellCnt;
	epicsThreadId pthreadInfo;

	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_RMCHK *pRMCHK;

	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode;
	
	ST_STD_device *pSTDcore = NULL;
	ST_RTcore *pRTcore;


	pRMCHK = (ST_RMCHK *)pSTDdev->pUser;

	pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	while( !pSTDcore ) { 
		epicsThreadSleep(0.1); printf(". ");
		pSTDcore = get_STDev_from_type(STR_DEVICE_TYPE_1);
	}
	pRTcore = (ST_RTcore *)pSTDcore->pUser;
	while( !pRTcore ) {
                epicsThreadSleep(0.1); printf(". ");
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
		ellAdd(pSTDdev->pList_BufferNode, &pbufferNode1->node);
	}
	epicsPrintf(" %s: create %d node (size:%dByte)\n", pSTDdev->ST_RingBufThread.threadName, 
																		MAX_RING_BUF_NUM,
																		sizeof(ST_buf_node) );
#if 0
	epicsPrintf(">>> %s, ellCnt:%d\n", pringThreadConfig->threadName, 
												ellCount(p16aiss8ao4->pdrvBufferConfig->pbufferList));
#endif

#if USE_CPU_AFFINITY_RT	
	pthreadInfo = epicsThreadGetIdSelf();
	epicsThreadSetCPUAffinity( pthreadInfo, AFFINITY_RMCHK_RING);
	epicsThreadSetPosixPriority(pthreadInfo, PRIOTY_RMCHK_RING, "SCHED_FIFO");
#endif

	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

		pbufferNode = queueData.pNode;

#if USE_RMCHK_RING_IO_HIGH
		WRITE32(pRTcore->base0 + 0x4, 0x1);
#endif

/*		fwrite( pbufferNode->data,  sizeof(unsigned int), LOOP_CNT * pRMCHK->cntTag, pRMCHK->fp_raw); */
		fwrite( pbufferNode->RM_PF_return,  sizeof(PF_return)*11, 1, pRMCHK->fp_raw);
//		fwrite( &pbufferNode->RM_TF_return,  sizeof(TF_return), 1, pRMCHK->fp_raw);
//		fwrite( &pbufferNode->RM_IVC_return,  sizeof(IVC_return), 1, pRMCHK->fp_raw);




#if USE_RMCHK_RING_IO_LOW
		WRITE32(pRTcore->base0 + 0x4, 0x0);
#endif
		
		
		ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
		ellCnt = ellCount(pSTDdev->pList_BufferNode);
/*		printf("RTcore_RingBuf, ellCnt: %d\n", ellCnt); */


	} /* while(TRUE)  */

	return;
}


#if 0
void threadFunc_RMCHK_DAQ(void *param)
{
	ST_STD_device *pSTDmy = (ST_STD_device*) param;
	ST_RMCHK *pRMCHK;
	ST_MASTER *pMaster = NULL;
	int			got;
	int		send=0;
	int		sent=0;
	int nval;
	int nFirst = 1;

	RFM2G_STATUS   result;                 /* Return codes from RFM2Get API calls  */
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDmy ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}

	pRMCHK = (ST_RMCHK *)pSTDmy->pUser;
	if( !pRMCHK ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of pRMCHK.\n");
		return;
	}


#if USE_RFM_MMAP
//	result = RFM2gUserMemory(pRMCHK->Handle, (volatile void **)(&outbuffer), pRMCHK->Offset, pRMCHK->Pages);
	result = RFM2gUserMemoryBytes(pRMCHK->Handle, (volatile void **)(&pRMCHK->mapBuf), pRMCHK->Offset, pRMCHK->Bytes);
	if( result != RFM2G_SUCCESS )
	{
		printf( "ERROR: RFM2gUserMemory() failed.\n" );
		printf( "ERROR MSG: %s\n", RFM2gErrorMsg(result));
		/* Close the Reflective Memory device */
		RFM2gClose( &pRMCHK->Handle );
		return;
	}
#endif

#if 0
	test_mmap_working();	
#endif

	while(TRUE) {
		epicsEventWait( pSTDmy->ST_DAQThread.threadEventId);
		pSTDmy->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		notify_refresh_master_status();
		nFirst = 1;
		printf("%s: Got epics Run event.\n", pSTDmy->taskName);
		while(TRUE) {
//			printf("request to kernel...");
			got	= read(pRMCHK->fd_event, &nval, 4);
//			printf("%d\n", nval);
			pSTDmy->rtCnt++; 

			WRITE32(pRMCHK->base0 + 0x4, 0x1);


			if( nFirst ) {
				send_master_stop_event();
				nFirst = 0;
			}
				
				
#if 0			/* Now read data from the other board from OFFSET_1 */
			result = RFM2gRead( pRMCHK->Handle, 0x1000, (void *)rfmBuf, 100 );
			if( result == RFM2G_SUCCESS )
			{

			//	printf( "\nData was read from Reflective Memory.\n" );
			}
			else
			{
				printf( "\nERROR: Could not read data from Reflective Memory.\n" );
			}
#endif
#if 0
			result = RFM2gRead( pRMCHK->Handle, RFM_PCS_TO_PF, (void *)&pRMCHK->RM_PCS_PF, sizeof(PCS_cmd_PF) );
//			result = RFM2gRead( pRMCHK->Handle, RFM_PCS_TO_IVC, (void *)&pRMCHK->RM_PCS_IVC, sizeof(PCS_cmd_IVC) );
//			result = RFM2gRead( pRMCHK->Handle, RFM_CCS_SHARE, (void *)&pRMCHK->RM_CCS_SHARE, sizeof(CCS_from_PCS) );
//			result = RFM2gRead( pRMCHK->Handle, RFM_CCS_SHARE, (void *)&pRMCHK->RM_CCS_SHARE, sizeof(CCS_from_PCS) );
//			result = RFM2gRead( pRMCHK->Handle, RFM_ID_PF1, (void *)&pRMCHK->RM_PF_return, 896 );
//			result = RFM2gRead( pRMCHK->Handle, RFM_ID_TF, (void *)&pRMCHK->RM_TF_return, sizeof(TF_return));
//			result = RFM2gRead( pRMCHK->Handle, RFM_ID_IVC, (void *)&pRMCHK->RM_IVC_return, sizeof(IVC_return));
#endif
#if 0
			fprintf( pRMCHK->fp_cnt, "%lu %lf : %d , %d %d | %d %d | %d %d | %d %d | %d %d | %d %d %08X %x | %x %x %x\n", pSTDmy->rtCnt, (double)pSTDmy->rtCnt * pRMCHK->daqStepTime + pRMCHK->clkStartTime, 
				pRMCHK->mapBuf[RM_PCS_CNT/4], pRMCHK->mapBuf[RM_PF1_CNT/4], pRMCHK->mapBuf[RM_PF2_CNT/4],
				pRMCHK->mapBuf[RM_PF3U_CNT/4], pRMCHK->mapBuf[RM_PF3L_CNT/4],
				pRMCHK->mapBuf[RM_PF4U_CNT/4], pRMCHK->mapBuf[RM_PF4L_CNT/4],
				pRMCHK->mapBuf[RM_PF5U_CNT/4], pRMCHK->mapBuf[RM_PF5L_CNT/4],
				pRMCHK->mapBuf[RM_PF6U_CNT/4], pRMCHK->mapBuf[RM_PF6L_CNT/4],
				pRMCHK->mapBuf[RM_PF7_CNT/4], pRMCHK->mapBuf[RM_IVC_CNT/4],
				pRMCHK->mapBuf[RM_PCS_FAULT_CODE/4],
				pRMCHK->mapBuf[RM_PCS_FAST_INTLOCK/4],
				pRMCHK->mapBuf[RM_RMP_BTM_I/4],
				pRMCHK->mapBuf[RM_RMP_MID_I/4],
				pRMCHK->mapBuf[RM_RMP_TOP_I/4]
				
				);
#endif
#if 0
			sent = write(pRMCHK->fd_shot, &(pSTDmy->rtCnt), 4);
			nval = ((double)pSTDmy->rtCnt * pRMCHK->daqStepTime + pRMCHK->clkStartTime ) * 10000;
			sent = write(pRMCHK->fd_shot, &nval, 4);
			
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PCS_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF1_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF2_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF3U_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF3L_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF4U_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF4L_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF5U_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF5L_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF6U_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF6L_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PF7_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_IVC_CNT/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PCS_FAULT_CODE/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PCS_FAST_INTLOCK/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_RMP_BTM_I/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_RMP_MID_I/4]), 4);
			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_RMP_TOP_I/4]), 4);
#endif
//			sent = write(pRMCHK->fd_shot, &(pRMCHK->mapBuf[RM_PCS_CNT/4]), 4*80);

			WRITE32(pRMCHK->base0 + 0x4, 0x0);


			if( pRMCHK->useScanIoRequest )
				scanIoRequest(pSTDmy->ioScanPvt_userCall);


			if( !(pSTDmy->StatusDev & TASK_IN_PROGRESS ) )
			{
				pSTDmy->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				printf("system stop!.. bye DAQ thread!\n");
				break;

			}

		}

		if( pMaster->cUseAutoSave ) 
			db_put("RTMON_SEND_DATA", "1");
		
	}
}
#endif

static long drvRMCHK_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_RMCHK *pRMCHK = NULL;
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

		if(create_RMCHK_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_RMCHK_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_RMCHK(pSTDdev);
		
	
		pRMCHK = (ST_RMCHK *)pSTDdev->pUser;


		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pRMCHK) { */

	notify_refresh_master_status();

	printf("****** RTMON local device init OK!\n");

	return 0;
}


static long drvRMCHK_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_RMCHK *pRMCHK;
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
		pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
/*		
			epicsPrintf("   callback time: %fusec\n", pRMCHK->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pRMCHK->adjCount_smplRate,
					                                                 pRMCHK->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pRMCHK->adjCount_Gain,
					                                                   pRMCHK->adjTime_Gain_Usec);
*/
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_RMCHK_OP_MODE(void *pArg, double arg1, double arg2)
{
	if( (int)arg1 == OPMODE_REMOTE ) {
		/* to do here */

	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		/* to do here */

	}
}

void func_RMCHK_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_RMCHK_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

	


}

void func_RMCHK_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_RMCHK *pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	char fileName[40];
	char fileNameB[40];

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;
/* to do here */
#if 1
		sprintf(fileName, "/mnt/ramdisk/%lu_cnt.txt", pSTDdev->ST_Base.shotNumber);
		if( (pRMCHK->fp_cnt = fopen(fileName, "w")) == NULL )
		{
			printf("'%s'  open fp error!\n", fileName);
			return;
		}
		sprintf(fileNameB, "/mnt/ramdisk/%lu_raw.dat", pSTDdev->ST_Base.shotNumber);
//		if( (pRMCHK->fp_raw = open(fileNameB,  O_RDWR | O_CREAT)) == 0 )
		if( (pRMCHK->fp_raw = fopen(fileNameB, "wb") ) == 0 )
		{
			printf("'%s'  open file error!\n", fileNameB);
			return;
		}


		
		printf("System Arming...\n");
		pSTDdev->stdDAQCnt = 0;
/*		pRMCHK->cntDAQ_loop = 0;
		pRMCHK->cntAccum = 0; */
		
/*		pRMCHK->daqStepTime = 1.0 /(double)pSTDdev->ST_Base.nSamplingRate; */
/*		pRMCHK->clkStartTime = (double)pSTDdev->ST_Base.dT0[0]; */



		fprintf(pRMCHK->fp_cnt, "Real Time monitor - counter check.\n");
		fprintf(pRMCHK->fp_cnt, "rtCoreCnt, DaqCnt, time, PCS | PF1, PF7, IVC |PCS fault, fast intl, Abort, PFC | T. M. B\n");
		fprintf(pRMCHK->fp_cnt, "RT clk: %dHz, Hz: %dHz, step time: %lf\n", pSTDdev->rtClkRate, pSTDdev->ST_Base.nSamplingRate, pSTDdev->ST_Base.fStepTime);
//		fprintf(pRMCHK->fp_cnt, "rtCnt daqCnt time(s) :  PCS,  PF1  PF2  (3U 3L) (4U 4L) (5U 5L) (6U 6L) PF7 IVC  PCS_fault Fast_INTL |BI MI TI \n");

		printf("Hz: %dHz, step time: %lf\n", pSTDdev->ST_Base.nSamplingRate, pSTDdev->ST_Base.fStepTime);
#endif

/*		flush_STDdev_to_MDSfetch( pSTDdev ); */
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;	
		notify_error(ERR_SCN, "no error\n");
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;
#if 1

		if( pRMCHK->fp_cnt != NULL )
			fclose(pRMCHK->fp_cnt);

		if( pRMCHK->fp_raw != NULL )
			fclose(pRMCHK->fp_raw);

		system("mv -f /mnt/ramdisk/* /home/data/rfmVal/");
#endif

		kfwPrint(0, "RTMON -> stdCoreCnt(no): %d, stdDAQCnt: %d", pSTDdev->stdCoreCnt, pSTDdev->stdDAQCnt);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		notify_error(ERR_SCN, "no error\n");
	}
}

void func_RMCHK_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_RMCHK *pRMCHK = (ST_RMCHK *)pSTDdev->pUser;

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
/*
void func_RMCHK_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_RMCHK *pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: event rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}
*/
void func_RMCHK_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your MDS related functions */



	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();
}

void func_RMCHK_SYS_RESET(void *pArg, double arg1, double arg2)
{
//	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your device reset functions */




}


void test_mmap_working()
{
	#define BUFFER_SIZE     4

	ST_STD_device *pSTDdev;
	ST_RMCHK *pRMCHK;

	int nval;


//	volatile RFM2G_UINT32 * outbuffer;	   /* Pointer mapped to RFM area		   */
	RFM2G_STATUS   result;				   /* Return codes from RFM2Get API calls  */
	RFM2G_UINT32   inbuffer[BUFFER_SIZE];  /* Data read from RFM area			   */
	
	
	pSTDdev = get_first_STDdev();
	if( !pSTDdev ) {
		printf("ERROR! test_mmap_working() has null pointer of STD device.\n");
		return;
	}
	
	pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	if( !pRMCHK ) {
		printf("ERROR! test_mmap_working() has null pointer of pRMCHK.\n");
		return;
	}


	/* Initialize the data buffers */
    for( nval=0; nval<BUFFER_SIZE; nval++ )
    {
        pRMCHK->mapBuf[nval] = 0xaabb0000 + nval;  /* Any data will do */
        inbuffer[nval] = 0;
    }

    /* Use read to collect the data */
    result = RFM2gRead(pRMCHK->Handle, 0, &inbuffer,
                       (BUFFER_SIZE * sizeof(RFM2G_UINT32)));

    /* Compare the data buffers */
    for( nval=0; nval<BUFFER_SIZE; nval++ )
    {
        printf("Wrote: %08X, Read: %08X\n", pRMCHK->mapBuf[nval], inbuffer[nval]);
        if(pRMCHK->mapBuf[nval] != inbuffer[nval])
        {
            printf( "ERROR: Test failed, index %d, Read %08X, Expected %08X\n",
                    nval, inbuffer[nval], pRMCHK->mapBuf[nval] );

            /* Close the Reflective Memory device */
            RFM2gClose( &pRMCHK->Handle );
            return;
        }
    }

    printf( "\nSuccess!\n\n" );

}

void drvRMCHK_init_file( ST_STD_device *pSTDdev)
{
	ST_RMCHK *pRMCHK = (ST_RMCHK*)pSTDdev->pUser;
	int nval, cnt;
	FILE *fp;
	char filename[64];
	char bufLine[64];
	unsigned int chid_t, on_t, addr_t;
	char tagname_t[SIZE_STRINGOUT_VAL];
	char bufAddr[10];

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
	pRMCHK->cntTag=0;
	cnt = 0;

	while(1) 
	{
		if( fgets(bufLine, 64, fp) == NULL ) break;

		if( bufLine[0] == '#') ;
		else {
	//		sscanf(bufLine, "%d %s %s", &chid, mask, tagname);
			nval = sscanf(bufLine, "%d %d %s %x", &chid_t, &on_t, tagname_t, &addr_t);
			if( nval == 4) {
/*				chid = chid_t;
				on = on_t;
				strcpy( tagname, tagname_t );
				addr = addr_t;
*/
				sprintf(pvname, "%s_%s_CH%d:TAG_NAME", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, cnt );
				db_put(pvname, tagname_t);

				sprintf(pvname, "%s_%s_CH%d:ENABLE", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, cnt );
				sprintf(mask, "%d", on_t);
				db_put(pvname, mask);

				sprintf(pvname, "%s_%s_CH%d:TAG_ADDR", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, cnt );
				sprintf(bufAddr, "%d", addr_t);
				db_put(pvname, bufAddr);
		
				cnt++;
			}
		}

		if( cnt > (MAX_TAG_CNT - 1) ) {
			printf("ERROR!!!    RFM Tag limit error. *****************\n");
			break;
		}
	}
	fclose(fp);

	pRMCHK->cntTag = cnt;

	printf(" RFM Address cnt: %d\n", pRMCHK->cntTag);
/*
	for(i=0; i<pRMCHK->cntTag; i++) {
		
		sprintf(pvname, "%s_%s_CH%d:TAG_NAME", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, i);
		db_put(pvname, tagname[i]);
//		printf("%s  %s\n", pvname, tagname[i]);
		
		sprintf(pvname, "%s_%s_CH%d:ENABLE", pSTDdev->pST_parentAdmin->taskName, pSTDdev->taskName, i);
//		itoa(on[i], mask, 10);
		sprintf(mask, "%d", on[i]);
		db_put(pvname, mask);
//		printf("%s  %s\n", pvname, mask);
	}
*/
//	fclose(fp);

}

void drvRMCHK_save_file( ST_STD_device *pSTDdev)
{
	ST_RMCHK *pRMCHK = (ST_RMCHK*)pSTDdev->pUser;
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

	for( i=0; i<pRMCHK->cntTag; i++){
		fprintf(fp, "%d\t%d\t%s\t%x\n", i, pRMCHK->channelOnOff[i], pRMCHK->strTagName[i], pRMCHK->tagAddr[i]);
	}

	fclose(fp);

}




