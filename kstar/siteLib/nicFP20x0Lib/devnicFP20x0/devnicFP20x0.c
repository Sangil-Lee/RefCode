





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
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "timestampRecord.h"

#include "epicsExport.h"

#include "drvnicFP20x0.h"


#define cFP20x0DEVPARAM_STATUS                           0
#define cFP20x0DEVPARAM_READ                             1
#define cFP20x0DEVPARAM_WRITE                            2
#define cFP20x0DEVPARAM_SCANSTATUS                       3

#define cFP20x0DEVPARAM_TERM                            -1

#define cFP20x0DEVPARAM_STATUS_NUMSUBMOD                 0
#define cFP20x0DEVPARAM_STATUS_CBCOUNT                   1
#define cFP20x0DEVPARAM_STATUS_TIMEOUTCOUNT              2
#define cFP20x0DEVPARAM_STATUS_RECONNCOUNT               3
#define cFP20x0DEVPARAM_STATUS_POSTREADFAILCOUNT         4
#define cFP20x0DEVPARAM_STATUS_ASYNSCANLOSTCOUNT         5
#define cFP20x0DEVPARAM_STATUS_ASYNREQUESTLOSTCOUNT      6
#define cFP20x0DEVPARAM_STATUS_FREEQUEUECBRNGBUF         7
#define cFP20x0DEVPARAM_STATUS_FREEPOSTCBRNGBUF          8
#define cFP20x0DEVPARAM_STATUS_FREEQUEUECBRNGEVT         9
#define cFP20x0DEVPARAM_STATUS_FREEPOSTCBRNGEVT          10
#define cFP20x0DEVPARAM_STATUS_SCANRATE                  11
#define cFP20x0DEVPARAM_STATUS_TIMESTAMP                 12

#define cFP20x0DEVPARAM_STR_STATUS                    "status"
#define cFP20x0DEVPARAM_STR_READ                      "read"
#define cFP20x0DEVPARAM_STR_WRITE                     "write"
#define cFP20x0DEVPARAM_STR_SCANSTATUS                "scanStatus"

#define cFP20x0DEVPARAM_STR_TERM                       NULL

#define cFP20x0DEVPARAM_STR_STATUS_NUMSUBMOD           "numSubMod"
#define cFP20x0DEVPARAM_STR_STATUS_CBCOUNT             "cbCount"
#define cFP20x0DEVPARAM_STR_STATUS_TIMEOUTCOUNT        "timeoutCount"
#define cFP20x0DEVPARAM_STR_STATUS_RECONNCOUNT         "reconnCount"
#define cFP20x0DEVPARAM_STR_STATUS_POSTREADFAILCOUNT   "postReadFailCount"
#define cFP20x0DEVPARAM_STR_STATUS_ASYNSCANLOSTCOUNT   "asynScanLostCount"
#define cFP20x0DEVPARAM_STR_STATUS_ASYNREQUESTLOSTCOUNT "asynRequestLostCount"
#define cFP20x0DEVPARAM_STR_STATUS_FREEQUEUECBRNGBUF    "freeQueueCbRngBuf"
#define cFP20x0DEVPARAM_STR_STATUS_FREEPOSTCBRNGBUF     "freePostCbRngBuf"
#define cFP20x0DEVPARAM_STR_STATUS_FREEQUEUECBRNGEVT    "freeQueueCbRngEvt"
#define cFP20x0DEVPARAM_STR_STATUS_FREEPOSTCBRNGEVT     "freePostCbRngEvt"
#define cFP20x0DEVPARAM_STR_STATUS_SCANRATE             "scanRate"
#define cFP20x0DEVPARAM_STR_STATUS_TIMESTAMP            "timestamp"



typedef struct {
    char port_name[40];
    char param_str_first[40];
    char param_str_second[40];
    int param_first;
    int param_second;
    char modLabel[40];
    int channel;
    char commandHeader[40];
    drvcFP20x0Config *pdrvcFP20x0Config;
    cFP20x0Module *pcFP20x0Module;
} devcFP20x0dpvt;


typedef struct {
    char *param_str;
    int param;
} devcFP20x0tParam;


LOCAL devcFP20x0tParam pdevcFP20x0tParamFirst[] = {
    {cFP20x0DEVPARAM_STR_STATUS,    cFP20x0DEVPARAM_STATUS},
    {cFP20x0DEVPARAM_STR_READ,      cFP20x0DEVPARAM_READ},
    {cFP20x0DEVPARAM_STR_WRITE,     cFP20x0DEVPARAM_WRITE},
    {cFP20x0DEVPARAM_STR_SCANSTATUS, cFP20x0DEVPARAM_SCANSTATUS},
    {cFP20x0DEVPARAM_STR_TERM,      cFP20x0DEVPARAM_TERM},
};

LOCAL devcFP20x0tParam pdevcFP20x0tParamSecond[] = {
    {cFP20x0DEVPARAM_STR_STATUS_NUMSUBMOD,            cFP20x0DEVPARAM_STATUS_NUMSUBMOD},
    {cFP20x0DEVPARAM_STR_STATUS_CBCOUNT,              cFP20x0DEVPARAM_STATUS_CBCOUNT},
    {cFP20x0DEVPARAM_STR_STATUS_TIMEOUTCOUNT,         cFP20x0DEVPARAM_STATUS_TIMEOUTCOUNT},
    {cFP20x0DEVPARAM_STR_STATUS_RECONNCOUNT,          cFP20x0DEVPARAM_STATUS_RECONNCOUNT},
    {cFP20x0DEVPARAM_STR_STATUS_POSTREADFAILCOUNT,    cFP20x0DEVPARAM_STATUS_POSTREADFAILCOUNT},
    {cFP20x0DEVPARAM_STR_STATUS_ASYNSCANLOSTCOUNT,    cFP20x0DEVPARAM_STATUS_ASYNSCANLOSTCOUNT},
    {cFP20x0DEVPARAM_STR_STATUS_ASYNREQUESTLOSTCOUNT, cFP20x0DEVPARAM_STATUS_ASYNREQUESTLOSTCOUNT},
    {cFP20x0DEVPARAM_STR_STATUS_FREEQUEUECBRNGBUF,    cFP20x0DEVPARAM_STATUS_FREEQUEUECBRNGBUF},
    {cFP20x0DEVPARAM_STR_STATUS_FREEPOSTCBRNGBUF,     cFP20x0DEVPARAM_STATUS_FREEPOSTCBRNGBUF},
    {cFP20x0DEVPARAM_STR_STATUS_FREEQUEUECBRNGEVT,    cFP20x0DEVPARAM_STATUS_FREEQUEUECBRNGEVT},
    {cFP20x0DEVPARAM_STR_STATUS_FREEPOSTCBRNGBUF,     cFP20x0DEVPARAM_STATUS_FREEPOSTCBRNGEVT},
    {cFP20x0DEVPARAM_STR_STATUS_SCANRATE,             cFP20x0DEVPARAM_STATUS_SCANRATE},
    {cFP20x0DEVPARAM_STR_STATUS_TIMESTAMP,            cFP20x0DEVPARAM_STATUS_TIMESTAMP},

    {cFP20x0DEVPARAM_STR_TERM,                     cFP20x0DEVPARAM_TERM}
};


