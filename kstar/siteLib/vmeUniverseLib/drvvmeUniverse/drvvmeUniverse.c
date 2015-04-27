/* drvvmeUnivese.h */
/*******************************************************************************************\
*   Copyright (c) 2004 Korea National Fusion R&D Center, Korea Basic Science Institute.
*   vmeUniverse is distributed subject to a Software License Agreement
*   found in file LICENSE that is included with this distribution. 
********************************************************************************************/

/*
 * drvvmeUniverse.c  2004/12/13 11:40:00 khkim (Kukhee Kim)
 */


#define DEBUG_

#include <vme/vme.h>
#include <vme/vme_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <epicsMutex.h>
#include <epicsThread.h>
#include <ellLib.h>
#include <callback.h>
#include <iocsh.h>
#include <taskwd.h>

#include <dbCommon.h>
#include <drvSup.h>
#include <devSup.h>
#include <dbScan.h>

#include <epicsExport.h>

#include "drvvmeUniverse.h"

void myISR(void *param)
{
    printf("I am in ISR %s\n", (char*) param);
}

LOCAL char             takeVME = 0;
LOCAL vme_bus_handle_t drvvmeUniverse_busHandle;
LOCAL ELLLIST          *pdrvvmeUniverse_ellList = NULL;
LOCAL ELLLIST          *pdrvvmeUniverse_DMAellList = NULL;
LOCAL ELLLIST          *pdrvvmeUniverse_PEEKellList = NULL;
LOCAL ELLLIST          *pdrvvmeUniverse_VMEInterruptellList = NULL;


LOCAL
vmeUniverseAddrMod tvmeUniverseAddrMod[] = {
                                               {VME_A64MB,  "VME_A64MB",  "A64 multiplex block transfer"},
                                               {VME_A64S,   "VME_A64S",   "A64 single cycle access"},
                                               {VME_A64B,   "VME_A64B",   "A64 block transfer"},
                                               {VME_A64LK,  "VME_A64LK",  "A64 lock command"},
                                               {VME_A32LK,  "VME_A32LK",  "A32 lock command"},
                                               {VME_A32UMB, "VME_A32UMB", "A32 nonprivileged multiplex block trnasfer"},
                                               {VME_A32UD,  "VME_A32UD",  "A32 nonprivileged data access"},
                                               {VME_A32UP,  "VME_A32UP",  "A32 nonprivileged program access"},
                                               {VME_A32UB,  "VME_A32UB",  "A32 nonprivileged block transfer"},
                                               {VME_A32SMB, "VME_A32SMB", "A32 supervisory multiplex block transfer"},
                                               {VME_A32SD,  "VME_A32SD",  "A32 supervisory data access"},
                                               {VME_A32SP,  "VME_A32SP",  "A32 supervisory program access"},
                                               {VME_A32SB,  "VME_A32SB",  "A32 supervisory block transfer"},
                                               {VME_A16U,   "VME_A16U",   "A16 nonprivileged access"},
                                               {VME_A16LK,  "VME_A16LK",  "A16 lock cycle"},
                                               {VME_A16S,   "VME_A16S",   "A16 supervisory access"},
                                               {VME_CR_CSR, "VME_CR_CSR", "Configuration ROM/Control&Status register access"},
                                               {VME_A24LK,  "VME_A24LK",  "A24 lock cycle"},
                                               {VME_A40,    "VME_A40",    "A40 single cycle access"},
                                               {VME_A40LK,  "VME_A40LK",  "A40 lock command"},
                                               {VME_A40B,   "VME_A40B",   "A40 block transfer"},
                                               {VME_A24UMB, "VME_A24UMB", "A24 nonprivileged multiplex block transfer"},
                                               {VME_A24UD,  "VME_A24UD",  "A24 nonprivileged data access"},
                                               {VME_A24UP,  "VME_A24UP",  "A24 nonprivileged program access"},
                                               {VME_A24UB,  "VME_A24UB",  "A24 nonprivileged block transfer"},
                                               {VME_A24SMB, "VME_A24SMB", "A24 supervisory multiplex block transfer"},
                                               {VME_A24SD,  "VME_A24SD",  "A24 supervisory data access"},
                                               {VME_A24SP,  "VME_A24SP",  "A24 supervisory program access"},
                                               {VME_A24SB,  "VME_A24SB",  "A24 supervisory blcok transfer"},
                                               {-1,         NULL,         NULL}

                                           };

LOCAL
vmeUniverseAddrSpace tvmeUniverseAddrSpace[] = {
                                                   {VME_A16, "VME_A16", "Short VMEbus addrss space"},
                                                   {VME_A24, "VME_A24", "Standard VMEbus address space"},
                                                   {VME_A32, "VME_A32", "Extended VMEbus address space"},
                                                   {VME_A64, "VME_A64", "64bits VMEbus address space"},
                                                   {-1,      NULL,      NULL}
                                               };

LOCAL
vmeUniverseDataWidth tvmeUniverseDataWidth[] = {
                                                   {VME_D8,   "VME_D8",    "Byte"},
                                                   {VME_D16,  "VME_D16",   "Word"},
                                                   {VME_D32,  "VME_D32",   "Double word"},
                                                   {VME_D64,  "VME_D64",   "Quad word"},
                                                   {-1,       NULL,        NULL}
                                               };
LOCAL
vmeUniverseWindowType tvmeUniverseWindowType[] = {
                                                     {master_window, "master_window", "VME master window"},
                                                     {slave_window,  "slave_window",  "VME slave window"},
                                                     {-1,            NULL,            "Not definded VME window"}
                                                 };


typedef struct {
    unsigned    width;
    char        *width_str;
} drvvmeUniverseDataWidth;

LOCAL drvvmeUniverseDataWidth tdrvvmeUniverseDataWidth[] = {
                                                                {1, "byte"},   {1, "1"},  {1, "1byte"},  {1, "8bits"},
                                                                {2, "short"},  {2, "2"},  {2, "2bytes"}, {2, "16bits"},
                                                                {4, "word"},   {4, "4"},  {4, "4bytes"}, {4, "32bits"},
                                                                {8, "double"}, {8, "8"},  {8, "8bytes"}, {8, "64bits"},
                                                                {0, NULL} 
                                                           };



typedef struct {
    ELLNODE           node;
    vme_addr_mod_t    vme_addr_mod;
    unsigned long     vme_addr;
    unsigned          width;
    int               count;
}  drvvmeUniversePeekConfig;
    


typedef struct {
    ELLNODE           node;
    char              name[40];
    vme_addr_mod_t    vme_addr_mod;
    unsigned long     vme_addr;
    volatile void     *localAddr;
    unsigned          width;
} drvvmeUniverseVarConfig;


typedef struct {
    int level;
    int level_macro;
    char *level_string;
} drvvmeUniverseVMEInterruptLevelIntMacroString;

LOCAL drvvmeUniverseVMEInterruptLevelIntMacroString tvmeUniverseVMEInterruptLevelIntMacroString[] = {
    {1, VME_INTERRUPT_VIRQ1, "VME_IRQ1"},
    {2, VME_INTERRUPT_VIRQ2, "VME_IRQ2"},
    {3, VME_INTERRUPT_VIRQ3, "VME_IRQ3"},
    {4, VME_INTERRUPT_VIRQ4, "VME_IRQ4"},
    {5, VME_INTERRUPT_VIRQ5, "VME_IRQ5"},
    {6, VME_INTERRUPT_VIRQ6, "VME_IRQ6"},
    {7, VME_INTERRUPT_VIRQ7, "VME_IRQ7"},

    {1, VME_INTERRUPT_VIRQ1, "1"},
    {2, VME_INTERRUPT_VIRQ2, "2"},
    {3, VME_INTERRUPT_VIRQ3, "3"},
    {4, VME_INTERRUPT_VIRQ4, "4"},
    {5, VME_INTERRUPT_VIRQ5, "5"},
    {6, VME_INTERRUPT_VIRQ6, "6"},
    {7, VME_INTERRUPT_VIRQ7, "7"},
    {-1, -1, NULL}
};

typedef enum {
    disable = 0,
    enable
} drvvmeUniverseEnableDisable;

typedef struct {
    ELLNODE       node;
    void          (*pFunction)(void*);
    void          *param;
    char          *pFileName;
    char          *pFuncName;
    int           line;
    unsigned      counter;
} drvvmeUniverse_VMEInterruptISRConfig;

typedef struct {
    ELLNODE       node;
    unsigned char level;
    unsigned char vector;
    int           interrupt_data;
    int           level_macro;
    vme_interrupt_handle_t  interrupt_handle;
    struct sigevent *psigevent;
} drvvmeUniverse_VMEInterruptConfig;

typedef struct {
    drvvmeUniverseEnableDisable  enableDisable;
    drvvmeUniverseEnableDisable  connected;
    epicsMutexId                 lock;
} drvvmeUniverse_VMEInterruptLevel;

typedef struct {
    ELLLIST          *pISRellList;
    epicsMutexId     lock;
    unsigned         counter;
} drvvmeUniverse_VMEInterruptVector;


drvvmeUniverse_VMEInterruptLevel     *pdrvvmeUniverse_VMEInterruptLevel;
drvvmeUniverse_VMEInterruptVector    *pdrvvmeUniverse_VMEInterruptVector;

static int univ_fd;



