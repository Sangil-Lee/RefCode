/*
**    =========================================================================
**
**        Abs: driver support for TPG262 Pfeiffer vacuum gauge
**
**        Name: drvTPG262.c
**
**        First Auth:   17-May-2004,  Kukhee Kim         (KHKIM)
**        Second Auth:  dd-mmm-yyyy,  First Lastname     (USERNAME)
**        Rev:          dd-mmm-yyyy,  Reviewer's name    (USERNAME)
**
**    -------------------------------------------------------------------------
**        Mod:
**                      19-May-2004, Kukhee Kim          (KHKIM)
**                                   drvTPG262 v0.0a1 release
**
**    =========================================================================
*/

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <epicsMutex.h>
#include <epicsThread.h>
#include <ellLib.h>
#include <callback.h>
#include <iocsh.h>
#include <taskwd.h>

#include <drvSup.h>
#include <devSup.h>
#include <devLib.h>
#include <dbScan.h>

#include <epicsExport.h>
#include <epicsPrint.h>

#include "drvTPG262.h"


LOCAL ELLLIST * pdrvTPG262_ellList = NULL;



drvTPG262Config*  drvTPG262_find(char *portName);
LOCAL int drvTPG262_findUnit(char* unitName);
LOCAL long drvTPG262_init(char *portName, char *unitName, double devTimtout, double cbTimeout, double scanInterval);
LOCAL void drvTPG262_timeoutCallback(asynUser* pasynTPG262User);
LOCAL void drvTPG262_queueCallback(asynUser* pasynTPG262User);
LOCAL void drvTPG262_read(asynUser* pasynTPG262User);
LOCAL void drvTPG262_setUnit(asynUser* pasynTPG262User);
LOCAL void drvTPG262_getGID(asynUser* pasynTPG262User);
LOCAL void drvTPG262_getProgVer(asynUser* pasynTPG262User);
LOCAL void drvTPG262_scanTask(int param);
LOCAL void drvTPG262_reportPrint(drvTPG262Config* pdrvTPG262Config, int level);


LOCAL long drvTPG262_io_report(int level);
LOCAL long drvTPG262_init_driver(void);


struct {
    long     number;
    DRVSUPFUN report;
    DRVSUPFUN init;
} drvTPG262 = {
    2,
    drvTPG262_io_report,
    drvTPG262_init_driver
};

epicsExportAddress(drvet, drvTPG262);



static const iocshArg drvTPG262_initArg0 = {"port", iocshArgString};
static const iocshArg drvTPG262_initArg1 = {"unit", iocshArgString};
static const iocshArg drvTPG262_initArg2 = {"dev timeout", iocshArgDouble};
static const iocshArg drvTPG262_initArg3 = {"callback timeout", iocshArgDouble};
static const iocshArg drvTPG262_initArg4 = {"scan interval", iocshArgDouble};
static const iocshArg* const drvTPG262_initArgs[5] = { &drvTPG262_initArg0, 
                                                       &drvTPG262_initArg1,
                                                       &drvTPG262_initArg2,
                                                       &drvTPG262_initArg3,
                                                       &drvTPG262_initArg4};
static const iocshFuncDef drvTPG262_initFuncDef = { "drvTPG262_init",
                                                    5,
                                                    drvTPG262_initArgs };
static void drvTPG262_initCallFunc(const iocshArgBuf *args)
{
    char port_name[40], unit_name[40];
    double devTimeout, cbTimeout, scanInterval;
 
    strcpy(port_name, args[0].sval); 
    strcpy(unit_name, args[1].sval);
    devTimeout = args[2].dval;
    cbTimeout = args[3].dval;
    scanInterval = args[4].dval;
    drvTPG262_init(port_name, unit_name, devTimeout, cbTimeout, scanInterval);
}

static void epicsShareAPI drvTPG262Registrar(void)
{
    static int firstTime =1;

    if(firstTime) {
        firstTime = 0;
    
        iocshRegister(&drvTPG262_initFuncDef, drvTPG262_initCallFunc);
    }
}

