#include <stdio.h>

#include "sfwCommon.h"
#include "sfwGlobalDef.h"

#include "IrtvCommand.h"
#include "IrtvCamInterface.h"
#include "IrtvCamFile.h"

#include "iacfUtil.h"
#include "ISC_Camera.h"
#include "IACFSDK.h"
#include "IACFHCLR.h"
#include "CieloSDK.h"

#pragma comment(lib, "ISC_Camera.lib")
#pragma comment(lib, "PhoenixProtocol.lib")
#pragma comment(lib, "IACFProtocol.lib")
#pragma comment(lib, "CieloSDK.lib")
#pragma comment(lib, "IACFSDK.lib")
#pragma comment(lib, "IACFUTIL.lib")

#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")		// for ConvertBSTRToString()
#else
#pragma comment(lib, "comsuppw.lib")
#endif

IrtvSync			gIrtvCommandSync;
iacfCalib			gIrtvCalibration	= NULL;
IrtvCameraStatus	gIrtvCamStatus;

extern "C" int irtvSetGainLevel (const int nGainLevel)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setGainLevel (nGainLevel);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetInvertEnable (const int bEnable)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setInvertEnable (bEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetRevertEnable (const int bEnable)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setRevertEnable (bEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetSyncPolarity (const int bEnable)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setSyncPolarity (bEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetCoolerOn (const int bCoolerOn)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setCoolerOn (bCoolerOn);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetCoolerType (const int nCoolerType)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setCoolerType (nCoolerType);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetCoolerAuto (const int nCoolerAuto)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setCoolerAuto (nCoolerAuto);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFPAAutoOffTemp (const int nCoolerOffTemp)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setFPAAutoTurnOffTemp (nCoolerOffTemp);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFPAAutoOnTemp (const int nCoolerOnTemp)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setFPAAutoTurnOnTemp (nCoolerOnTemp);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetConfigPreset (const int nConfigPreset)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setConfigPreset (nConfigPreset);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetActivePreset (const int nActivePreset)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setActivePreset (nActivePreset);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetPresetSeqMode (const int bPresetSeqMode)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setPresetSeqMode (bPresetSeqMode);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFullNucEnable (const int bFullNucEnable)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setFullNucEnable (bFullNucEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvLoadNucAction (const char *pNucName, const int nConfigPreset, const int nActivePreset)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.loadNucAction (pNucName, nConfigPreset, nActivePreset);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetFPAIntTime (const int nPreset, const double fFPAIntTime)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setFPAIntTime (nPreset, fFPAIntTime);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvGetStoredNUCs ()
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.getStoredNUCs ();

	gIrtvCommandSync.unlock ();

	scanIoRequest (getFirstDriver()->ioScanPvt_userCall);

	return (ret);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.251 iacfSetFPAGain
// ---------------------------------------------------------------------------------------------
// Sets the FPA gain.  The FPA gain is an index to a resistor used to apply an analog gain.  
// The number of supported gain levels is FPA dependant
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setGainLevel (const int nGainLevel)
{
	kLog (K_MON, "[IrtvCommand::setGainLevel] nGainLevel(%d)\n", nGainLevel);

	UInt8	currGainLevel	= 0;

	iacfGetFPAGain (m_nCtrlDeviceId, &currGainLevel);

	kLog (K_MON, "[IrtvCommand::setGainLevel] id(%d) : level(%d -> %d)\n", 
		m_nCtrlDeviceId, currGainLevel, nGainLevel);

	iacfSetFPAGain (m_nCtrlDeviceId, nGainLevel);

	kLog (K_MON, "[IrtvCommand::setGainLevel] id(%d) : end ... \n", m_nCtrlDeviceId);

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

	Int32	nRowReadoutOrder	= 0;

	iacfGetFPARowReadoutOrder (m_nCtrlDeviceId, &nRowReadoutOrder);

	kLog (K_MON, "[IrtvCommand::setInvertEnable] id(%d) : RowReadoutOrder(%d -> %d)\n", 
		m_nCtrlDeviceId, nRowReadoutOrder, bEnable);

	iacfSetFPARowReadoutOrder (m_nCtrlDeviceId, bEnable);

	kLog (K_MON, "[IrtvCommand::setInvertEnable] id(%d) : end ... \n", m_nCtrlDeviceId);

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

	iacfConfigInit (m_nCtrlDeviceId);

	Int32	nColReadoutOrder	= 0;

	iacfGetFPAColReadoutOrder (m_nCtrlDeviceId, &nColReadoutOrder);

	kLog (K_MON, "[IrtvCommand::setRevertEnable] id(%d) : ColReadoutOrder(%d -> %d)\n", 
		m_nCtrlDeviceId, nColReadoutOrder, bEnable);

	iacfSetFPAColReadoutOrder (m_nCtrlDeviceId, bEnable);

	kLog (K_MON, "[IrtvCommand::setRevertEnable] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.177 iacfSetCFGExtSyncPolarity
// ---------------------------------------------------------------------------------------------
// Sets the CFG external sync polarity.  The polarity determines when the sync is asserted
//  - POLARITY_POSITIVE_EDGE(0, POLARITY_FALLING_EDGE)
//  - POLARITY_NEGATIVE_EDGE(1, POLARITY_RISING_EDGE)
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setSyncPolarity (const int nSyncPolarity)
{
	kLog (K_MON, "[IrtvCommand::setSyncPolarity] nSyncPolarity(%d)\n", nSyncPolarity);

	Int32	currSyncPolarity	= 0;

	iacfGetCFGExtSyncPolarity (m_nCtrlDeviceId, &currSyncPolarity);

	kLog (K_MON, "[IrtvCommand::setSyncPolarity] id(%d) : SyncPolarity(%d -> %d)\n", 
		m_nCtrlDeviceId, currSyncPolarity, nSyncPolarity);

	iacfSetCFGExtSyncPolarity (m_nCtrlDeviceId, nSyncPolarity);

	kLog (K_MON, "[IrtvCommand::setSyncPolarity] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.295 iacfSetCLRCoolerState
// ---------------------------------------------------------------------------------------------
// Sets the CLR cooler enable state
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setCoolerOn (const int bCoolerOn)
{
	kLog (K_MON, "[IrtvCommand::setCoolerOn] nSyncPolarity(%d)\n", bCoolerOn);

	Int32	currCoolerOn	= 0;

	iacfGetCLRCoolerState (m_nCtrlDeviceId, &currCoolerOn);

	kLog (K_MON, "[IrtvCommand::setCoolerOn] id(%d) : FPACoolerOn(%d -> %d)\n", 
		m_nCtrlDeviceId, currCoolerOn, bCoolerOn);

	iacfSetCLRCoolerState (m_nCtrlDeviceId, bCoolerOn);

	kLog (K_MON, "[IrtvCommand::setCoolerOn] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

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
// IACF SDK Documentation.chm : 3.4.191 iacfSetCFGConfigPreset
// ---------------------------------------------------------------------------------------------
// Sets the CFG configuration preset.  
// The configuration preset is the preset that is currently being configured
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setConfigPreset (const int nConfigPreset)
{
	kLog (K_MON, "[IrtvCommand::setConfigPreset] nConfigPreset(%d)\n", nConfigPreset);

	UInt32	currConfigPreset	= 0;

	if (errOk != iacfGetCFGConfigPreset (m_nCtrlDeviceId, &currConfigPreset)) {
		kLog (K_ERR, "[IrtvCommand::setConfigPreset] id(%d) : iacfGetCFGConfigPreset() failed ...\n", m_nCtrlDeviceId);
		return (NOK);
	}

	kLog (K_MON, "[IrtvCommand::setConfigPreset] id(%d) : nConfigPreset(%d -> %d)\n", 
		m_nCtrlDeviceId, currConfigPreset, nConfigPreset);

	if (errOk != iacfSetCFGConfigPreset (m_nCtrlDeviceId, nConfigPreset)) {
		kLog (K_ERR, "[IrtvCommand::setConfigPreset] id(%d) : iacfSetCFGConfigPreset() failed ...\n", m_nCtrlDeviceId);
		return (NOK);
	}

	kLog (K_MON, "[IrtvCommand::setConfigPreset] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.148 iacfSetCFGActivePreset
// ---------------------------------------------------------------------------------------------
// Sets the CFG active preset.  
// The active preset is the preset used when in single preset mode. This preset is also used for NUCs
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setActivePreset (const int nActivePreset)
{
	kLog (K_MON, "[IrtvCommand::setActivePreset] nActivePreset(%d)\n", nActivePreset);

	UInt32	currActivePreset	= 0;

	if (errOk != iacfGetCFGActivePreset (m_nCtrlDeviceId, &currActivePreset)) {
		kLog (K_ERR, "[IrtvCommand::setActivePreset] id(%d) : iacfGetCFGActivePreset() failed ...\n", m_nCtrlDeviceId);
		return (NOK);
	}

	kLog (K_MON, "[IrtvCommand::setActivePreset] id(%d) : nActivePreset(%d -> %d)\n", 
		m_nCtrlDeviceId, currActivePreset, nActivePreset);

	if (errOk != iacfSetCFGActivePreset (m_nCtrlDeviceId, nActivePreset)) {
		kLog (K_ERR, "[IrtvCommand::setActivePreset] id(%d) : iacfSetCFGActivePreset() failed ...\n", m_nCtrlDeviceId);
		return (NOK);
	}

	kLog (K_MON, "[IrtvCommand::setActivePreset] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.158 iacfSetCFGPscSeqMode
// ---------------------------------------------------------------------------------------------
// Sets the CFG preset sequencing sequence mode.  
// This value is used in conjunction with enable cycling
// TRIG_SINGLE_PRESET, TRIG_SINGLE_SEQUENCE, TRIG_REPEATED_SEQUENCE
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setPresetSeqMode (const int nPresetSeqMode)
{
	kLog (K_MON, "[IrtvCommand::setPresetSeqMode] nPresetSeqMode(%d)\n", nPresetSeqMode);

	Int32	currPresetSeqMode	= 0;
	Int32	currPresetCycling	= 0;
	Int32	nPresetCycling		= nPresetSeqMode;

	iacfGetCFGPscSeqMode (m_nCtrlDeviceId, &currPresetSeqMode);
	iacfGetCFGPscEnableCycling (m_nCtrlDeviceId, &currPresetCycling);

	kLog (K_MON, "[IrtvCommand::setPresetSeqMode] id(%d) : SeqMode(%d -> %d) Cycling(%d -> %d)\n", 
		m_nCtrlDeviceId, currPresetSeqMode, nPresetSeqMode, currPresetCycling, nPresetCycling);

	// 두 값을 같이 설정해 주어야 SC6000 Controller의 상태와 일치하게 동작함
	iacfSetCFGPscSeqMode (m_nCtrlDeviceId, nPresetSeqMode);
	iacfSetCFGPscEnableCycling (m_nCtrlDeviceId, nPresetCycling);

	kLog (K_MON, "[IrtvCommand::setPresetSeqMode] id(%d) : end ... \n", m_nCtrlDeviceId);

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

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.126 iacfCalStartNuc
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

#if 0
	// Set active preset to the preset we want to NUC and config preset to the preset we are configuring
	iacfSetCFGConfigPreset (m_nCtrlDeviceId, nConfigPreset);
	iacfSetCFGActivePreset (m_nCtrlDeviceId, nActivePreset);

	// Set the load NUC name.
	iacfSetCALLoadNucName (m_nCtrlDeviceId, "TestNUC");

	// Loads the NUC named by the load NUC name into the current active preset
	iacfCalLoadNucCurrentPreset (m_nCtrlDeviceId);
#else
	// Set the preset we want to load into
	iacfSetCALLoadNucPresetSlot (m_nCtrlDeviceId, nConfigPreset);

	// Set the load NUC name
	iacfSetCALLoadNucName (m_nCtrlDeviceId, (char *)pNucName);

	// Loads the NUC named by the load NUC name into the preset given by the load NUC preset slot
	iacfCalLoadNucAtPreset (m_nCtrlDeviceId);
#endif

	// 로드 후 상태 업데이트 하기
	getCameraPreset ();

	kLog (K_MON, "[IrtvCommand::loadNucAction] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.221 iacfSetFPAIntTime
// ---------------------------------------------------------------------------------------------
// Sets the FPA integration time.  
// The integration time is how long the detector should count photons.
// ---------------------------------------------------------------------------------------------
// 특정 Preset을 지정하기 위하여, FPAEx.h의 함수 iacfSetIntegrationTime()를 사용함 
// ---------------------------------------------------------------------------------------------
int IrtvCommand::setFPAIntTime (const int nPreset, const double fFPAIntTime)
{
	kLog (K_MON, "[IrtvCommand::setFPAIntTime] nPreset(%d) IntTime(%f)\n", nPreset, fFPAIntTime);

	UInt32	currFPAIntTime	= 0;
	UInt32	nCurrConfigPreset;

	iacfGetCFGConfigPreset (m_nCtrlDeviceId, &nCurrConfigPreset);

	iacfSetCFGConfigPreset (m_nCtrlDeviceId, nPreset);
	iacfGetFPAIntTime (m_nCtrlDeviceId, &currFPAIntTime);

	kLog (K_MON, "[IrtvCommand::setFPAIntTime] id(%d) : Preset(%d) IntTime(%f -> %f)\n", 
		m_nCtrlDeviceId, nPreset, currFPAIntTime * 0.001, fFPAIntTime);

	// 약 1초 이상이 소요됨
	iacfSetFPAIntTime (m_nCtrlDeviceId, (int)(fFPAIntTime * 1000));

	iacfSetCFGConfigPreset (m_nCtrlDeviceId, nCurrConfigPreset);

	kLog (K_MON, "[IrtvCommand::setFPAIntTime] id(%d) : end ... \n", m_nCtrlDeviceId);

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
	IrtvCamFile	nucFS (m_nCtrlDeviceId, "nuc", ".nuc");

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
	kLog (K_MON, "[IrtvCommand::getStoredNUCs] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

extern "C" int irtvSetFrameRate (const float fFrameRate)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int	ret	= cmdObj.setFrameRate (fFrameRate);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetROI (const short nOffsetX, const short nOffsetY, 
						   const short nWidth, const short nHeight, 
						   const short nLifeTime, const short nColor, const short nCoordi)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setROI (nOffsetX, nOffsetY, nWidth, nHeight, nLifeTime, (UInt8)nColor, (Int32)nCoordi);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetSyncSource (const int nSyncSource)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setSyncSource (nSyncSource);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSendSoftTrigger ()
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.sendSoftTrigger ();

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvSetAGCEnable (const int nAGCEnable)
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.setAGCEnable (nAGCEnable);

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" int irtvPrintDevice ()
{
	gIrtvCommandSync.lock ();

	IrtvCommand	cmdObj;
	int ret = cmdObj.printDevice ();

	gIrtvCommandSync.unlock ();

	return (ret);
}

extern "C" iacfCalib irtvGetCalibration ()
{
#if 0
	//TODO: 20121017 : User calibration을 사용할 경우, Init시 오류가 발생함. Factory Calibration을 사용할 경우에만 가져오도록 할 예정?
	if (NULL == gIrtvCalibration) {
		gIrtvCommandSync.lock ();

		IrtvCommand	cmdObj;

		gIrtvCalibration = cmdObj.getCalibration ();

		gIrtvCommandSync.unlock ();
	}
#elif 0	//TODO WORK
	if (NULL == gIrtvCalibration) {
		gIrtvCommandSync.lock ();

		IrtvCommand	cmdObj;

		gIrtvCalibration = cmdObj.getCalibrationFromFile ();

		gIrtvCommandSync.unlock ();
	}
#endif

	return (gIrtvCalibration);
}

extern "C" void irtvFreeCalibration ()
{
	if (gIrtvCalibration) {
		iacfCalibFree (gIrtvCalibration);
		gIrtvCalibration	= NULL;
	}
}

// 주의 : 이미지 취득 중에는 Command를 사용 하지 않아야 함
extern "C" int irtvGetCameraStatus (ST_IRTV *pIRTV)
{
	if (NULL == pIRTV) {
		return (NOK);
	}

	gIrtvCommandSync.lock ();

	IrtvCommand			cmdObj;
	int					ret;

	ret = cmdObj.getCameraStatus (gIrtvCamStatus);

	gIrtvCommandSync.unlock ();

	if (OK != ret) {
		pIRTV->bi_sts_conn	= 0;	// 연결 비정상
		pIRTV->bInitSetting	= 0;	// 설정 정보 업데이트 필요
		pIRTV->bInitNUCs	= 0;	// NUC 정보 업데이트 필요

		kLog (K_ERR, "[irtvGetCameraStatus] getCameraStatus() failed \n");
	}
	else {
		pIRTV->bi_sts_conn	= 1;	// 연결 정상
		pIRTV->ai_max_rate	= getMaxFrameRate ();

		// Camera에 대한 정상 연결 시, 카메라 설정값을 얻어 내부 변수 및 PV 값 설정
		if ( (0 == pIRTV->bInitSetting) || (0 == pIRTV->bInitNUCs) ) {
			gIrtvCommandSync.lock ();

			if (OK == cmdObj.getCameraSetting (gIrtvCamStatus)) {
				pIRTV->bInitSetting	= 1;	// 설정 정보 업데이트 완료
			}

			if (OK == cmdObj.getStoredNUCs ()) {
				pIRTV->bInitNUCs	= 1;	// NUC 정보 업데이트 완료
			}

			cmdObj.clear ();
			gIrtvCommandSync.unlock ();

			cmdObj.writePVs (gIrtvCamStatus);
		}
	}

	scanIoRequest (pIRTV->ioScanPvt_userCall);

	return (ret);
}

IrtvCommand::IrtvCommand ()
{
	init ();
}

IrtvCommand::~IrtvCommand ()
{
	clear ();
}

int IrtvCommand::init ()
{
	kLog (K_DEBUG, "[IrtvCommand.init] ...\n");

	m_nCtrlDeviceId	= IRTV_INVALID_ID;

	// control device id 얻기
	if (IRTV_INVALID_ID == (m_nCtrlDeviceId = getControlId ())) {
		return (NOK);
	}

	iacfConfigInit (m_nCtrlDeviceId);

	lock (m_nCtrlDeviceId);

	return (OK);
}

void IrtvCommand::clear ()
{
	if (IRTV_INVALID_ID != m_nCtrlDeviceId) {
		unLock (m_nCtrlDeviceId);

		// free IACFSDK SDK information for the given id
		iacfConfigFini(m_nCtrlDeviceId);

		CloseControl (m_nCtrlDeviceId);
	}

	m_nCtrlDeviceId	= IRTV_INVALID_ID;

}

int IrtvCommand::setFrameRate (const float fFrameRate)
{
	kLog (K_MON, "[IrtvCommand::setFrameRate] frame rate (%f) \n", fFrameRate);

	UInt32	nFrameRate		= (UInt32) (fFrameRate * 10);
	UInt32	currFrameRate	= 0;

	iacfGetFPAFrameRate (m_nCtrlDeviceId, &currFrameRate);

	kLog (K_MON, "[IrtvCommand::setFrameRate] id(%d) :%d -> %d\n", m_nCtrlDeviceId, currFrameRate, nFrameRate);

	iacfSetFPAFrameRate (m_nCtrlDeviceId, nFrameRate);

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

	kLog (K_MON, "[IrtvCommand::setFrameRate] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.167 iacfSetCFGSyncSource
// 문서 내용과 IACFCFG.h의 enum 상수이름이 일치하지 않음
// 따라서, IACFCFG.h 헤더 파일의 내용을 기준으로 함
// ---------------------------------------------------------------------------------------------
// Sets the CFG sync source.  The sync source determines where syncs/triggers come from
// ---------------------------------------------------------------------------------------------
// SYNC_INTERNAL		: Syncs are generated internally (free run mode).
// SYNC_EXTERNAL		: Syncs are from the external sync in BNC.
// SYNC_EXTERNAL_VIDEO	: Syncs are taken from the genlock in BNC.
// SYNC_EXTERNAL_CPU	: Syncs are generated by a software trigger
// ---------------------------------------------------------------------------------------------

// 참고 : 4.5 Using external sync modes
// When a cielo board is present there are additional sync modes available when the core is in external sync mode
// SYNC_EXTERNAL_OPTO_ISOLATED(4), SYNC_EXTERNAL_IRIG_TRIGGERED(5)

int IrtvCommand::setSyncSource (const int nSyncSource, const int nSyncPolarity)
{
	kLog (K_MON, "[IrtvCommand::setSyncSource] sync source(%d) polarity(%d)\n", nSyncSource, nSyncPolarity);

	Int32	currSyncSource		= 0;
	Int32	currSyncPolarity	= 0;

	iacfGetCFGSyncSource (m_nCtrlDeviceId, &currSyncSource);
	//iacfGetTriggerSource (m_nCtrlDeviceId, &currSyncSource);
	iacfGetCFGExtSyncPolarity (m_nCtrlDeviceId, &currSyncPolarity);

	kLog (K_MON, "[IrtvCommand::setSyncSource] id(%d) : sync(%d -> %d) polarity(%d -> %d) \n", 
		m_nCtrlDeviceId, currSyncSource, nSyncSource, currSyncPolarity, nSyncPolarity);

	iacfSetCFGSyncSource (m_nCtrlDeviceId, nSyncSource);
	//iacfSetTriggerSource (m_nCtrlDeviceId, nSyncSource);
	iacfSetCFGExtSyncPolarity (m_nCtrlDeviceId, nSyncPolarity);

	kLog (K_MON, "[IrtvCommand::setSyncSource] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

int IrtvCommand::sendSoftTrigger ()
{
	kLog (K_MON, "[IrtvCommand::sendSoftTrigger] software trigger (1) \n");

	iacfSetCFGTriggerActivate (m_nCtrlDeviceId, 1);

	kLog (K_MON, "[IrtvCommand::sendSoftTrigger] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

// ---------------------------------------------------------------------------------------------
// IACF SDK Documentation.chm : 3.4.30 iacfSetAGCActiveRoiData
// ---------------------------------------------------------------------------------------------
// Sets the AGC active ROI data. The active ROI data describes the active ROI.
// AGC is calculated using pixels within the active ROI
// ---------------------------------------------------------------------------------------------
// id				: An open Accessories SDK control device id.
// xOffset			: X offset (x position of the upper left corner of the ROI) to set in pixels.
// yOffset			: Y offset (y position of the upper left corner of the ROI) to set in pixels.
// Width			: Width of the ROI to set in pixels.
// Height			: Height of the ROI to set in pixels.
// coordianteMode	: Coordinate mode of the ROI to set.  May be one of the following.
//	- FPA_SPACE		: Coordinates are relative to the focal plane array and constrained by the size of the FPA.
//	- DISPLAY_SPACE : Coordinates are relative to the display and are constrained by the display size (640x480).
// Lifetime			: Lifetime of the bounding box of the ROI to set, 
//						that is the amount of time the ROI is displayed after the camera boots.  
//						Use iacfSetAGCForceRoiDisplay to always show the ROI.
// Color			: Grayscale color of the bounding box of the ROI to set (0-255).
// ---------------------------------------------------------------------------------------------

int IrtvCommand::setROI (const UInt16 nOffsetX, const UInt16 nOffsetY, 
						 const UInt16 nWidth, const UInt16 nHeight, const UInt16 nLifeTime, const UInt8 nColor, const Int32 nCoordi)
{
	kLog (K_MON, "[IrtvCommand::setROI] id(%d) : iacfConfigInit() \n", m_nCtrlDeviceId);

	UInt16	currX, currY, currWidth, currHeight, currLifeTime; 
	Int32	currCoordi;
	UInt8	currColor;

	currX = currY = currWidth = currHeight = currLifeTime = 0;
	currCoordi = currColor = 0;

#if 0
	// 값은 기록되나 FPA가 아님. AGC(?)로 보임
	iacfGetAGCActiveRoiData (m_nCtrlDeviceId, &currX, &currY, &currWidth, &currHeight, &currCoordi, &currLifeTime, &currColor);
	//iacfGetFPAWindowOffsetAndSize (m_nCtrlDeviceId, &currY, &currX, &currHeight, &currWidth);
	//iacfGetFPAWindowOrigin (m_nCtrlDeviceId, (Int16 *)&currY, (Int16 *)&currX);
	//iacfGetFPAWindowSize (m_nCtrlDeviceId, (Int16 *)&currHeight, (Int16 *)&currWidth);

	kLog (K_MON, "[IrtvCommand::setROI] get : x(%d) y(%d) width(%d) height(%d) lifetime(%d) color(%d) coordi(%d)\n", 
		currX, currY, currWidth, currHeight, currLifeTime, currColor, currCoordi);

	kLog (K_MON, "[IrtvCommand::setROI] set : x(%d) y(%d) width(%d) height(%d) lifetime(%d) color(%d) coordi(%d)\n", 
		nOffsetX, nOffsetY, nWidth, nHeight, nLifeTime, nColor, nCoordi);

	iacfSetAGCActiveRoiData (m_nCtrlDeviceId, nOffsetX, nOffsetY, nWidth, nHeight, nCoordi, nLifeTime, nColor);
	//iacfSetFPAWindowOffsetAndSize (m_nCtrlDeviceId, nOffsetY, nOffsetX, nHeight, nWidth);
	//iacfSetFPAWindowOrigin (m_nCtrlDeviceId, nOffsetY, nOffsetX);
	//iacfSetFPAWindowSize (m_nCtrlDeviceId, nHeight, nWidth);

	// always show the ROI
	//iacfSetAGCForceRoiDisplay (m_nCtrlDeviceId, 1);
#else
	iacfGetFPAWindowOrigin (m_nCtrlDeviceId, (Int16 *)&currY, (Int16 *)&currX);
	iacfGetFPAWindowSize (m_nCtrlDeviceId, (Int16 *)&currHeight, (Int16 *)&currWidth);

	kLog (K_MON, "[IrtvCommand::setROI] get : x(%d) y(%d) width(%d) height(%d) lifetime(%d) color(%d) coordi(%d)\n", 
		currX, currY, currWidth, currHeight, currLifeTime, currColor, currCoordi);

	kLog (K_MON, "[IrtvCommand::setROI] set : x(%d) y(%d) width(%d) height(%d) lifetime(%d) color(%d) coordi(%d)\n", 
		nOffsetX, nOffsetY, nWidth, nHeight, nLifeTime, nColor, nCoordi);

	iacfSetFPAWindowOrigin (m_nCtrlDeviceId, nOffsetY, nOffsetX);
	iacfSetFPAWindowSize (m_nCtrlDeviceId, nHeight, nWidth);
#endif

	kLog (K_MON, "[IrtvCommand::setSyncSource] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

int IrtvCommand::setAGCEnable (const int nAGCEnable)
{
	kLog (K_MON, "[IrtvCommand::setAGCEnable] nAGCEnable (%d) \n", nAGCEnable);

	Int32	currAGCEnable	= 0;

	iacfGetAGCEnable (m_nCtrlDeviceId, &currAGCEnable);

	if (nAGCEnable != currAGCEnable) {
		kLog (K_MON, "[IrtvCommand::setAGCEnable] id(%d) : %d -> %d \n", m_nCtrlDeviceId, currAGCEnable, nAGCEnable);

		iacfSetAGCEnable (m_nCtrlDeviceId, nAGCEnable);
	}

	kLog (K_MON, "[IrtvCommand::setAGCEnable] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (OK);
}

int IrtvCommand::copyConfigValues (IrtvCameraStatus &camStatus)
{
	kLog (K_INFO, "[IrtvCommand::copyConfigValues] set config values \n");

	ST_IRTV	*pIRTV	= getFirstDriver();

	if (pIRTV) {
		pIRTV->ai_sts_fpa_temp			= gIrtvCamStatus.m_fpaTemp * 0.001;
		pIRTV->ai_sts_preset			= gIrtvCamStatus.m_activePreset;
		pIRTV->ai_sts_int_time			= gIrtvCamStatus.m_intTime * 0.001;
		pIRTV->ai_sts_x					= gIrtvCamStatus.m_xOffset;
		pIRTV->ai_sts_y					= gIrtvCamStatus.m_yOffset;
		pIRTV->ai_sts_width				= gIrtvCamStatus.m_width;
		pIRTV->ai_sts_height			= gIrtvCamStatus.m_height;
		pIRTV->ai_sts_frame_rate		= gIrtvCamStatus.m_frameRate * 0.1;
		pIRTV->mbbi_sts_sync_src		= gIrtvCamStatus.m_syncSource;
		pIRTV->mbbi_sts_sync_polarity	= gIrtvCamStatus.m_syncPolarity;	// 0 :Falling, 1 : Rising

		pIRTV->ai_active_preset_num		= gIrtvCamStatus.m_activePreset;
		pIRTV->ai_config_preset_num		= gIrtvCamStatus.m_configPreset;
		pIRTV->bi_preset_cycling		= gIrtvCamStatus.m_presetCycling;

		//if (!pIRTV->bInitSetting) {
		if (1) {
			pIRTV->bo_invert_enable			= gIrtvCamStatus.m_rowReadoutOrder;
			pIRTV->bo_revert_enable			= gIrtvCamStatus.m_colReadoutOrder;
			pIRTV->bo_cooler_on				= gIrtvCamStatus.m_coolerOn;
			pIRTV->ao_fpa_off_temp			= gIrtvCamStatus.m_autoTurnOffTemp * 0.001;
			pIRTV->ao_fpa_on_temp			= gIrtvCamStatus.m_autoTurnOnTemp * 0.001;
			pIRTV->bo_full_nuc_enable		= gIrtvCamStatus.m_fullNucEnable;
			pIRTV->bo_preset_seq_mode		= gIrtvCamStatus.m_presetSeqMode;
			pIRTV->ao_sel_preset_num		= gIrtvCamStatus.m_configPreset;

			pIRTV->sample_rate				= pIRTV->ai_sts_frame_rate;
			pIRTV->bo_sync_polarity			= pIRTV->mbbi_sts_sync_polarity;
		}

		for (int i = 0; i < IRTV_PRESET_NUM; i++) {
			pIRTV->ai_preset_rate[i]		= gIrtvCamStatus.m_presetFrameRate[i] * 0.1;
			pIRTV->ai_preset_gain[i]		= gIrtvCamStatus.m_presetGain[i];
			pIRTV->ao_preset_int_time[i]	= gIrtvCamStatus.m_presetIntTime[i] * 0.001;
			pIRTV->ai_preset_win_x[i]		= gIrtvCamStatus.m_presetXOffset[i];
			pIRTV->ai_preset_win_y[i]		= gIrtvCamStatus.m_presetYOffset[i];
			pIRTV->ai_preset_win_width[i]	= gIrtvCamStatus.m_presetWidth[i];
			pIRTV->ai_preset_win_height[i]	= gIrtvCamStatus.m_presetHeight[i];
			strcpy (pIRTV->si_preset_nuc_name[i], gIrtvCamStatus.m_presetNucName[i]);
		}
	}

	kLog (K_DEBUG, "[IrtvCommand::copyConfigValues] end ... \n");

	return (OK);
}

int IrtvCommand::getCameraStatus (IrtvCameraStatus &camStatus)
{
	kLog (K_DEBUG, "[IrtvCommand::getCameraStatus] get cameara status \n");

	if (errOk != iacfGetCLRTempActual (m_nCtrlDeviceId, &camStatus.m_fpaTemp)) { return (NOK);}
	if (errOk != iacfGetCFGSyncSource (m_nCtrlDeviceId, &camStatus.m_syncSource)) { return (NOK); }
	if (errOk != iacfGetCFGExtSyncPolarity (m_nCtrlDeviceId, &camStatus.m_syncPolarity)) { return (NOK); }
	if (errOk != iacfGetFPAIntSyncMode (m_nCtrlDeviceId, &camStatus.m_intMode)) { return (NOK); }
	if (errOk != iacfGetCFGActivePreset (m_nCtrlDeviceId, &camStatus.m_activePreset)) { return (NOK); }
	if (errOk != iacfGetCFGConfigPreset (m_nCtrlDeviceId, &camStatus.m_configPreset)) { return (NOK); }

	if (camStatus.m_activePreset != camStatus.m_configPreset) {
		// Active Preset으로 변경
		iacfSetCFGConfigPreset (m_nCtrlDeviceId, camStatus.m_activePreset);
	}

	if (errOk != iacfGetFPAIntTime(m_nCtrlDeviceId, &camStatus.m_intTime)) { return (NOK); }
	if (errOk != iacfGetFPAGain(m_nCtrlDeviceId, &camStatus.m_gain)) { return (NOK); }
	if (errOk != iacfGetFPAFrameRate (m_nCtrlDeviceId, &camStatus.m_frameRate)) { return (NOK); }

	if (errOk != iacfGetFPAWindowOrigin (m_nCtrlDeviceId, &camStatus.m_yOffset, &camStatus.m_xOffset)) { return (NOK); }
	if (errOk != iacfGetFPAWindowSize (m_nCtrlDeviceId, &camStatus.m_height, &camStatus.m_width)) { return (NOK); }

#if 0	// TODO : ROI?
	UInt16	xOffset, yOffset, width, height;

	if (errOk != iacfGetAGCActiveRoiData (m_nCtrlDeviceId, &xOffset, &yOffset, &width, &height, 
			&camStatus.m_coordi, &camStatus.m_lifeTime, &camStatus.m_color)) {
		return (NOK);
	}
#endif

	if (errOk != iacfGetFPARowReadoutOrder (m_nCtrlDeviceId, &camStatus.m_rowReadoutOrder)) { return (NOK); }
	if (errOk != iacfGetFPAColReadoutOrder (m_nCtrlDeviceId, &camStatus.m_colReadoutOrder)) { return (NOK); }
	if (errOk != iacfGetCLRCoolerState (m_nCtrlDeviceId, &camStatus.m_coolerOn)) { return (NOK); }
	if (errOk != iacfGetCLRFpaAutoTurnOffTempLimit (m_nCtrlDeviceId, &camStatus.m_autoTurnOffTemp)) { return (NOK); }
	if (errOk != iacfGetCLRFpaAutoTurnOnTempLimit (m_nCtrlDeviceId, &camStatus.m_autoTurnOnTemp)) { return (NOK); }
	if (errOk != iacfGetCFGFullWindowNuc (m_nCtrlDeviceId, &camStatus.m_fullNucEnable)) { return (NOK); }
	if (errOk != iacfGetCFGPscSeqMode (m_nCtrlDeviceId, &camStatus.m_presetSeqMode)) { return (NOK); }

	// Config Preset을 원래대로 설정
	iacfSetCFGConfigPreset (m_nCtrlDeviceId, camStatus.m_configPreset);

	copyConfigValues (camStatus);

	camStatus.printStatus ();

	kLog (K_DEBUG, "[IrtvCommand::getCameraStatus] end ... \n");

	return (OK);
}

int IrtvCommand::getCameraSetting (IrtvCameraStatus &camStatus)
{
	kLog (K_MON, "[IrtvCommand::getCameraSetting] get cameara settings \n");

#if 0
	if (errOk != iacfGetFPARowReadoutOrder (m_nCtrlDeviceId, &camStatus.m_rowReadoutOrder)) { return (NOK); }
	if (errOk != iacfGetFPAColReadoutOrder (m_nCtrlDeviceId, &camStatus.m_colReadoutOrder)) { return (NOK); }
	if (errOk != iacfGetCLRCoolerState (m_nCtrlDeviceId, &camStatus.m_coolerOn)) { return (NOK); }
	if (errOk != iacfGetCLRFpaAutoTurnOffTempLimit (m_nCtrlDeviceId, &camStatus.m_autoTurnOffTemp)) { return (NOK); }
	if (errOk != iacfGetCLRFpaAutoTurnOnTempLimit (m_nCtrlDeviceId, &camStatus.m_autoTurnOnTemp)) { return (NOK); }
	if (errOk != iacfGetCFGActivePreset (m_nCtrlDeviceId, &camStatus.m_activePreset)) { return (NOK); }
	if (errOk != iacfGetCFGConfigPreset (m_nCtrlDeviceId, &camStatus.m_configPreset)) { return (NOK); }
	if (errOk != iacfGetCFGFullWindowNuc (m_nCtrlDeviceId, &camStatus.m_fullNucEnable)) { return (NOK); }
	if (errOk != iacfGetCFGPscSeqMode (m_nCtrlDeviceId, &camStatus.m_presetSeqMode)) { return (NOK); }
#endif

	if (errOk != iacfGetCFGPscEnableCycling (m_nCtrlDeviceId, &camStatus.m_presetCycling)) { return (NOK); }

	getCameraPreset (camStatus);

	copyConfigValues (camStatus);

	camStatus.printSetting ();

	kLog (K_MON, "[IrtvCommand::getCameraSetting] end ... \n");

	return (OK);
}

int IrtvCommand::writePVs (IrtvCameraStatus &camStatus)
{
	// Output PV의 값 설정
	retryCaputByFloat (sfwEnvGet("PV_NAME_SAMPLING_RATE"), camStatus.m_frameRate * 0.1);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_X"), camStatus.m_xOffset);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_Y"), camStatus.m_yOffset);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_WIDTH"), camStatus.m_width);
	retryCaputByInt (sfwEnvGet("PV_NAME_ROI_HEIGHT"), camStatus.m_height);
	//retryCaputByInt (sfwEnvGet("IRTV_ROI_LIFETIME"), camStatus.m_lifeTime);
	//retryCaputByInt (sfwEnvGet("IRTV_ROI_COLOR"), camStatus.m_color);
	//retryCaputByInt (sfwEnvGet("IRTV_ROI_COORDI"), camStatus.m_coordi);
	retryCaputByInt (sfwEnvGet("IRTV_SYNC_SOURCE"), camStatus.m_syncSource);
	retryCaputByInt (sfwEnvGet("IRTV_SYNC_POLARITY"), camStatus.m_syncPolarity);
	retryCaputByInt (sfwEnvGet("IRTV_GAIN_LEVEL"), camStatus.m_gain);
	retryCaputByInt (sfwEnvGet("IRTV_INVERT_ENABLE"), camStatus.m_rowReadoutOrder);
	retryCaputByInt (sfwEnvGet("IRTV_REVERT_ENABLE"), camStatus.m_colReadoutOrder);
	retryCaputByInt (sfwEnvGet("IRTV_FPA_COOLER_ON"), camStatus.m_coolerOn);
	retryCaputByFloat (sfwEnvGet("IRTV_FPA_OFF_TEMP"), camStatus.m_autoTurnOffTemp * 0.001);
	retryCaputByFloat (sfwEnvGet("IRTV_FPA_ON_TEMP"), camStatus.m_autoTurnOnTemp * 0.001);
	retryCaputByInt (sfwEnvGet("IRTV_SEL_PRESET_NUM"), camStatus.m_configPreset);
	retryCaputByInt (sfwEnvGet("IRTV_PRESET_SEQ_MODE"), camStatus.m_presetSeqMode);
	retryCaputByInt (sfwEnvGet("IRTV_FULL_NUC_ENABLE"), camStatus.m_fullNucEnable);

	char	pvName[64];

	for (int i = 0; i < IRTV_PRESET_NUM; i++) {
		sprintf (pvName, "%s%d", sfwEnvGet("IRTV_FPA_INT_TIME"), i);		
		retryCaputByFloat (pvName, camStatus.m_presetIntTime[i] * 0.001);
		epicsThreadSleep (1);
	}

	return (OK);
}

int IrtvCommand::getCameraPreset ()
{
	return (getCameraPreset (gIrtvCamStatus));
}

int IrtvCommand::getCameraPreset (IrtvCameraStatus &camStatus)
{
	kLog (K_INFO, "[IrtvCommand::getCameraPrset] get cameara preset \n");

	UInt32	nCurrConfigPreset;
	iacfGetCFGConfigPreset (m_nCtrlDeviceId, &nCurrConfigPreset);

	for (int i = 0; i < IRTV_PRESET_NUM; i++) {
		iacfSetCFGConfigPreset (m_nCtrlDeviceId, i);

		if (errOk != iacfGetFPAFrameRate (m_nCtrlDeviceId, &camStatus.m_presetFrameRate[i])) { return (NOK); }
		if (errOk != iacfGetFPAGain (m_nCtrlDeviceId, &camStatus.m_presetGain[i])) { return (NOK); }
		if (errOk != iacfGetFPAIntTime (m_nCtrlDeviceId, &camStatus.m_presetIntTime[i])) { return (NOK); }
		if (errOk != iacfGetFPAWindow (m_nCtrlDeviceId, &camStatus.m_presetXOffset[i], &camStatus.m_presetYOffset[i],
						&camStatus.m_presetWidth[i], &camStatus.m_presetHeight[i])) { return (NOK); }
		if (errOk != iacfGetCALTag (m_nCtrlDeviceId, camStatus.m_presetNucName[i])) { return (NOK); }
	}

	iacfSetCFGConfigPreset (m_nCtrlDeviceId, nCurrConfigPreset);

	copyConfigValues (camStatus);

	kLog (K_DEBUG, "[IrtvCommand::getCameraSetting] end ... \n");

	return (OK);
}

iacfCalib IrtvCommand::getCalibration ()
{
	kLog (K_MON, "[IrtvCommand::getCalibration] calibration \n");

	kLog (K_MON, "[IrtvCommand::getCalibration] id(%d) iacfCalibInit \n", m_nCtrlDeviceId);

	// iacfCalibInit() 호출 시, 약 8초 소요됨
	if (iacfCalibInit(m_nCtrlDeviceId) != errOk) {
		kLog (K_ERR, "[IrtvCommand::getCalibration] iacfCalibInit FAILED\n");
		return (NULL);
	}

	bool	calibrated;

	if (iacfCalibIsFactoryCalibrated(m_nCtrlDeviceId, &calibrated) != errOk) {
		kLog (K_ERR, "[IrtvCommand::getCalibration] iacfCalibIsFactoryCalibrated FAILED\n");

		iacfCalibFini(m_nCtrlDeviceId);
		return (NULL);
	}

	iacfCalib	calib = NULL;

	if (calibrated) {
		int		nCalibIdx = 0;

		calib = iacfCalibAlloc();

		ST_IRTV	*pIRTV	= getFirstDriver();
		int		preset	= (int)pIRTV->ai_active_preset_num;
		
		kLog (K_MON, "[IrtvCommand::getCalibration] iacfCalibGetActiveCalibration(%d) \n", preset);

		if (iacfCalibGetActiveCalibration (m_nCtrlDeviceId, preset, &nCalibIdx) != errOk) {
			kLog (K_ERR, "[IrtvCommand::getCalibration] iacfCalibGetActiveCalibration FAILED\n");

			iacfCalibFree(calib);
			iacfCalibFini(m_nCtrlDeviceId);
			return (NULL);
		}

		kLog (K_INFO, "[IrtvCommand::getCalibration] calib %d\n", nCalibIdx);

		if (iacfCalibGetCalibration (m_nCtrlDeviceId, nCalibIdx, calib) != errOk) {
			kLog (K_ERR, "[IrtvCommand::getCalibration] iacfCalibGetCalibration FAILED\n");

			iacfCalibFree(calib);
			iacfCalibFini(m_nCtrlDeviceId);
			return (NULL);
		}
	}

	iacfCalibFini(m_nCtrlDeviceId);

	kLog (K_MON, "[IrtvCommand::getCalibration] id(%d) : end ... \n", m_nCtrlDeviceId);

	return (calib);
}

iacfCalib IrtvCommand::getCalibrationFromFile ()
{
	kLog (K_MON, "[IrtvCommand::getCalibrationFromFile] calibration \n");

	iacfCalib	calib		= NULL;
	FILE		*fpCalib	= NULL;

	if (NULL == (fpCalib = fopen (IRTV_CALIB_FILE_NAME, "rb"))) {
		kLog (K_ERR, "[IrtvCommand::getCalibrationFromFile] Can't open calib file (%s)\n", IRTV_CALIB_FILE_NAME);
		return (NULL);
	}

	calib = iacfCalibAlloc ();

	if (1 != fread (calib, iacfCalibSize(), 1, fpCalib)) {
		kLog (K_ERR, "[IrtvCommand::getCalibrationFromFile] Can't read calib from file (%s)\n", IRTV_CALIB_FILE_NAME);
		iacfCalibFree(calib);
		calib	= NULL;
	}

	fclose (fpCalib);

	kLog (K_MON, "[IrtvCommand::getCalibrationFromFile] end ... \n");

	return (calib);
}

int IrtvCommand::printDevice ()
{
#if 0
	BSTR	VIF[ISC_MAX_INTERFACES];
	BSTR	devices[ISC_MAX_INTERFACES][ISC_MAX_DEVICES];
	int		i, j;

	kLog (K_INFO, "[IrtvCommand::printDevice] ... \n");

	for (i = 0; GetControlIF (&VIF[i], i) == eOK; i++) {
		kLog (K_INFO, "%s \n", ConvertBSTRToString(VIF[i]));

		for (j = 0; GetIFDevice(VIF[i],&devices[i][j],j) == eOK; j++) {
			kLog (K_INFO, "    -> %s \n", ConvertBSTRToString(devices[i][j]));
			SysFreeString (devices[i][j]);
		}
		SysFreeString (VIF[i]);
	}

	for (i = 0; GetVideoIF (&VIF[i], i) == eOK; i++) {
		kLog (K_INFO, "%s \n", ConvertBSTRToString(VIF[i]));

		for (j = 0; GetIFDevice(VIF[i],&devices[i][j],j) == eOK; j++) {
			kLog (K_INFO, "    -> %s \n", ConvertBSTRToString(devices[i][j]));
		}
	}
#endif

	BSTR    interfaceName;
	BSTR    deviceName;
	int     iInterface;
	int     iDevice;

	kLog (K_INFO, "------------------------------------------------ \n");
	kLog (K_INFO, "[IrtvCommand::printDevice] Control Interface ... \n");

	iInterface = 0;

	while (GetControlIF (&interfaceName, iInterface) == eOK) {
		kLog (K_INFO, "%s \n", ConvertBSTRToString(interfaceName));

		iDevice = 0;

		while (GetIFDevice (interfaceName, &deviceName, iDevice) == eOK) {
			kLog (K_INFO, "    -> %s \n", ConvertBSTRToString(deviceName));
			SysFreeString(deviceName);
			++iDevice;
		}

		++iInterface;
		SysFreeString(interfaceName);
	}

	kLog (K_INFO, "[IrtvCommand::printDevice] Video Interface ... \n");

	iInterface = 0;

	while (GetVideoIF (&interfaceName, iInterface) == eOK) {
		kLog (K_INFO, "%s \n", ConvertBSTRToString(interfaceName));

		iDevice = 0;

		while (GetIFDevice (interfaceName, &deviceName, iDevice) == eOK) {
			kLog (K_INFO, "    -> %s \n", ConvertBSTRToString(deviceName));
			SysFreeString(deviceName);
			++iDevice;
		}

		++iInterface;
		SysFreeString(interfaceName);
	}

	kLog (K_INFO, "------------------------------------------------ \n");

	return (OK);
}

void IrtvCommand::searchDevice ()
{
	BSTR    interfaceName;
	BSTR    deviceName;
	int     iInterface;
	int     iDevice;

	iInterface = 0;

	while (GetControlIF(&interfaceName, iInterface) == eOK) {
		iDevice = 0;

		while(GetIFDevice(interfaceName, &deviceName, iDevice) == eOK) {
			wprintf(L"%s:%s\n", interfaceName, deviceName);
			SysFreeString(deviceName);
			++iDevice;
		}

		++iInterface;
		SysFreeString(interfaceName);
	}
}

const int IrtvCommand::getInterface (const int iInterface, wchar_t *output, int max)
{
	BSTR    interfaceName;

	if (eOK != GetControlIF (&interfaceName, iInterface)) {
		return (NOK);
	}

	if ((int)wcslen(interfaceName) >= max) {
		wcsncpy(output, interfaceName, max-1);
		output[max-1] = 0;
	}
	else {
		wcscpy(output, interfaceName);
	}

	SysFreeString(interfaceName);

	return (OK);
}

const int IrtvCommand::getDevice (const int iDevice, const wchar_t *ifname, wchar_t *output, int max)
{
	BSTR		deviceName = NULL;
	isc_Error	ret;

	if (eOK != (ret = GetIFDevice ((const LPWSTR)ifname, &deviceName, iDevice))) {
		kLog (K_ERR, "[getDevice] GetIFDevice failed (%d) \n", ret);
		return (NOK);
	}

	if ((int)wcslen(deviceName) >= max) {
		wcsncpy(output, deviceName, max-1);
		output[max-1] = 0;
	}
	else {
		wcscpy(output, deviceName);
	}

	SysFreeString(deviceName);

	return (OK);
}

int IrtvCommand::getControlId (wchar_t *wcInterfaceName, wchar_t *wcDeviceName)
{
	int			id	= IRTV_INVALID_ID;
	isc_Error	ret;

	kLog (K_INFO, "[IrtvCommand::getControlId] Indigo Control and Command Interfaces \n");

	if (IRTV_INVALID_ID == getDevice (0, wcInterfaceName, wcDeviceName, 256)) {
		kLog (K_ERR, "[getControlId] getDevice failed \n");
		return (IRTV_INVALID_ID);
	}

	kLog (K_INFO, "[IrtvCommand::getControlId] OpenControl() \n");

	if (eOK != (ret = OpenControl (wcInterfaceName, wcDeviceName, &id))) {
		kLog (K_ERR, "[IrtvCommand::getControlId] OpenControl() failed. id(%d) ret(%d)\n", id, ret);

		if (eAlreadyOpen == ret) {
			CloseControl(0);
		}

		return (IRTV_INVALID_ID);
	}

	if (true != isReady (id)) {
		kLog (K_ERR, "[IrtvCommand::getControlId] OpenControl() failed : not ready ...\n");

		CloseControl (id);
		return (IRTV_INVALID_ID);
	}

	kLog (K_INFO, "[IrtvCommand::getControlId] id(%d) \n", id);

	return (id);
}

int IrtvCommand::getVideoId (wchar_t *wcInterfaceName, wchar_t *wcDeviceName)
{
	int			id	= IRTV_INVALID_ID;
	isc_Error	err;

	kLog (K_INFO, "[IrtvCommand::getVideoId] Indigo Video Interfaces \n");

	if (IRTV_INVALID_ID == getDevice (0, wcInterfaceName, wcDeviceName, 256)) {
		return (IRTV_INVALID_ID);
	}

	kLog (K_INFO, "[IrtvCommand::getVideoId] OpenVideo() \n");

	if (eOK != (err = OpenVideo (wcInterfaceName, wcDeviceName, &id))) {
		kLog (K_ERR, "[IrtvCommand::getVideoId] OpenVideo() failed. ret(%d)\n", err);
		return (IRTV_INVALID_ID);
	}

	kLog (K_INFO, "[IrtvCommand::getVideoId] id(%d) \n", id);

	return (id);
}

int IrtvCommand::getControlId ()
{
	int			id;
	wchar_t		wcInterfaceName[256];
	wchar_t		wcDeviceName[256];

	wcscpy (wcInterfaceName, IRTV_CAM_INTERFACE_NAME);
	//wcscpy (wcDeviceName, L"[192.168.2.254]: (HighPerf)");

	if ( IRTV_INVALID_ID == (id = getControlId (wcInterfaceName, wcDeviceName)) ) {
		RefreshDeviceList (wcInterfaceName);
	}
	else {
		// baud rate 설정
		SetBaudRate (id, IRTV_CONTROL_BAUD_RATE);
	}

	return (id);
}

int IrtvCommand::getVideoId ()
{
	int			id;
	wchar_t		wcInterfaceName[256];
	wchar_t		wcDeviceName[256];

	wcscpy (wcInterfaceName, IRTV_CAM_INTERFACE_NAME);
	//wcscpy (wcDeviceName, L"[192.168.2.254]: (HighPerf)");

	if ( IRTV_INVALID_ID == (id = getVideoId (wcInterfaceName, wcDeviceName)) ) {
		RefreshDeviceList (wcInterfaceName);
	}

	return (id);
}

// before querying or modifying the camera configuration the application should wait for the camera to become ready 
bool IrtvCommand::isReady (const int nDeviceId)
{
	if (IRTV_INVALID_ID != nDeviceId) {
		char	msg[256];
		UInt16	code, maxWait;
		int		i;
		int		ret;

		iacfConfigInit (nDeviceId);

		kLog (K_INFO, "[IrtvCommand::isReady] start ... : id(%d)\n", nDeviceId);

		for (i = 0; i < IRTV_READY_WAIT_CNT; i++) {
			if (0 != (ret = iacfGetSYSCurrentStatus (nDeviceId, msg, &code, &maxWait))) {
				kLog (K_ERR, "[IrtvCommand::isReady] iacfGetSYSCurrentStatus failed : errCode(%d)\n", ret);
				break;
			}

			if (0 == code) {
				iacfConfigFini (nDeviceId);
				return (true);
			}

			kLog (K_MON, "[IrtvCommand::isReady] retry ... : code(%d)\n", code);

			epicsThreadSleep (IRTV_READY_WAIT_TIME / 1000.0);
		}

		iacfConfigFini (nDeviceId);
	}

	return (false);
}

void IrtvCommand::lock (const int nDeviceId)
{
	// Acquires global exclusive access to the control device to the calling thread (behaves like a mutex).
	AcquireControl(nDeviceId);
}

void IrtvCommand::unLock (const int nDeviceId)
{
	// Releases global exclusive access to the control device from the calling thread (behaves like a mutex).
	ReleaseControl (nDeviceId);
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
