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

#include "Acq196Dt100.h"

static long
initAiAcq196Record(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;

#if 0
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;
#endif

    pai->udf = FALSE;

    return 0;
}

static long
readAiAcq196(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    struct link *plink = (struct link *)&pai->inp;
/*
    CALLBACK *pcallback = (CALLBACK *)pai->dpvt;
*/
    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

/*
    if(pai->pact) {
*/
        if(num == 0) {
            pai->val = acq196drv[addr].clock/1000;
        }
        else if(num == 1) {
            if(shotinfo.kstar)
                pai->val = shotinfo.ca_kstarshot;
            else
                pai->val = shotinfo.shot_local;
        }
        else if(num == 2) {
            pai->val = acq196drv[addr].t0;
        }
        else if(num == 3) {
            pai->val = acq196drv[addr].t1;
        }
        else if(num == 4) {
            pai->val = acq196drv[addr].BlipTime;
        }
        else if(num == 5) {
            pai->val = shotinfo.shot_local;
        }

#if 0
        pai->udf = FALSE;
#endif

        return(2);
/*
    }
*/

#if 0
    if(pai->disv<=0) return(2);
    pai->pact=TRUE;
    callbackRequestProcessCallbackDelayed(pcallback,
        pai->prio,pai,(double)pai->disv);
#endif
    return 0;
}

static long
initAoAcq196Record(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoAcq196(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(num == 0) {
        acq196drv[addr].clock = (int)pao->val * 1000;
        acq196drv[addr].isWrite = ACQ_INT_CLOCK;
    }
    else if(num == 1) {
        shotinfo.shot_local = pao->val;
        printf("Local Shot : [%d]\n", shotinfo.shot_local);
    }
    else if(num == 2) {
        acq196drv[addr].t0 = pao->val;
        printf("D-TACQ[%d] T0 : [%f]\n", addr, acq196drv[addr].t0);
    }
    else if(num == 3) {
        acq196drv[addr].t1 = pao->val;
        printf("D-TACQ[%d] T1 : [%f]\n", addr, acq196drv[addr].t1);
    }
    else if(num == 4) {
        acq196drv[addr].BlipTime = pao->val;
        printf("D-TACQ[%d] LTU Time : %f\n", addr, acq196drv[addr].BlipTime);
    }
    else if(num == 10) {
#if 0
        if(acq196drv.kstarrdy && acq196drv.kstar && (acq196drv.kstardaq == 0)) {
            acq196drv.kstarshot = pao->val;
            acq196drv.kstardaq = 1;
        }
#else
        shotinfo.ca_kstarshot = pao->val;
#endif        
    }

    return 0;
}

static long
initBoAcq196Record(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

    return 0;
}

static long
writeBoAcq196(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
    case ACQ_ABORT:    /* Set Abort */
        acq196drv[addr].isWrite = ACQ_ABORT;
        break;
    case ACQ_ARM:    /* Set Arm */
        if(pbo->val == 1 && acq196drv[addr].arm != ACQ_ST_RUN) {
            acq196drv[addr].isWrite = ACQ_ARM;
        }
        break;
    case ACQ_TRG_RISING:
        break;
    case ACQ_TRG_FALLING:
        break;
    case ACQ_KSTAR_MDSPLUS:   /* KSTAR MODE */
        if(pbo->val) shotinfo.kstar = 1;
        else shotinfo.kstar = 0;
        break;
    case ACQ_KSTAR_RDY:
        if(pbo->val) acq196drv[addr].kstarrdy = 1;
        else acq196drv[addr].kstarrdy = 0;
        break;
    case ACQ_KSTAR_FORCE:
        if(pbo->val) acq196drv[addr].kstarput = 1;
        else acq196drv[addr].kstarput = 0;
        break;
    }

    return 0;
}

static long
initBiAcq196Record(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;

    return 0;
}