epicsExportRegistrar(drvTPG262Registrar);


drvTPG262Config* drvTPG262_find(char *portName)
{
    drvTPG262Config* pdrvTPG262Config = NULL;
    char port_name[40];

    strcpy(port_name, portName);

    if(pdrvTPG262_ellList) pdrvTPG262Config = (drvTPG262Config*) ellFirst(pdrvTPG262_ellList);
    if(!pdrvTPG262Config)  return pdrvTPG262Config;

    do {
        if(!strcmp(pdrvTPG262Config->port_name, port_name)) break;
        pdrvTPG262Config = (drvTPG262Config*) ellNext(&pdrvTPG262Config->node);
    } while(pdrvTPG262Config);

    return pdrvTPG262Config; 

}


LOCAL int drvTPG262_findUnit(char* unitName)
{ 
    int  unit = 0;
    char unit_name[40];

    strcpy(unit_name, unitName);

    while(tUnitList[unit]) {
        if(!strcmp(tUnitList[unit], unit_name)) break;
        else unit++;
    }

    return vUnitList[unit];
}


LOCAL long drvTPG262_init(char *portName, char *unitName, double devTimeout, double cbTimeout, double scanInterval)
{
    static int firstTime = 1;
    drvTPG262Config*  pdrvTPG262Config = NULL;
    TPG262_read*      pTPG262_read     = NULL;
    asynUser*         pasynTPG262User  = NULL;
    asynTPG262UserData* pasynTPG262UserData = NULL;
    asynInterface*    pasynTPG262Interface = NULL;
    asynStatus        asynTPG262Status;

    char port_name[40];
    char unit_name[40];

    if(firstTime) {
        firstTime = 0;
        epicsThreadCreate("drvTPG262ScanTask",
                          epicsThreadPriorityLow,
                          epicsThreadGetStackSize(epicsThreadStackSmall),
                          (EPICSTHREADFUNC) drvTPG262_scanTask,
                          0); 
    }

    strcpy(port_name,portName); 
    strcpy(unit_name,unitName);
    
    if(!pdrvTPG262_ellList) { 
        pdrvTPG262_ellList = (ELLLIST *) malloc(sizeof(ELLLIST));
        ellInit(pdrvTPG262_ellList);
    }

    if(drvTPG262_find(port_name)) return 0;       /* driver was intialized alrady, nothing to do */
    
    pdrvTPG262Config = (drvTPG262Config*) malloc(sizeof(drvTPG262Config));
    pTPG262_read     = (TPG262_read*) malloc(sizeof(TPG262_read));
    pasynTPG262UserData = (asynTPG262UserData*) malloc(sizeof(asynTPG262UserData));
    if(!pdrvTPG262Config || !pTPG262_read || !pasynTPG262UserData) return 0;

    pdrvTPG262Config->devTimeout = devTimeout;
    pdrvTPG262Config->cbTimeout = cbTimeout;
    pdrvTPG262Config->scanInterval = scanInterval;


    strcpy(pdrvTPG262Config->port_name, port_name);
    pdrvTPG262Config->cbCount = 0;
    pdrvTPG262Config->timeoutCount = 0;
    pdrvTPG262Config->reconnCount = 0;
    pTPG262_read->unit = drvTPG262_findUnit(unit_name);
    if(pTPG262_read->unit<0) pTPG262_read->unit = 1;      /* default unit */
    pTPG262_read->unit_str = tUnitList[vUnitList[pTPG262_read->unit]];
    pTPG262_read->gst0 = TPG262GST_TIMEOUT; 
    pTPG262_read->gst1 = TPG262GST_TIMEOUT;
    pTPG262_read->prs0 = 0.;
    pTPG262_read->prs1 = 0.;
    pTPG262_read->gst_str0 = tSenStList[vSenStList[pTPG262_read->gst0]];
    pTPG262_read->gst_str1 = tSenStList[vSenStList[pTPG262_read->gst1]];
    strcpy(pTPG262_read->gid_str0, TPG262GID_UNKNOWN_STR);
    strcpy(pTPG262_read->gid_str1, TPG262GID_UNKNOWN_STR);
    strcpy(pTPG262_read->progver_str, TPG262PROGVER_UNKNOWN_STR);

    pasynTPG262UserData->pdrvTPG262Config = (void*) pdrvTPG262Config;
    pasynTPG262User = pasynManager->createAsynUser(drvTPG262_queueCallback, drvTPG262_timeoutCallback);
    pasynTPG262User->timeout = pdrvTPG262Config->devTimeout;
    pasynTPG262User->userPvt = pasynTPG262UserData;
    asynTPG262Status = pasynManager->connectDevice(pasynTPG262User, port_name, 0);
    if(asynTPG262Status != asynSuccess) {
        goto end;
    }
    pasynTPG262Interface = pasynManager->findInterface(pasynTPG262User, asynOctetType, 1);
    if(!pasynTPG262Interface){
         goto end;
    }
    pasynTPG262UserData->pasynTPG262Octet = (asynOctet*)pasynTPG262Interface->pinterface;
    pasynTPG262UserData->pdrvPvt = pasynTPG262Interface->drvPvt; 
    pdrvTPG262Config->lock = epicsMutexCreate();
    scanIoInit(&pdrvTPG262Config->ioScanPvt);
    pdrvTPG262Config->pTPG262_read = pTPG262_read;
    pdrvTPG262Config->pasynTPG262User = pasynTPG262User;
    pdrvTPG262Config->pasynTPG262UserData = pasynTPG262UserData;
    pdrvTPG262Config->status = TPG262STATUS_TIMEOUT_MASK |
                               TPG262STATUS_SETUNIT_MASK |
                               TPG262STATUS_GETGID_MASK |
                               TPG262STATUS_GETPROGVER_MASK;
    ellAdd(pdrvTPG262_ellList, &pdrvTPG262Config->node);
   
    return 0;

    end:
        /* relase allocated memory and asynInterface */
    return 0;
}


