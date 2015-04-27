#include "sfwCommon.h"
/*#include "sfwMDSplus.h"*/


#include "drvTSSC.h"
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h> /* open() */



static long drvTSSC_io_report(int level);
static long drvTSSC_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvTSSC = {
       2,
       drvTSSC_io_report,
       drvTSSC_init_driver
};

epicsExportAddress(drvet, drvTSSC);



int create_TSSC_taskConfig( ST_STD_device *pSTDdev)
{
	ST_TSSC* pTSSC;

	pTSSC = (ST_TSSC *)malloc(sizeof(ST_TSSC));
	if( !pTSSC) return WR_ERROR;
	pSTDdev->pUser = pTSSC;

	pTSSC->fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (pTSSC->fd  < 0)
	{
		fprintf(stdout, "Device open error !!!!!!\n");
		return WR_ERROR;
	}
/*	pST_TSSC->pBaseAddr = (ST_REG_MAP *)malloc(MAP_SIZE ); */
	pTSSC->pBaseAddr  = mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pTSSC->fd, C_BASEADDR & ~MAP_MASK);
//	pTSSC->pBaseAddr  = mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pTSSC->fd, C_BASEADDR);
	if ( pTSSC->pBaseAddr == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pBaseAddr]\n");
		return WR_ERROR;
	}


	
	printf("REG_START_STOP_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpStartStop);
	printf("REG_LASER_CLK_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpLaserClkHz);
	printf("REG_LASER_WIDTH_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpLaserClkWidth);
	printf("REG_QSWITCH_DELAY_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpQSwitchDely);
	printf("REG_QSWITCH_WIDTH_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpQSwitchWidth);
	printf("REG_BACK_PULSE_DELAY_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpBackPulseDelay);
	printf("REG_BACK_PULSE_CNT_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpBackPulseCnt);
	printf("REG_BACK_PULSE_WIDTH_OFFSET:	  0x%X\n", pTSSC->pBaseAddr->gpBackPulseWidth);
	printf("REG_BTW_LTU_DG535_IN:	  0x%X\n", pTSSC->pBaseAddr->gpBtwLTU_dg535_input);
	
	printf("REG_INPUT_PULSE_CNT:	  0x%X\n", pTSSC->pBaseAddr->gpRealPulseCnt);
	printf("REG_LASER_OUT_PULSE_CNT:	  0x%X\n", pTSSC->pBaseAddr->gpLaserOutCnt);
	printf("REG_QSWT_OUT_PULSE_CNT:	  0x%X\n", pTSSC->pBaseAddr->gpQswtOutCnt);

	printf("REG_GPIO_IN:	  0x%X\n", pTSSC->pBaseAddr->gpGPIO_IN);
	printf("REG_GPIO_OUT:	  0x%X\n", pTSSC->pBaseAddr->gpGPIO_OUT);
	printf("REG_GPIO_DIR:	  0x%X\n", pTSSC->pBaseAddr->gpGPIO_Direction);

 

	return WR_OK;
}


int init_my_sfwFunc_TSSC( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_TSSC_OP_MODE;
	pSTDdev->ST_Func._TEST_PUT = func_TSSC_TEST_PUT;
	pSTDdev->ST_Func._SYS_ARMING= func_TSSC_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_TSSC_SYS_RUN;
	pSTDdev->ST_Func._DATA_SEND = func_TSSC_DATA_SEND;

	pSTDdev->ST_Func._SYS_RESET = func_TSSC_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_TSSC_taskConfig;


	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_TSSC_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
		


	return WR_OK;

}


void clear_TSSC_taskConfig(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_TSSC *pTSSC = (ST_TSSC *)pSTDdev->pUser;
	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 

	munmap(pTSSC->pBaseAddr, MAP_SIZE);
	
	close( pTSSC->fd );

}

void threadFunc_TSSC_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_TSSC *pTSSC;
	ST_MASTER *pMaster = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
	pTSSC = (ST_TSSC *)pSTDdev->pUser;

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		notify_refresh_master_status();

#if 0
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif

/*		if( pTSSC->u8_ip_fault != 1 ) 
		{ */
			db_put("TSSC_SYS_RUN", "0");
			epicsThreadSleep(1.0);
			db_put("TSSC_SYS_ARMING", "0");
/*		} */

		if( pMaster->cUseAutoSave ) 
			db_put("TSSC_SEND_DATA", "1");
		
	}
}


static long drvTSSC_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_TSSC *pTSSC = NULL;
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

		if(create_TSSC_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_TSSC_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_TSSC(pSTDdev);

	
		pTSSC = (ST_TSSC *)pSTDdev->pUser;


		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pTSSC) { */

	notify_refresh_master_status();

	printf("****** TSSC local device init OK!\n");

	return 0;
}


static long drvTSSC_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_TSSC *pTSSC;
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
		pTSSC = (ST_TSSC *)pSTDdev->pUser;
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
			epicsPrintf("   callback time: %fusec\n", pTSSC->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pTSSC->adjCount_smplRate,
					                                                 pTSSC->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pTSSC->adjCount_Gain,
					                                                   pTSSC->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pTSSC->pchannelConfig)>0) print_channelConfig(pTSSC,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_TSSC_OP_MODE(void *pArg, double arg1, double arg2)
{
/*	ST_STD_device* pSTDdev = (ST_STD_device *)pArg; */

	if( (int)arg1 == OPMODE_REMOTE ) {

		
	}
	else if ((int)arg1 == OPMODE_LOCAL ) {

		
	}
}

void func_TSSC_TEST_PUT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_TSSC *pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = (unsigned int)arg1;

	printf("call func_TSSC_TEST_PUT with %s, 0x%X\n", pSTDdev->taskName, value);

	pTSSC->pBaseAddr->gpStartStop =  SWAP32(value);

}

void func_TSSC_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_TSSC *pTSSC = (ST_TSSC *)pSTDdev->pUser;
//	char strBuf[16];

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		db_put("TSSC_DOUT0:OUT_0", "1");
		epicsThreadSleep(1.0);
		db_put("TSSC_DOUT0:OUT_0", "0");

		
		flush_STDdev_to_MDSfetch( pSTDdev );
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;

		db_put("TSSC_DOUT0:OUT_0", "1");
		epicsThreadSleep(1.0);
		db_put("TSSC_DOUT0:OUT_0", "0");
		

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}
}

void func_TSSC_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_TSSC *pTSSC = (ST_TSSC *)pSTDdev->pUser;

	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;
		
		

		db_put("TSSC_DOUT1:OUT_1", "1");
		epicsThreadSleep(1.0);
		db_put("TSSC_DOUT1:OUT_1", "0");

		pSTDdev->StatusDev |= TASK_IN_PROGRESS;
	}
	else {
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;
		
		
		db_put("TSSC_DOUT1:OUT_1", "1");
		epicsThreadSleep(1.0);
		db_put("TSSC_DOUT1:OUT_1", "0");

		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
	}

	
}


void func_TSSC_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
	notify_refresh_master_status();

//	ersc_put_raw_value(pSTDdev);


	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	notify_refresh_master_status();

}

void func_TSSC_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_TSSC *pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = pTSSC->pBaseAddr->gpStartStop;

/*	call your device reset functions */

	if( (int)arg1 ) {
		pTSSC->pBaseAddr->gpStartStop = SWAP32( value | 0x1);
		printf("Board reset!\n");
	}
	else {
		pTSSC->pBaseAddr->gpStartStop =  SWAP32(value & 0xfffffffe);
		printf("Reset clear!\n");
	}
}






