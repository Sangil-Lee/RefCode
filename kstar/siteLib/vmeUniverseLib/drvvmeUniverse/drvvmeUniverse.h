#ifndef drvvmeUniverse_H
#define drvvmeUniverse_H

#include <signal.h>
#include <stdio.h>
#include <vme/vme.h>
#include <vme/vme_api.h>

#include "ellLib.h"
#include "iocsh.h"

#include "drvSup.h"
#include "devSup.h"
#include "recSup.h"
#include "dbScan.h"
#include "shareLib.h"



typedef enum{
    master_window,
    slave_window
} vme_window_t;

typedef struct {
    vme_window_t   vme_window;
    char           *vme_window_s;
    char           *vme_window_desc;
} vmeUniverseWindowType;


typedef struct {
    vme_addr_mod_t vme_addr_mod;
    char           *vme_addr_mod_s;
    char           *vme_addr_mod_desc;
} vmeUniverseAddrMod;

typedef struct {
    vme_address_space_t   vme_address_space;
    char                  *vme_address_space_s;
    char                  *vme_address_space_desc;
} vmeUniverseAddrSpace;

typedef struct {
    vme_dwidth_t          vme_dwidth;
    char                  *vme_dwidth_s;
    char                  *vme_dwidth_desc;
} vmeUniverseDataWidth;


typedef struct {
    ELLNODE              node;
    vme_window_t         vme_window;
    vme_master_handle_t  vme_master_handle;
    vme_slave_handle_t   vme_slave_handle;
    vme_addr_mod_t       vme_addr_mod;
    vme_address_space_t     vme_addr_space;
    unsigned long        vmeAddr;
    unsigned long        windowSize;
    void                 *mapped_pointer;
    void                 *phys_addr;

    int                  endianConversion;
} drvvmeUniverseConfig;



#ifdef __cplusplus
extern "C" {
#endif

epicsShareFunc unsigned long long int epicsShareAPI vmeUniverse_asmReadCPUClock(void);

epicsShareFunc long epicsShareAPI drvvmeUniverse_makeISR(unsigned vector, 
                                                         void (*pFunction)(void*), 
                                                         void *param,
                                                         char *pFileName, 
                                                         int line, 
                                                         char *pFuncName);                                                                                                                                                
epicsShareFunc long epicsShareAPI vmeUniverse_sysBusToLocalAddrs(int            vme_addr_mod_space,
                                                                 vme_window_t   vme_window,
                                                                 unsigned long offset,
                                                                 void **localAddrs);

epicsShareFunc long epicsShareAPI vmeUniverse_generateVMEInterrupt(int level, 
                                                                   int vector);

#ifdef __cplusplus
}
#endif

#define vmeUniverse_intConnect(VECTOR, FUNCTION, PARAM) \
        drvvmeUniverse_makeISR((VECTOR), (FUNCTION), (PARAM), __FILE__, __LINE__, "##FUNCTION##")

#endif  /* drvvmeUniverse_H */
