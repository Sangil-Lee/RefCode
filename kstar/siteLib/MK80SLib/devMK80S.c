/*
 * ==============================================================================
 *   Name: drvMK80S.h
 *   Desc: Device support for MASTER-K 80S PLC
 * ------------------------------------------------------------------------------
 *   2012/06/29  yunsw      Initial revision (based on pfeffierTPG262Lib)
 * ==============================================================================
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
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "stringinRecord.h"
#include "epicsExport.h"

#include "drvMK80S.h"

static long devAoMK80S_init_record(aoRecord *prec)
{
    struct link *plink = (struct link *)&prec->out;

	kuDebug (kuINFO, "[devAoMK80S_init_record] %s : link(%d) addr(%d) parm(%s)\n",
			prec->name, plink->value.gpibio.link, plink->value.gpibio.addr, plink->value.gpibio.parm);

    prec->udf = FALSE;
    prec->dpvt = (void *)drvMK80S_get (plink->value.gpibio.link);

    return 2;
}

static long devAoMK80S_write_ao(aoRecord *prec)
{
    struct link *plink = (struct link *)&prec->out;

	drvMK80S_send (plink->value.gpibio.link, plink->value.gpibio.parm, prec->val);

    return 0;
}

static long devAiMK80S_init_record(aiRecord *prec)
{
    struct link *plink = (struct link *)&prec->inp;

	kuDebug (kuINFO, "[devAiMK80S_init_record] %s : link(%d) addr(%d) parm(%s)\n",
			prec->name, plink->value.gpibio.link, plink->value.gpibio.addr, plink->value.gpibio.parm);

	pdrvMK80SWordInputList->idx[pdrvMK80SWordInputList->num] = plink->value.gpibio.addr;
	strcpy (pdrvMK80SWordInputList->addr[pdrvMK80SWordInputList->num++], plink->value.gpibio.parm);

    prec->udf = FALSE;
    prec->dpvt = (void *)drvMK80S_get (plink->value.gpibio.link);

    /* Must sure record processing does not perform any conversion */
    prec->linr =0;

    return 0;
}

static long devAiMK80S_get_ioint_info(int cmd, aiRecord *prec, IOSCANPVT *ioScanPvt)
{
    drvMK80SConfig *pdrvMK80SConfig	= (drvMK80SConfig *) prec->dpvt;

    *ioScanPvt = pdrvMK80SConfig->ioScanPvt;

    return 0;
}

static long devAiMK80S_read_ai(aiRecord *prec)
{
    drvMK80SConfig *pdrvMK80SConfig	= (drvMK80SConfig *) prec->dpvt;
    struct link *plink = (struct link *)&prec->inp;

	prec->val = pdrvMK80SConfig->pMK80S_read->nWordValues[plink->value.gpibio.addr];

    return 2;  /* don't convert */
}

static long devBoMK80S_init_record(boRecord *prec)
{
    struct link *plink = (struct link *)&prec->out;

	kuDebug (kuINFO, "[devBoMK80S_init_record] %s : link(%d) addr(%d) parm(%s)\n",
			prec->name, plink->value.gpibio.link, plink->value.gpibio.addr, plink->value.gpibio.parm);

    prec->udf = FALSE;
    prec->dpvt = (void *)drvMK80S_get (plink->value.gpibio.link);

    return 0;
}

static long devBoMK80S_write_bo(boRecord *prec)
{
    struct link *plink = (struct link *)&prec->out;

	drvMK80S_send (plink->value.gpibio.link, plink->value.gpibio.parm, prec->val);

    return 0;
}

static long devBiMK80S_init_record(biRecord *prec)
{
    struct link *plink = (struct link *)&prec->inp;

	kuDebug (kuINFO, "[devBiMK80S_init_record] %s : link(%d) addr(%d) parm(%s)\n",
			prec->name, plink->value.gpibio.link, plink->value.gpibio.addr, plink->value.gpibio.parm);

	pdrvMK80SBitInputList->idx[pdrvMK80SBitInputList->num] = plink->value.gpibio.addr;
	strcpy (pdrvMK80SBitInputList->addr[pdrvMK80SBitInputList->num++], plink->value.gpibio.parm);

    prec->udf = FALSE;
    prec->dpvt = (void *)drvMK80S_get (plink->value.gpibio.link);

    return 0;
}

