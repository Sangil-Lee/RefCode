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

#include "tc_save.h"

static long
initAiTCdataRecord(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;

    pai->udf = FALSE;

    return 0;
}

static long
readAiTCdata(void *prec)
{
    struct aiRecord *pai = (struct aiRecord *)prec;
    struct link *plink = (struct link *)&pai->inp;
    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    return(2);
}

static long
initAoTCdataRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;

    return 0;
}

static long
writeAoTCdata(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;

    int num;
    int addr;
	int i;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

	switch(num) 
	{
	case 0: /* T0 */
		for(i=0; i < MAX_SYS_TC; i++)
			tcmds_info[i].t0 = (int)pao->val;
		break;
	case 1: /* T1 */
		for(i=0; i < MAX_SYS_TC; i++)
			tcmds_info[i].t1 = (int)pao->val;
		break;
	case 2: /* shot number */
		for(i=0; i < MAX_SYS_TC; i++)
			tcmds_info[i].shot = (int)pao->val;
		break;
	case 3: /* shot number - kstar */
		for(i=0; i < MAX_SYS_TC; i++)
			tcmds_info[i].kstarshot = (int)pao->val;
		break;
	}

    return 0;
}

static long
initBoTCdataRecord(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;

    return 0;
}

static long
writeBoTCdata(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;

	int i;
    int num;
    int addr;

    addr = plink->value.gpibio.addr;
    sscanf(plink->value.gpibio.parm, "%d", &num);

    switch(num) {
    case 0:    /* Ready */
		if(pbo->val == 1) {
			for(i=0; i < MAX_SYS_TC; i++)
				tcmds_info[i].msg = 1;
		}
        break;
    }

    return 0;
}

static long
initBiTCdataRecord(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;

    return 0;
}

static long
readBiTCdata(void *prec)
{
    struct biRecord *pbi = (struct biRecord *)prec;
    struct link *plink = (struct link *)&pbi->inp;

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
} devAoTCdata = { 6, NULL, NULL, initAoTCdataRecord, NULL, writeAoTCdata, NULL };

epicsExportAddress(dset,devAoTCdata);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devAiTCdata = { 6, NULL, NULL, initAiTCdataRecord, NULL, readAiTCdata, NULL };

epicsExportAddress(dset,devAiTCdata);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoTCdata = { 6, NULL, NULL, initBoTCdataRecord, NULL, writeBoTCdata, NULL };

epicsExportAddress(dset,devBoTCdata);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*read)(void *);
    long (*special_linconv)(void *,int);
} devBiTCdata = { 6, NULL, NULL, initBiTCdataRecord, NULL, readBiTCdata, NULL };

epicsExportAddress(dset,devBiTCdata);
