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

#include "drvNIDAQmx.h" 

static long devWaveNIDAQmx_init_record(waveformRecord *precord);
static long devWaveNIDAQmx_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt);
static long devWaveNIDAQmx_read_wave(waveformRecord *precord);



/* auto Run function */
static void devNIDAQmx_BO_AUTO_RUN(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NIDAQmx *pNIDAQmx = NULL;

        kLog (K_TRACE, "[devNIDAQmx_BO_AUTO_RUN] auto_run(%f)\n", pParam->setValue);

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
	/*	remove this function because ECH has not LTU, so We donot use the DBproc_get function */
				if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
					DBproc_get (PV_LTU_TIG_T0_STR, strBuf);   //Get T0 from LTU
					DBproc_put (PV_START_TIG_T0_STR, strBuf);   //Set T0 from LTU
					sscanf(strBuf, "%f", &stopT1);
					stopT1 = pNIDAQmx->sample_time - stopT1;
					sprintf(strBuf,"%f",stopT1);
					DBproc_put (PV_STOP_TIG_T1_STR, strBuf);   //Set T0 from LTU
				}
#endif
                while(pSTDdev) {
                        pSTDdev->StatusDev |= TASK_STANDBY;

                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->auto_run_flag = 1;

                        pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
                }
        }
        else if(!pParam->setValue) {
                while(pSTDdev) {
                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->auto_run_flag = 0;

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


static void devNIDAQmx_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devNIDAQmx_BO_DAQ_STOP] daq_stop(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	resetDeviceAll ();

	notify_refresh_master_status();

	scanIoRequest(get_master()->ioScanPvt_status);

	DBproc_put(PV_SYS_RUN_STR, "0");
   	DBproc_put(PV_SYS_ARMING_STR, "0");
	DBproc_put(PV_DAQ_STOP_STR, "0");
}

static void devNIDAQmx_BO_TREND_RUN(ST_execParam *pParam)
{

        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NIDAQmx *pNIDAQmx = NULL;

	kLog (K_MON, "[devNIDAQmx_BO_TREND_RUN] setValue(%f)\n", pParam->setValue);

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

                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->trend_run_flag = 1;

                        pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
                }
        }
        else if(!pParam->setValue) {
                while(pSTDdev) {
                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->trend_run_flag = 0;

                        pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
                }

                epicsThreadSleep(3.0);
                scanIoRequest(pMaster->ioScanPvt_status);
        }
}

static void devNIDAQmx_BO_CALC_RUN(ST_execParam *pParam)
{
	kLog (K_MON, "[devNIDAQmx_BO_CALC_RUN] setValue(%f)\n", pParam->setValue);

	if (pParam->setValue) {
         /*	processCalcRun ();   */
	}
}

static void devNIDAQmx_BO_MAKE_PATTERN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;

	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;
	if (pParam->setValue == 1){
		kLog (K_TRACE, "[devNIDAQmx_BO_MAKE_PATTERN] %s : Analog Output mode make pattern (%f)\n", pSTDdev->taskName,  pParam->setValue);
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNIDAQmx_BO_MAKE_PATTERN] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
			make_pattern_dataToFile(pSTDdev);
		} else kLog (K_TRACE, "[devNIDAQmx_BO_MAKE_PATTERN] Error %s : Only use analog output module make parttern (%f)\n", pSTDdev->taskName, pParam->setValue);

		scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
	}
}
static void devNIDAQmx_BO_STORE_PATTERN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;

	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	if (pParam->setValue == 1){
		kLog (K_TRACE, "[devNIDAQmx_BO_STORE_PATTERN] %s : Analog Output mode make pattern data storing  (%f)\n", pSTDdev->taskName,  pParam->setValue);
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNIDAQmx_BO_STORE_PATTERN] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
			proc_sendAOData2Tree(pSTDdev);
		} else kLog (K_TRACE, "[devNIDAQmx_BO_STORE_PATTERN] Error %s : Only use analog output module make parttern data storeing (%f)\n", pSTDdev->taskName, pParam->setValue);

		scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
	}
}
static void devNIDAQmx_BO_DOING_PROGRESS(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;

	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;
	if (pParam->setValue == 1){
		kLog (K_TRACE, "[devNIDAQmx_BO_DOING_PROGRESS] %s : Analog Output mode => change the status to progress (%f)\n", pSTDdev->taskName,  pParam->setValue);
		if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
			kLog (K_DATA,"[devNIDAQmx_BO_DOING_PROGRESS] %s: System is armed! \n", pSTDdev->taskName);
			notify_error (1, "System is armed!", pSTDdev->taskName );
		}
		else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
			if(pParam->setValue == 1){
				AO_Make_Status_Progress(pSTDdev);
			}
		} else kLog (K_TRACE, "[devNIDAQmx_BO_DOING_PROGRESS] Error %s : Only use analog output module (%f)\n", pSTDdev->taskName, pParam->setValue);

		scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
	}
}

