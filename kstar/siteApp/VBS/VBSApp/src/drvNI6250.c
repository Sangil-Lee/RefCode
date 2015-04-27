#include <fcntl.h>	// for mkdir

#include "drvNI6250.h"

static long drvNI6250_io_report(int level);
static long drvNI6250_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNI6250 = {
	2,
	drvNI6250_io_report,
	drvNI6250_init_driver
};

epicsExportAddress(drvet, drvNI6250);

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

ST_NI6250 *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NI6250 *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NI6250 *pNI6250) 
{ 
	return (pNI6250->sample_rate); 
}

int getArraySizeInBytes (ST_NI6250 *pNI6250) 
{
   	return (sizeof(pNI6250->readArray)); 
}

int getMaxNumChan (ST_NI6250 *pNI6250) 
{ 
	return (INIT_MAX_CHAN_NUM); 
}

float64 getTotSampsChan (ST_NI6250 *pNI6250) 
{ 
	return (pNI6250->sample_rate * pNI6250->sample_time); 
}

float64 getTotAcqSampsChan (ST_NI6250 *pNI6250) 
{ 
	return (pNI6250->totalRead);
}

int getTotSampsAllChan (ST_NI6250 *pNI6250) 
{ 
	return (pNI6250->sample_rate * getMaxNumChan(pNI6250)); 
}

int getSampRate (ST_NI6250 *pNI6250) 
{ 
	return (pNI6250->sample_rate); 
}

float64 getSampTime (ST_NI6250 *pNI6250) 
{ 
	return (pNI6250->sample_time); 
}

void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_NI6250 *pNI6250	= pSTDdev->pUser;

	if (pNI6250->stop_t1 <= pNI6250->start_t0) {
		pNI6250->sample_time = 0;
	}
	else {
		pNI6250->sample_time = pNI6250->stop_t1 - pNI6250->start_t0;
	}

	kLog (K_INFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pNI6250->start_t0, pNI6250->stop_t1, pNI6250->sample_time);

	scanIoRequest (pSTDdev->ioScanPvt_userCall);
}

void setStartT0 (ST_STD_device *pSTDdev, float64 val) 
{
	ST_NI6250 *pNI6250	= pSTDdev->pUser;

	pNI6250->start_t0 = val;

	setSamplTime (pSTDdev);
}

void setStopT1 (ST_STD_device *pSTDdev, float64 val)
{
	ST_NI6250 *pNI6250	= pSTDdev->pUser;

	pNI6250->stop_t1 = val;

	setSamplTime (pSTDdev);
}

float64 getRawGradient (ST_NI6250 *pNI6250)
{
	//return ( I16_FULL_SCALE / (pNI6250->maxVal - pNI6250->minVal) );
	return (RAW_GRADIENT);
}

float64 getEuGradient (ST_NI6250 *pNI6250)
{
	//return ( (pNI6250->maxVal - pNI6250->minVal) / I16_FULL_SCALE );
	return (EU_GRADIENT);
}

float64 getRawValue (ST_NI6250 *pNI6250, float64 euValue)
{
	return ( euValue * pNI6250->rawGradient );
}

float64 getEuValue (ST_NI6250 *pNI6250, float64 rawValue)
{
	return ( rawValue * pNI6250->euGradient );
}

void armingDeviceParams (ST_NI6250 *pNI6250)
{
	pNI6250->totalRead		= 0;
	pNI6250->offset			= 0;
	pNI6250->rawGradient	= getRawGradient (pNI6250);
	pNI6250->euGradient		= getEuGradient (pNI6250);

#if 0
	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
	}
#endif
}

