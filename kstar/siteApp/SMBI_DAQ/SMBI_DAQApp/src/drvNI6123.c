#include <fcntl.h>	// for mkdir

#include "drvNI6123.h"

static long drvNI6123_io_report(int level);
static long drvNI6123_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNI6123 = {
	2,
	drvNI6123_io_report,
	drvNI6123_init_driver
};

epicsExportAddress(drvet, drvNI6123);

// sleep time of DAQ thread (milli seconds)
int kDAQSleepMillis	= 1000;
epicsExportAddress (int, kDAQSleepMillis);

int kAutoTrendRunWaitMillis	= 3000;
epicsExportAddress (int, kAutoTrendRunWaitMillis);

int n8After_Processing_Flag		= 0; 

//Changed by linupark[[
#ifdef TREND_ENABLE
int gbCalcRunMode  			= 0;	// 0 : Trend mode, 1 : Calcuration & store mode
#else
int gbCalcRunMode  			= 1;	// 0 : Trend mode, 1 : Calcuration & store mode
#endif
//]]
int gbAutoTrendRunMode			= 1;	// 1 : Automatically start DAQ for trending

int NI_err_message(char *fname, int lineNo, int error)
{
	char	errBuff[2048]={'\0'};

	if ( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,sizeof(errBuff));
		kLog (K_ERR, "[%s:%d] DAQmx Error: %s\n", fname, lineNo, errBuff);
		notify_error (1, "DAQmx Error: %s\n", errBuff);

		epicsThreadSleep(1);
	}

	return (0);
}

int isCalcRunMode ()
{
	return (gbCalcRunMode);
}

void setCalcRunMode (int bCalcRunMode)
{
	kLog (K_MON, "[setCalcRunMode] %d\n", bCalcRunMode);
	gbCalcRunMode = bCalcRunMode; 
}

int isAutoTrendRunMode ()
{
	return ( (get_master()->ST_Base.opMode == OPMODE_REMOTE) && (TRUE == gbAutoTrendRunMode) );
}

void setAutoTrendRunMode (int bAutoTrendRunMode)
{
	gbAutoTrendRunMode = bAutoTrendRunMode;
}

//Added for CalibMode by linupark[[
int isRemoteMode ()
{
	return (OPMODE_REMOTE == get_master()->ST_Base.opMode);
}

int isLocalMode ()
{
	return (OPMODE_LOCAL == get_master()->ST_Base.opMode);
}

int isCalibMode ()
{
	return (OPMODE_CALIBRATION == get_master()->ST_Base.opMode);
}

int isStoreMode ()
{
	return ( isRemoteMode() || isLocalMode() );
}

//]]

int isFirstDevice (ST_STD_device *pSTDdev)
{
	return (0 == pSTDdev->BoardID);
}

ST_NI6123 *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NI6123 *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NI6123 *pNI6123) 
{ 
	return (pNI6123->sample_rate); 
}

int getArraySizeInBytes (ST_NI6123 *pNI6123) 
{
   	return (sizeof(pNI6123->readArray)); 
}

int getMaxNumChan (ST_NI6123 *pNI6123) 
{ 
	return (INIT_MAX_CHAN_NUM); 
}

float64 getTotSampsChan (ST_NI6123 *pNI6123) 
{ 
	return (pNI6123->sample_rate * pNI6123->sample_time); 
}

int getTotSampsAllChan (ST_NI6123 *pNI6123) 
{ 
	return (pNI6123->sample_rate * getMaxNumChan(pNI6123)); 
}

int getBeamPulseTime (ST_NI6123 *pNI6123) 
{ 
	return (pNI6123->beam_pulse); 
}

int getSampRate (ST_NI6123 *pNI6123) 
{ 
	return (pNI6123->sample_rate); 
}

float64 getSampTime (ST_NI6123 *pNI6123) 
{ 
	return (pNI6123->sample_time); 
}

/* void setSamplTime (ST_NI6123 *pNI6123) */
void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_NI6123 *pNI6123 = NULL;
	pNI6123 = pSTDdev->pUser;

/*	pNI6123->sample_time = (pNI6123->stop_t1 <= pNI6123->start_t0) ? 0 : (pNI6123->stop_t1 - pNI6123->start_t0);   */

	if(pNI6123->stop_t1 <= pNI6123->start_t0) {
		pNI6123->sample_time = 0;
	} else {
		pNI6123->sample_time = pNI6123->stop_t1 - pNI6123->start_t0;
	}

	kLog (K_INFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pNI6123->start_t0, pNI6123->stop_t1, pNI6123->sample_time);
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}
void setStartT0 (ST_NI6123 *pNI6123, float64 val) 
{
	pNI6123->start_t0 = val;
	pNI6123->sample_time = (pNI6123->stop_t1 <= pNI6123->start_t0) ? 0 : (pNI6123->stop_t1 - pNI6123->start_t0);
/*	setSamplTime (pNI6123);  */
}

void setStopT1 (ST_NI6123 *pNI6123, float64 val)
{
	pNI6123->stop_t1 = val;
	pNI6123->sample_time = (pNI6123->stop_t1 <= pNI6123->start_t0) ? 0 : (pNI6123->stop_t1 - pNI6123->start_t0);
/*	setSamplTime (pNI6123);  */
}

void armingDeviceParams (ST_NI6123 *pNI6123)
{
	int		i;
	float64	sampRate	= getSampRate (pNI6123);
	float64	pulseLength	= getBeamPulseTime (pNI6123);
	float64	tempVal		= NBI_POWER_CONST / pulseLength / sampRate;

	pNI6123->totalRead	= 0;

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		pNI6123->ST_Ch[i].offset		= 0;
		pNI6123->ST_Ch[i].power			= 0;
		pNI6123->ST_Ch[i].accum_temp	= 0;
		pNI6123->ST_Ch[i].accum_cnt		= 0;
		pNI6123->ST_Ch[i].integral_cnt	= pNI6123->ST_Ch[i].integral_time * sampRate;
		pNI6123->ST_Ch[i].coefficient	= pNI6123->ST_Ch[i].flow * tempVal;

		kLog (K_DEBUG, "[armingDeviceParams] %s : ch(%d) time(%f) cnt(%f) flow(%f) coeff(%f) \n",
				pNI6123->ST_Ch[i].physical_channel, i,
				pNI6123->ST_Ch[i].integral_time, pNI6123->ST_Ch[i].integral_cnt,
				pNI6123->ST_Ch[i].flow, pNI6123->ST_Ch[i].coefficient);
	}
}



