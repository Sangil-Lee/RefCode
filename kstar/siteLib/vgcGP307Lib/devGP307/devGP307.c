/*
**    =========================================================================
**
**        Abs: device support for VGC GP307 Vacuum Gauge Controller
**
**        Name: devGP307.c
**
**        First Auth:   06-Jul-2004,  Kukhee Kim         (KHKIM)
**        Second Auth:  dd-mmm-yyyy,  First Lastname     (USERNAME)
**        Rev:          dd-mmm-yyyy,  Reviewer's name    (USERNAME)
**
**    -------------------------------------------------------------------------
**        Mod:
**
**    =========================================================================
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "callback.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "link.h"
#include "dbCommon.h"

#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"

#include "epicsExport.h"

#include "drvGP307.h"

#define GP307DEVPARAM_TERM        -1

#define GP307DEVPARAM_DGONOFF     0
#define GP307DEVPARAM_IG0ONOFF    1
#define GP307DEVPARAM_IG1ONOFF    2
#define GP307DEVPARAM_READDGS     3
#define GP307DEVPARAM_READPCS     4
#define GP307DEVPARAM_READCG0     5
#define GP307DEVPARAM_READCG1     6
#define GP307DEVPARAM_READIG      7
#define GP307DEVPARAM_STATUSCG0   8
#define GP307DEVPARAM_STATUSCG1   9
#define GP307DEVPARAM_STATUSIG    10
#define GP307DEVPARAM_STATUSTIMEOUT 11
#define GP307DEVPARAM_STATUSONOFFIG0 12
#define GP307DEVPARAM_STATUSONOFFIG1 13
#define GP307DEVPARAM_STATUSCBCOUNT 14
#define GP307DEVPARAM_STATUSTIMEOUTCOUNT 15
#define GP307DEVPARAM_STATUSRECONNCOUNT 16
#define GP307DEVPARAM_STATUSASYNSCANLOSTCNT 17
#define GP307DEVPARAM_STATUSASYNREQUESTLOSTCNT 18
#define GP307DEVPARAM_STATUSQCBRNGBUFFREEBYTES 20
#define GP307DEVPARAM_STATUSQCBRNGBUFFREEEVENTS 21
#define GP307DEVPARAM_STATUSPCBRNGBUFFREEBYTES 22
#define GP307DEVPARAM_STATUSPCBRNGBUFFREEEVENTS 23

#define GP307DEVPARAM_STR_DGONOFF   "dg_onoff"
#define GP307DEVPARAM_STR_IG0ONOFF  "ig0_onoff"
#define GP307DEVPARAM_STR_IG1ONOFF  "ig1_onoff"
#define GP307DEVPARAM_STR_READDGS   "dgs"
#define GP307DEVPARAM_STR_READPCS   "pcs"
#define GP307DEVPARAM_STR_READCG0   "cg0"
#define GP307DEVPARAM_STR_READCG1   "cg1"
#define GP307DEVPARAM_STR_READIG    "ig"
#define GP307DEVPARAM_STR_STATUSCG0 "scg0"
#define GP307DEVPARAM_STR_STATUSCG1 "scg1"
#define GP307DEVPARAM_STR_STATUSIG  "sig"
#define GP307DEVPARAM_STR_STATUSTIMEOUT "timeout"
#define GP307DEVPARAM_STR_STATUSONOFFIG0 "sig0_onoff"
#define GP307DEVPARAM_STR_STATUSONOFFIG1 "sig1_onoff"
#define GP307DEVPARAM_STR_STATUSCBCOUNT "cbCount"
#define GP307DEVPARAM_STR_STATUSTIMEOUTCOUNT "timeoutCount"
#define GP307DEVPARAM_STR_STATUSRECONNCOUNT  "reconnCount"
#define GP307DEVPARAM_STR_STATUSASYNSCANLOSTCNT "asynScanLostCount"
#define GP307DEVPARAM_STR_STATUSASYNREQUESTLOSTCNT "asynRequestLostCount"
#define GP307DEVPARAM_STR_STATUSQCBRNGBUFFREEBYTES "qcbRngBufFreeBytes"
#define GP307DEVPARAM_STR_STATUSQCBRNGBUFFREEEVENTS "qcbRngBufFreeEvents"
#define GP307DEVPARAM_STR_STATUSPCBRNGBUFFREEBYTES "pcbRngBufFreeBytes"
#define GP307DEVPARAM_STR_STATUSPCBRNGBUFFREEEVENTS "pcbRngBufFreeEvents"

typedef struct {
    char port_name[40];
    char param_str[40];
    int  param;
    drvGP307Config *pdrvGP307Config;
} devGP307dpvt;


typedef struct {
    char *param_str;
    int param;
} devGP307tParam;


LOCAL devGP307tParam pdevGP307tParam[] = {
    {GP307DEVPARAM_STR_DGONOFF,  GP307DEVPARAM_DGONOFF},
    {GP307DEVPARAM_STR_IG0ONOFF, GP307DEVPARAM_IG0ONOFF},
    {GP307DEVPARAM_STR_IG1ONOFF, GP307DEVPARAM_IG1ONOFF},
    {GP307DEVPARAM_STR_READDGS,  GP307DEVPARAM_READDGS},
    {GP307DEVPARAM_STR_READPCS,  GP307DEVPARAM_READPCS},
    {GP307DEVPARAM_STR_READCG0,  GP307DEVPARAM_READCG0},
    {GP307DEVPARAM_STR_READCG1,  GP307DEVPARAM_READCG1},
    {GP307DEVPARAM_STR_READIG,   GP307DEVPARAM_READIG},
    {GP307DEVPARAM_STR_STATUSCG0, GP307DEVPARAM_STATUSCG0},
    {GP307DEVPARAM_STR_STATUSCG1, GP307DEVPARAM_STATUSCG1},
    {GP307DEVPARAM_STR_STATUSIG, GP307DEVPARAM_STATUSIG},
    {GP307DEVPARAM_STR_STATUSTIMEOUT, GP307DEVPARAM_STATUSTIMEOUT},
    {GP307DEVPARAM_STR_STATUSONOFFIG0, GP307DEVPARAM_STATUSONOFFIG0},
    {GP307DEVPARAM_STR_STATUSONOFFIG1, GP307DEVPARAM_STATUSONOFFIG1},
    {GP307DEVPARAM_STR_STATUSCBCOUNT,  GP307DEVPARAM_STATUSCBCOUNT},
    {GP307DEVPARAM_STR_STATUSTIMEOUTCOUNT, GP307DEVPARAM_STATUSTIMEOUTCOUNT},
    {GP307DEVPARAM_STR_STATUSRECONNCOUNT,  GP307DEVPARAM_STATUSRECONNCOUNT},
    {GP307DEVPARAM_STR_STATUSASYNSCANLOSTCNT, GP307DEVPARAM_STATUSASYNSCANLOSTCNT},
    {GP307DEVPARAM_STR_STATUSASYNREQUESTLOSTCNT, GP307DEVPARAM_STATUSASYNREQUESTLOSTCNT},
    {GP307DEVPARAM_STR_STATUSQCBRNGBUFFREEBYTES, GP307DEVPARAM_STATUSQCBRNGBUFFREEBYTES},
    {GP307DEVPARAM_STR_STATUSQCBRNGBUFFREEEVENTS, GP307DEVPARAM_STATUSQCBRNGBUFFREEEVENTS},
    {GP307DEVPARAM_STR_STATUSPCBRNGBUFFREEBYTES, GP307DEVPARAM_STATUSPCBRNGBUFFREEBYTES},
    {GP307DEVPARAM_STR_STATUSPCBRNGBUFFREEEVENTS, GP307DEVPARAM_STATUSPCBRNGBUFFREEEVENTS}, 
    {NULL,                       GP307DEVPARAM_TERM}
};



extern drvGP307Config* drvGP307_find(char *portName);


LOCAL long devAiGP307_init_record(aiRecord *precord);
LOCAL long devAiGP307_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiGP307_read_ai(aiRecord *precord);

struct {
    long        number;
    DEVSUPFUN   report;
    DEVSUPFUN   init;
    DEVSUPFUN   init_record;
    DEVSUPFUN   get_ioint_info;
    DEVSUPFUN   read_ai;
    DEVSUPFUN   special_linconv;
} devAiGP307 = {
    6,
    NULL,
    NULL, 
    devAiGP307_init_record,
    devAiGP307_get_ioint_info,
    devAiGP307_read_ai,
    NULL
};

epicsExportAddress(dset, devAiGP307);



LOCAL long devLiGP307_init_record(longinRecord *precord);
LOCAL long devLiGP307_get_ioint_info(int cmd, longinRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devLiGP307_read_li(longinRecord *precord);

struct {
    long        number;
    DEVSUPFUN   report;
    DEVSUPFUN   init;
    DEVSUPFUN   init_record;
    DEVSUPFUN   get_ioinit_info;
    DEVSUPFUN   read_li;
} devLiGP307 = {
    5,
    NULL,
    NULL,
    devLiGP307_init_record,
    devLiGP307_get_ioint_info,
    devLiGP307_read_li
};

epicsExportAddress(dset, devLiGP307);



LOCAL long devBiGP307_init_record(biRecord *precord);
LOCAL long devBiGP307_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devBiGP307_read_bi(biRecord *precord);
LOCAL long devBiGP307_read_bi_synchronous(biRecord *precord);
LOCAL long devBiGP307_read_bi_Asynchronous(biRecord *precord);


struct {
    long        number;
    DEVSUPFUN   report;
    DEVSUPFUN   init;
    DEVSUPFUN   init_record;
    DEVSUPFUN   get_ioint_info;
    DEVSUPFUN   read_bi;
} devBiGP307 = {
    5,
    NULL,
    NULL,
    devBiGP307_init_record,
    devBiGP307_get_ioint_info,
    devBiGP307_read_bi
};

epicsExportAddress(dset, devBiGP307);



LOCAL long devBoGP307_init_record(boRecord *precord);
LOCAL long devBoGP307_write_bo(boRecord *precord);


struct {
    long         number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoGP307 = {
    5,
    NULL,
    NULL,
    devBoGP307_init_record,
    NULL,
    devBoGP307_write_bo
};

epicsExportAddress(dset, devBoGP307);



LOCAL long devMbbiGP307_init_record(mbbiRecord *precord);
LOCAL long devMbbiGP307_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devMbbiGP307_read_mbbi(mbbiRecord *precord);

struct {
    long         number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_mbbi;
} devMbbiGP307 = {
    5,
    NULL,
    NULL,
    devMbbiGP307_init_record,
    devMbbiGP307_get_ioint_info,
    devMbbiGP307_read_mbbi
};

epicsExportAddress(dset, devMbbiGP307);



LOCAL long devSiGP307_init_record(stringinRecord *precord);
LOCAL long devSiGP307_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devSiGP307_read_si(stringinRecord *precord);


struct {
    long        number;
    DEVSUPFUN   report;
    DEVSUPFUN   init;
    DEVSUPFUN   init_record;
    DEVSUPFUN   get_ioinit_info;
    DEVSUPFUN   read_si;
} devSiGP307 = {
    5,
    NULL,
    NULL,
    devSiGP307_init_record,
    devSiGP307_get_ioint_info,
    devSiGP307_read_si
};

epicsExportAddress(dset, devSiGP307);



LOCAL long devAiGP307_init_record(aiRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)malloc(sizeof(devGP307dpvt));
    int index = 0;

    switch(precord->inp.type) {
        case(INST_IO):
            sscanf(precord->inp.value.instio.string,
                   "%s %s",
                   pdevGP307dpvt->port_name, pdevGP307dpvt->param_str);
            pdevGP307dpvt->pdrvGP307Config = drvGP307_find(pdevGP307dpvt->port_name);
            if(!pdevGP307dpvt->pdrvGP307Config) goto end;
            while((pdevGP307tParam+index)->param_str) {
                if(!strcmp((pdevGP307tParam+index)->param_str, pdevGP307dpvt->param_str)) break;
                index++;
            }
            pdevGP307dpvt->param = (pdevGP307tParam+index)->param;
            if(pdevGP307dpvt->param<0) goto end;
            if(pdevGP307dpvt->param == GP307DEVPARAM_READCG0 ||
               pdevGP307dpvt->param == GP307DEVPARAM_READCG1 ||
               pdevGP307dpvt->param == GP307DEVPARAM_READIG )
                break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord, 
                              "devAiGP307 (init_record) Illegal INP field");
            free(pdevGP307dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevGP307dpvt;
    /* Must sure record processing does not perform any conversion */
    precord->linr = 0;
    return 0;
}


