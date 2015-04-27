/****************************************************************************

Module      : sfwAdminHead.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2010-1-24

2010. 6. 8  divide  ioScanPvt  to status and user call.    by woong.


*****************************************************************************/

#include "sfwAdminHead.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"
#include "sfwDBSeq.h"


/**********************************
user include files 
***********************************/
#include "drvNI6123.h"
#include "drvNI6259.h"
#include "drvIT6322.h"
#include <NIDAQmx.h>



/**********************************
user include files
***********************************/




static ST_ADMIN          *pST_AdminCfg = NULL;


static ST_threadCfg*  make_Admin_controlThreadConfig();
static void threadFunc_Admin_control(void *param);
static void destroy_AdminTask(void);

static int make_STD_thread_CatchEnd(ST_STD_device *pSTDdev);
static int make_STD_thread_DAQ(ST_STD_device *pSTDdev);
static ST_threadCfg*  make_STD_thread_Control(ST_STD_device *pSTDdev);
#if USE_LOCAL_DATA_POOL
static ST_threadCfg* make_STD_thread_RingBuff(ST_STD_device *pSTDdev);
#endif



static void threadFunc_user_CatchEnd(void *param);
static void threadFunc_STD_control(void *param);


static void createAdminTaskCallFunction(const iocshArgBuf *args);
static void createDeviceCallFunction(const iocshArgBuf *args);
static void createChannelCallFunction(const iocshArgBuf *args);
static void setMDSplusCallFunction(const iocshArgBuf *args);


static const iocshArg createAdminTaskArg0 = {"task_name", iocshArgString};
static const iocshArg* const createAdminTaskArgs[] = { &createAdminTaskArg0 };
static const iocshFuncDef createAdminTaskFuncDef = { "createAdminTask", 1, createAdminTaskArgs };


static const iocshArg createDeviceArg0 = {"device_name", iocshArgString};
static const iocshArg createDeviceArg1 = {"dev type", iocshArgString};
static const iocshArg createDeviceArg2 = {"physical_channel", iocshArgString};
static const iocshArg createDeviceArg3 = {"trigger_source", iocshArgString};
static const iocshArg createDeviceArg4 = {"local_file_name", iocshArgString};
static const iocshArg* const createDeviceArgs[] = { &createDeviceArg0 , &createDeviceArg1, &createDeviceArg2, &createDeviceArg3, &createDeviceArg4};
static const iocshFuncDef createDeviceFuncDef = { "createDevice", 5, createDeviceArgs };


static const iocshArg createChannelArg0 = {"device_name", iocshArgString};
static const iocshArg createChannelArg1 = {"ch name", iocshArgString};
static const iocshArg createChannelArg2 = {"ch num", iocshArgString};
static const iocshArg* const createChannelArgs[] = { &createChannelArg0, &createChannelArg1, &createChannelArg2 };
static const iocshFuncDef createChannelFuncDef = {"createChannel", 3, createChannelArgs};

static const iocshArg setMDSplusArg0 = {"device_name", iocshArgString};
static const iocshArg setMDSplusArg1 = {"filter", iocshArgString};
static const iocshArg setMDSplusArg2 = {"value", iocshArgString};
static const iocshArg* const setMDSplusArgs[] = { &setMDSplusArg0, &setMDSplusArg1, &setMDSplusArg2 };
static const iocshFuncDef setMDSplusFuncDef = {"setMDSplus", 3, setMDSplusArgs};



static void AdminTaskRegister(void)
{
	iocshRegister(&createAdminTaskFuncDef, createAdminTaskCallFunction);
	iocshRegister(&createDeviceFuncDef, createDeviceCallFunction);
	iocshRegister(&createChannelFuncDef, createChannelCallFunction);
	iocshRegister(&setMDSplusFuncDef, setMDSplusCallFunction);
}
epicsExportRegistrar(AdminTaskRegister);






static long drvAdmin_io_report(int level);
static long drvAdmin_init_driver();
struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvAdmin = {
       2,
       drvAdmin_io_report,
       drvAdmin_init_driver
};
epicsExportAddress(drvet, drvAdmin);




int create_AdminConfig()
{
	if(pST_AdminCfg) return 0;
	
	pST_AdminCfg = (ST_ADMIN*) malloc(sizeof(ST_ADMIN));
	if(!pST_AdminCfg) return 1;

	pST_AdminCfg->pList_DeviceTask= (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pST_AdminCfg->pList_DeviceTask) return 1;
	else ellInit(pST_AdminCfg->pList_DeviceTask);

	return 0;
}

