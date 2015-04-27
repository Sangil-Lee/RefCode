
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

#include "sfwAdminHead.h"
#include "sfwDBSeq.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drvIT6322.h" 
#include "myMDSplus.h"
#include "niPvString.h"



static void devIT6322_AO_DC1_CH1_VOLTAGE(ST_execParam *pParam);
static void devIT6322_AO_DC1_CH2_VOLTAGE(ST_execParam *pParam);
static void devIT6322_AO_DC1_CH3_VOLTAGE(ST_execParam *pParam);
static void devIT6322_AO_DC1_CH1_ON_OFF(ST_execParam *pParam);
static void devIT6322_AO_DC1_CH2_ON_OFF(ST_execParam *pParam);
static void devIT6322_AO_DC1_CH3_ON_OFF(ST_execParam *pParam);
static void devIT6322_AO_DC2_CH1_VOLTAGE(ST_execParam *pParam);
static void devIT6322_AO_DC2_CH2_VOLTAGE(ST_execParam *pParam);
static void devIT6322_AO_DC2_CH3_VOLTAGE(ST_execParam *pParam);
static void devIT6322_AO_DC2_CH1_ON_OFF(ST_execParam *pParam);
static void devIT6322_AO_DC2_CH2_ON_OFF(ST_execParam *pParam);
static void devIT6322_AO_DC2_CH3_ON_OFF(ST_execParam *pParam);
static void devIT6322_AO_SENSOR_BLOCK1_MAX_TEMP(ST_execParam *pParam);
static void devIT6322_AO_SENSOR_BLOCK2_MAX_TEMP(ST_execParam *pParam);
static void devIT6322_AO_SENSOR_BLOCK3_MAX_TEMP(ST_execParam *pParam);
static void devIT6322_AO_SENSOR_BLOCK4_MAX_TEMP(ST_execParam *pParam);



static long devAoIT6322Control_init_record(aoRecord *precord);
static long devAoIT6322Control_write_ao(aoRecord *precord);

static long devAiIT6322RawRead_init_record(aiRecord *precord);
static long devAiIT6322RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiIT6322RawRead_read_ai(aiRecord *precord);

