#ifndef _IRTV_COMMAND_H
#define _IRTV_COMMAND_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>

#include "irtvCommon.h"
#include "irtvSync.h"
#include "IrtvGrabStatus.h"

#include "Types.h"		// for IACF types : SC6000SDK\include
#include "iacfSDK.h"
#include "iacfUTIL.h"

#include <comutil.h>	// for using namespace _com_util
using namespace _com_util;

#include <iostream>
using namespace std;

const int	IRTV_INVALID_ID			= -1;		// invalid device id
const int	IRTV_READY_WAIT_CNT		= 5;		// Ready 상태 대기 회수
const int	IRTV_READY_WAIT_TIME	= 100;		// 대기 시간 (msec)
const int	IRTV_CONTROL_BAUD_RATE	= 115200;	// Control device의 baud rate
const int	IRTV_STATUS_WAIT_TIME	= 3;		// Camera 상태 대기 대기 시간 (초)
const int	IRTV_PRESET_NUM			= 4;		// Preset 개수

class IrtvCameraStatus
{
public :
	// flag whether initialization had been completed or not
	int		m_bInitOk;

	// Status bar
	UInt32	m_fpaTemp;
	//UInt32	m_preset;
	UInt32	m_activePreset;
	Int32	m_intMode;
	UInt32	m_intTime;
	UInt8	m_gain;
	UInt32	m_frameRate;
	Int32	m_syncSource;
	Int32	m_syncPolarity;
	Int16	m_xOffset;
	Int16	m_yOffset;
	Int16	m_width;
	Int16	m_height;
	UInt32	m_autoTurnOffTemp;
	UInt32	m_autoTurnOnTemp;

	// ROI
	//Int32	m_coordi;
	//UInt16	m_lifeTime;
	//UInt8	m_color;

	// Settings
	Int32	m_rowReadoutOrder;
	Int32	m_colReadoutOrder;
	Int32	m_coolerOn;
	UInt32	m_configPreset;
	Int32	m_fullNucEnable;
	Int32	m_presetSeqMode;
	Int32	m_presetCycling;

	UInt32	m_presetFrameRate[IRTV_PRESET_NUM];
	UInt8	m_presetGain[IRTV_PRESET_NUM];
	UInt32	m_presetIntTime[IRTV_PRESET_NUM];
	Int16	m_presetXOffset[IRTV_PRESET_NUM];
	Int16	m_presetYOffset[IRTV_PRESET_NUM];
	Int16	m_presetWidth[IRTV_PRESET_NUM];
	Int16	m_presetHeight[IRTV_PRESET_NUM];
	char	m_presetNucName[IRTV_PRESET_NUM][64];

	void printStatus () {
		kLog (K_MON, "XYWH(%d/%d/%d/%d) K(%.2f) P(%d/%d) R(%d) I(%.3f,%d) S(%d,%d)\n", 
			m_xOffset, m_yOffset, m_width, m_height, m_fpaTemp * 0.001, 
			m_activePreset, m_configPreset, m_frameRate / 10, m_intTime * 0.001, m_intMode, m_syncSource, m_syncPolarity);
	}

	void printSetting () {
		//kLog (K_MON, "coordi(%d) lifeTime(%d) color(%d)\n", m_coordi, m_lifeTime, m_color);
		kLog (K_MON, "row(%d) col(%d) cooler(%d)\n", m_rowReadoutOrder, m_colReadoutOrder, m_coolerOn);
		kLog (K_MON, "active(%d) config(%d) fullNuc(%d) seqMode(%d) cycling(%d)\n", m_activePreset, m_configPreset, m_fullNucEnable, m_presetSeqMode, m_presetCycling);
		kLog (K_MON, "intTime(%d/%d/%d/%d)\n", m_presetIntTime[0], m_presetIntTime[1], m_presetIntTime[2], m_presetIntTime[3]);
	}

	void print () {
		printStatus ();
		printSetting ();
	}
};

class IrtvCommand
{
public :
	IrtvCommand ();

	~IrtvCommand ();

	int init ();
	void clear ();

	int setGainLevel (const int nGainLevel);
	int setInvertEnable (const int bEnable);
	int setRevertEnable (const int bEnable);
	int setSyncPolarity (const int nSyncPolarity);
	int setCoolerOn (const int bCoolerOn);
	int setCoolerType (const int nCoolerType);
	int setCoolerAuto (const int nCoolerAuto);
	int setFPAAutoTurnOffTemp (const int nTurnOffTemp);
	int setFPAAutoTurnOnTemp (const int nTurnOnTemp);
	int setActivePreset (const int nActivePreset);
	int setConfigPreset (const int nConfigPreset);
	int setPresetSeqMode (const int nPresetSeqMode);
	int setFullNucEnable (const int bEnable);
	int setNucActionType (const int nNucActionType);
	int loadNucAction (const char *pNucName, int nConfigPreset, int nActivePreset);
	int setFPAIntTime (const int nPreset, const double fFPAIntTime);

	int setFrameRate (const float fFrameRate);
	int setSyncSource (const int nSyncSource, const int nSyncPolarity = POLARITY_RISING_EDGE);
	int sendSoftTrigger ();
	int setAGCEnable (const int nAGCEnable);

	int setROI (const UInt16 nOffsetX, const UInt16 nOffsetY, 
		const UInt16 nWidth, const UInt16 nHeight, 
		const UInt16 nLifeTime, const UInt8 nColor, const Int32 nCoordi);

	int getCameraStatus (IrtvCameraStatus &camStatus);
	int getCameraSetting (IrtvCameraStatus &camStatus);
	int getStoredNUCs ();
	int getCameraPreset ();
	int getCameraPreset (IrtvCameraStatus &camStatus);
	int copyConfigValues (IrtvCameraStatus &camStatus);
	int writePVs (IrtvCameraStatus &camStatus);

	iacfCalib getCalibration ();
	iacfCalib getCalibrationFromFile ();

	int printDevice ();

	static int getControlId ();
	static int getVideoId ();

private :

	static void searchDevice ();
	static const int getDevice (const int iDevice, const wchar_t *ifname, wchar_t *output, int max);
	static const int getInterface (const int iInterface, wchar_t *output, int max);

	static int getControlId (wchar_t *wcInterfaceName, wchar_t *wcDeviceName);
	static int getVideoId (wchar_t *wcInterfaceName, wchar_t *wcDeviceName);

private :

	void lock (const int nDeviceId);
	void unLock (const int nDeviceId);

	static bool isReady (const int nDeviceId);

private :
	int		m_nCtrlDeviceId;

};

#endif // _IRTV_COMMAND_H
