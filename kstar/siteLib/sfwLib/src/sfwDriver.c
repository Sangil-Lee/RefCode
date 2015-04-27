/****************************************************************************

Module      : sfwDriver.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2010-1-24

2010. 6. 8  divide  ioScanPvt  to status and user call.    by woong.
2011. 1. 11  rename and reconfiguration. by woong
2011. 2. 9   increase argument string size from 10 to 20
		    add function: get_STDev_from_type( char * given name )
2011. 8. 24  add string to get TOP, IOC environment variable.
2012. 3. 9 several functions are modified to make master run as a controller.
		  device number could be negative.
		  add how to make simple controller "appExample.c"
		  modify error print PV
*****************************************************************************/

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"

#include "sfwInitControl.h"

static ST_MASTER          *pST_Master = NULL;


static ST_threadCfg*  make_STD_Master_CtrlThread();
static void threadFunc_STD_MainCtrl(void *param);
static void threadFunc_STD_DevCtrl(void *param);
static void threadFunc_STD_Master_Stop(void *param);
static void destroy_STD_AdminTask(void);

static void createMainTaskCallFunction(const iocshArgBuf *args);
static void createDeviceCallFunction(const iocshArgBuf *args);
static void createChannelCallFunction(const iocshArgBuf *args);
static void setMDSplusCallFunction(const iocshArgBuf *args);

static ST_STD_device* make_STD_deviceCallFunc(const iocshArgBuf *args);


static const iocshArg createSfwTaskArg0 = {"task_name", iocshArgString};
static const iocshArg* const createSfwTaskArgs[] = { &createSfwTaskArg0 };
static const iocshFuncDef createSfwTaskFuncDef = { "createMainTask", 1, createSfwTaskArgs };


static const iocshArg createDeviceName = {"device_name", iocshArgString};
static const iocshArg createDeviceType = {"dev type", iocshArgString};
static const iocshArg createDeviceArg0 = {"arg0", iocshArgString};
static const iocshArg createDeviceArg1 = {"arg1", iocshArgString};
static const iocshArg createDeviceArg2 = {"arg2", iocshArgString};
static const iocshArg createDeviceArg3 = {"arg3", iocshArgString};
static const iocshArg createDeviceArg4 = {"arg4", iocshArgString};
static const iocshArg createDeviceArg5 = {"arg5", iocshArgString};
static const iocshArg createDeviceArg6 = {"arg6", iocshArgString};
static const iocshArg createDeviceArg7 = {"arg7", iocshArgString};
static const iocshArg createDeviceArg8 = {"arg8", iocshArgString};
static const iocshArg createDeviceArg9 = {"arg9", iocshArgString};
static const iocshArg* const createDeviceArgs[] = { &createDeviceName , &createDeviceType, &createDeviceArg0, &createDeviceArg1, &createDeviceArg2, &createDeviceArg3, &createDeviceArg4, &createDeviceArg5, &createDeviceArg6, &createDeviceArg7, &createDeviceArg8, &createDeviceArg9 };
static const iocshFuncDef createDeviceFuncDef = { "createDevice", 12, createDeviceArgs };


static const iocshArg createChannelDevName = {"device_name", iocshArgString};
/*atic const iocshArg createChannelName = {"ch name", iocshArgString}; */
static const iocshArg createChannelId = {"ch Id", iocshArgString};
static const iocshArg createChannelArg0 = {"arg0", iocshArgString};
static const iocshArg createChannelArg1 = {"arg1", iocshArgString};
static const iocshArg createChannelArg2 = {"arg2", iocshArgString};
static const iocshArg createChannelArg3 = {"arg3", iocshArgString};
static const iocshArg createChannelArg4 = {"arg4", iocshArgString};
static const iocshArg createChannelArg5 = {"arg5", iocshArgString};
static const iocshArg createChannelArg6 = {"arg6", iocshArgString};
static const iocshArg createChannelArg7 = {"arg7", iocshArgString};
static const iocshArg createChannelArg8 = {"arg8", iocshArgString};
static const iocshArg createChannelArg9 = {"arg9", iocshArgString};
/*atic const iocshArg* const createChannelArgs[] = { &createChannelDevName, &createChannelName, &createChannelId, &createChannelArg0, &createChannelArg1, &createChannelArg2, &createChannelArg3, &createChannelArg4, &createChannelArg5, &createChannelArg6, &createChannelArg7, &createChannelArg8, &createChannelArg9 };*/
static const iocshArg* const createChannelArgs[] = { &createChannelDevName, &createChannelId, &createChannelArg0, &createChannelArg1, &createChannelArg2, &createChannelArg3, &createChannelArg4, &createChannelArg5, &createChannelArg6, &createChannelArg7, &createChannelArg8, &createChannelArg9 };
static const iocshFuncDef createChannelFuncDef = {"createChannel", 12, createChannelArgs};

static const iocshArg setMDSplusArg0 = {"device_name", iocshArgString};
static const iocshArg setMDSplusArg1 = {"filter", iocshArgString};
static const iocshArg setMDSplusArg2 = {"value", iocshArgString};
static const iocshArg* const setMDSplusArgs[] = { &setMDSplusArg0, &setMDSplusArg1, &setMDSplusArg2 };
static const iocshFuncDef setMDSplusFuncDef = {"setMDSplus", 3, setMDSplusArgs};



