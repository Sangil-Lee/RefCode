/****************************************************************************

Module      : sfwDevice.c

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

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"


#define PRINT_SFW_PHASE_INFO         0
#define PRINT_SFW_DEVSUP_ARG    0



/******************************************************/
#define MBBI_ADMIN_STATUS       1
#define MBBI_ADMIN_STATUS_STR   "status"
#define MBBI_ADMIN_ERROR        2
#define MBBI_ADMIN_ERROR_STR    "error"
/*------------------------------------------*/
#define BO_AUTO_SAVE        1
#define BO_SYS_ARMING       2
#define BO_SYS_RUN          3
#define BO_DATA_SEND        4
#define BO_SYS_RESET        5
#define BO_BOARD_SETUP      6
#define BO_AUTO_CREATE      7
#define BO_ERROR_CLEAR      8
#define BO_ENABLE_IP_MIN      9




#define BO_AUTO_SAVE_STR      "auto_save"
#define BO_SYS_ARMING_STR     "arm_enable"
#define BO_SYS_RUN_STR        "sys_run"
#define BO_DATA_SEND_STR      "snd_data"
#define BO_SYS_RESET_STR      "sys_reset"
#define BO_BOARD_SETUP_STR    "hw_setup"
#define BO_AUTO_CREATE_STR      "auto_create"
#define BO_ERROR_CLEAR_STR      "err_clear"
#define BO_ENABLE_IP_MIN_STR      "ip_min"




/*------------------------------------------*/
/*
#define AI_SYS_STATUS		1
#define AI_SYS_STATUS_STR		"status"
*/
/*------------------------------------------*/
#define  AO_OP_MODE                1
#define  AO_CREATE_LOCAL_TREE      3
#define  AO_SHOT_NUMBER            4
#define  AO_SAMPLING_RATE          5
#define  AO_START_T0               6
#define  AO_STOP_T1                7
#define  AO_CHANNEL_MASK                8
#define  AO_RUN_PERIOD                9
#define  AO_TEST_PUT                10
#define  AO_RT_CLOCK_RATE                11
#define  AO_PRINT_LEVEL                12
#define  AO_START_TIME               13
#define  AO_STOP_TIME                14



/*------------------------------*/
#define  AO_OP_MODE_STR            "set_mode"
#define  AO_CREATE_LOCAL_TREE_STR  "create_shot"
#define  AO_SHOT_NUMBER_STR        "shot_num"
#define  AO_SAMPLING_RATE_STR      "sample_rate"
#define  AO_START_T0_STR           "start_t0"
#define  AO_STOP_T1_STR            "stop_t1"
#define  AO_CHANNEL_MASK_STR                "ch_mask"
#define  AO_RUN_PERIOD_STR                "run_period"
#define  AO_TEST_PUT_STR                "test_put"
#define  AO_RT_CLOCK_RATE_STR                "rt_rate"
#define  AO_PRINT_LEVEL_STR                "print_lvl"
#define  AO_START_TIME_STR           "start_time"
#define  AO_STOP_TIME_STR            "stop_time"




/*****-----------------------------------------*/
#define STRINGIN_ERROR_STRING        1
#define STRINGIN_ERROR_STRING_STR    "error_str"


/*******************************************************/
/*******************************************************/

static long devAoSFW_init_record(aoRecord *precord);
static long devAoSFW_write_ao(aoRecord *precord);

static long devStringoutSFW_init_record(stringoutRecord *precord);
static long devStringoutSFW_write_stringout(stringoutRecord *precord);

static long devStringinSFW_init_record(stringinRecord *precord);
static long devStringinSFW_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt);
static long devStringinSFW_read_stringin(stringinRecord *precord);

static long devAiSFW_init_record(aiRecord *precord);
static long devAiSFW_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiSFW_read_ai(aiRecord *precord);

static long devBoSFW_init_record(boRecord *precord);
static long devBoSFW_write_bo(boRecord *precord);

static long devBiSFW_init_record(biRecord *precord);
static long devBiSFW_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiSFW_read_bi(biRecord *precord);

static long devMbbiSFW_init_record(mbbiRecord *precord);
static long devMbbiSFW_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
static long devMbbiSFW_read_mbbi(mbbiRecord *precord);

/*******************************************************/

