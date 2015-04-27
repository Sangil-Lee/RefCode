#include <fcntl.h>	// for mkdir

#include "sfwMDSplus.h"
#include "drvNI6254.h"
#include "kupaLib.h"

static long drvNI6254_io_report(int level);
static long drvNI6254_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNI6254 = {
	2,
	drvNI6254_io_report,
	drvNI6254_init_driver
};

epicsExportAddress(drvet, drvNI6254);

// sleep time of DAQ thread (milli seconds)
int kDAQSleepMillis	= 1000;
epicsExportAddress (int, kDAQSleepMillis);

int kAutoTrendRunWaitMillis	= 3000;
epicsExportAddress (int, kAutoTrendRunWaitMillis);

int n8After_Processing_Flag		= 0; 
int gbCalcRunMode  				= 0;	// 0 : Trend mode, 1 : Calcuration & store mode
int gbAutoTrendRunMode			= 1;	// 1 : Automatically start DAQ for trending

ST_User_Data	gUserData;
ST_IS_Setup		gIonSourceSetup[MAX_IS_NUM];

extern int proc_cal_alarm (char *pszTreeName, char *pszTreeIPport, int nShotNumber);

int NI_err_message(char *fname, int lineNo, int error)
{
	char	errBuff[2048]={'\0'};

	if ( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,sizeof(errBuff));
		kLog (K_ERR, "[%s:%d] DAQmx Error: %s\n", fname, lineNo, errBuff);
		notify_error (1, "DAQmx Error: %s\n", errBuff);

		epicsThreadSleep(1);
		return (NOK);
	}

	return (OK);
}

ST_User_Data *getUserData ()
{
	return (&gUserData);
}

void setUserSnapShot (int enable)
{
	getUserData()->snapShot = enable;

	char buf[64];
	sprintf (buf, "%d", enable);
	DBproc_put ("NB1_LODAQ_SNAP_SHOT", buf);
}

int getUserSnapShot ()
{
	return (getUserData()->snapShot);
}

void setUserCurrShot (int shot)
{
	getUserData()->currShot = shot;

	char buf[64];
	sprintf (buf, "%d", shot);
	DBproc_put ("NB1_LODAQ_CURR_SHOT", buf);
}

int getUserCurrShot ()
{
	return (getUserData()->currShot);
}

void lockMDS ()
{
	epicsMutexLock (get_master()->lock_mds);
	get_master()->n8MdsPutFlag = 1;
}

void unlockMDS ()
{
	get_master()->n8MdsPutFlag = 0;
	epicsMutexUnlock (get_master()->lock_mds);
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

int isFirstDevice (ST_STD_device *pSTDdev)
{
	return (0 == pSTDdev->BoardID);
}

ST_NI6254 *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NI6254 *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NI6254 *pNI6254) 
{ 
	return (pNI6254->sample_rate); 
}

int getArraySizeInBytes (ST_NI6254 *pNI6254) 
{
   	return (sizeof(pNI6254->readArray)); 
}

int getMaxNumChan (ST_NI6254 *pNI6254) 
{ 
	return (INIT_MAX_CHAN_NUM); 
}

int getTotSampsChan (ST_NI6254 *pNI6254) 
{ 
	return (pNI6254->sample_rate * pNI6254->sample_time); 
}

int getTotSampsAllChan (ST_NI6254 *pNI6254) 
{ 
	return (pNI6254->sample_rate * getMaxNumChan(pNI6254)); 
}

int getCallbackRate (ST_NI6254 *pNI6254) 
{ 
	return (pNI6254->callbackRate); 
}

int getCbNumSampsPerChan (ST_NI6254 *pNI6254) 
{ 
	return (getNumSampsPerChan (pNI6254) / getCallbackRate (pNI6254) );
}

float64 getSampRate (ST_NI6254 *pNI6254) 
{ 
	return (pNI6254->sample_rate); 
}

int getSampTime (ST_NI6254 *pNI6254) 
{ 
	return (pNI6254->sample_time); 
}

void setSamplTime (ST_NI6254 *pNI6254) 
{
	pNI6254->sample_time = (pNI6254->stop_t1 <= pNI6254->start_t0) ? 0 : (pNI6254->stop_t1 - pNI6254->start_t0);

	kLog (K_INFO, "[setSamplTime] start_t0(%.f) stop_t1(%.f) sample_time(%f)\n",
			pNI6254->start_t0, pNI6254->stop_t1, pNI6254->sample_time);
}

void setStartT0 (ST_NI6254 *pNI6254, float64 val)
{
	pNI6254->start_t0 = val;
	setSamplTime (pNI6254);
}

