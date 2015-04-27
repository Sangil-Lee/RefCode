#include <fcntl.h>	// for mkdir
#include <errno.h>

#include "drvNIDAQmx.h"

static long drvNIDAQmx_io_report(int level);
static long drvNIDAQmx_init_driver();


struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNIDAQmx = {
	2,
	drvNIDAQmx_io_report,
	drvNIDAQmx_init_driver
};

epicsExportAddress(drvet, drvNIDAQmx);

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
		DAQmxGetExtendedErrorInfo(errBuff,sizeof(errBuff));
		kLog (K_ERR, "[%s:%d] DAQmx Error: %s\n", fname, lineNo, errBuff);
		notify_error (1, "DAQmx Error: %s\n", errBuff);

		epicsThreadSleep(1);
	}

	return (0);
}

int isCalcRunMode (ST_STD_device *pSTDdev)
{
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	return (!pNIDAQmx->trend_run_flag);
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

ST_NIDAQmx *getDriver (ST_STD_device *pSTDdev) 
{
   	return ((ST_NIDAQmx *)pSTDdev->pUser); 
}

int getNumSampsPerChan (ST_NIDAQmx *pNIDAQmx) 
{ 
	return (pNIDAQmx->sample_rate / 10); 
//	return (pNIDAQmx->sample_rate); // Test every event per 10msc 
}

int getArraySizeInBytes (ST_NIDAQmx *pNIDAQmx) 
{
   	return (sizeof(pNIDAQmx->readArray)); 
}

int getMaxNumChan (ST_STD_device *pSTDdev) 
{
// TGLee 
	return (ellCount(pSTDdev->pList_Channel));
/* 	return (INIT_MAX_CHAN_NUM);   */
}

float64 getTotSampsChan (ST_NIDAQmx *pNIDAQmx) 
{ 
	return (pNIDAQmx->sample_rate * pNIDAQmx->sample_time); 
}

int getTotSampsAllChan (ST_STD_device *pSTDdev) 
{
ST_NIDAQmx                *pNIDAQmx = pSTDdev->pUser; 
	return (pNIDAQmx->sample_rate * getMaxNumChan(pSTDdev)); 
}

int getSampRate (ST_NIDAQmx *pNIDAQmx) 
{ 
	return (pNIDAQmx->sample_rate); 
}

float64 getSampTime (ST_NIDAQmx *pNIDAQmx) 
{ 
	return (pNIDAQmx->sample_time); 
}

/* void setSamplTime (ST_NIDAQmx *pNIDAQmx) */
void setSamplTime (ST_STD_device *pSTDdev) 
{
	ST_NIDAQmx *pNIDAQmx = NULL;
	pNIDAQmx = pSTDdev->pUser;

/*	pNIDAQmx->sample_time = (pNIDAQmx->stop_t1 <= pNIDAQmx->start_t0) ? 0 : (pNIDAQmx->stop_t1 - pNIDAQmx->start_t0);   */

	if(pNIDAQmx->stop_t1 <= pNIDAQmx->start_t0) {
		pNIDAQmx->sample_time = 0;
	} else {
		pNIDAQmx->sample_time = pNIDAQmx->stop_t1 - pNIDAQmx->start_t0;
	}

	kLog (K_INFO, "[setSamplTime] start_t0(%f) stop_t1(%f) sample_time(%f)\n",
			pNIDAQmx->start_t0, pNIDAQmx->stop_t1, pNIDAQmx->sample_time);
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}
void setStartT0 (ST_NIDAQmx *pNIDAQmx, float64 val) 
{
	pNIDAQmx->start_t0 = val;
	pNIDAQmx->sample_time = (pNIDAQmx->stop_t1 <= pNIDAQmx->start_t0) ? 0 : (pNIDAQmx->stop_t1 - pNIDAQmx->start_t0);
/*	setSamplTime (pNIDAQmx);  */
}

void setStopT1 (ST_NIDAQmx *pNIDAQmx, float64 val)
{
	pNIDAQmx->stop_t1 = val;
	pNIDAQmx->sample_time = (pNIDAQmx->stop_t1 <= pNIDAQmx->start_t0) ? 0 : (pNIDAQmx->stop_t1 - pNIDAQmx->start_t0);
/*	setSamplTime (pNIDAQmx);  */
}

void armingDeviceParams (ST_STD_device *pSTDdev)
{
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
//	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int	i;
	int 	chanNum;
	float64	sampRate	= getSampRate (pNIDAQmx);

	pNIDAQmx->totalRead	= 0;
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
	chanNum = ellCount(pSTDdev->pList_Channel); 
	
	if(0 == strcmp(pNIDAQmx->sigType, "ai")){
//		pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAICh = pSTDCh->pUser;
				pNIDAQmxAICh->offset		= 0;
				pNIDAQmxAICh->power		= 0;
				pNIDAQmxAICh->accum_temp	= 0;
				pNIDAQmxAICh->accum_cnt		= 0;
				pNIDAQmxAICh->integral_cnt	= pNIDAQmxAICh->integral_time * sampRate;

				kLog (K_DEBUG, "[armingDeviceParams] AI %s : ch(%d) time(%f) cnt(%f) flow(%f) coeff(%f) \n",
						pNIDAQmxAICh->physical_channel, i,
						pNIDAQmxAICh->integral_time, pNIDAQmxAICh->integral_cnt,
						pNIDAQmxAICh->flow, pNIDAQmxAICh->coefficient);
//				pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
			}
		}
	} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
		kLog (K_DEBUG, "[armingDeviceParams]  %s : Do not anything at this AO type( %s ) \n", pSTDdev->taskName, pNIDAQmx->sigType);
/*
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAOCh = pSTDCh->pUser;
				pNIDAQmxAOCh->power		= 0;
				pNIDAQmxAOCh->flow		= 0;
				pNIDAQmxAOCh->integral_cnt	= pNIDAQmxAOCh->integral_time * sampRate;

				kLog (K_DEBUG, "[armingDeviceParams] AO %s : ch(%d) time(%f) cnt(%f) \n",
						pNIDAQmxAICh->physical_channel, i,
						pNIDAQmxAICh->integral_time, pNIDAQmxAICh->integral_cnt);
			}
		}
*/
	} else   kLog (K_DEBUG, "[armingDeviceParams]  %s : Not yet support type( %s ) \n", pNIDAQmx->sigType, pNIDAQmx->sigType); 
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
	ST_NIDAQmx	*pNIDAQmx = getDriver(pSTDdev);
	// when trend_run_flag value is 1, then we gethering data until stop DAQ.
	return ( isCalcRunMode(pSTDdev) ? (pNIDAQmx->totalRead >= getTotSampsChan(pNIDAQmx)) : FALSE );
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

void make_pattern_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_pattern_file] %s ...\n", pSTDdev->taskName);

	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;
	int chanNum;
        chanNum = ellCount(pSTDdev->pList_Channel);

	if(0 == strcmp(pNIDAQmx->sigType, "ao")){
//		pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAOCh = pSTDCh->pUser;
				if (NULL != pNIDAQmxAOCh->write_fp) {
					fclose(pNIDAQmxAOCh->write_fp);
					pNIDAQmxAOCh->write_fp	= NULL;
					if (CH_USED == pNIDAQmxAOCh->used) {
						sprintf (pNIDAQmxAOCh->path_name,"%s%s",
								STR_LOCAL_DATA_DIR, pNIDAQmxAOCh->file_name);
						kLog (K_INFO, "[make_pattern_file] AO fname(%s)\n", pNIDAQmxAOCh->path_name);

						// read & write (trunc)
						if (NULL == (pNIDAQmxAOCh->write_fp = fopen(pNIDAQmxAOCh->path_name, "w+"))) {
							kLog (K_ERR, "[make_pattern_file] AO %s create failed!!!\n", pNIDAQmxAOCh->path_name);
							notify_error (1, "%s create AO failed!\n", pNIDAQmxAOCh->path_name);
						} else kLog (K_INFO, "[make_pattern_file] AO fname(%s) sucessed\n", pNIDAQmxAOCh->path_name);
					}
				} else {
					pNIDAQmxAOCh->write_fp = NULL;
					if (CH_USED == pNIDAQmxAOCh->used) {
						sprintf (pNIDAQmxAOCh->path_name,"%s%s",
								STR_LOCAL_DATA_DIR, pNIDAQmxAOCh->file_name);
						kLog (K_INFO, "[make_local_file] AO fname(%s)\n", pNIDAQmxAOCh->path_name);

						// read & write (trunc)
						if (NULL == (pNIDAQmxAOCh->write_fp = fopen(pNIDAQmxAOCh->path_name, "w+"))) {
							kLog (K_ERR, "[make_local_file] AO %s create failed!!!\n", pNIDAQmxAOCh->path_name);
							notify_error (1, "%s create AO failed!\n", pNIDAQmxAOCh->path_name);
						} else kLog (K_INFO, "[make_local_file] AO fname(%s) sucessed\n", pNIDAQmxAOCh->path_name);
					}
				}
				kLog (K_INFO, "[make_local_file] AO fname(%s) fpoint(%p) sucessed\n", pNIDAQmxAOCh->path_name, pNIDAQmxAOCh->write_fp);
//				pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) ellNext(&pNIDAQmxAOCh->node);
			}
		}
	
	} else kLog (K_INFO, "[close_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType);
}
void make_pattern_dataToFile(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_pattern_dataToFile] %s ...\n", pSTDdev->taskName);

	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i, iTime, j, totalWriteTemp;
	int waveTemp;
	int chanNum;
	float64 wavefTime0;
	int sampleRate;
	int waveTime0;
	int waveTime1;
	//float64 waveTime1;
    //float64 waveTime0;

	float64 waveValue0, waveValue1;
	float64 sample_step;
	float64 aA, bB, waveData;
