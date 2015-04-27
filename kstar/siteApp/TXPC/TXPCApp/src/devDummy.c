
#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"

#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drvDummy.h" 



/*******************************************************/

static long devBoDummy_init_record(boRecord *precord);
static long devBoDummy_write_bo(boRecord *precord);

static long devAiDummy_init_record(aiRecord *precord);
static long devAiDummy_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiDummy_read_ai(aiRecord *precord);


BINARYDSET devBoDummy = { 6,  NULL,  NULL,   devBoDummy_init_record,  NULL,  devBoDummy_write_bo,  NULL };
BINARYDSET  devAiDummy = { 6, NULL, NULL, devAiDummy_init_record, devAiDummy_get_ioint_info, devAiDummy_read_ai, NULL };



epicsExportAddress(dset, devBoDummy);
epicsExportAddress(dset, devAiDummy);



/*******************************************************/


static long devBoDummy_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %d", pDpvt->devName, pDpvt->arg0, &pDpvt->n32Arg0);
#if 0
	epicsPrintf("devBoACQ196Control arg num: %d: %s %s\n",i, devBoDummy->devName, devBoDummy->arg0);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBoDummy (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoDummy (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(pDpvt->arg0, "aaa")) {
		pDpvt->ind = 1;
/*		pDpvt->n32Arg0 = strtol(pDpvt->arg1,NULL,0); */
	} 
	else if(!strcmp(pDpvt->arg0, "bbb")) 
		pDpvt->ind = 1;

	else if(!strcmp(pDpvt->arg0, "ccc")) 
		pDpvt->ind = 1;
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoDummy_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoDummy_write_bo(boRecord *precord)
{
#if 0
	static int   kkh_cnt;
#endif
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
		
#if 0
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) {
/*
			case BO_DIO24_GPIO_PORTA_DIR:
				qData.pFunc = devBoDummy_BO_DIO24_GPIO_PORT_A_DIR;
				break;
			case BO_DIO24_GPIO_PORTB_DIR:
				qData.pFunc = devBoDummy_BO_DIO24_GPIO_PORT_B_DIR;
				break;
			case BO_DIO24_GPIO_PORTC_ALL_DIR:
				qData.pFunc = devBoDummy_BO_DIO24_GPIO_PORT_C_DIR;
				break;
			case BO_DIO24_GPIO_PORTC_DIR:
				qData.pFunc = devBoDummy_BO_DIO24_GPIO_PORT_C_single_DIR;
				break;

			case BO_DIO24_PUT_PORT_A:
				qData.pFunc = devBoDummy_BO_DIO24_PUT_PORT_A;
				break;
			case BO_DIO24_PUT_PORT_B:
				qData.pFunc = devBoDummy_BO_DIO24_PUT_PORT_B;
				break;
			case BO_DIO24_PUT_PORT_C:
				qData.pFunc = devBoDummy_BO_DIO24_PUT_PORT_C;
				break;
*/

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

#if 0
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
#if 0
		switch(pdevACQ196Controldpvt->ind) {
			case AO_SET_INTERCLK: precord->val = 0; break;
			case AO_SET_EXTERCLK: precord->val = 0; break;
			case AO_DEV_RUN: precord->val = 0; break;
			case AO_DAQ_STOP: precord->val = 0; break;
			case AO_DEV_ARMING: precord->val = 0; break;
			case AO_ADC_STOP: precord->val = 0; break;
			case AO_LOCAL_STORAGE: precord->val = 0; break;
			case AO_REMOTE_STORAGE: precord->val = 0; break;
			case AO_SET_CLEAR_STATUS: precord->val = 0; break;
		}
#endif
		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

static long devAiDummy_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pDpvt->devName,
				   pDpvt->arg0,
				   pDpvt->arg1);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiDummy (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiDummy (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pDpvt->arg0, "aaa"))
		pDpvt->ind = 0;
	else if(!strcmp(pDpvt->arg0, "bbb"))
		pDpvt->ind = 0;
	else if(!strcmp(pDpvt->arg0, "ccc"))
		pDpvt->ind = 0;
	else {
		pDpvt->ind = -1;
		epicsPrintf("_Dummy devAiDummy_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiDummy_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiDummy_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_Dummy *pDummy;
//	uint32 status;
	unsigned long get = 0;

	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pDummy = (ST_Dummy*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... _Dummy \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}