LOCAL long devAiGP307_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*) precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*) pdevGP307dpvt->pdrvGP307Config;


    *ioScanPvt = pdrvGP307Config->ioScanPvt;

    return 0;
}


LOCAL long devAiGP307_read_ai(aiRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*) pdrvGP307Config->pGP307_read;

    switch(pdevGP307dpvt->param) {
        case GP307DEVPARAM_READCG0:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pGP307_read->cg0_prs;
            epicsMutexUnlock(pdrvGP307Config->lock);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_READCG1:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pGP307_read->cg1_prs;
            epicsMutexUnlock(pdrvGP307Config->lock);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_READIG:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pGP307_read->ig_prs;
            epicsMutexUnlock(pdrvGP307Config->lock);
            precord->udf = FALSE;
            break;
    }


    return 2; /* don't do any conversion */
}


LOCAL long devLiGP307_init_record(longinRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)malloc(sizeof(devGP307dpvt));
    int index = 0;

    switch(precord->inp.type) {
        case(INST_IO):
            sscanf(precord->inp.value.instio.string,
                   "%s %s",
                   pdevGP307dpvt->port_name, pdevGP307dpvt->param_str);
            pdevGP307dpvt->pdrvGP307Config = drvGP307_find(pdevGP307dpvt->port_name);
            if(!pdevGP307dpvt->pdrvGP307Config) goto end;
            while((pdevGP307tParam+index)->param_str) {
                if(!strcmp((pdevGP307tParam+index)->param_str, pdevGP307dpvt->param_str)) break;
                index++;
            }
            pdevGP307dpvt->param = (pdevGP307tParam+index)->param;
            if(pdevGP307dpvt->param<0) goto end;
            if(pdevGP307dpvt->param == GP307DEVPARAM_STATUSCBCOUNT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSTIMEOUTCOUNT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSRECONNCOUNT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSASYNSCANLOSTCNT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSASYNREQUESTLOSTCNT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSQCBRNGBUFFREEBYTES ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSQCBRNGBUFFREEEVENTS ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSPCBRNGBUFFREEBYTES ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSPCBRNGBUFFREEEVENTS 
            ) break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devLiGP307 (init_record) Illegal INP field");
            free(pdevGP307dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevGP307dpvt;

    return 0;

}


LOCAL long devLiGP307_get_ioint_info(int cmd, longinRecord *precord, IOSCANPVT *ioScanPvt)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;

    *ioScanPvt = pdrvGP307Config->ioScanPvt;

    return 0;
}