void setStopT1 (ST_NI6254 *pNI6254, float64 val)
{
	pNI6254->stop_t1 = val;
	setSamplTime (pNI6254);
}

float64 getBeamPulse (int nIonSourceNum)
{
	return (gIonSourceSetup[nIonSourceNum].beam_pulse);
}

float64 getIntegralStarttime (int nIonSourceNum)
{
	return (gIonSourceSetup[nIonSourceNum].integral_stime);
}

void armingDeviceParams (ST_NI6254 *pNI6254)
{
	int		i;
	int		nIonSourceNum;
	float64	sampRate	= getSampRate (pNI6254);

	pNI6254->totalRead	= 0;

	for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
		nIonSourceNum	= pNI6254->ST_Ch[i].ionSourceNum;

		if (TRUE == isCalcRunMode ()) {
			pNI6254->ST_Ch[i].power		= 0;
			pNI6254->ST_Ch[i].flow		= 0;
		}

		pNI6254->ST_Ch[i].offset		= 0;
		pNI6254->ST_Ch[i].sample_cnt	= 0;	// total archived count
		pNI6254->ST_Ch[i].accum_temp	= 0;
		pNI6254->ST_Ch[i].accum_flow	= 0;
		pNI6254->ST_Ch[i].accum_cnt		= 0;
		pNI6254->ST_Ch[i].integral_stime= getIntegralStarttime (nIonSourceNum);
		pNI6254->ST_Ch[i].integral_ecnt	= pNI6254->ST_Ch[i].integral_time * sampRate;
		pNI6254->ST_Ch[i].integral_scnt	= pNI6254->ST_Ch[i].integral_stime * sampRate;
		pNI6254->ST_Ch[i].beam_pulse	= getBeamPulse (nIonSourceNum);

		kLog (K_DEBUG, "[armingDeviceParams] %s : ch(%d) is(%d) beam_pulse(%f) scnt(%.f) ecnt(%.f) \n",
				pNI6254->ST_Ch[i].physical_channel, i,
				nIonSourceNum, 
				pNI6254->ST_Ch[i].beam_pulse,
				pNI6254->ST_Ch[i].integral_scnt,
				pNI6254->ST_Ch[i].integral_ecnt);
	}
}

int processInputData (ST_STD_device *pSTDdev, int bCalcRunMode, ST_NI6254 *pNI6254, int chNo, intype rawValue)
{
	ST_NI6254_channel *pChInfo	= &pNI6254->ST_Ch[chNo];
	float64	dValue;

	// raw input value
	pChInfo->rawValue	= rawValue;

	// eu conversion
	// -----------------------------------------------
	// EU(V)  = Raw * (EU_H - EU_L) / (Raw_H - Raw_L)
	// EU(.C) = EU(V) * NBI_TEMP_PER_VOLT
	// -----------------------------------------------
	pChInfo->euVolt		= rawValue * EU_GRADIENT;
	pChInfo->euValue	= NBI_TEMP_PER_VOLT * pChInfo->euVolt;

	if (getLogChNo() == chNo) {
		kLog (K_ERR, "[processInputData] %s : chNo(%d) raw(%d) volt(%.2f) degC(%.2f)\n",
				pSTDdev->taskName, chNo, pChInfo->rawValue, pChInfo->euVolt, pChInfo->euValue);
	}
	else {
		kLog (K_DEBUG, "[processInputData] %s : chNo(%d) raw(%d) volt(%.2f) degC(%.2f)\n",
				pSTDdev->taskName, chNo, pChInfo->rawValue, pChInfo->euVolt, pChInfo->euValue);
	}

	if (TRUE != bCalcRunMode) {
		return (0);
	}

	if (NBI_INPUT_DELTA_T == pChInfo->inputType) {
		if (pChInfo->integral_ecnt > pChInfo->sample_cnt) {
			if (pChInfo->integral_scnt <= pChInfo->sample_cnt) {
				// accumulate of Delta-T
				pChInfo->accum_temp	+= pChInfo->euValue / getSampRate(pNI6254);

				// accumulate of flow
				dValue				= kupaGetValue(pChInfo->flowPVName);
				pChInfo->accum_flow	+= dValue;
				pChInfo->flow		= dValue;

				if (getLogChNo() == chNo) {
					kLog (K_ERR, "[processInputData] %s : offset(%d) rawValue(%d) eu(%.2f) accum_flow(%.5f) accum(temp:%.2f, cnt:%.f), samples(%.f)\n",
							pSTDdev->taskName,
							pChInfo->offset, pChInfo->rawValue,
							pChInfo->euValue, pChInfo->accum_flow, pChInfo->accum_temp, pChInfo->accum_cnt, pChInfo->sample_cnt);
				}
				else {
					kLog (K_DEBUG, "[processInputData] %s : offset(%d) rawValue(%d) eu(%.2f) power(%.2f) accum(temp:%.2f, cnt:%.f)\n",
							pSTDdev->taskName,
							pChInfo->offset, pChInfo->rawValue,
							pChInfo->euValue, pChInfo->power, pChInfo->accum_temp, pChInfo->accum_cnt);
				}

				// increase accumulate counter to check the end time for integral
				pChInfo->accum_cnt++;
			}
		}

		pChInfo->sample_cnt++;
	}

	return (0);
}