static long devBiMK80S_get_ioint_info(int cmd, biRecord *prec, IOSCANPVT *ioScanPvt)
{
    drvMK80SConfig *pdrvMK80SConfig	= (drvMK80SConfig *) prec->dpvt;

    *ioScanPvt = pdrvMK80SConfig->ioScanPvt;

    return 0;
}

static long devBiMK80S_read_bi(biRecord *prec)
{
    drvMK80SConfig *pdrvMK80SConfig	= (drvMK80SConfig *) prec->dpvt;
    struct link *plink = (struct link *)&prec->inp;

	prec->val = pdrvMK80SConfig->pMK80S_read->nBitValues[plink->value.gpibio.addr];

	kuDebug (kuINFO, "[devBiMK80S_read_bi] %s : addr(%d) val(%d)\n",
			prec->name, plink->value.gpibio.addr, prec->val);

    return 2;  /* don't convert */
}

static long devMbbiMK80S_init_record(mbbiRecord *prec)
{
    struct link *plink = (struct link *)&prec->inp;

	kuDebug (kuINFO, "[devMbbiMK80S_init_record] %s : link(%d) addr(%d) parm(%s)\n",
			prec->name, plink->value.gpibio.link, plink->value.gpibio.addr, plink->value.gpibio.parm);

    prec->udf = FALSE;
    prec->dpvt = (void *)drvMK80S_get (plink->value.gpibio.link);

    return 0;
}

static long devMbbiMK80S_get_ioint_info(int cmd, mbbiRecord *prec, IOSCANPVT *ioScanPvt)
{
    drvMK80SConfig *pdrvMK80SConfig	= (drvMK80SConfig *) prec->dpvt;

    *ioScanPvt = pdrvMK80SConfig->ioScanPvt;

    return 0;
}

static long devMbbiMK80S_read_mbbi(mbbiRecord *prec)
{
    drvMK80SConfig *pdrvMK80SConfig	= (drvMK80SConfig *) prec->dpvt;
    struct link *plink = (struct link *)&prec->inp;

	if (!strcmp(plink->value.gpibio.parm, "status")) {
		prec->val = pdrvMK80SConfig->status;

		kuDebug (kuINFO, "[devMbbiMK80S_read_mbbi] %s : parm(%s) val(%d)\n",
				prec->name, plink->value.gpibio.parm, prec->val);
	}

    return 2;  /* don't convert */
}

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(aoRecord *);
    long (*getIoIntInfo)();
    long (*write)(aoRecord *);
    long (*special_linconv)(void *,int);
} devAoMK80S = { 6, NULL, NULL, devAoMK80S_init_record, NULL, devAoMK80S_write_ao, NULL };

epicsExportAddress(dset,devAoMK80S);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(aiRecord *);
    long (*getIoIntInfo)();
    long (*read)(aiRecord *);
    long (*special_linconv)(void *,int);
} devAiMK80S = { 6, NULL, NULL, devAiMK80S_init_record, devAiMK80S_get_ioint_info, devAiMK80S_read_ai, NULL };

epicsExportAddress(dset,devAiMK80S);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(boRecord *);
    long (*getIoIntInfo)();
    long (*write)(boRecord *);
    long (*special_linconv)(void *,int);
} devBoMK80S = { 6, NULL, NULL, devBoMK80S_init_record, NULL, devBoMK80S_write_bo, NULL };

epicsExportAddress(dset,devBoMK80S);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(biRecord *);
    long (*getIoIntInfo)();
    long (*read)(biRecord *);
    long (*special_linconv)(void *,int);
} devBiMK80S = { 6, NULL, NULL, devBiMK80S_init_record, devBiMK80S_get_ioint_info, devBiMK80S_read_bi, NULL };

epicsExportAddress(dset,devBiMK80S);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(mbbiRecord *);
    long (*getIoIntInfo)();
    long (*read)(mbbiRecord *);
    long (*special_linconv)(void *,int);
} devMbbiMK80S = { 6, NULL, NULL, devMbbiMK80S_init_record, devMbbiMK80S_get_ioint_info, devMbbiMK80S_read_mbbi, NULL };

epicsExportAddress(dset,devMbbiMK80S);

