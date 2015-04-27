#include "sfwCommon.h"
#include "sfwMDSplus.h"

/*#include "sfwSequence.h" */


#include "drvV792.h"
#include "myMDSplus.h"
#include <math.h>



static int32_t   vme_handle;   /* The CAENVMELib handle of V2718 */

int header_dreg; /* 2011. 6. 16 remove for TSSC header pulse */


static long drvV792_io_report(int level);
static long drvV792_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvV792 = {
       2,
       drvV792_io_report,
       drvV792_init_driver
};

epicsExportAddress(drvet, drvV792);




int CAENVME_handle_init()
{
	if ( CAENVME_Init( cvV2718, 0, 0, &vme_handle) == cvSuccess) {
            return WR_OK;
        }
	return WR_ERROR;
}
void CAENVME_handle_release()
{
	CAENVME_End(vme_handle);
}

int create_v792_taskConfig( ST_STD_device *pSTDdev)
{
	ST_V792* pV792;
	int i, j;

	pV792 = (ST_V792 *)malloc(sizeof(ST_V792));
	if( !pV792) return WR_ERROR;

	pSTDdev->pUser = pV792;

	memset( &pV792->board_data, 0, sizeof( cvt_V792_data));

	pV792->m_base_address = strtoul(pSTDdev->strArg0,NULL,0);

	if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) ) {
		pV792->m_qtp_type = CVT_V792_TYPE_A; /*CVT_V792_TYPE_A, CVT_V792_TYPE_N */
		pV792->chNum = CH_CNT_MAX;
		pV792->PolycMaxNum = MAX_PLYC_PER_BOARD;
	}
	else if ( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) ) {
		pV792->m_qtp_type = CVT_V792_TYPE_N; /*CVT_V792_TYPE_A, CVT_V792_TYPE_N */
		pV792->chNum = 16;
		pV792->PolycMaxNum = 3;
	}
	else {
		free(pV792);
		return WR_ERROR;
		}

	pV792->m_ST_AcqModeParam.m_count_all_events = FALSE;
	pV792->m_ST_AcqModeParam.m_empty_enable = FALSE;
	pV792->m_ST_AcqModeParam.m_overflow_suppression_enable = FALSE;
	pV792->m_ST_AcqModeParam.m_sliding_scale_enable = FALSE;
	pV792->m_ST_AcqModeParam.m_zero_suppression_enable = FALSE;

	pV792->m_ST_zeroParam.m_step_threshold = 0;
	pV792->m_ST_zeroParam.m_high_thresholds_buff[0] = 0;
	pV792->m_ST_zeroParam.m_thresholds_buff[0] = 0;

	for(i = 0; i<CH_CNT_MAX; i++) {
		pV792->fpRaw[i] = NULL;
//		pV792->ST_Ch[i].acuum_value = 0;
//		pV792->ST_Ch[i].mean_value = 0;
//		pV792->ST_Ch[i].acuum_value_bg= 0;
//		pV792->ST_Ch[i].mean_value_bg= 0;
//		pV792->ST_Ch[i].rawVal_current = 0;
//		pV792->ST_Ch[i].rawVal_min = 4095;
//		pV792->ST_Ch[i].rawVal_max = 0;
/*		pV792->ST_Ch[i].rawVal_prev[CUR_1] = 0;
		pV792->ST_Ch[i].rawVal_prev[CUR_2] = 0; */

		for( j=0; j< CNT_QTP_READ_BUFF_EXTRA; j++) {
/*			pV792->ST_Ch[i].tmp_rawData[j] = 0; */
//			pV792->ST_Ch[i].ST_Calc[j].raw_cur_2 = 0;
//			pV792->ST_Ch[i].ST_Calc[j].raw_cur_1 = 0;
//			pV792->ST_Ch[i].ST_Calc[j].raw_cur = 0;

			pV792->raw_curr[i][j] = 0;
		}
		
//		for( j=0; j< 256; j++)
//			pV792->ST_Ch[i].Iped_ref[j] = 0;			
	}

	pV792->Pedestal = INIT_IPAD_VALUE;
	pV792->cmd_DAQ_stop = 0;
	pV792->cnt_block_read = 0;
	pV792->use_file_save = 0;
	pV792->toggle_rt_calc = 1;
	
	pV792->toggle_get_MeanVal = 0;
	pV792->accum_cnt = 0;
//	pV792->check_accum_cnt = 0;
	pV792->calib_iter_cnt = CALIB_ITER_CNT;
	pV792->calib_increase_cnt = CALIB_PEDESTAL_INCRSE_CNT;

	pV792->my_shot_period = 5;


	pV792->time_LTU_DG535_us = 0;
/*	pV792->time_LTU_LASER_us = 0; */
	pV792->count_DG535_IN_PULSE = 0;
	pV792->time_Bg_delay_us = 0;

	pV792->mask_channel = 0x0;

	return WR_OK;
}

int init_my_SFW_v792( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/

	pSTDdev->ST_Func._DATA_SEND = func_V792_DATA_SEND;
	pSTDdev->ST_Func._SYS_ARMING = func_V792_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_V792_SYS_RUN;
	pSTDdev->ST_Func._OP_MODE = func_V792_OP_MODE;
	pSTDdev->ST_Func._TEST_PUT = func_V792_TEST_PUT;


/************************************************/
/*	drvV792_make_calcBuff(pSTDdev); */



	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_V792_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#if 0
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_V792_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif
	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_V792_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}
	
	return WR_OK;
}

void clear_V792_Locally(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792=NULL;
	pV792 = (ST_V792*)pSTDdev->pUser;

	if( pV792->pList_BufferNode )
		free(pV792->pList_BufferNode);

	if( pV792->pST_BuffThread )
		free(pV792->pST_BuffThread);
}


int drvV792_status_reset(ST_STD_device *pSTDdev)
{
/*	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser; */

	set_STDdev_status_reset(pSTDdev);

	return WR_OK;
}





void threadFunc_V792_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
//	ST_MASTER *pMaster = get_master();
	
//	ST_User_Buf_node queueData;
//	ST_DATA_node *pbufferNode = NULL;
//	ST_threadCfg *pDAQST_threadConfig=NULL;
/*
	ST_Calc_Buff_node queueDataCalc;
	ST_DATA_node *pbufferNodeCalc = NULL;
	ST_threadCfg *pDAQST_threadConfigCalc=NULL; 
*/
	unsigned short qtp_rawData[CH_CNT_MAX][CNT_QTP_READ_BUFF_EXTRA];

	ST_V792 *pV792=NULL;
	int j, ch, cnt, nFirst, nval=0;
	int cmdEnd;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_usr_DAQ() has null pointer of STD device.\n");
		return;
	}
	pV792 = (ST_V792*)pSTDdev->pUser;
	if( !pV792 ) {
		printf("%s: threadFunc_usr_DAQ() has null pointer of User device.\n", pSTDdev->taskName );
		return;
	}


	while(TRUE) 
	{
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		
#if 1
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start. chNum: %d\n", pSTDdev->taskName, pV792->chNum);
#endif
//		pDAQST_threadConfig = (ST_threadCfg*) pSTDdev->pST_BuffThread;
//		if( !pDAQST_threadConfig ) {
//			printf("%s: can't recive \"pDAQST_threadConfig\".\n", pSTDdev->taskName );
//			return;
//		}
/*		pDAQST_threadConfigCalc = (ST_threadCfg*) pV792->pST_BuffThread;
		if( !pDAQST_threadConfigCalc ) {
			printf("%s: can't recive \"pDAQST_threadConfigCalc\".\n", pSTDdev->taskName );
			return;
		}
*/
		if ( !cvt_V792_data_clear( &pV792->board_data)) {
			epicsPrintf( "\nError %s: executing cvt_V792_data_clear \n", pSTDdev->taskName );
			return ;
		}
		

		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		pV792->cnt_block_read = 0;
		nFirst = 1;
		while( 1 )
		{
#if 1
/*			epicsMutexLock(pSTDdev->taskLock);*/
			cnt = drvV792_read_chanData_Loop(pSTDdev);
/*			epicsMutexUnlock(pSTDdev->taskLock); */
#endif
			cmdEnd = pV792->cmd_DAQ_stop;
			if( pV792->cnt_block_read >= CNT_QUEUE_SND_CUTOFF) 
			{
//				printf("   %s received %d \n", pSTDdev->taskName, pV792->cnt_block_read);
/* don't use buffer, just hard read test */

/*
				pbufferNode = (ST_DATA_node *)ellFirst(pSTDdev->pList_BufferNode);
				ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);
*/
				for(ch=0; ch< pV792->chNum; ch++) 
				{
/*					memcpy( pbufferNode->qtp_rawData[i], pV792->ST_Ch[i].tmp_rawData, 2*pV792->cnt_block_read); */
/*					queueData.acuum_value[i] = pV792->ST_Ch[i].acuum_value;  // i don't know where it is used. 2010. 9. 3
					queueData.mean_value[i] = pV792->ST_Ch[i].mean_value; */

						for(j=0; j<pV792->cnt_block_read; j++) {
//							qtp_rawData[ch][j] = pV792->ST_Ch[ch].ST_Calc[j].raw_cur;
							qtp_rawData[ch][j] = pV792->raw_curr[ch][j];
						}

						nval = fwrite( qtp_rawData[ch], 2*pV792->cnt_block_read, 1, pV792->fpRaw[ch] );

				}
/*				memcpy( pbufferNode->qtp_rawData, pV792->qtp_rawData, 2*32*pV792->cnt_block_read); */ /* 2*32*100  = 6400 */
/*
				queueData.pNode     = pbufferNode;
				queueData.cnt_block_read   = pV792->cnt_block_read;
				queueData.accum_cnt         = pV792->accum_cnt;
				queueData.Iped_num = pV792->Pedestal;
				queueData.cReset_IncreaseIped = 0x0;
*/

				if( nFirst==1 ) 
				{
					pSTDdev->StatusDev |= TASK_IN_PROGRESS;
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					scanIoRequest(pSTDdev->ioScanPvt_status);
					notify_refresh_master_status();
					
					if( pSTDdev->BoardID == 0  ) 
					{
						printf("%s: Send catch end event.\n", pSTDdev->taskName );
						epicsEventSignal( pSTDdev->ST_CatchEndThread.threadEventId); // epicsEvent_CatchEnd );
						nFirst = 0;
					}
				}

				pV792->cnt_block_read = 0;

			}
		/*  ________________End of if (xxx) 
			if( (pV792->cnt_block_read >= CNT_QUEUE_SND_CUTOFF) ||
				(pV792->cmd_DAQ_stop == 1)  )
		*/
			if( cmdEnd  ) 				
			{
				pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
				pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
//				pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
//				pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
				scanIoRequest(pSTDdev->ioScanPvt_status);
				notify_refresh_master_status();
/*
				if( pSTDdev->BoardID == 0  ) 
				{ 				
					db_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
					epicsThreadSleep(0.3);
					db_put(pMaster->strPvNames[NUM_SYS_ARMING], "0");
				}
*/
				break;
			}
			epicsThreadSleep(TIME_DAQ_THREAD_DELAY);

		}

		epicsThreadSleep(1);
		notify_refresh_master_status();
		pV792->cmd_DAQ_stop = 0;
		epicsPrintf("%s: wait DAQ start. \n", pSTDdev->taskName );

/*  move to ring buff thread...
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		scanIoRequest(pSTDdev->ioScanPvt_status);
*/
		
	}
}