static long
readBiAcq196(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;
    struct link *plink = (struct link *)&pbi->inp;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
    case FAULT_LIST:
        pbi->rval = acq196drv[addr].set_fault;
        break;
    case DTACQ_ST:
        pbi->rval = acq196drv[addr].st;
    default:
        break;
    }

    return 0;
}

static long
initSiAcq196Record(void *prec)
{
    struct stringinRecord *psi = (struct stringinRecord *)prec;

    return 0;
}

static long
readSiAcq196(void *prec)
{
    struct stringinRecord *psi = (struct stringinRecord *)prec;

/*
    memcpy(psi->val, acq196drv.state, strlen(acq196drv.state));
*/
    return 0;
}

static long
initSoAcq196Record(void *prec)
{
    struct stringoutRecord *pso = (struct stringoutRecord *)prec;

    return 0;
}

static long
writeSoAcq196(void *prec)
{
    struct stringoutRecord *psi = (struct stringoutRecord *)prec;

    return 0;
}

static long
initLiAcq196Record(void *prec)
{
    struct longinRecord *pli = (struct longinRecord *)prec;

    return 0;
}

static long
readLiAcq196(void *prec)
{
    struct longinRecord *pli = (struct longinRecord *)prec;
    struct link *plink = (struct link *)&pli->inp;
    CALLBACK *pcallback = (CALLBACK *)pli->dpvt;

    return 0;
}

static long
initLoAcq196Record(void *prec)
{
    struct longoutRecord *plo = (struct longoutRecord *)prec;

    return 0;
}

static long
writeLoAcq196(void *prec)
{
    struct longoutRecord *plo = (struct longoutRecord *)prec;

    return 0;
}

static long
initAoMdsGainBRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoMdsGainB(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;

    int i, j;
    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

	/* PNB b data */
	if(addr >= 10) nbiPNB_B[addr-10] = pao->val;
	else dtacqDrv[addr][num].offset = pao->val;

    return 0;
}

static long
initAoMdsGainARecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoMdsGainA(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;

    int num;
    int addr;
	int i;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

	/* PNB b data */
	if(addr >= 10) nbiPNB_A[addr-10] = pao->val;
	else dtacqDrv[addr][num].gain = pao->val;

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
} devAoAcq196 = { 6, NULL, NULL, initAoAcq196Record, NULL, writeAoAcq196, NULL };

epicsExportAddress(dset,devAoAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoMdsGainA = { 6, NULL, NULL, initAoMdsGainARecord, NULL, writeAoMdsGainA, NULL };

epicsExportAddress(dset,devAoMdsGainA);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoMdsGainB = { 6, NULL, NULL, initAoMdsGainBRecord, NULL, writeAoMdsGainB, NULL };

epicsExportAddress(dset,devAoMdsGainB);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiAcq196 = { 6, NULL, NULL, initAiAcq196Record, NULL, readAiAcq196, NULL };

epicsExportAddress(dset,devAiAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoAcq196 = { 6, NULL, NULL, initBoAcq196Record, NULL, writeBoAcq196, NULL };

epicsExportAddress(dset,devBoAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devBiAcq196 = { 6, NULL, NULL, initBiAcq196Record, NULL, readBiAcq196, NULL };

epicsExportAddress(dset,devBiAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devSiAcq196 = { 6, NULL, NULL, initSiAcq196Record, NULL, readSiAcq196, NULL };

epicsExportAddress(dset,devSiAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devSoAcq196 = { 6, NULL, NULL, initSoAcq196Record, NULL, writeSoAcq196, NULL };

epicsExportAddress(dset,devSoAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devLiAcq196 = { 6, NULL, NULL, initLiAcq196Record, NULL, readLiAcq196, NULL };

epicsExportAddress(dset,devLiAcq196);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devLoAcq196 = { 6, NULL, NULL, initLoAcq196Record, NULL, writeLoAcq196, NULL };

epicsExportAddress(dset,devLoAcq196);

