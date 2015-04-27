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

#include "drvNI6220.h" 

/* auto Run function */
static void devNI6220_BO_AUTO_RUN(ST_execParam *pParam)
{
}

static void devNI6220_BO_DAQ_STOP(ST_execParam *pParam)
{
	kuDebug (kuMON, "[devNI6220_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

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

static void devNI6220_BO_CH_ENABLE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev = pParam->pSTDdev;
	ST_NI6220		*pNI6220 = pSTDdev->pUser;
	int	chNo	= pParam->n32Arg0;

	kuDebug (kuMON, "[devNI6220_BO_CH_ENABLE] %s : ch(%d) enable(%d -> %f)\n",
			pSTDdev->taskName, chNo, pNI6220->ST_Ch[chNo].enabled, pParam->setValue);

	pNI6220->ST_Ch[chNo].enabled = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

static void devNI6220_AO_DAQ_MIN_VOLT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6220 *pNI6220 = NULL;

	kuDebug (kuMON,"[devNI6220_AO_DAQ_MIN_VOLT] minVolt(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kuDebug (kuDATA,"[devNI6220_AO_DAQ_MIN_VOLT] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else {
			pNI6220 = pSTDdev->pUser;
			pNI6220->minVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			kuDebug (kuDATA,"[devNI6220_AO_DAQ_MIN_VOLT] task(%s) minVolt(%.f)\n",
					pSTDdev->taskName, pNI6220->minVal);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
static void devNI6220_AO_DAQ_MAX_VOLT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6220 *pNI6220 = NULL;

	kuDebug (kuMON,"[devNI6220_AO_DAQ_MAX_VOLT] maxVolt(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kuDebug (kuDATA,"[devNI6220_AO_DAQ_MAX_VOLT] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else {
			pNI6220 = pSTDdev->pUser;
			pNI6220->maxVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			kuDebug (kuDATA,"[devNI6220_AO_DAQ_MAX_VOLT] task(%s) maxVolt(%.f)\n",
					pSTDdev->taskName, pNI6220->maxVal);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6220_AO_DAQ_GAIN(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev = pParam->pSTDdev;
	ST_NI6220		*pNI6220 = pSTDdev->pUser;
	int				chNo	 = pParam->n32Arg0;

	kuDebug (kuMON, "[devNI6220_AO_DAQ_GAIN] %s : ch(%d) gain(%f -> %f)\n",
			pSTDdev->taskName, chNo, pNI6220->ST_Ch[chNo].gain, pParam->setValue);

	pNI6220->ST_Ch[chNo].gain = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

static void devNI6220_AO_DAQ_OFFSET(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev = pParam->pSTDdev;
	ST_NI6220		*pNI6220 = pSTDdev->pUser;
	int				chNo	 = pParam->n32Arg0;

	kuDebug (kuMON, "[devNI6220_AO_DAQ_OFFSET] %s : ch(%d) offset(%f -> %f)\n",
			pSTDdev->taskName, chNo, pNI6220->ST_Ch[chNo].offset, pParam->setValue);

	pNI6220->ST_Ch[chNo].offset = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

static long devAoNI6220_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			kuDebug (kuINFO, "[devAoNI6220_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			if (1 == i) {
				ST_MASTER *pMaster = get_master();
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			}
			else if (3 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
				pSTdpvt->n32Arg0 = strtoul(pSTdpvt->arg1, NULL, 0);
			}

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devAoNI6220Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoNI6220Control (init_record) Illegal OUT field");
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

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}

static long devAoNI6220_write_ao(aoRecord *precord)
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

		kuDebug (kuDEBUG, "[devAoNI6220_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_MIN_VOLT:
				qData.pFunc = devNI6220_AO_DAQ_MIN_VOLT;
				break;
			case AO_DAQ_MAX_VOLT:
				qData.pFunc = devNI6220_AO_DAQ_MAX_VOLT;
				break;
			case AO_DAQ_GAIN:
				qData.pFunc = devNI6220_AO_DAQ_GAIN;
				break;
			case AO_DAQ_OFFSET:
				qData.pFunc = devNI6220_AO_DAQ_OFFSET;
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
		kuDebug (kuDEBUG, "[devAoNI6220_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiNI6220_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kuDebug (kuINFO, "[devAiNI6220_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiNI6220 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiNI6220 (init_record) Illegal INP field");
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
		else if (!strcmp (pSTdpvt->arg1, AI_VOLT_VALUE_STR)) {
			pSTdpvt->ind		= AI_VOLT_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
		else if (!strcmp (pSTdpvt->arg1, AI_VALUE_STR)) {
			pSTdpvt->ind		= AI_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
		else if (!strcmp (pSTdpvt->arg1, AI_CALC_VALUE_STR)) {
			pSTdpvt->ind		= AI_CALC_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
	}
	else {
		kuDebug (kuERR, "[devAiNI6220_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6220_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiNI6220_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NI6220 *pNI6220 = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNI6220 = (ST_NI6220*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			kuDebug (kuDATA, "[devAiNI6220_read_ai.AI_READ_STATE] %s : value(%f)\n", pSTDdev->taskName, precord->val);
			break;

		case AI_RAW_VALUE :
			precord->val = pNI6220->ST_Ch[chId].rawValue;
			kuDebug (kuDATA, "[devAiNI6220_read_ai.AI_RAW_VALUE] %s : ch(%d) raw(%d) (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VOLT_VALUE :
			precord->val = (float)pNI6220->ST_Ch[chId].euVolt;
			kuDebug (kuDATA, "[devAiNI6220_read_ai.AI_VOLT_VALUE] %s : ch(%d) value(%f) euVolt(%f)\n",
					pSTDdev->taskName, chId, precord->val, pNI6220->ST_Ch[chId].euVolt);
			break;

		case AI_VALUE :
			precord->val = pNI6220->ST_Ch[chId].euValue;
			kuDebug (kuDATA, "[devAiNI6220_read_ai.AI_VALUE] %s : ch(%d) value(%f) euVolt(%f) gain(%f) offset(%f)\n",
					pSTDdev->taskName, chId, precord->val,
					pNI6220->ST_Ch[chId].euVolt, pNI6220->ST_Ch[chId].gain, pNI6220->ST_Ch[chId].offset);
			break;

		case AI_CALC_VALUE :
			precord->val = pNI6220->ST_Ch[chId].calcValue;
			kuDebug (kuDATA, "[devAiNI6220_read_ai.AI_CALC_VALUE] %s : ch(%d) calc(%f) (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		default:
			kuDebug (kuERR, "[devAiNI6220_read_ai] %s : ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devBoNI6220_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			i = sscanf(precord->out.value.instio.string, "%s %s %s", pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			if (3 == i) {
				pSTdpvt->n32Arg0 = strtoul(pSTdpvt->arg1, NULL, 0);
				pSTdpvt->pSTDdev = get_STDev_from_name (pSTdpvt->devName);
			}
			else {
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(get_master()->pList_DeviceTask);
			}

			kuDebug (kuMON, "[devBoNI6220_init_record] arg num: %d: %s %s %s\n", i,
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoNI6220Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoNI6220Control (init_record) Illegal OUT field");
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
	else if (!strcmp(pSTdpvt->arg0, BO_CH_ENABLE_STR)) {
		pSTdpvt->ind = BO_CH_ENABLE;
	} 
	else {
		pSTdpvt->ind = -1;
		kuDebug (kuERR, "ERROR! devBoNI6220_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoNI6220_write_bo(boRecord *precord)
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
	qData.param.n32Arg0		= pSTdpvt->n32Arg0;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kuDebug (kuINFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case BO_AUTO_RUN:
				qData.pFunc = devNI6220_BO_AUTO_RUN;
				break;

			case BO_DAQ_STOP:
				qData.pFunc = devNI6220_BO_DAQ_STOP;
				break;

			case BO_CH_ENABLE:
				qData.pFunc = devNI6220_BO_CH_ENABLE;
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
		kuDebug (kuINFO, "db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());

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

static long devMbbiNI6220_init_record(mbbiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			i = sscanf(precord->inp.value.instio.string, "%s", pSTdpvt->arg0);

			kuDebug (kuINFO, "[devMbbiNI6220_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pMaster = get_master();
			if (!pSTdpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devMbbiNI6220 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devMbbiNI6220 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, MBBI_DIG_STAT_STR)) {
		pSTdpvt->ind	= MBBI_DIG_STAT;
	}
	else if (!strcmp (pSTdpvt->arg0, MBBI_MDS_STAT_STR)) {
		pSTdpvt->ind	= MBBI_MDS_STAT;
	}
	else {
		kuDebug (kuERR, "[devMbbiNI6220_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 0;
}

static long devMbbiNI6220_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER	*pMaster;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pMaster		= pSTdpvt->pMaster;
	*ioScanPvt	= pMaster->ioScanPvt_status;

	return 0;
}

static long devMbbiNI6220_read_mbbi(mbbiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER	*pMaster;

	if(!pSTdpvt) return 0;

	pMaster	= pSTdpvt->pMaster;

	switch (pSTdpvt->ind) {
		default:
			kuDebug (kuERR, "[devMbbiNI6220_read_mbbi] ind(%d) is invalid\n", pSTdpvt->ind); 
			break;
	}

	return (2);	/*(0,2)=>(success, success no convert)*/
}

BINARYDSET	devAoNI6220Control = { 6, NULL, NULL, devAoNI6220_init_record, NULL, devAoNI6220_write_ao, NULL };
BINARYDSET	devBoNI6220Control = { 5, NULL, NULL, devBoNI6220_init_record, NULL, devBoNI6220_write_bo };
BINARYDSET	devAiNI6220RawRead = { 6, NULL, NULL, devAiNI6220_init_record, devAiNI6220_get_ioint_info, devAiNI6220_read_ai, NULL };
BINARYDSET	devMbbiNI6220Read  = { 6, NULL, NULL, devMbbiNI6220_init_record, devMbbiNI6220_get_ioint_info, devMbbiNI6220_read_mbbi, NULL };

epicsExportAddress(dset, devAoNI6220Control);
epicsExportAddress(dset, devBoNI6220Control);
epicsExportAddress(dset, devAiNI6220RawRead);
epicsExportAddress(dset, devMbbiNI6220Read);