LOCAL void drvvmeUniverse_printCtl(void);
LOCAL void drvvmeUniverse_printInterrupt(void);
LOCAL void drvvmeUniverse_printMaster(void);
LOCAL void drvvmeUniverse_printSlave(void);
LOCAL void drvvmeUniverse_printIoMem(void);
LOCAL void drvvmeUniverse_printISR(void);
LOCAL void drvvmeUniverse_dumpPeekData(drvvmeUniversePeekConfig *pdrvvmeUniversePeekConfig);
LOCAL void drvvmeUniverse_dumpPeekDataByte(unsigned char *vmeAddr, unsigned long vme_addr, int count);
LOCAL void drvvmeUniverse_dumpPeekDataShort(unsigned short *vmeAddr, unsigned long vme_addr, int count);
LOCAL void drvvmeUniverse_dumpPeekDataWord(unsigned *vmeAddr, unsigned long vme_addr, int count);
LOCAL void drvvmeUniverse_dumpPeekDataDouble(unsigned long *vmeAddr, unsigned long vme_addr, int count);
LOCAL void drvvmeUniverse_reportConfig(drvvmeUniverseConfig *pdrvvmeUniverseConfig);
LOCAL void drvvmeUniverse_createMasterWindow(vme_addr_mod_t amCode, unsigned long vmeAddr, unsigned long size);
LOCAL void drvvmeUniverse_releaseMasterWindow(vme_addr_mod_t amCode);
LOCAL void drvvmeUniverse_createSlaveWindow(vme_address_space_t addrSpace, unsigned long vmeAddr, unsigned long size);
LOCAL void drvvmeUniverse_releaseSlaveWindow(vme_address_space_t addrSpace);
LOCAL drvvmeUniverseConfig* drvvmeUniverse_findConfigMaster(vme_addr_mod_t amCode);
LOCAL drvvmeUniverseConfig* drvvmeUniverse_findConfigSlave(vme_address_space_t addrSpace);


LOCAL void drvvmeUniverse_VMEInterruptHandlerFunc(int sig, siginfo_t *siginfo, void *extra);
LOCAL void drvvmeUniverse_initVMEInterruptellList(void);
LOCAL drvvmeUniverse_VMEInterruptConfig* drvvmeUniverse_findVMEInterruptConfig(int interrupt_data);
LOCAL drvvmeUniverse_VMEInterruptConfig* drvvmeUniverse_makeVMEInterruptConfig(unsigned char level, unsigned char vector);
LOCAL long drvvmeUniverse_attachVMEInterruptMonitor(drvvmeUniverse_VMEInterruptConfig *pConfig);
LOCAL long drvvmeUniverse_makeInterruptConfig(int level);

LOCAL long drvvmeUniverse_io_report(int level);
LOCAL long drvvmeUniverse_init_driver(void);

#ifdef linux
#ifdef _X86_
unsigned long long int epicsShareAPI vmeUniverse_asmReadCPUClock(void) {
    unsigned long long int x;
    __asm__ volatile(".byte 0x0f, 0x31" : "=A" (x));

    return x;
}
#endif 
#endif

struct {
    long         number;
    DRVSUPFUN    report;
    DRVSUPFUN    init;
}  drvvmeUniverse = {
    2, 
    drvvmeUniverse_io_report,
    drvvmeUniverse_init_driver
};

epicsExportAddress(drvet, drvvmeUniverse);


static const iocshArg drvvmeUniverse_printInfoArg0 = {"option", iocshArgString};
static const iocshArg* const drvvmeUniverse_printInfoArgs[] = { &drvvmeUniverse_printInfoArg0 };
static const iocshFuncDef drvvmeUniverse_printInfoFuncDef = {"vmeUniverse_printInfo",
                                                             1,
                                                             drvvmeUniverse_printInfoArgs};
static void drvvmeUniverse_printInfoCallFunc(const iocshArgBuf *args)
{
    char option[40];

    if(!args[0].sval) {
        printf("Usage: vmeUniverse_printInfo 'option'\n");
        printf("       option: ctl, interrupt, master, slave, iomem, isr\n");
        return;
    }
    
    strcpy(option, args[0].sval);
    if(!strcmp(option, "ctl"))            drvvmeUniverse_printCtl();
    else if(!strcmp(option, "interrupt")) drvvmeUniverse_printInterrupt();
    else if(!strcmp(option, "master"))    drvvmeUniverse_printMaster();
    else if(!strcmp(option, "slave"))     drvvmeUniverse_printSlave();
    else if(!strcmp(option, "iomem"))     drvvmeUniverse_printIoMem();
    else if(!strcmp(option, "isr"))       drvvmeUniverse_printISR();
    else printf("drvvmeUniverse: invalid option %s\n", option);
}


static const iocshArg drvvmeUniverse_createMasterWindowArg0 = {"AM code", iocshArgString};
static const iocshArg drvvmeUniverse_createMasterWindowArg1 = {"VMEbus addr", iocshArgString};
static const iocshArg drvvmeUniverse_createMasterWindowArg2 = {"size of window", iocshArgString};
static const iocshArg* const drvvmeUniverse_createMasterWindowArgs[] = { &drvvmeUniverse_createMasterWindowArg0,
                                                                         &drvvmeUniverse_createMasterWindowArg1,
                                                                         &drvvmeUniverse_createMasterWindowArg2};
static const iocshFuncDef drvvmeUniverse_createMasterWindowFuncDef = {"vmeUniverse_createMasterWindow",
                                                                      3,
                                                                      drvvmeUniverse_createMasterWindowArgs};

static void drvvmeUniverse_createMasterWindowCallFunc(const iocshArgBuf *args)
{
    char amCode_str[40];
    char vmeAddr_str[40];
    char size_str[40];
    vme_addr_mod_t      amCode;
    unsigned long       vmeAddr;
    unsigned long       size;
    int i = 0;

    if(!args[0].sval || !args[1].sval || !args[2].sval) {
        printf("Usage: vmeUniverse_createMasterWindow 'AM code' 'VMEbus addr' 'size of window'\n");
        return;
    }

    strcpy(amCode_str,  args[0].sval);
    strcpy(vmeAddr_str, args[1].sval);
    strcpy(size_str,    args[2].sval);


    /* process AM code */
    while((tvmeUniverseAddrMod+i)->vme_addr_mod_s) {
        if(!strcmp((tvmeUniverseAddrMod+i)->vme_addr_mod_s, amCode_str)) break;
        else i++;
    }
    if(!((tvmeUniverseAddrMod+i)->vme_addr_mod_s)) {
        printf("drvvmeUniverse: AM code mismatch\n");
        return;;
    }
    amCode = (tvmeUniverseAddrMod+i)->vme_addr_mod;

    /* process VMEbus address */
    if(!strncmp(vmeAddr_str, "0x",2)) 
        sscanf(vmeAddr_str+2, "%lx", &vmeAddr);
    else sscanf(vmeAddr_str, "%ld", &vmeAddr);

    /* process size of window */
    if(!strncmp(size_str, "0x", 2)) 
        sscanf(size_str+2, "%lx", &size);
    else sscanf(size_str, "%ld", &size); 

    #ifdef DEBUG
        printf("drvvmeUniverse: vmeUniverse_createMasterWindow\n");
        printf("                AM code: %s, %s\n", (tvmeUniverseAddrMod+i)->vme_addr_mod_s,
                                                    (tvmeUniverseAddrMod+i)->vme_addr_mod_desc);
        printf("                VMEbus address: 0x%8.8lx        VMEbus windows size: 0x%8.8lx\n",
                                                     vmeAddr, size);
    #endif

    drvvmeUniverse_createMasterWindow(amCode, vmeAddr, size);

}


static const iocshArg drvvmeUniverse_releaseMasterWindowArg0 = {"AM code", iocshArgString};
static const iocshArg* const drvvmeUniverse_releaseMasterWindowArgs[] = {&drvvmeUniverse_releaseMasterWindowArg0};
static const iocshFuncDef drvvmeUniverse_releaseMasterWindowFuncDef = {"vmeUniverse_releaseMasterWindow",
                                                                       1,
                                                                       drvvmeUniverse_releaseMasterWindowArgs};
static void drvvmeUniverse_releaseMasterWindowCallFunc(const iocshArgBuf *args)
{
    char               amCode_str[40];
    vme_addr_mod_t     amCode;
    int                i = 0;

    if(!args[0].sval) {
        printf("Usage: vmeUniverse_releaseMasterWindow 'AM code'\n");
        return;
    }

    strcpy(amCode_str, args[0].sval);

    /* process AM code */
    while((tvmeUniverseAddrMod+i)->vme_addr_mod_s) {
        if(!strcmp((tvmeUniverseAddrMod+i)->vme_addr_mod_s, amCode_str)) break;
        else i++;
    }
    if(!((tvmeUniverseAddrMod+i)->vme_addr_mod_s)) {
        printf("drvvmeUniverse: AM code mismatch\n");
        return;
    }
    amCode = (tvmeUniverseAddrMod+i)->vme_addr_mod;

    drvvmeUniverse_releaseMasterWindow(amCode);
}


static const iocshArg drvvmeUniverse_createSlaveWindowArg0 = {"Address space", iocshArgString};
static const iocshArg drvvmeUniverse_createSlaveWindowArg1 = {"VMEbus addr", iocshArgString};
static const iocshArg drvvmeUniverse_createSlaveWindowArg2 = {"size of window", iocshArgString};
static const iocshArg* const drvvmeUniverse_createSlaveWindowArgs[] = { &drvvmeUniverse_createSlaveWindowArg0,
                                                                        &drvvmeUniverse_createSlaveWindowArg1,
                                                                        &drvvmeUniverse_createSlaveWindowArg2 };
static const iocshFuncDef drvvmeUniverse_createSlaveWindowFuncDef = {"vmeUniverse_createSlaveWindow",
                                                                     3,
                                                                     drvvmeUniverse_createSlaveWindowArgs};

