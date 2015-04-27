/****************************************************************************

Module      : devRTMON.c

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
#include "stringoutRecord.h"

#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



/**********************************
user include files
***********************************/
#include "drvRMCHK.h"

/**********************************
user include files 
***********************************/

#define PRINT_DBPRO_PHASE_INFO	0



#define BO_USE_SCANIOREQUEST		1
#define BO_USE_SCANIOREQUEST_STR	"use_ioReq"
#define BO_CH_MASK      2
#define BO_CH_MASK_STR      "mask"
#define BO_INIT_FILE   3
#define BO_INIT_FILE_STR   "init_file"


#define BI_TEST_1		1
#define BI_TEST_2		2
#define BI_TEST_1_STR	"bi1"
#define BI_TEST_2_STR	"bi2"


#define STRINGOUT_USER_OFFSET 		1
#define STRINGOUT_USER_OFFSET_STR		"u_ofset"
#define STRINGOUT_TAG_NAME	2
#define STRINGOUT_TAG_NAME_STR	"tag"
/*
#define STRINGOUT_POKE32_OFFSET 		3
#define STRINGOUT_POKE32_OFFSET_STR		"poke32_ofset"
*/


#define AI_DAQ_CNT 		1
#define AI_DAQ_CNT_STR		"daq_cnt"
#define AI_SCAN_OFFSET 		2
#define AI_SCAN_OFFSET_STR		"scn_offset"
#define AI_USER_OFFSET_VALUE  	3
#define AI_USER_OFFSET_VALUE_STR  	"uoff_val"
#define AI_PLASMA_CURRENT  	4
#define AI_PLASMA_CURRENT_STR  	"info_ip"
#define AI_PLASMA_TIME  	5
#define AI_PLASMA_TIME_STR  	"ip_time"



#define AO_TAG_ADDR 		1
#define AO_TAG_ADDR_STR		"tag_addr"
#define AO_POKE32 		2
#define AO_POKE32_STR		"poke32_val"








/*******************************************************/
/*******************************************************/

static long devAoRMCHK_init_record(aoRecord *precord);
static long devAoRMCHK_write_ao(aoRecord *precord);

static long devAiRMCHK_init_record(aiRecord *precord);
static long devAiRMCHK_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiRMCHK_read_ai(aiRecord *precord);

static long devBoRMCHK_init_record(boRecord *precord);
static long devBoRMCHK_write_bo(boRecord *precord);

static long devBiRMCHK_init_record(biRecord *precord);
static long devBiRMCHK_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiRMCHK_read_bi(biRecord *precord);

static long devStringoutRMCHK_init_record(stringoutRecord *precord);
static long devStringoutRMCHK_write_stringout(stringoutRecord *precord);


/*******************************************************/

BINARYDSET devBoRMCHK = { 6,  NULL,  NULL,   devBoRMCHK_init_record,  NULL,  devBoRMCHK_write_bo,  NULL };
BINARYDSET  devAoRMCHK = { 6, NULL, NULL,  devAoRMCHK_init_record, NULL, devAoRMCHK_write_ao, NULL };
BINARYDSET  devAiRMCHK = { 6, NULL, NULL, devAiRMCHK_init_record, devAiRMCHK_get_ioint_info, devAiRMCHK_read_ai, NULL };
BINARYDSET  devBiRMCHK = { 6, NULL, NULL, devBiRMCHK_init_record, devBiRMCHK_get_ioint_info, devBiRMCHK_read_bi, NULL };
BINARYDSET  devStringoutRMCHK = { 6, NULL, NULL, devStringoutRMCHK_init_record, NULL, devStringoutRMCHK_write_stringout, NULL };


epicsExportAddress(dset, devBoRMCHK);
epicsExportAddress(dset, devAoRMCHK);
epicsExportAddress(dset, devAiRMCHK);
epicsExportAddress(dset, devBiRMCHK);
epicsExportAddress(dset, devStringoutRMCHK);




static void devRMCHK_BO_USE_SCANIOREQUEST(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK* pRMCHK = (ST_RMCHK *)pSTDdev->pUser;


	pRMCHK->useScanIoRequest = (int)pParam->setValue;

	epicsPrintf("%s => %d \n", precord->name, pRMCHK->useScanIoRequest );
}

static void  devRMCHK_BO_CH_MASK(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK *pRMCHK = (ST_RMCHK*)pSTDdev->pUser;
	int val = (int)pParam->setValue;
	int i = pParam->n32Arg0;

	pRMCHK->channelOnOff[i] = val;
	drvRMCHK_save_file(pSTDdev);

//	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devRMCHK_BO_INIT_FILE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	ST_RMCHK *pRMCHK = (ST_RMCHK*)pSTDdev->pUser;

	drvRMCHK_init_file(pSTDdev);
	
}


