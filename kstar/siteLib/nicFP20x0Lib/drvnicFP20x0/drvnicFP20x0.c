



#define DEBUG_

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

#include "drvnicFP20x0.h"


typedef struct {
    cFP20x0_modType      modType;
    char                 *modStr;
    char                 numCh;
} modTypeStr;

modTypeStr tmodTypeStr[] = {
    {cFP20x0_modType_DI8, cFP20x0_STR_MODTYPE_DI8,       8},
    {cFP20x0_modType_DI16, cFP20x0_STR_MODTYPE_DI16,     16},
    {cFP20x0_modType_DI16, cFP20x0_STR_MODTYPE_DI16_SHORT, 16},
    {cFP20x0_modType_DI32, cFP20x0_STR_MODTYPE_DI32,     32},

    {cFP20x0_modType_DO8, cFP20x0_STR_MODTYPE_DO8,       8},
    {cFP20x0_modType_DO16, cFP20x0_STR_MODTYPE_DO16,     16},
    {cFP20x0_modType_DO16, cFP20x0_STR_MODTYPE_DO16_SHORT, 16},
    {cFP20x0_modType_DO32, cFP20x0_STR_MODTYPE_DO32,     32},

    {cFP20x0_modType_AI8, cFP20x0_STR_MODTYPE_AI8_SHORT, 8},
    {cFP20x0_modType_AI2, cFP20x0_STR_MODTYPE_AI2,       2},
    {cFP20x0_modType_AI8, cFP20x0_STR_MODTYPE_AI8,       8},
    {cFP20x0_modType_AI16, cFP20x0_STR_MODTYPE_AI16,     16},
    {cFP20x0_modType_AI32, cFP20x0_STR_MODTYPE_AI32,     32},

    {cFP20x0_modType_AO8, cFP20x0_STR_MODTYPE_AO8_SHORT, 8},
    {cFP20x0_modType_AO2, cFP20x0_STR_MODTYPE_AO2,       2},
    {cFP20x0_modType_AO8, cFP20x0_STR_MODTYPE_AO8,       8},
    {cFP20x0_modType_AO16, cFP20x0_STR_MODTYPE_AO16,     16},
    {cFP20x0_modType_AO32, cFP20x0_STR_MODTYPE_AO32,     32},
 
    {cFP20x0_modType_term, NULL, -1}
};

LOCAL ELLLIST *pdrvcFP20x0_ellList = NULL;
LOCAL ELLLIST *pdrvcFP20x0MsgBufNode_ellList = NULL;
LOCAL epicsMutexId  drvcFP20x0MsgBufLock;

drvcFP20x0Config *drvcFP20x0_find(char *portName);
cFP20x0Module    *drvcFP20x0_modFind(char *modLabel, drvcFP20x0Config *pdrvcFP20x0Config);
void drvcFP20x0_requestWriteDev(drvcFP20x0Config *pdrvcFP20x0Config, char *commandBuf, dbCommon *precord);

