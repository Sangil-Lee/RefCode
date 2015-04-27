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

#include "bending_magnet.h"

static long
initAiBendingRecord(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    CALLBACK *pcallback;
    pcallback = (CALLBACK *)(calloc(1,sizeof(CALLBACK)));
    pai->dpvt = (void *)pcallback;

    return 0;
}

static long
readAiBending(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    struct link *plink = (struct link *)&pai->inp;
    CALLBACK *pcallback = (CALLBACK *)pai->dpvt;

    int num;
    int addr;
	int nIndex;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);
#if 0
	nIndex = plink->value.gpibio.link;
#else
	nIndex = addr;
#endif
   
	if(pai->pact) {

        switch(num) {
        case LIMIT_CURR:
            pai->val = bend_data[nIndex].limit_curr;
            break;
        case PRO_VOUT:
            pai->val = bend_data[nIndex].pro_vout;
            break;
        case CTRL_IOUT:
            pai->val = bend_data[nIndex].ctrl_iout;
            break;
        case VOUT:
            pai->val = bend_data[nIndex].vout * 0.1;
            break;
        case IOUT:
            pai->val = bend_data[nIndex].iout * 0.1;
            break;
        case DC_VOLT:
            pai->val = bend_data[nIndex].dc_volt * 0.1;
            break;
        case DC_CURR:
            pai->val = bend_data[nIndex].dc_curr * 0.1;
            break;
        case INV_CURR:
            pai->val = bend_data[nIndex].inv_curr * 0.1;
            break;
        case REQ_VOUT_SET:
            pai->val = bend_data[nIndex].req_vout_set;
            break;
        case REQ_IOUT_SET:
            pai->val = bend_data[nIndex].req_iout_set;
            break;
        case REQ_VOUT_UNDER_VOLT:
            pai->val = bend_data[nIndex].req_vout_under_volt;
            break;
        case REQ_VOUT_UNDER_CURR:
            pai->val = bend_data[nIndex].req_vout_under_curr;
            break;
        case SET_IOUT_UNDER:
            pai->val = bend_data[nIndex].set_iout_under;
            break;
        case SET_VOUT_UNDER:
            pai->val = bend_data[nIndex].set_vout_under;
            break;
        case SET_VOUT:
            pai->val = bend_data[nIndex].set_vout;
            break;
        case SET_IOUT:
            pai->val = bend_data[nIndex].set_iout;
            break;
		case FAULT_CODE:
			pai->val = bend_data[nIndex].fault_code;
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
initAoBendingRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoBending(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

    int num;
    int addr;
	int nIndex;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

#if 0
	nIndex = plink->value.gpibio.link;
#else
	nIndex = addr;
#endif

    if((bend_data[nIndex].touch_status & 0x1) == 0x0) return 0;

    switch(num) {
    case SET_IOUT_UNDER_AO:
        bend_data[nIndex].isWrite = IOUT_UNDER_WRITE;
        bend_data[nIndex].wr_iout_under = (short)pao->val;
        break;
    case SET_VOUT_UNDER_AO:
        bend_data[nIndex].isWrite = VOUT_UNDER_WRITE;
        bend_data[nIndex].wr_vout_under = (short)pao->val;
        break;
    case SET_VOUT_AO:
        bend_data[nIndex].isWrite = VOUT_WRITE;
        bend_data[nIndex].wr_vout = (short)pao->val;
        break;
    case SET_IOUT_AO:
        bend_data[nIndex].isWrite = IOUT_WRITE;
        bend_data[nIndex].wr_iout = (short)pao->val;
        break;
    }

    return 0;
}

static long
initBoBendingRecord(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

/*
*/

    return 0;
}

static long
writeBoBending(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;
    CALLBACK *pcallback = (CALLBACK *)pbo->dpvt;

    int num;
    int addr;
	int nIndex;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);
#if 0
	nIndex = plink->value.gpibio.link;
#else
	nIndex = addr;
#endif

    if((bend_data[nIndex].touch_status & 0x1) == 0x0) return 0;

    switch(num) {
    case REMOTE_BTN:
        bend_data[nIndex].btn_val = 0x01;
        break;
    case LOCAL_BTN:
        bend_data[nIndex].btn_val = 0x02;
        break;
    case RUN_BTN:
        bend_data[nIndex].btn_val = (bend_data[nIndex].touch_status | 0x04);
        bend_data[nIndex].btn_val = (bend_data[nIndex].btn_val & 0xf7);
        printf("Bending Magnet : RUN2 --> [%x]\n", bend_data[nIndex].btn_val);
        break;
    case STOP_BTN:
        bend_data[nIndex].btn_val = (bend_data[nIndex].touch_status | 0x08);
        bend_data[nIndex].btn_val = (bend_data[nIndex].btn_val & 0xfb);
        break;
    case ESTOP_BTN:
        if(pbo->val) {
            bend_data[nIndex].btn_val = (bend_data[nIndex].touch_status | 0x20);
            printf("Bending Magnet : ESTOP ON [%x]\n", bend_data[nIndex].btn_val);
        }
        else {
            bend_data[nIndex].btn_val = (bend_data[nIndex].touch_status & 0xDF);
            printf("Bending Magnet : ESTOP OFF [%x]\n", bend_data[nIndex].btn_val);
        }
        break;
    case SET_BTN:
        if(pbo->val) {
            bend_data[nIndex].btn_val = (bend_data[nIndex].touch_status | 0x40);
            printf("Bending Magnet : RESET ON [%x]\n", bend_data[nIndex].btn_val);
        }
        else {
            bend_data[nIndex].btn_val = (bend_data[nIndex].touch_status & 0xBF);
            printf("Bending Magnet : RESET OFF [%x]\n", bend_data[nIndex].btn_val);
        }
#if 0
        bend_data.btn_val = 0x20;
#endif
        break;
    }

    bend_data[nIndex].isWrite = TOUCH_CMD_WRITE;

    return 0;
}

static long
initBiBendingRecord(void *prec)
{
    struct mbbiRecord *pbi = (struct mbbiRecord *)prec;

/*
*/

    return 0;
}

static long
readBiBending(void *prec)
{
    struct mbbiDirectRecord *pbi = (struct mbbiDirectRecord *)prec;
    struct link *plink = (struct link *)&pbi->inp;
    CALLBACK *pcallback = (CALLBACK *)pbi->dpvt;

    int num;
    int addr;
	int nIndex;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

#if 0
	nIndex = plink->value.gpibio.link;
#else
	nIndex = addr;
#endif

    switch(num) {
    case RUN_STATUS:
        pbi->rval = bend_data[nIndex].status;
        break;
    case IOUT_STATUS:
        pbi->rval = bend_data[nIndex].fault_set;
        break;
    case FAULT_STATUS:
#if 1   /* Bending Magnet Power Supply Fault code - old */
        pbi->rval = bend_data[nIndex].fault_status;
#endif
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
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoBending = { 6, NULL, NULL, initAoBendingRecord, NULL, writeAoBending, NULL };

epicsExportAddress(dset,devAoBending);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiBending = { 6, NULL, NULL, initAiBendingRecord, NULL, readAiBending, NULL };

epicsExportAddress(dset,devAiBending);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoBending = { 6, NULL, NULL, initBoBendingRecord, NULL, writeBoBending, NULL };

epicsExportAddress(dset,devBoBending);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devBiBending = { 6, NULL, NULL, initBiBendingRecord, NULL, readBiBending, NULL };

epicsExportAddress(dset,devBiBending);
