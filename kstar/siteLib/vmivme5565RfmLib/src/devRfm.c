/*
**    =========================================================================
**
**        Abs: EPICS Driver Support for VMIVME5565 Reflect Memory Module
**
**        Name: drvVmivme5565.c
**
**        First Auth:   06-Feb-2004,  Kukhee Kim         (KHKIM)
**        Second Auth:  dd-mmm-yyyy,  First Lastname     (USERNAME)
**        Rev:          dd-mmm-yyyy,  Reviewer's name    (USERNAME)
**
**    -------------------------------------------------------------------------
**        Mod:
**                      dd-mmm-yyyy,  First Lastname     (USERNAME)
**                         comments
**
**    =========================================================================
*/

#define DEBUG
                         /* POSIX related headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdioLib.h>
#include <string.h>
                          /* vxWorks related headers */
#include <vxWorks.h>
#include <vxLib.h>
#include <sysLib.h>
#include <cacheLib.h>
#include <taskLib.h>
#include <logLib.h>
#include <memLib.h>
#include <rebootLib.h>
                          /* EPICS related headers */
#include "menuFtype.h"
#include "alarm.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "drvSup.h"
#include "devSup.h"
#include "recSup.h"
#include "devLib.h"
#include "link.h"
#include "dbScan.h"
#include "module_types.h"
#include "epicsExport.h"
                          /* drvVmivme5565 */
#include "drvVmivme5565.h"
                          /* RFM */
#include "rfmRecord.h"


epicsShareFunc long epicsShareAPI vmivme5565_valid(short card, short signal, char *param, vmivme5565_config_ts **);


    /* The prototype of common DSET for devRfm */
LOCAL long init_record(struct dbCommon*);
LOCAL long get_io_info(int,struct dbCommon*,IOSCANPVT*);
LOCAL long read_rfm(struct dbCommon*);
    /* The prototype of particular DSET for drvRfm */


struct {
    /* Common part of DSET for devRfm */
    long           number;            /* number of function in the list */          
    DEVSUPFUN      report;            /* print report - current not used */
    DEVSUPFUN      init;              /* call once during ioc init */
    DEVSUPFUN      init_record;       /* init support for particular record */
    DEVSUPFUN      get_io_info;       /* This routine is required for any device
                                      ** type that can use the ioEvant scanner.
                                      ** It is called by the ioEventScan system each
                                      ** time the record is added or deleted from
                                      ** an I/O scan list. The argument "cmd"
                                      ** of this function, has the value (0,1) if the
                                      ** record is being added to or delete from
                                      ** an I/O scan list. It must be proved for
                                      ** any device type that can use the ioEvent scanner. */
    DEVSUPFUN      read_rfm;          /* ptr to read function */
    DEVSUPFUN      special_linconv;   /* special linear converions */

    /* Particular part DSET for devRfm */
} devRfm = {
    /* Common part of DSET for devRfm */
    6,
    NULL,                 /* report() */
    NULL,                 /* init()   */
    init_record,          /**/
    get_io_info,          /**/
    read_rfm,             /**/
    NULL

    /* Particular part of DSET for devRfm */
};

epicsExportAddress(dset, devRfm);