static void devNIDAQmx_BO_SELECT_AO_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;

	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_BO_SELECT_AO_MODE] %s : Analog Output mode select operate mode (%f)\n", pSTDdev->taskName, pParam->setValue);
	if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
		kLog (K_DATA,"[devNIDAQmx_BO_SELECT_AO_MODE] %s: System is armed! \n", pSTDdev->taskName);
		notify_error (1, "System is armed!", pSTDdev->taskName );
	}
	else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
		pNIDAQmx->smp_mode_select = pParam->setValue;
		scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
	} else kLog (K_TRACE, "[devNIDAQmx_BO_SELECT_AO_MODE] Error %s : Only use analog output module select operate mode (%f)\n", pSTDdev->taskName, pParam->setValue);

}
static void devNIDAQmx_BO_SELECT_STORE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_BO_SELECT_STORE] %s : ch(%d) Store Select value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
		if(0 == strcmp(pNIDAQmx->sigType, "ai")){
			pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->saveToMds = pParam->setValue;
		} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
			pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
			pNIDAQmxAOCh->saveToMds = pParam->setValue;
		}
	}

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}


static void devNIDAQmx_AO_DAQ_MIN_VOLT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NIDAQmx *pNIDAQmx = NULL;

        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_MIN_VOLT] minVolt(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_MIN_VOLT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->minVal = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_MIN_VOLT] task(%s) minVolt(%.f)\n",
                                        pSTDdev->taskName, pNIDAQmx->minVal);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devNIDAQmx_AO_DAQ_MAX_VOLT(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NIDAQmx *pNIDAQmx = NULL;

        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_MAX_VOLT] minVolt(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_MAX_VOLT] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->maxVal = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_MAX_VOLT] task(%s) maxVolt(%.f)\n",
                                        pSTDdev->taskName, pNIDAQmx->maxVal);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}

static void devNIDAQmx_AO_DAQ_SAMPLEING_RATE(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NIDAQmx *pNIDAQmx = NULL;

        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_SAMPLEING_RATE] sampleRate(%f)\n", pParam->setValue);

        while(pSTDdev) {
                if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_SAMPLEING_RATE] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
			if(pParam->setValue > pNIDAQmx->sample_rateLimit){
				/*  Device Sampling Rate Limit Cut if User is over Sampling Rate Setting.  */
				pParam->setValue = pNIDAQmx->sample_rateLimit;
			} else if(pParam->setValue <= 0){
				 pParam->setValue = 1;
			}
			pNIDAQmx->sample_rate = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_SAMPLEING_RATE] task(%s) sampleRate(%.f)\n",
                                        pSTDdev->taskName, pNIDAQmx->sample_rate);
                }

                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}
static void devNIDAQmx_AO_DAQ_SAMPLEING_TIME(ST_execParam *pParam)
{
        ST_MASTER *pMaster = get_master();
        ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
        ST_NIDAQmx *pNIDAQmx = NULL;

        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_SAMPLEING_TIME] sampleTime(%f)\n", pParam->setValue);
#if 0
		char strBuf[24];
		float stopT1;
	/*	remove this function because ECH has not LTU, so We donot use the DBproc_get function */
		if (!(pSTDdev->StatusDev & TASK_ARM_ENABLED)) {
			/* Change T0 value form LUT when Remote Mode TG */
			if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
			/*	DBproc_get (PV_LTU_TIG_T0_STR, strBuf);   Get T0 from LTU - ECH Not yet ready LTU*/
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
                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_SAMPLEING_TIME] %s: System is armed! \n", pSTDdev->taskName);
                        notify_error (1, "System is armed!", pSTDdev->taskName );
                }
                else {
                        pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
                        pNIDAQmx->sample_time = pParam->setValue;
                        scanIoRequest(pSTDdev->ioScanPvt_userCall);

                        kLog (K_DATA,"[devNIDAQmx_AO_DAQ_SAMPLEING_TIME] task(%s) sampleTime(%.f)\n",
                                        pSTDdev->taskName, pNIDAQmx->sample_rate);
                }
                pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
        }
}