static long devBiIT6322_init_record(biRecord *precord);
static long devBiIT6322_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiIT6322_read_bi(biRecord *precord);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devBiIT6322BinaryRead = {
	6,
	NULL,
	NULL,
	devBiIT6322_init_record,
	devBiIT6322_get_ioint_info,
	devBiIT6322_read_bi,
	NULL
};
epicsExportAddress(dset, devBiIT6322BinaryRead);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoIT6322Control = {
	6,
	NULL,
	NULL,
	devAoIT6322Control_init_record,
	NULL,
	devAoIT6322Control_write_ao,
	NULL
};
epicsExportAddress(dset, devAoIT6322Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiIT6322RawRead = {
	6,
	NULL,
	NULL,
	devAiIT6322RawRead_init_record,
	devAiIT6322RawRead_get_ioint_info,
	devAiIT6322RawRead_read_ai,
	NULL
};
epicsExportAddress(dset, devAiIT6322RawRead);



static long devBiIT6322_init_record(biRecord *precord)
{
	ST_dpvt *pdevAiIT6322RawReaddpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiIT6322RawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiIT6322RawReaddpvt->devName,
				   pdevAiIT6322RawReaddpvt->arg0,
				   pdevAiIT6322RawReaddpvt->arg1);
#if 0
	epicsPrintf("devAiIT6322RawRead arg num: %d: %s %s %s\n",i, pdevAiIT6322RawReaddpvt->devName, 
								pdevAiIT6322RawReaddpvt->arg0,
								pdevAiIT6322RawReaddpvt->arg1);
#endif
			pdevAiIT6322RawReaddpvt->pSTDdev = get_STDev_from_name("IT6322");
			if(!pdevAiIT6322RawReaddpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiIT6322RawRead (init_record) Illegal INP field: task name");
				free(pdevAiIT6322RawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			if( i > 2)
			{
				pdevAiIT6322RawReaddpvt->pchannelConfig = drvIT6322_find_channelConfig(pdevAiIT6322RawReaddpvt->devName,
											         pdevAiIT6322RawReaddpvt->arg0);
				if(!pdevAiIT6322RawReaddpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiIT6322RawRead (init_record) Illegal INP field: channel name");
					free(pdevAiIT6322RawReaddpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiIT6322RawRead (init_record) Illegal INP field");
			free(pdevAiIT6322RawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


		if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_CH1_ON_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_CH1_ON_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_CH2_ON_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_CH2_ON_R;	
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_CH3_ON_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_CH3_ON_R;		
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_CH1_ON_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_CH1_ON_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_CH2_ON_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_CH2_ON_R;	
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_CH3_ON_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_CH3_ON_R;		
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_COMM_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_COMM;		
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_COMM_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_COMM;		
			}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiIT6322RawReaddpvt;

	return 2;    /* don't convert */ 
}

static long devBiIT6322_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pdevAiIT6322RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	ST_IT6322_dev *pIT6322 = NULL;

	if(!pdevAiIT6322RawReaddpvt) return 0;

	pSTDdev		= pdevAiIT6322RawReaddpvt->pSTDdev;

	pIT6322 = (ST_IT6322_dev*)pSTDdev->pUser;
	

	switch(pdevAiIT6322RawReaddpvt->ind) {


		case AI_IT6322_DC1_CH1_ON_R:
			precord->val = pIT6322->DC1_CH1_ON_OFF;
			break;

		case AI_IT6322_DC1_CH2_ON_R:
			precord->val = pIT6322->DC1_CH2_ON_OFF;
			break;

		case AI_IT6322_DC1_CH3_ON_R:
			precord->val = pIT6322->DC1_CH3_ON_OFF;
			break;

		case AI_IT6322_DC1_COMM:
			precord->val = pIT6322->DC1_Comm_status;
			break;
		
		case AI_IT6322_DC2_CH1_ON_R:
			precord->val = pIT6322->DC2_CH1_ON_OFF;
			break;
		
		case AI_IT6322_DC2_CH2_ON_R:
			precord->val = pIT6322->DC2_CH2_ON_OFF;
			break;
		
		case AI_IT6322_DC2_CH3_ON_R:
			precord->val = pIT6322->DC2_CH3_ON_OFF;
			break;
		
		case AI_IT6322_DC2_COMM:
			precord->val = pIT6322->DC2_Comm_status;
			break;

		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevAiIT6322RawReaddpvt->ind); 
			break;
	}
	return (2);
}


static long devBiIT6322_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiIT6322RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiIT6322RawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiIT6322RawReaddpvt->pSTDdev;
/*	if( pdevAiIT6322RawReaddpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else */
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}




static long devAoIT6322Control_init_record(aoRecord *precord)
{
	ST_dpvt *pdevIT6322Controldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevIT6322Controldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s",
			           pdevIT6322Controldpvt->arg0, pdevIT6322Controldpvt->arg1);

#if 0

	epicsPrintf("devAoIT6322Control arg num: %d: %s\n",i, pdevIT6322Controldpvt->arg0);

#endif
			ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
			pdevIT6322Controldpvt->pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
			if(!pdevIT6322Controldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoIT6322Control (init_record) Illegal INP field: task_name");
				free(pdevIT6322Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoIT6322Control (init_record) Illegal OUT field");
			free(pdevIT6322Controldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC1_CH1_VOLT_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC1_CH1_VOLT;


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC1_CH2_VOLT_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC1_CH2_VOLT;


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC1_CH3_VOLT_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC1_CH3_VOLT;


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC2_CH1_VOLT_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC2_CH1_VOLT;


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC2_CH2_VOLT_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC2_CH2_VOLT;


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC2_CH3_VOLT_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC2_CH3_VOLT;

	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC1_CH1_ON_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC1_CH1_ON;		


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC1_CH2_ON_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC1_CH2_ON;		


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC1_CH3_ON_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC1_CH3_ON;		


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC2_CH1_ON_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC2_CH1_ON;		


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC2_CH2_ON_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC2_CH2_ON;		


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_DC2_CH3_ON_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_DC2_CH3_ON;		


	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_SENSOR_BLOCK1_MAX_TEMP_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_SENSOR_BLOCK1_MAX_TEMP;		

	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_SENSOR_BLOCK2_MAX_TEMP_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_SENSOR_BLOCK2_MAX_TEMP;		

	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_SENSOR_BLOCK3_MAX_TEMP_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_SENSOR_BLOCK3_MAX_TEMP;		

	} else if(!strcmp(pdevIT6322Controldpvt->arg0, AO_IT6322_SENSOR_BLOCK4_MAX_TEMP_STR)) {
		pdevIT6322Controldpvt->ind = AO_IT6322_SENSOR_BLOCK4_MAX_TEMP;		
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevIT6322Controldpvt;
	
	return 2;     /* don't convert */
}


static long devAoIT6322Control_write_ao(aoRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	ST_dpvt        *pdevIT6322Controldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData         qData;



	if(!pdevIT6322Controldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pdevIT6322Controldpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif

		switch(pdevIT6322Controldpvt->ind) {
			case AO_IT6322_DC1_CH1_VOLT:

				qData.pFunc = devIT6322_AO_DC1_CH1_VOLTAGE;
				break;
			case AO_IT6322_DC1_CH2_VOLT:
				qData.pFunc = devIT6322_AO_DC1_CH2_VOLTAGE;
				break;
			case AO_IT6322_DC1_CH3_VOLT:
				qData.pFunc = devIT6322_AO_DC1_CH3_VOLTAGE;
				break;
			case AO_IT6322_DC2_CH1_VOLT:
				qData.pFunc = devIT6322_AO_DC2_CH1_VOLTAGE;
				break;
			case AO_IT6322_DC2_CH2_VOLT:
				qData.pFunc = devIT6322_AO_DC2_CH2_VOLTAGE;
				break;
			case AO_IT6322_DC2_CH3_VOLT:
				qData.pFunc = devIT6322_AO_DC2_CH3_VOLTAGE;
				break;
			case AO_IT6322_DC1_CH1_ON:
				qData.pFunc = devIT6322_AO_DC1_CH1_ON_OFF;
				break;
			case AO_IT6322_DC1_CH2_ON:
				qData.pFunc = devIT6322_AO_DC1_CH2_ON_OFF;
				break;
			case AO_IT6322_DC1_CH3_ON:
				qData.pFunc = devIT6322_AO_DC1_CH3_ON_OFF;
				break;
			case AO_IT6322_DC2_CH1_ON:
				qData.pFunc = devIT6322_AO_DC2_CH1_ON_OFF;
				break;
			case AO_IT6322_DC2_CH2_ON:
				qData.pFunc = devIT6322_AO_DC2_CH2_ON_OFF;
				break;
			case AO_IT6322_DC2_CH3_ON:
				qData.pFunc = devIT6322_AO_DC2_CH3_ON_OFF;
				break;

			case AO_IT6322_SENSOR_BLOCK1_MAX_TEMP:
				qData.pFunc = devIT6322_AO_SENSOR_BLOCK1_MAX_TEMP;
				break;

			case AO_IT6322_SENSOR_BLOCK2_MAX_TEMP:
				qData.pFunc = devIT6322_AO_SENSOR_BLOCK2_MAX_TEMP;
				break;

			case AO_IT6322_SENSOR_BLOCK3_MAX_TEMP:
				qData.pFunc = devIT6322_AO_SENSOR_BLOCK3_MAX_TEMP;
				break;

			case AO_IT6322_SENSOR_BLOCK4_MAX_TEMP:
				qData.pFunc = devIT6322_AO_SENSOR_BLOCK4_MAX_TEMP;
				break;
				
				
			default:
				epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevIT6322Controldpvt->ind); 
				break;

		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
#if 0		
		switch(pdevIT6322Controldpvt->ind) {
			case AO_SET_INTERCLK: precord->val = 0; break;
			case AO_SET_EXTERCLK: precord->val = 0; break;
			case AO_DAQ_START: precord->val = 0; break;
			case AO_DAQ_STOP: precord->val = 0; break;
			case AO_ADC_START: precord->val = 0; break;
			case AO_ADC_STOP: precord->val = 0; break;
			case AO_LOCAL_STORAGE: precord->val = 0; break;
			case AO_REMOTE_STORAGE: precord->val = 0; break;
			case AO_SET_CLEAR_STATUS: precord->val = 0; break;
		}
#endif
		return 2;    /* don't convert */
	}

	return 0;
}





static void devIT6322_AO_DC1_CH1_VOLTAGE(ST_execParam *pParam)
	
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {

			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC1_CH1_Voltage = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<0);				
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Power_Supply1/Channel1 Voltage is %f.\n", pParam->setValue);	
	

}

static void devIT6322_AO_DC1_CH2_VOLTAGE(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
			
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC1_CH2_Voltage = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<1);
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Power_Supply1/Channel2 Voltage is %f.\n", pParam->setValue);	

	

}

static void devIT6322_AO_DC1_CH3_VOLTAGE(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
				
				ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
				pIT6322->DC1_CH3_Voltage = pParam->setValue;
				pIT6322->change_option |= (0x000000000001<<2);				
				scanIoRequest(pSTDdev->ioScanPvt_userCall);				
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Power_Supply1/Channel3 Voltage is %f.\n", pParam->setValue);	

	

}



static void devIT6322_AO_DC1_CH1_ON_OFF(ST_execParam *pParam)
	
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
	 	if(strcmp(pSTDdev->devType, "IT6322") == 0) {
				
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC1_CH1_ON_OFF = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<6);				
			scanIoRequest(pSTDdev->ioScanPvt_userCall);							
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	if(pParam->setValue)
		{
			epicsPrintf("Power_Supply1/Channel1 Power is ON.\n");	
		}
	else
		{
			epicsPrintf("Power_Supply1/Channel1 Power is OFF.\n");	
		}
	

}




static void devIT6322_AO_DC1_CH2_ON_OFF(ST_execParam *pParam)
	{
		
			ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
			struct dbCommon *precord = pParam->precord;
			ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
			ST_IT6322_dev *pIT6322 = NULL;
		
		while(pSTDdev)
		{
		
	
	 	if(strcmp(pSTDdev->devType, "IT6322") == 0) {
					
				ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
				pIT6322->DC1_CH2_ON_OFF = pParam->setValue;
				pIT6322->change_option |= (0x000000000001<<7);				
				scanIoRequest(pSTDdev->ioScanPvt_userCall);								
			}
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
		
		if(pParam->setValue)
			{
				epicsPrintf("Power_Supply1/Channel2 Power is ON.\n");	
			}
		else
			{
				epicsPrintf("Power_Supply1/Channel2 Power is OFF.\n");	
			}
		
	
	}




static void devIT6322_AO_DC1_CH3_ON_OFF(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: ADC must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
				
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC1_CH3_ON_OFF = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<8);							
			scanIoRequest(pSTDdev->ioScanPvt_userCall);							
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	if(pParam->setValue)
		{
			epicsPrintf("Power_Supply1/Channel3 Power is ON.\n");	
		}
	else
		{
			epicsPrintf("Power_Supply1/Channel3 Power is OFF.\n");	
		}

	

}


static void devIT6322_AO_DC2_CH1_VOLTAGE(ST_execParam *pParam)
	
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
			
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC2_CH1_Voltage = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<3);			
			scanIoRequest(pSTDdev->ioScanPvt_userCall);							
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Power_Supply2/Channel1 Voltage is %f.\n", pParam->setValue);	

	

}

