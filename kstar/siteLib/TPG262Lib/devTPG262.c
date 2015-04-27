/*
**    ==============================================================================
**
**        Abs:  device support for TPG262 vacuum gauge
**
**        Name: devTPG262.h
**
**
**        First Auth:  19-May-2004, Kukhee Kim (KHKIM)
**        Second Auth: dd-mmm-yyyy, First  Lastname (USERNAME)
**        Rev:         dd-mmm-yyyy, Reviewer's name (USERNAME)
**
**    -------------------------------------------------------------------------------
**        Mod:
**                     dd-mmm-yyy, First Lastname   (USERNAME):
**                        comments
**
**    ===============================================================================
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
#include "stringinRecord.h"
#include "epicsExport.h"

#include "drvTPG262.h"


#define TPG262DEVPARAM_TERM -1
#define TPG262DEVPARAM_UNIT  0
#define TPG262DEVPARAM_FIRMWARE 1
#define TPG262DEVPARAM_GID 2
#define TPG262DEVPARAM_GST 3
#define TPG262DEVPARAM_PRS 4

typedef struct {
    char *param_str;
    int  param;
} devTPG262tParam;

LOCAL devTPG262tParam pdevTPG262tParam[] = {
    {"unit", TPG262DEVPARAM_UNIT},
    {"firmware", TPG262DEVPARAM_FIRMWARE},
    {"gid", TPG262DEVPARAM_GID},
    {"gst", TPG262DEVPARAM_GST},
    {"prs", TPG262DEVPARAM_PRS},
    {NULL, TPG262DEVPARAM_TERM}
};


extern drvTPG262Config* drvTPG262_find(char* portName);
LOCAL long devSiTPG262Read_unit_str(drvTPG262Config* pdrvTPG262Config, char* unit_str);               /* string read for measurement unit */
LOCAL long devSiTPG262Read_progver_str(drvTPG262Config* pdrvTPG262Config, char* progver_str);         /* string read for firmware version */
LOCAL long devSiTPG262Read_gid_str(drvTPG262Config* pdrvTPG262Config, int gauge_num, char* gid_str);  /* string read for gauge ID */
LOCAL long devSiTPG262Read_gst_str(drvTPG262Config* pdrvTPG262Config, int gauge_num, char* gst_str);  /* string read for gauge status */
LOCAL long devAiTPG262Read_prs(drvTPG262Config* pdrvTPG262Config, int gauge_num, double* prs);        /* read measured pressure */

typedef struct {
    char port_name[40];
    char param_str[40];
    int param;
    int gauge_num;
    drvTPG262Config *pdrvTPG262Config;
} devTPG262dpvt;


LOCAL long devAiTPG262_init_record(aiRecord *prec);
LOCAL long devAiTPG262_get_ioint_info(int cmd, aiRecord *prec, IOSCANPVT *ioScanPvt);
LOCAL long devAiTPG262_read_ai(aiRecord *prec);

LOCAL long devSiTPG262_init_record(stringinRecord *prec);
LOCAL long devSiTPG262_get_ioint_info(int cmd, stringinRecord *prec, IOSCANPVT *ioScanPvt);
LOCAL long devSiTPG262_read_stringin(stringinRecord *prec);


struct {
    long        number;
    DEVSUPFUN   report;
    DEVSUPFUN   init;
    DEVSUPFUN   init_record;
    DEVSUPFUN   get_ioint_info;
    DEVSUPFUN   read_ai;
    DEVSUPFUN   special_linconv;
} devAiTPG262 = {
    6,
    NULL,
    NULL,
    devAiTPG262_init_record,
    devAiTPG262_get_ioint_info,
    devAiTPG262_read_ai,
    NULL
};

epicsExportAddress(dset,devAiTPG262);


struct {
    long       number;
    DEVSUPFUN  report;
    DEVSUPFUN  init;
    DEVSUPFUN  init_record;
    DEVSUPFUN  get_ioint_info;
    DEVSUPFUN  read_stringin;
} devSiTPG262 = {
    5,
    NULL,
    NULL,
    devSiTPG262_init_record,
    devSiTPG262_get_ioint_info,
    devSiTPG262_read_stringin
};

epicsExportAddress(dset, devSiTPG262);