LOCAL void drvTPG262_timeoutCallback(asynUser* pasynTPG262User)
{
    asynTPG262UserData* pasynTPG262UserData = (asynTPG262UserData*) pasynTPG262User->userPvt;
    drvTPG262Config* pdrvTPG262Config = (drvTPG262Config*) pasynTPG262UserData->pdrvTPG262Config;
    TPG262_read* pTPG262_read = (TPG262_read*) pdrvTPG262Config->pTPG262_read;

    pdrvTPG262Config->status |= TPG262STATUS_SETUNIT_MASK;
    pdrvTPG262Config->status |= TPG262STATUS_TIMEOUT_MASK;
    pdrvTPG262Config->status |= TPG262STATUS_GETGID_MASK;
    pdrvTPG262Config->status |= TPG262STATUS_GETPROGVER_MASK;

    epicsMutexLock(pdrvTPG262Config->lock);
    pdrvTPG262Config->cbCount =0;
    pdrvTPG262Config->timeoutCount++;
    pTPG262_read->gst0 = TPG262GST_TIMEOUT; 
    pTPG262_read->gst1 = TPG262GST_TIMEOUT;
    pTPG262_read->gst_str0 = tSenStList[vSenStList[pTPG262_read->gst0]];
    pTPG262_read->gst_str1 = tSenStList[vSenStList[pTPG262_read->gst1]];
    strcpy(pTPG262_read->gid_str0, TPG262GID_UNKNOWN_STR);
    strcpy(pTPG262_read->gid_str1, TPG262GID_UNKNOWN_STR);
    strcpy(pTPG262_read->progver_str, TPG262PROGVER_UNKNOWN_STR);
    epicsMutexUnlock(pdrvTPG262Config->lock);

    scanIoRequest(pdrvTPG262Config->ioScanPvt);

   return; 
}