#if 0
void threadFunc_Calc_RingBuf(void *param)
{
	int i,polych,lut, ellCnt=0;
	int ch;


	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_threadCfg *pST_BufThread;

	ST_Calc_Buff_node qdata;
	ST_DATA_node *pnode;	
	ST_V792 *pp;

	pp = (ST_V792*)pSTDdev->pUser;
	pST_BufThread = (ST_threadCfg*) pp->pST_BuffThread;
	


#if 0
	epicsPrintf("\n %s: pSTDdev: %p, pp: %p , pST_BuffThread: %p\n", pSTDdev->taskName, pSTDdev, pp, pST_BufThread );
#endif

	for(i = 0; i< MAX_RING_BUF_NUM; i++)
	{
		ST_DATA_node *pnode1 = NULL;
		pnode1 = (ST_DATA_node*) malloc(sizeof(ST_DATA_node));
		if(!pnode1) {
			epicsPrintf("\n>>> threadFunc_Calc_RingBuf: malloc(sizeof(ST_DATA_node)) , index=%d... fail\n", i);
			return;
		}
		ellAdd(pp->pList_BufferNode, &pnode1->node);
	}
	epicsPrintf(" %s: create %d node (size:%dKB)\n", pST_BufThread->threadName,  MAX_RING_BUF_NUM, sizeof(ST_DATA_node)/1024 );

#if 0
	epicsPrintf(">>> %s, ellCnt:%d\n", pringThreadConfig->threadName, 
												ellCount(pAcq196->pdrvBufferConfig->pbufferList));
#endif
	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pST_BufThread->threadQueueId, (void*) &qdata, sizeof(ST_Calc_Buff_node));
#if 0
		printf("%s: got msg cnt_block_read: %d\n", pSTDdev->taskName, 
			qdata.cnt_block_read ); 
#endif 

		pnode = qdata.pNode;

#if 0 /* fixed value for calculation test */
		for( i=0; i<qdata.cnt_block_read; i++) 
		{
/*			for( ch=0; ch < pV792->chNum; ch++) {
				pnode->STbuf[ch][i].raw_cur_2 = 0;
				pnode->STbuf[ch][i].raw_cur_1 = 50.991318535920946;
				pnode->STbuf[ch][i].raw_cur = 0;
			}
*/
			pnode->STbuf[0][i].raw_cur_2 = 0;
			pnode->STbuf[0][i].raw_cur_1 = 70;  /*100ev  */
/*			pnode->STbuf[0][i].raw_cur_1 = 44.573717176728515; *//* 300 ev*/
			pnode->STbuf[0][i].raw_cur = 0;

			pnode->STbuf[1][i].raw_cur_2 = 5;
			pnode->STbuf[1][i].raw_cur_1 = 46; 
/*			pnode->STbuf[1][i].raw_cur_1 = 31.59636897092137; */
			pnode->STbuf[1][i].raw_cur = 5;

			pnode->STbuf[2][i].raw_cur_2 = 20;
			pnode->STbuf[2][i].raw_cur_1 = 111; 
/*			pnode->STbuf[2][i].raw_cur_1 = 86.25156408911836; */
			pnode->STbuf[2][i].raw_cur = 20;

			pnode->STbuf[3][i].raw_cur_2 = 10;
			pnode->STbuf[3][i].raw_cur_1 = 97; 
/*			pnode->STbuf[3][i].raw_cur_1 = 158.2393339634496; */
			pnode->STbuf[3][i].raw_cur = 10;

			pnode->STbuf[4][i].raw_cur_2 = 30;
			pnode->STbuf[4][i].raw_cur_1 = 32; 
/*			pnode->STbuf[4][i].raw_cur_1 = 76.64101258854444; */
			pnode->STbuf[4][i].raw_cur = 30;

			
				
		}
#endif


		for( i=0; i<qdata.cnt_block_read; i++) 
		{
			float64 tmpTe[MAX_PLYC_PER_BOARD];
			float64 fetchK2=0.0, fetchK3=0.0;
			int fndCnt = 0;
			UINT16 raw_2, raw_1, raw;

#if 1
			if( pnode->STbuf[0][i].isActual ) 
			{
/*				printf("%s: got actual signal. skip this\n", pSTDdev->taskName); */
				continue;
			}
/*			else {
				printf("%s: got background signal. solve Te\n", pSTDdev->taskName);
			}
*/
#endif
			for( ch=0; ch < pp->chNum; ch++) {
				/* noise compensation.... 2010. 9. 21 */
				raw_2 = (unsigned short)((float)pnode->STbuf[ch][i].raw_cur_2 * 0.01);
				raw_1 = (unsigned short)((float)pnode->STbuf[ch][i].raw_cur_1 * 0.01);
				raw = (unsigned short)((float)pnode->STbuf[ch][i].raw_cur * 0.01);

				pnode->STbuf[ch][i].raw_cur_2 = raw_2 *10;
				pnode->STbuf[ch][i].raw_cur_1 = raw_1 *10;
				pnode->STbuf[ch][i].raw_cur = raw *10;

				if(  pnode->STbuf[ch][i].raw_cur_2 >= pnode->STbuf[ch][i].raw_cur_1 )  pnode->STbuf[ch][i].raw_cur_2 = pnode->STbuf[ch][i].raw_cur_1 -1;
				if(  pnode->STbuf[ch][i].raw_cur >= pnode->STbuf[ch][i].raw_cur_1 )  pnode->STbuf[ch][i].raw_cur = pnode->STbuf[ch][i].raw_cur_1 -1;
				/* noise compensation.... 2010. 9. 21 ***************************************/

				
				pp->f32AvrBg[ch] = (pnode->STbuf[ch][i].raw_cur_2 + pnode->STbuf[ch][i].raw_cur) / 2.0;
				pp->n32ScatterData[ch] = pnode->STbuf[ch][i].raw_cur_1 - pp->f32AvrBg[ch];/*  - ( pp->ST_Ch[ch].mean_value - pp->ST_Ch[ch].mean_value_bg ); */

				pp->f64WeightFactor[ch] = 1.0 / ( 0.50 * (pow( pnode->STbuf[ch][i].raw_cur_2 - pp->f32AvrBg[ch], 2) + 
												pow( pnode->STbuf[ch][i].raw_cur - pp->f32AvrBg[ch], 2) ) +
												pp->n32ScatterData[ch] ) ;
			}

			for(polych=0; polych<MAX_PLYC_PER_BOARD; polych++) 
			{
				tmpTe[polych] = 99999999.0; /* initialize */
				fndCnt = 0;
				
				pp->f64K1[polych] = pp->f64WeightFactor[pp->plycW2ChMap[polych]] * pow(pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_1, 2) + 
								pp->f64WeightFactor[pp->plycW3ChMap[polych]] * pow(pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_1, 2) +
								pp->f64WeightFactor[pp->plycW4ChMap[polych]]*pow(pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_1, 2) + 
								pp->f64WeightFactor[pp->plycW5ChMap[polych]]*pow(pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_1, 2) ;
/*				if( (polych ==0) && (pSTDdev->BoardID ==0) ) {
					printf("%s\t K1[%d]: %f\n", pSTDdev->taskName, polych, pp->f64K1[polych] );
					printf("%s:poly 1 channel map, %d, %d, %d, %d, %d \n", pSTDdev->taskName, pp->plycW1ChMap[polych], 
							pp->plycW2ChMap[polych], 
							pp->plycW3ChMap[polych], 
							pp->plycW4ChMap[polych], 
							pp->plycW5ChMap[polych] );
				} */

				for( lut=0; lut<CNT_LOOKUP_TABLE; lut++)
				{
					pp->f64K2 = pow( (pp->f64WeightFactor[pp->plycW2ChMap[polych]]*pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_1*pp->SP2[polych][lut] + 
										pp->f64WeightFactor[pp->plycW3ChMap[polych]]*pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_1*pp->SP3[polych][lut] +
										pp->f64WeightFactor[pp->plycW4ChMap[polych]]*pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_1*pp->SP4[polych][lut] +
										pp->f64WeightFactor[pp->plycW5ChMap[polych]]*pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_1*pp->SP5[polych][lut]), 2);
					pp->f64K3 = pp->f64WeightFactor[pp->plycW2ChMap[polych]] * pow(pp->SP2[polych][lut], 2) + pp->f64WeightFactor[pp->plycW3ChMap[polych]] * pow(pp->SP3[polych][lut], 2) +
								pp->f64WeightFactor[pp->plycW4ChMap[polych]] * pow(pp->SP4[polych][lut], 2) + pp->f64WeightFactor[pp->plycW5ChMap[polych]] * pow(pp->SP5[polych][lut], 2) ;

					pp->f64Step2[lut] = pp->f64K1[polych] - pp->f64K2 /pp->f64K3;

#if 0
					if( (polych ==0) && (pSTDdev->BoardID ==0) ) {
						printf("%s:Signal, %d, %d, %d, %d, %d \n", pSTDdev->taskName, pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_1 );
						printf("%s:LUT[%d], %f, %f, %f, %f, %f \n", pSTDdev->taskName, lut, pp->SP1[polych][lut], pp->SP2[polych][lut], pp->SP3[polych][lut], pp->SP4[polych][lut], pp->SP5[polych][lut] );
							
							
						printf("%s:WF, %f, %f, %f, %f, %f \n", pSTDdev->taskName, pp->f64WeightFactor[pp->plycW1ChMap[polych]], 
							pp->f64WeightFactor[pp->plycW2ChMap[polych]], 
							pp->f64WeightFactor[pp->plycW3ChMap[polych]],
							pp->f64WeightFactor[pp->plycW4ChMap[polych]], 
							pp->f64WeightFactor[pp->plycW5ChMap[polych]] );
						
						printf("%s:polyc%d\t K2:%f, K3:%f,   K^2[%d]: %f\n\n", pSTDdev->taskName, polych, pp->f64K2, pp->f64K3, lut, pp->f64Step2[lut]  );
					}
#endif
					if( pSTDdev->BoardID ==0 ) /* print K2 value for all LUT counter */
						fprintf( pp->fpK2[polych], "Polyc:%d cnt:%d %0.2f\n", polych, lut, pp->f64Step2[lut]);

					/* find lowest Te value */
					if( pp->f64Step2[lut] < tmpTe[polych] ) 
					{
						tmpTe[polych] = pp->f64Step2[lut];
						fndCnt = lut;
						fetchK2 = pp->f64K2;
						fetchK3 = pp->f64K3;
#if 0
						if( (polych ==0) && (pSTDdev->BoardID ==0) ) {
							printf("%s, lineNum %d: wf:%f, Te[%d] = %f\n", pSTDdev->taskName,lut, pp->f64WeightFactor[lut],  polych, tmpTe[polych] );
/*							printf("%s\t(cur:%d, avrBG:%.2f, meanAvr:%.2f=scat:%.2f), wf(%f,%f,%f,%f,%f): TeV[%d] = %f\n", pSTDdev->taskName, 
								pnode->STbuf[0][i].raw_cur_1 , pp->f32AvrBg[0], ( pp->ST_Ch[0].mean_value - pp->ST_Ch[0].mean_value_bg ), pp->n32ScatterData[0],
								pp->f64WeightFactor[0], pp->f64WeightFactor[1], pp->f64WeightFactor[2], pp->f64WeightFactor[3], pp->f64WeightFactor[4],
								polych, tmpTe[polych]); */
						}
#endif
					}

				}
				/* End Of Loop for Lookup table ____ for( lut=0; lut<CNT_LOOKUP_TABLE; lut++) ____ */
				
				pp->f64Te[polych] = (fndCnt+1)*5;
#if 0
				if( (polych ==0) && (pSTDdev->BoardID ==0) ) {
/*					printf("%s, Te %d:\t(cur:%d, avrBG:%.2f, meanAvr:%.2f=scat:%.2f), wf(%f,%f,%f,%f,%f): TeV[%d] = %f\n", pSTDdev->taskName, (int)pp->f64Te[polych],
						pnode->STbuf[0][i].raw_cur_1 , pp->f32AvrBg[0], ( pp->ST_Ch[0].mean_value - pp->ST_Ch[0].mean_value_bg ), pp->n32ScatterData[0],
						pp->f64WeightFactor[0], pp->f64WeightFactor[1], pp->f64WeightFactor[2], pp->f64WeightFactor[3], pp->f64WeightFactor[4],
						polych, tmpTe[polych]); */

					if( (polych ==0) && (pSTDdev->BoardID ==0) ) {					

						printf("%s:Bg_prev, %d, %d, %d, %d, %d \n", pSTDdev->taskName, pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_2 );
						printf("%s:Signal, %d, %d, %d, %d, %d \n", pSTDdev->taskName, pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_1 );
						printf("%s:Bg_next, %d, %d, %d, %d, %d \n", pSTDdev->taskName, pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur );
							
						printf("%s:WF, %f, %f, %f, %f, %f \n", pSTDdev->taskName, pp->f64WeightFactor[pp->plycW1ChMap[polych]], 
							pp->f64WeightFactor[pp->plycW2ChMap[polych]], 
							pp->f64WeightFactor[pp->plycW3ChMap[polych]],
							pp->f64WeightFactor[pp->plycW4ChMap[polych]], 
							pp->f64WeightFactor[pp->plycW5ChMap[polych]] );
						
						printf("%s:LUT[%d], %0.8f, %0.8f, %0.8f, %0.8f, %0.8f \n", pSTDdev->taskName, fndCnt, 
							pp->SP1[polych][fndCnt], 
							pp->SP2[polych][fndCnt], 
							pp->SP3[polych][fndCnt], 
							pp->SP4[polych][fndCnt], 
							pp->SP5[polych][fndCnt] );
						
						printf("%s:polyc%d\t K1:%f, K2:%f, K3:%f,   K^2[%d]: %f,  Te:%f\n\n", pSTDdev->taskName, polych, pp->f64K1[polych] , fetchK2, fetchK3, fndCnt, pp->f64Step2[fndCnt], pp->f64Te[polych]  );
					}
					
				}
				
#endif
				if(  (pSTDdev->BoardID ==0) ) /* print K2 add seperate line at the end of total table */
				{
					fprintf( pp->fpK2[polych], "%04d %03d Te %04d (%d,%d,%d/ %d,%d,%d/ %d,%d,%d/ %d,%d,%d/ %d,%d,%d)\n", 
						pp->cntTe, fndCnt, (int)pp->f64Te[polych],
						pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_2, 
						pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_1, 
						pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur, 
						pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_2, 
						pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_1, 
						pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur, 
						pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_2, 
						pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_1, 
						pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur, 
						pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_2, 
						pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_1, 
						pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur, 
						pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_2, 
						pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_1, 
						pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur 
						);
					fprintf( pp->fpK2[polych], "           %s:LUT[%d], %0.8f, %0.8f, %0.8f, %0.8f, %0.8f\n", pSTDdev->taskName, fndCnt, 
							pp->SP1[polych][fndCnt], 
							pp->SP2[polych][fndCnt], 
							pp->SP3[polych][fndCnt], 
							pp->SP4[polych][fndCnt], 
							pp->SP5[polych][fndCnt] );
					fprintf( pp->fpK2[polych], "              polych%d\tK1:%f K2:%f K3:%f   K^2[%d]:%f\n\n", polych+1, pp->f64K1[polych] , fetchK2, fetchK3, fndCnt, pp->f64Step2[fndCnt] );
				}
				
				fprintf( pp->fpTe[polych], "%03d Te %04d\n", pp->cntTe, (int)pp->f64Te[polych] );


			}
			/* End Of Loop for each polychromater ____ for(j=0; j<MAX_PLYC_PER_BOARD; j++) ____ */
			pp->cntTe++;


			scanIoRequest(pSTDdev->ioScanPvt_userCall);

	
		}
		/* End Of Loop for read block counter ______ for( i=0; i<qdata.cnt_block_read; i++)____  */


		if( qdata.opcode == OPCODE_NORMAL_CLOSE) 
		{
			for( i=0; i< MAX_PLYC_PER_BOARD; i++) 
			{
				if( pp->fpTe[i] != NULL ) {
					fclose( pp->fpTe[i] );
					pp->fpTe[i] = NULL;
				}
				if( pp->fpK2[i] != NULL ) {
					fclose( pp->fpK2[i] );
					pp->fpK2[i] = NULL;
				}
			}
		}

	

		

		ellAdd(pp->pList_BufferNode, &pnode->node);
		ellCnt = ellCount(pp->pList_BufferNode);