static void drvvmeUniverse_createSlaveWindowCallFunc(const iocshArgBuf *args)
{
    char addrSpace_str[40];
    char vmeAddr_str[40];
    char size_str[40];
    vme_address_space_t      addrSpace;
    unsigned long       vmeAddr;
    unsigned long       size;

    int i =0;

    if(!args[0].sval || !args[1].sval || !args[2].sval) {
         printf("Usage: vmeUniverse_createSlaveWindow 'Address space' 'VMEbus addr' ' size of window'\n");
         return;
    }

    strcpy(addrSpace_str,  args[0].sval);
    strcpy(vmeAddr_str, args[1].sval);
    strcpy(size_str,    args[2].sval);

    /* process address space code */
    while((tvmeUniverseAddrSpace+i)->vme_address_space_s) {
        if(!strcmp((tvmeUniverseAddrSpace+i)->vme_address_space_s, addrSpace_str)) break;
        else i++;
    }
    if(!((tvmeUniverseAddrSpace+i)->vme_address_space_s)) {
        printf("drvvmeUniverse: address space code mismatch\n");
        return;
    }
    addrSpace = (tvmeUniverseAddrSpace+i)->vme_address_space;


    /* process VME address */
    if(!strncmp(vmeAddr_str, "0x", 2))
        sscanf(vmeAddr_str+2, "%lx", &vmeAddr);
    else sscanf(vmeAddr_str, "%ld", &vmeAddr);

    /* process size of window */
    if(!strncmp(size_str, "0x", 2))
        sscanf(size_str+2, "%lx", &size);
    else sscanf(size_str, "%ld", &size);

    #ifdef DEBUG
        printf("drvvmeUniverse: vmeUniverse_createMasterWindow\n");
        printf("                AM code: %s, %s\n", (tvmeUniverseAddrMod+i)->vme_addr_mod_s,
                                                    (tvmeUniverseAddrMod+i)->vme_addr_mod_desc);
        printf("                VMEbus address: 0x%8.8lx        VMEbus windows size: 0x%8.8lx\n",
                                                     vmeAddr, size);
    #endif

    drvvmeUniverse_createSlaveWindow(addrSpace, vmeAddr, size);

}


static const iocshArg drvvmeUniverse_releaseSlaveWindowArg0 = {"Address space code", iocshArgString};
static const iocshArg* const  drvvmeUniverse_releaseSlaveWindowArgs[] = {&drvvmeUniverse_releaseSlaveWindowArg0};
static const iocshFuncDef drvvmeUniverse_releaseSlaveWindowFuncDef = {"vmeUniverse_releaseSlaveWindow",
                                                                      1,
                                                                      drvvmeUniverse_releaseSlaveWindowArgs};
static void drvvmeUniverse_releaseSlaveWindowCallFunc(const iocshArgBuf *args)
{
    char           addrSpace_str[40];
    vme_address_space_t addrSpace;
    int            i =0;
 
    if(!args[0].sval) {
        printf("Usage: vmeUniverse_releaseSlaveWindow 'Address space code'\n");
        return;
    }

    strcpy(addrSpace_str, args[0].sval);

    /* process Address space code */
    while((tvmeUniverseAddrSpace+i)->vme_address_space_s) {
        if(!strcmp((tvmeUniverseAddrSpace+i)->vme_address_space_s, addrSpace_str)) break;
        else i++;
    }

    if(!((tvmeUniverseAddrSpace+i)->vme_address_space_s)) {
        printf("drvvmeUniverse: Address space  code mismatch\n");
        return;
    }
    addrSpace = (tvmeUniverseAddrSpace+i)->vme_address_space;

    drvvmeUniverse_releaseSlaveWindow(addrSpace);
}


static const iocshArg drvvmeUniverse_sysBusToLocalAddrsArg0 = {"AM code/address space", iocshArgString};
static const iocshArg drvvmeUniverse_sysBusToLocalAddrsArg1 = {"VME window(master/slave)", iocshArgString};
static const iocshArg drvvmeUniverse_sysBusToLocalAddrsArg2 = {"VME offset", iocshArgString};
static const iocshArg* const drvvmeUniverse_sysBusToLocalAddrsArgs[] = { &drvvmeUniverse_sysBusToLocalAddrsArg0,
                                                                         &drvvmeUniverse_sysBusToLocalAddrsArg1,
                                                                         &drvvmeUniverse_sysBusToLocalAddrsArg2};
static const iocshFuncDef drvvmeUniverse_sysBusToLocalAddrsFuncDef = {"vmeUniverse_sysBusToLocalAddrs",
                                                                      3,
                                                                      drvvmeUniverse_sysBusToLocalAddrsArgs};

static void drvvmeUniverse_sysBysToLocalAddrsCallFunc(const iocshArgBuf *args)
{
    drvvmeUniverseConfig   *pdrvvmeUniverseConfig = NULL;
    char amCode_str[40];
    char vmeWindow_str[40];
    char vmeOffset_str[40];
    vme_addr_mod_t       amCode;
    vme_address_space_t  addrSpace;
    vme_window_t         vmeWindow;
    unsigned long        vmeOffset;
    void                 *vmeAddrs;
    long                 status;
    int i = 0;
    int j = 0;

    if(!args[0].sval || !args[1].sval || !args[2].sval ) {
         printf("Usage: vmeUniverse_sysBusToLocalAddrs 'AM code/Address space' 'VME window(master/slave)' 'VME offset'\n");
         return;
    }

    strcpy(amCode_str, args[0].sval);
    strcpy(vmeWindow_str, args[1].sval);
    strcpy(vmeOffset_str, args[2].sval);


    /* process VME window */
    while((tvmeUniverseWindowType+j)->vme_window_s) {
        if(!strcmp((tvmeUniverseWindowType+j)->vme_window_s, vmeWindow_str)) break;
        else j++;
    }
    vmeWindow = (tvmeUniverseWindowType+j)->vme_window;
    if(!(tvmeUniverseWindowType+j)->vme_window_s) {
        printf("drvvmeUniverse: mismatched vme window type %s\n", vmeWindow_str);
        printf("                (master_window/slave_window\n");
        return;
    }


    if(vmeWindow == master_window) {
        /* process AM code */
        while((tvmeUniverseAddrMod+i)->vme_addr_mod_s) {
            if(!strcmp((tvmeUniverseAddrMod+i)->vme_addr_mod_s, amCode_str)) break;
            else i++;
        }
        if(!((tvmeUniverseAddrMod+i)->vme_addr_mod_s)) {
            printf("drvvmeUniverse: AM code mismatch\n");
            return;
        }
        amCode = (tvmeUniverseAddrMod+i)->vme_addr_mod;

    }

    if(vmeWindow == slave_window) {
        /* process Address space */
        while((tvmeUniverseAddrSpace+i)->vme_address_space_s) {
            if(!strcmp((tvmeUniverseAddrSpace+i)->vme_address_space_s, amCode_str)) break;
            else i++;
        }

        if(!((tvmeUniverseAddrSpace+i)->vme_address_space_s)) {
            printf("drvvmeUniverse: Address space mismatch\n");
            return;
        }
        addrSpace = (tvmeUniverseAddrSpace+i)->vme_address_space;
    }

    /* process VME offset */
    if(!strncmp(vmeOffset_str, "0x", 2))
        sscanf(vmeOffset_str+2, "%lx", &vmeOffset);
    else sscanf(vmeOffset_str, "%ld", &vmeOffset);


    if(vmeWindow == master_window) pdrvvmeUniverseConfig = drvvmeUniverse_findConfigMaster(amCode);
    if(vmeWindow == slave_window)  pdrvvmeUniverseConfig = drvvmeUniverse_findConfigSlave(addrSpace);

    if(!pdrvvmeUniverseConfig && vmeWindow == master_window) {
        printf("drvvmeUniverse: %s (%s) (%s) window not found\n",
               (tvmeUniverseAddrMod+i)->vme_addr_mod_s,
               (tvmeUniverseAddrMod+i)->vme_addr_mod_desc,
               (tvmeUniverseWindowType+j)->vme_window_desc);
        return;
    }

    if(!pdrvvmeUniverseConfig && vmeWindow == slave_window) {
        printf("drvvmeUniverse: %s (%s) (%s) window not found\n",
               (tvmeUniverseAddrSpace+i)->vme_address_space_s,
               (tvmeUniverseAddrSpace+i)->vme_address_space_desc,
               (tvmeUniverseWindowType+j)->vme_window_desc);
    }
   
    drvvmeUniverse_reportConfig(pdrvvmeUniverseConfig);
    if(vmeWindow == master_window) 
        status = vmeUniverse_sysBusToLocalAddrs(amCode, vmeWindow, vmeOffset, &vmeAddrs);
    if(vmeWindow == slave_window)
        status = vmeUniverse_sysBusToLocalAddrs(addrSpace, vmeWindow, vmeOffset, &vmeAddrs);

    if(status == -1) {
        printf("drvvmeUniverse: vme window not found\n");
        return;
    }
    if(status == -2) {
        printf("drvvmeUniverse: vme window is not master window\n");
        return;
    }
    if(status == -3) {
        printf("drvvmeUniverse: out of range\n");
        return;
    }

    if(!status) {
        printf("\t        VMEbos Offset: 0x%8.8lx         Local Address: %8.8p\n", vmeOffset, vmeAddrs);
    } 

}


