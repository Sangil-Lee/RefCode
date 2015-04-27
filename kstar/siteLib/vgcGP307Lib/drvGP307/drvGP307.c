/*
**    ==============================================================================
**
**        Abs: driver support for GP307 vacuum gauge
**
**        Name: drvGP307.c
**
**
**        First Auth:  28-Jun-2004, Kukhee Kim (KHKIM)
**        Second Auth: dd-mmm-yyyy, First  Lastname (USERNAME)
**        Rev:         dd-mmm-yyyy, Reviewer's name (USERNAME)
**
**    -------------------------------------------------------------------------------
**        Mod:
**
**    ===============================================================================
*/


#define _DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <epicsMutex.h>
#include <epicsThread.h>
#include <ellLib.h>
#include <callback.h>
#include <iocsh.h>
#include <taskwd.h>

#include <dbCommon.h>
#include <drvSup.h>
#include <devSup.h>
#include <devLib.h>
#include <dbScan.h>

#include <epicsExport.h>

#include "drvGP307.h"


LOCAL ELLLIST *pdrvGP307_ellList = NULL;



drvGP307Config* drvGP307_find(char *portName);
LOCAL long drvGP307_init(char *portName, double devTimeout, double dbTimeout, double scanInterval);
LOCAL void drvGP307_scanTask(int param);
LOCAL void drvGP307_queueCallback(asynUser *pasynGP307User);
LOCAL void drvGP307_timeoutCallback(asynUser *pasynGP307User);
LOCAL void drvGP307_postCallback(CALLBACK *pPostCallback);
LOCAL void drvGP307_readALL(drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf);
LOCAL void drvGP307_postReadALL(drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf);
LOCAL void drvGP307_genericCmd(drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf);
LOCAL void drvGP307_reportPrint(drvGP307Config *pdrvGP307Config, int level);


LOCAL long drvGP307_io_report(int level);
LOCAL long drvGP307_init_driver(void);

struct {
    long        number;
    DRVSUPFUN   report;
    DRVSUPFUN   init;
} drvGP307 = {
    2,
    drvGP307_io_report,
    drvGP307_init_driver
};

epicsExportAddress(drvet, drvGP307);


static const iocshArg drvGP307_initArg0 = {"port", iocshArgString};
static const iocshArg drvGP307_initArg1 = {"dev timeout", iocshArgDouble};
static const iocshArg drvGP307_initArg2 = {"callback timeout", iocshArgDouble};
static const iocshArg drvGP307_initArg3 = {"scan interval", iocshArgDouble};
static const iocshArg* const drvGP307_initArgs[4] = { &drvGP307_initArg0,
                                                      &drvGP307_initArg1,
                                                      &drvGP307_initArg2,
                                                      &drvGP307_initArg3 };
static const iocshFuncDef drvGP307_initFuncDef = {"drvGP307_init",
                                                  4,
                                                  drvGP307_initArgs};
static void drvGP307_initCallFunc(const iocshArgBuf *args)
{
    char port_name[40];
    double devTimeout, dbTimeout, scanInterval;

    strcpy(port_name, args[0].sval);
    devTimeout = args[1].dval;
    dbTimeout = args[2].dval;
    scanInterval = args[3].dval;

    drvGP307_init(port_name, devTimeout, dbTimeout, scanInterval);
}

static void epicsShareAPI drvGP307Registrar(void)
{
    static int firstTime = 1;

    if(firstTime) {
        firstTime = 0;
    
        iocshRegister(&drvGP307_initFuncDef, drvGP307_initCallFunc);
    }
}

epicsExportRegistrar(drvGP307Registrar);




drvGP307Config* drvGP307_find(char *portName)
{
    drvGP307Config *pdrvGP307Config = NULL;
    char port_name[40];

    strcpy(port_name, portName);

    if(pdrvGP307_ellList) pdrvGP307Config = (drvGP307Config*) ellFirst(pdrvGP307_ellList);
    if(!pdrvGP307Config) return pdrvGP307Config;

    do {
        if(!strcmp(pdrvGP307Config->port_name, port_name)) break;
        pdrvGP307Config = (drvGP307Config*) ellNext(&pdrvGP307Config->node);
    } while(pdrvGP307Config);

    return pdrvGP307Config;

}