// calcurate beam power after shot
void calcBeamPower (ST_STD_device *pSTDdev, ST_NI6254 *pNI6254, int chNo)
{
	ST_NI6254_channel *pChInfo	= &pNI6254->ST_Ch[chNo];

	if (NBI_INPUT_DELTA_T == pChInfo->inputType) {
		// average of flow
		pChInfo->flow	= pChInfo->accum_flow / pChInfo->accum_cnt;

		// power
		pChInfo->power	= pChInfo->accum_temp * pChInfo->flow * NBI_POWER_CONST / pChInfo->beam_pulse;

		if (getLogChNo() == chNo) {
			kLog (K_ERR, "[calcBeamPower] %s : F(%.2f) = accum_flow(%.2f) / accum_cnt(%.f)\n",
					pSTDdev->taskName, pChInfo->flow, pChInfo->accum_flow, pChInfo->accum_cnt);

			kLog (K_ERR, "[calcBeamPower] %s : P(%.2f) = accum_temp(%.2f) * avg_flow(%.2f) * %f / beam_pulse(%.2f)\n",
					pSTDdev->taskName, pChInfo->power, pChInfo->accum_temp, pChInfo->flow,
					NBI_POWER_CONST, pChInfo->beam_pulse);
		}
	}
}

float64 getStartTime (ST_STD_device *pSTDdev)
{
	float64	dStartTime = 0;

	if (pSTDdev->ST_Base.opMode == OPMODE_REMOTE) {
		dStartTime  = (pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime);
	}
	else {
		dStartTime  = pSTDdev->ST_Base_fetch.dT0[0];
	}

	return (dStartTime);
}

float64 getEndTime (ST_STD_device *pSTDdev)
{
	return (pSTDdev->ST_Base_fetch.dT1[0] - pSTDdev->ST_Base_fetch.dT0[0]);
}

int isSamplingStop (ST_STD_device *pSTDdev)
{
	ST_NI6254	*pNI6254 = getDriver(pSTDdev);

	return ( isCalcRunMode() ? (pNI6254->totalRead >= getTotSampsChan(pNI6254)) : FALSE );
}

int isTriggerRequiredMode (const int mode)
{
	switch (mode) {
		case OPMODE_REMOTE :
		case OPMODE_LOCAL :
			return (TRUE);
	}

	return (FALSE);
}

int isTriggerRequired (ST_STD_device *pSTDdev)
{
	return ( isTriggerRequiredMode(pSTDdev->ST_Base.opMode) && isCalcRunMode() );
}

int isAutoRunMode (ST_STD_device *pSTDdev)
{
	return ( (OPMODE_REMOTE != pSTDdev->ST_Base.opMode) || !isCalcRunMode() );
}

int isRemoteMode ()
{
	return (OPMODE_REMOTE == get_master()->ST_Base.opMode);
}

int isRemoteRunning ()
{
	ST_MASTER *pMaster = get_master();

	return ( isCalcRunMode() &&
			(pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
			( (pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ||
			  (pMaster->StatusAdmin & TASK_IN_PROGRESS) ||
			  (pMaster->StatusAdmin & TASK_POST_PROCESS) ||
			  (pMaster->StatusAdmin & TASK_DATA_TRANSFER) ) );
}

void make_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);

	ST_NI6254 *pNI6254 = pSTDdev->pUser;
	int i;
	char mkdirbuf[256];

	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6254->ST_Ch[i].write_fp) {
			fclose (pNI6254->ST_Ch[i].write_fp);
		}

		pNI6254->ST_Ch[i].write_fp = NULL;

		if (CH_USED == pNI6254->ST_Ch[i].used) {
			sprintf (pNI6254->ST_Ch[i].path_name,"%sS%06d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6254->ST_Ch[i].file_name);

			kLog (K_INFO, "[make_local_file] fname(%s)\n", pNI6254->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pNI6254->ST_Ch[i].write_fp = fopen(pNI6254->ST_Ch[i].path_name, "w+"))) {
				kLog (K_ERR, "[make_local_file] %s create failed!!!\n", pNI6254->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6254->ST_Ch[i].path_name);
			}
		}
	}
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6254 *pNI6254 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6254->ST_Ch[i].write_fp) {
			fflush(pNI6254->ST_Ch[i].write_fp);
		}
	}
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6254 *pNI6254 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6254->ST_Ch[i].write_fp) {
			fclose(pNI6254->ST_Ch[i].write_fp);
			pNI6254->ST_Ch[i].write_fp	= NULL;
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

#if 0
	if (WR_OK == mds_Connect (pSTDdev)) {
		mds_createNewShot (pSTDdev);
		mds_Disconnect (pSTDdev);

		return (WR_OK);
	}

	return (WR_ERROR);
#else
	// local tree is created by NB1_DAQ

	kLog (K_MON, "[createLocalMdsTree] %s : local tree is created by NB1_DAQ\n", pSTDdev->taskName);

	return (WR_OK);
#endif
}

