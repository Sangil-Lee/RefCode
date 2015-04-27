#if 0
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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
#include <errlog.h>

#if 0
using std::string;
using std::vector;
#endif
const char sysProcList[][32]={"caRepeater", "ntpd", "nimxs"};

typedef struct
{
	int insCount;
	int privateCount;
	subRecord *precord;
} NODE;

enum SYSTEMPROC {
	CAREPEATER  = 0x00000001,
	NTPD        = 0x00000001<<1,
	HTTPD       = 0x00000001<<2,
	JAVA        = 0x00000001<<3,
	NSRD        = 0x00000001<<4,
	MMFSD       = 0x00000001<<5,
	MYSQLD      = 0x00000001<<6,
	MDSIP       = 0x00000001<<7,
	GATEWAY     = 0x00000001<<8,
	NIMXS       = 0x00000001<<9
};

static epicsThreadOnceId	threadOnceFlag = EPICS_THREAD_ONCE_INIT;
static epicsMessageQueueId  queueId;
static epicsThreadId		threadId;

/*vector<string>	sysproclist;*/
/*static int checkProcess(const string processname)*/
int checkProcess(const char* processname)
{
	/*string checkproc = string("pidof ") + processname;*/
	char checkproc[64];
	sprintf(checkproc, "/sbin/pidof %s", processname);

	FILE *fp = NULL;
	fp = popen (checkproc, "r");

	fflush(fp);
	printf("command:%s\n", checkproc);

	int retVal = 0;
	if (fp == NULL)
	{
		fprintf(stderr,"Cannot execute %s\n", checkproc);
		pclose(fp);
		return 0;
	};

	char buf[32];
	while (fgets (buf, sizeof(buf), fp) )
	{
		char *token;
		token = strtok(buf, " ");
		pclose(fp);
		return ((token==NULL)?0:atoi(token));
	};


#if 0
	char * buff = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&buff, &len, fp)) != -1)
	{
		char *token = NULL;
		token = strtok(buff, " ");
		printf("Token:%s\n", token);
		retVal = strtol(token, NULL, 10);
	};
	if(buff) free(buff);
#endif
	pclose(fp);
	return retVal;
};

unsigned int getSysProcessStatus()
{
	unsigned int procstatus = 0;
	int i = 0;
	char strBuff[32];
	for (i=0; i < 3; i++)
	{
		strcpy(strBuff, sysProcList[i]); 
		int check = checkProcess(strBuff);
		if(strcmp(strBuff, "caRepeater") == 0)
		{
			if (check == 0)
				procstatus |= CAREPEATER;
			else
				procstatus &= ~CAREPEATER;
		}
		else if(strcmp(strBuff, "caRepeater") == 0)
		{
			if (check == 0)
				procstatus |= NTPD;
			else
				procstatus &= ~NTPD;
		}
		else if(strcmp(strBuff, "caRepeater") == 0)
		{
			if (check == 0)
				procstatus |= NIMXS;
			else
				procstatus &= ~NIMXS;
		}
		/*printf("%s(%d)\n", strBuff, check);*/
	};
	return procstatus;
}


static int subSysProcDebug = 0;
epicsExportAddress(int, subSysProcDebug);

typedef long (*processMethod)(subRecord *precord);

static EPICSTHREADFUNC calcThread(void *param)
{
	subRecord *precord;
	NODE	*pNode;
	struct rset *prset;
	processMethod process;

	while(epicsMessageQueueReceive(queueId, (void*)&pNode,sizeof(NODE*)))
	{
		precord = pNode->precord;
		prset = (struct rset*)precord->rset;
		if(subSysProcDebug) 
		{
			epicsMessageQueueShow(queueId, 1);
			epicsPrintf("Thread %s: Record %s requested thread processing\n",
					epicsThreadGetNameSelf(), precord->name);
		};

		pNode->privateCount++;

		dbScanLock((struct dbCommon*) precord);
		process = (processMethod)(*prset->process);
		process(precord);
		dbScanUnlock((struct dbCommon*) precord);
	};
	
	return 0;
}

static void spawnThread(void *param)
{
	subRecord *precord = (subRecord*)param;

	queueId  = epicsMessageQueueCreate(20, sizeof(NODE*));
	threadId = epicsThreadCreate("sysProcThread",
								epicsThreadPriorityLow,
								epicsThreadGetStackSize(epicsThreadStackSmall),
								(EPICSTHREADFUNC)calcThread, 
								NULL);
	if(subSysProcDebug) 
	{
		epicsPrintf("Thread Spawn by (thread) %s, (record) %s\n",
				epicsThreadGetNameSelf(), precord->name);
	};
}


/*subroutine record*/
static long subSysProcInit(subRecord *precord)
{
	static int instanctCount = 0;
	NODE *pNode;

	if(subSysProcDebug) {
		epicsPrintf("Init SubRecord by (thread) %s, (record) %s\n",
				epicsThreadGetNameSelf(),
				precord->name);
	}

	pNode = (NODE*)malloc(sizeof(NODE));
	pNode->insCount = ++instanctCount;
	pNode->privateCount = 0;
	pNode->precord = precord; 
	precord->dpvt = (void*)pNode;

	/* user data pointer in parameter*/
	/*epicsThreadOnce(&threadOnceFlag, (void(*))spawnThread, (void*) precord);*/


	epicsThreadOnce(&threadOnceFlag, spawnThread, (void*) precord);
	return 0;
}

static long subSysProcCalc(subRecord *precord)
{
	NODE *pNode = (NODE*)precord->dpvt;

	if(precord->pact)
	{
		if(subSysProcDebug) 
		{
			epicsPrintf("Proc Phase 2 execution by (thread) %s, (record) %s\n",
					epicsThreadGetNameSelf(), precord->name);
		};
		precord->val = getSysProcessStatus();
		precord->pact = FALSE;
		return 0;
	};

	precord->pact = TRUE;
	if(subSysProcDebug) 
	{
		epicsPrintf("Proc Phase 1 execution by (thread) %s, (record) %s\n",
				epicsThreadGetNameSelf(), precord->name);
	};

	epicsMessageQueueSend(queueId, (void*)&pNode, sizeof(NODE*));
	return 2;
}

epicsRegisterFunction(subSysProcInit);
epicsRegisterFunction(subSysProcCalc);
