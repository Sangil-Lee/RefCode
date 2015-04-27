#include <fcntl.h>	// for mkdir

#include "drvIRTV.h"
#include "irtvCamInterface.h"

static long drvIRTV_io_report(int level);
static long drvIRTV_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvIRTV = {
	2,
	drvIRTV_io_report,
	drvIRTV_init_driver
};

epicsExportAddress(drvet, drvIRTV);

// sleep time of DAQ thread (milli seconds)
int kDAQSleepMillis	= 1000;
epicsExportAddress (int, kDAQSleepMillis);

// 이미지 취득 만료 대기지연 시간 : SYS_RUN과 CTU 기동 시의 Delay
int kCTUDelayMillis	= 10;
epicsExportAddress (int, kCTUDelayMillis);

// 이미지 취득 만료 대기지연 시간 : Camera에서의 Image 취득 및 전송 소요 시간을 고려한 Frame Rate의 배수
int kDelayRateCnt	= 1;
epicsExportAddress (int, kDelayRateCnt);

const char *getModeMsg (unsigned int mode)
{
	static char szModeStr[64];

	switch (mode) {
		case OPMODE_INIT : 
			strcpy (szModeStr, "INIT");
			break;
		case OPMODE_REMOTE : 
			strcpy (szModeStr, "REMOTE");
			break;
		case OPMODE_LOCAL : 
			strcpy (szModeStr, "LOCAL");
			break;
		case OPMODE_CALIBRATION : 
			strcpy (szModeStr, "CALIB");
			break;
		default :
			strcpy (szModeStr, "N/A");
			break;
	}

	return (szModeStr);
}

const char *getStatusMsg (unsigned int status, char *pStatusMsg)
{
	static char buf[256];

	char	*pMsg	= (NULL != pStatusMsg) ? pStatusMsg : buf;

	pMsg[0] = 0x00;

	if (status & TASK_STANDBY) 			{ strcat (pMsg, "READY "); }
	if (status & TASK_ARM_ENABLED) 		{ strcat (pMsg, "ARM "); }
	if (status & TASK_WAIT_FOR_TRIGGER) { strcat (pMsg, "WTRG "); }
	if (status & TASK_IN_PROGRESS) 		{ strcat (pMsg, "RUN "); }
	if (status & TASK_POST_PROCESS) 	{ strcat (pMsg, "PST "); }
	if (status & TASK_DATA_TRANSFER) 	{ strcat (pMsg, "TRANS "); }
	if (status & TASK_SYSTEM_IDLE)		{ strcat (pMsg, "IDLE "); }

	if (0x00 == pMsg[0]) {
		strcpy (pMsg, "N/A ");
	}

	return (pMsg);
}