int stopDevice (ST_STD_device *pSTDdev)
{
	ST_NI6254 *pNI6254 = pSTDdev->pUser;

	if (0 != pNI6254->taskHandle) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNI6254->totalRead);

		DAQmxTaskControl(pNI6254->taskHandle, DAQmx_Val_Task_Abort);
		NIERROR(DAQmxStopTask(pNI6254->taskHandle));

		if (pNI6254->auto_run_flag == 1) {
			kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
			pNI6254->auto_run_flag = 0;

			// wait some times to synchronize running mode with device threads
			epicsThreadSleep (0.5);
		}
	}

	return (0);
}

int resetDevice (ST_STD_device *pSTDdev)
{
	close_local_file (pSTDdev);

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
	sprintf (pMaster->ErrorString, "Normal");

	return (0);
}

void armingDevice (ST_STD_device* pSTDdev)
{
	ST_MASTER *pMaster	= get_master();

	kLog (K_MON, "[armingDevice] %s : mode(%d)\n", pSTDdev->taskName, isCalcRunMode());

	// clear error message
	sprintf (pMaster->ErrorString, "Normal");

	// reset the running task
	stopDevice (pSTDdev);

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_ARM_ENABLED;

	pSTDdev->ST_Base.dT0[0]			= pMaster->ST_Base.dT0[0];
	pSTDdev->ST_Base_fetch.dT0[0]	= pMaster->ST_Base.dT0[0];

	pSTDdev->ST_Base.dT1[0]			= pMaster->ST_Base.dT1[0];
	pSTDdev->ST_Base_fetch.dT1[0]	= pMaster->ST_Base.dT1[0];

	kLog (K_MON, "[armingDevice] T0/T1 : Dev(%s) Master(%.f/%.f) Dev(%.f/%.f) fetch(%.f/%.f)\n",
			pSTDdev->taskName,
			pMaster->ST_Base.dT0[0], pMaster->ST_Base.dT1[0], 
			pSTDdev->ST_Base.dT0[0], pSTDdev->ST_Base.dT1[0], 
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0]);

	if (TRUE == isCalcRunMode()) {
		flush_STDdev_to_MDSfetch (pSTDdev);

		make_local_file (pSTDdev);

#if AUTO_CREATE_TREE
		createLocalMdsTree (pSTDdev);
#endif
	}

	setUserCurrShot (pMaster->ST_Base.shotNumber);
}

int processTrendRun ()
{
	setAutoTrendRunMode (FALSE);

	resetDeviceAll ();

	setCalcRunMode (FALSE);

	// start DAQ for trending
	DBproc_put ("NB1_LODAQ_CALC_RUN", "0");		// reset button
	DBproc_put ("NB1_LODAQ_SYS_ARMING", "1");	// arming

	return (OK);
}

int processCalcRun ()
{
	setAutoTrendRunMode (FALSE);

	resetDeviceAll ();

	setCalcRunMode (TRUE);

	// start DAQ for calcuration and storing
	DBproc_put ("NB1_LODAQ_TREND_RUN", "0");	// reset button

	if (get_master()->ST_Base.opMode != OPMODE_REMOTE) {
		DBproc_put ("NB1_LODAQ_SYS_ARMING", "1");	// arming
	}

	return (OK);
}

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

// makes name of tag for raw value
int drvNI6254_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	ST_NI6254 *pNI6254 = (ST_NI6254 *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvNI6254_set_TagName] ...\n");

	for(i=0; i<INIT_MAX_CHAN_NUM; i++) {
		sprintf(pNI6254->ST_Ch[i].strTagName, "\\%s:FOO", pNI6254->ST_Ch[i].inputTagName);
		kLog (K_INFO, "%s\n", pNI6254->ST_Ch[i].strTagName);
	}

	return WR_OK;
}