LOCAL long devLiGP307_read_li(longinRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*) pdrvGP307Config->pGP307_read;

    switch(pdevGP307dpvt->param) {
        case GP307DEVPARAM_STATUSCBCOUNT:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pdrvGP307Config->cbCount;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSTIMEOUTCOUNT:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pdrvGP307Config->timeoutCount;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSRECONNCOUNT:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pdrvGP307Config->reconnCount;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSASYNSCANLOSTCNT:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pdrvGP307Config->asynScanLostCount;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSASYNREQUESTLOSTCNT:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = pdrvGP307Config->asynRequestLostCount;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSQCBRNGBUFFREEBYTES:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = epicsRingBytesFreeBytes(pdrvGP307Config->queueCallbackRingBytesId);
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSQCBRNGBUFFREEEVENTS:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = epicsRingBytesFreeBytes(pdrvGP307Config->queueCallbackRingBytesId)/sizeof(drvGP307CallbackQueueBuf);
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSPCBRNGBUFFREEBYTES:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = epicsRingBytesFreeBytes(pdrvGP307Config->postCallbackRingBytesId);
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSPCBRNGBUFFREEEVENTS:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->val = epicsRingBytesFreeBytes(pdrvGP307Config->postCallbackRingBytesId)/sizeof(drvGP307CallbackQueueBuf);
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
    }

    return 2;
}



