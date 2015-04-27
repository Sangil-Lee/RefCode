/****************************************************************************

Module      : irtvCommon.c

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: yunsw   2011-04-26

2011.04.26   yunsw    Initial revision

*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <epicsTime.h>
#include <iocsh.h>

#if defined(WIN32)
#include <windows.h>
#endif

#include "irtvLog.h"

int	kPrintLevel				= K_MON;			// level of debug message
int	kLogLevel				= K_ERR;			// level of logging to file
char	kLogFileName[256]	= "IRTV_Log.txt";	// sfwGlobalDef.h

epicsUInt32 kEpochTimes ()
{
	epicsTimeStamp epics_time;

	epicsTimeGetCurrent (&epics_time);

	return (epics_time.secPastEpoch + POSIX_TIME_AT_EPICS_EPOCH);
}

void kLog (const int nDebugLevel, const char *fmt, ... )
{
	static FILE *fp = NULL;
	static char szLogFileName[256] = "";

	char buf[1024];
	char bufTime[64];
	char msg[1024];
	va_list argp;
	epicsTimeStamp epics_time;

	if ( (kPrintLevel < nDebugLevel) && (kLogLevel < nDebugLevel) ) {
		return;
	}

	va_start(argp, fmt);
#if defined(WIN32)
	vsprintf_s(buf, sizeof(buf)-1, fmt, argp);
#else
	vsprintf(buf, fmt, argp);
#endif
	va_end(argp);

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (bufTime, sizeof(bufTime), "%m/%d %H:%M:%S.%03f", &epics_time);

	sprintf (msg, "[%s] %s", bufTime, buf);

	if (kPrintLevel >= nDebugLevel) {
		fprintf (stdout, "%s", msg);

#if defined(_DEBUG)
		// Visual Studio의 디버그 출력창에 표시
		OutputDebugString (msg);
#endif
	}

	if (kLogLevel >= nDebugLevel) {
		if ( (NULL == fp) || (0 != strcmp (szLogFileName, kLogFileName)) ) {
			if (NULL != fp) {
				fclose (fp);
			}

			strcpy (szLogFileName, kLogFileName);

			if (NULL == (fp = fopen (szLogFileName, "a+"))) {
				fprintf (stderr, "[%s] open failed \n", szLogFileName);
			}
			else {
				fprintf (stdout, "[%s] logfile %s is opened ...\n", bufTime, szLogFileName);
			}
		}

		if (NULL != fp) {
			fprintf (fp, "%s", msg);
			fflush (fp);
		}
	}
}

int getPrintLevel ()
{
	return (kPrintLevel);
}

int isPrintDebugMsg (const int nDebugLevel)
{
	return ( (getPrintLevel() >= nDebugLevel) ? TRUE : FALSE );
}

void setPrintLevel (const int nPrintLevel)
{
	kPrintLevel	= nPrintLevel;
}

int getLogLevel ()
{
	return (kLogLevel);
}

void setLogLevel (const int nLogLevel)
{
	kLogLevel	= nLogLevel;
}

void setLogFile (const char *pLogFile)
{
	strcpy (kLogFileName, pLogFile);
}

// epics> var kPrintLevel  1
epicsExportAddress (int, kPrintLevel);
epicsExportAddress (int, kLogLevel);
epicsExportAddress (char, kLogFileName);

/* Information needed by iocsh */
static const iocshArg     setPrintLevelArg0		= {"printLevel", iocshArgString};
static const iocshArg    *setPrintLevelArgs[]	= {&setPrintLevelArg0};
static const iocshFuncDef setPrintLevelFuncDef	= {"setPrintLevel", 1, setPrintLevelArgs};

static const iocshArg     setLogLevelArg0		= {"logLevel", iocshArgString};
static const iocshArg    *setLogLevelArgs[]		= {&setLogLevelArg0};
static const iocshFuncDef setLogLevelFuncDef	= {"setLogLevel", 1, setLogLevelArgs};

static const iocshArg     setLogFileArg0		= {"logFile", iocshArgString};
static const iocshArg    *setLogFileArgs[]		= {&setLogFileArg0};
static const iocshFuncDef setLogFileFuncDef		= {"setLogFile", 1, setLogFileArgs};

/* Wrapper called by iocsh, selects the argument types that hello needs */
static void setPrintLevelCallFunc(const iocshArgBuf *args) {
    setPrintLevel(atoi(args[0].sval));
}

/* Registration routine, runs at startup */
static void setPrintLevelRegister(void) {
    iocshRegister(&setPrintLevelFuncDef, setPrintLevelCallFunc);
}

epicsExportRegistrar(setPrintLevelRegister);

static void setLogLevelCallFunc(const iocshArgBuf *args) {
    setLogLevel(atoi(args[0].sval));
}

static void setLogLevelRegister(void) {
    iocshRegister(&setLogLevelFuncDef, setLogLevelCallFunc);
}

epicsExportRegistrar(setLogLevelRegister);

static void setLogFileCallFunc(const iocshArgBuf *args) {
    setLogFile(args[0].sval);
}

static void setLogFileRegister(void) {
    iocshRegister(&setLogFileFuncDef, setLogFileCallFunc);
}

epicsExportRegistrar(setLogFileRegister);

