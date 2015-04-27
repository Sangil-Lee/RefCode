/****************************************************************************
 * kuThread.cpp
 * --------------------------------------------------------------------------
 * thread API
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.08.17  yunsw        Initial revision
 ****************************************************************************/

#include "kuThread.h"

kuInt32		kuThread::m_activeCnt = 0;

void *kuThreadStartup (void *tgtObject) 
{
	kuThread	*_tgtObject = (kuThread *)tgtObject;

	if (NULL == _tgtObject) {
		return( NULL );
	}

	kuDebug (kuDEBUG, "[kuThread.kuThreadStartup] Running thread object in a new thread\n");

	void *threadResult = _tgtObject->run ();

	//delete _tgtObject;

	return (threadResult);
}

// call application cleanup function
void kuThreadCleanup (void *tgtObject) 
{
	kuDebug (kuDEBUG, "[kuThread.kuThreadCleanup] cleanup handler was called\n");

	kuThread	*_tgtObject = (kuThread *)tgtObject;

	if (NULL != _tgtObject) {
		_tgtObject->clean (NULL);
	}
}

kuThread::kuThread (const char *pszThrName)
{
	if (NULL == pszThrName) {
		strcpy (m_thrName, "N/A");
	}
	else {
		strcpy (m_thrName, pszThrName);
	}

	m_activeCnt++;

	m_tid		= 0;
	m_isAlive	= kuFALSE;
}

kuThread::~kuThread ()
{
	m_activeCnt--;
}

const kuThreadId kuThread::start ()
{
	if (NULL != (m_tid = start (m_thrName, kuThreadStartup, this))) {
		m_isAlive = kuTRUE;
	}

	return (m_tid);
}

const kuThreadId kuThread::start (char *pszThrName, void * (*thread)(void *), void *arg)
{
	kuThreadId	tid = 0;

	//pSTDdev->ST_RingBufThread.threadQueueId   = epicsMessageQueueCreate(50, pSTDdev->maxMessageSize);

	tid = epicsThreadCreate (
			pszThrName,											// name
			epicsThreadPriorityHigh,							// prioirty
			epicsThreadGetStackSize(epicsThreadStackMedium),	// stack size
			(EPICSTHREADFUNC) thread,							// thread function
			arg													// argument
	);

	return (tid);
}

const kuBoolean kuThread::stop ()
{
	if (kuTRUE != stop (m_tid)) {
		return (kuFALSE);
	}

	m_isAlive = kuFALSE;

	return (kuTRUE);
}

const kuBoolean kuThread::stop (kuThreadId threadId)
{
	//TODO

	return (kuTRUE);
}

const kuBoolean kuThread::kill (const kuInt32 sigNo)
{
	kill (m_tid, sigNo);

	return (kuTRUE);
}

const kuBoolean kuThread::kill (kuThreadId threadId, const kuInt32 sigNo)
{
	//TODO

	return (kuTRUE);
}

const int kuThread::isAlive ()
{
	return (isAlive (m_tid));
}

const int kuThread::isAlive (kuThreadId threadId)
{
	if (0 == threadId) {
		return (kuFALSE);
	}

	return (kuTRUE);
}

void *kuThread::run (void)
{
	kuDebug (kuDEBUG, "[kuThread::clean] Entered the thread for object %p\n", this);

	return (NULL);
}

void kuThread::clean (void *)
{
	kuDebug (kuDEBUG, "[kuThread::clean] Entered the thread cleanup handler for object %p\n", this);
}

// End of File
