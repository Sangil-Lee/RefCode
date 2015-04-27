/*
 * ==============================================================================
 *   Name: drvMK80S.c
 *   Desc: Driver support for MASTER-K 80S PLC
 * ------------------------------------------------------------------------------
 *   2012/06/29  yunsw      Initial revision (based on pfeffierTPG262Lib)
 * ==============================================================================
 */

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

#include "drvMK80S.h"

drvMK80SInputList	*pdrvMK80SBitInputList	= NULL;
drvMK80SInputList	*pdrvMK80SWordInputList	= NULL;
static ELLLIST		*pdrvMK80S_ellList		= NULL;

drvMK80SConfig*	drvMK80S_get(int link);
static drvMK80SConfig* drvMK80S_find(char *portName);
static long drvMK80S_init(char *portName, double devTimtout, double cbTimeout, double scanInterval);
static void drvMK80S_timeoutCallback(asynUser* pasynMK80SUser);
static void drvMK80S_queueCallback(asynUser* pasynMK80SUser);
static int drvMK80S_recv(asynUser* pasynMK80SUser);
static void drvMK80S_scanTask(int param);
static void drvMK80S_reportPrint(drvMK80SConfig* pdrvMK80SConfig, int level);

static long drvMK80S_io_report(int level);
static long drvMK80S_init_driver(void);

struct {
    long     number;
    DRVSUPFUN report;
    DRVSUPFUN init;
} drvMK80S = {
    2,
    drvMK80S_io_report,
    drvMK80S_init_driver
};

epicsExportAddress(drvet, drvMK80S);

static const iocshArg drvMK80S_initArg0 = {"port", iocshArgString};
static const iocshArg drvMK80S_initArg1 = {"dev timeout", iocshArgDouble};
static const iocshArg drvMK80S_initArg2 = {"callback timeout", iocshArgDouble};
static const iocshArg drvMK80S_initArg3 = {"scan interval", iocshArgDouble};
static const iocshArg* const drvMK80S_initArgs[4] = { &drvMK80S_initArg0, 
                                                       &drvMK80S_initArg1,
                                                       &drvMK80S_initArg2,
                                                       &drvMK80S_initArg3};
static const iocshFuncDef drvMK80S_initFuncDef = { "drvMK80S_init",
                                                    4,
                                                    drvMK80S_initArgs };

static int drvMK80S_getValue (char *msg, int *val) 
{
	if (!msg || !val) {
		return 0;
	}

	char dataLenBuf[20];
	char dataBuf[20];
	int nDataLen;
	int nOffset;

	memcpy (dataLenBuf, msg, 2);
	dataLenBuf[2] = 0x00;
	nOffset = 2;

	nDataLen = atoi(dataLenBuf) * 2;

	memcpy (dataBuf, msg + nOffset, nDataLen);
	dataBuf[nDataLen] = 0x00;
	nOffset += nDataLen;

	sscanf (dataBuf, "%x", val);

	kuDebug (kuDEBUG, "[drvMK80S_getValue] msg(%s) len(%d) val(%s/%d)\n", msg, nDataLen, dataBuf, *val);

	return (nOffset);
}

static void drvMK80S_initCallFunc(const iocshArgBuf *args)
{
    char port_name[40];
    double devTimeout, cbTimeout, scanInterval;
 
    strcpy(port_name, args[0].sval); 
    devTimeout = args[1].dval;
    cbTimeout = args[2].dval;
    scanInterval = args[3].dval;

    drvMK80S_init(port_name, devTimeout, cbTimeout, scanInterval);
}

static void epicsShareAPI drvMK80SRegistrar(void)
{
    static int firstTime =1;

    if(firstTime) {
        firstTime = 0;
        iocshRegister(&drvMK80S_initFuncDef, drvMK80S_initCallFunc);
    }
}

epicsExportRegistrar(drvMK80SRegistrar);

drvMK80SConfig* drvMK80S_get(int link)
{
	char portName[40];
	sprintf (portName, "L%d", link);

	return (drvMK80S_find(portName));
}