int processInputData (int bCalcRunMode, ST_NI6123 *pNI6123, int chNo, intype rawValue)
{
	ST_NI6123_channel *pChInfo	= &pNI6123->ST_Ch[chNo];

	// raw input value
	pChInfo->rawValue	= rawValue;

	// eu conversion
	// -----------------------------------------------
	// EU(V)  = Raw * (EU_H - EU_L) / (Raw_H - Raw_L)
	// EU(.C) = EU(V) * NBI_TEMP_PER_VOLT
	// -----------------------------------------------
	// temp function -> pChInfo->euValue	= NBI_TEMP_PER_VOLT * rawValue * EU_GRADIENT;
	pChInfo->euValue	= rawValue * EU_GRADIENT;

	kLog (K_DEBUG, "[processInputData] chNo(%d) raw(%d) orgEu(%.2f)\n", chNo, pChInfo->rawValue, pChInfo->euValue);

	if (TRUE != bCalcRunMode) {
		return (0);
	}
#if 0
	if (NBI_INPUT_DELTA_T == pChInfo->inputType) {
		if (pChInfo->integral_cnt > pChInfo->accum_cnt) {
			// accumulate of Delta-T
			pChInfo->accum_temp	+= pChInfo->rawValue;

			// power
			pChInfo->power		= pChInfo->accum_temp * pChInfo->coefficient;

			// increase accumulate counter to check the end time for integral
			pChInfo->accum_cnt++;
		}

		kLog (K_DEBUG, "[processInputData] offset(%d) rawValue(%d) eu(%.2f) power(%.2f) accum(temp:%.2f, cnt:%.f)\n",
				pChInfo->offset, pChInfo->rawValue,
				pChInfo->euValue, pChInfo->power, pChInfo->accum_temp, pChInfo->accum_cnt);
	}
#endif
	return (0);
}

float64 getStartTime (ST_STD_device *pSTDdev)
{
	float64	fStartTime = 0;

	if (pSTDdev->ST_Base.opMode == OPMODE_REMOTE) {
		fStartTime  = (pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime);
	} else 	fStartTime  = pSTDdev->ST_Base_fetch.dT0[0];

	return (fStartTime);
}

float64 getEndTime (ST_STD_device *pSTDdev)
{
	return (pSTDdev->ST_Base_fetch.dT1[0] - pSTDdev->ST_Base_fetch.dT0[0]);
}

int isSamplingStop (ST_STD_device *pSTDdev)
{
	ST_NI6123	*pNI6123 = getDriver(pSTDdev);
//Changed by linupark[[
#ifdef TREND_ENABLE
	if (TRUE != isStoreMode ()) {
		return (FALSE);
	}

	return ( pNI6123->totalRead >= getTotSampsChan(pNI6123)  );
#else
	return ( isCalcRunMode() ? (pNI6123->totalRead >= getTotSampsChan(pNI6123)) : FALSE );
#endif

//]]

}

int isTriggerRequiredMode (const int mode)
{
	switch (mode) {
		case OPMODE_REMOTE :
/*		case OPMODE_LOCAL :   */
			return (TRUE);
	}

	return (FALSE);
}

int isTriggerRequired (ST_STD_device *pSTDdev)
{
//Changed by linupark[[
#ifdef TREND_ENABLE
	return ( isTriggerRequiredMode(pSTDdev->ST_Base.opMode) );
#else
	return ( isTriggerRequiredMode(pSTDdev->ST_Base.opMode) && isCalcRunMode() );
#endif
//]]
}

int isAutoRunMode (ST_STD_device *pSTDdev)
{
	//Changed by linupark[[
#ifdef TREND_ENABLE
	return ( (OPMODE_REMOTE != pSTDdev->ST_Base.opMode));
#else
	return ( (OPMODE_REMOTE != pSTDdev->ST_Base.opMode) || !isCalcRunMode() );
#endif
//]]

}

int isRemoteRunning ()
{
	ST_MASTER *pMaster = get_master();

//Changed by linupark[[
#ifdef TREND_ENABLE
return ((pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
		( (pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ||
		  (pMaster->StatusAdmin & TASK_IN_PROGRESS) ||
		  (pMaster->StatusAdmin & TASK_POST_PROCESS) ||
		  (pMaster->StatusAdmin & TASK_DATA_TRANSFER) ) );
#else
return ( isCalcRunMode() &&
		(pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
		( (pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ||
		  (pMaster->StatusAdmin & TASK_IN_PROGRESS) ||
		  (pMaster->StatusAdmin & TASK_POST_PROCESS) ||
		  (pMaster->StatusAdmin & TASK_DATA_TRANSFER) ) );
#endif
//]]
}

void make_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);

	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int i;
/*  Remove this function TGLee. I just over write same file
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);
*/
	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		pNI6123->ST_Ch[i].write_fp = NULL;

		if (CH_USED == pNI6123->ST_Ch[i].used) {
/*  Remove this function TGLee. I just over write same file
			sprintf (pNI6123->ST_Ch[i].path_name,"%sS%06d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6123->ST_Ch[i].file_name);
*/
			sprintf (pNI6123->ST_Ch[i].path_name,"%s%s",
                                        STR_LOCAL_DATA_DIR, pNI6123->ST_Ch[i].file_name);
			kLog (K_INFO, "[make_local_file] fname(%s)\n", pNI6123->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pNI6123->ST_Ch[i].write_fp = fopen(pNI6123->ST_Ch[i].path_name, "w+"))) {
				kLog (K_ERR, "[make_local_file] %s create failed!!!\n", pNI6123->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6123->ST_Ch[i].path_name);
			}
			if (pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {
				pNI6123->ST_Ch[i].write_den_fp = NULL;
				sprintf (pNI6123->ST_Ch[i].path_den_name, "%s%s",pNI6123->ST_Ch[i].path_name, "Density");
				kLog (K_INFO, "[make_local_file] density fname(%s)\n", pNI6123->ST_Ch[i].path_den_name);
				if (NULL == (pNI6123->ST_Ch[i].write_den_fp = fopen(pNI6123->ST_Ch[i].path_den_name, "w+"))) {
               		kLog (K_ERR, "[make_local_file] density %s create failed!!!\n", pNI6123->ST_Ch[i].path_den_name);
                	notify_error (1, "%s create density file failed!\n", pNI6123->ST_Ch[i].path_den_name);
            	}
			}
		}
	}
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6123->ST_Ch[i].write_fp) {
			fflush(pNI6123->ST_Ch[i].write_fp);
		}
		if (pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {
			if (NULL != pNI6123->ST_Ch[i].write_den_fp) {
				fflush(pNI6123->ST_Ch[i].write_den_fp);
			}
		}
	}
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6123->ST_Ch[i].write_fp) {
			fclose(pNI6123->ST_Ch[i].write_fp);
			pNI6123->ST_Ch[i].write_fp	= NULL;
		}
		if (pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {
			if (NULL != pNI6123->ST_Ch[i].write_den_fp) {
				fclose(pNI6123->ST_Ch[i].write_den_fp);
				pNI6123->ST_Ch[i].write_den_fp	= NULL;
			}
		}
	}
}

