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
#include <longinRecord.h>
#include <longoutRecord.h>
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

#include "NIDAQmx.h"
#include "nidaq.h"

/*
#define DAQ_START	0
#define DAQ_STOP	1
#define DAQ_ABORT	2
#define DAQ_CONFIG	3
#define DAQ_SAMPCLK     4
#define DAQ_OPRMODE	5
*/

static long
initAiNiPci6254Record(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;

    return 0;
}

static long
readAiNiPci6254(void *prec)
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
        case 0:
            pai->val = daq6254pvt.t0;
            break;
        case 1:
            pai->val = daq6254pvt.t1;
            break;
        case 2:
            pai->val = daq6254pvt.fBlipTime;
            break;
        case 3:
            pai->val = daq6254pvt.rate;
            break;
#if 0
        case 4:
            pai->val = daq6254pvt.localshot;
            break;
#endif
        case 5:
            pai->val = daq6254pvt.sampsPerChan;
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
initAoNiPci6254Record(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoNiPci6254(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;
    DaqQueueData queueData;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
    case 0:
        daq6254pvt.t0 = pao->val;
        break;
    case 1:
        daq6254pvt.t1 = pao->val;
        break;
    case 2:
        break;
    case 3:
        daq6254pvt.rate = pao->val;
        break;
    }

    if(num < 4) {
        queueData.opcode = OP_CODE_DAQ_CONFIG;
        epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
    }

    return 0;
}

static long
initBoNiPci6254Record(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

    return 0;
}

static long
writeBoNiPci6254(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;
    CALLBACK *pcallback = (CALLBACK *)pbo->dpvt;

    int num;
    int addr;
    DaqQueueData queueData;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
#if 0
    case DAQ_START:
        queueData.opcode = OP_CODE_DAQ_RUN;
        epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
        break;
    case DAQ_STOP:
        queueData.opcode = OP_CODE_DAQ_STOP;
        epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
        break;
    case DAQ_ABORT:
        queueData.opcode = OP_CODE_DAQ_ABORT;
        epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
        break;
    case DAQ_CONFIG:
        queueData.opcode = OP_CODE_DAQ_CONFIG;
        epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
        break;
    case DAQ_SAMPCLK:
        daq6254pvt.extMode = pbo->val;
        epicsPrintf("Sample Clk Mode : %d\n", daq6254pvt.extMode);
        break;
    case DAQ_OPRMODE:
        daq6254pvt.operation = pbo->val;
        epicsPrintf("Operation Mode : %d\n", daq6254pvt.operation);
        break;
#endif
    }

    return 0;
}

static long
initBiNiPci6254Record(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;

    return 0;
}

static long
readBiNiPci6254(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;
    struct link *plink = (struct link *)&pbi->inp;
    CALLBACK *pcallback = (CALLBACK *)pbi->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

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
} devAoNiPci6254 = { 6, NULL, NULL, initAoNiPci6254Record, NULL, writeAoNiPci6254, NULL };

epicsExportAddress(dset,devAoNiPci6254);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiNiPci6254 = { 6, NULL, NULL, initAiNiPci6254Record, NULL, readAiNiPci6254, NULL };

epicsExportAddress(dset,devAiNiPci6254);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoNiPci6254 = { 6, NULL, NULL, initBoNiPci6254Record, NULL, writeBoNiPci6254, NULL };

epicsExportAddress(dset,devBoNiPci6254);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devBiNiPci6254 = { 6, NULL, NULL, initBiNiPci6254Record, NULL, readBiNiPci6254, NULL };

epicsExportAddress(dset,devBiNiPci6254);
