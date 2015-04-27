/****************************************************************************

Module      : devACQ196.c

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
#include "mbbiRecord.h"

#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */
#include "sfwGlobalDef.h"


/**********************************
user include files
***********************************/
#include "drvACQ196.h"
#include "acqPvString.h"
#include "acqHTstream.h"

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

static long devAoACQ196_init_record(aoRecord *precord);
static long devAoACQ196_write_ao(aoRecord *precord);

static long devAiACQ196_init_record(aiRecord *precord);
static long devAiACQ196_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiACQ196_read_ai(aiRecord *precord);

static long devBoACQ196_init_record(boRecord *precord);
static long devBoACQ196_write_bo(boRecord *precord);

static long devBiACQ196_init_record(biRecord *precord);
static long devBiACQ196_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiACQ196_read_bi(biRecord *precord);

static long devStringoutACQ196_init_record(stringoutRecord *precord);
static long devStringoutACQ196_write_stringout(stringoutRecord *precord);

static long devMbbiACQ196_init_record(mbbiRecord *precord);
static long devMbbiACQ196_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
static long devMbbiACQ196_read_mbbi(mbbiRecord *precord);



/*******************************************************/

BINARYDSET devBoACQ196 = { 6,  NULL,  NULL,   devBoACQ196_init_record,  NULL,  devBoACQ196_write_bo,  NULL };
BINARYDSET  devAoACQ196 = { 6, NULL, NULL,  devAoACQ196_init_record, NULL, devAoACQ196_write_ao, NULL };
BINARYDSET  devAiACQ196 = { 6, NULL, NULL, devAiACQ196_init_record, devAiACQ196_get_ioint_info, devAiACQ196_read_ai, NULL };
BINARYDSET  devBiACQ196 = { 6, NULL, NULL, devBiACQ196_init_record, devBiACQ196_get_ioint_info, devBiACQ196_read_bi, NULL };

BINARYDSET  devStringoutACQ196 = { 6, NULL, NULL, devStringoutACQ196_init_record, NULL, devStringoutACQ196_write_stringout, NULL };
BINARYDSET  devMbbiACQ196 = { 5, NULL, NULL, devMbbiACQ196_init_record, devMbbiACQ196_get_ioint_info, devMbbiACQ196_read_mbbi };


epicsExportAddress(dset, devBoACQ196);
epicsExportAddress(dset, devAoACQ196);
epicsExportAddress(dset, devAiACQ196);
epicsExportAddress(dset, devBiACQ196);
epicsExportAddress(dset, devStringoutACQ196);
epicsExportAddress(dset, devMbbiACQ196);






static void devACQ196_BO_CH_MASK(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	int val = (int)pParam->setValue;
	int i = pParam->n32Arg0;

	pAcq196->channelOnOff[i] = val;

	drvACQ196_save_file(pSTDdev);

//	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devACQ196_BO_CLOCK_MODE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pAcq196->clockSource = (int)pParam->setValue;

	if( pAcq196->clockSource == CLOCK_INTERNAL )
		kfwPrint(0, "%s, internal clock.\n", precord->name);
	else
		kfwPrint(0, "%s, external clock.\n", precord->name);

	if( drvACQ196_set_cardMode(pSTDdev) == WR_ERROR )
		notify_error(ERR_PNT, "%s mode set error", pSTDdev->taskName);
	
}
static void devACQ196_BO_INIT_FILE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	drvACQ196_init_file(pSTDdev);
	
}



static void devACQ196_AO_MASK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	drvACQ196_set_channelMask(pSTDdev, pParam->setValue);
	epicsPrintf("control thread (Mask): %s %s %d (%s)\n", pSTDdev->taskName, precord->name, (int)pParam->setValue,
			                                   epicsThreadGetNameSelf());
}

static void devACQ196_AO_R_ONOFF(ST_execParam *pParam)
{
	int i, ntemp, CntOn=0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	int dval = (int)pParam->setValue;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		pAcq196->channelOnOff[i] = ( dval & ntemp ) ? 1 : 0;

		if( pAcq196->channelOnOff[i] ) {
			CntOn++;
/*			epicsPrintf(" %s:  ch%d ON\n", pSTDdev->taskName, i );
			epicsThreadSleep(0.1); */
		}
	}
	pAcq196->boardConfigure |= ACQ196_SET_R_BITMASK;
	drvACQ196_notify_InitCondition( pSTDdev );
	epicsPrintf("%s: %s:(1-32) on/off - %d/%d\n", pSTDdev->taskName, precord->name, CntOn, 32-CntOn);
}