int updateShotNumber ()
{
	ST_MASTER *pMaster = get_master();
	char strBuf[24];
	int	prevShotNumber;

	prevShotNumber	= pMaster->ST_Base.shotNumber;

	DBproc_get (PV_LOCAL_SHOT_NUMBER_STR, strBuf);

	sscanf (strBuf, "%lu", &pMaster->ST_Base.shotNumber);

	flush_ShotNum_to_All_STDdev();

	kLog (K_MON, "[updateShotNumber] from(%d) -> to(%d)\n", prevShotNumber, pMaster->ST_Base.shotNumber);

	return (0);
}

int createLocalMdsTree (ST_STD_device *pSTDdev)
{
	if (OPMODE_REMOTE == pSTDdev->ST_Base.opMode) {
		return (WR_ERROR);
	}

	if (TRUE != isFirstDevice(pSTDdev)) {
		return (WR_OK);
	}

	int		opMode = pSTDdev->ST_Base.opMode;

	kLog (K_MON, "[createLocalMdsTree] %s : mode(%s) ip(%s) tree(%s) shot(%d)\n",
			pSTDdev->taskName,
			getModeMsg(opMode), pSTDdev->ST_mds.treeIPport[opMode],
			pSTDdev->ST_mds.treeName[opMode], pSTDdev->ST_Base.shotNumber);

#if 1 
	if (WR_OK == mds_Connect (pSTDdev)) {
		mds_createNewShot (pSTDdev);
		mds_Disconnect (pSTDdev);

		return (WR_OK);
	}

	return (WR_ERROR);
#else
	// local tree is created by SMBI_DAQ

	kLog (K_MON, "[createLocalMdsTree] %s : local tree is created by SMBI_DAQ\n", pSTDdev->taskName);

	return (WR_OK);
#endif
}

int stopDevice (ST_STD_device *pSTDdev)
{
	ST_NI6123 *pNI6123 = pSTDdev->pUser;

	if (0 != pNI6123->taskHandle) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNI6123->totalRead);

		DAQmxTaskControl(pNI6123->taskHandle, DAQmx_Val_Task_Abort);
		NIERROR(DAQmxStopTask(pNI6123->taskHandle));

		if (pNI6123->auto_run_flag == 1) {
			kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
			pNI6123->auto_run_flag = 0;

			// wait some times to synchronize running mode with device threads
			epicsThreadSleep (0.5);
		}
	}

	return (0);
}

int resetDevice (ST_STD_device *pSTDdev)
{
	stopDevice (pSTDdev);

	pSTDdev->StatusDev = TASK_SYSTEM_IDLE | TASK_STANDBY;

	notify_refresh_master_status ();

	return (0);
}

int resetDeviceAll ()
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	kLog (K_MON, "[resetDeviceAll] ...\n");

	while (pSTDdev) {
		resetDevice (pSTDdev);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	// clear error message
	sprintf (pMaster->ErrorString, "Clear");

	return (0);
}

void setInitStatus (ST_STD_device* pSTDdev)
{
	pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

	if (TRUE == isFirstDevice (pSTDdev)) {
		DBproc_put(PV_SYS_ARMING_STR, "0");
		DBproc_put(PV_SYS_RUN_STR, "0");
	}
}

void armingDevice (ST_STD_device* pSTDdev)
{
	ST_MASTER *pMaster	= get_master();

//Changed by linupark[[
#ifdef TREND_ENABLE
	kLog (K_MON, "[armingDevice] %s\n", pSTDdev->taskName);
#else
	kLog (K_MON, "[armingDevice] %s : mode(%d)\n", pSTDdev->taskName, isCalcRunMode());
#endif
//]]
	// clear error message
	sprintf (pMaster->ErrorString, "Clear");

	// reset the running task
	stopDevice (pSTDdev);

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_ARM_ENABLED;

	pSTDdev->ST_Base.dT0[0]		= pMaster->ST_Base.dT0[0];
	pSTDdev->ST_Base_fetch.dT0[0]	= pMaster->ST_Base.dT0[0];

	pSTDdev->ST_Base.dT1[0]		= pMaster->ST_Base.dT1[0];
	pSTDdev->ST_Base_fetch.dT1[0]	= pMaster->ST_Base.dT1[0];

	kLog (K_MON, "[armingDevice] T0/T1 : Dev(%s) Master(%.f/%.f) Dev(%.f/%.f) fetch(%.f/%.f)\n",
			pSTDdev->taskName,
			pMaster->ST_Base.dT0[0], pMaster->ST_Base.dT1[0], 
			pSTDdev->ST_Base.dT0[0], pSTDdev->ST_Base.dT1[0], 
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0]);

//Changed by linupark[[
#ifdef TREND_ENABLE
flush_STDdev_to_MDSfetch (pSTDdev);
if (TRUE == isStoreMode ()) {
		make_local_file (pSTDdev);
	}
#else
if (TRUE == isCalcRunMode()) {
	flush_STDdev_to_MDSfetch (pSTDdev);

	make_local_file (pSTDdev);

}
#endif
//]]
	
}

int processTrendRun ()
{
	setAutoTrendRunMode (FALSE);

	resetDeviceAll ();

	setCalcRunMode (FALSE);

	// start DAQ for trending
	DBproc_put (PV_CALC_RUN_STR, "0");		// reset button
	DBproc_put (PV_SYS_ARMING_STR, "1");	// arming

	return (OK);
}

