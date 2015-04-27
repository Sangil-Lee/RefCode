#include <stdio.h>
#include <stdlib.h>
#include <dbAccess.h>
#include <link.h>
#include <dbDefs.h>
#include <dbLock.h>
#include <dbAddr.h>
#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <registryFunction.h>
#include <recSup.h>
#include <subRecord.h>
#include <aiRecord.h>
#include <calcRecord.h>
#include <longinRecord.h>
#include <epicsExport.h>

/*subroutine record*/
static long subHealthStateInit(subRecord *precord)
{
        return 0;
}

static long subHealthStateCalc(subRecord *precord)
{
    DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
    DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);
    DBADDR *pdbAddrC = dbGetPdbAddrFromLink(&precord->inpc);
    DBADDR *pdbAddrD = dbGetPdbAddrFromLink(&precord->inpd);
    aiRecord *paiRecordLinkA = (aiRecord *)pdbAddrA->precord;
    aiRecord *paiRecordLinkB = (aiRecord *)pdbAddrB->precord;
    aiRecord *paiRecordLinkC = (aiRecord *)pdbAddrC->precord;
    aiRecord *paiRecordLinkD = (aiRecord *)pdbAddrD->precord;

    if(paiRecordLinkA->val >= paiRecordLinkA->hihi ||
        paiRecordLinkB->val < paiRecordLinkB->lolo ||
        paiRecordLinkC->val != 0 || paiRecordLinkD->val !=0 )
        precord ->val = 1;
    else
        precord ->val = 0;

    return 2;
}
epicsRegisterFunction(subHealthStateInit);
epicsRegisterFunction(subHealthStateCalc);
