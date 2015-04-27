#include <fcntl.h>	// for mkdir

#include "drvNIDAQbase.h"

static long drvNIDAQ_io_report(int level);
static long drvNIDAQ_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNIDAQ = {
	2,
	drvNIDAQ_io_report,
	drvNIDAQ_init_driver
};

epicsExportAddress(drvet, drvNIDAQ);

// sleep time of DAQ thread (milli seconds)
int kDAQSleepMillis	= 1000;
epicsExportAddress (int, kDAQSleepMillis);

int kAutoTrendRunWaitMillis	= 3000;
epicsExportAddress (int, kAutoTrendRunWaitMillis);

int n8After_Processing_Flag		= 0; 

int gbCalcRunMode  			= 1;	// 0 : Trend mode, 1 : Calcuration & store mode
int gbAutoTrendRunMode			= 1;	// 1 : Automatically start DAQ for trending

int NI_err_message(char *fname, int lineNo, int error)
{
	char	errBuff[2048]={'\0'};

	if ( DAQmxFailed(error) ) {
		DAQmxBaseGetExtendedErrorInfo(errBuff,sizeof(errBuff));
		kLog (K_ERR, "[%s:%d] DAQmxBase Error: %s\n", fname, lineNo, errBuff);
		notify_error (1, "DAQmxBase Error: %s\n", errBuff);

		epicsThreadSleep(1);
	}

	return (0);
}

int isCalcRunMode (ST_STD_device *pSTDdev)
{
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	return (!pNIDAQ->trend_run_flag);
}
/* TG
int isCalcRunMode ()
{
	return (gbCalcRunMode);
}
*/
/* TG
void setCalcRunMode (int bCalcRunMode)
{
	kLog (K_MON, "[setCalcRunMode] %d\n", bCalcRunMode);
	gbCalcRunMode = bCalcRunMode; 
}
*/
/* TG
int isAutoTrendRunMode ()
{
	return ( (get_master()->ST_Base.opMode == OPMODE_REMOTE) && (TRUE == gbAutoTrendRunMode) );
}
*/
/* TG
void setAutoTrendRunMode (int bAutoTrendRunMode)
{
	gbAutoTrendRunMode = bAutoTrendRunMode;
}
*/
int isFirstDevice (ST_STD_device *pSTDdev)
{
	return (0 == pSTDdev->BoardID);
}

ST_NIDAQ *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NIDAQ *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NIDAQ *pNIDAQ) 
{ 
//	return (pNIDAQ->sample_rate / 10); 
	return (pNIDAQ->sample_rate); // Test every event per 10msc 
}

int getArraySizeInBytes (ST_NIDAQ *pNIDAQ) 
{
   	return (sizeof(pNIDAQ->readArray)); 
}

int getMaxNumChan (ST_STD_device *pSTDdev) 
{
// TGLee 
	return (ellCount(pSTDdev->pList_Channel));
/* 	return (INIT_MAX_CHAN_NUM);   */
}

float64 getTotSampsChan (ST_NIDAQ *pNIDAQ) 
{ 
	return (pNIDAQ->sample_rate * pNIDAQ->sample_time); 
}

int getTotSampsAllChan (ST_STD_device *pSTDdev) 
{
ST_NIDAQ                *pNIDAQ = pSTDdev->pUser; 
	return (pNIDAQ->sample_rate * getMaxNumChan(pSTDdev)); 
}

int getBeamPulseTime (ST_NIDAQ *pNIDAQ) 
{ 
	return (pNIDAQ->beam_pulse); 
}

int getSampRate (ST_NIDAQ *pNIDAQ) 
{ 
	return (pNIDAQ->sample_rate); 
}

float64 getSampTime (ST_NIDAQ *pNIDAQ) 
{ 
	return (pNIDAQ->sample_time); 
}

/* void setSamplTime (ST_NIDAQ *pNIDAQ) */
void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_NIDAQ *pNIDAQ = NULL;
	pNIDAQ = pSTDdev->pUser;

/*	pNIDAQ->sample_time = (pNIDAQ->stop_t1 <= pNIDAQ->start_t0) ? 0 : (pNIDAQ->stop_t1 - pNIDAQ->start_t0);   */

	if(pNIDAQ->stop_t1 <= pNIDAQ->start_t0) {
		pNIDAQ->sample_time = 0;
	} else {
		pNIDAQ->sample_time = pNIDAQ->stop_t1 - pNIDAQ->start_t0;
	}

	kLog (K_INFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pNIDAQ->start_t0, pNIDAQ->stop_t1, pNIDAQ->sample_time);
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}
void setStartT0 (ST_NIDAQ *pNIDAQ, float64 val) 
{
	pNIDAQ->start_t0 = val;
	pNIDAQ->sample_time = (pNIDAQ->stop_t1 <= pNIDAQ->start_t0) ? 0 : (pNIDAQ->stop_t1 - pNIDAQ->start_t0);
/*	setSamplTime (pNIDAQ);  */
}

void setStopT1 (ST_NIDAQ *pNIDAQ, float64 val)
{
	pNIDAQ->stop_t1 = val;
	pNIDAQ->sample_time = (pNIDAQ->stop_t1 <= pNIDAQ->start_t0) ? 0 : (pNIDAQ->stop_t1 - pNIDAQ->start_t0);
/*	setSamplTime (pNIDAQ);  */
}

void armingDeviceParams (ST_STD_device *pSTDdev)
{
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	int		i;
	int 		chanNum;
	float64	sampRate	= getSampRate (pNIDAQ);
	float64	pulseLength	= getBeamPulseTime (pNIDAQ);
	float64	tempVal		= NBI_POWER_CONST / pulseLength / sampRate;

	pNIDAQ->totalRead	= 0;
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
	chanNum = ellCount(pSTDdev->pList_Channel); 

	for(i=0;i<chanNum;i++) {
		pNIDAQ->ST_Ch[i].offset		= 0;
		pNIDAQ->ST_Ch[i].power			= 0;
		pNIDAQ->ST_Ch[i].accum_temp	= 0;
		pNIDAQ->ST_Ch[i].accum_cnt		= 0;
		pNIDAQ->ST_Ch[i].integral_cnt	= pNIDAQ->ST_Ch[i].integral_time * sampRate;
		pNIDAQ->ST_Ch[i].coefficient	= pNIDAQ->ST_Ch[i].flow * tempVal;

		kLog (K_DEBUG, "[armingDeviceParams] %s : ch(%d) time(%f) cnt(%f) flow(%f) coeff(%f) \n",
				pNIDAQ->ST_Ch[i].physical_channel, i,
				pNIDAQ->ST_Ch[i].integral_time, pNIDAQ->ST_Ch[i].integral_cnt,
				pNIDAQ->ST_Ch[i].flow, pNIDAQ->ST_Ch[i].coefficient);
	}
}

