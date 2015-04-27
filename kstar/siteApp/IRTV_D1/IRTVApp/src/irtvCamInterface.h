#ifndef _IRTV_CAM_INTERFACE_H
#define _IRTV_CAM_INTERFACE_H

#include "irtvGrabStatus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	IRTV_CAM_INTERFACE_NAME			(L"iPORT2")
#define	IRTV_CONFIG_FILE_NAME			"IrtvCameraConfig.xml"
#define	IRTV_CALIB_FILE_NAME			"IrtvUserCalib.cal"
#define	IRTV_CALIB_INC_FILE_NAME		"IrtvUserCalib.inc"

#define	IRTV_FRAME_BUF_QUEUE_SIZE	1200

// IrtvGrabFrameThr 서비스
extern int createGrabFrameThr (void *pSTDdev);
//extern int createLiveDataThr (void *pIRTV);
extern int generateImageFiles (void *pIRTV);

// IrtvCommand 서비스
extern int irtvSetROI (const short nOffsetX, const short nOffsetY, 
					   const short nWidth, const short nHeight, const short nLifeTime, const short nColor, const short nCoordi);
extern int irtvSetFrameRate (const double fFrameRate);
extern int irtvSetSyncSource (const int nSyncSource);
extern int irtvSendSoftTrigger ();
extern int irtvSendTriggerRearm ();
extern int irtvSetAGCEnable (const int nAGCEnable);

extern void irtvSetCameraDisconnect (ST_IRTV *pIRTV);
extern int irtvGetCameraStatus (ST_IRTV *pIRTV);
extern int irtvGetCameraSetting (ST_IRTV *pIRTV);

extern void *irtvGetCalibration ();
extern void irtvSetCalibration (void *calib);

extern int irtvPrintDevice ();

extern int irtvSetGainLevel (const int nGainLevel);
extern int irtvSetInvertEnable (const int bEnable);
extern int irtvSetRevertEnable (const int bEnable);
extern int irtvSetSyncPolarity (const int bEnable);
extern int irtvSetTriggerMode (const int nTriggerMode);
extern int irtvSetCoolerOn (const int bCoolerOn);
extern int irtvSetCoolerType (const int nCoolerType);
extern int irtvSetCoolerAuto (const int nCoolerAuto);
extern int irtvSetFPAAutoOffTemp (const int nCoolerOffTemp);
extern int irtvSetFPAAutoOnTemp (const int nCoolerOnTemp);
extern int irtvSetPresetSeqMode (const int bPresetSeqMode);
extern int irtvSetActivePreset (const int nActivePreset);
extern int irtvSetConfigPreset (const int nConfigPreset);
extern int irtvSetFullNucEnable (const int bFullNucEnable);
extern int irtvLoadNucAction (const char *pNucName, const int nConfigPreset, const int nActivePreset);
extern int irtvSetFPAIntTime (const int nPreset, const double fFPAIntTime);
extern int irtvGetStoredNUCs ();

extern int getCameraStatus (ST_IRTV *pIRTV);
extern int createGrabFrameShm ();

// 
extern int createListenThr (void *pIRTV);
extern int createDataSendThr (void *pIRTV);
extern int createStatusThr (void *pIRTV);

extern double getMaxFrameRate ();

#ifdef __cplusplus
}
#endif

#endif // _IRTV_CAM_INTERFACE_H
