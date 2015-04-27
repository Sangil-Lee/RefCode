/*
**    =========================================================================
**
**        Abs: EPICS Driver Support for VMIVME5565 Reflect Memory Module
**
**        Name: drvVmivme5565.c
**
**        First Auth:   17-Jan-2004,  Kukhee Kim         (KHKIM)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vxWorks.h>
#include <vme.h>
#include <iv.h>
#include <vxLib.h>
#include <sysLib.h>
#include <cacheLib.h>
#include <intLib.h>
#include <taskLib.h>
#include <logLib.h>
#include <memLib.h>
#include <rebootLib.h>

#include "epicsMutex.h"
#include "epicsThread.h"
#include "epicsRingBytes.h"
#include "ellLib.h"
#include "callback.h"
#include "devLib.h"
#include "errMdef.h"
#include "drvSup.h"
#include "devSup.h"
#include "devLib.h"
#include "iocsh.h"
#include "taskwd.h"
#include "dbScan.h"
#include "epicsExport.h"

#include "drvVmivme5565.h"


#define LOGMSG(FN_NAME,MSG_STRING) \
            logMsg("%s: %s\n",(int)FN_NAME,(int)MSG_STRING,0,0,0,0)


LOCAL struct {
    char                 *scan_task_name;
    epicsThreadId        scan_task_id;
    unsigned int         scan_task_priority;
    unsigned int         scan_task_stacksize;
    EPICSTHREADFUNC      scan_task_funcptr;
    int                  scan_task_param;             /* dummy value */
    TASKWDFUNCPRR        scan_taskwd_callback;
    int                  scan_taskwd_param;           /* dummy value */
} vmivme5565_scan_task_info;

LOCAL ELLLIST*       drvVmivme5565_ellList           = NULL;
LOCAL unsigned char  vmivme5565_scan_task_spawn_flag = FALSE;
LOCAL unsigned       VMIVME5565SramSize[2]           = { (unsigned)0x04000000, (unsigned)0x08000000};
LOCAL char*          VMIVME5565SramSizeStr[2]        = {"64MB","128MB"};

/* local proto type */
LOCAL long           vmivme5565_init(unsigned long, 
                                     unsigned long, 
                                     unsigned long, 
                                     unsigned char,
                                     unsigned char);
LOCAL void           vmivme5565_scan_task(int);
LOCAL void           vmivme5565_taskwd_callback(int);
LOCAL void           vmivme5565_just_blink_LED(vmivme5565_config_ts*);
LOCAL long           vmivme5565_reboot(int);
LOCAL void           vmivme5565_interrupt_setup(vmivme5565_config_ts*);
LOCAL void           vmivme5565_vint_isr(int);       /* VMEbus Interrupt Service Routine */
LOCAL void           vmivme5565_vint_callback(CALLBACK*);

LOCAL void           vmivme5565_testIRQ(double);                /* VMEbus interrupt test, 
                                 generate network interrupt and broadcast it to ALL nodes */

/* vmivme5565 driver support routines */
LOCAL long           vmivme5565_io_report(int);
LOCAL long           vmivme5565_init_driver(void);

/* vmivme5565 driver entry table */
struct {
    long               number;
    DRVSUPFUN          report;
    DRVSUPFUN          init;
} drvVmivme5565 = {
    2,
    vmivme5565_io_report,
    vmivme5565_init_driver
};
epicsExportAddress(drvet,drvVmivme5565);

/* Registrar vmivme5565_init command */
static const iocshArg vmivme5565_initArg0 = {"card",   iocshArgString};
static const iocshArg vmivme5565_initArg1 = {"sram",   iocshArgString};
static const iocshArg vmivme5565_initArg2 = {"regs",   iocshArgString};
static const iocshArg vmivme5565_initArg3 = {"level",  iocshArgInt};
static const iocshArg vmivme5565_initArg4 = {"vector", iocshArgInt};

static const iocshArg * const vmivme5565_initArgs[5] = { &vmivme5565_initArg0,
                                                         &vmivme5565_initArg1,
                                                         &vmivme5565_initArg2,
                                                         &vmivme5565_initArg3,
                                                         &vmivme5565_initArg4 };
static const iocshFuncDef vmivme5565_initFuncDef = {"vmivme5565_init",
                                                    5,
                                                    vmivme5565_initArgs};


static void vmivme5565_initCallFunc(const iocshArgBuf *args)
{
    unsigned long arg0, arg1, arg2;

    sscanf(args[0].sval, "%x", &arg0);
    sscanf(args[1].sval, "%x", &arg1);
    sscanf(args[2].sval, "%x", &arg2); 

    vmivme5565_init(arg0,
                    arg1,
                    arg2,
                    (unsigned char)args[3].ival,
                    (unsigned char)args[4].ival); 
    
}

