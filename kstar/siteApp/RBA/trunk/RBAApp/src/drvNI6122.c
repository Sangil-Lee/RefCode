#include <fcntl.h>	// for mkdir

#include "drvNI6122.h"

extern int resetDeviceAmp (ST_STD_device *pSTDdev);
extern int startDeviceAmp (ST_STD_device *pSTDdev);

static long drvNI6122_io_report(int level);
static long drvNI6122_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNI6122 = {
	2,
	drvNI6122_io_report,
	drvNI6122_init_driver
};

epicsExportAddress(drvet, drvNI6122);

// sleep time of DAQ thread (milli seconds)
int kDAQSleepMillis	= 1000;
epicsExportAddress (int, kDAQSleepMillis);

int kAutoTrendRunWaitMillis	= 3000;
epicsExportAddress (int, kAutoTrendRunWaitMillis);

int n8After_Processing_Flag		= 0; 

int NI_err_message(char *fname, int lineNo, int error)
{
	char	errBuff[2048]={'\0'};

	if ( DAQmxFailed(error) ) {
		set_ioc_dig_stat (DIG_ERROR);

		DAQmxGetExtendedErrorInfo(errBuff,sizeof(errBuff));
		kLog (K_ERR, "[%s:%d] DAQmx Error: %s\n", fname, lineNo, errBuff);
		notify_error (1, "DAQmx Error: %s\n", errBuff);

		epicsThreadSleep(1);
		return (NOK);
	}

	return (OK);
}

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

int isFirstDevice (ST_STD_device *pSTDdev)
{
	return (0 == pSTDdev->BoardID);
}

ST_NI6122 *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NI6122 *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NI6122 *pNI6122) 
{ 
	return (pNI6122->sample_rate); 
}

int getArraySizeInBytes (ST_NI6122 *pNI6122) 
{
   	return (sizeof(pNI6122->readArray)); 
}

int getMaxNumChan (ST_NI6122 *pNI6122) 
{ 
	return (INIT_MAX_CHAN_NUM); 
}

float64 getTotSampsChan (ST_NI6122 *pNI6122) 
{ 
	return (pNI6122->sample_rate * pNI6122->sample_time); 
}

int getTotSampsAllChan (ST_NI6122 *pNI6122) 
{ 
	return (pNI6122->sample_rate * getMaxNumChan(pNI6122)); 
}

int getCallbackRate (ST_NI6122 *pNI6122)
{
	return (pNI6122->callbackRate);
}

int getCbNumSampsPerChan (ST_NI6122 *pNI6122)
{
	return (getNumSampsPerChan (pNI6122) / getCallbackRate (pNI6122) );
}

int getSampRate (ST_NI6122 *pNI6122) 
{ 
	return (pNI6122->sample_rate); 
}

float64 getSampTime (ST_NI6122 *pNI6122) 
{ 
	return (pNI6122->sample_time); 
}

void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_NI6122 *pNI6122	= pSTDdev->pUser;

	if (pNI6122->stop_t1 <= pNI6122->start_t0) {
		pNI6122->sample_time = 0;
	}
	else {
		pNI6122->sample_time = pNI6122->stop_t1 - pNI6122->start_t0;
	}

	kLog (K_INFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pNI6122->start_t0, pNI6122->stop_t1, pNI6122->sample_time);

	scanIoRequest (pSTDdev->ioScanPvt_userCall);
}

void setStartT0 (ST_STD_device *pSTDdev, float64 val) 
{
	ST_NI6122 *pNI6122	= pSTDdev->pUser;

	pNI6122->start_t0 = val;

	setSamplTime (pSTDdev);
}

void setStopT1 (ST_STD_device *pSTDdev, float64 val)
{
	ST_NI6122 *pNI6122	= pSTDdev->pUser;

	pNI6122->stop_t1 = val;

	setSamplTime (pSTDdev);
}

float64 getRawGradient (ST_NI6122 *pNI6122)
{
	//return ( I16_FULL_SCALE / (pNI6122->maxVal - pNI6122->minVal) );
	return (RAW_GRADIENT);
}

float64 getEuGradient (ST_NI6122 *pNI6122)
{
	//return ( (pNI6122->maxVal - pNI6122->minVal) / I16_FULL_SCALE );
	return (EU_GRADIENT);
}