LOCAL long drvGP307_init(char *portName, double devTimeout, double cbTimeout, double scanInterval)
{
    static int firstTime = 1;
    drvGP307Config *pdrvGP307Config = NULL;
    GP307_read     *pGP307_read = NULL;
    CALLBACK       *pPostCallback = NULL;
    asynUser       *pasynGP307User = NULL;
    asynGP307UserData *pasynGP307UserData = NULL;
    asynInterface     *pasynGP307Interface = NULL;
    asynStatus        asynGP307Status;

    char port_name[40];

    if(firstTime) {
        firstTime = 0;
        epicsThreadCreate("drvGP307ScanTask",
                          epicsThreadPriorityLow,
                          epicsThreadGetStackSize(epicsThreadStackSmall),
                          (EPICSTHREADFUNC) drvGP307_scanTask,
                          0);
    }

    strcpy(port_name, portName);

    if(!pdrvGP307_ellList) {
        pdrvGP307_ellList = (ELLLIST *)malloc(sizeof(ELLLIST));
        ellInit(pdrvGP307_ellList)
    }
    if(drvGP307_find(port_name))  return 0;    /* the driver was initialized already */

    pdrvGP307Config = (drvGP307Config *)malloc(sizeof(drvGP307Config));
    pGP307_read     = (GP307_read*)malloc(sizeof(GP307_read));
    pPostCallback   = (CALLBACK*)malloc(sizeof(CALLBACK));
    pasynGP307UserData = (asynGP307UserData*)malloc(sizeof(asynGP307UserData));
    if(!pdrvGP307Config || !pGP307_read || !pPostCallback || !pasynGP307UserData) return 0;

    pdrvGP307Config->devTimeout = devTimeout;
    pdrvGP307Config->cbTimeout  = cbTimeout;
    pdrvGP307Config->scanInterval = scanInterval;

    strcpy(pdrvGP307Config->port_name, port_name);
    pdrvGP307Config->cbCount = 0;
    pdrvGP307Config->timeoutCount = 0;
    pdrvGP307Config->reconnCount = 0;

    pdrvGP307Config->asynScanLostCount = 0;
    pdrvGP307Config->asynRequestLostCount = 0;

    /* set internal data */
    pGP307_read->status =0;
    pGP307_read->status |= GP307STATUS_TIMEOUT_MASK;
    pGP307_read->cg0_prs = 0.;
    pGP307_read->cg1_prs = 0.;
    pGP307_read->ig_prs = 0.;
    

    /* set communication */
    pasynGP307UserData->pdrvGP307Config = (void *) pdrvGP307Config;
    pasynGP307User = pasynManager->createAsynUser(drvGP307_queueCallback, drvGP307_timeoutCallback);
    pasynGP307User->timeout = pdrvGP307Config->devTimeout;
    pasynGP307User->userPvt = pasynGP307UserData;
    asynGP307Status = pasynManager->connectDevice(pasynGP307User, port_name, 0);
    if(asynGP307Status != asynSuccess) {
        goto end;
    }
    pasynGP307Interface = pasynManager->findInterface(pasynGP307User, asynOctetType, 1);
    if(!pasynGP307Interface) {
        goto end;
    }
    pasynGP307UserData->pasynGP307Octet = (asynOctet*)pasynGP307Interface->pinterface;
    pasynGP307UserData->pdrvPvt = pasynGP307Interface->drvPvt;
    pdrvGP307Config->lock = epicsMutexCreate();
    scanIoInit(&pdrvGP307Config->ioScanPvt);

    callbackSetCallback(drvGP307_postCallback, pPostCallback);
    callbackSetPriority(priorityLow, pPostCallback);
    callbackSetUser((void*)pdrvGP307Config, pPostCallback);

    pdrvGP307Config->queueCallbackRingBytesId 
        = epicsRingBytesCreate(sizeof(drvGP307CallbackQueueBuf)*GP307_CALLBACK_QUEUE_SIZE);
    pdrvGP307Config->postCallbackRingBytesId
        = epicsRingBytesCreate(sizeof(drvGP307CallbackQueueBuf)*GP307_CALLBACK_QUEUE_SIZE);


    pdrvGP307Config->pPostCallback = pPostCallback;
    pdrvGP307Config->pGP307_read = pGP307_read;
    pdrvGP307Config->pasynGP307User = pasynGP307User;
    pdrvGP307Config->pasynGP307UserData = pasynGP307UserData;

    ellAdd(pdrvGP307_ellList, &pdrvGP307Config->node);


    return 0;

    end:
          /* release allocated memory and asynInterface */
    return 0;
        
}


