/****************************************************************************

Module      : devXXX.c

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
#include "drvXXX.h"

/**********************************
user include files 
***********************************/

#define PRINT_DBPRO_PHASE_INFO	0



#define BO_TEST_1		1
#define BO_TEST_2		2
#define BO_TEST_1_STR	"bo1"
#define BO_TEST_2_STR	"bo2"

#define BI_TEST_1		1
#define BI_TEST_2		2
#define BI_TEST_1_STR	"bi1"
#define BI_TEST_2_STR	"bi2"



#define AO_TEST_1  	1
#define AO_TEST_2	2
#define AO_TEST_1_STR  	"ao1"
#define AO_TEST_2_STR	"ao2"


#define AI_TEST_1 		1
#define AI_TEST_2 		2
#define AI_TEST_1_STR		"ai1"
#define AI_TEST_2_STR		"ai2"






/*******************************************************/
/*******************************************************/

static long devAoXXX_init_record(aoRecord *precord);
static long devAoXXX_write_ao(aoRecord *precord);

static long devAiXXX_init_record(aiRecord *precord);
static long devAiXXX_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiXXX_read_ai(aiRecord *precord);

static long devBoXXX_init_record(boRecord *precord);
static long devBoXXX_write_bo(boRecord *precord);

static long devBiXXX_init_record(biRecord *precord);
static long devBiXXX_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiXXX_read_bi(biRecord *precord);

/*******************************************************/

BINARYDSET devBoXXX = { 6,  NULL,  NULL,   devBoXXX_init_record,  NULL,  devBoXXX_write_bo,  NULL };
BINARYDSET  devAoXXX = { 6, NULL, NULL,  devAoXXX_init_record, NULL, devAoXXX_write_ao, NULL };
BINARYDSET  devAiXXX = { 6, NULL, NULL, devAiXXX_init_record, devAiXXX_get_ioint_info, devAiXXX_read_ai, NULL };
BINARYDSET  devBiXXX = { 6, NULL, NULL, devBiXXX_init_record, devBiXXX_get_ioint_info, devBiXXX_read_bi, NULL };

epicsExportAddress(dset, devBoXXX);
epicsExportAddress(dset, devAoXXX);
epicsExportAddress(dset, devAiXXX);
epicsExportAddress(dset, devBiXXX);



static void devBoXXX_BO_TEST_1(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_XXX* pXXX = (ST_XXX *)pSTDdev->pUser;

	printf("REG_REVISION:		0x%x\n", pXXX->pBaseAddr->gpRevision);
	printf("REG_BUILD_DATE:		0x%x\n", pXXX->pBaseAddr->gpBuildDate);
	printf("REG_SERIAL_NUMBER:	0x%x\n", pXXX->pBaseAddr->gpSerialNum);
	printf("REG_READ_WRITE_REG:	0x%x\n", pXXX->pBaseAddr->gpReadWrite);
	printf("REG_CONTROL:		0x%X\n", pXXX->pBaseAddr->gpBoardControl);

	epicsPrintf("%s: %s ( %d )\n", pSTDdev->taskName, precord->name, (int)pParam->setValue);
}
static void devBoXXX_BO_TEST_2(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}


static void devAoXXX_AO_TEST_1(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devAoXXX_AO_TEST_2(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}


/*********************************************************
 initial setup of records 
*********************************************************/
static long devBoXXX_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBoXXX (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoXXX (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(arg0, BO_TEST_1_STR)) {
		pDpvt->ind = BO_TEST_1;
		pDpvt->n32Arg0 = strtol(pDpvt->arg1,NULL,0);
	} 
	else if (!strcmp(arg0, BO_TEST_2_STR)) {
		pDpvt->ind = BO_TEST_2;
	} 
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoXXX_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoXXX_write_bo(boRecord *precord)
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

			case BO_TEST_1:
				qData.pFunc = devBoXXX_BO_TEST_1;
				break;
			case BO_TEST_2:
				qData.pFunc = devBoXXX_BO_TEST_2;
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
			case BO_TEST_1: 
				/*do something */
				/*precord->val = 0; */
				break;
			case BO_TEST_2: 
				/*do something */
				/*precord->val = 0; */
				break;
			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devBiXXX_init_record(biRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;

	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBiXXX (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiXXX (init_record) Illegal INP field");
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
		epicsPrintf("ERROR!! devBiXXX_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiXXX_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devBiXXX_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_XXX *pXXX;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pXXX = (ST_XXX*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case BI_TEST_1: 
			/* precord->val = your value */
			break;
		case BI_TEST_2: 
			/* precord->val = your value */
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... XXX \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoXXX_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoXXX (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoXXX (init_record) Illegal OUT field");
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
		epicsPrintf("ERROR!! devAoXXX_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoXXX_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_XXX *pXXX;
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

	pXXX = (ST_XXX*)pSTDdev->pUser;

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
				qData.pFunc = devAoXXX_AO_TEST_1;
				break;
			case AO_TEST_2:
				qData.pFunc = devAoXXX_AO_TEST_2;
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



static long devAiXXX_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiXXX (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiXXX (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, AI_TEST_1_STR))
		pDpvt->ind = AI_TEST_1;
	
	else if (!strcmp(arg0, AI_TEST_2_STR)) {
		pDpvt->ind = AI_TEST_2;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAiXXX_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiXXX_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiXXX_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_XXX *pXXX;


	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;
	pXXX = (ST_XXX*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_TEST_1: 
			/* precord->val = your value */
			break;
		case AI_TEST_2: 
			/* precord->val = your value */
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... XXX \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}