float64 getRawValue (ST_NI6122 *pNI6122, float64 euValue)
{
	return ( euValue * pNI6122->rawGradient );
}

float64 getEuValue (ST_NI6122 *pNI6122, float64 rawValue)
{
	return ( rawValue * pNI6122->euGradient );
}

void armingDeviceParams (ST_NI6122 *pNI6122)
{
	pNI6122->totalRead		= 0;
	pNI6122->offset			= 0;
	pNI6122->rawGradient	= getRawGradient (pNI6122);
	pNI6122->euGradient		= getEuGradient (pNI6122);

#if 0
	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
	}
#endif
}

float64 getStartTime (ST_STD_device *pSTDdev)
{
	float64	fStartTime = 0;

	if (pSTDdev->ST_Base.opMode == OPMODE_REMOTE) {
		fStartTime  = pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime;
	}
	else {
		fStartTime  = pSTDdev->ST_Base_fetch.dT0[0];
	}

	return (fStartTime);
}

float64 getEndTime (ST_STD_device *pSTDdev)
{
	return (pSTDdev->ST_Base_fetch.dT1[0] - pSTDdev->ST_Base_fetch.dT0[0]);
}

int isSamplingStop (ST_STD_device *pSTDdev)
{
	ST_NI6122	*pNI6122 = getDriver(pSTDdev);

	if (TRUE != isStoreMode ()) {
		return (FALSE);
	}

	return ( pNI6122->totalRead >= getTotSampsChan(pNI6122)  );
}

int isTriggerRequiredMode (const int mode)
{
	switch (mode) {
		case OPMODE_REMOTE :
		//TODO
		case OPMODE_LOCAL :
			return (TRUE);
	}

	return (FALSE);
}

int isTriggerRequired (ST_STD_device *pSTDdev)
{
	return ( isTriggerRequiredMode(pSTDdev->ST_Base.opMode) );
}

int isRemoteRunning ()
{
	ST_MASTER *pMaster = get_master();

	return ( (pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
			( (pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ||
			  (pMaster->StatusAdmin & TASK_IN_PROGRESS) ||
			  (pMaster->StatusAdmin & TASK_POST_PROCESS) ||
			  (pMaster->StatusAdmin & TASK_DATA_TRANSFER) ) );
}

void make_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);

	ST_NI6122 *pNI6122 = pSTDdev->pUser;
	int i;
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6122->ST_Ch[i].write_fp) {
			fclose (pNI6122->ST_Ch[i].write_fp);
		}

		pNI6122->ST_Ch[i].write_fp = NULL;

		if ( (CH_USED == pNI6122->ST_Ch[i].used) && (CH_USED == pNI6122->ST_Ch[i].enabled) ) {
			sprintf (pNI6122->ST_Ch[i].path_name,"%sS%06d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6122->ST_Ch[i].file_name);

			kLog (K_INFO, "[make_local_file] fname(%s)\n", pNI6122->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pNI6122->ST_Ch[i].write_fp = fopen(pNI6122->ST_Ch[i].path_name, "w+"))) {
				kLog (K_ERR, "[make_local_file] %s create failed!!!\n", pNI6122->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6122->ST_Ch[i].path_name);
			}
		}
	}
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6122 *pNI6122 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6122->ST_Ch[i].write_fp) {
			fflush(pNI6122->ST_Ch[i].write_fp);
		}
	}
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6122 *pNI6122 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6122->ST_Ch[i].write_fp) {
			fclose(pNI6122->ST_Ch[i].write_fp);
			pNI6122->ST_Ch[i].write_fp	= NULL;
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

	if (WR_OK == mds_Connect (pSTDdev)) {
		mds_createNewShot (pSTDdev);
		mds_Disconnect (pSTDdev);

		return (WR_OK);
	}

	return (WR_ERROR);
}