//	float64 patternTemp[1000];  // Do not use this temp buffer for make pattern. Now, I write the data one by one.
	pNIDAQmx->totalWrite = 0;
	totalWriteTemp = 0;
	aA = 0.0;
	bB = 0.0;
	waveData = 0.0;
	sampleRate = (int)pNIDAQmx->sample_rate;
    chanNum = ellCount(pSTDdev->pList_Channel);

	make_pattern_file(pSTDdev);

	if(0 == strcmp(pNIDAQmx->sigType, "ao")){
		// To find longest end time, because all of channel waveCounts is same size
		wavefTime0 = 0.0;
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAOCh = pSTDCh->pUser;
				for(waveTemp=0;waveTemp<80;waveTemp++) {  								// loop no is 79, because to find longest time. i+1 loop, so I do loop (buffer -l) times 
					if (wavefTime0 < pNIDAQmxAOCh->wavePatternTime[waveTemp]) {
						wavefTime0 = pNIDAQmxAOCh->wavePatternTime[waveTemp];
					}
				}
			}
		}
		pNIDAQmx->totalWrite =(int32)((wavefTime0 - pNIDAQmxAOCh->wavePatternTime[0]) * pNIDAQmx->sample_rate); 				// Find largest time value * sample rate

		sample_step = 1 / pNIDAQmx->sample_rate;
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAOCh = pSTDCh->pUser;
				totalWriteTemp = 0;
				for(waveTemp=0;waveTemp<79;waveTemp++) {  								// loop no is 78, because compare ch 0 and 1, i+1 loop, so I do loop (buffer -l) times 
					//waveTime0 = (int32)(pNIDAQmxAOCh->wavePatternTime[waveTemp] * (int)pNIDAQmx->sample_rate); 		// time unit (sec) * sample rate
					//waveTime1 = (int32)(pNIDAQmxAOCh->wavePatternTime[waveTemp + 1] * (int)pNIDAQmx->sample_rate); 	// time unit (sec) +1 * sample rate
					//waveTime0 = (int)((pNIDAQmxAOCh->wavePatternTime[waveTemp] * 10000) + 0.5); 		// time unit (sec) * sample rate
					//waveTime1 = (int)((pNIDAQmxAOCh->wavePatternTime[waveTemp + 1] * 10000) +0.5);     // To avoid cut low level value 
					waveTime0 = (int)((pNIDAQmxAOCh->wavePatternTime[waveTemp] * sampleRate) + 0.5); 		// time unit (sec) * sample rate
					waveTime1 = (int)((pNIDAQmxAOCh->wavePatternTime[waveTemp + 1] * sampleRate) +0.5);     // To avoid cut low level value 

					waveValue0 = pNIDAQmxAOCh->wavePatternValue[waveTemp];
					waveValue1 = pNIDAQmxAOCh->wavePatternValue[waveTemp +1];
					kLog (K_INFO, "[make_pattern_dataToFile] To Check Ch(%d) point(%d) (wVal0(%f)wPVal0(%f), wVal1(%f)wPVal1(%f), wT0(%d)wPT0(%f), wT1(%d)wPT1(%f)) \n",
				    i, waveTemp, waveValue0, pNIDAQmxAOCh->wavePatternValue[waveTemp], waveValue1, pNIDAQmxAOCh->wavePatternValue[waveTemp +1], waveTime0, pNIDAQmxAOCh->wavePatternTime[waveTemp], waveTime1, pNIDAQmxAOCh->wavePatternTime[waveTemp + 1]);
					if((pNIDAQmxAOCh->wavePatternValue[waveTemp] - pNIDAQmxAOCh->wavePatternValue[waveTemp + 1]) != 0) {

						if(waveTime1 > waveTime0) {
//							aA = (waveValue1 - waveValue0) / (pNIDAQmxAOCh->wavePatternTime[waveTemp + 1] - pNIDAQmxAOCh->wavePatternTime[waveTemp]);
							aA = (waveValue0 - waveValue1) / (pNIDAQmxAOCh->wavePatternTime[waveTemp] - pNIDAQmxAOCh->wavePatternTime[waveTemp + 1]);
							bB = waveValue1 - (aA * pNIDAQmxAOCh->wavePatternTime[waveTemp + 1]);
							for(iTime=waveTime0, j=0; iTime<waveTime1; iTime++,j++) {
								waveData = (aA *(iTime * sample_step)) + bB;
								if (NULL != pNIDAQmxAOCh->write_fp) {
									if(waveData > 9.03) {
										kLog (K_INFO, "[make_pattern_dataToFile] 1 Over Vlotage Error Cut the volt Ch(%d) point(%d) aA(%f) = (wV1(%f) -wV0(%f)) / (wPT1(%f) - wPT0(%f))	\n",
																				i, waveTemp, aA, waveValue1, waveValue0, pNIDAQmxAOCh->wavePatternTime[waveTemp + 1], pNIDAQmxAOCh->wavePatternTime[waveTemp]);		
										kLog (K_INFO, "[make_pattern_dataToFile] 1 Over Vlotage Error Cut the volt Ch(%d) point(%d) bB(%f) = (wV1(%f) -( aA(%f) * wPT1(%f)) \n",
																				i, waveTemp, bB, waveValue1, aA, pNIDAQmxAOCh->wavePatternTime[waveTemp + 1]);
										kLog (K_INFO, "[make_pattern_dataToFile] 1 Over Vlotage Error Cut the volt Ch(%d) point(%d) (wVal0:%f, wVal1 :%f, wT0(%d)wPT0(%f), wT1(%d)wPT1(%f), wData(fitdata)(%f) = aA(%f)*(iTime(%d)*step(%f))+bB(%f) loop counts(j) : %d ) \n",
											i, waveTemp, waveValue0, waveValue1, waveTime0,pNIDAQmxAOCh->wavePatternTime[waveTemp], waveTime1,pNIDAQmxAOCh->wavePatternTime[waveTemp + 1],waveData, aA, iTime, sample_step, bB, j);
										waveData = 0.0;
										fwrite(&waveData, sizeof(waveData), 1, pNIDAQmxAOCh->write_fp);
										fflush(pNIDAQmxAOCh->write_fp);
									} else if (waveData < 0) {
										kLog (K_INFO, "[make_pattern_dataToFile] 1 Low Vlotage Error Cut the volt Ch(%d) point(%d) (wVal0:%f, wVal1 :%f, wT0(%d)wPT0(%f), wT1(%d)wPT1(%f), wData(fitdata)(%f) = aA(%f)*(iTime(%d)*step(%f))+bB(%f) loop counts(j) : %d ) \n",
											i, waveTemp, waveValue0, waveValue1, waveTime0,pNIDAQmxAOCh->wavePatternTime[waveTemp], waveTime1,pNIDAQmxAOCh->wavePatternTime[waveTemp + 1],waveData, aA, iTime, sample_step, bB, j);
										waveData = 0.0;
										fwrite(&waveData, sizeof(waveData), 1, pNIDAQmxAOCh->write_fp);
										fflush(pNIDAQmxAOCh->write_fp);
									} else {
										fwrite(&waveData, sizeof(waveData), 1, pNIDAQmxAOCh->write_fp);
										fflush(pNIDAQmxAOCh->write_fp);
									}
								}
							}
							totalWriteTemp = totalWriteTemp + j;
							kLog (K_INFO, "[make_pattern_dataToFile] 'WaveTime1 > waveTime0' ? Ch(%d) point(%d) (iTime0 : %d, iTime1 : %d, totalWriteTemp(%d) loop counts(j) : %d ) \n",
															i, waveTemp, waveTime0, waveTime1, totalWriteTemp, j);
						} else  kLog (K_INFO, "[make_pattern_dataToFile] End of time ?Ch(%d) PointNo(%d) WaveTime1 is not bigger then waveTime0 (W0 : %f, W1 : %f) \n",
														i, waveTemp, pNIDAQmxAOCh->wavePatternTime[waveTemp], pNIDAQmxAOCh->wavePatternTime[waveTemp + 1]);
					} else {	// At this time, value0 and value1 data same, but time is increase. so, must write same value !!!
						//waveTime0 = (int32)(pNIDAQmxAOCh->wavePatternTime[waveTemp] * (int)pNIDAQmx->sample_rate); 		// time unit (sec) * sample rate
						//waveTime1 = (int32)(pNIDAQmxAOCh->wavePatternTime[waveTemp + 1] * (int)pNIDAQmx->sample_rate); 	// time unit (sec) +1 * sample rate
						if(waveTime1 > waveTime0) {
							for(iTime=waveTime0, j=0;iTime<waveTime1;iTime++,j++) {
								if (NULL != pNIDAQmxAOCh->write_fp) {
									if(pNIDAQmxAOCh->wavePatternValue[waveTemp] > 9.03) {
										kLog (K_INFO, "[make_pattern_dataToFile] 2 Over Vlotage Error Cut the volt Ch(%d) point(%d) (iTime0 : %d, iTime1 : %d, loop counts(j) : %d ) \n",
																																		i, waveTemp, waveTime0, waveTime1, j);
										pNIDAQmxAOCh->wavePatternValue[waveTemp] = 0.0;
										fwrite(&pNIDAQmxAOCh->wavePatternValue[waveTemp], sizeof(pNIDAQmxAOCh->wavePatternTime[0]), 1, pNIDAQmxAOCh->write_fp);
										fflush(pNIDAQmxAOCh->write_fp);
									} else if (pNIDAQmxAOCh->wavePatternValue[waveTemp] < 0) {
										kLog (K_INFO, "[make_pattern_dataToFile] 2 Low Vlotage Error Cut the volt Ch(%d) point(%d) (iTime0 : %d, iTime1 : %d, loop counts(j) : %d ) \n",
																																		i, waveTemp, waveTime0, waveTime1, j);
										pNIDAQmxAOCh->wavePatternValue[waveTemp] = 0.0;
										fwrite(&pNIDAQmxAOCh->wavePatternValue[waveTemp], sizeof(pNIDAQmxAOCh->wavePatternTime[0]), 1, pNIDAQmxAOCh->write_fp);
										fflush(pNIDAQmxAOCh->write_fp);
									} else {
										fwrite(&pNIDAQmxAOCh->wavePatternValue[waveTemp], sizeof(pNIDAQmxAOCh->wavePatternTime[0]), 1, pNIDAQmxAOCh->write_fp);
										fflush(pNIDAQmxAOCh->write_fp);
									}
								}
							}
							totalWriteTemp = totalWriteTemp + j;
							kLog (K_INFO, "[make_pattern_dataToFile] value data same 'WaveTime1 > waveTime0' ? Ch(%d) point(%d) (iTime0 : %d, iTime1 : %d, totalWriteTemp(%d) loop counts : %d ) \n",
																	 i, waveTemp, waveTime0, waveTime1, totalWriteTemp, j);
						} else  kLog (K_INFO, "[make_pattern_dataToFile] End of time ? Ch(%d) PointNo(%d) WaveTime1 is not bigger then waveTime0 (W0 : %f, W1 : %f) \n",
												i, waveTemp, pNIDAQmxAOCh->wavePatternTime[waveTemp], pNIDAQmxAOCh->wavePatternTime[waveTemp + 1]);
					}	
				}
				if (totalWriteTemp < pNIDAQmx->totalWrite) {
					waveData = 0.0;
					for(iTime=totalWriteTemp, j=0; iTime < pNIDAQmx->totalWrite; iTime++, j++) {
						if (NULL != pNIDAQmxAOCh->write_fp) {
							fwrite(&waveData, sizeof(waveData), 1, pNIDAQmxAOCh->write_fp);
							fflush(pNIDAQmxAOCh->write_fp);
						}
					}
					 kLog (K_INFO, "[make_pattern_dataToFile] How 'totalWriteTemp < pNIDAQmx->totalWrite' ? Ch(%d) (totalWrtieTemp : %d + zeroItime(%d) , pNIDAQmx->totalWrite : %d) \n",  i, totalWriteTemp, j, pNIDAQmx->totalWrite);
				} else  kLog (K_INFO, "[make_pattern_dataToFile] 'totalWriteTemp >= pNIDAQmx->totalWrite' ? Ch(%d) (totalWrtieTemp : %d, pNIDAQmx->totalWrite : %d) \n",
										i, totalWriteTemp, pNIDAQmx->totalWrite); 
			}
			kLog (K_INFO, "[make_pattern_dataToFile] 'At End of Ch(%d) Compare (totalWrtieTemp : %d, and  pNIDAQmx->totalWrite : %d) \n",i, totalWriteTemp, pNIDAQmx->totalWrite);
		}	
	} else kLog (K_INFO, "[close_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType);
}
void make_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);

	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;
	int chanNum;
