/****************************************************************************
 * drvMarteShm.h
 * --------------------------------------------------------------------------
 * Definition for device and driver
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.04.28  yunsw        Initial revision
 ****************************************************************************/

#ifndef __DRV_MARTE_SHM_H
#define __DRV_MARTE_SHM_H

#include <dbScan.h>

#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsTimer.h>
#include <epicsEvent.h>

#include "MarteShmType.h"
#include "MarteShmUtil.h"

/*  driver initialization define  */

typedef struct {

	// -------------------------------
	// configuration
	// -------------------------------

	//int			iShmKey;
	int			iShmSize;
	int			iShmReadKey;
	int			iShmWriteKey;
	double		dScanInterval;

	// -------------------------------
	// shared memory
	// -------------------------------

	char*		pMarteShmReadAddr;	// start address for reading
	char*		pMarteShmWriteAddr;	// start address for writing

	// -------------------------------
	// ...
	// -------------------------------

	IOSCANPVT	inScanPvt;
	IOSCANPVT	outScanPvt;

	// list
	struct MarteShmHandle*	next;

} MarteShmHandle;

MarteShmHandle* MarteShmGetHandle();

char *MarteShmReadAddr ();
char *MarteShmWriteAddr ();

int MarteShmRead (MarteShmHandle *pHandle, int iOffset, int iSize, char *pReadBuf);
int MarteShmReadByIdx (MarteShmHandle *pHandle, int iIdx, int iSize, char *pReadBuf);
int MarteShmReadInt32 (MarteShmHandle *pHandle, int iIdx, int *piValue);

int MarteShmWrite (MarteShmHandle *pHandle, int iOffset, int iSize, char *pWriteBuf);
int MarteShmWriteByIdx (MarteShmHandle *pHandle, int iIdx, int iSize, char *pWriteBuf);
int MarteShmWriteInt32 (MarteShmHandle *pHandle, int iIdx, int iValue);

IOSCANPVT MarteShmGetInScanPvt (MarteShmHandle *pHandle);
IOSCANPVT MarteShmGetOutScanPvt (MarteShmHandle *pHandle);

#endif /* __DRV_MARTE_SHM_H */