LOCAL char *devcFP20x0ChnStr[] = { "01", "02", "03", "04", "05", "06", "07", "08",
                                   "09", "10", "11", "12", "13", "14", "15", "16",
                                   "17", "18", "19", "20", "21", "22", "23", "24",
                                   "25", "26", "27", "28", "29", "30", "31", "32" };


extern drvcFP20x0Config *drvcFP20x0_find(char *portName);
extern cFP20x0Module *drvcFP20x0_modFind(char *modLabel, drvcFP20x0Config *pdrvcFP20x0Config);
extern void drvcFP20x0_requestWriteDev(drvcFP20x0Config *pdrvcFP20x0Config, char *commandBuf, dbCommon *precord);

LOCAL void          devAicFP20x0_get_status(aiRecord *precord);
LOCAL double        devAicFP20x0_status_get_scanRate(dbCommon *precord);
LOCAL unsigned char devAicFP20x0_status_get_cbCount(dbCommon *precord);
LOCAL unsigned int  devAicFP20x0_status_get_timeoutCount(dbCommon *precord);
LOCAL unsigned int  devAicFP20x0_status_get_reconnCount(dbCommon *precord);
LOCAL unsigned int  devAicFP20x0_status_get_postReadFailCount(dbCommon *precord);
LOCAL unsigned long devAicFP20x0_status_get_asynScanLostCount(dbCommon *precord);
LOCAL unsigned long devAicFP20x0_status_get_asynRequestLostCount(dbCommon *precord);
LOCAL int           devAicFP20x0_status_get_freeQueueCallbackRingBuf(dbCommon *precord);
LOCAL int           devAicFP20x0_status_get_freeQueuePostCallbackRingBuf(dbCommon *precord);
LOCAL int           devAicFP20x0_status_get_freeQueueCallbackRingEvent(dbCommon *precord);
LOCAL int           devAicFP20x0_status_get_freeQueuePostCallbackRingEvent(dbCommon *precord);

LOCAL void           devAicFP20x0_get_scanstatus(aiRecord *precord);
LOCAL unsigned long  devAicFP20x0_scanstatus_get_scanCount(dbCommon *precord);

LOCAL void          devAicFP20x0_get_read(aiRecord *precord);
LOCAL double        devAicFP20x0_read_get_Analog(dbCommon *precord);

LOCAL void          devBicFP20x0_get_read(biRecord *precord);
LOCAL short         devBicFP20x0_read_get_Binary(dbCommon *precord);

LOCAL void          devLicFP20x0_get_read(longinRecord *precord);
LOCAL long          devLicFP20x0_read_get_Long(dbCommon *precord);


LOCAL long devTimestampcFP20x0_init_record(timestampRecord *precord);
LOCAL long devTimestampcFP20x0_get_ioint_info(int cmd, timestampRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devTimestampcFP20x0_read_timestamp(timestampRecord *precord);

struct {
    long       number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_timestamp;
} devTimestampcFP20x0 = {
    5,
    NULL,
    NULL,
    devTimestampcFP20x0_init_record,
    devTimestampcFP20x0_get_ioint_info,
    devTimestampcFP20x0_read_timestamp
};

epicsExportAddress(dset, devTimestampcFP20x0);


LOCAL long devAicFP20x0_init_record(aiRecord *precord);
LOCAL long devAicFP20x0_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAicFP20x0_read_ai(aiRecord *precord);

struct {
    long        number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_ai;
    DEVSUPFUN    special_linconv;
} devAicFP20x0 = {
    6,
    NULL,
    NULL,
    devAicFP20x0_init_record,
    devAicFP20x0_get_ioint_info,
    devAicFP20x0_read_ai,
    NULL
};

epicsExportAddress(dset, devAicFP20x0);


LOCAL long devAocFP20x0_init_record(aoRecord *precord);
LOCAL long devAocFP20x0_write_ao(aoRecord *precord);

struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_ao;
    DEVSUPFUN    special_linconv;
}  devAocFP20x0 = {
    6,
    NULL,
    NULL,
    devAocFP20x0_init_record,
    NULL,
    devAocFP20x0_write_ao,
    NULL
};

epicsExportAddress(dset, devAocFP20x0);


LOCAL long devBicFP20x0_init_record(biRecord *precord);
LOCAL long devBicFP20x0_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devBicFP20x0_read_bi(biRecord *precord);

struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_bi;
} devBicFP20x0 = {
    5, 
    NULL,
    NULL,
    devBicFP20x0_init_record,
    devBicFP20x0_get_ioint_info,
    devBicFP20x0_read_bi
};

epicsExportAddress(dset, devBicFP20x0);



LOCAL long devBocFP20x0_init_record(boRecord *precord);
LOCAL long devBocFP20x0_write_bo(boRecord *precord);

struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBocFP20x0 = {
    5,
    NULL,
    NULL,
    devBocFP20x0_init_record,
    NULL,
    devBocFP20x0_write_bo
};

epicsExportAddress(dset, devBocFP20x0);


