#include <sys/stat.h>
#include <sys/types.h>

#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "waveformRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "drvNI6250.h" 

/* auto Run function */
static void devNI6250_BO_AUTO_RUN(ST_execParam *pParam)
{
}

static void devNI6250_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6250_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	resetDeviceAll ();

	notify_refresh_master_status();

	scanIoRequest(get_master()->ioScanPvt_status);

	DBproc_put(PV_SYS_ARMING_STR, "0");
	DBproc_put(PV_SYS_RUN_STR, "0");
	DBproc_put(PV_DAQ_STOP_STR, "0");
}

static void devNI6250_AO_DAQ_MIN_VOLT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6250 *pNI6250 = NULL;

	kLog (K_MON,"[devNI6250_AO_DAQ_MIN_VOLT] minVolt(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNI6250_AO_DAQ_MIN_VOLT] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else {
			pNI6250 = pSTDdev->pUser;
			pNI6250->minVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			kLog (K_DATA,"[devNI6250_AO_DAQ_MIN_VOLT] task(%s) minVolt(%.f)\n",
					pSTDdev->taskName, pNI6250->minVal);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
static void devNI6250_AO_DAQ_MAX_VOLT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6250 *pNI6250 = NULL;

	kLog (K_MON,"[devNI6250_AO_DAQ_MAX_VOLT] maxVolt(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNI6250_AO_DAQ_MAX_VOLT] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else {
			pNI6250 = pSTDdev->pUser;
			pNI6250->maxVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			kLog (K_DATA,"[devNI6250_AO_DAQ_MAX_VOLT] task(%s) maxVolt(%.f)\n",
					pSTDdev->taskName, pNI6250->maxVal);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6250_AO_DAQ_GAIN(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6250 *pNI6250 = NULL;

	kLog (K_MON, "[devNI6250_AO_DAQ_GAIN] gain(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_ERR, "[devNI6250_AO_DAQ_GAIN] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );		
		}
		else {
			pNI6250 = pSTDdev->pUser;
			pNI6250->gain = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

			kLog (K_DEBUG, "[devNI6250_AO_DAQ_GAIN] task(%s) gain(%.f)\n",
					pSTDdev->taskName, pNI6250->gain);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6250_AO_DAQ_OFFSET(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6250 *pNI6250 = NULL;

	kLog (K_TRACE, "[devNI6250_AO_DAQ_OFFSET] offset(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_ERR, "[devNI6250_AO_DAQ_OFFSET] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );		
		}
		else {
			pNI6250 = pSTDdev->pUser;
			pNI6250->offset = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

			kLog (K_DEBUG, "[devNI6250_AO_DAQ_OFFSET] task(%s) offset(%.f)\n",
					pSTDdev->taskName, pNI6250->offset);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6250_AO_DAQ_CUTOFF_FREQ(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6250 *pNI6250 = NULL;

	kLog (K_TRACE, "[devNI6250_AO_DAQ_CUTOFF_FREQ] cutoff_freq(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_ERR, "[devNI6250_AO_DAQ_CUTOFF_FREQ] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );		
		}
		else {
			pNI6250 = pSTDdev->pUser;
			pNI6250->cutoff_freq = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

			kLog (K_DEBUG, "[devNI6250_AO_DAQ_CUTOFF_FREQ] task(%s) offset(%.f)\n",
					pSTDdev->taskName, pNI6250->offset);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static long devAoNI6250_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoNI6250_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			if (1 == i) {
				ST_MASTER *pMaster = get_master();
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			}
			else if (3 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
			}

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devAoNI6250Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoNI6250Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if (!strcmp(pSTdpvt->arg0, AO_DAQ_MIN_VOLT_STR)) {
		pSTdpvt->ind            = AO_DAQ_MIN_VOLT;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_MAX_VOLT_STR)) {
		pSTdpvt->ind            = AO_DAQ_MAX_VOLT;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_GAIN_STR)) {
		pSTdpvt->ind            = AO_DAQ_GAIN;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_OFFSET_STR)) {
		pSTdpvt->ind            = AO_DAQ_OFFSET;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_CUTOFF_FREQ_STR)) {
		pSTdpvt->ind            = AO_DAQ_CUTOFF_FREQ;
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}

static long devAoNI6250_write_ao(aoRecord *precord)
{
	ST_dpvt        *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData         qData;

	if (!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev					= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;

	qData.pFunc				= NULL;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;
	qData.param.n32Arg0		= pSTdpvt->n32Arg0;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_DEBUG, "[devAoNI6250_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_MIN_VOLT:
				qData.pFunc = devNI6250_AO_DAQ_MIN_VOLT;
				break;
			case AO_DAQ_MAX_VOLT:
				qData.pFunc = devNI6250_AO_DAQ_MAX_VOLT;
				break;
			case AO_DAQ_GAIN:
				qData.pFunc = devNI6250_AO_DAQ_GAIN;
				break;
			case AO_DAQ_OFFSET:
				qData.pFunc = devNI6250_AO_DAQ_OFFSET;
				break;
			case AO_DAQ_CUTOFF_FREQ:
				qData.pFunc = devNI6250_AO_DAQ_CUTOFF_FREQ;
				break;
		}

		if (NULL != qData.pFunc) {
			epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
					(void*) &qData,
					sizeof(ST_threadQueueData));
		}

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		kLog (K_DEBUG, "[devAoNI6250_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiNI6250_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiNI6250_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiNI6250 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiNI6250 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (2 == i) {
		if (!strcmp (pSTdpvt->arg0, AI_READ_STATE_STR)) {
			pSTdpvt->ind		= AI_READ_STATE;
		}
	}
	else if (3 == i) {
		if (!strcmp (pSTdpvt->arg1, AI_RAW_VALUE_STR)) {
			pSTdpvt->ind		= AI_RAW_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
		else if (!strcmp (pSTdpvt->arg1, AI_VALUE_STR)) {
			pSTdpvt->ind		= AI_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
	}
	else {
		kLog (K_ERR, "[devAiNI6250_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6250_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	if (AI_READ_STATE == pSTdpvt->ind) {
		*ioScanPvt	= pSTDdev->ioScanPvt_status;
	}
	else {
		*ioScanPvt	= pSTDdev->ioScanPvt_userCall;
	}

	return 0;
}

static long devAiNI6250_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NI6250 *pNI6250 = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNI6250 = (ST_NI6250*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			kLog (K_DATA, "[devAiNI6250_read_ai.AI_READ_STATE] %s : value(%f)\n", pSTDdev->taskName, precord->val);
			break;

		case AI_RAW_VALUE :
			precord->val = pNI6250->ST_Ch[chId].rawValue;
			kLog (K_DATA, "[devAiNI6250_read_ai.AI_RAW_VALUE] %s : ch(%d) raw(%d) (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VALUE :
			precord->val = (float)pNI6250->ST_Ch[chId].euValue;
			//precord->val = pNI6250->ST_Ch[chId].euValue * pNI6250->gain + pNI6250->offset;
			kLog (K_DATA, "[devAiNI6250_read_ai.AI_VALUE] %s : ch(%d) value(%f) eu(%f) gain(%f) offset(%f)\n",
					pSTDdev->taskName, chId, precord->val,
					pNI6250->ST_Ch[chId].euValue, pNI6250->gain, pNI6250->offset);
			break;

		default:
			kLog (K_ERR, "[devAiNI6250_read_ai] %s : ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devBoNI6250_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s",
					pSTdpvt->arg0);

			kLog (K_INFO, "[devBoNI6250_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);

			ST_MASTER *pMaster = get_master();
			pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoNI6250Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoNI6250Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	if (!strcmp(pSTdpvt->arg0, BO_AUTO_RUN_STR)) {
		pSTdpvt->ind = BO_AUTO_RUN;
	}
	else if (!strcmp(pSTdpvt->arg0, BO_DAQ_STOP_STR)) {
		pSTdpvt->ind = BO_DAQ_STOP;
	} 
	else {
		pSTdpvt->ind = -1;
		kLog (K_ERR, "ERROR! devBoNI6250_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoNI6250_write_bo(boRecord *precord)
{
	ST_dpvt 	   *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData		   qData;

	if(!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev 				= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev 	= pSTDdev;
	qData.param.precord 	= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_INFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case BO_AUTO_RUN:
				qData.pFunc = devNI6250_BO_AUTO_RUN;
				break;

			case BO_DAQ_STOP:
				qData.pFunc = devNI6250_BO_DAQ_STOP;
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
	if(precord->pact == TRUE) {
		kLog (K_INFO, "db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pSTdpvt->ind) {
			default:
				break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

BINARYDSET	devAoNI6250Control = { 6, NULL, NULL, devAoNI6250_init_record, NULL, devAoNI6250_write_ao, NULL };
BINARYDSET	devBoNI6250Control = { 5, NULL, NULL, devBoNI6250_init_record, NULL, devBoNI6250_write_bo };
BINARYDSET	devAiNI6250RawRead = { 6, NULL, NULL, devAiNI6250_init_record, devAiNI6250_get_ioint_info, devAiNI6250_read_ai, NULL };

epicsExportAddress(dset, devAoNI6250Control);
epicsExportAddress(dset, devBoNI6250Control);
epicsExportAddress(dset, devAiNI6250RawRead);