LOCAL void drvGP307_scanTask(int param)
{
    drvGP307Config *pdrvGP307Config = NULL;
    GP307_read *pGP307_read = NULL;
    double drvGP307_scanInterval;
    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;
    asynStatus vasynStatus;

    while(!pdrvGP307_ellList || ellCount(pdrvGP307_ellList) < 1) {
        epicsThreadSleep(1.);
    }

    while(1) {
        pdrvGP307Config = (drvGP307Config*) ellFirst(pdrvGP307_ellList);
        drvGP307_scanInterval = pdrvGP307Config->scanInterval;

        do {
            if(drvGP307_scanInterval > pdrvGP307Config->scanInterval)
                drvGP307_scanInterval = pdrvGP307Config->scanInterval;

            if(pdrvGP307Config->cbCount > 0) {
                epicsMutexLock(pdrvGP307Config->lock);
                pdrvGP307Config->asynScanLostCount++;
                epicsMutexUnlock(pdrvGP307Config->lock);
                pdrvGP307Config = (drvGP307Config*) ellNext(&pdrvGP307Config->node);
                continue;
            }


            epicsMutexLock(pdrvGP307Config->lock);
            pdrvGP307Config->cbCount++;
            epicsMutexUnlock(pdrvGP307Config->lock);

            pGP307_read = pdrvGP307Config->pGP307_read;
            pBufGP307_read->status = pGP307_read->status;
            pdrvGP307CallbackQueueBuf->userPvt =(void*) pdrvGP307Config;
            pdrvGP307CallbackQueueBuf->command = GP307CMD_READALL; 
            pdrvGP307CallbackQueueBuf->precord = NULL;
            nbytes = epicsRingBytesPut(pdrvGP307Config->queueCallbackRingBytesId, 
                                       (char*)pdrvGP307CallbackQueueBuf, 
                                       sizeof(drvGP307CallbackQueueBuf));
            vasynStatus 
                = pasynManager->queueRequest(pdrvGP307Config->pasynGP307User, asynQueuePriorityLow, pdrvGP307Config->cbTimeout);
            if(vasynStatus == asynError) {
                epicsMutexLock(pdrvGP307Config->lock);
                pdrvGP307Config->asynRequestLostCount++;
                epicsMutexUnlock(pdrvGP307Config->lock);
            }
            pdrvGP307Config = (drvGP307Config*) ellNext(&pdrvGP307Config->node);
        } while(pdrvGP307Config);

        epicsThreadSleep(drvGP307_scanInterval);
    }
}