/* TG

int processInputData (int bCalcRunMode, ST_NIDAQ *pNIDAQ, int chNo, intype rawValue)
{
	ST_NIDAQ_channel *pChInfo	= &pNIDAQ->ST_Ch[chNo];

	// raw input value
	pChInfo->rawValue	= rawValue;

	// eu conversion
	// -----------------------------------------------
	// EU(V)  = Raw * (EU_H - EU_L) / (Raw_H - Raw_L)
	// EU(.C) = EU(V) * NBI_TEMP_PER_VOLT
	// -----------------------------------------------
	// temp function -> pChInfo->euValue	= NBI_TEMP_PER_VOLT * rawValue * EU_GRADIENT;
	pChInfo->euValue	= rawValue * EU_GRADIENT14;

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
*/
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
	ST_NIDAQ	*pNIDAQ = getDriver(pSTDdev);
	// when trend_run_flag value is 1, then we gethering data until stop DAQ.
	return ( isCalcRunMode(pSTDdev) ? (pNIDAQ->totalRead >= getTotSampsChan(pNIDAQ)) : FALSE );
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
	// we need trigger when trend_run_flag value is 0 (!trend_run_flag)  
	return ( isTriggerRequiredMode(pSTDdev->ST_Base.opMode) && isCalcRunMode(pSTDdev) ); 

}
/*
int isAutoRunMode (ST_STD_device *pSTDdev)
{
	// I don't care operation mode. we always run after shot.
	return ( !isCalcRunMode(pSTDdev) );
	//return ( (OPMODE_REMOTE != pSTDdev->ST_Base.opMode) || !isCalcRunMode(pSTDdev) );
}
*/
int isRemoteRunning (ST_STD_device *pSTDdev)
{
	ST_MASTER *pMaster = get_master();

	return (	(pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
			( (pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ||
			  (pMaster->StatusAdmin & TASK_IN_PROGRESS) ||
			  (pMaster->StatusAdmin & TASK_POST_PROCESS) ||
			  (pMaster->StatusAdmin & TASK_DATA_TRANSFER) ) );
}

void make_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);

	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	int i;
	int chanNum;
/*  Remove this function TGLee. I just over write same file
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);
*/
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	for(i=0;i<chanNum;i++) {
		pNIDAQ->ST_Ch[i].write_fp = NULL;

		if (CH_USED == pNIDAQ->ST_Ch[i].used) {
/*  Remove this function TGLee. I just over write same file
			sprintf (pNIDAQ->ST_Ch[i].path_name,"%sS%06d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pNIDAQ->ST_Ch[i].file_name);
*/
			sprintf (pNIDAQ->ST_Ch[i].path_name,"%s%s",
                                        STR_LOCAL_DATA_DIR, pNIDAQ->ST_Ch[i].file_name);
			kLog (K_INFO, "[make_local_file] fname(%s)\n", pNIDAQ->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pNIDAQ->ST_Ch[i].write_fp = fopen(pNIDAQ->ST_Ch[i].path_name, "w+"))) {
				kLog (K_ERR, "[make_local_file] %s create failed!!!\n", pNIDAQ->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pNIDAQ->ST_Ch[i].path_name);
			} else kLog (K_INFO, "[make_local_file] fname(%s) sucessed\n", pNIDAQ->ST_Ch[i].path_name);
#if DENSITY 
			// remove to find segmentation fault
			if (pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {
				pNIDAQ->ST_Ch[i].write_den_fp = NULL;
				sprintf (pNIDAQ->ST_Ch[i].path_den_name, "%s%s",pNIDAQ->ST_Ch[i].path_name, "Density");
				kLog (K_INFO, "[make_local_file] density fname(%s)\n", pNIDAQ->ST_Ch[i].path_den_name);
				if (NULL == (pNIDAQ->ST_Ch[i].write_den_fp = fopen(pNIDAQ->ST_Ch[i].path_den_name, "w+"))) {
					kLog (K_ERR, "[make_local_file] density %s create failed!!!\n", pNIDAQ->ST_Ch[i].path_den_name);
					notify_error (1, "%s create density file failed!\n", pNIDAQ->ST_Ch[i].path_den_name);
				}
			}
#endif
		}
	}
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	int i;
	int chanNum;
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);


	kLog (K_TRACE, "[flush_local_file] %s . channel %d..\n", pSTDdev->taskName, chanNum);

	for(i=0;i<chanNum;i++) {
		if (NULL != pNIDAQ->ST_Ch[i].write_fp) {
			fflush(pNIDAQ->ST_Ch[i].write_fp);
		}
#if DENSITY 
		// remove to find segmentation fault
		if (pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {
			if (NULL != pNIDAQ->ST_Ch[i].write_den_fp) {
				fflush(pNIDAQ->ST_Ch[i].write_den_fp);
			}
		}
#endif
	}
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	int i;
	int chanNum;
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	kLog (K_TRACE, "[close_local_file] %s .channel %d..\n", pSTDdev->taskName, chanNum);
	for(i=0;i<chanNum;i++) {
		if (NULL != pNIDAQ->ST_Ch[i].write_fp) {
			fclose(pNIDAQ->ST_Ch[i].write_fp);
			pNIDAQ->ST_Ch[i].write_fp	= NULL;
		}
#if DENSITY
		// remove to find segmentation fault
		if (pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {
			if (NULL != pNIDAQ->ST_Ch[i].write_den_fp) {
				fclose(pNIDAQ->ST_Ch[i].write_den_fp);
				pNIDAQ->ST_Ch[i].write_den_fp	= NULL;
			}
		}
#endif
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
	// local tree is created by ECH_DAQ

	kLog (K_MON, "[createLocalMdsTree] %s : local tree is created by ECH\n", pSTDdev->taskName);

	return (WR_OK);
#endif
}

int stopDevice (ST_STD_device *pSTDdev)
{
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;

	if (0 != pNIDAQ->taskHandle) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNIDAQ->totalRead);
		NIERROR(DAQmxBaseStopTask(pNIDAQ->taskHandle));

		kLog (K_INFO, "[threadFunc_NIDAQ_DAQ] before DAQmxBaseClearTask for %s\n", pSTDdev->taskName);
		NIERROR(DAQmxBaseClearTask(pNIDAQ->taskHandle));
		kLog (K_INFO, "[threadFunc_NIDAQ_DAQ] after  DAQmxBaseClearTask for %s\n", pSTDdev->taskName);
		pNIDAQ->taskHandle = 0;
		
		epicsThreadSleep (0.5);
		if (pNIDAQ->protect_stop_flag == 1) {
			kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			pNIDAQ->protect_stop_flag = 0;
			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);

			// wait some times to synchronize running mode with device threads
			//epicsThreadSleep (0.5);
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

void armingDevice (ST_STD_device* pSTDdev)
{
	ST_MASTER *pMaster	= get_master();

	kLog (K_MON, "[armingDevice] %s : mode(%d)\n", pSTDdev->taskName, isCalcRunMode(pSTDdev));

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

	if (TRUE == isCalcRunMode(pSTDdev)) {
		flush_STDdev_to_MDSfetch (pSTDdev);

		make_local_file (pSTDdev);

	}
}
/* TG
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
*/
/* TG
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
*/
/* TG
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
// makes name of tag for raw value
int drvNIDAQ_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	int chanNum;
	ST_NIDAQ *pNIDAQ = (ST_NIDAQ *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvNIDAQ_set_TagName] ...\n");
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	for(i=0; i<chanNum; i++) {
		sprintf(pNIDAQ->ST_Ch[i].strTagName, "\\%s:FOO", pNIDAQ->ST_Ch[i].inputTagName);
		kLog (K_INFO, "%s\n", pNIDAQ->ST_Ch[i].strTagName);
	}

	return WR_OK;
}

// starts DAQ for trending : this funciton call after shot by sfw.
void func_NIDAQ_POST_SEQSTOP(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[POST_SEQSTOP] %s : value(%d) status(%d)\n", pSTDdev->taskName, (int)arg1, (int)arg2);

	if (0 == (int)arg1) {
		if (TRUE != isRemoteRunning (pSTDdev)) {
			if (OPMODE_REMOTE == pSTDdev->ST_Base.opMode) {
				// TGL remote mode and after shot sequence terminated. we doing trend run mode setting, arming, running until next shot seqence start
/*

				DBproc_put(PV_TREND_RUN_STR, "1");
				epicsThreadSleep(0.5);
				DBproc_put(PV_SYS_ARMING_STR, "1");
				epicsThreadSleep(0.5);
				DBproc_put(PV_SYS_RUN_STR, "1");
*/
			}
		}
		else {
			kLog (K_INFO, "[POST_SEQSTOP] %s : DAQ is running in remote mode. we cannot do trend mode.\n", pSTDdev->taskName);
		}
	}
}

