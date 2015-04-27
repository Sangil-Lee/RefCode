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

static int	kLogChNo			= -1;				// channel id to print and log

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
		kuDebug (kuMON, "[Master] %s\n", szStatusMsg);
	}

	ST_STD_device *pSTDdev = get_first_STDdev();

	while (pSTDdev) {
		getStatusMsg (pSTDdev->StatusDev, szStatusMsg);
		kuDebug (kuMON, "[Device] name(%s) : %s\n", pSTDdev->taskName, szStatusMsg);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

int getLogChNo ()
{
	return (kLogChNo);
}

void setLogChNo (const int nLogChNo)
{
	kLogChNo    = nLogChNo;
}

epicsExportAddress (int, kLogChNo);

/* Information needed by iocsh */
static const iocshArg     setLogChNoArg0		= {"logChNo", iocshArgString};
static const iocshArg    *setLogChNoArgs[]		= {&setLogChNoArg0};
static const iocshFuncDef setLogChNoFuncDef		= {"setLogChNo", 1, setLogChNoArgs};

static void setLogChNoCallFunc(const iocshArgBuf *args) {
	setLogChNo(atoi(args[0].sval));
}

static void setLogChNoRegister(void) {
	iocshRegister(&setLogChNoFuncDef, setLogChNoCallFunc);
}

epicsExportRegistrar(setLogChNoRegister);