static void sfwTaskRegister(void)
{
	iocshRegister(&createSfwTaskFuncDef, createMainTaskCallFunction);
	iocshRegister(&createDeviceFuncDef, createDeviceCallFunction);
	iocshRegister(&createChannelFuncDef, createChannelCallFunction);
	iocshRegister(&setMDSplusFuncDef, setMDSplusCallFunction);
}
epicsExportRegistrar(sfwTaskRegister);

static long drvSFW_io_report(int level);
static long drvSFW_init_driver();
struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvSFW = {
       2,
       drvSFW_io_report,
       drvSFW_init_driver
};
epicsExportAddress(drvet, drvSFW);



extern int make_KSTAR_CA_seq();
extern void initControl(const char *name) { }

int create_AdminConfig()
{
	if(pST_Master) return 0;
	
	pST_Master = (ST_MASTER*) malloc(sizeof(ST_MASTER));
	if(!pST_Master) return 1;
	pST_Master->pUser = NULL; /* 2013. 6. 7 */

	pST_Master->pList_DeviceTask= (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pST_Master->pList_DeviceTask) { free(pST_Master); return 1; }
	else ellInit(pST_Master->pList_DeviceTask);

	return 0;
}

static void createMainTaskCallFunction(const iocshArgBuf * args)
{
	char task_name[SIZE_TASK_NAME];

	if(args[0].sval) strcpy(task_name, args[0].sval);

	if(create_AdminConfig()) return;

	epicsThreadSleep(.5);

	strcpy(pST_Master->taskName, task_name);

	pST_Master->lock_mds = epicsMutexCreate();
	scanIoInit(&pST_Master->ioScanPvt_status);
	pST_Master->StatusAdmin = TASK_NOT_INIT;
	pST_Master->StatusAdmin |= TASK_SYSTEM_IDLE;
	pST_Master->ErrorAdmin = ERROR_NONE;
	strcpy(pST_Master->ErrorString, "no error");

	/*Common base variables *********************************/
	pST_Master->ST_Base.opMode = OPMODE_INIT;
	pST_Master->ST_Base.shotNumber = 1;
	pST_Master->ST_Base.fBlipTime = INIT_BLIP_TIME;
	pST_Master->ST_Base.fStepTime = INIT_STEP_TIME;
	pST_Master->ST_Base.nSamplingRate = INIT_SAMPLING;
	pST_Master->ST_Base.dT0[0] = 1.0;
	pST_Master->ST_Base.dT1[0] = 10.0;
	pST_Master->ST_Base.dRunPeriod = 10.0;
	/*Common base variables *********************************/
	
	pST_Master->cUseAutoSave = 1;
	pST_Master->cEnable_IPMIN = 1;
	pST_Master->cUseAutoCreateTree = 0x0;
	pST_Master->s32DeviceNum = -1;
	pST_Master->n8MdsPutFlag = 0; /* must start with "0" */
	pST_Master->n8EscapeWhile = 1;
	pST_Master->n8SaveLog = 1;
	pST_Master->s32ShowLevel = 0;

/*	pST_Master->cStatus_ip_min_fault = 0;
	pST_Master->flag_plasma_status = 0;
	pST_Master->flag_sequence_on = 0; */
	reset_event_flag();

	if( env_SYS_pvname_assign(pST_Master->taskName) == WR_ERROR ) {
		kfwPrint(0, ">>>>>>>>>>> ERROR !!!!!   check master epicsEnvSet()! \n");
		free(pST_Master);
		return;
	}
	if( env_MDS_Info_assign() == WR_ERROR ) {
		free(pST_Master);
		return;
	}
	if( env_Get_EPICS_ENV() == WR_ERROR ) {
		/* do noting */
	}

	pST_Master->pST_MstrCtrlThread = make_STD_Master_CtrlThread( );
	if(!pST_Master->pST_MstrCtrlThread) { free(pST_Master); return; }

	if(make_STD_Master_Stop_thread() == WR_ERROR) {		
		kfwPrint(0, " %s can't create \"ST_StopEventThread\" \n", pST_Master->taskName );
		free(pST_Master); 
		return;
	}

	/* 2012. 3. 6 add external call function in order to run as a controller */
	pST_Master->ST_Func._BOARD_SETUP=NULL;
	pST_Master->ST_Func._SYS_RESET=NULL;
	pST_Master->ST_Func._DATA_SEND=NULL;
	pST_Master->ST_Func._SYS_ARMING=NULL;
	pST_Master->ST_Func._SYS_RUN=NULL;
	pST_Master->ST_Func._SAMPLING_RATE=NULL;
	pST_Master->ST_Func._SYS_T0=NULL;
	pST_Master->ST_Func._SYS_T1=NULL;
	pST_Master->ST_Func._OP_MODE=NULL;
	pST_Master->ST_Func._SHOT_NUMBER=NULL; /* doesn't support*/
	pST_Master->ST_Func._CREATE_LOCAL_TREE=NULL;   /* doesn't support*/
	pST_Master->ST_Func._TEST_PUT=NULL;   /* doesn't support*/
	pST_Master->ST_Func._Exit_Call=NULL;

	pST_Master->ST_Func._PRE_SEQSTART=NULL;
	pST_Master->ST_Func._PRE_SEQSTOP=NULL;
	pST_Master->ST_Func._POST_SEQSTOP=NULL;
	pST_Master->ST_Func._PRE_SHOTSTART=NULL;
	pST_Master->ST_Func._TREE_CREATE=NULL;
	pST_Master->ST_Func._FAULT_IP_MINIMUM=NULL;

	epicsPrintf("make Main control thread ok!: %p\n", pST_Master->pST_MstrCtrlThread);

	return;
}