// starts DAQ for trending
void func_NI6254_POST_SEQSTOP(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isRemoteMode ()) {
		return;
	}

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
void func_NI6254_PRE_SEQSTART(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isRemoteMode ()) {
		return;
	}

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[PRE_SEQSTART] %s : setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		processCalcRun ();
	}
}

void func_NI6254_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_RESET] %s: setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		resetDevice (pSTDdev);

		admin_all_taskStatus_reset();

		scanIoRequest(pMaster->ioScanPvt_status);

		DBproc_put("NB1_LODAQ_RESET", "0");
	}
}

void func_NI6254_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);

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
			return;
		}

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6254_SYS_RUN(void *pArg, double arg1, double arg2)
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

void func_NI6254_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[OP_MODE] %s : %f, %f\n", pSTDdev->taskName, arg1, arg2);

#if 1
	if (pMaster->ST_Base.opMode == OPMODE_INIT) {
		kLog (K_ERR, "[OP_MODE] %s : force to LOCAL mode\n", pSTDdev->taskName);

		pMaster->ST_Base.opMode	= OPMODE_LOCAL;
		pSTDdev->ST_Base.opMode	= OPMODE_LOCAL;
	}
#endif

	if (pMaster->ST_Base.opMode != OPMODE_INIT) {
		pSTDdev->StatusDev |= TASK_STANDBY;

		resetDeviceAll ();

		if (OPMODE_LOCAL == pMaster->ST_Base.opMode) {
			updateShotNumber ();
		}

		if ( (pSTDdev->ST_Base.opMode != pMaster->ST_Base.opMode) && (OPMODE_REMOTE == pMaster->ST_Base.opMode) ) {
			setAutoTrendRunMode (TRUE);
		}
	}

	mds_copy_master_to_STD(pSTDdev);

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6254_DATA_SEND(void *pArg, double arg1, double arg2)
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

void func_NI6254_CREATE_LOCAL_TREE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_TREE] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}

void func_NI6254_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NI6254_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6254 *pNI6254 = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNI6254 = pSTDdev->pUser;
	pNI6254->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
}

void func_NI6254_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6254 *pNI6254 = NULL;

	kLog (K_MON, "[func_NI6254_T0] %s : start_t0(%.f) \n", pSTDdev->taskName, arg1);

	pNI6254 = pSTDdev->pUser;

	setStartT0 (pNI6254, arg1);
}

void func_NI6254_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6254 *pNI6254 = NULL;

	kLog (K_MON, "[func_NI6254_T1] %s : stop_t1(%.f) \n", pSTDdev->taskName, arg1);

	pNI6254 = pSTDdev->pUser;

	setStopT1 (pNI6254, arg1);
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	ST_STD_device	*pSTDdev = (ST_STD_device*) callbackData;
	ST_NI6254		*pNI6254 = pSTDdev->pUser;

	int 	i;
	int32	sampsRead=0;
	int32	maxNumChan = getMaxNumChan(pNI6254);
	int32	numSampsPerChan = nSamples;
	int32	totSampsAllChan	= numSampsPerChan * maxNumChan;
	int		ret;

#if !USE_SCALING_VALUE
	int32	numBytesPerSamp = 0;
	int32 	arraySizeInBytes = getArraySizeInBytes(pNI6254);
#endif

	if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

		kLog (K_MON, "[EveryNCallback] %s : samples(%d) rate(%.0f) total(%d) shot(%lu/%lu)\n",
				pSTDdev->taskName, nSamples, pNI6254->sample_rate, pNI6254->totalRead,
				get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		kLog (K_INFO, "[EveryNCallback] %s : status(0x%x:%s)\n",
				pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

		notify_refresh_master_status();
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	else {
		kLog (K_INFO, "[EveryNCallback] %s : samples(%d) rate(%.0f) total(%d) shot(%lu/%lu)\n",
				pSTDdev->taskName, nSamples, pNI6254->sample_rate, pNI6254->totalRead,
				get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);
	}

#if USE_SCALING_VALUE
	ret = NIERROR(
		DAQmxReadAnalogF64 (
			pNI6254->taskHandle, 	// taskHandle
			numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
			600.0, 					// timeout (seconds)
			DAQmx_Val_GroupByScanNumber,
			pNI6254->fScaleReadArray, 	// readAray. array into which samples are read
			sizeof(pNI6254->fScaleReadArray), 		// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 	// sampsRead. actual number of bytes read into the array per scan
			NULL					// reserved
		)
	);

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%.2f ", pNI6254->fScaleReadArray[i*maxNumChan]);
		}
		printf ("\n");
	}

	// converts scaling eu value to raw value of 16bits resolution
	for (i=0;i<totSampsAllChan;i++) {
		pNI6254->readArray[i] = GET_RAW_VALUE(pNI6254->fScaleReadArray[i]);
	}