float64 getStartTime (ST_STD_device *pSTDdev)
{
	float64	dStartTime = 0;

	if (pSTDdev->ST_Base.opMode == OPMODE_REMOTE) {
		dStartTime  = pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime;
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

int isIpMinFaultEnable ()
{
	return (get_master()->cEnable_IPMIN);
}

int isIpMinFault (ST_NI6250 *pNI6250)
{
#if 0
	if (TRUE == isRemoteMode()) {
		if (TRUE == pNI6250->fault_ipmin) {
			return (TRUE);
		}
	}
#else
	if (TRUE == isIpMinFaultEnable()) {
		if (TRUE == isRemoteMode()) {
			if (TRUE == get_master()->cStatus_ip_min_fault) {
				return (TRUE);
			}
		}
	}
#endif

	return (FALSE);
}

void setIpMinFault (ST_NI6250 *pNI6250, int bFault)
{
	pNI6250->fault_ipmin = bFault;
}

int isSamplingStop (ST_STD_device *pSTDdev)
{
	ST_NI6250	*pNI6250 = getDriver(pSTDdev);

	if (TRUE != isStoreMode ()) {
		return (FALSE);
	}

	if (TRUE == isIpMinFault (pNI6250)) {
		return (TRUE);
	}

	return ( pNI6250->totalRead >= getTotSampsChan(pNI6250)  );
}

int isTriggerRequiredMode (const int mode)
{
	switch (mode) {
		case OPMODE_REMOTE :
		//TODO
		//case OPMODE_LOCAL :
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

	ST_NI6250 *pNI6250 = pSTDdev->pUser;

#if VBS_SFW_TREE_USE
	// SEG-3-1. setup shot information for archiving
	if (sfwTreeOk != sfwTreeSetShotInfo (pNI6250->pSfwTree, 
			pSTDdev->ST_Base_fetch.shotNumber,
			pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode], pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode], 
			pSTDdev->ST_mds.treeEventArg, pSTDdev->ST_mds.treeEventIP[pSTDdev->ST_Base.opMode])) {
		kuDebug (kuERR, "%s : cannot set shot information\n", pSTDdev->taskName);
		//return;
	}

	sfwTreePrintInfo (pNI6250->pSfwTree);

	// SEG-3-1.
	if (sfwTreeOk != sfwTreeSetSamplingRate (pNI6250->pSfwTree, getSampRate(pNI6250))) {
		kuDebug (kuERR, "%s : cannot set sampling rate\n", pSTDdev->taskName);
		//return;
	}
#endif	// VBS_SFW_TREE_USE

	int i;
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%09d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6250->ST_Ch[i].write_fp) {
			fclose (pNI6250->ST_Ch[i].write_fp);
		}

		pNI6250->ST_Ch[i].write_fp = NULL;

#if VBS_SFW_TREE_USE
		// SEG-3-2. 
		pNI6250->ST_Ch[i].pSfwTreeNode	= NULL;
#endif	// VBS_SFW_TREE_USE

		if (CH_USED == pNI6250->ST_Ch[i].used) {
			sprintf (pNI6250->ST_Ch[i].path_name,"%sS%09d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNI6250->ST_Ch[i].file_name);

			kLog (K_INFO, "[make_local_file] fname(%s)\n", pNI6250->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pNI6250->ST_Ch[i].write_fp = fopen(pNI6250->ST_Ch[i].path_name, "w+"))) {
				kLog (K_ERR, "[make_local_file] %s create failed!!!\n", pNI6250->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNI6250->ST_Ch[i].path_name);
			}

#if VBS_SFW_TREE_USE
			// SEG-3-2. add nodes to Tree object
#if VBS_SFW_TREE_STORE_RAW
			// 16 bits
			pNI6250->ST_Ch[i].pSfwTreeNode	= sfwTreeAddNode (pNI6250->pSfwTree,
					pNI6250->ST_Ch[i].strTagNameSeg, sfwTreeNodeWaveform, sfwTreeNodeInt16, pNI6250->ST_Ch[i].file_name);
			pNI6250->ST_Ch[i].pSfwTreeScale	= sfwTreeAddNode (pNI6250->pSfwTree,
					pNI6250->ST_Ch[i].strTagNameScale, sfwTreeNodeValue, sfwTreeNodeFloat64, NULL);
#else
			// float64
			pNI6250->ST_Ch[i].pSfwTreeNode	= sfwTreeAddNode (pNI6250->pSfwTree,
					pNI6250->ST_Ch[i].strTagNameSeg, sfwTreeNodeWaveform, sfwTreeNodeFloat64, pNI6250->ST_Ch[i].file_name);
#endif	// VBS_SFW_TREE_STORE_RAW

			if (NULL == pNI6250->ST_Ch[i].pSfwTreeNode) {
				if (epicsTrue == sfwTreeEnabled ()) {
					kuDebug (kuERR, "%s : cannot create TreeNode object\n", pSTDdev->taskName);
				}
			}
#endif	// VBS_SFW_TREE_USE
		}
	}

#if VBS_SFW_TREE_USE
	// SEG-3-2.
	sfwTreePrintNodes (pNI6250->pSfwTree);