static drvMK80SConfig* drvMK80S_find(char *portName)
{
    drvMK80SConfig* pdrvMK80SConfig = NULL;
    char port_name[40];

    strcpy(port_name, portName);

    if(pdrvMK80S_ellList) pdrvMK80SConfig = (drvMK80SConfig*) ellFirst(pdrvMK80S_ellList);
    if(!pdrvMK80SConfig)  return pdrvMK80SConfig;

    do {
        if(!strcmp(pdrvMK80SConfig->port_name, port_name)) break;
        pdrvMK80SConfig = (drvMK80SConfig*) ellNext(&pdrvMK80SConfig->node);
    } while(pdrvMK80SConfig);

    return pdrvMK80SConfig; 
}

static long drvMK80S_init(char *portName, double devTimeout, double cbTimeout, double scanInterval)
{
    static int firstTime = 1;
    char port_name[40];

    drvMK80SConfig		*pdrvMK80SConfig	= NULL;
    MK80S_read			*pMK80S_read		= NULL;
    asynUser			*pasynMK80SUser		= NULL;
    asynMK80SUserData	*pasynMK80SUserData	= NULL;
    asynInterface		*pasynMK80SInterface= NULL;
    asynStatus			asynMK80SStatus;

    if(firstTime) {
        firstTime = 0;
        epicsThreadCreate("drvMK80SScanTask",
                          epicsThreadPriorityLow,
                          epicsThreadGetStackSize(epicsThreadStackSmall),
                          (EPICSTHREADFUNC) drvMK80S_scanTask,
                          0); 
    }

    strcpy(port_name,portName); 
    
    if(!pdrvMK80S_ellList) { 
        pdrvMK80S_ellList = (ELLLIST *) malloc(sizeof(ELLLIST));
        ellInit(pdrvMK80S_ellList);
    }

    if(!pdrvMK80SBitInputList) { 
        pdrvMK80SBitInputList = (drvMK80SInputList *) calloc(1, sizeof(drvMK80SInputList));
		pdrvMK80SBitInputList->num = 0;
    }

    if(!pdrvMK80SWordInputList) { 
        pdrvMK80SWordInputList = (drvMK80SInputList *) calloc(1, sizeof(drvMK80SInputList));
		pdrvMK80SWordInputList->num = 0;
    }

    if(drvMK80S_find(port_name)) return 0;       /* driver was intialized alrady, nothing to do */
    
    pdrvMK80SConfig = (drvMK80SConfig*) malloc(sizeof(drvMK80SConfig));
    pMK80S_read     = (MK80S_read*) malloc(sizeof(MK80S_read));
    pasynMK80SUserData = (asynMK80SUserData*) malloc(sizeof(asynMK80SUserData));
    if(!pdrvMK80SConfig || !pMK80S_read || !pasynMK80SUserData) return 0;

    pdrvMK80SConfig->devTimeout = devTimeout;
    pdrvMK80SConfig->cbTimeout = cbTimeout;
    pdrvMK80SConfig->scanInterval = scanInterval;

    strcpy(pdrvMK80SConfig->port_name, port_name);
    pdrvMK80SConfig->cbCount = 0;
    pdrvMK80SConfig->timeoutCount = 0;

	memset (pMK80S_read, 0x00, sizeof(MK80S_read));

    pasynMK80SUserData->pdrvMK80SConfig = (void*) pdrvMK80SConfig;
    pasynMK80SUser = pasynManager->createAsynUser(drvMK80S_queueCallback, drvMK80S_timeoutCallback);
    pasynMK80SUser->timeout = pdrvMK80SConfig->devTimeout;
    pasynMK80SUser->userPvt = pasynMK80SUserData;

    asynMK80SStatus = pasynManager->connectDevice(pasynMK80SUser, port_name, 0);
    if(asynMK80SStatus != asynSuccess) {
        goto end;
    }

    pasynMK80SInterface = pasynManager->findInterface(pasynMK80SUser, asynOctetType, 1);
    if(!pasynMK80SInterface){
         goto end;
    }

    pasynMK80SUserData->pasynMK80SOctet = (asynOctet*)pasynMK80SInterface->pinterface;

	// set EOS(end of string) of receive message
	pasynMK80SUserData->pasynMK80SOctet->setInputEos(pasynMK80SInterface->drvPvt, pasynMK80SUser, ETX_STR, 1);

    pasynMK80SUserData->pdrvPvt = pasynMK80SInterface->drvPvt; 
    pdrvMK80SConfig->lock = epicsMutexCreate();

    scanIoInit(&pdrvMK80SConfig->ioScanPvt);

    pdrvMK80SConfig->pMK80S_read = pMK80S_read;
    pdrvMK80SConfig->pasynMK80SUser = pasynMK80SUser;
    pdrvMK80SConfig->pasynMK80SUserData = pasynMK80SUserData;
    pdrvMK80SConfig->status = MK80SSTATUS_TIMEOUT_MASK;

    ellAdd(pdrvMK80S_ellList, &pdrvMK80SConfig->node);
   
    return 0;

end:
	/* relase allocated memory and asynInterface */

	kuDebug (kuERR, "[drvMK80S_init] init failed for %s!!!\n", port_name);

    return 0;
}

