/****************************************************************************

Module      : sfwAdminBody.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2010-1-24
  

*****************************************************************************/

#include "aiRecord.h"
#include "aoRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "mbbiRecord.h"

#include "dbAccess.h"  /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwAdminHead.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"
#include "sfwPVs.h"
#include "sfwDBSeq.h"
#include <sys/stat.h>
#include <sys/types.h>


/**********************************
user include files
***********************************/

#include "myMDSplus.h"
#include "drvNI6123.h"

/**********************************
user include files 
***********************************/



typedef struct {
	ST_ADMIN  *ptaskConfig;

	char            arg0[SIZE_STRINGOUT_VAL];
	char            arg1[SIZE_STRINGOUT_VAL];
	char            recordName[SIZE_PV_NAME];	
	int               ind;
} ST_devAdmin_dpvt;


/*******************************************************/
/*******************************************************/

static long devAoAdmin_init_record(aoRecord *precord);
static long devAoAdmin_write_ao(aoRecord *precord);

static long devStringoutAdmin_init_record(stringoutRecord *precord);
static long devStringoutAdmin_write_stringout(stringoutRecord *precord);

static long devStringinAdmin_init_record(stringinRecord *precord);
static long devStringinAdmin_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt);
static long devStringinAdmin_read_stringin(stringinRecord *precord);

static long devAiAdmin_init_record(aiRecord *precord);
static long devAiAdmin_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiAdmin_read_ai(aiRecord *precord);

static long devBoAdmin_init_record(boRecord *precord);
static long devBoAdmin_write_bo(boRecord *precord);

static long devBiAdmin_init_record(biRecord *precord);
static long devBiAdmin_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiAdmin_read_bi(biRecord *precord);

static long devMbbiAdmin_init_record(mbbiRecord *precord);
static long devMbbiAdmin_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
static long devMbbiAdmin_read_mbbi(mbbiRecord *precord);

/*******************************************************/


typedef struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_write;
	DEVSUPFUN	special_linconv;
} BINARYDSET;


BINARYDSET devBoAdmin = { 6,  NULL,  NULL,   devBoAdmin_init_record,  NULL,  devBoAdmin_write_bo,  NULL };
BINARYDSET  devAoAdmin = {	6, NULL, NULL,  devAoAdmin_init_record, NULL, devAoAdmin_write_ao, NULL };
BINARYDSET  devStringoutAdmin = { 6, NULL, NULL, devStringoutAdmin_init_record, NULL, devStringoutAdmin_write_stringout, NULL };
BINARYDSET  devStringinAdmin = { 5, NULL, NULL, devStringinAdmin_init_record, devStringinAdmin_get_ioint_info, devStringinAdmin_read_stringin };
BINARYDSET  devAiAdmin = { 6, NULL, NULL, devAiAdmin_init_record, devAiAdmin_get_ioint_info, devAiAdmin_read_ai, NULL };
BINARYDSET  devBiAdmin = { 5, NULL, NULL, devBiAdmin_init_record, devBiAdmin_get_ioint_info, devBiAdmin_read_bi };
BINARYDSET  devMbbiAdmin = { 5, NULL, NULL, devMbbiAdmin_init_record, devMbbiAdmin_get_ioint_info, devMbbiAdmin_read_mbbi };

epicsExportAddress(dset, devBoAdmin);
epicsExportAddress(dset, devAoAdmin);
epicsExportAddress(dset, devStringoutAdmin);
epicsExportAddress(dset, devStringinAdmin);
epicsExportAddress(dset, devAiAdmin);
epicsExportAddress(dset, devBiAdmin);
epicsExportAddress(dset, devMbbiAdmin);