#endif	// VBS_SFW_TREE_USE
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6250 *pNI6250 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6250->ST_Ch[i].write_fp) {
			fflush(pNI6250->ST_Ch[i].write_fp);
		}
	}
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NI6250 *pNI6250 = pSTDdev->pUser;
	int i;

	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<INIT_MAX_CHAN_NUM;i++) {
		if (NULL != pNI6250->ST_Ch[i].write_fp) {
			fclose(pNI6250->ST_Ch[i].write_fp);
			pNI6250->ST_Ch[i].write_fp	= NULL;
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
	ST_NI6250 *pNI6250 = pSTDdev->pUser;

	if (0 != pNI6250->taskHandle) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNI6250->totalRead);

		DAQmxTaskControl(pNI6250->taskHandle, DAQmx_Val_Task_Abort);
		NIERROR(DAQmxStopTask(pNI6250->taskHandle));

		if (pNI6250->auto_run_flag == 1) {
			kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
			pNI6250->auto_run_flag = 0;

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
	int i;
	ST_MASTER *pMaster	= get_master();
	ST_NI6250 *pNI6250 = (ST_NI6250 *)pSTDdev->pUser;

	kLog (K_MON, "[armingDevice] %s\n", pSTDdev->taskName);

	// clear error message
	sprintf (pMaster->ErrorString, "Normal");

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

#if VBS_SFW_TREE_USE
		// SEG-4-1. initialize archiving : creates connection and file
		if (sfwTreeOk != sfwTreeBeginArchive (getDriver(pSTDdev)->pSfwTree)) {
			kuDebug (kuERR, "[armingDevice] cannot begin archiving\n", pSTDdev->taskName);
		}

		for (i = 0; i < INIT_MAX_CHAN_NUM; i++) {
			if (CH_USED == pNI6250->ST_Ch[i].used) {
				// archiving : scalar value : scale
				sfwTreePutFloat64 (pNI6250->ST_Ch[i].pSfwTreeScale, pNI6250->euGradient);
			}
		}
#endif	// VBS_SFW_TREE_USE
	}

}

// makes name of tag for raw value
int drvNI6250_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	ST_NI6250 *pNI6250 = (ST_NI6250 *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvNI6250_set_TagName] ...\n");

	for(i=0; i<INIT_MAX_CHAN_NUM; i++) {
		sprintf(pNI6250->ST_Ch[i].strTagName, "\\%s:FOO", pNI6250->ST_Ch[i].inputTagName);
		kLog (K_INFO, "%s\n", pNI6250->ST_Ch[i].strTagName);

#if VBS_SFW_TREE_USE
		//TODO : Temporary. Later sfwTree will uses not strTagNameSeg but strTagName 
		sprintf(pNI6250->ST_Ch[i].strTagNameSeg  , "\\%s:RAW"  , pNI6250->ST_Ch[i].inputTagName);
		sprintf(pNI6250->ST_Ch[i].strTagNameScale, "\\%s:FBITS", pNI6250->ST_Ch[i].inputTagName);
#endif	// VBS_SFW_TREE_USE
	}

	return WR_OK;
}

void func_NI6250_POST_SEQSTOP(void *pArg, double arg1, double arg2)
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
void func_NI6250_PRE_SEQSTART(void *pArg, double arg1, double arg2)
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

void func_NI6250_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_RESET] %s: setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		resetDevice (pSTDdev);

		admin_all_taskStatus_reset();

		scanIoRequest(pMaster->ioScanPvt_status);

		DBproc_put(PV_RESET_STR, "0");
		DBproc_put(PV_SYS_ARMING_STR, "0");
	}
}

void func_NI6250_SYS_ARMING(void *pArg, double arg1, double arg2)
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
	}
	else { /* release arming condition */
		if (admin_check_Release_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : release failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			//notify_error (1, "%s: release failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

#if VBS_SFW_TREE_USE
		// SEG-4-1. initialize archiving : creates connection and file
		if (sfwTreeOk != sfwTreeEndArchive (getDriver(pSTDdev)->pSfwTree, sfwTreeActionDrop)) {
			kuDebug (kuERR, "[armingDevice] cannot end archiving\n", pSTDdev->taskName);
		}
#endif	// VBS_SFW_TREE_USE

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6250_SYS_RUN(void *pArg, double arg1, double arg2)
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

void func_NI6250_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[OP_MODE] %s : %f, %f\n", pSTDdev->taskName, arg1, arg2);

	if (pMaster->ST_Base.opMode != OPMODE_INIT) {
		pSTDdev->StatusDev |= TASK_STANDBY;

		resetDeviceAll ();

		if (OPMODE_LOCAL == pMaster->ST_Base.opMode) {
			updateShotNumber ();
		}
	}

	mds_copy_master_to_STD(pSTDdev);

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NI6250_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	kLog (K_MON, "[DATA_SEND] %s\n", pSTDdev->taskName);
}

void func_NI6250_CREATE_LOCAL_TREE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_SHOT] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}