static void drvMK80S_timeoutCallback(asynUser* pasynMK80SUser)
{
	kuDebug (kuTRACE, "[drvMK80S_timeoutCallback] timeout \n");

    asynMK80SUserData* pasynMK80SUserData = (asynMK80SUserData*) pasynMK80SUser->userPvt;
    drvMK80SConfig* pdrvMK80SConfig = (drvMK80SConfig*) pasynMK80SUserData->pdrvMK80SConfig;

    pdrvMK80SConfig->status |= MK80SSTATUS_TIMEOUT_MASK;

    epicsMutexLock(pdrvMK80SConfig->lock);
    pdrvMK80SConfig->cbCount =0;
    pdrvMK80SConfig->timeoutCount++;
    epicsMutexUnlock(pdrvMK80SConfig->lock);

    scanIoRequest(pdrvMK80SConfig->ioScanPvt);

   return; 
}

static void drvMK80S_queueCallback(asynUser* pasynMK80SUser)
{
	kuDebug (kuTRACE, "[drvMK80S_queueCallback] ... \n");

    asynMK80SUserData*  pasynMK80SUserData = (asynMK80SUserData*) pasynMK80SUser->userPvt;
    drvMK80SConfig* pdrvMK80SConfig = (drvMK80SConfig*) pasynMK80SUserData->pdrvMK80SConfig;

    pdrvMK80SConfig->cbCount--;

    drvMK80S_recv(pasynMK80SUser);

    scanIoRequest(pdrvMK80SConfig->ioScanPvt);

    return;
}

