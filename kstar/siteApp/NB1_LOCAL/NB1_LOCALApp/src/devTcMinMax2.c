/*
 * Simple-minded MCF5282 Queued ADC support
 * Simple non-multiplexed scanning operation.
 */
#include <epicsStdioRedirect.h>
#include <epicsStdlib.h>
#include <recGbl.h>
#include <devSup.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <boRecord.h>
#include <biRecord.h>
#include <mbboRecord.h>
#include <mbbiRecord.h>
#include <mbbiDirectRecord.h>
#include <stringinRecord.h>
#include <waveformRecord.h>
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
#include "TcMinMax2.h"

static long
initAiTcMinMax2Record(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;

    return 0;
}

static long
readAiTcMinMax2(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    struct link *plink = (struct link *)&pai->inp;
    CALLBACK *pcallback = (CALLBACK *)pai->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(pai->pact) {

        switch(num) {
        case 0:  /* Max */
            pai->val = m_TcfMax2;
            break;
        case 1: /* Min */
            pai->val = m_TcfMin2;
            break;
        }

        pai->udf = FALSE;
        return(2);
    }

    if(pai->disv<=0) return(2);
    pai->pact=TRUE;
    callbackRequestProcessCallbackDelayed(pcallback,
        pai->prio,pai,(double)pai->disv);

    return 0;
}

static long
initSiTcMinMax2Record(void *prec)
{
    struct mbbiRecord *psi = (struct stringinRecord *)prec;

    return 0;
}

static long
readSiTcMinMax2(void *prec)
{
    struct stringinRecord *psi = (struct stringinRecord *)prec;
    struct link *plink = (struct link *)&psi->inp;
    CALLBACK *pcallback = (CALLBACK *)psi->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
    case 0: /* Max */
        memcpy(psi->val, tcMinMax2.minmax[m_TcnMaxInd2], 50);
        break;
    case 1: /* Min */
        memcpy(psi->val, tcMinMax2.minmax[m_TcnMinInd2], 50);
        break;
    }

    return 0;
}

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiTcMinMax2 = { 6, NULL, NULL, initAiTcMinMax2Record, NULL, readAiTcMinMax2, NULL };

epicsExportAddress(dset,devAiTcMinMax2);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devSiTcMinMax2 = { 6, NULL, NULL, initSiTcMinMax2Record, NULL, readSiTcMinMax2, NULL };

epicsExportAddress(dset,devSiTcMinMax2);
