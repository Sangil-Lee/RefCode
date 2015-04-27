#include <stdio.h>
#include <epicsThread.h>

#include "irtvBHP.h"
#include "irtvCommon.h"
#include "irtvGrabStatus.h"
#include "irtvCamStatus.h"
#include "irtvCamInterface.h"
#include "irtvCommand.h"

void threadFunc_StatusThr (void *param)
{
	ST_IRTV		*pIRTV	= (ST_IRTV *)param;

	// 옵션 : IOC 초기화 하는 동안 일정 시간 대기
	epicsThreadSleep (5);

	kLog (K_MON, "[threadFunc_StatusThr] start ... \n");

	while (true) {
		epicsThreadSleep (IRTV_STATUS_WAIT_TIME);

		kLog (K_DEBUG, "[threadFunc_StatusThr] run ... \n");

		if (1 != pIRTV->bo_conn_enable) {
			// close connection
			if (true == IrtvCommand::getInstance().isConnected ()) {
				kLog (K_MON, "[threadFunc_StatusThr] close connection ... \n");
				
				IrtvCommand::getInstance().disConnectCamera ();

				irtvSetCameraDisconnect (pIRTV);
			}

			kLog (K_MON, "[threadFunc_StatusThr] connection is not enabled by operator ...\n");
			continue;
		}

		if (true != IrtvCommand::getInstance().connectCamera ()) {
			// connection failed
			continue;
		}

		// Status 얻기와 이미지 취득 쓰레드와의 록
		epicsMutexLock (pIRTV->grabVideoLock);
		
		// Command를 이용하여 Camera 상태 얻기
		if (OK == irtvGetCameraStatus (pIRTV)) {
			// 시간이 오래 소요되는 관계로 초기에 호출
			irtvGetCalibration ();
		}
		else {
			// camera의 전원을 원격에서 껐을 경우에도 camera.IsConnected()는 true를 반환함
			// 이에 따라, Camera 상태를 얻지 못하면 명시적으로 연결을 종료해 줌
			IrtvCommand::getInstance().disConnectCamera ();
		}

		// 록 해제
		epicsMutexUnlock (pIRTV->grabVideoLock);
	}
}

extern "C" int createStatusThr (void *pIRTV)
{
	epicsThreadCreate ("IRTV.StatusThr", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) threadFunc_StatusThr,
			(void*)pIRTV);

	return 1;
}