#if 0
		printf("%s: Calculation ELL cnt: %d\n", pSTDdev->taskName, ellCnt ); 
#endif
		
		
		if( ellCnt < MAX_CUTOFF_BUF_NODE)
			notify_error(1, "%s: DMA must Stop! Ell cnt:%d\n", 
					pST_BufThread->threadName,  ellCnt);

		
	}

	return;
}

#endif

#if 0
void threadFunc_Calc_RingBuf(void *param)
{
	int i,polych,lut, ellCnt=0;
	int ch;


	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_threadCfg *pST_BufThread;

	ST_Calc_Buff_node qdata;
	ST_DATA_node *pnode;	
	ST_V792 *pp;

	pp = (ST_V792*)pSTDdev->pUser;
	pST_BufThread = (ST_threadCfg*) pp->pST_BuffThread;
	


#if 0
	epicsPrintf("\n %s: pSTDdev: %p, pp: %p , pST_BuffThread: %p\n", pSTDdev->taskName, pSTDdev, pp, pST_BufThread );
#endif

	for(i = 0; i< MAX_RING_BUF_NUM; i++)
	{
		ST_DATA_node *pnode1 = NULL;
		pnode1 = (ST_DATA_node*) malloc(sizeof(ST_DATA_node));
		if(!pnode1) {
			epicsPrintf("\n>>> threadFunc_Calc_RingBuf: malloc(sizeof(ST_DATA_node)) , index=%d... fail\n", i);
			return;
		}
		ellAdd(pp->pList_BufferNode, &pnode1->node);
	}
	epicsPrintf(" %s: create %d node (size:%dKB)\n", pST_BufThread->threadName,  MAX_RING_BUF_NUM, sizeof(ST_DATA_node)/1024 );

#if 0
	epicsPrintf(">>> %s, ellCnt:%d\n", pringThreadConfig->threadName, 
												ellCount(pAcq196->pdrvBufferConfig->pbufferList));
#endif
	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pST_BufThread->threadQueueId, (void*) &qdata, sizeof(ST_Calc_Buff_node));
#if 0
		printf("%s: got msg cnt_block_read: %d\n", pSTDdev->taskName, 
			qdata.cnt_block_read ); 
#endif 

		pnode = qdata.pNode;

#if 0 /* fixed value for calculation test */
		for( i=0; i<qdata.cnt_block_read; i++) 
		{
/*			for( ch=0; ch < pp->chNum; ch++) {
				pnode->STbuf[ch][i].raw_cur_2 = 0;
				pnode->STbuf[ch][i].raw_cur_1 = 50.991318535920946;
				pnode->STbuf[ch][i].raw_cur = 0;
			}
*/
/*			pnode->STbuf[0][i].raw_cur_2 = 0;
			pnode->STbuf[0][i].raw_cur = 0;

			pnode->STbuf[1][i].raw_cur_2 = 5;
			pnode->STbuf[1][i].raw_cur = 5;

			pnode->STbuf[2][i].raw_cur_2 = 20;
			pnode->STbuf[2][i].raw_cur = 20;

			pnode->STbuf[3][i].raw_cur_2 = 10;
			pnode->STbuf[3][i].raw_cur = 10;

			pnode->STbuf[4][i].raw_cur_2 = 30;
			pnode->STbuf[4][i].raw_cur = 30;
*/
			pnode->STbuf[0][i].raw_cur_2 = 500;
			pnode->STbuf[0][i].raw_cur = 500;

			pnode->STbuf[1][i].raw_cur_2 = 424;
			pnode->STbuf[1][i].raw_cur = 405;

			pnode->STbuf[2][i].raw_cur_2 = 836;
			pnode->STbuf[2][i].raw_cur = 870;

			pnode->STbuf[3][i].raw_cur_2 = 640;
			pnode->STbuf[3][i].raw_cur = 630;

			pnode->STbuf[4][i].raw_cur_2 = 838;
			pnode->STbuf[4][i].raw_cur = 850;			

/*50ev  */
/*			pnode->STbuf[0][i].raw_cur_1 = 94; 
			pnode->STbuf[1][i].raw_cur_1 = 57;	
			pnode->STbuf[2][i].raw_cur_1 = 115; 
			pnode->STbuf[3][i].raw_cur_1 = 49;	
			pnode->STbuf[4][i].raw_cur_1 = 31;
*/

/*100ev  */
/*			pnode->STbuf[0][i].raw_cur_1 = 70.78560466909947; 
			pnode->STbuf[1][i].raw_cur_1 = 46.413811477130444;	
			pnode->STbuf[2][i].raw_cur_1 = 111.81382837751927; 
			pnode->STbuf[3][i].raw_cur_1 = 97.78193470948456;	
			pnode->STbuf[4][i].raw_cur_1 = 32.87173198125989;
*/
/* 300 ev*/
/*
			pnode->STbuf[0][i].raw_cur_1 = 44.573717176728515;
			pnode->STbuf[1][i].raw_cur_1 = 31.59636897092137; 
			pnode->STbuf[2][i].raw_cur_1 = 86.25156408911836; 
			pnode->STbuf[3][i].raw_cur_1 = 158.2393339634496; 
			pnode->STbuf[4][i].raw_cur_1 = 76.64101258854444; */
/*1200	*/
/*			pnode->STbuf[0][i].raw_cur_1 = 26.183516212303044; 
			pnode->STbuf[1][i].raw_cur_1 = 19.873139369654478;
			pnode->STbuf[2][i].raw_cur_1 = 56.309520797214034; 
			pnode->STbuf[3][i].raw_cur_1 = 132.5013243891952;	
			pnode->STbuf[4][i].raw_cur_1 = 225.7217443314165;  */

			pnode->STbuf[0][i].raw_cur_1 = 1168; 
			pnode->STbuf[1][i].raw_cur_1 = 424;
			pnode->STbuf[2][i].raw_cur_1 = 836; 
			pnode->STbuf[3][i].raw_cur_1 = 640;	
			pnode->STbuf[4][i].raw_cur_1 = 838; 


		}