/*  Remove this function TGLee. I just over write same file
	char mkdirbuf[256];
	sprintf (mkdirbuf, "%sS%06d", STR_LOCAL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber);
	mkdir (mkdirbuf, 0755);
*/
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	if(0 == strcmp(pNIDAQmx->sigType, "ai")){
//		pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAICh = pSTDCh->pUser;
				pNIDAQmxAICh->write_fp = NULL;

				if (CH_USED == pNIDAQmxAICh->used) {
					sprintf (pNIDAQmxAICh->path_name,"%s%s",
							STR_LOCAL_DATA_DIR, pNIDAQmxAICh->file_name);
					kLog (K_INFO, "[make_local_file] AI fname(%s)\n", pNIDAQmxAICh->path_name);

					// read & write (trunc)
					if (NULL == (pNIDAQmxAICh->write_fp = fopen(pNIDAQmxAICh->path_name, "w+"))) {
						kLog (K_ERR, "[make_local_file] AI %s create failed!!!\n", pNIDAQmxAICh->path_name);
						notify_error (1, "%s create AI failed!\n", pNIDAQmxAICh->path_name);
					} else kLog (K_INFO, "[make_local_file] AI fname(%s) sucessed\n", pNIDAQmxAICh->path_name);
					if (pNIDAQmxAICh->inputType == DENSITY_CALC) {
						pNIDAQmxAICh->write_den_fp = NULL;
						sprintf (pNIDAQmxAICh->path_den_name, "%s%s",pNIDAQmxAICh->path_name, "Density");
						kLog (K_INFO, "[make_local_file] AI density fname(%s)\n", pNIDAQmxAICh->path_den_name);
						if (NULL == (pNIDAQmxAICh->write_den_fp = fopen(pNIDAQmxAICh->path_den_name, "w+"))) {
							kLog (K_ERR, "[make_local_file] AI density %s create failed!!!\n", pNIDAQmxAICh->path_den_name);
							notify_error (1, "%s create AI density file failed!\n", pNIDAQmxAICh->path_den_name);
						}
					}
				}
//				pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
			}
		}
	} else kLog (K_INFO, "[make_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType); 
}

void AO_Make_Status_Progress(ST_STD_device *pSTDdev)
{
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;

	kLog (K_TRACE, "[AO_Make_Status_Progress] %s ...\n", pSTDdev->taskName);

	if(0 == strcmp(pNIDAQmx->sigType, "ao")){
		if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
			pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
			pSTDdev->StatusDev |= TASK_IN_PROGRESS;

			kLog (K_INFO, "[AO_Make_Status_Progress] %s : status(0x%x:%s)\n",
					pSTDdev->taskName, pSTDdev->StatusDev, getStatusMsg(pSTDdev->StatusDev, NULL));

			notify_refresh_master_status();
			scanIoRequest(pSTDdev->ioScanPvt_status);
		}

	} else kLog (K_INFO, "[AO_Make_Status_Progress] Not yet support ? sigType (%s) fail \n", pNIDAQmx->sigType);
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;
	int chanNum;
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);


	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);

	if(0 == strcmp(pNIDAQmx->sigType, "ai")){
//		pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAICh = pSTDCh->pUser;
				if (NULL != pNIDAQmxAICh->write_fp) {
					fflush(pNIDAQmxAICh->write_fp);
				}
				if (pNIDAQmxAICh->inputType == DENSITY_CALC) {
					if (NULL != pNIDAQmxAICh->write_den_fp) {
						fflush(pNIDAQmxAICh->write_den_fp);
					}
				}
//				pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
			}
		}
	} else kLog (K_INFO, "[flush_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType);
}