int stopDevice (ST_STD_device *pSTDdev)
{
	ST_NI6122 *pNI6122 = pSTDdev->pUser;

	if (0 != pNI6122->taskHandle) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNI6122->totalRead);

		DAQmxTaskControl(pNI6122->taskHandle, DAQmx_Val_Task_Abort);
		NIERROR(DAQmxStopTask(pNI6122->taskHandle));

		if (pNI6122->auto_run_flag == 1) {
			kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			// post event to threadFunc_NI6122_DAQ
			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
			pNI6122->auto_run_flag = 0;

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

	// reset amplifier : NI 488.2
	resetDeviceAmp (pSTDdev);

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

	set_ioc_amp_stat (AMP_RESET);
	set_ioc_dig_stat (DIG_RESET);
	set_ioc_mds_stat (MDS_NORMAL);

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

	kLog (K_MON, "[armingDevice] %s\n", pSTDdev->taskName);

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

	flush_STDdev_to_MDSfetch (pSTDdev);

	if (TRUE == isStoreMode ()) {
		make_local_file (pSTDdev);
	}

	// start amplifier : NI 488.2
	//TODO
	startDeviceAmp (pSTDdev);
}

// makes name of tag for raw value
int drvNI6122_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	ST_NI6122 *pNI6122 = (ST_NI6122 *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvNI6122_set_TagName] ...\n");

	for(i=0; i<INIT_MAX_CHAN_NUM; i++) {
		sprintf(pNI6122->ST_Ch[i].strTagName, "\\%s:FOO", pNI6122->ST_Ch[i].inputTagName);
		kLog (K_INFO, "%s\n", pNI6122->ST_Ch[i].strTagName);
	}

	return WR_OK;
}

void func_NI6122_POST_SEQSTOP(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[POST_SEQSTOP] %s : value(%d) status(%d)\n", pSTDdev->taskName, (int)arg1, (int)arg2);

	if (0 == (int)arg1) {
		if (TRUE != isRemoteRunning ()) {
			//processTrendRun ();
		}
		else {
			kLog (K_INFO, "[POST_SEQSTOP] %s : DAQ is running in remote mode\n", pSTDdev->taskName);
		}
	}
}

// stops DAQ to ready shot start
void func_NI6122_PRE_SEQSTART(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[PRE_SEQSTART] %s : setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		//processCalcRun ();
	}
}

void func_NI6122_SYS_RESET(void *pArg, double arg1, double arg2)
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

