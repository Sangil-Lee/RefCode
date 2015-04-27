#include <stdio.h>
#include <epicsThread.h>

#include "IrtvCommon.h"
#include "irtvGrabFrame.h"

#include "irtvDataService.h"

static	IrtvGrabFrame	*gpIrtvGrabFrame = NULL;

GrabFrameData		gLiveFrameData;
IrtvSync			gLiveFrameDataSync;

IrtvGrabFrameShm	gGrabFrameShm;

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

	// Status ���� �̹��� ��� ��������� ��
	epicsMutexLock (pIRTV->grabVideoLock);

	kLog (K_MON, "[threadFunc_GrabFrameThr] running grab ...\n");

	// �̹��� ����� ���� ���� �۾�
	gpIrtvGrabFrame->setup ();

	kLog (K_MON, "[threadFunc_GrabFrameThr] epicsEventWait() ...\n");

	// SYS_RUN ���
	epicsEventWait (pIRTV->threadEventId);

	if (TRUE == pIRTV->bRunGrabFrame) {
		// �̹��� ���
		gpIrtvGrabFrame->run ();

		// �ڿ� ����
		gpIrtvGrabFrame->stop ();
	}
	else {
		kLog (K_MON, "[threadFunc_GrabFrameThr] grab was canceled ...\n");

		// �ڿ� ����
		gpIrtvGrabFrame->stop ();

		releaseGrabFrame ();
	}

	// �� ����
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
		// ����� �̹��� ���� �뺸 ���
		gLiveFrameDataSync.wait ();

		kLog (K_INFO, "[threadFunc_LiveDataThr] processes live data ... \n");

		// �� ����
		gLiveFrameDataSync.lock ();

		// RAW �����͸� ���ÿ� ����
		gLiveFrameData.getFrameData (pLiveData, nLiveDataSize);

		nWidth	= gLiveFrameData.getWidth ();
		nHeight	= gLiveFrameData.getHeight ();

		// �� ����
		gLiveFrameDataSync.unlock ();

		kLog (K_INFO, "[threadFunc_LiveDataThr] width(%d) height(%d) size(%d) \n", nWidth, nHeight, nLiveDataSize);

		// RAW �����͸� �̿��Ͽ� TIFF ���� ����
		IrtvGrabFrame::saveToTIFF (pLiveData, nWidth, nHeight, false);
		IrtvGrabFrame::saveToRAW (pLiveData, nWidth, nHeight);

		kLog (K_INFO, "[threadFunc_LiveDataThr] tiff was created ... \n");

		// TIFF ������ ������ OPI�� ���� (Multicast)

		kLog (K_INFO, "[threadFunc_LiveDataThr] tiff data was sent to opis ... \n");
	}
}
#endif

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

extern "C" int generateImageFiles (void *pIRTV)
{
	if (NULL == gpIrtvGrabFrame) {
		return (NOK);
	}

	// ����� �����ͷκ��� ��� �̹��� ���� (TIFF, AVI, SEQ ��) ����
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