static void devACQ196_AO_M_ONOFF(ST_execParam *pParam)
{
	int i, ntemp, CntOn=0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	int dval = (int)pParam->setValue;

	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		pAcq196->channelOnOff[32+i] = ( dval & ntemp ) ? 1 : 0;

		if( pAcq196->channelOnOff[32+i] ) {
			CntOn++;
/*			epicsPrintf(" %s:  ch%d ON\n", pSTDdev->taskName, 32+i );
			epicsThreadSleep(0.1); */
		}

	}
	pAcq196->boardConfigure |= ACQ196_SET_M_BITMASK;
	drvACQ196_notify_InitCondition( pSTDdev );
	epicsPrintf("%s: %s:(33-64) on/off - %d/%d\n", pSTDdev->taskName, precord->name, CntOn, 32-CntOn);
}


static void devACQ196_AO_L_ONOFF(ST_execParam *pParam)
{
	int i, ntemp, CntOn=0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	int dval = (int)pParam->setValue;

	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		pAcq196->channelOnOff[64+i] = ( dval & ntemp ) ? 1 : 0;

		if( pAcq196->channelOnOff[64+i] ) {
			CntOn++;
/*			epicsPrintf(" %s:  ch%d ON\n", pSTDdev->taskName, 64+i );
			epicsThreadSleep(0.1); */
		}

	}
	pAcq196->boardConfigure |= ACQ196_SET_L_BITMASK;
	drvACQ196_notify_InitCondition( pSTDdev );
	epicsPrintf("%s: %s:(65-96) on/off - %d/%d\n", pSTDdev->taskName, precord->name, CntOn, 32-CntOn);
}

static void devACQ196_AO_MAXDMA(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pAcq196->n32_maxdma = (uint32)pParam->setValue;
	if( acq196_set_maxdma(pSTDdev) == WR_OK ) {
		epicsPrintf("\n>>> %s: %s ( %d )\n", pSTDdev->taskName, precord->name, pAcq196->n32_maxdma);
	}
}

static void devACQ196_AO_CLOCK_DIV(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if( pParam->setValue == 0 ) {
		epicsPrintf("\n>>> %s: must be bigger than zero ( %f )\n", precord->name, pParam->setValue);
		return;
	}
	pAcq196->clockDivider = (uint32)pParam->setValue;
	epicsPrintf("\n>>> %s: %s ( %d )\n", pSTDdev->taskName, precord->name, pAcq196->clockDivider);
}

static void devACQ196_AO_CARD_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	int nval = (int)pParam->setValue;

	if( nval < CARD_MODE_SINGLE || nval > CARD_MODE_SLAVE) {
		notify_error(ERR_PNT, "%s (%d).. error.\n", precord->name, nval );
		printlog("\nERROR: %s (%d)\n", precord->name, nval );
		epicsPrintf("Current card mode: %s (%d)\n", precord->name, pAcq196->nCardMode  );
		return;
	}

	pAcq196->nCardMode = nval;
	switch( pAcq196->nCardMode ) {
		case CARD_MODE_SINGLE: 			
			kfwPrint(0, "%s (Single)\n", precord->name );
			break;
		case CARD_MODE_MASTER:
			kfwPrint(0, "%s (Master)\n", precord->name );
			break;
		case CARD_MODE_SLAVE:
			kfwPrint(0, "%s (Slave)\n", precord->name );
			break;
		default:
			kfwPrint(0, "%s (%d)\n", precord->name, pAcq196->nCardMode  );
			break;
	}

	if( drvACQ196_set_cardMode(pSTDdev) == WR_ERROR )
		notify_error(ERR_PNT, "%s mode set error", pSTDdev->taskName);
	
}

