/****************************************************************************
 * kuLock.cpp
 * --------------------------------------------------------------------------
 * lock API
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.05.13  yunsw        Initial revision
 ****************************************************************************/

#include "kuStd.h"
#include "kuDebug.h"
#include "kuLock.h"

kuLock::kuLock (const char *pszLockName)
{
	m_lockName	= pszLockName ? pszLockName : "N/A";
	m_lockId	= NULL;
	m_bCreated	= kuFALSE;

	create ();
}

kuLock::~kuLock ()
{
	if (NULL != getLockId ()) {
		destroy ();
	}
}

const kuInt32 kuLock::create ()
{
	if (NULL == getLockId ()) {
		m_lockId	= epicsMutexCreate ();

		if (NULL != m_lockId) {
			m_bCreated = kuTRUE;
		}
	}

	if (NULL == getLockId ()) {
		return (kuNOK);
	}
	else {
		return (kuOK);
	}
}

const kuInt32 kuLock::destroy ()
{
	if (NULL != getLockId ()) {
		if (kuTRUE == m_bCreated) {
			epicsMutexDestroy (getLockId());
			m_lockId = NULL;
			m_bCreated = kuFALSE;
		}
	}

	return (kuOK);
}

epicsMutexId kuLock::getLockId()
{
	return (m_lockId);
}

void kuLock::setLockId(epicsMutexId id) {
	destroy ();
	m_lockId = id;
}

const kuInt32 kuLock::lock ()
{
	return (lock (getLockId(), getLockName().c_str()));
}

const kuInt32 kuLock::lock (epicsMutexId id, const char *name)
{
	if (NULL != id) {
		kuDebug (kuTRACE, "[kuLock::lock] lock for %s\n", kuStd::nullprint(name));

		epicsMutexLockStatus	status;
		status = epicsMutexLock (id);
		return (kuOK);
	}
	else {
		return (kuNOK);
	}
}

const kuInt32 kuLock::unlock ()
{
	return (unlock (getLockId(), getLockName().c_str()));
}

const kuInt32 kuLock::unlock (epicsMutexId id, const char *name)
{
	if (NULL != id) {
		kuDebug (kuTRACE, "[kuLock::unlock] unlock for %s\n", kuStd::nullprint(name));

		epicsMutexUnlock (id);
		return (kuOK);
	}
	else {
		return (kuNOK);
	}
}

// End of File