static const iocshArg vmivme5565_testIRQArg0 = {"seconds", iocshArgDouble};
static const iocshArg * const vmivme5565_testIRQArgs[1] = { &vmivme5565_testIRQArg0 };
static const iocshFuncDef vmivme5565_testIRQDef = {"vmivme5565_testIRQ",
                                                    1,
                                                    vmivme5565_testIRQArgs};
static void vmivme5565_testIRQCallFunc(const iocshArgBuf *args)
{
    vmivme5565_testIRQ(args[0].dval);
}

static void epicsShareAPI drvVmivme5565Registrar(void)
{
    static int firstTime = TRUE;

    if(firstTime) {
        firstTime=FALSE;

        iocshRegister(&vmivme5565_initFuncDef,vmivme5565_initCallFunc); 
        iocshRegister(&vmivme5565_testIRQDef,vmivme5565_testIRQCallFunc);
   }
}
epicsExportRegistrar(drvVmivme5565Registrar);


LOCAL long vmivme5565_init(unsigned long card,
                           unsigned long offset_sram,
                           unsigned long offset_regs,
                           unsigned char vint_lev,
                           unsigned char vint_vec)
{
    long              status         = OK;
    char*             func_name      = "vmivme5565_init";
    char*             devRegS_sram   = "drvVmivme5565_sram";
    char*             devRegS_regs   = "drvVmivme5565_regs";
    char              owner_name[80];
    volatile void*    localAddr_sram = NULL;
    volatile void*    localAddr_regs = NULL;

    vmivme5565_ts*               vmivme5565_io_ps = NULL;
    vmivme5565_config_ts*    vmivme5565_config_ps = NULL;

    unsigned char            vmivme5565_lcsr_config;
    unsigned long            vmivme5565_sram_test = 0x00000000;

    
    if(!drvVmivme5565_ellList) {
        drvVmivme5565_ellList = (ELLLIST*) malloc(sizeof(ELLLIST));
        if(!drvVmivme5565_ellList) {
            status = S_drv_noDrvSup;
            errMessage(status, func_name);
            return status;
        } else ellInit(drvVmivme5565_ellList);
    }

    sprintf(owner_name,"%s card %ld",devRegS_regs, card);
    status = devRegisterAddress(owner_name,
                                atVMEA24,
                                (size_t)offset_regs,
                                sizeof(vmivme5565_ts),
                                &localAddr_regs);
    if(status != OK) {
        errMessage(status, func_name);
        return status;
    } 
    
    vmivme5565_io_ps = (vmivme5565_ts *)localAddr_regs;
    if(vmivme5565_io_ps->bid != VMIVME5565_PRODUCT_BID) {
        status = devUnregisterAddress(atVMEA24,
                                      (size_t)offset_regs,
                                      owner_name);
        #ifdef DEBUG
        LOGMSG(func_name,"Product ID mismatch.");
        #endif /* DEBUG */
        status = S_dev_addrMapFail;
        return status;
     }

    vmivme5565_config_ps = (vmivme5565_config_ts *) malloc(sizeof(vmivme5565_config_ts));
    if(!vmivme5565_config_ps) {
        #ifdef DEBUG
        LOGMSG(func_name,"Memory allocation error for vmivme5565_config_ps");
        #endif /* DEBUG */
        status = S_dev_internal;
        errMessage(status, func_name);
        return status;
    }
 
    vmivme5565_config_ps->vmivme5565_io_ps = vmivme5565_io_ps;
    vmivme5565_config_ps->vmivme5565_io_UniverseII_DMA_ctrl_ps
                                           = (vmivme5565_UniverseII_DMA_ctrl_ts*) vmivme5565_io_ps;
    vmivme5565_config_ps->vmivme5565_io_UniverseII_ctrl_ps
                                           = (vmivme5565_UniverseII_ctrl_ts*) vmivme5565_io_ps;
    vmivme5565_config_ps->card           = card;
    vmivme5565_config_ps->vint_lev       = vint_lev;
    vmivme5565_config_ps->vint_vec       = vint_vec;
    vmivme5565_config_ps->addr_type_regs = atVMEA24;
    vmivme5565_config_ps->addr_type_sram = atVMEA32;
    vmivme5565_config_ps->base_addr_regs = offset_regs;
    vmivme5565_config_ps->local_addr_regs = (unsigned long) localAddr_regs;

    /* check SRAM size */
    vmivme5565_lcsr_config =  (unsigned char)
                ((vmivme5565_io_ps->lcsr & VMIVME5565_LCSR_CONFIG_MASK)>>VMIVME5565_LCSR_CONFIG);
    switch(vmivme5565_lcsr_config) {
        case 0x00:
            vmivme5565_config_ps->sram_size = vmivme5565Sram64MB;
            break;
        case 0x01:
            vmivme5565_config_ps->sram_size = vmivme5565Sram128MB;
            break;
    }

    sprintf(owner_name, "%s card %ld",devRegS_sram,card);
    status = devRegisterAddress(owner_name,
                                atVMEA32,
                                (size_t)offset_sram,
                                (size_t)VMIVME5565SramSize[vmivme5565_config_ps->sram_size],
                                &localAddr_sram);
    if(status != OK){
        errMessage(status, func_name);
        return status;
    }

    vmivme5565_config_ps->lock = epicsMutexCreate();
    
    {   /* SRAM Test Routine */
            unsigned long*     sram_p = (unsigned long*)localAddr_sram;
            unsigned long      num_comp 
                                   = VMIVME5565SramSize[vmivme5565_config_ps->sram_size]/sizeof(unsigned long);
            unsigned long      index;

                    /* Transmitter Disable to test SRAM */
            epicsMutexLock(vmivme5565_config_ps->lock); 
            vmivme5565_config_ps->lcsr  = (vmivme5565_io_ps->lcsr | VMIVME5565_LCSR_TRMDIS_MASK);
            vmivme5565_io_ps->lcsr      = vmivme5565_config_ps->lcsr;
            epicsMutexUnlock(vmivme5565_config_ps->lock);

            for(index=0;index<num_comp;index+=4096) {
                status = devWriteProbe(sizeof(unsigned long), sram_p+index,&vmivme5565_sram_test); 
                
                if(status != OK) {
                    #ifdef DEBUG 
                    LOGMSG(func_name,"SRAM test fail");
                    #endif /* DEBUG */
                    errMessage(status, func_name);
                    status = S_dev_internal;
                    return status;
                 }
             }
                    /* Transmitter Enable */
             epicsMutexLock(vmivme5565_config_ps->lock);
             vmivme5565_config_ps->lcsr = (vmivme5565_io_ps->lcsr & ~VMIVME5565_LCSR_TRMDIS_MASK);
             vmivme5565_io_ps->lcsr     = vmivme5565_config_ps->lcsr;
             epicsMutexUnlock(vmivme5565_config_ps->lock);

     } /* End of SRAM Test Routine */


    vmivme5565_config_ps->vmivme5565_sram_ps = (void *) localAddr_sram;
    vmivme5565_config_ps->base_addr_sram     = offset_sram;
    vmivme5565_config_ps->local_addr_sram    = (unsigned long) localAddr_sram;

    vmivme5565_config_ps->brv                = vmivme5565_io_ps->brv;
    vmivme5565_config_ps->bid                = vmivme5565_io_ps->bid;
    vmivme5565_config_ps->nid                = vmivme5565_io_ps->nid;

    vmivme5565_config_ps->init_flag          = VMIVME5565_USR_INIT_MASK;
    vmivme5565_config_ps->scan_cnt           = 0;

    scanIoInit(&(vmivme5565_config_ps->ioScanPvtRfm));

    ellAdd(drvVmivme5565_ellList, &vmivme5565_config_ps->node);    

    if(vmivme5565_scan_task_spawn_flag == FALSE) {
        vmivme5565_scan_task_spawn_flag = TRUE;
        vmivme5565_scan_task_info.scan_task_name        = "vmivme5565_scan_task";
        vmivme5565_scan_task_info.scan_task_priority    = epicsThreadPriorityLow;
        vmivme5565_scan_task_info.scan_task_stacksize   = epicsThreadGetStackSize(epicsThreadStackSmall);
        vmivme5565_scan_task_info.scan_task_funcptr     = (EPICSTHREADFUNC) vmivme5565_scan_task;
        vmivme5565_scan_task_info.scan_task_param       = 0;  /* dummy value */
        vmivme5565_scan_task_info.scan_taskwd_callback = (TASKWDFUNCPRR) vmivme5565_taskwd_callback;
        vmivme5565_scan_task_info.scan_taskwd_param    = 0; /* dummy value */

        
        /* task spawn for scan task */
        vmivme5565_scan_task_info.scan_task_id = epicsThreadCreate(
                                                        vmivme5565_scan_task_info.scan_task_name,
                                                        vmivme5565_scan_task_info.scan_task_priority,
                                                        vmivme5565_scan_task_info.scan_task_stacksize,
                                                        vmivme5565_scan_task_info.scan_task_funcptr,
                                                        &vmivme5565_scan_task_info.scan_task_param
                                                 );
        taskwdInsert(vmivme5565_scan_task_info.scan_task_id,
                     vmivme5565_scan_task_info.scan_taskwd_callback,
                     &vmivme5565_scan_task_info.scan_taskwd_param);
    }
    return status;
}


