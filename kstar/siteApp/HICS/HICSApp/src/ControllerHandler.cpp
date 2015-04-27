///////////////////////////////////////////////////////////
//  ControllerHandler.cpp
//  Implementation of the Class ControllerInterface
//  Created on:      09-4-2013 ���� 7:40:26
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "kutilObj.h"
#include "kupaLib.h"

#include "ControllerHandler.h"
#include "SerialImpl.h"
#include "UdpImpl.h"

#include "RfmHandler.h"

#include "drvEC1_ANT.h"

#define	EC1_KSTAR_PV_NAME	"EC1_PSPLC_STAT_KSTAR"
#define	EC1_FAULT_PV_NAME1	"EC1_PLCS_STAT_FLT"
#define EC1_FAULT_PV_NAME2	"EC1_PSPLC_SLOW_LIST"

static	ControllerInterface	*gpControllerInterface	= NULL;

const int ctrl_comm_open ()
{
	return ( gpControllerInterface ? gpControllerInterface->open () : kuNOK);
}

const int ctrl_comm_read (void * buf, const int size)
{
	return ( gpControllerInterface ? gpControllerInterface->read (buf, size) : kuNOK);
}

const int ctrl_comm_write (void * buf, const int size) 
{
	return ( gpControllerInterface ? gpControllerInterface->write (buf, size) : kuNOK);
}

const int ctrl_comm_close()
{
	return ( gpControllerInterface ? gpControllerInterface->close () : kuNOK);
}

const int ctrl_comm_init_serial (const char *pszDevName, const int iBaudRate)
{
	if (NULL == gpControllerInterface) {
		SerialImpl	*pSerialImpl	= new SerialImpl ();

		if (NULL != pSerialImpl) {
			pSerialImpl->setConfiguration (pszDevName, iBaudRate);
		}

		gpControllerInterface	= pSerialImpl;
	}

	return (ctrl_comm_open ());
}

const int ctrl_comm_init_udp (const char *pszIpAddr, const short sPortNo)
{
	if (NULL == gpControllerInterface) {
		UdpImpl	*pUdpImpl	= new UdpImpl ();

		if (NULL != pUdpImpl) {
			pUdpImpl->setConfiguration (pszIpAddr, sPortNo);
		}

		gpControllerInterface	= pUdpImpl;;
	}

	return (ctrl_comm_open ());
}