LOCAL long init_record(struct dbCommon* rec_p)
{
    char                    *func_name            = "init_record";
    long                    status                = OK;
    struct rfmRecord        *precord              = (struct rfmRecord*)rec_p;
    struct vmeio            *vmeio_ps             = NULL;
    vmivme5565_config_ts    *vmivme5565_config_ps = NULL;
     
    /* Is ths bus type supported for this module? */
    switch(precord->inp.type) {
        case VME_IO: /* VMEbus */
            vmeio_ps = (struct vmeio*) &(precord->inp.value);
            status   = vmivme5565_valid(vmeio_ps->card,     /* VME card number */
                                        vmeio_ps->signal,   /* VME signal number, not used it */
                                        vmeio_ps->parm,     /* VME parameter string, not used it */
                                        &vmivme5565_config_ps);
        break; /* end of VMEbus hadler */

        default: /* anyother type */
            status = S_dev_badBus;
            precord->val = rfmInvBusType;
        break;
    }  /* end of switch statement */ 
 
    if(status == OK)  precord->udf = FALSE;  /* Init completed successfully */
    else {            recGblRecordError(status, precord, "devRfm (init) Illegal INP field");
                      precord->val = rfmCardNotFound;
         }

    if(!precord->dpvt) precord->val = rfmNotRegistered;

    precord->dpvt = (void*)vmivme5565_config_ps; 

    if(status == OK) {   /* read static information */
        vmivme5565_config_ps->init_flag |= (VMIVME5565_DEV_INIT_MASK | 
                                            VMIVME5565_DEV_INIT_AFTER_MASK |
                                            VMIVME5565_REC_INIT_MASK |
                                            VMIVME5565_REC_INIT_AFTER_MASK);
        precord->val = rfmSuccess;                       /* set VAL field*/
        precord->crd = vmivme5565_config_ps->card;       /* card number */
        precord->vec = vmivme5565_config_ps->vint_vec;   /* VMEbus interrupt vector */
        precord->lev = vmivme5565_config_ps->vint_lev;   /* VMEbus interrupt level */
        precord->rat = vmivme5565_config_ps->addr_type_regs; /* Address type for control register */
        precord->sat = vmivme5565_config_ps->addr_type_sram; /* address type for SRAM */
        precord->bar = vmivme5565_config_ps->base_addr_regs; /* base address for registers */
        precord->lar = vmivme5565_config_ps->local_addr_regs; /* local address for registers */
        precord->bas = vmivme5565_config_ps->base_addr_sram; /* base address for SRAM */
        precord->las = vmivme5565_config_ps->local_addr_sram; /* local address for SRAM */
        precord->brv = vmivme5565_config_ps->brv;         /* board revision number */
        precord->bid = vmivme5565_config_ps->bid;         /* boad id */
        precord->nid = vmivme5565_config_ps->nid;         /* boad node ID */
    }
    
    return status;
}

LOCAL long get_io_info(int cmd, struct dbCommon *rec_p, IOSCANPVT *evt_pp)
{
    long                 status                = OK;
    struct rfmRecord     *precord              = (struct rfmRecord*)rec_p;
    vmivme5565_config_ts *vmivme5565_config_ps = (vmivme5565_config_ts*)precord->dpvt; 

    if(vmivme5565_config_ps) *evt_pp = vmivme5565_config_ps->ioScanPvtRfm; 

    return status;
}

LOCAL long read_rfm(struct dbCommon* rec_p)
{
    long         status = OK;
    struct rfmRecord  *precord = (struct rfmRecord*)rec_p;
    vmivme5565_config_ts *vmivme5565_config_ps = (vmivme5565_config_ts*)precord->dpvt;

    if(!vmivme5565_config_ps) return OK;

    precord->csr = vmivme5565_config_ps->lcsr;    /* local status and control register */
    precord->isr = vmivme5565_config_ps->lisr;    /* local interrupt status register */
    precord->ier = vmivme5565_config_ps->lier;    /* local interrupt enable register */

    precord->ict = vmivme5565_config_ps->int_cnt; /* interrupt counter */
    precord->ic1 = vmivme5565_config_ps->net_int1cmd_cnt;  /* network interrupt1 counter */
    precord->ic2 = vmivme5565_config_ps->net_int2cmd_cnt;  /* network interrupt2 counter */
    precord->ic3 = vmivme5565_config_ps->net_int3cmd_cnt;  /* network interrupt3 counter */
    precord->ic4 = vmivme5565_config_ps->net_int4cmd_cnt;  /* network interrupt4 counter */

    precord->rfb = vmivme5565_config_ps->rngBuffFreeBytes;  /* ring buffer free bytes */
    precord->rfe = vmivme5565_config_ps->rngBuffFreeEvents; /* ring buffer free events */

    precord->ifg = vmivme5565_config_ps->init_flag;         /* init flag */

    if(precord->csr & VMIVME5565_LCSR_LED_MASK) precord->led = rfmLEDOn;
    else                                        precord->led = rfmLEDOff;


    return status;
}