int processCalcRun ()
{
	setAutoTrendRunMode (FALSE);

	resetDeviceAll ();

	setCalcRunMode (TRUE);

	// start DAQ for calcuration and storing
	DBproc_put (PV_TREND_RUN_STR, "0");	// reset button

	if (get_master()->ST_Base.opMode != OPMODE_REMOTE) {
		DBproc_put (PV_SYS_ARMING_STR, "1");	// arming
	}

	return (OK);
}
//Changed by linupark[[
/*
int processAutoTrendRun (ST_STD_device *pSTDdev)
{
	if (TRUE == isAutoTrendRunMode ()) {
		if (TRUE == isFirstDevice (pSTDdev)) {
			if (get_master()->StatusAdmin & TASK_SYSTEM_IDLE) {
				kLog (K_MON, "[processAutoTrendRun] delay ...\n");

				epicsThreadSleep (kAutoTrendRunWaitMillis/1000.);

				kLog (K_MON, "[processAutoTrendRun] start ...\n");

				processTrendRun ();
			}
		}
	}
	
	return (0);
}
*/
//]]
// makes name of tag for raw value
int drvNI6123_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	ST_NI6123 *pNI6123 = (ST_NI6123 *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvNI6123_set_TagName] ...\n");

	for(i=0; i<INIT_MAX_CHAN_NUM; i++) {
		sprintf(pNI6123->ST_Ch[i].strTagName, "\\%s:FOO", pNI6123->ST_Ch[i].inputTagName);
		kLog (K_INFO, "%s\n", pNI6123->ST_Ch[i].strTagName);
	}

	return WR_OK;
}
//Changed by linupark[[
// starts DAQ for trending
/*
void func_NI6123_POST_SEQSTOP(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[POST_SEQSTOP] %s : value(%d) status(%d)\n", pSTDdev->taskName, (int)arg1, (int)arg2);

	if (0 == (int)arg1) {
		if (TRUE != isRemoteRunning ()) {
			processTrendRun ();
		}
		else {
			setAutoTrendRunMode (TRUE);

			kLog (K_INFO, "[POST_SEQSTOP] %s : DAQ is running in remote mode\n", pSTDdev->taskName);
		}
	}
}

// stops DAQ to ready shot start
void func_NI6123_PRE_SEQSTART(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[PRE_SEQSTART] %s : setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		processCalcRun ();
	}
}
*/
//]]
void func_NI6123_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_RESET] %s: setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		resetDevice (pSTDdev);

		admin_all_taskStatus_reset();

		scanIoRequest(pMaster->ioScanPvt_status);

		DBproc_put(PV_RESET_STR, "0");
	}
}

void func_NI6123_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;
	ST_NI6123 *pNI6123 = (ST_NI6123 *)pSTDdev->pUser;
	int i, j;
	int sampleRate;
	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);

	sampleRate = pNI6123->sample_rate;
	if (1 == (int)arg1) { /* in case of arming  */
		if (admin_check_Arming_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : arming failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		armingDevice (pSTDdev);

#if AUTO_RUN
		if (TRUE == isAutoRunMode (pSTDdev)) {
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		}
#endif
	}
	else { /* release arming condition */
		if (admin_check_Release_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : release failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: release failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
				if(pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {  /* init density data channel */
					pNI6123->ST_Ch[i].isFirst = 0;	
					pNI6123->ST_Ch[i].density = 0;
					for(j=0; j<sampleRate; j++) {
						pNI6123->ST_Ch[i].calcDensity[j] = 0;	
					}
					scanIoRequest(pSTDdev->ioScanPvt_userCall);
				}
			}
			return;
		}
		for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
			if(pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {  /* init density data channel */
				pNI6123->ST_Ch[i].isFirst = 0;	
				pNI6123->ST_Ch[i].density = 0;
				for(j=0; j<sampleRate; j++) {
					pNI6123->ST_Ch[i].calcDensity[j] = 0;	
				}
				scanIoRequest(pSTDdev->ioScanPvt_userCall);
			}
		}

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6123_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_RUN] %s : %d\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		if (admin_check_Run_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_RUN] %s : run failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: run failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6123_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[OP_MODE] %s : %f, %f\n", pSTDdev->taskName, arg1, arg2);

	if (pMaster->ST_Base.opMode != OPMODE_INIT) {
		pSTDdev->StatusDev |= TASK_STANDBY;
/* Ask */
		resetDeviceAll ();

		if (OPMODE_LOCAL == pMaster->ST_Base.opMode) {
			updateShotNumber ();
		}
	}

	mds_copy_master_to_STD(pSTDdev);

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

/*
void func_NI6123_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	kLog (K_MON, "[DATA_SEND] %s\n", pSTDdev->taskName);

	switch (pSTDdev->ST_Base.opMode) {
		case OPMODE_CALIBRATION :
			break;

		case OPMODE_REMOTE :
		case OPMODE_LOCAL :
			proc_sendData2Tree(pSTDdev);
			break;

		default :
			notify_error (1, "%s: Wrong op mode (%d)!\n", pSTDdev->taskName, pSTDdev->ST_Base.opMode);
			break;
	}
}
*/
void func_NI6123_CREATE_LOCAL_SHOT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_SHOT] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}

void func_NI6123_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NI6123_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6123 *pNI6123 = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNI6123 = pSTDdev->pUser;
	pNI6123->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

void func_NI6123_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6123 *pNI6123 = NULL;

	kLog (K_MON, "[func_NI6123_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	pNI6123 = pSTDdev->pUser;

	pNI6123->start_t0 = arg1;
	setSamplTime (pSTDdev);
/*	setSamplTime (pNI6123);      */
/*	setStartT0 (pNI6123, arg1);  */
}

void func_NI6123_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6123 *pNI6123 = NULL;
//Added by linupark
	char strBuf[24];