LOCAL long devBiGP307_init_record(biRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)malloc(sizeof(devGP307dpvt));
    int index = 0;

    switch(precord->inp.type) {
        case (INST_IO):
            sscanf(precord->inp.value.instio.string,
                   "%s %s",
                   pdevGP307dpvt->port_name, pdevGP307dpvt->param_str);
            pdevGP307dpvt->pdrvGP307Config = drvGP307_find(pdevGP307dpvt->port_name);
            if(!pdevGP307dpvt->pdrvGP307Config) goto end;
            while((pdevGP307tParam+index)->param_str) {
                if(!strcmp((pdevGP307tParam+index)->param_str, pdevGP307dpvt->param_str)) break;
                index++;
            }
            pdevGP307dpvt->param = (pdevGP307tParam+index)->param;
            if(pdevGP307dpvt->param < 0) goto end;
            if(pdevGP307dpvt->param == GP307DEVPARAM_READDGS ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSCG0 ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSCG1 ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSIG ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSTIMEOUT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSONOFFIG0 ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSONOFFIG1)
                break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devBiGP307 (init_record) Illegal INP field");
            free(pdevGP307dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevGP307dpvt;

    return 0;
}


LOCAL long devBiGP307_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;

    *ioScanPvt = pdrvGP307Config->ioScanPvt;

    return 0;
}