#endif


		for( i=0; i<qdata.cnt_block_read; i++) 
		{
			float64 tmpTe[MAX_PLYC_PER_BOARD];
			int fndCnt = 0;
			UINT16 raw_2, raw_1, raw, tAct, tBg;

#if 1
			if( pnode->STbuf[0][i].isActual ) 
			{
/*				printf("%s: got actual signal. skip this\n", pSTDdev->taskName); */
				continue;
			}
/*			else {
				printf("%s: got background signal. solve Te\n", pSTDdev->taskName);
			}
*/
#endif
			for( ch=0; ch < pp->chNum; ch++) 
			{
				/* noise compensation.... 2010. 9. 21 */
				raw_2 = (unsigned short)((float)pnode->STbuf[ch][i].raw_cur_2 * 0.01);
				raw_1 = (unsigned short)((float)pnode->STbuf[ch][i].raw_cur_1 * 0.01);
				raw = (unsigned short)((float)pnode->STbuf[ch][i].raw_cur * 0.01);

				pnode->STbuf[ch][i].raw_cur_2 = raw_2 *100;
				pnode->STbuf[ch][i].raw_cur_1 = raw_1 *100;
				pnode->STbuf[ch][i].raw_cur = raw *100;

				tAct = (unsigned short)(pp->ST_Ch[ch].mean_value * 0.01 );
				tBg = (unsigned short)(pp->ST_Ch[ch].mean_value_bg * 0.01 );

				pp->ST_Ch[ch].mean_value = tAct * 100;
				pp->ST_Ch[ch].mean_value_bg = tBg * 100;

/*				if(  pnode->STbuf[ch][i].raw_cur_2 >= pnode->STbuf[ch][i].raw_cur_1 )  pnode->STbuf[ch][i].raw_cur_2 = pnode->STbuf[ch][i].raw_cur_1 -1;
				if(  pnode->STbuf[ch][i].raw_cur >= pnode->STbuf[ch][i].raw_cur_1 )  pnode->STbuf[ch][i].raw_cur = pnode->STbuf[ch][i].raw_cur_1 -1; */
				/* noise compensation.... 2010. 9. 21 ***************************************/
				
				pp->f32AvrBg[ch] = (pnode->STbuf[ch][i].raw_cur_2 + pnode->STbuf[ch][i].raw_cur) / 2.0;

/*				if( pp->ST_Ch[ch].mean_value < pp->ST_Ch[ch].mean_value_bg ) */
					pp->n32ScatterData[ch] = pnode->STbuf[ch][i].raw_cur_1 - pp->f32AvrBg[ch];
/*				else 
					pp->n32ScatterData[ch] = pnode->STbuf[ch][i].raw_cur_1 - pp->f32AvrBg[ch]  - ( pp->ST_Ch[ch].mean_value - pp->ST_Ch[ch].mean_value_bg );   neglect this 2010. 10. 27*/

				if( pp->n32ScatterData[ch] < 0 )
					pp->n32ScatterData[ch] = 0;

			}

			for(polych=0; polych < pp->PolycMaxNum;  polych++) 
			{
				tmpTe[polych] = 999999999.0; /* initialize */
				fndCnt = 0;
			
				for( lut=0; lut<CNT_LOOKUP_TABLE; lut++)
				{

					pp->f64Step1[pp->plycW1ChMap[polych]][lut] = pp->n32ScatterData[pp->plycW1ChMap[polych]] / pp->SP1[polych][lut] ;
					pp->f64Step1[pp->plycW2ChMap[polych]][lut] = pp->n32ScatterData[pp->plycW2ChMap[polych]] / pp->SP2[polych][lut] ;
					pp->f64Step1[pp->plycW3ChMap[polych]][lut] = pp->n32ScatterData[pp->plycW3ChMap[polych]] / pp->SP3[polych][lut] ;
					pp->f64Step1[pp->plycW4ChMap[polych]][lut] = pp->n32ScatterData[pp->plycW4ChMap[polych]] / pp->SP4[polych][lut] ;
					pp->f64Step1[pp->plycW5ChMap[polych]][lut] = pp->n32ScatterData[pp->plycW5ChMap[polych]] / pp->SP5[polych][lut] ;

					pp->f64Step2[lut] = ( pp->f64Step1[pp->plycW1ChMap[polych]][lut] + 
						pp->f64Step1[pp->plycW2ChMap[polych]][lut] + 
						pp->f64Step1[pp->plycW3ChMap[polych]][lut] + 
						pp->f64Step1[pp->plycW4ChMap[polych]][lut] + 
						pp->f64Step1[pp->plycW5ChMap[polych]][lut] ) / 5.0 ;
					
					pp->f64Step3[polych][lut] = ( 
						pow((pp->f64Step1[pp->plycW1ChMap[polych]][lut] -pp->f64Step2[lut]), 2) + 
						pow((pp->f64Step1[pp->plycW2ChMap[polych]][lut] -pp->f64Step2[lut]), 2) + 
						pow((pp->f64Step1[pp->plycW3ChMap[polych]][lut] -pp->f64Step2[lut]), 2) + 
						pow((pp->f64Step1[pp->plycW4ChMap[polych]][lut] -pp->f64Step2[lut]), 2) + 
						pow((pp->f64Step1[pp->plycW5ChMap[polych]][lut] -pp->f64Step2[lut]), 2)  ) / 5.0;

					pp->f64Step3[polych][lut] = sqrt( pp->f64Step3[polych][lut]  );


					if( pp->f64Step3[polych][lut] < tmpTe[polych] ) 
					{
						tmpTe[polych] = pp->f64Step3[polych][lut];
						fndCnt = lut;

						fprintf( pp->fpK2[polych], "%04d %03d (%d,%d,%d/ %d,%d,%d/ %d,%d,%d/ %d,%d,%d/ %d,%d,%d)\n", 
							pp->cntTe, fndCnt, 
							pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW1ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW2ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW3ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW4ChMap[polych]][i].raw_cur, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_2, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur_1, 
							pnode->STbuf[pp->plycW5ChMap[polych]][i].raw_cur );
						fprintf( pp->fpK2[polych], " pre-daq(%0.2f, %0.2f) (%0.2f, %0.2f) (%0.2f, %0.2f) (%0.2f, %0.2f) (%0.2f, %0.2f)\n", 
							pp->ST_Ch[pp->plycW1ChMap[polych]].mean_value , 
							pp->ST_Ch[pp->plycW1ChMap[polych]].mean_value_bg,
							pp->ST_Ch[pp->plycW2ChMap[polych]].mean_value , 
							pp->ST_Ch[pp->plycW2ChMap[polych]].mean_value_bg,
							pp->ST_Ch[pp->plycW3ChMap[polych]].mean_value , 
							pp->ST_Ch[pp->plycW3ChMap[polych]].mean_value_bg,
							pp->ST_Ch[pp->plycW4ChMap[polych]].mean_value , 
							pp->ST_Ch[pp->plycW4ChMap[polych]].mean_value_bg,
							pp->ST_Ch[pp->plycW5ChMap[polych]].mean_value , 
							pp->ST_Ch[pp->plycW5ChMap[polych]].mean_value_bg );

						fprintf( pp->fpK2[polych], " scattered(%d %d %d %d %d) ", 
							pp->n32ScatterData[pp->plycW1ChMap[polych]], 
							pp->n32ScatterData[pp->plycW2ChMap[polych]], 
							pp->n32ScatterData[pp->plycW3ChMap[polych]], 
							pp->n32ScatterData[pp->plycW4ChMap[polych]], 
							pp->n32ScatterData[pp->plycW5ChMap[polych]]   );
						fprintf( pp->fpK2[polych], " %s:LUT[%d], %0.8f, %0.8f, %0.8f, %0.8f, %0.8f\n", pSTDdev->taskName, lut, 
								pp->SP1[polych][lut], 
								pp->SP2[polych][lut], 
								pp->SP3[polych][lut], 
								pp->SP4[polych][lut], 
								pp->SP5[polych][lut] );
						fprintf( pp->fpK2[polych], " step2 %f, step3 %f, Te:%d\n\n",pp->f64Step2[lut], pp->f64Step3[polych][lut], (lut+1)*5);
					
					}


				}

				pp->f64Te[polych] = (fndCnt+1)*5;

				fprintf( pp->fpTe[polych], "%03d Te %04d\n", pp->cntTe, (int)pp->f64Te[polych] );
				
			}

			pp->cntTe++;


			scanIoRequest(pSTDdev->ioScanPvt_userCall);

	
		}
		/* End Of Loop for read block counter ______ for( i=0; i<qdata.cnt_block_read; i++)____  */


		if( qdata.opcode == OPCODE_NORMAL_CLOSE) 
		{
			for( i=0; i< pp->PolycMaxNum; i++) 
			{
				if( pp->fpTe[i] != NULL ) {
					fclose( pp->fpTe[i] );
					pp->fpTe[i] = NULL;
				}
				if( pp->fpK2[i] != NULL ) {
					fclose( pp->fpK2[i] );
					pp->fpK2[i] = NULL;
				}
			}
		}


		ellAdd(pp->pList_BufferNode, &pnode->node);
		ellCnt = ellCount(pp->pList_BufferNode);
#if 0
		printf("%s: Calculation ELL cnt: %d\n", pSTDdev->taskName, ellCnt ); 
#endif
		
		
		if( ellCnt < MAX_CUTOFF_BUF_NODE)
			notify_error(1, "%s: DMA must Stop! Ell cnt:%d\n", 
					pST_BufThread->threadName,  ellCnt);

		
	}

	return;
}

#endif