void close_local_file(ST_STD_device *pSTDdev)
{
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i;
	int chanNum;
	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	if(0 == strcmp(pNIDAQmx->sigType, "ai")){
//		pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0;i<chanNum;i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				pNIDAQmxAICh = pSTDCh->pUser;
				if (NULL != pNIDAQmxAICh->write_fp) {
					fclose(pNIDAQmxAICh->write_fp);
					pNIDAQmxAICh->write_fp	= NULL;
				}
				if (pNIDAQmxAICh->inputType == DENSITY_CALC) {
					if (NULL != pNIDAQmxAICh->write_den_fp) {
						fclose(pNIDAQmxAICh->write_den_fp);
						pNIDAQmxAICh->write_den_fp	= NULL;
					}
				}
				//			pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
			}
		}
	} else kLog (K_INFO, "[close_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType);
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

int drvNIDAQmx_AO_DAQ_VALUE (ST_STD_device *pSTDdev)
{
	int chanNum;
	int nDataBufSize;
	int32 	writtenSampleChan;
	int i;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;

    chanNum = ellCount(pSTDdev->pList_Channel);
	float64 *databuf = NULL; /* for read ao data float64 */

	if(pNIDAQmx->smp_mode_select == 1){
		if ((0 != pNIDAQmx->taskHandle) && (pNIDAQmx->protect_stop_flag == 1)) {
			nDataBufSize = sizeof(float64) * chanNum; // 1(points) * chanNum // read data for float64 data
			databuf = NULL; /* for read ao data float64 */
			if (NULL == (databuf = (float64 *)malloc(nDataBufSize))) {
				kLog (K_ERR, "[drvNIDAQmx_AO_DAQ_VALUE] AO %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nDataBufSize);
				notify_error (1, "malloc*%d) failed", (int)nDataBufSize);
				return (WR_ERROR);
			}
			// I used ellFirst funcition because "get_STCh_from_STDev_chID()" function always search, I need always all of channel data 
			pSTDCh = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
			for (i=0;i<chanNum;i++) {
				pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
				databuf[i] = pNIDAQmxAOCh->voltOut;
				pSTDCh = (ST_STD_channel*) ellNext(&pSTDCh->node);
			}
			NIERROR(		DAQmxWriteAnalogF64 (
						pNIDAQmx->taskHandle,   // taskHandle
						1,             // numSampsPerChan. number of samples per channel to write
						0,             //0-need TaskStart, 1-not need TaskStart, autoStart(bool32) automatically starts the task if you do not start
						10.0,                  // timeout (seconds)
						DAQmx_Val_GroupByChannel,// DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved), DAQmx_Val_GroupByChannel (non-interleaved)
						databuf,                // writeArray. array of 64-bit samples to write to the task
						(void *)&writtenSampleChan,  // actual number of samples per channel successfully writen to the buff
						NULL                      // reserved for future use
						)
			);
			kLog (K_MON, "[drvNIDAQmx_AO_DAQ_VALUE] %s : analog output  writtenSampleChan Successfully(%d)\n", pSTDdev->taskName, writtenSampleChan);
			free(databuf);
		} else  kLog (K_MON, "[drvNIDAQmx_AO_DAQ_VALUE] %s is Not Start Task(flag is (%d) \n", pSTDdev->taskName, pNIDAQmx->protect_stop_flag);
	} else	kLog (K_MON, "[drvNIDAQmx_AO_DAQ_VALUE] %s is Not Run-Time mode(%d) \n", pSTDdev->taskName, pNIDAQmx->smp_mode_select);

	return (0);
}
int stopDevice (ST_STD_device *pSTDdev)
{	
	int chanNum;
	int nDataBufSize;
	int32 	writtenSampleChan;
	int i;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	kLog (K_TRACE, "[drvNIDAQmx_stopDevice] ...\n");
	float64 *databuf = NULL; /* for read ao data float64 */
    chanNum = ellCount(pSTDdev->pList_Channel);

	if (0 != pNIDAQmx->taskHandle) {
		kLog (K_MON, "[stopDevice] %s : total(%d)\n", pSTDdev->taskName, pNIDAQmx->totalRead);

		if(0 == strcmp(pNIDAQmx->sigType, "ai")){
			DAQmxTaskControl(pNIDAQmx->taskHandle, DAQmx_Val_Task_Abort);
			NIERROR(DAQmxStopTask(pNIDAQmx->taskHandle));

			kLog (K_INFO, "[stopDevice] before DAQmxClearTask for %s\n", pSTDdev->taskName);
			NIERROR(DAQmxClearTask(pNIDAQmx->taskHandle));
			kLog (K_INFO, "[stopDevice] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
			pNIDAQmx->taskHandle = 0;

			if (pNIDAQmx->protect_stop_flag == 1) {
				kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

				epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
				pNIDAQmx->protect_stop_flag = 0;
				// wait some times to synchronize running mode with device threads
				epicsThreadSleep (0.5);
			}
		} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
			if((pNIDAQmx->smp_mode_select == 1) && (pNIDAQmx->protect_stop_flag == 1)){
				nDataBufSize = sizeof(float64) * chanNum; // 1(points) * chanNum // read data for float64 data
				databuf = NULL; /* for read ao data float64 */
				if (NULL == (databuf = (float64 *)malloc(nDataBufSize))) {
					kLog (K_ERR, "[stopDevice] AO %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nDataBufSize);
					notify_error (1, "malloc*%d) failed", (int)nDataBufSize);
					return (WR_ERROR);
				}
				// I used ellFirst funcition because "get_STCh_from_STDev_chID()" function always search, I need always all of channel data 
				for (i=0;i<chanNum;i++) {
					databuf[i] = 0.0;
				}
				NIERROR(		DAQmxWriteAnalogF64 (
						pNIDAQmx->taskHandle,   // taskHandle
						1,             // numSampsPerChan. number of samples per channel to write
						0,             //0-need TaskStart, 1-not need TaskStart, autoStart(bool32) automatically starts the task if you do not start
						10.0,                  // timeout (seconds)
						DAQmx_Val_GroupByChannel,// DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved), DAQmx_Val_GroupByChannel (non-interleaved)
						databuf,                // writeArray. array of 64-bit samples to write to the task
						(void *)&writtenSampleChan,  // actual number of samples per channel successfully writen to the buff
						NULL                      // reserved for future use
						)
				);
				// To avoid other process control
				// If not used sleep 10usec(over or less ??), it is not working.
				epicsThreadSleep (0.00001);
				DAQmxTaskControl(pNIDAQmx->taskHandle, DAQmx_Val_Task_Abort);
				NIERROR(DAQmxStopTask(pNIDAQmx->taskHandle));

				kLog (K_INFO, "[stopDevice] before DAQmxClearTask for %s\n", pSTDdev->taskName);
				NIERROR(DAQmxClearTask(pNIDAQmx->taskHandle));
				kLog (K_INFO, "[stopDevice] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
				pNIDAQmx->taskHandle = 0;

				if (pNIDAQmx->protect_stop_flag == 1) {
					kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

					epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
					pNIDAQmx->protect_stop_flag = 0;
					// wait some times to synchronize running mode with device threads
					epicsThreadSleep (0.5);
				}
				kLog (K_MON, "[stopDevice] %s : analog output init Value zero(0) writtenSampleChan Successfully(%d)\n", pSTDdev->taskName, writtenSampleChan);
				free(databuf);
				// we need initialize analog output data value with PV and buffer when we finish the NIDAQmx task stop and clear
				DBproc_put(PV_ECH_AO_OUTPUT_VALUE_CH0_STR, "0");
				DBproc_put(PV_ECH_AO_OUTPUT_VALUE_CH1_STR, "0");
				DBproc_put(PV_ECH_AO_OUTPUT_VALUE_CH2_STR, "0");
				DBproc_put(PV_ECH_AO_OUTPUT_VALUE_PWR_CH0_STR, "0");  // FLNK to the output value ch0 and ch1 about "zero" value 
				epicsThreadSleep (0.1);
			} else if((pNIDAQmx->smp_mode_select == 0) && (pNIDAQmx->protect_stop_flag == 1)){
				if (0 != pNIDAQmx->taskHandle) {
					DAQmxTaskControl(pNIDAQmx->taskHandle, DAQmx_Val_Task_Abort);
					NIERROR(DAQmxStopTask(pNIDAQmx->taskHandle));

					kLog (K_INFO, "[stopDevice] before DAQmxClearTask for %s\n", pSTDdev->taskName);
					NIERROR(DAQmxClearTask(pNIDAQmx->taskHandle));
					kLog (K_INFO, "[stopDevice] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
					pNIDAQmx->taskHandle = 0;
				}
				if (pNIDAQmx->protect_stop_flag == 1) {
					kLog (K_INFO, "[stopDevice] %s : signal(0x%x)\n", pSTDdev->taskName, pSTDdev->ST_DAQThread.threadEventId);

					epicsEventSignal(pSTDdev->ST_DAQThread.threadEventId);
					pNIDAQmx->protect_stop_flag = 0;
					// wait some times to synchronize running mode with device threads
					epicsThreadSleep (0.5);
				}
			} else kLog (K_MON, "[drvNIDAQmx_AO_DAQ_VALUE] %s is Not Start Task(flag is (%d) \n", pSTDdev->taskName, pNIDAQmx->protect_stop_flag);
		} else kLog (K_INFO, "[close_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType);
	} else kLog (K_MON, "[drvNIDAQmx_AO_DAQ_VALUE] %s is Not yet create Task(flag is (%d) \n", pSTDdev->taskName, pNIDAQmx->protect_stop_flag);
	return (0);
}

int resetDevice (ST_STD_device *pSTDdev)
{
	stopDevice (pSTDdev);

	pSTDdev->StatusDev = TASK_SYSTEM_IDLE | TASK_STANDBY;

	notify_refresh_master_status ();

	return (0);
}

int stopDeviceAll ()
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	kLog (K_MON, "[stopDeviceAll] ...\n");

	while (pSTDdev) {
		stopDevice (pSTDdev);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
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
int drvNIDAQmx_set_TagName(ST_STD_device *pSTDdev)
{
	int i;
	int chanNum;
	ST_NIDAQmx *pNIDAQmx = (ST_NIDAQmx *)pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	kLog (K_TRACE, "[drvNIDAQmx_set_TagName] ...\n");
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	if(0 == strcmp(pNIDAQmx->sigType, "ai")){
//		pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0; i<chanNum; i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				kLog (K_TRACE, "[drvNIDAQmx_AI_set_TagName_to_Check_TG] task(%s) STDchID(%d) inputTagName(%s) sigType(%s) strArg2(%s) channelCounts(%d)\n",
						pSTDdev->taskName, pSTDCh->channelId, pSTDCh->strArg1, pSTDCh->strArg0, pSTDCh->strArg2, chanNum);
				pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
				sprintf(pNIDAQmxAICh->strTagName, "\\%s:FOO", pNIDAQmxAICh->inputTagName);
				kLog (K_INFO, "AI channel Store  to ch(%d) ChID(%d) Node(%s) fileName(%s)\n", i, pNIDAQmxAICh->channelId, pNIDAQmxAICh->strTagName, pNIDAQmxAICh->file_name);
				//pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
			}
		}
	} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
//		pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) ellFirst(pNIDAQmx->pchannelConfig);
		for(i=0; i<chanNum; i++) {
			if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
				kLog (K_TRACE, "[drvNIDAQmx_AO_set_TagName_to_Check_TG] task(%s) STDchID(%d) inputTagName(%s) sigType(%s) strArg2(%s) channelCounts(%d)\n",
						pSTDdev->taskName, pSTDCh->channelId, pSTDCh->strArg1, pSTDCh->strArg0, pSTDCh->strArg2, chanNum);
				pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
				sprintf(pNIDAQmxAOCh->strTagName, "\\%s:FOO", pNIDAQmxAOCh->inputTagName);
				kLog (K_INFO, "AO channel Store  to ch(%d) ChID(%d) Node(%s) fileName(%s) point(%p)\n", i, pNIDAQmxAOCh->channelId, pNIDAQmxAOCh->strTagName,
							 pNIDAQmxAOCh->file_name, pNIDAQmxAOCh);
//				pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) ellNext(&pNIDAQmxAOCh->node);
			}
		}
	} else kLog (K_INFO, "[close_local_file] Not yet support sigType (%s) fail \n", pNIDAQmx->sigType);

	return WR_OK;
}

// starts DAQ for trending : this funciton call after shot by sfw.
void func_NIDAQmx_POST_SEQSTOP(void *pArg, double arg1, double arg2)
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
void func_NIDAQmx_PRE_SEQSTART(void *pArg, double arg1, double arg2)
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

void func_NIDAQmx_SYS_RESET(void *pArg, double arg1, double arg2)
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

void func_NIDAQmx_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER               *pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQmx *pNIDAQmx = (ST_NIDAQmx *)pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int i, j;
	int sampleRate;
	int chanNum;
	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	sampleRate = pNIDAQmx->sample_rate;

	if ((OPMODE_REMOTE == pSTDdev->ST_Base.opMode) && (pNIDAQmx->auto_run_flag == 0)) {
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
		if (TRUE == pNIDAQmx->trend_run_flag) {
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		}
#endif
	}
	else { /* release arming condition */
		if (admin_check_Release_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : arming released. status(%s)\n",
						pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming released. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			if(0 == strcmp(pNIDAQmx->sigType, "ai")){
				for (i=0;i<chanNum;i++) {
					if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
						pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
						if(pNIDAQmxAICh->inputType == DENSITY_CALC) {  /* init density data channel */
							pNIDAQmxAICh->isFirst = 0;	
							pNIDAQmxAICh->density = 0;
							for(j=0; j<sampleRate; j++) {
								pNIDAQmxAICh->calcDensity[j] = 0;	
							}
							scanIoRequest(pSTDdev->ioScanPvt_userCall);
						}
					}
				}
				return;
			} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
/*   // Not yet any function TG
				for(i=0; i<chanNum; i++) {
					if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
						pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
					}
				}
*/
				return;
			} else kLog (K_ERR, "[SYS_ARMING] %s : release failed. It is Not sigType AI or AO  status(%s)\n", pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
		} else {
			if(0 == strcmp(pNIDAQmx->sigType, "ai")){
				for (i=0;i<chanNum;i++) {
					if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
						pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
						if(pNIDAQmxAICh->inputType == DENSITY_CALC) {  /* init density data channel */
							pNIDAQmxAICh->isFirst = 0;	
							pNIDAQmxAICh->density = 0;
							for(j=0; j<sampleRate; j++) {
								pNIDAQmxAICh->calcDensity[j] = 0;	
							}
							scanIoRequest(pSTDdev->ioScanPvt_userCall);
						}
					}
				}
			} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
/*   // Not yet any function TG
				for(i=0; i<chanNum; i++) {
					if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
						pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
					}
				}
*/
			} else kLog (K_ERR, "[SYS_ARMING] %s : released. It is Not sigType AI or AO  status(%s)\n", pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
		}
		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_NIDAQmx_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQmx *pNIDAQmx = (ST_NIDAQmx *)pSTDdev->pUser;

	kLog (K_MON, "[SYS_RUN] %s : %d\n", pSTDdev->taskName, (int)arg1);

	if ((OPMODE_REMOTE == pSTDdev->ST_Base.opMode) && (pNIDAQmx->auto_run_flag == 0)) {
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

void func_NIDAQmx_OP_MODE(void *pArg, double arg1, double arg2)
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
void func_NIDAQmx_DATA_SEND(void *pArg, double arg1, double arg2)
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
void func_NIDAQmx_CREATE_LOCAL_SHOT(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_SHOT] %s shot(%f)\n", pSTDdev->taskName, arg1);

	createLocalMdsTree (pSTDdev);
}

void func_NIDAQmx_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_NIDAQmx_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQmx *pNIDAQmx = NULL;

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;
	pNIDAQmx->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = arg1;
	scanIoRequest(pSTDdev->ioScanPvt_userCall);
}

void func_NIDAQmx_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQmx *pNIDAQmx = NULL;

	kLog (K_MON, "[func_NIDAQmx_T0] %s : start_t0(%f) \n", pSTDdev->taskName, arg1);

	pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;

	pNIDAQmx->start_t0 = arg1;
	setSamplTime (pSTDdev);
/*	setSamplTime (pNIDAQmx);      */
/*	setStartT0 (pNIDAQmx, arg1);  */
}

void func_NIDAQmx_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_NIDAQmx *pNIDAQmx = NULL;

	kLog (K_MON, "[func_NIDAQmx_T1] %s : stop_t1(%f) \n", pSTDdev->taskName, arg1);

	pNIDAQmx =(ST_NIDAQmx *)pSTDdev->pUser;

	pNIDAQmx->stop_t1 = arg1;
	setSamplTime (pSTDdev);     
/*	setSamplTime (pNIDAQmx);      */
/*	setStopT1 (pNIDAQmx, arg1);   */
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	ST_STD_device	*pSTDdev = (ST_STD_device*) callbackData;

//	kLog (K_INFO,"[ EveryNCallback ] : measured function process time sec. is %lf msec\n", 1.E-3 * (double)wf_intervalUSec(start,stop));

	kLog (K_INFO, "[CVICALLBACK DoneCallback] for AO signal, sampleMode is FiniteSampes %s : stop ....\n", pSTDdev->taskName);
//	stopDevice (pSTDdev); remove because avoid double stopDevice function in here and AI acquire done.

	return 0;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	ST_STD_device	*pSTDdev = (ST_STD_device*) callbackData;
	ST_NIDAQmx		*pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int 	i;
	int 	chanNum;
	int32	sampsRead=0;
	int32	numSampsPerChan = getNumSampsPerChan(pNIDAQmx);
	int32	totSampsAllChan	= getTotSampsAllChan(pSTDdev);