LOCAL long devBiGP307_read_bi(biRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;

    switch(pdevGP307dpvt->param){
        /* asynchronous processing */
        case GP307DEVPARAM_READDGS:
            return devBiGP307_read_bi_Asynchronous(precord);
            break;
        /* synchronous processing */
        case GP307DEVPARAM_STATUSCG0:
        case GP307DEVPARAM_STATUSCG1:
        case GP307DEVPARAM_STATUSIG:
        case GP307DEVPARAM_STATUSTIMEOUT:
        case GP307DEVPARAM_STATUSONOFFIG0:
        case GP307DEVPARAM_STATUSONOFFIG1:
        default:
            return devBiGP307_read_bi_synchronous(precord);
    }

}


LOCAL long devBiGP307_read_bi_Asynchronous(biRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*)pdrvGP307Config->pGP307_read;
    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;
    asynStatus vasynStatus;


    if(precord->pact) { /* post processing */
        switch(pGP307_read->command_from_postCallback) {
            case GP307CMD_DGS:
                epicsMutexLock(pdrvGP307Config->lock);
                precord->rval = (pGP307_read->status & GP307STATUS_DGONOFF_MASK)?1:0;
                epicsMutexUnlock(pdrvGP307Config->lock);
                break;
            default:     /* disable record function forever */
                precord->pact = TRUE;
                precord->udf = TRUE;
                return 0;
        }

        precord->val = precord->rval;
        precord->udf = FALSE;
        return 2;        /* don't do any conversion */
    }

    /* pre-processing */
    precord->pact = TRUE;

    epicsMutexLock(pdrvGP307Config->lock);
    pdrvGP307Config->cbCount++;
    pBufGP307_read->status = pGP307_read->status;
    epicsMutexUnlock(pdrvGP307Config->lock);

    pdrvGP307CallbackQueueBuf->userPvt = (void*)pdrvGP307Config;
    pdrvGP307CallbackQueueBuf->precord = (dbCommon*)precord;

    switch(pdevGP307dpvt->param) {
        case GP307DEVPARAM_READDGS:
            pdrvGP307CallbackQueueBuf->command = GP307CMD_DGS;
            break; 
        default: /* disable record function forerver */
            precord->pact = TRUE;
            precord->udf = TRUE;
            return 0;
    }
   
    /* put the callback queue */
    nbytes = epicsRingBytesPut(pdrvGP307Config->queueCallbackRingBytesId,
                               (char*)pdrvGP307CallbackQueueBuf,
                               sizeof(drvGP307CallbackQueueBuf));
    vasynStatus = pasynManager->queueRequest(pdrvGP307Config->pasynGP307User, asynQueuePriorityLow, pdrvGP307Config->cbTimeout);
    if(vasynStatus == asynError) {
        epicsMutexLock(pdrvGP307Config->lock);
        pdrvGP307Config->asynRequestLostCount++;
        epicsMutexUnlock(pdrvGP307Config->lock);
    }
    return 0;
}


LOCAL long devBiGP307_read_bi_synchronous(biRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*)pdrvGP307Config->pGP307_read;

    switch(pdevGP307dpvt->param){
        case GP307DEVPARAM_STATUSCG0:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->rval = (pGP307_read->status & GP307STATUS_CG0VALID_MASK)?1:0;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSCG1:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->rval = (pGP307_read->status & GP307STATUS_CG1VALID_MASK)?1:0;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSIG:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->rval = (pGP307_read->status & GP307STATUS_IGVALID_MASK)?1:0;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSTIMEOUT:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->rval = (pGP307_read->status & GP307STATUS_TIMEOUT_MASK)?1:0;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSONOFFIG0:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->rval = (pGP307_read->status & GP307STATUS_IG0ONOFF_MASK)?1:0;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        case GP307DEVPARAM_STATUSONOFFIG1:
            epicsMutexLock(pdrvGP307Config->lock);
            precord->rval = (pGP307_read->status & GP307STATUS_IG1ONOFF_MASK)?1:0;
            epicsMutexUnlock(pdrvGP307Config->lock);
            break;
        default:
            precord->pact = TRUE;
            precord->udf = TRUE;
            return 0;
    }

    precord->val = precord->rval;
    precord->udf = FALSE;

    return 2; /* don't do any conversion */

}


