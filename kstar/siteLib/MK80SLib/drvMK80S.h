/*
 * ==============================================================================
 *   Name: drvMK80S.h
 *   Desc: Include file for MASTER-K 80S PLC
 * ------------------------------------------------------------------------------
 *   2012/06/29  yunsw      Initial revision
 * ==============================================================================
*/

#ifndef drvMK80S_H
#define drvMK80S_H

#include "kutil.h"

#include "errlog.h"
#include "epicsRingBytes.h"
#include "ellLib.h"
#include "callback.h"
#include "dbScan.h"

#include "asynDriver.h"

#define ETX_STR "\003"
#define EOT_STR "\004"
#define ENQ_STR "\005"
#define ACK_STR "\006"
#define NAK_STR "\025"

#define ETX_CHR 003
#define EOT_CHR 004
#define ENQ_CHR 005
#define ACK_CHR 006
#define NAK_CHR 025

#define MK80SSTATUS_TIMEOUT 0
#define MK80SSTATUS_TIMEOUT_MASK (1<<(MK80SSTATUS_TIMEOUT))

#define MK80S_MSGBUFSIZE	256
#define	MK80S_MAX_INPUT		16

typedef struct {
	int					nBitValues[MK80S_MAX_INPUT];	/* input value for bit  */
	int					nWordValues[MK80S_MAX_INPUT];	/* input value for word */
} MK80S_read;

typedef struct {
	asynOctet			*pasynMK80SOctet;
	void				*pdrvPvt;
	void				*pdrvMK80SConfig;
	char				msgBuf[MK80S_MSGBUFSIZE];
} asynMK80SUserData;

typedef struct {
	ELLNODE             node;            /* driver linked list */
	char                port_name[40];   /* port name */
	double              devTimeout;
	double              cbTimeout;
	double              scanInterval;

	MK80S_read*         pMK80S_read;     /* data buffer */

	asynUser*           pasynMK80SUser;
	asynMK80SUserData*  pasynMK80SUserData;

	unsigned char       status; 
	unsigned char       cbCount;         /* callback STCK counter */
	unsigned int        timeoutCount;
	epicsMutexId        lock;            /* mutex lock */

	IOSCANPVT           ioScanPvt;
} drvMK80SConfig;

typedef struct {
	int					num;
	char				addr[MK80S_MAX_INPUT][20];
	int					idx [MK80S_MAX_INPUT];
} drvMK80SInputList;

extern drvMK80SConfig *drvMK80S_get(int link);
extern int drvMK80S_send (int link, char *addr, int val);

extern drvMK80SInputList *pdrvMK80SBitInputList;
extern drvMK80SInputList *pdrvMK80SWordInputList;

#endif /* drvMK80S_H */
