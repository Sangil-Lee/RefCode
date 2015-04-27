#include "kutilObj.h"
#include "kuRFM.h"
#include "RfmHandler.h"
#include "ControllerHandler.h"
#include "drvEC1_ANT.h"

#if USE_ECH_POWER_CTRL
extern "C" void ech_power_ctrl (void *param);
#endif

void kstar_rfm_handler (void *param)
{
	ST_STD_device	*pSTDmy		= (ST_STD_device *) param;
	ST_EC1_ANT		*pEC1_ANT	= NULL;
	int				bCmdEnable	= kuFALSE;		// pcs_mode && pcs_run

	kuDebug (kuTRACE, "[kstar_rfm_handler] started ...\n");

	if (NULL == (pEC1_ANT = (ST_EC1_ANT *)pSTDmy->pUser)) {
		kuDebug (kuERR, "[kstar_rfm_handler] pEC1_ANT is null\n");
		return;
	}

	if (kuOK != kstar_rfm_open ((char *)STR_RFM_DEV_NAME)) {
		kuDebug (kuERR, "[kstar_rfm_handler] open(%s) failed\n", STR_RFM_DEV_NAME);
		return;
	}

#if USE_CPU_AFFINITY_RT	
	epicsThreadId	pthreadInfo;

	pthreadInfo = epicsThreadGetIdSelf();
	kuDebug (kuTRACE, "%s: EPICS ID %p, pthreadID %lu\n", pthreadInfo->name, (void *)pthreadInfo, (unsigned long)pthreadInfo->tid);

	epicsThreadSetCPUAffinity (pthreadInfo, "5");
	epicsThreadSetPosixPriority (pthreadInfo, PRIOTY_EC1_ANT, "SCHED_FIFO");
#endif

	// TODO : waiting for serial initialization
	epicsThreadSleep (5);

	kuDebug (kuMON, "[kstar_rfm_handler] dDbCmdRate(%g)\n", pEC1_ANT->dDbCmdRate);

	// communication is OK
	pEC1_ANT->bDbRfmStatus	= kuTRUE;

#if USE_ECH_POWER_CTRL
	// initialize event for ECH Power Control thread
	pEC1_ANT->echRunEventId	= epicsEventCreate (epicsEventEmpty);

	// create thread for ECH Power Control
	epicsThreadCreate ("ech_power",
			epicsThreadPriorityLow,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) ech_power_ctrl,
			(void*)pEC1_ANT->echRunEventId);
#endif	// USE_ECH_POWER_CTRL

	while (TRUE) {
		if (0 < pEC1_ANT->dDbCmdRate) {
			epicsThreadSleep (1.0 / pEC1_ANT->dDbCmdRate);
			kuDebug (kuDEBUG, "[kstar_rfm_handler] scheduled by sleep\n");
		}
		else {
			epicsEventWait (pEC1_ANT->ctrlRunEventId);
			kuDebug (kuDEBUG, "[kstar_rfm_handler] scheduled by external clock\n");
		}

		// reads pcs command from RFM
		if (kuOK != kstar_rfm_read (&pEC1_ANT->stMsgPcsCmd, RFM_EC1_ANT_CMD, sizeof(EC1_ANT_PCS_CMD))) {
			kuDebug (kuERR, "[kstar_rfm_handler] kstar_rfm_read() failed \n");
			return;
		}

		kuDebug (kuMON, "EC1_ANT_RFM_READ : Count(%5d) PcsMode(%d) PcsRun(%d) Min/Max(%d/%d)\n",
				pEC1_ANT->stMsgPcsCmd.count, 
				pEC1_ANT->stMsgPcsCmd.pcs_mode, pEC1_ANT->stMsgPcsCmd.pcs_run, 
				pEC1_ANT->stMsgPcsCmd.count_min, pEC1_ANT->stMsgPcsCmd.count_max);

		// stores a received command to local variable
		pEC1_ANT->stMsgPcsStsLocal.count	= pEC1_ANT->stMsgPcsCmd.count;
		pEC1_ANT->stMsgPcsStsLocal.pcs_mode	= pEC1_ANT->stMsgPcsCmd.pcs_mode;
		pEC1_ANT->stMsgPcsStsLocal.pcs_run	= pEC1_ANT->stMsgPcsCmd.pcs_run;

		// makes a command message to be sent to controller
		kuStd::memset (&pEC1_ANT->stMsgCtrlCmd, 0x00, sizeof(EC1_ANT_CTRL_CMD));

		pEC1_ANT->stMsgCtrlCmd.count		= pEC1_ANT->stMsgPcsCmd.count;
		pEC1_ANT->stMsgCtrlCmd.pcs_mode		= pEC1_ANT->stMsgPcsCmd.pcs_mode;
		pEC1_ANT->stMsgCtrlCmd.pcs_run		= pEC1_ANT->stMsgPcsCmd.pcs_run;

		// step 2 related variables : these will be sent through digital output
		pEC1_ANT->stMsgCtrlCmd.motor_on		= 0;
		pEC1_ANT->stMsgCtrlCmd.motor_down	= 0;
		pEC1_ANT->stMsgCtrlCmd.motor_up		= 0;

		// Controller requires an information to stop operation when shot has been finished
		// So, PCSRT1 preserves pcs_mode(1) and pcs_run(0) during some period after shot
		if (1 == pEC1_ANT->stMsgPcsCmd.pcs_mode) {
			if (1 == pEC1_ANT->stMsgPcsCmd.pcs_run) {
				if (kuFALSE == bCmdEnable) {
					bCmdEnable	= kuTRUE;
					kuDebug (kuWARN, "EC1_ANT_CTRL_CMD : PcsRun(1) : Start ...\n");
				}
			}

			kuDebug (kuDEBUG, "EC1_ANT_CTRL_CMD : Count(%5d) PcsMode(%d) PcsRun(%d) \n",
					pEC1_ANT->stMsgCtrlCmd.count, pEC1_ANT->stMsgCtrlCmd.pcs_mode, pEC1_ANT->stMsgCtrlCmd.pcs_run);

			// sends a command to controller
			if (kuOK != ctrl_comm_write (&pEC1_ANT->stMsgCtrlCmd, sizeof(EC1_ANT_CTRL_CMD))) {
				kuDebug (kuERR, "[kstar_rfm_handler] sending a command was failed \n");
			}
		}
		else if (kuTRUE == bCmdEnable) {
			bCmdEnable	= kuFALSE;

			kuDebug (kuWARN, "EC1_ANT_CTRL_CMD : Count(%5d) PcsMode(%d) PcsRun(%d) : End !!!\n",
					pEC1_ANT->stMsgCtrlCmd.count, pEC1_ANT->stMsgCtrlCmd.pcs_mode, pEC1_ANT->stMsgCtrlCmd.pcs_run);

			// sends a command to controller
			if (kuOK != ctrl_comm_write (&pEC1_ANT->stMsgCtrlCmd, sizeof(EC1_ANT_CTRL_CMD))) {
				kuDebug (kuERR, "[kstar_rfm_handler] sending a command was failed \n");
			}
		}

#if USE_ECH_POWER_CTRL
		//kuDebug (kuTRACE, "[kstar_rfm_handler] send event for ECH Power Control...\n");

		//epicsEventSignal (pEC1_ANT->echRunEventId);
#endif	// USE_ECH_POWER_CTRL
	}
}

