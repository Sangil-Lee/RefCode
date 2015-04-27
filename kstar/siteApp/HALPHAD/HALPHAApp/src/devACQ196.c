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

#include "drvACQ196.h" 

static long devWaveACQ196_init_record(waveformRecord *precord);
static long devWaveACQ196_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt);
static long devWaveACQ196_read_wave(waveformRecord *precord);




/* auto Run function */
static void devACQ196_BO_AUTO_RUN(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_ACQ196 *pACQ196 = NULL;

        kLog (K_TRACE, "[devACQ196_BO_AUTO_RUN] auto_run(%f)\n", pParam->setValue);

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
#if 0
		char strBuf[24];
		float stopT1;
	/*	remove this function because GCDS has not LTU, so We donot use the DBproc_get function */
				if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
					DBproc_get (PV_LTU_TIG_T0_STR, strBuf);   //Get T0 from LTU
					DBproc_put (PV_START_TIG_T0_STR, strBuf);   //Set T0 from LTU
					sscanf(strBuf, "%f", &stopT1);
					stopT1 = pACQ196->sample_time - stopT1;
					sprintf(strBuf,"%f",stopT1);
					DBproc_put (PV_STOP_TIG_T1_STR, strBuf);   //Set T0 from LTU
				}
#endif
                while(pSTDdev) {
                        pSTDdev->StatusDev |= TASK_STANDBY;

                        pACQ196 = pSTDdev->pUser;
                      //  pACQ196->auto_run_flag = 1;

                        pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
                }
        }
        else if(!pParam->setValue) {
                while(pSTDdev) {
                        pACQ196 = pSTDdev->pUser;
                      //  pACQ196->auto_run_flag = 0;

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


static void devACQ196_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devACQ196_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

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

static void devACQ196_BO_TREND_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devACQ196_BO_TREND_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
	/*	processTrendRun ();  */
	}
}

static void devACQ196_BO_CALC_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devACQ196_BO_CALC_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
         /*	processCalcRun ();   */
	}
}

static void devACQ196_BO_SELECT_STORE(ST_execParam *pParam)
{
    ST_STD_device *pSTDdev = pParam->pSTDdev;
    ST_ACQ196 *pACQ196 = pSTDdev->pUser;
    int chId;

    chId    = pParam->n32Arg0;
    pACQ196  = (ST_ACQ196*)pSTDdev->pUser;

    kLog (K_TRACE, "[devACQ196_BO_SELECT_STORE] %s : ch(%d) Store Select value(%f)\n",
            pSTDdev->taskName, chId, pParam->setValue);

    pACQ196->ST_Ch[chId].saveToMds = pParam->setValue;

    scanIoRequest(pSTDdev->ioScanPvt_userCall);
}


static void devACQ196_AO_DAQ_MIN_VOLT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_ACQ196 *pACQ196 = NULL;

        kLog (K_DATA,"[devACQ196_AO_DAQ_MIN_VOLT] minVolt(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devACQ196_AO_DAQ_MIN_VOLT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pACQ196 = pSTDdev->pUser;
                        pACQ196->minVal = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devACQ196_AO_DAQ_MIN_VOLT] task(%s) minVolt(%.f)\n",
                                        pSTDdev->taskName, pACQ196->minVal);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devACQ196_AO_DAQ_MAX_VOLT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_ACQ196 *pACQ196 = NULL;

        kLog (K_DATA,"[devACQ196_AO_DAQ_MAX_VOLT] minVolt(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devACQ196_AO_DAQ_MAX_VOLT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pACQ196 = pSTDdev->pUser;
                        pACQ196->maxVal = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devACQ196_AO_DAQ_MAX_VOLT] task(%s) maxVolt(%.f)\n",
                                        pSTDdev->taskName, pACQ196->maxVal);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}

