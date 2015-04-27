///////////////////////////////////////////////////////////
//  kupaLib.cpp
//  Implementation of inteface with C program
//  Created on:      26-4-2013 ¿ÀÈÄ 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "kupaLib.h"
#include "kupaManager.h"
#include "kupaMDS.h"


void kupaStart ()
{
	kupaManager::getInstance()->sendStartEvent ();
}


void kupaUpdateValues ()
{
	kupaManager::getInstance()->sendUpdateEvent ();
}


void kupaSetShotInfo (int opMode, int shotNum, int samplingRate, double startTime, double duration, double delaySec)
{
	kupaSetOpMode (opMode);
	kupaSetShotNum (shotNum);
	kupaSetSamplingRate (samplingRate);
	kupaSetStartTime (startTime);
	kupaSetDuration (duration);
	kupaSetDelaySec (delaySec);
}


void kupaSetOpMode (int opMode)
{
	kupaManager::getInstance()->setOpMode (opMode);
}


void kupaSetShotNum (int shotNum)
{
	kupaManager::getInstance()->setShotNum (shotNum);
}


void kupaSetSamplingRate (int samplingRate)
{
	kupaManager::getInstance()->setSamplingRate (samplingRate);
}


void kupaSetStartTime (double startTime)
{
	kupaManager::getInstance()->setStartTime (startTime);
}


void kupaSetDuration (double duration)
{
	kupaManager::getInstance()->setDuration (duration);
}


void kupaSetDelaySec (double delaySec)
{
	kupaManager::getInstance()->setDelaySec (delaySec);
}


double kupaGetValue (char *pszPvName)
{
	string	valueStr;

	kupaManager::getInstance()->getValue (pszPvName, valueStr);

	return (atof(valueStr.c_str()));
}

void kupaSetLockId (epicsMutexId id)
{
	kupaManager::getInstance()->setLockId (id);
}

epicsMutexId kupaGetLockId ()
{
	return (kupaManager::getInstance()->getLockId());
}

int kupaMdsPut (char *szTreeName, char *szMdsAddr, int nShotNum,
			char *szTagName, char *szUnit, kuFloat64 *pValues,
			int nSamples, kuFloat64 fStartTime, kuFloat64 fSamplingRate)
{
	if (!szTreeName || !szMdsAddr || !szTagName || !pValues || (0 >= fSamplingRate)) {
		kuDebug (kuWARN, "[kupaMdsPut] input argument(s) are invalid ...\n");
		return (kuNOK);
	}

	kupaManager		*pManager	= kupaManager::getInstance();

	pManager->lock ();

	// --------------------------------
	// creates and opens tree
	// --------------------------------

	kupaMDS		mdsAccess;

	mdsAccess.set (szTreeName, szMdsAddr, "", "");
	mdsAccess.connect ();
	mdsAccess.open (nShotNum);

	// --------------------------------
	// stores data
	// --------------------------------

	kuFloat64	timeGap = 1.0 / fSamplingRate;

	// puts data to MDSplus
	mdsAccess.write (szTagName, szUnit, pValues, DTYPE_DOUBLE, nSamples, fStartTime, timeGap);

	// --------------------------------
	// closes tree
	// --------------------------------

	mdsAccess.close ();
	mdsAccess.disConnect ();

	pManager->unlock ();

	return (kuOK);
}