void func_NI6250_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NI6250_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NI6250 *pNI6250 = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNI6250 = pSTDdev->pUser;
	pNI6250->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
}

void func_NI6250_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[func_NI6250_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	setStartT0 (pSTDdev, arg1);
}

void func_NI6250_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[func_NI6250_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	setStopT1 (pSTDdev, arg1);
}

void func_NI6250_FAULT_IP_MINIMUM(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[FAULT_IP_MINIMUM] %s : %d\n", pSTDdev->taskName, (int)arg1);

	// this value is refered by EveryNCallback
	setIpMinFault (pSTDdev->pUser, (int)arg1);
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	ST_STD_device	*pSTDdev = (ST_STD_device*) callbackData;
	ST_NI6250		*pNI6250 = pSTDdev->pUser;

	int 	i;
	int		chNo;
	int32	sampsRead		= 0;
	int32	numSampsPerChan = getNumSampsPerChan(pNI6250);
	int		ret;

#if !USE_SCALING_VALUE
	int32	numBytesPerSamp = 0;
	int32 	arraySizeInBytes = getArraySizeInBytes(pNI6250);
#endif

	if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

		kLog (K_MON, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) shot(%lu/%lu)\n",
				pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6250->sample_rate,
				getStatusMsg(pSTDdev->StatusDev, NULL),
				pNI6250->totalRead, get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		kLog (K_INFO, "[EveryNCallback] %s : status(0x%x:%s)\n",
				pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

		notify_refresh_master_status();
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	else {
		kLog (K_INFO, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) shot(%lu/%lu)\n",
				pSTDdev->taskName, everyNsamplesEventType, nSamples, pNI6250->sample_rate,
				getStatusMsg(pSTDdev->StatusDev, NULL),
				pNI6250->totalRead, get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);
	}

#if USE_SCALING_VALUE
	ret = NIERROR(
		DAQmxReadAnalogF64 (
			pNI6250->taskHandle, 			// taskHandle
			numSampsPerChan, 				// numSampsPerChan. number of samples per channel to read
			600.0, 							// timeout (seconds)
			DAQmx_Val_GroupByChannel,		// DAQmx_Val_GroupByScanNumber (Group by scan number (interleaved), DAQmx_Val_GroupByChannel
			pNI6250->fScaleReadArray, 		// readAray. array into which samples are read
			sizeof(pNI6250->fScaleReadArray),	// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 			// sampsRead. actual number of bytes read into the array per scan
			NULL							// reserved
		)
	);

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%.2f ", pNI6250->fScaleReadArray[i]);
		}
		printf ("\n");
	}

	int32	totSampsAllChan	= getTotSampsAllChan(pNI6250);

	// converts scaling eu value to raw value of 16bits resolution
	for (i = 0; i < totSampsAllChan; i++) {
		// make positive value
		if (0.0 > pNI6250->fScaleReadArray[i]) {
			pNI6250->fScaleReadArray[i] *= -1;
		}

		// assign gain and offset to get the +-10V value as result
		pNI6250->fScaleReadArray[i]	= pNI6250->fScaleReadArray[i] * pNI6250->gain + pNI6250->offset;

		// if result voltage value is satulated, then cutoff with maximum voltage
		if (EU_MAX_VOLT < pNI6250->fScaleReadArray[i]) {
			pNI6250->fScaleReadArray[i]	= EU_MAX_VOLT;
			pNI6250->readArray[i]		= (I16_FULL_SCALE / 2) - 1;
		}
		else {
			pNI6250->readArray[i] = getRawValue (pNI6250, pNI6250->fScaleReadArray[i]);
		}
	}