static void createAdminTaskCallFunction(const iocshArgBuf * args)
{
	char task_name[SIZE_TASK_NAME];

	if(args[0].sval) strcpy(task_name, args[0].sval);

	if(create_AdminConfig()) return;

	epicsThreadSleep(.5);

	strcpy(pST_AdminCfg->taskName, task_name);

	pST_AdminCfg->lock_mds = epicsMutexCreate();
	scanIoInit(&pST_AdminCfg->ioScanPvt_status);
	pST_AdminCfg->StatusAdmin = TASK_NOT_INIT;
	pST_AdminCfg->StatusAdmin |= TASK_SYSTEM_IDLE;
	pST_AdminCfg->ErrorAdmin = ERROR_NONE;
	strcpy(pST_AdminCfg->ErrorString, "Clean");

	/*Common base variables *********************************/
	pST_AdminCfg->ST_Base.opMode = OPMODE_INIT;
	pST_AdminCfg->ST_Base.shotNumber = 1;
	pST_AdminCfg->ST_Base.fBlipTime = INIT_BLIP_TIME;
	pST_AdminCfg->ST_Base.nSamplingRate = INIT_SAMPLING;
	pST_AdminCfg->ST_Base.dT0[0] = 1.0;
	pST_AdminCfg->ST_Base.dT1[0] = 10.0;
	/*Common base variables *********************************/
	
	pST_AdminCfg->cUseAutoSave = USE_AUTO_SAVE;
	pST_AdminCfg->n16DeviceNum = 0;
	pST_AdminCfg->n8MdsPutFlag = 0; /* must start with "0" */
	pST_AdminCfg->n8EscapeWhile = 1;

	pST_AdminCfg->pST_adminCtrlThread = make_Admin_controlThreadConfig(pST_AdminCfg);
	if(!pST_AdminCfg->pST_adminCtrlThread) return;

	epicsPrintf("make Admin control thread ok!: %p\n", pST_AdminCfg->pST_adminCtrlThread);

	return;
}

static void createDeviceCallFunction(const iocshArgBuf *args)
{
	static unsigned short boardcnt = 0;
	ST_STD_device* pSTDdev = NULL;
	char task_name[SIZE_TASK_NAME];
	char dev_type[SIZE_TASK_NAME];
	char physical_channel[32];
	char trigger_source[32];
	char local_file_name[32];

	if(args[0].sval) strcpy(dev_type, args[0].sval);
	if(args[1].sval) strcpy(task_name, args[1].sval);
	if(args[2].sval) strcpy(physical_channel, args[2].sval);
	if(args[3].sval) strcpy(trigger_source, args[3].sval);
	if(args[4].sval) strcpy(local_file_name, args[4].sval);

	
	while (!pST_AdminCfg) {
		printf("ERROR!  there is no admin task.\n");
		return;
	}

	pSTDdev = (ST_STD_device*) malloc(sizeof(ST_STD_device));
	if(!pSTDdev) {
		printf("ERROR! \"%s\" malloc() failed!\n", task_name);
		return;
	}
	strcpy(pSTDdev->taskName, task_name);
//	pSTDdev->BoardID = boardcnt; //SXR system is not use this
//	boardcnt++;
//	pST_AdminCfg->n16DeviceNum = boardcnt;
	strcpy(pSTDdev->devType, dev_type);
	pSTDdev->pST_parentAdmin = pST_AdminCfg;
	scanIoInit(&pSTDdev->ioScanPvt_status);
	scanIoInit(&pSTDdev->ioScanPvt_userCall);


	STDdev_status_reset(pSTDdev);

	flush_StBase_to_STDdev( pSTDdev );

	flush_STDdev_to_MDSfetch(pSTDdev);


	if( mds_assign_from_Env(pSTDdev) == WR_ERROR ) {
		free(pSTDdev);
		return;
	}

	pSTDdev->pST_stdCtrlThread = make_STD_thread_Control(pSTDdev);
	if(!pSTDdev->pST_stdCtrlThread) {		
		printf("ERROR! %s can't create \"pST_stdCtrlThread\" \n", pSTDdev->taskName );
		free(pSTDdev);
		return;
	}

	pSTDdev->pList_Channel = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pSTDdev->pList_Channel) {
		printf("ERROR! %s can't create \"pList_Channel\" \n", pSTDdev->taskName );
		free(pSTDdev);
		free(pSTDdev->pST_stdCtrlThread);
		return;
	}
	else  ellInit(pSTDdev->pList_Channel);
	

	/*************************************************
	user edit , must be here!.
	**************************************************/
	if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
	{
		pSTDdev->pUser = create_NI6123_taskConfig(physical_channel, trigger_source, local_file_name);
		if(!pSTDdev->pUser) {
			printf("ERROR!  \"%s\" create_NI6123_taskConfig() failed.\n", pSTDdev->taskName );
			free(pSTDdev);
			free(pSTDdev->pST_stdCtrlThread);
			return;
		}
		boardcnt++;
		pST_AdminCfg->n16DeviceNum = boardcnt;

		if( make_STD_thread_DAQ(pSTDdev) == WR_ERROR ) {
			printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
			free(pSTDdev);
			free(pSTDdev->pST_stdCtrlThread);
			return;
		}

	}

	else if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) )
	{
		pSTDdev->pUser = create_NI6259_taskConfig(physical_channel, trigger_source, local_file_name);
		if(!pSTDdev->pUser) {
			printf("ERROR!  \"%s\" create_NI6259_taskConfig() failed.\n", pSTDdev->taskName );
			free(pSTDdev);
			free(pSTDdev->pST_stdCtrlThread);
			return;
		}

		if( make_STD_thread_DAQ(pSTDdev) == WR_ERROR ) {
			printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
			free(pSTDdev);
			free(pSTDdev->pST_stdCtrlThread);
			return;
		}

	}

	else if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_3) )
	{
		pSTDdev->pUser = create_IT6322_taskConfig(physical_channel, trigger_source, local_file_name);
		if(!pSTDdev->pUser) {
			printf("ERROR!  \"%s\" create_IT6322_taskConfig() failed.\n", pSTDdev->taskName );
			free(pSTDdev);
			free(pSTDdev->pST_stdCtrlThread);
			return;
		}

		if( make_STD_thread_DAQ(pSTDdev) == WR_ERROR ) {
			printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
			free(pSTDdev);
			free(pSTDdev->pST_stdCtrlThread);
			return;
		}

	}
	else {
		printf("ERROR! Device type \"%s\" not available!\n", pSTDdev->devType );
		free(pSTDdev);
		free(pSTDdev->pST_stdCtrlThread);
		return;
	}
		notify_refresh_admin_status();
	/*********************************************************
	end of user edit , it should precede any user data pool.
	**********************************************************/