BINARYDSET devBoSFW = { 6,  NULL,  NULL,   devBoSFW_init_record,  NULL,  devBoSFW_write_bo,  NULL };
BINARYDSET  devAoSFW = {	6, NULL, NULL,  devAoSFW_init_record, NULL, devAoSFW_write_ao, NULL };
BINARYDSET  devStringoutSFW = { 6, NULL, NULL, devStringoutSFW_init_record, NULL, devStringoutSFW_write_stringout, NULL };
BINARYDSET  devStringinSFW = { 5, NULL, NULL, devStringinSFW_init_record, devStringinSFW_get_ioint_info, devStringinSFW_read_stringin };
BINARYDSET  devAiSFW = { 6, NULL, NULL, devAiSFW_init_record, devAiSFW_get_ioint_info, devAiSFW_read_ai, NULL };
BINARYDSET  devBiSFW = { 5, NULL, NULL, devBiSFW_init_record, devBiSFW_get_ioint_info, devBiSFW_read_bi };
BINARYDSET  devMbbiSFW = { 5, NULL, NULL, devMbbiSFW_init_record, devMbbiSFW_get_ioint_info, devMbbiSFW_read_mbbi };

epicsExportAddress(dset, devBoSFW);
epicsExportAddress(dset, devAoSFW);
epicsExportAddress(dset, devStringoutSFW);
epicsExportAddress(dset, devStringinSFW);
epicsExportAddress(dset, devAiSFW);
epicsExportAddress(dset, devBiSFW);
epicsExportAddress(dset, devMbbiSFW);