LOCAL long devLicFP20x0_init_record(longinRecord *precord);
LOCAL long devLicFP20x0_get_ioint_info(int cmd, longinRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devLicFP20x0_read_li(longinRecord *precord);
struct {
    long    number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_li;
} devLicFP20x0 = {
    5,
    NULL,
    NULL,
    devLicFP20x0_init_record,
    devLicFP20x0_get_ioint_info,
    devLicFP20x0_read_li
};

epicsExportAddress(dset, devLicFP20x0);



LOCAL long devLocFP20x0_init_record(longoutRecord *precord);
LOCAL long devLocFP20x0_write_lo(longoutRecord *precord);

struct {
    long    number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    int_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_lo;
} devLocFP20x0 = {
    5,
    NULL,
    NULL,
    devLocFP20x0_init_record,
    NULL,
    devLocFP20x0_write_lo
};

epicsExportAddress(dset, devLocFP20x0);


LOCAL void devAicFP20x0_get_status(aiRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    
    switch(pdevcFP20x0dpvt->param_second) {
        case cFP20x0DEVPARAM_STATUS_NUMSUBMOD:
        case cFP20x0DEVPARAM_STATUS_CBCOUNT:
            precord->val = devAicFP20x0_status_get_cbCount((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_TIMEOUTCOUNT:
            precord->val = devAicFP20x0_status_get_timeoutCount((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_RECONNCOUNT:
            precord->val = devAicFP20x0_status_get_reconnCount((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_POSTREADFAILCOUNT:
            precord->val = devAicFP20x0_status_get_postReadFailCount((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_ASYNSCANLOSTCOUNT:
            precord->val = devAicFP20x0_status_get_asynScanLostCount((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_ASYNREQUESTLOSTCOUNT:
            precord->val = devAicFP20x0_status_get_asynRequestLostCount((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_FREEQUEUECBRNGBUF:
            precord->val = devAicFP20x0_status_get_freeQueueCallbackRingBuf((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_FREEPOSTCBRNGBUF:
            precord->val = devAicFP20x0_status_get_freeQueuePostCallbackRingBuf((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_FREEQUEUECBRNGEVT:
            precord->val = devAicFP20x0_status_get_freeQueueCallbackRingEvent((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_FREEPOSTCBRNGEVT:
            precord->val = devAicFP20x0_status_get_freeQueuePostCallbackRingEvent((dbCommon*)precord); break;
        case cFP20x0DEVPARAM_STATUS_SCANRATE:
            precord->val = devAicFP20x0_status_get_scanRate(precord); break;
    }

}
LOCAL double  devAicFP20x0_status_get_scanRate(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    double scanRate;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        scanRate = pdrvcFP20x0Config->scanRate;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return scanRate;
}

LOCAL unsigned char devAicFP20x0_status_get_cbCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    unsigned char cbCount;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        cbCount = pdrvcFP20x0Config->cbCount;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return cbCount;
}

LOCAL unsigned int devAicFP20x0_status_get_timeoutCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    unsigned int timeoutCount;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        timeoutCount = pdrvcFP20x0Config->timeoutCount;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return timeoutCount;
}

LOCAL unsigned int devAicFP20x0_status_get_reconnCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    unsigned int  reconnCount;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        reconnCount = pdrvcFP20x0Config->reconnCount;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return reconnCount;
}

LOCAL unsigned int devAicFP20x0_status_get_postReadFailCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    unsigned int postReadFailCount;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        postReadFailCount = pdrvcFP20x0Config->postReadFailCount;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return postReadFailCount;
}

LOCAL unsigned long devAicFP20x0_status_get_asynScanLostCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    unsigned long asynScanLostCount;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        asynScanLostCount = pdrvcFP20x0Config->asynScanLostCount;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return asynScanLostCount;

}

LOCAL unsigned long devAicFP20x0_status_get_asynRequestLostCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    unsigned long asynRequestLostCount;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        asynRequestLostCount = pdrvcFP20x0Config->asynRequestLostCount;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return asynRequestLostCount;
}

LOCAL int devAicFP20x0_status_get_freeQueueCallbackRingBuf(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    int nbytes;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        nbytes = epicsRingBytesFreeBytes(pdrvcFP20x0Config->queueCallbackRingBytesId);
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return nbytes;
}

LOCAL int devAicFP20x0_status_get_freeQueuePostCallbackRingBuf(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    int nbytes;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        nbytes = epicsRingBytesFreeBytes(pdrvcFP20x0Config->postCallbackRingBytesId);
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return nbytes;
}

LOCAL int devAicFP20x0_status_get_freeQueueCallbackRingEvent(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    int events;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        events = epicsRingBytesFreeBytes(pdrvcFP20x0Config->queueCallbackRingBytesId)/sizeof(drvcFP20x0CallbackQueueBuf);
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return events;
}

LOCAL int devAicFP20x0_status_get_freeQueuePostCallbackRingEvent(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    int events;

    epicsMutexLock(pdrvcFP20x0Config->lock);
        events = epicsRingBytesFreeBytes(pdrvcFP20x0Config->postCallbackRingBytesId)/sizeof(drvcFP20x0CallbackQueueBuf);
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return events;
}



LOCAL void devAicFP20x0_get_scanstatus(aiRecord *precord)
{
    precord->val = devAicFP20x0_scanstatus_get_scanCount((dbCommon*) precord);
}



LOCAL unsigned long devAicFP20x0_scanstatus_get_scanCount(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    cFP20x0Module *pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;
    unsigned long val;

    epicsMutexLock(pcFP20x0Module->lock);
        switch(pcFP20x0Module->modType) {
            case cFP20x0_modType_DI8:
                 val = ((cFP20x0_modConfig_DI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_DI16:
                 val = ((cFP20x0_modConfig_DI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_DI32:
                 val = ((cFP20x0_modConfig_DI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_DO8:
                 val = ((cFP20x0_modConfig_DO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_DO16:
                 val = ((cFP20x0_modConfig_DO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_DO32:
                 val = ((cFP20x0_modConfig_DO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AI2:
                 val = ((cFP20x0_modConfig_AI2*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AI8:
                 val = ((cFP20x0_modConfig_AI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AI16:
                 val = ((cFP20x0_modConfig_AI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AI32:
                 val = ((cFP20x0_modConfig_AI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AO2:
                 val = ((cFP20x0_modConfig_AO2*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AO8:
                 val = ((cFP20x0_modConfig_AO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AO16:
                 val = ((cFP20x0_modConfig_AO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
            case cFP20x0_modType_AO32:
                 val = ((cFP20x0_modConfig_AO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->scanCnt[pdevcFP20x0dpvt->channel];
                 break;
        }
    epicsMutexUnlock(pcFP20x0Module->lock);
 
    return val;
}


LOCAL void devAicFP20x0_get_read(aiRecord *precord)
{
    precord->val = devAicFP20x0_read_get_Analog((dbCommon*) precord);
}

LOCAL double devAicFP20x0_read_get_Analog(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    cFP20x0Module *pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;
    double val;

    epicsMutexLock(pcFP20x0Module->lock);
        switch(pcFP20x0Module->modType) {
            case cFP20x0_modType_AI2:
                val = ((cFP20x0_modConfig_AI2*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AI8:
                val = ((cFP20x0_modConfig_AI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AI16:
                val = ((cFP20x0_modConfig_AI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AI32:
                val = ((cFP20x0_modConfig_AI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AO2:
                val = ((cFP20x0_modConfig_AO2*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AO8:
                val = ((cFP20x0_modConfig_AO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AO16:
                val = ((cFP20x0_modConfig_AO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
            case cFP20x0_modType_AO32:
                val = ((cFP20x0_modConfig_AO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->data[pdevcFP20x0dpvt->channel];
                break;
 
        }
    epicsMutexUnlock(pcFP20x0Module->lock);

    return val;
}



LOCAL void  devBicFP20x0_get_read(biRecord *precord)
{
    precord->rval = devBicFP20x0_read_get_Binary((dbCommon*) precord);
}

LOCAL short devBicFP20x0_read_get_Binary(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    cFP20x0Module *pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;
    epicsUInt8   mask_8 = 0x01;
    epicsUInt16  mask_16 = 0x0001;
    epicsUInt32  mask_32 = 0x00000001;
    short val;
    

    epicsMutexLock(pcFP20x0Module->lock);
        switch(pcFP20x0Module->modType) {
            case cFP20x0_modType_DI8:
                mask_8 <<= pdevcFP20x0dpvt->channel;
                if(((cFP20x0_modConfig_DI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->data & mask_8) val = 0x0001;
                else                                                                               val = 0x0000;
                break;
            case cFP20x0_modType_DI16:
                mask_16 <<= pdevcFP20x0dpvt->channel;
                if(((cFP20x0_modConfig_DI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->data & mask_16) val = 0x0001;
                else                                                                                 val = 0x0000;
                break;
            case cFP20x0_modType_DI32:
                mask_32 <<= pdevcFP20x0dpvt->channel;
                if(((cFP20x0_modConfig_DI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->data & mask_32) val = 0x0001;
                else                                                                                 val = 0x0000;
                break;
            case cFP20x0_modType_DO8:
                mask_8 <<= pdevcFP20x0dpvt->channel;
                if(((cFP20x0_modConfig_DO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->data & mask_8) val = 0x0001;
                else                                                                               val = 0x0000;
                break;
            case cFP20x0_modType_DO16:
                mask_16 <<= pdevcFP20x0dpvt->channel;
                if(((cFP20x0_modConfig_DO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->data & mask_16) val = 0x0001;
                else                                                                                 val = 0x0000;
                break;
            case cFP20x0_modType_DO32:
                mask_32 <<= pdevcFP20x0dpvt->channel;
                if(((cFP20x0_modConfig_DO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->data & mask_32) val = 0x0001;
                else                                                                                 val = 0x0000;
                break;
        }
    epicsMutexUnlock(pcFP20x0Module->lock);

    return val;
}


LOCAL void devLicFP20x0_get_read(longinRecord *precord)
{
    precord->val = devLicFP20x0_read_get_Long((dbCommon*) precord);
}

LOCAL long devLicFP20x0_read_get_Long(dbCommon *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    cFP20x0Module *pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;
    long val;

    epicsMutexLock(pcFP20x0Module->lock);
        switch(pcFP20x0Module->modType) {
            case cFP20x0_modType_DI8:
                val = (long) ((cFP20x0_modConfig_DI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->data;
                break;
            case cFP20x0_modType_DI16:
                val = (long) ((cFP20x0_modConfig_DI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->data;
                break;
            case cFP20x0_modType_DI32:
                val = (long) ((cFP20x0_modConfig_DI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->data;
                break;
            case cFP20x0_modType_DO8:
                val = (long) ((cFP20x0_modConfig_DO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->data;
                break;
            case cFP20x0_modType_DO16:
                val = (long) ((cFP20x0_modConfig_DO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->data;
                break;
            case cFP20x0_modType_DO32:
                val = (long) ((cFP20x0_modConfig_DO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->data;
                break;
        }
    epicsMutexUnlock(pcFP20x0Module->lock);

    return val;
}



LOCAL long devTimestampcFP20x0_init_record(timestampRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc(sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->inp.type) {
        case INST_IO:
            sscanf(precord->inp.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) {  /* port not found */
                recGblRecordError(S_db_badField, (void*) precord,
                                  "devTiestampcFP20x0 (init_record) Illegal INP field: port not found.");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }
 
            index = 0;                                           /* check up first param */
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first)) break;
                index++;
            } 
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;
            if(pdevcFP20x0dpvt->param_first != cFP20x0DEVPARAM_STATUS) {  /* It is not status */
                recGblRecordError(S_db_badField, (void*) precord,
                                  "devTimestampcFP20x0 (init_record) Illegal INP field: first param is not status.");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            index = 0;                                            /* check up second param */
            while((pdevcFP20x0tParamSecond+index)->param_str) { 
                if(!strcmp((pdevcFP20x0tParamSecond+index)->param_str, pdevcFP20x0dpvt->param_str_second)) break;
                index++;
            }
            pdevcFP20x0dpvt->param_second = (pdevcFP20x0tParamSecond+index)->param;
            if(pdevcFP20x0dpvt->param_second != cFP20x0DEVPARAM_STATUS_TIMESTAMP) { /* It is not timestamp */
                recGblRecordError(S_db_badField, (void*) precord,
                                  "devTimestampcFP20x0 (init_record) Illegal INP field: second param is not timestamp.");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }
            break;
        default: 
            end:    
            recGblRecordError(S_db_badField, (void*) precord, 
                              "devTimestampcFP20x0 (init_record) Illegal INP field");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevcFP20x0dpvt;

    return 0;
}


LOCAL long devTimestampcFP20x0_get_ioint_info(int cmd, timestampRecord *precord, IOSCANPVT *ioScanPvt)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config;

    if(!precord->dpvt) {
        *ioScanPvt = NULL;
        return 0; /* nothing to do */
    }

   pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
   pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;


    *ioScanPvt = pdrvcFP20x0Config->stampIoScanPvt;

    return 0;
}



LOCAL long devTimestampcFP20x0_read_timestamp(timestampRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;

    if(!precord->dpvt) return 0;  /* nothing to do  */

    epicsMutexLock(pdrvcFP20x0Config->lock);
    precord->time = pdrvcFP20x0Config->stamp;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    return 0;
}


LOCAL long devAicFP20x0_init_record(aiRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc(sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->inp.type) {
        case INST_IO:
            sscanf(precord->inp.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) { /* port not found */
                recGblRecordError(S_db_badField, (void *) precord,
                                  "devAicFP20x0 (init_record) Illegal INP field: port not found");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            index = 0;           /* check up first param */
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first)) break;
                index++;
            }
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;

            switch(pdevcFP20x0dpvt->param_first) {
                case cFP20x0DEVPARAM_STATUS:     /* first param is status */
                    index = 0;  /* check up second param */
                    while((pdevcFP20x0tParamSecond+index)->param_str) {
                        if(!strcmp((pdevcFP20x0tParamSecond+index)->param_str, pdevcFP20x0dpvt->param_str_second)) break;
                        index++;
                    } 
                    pdevcFP20x0dpvt->param_second = (pdevcFP20x0tParamSecond+index)->param;
                    if(pdevcFP20x0dpvt->param_second<0) {
                        recGblRecordError(S_db_badField, (void*) precord,
                                          "devAicFP20x0 (init_record) Illegal INP field. check second param");
                        free(pdevcFP20x0dpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;
                    }
                    break;
                case cFP20x0DEVPARAM_READ:
                case cFP20x0DEVPARAM_WRITE:      /* first param is read or write */
                case cFP20x0DEVPARAM_SCANSTATUS:
                    strncpy(pdevcFP20x0dpvt->modLabel, pdevcFP20x0dpvt->param_str_second, 3);
                    pdevcFP20x0dpvt->modLabel[3] = '\0';   /* append NULL terminator */
                    sscanf(pdevcFP20x0dpvt->param_str_second+5, "%d", &pdevcFP20x0dpvt->channel);
                    pdevcFP20x0dpvt->pcFP20x0Module = drvcFP20x0_modFind(pdevcFP20x0dpvt->modLabel, 
                                                                         pdevcFP20x0dpvt->pdrvcFP20x0Config);
                    if(!(pdevcFP20x0dpvt->pcFP20x0Module) ||                                 /* subMod not found */
                       pdevcFP20x0dpvt->channel >= pdevcFP20x0dpvt->pcFP20x0Module->numCh || /* mismatched channel number */
                       pdevcFP20x0dpvt->channel < 0) {                                       /* negative number */
                        recGblRecordError(S_db_badField, (void*) precord,
                                          "devAicFP20x0 (init_record) Illegal INP field, check second param");
                        free(pdevcFP20x0dpvt);
                        precord->dpvt = NULL;
                        return S_db_badField;
                    }
                    break;
                case cFP20x0DEVPARAM_TERM:       /* Illegal string in first param */
                default:
                    recGblRecordError(S_db_badField, (void*) precord,
                                      "devAicFP20x0 (init_record) Illegal INP field. check first param");
                     free(pdevcFP20x0dpvt);
                     precord->udf = TRUE;
                     precord->dpvt = NULL;
                     return S_db_badField;
            }

            break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devAicFP20x0 (init_record) Illegal INP field");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevcFP20x0dpvt;

    return 2;  /* don't convert */
}


LOCAL long devAicFP20x0_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config;
    cFP20x0Module *pcFP20x0Module;

    if(!precord->dpvt) {
        ioScanPvt = NULL;
        return 0;
    }

    pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;


    switch(pdevcFP20x0dpvt->param_first) {
        case cFP20x0DEVPARAM_STATUS:
            *ioScanPvt = pdrvcFP20x0Config->scanIoScanPvt;
            break;
        case cFP20x0DEVPARAM_READ:
        case cFP20x0DEVPARAM_WRITE:
        case cFP20x0DEVPARAM_SCANSTATUS:
            switch(pcFP20x0Module->modType) {
                case cFP20x0_modType_DI8:
                    *ioScanPvt = 
                        ((cFP20x0_modConfig_DI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_DI16:
                    *ioScanPvt = 
                        ((cFP20x0_modConfig_DI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_DI32:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_DI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_DO8:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_DO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_DO16:
                    *ioScanPvt = 
                        ((cFP20x0_modConfig_DO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_DO32:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_DO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                     break;
                case cFP20x0_modType_AI2:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_AI2*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                     break;
                case cFP20x0_modType_AI8:
                    *ioScanPvt = 
                        ((cFP20x0_modConfig_AI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_AI16:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_AI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_AI32:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_AI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_AO2:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_AO2*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_AO8:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_AO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_AO16:
                    *ioScanPvt =
                        ((cFP20x0_modConfig_AO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_AO32:
                    *ioScanPvt = 
                        ((cFP20x0_modConfig_AO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
                    break;
                case cFP20x0_modType_term:
                default:
                    *ioScanPvt = NULL;
                    return 0;
            }
            break;
        case cFP20x0DEVPARAM_TERM:
        default:
            ioScanPvt = NULL;
    }
    
    return 0;
}


LOCAL long devAicFP20x0_read_ai(aiRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;

    if(!pdevcFP20x0dpvt) {
        return 0;
    }

    switch(pdevcFP20x0dpvt->param_first) {
        case cFP20x0DEVPARAM_STATUS:
            devAicFP20x0_get_status(precord); break;
        case cFP20x0DEVPARAM_READ:
            devAicFP20x0_get_read(precord); break;
        case cFP20x0DEVPARAM_WRITE:
            break;
        case cFP20x0DEVPARAM_SCANSTATUS:
            devAicFP20x0_get_scanstatus(precord); break;
        case cFP20x0DEVPARAM_TERM: break;
        default: break;
    }

    return 2;  /* don't do any conversion */
}


LOCAL long devAocFP20x0_init_record(aoRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc (sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->out.type) {
        case INST_IO:
            sscanf(precord->out.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) {   /* port not found */
                recGblRecordError(S_db_badField, (void*) precord,
                                  "devAocFP20x0 (init_record) Illegal INP field: port not found");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }
            index = 0;
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first)) break;
                index++;
            }
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;

            switch(pdevcFP20x0dpvt->param_first) {
                case cFP20x0DEVPARAM_WRITE:
                    strncpy(pdevcFP20x0dpvt->modLabel, pdevcFP20x0dpvt->param_str_second, 3);
                    pdevcFP20x0dpvt->modLabel[3] = '\0';   /* append NULL terminator */
                    sscanf(pdevcFP20x0dpvt->param_str_second+5, "%d", &pdevcFP20x0dpvt->channel);
                    pdevcFP20x0dpvt->pcFP20x0Module = drvcFP20x0_modFind(pdevcFP20x0dpvt->modLabel,
                                                                         pdevcFP20x0dpvt->pdrvcFP20x0Config);
                    if(!(pdevcFP20x0dpvt->pcFP20x0Module) ||                        /* subMod not found */
                       pdevcFP20x0dpvt->channel >= pdevcFP20x0dpvt->pcFP20x0Module->numCh || /* mismatched channel number */
                       pdevcFP20x0dpvt->channel < 0 ) {                             /* negative channel number */
                           recGblRecordError(S_db_badField, (void*) precord,
                                             "devAocFP20x0 (init_record) Illegal OUT field, check second param");
                           free(pdevcFP20x0dpvt);
                           precord->udf = TRUE;
                           precord->dpvt = NULL;
                           return S_db_badField;
                    }
                    /* make command header */
                    sprintf(pdevcFP20x0dpvt->commandHeader, "W%sCH%s", pdevcFP20x0dpvt->modLabel, 
                                                                       devcFP20x0ChnStr[pdevcFP20x0dpvt->channel]);
                    pdevcFP20x0dpvt->commandHeader[3] += (char)0x01;
                    break;    /* end of cFP20x0DEVPARAM_WRITE */
                case cFP20x0DEVPARAM_STATUS:
                case cFP20x0DEVPARAM_READ:
                case cFP20x0DEVPARAM_SCANSTATUS:
                case cFP20x0DEVPARAM_TERM:
                default:
                    recGblRecordError(S_db_badField, (void*) precord,
                                      "devAocFP20x0 (init_reocrd) Illegal OUT field. check first param");
                    free(pdevcFP20x0dpvt);
                    precord->udf = TRUE;
                    precord->dpvt = NULL;
                    
                    return S_db_badField;
            }
            break;    /* end of inst_io */
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devAocFP20x0 (init_record) Illegal OUT field");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevcFP20x0dpvt;
    return 2;    /* don't convert */
}



LOCAL long devAocFP20x0_write_ao(aoRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = pdevcFP20x0dpvt->pdrvcFP20x0Config;
    char commandBuf[40];

    if(!pdevcFP20x0dpvt || precord->udf == TRUE) {
        precord->pact = TRUE;
        return 0;
    }

    if(precord->pact == FALSE) {
        precord->pact = TRUE;
        sprintf(commandBuf, "%sV%f%s", pdevcFP20x0dpvt->commandHeader, precord->val, EOS_STR);
        drvcFP20x0_requestWriteDev(pdrvcFP20x0Config, commandBuf, (dbCommon*) precord);
        return 0;
    }


    /* nothing to do for post processing */
    precord->pact = FALSE;

    return 2;  /* don't convert */
}



LOCAL long devBicFP20x0_init_record(biRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc(sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->inp.type) {
        case INST_IO:
            sscanf(precord->inp.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) { /* port not found */
                recGblRecordError(S_db_badField, (void*)precord,
                                  "devBicFP20x0 (init_record) Illegal INP field: port not found (1)");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            index = 0;
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first)) break;
                index++;
            }
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;

            switch(pdevcFP20x0dpvt->param_first) {
                case cFP20x0DEVPARAM_READ:
                case cFP20x0DEVPARAM_WRITE:
                    strncpy(pdevcFP20x0dpvt->modLabel, pdevcFP20x0dpvt->param_str_second, 3);
                    pdevcFP20x0dpvt->modLabel[3] = '\0';   /* append NULL terminator */
                    sscanf(pdevcFP20x0dpvt->param_str_second+5, "%d", &pdevcFP20x0dpvt->channel);
                    pdevcFP20x0dpvt->pcFP20x0Module = drvcFP20x0_modFind(pdevcFP20x0dpvt->modLabel,
                                                                         pdevcFP20x0dpvt->pdrvcFP20x0Config);
                    if(!(pdevcFP20x0dpvt->pcFP20x0Module) ||                                   /* subMod not found */
                       pdevcFP20x0dpvt->channel >= pdevcFP20x0dpvt->pcFP20x0Module->numCh ||   /* mismatched channel number */
                       pdevcFP20x0dpvt->channel < 0) {                                         /* negative channel number */
                       recGblRecordError(S_db_badField, (void*) precord,
                                         "devBicFP20x0 (init_record) Illegal INP field, check second param (2)");
                       free(pdevcFP20x0dpvt);
                       precord->udf = TRUE;
                       precord->dpvt = NULL;
                       return S_db_badField;
                    }
                    break;   /* end of cFP20x0DEVPARAM_READ and cFP20x0DEVPARAM_WRITE */
                case cFP20x0DEVPARAM_STATUS:
                case cFP20x0DEVPARAM_SCANSTATUS:
                case cFP20x0DEVPARAM_TERM:
                default:
                    recGblRecordError(S_db_badField, (void*)precord,
                                      "devBicFP20x0 (init_record) Illegal INP field. check first param (3)");
                    free(pdevcFP20x0dpvt);
                    precord->udf = TRUE;
                    precord->dpvt = NULL;
                    return S_db_badField;
                    
            }           /*end of switch for first_param */
            break;      /* end of INST_IO */
        default:
            end:
            recGblRecordError(S_db_badField, (void*)precord,
                              "devBicFP20x0 (init_record) Illegal INP field (4)");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }  /* end of main switch statement */

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevcFP20x0dpvt;

    return 2;  /* don't convert */
}



LOCAL long devBicFP20x0_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt; 
    drvcFP20x0Config *pdrvcFP20x0Config;
    cFP20x0Module *pcFP20x0Module;

    if(!precord->dpvt) {
        *ioScanPvt = NULL;
        return 0;
    }
    pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;

    switch(pcFP20x0Module->modType) {
        case cFP20x0_modType_DI8:
            *ioScanPvt =
                ((cFP20x0_modConfig_DI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_DI16:
            *ioScanPvt =
                ((cFP20x0_modConfig_DI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_DI32:
            *ioScanPvt =
                ((cFP20x0_modConfig_DI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_DO8:
            *ioScanPvt =
               ((cFP20x0_modConfig_DO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_DO16:
            *ioScanPvt = 
                ((cFP20x0_modConfig_DO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_DO32:
            *ioScanPvt = 
                ((cFP20x0_modConfig_DO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AI2:
            *ioScanPvt = 
                ((cFP20x0_modConfig_AI2*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AI8:
            *ioScanPvt =
                ((cFP20x0_modConfig_AI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AI16:
            *ioScanPvt = 
                ((cFP20x0_modConfig_AI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AI32:
            *ioScanPvt =
                ((cFP20x0_modConfig_AI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AO2:
            *ioScanPvt = 
                ((cFP20x0_modConfig_AO2*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AO8:
            *ioScanPvt =
                ((cFP20x0_modConfig_AO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AO16:
            *ioScanPvt =
                 ((cFP20x0_modConfig_AO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
        case cFP20x0_modType_AO32:
            *ioScanPvt = 
                 ((cFP20x0_modConfig_AO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanPvt[pdevcFP20x0dpvt->channel];
            break;
    }

    return 0;
}



LOCAL long devBicFP20x0_read_bi(biRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;

    if(!pdevcFP20x0dpvt) {
        return 0;
    }


    switch(pdevcFP20x0dpvt->param_first) {
        case cFP20x0DEVPARAM_STATUS: break;
        case cFP20x0DEVPARAM_READ:
            devBicFP20x0_get_read(precord); break;
        case cFP20x0DEVPARAM_WRITE:         break;
        case cFP20x0DEVPARAM_SCANSTATUS:    break;
        case cFP20x0DEVPARAM_TERM:          break;
        default:                            break;
    }

    return 0;   /* don't convert */
}



LOCAL long devBocFP20x0_init_record(boRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc(sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->out.type) {
        case INST_IO:
            sscanf(precord->out.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) { /* port not found */
                recGblRecordError(S_db_badField, (void*)precord,
                                  "devBocFP20x0 (init_record) Illegal OUT field: port not found");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            index = 0;
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first)) break;
                index++;
            }
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;

            switch(pdevcFP20x0dpvt->param_first) {
                case cFP20x0DEVPARAM_WRITE:
                    strncpy(pdevcFP20x0dpvt->modLabel, pdevcFP20x0dpvt->param_str_second,3);
                    pdevcFP20x0dpvt->modLabel[3] = '\0';     /* append NULL terminator */
                    sscanf(pdevcFP20x0dpvt->param_str_second+5, "%d", &pdevcFP20x0dpvt->channel);
                    pdevcFP20x0dpvt->pcFP20x0Module = drvcFP20x0_modFind(pdevcFP20x0dpvt->modLabel,
                                                                         pdevcFP20x0dpvt->pdrvcFP20x0Config);
                    if(!(pdevcFP20x0dpvt->pcFP20x0Module) ||                           /* subMod not found */
                       pdevcFP20x0dpvt->channel >= pdevcFP20x0dpvt->pcFP20x0Module->numCh ||  /* mistmatched channel number */
                       pdevcFP20x0dpvt->channel < 0 ) {                                /* negative channel number */
                           recGblRecordError(S_db_badField, (void*) precord,
                                             "devBocFP20x0 (init_record) Illegal OUT field. check second param");
                           free(pdevcFP20x0dpvt);
                           precord->udf = TRUE;
                           precord->dpvt = NULL;
                           return S_db_badField;
                    }
                    /* make command header */
                    sprintf(pdevcFP20x0dpvt->commandHeader, "W%sCH%s", pdevcFP20x0dpvt->modLabel,
                                                                       devcFP20x0ChnStr[pdevcFP20x0dpvt->channel]);
                    pdevcFP20x0dpvt->commandHeader[3] += (char)0x01;
                    break;   /* end of cFP20x0DEVPARAM_WRITE */
                case cFP20x0DEVPARAM_STATUS:
                case cFP20x0DEVPARAM_READ:
                case cFP20x0DEVPARAM_SCANSTATUS:
                case cFP20x0DEVPARAM_TERM:
                default:
                    recGblRecordError(S_db_badField, (void*) precord,
                                      "devBocFP20x0 (init_record) Illegal OUT field. check first param");
                    free(pdevcFP20x0dpvt);
                    precord->udf = TRUE;
                    precord->dpvt = NULL;
                    return S_db_badField;
                
            } /* end of switch for pdevcFP20x0dpvt->param_first */
            break;  /* end of inst_io */
        default:
            end:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devBoFP20x0 (init_record) Illegal OUT field");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    } /* end of switch statement for precord->out.type */

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevcFP20x0dpvt;
    return 0;
}


LOCAL long devBocFP20x0_write_bo(boRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config;
    char commandBuf[40];


    if(!precord->dpvt || precord->udf == TRUE) {
        precord->pact = TRUE;
        return 0;
    }

    pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    pdrvcFP20x0Config = pdevcFP20x0dpvt->pdrvcFP20x0Config;


    if(precord->pact == FALSE) {
        precord->pact = TRUE;
        sprintf(commandBuf, "%s%d%s", pdevcFP20x0dpvt->commandHeader,
                                      (precord->rval)?1:0,
                                      EOS_STR);
        drvcFP20x0_requestWriteDev(pdrvcFP20x0Config, commandBuf, (dbCommon*) precord);
        return 0;
    }

    /* nothing to do for post processing */
    precord->pact = FALSE;

    return 0;
}



LOCAL long devLicFP20x0_init_record(longinRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc(sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->inp.type) {
        case INST_IO:
            sscanf(precord->inp.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) {     /* port not found */
                recGblRecordError(S_db_badField, (void*) precord,
                                  "devLicFP20x0 (init_record) Illegal INP field: port not found");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            index = 0;  /* check up first param */
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first))  break;
                index++;
            } 
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;

            switch(pdevcFP20x0dpvt->param_first) {
                case cFP20x0DEVPARAM_READ:
                    strncpy(pdevcFP20x0dpvt->modLabel, pdevcFP20x0dpvt->param_str_second,3);
                    pdevcFP20x0dpvt->modLabel[3] = '\0';     /* append NULL terminator */
                    pdevcFP20x0dpvt->pcFP20x0Module = drvcFP20x0_modFind(pdevcFP20x0dpvt->modLabel,
                                                                         pdevcFP20x0dpvt->pdrvcFP20x0Config);
                    if(!pdevcFP20x0dpvt->pcFP20x0Module) {  /* sub module not found, ie, modLabel mismatch */
                        recGblRecordError(S_db_badField, (void*) precord,
                                          "devLicFP20x0 (init_record) Illegal INP field: check second param");
                        free(pdevcFP20x0dpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;
                    }

                    switch(pdevcFP20x0dpvt->pcFP20x0Module->modType) {
                        case cFP20x0_modType_DI8:
                        case cFP20x0_modType_DI16:
                        case cFP20x0_modType_DI32:
                        case cFP20x0_modType_DO8:
                        case cFP20x0_modType_DO16:
                        case cFP20x0_modType_DO32:
                            /* its OK to operate longin record */
                            break;
                        case cFP20x0_modType_AI2:
                        case cFP20x0_modType_AI8:
                        case cFP20x0_modType_AI16:
                        case cFP20x0_modType_AI32:
                        case cFP20x0_modType_AO2:
                        case cFP20x0_modType_AO8:
                        case cFP20x0_modType_AO16:
                        case cFP20x0_modType_AO32:
                        case cFP20x0_modType_term:
                        default:
                            recGblRecordError(S_db_badField, (void*) precord,
                                              "devLicFP20x0 (init_record) Illegal INP field: check second param");
                            free(pdevcFP20x0dpvt);
                            precord->udf = TRUE;
                            precord->dpvt = NULL;
                            return S_db_badField;
                    }
                    
                    break;
                case cFP20x0DEVPARAM_WRITE:
                case cFP20x0DEVPARAM_STATUS:
                case cFP20x0DEVPARAM_SCANSTATUS:
                case cFP20x0DEVPARAM_TERM:
                default:
                    recGblRecordError(S_db_badField, (void*) precord,
                                      "devLicFP20x0 (init_record) Illegal INP field: check second param");
                    free(pdevcFP20x0dpvt);
                    precord->udf = TRUE;
                    precord->dpvt = NULL;
                    return S_db_badField;
            }


            break;      /* end of INST_IO */
        default:
            end:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devLicFP20x0 (init_record) Illegal INP field");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }                  /* end of switch statement for precord->inp.type */

    precord->udf = FALSE;
    precord->dpvt = (void*) pdevcFP20x0dpvt;

    return 0;
}


LOCAL long devLicFP20x0_get_ioint_info(int cmd, longinRecord *precord, IOSCANPVT *ioScanPvt)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config;
    cFP20x0Module *pcFP20x0Module;


    if(!precord->dpvt) {
       *ioScanPvt = NULL;
       return 0;
    }

    pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    pdrvcFP20x0Config = (drvcFP20x0Config*) pdevcFP20x0dpvt->pdrvcFP20x0Config;
    pcFP20x0Module = (cFP20x0Module*) pdevcFP20x0dpvt->pcFP20x0Module;


    switch(pcFP20x0Module->modType) {
        case cFP20x0_modType_DI8:
            *ioScanPvt = 
                ((cFP20x0_modConfig_DI8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanMbbiLiPvt;
            break;
        case cFP20x0_modType_DI16:
            *ioScanPvt = 
                ((cFP20x0_modConfig_DI16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanMbbiLiPvt;
            break;
        case cFP20x0_modType_DI32:
            *ioScanPvt = 
                ((cFP20x0_modConfig_DI32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanMbbiLiPvt;
            break;
        case cFP20x0_modType_DO8:
            *ioScanPvt =
                ((cFP20x0_modConfig_DO8*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanMbboLoPvt;
            break;
        case cFP20x0_modType_DO16:
            *ioScanPvt =
                ((cFP20x0_modConfig_DO16*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanMbboLoPvt;
            break;
        case cFP20x0_modType_DO32:
            *ioScanPvt = 
                ((cFP20x0_modConfig_DO32*)(&pcFP20x0Module->vcFP20x0_modConfig))->ioScanMbboLoPvt;
            break;
        default:
            *ioScanPvt = NULL;
    }

    return 0;
}


LOCAL long devLicFP20x0_read_li(longinRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;

    if(!pdevcFP20x0dpvt) {
        return 0;
    }

    switch(pdevcFP20x0dpvt->param_first) {
        case cFP20x0DEVPARAM_STATUS: break;
        case cFP20x0DEVPARAM_READ:
            devLicFP20x0_get_read(precord); break;
        case cFP20x0DEVPARAM_WRITE:         break;
        case cFP20x0DEVPARAM_SCANSTATUS:    break;
        case cFP20x0DEVPARAM_TERM:          break;
        default:                            break;
    }
    return 0;
}


LOCAL long devLocFP20x0_init_record(longoutRecord *precord)
{ 
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) malloc(sizeof(devcFP20x0dpvt));
    int index;

    switch(precord->out.type) {
        case INST_IO:
            sscanf(precord->out.value.instio.string, "%s %s %s", pdevcFP20x0dpvt->port_name,
                                                                 pdevcFP20x0dpvt->param_str_first,
                                                                 pdevcFP20x0dpvt->param_str_second);
            pdevcFP20x0dpvt->pdrvcFP20x0Config = drvcFP20x0_find(pdevcFP20x0dpvt->port_name);
            if(!pdevcFP20x0dpvt->pdrvcFP20x0Config) {       /* port not found */
                recGblRecordError(S_db_badField, (void*) precord,
                                  "devLocFP20x0 (init_record) Illegal OUT field: port not found");
                free(pdevcFP20x0dpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            index = 0;      /* check up first param */
            while((pdevcFP20x0tParamFirst+index)->param_str) {
                if(!strcmp((pdevcFP20x0tParamFirst+index)->param_str, pdevcFP20x0dpvt->param_str_first)) break;
                index ++;
            }
            pdevcFP20x0dpvt->param_first = (pdevcFP20x0tParamFirst+index)->param;

            switch(pdevcFP20x0dpvt->param_first) {
                case cFP20x0DEVPARAM_WRITE:
                    strncpy(pdevcFP20x0dpvt->modLabel, pdevcFP20x0dpvt->param_str_second,3);
                    pdevcFP20x0dpvt->modLabel[3] = '\0';     /* append NULL terminator */
                    pdevcFP20x0dpvt->pcFP20x0Module = drvcFP20x0_modFind(pdevcFP20x0dpvt->modLabel,
                                                                         pdevcFP20x0dpvt->pdrvcFP20x0Config);
                    if(!pdevcFP20x0dpvt->pcFP20x0Module) { /* sub module not found, ie, modLabel mismatch */
                        recGblRecordError(S_db_badField, (void*) precord,
                                          "devLocFP20x0 (init_record) Illegal OUT field: check second param");
                        free(pdevcFP20x0dpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;
                    }

                    /* make command header */
                    sprintf(pdevcFP20x0dpvt->commandHeader, "WM%s", pdevcFP20x0dpvt->modLabel);
                    pdevcFP20x0dpvt->commandHeader[4] += (char)0x01;


                    switch(pdevcFP20x0dpvt->pcFP20x0Module->modType) {
                        case cFP20x0_modType_DI8:
                        case cFP20x0_modType_DI16:
                        case cFP20x0_modType_DI32:
                        case cFP20x0_modType_DO8:
                        case cFP20x0_modType_DO16:
                        case cFP20x0_modType_DO32:
                            /* Its OK to operate longout Record */
                            break;
                        case cFP20x0_modType_AI2:
                        case cFP20x0_modType_AI8:
                        case cFP20x0_modType_AI16:
                        case cFP20x0_modType_AI32:
                        case cFP20x0_modType_AO2:
                        case cFP20x0_modType_AO8:
                        case cFP20x0_modType_AO16:
                        case cFP20x0_modType_AO32:
                        case cFP20x0_modType_term:
                        default:
                            recGblRecordError(S_db_badField, (void*) precord,
                                              "devLocFP20x0 (init_record) Illegal OUT field: check second param");
                            free(pdevcFP20x0dpvt);
                            precord->udf =TRUE;
                            precord->dpvt = NULL;
                            return S_db_badField;
                    }

                    break;
                case cFP20x0DEVPARAM_READ:
                case cFP20x0DEVPARAM_STATUS:
                case cFP20x0DEVPARAM_SCANSTATUS:
                case cFP20x0DEVPARAM_TERM:
                default:
                    recGblRecordError(S_db_badField, (void*) precord,
                                      "devLocFP20x0 (init_record) Illegal OUT field: check first param");
                    free(pdevcFP20x0dpvt);
                    precord->udf = TRUE;
                    precord->dpvt = NULL;
                    return S_db_badField;
            }
            
            break;    /* end of INST_IO */
        default:
            end:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devLocFP20x0 (init_record) Illegal OUT field");
            free(pdevcFP20x0dpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    }              /* end of switch statement for precord->out.type */

    precord->udf = FALSE;
    precord->dpvt = (void*) pdevcFP20x0dpvt;

    return 0;
}


LOCAL long devLocFP20x0_write_lo(longoutRecord *precord)
{
    devcFP20x0dpvt *pdevcFP20x0dpvt = (devcFP20x0dpvt*) precord->dpvt;
    drvcFP20x0Config *pdrvcFP20x0Config = pdevcFP20x0dpvt->pdrvcFP20x0Config;
    char commandBuf[40];
    

    if(!pdevcFP20x0dpvt || precord->udf == TRUE) {
        precord->pact = TRUE;
        return 0;
    }


    if(precord->pact == FALSE) {
        precord->pact = TRUE;

        
        switch(pdevcFP20x0dpvt->pcFP20x0Module->modType) {
            case cFP20x0_modType_DI8:
            case cFP20x0_modType_DO8:
                sprintf(commandBuf, "%s%.2X%s", pdevcFP20x0dpvt->commandHeader,
                                                (unsigned char) precord->val,
                                                EOS_STR);
                drvcFP20x0_requestWriteDev(pdrvcFP20x0Config, commandBuf, (dbCommon*) precord);
                break;
            case cFP20x0_modType_DI16:
            case cFP20x0_modType_DO16:
                sprintf(commandBuf, "%s%.4X%s", pdevcFP20x0dpvt->commandHeader,
                                                (unsigned short) precord->val,
                                                 EOS_STR);
                drvcFP20x0_requestWriteDev(pdrvcFP20x0Config, commandBuf, (dbCommon*) precord); 
                break;
            case cFP20x0_modType_DI32:
            case cFP20x0_modType_DO32:
                sprintf(commandBuf, "%s%.8X%s", pdevcFP20x0dpvt->commandHeader,
                                                (unsigned) precord->val,
                                                EOS_STR);
                drvcFP20x0_requestWriteDev(pdrvcFP20x0Config, commandBuf, (dbCommon*) precord);
                break;
              
        }
        return 0;
    }

    /* nothing to do for post processing */
    precord->pact = FALSE;

    return 0;
}