#if USE_LOCAL_DATA_POOL
	pSTDdev->pST_BuffThread = make_STD_thread_RingBuff(pSTDdev);
	if(!pSTDdev->pST_BuffThread) {
		printf("ERROR! %s can't create \"pST_BuffThread\" \n", pSTDdev->taskName );
		free(pSTDdev);
		free(pSTDdev->pST_stdCtrlThread);		
		return;
	}
#endif

#if USE_THREAD_CATCHEND
	/* move here to get child device pointer */
	/* must be at this position              */
	if( make_STD_thread_CatchEnd(pSTDdev) == WR_ERROR ) { 
		free(pSTDdev);
		return;
	}
#endif
	ellAdd(pST_AdminCfg->pList_DeviceTask, &pSTDdev->node);

	return;
}

static void createChannelCallFunction(const iocshArgBuf *args)
{
	ST_STD_device *pSTDdev = NULL;
	ST_STD_channel *pSTDch = NULL;
	char device_name[SIZE_TASK_NAME];
	char channel_name[SIZE_TASK_NAME];
	unsigned short channel_ID=0;

	if(args[0].sval) strcpy(device_name, args[0].sval);
	if(args[1].sval) strcpy(channel_name, args[1].sval);
	if(args[2].sval) channel_ID = strtoul(args[2].sval,NULL,0);

	pSTDdev = get_STDev_from_name(device_name);
	if(!pSTDdev) {
		printf("ERROR! can't find device task \"%s\" \n", device_name );
		return;
	}

	pSTDch = (ST_STD_channel*) malloc(sizeof(ST_STD_channel));
	if(!pSTDch) {
		printf("ERROR! %s's channel malloc() failed \n", pSTDdev->taskName );
		return;
	}

	pSTDch->StatusCh = 0x0;
	strcpy(pSTDch->chName, channel_name);
	pSTDch->ChannelID = channel_ID;

	pSTDch->f32Gain= 1.0;
	pSTDch->f32Offset = 0.0;
	strcpy(pSTDch->strTagName, "NULL");
	
	pSTDch->pST_parentSTDdev = pSTDdev;


	/*************************************************
	user edit 
	**************************************************/
/*	if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
	{
		pSTDch->pUser = create_ni5412_taskConfig(arg2);
		if(!pSTDch->pUser) {
			printf("ERROR!	\"%s\" create_ni5412_taskConfig() failed.\n", pSTDdev->taskName );
			free(pSTDch);
			return;
		}
			
	} 
	else {
		printf("ERROR! Device type \"%s\" not available!\n", pSTDdev->devType );
		free(pSTDch);
		return;
	} */
	/*************************************************
	user edit 
	**************************************************/


	ellAdd(pSTDdev->pList_Channel, &pSTDch->node);

	
	return;
}

static void setMDSplusCallFunction(const iocshArgBuf *args)
{
	ST_STD_device* pSTDdev = NULL;
	char task_name[SIZE_TASK_NAME];
	char filter[SIZE_TASK_NAME];
	char arg2[32];

	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(filter, args[1].sval);
	if(args[2].sval) strcpy(arg2, args[2].sval);

	pSTDdev = get_STDev_from_name( task_name );
	if( pSTDdev == NULL ) {
		printf("ERROR! can't find \"%s\" device!\n", task_name );
		return;
	}
	if( callFunc_set_MDSplus(pSTDdev, filter, arg2) == WR_ERROR ) {
		return;
	}
	return;
}

static ST_threadCfg*  make_Admin_controlThreadConfig()
{
	ST_threadCfg *pST_adminCtrlThread  = NULL;

	pST_adminCtrlThread  = (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pST_adminCtrlThread) return pST_adminCtrlThread; 

	sprintf(pST_adminCtrlThread->threadName, "%s_ctrl", pST_AdminCfg->taskName); 

	pST_adminCtrlThread->threadPriority = epicsThreadPriorityLow;
	pST_adminCtrlThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);  /* epicsThreadStackMedium   */
	pST_adminCtrlThread->threadFunc      = (EPICSTHREADFUNC) threadFunc_Admin_control;
	pST_adminCtrlThread->threadParam     = (void*) pST_AdminCfg;

	pST_adminCtrlThread->threadQueueId   = epicsMessageQueueCreate(1000,sizeof(ST_threadQueueData));
                                                /* Increase queue size to avoid buffer overflow, KKH May 14, 2008 */

	epicsThreadCreate(pST_adminCtrlThread->threadName,
			  pST_adminCtrlThread->threadPriority,
			  pST_adminCtrlThread->threadStackSize,
			  (EPICSTHREADFUNC) pST_adminCtrlThread->threadFunc,
			  (void*) pST_adminCtrlThread->threadParam);

	return pST_adminCtrlThread;
}