LOCAL long devBoGP307_init_record(boRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)malloc(sizeof(devGP307dpvt));
    int index = 0;

    switch(precord->out.type) {
        case (INST_IO):
            sscanf(precord->out.value.instio.string,
                   "%s %s",
                   pdevGP307dpvt->port_name, pdevGP307dpvt->param_str); 
            pdevGP307dpvt->pdrvGP307Config = drvGP307_find(pdevGP307dpvt->port_name);
            if(!pdevGP307dpvt->pdrvGP307Config) goto end;
            while((pdevGP307tParam+index)->param_str) {
                if(!strcmp((pdevGP307tParam+index)->param_str, pdevGP307dpvt->param_str)) break;
                index++;
            }
            pdevGP307dpvt->param = (pdevGP307tParam+index)->param;
            if(pdevGP307dpvt->param < 0 ) goto end;
            if(pdevGP307dpvt->param == GP307DEVPARAM_DGONOFF ||
               pdevGP307dpvt->param == GP307DEVPARAM_IG0ONOFF ||
               pdevGP307dpvt->param == GP307DEVPARAM_IG1ONOFF)
                break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devBoGP307 (init_record) Illegal OUT field");
            free(pdevGP307dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
        
    }
  
    precord->udf = FALSE;
    precord->dpvt = (void*)pdevGP307dpvt;

    return 0;

}


LOCAL long devBoGP307_write_bo(boRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*) pdrvGP307Config->pGP307_read;
    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;
    asynStatus vasynStatus;

    if(precord->pact) {  /* post prcoessing */

        switch(pGP307_read->command_from_postCallback) {
            case GP307CMD_ONDG:
            case GP307CMD_OFFDG:
            case GP307CMD_ONIG0:
            case GP307CMD_OFFIG0:
            case GP307CMD_ONIG1:
            case GP307CMD_OFFIG1:
                break;    /* nothing to do */
            default:
                /* disable record function forever */
                precord->pact = TRUE;
                precord->udf = TRUE;
                return 0;
        }

        precord->udf =FALSE;
        return 2;  /* don't do any conversion */

    }

    /* pre-prcoessing */
    precord->pact = TRUE;

    epicsMutexLock(pdrvGP307Config->lock);
    pdrvGP307Config->cbCount++;
    pBufGP307_read->status = pGP307_read->status;
    epicsMutexUnlock(pdrvGP307Config->lock);
    
    pdrvGP307CallbackQueueBuf->userPvt = (void*)pdrvGP307Config;
    pdrvGP307CallbackQueueBuf->precord = (dbCommon*) precord;

    switch(pdevGP307dpvt->param) {
        case GP307DEVPARAM_DGONOFF:
            if(precord->rval) pdrvGP307CallbackQueueBuf->command = GP307CMD_ONDG;
            else             pdrvGP307CallbackQueueBuf->command = GP307CMD_OFFDG;
            break;
        case GP307DEVPARAM_IG0ONOFF:
            if(precord->rval) pdrvGP307CallbackQueueBuf->command = GP307CMD_ONIG0;
            else             pdrvGP307CallbackQueueBuf->command = GP307CMD_OFFIG0;
            break;
        case GP307DEVPARAM_IG1ONOFF:
            if(precord->rval) pdrvGP307CallbackQueueBuf->command = GP307CMD_ONIG1;
            else             pdrvGP307CallbackQueueBuf->command = GP307CMD_OFFIG1;
            break;
        default:
            /* disable record function forever */
            precord->pact = TRUE;
            precord->udf = TRUE;
            return 0;
    }

    /* put the callback queue */
    nbytes = epicsRingBytesPut(pdrvGP307Config->queueCallbackRingBytesId,
                               (char*)pdrvGP307CallbackQueueBuf,
                               sizeof(drvGP307CallbackQueueBuf)); 
    vasynStatus = pasynManager->queueRequest(pdrvGP307Config->pasynGP307User, asynQueuePriorityLow, pdrvGP307Config->cbTimeout);
    if(vasynStatus == asynError) {
        epicsMutexLock(pdrvGP307Config->lock);
        pdrvGP307Config->asynRequestLostCount++;
        epicsMutexUnlock(pdrvGP307Config->lock);
    }
    return 0;
}