static void devACQ196_AO_SAMPLING_RATE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	uint32 prev = pSTDdev->ST_Base.nSamplingRate;
	pSTDdev->ST_Base.nSamplingRate = (unsigned int)pParam->setValue;
/*
	if( drvACQ196_set_cardMode(pSTDdev) == WR_ERROR )
		notify_error(ERR_PNT, "%s mode set error", pSTDdev->taskName);
*/
	printf("%s: sampling rate changed from %dHz to %dHz.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);

	if( pAcq196->clockSource == CLOCK_INTERNAL ) 
	{
		if( acq196_set_internalClk_DO0(pSTDdev) == WR_ERROR)
		{
			pSTDdev->ST_Base.nSamplingRate = prev;
			printf("%s: internal clock set error. return to %dHz.\n", pSTDdev->taskName, pSTDdev->ST_Base.nSamplingRate);
		}

	}

}

static void devACQ196_AO_START_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
//	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pSTDdev->ST_Base.dT0[0] = pParam->setValue;

	epicsPrintf("%s: %s  %f\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dT0[0]);
}
static void devACQ196_AO_STOP_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
//	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pSTDdev->ST_Base.dT1[0] = pParam->setValue;
	pSTDdev->ST_Base.dRunPeriod = pSTDdev->ST_Base.dT1[0] - pSTDdev->ST_Base.dT0[0];

	epicsPrintf("%s: %s  %f, runTime: %f\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dT1[0], pSTDdev->ST_Base.dRunPeriod );
}
static void devACQ196_AO_RUN_PERIOD(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
//	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pSTDdev->ST_Base.dRunPeriod = pParam->setValue;
	pSTDdev->ST_Base.dT1[0] = pSTDdev->ST_Base.dT0[0] + pSTDdev->ST_Base.dRunPeriod;
//	pSTDdev->ST_Base.dT1[0] = pSTDdev->ST_Base.dRunPeriod;


	epicsPrintf("%s: %s  %f\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dRunPeriod);

}

static void devAoACQ196_AO_TEST_2(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}


static void devACQ196_STRINGOUT_TAG(ST_execParam *pParam)
{
	int i;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
	if( pParam->n32Arg0 > 96 ) {
		epicsPrintf("\n>>> %d,  input channel limit : %d\n", pParam->n32Arg0, 96);
		return;
	}

/*	strcpy(channelTagName[pParam->n32Arg0], "\\");
	strcat(channelTagName[pParam->n32Arg0], pParam->setStr);
*/
/*	strcpy(channelTagName[pParam->n32Arg0], pParam->setStr); */
	strcpy(pAcq196->strTagName[pParam->n32Arg0], pParam->setStr);
/*	pAcq196->cTouchTag[pParam->n32Arg0] = 0x1;*/
	drvACQ196_save_file(pSTDdev);


#if 0
	epicsPrintf("control thread (tag Name): %s (%d: %s)\n", pSTDdev->taskName, pParam->n32Arg0, pParam->setStr );
#endif
}



/*********************************************************
 initial setup of records 
*********************************************************/
static long devBoACQ196_init_record(boRecord *precord)
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
				recGblRecordError(S_db_badField, (void*) precord,  "devBoACQ196 (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoACQ196 (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(arg0, BO_CH_MASK_STR)) {
		pDpvt->ind = BO_CH_MASK;
		pDpvt->n32Arg0 = strtol(arg1,NULL,0);
	} 
	else if (!strcmp(arg0, BO_CLOCK_MODE_STR)) {
		pDpvt->ind = BO_CLOCK_MODE;
	} 
	else if (!strcmp(arg0, BO_INIT_FILE_STR)) {
		pDpvt->ind = BO_INIT_FILE;
	} 
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoACQ196_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoACQ196_write_bo(boRecord *precord)
{
	ST_dpvt		*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg 	*pControlThreadConfig;
	ST_threadQueueData	qData;
	ST_ACQ196 *pACQ196;

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

	pACQ196 = (ST_ACQ196*)pSTDdev->pUser;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) {

			case BO_CH_MASK:
				qData.pFunc = devACQ196_BO_CH_MASK;
				break;
			case BO_CLOCK_MODE:
				qData.pFunc = devACQ196_BO_CLOCK_MODE;
				break;
			case BO_INIT_FILE:
				qData.pFunc = devACQ196_BO_INIT_FILE;
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
			case BO_CH_MASK: 
				precord->val = pACQ196->channelOnOff[pDpvt->n32Arg0];
//				printf("Arg:%d   value: %d\n", pDpvt->n32Arg0, pACQ196->channelOnOff[pDpvt->n32Arg0] );
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


static long devBiACQ196_init_record(biRecord *precord)
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
						  "devBiACQ196 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiACQ196 (init_record) Illegal INP field");
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
		epicsPrintf("ERROR!! devBiACQ196_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiACQ196_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devBiACQ196_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pACQ196;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pACQ196 = (ST_ACQ196*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case BI_TEST_1: 
			/* precord->val = your value */
			break;
		case BI_TEST_2: 
			/* precord->val = your value */
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... ACQ196 \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoACQ196_init_record(aoRecord *precord)
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
						  "devAoACQ196 (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoACQ196 (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, AO_MASK_STR)) 
		pDpvt->ind = AO_MASK;
	else if (!strcmp(arg0, AO_R_ONOFF_STR)) 
		pDpvt->ind = AO_R_ONOFF;
	else if (!strcmp(arg0, AO_M_ONOFF_STR)) 
		pDpvt->ind = AO_M_ONOFF;
	else if (!strcmp(arg0, AO_L_ONOFF_STR))
		pDpvt->ind = AO_L_ONOFF;
	else if (!strcmp(arg0, AO_MAXDMA_STR)) 
		pDpvt->ind = AO_MAXDMA;
	else if (!strcmp(arg0, AO_CLOCK_DIV_STR)) 
		pDpvt->ind = AO_CLOCK_DIV;
	else if (!strcmp(arg0, AO_CARD_MODE_STR)) 
		pDpvt->ind = AO_CARD_MODE;
	else if (!strcmp(arg0, AO_SAMPLING_RATE_STR)) 
		pDpvt->ind = AO_SAMPLING_RATE;
	else if (!strcmp(arg0, AO_START_TIME_STR)) 
		pDpvt->ind = AO_START_TIME;
	else if (!strcmp(arg0, AO_RUN_PERIOD_STR)) 
		pDpvt->ind = AO_RUN_PERIOD;
	else if (!strcmp(arg0, AO_STOP_TIME_STR)) 
		pDpvt->ind = AO_STOP_TIME;


	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAoACQ196_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoACQ196_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_ACQ196 *pACQ196;
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

	pACQ196 = (ST_ACQ196*)pSTDdev->pUser;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_MASK:
				qData.pFunc = devACQ196_AO_MASK;
				break;
			case AO_R_ONOFF:
				qData.pFunc = devACQ196_AO_R_ONOFF;
				break;
			case AO_M_ONOFF:
				qData.pFunc = devACQ196_AO_M_ONOFF;
				break;
			case AO_L_ONOFF:
				qData.pFunc = devACQ196_AO_L_ONOFF;
				break;
			case AO_MAXDMA:
				qData.pFunc = devACQ196_AO_MAXDMA;
				break;
			case AO_CLOCK_DIV:
				qData.pFunc = devACQ196_AO_CLOCK_DIV;
				break;
			case AO_CARD_MODE:
				qData.pFunc = devACQ196_AO_CARD_MODE;
				break;
			case AO_SAMPLING_RATE:
				qData.pFunc = devACQ196_AO_SAMPLING_RATE;
				break;
			case AO_START_TIME:
				qData.pFunc = devACQ196_AO_START_TIME;
				break;
			case AO_STOP_TIME:
				qData.pFunc = devACQ196_AO_STOP_TIME;
				break;
			case AO_RUN_PERIOD:
				qData.pFunc = devACQ196_AO_RUN_PERIOD;
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
			case AO_CARD_MODE:
				precord->val = pACQ196->nCardMode;
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



static long devAiACQ196_init_record(aiRecord *precord)
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
						  "devAiACQ196 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiACQ196 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, AI_READ_STATE_STR))
		pDpvt->ind = AI_READ_STATE;
	
	else if (!strcmp(arg0, AI_PARSING_CNT_STR)) 
		pDpvt->ind = AI_PARSING_CNT;
	else if (!strcmp(arg0, AI_MDS_SND_CNT_STR)) 
		pDpvt->ind = AI_MDS_SND_CNT;
	else if (!strcmp(arg0, AI_DAQING_CNT_STR)) 
		pDpvt->ind = AI_DAQING_CNT;

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAiACQ196_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiACQ196_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	if( pDpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else 
		*ioScanPvt  = pSTDdev->ioScanPvt_userCall;

	return 0;
}

static long devAiACQ196_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pACQ196;


	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;
	pACQ196 = (ST_ACQ196*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_READ_STATE: 
			precord->val = pSTDdev->StatusDev;
			break;
		case AI_PARSING_CNT: 
			precord->val = pACQ196->parsing_remained_cnt;
			break;
		case AI_MDS_SND_CNT: 
			precord->val = pACQ196->mdsplus_snd_cnt;
			break;
		case AI_DAQING_CNT: 
			precord->val = pACQ196->daqing_cnt;
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... ACQ196 \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}

static long devStringoutACQ196_init_record(stringoutRecord *precord)
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
#if PRINT_ACQ196_DEVSUP_ARG
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


	if(!strcmp(arg0, STRINGOUT_TAG_STR)) {
		pDpvt->ind = STRINGOUT_TAG;
		pDpvt->n32Arg0 = strtol(arg1,NULL,0);
	}  
	else if (!strcmp(arg0, "xxx")) {
		pDpvt->ind = 999;
	}  
	else {
		pDpvt->ind = -1;
		epicsPrintf("[ACQ196] devStringoutACQ196_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}


static long devStringoutACQ196_write_stringout(stringoutRecord *precord)
{
	int len;
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER          *pMaster;
	ST_STD_device		*pSTDdev;
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
/*	pACQ196 = (ST_ACQ196*)pSTDdev->pUser; */

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
#if PRINT_ACQ196_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif			
		switch(pDpvt->ind) 
		{
			case STRINGOUT_TAG:
				qData.pFunc = devACQ196_STRINGOUT_TAG;
				qData.param.n32Arg0 = pDpvt->n32Arg0;
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
#if PRINT_ACQ196_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1; /*(-1,0)=>(failure,success)*/
}


static long devMbbiACQ196_init_record(mbbiRecord *precord)
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
						  "devAiACQ196 (init_record) Illegal INP field: task name");
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

	
	if(!strcmp(arg0, MBBI_DEV_STATUS_STR))
		pDpvt->ind = MBBI_DEV_STATUS;

	else {
		pDpvt->ind = -1;
		epicsPrintf("[ACQ196] devMbbiACQ196_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;   /*returns: (-1,0)=>(failure,success)*/
}


static long devMbbiACQ196_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pSTDdev = pDpvt->pSTDdev;

	*ioScanPvt  = pSTDdev->ioScanPvt_status;

	return 0;
	
}

static long devMbbiACQ196_read_mbbi(mbbiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pAcq196;

	if(!pDpvt) return -1;

	pSTDdev = pDpvt->pSTDdev;
	pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case MBBI_DEV_STATUS:
			precord->rval = pSTDdev->StatusDev;
			if( pSTDdev->StatusDev & TASK_STANDBY ) 
			{
				if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) precord->val = 1; /* ready to run */
				else if( pSTDdev->StatusDev & TASK_DATA_TRANSFER ) precord->val = 6;
				else if( pSTDdev->StatusDev & TASK_POST_PROCESS ) precord->val = 5;
				else if( pSTDdev->StatusDev & TASK_IN_PROGRESS) precord->val = 4;
				else if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) precord->val = 3;
				else if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) precord->val = 2;
				else precord->val = 8; /* ready but busy with mystery, or maybe expressed error! (???) */
			} else {
				if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) precord->val = 7; /* not ready, Idle */
				else precord->val = 0; /* not initiated or NULL */
			} 
/*			epicsPrintf("%s Status: 0x%X, rval: %d, val: %d \n", pACQ196Cfg->taskName, pACQ196Cfg->StatusACQ196, precord->rval, precord->val);  */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... _ACQ196 \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=>(success, success no convert)*/
}