/*
you can use temporary parameter values and final board setup seperately. 
In this case, you can use bellow PV for true setting before arming. 
*/
static void devAdmin_BO_BOARD_SETUP(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( !(pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE) ) 
	{
		epicsPrintf("ERROR!   System busy! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
		return;
	}

	while(pSTDdev) {
/*  add here user modified code. */

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	notify_refresh_admin_status();
	epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, pAdminCfg->n8EscapeWhile);
}

static void devAdmin_BO_SYS_RESET(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;	
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	ST_NI6123_dev *pNI6123 = NULL;

	if(pParam->setValue) {

		pAdminCfg->ST_Base.opMode = OPMODE_INIT;
		pAdminCfg->n8EscapeWhile = 0;

		while(pSTDdev)
		{
		/*************************************************
		user edit 
		**************************************************/
			if(!strcmp(pSTDdev->devType, "NI6123"))
				{
					pNI6123 = pSTDdev->pUser;
					if(pNI6123->taskHandle != 0)
						{
//Remove TG							DAQmxTaskControl(pNI6123->taskHandle, DAQmx_Val_Task_Abort);
							DAQmxStopTask (pNI6123->taskHandle);
							NI_err_message(DAQmxClearTask(pNI6123->taskHandle));
							pNI6123->taskHandle = NULL;
							if(pNI6123->auto_run_flag == 1)
								{
									epicsEventSignal(pSTDdev->epicsEvent_DAQthread);
									pNI6123->auto_run_flag = 0;
								}
							epicsPrintf("Clear Task %s\n", pSTDdev->taskName);
						}
		
				}
			
			pSTDdev->StatusDev = TASK_NOT_INIT;
			pSTDdev->StatusDev = TASK_SYSTEM_IDLE;

		
		/*************************************************
		user edit 
		**************************************************/
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}

		pAdminCfg->n8EscapeWhile = 1;

		
		DBproc_put("SXR_DC_SUPPLY1_CH1_ON", "0");
		epicsThreadSleep(2);
		DBproc_put("SXR_DC_SUPPLY1_CH2_ON", "0");
		epicsThreadSleep(2);
		DBproc_put("SXR_DC_SUPPLY2_CH1_ON", "0");	
		epicsThreadSleep(2);
		DBproc_put("SXR_DC_SUPPLY2_CH2_ON", "0");
		epicsThreadSleep(2);

		admin_all_taskStatus_reset();
		notify_refresh_admin_status();
		scanIoRequest(pAdminCfg->ioScanPvt_status);
		DBproc_put("SXR_RESET", "0");
		epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, pAdminCfg->n8EscapeWhile);
	}
}

static void devAdmin_BO_DATA_SEND(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	pAdminCfg->StatusAdmin &=  ~TASK_SYSTEM_IDLE;
	pAdminCfg->StatusAdmin |= TASK_DATA_PUT_STORAGE;
	scanIoRequest(pAdminCfg->ioScanPvt_status);	
	while(pSTDdev)
	{
/*  add here user modified code. */

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	pAdminCfg->StatusAdmin &= ~TASK_DATA_PUT_STORAGE;
	pAdminCfg->StatusAdmin |=  TASK_SYSTEM_IDLE;
	
	notify_refresh_admin_status();
	epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, pAdminCfg->cUseAutoSave);
}