static void createDeviceCallFunction(const iocshArgBuf *args)
{
	ST_STD_device* pSTDdev = NULL;

	if( !pST_Master ) return;
	pSTDdev = make_STD_deviceCallFunc(args);
	if( !pSTDdev) return;

	pSTDdev->ST_Func._BOARD_SETUP=NULL;
	pSTDdev->ST_Func._SYS_RESET=NULL;
	pSTDdev->ST_Func._DATA_SEND=NULL;
	pSTDdev->ST_Func._SYS_ARMING=NULL;
	pSTDdev->ST_Func._SYS_RUN=NULL;
	pSTDdev->ST_Func._SAMPLING_RATE=NULL;
	pSTDdev->ST_Func._SYS_T0=NULL;
	pSTDdev->ST_Func._SYS_T1=NULL;
	pSTDdev->ST_Func._OP_MODE=NULL;
	pSTDdev->ST_Func._SHOT_NUMBER=NULL;
	pSTDdev->ST_Func._CREATE_LOCAL_TREE=NULL;
	pSTDdev->ST_Func._TEST_PUT=NULL;
	pSTDdev->ST_Func._Exit_Call=NULL;

	pSTDdev->ST_Func._PRE_SEQSTART=NULL;
	pSTDdev->ST_Func._PRE_SEQSTOP=NULL;
	pSTDdev->ST_Func._POST_SEQSTOP=NULL;
	pSTDdev->ST_Func._PRE_SHOTSTART=NULL;
	pSTDdev->ST_Func._TREE_CREATE=NULL;
	pSTDdev->ST_Func._FAULT_IP_MINIMUM=NULL;
	
#if 0
	if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
	{
		/*********************************************************
		user edit here!
		**********************************************************/
		if(create_16aiss8ao4_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("[SFW]  \"%s\" create_16aiss8ao4_taskConfig() failed.\n", pSTDdev->taskName );
			if( pSTDdev) free(pSTDdev);
			return;
		}

	}
#endif

	ellAdd(pST_Master->pList_DeviceTask, &pSTDdev->node);

	return;
}

static void createChannelCallFunction(const iocshArgBuf *args)
{
	ST_STD_device *pSTDdev = NULL;
	ST_STD_channel *pSTDch = NULL;
	char device_name[SIZE_TASK_NAME];
/*	char ch_name[SIZE_TASK_NAME]; */

	if(args[0].sval) strcpy(device_name, args[0].sval);
/*	if(args[1].sval) strcpy(ch_name, args[1].sval); */
	
	pSTDdev = get_STDev_from_name(device_name);
	if(!pSTDdev) {
		kfwPrint(0, "Can't find device task \"%s\" \n", device_name );
		return;
	}
/*	pSTDch = (ST_STD_channel*) malloc(sizeof(ST_STD_channel)); */
	pSTDch = (ST_STD_channel*) calloc(1, sizeof(ST_STD_channel));
	if(!pSTDch) {
		kfwPrint(0, "%s's channel malloc() failed \n", pSTDdev->taskName );
		return;
	}
	pSTDch->StatusCh = 0x0;
	pSTDch->pUser = NULL;  /* 2013. 6. 7 */

/*	strcpy(pSTDch->chName, ch_name);*/
	if(args[1].sval) pSTDch->channelId = strtoul(args[1].sval,NULL,0);
	
	if(args[2].sval) strcpy(pSTDch->strArg0, args[2].sval);	
	if(args[3].sval) strcpy(pSTDch->strArg1, args[3].sval);
	if(args[4].sval) strcpy(pSTDch->strArg2, args[4].sval);
	if(args[5].sval) strcpy(pSTDch->strArg3, args[5].sval);
	if(args[6].sval) strcpy(pSTDch->strArg4, args[6].sval);
	if(args[7].sval) strcpy(pSTDch->strArg5, args[7].sval);
	if(args[8].sval) strcpy(pSTDch->strArg6, args[8].sval);
	if(args[9].sval) strcpy(pSTDch->strArg7, args[9].sval);
	if(args[10].sval) strcpy(pSTDch->strArg8, args[10].sval);
	if(args[11].sval) strcpy(pSTDch->strArg9, args[11].sval);

	sprintf(pSTDch->chName, "CH%d", pSTDch->channelId);	
	pSTDch->f32Gain= 1.0;
	pSTDch->f32Offset = 0.0;
	strcpy(pSTDch->strTagName, "NULL");
	
	pSTDch->pST_parentSTDdev = pSTDdev;


	pSTDch->_Exit_Call=NULL;
	
	ellAdd(pSTDdev->pList_Channel, &pSTDch->node);
	
	return;
}