static void threadFunc_Admin_control(void *param)
{
	ST_ADMIN *pAdminCfg = (ST_ADMIN*) param;
	ST_threadCfg *pST_controlThread;
	ST_threadQueueData		queueData;

	while (!pAdminCfg->pST_adminCtrlThread)  epicsThreadSleep(.1);

	pST_controlThread = (ST_threadCfg*) pAdminCfg->pST_adminCtrlThread;
	while(TRUE) 
	{
		EXECFUNCQUEUE            pFunc;
		ST_execParam                *pST_execParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		epicsMessageQueueReceive(pST_controlThread->threadQueueId,
				         (void*) &queueData,
					 sizeof(ST_threadQueueData));

		pFunc      = queueData.pFunc;
		pST_execParam = &queueData.param;	
		precord    = (struct dbCommon*) pST_execParam->precord;
		prset      = (struct rset*) precord->rset;

		if(!pFunc) continue;
		else pFunc(pST_execParam);

		if(!precord) continue;

		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}

	return;
}
static void threadFunc_STD_control(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_threadCfg *pST_ctrlThreadCfg;
	ST_threadQueueData		queueData;

	while (!pSTDdev->pST_stdCtrlThread)  epicsThreadSleep(.1);

	pST_ctrlThreadCfg = (ST_threadCfg*) pSTDdev->pST_stdCtrlThread;
	while(TRUE) 
	{
		EXECFUNCQUEUE            pFunc;
		ST_execParam                *pST_execParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		epicsMessageQueueReceive(pST_ctrlThreadCfg->threadQueueId,
				         (void*) &queueData,
					 sizeof(ST_threadQueueData));

		pFunc      = queueData.pFunc;
		pST_execParam = &queueData.param;	
		precord    = (struct dbCommon*) pST_execParam->precord;
		prset      = (struct rset*) precord->rset;

		if(!pFunc) continue;
		else pFunc(pST_execParam);

		if(!precord) continue;

		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}

	return;
}




static long drvAdmin_init_driver(void)
{
	unsigned long long int a;
	ST_STD_device *pSTDdev = NULL;

	if(!pST_AdminCfg)  return 0;
	epicsPrintf("Total device number: %d\n", pST_AdminCfg->n16DeviceNum );

	pSTDdev = (ST_STD_device*) ellFirst(pST_AdminCfg->pList_DeviceTask);
	while(pSTDdev) 
	{
		/*************************************************
		user add function for local device.
		**************************************************/	

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	if( make_KSTAR_CA_seq() == WR_ERROR )
		epicsPrintf("\n>>> drvAdmin_init_driver(): make_KSTAR_CA_seq() ... ERROR! \n");

	epicsAtExit((VOIDFUNCPTR) destroy_AdminTask, NULL);

	a = wf_getCurrentUsec();
	epicsThreadSleep(1.0);
	pST_AdminCfg->one_sec_interval = wf_getCurrentUsec() - a;

	epicsPrintf("drvAdmin: measured one sec. is %lf msec\n", 1.E-3 * (double)pST_AdminCfg->one_sec_interval);
	printf("Administrator device init OK!\n");
	printlog("IOC:System On ++++++++++++++++++++++++++++++++++++");

	return 0;
}

static void destroy_AdminTask(void)
{
	void *pnext, *pchNext;
	ST_STD_channel *pSTDch = NULL;
	ST_STD_device *pSTDdev = get_first_STDdev();
	ST_NI6259_dev *pNI6259 = NULL;

	printf("Exit Hook: Stopping Task %s ... \n", pST_AdminCfg->taskName); 
	while(pSTDdev) 
	{
		pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
		while(pSTDch)
		{
			/**********************************
			user channel clear function here
			***********************************/
			if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
			{
				/* clear_acq196Locally(pSTDdev); */
			} 

			
			if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) )
			{
				pNI6259 = pSTDdev->pUser;
				epicsEventSignal(pSTDdev->epicsEvent_DAQthread);
			} 

			/**********************************
			user channel clear function here
			***********************************/
			if( pSTDch->pUser)
				free(pSTDch->pUser);			

			pchNext = ellNext(&pSTDch->node);
			ellDelete(pSTDdev->pList_Channel, &pSTDch->node);
			printf("%s: call free()\n", pSTDch->chName);
			free((void *)pSTDch);
			pSTDch = (ST_STD_channel*) pchNext;
		}

		/**********************************
		user device clear function here
		***********************************/
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
		{
/*			clear_acq196Locally(pSTDdev); */
		} 
		/**********************************
		user device clear function here
		***********************************/

/* think about it next time.... consider DAQ wait event procedure */
/*		epicsEventDestroy(pSTDdev->epicsEvent_DAQthread ); */
/*
		if ( pSTDdev->taskLock )
			epicsMutexDestroy ( pSTDdev->taskLock );
*/
#if USE_LOCAL_DATA_POOL
		if( pSTDdev->pList_BufferNode )
			free(pSTDdev->pList_BufferNode);

		if( pSTDdev->pST_BuffThread )
			free(pSTDdev->pST_BuffThread);
#endif
		if( pSTDdev->pST_stdCtrlThread )
			free(pSTDdev->pST_stdCtrlThread);
		
		if( pSTDdev->pUser)
			free(pSTDdev->pUser);

		if( pSTDdev->pList_Channel )
			free(pSTDdev->pList_Channel);


		pnext = ellNext(&pSTDdev->node);
		ellDelete(pST_AdminCfg->pList_DeviceTask, &pSTDdev->node);
		printf("%s: call free()\n", pSTDdev->taskName);
		free((void *)pSTDdev);
		pSTDdev = (ST_STD_device*) pnext;
	}

	
	if ( pST_AdminCfg->lock_mds )
		epicsMutexDestroy ( pST_AdminCfg->lock_mds );

	if( pST_AdminCfg->pST_adminCtrlThread)
		free(pST_AdminCfg->pST_adminCtrlThread);
	
	if( pST_AdminCfg ) {
		printf("%s: call free()\n", pST_AdminCfg->taskName);
		free(pST_AdminCfg);		
	}

	printlog("IOC:System Off.");
	return;
}