LOCAL void drvGP307_queueCallback(asynUser *pasynGP307User)
{
    asynGP307UserData *pasynGP307UserData = (asynGP307UserData*) pasynGP307User->userPvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*) pasynGP307UserData->pdrvGP307Config;
    GP307_read *pGP307_read  = (GP307_read*) pdrvGP307Config->pGP307_read;
    CALLBACK  *pPostCallback = (CALLBACK*) pdrvGP307Config->pPostCallback;
    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;

    while(!epicsRingBytesIsEmpty(pdrvGP307Config->queueCallbackRingBytesId)) {

        epicsMutexLock(pdrvGP307Config->lock);
        pdrvGP307Config->cbCount--;
        epicsMutexUnlock(pdrvGP307Config->lock);

        nbytes = epicsRingBytesGet(pdrvGP307Config->queueCallbackRingBytesId, 
                                   (char*) pdrvGP307CallbackQueueBuf,
                                   sizeof(drvGP307CallbackQueueBuf));

        if(pBufGP307_read->status & GP307STATUS_TIMEOUT_MASK) {
            pBufGP307_read->status &= ~GP307STATUS_TIMEOUT_MASK;
            epicsMutexLock(pdrvGP307Config->lock);
            pdrvGP307Config->reconnCount++;
            epicsMutexUnlock(pdrvGP307Config->lock);
        }

        switch(pdrvGP307CallbackQueueBuf->command){
            case GP307CMD_READALL:
                pBufGP307_read->status |= GP307STATUS_TIMEOUT_MASK;
                drvGP307_readALL(pdrvGP307CallbackQueueBuf);
                pBufGP307_read->status &= ~GP307STATUS_TIMEOUT_MASK;
                pdrvGP307CallbackQueueBuf->userPvt = (void*) pdrvGP307Config;
                pdrvGP307CallbackQueueBuf->precord = NULL;
                break;
            case GP307CMD_ONDG:
            case GP307CMD_OFFDG:
            case GP307CMD_DGS:
            case GP307CMD_READPCS:
            case GP307CMD_ONIG0:
            case GP307CMD_OFFIG0:
            case GP307CMD_ONIG1:
            case GP307CMD_OFFIG1:
                pBufGP307_read->status |= GP307STATUS_TIMEOUT_MASK;
                drvGP307_genericCmd(pdrvGP307CallbackQueueBuf);
                pBufGP307_read->status &= ~GP307STATUS_TIMEOUT_MASK;
                pdrvGP307CallbackQueueBuf->userPvt = (void*) pdrvGP307Config;
                break;
            default:
                return;
        }

        nbytes = epicsRingBytesPut(pdrvGP307Config->postCallbackRingBytesId,
                                   (char*) pdrvGP307CallbackQueueBuf,
                                   sizeof(drvGP307CallbackQueueBuf));
        callbackRequest(pPostCallback);
      } /* end of while statement */

}


LOCAL void drvGP307_timeoutCallback(asynUser *pasynGP307User)
{
    asynGP307UserData *pasynGP307UserData = (asynGP307UserData*) pasynGP307User->userPvt;
    drvGP307Config *pdrvGP307Config = (drvGP307Config*) pasynGP307UserData->pdrvGP307Config;
    GP307_read *pGP307_read = (GP307_read*) pdrvGP307Config->pGP307_read;
    CALLBACK *pPostCallback = (CALLBACK*) pdrvGP307Config->pPostCallback;

    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;

    nbytes = epicsRingBytesGet(pdrvGP307Config->queueCallbackRingBytesId,
                               (char*) pdrvGP307CallbackQueueBuf,
                               sizeof(drvGP307CallbackQueueBuf));

    epicsMutexLock(pdrvGP307Config->lock);
    pdrvGP307Config->cbCount = 0;
    pdrvGP307Config->timeoutCount++; 
    epicsMutexUnlock(pdrvGP307Config->lock);
    
    pBufGP307_read->status |= GP307STATUS_TIMEOUT_MASK;
    pBufGP307_read->status &= ~(GP307STATUS_CG0VALID_MASK |
                                GP307STATUS_CG1VALID_MASK |
                                GP307STATUS_IGVALID_MASK); 

    pdrvGP307CallbackQueueBuf->userPvt = (void*) pdrvGP307Config;
    pdrvGP307CallbackQueueBuf->command = GP307CMD_TIMEOUT;
    nbytes = epicsRingBytesPut(pdrvGP307Config->postCallbackRingBytesId,
                               (char*) pdrvGP307CallbackQueueBuf,
                               sizeof(drvGP307CallbackQueueBuf));
    callbackRequest(pPostCallback);

}