static void destroy_STD_AdminTask(void)
{
	void *pnext, *pchNext;
	ST_STD_channel *pSTDch = NULL;
	ST_STD_device *pSTDdev = get_first_STDdev();

	kfwPrint(0, "Exit Hook: destroy driver support routine '%s' ... \n", pST_Master->taskName); 
	while(pSTDdev) 
	{
		pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
		while(pSTDch)
		{
			if(pSTDch->_Exit_Call)
				(pSTDch->_Exit_Call)(pSTDch, 0.0, 0.0);

			pchNext = ellNext(&pSTDch->node);
			ellDelete(pSTDdev->pList_Channel, &pSTDch->node);
			kfwPrint(0, " %s: call free()\n", pSTDch->chName);
			if( pSTDch->pUser ) free(pSTDch->pUser); /* 2013. 6. 7 */
			free((void *)pSTDch);
			pSTDch = NULL;
			pSTDch = (ST_STD_channel*) pchNext;
		}
		if( pSTDdev->pList_Channel ) {
			free(pSTDdev->pList_Channel);
			pSTDdev->pList_Channel = NULL;
			kfwPrint(0, " %s: clear channel.\n", pSTDdev->taskName);
		}

		
		if(pSTDdev->ST_Func._Exit_Call)
			(pSTDdev->ST_Func._Exit_Call)(pSTDdev, 0.0, 0.0);

/* think about it next time.... consider DAQ wait event procedure */
/*		epicsEventDestroy(pSTDdev->epicsEvent_DAQthread ); */
/*
		if ( pSTDdev->taskLock )
			epicsMutexDestroy ( pSTDdev->taskLock );
*/

		if( pSTDdev->pList_BufferNode ) {
			free(pSTDdev->pList_BufferNode);
			pSTDdev->pList_BufferNode = NULL;
			kfwPrint(0, "%s: clear Ring buffer node.\n", pSTDdev->taskName);
		}

/*		if( pSTDdev->pST_RingBufThread )
			free(pSTDdev->pST_RingBufThread); */

		if( pSTDdev->pST_stdCtrlThread ) {
			free(pSTDdev->pST_stdCtrlThread);
			pSTDdev->pST_stdCtrlThread = NULL;
			kfwPrint(0, "%s: clear control thread.\n", pSTDdev->taskName);
		}
		

		pnext = ellNext(&pSTDdev->node);
		ellDelete(pST_Master->pList_DeviceTask, &pSTDdev->node);
		kfwPrint(0, "%s: call free()\n", pSTDdev->taskName);
		if( pSTDdev->pUser ) free(pSTDdev->pUser); /* 2013. 6. 7 */
		free((void *)pSTDdev);
		pSTDdev = NULL;
		pSTDdev = (ST_STD_device*) pnext;
	}

	if(pST_Master->ST_Func._Exit_Call)
		(pST_Master->ST_Func._Exit_Call)(pST_Master, 0.0, 0.0);

	
	if ( pST_Master->lock_mds )
		epicsMutexDestroy ( pST_Master->lock_mds );

	if( pST_Master->pST_MstrCtrlThread)
		free(pST_Master->pST_MstrCtrlThread);

	printlog("IOC:System Off.");

	if( pST_Master->pUser ) free(pST_Master->pUser); /* 2013. 6. 7 */

	if( pST_Master ) {
		free(pST_Master);
		pST_Master = NULL;
		printf("SFW terminated!\n"); /* can not use kfwPrintf() */
	}

	return;
}

/******************************************************************************
*
*  FUNCTION:   setMDSplusCallFunction
*
*  PURPOSE:    must be called after iocInit(). 

*  PARAMETERS: 
				

*  RETURNS:
*  GLOBALS:
*
******************************************************************************/
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
		kfwPrint(0, "No \"%s\" device!\n", task_name );
		return;
	}
	if( env_MDSinfo_to_STDdev(pSTDdev, filter, arg2) == WR_ERROR ) {
		return;
	}
	return;
}

