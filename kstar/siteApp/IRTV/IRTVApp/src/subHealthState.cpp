#include <stdio.h>
#include <string>
#include <vector>
#include "epicsExport.h"
#include "aiRecord.h"
#include "stringinRecord.h"
#include "alarm.h"
#include "dbAccess.h"
#include "dbDefs.h"
#include "dbAddr.h"
#include "link.h"
#include "registryFunction.h"
#include "recSup.h"
#include "recGbl.h"
#include "subRecord.h"

#include "dbLock.h"
#include "epicsThread.h"
#include "epicsMessageQueue.h"
#include "calcRecord.h"
#include "longinRecord.h"

int HealthDebug;

using namespace std;
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
    DBADDR *pdbAddrE = dbGetPdbAddrFromLink(&precord->inpe);

    aiRecord *paiRecordLinkA = (aiRecord *)pdbAddrA->precord;
    calcRecord *pcalcRecordLinkB = (calcRecord *)pdbAddrB->precord;
    aiRecord *paiRecordLinkC = (aiRecord *)pdbAddrC->precord;
    aiRecord *paiRecordLinkD = (aiRecord *)pdbAddrD->precord;
    aiRecord *paiRecordLinkE = (aiRecord *)pdbAddrE->precord;

    if( paiRecordLinkA->val   >= paiRecordLinkA->hihi ||
		pcalcRecordLinkB->val >= pcalcRecordLinkB->hihi ||
        paiRecordLinkC->val   >= paiRecordLinkC->hihi ||
        paiRecordLinkD->val   >= paiRecordLinkD->hihi ||
        paiRecordLinkE->val != 0 )
        precord ->val = 1;
    else
        precord ->val = 0;

    return 2;
}


/* Register these symbols for use by IOC code: */
extern "C" {
epicsExportAddress(int, HealthDebug);
epicsRegisterFunction(subHealthStateInit);
epicsRegisterFunction(subHealthStateCalc);
};
