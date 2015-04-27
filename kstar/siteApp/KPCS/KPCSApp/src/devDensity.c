/****************************************************************************

Module      : devDNST.c

Copyright(c): 2012 NFRI. All Rights Reserved.

Revision History:
Author: woong   2012-4-27
  

*****************************************************************************/
#include <sys/mman.h>
#include <fcntl.h> /* open() */


#include "aiRecord.h"
#include "aoRecord.h"
#include "biRecord.h"
#include "boRecord.h"

#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



/**********************************
user include files
***********************************/
#include "drvDensity.h"

/**********************************
user include files 
***********************************/

#define PRINT_DBPRO_PHASE_INFO	0



#define USE_SCANIOREQUEST		1
#define USE_SCANIOREQUEST_STR	"use_ioReq"

#define BI_TEST_1		1
#define BI_TEST_2		2
#define BI_TEST_1_STR	"bi1"
#define BI_TEST_2_STR	"bi2"



#define AO_TEST_1  	1
#define AO_TEST_2	2
#define AO_TEST_1_STR  	"ao1"
#define AO_TEST_2_STR	"ao2"


#define AI_DAQ_CNT 		1
#define AI_DAQ_CNT_STR		"daq_cnt"
#define AI_PID_CMD_VOLTAGE 		2
#define AI_PID_CMD_VOLTAGE_STR		"pid_cmd"






/*******************************************************/
/*******************************************************/

static long devAoDNST_init_record(aoRecord *precord);
static long devAoDNST_write_ao(aoRecord *precord);

static long devAiDNST_init_record(aiRecord *precord);
static long devAiDNST_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiDNST_read_ai(aiRecord *precord);

static long devBoDNST_init_record(boRecord *precord);
static long devBoDNST_write_bo(boRecord *precord);

static long devBiDNST_init_record(biRecord *precord);
static long devBiDNST_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiDNST_read_bi(biRecord *precord);

/*******************************************************/

BINARYDSET devBoDNST = { 6,  NULL,  NULL,   devBoDNST_init_record,  NULL,  devBoDNST_write_bo,  NULL };
BINARYDSET  devAoDNST = { 6, NULL, NULL,  devAoDNST_init_record, NULL, devAoDNST_write_ao, NULL };
BINARYDSET  devAiDNST = { 6, NULL, NULL, devAiDNST_init_record, devAiDNST_get_ioint_info, devAiDNST_read_ai, NULL };
BINARYDSET  devBiDNST = { 6, NULL, NULL, devBiDNST_init_record, devBiDNST_get_ioint_info, devBiDNST_read_bi, NULL };

epicsExportAddress(dset, devBoDNST);
epicsExportAddress(dset, devAoDNST);
epicsExportAddress(dset, devAiDNST);
epicsExportAddress(dset, devBiDNST);



static void devBoDNST_USE_SCANIOREQUEST(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_DNST* pDNST = (ST_DNST *)pSTDdev->pUser;


	pDNST->useScanIoRequest = (int)pParam->setValue;

	epicsPrintf("%s => %d \n", precord->name, pDNST->useScanIoRequest );
}


static void devAoDNST_AO_TEST_1(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devAoDNST_AO_TEST_2(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}


/*********************************************************
 initial setup of records 
*********************************************************/
static long devBoDNST_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBoDNST (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoDNST (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, USE_SCANIOREQUEST_STR)) {
		pDpvt->ind = USE_SCANIOREQUEST;
	} 
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoDNST_init_record: arg0 \"%s\" \n",  arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoDNST_write_bo(boRecord *precord)
{
	ST_dpvt		*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg 	*pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;
	qData.param.n32Arg0		= pDpvt->n32Arg0;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) {

			case USE_SCANIOREQUEST:
				qData.pFunc = devBoDNST_USE_SCANIOREQUEST;
				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pDpvt->ind) {
			case USE_SCANIOREQUEST: 
				/*do something */
				/*precord->val = 0; */
				break;
			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devBiDNST_init_record(biRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;

	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBiDNST (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiDNST (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, BI_TEST_1_STR)) {
		pDpvt->ind = BI_TEST_1;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */	
	}
	else if(!strcmp(arg0, BI_TEST_2_STR))
		pDpvt->ind = BI_TEST_2;

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devBiDNST_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiDNST_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devBiDNST_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_DNST *pDNST;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pDNST = (ST_DNST*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case BI_TEST_1: 
			precord->val = 1;
			break;
		case BI_TEST_2: 
			precord->val = 0;
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... DNST \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoDNST_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoDNST (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoDNST (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, AO_TEST_1_STR))
		pDpvt->ind = AO_TEST_1;
	
	else if (!strcmp(arg0, AO_TEST_2_STR)) {
		pDpvt->ind = AO_TEST_2;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}
	


	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAoDNST_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoDNST_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_DNST *pDNST;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;


	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;

	pDNST = (ST_DNST*)pSTDdev->pUser;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_TEST_1:
				qData.pFunc = devAoDNST_AO_TEST_1;
				break;
			case AO_TEST_2:
				qData.pFunc = devAoDNST_AO_TEST_2;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				qData.param.n32Arg1 = pDpvt->n32Arg1; /* section id */
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
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_TEST_1:
				/*do something */
				break;
			case AO_TEST_2:
				/*do something */
				break;

			default:
				break;
		}

		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0; /*(0)=>(success ) */
	}

	return -1;
}



static long devAiDNST_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiDNST (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiDNST (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if (!strcmp(arg0, AI_DAQ_CNT_STR)) {
		pDpvt->ind = AI_DAQ_CNT;
//		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
//		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}
	else if( !strcmp(arg0, AI_PID_CMD_VOLTAGE_STR)) {
		pDpvt->ind = AI_PID_CMD_VOLTAGE;

	}
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAiDNST_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiDNST_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiDNST_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_DNST *pDNST;


	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;
	pDNST = (ST_DNST*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_DAQ_CNT: 
			precord->val = pDNST->cntDAQ;
			break;
		case AI_PID_CMD_VOLTAGE:
			precord->val = pDNST->pid_u;
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... DNST \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}