static void threadFunc_STD_MainCtrl(void *param)
{
	ST_MASTER *pMaster = (ST_MASTER*) param;
	ST_threadCfg *pST_controlThread;
	ST_threadQueueData		queueData;

	while (!pMaster->pST_MstrCtrlThread)  epicsThreadSleep(.1);

	pST_controlThread = (ST_threadCfg*) pMaster->pST_MstrCtrlThread;
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
static void threadFunc_STD_DevCtrl(void *param)
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
static void threadFunc_STD_Master_Stop(void *param)
{
	ST_MASTER *pSTMaster = (ST_MASTER*) param;
	int cnt_sleep=0;

	while(TRUE) 
	{
	/* epicsEventSignal( pSTMaster->ST_StopEventThread.threadEventId); */
		epicsEventWait( pSTMaster->ST_StopEventThread.threadEventId);
		kfwPrint(0, "%s: got Master Stop event! it will sleep %f sec. \n", pSTMaster->taskName, pSTMaster->ST_Base.dRunPeriod);
/*		epicsThreadSleep(pSTMaster->ST_Base.dRunPeriod); */
		cnt_sleep = 0;
		while(1) {
			epicsThreadSleep(1);
			cnt_sleep++;
			if( cnt_sleep > pSTMaster->ST_Base.dRunPeriod) {
				kfwPrint(0, "%s: time expired!", pSTMaster->taskName);
				break;
			}
			if( !(pSTMaster->StatusAdmin & TASK_IN_PROGRESS) ) {
				kfwPrint(0, "%s: status is not in-progress.", pSTMaster->taskName);
				break;
			}
		}
		
		db_put(pSTMaster->strPvNames[NUM_SYS_RUN], "0");
		epicsThreadSleep(1.0);
		db_put(pSTMaster->strPvNames[NUM_SYS_ARMING], "0");
		epicsThreadSleep(1.0);

		notify_refresh_master_status();
		
	}

	return;
}




static long drvSFW_init_driver(void)
{
	unsigned long long int a;
/*	ST_STD_device *pSTDdev = NULL; */

	if(!pST_Master)  return 0;
	epicsPrintf("Total number of slave device: %d\n", pST_Master->s32DeviceNum );
#if 0
	pSTDdev = (ST_STD_device*) ellFirst(pST_Master->pList_DeviceTask);
	while(pSTDdev) 
	{

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
#endif
#if 1
	if( make_KSTAR_CA_seq() == WR_ERROR )
		epicsPrintf("\n>>> drvSFW_init_driver(): make_KSTAR_CA_seq() ... _SFW \n");
#endif
	epicsAtExit((VOIDFUNCPTREXIT) destroy_STD_AdminTask, NULL);

	a = wf_getCurrentUsec();
	epicsThreadSleep(1.0);
	pST_Master->one_sec_interval = wf_getCurrentUsec() - a;

	epicsPrintf("drvSFW: measured one sec. is %lf msec\n", 1.E-3 * (double)pST_Master->one_sec_interval);
	kfwPrint(0, "S-FrameWork driver support init OK!\n\n");
/*	printf("[SFW] status: 0x%x\n", pST_Master->StatusAdmin); */
	printlog("%s:System On ++++++++++++++++++++++++++++++++++++", pST_Master->taskName);

	return 0;
}

void set_Exit_function()
{
	static int isFirst = 1;
	if( isFirst ) 
	{
		epicsAtExit((VOIDFUNCPTREXIT) destroy_STD_AdminTask, NULL);
		kfwPrint(0, "Registration of epics exit call function ok!\n");
		isFirst = 0;
	} 
	else
		notify_error(ERR_SCN_PNT, "ERROR: setExit called twiced!");
}

static long drvSFW_io_report(int level)
{
	if(level<1) return 0;
	epicsPrintf("  Task name: %s, ptr_addr: 0x%X, status: 0x%x \n",
		    pST_Master->taskName, 
		    *((unsigned int*)pST_Master), 
		    pST_Master->StatusAdmin   );
	if(level>2) {
		epicsPrintf("   measured one sec. is %lf msec\n", 1.E-3 * (double)pST_Master->one_sec_interval);
	}

	if(level>3 ) {
		
	}

	return 0;
}







ST_MASTER* get_master()
{
	return pST_Master;
}

static ST_STD_device* make_STD_deviceCallFunc(const iocshArgBuf *args)
{
	static uint16 boardcnt = 0;
	ST_STD_device* pSTDdev = NULL;
	char task_name[SIZE_TASK_NAME];
	char dev_type[SIZE_TASK_NAME];

	if(args[0].sval) strcpy(dev_type, args[0].sval);
	if(args[1].sval) strcpy(task_name, args[1].sval);

/*	if( (strcmp(dev_type, STR_DEVICE_TYPE_1) != 0)  && 
		(strcmp(dev_type, STR_DEVICE_TYPE_2) != 0) )
	{	
		printf("[SFW] Device type \"%s\" not available!\n", dev_type );
		return pSTDdev;
	}
*/
/*	pSTDdev = (ST_STD_device*) malloc(sizeof(ST_STD_device)); */
	pSTDdev = (ST_STD_device*) calloc(1, sizeof(ST_STD_device)); /* request by swyun 2012. 9. 4 */
	if(NULL == pSTDdev) {
		kfwPrint(0, "\"%s\" memory allocation failed!\n", task_name);
		return pSTDdev;
	}
	pSTDdev->pUser = NULL; /* 2013. 6. 7 */

	strcpy(pSTDdev->devType, dev_type);
	strcpy(pSTDdev->taskName, task_name);
	if(args[2].sval) strcpy(pSTDdev->strArg0, args[2].sval);
	if(args[3].sval) strcpy(pSTDdev->strArg1, args[3].sval);
	if(args[4].sval) strcpy(pSTDdev->strArg2, args[4].sval);
	if(args[5].sval) strcpy(pSTDdev->strArg3, args[5].sval);
	if(args[6].sval) strcpy(pSTDdev->strArg4, args[6].sval);
	if(args[7].sval) strcpy(pSTDdev->strArg5, args[7].sval);
	if(args[8].sval) strcpy(pSTDdev->strArg6, args[8].sval);
	if(args[9].sval) strcpy(pSTDdev->strArg7, args[9].sval);
	if(args[10].sval) strcpy(pSTDdev->strArg8, args[10].sval);
	if(args[11].sval) strcpy(pSTDdev->strArg9, args[11].sval);

	
	pSTDdev->BoardID = boardcnt;
	boardcnt++;
	pST_Master->s32DeviceNum = boardcnt;
	pSTDdev->pST_parentAdmin = pST_Master;
	scanIoInit(&pSTDdev->ioScanPvt_status);
	scanIoInit(&pSTDdev->ioScanPvt_userCall);
	scanIoInit(&pSTDdev->ioScanPvt_userCall1);
	scanIoInit(&pSTDdev->ioScanPvt_userCall2);
	scanIoInit(&pSTDdev->ioScanPvt_userCall3);

	pSTDdev->channel_mask = 0xffffffff;

	/* related for Real time performance   2012. 8. 16 *********/
	pSTDdev->stdCoreCnt = 0;
	pSTDdev->stdDAQCnt = 0;
	pSTDdev->rtClkRate = 1000;
	pSTDdev->rtStepTime = 0.001;
	

	set_STDdev_status_reset(pSTDdev);
	flush_Master_to_STDdev( pSTDdev );
	flush_STDdev_to_MDSfetch(pSTDdev);

	if( mds_copy_master_to_STD(pSTDdev) == WR_ERROR ) {
		free(pSTDdev);
		return pSTDdev;
	}

	
	if(make_STD_Dev_CtrlThread(pSTDdev) == WR_ERROR) {		
		kfwPrint(0, "%s can't create \"pST_stdCtrlThread\" \n", pSTDdev->taskName );
		free(pSTDdev);
		return pSTDdev;
	}


	pSTDdev->pList_Channel = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pSTDdev->pList_Channel) {
		kfwPrint(0, "%s can't create \"pList_Channel\" \n", pSTDdev->taskName );
		free(pSTDdev);		
		return pSTDdev;
	}
	else  ellInit(pSTDdev->pList_Channel);

	return pSTDdev;

}

static ST_threadCfg*  make_STD_Master_CtrlThread()
{
	ST_threadCfg *pST_MstrCtrlThread  = NULL;

	pST_MstrCtrlThread  = (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pST_MstrCtrlThread) return pST_MstrCtrlThread; 

	sprintf(pST_MstrCtrlThread->threadName, "%s_ctrl", pST_Master->taskName); 

	pST_MstrCtrlThread->threadPriority = epicsThreadPriorityLow;
	pST_MstrCtrlThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);  /* epicsThreadStackMedium   */
	pST_MstrCtrlThread->threadFunc      = (EPICSTHREADFUNC) threadFunc_STD_MainCtrl;
	pST_MstrCtrlThread->threadParam     = (void*) pST_Master;

	pST_MstrCtrlThread->threadQueueId   = epicsMessageQueueCreate(1000,sizeof(ST_threadQueueData));
                                                /* Increase queue size to avoid buffer overflow, KKH May 14, 2008 */

	epicsThreadCreate(pST_MstrCtrlThread->threadName,
			  pST_MstrCtrlThread->threadPriority,
			  pST_MstrCtrlThread->threadStackSize,
			  (EPICSTHREADFUNC) pST_MstrCtrlThread->threadFunc,
			  (void*) pST_MstrCtrlThread->threadParam);

	return pST_MstrCtrlThread;
}

