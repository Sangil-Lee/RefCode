/****************************************************************************

Module      : statCommon.c

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: yunsw   2011-01-31

2011.01.31   yunsw    Initial revision
2011.04.10   tgLee    revision

*****************************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#include "statCommon.h"

static int	kPrintLevel			= K_MON;			// level of debug message
static int	kLogLevel			= K_ERR;			// level of logging to file
static char	kLogFileName[256]	= STR_LOG_FNAME;	// sfwGlobalDef.h

void kLog (const int nDebugLevel, const char *fmt, ... )
{
	if ( (kPrintLevel < nDebugLevel) && (kLogLevel < nDebugLevel) ) {
		return;
	}

	char buf[1024];
	char bufTime[64];
	va_list argp;
	epicsTimeStamp epics_time;

	static FILE *fp = NULL;
	static char szLogFileName[256] = "";

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (bufTime, sizeof(bufTime), "%m/%d %H:%M:%S.%03f", &epics_time);

	if (kPrintLevel >= nDebugLevel) {
		//epicsPrintf ("%s %s", bufTime, buf);
		fprintf (stdout, "[%s] %s", bufTime, buf);
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
			fprintf (fp, "[%s] %s", bufTime, buf);
			fflush (fp);
		}
	}
}

const char *getModeMsg (unsigned int mode)
{
	static char szModeStr[64];

	switch (mode) {
		case OPMODE_INIT : 
			strcpy (szModeStr, "INIT");
			break;
		case OPMODE_REMOTE : 
			strcpy (szModeStr, "REMOTE");
			break;
		case OPMODE_LOCAL : 
			strcpy (szModeStr, "LOCAL");
			break;
		case OPMODE_CALIBRATION : 
			strcpy (szModeStr, "CALIB");
			break;
		default :
			strcpy (szModeStr, "N/A");
			break;
	}

	return (szModeStr);
}

const char *getStatusMsg (unsigned int status, char *pStatusMsg)
{
	static char buf[256];

	char	*pMsg	= (NULL != pStatusMsg) ? pStatusMsg : buf;

	pMsg[0] = 0x00;

	if (status & TASK_STANDBY) 			{ strcat (pMsg, "READY "); }
	if (status & TASK_ARM_ENABLED) 		{ strcat (pMsg, "ARM "); }
	if (status & TASK_WAIT_FOR_TRIGGER) { strcat (pMsg, "WTRG "); }
	if (status & TASK_IN_PROGRESS) 		{ strcat (pMsg, "RUN "); }
	if (status & TASK_POST_PROCESS) 	{ strcat (pMsg, "PST "); }
	if (status & TASK_DATA_TRANSFER) 	{ strcat (pMsg, "TRANS "); }
	if (status & TASK_SYSTEM_IDLE)		{ strcat (pMsg, "IDLE "); }

	if (0x00 == pMsg[0]) {
		strcpy (pMsg, "N/A ");
	}

	return (pMsg);
}

void printStatusMsg ()
{
	char szStatusMsg[256];

	if (NULL != get_master()) {
		getStatusMsg (get_master()->StatusAdmin, szStatusMsg);
		kLog (K_MON, "[Master] %s\n", szStatusMsg);
	}

	ST_STD_device *pSTDdev = get_first_STDdev();

	while (pSTDdev) {
		getStatusMsg (pSTDdev->StatusDev, szStatusMsg);
		kLog (K_MON, "[Device] name(%s) : %s\n", pSTDdev->taskName, szStatusMsg);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
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