static void dev_AO_OP_MODE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = NULL;
	unsigned short  prev_opmode;

	if( !(pMaster->StatusAdmin & TASK_SYSTEM_IDLE) ) {
		notify_error(ERR_SCN_PNT, "%s not Idle! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	
	prev_opmode = pMaster->ST_Base.opMode;
	pMaster->ST_Base.opMode = (uint16)pParam->setValue;
	switch(pMaster->ST_Base.opMode ) 
	{
		case OPMODE_REMOTE: kfwPrint(0, "\"%s\" set Remote mode.\n", pMaster->taskName ); break;
		case OPMODE_LOCAL: kfwPrint(0, "\"%s\" set Local mode.\n", pMaster->taskName ); break;
		case OPMODE_CALIBRATION: kfwPrint(0, "\"%s\" set Calibration mode.\n", pMaster->taskName ); break;
		case OPMODE_INIT:
			kfwPrint(0, "\"%s\" set op mode none.\n", pMaster->taskName );
			admin_all_taskStatus_reset();
			break;
		default: 
			kfwPrint(0, "\"%s\" got invalid value(%d). return to '%d'\n", pMaster->taskName, pMaster->ST_Base.opMode, prev_opmode); 
			pMaster->ST_Base.opMode = prev_opmode;
			break;
	}
	if( OPMODE_INIT < pMaster->ST_Base.opMode &&  pMaster->ST_Base.opMode < CNT_OPMODE )
		pMaster->StatusAdmin |= TASK_STANDBY;

	if( pMaster->ST_Func._OP_MODE )
		(pMaster->ST_Func._OP_MODE)(pMaster, pParam->setValue, 0);

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.opMode = pMaster->ST_Base.opMode;

		if( OPMODE_INIT < pSTDdev->ST_Base.opMode &&  pSTDdev->ST_Base.opMode < CNT_OPMODE )
			pSTDdev->StatusDev|= TASK_STANDBY;
		
		if( (pSTDdev->ST_Func._OP_MODE) ) {
/*			printf("call '_OP_MODE': %s, %.2f\n", pSTDdev->taskName, pParam->setValue); */
			(pSTDdev->ST_Func._OP_MODE)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
}

static void dev_BO_BOARD_SETUP(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( !(pMaster->StatusAdmin & TASK_SYSTEM_IDLE) ) 
	{
		notify_error(ERR_SCN_PNT, "%s busy! check again. 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}

	/* 2012. 3. 6 add external call function in order to run as a controller */
	if( pMaster->ST_Func._BOARD_SETUP ) {
/*		printf("Mtr call '_BOARD_SETUP': %s, %.2f\n", pSTDdev->taskName, pParam->setValue); */
		(pMaster->ST_Func._BOARD_SETUP)(pMaster, pParam->setValue, 0);
	}
	/*  to here */
	
	while(pSTDdev) 
	{
		if( (pSTDdev->ST_Func._BOARD_SETUP) ) {
/*			printf("call '_BOARD_SETUP': %s, %.2f\n", pSTDdev->taskName, pParam->setValue); */
			(pSTDdev->ST_Func._BOARD_SETUP)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void dev_BO_SYS_ARMING(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( !(pMaster->StatusAdmin & TASK_STANDBY) ) {
		notify_error(ERR_SCN_PNT, "%s not standby! 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ) {
		notify_error(ERR_SCN_PNT, "%s waiting trigger! 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	if( pMaster->StatusAdmin & TASK_IN_PROGRESS ) 
	{
		notify_error(ERR_SCN_PNT, "%s is in-progress! 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	
	if( pMaster->ST_Func._SYS_ARMING )
		(pMaster->ST_Func._SYS_ARMING)(pMaster, pParam->setValue, 0);
	
	while(pSTDdev) 
	{
		if( (pSTDdev->ST_Func._SYS_ARMING) ) {
			(pSTDdev->ST_Func._SYS_ARMING)(pSTDdev, pParam->setValue, 0);
		}
		
		/* 2012. 5. 31 for auto save */
#if 0		
		if( (pParam->setValue == 0)  && (pSTDdev->StatusDev & TASK_RUN_TRIGGERED) && get_status_auto_save() ) 
		{
			pSTDdev->StatusDev &= ~TASK_RUN_TRIGGERED;
			
			if( (pSTDdev->ST_Func._DATA_SEND) )
				(pSTDdev->ST_Func._DATA_SEND)(pSTDdev, 1, 0);
		}
#endif
		flush_STDdev_to_MDSfetch( pSTDdev );  /* execute in SFW. 2013. 6. 19 */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void dev_BO_SYS_RUN(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( !(pMaster->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		notify_error(ERR_SCN_PNT, "%s requires arming! 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	
	if( pMaster->ST_Func._SYS_RUN )
		(pMaster->ST_Func._SYS_RUN)(pMaster, pParam->setValue, 0);
	
	while(pSTDdev) 
	{
		if( (pSTDdev->ST_Func._SYS_RUN) ) {
			(pSTDdev->ST_Func._SYS_RUN)(pSTDdev, pParam->setValue, 0);
		}
		
		if( pParam->setValue ) /* 2012. 5. 31 for auto save */
			pSTDdev->StatusDev |= TASK_RUN_TRIGGERED;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void dev_BO_DATA_SEND(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	pMaster->StatusAdmin &=  ~TASK_SYSTEM_IDLE;
	pMaster->StatusAdmin |= TASK_DATA_TRANSFER;
	scanIoRequest(pMaster->ioScanPvt_status);

	if( pMaster->ST_Func._DATA_SEND )
		(pMaster->ST_Func._DATA_SEND)(pMaster, pParam->setValue, 0);

	while(pSTDdev) 
	{
		if( (pSTDdev->ST_Func._DATA_SEND) ) {
			(pSTDdev->ST_Func._DATA_SEND)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	pMaster->StatusAdmin &= ~TASK_DATA_TRANSFER;
	pMaster->StatusAdmin |=  TASK_SYSTEM_IDLE;

}

static void dev_AO_SAMPLING_RATE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	
	pMaster->ST_Base.nSamplingRate = (uint32)pParam->setValue;
	pMaster->ST_Base.fStepTime = 1.0 / (float)pMaster->ST_Base.nSamplingRate;
/*
	if( pMaster->ST_Func._SAMPLING_RATE )
		(pMaster->ST_Func._SAMPLING_RATE)(pMaster, pParam->setValue, 0);
*/
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.nSamplingRate = (uint32)pParam->setValue; /* 2013. 6. 19 */

		if( pSTDdev->ST_Base.nSamplingRate != 0) /* 2013. 6. 19 */
			pSTDdev->ST_Base.fStepTime = 1.0 /(float)pSTDdev->ST_Base.nSamplingRate;

		if( (pSTDdev->ST_Func._SAMPLING_RATE) ) {
			(pSTDdev->ST_Func._SAMPLING_RATE)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}





/******************************************************************************
*
*  FUNCTION:   dev_AO_T0
*
*  PURPOSE:    

*  PARAMETERS: 
		pParam->setValue: DAQ start time
		pParam->n32Arg0: muti-trigger session (0,1,2)

*  RETURNS:
*  GLOBALS:
*
******************************************************************************/
static void dev_AO_T0(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	if( pParam->n32Arg0 >= SIZE_CNT_MULTI_TRIG ) {
		notify_error(ERR_SCN_PNT, "%s timing section not valid(%d).\n", pMaster->taskName, pParam->n32Arg0);
		return;
	}
	 
	pMaster->ST_Base.dT0[pParam->n32Arg0]= (double)pParam->setValue;
/*
	if( pMaster->ST_Func._SYS_T0 )
		(pMaster->ST_Func._SYS_T0)(pMaster, pParam->setValue, 0);
*/
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.dT0[pParam->n32Arg0] = pMaster->ST_Base.dT0[pParam->n32Arg0];
		
		if( (pSTDdev->ST_Func._SYS_T0) ) {
			(pSTDdev->ST_Func._SYS_T0)(pSTDdev, pParam->setValue, (double)pParam->n32Arg0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

static void dev_AO_T1(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	if( pParam->n32Arg0 >= SIZE_CNT_MULTI_TRIG ) {
		notify_error(ERR_SCN_PNT, "%s timing section not valid(%d).\n", pMaster->taskName, pParam->n32Arg0);
		return;
	}
	 
	pMaster->ST_Base.dT1[pParam->n32Arg0]= (double)pParam->setValue;
/*
	if( pMaster->ST_Func._SYS_T1 )
		(pMaster->ST_Func._SYS_T1)(pMaster, pParam->setValue, 0);
*/
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.dT1[pParam->n32Arg0] = pMaster->ST_Base.dT1[pParam->n32Arg0];
		
		if( (pSTDdev->ST_Func._SYS_T1) ){
			(pSTDdev->ST_Func._SYS_T1)(pSTDdev, pParam->setValue, (double)pParam->n32Arg0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
}

static void dev_AO_START_TIME(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	if( pParam->n32Arg0 >= SIZE_CNT_MULTI_TRIG ) {
		notify_error(ERR_SCN_PNT, "%s timing section not valid(%d).\n", pMaster->taskName, pParam->n32Arg0);
		return;
	}
	 
	pMaster->ST_Base.dT0[pParam->n32Arg0]= (double)pParam->setValue;
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.dT0[pParam->n32Arg0] = pMaster->ST_Base.dT0[pParam->n32Arg0];
		
		if( (pSTDdev->ST_Func._SYS_T0) ) {
			(pSTDdev->ST_Func._SYS_T0)(pSTDdev, pParam->setValue, (double)pParam->n32Arg0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

static void dev_AO_STOP_TIME(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n", pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	if( pParam->n32Arg0 >= SIZE_CNT_MULTI_TRIG ) {
		notify_error(ERR_SCN_PNT, "%s timing section not valid(%d).\n", pMaster->taskName, pParam->n32Arg0);
		return;
	}
	 
	pMaster->ST_Base.dT1[pParam->n32Arg0]= (double)pParam->setValue;
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.dT1[pParam->n32Arg0] = pMaster->ST_Base.dT1[pParam->n32Arg0];
		
		if( (pSTDdev->ST_Func._SYS_T1) ){
			(pSTDdev->ST_Func._SYS_T1)(pSTDdev, pParam->setValue, (double)pParam->n32Arg0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
}

static void dev_AO_SHOT_NUMBER(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "WR!shot changed!but %s armed!\n", pMaster->taskName);
	}
	
	pMaster->ST_Base.shotNumber = (uint32)pParam->setValue;

	flush_ShotNum_to_All_STDdev();

	while(pSTDdev) 
	{
		if( (pSTDdev->ST_Func._SHOT_NUMBER) ){
/*			printf("call '_SHOT_NUMBER': %s, %lu\n", pSTDdev->taskName, pSTDdev->ST_Base.shotNumber); */
			(pSTDdev->ST_Func._SHOT_NUMBER)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

static void dev_AO_CREATE_LOCAL_TREE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();	
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n",pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	pMaster->ST_Base.shotNumber = (uint32)pParam->setValue;
	if( mds_Connect_master(pMaster)==WR_ERROR ) {
		notify_error(ERR_SCN_PNT, "mds_Connect_master()... error.\n");
	}
	if( mds_createNewShot_master( pMaster )==WR_ERROR) {
		notify_error(ERR_SCN_PNT, "mds_createNewShot_master()... error.\n");
	}
	mds_Disconnect_master( pMaster );
/* it will be deleted soon. don't need perform in slave device  */
	while(pSTDdev) 
	{
		pSTDdev->ST_Base.shotNumber = pMaster->ST_Base.shotNumber;
		
		if( (pSTDdev->ST_Func._CREATE_LOCAL_TREE) ){
/*			printf("call '_CREATE_LOCAL_TREE': %s, %lu\n", pSTDdev->taskName, pSTDdev->ST_Base.shotNumber); */
			(pSTDdev->ST_Func._CREATE_LOCAL_TREE)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

static void dev_AO_TEST_PUT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();	
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	/* you can do this any time ... */
/*	
	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(1, "%s armed! check again. 0x%x\n",pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
*/
	while(pSTDdev) 
	{
		
		if( (pSTDdev->ST_Func._TEST_PUT) ){
/*			printf("call '_CREATE_LOCAL_TREE': %s, %lu\n", pSTDdev->taskName, pSTDdev->ST_Base.shotNumber); */
			(pSTDdev->ST_Func._TEST_PUT)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

static void dev_BO_SYS_RESET(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
		pMaster->StatusAdmin & TASK_IN_PROGRESS ) 
	{
		notify_error(ERR_SCN_PNT, "%s running now!\n", pMaster->taskName);
		return;
	}
	/* 2012. 5. 14  unconditional function.. maybe this is right    */
#if 0  
	pMaster->n8EscapeWhile = 0;
	db_put(pMaster->strPvNames[NUM_SYS_RUN], "0");
	epicsThreadSleep(1.0);
	db_put(pMaster->strPvNames[NUM_SYS_ARMING], "0");
	epicsThreadSleep(1.0);
	db_put(pMaster->strPvNames[NUM_SYS_OP_MODE], "0");
	pMaster->n8EscapeWhile = 1;

	notify_error(ERR_SCN, "Clean\n");

	printf("\n[SFW] performed 'caput' to SYSTEM ARMING, OP_MODE with zero value !!! \n");
#endif
/*
	if( pMaster->ST_Func._SYS_RESET )
		(pMaster->ST_Func._SYS_RESET)(pMaster, pParam->setValue, 0);
*/
	while(pSTDdev) 
	{
		if( (pSTDdev->ST_Func._SYS_RESET) ) {
			(pSTDdev->ST_Func._SYS_RESET)(pSTDdev, pParam->setValue, 0);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	if( (int)pParam->setValue)
		printlog("!!!!!! System reset !!!!!!\n" );
	else
		printlog("Reset clear!\n" );

}



/********************************************

 not using call function pointer 


*********************************************/

static void dev_BO_AUTO_CREATE_LOCAL_TREE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	struct dbCommon *precord = pParam->precord;

	pMaster->cUseAutoCreateTree = (char)pParam->setValue;
	if( pMaster->cUseAutoCreateTree ) 
		kfwPrint(0, "%s: will create local tree with CCS shot number when KSTAR sequence start. \n", pMaster->taskName);
	kfwPrint(0, "%s: %s: %d \n", pMaster->taskName, precord->name, pMaster->cUseAutoCreateTree);
}

static void dev_BO_AUTO_SAVE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	struct dbCommon *precord = pParam->precord;

	pMaster->cUseAutoSave = (int)pParam->setValue;
	kfwPrint(0, "%s: %s: %d \n", pMaster->taskName, precord->name, pMaster->cUseAutoSave);
}

static void dev_BO_ENABLE_IP_MIN(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	struct dbCommon *precord = pParam->precord;

	pMaster->cEnable_IPMIN = (int)pParam->setValue;
	kfwPrint(0, "%s: %s: %d \n", pMaster->taskName, precord->name, pMaster->cEnable_IPMIN);
}

static void dev_AO_RUN_PERIOD(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
/*	struct dbCommon *precord = pParam->precord; */
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) 
	{
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n",pMaster->taskName, pMaster->StatusAdmin);
		return;
	}
	pMaster->ST_Base.dRunPeriod = (double)pParam->setValue;

	while(pSTDdev) 
	{
		pSTDdev->ST_Base.dRunPeriod = pMaster->ST_Base.dRunPeriod;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}
static void dev_AO_RT_CLOCK_RATE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
/*	ST_STD_device *pSTDdev = pParam->pSTDdev;*/ /* not valid */
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) {
		notify_error(ERR_SCN_PNT, "%s armed! check again. 0x%x\n",pMaster->taskName, pMaster->StatusAdmin);
		return;
	}

	while(pSTDdev) 
	{
		pSTDdev->rtClkRate =  (epicsUInt32)pParam->setValue;
		pSTDdev->rtStepTime = 1.0 / (float)pSTDdev->rtClkRate;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	kfwPrint(0, "%s: RT clk: %d, Step time: %f\n", pSTDdev->taskName, pSTDdev->rtClkRate, pSTDdev->rtStepTime);
}
static void dev_AO_PRINT_LEVEL(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
/*	ST_STD_device *pSTDdev = pParam->pSTDdev;*/ /* not valid */
/*	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask); */
	pMaster->s32ShowLevel = (epicsInt32)pParam->setValue;

	kfwPrint(0, "%s: Print Level: %d\n", pMaster->taskName, pMaster->s32ShowLevel);
}

static void dev_BO_ERROR_CLEAR(ST_execParam *pParam)
{
/*	ST_MASTER *pMaster = get_master();
	struct dbCommon *precord = pParam->precord; */
	notify_error(ERR_SCN, "no error\n");
}

static long devBoSFW_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	
	switch(precord->out.type) 
	{
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s", arg0);
#if PRINT_SFW_DEVSUP_ARG
			printf("devBo arg num: %d: %s\n", nval,  arg0);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devBo (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devBo (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(arg0, BO_AUTO_SAVE_STR)) 
		pDpvt->ind = BO_AUTO_SAVE;

	else if(!strcmp(arg0, BO_SYS_ARMING_STR)) 
		pDpvt->ind = BO_SYS_ARMING;

	else if(!strcmp(arg0, BO_SYS_RUN_STR)) 
		pDpvt->ind = BO_SYS_RUN;

	else if(!strcmp(arg0, BO_DATA_SEND_STR)) 
		pDpvt->ind = BO_DATA_SEND;

	else if(!strcmp(arg0, BO_SYS_RESET_STR )) 
		pDpvt->ind = BO_SYS_RESET;

	else if(!strcmp(arg0, BO_BOARD_SETUP_STR )) 
		pDpvt->ind = BO_BOARD_SETUP;

	else if( !strcmp(arg0, BO_AUTO_CREATE_STR))
		pDpvt->ind = BO_AUTO_CREATE;
	
	else if( !strcmp(arg0, BO_ERROR_CLEAR_STR))
		pDpvt->ind = BO_ERROR_CLEAR;
	else if( !strcmp(arg0, BO_ENABLE_IP_MIN_STR))
		pDpvt->ind = BO_ENABLE_IP_MIN;

	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devBoSFW_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoSFW_write_bo(boRecord *precord)
{
	ST_dpvt		*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER   *pMaster;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pMaster			= pDpvt->pMaster;
	pControlThreadConfig		= pMaster->pST_MstrCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;


        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{
		precord->pact = TRUE;
#if PRINT_SFW_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) {
			case BO_AUTO_SAVE:
				qData.pFunc = dev_BO_AUTO_SAVE;
				break;
			case BO_SYS_ARMING:
				qData.pFunc = dev_BO_SYS_ARMING;
				break;
			case BO_SYS_RUN:
				qData.pFunc = dev_BO_SYS_RUN;
				break;
			case BO_DATA_SEND:
				qData.pFunc = dev_BO_DATA_SEND;
				break;
			case BO_SYS_RESET:
				qData.pFunc = dev_BO_SYS_RESET;
				break;
			case BO_BOARD_SETUP:
				qData.pFunc = dev_BO_BOARD_SETUP;
				break;
			case BO_AUTO_CREATE:
				qData.pFunc = dev_BO_AUTO_CREATE_LOCAL_TREE;
				break;
			case BO_ERROR_CLEAR:
				qData.pFunc = dev_BO_ERROR_CLEAR;
				break;
			case BO_ENABLE_IP_MIN:
				qData.pFunc = dev_BO_ENABLE_IP_MIN;
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
#if PRINT_SFW_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		notify_refresh_master_status();

		switch(pDpvt->ind) {
			case BO_SYS_ARMING: 
				if( pMaster->StatusAdmin & TASK_ARM_ENABLED )  precord->val = 1;
				else  precord->val = 0;
				break;
			case BO_SYS_RUN: 
				if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
					pMaster->StatusAdmin & TASK_IN_PROGRESS )  precord->val = 1;
				else  precord->val = 0;
				break;
			case BO_DATA_SEND:
				if( pMaster->StatusAdmin & TASK_DATA_TRANSFER)  precord->val = 1;
				else  precord->val = 0;
				break;
			default: break;
		}
		
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devAoSFW_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s %s", arg0, arg1);
#if PRINT_SFW_DEVSUP_ARG
			printf("devAo arg num: %d: %s %s\n", nval,  arg0, arg1);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAo (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAo (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if (!strcmp(arg0, AO_SHOT_NUMBER_STR)) {
		pDpvt->ind = AO_SHOT_NUMBER;
	} 
	else if (!strcmp(arg0, AO_OP_MODE_STR)) {
		pDpvt->ind = AO_OP_MODE;
	}
	 else if (!strcmp(arg0, AO_CREATE_LOCAL_TREE_STR)) {
		pDpvt->ind = AO_CREATE_LOCAL_TREE;
	}
	else if (!strcmp(arg0, AO_SAMPLING_RATE_STR)) {
		pDpvt->ind = AO_SAMPLING_RATE;
	}
	else if (!strcmp(arg0, AO_START_T0_STR)) {
		pDpvt->ind = AO_START_T0;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0);
	}
	else if (!strcmp(arg0, AO_STOP_T1_STR)) {
		pDpvt->ind = AO_STOP_T1;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0);
	}
	else if (!strcmp(arg0, AO_START_TIME_STR)) {
		pDpvt->ind = AO_START_TIME;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0);
	}
	else if (!strcmp(arg0, AO_STOP_TIME_STR)) {
		pDpvt->ind = AO_STOP_TIME;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0);
	}
	else if (!strcmp(arg0, AO_RUN_PERIOD_STR)) {
		pDpvt->ind = AO_RUN_PERIOD;
	}
	else if (!strcmp(arg0, AO_TEST_PUT_STR)) {
		pDpvt->ind = AO_TEST_PUT;
	}
	else if (!strcmp(arg0, AO_RT_CLOCK_RATE_STR)) {
		pDpvt->ind = AO_RT_CLOCK_RATE;
	}
	else if (!strcmp(arg0, AO_PRINT_LEVEL_STR)) {
		pDpvt->ind = AO_PRINT_LEVEL;
	}
	

	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devAoSFW_init_record: arg0 \"%s\" \n",  arg0 );
		
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoSFW_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER          *pMaster;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pMaster			= pDpvt->pMaster;
	pControlThreadConfig		= pMaster->pST_MstrCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;
	qData.param.n32Arg0		= pDpvt->n32Arg0;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_SFW_PHASE_INFO
		printf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_SHOT_NUMBER:
				qData.pFunc = dev_AO_SHOT_NUMBER;
				break;
			case AO_OP_MODE:
				qData.pFunc = dev_AO_OP_MODE;
				break;
			case AO_CREATE_LOCAL_TREE:
				qData.pFunc = dev_AO_CREATE_LOCAL_TREE;
				break;
			case AO_SAMPLING_RATE:
				qData.pFunc = dev_AO_SAMPLING_RATE;
				break;
			case AO_START_T0:
				qData.pFunc = dev_AO_T0;
				break;
			case AO_STOP_T1:
				qData.pFunc = dev_AO_T1;
				break;
			case AO_START_TIME:
				qData.pFunc = dev_AO_START_TIME;
				break;
			case AO_STOP_TIME:
				qData.pFunc = dev_AO_STOP_TIME;
				break;
			case AO_RUN_PERIOD:
				qData.pFunc = dev_AO_RUN_PERIOD;
				break;
			case AO_TEST_PUT:
				qData.pFunc = dev_AO_TEST_PUT;
				break;
			case AO_RT_CLOCK_RATE:
				qData.pFunc = dev_AO_RT_CLOCK_RATE;
				break;
			case AO_PRINT_LEVEL:
				qData.pFunc = dev_AO_PRINT_LEVEL;
				break;
			
			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId, (void*) &qData, sizeof(ST_threadQueueData));

		return 0; /*(0)=>(success ) */
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
/*		int n32Arg0; */
#if PRINT_SFW_PHASE_INFO
		printf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		
		switch(pDpvt->ind) 
		{
			case AO_SHOT_NUMBER: precord->val = pMaster->ST_Base.shotNumber; break;
			case AO_OP_MODE: precord->val = pMaster->ST_Base.opMode; break;
			case AO_CREATE_LOCAL_TREE: precord->val = pMaster->ST_Base.shotNumber; break;
			case AO_SAMPLING_RATE: precord->val = pMaster->ST_Base.nSamplingRate; break;
			case AO_START_T0: 
/*				n32Arg0 = strtoul(pDpvt->arg1, NULL, 0); */
				precord->val = pMaster->ST_Base.dT0[pDpvt->n32Arg0]; 
				break;
			case AO_STOP_T1:
/*				n32Arg0 = strtoul(pDpvt->arg1, NULL, 0); */
				precord->val = pMaster->ST_Base.dT1[pDpvt->n32Arg0]; 
				break;
			case AO_RUN_PERIOD:precord->val = pMaster->ST_Base.dRunPeriod; break;

			default: break;
		}

		notify_refresh_master_status();

		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0; /*(0)=>(success ) */
	}

	return -1;
}

static long devStringoutSFW_init_record(stringoutRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s", arg0 );
#if PRINT_SFW_DEVSUP_ARG
			printf("devStringout arg num: %d: %s\n",nval, arg0);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) 
			{
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringout (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringout (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(arg0, "xxx")) {
		pDpvt->ind = 1;
	}  
	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devStringoutSFW_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}


static long devStringoutSFW_write_stringout(stringoutRecord *precord)
{
	int len;
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER          *pMaster;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}

	pMaster			= pDpvt->pMaster;
	pControlThreadConfig		= pMaster->pST_MstrCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;

	len = strlen(precord->val);
	if( len > SIZE_STRINGOUT_VAL ) 
	{
		kfwPrint(0, "string out size limited, %d\n", len);
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}
	strcpy( qData.param.setStr, precord->val );

 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_SFW_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif			
		switch(pDpvt->ind) 
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
#if PRINT_SFW_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1; /*(-1,0)=>(failure,success)*/
}

static long devStringinSFW_init_record(stringinRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];

	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s", arg0 );
#if PRINT_SFW_DEVSUP_ARG
			printf("devStringin arg num: %d: %s\n",nval, arg0);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) 
			{
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringin (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringin (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(arg0, STRINGIN_ERROR_STRING_STR)) {
		pDpvt->ind = STRINGIN_ERROR_STRING;
	}  
	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devStringinSFW_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 0;    /*returns: (-1,0)=>(failure,success)*/
}

static long devStringinSFW_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER   *pMaster;
	if(!pDpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pMaster = pDpvt->pMaster;
	*ioScanPvt  = pMaster->ioScanPvt_status;
	return 0;
}
static long devStringinSFW_read_stringin(stringinRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER		*pMaster = NULL;

	if(!pDpvt) return -1; /*returns: (-1,0)=>(failure,success)*/

	pMaster = pDpvt->pMaster;
	if( !pMaster) {
		kfwPrint(0, "read stingin: get master failed!\n");
		return -1; /*returns: (-1,0)=>(failure,success)*/
	}

	switch(pDpvt->ind) 
	{
		case STRINGIN_ERROR_STRING:
			strcpy( precord->val, pMaster->ErrorString);
/*			epicsPrintf("%s Error Info: %s \n", pMaster->taskName, precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... _SFW \n", pMaster->taskName, pDpvt->ind); 
			break;
	}
	return (0); /*returns: (-1,0)=>(failure,success)*/
}


static long devAiSFW_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s", arg0 );
#if PRINT_SFW_DEVSUP_ARG
			printf("devAi arg num: %d: %s\n", nval, arg0);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAi (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAi (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, "xxx"))
		pDpvt->ind = 1;	
	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devAiSFW_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiSFW_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER   *pMaster;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pMaster = pDpvt->pMaster;
	*ioScanPvt  = pMaster->ioScanPvt_status;
	return 0;
}

static long devAiSFW_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER		*pMaster;

	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pMaster = pDpvt->pMaster;

	switch(pDpvt->ind) 
	{
		case 0:
/*			precord->val = pMaster->StatusAdmin; */
/*			epicsPrintf("%s StatusDev: 0x%X \n", pMaster->taskName, (int)precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... _SFW \n", pMaster->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}

static long devBiSFW_init_record(biRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s", arg0 );
#if PRINT_SFW_DEVSUP_ARG
			printf("devBi arg num: %d: %s\n", nval, arg0);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBi (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBi (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, "xxx"))
		pDpvt->ind = 1;
	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devBiSFW_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiSFW_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER   *pMaster;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pMaster = pDpvt->pMaster;
	*ioScanPvt  = pMaster->ioScanPvt_status;
	return 0;
}

static long devBiSFW_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER		*pMaster;

	if(!pDpvt) return -1;

	pMaster = pDpvt->pMaster;

	switch(pDpvt->ind) 
	{
		case 1:
			precord->val = 0;
/*			epicsPrintf("%s StatusDev: 0x%X \n", pMaster->taskName, (int)precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... _SFW \n", pMaster->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert, don't convert)*/
}

static long devMbbiSFW_init_record(mbbiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s", arg0 );
#if PRINT_SFW_DEVSUP_ARG
			printf("devMbbi arg num: %d: %s\n", nval, arg0);
#endif
			pDpvt->pMaster = get_master();
			if(!pDpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devMbbi (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devMbbi (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, MBBI_ADMIN_STATUS_STR))
		pDpvt->ind = MBBI_ADMIN_STATUS;
	else if (!strcmp(arg0, MBBI_ADMIN_ERROR_STR))
		pDpvt->ind = MBBI_ADMIN_ERROR;
	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devMbbiSFW_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;   /*returns: (-1,0)=>(failure,success)*/
}


static long devMbbiSFW_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER   *pMaster;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pMaster = pDpvt->pMaster;
	*ioScanPvt  = pMaster->ioScanPvt_status;
	return 0;
}

static long devMbbiSFW_read_mbbi(mbbiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER		*pMaster;

	if(!pDpvt) return -1;

	pMaster = pDpvt->pMaster;

	switch(pDpvt->ind) 
	{
		case MBBI_ADMIN_STATUS:
			precord->rval = pMaster->StatusAdmin;
			if( pMaster->StatusAdmin & TASK_STANDBY ) 
			{
				if( pMaster->StatusAdmin & TASK_SYSTEM_IDLE ) precord->val = STATUS_STANDBY; /* standby */
				else if( pMaster->StatusAdmin & TASK_DATA_TRANSFER ) precord->val = STATUS_DATA_TRANSFER;
				else if( pMaster->StatusAdmin & TASK_POST_PROCESS ) precord->val = STATUS_POST_PROCESS;
				else if( pMaster->StatusAdmin & TASK_IN_PROGRESS) precord->val = STATUS_IN_PROGRESS;
				else if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ) precord->val = STATUS_WAIT_FOR_TRIGGER;
				else if( pMaster->StatusAdmin & TASK_ARM_ENABLED ) precord->val = STATUS_ARM_ENABLED;
				else precord->val = 8; /* state transition. 2011.4.18 */
			} else {
				if( pMaster->StatusAdmin & TASK_SYSTEM_IDLE ) precord->val = STATUS_SYSTEM_IDLE; /* not ready, Idle */
				else precord->val = 0; /* not initiated or NULL */
			} 
/*			printf("[SFW] %s Status: 0x%X, rval: %d, val: %d \n", pMaster->taskName, pMaster->StatusAdmin, precord->rval, precord->val); */
			break;
		case MBBI_ADMIN_ERROR:
			precord->val = pMaster->ErrorAdmin; 
			epicsPrintf("%s Error: 0x%X, %d \n", pMaster->taskName, (unsigned short)precord->val, (unsigned short)precord->val);
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... _SFW \n", pMaster->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=>(success, success no convert)*/
}


