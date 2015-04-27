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
#include <epicsExport.h>
#include <waveformRecord.h>
#include <stringinRecord.h>
#include <epicsTimeHelper.h>

static int NextShotDebug = 0;
epicsExportAddress(int, NextShotDebug); /*iocsh export variable*/

using std::string;
/*subroutine record*/
static long subNextShotTimeInit(subRecord *precord)
{
	    return 0;
}

static long subNextShotTimeCalc(subRecord *precord)
{
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	char *pfieldLinkA =  (char*)pdbAddrA->pfield;

	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);
	unsigned short *pfieldLinkB =  (unsigned short*)pdbAddrB->pfield;

	DBADDR *pdbAddrC = dbGetPdbAddrFromLink(&precord->inpc);
	//double *pfieldLinkC =  (double*)pdbAddrC->pfield;
	aiRecord *pairecordLinkC = (aiRecord *)pdbAddrC->precord;


	if( NextShotDebug == 1)
		printf("subNextToShot:%s\n",pfieldLinkA);

	//printf("pfieldLinkB:%d\n", pfieldLinkB[0]);
	
	if(strcmp(pfieldLinkA, "-1") == 0 )
	{
		return 0;
	}
	epicsTimeStamp current_TS;
	epicsTimeGetCurrent(&current_TS);
#if 0
	char strcurDate[12]={0};
	char strcurTime[12]={0};
	epicsTimeToStrftime(strcurDate, 12, "%Y/%m/%d", &current_TS);
	epicsTimeToStrftime(strcurTime, 12, "%H:%M:%S", &current_TS);
#endif

	char strTargetDate[12]={0};
	epicsTimeToStrftime(strTargetDate, 12, "%m/%d/%Y", &current_TS);

	epicsTime targetTime;
	string strTargetTime = string(strTargetDate)+string(" ")+string(pfieldLinkA);
	string2epicsTime(strTargetTime.c_str(), targetTime);

	struct epicsTimeStamp target_TS = (struct epicsTimeStamp)targetTime;

	//printf("TarTime:%s, CurTime:%s %s, TarEpoch:%d, CurEpoch:%d\n", strTargetTime.c_str(), 
	//		strcurDate, strcurTime, target_TS.secPastEpoch, current_TS.secPastEpoch);

	//struct epicsTimeStamp elapse_TS;
	//elapse_TS.secPastEpoch = target_TS.secPastEpoch - current_TS.secPastEpoch;
	int counter;
	double msec = 0.;
	counter = target_TS.secPastEpoch - current_TS.secPastEpoch;
	char strelapseTime[12]={0};

	msec = current_TS.nsec*1.E-9;

	DBADDR *pdbaddr = (DBADDR*)dbGetPdbAddrFromLink(&precord->flnk);
#if 0
	if( counter == 0 )
	{
		//15 miniute set
		target_TS.secPastEpoch += (15*60);
		char strcurTime[12]={0};
		stringinRecord *strINData = (stringinRecord*)pdbAddrA->precord;
		epicsTimeToStrftime(strcurTime, 12, "%H:%M:%S", &target_TS);
		strcpy(pfieldLinkA, strcurTime);
		dbProcess((dbCommon*)strINData);
		printf("%s\n",pfieldLinkA);
	}
#endif
	if( counter < 0 )
	{ 
		//counter = abs(counter);
		counter = 0;
	};

	if( pfieldLinkB[0] == 0 )
		strcpy(strelapseTime, "00:00:00");
	else
		sprintf(strelapseTime, "%02d:%02d:%02d", (int)(counter/3600), (int)((counter%3600)/60), (int)(counter%60));

	if( NextShotDebug == 1)
		printf("ElapsedTime:%s\n", strelapseTime);

	double dcounter = counter+msec;
	if(pdbaddr)
	{
		char *pstr = (char*)pdbaddr->pfield;
		if(strcmp(strelapseTime, "00:00:00") == 0 ) dcounter = 0.;
		strcpy(pstr, strelapseTime);
	};
	pairecordLinkC->val = dcounter;

	dbProcess((dbCommon*)pairecordLinkC);
	return 2;
}

epicsRegisterFunction(subNextShotTimeInit);
epicsRegisterFunction(subNextShotTimeCalc);