/* TG 	int32	totSampsAllChan	= getTotSampsAllChan(pNIDAQmx);  */

	unsigned long long int writeFileStart;
	unsigned long long int writeFileStop;
	unsigned long long int readDataStart;
	unsigned long long int readDataStop;
	unsigned long long int start;
	unsigned long long int stop;
	start = wf_getCurrentUsec();
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

#if !USE_SCALING_VALUE
	int32	numBytesPerSamp = 0;
	int32 	arraySizeInBytes = getArraySizeInBytes(pNIDAQmx);
#endif

	kLog (K_MON, "[EveryNCallback] %s : type(%d) nSamples(%d) rate(%.f) status(%s) total(%d) mode(%d) shot(%lu/%lu)\n",
			pSTDdev->taskName, everyNsamplesEventType, nSamples, pNIDAQmx->sample_rate,
			getStatusMsg(pSTDdev->StatusDev, NULL),
			pNIDAQmx->totalRead, isCalcRunMode(pSTDdev), get_master()->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

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
	 kLog (K_MON,"[ EveryNCallback- read DAQmxReadAnalogF64 1] : measured function process time sec. is %lf msec\n",
				1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
	NIERROR(
		DAQmxReadAnalogF64 (
			pNIDAQmx->taskHandle, 			// taskHandle
			numSampsPerChan, 				// numSampsPerChan. number of samples per channel to read
			600.0, 							// timeout (seconds)
			DAQmx_Val_GroupByChannel,	// DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved), DAQmx_Val_GroupByChannel (non-interleaved)
			pNIDAQmx->fScaleReadArray, 		// readAray. array into which samples are read
			sizeof(pNIDAQmx->fScaleReadArray),	// arraySizeInBytes. size of the array into which samples are read
			(void *)&sampsRead, 			// sampsRead. actual number of bytes read into the array per scan
			NULL							// reserved
		)
	);


/*
	if (TRUE == isPrintDebugMsg (K_DATA)) {
		for (i=0;i<numSampsPerChan;i++) {
			if ( (i > 0) && ! (i % 10)) printf ("\n");
			printf ("%.2f ", pNIDAQmx->fScaleReadArray[i*chanNum]);
		}
		printf ("\n");
	}
*/
	 kLog (K_MON,"[ EveryNCallback- read DAQmxReadAnalogF64 2] : measured function process time sec. is %lf msec\n",
				1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
	// converts scaling eu value to raw value of 14bits resolution
	 for (i=0;i<totSampsAllChan;i++) {
		 pNIDAQmx->readArray[i] = GET_RAW_VALUE(pNIDAQmx->fScaleReadArray[i]);
		 /*		pNIDAQmx->readArray[i] = GET_RAW14_VALUE(pNIDAQmx->fScaleReadArray[i]);   converts scaling eu value to raw value of 14bits resolution */
		 /*		 converts scaling eu value to raw value of 16bits resolution
				 pNIDAQmx->readArray[i] = GET_RAW_VALUE(pNIDAQmx->fScaleReadArray[i]);
		  */
	 }
	 kLog (K_MON,"[ EveryNCallback- read DAQmxReadAnalogF64 3] : measured function process time sec. is %lf msec\n",
				1.E-3 * (double)wf_intervalUSec(readDataStart,readDataStop));
#else
	NIERROR(
		DAQmxReadRaw (
			pNIDAQmx->taskHandle, 	// taskHandle
			numSampsPerChan, 		// numSampsPerChan. number of samples per channel to read
			600.0, 					// timeout (seconds)
			pNIDAQmx->readArray, 	// readAray. array into which samples are read
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
			printf ("%d ", pNIDAQmx->readArray[i*chanNum]);
		}
		printf ("\n");
	}

	pNIDAQmx->totalRead	+= sampsRead;
	FILE *fp = NULL;
	int chNo;

	int32 arrayJump;
	int32 arrayJumpCurrent;

//	pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
    /* When use this function -  DAQmx_Val_GroupByChannel (non-interleaved) at DAQmxReadAnalogF64 */
	for(chNo=0;	chNo<chanNum;	chNo++) {
		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,chNo))) {
			pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
			arrayJump = chNo * numSampsPerChan;
			arrayJumpCurrent = (chNo +1) * numSampsPerChan - 1; /* This pupose is last value display in the pv  */

			/*		epicsPrintf("[ EveryNCallback - write raw data to local file chNu :%d ] : measured function process time sec. is %lf msec\n", 
					chNo, 1.E-3 * (double)wf_intervalUSec(writeFileStart,writeFileStop));
			 */
			pNIDAQmxAICh->rawValue  = pNIDAQmx->readArray[arrayJumpCurrent];
			pNIDAQmxAICh->euValue  = pNIDAQmx->fScaleReadArray[arrayJumpCurrent];

			writeFileStart = wf_getCurrentUsec();
			if(pNIDAQmx->trend_run_flag==0){
				// protected file point value is NULL, even if trend_run_flag value is 1.
				if (NULL != (fp = pNIDAQmxAICh->write_fp)) {
					fwrite(&pNIDAQmx->readArray[arrayJump], sizeof(pNIDAQmx->readArray[0]), numSampsPerChan, pNIDAQmxAICh->write_fp);
					fflush(pNIDAQmxAICh->write_fp);
				}
				writeFileStop = wf_getCurrentUsec();

			}
//			pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
		}
	}
	
#if 0 
		/* change write to file routine */
		for(i=0; i<numSampsPerChan; i++) {
			if (NULL != (fp = pNIDAQmx->ST_Ch[chNo].write_fp)) {
				// write to file
				fwrite(&pNIDAQmx->readArray[arrayJump+i], sizeof(pNIDAQmx->readArray[0]), 1, pNIDAQmx->ST_Ch[chNo].write_fp);
				fflush(pNIDAQmx->ST_Ch[chNo].write_fp);

				if (0 == (i % 10)) {
					kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
							pSTDdev->taskName, chNo, pNIDAQmx->ST_Ch[chNo].file_name, pNIDAQmx->readArray[arrayJump+i]);
				}
			}
		}
#endif


#if 0
    /* When use this function -  DAQmx_Val_GroupByScanNumber (Group by scan number (interleaved) at DAQmxReadAnalogF64 */
	for (i=0;i<totSampsAllChan;i++) {
		chNo = GET_CHID(i);

		pNIDAQmx->ST_Ch[chNo].rawValue  = pNIDAQmx->readArray[i];
		pNIDAQmx->ST_Ch[chNo].euValue  = pNIDAQmx->fScaleReadArray[i];
		// offset compensation
		/* remove ... because I used not offset value */

		if (NULL != (fp = pNIDAQmx->ST_Ch[chNo].write_fp)) {
			// write to file
			fwrite(&pNIDAQmx->readArray[i], sizeof(pNIDAQmx->readArray[0]), 1, pNIDAQmx->ST_Ch[chNo].write_fp);
			fflush(pNIDAQmx->ST_Ch[chNo].write_fp);

			if (0 == chNo) {
				kLog (K_DEL, "[EveryNCallback] %s : chNo(%d) fname(%s) raw(%d)\n",
						pSTDdev->taskName, chNo, pNIDAQmx->ST_Ch[chNo].file_name, pNIDAQmx->readArray[i]);
			}
		}

		// process input data : offset compensation, eu conversion, integral
		/* remove TG.Lee processInputData (isCalcRunMode(), pNIDAQmx, chNo, pNIDAQmx->readArray[i]);  */
	}
#endif
	stop = wf_getCurrentUsec();
	kLog (K_INFO,"[ EveryNCallback ] : measured function process time sec. is %lf msec\n", 1.E-3 * (double)wf_intervalUSec(start,stop));

	scanIoRequest(pSTDdev->ioScanPvt_userCall);

	if (TRUE == isSamplingStop (pSTDdev)) {
		kLog (K_INFO, "[EveryNCallback] %s : stop ....\n", pSTDdev->taskName);
		//stopDevice (pSTDdev); to used ao stop every function stop (pre-define AO or Real-Time AO), I don't know finish time when Real-time AO finish time
		stopDeviceAll();
	} 
	return 0;
}