#else
	ret = NIERROR(
		DAQmxReadRaw (
			pNI6254->taskHandle, 	// taskHandle
			numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
			600.0, 					// timeout (seconds)
			pNI6254->readArray, 	// readAray. array into which samples are read
			arraySizeInBytes, 		// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 	// sampsRead. actual number of bytes read into the array per scan
			&numBytesPerSamp, 		// numBytesPerSamp
			NULL					// reserved
		)
	);
#endif

	if (OK != ret) {
		stopDevice (pSTDdev);
		return 1;
	}

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%d ", pNI6254->readArray[i*maxNumChan]);
		}
		printf ("\n");
	}

	if (TRUE == isCalcRunMode ()) {
		if (0 >= pNI6254->totalRead) {
			// save the first offset to compensate
			for(i = 0; i<INIT_MAX_CHAN_NUM; i++) {
				if (NBI_INPUT_DELTA_T == pNI6254->ST_Ch[i].inputType) {
					pNI6254->ST_Ch[i].offset	= pNI6254->readArray[i];

					kLog (K_DEL, "[EveryNCallback] %s : ch(%d) offset(%d) eu(%.5f)\n", pSTDdev->taskName, i,
							pNI6254->readArray[i], pNI6254->fScaleReadArray[i]);
				}
			}
		}

		pNI6254->totalRead	+= sampsRead;
	}

	FILE *fp = NULL;
	int chNo;

	for (i=0;i<totSampsAllChan;i++) {
		chNo = GET_CHID(i);

		// offset compensation
		pNI6254->readArray[i]	-= pNI6254->ST_Ch[chNo].offset;

		if ( (NULL != (fp = pNI6254->ST_Ch[chNo].write_fp)) && (TRUE == isCalcRunMode ()) ) {
			// write to file
			fwrite(&pNI6254->readArray[i], sizeof(pNI6254->readArray[0]), 1, pNI6254->ST_Ch[chNo].write_fp);
			fflush(pNI6254->ST_Ch[chNo].write_fp);

			if (0 == chNo) {
				kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
						pSTDdev->taskName, chNo, pNI6254->ST_Ch[chNo].file_name, pNI6254->readArray[i]);
			}
		}

		// process input data : offset compensation, eu conversion, integral
		processInputData (pSTDdev, isCalcRunMode(), pNI6254, chNo, pNI6254->readArray[i]);
	}

	if (TRUE == isSamplingStop (pSTDdev)) {
		for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
			calcBeamPower (pSTDdev, pNI6254, i);
		}
	}

	scanIoRequest(pSTDdev->ioScanPvt_userCall);

	if (TRUE == isSamplingStop (pSTDdev)) {
		kLog (K_INFO, "[EveryNCallback] %s : stop ....\n", pSTDdev->taskName);
		stopDevice (pSTDdev);
	}

	return 0;
}

