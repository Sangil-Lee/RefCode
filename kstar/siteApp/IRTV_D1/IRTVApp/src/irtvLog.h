#ifndef _IRTV_LOG_H
#define _IRTV_LOG_H

#include "irtvTypes.h"

#ifdef __cplusplus
extern "C" {
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

extern void kLog (const int nLogLevel, const char *fmt, ... );
extern int getPrintLevel ();
extern void setPrintLevel (const int nPrintLevel);
extern int getLogLevel ();
extern int isPrintDebugMsg ();
extern void setLogFile (const char *pLogFile);
extern void setLogLevel (const int nLogLevel);

extern int	kPrintLevel;
extern int	kLogLevel;
extern char	kLogFileName[256];

extern epicsUInt32 kEpochTimes ();

#ifdef __cplusplus
}
#endif

#endif	// _IRTV_LOG_H