static long drvAdmin_io_report(int level)
{
	if(level<1) return 0;
	epicsPrintf("  Task name: %s, ptr_addr: 0x%X, status: 0x%x \n",
		    pST_AdminCfg->taskName, 
		    (unsigned int)pST_AdminCfg, 
		    pST_AdminCfg->StatusAdmin   );
	if(level>2) {
		epicsPrintf("   measured one sec. is %lf msec\n", 1.E-3 * (double)pST_AdminCfg->one_sec_interval);
	}

	if(level>3 ) {
		
	}

	return 0;
}







ST_ADMIN* drvAdmin_get_AdminPtr()
{
	return pST_AdminCfg;
}

static void threadFunc_user_CatchEnd(void *param)
{
#if 0
	ST_ACQ196 *pAcq196;
	ST_STD_device *pSTDdev = (ST_STD_device*) param;

	while (!pSTDdev)  {
		printf("ERROR! threadFunc_user_CatchEnd() has null pointer of STD device.\n");
		epicsThreadSleep(.1);
	}
	pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
	while(TRUE) 
	{
		epicsEventWait( pSTDdev->epicsEvent_CatchEnd);
		epicsThreadSleep( pSTDdev->ST_mds_fetch.dT1[0] + 0.1 );
		printf(">>> %s: Caught the T1 stop trigger!\n", pSTDdev->taskName);

		DBproc_put( pAcq196->pvName_DEV_RUN, "0" );
		epicsThreadSleep( 1.0 );
/*
		if( !drvACQ196_RUN_stop(pSTDdev) )
			continue;
*/
		drvACQ196_wait_RUN_stop_complete(pSTDdev);

		DBproc_put( pAcq196->pvName_DEV_ARMING, "0" );
		epicsThreadSleep( 1.0 );


		pST_AdminCfg->StatusAdmin &= ~TASK_SYSTEM_IDLE;
		pST_AdminCfg->StatusAdmin |= TASK_AFTER_SHOT_PROCESS;
		scanIoRequest(pST_AdminCfg->ioScanPvt_status);
		if( drvACQ196_set_data_parsing( pSTDdev ) == WR_ERROR )
			continue;

		notify_refresh_admin_status();
		
		if( pST_AdminCfg->cUseAutoSave )
		{

			pST_AdminCfg->StatusAdmin |= TASK_DATA_PUT_STORAGE;
			scanIoRequest(pST_AdminCfg->ioScanPvt_status);

			if( admin_spinLock_mds_put_flag(pSTDdev) == WR_OK ) 
			{ 
				printf("\"%s\" start data tranfer to MDS tree.\n", pSTDdev->taskName );
				if( drvACQ196_set_send_data( pSTDdev) == WR_ERROR ) {
					printf("\"%s\" data send error!\n ", pSTDdev->taskName );
					pST_AdminCfg->n8MdsPutFlag  = 0;
					continue;
				}
				printf("\"%s\" release mds put flag ", pSTDdev->taskName );
				/*epicsMutexLock(pST_AdminCfg->lock_mds);*/
				pST_AdminCfg->n8MdsPutFlag  = 0; /* ok. I'm finished. release write process. */				
				/*epicsMutexUnlock(pST_AdminCfg->lock_mds);*/
				printf("... OK.\n");
				
			}
			notify_refresh_admin_status();
/*
			epicsMutexLock(pST_AdminCfg->lock_mds);
			drvACQ196_set_send_data( pSTDdev);
			epicsMutexUnlock(pST_AdminCfg->lock_mds);
*/
			if( drvACQ196_check_AllStorageFinished() == WR_OK ) 
			{
				printf("\n######################################(%s)\n", pSTDdev->taskName);
				printf("DDS1: Ready to Next Shot(%s)\n", pSTDdev->taskName);
#if USE_MDS_EVENT_NOTIFY				
				mds_notify_EndOfTreePut(&(pSTDdev->STD));	/* it will be good for Remote mode.		 2009-11-20 */	
#endif
				wf_print_date();					/* 2009-11-24 */
				pST_AdminCfg->StatusAdmin |= TASK_SYSTEM_IDLE;
				notify_refresh_admin_status();
				printf("######################################(%s)\n", pSTDdev->taskName);
				DBproc_put(SYS_RUN, "0");
				DBproc_put(SYS_ARMING, "0");
			}
		
		}	/* eof auto data put function */
		
		/* back to waiting for next shot start */
		
	}/* eof  While(TRUE) */
#endif
}