// stops DAQ to ready shot start
void func_NIDAQ_PRE_SEQSTART(void *pArg, double arg1, double arg2)
{
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	if (TRUE != isFirstDevice(pSTDdev)) {
		return;
	}

	kLog (K_MON, "[PRE_SEQSTART] %s : setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		if (TRUE == isRemoteRunning (pSTDdev)) {
			if (OPMODE_REMOTE == pSTDdev->ST_Base.opMode) {
				// TGL remote mode and after shot sequence terminated. we doing trend run mode setting, arming, running until next shot seqence start
				//run stop, arming stop, trend mode disable for shot data store in MDSplus
/*
				DBproc_put(PV_SYS_RUN_STR, "0");
				epicsThreadSleep(0.1);
				DBproc_put(PV_SYS_ARMING_STR, "0");
				epicsThreadSleep(0.5);
				DBproc_put(PV_TREND_RUN_STR, "0");
				epicsThreadSleep(0.5);
				DBproc_put(PV_SYS_ARMING_STR, "1");
*/
				/* rearming in this time because this function called a few m-sec before seqstart function. so, I have to do the sys arming function instead of seqstart function call */
			}
		}
		else {
			kLog (K_INFO, "[PRE_SEQSTOP] %s : DAQ is not running in remote mode. we do not need pre sequence.\n", pSTDdev->taskName);
		}
	}
}

void func_NIDAQ_SYS_RESET(void *pArg, double arg1, double arg2)
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