#else
	ret = NIERROR(
		DAQmxReadRaw (
			pNI6250->taskHandle, 	// taskHandle
			numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
			600.0, 					// timeout (seconds)
			pNI6250->readArrayScan,	// readAray. array into which samples are read
			arraySizeInBytes, 		// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 	// sampsRead. actual number of bytes read into the array per scan
			&numBytesPerSamp, 		// numBytesPerSamp
			NULL					// reserved
		)
	);

	int	idx = 0;
	int offset;
	int32	maxNumChan		= getMaxNumChan(pNI6250);

	// align raw datas by channel
	for (chNo = 0; chNo < INIT_MAX_CHAN_NUM; chNo++) {
		for (i = 0; i < numSampsPerChan; i++) {
			offset		= i * maxNumChan + chNo;

			if (0.0 > pNI6250->readArrayScan[ofset]) {
				pNI6250->readArrayScan[ofset] *= -1;
			}

			pNI6250->readArray[idx]			= pNI6250->readArrayScan[offset];
			pNI6250->fScaleReadArray[idx]	= getEuValue (pNI6250, pNI6250->readArrayScan[offset]);

			idx++;
		}
	}

	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%d ", pNI6250->readArray[i]);
		}
		printf ("\n");
	}
#endif

	if (OK != ret) {
		stopDevice (pSTDdev);
		return 1;
	}

	if (TRUE == isStoreMode ()) {
		pNI6250->totalRead	+= sampsRead;
	}

	FILE *fp = NULL;
	int32 arrayJump;
	int32 arrayJumpForLastSample;

    // DAQmx_Val_GroupByChannel (non-interleaved) at DAQmxReadAnalogF64
	for (chNo = 0; chNo < INIT_MAX_CHAN_NUM; chNo++) {
		arrayJump = chNo * numSampsPerChan;
		arrayJumpForLastSample	= arrayJump + numSampsPerChan - 1;

		if (NULL != (fp = pNI6250->ST_Ch[chNo].write_fp)) {
			fwrite(&pNI6250->readArray[arrayJump], sizeof(pNI6250->readArray[0]), numSampsPerChan, pNI6250->ST_Ch[chNo].write_fp);
			fflush(pNI6250->ST_Ch[chNo].write_fp);

#if VBS_SFW_TREE_USE
			// SEG-4-2. archiving : index, start time, size, buffer
			epicsInt32		iIndex		= (pNI6250->totalRead / sampsRead) - 1;		// 0 ~ N-1
			epicsFloat64	dStartTime	= getStartTime (pSTDdev) + iIndex;

#if VBS_SFW_TREE_STORE_RAW
			// 16 bits
			if (sfwTreeOk != sfwTreePutData (pNI6250->ST_Ch[chNo].pSfwTreeNode, iIndex, dStartTime, numSampsPerChan, &pNI6250->readArray[arrayJump])) {
#else
			// float64
			if (sfwTreeOk != sfwTreePutData (pNI6250->ST_Ch[chNo].pSfwTreeNode, iIndex, dStartTime, numSampsPerChan, &pNI6250->fScaleReadArray[arrayJump])) {
#endif	// VBS_SFW_TREE_STORE_RAW
				kuDebug (kuERR, "[EveryNCallback] %s : cannot put record\n", pSTDdev->taskName);
			}
#endif	// VBS_SFW_TREE_USE
		}

		pNI6250->ST_Ch[chNo].rawValue	= pNI6250->readArray[arrayJumpForLastSample];
		pNI6250->ST_Ch[chNo].euValue	= pNI6250->fScaleReadArray[arrayJumpForLastSample];

		for (i = 0; i < numSampsPerChan; i++) {
			if (getLogChNo() == chNo) {
				kLog (K_MON, "[EveryNCallback] %s : chNo(%d) raw(%d) volt(%.4f) \n",
						pSTDdev->taskName, chNo, pNI6250->readArray[arrayJump+i], pNI6250->fScaleReadArray[arrayJump+i]);
			}
			else if (TRUE == isPrintDebugMsg (K_DEBUG)) {
				kLog (K_DEBUG, "[EveryNCallback] %s : chNo(%d) raw(%d) volt(%.4f) \n",
						pSTDdev->taskName, chNo, pNI6250->readArray[arrayJump+i], pNI6250->fScaleReadArray[arrayJump+i]);
			}
		}
	}

#if VBS_SFW_TREE_USE
	sfwTreeUpdateArchive (pNI6250->pSfwTree);
#endif	// VBS_SFW_TREE_USE

	scanIoRequest(pSTDdev->ioScanPvt_userCall);

	if (TRUE == isSamplingStop (pSTDdev)) {
		kLog (K_INFO, "[EveryNCallback] %s : total(%d) : stop ....\n", pSTDdev->taskName, pNI6250->totalRead);
		stopDevice (pSTDdev);
	}

	return 0;
}

