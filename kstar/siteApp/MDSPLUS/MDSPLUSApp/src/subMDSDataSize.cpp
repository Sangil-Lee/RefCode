#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <unistd.h>

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

/*subroutine record*/
static long subMDSDataSizeInit(subRecord *precord)
{
	string sDatapath = "/mdsdata/";
	chdir(sDatapath.c_str());
    return 0;
}
static long subMDSDataSizeCalc(subRecord *precord)
{
	const int GPATH_MAX=64;
	FILE *fp;
	char path[GPATH_MAX];
	char *command = "du -sk Exp_Data_2012 | awk '{print $1}'";
	fp = popen(command, "r");
	if (fp == NULL) {
		fprintf(stderr,"Cannot execute %s\n", command);
		pclose(fp);
	};

	double totalsize = 0;
	while (fgets(path, GPATH_MAX, fp) != NULL)
	{
		totalsize = strtod(path,NULL);
	};

	DBADDR *pdbAddrFlnk = (DBADDR*)dbGetPdbAddrFromLink(&precord->flnk);
	epicsTimeStamp current_TS;
	epicsTimeGetCurrent(&current_TS);
	int epochCount = current_TS.secPastEpoch;

	int hour = (int)(epochCount/3600);
	int min = (int)((epochCount%3600)/60);
	int sec = (int)(epochCount%60);

	static int first = 0;
	static double olddisksize = 0.;
	if(pdbAddrFlnk)
	{
		double *dval = (double*)pdbAddrFlnk->pfield;
		*dval = totalsize;

		aiRecord *paiRecordFlnkRecord = (aiRecord *)pdbAddrFlnk->precord;
		DBADDR *pdbAddrFlnkFlnk = (DBADDR*)dbGetPdbAddrFromLink(&paiRecordFlnkRecord->flnk);
		if( hour == 9 && min == 0 && sec >= 0 && sec < 10)
		{
			double *pCurvalue = (double*)pdbAddrFlnkFlnk->pfield;
			double disksize = *dval;
			if(first == 0)
			{
				*pCurvalue = 0;
			}
			else
			{
				double dsize = disksize-olddisksize; 
				if(dsize < 0) dsize = 0;
				*pCurvalue = dsize;
			};
			olddisksize = disksize;
			first = 1;
		};
	};

	pclose(fp);
	return 2;
}

epicsRegisterFunction(subMDSDataSizeInit);
epicsRegisterFunction(subMDSDataSizeCalc);