void threadFunc_V792_CatchEnd(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_V792 *pV792 = NULL;
	ST_MASTER *pMaster;
	char strVal[16];


	while (!pSTDdev)  {
		printf("ERROR! threadFunc_V792_CatchEnd() has null pointer of STD device.\n");
		epicsThreadSleep(.1);
	}
	pV792 = (ST_V792 *)pSTDdev->pUser;
	pMaster = get_master();

	if( pSTDdev->BoardID != 0 ) return; /* only work for first board, master */
	
	while(TRUE) 
	{
		epicsEventWait( pSTDdev->ST_CatchEndThread.threadEventId);
		printf("%s: got End event! it will sleep %d sec. \n", pSTDdev->taskName, pV792->my_shot_period);
		
		epicsThreadSleep(pV792->my_shot_period+2);

		

		printf("%s: Master send TS1 system stop \n", pSTDdev->taskName);
/*		db_put(SYS_RUN, "0"); */
		db_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
		epicsThreadSleep(1.5);
		printf("%s: Master send TS1 disarming \n", pSTDdev->taskName);
		db_put(pMaster->strPvNames[NUM_SYS_ARMING], "0");
		epicsThreadSleep(0.5);

		/* send TSSC stop signal */
		/* performed in TSSC  2010. 10. 27 */
/*		printf("%s: send TSSC stop pulse.\n", pSTDdev->taskName);
		db_put("TS1_SOFT_CALL_TSSC_STOP_PULSE", "1");
		epicsThreadSleep(1.);
*/
/*		printf("%s: send TS1 release arming \n", pSTDdev->taskName);
		db_put("TS1_SYS_ARMING", "0");
*/

		db_get("TS1_TSSC_LTU_DG535_us", strVal);
		sscanf(strVal, "%d", &pV792->time_LTU_DG535_us);

		db_get("TS1_TSSC_PULSE_CNT", strVal);
		sscanf(strVal, "%u", &pV792->count_DG535_IN_PULSE);

		db_get("TS1_TSSC_BG_DELAY_us", strVal);
		sscanf(strVal, "%u", &pV792->time_Bg_delay_us);

		drvV792_assign_after_shot_info();
/*
		printf("%s: send TSSC stop pulse reset.\n", pSTDdev->taskName);
		db_put("TS1_SOFT_CALL_TSSC_STOP_PULSE", "0");
*/
/*		printf("%s: Header compensation true:%d, fixed:%d\n", pSTDdev->taskName, pV792->check_accum_cnt, pV792->accum_cnt ); */
		epicsThreadSleep(1.0);
		if( pMaster->cUseAutoSave )
			db_put("TS1_SEND_DATA", "1");
		
	}/* eof  While(TRUE) */

}


/*************************************************
to be preceded befor Admin driver init.
**************************************************/	
static long drvV792_init_driver(void)
{
	ST_MASTER *pAdminConfig = NULL;
	ST_V792 *pV792 = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pAdminConfig = get_master();
	if(!pAdminConfig)	return -1;

	if( CAENVME_handle_init() == WR_ERROR ) 
	{
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		notify_error(1, "ERROR! CAENVME handle init. failed!\n" );
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		return -1;
	}

	pSTDdev = (ST_STD_device*) ellFirst(pAdminConfig->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) ||
			!strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2))
		{
			if(create_v792_taskConfig( pSTDdev) == WR_ERROR) {
				printf("ERROR!	\"%s\" create_v792_taskConfig() failed.\n", pSTDdev->taskName );
				return -1;
			}
			init_my_SFW_v792(pSTDdev);
	

			pV792 = (ST_V792 *)pSTDdev->pUser;

			if ( cvt_V792_open( &pV792->board_data, pV792->m_base_address, vme_handle, pV792->m_qtp_type) == FALSE) {
				epicsPrintf( "\nError %s: executing cvt_V792_open \n", pSTDdev->taskName);
				return -1;
			}


			drvV792_status_reset(pSTDdev); /* add new function by woong  2009.7.15 */
			
			if( drvV792_set_defaultValue(pSTDdev) == WR_OK )
				pSTDdev->StatusDev |= TASK_STANDBY;

	/*		drvV792_show_card_info(pSTDdev); */

//			drvV792_set_TagName(pSTDdev);  /* assign just my calibration tag name */
//			drvV792_init_Lookup_table(pSTDdev);
//			drvV792_init_Polyc_Ch_mapping(pSTDdev);
		
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pV792) { */

/*	drvV792_init_SP_Table(); */

	printf("Target driver initialized.... OK!\n");

	return 0;
}


static long drvV792_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_V792 *pV792;
	ST_MASTER *pAdminCfg = get_master();

	if(!pAdminCfg) return -1;

	if(ellCount(pAdminCfg->pList_DeviceTask))
		pSTDdev = (ST_STD_device*) ellFirst (pAdminCfg->pList_DeviceTask);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pAdminCfg->pList_DeviceTask));

	if(level<1) return -1;

	while(pSTDdev) {
		pV792 = (ST_V792*)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pV792->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pV792->adjCount_smplRate,
					                                                 pV792->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pV792->adjCount_Gain,
					                                                   pV792->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pV792->pchannelConfig)>0) print_channelConfig(pV792,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}


int drvV792_ARM_enable(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	if( !(pSTDdev->StatusDev & TASK_STANDBY) ) 
	{
		epicsPrintf("ERROR!   %s not ready! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR!   %s already arming! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}

	if ( !cvt_V792_data_clear( &pV792->board_data)) {
		epicsPrintf( "\nError %s: executing cvt_V792_data_clear \n", pSTDdev->taskName );
		return WR_ERROR;
	}
		
	if( drvV792_openRawFile(pSTDdev) == WR_ERROR) {
		notify_error(1, "%s: drvV792_openRawFile().. FAIL", pSTDdev->taskName);
		return WR_ERROR;
	}
	
	drvV792_reset_calibration_param(pSTDdev);
	
	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_ARM_ENABLED;

	return WR_OK;
}
int drvV792_ARM_disable(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	int i;
/*
	if( !(pSTDdev->StatusDev & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf("ERROR!   %s not yet armed! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev & TASK_IN_PROGRESS ) 
	{
		epicsPrintf("ERROR!    %s already run! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
*/

	for( i=0; i< pV792->chNum; i++) 
	{
		if( pV792->fpRaw[i] != NULL ) {
			fclose( pV792->fpRaw[i] );
			pV792->fpRaw[i] = NULL;
		}
	}
/*	for( i=0; i< MAX_PLYC_PER_BOARD; i++) 
	{
		if( pV792->fpTe[i] != NULL ) {
			fclose( pV792->fpTe[i] );
			pV792->fpTe[i] = NULL;
		}
		if( pV792->fpK2[i] != NULL ) {
			fclose( pV792->fpK2[i] );
			pV792->fpK2[i] = NULL;
		}
	} */
/*	pV792->cnt_block_read = 0; */
	pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;

	return WR_OK;
}

int drvV792_RUN_start(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	if( !(pSTDdev->StatusDev & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf("ERROR!   %s need to arming! please check again. \n", pSTDdev->taskName);
		return WR_ERROR;
	}
	if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev & TASK_IN_PROGRESS ) 
	{
		epicsPrintf("ERROR!   %s already run! please check again. \n", pSTDdev->taskName);
		return WR_ERROR;
	}
	
	/* data, event counter reset */
	if ( !cvt_V792_data_clear( &pV792->board_data)) {
		epicsPrintf( "\nError %s: executing cvt_V792_data_clear \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	pV792->cmd_DAQ_stop = 0;
		
	epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId );

	return WR_OK;
}

int drvV792_RUN_stop(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev & TASK_IN_PROGRESS ) 
	{
		pV792->cmd_DAQ_stop = 1;
		printf("%s: DAQ stop Cmd set ON!\n", pSTDdev->taskName);
	}
	else {
		epicsPrintf(">> Request Stop but \"%s\" not run!\n", pSTDdev->taskName);
		return WR_ERROR;
	}
	
	return WR_OK;
}


int drvV792_openRawFile(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	int i;
	char strbuff[64];

	if( !pV792->use_file_save ){
		printf("%s: not used raw file!\n", pSTDdev->taskName );
		return WR_OK;
	}

	for( i=0; i<pV792->chNum; i++) 
	{
		sprintf(strbuff, "%s/%d_b%d.%02d.dat", STR_CHANNEL_DATA_DIR, (int)pSTDdev->ST_Base.shotNumber, pSTDdev->BoardID,  i); 
		pV792->fpRaw[i]  = fopen(strbuff, "wb");
		if ( pV792->fpRaw[i]  == NULL)
		{
			printf("WR_ERROR: failed to open file %s\n", strbuff);		
			return WR_ERROR;
		} 
	}
	return WR_OK;	
}

int drvV792_openTeFile(ST_STD_device *pSTDdev)
{
#if 0
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	int i;
	char strbuff[64];

	for( i=0; i<pV792->PolycMaxNum; i++) 
	{
		sprintf(strbuff, "%s/%lu.B%d.ply%d.txt", STR_Te_DATA_DIR, pSTDdev->ST_Base.shotNumber, pSTDdev->BoardID,  i+1); 
		if ( pV792->fpTe[i]  != NULL){
			fclose( pV792->fpTe[i] );
			pV792->fpTe[i] = NULL;
		}
		pV792->fpTe[i]  = fopen(strbuff, "w");
		if ( pV792->fpTe[i]  == NULL)
		{
			printf("WR_ERROR: failed to open file %s\n", strbuff);		
			return WR_ERROR;
		} 

		sprintf(strbuff, "%s/%lu.B%d.ply%d.txt", STR_K2_DATA_DIR, pSTDdev->ST_Base.shotNumber, pSTDdev->BoardID,  i+1); 
		if ( pV792->fpK2[i]  != NULL){
			fclose( pV792->fpK2[i] );
			pV792->fpK2[i] = NULL;
		}
		pV792->fpK2[i]  = fopen(strbuff, "w");
		if ( pV792->fpK2[i]  == NULL)
		{
			printf("WR_ERROR: failed to open file %s\n", strbuff);		
			return WR_ERROR;
		} 

	}
#endif
	return WR_OK;	
}



int drvV792_open_card(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792*)pSTDdev->pUser;
	if ( cvt_V792_open( &pV792->board_data, pV792->m_base_address, vme_handle, pV792->m_qtp_type) == FALSE ) {
		epicsPrintf( "\nError %s: executing cvt_V792_open(). \n", pSTDdev->taskName);
		return WR_ERROR;
	}
	return WR_OK;
}
int drvV792_close_card(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792*)pSTDdev->pUser;
	if ( cvt_V792_close( &pV792->board_data) == FALSE) {
		epicsPrintf( "\nError %s: executing cvt_V792_close \n", pSTDdev->taskName);
		return WR_ERROR;
	}
	return WR_OK;
}

int drvV792_show_card_info(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792*)pSTDdev->pUser;
	UINT16 reg_value;
	if ( !cvt_V792_get_system_info( &pV792->board_data, &pV792->firmware_rev, &pV792->piggy_back_type, &pV792->serial_number)) {
		epicsPrintf( "\nError %s: executing cvt_V792_get_system_info \n", pSTDdev->taskName);
		return WR_ERROR;
	}

	epicsPrintf( "\n   %s Firmware Rev.       : %04x\n", pSTDdev->taskName, pV792->firmware_rev);
	epicsPrintf( "   %s Piggy back type     : %s\n", pSTDdev->taskName, cvt_V792_get_piggy_back_str( &pV792->board_data, pV792->piggy_back_type));
	epicsPrintf( "   %s Serial Number       : %04x\n", pSTDdev->taskName, pV792->serial_number);

	cvt_read_reg( &pV792->board_data.m_common_data,  CVT_V792_IPED_INDEX, &reg_value);
	epicsPrintf( "   %s Iped                : %d\n", pSTDdev->taskName, reg_value);

	cvt_read_reg( &pV792->board_data.m_common_data, CVT_V792_MCST_CBLT_ADDRESS_INDEX, &reg_value);
	epicsPrintf("   %s Base Address: 0x%X -> 0x%X\n", pSTDdev->taskName, reg_value, 0xff&reg_value);

	return WR_OK;
}

