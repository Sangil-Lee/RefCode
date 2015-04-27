/* rfmRecord.c */
/* monitoring for VMIVME5565 reflective memory module */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "dbEvent.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "devSup.h"
#include "errMdef.h"
#include "recSup.h"
#include "special.h"
#define GEN_SIZE_OFFSET
#include "rfmRecord.h"
#undef  GEN_SIZE_OFFSET
#include "epicsExport.h"


/* Create RSET - Record Support Entry Table */
#define report NULL
#define initialize NULL
static long init_record();
static long process();
#define special NULL
#define get_value NULL
#define cvt_dbaddr NULL
#define get_array_info NULL
#define put_array_info NULL
#define get_units NULL
#define get_precision NULL
#define get_enum_str NULL
#define get_enum_strs NULL
#define put_enum_str NULL
#define get_graphic_double NULL
#define get_control_double NULL
#define get_alarm_double NULL

rset rfmRSET = {
    RSETNUMBER,
    report,
    initialize,
    init_record,
    process,
    special,
    get_value,
    cvt_dbaddr,
    get_array_info,
    put_array_info,
    get_units,
    get_precision,
    get_enum_str,
    get_enum_strs,
    put_enum_str,
    get_graphic_double,
    get_control_double,
    get_alarm_double
};

epicsExportAddress(rset, rfmRSET);


/* declarations for associated DSET */
typedef struct {
    long number;
    DEVSUPFUN dev_report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_io_info;
    DEVSUPFUN read_rfm;
    DEVSUPFUN special_linconv;
} rfmdset;


static long alarm(rfmRecord*);
static long monitor(rfmRecord*);



static long init_record(void *rec_p, int pass)
{
    rfmRecord   *precord  = (rfmRecord*)rec_p;
    rfmdset     *pdset    = NULL;
    long        status    = OK;

    if(pass==0) return status;

    /* check device entry table */
    if((pdset = (rfmdset*)(precord->dset)) == NULL) {
        recGblRecordError((status = S_dev_noDSET), precord, "rfm: init_record");
        return status;
    }

    /* must have read_rfm function defined */
    if((pdset->number<5) ||(pdset->read_rfm == NULL)) {
        recGblRecordError((status = S_dev_missingSup), precord, "rfm: init_record");
        return status;
    }

    /* execute device support level initialization */
    if(pdset->init_record) status = (*pdset->init_record)(precord);

    return status;
}


static long process(void *rec_p) 
{
    rfmRecord  *precord  = (rfmRecord*)rec_p;
    rfmdset    *pdset    = (rfmdset*)precord->dset;
    long       status    = OK;
    unsigned char pact   = precord->pact;

    if((pdset==NULL) || (pdset->read_rfm==NULL)) {
        precord->pact = TRUE;  /* if DSET is not found, record will be blocked */
        recGblRecordError((status=S_dev_missingSup),precord,"read_rfm");
        return status;
    }

    /* pact must not be set ture until read_rfm completes */
    status = (*pdset->read_rfm)(precord);   /* read status of rfm moule */
    /* return if begining of asynch processing */
    if(!pact && precord->pact) return status;
    precord->pact = TRUE;
    recGblGetTimeStamp(precord);
    
    /* check alarm */
    alarm(precord);

    /* check monitor */
    monitor(precord);

    /* processing the forward scan link record */
    recGblFwdLink(precord);

    /* clear PACT field */
    precord->pact = FALSE;


    return status;
}


static long alarm(rfmRecord *precord)
{
    long          status   = OK;

    if(precord->udf == TRUE) {
        recGblSetSevr(precord, UDF_ALARM, INVALID_ALARM);
        return status;
     }

     if(precord->val != rfmSuccess) {
        recGblSetSevr(precord, DISABLE_ALARM, MAJOR_ALARM);
        return status;
      }

     if(precord->rfe < 50) { 
        recGblSetSevr(precord,SOFT_ALARM, MAJOR_ALARM);
        return status;
      }

     if(precord->rfe < 100) {
        recGblSetSevr(precord, SOFT_ALARM, MINOR_ALARM);
        return status;
     }

    precord->val = rfmSuccess;
    return status;
}


#define MONITOR_RFMFIELD(PRECORD,NEW,OLD,MASK) \
        if((NEW) != (OLD)) { \
            (OLD) = (NEW); \
            db_post_events((PRECORD),&(NEW),(MASK)); \
        }

static long monitor(rfmRecord *precord)
{
    long          status   = OK;
    unsigned short monitor_mask;

    monitor_mask = recGblResetAlarms(precord);
    monitor_mask |= (DBE_VALUE|DBE_LOG);

    MONITOR_RFMFIELD(precord, precord->led, precord->oled, monitor_mask);  /* led on fron panel */
    MONITOR_RFMFIELD(precord, precord->csr, precord->ocsr, monitor_mask);  /* local control and status register */
    MONITOR_RFMFIELD(precord, precord->isr, precord->oisr, monitor_mask);  /* local interrupt status register */
    MONITOR_RFMFIELD(precord, precord->ier, precord->oier, monitor_mask);  /* local interrupt enable register */
    MONITOR_RFMFIELD(precord, precord->ict, precord->oict, monitor_mask);  /* interupt counter */
    MONITOR_RFMFIELD(precord, precord->ic1, precord->oic1, monitor_mask);  /* network interrupt1 counter */
    MONITOR_RFMFIELD(precord, precord->ic2, precord->oic2, monitor_mask);  /* network interrupt2 counter */
    MONITOR_RFMFIELD(precord, precord->ic3, precord->oic3, monitor_mask);  /* network interrupt3 counter */
    MONITOR_RFMFIELD(precord, precord->ic4, precord->oic4, monitor_mask);  /* network interrupt4 counter */
    MONITOR_RFMFIELD(precord, precord->rfb, precord->orfb, monitor_mask);  /* ring buffer free bytes */
    MONITOR_RFMFIELD(precord, precord->rfe, precord->orfe, monitor_mask);  /* ring buffer free events */
    MONITOR_RFMFIELD(precord, precord->sct, precord->osct, monitor_mask);  /* scan counter */
    MONITOR_RFMFIELD(precord, precord->ifg, precord->oifg, monitor_mask);  /* INIT flag */
    return status;
}

#undef MONITOR_RFMFIELD