void func_NI6122_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) { /* in case of arming  */
		//if (admin_check_Arming_condition() == WR_ERROR) {
		if (check_dev_arming_condition(pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : arming failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		armingDevice (pSTDdev);
	}
	else { /* release arming condition */
		// reset amplifier : NI 488.2
		//TODO
		resetDeviceAmp (pSTDdev);

		if (admin_check_Release_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : release failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			//notify_error (1, "%s: release failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6122_SYS_RUN(void *pArg, double arg1, double arg2)
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

void func_NI6122_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[OP_MODE] %s : %f, %f\n", pSTDdev->taskName, arg1, arg2);

	if (pMaster->ST_Base.opMode != OPMODE_INIT) {
		pSTDdev->StatusDev |= TASK_STANDBY;

		//resetDeviceAll ();
		//TODO
		resetDevice (pSTDdev);

		if (OPMODE_LOCAL == pMaster->ST_Base.opMode) {
			updateShotNumber ();
		}
	}

	mds_copy_master_to_STD(pSTDdev);

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6122_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	kLog (K_MON, "[DATA_SEND] %s\n", pSTDdev->taskName);
}

void func_NI6122_CREATE_LOCAL_SHOT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[CREATE_LOCAL_SHOT] %s shot(%f)\n", pSTDdev->taskName, arg1);

	//createLocalMdsTree (pSTDdev);

#if 0
	char buf[64];
	sprintf (buf, "%ld", pSTDdev->ST_Base.shotNumber);
	DBproc_put("RBA_SHOT_NUMBER", buf);
#endif
}

void func_NI6122_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NI6122_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6122 *pNI6122 = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNI6122 = pSTDdev->pUser;
	pNI6122->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
}

void func_NI6122_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[func_NI6122_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	setStartT0 (pSTDdev, arg1);
}

void func_NI6122_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[func_NI6122_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	setStopT1 (pSTDdev, arg1);
}

void func_NI6122_EXIT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[func_NI6122_EXIT] %s \n", pSTDdev->taskName);
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	ST_STD_device	*pSTDdev = (ST_STD_device*) callbackData;
	ST_NI6122		*pNI6122 = pSTDdev->pUser;

	int 	i;
	int		chNo;
	int32	sampsRead		= 0;
	int32	maxNumChan		= getMaxNumChan(pNI6122);
	int32	numSampsPerChan = nSamples;
	int32	totSampsAllChan	= numSampsPerChan * maxNumChan;
	int		ret;

#if !USE_SCALING_VALUE
	int32	numBytesPerSamp = 0;
	int32 	arraySizeInBytes = getArraySizeInBytes(pNI6122);
#endif

	if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

		kLog (K_MON, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) shot(%lu/%lu)\n",
				pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6122->sample_rate,
				getStatusMsg(pSTDdev->StatusDev, NULL),
				pNI6122->totalRead, get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		kLog (K_INFO, "[EveryNCallback] %s : status(0x%x:%s)\n",
				pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

		notify_refresh_master_status();
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	else {
		kLog (K_INFO, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) shot(%lu/%lu)\n",
				pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6122->sample_rate,
				getStatusMsg(pSTDdev->StatusDev, NULL),
				pNI6122->totalRead, get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);
	}

#if USE_SCALING_VALUE
	ret = NIERROR(
		DAQmxReadAnalogF64 (
			pNI6122->taskHandle, 			// taskHandle
			numSampsPerChan, 				// numSampsPerChan. number of samples per channel to read
			600.0, 							// timeout (seconds)
			DAQmx_Val_GroupByChannel,		// DAQmx_Val_GroupByScanNumber (Group by scan number (interleaved), DAQmx_Val_GroupByChannel
			pNI6122->fScaleReadArray, 		// readAray. array into which samples are read
			sizeof(pNI6122->fScaleReadArray),	// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 			// sampsRead. actual number of bytes read into the array per scan
			NULL							// reserved
		)
	);

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%.2f ", pNI6122->fScaleReadArray[i]);
		}
		printf ("\n");
	}

	// converts scaling eu value to raw value of 16bits resolution
	for (i = 0; i < totSampsAllChan; i++) {
		pNI6122->fScaleReadArray[i]	= pNI6122->fScaleReadArray[i] * pNI6122->gain + pNI6122->offset;

		pNI6122->readArray[i] = getRawValue (pNI6122, pNI6122->fScaleReadArray[i]);
	}
#else
	ret = NIERROR(
		DAQmxReadRaw (
			pNI6122->taskHandle, 	// taskHandle
			numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
			600.0, 					// timeout (seconds)
			pNI6122->readArrayScan,	// readAray. array into which samples are read
			arraySizeInBytes, 		// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 	// sampsRead. actual number of bytes read into the array per scan
			&numBytesPerSamp, 		// numBytesPerSamp
			NULL					// reserved
		)
	);

	int	idx = 0;
	int offset;

	// align raw datas by channel
	for (chNo = 0; chNo < INIT_MAX_CHAN_NUM; chNo++) {
		for (i = 0; i < numSampsPerChan; i++) {
			offset		= i * maxNumChan + chNo;

			pNI6122->readArray[idx]			= pNI6122->readArrayScan[offset];
			pNI6122->fScaleReadArray[idx]	= getEuValue (pNI6122, pNI6122->readArrayScan[offset]);

			idx++;
		}
	}

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%d ", pNI6122->readArray[i]);
		}
		printf ("\n");
	}