LOCAL int drvcFP20x0_timeGetCurrentDouble(double *pTime);
LOCAL modTypeStr* drvcFP20x0_modTypeFind(char *modLabel);
LOCAL long drvcFP20x0_init(char *portName, double devTimeout, double cbTimeout, double scanInterval, int monitorInterval); 
LOCAL void drvcFP20x0_scanTask(int param);
LOCAL void drvcFP20x0_queueCallback(asynUser *pasyncFP20x0User);
LOCAL void drvcFP20x0_timeoutCallback(asynUser *pasyncFP20x0User);
LOCAL void drvcFP20x0_postCallback(CALLBACK *pPostCallback);
LOCAL void drvcFP20x0_reportPrint(drvcFP20x0Config *pdrvcFP20x0Config, int level);
LOCAL void drvcFP20x0_reportPrintScanCountDI8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountDI16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountDI32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountDO8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountDO16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountDO32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAI2(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAI8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAI16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAI32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAO2(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAO8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAO16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintScanCountAO32(cFP20x0Module *pcFP20x0Moudle);
LOCAL void drvcFP20x0_reportPrintDataDI8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataDI16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataDI32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataDO8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataDO16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataDO32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAI2(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAI8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAI16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAI32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAO2(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAO8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAO16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_reportPrintDataAO32(cFP20x0Module *pcFP20x0Module);
LOCAL int drvcFP20x0_setDelay(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf);
LOCAL int drvcFP20x0_checkModule(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf);
LOCAL int drvcFP20x0_postCheckModule(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf);
LOCAL void drvcFP20x0_initModTypeDI8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeDI16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeDI32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeDO8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeDO16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeDO32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAI2(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAI8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAI16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAI32(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAO2(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAO8(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAO16(cFP20x0Module *pcFP20x0Module);
LOCAL void drvcFP20x0_initModTypeAO32(cFP20x0Module *pcFP20x0Module);
LOCAL int drvcFP20x0_takeTokenCheckModule(char *msgBuf, char *modStr[]);
LOCAL int drvcFP20x0_destructModule(drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL int drvcFP20x0_read(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf);
LOCAL int drvcFP20x0_write(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf);
LOCAL int drvcFP20x0_requestSetDelay(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL int drvcFP20x0_requestCheck(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL int drvcFP20x0_requestRead(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL int drvcFP20x0_requestWrite(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config,
                                  char *commandBuf, dbCommon *precord);
LOCAL int drvcFP20x0_postRead(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL int drvcFP20x0_postWrite(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL int drvcFP20x0_postSetDelay(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueuebuf, drvcFP20x0Config *pdrvcFP20x0Config);
LOCAL void drvcFP20x0_makeTimeStamp(drvcFP20x0Config *pdrvcFP20x0Config, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeDI8(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeDI16(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeDI32(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeDO8(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeDO16(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeDO32(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAI2(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAI8(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAI16(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAI32(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAO2(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAO8(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAO16(cFP20x0Module *pcFP20x0Module, char *msgBuf);
LOCAL void drvcFP20x0_readModTypeAO32(cFP20x0Module *pcFP20x0Module, char *msgBuf);



LOCAL long drvcFP20x0_io_report(int level);
LOCAL long drvcFP20x0_init_driver(void);

struct {
    long              number;
    DRVSUPFUN         report;
    DRVSUPFUN         init;
} drvcFP20x0 = {
    2,
    drvcFP20x0_io_report,
    drvcFP20x0_init_driver
};

epicsExportAddress(drvet, drvcFP20x0);


static const iocshArg drvcFP20x0_initArg0 = {"port", iocshArgString};
static const iocshArg drvcFP20x0_initArg1 = {"dev timeout", iocshArgDouble};
static const iocshArg drvcFP20x0_initArg2 = {"callback timeout", iocshArgDouble};
static const iocshArg drvcFP20x0_initArg3 = {"scan interval", iocshArgDouble};
static const iocshArg drvcFP20x0_initArg4 = {"monitor interval[msec]", iocshArgInt};
static const iocshArg* const drvcFP20x0_initArgs[] = { &drvcFP20x0_initArg0,
                                                       &drvcFP20x0_initArg1,
                                                       &drvcFP20x0_initArg2,
                                                       &drvcFP20x0_initArg3,
                                                       &drvcFP20x0_initArg4 };
static const iocshFuncDef drvcFP20x0_initFuncDef = {"drvcFP20x0_init",
                                                    5,
                                                    drvcFP20x0_initArgs};
static void drvcFP20x0_initCallFunc(const iocshArgBuf *args)
{
    char port_name[40];
    double devTimeout, cbTimeout, scanInterval;
    int    monitorInverval;

    strcpy(port_name, args[0].sval);
    devTimeout = args[1].dval;
    cbTimeout  = args[2].dval;
    scanInterval = args[3].dval;
    monitorInverval = args[4].ival;
    

   
    drvcFP20x0_init(port_name, devTimeout, cbTimeout, scanInterval, monitorInverval);
  
}

static void epicsShareAPI drvcFP20x0Registrar(void)
{
    static int firstTime = 1;

    if(firstTime) {
        firstTime = 0;
        iocshRegister(&drvcFP20x0_initFuncDef, drvcFP20x0_initCallFunc);
    }
}

epicsExportRegistrar(drvcFP20x0Registrar);




drvcFP20x0Config* drvcFP20x0_find(char *portName)
{
    drvcFP20x0Config *pdrvcFP20x0Config = NULL;
    char port_name[40];

    strcpy(port_name, portName);

    if(pdrvcFP20x0_ellList) pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
    if(!pdrvcFP20x0Config) return pdrvcFP20x0Config;

    do {
        if(!strcmp(pdrvcFP20x0Config->port_name, port_name)) break;
        pdrvcFP20x0Config = (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
    } while(pdrvcFP20x0Config);

    return pdrvcFP20x0Config;
}


cFP20x0Module* drvcFP20x0_modFind(char *modLabel, drvcFP20x0Config *pdrvcFP20x0Config)
{
    ELLLIST *pcFP20x0Module_ellList = pdrvcFP20x0Config->pcFP20x0Module_ellList;
    cFP20x0Module *pcFP20x0Module = NULL;
    char mod_label[40];

    strcpy(mod_label, modLabel);

    if(pcFP20x0Module_ellList) pcFP20x0Module = (cFP20x0Module*) ellFirst(pcFP20x0Module_ellList);
    if(!pcFP20x0Module) return pcFP20x0Module;

    do {
        if(!strcmp(pcFP20x0Module->modLabel, mod_label)) break;
        pcFP20x0Module = (cFP20x0Module*) ellNext(&pcFP20x0Module->node);
    } while(pcFP20x0Module);

    return pcFP20x0Module;
}


void drvcFP20x0_requestWriteDev(drvcFP20x0Config *pdrvcFP20x0Config, char *commandBuf, dbCommon *precord)
{
    drvcFP20x0CallbackQueueBuf vdrvcFP20x0CallbackQueueBuf;
    drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf = &vdrvcFP20x0CallbackQueueBuf;
    int tryCnt;

    tryCnt = drvcFP20x0_requestWrite(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config, commandBuf, precord);
    
}


LOCAL int drvcFP20x0_timeGetCurrentDouble(double *pTime)
{
    epicsTimeStamp stamp;

    *pTime = 0.0;
    if(epicsTimeGetCurrent(&stamp) == epicsTimeERROR)  return epicsTimeERROR;

    *pTime = (double) stamp.secPastEpoch + ((double)stamp.nsec/1.E+9);
  
    return epicsTimeOK;

}


LOCAL modTypeStr* drvcFP20x0_modTypeFind(char *modLabel)
{
    char  mod_label[40];
    int   index = 0;

    strcpy(mod_label, modLabel);

    do {
        if(!strcmp((tmodTypeStr+index)->modStr, mod_label)) break;
        index++;
    } while((tmodTypeStr+index)->modType != cFP20x0_modType_term);

    return tmodTypeStr+index;
}


LOCAL long drvcFP20x0_init(char *portName, double devTimeout, double cbTimeout, double scanInterval, int monitorInterval)
{
    drvcFP20x0Config   *pdrvcFP20x0Config = NULL;
    ELLLIST            *pcFP20x0Module_ellList = NULL;
    asynUser            *pasyncFP20x0User = NULL;
    asyncFP20x0UserData *pasyncFP20x0UserData = NULL;
    asynInterface       *pasyncFP20x0Interface = NULL;
    asynStatus          asyncFP20x0Status;
    CALLBACK            *pPostCallback = NULL;

    char port_name[40];


    strcpy(port_name, portName);
    if(!pdrvcFP20x0_ellList) {  /* init ellList for pdrvcFP20x0Config */
        pdrvcFP20x0_ellList = (ELLLIST*)malloc(sizeof(ELLLIST));
        ellInit(pdrvcFP20x0_ellList);
    } 
    if(!pdrvcFP20x0MsgBufNode_ellList) {
        pdrvcFP20x0MsgBufNode_ellList = (ELLLIST*)malloc(sizeof(ELLLIST));
        ellInit(pdrvcFP20x0MsgBufNode_ellList);
    }
    
    if(drvcFP20x0_find(port_name)) return 0;  /* the driver was initialized already */

    pdrvcFP20x0Config = (drvcFP20x0Config*)malloc(sizeof(drvcFP20x0Config));
    pcFP20x0Module_ellList = (ELLLIST*)malloc(sizeof(ELLLIST));
    pasyncFP20x0UserData   = (asyncFP20x0UserData*)malloc(sizeof(asyncFP20x0UserData));
    pPostCallback          = (CALLBACK*)malloc(sizeof(CALLBACK));
    if(!pdrvcFP20x0Config || !pcFP20x0Module_ellList || !pasyncFP20x0UserData || !pPostCallback) return 0;

    strcpy(pdrvcFP20x0Config->port_name, port_name);
    pdrvcFP20x0Config->devTimeout = devTimeout;
    pdrvcFP20x0Config->cbTimeout  = cbTimeout;
    pdrvcFP20x0Config->scanInterval = scanInterval; 
    pdrvcFP20x0Config->monitorInterval = monitorInterval;
    ellInit(pcFP20x0Module_ellList);
    pdrvcFP20x0Config->pcFP20x0Module_ellList = pcFP20x0Module_ellList;
    pdrvcFP20x0Config->flg_moduleConstruction = 0x00;
    pdrvcFP20x0Config->flg_first = 0xff;
    pdrvcFP20x0Config->cbCount = 0;
    pdrvcFP20x0Config->timeoutCount = 0;
    pdrvcFP20x0Config->reconnCount = 0;
    pdrvcFP20x0Config->postReadFailCount = 0;
    pdrvcFP20x0Config->asynScanLostCount = 0;
    pdrvcFP20x0Config->asynRequestLostCount = 0;

    pdrvcFP20x0Config->scanCnt = 0;
    pdrvcFP20x0Config->scanRate = 0.;

    drvcFP20x0_timeGetCurrentDouble(&pdrvcFP20x0Config->scanTime);
    epicsTimeGetCurrent(&pdrvcFP20x0Config->stamp);

    pdrvcFP20x0Config->lock = epicsMutexCreate();

    scanIoInit(&pdrvcFP20x0Config->scanIoScanPvt);
    scanIoInit(&pdrvcFP20x0Config->stampIoScanPvt);

    /* set communication */
    pasyncFP20x0UserData->pdrvcFP20x0Config = (void*)pdrvcFP20x0Config;
    pasyncFP20x0User = pasynManager->createAsynUser(drvcFP20x0_queueCallback, drvcFP20x0_timeoutCallback);
    pasyncFP20x0User->timeout = pdrvcFP20x0Config->devTimeout;
    pasyncFP20x0User->userPvt = pasyncFP20x0UserData;
    asyncFP20x0Status = pasynManager->connectDevice(pasyncFP20x0User, port_name, 0);
    if(asyncFP20x0Status != asynSuccess) goto end;
    pasyncFP20x0Interface = pasynManager->findInterface(pasyncFP20x0User, asynOctetType, 1);
    if(!pasyncFP20x0Interface) goto end;
    pasyncFP20x0UserData->pasyncFP20x0Octet = (asynOctet*)pasyncFP20x0Interface->pinterface;
    pasyncFP20x0UserData->pdrvPvt = pasyncFP20x0Interface->drvPvt;


    callbackSetCallback(drvcFP20x0_postCallback, pPostCallback);
    callbackSetPriority(priorityLow, pPostCallback);
    callbackSetUser((void*)pdrvcFP20x0Config, pPostCallback);


    pdrvcFP20x0Config->pPostCallback = pPostCallback;
    pdrvcFP20x0Config->queueCallbackRingBytesId
        = epicsRingBytesCreate(sizeof(drvcFP20x0CallbackQueueBuf)*cFP20x0_CALLBACK_QUEUE_SIZE);
    pdrvcFP20x0Config->postCallbackRingBytesId
        = epicsRingBytesCreate(sizeof(drvcFP20x0CallbackQueueBuf)*cFP20x0_CALLBACK_QUEUE_SIZE);


    pdrvcFP20x0Config->pasyncFP20x0User = pasyncFP20x0User;
    pdrvcFP20x0Config->pasyncFP20x0UserData = pasyncFP20x0UserData;
    ellAdd(pdrvcFP20x0_ellList, &pdrvcFP20x0Config->node);


    return 0;

    end:  /* de-allocate resource of asyn */
   
    return 0;
}



LOCAL void drvcFP20x0_scanTask(int param)
{
    drvcFP20x0Config *pdrvcFP20x0Config = NULL;
    double  drvcFP20x0_scanInterval;
    drvcFP20x0CallbackQueueBuf vdrvcFP20x0CallbackQueueBuf;
    drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf = &vdrvcFP20x0CallbackQueueBuf;
    int nbytes;
    int tryCnt;
    double timeNow;
    asynStatus vasynStatus;

    while(!pdrvcFP20x0_ellList || ellCount(pdrvcFP20x0_ellList)<1) epicsThreadSleep(1.);

    while(1) {
        pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
        drvcFP20x0_scanInterval = pdrvcFP20x0Config->scanInterval;

        do {
            if(drvcFP20x0_scanInterval > pdrvcFP20x0Config->scanInterval)
                            drvcFP20x0_scanInterval = pdrvcFP20x0Config->scanInterval;

            drvcFP20x0_timeGetCurrentDouble(&timeNow);
       
            epicsMutexLock(pdrvcFP20x0Config->lock);
            pdrvcFP20x0Config->scanRate = (double)pdrvcFP20x0Config->scanCnt / (timeNow - pdrvcFP20x0Config->scanTime);
            pdrvcFP20x0Config->scanTime = timeNow;
            pdrvcFP20x0Config->scanCnt = 0;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);

            scanIoRequest(pdrvcFP20x0Config->scanIoScanPvt);

            pdrvcFP20x0Config = (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
        } while(pdrvcFP20x0Config);
        epicsThreadSleep(drvcFP20x0_scanInterval);
  
        /* drvcFP20x0_io_report(3);     KHKIM DEBUG */
    }
}



LOCAL void drvcFP20x0_queueCallback(asynUser *pasyncFP20x0User)
{
    asyncFP20x0UserData *pasyncFP20x0UserData = (asyncFP20x0UserData*) pasyncFP20x0User->userPvt;
    drvcFP20x0Config    *pdrvcFP20x0Config    = (drvcFP20x0Config*) pasyncFP20x0UserData->pdrvcFP20x0Config;
    CALLBACK            *pPostCallback        = (CALLBACK*) pdrvcFP20x0Config->pPostCallback;
    drvcFP20x0CallbackQueueBuf  vdrvcFP20x0CallbackQueueBuf;
    drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf = &vdrvcFP20x0CallbackQueueBuf;
    int nbytes;
    int tryCnt;

    while(!epicsRingBytesIsEmpty(pdrvcFP20x0Config->queueCallbackRingBytesId)) {
        epicsMutexLock(pdrvcFP20x0Config->lock);
        pdrvcFP20x0Config->cbCount--;
        epicsMutexUnlock(pdrvcFP20x0Config->lock);
        nbytes = epicsRingBytesGet(pdrvcFP20x0Config->queueCallbackRingBytesId,
                                   (char*) pdrvcFP20x0CallbackQueueBuf,
                                   sizeof(drvcFP20x0CallbackQueueBuf));
       
        switch(pdrvcFP20x0CallbackQueueBuf->command){ 
            case cFP20x0_CMD_CHECKMODULE:
                tryCnt = drvcFP20x0_checkModule(pdrvcFP20x0CallbackQueueBuf);
                #ifdef DEBUG
                epicsPrintf("drvcFP20x0:checkModule %d queries\n", tryCnt);
                #endif
                break;
            case cFP20x0_CMD_READ:
                tryCnt = drvcFP20x0_read(pdrvcFP20x0CallbackQueueBuf);
                break;
            case cFP20x0_CMD_SETDELAY:
                tryCnt = drvcFP20x0_setDelay(pdrvcFP20x0CallbackQueueBuf);
                break;
            case cFP20x0_CMD_WRITE:
                tryCnt = drvcFP20x0_write(pdrvcFP20x0CallbackQueueBuf);
                break;
            default: break;
        }

        pdrvcFP20x0CallbackQueueBuf->userPvt = (void*)pdrvcFP20x0Config;
        nbytes = epicsRingBytesPut(pdrvcFP20x0Config->postCallbackRingBytesId,
                                   (char*)pdrvcFP20x0CallbackQueueBuf,
                                   sizeof(drvcFP20x0CallbackQueueBuf));
        callbackRequest(pPostCallback);
    }
}


LOCAL void drvcFP20x0_timeoutCallback(asynUser *pasyncFP20x0User)
{
}


LOCAL void drvcFP20x0_postCallback(CALLBACK *pPostCallback)
{
    drvcFP20x0Config *pdrvcFP20x0Config = NULL;
    drvcFP20x0CallbackQueueBuf  vdrvcFP20x0CallbackQueueBuf;
    drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf = &vdrvcFP20x0CallbackQueueBuf;
    drvcFP20x0CallbackQueueBuf  vdrvcFP20x0CallbackQueueBufNew;
    drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBufNew = &vdrvcFP20x0CallbackQueueBufNew;
    int nbytes;
    int tryCnt;
    dbCommon *precord = NULL;
    struct rset  *preset = NULL;

    callbackGetUser(pdrvcFP20x0Config, pPostCallback);
    nbytes = epicsRingBytesGet(pdrvcFP20x0Config->postCallbackRingBytesId,
                               (char*) pdrvcFP20x0CallbackQueueBuf,
                               sizeof(drvcFP20x0CallbackQueueBuf));

    switch(pdrvcFP20x0CallbackQueueBuf->command_from_postCallback = pdrvcFP20x0CallbackQueueBuf->command){
        case cFP20x0_CMD_CHECKMODULE:
            tryCnt = drvcFP20x0_postCheckModule(pdrvcFP20x0CallbackQueueBuf); 
            epicsMutexLock(pdrvcFP20x0Config->lock);
                pdrvcFP20x0Config->scanCnt++;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);
            break;
        case cFP20x0_CMD_READ:
            tryCnt = drvcFP20x0_requestRead(pdrvcFP20x0CallbackQueueBufNew, pdrvcFP20x0Config);
            tryCnt = drvcFP20x0_postRead(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
            epicsMutexLock(pdrvcFP20x0Config->lock);
                pdrvcFP20x0Config->scanCnt++;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);
            break;
        case cFP20x0_CMD_SETDELAY:
            tryCnt = drvcFP20x0_postSetDelay(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
            epicsMutexLock(pdrvcFP20x0Config->lock);
                pdrvcFP20x0Config->scanCnt++;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);
            break;
        case cFP20x0_CMD_WRITE:
            tryCnt = drvcFP20x0_postWrite(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
            epicsMutexLock(pdrvcFP20x0Config->lock);
                pdrvcFP20x0Config->scanCnt++;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);
    }

}


LOCAL void drvcFP20x0_reportPrint(drvcFP20x0Config *pdrvcFP20x0Config, int level)
{
    cFP20x0Module *pcFP20x0Module = NULL;
    char buf[40];

    printf("port_name: %s  module: %d\n", 
           pdrvcFP20x0Config->port_name, ellCount(pdrvcFP20x0Config->pcFP20x0Module_ellList));
    if(ellCount(pdrvcFP20x0Config->pcFP20x0Module_ellList)<1) return;

    pcFP20x0Module = (cFP20x0Module*) ellFirst(pdrvcFP20x0Config->pcFP20x0Module_ellList);
    do {
        if(level>0) printf("          modLabel %s, modType %d, modStr %s, numCh %d\n",
                           pcFP20x0Module->modLabel, pcFP20x0Module->modType, pcFP20x0Module->modStr, pcFP20x0Module->numCh);

        switch(pcFP20x0Module->modType){
            case cFP20x0_modType_DI8:
                if(level>2) drvcFP20x0_reportPrintScanCountDI8(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataDI8(pcFP20x0Module);
                break;
            case cFP20x0_modType_DI16: 
                if(level>2) drvcFP20x0_reportPrintScanCountDI16(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataDI16(pcFP20x0Module);
                break;
            case cFP20x0_modType_DI32:
                if(level>2) drvcFP20x0_reportPrintScanCountDI32(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataDI32(pcFP20x0Module);
                break;
            case cFP20x0_modType_DO8:
                if(level>2) drvcFP20x0_reportPrintScanCountDO8(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataDO8(pcFP20x0Module);
                break;
            case cFP20x0_modType_DO16:
                if(level>2) drvcFP20x0_reportPrintScanCountDO16(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataDO16(pcFP20x0Module);
                break;
            case cFP20x0_modType_DO32:
                if(level>2) drvcFP20x0_reportPrintScanCountDO32(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataDO32(pcFP20x0Module);
                break;
            case cFP20x0_modType_AI2:
                if(level>2) drvcFP20x0_reportPrintScanCountAI2(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAI2(pcFP20x0Module);
                break;
            case cFP20x0_modType_AI8:
                if(level>2) drvcFP20x0_reportPrintScanCountAI8(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAI8(pcFP20x0Module);
                break;
            case cFP20x0_modType_AI16:
                if(level>2) drvcFP20x0_reportPrintScanCountAI16(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAI16(pcFP20x0Module);
                break;
            case cFP20x0_modType_AI32: 
                if(level>2) drvcFP20x0_reportPrintScanCountAI32(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAI32(pcFP20x0Module);
                break;
            case cFP20x0_modType_AO2:
                if(level>2) drvcFP20x0_reportPrintScanCountAO2(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAO2(pcFP20x0Module);
                break;
            case cFP20x0_modType_AO8:
                if(level>2) drvcFP20x0_reportPrintScanCountAO8(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAO8(pcFP20x0Module);
                break;
            case cFP20x0_modType_AO16: 
                if(level>2) drvcFP20x0_reportPrintScanCountAO16(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAO16(pcFP20x0Module);
                break;
            case cFP20x0_modType_AO32:
                if(level>2) drvcFP20x0_reportPrintScanCountAO32(pcFP20x0Module);
                if(level>3) drvcFP20x0_reportPrintDataAO32(pcFP20x0Module);
                break;
            case cFP20x0_modType_term: break;
        }

        pcFP20x0Module = (cFP20x0Module*) ellNext(&pcFP20x0Module->node);
    } while(pcFP20x0Module); 

    if(level>1) {
        epicsTimeToStrftime(buf, 40, "%Y/%m/%d %H:%M:%S.%09f",&pdrvcFP20x0Config->stamp); 
        printf("    \
cbCount %d,  timeoutCount %d,  postReadFailCount %d,  \ 
asynScanLostCount %ld,    asynRequestLostCount %ld,   scanRate %f\n    \
last scan Time %s\n",
                       pdrvcFP20x0Config->cbCount, pdrvcFP20x0Config->timeoutCount,
                       pdrvcFP20x0Config->postReadFailCount,
                       pdrvcFP20x0Config->asynScanLostCount,
                       pdrvcFP20x0Config->asynRequestLostCount,
                       pdrvcFP20x0Config->scanRate,
                       buf);                   
    }

    return;
}


LOCAL void drvcFP20x0_reportPrintScanCountDI8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI8 *pcFP20x0_modConfig_DI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI8;
    unsigned long *scanCnt = pcFP20x0_modConfig_DI8->scanCnt;
   
    epicsMutexLock(pcFP20x0Module->lock); 
    printf("            - scan count: %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3],
           scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7]);
    printf("            - scan count (mbbi, Li): %8ld\n",
           pcFP20x0_modConfig_DI8->scanCntMbbiLi);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountDI16(cFP20x0Module *pcFP20x0Module) 
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI16 *pcFP20x0_modConfig_DI16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI16;
    unsigned long *scanCnt = pcFP20x0_modConfig_DI16->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
        printf("\
           - scan count: %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n\
                         %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3],
           scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7],
           scanCnt[8], scanCnt[9], scanCnt[10], scanCnt[11],
           scanCnt[12], scanCnt[13], scanCnt[14], scanCnt[15]);
        printf("\
           - scan count (mbbi, Li): %8ld\n",
           pcFP20x0_modConfig_DI16->scanCntMbbiLi);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountDI32(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }

LOCAL void drvcFP20x0_reportPrintScanCountDO8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO8 *pcFP20x0_modConfig_DO8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO8;
    unsigned long *scanCnt = pcFP20x0_modConfig_DO8->scanCnt;
   
    epicsMutexLock(pcFP20x0Module->lock); 
    printf("           - scan count: %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3],
           scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7]);
    printf("           - scan count (mbbo, Lo): %8ld\n",
           pcFP20x0_modConfig_DO8->scanCntMbboLo);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountDO16(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO16 *pcFP20x0_modConfig_DO16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO16;
    unsigned long *scanCnt = pcFP20x0_modConfig_DO16->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
        printf("\
           - scan count: %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n\
                         %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3],
           scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7],
           scanCnt[8], scanCnt[9], scanCnt[10], scanCnt[11],
           scanCnt[12], scanCnt[13], scanCnt[14], scanCnt[15]);
    printf("           - scan count (mbbo, Lo): %8ld\n",
           pcFP20x0_modConfig_DO16->scanCntMbboLo);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountDO32(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }

LOCAL void drvcFP20x0_reportPrintScanCountAI2(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI2 *pcFP20x0_modConfig_AI2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI2;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI2->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("           - scan count: %8ld %8ld\n", scanCnt[0], scanCnt[1]);
    epicsMutexUnlock(pcFP20x0Module->lock);

}

LOCAL void drvcFP20x0_reportPrintScanCountAI8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI8 *pcFP20x0_modConfig_AI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI8;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI8->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - scan count: %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3],
           scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountAI16(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }
LOCAL void drvcFP20x0_reportPrintScanCountAI32(cFP20x0Module *pcFP20x0Module) {
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI32 *pcFP20x0_modConfig_AI32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI32;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI32->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("\
           - scan count:%8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n\
                        %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n\
                        %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n\
                        %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3], scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7],
           scanCnt[8], scanCnt[9], scanCnt[10], scanCnt[11], scanCnt[12], scanCnt[13], scanCnt[14], scanCnt[15],
           scanCnt[16], scanCnt[17], scanCnt[18], scanCnt[19], scanCnt[20], scanCnt[21], scanCnt[22], scanCnt[23],
           scanCnt[24], scanCnt[25], scanCnt[26], scanCnt[27], scanCnt[28], scanCnt[29], scanCnt[30], scanCnt[31]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountAO2(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO2 *pcFP20x0_modConfig_AO2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO2;
    unsigned long *scanCnt = pcFP20x0_modConfig_AO2->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("           - scan count: %8ld %8ld\n", scanCnt[0], scanCnt[1]);
    epicsMutexUnlock(pcFP20x0Module->lock); 
}

LOCAL void drvcFP20x0_reportPrintScanCountAO8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO8 *pcFP20x0_modConfig_AO8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO8;
    unsigned long *scanCnt = pcFP20x0_modConfig_AO8->scanCnt;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - scan count: %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld\n",
           scanCnt[0], scanCnt[1], scanCnt[2], scanCnt[3],
           scanCnt[4], scanCnt[5], scanCnt[6], scanCnt[7]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintScanCountAO16(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }
LOCAL void drvcFP20x0_reportPrintScanCountAO32(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }

LOCAL void drvcFP20x0_reportPrintDataDI8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI8 *pcFP20x0_modConfig_DI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI8;
    epicsUInt8 data = pcFP20x0_modConfig_DI8->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: 0x%2.2x\n", data);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataDI16(cFP20x0Module *pcFP20x0Module) 
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI16 *pcFP20x0_modConfig_DI16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI16;
    epicsUInt16 data = pcFP20x0_modConfig_DI16->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: 0x%4.4x\n", data);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataDI32(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI32 *pcFP20x0_modConfig_DI32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI32;
    epicsUInt32 data = pcFP20x0_modConfig_DI32->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: 0x%8.8x\n", data);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataDO8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO8 *pcFP20x0_modConfig_DO8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO8;
    epicsUInt8 data = pcFP20x0_modConfig_DO8->data;
        
    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: 0x%2.2x\n", data);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataDO16(cFP20x0Module *pcFP20x0Module) 
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO16 *pcFP20x0_modConfig_DO16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO16;
    epicsUInt16 data = pcFP20x0_modConfig_DO16->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: 0x%4.4x\n", data);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataDO32(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO32 *pcFP20x0_modConfig_DO32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO32;
    epicsUInt32 data = pcFP20x0_modConfig_DO32->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: 0x%8.8x\n", data);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataAI2(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI2 *pcFP20x0_modConfig_AI2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI2;
    epicsFloat32 *data = pcFP20x0_modConfig_AI2->data;
  
    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: %10.7e %10.7e\n", data[0], data[1]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataAI8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI8 *pcFP20x0_modConfig_AI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI8;
    epicsFloat32 *data = pcFP20x0_modConfig_AI8->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: %10.7e %10.7e %10.7e %10.7e %10.7e %10.7e %10.7e %10.7e\n",
            data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataAI16(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }
LOCAL void drvcFP20x0_reportPrintDataAI32(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }

LOCAL void drvcFP20x0_reportPrintDataAO2(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO2 *pcFP20x0_modConfig_AO2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO2;
    epicsFloat32 *data = pcFP20x0_modConfig_AO2->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: %10.7e %10.7e\n", data[0], data[1]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataAO8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO8 *pcFP20x0_modConfig_AO8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO8;
    epicsFloat32 *data = pcFP20x0_modConfig_AO8->data;

    epicsMutexLock(pcFP20x0Module->lock);
    printf("            - data: %10.7e %10.7e %10.7e %10.7e %10.7e %10.7e %10.7e %10.7e\n",
            data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_reportPrintDataAO16(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }
LOCAL void drvcFP20x0_reportPrintDataAO32(cFP20x0Module *pcFP20x0Module) { /* not implemented yet */ }


LOCAL int drvcFP20x0_setDelay(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf)
{
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdrvcFP20x0CallbackQueueBuf->userPvt;
    asynUser *pasyncFP20x0User = (asynUser*) pdrvcFP20x0Config->pasyncFP20x0User;
    asyncFP20x0UserData *pasyncFP20x0UserData = (asyncFP20x0UserData*) pdrvcFP20x0Config->pasyncFP20x0UserData;
    asynOctet *pasyncFP20x0Octet = (asynOctet*) pasyncFP20x0UserData->pasyncFP20x0Octet;
    void *pdrvPvt = (void*) pasyncFP20x0UserData->pdrvPvt;
    char *msgBuf = pdrvcFP20x0CallbackQueueBuf->msgBuf;
    char buf[40];
    int tryCnt = 0;

    sprintf(buf, "D%d%s", pdrvcFP20x0Config->monitorInterval, EOS_STR);

    pasyncFP20x0Octet->setEos(pdrvPvt, pasyncFP20x0User, EOS_STR, strlen(EOS_STR));
    pasyncFP20x0Octet->flush(pdrvPvt, pasyncFP20x0User); 
    pasyncFP20x0Octet->write(pdrvPvt, pasyncFP20x0User, buf, strlen(buf));
    pasyncFP20x0Octet->read(pdrvPvt, pasyncFP20x0User, msgBuf, cFP20x0_MSGBUFSIZE);
    if(msgBuf[0] != 'D' && msgBuf[0] != 'd')
        pasyncFP20x0Octet->read(pdrvPvt, pasyncFP20x0User, msgBuf, cFP20x0_MSGBUFSIZE);


    return tryCnt;
}


LOCAL int drvcFP20x0_checkModule(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf)
{
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdrvcFP20x0CallbackQueueBuf->userPvt;
    asynUser *pasyncFP20x0User = (asynUser*) pdrvcFP20x0Config->pasyncFP20x0User;
    asyncFP20x0UserData *pasyncFP20x0UserData = (asyncFP20x0UserData*) pdrvcFP20x0Config->pasyncFP20x0UserData;
    asynOctet *pasyncFP20x0Octet = (asynOctet*) pasyncFP20x0UserData->pasyncFP20x0Octet;
    void *pdrvPvt = (void*)pasyncFP20x0UserData->pdrvPvt;
    char *msgBuf = pdrvcFP20x0CallbackQueueBuf->msgBuf;
    int tryCnt =0;
    int i;


    pasyncFP20x0Octet->setEos(pdrvPvt, pasyncFP20x0User, EOS_STR, strlen(EOS_STR));
    do { 
        if(tryCnt++) epicsThreadSleep(.02);
        pasyncFP20x0Octet->flush(pdrvPvt, pasyncFP20x0User);
        pasyncFP20x0Octet->write(pdrvPvt, pasyncFP20x0User, 
                                 cFP20x0_CMD_STR_CHECKMODULE EOS_STR, strlen(cFP20x0_CMD_STR_CHECKMODULE EOS_STR));
        pasyncFP20x0Octet->read(pdrvPvt, pasyncFP20x0User, msgBuf, cFP20x0_MSGBUFSIZE);
        if(msgBuf[0] != 'C' && msgBuf[0] != 'c')
            pasyncFP20x0Octet->read(pdrvPvt, pasyncFP20x0User, msgBuf, cFP20x0_MSGBUFSIZE);
        /*
        for(i=0;i<cFP20x0_MSGBUFSIZE-10;i++) {
            if((msgBuf[i] == 'C' || msgBuf[i] == 'c')) {
                msgBuf = msgBuf+i;
                break;
            }
        } */
    } while (msgBuf[0] != 'C' && msgBuf[0] != 'c');

    pdrvcFP20x0CallbackQueueBuf->msgBuf = msgBuf;


    return tryCnt;
}


LOCAL int drvcFP20x0_postCheckModule(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf)
{
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdrvcFP20x0CallbackQueueBuf->userPvt;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = (drvcFP20x0MsgBufNode*) pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode;
    char *modStr[16];
    int i,j;
    int tryCnt =0;
    char aiCnt =0, aoCnt =0, diCnt =0, doCnt =0;
    cFP20x0Module *pcFP20x0Module = NULL;

    i = drvcFP20x0_takeTokenCheckModule(pdrvcFP20x0CallbackQueueBuf->msgBuf, modStr);    

    for(tryCnt = 0; tryCnt < i; tryCnt++) {
        j = 0;
        do {
            if(!strcmp((tmodTypeStr+j)->modStr, modStr[tryCnt])) break;
            j++;
        } while((tmodTypeStr+j)->modStr);
       
        if((tmodTypeStr+j)->modType == cFP20x0_modType_term) {
            epicsPrintf("drvcFP20x0: (%s) Can not understand module type %s\n", 
                         pdrvcFP20x0Config->port_name, modStr[tryCnt]);
            return 0;
        }

        pcFP20x0Module = (cFP20x0Module*)malloc(sizeof(cFP20x0Module));
        if(!pcFP20x0Module) {
            epicsPrintf("drvcFP20x0: (%s)memory allocation error for %s\n", 
                        pdrvcFP20x0Config->port_name, (tmodTypeStr+j)->modStr);
            return 0;
        }
        pcFP20x0Module->modType = (tmodTypeStr+j)->modType;
        pcFP20x0Module->modStr  = (tmodTypeStr+j)->modStr;
        pcFP20x0Module->numCh   = (tmodTypeStr+j)->numCh;
        pcFP20x0Module->lock    = epicsMutexCreate();
        pcFP20x0Module->pdrvcFP20x0Config = pdrvcFP20x0Config;

        if(pcFP20x0Module->modType>=cFP20x0_modType_DI8 && pcFP20x0Module->modType<=cFP20x0_modType_DI32)
            sprintf(pcFP20x0Module->modLabel, "%s%d", "DI", diCnt++);
        else if(pcFP20x0Module->modType>=cFP20x0_modType_DO8 && pcFP20x0Module->modType<=cFP20x0_modType_DO32)
            sprintf(pcFP20x0Module->modLabel, "%s%d", "DO", doCnt++);
        else if(pcFP20x0Module->modType>=cFP20x0_modType_AI8 && pcFP20x0Module->modType<=cFP20x0_modType_AI32)
            sprintf(pcFP20x0Module->modLabel, "%s%d", "AI", aiCnt++);
        else if(pcFP20x0Module->modType>=cFP20x0_modType_AO8 && pcFP20x0Module->modType<=cFP20x0_modType_AO32)
            sprintf(pcFP20x0Module->modLabel, "%s%d", "AO", aoCnt++);

        switch(pcFP20x0Module->modType) {
            case cFP20x0_modType_DI8:  drvcFP20x0_initModTypeDI8(pcFP20x0Module); break;
            case cFP20x0_modType_DI16: drvcFP20x0_initModTypeDI16(pcFP20x0Module); break;
            case cFP20x0_modType_DI32: drvcFP20x0_initModTypeDI32(pcFP20x0Module); break;
            case cFP20x0_modType_DO8:  drvcFP20x0_initModTypeDO8(pcFP20x0Module); break;
            case cFP20x0_modType_DO16: drvcFP20x0_initModTypeDO16(pcFP20x0Module); break;
            case cFP20x0_modType_DO32: drvcFP20x0_initModTypeDO32(pcFP20x0Module); break;
            case cFP20x0_modType_AI2:  drvcFP20x0_initModTypeAI2(pcFP20x0Module); break;
            case cFP20x0_modType_AI8:  drvcFP20x0_initModTypeAI8(pcFP20x0Module); break;
            case cFP20x0_modType_AI16: drvcFP20x0_initModTypeAI16(pcFP20x0Module); break;
            case cFP20x0_modType_AI32: drvcFP20x0_initModTypeAI32(pcFP20x0Module); break;
            case cFP20x0_modType_AO2:  drvcFP20x0_initModTypeAO2(pcFP20x0Module); break;
            case cFP20x0_modType_AO8:  drvcFP20x0_initModTypeAO8(pcFP20x0Module); break;
            case cFP20x0_modType_AO16: drvcFP20x0_initModTypeAO16(pcFP20x0Module); break;
            case cFP20x0_modType_AO32: drvcFP20x0_initModTypeAO32(pcFP20x0Module); break;
            case cFP20x0_modType_term: break;
        } 

        ellAdd(pdrvcFP20x0Config->pcFP20x0Module_ellList, &pcFP20x0Module->node);
       
        #ifdef DEBUG 
        epicsPrintf("drvFP20x0: (%s) allocation succeed for module type %s\n", 
                    pdrvcFP20x0Config->port_name,(tmodTypeStr+j)->modStr);
        #endif
    }
    epicsMutexLock(pdrvcFP20x0Config->lock);
    pdrvcFP20x0Config->flg_moduleConstruction = 0x00;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);


    /* add msgBuf to the pool */
    epicsMutexLock(drvcFP20x0MsgBufLock);
    ellAdd(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node);
    epicsMutexUnlock(drvcFP20x0MsgBufLock);

    return tryCnt;
}


LOCAL void drvcFP20x0_initModTypeDI8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig     *pcFP20x0_modConfig     = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI8 *pcFP20x0_modConfig_DI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI8;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI8->data   = (epicsUInt8) 0x00;
    pcFP20x0_modConfig_DI8->o_data = (epicsUInt8) 0x00;
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_DI8->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_DI8->ioScanPvt[i]);
    }
    pcFP20x0_modConfig_DI8->scanCntMbbiLi = 0;
    scanIoInit(&pcFP20x0_modConfig_DI8->ioScanMbbiLiPvt);
    epicsMutexUnlock(pcFP20x0Module->lock);
    
}

LOCAL void drvcFP20x0_initModTypeDI16(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI16 *pcFP20x0_modConfig_DI16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI16;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI16->data = (epicsUInt16) 0x0000;
    pcFP20x0_modConfig_DI16->o_data = (epicsUInt16) 0x0000;
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_DI16->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_DI16->ioScanPvt[i]);
    }
    pcFP20x0_modConfig_DI16->scanCntMbbiLi = 0;
    scanIoInit(&pcFP20x0_modConfig_DI16->ioScanMbbiLiPvt);
    epicsMutexUnlock(pcFP20x0Module->lock);

}

LOCAL void drvcFP20x0_initModTypeDI32(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI32 *pcFP20x0_modConfig_DI32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI32;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI32->data = (epicsUInt32) 0x00000000;
    pcFP20x0_modConfig_DI32->o_data = (epicsUInt32) 0x00000000;
    for(i=0; i<pcFP20x0Module->numCh; i++){
        pcFP20x0_modConfig_DI32->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_DI32->ioScanPvt[i]);
    }    
    pcFP20x0_modConfig_DI32->scanCntMbbiLi = 0;
    scanIoInit(&pcFP20x0_modConfig_DI32->ioScanMbbiLiPvt);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeDO8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO8  *pcFP20x0_modConfig_DO8  = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO8;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO8->data = (epicsUInt8) 0x00;
    pcFP20x0_modConfig_DO8->o_data = (epicsUInt8) 0x00;
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_DO8->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_DO8->ioScanPvt[i]);
    }
    pcFP20x0_modConfig_DO8->scanCntMbboLo = 0;
    scanIoInit(&pcFP20x0_modConfig_DO8->ioScanMbboLoPvt);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeDO16(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO16 *pcFP20x0_modConfig_DO16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO16;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO16->data = (epicsUInt16) 0x0000;
    pcFP20x0_modConfig_DO16->o_data = (epicsUInt16) 0x0000;
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_DO16->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_DO16->ioScanPvt[i]);
    }
    pcFP20x0_modConfig_DO16->scanCntMbboLo = 0;
    scanIoInit(&pcFP20x0_modConfig_DO16->ioScanMbboLoPvt);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeDO32(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO32 *pcFP20x0_modConfig_DO32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO32;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO32->data = (epicsUInt32) 0x00000000;
    pcFP20x0_modConfig_DO32->o_data = (epicsUInt32) 0x00000000;
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_DO32->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_DO32->ioScanPvt[i]);
    }
    pcFP20x0_modConfig_DO32->scanCntMbboLo = 0;
    scanIoInit(&pcFP20x0_modConfig_DO32->ioScanMbboLoPvt);
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAI2(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig     *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI2 *pcFP20x0_modConfig_AI2  = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI2;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
        for(i=0; i<pcFP20x0Module->numCh; i++) {
            pcFP20x0_modConfig_AI2->data[i] = 0.;
            pcFP20x0_modConfig_AI2->o_data[i] = 0.;
            pcFP20x0_modConfig_AI2->scanCnt[i] = 0;
            scanIoInit(&pcFP20x0_modConfig_AI2->ioScanPvt[i]);
        }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAI8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI8  *pcFP20x0_modConfig_AI8  = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI8;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_AI8->data[i] = 0.;
        pcFP20x0_modConfig_AI8->o_data[i] = 0.;
        pcFP20x0_modConfig_AI8->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_AI8->ioScanPvt[i]);
    }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAI16(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI16 *pcFP20x0_modConfig_AI16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI16;
    int i;
  
    epicsMutexLock(pcFP20x0Module->lock);
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_AI16->data[i] = 0.;
        pcFP20x0_modConfig_AI16->o_data[i] = 0.;
        pcFP20x0_modConfig_AI16->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_AI16->ioScanPvt[i]);
    }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAI32(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI32 *pcFP20x0_modConfig_AI32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI32;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_AI32->data[i] = 0.;
        pcFP20x0_modConfig_AI32->o_data[i] = 0.;
        pcFP20x0_modConfig_AI32->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_AI32->ioScanPvt[i]);
    }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAO2(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig     = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO2  *pcFP20x0_modConfig_AO2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO2;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
        for(i=0; i<pcFP20x0Module->numCh; i++) {
            pcFP20x0_modConfig_AO2->data[i] = 0.;
            pcFP20x0_modConfig_AO2->o_data[i] = 0.;
            pcFP20x0_modConfig_AO2->scanCnt[i] = 0;
            scanIoInit(&pcFP20x0_modConfig_AO2->ioScanPvt[i]);
        }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAO8(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO8  *pcFP20x0_modConfig_AO8  = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO8;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    for(i=0; i<pcFP20x0Module->numCh; i++) {
        pcFP20x0_modConfig_AO8->data[i] = 0.;
        pcFP20x0_modConfig_AO8->o_data[i] = 0.;
        pcFP20x0_modConfig_AO8->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_AO8->ioScanPvt[i]);
    }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAO16(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO16 *pcFP20x0_modConfig_AO16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO16;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    for(i=0; i<pcFP20x0Module->numCh;i++) {
        pcFP20x0_modConfig_AO16->data[i] = 0.;
        pcFP20x0_modConfig_AO16->o_data[i] = 0.;
        pcFP20x0_modConfig_AO16->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_AO16->ioScanPvt[i]);
    }
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_initModTypeAO32(cFP20x0Module *pcFP20x0Module)
{
    cFP20x0_modConfig      *pcFP20x0_modConfig      = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO32 *pcFP20x0_modConfig_AO32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO32;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    for(i=0; i<pcFP20x0Module->numCh;i++) {
        pcFP20x0_modConfig_AO32->data[i] = 0.;
        pcFP20x0_modConfig_AO32->o_data[i] = 0.;
        pcFP20x0_modConfig_AO32->scanCnt[i] = 0;
        scanIoInit(&pcFP20x0_modConfig_AO32->ioScanPvt[i]);
    }
    epicsMutexUnlock(pcFP20x0Module->lock);
}


LOCAL int drvcFP20x0_takeTokenCheckModule(char *msgBuf, char *modStr[])
{
    int i =0;
    int j =0;
    int k;


    modStr[0] = msgBuf+1;
    while(msgBuf[i++] != '\r' && j<16) {
       if(msgBuf[i-1] == '\t' || msgBuf[i-1] == ' ') {
           msgBuf[i-1] = NULL;
           modStr[++j] = msgBuf+i;
       }
    }
    msgBuf[i-1] = NULL;

    printf("drvcFP20x0 (check module): ");
    for(k=0;k<j+1;k++) printf("%s ", modStr[k]); 
    printf("\n");

    return j+1;
}


LOCAL int drvcFP20x0_destructModule(drvcFP20x0Config *pdrvcFP20x0Config)
{
    cFP20x0Module *pcFP20x0Module = NULL;
    int tryCnt=0;

    epicsMutexLock(pdrvcFP20x0Config->lock);
    pdrvcFP20x0Config->flg_moduleConstruction = 0xff;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);
 
    while(ellCount(pdrvcFP20x0Config->pcFP20x0Module_ellList)>0) {
        tryCnt++;
        pcFP20x0Module = (cFP20x0Module*) ellFirst(pdrvcFP20x0Config->pcFP20x0Module_ellList);
        ellDelete(pdrvcFP20x0Config->pcFP20x0Module_ellList, &pcFP20x0Module->node);
        epicsPrintf("drvFP20x0: (%s) destruction succeed for module type %s\n",
                    pdrvcFP20x0Config->port_name, pcFP20x0Module->modStr);
        free(pcFP20x0Module);
    }

    epicsMutexLock(pdrvcFP20x0Config->lock);
    pdrvcFP20x0Config->flg_moduleConstruction = 0x00;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);
    return tryCnt;
}


LOCAL int drvcFP20x0_read(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf)
{
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdrvcFP20x0CallbackQueueBuf->userPvt;
    asynUser *pasyncFP20x0User = (asynUser*) pdrvcFP20x0Config->pasyncFP20x0User;
    asyncFP20x0UserData *pasyncFP20x0UserData = (asyncFP20x0UserData*) pdrvcFP20x0Config->pasyncFP20x0UserData;
    asynOctet *pasyncFP20x0Octet = (asynOctet*) pasyncFP20x0UserData->pasyncFP20x0Octet;
    void *pdrvPvt = (void*)pasyncFP20x0UserData->pdrvPvt;
    char *msgBuf = pdrvcFP20x0CallbackQueueBuf->msgBuf;
    int tryCnt = 0;

    pasyncFP20x0Octet->setEos(pdrvPvt, pasyncFP20x0User, EOS_STR, strlen(EOS_STR));
    do {
        tryCnt++;
        pasyncFP20x0Octet->read(pdrvPvt, pasyncFP20x0User, msgBuf, cFP20x0_MSGBUFSIZE);
    } while(msgBuf[0] != 'B' && msgBuf[0] != 'b');

    return tryCnt;
}


LOCAL int drvcFP20x0_write(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf)
{
    drvcFP20x0Config *pdrvcFP20x0Config = (drvcFP20x0Config*) pdrvcFP20x0CallbackQueueBuf->userPvt;
    asynUser *pasyncFP20x0User = (asynUser*) pdrvcFP20x0Config->pasyncFP20x0User;
    asyncFP20x0UserData *pasyncFP20x0UserData = (asyncFP20x0UserData*) pdrvcFP20x0Config->pasyncFP20x0UserData;
    asynOctet *pasyncFP20x0Octet = (asynOctet*) pasyncFP20x0UserData->pasyncFP20x0Octet;

    void *pdrvPvt = (void*)pasyncFP20x0UserData->pdrvPvt;
    char *msgBuf = pdrvcFP20x0CallbackQueueBuf->msgBuf;
    int tryCnt = 0;

    pasyncFP20x0Octet->setEos(pdrvPvt, pasyncFP20x0User, EOS_STR, strlen(EOS_STR));
    pasyncFP20x0Octet->write(pdrvPvt, pasyncFP20x0User, msgBuf, strlen(msgBuf));
    /*
    do {
        tryCnt++;
        pasyncFP20x0Octet->read(pdrvPvt, pasyncFP20x0User, msgBuf, cFP20x0_MSGBUFSIZE);
    } while(msgBuf[0] != 'W' && msgBuf[0] != 'w');
    */

   return tryCnt;

}


LOCAL int drvcFP20x0_requestSetDelay(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config)
{
    asynStatus vasynStatus;
    int nbytes;
    int tryCnt = 0;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode;

    epicsMutexLock(drvcFP20x0MsgBufLock);
    pdrvcFP20x0MsgBufNode = (drvcFP20x0MsgBufNode*)ellFirst(pdrvcFP20x0MsgBufNode_ellList);
    ellDelete(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node);
    epicsMutexUnlock(drvcFP20x0MsgBufLock);

    pdrvcFP20x0MsgBufNode->cnt++;

    pdrvcFP20x0CallbackQueueBuf->command = cFP20x0_CMD_SETDELAY;
    pdrvcFP20x0CallbackQueueBuf->userPvt = (void*) pdrvcFP20x0Config;
    pdrvcFP20x0CallbackQueueBuf->precord = (dbCommon*) NULL;
    pdrvcFP20x0CallbackQueueBuf->msgBuf  = pdrvcFP20x0MsgBufNode->msgBuf;
    pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode = pdrvcFP20x0MsgBufNode;

    nbytes = epicsRingBytesPut(pdrvcFP20x0Config->queueCallbackRingBytesId,
                               (char*)pdrvcFP20x0CallbackQueueBuf,
                               sizeof(drvcFP20x0CallbackQueueBuf));

    vasynStatus = pasynManager->queueRequest(pdrvcFP20x0Config->pasyncFP20x0User,
                                             asynQueuePriorityLow,
                                             pdrvcFP20x0Config->cbTimeout);

    epicsMutexLock(pdrvcFP20x0Config->lock);
    pdrvcFP20x0Config->cbCount++;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    if(vasynStatus != asynSuccess) {
        epicsMutexLock(pdrvcFP20x0Config->lock);
        pdrvcFP20x0Config->asynRequestLostCount++;
        epicsMutexUnlock(pdrvcFP20x0Config->lock);
    }
    
    return tryCnt;
}


LOCAL int drvcFP20x0_requestCheck(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config)
{
    asynStatus vasynStatus;
    int nbytes;
    int tryCnt = 0;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode;

    epicsMutexLock(drvcFP20x0MsgBufLock);
    pdrvcFP20x0MsgBufNode = (drvcFP20x0MsgBufNode*)ellFirst(pdrvcFP20x0MsgBufNode_ellList);
    ellDelete(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node); 
    epicsMutexUnlock(drvcFP20x0MsgBufLock);

    pdrvcFP20x0MsgBufNode->cnt++;

    pdrvcFP20x0CallbackQueueBuf->command = cFP20x0_CMD_CHECKMODULE;
    pdrvcFP20x0CallbackQueueBuf->userPvt = (void*) pdrvcFP20x0Config;
    pdrvcFP20x0CallbackQueueBuf->precord = (dbCommon*) NULL;
    pdrvcFP20x0CallbackQueueBuf->msgBuf  = pdrvcFP20x0MsgBufNode->msgBuf;
    pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode = pdrvcFP20x0MsgBufNode;

    nbytes = epicsRingBytesPut(pdrvcFP20x0Config->queueCallbackRingBytesId,
                               (char*)pdrvcFP20x0CallbackQueueBuf,
                               sizeof(drvcFP20x0CallbackQueueBuf));

    vasynStatus = pasynManager->queueRequest(pdrvcFP20x0Config->pasyncFP20x0User,
                                             asynQueuePriorityLow,
                                             pdrvcFP20x0Config->cbTimeout);

    epicsMutexLock(pdrvcFP20x0Config->lock);
    pdrvcFP20x0Config->cbCount++;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    if(vasynStatus != asynSuccess) {
        epicsMutexLock(pdrvcFP20x0Config->lock);
        pdrvcFP20x0Config->asynRequestLostCount++;
        epicsMutexUnlock(pdrvcFP20x0Config->lock);
    }    

    return tryCnt;    
}


LOCAL int drvcFP20x0_requestRead(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config)
{
    asynStatus vasynStatus;
    int nbytes;
    int tryCnt =0;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = NULL;

    epicsMutexLock(drvcFP20x0MsgBufLock);
        pdrvcFP20x0MsgBufNode = (drvcFP20x0MsgBufNode*) ellFirst(pdrvcFP20x0MsgBufNode_ellList);
        ellDelete(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node);
    epicsMutexUnlock(drvcFP20x0MsgBufLock);

    pdrvcFP20x0MsgBufNode->cnt++;

    pdrvcFP20x0CallbackQueueBuf->command = cFP20x0_CMD_READ;
    pdrvcFP20x0CallbackQueueBuf->userPvt = (void*) pdrvcFP20x0Config;
    pdrvcFP20x0CallbackQueueBuf->precord = (dbCommon*) NULL;
    pdrvcFP20x0CallbackQueueBuf->msgBuf  = pdrvcFP20x0MsgBufNode->msgBuf;
    pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode = pdrvcFP20x0MsgBufNode;


    nbytes = epicsRingBytesPut(pdrvcFP20x0Config->queueCallbackRingBytesId,
                               (char*) pdrvcFP20x0CallbackQueueBuf,
                               sizeof(drvcFP20x0CallbackQueueBuf)); 
    vasynStatus = pasynManager->queueRequest(pdrvcFP20x0Config->pasyncFP20x0User,
                                             asynQueuePriorityLow,
                                             pdrvcFP20x0Config->cbTimeout);

    epicsMutexLock(pdrvcFP20x0Config->lock);
    pdrvcFP20x0Config->cbCount++;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    if(vasynStatus != asynSuccess) {
        epicsMutexLock(pdrvcFP20x0Config->lock);
        pdrvcFP20x0Config->asynRequestLostCount++;
        epicsMutexUnlock(pdrvcFP20x0Config->lock);
    }

    return tryCnt;
}


LOCAL int drvcFP20x0_requestWrite(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf,
                                  drvcFP20x0Config *pdrvcFP20x0Config,
                                  char *commandBuf,
                                  dbCommon *precord)
{
    asynStatus vasynStatus;
    int nbytes;
    int tryCnt = 0;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = NULL;

    epicsMutexLock(drvcFP20x0MsgBufLock);
        pdrvcFP20x0MsgBufNode = (drvcFP20x0MsgBufNode*) ellFirst(pdrvcFP20x0MsgBufNode_ellList);
        ellDelete(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node);
    epicsMutexUnlock(drvcFP20x0MsgBufLock);

    pdrvcFP20x0MsgBufNode->cnt++;

    pdrvcFP20x0CallbackQueueBuf->command = cFP20x0_CMD_WRITE;
    pdrvcFP20x0CallbackQueueBuf->userPvt = (void*) pdrvcFP20x0Config;
    pdrvcFP20x0CallbackQueueBuf->precord = precord;
    pdrvcFP20x0CallbackQueueBuf->msgBuf = pdrvcFP20x0MsgBufNode->msgBuf;
    pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode = pdrvcFP20x0MsgBufNode;

    strcpy(pdrvcFP20x0CallbackQueueBuf->msgBuf, commandBuf);

    nbytes = epicsRingBytesPut(pdrvcFP20x0Config->queueCallbackRingBytesId,
                               (char*) pdrvcFP20x0CallbackQueueBuf,
                               sizeof(drvcFP20x0CallbackQueueBuf));
    vasynStatus = pasynManager->queueRequest(pdrvcFP20x0Config->pasyncFP20x0User,
                                             asynQueuePriorityLow,
                                             pdrvcFP20x0Config->cbTimeout);

    epicsMutexLock(pdrvcFP20x0Config->lock);
        pdrvcFP20x0Config->cbCount++;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    if(vasynStatus != asynSuccess) {
        epicsMutexLock(pdrvcFP20x0Config->lock);
            pdrvcFP20x0Config->asynRequestLostCount++; 
        epicsMutexUnlock(pdrvcFP20x0Config->lock);
    }

    
    

}

LOCAL int drvcFP20x0_postSetDelay(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config)
{
    int tryCnt = 0;
  
    return tryCnt;
}

LOCAL int drvcFP20x0_postRead(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config)
{
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode;
    cFP20x0Module *pcFP20x0Module = NULL;
    char *msgBuf;
    int bufCnt = 0;
    int nbytes = 0;
    int tryCnt = 0;


    /* strip off garbage data */
    while(nbytes < cFP20x0_MSGBUFSIZE) {
        if(pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes] == '\r') {
            pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes] = (char)NULL;
            break;
        }
        nbytes++;
    }
    pdrvcFP20x0CallbackQueueBuf->msgBuf[cFP20x0_MSGBUFSIZE-1] = (char)NULL;

    #ifdef DEBUG
    epicsPrintf("drvcFP20x0: port(%s) %s\n", pdrvcFP20x0Config->port_name, pdrvcFP20x0CallbackQueueBuf->msgBuf);
    #endif

    if((ellCount(pdrvcFP20x0Config->pcFP20x0Module_ellList)<1) ||
       (pdrvcFP20x0CallbackQueueBuf->msgBuf[0] != 'B' && pdrvcFP20x0CallbackQueueBuf->msgBuf[0] != 'b')) {
        epicsMutexLock(pdrvcFP20x0Config->lock);
        pdrvcFP20x0Config->postReadFailCount++;
        epicsMutexUnlock(pdrvcFP20x0Config->lock);
        return tryCnt;
    }

    nbytes=1;
    pcFP20x0Module = (cFP20x0Module*)ellFirst(pdrvcFP20x0Config->pcFP20x0Module_ellList);
    while(pcFP20x0Module) {
        msgBuf = &pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes];
        while(pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes] != '\t' &&
              pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes] != NULL) {
            nbytes++;
        }
        pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes++] = NULL;

        #ifdef DEBUG
        epicsPrintf("drvcFP20x0: port(%s) modCnt(%d) %s, %d, %s, %d, %s\n",
                    pdrvcFP20x0Config->port_name, tryCnt,
                    pcFP20x0Module->modLabel, pcFP20x0Module->modType, 
                    pcFP20x0Module->modStr, pcFP20x0Module->numCh, msgBuf);
        #endif
 

        switch(pcFP20x0Module->modType) {
            case cFP20x0_modType_DI8: drvcFP20x0_readModTypeDI8(pcFP20x0Module, msgBuf); break; 
            case cFP20x0_modType_DI16: drvcFP20x0_readModTypeDI16(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_DI32: drvcFP20x0_readModTypeDI32(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_DO8: drvcFP20x0_readModTypeDO8(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_DO16: drvcFP20x0_readModTypeDO16(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_DO32: drvcFP20x0_readModTypeDO32(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AI2: drvcFP20x0_readModTypeAI2(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AI8: drvcFP20x0_readModTypeAI8(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AI16: drvcFP20x0_readModTypeAI16(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AI32: drvcFP20x0_readModTypeAI32(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AO2: drvcFP20x0_readModTypeAO2(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AO8: drvcFP20x0_readModTypeAO8(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AO16: drvcFP20x0_readModTypeAO16(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_AO32: drvcFP20x0_readModTypeAO32(pcFP20x0Module, msgBuf); break;
            case cFP20x0_modType_term: break;
        }

        pcFP20x0Module = (cFP20x0Module*)ellNext(&pcFP20x0Module->node);
        tryCnt++;
    }

    msgBuf = &pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes];
    while(pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes] != '\t' &&
          pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes] != NULL) {
          nbytes++;
    }
    pdrvcFP20x0CallbackQueueBuf->msgBuf[nbytes++] = NULL;
    drvcFP20x0_makeTimeStamp(pdrvcFP20x0Config, msgBuf);

    epicsMutexLock(drvcFP20x0MsgBufLock);
        ellAdd(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node);
    epicsMutexUnlock(drvcFP20x0MsgBufLock);
    
    return tryCnt;
}


LOCAL int drvcFP20x0_postWrite(drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf, drvcFP20x0Config *pdrvcFP20x0Config)
{
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = pdrvcFP20x0CallbackQueueBuf->pdrvcFP20x0MsgBufNode;
    dbCommon *precord = NULL;
    struct rset *prset = NULL;
    int tryCnt;

    if((precord = (dbCommon*) pdrvcFP20x0CallbackQueueBuf->precord) &&
       (prset = (struct rset*) precord->rset)) {
        dbScanLock(precord);
            (*prset->process)(precord);
        dbScanUnlock(precord);
    }

    epicsMutexLock(drvcFP20x0MsgBufLock);
        ellAdd(pdrvcFP20x0MsgBufNode_ellList, &pdrvcFP20x0MsgBufNode->node);
    epicsMutexUnlock(drvcFP20x0MsgBufLock);

    return tryCnt;
}


LOCAL void drvcFP20x0_makeTimeStamp(drvcFP20x0Config *pdrvcFP20x0Config, char *msgBuf)
{
    epicsTimeStamp *pstamp = &pdrvcFP20x0Config->stamp;
    double timeNow;
 
    sscanf(msgBuf,"%lf", &timeNow); 

    epicsMutexLock(pdrvcFP20x0Config->lock);
    pstamp->secPastEpoch = (epicsUInt32) timeNow;
    timeNow -= (double) pstamp->secPastEpoch;
    pstamp->nsec = (epicsUInt32) (timeNow * 1.E+9);
    pstamp->secPastEpoch -= (unsigned)2713996800;
    epicsMutexUnlock(pdrvcFP20x0Config->lock);

    scanIoRequest(pdrvcFP20x0Config->stampIoScanPvt);
}


LOCAL void drvcFP20x0_readModTypeDI8(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI8 *pcFP20x0_modConfig_DI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI8;
    unsigned long *scanCnt = pcFP20x0_modConfig_DI8->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_DI8->ioScanPvt;
    int i;
    unsigned int buf;
    epicsUInt8 diff;
    epicsUInt8 mask = 0x01;

    sscanf(msgBuf, "%x",&buf);

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI8->data = (epicsUInt8) buf;
    diff = pcFP20x0_modConfig_DI8->data ^ pcFP20x0_modConfig_DI8->o_data;
    epicsMutexUnlock(pcFP20x0Module->lock);
    
    for(i=0; i<pcFP20x0Module->numCh;i++) {
        if(diff & mask) {
            epicsMutexLock(pcFP20x0Module->lock);
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
        mask<<=1;   
    }

    if(diff) {
        epicsMutexLock(pcFP20x0Module->lock);
            pcFP20x0_modConfig_DI8->scanCntMbbiLi++;
        epicsMutexUnlock(pcFP20x0Module->lock);
        scanIoRequest(pcFP20x0_modConfig_DI8->ioScanMbbiLiPvt);
    }
    
    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI8->o_data = pcFP20x0_modConfig_DI8->data; 
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_readModTypeDI16(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI16 *pcFP20x0_modConfig_DI16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI16;
    unsigned long *scanCnt = pcFP20x0_modConfig_DI16->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_DI16->ioScanPvt;
    int i;
    unsigned int buf;
    epicsUInt16 diff;
    epicsUInt16 mask = 0x0001;
  
    sscanf(msgBuf, "%x", &buf);

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI16->data = (epicsUInt16) buf;
    diff = pcFP20x0_modConfig_DI16->data ^ pcFP20x0_modConfig_DI16->o_data;
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(diff & mask) {
            epicsMutexLock(pcFP20x0Module->lock);
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
        mask<<=1;
    }

    if(diff) {
        epicsMutexLock(pcFP20x0Module->lock);
            pcFP20x0_modConfig_DI16->scanCntMbbiLi++;
        epicsMutexUnlock(pcFP20x0Module->lock);
        scanIoRequest(pcFP20x0_modConfig_DI16->ioScanMbbiLiPvt);
    }

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI16->o_data = pcFP20x0_modConfig_DI16->data;
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_readModTypeDI32(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DI32 *pcFP20x0_modConfig_DI32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DI32;
    unsigned long *scanCnt = pcFP20x0_modConfig_DI32->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_DI32->ioScanPvt;
    int i;
    unsigned int buf;
    epicsUInt32 diff;
    epicsUInt32 mask = 0x00000001;

    sscanf(msgBuf, "%x", &buf);

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI32->data = (epicsUInt32) buf;
    diff = pcFP20x0_modConfig_DI32->data ^ pcFP20x0_modConfig_DI32->o_data;
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(diff & mask) {
            epicsMutexLock(pcFP20x0Module->lock);
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
        mask<<=1;
    }

    if(diff) {
        epicsMutexLock(pcFP20x0Module->lock);
           pcFP20x0_modConfig_DI32->scanCntMbbiLi++;
        epicsMutexUnlock(pcFP20x0Module->lock);
        scanIoRequest(pcFP20x0_modConfig_DI32->ioScanMbbiLiPvt);
    }

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DI32->o_data = pcFP20x0_modConfig_DI32->data;
    epicsMutexUnlock(pcFP20x0Module->lock);

}

LOCAL void drvcFP20x0_readModTypeDO8(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO8 *pcFP20x0_modConfig_DO8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO8;
    unsigned long *scanCnt = pcFP20x0_modConfig_DO8->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_DO8->ioScanPvt;
    int i;
    unsigned int buf;
    epicsUInt8 diff;
    epicsUInt8 mask = 0x01;

    sscanf(msgBuf, "%x", &buf);

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO8->data = (epicsUInt8) buf;
    diff = pcFP20x0_modConfig_DO8->data ^ pcFP20x0_modConfig_DO8->o_data;
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(diff & mask) {
            epicsMutexLock(pcFP20x0Module->lock);
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]); 
        }
        mask<<=1;
    }

    if(diff) {
        epicsMutexLock(pcFP20x0Module->lock);
            pcFP20x0_modConfig_DO8->scanCntMbboLo++;
        epicsMutexUnlock(pcFP20x0Module->lock);
        scanIoRequest(pcFP20x0_modConfig_DO8->ioScanMbboLoPvt);
    }

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO8->o_data = pcFP20x0_modConfig_DO8->data;
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_readModTypeDO16(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO16 *pcFP20x0_modConfig_DO16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO16;
    unsigned long *scanCnt = pcFP20x0_modConfig_DO16->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_DO16->ioScanPvt;
    int i;
    unsigned int buf;
    epicsUInt16 diff;
    epicsUInt16 mask = 0x0001;

    sscanf(msgBuf, "%x", &buf);

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO16->data = (epicsUInt16) buf;
    diff = pcFP20x0_modConfig_DO16->data ^ pcFP20x0_modConfig_DO16->o_data;
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(diff & mask){
            epicsMutexLock(pcFP20x0Module->lock);
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
        mask<<=1;
    }

    if(diff) {
        epicsMutexLock(pcFP20x0Module->lock);
            pcFP20x0_modConfig_DO16->scanCntMbboLo++;
        epicsMutexUnlock(pcFP20x0Module->lock);
        scanIoRequest(pcFP20x0_modConfig_DO16->ioScanMbboLoPvt);
    }

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO16->o_data = pcFP20x0_modConfig_DO16->data;
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_readModTypeDO32(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_DO32 *pcFP20x0_modConfig_DO32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_DO32;
    unsigned long *scanCnt = pcFP20x0_modConfig_DO32->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_DO32->ioScanPvt;
    int i;
    unsigned int buf;
    epicsUInt32 diff;
    epicsUInt32 mask = 0x00000001;

    scanf(msgBuf, "%x", &buf);

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO32->data = (epicsUInt32) buf;
    diff = pcFP20x0_modConfig_DO32->data ^ pcFP20x0_modConfig_DO32->o_data;
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(diff & mask) {
            epicsMutexLock(pcFP20x0Module->lock);
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
        mask<<=1;
    }

    if(diff) {
        epicsMutexLock(pcFP20x0Module->lock);
            pcFP20x0_modConfig_DO32->scanCntMbboLo++;
        epicsMutexUnlock(pcFP20x0Module->lock);
        scanIoRequest(pcFP20x0_modConfig_DO32->ioScanMbboLoPvt);
    }

    epicsMutexLock(pcFP20x0Module->lock);
    pcFP20x0_modConfig_DO32->o_data = pcFP20x0_modConfig_DO32->data;
    epicsMutexUnlock(pcFP20x0Module->lock);
}

LOCAL void drvcFP20x0_readModTypeAI2(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI2 *pcFP20x0_modConfig_AI2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI2;
    epicsFloat32 *data = pcFP20x0_modConfig_AI2->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AI2->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI2->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AI2->ioScanPvt;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
        sscanf(msgBuf, "%e,%e", data+0, data+1);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++){
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
                o_data[i] = data[i];
                scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
    }
}

LOCAL void drvcFP20x0_readModTypeAI8(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI8 *pcFP20x0_modConfig_AI8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI8;
    epicsFloat32 *data = pcFP20x0_modConfig_AI8->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AI8->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI8->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AI8->ioScanPvt;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    sscanf(msgBuf, "%e,%e,%e,%e,%e,%e,%e,%e",
          data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
            o_data[i] = data[i];
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
    } 
}

LOCAL void drvcFP20x0_readModTypeAI16(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI16 *pcFP20x0_modConfig_AI16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI16;
    epicsFloat32 *data = pcFP20x0_modConfig_AI16->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AI16->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI16->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AI16->ioScanPvt;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    sscanf(msgBuf, "%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e",
           data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7,
           data+8, data+9, data+10, data+11, data+12, data+13, data+14, data+15);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
            o_data[i] = data[i];
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
    }

}

LOCAL void drvcFP20x0_readModTypeAI32(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AI32 *pcFP20x0_modConfig_AI32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AI32;
    epicsFloat32 *data = pcFP20x0_modConfig_AI32->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AI32->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AI32->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AI32->ioScanPvt;
    int i;
/*
    printf("%s %s\n", pcFP20x0Module->modLabel, msgBuf); */
    epicsMutexLock(pcFP20x0Module->lock);
    sscanf(msgBuf, "%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,\
                    %e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e", 
           data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7,
           data+8, data+9, data+10, data+11, data+12, data+13, data+14, data+15,
           data+16, data+17, data+18, data+19, data+20, data+21, data+22, data+23,
           data+24, data+25, data+26, data+27, data+28, data+29, data+30, data+31
           );
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
            o_data[i] = data[i];
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
    }
}

LOCAL void drvcFP20x0_readModTypeAO2(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO2 *pcFP20x0_modConfig_AO2 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO2;
    epicsFloat32 *data = pcFP20x0_modConfig_AO2->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AO2->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AO2->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AO2->ioScanPvt;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
        sscanf(msgBuf, "%e,%e", data+0, data+1);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
                o_data[i] = data[i];
                scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
    }
}

LOCAL void drvcFP20x0_readModTypeAO8(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO8 *pcFP20x0_modConfig_AO8 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO8;
    epicsFloat32 *data = pcFP20x0_modConfig_AO8->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AO8->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AO8->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AO8->ioScanPvt;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    sscanf(msgBuf, "%e,%e,%e,%e,%e,%e,%e,%e",
           data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
            o_data[i] = data[i];
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }
    }
}

LOCAL void drvcFP20x0_readModTypeAO16(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO16 *pcFP20x0_modConfig_AO16 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO16;
    epicsFloat32 *data = pcFP20x0_modConfig_AO16->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AO16->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AO16->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AO16->ioScanPvt;
    int i;

    epicsMutexLock(pcFP20x0Module->lock);
    sscanf(msgBuf, "%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e",
           data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7,
           data+8, data+9, data+10, data+11, data+12, data+13, data+14, data+15);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
            o_data[i] = data[i];
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }       
    }
}

LOCAL void drvcFP20x0_readModTypeAO32(cFP20x0Module *pcFP20x0Module, char *msgBuf)
{
    cFP20x0_modConfig *pcFP20x0_modConfig = &pcFP20x0Module->vcFP20x0_modConfig;
    cFP20x0_modConfig_AO32 *pcFP20x0_modConfig_AO32 = &pcFP20x0_modConfig->vcFP20x0_modConfig_AO32;
    epicsFloat32 *data = pcFP20x0_modConfig_AO32->data;
    epicsFloat32 *o_data = pcFP20x0_modConfig_AO32->o_data;
    unsigned long *scanCnt = pcFP20x0_modConfig_AO32->scanCnt;
    IOSCANPVT *ioScanPvt = pcFP20x0_modConfig_AO32->ioScanPvt;
    int i;  

    epicsMutexLock(pcFP20x0Module->lock);
    sscanf(msgBuf, "%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e"
                   "%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e",
           data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7,
           data+8, data+9, data+10, data+11, data+12, data+13, data+14, data+15,
           data+16, data+17, data+18, data+19, data+20, data+21, data+22, data+23,
           data+24, data+25, data+26, data+27, data+28, data+29, data+30, data+31);
    epicsMutexUnlock(pcFP20x0Module->lock);

    for(i=0; i<pcFP20x0Module->numCh; i++) {
        if(data[i] != o_data[i]) {
            epicsMutexLock(pcFP20x0Module->lock);
            o_data[i] = data[i];
            scanCnt[i]++;
            epicsMutexUnlock(pcFP20x0Module->lock);
            scanIoRequest(ioScanPvt[i]);
        }       
    }
}



LOCAL long drvcFP20x0_io_report(int level)
{
    drvcFP20x0Config *pdrvcFP20x0Config = NULL;

    if(!pdrvcFP20x0_ellList || ellCount(pdrvcFP20x0_ellList)<1) return 0;
    printf("Total %d module(s) found\n", ellCount(pdrvcFP20x0_ellList));

    pdrvcFP20x0Config = (drvcFP20x0Config*)ellFirst(pdrvcFP20x0_ellList);
    do {
        drvcFP20x0_reportPrint(pdrvcFP20x0Config, level);
        pdrvcFP20x0Config = (drvcFP20x0Config*)ellNext(&pdrvcFP20x0Config->node);
    } while(pdrvcFP20x0Config);

    {
        drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = ellFirst(pdrvcFP20x0MsgBufNode_ellList);
        printf("buf: %d --", ellCount(pdrvcFP20x0MsgBufNode_ellList));
        do {
            printf("%d ", pdrvcFP20x0MsgBufNode->cnt);
            pdrvcFP20x0MsgBufNode = ellNext(&pdrvcFP20x0MsgBufNode->node);
        } while(pdrvcFP20x0MsgBufNode);
        printf("\n");
    }

    return 0;
}


LOCAL long drvcFP20x0_init_driver(void)
{
    drvcFP20x0Config *pdrvcFP20x0Config = NULL;
    double drvcFP20x0_scanInterval;
    drvcFP20x0CallbackQueueBuf vdrvcFP20x0CallbackQueueBuf;
    drvcFP20x0CallbackQueueBuf *pdrvcFP20x0CallbackQueueBuf = &vdrvcFP20x0CallbackQueueBuf;
    int nbytes;
    int tryCnt;
    int flag = 0;
    asynStatus vasynStatus;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode = NULL;
    int num_msgBufNode;


    /* dynamic allocation for sub module */
    while(!pdrvcFP20x0_ellList || ellCount(pdrvcFP20x0_ellList)<1 || !pdrvcFP20x0MsgBufNode_ellList){
        epicsThreadSleep(1.);
        if(tryCnt++ >10) return -1;
    }

    num_msgBufNode = cFP20x0_CALLBACK_QUEUE_SIZE * ellCount(pdrvcFP20x0_ellList);
    pdrvcFP20x0MsgBufNode = (drvcFP20x0MsgBufNode*) malloc(sizeof(drvcFP20x0MsgBufNode)*num_msgBufNode);
    if(!pdrvcFP20x0MsgBufNode) return -1;

    for(tryCnt = 0; tryCnt < num_msgBufNode; tryCnt++) {
        (pdrvcFP20x0MsgBufNode+tryCnt)->cnt = 0;
        ellAdd(pdrvcFP20x0MsgBufNode_ellList, &(pdrvcFP20x0MsgBufNode+tryCnt)->node);        
    }

    drvcFP20x0MsgBufLock = epicsMutexCreate();

    /* make delay to check module 
    pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
    while(pdrvcFP20x0Config) {
        tryCnt = drvcFP20x0_requestSetDelay(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
        pdrvcFP20x0Config= (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
    } */

    /* check module */
    pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
    while(pdrvcFP20x0Config) {
        if(ellCount(pdrvcFP20x0Config->pcFP20x0Module_ellList)<1 && !pdrvcFP20x0Config->flg_moduleConstruction) {
            epicsMutexLock(pdrvcFP20x0Config->lock);
            pdrvcFP20x0Config->flg_moduleConstruction = 0xff;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);
            tryCnt = drvcFP20x0_requestCheck(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
        }
        pdrvcFP20x0Config = (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
    }


    /* check module allocation */
    tryCnt = 0;
    while(!flag) {
        epicsThreadSleep(.5); flag = 1;
        pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
        while(pdrvcFP20x0Config) {
            if(pdrvcFP20x0Config->flg_moduleConstruction) flag *=0;
            else  flag *= 1;
            pdrvcFP20x0Config = (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
        }
    }

    /* reset delay to operate */
    pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
    while(pdrvcFP20x0Config) {
        if(pdrvcFP20x0Config->monitorInterval>0) {
            tryCnt = drvcFP20x0_requestSetDelay(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
        }
        pdrvcFP20x0Config = (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
    }

    /* start reading */
    pdrvcFP20x0Config = (drvcFP20x0Config*) ellFirst(pdrvcFP20x0_ellList);
    while(pdrvcFP20x0Config) {
        if(ellCount(pdrvcFP20x0Config->pcFP20x0Module_ellList)>0 && pdrvcFP20x0Config->flg_first) {
            epicsMutexLock(pdrvcFP20x0Config->lock);
            pdrvcFP20x0Config->flg_first = 0x00;
            epicsMutexUnlock(pdrvcFP20x0Config->lock);
            tryCnt = drvcFP20x0_requestRead(pdrvcFP20x0CallbackQueueBuf, pdrvcFP20x0Config);
        }
        pdrvcFP20x0Config = (drvcFP20x0Config*) ellNext(&pdrvcFP20x0Config->node);
    }
    
    /* start scan Thread */  
    epicsThreadCreate("drvcFP20x0ScanTask",
                      epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackSmall),
                      (EPICSTHREADFUNC) drvcFP20x0_scanTask,
                      0); 
    

    return 0; 
}
