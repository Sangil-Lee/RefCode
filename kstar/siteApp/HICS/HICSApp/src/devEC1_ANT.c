/****************************************************************************

Module      : devEC1_ANT.c

Copyright(c): 2013 NFRI. All Rights Reserved.

Revision History:
Author: yunsw   2013-04-08

 *****************************************************************************/

#include <termios.h>	// for baud rate

#include "aiRecord.h"
#include "aoRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

/**********************************
  user include files
 ***********************************/

#include "drvEC1_ANT.h"

#define BI_RFM_STATUS			1
#define BI_CTRL_STATUS			2
#define BI_RFM_STATUS_STR		"rfm_comm"
#define BI_CTRL_STATUS_STR		"ctrl_comm"

#define AI_TEST_1				1
#define AI_TEST_1_STR			"ai1"

#define BO_STS_SIM				1
#define BO_STS_SIM_STR			"sts_sim"

#define AO_STS_RATE				1
#define AO_STS_RATE_STR			"sts_rate"
#define AO_CMD_RATE				2
#define AO_CMD_RATE_STR			"cmd_rate"
#define AO_ECH_CONT_PEC			3
#define AO_ECH_CONT_PEC_STR		"ech_cont_pec"

#define SI_TEST					1
#define SI_TEST_STR				"si_test"

#define SO_COM_PORT_NAME		1
#define SO_COM_BAUD_RATE		2
#define SO_COM_PORT_NAME_STR	"com_port"
#define SO_COM_BAUD_RATE_STR	"com_baud"

/*******************************************************/

static void devBoEC1_ANT_STS_SIM (ST_execParam *pParam)
{
	struct dbCommon *precord	= pParam->precord;
	ST_STD_device	*pSTDdev	= pParam->pSTDdev;
	ST_EC1_ANT		*pEC1_ANT	= (ST_EC1_ANT *)pSTDdev->pUser;

	pEC1_ANT->bDbSimulation	= pParam->setValue;

	kuDebug (kuMON, "%s : %d\n", precord->name, (int)pParam->setValue);
}

static void devAoEC1_ANT_STS_RATE (ST_execParam *pParam)
{
	struct dbCommon *precord	= pParam->precord;
	ST_STD_device	*pSTDdev	= pParam->pSTDdev;
	ST_EC1_ANT		*pEC1_ANT	= (ST_EC1_ANT *)pSTDdev->pUser;

	if (0 < pParam->setValue) {
		pEC1_ANT->dDbStsRate	= pParam->setValue;
	}

	kuDebug (kuMON, "%s : %g\n", precord->name, pParam->setValue);
}

static void devAoEC1_ANT_CMD_RATE (ST_execParam *pParam)
{
	struct dbCommon *precord	= pParam->precord;
	ST_STD_device	*pSTDdev	= pParam->pSTDdev;
	ST_EC1_ANT		*pEC1_ANT	= (ST_EC1_ANT *)pSTDdev->pUser;

	// if rate is 0, then controller is schedule by external clock through digitizer
	pEC1_ANT->dDbCmdRate	= pParam->setValue;

	kuDebug (kuMON, "%s : %g\n", precord->name, pParam->setValue);
}

static void devAoECH_CONT_PEC (ST_execParam *pParam)
{
	struct dbCommon *precord	= pParam->precord;

	kuDebug (kuDEBUG, "%s : %g\n", precord->name, pParam->setValue);
}

static void devSoEC1_ANT_COM_PORT_NAME (ST_execParam *pParam)
{
	struct dbCommon *precord	= pParam->precord;
	ST_STD_device	*pSTDdev	= pParam->pSTDdev;
	ST_EC1_ANT		*pEC1_ANT	= (ST_EC1_ANT *)pSTDdev->pUser;

	strcpy (pEC1_ANT->szDbPortName, pParam->setStr);

	kuDebug (kuMON, "%s : %s\n", precord->name, pParam->setStr);
}