LOCAL void vmivme5565_taskwd_callback(int vmivme5565_taskwd_callback_param)
{
    printf("%s: task stopped\n");

    taskwdRemove(vmivme5565_scan_task_info.scan_task_id);
    vmivme5565_scan_task_info.scan_task_id = epicsThreadCreate(
                                                     vmivme5565_scan_task_info.scan_task_name,
                                                     vmivme5565_scan_task_info.scan_task_priority,
                                                     vmivme5565_scan_task_info.scan_task_stacksize,
                                                     vmivme5565_scan_task_info.scan_task_funcptr,
                                                     &vmivme5565_scan_task_info.scan_task_param
                                              );
    taskwdInsert(vmivme5565_scan_task_info.scan_task_id,
                 vmivme5565_scan_task_info.scan_taskwd_callback,
                 &vmivme5565_scan_task_info.scan_taskwd_param);
}


LOCAL void  vmivme5565_scan_task(int param)
{
    vmivme5565_config_ts*           vmivme5565_config_ps   = NULL;

    while(!drvVmivme5565_ellList || ellCount(drvVmivme5565_ellList) < 1) {
        taskDelay(sysClkRateGet());         /* wait for initialization of linked list */
    }

    while(TRUE) {  /* spin forever */
        vmivme5565_config_ps = (vmivme5565_config_ts*) ellFirst(drvVmivme5565_ellList);
        do {
            vmivme5565_config_ps->scan_cnt++;
            if(!(vmivme5565_config_ps->init_flag & VMIVME5565_SCN_INIT_MASK))
                                        vmivme5565_config_ps->init_flag |= VMIVME5565_SCN_INIT_MASK; 
            switch(vmivme5565_config_ps->init_flag) {
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                    break;
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK |
                      VMIVME5565_DRV_INIT_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                    break;
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK |
                      VMIVME5565_DRV_INIT_MASK | VMIVME5565_DRV_INIT_AFTER_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                    break;
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK |
                      VMIVME5565_DRV_INIT_MASK | VMIVME5565_DRV_INIT_AFTER_MASK |
                      VMIVME5565_DEV_INIT_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                    break;
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK |
                      VMIVME5565_DRV_INIT_MASK | VMIVME5565_DRV_INIT_AFTER_MASK |
                      VMIVME5565_DEV_INIT_MASK | VMIVME5565_DEV_INIT_AFTER_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                    break;
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK |
                      VMIVME5565_DRV_INIT_MASK | VMIVME5565_DRV_INIT_AFTER_MASK |
                      VMIVME5565_DEV_INIT_MASK | VMIVME5565_DEV_INIT_AFTER_MASK |
                      VMIVME5565_REC_INIT_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                case (VMIVME5565_USR_INIT_MASK | VMIVME5565_SCN_INIT_MASK |
                      VMIVME5565_DRV_INIT_MASK | VMIVME5565_DRV_INIT_AFTER_MASK |
                      VMIVME5565_DEV_INIT_MASK | VMIVME5565_DEV_INIT_AFTER_MASK |
                      VMIVME5565_REC_INIT_MASK | VMIVME5565_REC_INIT_AFTER_MASK):
                    vmivme5565_just_blink_LED(vmivme5565_config_ps);
                    break;
                default:
                    break; 
            }  /* end of switch statement */
           
            if(vmivme5565_config_ps->init_flag & VMIVME5565_DRV_INIT_AFTER_MASK) { 
                vmivme5565_config_ps->rngBuffFreeBytes  
                    = epicsRingBytesFreeBytes(vmivme5565_config_ps->rngBuffId); 
                vmivme5565_config_ps->rngBuffFreeEvents 
                    = vmivme5565_config_ps->rngBuffFreeBytes/sizeof(vmivme5565_netIntFIFO_ts);
            }
            scanIoRequest(vmivme5565_config_ps->ioScanPvtRfm);
            vmivme5565_config_ps = (vmivme5565_config_ts*) 
                                           ellNext(&vmivme5565_config_ps->node);
        } while(vmivme5565_config_ps);   /* internal loop, track linked list */
        epicsThreadSleep(VMIVME5565_SCAN_INTERVAL); 
    }   /* external loop, infinite loop */
}