static void devStringoutRMCHK_USER_OFFSET_ID(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK* pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	char *endp;
	unsigned int myOffset = strtoul(pParam->setStr, &endp, 16); /* offset */
	

	pRMCHK->user_offset[(int)pParam->n32Arg0] = myOffset;

	epicsPrintf("%s ID: %d ( 0x%x )\n", precord->name, (int)pParam->n32Arg0, myOffset); 
}

static void devStringoutRMCHK_TAG_NAME(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK* pRMCHK = (ST_RMCHK *)pSTDdev->pUser;

	strcpy(pRMCHK->strTagName[pParam->n32Arg0], pParam->setStr);
	drvRMCHK_save_file(pSTDdev);

/*	epicsPrintf("%s CH: %d ( %s )\n", precord->name, (int)pParam->n32Arg0, pRMCHK->strTagName[pParam->n32Arg0]); */
}
/*
static void devStringoutRMCHK_POKE32_OFFSET	(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK* pRMCHK = (ST_RMCHK *)pSTDdev->pUser;
	char *endp;
	unsigned int myOffset = strtoul(pParam->setStr, &endp, 16); 

	pRMCHK->user_offset[(int)pParam->n32Arg0] = myOffset;

	epicsPrintf("%s ID: %d ( 0x%x )\n", precord->name, (int)pParam->n32Arg0, myOffset); 
}
*/
static void devRMCHK_AO_TAG_ADDR(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK* pRMCHK = (ST_RMCHK *)pSTDdev->pUser;

	pRMCHK->tagAddr[pParam->n32Arg0] = (unsigned int)pParam->setValue;
	drvRMCHK_save_file(pSTDdev);

/*	epicsPrintf("%s ( 0x%x )\n", precord->name, (unsigned int)pParam->setValue); */
}
static void devRMCHK_AO_POKE32(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_RMCHK* pRMCHK = (ST_RMCHK *)pSTDdev->pUser;

	unsigned int uoffset = pRMCHK->user_offset[pParam->n32Arg0];
	pRMCHK->mapBuf[uoffset/4] = (int)pParam->setValue;;
			

	epicsPrintf("%s ( %d, 0x%x )\n", precord->name, (int)pParam->setValue, (int)pParam->setValue);
}


/*********************************************************
 initial setup of records 
*********************************************************/
static long devBoRMCHK_init_record(boRecord *precord)
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
				recGblRecordError(S_db_badField, (void*) precord,  "devBoRTMON (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoRTMON (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, BO_USE_SCANIOREQUEST_STR)) {
		pDpvt->ind = BO_USE_SCANIOREQUEST;
	} 
	else if(!strcmp(arg0, BO_CH_MASK_STR)) {
		pDpvt->ind = BO_CH_MASK;
		pDpvt->n32Arg0 = strtol(arg1,NULL,0);
	} 
	else if (!strcmp(arg0, BO_INIT_FILE_STR)) {
		pDpvt->ind = BO_INIT_FILE;
	} 
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoRMCHK_init_record: arg0 \"%s\" \n",  arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoRMCHK_write_bo(boRecord *precord)
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

			case BO_USE_SCANIOREQUEST:
				qData.pFunc = devRMCHK_BO_USE_SCANIOREQUEST;
				break;
			case BO_CH_MASK:
				qData.pFunc = devRMCHK_BO_CH_MASK;
				break;
			case BO_INIT_FILE:
				qData.pFunc = devRMCHK_BO_INIT_FILE;
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
			case BO_USE_SCANIOREQUEST: 
				/*do something */
				/*precord->val = 0; */
				break;
			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devBiRMCHK_init_record(biRecord *precord)
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
						  "devBiRTMON (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiRTMON (init_record) Illegal INP field");
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
		epicsPrintf("ERROR!! devBiRMCHK_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiRMCHK_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devBiRMCHK_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_RMCHK *pRMCHK;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pRMCHK = (ST_RMCHK*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case BI_TEST_1: 
			precord->val = 1;
			break;
		case BI_TEST_2: 
			precord->val = 0;
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... RTMON \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoRMCHK_init_record(aoRecord *precord)
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
						  "devAoRTMON (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoRTMON (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, AO_TAG_ADDR_STR)) {
		pDpvt->ind = AO_TAG_ADDR;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* tag address in RFM */

	} else if (!strcmp(arg0, AO_POKE32_STR)) {
		pDpvt->ind = AO_POKE32;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* offset ID*/

	} 
	
	


	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAoRMCHK_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoRMCHK_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_RMCHK *pRMCHK;
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

	pRMCHK = (ST_RMCHK*)pSTDdev->pUser;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_TAG_ADDR:
				qData.pFunc = devRMCHK_AO_TAG_ADDR;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				break;
			case AO_POKE32:
				qData.pFunc = devRMCHK_AO_POKE32;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* offset id */
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
			case 99:
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


