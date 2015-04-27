#include "acqHTstream.h"
#include "drvACQ196.h"


static long drvACQ196_io_report(int level);
static long drvACQ196_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvACQ196 = {
	2,
	drvACQ196_io_report,
	drvACQ196_init_driver
};

epicsExportAddress(drvet, drvACQ196);

// sleep time of DAQ thread (milli seconds)
int kDAQSleepMillis	= 1000;
epicsExportAddress (int, kDAQSleepMillis);

int kAutoTrendRunWaitMillis	= 3000;
epicsExportAddress (int, kAutoTrendRunWaitMillis);

int n8After_Processing_Flag		= 0; 

int gbCalcRunMode  			= 1;	// 0 : Trend mode, 1 : Calcuration & store mode
int gbAutoTrendRunMode			= 1;	// 1 : Automatically start DAQ for trending


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

ST_ACQ196 *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_ACQ196 *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_ACQ196 *pACQ196) 
{ 
	return (pACQ196->sample_rate / 10); 
//	return (pACQ196->sample_rate); // Test every event per 10msc 
}

int getArraySizeInBytes (ST_ACQ196 *pACQ196) 
{
   	return (sizeof(pACQ196->readArray)); 
}

int getMaxNumChan (ST_STD_device *pSTDdev) 
{
// TGLee 
	return (ellCount(pSTDdev->pList_Channel));
/* 	return (INIT_MAX_CHAN_NUM);   */
}

float64 getTotSampsChan (ST_ACQ196 *pACQ196) 
{ 
	return (pACQ196->sample_rate * pACQ196->sample_time); 
}

int getTotSampsAllChan (ST_STD_device *pSTDdev) 
{
ST_ACQ196                *pACQ196 = pSTDdev->pUser; 
	return (pACQ196->sample_rate * getMaxNumChan(pSTDdev)); 
}

int getSampRate (ST_ACQ196 *pACQ196) 
{ 
	return (pACQ196->sample_rate); 
}

float64 getSampTime (ST_ACQ196 *pACQ196) 
{ 
	return (pACQ196->sample_time); 
}

/* void setSamplTime (ST_ACQ196 *pACQ196) */
void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_ACQ196 *pACQ196 = NULL;
	pACQ196 = pSTDdev->pUser;

/*	pACQ196->sample_time = (pACQ196->stop_t1 <= pACQ196->start_t0) ? 0 : (pACQ196->stop_t1 - pACQ196->start_t0);   */

	if(pACQ196->stop_t1 <= pACQ196->start_t0) {
		pACQ196->sample_time = 0;
	} else {
		pACQ196->sample_time = pACQ196->stop_t1 - pACQ196->start_t0;
	}

	kLog (K_INFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pACQ196->start_t0, pACQ196->stop_t1, pACQ196->sample_time);
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}
void setStartT0 (ST_ACQ196 *pACQ196, float64 val) 
{
	pACQ196->start_t0 = val;
	pACQ196->sample_time = (pACQ196->stop_t1 <= pACQ196->start_t0) ? 0 : (pACQ196->stop_t1 - pACQ196->start_t0);
/*	setSamplTime (pACQ196);  */
}

void setStopT1 (ST_ACQ196 *pACQ196, float64 val)
{
	pACQ196->stop_t1 = val;
	pACQ196->sample_time = (pACQ196->stop_t1 <= pACQ196->start_t0) ? 0 : (pACQ196->stop_t1 - pACQ196->start_t0);
/*	setSamplTime (pACQ196);  */
}

void armingDeviceParams (ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int		i;
	int 		chanNum;
	float64	sampRate	= getSampRate (pACQ196);

	pACQ196->totalRead	= 0;
	pACQ196->ibuffRead      = 0;
        pACQ196->nbuffRead      = 0;
        pACQ196->nsamplesRead   = 0;
        pACQ196->htimeRead      = 0.0;
        pACQ196->fileHandling   = 1;    // file index for process data, It  is important
	chanNum = ellCount(pSTDdev->pList_Channel); 

	for(i=0;i<chanNum;i++) {
		pACQ196->ST_Ch[i].offset		= 0;
		pACQ196->ST_Ch[i].power			= 0;
		pACQ196->ST_Ch[i].accum_temp	= 0;
		pACQ196->ST_Ch[i].accum_cnt		= 0;
		pACQ196->ST_Ch[i].integral_cnt	= pACQ196->ST_Ch[i].integral_time * sampRate;

		kLog (K_DEBUG, "[armingDeviceParams] %s : ch(%d) time(%f) cnt(%f) flow(%f) coeff(%f) \n",
				pACQ196->ST_Ch[i].physical_channel, i,
				pACQ196->ST_Ch[i].integral_time, pACQ196->ST_Ch[i].integral_cnt,
				pACQ196->ST_Ch[i].flow, pACQ196->ST_Ch[i].coefficient);
	}
}



int processInputData (int bCalcRunMode, ST_ACQ196 *pACQ196, int chNo, intype rawValue)
{
	ST_ACQ196_channel *pChInfo	= &pACQ196->ST_Ch[chNo];

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
	ST_ACQ196	*pACQ196 = getDriver(pSTDdev);

	return ( isCalcRunMode() ? (pACQ196->totalRead >= getTotSampsChan(pACQ196)) : FALSE );
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
	return ( isTriggerRequiredMode(pSTDdev->ST_Base.opMode) && isCalcRunMode() );
}