//]]

	kLog (K_MON, "[func_NI6123_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	pNI6123 = pSTDdev->pUser;

	pNI6123->stop_t1 = arg1;
	setSamplTime (pSTDdev);     
/*	setSamplTime (pNI6123);      */
/*	setStopT1 (pNI6123, arg1);   */
//Added by linupark for setting T0 [[
#if 1
	switch (pSTDdev->ST_Base.opMode) {
		case OPMODE_CALIBRATION :
		case OPMODE_LOCAL :
			break;				
		case OPMODE_REMOTE :
			memset(strBuf, 0x00, sizeof(char)*24);
			if(!strcmp(strBuf, ""))
			{				
				kLog (K_MON, "[func_NI6123_T1] OPMODE_REMOTE DBproc_get->put star\n");
				DBproc_get("SMBI_LTU_T0_p0",strBuf);
				DBproc_put("SMBI_DAQ_START_T0_SEC1",strBuf);
				kLog (K_MON, "[func_NI6123_T1] OPMODE_REMOTE DBproc_get->put end\n");
			}
			break;				
		default :
			notify_error (1, "%s: Wrong op mode (%d) in threadFunc_NI6123_DAQ!\n", pSTDdev->taskName, pSTDdev->ST_Base.opMode);
			break;
	}
#endif
//]]

}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	ST_STD_device	*pSTDdev = (ST_STD_device*) callbackData;
	ST_NI6123		*pNI6123 = pSTDdev->pUser;

	int 	i;
	int32	sampsRead=0;
	int32	numSampsPerChan = getNumSampsPerChan(pNI6123);
	int32	totSampsAllChan	= getTotSampsAllChan(pNI6123);
	int32	maxNumChan = getMaxNumChan(pNI6123);

	unsigned long long int writeFileStart;
	unsigned long long int writeFileStop;
	unsigned long long int readDataStart;
	unsigned long long int readDataStop;
	unsigned long long int start;
	unsigned long long int stop;
	start = wf_getCurrentUsec();
#if !USE_SCALING_VALUE
	int32	numBytesPerSamp = 0;
	int32 	arraySizeInBytes = getArraySizeInBytes(pNI6123);
#endif

//Changed by linupark[[
#ifdef TREND_ENABLE
	kLog (K_MON, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) shot(%lu/%lu)\n",
	pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6123->sample_rate,
	getStatusMsg(pSTDdev->StatusDev, NULL),
	pNI6123->totalRead, get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);
#else
	kLog (K_MON, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) mode(%d) shot(%lu/%lu)\n",
			pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6123->sample_rate,
			getStatusMsg(pSTDdev->StatusDev, NULL),
			pNI6123->totalRead, isCalcRunMode(), get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);
#endif
//]]
	if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

		kLog (K_INFO, "[EveryNCallback] %s : status(0x%x:%s)\n",
				pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

		notify_refresh_master_status();
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}

#if USE_SCALING_VALUE
	readDataStart = wf_getCurrentUsec();
	NIERROR(
		DAQmxReadAnalogF64 (
			pNI6123->taskHandle, 			// taskHandle
			numSampsPerChan, 				// numSampsPerChan. number of samples per channel to read
			600.0, 							// timeout (seconds)
			DAQmx_Val_GroupByChannel,	// DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved), DAQmx_Val_GroupByChannel (non-interleaved)
			pNI6123->fScaleReadArray, 		// readAray. array into which samples are read
			sizeof(pNI6123->fScaleReadArray),	// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 			// sampsRead. actual number of bytes read into the array per scan
			NULL							// reserved
		)
	);

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%.2f ", pNI6123->fScaleReadArray[i*maxNumChan]);
		}
		printf ("\n");
	}
	// converts scaling eu value to raw value of 16bits resolution
	for (i=0;i<totSampsAllChan;i++) {
		pNI6123->readArray[i] = GET_RAW_VALUE(pNI6123->fScaleReadArray[i]);
	}

	readDataStop = wf_getCurrentUsec();
	 kLog (K_MON,"[ EveryNCallback- read DAQmxReadAnalogF64 ] : measured function process time sec. is %lf msec\n",
				1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
#else
	NIERROR(
		DAQmxReadRaw (
			pNI6123->taskHandle, 	// taskHandle
			numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
			600.0, 					// timeout (seconds)
			pNI6123->readArray, 	// readAray. array into which samples are read
			arraySizeInBytes, 		// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 	// sampsRead. actual number of bytes read into the array per scan
			&numBytesPerSamp, 		// numBytesPerSamp
			NULL					// reserved
		)
	);
#endif
	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%d ", pNI6123->readArray[i*maxNumChan]);
		}
		printf ("\n");
	}

#if 0
  /* remove ... because I used not offset value */
	if (TRUE == isCalcRunMode ()) {
		if (0 >= pNI6123->totalRead) {
			// save the first offset to compensate
			for(i = 0; i<INIT_MAX_CHAN_NUM; i++) {
				pNI6123->ST_Ch[i].offset	= pNI6123->readArray[i];

				kLog (K_DEL, "[EveryNCallback] %s : ch(%d) offset(%d) eu(%.5f)\n", pSTDdev->taskName, i,
						pNI6123->readArray[i], pNI6123->fScaleReadArray[i]);
			}
		}

		pNI6123->totalRead	+= sampsRead;
	}
#endif
	pNI6123->totalRead	+= sampsRead;
	FILE *fp = NULL;
	int chNo;

	int32 arrayJump;

    /* When use this function -  DAQmx_Val_GroupByChannel (non-interleaved) at DAQmxReadAnalogF64 */
	for(chNo=0;	chNo<INIT_MAX_CHAN_NUM;	chNo++) {
		arrayJump = chNo * numSampsPerChan;

		writeFileStart = wf_getCurrentUsec();
		if (NULL != (fp = pNI6123->ST_Ch[chNo].write_fp)) {
			fwrite(&pNI6123->readArray[arrayJump], sizeof(pNI6123->readArray[0]), numSampsPerChan, pNI6123->ST_Ch[chNo].write_fp);
			fflush(pNI6123->ST_Ch[chNo].write_fp);
		}
		writeFileStop = wf_getCurrentUsec();
		epicsPrintf("[ EveryNCallback - write raw data to local file chNu :%d ] : measured function process time sec. is %lf msec\n", 
						chNo, 1.E-3 * (double)wf_intervalUSec(writeFileStart,writeFileStop));
		pNI6123->ST_Ch[chNo].rawValue  = pNI6123->readArray[arrayJump+1];
		pNI6123->ST_Ch[chNo].euValue  = pNI6123->fScaleReadArray[arrayJump+1];
		
		if(pNI6123->ST_Ch[chNo].inputType == DENSITY_CALC) {  /* Write data from bufferAllChannel to channel buffer for density channel */
			/* copy 2 channels, two channel is density channel and next channel  */
			memcpy(pNI6123->ST_Ch[chNo].fScaleReadChannel, &pNI6123->fScaleReadArray[arrayJump],sizeof(float64)* numSampsPerChan);
			memcpy(pNI6123->ST_Ch[chNo+1].fScaleReadChannel, &pNI6123->fScaleReadArray[(chNo+1) * numSampsPerChan],sizeof(float64)* numSampsPerChan);
			processDensity(pNI6123, chNo);
			writeFileStart = wf_getCurrentUsec();
			if (NULL != (fp = pNI6123->ST_Ch[chNo].write_den_fp)) {
				fwrite(&pNI6123->ST_Ch[chNo].calcDensity, sizeof(pNI6123->ST_Ch[chNo].calcDensity[0]), numSampsPerChan, pNI6123->ST_Ch[chNo].write_den_fp);
				fflush(pNI6123->ST_Ch[chNo].write_den_fp);
			}
			writeFileStop = wf_getCurrentUsec();
			epicsPrintf("[ EveryNCallback - write float64 calc data to local file chNu :%d  ] : measured function process time sec. is %lf msec\n", 
							chNo, 1.E-3 * (double)wf_intervalUSec(writeFileStart,writeFileStop));
		}
	}
	
