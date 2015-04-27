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

#include "drvNI6123.h" 

static long devWaveNI6123_init_record(waveformRecord *precord);
static long devWaveNI6123_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt);
static long devWaveNI6123_read_wave(waveformRecord *precord);




/* auto Run function */
static void devNI6123_BO_AUTO_RUN(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_TRACE, "[devNI6123_BO_AUTO_RUN] auto_run(%f)\n", pParam->setValue);

        if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
                        pMaster->StatusAdmin & TASK_IN_PROGRESS ) {
                kLog (K_ERR, "System is running! (0x%x)\n", pMaster->StatusAdmin);
                notify_error (1, "System is running! (0x%x)\n", pMaster->StatusAdmin);
                return;
        }

        if(pParam->setValue) {
                if( !(pMaster->StatusAdmin & TASK_SYSTEM_IDLE) ) {
                        kLog (K_ERR, "System is busy! (0x%x)\n", pMaster->StatusAdmin);
                        notify_error (1, "System is busy! (0x%x)\n", pMaster->StatusAdmin);
                        return;
                }

                while(pSTDdev) {
                        pSTDdev->StatusDev |= TASK_STANDBY;

                        pNI6123 = pSTDdev->pUser;
                        pNI6123->auto_run_flag = 1;

                        pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
                }
        }
        else if(!pParam->setValue) {
                while(pSTDdev) {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->auto_run_flag = 0;

                        pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
                }

                pMaster->n8EscapeWhile = 0;
                epicsThreadSleep(3.0);
                pMaster->n8EscapeWhile = 1;
                admin_all_taskStatus_reset();
                scanIoRequest(pMaster->ioScanPvt_status);
        }

        notify_refresh_master_status();
}


static void devNI6123_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6123_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	resetDeviceAll ();

	notify_refresh_master_status();

	scanIoRequest(get_master()->ioScanPvt_status);

	DBproc_put(PV_DAQ_STOP_STR, "0");
/* remove tglee
   DBproc_put(PV_TREND_RUN_STR, "0");
	DBproc_put(PV_CALC_RUN_STR, "0");
*/
}

static void devNI6123_BO_TREND_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6123_BO_TREND_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
	/*	processTrendRun ();  */
	}
}

static void devNI6123_BO_CALC_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devNI6123_BO_CALC_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
         /*	processCalcRun ();   */
	}
}



static void devNI6123_AO_DAQ_MIN_VOLT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_DATA,"[devNI6123_AO_DAQ_MIN_VOLT] minVolt(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNI6123_AO_DAQ_MIN_VOLT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->minVal = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNI6123_AO_DAQ_MIN_VOLT] task(%s) minVolt(%.f)\n",
                                        pSTDdev->taskName, pNI6123->minVal);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devNI6123_AO_DAQ_MAX_VOLT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_DATA,"[devNI6123_AO_DAQ_MAX_VOLT] minVolt(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNI6123_AO_DAQ_MAX_VOLT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->maxVal = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNI6123_AO_DAQ_MAX_VOLT] task(%s) maxVolt(%.f)\n",
                                        pSTDdev->taskName, pNI6123->maxVal);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}

static void devNI6123_AO_DAQ_SAMPLEING_RATE(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_DATA,"[devNI6123_AO_DAQ_SAMPLEING_RATE] sampleRate(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNI6123_AO_DAQ_SAMPLEING_RATE] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->sample_rate = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNI6123_AO_DAQ_SAMPLEING_RATE] task(%s) sampleRate(%.f)\n",
                                        pSTDdev->taskName, pNI6123->sample_rate);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devNI6123_AO_DAQ_SAMPLEING_TIME(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_DATA,"[devNI6123_AO_DAQ_SAMPLEING_TIME] sampleTime(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNI6123_AO_DAQ_SAMPLEING_TIME] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->sample_time = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNI6123_AO_DAQ_SAMPLEING_TIME] task(%s) sampleTime(%.f)\n",
                                        pSTDdev->taskName, pNI6123->sample_rate);
                }
                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}

static void devNI6123_AO_BEAM_PLUSE_T0(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_TRACE, "[devNI6123_AO_BEAM_PLUSE_T0] beam_pulse_t0(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_ERR, "[devNI6123_AO_BEAM_PLUSE_T0] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->beam_pulse_t0 = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DEBUG, "[devNI6123_AO_BEAM_PLUSE_T0] task(%s) T0(%.f)\n",
                                        pSTDdev->taskName, pNI6123->beam_pulse_t0);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}