static void devNIDAQmx_AO_INTEGRAL_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = NULL;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
        ST_STD_channel *pSTDCh = NULL;

	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_INTEGRAL_TIME] %s : ch(%d) time(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->integral_time = pParam->setValue;
                } else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
                        pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
			pNIDAQmxAOCh->integral_time = pParam->setValue;
                }
        }

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNIDAQmx_AO_FLOW(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
        ST_STD_channel *pSTDCh = NULL;

	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_FLOW] %s : ch(%d) flow(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->flow = pParam->setValue;
                } else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
                        pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
			pNIDAQmxAOCh->flow = pParam->setValue;
                }
        }

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNIDAQmx_AO_FRINGE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_FRINGE] %s : ch(%d) density fringe(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	 if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->fringe = pParam->setValue;
                } 
	}

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNIDAQmx_AO_LIMITE_UP(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;

	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_LIMITE_UP] %s : ch(%d) density limite upper(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->dLimUpper = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNIDAQmx_AO_LIMITE_LOW(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_LIMITE_LOW] %s : ch(%d) density limite lower(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->dLimLow = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_CONSTANT_A(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;

	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_CONSTANT_A] %s : ch(%d) density constant A(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->conA = pParam->setValue;
                }
        }	
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_CONSTANT_B(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_CONSTANT_B] %s : ch(%d) density constant B(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->conB = pParam->setValue;
                }
        }	
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_CONSTANT_C(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_CONSTANT_C] %s : ch(%d) density constant C(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
                        pNIDAQmxAICh->conC = pParam->setValue;
                }
        }

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_CONSTANT_D(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_CONSTANT_D] %s : ch(%d) density constant D(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
                        pNIDAQmxAICh->conD = pParam->setValue;
                }
        }

	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNIDAQmx_AO_MDS_PARAM_VALUE1(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_MDS_PARAM_VALUE1] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			pNIDAQmxAICh->mdsParam1 = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_MDS_PARAM_VALUE2(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_MDS_PARAM_VALUE2] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
                        pNIDAQmxAICh->mdsParam2 = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_MDS_PARAM_VALUE3(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_MDS_PARAM_VALUE3] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
                        pNIDAQmxAICh->mdsParam3 = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_MDS_PARAM_VALUE4(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_MDS_PARAM_VALUE4] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
                        pNIDAQmxAICh->mdsParam4 = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}
static void devNIDAQmx_AO_MDS_PARAM_VALUE5(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_MDS_PARAM_VALUE5] %s : ch(%d) MDSplus Param value(%f)\n",
			pSTDdev->taskName, chId, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ai")){
                        pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
                        pNIDAQmxAICh->mdsParam5 = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
}

