#define _GNU_SOURCE
#include <math.h>
#include <fcntl.h>	// for mkdir

#include "drvNI6220.h"

static long drvNI6220_io_report(int level);
static long drvNI6220_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNI6220 = {
	2,
	drvNI6220_io_report,
	drvNI6220_init_driver
};

epicsExportAddress(drvet, drvNI6220);

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
		DAQmxGetExtendedErrorInfo(errBuff,sizeof(errBuff));
		kuDebug (kuERR, "[%s:%d] DAQmx Error: %s\n", fname, lineNo, errBuff);
		notify_error (1, "DAQmx Error: %s\n", errBuff);

		epicsThreadSleep(1);
		return (NOK);
	}

	return (OK);
}

int isCalcRequired (ST_NI6220_channel *pChInfo)
{
	return (0 < pChInfo->inputType);
}

int isStoreRequired (ST_NI6220_channel *pChInfo)
{
	return (0 < pChInfo->inputType);
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

ST_NI6220 *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NI6220 *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NI6220 *pNI6220) 
{ 
	return (pNI6220->sample_rate); 
}

int getArraySizeInBytes (ST_NI6220 *pNI6220) 
{
   	return (sizeof(pNI6220->readArray)); 
}

int getMaxNumChan (ST_NI6220 *pNI6220) 
{ 
	return (INIT_MAX_CHAN_NUM); 
}

float64 getTotSampsChan (ST_NI6220 *pNI6220) 
{ 
	return (pNI6220->sample_rate * pNI6220->sample_time); 
}

int getTotSampsAllChan (ST_NI6220 *pNI6220) 
{ 
	return (pNI6220->sample_rate * getMaxNumChan(pNI6220)); 
}

int getCallbackRate (ST_NI6220 *pNI6220)
{
	return (pNI6220->callbackRate);
}

int getCbNumSampsPerChan (ST_NI6220 *pNI6220)
{
	return (getNumSampsPerChan (pNI6220) / getCallbackRate (pNI6220) );
}

int getSampRate (ST_NI6220 *pNI6220) 
{ 
	return (pNI6220->sample_rate); 
}

float64 getSampTime (ST_NI6220 *pNI6220) 
{ 
	return (pNI6220->sample_time); 
}

void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_NI6220 *pNI6220	= pSTDdev->pUser;

	if (pNI6220->stop_t1 <= pNI6220->start_t0) {
		pNI6220->sample_time = 0;
	}
	else {
		pNI6220->sample_time = pNI6220->stop_t1 - pNI6220->start_t0;
	}

	kuDebug (kuINFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pNI6220->start_t0, pNI6220->stop_t1, pNI6220->sample_time);

	scanIoRequest (pSTDdev->ioScanPvt_userCall);
}

void setStartT0 (ST_STD_device *pSTDdev, float64 val) 
{
	ST_NI6220 *pNI6220	= pSTDdev->pUser;

	pNI6220->start_t0 = val;

	setSamplTime (pSTDdev);
}

void setStopT1 (ST_STD_device *pSTDdev, float64 val)
{
	ST_NI6220 *pNI6220	= pSTDdev->pUser;

	pNI6220->stop_t1 = val;

	setSamplTime (pSTDdev);
}

float64 getRawGradient (ST_NI6220 *pNI6220)
{
	return (RAW_GRADIENT);
}

float64 getEuGradient (ST_NI6220 *pNI6220)
{
	return (EU_GRADIENT);
}

float64 getRawValue (ST_NI6220 *pNI6220, float64 euValue)
{
	return ( euValue * pNI6220->rawGradient );
}

float64 getEuValue (ST_NI6220 *pNI6220, float64 rawValue)
{
	return ( rawValue * pNI6220->euGradient );
}