float64 average(ST_NIDAQmx *pNIDAQmx, int chNo)
{
	float64 sum = 0.0;
	int i;
	int32	numSampsPerChan = getNumSampsPerChan(pNIDAQmx);
	if (numSampsPerChan > 1000) numSampsPerChan=1000;
	for (i=0; i<numSampsPerChan; i++)
	{
	}
	return (sum / numSampsPerChan);
}
void threadFunc_NIDAQmx_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NIDAQmx *pNIDAQmx = pSTDdev->pUser;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];
	int i;
	int chanNum;
	unsigned long long int start;
	unsigned long long int stop;
	int32 	writtenSampleChan;
	int32	totSampsAllChan = 0;
	int32	numSampsPerChan = 0;
	int     nDataBufSize; // numSampsPerChan * chanNum or totSampsAllChan // read data for float64 data
	float64 *databuf = NULL; /* for read ao data float64 */

	kLog (K_TRACE, "[NIDAQmx_DAQ] dev(%s)\n", pSTDdev->taskName);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_NIDAQmx_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNIDAQmx->sample_rate;
	n8After_Processing_Flag = 0;

	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		kLog (K_MON, "[NIDAQmx_DAQ] %s : mode(%s) rate(%.f) time(%f) status(%s) shot(%lu/%lu)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNIDAQmx->sample_rate, pNIDAQmx->sample_time,
				getStatusMsg(pSTDdev->StatusDev, statusMsg),
				pMaster->ST_Base.shotNumber, pSTDdev->ST_Base_fetch.shotNumber);

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_MON, "[NIDAQmx_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		kLog (K_INFO, "[NIDAQmx_DAQ] %s : mode(%s) rate(%.f) time(%f-%f = %f) \n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
				pNIDAQmx->sample_rate,
				pNIDAQmx->stop_t1, pNIDAQmx->start_t0, pNIDAQmx->sample_time);

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNIDAQmx->taskHandle == 0)) {
				/*	processAutoTrendRun (pSTDdev);   */
			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNIDAQmx->taskHandle == 0)) {

			}	
			else if(pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
				stopDevice (pSTDdev);

				NIERROR(DAQmxCreateTask("",&pNIDAQmx->taskHandle));

				kLog (K_INFO, "[NIDAQmx_DAQ] %s : term_config(%d) minVal(%.f) maxVal(%.f) units(%d)\n",
						pSTDdev->taskName, pNIDAQmx->terminal_config, pNIDAQmx->minVal, pNIDAQmx->maxVal, pNIDAQmx->units);

				if(0 == strcmp(pNIDAQmx->sigType, "ai")){
//					pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellFirst(pNIDAQmx->pchannelConfig);
					for (i=0;i<chanNum;i++) {
						if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))){ 
							pNIDAQmxAICh =(ST_NIDAQmx_AIchannel *)pSTDCh->pUser;
							if(pNIDAQmxAICh->inputType == STRAIN_DATA){
								kLog (K_INFO, "[NIDAQmx_DAQ] STRAIN DATA in DAQmxCreateAIStrainGageChan : ch(%d), physical_channelSCXI(%s) minVal(%f) maxVal(%f)\n",
										i,  pNIDAQmxAICh->physical_channelSCXI, pNIDAQmx->minVal, pNIDAQmx->maxVal);

								NIERROR ( DAQmxCreateAIStrainGageChan ( pNIDAQmx->taskHandle,
											pNIDAQmxAICh->physical_channelSCXI,
											NULL,
											pNIDAQmx->minVal,   //Min Range 
											pNIDAQmx->maxVal,   //Max Range
											DAQmx_Val_Strain,	   //unit
											DAQmx_Val_QuarterBridgeII, //bridgeType
											DAQmx_Val_Internal,        //excitationSource
											6.0,                       //excitationVoltage
											2.03,                      //gageFactor
											0.0,                       //initialBridgeVoltage
											350.0,                     //nominalGageResistance
											0.30,                      //poissonRate
											0.0,                       //leadWireResistance
											NULL
											)	
									);
							} else {
								kLog (K_INFO, "[NIDAQmx_DAQ] AI DATA in DAQmxCreateAIVoltageChan : ch(%d), physical_channel(%s) minVal(%.f) maxVal(%f)\n",
										i,  pNIDAQmxAICh->physical_channel, pNIDAQmx->minVal, pNIDAQmx->maxVal);
								NIERROR (
										DAQmxCreateAIVoltageChan (
											pNIDAQmx->taskHandle,			// taskHandle
											pNIDAQmxAICh->physical_channel,	        // physicalChannel
											"",					// nameToAssignToChannel
											pNIDAQmx->terminal_config,		// terminalConfig
											pNIDAQmx->minVal,			// minVal
											pNIDAQmx->maxVal,			// maxVal
											pNIDAQmx->units,				// units
											NULL					// customScaleName
											)
									);
								if(pNIDAQmxAICh->inputType == DENSITY_CALC) {  /* init density data channel */
									pNIDAQmxAICh->isFirst = 0;	
									pNIDAQmxAICh->density = 0;	
									scanIoRequest(pSTDdev->ioScanPvt_userCall);
								}
							}
//							pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) ellNext(&pNIDAQmxAICh->node);
							kLog (K_INFO, "[NIDAQmx_DAQ] DAQmxCreateAIVoltageChan : %s\n", pNIDAQmxAICh->physical_channel);
						}
					}
					/*      This Function have to perform after  Channel created.. ellNext - like Ring buffer */
					if(pNIDAQmxAICh->inputType != STRAIN_DATA){
						/* SCXI type is Backplan clock used by 333kHz. and it is depend on the number of channels  */
						NIERROR(DAQmxGetSampClkMaxRate(pNIDAQmx->taskHandle, &(pNIDAQmx->sample_rateLimit)));
					}
					totSampsAllChan	= getTotSampsAllChan(pSTDdev);
					numSampsPerChan = getNumSampsPerChan(pNIDAQmx);

					// if (TRUE == isTriggerRequired (pSTDdev)) {
						NIERROR(
								DAQmxCfgSampClkTiming(
									pNIDAQmx->taskHandle,	// taskHandle
									pNIDAQmx->clockSource,	// source terminal of the Sample clock. NULL(internal)
									pNIDAQmx->sample_rate,	// sampling rate in samples per second per channel
									DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
									pNIDAQmx->smp_mode,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
									totSampsAllChan		// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
									)
						       );
				/*	} else {
						NIERROR(
								DAQmxCfgSampClkTiming(
									pNIDAQmx->taskHandle,	// taskHandle
									"",			// source terminal of the Sample clock. NULL(internal)
									pNIDAQmx->sample_rate,	// sampling rate in samples per second per channel
									DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
									pNIDAQmx->smp_mode,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
									totSampsAllChan		// FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
									)
						       );
						kLog (K_MON, "[NIDAQmx_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
					}
					*/
					kLog (K_INFO, "[NIDAQmx_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
							pSTDdev->taskName,
							getModeMsg(pSTDdev->ST_Base.opMode),
							pNIDAQmx->smp_mode, pNIDAQmx->sample_rate, pNIDAQmx->sample_time,
							numSampsPerChan, 
							totSampsAllChan);

			//		if (TRUE == isTriggerRequired (pSTDdev)) {
						NIERROR(
								DAQmxCfgDigEdgeStartTrig(
									pNIDAQmx->taskHandle,
									pNIDAQmx->triggerSource,
									DAQmx_Val_Rising
									)
						       );

						kLog (K_MON, "[NIDAQmx_DAQ] %s : waiting trigger %s\n",
								pSTDdev->taskName, pNIDAQmx->triggerSource);
			/*
					}
					else {
						kLog (K_MON, "[NIDAQmx_DAQ] %s : DAQ Start without trigger\n", pSTDdev->taskName);
					}
			*/
					NIERROR(
							DAQmxRegisterEveryNSamplesEvent(
								pNIDAQmx->taskHandle,		// taskHandle
								DAQmx_Val_Acquired_Into_Buffer,	// everyNsamplesEventType. Into_Buffer(input)/From_Buffer(output)
								numSampsPerChan,		// nSamples. number of samples after whitch each event should occur
								0,				// options. 0(callback function is called in a DAQmx thread)
								EveryNCallback,			// callbackFunction
								(void *)pSTDdev			// callbackData. parameter to be passed to callback function
								)
					       );
				} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
					if (0 == pNIDAQmx->smp_mode_select) {
						totSampsAllChan	= pNIDAQmx->totalWrite * chanNum;
						numSampsPerChan = pNIDAQmx->totalWrite;
						 nDataBufSize = (int)sizeof(float64) * totSampsAllChan; // numSampsPerChan * chanNum or totSampsAllChan // read data for float64 data
						databuf = NULL; /* for read ao data float64 */
						if (NULL == (databuf = (float64 *)malloc(nDataBufSize))) {
							kLog (K_ERR, "[NIDAQmx_DAQ] AO %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nDataBufSize);
							notify_error (1, "malloc*%d) failed", (int)nDataBufSize);
						}
						for (i=0;i<chanNum;i++) {
							if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))){
								pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
								kLog (K_INFO, "[NIDAQmx_DAQ] AO DATA in DAQmxCreateAOVoltageChan : ch(%d), physical_channel(%s) minVal(%.f) maxVal(%f)\n",
										i,  pNIDAQmxAOCh->physical_channel, pNIDAQmx->minVal, pNIDAQmx->maxVal);
								NIERROR (
										DAQmxCreateAOVoltageChan (
											pNIDAQmx->taskHandle,			// taskHandle
											pNIDAQmxAOCh->physical_channel,	        // physicalChannel
											"",					// nameToAssignToChannel
											pNIDAQmxAOCh->minVal,			// minVal
											pNIDAQmxAOCh->maxVal,			// maxVal
											pNIDAQmx->units,			// units DAQmx_Val_Volts or DAQmx_Val_FromCustomScale
											NULL					// customScaleName
											)
								);
								kLog (K_INFO, "[NIDAQmx_DAQ] DAQmxCreateAOVoltageChan compare file size( ): %s\n", pNIDAQmxAOCh->physical_channel);
//								kLog (K_INFO, "[NIDAQmx_DAQ] DAQmxCreateAOVoltageChan file fpoint (%p) size(%f):numSam(%d) %s\n", pNIDAQmxAOCh->write_fp, nDataBufSize/chanNum, numSampsPerChan, pNIDAQmxAOCh->physical_channel);

								if (NULL != pNIDAQmxAOCh->write_fp){
									fseek (pNIDAQmxAOCh->write_fp, 0L, SEEK_SET);

									if (1 != fread (&databuf[i*numSampsPerChan], nDataBufSize/chanNum, 1, pNIDAQmxAOCh->write_fp)) {
										kLog (K_ERR, "[NIDAQmx_DAQ] AO %s : fread failed (%s)\n", pNIDAQmxAOCh->file_name, strerror(errno));
										notify_error (1, "%s read failed\n", pNIDAQmxAOCh->file_name);
										fclose(pNIDAQmxAOCh->write_fp);
										pNIDAQmxAOCh->write_fp = NULL;
										continue;
									}
								}
							}
						}
					//	if (TRUE == isTriggerRequired (pSTDdev)) { // In this mode always need external clock and trigger
							NIERROR(
									DAQmxCfgSampClkTiming(
										pNIDAQmx->taskHandle,	// taskHandle
										pNIDAQmx->clockSource,	// source terminal of the Sample clock. NULL(internal)
										pNIDAQmx->sample_rate,	// sampling rate in samples per second per channel
										DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
										DAQmx_Val_FiniteSamps,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
										numSampsPerChan 	// TG Change from totSampsAllChan to numSampsPerChan FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
										)
							       );
							NIERROR(
									DAQmxCfgDigEdgeStartTrig(
										pNIDAQmx->taskHandle,
										pNIDAQmx->triggerSource,
										DAQmx_Val_Rising
										)
							       );
					/*	} else {
							NIERROR(
                                                                DAQmxCfgSampClkTiming(
                                                                        pNIDAQmx->taskHandle,   // taskHandle
                                                                        "",                     // source terminal of the Sample clock. NULL(internal)
                                                                        pNIDAQmx->sample_rate,  // sampling rate in samples per second per channel
                                                                        DAQmx_Val_Rising,       // activeEdge. edge of the clock to acquire or generate samples
                                                                        DAQmx_Val_FiniteSamps,  // sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint
                                                                        totSampsAllChan         // FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
                                                                        )
                                                       );
                                                	kLog (K_MON, "[NIDAQmx_DAQ AO] %s : DAQ Start AO without trigger\n", pSTDdev->taskName);	
						}
					*/
						NIERROR(
								DAQmxRegisterDoneEvent(
									pNIDAQmx->taskHandle,   // taskHandle
									0,			// options. 0(callback function is called in a DAQmx thread)
									DoneCallback,           // callbackFunction
									(void *)pSTDdev		// callbackData. parameter to be passed to callback function	
									)
						       );
						NIERROR(
								DAQmxWriteAnalogF64 (
									pNIDAQmx->taskHandle, 	// taskHandle
									numSampsPerChan, 	// numSampsPerChan. number of samples per channel to write 
									0,			// autoStart(bool32) automatically starts the task if you do not start
									600.0, 			// timeout (seconds)
									DAQmx_Val_GroupByChannel,// DAQmx_Val_GroupByScanNumberk (Group by scan number (interleaved), DAQmx_Val_GroupByChannel (non-interleaved)
									databuf,		// writeArray. array of 64-bit samples to write to the task
									(void *)&writtenSampleChan,  // actual number of samples per channel successfully writen to the buff
									NULL			  // reserved for future use
									)
							);

						kLog (K_MON, "[NIDAQmx_DAQ] %s :DAQ AO waiting trigger %s,and waiting clock %s, Num of Sample per Channel(%d), Written Samples Channel(%d). \n",
											pSTDdev->taskName, pNIDAQmx->triggerSource, pNIDAQmx->clockSource, numSampsPerChan, writtenSampleChan);
					} else {
//						pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) ellFirst(pNIDAQmx->pchannelConfig);
						for (i=0;i<chanNum;i++) {
							if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))){ 
								pNIDAQmxAOCh =(ST_NIDAQmx_AOchannel *)pSTDCh->pUser;
								kLog (K_INFO, "[NIDAQmx_DAQ] AO DATA in DAQmxCreateAOVoltageChan without triggger: ch(%d), physical_channel(%s) minVal(%.f) maxVal(%f)\n",
											i,  pNIDAQmxAOCh->physical_channel, pNIDAQmxAOCh->minVal, pNIDAQmxAOCh->maxVal);
								NIERROR (
									DAQmxCreateAOVoltageChan (
										pNIDAQmx->taskHandle,			// taskHandle
										pNIDAQmxAOCh->physical_channel,	        // physicalChannel
										"",					// nameToAssignToChannel
										pNIDAQmxAOCh->minVal,			// minVal
										pNIDAQmxAOCh->maxVal,			// maxVal
										pNIDAQmx->units,			// units DAQmx_Val_Volts or DAQmx_Val_FromCustomScale
										NULL					// customScaleName
										)
								);
								NIERROR(
									DAQmxCfgSampClkTiming(
										pNIDAQmx->taskHandle,	// taskHandle
										pNIDAQmx->clockSource,	// source terminal of the Sample clock. NULL(internal)
										pNIDAQmx->sample_rate,	// sampling rate in samples per second per channel
										DAQmx_Val_Rising,	// activeEdge. edge of the clock to acquire or generate samples
										DAQmx_Val_HWTimedSinglePoint,	// sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint - 
										1 	// TG Change from totSampsAllChan to numSampsPerChan FiniteSamps(sampsPerChanToAcquire), ContSamps(determine the buffer size)
										)
							    );
								NIERROR(
									DAQmxCfgDigEdgeStartTrig(
										pNIDAQmx->taskHandle,
										pNIDAQmx->triggerSource,
										DAQmx_Val_Rising
										)
							    );
//								pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) ellNext(&pNIDAQmxAOCh->node);
								kLog (K_INFO, "[NIDAQmx_DAQ] DAQmxCreateAOVoltageChan without triggger : %s\n", pNIDAQmxAOCh->physical_channel);
							}
						}
						// DAQmxCfgSampClkTiming, DAQmxCfgDigEdgeStartTrig, DAQmxRegisterDoneEvent, DAQmxWriteAnalogF64 functions do not requier at this time 
						kLog (K_MON, "[NIDAQmx_DAQ] %s : DAQ AO Start without trigger, AO Write command doing other process by PV \n", pSTDdev->taskName);
					}
					kLog (K_INFO, "[NIDAQmx_DAQ] %s : mode(%s) smp_mode(%d) sample_rate(%f) smp_time(%f) samples(%d) tot(%d)\n",
							pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),	pNIDAQmx->smp_mode, pNIDAQmx->sample_rate, pNIDAQmx->sample_time, numSampsPerChan, totSampsAllChan);
				}
				armingDeviceParams (pSTDdev);

				NIERROR(DAQmxStartTask(pNIDAQmx->taskHandle));

				kLog (K_INFO, "[threadFunc_NIDAQmx_DAQ] before epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				pNIDAQmx->protect_stop_flag = 1;
				epicsEventWait(pSTDdev->ST_DAQThread.threadEventId);
				pNIDAQmx->protect_stop_flag = 0;

				kLog (K_INFO, "[threadFunc_NIDAQmx_DAQ] after  epicsEventWait(pSTDdev->ST_DAQThread.threadEventId) \n");

				if (isCalcRunMode(pSTDdev)) {
					// data acquisition and storeing into the local files. and then transfer mdsplus
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;

					/*  DBproc_put(PV_CALC_RUN_STR, "0");  */
				} else {
					// data acquisiion without storing in local file and mdsplus.
					//pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					//pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

					DBproc_put(PV_TREND_RUN_STR, "0");
					//	pNIDAQmx->trend_run_flag = 0;
				}

				if(0 == strcmp(pNIDAQmx->sigType, "ao")){
					if (0 == pNIDAQmx->smp_mode_select) {
						free(databuf);
						databuf = NULL;
					}
				}
/* Move to stop function To avoid double Task stop clear 
				kLog (K_INFO, "[threadFunc_NIDAQmx_DAQ] before DAQmxClearTask for %s\n", pSTDdev->taskName);
				NIERROR(DAQmxClearTask(pNIDAQmx->taskHandle));
				kLog (K_INFO, "[threadFunc_NIDAQmx_DAQ] after  DAQmxClearTask for %s\n", pSTDdev->taskName);
				pNIDAQmx->taskHandle = 0;
*/
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
						if(0 == strcmp(pNIDAQmx->sigType, "ai")){
							if (proc_sendData2Tree(pSTDdev) == WR_OK) {
								pMaster->n8MdsPutFlag = 0;
								break;
							}
						} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
							if (proc_sendAOData2Tree(pSTDdev) == WR_OK) {
								pMaster->n8MdsPutFlag = 0;
								break;
							}
						}

						epicsThreadSleep(1);

						kLog (K_INFO, "[threadFunc_NIDAQmx_DAQ] %s : retry %d for proc_sendData2Tree()\n", pSTDdev->taskName, i);
					}
					stop = wf_getCurrentUsec();
					epicsPrintf("[threadFunc_NIDAQmx_DAQ - write to mdsplus DB Mode (%d) ] : measured function process time sec. is %lf msec\n",
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
				}
				else if(pMaster->n8MdsPutFlag == 1) {
					kLog (K_INFO, "[threadFunc_NIDAQmx_DAQ] %s : Waiting for MdsPlus Put Data\n", pSTDdev->taskName);
				}
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {

		}
	}
}