static int drvMK80S_recv_multi (asynUser* pasynMK80SUser, drvMK80SInputList *pdrvMK80SInputList, int *nValues)
{
	kuDebug (kuTRACE, "[drvMK80S_recv] ... \n");

    asynMK80SUserData	*pasynMK80SUserData	= (asynMK80SUserData*) pasynMK80SUser->userPvt;
    asynOctet			*pasynMK80SOctet	= (asynOctet*) pasynMK80SUserData->pasynMK80SOctet;
    void				*pdrvPvt			= (void*) pasynMK80SUserData->pdrvPvt;
    drvMK80SConfig		*pdrvMK80SConfig	= (drvMK80SConfig*) pasynMK80SUserData->pdrvMK80SConfig;

    //int		i, nread, val, offset;
    int		i, val, offset;
    char	msgBuf[MK80S_MSGBUFSIZE], resBuf[MK80S_MSGBUFSIZE];

	memset (msgBuf, 0x00, sizeof(msgBuf));
	memset (resBuf, 0x00, sizeof(resBuf));

	// ----------------------------------------------------------
	// makes frame data
	// ----------------------------------------------------------
	//  ENQ / Station / CMD / Type / Blk # / (Len + Name)s / EOT
	// (H05)   (H01)    (R)   (SS)   (H01)  (H07 + %MW1000) (H04)
	// ----------------------------------------------------------

	offset = 0;

	// Header : ENQ
	msgBuf[offset++] = ENQ_CHR;

	// Station / Command / Type of command / Number of blocks
	sprintf (msgBuf + offset, "01RSS%02d", pdrvMK80SInputList->num);
	offset += 7;

	for (i = 0; i < pdrvMK80SInputList->num; i++) {
		// Size of variable (address) / Name of variable (MW0103 or DW0600)
		sprintf (msgBuf + offset, "07%%%s", pdrvMK80SInputList->addr[i]);
		offset += 9;
	}

	// Tail : EOT
	msgBuf[offset++] = EOT_CHR;

	epicsMutexLock(pdrvMK80SConfig->lock);

	kuDebug (kuINFO, "[drvMK80S_recv] REQ : size(%2d) req(%s)\n", strlen(msgBuf), msgBuf);

	size_t nread, nwrite;
	int eomReason;
	asynStatus status;

	// writes request frame data to PLC
	//pasynMK80SOctet->write (pdrvPvt, pasynMK80SUser, msgBuf, strlen(msgBuf));
	pasynOctetSyncIO->write (pasynMK80SUser, msgBuf, strlen(msgBuf), 1.0, &nwrite);

	// reads response from PLC
	//nread = pasynMK80SOctet->read (pdrvPvt, pasynMK80SUser, resBuf, sizeof(resBuf));
	status = pasynOctetSyncIO->read (pasynMK80SUser, resBuf, sizeof(resBuf), 5.0, &nread, &eomReason);

	epicsMutexUnlock(pdrvMK80SConfig->lock);

	if (nread > 0) {
		if (ACK_CHR == resBuf[0]) {		// ACK Response
			kuDebug (kuINFO, "[drvMK80S_recv] ACK : size(%2d) res(%s)\n", nread, resBuf);

			for (i = 0, offset = 8; i < pdrvMK80SInputList->num; i++) {
				offset += drvMK80S_getValue (resBuf + offset, &val);
				nValues[pdrvMK80SInputList->idx[i]] = val;

				kuDebug (kuDEBUG, "[drvMK80S_recv] idx(%02d) addr(%s) val(%d)\n",
						pdrvMK80SInputList->idx[i], pdrvMK80SInputList->addr[i], val);
			}
		}
		else if (NAK_CHR == resBuf[0]) {	// NAK Response
			kuDebug (kuINFO, "[drvMK80S_recv] NAK : req(%s) res(%s)\n", msgBuf, resBuf);
			return (kuNOK);
		}
	}
	else {	// Error
		kuDebug (kuERR, "[drvMK80S_recv] ERR : req(%s) ret(%d)\n", msgBuf, nread);
		return (kuNOK);
	}

    return (kuOK);
}

static int drvMK80S_recv (asynUser* pasynMK80SUser)
{
    asynMK80SUserData	*pasynMK80SUserData	= (asynMK80SUserData*) pasynMK80SUser->userPvt;
    drvMK80SConfig		*pdrvMK80SConfig	= (drvMK80SConfig*) pasynMK80SUserData->pdrvMK80SConfig;
    MK80S_read			*pMK80S_read		= (MK80S_read*) pdrvMK80SConfig->pMK80S_read;

	// Bits
	if (kuOK != drvMK80S_recv_multi (pasynMK80SUser, pdrvMK80SBitInputList, pMK80S_read->nBitValues)) {
		return (kuNOK);
	}

	// Words
	if (kuOK != drvMK80S_recv_multi (pasynMK80SUser, pdrvMK80SWordInputList, pMK80S_read->nWordValues)) {
		return (kuNOK);
	}

	// PLC communication is ok
	pdrvMK80SConfig->status = 0;

    return (kuOK);
}

