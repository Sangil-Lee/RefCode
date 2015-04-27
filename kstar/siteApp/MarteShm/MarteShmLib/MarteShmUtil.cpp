/****************************************************************************
 * MarteShmUtil.c
 * --------------------------------------------------------------------------
 * debugging API
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

#include "MarteShmUtil.h"

#include <sys/types.h>
#include <sys/shm.h>

// ---------------------------------------------------------
// Debug
// ---------------------------------------------------------

static int	msuPrintLevel		= msuMON;			// level of debug message

void msuDebug (const int nDebugLevel, const char *fmt, ...)
{
	if (msuPrintLevel < nDebugLevel) {
		return;
	}

	char buf[2048];
	char bufTime[64];
	va_list argp;
	epicsTimeStamp epics_time;

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (bufTime, sizeof(bufTime), "%m/%d %H:%M:%S.%03f", &epics_time);

	//epicsPrintf ("[%s] %s", bufTime, buf);
	fprintf (stdout, "[%s] %s", bufTime, buf);
}

int msuGetPrintLevel ()
{
	return (msuPrintLevel);
}

void msuSetPrintLevel (const int nPrintLevel)
{
	msuPrintLevel	= nPrintLevel;
}

int msuCanPrint (const int nDebugLevel)
{
	return ( (msuGetPrintLevel() >= nDebugLevel) ? msuTRUE : msuFALSE );
}

// epics> var msuPrintLevel  1
epicsExportAddress (int , msuPrintLevel);

/* Information needed by iocsh */
static const iocshArg     setPrintLevelArg0		= {"iLevel", iocshArgString};
static const iocshArg    *setPrintLevelArgs[]	= {&setPrintLevelArg0};
static const iocshFuncDef setPrintLevelFuncDef	= {"msuSetPrintLevel", 1, setPrintLevelArgs};

/* Wrapper called by iocsh, selects the argument types that hello needs */
static void setPrintLevelCallFunc(const iocshArgBuf *args) 
{
	msuSetPrintLevel (atoi(args[0].sval));
}

/* Registration routine, runs at startup */
static void msuSetPrintLevelRegister(void) 
{
	iocshRegister (&setPrintLevelFuncDef, setPrintLevelCallFunc);
}

epicsExportRegistrar (msuSetPrintLevelRegister);


// ---------------------------------------------------------
// Shared memory
// ---------------------------------------------------------
// refered the BaseLib2/Level0/Memory.cpp
// ---------------------------------------------------------

void *msuSharedMemoryAlloc (epicsUInt32 key, epicsUInt32 size, epicsUInt32 permMask)
{
	epicsInt32 shmid = shmget(key, size, IPC_CREAT | permMask);
	if(msuNOK == shmid){
		msuDebug (msuFATAL, "SharedMemoryAlloc:smhid: %s\n", strerror(errno));
		return NULL;
	}

	void *shm = shmat(shmid, NULL, 0);
	if((char *) msuNOK == shm){
		msuDebug (msuFATAL, "SharedMemoryAlloc:shmat: %s\n", strerror(errno));
		return NULL;
	}
	return shm;
}

void msuSharedMemoryFree (void *address)
{
	epicsInt32 ret = shmdt(address);
	if(msuNOK == ret){
		msuDebug (msuFATAL, "SharedMemoryFree: %s\n", strerror(errno));
	}
}