static const iocshArg drvvmeUniverse_peekArg0 = {"AM code", iocshArgString};
static const iocshArg drvvmeUniverse_peekArg1 = {"VMEbus Address", iocshArgString};
static const iocshArg drvvmeUniverse_peekArg2 = {"number of data", iocshArgString};
static const iocshArg drvvmeUniverse_peekArg3 = {"data width", iocshArgString};
static const iocshArg* const drvvmeUniverse_peekArgs [] = { &drvvmeUniverse_peekArg0,
                                                            &drvvmeUniverse_peekArg1,
                                                            &drvvmeUniverse_peekArg2,
                                                            &drvvmeUniverse_peekArg3 };
static const iocshFuncDef drvvmeUniverse_peekFuncDef = { "vmeUniverse_peek",
                                                         4,
                                                         drvvmeUniverse_peekArgs };
static void drvvmeUniverse_peekCallFunc(const iocshArgBuf *args)
{
    char amCode_str[40];
    char vmeAddr_str[40];
    char dataCount_str[40];
    char dataWidth_str[40];
    vme_addr_mod_t  amCode;
    unsigned long   vmeAddr;
    unsigned        dataCount;
    unsigned        dataWidth;
    int i;
    drvvmeUniversePeekConfig *pdrvvmeUniversePeekConfig = NULL;

    if(args[0].sval) {
        strcpy(amCode_str, args[0].sval);
        /* process AM code */
        i=0;
        while((tvmeUniverseAddrMod+i)->vme_addr_mod_s) {
            if(!strcmp((tvmeUniverseAddrMod+i)->vme_addr_mod_s, amCode_str)) break;
            else i++;
        }
        if(!((tvmeUniverseAddrMod+i)->vme_addr_mod_s)) {
            printf("drvvmeUniverse: AM code mismatch\n");
            return;
        }
        amCode = (tvmeUniverseAddrMod+i)->vme_addr_mod;
    }
    else amCode_str[0] = '\0';
 
    if(args[1].sval) {
        strcpy(vmeAddr_str, args[1].sval);
        /* process VMEbus addrss */
        if(!strncmp(vmeAddr_str, "0x",2))
            sscanf(vmeAddr_str+2, "%lx", &vmeAddr);
        else sscanf(vmeAddr_str, "%ld", &vmeAddr);
    }
    else vmeAddr_str[0] = '\0';

    if(args[2].sval) {
       strcpy(dataCount_str, args[2].sval);
       /* process DATA counter */
       if(!strncmp(dataCount_str, "0x", 2))
           sscanf(dataCount_str+2, "%x", &dataCount);
       else sscanf(dataCount_str, "%d", &dataCount);
    }
    else dataCount_str[0]='\0';

    if(args[3].sval) {
        strcpy(dataWidth_str, args[3].sval);
        /* process data width */
        i=0;
        while((tdrvvmeUniverseDataWidth+i)->width_str) {
            if(!strcmp((tdrvvmeUniverseDataWidth+i)->width_str, dataWidth_str)) break;
            else i++;
        }
        if(!((tdrvvmeUniverseDataWidth+i)->width_str)) {
            printf("drvvmeUniverse: data width mismatch\n");
            return;
        }
        dataWidth = (tdrvvmeUniverseDataWidth+i)->width;
    }
    else dataWidth_str[0]='\0';


    if(!pdrvvmeUniverse_PEEKellList) { /* init ellList for PEEK function */
        pdrvvmeUniverse_PEEKellList = (ELLLIST*) malloc(sizeof(ELLLIST));
        ellInit(pdrvvmeUniverse_PEEKellList);
        
    }

    if(ellCount(pdrvvmeUniverse_PEEKellList)<1) {    /* make new */
        if(!amCode_str[0] || !vmeAddr_str[0] || !dataCount_str[0] || !dataWidth_str[0]) {
            printf("\tUsage: vmeUniverse_peek [AM code] [VMEbus address] [number of data] [data width]\n");
            return;
        }

        pdrvvmeUniversePeekConfig = (drvvmeUniversePeekConfig*) malloc(sizeof(drvvmeUniversePeekConfig));
        pdrvvmeUniversePeekConfig->vme_addr_mod = amCode;
        pdrvvmeUniversePeekConfig->vme_addr = vmeAddr;
        pdrvvmeUniversePeekConfig->width    = dataWidth;
        pdrvvmeUniversePeekConfig->count    = dataCount;
        ellAdd(pdrvvmeUniverse_PEEKellList, &pdrvvmeUniversePeekConfig->node);
    }

    if(amCode_str[0]) {
        pdrvvmeUniversePeekConfig = (drvvmeUniversePeekConfig*) ellFirst(pdrvvmeUniverse_PEEKellList);
        while(pdrvvmeUniversePeekConfig) {
            if(pdrvvmeUniversePeekConfig->vme_addr_mod == amCode) break;
            else pdrvvmeUniversePeekConfig = (drvvmeUniversePeekConfig*) ellNext(&pdrvvmeUniversePeekConfig->node);
        }
        if(pdrvvmeUniversePeekConfig) ellDelete(pdrvvmeUniverse_PEEKellList, &pdrvvmeUniversePeekConfig->node);
        else {
            if(!vmeAddr_str[0] || !dataCount_str[0] || !dataWidth_str[0]) {
                printf("\tUsage: vmeUniverse_peek %s [VMEbus address] [number of data] [data width]\n",
                       amCode_str);
                return;
            }
            pdrvvmeUniversePeekConfig = (drvvmeUniversePeekConfig*) malloc(sizeof(drvvmeUniverseConfig));
            pdrvvmeUniversePeekConfig->vme_addr_mod = amCode;
            pdrvvmeUniversePeekConfig->vme_addr = vmeAddr;
            pdrvvmeUniversePeekConfig->width = dataWidth;
            pdrvvmeUniversePeekConfig->count = dataCount;
        }
       
        ellAdd(pdrvvmeUniverse_PEEKellList, &pdrvvmeUniversePeekConfig->node); 
    }
    
    pdrvvmeUniversePeekConfig = (drvvmeUniversePeekConfig*) ellLast (pdrvvmeUniverse_PEEKellList); 
    if(vmeAddr_str[0]) pdrvvmeUniversePeekConfig->vme_addr = vmeAddr;
    if(dataCount_str[0]) pdrvvmeUniversePeekConfig->count = dataCount;
    if(dataWidth_str[0]) pdrvvmeUniversePeekConfig->width = dataWidth;
    drvvmeUniverse_dumpPeekData(pdrvvmeUniversePeekConfig);
}


static const iocshArg drvvmeUniverse_pokeArg0 = {"AM code", iocshArgString};
static const iocshArg drvvmeUniverse_pokeArg1 = {"VMEbus Addrs", iocshArgString};
static const iocshArg drvvmeUniverse_pokeArg2 = {"data width", iocshArgString};
static const iocshArg drvvmeUniverse_pokeArg3 = {"data", iocshArgString};
static const iocshArg* const drvvmeUniverse_pokeArgs[] = { &drvvmeUniverse_pokeArg0,
                                                           &drvvmeUniverse_pokeArg1,
                                                           &drvvmeUniverse_pokeArg2,
                                                           &drvvmeUniverse_pokeArg3};
static const iocshFuncDef drvvmeUniverse_pokeFuncDef = {"vmeUniverse_poke",
                                                        4,
                                                        drvvmeUniverse_pokeArgs};
static void drvvmeUniverse_pokeCallFunc(const iocshArgBuf *args)
{
    char amCode_str[40];
    char vmeAddr_str[40];
    char width_str[40];
    char data_str[40];
    vme_addr_mod_t     amCode;
    unsigned long      vmeAddr;
    unsigned           width;
    unsigned long      data;
    volatile void               *localAddr;
    volatile unsigned char      *localAddr_byte;
    volatile unsigned short     *localAddr_short;
    volatile unsigned           *localAddr_word;
    int i;
    

    if(args[0].sval) {
        strcpy(amCode_str, args[0].sval);
        i=0;
        while((tvmeUniverseAddrMod+i)->vme_addr_mod_s) {
            if(!strcmp((tvmeUniverseAddrMod+i)->vme_addr_mod_s, amCode_str)) break;
            else i++;
        }
        if(!((tvmeUniverseAddrMod+i)->vme_addr_mod_s)) {
            printf("drvvmeUniverse: AM code mismatch\n");
            return;
        }
        amCode = (tvmeUniverseAddrMod+i)->vme_addr_mod;
    }
    else amCode_str[0] = '\0';

    if(args[1].sval) {
        strcpy(vmeAddr_str, args[1].sval);
        if(!strncmp(vmeAddr_str, "0x", 2)) sscanf(vmeAddr_str+2, "%lx", &vmeAddr);
        else                               sscanf(vmeAddr_str,"%ld", &vmeAddr);
    }
    else vmeAddr_str[0] = '\0';

    if(args[2].sval) {
        strcpy(width_str, args[2].sval);
        i=0;
        while((tdrvvmeUniverseDataWidth+i)->width_str) {
            if(!strcmp((tdrvvmeUniverseDataWidth+i)->width_str, width_str)) break;
            else i++;
        }
        if(!((tdrvvmeUniverseDataWidth+i)->width_str)) {
            printf("drvvmeUniverse: data width mismatch\n");
            return;
        }
        width = (tdrvvmeUniverseDataWidth+i)->width;
    }
    else width_str[0] = '\0';

    if(args[3].sval) {
        strcpy(data_str,args[3].sval);
        if(!strncmp(data_str, "0x", 2)) sscanf(data_str+2, "%lx", &data);
        else                            sscanf(data_str, "%ld", &data);
    }
    else data_str[0] = '\0';

    if(!amCode_str[0] || !vmeAddr_str[0] || !width_str[0] || !data_str[0]) {
        printf("Usage: vmeUniverse_poke 'AM code' 'VMEbus Addrs' 'data width' 'data'\n");
        return;
    }

    
    if(vmeUniverse_sysBusToLocalAddrs(amCode, master_window, vmeAddr, (void**)&localAddr)<0) {
        printf("drvvmeUniverse: VMEbus address conversion fail\n");
        return;
    }

    switch(width) {
        case 1: /* byte */
            localAddr_byte = (unsigned char*)localAddr;
            *localAddr_byte = (unsigned char) data;
            break;
        case 2: /* short */
            localAddr_short = (unsigned short*) localAddr;
            *localAddr_short = (unsigned short) data;
            break;
        case 4: /* word */
            localAddr_word  = (unsigned*) localAddr;
            *localAddr_word = (unsigned) data;
            break;
        case 8: /* double */
    }

}


