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
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "drvNI6254.h" 

static void devNI6254_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6254_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	resetDeviceAll ();

	notify_refresh_master_status();

	scanIoRequest(get_master()->ioScanPvt_status);

	DBproc_put("NB1_LODAQ_DAQ_STOP", "0");
	DBproc_put("NB1_LODAQ_TREND_RUN", "0");
	DBproc_put("NB1_LODAQ_CALC_RUN", "0");
}

static void devNI6254_BO_TREND_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6254_BO_TREND_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
		processTrendRun ();
	}
}

static void devNI6254_BO_CALC_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6254_BO_CALC_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
		processCalcRun ();
	}
}

static void devNI6254_AO_BEAM_PULSE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	int	nIonSrcNum;

	kLog (K_MON, "[devNI6254_AO_BEAM_PULSE] beam_pulse(%f)\n", pParam->setValue);

	if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
		kLog (K_ERR, "[devNI6254_AO_BEAM_PULSE] %s: System is armed! \n", pSTDdev->taskName);
		notify_error (1, "System is armed!", pSTDdev->taskName );		
	}
	else {
		nIonSrcNum	= pParam->n32Arg0;
		gIonSourceSetup[nIonSrcNum].beam_pulse	= pParam->setValue;

		kLog (K_MON, "[devNI6254_AO_BEAM_PULSE] task(%s) beam_pulse(%.f) IS(%d)\n",
				pSTDdev->taskName, gIonSourceSetup[nIonSrcNum].beam_pulse, nIonSrcNum);
	}
}

static void devNI6254_AO_INTEGRAL_STIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	int	nIonSrcNum;

	kLog (K_MON, "[devNI6254_AO_INTEGRAL_STIME] integral_stime(%f)\n", pParam->setValue);

	if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
		kLog (K_ERR, "[devNI6254_AO_INTEGRAL_STIME] %s: System is armed! \n", pSTDdev->taskName);
		notify_error (1, "System is armed!", pSTDdev->taskName );		
	}
	else {
		nIonSrcNum	= pParam->n32Arg0;
		gIonSourceSetup[nIonSrcNum].integral_stime	= pParam->setValue;

		kLog (K_MON, "[devNI6254_AO_INTEGRAL_STIME] task(%s) integral_stime(%.f) IS(%d)\n",
				pSTDdev->taskName, gIonSourceSetup[nIonSrcNum].integral_stime, nIonSrcNum);
	}
}

static void devNI6254_AO_INTEGRAL_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NI6254 *pNI6254 = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNI6254	= (ST_NI6254*)pSTDdev->pUser;

	kLog (K_MON, "[devNI6254_AO_INTEGRAL_TIME] %s : ch(%d) time(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pNI6254->ST_Ch[chId].integral_time = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static long devAoNI6254_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoNI6254_init_record] %d: %s %s %s\n",
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
						"devAoNI6254Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoNI6254Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if (!strcmp(pSTdpvt->arg0, AO_DAQ_BEAM_PULSE_STR)) {
		pSTdpvt->ind		= AO_DAQ_BEAM_PULSE;
		pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_INTEGRAL_STIME_STR)) {
		pSTdpvt->ind		= AO_INTEGRAL_STIME;
		pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_INTEGRAL_TIME_STR)) {
		pSTdpvt->ind		= AO_INTEGRAL_TIME;
		pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg1, NULL, 0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}