static ST_threadCfg*  make_STD_thread_Control(ST_STD_device *pSTDdev)
{
	ST_threadCfg *pST_ctrlThread  = NULL;

	pST_ctrlThread  = (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pST_ctrlThread) return pST_ctrlThread; 

	sprintf(pST_ctrlThread->threadName, "%s_ctrl", pSTDdev->taskName); 

	pST_ctrlThread->threadPriority = epicsThreadPriorityLow;
	pST_ctrlThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall); 
	pST_ctrlThread->threadFunc      = (EPICSTHREADFUNC) threadFunc_STD_control;
	pST_ctrlThread->threadParam     = (void*) pSTDdev;
	pST_ctrlThread->threadQueueId   = epicsMessageQueueCreate(1000,sizeof(ST_threadQueueData));

	epicsThreadCreate(pST_ctrlThread->threadName,
			  pST_ctrlThread->threadPriority,
			  pST_ctrlThread->threadStackSize,
			  (EPICSTHREADFUNC) pST_ctrlThread->threadFunc,
			  (void*) pST_ctrlThread->threadParam);

	return pST_ctrlThread;
}

static int make_STD_thread_DAQ(ST_STD_device *pSTDdev)
{
	char cName[SIZE_TASK_NAME];
	sprintf(cName, "%s_DAQ", pSTDdev->taskName );

	pSTDdev->epicsEvent_DAQthread = epicsEventCreate(epicsEventEmpty);
	if ( ! pSTDdev->epicsEvent_DAQthread ) {
		return WR_ERROR;
	}
	
	if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
		{

			epicsThreadCreate(cName, 
						epicsThreadPriorityHigh,
						epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadStackBig */
						threadFunc_DAQ_NI6123,
						(void*)pSTDdev); 
			return WR_OK;
		}
	else if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_2) )
		{

			epicsThreadCreate(cName, 
						epicsThreadPriorityHigh,
						epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadStackBig */
						threadFunc_DAQ_NI6259,
						(void*)pSTDdev); 
			return WR_OK;
		}
	else if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_3) )
		{

			epicsThreadCreate(cName, 
						epicsThreadPriorityHigh,
						epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadStackBig */
						threadFunc_Power_Supply_IT6322,
						(void*)pSTDdev); 
			return WR_OK;
		}
}

static int  make_STD_thread_CatchEnd(ST_STD_device *pSTDdev)
{
	char cName[SIZE_TASK_NAME];
	sprintf(cName, "%s_CatchEnd", pSTDdev->taskName );

	pSTDdev->epicsEvent_CatchEnd = epicsEventCreate(epicsEventEmpty);
	if ( ! pSTDdev->epicsEvent_CatchEnd ) {
		printf("ERROR! %s can't create \"epicsEvent_CatchEnd\" \n", pSTDdev->taskName );		
		return WR_ERROR;
	}
	
	epicsThreadCreate(cName, 
			epicsThreadPriorityMedium,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			threadFunc_user_CatchEnd,
			(void*)pSTDdev);	
	return WR_OK;
}

#if USE_LOCAL_DATA_POOL
static ST_threadCfg*  make_STD_thread_RingBuff(ST_STD_device *pSTDdev)
{
	ST_threadCfg *pST_BuffThread  = NULL;

	pSTDdev->pList_BufferNode = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pSTDdev->pList_BufferNode) {
		printf("ERROR! %s malloc ( pList_BufferNode )  failed. \n", pSTDdev->taskName );
		return NULL;
	} else 
		ellInit(pSTDdev->pList_BufferNode);

	pST_BuffThread  = (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pST_BuffThread){
		printf("ERROR! %s malloc ( pST_BuffThread )  failed. \n", pSTDdev->taskName );
		free(pSTDdev->pList_BufferNode);
		return pST_BuffThread; 
	}
	sprintf(pST_BuffThread->threadName, "%s_Buf_ctrl", pSTDdev->taskName);

	pST_BuffThread->threadPriority = epicsThreadPriorityHigh;
	pST_BuffThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackMedium);  /* epicsThreadStackBig */
	pST_BuffThread->threadFunc      = (EPICSTHREADFUNC) threadFunc_user_RingBuf;
	pST_BuffThread->threadParam     = (void*) pSTDdev;
	pST_BuffThread->threadQueueId   = epicsMessageQueueCreate(50, sizeof(ST_User_Buff_node));

	epicsThreadCreate(pST_BuffThread->threadName,
			  pST_BuffThread->threadPriority,
			  pST_BuffThread->threadStackSize,
			  (EPICSTHREADFUNC) pST_BuffThread->threadFunc,
			  (void*) pST_BuffThread->threadParam);

	return pST_BuffThread;
}
#endif




void notify_admin_error_info(int level, const char *fmt, ...)
{
	char buf[SIZE_STRINGOUT_VAL];
	int len;
	va_list argp;
	
	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	pST_AdminCfg->StatusAdmin &= ~TASK_SYSTEM_READY;
	len = strlen(buf);
	if( len > SIZE_STRINGOUT_VAL ) 
	{
		printf("ERROR. string buffer overflow! (%d) \n", len);
		return;
	}
	if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;
	switch(level) {
		case 0: 
			sprintf(pST_AdminCfg->ErrorString, "%s", buf);
			scanIoRequest(pST_AdminCfg->ioScanPvt_status);
			break;
		case 1:
			sprintf(pST_AdminCfg->ErrorString, "%s", buf);
			epicsPrintf("%s\n", pST_AdminCfg->ErrorString);
			scanIoRequest(pST_AdminCfg->ioScanPvt_status);
			break;
		default: printf("%s\n", buf ); break;
	}
}