LOCAL void drvTPG262_queueCallback(asynUser* pasynTPG262User)
{

    asynTPG262UserData*  pasynTPG262UserData = (asynTPG262UserData*) pasynTPG262User->userPvt;
    drvTPG262Config* pdrvTPG262Config = (drvTPG262Config*) pasynTPG262UserData->pdrvTPG262Config;

    pdrvTPG262Config->cbCount--;

    if(pdrvTPG262Config->status & TPG262STATUS_SETUNIT_MASK) {
        pdrvTPG262Config->status &= ~TPG262STATUS_SETUNIT_MASK;
        drvTPG262_setUnit(pasynTPG262User);
        return;
    }

    if(pdrvTPG262Config->status & TPG262STATUS_GETPROGVER_MASK) {
        pdrvTPG262Config->status &= ~TPG262STATUS_GETPROGVER_MASK;
        drvTPG262_getProgVer(pasynTPG262User);
        return;
    }


    if(pdrvTPG262Config->status & TPG262STATUS_GETGID_MASK) {
        pdrvTPG262Config->status &= ~TPG262STATUS_GETGID_MASK;
        drvTPG262_getGID(pasynTPG262User);
        return;
    }

    drvTPG262_read(pasynTPG262User);
    if(pdrvTPG262Config->status & TPG262STATUS_TIMEOUT_MASK) {
        pdrvTPG262Config->status &= ~TPG262STATUS_TIMEOUT_MASK;
        pdrvTPG262Config->reconnCount++;
    }
    scanIoRequest(pdrvTPG262Config->ioScanPvt);

    return;
}


LOCAL void drvTPG262_read(asynUser* pasynTPG262User)
{

    asynTPG262UserData*  pasynTPG262UserData = (asynTPG262UserData*) pasynTPG262User->userPvt;
    asynOctet*  pasynTPG262Octet     = (asynOctet*) pasynTPG262UserData->pasynTPG262Octet;
    void*  pdrvPvt = (void*) pasynTPG262UserData->pdrvPvt;
    drvTPG262Config* pdrvTPG262Config = (drvTPG262Config*) pasynTPG262UserData->pdrvTPG262Config;
    TPG262_read*  pTPG262_read = (TPG262_read*) pdrvTPG262Config->pTPG262_read;
    char*  msgBuf = pasynTPG262UserData->msgBuf;
    char*  dumBuf = pasynTPG262UserData->dumBuf;
    int i;


    pasynTPG262Octet->setInputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);
    pasynTPG262Octet->setOutputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);

	size_t nbytesTransfered;
	int eomReason;
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, "PRX"EOS_STR, 5, &nbytesTransfered);
    pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, dumBuf,TPG262_DUMBUFSIZE, &nbytesTransfered, &eomReason);
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, ENQ_STR, 1, &nbytesTransfered);
    i=pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, msgBuf, TPG262_MSGBUFSIZE, &nbytesTransfered, &eomReason);

    epicsMutexLock(pdrvTPG262Config->lock);
    if(i>2) {
        msgBuf[i-2] = NULL;
        sscanf(msgBuf, "%d, %le, %d, %le", &pTPG262_read->gst0, &pTPG262_read->prs0, &pTPG262_read->gst1, &pTPG262_read->prs1);
        pTPG262_read->gst_str0 = tSenStList[vSenStList[pTPG262_read->gst0]];
        pTPG262_read->gst_str1 = tSenStList[vSenStList[pTPG262_read->gst1]];
    }
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return;
}