#if 0 
		/* change write to file routine */
		for(i=0; i<numSampsPerChan; i++) {
			if (NULL != (fp = pNI6123->ST_Ch[chNo].write_fp)) {
				// write to file
				fwrite(&pNI6123->readArray[arrayJump+i], sizeof(pNI6123->readArray[0]), 1, pNI6123->ST_Ch[chNo].write_fp);
				fflush(pNI6123->ST_Ch[chNo].write_fp);

				if (0 == (i % 10)) {
					kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
							pSTDdev->taskName, chNo, pNI6123->ST_Ch[chNo].file_name, pNI6123->readArray[arrayJump+i]);
				}
			}
		}
#endif


#if 0
    /* When use this function -  DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved) at DAQmxReadAnalogF64 */
	for (i=0;i<totSampsAllChan;i++) {
		chNo = GET_CHID(i);

		pNI6123->ST_Ch[chNo].rawValue  = pNI6123->readArray[i];
		pNI6123->ST_Ch[chNo].euValue  = pNI6123->fScaleReadArray[i];
		// offset compensation
		/* remove ... because I used not offset value */
		/*		pNI6123->readArray[i]	-= pNI6123->ST_Ch[chNo].offset;   */

		if (NULL != (fp = pNI6123->ST_Ch[chNo].write_fp)) {
			// write to file
			fwrite(&pNI6123->readArray[i], sizeof(pNI6123->readArray[0]), 1, pNI6123->ST_Ch[chNo].write_fp);
			fflush(pNI6123->ST_Ch[chNo].write_fp);

			if (0 == chNo) {
				kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
						pSTDdev->taskName, chNo, pNI6123->ST_Ch[chNo].file_name, pNI6123->readArray[i]);
			}
		}

		// process input data : offset compensation, eu conversion, integral
		/* remove TG.Lee processInputData (isCalcRunMode(), pNI6123, chNo, pNI6123->readArray[i]);  */
	}
#endif
	stop = wf_getCurrentUsec();
	epicsPrintf("[ EveryNCallback ] : measured function process time sec. is %lf msec\n", 1.E-3 * (double)wf_intervalUSec(start,stop));

	scanIoRequest(pSTDdev->ioScanPvt_userCall);

	if (TRUE == isSamplingStop (pSTDdev)) {
		kLog (K_INFO, "[EveryNCallback] %s : stop ....\n", pSTDdev->taskName);
		stopDevice (pSTDdev);
		
	}

	return 0;
}

float64 average(ST_NI6123 *pNI6123, int chNo)
{
	float64 sum = 0.0;
	int i;
	int32	numSampsPerChan = getNumSampsPerChan(pNI6123);
	if (numSampsPerChan > 1000) numSampsPerChan=1000;
	for (i=0; i<numSampsPerChan; i++)
	{
		sum += pNI6123->ST_Ch[chNo].fScaleReadChannel[i]; 
	}
	return sum / numSampsPerChan;
}
int processDensity (ST_NI6123 *pNI6123, int chNo)
{
	int selectCh;
	int i;
	unsigned long long int start;
	unsigned long long int stop;
	start = wf_getCurrentUsec();
	int32	numSampsPerChan = getNumSampsPerChan(pNI6123);
	pNI6123->ST_Ch[chNo].avgNES1 = average(pNI6123, chNo);
	pNI6123->ST_Ch[chNo].avgNES2 = average(pNI6123, chNo+1);

	/* 1. Get n-th data from reading channel */
	/* 2. Voltage is in stable range ? No, goto #3.  Yes, goto #6  */
	if(pNI6123->ST_Ch[chNo].isFirst == 0) {
		if (abs(pNI6123->ST_Ch[chNo].avgNES1 - 0.1) < abs(pNI6123->ST_Ch[chNo].avgNES2 - 0.1))
		{
			selectCh = chNo;
			pNI6123->ST_Ch[chNo].density =- pNI6123->ST_Ch[chNo].avgNES1;
		} else
		{
			selectCh = chNo+1;
			pNI6123->ST_Ch[chNo].density =- pNI6123->ST_Ch[chNo].avgNES2;
		}
		pNI6123->ST_Ch[chNo].isFirst = 1;
		kLog (K_INFO,"[threadFunc_processDensity_isFirst ] Select Ch : %d , Channel Number : %d \n", selectCh, chNo);
	} else {
		selectCh = pNI6123->ST_Ch[chNo].selectCh;
	}

	/* 3. Difference from center is smaller than another channel? No, goto #4.  Yes, goto #6  */
	/* 5. set new base density */
	for (i=0; i<numSampsPerChan; i++)
	{
	/* 2. Voltage is in stable range ? No, goto #3.  Yes, goto #6  */
		if(pNI6123->ST_Ch[selectCh].fScaleReadChannel[i] > pNI6123->ST_Ch[chNo].dLimUpper)
		{
	/* 4. change reading channel */
			if (selectCh==chNo)
			{
				selectCh=chNo+1;
			} else {
				selectCh=chNo;
			}
    /* 6. Voltage to Density Conversion */
		pNI6123->ST_Ch[chNo].density = pNI6123->ST_Ch[chNo].density + (pNI6123->ST_Ch[chNo].fringe / 2.);
		}
		if(pNI6123->ST_Ch[selectCh].fScaleReadChannel[i] < pNI6123->ST_Ch[chNo].dLimLow)
		{
	/* 4. change reading channel */
			if (selectCh==chNo)
			{
				selectCh=chNo+1;
			} else {
				selectCh=chNo;
			}
    /* 6. Voltage to Density Conversion */
		pNI6123->ST_Ch[chNo].density = pNI6123->ST_Ch[chNo].density - (pNI6123->ST_Ch[chNo].fringe / 2.);
		}
	/* 7. Confirm data */
    /* 8. End of Data - No, goto #1. Yes, Save Corrected data */
	pNI6123->ST_Ch[chNo].calcDensity[i] = pNI6123->ST_Ch[chNo].density;	
	}
	pNI6123->ST_Ch[chNo].selectCh = selectCh;  /* remember last select channel for density calc */
	kLog (K_INFO,"[threadFunc_processDensity_Every Event ] Select Ch : %d , Channel Number : %d \n", selectCh, chNo);
	stop = wf_getCurrentUsec();
	kLog (K_INFO,"[threadFunc_processDensity_Every Event ] : measured function process time sec. is %lf msec\n", 1.E-3 * (double)wf_intervalUSec(start,stop));
	return (0);
}
void threadFunc_NI6123_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6123 *pNI6123 = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	unsigned long long int start;
	unsigned long long int stop;

	int32	totSampsAllChan;
	int32	numSampsPerChan;
