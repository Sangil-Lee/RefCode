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
//static int NextShotDebug = 0;
//epicsExportAddress(int, NextShotDebug); /*iocsh export variable*/

/*subroutine record*/
static long subHealthStateInit(subRecord *precord)
{
	printf("SubHeathCalc***\n");
	    return 0;
}

static long subHealthStateCalc(subRecord *precord)
{
	printf("SubHeathCalc***\n");
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);
	DBADDR *pdbAddrC = dbGetPdbAddrFromLink(&precord->inpc);
	DBADDR *pdbAddrD = dbGetPdbAddrFromLink(&precord->inpd);
	DBADDR *pdbAddrE = dbGetPdbAddrFromLink(&precord->inpe);

	calcRecord *pcalcRecordLinkA = (calcRecord *)pdbAddrA->precord;
	aiRecord *paiRecordLinkB = (aiRecord *)pdbAddrB->precord;
	aiRecord *paiRecordLinkC = (aiRecord *)pdbAddrC->precord;
	aiRecord *paiRecordLinkD = (aiRecord *)pdbAddrD->precord;
	aiRecord *paiRecordLinkE = (aiRecord *)pdbAddrE->precord;

	if(pcalcRecordLinkA->val >= pcalcRecordLinkA->hihi ||
		paiRecordLinkB->val >= paiRecordLinkB->hihi ||
		paiRecordLinkC->val >= paiRecordLinkC->hihi ||
		paiRecordLinkD->val >= paiRecordLinkD->hihi ||
		paiRecordLinkE->val != 0 )
		precord ->val = 1;
	else
		precord ->val = 0;
#if 0
	//pairecordLinkA->val = ifdriver.DiffKByteIn(5);
	//pairecordLinkB->val = ifdriver.DiffKByteOut(5);
	//ifdriver.DiffKByteCalc(pairecordLinkA->val,pairecordLinkB->val,5);
	if(ifdriver.IsEth1() == 0)
		ifdriver.DiffKByteCalc(pairecordLinkA->val,pairecordLinkB->val,precord->scan);
	else
		ifdriver.DiffKByteCalc(pairecordLinkA->val,pairecordLinkB->val,
						pairecordLinkC->val,pairecordLinkD->val, precord->scan);
#endif
	return 2;
}

epicsRegisterFunction(subHealthStateInit);
epicsRegisterFunction(subHealthStateCalc);
