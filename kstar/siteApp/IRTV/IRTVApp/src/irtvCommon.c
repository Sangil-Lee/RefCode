/****************************************************************************

Module      : irtvCommon.c

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: yunsw   2011-04-26

2011.04.26   yunsw    Initial revision

*****************************************************************************/

#include <stdio.h>
#include <string.h>

#include "irtvCommon.h"
#include "drvIRTV.h"

#define SFW_ENV_MAX	100

static int	gSfwEnvListCnt = 0;
static char	gSfwEnvNameList [SFW_ENV_MAX][100];
static char	gSfwEnvValueList[SFW_ENV_MAX][256];

#if 1
epicsShareFunc int epicsShareAPI retryCaputByStr (const char *pvName, const char *pvVal)
{
	int	i;

	kLog (K_MON, "[retryCaputByStr] pv(%s) val(%s)\n", pvName, pvVal);

	for (i = 0; i < 3; i++) {
		if (WR_OK == DBproc_put (pvName, pvVal)) {
			// PV 변경으로 Camera 명령이 실행되는 경우, 일정 정도의 Sleep이 필요할 수 있음
			//epicsThreadSleep (0.05);
			return (OK);
		}
		epicsThreadSleep (0.1);
	}

	kLog (K_ERR, "[retryCaputByStr] pv(%s) val(%s) : failed\n", pvName, pvVal);

	return (NOK);
}

epicsShareFunc int epicsShareAPI retryCaputByFloat (const char *pvName, const double pvVal)
{
	char	szValue[64];
	sprintf (szValue, "%f", pvVal);

	return (retryCaputByStr (pvName, szValue));
}

epicsShareFunc int epicsShareAPI retryCaputByInt (const char *pvName, const int pvVal)
{
	char	szValue[64];
	sprintf (szValue, "%d", pvVal);

	return (retryCaputByStr (pvName, szValue));
}
#endif

epicsShareFunc const char * epicsShareAPI sfwEnvGet (const char *pszEnvName)
{
	int	i;

	if (NULL == pszEnvName) {
		return (NULL);
	}

	for (i = 0; i < gSfwEnvListCnt; i++) {
		if (0 == strcmp (pszEnvName, gSfwEnvNameList[i])) {
			//printf ("[sfwEnvGet] %s : %s\n", gSfwEnvNameList[i], gSfwEnvValueList[i]);
			return (gSfwEnvValueList[i]);
		}
	}

	printf ("[sfwEnvGet] %s not found !!!\n", pszEnvName);

	return (NULL);
}

// Set the value of an environment variable
epicsShareFunc void epicsShareAPI sfwEnvSet (const char *name, const char *value)
{
	if (!name || !value) {
		printf ("[sfwEnvSet] input name or value is invalid ... \n");
		return;
	}

	if (SFW_ENV_MAX <= gSfwEnvListCnt) {
		printf ("[sfwEnvSet] overflow ... \n");
		return;
	}

	strcpy (gSfwEnvNameList [gSfwEnvListCnt], name );
	strcpy (gSfwEnvValueList[gSfwEnvListCnt], value);

	gSfwEnvListCnt++;

	//printf ("[sfwEnvSet] cnt(%d) name(%s) value(%s) \n", gSfwEnvListCnt, name, value);
}

static const iocshArg		sfwEnvSetArg0		= {"name", iocshArgString};
static const iocshArg		sfwEnvSetArg1		= {"value", iocshArgString};
static const iocshArg* const sfwEnvSetArgs[]	= {&sfwEnvSetArg0, &sfwEnvSetArg1};
static const iocshFuncDef sfwEnvSetFuncDef		= {"sfwEnvSet", 2, sfwEnvSetArgs};

static void sfwEnvSetCallFunc(const iocshArgBuf *args) {
    sfwEnvSet(args[0].sval, args[1].sval);
}

static void sfwEnvSetRegister(void) {
    iocshRegister(&sfwEnvSetFuncDef, sfwEnvSetCallFunc);
}

epicsExportRegistrar(sfwEnvSetRegister);