static void devIT6322_AO_DC2_CH2_VOLTAGE(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
			
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC2_CH2_Voltage = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<4);			
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Power_Supply2/Channel2 Voltage is %f.\n", pParam->setValue);	

	

}

static void devIT6322_AO_DC2_CH3_VOLTAGE(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: ADC must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
			
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC2_CH3_Voltage = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<5);						
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Power_Supply2/Channel3 Voltage is %f.\n", pParam->setValue);	

	

}



static void devIT6322_AO_DC2_CH1_ON_OFF(ST_execParam *pParam)
	
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{

	 	if(strcmp(pSTDdev->devType, "IT6322") == 0) {
					
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC2_CH1_ON_OFF = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<9);						
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	if(pParam->setValue)
		{
			epicsPrintf("Power_Supply2/Channel1 Power is ON.\n");	
		}
	else
		{
			epicsPrintf("Power_Supply2/Channel1 Power is OFF.\n");	
		}

	

}




static void devIT6322_AO_DC2_CH2_ON_OFF(ST_execParam *pParam)
	{
		
			ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
			struct dbCommon *precord = pParam->precord;
			ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
			ST_IT6322_dev *pIT6322 = NULL;

		while(pSTDdev)
			{
		
	 	if(strcmp(pSTDdev->devType, "IT6322") == 0) {
							
				ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
				pIT6322->DC2_CH2_ON_OFF = pParam->setValue;
				pIT6322->change_option |= (0x000000000001<<10);						
				scanIoRequest(pSTDdev->ioScanPvt_userCall);				

	 		}
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
		
		if(pParam->setValue)
			{
				epicsPrintf("Power_Supply2/Channel2 Power is ON.\n");	
			}
		else
			{
				epicsPrintf("Power_Supply2/Channel2 Power is OFF.\n");	
			}

		
	
	}




static void devIT6322_AO_DC2_CH3_ON_OFF(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
	if(strcmp(pSTDdev->devType, "IT6322") == 0) {
						
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->DC2_CH3_ON_OFF = pParam->setValue;
			pIT6322->change_option |= (0x000000000001<<11);						
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	if(pParam->setValue)
		{
			epicsPrintf("Power_Supply2/Channel3 Power is ON.\n");	
		}
	else
		{
			epicsPrintf("Power_Supply2/Channel3 Power is OFF.\n");	
		}

	

}



static void devIT6322_AO_SENSOR_BLOCK1_MAX_TEMP(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
						
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->SENSOR_BLOCK1_MAX_TEMP = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Sensor_block NO.1 Limited temp %f.\n", pParam->setValue);	
	

}

static void devIT6322_AO_SENSOR_BLOCK2_MAX_TEMP(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
						
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->SENSOR_BLOCK2_MAX_TEMP = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Sensor_block NO.2 Limited temp is %f.\n", pParam->setValue);	

	

}

static void devIT6322_AO_SENSOR_BLOCK3_MAX_TEMP(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
						
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->SENSOR_BLOCK3_MAX_TEMP = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Sensor_block NO.3 Limited temp is %f.\n", pParam->setValue);	
	

}

static void devIT6322_AO_SENSOR_BLOCK4_MAX_TEMP(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_IT6322_dev *pIT6322 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "IT6322") == 0) {
						
			ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
			pIT6322->SENSOR_BLOCK4_MAX_TEMP = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Sensor_block NO.4 Limited temp is %f.\n", pParam->setValue);	
	
}


static long devAiIT6322RawRead_init_record(aiRecord *precord)
{
	ST_dpvt *pdevAiIT6322RawReaddpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiIT6322RawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiIT6322RawReaddpvt->devName,
				   pdevAiIT6322RawReaddpvt->arg0,
				   pdevAiIT6322RawReaddpvt->arg1);
#if 0
	epicsPrintf("devAiIT6322RawRead arg num: %d: %s %s %s\n",i, pdevAiIT6322RawReaddpvt->devName, 
								pdevAiIT6322RawReaddpvt->arg0,
								pdevAiIT6322RawReaddpvt->arg1);
#endif
			pdevAiIT6322RawReaddpvt->pSTDdev = get_STDev_from_name("IT6322");
			if(!pdevAiIT6322RawReaddpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiIT6322RawRead (init_record) Illegal INP field: task name");
				free(pdevAiIT6322RawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			if( i > 2)
			{
				pdevAiIT6322RawReaddpvt->pchannelConfig = drvIT6322_find_channelConfig(pdevAiIT6322RawReaddpvt->devName,
											         pdevAiIT6322RawReaddpvt->arg0);
				if(!pdevAiIT6322RawReaddpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiIT6322RawRead (init_record) Illegal INP field: channel name");
					free(pdevAiIT6322RawReaddpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiIT6322RawRead (init_record) Illegal INP field");
			free(pdevAiIT6322RawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


		if(!strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_CH1_VOLT_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_CH1_VOLT_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_CH2_VOLT_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_CH2_VOLT_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC1_CH3_VOLT_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC1_CH3_VOLT_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_CH1_VOLT_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_CH1_VOLT_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_CH2_VOLT_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_CH2_VOLT_R;
			}
		else if( !strcmp(pdevAiIT6322RawReaddpvt->devName, AI_IT6322_DC2_CH3_VOLT_R_STR)) {
			pdevAiIT6322RawReaddpvt->ind = AI_IT6322_DC2_CH3_VOLT_R;
			}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiIT6322RawReaddpvt;

	return 2;    /* don't convert */ 
}

static long devAiIT6322RawRead_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pdevAiIT6322RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	ST_IT6322_dev *pIT6322 = NULL;

	if(!pdevAiIT6322RawReaddpvt) return 0;

	pSTDdev		= pdevAiIT6322RawReaddpvt->pSTDdev;

	pIT6322 = (ST_IT6322_dev*)pSTDdev->pUser;
	

	switch(pdevAiIT6322RawReaddpvt->ind) {

		case AI_IT6322_DC1_CH1_VOLT_R:
			precord->val = pIT6322->DC1_CH1_Voltage;
			break;

		case AI_IT6322_DC1_CH2_VOLT_R:
			precord->val = pIT6322->DC1_CH2_Voltage;
			break;
			
		case AI_IT6322_DC1_CH3_VOLT_R:
			precord->val = pIT6322->DC1_CH3_Voltage;
			break;

		
		case AI_IT6322_DC2_CH1_VOLT_R:
			precord->val = pIT6322->DC2_CH1_Voltage;
			break;
		
		case AI_IT6322_DC2_CH2_VOLT_R:
			precord->val = pIT6322->DC2_CH2_Voltage;
			break;
			
		case AI_IT6322_DC2_CH3_VOLT_R:
			precord->val = pIT6322->DC2_CH3_Voltage;
			break;
		
		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevAiIT6322RawReaddpvt->ind); 
			break;
	}
	return (2);
}


static long devAiIT6322RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiIT6322RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiIT6322RawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiIT6322RawReaddpvt->pSTDdev;
/*	if( pdevAiIT6322RawReaddpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else */
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