static void devAdmin_BO_SYS_ARMING(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
/*	ST_NI6123_dev *pNI6123 = NULL;   */
	char strBuf[24], mkdirbuf[255];
	memset(strBuf, 0, sizeof(strBuf));
	memset(mkdirbuf, 0, sizeof(mkdirbuf));	

	/**********************************************
		user add here modified code. 
	***********************************************/
	if( (int)pParam->setValue == 1 ) /* in case of arming  */
	{
		if( admin_check_Arming_condition()  == WR_ERROR) 
			return;

		/* direct call to sub-device.. for status notify immediately */

		
		DBproc_get("SXR_LTU_T0_SEC", strBuf);
		pAdminCfg->ST_Base.dT0[0] = atof(strBuf);
		epicsPrintf("Admin ST_Base.dT0 : %f\n", pAdminCfg->ST_Base.dT0[0]);

		while(pSTDdev) 
		{
				pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
				pSTDdev->StatusDev |= TASK_ARM_ENABLED;
				pSTDdev->ST_Base.dT0[0]= pAdminCfg->ST_Base.dT0[0];
				pSTDdev->ST_mds_fetch.dT0[0] = pAdminCfg->ST_Base.dT0[0];
				flush_STDdev_to_MDSfetch(pSTDdev);
				flush_STDdev_to_mdsplus(pSTDdev);
			
				pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
			DBproc_put("SXR_DC_SUPPLY1_CH1_ON", "1");
				epicsThreadSleep(2);
			DBproc_put("SXR_DC_SUPPLY1_CH2_ON", "1");
				epicsThreadSleep(2);
			DBproc_put("SXR_DC_SUPPLY2_CH1_ON", "1");	
				epicsThreadSleep(2);
			DBproc_put("SXR_DC_SUPPLY2_CH2_ON", "1");
				epicsThreadSleep(2);
			sprintf(mkdirbuf, "%s%d", STR_LOCAL_DATA_DIR, (int)pAdminCfg->ST_Base.shotNumber);
			mkdir(mkdirbuf,NULL);




	}

	else /* release arming condition */
	{
		if( admin_check_Release_condition()  == WR_ERROR) 
			return;
		
		while(pSTDdev) 
		{
			
			printf("\"%s\" arm disabled.\n", pSTDdev->taskName );
	
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	
			scanIoRequest(pSTDdev->ioScanPvt_status);
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
			DBproc_put("SXR_DC_SUPPLY1_CH1_ON", "0");
				epicsThreadSleep(2);
			DBproc_put("SXR_DC_SUPPLY1_CH2_ON", "0");
				epicsThreadSleep(2);
			DBproc_put("SXR_DC_SUPPLY2_CH1_ON", "0");	
				epicsThreadSleep(2);
			DBproc_put("SXR_DC_SUPPLY2_CH2_ON", "0");
				epicsThreadSleep(2);

	}
	notify_refresh_admin_status();
	epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, (int)pParam->setValue);
}

static void devAdmin_BO_SYS_RUN(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	/**********************************************
		user add here modified code. 
	***********************************************/
	if( (int)pParam->setValue == 1 ) /* command to run  */
	{
		if( admin_check_Run_condition()  == WR_ERROR) 
			return;

		/* direct call to sub-device.. for status notify immediately */
		while(pSTDdev) 
		{
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_DAQ_STARTED;

			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}

		epicsThreadSleep(0.3);
	}
	else 
	{
#if 0	/* do nothing... 'cause aleady done in sub devices */	
		if( admin_check_Stop_condition()  == WR_ERROR) 
			return;

		/* do something, if you need */
		while(pSTDdev) 
		{
			drvACQ196_RUN_stop(pSTDdev);

			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
#endif
	}
	notify_refresh_admin_status();
	epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, (int)pParam->setValue);
}

static void devAdmin_BO_AUTO_SAVE(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;

	pAdminCfg->cUseAutoSave = (int)pParam->setValue;
	epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, pAdminCfg->cUseAutoSave);
}


