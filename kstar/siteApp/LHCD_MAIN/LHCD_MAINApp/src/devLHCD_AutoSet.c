/*
 * Simple-minded MCF5282 Queued ADC support
 * Simple non-multiplexed scanning operation.
 */
#include <epicsStdioRedirect.h>
#include <epicsStdlib.h>
#include <recGbl.h>
#include <devSup.h>
#include <boRecord.h>
#include <epicsExport.h>

#include <alarm.h>
#include <callback.h>
#include <cvtTable.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <recGbl.h>
#include <recSup.h>
#include <devSup.h>
#include <link.h>
#include <dbCommon.h>
#include <string.h>

#include <stddef.h>
#include <cadef.h>

static long
initBoAutosetRecord(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

    return 0;
}   

static long
writeBoAutoset(void *prec)
{   
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;
    CALLBACK *pcallback = (CALLBACK *)pbo->dpvt;

    int num;
    int addr;

    chid mychid;
    double data;
    int ret;

    if(pbo->val) {
        data = 1.12;
        ret = ca_context_create(ca_disable_preemptive_callback);
        printf("ca_context_create : [%d]\n", ret);
        ret = ca_create_channel("LH1_RF_PULSE_WIDTH", NULL, NULL, 10, &mychid);
        printf("ca_create_channel : [%d]\n", ret);
        ca_pend_io(1.0);
        ret = ca_put(DBR_DOUBLE, mychid, &data);
        printf("ca_put : [%d]\n", ret);
        ca_pend_io(1.0);
    }

    return 0;
}

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoAutoset = { 6, NULL, NULL, initBoAutosetRecord, NULL, writeBoAutoset, NULL };

epicsExportAddress(dset,devBoAutoset);