void armingDeviceParams (ST_NI6220 *pNI6220)
{
	pNI6220->totalRead		= 0;
	pNI6220->rawGradient	= getRawGradient (pNI6220);
	pNI6220->euGradient		= getEuGradient (pNI6220);
	pNI6220->maxAmpl		= INT_MIN;	//TODO : MINVAL

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
	ST_NI6220	*pNI6220 = getDriver(pSTDdev);

	if (TRUE != isStoreMode ()) {
		return (FALSE);
	}

	return ( pNI6220->totalRead >= getTotSampsChan(pNI6220)  );
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
	kuDebug (kuTRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);

	ST_NI6220 *pNI6220 = pSTDdev->pUser;
	int i;
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6220->ST_Ch[i].write_fp) {
			fclose (pNI6220->ST_Ch[i].write_fp);
			pNI6220->ST_Ch[i].write_fp = NULL;
		}

		if (NULL != pNI6220->ST_Ch[i].write_fp_eu) {
			fclose (pNI6220->ST_Ch[i].write_fp_eu);
			pNI6220->ST_Ch[i].write_fp_eu = NULL;
		}

		if (NULL != pNI6220->ST_Ch[i].write_fp_calc) {
			fclose (pNI6220->ST_Ch[i].write_fp_calc);
			pNI6220->ST_Ch[i].write_fp_calc = NULL;
		}

		if (TRUE != isStoreRequired (&pNI6220->ST_Ch[i])) {
			continue;
		}

		if ( (CH_USED == pNI6220->ST_Ch[i].used) && (CH_USED == pNI6220->ST_Ch[i].enabled) ) {
			sprintf (pNI6220->ST_Ch[i].path_name, "%sS%06d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6220->ST_Ch[i].file_name);

			kuDebug (kuINFO, "[make_local_file] fname(%s)\n", pNI6220->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pNI6220->ST_Ch[i].write_fp = fopen(pNI6220->ST_Ch[i].path_name, "w+"))) {
				kuDebug (kuERR, "[make_local_file] %s create failed!!!\n", pNI6220->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6220->ST_Ch[i].path_name);
			}

			sprintf (pNI6220->ST_Ch[i].path_name, "%sS%06d/%s_EU",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6220->ST_Ch[i].file_name);

			if (NULL == (pNI6220->ST_Ch[i].write_fp_eu = fopen(pNI6220->ST_Ch[i].path_name, "w+"))) {
				kuDebug (kuERR, "[make_local_file] %s create failed!!!\n", pNI6220->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6220->ST_Ch[i].path_name);
			}

			sprintf (pNI6220->ST_Ch[i].path_name, "%sS%06d/%s_CALC",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6220->ST_Ch[i].file_name);

			if (NULL == (pNI6220->ST_Ch[i].write_fp_calc = fopen(pNI6220->ST_Ch[i].path_name, "w+"))) {
				kuDebug (kuERR, "[make_local_file] %s create failed!!!\n", pNI6220->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6220->ST_Ch[i].path_name);
			}
		}
	}
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6220 *pNI6220 = pSTDdev->pUser;
	int i;

	kuDebug (kuTRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6220->ST_Ch[i].write_fp) {
			fflush(pNI6220->ST_Ch[i].write_fp);
		}

		if (NULL != pNI6220->ST_Ch[i].write_fp_eu) {
			fflush(pNI6220->ST_Ch[i].write_fp_eu);
		}

		if (NULL != pNI6220->ST_Ch[i].write_fp_calc) {
			fflush(pNI6220->ST_Ch[i].write_fp_calc);
		}
	}
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6220 *pNI6220 = pSTDdev->pUser;
	int i;

	kuDebug (kuTRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6220->ST_Ch[i].write_fp) {
			fclose(pNI6220->ST_Ch[i].write_fp);
			pNI6220->ST_Ch[i].write_fp	= NULL;
		}

		if (NULL != pNI6220->ST_Ch[i].write_fp_eu) {
			fclose(pNI6220->ST_Ch[i].write_fp_eu);
			pNI6220->ST_Ch[i].write_fp_eu	= NULL;
		}

		if (NULL != pNI6220->ST_Ch[i].write_fp_calc) {
			fclose(pNI6220->ST_Ch[i].write_fp_calc);
			pNI6220->ST_Ch[i].write_fp_calc	= NULL;
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

	kuDebug (kuMON, "[updateShotNumber] from(%d) -> to(%d)\n", prevShotNumber, pMaster->ST_Base.shotNumber);

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

	kuDebug (kuMON, "[createLocalMdsTree] %s : mode(%s) ip(%s) tree(%s) shot(%d)\n",
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
	ST_NI6220 *pNI6220 = pSTDdev->pUser;

	if (0 != pNI6220->taskHandle) {
		kuDebug (kuMON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNI6220->totalRead);

		DAQmxTaskControl(pNI6220->taskHandle, DAQmx_Val_Task_Abort);
		NIERROR(DAQmxStopTask(pNI6220->taskHandle));

		if (pNI6220->auto_run_flag == 1) {
			kuDebug (kuINFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			// post event to threadFunc_NI6220_DAQ
			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
			pNI6220->auto_run_flag = 0;

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

	kuDebug (kuMON, "[resetDeviceAll] ...\n");

	while (pSTDdev) {
		resetDevice (pSTDdev);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	// clear error message
	strcpy (pMaster->ErrorString, "");

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

	kuDebug (kuMON, "[armingDevice] %s\n", pSTDdev->taskName);

	// clear error message
	strcpy (pMaster->ErrorString, "");

	// reset the running task
	stopDevice (pSTDdev);

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_ARM_ENABLED;

	pSTDdev->ST_Base.dT0[0]		= pMaster->ST_Base.dT0[0];
	pSTDdev->ST_Base_fetch.dT0[0]	= pMaster->ST_Base.dT0[0];

	pSTDdev->ST_Base.dT1[0]		= pMaster->ST_Base.dT1[0];
	pSTDdev->ST_Base_fetch.dT1[0]	= pMaster->ST_Base.dT1[0];

	kuDebug (kuMON, "[armingDevice] T0/T1 : Dev(%s) Master(%.f/%.f) Dev(%.f/%.f) fetch(%.f/%.f)\n",
			pSTDdev->taskName,
			pMaster->ST_Base.dT0[0], pMaster->ST_Base.dT1[0], 
			pSTDdev->ST_Base.dT0[0], pSTDdev->ST_Base.dT1[0], 
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0]);

	flush_STDdev_to_MDSfetch (pSTDdev);

	if (TRUE == isStoreMode ()) {
		make_local_file (pSTDdev);
	}
}

// makes name of tag for raw value
int drvNI6220_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	ST_NI6220 *pNI6220 = (ST_NI6220 *)pSTDdev->pUser;

	kuDebug (kuTRACE, "[drvNI6220_set_TagName] ...\n");

	for(i=0; i<INIT_MAX_CHAN_NUM; i++) {
		sprintf(pNI6220->ST_Ch[i].strTagName, "\\%s:FOO", pNI6220->ST_Ch[i].inputTagName);

		kuDebug (kuINFO, "%s\n", pNI6220->ST_Ch[i].strTagName);

		if (TRUE == isCalcRequired (&pNI6220->ST_Ch[i])) {
			sprintf(pNI6220->ST_Ch[i].strTagNameEu, "\\%s", pNI6220->ST_Ch[i].inputTagName);
			sprintf(pNI6220->ST_Ch[i].strTagNameCalc, "\\%s:FOO", pNI6220->ST_Ch[i].inputTagNameCalc);

			kuDebug (kuINFO, "%s / %s\n", pNI6220->ST_Ch[i].strTagNameEu, pNI6220->ST_Ch[i].strTagNameCalc);
		}
	}

	return WR_OK;
}

void func_NI6220_POST_SEQSTOP(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kuDebug (kuMON, "[POST_SEQSTOP] %s : value(%d) status(%d)\n", pSTDdev->taskName, (int)arg1, (int)arg2);

	if (0 == (int)arg1) {
		if (TRUE != isRemoteRunning ()) {
			//processTrendRun ();
		}
		else {
			kuDebug (kuINFO, "[POST_SEQSTOP] %s : DAQ is running in remote mode\n", pSTDdev->taskName);
		}
	}
}

// stops DAQ to ready shot start
void func_NI6220_PRE_SEQSTART(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kuDebug (kuMON, "[PRE_SEQSTART] %s : setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		//processCalcRun ();
	}
}

void func_NI6220_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[SYS_RESET] %s: setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		resetDevice (pSTDdev);

		admin_all_taskStatus_reset();

		scanIoRequest(pMaster->ioScanPvt_status);

		DBproc_put(PV_RESET_STR, "0");
	}
}

void func_NI6220_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) { /* in case of arming  */
		if (admin_check_Arming_condition() == WR_ERROR) {
			kuDebug (kuERR, "[SYS_ARMING] %s : arming failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		armingDevice (pSTDdev);
	}
	else { /* release arming condition */
		if (admin_check_Release_condition() == WR_ERROR) {
			kuDebug (kuERR, "[SYS_ARMING] %s : release failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			//notify_error (1, "%s: release failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		kuDebug (kuMON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6220_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[SYS_RUN] %s : %d\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		if (admin_check_Run_condition() == WR_ERROR) {
			kuDebug (kuERR, "[SYS_RUN] %s : run failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: run failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6220_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[OP_MODE] %s : %f, %f\n", pSTDdev->taskName, arg1, arg2);

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

void func_NI6220_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	kuDebug (kuMON, "[DATA_SEND] %s\n", pSTDdev->taskName);
}

void func_NI6220_CREATE_LOCAL_TREE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[CREATE_LOCAL_TREE] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}

void func_NI6220_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NI6220_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6220 *pNI6220 = NULL;

	kuDebug (kuMON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNI6220 = pSTDdev->pUser;
	pNI6220->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
}

void func_NI6220_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[func_NI6220_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	setStartT0 (pSTDdev, arg1);
}

void func_NI6220_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[func_NI6220_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	setStopT1 (pSTDdev, arg1);
}

void func_NI6220_EXIT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kuDebug (kuMON, "[func_NI6220_EXIT] %s \n", pSTDdev->taskName);
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	ST_STD_device		*pSTDdev	= (ST_STD_device*) callbackData;
	ST_NI6220			*pNI6220	= pSTDdev->pUser;
	ST_NI6220_channel	*pNI6220_CH	= NULL;

	int 	i;
	int		chNo;
	int32	sampsRead		= 0;
	int32	numSampsPerChan = nSamples;
	int		ret;

	kuDebug (kuMON, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) shot(%lu/%lu)\n",
			pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6220->sample_rate,
			getStatusMsg(pSTDdev->StatusDev, NULL),
			pNI6220->totalRead, get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

	if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

		kuDebug (kuINFO, "[EveryNCallback] %s : status(0x%x:%s)\n",
				pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

		notify_refresh_master_status();
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}

	ret = NIERROR(
		DAQmxReadAnalogF64 (
			pNI6220->taskHandle, 			// taskHandle
			numSampsPerChan, 				// numSampsPerChan. number of samples per channel to read
			600.0, 							// timeout (seconds)
			DAQmx_Val_GroupByChannel,		// DAQmx_Val_GroupByScanNumber (Group by scan number (interleaved), DAQmx_Val_GroupByChannel
			pNI6220->fScaleReadArray, 		// readAray. array into which samples are read
			sizeof(pNI6220->fScaleReadArray),	// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 			// sampsRead. actual number of bytes read into the array per scan
			NULL							// reserved
		)
	);

	if (OK != ret) {
		stopDevice (pSTDdev);
		return 1;
	}

	if (TRUE == kuCanPrint (kuDATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%.2f ", pNI6220->fScaleReadArray[i]);
		}
		printf ("\n");
	}

	if (TRUE == isStoreMode ()) {
		pNI6220->totalRead	+= sampsRead;
	}

	FILE *fp = NULL;
	int32 arrayJump;

    // DAQmx_Val_GroupByChannel (non-interleaved) at DAQmxReadAnalogF64
	for (chNo = 0; chNo < INIT_MAX_CHAN_NUM; chNo++) {
		pNI6220_CH	= &pNI6220->ST_Ch[chNo];
		arrayJump	= chNo * numSampsPerChan;

		if ( (CH_USED != pNI6220_CH->used) || (CH_USED != pNI6220_CH->enabled) ) {
			continue;
		}

		for (i = arrayJump; i < arrayJump + numSampsPerChan; i++) {
			// scaled -> raw
			pNI6220->readArray[i]	= getRawValue (pNI6220, pNI6220->fScaleReadArray[i]);
		}

		// last value
		pNI6220_CH->rawValue	= pNI6220->readArray[i-1];
		pNI6220_CH->euVolt		= pNI6220->fScaleReadArray[i-1];
		pNI6220_CH->euValue		= pNI6220_CH->euVolt * pNI6220_CH->gain + pNI6220_CH->offset;

		if (NULL != (fp = pNI6220_CH->write_fp)) {
			fwrite(&pNI6220->readArray[arrayJump], sizeof(pNI6220->readArray[0]), numSampsPerChan, pNI6220_CH->write_fp);
			fflush(pNI6220_CH->write_fp);
		}
	}

	pNI6220->ST_Ch[0].pfValArray	= &pNI6220->fScaleReadArray[0];
	pNI6220->ST_Ch[1].pfValArray	= &pNI6220->fScaleReadArray[numSampsPerChan];

	for (i = 0; i < numSampsPerChan; i++) {
		// -------------------------------
		// Ch #0 : AMPL
		// -------------------------------

		// ax + b
		pNI6220->ST_Ch[0].fVoltArray[i]	= pNI6220->ST_Ch[0].pfValArray[i] * pNI6220->ST_Ch[0].gain + pNI6220->ST_Ch[0].offset;

		// ax + b = G -> G = 20 * log10(E/E), E/E = exp10(G/20)
		pNI6220->ST_Ch[0].fEuArray[i]	= exp10 (pNI6220->ST_Ch[0].fVoltArray[i] / 20.0);

		pNI6220->maxAmpl				= MAX (pNI6220->maxAmpl, pNI6220->ST_Ch[0].fEuArray[i]);

		// -------------------------------
		// Ch #1 : PHASE
		// -------------------------------

		// ax + b
		pNI6220->ST_Ch[1].fVoltArray[i]	= pNI6220->ST_Ch[1].pfValArray[i] * pNI6220->ST_Ch[1].gain + pNI6220->ST_Ch[1].offset;

		// phase
		pNI6220->ST_Ch[1].fEuArray[i]	= pNI6220->ST_Ch[1].fVoltArray[i];

		// -------------------------------
		// Calcuration : $(delta)
		// -------------------------------

		// alpha : 1/2 * tan^-1 ( 2 * tan& / (1 - tan&^2) * cos$)
		pNI6220->ST_Ch[0].fCalcArray[i]	= 0.5 / tan(2 * pNI6220->maxAmpl / (1 - exp(pNI6220->maxAmpl)) * cos(pNI6220->ST_Ch[1].fEuArray[i]));

		// beta  : 1/2 * sin^-1 ( 2 * sin@ * cos@ * sin$)
		pNI6220->ST_Ch[1].fCalcArray[i]	= 0.5 / sin(2 * sin(pNI6220->ST_Ch[0].fCalcArray[i]) * cos(pNI6220->ST_Ch[0].fCalcArray[i]) * sin(pNI6220->ST_Ch[1].fEuArray[i]));
	}

	for (i = 0; i < 2; i++) {
		pNI6220->ST_Ch[i].euVolt	= pNI6220->ST_Ch[i].fVoltArray[numSampsPerChan-1];
		pNI6220->ST_Ch[i].euValue	= pNI6220->ST_Ch[i].fEuArray  [numSampsPerChan-1];
		pNI6220->ST_Ch[i].calcValue	= pNI6220->ST_Ch[i].fCalcArray[numSampsPerChan-1];

		if (NULL != (fp = pNI6220->ST_Ch[i].write_fp_eu)) {
			fwrite(pNI6220->ST_Ch[i].fEuArray, sizeof(pNI6220->ST_Ch[i].fEuArray[0]), numSampsPerChan, pNI6220->ST_Ch[i].write_fp_eu);
			fflush(pNI6220->ST_Ch[i].write_fp_eu);
		}

		if (NULL != (fp = pNI6220->ST_Ch[i].write_fp_calc)) {
			fwrite(pNI6220->ST_Ch[i].fCalcArray, sizeof(pNI6220->ST_Ch[i].fCalcArray[0]), numSampsPerChan, pNI6220->ST_Ch[i].write_fp_calc);
			fflush(pNI6220->ST_Ch[i].write_fp_calc);
		}
	}

	scanIoRequest(pSTDdev->ioScanPvt_userCall);

	if (TRUE == isSamplingStop (pSTDdev)) {
		kuDebug (kuINFO, "[EveryNCallback] %s : stop ....\n", pSTDdev->taskName);
		stopDevice (pSTDdev);
	}

	return 0;
}

void threadFunc_NI6220_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6220 *pNI6220 = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	int32	totSampsAllChan;
	int32	numSampsPerChan;

	kuDebug (kuTRACE, "[NI6220_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kuDebug (kuERR, "[threadFunc_NI6220_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNI6220->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kuDebug (kuTRACE, "[NI6220_DAQ] %s : mode(%s) rate(%.f) time(%.2f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6220->sample_rate, pNI6220->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kuDebug (kuINFO, "[NI6220_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kuDebug (kuDEBUG, "[NI6220_DAQ] %s : mode(%s) rate(%.f) time(%.2f-%.2f = %.2f)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6220->sample_rate,
				pNI6220->stop_t1, pNI6220->start_t0, pNI6220->sample_time);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNI6220->taskHandle == 0)) {
				//processAutoTrendRun (pSTDdev);
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNI6220->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
#if !TEST_WITHOUT_DAQ	// for testing without DAQ
				stopDevice (pSTDdev);

				// if DAQmxResetDevice is called, system is suspended at 2nd sharing trigger operation
				//NIERROR(DAQmxResetDevice(pSTDdev->strArg0));
				NIERROR(DAQmxCreateTask("",&pNI6220->taskHandle));

				kuDebug (kuMON, "[NI6220_DAQ] %s : term_config(%d) minVal(%.3f) maxVal(%.3f) units(%d)\n",
						pSTDdev->taskName, pNI6220->terminal_config, pNI6220->minVal, pNI6220->maxVal, pNI6220->units);

				for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
					NIERROR (
						DAQmxCreateAIVoltageChan (
							pNI6220->taskHandle,				// taskHandle
							pNI6220->ST_Ch[i].physical_channel,	// physicalChannel
							"",									// nameToAssignToChannel
							pNI6220->terminal_config,			// terminalConfig
							pNI6220->minVal,					// minVal
							pNI6220->maxVal,					// maxVal
							pNI6220->units,						// units
							NULL								// customScaleName
						)
					);
				}

				totSampsAllChan	= getTotSampsAllChan(pNI6220);
				numSampsPerChan = getCbNumSampsPerChan(pNI6220);

				NIERROR(
					DAQmxCfgSampClkTiming(
						pNI6220->taskHandle,	// taskHandle
						"",						// source terminal of the Sample clock. NULL(internal)
						pNI6220->sample_rate,	// sampling rate in samples per second per channel
						DAQmx_Val_Rising,		// activeEdge. edge of the clock to acquire or generate samples
						pNI6220->smp_mode,		// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
						totSampsAllChan			// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
					)
				);

				kuDebug (kuINFO, "[NI6220_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
						pSTDdev->taskName,
						getModeMsg(pSTDdev->ST_Base.opMode),
						pNI6220->smp_mode, pNI6220->sample_rate, pNI6220->sample_time,
						numSampsPerChan, 
						totSampsAllChan);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
						DAQmxCfgDigEdgeStartTrig(
							pNI6220->taskHandle,
							pNI6220->triggerSource,
							DAQmx_Val_Rising
						)
					);

					kuDebug (kuMON, "[NI6220_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pNI6220->triggerSource);
				}
				else {
					kuDebug (kuMON, "[NI6220_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}

				NIERROR(
					DAQmxRegisterEveryNSamplesEvent(
						pNI6220->taskHandle,		// taskHandle
						DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
						numSampsPerChan,		// nSamples. number of samples after whitch each event should occur
						0,				// options. 0(callback function is called in a DAQmx thread)
						EveryNCallback,			// callbackFunction
						(void *)pSTDdev			// callbackData. parameter to be passed to callback function
					)
				);

				armingDeviceParams (pNI6220);

				NIERROR(DAQmxStartTask(pNI6220->taskHandle));

				kuDebug (kuINFO, "[threadFunc_NI6220_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNI6220->auto_run_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNI6220->auto_run_flag = 0;

				kuDebug (kuINFO, "[threadFunc_NI6220_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				kuDebug (kuINFO, "[threadFunc_NI6220_DAQ] before DAQmxClearTask for %s\n", pSTDdev->taskName);

				NIERROR(DAQmxClearTask(pNI6220->taskHandle));
				pNI6220->taskHandle = 0;

				kuDebug (kuINFO, "[threadFunc_NI6220_DAQ] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
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
				if(pMaster->n8MdsPutFlag == 0) {
					pMaster->n8MdsPutFlag = 1;

					for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
						if (proc_sendData2Tree(pSTDdev) == WR_OK) {
							break;
						}

						epicsThreadSleep(1);

						kuDebug (kuINFO, "[threadFunc_NI6220_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}

					close_local_file (pSTDdev);

					pMaster->n8MdsPutFlag = 0;

					setInitStatus (pSTDdev);

					notify_refresh_master_status();
				}
				else if(pMaster->n8MdsPutFlag == 1) {
					kuDebug (kuINFO, "[threadFunc_NI6220_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_NI6220_RingBuf(void *param)
{
	kuDebug (kuTRACE, "[threadFunc_NI6220_RingBuf] ...\n");
}

void threadFunc_NI6220_CatchEnd(void *param)
{
	kuDebug (kuTRACE, "[threadFunc_NI6220_CatchEnd] ...\n");
}

int create_NI6220_taskConfig(ST_STD_device *pSTDdev)
{
	ST_NI6220 *pNI6220 = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;

	pNI6220 = (ST_NI6220*) calloc(1, sizeof(ST_NI6220));
	if(!pNI6220) return WR_ERROR;
	
	kuDebug (kuTRACE, "[create_NI6220_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pNI6220->taskHandle = 0x0;
	pNI6220->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNI6220->data_buf) {
		free (pNI6220);
		return WR_ERROR;
	}

	ellInit(pNI6220->data_buf);

	pNI6220->callbackRate	= 10;	// Hz

	for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
		pNI6220->ST_Ch[i].write_fp		= NULL;
		pNI6220->ST_Ch[i].write_fp_eu	= NULL;
		pNI6220->ST_Ch[i].write_fp_calc	= NULL;

		sprintf (pNI6220->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
		sprintf (pNI6220->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);

		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			pNI6220->ST_Ch[i].used		= CH_USED;
			pNI6220->ST_Ch[i].enabled	= CH_USED;
			pNI6220->ST_Ch[i].gain		= 1;
			pNI6220->ST_Ch[i].offset	= 0;

			strcpy (pNI6220->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			pNI6220->ST_Ch[i].inputType	= strtoul(pSTDCh->strArg0, NULL, 0);

			// calcuration is required
			if (TRUE == isCalcRequired (&pNI6220->ST_Ch[i])) {
				strcpy (pNI6220->ST_Ch[i].inputTagNameCalc, pSTDCh->strArg2);
			}

			kuDebug (kuINFO, "[create_NI6220_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pNI6220->ST_Ch[i].physical_channel,
					pNI6220->ST_Ch[i].inputType, pNI6220->ST_Ch[i].inputTagName,
					pNI6220->ST_Ch[i].file_name);

			kuDebug (kuINFO, "[create_NI6220_taskConfig] dev(%s) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, 
					pNI6220->ST_Ch[i].inputTagNameCalc, pNI6220->ST_Ch[i].file_name);
		}
	}

	sprintf (pNI6220->triggerSource, "%s", pSTDdev->strArg1); 

	//pNI6220->terminal_config	= DAQmx_Val_Diff;	// DAQmx_Val_Diff(6220), DAQmx_Val_NRSE, NI-6254(DAQmx_Val_RSE)
	pNI6220->terminal_config	= DAQmx_Val_RSE;	// DAQmx_Val_Diff(6220), DAQmx_Val_NRSE, NI-6254(DAQmx_Val_RSE)
	pNI6220->minVal				= INIT_LOW_LIMIT;
	pNI6220->maxVal				= INIT_HIGH_LIMIT;
	pNI6220->sample_rate		= INIT_SAMPLE_RATE;
	pNI6220->sample_time		= getSampTime(pNI6220);
	pNI6220->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNI6220->units				= DAQmx_Val_Volts;
	pNI6220->rawGradient		= getRawGradient (pNI6220);
	pNI6220->euGradient			= getEuGradient (pNI6220);
	pNI6220->maxAmpl			= INT_MIN;	//TODO : MINVAL

	pSTDdev->pUser				= pNI6220;

	pSTDdev->ST_Func._OP_MODE			= func_NI6220_OP_MODE;
	//pSTDdev->ST_Func._CREATE_LOCAL_TREE	= func_NI6220_CREATE_LOCAL_TREE;
	pSTDdev->ST_Func._SYS_ARMING		= func_NI6220_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN			= func_NI6220_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_NI6220_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET			= func_NI6220_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0			= func_NI6220_T0;
	pSTDdev->ST_Func._SYS_T1			= func_NI6220_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NI6220_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NI6220_SHOT_NUMBER;

	pSTDdev->ST_Func._Exit_Call			= func_NI6220_EXIT;

	//pSTDdev->ST_Func._POST_SEQSTOP		= func_NI6220_POST_SEQSTOP; 
	//pSTDdev->ST_Func._PRE_SEQSTART		= func_NI6220_PRE_SEQSTART;

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NI6220_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kuDebug (kuERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	pSTDdev->ST_RingBufThread.threadFunc = NULL;

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6220_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvNI6220_init_driver(void)
{
	kuDebug (kuTRACE, "[drvNI6220_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_NI6220_taskConfig (pSTDdev) == WR_ERROR) {
			kuDebug (kuERR, "[drvNI6220_init_driver] %s : create_NI6220_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvNI6220_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kuDebug (kuDEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNI6220_io_report(int level)
{
	kuDebug (kuTRACE, "[drvNI6220_io_report] ...\n");

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