static void devAdmin_AO_CLOCK(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR!   System armed! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
		return;
	}
	pAdminCfg->ST_Base.nSamplingRate = (uint32)pParam->setValue;
	
	while(pSTDdev) {

  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("AO thread (%d): %s %s (%s)\n", pAdminCfg->ST_Base.nSamplingRate, pAdminCfg->taskName, precord->name,
										 epicsThreadGetNameSelf());
}
static void devAdmin_AO_T0(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR!   System armed! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
		return;
	}
	if( pParam->n32Arg0 >= SIZE_CNT_MULTI_TRIG ) {
		epicsPrintf("ERROR!   Timing section not valid(%d).\n", pParam->n32Arg0);
		return;
	}
	pAdminCfg->ST_Base.dT0[pParam->n32Arg0]= (double)pParam->setValue;
	
	while(pSTDdev) {

  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("T0 (%lf): Sec. %d, %s %s (%s)\n", pAdminCfg->ST_Base.dT0[pParam->n32Arg0], pParam->n32Arg0, 
		pAdminCfg->taskName, precord->name, epicsThreadGetNameSelf());
}
static void devAdmin_AO_T1(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR!   System armed! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
		return;
	}
	if( pParam->n32Arg0 >= SIZE_CNT_MULTI_TRIG ) {
		epicsPrintf("ERROR!   Timing section not valid(%d).\n", pParam->n32Arg0);
		return;
	}
	pAdminCfg->ST_Base.dT1[pParam->n32Arg0]= (double)pParam->setValue;
	
	while(pSTDdev) {

  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("T1 (%lf): Sec. %d, %s %s (%s)\n", pAdminCfg->ST_Base.dT1[pParam->n32Arg0], pParam->n32Arg0, 
		pAdminCfg->taskName, precord->name, epicsThreadGetNameSelf());
}