void ctrl_comm_handler (void *param)
{
	ST_STD_device	*pSTDmy		= (ST_STD_device *) param;
	ST_EC1_ANT		*pEC1_ANT	= NULL;
	kuFloat32		fKstarModeValue;
	kuFloat32		fEchFaultValue1;
	kuFloat32		fEchFaultValue2;

	//TODO : other method ???
	// waiting initialization of PV is finished
	epicsThreadSleep (3);

	kuDebug (kuINFO, "[ctrl_comm_handler] started ...\n");

	if (NULL == (pEC1_ANT = (ST_EC1_ANT *)pSTDmy->pUser)) {
		kuDebug (kuERR, "[ctrl_comm_handler] pEC1_ANT is null\n");
		return;
	}

	if (0 == strcmp (pEC1_ANT->szDbPortName, "")) {
		strcpy (pEC1_ANT->szDbPortName, SerialImpl::MODEMDEVICE);
	}

	if (0 == pEC1_ANT->iDbBaudRate) {
		pEC1_ANT->iDbBaudRate	= SerialImpl::BAUDRATE;
	}

	if (kuOK != ctrl_comm_init_serial (pEC1_ANT->szDbPortName, pEC1_ANT->iDbBaudRate)) {
		kuDebug (kuERR, "[ctrl_comm_handler] init() failed\n");
		return;
	}

	epicsThreadSleep (3);

	const int	iStatusSize	= sizeof(EC1_ANT_CTRL_STS);

	kuDebug (kuMON, "[ctrl_comm_handler] bDbSimulation(%d) dDbStsRate(%g)\n", pEC1_ANT->bDbSimulation, pEC1_ANT->dDbStsRate);

	// communication is OK
	pEC1_ANT->bDbCtrlStatus	= kuTRUE;

	while (TRUE) {
		if (kuTRUE != pEC1_ANT->bDbSimulation) {
			// reads status from controller
			if (iStatusSize != ctrl_comm_read (&pEC1_ANT->stMsgCtrlSts, sizeof(EC1_ANT_CTRL_STS))) {
				// communication was failed
				pEC1_ANT->bDbCtrlStatus	= kuFALSE;

				kuDebug (kuERR, "[ctrl_comm_handler] reading was failed from controller\n");
				epicsThreadSleep (1);
				//return;
			}
			else {
				// communication was ok
				pEC1_ANT->bDbCtrlStatus	= kuTRUE;
			}
		}
		else {
			// ---------------------------------
			// Simulation mode
			// ---------------------------------

			epicsThreadSleep (1.0 / pEC1_ANT->dDbStsRate);

			// returns the received command without any change
			pEC1_ANT->stMsgCtrlSts.count			= pEC1_ANT->stMsgPcsStsLocal.count;
			pEC1_ANT->stMsgCtrlSts.rt_mode			= pEC1_ANT->stMsgPcsStsLocal.pcs_mode;
			pEC1_ANT->stMsgCtrlSts.shutter			= 0;
			pEC1_ANT->stMsgCtrlSts.ech				= 1;
			pEC1_ANT->stMsgCtrlSts.hw_limit_up		= 0;
			pEC1_ANT->stMsgCtrlSts.hw_limit_down	= 1;
			pEC1_ANT->stMsgCtrlSts.pcs_run			= 0;

			//++pEC1_ANT->stMsgCtrlSts.count		%= 1000;
		}

		kuDebug (kuINFO, "EC1_ANT_CTRL_STS : Count(%5d) RT-Mode(%d) PcsRun(%d) Up(%d) Down(%d) Shutter(%d)\n",
				pEC1_ANT->stMsgCtrlSts.count, pEC1_ANT->stMsgCtrlSts.rt_mode,
				pEC1_ANT->stMsgCtrlSts.pcs_run,
				pEC1_ANT->stMsgCtrlSts.hw_limit_up, pEC1_ANT->stMsgCtrlSts.hw_limit_down,
				pEC1_ANT->stMsgCtrlSts.shutter);

		// makes a RFM message

		kuStd::memset (&pEC1_ANT->stMsgPcsSts, 0x00, sizeof(EC1_ANT_PCS_STS));

		pEC1_ANT->stMsgPcsSts.count			= pEC1_ANT->stMsgCtrlSts.count;
		pEC1_ANT->stMsgPcsSts.rt_mode		= pEC1_ANT->stMsgCtrlSts.rt_mode;
		pEC1_ANT->stMsgPcsSts.shutter		= pEC1_ANT->stMsgCtrlSts.shutter;
		pEC1_ANT->stMsgPcsSts.hw_limit_up	= pEC1_ANT->stMsgCtrlSts.hw_limit_up;
		pEC1_ANT->stMsgPcsSts.hw_limit_down	= pEC1_ANT->stMsgCtrlSts.hw_limit_down;

#if 0 // ECH fault is decided according to EPICS PVs in HICS 
		pEC1_ANT->stMsgPcsSts.ech			= pEC1_ANT->stMsgCtrlSts.ech;
#else
		fKstarModeValue	= kupaGetValue ((char *)EC1_KSTAR_PV_NAME);
		fEchFaultValue1	= kupaGetValue ((char *)EC1_FAULT_PV_NAME1);
		fEchFaultValue2	= kupaGetValue ((char *)EC1_FAULT_PV_NAME2);

#if 1
		pEC1_ANT->stMsgPcsSts.kstar_mode	= fKstarModeValue ? 1 : 0;
		pEC1_ANT->stMsgPcsSts.ech			= (fEchFaultValue1 || fEchFaultValue2) ? 1 : 0;
#else
//		local test without EC1.  2013. 12. 2  woong
		pEC1_ANT->stMsgPcsSts.kstar_mode	= 1;
		pEC1_ANT->stMsgPcsSts.ech			= 0;
#endif

		kuDebug (kuDEBUG, "EC1_ANT_PCS_STS  : KSTAR(%.f -> %d) ECH(%.f | %.f -> %d)\n",
				fKstarModeValue, pEC1_ANT->stMsgPcsSts.kstar_mode,
				fEchFaultValue1, fEchFaultValue2, pEC1_ANT->stMsgPcsSts.ech);
#endif

		// to be used in step 2

		pEC1_ANT->stMsgPcsSts.pcs_mode		= pEC1_ANT->stMsgPcsStsLocal.pcs_mode;
		pEC1_ANT->stMsgPcsSts.pcs_run		= pEC1_ANT->stMsgPcsStsLocal.pcs_run;
		pEC1_ANT->stMsgPcsSts.sw_limit_up	= pEC1_ANT->stMsgPcsStsLocal.sw_limit_up;
		pEC1_ANT->stMsgPcsSts.sw_limit_down	= pEC1_ANT->stMsgPcsStsLocal.sw_limit_down;

		pEC1_ANT->stMsgPcsSts.motor_on		= pEC1_ANT->stMsgPcsStsLocal.motor_on;
		pEC1_ANT->stMsgPcsSts.motor_down	= pEC1_ANT->stMsgPcsStsLocal.motor_down;
		pEC1_ANT->stMsgPcsSts.motor_up		= pEC1_ANT->stMsgPcsStsLocal.motor_up;

		kuDebug (kuDEBUG, "EC1_ANT_PCS_STS  : Count(%5d) RT-Mode(%d) PcsRun(%d) Up(%d) Down(%d) KSTAR(%d) ECH(%d) Shutter(%d)\n",
				pEC1_ANT->stMsgPcsSts.count, pEC1_ANT->stMsgPcsSts.rt_mode, pEC1_ANT->stMsgPcsSts.pcs_run,
				pEC1_ANT->stMsgPcsSts.hw_limit_up, pEC1_ANT->stMsgPcsSts.hw_limit_down,
				pEC1_ANT->stMsgPcsSts.kstar_mode, pEC1_ANT->stMsgPcsSts.ech,
				pEC1_ANT->stMsgPcsSts.shutter);

		kuDebug (kuTRACE, "EC1_ANT_PCS_STS  : Motor : On(%d) Up(%d) Down(%d)\n",
				pEC1_ANT->stMsgPcsSts.motor_on, pEC1_ANT->stMsgPcsSts.motor_up, pEC1_ANT->stMsgPcsSts.motor_down);

		if (kuOK != kstar_rfm_write (&pEC1_ANT->stMsgPcsSts, RFM_EC1_ANT_STS, sizeof(EC1_ANT_PCS_STS))) {
			kuDebug (kuERR, "[ctrl_comm_handler] cannot write status to RFM \n");
			//return;
		}
	}
}