LOCAL void drvTPG262_setUnit(asynUser* pasynTPG262User)
{
    asynTPG262UserData*  pasynTPG262UserData = (asynTPG262UserData*) pasynTPG262User->userPvt;
    asynOctet*  pasynTPG262Octet     = (asynOctet*) pasynTPG262UserData->pasynTPG262Octet;
    void*  pdrvPvt = (void*) pasynTPG262UserData->pdrvPvt;
    drvTPG262Config* pdrvTPG262Config = (drvTPG262Config*) pasynTPG262UserData->pdrvTPG262Config;
    TPG262_read* pTPG262_read = (TPG262_read*) pdrvTPG262Config->pTPG262_read;
    char*  msgBuf = pasynTPG262UserData->msgBuf;
    char*  dumBuf = pasynTPG262UserData->dumBuf;

    sprintf(msgBuf, "UNI, %d"EOS_STR, pTPG262_read->unit);
    pasynTPG262Octet->setInputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);
    pasynTPG262Octet->setOutputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);

	size_t nbytesTransfered;
	int eomReason;
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, msgBuf, strlen(msgBuf), &nbytesTransfered);
    pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, dumBuf,TPG262_DUMBUFSIZE, &nbytesTransfered, &eomReason);
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, ENQ_STR, 1, &nbytesTransfered);
    pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, msgBuf, TPG262_MSGBUFSIZE, &nbytesTransfered, &eomReason);

    epicsMutexLock(pdrvTPG262Config->lock);
    pTPG262_read->unit_str = tUnitList[vUnitList[pTPG262_read->unit]];
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return;
}


LOCAL void drvTPG262_getGID(asynUser* pasynTPG262User)
{
    asynTPG262UserData* pasynTPG262UserData = (asynTPG262UserData*) pasynTPG262User->userPvt;
    asynOctet* pasynTPG262Octet = (asynOctet*) pasynTPG262UserData->pasynTPG262Octet;
    void* pdrvPvt = (void*) pasynTPG262UserData->pdrvPvt;
    drvTPG262Config* pdrvTPG262Config = (drvTPG262Config*) pasynTPG262UserData->pdrvTPG262Config;
    TPG262_read* pTPG262_read = (TPG262_read*) pdrvTPG262Config->pTPG262_read;
    char* msgBuf = pasynTPG262UserData->msgBuf;
    char* dumBuf = pasynTPG262UserData->dumBuf;
    int i;

    strcpy(msgBuf, "TID"EOS_STR);
    pasynTPG262Octet->setInputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);
    pasynTPG262Octet->setOutputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);
    
	size_t nbytesTransfered;
	int eomReason;
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, msgBuf, strlen(msgBuf), &nbytesTransfered);
    pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, dumBuf,TPG262_DUMBUFSIZE, &nbytesTransfered, &eomReason);
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, ENQ_STR, 1, &nbytesTransfered);
    i = pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, msgBuf, TPG262_MSGBUFSIZE, &nbytesTransfered, &eomReason);

    epicsMutexLock(pdrvTPG262Config->lock);
    if(i>2) {
        msgBuf[i-2] = NULL; 
        for(i=1;i<strlen(msgBuf);i++){
            if(msgBuf[i]==',') msgBuf[i]=' ';
        } 
        sscanf(msgBuf,"%s %s", pTPG262_read->gid_str0, pTPG262_read->gid_str1);
    }
    epicsMutexUnlock(pdrvTPG262Config->lock);

   

    return;
}


LOCAL void drvTPG262_getProgVer(asynUser* pasynTPG262User)
{
    asynTPG262UserData* pasynTPG262UserData = (asynTPG262UserData*) pasynTPG262User->userPvt;
    asynOctet* pasynTPG262Octet = (asynOctet*) pasynTPG262UserData->pasynTPG262Octet;
    void* pdrvPvt = (void*) pasynTPG262UserData->pdrvPvt;
    drvTPG262Config* pdrvTPG262Config = (drvTPG262Config*) pasynTPG262UserData->pdrvTPG262Config;
    TPG262_read* pTPG262_read = (TPG262_read*) pdrvTPG262Config->pTPG262_read;
    char* msgBuf = pasynTPG262UserData->msgBuf;
    char* dumBuf = pasynTPG262UserData->dumBuf;
    int i;

    strcpy(msgBuf, "PNR"EOS_STR);
    pasynTPG262Octet->setInputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);
    pasynTPG262Octet->setOutputEos(pdrvPvt, pasynTPG262User, EOS_STR, 2);

	size_t nbytesTransfered;
	int eomReason;
    epicsMutexLock(pdrvTPG262Config->lock);
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, msgBuf, strlen(msgBuf), &nbytesTransfered);
    pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, dumBuf,TPG262_DUMBUFSIZE, &nbytesTransfered, &eomReason);
    pasynTPG262Octet->write(pdrvPvt, pasynTPG262User, ENQ_STR, 1, &nbytesTransfered);
    i = pasynTPG262Octet->read(pdrvPvt, pasynTPG262User, msgBuf, TPG262_MSGBUFSIZE, &nbytesTransfered, &eomReason);
    if(i>2) {
        msgBuf[i-2] = NULL;
        strcpy(pTPG262_read->progver_str, msgBuf);
    }
    epicsMutexUnlock(pdrvTPG262Config->lock);

    return;
}



