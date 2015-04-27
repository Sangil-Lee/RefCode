/****************************************************************************
 * drvMarteShm.c
 * --------------------------------------------------------------------------
 * Driver support routine for shared memory interface with MARTe
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.04.28  yunsw        Initial revision
 ****************************************************************************/

#include <drvSup.h>

#include "drvMarteShm.h"

// returns the unique shared memory handler
MarteShmHandle* MarteShmGetHandle ()
{
	static MarteShmHandle*	pMarteShmHandle	= NULL;

	if (NULL == pMarteShmHandle) {
		if (NULL == (pMarteShmHandle = calloc (1, sizeof(MarteShmHandle)))) {
			msuDebug (msuFATAL, "[MarteShmGetHandle] cannot create MarteShmHandle\n");
		}
		else {
			pMarteShmHandle->pMarteShmReadAddr	= NULL;
			pMarteShmHandle->pMarteShmWriteAddr	= NULL;

			msuDebug (msuINFO, "[MarteShmGetHandle] handler was created\n");
		}
	}

	if (NULL == pMarteShmHandle) {
		msuDebug (msuERROR, "[MarteShmGetHandle] no valid handler\n");
	}

	return (pMarteShmHandle);
}

void drvMarteShmMain (void *arg)
{
	MarteShmHandle	*pHandle	= MarteShmGetHandle();
	if (!pHandle) { return; }

	msuDebug (msuINFO, "[drvMarteShmMain] main thread started\n");

	while (msuTRUE) {   
		msuDebug (msuDEBUG, "[drvMarteShmMain] running ...\n");

		epicsThreadSleep (pHandle->dScanInterval);

		// requestes to device support to read values of MARTe signals from shared memory
		scanIoRequest (pHandle->inScanPvt);
	}
}

char *MarteShmReadAddr ()
{
	MarteShmHandle	*pHandle	= MarteShmGetHandle();
	if (!pHandle) { return (NULL); }

	return (pHandle->pMarteShmReadAddr);
}

char *MarteShmWriteAddr ()
{
	MarteShmHandle	*pHandle	= MarteShmGetHandle();
	if (!pHandle) { return (NULL); }

	return (pHandle->pMarteShmWriteAddr);
}

int MarteShmReadByOffset (MarteShmHandle *pHandle, int iOffset, int iSize, char *pReadBuf)
{
	if (!pHandle || !pReadBuf || (iOffset < 0) || (iSize <= 0) ) { return (msuNOK); }

	memcpy (pReadBuf, pHandle->pMarteShmReadAddr + iOffset, iSize);

	return (msuOK);
}

int MarteShmRead (MarteShmHandle *pHandle, int iIdx, int iSize, char *pReadBuf)
{
	return (MarteShmReadByOffset (pHandle, iIdx * MARTE_SHM_TYPE_MAX_SIZE, iSize, (char *)pReadBuf));
}

int MarteShmReadInt32 (MarteShmHandle *pHandle, int iIdx, int *piValue)
{
	return (MarteShmRead (pHandle, iIdx, sizeof(int), (char *)piValue));
}

int MarteShmWriteByOffset (MarteShmHandle *pHandle, int iOffset, int iSize, char *pWriteBuf)
{
	if (!pHandle || !pWriteBuf || (iOffset < 0) || (iSize <= 0) ) { return (msuNOK); }

	memcpy (pHandle->pMarteShmWriteAddr + iOffset, pWriteBuf, iSize);

	return (msuOK);
}

int MarteShmWrite (MarteShmHandle *pHandle, int iIdx, int iSize, char *pWriteBuf)
{
	return (MarteShmWriteByOffset (pHandle, iIdx * MARTE_SHM_TYPE_MAX_SIZE, iSize, (char *)pWriteBuf));
}

int MarteShmWriteInt32 (MarteShmHandle *pHandle, int iIdx, int iValue)
{
	return (MarteShmWrite (pHandle, iIdx, sizeof(int), (char *)&iValue));
}

IOSCANPVT MarteShmGetInScanPvt (MarteShmHandle *pHandle)
{
	return (pHandle->inScanPvt);
}

IOSCANPVT MarteShmGetOutScanPvt (MarteShmHandle *pHandle)
{
	return (pHandle->outScanPvt);
}

// -----------------------------------------------------------
// Driver initialization
// -----------------------------------------------------------

