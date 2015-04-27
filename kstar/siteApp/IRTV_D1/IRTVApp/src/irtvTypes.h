#ifndef _IRTV_TYPES_H
#define _IRTV_TYPES_H

#include <epicsTypes.h>
#include <epicsStdio.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(OK)
#define OK		0
#endif

#if !defined(NOK)
#define NOK		-1
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#if defined(WIN32)
typedef int		socklen_t;

#define localtime_r(sec,buf)	(*(buf) = *localtime(sec))
#endif

#ifdef __cplusplus
}
#endif

#endif	// _IRTV_TYPES_H