LOCAL long devAiTPG262_init_record(aiRecord *prec)
{
    devTPG262dpvt *pdevTPG262dpvt = (devTPG262dpvt*)malloc(sizeof(devTPG262dpvt));
    int index=0;

    
    switch(prec->inp.type) {
        case(INST_IO):
            sscanf(prec->inp.value.instio.string,
                   "%s %s %d",
                   pdevTPG262dpvt->port_name, pdevTPG262dpvt->param_str, &pdevTPG262dpvt->gauge_num);
            pdevTPG262dpvt->pdrvTPG262Config = drvTPG262_find(pdevTPG262dpvt->port_name);
            if(!pdevTPG262dpvt->pdrvTPG262Config) goto end;
            while((pdevTPG262tParam+index)->param_str) {
                if(!strcmp((pdevTPG262tParam+index)->param_str, pdevTPG262dpvt->param_str)) break;
                index++;
            }
            pdevTPG262dpvt->param = (pdevTPG262tParam+index)->param;
            if(pdevTPG262dpvt->param<0) goto end; 
            if(pdevTPG262dpvt->gauge_num<0 || pdevTPG262dpvt->gauge_num >1) goto end; 
            break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)prec, 
                              "devAiTPG262 (init_record) Illegal INP field");
            free(pdevTPG262dpvt);
            prec->udf = TRUE;
            prec->dpvt = NULL;
            return S_db_badField;
    }
    prec->udf = FALSE;
    prec->dpvt = (void*) pdevTPG262dpvt;
    /* Must sure record processing does not perform any conversion */
    prec->linr =0;
    return 0;
}


LOCAL long devAiTPG262_get_ioint_info(int cmd, aiRecord *prec, IOSCANPVT *ioScanPvt)
{
    devTPG262dpvt *pdevTPG262dpvt = (devTPG262dpvt*) prec->dpvt;
    drvTPG262Config *pdrvTPG262Config = (drvTPG262Config*) pdevTPG262dpvt->pdrvTPG262Config;

    *ioScanPvt = pdrvTPG262Config->ioScanPvt;

    return 0;

}


LOCAL long devAiTPG262_read_ai(aiRecord *prec)
{
    devTPG262dpvt *pdevTPG262dpvt = (devTPG262dpvt*) prec->dpvt;
    drvTPG262Config *pdrvTPG262Config = (drvTPG262Config*) pdevTPG262dpvt->pdrvTPG262Config;

    switch(pdevTPG262dpvt->param) {
        case TPG262DEVPARAM_PRS:
            devAiTPG262Read_prs(pdrvTPG262Config, pdevTPG262dpvt->gauge_num, &prec->val);
            prec->udf = FALSE;
        break;
    }

    return 2;  /* don't convert */
}


LOCAL long devSiTPG262_init_record(stringinRecord *prec)
{
    devTPG262dpvt *pdevTPG262dpvt = (devTPG262dpvt*)malloc(sizeof(devTPG262dpvt));
    int index=0;

   
    switch(prec->inp.type) {
        case(INST_IO):
            sscanf(prec->inp.value.instio.string,
                   "%s %s %d",
                   pdevTPG262dpvt->port_name, pdevTPG262dpvt->param_str, &pdevTPG262dpvt->gauge_num);
            pdevTPG262dpvt->pdrvTPG262Config = drvTPG262_find(pdevTPG262dpvt->port_name);
            if(!pdevTPG262dpvt->pdrvTPG262Config) goto end;
            while((pdevTPG262tParam+index)->param_str) {
                if(!strcmp((pdevTPG262tParam+index)->param_str, pdevTPG262dpvt->param_str)) break;
                index++;
            }
            pdevTPG262dpvt->param = (pdevTPG262tParam+index)->param;
            if(pdevTPG262dpvt->param<0) goto end;
            if(pdevTPG262dpvt->gauge_num<0 || pdevTPG262dpvt->gauge_num >1) goto end;
            break;
        default:
            end:
            recGblRecordError(S_db_badField, (void*)prec,
                              "devAiTPG262 (init_record) Illegal INP field");
            free(pdevTPG262dpvt);
            prec->udf = TRUE;
            prec->dpvt = NULL;
            return S_db_badField;
    }
    prec->udf = FALSE;
    prec->dpvt = (void*) pdevTPG262dpvt;
    return 0;
}