int drvMK80S_send (int link, char *addr, int val)
{
	kuDebug (kuTRACE, "[drvMK80S_send] addr(%s) val(%d)\n", addr, val);

    drvMK80SConfig		*pdrvMK80SConfig	= drvMK80S_get (link);
    asynMK80SUserData	*pasynMK80SUserData	= (asynMK80SUserData*) pdrvMK80SConfig->pasynMK80SUserData;
    asynUser			*pasynMK80SUser		= (asynUser*) pdrvMK80SConfig->pasynMK80SUser;
    asynOctet			*pasynMK80SOctet	= (asynOctet*) pasynMK80SUserData->pasynMK80SOctet;
    void				*pdrvPvt			= (void*) pasynMK80SUserData->pdrvPvt;

    //int		nread;
    char	msgBuf[MK80S_MSGBUFSIZE], resBuf[MK80S_MSGBUFSIZE];

	memset (msgBuf, 0x00, sizeof(msgBuf));
	memset (resBuf, 0x00, sizeof(resBuf));

	if (addr[1] == 'X') {	// Bit : ex) MX0010
		sprintf (msgBuf, "%c01WSS0107%%%s%02x%c", ENQ_CHR, addr, val, EOT_CHR);
	}
	else {	// Word : ex) DW0600
		sprintf (msgBuf, "%c01WSS0107%%%s%04x%c", ENQ_CHR, addr, val, EOT_CHR);
	}

	epicsMutexLock(pdrvMK80SConfig->lock);

	kuDebug (kuMON, "[drvMK80S_send] REQ : size(%2d) req(%s)\n", strlen(msgBuf), msgBuf);

	size_t nread, nwrite;
	int eomReason;
	asynStatus status;

	pasynOctetSyncIO->write (pasynMK80SUser, msgBuf, strlen(msgBuf), 1.0, &nwrite);

	status = pasynOctetSyncIO->read (pasynMK80SUser, resBuf, sizeof(resBuf), 5.0, &nread, &eomReason);

	epicsMutexUnlock(pdrvMK80SConfig->lock);

	if (nread > 0) {
		if (ACK_CHR == resBuf[0]) {		// ACK Response
			kuDebug (kuMON, "[drvMK80S_send] ACK : size(%2d) res(%s)\n", nread, resBuf);
		}
		else if (NAK_CHR == resBuf[0]) {	// NAK Response
			kuDebug (kuERR, "[drvMK80S_send] NAK : req(%s) res(%s)\n", msgBuf, resBuf);
			return (kuNOK);
		}
	}
	else {	// Error
		kuDebug (kuERR, "[drvMK80S_send] ERR : req(%s) ret(%d)\n", msgBuf, nread);
		return (kuNOK);
	}

    return (kuOK);
}

static void drvMK80S_scanTask(int param)
{
	kuDebug (kuTRACE, "[drvMK80S_scanTask] ... \n");

    drvMK80SConfig* pdrvMK80SConfig = NULL;
    double drvMK80S_scanInterval;

    while(!pdrvMK80S_ellList || ellCount(pdrvMK80S_ellList) <1) {
        epicsThreadSleep(1.);
    }

    while(1) {
        pdrvMK80SConfig = (drvMK80SConfig*) ellFirst(pdrvMK80S_ellList);
        drvMK80S_scanInterval = pdrvMK80SConfig->scanInterval;

        do {
            if(drvMK80S_scanInterval > pdrvMK80SConfig->scanInterval) {
                drvMK80S_scanInterval = pdrvMK80SConfig->scanInterval;
			}

            if(!pdrvMK80SConfig->cbCount) {
                pdrvMK80SConfig->cbCount++;

				kuDebug (kuDEBUG, "[drvMK80S_scanTask] pasynManager->queueRequest() \n");
                pasynManager->queueRequest(pdrvMK80SConfig->pasynMK80SUser, asynQueuePriorityLow, pdrvMK80SConfig->cbTimeout);
            }

            pdrvMK80SConfig = (drvMK80SConfig*) ellNext(&pdrvMK80SConfig->node);
        } while(pdrvMK80SConfig);

        epicsThreadSleep(drvMK80S_scanInterval);
    }

	kuDebug (kuCRI, "[drvMK80S_scanTask] end ...\n");
}

static void drvMK80S_reportPrint(drvMK80SConfig* pdrvMK80SConfig, int level)
{
    return;
}

static long drvMK80S_io_report(int level)
{
    drvMK80SConfig* pdrvMK80SConfig = NULL;

    if(!pdrvMK80S_ellList) return 0;

    kuDebug (kuINFO, "Total %d module(s) found\n", ellCount(pdrvMK80S_ellList));

    if(ellCount(pdrvMK80S_ellList)<1) return 0;

    pdrvMK80SConfig = (drvMK80SConfig*) ellFirst(pdrvMK80S_ellList);
    do {
        drvMK80S_reportPrint(pdrvMK80SConfig, level);
        pdrvMK80SConfig = (drvMK80SConfig*) ellNext(&pdrvMK80SConfig->node);
    } while(pdrvMK80SConfig);
    
    return 0;
}

static long drvMK80S_init_driver(void)
{
    return 0;
}