int drvV792_set_defaultValue(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	/* data, event counter reset */
	if ( !cvt_V792_data_clear( &pV792->board_data)) {
		epicsPrintf( "\nError %s: executing cvt_V792_data_clear \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	if ( !cvt_V792_set_sliding_scale( &pV792->board_data, pV792->m_ST_AcqModeParam.m_sliding_scale_enable)) {
		epicsPrintf( "\nError %s: executing cvt_V792_set_sliding_scale \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	if ( !cvt_V792_set_zero_suppression( &pV792->board_data,
					pV792->m_ST_AcqModeParam.m_zero_suppression_enable,
					pV792->m_ST_zeroParam.m_step_threshold,
					&pV792->m_ST_zeroParam.m_thresholds_buff[0])) {
		epicsPrintf( "Error executing cvt_V792_set_zero_suppression\n");
		return WR_ERROR;
	}

	if ( !cvt_V792_set_acquisition_mode( &pV792->board_data,
			pV792->m_ST_AcqModeParam.m_sliding_scale_enable,
			pV792->m_ST_AcqModeParam.m_zero_suppression_enable,
			pV792->m_ST_AcqModeParam.m_overflow_suppression_enable,
			pV792->m_ST_AcqModeParam.m_empty_enable,
			pV792->m_ST_AcqModeParam.m_count_all_events
	)) {
		epicsPrintf( "Error %s, executing cvt_V792_set_zero_suppression\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	if ( !cvt_V792_set_readout_mode( &pV792->board_data, TRUE, TRUE, TRUE)) {
		epicsPrintf( "Error %s, executing cvt_V792_set_readout_mode \n", pSTDdev->taskName);
		return WR_ERROR;
	}

	if ( !cvt_V792_set_crate_number( &pV792->board_data, (UINT8)(pSTDdev->BoardID+1) )) {
		epicsPrintf( "Error %s, executing cvt_V792_set_crate_number \n", pSTDdev->taskName);
		return WR_ERROR;
	}

	if ( !cvt_V792_set_pedestal( &pV792->board_data, pV792->Pedestal)) {
		notify_error(1,"%s: pedestal(%d)", pSTDdev->taskName, pV792->Pedestal);
		return WR_ERROR;		
	}


	cvt_V792_set_MCST_CBLT_address(  &pV792->board_data, (pV792->board_data.m_common_data.m_base_address)>>8);

#if 0
	if( pSTDdev->BoardID == 0 ) {
		cvt_V792_set_MCST_CBLT_control(  &pV792->board_data, 0x2);
		epicsPrintf("%s, first board in CBLT or MCST chain\n", pSTDdev->taskName );
	}
	else if( pSTDdev->BoardID == 3 ) {
		cvt_V792_set_MCST_CBLT_control(  &pV792->board_data, 0x1);
		epicsPrintf("%s, last board in CBLT or MCST chain\n", pSTDdev->taskName );
	} else {
		cvt_V792_set_MCST_CBLT_control(  &pV792->board_data, 0x3);
		epicsPrintf("%s, active intermediate board in CBLT or MCST chain\n", pSTDdev->taskName );
	}

	cvt_read_reg( &pV792->board_data.m_common_data, CVT_V792_MCST_CBLT_CTRL_INDEX, &reg_value);
	epicsPrintf("%s: CVT_V792_MCST_CBLT_CTRL_INDEX:	   0x%X -> 0x%X\n", pSTDdev->taskName, reg_value, 0x3&reg_value);

/*	cvt_V792_get_MCST_CBLT_control( &pV792->board_data, &u8Val);
	epicsPrintf("%s, read value is 0x%X\n", u8Val ); */
#endif
	
	return WR_OK;
}


int drvV792_set_testFunc(ST_STD_device *pSTDdev)
{
#if 0
	UINT16 reg_value;
	UINT32 read_data_buff;
	int nval;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	pV792->data_size = 1048576; /*1024x1024; */
	pV792->data_buff= malloc( pV792->data_size );
	if ( pV792->data_buff== NULL) {
		epicsPrintf( "Error allocating events' buffer (%d bytes)", pSTDdev->taskName, pV792->data_size);
		return WR_ERROR;
	}

	if ( !cvt_V792_read_MEB( &pV792->board_data, pV792->data_buff, &pV792->data_size)) {
		epicsPrintf( " \nError executing cvt_V792_read_MEB \n");
		return WR_ERROR;
	}
	epicsPrintf("MEV Data Size :   0x%x\n", pV792->data_size);

	for( nval=0; nval<34; nval++) {
		cvt_read( &pV792->board_data.m_common_data, CVT_V792_OUT_BUFFER_ADD, &read_data_buff, cvA32_S_DATA, cvD32);
		epicsPrintf(" %d:   0x%x\n", nval, read_data_buff);
	}

	free(pV792->data_buff);
#endif
	return WR_OK;
}

int drvV792_read_chanData_Loop(ST_STD_device *pSTDdev)
{
	int nflag=0, chId=0, headFlg=0;
	UINT32 read_data_buff;	
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;


	UINT16 reg_value= 0;
/*	pV792->cnt_block_read = 0; */
	
	while(1) 
	{

		if( !cvt_V792_get_status_2( &pV792->board_data, &reg_value))
		{
			epicsPrintf( "\nError %s: executing cvt_V792_get_status_2 \n", pSTDdev->taskName );
//			return readcnt;
			return pV792->cnt_block_read;
		}
		if( reg_value & 0x2 ) {
/*			printf("%s:  [1]-1 Buffer empty.\n", pSTDdev->taskName); */
//			return readcnt; /*epicsPrintf("\t [1]-1 Buffer empty.\n"); */
			return pV792->cnt_block_read;
		}
/*	else epicsPrintf("\t [1]-0 Buffer not empty.\n"); */

		if ( !cvt_read( &pV792->board_data.m_common_data, CVT_V792_OUT_BUFFER_ADD, &read_data_buff, cvA32_S_DATA, cvD32) ) {
			printf("%s: drvV792_read_chanData_Loop():cvt_read() fail!\n", pSTDdev->taskName );
//			return readcnt;
			return pV792->cnt_block_read;
		}
/*		printf("   0x%X  Ch: %d, UN: %d, OV: %d, value: %d (0x%X)\n", read_data_buff, 
				(read_data_buff>>16 ) & 0x1F ,
				(read_data_buff>>13 ) & 0x1 ,
				(read_data_buff>>12 ) & 0x1 ,
				(read_data_buff) & 0xFFF, read_data_buff & 0xFFF	);
*/
		nflag = (read_data_buff>>24 ) & 0x7;
		switch( nflag ) 
		{
			case 2: /* 010 : Header */
				headFlg = 1;
/*				printf(">>>>>>>got header \n"); */
				break;
			case 0: /* 000 : Datum*/  
				if( pV792->m_qtp_type == CVT_V792_TYPE_A) /*CVT_V792_TYPE_A, CVT_V792_TYPE_N */
					chId = (read_data_buff>>16) & 0x1F;
				else 
					chId = (read_data_buff>>17) & 0xF;

				if( chId >= pV792->chNum ) printf("%s: limited Ch ID %d\n", pSTDdev->taskName, chId);

//				pV792->ST_Ch[chId].rawVal_current = read_data_buff  & 0xFFF;
				pV792->rawVal_current[chId] = read_data_buff  & 0xFFF;

//				pV792->ST_Ch[chId].ST_Calc[pV792->cnt_block_read].raw_cur = pV792->ST_Ch[chId].rawVal_current;
				pV792->raw_curr[chId][pV792->cnt_block_read] = pV792->rawVal_current[chId];

/*
				pV792->ST_Ch[chId].ST_Calc[pV792->cnt_block_read].isActual = ( pV792->accum_cnt%2 == 0) ? 1:0; 
				
				if( pV792->ST_Ch[chId].rawVal_min > pV792->ST_Ch[chId].rawVal_current )
					pV792->ST_Ch[chId].rawVal_min = pV792->ST_Ch[chId].rawVal_current;
				if( pV792->ST_Ch[chId].rawVal_max < pV792->ST_Ch[chId].rawVal_current )
					pV792->ST_Ch[chId].rawVal_max = pV792->ST_Ch[chId].rawVal_current;
*/
#if 0
				if( (chId == 0 || chId == 0) && pSTDdev->BoardID == 0)
				{
/*					printf("%s/Ch%d:  %d\n", pSTDdev->taskName, chId, read_data_buff  & 0xFFF);  */
					printf("%s/Ch%d: isActual:%d: %d", pSTDdev->taskName, chId,
						pV792->ST_Ch[chId].ST_Calc[pV792->cnt_block_read].isActual , 
						pV792->ST_Ch[chId].ST_Calc[pV792->cnt_block_read].raw_cur );
					printf("       readCnt: %d, block cnt: %d, accum cnt: %d\n",  
								readcnt,
								pV792->cnt_block_read,
								pV792->accum_cnt );

				}
#endif

#if 0
				if(chId == 0 || chId == 10)
				{
					printf("%s/Ch%d:  %d\n", pSTDdev->taskName, chId, read_data_buff  & 0xFFF); 
				}
#endif
				break;
			case 4: /* 100 : End of Block */
//				readcnt++;
				pV792->cnt_block_read++;
				pV792->accum_cnt++;
//				pV792->check_accum_cnt++;

				if( pV792->cnt_block_read > CNT_QTP_READ_BUFF_EXTRA ) 
					printf("%s: overflow cnt_block_read %d\n", pSTDdev->taskName, pV792->cnt_block_read);
/*				nval++; */
/*				printf("%s: thread loop got End of Block 1 set cnt: %d\n", pSTDdev->taskName, pV792->cnt_block_read ); */

				break;
			case 6: /* 110 : not valid */
#if PRINT_DAQ_DEBUG
				if( headFlg )
					printf("%s: block cnt: %d, accum cnt: %d\n", pSTDdev->taskName, 
					pV792->cnt_block_read,
					pV792->accum_cnt );
#endif
/*				return readcnt; */
				break;
			default: 
//				return readcnt;
				return pV792->cnt_block_read;
			break;
		}
	}

/*	scanIoRequest(pSTDdev->ioScanPvt_userCall); always return without scanIoRequest*/
	
	return pV792->cnt_block_read;
}

int drvV792_read_chanData_clear(ST_STD_device *pSTDdev)
{
	int readcnt=0, nflag=0, chId=0, headFlg=0;
	UINT32 read_data_buff;	
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	UINT16 reg_value= 0;

	while(1) 
	{

		if( !cvt_V792_get_status_2( &pV792->board_data, &reg_value))
		{
			epicsPrintf( "\nError %s: executing cvt_V792_get_status_2 \n", pSTDdev->taskName );
			return readcnt;
		}
		if( reg_value & 0x2 ) {
			printf("%s:  [1]-1 Buffer empty. clear cnt: %d\n", pSTDdev->taskName, readcnt);
			return readcnt; /*epicsPrintf("\t [1]-1 Buffer empty.\n"); */
		}
/*	else epicsPrintf("\t [1]-0 Buffer not empty.\n"); */

		if ( !cvt_read( &pV792->board_data.m_common_data, CVT_V792_OUT_BUFFER_ADD, &read_data_buff, cvA32_S_DATA, cvD32) ) {
			printf("%s: drvV792_read_chanData_Loop():cvt_read() fail!\n", pSTDdev->taskName );
			return readcnt;
		}
/*		printf("   0x%X  Ch: %d, UN: %d, OV: %d, value: %d (0x%X)\n", read_data_buff, 
				(read_data_buff>>16 ) & 0x1F ,
				(read_data_buff>>13 ) & 0x1 ,
				(read_data_buff>>12 ) & 0x1 ,
				(read_data_buff) & 0xFFF, read_data_buff & 0xFFF	);
*/
		nflag = (read_data_buff>>24 ) & 0x7;
		switch( nflag ) 
		{
			case 2: /* 010 : Header */
				headFlg = 1;
/*				printf("%s: >>>>>>>got header \n", pSTDdev->taskName); */
				break;
			case 0: /* 000 : Datum*/  
				if( pV792->m_qtp_type == CVT_V792_TYPE_A) /*CVT_V792_TYPE_A, CVT_V792_TYPE_N */
					chId = (read_data_buff>>16) & 0x1F;
				else 
					chId = (read_data_buff>>17) & 0xF;
				
/*				pV792->ST_Ch[chId].rawVal_current = read_data_buff  & 0xFFF; */

#if 0
				if( chId == 4 || chId == 4 )
				{
					printf("%s/Ch%d:  %d", pSTDdev->taskName, chId,  read_data_buff  & 0xFFF  );
					printf("       readCnt: %d\n",  readcnt );
				}
#endif

				break;
			case 4: /* 100 : End of Block */
				readcnt++;
/*				printf("%s: clear loop got End of Block.  cnt: %d\n", pSTDdev->taskName, readcnt ); */
				break;
			case 6: /* 110 : not valid */
				printf("%s: Not valid!! cnt: %d\n", pSTDdev->taskName, readcnt );
				break;
			default: 
				return readcnt;
			break;
		}
	}


	return readcnt;
}


int drvV792_read_FullData(ST_STD_device *pSTDdev)
{
	int nval, nflag;
/*	UINT32 read_data_buff = 0; */
	UINT32 data_buff[34];
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	for( nval=0; nval<34; nval++) {
		cvt_read( &pV792->board_data.m_common_data, CVT_V792_OUT_BUFFER_ADD, &data_buff[nval], cvA32_S_DATA, cvD32);
	}


	for( nval=0; nval<34; nval++) 
	{
		nflag = (data_buff[nval]>>24 ) & 0x7;
		switch( nflag ) 
		{
			case 2: /* 010 */				
				printf("%s:%d Got Header.  Iped: %d\n", pSTDdev->taskName, nval, pV792->Pedestal );
				printf("      GEO address: 0x%X\n", (data_buff[nval]>>27 ) & 0x1F );
				printf("      Create number: %d\n",  (data_buff[nval]>>16 ) & 0xFF );
				printf("      Memorised channels: %d\n",  (data_buff[nval]>>8 ) & 0x3F );
				break;
			case 0: /* 000 */
				printf("   %d  Ch: %d, UN: %d, OV: %d, value: %d (0x%X)\n", nval, 
				(data_buff[nval]>>16 ) & 0x1F ,
				(data_buff[nval]>>13 ) & 0x1 ,
				(data_buff[nval]>>12 ) & 0x1 ,
				data_buff[nval] & 0xFFF, data_buff[nval] & 0xFFF	);
//				pV792->ST_Ch[(data_buff[nval]>>16 ) & 0x1F].rawVal_current = data_buff[nval] & 0xFFF;
				pV792->rawVal_current[(data_buff[nval]>>16 ) & 0x1F] = data_buff[nval] & 0xFFF;
				break;
			case 4: /* 100 */
				printf("%s:%d Got EOB.\n", pSTDdev->taskName, nval);
				printf("      GEO address: 0x%X\n", (data_buff[nval]>>27 ) & 0x1F );
				printf("       Event Counter: %d (0x%X)\n", data_buff[nval] & 0xFFFFFF, data_buff[nval] & 0xFFFFFF  );
				break;
			case 6: /* 110 */
				printf("%s:%d Not valid datum.\n", pSTDdev->taskName, nval);
				break;
			default: 
				printf("%s:%d, 0x%X is Invalid identified type. \n", pSTDdev->taskName, nval, nflag );
			break;
		}
				

	}

/*	scanIoRequest(pSTDdev->ioScanPvt_userCall); */

	return WR_OK;
}


int drvV792_check_DataReady(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	UINT16 reg_value= 0;
	if( !cvt_read_reg( &pV792->board_data.m_common_data, CVT_V792_STATUS_1_INDEX, &reg_value))
	{
		epicsPrintf( "\nError %s: read CVT_V792_STATUS_1_INDEX \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	if( reg_value & 0x1 ) return WR_OK;
	return WR_ERROR;
}

int drvV792_reset_calibration_param(ST_STD_device *pSTDdev)
{
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	int i;
	for(i = 0; i< pV792->chNum; i++) {
/*		pV792->ST_Ch[i].acuum_value = 0;
		pV792->ST_Ch[i].acuum_value_bg = 0;
		if( pV792->toggle_get_MeanVal ) {
			pV792->ST_Ch[i].mean_value = 0;
			pV792->ST_Ch[i].mean_value_bg = 0;
		}
		pV792->ST_Ch[i].rawVal_min = 4095;
		pV792->ST_Ch[i].rawVal_max = 0;

		pV792->ST_Ch[i].tmp_prev_2  = 0;
		pV792->ST_Ch[i].tmp_prev_1  = 0;
		pV792->ST_Ch[i].rawVal_current  = 0;
*/
		pV792->rawVal_current[i] = 0;
	}
	pV792->accum_cnt = 0;

	return WR_OK;
}

int drvV792_assign_after_shot_info()
{
	
	ST_STD_device *pSTDdev;
	ST_V792 *pV792;

	UINT32 time_LTU_DG535_us;
/*	UINT32 time_LTU_LASER_us; */
	UINT16 count_DG535_IN_PULSE;
	UINT32 time_Bg_delay_us;


	int prev_accum_cnt=0;
	header_dreg = 0;
//	FILE *fp;
//	char buf[128];
//	int i;
//	unsigned short curr_data[2];
//	unsigned short prev_data;
		


	pSTDdev = get_first_STDdev();
	pV792 = (ST_V792 *)pSTDdev->pUser;
	time_LTU_DG535_us = pV792->time_LTU_DG535_us;
/*	time_LTU_LASER_us = pV792->time_LTU_LASER_us; */
	count_DG535_IN_PULSE = pV792->count_DG535_IN_PULSE;
	time_Bg_delay_us = pV792->time_Bg_delay_us;


	if( prev_accum_cnt < pV792->accum_cnt ) {
		header_dreg = pV792->accum_cnt - count_DG535_IN_PULSE;
	}
	prev_accum_cnt = pV792->accum_cnt;

	pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->time_LTU_DG535_us = time_LTU_DG535_us;
/*		pV792->time_LTU_LASER_us = time_LTU_LASER_us; */
		pV792->count_DG535_IN_PULSE = count_DG535_IN_PULSE;
		pV792->time_Bg_delay_us = time_Bg_delay_us;


		if( prev_accum_cnt > pV792->accum_cnt ) {
			header_dreg = pV792->accum_cnt - count_DG535_IN_PULSE;
		}
		prev_accum_cnt = pV792->accum_cnt;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

/***************************/
/* find header_dreg value. temporary. 2011. 6. 20*/
#if 0
	header_dreg = 0;

	pSTDdev = get_first_STDdev();

	sprintf(buf, "%s/%d_b0.00.dat", STR_CHANNEL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber); 
	fp = fopen(buf, "rb");
	fread( curr_data, 2, 1, fp);
	prev_data = curr_data[0];
	for(i=1; i< 100; i++) {
		fread( curr_data, 2, 1, fp);
		if( curr_data[0]  > (prev_data+TEMP_HEADER_DIFF_VAL) ) {
			header_dreg = i;
			break;
		} 
		else
			prev_data = curr_data[0];
	}
	fclose(fp);
#endif

	return WR_OK;
}

int drvV792_get_header_dreg()
{
	return header_dreg;
}

int drvV792_get_info_befor_shot()
{
	ST_STD_device *pSTDdev;
	ST_V792 *pV792;
	char strBuf[16];

	pSTDdev = get_first_STDdev();
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;

		if( db_get("TS_LTU_P0_SEC0_T0", strBuf) == WR_ERROR ) {
			notify_error(1, "'TS_LTU_P0_SEC0_T0' get error!\n");
			return WR_ERROR ;
		} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[0]);

		if( db_get("TS1_TSSC_LASER_CLOCK_hz", strBuf) == WR_ERROR ) {
			notify_error(1, "'TS1_TSSC_LASER_CLOCK_hz' get error!\n");
			return WR_ERROR ;
		} sscanf(strBuf, "%d", &pSTDdev->ST_Base.nSamplingRate);
		printf("%s: sample rate: %d\n", pSTDdev->taskName, pSTDdev->ST_Base.nSamplingRate);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return WR_OK;
}

int drvV792_make_calcBuff(ST_STD_device *pSTDdev)
{
#if 0
	ST_V792 *pV792;
	pV792 = (ST_V792 *)pSTDdev->pUser;

	pV792->pList_BufferNode = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pV792->pList_BufferNode) {
		printf("ERROR! %s malloc ( pList_BufferNode )  failed. \n", pSTDdev->taskName );
		return WR_ERROR;
	} else 
		ellInit(pV792->pList_BufferNode);

	pV792->pST_BuffThread	= (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pV792->pST_BuffThread){
		printf("ERROR! %s malloc ( pST_BuffThread )  failed. \n", pSTDdev->taskName );
		free(pV792->pList_BufferNode);
		return WR_ERROR; 
	}
	sprintf(pV792->pST_BuffThread->threadName, "%s_CalcBuf_ctrl", pSTDdev->taskName);

	pV792->pST_BuffThread->threadPriority = epicsThreadPriorityHigh;
	pV792->pST_BuffThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackMedium);	/* epicsThreadStackBig */
	pV792->pST_BuffThread->threadFunc		= (EPICSTHREADFUNC) threadFunc_Calc_RingBuf;
	pV792->pST_BuffThread->threadParam 	= (void*) pSTDdev;
	pV792->pST_BuffThread->threadQueueId	= epicsMessageQueueCreate(50, sizeof(ST_Calc_Buff_node));

	epicsThreadCreate(pV792->pST_BuffThread->threadName,
			  pV792->pST_BuffThread->threadPriority,
			  pV792->pST_BuffThread->threadStackSize,
			  (EPICSTHREADFUNC) pV792->pST_BuffThread->threadFunc,
			  (void*) pV792->pST_BuffThread->threadParam);

#endif
	return WR_OK;
}

void drvV792_print_Jitter()
{
	int i;
	int jitter[20];
	int gap=0;
	char buf[1024];
	char shortbuf[32];
	ST_STD_device *pSTDdev;
	ST_V792 *pV792;

	pSTDdev = get_first_STDdev();
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;

		for(i=0;i<20;i++) jitter[i]=0;
	
		for( i=0; i< pV792->chNum; i++) 
		{
//			gap = pV792->ST_Ch[i].rawVal_max - pV792->ST_Ch[i].rawVal_min;
/*			value[i]=gap; */
			if( gap > 20 ) {
				printf("ERROR! %s, big difference...Ch%d, gap: %d\n", pSTDdev->taskName, i, gap);
			}
			else
				jitter[gap] += 1;
			if( gap>5)
				printlog("%s: 'Ch%d'=%d \n", pSTDdev->taskName, i, gap );
		}
		sprintf(buf, "%s:jitter ", pSTDdev->taskName);
		for(i=0; i<20; i++) 
		{
			if( jitter[i] != 0 ) 
			{
				sprintf(shortbuf, "[%d]=%d,\t", i, jitter[i] );
				strcat(buf, shortbuf);
/*				printlog("%s:jitter[%d]=%d\t", pSTDdev->taskName, i, jitter[i] ); */				
			}
		}
		printlog("%s\n", buf );

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
/*			
		printlog("%s/Ch%d, [%d, %d] gap:%d\n", pSTDdev->taskName, i, 
		pV792->ST_Ch[i].rawVal_max, pV792->ST_Ch[i].rawVal_min,
		pV792->ST_Ch[i].rawVal_max - pV792->ST_Ch[i].rawVal_min);
*/
	}

}