static long devAoNI6254_write_ao(aoRecord *precord)
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

		kLog (K_DEBUG, "[devAoNI6254_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_BEAM_PULSE:
				qData.pFunc = devNI6254_AO_BEAM_PULSE;
				break;
			case AO_INTEGRAL_STIME :
				qData.pFunc = devNI6254_AO_INTEGRAL_STIME;
				break;
			case AO_INTEGRAL_TIME :
				qData.pFunc = devNI6254_AO_INTEGRAL_TIME;
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
		kLog (K_DEBUG, "[devAoNI6254_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiNI6254_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiNI6254_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			if (1 == i) {
				strcpy (pSTdpvt->arg0, pSTdpvt->devName);
				pSTdpvt->pMaster = get_master();
			}
			else {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

				if (!pSTdpvt->pSTDdev) {
					recGblRecordError(S_db_badField, (void*) precord,
							"devAiNI6254 (init_record) Illegal INP field: task name");
					free(pSTdpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiNI6254 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (1 == i) {
	}
	else if (2 == i) {
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
		else if (!strcmp (pSTdpvt->arg1, AI_POWER_STR)) {
			pSTdpvt->ind		= AI_POWER;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
		else if (!strcmp (pSTdpvt->arg1, AI_FLOW_STR)) {
			pSTdpvt->ind		= AI_FLOW;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
	}
	else {
		kLog (K_ERR, "[devAiNI6254_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6254_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiNI6254_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NI6254 *pNI6254 = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	if (NULL != pSTDdev) {
		pNI6254 = (ST_NI6254*)pSTDdev->pUser;
	}

	switch (pSTdpvt->ind) {
		case AI_RAW_VALUE :
			precord->val = (float)pNI6254->ST_Ch[chId].rawValue;
			kLog (K_DATA, "[devAiNI6254_read_ai.AI_RAW_VALUE] %s:%d raw value: %d (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VALUE :
			precord->val = (float)pNI6254->ST_Ch[chId].euValue;
			kLog (K_DATA, "[devAiNI6254_read_ai.AI_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_POWER :
			precord->val = (float)pNI6254->ST_Ch[chId].power;
			kLog (K_DATA, "[devAiNI6254_read_ai.AI_POWER] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_FLOW :
			precord->val = (float)pNI6254->ST_Ch[chId].flow;
			kLog (K_DATA, "[devAiNI6254_read_ai.AI_FLOW] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			break;

		default:
			kLog (K_ERR, "[devAiNI6254_read_ai] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devBoNI6254_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s",
					pSTdpvt->arg0);

			kLog (K_INFO, "[devBoNI6254_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);

			ST_MASTER *pMaster = get_master();
			pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoNI6254Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoNI6254Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	if (!strcmp(pSTdpvt->arg0, BO_DAQ_STOP_STR)) {
		pSTdpvt->ind = BO_DAQ_STOP;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_TREND_RUN_STR)) {
		pSTdpvt->ind = BO_TREND_RUN;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_CALC_RUN_STR)) {
		pSTdpvt->ind = BO_CALC_RUN;
	} 
	else {
		pSTdpvt->ind = -1;
		kLog (K_ERR, "ERROR! devBoNI6254_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoNI6254_write_bo(boRecord *precord)
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
			case BO_DAQ_STOP:
				qData.pFunc = devNI6254_BO_DAQ_STOP;
				break;

			case BO_TREND_RUN:
				qData.pFunc = devNI6254_BO_TREND_RUN;
				break;

			case BO_CALC_RUN:
				qData.pFunc = devNI6254_BO_CALC_RUN;
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

static long devBiNI6254_init_record(biRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devBiNI6254_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pMaster = get_master();

			if (!pSTdpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devBiNI6254 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devBiNI6254 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (0) {
	}
	else {
		kLog (K_ERR, "[devBiNI6254_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 0;
}

static long devBiNI6254_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;

	if (!pSTdpvt) {
		ioScanPvt = NULL;
		return -1;
	}

	*ioScanPvt	= pSTdpvt->pMaster->ioScanPvt_status;

	return 0;
}

static long devBiNI6254_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;

	if(!pSTdpvt) return 0;

	switch (pSTdpvt->ind) {
		default:
			kLog (K_ERR, "[devBiNI6254_read_bi] ind(%d) is invalid\n", pSTdpvt->ind); 
			break;
	}

	return (2);
}

BINARYDSET	devAoNI6254Control = { 6, NULL, NULL, devAoNI6254_init_record, NULL, devAoNI6254_write_ao, NULL };
BINARYDSET	devBoNI6254Control = { 5, NULL, NULL, devBoNI6254_init_record, NULL, devBoNI6254_write_bo };
BINARYDSET	devAiNI6254RawRead = { 6, NULL, NULL, devAiNI6254_init_record, devAiNI6254_get_ioint_info, devAiNI6254_read_ai, NULL };
BINARYDSET	devBiNI6254Read    = { 6, NULL, NULL, devBiNI6254_init_record, devBiNI6254_get_ioint_info, devBiNI6254_read_bi, NULL };

epicsExportAddress(dset, devAoNI6254Control);
epicsExportAddress(dset, devBoNI6254Control);
epicsExportAddress(dset, devAiNI6254RawRead);
epicsExportAddress(dset, devBiNI6254Read);

