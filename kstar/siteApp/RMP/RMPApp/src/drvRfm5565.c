#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvRfm5565.h"



static long drvRFM5565_io_report(int level);
static long drvRFM5565_init_driver();


struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvRfm5565 = {
       2,
       drvRFM5565_io_report,
       drvRFM5565_init_driver
};

epicsExportAddress(drvet, drvRfm5565);




int init_RFM5565_taskConfig( ST_STD_device *pSTDdev)
{
	ST_RFM5565* pRFM5565;
	

	pRFM5565 = (ST_RFM5565 *)malloc(sizeof(ST_RFM5565));
	if( !pRFM5565) return WR_ERROR;

	pSTDdev->pUser = pRFM5565;


	strcpy(pRFM5565->device, pSTDdev->strArg0);
	printf("RFM: device file '%s'\n", pRFM5565->device);

	pRFM5565->Offset = 0x0;

	
	/*********************************************************/

	pSTDdev->ST_Func._Exit_Call = exit_RFM5565_taskConfig;

	
#if 0
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		free(pSTDdev);
		return WR_ERROR;
	}

	pSTDdev->pST_BuffThread = make_STD_RingBuf_thread(pSTDdev);
	if(!pSTDdev->pST_BuffThread) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		free(pSTDdev);
		return WR_ERROR;
	}
#endif
	/* move here to get child device pointer */
	/* must be at this position              */
#if 0
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		free(pSTDdev);
		return WR_ERROR;
	}
#endif

	return WR_OK;
}

void exit_RFM5565_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	ST_RFM5565 *pRFM5565 = (ST_RFM5565 *)pSTDdev->pUser;

	/* Close the Reflective Memory device */
	RFM2gClose( &pRFM5565->Handle );
	printf("  %s: device closed.\n", pSTDdev->taskName);

}

int init_RFM5565_vender( ST_RFM5565 *pRFM5565 )
{
	RFM2G_STATUS   result;				   /* Return codes from RFM2Get API calls  */

	/* Open the Reflective Memory device */
	result = RFM2gOpen( pRFM5565->device, &pRFM5565->Handle );
	if( result != RFM2G_SUCCESS )
	{
		printf( "ERROR: RFM2gOpen() failed.\n" );
		printf( "Error: %s.\n\n",  RFM2gErrorMsg(result));
		return(WR_ERROR);
	}

	 /* Tell the user the NodeId of this device */
	result = RFM2gNodeID( pRFM5565->Handle, &pRFM5565->NodeId );
	if( result != RFM2G_SUCCESS )
	{
		printf( "Could not get the Node ID.\n" );
		printf( "Error: %s.\n\n",  RFM2gErrorMsg(result));
		return(WR_ERROR);
	}
	printf("rfm2g:  NodeID = 0x%02x\n", pRFM5565->NodeId);

	return WR_OK;
}



void threadFunc_user_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_RFM5565 *pRFM5565;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
	pRFM5565 = (ST_RFM5565 *)pSTDdev->pUser;

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId );

		
	}
}

void threadFunc_user_RingBuf(void *param)
{
	

	return;
}

void threadFunc_user_CatchEnd(void *param)
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


static long drvRFM5565_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_RFM5565 *pRFM5565 = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	
	pMaster = get_master();
	if(!pMaster)	return 0;


	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) )
		{
			if(init_RFM5565_taskConfig( pSTDdev ) == WR_ERROR) {
				printf("ERROR!  \"%s\" init_RFM5565_taskConfig() failed.\n", pSTDdev->taskName );
				return 0;
			}
			pRFM5565 = (ST_RFM5565 *)pSTDdev->pUser;


			if(init_RFM5565_vender( pRFM5565 ) == WR_ERROR) {
				printf("ERROR!  \"%s\" init_RFM5565_vender() failed.\n", pSTDdev->taskName );
				return 0;
			}

			 
			pSTDdev->StatusDev |= TASK_STANDBY;	
			pSTDdev->StatusDev |= TASK_ARM_ENABLED;	
			pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;	
			pSTDdev->StatusDev |= TASK_IN_PROGRESS;	
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pRFM5565) { */

//	epicsAtExit((VOIDFUNCPTREXIT) exit_RFM5565_taskConfig, NULL);

	notify_refresh_master_status();

	
	printf("******* RFM5565 local device init OK! (%p)\n", pRFM5565);

	return 0;
}


static long drvRFM5565_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_RFM5565 *pRFM5565;
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
		pRFM5565 = (ST_RFM5565*)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pRFM5565->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pRFM5565->adjCount_smplRate,
					                                                 pRFM5565->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pRFM5565->adjCount_Gain,
					                                                   pRFM5565->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pRFM5565->pchannelConfig)>0) print_channelConfig(pRFM5565,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}




void _bin_show(unsigned char data)
{
	unsigned char	bit;

	for (bit = 0x01; bit; bit *= 2)
	{
		if (bit & data)
			printf("1");
		else
			printf("0");
	}
}