static void devNI6123_AO_BEAM_PLUSE_T1(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NI6123 *pNI6123 = NULL;

        kLog (K_TRACE, "[devNI6123_AO_BEAM_PLUSE_T1] beam_pulse_t1(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_ERR, "[devNI6123_AO_BEAM_PLUSE_T1] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNI6123 = pSTDdev->pUser;
                        pNI6123->beam_pulse_t1 = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DEBUG, "[devNI6123_AO_BEAM_PLUSE_T1] task(%s) T1(%.f)\n",
                                        pSTDdev->taskName, pNI6123->beam_pulse_t1);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}

static void devNI6123_AO_BEAM_PLUSE(ST_execParam *pParam)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	ST_NI6123 *pNI6123 = NULL;

	kLog (K_TRACE, "[devNI6123_AO_BEAM_PLUSE] beam_pulse(%f)\n", pParam->setValue);

	while(pSTDdev) {
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_ERR, "[devNI6123_AO_BEAM_PLUSE] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );		
		}
		else {
			pNI6123 = pSTDdev->pUser;
			pNI6123->beam_pulse = pParam->setValue;
			scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

			kLog (K_DEBUG, "[devNI6123_AO_BEAM_PLUSE] task(%s) pulse(%.f)\n",
					pSTDdev->taskName, pNI6123->beam_pulse);
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

static void devNI6123_AO_INTEGRAL_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NI6123 *pNI6123 = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNI6123	= (ST_NI6123*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNI6123_AO_INTEGRAL_TIME] %s : ch(%d) time(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pNI6123->ST_Ch[chId].integral_time = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNI6123_AO_FLOW(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pNI6123	= (ST_NI6123*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNI6123_AO_FLOW] %s : ch(%d) flow(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pNI6123->ST_Ch[chId].flow = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNI6123_AO_FRINGE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pNI6123	= (ST_NI6123*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNI6123_AO_FRINGE] %s : ch(%d) density fringe(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pNI6123->ST_Ch[chId].fringe = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNI6123_AO_LIMITE_UP(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pNI6123	= (ST_NI6123*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNI6123_AO_LIMITE_UP] %s : ch(%d) density limite upper(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pNI6123->ST_Ch[chId].dLimUpper = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNI6123_AO_LIMITE_LOW(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pNI6123	= (ST_NI6123*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNI6123_AO_LIMITE_LOW] %s : ch(%d) density limite lower(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pNI6123->ST_Ch[chId].dLimLow = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static long devAoNI6123_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoNI6123_init_record] %d: %s %s %s\n",
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
						"devAoNI6123Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoNI6123Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if (!strcmp(pSTdpvt->arg0, AO_DAQ_BEAM_PLUSE_T0_STR)) {
		pSTdpvt->ind            = AO_DAQ_BEAM_PLUSE_T0;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_BEAM_PLUSE_T1_STR)) {
		pSTdpvt->ind            = AO_DAQ_BEAM_PLUSE_T1;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_MIN_VOLT_STR)) {
		pSTdpvt->ind            = AO_DAQ_MIN_VOLT;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_MAX_VOLT_STR)) {
		pSTdpvt->ind            = AO_DAQ_MAX_VOLT;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_SAMPLING_RATE_STR)) {
		pSTdpvt->ind            = AO_DAQ_SAMPLING_RATE;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_SAMPLING_TIME_STR)) {
		pSTdpvt->ind            = AO_DAQ_SAMPLING_TIME;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_BEAM_PLUSE_STR)) {
		pSTdpvt->ind			= AO_DAQ_BEAM_PLUSE;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_FRINGE_STR)) {
		pSTdpvt->ind			= AO_DENSITY_FRINGE;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_LIMITE_UP_STR)) {
		pSTdpvt->ind			= AO_DENSITY_LIMITE_UP;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_LIMITE_LOW_STR)) {
		pSTdpvt->ind			= AO_DENSITY_LIMITE_LOW;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_INTEGRAL_TIME_STR)) {
		pSTdpvt->ind			= AO_INTEGRAL_TIME;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_FLOW_STR)) {
		pSTdpvt->ind			= AO_FLOW;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}