LOCAL void      vmivme5565_just_blink_LED(vmivme5565_config_ts*  vmivme5565_config_ps)
{
    vmivme5565_ts*    vmivme5565_io_ps = vmivme5565_config_ps->vmivme5565_io_ps;
    
    vmivme5565_config_ps->lcsr =
              ~(vmivme5565_config_ps->lcsr & VMIVME5565_LCSR_LED_MASK);
    
    epicsMutexLock(vmivme5565_config_ps->lock); 
        vmivme5565_config_ps->lcsr &= (vmivme5565_io_ps->lcsr | VMIVME5565_LCSR_LED_MASK) ;
        vmivme5565_io_ps->lcsr     =  vmivme5565_config_ps->lcsr; 
    epicsMutexUnlock(vmivme5565_config_ps->lock);
  
}

LOCAL void     vmivme5565_update_latched_value(vmivme5565_config_ts* vmivme5565_config_ps)
{

}

LOCAL long     vmivme5565_reboot(int test)
{
    long                        status = OK;
    return status;
}

LOCAL long     vmivme5565_io_report(int level)
{
    long                        status  = OK;
    vmivme5565_config_ts*       vmivme5565_config_ps = NULL;
    int                         i;
    unsigned char               mask;

    if(!drvVmivme5565_ellList || drvVmivme5565_ellList->count<1) {
        printf("module not found\n");
        return status;
    }

    printf("total %d module(s) found\n",drvVmivme5565_ellList->count);

    vmivme5565_config_ps = (vmivme5565_config_ts*)ellFirst(drvVmivme5565_ellList);
    do {
        printf("  card %d:\n         %s sram space %s SRAM board offset 0x%8.8lx local addrs 0x%8.8lx"\
               "\n         %s regs. space           board offset 0x%8.8lx local addrs 0x%8.8lx\n",
               vmivme5565_config_ps->card,
               epicsAddressTypeName[vmivme5565_config_ps->addr_type_sram],
               VMIVME5565SramSizeStr[vmivme5565_config_ps->sram_size],
               vmivme5565_config_ps->base_addr_sram,
               vmivme5565_config_ps->local_addr_sram,
               epicsAddressTypeName[vmivme5565_config_ps->addr_type_regs],
               vmivme5565_config_ps->base_addr_regs,
               vmivme5565_config_ps->local_addr_regs);

        if(level < 1) goto vmivme5565_report_next;

        printf("         Drv soft init. status 0x%2.2x\n"\
               "          Drv. user level init  -------------------+\n"\
               "          Drv. scan proc  init  -----------------+ |\n"\
               "          Drv. supports   init  ---------------+ | |\n"\
               "          Dev. supports   init  -------------+ | | |\n"\
               "          Rec. supports   init  -----------+ | | | |\n"\
               "          Drv. sup. post  init  ---------+ | | | | |\n"\
               "          Dev. sup. post  init  -------+ | | | | | |\n"\
               "          Rec. sup. post  init  -----+ | | | | | | |\n"\
               "                                     V V V V V V V V\n"\
               "                                     ",
               vmivme5565_config_ps->init_flag);
        for(i=0, mask=0x80;i<8;i++){
            if(vmivme5565_config_ps->init_flag & mask) printf("X ");
            else                                       printf(". ");
            mask>>=1;
        }
        printf("\n         scan counter: %ld\n", vmivme5565_config_ps->scan_cnt);

        if(level < 3) goto vmivme5565_report_next;
        printf("        Board Status/Control Registers\n");
        printf("        BRV 0x%2.2x  BID 0x%2.2x  NID 0x%2.2x  LCSR 0x%8.8lx\n",
               vmivme5565_config_ps->brv,
               vmivme5565_config_ps->bid,
               vmivme5565_config_ps->nid,
               vmivme5565_config_ps->lcsr); 
        printf("       Universe II Status/Control Register\n");

        if(level < 6) goto vmivme5565_report_next;
        if(vmivme5565_config_ps->init_flag & VMIVME5565_DRV_INIT_AFTER_MASK) {
            printf("       Interrupt counter: "\
                   "int.cnt. %ld  mod.icnt %ld  net.int1 %ld  "\
                   "net.int2 %ld  net.int3 %ld  net.int4 %ld\n",
                   vmivme5565_config_ps->int_cnt,
                   vmivme5565_config_ps->mod_int_cnt,
                   vmivme5565_config_ps->net_int1cmd_cnt,
                   vmivme5565_config_ps->net_int2cmd_cnt,
                   vmivme5565_config_ps->net_int3cmd_cnt,
                   vmivme5565_config_ps->net_int4cmd_cnt);
            printf("       Ring buffer info: free bytes %ld   free evnets %d\n",
                   vmivme5565_config_ps->rngBuffFreeBytes,
                   vmivme5565_config_ps->rngBuffFreeEvents);
        }
        

        vmivme5565_report_next:
            vmivme5565_config_ps = (vmivme5565_config_ts*)ellNext(&vmivme5565_config_ps->node);
    }while(vmivme5565_config_ps);

 
    return status;
}