//Added by linupark
	char strBuf[24];
//]]
	kLog (K_TRACE, "[NI6123_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_NI6123_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNI6123->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_MON, "[NI6123_DAQ] %s : mode(%s) rate(%.f) time(%f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6123->sample_rate, pNI6123->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_MON, "[NI6123_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_INFO, "[NI6123_DAQ] %s : mode(%s) rate(%.f) time(%f-%f = %f) beam(%.f)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6123->sample_rate,
				pNI6123->stop_t1, pNI6123->start_t0, pNI6123->sample_time,
				pNI6123->beam_pulse);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNI6123->taskHandle == 0)) {
			/*	processAutoTrendRun (pSTDdev);   */
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNI6123->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
//Added by linupark for setting T0 [[
#if 1
			switch (pSTDdev->ST_Base.opMode) {
				case OPMODE_CALIBRATION :
				case OPMODE_LOCAL :
					break;				
				case OPMODE_REMOTE :
					memset(strBuf, 0x00, sizeof(char)*24);
				kLog (K_MON, "[func_NI6123_T1] OPMODE_REMOTE DBproc_get->put star\n");
					DBproc_get("SMBI_LTU_T0_p0",strBuf);				
					DBproc_put("SMBI_DAQ_START_T0_SEC1",strBuf);					
				kLog (K_MON, "[func_NI6123_T1] OPMODE_REMOTE DBproc_get->put end\n");
					break;				
				default :
					notify_error (1, "%s: Wrong op mode (%d) in threadFunc_NI6123_DAQ!\n", pSTDdev->taskName, pSTDdev->ST_Base.opMode);
					break;
			}
#endif
//]]

				stopDevice (pSTDdev);

				NIERROR(DAQmxResetDevice(pSTDdev->strArg0));
				NIERROR(DAQmxCreateTask("",&pNI6123->taskHandle));

				kLog (K_INFO, "[NI6123_DAQ] %s : term_config(%d) minVal(%.f) maxVal(%.f) units(%d)\n",
						pSTDdev->taskName, pNI6123->terminal_config, pNI6123->minVal, pNI6123->maxVal, pNI6123->units);

				for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
					NIERROR (
						DAQmxCreateAIVoltageChan (
							pNI6123->taskHandle,			// taskHandle
							pNI6123->ST_Ch[i].physical_channel,	// physicalChannel
							"",					// nameToAssignToChannel
							pNI6123->terminal_config,		// terminalConfig
							pNI6123->minVal,			// minVal
							pNI6123->maxVal,			// maxVal
							pNI6123->units,				// units
							NULL					// customScaleName
						)
					);
					if(pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {  /* init density data channel */
						pNI6123->ST_Ch[i].isFirst = 0;	
						pNI6123->ST_Ch[i].density = 0;	
						scanIoRequest(pSTDdev->ioScanPvt_userCall);
					}

					kLog (K_INFO, "[NI6123_DAQ] DAQmxCreateAIVoltageChan : %s\n", pNI6123->ST_Ch[i].physical_channel);
				}

				totSampsAllChan	= getTotSampsAllChan(pNI6123);
				numSampsPerChan = getNumSampsPerChan(pNI6123);

				NIERROR(
					DAQmxCfgSampClkTiming(
						pNI6123->taskHandle,	// taskHandle
						"",			// source terminal of the Sample clock. NULL(internal)
						pNI6123->sample_rate,	// sampling rate in samples per second per channel
						DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
						pNI6123->smp_mode,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
						totSampsAllChan		// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
					)
				);

				kLog (K_INFO, "[NI6123_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
						pSTDdev->taskName,
						getModeMsg(pSTDdev->ST_Base.opMode),
						pNI6123->smp_mode, pNI6123->sample_rate, pNI6123->sample_time,
						numSampsPerChan, 
						totSampsAllChan);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
						DAQmxCfgDigEdgeStartTrig(
							pNI6123->taskHandle,
							pNI6123->triggerSource,
							DAQmx_Val_Rising
						)
					);

					kLog (K_MON, "[NI6123_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pNI6123->triggerSource);
				}
				else {
					kLog (K_MON, "[NI6123_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}

				NIERROR(
					DAQmxRegisterEveryNSamplesEvent(
						pNI6123->taskHandle,		// taskHandle
						DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
						numSampsPerChan,		// nSamples. number of samples after whitch each event should occur
						0,				// options. 0(callback function is called in a DAQmx thread)
						EveryNCallback,			// callbackFunction
						(void *)pSTDdev			// callbackData. parameter to be passed to callback function
					)
				);

				armingDeviceParams (pNI6123);

				NIERROR(DAQmxStartTask(pNI6123->taskHandle));

				kLog (K_INFO, "[threadFunc_NI6123_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNI6123->auto_run_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNI6123->auto_run_flag = 0;

				kLog (K_INFO, "[threadFunc_NI6123_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");
//Changed by linupark[[
#ifdef TREND_ENABLE

				if (isStoreMode()) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;
				}
				else {
					setInitStatus (pSTDdev);
				}

#else
				if (isCalcRunMode()) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;

				//  DBproc_put(PV_CALC_RUN_STR, "0"); 
				}
				else {
					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;
					DBproc_put(PV_TREND_RUN_STR, "0");
				}
#endif
//]]

				kLog (K_INFO, "[threadFunc_NI6123_DAQ] before DAQmxClearTask for %s\n", pSTDdev->taskName);
				NIERROR(DAQmxClearTask(pNI6123->taskHandle));
				kLog (K_INFO, "[threadFunc_NI6123_DAQ] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
				pNI6123->taskHandle = 0;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_POST_PROCESS)) {
				flush_local_file(pSTDdev);								

				pSTDdev->StatusDev = TASK_STANDBY | TASK_DATA_TRANSFER;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_DATA_TRANSFER)) {
				if(pMaster->n8MdsPutFlag == 0) {
					pMaster->n8MdsPutFlag = 1;

					start = wf_getCurrentUsec();
					for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
						if (proc_sendData2Tree(pSTDdev) == WR_OK) {
							pMaster->n8MdsPutFlag = 0;
							break;
						}

						epicsThreadSleep(1);

						kLog (K_INFO, "[threadFunc_NI6123_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}
					stop = wf_getCurrentUsec();
					epicsPrintf("[threadFunc_NI6123_DAQ - write to mdsplus DB Mode (%d) ] : measured function process time sec. is %lf msec\n",
																				pSTDdev->ST_Base.opMode, 1.E-3 * (double)wf_intervalUSec(start,stop));

//Added by linupark for auto stop (SYS_ARMING)[[
		DBproc_put(PV_SYS_RUN_STR, "0");
		DBproc_put(PV_SYS_ARMING_STR, "0");
//]]

					close_local_file (pSTDdev);

					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					notify_refresh_master_status();

					pMaster->n8MdsPutFlag = 0;
				}
				else if(pMaster->n8MdsPutFlag == 1) {
					kLog (K_INFO, "[threadFunc_NI6123_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_NI6123_RingBuf(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6123_RingBuf] ...\n");
}

void threadFunc_NI6123_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6123_CatchEnd] ...\n");
}