static void devAdmin_AO_OP_MODE(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
/*	struct dbCommon *precord = pParam->precord; */	
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( !(pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE) ) {
		notify_admin_error_info(1, "\"%s\" not Idle! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return;
	}
	
	pAdminCfg->ST_Base.opMode = (uint16)pParam->setValue;
	switch(pAdminCfg->ST_Base.opMode ) 
	{
		case OPMODE_REMOTE: epicsPrintf("\"%s\" set Remote mode.\n", pAdminCfg->taskName ); break;
		case OPMODE_LOCAL: epicsPrintf("\"%s\" set Local mode.\n", pAdminCfg->taskName ); break;
		case OPMODE_CALIBRATION: epicsPrintf("\"%s\" set Calibration mode.\n", pAdminCfg->taskName ); break;
		case OPMODE_INIT:
			epicsPrintf("\"%s\" set op mode none.\n", pAdminCfg->taskName );
			admin_all_taskStatus_reset();
			break;
		default: 
			epicsPrintf("ERROR!  \"%s\" got invalid value(%d).\n", pAdminCfg->taskName, pAdminCfg->ST_Base.opMode ); 
			break;
	}

	while(pSTDdev)
	{
		pSTDdev->ST_Base.opMode = pAdminCfg->ST_Base.opMode;
		/**********************************************
		user add here modified code. 
		***********************************************/
		mds_assign_from_Env(pSTDdev);

		scanIoRequest(pSTDdev->ioScanPvt_status);
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	notify_refresh_admin_status();

}

static void devAdmin_AO_SET_BLIP(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();	
	struct dbCommon *precord = pParam->precord;

	if( !(pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE) ) {
		epicsPrintf("ERROR!   %s not Idle condition! (0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
		return;
	}
	pAdminCfg->ST_Base.fBlipTime = (float)pParam->setValue;
	flush_Blip_to_All_STDdev();
	epicsPrintf("%s: %s: %f sec\n", pAdminCfg->taskName, precord->name, pAdminCfg->ST_Base.fBlipTime);
		
}

static void devAdmin_AO_SHOT_NUMBER(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("WARNING!   %s armed! but shot number changed!(0x%x)\n", pAdminCfg->taskName, pAdminCfg->StatusAdmin);
	}
	pAdminCfg->ST_Base.shotNumber = (uint32)pParam->setValue; 
	pSTDdev->ST_Base.shotNumber = (uint32)pParam->setValue;  /* TGLee  */

	flush_ShotNum_to_All_STDdev();
	
	epicsPrintf("AO thread (%lu): %s %s (%s)\n", pAdminCfg->ST_Base.shotNumber, pAdminCfg->taskName, precord->name,
										 epicsThreadGetNameSelf());
}

static void devAdmin_AO_CREATE_LOCAL_SHOT(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();	
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);

	if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR!   System armed! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
		return;
	}
//TG 2012.0906	pAdminCfg->ST_Base.shotNumber = (uint32)pParam->setValue;
	
	while(pSTDdev) {
		pSTDdev->ST_Base.shotNumber = pAdminCfg->ST_Base.shotNumber;
		
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	pSTDdev = get_first_STDdev();
	if( pSTDdev ) {
		mds_Connect(pSTDdev);
		mds_createNewShot( pSTDdev );
		mds_Disconnect( pSTDdev );
		epicsPrintf("%s: %s (%d) \n", pAdminCfg->taskName, precord->name,  (int)pParam->setValue);
	}

	pSTDdev = get_STDev_from_name("B4");
	if( pSTDdev ) 
	{
		mds_Connect( pSTDdev );
		mds_createNewShot( pSTDdev );
		mds_Disconnect(pSTDdev );
		epicsPrintf("%s: %s (%d) \n", pSTDdev->taskName, precord->name,  (int)pParam->setValue);
	}

}


static long devBoAdmin_init_record(boRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;
	
	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s", pdevAdmin_dpvt->arg0);
#if PRINT_DEV_SUPPORT_ARG
			printf("devBoAdmin arg num: %d: %s\n", nval,  pdevAdmin_dpvt->arg0);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devBoAdmin (init_record) Illegal OUT field: task_name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devBoAdmin (init_record) Illegal OUT field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(pdevAdmin_dpvt->arg0, BO_AUTO_SAVE_STR)) {
		pdevAdmin_dpvt->ind = BO_AUTO_SAVE;
	} 
	else if(!strcmp(pdevAdmin_dpvt->arg0, BO_SYS_ARMING_STR)) {
		pdevAdmin_dpvt->ind = BO_SYS_ARMING;
	}
	else if(!strcmp(pdevAdmin_dpvt->arg0, BO_SYS_RUN_STR)) {
		pdevAdmin_dpvt->ind = BO_SYS_RUN;
	} 
	else if(!strcmp(pdevAdmin_dpvt->arg0, BO_DATA_SEND_STR)) {
		pdevAdmin_dpvt->ind = BO_DATA_SEND;
	} 
	else if(!strcmp(pdevAdmin_dpvt->arg0, BO_SYS_RESET_STR )) {
		pdevAdmin_dpvt->ind = BO_SYS_RESET;
	} 
	else if(!strcmp(pdevAdmin_dpvt->arg0, BO_BOARD_SETUP_STR )) {
		pdevAdmin_dpvt->ind = BO_BOARD_SETUP;
	} 

	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devBoAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoAdmin_write_bo(boRecord *precord)
{
	ST_devAdmin_dpvt		*pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN   *pAdminCfg;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pdevAdmin_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pAdminCfg			= pdevAdmin_dpvt->ptaskConfig;
	pControlThreadConfig		= pAdminCfg->pST_adminCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;


        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{
		precord->pact = TRUE;
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pdevAdmin_dpvt->ind) {
			case BO_AUTO_SAVE:
				qData.pFunc = devAdmin_BO_AUTO_SAVE;
				break;
			case BO_SYS_ARMING:
				qData.pFunc = devAdmin_BO_SYS_ARMING;
				break;
			case BO_SYS_RUN:
				qData.pFunc = devAdmin_BO_SYS_RUN;
				break;
			case BO_DATA_SEND:
				qData.pFunc = devAdmin_BO_DATA_SEND;
				break;
			case BO_SYS_RESET:
				qData.pFunc = devAdmin_BO_SYS_RESET;
				break;
			case BO_BOARD_SETUP:
				qData.pFunc = devAdmin_BO_BOARD_SETUP;
				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pdevAdmin_dpvt->ind) {
			case BO_SYS_ARMING: 
				if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED )  precord->val = 1;
				else  precord->val = 0;
				break;
			case BO_SYS_RUN: 
				if( pAdminCfg->StatusAdmin & TASK_DAQ_STARTED)  precord->val = 1;
				else  precord->val = 0;
				break;
			case BO_DATA_SEND:
				if( pAdminCfg->StatusAdmin & TASK_DATA_PUT_STORAGE)  precord->val = 1;
				else  precord->val = 0;
				break;
			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devAoAdmin_init_record(aoRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;

	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s", pdevAdmin_dpvt->arg0, pdevAdmin_dpvt->arg1);
#if PRINT_DEV_SUPPORT_ARG
			printf("devAoAdmin arg num: %d: %s %s\n", nval,  pdevAdmin_dpvt->arg0, pdevAdmin_dpvt->arg1);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoAdmin (init_record) Illegal OUT field: task_name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoAdmin (init_record) Illegal OUT field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if (!strcmp(pdevAdmin_dpvt->arg0, AO_SHOT_NUMBER_STR)) {
		pdevAdmin_dpvt->ind = AO_SHOT_NUMBER;
	} else if (!strcmp(pdevAdmin_dpvt->arg0, AO_OP_MODE_STR)) {
		pdevAdmin_dpvt->ind = AO_OP_MODE;
	}

	else if (!strcmp(pdevAdmin_dpvt->arg0, AO_SET_BLIP_STR)) {
		pdevAdmin_dpvt->ind = AO_SET_BLIP;
	} else if (!strcmp(pdevAdmin_dpvt->arg0, AO_CREATE_LOCAL_SHOT_STR)) {
		pdevAdmin_dpvt->ind = AO_CREATE_LOCAL_SHOT;
	}
	else if (!strcmp(pdevAdmin_dpvt->arg0, AO_SAMPLING_RATE_STR)) {
		pdevAdmin_dpvt->ind = AO_SAMPLING_RATE;
	}
	else if (!strcmp(pdevAdmin_dpvt->arg0, AO_START_T0_STR)) {
		pdevAdmin_dpvt->ind = AO_START_T0;
	}
	else if (!strcmp(pdevAdmin_dpvt->arg0, AO_STOP_T1_STR)) {
		pdevAdmin_dpvt->ind = AO_STOP_T1;
	}

	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devAoAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
		
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoAdmin_write_ao(aoRecord *precord)
{
	ST_devAdmin_dpvt        *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN          *pAdminCfg;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pdevAdmin_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pAdminCfg			= pdevAdmin_dpvt->ptaskConfig;
	pControlThreadConfig		= pAdminCfg->pST_adminCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pdevAdmin_dpvt->ind) 
		{
			case AO_SHOT_NUMBER:
				qData.pFunc = devAdmin_AO_SHOT_NUMBER;
				break;
			case AO_OP_MODE:
				qData.pFunc = devAdmin_AO_OP_MODE;
				break;
			case AO_SET_BLIP:
				qData.pFunc = devAdmin_AO_SET_BLIP;
				break;
			case AO_CREATE_LOCAL_SHOT:
				qData.pFunc = devAdmin_AO_CREATE_LOCAL_SHOT;
				break;
			case AO_SAMPLING_RATE:
				qData.pFunc = devAdmin_AO_CLOCK;
				break;
			case AO_START_T0:
				qData.pFunc = devAdmin_AO_T0;
				qData.param.n32Arg0 = strtoul(pdevAdmin_dpvt->arg1, NULL, 0);
				break;
			case AO_STOP_T1:
				qData.pFunc = devAdmin_AO_T1;
				qData.param.n32Arg0 = strtoul(pdevAdmin_dpvt->arg1, NULL, 0);
				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*(0)=>(success ) */
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		
		switch(pdevAdmin_dpvt->ind) 
		{
			case AO_OP_MODE: precord->val = pAdminCfg->ST_Base.opMode; break;

			default: break;
		}
		return 0; /*(0)=>(success ) */
	}

	return -1;
}

static long devStringoutAdmin_init_record(stringoutRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;

	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s", pdevAdmin_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devStringoutAdmin arg num: %d: %s\n",nval, pdevAdmin_dpvt->arg0);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) 
			{
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutAdmin (init_record) Illegal OUT field: task_name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutAdmin (init_record) Illegal OUT field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevAdmin_dpvt->arg0, "xxx")) {
		pdevAdmin_dpvt->ind = 1;
	}  
	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devStringoutAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}


