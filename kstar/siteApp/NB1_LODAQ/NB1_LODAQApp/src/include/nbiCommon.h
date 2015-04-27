#ifndef NBI_COMMON_FUNC_H
#define NBI_COMMON_FUNC_H

#include "sfwCommon.h"
#include "sfwDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(OK)
#define OK		0
#endif

#if !defined(NOK)
#define NOK		-1
#endif

typedef enum {
	K_CRI	= 1,
	K_ERR	= 2,
	K_MON	= 3,
	K_INFO	= 4,
	K_TRACE	= 5,
	K_DEBUG	= 6,
	K_DATA	= 8,
	K_DEL	= 9
} kDebugLevel_e;

typedef enum {
	CH_NOTUSED = 0,
	CH_USED = 1
} ChannelStatus_e;

extern const char *getModeMsg (unsigned int mode);
extern const char *getStatusMsg (unsigned int status, char *pStatusMsg);
extern void printStatusMsg ();
extern void kLog (const int nLogLevel, const char *fmt, ... );
extern int getPrintLevel ();
extern int getLogLevel ();
extern int getLogChNo ();
extern int isPrintDebugMsg ();

#ifdef __cplusplus
}
#endif

#endif