int drvV792_init_Lookup_table(ST_STD_device *pSTDdev)
{
#if 0
	ST_V792 *pV792;
	FILE *fp;
	char fileName[128];
	char bufLineScan[200];
	char csp1[16], csp2[16], csp3[16], csp4[16], csp5[16];
/*	double dsp1, dsp2, dsp3, dsp4, dsp5; */
	int cnt, nval;
	int i;
	char strTOP[64];

	pV792 = (ST_V792 *)pSTDdev->pUser;

	if( getenv("TOP") ){
		sprintf(strTOP, "%s", getenv("TOP")  );
		printf("%s: TOP is \"%s\"\n",pSTDdev->taskName, strTOP);
	} else {
		printf("ERROR: %s: There is no \"TOP\" environment value\n",pSTDdev->taskName);
		return WR_ERROR;
	}

	for( i=0; i< pV792->PolycMaxNum; i++) 
	{
		sprintf(fileName, "%s/local/B%d.poly%d.txt", strTOP, pSTDdev->BoardID, i+1 );
		if( (fp = fopen(fileName, "r") ) == NULL )
		{	
			notify_error(1, "Can't read 'B%d.poly%d.txt'", pSTDdev->BoardID, i+1);
			continue;
		} 
		cnt = 0;
		while(1) 
		{
			if( fgets(bufLineScan, 200, fp) == NULL ) break;
			if( bufLineScan[0] == '#') ;
			else if( bufLineScan[0] == '\n' ) ;
			else if( bufLineScan[0] == '\r' ) ;
			else if( bufLineScan[0] == ' ' ) ;
			else 
			{
/*				sscanf(bufLineScan, "%d %0.8f %0.8f %0.8f %0.8f %0.8f", &nval, &dsp1, &dsp2, &dsp3, &dsp4, &dsp5); */
				sscanf(bufLineScan, "%d %s %s %s %s %s ", &nval, csp1, csp2, csp3, csp4, csp5);

				pV792->SP1[i][cnt] = strtod(csp1, NULL);
				pV792->SP2[i][cnt] = strtod(csp2, NULL);
				pV792->SP3[i][cnt] = strtod(csp3, NULL);
				pV792->SP4[i][cnt] = strtod(csp4, NULL);
				pV792->SP5[i][cnt] = strtod(csp5, NULL);

/*
				printf("%d: %d %0.12f %0.12f %0.12f %0.12f %0.12f\n", cnt, nval, 
					pV792->SP1[i][cnt], pV792->SP2[i][cnt] , pV792->SP3[i][cnt], pV792->SP4[i][cnt], pV792->SP5[i][cnt] );
*/
				cnt++;
			}
		}
		printf("'%s' has %d count value\n", fileName, cnt );
		
		fclose(fp); 
	}
#endif
	return WR_ERROR;
}

