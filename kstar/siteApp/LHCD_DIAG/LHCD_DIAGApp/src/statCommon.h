#ifndef EC1_D1_DAQ_COMMON_FUNC_H
#define EC1_D1_DAQ_COMMON_FUNC_H

#include "kutil.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"
//#include "sfwDBSeq.h"
#include "sfwDriver.h"

#include "myMDSplus.h"

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
	CH_NOTUSED = 0,
	CH_USED = 1
} ChannelStatus_e;

extern const char *getModeMsg (unsigned int mode);
extern const char *getStatusMsg (unsigned int status, char *pStatusMsg);
extern void printStatusMsg ();
extern int getLogLevel ();
extern int getLogChNo ();

#ifdef __cplusplus
}
#endif

#endif	// EC1_D1_DAQ_COMMON_FUNC_H

