#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
#include <boRecord.h>

/*subroutine record*/
static long subIfAUTOInit(subRecord *precord)
{
	    return 0;
}

static long subIfAUTOCalc(subRecord *precord)
{
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);

	boRecord *pairecordLinkA = (boRecord *)pdbAddrA->precord;
	boRecord *pairecordLinkB = (boRecord *)pdbAddrB->precord;
	printf("HERE!!!!!!!!!!!!!!!!!");
	if(precord->val == 1)
	{
		pairecordLinkB->val = 1;
	dbProcess((dbCommon*)pairecordLinkB);
	}
	else 
	{
		pairecordLinkA->val = 1;
	dbProcess((dbCommon*)pairecordLinkA);

	}	
	return (0);
}

epicsRegisterFunction(subIfAUTOInit);
epicsRegisterFunction(subIfAUTOCalc);
