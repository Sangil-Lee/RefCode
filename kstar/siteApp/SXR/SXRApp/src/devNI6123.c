
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
#include <sys/stat.h>
#include <sys/types.h>


#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwAdminHead.h"
#include "sfwDBSeq.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drvNI6123.h" 
#include "myMDSplus.h"
#include "niPvString.h"

typedef struct {
	ST_ADMIN  *ptaskConfig;

	char            arg0[SIZE_STRINGOUT_VAL];
	char            arg1[SIZE_STRINGOUT_VAL];
	char            recordName[SIZE_PV_NAME];	
	int               ind;
} ST_devAdmin_dpvt;


static void devNI6123_AO_SAMPLING(ST_execParam *pParam);
static void devNI6123_AO_TERMINAL_CONFIG(ST_execParam *pParam);
static void devNI6123_AO_MINIMUM_VOLTAGE(ST_execParam *pParam);
static void devNI6123_AO_MAXIMUM_VOLTAGE(ST_execParam *pParam);
static void devNI6123_AO_UNITS(ST_execParam *pParam);
static void devNI6123_AO_SAMPLING_TIME(ST_execParam *pParam);
static void devNI6123_AO_SAMPLING_MODE(ST_execParam *pParam);
static void devNI6123_AO_SAMP_PER_CHAN_ACQUIE(ST_execParam *pParam);
static void devNI6123_BO_AUTO_RUN(ST_execParam *pParam);
static void devNI6123_BO_LOCAL_START(ST_execParam *pParam);




static long devAoNI6123Control_init_record(aoRecord *precord);
static long devAoNI6123Control_write_ao(aoRecord *precord);

static long devAiNI6123RawRead_init_record(aiRecord *precord);
static long devAiNI6123RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiNI6123RawRead_read_ai(aiRecord *precord);

static long devBoNI6123_init_record(boRecord *precord);
static long devBoNI6123_write_bo(boRecord *precord);




struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoNI6123Control = {
	6,
	NULL,
	NULL,
	devAoNI6123Control_init_record,
	NULL,
	devAoNI6123Control_write_ao,
	NULL
};

epicsExportAddress(dset, devAoNI6123Control);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devBoNI6123Control = {
	6,
	NULL,
	NULL,
	devBoNI6123_init_record,
	NULL,
	devBoNI6123_write_bo,
	NULL
};