void printStatusMsg ()
{
	char szStatusMsg[256];
	ST_STD_device *pSTDdev = NULL;

	if (NULL != get_master()) {
		getStatusMsg (get_master()->StatusAdmin, szStatusMsg);
		kLog (K_MON, "[Master] %s\n", szStatusMsg);
	}

	pSTDdev = get_first_STDdev();

	while (pSTDdev) {
		getStatusMsg (pSTDdev->StatusDev, szStatusMsg);
		kLog (K_MON, "[Device] name(%s) : %s\n", pSTDdev->taskName, szStatusMsg);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

int getSampRate (ST_IRTV *pIRTV) 
{ 
	return ((int)pIRTV->sample_rate); 
}

float64 getSampTime (ST_IRTV *pIRTV) 
{ 
	return (pIRTV->sample_time); 
}

void setSamplTime (ST_IRTV *pIRTV) 
{
#if 0
	pIRTV->sample_time = (pIRTV->stop_t1 <= pIRTV->start_t0) ? 0 : (pIRTV->stop_t1 - pIRTV->start_t0);
#else
	pIRTV->sample_time = pIRTV->ao_grab_time;
#endif

	kLog (K_INFO, "[setSamplTime] start_t0(%.f) stop_t1(%.f) grab_time(%f) sample_time(%f)\n",
			pIRTV->start_t0, pIRTV->stop_t1, pIRTV->ao_grab_time, pIRTV->sample_time);
}

int getTotSamps (ST_IRTV *pIRTV) 
{ 
	return ( (int) (pIRTV->sample_rate * pIRTV->sample_time) ); 
}

void setStartT0 (ST_IRTV *pIRTV, float64 val)
{
	pIRTV->start_t0 = val;
	setSamplTime (pIRTV);
}

void setStopT1 (ST_IRTV *pIRTV, float64 val)
{
	pIRTV->stop_t1 = val;
	setSamplTime (pIRTV);
}

double getSleepTimeForT0 (ST_STD_device *pSTDdev)
{
	ST_IRTV *pIRTV		= pSTDdev->pUser;
	double	nSleepTime	= 0;

	if (get_master()->ST_Base.opMode == OPMODE_REMOTE) {
		nSleepTime	+= pIRTV->start_t0;
		nSleepTime	+= pSTDdev->ST_Base_fetch.fBlipTime;
	}

	kLog (K_MON, "[getSleepTimeForT0] blip(%.2f) t0(%.2f) => sleep(%.2f) \n", 
		pSTDdev->ST_Base_fetch.fBlipTime, pIRTV->start_t0, nSleepTime);

	return (nSleepTime);
}

double getSleepTimeForT1 (ST_STD_device *pSTDdev)
{
	ST_IRTV *pIRTV		= pSTDdev->pUser;
	double	nSleepTime	= pIRTV->ao_grab_time;

	if (get_master()->ST_Base.opMode == OPMODE_REMOTE) {
		nSleepTime	+= kCTUDelayMillis / 1000.0;
	}

	if (0 < pIRTV->sample_rate) {
		nSleepTime	+= kDelayRateCnt / pIRTV->sample_rate;
	}

	kLog (K_MON, "[getSleepTimeForT1] grab(%.2f) rate(%.2f) CTU(%.3f) RateCnt(%d) => sleep(%.2f) \n", 
		pIRTV->ao_grab_time, pIRTV->sample_rate, kCTUDelayMillis/1000.0, kDelayRateCnt, nSleepTime);

	return (nSleepTime);
}

int isStoreRunMode ()
{
	return ( (get_master()->ST_Base.opMode == OPMODE_REMOTE) || (get_master()->ST_Base.opMode == OPMODE_LOCAL) );
}

int isRemoteOpMode ()
{
	return ( get_master()->ST_Base.opMode == OPMODE_REMOTE );
}

int isLocalOpMode ()
{
	return ( get_master()->ST_Base.opMode == OPMODE_LOCAL );
}

int isCalibOpMode ()
{
	return ( get_master()->ST_Base.opMode == OPMODE_CALIBRATION );
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

int isIpMinFaultEnable ()
{
	return (get_master()->cEnable_IPMIN);
}

int isIpMinFault ()
{
	if (TRUE == isIpMinFaultEnable()) {
		if (TRUE == isRemoteOpMode()) {
			if (TRUE == get_master()->cStatus_ip_min_fault) {
				return (TRUE);
			}
		}
	}

	return (FALSE);
}

void make_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[make_local_file] %s ...\n", pSTDdev->taskName);
}

void flush_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[flush_local_file] %s ...\n", pSTDdev->taskName);
}

void close_local_file(ST_STD_device *pSTDdev)
{
	kLog (K_TRACE, "[close_local_file] %s ...\n", pSTDdev->taskName);
}

ST_IRTV *getDriver (ST_STD_device *pSTDdev)
{
    return ((ST_IRTV *)(pSTDdev ? pSTDdev->pUser : NULL));
}
    
ST_IRTV *getFirstDriver ()
{
    return ( getDriver (get_first_STDdev()) );
}
    
int isFirstDevice (ST_STD_device *pSTDdev)
{
	return (0 == pSTDdev->BoardID);
}

int stopDevice (ST_STD_device *pSTDdev)
{
	ST_IRTV *pIRTV = pSTDdev->pUser;

	// 저장 시간 종료 설정
	pIRTV->bRunGrabFrame	= FALSE;

	// 파일 저장 완료 설정
	pIRTV->bStoreGrabFrame	= FALSE;

	epicsThreadSleep (1.0);

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

	kLog (K_MON, "[armingDevice] %s : isStoreRunMode(%d)\n", pSTDdev->taskName, isStoreRunMode());

	// clear error message
	sprintf (pMaster->ErrorString, "Clear");

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

	if (TRUE == isStoreRunMode()) {
		make_local_file (pSTDdev);

		if (TRUE == isRemoteOpMode()) {
			// Shot 번호 업데이트
			//retryCaputByInt (sfwEnvGet("PV_NAME_SHOT_NUMBER"), pSTDdev->ST_Base_fetch.shotNumber);
			retryCaputByInt (sfwEnvGet("PV_NAME_SHOT_NUMBER"), pSTDdev->ST_Base.shotNumber);
		}
	}
}

void armingDeviceParams (ST_STD_device* pSTDdev)
{
	ST_IRTV *pIRTV	= pSTDdev->pUser;

	pIRTV->totalRead		= getTotSamps (pIRTV);
	pIRTV->nGrabFrameCnt	= 0;
	pIRTV->bStoreGrabFrame	= FALSE;
	pIRTV->threadEventId	= pSTDdev->ST_DAQThread.threadEventId;
}

void copyStatusToPVs (ST_IRTV *pIRTV)
{
	char	sValue[64];

	sprintf (sValue, "%.f", pIRTV->ai_sts_frame_rate);
	DBproc_put (sfwEnvGet("PV_NAME_SAMPLING_RATE"), sValue);

	sprintf (sValue, "%d", (int)pIRTV->ai_sts_x);
	DBproc_put (sfwEnvGet("PV_NAME_ROI_X"), sValue);

	sprintf (sValue, "%d", (int)pIRTV->ai_sts_y);
	DBproc_put (sfwEnvGet("PV_NAME_ROI_Y"), sValue);

	sprintf (sValue, "%d", (int)pIRTV->ai_sts_width);
	DBproc_put (sfwEnvGet("PV_NAME_ROI_WIDTH"), sValue);

	sprintf (sValue, "%d", (int)pIRTV->ai_sts_height);
	DBproc_put (sfwEnvGet("PV_NAME_ROI_HEIGHT"), sValue);
}

void func_IRTV_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SYS_RESET] %s: setValue(%d)\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		resetDevice (pSTDdev);

		admin_all_taskStatus_reset();

		scanIoRequest(pMaster->ioScanPvt_status);

		DBproc_put (sfwEnvGet("PV_NAME_RESET"), "0");
	}
}