LOCAL long    vmivme5565_init_driver(void)
{
    long                       status               = OK;
    char*                      func_name            = "vmivme5565_init_driver";
    vmivme5565_config_ts*      vmivme5565_config_ps = NULL;

    if(!drvVmivme5565_ellList || (drvVmivme5565_ellList->count < 1)) {
        status = S_dev_noDevice;
        errMessage(status,func_name);
        return status;
    }
   
    vmivme5565_config_ps = (vmivme5565_config_ts*)ellFirst(drvVmivme5565_ellList); 
    do {
        vmivme5565_config_ps->rngBuffId 
            = epicsRingBytesCreate(VMIVME5565_RNGBUFF_FIFO_DEPTH
                                   *sizeof(vmivme5565_netIntFIFO_ts));  /* make ring buffer for FIFO */
        epicsRingBytesFlush(vmivme5565_config_ps->rngBuffId);           /* make ring buffer emty */
        vmivme5565_config_ps->rngBuffFreeBytes  = epicsRingBytesFreeBytes(vmivme5565_config_ps->rngBuffId); 
        vmivme5565_config_ps->rngBuffFreeEvents = vmivme5565_config_ps->rngBuffFreeBytes/sizeof(vmivme5565_netIntFIFO_ts);

        vmivme5565_config_ps->netIntMask = (
                                             VMIVME5565_LICR_ACLEAR_MASK | /* auto clear enable */
                                             VMIVME5565_LICR_INT1_MASK   | /* enable network interrupt 1 */
                                             VMIVME5565_LICR_INT2_MASK   | /* enable network interrupt 2 */
                                             VMIVME5565_LICR_INT3_MASK   | /* enable network interrupt 3 */
                                             VMIVME5565_LICR_INT4_MASK     /* enable network interrupt 4 */
                                           );

        vmivme5565_config_ps->init_flag |= VMIVME5565_DRV_INIT_MASK;   /* set drv. init mask */
        
        devDisableInterruptLevel(intVME,vmivme5565_config_ps->vint_lev);  /* disable VME int. */
        status = devConnectInterrupt(intVME, 
                                     (unsigned)vmivme5565_config_ps->vint_vec,
                                     vmivme5565_vint_isr,
                                     (void*)vmivme5565_config_ps);
        if(status!=OK){
            errMessage(status, func_name);
            goto vmivme5565_init_driver_next;
        } else {
            callbackSetCallback(vmivme5565_vint_callback, &vmivme5565_config_ps->vint_callback);
            callbackSetUser(vmivme5565_config_ps, &vmivme5565_config_ps->vint_callback);
            callbackSetPriority(priorityHigh, &vmivme5565_config_ps->vint_callback);
        }

        vmivme5565_interrupt_setup(vmivme5565_config_ps);
        devEnableInterruptLevel(intVME,vmivme5565_config_ps->vint_lev);
        vmivme5565_config_ps->init_flag |= VMIVME5565_DRV_INIT_AFTER_MASK;

        vmivme5565_init_driver_next:
            vmivme5565_config_ps = (vmivme5565_config_ts*)ellNext(&vmivme5565_config_ps->node);
    } while(vmivme5565_config_ps);

    return status;
}