int make_STD_Master_Stop_thread()
{
	sprintf(pST_Master->ST_StopEventThread.threadName, "%s_StopEvt", pST_Master->taskName); 
	pST_Master->ST_StopEventThread.threadEventId = epicsEventCreate(epicsEventEmpty);
	if ( ! pST_Master->ST_StopEventThread.threadEventId ) {
		return WR_ERROR;
	}
	
	pST_Master->ST_StopEventThread.threadFunc = (EPICSTHREADFUNC) threadFunc_STD_Master_Stop;

	epicsThreadCreate(pST_Master->ST_StopEventThread.threadName,
			  epicsThreadPriorityLow,
			  epicsThreadGetStackSize(epicsThreadStackSmall),
			  (EPICSTHREADFUNC) pST_Master->ST_StopEventThread.threadFunc,
			  (void*) pST_Master);

	return WR_OK;
}

int make_STD_Dev_CtrlThread(ST_STD_device *pSTDdev)
{
	ST_threadCfg *pST_ctrlThread  = NULL;

	pST_ctrlThread  = (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pST_ctrlThread) return WR_ERROR; 

	sprintf(pST_ctrlThread->threadName, "%s_ctrl", pSTDdev->taskName); 
	pSTDdev->pST_stdCtrlThread = pST_ctrlThread;

	pST_ctrlThread->threadPriority = epicsThreadPriorityLow;
	pST_ctrlThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall); 
	pST_ctrlThread->threadFunc      = (EPICSTHREADFUNC) threadFunc_STD_DevCtrl;
	pST_ctrlThread->threadParam     = (void*) pSTDdev;
	pST_ctrlThread->threadQueueId   = epicsMessageQueueCreate(1000,sizeof(ST_threadQueueData));

	epicsThreadCreate(pST_ctrlThread->threadName,
			  pST_ctrlThread->threadPriority,
			  pST_ctrlThread->threadStackSize,
			  (EPICSTHREADFUNC) pST_ctrlThread->threadFunc,
			  (void*) pST_ctrlThread->threadParam);

	return WR_OK;
}

int make_STD_DAQ_thread(ST_STD_device *pSTDdev)
{
	sprintf(pSTDdev->ST_DAQThread.threadName, "%s_DAQ", pSTDdev->taskName );
	pSTDdev->ST_DAQThread.threadEventId = epicsEventCreate(epicsEventEmpty);
	if ( ! pSTDdev->ST_DAQThread.threadEventId ) {
		return WR_ERROR;
	}
#if 1
	epicsThreadCreate(pSTDdev->ST_DAQThread.threadName, 
			epicsThreadPriorityHigh, /* epicsThreadPriorityHigh, */
			epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadGetStackSize(epicsThreadStackMedium), */ /* epicsThreadStackBig */
			(EPICSTHREADFUNC) pSTDdev->ST_DAQThread.threadFunc, /*pSTDdev->thread_DAQ.pFunc, */
			(void*)pSTDdev);
#endif
#if 0
	epicsThreadCreate(cName, 
				epicsThreadPriorityHigh,
				epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadStackBig */
				pSTDdev->pFunc_STD_DAQ,
				(void*)pSTDdev);
#endif

	return WR_OK;
}