static const iocshArg drvvmeUniverse_generateVMEInterruptArg0 = {"Interrupt level", iocshArgString};
static const iocshArg drvvmeUniverse_generateVMEInterruptArg1 = {"Interrupt vector", iocshArgString};
static const iocshArg* const drvvmeUniverse_generateVMEInterruptArgs[] = { &drvvmeUniverse_generateVMEInterruptArg0,
                                                                           &drvvmeUniverse_generateVMEInterruptArg1 };
static const iocshFuncDef drvvmeUniverse_generateVMEInterruptFuncDef = {"vmeUniverse_generateVMEInterrupt",
                                                                        2,
                                                                        drvvmeUniverse_generateVMEInterruptArgs};
static void drvvmeUniverse_generateVMEInterruptCallFunc(const iocshArgBuf *args) {
    int status;
    char level_str[40];
    char vector_str[40];
    int level;
    int vector;
    int i;

    if(!takeVME) {
        if(vme_init(&drvvmeUniverse_busHandle)) {
            printf("drvvmeUniverse: vme_init fault\n");
            return;
        }
        else takeVME = 1;
    }

    if(!args[0].sval || !args[1].sval) {
        printf("Usage: vmeUniverse_generateVMEInterrupt 'level' 'vector'\n");
        return;
    }

    strcpy(level_str, args[0].sval);
    strcpy(vector_str, args[1].sval);

    /* process interrupt level */
    i = 0;
    while((tvmeUniverseVMEInterruptLevelIntMacroString+i)->level_string) {
        if(!strcmp((tvmeUniverseVMEInterruptLevelIntMacroString+i)->level_string, level_str)) break;
        else i++;
    }
    if((tvmeUniverseVMEInterruptLevelIntMacroString)->level <0) {
        printf("drvvmeUniverse: wrong VMEInterrupt level\n");
        return;
    }
    level = (tvmeUniverseVMEInterruptLevelIntMacroString+i)->level_macro;
     

    /* process interrupt vector */
    if(!strncmp(vector_str, "0x", 2))
        sscanf(vector_str+2, "%x", &vector);
    else sscanf(vector_str, "%d", &vector);

    if(vector<0 || vector > 255) {
        printf("drvvmeUniverse: wrong VMEInterrupt vector\n");
        return;
    }

    status = vme_interrupt_generate(drvvmeUniverse_busHandle, level, vector);
    if(status) printf("drvvmeUniverse: interrupt generatation fault %s, %d\n",
                      (tvmeUniverseVMEInterruptLevelIntMacroString+i)->level_string, vector);
}


/* 
** You don't have to make VMEInterruptHandler explict.
** It will be made automatically when you regist ISR.
static const iocshArg drvvmeUniverse_makeVMEInterruptHandlerArg0 = {"Interrupt level", iocshArgString};
static const iocshArg drvvmeUniverse_makeVMEInterruptHandlerArg1 = {"Interrupt vector", iocshArgString};
static const iocshArg* const drvvmeUniverse_makeVMEInterruptHandlerArgs[] = { &drvvmeUniverse_makeVMEInterruptHandlerArg0,
                                                                              &drvvmeUniverse_makeVMEInterruptHandlerArg1};
static const iocshFuncDef drvvmeUniverse_makeVMEInterruptHandlerFuncDef = {"vmeUniverse_makeVMEInterruptHandler",
                                                                           2,
                                                                           drvvmeUniverse_makeVMEInterruptHandlerArgs};
static void drvvmeUniverse_makeVMEInterruptHandlerCallFunc(const iocshArgBuf *args)
{
    int status;
    char level_str[40];
    char vector_str[40];
    int level;
    int vector;
    int i;

    drvvmeUniverse_VMEInterruptConfig *pConfig;
    drvvmeUniverse_VMEInterruptISRConfig  *pISRConfig;


    if(!takeVME) {
        if(vme_init(&drvvmeUniverse_busHandle)) {
            printf("drvvmeUniverse: vme_init fault\n");
            return;
        }
        else takeVME = 1;
    }

    if(!args[0].sval || !args[1].sval) {
        printf("Usage: vmeUniverse_generateVMEInterrupt 'level' 'vector'\n");
        return;
    }

    strcpy(level_str, args[0].sval);
    strcpy(vector_str, args[1].sval);

    i = 0;
    while((tvmeUniverseVMEInterruptLevelIntMacroString+i)->level_string) {
        if(!strcmp((tvmeUniverseVMEInterruptLevelIntMacroString+i)->level_string, level_str)) break;
        else i++;
    }
    if((tvmeUniverseVMEInterruptLevelIntMacroString)->level <0) {
        printf("drvvmeUniverse: wrong VMEInterrupt level\n");
        return;
    }
    level = (tvmeUniverseVMEInterruptLevelIntMacroString+i)->level;
     

    if(!strncmp(vector_str, "0x", 2))
        sscanf(vector_str+2, "%x", &vector);
    else sscanf(vector_str, "%d", &vector);

    if(vector<0 || vector > 255) {
        printf("drvvmeUniverse: wrong VMEInterrupt vector\n");
        return;
    }

    vmeUniverse_intConnect(vector, myISR, "ARG");
       pISRConfig = ellFirst((pdrvvmeUniverse_VMEInterruptVector+vector)->pISRellList); 
       printf("%p\n", pISRConfig->pFunction);
       printf("%p\n", pISRConfig->param);
       printf("%s\n", pISRConfig->pFileName);
       printf("%s\n", pISRConfig->pFuncName);
       printf("%d\n", pISRConfig->line);
}*/

static void epicsShareAPI drvvmeUniverseRegistrar(void)
{
    static int firstTime = 1;
  
    if(firstTime) {
        firstTime = 0;
        iocshRegister(&drvvmeUniverse_printInfoFuncDef, drvvmeUniverse_printInfoCallFunc);
        iocshRegister(&drvvmeUniverse_createMasterWindowFuncDef, drvvmeUniverse_createMasterWindowCallFunc);
        iocshRegister(&drvvmeUniverse_releaseMasterWindowFuncDef, drvvmeUniverse_releaseMasterWindowCallFunc);
        iocshRegister(&drvvmeUniverse_createSlaveWindowFuncDef, drvvmeUniverse_createSlaveWindowCallFunc);
        iocshRegister(&drvvmeUniverse_releaseSlaveWindowFuncDef, drvvmeUniverse_releaseSlaveWindowCallFunc);
        iocshRegister(&drvvmeUniverse_sysBusToLocalAddrsFuncDef, drvvmeUniverse_sysBysToLocalAddrsCallFunc);
        iocshRegister(&drvvmeUniverse_peekFuncDef, drvvmeUniverse_peekCallFunc);
        iocshRegister(&drvvmeUniverse_pokeFuncDef, drvvmeUniverse_pokeCallFunc);
        iocshRegister(&drvvmeUniverse_generateVMEInterruptFuncDef, drvvmeUniverse_generateVMEInterruptCallFunc);
    }
}

epicsExportRegistrar(drvvmeUniverseRegistrar);
    




LOCAL long drvvmeUniverse_io_report(int level)
{
    long status = 0;
    int  i      = 0;
    drvvmeUniverseConfig *pdrvvmeUniverseConfig;

    if(pdrvvmeUniverse_ellList) {
        printf("\tTotal %d VME window(s) found\n", ellCount(pdrvvmeUniverse_ellList));
        if(level>0) {
            pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) ellFirst(pdrvvmeUniverse_ellList);
            while(pdrvvmeUniverseConfig) {
                printf("\ndrvvmeUniverse: node %d\n",i++);
                drvvmeUniverse_reportConfig(pdrvvmeUniverseConfig);
                pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) ellNext(&pdrvvmeUniverseConfig->node);
            }
        }
        
    }
    else printf("\tCan not find VME windows\n");

    if(level>1) {
        if(pdrvvmeUniverse_DMAellList) {
            printf("\n\tTotal %d DMA buffer(s) found\n", ellCount(pdrvvmeUniverse_DMAellList));
            if(level>1) {
            }
        }
        else printf("\n\tCan not find DMA buffer(s)\n");
    }

    if(level>3) {
        drvvmeUniverse_printCtl();
        drvvmeUniverse_printInterrupt();
        drvvmeUniverse_printMaster();
        drvvmeUniverse_printSlave();
        drvvmeUniverse_printIoMem();
        drvvmeUniverse_printISR();
    }

    return status;
}


