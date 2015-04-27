/*
 * Simple-minded MCF5282 Queued ADC support
 * Simple non-multiplexed scanning operation.
 */
#include <errlog.h>
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
initAoPoloScanRecord(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;


    return 0;
}

static long
writeAoPoloScan(void *prec)
{
    struct aoRecord *pao = (struct aoRecord *)prec;
    struct link *plink = (struct link *)&pao->out;
    CALLBACK *pcallback = (CALLBACK *)pao->dpvt;

/*
    poloScan.val = pao->val;
*/

    return 0;
}

static long
initBoPoloScanRecord(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
 
    return 0;
}
 
static long
writeBoPoloScan(void *prec)
{
    struct boRecord *pbo = (struct boRecord *)prec;
    struct link *plink = (struct link *)&pbo->out;
    CALLBACK *pcallback = (CALLBACK *)pbo->dpvt;
 
    if(pbo->val == 1) {
        epicsPrintf("--- Poloidal Scan Trigger [%d] ---\n", pbo->val);
    }
    else {
        epicsPrintf("--- Poloidal Scan Trigger [%d] ---\n", pbo->val);
    }

    if(poloScan.daqReady == 1) {
        poloScan.msg = POLOSCAN_START;
        epicsPrintf("--- Poloidal Scan Trigger Start ---\n");
    }
}

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devAoPoloScan = { 6, NULL, NULL, initAoPoloScanRecord, NULL, writeAoPoloScan, NULL };

epicsExportAddress(dset,devAoPoloScan);

static struct {
    long number;
    long (*report)(int);
    long (*initialize)(int);
    long (*initRecord)(void *);
    long (*getIoIntInfo)();
    long (*write)(void *);
    long (*special_linconv)(void *,int);
} devBoPoloScan = { 6, NULL, NULL, initBoPoloScanRecord, NULL, writeBoPoloScan, NULL };
    
epicsExportAddress(dset,devBoPoloScan);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