void func_NIDAQ_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER               *pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQ *pNIDAQ = (ST_NIDAQ *)pSTDdev->pUser;
	int i;
	int sampleRate;
	int chanNum;
	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	sampleRate = pNIDAQ->sample_rate;

	if ((OPMODE_REMOTE == pSTDdev->ST_Base.opMode) && (pNIDAQ->auto_run_flag == 0)) {
		kLog (K_ERR, "[SYS_ARMING] %s : arming failed.Because Remoter mode slececded, but not yet setting done(not auto run mode)\n",
					pSTDdev->taskName);
		return;
	} else if (1 == (int)arg1) { /* in case of arming  */
		if (admin_check_Arming_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : arming failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		armingDevice (pSTDdev);

#if 0 
		if (TRUE == pNIDAQ->trend_run_flag) {
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
			for (i=0;i<chanNum;i++) {
#if DENSITY
				if(pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {  /* init density data channel */
					pNIDAQ->ST_Ch[i].isFirst = 0;	
					pNIDAQ->ST_Ch[i].density = 0;
					for(j=0; j<sampleRate; j++) {
						pNIDAQ->ST_Ch[i].calcDensity[j] = 0;	
					}
					scanIoRequest(pSTDdev->ioScanPvt_userCall);
				}
#endif
			}
			return;
		}
		for (i=0;i<chanNum;i++) {
#if DENSITY
			if(pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {  /* init density data channel */
				pNIDAQ->ST_Ch[i].isFirst = 0;	
				pNIDAQ->ST_Ch[i].density = 0;
				for(j=0; j<sampleRate; j++) {
					pNIDAQ->ST_Ch[i].calcDensity[j] = 0;	
				}
				scanIoRequest(pSTDdev->ioScanPvt_userCall);
			}
#endif
		}

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NIDAQ_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQ *pNIDAQ = (ST_NIDAQ *)pSTDdev->pUser;

	kLog (K_MON, "[SYS_RUN] %s : %d\n", pSTDdev->taskName, (int)arg1);

	if ((OPMODE_REMOTE == pSTDdev->ST_Base.opMode) && (pNIDAQ->auto_run_flag == 0)) {
		kLog (K_ERR, "[SYS_RUN] %s : arming failed.Because Remoter mode slececded, but not yet setting done(not auto run mode)\n",
					pSTDdev->taskName);
		return;
	} else if (1 == (int)arg1) {
		if (admin_check_Run_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_RUN] %s : run failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: run failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
	} else { /* release sys run - stop when pvPut value 0 */
		stopDevice(pSTDdev);
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NIDAQ_OP_MODE(void *pArg, double arg1, double arg2)
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
void func_NIDAQ_DATA_SEND(void *pArg, double arg1, double arg2)
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
/*
void func_NIDAQ_CREATE_LOCAL_SHOT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_SHOT] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}
*/
void func_NIDAQ_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NIDAQ_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQ *pNIDAQ = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNIDAQ = pSTDdev->pUser;
	pNIDAQ->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

void func_NIDAQ_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQ *pNIDAQ = NULL;

	kLog (K_MON, "[func_NIDAQ_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	pNIDAQ = pSTDdev->pUser;

	pNIDAQ->start_t0 = arg1;
	setSamplTime (pSTDdev);
/*	setSamplTime (pNIDAQ);      */
/*	setStartT0 (pNIDAQ, arg1);  */
}

void func_NIDAQ_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQ *pNIDAQ = NULL;

	kLog (K_MON, "[func_NIDAQ_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	pNIDAQ = pSTDdev->pUser;

	pNIDAQ->stop_t1 = arg1;
	setSamplTime (pSTDdev);     
/*	setSamplTime (pNIDAQ);      */
/*	setStopT1 (pNIDAQ, arg1);   */
}

void threadFunc_NIDAQ_RingBuf(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	int i;
	int chanNum;

	kLog (K_TRACE, "[threadFunc_NIDAQ_RingBuf] ...\n");

	int32	sampsRead=0;
	int32	numSampsPerChan = getNumSampsPerChan(pNIDAQ);
	int32	totSampsAllChan	= getTotSampsAllChan(pSTDdev);
/* TG 	int32	totSampsAllChan	= getTotSampsAllChan(pNIDAQ);  */

	unsigned long long int writeFileStart;
	unsigned long long int writeFileStop;
	unsigned long long int readDataStart;
	unsigned long long int readDataStop;
	unsigned long long int start;
	unsigned long long int stop;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);
		if (pNIDAQ->protect_stop_flag == 1) {
			start = wf_getCurrentUsec();
			/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
			chanNum = ellCount(pSTDdev->pList_Channel);

#if !USE_SCALING_VALUE
			int32	numBytesPerSamp = 0;
			int32 	arraySizeInBytes = getArraySizeInBytes(pNIDAQ);
#endif

			kLog (K_MON, "[EveryNCallback] %s :  rate(%.f) status(%s) total(%d) mode(%d) shot(%lu/%lu)\n",
					pSTDdev->taskName, pNIDAQ->sample_rate,
					getStatusMsg(pSTDdev->StatusDev, NULL),
					pNIDAQ->totalRead, isCalcRunMode(pSTDdev), get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

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
			readDataStop = wf_getCurrentUsec();
			kLog (K_MON,"[ EveryNCallback- read DAQmxBaseReadAnalogF64 1] : measured function process time sec. is %lf msec\n",
					1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
			NIERROR(
					DAQmxBaseReadAnalogF64 (
						pNIDAQ->taskHandle, 			// taskHandle
						numSampsPerChan, 				// numSampsPerChan. number of samples per channel to read
						600.0, 							// timeout (seconds)
						DAQmx_Val_GroupByChannel,	// DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved), DAQmx_Val_GroupByChannel (non-interleaved)
						pNIDAQ->fScaleReadArray, 		// readAray. array into which samples are read
						sizeof(pNIDAQ->fScaleReadArray),	// arraySizeInBytes. size of the array into which samples are read
						(void *)&sampsRead, 			// sampsRead. actual number of bytes read into the array per scan
						NULL							// reserved
							)
						);


					/*
					   if (TRUE == isPrintDebugMsg (K_DATA)) {
					   for (i=0;i<numSampsPerChan;i++) {
					   if ( (i > 0) && ! (i % 10)) printf ("\n");
					   printf ("%.2f ", pNIDAQ->fScaleReadArray[i*chanNum]);
					   }
					   printf ("\n");
					   }
					 */
			kLog (K_MON,"[ EveryNCallback- read DAQmxBaseReadAnalogF64 2] : measured function process time sec. is %lf msec\n",
					1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
			// converts scaling eu value to raw value of 14bits resolution
			for (i=0;i<totSampsAllChan;i++) {
				pNIDAQ->readArray[i] = GET_RAW_VALUE(pNIDAQ->fScaleReadArray[i]);
				/*		pNIDAQ->readArray[i] = GET_RAW14_VALUE(pNIDAQ->fScaleReadArray[i]);   converts scaling eu value to raw value of 14bits resolution */
				/*		 converts scaling eu value to raw value of 16bits resolution
						 pNIDAQ->readArray[i] = GET_RAW_VALUE(pNIDAQ->fScaleReadArray[i]);
				 */
			}
			kLog (K_MON,"[ EveryNCallback- read DAQmxBaseReadAnalogF64 3] : measured function process time sec. is %lf msec\n",
					1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
#else
			NIERROR(
					DAQmxBaseReadBinaryI16 (
						pNIDAQ->taskHandle, 	// taskHandle
						numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
						600.0, 					// timeout (seconds)
						pNIDAQ->readArray, 	// readAray. array into which samples are read
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
					printf ("%d ", pNIDAQ->readArray[i*chanNum]);
				}
				printf ("\n");
			}

			pNIDAQ->totalRead	+= sampsRead;
			FILE *fp = NULL;
			int chNo;

			int32 arrayJump;
			int32 arrayJumpCurrent;

			/* When use this function -  DAQmx_Val_GroupByChannel (non-interleaved) at DAQmxBaseReadAnalogF64 */
			for(chNo=0;	chNo<chanNum;	chNo++) {
				arrayJump = chNo * numSampsPerChan;
				arrayJumpCurrent = (chNo +1) * numSampsPerChan - 1; /* This pupose is last value display in the pv  */

				/*		epicsPrintf("[ EveryNCallback - write raw data to local file chNu :%d ] : measured function process time sec. is %lf msec\n", 
						chNo, 1.E-3 * (double)wf_intervalUSec(writeFileStart,writeFileStop));
				 */
				pNIDAQ->ST_Ch[chNo].rawValue  = pNIDAQ->readArray[arrayJumpCurrent];
				pNIDAQ->ST_Ch[chNo].euValue  = pNIDAQ->fScaleReadArray[arrayJumpCurrent];

				writeFileStart = wf_getCurrentUsec();
				if(pNIDAQ->trend_run_flag==0){
					// protected file point value is NULL, even if trend_run_flag value is 1.
					if (NULL != (fp = pNIDAQ->ST_Ch[chNo].write_fp)) {
						fwrite(&pNIDAQ->readArray[arrayJump], sizeof(pNIDAQ->readArray[0]), numSampsPerChan, pNIDAQ->ST_Ch[chNo].write_fp);
						fflush(pNIDAQ->ST_Ch[chNo].write_fp);
					}
					writeFileStop = wf_getCurrentUsec();
#if DENSITY 
					// Remove to find about segmentation fault
					if(pNIDAQ->ST_Ch[chNo].inputType == DENSITY_CALC) {  /* Write data from bufferAllChannel to channel buffer for density channel */
						/* copy 2 channels, two channel is density channel and next channel  */
						epicsPrintf("EvenyNCallback I just Test in this function if I print then some wrong, I'am here density calc");
						memcpy(pNIDAQ->ST_Ch[chNo].fScaleReadChannel, &pNIDAQ->fScaleReadArray[arrayJump],sizeof(float64)* numSampsPerChan);
						memcpy(pNIDAQ->ST_Ch[chNo+1].fScaleReadChannel, &pNIDAQ->fScaleReadArray[(chNo+1) * numSampsPerChan],sizeof(float64)* numSampsPerChan);
						processDensity(pNIDAQ, chNo);
						writeFileStart = wf_getCurrentUsec();
						if (NULL != (fp = pNIDAQ->ST_Ch[chNo].write_den_fp)) {
							fwrite(&pNIDAQ->ST_Ch[chNo].calcDensity, sizeof(pNIDAQ->ST_Ch[chNo].calcDensity[0]), numSampsPerChan, pNIDAQ->ST_Ch[chNo].write_den_fp);
							fflush(pNIDAQ->ST_Ch[chNo].write_den_fp);
						}
						writeFileStop = wf_getCurrentUsec();
					}
#endif
				}
			}

#if 0 
			/* change write to file routine */
			for(i=0; i<numSampsPerChan; i++) {
				if (NULL != (fp = pNIDAQ->ST_Ch[chNo].write_fp)) {
					// write to file
					fwrite(&pNIDAQ->readArray[arrayJump+i], sizeof(pNIDAQ->readArray[0]), 1, pNIDAQ->ST_Ch[chNo].write_fp);
					fflush(pNIDAQ->ST_Ch[chNo].write_fp);

					if (0 == (i % 10)) {
						kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
								pSTDdev->taskName, chNo, pNIDAQ->ST_Ch[chNo].file_name, pNIDAQ->readArray[arrayJump+i]);
					}
				}
			}
#endif


#if 0
			/* When use this function -  DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved) at DAQmxBaseReadAnalogF64 */
			for (i=0;i<totSampsAllChan;i++) {
				chNo = GET_CHID(i);

				pNIDAQ->ST_Ch[chNo].rawValue  = pNIDAQ->readArray[i];
				pNIDAQ->ST_Ch[chNo].euValue  = pNIDAQ->fScaleReadArray[i];
				// offset compensation
				/* remove ... because I used not offset value */
				/*		pNIDAQ->readArray[i]	-= pNIDAQ->ST_Ch[chNo].offset;   */

				if (NULL != (fp = pNIDAQ->ST_Ch[chNo].write_fp)) {
					// write to file
					fwrite(&pNIDAQ->readArray[i], sizeof(pNIDAQ->readArray[0]), 1, pNIDAQ->ST_Ch[chNo].write_fp);
					fflush(pNIDAQ->ST_Ch[chNo].write_fp);

					if (0 == chNo) {
						kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
								pSTDdev->taskName, chNo, pNIDAQ->ST_Ch[chNo].file_name, pNIDAQ->readArray[i]);
					}
				}

				// process input data : offset compensation, eu conversion, integral
				/* remove TG.Lee processInputData (isCalcRunMode(), pNIDAQ, chNo, pNIDAQ->readArray[i]);  */
			}
#endif
			stop = wf_getCurrentUsec();
			kLog (K_INFO,"[ EveryNCallback ] : measured function process time sec. is %lf msec\n", 1.E-3 * (double)wf_intervalUSec(start,stop));

			scanIoRequest(pSTDdev->ioScanPvt_userCall);

			if (TRUE == isSamplingStop (pSTDdev)) {
				kLog (K_INFO, "[EveryNCallback] %s : stop ....\n", pSTDdev->taskName);
				stopDevice (pSTDdev);
			}
		}
	}
}

void processConstantDensity(ST_NIDAQ *pNIDAQ, int chNo)
{
	int i;
	float64 dataTemp;
	float64 conA = pNIDAQ->ST_Ch[chNo].conA;
	float64 conB = pNIDAQ->ST_Ch[chNo].conB;
	float64 conC = pNIDAQ->ST_Ch[chNo].conC;
	float64 conD = pNIDAQ->ST_Ch[chNo].conD;

	int32	numSampsPerChan = getNumSampsPerChan(pNIDAQ);
	/* Calc the density from raw scale data to density param value for each channel before density calc between 2 channels  */
	for (i=0; i<numSampsPerChan; i++)
	{	dataTemp = pNIDAQ->ST_Ch[chNo].fScaleReadChannel[i];
		pNIDAQ->ST_Ch[chNo].fScaleReadChannel[i] = conA*((conB*(dataTemp*dataTemp)) + (conC * dataTemp) + conD); 
	}
	kLog (K_INFO,"[threadFunc_processiConstantDensity_isFirst TG ] ConA : %f , ConB : %f, ConC : %f , ConD : %f , \n", conA,conB,conC,conD);
}
float64 average(ST_NIDAQ *pNIDAQ, int chNo)
{
	float64 sum = 0.0;
	int i;
	int32	numSampsPerChan = getNumSampsPerChan(pNIDAQ);
	if (numSampsPerChan > 1000) numSampsPerChan=1000;
	for (i=0; i<numSampsPerChan; i++)
	{
		sum += pNIDAQ->ST_Ch[chNo].fScaleReadChannel[i]; 
	}
	return (sum / numSampsPerChan);
}
int processDensity (ST_NIDAQ *pNIDAQ, int chNo)
{
	int selectCh;
	int i;
	unsigned long long int start;
	unsigned long long int stop;
	start = wf_getCurrentUsec();
	int32	numSampsPerChan = getNumSampsPerChan(pNIDAQ);
/* herer TG */ 
	processConstantDensity(pNIDAQ, chNo);
	processConstantDensity(pNIDAQ, chNo+1);

	/* 1. Get n-th data from reading channel */
	/* 2. Voltage is in stable range ? No, goto #3.  Yes, goto #6  */
	if(pNIDAQ->ST_Ch[chNo].isFirst == 0) {
		pNIDAQ->ST_Ch[chNo].avgNES1 = average(pNIDAQ, chNo);
		pNIDAQ->ST_Ch[chNo].avgNES2 = average(pNIDAQ, chNo+1);
		if (abs(pNIDAQ->ST_Ch[chNo].avgNES1 - 0.1) < abs(pNIDAQ->ST_Ch[chNo].avgNES2 - 0.1))
		{
			selectCh = chNo;
			pNIDAQ->ST_Ch[chNo].density =- pNIDAQ->ST_Ch[chNo].avgNES1;
		} else
		{
			selectCh = chNo+1;
			pNIDAQ->ST_Ch[chNo].density =- pNIDAQ->ST_Ch[chNo].avgNES2;
		}
		pNIDAQ->ST_Ch[chNo].isFirst = 1;
		kLog (K_INFO,"[threadFunc_processDensity_isFirst ] Select Ch : %d , Channel Number : %d \n", selectCh, chNo);
		kLog (K_INFO,"[threadFunc_processDensity_isFirst ] Density first value : %f , avgNES1 : %f,  avgNES2 : %f \n", 
				pNIDAQ->ST_Ch[chNo].density, pNIDAQ->ST_Ch[chNo].avgNES1, pNIDAQ->ST_Ch[chNo].avgNES2);
	} else {
		selectCh = pNIDAQ->ST_Ch[chNo].selectCh;
	}

	/* 3. Difference from center is smaller than another channel? No, goto #4.  Yes, goto #6  */
	/* 5. set new base density */
	for (i=0; i<numSampsPerChan; i++)
	{
		/* 2. Voltage is in stable range ? No, goto #3.  Yes, goto #6  */
		if(pNIDAQ->ST_Ch[selectCh].fScaleReadChannel[i] > pNIDAQ->ST_Ch[chNo].dLimUpper)
		{
			/* 4. change reading channel */
			if (selectCh==chNo)
			{
				selectCh=chNo+1;
			} else {
				selectCh=chNo;
			}
			/* 6. Voltage to Density Conversion */
			pNIDAQ->ST_Ch[chNo].density = pNIDAQ->ST_Ch[chNo].density + (pNIDAQ->ST_Ch[chNo].fringe / 2.);
		}
		if(pNIDAQ->ST_Ch[selectCh].fScaleReadChannel[i] < pNIDAQ->ST_Ch[chNo].dLimLow)
		{
			/* 4. change reading channel */
			if (selectCh==chNo)
			{
				selectCh=chNo+1;
			} else {
				selectCh=chNo;
			}
			/* 6. Voltage to Density Conversion */
			pNIDAQ->ST_Ch[chNo].density = pNIDAQ->ST_Ch[chNo].density - (pNIDAQ->ST_Ch[chNo].fringe / 2.);
		}
		/* 7. Confirm data */
		/* 8. End of Data - No, goto #1. Yes, Save Corrected data */
		/* modify TG 20110620. NEC(dp) = NES+NE.d(dp,CCH);so, change  */
		/* old - function TG : pNIDAQ->ST_Ch[chNo].calcDensity[i] = pNIDAQ->ST_Ch[chNo].density;	  */
		pNIDAQ->ST_Ch[chNo].calcDensity[i] = pNIDAQ->ST_Ch[chNo].density + pNIDAQ->ST_Ch[selectCh].fScaleReadChannel[i];	
		if(i==0){	
			kLog (K_INFO,"[threadFunc_processDensity_is first array[0]] Density first value : %f , CalcDensity : %f,  fringValue : %f \n", 
					pNIDAQ->ST_Ch[chNo].density, pNIDAQ->ST_Ch[chNo].calcDensity[i], pNIDAQ->ST_Ch[selectCh].fScaleReadChannel[i]);
		}
	}
	pNIDAQ->ST_Ch[chNo].selectCh = selectCh;  /* remember last select channel for density calc */
	kLog (K_INFO,"[threadFunc_processDensity_Every Event ] Select Ch : %d , Channel Number : %d \n", selectCh, chNo);
	stop = wf_getCurrentUsec();
	kLog (K_INFO,"[threadFunc_processDensity_Every Event ] : measured function process time sec. is %lf msec\n", 1.E-3 * (double)wf_intervalUSec(start,stop));
	return (0);
}
void threadFunc_NIDAQ_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NIDAQ *pNIDAQ = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	int chanNum;
	unsigned long long int start;
	unsigned long long int stop;

	int32	totSampsAllChan;
	int32	numSampsPerChan;

	kLog (K_TRACE, "[NIDAQ_DAQ] dev(%s)\n", pSTDdev->taskName);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_NIDAQ_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNIDAQ->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_MON, "[NIDAQ_DAQ] %s : mode(%s) rate(%.f) time(%f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNIDAQ->sample_rate, pNIDAQ->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_MON, "[NIDAQ_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_INFO, "[NIDAQ_DAQ] %s : mode(%s) rate(%.f) time(%f-%f = %f) beam(%.f)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNIDAQ->sample_rate,
				pNIDAQ->stop_t1, pNIDAQ->start_t0, pNIDAQ->sample_time,
				pNIDAQ->beam_pulse);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNIDAQ->taskHandle == 0)) {
			/*	processAutoTrendRun (pSTDdev);   */
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNIDAQ->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
				stopDevice (pSTDdev);

				NIERROR(DAQmxBaseCreateTask("",&pNIDAQ->taskHandle));

				kLog (K_INFO, "[NIDAQ_DAQ] %s : term_config(%d) minVal(%.f) maxVal(%.f) units(%d)\n",
						pSTDdev->taskName, pNIDAQ->terminal_config, pNIDAQ->minVal, pNIDAQ->maxVal, pNIDAQ->units);

				for (i=0;i<chanNum;i++) {
					if(pNIDAQ->ST_Ch[i].inputType == STRAIN_DATA){
						kLog (K_INFO, "[NIDAQ_DAQ] STRAIN DATA in DAQmxCreateAIStrainGageChan : ch(%d), physical_channelSCXI(%s) minVal(%f) maxVal(%f)\n",
								i,  pNIDAQ->ST_Ch[i].physical_channelSCXI, pNIDAQ->minVal, pNIDAQ->maxVal);
					} else {
						kLog (K_INFO, "[NIDAQ_DAQ] AI DATA in DAQmxBaseCreateAIVoltageChan : ch(%d), physical_channel(%s) minVal(%.f) maxVal(%f)\n",
								i,  pNIDAQ->ST_Ch[i].physical_channel, pNIDAQ->minVal, pNIDAQ->maxVal);
						NIERROR (
								DAQmxBaseCreateAIVoltageChan (
									pNIDAQ->taskHandle,			// taskHandle
									pNIDAQ->ST_Ch[i].physical_channel,	// physicalChannel
									"",					// nameToAssignToChannel
									pNIDAQ->terminal_config,		// terminalConfig
									pNIDAQ->minVal,			// minVal
									pNIDAQ->maxVal,			// maxVal
									pNIDAQ->units,				// units
									NULL					// customScaleName
									)
							);
#if DENSITY
						if(pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {  /* init density data channel */
							pNIDAQ->ST_Ch[i].isFirst = 0;	
							pNIDAQ->ST_Ch[i].density = 0;	
							scanIoRequest(pSTDdev->ioScanPvt_userCall);
						}
#endif
					}
					kLog (K_INFO, "[NIDAQ_DAQ] DAQmxBaseCreateAIVoltageChan : %s\n", pNIDAQ->ST_Ch[i].physical_channel);
				}

				totSampsAllChan	= getTotSampsAllChan(pSTDdev);
				numSampsPerChan = getNumSampsPerChan(pNIDAQ);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
							DAQmxBaseCfgSampClkTiming(
								pNIDAQ->taskHandle,	// taskHandle
								pNIDAQ->clockSource,	// source terminal of the Sample clock. NULL(internal)
								pNIDAQ->sample_rate,	// sampling rate in samples per second per channel
								DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
								pNIDAQ->smp_mode,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
								totSampsAllChan		// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
								)
					       );
				} else {
					NIERROR(
							DAQmxBaseCfgSampClkTiming(
								pNIDAQ->taskHandle,	// taskHandle
								"",			// source terminal of the Sample clock. NULL(internal)
								pNIDAQ->sample_rate,	// sampling rate in samples per second per channel
								DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
								pNIDAQ->smp_mode,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
								totSampsAllChan		// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
								)
					       );
					kLog (K_MON, "[NIDAQ_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}

				kLog (K_INFO, "[NIDAQ_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
						pSTDdev->taskName,
						getModeMsg(pSTDdev->ST_Base.opMode),
						pNIDAQ->smp_mode, pNIDAQ->sample_rate, pNIDAQ->sample_time,
						numSampsPerChan, 
						totSampsAllChan);

				if (TRUE == isTriggerRequired (pSTDdev)) {
					NIERROR(
						DAQmxBaseCfgDigEdgeStartTrig(
							pNIDAQ->taskHandle,
							pNIDAQ->triggerSource,
							DAQmx_Val_Rising
						)
					);

					kLog (K_MON, "[NIDAQ_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pNIDAQ->triggerSource);
				}
				else {
					kLog (K_MON, "[NIDAQ_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
				}
#if 0
// DAQmxBase is not support this function (DAQmxRegisterEveryNSamplesEvent)
				NIERROR(
					DAQmxRegisterEveryNSamplesEvent(
						pNIDAQ->taskHandle,		// taskHandle
						DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
						numSampsPerChan,		// nSamples. number of samples after whitch each event should occur
						0,				// options. 0(callback function is called in a DAQmx thread)
						EveryNCallback,			// callbackFunction
						(void *)pSTDdev			// callbackData. parameter to be passed to callback function
					)
				);
#endif
				armingDeviceParams (pSTDdev);

				NIERROR(DAQmxBaseStartTask(pNIDAQ->taskHandle));

				kLog (K_INFO, "[threadFunc_NIDAQ_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNIDAQ->protect_stop_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNIDAQ->protect_stop_flag = 0;

				kLog (K_INFO, "[threadFunc_NIDAQ_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				if (isCalcRunMode(pSTDdev)) {
					// data acquisition and storeing into the local files. and then transfer mdsplus
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;

				/*  DBproc_put(PV_CALC_RUN_STR, "0");  */
				}
				else {
					// data acquisiion without storing in local file and mdsplus.
					//pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					//pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					DBproc_put(PV_TREND_RUN_STR, "0");
				//	pNIDAQ->trend_run_flag = 0;
				}


				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_POST_PROCESS)) {
				flush_local_file(pSTDdev);								

				pSTDdev->StatusDev = TASK_STANDBY | TASK_DATA_TRANSFER;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_DATA_TRANSFER)) {
				if(0 == strcmp(pSTDdev->taskName,"Dev1"))
				{
					epicsThreadSleep(0.1);
					epicsPrintf("[threadFunc_NIDAQ_DAQ - write to mdsplus DB wait Dev1 0.1sec  ############### I am here\n");
				}
				//		epicsMutexLock(pMaster->lock_mds); // Sometimes Device Thread into this routine
				if(pMaster->n8MdsPutFlag == 0) {
					pMaster->n8MdsPutFlag = 1;
				//		epicsMutexUnlock(pMaster->lock_mds);

					start = wf_getCurrentUsec();
					for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
						if (proc_sendData2Tree(pSTDdev) == WR_OK) {
							pMaster->n8MdsPutFlag = 0;
							break;
						}
						epicsThreadSleep(1);
						kLog (K_INFO, "[threadFunc_NIDAQ_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}
					pMaster->n8MdsPutFlag = 0;
					stop = wf_getCurrentUsec();
					epicsPrintf("[threadFunc_NIDAQ_DAQ - write to mdsplus DB Mode (%d) ] : measured function process time sec. is %lf msec\n",
																				pSTDdev->ST_Base.opMode, 1.E-3 * (double)wf_intervalUSec(start,stop));

					epicsThreadSleep(2);
				/* SYS_ARMING and SYS_RUN PV value Return the init value sequence TG */
				/* All time return init state after end of data acquisition complate */

					DBproc_put (PV_SYS_RUN_STR, "0");	// runing 
					epicsThreadSleep(1);
					DBproc_put (PV_SYS_ARMING_STR, "0");	// arming
				
					close_local_file (pSTDdev);

					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					notify_refresh_master_status();
				}
				else if(pMaster->n8MdsPutFlag == 1) {
					kLog (K_INFO, "[threadFunc_NIDAQ_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}


void threadFunc_NIDAQ_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_NIDAQ_CatchEnd] ...\n");
}

int create_NIDAQ_taskConfig(ST_STD_device *pSTDdev)
{
	ST_NIDAQ *pNIDAQ = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int 	i;
	int	chanNum;
	pNIDAQ = (ST_NIDAQ*) calloc(1, sizeof(ST_NIDAQ));
	if(!pNIDAQ) return WR_ERROR;
	
	kLog (K_TRACE, "[create_NIDAQ_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s) clock(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2, pSTDdev->strArg3);

	pNIDAQ->taskHandle = 0x0;
	pNIDAQ->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNIDAQ->data_buf) {
		free (pNIDAQ);
		pNIDAQ = NULL;
		return WR_ERROR;
	}

	ellInit(pNIDAQ->data_buf);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);
	kLog (K_TRACE, "[create_NIDAQ_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s) clock(%s) channelCounts(%d)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2, pSTDdev->strArg3, chanNum);

	for (i=0;i<chanNum;i++) {
		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			sprintf (pNIDAQ->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
	// TGL. I will do not used physical_channel info from createDevice argement. 
	//		sprintf (pNIDAQ->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);
	// TGL. Direct physical_channel data put from createChannel argements. Because sometimes physical_channel is not regular like VVS, FUEL_DAQ
			sprintf (pNIDAQ->ST_Ch[i].physical_channel, "%s", pSTDCh->strArg2);
			pNIDAQ->ST_Ch[i].used = CH_USED;

			strcpy (pNIDAQ->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			if (0 == strcmp (NBI_INPUT_TYPE_DT_STR, pSTDCh->strArg0)) {
				pNIDAQ->ST_Ch[i].inputType	= NBI_INPUT_DELTA_T;
			}
			else if (0 == strcmp (NBI_INPUT_TYPE_TC_STR, pSTDCh->strArg0)) {
				pNIDAQ->ST_Ch[i].inputType	= NBI_INPUT_TC;
			}
			else if (0 == strcmp (STRAIN_STR, pSTDCh->strArg0)) {
				/* I consider min/max value all channel of same value */
				sprintf (pNIDAQ->ST_Ch[i].physical_channelSCXI, "%s", pSTDCh->strArg2);
				pNIDAQ->ST_Ch[i].inputType = STRAIN_DATA;
				pNIDAQ->minVal = -0.015;
				pNIDAQ->maxVal = 0.015;
					
			}
			else if (0 == strcmp (DENSITY_CALC_STR, pSTDCh->strArg2)) {
				pNIDAQ->ST_Ch[i].inputType = DENSITY_CALC;
				pNIDAQ->ST_Ch[i].isFirst = 0;
				pNIDAQ->ST_Ch[i].density = 0;
				pNIDAQ->ST_Ch[i].fringe = 0.2078;
				pNIDAQ->ST_Ch[i].dLimUpper = 0.16;
				pNIDAQ->ST_Ch[i].dLimLow = 0.05;
			}
			else if (0 == strcmp (MDS_PARAM_PUT_STR, pSTDCh->strArg2)) {
				pNIDAQ->ST_Ch[i].inputType = MDS_PARAM_PUT;
			}
			else {
				pNIDAQ->ST_Ch[i].inputType	= OTHER_INPUT_NONE;
			}

			kLog (K_INFO, "[create_NIDAQ_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pNIDAQ->ST_Ch[i].physical_channel,
					pNIDAQ->ST_Ch[i].inputType, pNIDAQ->ST_Ch[i].inputTagName,
					pNIDAQ->ST_Ch[i].file_name);
		}
	}

	sprintf (pNIDAQ->triggerSource, "%s", pSTDdev->strArg1); 
	sprintf (pNIDAQ->clockSource, "%s", pSTDdev->strArg3); 

	pNIDAQ->terminal_config			= DAQmx_Val_Diff;	// DAQmx_Val_Diff(6133,6259), DAQmx_Val_NRSE NI-6254&NI6220(DAQmx_Val_RSE)
	if(pNIDAQ->ST_Ch[0].inputType != STRAIN_DATA){
		pNIDAQ->minVal			= INIT_LOW_LIMIT;
		pNIDAQ->maxVal			= INIT_HIGH_LIMIT;
	}
	pNIDAQ->sample_rate			= INIT_SAMPLE_RATE;
	pNIDAQ->sample_rateLimit		= INIT_SAMPLE_RATE_LIMIT;
	pNIDAQ->sample_time			= getSampTime(pNIDAQ);
	pNIDAQ->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNIDAQ->units				= DAQmx_Val_Volts;

	pSTDdev->pUser				= pNIDAQ;

	pSTDdev->ST_Func._OP_MODE		= func_NIDAQ_OP_MODE;
// remove	pSTDdev->ST_Func._CREATE_LOCAL_SHOT	= func_NIDAQ_CREATE_LOCAL_SHOT;
	pSTDdev->ST_Func._SYS_ARMING		= func_NIDAQ_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN		= func_NIDAQ_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_NIDAQ_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET		= func_NIDAQ_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0		= func_NIDAQ_T0;
	pSTDdev->ST_Func._SYS_T1		= func_NIDAQ_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NIDAQ_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NIDAQ_SHOT_NUMBER;

	pSTDdev->ST_Func._POST_SEQSTOP		= func_NIDAQ_POST_SEQSTOP; 
	pSTDdev->ST_Func._PRE_SEQSTART		= func_NIDAQ_PRE_SEQSTART;

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NIDAQ_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 1
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NIDAQ_RingBuf;
	//Remove TG pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NIDAQ_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvNIDAQ_init_driver(void)
{
	kLog (K_TRACE, "[drvNIDAQ_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while (pSTDdev) {
		if (create_NIDAQ_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvNIDAQ_init_driver] %s : create_NIDAQ_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvNIDAQ_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNIDAQ_io_report(int level)
{
	kLog (K_TRACE, "[drvNIDAQ_io_report] ...\n");
	
	int i;	
	int chanNum;
	ST_STD_device *pSTDdev = NULL;
	ST_MASTER *pMaster = get_master();
	ST_NIDAQ	*pNIDAQ = NULL;

	if (!pMaster) return -1;

	if (ellCount(pMaster->pList_DeviceTask)) {
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
		pNIDAQ = pSTDdev->pUser;
	}
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pMaster->pList_DeviceTask));

	if (level<1) return -1;

	while (pSTDdev) {
        	chanNum = ellCount(pSTDdev->pList_Channel);
		pNIDAQ = pSTDdev->pUser;
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, (unsigned int)pSTDdev, pSTDdev->StatusDev );

		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3) {
			
			epicsPrintf("   status of Device Channel information (rpointer)\n");
			epicsPrintf("   ");
			epicsPrintf("\n");
#if 1 
			for(i=0;i<chanNum;i++) {
				if(pNIDAQ->ST_Ch[i].inputType == DENSITY_CALC) {
					epicsPrintf(" Ch No:%d, Tag Name : %s, selectCh : %f, density : %f, fringe : %f, dLimUpper :%f, dLimLow : %f \n",
					i, pNIDAQ->ST_Ch[i].strTagName,pNIDAQ->ST_Ch[i].selectCh, pNIDAQ->ST_Ch[i].density, pNIDAQ->ST_Ch[i].fringe, pNIDAQ->ST_Ch[i].dLimUpper, pNIDAQ->ST_Ch[i].dLimLow);
					epicsPrintf("\n");
					epicsPrintf(" Constant A : %f, Constant B : %f, Constant C : %f, Constant D : %f, avgNES1 :%f, avgNES2 : %f \n",
					pNIDAQ->ST_Ch[i].conA, pNIDAQ->ST_Ch[i].conB, pNIDAQ->ST_Ch[i].conC, pNIDAQ->ST_Ch[i].conD, pNIDAQ->ST_Ch[i].avgNES1, pNIDAQ->ST_Ch[i].avgNES2);
					epicsPrintf("\n");
				} else {
					epicsPrintf(" Ch No:%d, ChannelPhy:%s, Tag Name : %s, Store Select : %d \n",
					      i, pNIDAQ->ST_Ch[i].physical_channel, pNIDAQ->ST_Ch[i].strTagName, pNIDAQ->ST_Ch[i].saveToMds);
					epicsPrintf("\n");
					
				}
			}
#endif
			epicsPrintf("   ");
			epicsPrintf("\n");
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