LOCAL long drvvmeUniverse_init_driver(void)
{
    long status = 0;
    int i;

    struct sigaction *paction;
  
    paction = (struct sigaction*) malloc (sizeof(struct sigaction));
    sigemptyset(&paction->sa_mask);
    paction->sa_sigaction = drvvmeUniverse_VMEInterruptHandlerFunc;
    paction->sa_flags = SA_SIGINFO;

    sigaction(SIGIO, paction, NULL);


    #define UNIVERSE_DEVICE "/dev/bus/vme/ctl"
    univ_fd = open(UNIVERSE_DEVICE, O_RDWR);
    if (univ_fd == -1 ) {
        perror("Cannot open device file");
        status = -1;
    }


    pdrvvmeUniverse_VMEInterruptLevel 
        = (drvvmeUniverse_VMEInterruptLevel*) malloc(sizeof(drvvmeUniverse_VMEInterruptLevel)*7);
    pdrvvmeUniverse_VMEInterruptVector
        = (drvvmeUniverse_VMEInterruptVector*) malloc(sizeof(drvvmeUniverse_VMEInterruptVector)*256);

    for(i=0; i<7; i++) {
        (pdrvvmeUniverse_VMEInterruptLevel+i)->enableDisable = enable;
        (pdrvvmeUniverse_VMEInterruptLevel+i)->connected     = disable;
        (pdrvvmeUniverse_VMEInterruptLevel+i)->lock          = epicsMutexCreate();
    }

    for(i=0; i<256; i++) {
        (pdrvvmeUniverse_VMEInterruptVector+i)->pISRellList = (ELLLIST*) malloc(sizeof(ELLLIST));
        ellInit((pdrvvmeUniverse_VMEInterruptVector+i)->pISRellList);
        
        (pdrvvmeUniverse_VMEInterruptVector+i)->lock        = epicsMutexCreate();
        (pdrvvmeUniverse_VMEInterruptVector+i)->counter     = 0;
    }

    return status;
}


LOCAL void drvvmeUniverse_printCtl(void)
{
    char *filename = "/proc/vme/ctl";
    FILE *fp;
    char *pstr;
    char tempStr[135];

    fp = fopen(filename, "r");
    if(!fp) {
        printf("drvvmeUniverse: ctl file not found\n");
        return;
    }

    printf("drvvmeUniverse: vme/ctl\n");
    while(1) {
        pstr = fgets(tempStr, 134, fp);
        if(!pstr) break;
        printf("%s", pstr);
    }

    fclose(fp);
}

LOCAL void drvvmeUniverse_printInterrupt(void)
{
    char *filename = "/proc/vme/interrupt";
    FILE *fp;
    char *pstr;
    char tempStr[135];

    fp = fopen(filename, "r");
    if(!fp) {
        printf("drvvmeUniverse: interrupt file not found\n");
        return;
    }

    printf("drvvmeUniverse: vme/interrupt\n");
    while(1) {
        pstr = fgets(tempStr, 134, fp);
        if(!pstr) break;
        printf("%s", pstr);
    }

    fclose(fp);
}

LOCAL void drvvmeUniverse_printMaster(void)
{
    char *filename = "/proc/vme/master";
    FILE *fp;
    char *pstr;
    char tempStr[135];

    fp = fopen(filename, "r");
    if(!fp) {
        printf("drvvmeUniverse: master file not found\n");
        return;
    }

    printf("drvvmeUniverse: vme/master\n");
    while(1) {
        pstr = fgets(tempStr, 134, fp);
        if(!pstr) break;
        printf("%s", pstr);
    }

    fclose(fp);
}

LOCAL void drvvmeUniverse_printSlave(void)
{
    char *filename = "/proc/vme/slave";
    FILE *fp;
    char *pstr;
    char tempStr[135];

    fp = fopen(filename, "r");
    if(!fp) {
        printf("drvvmeUniverse: slave file not found\n");
        return;
    }

    printf("drvvmeUniverse: vme/slave\n");
    while(1) {
        pstr = fgets(tempStr, 134, fp);
        if(!pstr) break;
        printf("%s", pstr);
    }

    fclose(fp);
}

LOCAL void drvvmeUniverse_printIoMem(void)
{
    char *filename = "/proc/iomem";
    FILE *fp;
    char *pstr;
    char tempStr[135];

    fp = fopen(filename, "r");
    if(!fp) {
        printf("drvvmeUniverse: iomem file not found\n");
        return;
    }

    printf("drvvmeUniverse: iomem\n");
    while(1) {
        pstr = fgets(tempStr, 134, fp);
        if(!pstr) break;
        printf("%s", pstr);
    }

    fclose(fp);
}

LOCAL void drvvmeUniverse_printISR(void)
{
    drvvmeUniverse_VMEInterruptVector *pVector;
    drvvmeUniverse_VMEInterruptISRConfig *pISRConfig;
    int vector;
    int connectedVectors = 0;
    int disconnectedVectors = 0;
    int i;

    printf("drvvmeUniverse: isr information\n");
    for(vector=0; vector<256; vector++) {
        pVector = pdrvvmeUniverse_VMEInterruptVector+vector;
        if(ellCount(pVector->pISRellList)<1) {
            disconnectedVectors++;
            continue;
        }

        connectedVectors++;
        printf("  Vector[%d]: %d ISR(s) connected, %d times called\n",
               vector, ellCount(pVector->pISRellList), pVector->counter);
        i=0;
        pISRConfig = (drvvmeUniverse_VMEInterruptISRConfig*) ellFirst(pVector->pISRellList);
        while(pISRConfig) {
            printf("      %d: %s:%d: %s, %d times called \n", 
                   i++, pISRConfig->pFileName, pISRConfig->line, pISRConfig->pFuncName, pISRConfig->counter);
            printf("                 ISR pointer %p, Param pointer %p\n",
                   pISRConfig->pFunction, pISRConfig->param);
            pISRConfig = (drvvmeUniverse_VMEInterruptISRConfig*) ellNext(&pISRConfig->node);
        }

    }
    printf("    Total: %d vectors connected / %d vectors disconnected\n",
                connectedVectors, disconnectedVectors);

}


LOCAL void drvvmeUniverse_dumpPeekData(drvvmeUniversePeekConfig *pdrvvmeUniversePeekConfig)
{
    void *localAddr;

    
    if(vmeUniverse_sysBusToLocalAddrs(pdrvvmeUniversePeekConfig->vme_addr_mod,
                                      master_window,
                                      pdrvvmeUniversePeekConfig->vme_addr,
                                      &localAddr) < 0) {
        printf("drvvmeUniverse: fault from sysButToLocalAddrs\n");
        return;
    }

    switch(pdrvvmeUniversePeekConfig->width) {
        case 1:
                  drvvmeUniverse_dumpPeekDataByte((unsigned char*) localAddr, 
                                                  pdrvvmeUniversePeekConfig->vme_addr,
                                                  pdrvvmeUniversePeekConfig->count);
                  break;
        case 2:
                  drvvmeUniverse_dumpPeekDataShort((unsigned short*) localAddr, 
                                                   pdrvvmeUniversePeekConfig->vme_addr,
                                                   pdrvvmeUniversePeekConfig->count);
                  break;
        case 4:
                  drvvmeUniverse_dumpPeekDataWord((unsigned*) localAddr, 
                                                  pdrvvmeUniversePeekConfig->vme_addr,
                                                  pdrvvmeUniversePeekConfig->count);
                  break;
        case 8:
                  drvvmeUniverse_dumpPeekDataDouble((unsigned long*) localAddr,
                                                    pdrvvmeUniversePeekConfig->vme_addr,
                                                    pdrvvmeUniversePeekConfig->count);
    }
    pdrvvmeUniversePeekConfig->vme_addr += pdrvvmeUniversePeekConfig->width * pdrvvmeUniversePeekConfig->count;
}

LOCAL void drvvmeUniverse_dumpPeekDataByte(unsigned char *localAddr, unsigned long vme_addr, int count)
{
    int i;

    for(i=0; i<count; i++, localAddr++, vme_addr+= sizeof(unsigned char)) {
        if(i==0 || !(i % 0x10)) printf("\n%.8lx : ", vme_addr);
        printf("%.2x ", *localAddr);
    }
    printf("\n");
}

LOCAL void drvvmeUniverse_dumpPeekDataShort(unsigned short *localAddr, unsigned long vme_addr, int count)
{
    int i;
    for(i=0; i<count; i++, localAddr++, vme_addr+= sizeof(unsigned short)){
        if(i==0 || !(i % 0x08)) printf("\n%.8lx : ", vme_addr);
        printf("%.4x ", *localAddr);
    }
    printf("\n");
}

LOCAL void drvvmeUniverse_dumpPeekDataWord(unsigned *localAddr, unsigned long vme_addr, int count)
{
    int i;
    for(i=0; i<count; i++, localAddr++, vme_addr+= sizeof(unsigned)) {
        if(i==0 || !(i % 0x04)) printf("\n%.8lx : ", vme_addr);
        printf("%.8x ", *localAddr);
    }
    printf("\n");
   
}

LOCAL void drvvmeUniverse_dumpPeekDataDouble(unsigned long *localAddr, unsigned long vme_addr, int count)
{
    int i;
    for(i=0; i<count; i++, localAddr++, vme_addr+= sizeof(unsigned long)) {
        if(i==0 || !(i % 0x02)) printf("\n%.8lx : ", vme_addr);
        printf("%.16x ", *localAddr);
    }
    printf("\n");
}