void threadFunc_NI6254_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6254 *pNI6254 = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	long	nCurrShotNum = 0;
	int32	totSampsAllChan;
	int32	numSampsPerChan;

	kLog (K_TRACE, "[NI6254_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_NI6254_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNI6254->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_TRACE, "[NI6254_DAQ] %s : mode(%s) rate(%.f) time(%d) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6254->sample_rate, pNI6254->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_INFO, "[NI6254_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_DEBUG, "[NI6254_DAQ] %s : mode(%s) rate(%.f) time(%.f-%.f = %d) \n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6254->sample_rate, pNI6254->stop_t1, pNI6254->start_t0, pNI6254->sample_time);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNI6254->taskHandle == 0)) {
				processAutoTrendRun (pSTDdev);
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNI6254->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
				stopDevice (pSTDdev);

				//NIERROR(DAQmxResetDevice(pSTDdev->strArg0));
				NIERROR(DAQmxCreateTask("",&pNI6254->taskHandle));

				kLog (K_INFO, "[NI6254_DAQ] %s : minVal(%.f) maxVal(%.f) units(%d)\n",
						pSTDdev->taskName, pNI6254->minVal, pNI6254->maxVal, pNI6254->units);

				for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
					NIERROR (
						DAQmxCreateAIVoltageChan (
							pNI6254->taskHandle,				// taskHandle
							pNI6254->ST_Ch[i].physical_channel,	// physicalChannel
							"",									// nameToAssignToChannel
							//pNI6254->terminal_config,			// terminalConfig
							pNI6254->ST_Ch[i].termCfg,			// terminalConfig
							pNI6254->minVal,					// minVal
							pNI6254->maxVal,					// maxVal
							pNI6254->units,						// units
							NULL								// customScaleName
						)
					);

					kLog (K_INFO, "[NI6254_DAQ] DAQmxCreateAIVoltageChan : %s\n", pNI6254->ST_Ch[i].physical_channel);
				}

				totSampsAllChan	= getTotSampsAllChan(pNI6254);
				numSampsPerChan = getCbNumSampsPerChan(pNI6254);

				NIERROR(
					DAQmxCfgSampClkTiming(
						pNI6254->taskHandle,	// taskHandle
						"",						// source terminal of the Sample clock. NULL(internal)
						pNI6254->sample_rate,	// sampling rate in samples per second per channel
						DAQmx_Val_Rising,		// activeEdge. edge of the clock to acquire or generate samples
						pNI6254->smp_mode,		// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
						totSampsAllChan			// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
					)
				);

				kLog (K_INFO, "[NI6254_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%d) samples(%d) tot(%d)\n",
						pSTDdev->taskName,
						getModeMsg(pSTDdev->ST_Base.opMode),
						pNI6254->smp_mode, pNI6254->sample_rate, pNI6254->sample_time,
						numSampsPerChan, totSampsAllChan);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
						DAQmxCfgDigEdgeStartTrig(
							pNI6254->taskHandle,
							pNI6254->triggerSource,
							DAQmx_Val_Rising
						)
					);

					kLog (K_MON, "[NI6254_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pNI6254->triggerSource);
				}
				else {
					kLog (K_MON, "[NI6254_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}

				NIERROR(
					DAQmxRegisterEveryNSamplesEvent(
						pNI6254->taskHandle,			// taskHandle
						DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
						numSampsPerChan,				// nSamples. number of samples after whitch each event should occur
						0,								// options. 0(callback function is called in a DAQmx thread)
						EveryNCallback,					// callbackFunction
						(void *)pSTDdev					// callbackData. parameter to be passed to callback function
					)
				);

				armingDeviceParams (pNI6254);

				// during data acqusition, shot number is changed. so keep the current shot number to use later
				nCurrShotNum	= pMaster->ST_Base.shotNumber;

				NIERROR(DAQmxStartTask(pNI6254->taskHandle));

				kLog (K_MON, "[NI6254_DAQ] %s : Start Task ...\n", pSTDdev->taskName);

				kLog (K_INFO, "[threadFunc_NI6254_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNI6254->auto_run_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNI6254->auto_run_flag = 0;

				kLog (K_INFO, "[threadFunc_NI6254_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				if (isCalcRunMode()) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;

					DBproc_put("NB1_LODAQ_CALC_RUN", "0");
				}
				else {
					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					DBproc_put("NB1_LODAQ_TREND_RUN", "0");
				}

				kLog (K_INFO, "[threadFunc_NI6254_DAQ] before DAQmxClearTask for %s\n", pSTDdev->taskName);
				NIERROR(DAQmxClearTask(pNI6254->taskHandle));
				kLog (K_INFO, "[threadFunc_NI6254_DAQ] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
				pNI6254->taskHandle = 0;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_POST_PROCESS)) {
				flush_local_file(pSTDdev);								

				pSTDdev->StatusDev = TASK_STANDBY | TASK_DATA_TRANSFER;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_DATA_TRANSFER)) {
				kLog (K_MON, "[threadFunc_NI6254_DAQ] %s : Waiting lock for MdsPlus\n", pSTDdev->taskName);

				lockMDS ();

				kLog (K_MON, "[threadFunc_NI6254_DAQ] %s : Starting Mdsplus processing\n", pSTDdev->taskName);

				for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
					if (proc_sendData2Tree(pSTDdev) == WR_OK) {
						break;
					}

					epicsThreadSleep(1);
					kLog (K_INFO, "[threadFunc_NI6254_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
				}

				unlockMDS ();

				close_local_file (pSTDdev);

				pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

				// PV archiving
				if (TRUE == isFirstDevice(pSTDdev)) {
					kuDebug (kuMON, "[threadFunc_NI6254_DAQ] %s : kupaStart : %d\n", pSTDdev->taskName, nCurrShotNum);

					kupaSetShotNum (nCurrShotNum);

					kupaStart ();

					setUserSnapShot (1);
					epicsThreadSleep(1);
					setUserSnapShot (0);
				}

				notify_refresh_master_status();
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_NI6254_RingBuf(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6254_RingBuf] ...\n");
}

void threadFunc_NI6254_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6254_CatchEnd] ...\n");
}

