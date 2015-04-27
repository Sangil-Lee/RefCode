#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvDio24.h"
#include <math.h>
#include <errno.h>




static long drvDio24_io_report(int level);
static long drvDio24_init_driver();

//static void exit_Dio24_Task(void);


struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvDio24 = {
       2,
       drvDio24_io_report,
       drvDio24_init_driver
};

epicsExportAddress(drvet, drvDio24);


static	int		_def_board_dio24		= -1;


int create_Dio24_taskConfig( ST_STD_device *pSTDdev)
{
	ST_Dio24* pDio24;
	uint8 qty;
	int i;
//	uint32 status;


	pDio24 = (ST_Dio24 *)malloc(sizeof(ST_Dio24));
	if( !pDio24) return WR_ERROR;

	pSTDdev->pUser = pDio24;

	if( dio24_board_count(&qty) == GSC_SUCCESS ) {
		printf("%s: dio24_board_count: %d\n", pSTDdev->taskName, qty);
		if (qty == 1)
			_def_board_dio24 = 0;
	} 
	else {
		printf("%s: There is no dio24 board\n", pSTDdev->taskName);
		free(pDio24);
		return WR_ERROR;
	}

	if( dio24_open(_def_board_dio24, &pDio24->dev) != GSC_SUCCESS ) {
		printf("%s: can't open dio24 board.\n", pSTDdev->taskName);
		free(pDio24);
		return WR_ERROR;
	}

	if( dio24_init(pDio24->dev) != GSC_SUCCESS ) {
		printf("%s: dio24_init() failed.\n", pSTDdev->taskName);
		dio24_close(pDio24->dev);
		free(pDio24);
		return WR_ERROR;
	}


	pDio24->portA_io = DIO24_GPIO_DIRECTION_INPUT; /* 0 */
	pDio24->portB_io = DIO24_GPIO_DIRECTION_INPUT;
	pDio24->portC_io = DIO24_GPIO_DIRECTION_INPUT;
	for(i=0; i<8; i++)
		pDio24->portCX_io[i] = DIO24_GPIO_DIRECTION_INPUT;


	return WR_OK;
}

int init_my_SFW_Dio24( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._BOARD_SETUP = func_Dio24_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_Dio24_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_Dio24_SYS_RUN;

	pSTDdev->ST_Func._Exit_Call = clear_Dio24_taskConfig;


	return WR_OK;

}


void clear_Dio24_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;

	uint32 status;

	if( dio24_init(pDio24->dev) != GSC_SUCCESS )
		printf("%s: dio24_init() failed.\n", pSTDdev->taskName);
	
	status	= dio24_close(pDio24->dev);
	if( status == GSC_SUCCESS) printf("%s: dio24_close()... OK.\n", pSTDdev->taskName );
	else 
		printf("%s: dio24_close()... failed.\n", pSTDdev->taskName );
/*	int		errs	= (status == GSC_SUCCESS) ? 0 : 1; */


/*	if( pDio24 ) free(pDio24); */
	
}



static long drvDio24_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_Dio24 *pDio24 = NULL;
	ST_STD_device *pSTDdev = NULL;
//	uint32 val = TRUE;
//	int errs;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( strcmp(pSTDdev->devType, STR_DEVICE_TYPE_3) != 0 ) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if(create_Dio24_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_Dio24_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_SFW_Dio24(pSTDdev);
	
		pDio24 = (ST_Dio24 *)pSTDdev->pUser;



		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pDio24) { */

	notify_refresh_master_status();


//	epicsAtExit((VOIDFUNCPTREXIT) exit_Dio24_Task, NULL);
	

	printf("****** Dio24 local device init OK!\n");

	return 0;
}


static long drvDio24_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_Dio24 *pDio24;
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
		pDio24 = (ST_Dio24 *)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pDio24->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pDio24->adjCount_smplRate,
					                                                 pDio24->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pDio24->adjCount_Gain,
					                                                   pDio24->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pDio24->pchannelConfig)>0) print_channelConfig(pDio24,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

void func_Dio24_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	printf("call func_Dio24_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2);

	
}

void func_Dio24_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		

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
void func_Dio24_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;


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