LOCAL void drvvmeUniverse_reportConfig(drvvmeUniverseConfig *pdrvvmeUniverseConfig)
{
    int i;
    char *vme_window_desc;
    char *vme_window_s;
    char *vme_addr_mod_desc;
    char *vme_addr_mod_s;
    char *vme_addr_space_desc;
    char *vme_addr_space_s;
    int endianConversion;
    int endianConversionBypassed;


    /* make window information */
    i=0;
    while((tvmeUniverseWindowType+i)->vme_window_desc) {
        if((tvmeUniverseWindowType+i)->vme_window == pdrvvmeUniverseConfig->vme_window) break;
        i++;
    }
    vme_window_desc = (tvmeUniverseWindowType+i)->vme_window_desc;
    vme_window_s    = (tvmeUniverseWindowType+i)->vme_window_s;

    if(pdrvvmeUniverseConfig->vme_window == master_window) {
        /* make AM information */
        i = 0;
        while((tvmeUniverseAddrMod+i)->vme_addr_mod_desc) {
            if((tvmeUniverseAddrMod+i)->vme_addr_mod == pdrvvmeUniverseConfig->vme_addr_mod) break;
            else i++;
        }
        vme_addr_mod_desc = (tvmeUniverseAddrMod+i)->vme_addr_mod_desc;
        vme_addr_mod_s    = (tvmeUniverseAddrMod+i)->vme_addr_mod_s;
    }

    if(pdrvvmeUniverseConfig->vme_window == slave_window) {
        /* make address space information */
        i = 0;
        while((tvmeUniverseAddrSpace+i)->vme_address_space_desc) {
            if((tvmeUniverseAddrSpace+i)->vme_address_space == pdrvvmeUniverseConfig->vme_addr_space) break;
            else i++;
        }
        vme_addr_space_desc = (tvmeUniverseAddrSpace+i)->vme_address_space_desc;
        vme_addr_space_s    = (tvmeUniverseAddrSpace+i)->vme_address_space_s;
       
    }

    switch(pdrvvmeUniverseConfig->vme_window) {
        case master_window:
            if(vme_get_master_endian_conversion(drvvmeUniverse_busHandle, &endianConversion) < 0 )
                endianConversion = -1;
            break;
        case slave_window:
            if(vme_get_slave_endian_conversion(drvvmeUniverse_busHandle, &endianConversion) < 0 )
                endianConversion = -1;
            break;
    }
    if(vme_get_endian_conversion_bypass(drvvmeUniverse_busHandle, &endianConversionBypassed) < 0)
        endianConversionBypassed = -1;

    if(pdrvvmeUniverseConfig->vme_window == master_window) 
    printf("\tWindow: %s (%s)\n\tVMEbus: %s (%s)\n", vme_window_desc, vme_window_s,
                                                     vme_addr_mod_desc, vme_addr_mod_s);
    if(pdrvvmeUniverseConfig->vme_window == slave_window)
    printf("\tWindow: %s (%s)\n\tVMEbus: %s (%s)\n", vme_window_desc, vme_window_s,
                                                     vme_addr_space_desc, vme_addr_space_s);
    printf("\t        VMEbus addr: 0x%8.8lx    size: 0x%8.8lx\n", pdrvvmeUniverseConfig->vmeAddr,
                                                                  pdrvvmeUniverseConfig->windowSize);
    printf("\t        Local mapped pointer       : %8.8p\n", pdrvvmeUniverseConfig->mapped_pointer);
    if(pdrvvmeUniverseConfig->vme_window == master_window)
    printf("\t        Master window physical addr: %8.8p\n", pdrvvmeUniverseConfig->phys_addr);
    if(pdrvvmeUniverseConfig->vme_window == slave_window)
    printf("\t        Slave window physical addr: %8.8p\n", pdrvvmeUniverseConfig->phys_addr);
    printf("\t        Endian Conversion          : %d, %d\n", endianConversion, endianConversionBypassed);
  
}



LOCAL void drvvmeUniverse_createMasterWindow(vme_addr_mod_t amCode, unsigned long vmeAddr, unsigned long size)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = NULL;
  
    static char firstTime = 1;

    if(firstTime) {
        firstTime = 0;
        if(!takeVME) {
            if(vme_init(&drvvmeUniverse_busHandle)) {
                printf("drvvmeUniverse: vmeUniverse_createMasterWindow can not do vme_init\n");
                return;
            }
            else takeVME = 1;
       }
    }

    if(!pdrvvmeUniverse_ellList) { /* init ellList for pdrvvmeUniverseConfig */
        pdrvvmeUniverse_ellList = (ELLLIST*) malloc(sizeof(ELLLIST));
        ellInit(pdrvvmeUniverse_ellList);
    }

    if(pdrvvmeUniverseConfig = drvvmeUniverse_findConfigMaster(amCode)) {
        printf("drvvmeUniverse: vmeUniverse_createMasterWindow detects the window already installed\n");
        return;        
    }
   
    pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) malloc(sizeof(drvvmeUniverseConfig));
    pdrvvmeUniverseConfig->vme_window = master_window;
    pdrvvmeUniverseConfig->vme_addr_mod = amCode;
    pdrvvmeUniverseConfig->vmeAddr      = vmeAddr;
    pdrvvmeUniverseConfig->windowSize   = size;

    if(vme_master_window_create(drvvmeUniverse_busHandle, &pdrvvmeUniverseConfig->vme_master_handle,
                                vmeAddr, amCode, size, VME_CTL_PWEN, NULL)) {
        printf("drvvmeUniverse: vmeUniverse_createMasterWindow can not create master window\n");
        free(pdrvvmeUniverseConfig);
        return;
    }

    pdrvvmeUniverseConfig->mapped_pointer = vme_master_window_map(drvvmeUniverse_busHandle,
                                                                  pdrvvmeUniverseConfig->vme_master_handle,
                                                                  0);
    if(!pdrvvmeUniverseConfig->mapped_pointer) {
        printf("drvvmeUniverse: vmeUniverse_createMasterWindow can not make mapping\n");
        vme_master_window_release(drvvmeUniverse_busHandle, pdrvvmeUniverseConfig->vme_master_handle);
        free(pdrvvmeUniverseConfig);
        return;
    }

    pdrvvmeUniverseConfig->phys_addr = vme_master_window_phys_addr(drvvmeUniverse_busHandle,
                                                                   pdrvvmeUniverseConfig->vme_master_handle);

    ellAdd(pdrvvmeUniverse_ellList, &pdrvvmeUniverseConfig->node);

}


LOCAL void drvvmeUniverse_releaseMasterWindow(vme_addr_mod_t amCode)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = drvvmeUniverse_findConfigMaster(amCode);

    if(!pdrvvmeUniverseConfig) {
        printf("drvvmeUniverse: window not found\n");
        return;
    }
    vme_master_window_unmap(drvvmeUniverse_busHandle, pdrvvmeUniverseConfig->vme_master_handle);
    vme_master_window_release(drvvmeUniverse_busHandle, pdrvvmeUniverseConfig->vme_master_handle);

    ellDelete(pdrvvmeUniverse_ellList, &pdrvvmeUniverseConfig->node);
    free(pdrvvmeUniverseConfig);
 
}


LOCAL void drvvmeUniverse_createSlaveWindow(vme_address_space_t addrSpace, unsigned long vmeAddr, unsigned long size)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = NULL;

    static char firstTime =1;

    if(firstTime) {
        firstTime = 0;
        if(!takeVME) {
            if(vme_init(&drvvmeUniverse_busHandle)) {
                printf("drvvmeUniverse: vmeUniverse_createSlaveWindow can not do vme_init\n");
                return;
            }
        } 
        else takeVME =1;
    }


    if(!pdrvvmeUniverse_ellList) { /* init ellList for pdrvvmeUniverseConfig */
        pdrvvmeUniverse_ellList = (ELLLIST*) malloc(sizeof(ELLLIST));
        ellInit(pdrvvmeUniverse_ellList);
    }

    if(pdrvvmeUniverseConfig = drvvmeUniverse_findConfigSlave(addrSpace)) {
        printf("drvvmeUniverse: vmeUniverse_createSlaveWindow detects the window already installed\n");
        return;
    }

    pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) malloc(sizeof(drvvmeUniverseConfig));
    pdrvvmeUniverseConfig->vme_window = slave_window;
    pdrvvmeUniverseConfig->vme_addr_space = addrSpace ;
    pdrvvmeUniverseConfig->vmeAddr      = vmeAddr;
    pdrvvmeUniverseConfig->windowSize   = size;

    if(vme_slave_window_create(drvvmeUniverse_busHandle, &pdrvvmeUniverseConfig->vme_slave_handle,
                               vmeAddr, addrSpace, size, (VME_CTL_PWEN | VME_CTL_PREN), NULL)) {
        printf("drvvmeUniverse: vmeUniverse_createSlaveWindow can not create slave window\n");
        free(pdrvvmeUniverseConfig);
        return;
    }

    pdrvvmeUniverseConfig->mapped_pointer = vme_slave_window_map(drvvmeUniverse_busHandle,
                                                                 pdrvvmeUniverseConfig->vme_slave_handle,
                                                                 0);

    if(!pdrvvmeUniverseConfig->mapped_pointer) {
        printf("drvvmeUniverse: vmeUniverse_createSlaveWindow can not make mapping\n");
        vme_slave_window_release(drvvmeUniverse_busHandle, pdrvvmeUniverseConfig->vme_slave_handle);
        free(pdrvvmeUniverseConfig);
        return;
    }

    pdrvvmeUniverseConfig->phys_addr = vme_slave_window_phys_addr(drvvmeUniverse_busHandle,
                                                                  pdrvvmeUniverseConfig->vme_slave_handle);

    ellAdd(pdrvvmeUniverse_ellList, &pdrvvmeUniverseConfig->node);
    
}


LOCAL void drvvmeUniverse_releaseSlaveWindow(vme_address_space_t addrSpace)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = drvvmeUniverse_findConfigSlave(addrSpace);

    if(!pdrvvmeUniverseConfig) {
        printf("drvvmeUniverse: window not found\n");
        return;
    }

    vme_slave_window_unmap(drvvmeUniverse_busHandle, pdrvvmeUniverseConfig->vme_slave_handle);
    vme_slave_window_release(drvvmeUniverse_busHandle, pdrvvmeUniverseConfig->vme_slave_handle);

    ellDelete(pdrvvmeUniverse_ellList, &pdrvvmeUniverseConfig->node);
    free(pdrvvmeUniverseConfig);
}