void vmivme5565_interrupt_setup(vmivme5565_config_ts* vmivme5565_config_ps)
{
    vmivme5565_ts*        vmivme5565_io_ps = vmivme5565_config_ps->vmivme5565_io_ps;
    vmivme5565_UniverseII_ctrl_ts * 
          vmivme5565_io_UniverseII_ctrl_ps = vmivme5565_config_ps->vmivme5565_io_UniverseII_ctrl_ps;
 
    /* initialize counters */
    vmivme5565_config_ps->int_cnt       = 0;      
    vmivme5565_config_ps->mod_int_cnt   = 0;
    vmivme5565_config_ps->net_int1cmd_cnt = 0;
    vmivme5565_config_ps->net_int2cmd_cnt = 0;
    vmivme5565_config_ps->net_int3cmd_cnt = 0;
    vmivme5565_config_ps->net_int4cmd_cnt = 0;

    epicsMutexLock(vmivme5565_config_ps->lock);

    vmivme5565_io_ps->lier = 0x00000000;    /* disable ALL interrupt, module level */
                                            /* To clear Network interrupt FIFO */
    vmivme5565_io_ps->sid1=0x00;   /* clear SID/ISD FIFO for network interrupt level 1 */ 
    vmivme5565_io_ps->sid2=0x00;   /* clear SID/ISD FIFO for network interrupt level 2 */
    vmivme5565_io_ps->sid3=0x00;   /* clear SID/ISD FIFO for network interrupt level 3 */
    vmivme5565_io_ps->sid4=0x00;   /* clear SID/ISD FIFO for network interrupt level 4 */

    vmivme5565_io_ps->lisr |= VMIVME5565_LICR_GBLINT_MASK ; /* set global interrupt enable */
    vmivme5565_io_UniverseII_ctrl_ps->vint_map0 
                           = ((((unsigned long)vmivme5565_config_ps->vint_lev) << VMIVME5565_VINTMAP0_LINT0) & 
                                                                                       VMIVME5565_VINTMAP0_LINT0_MASK);
                                                     /* set VMEbus interrupt level */
    vmivme5565_io_UniverseII_ctrl_ps->statid
                           = (((unsigned long)(vmivme5565_config_ps->vint_vec) << VMIVME5565_STATID_U8BIT) &
                             VMIVME5565_STATID_U8BIT_MASK);
                                                     /* set VMEbus interrupt vector, must be odd number */
    vmivme5565_io_ps->lier |= vmivme5565_config_ps->netIntMask;
    vmivme5565_io_UniverseII_ctrl_ps->vint_en
                           = VMIVME5565_VINT_LINT0_MASK; 
                                                    /* enable LINT interrupt */

    epicsMutexUnlock(vmivme5565_config_ps->lock);

}