static long devAoNI6123_write_ao(aoRecord *precord)
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

		kLog (K_DEBUG, "[devAoNI6123_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_MIN_VOLT:
				qData.pFunc = devNI6123_AO_DAQ_MIN_VOLT;
				break;
			case AO_DAQ_MAX_VOLT:
				qData.pFunc = devNI6123_AO_DAQ_MAX_VOLT;
				break;
			case AO_DAQ_SAMPLING_RATE:
				qData.pFunc = devNI6123_AO_DAQ_SAMPLEING_RATE;
				break;
			case AO_DAQ_SAMPLING_TIME:
				qData.pFunc = devNI6123_AO_DAQ_SAMPLEING_TIME;
				break;
			case AO_DAQ_BEAM_PLUSE_T0:
				qData.pFunc = devNI6123_AO_BEAM_PLUSE_T0;
				break;
			case AO_DAQ_BEAM_PLUSE_T1:
				qData.pFunc = devNI6123_AO_BEAM_PLUSE_T1;
				break;
			case AO_DAQ_BEAM_PLUSE:
				qData.pFunc = devNI6123_AO_BEAM_PLUSE;
				break;
			case AO_INTEGRAL_TIME :
				qData.pFunc = devNI6123_AO_INTEGRAL_TIME;
				break;
			case AO_FLOW :
				qData.pFunc = devNI6123_AO_FLOW;
				break;
			case AO_DENSITY_FRINGE :
				qData.pFunc = devNI6123_AO_FRINGE;
				break;
			case AO_DENSITY_LIMITE_UP :
				qData.pFunc = devNI6123_AO_LIMITE_UP;
				break;
			case AO_DENSITY_LIMITE_LOW :
				qData.pFunc = devNI6123_AO_LIMITE_LOW;
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
		kLog (K_DEBUG, "[devAoNI6123_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiNI6123_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiNI6123_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiNI6123 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiNI6123 (init_record) Illegal INP field");
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
		else if (!strcmp (pSTdpvt->arg0, AI_DAQ_MIN_VOLT_R_STR)) {
                        pSTdpvt->ind            = AI_DAQ_MIN_VOLT_R;
                }
                else if (!strcmp (pSTdpvt->arg0, AI_DAQ_MAX_VOLT_R_STR)) {
                        pSTdpvt->ind            = AI_DAQ_MAX_VOLT_R;
                }
                else if (!strcmp (pSTdpvt->arg0, AI_DAQ_SAMPLING_RATE_R_STR)) {
                        pSTdpvt->ind            = AI_DAQ_SAMPLING_RATE_R;
                }
                else if (!strcmp (pSTdpvt->arg0, AI_DAQ_SAMPLING_TIME_R_STR)) {
                        pSTdpvt->ind            = AI_DAQ_SAMPLING_TIME_R;
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
		else if (!strcmp (pSTdpvt->arg1, AI_DENSITY_STR)) {
			pSTdpvt->ind		= AI_DENSITY;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
	}
	else {
		kLog (K_ERR, "[devAiNI6123_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6123_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiNI6123_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NI6123 *pNI6123 = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNI6123 = (ST_NI6123*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case AI_DAQ_MIN_VOLT_R :
                        precord->val = (float)pNI6123->minVal;
                        kLog (K_DATA,"[devAiNI6123RawRead_read_ai.AI_DAQ_MIN_VOLT] %s:%d minVolt: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_MAX_VOLT_R :
                        precord->val = (float)pNI6123->maxVal;
                        kLog (K_DATA,"[devAiNI6123RawRead_read_ai.AI_DAQ_MAX_VOLT] %s:%d maxVolt: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_SAMPLING_RATE_R :
                        precord->val = (float)pNI6123->sample_rate;
                        kLog (K_DATA,"[devAiNI6123RawRead_read_ai.AI_DAQ_SAMPLING_RATE] %s:%d sampleRate: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_SAMPLING_TIME_R :
                        precord->val = (float)pNI6123->sample_time;
                        kLog (K_DATA,"[devAiNI6123RawRead_read_ai.AI_DAQ_SAMPLING_TIME] %s:%d sampleTime: %f (0x%x)\n",
                                        pSTDdev->taskName, chId, precord->val, precord->val);
                        break;
		case AI_RAW_VALUE :
			precord->val = (float)pNI6123->ST_Ch[chId].rawValue;
			kLog (K_DATA, "[devAiNI6123_read_ai.AI_RAW_VALUE] %s:%d raw value: %d (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VALUE :
			precord->val = (float)pNI6123->ST_Ch[chId].euValue;
			kLog (K_DATA, "[devAiNI6123_read_ai.AI_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, precord->val);
			if(chId == 4)
			{
				precord->val = precord->val*1000;
				kLog (K_DATA, "[devAiNI6123_read_ai.AI_VALUE.Capacitor discharge current] %s:%d value: %f\n", pSTDdev->taskName, chId, precord->val);
			}
			else if(chId == 5){
				precord->val = ((3000/8)*precord->val)-750; 
				kLog (K_DATA, "[devAiNI6123_read_ai.AI_VALUE.Capacitor charge Voltage] %s:%d value: %f\n", pSTDdev->taskName, chId, precord->val);
			}
			break;

		case AI_POWER :
			precord->val = (float)pNI6123->ST_Ch[chId].power;
			kLog (K_DATA, "[devAiNI6123_read_ai.AI_POWER] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_DENSITY :
			precord->val = (float)pNI6123->ST_Ch[chId].density;
			kLog (K_DATA, "[devAiNI6123_read_ai.AI_DENSITY] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;
		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			break;

		default:
			kLog (K_ERR, "[devAiNI6123_read_ai] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devWaveNI6123_init_record(waveformRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devWaveNI6123_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devWaveNI6123 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devWaveNI6123 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (3 == i) {
		if (!strcmp (pSTdpvt->arg1, WAVE_RAW_VALUE_STR)) {
			pSTdpvt->ind		= WAVE_RAW_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
		else if (!strcmp (pSTdpvt->arg1, WAVE_VALUE_STR)) {
			pSTdpvt->ind		= WAVE_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
	}
	else {
		kLog (K_ERR, "[devWaveNI6123_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}
static long devWaveNI6123_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;

	return 0;
}
static long devWaveNI6123_read_wave(waveformRecord *precord)
{
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NI6123 *pNI6123 = NULL;
	int chId;
	unsigned long sampleRate;  /* wave form record Number of Elements and Number elements read field */

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNI6123 = (ST_NI6123*)pSTDdev->pUser;

	sampleRate = (unsigned long)pNI6123->sample_rate;
	switch (pSTdpvt->ind) {
		case WAVE_RAW_VALUE :
			if(sampleRate > 0) precord->nelm = sampleRate;
			precord->bptr = pNI6123->ST_Ch[chId].fScaleReadChannel;
			if(sampleRate > 0) precord->nord = sampleRate;
			kLog (K_DATA, "[devWaveNI6123_read_wave.WAVE_RAW_VALUE] %s:%d raw value: %f \n",
					pSTDdev->taskName, chId, pNI6123->ST_Ch[chId].fScaleReadChannel[0]);
			break;

		case WAVE_VALUE :
			if(sampleRate > 0) precord->nelm = sampleRate;
			precord->bptr = pNI6123->ST_Ch[chId].calcDensity;
			if(sampleRate > 0) precord->nord = sampleRate;
			kLog (K_DATA, "[devWaveNI6123_read_wave.WAVE_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, pNI6123->ST_Ch[chId].fScaleReadChannel[0]);
			break;

		default:
			kLog (K_ERR, "[devWaveNI6123_read_wave] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}



static long devBoNI6123_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s",
					pSTdpvt->arg0);

			kLog (K_INFO, "[devBoNI6123_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);

			ST_MASTER *pMaster = get_master();
			pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoNI6123Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoNI6123Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}
	if(!strcmp(pSTdpvt->arg0, BO_AUTO_RUN_STR)) {
                pSTdpvt->ind = BO_AUTO_RUN;
        }
	else if (!strcmp(pSTdpvt->arg0, BO_DAQ_STOP_STR)) {
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
		kLog (K_ERR, "ERROR! devBoNI6123_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoNI6123_write_bo(boRecord *precord)
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
                                qData.pFunc = devNI6123_BO_AUTO_RUN;
                                break;
			case BO_DAQ_STOP:
				qData.pFunc = devNI6123_BO_DAQ_STOP;
				break;

			case BO_TREND_RUN:
				qData.pFunc = devNI6123_BO_TREND_RUN;
				break;

			case BO_CALC_RUN:
				qData.pFunc = devNI6123_BO_CALC_RUN;
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

BINARYDSET	devAoNI6123Control = { 6, NULL, NULL, devAoNI6123_init_record, NULL, devAoNI6123_write_ao, NULL };
BINARYDSET	devBoNI6123Control = { 5, NULL, NULL, devBoNI6123_init_record, NULL, devBoNI6123_write_bo };
BINARYDSET	devAiNI6123RawRead = { 6, NULL, NULL, devAiNI6123_init_record, devAiNI6123_get_ioint_info, devAiNI6123_read_ai, NULL };
BINARYDSET  devWaveNI6123Read = { 5, NULL, NULL, devWaveNI6123_init_record, devWaveNI6123_get_ioint_info, devWaveNI6123_read_wave};

epicsExportAddress(dset, devWaveNI6123Read);
epicsExportAddress(dset, devAoNI6123Control);
epicsExportAddress(dset, devBoNI6123Control);
epicsExportAddress(dset, devAiNI6123RawRead);