int isAutoRunMode (ST_STD_device *pSTDdev)
{
	return ( (OPMODE_REMOTE != pSTDdev->ST_Base.opMode) || !isCalcRunMode() );
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

	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int i;
//	int chanNum;
/*  Remove this function TGLee. I just over write same file
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);
*/
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
// someError        chanNum = ellCount(pSTDdev->pList_Channel);

	for(i=0;i<96;i++) {
		pACQ196->ST_Ch[i].write_fp = NULL;

		if (CH_USED == pACQ196->ST_Ch[i].used) {
/*  Remove this function TGLee. I just over write same file
			sprintf (pACQ196->ST_Ch[i].path_name,"%sS%06d/%s",
					STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pACQ196->ST_Ch[i].file_name);
*/
			sprintf (pACQ196->ST_Ch[i].path_name,"%s%s",
                                        STR_LOCAL_DATA_DIR, pACQ196->ST_Ch[i].file_name);
			kLog (K_INFO, "[make_local_file] fname(%s)\n", pACQ196->ST_Ch[i].path_name);

			// read & write (trunc)
			if (NULL == (pACQ196->ST_Ch[i].write_fp = fopen(pACQ196->ST_Ch[i].path_name, "w+"))) {
				kLog (K_ERR, "[make_local_file] %s create failed!!!\n", pACQ196->ST_Ch[i].path_name);
				notify_error (1, "%s create failed!\n", pACQ196->ST_Ch[i].path_name);
			}
		}
	}
	pACQ196->writeRaw_fp = NULL;
	sprintf (pACQ196->path_name,"%sB%d_Raw", STR_LOCAL_DATA_DIR, pSTDdev->BoardID );
	kLog (K_INFO, "[make_local_file] fname(%s)\n", pACQ196->path_name);
	if (NULL == (pACQ196->writeRaw_fp = fopen(pACQ196->path_name, "w+"))) {
		kLog (K_ERR, "[make_local_file] %s writeRaw_fp create failed!!!\n", pACQ196->path_name);
		notify_error (1, "%s create failed!\n", pACQ196->path_name);
	}
	
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int i;
//	int chanNum;
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
//        chanNum = ellCount(pSTDdev->pList_Channel);


	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	for(i=0;i<96;i++) {
		if (NULL != pACQ196->ST_Ch[i].write_fp) {
			fflush(pACQ196->ST_Ch[i].write_fp);
		}
	}
	if (NULL != pACQ196->writeRaw_fp) {
		fflush(pACQ196->writeRaw_fp);
	}
	
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	int i;
//	int chanNum;
	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
//        chanNum = ellCount(pSTDdev->pList_Channel);

	for(i=0;i<96;i++) {
		if (NULL != pACQ196->ST_Ch[i].write_fp) {
			fclose(pACQ196->ST_Ch[i].write_fp);
			pACQ196->ST_Ch[i].write_fp	= NULL;
		}
	}
	if (NULL != pACQ196->writeRaw_fp) {
		fclose(pACQ196->writeRaw_fp);
		pACQ196->writeRaw_fp	= NULL;
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

	kLog (K_MON, "[createLocalMdsTree] %s : local tree is created by ECEHR\n", pSTDdev->taskName);

	return (WR_OK);
#endif
}

int stopDevice (ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;

	if (0 != pACQ196->slot) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pACQ196->totalRead);

  		acq196_set_ABORT(pSTDdev);  

	

		if (pACQ196->auto_run_flag == 1) {
			kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

			epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
			pACQ196->auto_run_flag = 0;

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

void armingDevice (ST_STD_device* pSTDdev)
{
	ST_MASTER *pMaster	= get_master();

	kLog (K_MON, "[armingDevice] %s : mode(%d)\n", pSTDdev->taskName, isCalcRunMode());

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

	if (TRUE == isCalcRunMode()) {
		flush_STDdev_to_MDSfetch (pSTDdev);

		make_local_file (pSTDdev);

	}
}
void processECEposition (ST_STD_device* pSTDdev)
{
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;

	kLog (K_MON, "[processECEposition] %s : Start Calc ECE Channel Position \n", pSTDdev->taskName);

	int i;
	int chanNum;
	float64 measuredTF;
	float64 bFieldReson;

	measuredTF = pACQ196->currentTF * 1e3;    	// currentTF(kA) * 1000 = A,  default 19.64kA
        chanNum = ellCount(pSTDdev->pList_Channel);

	for(i=0; i<chanNum; i++) 
	{
		bFieldReson = (2 * PI * pACQ196->ST_Ch[i].targetFrq * ECE_ME) / ECE_E;     // Resonant B-field (T) 
		pACQ196->ST_Ch[i].posHar1st = (KSTAR_MU0 * KSTAR_TTN * measuredTF) / (2 * PI * bFieldReson); 
		pACQ196->ST_Ch[i].posHar2nd = (KSTAR_MU0 * KSTAR_TTN * measuredTF) / ((2 * PI * bFieldReson)/2); 
		pACQ196->ST_Ch[i].posHar3rd = (KSTAR_MU0 * KSTAR_TTN * measuredTF) / ((2 * PI * bFieldReson)/3); 
		pACQ196->rtTePos[i] = pACQ196->ST_Ch[i].posHar2nd;
		kLog (K_TRACE, "[processECEposition] : Ch No:%d currentTF(%f) bFieldReson(%f), position1StH(%f), position2NdH(%f), position3RdH(%f) \n",
					i, measuredTF, bFieldReson, pACQ196->ST_Ch[i].posHar1st, pACQ196->ST_Ch[i].posHar2nd, pACQ196->ST_Ch[i].posHar3rd);
	}
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
	scanIoRequest(pSTDdev->ioScanPvt_userCall1);

}
/*
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
// makes name of tag for raw value
int drvACQ196_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	int chanNum;
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvACQ196_set_TagName] ...\n");
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	for(i=0; i<chanNum; i++) {
		sprintf(pACQ196->ST_Ch[i].strTagName, "\\%s:FOO", pACQ196->ST_Ch[i].inputTagName);
		kLog (K_INFO, "%s\n", pACQ196->ST_Ch[i].strTagName);
	}

	return WR_OK;
}
// makes name of tag for raw value
int drvACQ196_set_VIN(ST_STD_device *pSTDdev)
{
	int i;
	int chanNum;
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;

	kLog (K_TRACE, "[drvACQ196_set_VIN] ...\n");
        chanNum = ellCount(pSTDdev->pList_Channel);

	FILE *pfp, *fp;
        char buff[128], *sp;
        char buffVin[2000];
        char strCmd[128];

        //char strCmd[128]={"acqcmd -s 100 get.vin > /usr/local/epics/siteApp/acq196_100_vin.txt"};
	sprintf(strCmd, "acqcmd -s %d get.vin > %s_%d", pACQ196->slot, STR_CH_VIN_FILE, pACQ196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		kLog (K_TRACE, "[drvACQ196_set_VIN] ...acqcmd -s 100 get.vin... failed \n");
		return WR_ERROR;
	} else {
		pclose(pfp);

		sprintf(buff, "%s_%d", STR_CH_VIN_FILE, pACQ196->slot);
		fp = fopen(buff,"r");
		if(fp == NULL) {
			kLog (K_TRACE, "[drvACQ196_set_VIN] .Can't open channel vin file :%s failed \n",buff);
		} else {
			if(fgets(buffVin, sizeof(buffVin),fp) !=NULL)
			{
				sp = strtok(buffVin,",");
				for(i=0; i<chanNum; i++) 
				{
					kLog (K_TRACE, "[drvACQ196_set_VIN] :Ch No:%d data value Min :%f \n",i, atof(sp));
					pACQ196->ST_Ch[i].minVoltRange = atof(sp);
					sp = strtok(NULL,",");
					pACQ196->ST_Ch[i].maxVoltRange = atof(sp);
					kLog (K_TRACE, "[drvACQ196_set_VIN] :Ch No:%d data value Max :%f \n",i, atof(sp));
					sp = strtok(NULL,",");
				}
			}
			fclose(fp);
		}

	}
	return WR_OK;
}
/*
// starts DAQ for trending
void func_ACQ196_POST_SEQSTOP(void *pArg, double arg1, double arg2)
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
void func_ACQ196_PRE_SEQSTART(void *pArg, double arg1, double arg2)
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
void func_ACQ196_SYS_RESET(void *pArg, double arg1, double arg2)
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

void func_ACQ196_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER               *pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = (ST_ACQ196 *)pSTDdev->pUser;
	int i;
	int sampleRate;
	int chanNum;
	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);


	sampleRate = pACQ196->sample_rate;
	if (1 == (int)arg1) { /* in case of arming  */
		if (admin_check_Arming_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : arming failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		processECEposition (pSTDdev);

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
			for (i=0;i<chanNum;i++) {
			}
			return;
		}

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_ACQ196_SYS_RUN(void *pArg, double arg1, double arg2)
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

void func_ACQ196_OP_MODE(void *pArg, double arg1, double arg2)
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
void func_ACQ196_DATA_SEND(void *pArg, double arg1, double arg2)
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
void func_ACQ196_CREATE_LOCAL_TREE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_TREE] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}