static long devStringoutAdmin_write_stringout(stringoutRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN          *pAdminCfg;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pdevAdmin_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}

	pAdminCfg			= pdevAdmin_dpvt->ptaskConfig;
	pControlThreadConfig		= pAdminCfg->pST_adminCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	strcpy( qData.param.setStr, precord->val );

 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif			
		switch(pdevAdmin_dpvt->ind) 
		{
			case 0:

				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId, (void*) &qData, sizeof(ST_threadQueueData));

		return 0; /*(-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1; /*(-1,0)=>(failure,success)*/
}

static long devStringinAdmin_init_record(stringinRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;

	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s", pdevAdmin_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devStringinAdmin arg num: %d: %s\n",nval, pdevAdmin_dpvt->arg0);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) 
			{
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringinAdmin (init_record) Illegal INP field: task_name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringinAdmin (init_record) Illegal INP field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevAdmin_dpvt->arg0, STRINGIN_ERROR_STRING_STR)) {
		pdevAdmin_dpvt->ind = STRINGIN_ERROR_STRING;
	}  
	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devStringinAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;
	
	return 0;    /*returns: (-1,0)=>(failure,success)*/
}

static long devStringinAdmin_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN   *pAdminCfg;
	if(!pdevAdmin_dpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;
	*ioScanPvt  = pAdminCfg->ioScanPvt_status;
	return 0;
}
static long devStringinAdmin_read_stringin(stringinRecord *precord)
{ 
	ST_devAdmin_dpvt	*pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN		*pAdminCfg;

	if(!pdevAdmin_dpvt) return -1;

	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;

	switch(pdevAdmin_dpvt->ind) 
	{
		case STRINGIN_ERROR_STRING:
			strcpy( precord->val, pAdminCfg->ErrorString);
/*			epicsPrintf("%s Error Info: %s \n", pAdminCfg->taskName, precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pAdminCfg->taskName, pdevAdmin_dpvt->ind); 
			break;
	}
	return (0); /*returns: (-1,0)=>(failure,success)*/
}


static long devAiAdmin_init_record(aiRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s", pdevAdmin_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devAiAdmin arg num: %d: %s\n", nval, pdevAdmin_dpvt->arg0);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiAdmin (init_record) Illegal INP field: task name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiAdmin (init_record) Illegal INP field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pdevAdmin_dpvt->arg0, "xxx"))
		pdevAdmin_dpvt->ind = 1;	
	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devAiAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiAdmin_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN   *pAdminCfg;

	if(!pdevAdmin_dpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;
	*ioScanPvt  = pAdminCfg->ioScanPvt_status;
	return 0;
}