int create_NI6254_taskConfig(ST_STD_device *pSTDdev)
{
	ST_NI6254 *pNI6254 = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;

	pNI6254 = (ST_NI6254*) calloc(1, sizeof(ST_NI6254));
	if(!pNI6254) return WR_ERROR;
	
	kLog (K_TRACE, "[create_NI6254_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pNI6254->taskHandle = 0x0;
	pNI6254->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNI6254->data_buf) {
		free (pNI6254);
		return WR_ERROR;
	}

	ellInit(pNI6254->data_buf);

	pNI6254->callbackRate	= 1;

	for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
		pNI6254->ST_Ch[i].write_fp	= NULL;
		pNI6254->ST_Ch[i].termCfg	= DAQmx_Val_RSE;

		sprintf (pNI6254->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
		sprintf (pNI6254->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);

		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			pNI6254->ST_Ch[i].used = CH_USED;

			strcpy (pNI6254->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			if (0 == strcmp (NBI_INPUT_TYPE_DT_STR, pSTDCh->strArg0)) {
				pNI6254->ST_Ch[i].inputType	= NBI_INPUT_DELTA_T;
				pNI6254->callbackRate		= DAQ_CALLBACK_RATE;

				strcpy (pNI6254->ST_Ch[i].flowPVName, pSTDCh->strArg3);
				pNI6254->ST_Ch[i].ionSourceNum = atoi(pSTDCh->strArg4);
			}
			else if (0 == strcmp (NBI_INPUT_TYPE_TC_STR, pSTDCh->strArg0)) {
				pNI6254->ST_Ch[i].inputType	= NBI_INPUT_TC;
			}
			else {
				pNI6254->ST_Ch[i].inputType	= NBI_INPUT_NONE;
			}

			if (0 == strcasecmp (NBI_TERM_CFG_DIFF_STR, pSTDCh->strArg2)) {
				pNI6254->ST_Ch[i].termCfg	= DAQmx_Val_Diff;
			}
			else if (0 == strcasecmp (NBI_TERM_CFG_NRSE_STR, pSTDCh->strArg2)) {
				pNI6254->ST_Ch[i].termCfg	= DAQmx_Val_NRSE;
			}
			else {
				pNI6254->ST_Ch[i].termCfg	= DAQmx_Val_RSE;
			}

			kLog (K_INFO, "[create_NI6254_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) term(%d) fname(%s) IS(%d)\n",
					pSTDdev->strArg0, pNI6254->ST_Ch[i].physical_channel,
					pNI6254->ST_Ch[i].inputType, pNI6254->ST_Ch[i].inputTagName,
					pNI6254->ST_Ch[i].termCfg, pNI6254->ST_Ch[i].file_name, pNI6254->ST_Ch[i].ionSourceNum);
		}
	}

	sprintf (pNI6254->triggerSource, "%s", pSTDdev->strArg1); 

	//pNI6254->terminal_config	= DAQmx_Val_RSE;	// DAQmx_Val_Diff, DAQmx_Val_NRSE
	pNI6254->terminal_config	= DAQmx_Val_Diff;	// DAQmx_Val_Diff, DAQmx_Val_NRSE
	pNI6254->minVal				= INIT_LOW_LIMIT;
	pNI6254->maxVal				= INIT_HIGH_LIMIT;
	pNI6254->sample_rate		= INIT_SAMPLE_RATE;
	pNI6254->sample_time		= getSampTime(pNI6254);
	pNI6254->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNI6254->units				= DAQmx_Val_Volts;

	pSTDdev->pUser	= pNI6254;

	pSTDdev->ST_Func._OP_MODE			= func_NI6254_OP_MODE;
	pSTDdev->ST_Func._CREATE_LOCAL_TREE	= func_NI6254_CREATE_LOCAL_TREE;
	pSTDdev->ST_Func._SYS_ARMING		= func_NI6254_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN			= func_NI6254_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND			= func_NI6254_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET			= func_NI6254_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0			= func_NI6254_T0;
	pSTDdev->ST_Func._SYS_T1			= func_NI6254_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NI6254_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NI6254_SHOT_NUMBER;

	pSTDdev->ST_Func._POST_SEQSTOP		= func_NI6254_POST_SEQSTOP;
	pSTDdev->ST_Func._PRE_SEQSTART		= func_NI6254_PRE_SEQSTART;

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NI6254_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 0
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6254_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6254_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvNI6254_init_driver(void)
{
	kLog (K_TRACE, "[drvNI6254_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_NI6254_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvNI6254_init_driver] %s : create_NI6254_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvNI6254_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNI6254_io_report(int level)
{
	kLog (K_TRACE, "[drvNI6254_io_report] ...\n");

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

