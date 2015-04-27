
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



#include "drvDio24.h" 
#include "pv_list.h" 


/*******************************************************/

static long devBoDio24_init_record(boRecord *precord);
static long devBoDio24_write_bo(boRecord *precord);

static long devAiDio24_init_record(aiRecord *precord);
static long devAiDio24_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiDio24_read_ai(aiRecord *precord);


BINARYDSET devBoDio24 = { 6,  NULL,  NULL,   devBoDio24_init_record,  NULL,  devBoDio24_write_bo,  NULL };
BINARYDSET  devAiDio24 = { 6, NULL, NULL, devAiDio24_init_record, devAiDio24_get_ioint_info, devAiDio24_read_ai, NULL };



epicsExportAddress(dset, devBoDio24);
epicsExportAddress(dset, devAiDio24);



/*******************************************************/

static void devBoDio24_BO_DIO24_GPIO_PORT_A_DIR(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;

	if ( (uint32)pParam->setValue  > 1 ) {
		printf(">>> ERROR wrong value!\n");
		return;
	}

	pDio24->portA_io = (uint32)pParam->setValue;

	if( DIO24_GPIO_DIRECTION__SET(pDio24->dev, DIO24_WHICH_PORT_A, pDio24->portA_io) != GSC_SUCCESS )
	{
		notify_error(1, "%s: PORT_A direction: %d\n", pSTDdev->taskName, pDio24->portA_io );
		return;
	}
	
	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );

}

static void devBoDio24_BO_DIO24_GPIO_PORT_B_DIR(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;

	if ( (uint32)pParam->setValue  > 1 ) {
		printf(">>> ERROR wrong value!\n");
		return;
	}

	pDio24->portB_io = (uint32)pParam->setValue;

	if( DIO24_GPIO_DIRECTION__SET(pDio24->dev, DIO24_WHICH_PORT_B, pDio24->portB_io) != GSC_SUCCESS )
	{
		notify_error(1, "%s: PORT_B direction: %d\n", pSTDdev->taskName, pDio24->portB_io );
		return;
	}
	
	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );

}

static void devBoDio24_BO_DIO24_GPIO_PORT_C_DIR(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;

	if ( (uint32)pParam->setValue  > 1 ) {
		printf(">>> ERROR wrong value!\n");
		return;
	}

	pDio24->portC_io = (uint32)pParam->setValue;

	if( DIO24_GPIO_DIRECTION__SET(pDio24->dev, DIO24_WHICH_PORT_C, pDio24->portC_io) != GSC_SUCCESS )
	{
		notify_error(1, "%s: PORT_C direction: %d\n", pSTDdev->taskName, pDio24->portC_io );
		return;
	}
	
	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
	
}

static void devBoDio24_BO_DIO24_GPIO_PORT_C_single_DIR(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;
	uint32 status;
	uint32 port;

	if ( (uint32)pParam->setValue  > 1 ) {
		printf(">>> ERROR wrong value!\n");
		return;
	}
	switch( pParam->n32Arg0 ) {
		case 0: port = DIO24_WHICH_PORT_C0; break;
		case 1: port = DIO24_WHICH_PORT_C1; break;
		case 2: port = DIO24_WHICH_PORT_C2; break;
		case 3: port = DIO24_WHICH_PORT_C3; break;
		case 4: port = DIO24_WHICH_PORT_C4; break;
		case 5: port = DIO24_WHICH_PORT_C5; break;
		case 6: port = DIO24_WHICH_PORT_C6; break;
		case 7: port = DIO24_WHICH_PORT_C7; break;
		default: 
			notify_error(1, "%s: select PORTC", pSTDdev->taskName);
			return;
	}
	pDio24->portCX_io[ pParam->n32Arg0] = (uint32)pParam->setValue;

	status	= DIO24_GPIO_DIRECTION__SET(pDio24->dev, port, pDio24->portCX_io[ pParam->n32Arg0]);
	if (status != GSC_SUCCESS)
	{
		notify_error(1, "%s: DIO24_DIRECTION_OUTPUT(PORT_C)\n", pSTDdev->taskName);
		return;
	}

	if( (int)pParam->setValue == 0) 
	{
		epicsPrintf(" %s %s PC_%d(0x%x) (input)  OK!\n", pSTDdev->taskName, precord->name, pParam->n32Arg0, port );
	} 
	else {
		epicsPrintf(" %s %s PC_%d(0x%x) (output)  OK!\n", pSTDdev->taskName, precord->name, pParam->n32Arg0, port );
	}
}

