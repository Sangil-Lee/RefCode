#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>

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

using std::string;
static long subSvrRoomTempInit(subRecord *precord)
{
	    return 0;
}

static long subSvrRoomTempCalc(subRecord *precord)
{
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);
	DBADDR *pdbAddrC = dbGetPdbAddrFromLink(&precord->inpc);
	DBADDR *pdbAddrD = dbGetPdbAddrFromLink(&precord->inpd);
	DBADDR *pdbAddrE = dbGetPdbAddrFromLink(&precord->inpe);
	DBADDR *pdbAddrF = dbGetPdbAddrFromLink(&precord->inpf);
	DBADDR *pdbAddrG = dbGetPdbAddrFromLink(&precord->inpg);

	calcRecord *pcalcRecordLinkA = (calcRecord *)pdbAddrA->precord;
	calcRecord *pcalcRecordLinkB = (calcRecord *)pdbAddrB->precord;
	calcRecord *pcalcRecordLinkC = (calcRecord *)pdbAddrC->precord;
	calcRecord *pcalcRecordLinkD = (calcRecord *)pdbAddrD->precord;
	calcRecord *pcalcRecordLinkE = (calcRecord *)pdbAddrE->precord;
	calcRecord *pcalcRecordLinkF = (calcRecord *)pdbAddrF->precord;
	calcRecord *pcalcRecordLinkG = (calcRecord *)pdbAddrG->precord;

	if( pcalcRecordLinkA->val >= pcalcRecordLinkA->hihi ||
		pcalcRecordLinkB->val >= pcalcRecordLinkB->hihi ||
		pcalcRecordLinkC->val >= pcalcRecordLinkC->hihi ||
		pcalcRecordLinkD->val >= pcalcRecordLinkD->hihi ||
		pcalcRecordLinkE->val == 1	||
		pcalcRecordLinkF->val == 1  ||
		pcalcRecordLinkG->val == 1 )
		precord ->val = 1;
	else
		precord ->val = 0;
	return 2;
}

epicsRegisterFunction(subSvrRoomTempInit);
epicsRegisterFunction(subSvrRoomTempCalc);
