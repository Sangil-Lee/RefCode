/*
**    ==============================================================================
**
**        Abs: Include file for GP307 vacuum gauge
**
**        Name: drvGP307.h
**
**
**        First Auth:  28-Jun-2004, Kukhee Kim (KHKIM)
**        Second Auth: dd-mmm-yyyy, First  Lastname (USERNAME)
**        Rev:         dd-mmm-yyyy, Reviewer's name (USERNAME)
**
**    -------------------------------------------------------------------------------
**        Mod:
**
**    ===============================================================================
*/

#ifndef drvGP307_H
#define drvGP307_H

#include "epicsRingBytes.h"
#include "ellLib.h"
#include "callback.h"
#include "iocsh.h"
#include "asynDriver.h"

#include "drvSup.h"
#include "devSup.h"
#include "recSup.h"
#include "dbScan.h"



#define EOS_STR "\r\n"
#define GP307CMDSTR_DGON    "DG ON"
#define GP307CMDSTR_DGOFF   "DG OFF"
#define GP307CMDSTR_DGS     "DGS"
#define GP307CMDSTR_DSCG0   "DS CG1"
#define GP307CMDSTR_DSCG1   "DS CG2"
#define GP307CMDSTR_DSIG    "DS IG"
#define GP307CMDSTR_IG0ON   "IG1 ON"
#define GP307CMDSTR_IG0OFF  "IG1 OFF"
#define GP307CMDSTR_IG1ON   "IG2 ON"
#define GP307CMDSTR_IG1OFF  "IG2 OFF"
#define GP307CMDSTR_PCS     "PCS"


#define GP307_MSGBUFSIZE 80
#define GP307_DUMBUFSIZE 40

#define GP307_CALLBACK_QUEUE_SIZE 16

#define GP307STATUS_TIMEOUT 0                                 /* communication time out */
#define GP307STATUS_TIMEOUT_MASK (1<<(GP307STATUS_TIMEOUT))
#define GP307STATUS_IG0ONOFF 1                                /* IG0 On Off bit */
#define GP307STATUS_IG0ONOFF_MASK (1<<(GP307STATUS_IG0ONOFF))
#define GP307STATUS_IG1ONOFF 2                                /* IG1 On Off bit */
#define GP307STATUS_IG1ONOFF_MASK (1<<(GP307STATUS_IG1ONOFF))
#define GP307STATUS_DGONOFF 3
#define GP307STATUS_DGONOFF_MASK (1<<(GP307STATUS_DGONOFF))
#define GP307STATUS_CG0VALID 4                               /* valid bit for CG0 reading */
#define GP307STATUS_CG0VALID_MASK (1<<(GP307STATUS_CG0VALID))
#define GP307STATUS_CG1VALID 5                               /* valid bit for CG1 reading */
#define GP307STATUS_CG1VALID_MASK (1<<(GP307STATUS_CG1VALID))
#define GP307STATUS_IGVALID 6                               /* valid bit for IG0 reading */
#define GP307STATUS_IGVALID_MASK (1<<(GP307STATUS_IGVALID))

#define GP307CMD_DONE    -10
#define GP307CMD_TIMEOUT -2
#define GP307CMD_READALL -1
#define GP307CMD_READCG0  0
#define GP307CMD_READCG1  1
#define GP307CMD_ONIG0    2
#define GP307CMD_ONIG1    3
#define GP307CMD_OFFIG0   4
#define GP307CMD_OFFIG1   5
#define GP307CMD_READIG   6 
#define GP307CMD_READPCS  8
#define GP307CMD_ONDG     9
#define GP307CMD_OFFDG    10
#define GP307CMD_DGS      11

#define GP307PROC_CGIGTRANS0   10
#define GP307PROC_IGCGTRNAS1   11
#define GP307PROC_EVALPRESS0   12
#define GP307PROC_EVALPRESS1   13

