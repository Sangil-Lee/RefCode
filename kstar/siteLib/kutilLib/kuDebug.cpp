/****************************************************************************
 * kuDebug.c
 * --------------------------------------------------------------------------
 * debugging API
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

#include "kuDebug.h"

static int	kuPrintLevel	= kuMON;			// level of debug message
static int	kuLogLevel		= kuERR;			// level of logging to file
static char	kuLogFileName[KU_PATH_NAME_LEN]	= "";	// name of log file

void kuDebug (const kuInt32 nDebugLevel, const char *fmt, ...)
{
	if ( (kuPrintLevel < nDebugLevel) && (kuLogLevel < nDebugLevel) ) {
		return;
	}

	char buf[1024];
	char bufTime[64];
	va_list argp;
	kuTimeStamp epics_time;

	static FILE *fp = NULL;
	static char szLogFileName[KU_PATH_NAME_LEN] = "";

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (bufTime, sizeof(bufTime), "%m/%d %H:%M:%S.%03f", &epics_time);

	if (kuPrintLevel >= nDebugLevel) {
		//epicsPrintf ("%s %s", bufTime, buf);
		fprintf (stdout, "[%s] %s", bufTime, buf);
	}

	if (kuLogLevel >= nDebugLevel) {
		if ( (strlen(kuLogFileName) > 0)
				&& ((NULL == fp) || (0 != strcmp (szLogFileName, kuLogFileName)) ) ) {
			if (NULL != fp) {
				fclose (fp);
			}

			strcpy (szLogFileName, kuLogFileName);

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

kuInt32 kuGetPrintLevel ()
{
	return (kuPrintLevel);
}

void kuSetPrintLevel (const kuInt32 nPrintLevel)
{
	kuPrintLevel	= nPrintLevel;
}

kuInt32 kuCanPrint (const kuInt32 nDebugLevel)
{
	return ( (kuGetPrintLevel() >= nDebugLevel) ? kuTRUE : kuFALSE );
}

kuInt32 kuGetLogLevel ()
{
	return (kuLogLevel);
}

void kuSetLogLevel (const kuInt32 nLogLevel)
{
	kuLogLevel	= nLogLevel;
}

const char *kuGetLogFile ()
{
	return (kuLogFileName);
}

void kuSetLogFile (const char *pLogFile)
{
	if (pLogFile && (KU_PATH_NAME_LEN > strlen(pLogFile)) ) {
		strncpy (kuLogFileName, pLogFile, KU_PATH_NAME_LEN);
	}
}

// epics> var kuPrintLevel  1
epicsExportAddress (int , kuPrintLevel);
epicsExportAddress (int , kuLogLevel);
epicsExportAddress (char, kuLogFileName);

/* Information needed by iocsh */
static const iocshArg     setPrintLevelArg0		= {"iLevel", iocshArgString};
static const iocshArg    *setPrintLevelArgs[]	= {&setPrintLevelArg0};
static const iocshFuncDef setPrintLevelFuncDef	= {"kuSetPrintLevel", 1, setPrintLevelArgs};

static const iocshArg     setLogLevelArg0		= {"iLevel", iocshArgString};
static const iocshArg    *setLogLevelArgs[]		= {&setLogLevelArg0};
static const iocshFuncDef setLogLevelFuncDef	= {"kuSetLogLevel", 1, setLogLevelArgs};

static const iocshArg     setLogFileArg0		= {"szFileName", iocshArgString};
static const iocshArg    *setLogFileArgs[]		= {&setLogFileArg0};
static const iocshFuncDef setLogFileFuncDef		= {"kuSetLogFile", 1, setLogFileArgs};

/* Wrapper called by iocsh, selects the argument types that hello needs */
static void setPrintLevelCallFunc(const iocshArgBuf *args) {
	kuSetPrintLevel (atoi(args[0].sval));
}

/* Registration routine, runs at startup */
static void kuSetPrintLevelRegister(void) {
	iocshRegister (&setPrintLevelFuncDef, setPrintLevelCallFunc);
}

epicsExportRegistrar (kuSetPrintLevelRegister);

static void setLogLevelCallFunc(const iocshArgBuf *args) {
	kuSetLogLevel (atoi(args[0].sval));
}

static void kuSetLogLevelRegister(void) {
	iocshRegister (&setLogLevelFuncDef, setLogLevelCallFunc);
}

epicsExportRegistrar (kuSetLogLevelRegister);

static void setLogFileCallFunc(const iocshArgBuf *args) {
	kuSetLogFile (args[0].sval);
}

static void kuSetLogFileRegister(void) {
	iocshRegister (&setLogFileFuncDef, setLogFileCallFunc);
}

epicsExportRegistrar (kuSetLogFileRegister);