static void devSoEC1_ANT_COM_BAUD_RATE (ST_execParam *pParam)
{
	struct dbCommon *precord	= pParam->precord;
	ST_STD_device	*pSTDdev	= pParam->pSTDdev;
	ST_EC1_ANT		*pEC1_ANT	= (ST_EC1_ANT *)pSTDdev->pUser;

	if (0 == strcmp (pParam->setStr, "B9600"))			pEC1_ANT->iDbBaudRate	= B9600;
	else if (0 == strcmp (pParam->setStr, "B19200"))	pEC1_ANT->iDbBaudRate	= B19200;
	else if (0 == strcmp (pParam->setStr, "B38400"))	pEC1_ANT->iDbBaudRate	= B38400;
	else if (0 == strcmp (pParam->setStr, "B57600"))	pEC1_ANT->iDbBaudRate	= B57600;
	else if (0 == strcmp (pParam->setStr, "B115200"))	pEC1_ANT->iDbBaudRate	= B115200;

	kuDebug (kuMON, "%s : %s\n", precord->name, pParam->setStr);
}

/*********************************************************
  initial setup of records 
 *********************************************************/
static long devBoEC1_ANT_init_record (boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) calloc (1, sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s %s", pSTdpvt->devName, arg0, arg1);
			pSTdpvt->pSTDdev = get_STDev_from_name(pSTdpvt->devName);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBoEC1_ANT (init_record) Illegal OUT field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoEC1_ANT (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp(arg0, BO_STS_SIM_STR)) {
		pSTdpvt->ind	= BO_STS_SIM;
	} 
	else {
		epicsPrintf("ERROR! devBoEC1_ANT_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoEC1_ANT_write_bo (boRecord *precord)
{
	ST_dpvt				*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg		*pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev					= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue	= (double)precord->val;
	qData.param.n32Arg0		= pSTdpvt->n32Arg0;

	/* db processing: phase I */
	if (precord->pact == FALSE) {	
		precord->pact = TRUE;

		switch(pSTdpvt->ind) {
			case BO_STS_SIM :
				qData.pFunc = devBoEC1_ANT_STS_SIM;
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
	if (precord->pact == TRUE) {
		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pSTdpvt->ind) {
			case BO_STS_SIM : 
				/*do something */
				/*precord->val = 0; */
				break;
			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

static long devBiEC1_ANT_init_record(biRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) calloc (1, sizeof(ST_dpvt));
	int nval;

	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s", pSTdpvt->devName, arg0, arg1);

			pSTdpvt->pSTDdev = get_STDev_from_name(pSTdpvt->devName);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devBiEC1_ANT (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devBiEC1_ANT (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	pSTdpvt->ind = -1;

	if (!strcmp(arg0, BI_RFM_STATUS_STR)) {
		pSTdpvt->ind = BI_RFM_STATUS;
	}
	else if (!strcmp(arg0, BI_CTRL_STATUS_STR)) {
		pSTdpvt->ind = BI_CTRL_STATUS;
	}
	else {
		epicsPrintf("ERROR!! devBiEC1_ANT_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiEC1_ANT_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt			*pSTdpvt	= (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev		= pSTdpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devBiEC1_ANT_read_bi(biRecord *precord)
{ 
	ST_dpvt			*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev;
	ST_EC1_ANT		*pEC1_ANT;

	if(!pSTdpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev		= pSTdpvt->pSTDdev;
	pEC1_ANT	= (ST_EC1_ANT*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case BI_RFM_STATUS: 
			precord->val = pEC1_ANT->bDbRfmStatus;
			break;

		case BI_CTRL_STATUS: 
			precord->val = pEC1_ANT->bDbCtrlStatus;
			break;

		default:
			epicsPrintf("\nERROR: %s: ind( %d )... EC1_ANT \n", pSTDdev->taskName, pSTdpvt->ind);
			break;
	}

	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoEC1_ANT_init_record (aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) calloc (1, sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s %s %s", pSTdpvt->devName, arg0, arg1, arg2);

			pSTdpvt->pSTDdev = get_STDev_from_name(pSTdpvt->devName);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devAoEC1_ANT (init_record) Illegal OUT field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoEC1_ANT (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (arg0, AO_STS_RATE_STR)) {
		pSTdpvt->ind = AO_STS_RATE;
	}
	else if (!strcmp(arg0, AO_CMD_RATE_STR)) {
		pSTdpvt->ind = AO_CMD_RATE;
	}
	else if (!strcmp(arg0, AO_ECH_CONT_PEC_STR)) { // not used
		pSTdpvt->ind = AO_ECH_CONT_PEC;
	}
	else {
		epicsPrintf("ERROR!! devAoEC1_ANT_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoEC1_ANT_write_ao(aoRecord *precord)
{
	ST_dpvt				*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_EC1_ANT			*pEC1_ANT;
	ST_threadCfg		*pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev					= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;

	pEC1_ANT = (ST_EC1_ANT*)pSTDdev->pUser;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		switch(pSTdpvt->ind) {
			case AO_STS_RATE :
				qData.pFunc = devAoEC1_ANT_STS_RATE;
				break;
			case AO_CMD_RATE :
				qData.pFunc = devAoEC1_ANT_CMD_RATE;
				break;
			case AO_ECH_CONT_PEC :
				qData.pFunc = devAoECH_CONT_PEC;
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
	if(precord->pact == TRUE) {
		switch(pSTdpvt->ind) {
			case AO_STS_RATE :
				/*do something */
				break;
			case AO_CMD_RATE :
				/*do something */
				break;

			default:
				break;
		}

		precord->pact	= FALSE;
		precord->udf	= FALSE;
		return 0; /*(0)=>(success ) */
	}

	return -1;
}

static long devAiEC1_ANT_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) calloc (1, sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s %s", pSTdpvt->devName, arg0, arg1, arg2);

			pSTdpvt->pSTDdev = get_STDev_from_name(pSTdpvt->devName);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiEC1_ANT (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiEC1_ANT (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if (!strcmp(arg0, AI_TEST_1_STR)) {
		pSTdpvt->ind = AI_TEST_1;
	}
	else {
		pSTdpvt->ind = -1;
		epicsPrintf("ERROR!! devAiEC1_ANT_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}

static long devAiEC1_ANT_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt			*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev		= pSTdpvt->pSTDdev;
	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiEC1_ANT_read_ai(aiRecord *precord)
{ 
	ST_dpvt			*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev;
	ST_EC1_ANT		*pEC1_ANT;

	if(!pSTdpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev		= pSTdpvt->pSTDdev;
	pEC1_ANT	= (ST_EC1_ANT*)pSTDdev->pUser;

	switch(pSTdpvt->ind) {
		case AI_TEST_1: 
			precord->val = 1;
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... EC1_ANT \n", pSTDdev->taskName, pSTdpvt->ind);
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}

static long devSiEC1_ANT_init_record(stringinRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) calloc (1, sizeof(ST_dpvt));
	int nval;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s", pSTdpvt->devName, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringin (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
				   
			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringin (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind = -1;

	if (!strcmp (pSTdpvt->arg0, SI_TEST_STR)) {
		pSTdpvt->ind	= SI_TEST;
	}  
	else {
		kuDebug (kuERR, "[SFW] devStringinSFW_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0);
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;
	
	return 0;    /*returns: (-1,0)=>(failure,success)*/
}

static long devSiEC1_ANT_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt			*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return -1;
	}

	pSTDdev		= pSTdpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_status;

	return 0;
}

static long devSiEC1_ANT_read_si(stringinRecord *precord)
{ 
	ST_dpvt			*pSTdpvt	= (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev	= NULL;
	ST_EC1_ANT		*pEC1_ANT	= NULL;

	if(!pSTdpvt) return -1; /*returns: (-1,0)=>(failure,success)*/

	pSTDdev		= pSTdpvt->pSTDdev;
	pEC1_ANT	= (ST_EC1_ANT *)pSTDdev->pUser;

	static int i = 0;
	char buf[64];

	switch (pSTdpvt->ind) {
		case SI_TEST :
			sprintf (buf, "%05d", i++);
			strcpy (precord->val, buf);
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... _SFW \n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (0); /*returns: (-1,0)=>(failure,success)*/
}

static long devSoEC1_ANT_init_record(stringoutRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) calloc (1, sizeof(ST_dpvt));
	int nval;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s", pSTdpvt->devName, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_STDev_from_name(pSTdpvt->devName);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringout (init_record) Illegal OUT field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringout (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(!strcmp(pSTdpvt->arg0, SO_COM_PORT_NAME_STR)) {
		pSTdpvt->ind = SO_COM_PORT_NAME;
	}  
	else if(!strcmp(pSTdpvt->arg0, SO_COM_BAUD_RATE_STR)) {
		pSTdpvt->ind = SO_COM_BAUD_RATE;
	}  
	else {
		pSTdpvt->ind = -1;
		epicsPrintf("devSoEC1_ANT_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}

static long devSoEC1_ANT_write_so(stringoutRecord *precord)
{
	ST_dpvt				*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg		*pControlThreadConfig;
	ST_threadQueueData	qData;

	if (!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}

	pSTDdev					= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;

	qData.pFunc             = NULL;
	qData.param.pSTDdev     = pSTDdev;
	qData.param.precord     = (struct dbCommon *)precord;

	strncpy (qData.param.setStr, precord->val, SIZE_STRINGOUT_VAL-1);

 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		switch(pSTdpvt->ind) {
			case SO_COM_PORT_NAME :
				qData.pFunc = devSoEC1_ANT_COM_PORT_NAME;
				break;

			case SO_COM_BAUD_RATE :
				qData.pFunc = devSoEC1_ANT_COM_BAUD_RATE;
				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId, (void*) &qData, sizeof(ST_threadQueueData));

		return 0; /*(-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		precord->pact	= FALSE;
		precord->udf	= FALSE;
		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1; /*(-1,0)=>(failure,success)*/
}

BINARYDSET	devBoEC1_ANT = { 6, NULL, NULL, devBoEC1_ANT_init_record, NULL, devBoEC1_ANT_write_bo, NULL };
BINARYDSET	devAoEC1_ANT = { 6, NULL, NULL, devAoEC1_ANT_init_record, NULL, devAoEC1_ANT_write_ao, NULL };
BINARYDSET	devAiEC1_ANT = { 6, NULL, NULL, devAiEC1_ANT_init_record, devAiEC1_ANT_get_ioint_info, devAiEC1_ANT_read_ai, NULL };
BINARYDSET	devBiEC1_ANT = { 6, NULL, NULL, devBiEC1_ANT_init_record, devBiEC1_ANT_get_ioint_info, devBiEC1_ANT_read_bi, NULL };
BINARYDSET	devSoEC1_ANT = { 6, NULL, NULL, devSoEC1_ANT_init_record, NULL, devSoEC1_ANT_write_so, NULL };
BINARYDSET	devSiEC1_ANT = { 6, NULL, NULL, devSiEC1_ANT_init_record, devSiEC1_ANT_get_ioint_info, devSiEC1_ANT_read_si, NULL };

epicsExportAddress(dset, devBoEC1_ANT);
epicsExportAddress(dset, devAoEC1_ANT);
epicsExportAddress(dset, devAiEC1_ANT);
epicsExportAddress(dset, devBiEC1_ANT);
epicsExportAddress(dset, devSiEC1_ANT);
epicsExportAddress(dset, devSoEC1_ANT);