int  make_STD_CatchEnd_thread(ST_STD_device *pSTDdev)
{
	sprintf(pSTDdev->ST_CatchEndThread.threadName, "%s_CatchEnd", pSTDdev->taskName );
	pSTDdev->ST_CatchEndThread.threadEventId = epicsEventCreate(epicsEventEmpty);
	if ( ! pSTDdev->ST_CatchEndThread.threadEventId ) {
		return WR_ERROR;
	}
	epicsThreadCreate(pSTDdev->ST_CatchEndThread.threadName, 
			epicsThreadPriorityMedium,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) pSTDdev->ST_CatchEndThread.threadFunc,
			(void*)pSTDdev);	
	return WR_OK;
}
int make_STD_RT_thread(ST_STD_device *pSTDdev)
{
	sprintf(pSTDdev->ST_RTthread.threadName, "%s_RT", pSTDdev->taskName );
	pSTDdev->ST_RTthread.threadEventId = epicsEventCreate(epicsEventEmpty);
	if ( ! pSTDdev->ST_RTthread.threadEventId ) {
		return WR_ERROR;
	}
	epicsThreadCreate(pSTDdev->ST_RTthread.threadName, 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium), /* epicsThreadGetStackSize(epicsThreadStackMedium), */ /* epicsThreadStackBig */
			(EPICSTHREADFUNC) pSTDdev->ST_RTthread.threadFunc,
			(void*)pSTDdev);

	return WR_OK;
}

int make_STD_RingBuf_thread(ST_STD_device *pSTDdev)
{
	pSTDdev->pList_BufferNode = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pSTDdev->pList_BufferNode) {
		kfwPrint(0, "%s malloc ( pList_BufferNode )  failed. \n", pSTDdev->taskName );
		return WR_ERROR;
	} else 
		ellInit(pSTDdev->pList_BufferNode);
	
	sprintf(pSTDdev->ST_RingBufThread.threadName, "%s_Buf_ctrl", pSTDdev->taskName);

	pSTDdev->ST_RingBufThread.threadPriority = epicsThreadPriorityHigh;
	pSTDdev->ST_RingBufThread.threadStackSize = epicsThreadGetStackSize(epicsThreadStackMedium);  /* epicsThreadStackBig */
/*	pSTDdev->ST_RingBufThread.threadFunc      = (EPICSTHREADFUNC) pSTDdev->pFunc_STD_RingBuf; */
	pSTDdev->ST_RingBufThread.threadParam     = (void*) pSTDdev;
	pSTDdev->ST_RingBufThread.threadQueueId   = epicsMessageQueueCreate(50, pSTDdev->maxMessageSize);

	epicsThreadCreate(pSTDdev->ST_RingBufThread.threadName,
			  pSTDdev->ST_RingBufThread.threadPriority,
			  pSTDdev->ST_RingBufThread.threadStackSize,
			  (EPICSTHREADFUNC) pSTDdev->ST_RingBufThread.threadFunc,
			  (void*) pSTDdev->ST_RingBufThread.threadParam);

	return WR_OK;
}



int admin_spinLock_mds_put_flag( ST_STD_device *pSTDdev )
{
/*	pSTDdev->StatusDev |= TASK_DATA_TRANSFER; */

	while(pST_Master->n8EscapeWhile) 
	{
		if( pST_Master->n8MdsPutFlag == 0){ /* any body use tree put */
			/*epicsMutexLock(pST_Master->lock_mds);*/
			pST_Master->n8MdsPutFlag  = 1 ; /* i'll use write process bye. */
			/*epicsMutexUnlock(pST_Master->lock_mds);*/
			return WR_OK;
		} 
		else 
			epicsPrintf(".%s ", pSTDdev->taskName );
		
		epicsThreadSleep(0.7);
	}
	pST_Master->n8EscapeWhile = 1;

	return WR_ERROR;	
}