LOCAL void drvTPG262_scanTask(int param)
{
    drvTPG262Config* pdrvTPG262Config = NULL;
    double drvTPG262_scanInterval;

    while(!pdrvTPG262_ellList || ellCount(pdrvTPG262_ellList) <1) {
        epicsThreadSleep(1.);
    }

    while(1) {
        pdrvTPG262Config = (drvTPG262Config*) ellFirst(pdrvTPG262_ellList);
        drvTPG262_scanInterval = pdrvTPG262Config->scanInterval;

        do {
            if(drvTPG262_scanInterval > pdrvTPG262Config->scanInterval)
                drvTPG262_scanInterval = pdrvTPG262Config->scanInterval;
            if(!pdrvTPG262Config->cbCount) {
                pdrvTPG262Config->cbCount++;
                pasynManager->queueRequest(pdrvTPG262Config->pasynTPG262User, asynQueuePriorityLow, pdrvTPG262Config->cbTimeout);
            }
            pdrvTPG262Config = (drvTPG262Config*) ellNext(&pdrvTPG262Config->node);
        } while(pdrvTPG262Config);
        epicsThreadSleep(drvTPG262_scanInterval);
    }

}


LOCAL void drvTPG262_reportPrint(drvTPG262Config* pdrvTPG262Config, int level)
{
    TPG262_read* pTPG262_read = (TPG262_read*)pdrvTPG262Config->pTPG262_read;
    
    epicsPrintf("port: %s TPG262 %s\n",
                pdrvTPG262Config->port_name, pTPG262_read->progver_str);
    if(level<1) return;
    epicsPrintf("      gauge0: %10s %10s, %8.3e %s\n"
                "      gauge1: %10s %10s, %8.3e %s\n",
                pTPG262_read->gid_str0, pTPG262_read->gst_str0, pTPG262_read->prs0, pTPG262_read->unit_str,
                pTPG262_read->gid_str1, pTPG262_read->gst_str1, pTPG262_read->prs1, pTPG262_read->unit_str);
    if(level<2) return;
    epicsPrintf("      timeout counter: %d     reconnection counter: %d\n",
                pdrvTPG262Config->timeoutCount, pdrvTPG262Config->reconnCount);

    return;
     
}


LOCAL long drvTPG262_io_report(int level)
{
    drvTPG262Config* pdrvTPG262Config = NULL;

    if(!pdrvTPG262_ellList) return 0;

    epicsPrintf("Total %d module(s) found\n", ellCount(pdrvTPG262_ellList));
    if(ellCount(pdrvTPG262_ellList)<1) return 0;

    pdrvTPG262Config = (drvTPG262Config*) ellFirst(pdrvTPG262_ellList);
    do {
        drvTPG262_reportPrint(pdrvTPG262Config, level);
        pdrvTPG262Config = (drvTPG262Config*) ellNext(&pdrvTPG262Config->node);
    } while(pdrvTPG262Config);
    
    return 0;
}


LOCAL long drvTPG262_init_driver(void)
{
    return 0;
}
