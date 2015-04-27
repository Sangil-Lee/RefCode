#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <errno.h>

#include <aiRecord.h>
#include <alarm.h>
#include <dbAccess.h>
#include <dbDefs.h>
#include <dbLock.h>
#include <dbAddr.h>
#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <epicsExport.h>
#include <link.h>
#include <registryFunction.h>
#include <recSup.h>
#include <recGbl.h>
#include <subRecord.h>
#include "userclass.h"

IfDriver	*pifdriver = new IfDriver();

/*subroutine record*/
static long subIfNetworkInit(subRecord *precord)
{
	    return 0;
}

static long subIfNetworkCalc(subRecord *precord)
{
#if 1
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);
	DBADDR *pdbAddrC = dbGetPdbAddrFromLink(&precord->inpc);
	DBADDR *pdbAddrD = dbGetPdbAddrFromLink(&precord->inpd);
	//double *pfieldLinkA =  (double*)&pdbAddrA->pfield;
	//double *pfieldLinkB =  (double*)&pdbAddrB->pfield;
	if(!pdbAddrA || !pdbAddrB || !pdbAddrC || !pdbAddrD) return (1);

	aiRecord *pairecordLinkA = (aiRecord *)pdbAddrA->precord;
	aiRecord *pairecordLinkB = (aiRecord *)pdbAddrB->precord;
	aiRecord *pairecordLinkC = (aiRecord *)pdbAddrC->precord;
	aiRecord *pairecordLinkD = (aiRecord *)pdbAddrD->precord;

#if 0
	unsigned char pact = precord->pact;
	if(precord->udf == FALSE) 
	{
		recGblSetSevr(precord, UDF_ALARM, INVALID_ALARM);
		return 1;
	};
#endif

#if 1
	pifdriver->IfGetstats();
	//pifdriver->PrintByte();
	//printf("OutKB:%fKB, InKB:%fKB  \n", pifdriver->DiffKByteOut(5), pifdriver->DiffKByteIn(5));
		//pairecordLinkA->val = pifdriver->DiffKByteIn(5);
		//pairecordLinkB->val = pifdriver->DiffKByteOut(5);
		//pifdriver->DiffKByteCalc(pairecordLinkA->val,pairecordLinkB->val,5);
	if(pifdriver->IsEth1() == 0)
		pifdriver->DiffKByteCalc(pairecordLinkA->val,pairecordLinkB->val,precord->scan);
	else
		pifdriver->DiffKByteCalc(pairecordLinkA->val,pairecordLinkB->val,
						pairecordLinkC->val,pairecordLinkD->val, precord->scan);
#endif
#endif
	dbProcess((dbCommon*)pairecordLinkA);
	dbProcess((dbCommon*)pairecordLinkB);
	dbProcess((dbCommon*)pairecordLinkC);
	dbProcess((dbCommon*)pairecordLinkD);
	return (0);
}

epicsRegisterFunction(subIfNetworkInit);
epicsRegisterFunction(subIfNetworkCalc);
