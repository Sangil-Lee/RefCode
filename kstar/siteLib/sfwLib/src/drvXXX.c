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



#include "drvXXX.h"
/*#include "myMDSplus.h" */


static long drvXXX_io_report(int level);
static long drvXXX_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvXXX = {
       2,
       drvXXX_io_report,
       drvXXX_init_driver
};

epicsExportAddress(drvet, drvXXX);



int create_XXX_taskConfig( ST_STD_device *pSTDdev)
{
	ST_XXX* pXXX;

	pXXX = (ST_XXX *)malloc(sizeof(ST_XXX));
	if( !pXXX) return WR_ERROR;
	pSTDdev->pUser = pXXX;

	pXXX->fd = open("/dev/your_fd", O_RDWR|O_SYNC);
	if (pXXX->fd  < 0)
	{
		fprintf(stdout, "Device open error !!!!!!\n");
		return WR_ERROR;
	}


	return WR_OK;
}


int init_my_sfwFunc_XXX( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_XXX_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_XXX_BOARD_SETUP;
	pSTDdev->ST_Func._SAMPLING_RATE= func_XXX_SAMPLING_RATE;
	pSTDdev->ST_Func._TEST_PUT = func_XXX_TEST_PUT;
	pSTDdev->ST_Func._SYS_ARMING= func_XXX_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_XXX_SYS_RUN;	
	pSTDdev->ST_Func._DATA_SEND = func_XXX_DATA_SEND;

	pSTDdev->ST_Func._SYS_RESET = func_XXX_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_XXX_taskConfig;


	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_XXX_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	
#if 1
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_XXX_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);
	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
#endif	
		


	return WR_OK;

}


void clear_XXX_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_XXX *pXXX = (ST_XXX *)pSTDdev->pUser;
	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 


	close( pXXX->fd );

}

void threadFunc_XXX_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_XXX *pXXX;
	ST_MASTER *pMaster = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
	pXXX = (ST_XXX *)pSTDdev->pUser;

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		notify_refresh_master_status();

#if 0
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif

		

		db_put("XXX_SYS_RUN", "0");
		epicsThreadSleep(1.0);
		db_put("XXX_SYS_ARMING", "0");


		if( pMaster->cUseAutoSave ) 
			db_put("XXX_SEND_DATA", "1");
		
	}
}

void threadFunc_XXX_RingBuf(void *param)
{
	int i, ellCnt;
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_User_Buf_node queueData;
	ST_buf_node *pbufferNode;	
	ST_XXX *pXXX = (ST_XXX *)pSTDdev->pUser;


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
	
	while(TRUE) 
	{
		epicsMessageQueueReceive(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buf_node));

		pbufferNode = queueData.pNode;

		/* TODO */

		
		
		ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
		ellCnt = ellCount(pSTDdev->pList_BufferNode);

	} /* while(TRUE)  */

	return;
}


static long drvXXX_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_XXX *pXXX = NULL;
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

		if(create_XXX_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_XXX_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_XXX(pSTDdev);
		
	
		pXXX = (ST_XXX *)pSTDdev->pUser;


		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pXXX) { */

	notify_refresh_master_status();

	printf("****** XXX local device init OK!\n");

	return 0;
}


static long drvXXX_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_XXX *pXXX;
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
		pXXX = (ST_XXX *)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pXXX->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pXXX->adjCount_smplRate,
					                                                 pXXX->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pXXX->adjCount_Gain,
					                                                   pXXX->adjTime_Gain_Usec);
*/
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_XXX_OP_MODE(void *pArg, double arg1, double arg2)
{
	if( (int)arg1 == OPMODE_REMOTE ) {
		/* to do here */

	}
	else if ((int)arg1 == OPMODE_LOCAL ) {
		/* to do here */

	}
}

void func_XXX_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_XXX_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);


}

void func_XXX_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_XXX *pXXX = (ST_XXX *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;
/* to do here */

		

		flush_STDdev_to_MDSfetch( pSTDdev );
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;	
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;


		

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}
}

void func_XXX_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_XXX *pXXX = (ST_XXX *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		/* to do here */
		epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId );



		pSTDdev->StatusDev |= TASK_IN_PROGRESS;
	}
	else 
	{
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;



		
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
	}

	
}

void func_XXX_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_XXX *pXXX = (ST_XXX *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: sampling rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}
void func_XXX_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your MDS related functions */



	pSTDdev->StatusDev |= TASK_STANDBY;
	notify_refresh_master_status();
}

void func_XXX_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	call your device reset functions */




}