static void devBoDio24_BO_DIO24_PUT_PORT_A(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
/*	struct dbCommon *precord = pParam->precord; */
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;
	uint32 status;
	unsigned long value=0x0;

	if( pDio24->portA_io == DIO24_GPIO_DIRECTION_INPUT ) {
		notify_error(1, "%s: PORT_A is input mode!", pSTDdev->taskName);
		return;
	}

	status	= DIO24_GPIO_OUTPUT__GET(pDio24->dev, DIO24_WHICH_PORT_A, &value);

/*
	status = dio24_config(	pDio24->dev,
								DIO24_GPIO_OUTPUT,
								DIO24_WHICH_PORT_A,
								GSC_NO_CHANGE,
								&value);
*/

/*	printf("%s:%s before:0x%x \n", pSTDdev->taskName, precord->name, (unsigned int)value ); */
	if ( (int)pParam->setValue ) {
		value |= 0x1 << pParam->n32Arg0;
	} else {
		value &= ~(0x1 << pParam->n32Arg0);
	}
/*	printf(" after:0x%x\n", (unsigned int)value ); */
	
	status	= DIO24_GPIO_OUTPUT__SET(pDio24->dev, DIO24_WHICH_PORT_A, value);
	
}

static void devBoDio24_BO_DIO24_PUT_PORT_B(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;
	uint32 status;
	unsigned long value=0x0;

	if( pDio24->portB_io == DIO24_GPIO_DIRECTION_INPUT ) {
		notify_error(1, "%s: PORT_B is input mode!", pSTDdev->taskName);
		return;
	}

	status	= DIO24_GPIO_OUTPUT__GET(pDio24->dev, DIO24_WHICH_PORT_B, &value);

	if ( (int)pParam->setValue ) {
		value |= 0x1 << pParam->n32Arg0;
	} else {
		value &= ~(0x1 << pParam->n32Arg0);
	}
	
	status	= DIO24_GPIO_OUTPUT__SET(pDio24->dev, DIO24_WHICH_PORT_B, value);
	
}

static void devBoDio24_BO_DIO24_PUT_PORT_C(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_Dio24 *pDio24 = (ST_Dio24 *)pSTDdev->pUser;
	uint32 status;
	unsigned long value=0x0;

	if( pDio24->portC_io == DIO24_GPIO_DIRECTION_INPUT ) {
		notify_error(1, "%s: PORT_C is input mode!", pSTDdev->taskName);
		return;
	}

	status	= DIO24_GPIO_OUTPUT__GET(pDio24->dev, DIO24_WHICH_PORT_C, &value);

	if ( (int)pParam->setValue ) {
		value |= 0x1 << pParam->n32Arg0;
	} else {
		value &= ~(0x1 << pParam->n32Arg0);
	}
	
	status	= DIO24_GPIO_OUTPUT__SET(pDio24->dev, DIO24_WHICH_PORT_C, value);
	
}