#endif

	if (OK != ret) {
		stopDevice (pSTDdev);
		return 1;
	}

	if (TRUE == isStoreMode ()) {
		pNI6122->totalRead	+= sampsRead;
	}

	set_ioc_dig_stat (DIG_ACQUIRING);

	FILE *fp = NULL;
	int32 arrayJump;
	int32 arrayJumpForLastSample;

    // DAQmx_Val_GroupByChannel (non-interleaved) at DAQmxReadAnalogF64
	for (chNo = 0; chNo < INIT_MAX_CHAN_NUM; chNo++) {
		arrayJump = chNo * numSampsPerChan;
		arrayJumpForLastSample	= arrayJump + numSampsPerChan - 1;

		if (NULL != (fp = pNI6122->ST_Ch[chNo].write_fp)) {
			fwrite(&pNI6122->readArray[arrayJump], sizeof(pNI6122->readArray[0]), numSampsPerChan, pNI6122->ST_Ch[chNo].write_fp);
			fflush(pNI6122->ST_Ch[chNo].write_fp);
		}

		pNI6122->ST_Ch[chNo].rawValue	= pNI6122->readArray[arrayJumpForLastSample];
		pNI6122->ST_Ch[chNo].euValue	= pNI6122->fScaleReadArray[arrayJumpForLastSample];

		for (i = 0; i < numSampsPerChan; i++) {
			if (getLogChNo() == chNo) {
				kLog (K_MON, "[EveryNCallback] %s : chNo(%d) raw(%d) volt(%.4f) \n",
						pSTDdev->taskName, chNo, pNI6122->readArray[arrayJump+i], pNI6122->fScaleReadArray[arrayJump+i]);
			}
			else if (TRUE == isPrintDebugMsg (K_DEBUG)) {
				kLog (K_DEBUG, "[EveryNCallback] %s : chNo(%d) raw(%d) volt(%.4f) \n",
						pSTDdev->taskName, chNo, pNI6122->readArray[arrayJump+i], pNI6122->fScaleReadArray[arrayJump+i]);
			}
		}
	}
	
	scanIoRequest(pSTDdev->ioScanPvt_userCall);

	if (TRUE == isSamplingStop (pSTDdev)) {
		kLog (K_INFO, "[EveryNCallback] %s : total(%d) : stop ....\n", pSTDdev->taskName, pNI6122->totalRead);
		stopDevice (pSTDdev);
	}

	return 0;
}

void threadFunc_NI6122_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6122 *pNI6122 = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	int32	totSampsAllChan;
	int32	numSampsPerChan;
#if 0
	// RESERVED : GAIN
	float64	currGain;
