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
#include <stringinRecord.h>
#include <stringoutRecord.h>
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

#include "etos_tcp.h"
#include "SoftMdsplus.h"

static long
initAiEtosRecord(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;

    return 0;
}

static long
readAiEtos(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    struct link *plink = (struct link *)&pai->inp;
    CALLBACK *pcallback = (CALLBACK *)pai->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(pai->pact) {
        switch(addr) {
        case LAKE_1:  /* Model 218 */
            pai->val = lake.val1[num];
            break;
        case LAKE_2:
            pai->val = lake.val2[num];
            break;
        case LAKE_3:
            pai->val = lake.val3[num];
            break;
        case LAKE_4:
            pai->val = lake.val4[num];
            break;
        case VG_FG:    /* Vacuum Gauge */
            pai->val = vg.fg[num];
            break;
        case VG_PG:
            pai->val = vg.pg[num];
            break;
        case VG_CG:
            pai->val = vg.cg[num];
            break;
        case VG_ION:
            pai->val = vg.ion[num];
            break;
		case VG_4:
			pai->val = vg.vg4[num];
			break;
        case MFC:   /* MFC */
            pai->val = mfc.val[num];
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
initAoEtosRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoEtos(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(num == 0) {
        mfc_write = MFC_SET_NEU;
        mfc_set_neu.val = pao->val;
    }
    else if(num == 1) {
        mfc_write = MFC_SET_ION;
        mfc_set_ion.val = pao->val;
    }
    else if(num == 2) {  /* MDS T0 */
        mdsinfo.t0 = (int)pao->val;
    }
    else if(num == 3) {  /* MDS T1 */
        mdsinfo.t1 = (int)pao->val;
    }
	else if(num == 4) {
        mfc_write = MFC_SET_NEU2;
		mfc_set_neu2.val = pao->val;
	}
	else if(num == 5) {
        mfc_write = MFC_SET_ION2;
		mfc_set_ion2.val = pao->val;
	}

    return 0;
}

static long
initBoEtosRecord(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

/*
*/

    return 0;
}

static long
writeBoEtos(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;
    CALLBACK *pcallback = (CALLBACK *)pbo->dpvt;

    int num;
    int addr;
    int status;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(num == 0) {
        if(pbo->rval == 0x01) {
            softtrigger = 1;
        }        
    }

    return 0;
}

static long
initBiEtosRecord(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;

    
/*
*/

    return 0;
}

static long
readBiEtos(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;
    struct link *plink = (struct link *)&pbi->inp;
    CALLBACK *pcallback = (CALLBACK *)pbi->dpvt;

    int num;
    int addr;
    int status;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    if(num == 0) {
        pbi->rval = mfc.status[0]&0xFF;
    }
    else if(num == 1) {
        pbi->rval = mfc.status[1]&0xFF;
    }
    else if(num == 2) {
        pbi->rval = mfc.status[2]&0xFF;
    }
    else if(num == 3) {
        pbi->rval = mfc.status[3]&0xFF;
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
} devAoEtos = { 6, NULL, NULL, initAoEtosRecord, NULL, writeAoEtos, NULL };

epicsExportAddress(dset,devAoEtos);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiEtos = { 6, NULL, NULL, initAiEtosRecord, NULL, readAiEtos, NULL };

epicsExportAddress(dset,devAiEtos);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoEtos = { 6, NULL, NULL, initBoEtosRecord, NULL, writeBoEtos, NULL };

epicsExportAddress(dset,devBoEtos);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devBiEtos = { 6, NULL, NULL, initBiEtosRecord, NULL, readBiEtos, NULL };

epicsExportAddress(dset,devBiEtos);