epicsExportAddress(dset, devBoNI6123Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiNI6123RawRead_ai = {
	6,
	NULL,
	NULL,
	devAiNI6123RawRead_init_record,
	devAiNI6123RawRead_get_ioint_info,
	devAiNI6123RawRead_read_ai,
	NULL
};
epicsExportAddress(dset, devAiNI6123RawRead_ai);



static long devAoNI6123Control_init_record(aoRecord *precord)
{
	ST_dpvt *pdevNI6123Controldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevNI6123Controldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s",
			           pdevNI6123Controldpvt->arg0);

#if 0

	epicsPrintf("devAoACQ196Control arg num: %d: %s\n",i, pdevNI6123Controldpvt->arg0);

#endif
			ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
			pdevNI6123Controldpvt->pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
			if(!pdevNI6123Controldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoNI6123Control (init_record) Illegal INP field: task_name");
				free(pdevNI6123Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoNI6123Control (init_record) Illegal OUT field");
			free(pdevNI6123Controldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_MINIMUM_VOLTAGE_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_MINIMUM_VOLTAGE;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_MAXIMUM_VOLTAGE_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_MAXIMUM_VOLTAGE;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_UNITS_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_UNITS;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_SAMPLING_TIME_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_SAMPLING_TIME;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_SAMPLING_MODE_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_SAMPLING_MODE;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_SAMP_PER_CHAN_ACQUIE_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_SAMP_PER_CHAN_ACQUIE;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_TERMINAL_CONFIG_STR)) {
		pdevNI6123Controldpvt->ind = AO_DAQ_TERMINAL_CONFIG;

	} else if(!strcmp(pdevNI6123Controldpvt->arg0, AO_DAQ_SAMPLING_RATE_STR)) {
			pdevNI6123Controldpvt->ind = AO_DAQ_SAMPLING_RATE;
	}		

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevNI6123Controldpvt;
	
	return 2;     /* don't convert */
}

static long devAoNI6123Control_write_ao(aoRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	ST_dpvt        *pdevNI6123Controldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData         qData;



	if(!pdevNI6123Controldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pdevNI6123Controldpvt->pSTDdev;
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
		switch(pdevNI6123Controldpvt->ind) {
			case AO_DAQ_MINIMUM_VOLTAGE:
				qData.pFunc = devNI6123_AO_MINIMUM_VOLTAGE;
				break;
			case AO_DAQ_MAXIMUM_VOLTAGE:
				qData.pFunc = devNI6123_AO_MAXIMUM_VOLTAGE;
				break;
			case AO_DAQ_UNITS:
				qData.pFunc = devNI6123_AO_UNITS;
				break;
			case AO_DAQ_SAMPLING_TIME:
				qData.pFunc = devNI6123_AO_SAMPLING_TIME;
				break;
			case AO_DAQ_SAMPLING_MODE:
				qData.pFunc = devNI6123_AO_SAMPLING_MODE;
				break;
			case AO_DAQ_SAMP_PER_CHAN_ACQUIE:
				qData.pFunc = devNI6123_AO_SAMP_PER_CHAN_ACQUIE;
				break;
			case AO_DAQ_TERMINAL_CONFIG:
				qData.pFunc = devNI6123_AO_TERMINAL_CONFIG;
				break;
			case AO_DAQ_SAMPLING_RATE:
				qData.pFunc = devNI6123_AO_SAMPLING;
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
		switch(pdevNI6123Controldpvt->ind) {
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





static void devNI6123_AO_MINIMUM_VOLTAGE(ST_execParam *pParam)
	
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_NI6123_dev *pNI6123 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {
			ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
			pNI6123->minVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Minimum Measure Voltage is %f.\n", pParam->setValue);	
	

}

static void devNI6123_AO_MAXIMUM_VOLTAGE(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_NI6123_dev *pNI6123 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: ADC must stop! \n", pSTDdev->taskName );		
		} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {
			ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
			pNI6123->maxVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 						
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Maximum Measure Voltage is %f.\n", pParam->setValue);	

	

}

static void devNI6123_AO_UNITS(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_NI6123_dev *pNI6123 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

		} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {

			ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
			pNI6123->units = pParam->setValue;
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("UNITS Configure is %f.\n", pParam->setValue);	
	

}

static void devNI6123_AO_SAMPLING_TIME(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_NI6123_dev *pNI6123 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

		} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {

			pNI6123 = pSTDdev->pUser;
			pSTDdev->ST_Base.dT1[0] = pParam->setValue;
			pNI6123->sample_time = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Minimum Sampling Time  is %f.\n", pParam->setValue);	

	

}







static void devNI6123_AO_SAMPLING_MODE(ST_execParam *pParam)
	
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_NI6123_dev *pNI6123 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

		} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {

			ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
			pNI6123->smp_mode = pParam->setValue;
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("SAMPLING_MODE is %f.\n", pParam->setValue);	
	

}




static void devNI6123_AO_SAMP_PER_CHAN_ACQUIE(ST_execParam *pParam)
	{
		
			ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
			struct dbCommon *precord = pParam->precord;
			ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
			ST_NI6123_dev *pNI6123 = NULL;
		
		while(pSTDdev)
		{
		
			if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
			{
				epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

			} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {

				ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
				pNI6123->sampPerChanToAcquie = pParam->setValue;
			}
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
		
		epicsPrintf("SAMP_PER_CHAN_ACQUIE is %f.\n", pParam->setValue);	
		
	
	}




static void devNI6123_AO_TERMINAL_CONFIG(ST_execParam *pParam)
{
	
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		ST_NI6123_dev *pNI6123 = NULL;
	
	while(pSTDdev)
	{
	
		if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
		{
			epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

		} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {

			ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
			pNI6123->terminal_config = pParam->setValue;
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	epicsPrintf("Terminal Configure is %f.\n", pParam->setValue);	
	

}


static void devNI6123_AO_SAMPLING(ST_execParam *pParam)
{

	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	ST_NI6123_dev *pNI6123 = NULL;

while(pSTDdev)
{

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
	{
		epicsPrintf("ERROR! %s: DAQ must stop! \n", pSTDdev->taskName );		

	} else if(strcmp(pSTDdev->devType, "NI6123") == 0) {

		pNI6123 = pSTDdev->pUser;
		pSTDdev->ST_Base.nSamplingRate = (int)pParam->setValue;
		pNI6123->smp_rate = pParam->setValue;
		scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
	}
	pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
}

epicsPrintf("Sampling Rate is %f.\n", pParam->setValue);	

}

static long devAiNI6123RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiNI6123RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiNI6123RawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiNI6123RawReaddpvt->pSTDdev;
/*	if( pdevAiNI6123RawReaddpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else */
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiNI6123RawRead_init_record(aiRecord *precord)
{
	ST_dpvt *pdevAiNI6123RawReaddpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiNI6123RawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiNI6123RawReaddpvt->devName,
				   pdevAiNI6123RawReaddpvt->arg0,
				   pdevAiNI6123RawReaddpvt->arg1);
#if 0
	epicsPrintf("devAiNI6123RawRead arg num: %d: %s %s %s\n",i, pdevAiNI6123RawReaddpvt->devName, 
								pdevAiNI6123RawReaddpvt->arg0,
								pdevAiNI6123RawReaddpvt->arg1);
#endif
			pdevAiNI6123RawReaddpvt->pSTDdev = get_STDev_from_Dev_Type("NI6123");
			if(!pdevAiNI6123RawReaddpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiNI6123RawRead (init_record) Illegal INP field: task name");
				free(pdevAiNI6123RawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			if( i > 2)
			{
				pdevAiNI6123RawReaddpvt->pchannelConfig = drvNI6123_find_channelConfig(pdevAiNI6123RawReaddpvt->devName,
											         pdevAiNI6123RawReaddpvt->arg0);
				if(!pdevAiNI6123RawReaddpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiNI6123RawRead (init_record) Illegal INP field: channel name");
					free(pdevAiNI6123RawReaddpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiNI6123RawRead (init_record) Illegal INP field");
			free(pdevAiNI6123RawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


		if(!strcmp(pdevAiNI6123RawReaddpvt->arg0, AI_DAQ_MAXIMUM_VOLTAGE_R_STR)) {
			pdevAiNI6123RawReaddpvt->ind = AI_DAQ_MAXIMUM_VOLTAGE_R;
			}
		else if( !strcmp(pdevAiNI6123RawReaddpvt->arg0, AI_DAQ_MINIMUM_VOLTAGE_R_STR)) {
			pdevAiNI6123RawReaddpvt->ind = AI_DAQ_MINIMUM_VOLTAGE_R;
			}
		else if( !strcmp(pdevAiNI6123RawReaddpvt->arg0, AI_DAQ_SAMPLING_RATE_R_STR)) {
			pdevAiNI6123RawReaddpvt->ind = AI_DAQ_SAMPLING_RATE_R;
			}
		else if( !strcmp(pdevAiNI6123RawReaddpvt->arg0, AI_DAQ_SAMPLING_TIME_R_STR)) {
			pdevAiNI6123RawReaddpvt->ind = AI_DAQ_SAMPLING_TIME_R;
			}


	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiNI6123RawReaddpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6123RawRead_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pdevAiNI6123RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	ST_NI6123_dev *pNI6123 = NULL;

	if(!pdevAiNI6123RawReaddpvt) return 0;

	pSTDdev		= pdevAiNI6123RawReaddpvt->pSTDdev;

	pNI6123 = (ST_NI6123_dev*)pSTDdev->pUser;
	

	switch(pdevAiNI6123RawReaddpvt->ind) {

		case AI_DAQ_MAXIMUM_VOLTAGE_R:
			precord->val = pNI6123->maxVal;
			break;

		case AI_DAQ_MINIMUM_VOLTAGE_R:
			precord->val = pNI6123->minVal;
			break;
			
		case AI_DAQ_SAMPLING_RATE_R:
			precord->val = pNI6123->smp_rate;
			pSTDdev->ST_Base.nSamplingRate = (int)pNI6123->smp_rate;
			break;

		case AI_DAQ_SAMPLING_TIME_R:
			precord->val = pNI6123->sample_time;
			pSTDdev->ST_Base.dT1[0] = (double)pNI6123->sample_time;
			break;

		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevAiNI6123RawReaddpvt->ind); 
			break;
	}
	return (2);
}



static long devBoNI6123_init_record(boRecord *precord)
{
		ST_dpvt *pdevNI6123Controldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
		int i;
	
		switch(precord->out.type) {
			case INST_IO:
				strcpy(pdevNI6123Controldpvt->recordName, precord->name);
				i = sscanf(precord->out.value.instio.string, "%s",
						   pdevNI6123Controldpvt->arg0);
	
#if 0
	
		epicsPrintf("devAoACQ196Control arg num: %d: %s\n",i, pdevNI6123Controldpvt->arg0);
	
#endif
				ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
				pdevNI6123Controldpvt->pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
				if(!pdevNI6123Controldpvt->pSTDdev) {
					recGblRecordError(S_db_badField, (void*) precord, 
							  "devAoNI6123Control (init_record) Illegal INP field: task_name");
					free(pdevNI6123Controldpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			
					   
				break;
			default:
				recGblRecordError(S_db_badField,(void*) precord,
						  "devAoNI6123Control (init_record) Illegal OUT field");
				free(pdevNI6123Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
				
		}


	if(!strcmp(pdevNI6123Controldpvt->arg0, BO_SXR_AUTO_RUN_STR)) {
		pdevNI6123Controldpvt->ind = BO_SXR_AUTO_RUN;
	} 
	else if(!strcmp(pdevNI6123Controldpvt->arg0, BO_SXR_LOCAL_START_STR)) {
		pdevNI6123Controldpvt->ind = BO_SXR_LOCAL_START;
	}

	else {
		pdevNI6123Controldpvt->ind = -1;
		epicsPrintf("ERROR! devBoNI6123_init_record: arg0 \"%s\" \n",  pdevNI6123Controldpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevNI6123Controldpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoNI6123_write_bo(boRecord *precord)
{
#if PRINT_PHASE_INFO
		static int	 kkh_cnt;
#endif
		ST_dpvt 	   *pdevNI6123Controldpvt = (ST_dpvt*) precord->dpvt;
		ST_STD_device  *pSTDdev;
		ST_threadCfg   *pControlThreadConfig;
		ST_threadQueueData		   qData;
	
	
	
		if(!pdevNI6123Controldpvt || precord->udf == TRUE) {
			precord->pact = TRUE;
			return 0;
		}
	
		pSTDdev 		= pdevNI6123Controldpvt->pSTDdev;
		pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
		qData.param.pSTDdev 	= pSTDdev;
		qData.param.precord 	= (struct dbCommon *)precord;
		qData.param.setValue		= precord->val;
	
	
	
			/* db processing: phase I */
		if(precord->pact == FALSE) {	
			precord->pact = TRUE;
			
#if PRINT_PHASE_INFO
			epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
													epicsThreadGetNameSelf());
#endif
		switch(pdevNI6123Controldpvt->ind) {
			case BO_SXR_AUTO_RUN:
				qData.pFunc = devNI6123_BO_AUTO_RUN;
				break;
			case BO_SXR_LOCAL_START:
				qData.pFunc = devNI6123_BO_LOCAL_START;
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

		switch(pdevNI6123Controldpvt->ind) {

			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

static void devNI6123_BO_AUTO_RUN(ST_execParam *pParam)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	ST_NI6123_dev *pNI6123 = NULL;

	if( pAdminCfg->StatusAdmin & TASK_DAQ_STARTED ) 
		{
			epicsPrintf("ERROR!   System running! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
			return;
		}


	if(pParam->setValue)
		{
		if( !(pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE) ) 
		{
			epicsPrintf("ERROR!   System busy! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
			return;
		}
			while(pSTDdev) {
			/*  add here user modified code. */
			    pSTDdev->StatusDev |= TASK_SYSTEM_READY;
			    if(!strcmp(pSTDdev->devType, "NI6123"))
	    			{
	    				pNI6123 = pSTDdev->pUser;
						pNI6123->auto_run_flag = 1;
	    			}

				
				pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			}
		}
	else if(!pParam->setValue)
		{
			while(pSTDdev) {
			/*  add here user modified code. */

			    if(!strcmp(pSTDdev->devType, "NI6123"))
	    			{
	    				pNI6123 = pSTDdev->pUser;
						pNI6123->auto_run_flag = 0;
	    			}


				pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			}
				
				pAdminCfg->n8EscapeWhile = 0;
				epicsThreadSleep(3.0);
				pAdminCfg->n8EscapeWhile = 1;
				admin_all_taskStatus_reset();
				scanIoRequest(pAdminCfg->ioScanPvt_status);
		}
	notify_refresh_admin_status();
	epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, pAdminCfg->n8EscapeWhile);
}

static void devNI6123_BO_LOCAL_START(ST_execParam *pParam)
{
		ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
		struct dbCommon *precord = pParam->precord;
		ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		char mkdirbuf[255];
		memset(mkdirbuf, 0, sizeof(mkdirbuf));
		if(pParam->setValue) {
		if( pAdminCfg->StatusAdmin & TASK_DAQ_STARTED ) 
		{
			epicsPrintf("ERROR!   System running! please check again. (0x%x)\n", pAdminCfg->StatusAdmin);
			return;
		}
	
	/**********************************************
		user add here modified code. 
	***********************************************/
		while(pSTDdev) 
	
		{
	
			pSTDdev->StatusDev |= TASK_SYSTEM_READY;
	
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	
		}
		pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		notify_refresh_admin_status();
		epicsThreadSleep(1);
	
		while(pSTDdev) 
	
		{
			
	
			pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
			pSTDdev->StatusDev |= TASK_ARM_ENABLED;
	
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
		}
		notify_refresh_admin_status();
		DBproc_put("SXR_DC_SUPPLY1_CH1_ON", "1");
		epicsThreadSleep(2);
		DBproc_put("SXR_DC_SUPPLY1_CH2_ON", "1");
		epicsThreadSleep(2);
		DBproc_put("SXR_DC_SUPPLY2_CH1_ON", "1");
		epicsThreadSleep(2);
		DBproc_put("SXR_DC_SUPPLY2_CH2_ON", "1");
		epicsThreadSleep(2);

		pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
		sprintf(mkdirbuf, "%s%d", STR_LOCAL_DATA_DIR, pAdminCfg->ST_Base.shotNumber);
		mkdir(mkdirbuf, NULL);

		while(pSTDdev)
			{
	
			flush_STDdev_to_mdsplus(pSTDdev);
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_DAQ_STARTED;
			epicsThreadSleep(0.3);
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			}
		/* >>>>>>>>>>>>>>>>>>>>>>>>>>>> user edit  untill here.. */
		notify_refresh_admin_status();
		epicsPrintf("%s: %s: %d \n", pAdminCfg->taskName, precord->name, (int)pParam->setValue);
		DBproc_put("SXR_LOCAL_START", "0"); 
		}
}