LOCAL void drvGP307_postCallback(CALLBACK *pPostCallback)
{
    drvGP307Config *pdrvGP307Config = NULL;
    GP307_read *pGP307_read = NULL; 
    drvGP307CallbackQueueBuf vdrvGP307CallbackQueueBuf;
    drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf = &vdrvGP307CallbackQueueBuf;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;
    int nbytes;
    dbCommon        *precord;
    struct rset     *prset;


    callbackGetUser(pdrvGP307Config, pPostCallback);
    pGP307_read = (GP307_read*) pdrvGP307Config->pGP307_read;

    nbytes = epicsRingBytesGet(pdrvGP307Config->postCallbackRingBytesId,
                               (char*) pdrvGP307CallbackQueueBuf,
                               sizeof(drvGP307CallbackQueueBuf));

    switch(pBufGP307_read->command_from_postCallback = pdrvGP307CallbackQueueBuf->command) {
        case GP307CMD_TIMEOUT:
            epicsMutexLock(pdrvGP307Config->lock);
            pGP307_read->status = pBufGP307_read->status; 
            epicsMutexUnlock(pdrvGP307Config->lock);
            scanIoRequest(pdrvGP307Config->ioScanPvt);
            break;
        case GP307CMD_READALL:
            drvGP307_postReadALL(pdrvGP307CallbackQueueBuf);
            scanIoRequest(pdrvGP307Config->ioScanPvt);
            break;
        case GP307CMD_ONDG:
        case GP307CMD_OFFDG:
        case GP307CMD_DGS:
        case GP307CMD_READPCS:
        case GP307CMD_ONIG0:
        case GP307CMD_OFFIG0:
        case GP307CMD_ONIG1:
        case GP307CMD_OFFIG1:
            if(!(precord = (dbCommon*)(pdrvGP307CallbackQueueBuf->precord)) || !(prset = (struct rset*)(precord->rset))) break;
            epicsMutexLock(pdrvGP307Config->lock);
                pGP307_read->status = pBufGP307_read->status;
                pGP307_read->command_from_postCallback = pBufGP307_read->command_from_postCallback;
            epicsMutexUnlock(pdrvGP307Config->lock);
            dbScanLock(precord);
                (*prset->process)(precord);
            dbScanUnlock(precord);
            break;
        default:
            return;
    }

    return;
}


LOCAL void drvGP307_readALL(drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf)
{
    drvGP307Config *pdrvGP307Config = (drvGP307Config*) pdrvGP307CallbackQueueBuf->userPvt;
    asynUser *pasynGP307User = (asynUser*) pdrvGP307Config->pasynGP307User;
    asynGP307UserData *pasynGP307UserData = (asynGP307UserData*) pdrvGP307Config->pasynGP307UserData;
    asynOctet *pasynGP307Octet = (asynOctet*) pasynGP307UserData->pasynGP307Octet;
    void *pdrvPvt = (void*) pasynGP307UserData->pdrvPvt;

    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;

    char *msgBuf = pasynGP307UserData->msgBuf;
    char *dumBuf = pasynGP307UserData->dumBuf;
    

    pasynGP307Octet->setEos(pdrvPvt, pasynGP307User, EOS_STR, strlen(EOS_STR)); 

    pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_DSCG0 EOS_STR, strlen(GP307CMDSTR_DSCG0 EOS_STR));
    pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
    sscanf(msgBuf, "%le", &pBufGP307_read->cg0_prs);

    pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_DSCG1 EOS_STR, strlen(GP307CMDSTR_DSCG1 EOS_STR));
    pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
    sscanf(msgBuf, "%le", &pBufGP307_read->cg1_prs);

    pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_DSIG EOS_STR, strlen(GP307CMDSTR_DSIG EOS_STR));
    pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
    sscanf(msgBuf, "%le", &pBufGP307_read->ig_prs);


    return;
}