int create_NI6123_taskConfig(ST_STD_device *pSTDdev)
{
	ST_NI6123 *pNI6123 = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;

	pNI6123 = (ST_NI6123*) calloc(1, sizeof(ST_NI6123));
	if(!pNI6123) return WR_ERROR;
	
	kLog (K_TRACE, "[create_NI6123_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pNI6123->taskHandle = 0x0;
	pNI6123->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNI6123->data_buf) {
		free (pNI6123);
		return WR_ERROR;
	}

	ellInit(pNI6123->data_buf);

	for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
		sprintf (pNI6123->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
		sprintf (pNI6123->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);

		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			pNI6123->ST_Ch[i].used = CH_USED;

			strcpy (pNI6123->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			if (0 == strcmp (NBI_INPUT_TYPE_DT_STR, pSTDCh->strArg0)) {
				pNI6123->ST_Ch[i].inputType	= NBI_INPUT_DELTA_T;
			}
			else if (0 == strcmp (NBI_INPUT_TYPE_TC_STR, pSTDCh->strArg0)) {
				pNI6123->ST_Ch[i].inputType	= NBI_INPUT_TC;
			}
			else if (0 == strcmp (DENSITY_CALC_STR, pSTDCh->strArg2)) {
				pNI6123->ST_Ch[i].inputType = DENSITY_CALC;
				pNI6123->ST_Ch[i].isFirst = 0;
				pNI6123->ST_Ch[i].density = 0;
				pNI6123->ST_Ch[i].fringe = 0.2078;
				pNI6123->ST_Ch[i].dLimUpper = 0.16;
				pNI6123->ST_Ch[i].dLimLow = 0.05;
			}
			else {
				pNI6123->ST_Ch[i].inputType	= OTHER_INPUT_NONE;
			}

			kLog (K_INFO, "[create_NI6123_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pNI6123->ST_Ch[i].physical_channel,
					pNI6123->ST_Ch[i].inputType, pNI6123->ST_Ch[i].inputTagName,
					pNI6123->ST_Ch[i].file_name);
		}
	}

	sprintf (pNI6123->triggerSource, "%s", pSTDdev->strArg1); 

	pNI6123->terminal_config	=       DAQmx_Val_Cfg_Default;	// DAQmx_Val_Diff(6123), DAQmx_Val_NRSE NI-6254(DAQmx_Val_RSE)
	pNI6123->minVal				= INIT_LOW_LIMIT;
	pNI6123->maxVal				= INIT_HIGH_LIMIT;
	pNI6123->sample_rate		= INIT_SAMPLE_RATE;
	pNI6123->sample_time		= getSampTime(pNI6123);
	pNI6123->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNI6123->units				= DAQmx_Val_Volts;

	pSTDdev->pUser				= pNI6123;

	pSTDdev->ST_Func._OP_MODE		= func_NI6123_OP_MODE;
	pSTDdev->ST_Func._CREATE_LOCAL_SHOT	= func_NI6123_CREATE_LOCAL_SHOT;
	pSTDdev->ST_Func._SYS_ARMING		= func_NI6123_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN		= func_NI6123_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_NI6123_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET		= func_NI6123_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0		= func_NI6123_T0;
	pSTDdev->ST_Func._SYS_T1		= func_NI6123_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NI6123_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NI6123_SHOT_NUMBER;

//Changed by linupark[[
/*
	pSTDdev->ST_Func._POST_SEQSTOP		= func_NI6123_POST_SEQSTOP; 
	pSTDdev->ST_Func._PRE_SEQSTART		= func_NI6123_PRE_SEQSTART;
*/
//]]
	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NI6123_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 0
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6123_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6123_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvNI6123_init_driver(void)
{
	kLog (K_TRACE, "[drvNI6123_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_NI6123_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvNI6123_init_driver] %s : create_NI6123_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvNI6123_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNI6123_io_report(int level)
{
	kLog (K_TRACE, "[drvNI6123_io_report] ...\n");

	ST_STD_device *pSTDdev;
	ST_MASTER *pMaster = get_master();

	if (!pMaster) return -1;

	if (ellCount(pMaster->pList_DeviceTask)) {
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
	}
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pMaster->pList_DeviceTask));

	if (level<1) return -1;

	while (pSTDdev) {
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, (unsigned int)pSTDdev, pSTDdev->StatusDev );

		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			epicsPrintf("\n");
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