#define GP307POST_CG0          20
#define GP307POST_CG1          21
#define GP307POST_IGONOFF0     22
#define GP307POST_IGONOFF1     23
#define GP307POST_IG0          24
#define GP307POST_IG1          25
#define GP307POST_CGIGTR0      26
#define GP307POST_CGIGTR1      27
#define GP307POST_IGCGTR0      28
#define GP307POST_IGCGTR1      29
#define GP307POST_PRESS0       30
#define GP307POST_PRESS1       31

#define GP307_STR_CG 0
#define GP307_STR_IG 1
#define GP307_STR_OFF 0
#define GP307_STR_ON 1
#define GP307_STR_UNLOCK 0
#define GP307_STR_LOCK 1
#define GP307_STR_INVALID 0
#define GP307_STR_VALID 1
#define GP307_STR_DISABLE 0
#define GP307_STR_ENABLE 1
#define GP307_STR_TIMEOUT 0
#define GP307_STR_OK 1
#define GP307_STR_OVERRUN 0
#define GP307_STR_PARITY 1
#define GP307_STR_SYNTAX 2




static char *tCgIgList[] = {
    "CG",
    "IG",
    NULL
};

static char vCgIgList[] = {
    0,
    1,
    -1
};

static char *tOnOffList[] = {
    "OFF",
    "ON",
    NULL
};

static char vOnOffList[] = {
    0,
    1,
    -1
};

static char *tLockUnlockList[] = {
    "Unlock",
    "Lock",
    NULL
};

static char vLockUnlockList[] = {
    0,
    1,
    -1
};

static char *tValidInvalidList[] = {
    "Invalid",
    "Valid",
    NULL
};

static char vValidInvalidList[] = {
    0, 
    1,
    -1
};

static char *tEnableDisableList[] = {
    "Disable",
    "Enable",
    NULL,
};

static char vEnableDisableList[] = {
    0,
    1,
    -1
};


static char *tErrMsgList[] = {
    "OVERRUN ERROR",
    "PARITY ERROR",
    "SYNTAX ERROR",
    NULL
};

static char vErrMsgList[] = {
    0,
    1,
    2,
    -1
};

static char *tOkInvalidList[] = {
    "INVALID",
    "OK",
    NULL
};

static char vOkInvalidList[] = {
    0,
    1,
    -1
};

static char *tTimeoutOkList[] = {
    "TIMEOUT",
    "OK",
    NULL
};

static char vTimeoutOkList[] = {
    0,
    1,
    -1
};


typedef struct {
    unsigned char status;
    double cg0_prs;
    double cg1_prs;
    double ig_prs;
    unsigned char pcs;
    char command_from_postCallback;
} GP307_read;

typedef struct {
    asynOctet     *pasynGP307Octet;
    void          *pdrvPvt;
    void          *pdrvGP307Config;
    char          msgBuf[GP307_MSGBUFSIZE];
    char          dumBuf[GP307_DUMBUFSIZE];
} asynGP307UserData;

typedef struct {
    char     command;
    void     *userPvt;
    GP307_read  vGP307_read;
    struct dbCommon *precord;
} drvGP307CallbackQueueBuf;


typedef struct {
    ELLNODE              node;
    char                 port_name[40];
    double               devTimeout;
    double               cbTimeout;
    double               scanInterval;

    GP307_read           *pGP307_read;

    asynUser             *pasynGP307User;
    asynGP307UserData    *pasynGP307UserData;

    unsigned char        cbCount;
    unsigned int         timeoutCount;
    unsigned int         reconnCount;
    unsigned long        asynScanLostCount;
    unsigned long        asynRequestLostCount;

    epicsMutexId         lock;

    CALLBACK             *pPostCallback;

    epicsRingBytesId     queueCallbackRingBytesId;
    epicsRingBytesId     postCallbackRingBytesId;

    IOSCANPVT            ioScanPvt;
} drvGP307Config;



#endif /* drvGP307_H */