LOCAL void drvGP307_postReadALL(drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf)
{
    drvGP307Config *pdrvGP307Config = (drvGP307Config*) pdrvGP307CallbackQueueBuf->userPvt;
    asynUser *pasynGP307User = (asynUser*) pdrvGP307Config->pasynGP307User;
    asynGP307UserData *pasynGP307UserData = (asynGP307UserData*) pdrvGP307Config->pasynGP307UserData;
    asynOctet *pasynGP307Octet = (asynOctet*) pasynGP307UserData->pasynGP307Octet;
    void *pdrvPvt = (void*) pasynGP307UserData->pdrvPvt;
    GP307_read *pGP307_read = pdrvGP307Config->pGP307_read;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;

    if(pBufGP307_read->ig_prs > 1.E+4) pBufGP307_read->status &= ~(GP307STATUS_IG0ONOFF_MASK | GP307STATUS_IG1ONOFF_MASK);
    if(pBufGP307_read->cg0_prs > 0. && pBufGP307_read->cg0_prs < 1.E+4) pBufGP307_read->status |= GP307STATUS_CG0VALID_MASK;
    else                                                                pBufGP307_read->status &= ~GP307STATUS_CG0VALID_MASK;
    if(pBufGP307_read->cg1_prs > 0. && pBufGP307_read->cg1_prs < 1.E+4) pBufGP307_read->status |= GP307STATUS_CG1VALID_MASK;
    else                                                                pBufGP307_read->status &= ~GP307STATUS_CG1VALID_MASK;
    if(pBufGP307_read->ig_prs > 0. && pBufGP307_read->ig_prs < 1.E+4) pBufGP307_read->status |= GP307STATUS_IGVALID_MASK;
    else                                                              pBufGP307_read->status &= ~GP307STATUS_IGVALID_MASK;

    epicsMutexLock(pdrvGP307Config->lock);
    pGP307_read->status = pBufGP307_read->status;
    pGP307_read->cg0_prs = pBufGP307_read->cg0_prs;
    pGP307_read->cg1_prs = pBufGP307_read->cg1_prs;
    pGP307_read->ig_prs = pBufGP307_read->ig_prs;
    epicsMutexUnlock(pdrvGP307Config->lock); 


    return;
}


