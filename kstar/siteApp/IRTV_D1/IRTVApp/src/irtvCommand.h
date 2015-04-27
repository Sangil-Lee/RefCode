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

#include "irtvBHP.h"

#include <iostream>
using namespace std;

const int	IRTV_INVALID_ID			= -1;		// invalid device id
const int	IRTV_READY_WAIT_CNT		= 5;		// Ready 상태 대기 회수
const int	IRTV_READY_WAIT_TIME	= 100;		// 대기 시간 (msec)
const int	IRTV_CONTROL_BAUD_RATE	= 115200;	// Control device의 baud rate
const int	IRTV_PRESET_NUM			= 4;		// Preset 개수

const int	IRTV_GRAP_TIMEOUT_MSEC	= 20000;	// 타임아웃 (msec)
const int	IRTV_GRAP_TIMEOUT_CNT	= 5;		// 타임아웃 허용회수

#define	errOk	reserrOk
#if !defined(BSTR)
#define BSTR	wchar_t *
#endif

class IrtvCameraStatus
{
public :
	// flag whether initialization had been completed or not
	int		m_bInitOk;

	// Status bar
	Flt32	m_fpaTemp;
	Flt32	m_frontPannelTemp;
	Flt32	m_airGapTemp;
	Flt32	m_internalTemp;
	//UInt32	m_preset;
	UInt8	m_activePreset;
	UInt8	m_intMode;
	Flt64	m_intTime;
	UInt8	m_gain;
	Flt64	m_frameRate;
	UInt8	m_syncSource;
	UInt8	m_triggerMode;
	UInt8	m_syncPolarity;
	UInt16	m_xOffset;
	UInt16	m_yOffset;
	UInt16	m_width;
	UInt16	m_height;
	UInt32	m_autoTurnOffTemp;
	UInt32	m_autoTurnOnTemp;

	// ROI
	//Int32	m_coordi;
	//UInt16	m_lifeTime;
	//UInt8	m_color;

	// Settings
	bool	m_rowReadoutOrder;
	bool	m_colReadoutOrder;
	bool	m_coolerOn;
	UInt8	m_configPreset;	//TODO : ActivePreset과 동일(?)
	Int32	m_fullNucEnable;
	UInt8	m_presetSeqMode;
	Int32	m_presetCycling;

	Flt64	m_presetFrameRate[IRTV_PRESET_NUM];
	UInt8	m_presetGain[IRTV_PRESET_NUM];
	Flt64	m_presetIntTime[IRTV_PRESET_NUM];
	UInt16	m_presetXOffset[IRTV_PRESET_NUM];
	UInt16	m_presetYOffset[IRTV_PRESET_NUM];
	UInt16	m_presetWidth[IRTV_PRESET_NUM];
	UInt16	m_presetHeight[IRTV_PRESET_NUM];
	char	m_presetNucName[IRTV_PRESET_NUM][64];

	void printStatus () {
		kLog (K_INFO, "XYWH(%d/%d/%d/%d) K(%.2f) P(%d/%d) R(%f) I(%.3f,%d) S(%d,%d)\n", 
			m_xOffset, m_yOffset, m_width, m_height, m_fpaTemp, 
			m_activePreset, m_configPreset, m_frameRate, m_intTime, m_intMode, m_syncSource, m_syncPolarity);
	}

	void printSetting () {
		//kLog (K_MON, "coordi(%d) lifeTime(%d) color(%d)\n", m_coordi, m_lifeTime, m_color);
		kLog (K_MON, "row(%d) col(%d) cooler(%d)\n", m_rowReadoutOrder, m_colReadoutOrder, m_coolerOn);
		kLog (K_MON, "active(%d) config(%d) fullNuc(%d) seqMode(%d) cycling(%d)\n", m_activePreset, m_configPreset, m_fullNucEnable, m_presetSeqMode, m_presetCycling);
		kLog (K_MON, "intTime(%f/%f/%f/%f)\n", m_presetIntTime[0], m_presetIntTime[1], m_presetIntTime[2], m_presetIntTime[3]);
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

	bool isConnected ();

	int setGainLevel (const int nGainLevel);
	int setInvertEnable (const int bEnable);
	int setRevertEnable (const int bEnable);
	int setSyncPolarity (const int nSyncPolarity);
	int setTriggerSeqCount (const UInt32 nSeqCount);
	int setTriggerMode (const int nTriggerMode);
	int setCoolerOn (const int bCoolerOn);
	int setCoolerType (const int nCoolerType) {return (OK);};
	int setCoolerAuto (const int nCoolerAuto) {return (OK);};
	int setFPAAutoTurnOffTemp (const int nTurnOffTemp) {return (OK);};
	int setFPAAutoTurnOnTemp (const int nTurnOnTemp) {return (OK);};
	int setActivePreset (const int nActivePreset);
	int setConfigPreset (const int nConfigPreset);
	int setPresetSeqMode (const int nPresetSeqMode);
	int setFullNucEnable (const int bEnable) {return (OK);};
	int setNucActionType (const int nNucActionType) {return (OK);};
	int loadNucAction (const char *pNucName, int nConfigPreset, int nActivePreset);
	int setFPAIntTime (const int nPreset, const double fFPAIntTime);

	int setFrameRate (const double fFrameRate);
	int setSyncSource (const int nSyncSource, const int nSyncPolarity = bhppolActiveHigh);
	int sendSoftTrigger ();
	int sendRearmTrigger ();
	int setAGCEnable (const int nAGCEnable);

	int setROI (const UInt16 nOffsetX, const UInt16 nOffsetY, 
		const UInt16 nWidth, const UInt16 nHeight, 
		const UInt16 nLifeTime, const UInt8 nColor, const Int32 nCoordi);

	int getCameraStatus (IrtvCameraStatus &camStatus);
	int getCameraSetting (IrtvCameraStatus &camStatus);
	int getCameraPreset ();
	int getCameraPreset (IrtvCameraStatus &camStatus);
	int getStoredNUCs ();

	int writePVs (IrtvCameraStatus &camStatus);

	bhpSCalInfo * getCalibration ();
	bhpSCalInfo * getCalibrationFromFile ();
	void releaseCalibration ();

private :

	void lock ();
	void unLock ();
	int getActivePreset ();

	int copyConfigValues (IrtvCameraStatus &camStatus);

	static bool isReady (const int nDeviceId) {return (OK);};

private :
	resHandle	m_resHandle;

public :
	static CCamera		camera;
	static bhpSCalInfo	*m_pCalib;
	
	CCamera & getCamera ();
	bool connectCamera ();
	void disConnectCamera ();
	bool selectCamera (SCameraInfo &info);

	static IrtvCommand & getInstance ();
};

#endif // _IRTV_COMMAND_H