void func_IRTV_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_MASTER 		*pMaster = get_master();
	ST_STD_device	*pSTDdev = (ST_STD_device *)pArg;
	ST_IRTV			*pIRTV	 = pSTDdev->pUser;

	kLog (K_MON, "[SYS_ARMING] %s : %d \n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) { /* in case of arming  */
		if (admin_check_Arming_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_ARMING] %s : arming failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: arming failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		armingDevice (pSTDdev);

		armingDeviceParams (pSTDdev);

		kLog (K_MON, "[SYS_ARMING] %s : mode(%s) sample_rate(%f) sample_time(%f) total(%d)\n",
				pSTDdev->taskName,
				getModeMsg(pSTDdev->ST_Base.opMode),
				pIRTV->sample_rate, pIRTV->sample_time, pIRTV->totalRead);

#if 0
		// Arming 시점의 최종 Camera 상태 얻기
		irtvGetCameraStatus (pIRTV);
#endif

		// Frame을 취득할 쓰레드 생성
		if (createGrabFrameThr (pIRTV) == WR_ERROR) {
			kLog (K_ERR, "[create_IRTV_taskConfig] %s : Frame acquisition thread creation failed\n", pSTDdev->taskName);
			notify_error (1, "acquisition thread failed\n");
		}
	}
	else { /* release arming condition */
		if (admin_check_Release_condition() == WR_ERROR) {
			kLog (K_DEBUG, "[SYS_ARMING] %s : release failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			//notify_error (1, "%s: release failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		// 취소
		pIRTV->bRunGrabFrame	= FALSE;

		epicsEventSignal (pSTDdev->ST_DAQThread.threadEventId);

		kLog (K_MON, "[SYS_ARMING] %s arm disabled\n", pSTDdev->taskName);

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_IRTV_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_IRTV *pIRTV = pSTDdev->pUser;

	kLog (K_MON, "[SYS_RUN] %s : %d\n", pSTDdev->taskName, (int)arg1);

	if (1 == (int)arg1) {
		if (admin_check_Run_condition() == WR_ERROR) {
			kLog (K_ERR, "[SYS_RUN] %s : run failed. status(%s)\n",
					pSTDdev->taskName, getStatusMsg(pMaster->StatusAdmin, NULL));
			notify_error (1, "%s: run failed. status(%d)!\n", pSTDdev->taskName, pMaster->StatusAdmin);
			return;
		}

		pIRTV->shotNumber	= pMaster->ST_Base.shotNumber;

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_IRTV_OP_MODE(void *pArg, double arg1, double arg2)
{
	ST_MASTER *pMaster = get_master();
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[OP_MODE] %s : %f, %f\n", pSTDdev->taskName, arg1, arg2);

	if (pMaster->ST_Base.opMode != OPMODE_INIT) {
		pSTDdev->StatusDev |= TASK_STANDBY;

		resetDeviceAll ();
	}

	if (OPMODE_REMOTE == pMaster->ST_Base.opMode) {
#if 0
		DBproc_put (sfwEnvGet("PV_NAME_LTU_SET_FREE_RUN"), "0");
		DBproc_put (sfwEnvGet("PV_NAME_LTU_ENABLE"), "1");
		DBproc_put (sfwEnvGet("PV_NAME_LTU_MODE"), "0");
#endif
	}
	else if (OPMODE_LOCAL == pMaster->ST_Base.opMode) {
#if 0
		DBproc_put (sfwEnvGet("PV_NAME_LTU_SET_FREE_RUN"), "1");
		DBproc_put (sfwEnvGet("PV_NAME_LTU_ENABLE"), "1");
		DBproc_put (sfwEnvGet("PV_NAME_LTU_MODE"), "0");
#endif
	}
	else if (OPMODE_CALIBRATION == pMaster->ST_Base.opMode) {
		char	szValue[64];

		// 30Hz
		sprintf (szValue, "%d", CALIB_SAMPLE_RATE);
		DBproc_put (sfwEnvGet("PV_NAME_SAMPLING_RATE"), szValue);

		// Auto gain 설정
		DBproc_put (sfwEnvGet("PV_NAME_AGC_ENABLE"), "1");
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

void func_IRTV_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	kLog (K_MON, "[DATA_SEND] %s\n", pSTDdev->taskName);

	switch (pSTDdev->ST_Base.opMode) {
		case OPMODE_CALIBRATION :
			break;

		case OPMODE_REMOTE :
		case OPMODE_LOCAL :
			//TODO
			// MDSplus Tree에 저장
			break;

		default :
			notify_error (1, "%s: Wrong op mode (%d)!\n", pSTDdev->taskName, pSTDdev->ST_Base.opMode);
			break;
	}
}

void func_IRTV_CREATE_LOCAL_TREE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[CREATE_LOCAL_TREE] %s shot(%f)\n", pSTDdev->taskName, arg1);

	//TODO
	//createLocalMdsTree (pSTDdev);
}

void func_IRTV_SHOT_NUMBER(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	kLog (K_MON, "[SHOT_NUMBER] %s shot(%f)\n", pSTDdev->taskName, arg1);
}

void func_IRTV_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_IRTV *pIRTV = pSTDdev->pUser;

	if (TRUE == pIRTV->bRunGrabFrame) {
		kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f) : grabing ...\n", pSTDdev->taskName, arg1);
		return;
	}

	if (pIRTV->sample_rate == arg1) {
		kLog (K_DEBUG, "[SAMPLING_RATE] %s : sample_rate(%f) is same : skip ...\n", pSTDdev->taskName, arg1);
		return;
	}

	kLog (K_MON, "[SAMPLING_RATE] %s : sample_rate(%f)\n", pSTDdev->taskName, arg1);

	pIRTV->sample_rate = arg1;
	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	kLog (K_MON, "[SAMPLING_RATE] %s : before ... \n", pSTDdev->taskName);

	irtvSetFrameRate (arg1);

	kLog (K_MON, "[SAMPLING_RATE] %s : after  ... \n", pSTDdev->taskName);
}

void func_IRTV_T0(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_IRTV *pIRTV = NULL;

	kLog (K_MON, "[func_IRTV_T0] %s : start_t0(%.f) \n", pSTDdev->taskName, arg1);

	pIRTV = pSTDdev->pUser;

	setStartT0 (pIRTV, arg1);
}

void func_IRTV_T1(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_IRTV *pIRTV = NULL;

	kLog (K_MON, "[func_IRTV_T1] %s : stop_t1(%.f) \n", pSTDdev->taskName, arg1);

	pIRTV = pSTDdev->pUser;

	setStopT1 (pIRTV, arg1);
}

void threadFunc_IRTV_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_IRTV *pIRTV = pSTDdev->pUser;
	ST_MASTER *pMaster = get_master();
	char statusMsg[256];

	kLog (K_TRACE, "[IRTV_DAQ] dev(%s)\n", pSTDdev->taskName);

	if( !pSTDdev ) {
		kLog (K_ERR, "[threadFunc_IRTV_DAQ] %s : STD device is null\n", pSTDdev->taskName);
		notify_error (1, "%s: STD device is null!\n", pSTDdev->taskName);
		return;
	}

	epicsThreadSleep (10.0);

	pSTDdev->ST_Base.nSamplingRate = (int)pIRTV->sample_rate;
	
	while (TRUE) {
		epicsThreadSleep (kDAQSleepMillis / 1000.);

		if (NULL == pMaster) {
			kLog (K_ERR, "{IRTV_DAQ] ST_MASTER is null !!!\n");
		}

		kLog (K_DEBUG, "[IRTV_DAQ] mode(%s) rate(%.f) t0(%.f) t1(%.f) grap(%.f) time(%f) shot(%d) sync(%d) status(%s)\n",
				getModeMsg(pSTDdev->ST_Base.opMode),
				pIRTV->sample_rate, 
				pIRTV->start_t0, pIRTV->stop_t1, pIRTV->ao_grab_time, pIRTV->sample_time,
				pMaster->ST_Base.shotNumber, (int)pIRTV->ao_sync_source,
				getStatusMsg(pSTDdev->StatusDev, statusMsg));

		if (pSTDdev->StatusDev != get_master()->StatusAdmin) {
			kLog (K_MON, "[IRTV_DAQ] %s : status(%s) != master(%s)\n",
					pSTDdev->taskName,
					getStatusMsg(pSTDdev->StatusDev, statusMsg),
					getStatusMsg(get_master()->StatusAdmin,NULL));
		}

		if (pSTDdev->StatusDev & TASK_STANDBY) {
			if (pSTDdev->StatusDev & TASK_SYSTEM_IDLE) {
				// Camera 상태 얻기 : 취득한 Frame의 헤더 정보 분석
				//getCameraStatus (pIRTV);
				//irtvGetCameraStatus (pIRTV);
			}
			else if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {

			}	
			else if (pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER) {
				//pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
				//pSTDdev->StatusDev |= TASK_IN_PROGRESS;

				//notify_refresh_master_status();

				// 파일 저장 시작 플래그 설정
				pIRTV->bRunGrabFrame	= TRUE;
				pIRTV->bStoreGrabFrame	= isStoreRunMode ();
				pIRTV->nGrabFrameCnt	= 0;

				kLog (K_MON, "[threadFunc_IRTV_DAQ] send signal ... : request to grab image \n");

				epicsEventSignal (pSTDdev->ST_DAQThread.threadEventId);

				while (pIRTV->bRunGrabFrame) {
					kLog (K_DEBUG, "[threadFunc_IRTV_DAQ] waiting for grab finish \n");

					epicsThreadSleep (0.5);

					if ( (0 < pIRTV->nGrabFrameCnt) && (TASK_WAIT_FOR_TRIGGER & pSTDdev->StatusDev) ) {
						pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
						pSTDdev->StatusDev |= TASK_IN_PROGRESS;

						notify_refresh_master_status();
					}
				}

				kLog (K_MON, "[threadFunc_IRTV_DAQ] grab is stopped !!!\n");

				// 파일 저장 완료 설정
				pIRTV->bStoreGrabFrame	= FALSE;

				if (TRUE == isStoreRunMode ()) {
					pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
					pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
					pSTDdev->StatusDev |= TASK_POST_PROCESS;
				}
				else {
					DBproc_put (sfwEnvGet("PV_NAME_DAQ_STOP"), "1");
				}

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_POST_PROCESS)) {
				flush_local_file(pSTDdev);

				if (TRUE == isStoreRunMode ()) {
					// 이미지 파일 생성
					generateImageFiles (pIRTV);
				}

				pSTDdev->StatusDev = TASK_STANDBY | TASK_DATA_TRANSFER;

				notify_refresh_master_status();
			}
			else if((pSTDdev->StatusDev & TASK_DATA_TRANSFER)) {
#if 0
				//TODO : MDSplus Tree에 저장 또는, NAS(?)에 저장
#endif
				close_local_file (pSTDdev);

				pSTDdev->StatusDev = TASK_STANDBY | TASK_SYSTEM_IDLE;

				notify_refresh_master_status();

				// LOCAL 모드일 때, Shot # 자동 증가
				if (TRUE == isLocalOpMode ()) {
					char	szLocalShotNum[20];
					sprintf (szLocalShotNum, "%d", pMaster->ST_Base.shotNumber + 1);

					DBproc_put (sfwEnvGet("PV_NAME_SHOT_NUMBER"), szLocalShotNum);

					kLog (K_MON, "[threadFunc_IRTV_DAQ] shot number : %d -> %s\n", pMaster->ST_Base.shotNumber, szLocalShotNum);
				}

				DBproc_put (sfwEnvGet("PV_NAME_SYS_RUN"), "0");
				DBproc_put (sfwEnvGet("PV_NAME_SYS_ARMING"), "0");
			}
		}
		else if(!(pSTDdev->StatusDev & TASK_STANDBY)) {
			pSTDdev->StatusDev |= TASK_STANDBY;
			notify_refresh_master_status();
		}
	}
}