LOCAL void drvGP307_genericCmd(drvGP307CallbackQueueBuf *pdrvGP307CallbackQueueBuf)
{
    drvGP307Config *pdrvGP307Config = (drvGP307Config*) pdrvGP307CallbackQueueBuf->userPvt;
    asynUser *pasynGP307User = (asynUser*) pdrvGP307Config->pasynGP307User;
    asynGP307UserData *pasynGP307UserData = (asynGP307UserData*) pdrvGP307Config->pasynGP307UserData;
    asynOctet *pasynGP307Octet = (asynOctet*) pasynGP307UserData->pasynGP307Octet;
    void *pdrvPvt = (void*) pasynGP307UserData->pdrvPvt;
    GP307_read *pGP307_read = pdrvGP307Config->pGP307_read;
    GP307_read *pBufGP307_read = &pdrvGP307CallbackQueueBuf->vGP307_read;

    char *msgBuf = pasynGP307UserData->msgBuf;
    int pcs[6], dgs, index;

    switch(pdrvGP307CallbackQueueBuf->command) {
        case GP307CMD_ONDG:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_DGON EOS_STR, strlen(GP307CMDSTR_DGON EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            pBufGP307_read->status |= GP307STATUS_DGONOFF_MASK;
            break; 
        case GP307CMD_OFFDG:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_DGOFF EOS_STR, strlen(GP307CMDSTR_DGOFF EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            pBufGP307_read->status &= ~GP307STATUS_DGONOFF_MASK;
            break;
        case GP307CMD_DGS:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_DGS EOS_STR, strlen(GP307CMDSTR_DGS EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            sscanf(msgBuf, "%d", &dgs);
            if(dgs) pBufGP307_read->status |= GP307STATUS_DGONOFF_MASK;
            else    pBufGP307_read->status &= ~GP307STATUS_DGONOFF_MASK;
            break;
        case GP307CMD_READPCS:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_PCS EOS_STR, strlen(GP307CMDSTR_PCS EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            sscanf(msgBuf, "%d %d %d %d %d %d", &pcs[5], &pcs[4], &pcs[3], &pcs[2], &pcs[1], &pcs[0]);
            pBufGP307_read->pcs = 0x00;
            for(index=0;index<6;index++)  if(pcs[index]) pBufGP307_read->pcs |= (0x01<<index);
            break;
        case GP307CMD_ONIG0:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_IG0ON EOS_STR, strlen(GP307CMDSTR_IG0ON EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            pBufGP307_read->status |= GP307STATUS_IG0ONOFF_MASK;
            pBufGP307_read->status &= ~GP307STATUS_IG1ONOFF_MASK;
            break;
        case GP307CMD_OFFIG0:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_IG0OFF EOS_STR, strlen(GP307CMDSTR_IG0OFF EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            pBufGP307_read->status &= ~GP307STATUS_IG0ONOFF_MASK;
            break;
        case GP307CMD_ONIG1:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_IG1ON EOS_STR, strlen(GP307CMDSTR_IG1ON EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            pBufGP307_read->status |= GP307STATUS_IG1ONOFF_MASK;
            pBufGP307_read->status &= ~GP307STATUS_IG0ONOFF_MASK;
            break;
        case GP307CMD_OFFIG1:
            pasynGP307Octet->write(pdrvPvt, pasynGP307User, GP307CMDSTR_IG1OFF EOS_STR, strlen(GP307CMDSTR_IG1OFF EOS_STR));
            pasynGP307Octet->read(pdrvPvt, pasynGP307User, msgBuf, GP307_MSGBUFSIZE);
            pBufGP307_read->status &= ~GP307STATUS_IG1ONOFF_MASK;
            break;
        default:
    } 

    return;
}


LOCAL void drvGP307_reportPrint(drvGP307Config *pdrvGP307Config, int level)
{
    GP307_read *pGP307_read = (GP307_read*)pdrvGP307Config->pGP307_read;

    epicsMutexLock(pdrvGP307Config->lock);
    epicsPrintf("port: %s Granville-Phillips GP307\n",
                pdrvGP307Config->port_name);
    if(level>0)
    epicsPrintf("      status 0x%2x  cg0 %5.1le  cg1 %5.1le  ig %5.1le\n",
                pGP307_read->status, pGP307_read->cg0_prs, pGP307_read->cg1_prs, pGP307_read->ig_prs);
    if(level>1)
    epicsPrintf("      cbCount %d  timeoutCount %d  reconnCount %d\n",
                pdrvGP307Config->cbCount, pdrvGP307Config->timeoutCount, pdrvGP307Config->reconnCount);
    if(level>2)
    epicsPrintf("      queueCallback RingBuf: free bytes %d, free events %d\n"
                "      postCallback  RingBuf: free bytes %d, free events %d\n"
                "      asynScanLostCount %ld, asynRequestLostCount %ld\n",
                epicsRingBytesFreeBytes(pdrvGP307Config->queueCallbackRingBytesId),
                epicsRingBytesFreeBytes(pdrvGP307Config->queueCallbackRingBytesId)/sizeof(drvGP307CallbackQueueBuf),
                epicsRingBytesFreeBytes(pdrvGP307Config->postCallbackRingBytesId),
                epicsRingBytesFreeBytes(pdrvGP307Config->postCallbackRingBytesId)/sizeof(drvGP307CallbackQueueBuf),
                pdrvGP307Config->asynScanLostCount, pdrvGP307Config->asynRequestLostCount
                );
    epicsMutexUnlock(pdrvGP307Config->lock);
               
    return;
}


LOCAL long drvGP307_io_report(int level)
{
    drvGP307Config *pdrvGP307Config =NULL;

    if(!pdrvGP307_ellList || ellCount(pdrvGP307_ellList)<1) return 0; 
    epicsPrintf("Total %d module(s) found\n", ellCount(pdrvGP307_ellList));
 
    pdrvGP307Config = (drvGP307Config*) ellFirst(pdrvGP307_ellList); 
    do {
        drvGP307_reportPrint(pdrvGP307Config, level);
        pdrvGP307Config = (drvGP307Config*) ellNext(&pdrvGP307Config->node);
    } while(pdrvGP307Config);
 
    return 0;
}


LOCAL long drvGP307_init_driver(void)
{
    return 0;
}