LOCAL void vmivme5565_vint_isr(int param)
{
    vmivme5565_config_ts*    vmivme5565_config_ps = (vmivme5565_config_ts*) param;
    vmivme5565_ts*           vmivme5565_io_ps     = vmivme5565_config_ps->vmivme5565_io_ps;
    vmivme5565_UniverseII_ctrl_ts*
                 vmivme5565_io_UniverseII_ctrl_ps = vmivme5565_config_ps->vmivme5565_io_UniverseII_ctrl_ps;
    vmivme5565_netIntFIFO_ts vmivme5565_netIntFIFO;

    vmivme5565_config_ps->int_cnt++;
    epicsMutexLock(vmivme5565_config_ps->lock);
     
    while(vmivme5565_config_ps->lisr = (vmivme5565_io_ps->lisr 
                                        & vmivme5565_config_ps->netIntMask
                                        & ~VMIVME5565_LICR_ACLEAR_MASK)) {
        switch(vmivme5565_config_ps->lisr & VMIVME5565_NETINTMASK_ALL) {
            #define  PUT_RNGBUFF_AND_CALL_CALLBACK(INT_NUM) \
                     vmivme5565_netIntFIFO.int_num = INT_NUM; \
                     vmivme5565_netIntFIFO.isd     = vmivme5565_io_ps->isd##INT_NUM; \
                     vmivme5565_netIntFIFO.sid     = vmivme5565_io_ps->sid##INT_NUM; \
                     if(epicsRingBytesIsFull(vmivme5565_config_ps->rngBuffId)) break; \
                     else epicsRingBytesPut(vmivme5565_config_ps->rngBuffId, \
                                            (char*)&vmivme5565_netIntFIFO, \
                                            sizeof(vmivme5565_netIntFIFO_ts)); \
                     callbackRequest(&vmivme5565_config_ps->vint_callback);
                     

            case VMIVME5565_LICR_INT1_MASK:
               PUT_RNGBUFF_AND_CALL_CALLBACK(1)      /* VMIVME5565_NETWORK_INT_1 */
               vmivme5565_config_ps->net_int1cmd_cnt++;
               break;
            case VMIVME5565_LICR_INT2_MASK:
               PUT_RNGBUFF_AND_CALL_CALLBACK(2)      /* VMIVME5565_NETWORK_INT_2 */
               vmivme5565_config_ps->net_int2cmd_cnt++;
               break;
            case VMIVME5565_LICR_INT3_MASK:
               PUT_RNGBUFF_AND_CALL_CALLBACK(3)      /* VMIVME5565_NETWORK_INT_3 */
               vmivme5565_config_ps->net_int3cmd_cnt++;
               break;
            case VMIVME5565_LICR_INT4_MASK:
               PUT_RNGBUFF_AND_CALL_CALLBACK(4)      /* VMIVME5565_NETWORK_INT_4 */
               vmivme5565_config_ps->net_int4cmd_cnt++;
               break;
            #undef PUT_RNGBUFF_AND_CALL_CALLBACK
        }
        /*
        vmivme5565_config_ps->rngBuffFreeBytes  = epicsRingBytesFreeBytes(vmivme5565_config_ps->rngBuffId);
        vmivme5565_config_ps->rngBuffFreeEvents = vmivme5565_config_ps->rngBuffFreeBytes/sizeof(vmivme5565_tIntFIFO_ts);
        */
    }; 
 
    vmivme5565_io_UniverseII_ctrl_ps->vint_stat = VMIVME5565_VINT_LINT0_MASK;
    vmivme5565_io_UniverseII_ctrl_ps->vint_en = VMIVME5565_VINT_LINT0_MASK;
    vmivme5565_io_ps->lisr |= VMIVME5565_LICR_GBLINT_MASK;
    epicsMutexUnlock(vmivme5565_config_ps->lock);
}