void threadFunc_NI6250_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6250 *pNI6250 = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	int32	totSampsAllChan;
	int32	numSampsPerChan;
	float64	currGain;
	float64	currCutoffFreq;

	kLog (K_TRACE, "[NI6250_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_NI6250_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNI6250->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_TRACE, "[NI6250_DAQ] %s : mode(%s) rate(%.f) time(%.2f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6250->sample_rate, pNI6250->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_INFO, "[NI6250_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_DEBUG, "[NI6250_DAQ] %s : mode(%s) rate(%.f) time(%.2f-%.2f = %.2f)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNI6250->sample_rate,
				pNI6250->stop_t1, pNI6250->start_t0, pNI6250->sample_time);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNI6250->taskHandle == 0)) {
				//processAutoTrendRun (pSTDdev);
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNI6250->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
#if !TEST_WITHOUT_DAQ	// for testing without DAQ
				stopDevice (pSTDdev);

				// if DAQmxResetDevice is called, system is suspended at 2nd sharing trigger operation
				//NIERROR(DAQmxResetDevice(pSTDdev->strArg0));
				NIERROR(DAQmxCreateTask("",&pNI6250->taskHandle));

				kLog (K_MON, "[NI6250_DAQ] %s : term_config(%d) minVal(%.3f) maxVal(%.3f) units(%d)\n",
						pSTDdev->taskName, pNI6250->terminal_config, pNI6250->minVal, pNI6250->maxVal, pNI6250->units);

				for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
					NIERROR (
						DAQmxCreateAIVoltageChan (
							pNI6250->taskHandle,				// taskHandle
							pNI6250->ST_Ch[i].physical_channel,	// physicalChannel
							"",									// nameToAssignToChannel
							pNI6250->terminal_config,			// terminalConfig
							pNI6250->minVal,					// minVal
							pNI6250->maxVal,					// maxVal
							pNI6250->units,						// units
							NULL								// customScaleName
						)
					);

					NIERROR (
						DAQmxGetAIGain (
							pNI6250->taskHandle,				// taskHandle
							pNI6250->ST_Ch[i].physical_channel,	// physicalChannel
							&currGain							// taskHandle
						)
					);

					if (pNI6250->cutoff_freq > 0) {
						NIERROR (
							DAQmxSetAILowpassEnable (
								pNI6250->taskHandle,				// taskHandle
								pNI6250->ST_Ch[i].physical_channel,	// physicalChannel
								1									// enable
							)
						);

						NIERROR (
							DAQmxGetAILowpassCutoffFreq (
								pNI6250->taskHandle,				// taskHandle
								pNI6250->ST_Ch[i].physical_channel,	// physicalChannel
								&currCutoffFreq				// cutoff frequency
							)
						);

						NIERROR (
							DAQmxSetAILowpassCutoffFreq (
								pNI6250->taskHandle,				// taskHandle
								pNI6250->ST_Ch[i].physical_channel,	// physicalChannel
								pNI6250->cutoff_freq				// cutoff frequency
							)
						);
					}

					kLog (K_INFO, "[NI6250_DAQ] ch(%s) : range(%.3f ~ %.3f) gain(%.0f->%.0f) cutoff(%.0f->%.0f)\n",
							pNI6250->ST_Ch[i].physical_channel, pNI6250->minVal, pNI6250->maxVal,
							currGain, pNI6250->gain, currCutoffFreq, pNI6250->cutoff_freq);
				}

				totSampsAllChan	= getTotSampsAllChan(pNI6250);
				numSampsPerChan = getNumSampsPerChan(pNI6250);

				NIERROR(
					DAQmxCfgSampClkTiming(
						pNI6250->taskHandle,	// taskHandle
						"",						// source terminal of the Sample clock. NULL(internal)
						pNI6250->sample_rate,	// sampling rate in samples per second per channel
						DAQmx_Val_Rising,		// activeEdge. edge of the clock to acquire or generate samples
						pNI6250->smp_mode,		// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
						totSampsAllChan			// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
					)
				);

				kLog (K_INFO, "[NI6250_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
						pSTDdev->taskName,
						getModeMsg(pSTDdev->ST_Base.opMode),
						pNI6250->smp_mode, pNI6250->sample_rate, pNI6250->sample_time,
						numSampsPerChan, 
						totSampsAllChan);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
						DAQmxCfgDigEdgeStartTrig(
							pNI6250->taskHandle,
							pNI6250->triggerSource,
							DAQmx_Val_Rising
						)
					);

					kLog (K_MON, "[NI6250_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pNI6250->triggerSource);
				}
				else {
					kLog (K_MON, "[NI6250_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}

				NIERROR(
					DAQmxRegisterEveryNSamplesEvent(
						pNI6250->taskHandle,		// taskHandle
						DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
						numSampsPerChan,		// nSamples. number of samples after whitch each event should occur
						0,				// options. 0(callback function is called in a DAQmx thread)
						EveryNCallback,			// callbackFunction
						(void *)pSTDdev			// callbackData. parameter to be passed to callback function
					)
				);

				armingDeviceParams (pNI6250);

				NIERROR(DAQmxStartTask(pNI6250->taskHandle));

				kLog (K_INFO, "[threadFunc_NI6250_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNI6250->auto_run_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNI6250->auto_run_flag = 0;

				kLog (K_INFO, "[threadFunc_NI6250_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				kLog (K_INFO, "[threadFunc_NI6250_DAQ] before DAQmxClearTask for %s\n", pSTDdev->taskName);

				NIERROR(DAQmxClearTask(pNI6250->taskHandle));
				pNI6250->taskHandle = 0;

				kLog (K_INFO, "[threadFunc_NI6250_DAQ] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
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
#if VBS_SFW_TREE_USE
				//TODO
				epicsThreadSleep(2);

				// SEG-4-3. cleanups archiving : flush data, close tree and disconnect
				if (sfwTreeOk != sfwTreeEndArchive (getDriver(pSTDdev)->pSfwTree, sfwTreeActionFlush)) {
					kuDebug (kuERR, "[threadFunc_NI6250_DAQ] %s : cannot cleanup archiving\n", pSTDdev->taskName);
				}

				//TODO
				epicsThreadSleep(1);
#endif	// VBS_SFW_TREE_USE

				flush_local_file(pSTDdev);								

				pSTDdev->StatusDev = TASK_STANDBY | TASK_DATA_TRANSFER;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_DATA_TRANSFER)) {
#if VBS_SFW_TREE_USE
				while (sfwTreeConnected (getDriver(pSTDdev)->pSfwTree)) {
					kLog (K_MON, "[threadFunc_NI6250_DAQ] %s : wait for disconnection for sfwTree ...\n", pSTDdev->taskName);
					epicsThreadSleep(1);
				}
#endif	// VBS_SFW_TREE_USE

#if 0
				if (pMaster->n8MdsPutFlag == 0) {
					pMaster->n8MdsPutFlag = 1;

#if 1	//TODO : SEG : when segmented archiving is stable, below mds put routine is not required
					for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
						if (proc_sendData2Tree(pSTDdev) == WR_OK) {
							pMaster->n8MdsPutFlag = 0;
							break;
						}

						epicsThreadSleep(1);

						kLog (K_INFO, "[threadFunc_NI6250_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}
#endif

					close_local_file (pSTDdev);

					pMaster->n8MdsPutFlag = 0;

					setInitStatus (pSTDdev);

					notify_refresh_master_status();
				}
				else if(pMaster->n8MdsPutFlag == 1) {
					kLog (K_INFO, "[threadFunc_NI6250_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
#else
				kLog (K_MON, "[threadFunc_NI6250_DAQ] %s : Waiting lock for MdsPlus\n", pSTDdev->taskName);

				epicsMutexLock (pMaster->lock_mds);

				kLog (K_MON, "[threadFunc_NI6250_DAQ] %s : Starting Mdsplus processing\n", pSTDdev->taskName);

				pMaster->n8MdsPutFlag = 1;

#if 1	//TODO : SEG : when segmented archiving is stable, below mds put routine is not required
				for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
					if (proc_sendData2Tree(pSTDdev) == WR_OK) {
						break;
					}

					epicsThreadSleep(1);
					kLog (K_INFO, "[threadFunc_NI6250_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
				}
#endif

				pMaster->n8MdsPutFlag = 0;

				epicsMutexUnlock (pMaster->lock_mds);

				close_local_file (pSTDdev);

				setInitStatus (pSTDdev);

				notify_refresh_master_status();
#endif
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_NI6250_RingBuf(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6250_RingBuf] ...\n");
}

void threadFunc_NI6250_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_NI6250_CatchEnd] ...\n");
}

int create_NI6250_taskConfig(ST_STD_device *pSTDdev)
{
	ST_NI6250 *pNI6250 = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;

	pNI6250 = (ST_NI6250*) calloc(1, sizeof(ST_NI6250));
	if(!pNI6250) return WR_ERROR;
	
	kLog (K_TRACE, "[create_NI6250_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pNI6250->taskHandle = 0x0;
	pNI6250->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNI6250->data_buf) {
		free (pNI6250);
		return WR_ERROR;
	}

	ellInit(pNI6250->data_buf);

	for (i=0;i<INIT_MAX_CHAN_NUM;i++) {
		pNI6250->ST_Ch[i].write_fp	= NULL;

		sprintf (pNI6250->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
		sprintf (pNI6250->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);

		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			pNI6250->ST_Ch[i].used = CH_USED;

			strcpy (pNI6250->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			pNI6250->ST_Ch[i].inputType	= INPUT_TYPE_NONE;

			kLog (K_INFO, "[create_NI6250_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pNI6250->ST_Ch[i].physical_channel,
					pNI6250->ST_Ch[i].inputType, pNI6250->ST_Ch[i].inputTagName,
					pNI6250->ST_Ch[i].file_name);
		}
	}

	sprintf (pNI6250->triggerSource, "%s", pSTDdev->strArg1); 

	pNI6250->terminal_config	= DAQmx_Val_Diff;	// DAQmx_Val_Diff(6250), DAQmx_Val_NRSE, NI-6254(DAQmx_Val_RSE)
	pNI6250->minVal				= INIT_LOW_LIMIT;
	pNI6250->maxVal				= INIT_HIGH_LIMIT;
	pNI6250->gain				= INIT_GAIN;
	pNI6250->sample_rate		= INIT_SAMPLE_RATE;
	pNI6250->sample_time		= getSampTime(pNI6250);
	pNI6250->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNI6250->units				= DAQmx_Val_Volts;
	pNI6250->rawGradient		= getRawGradient (pNI6250);
	pNI6250->euGradient			= getEuGradient (pNI6250);

	pSTDdev->pUser				= pNI6250;

	pSTDdev->ST_Func._OP_MODE			= func_NI6250_OP_MODE;
	//pSTDdev->ST_Func._CREATE_LOCAL_TREE	= func_NI6250_CREATE_LOCAL_TREE;
	pSTDdev->ST_Func._SYS_ARMING		= func_NI6250_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN			= func_NI6250_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_NI6250_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET			= func_NI6250_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0			= func_NI6250_T0;
	pSTDdev->ST_Func._SYS_T1			= func_NI6250_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NI6250_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NI6250_SHOT_NUMBER;

	pSTDdev->ST_Func._FAULT_IP_MINIMUM	= func_NI6250_FAULT_IP_MINIMUM;

	//pSTDdev->ST_Func._POST_SEQSTOP		= func_NI6250_POST_SEQSTOP; 
	//pSTDdev->ST_Func._PRE_SEQSTART		= func_NI6250_PRE_SEQSTART;

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NI6250_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 0
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6250_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NI6250_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

#if VBS_SFW_TREE_USE
	sfwTreeErr_e	eReturn;

	// SEG-1. creates an object for Tree per each task (or device) : object & thread
	if (NULL == (pNI6250->pSfwTree = sfwTreeInit (pSTDdev->taskName))) {
		kuDebug (kuERR, "%s : sfwTreeInit() failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	// SEG-2. setup general configurations
	if (sfwTreeOk != (eReturn = sfwTreeSetLocalFileHome (pNI6250->pSfwTree, STR_LOCAL_SEG_DATA_DIR))) {
		kuDebug (kuERR, "%s : cannot set home directory for local data file\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	// SEG-2.
	if (sfwTreeOk != (eReturn = sfwTreeSetPutType (pNI6250->pSfwTree, sfwTreePutSegment))) {
		kuDebug (kuERR, "%s : cannot set type of storing data\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#endif	// VBS_SFW_TREE_USE

	return WR_OK;
}

static long drvNI6250_init_driver(void)
{
	kLog (K_TRACE, "[drvNI6250_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_NI6250_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvNI6250_init_driver] %s : create_NI6250_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvNI6250_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNI6250_io_report(int level)
{
	kLog (K_TRACE, "[drvNI6250_io_report] ...\n");

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