void notify_refresh_admin_status()
{
	int bit[6], i;

	ST_STD_device *pSTDdev = get_first_STDdev();
	for( i=0; i<6; i++) bit[i]=0;

	while(pSTDdev) {
		
		if( !strcmp(pSTDdev->devType, "NI6123"))
			{
				if( pSTDdev->StatusDev & TASK_SYSTEM_READY ) bit[0]++;
				if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) bit[1]++;
				if( pSTDdev->StatusDev & TASK_DAQ_STARTED ) bit[2]++;
				if( pSTDdev->StatusDev & TASK_AFTER_SHOT_PROCESS ) bit[3]++;
				if( pSTDdev->StatusDev & TASK_DATA_PUT_STORAGE ) bit[4]++;
				if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) bit[5]++;
			}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	if( bit[0] == pST_AdminCfg->n16DeviceNum ) pST_AdminCfg->StatusAdmin |= TASK_SYSTEM_READY;
	else pST_AdminCfg->StatusAdmin &= ~TASK_SYSTEM_READY;

	if( bit[1] == pST_AdminCfg->n16DeviceNum )  pST_AdminCfg->StatusAdmin |= TASK_ARM_ENABLED;
	else pST_AdminCfg->StatusAdmin &= ~TASK_ARM_ENABLED;

	if( bit[2] == 0 )  pST_AdminCfg->StatusAdmin &= ~TASK_DAQ_STARTED;
	else pST_AdminCfg->StatusAdmin |= TASK_DAQ_STARTED;

	if( bit[3] == 0 )  pST_AdminCfg->StatusAdmin &= ~TASK_AFTER_SHOT_PROCESS;
	else pST_AdminCfg->StatusAdmin |= TASK_AFTER_SHOT_PROCESS;

	if( bit[4] == 0 )  pST_AdminCfg->StatusAdmin &= ~TASK_DATA_PUT_STORAGE;
	else pST_AdminCfg->StatusAdmin |= TASK_DATA_PUT_STORAGE;

	if( bit[5] == pST_AdminCfg->n16DeviceNum )  pST_AdminCfg->StatusAdmin |= TASK_SYSTEM_IDLE;
	else pST_AdminCfg->StatusAdmin &= ~TASK_SYSTEM_IDLE;

	scanIoRequest(pST_AdminCfg->ioScanPvt_status);
	
}

int admin_spinLock_mds_put_flag( ST_STD_device *pSTDdev )
{
	while(pST_AdminCfg->n8EscapeWhile) 
	{
		pSTDdev->StatusDev |= TASK_DATA_PUT_STORAGE;
		if( pST_AdminCfg->n8MdsPutFlag == 0){ /* any body use tree put */
			/*epicsMutexLock(pST_AdminCfg->lock_mds);*/
			pST_AdminCfg->n8MdsPutFlag  = 1 ; /* i'll use write process bye. */
			/*epicsMutexUnlock(pST_AdminCfg->lock_mds);*/
			return WR_OK;
		} 
		else 
			epicsPrintf(".%s ", pSTDdev->taskName );
		
		epicsThreadSleep(1.0);
	}
	pST_AdminCfg->n8EscapeWhile = 1;

	return WR_ERROR;	
}