static void devACQ196_AO_DAQ_SAMPLEING_RATE(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_ACQ196 *pACQ196 = NULL;

        kLog (K_DATA,"[devACQ196_AO_DAQ_SAMPLEING_RATE] sampleRate(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devACQ196_AO_DAQ_SAMPLEING_RATE] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pACQ196 = pSTDdev->pUser;
			if(pParam->setValue > pACQ196->sample_rateLimit){
				/*  Device Sampling Rate Limit Cut if User is over Sampling Rate Setting.  */
				pParam->setValue = pACQ196->sample_rateLimit;
			} else if(pParam->setValue <= 0){
				 pParam->setValue = 1;
			}
			pACQ196->sample_rate = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devACQ196_AO_DAQ_SAMPLEING_RATE] task(%s) sampleRate(%.f)\n",
                                        pSTDdev->taskName, pACQ196->sample_rate);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devACQ196_AO_DAQ_KSTAR_TF_CURRENT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_ACQ196 *pACQ196 = NULL;

        kLog (K_DATA,"[devACQ196_AO_DAQ_KSTAR_TF_CURRENT] MPS TF Current (%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devACQ196_AO_DAQ_KSTAR_TF_CURRENT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pACQ196 = pSTDdev->pUser;
			pACQ196->currentTF = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devACQ196_AO_DAQ_KSTAR_TF_CURRENT] task(%s) TF Current(%.f)\n",
                                        pSTDdev->taskName, pACQ196->currentTF);
                }
                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devACQ196_AO_DAQ_SAMPLEING_TIME(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_ACQ196 *pACQ196 = NULL;

        kLog (K_DATA,"[devACQ196_AO_DAQ_SAMPLEING_TIME] sampleTime(%f)\n", pParam->setValue);
#if 0
		char strBuf[24];
		float stopT1;
	/*	remove this function because GCDS has not LTU, so We donot use the DBproc_get function */
		if (!(pSTDdev->StatusDev & TASK_ARM_ENABLED)) {
			/* Change T0 value form LUT when Remote Mode TG */
			if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
			/*	DBproc_get (PV_LTU_TIG_T0_STR, strBuf);   Get T0 from LTU - GCDS Not yet ready LTU*/
				DBproc_put (PV_START_TIG_T0_STR, strBuf);   //Set T0 from LTU 
				sscanf(strBuf, "%f", &stopT1);
				stopT1 = pParam->setValue - stopT1;
				sprintf(strBuf,"%f",stopT1);
				DBproc_put (PV_STOP_TIG_T1_STR, strBuf);   //Set T0 from LTU
			}
		}
#endif
        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devACQ196_AO_DAQ_SAMPLEING_TIME] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pACQ196 = pSTDdev->pUser;
                        pACQ196->sample_time = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devACQ196_AO_DAQ_SAMPLEING_TIME] task(%s) sampleTime(%.f)\n",
                                        pSTDdev->taskName, pACQ196->sample_rate);
                }
                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}