#endif

	kLog (K_TRACE, "[NI6122_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_NI6122_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNI6122->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_TRACE, "[NI6122_DAQ] %s : mode(%s) rate(%.f) time(%.2f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6122->sample_rate, pNI6122->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_INFO, "[NI6122_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_DEBUG, "[NI6122_DAQ] %s : mode(%s) rate(%.f) time(%.2f-%.2f = %.2f)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6122->sample_rate,
				pNI6122->stop_t1, pNI6122->start_t0, pNI6122->sample_time);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNI6122->taskHandle == 0)) {
				//processAutoTrendRun (pSTDdev);
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNI6122->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
#if !TEST_WITHOUT_DAQ	// for testing without DAQ
				stopDevice (pSTDdev);

				// if DAQmxResetDevice is called, system is suspended at 2nd sharing trigger operation
				//NIERROR(DAQmxResetDevice(pSTDdev->strArg0));
				NIERROR(DAQmxCreateTask("",&pNI6122->taskHandle));

				kLog (K_MON, "[NI6122_DAQ] %s : term_config(%d) minVal(%.3f) maxVal(%.3f) units(%d)\n",
						pSTDdev->taskName, pNI6122->terminal_config, pNI6122->minVal, pNI6122->maxVal, pNI6122->units);

				for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
					NIERROR (
						DAQmxCreateAIVoltageChan (
							pNI6122->taskHandle,				// taskHandle
							pNI6122->ST_Ch[i].physical_channel,	// physicalChannel
							"",									// nameToAssignToChannel
							pNI6122->terminal_config,			// terminalConfig
							pNI6122->minVal,					// minVal
							pNI6122->maxVal,					// maxVal
							pNI6122->units,						// units
							NULL								// customScaleName
						)
					);

#if 0	// RESERVED : GAIN
					NIERROR (
						DAQmxGetAIGain (
							pNI6122->taskHandle,				// taskHandle
							pNI6122->ST_Ch[i].physical_channel,	// physicalChannel
							&currGain							// taskHandle
						)
					);

					kLog (K_INFO, "[NI6122_DAQ] DAQmxCreateAIVoltageChan : ch(%s) gain(%f) new(%f)\n",
							pNI6122->ST_Ch[i].physical_channel, currGain, pNI6122->gain);
#endif
				}

				totSampsAllChan	= getTotSampsAllChan(pNI6122);
				numSampsPerChan = getCbNumSampsPerChan(pNI6122);

				NIERROR(
					DAQmxCfgSampClkTiming(
						pNI6122->taskHandle,	// taskHandle
						"",						// source terminal of the Sample clock. NULL(internal)
						pNI6122->sample_rate,	// sampling rate in samples per second per channel
						DAQmx_Val_Rising,		// activeEdge. edge of the clock to acquire or generate samples
						pNI6122->smp_mode,		// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
						totSampsAllChan			// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
					)
				);

				kLog (K_INFO, "[NI6122_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
						pSTDdev->taskName,
						getModeMsg(pSTDdev->ST_Base.opMode),
						pNI6122->smp_mode, pNI6122->sample_rate, pNI6122->sample_time,
						numSampsPerChan, 
						totSampsAllChan);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
						DAQmxCfgDigEdgeStartTrig(
							pNI6122->taskHandle,
							pNI6122->triggerSource,
							DAQmx_Val_Rising
						)
					);

					kLog (K_MON, "[NI6122_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pNI6122->triggerSource);
				}
				else {
					kLog (K_MON, "[NI6122_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}

				NIERROR(
					DAQmxRegisterEveryNSamplesEvent(
						pNI6122->taskHandle,		// taskHandle
						DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
						numSampsPerChan,		// nSamples. number of samples after whitch each event should occur
						0,				// options. 0(callback function is called in a DAQmx thread)
						EveryNCallback,			// callbackFunction
						(void *)pSTDdev			// callbackData. parameter to be passed to callback function
					)
				);

				armingDeviceParams (pNI6122);

				NIERROR(DAQmxStartTask(pNI6122->taskHandle));

				set_ioc_dig_stat (DIG_STARTED);

				kLog (K_INFO, "[threadFunc_NI6122_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNI6122->auto_run_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNI6122->auto_run_flag = 0;

				kLog (K_INFO, "[threadFunc_NI6122_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				kLog (K_INFO, "[threadFunc_NI6122_DAQ] before DAQmxClearTask for %s\n", pSTDdev->taskName);

				NIERROR(DAQmxClearTask(pNI6122->taskHandle));
				pNI6122->taskHandle = 0;

				kLog (K_INFO, "[threadFunc_NI6122_DAQ] after  DAQmxClearTask for %s\n", pSTDdev->taskName);

				set_ioc_dig_stat (DIG_RESET);
#endif

				if (TRUE == isStoreMode ()) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;
				}
				else {
					setInitStatus (pSTDdev);
				}

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_POST_PROCESS)) {
				flush_local_file(pSTDdev);								

				pSTDdev->StatusDev = TASK_STANDBY | TASK_DATA_TRANSFER;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_DATA_TRANSFER)) {
#if 0
				if(pMaster->n8MdsPutFlag == 0) {
					pMaster->n8MdsPutFlag = 1;

					for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
						if (proc_sendData2Tree(pSTDdev) == WR_OK) {
							pMaster->n8MdsPutFlag = 0;
							break;
						}

						epicsThreadSleep(1);

						kLog (K_INFO, "[threadFunc_NI6122_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}

					close_local_file (pSTDdev);

					pMaster->n8MdsPutFlag = 0;

					setInitStatus (pSTDdev);

					notify_refresh_master_status();

					// reset amplifier : NI 488.2
					resetDeviceAmp (pSTDdev);
				}
				else if(pMaster->n8MdsPutFlag == 1) {
					kLog (K_INFO, "[threadFunc_NI6122_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
#else
				kLog (K_MON, "[threadFunc_NI6122_DAQ] %s : Waiting lock for MdsPlus\n", pSTDdev->taskName);

				epicsMutexLock (pMaster->lock_mds);

				kLog (K_MON, "[threadFunc_NI6122_DAQ] %s : Starting Mdsplus processing\n", pSTDdev->taskName);

				pMaster->n8MdsPutFlag = 1;

				for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
					if (proc_sendData2Tree(pSTDdev) == WR_OK) {
						break;
					}

					epicsThreadSleep(1);

					kLog (K_INFO, "[threadFunc_NI6122_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
				}

				pMaster->n8MdsPutFlag = 0;

				epicsMutexUnlock (pMaster->lock_mds);

				close_local_file (pSTDdev);

				setInitStatus (pSTDdev);

				notify_refresh_master_status();

				// reset amplifier : NI 488.2
				resetDeviceAmp (pSTDdev);
#endif
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_NI6122_RingBuf(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6122_RingBuf] ...\n");
}

void threadFunc_NI6122_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6122_CatchEnd] ...\n");
}

int create_NI6122_taskConfig(ST_STD_device *pSTDdev, const int nAmpDeviceIdx)
{
	ST_NI6122 *pNI6122 = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;

	pNI6122 = (ST_NI6122*) calloc(1, sizeof(ST_NI6122));
	if(!pNI6122) return WR_ERROR;
	
	kLog (K_TRACE, "[create_NI6122_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pNI6122->nAmpDeviceIdx = nAmpDeviceIdx;
	pNI6122->taskHandle = 0x0;
	pNI6122->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNI6122->data_buf) {
		free (pNI6122);
		return WR_ERROR;
	}

	ellInit(pNI6122->data_buf);

	pNI6122->callbackRate	= 1;	// 1Hz

	for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
		pNI6122->ST_Ch[i].write_fp	= NULL;

		sprintf (pNI6122->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
		sprintf (pNI6122->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);

		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			pNI6122->ST_Ch[i].used		= CH_USED;
			pNI6122->ST_Ch[i].enabled	= CH_USED;

			strcpy (pNI6122->ST_Ch[i].inputTagName, pSTDCh->strArg1);
			strcpy (pNI6122->ST_Ch[i].inputPvName , pSTDCh->strArg2);

			pNI6122->ST_Ch[i].inputType	= INPUT_TYPE_NONE;

			kLog (K_INFO, "[create_NI6122_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pNI6122->ST_Ch[i].physical_channel,
					pNI6122->ST_Ch[i].inputType, pNI6122->ST_Ch[i].inputTagName,
					pNI6122->ST_Ch[i].file_name);
		}
	}

	sprintf (pNI6122->triggerSource, "%s", pSTDdev->strArg1); 

	pNI6122->terminal_config	= DAQmx_Val_Diff;	// DAQmx_Val_Diff(6122), DAQmx_Val_NRSE, NI-6254(DAQmx_Val_RSE)
	pNI6122->minVal				= INIT_LOW_LIMIT;
	pNI6122->maxVal				= INIT_HIGH_LIMIT;
	pNI6122->gain				= INIT_GAIN;
	pNI6122->sample_rate		= INIT_SAMPLE_RATE;
	pNI6122->sample_time		= getSampTime(pNI6122);
	pNI6122->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNI6122->units				= DAQmx_Val_Volts;
	pNI6122->rawGradient		= getRawGradient (pNI6122);
	pNI6122->euGradient			= getEuGradient (pNI6122);

	pSTDdev->pUser				= pNI6122;

	pSTDdev->ST_Func._OP_MODE			= func_NI6122_OP_MODE;
	pSTDdev->ST_Func._SYS_ARMING		= func_NI6122_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN			= func_NI6122_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_NI6122_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET			= func_NI6122_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0			= func_NI6122_T0;
	pSTDdev->ST_Func._SYS_T1			= func_NI6122_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NI6122_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NI6122_SHOT_NUMBER;

	pSTDdev->ST_Func._Exit_Call			= func_NI6122_EXIT;

	//pSTDdev->ST_Func._POST_SEQSTOP		= func_NI6122_POST_SEQSTOP; 
	//pSTDdev->ST_Func._PRE_SEQSTART		= func_NI6122_PRE_SEQSTART;

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NI6122_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 0
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6122_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6122_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvNI6122_init_driver(void)
{
	kLog (K_TRACE, "[drvNI6122_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	int	nAmpDeviceIdx = 0;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_NI6122_taskConfig (pSTDdev, nAmpDeviceIdx) == WR_ERROR) {
			kLog (K_ERR, "[drvNI6122_init_driver] %s : create_NI6122_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		nAmpDeviceIdx++;

		set_STDdev_status_reset(pSTDdev);

		drvNI6122_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	// initialize global variable
	memset (&gIocVar, 0x00, sizeof(ST_IOC_VAR));

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNI6122_io_report(int level)
{
	kLog (K_TRACE, "[drvNI6122_io_report] ...\n");

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

