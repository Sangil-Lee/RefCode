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
#include "winPerfmon.h"

int NetDebug;

using namespace std;


CAWinPerfmon *gpCAWinperfmonin;
CAWinPerfmon *gpCAWinperfmonout;

static long subIfNetworkInit(subRecord *precord)
{
    if (NetDebug)
        printf("Record %s called subIfNetworkInit(%p)\n", precord->name, (void*) precord);

	gpCAWinperfmonin = new CAWinPerfmon();
	gpCAWinperfmonout = new CAWinPerfmon();

    return 0;
}

static long subIfNetworkCalc(subRecord *precord)
{
    if (NetDebug)
	{
		static int count = 0;
		if(count == 0 )
			printf("Record %s called subIfNetworkCalc(%p)\n", precord->name, (void*) precord);
		printf("SubInNetworkCalc: -> %d\n", count++);
	}


	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);

	if(!pdbAddrA || !pdbAddrB) return (1);

	aiRecord *pairecordLinkA = (aiRecord*)pdbAddrA->precord;
	aiRecord *pairecordLinkB = (aiRecord*)pdbAddrB->precord;

	gpCAWinperfmonin->SetTrafficType(CAWinPerfmon::IncomingTraffic);
	pairecordLinkA->val= gpCAWinperfmonin->GetTraffic(0)/(precord->scan*1024);

	gpCAWinperfmonout->SetTrafficType(CAWinPerfmon::OutGoingTraffic);
	pairecordLinkB->val= gpCAWinperfmonout->GetTraffic(0)/(precord->scan*1024);

	//double data = gpCAWinperfmonin->GetTraffic(0);

	if(NetDebug)
		printf("Inpacket[%f], Outpacket[%f]\n",pairecordLinkA->val, pairecordLinkB->val );

	dbProcess( reinterpret_cast<dbCommon*> (pairecordLinkA));
	dbProcess( reinterpret_cast<dbCommon*> (pairecordLinkB));
    return 0;
}
#if 0
aSub -> To implement for ASynchronous Subroutine 
static long process(subRecord *prec)
{
     long status = 0;
     int pact = prec->pact;

     if (!pact) {
         prec->pact = TRUE;
         status = fetch_values(prec);
         prec->pact = FALSE;
     }
     if (status == 0) status = do_sub(prec);
    ...
}
#endif

/* Register these symbols for use by IOC code: */
extern "C" {
epicsExportAddress(int, NetDebug);
epicsRegisterFunction(subIfNetworkInit);
epicsRegisterFunction(subIfNetworkCalc);
};