void threadFunc_NIDAQmx_RingBuf(void *param)
{
	kLog (K_TRACE, "[threadFunc_NIDAQmx_RingBuf] ...\n");
}

void threadFunc_NIDAQmx_CatchEnd(void *param)
{
	kLog (K_TRACE, "[threadFunc_NIDAQmx_CatchEnd] ...\n");
}

int create_NIDAQmx_taskConfig(ST_STD_device *pSTDdev)
{
	ST_NIDAQmx *pNIDAQmx = NULL;
	ST_NIDAQmx_AIchannel *pNIDAQmxAICh = NULL;
	ST_NIDAQmx_AOchannel *pNIDAQmxAOCh = NULL;
	ST_STD_channel *pSTDCh = NULL;
	int 	i;
	int 	j;
	int	chanNum;
	pNIDAQmx = (ST_NIDAQmx*) calloc(1, sizeof(ST_NIDAQmx));
	if(!pNIDAQmx) return WR_ERROR;
	kLog (K_TRACE, "[create_NIDAQmx_taskConfig- TGLee 1] ...\n");
	
	
	kLog (K_TRACE, "[create_NIDAQmx_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s) clock(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2, pSTDdev->strArg3);

	pNIDAQmx->taskHandle = 0x0;
	pNIDAQmx->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));

	if (NULL == pNIDAQmx->data_buf) {
		free (pNIDAQmx);
		pNIDAQmx = NULL;
		return WR_ERROR;
	}

	ellInit(pNIDAQmx->data_buf);