static long devStringoutRMCHK_init_record(stringoutRecord *precord)
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
						  "devStringoutRTMON (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutRTMON (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if (!strcmp(arg0, STRINGOUT_USER_OFFSET_STR)) {
		pDpvt->ind = STRINGOUT_USER_OFFSET;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* target offset Id */
	}
	else if (!strcmp(arg0, STRINGOUT_TAG_NAME_STR)) {
		pDpvt->ind = STRINGOUT_TAG_NAME;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* tag name id */
	}
/*	else if (!strcmp(arg0, STRINGOUT_POKE32_OFFSET_STR)) {
		pDpvt->ind = STRINGOUT_POKE32_OFFSET;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); 
	}
*/	
	else {
		pDpvt->ind = -1;
		epicsPrintf("[RTMON] devStringoutRMCHK_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}

static long devStringoutRMCHK_write_stringout(stringoutRecord *precord)
{
	int len;
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_RMCHK *pRMCHK;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;


	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;


	pRMCHK = (ST_RMCHK*)pSTDdev->pUser;

	

	len = strlen(precord->val);
	if( len > SIZE_STRINGOUT_VAL ) 
	{
		printf("string out size limited, %d\n", len);
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
			case STRINGOUT_USER_OFFSET:
				qData.pFunc = devStringoutRMCHK_USER_OFFSET_ID;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* user offset Id */
				break;
			case STRINGOUT_TAG_NAME:
				qData.pFunc = devStringoutRMCHK_TAG_NAME;
				qData.param.n32Arg0 = pDpvt->n32Arg0;  /* tag name Id */
				break;
/*			case STRINGOUT_POKE32_OFFSET:
				qData.pFunc = devStringoutRMCHK_POKE32_OFFSET;
				qData.param.n32Arg0 = pDpvt->n32Arg0; 
				break; */
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



static long devAiRMCHK_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];
	char *endp;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiRTMON (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiRTMON (init_record) Illegal INP field");
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
	else if(!strcmp(arg0, AI_SCAN_OFFSET_STR)) {
		pDpvt->ind = AI_SCAN_OFFSET;
		pDpvt->n32Arg0 = strtoul(arg1, &endp, 16); /* offset */
		printf("arg1: %d 0x%x %s\n", pDpvt->n32Arg0, pDpvt->n32Arg0, arg1);
	}
	else if(!strcmp(arg0, AI_USER_OFFSET_VALUE_STR)) {
		pDpvt->ind = AI_USER_OFFSET_VALUE;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* offset Id */
	}
	else if(!strcmp(arg0, AI_PLASMA_CURRENT_STR)) {
		pDpvt->ind = AI_PLASMA_CURRENT;
	}
	else if(!strcmp(arg0, AI_PLASMA_TIME_STR)) {
		pDpvt->ind = AI_PLASMA_TIME;
	}
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAiRMCHK_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiRMCHK_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

double pre_ip;
static long devAiRMCHK_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_RMCHK *pRMCHK;
	int offset;
	RFM2G_STATUS   result;   


	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;
	pRMCHK = (ST_RMCHK*)pSTDdev->pUser;
	offset = pDpvt->n32Arg0;

	switch(pDpvt->ind) 
	{
		case AI_DAQ_CNT: 
			precord->val = pSTDdev->stdDAQCnt;
			break;
		case AI_SCAN_OFFSET: 
			precord->val = pRMCHK->mapBuf[offset/4];
			break;
		case AI_USER_OFFSET_VALUE:
			{
			unsigned int uoffset = pRMCHK->user_offset[pDpvt->n32Arg0];
			precord->val = pRMCHK->mapBuf[uoffset/4];
			}
			break;
		case AI_PLASMA_CURRENT:
			{
				float current;
				int index = pRMCHK->mapBuf[RM_SCOPE_BUF_INDEX/4];
				int offset = (RM_SCOPE_BUF_DATA + 200*(index-1) );				

//				memcpy(&current, &pRMCHK->mapBuf[ offset / 4], sizeof(current) );
				result = RFM2gRead( pRMCHK->Handle, offset, &current, sizeof(current) );
				precord->val = -1 * current * 1.0e-6;
/*				
				if( pre_ip == precord->val){
//					precord->val = pre_ip + 0.00123456780;
					precord->val = pre_ip + (srand(time(NULL))%10) * 0.0001;
//				} else pre_ip = precord->val;
*/
				pre_ip = precord->val;
//				printf("IP: Index: %d, offset: 0x%x, ip: %f, %f\n", index, offset, precord->val, current );

				/* just RMP check *//*
				result = RFM2gRead( pRMCHK->Handle, RM_RMP_TOP_I, &current, sizeof(current) );
				printf("RMP Top current: %f\n", current); */
			}
			break;
		case AI_PLASMA_TIME:
			{
				int mtime = pRMCHK->mapBuf[0x3800004 /4];
				precord->val = (double)((double)mtime - 15000000.0)*1.0e-6;
/*				printf("IP current time: %f sec\n", precord->val ); */
			}
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... RTMON \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}


