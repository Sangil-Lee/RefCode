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

#include "drvNI6122.h" 

/* auto Run function */
static void devNI6122_BO_AUTO_RUN(ST_execParam *pParam)
{
}

static void devNI6122_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6122_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

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

static void devNI6122_BO_CH_ENABLE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev = pParam->pSTDdev;
	ST_NI6122		*pNI6122 = pSTDdev->pUser;
	int	chNo	= pParam->n32Arg0;

	kLog (K_MON, "[devNI6122_BO_CH_ENABLE] %s : ch(%d) enable(%d -> %f)\n",
			pSTDdev->taskName, chNo, pNI6122->ST_Ch[chNo].enabled, pParam->setValue);

	pNI6122->ST_Ch[chNo].enabled = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

static void devNI6122_AO_DAQ_MIN_VOLT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6122 *pNI6122 = NULL;

	kLog (K_MON,"[devNI6122_AO_DAQ_MIN_VOLT] minVolt(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNI6122_AO_DAQ_MIN_VOLT] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else {
			pNI6122 = pSTDdev->pUser;
			pNI6122->minVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			kLog (K_DATA,"[devNI6122_AO_DAQ_MIN_VOLT] task(%s) minVolt(%.f)\n",
					pSTDdev->taskName, pNI6122->minVal);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
static void devNI6122_AO_DAQ_MAX_VOLT(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6122 *pNI6122 = NULL;

	kLog (K_MON,"[devNI6122_AO_DAQ_MAX_VOLT] maxVolt(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNI6122_AO_DAQ_MAX_VOLT] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else {
			pNI6122 = pSTDdev->pUser;
			pNI6122->maxVal = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			kLog (K_DATA,"[devNI6122_AO_DAQ_MAX_VOLT] task(%s) maxVolt(%.f)\n",
					pSTDdev->taskName, pNI6122->maxVal);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6122_AO_DAQ_GAIN(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6122 *pNI6122 = NULL;

	kLog (K_MON, "[devNI6122_AO_DAQ_GAIN] gain(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_ERR, "[devNI6122_AO_DAQ_GAIN] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );		
		}
		else {
			pNI6122 = pSTDdev->pUser;
			pNI6122->gain = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

			kLog (K_DEBUG, "[devNI6122_AO_DAQ_GAIN] task(%s) gain(%.f)\n",
					pSTDdev->taskName, pNI6122->gain);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6122_AO_DAQ_OFFSET(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6122 *pNI6122 = NULL;

	kLog (K_TRACE, "[devNI6122_AO_DAQ_OFFSET] beam_pulse(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_ERR, "[devNI6122_AO_DAQ_OFFSET] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );		
		}
		else {
			pNI6122 = pSTDdev->pUser;
			pNI6122->offset = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

			kLog (K_DEBUG, "[devNI6122_AO_DAQ_OFFSET] task(%s) offset(%.f)\n",
					pSTDdev->taskName, pNI6122->offset);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static long devAoNI6122_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoNI6122_init_record] %d: %s %s %s\n",
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
						"devAoNI6122Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoNI6122Control (init_record) Illegal OUT field");
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

static long devAoNI6122_write_ao(aoRecord *precord)
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

		kLog (K_DEBUG, "[devAoNI6122_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_MIN_VOLT:
				qData.pFunc = devNI6122_AO_DAQ_MIN_VOLT;
				break;
			case AO_DAQ_MAX_VOLT:
				qData.pFunc = devNI6122_AO_DAQ_MAX_VOLT;
				break;
			case AO_DAQ_GAIN:
				qData.pFunc = devNI6122_AO_DAQ_GAIN;
				break;
			case AO_DAQ_OFFSET:
				qData.pFunc = devNI6122_AO_DAQ_OFFSET;
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
		kLog (K_DEBUG, "[devAoNI6122_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiNI6122_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiNI6122_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiNI6122 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiNI6122 (init_record) Illegal INP field");
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
		kLog (K_ERR, "[devAiNI6122_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6122_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiNI6122_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NI6122 *pNI6122 = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNI6122 = (ST_NI6122*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			kLog (K_DATA, "[devAiNI6122_read_ai.AI_READ_STATE] %s : value(%f)\n", pSTDdev->taskName, precord->val);
			break;

		case AI_RAW_VALUE :
			precord->val = pNI6122->ST_Ch[chId].rawValue;
			kLog (K_DATA, "[devAiNI6122_read_ai.AI_RAW_VALUE] %s : ch(%d) raw(%d) (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VALUE :
			precord->val = (float)pNI6122->ST_Ch[chId].euValue;
			//precord->val = pNI6122->ST_Ch[chId].euValue * pNI6122->gain + pNI6122->offset;
			kLog (K_DATA, "[devAiNI6122_read_ai.AI_VALUE] %s : ch(%d) value(%f) eu(%f) gain(%f) offset(%f)\n",
					pSTDdev->taskName, chId, precord->val,
					pNI6122->ST_Ch[chId].euValue, pNI6122->gain, pNI6122->offset);
			break;

		default:
			kLog (K_ERR, "[devAiNI6122_read_ai] %s : ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devBoNI6122_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
#if 0
			i = sscanf(precord->out.value.instio.string, "%s",
					pSTdpvt->arg0);

			kLog (K_INFO, "[devBoNI6122_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);
#else
			i = sscanf(precord->out.value.instio.string, "%s %s %s", pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			if (3 == i) {
				pSTdpvt->n32Arg0 = strtoul(pSTdpvt->arg1, NULL, 0);
				pSTdpvt->pSTDdev = get_STDev_from_name (pSTdpvt->devName);
			}
			else {
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(get_master()->pList_DeviceTask);
			}

			kLog (K_MON, "[devBoNI6122_init_record] arg num: %d: %s %s %s\n", i,
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);
#endif

			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoNI6122Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoNI6122Control (init_record) Illegal OUT field");
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
		kLog (K_ERR, "ERROR! devBoNI6122_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoNI6122_write_bo(boRecord *precord)
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

		kLog (K_INFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case BO_AUTO_RUN:
				qData.pFunc = devNI6122_BO_AUTO_RUN;
				break;

			case BO_DAQ_STOP:
				qData.pFunc = devNI6122_BO_DAQ_STOP;
				break;

			case BO_CH_ENABLE:
				qData.pFunc = devNI6122_BO_CH_ENABLE;
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

static long devMbbiNI6122_init_record(mbbiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devMbbiNI6122_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pMaster = get_master();
			if (!pSTdpvt->pMaster) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devMbbiNI6122 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devMbbiNI6122 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, MBBI_AMP_STAT_STR)) {
		pSTdpvt->ind	= MBBI_AMP_STAT;
	}
	else if (!strcmp (pSTdpvt->arg0, MBBI_DIG_STAT_STR)) {
		pSTdpvt->ind	= MBBI_DIG_STAT;
	}
	else if (!strcmp (pSTdpvt->arg0, MBBI_MDS_STAT_STR)) {
		pSTdpvt->ind	= MBBI_MDS_STAT;
	}
	else {
		kLog (K_ERR, "[devMbbiNI6122_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 0;
}

static long devMbbiNI6122_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devMbbiNI6122_read_mbbi(mbbiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_MASTER	*pMaster;

	if(!pSTdpvt) return 0;

	pMaster	= pSTdpvt->pMaster;

	switch (pSTdpvt->ind) {
		case MBBI_AMP_STAT :
			precord->val = gIocVar.amp_stat;
			kLog (K_DATA, "[devMbbiNI6122_read_mbbi.MBBI_AMP_STAT] value(0x%x)\n", precord->val);
			break;

		case MBBI_DIG_STAT :
			precord->val = gIocVar.dig_stat;
			kLog (K_DATA, "[devMbbiNI6122_read_mbbi.MBBI_DIG_STAT] value(0x%x)\n", precord->val);
			break;

		case MBBI_MDS_STAT :
			precord->val = gIocVar.mds_stat;
			kLog (K_DATA, "[devMbbiNI6122_read_mbbi.MBBI_MDS_STAT] value(0x%x)\n", precord->val);
			break;

		default:
			kLog (K_ERR, "[devMbbiNI6122_read_mbbi] ind(%d) is invalid\n", pSTdpvt->ind); 
			break;
	}

	return (2);	/*(0,2)=>(success, success no convert)*/
}

BINARYDSET	devAoNI6122Control = { 6, NULL, NULL, devAoNI6122_init_record, NULL, devAoNI6122_write_ao, NULL };
BINARYDSET	devBoNI6122Control = { 5, NULL, NULL, devBoNI6122_init_record, NULL, devBoNI6122_write_bo };
BINARYDSET	devAiNI6122RawRead = { 6, NULL, NULL, devAiNI6122_init_record, devAiNI6122_get_ioint_info, devAiNI6122_read_ai, NULL };
BINARYDSET	devMbbiNI6122Read  = { 6, NULL, NULL, devMbbiNI6122_init_record, devMbbiNI6122_get_ioint_info, devMbbiNI6122_read_mbbi, NULL };

epicsExportAddress(dset, devAoNI6122Control);
epicsExportAddress(dset, devBoNI6122Control);
epicsExportAddress(dset, devAiNI6122RawRead);
epicsExportAddress(dset, devMbbiNI6122Read);

