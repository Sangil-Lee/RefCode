#ifndef _IRTV_COMMON_H
#define _IRTV_COMMON_H

#include "irtvTypes.h"
#include "irtvLog.h"

#ifdef __cplusplus
extern "C" {
#endif

epicsShareFunc const char * epicsShareAPI sfwEnvGet (const char *pszEnvName);
epicsShareFunc void epicsShareAPI sfwEnvSet (const char *name, const char *value);

epicsShareFunc int epicsShareAPI retryCaputByStr (const char *pvName, const char *pvVal);
epicsShareFunc int epicsShareAPI retryCaputByFloat (const char *pvName, const double pvVal);
epicsShareFunc int epicsShareAPI retryCaputByInt (const char *pvName, const int pvVal);

#ifdef __cplusplus
}
#endif

#endif	// _IRTV_COMMON_H