static long devBoDio24_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %d", pDpvt->devName, pDpvt->arg0, &pDpvt->n32Arg0);
#if 0
	epicsPrintf("devBoACQ196Control arg num: %d: %s %s\n",i, devBoDio24->devName, devBoDio24->arg0);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBoDio24 (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoDio24 (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(pDpvt->arg0, BO_DIO24_GPIO_PORTA_DIR_STR)) {
		pDpvt->ind = BO_DIO24_GPIO_PORTA_DIR;
/*		pDpvt->n32Arg0 = strtol(pDpvt->arg1,NULL,0); */
	} 
	else if(!strcmp(pDpvt->arg0, BO_DIO24_GPIO_PORTB_DIR_STR)) 
		pDpvt->ind = BO_DIO24_GPIO_PORTB_DIR;

	else if(!strcmp(pDpvt->arg0, BO_DIO24_GPIO_PORTC_ALL_DIR_STR)) 
		pDpvt->ind = BO_DIO24_GPIO_PORTC_ALL_DIR;

	else if(!strcmp(pDpvt->arg0, BO_DIO24_GPIO_PORTC_DIR_STR)) 
		pDpvt->ind = BO_DIO24_GPIO_PORTC_DIR;
	
	else if(!strcmp(pDpvt->arg0, BO_DIO24_PUT_PORT_A_STR)) 
		pDpvt->ind = BO_DIO24_PUT_PORT_A;
	else if(!strcmp(pDpvt->arg0, BO_DIO24_PUT_PORT_B_STR)) 
		pDpvt->ind = BO_DIO24_PUT_PORT_B;
	else if(!strcmp(pDpvt->arg0, BO_DIO24_PUT_PORT_C_STR)) 
		pDpvt->ind = BO_DIO24_PUT_PORT_C;
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoDio24_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoDio24_write_bo(boRecord *precord)
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

			case BO_DIO24_GPIO_PORTA_DIR:
				qData.pFunc = devBoDio24_BO_DIO24_GPIO_PORT_A_DIR;
				break;
			case BO_DIO24_GPIO_PORTB_DIR:
				qData.pFunc = devBoDio24_BO_DIO24_GPIO_PORT_B_DIR;
				break;
			case BO_DIO24_GPIO_PORTC_ALL_DIR:
				qData.pFunc = devBoDio24_BO_DIO24_GPIO_PORT_C_DIR;
				break;
			case BO_DIO24_GPIO_PORTC_DIR:
				qData.pFunc = devBoDio24_BO_DIO24_GPIO_PORT_C_single_DIR;
				break;

			case BO_DIO24_PUT_PORT_A:
				qData.pFunc = devBoDio24_BO_DIO24_PUT_PORT_A;
				break;
			case BO_DIO24_PUT_PORT_B:
				qData.pFunc = devBoDio24_BO_DIO24_PUT_PORT_B;
				break;
			case BO_DIO24_PUT_PORT_C:
				qData.pFunc = devBoDio24_BO_DIO24_PUT_PORT_C;
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

static long devAiDio24_init_record(aiRecord *precord)
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
						  "devAiDio24 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiDio24 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pDpvt->arg0, AI_DIO24_GET_PORT_A_STR))
		pDpvt->ind = AI_DIO24_GET_PORT_A;
	else if(!strcmp(pDpvt->arg0, AI_DIO24_GET_PORT_B_STR))
		pDpvt->ind = AI_DIO24_GET_PORT_B;
	else if(!strcmp(pDpvt->arg0, AI_DIO24_GET_PORT_C_STR))
		pDpvt->ind = AI_DIO24_GET_PORT_C;
	else {
		pDpvt->ind = -1;
		epicsPrintf("_Dio24 devAiDio24_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiDio24_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiDio24_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_Dio24 *pDio24;
//	uint32 status;
	unsigned long get = 0;

	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pDio24 = (ST_Dio24*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_DIO24_GET_PORT_A:
			if( pDio24->portA_io == DIO24_GPIO_DIRECTION_INPUT) {
				if( DIO24_GPIO_INPUT__GET(pDio24->dev, DIO24_WHICH_PORT_A, &get) == GSC_SUCCESS)
					precord->val = get;
				else precord->val = -1;

//				epicsPrintf("%s: port A value: 0x%x, %d \n", pSTDdev->taskName, (int)precord->val, (int)precord->val);
			}
			else {
				if( dio24_config( pDio24->dev,
								DIO24_GPIO_OUTPUT,
								DIO24_WHICH_PORT_A,
								GSC_NO_CHANGE,
								&get) == GSC_SUCCESS )
					precord->val = get;
				else 
					precord->val = -1;
			}
			
			break;
		case AI_DIO24_GET_PORT_B:
			if( pDio24->portB_io == DIO24_GPIO_DIRECTION_INPUT) {
				if( DIO24_GPIO_INPUT__GET(pDio24->dev, DIO24_WHICH_PORT_B, &get) == GSC_SUCCESS)
					precord->val = get;
				else precord->val = -1;

//				epicsPrintf("%s: port B value: 0x%x, %d \n", pSTDdev->taskName, (int)precord->val, (int)precord->val);
			}
			else {
				if( dio24_config( pDio24->dev,
								DIO24_GPIO_OUTPUT,
								DIO24_WHICH_PORT_B,
								GSC_NO_CHANGE,
								&get) == GSC_SUCCESS )
					precord->val = get;
				else 
					precord->val = -1;
			}
			
			break;
		case AI_DIO24_GET_PORT_C:
			if( pDio24->portC_io == DIO24_GPIO_DIRECTION_INPUT) {
				if( DIO24_GPIO_INPUT__GET(pDio24->dev, DIO24_WHICH_PORT_C, &get) == GSC_SUCCESS)
					precord->val = get;
				else precord->val = -1;

//				epicsPrintf("%s: port C value: 0x%x, %d \n", pSTDdev->taskName, (int)precord->val, (int)precord->val);
			}
			else {
				if( dio24_config( pDio24->dev,
								DIO24_GPIO_OUTPUT,
								DIO24_WHICH_PORT_C,
								GSC_NO_CHANGE,
								&get) == GSC_SUCCESS )
					precord->val = get;
				else 
					precord->val = -1;
			}
			
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... _Dio24 \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}