void flush_Master_to_STDdev( ST_STD_device *pSTDdev )
{
	int nval;
	pSTDdev->ST_Base.opMode = pST_Master->ST_Base.opMode;
	pSTDdev->ST_Base.shotNumber = pST_Master->ST_Base.shotNumber;
	pSTDdev->ST_Base.fBlipTime = pST_Master->ST_Base.fBlipTime;
	pSTDdev->ST_Base.nSamplingRate = pST_Master->ST_Base.nSamplingRate;
	pSTDdev->ST_Base.fStepTime = pST_Master->ST_Base.fStepTime;
	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
		pSTDdev->ST_Base.dT0[nval] = pST_Master->ST_Base.dT0[nval];
		pSTDdev->ST_Base.dT1[nval] = pST_Master->ST_Base.dT1[nval];
	}
	pSTDdev->ST_Base.dRunPeriod = pST_Master->ST_Base.dRunPeriod;
}
void flush_STbase_to_All_STDdev( ) /* will be delete, don't use this function */
{
	flush_STBase_to_All_STDdev();	
}
void flush_STBase_to_All_STDdev( )
{
	int nval;
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.opMode = pST_Master->ST_Base.opMode;
		pSTDdev->ST_Base.shotNumber = pST_Master->ST_Base.shotNumber;
		pSTDdev->ST_Base.fBlipTime = pST_Master->ST_Base.fBlipTime;
		pSTDdev->ST_Base.nSamplingRate = pST_Master->ST_Base.nSamplingRate;
		pSTDdev->ST_Base.fStepTime = pST_Master->ST_Base.fStepTime;
		for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
			pSTDdev->ST_Base.dT0[nval] = pST_Master->ST_Base.dT0[nval];
			pSTDdev->ST_Base.dT1[nval] = pST_Master->ST_Base.dT1[nval];
		}
		pSTDdev->ST_Base.dRunPeriod = pST_Master->ST_Base.dRunPeriod;
		
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void flush_Blip_to_All_STDdev( )
{
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.fBlipTime = pST_Master->ST_Base.fBlipTime;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void flush_ShotNum_to_All_STDdev( )
{
	ST_STD_device *pSTDdev = get_first_STDdev();
	while( pSTDdev ) {
		pSTDdev->ST_Base.shotNumber = pST_Master->ST_Base.shotNumber;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void flush_STDdev_to_MDSfetch( ST_STD_device *pSTDdev )
{
	int nval;
	pSTDdev->ST_Base_fetch.opMode = pSTDdev->ST_Base.opMode;
	pSTDdev->ST_Base_fetch.shotNumber = pSTDdev->ST_Base.shotNumber;
	pSTDdev->ST_Base_fetch.fBlipTime = pSTDdev->ST_Base.fBlipTime;
	pSTDdev->ST_Base_fetch.nSamplingRate = pSTDdev->ST_Base.nSamplingRate;
	pSTDdev->ST_Base_fetch.fStepTime = pSTDdev->ST_Base.fStepTime;
	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) {
		pSTDdev->ST_Base_fetch.dT0[nval] = pSTDdev->ST_Base.dT0[nval];
		pSTDdev->ST_Base_fetch.dT1[nval] = pSTDdev->ST_Base.dT1[nval];
	}
	pSTDdev->ST_Base_fetch.dRunPeriod = pSTDdev->ST_Base.dRunPeriod;
}

void set_STDdev_status_reset(ST_STD_device *pSTDdev)
{
	pSTDdev->ST_mds.StatusMds = MDS_INIT;	
	pSTDdev->StatusDev = TASK_NOT_INIT;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	pSTDdev->ErrorDev = ERROR_NONE;
}


/******************************************************************************
*
*  FUNCTION: get_STDev_from_name
*  PURPOSE: get standard device node pointer with given name
*  PARAMETERS: 
		taskName->give task name

******************************************************************************/
ST_STD_device* get_STDev_from_name(char *taskName)
{
	ST_STD_device *pSTDdev = NULL;
	if( !pST_Master) {
		kfwPrint(0, "ERROR! Null master pointer!\n");
		return NULL;
	}
	pSTDdev = (ST_STD_device*) ellFirst(pST_Master->pList_DeviceTask);
	while(pSTDdev) {
		if(!strcmp(pSTDdev->taskName, taskName)) return pSTDdev;
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	return pSTDdev;
}


/******************************************************************************
*
*  FUNCTION: get_STDev_from_type
*  PURPOSE: get first matching node with given type
*  PARAMETERS: 
		typeName: given device type

******************************************************************************/
ST_STD_device* get_STDev_from_type(char *typeName)
{
	ST_STD_device *pSTDdev = NULL;
	if( !pST_Master) {
		kfwPrint(0, "ERROR! Null master pointer!\n");
		return NULL;
	}
	pSTDdev = (ST_STD_device*) ellFirst(pST_Master->pList_DeviceTask);
	while(pSTDdev) {
		if(!strcmp(pSTDdev->devType, typeName)) return pSTDdev;
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	return pSTDdev;
}

ST_STD_device* get_first_STDdev()
{
	if( !pST_Master) {
		kfwPrint(0, "ERROR! Null master pointer!\n");
		return NULL;
	}
	return (ST_STD_device*) ellFirst(pST_Master->pList_DeviceTask);
}

ST_STD_channel* get_STCh_from_STDev_chName(ST_STD_device *pSTDdev, char *chanName)
{
	ST_STD_channel *pSTDch = NULL;
	if( !pSTDdev) {
		kfwPrint(0, "ERROR! Null device pointer!\n");
		return NULL;
	}
	pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
	while(pSTDch) {
		if(!strcmp(pSTDch->chName, chanName)) return pSTDch;
		pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
	}
	return pSTDch;
}
ST_STD_channel* get_STCh_from_STDev_chID(ST_STD_device *pSTDdev, unsigned short ch)
{
	ST_STD_channel *pSTDch = NULL;
	if( !pSTDdev) {
		kfwPrint(0, "ERROR! Null device pointer!\n");
		return NULL;
	}
	pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
	while(pSTDch) {
		if(pSTDch->channelId == ch) return pSTDch;
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

ST_STD_channel* get_STCh_from_devName_chID(char *devName, unsigned short ch)
{
	ST_STD_device *pSTDdev = get_STDev_from_name(devName);
	if(!pSTDdev) return NULL;

	return get_STCh_from_STDev_chID(pSTDdev, ch);
}