void func_ACQ196_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_ACQ196_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pACQ196 = pSTDdev->pUser;
	pACQ196->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

void func_ACQ196_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = NULL;

	kLog (K_MON, "[func_ACQ196_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	pACQ196 = pSTDdev->pUser;

	pACQ196->start_t0 = arg1;
	setSamplTime (pSTDdev);
/*	setSamplTime (pACQ196);      */
/*	setStartT0 (pACQ196, arg1);  */
}

void func_ACQ196_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ACQ196 *pACQ196 = NULL;

	kLog (K_MON, "[func_ACQ196_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	pACQ196 = pSTDdev->pUser;

	pACQ196->stop_t1 = arg1;
	setSamplTime (pSTDdev);     
/*	setSamplTime (pACQ196);      */
/*	setStopT1 (pACQ196, arg1);   */
}



void threadFunc_ACQ196_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	int acqStatus;
	int chanNum;
	unsigned long long int start;
	unsigned long long int stop;

//	int32	totSampsAllChan;
//	int32	numSampsPerChan;

//	ST_User_Buff_node queueData;
//	ST_buf_node *pbufferNode;



	kLog (K_TRACE, "[ACQ196_DAQ] dev(%s)\n", pSTDdev->taskName);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_ACQ196_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}


	pSTDdev->ST_Base.nSamplingRate = (int)pACQ196->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_MON, "[ACQ196_DAQ] %s : mode(%s) rate(%.f) time(%f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pACQ196->sample_rate, pACQ196->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_MON, "[ACQ196_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_INFO, "[ACQ196_DAQ] %s : mode(%s) rate(%.f) time(%f-%f = %f)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pACQ196->sample_rate,
				pACQ196->stop_t1, pACQ196->start_t0, pACQ196->sample_time);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pACQ196->slot == 0)) {
			/*	processAutoTrendRun (pSTDdev);   */
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pACQ196->slot != 0)) {
/* not need before already doing in arming function	stopDevice (pSTDdev);  */
				acqStatus = acq196_route_LEMO_to_FPGA(pSTDdev);  /* trig d0 and Clock d3 setting about lemo to fpga */
				epicsThreadSleep(0.5);
				acqStatus = acq196_set_TriggerConfig(pSTDdev);	 /* trig di3 setting about riging and event0 DI3 falling */
				epicsThreadSleep(0.5);
				acqStatus = acq196_channel_Block_Mask(pSTDdev);	 /* setting channelBlockMask, I will 96 channel all set */
				epicsThreadSleep(0.5);
				acqStatus = acq196_set_MODE(pSTDdev);	 	 /* SOFT_CONTINUOUS 1024 , I will not use setModeTriggeredContinuous*/
				epicsThreadSleep(0.5);
 

				if (TRUE == isTriggerRequired (pSTDdev)) {
					
					acqStatus = acq196_set_externalClk_DI0(pSTDdev);  /* setting setExternalClock DIO clockDivider */
					epicsThreadSleep(0.5);
					kLog (K_MON, "[ACQ196_DAQ] %s : waiting trigger %s\n",
							pSTDdev->taskName, pACQ196->triggerSource);
				}
				else {
					acqStatus = acq196_set_externalClk_DI0(pSTDdev); /* I use Ext trig & Ext clock. setting setExternalClock DIO clockDivider */
			/* I will Test	acqStatus = acq196_set_internalClk_DO0(pSTDdev);  setting acq196_set_internalClk_DO0 sampleRate*/
					epicsThreadSleep(0.5);
					kLog (K_MON, "[ACQ196_DAQ] %s : DAQ Start Local Mode with trigger\n", pSTDdev->taskName);
				}


				armingDeviceParams (pSTDdev);

				if(pACQ196->data_acquire_flag != 1)
				{
					// I have to make create Thread ,and then there, I have to process the data and ready to acquire data.
					pACQ196->data_acquire_flag = 1;
					epicsThreadSleep(1.0); // To sleep for event wait
					epicsEventSignal(pSTDdev->ST_CatchEndThread.threadEventId);
					epicsThreadSleep(1);     // To avoide data acquire missing when arming and then iminditly trigger
				} else  kLog (K_MON, "[ACQ196_DAQ] %s : Already doing stream-to-elastic-store function. If do not terminate shot without acquiring data then I will appear \n", pSTDdev->taskName); 
			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
				acqStatus = acq196_set_ARM(pSTDdev);	  /* setArm */
				epicsThreadSleep(0.1);   // To avoide data acquire missing when arming and then iminditly trigger

				kLog (K_INFO, "[threadFunc_ACQ196_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pACQ196->auto_run_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pACQ196->auto_run_flag = 0;

				kLog (K_INFO, "[threadFunc_ACQ196_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				if (isCalcRunMode()) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;

				/*  DBproc_put(PV_CALC_RUN_STR, "0");  */
				}
				else {
					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					DBproc_put(PV_TREND_RUN_STR, "0");
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

					start = wf_getCurrentUsec();
					for (i = 1; i <= MAX_MDSPUT_CNT; i++) {
						if (proc_sendData2Tree(pSTDdev) == WR_OK) {
							pMaster->n8MdsPutFlag = 0;
							break;
						}

						epicsThreadSleep(1);

						kLog (K_INFO, "[threadFunc_ACQ196_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}
					stop = wf_getCurrentUsec();
					epicsPrintf("[threadFunc_ACQ196_DAQ - write to mdsplus DB Mode (%d) ] : measured function process time sec. is %lf msec\n",
																				pSTDdev->ST_Base.opMode, 1.E-3 * (double)wf_intervalUSec(start,stop));

				/* SYS_ARMING and SYS_RUN PV value Return the init value sequence TG */
				/* All time return init state after end of data acquisition complate */
					DBproc_put (PV_SYS_RUN_STR, "0");	// runing 
					epicsThreadSleep(1);
					DBproc_put (PV_SYS_ARMING_STR, "0");	// arming
				
					close_local_file (pSTDdev);

					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					notify_refresh_master_status();

					pMaster->n8MdsPutFlag = 0;
				} else if(pMaster->n8MdsPutFlag == 1) {
					kLog (K_INFO, "[threadFunc_ACQ196_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_ACQ196_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_ACQ196_CatchEnd] ...\n");
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
//	ST_MASTER *pMaster = get_master();
// ACQ
// 	int next = BUFFERA;

        ST_User_Buff_node queueData;
        ST_buf_node *pbufferNode = NULL;
// Untill
	FILE *pfp;
        char buff[128];
        char strCmd[128]={"/home/dt100/bin/stream-to-elastic-store"};

	kLog (K_TRACE, "[ACQ196_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_ACQ196_CatchEnd] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 2000.);
		if (pACQ196->data_acquire_flag == 1) {
			kLog (K_INFO, "[start Data Acquire setting ] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_CatchEndThread.threadEventId);

			epicsEventWait(pSTDdev->ST_CatchEndThread.threadEventId);

			pfp=popen( strCmd, "r");
			if(!pfp) {
				kLog (K_INFO, "[stream-to-elastic-store : %s] %s ...failed\n",strCmd, pSTDdev->taskName);
	//			return WR_ERROR;
			}
			while(fgets(buff, 128, pfp) != NULL ) {
				// I want to process until that I want to data size without waiting error message (time out)
				pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
				ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);
				//	memcpy( pbufferNode->data, buff, sizeof(buff)); include space full 128
				memcpy( pbufferNode->data, buff, strlen(buff)-1);
				pbufferNode->data[strlen(buff)-1] = '\0';
				queueData.pNode         = pbufferNode;
				queueData.size          = strlen(buff);
				epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));
				//			printf("\n>>> %s",buff);
			} pclose(pfp);

			pACQ196->data_acquire_flag = 0;

		} else	kLog (K_INFO, "[threadFunc_ACQ196_CatchEnd] %s : waitting the ACQ196 Arming ....\n", pSTDdev->taskName);
	}
}
/*
	short *int16TempPack;
	short *int16TempPack32;	
	short *int16TempPack64;	
	short *int16TempPack5461;	
	short *int16TempPack5460;	
*/

	short int16TempPack[96];	
	short int16TempPack32[32];	
	short int16TempPack64[64];	
	short int16TempPack5461[524256];	
	short int16TempPack96_5461[96][5461];	
	short int16TempPack5460[524160];	
	short int16TempPack96_5460[96][5460];	

	short int16TempPack1M[524288];	
void threadFunc_ACQ196_RingBuf(void *param)
{
	kLog (K_TRACE, "[threadFunc_ACQ196_RingBuf] ...\n");
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_ACQ196 *pACQ196 = pSTDdev->pUser;
	ST_User_Buff_node queueData;
	ST_buf_node *pbufferNode;

	int i, ellCnt, j;
	int chanNum;

// file info
	FILE *fp;
	FILE *pfp;
	FILE *fpdata;
//	int indexBuff =0;      // index number in folder
//      int numberBuff=0;     // total number of buffering in all data
//      int numberSamples=0;  // sum data about the acquired data number from target buffer
//      float humTime=0;
//	int forLoop;
        char buffRaw[128];
        char rmBuff1[128];
        char rmBuff2[128];
        char buff[128];
//	char *sp;
// channel mapping info
	int channelMapping[96];
	int nVal, nTemp;
	int step;	
	
/*
 	// change to global varable for protect of exaction fault 
	int16TempPack = (short*) malloc(96*sizeof(short));
	int16TempPack32 = (short*) malloc(32*sizeof(short));	
	int16TempPack64 = (short*) malloc(64*sizeof(short));	
	int16TempPack5461 = (short*) malloc(5461*sizeof(short));	
	int16TempPack5460 = (short*) malloc(5460*sizeof(short));	
*/

//	short int16TempPack32[32];	
//	short int16TempPack64[64];	
//	short int16TempPack5461[524256];	
//	short int16TempPack5460[524160];	
	
        for(i = 0; i< MAX_RING_BUF_NUM; i++)
        {
                ST_buf_node *pbufferNode1 = NULL;
                pbufferNode1 = (ST_buf_node*) malloc(sizeof(ST_buf_node));
                if(!pbufferNode1) {
                        epicsPrintf("\n>>> threadFunc_user_RingBuf: malloc(sizeof(ST_buf_node))... fail\n");
                        return;
                }
                ellAdd(pSTDdev->pList_BufferNode, &pbufferNode1->node);
        }
        epicsPrintf(" %s: create %d node (size:%ldKB)\n", pSTDdev->ST_RingBufThread.threadName, MAX_RING_BUF_NUM, sizeof(ST_buf_node)/1024 );
	// CVI Function
	ellCnt = 0;
	chanNum = ellCount(pSTDdev->pList_Channel);
// To get information about channel mapping for channelization 

	sprintf(buffRaw, "%s", STR_CH_MAPPING_FILE);
	fp = fopen(buffRaw,"r");
	if(fp == NULL) {
		printf("Can't open channel mapping file :%s \n",buffRaw);
	} else {
		for(i=0; i<96; i++)
		{
			if( fgets(buffRaw, 32, fp) == NULL ) {
			kLog (K_MON, "[threadFunc_ACQ196_RingBuf] : Channel Mapping  file EOF error \n");
			}
			sscanf(buffRaw, "%d %d", &nTemp, &nVal);
			if( nTemp != (i+1) ) {
				kLog (K_MON, "[threadFunc_ACQ196_RingBuf] : Channel Mapping channel index order error! \n");
			}
			channelMapping[nVal] = i;

			kLog (K_MON, "[threadFunc_ACQ196_RingBuf] : Channel Mapping check after mapping nTemp(%d), nVal(%d), indexing(%d)\n", nTemp, nVal, i);
		}
		fclose(fp);
		for(i=0; i<96; i++)
		{
			kLog (K_MON, "[threadFunc_ACQ196_RingBuf checking] : Channel Mapping check after mapping index(%d) Final MappingVal(%d) \n", i, channelMapping[i]);
		}
	}
	
	
// end channelization 

	while (TRUE) {
		epicsMessageQueueReceive(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));
		//		if( queueData.opcode & QUEUE_OPCODE_CLOSE_FILE)
		//		Maybe I do not need this function, because I know the dataSize(5461 - 96ch(524280)). dataSize(5461) = numberSamples / numberBuff 
		pbufferNode = queueData.pNode;

#if 1
		//memcpy( buff, pbufferNode->data, strlen(pbufferNode->data)+1);
		memcpy( buff, pbufferNode->data, strlen(pbufferNode->data));
		//		buff[strlen(pbufferNode->data)-1]='\0';

#if 0
		fp=fopen(buff, "r");
		if(fp != NULL)
#endif		
			if(strlen(pbufferNode->data) > 8)
			{
				if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev |= TASK_IN_PROGRESS;

					kLog (K_INFO, "[threadFunc_ACQ196_RingBuf] %s : status(0x%x:%s)\n",
							pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

					notify_refresh_master_status();
					scanIoRequest(pSTDdev->ioScanPvt_status);
				}
#if 0
				for(forLoop=0; forLoop<4; forLoop++)
				{
					if(fgets(buff, sizeof(buff),fp) == NULL) break;
					if(strlen(buff))
					{
						sp = strtok(buff,"=");
						sp = strtok(NULL,"=");
						if(forLoop == 0) {
							indexBuff = atoi(sp);
						} else if (forLoop == 1) {
							numberBuff = atoi(sp);
						} else if (forLoop == 2) {
							numberSamples = atoi(sp);
						} else if (forLoop == 3) {
							humTime = atof(sp);
						} else  printf("error in read infomation \n");
					}
				}
				fclose(fp);
#endif
#endif
				// To get file name from file information file .. remove ".id"  		
				//strncpy(buffRaw, pbufferNode->data, strlen(pbufferNode->data)-3);

				memset(buffRaw, 0, sizeof(buffRaw));
				memcpy(buffRaw, pbufferNode->data, strlen(pbufferNode->data)-3);
				buffRaw[strlen(pbufferNode->data)-2]='\0';

				fpdata=fopen(buffRaw,"r");
				if(fpdata == NULL){
					kLog (K_MON, "[threadFunc_ACQ196_RingBuf] %s :   Error file open the raw data file......) \n",buffRaw);
				} else {
					//why did not sucess channelization after second file. Because of data file size of 1MB(1,048,576) so we have always acquire over 64byte(32ch) data. 
					//for(i=0; i<(numberSamples - pACQ196->totalRead); i++) //5461(+1) = numberSamples / numberBuff, sometimes over sample data of 5461 
					if( pACQ196->fileHandling == 1){
						// we have to process second data file after first file. 524,256 
						pACQ196->fileHandling = 2;
#if NO_RT_CH
						fread(&int16TempPack1M, 1048576, 1, fpdata);
						fwrite(&int16TempPack1M, 1048576, 1, pACQ196->writeRaw_fp);
						fflush(pACQ196->writeRaw_fp);
#endif
#if RT_CH 
						fread(&int16TempPack5461, 1048512, 1, fpdata);
						step = 0;
						for(i=0; i<5461; i++) 
						{
							//fread(&int16TempPack, 192, 1, fpdata);
							step = 96 * i;
							for(j=0; j<96; j++)
							{
								//fwrite(&int16TempPack[j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
								//fwrite(&int16TempPack5461[step+j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
								//In this time, I just separate the raw data sort 
							//	int16TempPack96_5461[j][i] = int16TempPack5461[step+j];
								//In this time, I do channelization the raw data
								int16TempPack96_5461[channelMapping[j]][i] = int16TempPack5461[step+j];
							}	

						}
						for(j=0; j<96; j++)
						{ 
							// Raw data template buffer is not channalization data, so I do channel mapping to use the raw data at this time. 
							// Normal scaling	pACQ196->ST_Ch[j].euValue  = (float64)int16TempPack96_5461[5460][channelMapping[j]] * (10.0/32768.0);
							//pACQ196->ST_Ch[j].rawValue  = int16TempPack96_5461[channelMapping[j]][5460];
							//pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack96_5461[channelMapping[j]][5460] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
							pACQ196->ST_Ch[j].rawValue  = int16TempPack96_5461[j][5460];
							pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack96_5461[j][5460] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
						//	pACQ196->ST_Ch[j].teValue = (float64)(pACQ196->ST_Ch[j].euValue * pACQ196->ST_Ch[j].calFactor);
						//	pACQ196->rtTe[j] = pACQ196->ST_Ch[j].teValue;
						}
						// I have to recently data send to user client for real-time control (ex, ECE, ECH ?)
						scanIoRequest(pSTDdev->ioScanPvt_userCall);
						scanIoRequest(pSTDdev->ioScanPvt_userCall1);
						// we acquire over data size 64byte after processed in first file 
						fread(&int16TempPack32, 64, 1, fpdata);
						for(j=0; j<96; j++)
						{
							//In this time, I do channalization the raw data into file , 10922 = 5461 * 2byte 
							//fwrite(&int16TempPack96_5461[j], 10922, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
							fwrite(&int16TempPack96_5461[j], 10922, 1, pACQ196->ST_Ch[j].write_fp);
						}	
#endif
#if 0
						pACQ196->ibuffRead	= indexBuff;
						pACQ196->nbuffRead	= numberBuff;
						pACQ196->nsamplesRead	= numberSamples;
						pACQ196->htimeRead	= humTime;
#endif
#if NO_RT_CH
						for(j=0; j<96; j++)
						{
							pACQ196->ST_Ch[j].rawValue  = int16TempPack1M[channelMapping[j]];
							// Normal scaling	pACQ196->ST_Ch[j].euValue  = (float64)int16TempPack1M[channelMapping[j]] * (10.0/32768.0);
							pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack1M[channelMapping[j]] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
					//		pACQ196->ST_Ch[j].teValue = (float64)(pACQ196->ST_Ch[j].euValue * pACQ196->ST_Ch[j].calFactor);
					//		pACQ196->rtTe[j] = pACQ196->ST_Ch[j].teValue;
						}	
#endif

						pACQ196->totalRead	+= 5461;  // last loop data counts, and then I compare the value in myMDSplus.c 
						pACQ196->ibuffRead	+= 1;
						pACQ196->nbuffRead      += 1;

						ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
						ellCnt = ellCount(pSTDdev->pList_BufferNode);
						fclose(fpdata);
					} else if (pACQ196->fileHandling == 2) {
						// we have to process third data file after second file. 
						pACQ196->fileHandling = 3;

						// we acquire over data size 64byte after processed in first file 
#if NO_RT_CH
						fread(&int16TempPack1M, 1048576, 1, fpdata);
						fwrite(&int16TempPack1M, 1048576, 1, pACQ196->writeRaw_fp);
						fflush(pACQ196->writeRaw_fp);
#endif

#if RT_CH
						// At this time, I process the remained data at 1st last32 and 2nd first 64
						fread(&int16TempPack64, 128, 1, fpdata);
						memcpy(int16TempPack, int16TempPack32, 64);
						memcpy(int16TempPack+32, int16TempPack64, 128);

						for(j=0; j<96; j++)
						{
							fwrite(&int16TempPack[j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
						}
		
						// In second file, for loop counts is important value (5460), Because of second data is first and last data(128byte) except in this loop and then process data other place
						fread(&int16TempPack5460, 1048320, 1, fpdata);  // 5460 * 96 * 2 = 1,048,320
						step = 0;
						for(i=0; i<5460; i++) 
						{
							//	fread(&int16TempPack, 192, 1, fpdata);
							step = 96 * i;
							for(j=0; j<96; j++)
							{
								// fwrite(&int16TempPack[j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
								// fwrite(&int16TempPack5460[step +j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
								//int16TempPack96_5460[j][i] = int16TempPack5460[step+j];
								int16TempPack96_5460[channelMapping[j]][i] = int16TempPack5460[step+j];
							}	

						}
						// I have to recently data send to user client for real-time control (ex, ECE, ECH ?)
						for(j=0; j<96; j++)
						{ 
							// Raw data template buffer is not channalization data, so I do channel mapping to use the raw data at this time. 
							// Normal scaling	pACQ196->ST_Ch[j].euValue  = (float64)int16TempPack96_5461[5460][channelMapping[j]] * (10.0/32768.0);
							//pACQ196->ST_Ch[j].rawValue  = int16TempPack96_5460[channelMapping[j]][5459];
							//pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack96_5460[channelMapping[j]][5459] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
							pACQ196->ST_Ch[j].rawValue  = int16TempPack96_5460[j][5459];
							pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack96_5460[j][5459] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
					//		pACQ196->ST_Ch[j].teValue = (float64)(pACQ196->ST_Ch[j].euValue * pACQ196->ST_Ch[j].calFactor);
					//		pACQ196->rtTe[j] = pACQ196->ST_Ch[j].teValue;
						}
						scanIoRequest(pSTDdev->ioScanPvt_userCall);
						scanIoRequest(pSTDdev->ioScanPvt_userCall1);
						for(j=0; j<96; j++)
						{
							//In this time, I do channalization the raw data into file , 10920 = 5460 * 2byte 
							//fwrite(&int16TempPack96_5460[j], 10920, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
							fwrite(&int16TempPack96_5460[j], 10920, 1, pACQ196->ST_Ch[j].write_fp);
						}	

						// we acquire over data size 128byte in the end of second file 
						fread(&int16TempPack64, 128, 1, fpdata);
/*
						// Not use this function at this time
						pACQ196->totalRead	+= i + 1;  // last loop data counts, and then I compare the value in myMDSplus.c 
						pACQ196->ibuffRead	+= 1;
						pACQ196->ibuffRead	= indexBuff;
						pACQ196->nbuffRead	= numberBuff;
						pACQ196->nsamplesRead	= numberSamples;
						pACQ196->htimeRead	= humTime;
*/
#endif
						pACQ196->totalRead	+= 5461;  // last loop data counts, and then I compare the value in myMDSplus.c 
						pACQ196->ibuffRead	+= 1;
						pACQ196->nbuffRead      += 1;

						ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
						ellCnt = ellCount(pSTDdev->pList_BufferNode);
						fclose(fpdata);

					} else if (pACQ196->fileHandling == 3) {
						// we have to process first data file after third file. 
						pACQ196->fileHandling = 1;
#if NO_RT_CH
						fread(&int16TempPack1M, 1048576, 1, fpdata);
						fwrite(&int16TempPack1M, 1048576, 1, pACQ196->writeRaw_fp);
						fflush(pACQ196->writeRaw_fp);
#endif
						// we acquire over data size 64byte after processed in  file 
#if RT_CH 
						fread(&int16TempPack32, 64, 1, fpdata);

						memcpy(int16TempPack, int16TempPack64, 128);
						memcpy(int16TempPack+64, int16TempPack32, 64);
						for(j=0; j<96; j++)
						{
							fwrite(&int16TempPack[j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
						}

						fread(&int16TempPack5461, 1048512, 1, fpdata);
						step = 0;

						for(i=0; i<5461; i++) 
						{
							//fread(&int16TempPack, 192, 1, fpdata);
							step = 96 * i;
							for(j=0; j<96; j++)
							{
								// fwrite(&int16TempPack5461[step+j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
								// fwrite(&int16TempPack[j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
								//int16TempPack96_5461[j][i] = int16TempPack5461[step+j];
								int16TempPack96_5461[channelMapping[j]][i] = int16TempPack5461[step+j];
							}	

						}
						// I have to recently data send to user client for real-time control (ex, ECE, ECH ?)
						for(j=0; j<96; j++)
						{ 
							// Raw data template buffer is not channalization data, so I do channel mapping to use the raw data at this time. 
							// Normal scaling	pACQ196->ST_Ch[j].euValue  = (float64)int16TempPack96_5461[5460][channelMapping[j]] * (10.0/32768.0);
							//pACQ196->ST_Ch[j].rawValue  = int16TempPack96_5461[channelMapping[j]][5460];
							//pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack96_5461[channelMapping[j]][5460] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
							pACQ196->ST_Ch[j].rawValue  = int16TempPack96_5461[j][5460];
							pACQ196->ST_Ch[j].euValue  = (float64)(pACQ196->ST_Ch[j].minVoltRange + ((int16TempPack96_5461[j][5460] + 32768) * (pACQ196->ST_Ch[j].maxVoltRange - pACQ196->ST_Ch[j].minVoltRange)/65535));
							pACQ196->ST_Ch[j].teValue = (float64)(pACQ196->ST_Ch[j].euValue * pACQ196->ST_Ch[j].calFactor);
							pACQ196->rtTe[j] = pACQ196->ST_Ch[j].teValue;
						}
						scanIoRequest(pSTDdev->ioScanPvt_userCall);
						scanIoRequest(pSTDdev->ioScanPvt_userCall1);

						for(j=0; j<96; j++)
						{
							//In this time, I do channalization the raw data into file , 10922 = 5461 * 2byte 
							//fwrite(&int16TempPack96_5461[j], 10922, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
							fwrite(&int16TempPack96_5461[j], 10922, 1, pACQ196->ST_Ch[j].write_fp);
						}	
/*
						// Not use this function at this time
						pACQ196->totalRead	+= i + 1;  // last loop data counts, and then I compare the value in myMDSplus.c 
						pACQ196->ibuffRead	+= 1;
						pACQ196->ibuffRead	= indexBuff;
						pACQ196->nbuffRead	= numberBuff;
						pACQ196->nsamplesRead	= numberSamples;
						pACQ196->htimeRead	= humTime;
*/
#endif
						pACQ196->totalRead	+= 5462;  // last loop data counts, and then I compare the value in myMDSplus.c 
						pACQ196->ibuffRead	+= 1;
						pACQ196->nbuffRead      += 1;
	
						ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
						ellCnt = ellCount(pSTDdev->pList_BufferNode);
						fclose(fpdata);

					} else kLog (K_MON, "[threadFunc_ACQ196_RingBuf] %s : Error file Handling for data processing. It is important !!!!!!!!!.##########################...) \n",pSTDdev->taskName); 

					kLog (K_MON, "[threadFunc_ACQ196_RingBuf] Open and storing Task(%s), totalRead(%d),iBuff(%d), nBuff(%d)..nBuffNode(ellCnt:%d)...%s.\n",
							pSTDdev->taskName, pACQ196->totalRead, pACQ196->ibuffRead, pACQ196->nbuffRead, ellCnt, buffRaw);
				}
#if 0
				kLog (K_MON, "[threadFunc_ACQ196_RingBuf] %s : indexBuff(%d) numberBuff(%d) numberSamples(%d) humTime(%f) BufferNodeCnt(%d). \n",pbufferNode->data, indexBuff,numberBuff,numberSamples,humTime, ellCnt);
#endif
				if (TRUE == isSamplingStop (pSTDdev)) {
					kLog (K_INFO, "[threadFunc_ACQ196_RingBuf] %s : stop ....\n", pSTDdev->taskName);
					stopDevice (pSTDdev);
				}

				if(pACQ196->ibuffRead==66) { // I have to remove alwalys after processed data in current folder.
#if 1 
					memset(rmBuff1, 0, sizeof(rmBuff1));
					memset(rmBuff2, 0, sizeof(rmBuff2));
					memcpy(rmBuff2, pbufferNode->data, strlen(pbufferNode->data)-7);
					rmBuff2[strlen(pbufferNode->data)-6]='\0';
					// I have to check the include null or not null 	
					sprintf(rmBuff1,"rm -rf %s0.*",rmBuff2);
					pfp=popen( rmBuff1, "r");
					if(!pfp) {
						kLog (K_MON, "[threadFunc_ACQ196_RingBuf] Remove file folder :%s.....Failed.:%s \n",pSTDdev->taskName,rmBuff1);
					}
					/*
					   while(fgets(buff, 128, pfp) != NULL ) {
					   kLog (K_MON, "[threadFunc_ACQ196_RingBuf] Success Remove file folder: %s \n",rmBuff1);
					   }
					   */
					pclose(pfp);	
					kLog (K_MON, "[threadFunc_ACQ196_RingBuf] Remove file folder :%s.....:%s \n",pSTDdev->taskName,rmBuff1);
					pACQ196->ibuffRead	= 0;
#endif
				}
			} else {
				ellAdd(pSTDdev->pList_BufferNode, &pbufferNode->node);
				ellCnt = ellCount(pSTDdev->pList_BufferNode);
				kLog (K_MON, "[threadFunc_ACQ196_RingBuf] Bad file name %sBuffLen:%d..BufferNodeCnt(%d). \n",buffRaw,strlen(buffRaw),ellCnt);
				if (pACQ196->totalRead >= 5661) {
					kLog (K_INFO, "[threadFunc_ACQ196_RingBuf] %s : totalRead : %d  stopDevice last file read fail. Because the script return empty string when end of streaming.\n",
							pSTDdev->taskName, pACQ196->totalRead);
					stopDevice (pSTDdev);
				}
			}
	}
}

int create_ACQ196_taskConfig(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pACQ196 = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int 	i;
	int	chanNum;
	pACQ196 = (ST_ACQ196*) calloc(1, sizeof(ST_ACQ196));
	if(!pACQ196) return WR_ERROR;
	
	kLog (K_TRACE, "[create_ACQ196_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pACQ196->slot = atoi(pSTDdev->strArg0);
	pACQ196->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pACQ196->data_buf) {
		free (pACQ196);
		return WR_ERROR;
	}

	ellInit(pACQ196->data_buf);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);
	kLog (K_TRACE, "[create_ACQ196_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s) channelCounts(%d)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2, chanNum);

	for (i=0;i<chanNum;i++) {
		sprintf (pACQ196->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
		sprintf (pACQ196->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);

		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			pACQ196->ST_Ch[i].used = CH_USED;

			strcpy (pACQ196->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			if (0 == strcmp (NBI_INPUT_TYPE_DT_STR, pSTDCh->strArg0)) {
				pACQ196->ST_Ch[i].inputType	= NBI_INPUT_DELTA_T;
			}
			else if (0 == strcmp (ECEHR_INPUT_TE_TYPE_STR, pSTDCh->strArg0)) {
				pACQ196->ST_Ch[i].inputType	= ECEHR_INPUT_TE;
				pACQ196->ST_Ch[i].targetFrq	= 1e+9 * (strtoul(pSTDCh->strArg2,NULL,0));
				pACQ196->ST_Ch[i].calFactor	= strtod(pSTDCh->strArg3,NULL);
			}
			else if (0 == strcmp (NBI_INPUT_TYPE_TC_STR, pSTDCh->strArg0)) {
				pACQ196->ST_Ch[i].inputType	= NBI_INPUT_TC;
			}
			else if (0 == strcmp (DENSITY_CALC_STR, pSTDCh->strArg2)) {
				pACQ196->ST_Ch[i].isFirst = 0;
				pACQ196->ST_Ch[i].density = 0;
				pACQ196->ST_Ch[i].fringe = 0.2078;
				pACQ196->ST_Ch[i].dLimUpper = 0.16;
				pACQ196->ST_Ch[i].dLimLow = 0.05;
			}
			else if (0 == strcmp (MDS_PARAM_PUT_STR, pSTDCh->strArg2)) {
				pACQ196->ST_Ch[i].inputType = MDS_PARAM_PUT;
			}
			else {
				pACQ196->ST_Ch[i].inputType	= OTHER_INPUT_NONE;
			}

			kLog (K_INFO, "[create_ACQ196_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pACQ196->ST_Ch[i].physical_channel,
					pACQ196->ST_Ch[i].inputType, pACQ196->ST_Ch[i].inputTagName,
					pACQ196->ST_Ch[i].file_name);
		}
	}

	sprintf (pACQ196->triggerSource, "%s", pSTDdev->strArg1); 

	pACQ196->sample_rate			= INIT_SAMPLE_RATE;
	pACQ196->sample_rateLimit		= INIT_SAMPLE_RATE_LIMIT;
	pACQ196->sample_time			= getSampTime(pACQ196);
	pACQ196->currentTF			= 19.64;                // 19.64kA -> 1.964 T

	pSTDdev->pUser				= pACQ196;

	pSTDdev->ST_Func._OP_MODE		= func_ACQ196_OP_MODE;
	pSTDdev->ST_Func._CREATE_LOCAL_TREE	= func_ACQ196_CREATE_LOCAL_TREE;
	pSTDdev->ST_Func._SYS_ARMING		= func_ACQ196_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN		= func_ACQ196_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_ACQ196_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET		= func_ACQ196_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0		= func_ACQ196_T0;
	pSTDdev->ST_Func._SYS_T1		= func_ACQ196_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_ACQ196_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_ACQ196_SHOT_NUMBER;

/* Remove TG
	pSTDdev->ST_Func._POST_SEQSTOP		= func_ACQ196_POST_SEQSTOP; 
	pSTDdev->ST_Func._PRE_SEQSTART		= func_ACQ196_PRE_SEQSTART;
*/
	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_ACQ196_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 1
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ACQ196_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buff_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ACQ196_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		kLog (K_ERR, "%s : CatchEnd thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvACQ196_init_driver(void)
{
	kLog (K_TRACE, "[drvACQ196_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_ACQ196_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvACQ196_init_driver] %s : create_ACQ196_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvACQ196_set_TagName(pSTDdev);
		drvACQ196_set_VIN(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvACQ196_io_report(int level)
{
	kLog (K_TRACE, "[drvACQ196_io_report] ...\n");
	
	int i;	
	int chanNum;
	ST_STD_device *pSTDdev = NULL;
	ST_MASTER *pMaster = get_master();
	ST_ACQ196	*pACQ196 = NULL;

	if (!pMaster) return -1;

	if (ellCount(pMaster->pList_DeviceTask)) {
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
		pACQ196 = pSTDdev->pUser;
	}
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pMaster->pList_DeviceTask));

	if (level<1) return -1;

	while (pSTDdev) {
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
					epicsPrintf(" Ch No:%d, Tag Name : %s, euValue : %f \n",
						i, pACQ196->ST_Ch[i].strTagName,pACQ196->ST_Ch[i].euValue);
					epicsPrintf("\n");
					
			}
#endif
			epicsPrintf("   ");
			epicsPrintf("\n");
		}

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}

