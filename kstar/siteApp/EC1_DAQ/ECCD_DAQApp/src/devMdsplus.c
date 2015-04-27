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

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

static long
initAiMdsplusRecord(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;

    return 0;
}

static long
readAiMdsplus(void *prec)
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
            if(KstarMode == 1) {
                pai->val = Mds_KstarShot;
            }
            else {
                pai->val = Mds_LocalShot;
            }
            break;
        case 1:
            if(daq6254pvt.status == DAQ_STATUS_RUN && daq6220pvt.status == DAQ_STATUS_RUN)
                pai->val = DAQ_STATUS_RUN;
            else if(daq6254pvt.status == DAQ_STATUS_STOP && daq6220pvt.status == DAQ_STATUS_STOP)
                pai->val = DAQ_STATUS_STOP;
            else if(daq6254pvt.status == DAQ_STATUS_DONE && daq6220pvt.status == DAQ_STATUS_DONE)
                pai->val = DAQ_STATUS_DONE;
            else if(daq6254pvt.status == DAQ_STATUS_MDSPLUS && daq6220pvt.status == DAQ_STATUS_MDSPLUS)
                pai->val = DAQ_STATUS_MDSPLUS;
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
initAoMdsplusRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoMdsplus(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
    case 0:
        Mds_LocalShot = pao->val;
        break;
    case 1:
        Mds_KstarShot = pao->val;
        break;
    }

    return 0;
}

static long
initBoMdsplusRecord(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

    return 0;
}

static long
writeBoMdsplus(void *prec)
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
    case 0:
        if(pbo->val == 1) {
            queueData.opcode = OP_CODE_DAQ_RUN;
            epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
            epicsMessageQueueSend(daq6220pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
            if(KstarMode) {
                printf("--- KSTAR MODE : [%d] ---\n", Mds_KstarShot);
                mdsShot = Mds_KstarShot;
            }
            else {
                mdsShot = Mds_LocalShot;
            }
            mds_createNewShot(mdsShot);

            poloScan.daqReady = 1;
#if 0
            daqMdsPutGain(&daqGainPvt);
#endif
        }
        break;
    case 1:
        if(pbo->val == 1) {
            queueData.opcode = OP_CODE_DAQ_STOP;
            epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
            epicsMessageQueueSend(daq6220pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
            poloScan.daqReady = 0;
            poloScan.msg = POLOSCAN_DONE;
        }
        break;
    case 2:
        if(pbo->val == 1) {
            queueData.opcode = OP_CODE_DAQ_ABORT;
            epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
            epicsMessageQueueSend(daq6220pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
            poloScan.daqReady = 0;
            poloScan.msg = POLOSCAN_DONE;
        }
        break;
    case 3:
        if(pbo->val == 1) {
            KstarMode = 1;
        }
        else {
            KstarMode = 0;
        }
        break;
    }
}

static long
initAoGainRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoGain(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    daqGainPvt.gain[num] = pao->val;

    return 0;
}

static long
initAoOffsetRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoOffset(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    daqGainPvt.offset[num] = pao->val;

    return 0;
}

static long
initAoIntegralParamRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;

    return 0;
}

static long
writeAoIntegralParam(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(addr == 0) {
        daqIntegral.lpm[num] = pao->val;
    }
    else if(addr == 1) {
        daqIntegral.integral_t[num] = pao->val;
    }
    else if(addr == 2) {
        daqIntegral.beam_pw = pao->val;
    }
    else if(addr == 3) {
        daqIntegral.integral_s = pao->val;
    }

    return 0;
}

static long
initAiPowerCalcRecord(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;

    return 0;
}

static long
readAiPowerCalc(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    struct link *plink = (struct link *)&pai->inp;
    CALLBACK *pcallback = (CALLBACK *)pai->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(pai->pact) {

        pai->val = daqIntegral.pw[num];

        pai->udf = FALSE;
        return(2);
    }

    if(pai->disv<=0) return(2);
    pai->pact=TRUE;
    callbackRequestProcessCallbackDelayed(pcallback,
        pai->prio,pai,(double)pai->disv);

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
} devAoMdsplus = { 6, NULL, NULL, initAoMdsplusRecord, NULL, writeAoMdsplus, NULL };

epicsExportAddress(dset,devAoMdsplus);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiMdsplus = { 6, NULL, NULL, initAiMdsplusRecord, NULL, readAiMdsplus, NULL };

epicsExportAddress(dset,devAiMdsplus);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoMdsplus = { 6, NULL, NULL, initBoMdsplusRecord, NULL, writeBoMdsplus, NULL };

epicsExportAddress(dset,devBoMdsplus);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoGain = { 6, NULL, NULL, initAoGainRecord, NULL, writeAoGain, NULL };

epicsExportAddress(dset,devAoGain);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoOffset = { 6, NULL, NULL, initAoOffsetRecord, NULL, writeAoOffset, NULL };

epicsExportAddress(dset,devAoOffset);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoIntegralParam = { 6, NULL, NULL, initAoIntegralParamRecord, NULL, writeAoIntegralParam, NULL };

epicsExportAddress(dset,devAoIntegralParam);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiPowerCalc = { 6, NULL, NULL, initAiPowerCalcRecord, NULL, readAiPowerCalc, NULL };

epicsExportAddress(dset,devAiPowerCalc);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