int drvMarteShmConfigure (int iShmReadKey, int iShmWriteKey, int iShmSize, int iScanInterval)
{
	if (0 >= iShmReadKey) {
		msuDebug (msuERROR, "[drvMarteShmConfigure] invalid key : 0x%x\n", iShmReadKey);
		return (msuNOK);
	}

	if (0 >= iShmWriteKey) {
		msuDebug (msuERROR, "[drvMarteShmConfigure] invalid write key : 0x%x\n", iShmWriteKey);
		return (msuNOK);
	}

	if (0 >= iShmSize) {
		msuDebug (msuERROR, "[drvMarteShmConfigure] invalid size : %d\n", iShmSize);
		return (msuNOK);
	}

	MarteShmHandle	*pHandle	= MarteShmGetHandle();
	if (!pHandle) { return 0; }

	// stores configuration information
	pHandle->iShmReadKey	= iShmReadKey;
	pHandle->iShmWriteKey	= iShmWriteKey;
	pHandle->iShmSize		= iShmSize * MARTE_SHM_TYPE_MAX_SIZE;	// max signals * max type size
	pHandle->dScanInterval	= iScanInterval > 0 ? iScanInterval/1000.0 : 1.0;

	msuDebug (msuINFO, "[drvMarteShmConfigure] key(%d/0x%04x, %d/0x%04x) size(%d) interval(%f)\n",
			pHandle->iShmReadKey, pHandle->iShmWriteKey, pHandle->iShmSize, pHandle->dScanInterval);

	scanIoInit (&pHandle->inScanPvt);
	scanIoInit (&pHandle->outScanPvt);

	return (msuOK);
}

static long drvMarteShmIoReport (int level)
{
	return 0;
}

static long drvMarteShmInit ()
{
	MarteShmHandle	*pHandle	= MarteShmGetHandle();
	if (!pHandle) { return 0; }

	msuDebug (msuINFO, "[drvMarteShmInit] creates shared memory\n");

	// creates shared memory
	if (NULL == (pHandle->pMarteShmReadAddr = msuSharedMemoryAlloc (pHandle->iShmReadKey, pHandle->iShmSize, 0660))) {
		msuDebug (msuFATAL, "[drvMarteShmInit] cannot create shared memory\n");
		return (msuNOK);
	}

	if (NULL == (pHandle->pMarteShmWriteAddr = msuSharedMemoryAlloc (pHandle->iShmWriteKey, pHandle->iShmSize, 0660))) {
		msuDebug (msuFATAL, "[drvMarteShmInit] cannot create shared memory\n");
		return (msuNOK);
	}

	msuDebug (msuINFO, "[drvMarteShmInit] starting main thread\n");

	// creates thread that accesses shared memory
	epicsThreadCreate (
			"MarteShmMain",
			epicsThreadPriorityMedium,
			epicsThreadGetStackSize(epicsThreadStackBig),
			(EPICSTHREADFUNC)drvMarteShmMain,
			NULL);

	return 0;
}

// -----------------------------------------------------------
// Export and register
// -----------------------------------------------------------

struct {
	long		number;
	DRVSUPFUN	report;
	DRVSUPFUN	init;
} drvMarteShm = {
	2,
	drvMarteShmIoReport,
	drvMarteShmInit
};
epicsExportAddress(drvet, drvMarteShm);

static const iocshArg drvMarteShmConfigureArg0 = { "iShmReadKey" , iocshArgInt };
static const iocshArg drvMarteShmConfigureArg1 = { "iShmWriteKey", iocshArgInt };
static const iocshArg drvMarteShmConfigureArg2 = { "iShmSize"    , iocshArgInt };
static const iocshArg drvMarteShmConfigureArg3 = { "iInterval"   , iocshArgInt };
static const iocshArg * const drvMarteShmConfigureArgs[] = {
	&drvMarteShmConfigureArg0, &drvMarteShmConfigureArg1,
	&drvMarteShmConfigureArg2, &drvMarteShmConfigureArg2
};

static const iocshFuncDef drvMarteShmConfigureDef = { "drvMarteShmConfigure", 4, drvMarteShmConfigureArgs };
static void drvMarteShmConfigureFunc (const iocshArgBuf *args)
{
	if (msuOK != drvMarteShmConfigure (args[0].ival, args[1].ival, args[2].ival, args[3].ival)) {
		exit (1);
	}
}

static void MarteShmLibRegister ()
{
	iocshRegister (&drvMarteShmConfigureDef, drvMarteShmConfigureFunc);
}

epicsExportRegistrar(MarteShmLibRegister);

// End of file