void flush_StBase_to_STDdev( ST_STD_device *pSTDdev )
{
	int nval;
	pSTDdev->ST_Base.opMode = pST_AdminCfg->ST_Base.opMode;
	pSTDdev->ST_Base.shotNumber = pST_AdminCfg->ST_Base.shotNumber;
	pSTDdev->ST_Base.fBlipTime = pST_AdminCfg->ST_Base.fBlipTime;
	pSTDdev->ST_Base.nSamplingRate = pST_AdminCfg->ST_Base.nSamplingRate;
	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
		pSTDdev->ST_Base.dT0[nval] = pST_AdminCfg->ST_Base.dT0[nval];
		pSTDdev->ST_Base.dT1[nval] = pST_AdminCfg->ST_Base.dT1[nval];
	}
}
void flush_STbase_to_All_STDdev( )
{
	int nval;
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.opMode = pST_AdminCfg->ST_Base.opMode;
		pSTDdev->ST_Base.shotNumber = pST_AdminCfg->ST_Base.shotNumber;
		pSTDdev->ST_Base.fBlipTime = pST_AdminCfg->ST_Base.fBlipTime;
		pSTDdev->ST_Base.nSamplingRate = pST_AdminCfg->ST_Base.nSamplingRate;
		for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
			pSTDdev->ST_Base.dT0[nval] = pST_AdminCfg->ST_Base.dT0[nval];
			pSTDdev->ST_Base.dT1[nval] = pST_AdminCfg->ST_Base.dT1[nval];
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void flush_Blip_to_All_STDdev( )
{
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.fBlipTime = pST_AdminCfg->ST_Base.fBlipTime;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

void flush_dT0_to_All_STDdev( )
{
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.dT0[0] = pST_AdminCfg->ST_Base.dT0[0];
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

void flush_ShotNum_to_All_STDdev( )
{
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.shotNumber = pST_AdminCfg->ST_Base.shotNumber;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void flush_STDdev_to_MDSfetch( ST_STD_device *pSTDdev )
{
	int nval;
	pSTDdev->ST_mds_fetch.opMode = pSTDdev->ST_Base.opMode;
	pSTDdev->ST_mds_fetch.shotNumber = pSTDdev->ST_Base.shotNumber;
	pSTDdev->ST_mds_fetch.fBlipTime = pSTDdev->ST_Base.fBlipTime;
	pSTDdev->ST_mds_fetch.nSamplingRate = pSTDdev->ST_Base.nSamplingRate;
	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
		pSTDdev->ST_mds_fetch.dT0[nval] = pSTDdev->ST_Base.dT0[nval];
		pSTDdev->ST_mds_fetch.dT1[nval] = pSTDdev->ST_Base.dT1[nval];
	}	
}

void STDdev_status_reset(ST_STD_device *pSTDdev)
{
	pSTDdev->StatusMds = MDS_INIT;	
	pSTDdev->StatusDev = TASK_NOT_INIT;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	pSTDdev->ErrorDev = ERROR_NONE;
}

void admin_all_taskStatus_reset()
{
	ST_STD_device *pSTDdev=NULL;

	pST_AdminCfg->StatusAdmin = TASK_NOT_INIT;
	pST_AdminCfg->StatusAdmin |= TASK_SYSTEM_IDLE;
	pST_AdminCfg->ErrorAdmin = ERROR_NONE;

	pSTDdev = (ST_STD_device*) ellFirst(pST_AdminCfg->pList_DeviceTask);
	while(pSTDdev) {
		STDdev_status_reset(pSTDdev);
		
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}			
}




ST_STD_device* get_STDev_from_name(char *taskName)
{
	ST_STD_device *pSTDdev = NULL;
	pSTDdev = (ST_STD_device*) ellFirst(pST_AdminCfg->pList_DeviceTask);
	while(pSTDdev) {
		if(!strcmp(pSTDdev->taskName, taskName)) return pSTDdev;
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	return pSTDdev;
}
ST_STD_device* get_first_STDdev()
{
	return (ST_STD_device*) ellFirst(pST_AdminCfg->pList_DeviceTask);
}

ST_STD_channel* get_STCh_from_STDev_chName(ST_STD_device *pSTDdev, char *chanName)
{
	ST_STD_channel *pSTDch = NULL;

	pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
	while(pSTDch) {
		if(!strcmp(pSTDch->chName, chanName)) return pSTDch;
		pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
	}
	return pSTDch;
}
ST_STD_channel* get_STCh_from_STDev_chID(ST_STD_device *pSTDdev, int ch)
{
	ST_STD_channel *pSTDch = NULL;

	pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
	while(pSTDch) {
		if(pSTDch->ChannelID == ch) return pSTDch;
		pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
	}
	return pSTDch;
}

ST_STD_channel* get_STCh_from_devName_chName(char *devName, char *chanName)
{
	ST_STD_device *pSTDdev = get_STDev_from_name(devName);
	if(!pSTDdev) return NULL;

	return get_STCh_from_STDev_chName(pSTDdev, chanName);
}

ST_STD_channel* get_STCh_from_devName_chID(char *devName, int ch)
{
	ST_STD_device *pSTDdev = get_STDev_from_name(devName);
	if(!pSTDdev) return NULL;

	return get_STCh_from_STDev_chID(pSTDdev, ch);
}

int admin_check_Arming_condition()
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	if( !(pAdminCfg->StatusAdmin & TASK_SYSTEM_READY) ) 
	{
		epicsPrintf(">> Request Arming but \"%s\" not ready! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	if( !(pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf(">> Request Arming but \"%s\" not Idle condition! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}
int admin_check_Release_condition()
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	if( !(pAdminCfg->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request release but \"%s\" not yet armed! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	if( pAdminCfg->StatusAdmin & TASK_DAQ_STARTED ) 
	{
		epicsPrintf(">> Request release but \"%s\" already run! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}

int admin_check_Run_condition()
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	if( !(pAdminCfg->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request Run but \"%s\" need to arming! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	if( pAdminCfg->StatusAdmin & TASK_DAQ_STARTED ) 
	{
		epicsPrintf(">> Request Run but \"%s\" already run! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}
int admin_check_Stop_condition()
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	if( !(pAdminCfg->StatusAdmin & TASK_DAQ_STARTED) ) 
	{
		epicsPrintf(">> Request Stop but \"%s\" not run! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}
ST_STD_device* get_STDev_from_Dev_Type(char *taskName)

{
	ST_STD_device *pSTDdev = NULL;
	pSTDdev = (ST_STD_device*) ellFirst(pST_AdminCfg->pList_DeviceTask);
	while(pSTDdev) {
		if(!strcmp(pSTDdev->devType, taskName)) return pSTDdev;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	return pSTDdev;
}

void flush_STDdev_to_mdsplus( void *pArg )
{
	int nval;
	ST_STD_device *pSTDdev = (ST_STD_device *)pArg;
	pSTDdev->ST_mds_fetch.opMode = pSTDdev->ST_Base.opMode;
	pSTDdev->ST_mds_fetch.shotNumber = pSTDdev->ST_Base.shotNumber;
	pSTDdev->ST_mds_fetch.fBlipTime = pSTDdev->ST_Base.fBlipTime;
	epicsPrintf("fetch_Blip_time %f\n", pSTDdev->ST_mds_fetch.fBlipTime);
	pSTDdev->ST_mds_fetch.nSamplingRate = pSTDdev->ST_Base.nSamplingRate;
	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
		pSTDdev->ST_mds_fetch.dT0[nval] = pSTDdev->ST_Base.dT0[nval];
		pSTDdev->ST_mds_fetch.dT1[nval] = pSTDdev->ST_Base.dT1[nval];
	}	
}