int drvV792_init_Polyc_Ch_mapping(ST_STD_device *pSTDdev)
{
#if 0
	ST_V792 *pV792;
	FILE *fp;
	char buf[64];
	char fileName[128];
	char bufLineScan[64];
	int nCh, cntWL, cntPLYC;
	char strTOP[64];

	pV792 = (ST_V792 *)pSTDdev->pUser;

	if( getenv("TOP") ){
		sprintf(strTOP, "%s", getenv("TOP")  );
		printf("%s: TOP is \"%s\"\n",pSTDdev->taskName, strTOP);
	} else {
		printf("ERROR: %s: There is no \"TOP\" environment value\n",pSTDdev->taskName);
		return WR_ERROR;
	}

	sprintf(fileName, "%s/local/B%d.mapping", strTOP, pSTDdev->BoardID );

	if( (fp = fopen(fileName, "r") ) == NULL )
	{	
		notify_error(1, "Can't read '%s' file", fileName);
		return WR_ERROR;
	} 
	cntWL = cntPLYC = 0;
	while(1) 
	{
		if( fgets(bufLineScan, 64, fp) == NULL ) break;
		if( bufLineScan[0] == '#') ;
		else if( bufLineScan[0] == '\n' ) ;
		else if( bufLineScan[0] == '\r' ) ;
		else if( bufLineScan[0] == ' ' ) ;
		else 
		{
			sscanf(bufLineScan, "%s %d", buf, &nCh);

			switch(cntWL){
				case 0: pV792->plycW1ChMap[cntPLYC] = nCh; break;
				case 1: pV792->plycW2ChMap[cntPLYC] = nCh; break;
				case 2: pV792->plycW3ChMap[cntPLYC] = nCh; break;
				case 3: pV792->plycW4ChMap[cntPLYC] = nCh; break;
				case 4: pV792->plycW5ChMap[cntPLYC] = nCh; break;
				default: 
					notify_error(1, "Polych WaveLength Cnt error '%d' ", cntWL);
					break;				
			}
			cntWL++;

			if( cntWL >= 5 ) 
			{
				cntPLYC++;
				cntWL = 0;
			}
		}
	}
	fclose(fp); 
#endif
	return WR_OK;
}

/*
void drvV792_init_SP_Table()
{
	int i;
	ST_STD_device *pSTDdev;
	ST_V792 *pV792;
	FILE *fp;
	char buf[64];
	char fileName[128];
	char bufLineScan[64];
	int nCh, cntWL, cntPLYC;
	ST_MASTER *pAdminCfg = get_master();
	
	for(i=0; i<CNT_LOOKUP_TABLE; i++) {
		pV792->SP1[0][i] = pV792->SP2[0][i] = pV792->SP3[0][i] = pV792->SP4[0][i] = 1.0;
	}

	pSTDdev = get_first_STDdev();
	for( i=0; i<pAdminCfg->n16DeviceNum; i++) 
	{
		if( pSTDdev == NULL ) 
		{
			notify_error(1, "No valied Device in 'drvV792_init_SP_Table()' ");
			return;
		}
		pV792 = (ST_V792 *)pSTDdev->pUser;
		
		sprintf(fileName, "/usr/local/epics/siteApp/config/B%d.mapping", i+1 );
		if( (fp = fopen(fileName, "r") ) == NULL )
		{	
			notify_error(1, "Can't read '%s' file", fileName);
			return;
		} 
		cntWL = cntPLYC = 0;
		while(1) 
		{
			if( fgets(bufLineScan, 64, fp) == NULL ) break;
			if( bufLineScan[0] == '#') ;
			else if( bufLineScan[0] == '\n' ) ;
			else if( bufLineScan[0] == '\r' ) ;
			else if( bufLineScan[0] == ' ' ) ;
			else 
			{
				sscanf(bufLineScan, "%s %d", buf, &nCh);

				pV792->plycW1ChMap[cntPLYC] = nCh;
				cntWL++;

				if( cntWL >= 5 ) 
				{
					cntPLYC++;
					cntWL = 0;
				}
			}
		}
		fclose(fp); 

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
*/

void func_V792_DATA_SEND(void *pArg, double arg1, double arg2)
{
//	int i;
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	
	if( pSTDdev->ST_Base.opMode == OPMODE_CALIBRATION )
		ts_put_calib_Iped_value(pSTDdev);
	else if ( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ||
		pSTDdev->ST_Base.opMode == OPMODE_LOCAL ) {
	
		ts_put_raw_value(pSTDdev); 
	}
	else {
		notify_error(1, "%s: Wrong op mode (%d)!\n", pSTDdev->taskName, pSTDdev->ST_Base.opMode );
		return;
	}

	drvV792_read_chanData_clear(pSTDdev);

}

void func_V792_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if( (int)arg1 == 1 ) /* in case of arming  */
	{
		if( admin_check_Arming_condition()  == WR_ERROR) 
			return;

		if( drvV792_get_info_befor_shot() == WR_ERROR )
			return;

/*		flush_STDdev_to_MDSfetch( pSTDdev ); */ /* move to SFW 2013. 6. 19  --> recovery.*/

			if( drvV792_ARM_enable(pSTDdev ) == WR_ERROR ) {
				return;
			}
			printf("\"%s\" armed.\n", pSTDdev->taskName );

			
			scanIoRequest(pSTDdev->ioScanPvt_status);
		
	}
	else /* release arming condition */
	{
//		if( admin_check_Release_condition()  == WR_ERROR) 
//			return;
		
			if( drvV792_ARM_disable( pSTDdev ) == WR_ERROR ) {
				epicsPrintf("\n>>> devAdmin_BO_RUN_START : drvACQ196_ARM_enable... failed\n");
				return ;
			}
			scanIoRequest(pSTDdev->ioScanPvt_status);
			notify_refresh_master_status();

	}

}

void func_V792_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	if( (int)arg1 == 1 ) /* command to run  */
	{
		if( admin_check_Run_condition()  == WR_ERROR) 
			return;

		if( drvV792_RUN_start(pSTDdev ) == WR_ERROR )
			return;
/*
		pSTDdev = get_first_STDdev();
		epicsEventSignal( pSTDdev->epicsEvent_CatchEnd );
		it to be done by LTU event...
*/
		epicsThreadSleep(0.1);
	}
	else 
	{
		if( drvV792_RUN_stop(pSTDdev) == WR_ERROR )
			return;
		
		/* for OPI fast reaction... */
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
	}
}

void func_V792_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_MASTER *pMaster = get_master();
	
	if( pMaster->ST_Base.opMode != OPMODE_INIT )
		pSTDdev->StatusDev |= TASK_STANDBY;

	if( pSTDdev->BoardID != 0) return;
	
	if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ) {
		db_put("TS_LTU_SET_FREE_RUN", "0");
		db_put("TS_LTU_SHOT_END", "1");
		db_put("TS_LTU_P0_ENABLE", "1");
		db_put("TS_LTU_SHOT_END", "0");
	} else {
		db_put("TS_LTU_SET_FREE_RUN", "1");
		db_put("TS_LTU_SHOT_END", "1");
		db_put("TS_LTU_P0_ENABLE", "1");
		db_put("TS_LTU_SHOT_END", "0");
	}

}

void func_V792_TEST_PUT(void *pArg, double arg1, double arg2)
{
	int i;
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	
	for( i=0; i<pV792->chNum; i++)
		printf("%s:%d tag name: %s\n", pSTDdev->taskName, i, pV792->strTagName[i]);
	
}