static long devAiAdmin_read_ai(aiRecord *precord)
{ 
	ST_devAdmin_dpvt	*pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN		*pAdminCfg;

	if(!pdevAdmin_dpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;

	switch(pdevAdmin_dpvt->ind) 
	{
		case 0:
/*			precord->val = pAdminCfg->StatusAdmin; */
/*			epicsPrintf("%s StatusDev: 0x%X \n", pAdminCfg->taskName, (int)precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pAdminCfg->taskName, pdevAdmin_dpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}

static long devBiAdmin_init_record(biRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s", pdevAdmin_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devBiAdmin arg num: %d: %s\n", nval, pdevAdmin_dpvt->arg0);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBiAdmin (init_record) Illegal INP field: task name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiAdmin (init_record) Illegal INP field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pdevAdmin_dpvt->arg0, "xxx"))
		pdevAdmin_dpvt->ind = 1;
	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devBiAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiAdmin_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN   *pAdminCfg;

	if(!pdevAdmin_dpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;
	*ioScanPvt  = pAdminCfg->ioScanPvt_status;
	return 0;
}

static long devBiAdmin_read_bi(biRecord *precord)
{ 
	ST_devAdmin_dpvt	*pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN		*pAdminCfg;

	if(!pdevAdmin_dpvt) return -1;

	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;

	switch(pdevAdmin_dpvt->ind) 
	{
		case 1:
			precord->val = 0;
/*			epicsPrintf("%s StatusDev: 0x%X \n", pAdminCfg->taskName, (int)precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pAdminCfg->taskName, pdevAdmin_dpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert, don't convert)*/
}

static long devMbbiAdmin_init_record(mbbiRecord *precord)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) malloc(sizeof(ST_devAdmin_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pdevAdmin_dpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s", pdevAdmin_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devMbbiAdmin arg num: %d: %s\n", nval, pdevAdmin_dpvt->arg0);
#endif
			pdevAdmin_dpvt->ptaskConfig = drvAdmin_get_AdminPtr();
			if(!pdevAdmin_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devMbbiAdmin (init_record) Illegal INP field: task name");
				free(pdevAdmin_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devMbbiAdmin (init_record) Illegal INP field");
			free(pdevAdmin_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pdevAdmin_dpvt->arg0, MBBI_ADMIN_STATUS_STR))
		pdevAdmin_dpvt->ind = MBBI_ADMIN_STATUS;
	else if (!strcmp(pdevAdmin_dpvt->arg0, MBBI_ADMIN_ERROR_STR))
		pdevAdmin_dpvt->ind = MBBI_ADMIN_ERROR;
	else {
		pdevAdmin_dpvt->ind = -1;
		epicsPrintf("ERROR! devMbbiAdmin_init_record: arg0 \"%s\" \n",  pdevAdmin_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAdmin_dpvt;

	return 0;   /*returns: (-1,0)=>(failure,success)*/
}


static long devMbbiAdmin_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_devAdmin_dpvt *pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN   *pAdminCfg;

	if(!pdevAdmin_dpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;
	*ioScanPvt  = pAdminCfg->ioScanPvt_status;
	return 0;
}

static long devMbbiAdmin_read_mbbi(mbbiRecord *precord)
{ 
	ST_devAdmin_dpvt	*pdevAdmin_dpvt = (ST_devAdmin_dpvt*) precord->dpvt;
	ST_ADMIN		*pAdminCfg;

	if(!pdevAdmin_dpvt) return -1;

	pAdminCfg = pdevAdmin_dpvt->ptaskConfig;

	switch(pdevAdmin_dpvt->ind) 
	{
		case MBBI_ADMIN_STATUS:
			precord->rval = pAdminCfg->StatusAdmin;

			if( pAdminCfg->StatusAdmin & TASK_SYSTEM_READY ) 
			{
				if( pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE ) precord->val = 1; /* ready to run */
				else if( pAdminCfg->StatusAdmin & TASK_DATA_PUT_STORAGE ) precord->val = 5;
				else if( pAdminCfg->StatusAdmin & TASK_AFTER_SHOT_PROCESS ) precord->val = 4;
				else if( pAdminCfg->StatusAdmin & TASK_DAQ_STARTED ) precord->val = 3;
				else if( pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) precord->val = 2;
				else precord->val = 7; /* ready but busy with mystery */
			} else {
				if( pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE ) precord->val = 6; /* not ready, Idle */
				else precord->val = 0; /* not initiated or NULL */
			} 
/*			epicsPrintf("%s Status: 0x%X, rval: %d, val: %d \n", pAdminCfg->taskName, pAdminCfg->StatusAdmin, precord->rval, precord->val);  */
			break;
		case MBBI_ADMIN_ERROR:
			precord->val = pAdminCfg->ErrorAdmin; 
			epicsPrintf("%s Error: 0x%X, %d \n", pAdminCfg->taskName, (unsigned short)precord->val, (unsigned short)precord->val);
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pAdminCfg->taskName, pdevAdmin_dpvt->ind); 
			break;
	}
	return (2); /*(0,2)=>(success, success no convert)*/
}


