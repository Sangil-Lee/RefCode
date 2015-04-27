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
static long subCHDataSizeInit(subRecord *precord)
{
	string sDatapath = "/channel/OP_2011/";
	chdir(sDatapath.c_str());
    return 0;
}
#if 0
static double diskSize()
{
	const int GPATH_MAX=64;
	FILE *fp;
	char path[GPATH_MAX];
	char *command = "df -k -P /channel/ | grep -v Filesystem | awk '{print $3}'";
	fp = popen(command, "r");
	if (fp == NULL) {
		fprintf(stderr,"Cannot execute %s\n", command);
		pclose(fp);
	};

	double disksize = 0.;
	while (fgets(path, GPATH_MAX, fp) != NULL)
	{
		disksize = strtod(path, NULL);
	};

	pclose(fp);
	return disksize;
}
#endif
static long subCHDataSizeCalc(subRecord *precord)
{
	DBADDR *pdbAddrA = dbGetPdbAddrFromLink(&precord->inpa);
	DBADDR *pdbAddrB = dbGetPdbAddrFromLink(&precord->inpb);
	DBADDR *pdbAddrC = dbGetPdbAddrFromLink(&precord->inpc);
	DBADDR *pdbAddrD = dbGetPdbAddrFromLink(&precord->inpd);
	DBADDR *pdbAddrE = dbGetPdbAddrFromLink(&precord->inpe);
	DBADDR *pdbAddrF = dbGetPdbAddrFromLink(&precord->inpf);
	DBADDR *pdbAddrG = dbGetPdbAddrFromLink(&precord->inpg);
	DBADDR *pdbAddrH = dbGetPdbAddrFromLink(&precord->inph);
	DBADDR *pdbAddrI = dbGetPdbAddrFromLink(&precord->inpi);
	DBADDR *pdbAddrJ = dbGetPdbAddrFromLink(&precord->inpj);
	DBADDR *pdbAddrK = dbGetPdbAddrFromLink(&precord->inpk);
	DBADDR *pdbAddrL = dbGetPdbAddrFromLink(&precord->inpl);

	aiRecord *paiRecordLinkA = (aiRecord *)pdbAddrA->precord;
	aiRecord *paiRecordLinkB = (aiRecord *)pdbAddrB->precord;
	aiRecord *paiRecordLinkC = (aiRecord *)pdbAddrC->precord;
	aiRecord *paiRecordLinkD = (aiRecord *)pdbAddrD->precord;
	aiRecord *paiRecordLinkE = (aiRecord *)pdbAddrE->precord;
	aiRecord *paiRecordLinkF = (aiRecord *)pdbAddrF->precord;
	aiRecord *paiRecordLinkG = (aiRecord *)pdbAddrG->precord;
	aiRecord *paiRecordLinkH = (aiRecord *)pdbAddrH->precord;
	aiRecord *paiRecordLinkI = (aiRecord *)pdbAddrI->precord;
	aiRecord *paiRecordLinkJ = (aiRecord *)pdbAddrJ->precord;
	aiRecord *paiRecordLinkK = (aiRecord *)pdbAddrK->precord;
	aiRecord *paiRecordLinkL = (aiRecord *)pdbAddrL->precord;

	const int GPATH_MAX=64;
	FILE *fp;
	char path[GPATH_MAX];
	char *command = "du -skc * | awk '{print $1,$2}'";
	fp = popen(command, "r");
	if (fp == NULL) {
		fprintf(stderr,"Cannot execute %s\n", command);
		pclose(fp);
	};

	char *ptok = 0;
	int count = 0;
	int enginecount = 0;
	double datasize = 0;
	double totalsize = 0;
	string sEngine;
	while (fgets(path, GPATH_MAX, fp) != NULL)
	{
		count = 0;
		if(!(ptok = strtok (path," "))) continue;
		while (ptok != NULL)
		{
			//printf("Tok[%d]:%s\n", count, ptok);
			if(count == 0) datasize = strtod(ptok, NULL);
			if(count == 1) sEngine = string(ptok);
			if(!(ptok = strtok (NULL," "))) 
			{
				continue;
			};
			count++;
		};
		//printf("size:%f, Engine:%s\n",datasize,sEngine.c_str());
		if (sEngine.compare(0,3,"CLS")==0){
			paiRecordLinkA->val = datasize;
			//printf("val:%f, datasize:%f\n",paiRecordLinkA->val, datasize);
		}
		else if (sEngine.compare(0,3,"PMS")==0){
			paiRecordLinkB->val= datasize;
		}
		else if (sEngine.compare(0,4,"FUEL")==0){
			paiRecordLinkC->val= datasize;
		}
		else if (sEngine.compare(0,3,"HDS")==0){
			paiRecordLinkD->val= datasize;
		}
		else if (sEngine.compare(0,3,"HRS")==0){
			paiRecordLinkE->val= datasize;
		}
		else if (sEngine.compare(0,4,"ICRF")==0){
			paiRecordLinkF->val= datasize;
		}
		else if (sEngine.compare(0,3,"ICS")==0){
			paiRecordLinkG->val= datasize;
		}
		else if (sEngine.compare(0,3,"MPS")==0){
			paiRecordLinkH->val= datasize;
		}
		else if (sEngine.compare(0,3,"QDS")==0){
			paiRecordLinkI->val= datasize;
		}
		else if (sEngine.compare(0,3,"TMS")==0){
			paiRecordLinkJ->val= datasize;
		}
		else if (sEngine.compare(0,3,"VMS")==0){
			paiRecordLinkK->val= datasize;
		}
		else if (sEngine.compare(0,3,"NB1")==0){
			paiRecordLinkL->val= datasize;
		}
		else if (sEngine.compare(0,5,"total")==0){
			totalsize= datasize;
		}
		enginecount++;
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
#if 0
		*dval = (paiRecordLinkA->val+paiRecordLinkB->val+paiRecordLinkC->val+paiRecordLinkD->val+
			paiRecordLinkE->val+paiRecordLinkF->val+paiRecordLinkG->val+paiRecordLinkH->val+
			paiRecordLinkI->val+paiRecordLinkJ->val+paiRecordLinkK->val+paiRecordLinkL->val);
#else
		*dval = totalsize;
#endif

		aiRecord *paiRecordFlnkRecord = (aiRecord *)pdbAddrFlnk->precord;
		DBADDR *pdbAddrFlnkFlnk = (DBADDR*)dbGetPdbAddrFromLink(&paiRecordFlnkRecord->flnk);
		if( sec >= 0 && sec < 10) //for test
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

	dbProcess((dbCommon*)paiRecordLinkA);
	dbProcess((dbCommon*)paiRecordLinkB);
	dbProcess((dbCommon*)paiRecordLinkC);
	dbProcess((dbCommon*)paiRecordLinkD);
	dbProcess((dbCommon*)paiRecordLinkE);
	dbProcess((dbCommon*)paiRecordLinkF);
	dbProcess((dbCommon*)paiRecordLinkG);
	dbProcess((dbCommon*)paiRecordLinkH);
	dbProcess((dbCommon*)paiRecordLinkI);
	dbProcess((dbCommon*)paiRecordLinkJ);
	dbProcess((dbCommon*)paiRecordLinkK);
	dbProcess((dbCommon*)paiRecordLinkL);

	pclose(fp);
	return 2;
}

epicsRegisterFunction(subCHDataSizeInit);
epicsRegisterFunction(subCHDataSizeCalc);