LOCAL void vmivme5565_vint_callback(CALLBACK *pcallback)
{ 
    vmivme5565_config_ts*     vmivme5565_config_ps = NULL;
    vmivme5565_ts*            vmivme5565_io_ps     = NULL;
    vmivme5565_netIntFIFO_ts  vmivme5565_netIntFIFO;

    callbackGetUser(vmivme5565_config_ps, pcallback);
    vmivme5565_io_ps = vmivme5565_config_ps->vmivme5565_io_ps;

    while(!epicsRingBytesIsEmpty(vmivme5565_config_ps->rngBuffId)){
        epicsRingBytesGet(vmivme5565_config_ps->rngBuffId,
                          (char*)&vmivme5565_netIntFIFO,
                          sizeof(vmivme5565_netIntFIFO_ts));
       
        if(vmivme5565_netIntFIFO.int_num < 0) {  /* processing for NOT network interrupt */

        }  else {  /* processing for network interrupt */
            switch(vmivme5565_netIntFIFO.int_num){
                case VMIVME5565_NETWORK_INT_1:  /* processing for  epics to epics communication */
                case VMIVME5565_NETWORK_INT_2:  /* processing for  epics to PCS communication */

                    break;
                case VMIVME5565_NETWORK_INT_3:  /* process PCS to PCS communication */
                case VMIVME5565_NETWORK_INT_4:
                default:
                    break;     /* nothing to do for net int. 3 and 4 or others */    
            } /* end of switch statement */
            #ifdef DEBUG    /* debugging message */
            printf("Int.Callback: card %d IntCnt %ld IntNum %d ISD %ld SID %d\n",
                   vmivme5565_config_ps->card, vmivme5565_config_ps->int_cnt,
                   vmivme5565_netIntFIFO.int_num, vmivme5565_netIntFIFO.isd, vmivme5565_netIntFIFO.sid);
            #endif

        }  /* end of if statement */
    }  /* end of while loop, main loop to take FIFO data */

}


long epicsShareAPI vmivme5565_valid(short card, short signal, char* param, vmivme5565_config_ts **pVmivme5565_config_ps)
{
    long                  status                = OK;
    vmivme5565_config_ts  *vmivme5565_config_ps = NULL;
    char                  card_found            = 0;

    vmivme5565_config_ps = (vmivme5565_config_ts*)ellFirst(drvVmivme5565_ellList);

    if(card<0) {
        status = S_dev_badCard;
        return status;
    }

    do {
        if(vmivme5565_config_ps->card == card) {
            card_found = 1;
            break;
        }
    } while(vmivme5565_config_ps = (vmivme5565_config_ts*)ellNext(&vmivme5565_config_ps->node));

    if(card_found) {
        *pVmivme5565_config_ps = vmivme5565_config_ps;
        status                 = OK;
    }
    else {
        *pVmivme5565_config_ps = (vmivme5565_config_ts*)NULL;
         status                = S_dev_noDevice;
    }
    return status; 
}


LOCAL void   vmivme5565_testIRQ(double wait_double)
{
    vmivme5565_config_ts*      vmivme5565_config_ps = NULL;
    vmivme5565_ts*             vmivme5565_io_ps     = NULL;
    unsigned long              dummyCnt             = 0;   /* dummy index for testing */

   while(TRUE){
       vmivme5565_config_ps = (vmivme5565_config_ts*)ellFirst(drvVmivme5565_ellList);
       do{
           vmivme5565_io_ps      = vmivme5565_config_ps->vmivme5565_io_ps;
           epicsMutexLock(vmivme5565_config_ps->lock);
           vmivme5565_io_ps->ntd = dummyCnt++;
           switch((dummyCnt%4)+1){
               case 1:
                   vmivme5565_io_ps->nic = VMIVME5565_NIC_INT1_BITPTN | VMIVME5565_NIC_INTALL_BITPTN;
                   break;
               case 2:
                   vmivme5565_io_ps->nic = VMIVME5565_NIC_INT2_BITPTN | VMIVME5565_NIC_INTALL_BITPTN;
                   break;
               case 3:
                   vmivme5565_io_ps->nic = VMIVME5565_NIC_INT3_BITPTN | VMIVME5565_NIC_INTALL_BITPTN;
                   break;
               case 4:
                   vmivme5565_io_ps->nic = VMIVME5565_NIC_INT4_BITPTN | VMIVME5565_NIC_INTALL_BITPTN;
                   break;
               default:
           }
           epicsMutexUnlock(vmivme5565_config_ps->lock);
       }while(vmivme5565_config_ps = (vmivme5565_config_ts*)ellNext(&vmivme5565_config_ps->node));
       epicsThreadSleep(wait_double);
   }
}