LOCAL long devMbbiGP307_init_record(mbbiRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)malloc(sizeof(devGP307dpvt));
    int index = 0;

    switch(precord->inp.type) {
        case (INST_IO):
            sscanf(precord->inp.value.instio.string,
                   "%s %s",
                   pdevGP307dpvt->port_name, pdevGP307dpvt->param_str);
            pdevGP307dpvt->pdrvGP307Config = drvGP307_find(pdevGP307dpvt->port_name);
            if(!pdevGP307dpvt->pdrvGP307Config) goto end;
            while((pdevGP307tParam+index)->param_str) {
                if(!strcmp((pdevGP307tParam+index)->param_str, pdevGP307dpvt->param_str)) break;
                index++;
            }
            pdevGP307dpvt->param = (pdevGP307tParam+index)->param;
            if(pdevGP307dpvt->param < 0) goto end;
            if(pdevGP307dpvt->param == GP307DEVPARAM_READDGS ||
               pdevGP307dpvt->param == GP307DEVPARAM_READPCS)
                break;
               
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devMbbiGP307 (init_record) Illegal INP field");
            free(pdevGP307dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;

    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevGP307dpvt;

    return 0;
}


LOCAL long devMbbiGP307_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;

    *ioScanPvt = pdrvGP307Config->ioScanPvt;

    return 0;
}


LOCAL long devMbbiGP307_read_mbbi(mbbiRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*)pdrvGP307Config->pGP307_read;
    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;
    asynStatus vasynStatus;

    if(precord->pact) { /* post processing */
        switch(pGP307_read->command_from_postCallback) {
            case GP307CMD_DGS:
                epicsMutexLock(pdrvGP307Config->lock);
                precord->rval = (pGP307_read->status & GP307STATUS_DGONOFF_MASK)?((unsigned long)0x01):((unsigned long)0x00);
                epicsMutexUnlock(pdrvGP307Config->lock);
                break;
            case GP307CMD_READPCS:
                epicsMutexLock(pdrvGP307Config->lock);
                precord->rval = (unsigned long) pGP307_read->pcs;
                epicsMutexUnlock(pdrvGP307Config->lock);
                break;
            default:
                /* disable record function forever */
                precord->pact = TRUE;
                precord->udf = TRUE;
                return 0;
        }
        precord->val = precord->rval;
        precord->udf = FALSE;
        return 2;       /* don't do any conversion */
    }

    /* pre-processing */
    precord->pact = TRUE;

    epicsMutexLock(pdrvGP307Config->lock);
    pdrvGP307Config->cbCount++;
    pBufGP307_read->status = pGP307_read->status;
    epicsMutexUnlock(pdrvGP307Config->lock);

    pdrvGP307CallbackQueueBuf->userPvt = (void*)pdrvGP307Config;
    pdrvGP307CallbackQueueBuf->precord = (dbCommon*)precord;

    switch(pdevGP307dpvt->param) {
        case GP307DEVPARAM_READDGS:
            pdrvGP307CallbackQueueBuf->command = GP307CMD_DGS;
            break;
        case GP307DEVPARAM_READPCS:
            pdrvGP307CallbackQueueBuf->command = GP307CMD_READPCS;
            break;
        default:
            /* disable record function forever */
            precord->pact = TRUE;
            precord->udf = TRUE;
            return 0;
    }

    /* put the callback queue */
    nbytes = epicsRingBytesPut(pdrvGP307Config->queueCallbackRingBytesId,
                               (char*)pdrvGP307CallbackQueueBuf,
                                sizeof(drvGP307CallbackQueueBuf));
    vasynStatus = pasynManager->queueRequest(pdrvGP307Config->pasynGP307User, asynQueuePriorityLow, pdrvGP307Config->cbTimeout);
    if(vasynStatus == asynError) {
        epicsMutexLock(pdrvGP307Config->lock);
        pdrvGP307Config->asynRequestLostCount++;
        epicsMutexUnlock(pdrvGP307Config->lock);
    }
    return 0; 
}


