/****************************************************************************
 * MarteShmUtil.h
 * --------------------------------------------------------------------------
 * debug interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

#ifndef __MARTE_SHM_DEBUG_H
#define __MARTE_SHM_DEBUG_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#include <epicsStdio.h>
#include <epicsTime.h>
#include <epicsPrint.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>

#include <iocsh.h>
#include <errlog.h>
#include <shareLib.h>
#include <ellLib.h>

#if !defined(msuOK)
#define msuOK    0
#endif

#if !defined(msuNOK)
#define msuNOK   -1
#endif

#if !defined(msuTRUE)
#define msuTRUE  1
#endif

#if !defined(msuFALSE)
#define msuFALSE 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------
// Debug
// -----------------------------------------

typedef enum {
	msuOFF		= 0,
	msuFATAL	= 1,
	msuERROR	= 2,
	msuWARN		= 3,
	msuMON		= 4,
	msuINFO		= 5,
	msuDEBUG	= 6,
	msuTRACE	= 7,
	msuDATA		= 8,
	msuALL		= 9
} MarteShmUtilLevel_e;

epicsShareFunc void epicsShareAPI msuDebug (const int nLogLevel, const char *fmt, ... );
epicsShareFunc int epicsShareAPI msuGetPrintLevel ();
epicsShareFunc void epicsShareAPI msuSetPrintLevel (const int nPrintLevel);
epicsShareFunc int epicsShareAPI msuCanPrint (const int nLogLevel);

void *msuSharedMemoryAlloc (epicsUInt32 key, epicsUInt32 size, epicsUInt32 permMask);
void msuSharedMemoryFree (void *address);

#ifdef __cplusplus
}
#endif

#endif	// __MARTE_SHM_DEBUG_H