LOCAL long devSiTPG262_get_ioint_info(int cmd, stringinRecord *prec, IOSCANPVT *ioScanPvt)
{
    devTPG262dpvt *pdevTPG262dpvt = (devTPG262dpvt*) prec->dpvt;
    drvTPG262Config *pdrvTPG262Config = (drvTPG262Config*) pdevTPG262dpvt->pdrvTPG262Config;

    *ioScanPvt = pdrvTPG262Config->ioScanPvt;

    return 0;
}

LOCAL long devSiTPG262_read_stringin(stringinRecord *prec)
{
    devTPG262dpvt *pdevTPG262dpvt = (devTPG262dpvt*) prec->dpvt;
    drvTPG262Config *pdrvTPG262Config = (drvTPG262Config*) pdevTPG262dpvt->pdrvTPG262Config;

    switch(pdevTPG262dpvt->param){
        case TPG262DEVPARAM_UNIT:
            devSiTPG262Read_unit_str(pdrvTPG262Config, prec->val);
            prec->udf = FALSE;
            break;
        case TPG262DEVPARAM_FIRMWARE:
            devSiTPG262Read_progver_str(pdrvTPG262Config, prec->val);
            prec->udf = FALSE;
            break;
        case TPG262DEVPARAM_GID:
            devSiTPG262Read_gid_str(pdrvTPG262Config, pdevTPG262dpvt->gauge_num, prec->val);
            prec->udf = FALSE;
            break;
        case TPG262DEVPARAM_GST:
            devSiTPG262Read_gst_str(pdrvTPG262Config, pdevTPG262dpvt->gauge_num, prec->val);
            prec->udf = FALSE;
            break;
        default:
            prec->udf = TRUE;
    }

    return 0;
}


LOCAL long devSiTPG262Read_unit_str(drvTPG262Config* pdrvTPG262Config, char* unit_str)
{
    TPG262_read* pTPG262_read = (TPG262_read*)pdrvTPG262Config->pTPG262_read;

    epicsMutexLock(pdrvTPG262Config->lock);
    strcpy(unit_str, pTPG262_read->unit_str);
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return 0;
}


LOCAL long devSiTPG262Read_progver_str(drvTPG262Config* pdrvTPG262Config, char* progver_str)
{

    TPG262_read* pTPG262_read = (TPG262_read*)pdrvTPG262Config->pTPG262_read;

    epicsMutexLock(pdrvTPG262Config->lock);
    strcpy(progver_str, pTPG262_read->progver_str);
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return 0;
}


LOCAL long devSiTPG262Read_gid_str(drvTPG262Config* pdrvTPG262Config, int gauge_num, char* gid_str)
{
    TPG262_read* pTPG262_read = (TPG262_read*)pdrvTPG262Config->pTPG262_read;

    epicsMutexLock(pdrvTPG262Config->lock);
    switch(gauge_num){
        case 0:
            strcpy(gid_str, pTPG262_read->gid_str0);
            break;
        case 1:
            strcpy(gid_str, pTPG262_read->gid_str1);
            break;
        default:
            gid_str[0] = NULL;
    }
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return 0;
}


LOCAL long devSiTPG262Read_gst_str(drvTPG262Config* pdrvTPG262Config, int gauge_num, char* gst_str)
{
    TPG262_read* pTPG262_read = (TPG262_read*)pdrvTPG262Config->pTPG262_read;

    epicsMutexLock(pdrvTPG262Config->lock);
    switch(gauge_num){
        case 0:
            strcpy(gst_str, pTPG262_read->gst_str0);
            break;
        case 1:
            strcpy(gst_str, pTPG262_read->gst_str1);
            break;
        default:
            gst_str[0] = NULL;
    }
    epicsMutexUnlock(pdrvTPG262Config->lock);
    return 0;
}


LOCAL long devAiTPG262Read_prs(drvTPG262Config* pdrvTPG262Config, int gauge_num,double* prs)
{
    TPG262_read* pTPG262_read = (TPG262_read*)pdrvTPG262Config->pTPG262_read;

    epicsMutexLock(pdrvTPG262Config->lock);
    switch(gauge_num) {
        case 0:
            *prs = pTPG262_read->prs0;
            break;
        case 1:
            *prs = pTPG262_read->prs1;
            break;
        default:
            *prs = -1.;
    }
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return 0;
}