LOCAL long devSiGP307_init_record(stringinRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)malloc(sizeof(devGP307dpvt));
    int index =0;

    switch(precord->inp.type) {
        case(INST_IO):
            sscanf(precord->inp.value.instio.string,
                   "%s %s",
                   pdevGP307dpvt->port_name, pdevGP307dpvt->param_str);
            pdevGP307dpvt->pdrvGP307Config = drvGP307_find(pdevGP307dpvt->port_name);
            if(!pdevGP307dpvt->pdrvGP307Config) goto end;
            while((pdevGP307tParam+index)->param_str) {
                if(!strcmp((pdevGP307tParam+index)->param_str, pdevGP307dpvt->param_str)) break;
                index++;
            }
            pdevGP307dpvt->param = (pdevGP307tParam+index)->param;
            if(pdevGP307dpvt->param < 0) goto end;
            if(pdevGP307dpvt->param == GP307DEVPARAM_STATUSCG0 ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSCG1 ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSIG ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSTIMEOUT ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSONOFFIG0 ||
               pdevGP307dpvt->param == GP307DEVPARAM_STATUSONOFFIG1)
                break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devSiGP307 (init_record) Illegal INP field");
            free(pdevGP307dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevGP307dpvt;

    return 0;

}


LOCAL long devSiGP307_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;

    *ioScanPvt = pdrvGP307Config->ioScanPvt;

    return 0;
}


LOCAL long devSiGP307_read_si(stringinRecord *precord)
{
    devGP307dpvt *pdevGP307dpvt = (devGP307dpvt*)precord->dpvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*)pdevGP307dpvt->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*) pdrvGP307Config->pGP307_read;
    int index;

    switch(pdevGP307dpvt->param) {
        case GP307DEVPARAM_STATUSCG0:
            epicsMutexLock(pdrvGP307Config->lock);
            if(pGP307_read->status & GP307STATUS_CG0VALID_MASK) index = GP307_STR_VALID;
            else                                                index = GP307_STR_INVALID;
            epicsMutexUnlock(pdrvGP307Config->lock);
            strcpy(precord->val, tValidInvalidList[(unsigned)vValidInvalidList[index]]);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_STATUSCG1:
            epicsMutexLock(pdrvGP307Config->lock);
            if(pGP307_read->status & GP307STATUS_CG1VALID_MASK) index = GP307_STR_VALID;
            else                                                index = GP307_STR_INVALID;
            epicsMutexUnlock(pdrvGP307Config->lock);
            strcpy(precord->val, tValidInvalidList[(unsigned)vValidInvalidList[index]]);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_STATUSIG:
            epicsMutexLock(pdrvGP307Config->lock);
            if(pGP307_read->status & GP307STATUS_IGVALID_MASK) index = GP307_STR_VALID;
            else                                               index = GP307_STR_INVALID;
            epicsMutexUnlock(pdrvGP307Config->lock);
            strcpy(precord->val, tValidInvalidList[(unsigned)vValidInvalidList[index]]);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_STATUSTIMEOUT:
            epicsMutexLock(pdrvGP307Config->lock);
            if(pGP307_read->status & GP307STATUS_TIMEOUT_MASK) index = GP307_STR_TIMEOUT;
            else                                               index = GP307_STR_OK;
            epicsMutexUnlock(pdrvGP307Config->lock);
            strcpy(precord->val, tTimeoutOkList[(unsigned)vTimeoutOkList[index]]);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_STATUSONOFFIG0:
            epicsMutexLock(pdrvGP307Config->lock);
            if(pGP307_read->status & GP307STATUS_IG0ONOFF_MASK) index = GP307_STR_ON;
            else                                                index = GP307_STR_OFF;
            epicsMutexUnlock(pdrvGP307Config->lock);
            strcpy(precord->val, tOnOffList[(unsigned)vOnOffList[index]]);
            precord->udf = FALSE;
            break;
        case GP307DEVPARAM_STATUSONOFFIG1:
            epicsMutexLock(pdrvGP307Config->lock);
            if(pGP307_read->status & GP307STATUS_IG1ONOFF_MASK) index = GP307_STR_ON;
            else                                                index = GP307_STR_OFF;
            epicsMutexUnlock(pdrvGP307Config->lock);
            strcpy(precord->val, tOnOffList[(unsigned)vOnOffList[index]]);
            precord->udf = FALSE;
            break;

    }

    return 2; /* don't do any conversion */
}



