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

	// �ɼ� : IOC �ʱ�ȭ �ϴ� ���� ���� �ð� ���
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

		// Status ���� �̹��� ��� ��������� ��
		epicsMutexLock (pIRTV->grabVideoLock);
		
		// Command�� �̿��Ͽ� Camera ���� ���
		if (OK == irtvGetCameraStatus (pIRTV)) {
			// �ð��� ���� �ҿ�Ǵ� ����� �ʱ⿡ ȣ��
			irtvGetCalibration ();
		}
		else {
			// camera�� ������ ���ݿ��� ���� ��쿡�� camera.IsConnected()�� true�� ��ȯ��
			// �̿� ����, Camera ���¸� ���� ���ϸ� ��������� ������ ������ ��
			IrtvCommand::getInstance().disConnectCamera ();
		}

		// �� ����
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