LOCAL drvvmeUniverseConfig* drvvmeUniverse_findConfigMaster(vme_addr_mod_t amCode)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = NULL;

    if(!pdrvvmeUniverse_ellList) return pdrvvmeUniverseConfig;

    pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) ellFirst(pdrvvmeUniverse_ellList);
    while(pdrvvmeUniverseConfig) {
        if(pdrvvmeUniverseConfig->vme_addr_mod == amCode &&
           pdrvvmeUniverseConfig->vme_window   == master_window) break;
        pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) ellNext(&pdrvvmeUniverseConfig->node);
    }

    return pdrvvmeUniverseConfig;
}


LOCAL drvvmeUniverseConfig* drvvmeUniverse_findConfigSlave(vme_address_space_t addrSpace)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = NULL;

    if(!pdrvvmeUniverse_ellList) return pdrvvmeUniverseConfig;

    pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) ellFirst(pdrvvmeUniverse_ellList);
    while(pdrvvmeUniverseConfig) {
        if(pdrvvmeUniverseConfig->vme_addr_space == addrSpace &&
           pdrvvmeUniverseConfig->vme_window     == slave_window) break;

        pdrvvmeUniverseConfig = (drvvmeUniverseConfig*) ellNext(&pdrvvmeUniverseConfig->node);
    }

    return pdrvvmeUniverseConfig;
}




LOCAL void drvvmeUniverse_VMEInterruptHandlerFunc(int sig, siginfo_t *siginfo, void *extra)
{
    int level  = (siginfo->si_value.sival_int) >> 8;                   /* take interrupt level from sig info */
    int vector = (siginfo->si_value.sival_int) & 0x000000ff;           /* take interrupt vector from sig info */

    drvvmeUniverse_VMEInterruptLevel *pLevel = pdrvvmeUniverse_VMEInterruptLevel+(level-1);
    drvvmeUniverse_VMEInterruptVector *pVector = pdrvvmeUniverse_VMEInterruptVector+vector;
    drvvmeUniverse_VMEInterruptISRConfig *pISRConfig;


    if(pLevel->enableDisable == disable ||                             /* check up disable/enable the level */
       ellCount(pVector->pISRellList)<1) return;                       /* check up connection of ISR */

    epicsMutexLock(pVector->lock);                                     /* make critical session */
        pVector->counter++;                         
        pISRConfig = (drvvmeUniverse_VMEInterruptISRConfig*) ellFirst(pVector->pISRellList);
        while(pISRConfig) {
            (*pISRConfig->pFunction)(pISRConfig->param);               /* DO the ISR */
            pISRConfig->counter++;
            pISRConfig = (drvvmeUniverse_VMEInterruptISRConfig*) ellNext(&pISRConfig->node);
        }

      if (ioctl(univ_fd, VMECTL_ENINT, level)) {
          perror("ioctl VMECTL_ENINT failed\n");
      }


    epicsMutexUnlock(pVector->lock);
}



LOCAL void drvvmeUniverse_initVMEInterruptellList(void)
{
    if(!pdrvvmeUniverse_VMEInterruptellList) {
        pdrvvmeUniverse_VMEInterruptellList = (ELLLIST*) malloc(sizeof(ELLLIST));
        ellInit(pdrvvmeUniverse_VMEInterruptellList);
    }
}

LOCAL drvvmeUniverse_VMEInterruptConfig* drvvmeUniverse_findVMEInterruptConfig(int interrupt_data)
{
    drvvmeUniverse_VMEInterruptConfig *p = (drvvmeUniverse_VMEInterruptConfig*) NULL;

    if(!pdrvvmeUniverse_VMEInterruptellList ||
       ellCount(pdrvvmeUniverse_VMEInterruptellList)< 1) return p;

    p = (drvvmeUniverse_VMEInterruptConfig*) ellFirst(pdrvvmeUniverse_VMEInterruptellList);
    while(p) {
        if(p->interrupt_data == interrupt_data) break;
        else p = (drvvmeUniverse_VMEInterruptConfig*) ellNext(&p->node);
    }

    return p;
}

LOCAL drvvmeUniverse_VMEInterruptConfig* drvvmeUniverse_makeVMEInterruptConfig(unsigned char level, unsigned char vector)
{
    drvvmeUniverse_VMEInterruptConfig *pConfig = (drvvmeUniverse_VMEInterruptConfig*) NULL;
    int interrupt_data                         = (level<<8 | vector);

    if(level<0 || level>7 || vector%2) return pConfig;

    drvvmeUniverse_initVMEInterruptellList();

    pConfig = drvvmeUniverse_findVMEInterruptConfig(interrupt_data);
    if(pConfig) return pConfig;

    pConfig = (drvvmeUniverse_VMEInterruptConfig*) malloc (sizeof(drvvmeUniverse_VMEInterruptConfig));
    if(!pConfig) return pConfig;

    pConfig->level = level;
    pConfig->vector = vector;
    pConfig->interrupt_data = interrupt_data;
    pConfig->level_macro = (tvmeUniverseVMEInterruptLevelIntMacroString+(level-1))->level_macro;
    pConfig->psigevent = (struct sigevent*) malloc(sizeof(struct sigevent));


    ellAdd(pdrvvmeUniverse_VMEInterruptellList, &pConfig->node);

    drvvmeUniverse_attachVMEInterruptMonitor(pConfig);

    return pConfig;

}

LOCAL long drvvmeUniverse_attachVMEInterruptMonitor(drvvmeUniverse_VMEInterruptConfig *pConfig)
{
    long status = 0;
    struct sigevent *psigevent = pConfig->psigevent;

    psigevent->sigev_signo = SIGIO;
    psigevent->sigev_notify = SIGEV_SIGNAL;
    psigevent->sigev_value.sival_int = 0;

    vme_interrupt_attach(drvvmeUniverse_busHandle, &pConfig->interrupt_handle,
                         pConfig->level, pConfig->vector,
                         VME_INTERRUPT_SIGEVENT, pConfig->psigevent);


    return status;
}

LOCAL long drvvmeUniverse_makeInterruptConfig(int vector)
{
    long status  = 0;
    unsigned char level;

    for(level = 1; level <= 7; level++) {
        drvvmeUniverse_makeVMEInterruptConfig(level, (unsigned char) vector);
        (pdrvvmeUniverse_VMEInterruptLevel+(level-1))->connected = enable;
    }

    return status;

}

long epicsShareAPI drvvmeUniverse_makeISR(unsigned vector, void (*pFunction)(void*), void *param,
                                  char *pFileName, int line, char *pFuncName)
{
    long status = 0;
    drvvmeUniverse_VMEInterruptISRConfig *pISRConfig;

    if(vector<0 || vector>255 || vector%2) return status=1;

    pISRConfig = (drvvmeUniverse_VMEInterruptISRConfig*) malloc(sizeof(drvvmeUniverse_VMEInterruptISRConfig));
    if(!pISRConfig) return status = 1;

    pISRConfig->pFunction = pFunction;
    pISRConfig->param     = param;
    pISRConfig->pFileName = pFileName;
    pISRConfig->pFuncName = pFuncName;
    pISRConfig->line      = line;
    pISRConfig->counter   = 0;

    epicsMutexLock((pdrvvmeUniverse_VMEInterruptVector+vector)->lock);
        ellAdd((pdrvvmeUniverse_VMEInterruptVector+vector)->pISRellList, &pISRConfig->node);
    epicsMutexUnlock((pdrvvmeUniverse_VMEInterruptVector+vector)->lock);

    
    status = drvvmeUniverse_makeInterruptConfig((int)vector);

    return status;
}




long epicsShareAPI vmeUniverse_sysBusToLocalAddrs(int vme_addr_mod_space, vme_window_t vmeWindow, 
                                                  unsigned long offset, void **localAddrs)
{
    drvvmeUniverseConfig *pdrvvmeUniverseConfig = NULL;
    

    if(vmeWindow == master_window) 
        pdrvvmeUniverseConfig = drvvmeUniverse_findConfigMaster((vme_addr_mod_t) vme_addr_mod_space);
    if(vmeWindow == slave_window)  
        pdrvvmeUniverseConfig = drvvmeUniverse_findConfigSlave((vme_address_space_t) vme_addr_mod_space);


    /* check up the window */
    if(!pdrvvmeUniverseConfig) {
        *localAddrs = (void*) NULL;
        return -1;
    }

    /* check up the window type, must be master_window */
    /**  now, it has to correspond to slave_window also.
     **  so, this is ignorable error.
    if(pdrvvmeUniverseConfig->vme_window != master_window) {
        *localAddrs = (void*) NULL;
        return -2;
    } 
    */

    /* check up window size and range */
    if(pdrvvmeUniverseConfig->vmeAddr > offset || (pdrvvmeUniverseConfig->vmeAddr+pdrvvmeUniverseConfig->windowSize) < offset ) {
        *localAddrs = (void*) NULL;
        return -3;
    }


    *localAddrs = (pdrvvmeUniverseConfig->mapped_pointer)+(offset - pdrvvmeUniverseConfig->vmeAddr);
    return 0;

}

long epicsShareAPI vmeUniverse_generateVMEInterrupt(int level, int vector)
{
     long status = (long) vme_interrupt_generate(drvvmeUniverse_busHandle, level, vector);
     return status;
}