static void devNIDAQmx_AO_DAQ_VALUE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
      	ST_STD_channel *pSTDCh = NULL;
	int	chId;

	chId	= pParam->n32Arg0;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	if(0 == strcmp(pNIDAQmx->sigType, "ao")){
		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev, chId))){
			pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
			if(pParam->setValue > 9.03){
				 kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_VALUE] %s Over 9.03 vlot : ch(%d) output value(%f)\n", pSTDdev->taskName, chId, pParam->setValue);
				pNIDAQmxAOCh->voltOut = 0.0;
				drvNIDAQmx_AO_DAQ_VALUE(pSTDdev);
			} else if (pParam->setValue < 0){
				 kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_VALUE] %s Low 0 vlot : ch(%d) output value(%f)\n", pSTDdev->taskName, chId, pParam->setValue);
				pNIDAQmxAOCh->voltOut = 0.0;
				drvNIDAQmx_AO_DAQ_VALUE(pSTDdev);
			} else {
				pNIDAQmxAOCh->voltOut = pParam->setValue;
				drvNIDAQmx_AO_DAQ_VALUE(pSTDdev);
			}
		} else kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_VALUE] Error %s :Can not recived pSTDCh ch(%d) output value type (%f)\n",pSTDdev->taskName, chId, pParam->setValue);
	} else  kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_VALUE] Error %s : ch(%d) output value type (%f)\n",pSTDdev->taskName, chId, pParam->setValue);
	kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_VALUE] %s : ch(%d) output value(%f)\n", pSTDdev->taskName, chId, pParam->setValue);

	scanIoRequest(pSTDdev->ioScanPvt_userCall2); 			
}
static void devNIDAQmx_AO_DAQ_PATTERN_VALUE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;
	int	arrayPos;

	chId	= pParam->n32Arg0;
	arrayPos= pParam->n32Arg1;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_PATTERN_VALUE] %s : ch(%d) pattern value pos(%d) : (%f)\n",
			pSTDdev->taskName, chId, arrayPos, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
		if(0 == strcmp(pNIDAQmx->sigType, "ao")){
			pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
			if(pParam->setValue > 9.03){
				kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_PATTERN_VALUE] %s Over 9.03 vlot : ch(%d) output value(%f)\n", pSTDdev->taskName, chId, pParam->setValue);
				pNIDAQmxAOCh->wavePatternValue[arrayPos] = 0.0;
			} else if (pParam->setValue < 0){
				kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_PATTERN_VALUE] %s Low 0 vlot : ch(%d) output value(%f)\n", pSTDdev->taskName, chId, pParam->setValue);
				pNIDAQmxAOCh->wavePatternValue[arrayPos] = 0.0;
			} else {
				pNIDAQmxAOCh->wavePatternValue[arrayPos] = pParam->setValue;
			}
		}
	}
	scanIoRequest(pSTDdev->ioScanPvt_userCall2); 			
}
static void devNIDAQmx_AO_DAQ_PATTERN_POWER(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;
	int	arrayPos;

	chId	= pParam->n32Arg0;
	arrayPos= pParam->n32Arg1;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_PATTERN_POWER] %s : ch(%d) pattern power pos(%d) : (%f)\n",
			pSTDdev->taskName, chId, arrayPos, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ao")){
                        pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
                        pNIDAQmxAOCh->wavePatternPower[arrayPos] = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall2); 			
}
static void devNIDAQmx_AO_DAQ_PATTERN_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int	chId;
	int	arrayPos;

	chId	= pParam->n32Arg0;
	arrayPos= pParam->n32Arg1;
	pNIDAQmx	= (ST_NIDAQmx*)pSTDdev->pUser;

	kLog (K_TRACE, "[devNIDAQmx_AO_DAQ_PATTERN_TIME] %s : ch(%d) pattern time pos(%d) : (%f)\n",
			pSTDdev->taskName, chId, arrayPos, pParam->setValue);
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                if(0 == strcmp(pNIDAQmx->sigType, "ao")){
                        pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
                        pNIDAQmxAOCh->wavePatternTime[arrayPos] = pParam->setValue;
                }
        }
	scanIoRequest(pSTDdev->ioScanPvt_userCall2); 			
}
static long devAoNIDAQmx_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			// string, task, channel, Ch-arrayPosition
			i = sscanf(precord->out.value.instio.string, "%s %s %s %s",
					arg0, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoNIDAQmx_init_record] %d: %s %s %s %s\n",
					i, pSTdpvt->devName, arg0, pSTdpvt->arg0, pSTdpvt->arg1);

			if (1 == i) {
				ST_MASTER *pMaster = get_master();
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			}
			else if (3 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
			}
			else if (4 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
			}

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devAoNIDAQmxControl (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoNIDAQmxControl (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if (!strcmp(arg0, AO_DAQ_MIN_VOLT_STR)) {
		pSTdpvt->ind            = AO_DAQ_MIN_VOLT;
	}
	else if (!strcmp(arg0, AO_DAQ_MAX_VOLT_STR)) {
		pSTdpvt->ind            = AO_DAQ_MAX_VOLT;
	}
	else if (!strcmp(arg0, AO_DAQ_SAMPLING_RATE_STR)) {
		pSTdpvt->ind            = AO_DAQ_SAMPLING_RATE;
	}
	else if (!strcmp(arg0, AO_DAQ_SAMPLING_TIME_STR)) {
		pSTdpvt->ind            = AO_DAQ_SAMPLING_TIME;
	}
	else if (!strcmp(arg0, AO_DENSITY_FRINGE_STR)) {
		pSTdpvt->ind			= AO_DENSITY_FRINGE;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DENSITY_LIMITE_UP_STR)) {
		pSTdpvt->ind			= AO_DENSITY_LIMITE_UP;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DENSITY_LIMITE_LOW_STR)) {
		pSTdpvt->ind			= AO_DENSITY_LIMITE_LOW;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DENSITY_CONSTANT_A_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_A;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DENSITY_CONSTANT_B_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_B;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DENSITY_CONSTANT_C_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_C;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DENSITY_CONSTANT_D_STR)) {
		pSTdpvt->ind			= AO_DENSITY_CONSTANT_D;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_INTEGRAL_TIME_STR)) {
		pSTdpvt->ind			= AO_INTEGRAL_TIME;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_FLOW_STR)) {
		pSTdpvt->ind			= AO_FLOW;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_MDS_PARAM_VALUE1_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE1;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_MDS_PARAM_VALUE2_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE2;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_MDS_PARAM_VALUE3_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE3;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_MDS_PARAM_VALUE4_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE4;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_MDS_PARAM_VALUE5_STR)) {
		pSTdpvt->ind			= AO_MDS_PARAM_VALUE5;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DAQ_VALUE_STR)) {
		pSTdpvt->ind			= AO_DAQ_VALUE;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DAQ_PATTERN_VALUE_STR)) {
		pSTdpvt->ind			= AO_DAQ_PATTERN_VALUE;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
		pSTdpvt->n32Arg1		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DAQ_PATTERN_POWER_STR)) {
		pSTdpvt->ind			= AO_DAQ_PATTERN_POWER;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
		pSTdpvt->n32Arg1		= strtoul(pSTdpvt->arg1, NULL, 0);
	}
	else if (!strcmp(arg0, AO_DAQ_PATTERN_TIME_STR)) {
		pSTdpvt->ind			= AO_DAQ_PATTERN_TIME;
		pSTdpvt->n32Arg0		= strtoul(pSTdpvt->arg0, NULL, 0);
		pSTdpvt->n32Arg1		= strtoul(pSTdpvt->arg1, NULL, 0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}
static long devAoNIDAQmx_get_ioint_info(int cmd, aoRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall2;

	return 0;
}

static long devAoNIDAQmx_write_ao(aoRecord *precord)
{
	ST_dpvt        *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData         qData;

	if (!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;

	qData.pFunc				= NULL;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;
	qData.param.n32Arg0		= pSTdpvt->n32Arg0;
	qData.param.n32Arg1		= pSTdpvt->n32Arg1;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_DEBUG, "[devAoNIDAQmx_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_MIN_VOLT:
				qData.pFunc = devNIDAQmx_AO_DAQ_MIN_VOLT;
				break;
			case AO_DAQ_MAX_VOLT:
				qData.pFunc = devNIDAQmx_AO_DAQ_MAX_VOLT;
				break;
			case AO_DAQ_SAMPLING_RATE:
				qData.pFunc = devNIDAQmx_AO_DAQ_SAMPLEING_RATE;
				break;
			case AO_DAQ_SAMPLING_TIME:
				qData.pFunc = devNIDAQmx_AO_DAQ_SAMPLEING_TIME;
				break;
			case AO_INTEGRAL_TIME :
				qData.pFunc = devNIDAQmx_AO_INTEGRAL_TIME;
				break;
			case AO_FLOW :
				qData.pFunc = devNIDAQmx_AO_FLOW;
				break;
			case AO_DENSITY_FRINGE :
				qData.pFunc = devNIDAQmx_AO_FRINGE;
				break;
			case AO_DENSITY_LIMITE_UP :
				qData.pFunc = devNIDAQmx_AO_LIMITE_UP;
				break;
			case AO_DENSITY_LIMITE_LOW :
				qData.pFunc = devNIDAQmx_AO_LIMITE_LOW;
				break;
			case AO_DENSITY_CONSTANT_A :
				qData.pFunc = devNIDAQmx_AO_CONSTANT_A;
				break;
			case AO_DENSITY_CONSTANT_B :
				qData.pFunc = devNIDAQmx_AO_CONSTANT_B;
				break;
			case AO_DENSITY_CONSTANT_C :
				qData.pFunc = devNIDAQmx_AO_CONSTANT_C;
				break;
			case AO_DENSITY_CONSTANT_D :
				qData.pFunc = devNIDAQmx_AO_CONSTANT_D;
				break;
			case AO_MDS_PARAM_VALUE1 :
				qData.pFunc = devNIDAQmx_AO_MDS_PARAM_VALUE1;
				break;
			case AO_MDS_PARAM_VALUE2 :
				qData.pFunc = devNIDAQmx_AO_MDS_PARAM_VALUE2;
				break;
			case AO_MDS_PARAM_VALUE3 :
				qData.pFunc = devNIDAQmx_AO_MDS_PARAM_VALUE3;
				break;
			case AO_MDS_PARAM_VALUE4 :
				qData.pFunc = devNIDAQmx_AO_MDS_PARAM_VALUE4;
				break;
			case AO_MDS_PARAM_VALUE5 :
				qData.pFunc = devNIDAQmx_AO_MDS_PARAM_VALUE5;
				break;
			case AO_DAQ_VALUE :
				// To protect over range voltage in anlog output On ECH system
				if(precord->val < 0){
					qData.param.setValue = 0.0;
				} else if(precord->val > 9.03){
					qData.param.setValue = 0.0;
				}
				qData.pFunc = devNIDAQmx_AO_DAQ_VALUE;
				break;
			case AO_DAQ_PATTERN_VALUE :
				// To protect over range voltage in anlog output On ECH system
				if(precord->val < 0){
					qData.param.setValue = 0.0;
				} else if(precord->val > 9.03){
					qData.param.setValue = 0.0;
				}
				qData.pFunc = devNIDAQmx_AO_DAQ_PATTERN_VALUE;
				break;
			case AO_DAQ_PATTERN_POWER :
				if(precord->val < 0){
					qData.param.setValue = 0.0;
				}
				qData.pFunc = devNIDAQmx_AO_DAQ_PATTERN_POWER;
				break;
			case AO_DAQ_PATTERN_TIME :
				if(precord->val < 0){
					qData.param.setValue = 0.0;
				}
				qData.pFunc = devNIDAQmx_AO_DAQ_PATTERN_TIME;
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
		kLog (K_DEBUG, "[devAoNIDAQmx_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case AO_DAQ_VALUE :
				if(precord->val < 0){
					precord->val = 0.0;
				} else if(precord->val > 9.03){
					precord->val = 0.0;
				}
				break;
			case AO_DAQ_PATTERN_VALUE :
				// To protect over range voltage in anlog output On ECH system
				if(precord->val < 0){
					precord->val = 0.0;
				} else if(precord->val > 9.03){
					precord->val = 0.0;
				}
				break;
			case AO_DAQ_PATTERN_POWER :
				if(precord->val < 0){
					precord->val = 0.0;
				}
				break;
			case AO_DAQ_PATTERN_TIME :
				if(precord->val < 0){
					precord->val = 0.0;
				}
				break;
			default:
				break;
		}
		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiNIDAQmx_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiNIDAQmx_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiNIDAQmx (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiNIDAQmx (init_record) Illegal INP field");
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
		kLog (K_ERR, "[devAiNIDAQmx_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiNIDAQmx_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiNIDAQmx_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NIDAQmx *pNIDAQmx = NULL;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int chId;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNIDAQmx = (ST_NIDAQmx*)pSTDdev->pUser;

	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
        }
	

	switch (pSTdpvt->ind) {
		case AI_DAQ_MIN_VOLT_R :
                        precord->val = (float)pNIDAQmx->minVal;
                        kLog (K_DATA,"[devAiNIDAQmxRawRead_read_ai.AI_DAQ_MIN_VOLT] %s:%d minVolt: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_MAX_VOLT_R :
                        precord->val = (float)pNIDAQmx->maxVal;
                        kLog (K_DATA,"[devAiNIDAQmxRawRead_read_ai.AI_DAQ_MAX_VOLT] %s:%d maxVolt: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_SAMPLING_RATE_R :
                        precord->val = (float)pNIDAQmx->sample_rate;
                        kLog (K_DATA,"[devAiNIDAQmxRawRead_read_ai.AI_DAQ_SAMPLING_RATE] %s:%d sampleRate: %d (0x%x)\n",
                                        pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
                        break;
                case AI_DAQ_SAMPLING_TIME_R :
                        precord->val = (float)pNIDAQmx->sample_time;
                        kLog (K_DATA,"[devAiNIDAQmxRawRead_read_ai.AI_DAQ_SAMPLING_TIME] %s:%d sampleTime: %f (0x%x)\n",
                                        pSTDdev->taskName, chId, precord->val, precord->val);
                        break;
		case AI_RAW_VALUE :
			precord->val = (float)pNIDAQmxAICh->rawValue;
			kLog (K_DATA, "[devAiNIDAQmx_read_ai.AI_RAW_VALUE] %s:%d raw value: %d (0x%x)\n",
					pSTDdev->taskName, chId, (int)precord->val, (int)precord->val);
			break;

		case AI_VALUE :
			precord->val = (float)pNIDAQmxAICh->euValue;
			kLog (K_DATA, "[devAiNIDAQmx_read_ai.AI_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_POWER :
			precord->val = (float)pNIDAQmxAICh->power;
			kLog (K_DATA, "[devAiNIDAQmx_read_ai.AI_POWER] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;

		case AI_DENSITY :
			precord->val = (float)pNIDAQmxAICh->density;
			kLog (K_DATA, "[devAiNIDAQmx_read_ai.AI_DENSITY] %s:%d power: %f\n", pSTDdev->taskName, chId, precord->val);
			break;
		case AI_READ_STATE :
			precord->val = pSTDdev->StatusDev;
			break;

		default:
			kLog (K_ERR, "[devAiNIDAQmx_read_ai] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devWaveNIDAQmx_init_record(waveformRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
					pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devWaveNIDAQmx_init_record] %d: %s %s %s\n",
					i, pSTdpvt->devName, pSTdpvt->arg0, pSTdpvt->arg1);

			pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devWaveNIDAQmx (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devWaveNIDAQmx (init_record) Illegal INP field");
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
		kLog (K_ERR, "[devWaveNIDAQmx_init_record] arg cnt (%d) \n",  i );
		return -1;	/*returns: (-1,0)=>(failure,success)*/
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}
static long devWaveNIDAQmx_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall1;

	return 0;
}
static long devWaveNIDAQmx_read_wave(waveformRecord *precord)
{
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_NIDAQmx *pNIDAQmx = NULL;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
        ST_STD_channel *pSTDCh = NULL;
	int chId;
	unsigned long sampleRate;  /* wave form record Number of Elements and Number elements read field */

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	chId	= pSTdpvt->n32Arg0;

	pNIDAQmx = (ST_NIDAQmx*)pSTDdev->pUser;
	if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chId))) {
                pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
        }
	sampleRate = (unsigned long)pNIDAQmx->sample_rate;
	switch (pSTdpvt->ind) {
		case WAVE_RAW_VALUE :
			if(sampleRate > 0) precord->nelm = sampleRate;
			// remove fScaleReadChannel precord->bptr = pNIDAQmxAICh->fScaleReadChannel;
			if(sampleRate > 0) precord->nord = sampleRate;
			// remove fScaleReadChannel kLog (K_DATA, "[devWaveNIDAQmx_read_wave.WAVE_RAW_VALUE] %s:%d raw value: %f \n",pSTDdev->taskName, chId, pNIDAQmxAICh->fScaleReadChannel[0]);
			break;

		case WAVE_VALUE :
			if(sampleRate > 0) precord->nelm = sampleRate;
			//remove density precord->bptr = pNIDAQmxAICh->calcDensity;
			if(sampleRate > 0) precord->nord = sampleRate;
			// reove fScaleReadChannel		kLog (K_DATA, "[devWaveNIDAQmx_read_wave.WAVE_VALUE] %s:%d value: %f\n", pSTDdev->taskName, chId, pNIDAQmxAICh->fScaleReadChannel[0]);
			break;

		default:
			kLog (K_ERR, "[devWaveNIDAQmx_read_wave] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}



static long devBoNIDAQmx_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
/* modify 20120802 TG
			i = sscanf(precord->out.value.instio.string, "%s",
					pSTdpvt->arg0);
*/
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				        pSTdpvt->arg0,	pSTdpvt->devName, pSTdpvt->arg1);

			kLog (K_INFO, "[devBoNIDAQmx_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);
			if (1 == i) {
				ST_MASTER *pMaster = get_master();
				pSTdpvt->pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
			}
			else if (2 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
			}
			else if (3 == i) {
				pSTdpvt->pSTDdev = (ST_STD_device *)get_STDev_from_name(pSTdpvt->devName);
			}

			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoNIDAQmxControl (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoNIDAQmxControl (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}
	if (1 == i) {
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
	} else if (2 == i) {
		if(!strcmp(pSTdpvt->arg0, BO_SELECT_AO_MODE_STR)) {
			pSTdpvt->ind 	= BO_SELECT_AO_MODE;
		}
		else if (!strcmp(pSTdpvt->arg0, BO_MAKE_PATTERN_STR)) {
			pSTdpvt->ind 	= BO_MAKE_PATTERN;
		}
		else if (!strcmp(pSTdpvt->arg0, BO_STORE_PATTERN_STR)) {
			pSTdpvt->ind 	= BO_STORE_PATTERN;
		}
		else if (!strcmp(pSTdpvt->arg0, BO_DOING_PROGRESS_STR)) {
			pSTdpvt->ind 	= BO_DOING_PROGRESS;
		}
	} else if (3 == i) {
		if(!strcmp(pSTdpvt->arg1, BO_STORE_TO_MDS_STR)) {
			pSTdpvt->ind 		= BO_STORE_TO_MDS;
			pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg0, NULL, 0);
		}
	} else {
		pSTdpvt->ind = -1;
		kLog (K_ERR, "ERROR! devBoNIDAQmx_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoNIDAQmx_write_bo(boRecord *precord)
{
	ST_dpvt        *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData		   qData;

	if(!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev 		= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev 	= pSTDdev;
	qData.param.precord 	= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;
	qData.param.n32Arg0	= pSTdpvt->n32Arg0;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_INFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case BO_AUTO_RUN:
                                qData.pFunc = devNIDAQmx_BO_AUTO_RUN;
                                break;
			case BO_DAQ_STOP:
				qData.pFunc = devNIDAQmx_BO_DAQ_STOP;
				break;

			case BO_TREND_RUN:
				qData.pFunc = devNIDAQmx_BO_TREND_RUN;
				break;

			case BO_CALC_RUN:
				qData.pFunc = devNIDAQmx_BO_CALC_RUN;
				break;
			case BO_SELECT_AO_MODE:
				qData.pFunc = devNIDAQmx_BO_SELECT_AO_MODE;
				break;
			case BO_MAKE_PATTERN:
				qData.pFunc = devNIDAQmx_BO_MAKE_PATTERN;
				break;
			case BO_STORE_PATTERN:
				qData.pFunc = devNIDAQmx_BO_STORE_PATTERN;
				break;
			case BO_DOING_PROGRESS:
				qData.pFunc = devNIDAQmx_BO_DOING_PROGRESS;
				break;

			case BO_STORE_TO_MDS:
				qData.pFunc = devNIDAQmx_BO_SELECT_STORE;
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

// TG Move		precord->pact = FALSE;
// TG Move		precord->udf = FALSE;

		switch(pSTdpvt->ind) {
			case BO_MAKE_PATTERN:
				precord->rval = 0;
				break;
			case BO_STORE_PATTERN:
				precord->rval = 0;
				break;
			case BO_DOING_PROGRESS:
				precord->rval = 0;
				break;
			default:
				break;
		}
		precord->pact = FALSE;
		precord->udf = FALSE;

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

BINARYDSET	devAoNIDAQmxControl = { 6, NULL, NULL, devAoNIDAQmx_init_record, devAoNIDAQmx_get_ioint_info, devAoNIDAQmx_write_ao, NULL };
BINARYDSET	devBoNIDAQmxControl = { 5, NULL, NULL, devBoNIDAQmx_init_record, NULL, devBoNIDAQmx_write_bo };
BINARYDSET	devAiNIDAQmxRawRead = { 6, NULL, NULL, devAiNIDAQmx_init_record, devAiNIDAQmx_get_ioint_info, devAiNIDAQmx_read_ai, NULL };
BINARYDSET  devWaveNIDAQmxRead = { 5, NULL, NULL, devWaveNIDAQmx_init_record, devWaveNIDAQmx_get_ioint_info, devWaveNIDAQmx_read_wave};

epicsExportAddress(dset, devWaveNIDAQmxRead);
epicsExportAddress(dset, devAoNIDAQmxControl);
epicsExportAddress(dset, devBoNIDAQmxControl);
epicsExportAddress(dset, devAiNIDAQmxRawRead);