int create_IRTV_taskConfig(ST_STD_device *pSTDdev)
{
	ST_IRTV *pIRTV = NULL;
	ST_STD_channel *pSTDCh = NULL;

	pIRTV = (ST_IRTV*) calloc(1, sizeof(ST_IRTV));
	if(!pIRTV) return WR_ERROR;
	
	kLog (K_TRACE, "[create_IRTV_taskConfig] task(%s) dev(%s) trig(%s) sigType(%s)\n",
			pSTDdev->taskName, pSTDdev->strArg0, pSTDdev->strArg1, pSTDdev->strArg2);

	pIRTV->sample_rate			= INIT_SAMPLE_RATE;
	pIRTV->sample_time			= getSampTime(pIRTV);
	pIRTV->bStoreGrabFrame		= FALSE;
	pIRTV->ioScanPvt_userCall	= pSTDdev->ioScanPvt_userCall;		// to call in IrtvGrabFrame thread
	pIRTV->threadEventId		= pSTDdev->ST_DAQThread.threadEventId;
	pIRTV->grabVideoLock		= epicsMutexMustCreate();

	DBproc_get (sfwEnvGet("PV_NAME_IMAGE_PATH"), pIRTV->so_image_path);

	pSTDdev->pUser	= pIRTV;

	pSTDdev->ST_Func._OP_MODE			= func_IRTV_OP_MODE;
	pSTDdev->ST_Func._CREATE_LOCAL_TREE	= func_IRTV_CREATE_LOCAL_TREE;
	pSTDdev->ST_Func._SYS_ARMING		= func_IRTV_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN			= func_IRTV_SYS_RUN;
	pSTDdev->ST_Func._DATA_SEND			= func_IRTV_DATA_SEND;
	pSTDdev->ST_Func._SYS_RESET			= func_IRTV_SYS_RESET;

	pSTDdev->ST_Func._SYS_T0			= func_IRTV_T0;
	pSTDdev->ST_Func._SYS_T1			= func_IRTV_T1;
	pSTDdev->ST_Func._SAMPLING_RATE		= func_IRTV_SAMPLING_RATE;
	pSTDdev->ST_Func._SHOT_NUMBER		= func_IRTV_SHOT_NUMBER;

	pSTDdev->ST_Func._POST_SEQSTOP		= NULL;
	pSTDdev->ST_Func._PRE_SEQSTART		= NULL;

	pSTDdev->ST_Func._TREE_CREATE		= NULL;
	pSTDdev->ST_Func._PRE_SHOTSTART		= NULL;
	pSTDdev->ST_Func._PRE_SEQSTOP		= NULL;
	pSTDdev->ST_Func._BOARD_SETUP		= NULL;
	pSTDdev->ST_Func._TEST_PUT			= NULL;
	pSTDdev->ST_Func._Exit_Call			= NULL;

	pSTDdev->ST_DAQThread.threadFunc	= (EPICSTHREADFUNC)threadFunc_IRTV_DAQ;

	if (make_STD_DAQ_thread (pSTDdev) == WR_ERROR) {
		kLog (K_ERR, "[create_IRTV_taskConfig] %s : DAQ thread creation failed\n", pSTDdev->taskName);
		notify_error (1, "%s: DAQ thread failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	pSTDdev->ST_RingBufThread.threadFunc	= NULL;
	pSTDdev->ST_CatchEndThread.threadFunc	= NULL;

	// 공유메모리 생성
	createGrabFrameShm ();

#if 0
	// Live Data를 취득할 쓰레드 생성
	if (createLiveDataThr (pIRTV) == WR_ERROR) {
		kLog (K_ERR, "[create_IRTV_taskConfig] %s : Live data thread creation failed \n", pSTDdev->taskName);
		notify_error (1, "%s: Live data thread failed \n", pSTDdev->taskName);
		return WR_ERROR;
	}
#endif
	
	// Client의 Heartbeat을 처리할 쓰레드 생성
	if (createListenThr (pIRTV) == WR_ERROR) {
		kLog (K_ERR, "[create_IRTV_taskConfig] %s : Listen thread creation failed \n", pSTDdev->taskName);
		notify_error (1, "%s: Listen thread failed \n", pSTDdev->taskName);
		return WR_ERROR;
	}

	// Frame Data를 전송할 쓰레드 생성
	if (createDataSendThr (pIRTV) == WR_ERROR) {
		kLog (K_ERR, "[create_IRTV_taskConfig] %s : Data Send thread creation failed \n", pSTDdev->taskName);
		notify_error (1, "%s: Data Send thread failed \n", pSTDdev->taskName);
		return WR_ERROR;
	}

#if 1	//TODO
	// Camera 상태 정보를 취득할 쓰레드 생성
	if (createStatusThr (pIRTV) == WR_ERROR) {
		kLog (K_ERR, "[create_IRTV_taskConfig] %s : Status thread creation failed \n", pSTDdev->taskName);
		notify_error (1, "%s: Status thread failed \n", pSTDdev->taskName);
		return WR_ERROR;
	}
#endif

	return WR_OK;
}

static long drvIRTV_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	kLog (K_TRACE, "[drvIRTV_init_driver] ...\n");

	pMaster = get_master();
	if(!pMaster)	return -1;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);

	while (pSTDdev) {
		if (create_IRTV_taskConfig (pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[drvIRTV_init_driver] %s : create_IRTV_taskConfig() failed.\n", pSTDdev->taskName);
			notify_error (1, "%s creation failed!\n", pSTDdev->taskName);
			return -1;
		}

		set_STDdev_status_reset(pSTDdev);

		//pSTDdev->StatusDev |= TASK_STANDBY;

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} 

	kLog (K_DEBUG, "[drvIRTV_init_driver] Target driver initialized.... OK!\n");

	return 0;
}

static long drvIRTV_io_report(int level)
{
	kLog (K_TRACE, "[drvIRTV_io_report] ...\n");

	return 0;
}