/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
        chanNum = ellCount(pSTDdev->pList_Channel);
	kLog (K_TRACE, "[create_NIDAQmx_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s) clock(%s) channelCounts(%d)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2, pSTDdev->strArg3, chanNum);


	sprintf (pNIDAQmx->triggerSource, "%s", pSTDdev->strArg1); 
	sprintf (pNIDAQmx->sigType, "%s", pSTDdev->strArg2); 
	sprintf (pNIDAQmx->clockSource, "%s", pSTDdev->strArg3); 

	pNIDAQmx->terminal_config		= DAQmx_Val_RSE;	// DAQmx_Val_Diff(6133,6259), DAQmx_Val_NRSE NI-6254&NI6220(DAQmx_Val_RSE)
	pNIDAQmx->sample_rate			= INIT_SAMPLE_RATE;
	pNIDAQmx->sample_rateLimit		= INIT_SAMPLE_RATE_LIMIT;
	pNIDAQmx->sample_time			= getSampTime(pNIDAQmx);
	pNIDAQmx->smp_mode			= DAQmx_Val_ContSamps;	// DAQmx_Val_FiniteSamps
	pNIDAQmx->smp_mode_select		= 0;	// 0 = DAQmx_Val_FiniteSamps, 1 = DAQmx_Val_ContSamps
	pNIDAQmx->units				= DAQmx_Val_Volts;

	pNIDAQmx->minVal			= INIT_LOW_LIMIT;
	pNIDAQmx->maxVal			= INIT_HIGH_LIMIT;

	for (i=0;i<chanNum;i++) {
		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			kLog (K_TRACE, "[create_NIDAQmx_taskConfig-STDCh_Check_TG] task(%s) STDchID(%d) inputTagName(%s) sigType(%s) strArg2(%s) channelCounts(%d)\n",
					pSTDdev->taskName, pSTDCh->channelId, pSTDCh->strArg1, pSTDCh->strArg0, pSTDCh->strArg2, chanNum);
			if(0 == strcmp(pNIDAQmx->sigType, "ai")){
				pNIDAQmxAICh = (ST_NIDAQmx_AIchannel*) malloc(sizeof(ST_NIDAQmx_AIchannel));
				if(!pNIDAQmxAICh) return WR_ERROR;
				kLog (K_TRACE, "[create_NIDAQmx_taskConfig- TGLee 2] ...\n");
				sprintf (pNIDAQmxAICh->file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
				sprintf (pNIDAQmxAICh->physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);
				pNIDAQmxAICh->used = CH_USED;
				pNIDAQmxAICh->channelId = i;

				strcpy (pNIDAQmxAICh->inputTagName, pSTDCh->strArg1);

				if (0 == strcmp (NBI_INPUT_TYPE_DT_STR, pSTDCh->strArg0)) {
					pNIDAQmxAICh->inputType	= NBI_INPUT_DELTA_T;
				}
				else if (0 == strcmp (NBI_INPUT_TYPE_TC_STR, pSTDCh->strArg0)) {
					pNIDAQmxAICh->inputType	= NBI_INPUT_TC;
				}
				else if (0 == strcmp (STRAIN_STR, pSTDCh->strArg0)) {
					/* I consider min/max value all channel of same value */
					sprintf (pNIDAQmxAICh->physical_channelSCXI, "%s", pSTDCh->strArg2);
					pNIDAQmxAICh->inputType = STRAIN_DATA;
					pNIDAQmx->minVal = -0.015;
					pNIDAQmx->maxVal = 0.015;
				}
				else if (0 == strcmp (DENSITY_CALC_STR, pSTDCh->strArg2)) {
					pNIDAQmxAICh->inputType = DENSITY_CALC;
					pNIDAQmxAICh->isFirst = 0;
					pNIDAQmxAICh->density = 0;
					pNIDAQmxAICh->fringe = 0.2078;
					pNIDAQmxAICh->dLimUpper = 0.16;
					pNIDAQmxAICh->dLimLow = 0.05;
				}
				else if (0 == strcmp (MDS_PARAM_PUT_STR, pSTDCh->strArg2)) {
					pNIDAQmxAICh->inputType = MDS_PARAM_PUT;
				}
				else {
					pNIDAQmxAICh->inputType	= OTHER_INPUT_NONE;
				}

				kLog (K_INFO, "[create_NIDAQmx_taskConfig_AI] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
						pSTDdev->strArg0, pNIDAQmxAICh->physical_channel,
						pNIDAQmxAICh->inputType, pNIDAQmxAICh->inputTagName,
						pNIDAQmxAICh->file_name);
				pSTDCh->pUser = pNIDAQmxAICh;
//				ellAdd(pNIDAQmx->pchannelConfig, &pNIDAQmxAICh->node);
			} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
				pNIDAQmxAOCh = (ST_NIDAQmx_AOchannel*) malloc(sizeof(ST_NIDAQmx_AOchannel));
				if(!pNIDAQmxAOCh) return WR_ERROR;
				kLog (K_TRACE, "[create_NIDAQmx_taskConfig- TGLee 3] ...\n");
				sprintf (pNIDAQmxAOCh->file_name, "B%d_AOCH%02d", pSTDdev->BoardID, i);
				sprintf (pNIDAQmxAOCh->physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);
				pNIDAQmxAOCh->used = CH_USED;
				pNIDAQmxAOCh->channelId = i;
				pNIDAQmxAOCh->minVal	= 0.0;
				pNIDAQmxAOCh->maxVal	= 10.0;
				pNIDAQmxAOCh->write_fp	= NULL;
			
				for(j=0; j<80; j++){
					pNIDAQmxAOCh->wavePatternTime[j] = 0.0;
					pNIDAQmxAOCh->wavePatternPower[j] = 0.0;
					pNIDAQmxAOCh->wavePatternValue[j] = 0.0;
				}	
				if (0 == strcmp (NIDAQ_AO_CH_STR, pSTDCh->strArg0)) {
					pNIDAQmxAOCh->inputType = NIDAQ_AO_CH;
				}
				else {
					pNIDAQmxAOCh->inputType	= OTHER_OUTPUT_NONE;
				}
				strcpy (pNIDAQmxAOCh->inputTagName, pSTDCh->strArg1);

				pSTDCh->pUser = pNIDAQmxAOCh;
				kLog (K_INFO, "[create_NIDAQmx_taskConfig_AO] dev(%s) ch(%s) inputType(%d) outputMode(%d) inputTagName(%s) fname(%s) point(%p)\n",
						pSTDdev->strArg0, pNIDAQmxAOCh->physical_channel,
						pNIDAQmxAOCh->inputType, pNIDAQmx->smp_mode_select,
						pNIDAQmxAOCh->inputTagName, pNIDAQmxAOCh->file_name, pNIDAQmxAOCh);
//				ellAdd(pNIDAQmx->pchannelConfig, &pNIDAQmxAOCh->node);
			} else if(0 == strcmp(pNIDAQmx->sigType, "port")){
//				sprintf (pNIDAQmxDIOCh->physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);
			} else {
    				//Addition ? for SCXI	sprintf (pNIDAQmx->ST_Ch[i].physical_channel, "%s", pSTDCh->strArg2);
				kLog (K_INFO, "[create_NIDAQmx_taskConfig_Error] dev(%s) ch(%s) inputType(%d) sigType(%s) tag(%s) fname(%s) Error\n",
					pSTDdev->strArg0, i,
					pNIDAQmxAICh->inputType, pSTDdev->strArg2, pNIDAQmxAICh->inputTagName,
					pNIDAQmxAICh->file_name);

				return WR_ERROR;
			}
		}
#if 0
		if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
			sprintf (pNIDAQmx->ST_Ch[i].file_name, "B%d_CH%02d", pSTDdev->BoardID, i);
			sprintf (pNIDAQmx->ST_Ch[i].physical_channel, "%s/%s%d", pSTDdev->strArg0, pSTDdev->strArg2, i);
	// TGL. I will do not used physical_channel info from createDevice argement. 
	// TGL. Direct physical_channel data put from createChannel argements. Because sometimes physical_channel is not regular like VVS, FUEL_DAQ
    //SCXI	sprintf (pNIDAQmx->ST_Ch[i].physical_channel, "%s", pSTDCh->strArg2);
			pNIDAQmx->ST_Ch[i].used = CH_USED;

			strcpy (pNIDAQmx->ST_Ch[i].inputTagName, pSTDCh->strArg1);

			if (0 == strcmp (NBI_INPUT_TYPE_DT_STR, pSTDCh->strArg0)) {
				pNIDAQmx->ST_Ch[i].inputType	= NBI_INPUT_DELTA_T;
			}
			else if (0 == strcmp (NBI_INPUT_TYPE_TC_STR, pSTDCh->strArg0)) {
				pNIDAQmx->ST_Ch[i].inputType	= NBI_INPUT_TC;
			}
			else if (0 == strcmp (STRAIN_STR, pSTDCh->strArg0)) {
				/* I consider min/max value all channel of same value */
				sprintf (pNIDAQmx->ST_Ch[i].physical_channelSCXI, "%s", pSTDCh->strArg2);
				pNIDAQmx->ST_Ch[i].inputType = STRAIN_DATA;
				pNIDAQmx->minVal = -0.015;
				pNIDAQmx->maxVal = 0.015;
					
			}
			else if (0 == strcmp (DENSITY_CALC_STR, pSTDCh->strArg2)) {
				pNIDAQmx->ST_Ch[i].inputType = DENSITY_CALC;
				pNIDAQmx->ST_Ch[i].isFirst = 0;
				pNIDAQmx->ST_Ch[i].density = 0;
				pNIDAQmx->ST_Ch[i].fringe = 0.2078;
				pNIDAQmx->ST_Ch[i].dLimUpper = 0.16;
				pNIDAQmx->ST_Ch[i].dLimLow = 0.05;
			}
			else if (0 == strcmp (MDS_PARAM_PUT_STR, pSTDCh->strArg2)) {
				pNIDAQmx->ST_Ch[i].inputType = MDS_PARAM_PUT;
			}
			else {
				pNIDAQmx->ST_Ch[i].inputType	= OTHER_INPUT_NONE;
			}

			kLog (K_INFO, "[create_NIDAQmx_taskConfig] dev(%s) ch(%s) inputType(%d) tag(%s) fname(%s)\n",
					pSTDdev->strArg0, pNIDAQmx->ST_Ch[i].physical_channel,
					pNIDAQmx->ST_Ch[i].inputType, pNIDAQmx->ST_Ch[i].inputTagName,
					pNIDAQmx->ST_Ch[i].file_name);
		}
#endif
	}

	pSTDdev->pUser				= pNIDAQmx;

	pSTDdev->ST_Func._OP_MODE		= func_NIDAQmx_OP_MODE;
	pSTDdev->ST_Func._CREATE_LOCAL_TREE	= func_NIDAQmx_CREATE_LOCAL_SHOT;
	pSTDdev->ST_Func._SYS_ARMING		= func_NIDAQmx_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN		= func_NIDAQmx_SYS_RUN;
	//pSTDdev->ST_Func._DATA_SEND		= func_NIDAQmx_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET		= func_NIDAQmx_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0		= func_NIDAQmx_T0;
	pSTDdev->ST_Func._SYS_T1		= func_NIDAQmx_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_NIDAQmx_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_NIDAQmx_SHOT_NUMBER;

	pSTDdev->ST_Func._POST_SEQSTOP		= func_NIDAQmx_POST_SEQSTOP; 
	pSTDdev->ST_Func._PRE_SEQSTART		= func_NIDAQmx_PRE_SEQSTART;
//	pSTDdev->ST_Func._FAULT_IP_MINIMUM	= func_XXXX_FAULLT_IP_MINIMUM;  //  Local mode : do not action, return value 1 or 0

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_NIDAQmx_DAQ;

	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		kLog (K_ERR, "%s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 0
	pSTDdev->ST_RingBufThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NIDAQmx_RingBuf;
	pSTDdev->maxMessageSize = sizeof(ST_User_Buf_node);

	if(make_STD_RingBuf_thread(pSTDdev)==WR_ERROR) {
		kLog (K_ERR, "%s : pST_BuffThread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: pST_BuffThread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}
#else
	pSTDdev->ST_RingBufThread.threadFunc = NULL;
#endif

	pSTDdev->ST_CatchEndThread.threadFunc = (EPICSTHREADFUNC)threadFunc_NIDAQmx_CatchEnd;
	if( make_STD_CatchEnd_thread(pSTDdev) == WR_ERROR ) { 
		return WR_ERROR;
	}

	return WR_OK;
}

static long drvNIDAQmx_init_driver(void)
{
	kLog (K_TRACE, "[drvNIDAQmx_init_driver] ...\n");

	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while (pSTDdev) {
		if (create_NIDAQmx_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvNIDAQmx_init_driver] %s : create_NIDAQmx_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		drvNIDAQmx_set_TagName(pSTDdev);

		pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "Target driver initialized.... OK!\n");

	return 0;
}

static long drvNIDAQmx_io_report(int level)
{
	kLog (K_TRACE, "[drvNIDAQmx_io_report] ...\n");
	
	int chanNum;
	ST_STD_device *pSTDdev = NULL;
	ST_MASTER *pMaster = get_master();
	ST_NIDAQmx	*pNIDAQmx = NULL;

	if (!pMaster) return -1;

	if (ellCount(pMaster->pList_DeviceTask)) {
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
		pNIDAQmx = pSTDdev->pUser;
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
		pNIDAQmx = pSTDdev->pUser;
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, (unsigned int)pSTDdev, pSTDdev->StatusDev );

		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3) {
			
			epicsPrintf("   status of Device Channel information (rpointer)\n");
			epicsPrintf("   ");
			epicsPrintf("\n");
#if 0 
			for(i=0;i<chanNum;i++) {
				if (NULL != (pSTDCh = get_STCh_from_STDev_chID (pSTDdev,i))) {
					if(0 == strcmp(pNIDAQmx->sigType, "ai")){
						pNIDAQmxAICh = pSTDCh->pUser;
					} else if(0 == strcmp(pNIDAQmx->sigType, "ao")){
						pNIDAQmxAOCh = pSTDCh->pUser;
					}
					epicsPrintf("\n");
					epicsPrintf("\n");
					epicsPrintf(" Ch No:, ChannelPhy:, Tag Name : , Store Select : \n")
						epicsPrintf("\n");

				}
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
