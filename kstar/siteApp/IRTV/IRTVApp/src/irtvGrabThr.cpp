#include <stdio.h>
#include <epicsThread.h>

#include "IrtvCommon.h"
#include "irtvGrabFrame.h"

#include "irtvDataService.h"

static	IrtvGrabFrame	*gpIrtvGrabFrame = NULL;

GrabFrameData		gLiveFrameData;
IrtvSync			gLiveFrameDataSync;

IrtvGrabFrameShm	gGrabFrameShm;

CyResult CallbackFunction (void *aApp, CyDevice::CallbackId aId, unsigned long aTimeStamp, CyDevice::InterruptStatus aStatus)
{
	kLog (K_MON, "CallbackFunction\n");

	CyAssert( aId == CyDevice::CB_ON_INTERRUPT_GPIO );

	kLog (K_MON, "intrId(%d), mask(%d) timestamp(%d)\n", aStatus.mInterruptID, aStatus.mGPIOLUTInputs, aTimeStamp);

    return CY_RESULT_OK;
}

void releaseGrabFrame ()
{
	if (NULL != gpIrtvGrabFrame) {
		delete (gpIrtvGrabFrame);
	}

	gpIrtvGrabFrame	= NULL;
}

void threadFunc_GrabFrameThr (void *param)
{
	ST_IRTV		*pIRTV	= (ST_IRTV *)param;

	releaseGrabFrame ();

	if (NULL == gpIrtvGrabFrame) {
		gpIrtvGrabFrame = new IrtvGrabFrame (pIRTV);
	}

	if (NULL == gpIrtvGrabFrame) {
		kLog (K_ERR, "[threadFunc_GrabFrameThr] cannot create IrtvGrabFrame object\n");
		return;
	}

	gpIrtvGrabFrame->setIRTVAddr (pIRTV);

	// Status 얻기와 이미지 취득 쓰레드와의 록
	epicsMutexLock (pIRTV->grabVideoLock);

	kLog (K_MON, "[threadFunc_GrabFrameThr] running grab ...\n");

	// 이미지 취득을 위한 설정 작업
	gpIrtvGrabFrame->setup ();

	kLog (K_MON, "[threadFunc_GrabFrameThr] epicsEventWait() ...\n");

	// SYS_RUN 대기
	epicsEventWait (pIRTV->threadEventId);

	if (TRUE == pIRTV->bRunGrabFrame) {
		// 이미지 취득
		gpIrtvGrabFrame->run ();

		// 자원 해제
		gpIrtvGrabFrame->stop ();
	}
	else {
		kLog (K_MON, "[threadFunc_GrabFrameThr] grab was canceled ...\n");

		// 자원 해제
		gpIrtvGrabFrame->stop ();

		releaseGrabFrame ();
	}

	// 록 해제
	epicsMutexUnlock (pIRTV->grabVideoLock);
}

#if 0
void threadFunc_LiveDataThr (void *param)
{
	ST_IRTV			*pIRTV		= (ST_IRTV *)param;
	unsigned char	*pLiveData	= (unsigned char *) malloc(IRTV_MAX_BUF_SIZE);
	unsigned int	nLiveDataSize	= 0;
	unsigned short	nWidth	= 0;
	unsigned short	nHeight	= 0;

	if (NULL == pLiveData) {
		kLog (K_CRI, "[threadFunc_LiveDataThr] malloc failed for live data \n");
		exit (1);
	}

	kLog (K_MON, "[threadFunc_LiveDataThr] start ... \n");

	while (true) {
		// 취득한 이미지 저장 통보 대기
		gLiveFrameDataSync.wait ();

		kLog (K_INFO, "[threadFunc_LiveDataThr] processes live data ... \n");

		// 록 설정
		gLiveFrameDataSync.lock ();

		// RAW 데이터를 로컬에 복사
		gLiveFrameData.getFrameData (pLiveData, nLiveDataSize);

		nWidth	= gLiveFrameData.getWidth ();
		nHeight	= gLiveFrameData.getHeight ();

		// 록 해제
		gLiveFrameDataSync.unlock ();

		kLog (K_INFO, "[threadFunc_LiveDataThr] width(%d) height(%d) size(%d) \n", nWidth, nHeight, nLiveDataSize);

		// RAW 데이터를 이용하여 TIFF 파일 생성
		IrtvGrabFrame::saveToTIFF (pLiveData, nWidth, nHeight, false);
		IrtvGrabFrame::saveToRAW (pLiveData, nWidth, nHeight);

		kLog (K_INFO, "[threadFunc_LiveDataThr] tiff was created ... \n");

		// TIFF 파일을 임의의 OPI에 전송 (Multicast)

		kLog (K_INFO, "[threadFunc_LiveDataThr] tiff data was sent to opis ... \n");
	}
}
#endif

void threadFunc_StatusThr (void *param)
{
	ST_IRTV		*pIRTV	= (ST_IRTV *)param;

	// 옵션 : IOC 초기화 하는 동안 일정 시간 대기
	epicsThreadSleep (5);

	kLog (K_MON, "[threadFunc_StatusThr] start ... \n");

	while (true) {
		epicsThreadSleep (IRTV_STATUS_WAIT_TIME);

		// Status 얻기와 이미지 취득 쓰레드와의 록
		epicsMutexLock (pIRTV->grabVideoLock);
		
		// Command를 이용하여 Camera 상태 얻기
		if (OK == irtvGetCameraStatus (pIRTV)) {
			// 시간이 오래 소요되는 관계로 초기에 호출
			irtvGetCalibration ();
		}

		// 록 해제
		epicsMutexUnlock (pIRTV->grabVideoLock);
	}
}

extern "C" int createGrabFrameThr (void *pIRTV)
{
	epicsThreadCreate ("IRTV.GrapFrameThr", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) threadFunc_GrabFrameThr,
			(void*)pIRTV);

	return 1;
}

#if 0
extern "C" int createLiveDataThr (void *pIRTV)
{
	epicsThreadCreate ("IRTV.LiveDataThr", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) threadFunc_LiveDataThr,
			(void*)pIRTV);

	return 1;
}
#endif

extern "C" int createStatusThr (void *pIRTV)
{
	epicsThreadCreate ("IRTV.StatusThr", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) threadFunc_StatusThr,
			(void*)pIRTV);

	return 1;
}

extern "C" int generateImageFiles (void *pIRTV)
{
	if (NULL == gpIrtvGrabFrame) {
		return (NOK);
	}

	// 취득한 데이터로부터 대상 이미지 파일 (TIFF, AVI, SEQ 등) 생성
	if (OK != gpIrtvGrabFrame->generateImageFiles ()) {
		releaseGrabFrame ();
		return (NOK);
	}

	releaseGrabFrame ();

	return (OK);
}

extern "C" int createGrabFrameShm ()
{
	if (NULL == gGrabFrameShm.create ()) {
		return (NOK);
	}

	return (OK);
}