static void devACQ196_AO_INTEGRAL_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_INTEGRAL_TIME] %s : ch(%d) time(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].integral_time = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devACQ196_AO_FLOW(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_FLOW] %s : ch(%d) flow(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].flow = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devACQ196_AO_FRINGE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_FRINGE] %s : ch(%d) density fringe(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].fringe = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devACQ196_AO_LIMITE_UP(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_LIMITE_UP] %s : ch(%d) density limite upper(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].dLimUpper = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devACQ196_AO_LIMITE_LOW(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_LIMITE_LOW] %s : ch(%d) density limite lower(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].dLimLow = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_CONSTANT_A(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_CONSTANT_A] %s : ch(%d) density constant A(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].conA = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_CONSTANT_B(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_CONSTANT_B] %s : ch(%d) density constant B(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].conB = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_CONSTANT_C(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_CONSTANT_C] %s : ch(%d) density constant C(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].conC = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_CONSTANT_D(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_CONSTANT_D] %s : ch(%d) density constant D(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].conD = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devACQ196_AO_MDS_PARAM_VALUE1(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_MDS_PARAM_VALUE1] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].mdsParam1 = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_MDS_PARAM_VALUE2(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_MDS_PARAM_VALUE2] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].mdsParam2 = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_MDS_PARAM_VALUE3(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_MDS_PARAM_VALUE3] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].mdsParam3 = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_MDS_PARAM_VALUE4(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_MDS_PARAM_VALUE4] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].mdsParam4 = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devACQ196_AO_MDS_PARAM_VALUE5(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int	chId;

	chId	= pParam->n32Arg0;
	pACQ196	= (ST_ACQ196*)pSTDdev->pUser;

	kLog (K_TRACE, "[devACQ196_AO_MDS_PARAM_VALUE5] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);

	pACQ196->ST_Ch[chId].mdsParam5 = pParam->setValue;

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static long devAoACQ196_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0, pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoACQ196_init_record] %d: %s %s %s\n",
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
						"devAoACQ196Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoACQ196Control (init_record) Illegal OUT field");
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
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_SAMPLING_RATE_STR)) {
		pSTdpvt->ind            = AO_DAQ_SAMPLING_RATE;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_KSTAR_TF_CURRENT_STR)) {
		pSTdpvt->ind            = AO_DAQ_KSTAR_TF_CURRENT;
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DAQ_SAMPLING_TIME_STR)) {
		pSTdpvt->ind            = AO_DAQ_SAMPLING_TIME;
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
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_CONSTANT_A_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_A;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_CONSTANT_B_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_B;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_CONSTANT_C_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_C;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_DENSITY_CONSTANT_D_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_D;
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
	else if (!strcmp(pSTdpvt->arg0, AO_MDS_PARAM_VALUE1_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE1;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_MDS_PARAM_VALUE2_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE2;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_MDS_PARAM_VALUE3_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE3;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_MDS_PARAM_VALUE4_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE4;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(pSTdpvt->arg0, AO_MDS_PARAM_VALUE5_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE5;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg1, NULL, 0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}

static long devAoACQ196_write_ao(aoRecord *precord)
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

		kLog (K_DEBUG, "[devAoACQ196_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_MIN_VOLT:
				qData.pFunc = devACQ196_AO_DAQ_MIN_VOLT;
				break;
			case AO_DAQ_MAX_VOLT:
				qData.pFunc = devACQ196_AO_DAQ_MAX_VOLT;
				break;
			case AO_DAQ_SAMPLING_RATE:
				qData.pFunc = devACQ196_AO_DAQ_SAMPLEING_RATE;
				break;
			case AO_DAQ_KSTAR_TF_CURRENT:
				qData.pFunc = devACQ196_AO_DAQ_KSTAR_TF_CURRENT;
				break;
			case AO_DAQ_SAMPLING_TIME:
				qData.pFunc = devACQ196_AO_DAQ_SAMPLEING_TIME;
				break;
			case AO_INTEGRAL_TIME :
				qData.pFunc = devACQ196_AO_INTEGRAL_TIME;
				break;
			case AO_FLOW :
				qData.pFunc = devACQ196_AO_FLOW;
				break;
			case AO_DENSITY_FRINGE :
				qData.pFunc = devACQ196_AO_FRINGE;
				break;
			case AO_DENSITY_LIMITE_UP :
				qData.pFunc = devACQ196_AO_LIMITE_UP;
				break;
			case AO_DENSITY_LIMITE_LOW :
				qData.pFunc = devACQ196_AO_LIMITE_LOW;
				break;
			case AO_DENSITY_CONSTANT_A :
				qData.pFunc = devACQ196_AO_CONSTANT_A;
				break;
			case AO_DENSITY_CONSTANT_B :
				qData.pFunc = devACQ196_AO_CONSTANT_B;
				break;
			case AO_DENSITY_CONSTANT_C :
				qData.pFunc = devACQ196_AO_CONSTANT_C;
				break;
			case AO_DENSITY_CONSTANT_D :
				qData.pFunc = devACQ196_AO_CONSTANT_D;
				break;
			case AO_MDS_PARAM_VALUE1 :
				qData.pFunc = devACQ196_AO_MDS_PARAM_VALUE1;
				break;
			case AO_MDS_PARAM_VALUE2 :
				qData.pFunc = devACQ196_AO_MDS_PARAM_VALUE2;
				break;
			case AO_MDS_PARAM_VALUE3 :
				qData.pFunc = devACQ196_AO_MDS_PARAM_VALUE3;
				break;
			case AO_MDS_PARAM_VALUE4 :
				qData.pFunc = devACQ196_AO_MDS_PARAM_VALUE4;
				break;
			case AO_MDS_PARAM_VALUE5 :
				qData.pFunc = devACQ196_AO_MDS_PARAM_VALUE5;
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
		kLog (K_DEBUG, "[devAoACQ196_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiACQ196_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiACQ196_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiACQ196 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiACQ196 (init_record) Illegal INP field");
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
		else if (!strcmp (pSTdpvt->arg1, AI_TE_VALUE_STR)) {
			pSTdpvt->ind		= AI_TE_VALUE;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
		else if (!strcmp (pSTdpvt->arg1, AI_TE_POS2ND_STR)) {
			pSTdpvt->ind		= AI_TE_POS2ND;
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
		kLog (K_ERR, "[devAiACQ196_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiACQ196_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiACQ196_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pACQ196 = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pACQ196 = (ST_ACQ196*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case AI_DAQ_MIN_VOLT_R :
                        precord->val = (float)pACQ196->minVal;
                        kLog (K_DATA,"[devAiACQ196RawRead_read_ai.AI_DAQ_MIN_VOLT] %s:%d minVolt: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_MAX_VOLT_R :
                        precord->val = (float)pACQ196->maxVal;
                        kLog (K_DATA,"[devAiACQ196RawRead_read_ai.AI_DAQ_MAX_VOLT] %s:%d maxVolt: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_SAMPLING_RATE_R :
                        precord->val = (float)pACQ196->sample_rate;
                        kLog (K_DATA,"[devAiACQ196RawRead_read_ai.AI_DAQ_SAMPLING_RATE] %s:%d sampleRate: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_SAMPLING_TIME_R :
                        precord->val = (float)pACQ196->sample_time;
                        kLog (K_DATA,"[devAiACQ196RawRead_read_ai.AI_DAQ_SAMPLING_TIME] %s:%d sampleTime: %f (0x%x)\n",
                                        pSTDdev->taskName, chId, precord->val, precord->val);
                        break;
		case AI_RAW_VALUE :
			precord->val = (float)pACQ196->ST_Ch[chId].rawValue;
			kLog (K_DATA, "[devAiACQ196_read_ai.AI_RAW_VALUE] %s:%d raw value: %d (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VALUE :
			precord->val = (float)pACQ196->ST_Ch[chId].euValue;
			kLog (K_DATA, "[devAiACQ196_read_ai.AI_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_TE_VALUE :
			precord->val = (float)pACQ196->ST_Ch[chId].teValue;
			precord->sval = (float)pACQ196->ST_Ch[chId].posHar2nd;
			kLog (K_DATA, "[devAiACQ196_read_ai.AI_TE_VALUE] %s:%d teValue: %f 2ndHarPosition(%f)\n", pSTDdev->taskName, chId, precord->val, precord->sval);
			break;
		case AI_TE_POS2ND :
			precord->val = (float)pACQ196->ST_Ch[chId].posHar2nd;
			kLog (K_DATA, "[devAiACQ196_read_ai.AI_TE_POS2ND] %s:%d 2ndHarPosition(%f)\n", pSTDdev->taskName, chId, precord->val);
			break;
		case AI_POWER :
			precord->val = (float)pACQ196->ST_Ch[chId].power;
			kLog (K_DATA, "[devAiACQ196_read_ai.AI_POWER] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_DENSITY :
			precord->val = (float)pACQ196->ST_Ch[chId].density;
			kLog (K_DATA, "[devAiACQ196_read_ai.AI_DENSITY] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;
		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			break;

		default:
			kLog (K_ERR, "[devAiACQ196_read_ai] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devWaveACQ196_init_record(waveformRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devWaveACQ196_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devWaveACQ196 (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devWaveACQ196 (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (2 == i) {
		if (!strcmp (pSTdpvt->arg0, WAVE_TE_VALUE_STR)) {
			pSTdpvt->ind		= WAVE_TE_VALUE;
		}
		else if (!strcmp (pSTdpvt->arg0, WAVE_TE_POS_STR)) {
			pSTdpvt->ind		= WAVE_TE_POS;
		}
	} else if (3 == i) {
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
		kLog (K_ERR, "[devWaveACQ196_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}
static long devWaveACQ196_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;
//	*ioScanPvt	= pSTDdev->ioScanPvt_userCall1;

	return 0;
}
static long devWaveACQ196_read_wave(waveformRecord *precord)
{
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pACQ196 = NULL;
	int chId;
	unsigned long chanNum;
	unsigned long sampleRate;  /* wave form record Number of Elements and Number elements read field */
	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pACQ196 = (ST_ACQ196*)pSTDdev->pUser;
	chanNum = ellCount(pSTDdev->pList_Channel);

	sampleRate = (unsigned long)pACQ196->sample_rate;
	switch (pSTdpvt->ind) {
		case WAVE_RAW_VALUE :
			if(sampleRate > 0) precord->nelm = sampleRate;
			precord->bptr = pACQ196->ST_Ch[chId].fScaleReadChannel;
			if(sampleRate > 0) precord->nord = sampleRate;
			kLog (K_DATA, "[devWaveACQ196_read_wave.WAVE_RAW_VALUE] %s:%d raw value: %f \n",
					pSTDdev->taskName, chId, pACQ196->ST_Ch[chId].fScaleReadChannel[0]);
			break;
		case WAVE_TE_VALUE :
			precord->nelm = 76;
			//precord->val = waveVal;
			precord->bptr = pACQ196->rtTe;
			precord->nord = 76;
			kLog (K_DATA, "[devWaveACQ196_read_wave.WAVE_TE_VALUE] %s:%d te value( %f) NORD(%d) NELM(%d) \n",
					pSTDdev->taskName, chId, pACQ196->rtTe[0], chanNum,76);
			break;
		case WAVE_TE_POS :
			precord->nelm = 76;
			//precord->val = waveVal;
			precord->bptr = pACQ196->rtTePos;
			precord->nord = 76;
			kLog (K_DATA, "[devWaveACQ196_read_wave.WAVE_TE_POS] %s:%d te pos( %f) NORD(%d) NELM(%d)\n",
					pSTDdev->taskName, chId, pACQ196->rtTePos[0], chanNum,76);
			break;

		case WAVE_VALUE :
			if(sampleRate > 0) precord->nelm = sampleRate;
			precord->bptr = pACQ196->ST_Ch[chId].calcDensity;
			if(sampleRate > 0) precord->nord = sampleRate;
			kLog (K_DATA, "[devWaveACQ196_read_wave.WAVE_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, pACQ196->ST_Ch[chId].fScaleReadChannel[0]);
			break;

		default:
			kLog (K_ERR, "[devWaveACQ196_read_wave] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}



static long devBoACQ196_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
#if 0
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);

			i = sscanf(precord->out.value.instio.string, "%s",
					pSTdpvt->arg0);

			kLog (K_INFO, "[devBoACQ196_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);

			ST_MASTER *pMaster = get_master();
			pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoACQ196Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoACQ196Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}
#endif
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
					pSTdpvt->arg0,  pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devBoACQ196_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);
			if (1 == i) {
				ST_MASTER *pMaster = get_master();
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			}
			else if (3 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
			}

			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devBoACQ196Control (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoACQ196Control (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}
	if(1 == i){
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
	} else if (3 == i){
		if(!strcmp(pSTdpvt->arg1, BO_STORE_TO_MDS_STR)) {
			pSTdpvt->ind        = BO_STORE_TO_MDS;
			pSTdpvt->n32Arg0    = strtoul(pSTdpvt->arg0, NULL, 0);
		}
	} else {
		pSTdpvt->ind = -1;
		kLog (K_ERR, "ERROR! devBoACQ196_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoACQ196_write_bo(boRecord *precord)
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
	qData.param.n32Arg0 	= pSTdpvt->n32Arg0;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_INFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case BO_AUTO_RUN:
                                qData.pFunc = devACQ196_BO_AUTO_RUN;
                                break;
			case BO_DAQ_STOP:
				qData.pFunc = devACQ196_BO_DAQ_STOP;
				break;

			case BO_TREND_RUN:
				qData.pFunc = devACQ196_BO_TREND_RUN;
				break;

			case BO_CALC_RUN:
				qData.pFunc = devACQ196_BO_CALC_RUN;
				break;
			case BO_STORE_TO_MDS:
				qData.pFunc = devACQ196_BO_SELECT_STORE;
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

BINARYDSET	devAoACQ196Control = { 6, NULL, NULL, devAoACQ196_init_record, NULL, devAoACQ196_write_ao, NULL };
BINARYDSET	devBoACQ196Control = { 5, NULL, NULL, devBoACQ196_init_record, NULL, devBoACQ196_write_bo };
BINARYDSET	devAiACQ196RawRead = { 6, NULL, NULL, devAiACQ196_init_record, devAiACQ196_get_ioint_info, devAiACQ196_read_ai, NULL };
BINARYDSET  devWaveACQ196Read = { 5, NULL, NULL, devWaveACQ196_init_record, devWaveACQ196_get_ioint_info, devWaveACQ196_read_wave};

epicsExportAddress(dset, devWaveACQ196Read);
epicsExportAddress(dset, devAoACQ196Control);
epicsExportAddress(dset, devBoACQ196Control);
epicsExportAddress(dset, devAiACQ196RawRead);

