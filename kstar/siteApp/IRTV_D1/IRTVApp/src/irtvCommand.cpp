#include <stdio.h>

#include "sfwCommon.h"
#include "sfwGlobalDef.h"

#include "IrtvCommand.h"
#include "IrtvCamInterface.h"
#include "IrtvCamFile.h"
#include "irtvBHP.h"

#pragma comment(lib, "tc.lib")
#pragma comment(lib, "tc.ui.lib")
#pragma comment(lib, "tc.cam.lib")
#pragma comment(lib, "tc.file.lib")
#pragma comment(lib, "tc.reduce.lib")
#pragma comment(lib, "bhpSDK.lib")
#pragma comment(lib, "ResourceTree.lib")

#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")		// for ConvertBSTRToString()
#else
#pragma comment(lib, "comsuppw.lib")
#endif

IrtvSync			gIrtvCommandSync;
IrtvCameraStatus	gIrtvCamStatus;


static IrtvCommand	gIrtvCommand;

CCamera				IrtvCommand::camera;
bhpSCalInfo			*IrtvCommand::m_pCalib = NULL;

IrtvCommand & IrtvCommand::getInstance()
{
	return (gIrtvCommand);
}

extern "C" int irtvSetGainLevel (const int nGainLevel)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setGainLevel (nGainLevel);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetInvertEnable (const int bEnable)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setInvertEnable (bEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetRevertEnable (const int bEnable)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setRevertEnable (bEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetSyncPolarity (const int bEnable)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setSyncPolarity (bEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetTriggerMode (const int nTriggerMode)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setTriggerMode (nTriggerMode);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetCoolerOn (const int bCoolerOn)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setCoolerOn (bCoolerOn);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetCoolerType (const int nCoolerType)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setCoolerType (nCoolerType);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetCoolerAuto (const int nCoolerAuto)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setCoolerAuto (nCoolerAuto);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFPAAutoOffTemp (const int nCoolerOffTemp)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setFPAAutoTurnOffTemp (nCoolerOffTemp);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFPAAutoOnTemp (const int nCoolerOnTemp)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setFPAAutoTurnOnTemp (nCoolerOnTemp);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetConfigPreset (const int nConfigPreset)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setConfigPreset (nConfigPreset);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetActivePreset (const int nActivePreset)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setActivePreset (nActivePreset);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetPresetSeqMode (const int bPresetSeqMode)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setPresetSeqMode (bPresetSeqMode);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFullNucEnable (const int bFullNucEnable)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setFullNucEnable (bFullNucEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvLoadNucAction (const char *pNucName, const int nConfigPreset, const int nActivePreset)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().loadNucAction (pNucName, nConfigPreset, nActivePreset);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFPAIntTime (const int nPreset, const double fFPAIntTime)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setFPAIntTime (nPreset, fFPAIntTime);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvGetStoredNUCs ()
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().getStoredNUCs ();

	gIrtvCommandSync.unlock ();

	scanIoRequest (getFirstDriver()->ioScanPvt_userCall);

	return (ret);
}

extern "C" int irtvSetFrameRate (const double fFrameRate)
{
	gIrtvCommandSync.lock ();

	int	ret	= IrtvCommand::getInstance().setFrameRate (fFrameRate);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetROI (const short nOffsetX, const short nOffsetY, 
						   const short nWidth, const short nHeight, 
						   const short nLifeTime, const short nColor, const short nCoordi)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setROI (nOffsetX, nOffsetY, nWidth, nHeight, nLifeTime, (UInt8)nColor, (Int32)nCoordi);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetSyncSource (const int nSyncSource)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setSyncSource (nSyncSource);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSendSoftTrigger ()
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().sendSoftTrigger ();

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSendTriggerRearm ()
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().sendRearmTrigger ();

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetAGCEnable (const int nAGCEnable)
{
	gIrtvCommandSync.lock ();

	int ret = IrtvCommand::getInstance().setAGCEnable (nAGCEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" void * irtvGetCalibration ()
{
	gIrtvCommandSync.lock ();

	bhpSCalInfo	*pCalInfo	= NULL;

#if 1
	//TODO: 20121017 : IACF : User calibration을 사용할 경우, Init시 오류가 발생함. Factory Calibration을 사용할 경우에만 가져오도록 할 예정?
	pCalInfo = IrtvCommand::getInstance().getCalibration ();
#elif 0	//TODO WORK
	pCalInfo = IrtvCommand::getInstance().getCalibrationFromFile ();
#endif

	gIrtvCommandSync.unlock ();

	return (pCalInfo);
}

extern "C" void irtvFreeCalibration ()
{
	IrtvCommand::getInstance().releaseCalibration ();
}

extern "C" void irtvSetCameraDisconnect (ST_IRTV *pIRTV)
{
	pIRTV->bi_sts_conn	= 0;	// 연결 비정상
	pIRTV->bInitSetting	= 0;	// 설정 정보 업데이트 필요
	pIRTV->bInitNUCs	= 0;	// NUC 정보 업데이트 필요

	scanIoRequest (pIRTV->ioScanPvt_userCall);
}

// 주의 : 이미지 취득 중에는 Command를 사용 하지 않아야 함
extern "C" int irtvGetCameraStatus (ST_IRTV *pIRTV)
{
	if (NULL == pIRTV) {
		return (NOK);
	}

	gIrtvCommandSync.lock ();

	int	ret = IrtvCommand::getInstance().getCameraStatus (gIrtvCamStatus);

	gIrtvCommandSync.unlock ();

	if (OK != ret) {
		irtvSetCameraDisconnect (pIRTV);

		kLog (K_ERR, "[irtvGetCameraStatus] getCameraStatus() failed \n");
	}
	else {
		pIRTV->bi_sts_conn	= 1;	// 연결 정상
		pIRTV->ai_max_rate	= getMaxFrameRate ();

		// Camera에 대한 정상 연결 시, 카메라 설정값을 얻어 내부 변수 및 PV 값 설정
		if ( (0 == pIRTV->bInitSetting) || (0 == pIRTV->bInitNUCs) ) {
			gIrtvCommandSync.lock ();

			if (OK == IrtvCommand::getInstance().getCameraSetting (gIrtvCamStatus)) {
				pIRTV->bInitSetting	= 1;	// 설정 정보 업데이트 완료
			}

			if (OK == IrtvCommand::getInstance().getStoredNUCs ()) {
				pIRTV->bInitNUCs	= 1;	// NUC 정보 업데이트 완료
			}

			gIrtvCommandSync.unlock ();

			IrtvCommand::getInstance().writePVs (gIrtvCamStatus);
		}
	}

	scanIoRequest (pIRTV->ioScanPvt_userCall);

	return (ret);
}

IrtvCommand::IrtvCommand ()
{
	m_resHandle	= NULL;
	m_pCalib	= NULL;
}

IrtvCommand::~IrtvCommand ()
{
	clear ();
}

int IrtvCommand::init ()
{
	kLog (K_DEBUG, "[IrtvCommand.init] ...\n");

	m_resHandle = camera.GetResourceTree();

	bhpConfigInit (m_resHandle);

	return (OK);
}

void IrtvCommand::clear ()
{
	bhpConfigFini(m_resHandle);
}

CCamera & IrtvCommand::getCamera ()
{
	return (camera);
}

// Select a camera
bool IrtvCommand::selectCamera (SCameraInfo &info)
{
	CEvent					cancel;	//	not used since we're not multithreaded.
	TArray<SCameraInfo>		cameras;
	UInt32					i;

	kLog (K_MON, "[IrtvCommand::selectCamera] finding ...\n");

	// 약 1.8초 소요
	if (!Cameras.Find (cameras, cancel, CCameras::findCommAndVideo) || cameras.GetSize() == 0) {
		kLog (K_ERR, "[IrtvCommand::selectCamera] No cameras found.\n");
		return false;
	}

	for (i = 0; i < cameras.GetSize(); ++i) {
		kLog (K_MON, "[IrtvCommand::selectCamera] %d) %S\n", i, cameras[i].Identity.GetTChar());
	}

	info = cameras[0];

	return true;
}

bool IrtvCommand::isConnected ()
{
	return (camera.IsConnected());
}

bool IrtvCommand::connectCamera ()
{
	if (true == camera.IsConnected()) {
		kLog (K_DEBUG, "[IrtvCommand::connectCamera] camera.IsConnected() : already connected ...\n");
		return (true);
	}

	// Camera 정보 얻기
	SCameraInfo	camInfo;

	// Find : 약 1.8 ~ 2초 소요됨
	if (true != selectCamera (camInfo)) {
		return (false);
	}

	// set camera info and connect
	camera.CameraInfo = camInfo;

	kLog (K_MON, "[IrtvCommand::connectCamera] connecting ...\n");

	// video와 autoStartVideo 모두 true이어야 이미지 취득 가능함
	// bool Connect(bool video = true, bool autoStartVideo = true);
	// Connect : 약 11초 소요됨
	if (true != camera.Connect (true, true)) {
		kLog (K_ERR, "[IrtvCommand::connectCamera] Can't connect to camera.\n");
		return (false);
	}

	kLog (K_MON, "[IrtvCommand::connectCamera] connected ...\n");

	//	get and print some basic camera info
	if (camera.ReduceObjects.GetSourceInfo() == NULL) {
		camera.Disconnect();
		printf("Source info not valid (very bad thing).\n");
		return (false);
	}

	kLog (K_MON, "[IrtvCommand::connectCamera] Model(%S) Serial(%S)\n",
		camera.ReduceObjects.GetSourceInfo()->camModel.GetTChar(),
		camera.ReduceObjects.GetSourceInfo()->camSerial.GetTChar());

	// set timeout for grab frame (msec) : 기본 값은 3초
	camera.Timeout = IRTV_GRAP_TIMEOUT_MSEC;

	init();

#if 1	// 픽셀 정보 확인
	UInt32	pixelType;
	UInt32	pixelWidth;
	UInt16	xOff, yOff, width, height;

	camera.Controller->GetPixelType (pixelType);
	pixelWidth = PixelWidth (pixelType);

	camera.Controller->GetWindow (xOff, yOff, width, height);

	kLog (K_MON, "[IrtvCommand::connectCamera] pixelType(%d) pixelWidth(%d) window(%d/%d/%d/%d)\n",
		pixelType, pixelWidth, xOff, yOff, width, height);
#endif

	return (true);
}

void IrtvCommand::disConnectCamera ()
{
	if (true == camera.IsConnected()) {
		camera.Disconnect ();
		
		kLog (K_MON, "[IrtvCommand::disConnectCamera] camera.Disconnect() : disconnected ...\n");
	}
}

int IrtvCommand::getActivePreset ()
{
	return ((int)getFirstDriver()->ai_active_preset_num);
}

int IrtvCommand::getCameraStatus (IrtvCameraStatus &camStatus)
{
	kLog (K_DEBUG, "[IrtvCommand::getCameraStatus] get cameara status \n");

#if 0	// 클래스 내의 메소드가 있는 경우, 다음과 같이 사용도 가능함
	if (true != camera.Controller->GetActivePreset (camStatus.m_activePreset)) { return (NOK);}
	if (true != camera.Controller->GetFrameRate (camStatus.m_activePreset, camStatus.m_frameRate)) { return (NOK);}
#endif

#if 0
	if (errOk != bhpGetFrameSyncSource (m_resHandle, &camStatus.m_syncSource)) { return (NOK); }
	if (errOk != bhpGetFrameSyncPolarity (m_resHandle, &camStatus.m_syncPolarity)) { return (NOK); }
	if (errOk != bhpGetFrameSyncMode (m_resHandle, &camStatus.m_triggerMode)) { return (NOK); }
#else
	if (errOk != bhpGetTriggerSource (m_resHandle, &camStatus.m_syncSource)) { return (NOK); }
	if (errOk != bhpGetTriggerPolarity (m_resHandle, &camStatus.m_syncPolarity)) { return (NOK); }
	if (errOk != bhpGetTriggerMode (m_resHandle, &camStatus.m_triggerMode)) { return (NOK); }
#endif

	if (errOk != bhpGetFrameSyncMode (m_resHandle, &camStatus.m_intMode)) { return (NOK); }
	if (errOk != bhpGetFPATemperature (m_resHandle, &camStatus.m_fpaTemp)) { return (NOK);}
	if (errOk != bhpGetActivePreset (m_resHandle, &camStatus.m_activePreset)) { return (NOK); }
	if (errOk != bhpGetActivePreset (m_resHandle, &camStatus.m_configPreset)) { return (NOK); }

	if (errOk != bhpGetIntegrationTime (m_resHandle, camStatus.m_activePreset, &camStatus.m_intTime)) { return (NOK); }
	if (errOk != bhpGetGainLevel (m_resHandle, &camStatus.m_gain)) { return (NOK); }
	if (errOk != bhpGetFrameRate (m_resHandle, camStatus.m_activePreset, &camStatus.m_frameRate)) { return (NOK); }

	if (errOk != bhpGetWindow (m_resHandle, camStatus.m_activePreset, 
					&camStatus.m_xOffset, &camStatus.m_yOffset, &camStatus.m_width, &camStatus.m_height)) {
		return (NOK);
	}
	if (errOk != bhpGetInvertEnabled (m_resHandle, &camStatus.m_rowReadoutOrder)) { return (NOK); }
	if (errOk != bhpGetRevertEnabled (m_resHandle, &camStatus.m_colReadoutOrder)) { return (NOK); }
	if (errOk != bhpGetFlagCoolerEnabled (m_resHandle, &camStatus.m_coolerOn)) { return (NOK); }

	/*
	if (errOk != iacfGetCLRFpaAutoTurnOffTempLimit (m_nCtrlDeviceId, &camStatus.m_autoTurnOffTemp)) { return (NOK); }
	if (errOk != iacfGetCLRFpaAutoTurnOnTempLimit (m_nCtrlDeviceId, &camStatus.m_autoTurnOnTemp)) { return (NOK); }
	if (errOk != iacfGetCFGFullWindowNuc (m_nCtrlDeviceId, &camStatus.m_fullNucEnable)) { return (NOK); }
	if (errOk != iacfGetCFGPscSeqMode (m_nCtrlDeviceId, &camStatus.m_presetSeqMode)) { return (NOK); }
	*/

	if (errOk != bhpGetPresetSequencingMode (m_resHandle, &camStatus.m_presetSeqMode)) { return (NOK); }

	if (errOk != bhpGetSCTemperatures (m_resHandle, 
					&camStatus.m_frontPannelTemp, &camStatus.m_airGapTemp, &camStatus.m_internalTemp)) { return (NOK); }

	copyConfigValues (camStatus);

	camStatus.printStatus ();

	kLog (K_DEBUG, "[IrtvCommand::getCameraStatus] end ... \n");

	return (OK);
}

int IrtvCommand::copyConfigValues (IrtvCameraStatus &camStatus)
{
	kLog (K_INFO, "[IrtvCommand::copyConfigValues] set config values \n");

	ST_IRTV	*pIRTV	= getFirstDriver();

	if (pIRTV) {
		pIRTV->ai_sts_fpa_temp			= gIrtvCamStatus.m_fpaTemp;
		pIRTV->ai_sts_front_temp		= gIrtvCamStatus.m_frontPannelTemp;
		pIRTV->ai_sts_airgap_temp		= gIrtvCamStatus.m_airGapTemp;
		pIRTV->ai_sts_internal_temp		= gIrtvCamStatus.m_internalTemp;
		pIRTV->ai_sts_preset			= gIrtvCamStatus.m_activePreset;
		pIRTV->ai_sts_int_time			= gIrtvCamStatus.m_intTime;
		pIRTV->ai_sts_x					= gIrtvCamStatus.m_xOffset;
		pIRTV->ai_sts_y					= gIrtvCamStatus.m_yOffset;
		pIRTV->ai_sts_width				= gIrtvCamStatus.m_width;
		pIRTV->ai_sts_height			= gIrtvCamStatus.m_height;
		pIRTV->ai_sts_frame_rate		= gIrtvCamStatus.m_frameRate;
		pIRTV->mbbi_sts_sync_src		= gIrtvCamStatus.m_syncSource;
		pIRTV->mbbi_sts_sync_polarity	= gIrtvCamStatus.m_syncPolarity;	// 0 :Falling, 1 : Rising

		pIRTV->ai_active_preset_num		= gIrtvCamStatus.m_activePreset;
		pIRTV->ai_config_preset_num		= gIrtvCamStatus.m_configPreset;
		pIRTV->bi_preset_cycling		= gIrtvCamStatus.m_presetCycling;

		pIRTV->sample_rate				= pIRTV->ai_sts_frame_rate;

		for (int i = 0; i < IRTV_PRESET_NUM; i++) {
			pIRTV->ai_preset_rate[i]		= gIrtvCamStatus.m_presetFrameRate[i];
			pIRTV->ai_preset_gain[i]		= gIrtvCamStatus.m_presetGain[i];
			//pIRTV->ao_preset_int_time[i]	= gIrtvCamStatus.m_presetIntTime[i];
			pIRTV->ai_preset_win_x[i]		= gIrtvCamStatus.m_presetXOffset[i];
			pIRTV->ai_preset_win_y[i]		= gIrtvCamStatus.m_presetYOffset[i];
			pIRTV->ai_preset_win_width[i]	= gIrtvCamStatus.m_presetWidth[i];
			pIRTV->ai_preset_win_height[i]	= gIrtvCamStatus.m_presetHeight[i];
			strcpy (pIRTV->si_preset_nuc_name[i], gIrtvCamStatus.m_presetNucName[i]);
		}

		//if (!pIRTV->bInitSetting) {
		if (1) {
#if 0	//TODO : Output으로 PV 값을 직접 기록함
			pIRTV->bo_invert_enable			= gIrtvCamStatus.m_rowReadoutOrder;
			pIRTV->bo_revert_enable			= gIrtvCamStatus.m_colReadoutOrder;
			pIRTV->bo_cooler_on				= gIrtvCamStatus.m_coolerOn;
			pIRTV->ao_fpa_off_temp			= gIrtvCamStatus.m_autoTurnOffTemp;
			pIRTV->ao_fpa_on_temp			= gIrtvCamStatus.m_autoTurnOnTemp;
			pIRTV->bo_full_nuc_enable		= gIrtvCamStatus.m_fullNucEnable;
			pIRTV->bo_preset_seq_mode		= gIrtvCamStatus.m_presetSeqMode;
			pIRTV->ao_sel_preset_num		= gIrtvCamStatus.m_configPreset;
			pIRTV->ao_trigger_mode			= gIrtvCamStatus.m_triggerMode;
			pIRTV->bo_sync_polarity			= (int)pIRTV->mbbi_sts_sync_polarity;
			pIRTV->ao_sync_source			= pIRTV->mbbi_sts_sync_src;

			for (int i = 0; i < IRTV_PRESET_NUM; i++) {
				pIRTV->ao_preset_int_time[i]	= gIrtvCamStatus.m_presetIntTime[i];
			}

			pIRTV->sample_rate				= pIRTV->ai_sts_frame_rate;
#endif
		}
	}

	kLog (K_DEBUG, "[IrtvCommand::copyConfigValues] end ... \n");

	return (OK);
}

int IrtvCommand::writePVs (IrtvCameraStatus &camStatus)
{
	// Output PV의 값 설정
	retryCaputByFloat (sfwEnvGet("PV_NAME_SAMPLING_RATE"), camStatus.m_frameRate);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_X"), camStatus.m_xOffset);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_Y"), camStatus.m_yOffset);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_WIDTH"), camStatus.m_width);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_HEIGHT"), camStatus.m_height);
	//retryCaputByInt (sfwEnvGet("IRTV_ROI_LIFETIME"), camStatus.m_lifeTime);
	//retryCaputByInt (sfwEnvGet("IRTV_ROI_COLOR"), camStatus.m_color);
	//retryCaputByInt (sfwEnvGet("IRTV_ROI_COORDI"), camStatus.m_coordi);
	retryCaputByInt (sfwEnvGet("IRTV_SYNC_SOURCE"), camStatus.m_syncSource);
	retryCaputByInt (sfwEnvGet("IRTV_SYNC_POLARITY"), camStatus.m_syncPolarity);
	retryCaputByInt (sfwEnvGet("IRTV_TRIGGER_MODE"), camStatus.m_triggerMode);
	retryCaputByInt (sfwEnvGet("IRTV_GAIN_LEVEL"), camStatus.m_gain);
	retryCaputByInt (sfwEnvGet("IRTV_INVERT_ENABLE"), camStatus.m_rowReadoutOrder);
	retryCaputByInt (sfwEnvGet("IRTV_REVERT_ENABLE"), camStatus.m_colReadoutOrder);
	retryCaputByInt (sfwEnvGet("IRTV_FPA_COOLER_ON"), camStatus.m_coolerOn);
	retryCaputByFloat (sfwEnvGet("IRTV_FPA_OFF_TEMP"), camStatus.m_autoTurnOffTemp);
	retryCaputByFloat (sfwEnvGet("IRTV_FPA_ON_TEMP"), camStatus.m_autoTurnOnTemp);
	retryCaputByInt (sfwEnvGet("IRTV_SEL_PRESET_NUM"), camStatus.m_configPreset);
	retryCaputByInt (sfwEnvGet("IRTV_PRESET_SEQ_MODE"), camStatus.m_presetSeqMode);
	retryCaputByInt (sfwEnvGet("IRTV_FULL_NUC_ENABLE"), camStatus.m_fullNucEnable);

	char	pvName[64];

	for (int i = 0; i < IRTV_PRESET_NUM; i++) {
		sprintf (pvName, "%s%d", sfwEnvGet("IRTV_FPA_INT_TIME"), i);		
		retryCaputByFloat (pvName, camStatus.m_presetIntTime[i]);
		epicsThreadSleep (1);
	}

	return (OK);
}

#if 0
// ---------------------------------------------------------------------------------------------
// Set the frame sync source
// ---------------------------------------------------------------------------------------------
// bhpfssInternal : Frame syncs are generated internally using the selected frame rate or superframe rate.
// bhpfssExternal : Frame syncs are generated externally and given to the camera via the Sync In input.
// bhpfssVideo	  : Frame syncs are generated from the video output frame sync.
// ---------------------------------------------------------------------------------------------
#endif
// ---------------------------------------------------------------------------------------------
// Source used to trigger output when not in free-run mode
// ---------------------------------------------------------------------------------------------
// bhptsInternal : Use internal trigger rate to trigger output
// bhptsExternal : Use internal trigger rate to trigger output
// bhptsSoftware : Use internal trigger rate to trigger output
// bhptsIRIG     : Use internal trigger rate to trigger output
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setSyncSource (const int nSyncSource, const int nSyncPolarity)
{
	kLog (K_MON, "[IrtvCommand::setSyncSource] sync source(%d) polarity(%d)\n", nSyncSource, nSyncPolarity);

	UInt8	currSyncSource		= 0;
	UInt8	currSyncPolarity	= 0;

#if 0
	bhpGetFrameSyncSource (m_resHandle, &currSyncSource);
	bhpGetFrameSyncPolarity (m_resHandle, &currSyncPolarity);

	bhpSetFrameSyncSource (m_resHandle, nSyncSource);
	bhpSetFrameSyncPolarity (m_resHandle, nSyncPolarity);
#else
	bhpGetTriggerSource (m_resHandle, &currSyncSource);
	bhpGetTriggerPolarity (m_resHandle, &currSyncPolarity);

	bhpSetTriggerSource (m_resHandle, nSyncSource);
	bhpSetTriggerPolarity (m_resHandle, nSyncPolarity);
#endif

	kLog (K_MON, "[IrtvCommand::setSyncSource] sync(%d -> %d) polarity(%d -> %d) \n", currSyncSource, nSyncSource, currSyncPolarity, nSyncPolarity);
	kLog (K_MON, "[IrtvCommand::setSyncSource] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Set the frame sync source
// ---------------------------------------------------------------------------------------------
//  - bhppolActiveLow  : Signal is active when it is driven low
//  - bhppolActiveHigh : Signal is active when it is driven high
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setSyncPolarity (const int nSyncPolarity)
{
	kLog (K_MON, "[IrtvCommand::setSyncPolarity] nSyncPolarity(%d)\n", nSyncPolarity);

	UInt8	currSyncPolarity	= 0;

#if 0
	bhpGetFrameSyncPolarity (m_resHandle, &currSyncPolarity);
	bhpSetFrameSyncPolarity (m_resHandle, nSyncPolarity);
#else
	bhpGetTriggerPolarity (m_resHandle, &currSyncPolarity);
	bhpSetTriggerPolarity (m_resHandle, nSyncPolarity);
#endif

	kLog (K_MON, "[IrtvCommand::setSyncPolarity] SyncPolarity(%d -> %d)\n", currSyncPolarity, nSyncPolarity);
	kLog (K_MON, "[IrtvCommand::setSyncPolarity] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Sets the number of sequences output in triggered sequence mode
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setTriggerSeqCount (const UInt32 nSeqCount)
{
	kLog (K_MON, "[IrtvCommand::setTriggerSeqCount] nSeqCount(%d)\n", nSeqCount);

	UInt32	currSeqCount	= 0;

	bhpGetSequenceCount (m_resHandle, &currSeqCount);
	bhpSetSequenceCount (m_resHandle, nSeqCount);

	kLog (K_MON, "[IrtvCommand::setTriggerSeqCount] nSeqCount(%d -> %d)\n", currSeqCount, nSeqCount);
	kLog (K_MON, "[IrtvCommand::setTriggerSeqCount] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Set the trigger mode
// ---------------------------------------------------------------------------------------------
// bhptmFreeRun					: No trigger is used, the camera outputs frames continuosly (free runs)
// bhptmTriggeredFreeRun		: A trigger cause the camera to output frames continuously
// bhptmTriggeredSequence		: A trigger outputs a number of sequences (then stops)
// bhptmTriggeredPresetAdvance	: The current preset advances to the next preset in the sequence when a trigger is recieved
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setTriggerMode (const int nTriggerMode)
{
	kLog (K_MON, "[IrtvCommand::setTriggerMode] trigger mode(%d)\n", nTriggerMode);

	UInt8	currTriggerMode	= 0;

	bhpGetTriggerMode  (m_resHandle, &currTriggerMode);
	bhpSetTriggerMode  (m_resHandle, nTriggerMode);

	kLog (K_MON, "[IrtvCommand::setTriggerMode] trigger mode (%d -> %d) \n", currTriggerMode, nTriggerMode);
	kLog (K_MON, "[IrtvCommand::setTriggerMode] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Set the active preset (single preset mode)
// ---------------------------------------------------------------------------------------------
// ps Active preset (0-3) 
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setActivePreset (const int nActivePreset)
{
	kLog (K_MON, "[IrtvCommand::setActivePreset] nActivePreset(%d)\n", nActivePreset);

	UInt8	currActivePreset	= 0;

	if (errOk != bhpGetActivePreset (m_resHandle, &currActivePreset)) {
		kLog (K_ERR, "[IrtvCommand::setActivePreset] bhpGetActivePreset() failed ...\n");
		return (NOK);
	}

	kLog (K_MON, "[IrtvCommand::setActivePreset] nActivePreset(%d -> %d)\n", currActivePreset, nActivePreset);

	if (errOk != bhpSetActivePreset (m_resHandle, nActivePreset)) {
		kLog (K_ERR, "[IrtvCommand::setActivePreset] bhpSetActivePreset() failed ...\n");
		return (NOK);
	}

	kLog (K_MON, "[IrtvCommand::setActivePreset] end ... \n");

	return (OK);
}

//TODO : Config preset을 active preset과 동일한 것으로 간주함
// ---------------------------------------------------------------------------------------------
// Set the config preset
// ---------------------------------------------------------------------------------------------
// BHPSDK에는 config preset을 위한 API가 존재하지 않는 것으로 보임
// 따라서, active preset과 동일한 것으로 간주함
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setConfigPreset (const int nConfigPreset)
{
	kLog (K_MON, "[IrtvCommand::setConfigPreset] nConfigPreset(%d)\n", nConfigPreset);

	return (setActivePreset (nConfigPreset));
}

// ---------------------------------------------------------------------------------------------
// Set the integration time in milliseconds.
// ---------------------------------------------------------------------------------------------
// The camera uses clocks internally and all calculations and camera setup is done in clocks. 
// Utility functions are provided to convert between clocks and more meaningfull units. 
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setFPAIntTime (const int nPreset, const double fFPAIntTime)
{
	kLog (K_MON, "[IrtvCommand::setFPAIntTime] nPreset(%d) IntTime(%f)\n", nPreset, fFPAIntTime);

	Flt64	currFPAIntTime	= 0;

	bhpGetIntegrationTime (m_resHandle, nPreset, &currFPAIntTime);

	kLog (K_MON, "[IrtvCommand::setFPAIntTime] Preset(%d) IntTime(%f -> %f)\n", nPreset, currFPAIntTime, fFPAIntTime);

	// 약 1초 이상이 소요됨
	bhpSetIntegrationTime (m_resHandle, nPreset, fFPAIntTime);

	kLog (K_MON, "[IrtvCommand::setFPAIntTime] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Set the digital gain
// ---------------------------------------------------------------------------------------------
// The digital gain and offset are applied to the digital data before 
// it is output using the following formula: output pixel = (raw pixel * gain) + offset.
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setGainLevel (const int nGainLevel)
{
	kLog (K_MON, "[IrtvCommand::setGainLevel] nGainLevel(%f)\n", nGainLevel);

	UInt8	currGainLevel	= 0;

	bhpGetGainLevel (m_resHandle, &currGainLevel);

	kLog (K_MON, "[IrtvCommand::setGainLevel] level(%d -> %d)\n", currGainLevel, nGainLevel);

	bhpSetGainLevel (m_resHandle, nGainLevel);

	kLog (K_MON, "[IrtvCommand::setGainLevel] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Set the frame rate in herZ
// ---------------------------------------------------------------------------------------------
// Frame width/rate is used when syncing internally and when in single preset or preset sequencing mode. 
// When in superframing mode use the superframe width/rate instead. 
// The camera uses clocks internally and all calculations and camera setup is done in clocks. 
// Utility functions are provided to convert between clocks and more meaningfull units. 
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setFrameRate (const double fFrameRate)
{
	kLog (K_MON, "[IrtvCommand::setFrameRate] frame rate (%f) \n", fFrameRate);

	UInt8	nPreset			= getActivePreset();
	Flt64	currFrameRate	= 0;

	bhpGetFrameRate (m_resHandle, nPreset, &currFrameRate);

	kLog (K_MON, "[IrtvCommand::setFrameRate] %f -> %f\n", currFrameRate, fFrameRate);

	bhpSetFrameRate (m_resHandle, nPreset, fFrameRate);

	//TODO : 모든 preset 설정이 필요할지?
#if 0
	kLog (K_MON, "[IrtvCommand::setFrameRate] id(%d) : Preset(%d) set ... \n", m_nCtrlDeviceId, 0);
	iacfSetCFGConfigPreset (m_nCtrlDeviceId, 0);
	iacfSetFPAFrameRate (m_nCtrlDeviceId, nFrameRate);

	kLog (K_MON, "[IrtvCommand::setFrameRate] id(%d) : Preset(%d) set ... \n", m_nCtrlDeviceId, 1);
	iacfSetCFGConfigPreset (m_nCtrlDeviceId, 1);
	iacfSetFPAFrameRate (m_nCtrlDeviceId, nFrameRate);

	kLog (K_MON, "[IrtvCommand::setFrameRate] id(%d) : Preset(%d) set ... \n", m_nCtrlDeviceId, 2);
	iacfSetCFGConfigPreset (m_nCtrlDeviceId, 2);
	iacfSetFPAFrameRate (m_nCtrlDeviceId, nFrameRate);

	kLog (K_MON, "[IrtvCommand::setFrameRate] id(%d) : Preset(%d) set ... \n", m_nCtrlDeviceId, 3);
	iacfSetCFGConfigPreset (m_nCtrlDeviceId, 3);
	iacfSetFPAFrameRate (m_nCtrlDeviceId, nFrameRate);
#endif

	kLog (K_MON, "[IrtvCommand::setFrameRate] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Set the FPA window
// ---------------------------------------------------------------------------------------------
// It is possible for the camrea to have a different fpa window for each preset, however most software 
// does not support this and most frame grabbers do not allow the frame size to change when sequencing. 
// Usually the window is set for all presets using 4 calls to bhpSetWindow 
// and the window is queried using preset 0 and the other presets are assumed to have the same window.
// ---------------------------------------------------------------------------------------------
// xOffset			: X offset from the FPA origin to the window origin
// yOffset			: Y offset from the FPA origin to the window origin
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setROI (const UInt16 nOffsetX, const UInt16 nOffsetY, 
						 const UInt16 nWidth, const UInt16 nHeight, const UInt16 nLifeTime, const UInt8 nColor, const Int32 nCoordi)
{
	kLog (K_MON, "[IrtvCommand::setROI] bhpGetWindow() \n");

	UInt16	currX, currY, currWidth, currHeight, currLifeTime; 
	Int32	currCoordi;
	UInt8	currColor;

	UInt8	nPreset			= getActivePreset();

	currX = currY = currWidth = currHeight = currLifeTime = 0;
	currCoordi = currColor = 0;

	bhpGetWindow  (m_resHandle, nPreset, &currX,  &currY, &currWidth, &currHeight);

	kLog (K_MON, "[IrtvCommand::setROI] get : x(%d) y(%d) width(%d) height(%d) lifetime(%d) color(%d) coordi(%d)\n", 
		currX, currY, currWidth, currHeight, currLifeTime, currColor, currCoordi);

	kLog (K_MON, "[IrtvCommand::setROI] set : x(%d) y(%d) width(%d) height(%d) lifetime(%d) color(%d) coordi(%d)\n", 
		nOffsetX, nOffsetY, nWidth, nHeight, nLifeTime, nColor, nCoordi);

	bhpSetWindow (m_resHandle, nPreset, nOffsetX, nOffsetY, nWidth, nHeight);

	kLog (K_MON, "[IrtvCommand::setSyncSource] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.219 iacfSetFPARowReadoutOrder
// ---------------------------------------------------------------------------------------------
// Sets the FPA row readout order.
// The FPA row readout order determines the order in which rows are read out from the detector.
// This corresponds to image invert (flip image data vertically) setting.
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setInvertEnable (const int bEnable)
{
	kLog (K_MON, "[IrtvCommand::setInvertEnable] bEnable(%d)\n", bEnable);

	bool	nRowReadoutOrder	= 0;

	bhpGetInvertEnabled (m_resHandle, &nRowReadoutOrder);

	kLog (K_MON, "[IrtvCommand::setInvertEnable] RowReadoutOrder(%d -> %d)\n", nRowReadoutOrder, bEnable);

	bhpSetInvertEnabled (m_resHandle, bEnable);

	kLog (K_MON, "[IrtvCommand::setInvertEnable] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.217 iacfSetFPAColReadoutOrder
// ---------------------------------------------------------------------------------------------
// Sets the FPA col readout order.
// The FPA column readout order determines the order in which columns are read out from the detector.
// This corresponds to image invert (flip image data horizontally) setting.
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setRevertEnable (const int bEnable)
{
	kLog (K_MON, "[IrtvCommand::setRevertEnable] bEnable(%d)\n", bEnable);

	bool	nColReadoutOrder	= 0;

	bhpGetRevertEnabled (m_resHandle, &nColReadoutOrder);

	kLog (K_MON, "[IrtvCommand::setRevertEnable] ColReadoutOrder(%d -> %d)\n", nColReadoutOrder, bEnable);

	bhpSetRevertEnabled (m_resHandle, bEnable);

	kLog (K_MON, "[IrtvCommand::setRevertEnable] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Get the flags cooler enable state.
// ---------------------------------------------------------------------------------------------
// The cooler/heater will only run when the flag is in the field of view (FOV). 
// So if the cooler is disabled it will not run when the flag is stowed or in the FOV; 
// and if the cooler is enabled it will not run when the flag is stowed, but will run when the flag is in the FOV. 
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setCoolerOn (const int bCoolerOn)
{
	kLog (K_MON, "[IrtvCommand::setCoolerOn] nSyncPolarity(%d)\n", bCoolerOn);

	bool	currCoolerOn	= 0;

	bhpGetFlagCoolerEnabled (m_resHandle, &currCoolerOn);

	kLog (K_MON, "[IrtvCommand::setCoolerOn] FPACoolerOn(%d -> %d)\n", currCoolerOn, bCoolerOn);

	bhpSetFlagCoolerEnabled (m_resHandle, bCoolerOn);

	kLog (K_MON, "[IrtvCommand::setCoolerOn] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Get the preset sequencing mode.
// ---------------------------------------------------------------------------------------------
// bhppsmSinglePreset     : Output a single preset given by ActivePreset
// bhppsmPresetSequencing : Output one or more presets controlled by dwell
// bhppsmSuperframing     : Special case of preset sequencing where dwell is 0 or 1, controlled by SubframeEnabled
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setPresetSeqMode (const int nPresetSeqMode)
{
	kLog (K_MON, "[IrtvCommand::setPresetSeqMode] nPresetSeqMode(%d)\n", nPresetSeqMode);

	UInt8	currPresetSeqMode	= 0;
	Int32	currPresetCycling	= 0;
	Int32	nPresetCycling		= nPresetSeqMode;

	bhpGetPresetSequencingMode (m_resHandle, &currPresetSeqMode);
	//iacfGetCFGPscEnableCycling (m_resHandle, &currPresetCycling);

	kLog (K_MON, "[IrtvCommand::setPresetSeqMode] SeqMode(%d -> %d) Cycling(%d -> %d)\n", 
		currPresetSeqMode, nPresetSeqMode, currPresetCycling, nPresetCycling);

	bhpSetPresetSequencingMode (m_resHandle, nPresetSeqMode);
	// 두 값을 같이 설정해 주어야 SC6000 Controller의 상태와 일치하게 동작함
	//iacfSetCFGPscEnableCycling (m_resHandle, nPresetCycling);

	kLog (K_MON, "[IrtvCommand::setPresetSeqMode] end ... \n");

	return (OK);
}

int IrtvCommand::sendSoftTrigger ()
{
	kLog (K_MON, "[IrtvCommand::sendSoftTrigger] software trigger (1) \n");

	bhpSoftwareTrigger (m_resHandle);

	kLog (K_MON, "[IrtvCommand::sendSoftTrigger] end ... \n");

	return (OK);
}


// ---------------------------------------------------------------------------------------------
// Reset the trigger when in triggerd then free run mode
// ---------------------------------------------------------------------------------------------
// When in triggered then free run mode (bhptmTriggeredFreeRun) 
// the camera will output frames continuously once triggered. 
// Rearming the trigger stops output and makes the camera wait for a new trigger before outputing frames
// ---------------------------------------------------------------------------------------------
int IrtvCommand::sendRearmTrigger ()
{
	kLog (K_MON, "[IrtvCommand::sendRearmTrigger] rearm trigger (1) \n");

	bhpRearmTrigger  (m_resHandle);

	kLog (K_MON, "[IrtvCommand::sendRearmTrigger] end ... \n");

	return (OK);
}

int IrtvCommand::setAGCEnable (const int nAGCEnable)
{
	kLog (K_MON, "[IrtvCommand::setAGCEnable] nAGCEnable (%d) \n", nAGCEnable);

	bool	currAGCEnable	= 0;

	//TODO : 맞는지???
	bhpGetVideoAGCROIEnabled (m_resHandle, &currAGCEnable);

	if (nAGCEnable != (int)currAGCEnable) {
		kLog (K_MON, "[IrtvCommand::setAGCEnable] %d -> %d \n", currAGCEnable, nAGCEnable);

		bhpSetVideoAGCROIEnabled (m_resHandle, nAGCEnable);
	}

	kLog (K_MON, "[IrtvCommand::setAGCEnable] end ... \n");

	return (OK);
}

int IrtvCommand::getCameraSetting (IrtvCameraStatus &camStatus)
{
	kLog (K_MON, "[IrtvCommand::getCameraSetting] get cameara settings \n");

	//TODO : 꼭 필요한지???
	//if (errOk != iacfGetCFGPscEnableCycling (m_nCtrlDeviceId, &camStatus.m_presetCycling)) { return (NOK); }

	getCameraPreset (camStatus);

	camStatus.printSetting ();

	kLog (K_MON, "[IrtvCommand::getCameraSetting] end ... \n");

	return (OK);
}

int IrtvCommand::getCameraPreset ()
{
	return (getCameraPreset (gIrtvCamStatus));
}

int IrtvCommand::getCameraPreset (IrtvCameraStatus &camStatus)
{
	kLog (K_INFO, "[IrtvCommand::getCameraPrset] get cameara preset \n");

	for (int i = 0; i < IRTV_PRESET_NUM; i++) {
		if (errOk != bhpGetFrameRate (m_resHandle, i, &camStatus.m_presetFrameRate[i])) { return (NOK); }
		if (errOk != bhpGetGainLevel (m_resHandle, &camStatus.m_presetGain[i])) { return (NOK); }
		if (errOk != bhpGetIntegrationTime (m_resHandle, i, &camStatus.m_presetIntTime[i])) { return (NOK); }

		if (errOk != bhpGetWindow (m_resHandle, i, 
						&camStatus.m_presetXOffset[i], &camStatus.m_presetYOffset[i],
						&camStatus.m_presetWidth[i], &camStatus.m_presetHeight[i])) {
			return (NOK);
		}

		if (errOk != bhpGetNUCName (m_resHandle, i, 
						camStatus.m_presetNucName[i], sizeof(camStatus.m_presetNucName[i]))) { return (NOK); }
	}

	copyConfigValues (camStatus);

	kLog (K_DEBUG, "[IrtvCommand::getCameraSetting] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// Load a NUC into a preset
// ---------------------------------------------------------------------------------------------
// Starts the NUC process.  
// The NUC process will create a non uniformity correction that can be applied to analog and digital data.
// ---------------------------------------------------------------------------------------------
int IrtvCommand::loadNucAction (const char *pNucName, const int nConfigPreset, const int nActivePreset)
{
	kLog (K_MON, "[IrtvCommand::loadNucAction] name(%s) config(%d) active(%d)\n", pNucName, nConfigPreset, nActivePreset);

	if ( (NULL == pNucName) || (0 == strlen(pNucName)) ) {
		kLog (K_MON, "[IrtvCommand::loadNucAction] name is invalid \n", pNucName);
		return (NOK);
	}

	// Loads the NUC named by the load NUC name into the preset given by the load NUC preset slot
	bhpLoadNUC (m_resHandle, nConfigPreset, (char *)pNucName);

	// 로드 후 상태 업데이트 하기
	getCameraPreset ();

	kLog (K_MON, "[IrtvCommand::loadNucAction] end ... \n");

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 4.4 Receiving a directory listing
// ---------------------------------------------------------------------------------------------
// 또는, ThermoVision SC6000 SDK\examples\iacfSDKExample\main.cpp 참조
// ---------------------------------------------------------------------------------------------
int IrtvCommand::getStoredNUCs ()
{
	kLog (K_MON, "[IrtvCommand::getStoredNUCs] \n");

	// nuc 폴더로부터 nuc 확장자를 갖는 파일 목록 관리
	IrtvCamFile	nucFS (m_resHandle, "nuc", ".nuc");

	// 해당 파일의 목록 얻기
	nucFS.makeFileList ();

	vector<string> &vList = nucFS.getFileList ();
    vector<string>::iterator pos;

	ST_IRTV	*pIRTV	= getFirstDriver();

	for (UInt32 i = 0; i < vList.size(); i++) {
		strcpy (pIRTV->wave_stored_nucs[i], vList[i].c_str());
	}

	pIRTV->num_stored_nucs	= vList.size();

	kLog (K_MON, "[IrtvCommand::getStoredNUCs] cnt(%d) \n", nucFS.getNumOfFile());
	kLog (K_MON, "[IrtvCommand::getStoredNUCs] end ... \n");

	return (OK);
}

bhpSCalInfo * IrtvCommand::getCalibration ()
{
	static bool bFirst = true;

	//if (NULL != m_pCalib) {
	// 20130816 : 장치담당자의 요청으로 1회만 시도함
	if ( (NULL != m_pCalib) || (true != bFirst) ) {
		return (m_pCalib);
	}

	bFirst = false;

	kLog (K_DEBUG, "[IrtvCommand::getCalibration] calibration \n");

	int		i;
	bool	calibrated;

	if (!camera.Controller->GetIsFactoryCalibrated(calibrated)) {
		kLog (K_ERR, "[IrtvCommand::getCalibration] GetIsFactoryCalibrated FAILED\n");
		camera.Disconnect();
		return (NULL);
	}

	if (!calibrated) {
		kLog (K_ERR, "[IrtvCommand::getCalibration] Camera is not calibrated.\n");
		return (NULL);
	}

	TArray<IController::SFactoryCalibration>	cals;

	if (!camera.Controller->GetFactoryCalibrations(cals)) {
		camera.Disconnect();
		kLog (K_ERR, "[IrtvCommand::getCalibration] GetFactoryCalibrations FAILED\n");
		return (NULL);
	}

	for (i = 0; i < (int)cals.GetSize(); ++i) {
		kLog (K_INFO, "[IrtvCommand::getCalibration] %d) %S\n", i, cals[i].name.GetTChar());
	}

	int		preset	= getActivePreset();
	int		nCalibIdx = -2;
	char	tag[256];
		
	kLog (K_DEBUG, "[IrtvCommand::getCalibration] GetActiveFactoryCalibration(%d) \n", preset);

	if (!camera.Controller->GetActiveFactoryCalibration (preset, nCalibIdx)) {
		camera.Disconnect();
		kLog (K_ERR, "[IrtvCommand::getCalibration] GetActiveFactoryCalibration FAILED\n");
		return (NULL);
	}

	kLog (K_INFO, "[IrtvCommand::getCalibration] calib %d\n", nCalibIdx);

	if (bhpGetCalTag (m_resHandle, preset, tag, sizeof(tag)) != errOk) {
		kLog (K_ERR, "[IrtvCommand::getCalibration] bhpGetCalTag FAILED\n");
		return (NULL);
	}

	kLog (K_INFO, "[IrtvCommand::getCalibration] tag (%s)\n", tag);

	if (NULL == m_pCalib) {
		if (NULL == (m_pCalib = (bhpSCalInfo *)calloc (1, sizeof(bhpSCalInfo)))) {
			kLog (K_ERR, "[IrtvCommand::getCalibration] calloc FAILED\n");
			return (NULL);
		}
	}

	if (bhpGetCalInfo (m_resHandle, tag, m_pCalib, false) != errOk) {
		free (m_pCalib);
		m_pCalib = NULL;

		kLog (K_INFO, "[IrtvCommand::getCalibration] bhpGetCalInfo FAILED\n");
		return (NULL);
	}

	kLog (K_MON, "[IrtvCommand::getCalibration] end ... \n");

	return (m_pCalib);
}

bhpSCalInfo * IrtvCommand::getCalibrationFromFile ()
{
	kLog (K_MON, "[IrtvCommand::getCalibrationFromFile] calibration \n");

	bhpSCalInfo	calib;
	FILE		*fpCalib	= NULL;

	if (NULL == (fpCalib = fopen (IRTV_CALIB_FILE_NAME, "rb"))) {
		kLog (K_ERR, "[IrtvCommand::getCalibrationFromFile] Can't open calib file (%s)\n", IRTV_CALIB_FILE_NAME);
		return (NULL);
	}

	if (1 != fread (&calib, sizeof(bhpSCalInfo), 1, fpCalib)) {
		kLog (K_ERR, "[IrtvCommand::getCalibrationFromFile] Can't read calib from file (%s)\n", IRTV_CALIB_FILE_NAME);
		return (NULL);
	}

	fclose (fpCalib);

	if (NULL == m_pCalib) {
		if (NULL == (m_pCalib = (bhpSCalInfo *)calloc (1, sizeof(bhpSCalInfo)))) {
			kLog (K_ERR, "[IrtvCommand::getCalibrationFromFile] calloc FAILED\n");
			return (NULL);
		}
	}

	memcpy (m_pCalib, &calib, sizeof(bhpSCalInfo));

	kLog (K_MON, "[IrtvCommand::getCalibrationFromFile] end ... \n");

	return (m_pCalib);
}

void IrtvCommand::releaseCalibration ()
{
	if (NULL != m_pCalib) {
		free (m_pCalib);
		m_pCalib = NULL;
	}
}

#if 0
// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.304 iacfGetCLRCoolerType
// ---------------------------------------------------------------------------------------------
// Gets the cooler type.  
// The cooler type determines what cooler is installed in the camera 
// and also determines what cooler controls are used
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setCoolerType (const int nCoolerType)
{
	kLog (K_MON, "[IrtvCommand::setCoolerType] nCoolerType(%d)\n", nCoolerType);

	Int32	currCoolerType	= 0;

	iacfGetCLRCoolerType (m_nCtrlDeviceId, &currCoolerType);

	kLog (K_MON, "[IrtvCommand::setCoolerType] id(%d) : nCoolerType(%d -> %d)\n", 
		m_nCtrlDeviceId, currCoolerType, nCoolerType);

	iacfSetCLRCoolerType (m_nCtrlDeviceId, nCoolerType);

	kLog (K_MON, "[IrtvCommand::setCoolerType] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

int IrtvCommand::setCoolerAuto (const int nCoolerAuto)
{
	kLog (K_MON, "[IrtvCommand::setCoolerAuto] nCoolerAuto(%d)\n", nCoolerAuto);

#if 0
	Int32	currCoolerAuto	= 0;

	iacfGetCLRCoolerType (m_nCtrlDeviceId, &currCoolerAuto);

	kLog (K_MON, "[IrtvCommand::setCoolerAuto] id(%d) : nCoolerAuto(%d -> %d)\n", 
		m_nCtrlDeviceId, currCoolerAuto, nCoolerAuto);

	iacfSetCLRCoolerType (m_nCtrlDeviceId, nCoolerAuto);
#else
	//TODO
	// <iacfHCLR.h> : iacfSetHCLRFlagCurrentTemperature() 참조
#endif

	kLog (K_MON, "[IrtvCommand::setCoolerAuto] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACFCLR.h
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setFPAAutoTurnOffTemp (const int nTurnOffTemp)
{
	kLog (K_MON, "[IrtvCommand::setFPAAutoTurnOffTemp] nTurnOffTemp(%d)\n", nTurnOffTemp);

	UInt32	currTemp	= 0;

	iacfGetCLRFpaAutoTurnOffTempLimit (m_nCtrlDeviceId, &currTemp);

	kLog (K_MON, "[IrtvCommand::setFPAAutoTurnOffTemp] id(%d) : nTurnOffTemp(%d -> %d)\n", 
		m_nCtrlDeviceId, currTemp / 1000, nTurnOffTemp);

	iacfSetCLRFpaAutoTurnOffTempLimit (m_nCtrlDeviceId, nTurnOffTemp * 1000);

	kLog (K_MON, "[IrtvCommand::setFPAAutoTurnOffTemp] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACFCLR.h
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setFPAAutoTurnOnTemp (const int nTurnOnTemp)
{
	kLog (K_MON, "[IrtvCommand::setFPAAutoTurnOnTemp] nTurnOnTemp(%d)\n", nTurnOnTemp);

	UInt32	currTemp	= 0;

	iacfGetCLRFpaAutoTurnOnTempLimit (m_nCtrlDeviceId, &currTemp);

	kLog (K_MON, "[IrtvCommand::setFPAAutoTurnOnTemp] id(%d) : nTurnOnTemp(%d -> %d)\n", 
		m_nCtrlDeviceId, currTemp / 1000, nTurnOnTemp);

	iacfSetCLRFpaAutoTurnOnTempLimit (m_nCtrlDeviceId, nTurnOnTemp * 1000);

	kLog (K_MON, "[IrtvCommand::setFPAAutoTurnOnTemp] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.181 iacfSetCFGFullWindowNuc
// ---------------------------------------------------------------------------------------------
// Sets the CFG full window NUC enable state.  
// When enabled, NUCs will always be performed with a full sized window 
// and then windowed down to the current window size
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setFullNucEnable (const int bFullNucEnable)
{
	kLog (K_MON, "[IrtvCommand::setFullNucEnable] FullNucEnable(%d)\n", bFullNucEnable);

	Int32	currFullNucEnable	= 0;

	iacfGetCFGFullWindowNuc (m_nCtrlDeviceId, &currFullNucEnable);

	kLog (K_MON, "[IrtvCommand::setFullNucEnable] id(%d) : FullNucEnable(%d -> %d)\n", 
		m_nCtrlDeviceId, currFullNucEnable, bFullNucEnable);

	iacfSetCFGFullWindowNuc (m_nCtrlDeviceId, bFullNucEnable);

	kLog (K_MON, "[IrtvCommand::setFullNucEnable] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

#endif

void IrtvCommand::lock ()
{
	//TODO : 필요 없는지?
}

void IrtvCommand::unLock ()
{
	//TODO : 필요 없는지?
}

#if 0
// mode : 1(640x512 FPA sizes), 2(320x256 FPA sizes)
double IrtvCommand::getMaxFrameRate (const int mode, const int width, const int height)
{
	double	dCalcRate;
	double	dRetRate;
	char	szRate[20];

	if (2 == mode) {
		dCalcRate	= 50000000 / ((height + 2) * (width + 128) + 8);
	}
	else {
		dCalcRate	= 50000000 / ((height + 3) * (width + 128) + 8);
	}

	sprintf (szRate, "%.1f", dCalcRate);
	dRetRate	= atof(szRate);

	kLog (K_INFO, "[] mode(%d) width(%d) height(%d) rate(%.2f) ret(%.2f) \n",
		mode, width, height, dCalcRate, dRetRate);

	return (dRetRate);
}
#endif