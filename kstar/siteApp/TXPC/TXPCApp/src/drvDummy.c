#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvDummy.h"
#include <math.h>
#include <errno.h>




static long drvDummy_io_report(int level);
static long drvDummy_init_driver();

//static void exit_Dummy_Task(void);


struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvDummy = {
       2,
       drvDummy_io_report,
       drvDummy_init_driver
};

epicsExportAddress(drvet, drvDummy);



int create_Dummy_taskConfig( ST_STD_device *pSTDdev)
{
	ST_Dummy* pDummy;
	uint8 qty;
	int i;
//	uint32 status;


	pDummy = (ST_Dummy *)malloc(sizeof(ST_Dummy));
	if( !pDummy) return WR_ERROR;

	pSTDdev->pUser = pDummy;

	return WR_OK;
}

int init_my_SFW_Dummy( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._BOARD_SETUP = func_Dummy_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_Dummy_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_Dummy_SYS_RUN;

	pSTDdev->ST_Func._Exit_Call = clear_Dummy_taskConfig;


	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_Dummy_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		free(pSTDdev);
		return WR_ERROR;
	}


	return WR_OK;

}


void clear_Dummy_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
/*	ST_Dummy *pDummy = (ST_Dummy *)pSTDdev->pUser;



	if( pDummy ) free(pDummy);
*/	
}



static long drvDummy_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_Dummy *pDummy = NULL;
	ST_STD_device *pSTDdev = NULL;
//	uint32 val = TRUE;
//	int errs;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) != 0 ) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if(create_Dummy_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_Dummy_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_SFW_Dummy(pSTDdev);
	
		pDummy = (ST_Dummy *)pSTDdev->pUser;


		pSTDdev->StatusDev |= TASK_STANDBY;
//		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pDummy) { */

	notify_refresh_master_status();


//	epicsAtExit((VOIDFUNCPTREXIT) exit_Dummy_Task, NULL);
	

	printf("****** Dummy local device init OK!\n");

	return 0;
}


static long drvDummy_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_Dummy *pDummy;
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
		pDummy = (ST_Dummy *)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pDummy->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pDummy->adjCount_smplRate,
					                                                 pDummy->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pDummy->adjCount_Gain,
					                                                   pDummy->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pDummy->pchannelConfig)>0) print_channelConfig(pDummy,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}


void threadFunc_Dummy_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_Dummy *pDummy;
	ST_MASTER *pMaster = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
	pDummy = (ST_Dummy *)pSTDdev->pUser;

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		printf(" got DAQ event ......\n");
		/* TODO here */

		

	}

}


void func_Dummy_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_Dummy_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

	
}

void func_Dummy_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_Dummy *pDummy = (ST_Dummy *)pSTDdev->pUser;
	char lineBuf1[256]="empty\n";
	char lineBuf2[256]="empty\n";
	char lineBuf3[256]="empty\n";
	char tempBuf[256]="empty\n";
	int id = 0;

	FILE *fp;
	char *command = "ps -ef | grep gedit";

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		printf("Dummy got Sequence start event.\n");
		printf("Shot number is : %lu\n", pSTDdev->ST_Base.shotNumber );

		fp = popen(command, "r");
		if (fp == NULL)
		{
			fprintf(stderr,"Cannot execute %s\n", command);
			pclose(fp);
		}
		while (fgets(tempBuf, 256, fp) != NULL)
		{
			if( id == 0)
				strcpy(lineBuf1, tempBuf);
			else if( id == 1)
				strcpy(lineBuf2, tempBuf);
			else 
				strcpy(lineBuf3, tempBuf);
			id++;
		}
		pclose(fp);

		if( id == 2) {
			printlog("Execute target program. shot: %lu\n", pSTDdev->ST_Base.shotNumber);

		} else {
			printlog("still live. id:%d, shot: %lu\n", id, pSTDdev->ST_Base.shotNumber);
			printlog("-> %s", lineBuf1);
                	printlog("-> %s", lineBuf2);
			printlog("-> %s", lineBuf3);
		}

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

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}


}
void func_Dummy_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_Dummy *pDummy = (ST_Dummy *)pSTDdev->pUser;


	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;
		
		
	}
	else {
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;
		/* nothing to do */

		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER; 
	}

	
}


