/*
**    ==============================================================================
**
**        Abs: Include file for TPG262 vacuum gauge
**
**        Name: drvTPG262.h
**
**
**        First Auth:  17-May-2004, Kukhee Kim (KHKIM)
**        Second Auth: dd-mmm-yyyy, First  Lastname (USERNAME)
**        Rev:         dd-mmm-yyyy, Reviewer's name (USERNAME)
**
**    -------------------------------------------------------------------------------
**        Mod:
**                     19-May-2004, Kukhee Kim (KHKIM)
**                                  drvTPG262 v0.0a1 release
**
**    ===============================================================================
*/


#ifndef drvTPG262_H
#define drvTPG262_H

#include "epicsRingBytes.h"
#include "ellLib.h"
#include "callback.h"
#include "dbScan.h"

#include "asynDriver.h"
#include "asynStandardInterfaces.h"


#define EOS_STR "\r\n"
#define ETX_STR "\003"
#define ENQ_STR "\005"
#define ACK_STR "\006"
#define NAK_STR "\025"

#define ETX_CHR 003
#define ENQ_CHR 005
#define ACK_CHR 006
#define NAK_CHR 025

#define TPG262_DUMBUFSIZE 40
#define TPG262_MSGBUFSIZE 80
#define TPG262_SCANDELAY  .5
#define TPG262_TIMEOUT    2.

#define TPG262GST_TIMEOUT 7
#define TPG262GID_UNKNOWN_STR "Unknown"    
#define TPG262PROGVER_UNKNOWN_STR "Unknown version"

#define TPG262STATUS_TIMEOUT 0
#define TPG262STATUS_TIMEOUT_MASK (1<<(TPG262STATUS_TIMEOUT))
#define TPG262STATUS_SETUNIT 1
#define TPG262STATUS_SETUNIT_MASK (1<<(TPG262STATUS_SETUNIT))
#define TPG262STATUS_GETGID 2
#define TPG262STATUS_GETGID_MASK (1<<(TPG262STATUS_GETGID))
#define TPG262STATUS_GETPROGVER 3
#define TPG262STATUS_GETPROGVER_MASK (1<<(TPG262STATUS_GETPROGVER))

#define TPG262STATUS_ISSETDRV 7
#define TPG262STATUS_ISSETDRV_MASK (1<<(TPG262STATUS_ISSETDRV_MASK))


static char *tUnitList[] = {
    "mBar",
    "Torr",
    "Pascal",
    NULL
};

static int vUnitList[] = {
    0,
    1,
    2,
    -1
};


static char *tSenStList[] = {
    "data OK",
    "Underrange",
    "Overrange",
    "Sensor Error",
    "Sensor Off",
    "No Sensor",
    "Id Error",
    "Timeout"
};

static int vSenStList[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7
};


static char *tSenIdList[] = {
    "TPR",
    "IKR9",
    "IRK11",
    "PRK",
    "PBR",
    "IMR",
    "CMR",
    "noSen",
    "noid",
    "Unknown"
};

static int vSenIdList[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9
};

static char *tSenIdDescList[] = {
    "Pirani gaguge",
    "Cold cathode gague 1.E-9",
    "Cold cathode gauge 1.E-11",
    "Fullrange CC gague",
    "Fullrange BA gauge",
    "Pirani high pressure gague",
    "Linear gauge",
    "no Sensor",
    "no identification",
    "Unknown/Communication error"
};


typedef struct {
    int                unit;            /* unit */
    char*              unit_str;
    char               progver_str[20];
    char               gid_str0[20];
    char               gid_str1[20];
    int                gst0;            /* gauge status for first */
    double             prs0;            /* pressure reading for first */
    char*              gst_str0;        
    int                gst1;            /* gauge status for second */
    double             prs1;            /* pressure reading for second */
    char*              gst_str1;
} TPG262_read;

typedef struct {
    asynOctet *pasynTPG262Octet;
    void      *pdrvPvt;
    void      *pdrvTPG262Config;
    char      msgBuf[TPG262_MSGBUFSIZE];
    char      dumBuf[TPG262_DUMBUFSIZE];
} asynTPG262UserData;

typedef struct {
    ELLNODE             node;            /* driver linked list */
    char                port_name[40];   /* port name */
    double              devTimeout;
    double              cbTimeout;
    double              scanInterval;

    TPG262_read*        pTPG262_read;    /* reading buffer */

    asynUser*           pasynTPG262User;
    asynTPG262UserData* pasynTPG262UserData;

    unsigned char       status; 
    unsigned char       cbCount;         /* callback STCK counter */
    unsigned int        timeoutCount;
    unsigned int        reconnCount;
    epicsMutexId        lock;            /* mutex lock */

    IOSCANPVT           ioScanPvt;
} drvTPG262Config;

#endif /* drvTPG262_H */
