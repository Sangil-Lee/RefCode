/****************************************************************************

Module      : sfwTSSGBody.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2010-1-24
  

*****************************************************************************/
#include <sys/mman.h>
#include <fcntl.h> /* open() */


#include "aiRecord.h"
#include "aoRecord.h"
#include "biRecord.h"
#include "boRecord.h"

#include "dbAccess.h"  /*#include "dbAccessDefs.h" --> S_db_badField */


/**********************************
user include files
***********************************/
#include "devTSSG.h"

/**********************************
user include files 
***********************************/


#define BO_SHOW_REG		1
#define BO_START		2
#define BO_STOP_PULSE		3



#define BO_SHOW_REG_STR	"show_reg"
#define BO_START_STR	"start"
#define BO_STOP_PULSE_STR		"stop_pulse"



#define AO_LASER_CLOCK  	1
#define AO_LASER_CLOCK_WIDTH	2
#define AO_QSWITCH_DELAY  	3
#define AO_QSWITCH_WIDTH	4


/*#define AO_LTU_LEAD_TIME		3 */
#define AO_BG_PULSE_DELAY		5
#define AO_BG_PULSE_WIDTH		6
#define AO_BG_PULSE_CNT		7
/*
#define AO_QDC_IN_COARSE_DELAY		7
#define AO_QDC_IN_FINE_DELAY		8
#define AO_QDC_INPUT_WIDTH		9
*/
#define AO_LASER_CLOCK_STR  	"lamp_clock"
#define AO_LASER_CLOCK_WIDTH_STR	"lamp_width"
#define AO_QSWITCH_DELAY_STR  	"q_delay"
#define AO_QSWITCH_WIDTH_STR	"q_width"

/*#define AO_LTU_LEAD_TIME_STR		"ltu_lead" */
#define AO_BG_PULSE_DELAY_STR		"bg_delay"
#define AO_BG_PULSE_WIDTH_STR		"bg_width"
#define AO_BG_PULSE_CNT_STR		"bg_cnt"
/*
#define AO_QDC_IN_COARSE_DELAY_STR		"coarse_delay"
#define AO_QDC_IN_FINE_DELAY_STR		"fine_delay"
#define AO_QDC_INPUT_WIDTH_STR		"qdc_width"
*/


#define AI_BTW_LTU_DG535_IN		1
#define AI_INPUT_PULSE_CNT		2


#define AI_BTW_LTU_DG535_IN_STR		"btw_dg535_in"
#define AI_INPUT_PULSE_CNT_STR		"pulse_cnt"










typedef struct {
	ST_TSSG  *ptaskConfig;

	char            arg0[64];
	char            arg1[64];
	char            recordName[SIZE_PV_NAME];	
	int               ind;
} ST_devTSSG_dpvt;

typedef struct {
	void    *precord;
	double   setValue;
	int      n32Arg0;
} ST_execParam;

typedef void (*EXECFUNCQUEUE) (ST_execParam *pParam);

typedef void (*VOIDFUNCPTR)(void *arg);

typedef struct {
	EXECFUNCQUEUE     pFunc;
	ST_execParam      param;
} ST_threadQueueData;




static ST_TSSG          *pST_TSSG = NULL;

static ST_threadCfg*  make_TSSG_controlThreadConfig();
static void threadFunc_TSSG_control(void *param);
static void destroy_TSSGTask(void);


/*******************************************************/
/*******************************************************/

static long devAoTSSG_init_record(aoRecord *precord);
static long devAoTSSG_write_ao(aoRecord *precord);

static long devAiTSSG_init_record(aiRecord *precord);
static long devAiTSSG_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiTSSG_read_ai(aiRecord *precord);

static long devBoTSSG_init_record(boRecord *precord);
static long devBoTSSG_write_bo(boRecord *precord);

static long devBiTSSG_init_record(biRecord *precord);
static long devBiTSSG_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiTSSG_read_bi(biRecord *precord);

/*******************************************************/
static void createTSSGTaskCallFunction(const iocshArgBuf *args);

static const iocshArg createTSSGTaskArg0 = {"task_name", iocshArgString};
static const iocshArg* const createTSSGTaskArgs[] = { &createTSSGTaskArg0 };
static const iocshFuncDef createTSSGTaskFuncDef = { "createTSSGTask", 1, createTSSGTaskArgs };



typedef struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_write;
	DEVSUPFUN	special_linconv;
} BINARYDSET;


BINARYDSET devBoTSSG = { 6,  NULL,  NULL,   devBoTSSG_init_record,  NULL,  devBoTSSG_write_bo,  NULL };
BINARYDSET  devAoTSSG = {	6, NULL, NULL,  devAoTSSG_init_record, NULL, devAoTSSG_write_ao, NULL };
BINARYDSET  devAiTSSG = { 6, NULL, NULL, devAiTSSG_init_record, devAiTSSG_get_ioint_info, devAiTSSG_read_ai, NULL };
BINARYDSET  devBiTSSG = { 5, NULL, NULL, devBiTSSG_init_record, devBiTSSG_get_ioint_info, devBiTSSG_read_bi };

epicsExportAddress(dset, devBoTSSG);
epicsExportAddress(dset, devAoTSSG);
epicsExportAddress(dset, devAiTSSG);
epicsExportAddress(dset, devBiTSSG);

static void TSSGTaskRegister(void)
{
	iocshRegister(&createTSSGTaskFuncDef, createTSSGTaskCallFunction);
}
epicsExportRegistrar(TSSGTaskRegister);



int create_TSSGConfig()
{
	if(pST_TSSG) return 0;
	
	pST_TSSG = (ST_TSSG*) malloc(sizeof(ST_TSSG));
	if(!pST_TSSG) return 1;

	return 0;
}

static void createTSSGTaskCallFunction(const iocshArgBuf * args)
{
	char task_name[SIZE_TASK_NAME];
	

	if(args[0].sval) strcpy(task_name, args[0].sval);

	if(create_TSSGConfig()) return;

	epicsThreadSleep(.5);

	strcpy(pST_TSSG->taskName, task_name);

	scanIoInit(&pST_TSSG->ioScanPvt_status);
	pST_TSSG->StatusTSSG = TASK_NOT_INIT;
	pST_TSSG->StatusTSSG |= TASK_SYSTEM_IDLE;
	pST_TSSG->ErrorTSSG = ERROR_NONE;


	pST_TSSG->fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (pST_TSSG->fd  < 0)
	{
		fprintf(stdout, "Device open error !!!!!!\n");
		return;
	}
/*	pST_TSSG->pBaseAddr = (ST_REG_MAP *)malloc(MAP_SIZE ); */
	pST_TSSG->pBaseAddr  = mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pST_TSSG->fd, C_BASEADDR & ~MAP_MASK);
	if ( pST_TSSG->pBaseAddr == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [gpStartStop]\n");
		return;
	}
/*
	 pST_TSSG->pReg = (ST_REG_MAP*) (pST_TSSG->pBaseAddr);

	printf("REG_START_STOP_OFFSET:    0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_START_STOP_OFFSET));
	printf("REG_LAMP_CLK_OFFSET:     0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_LAMP_CLK_OFFSET));
	printf("REG_LAMP_WIDTH_OFFSET:    0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_LAMP_WIDTH_OFFSET));
	printf("REG_QSWITCH_DELAY_OFFSET:     0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_QSWITCH_DELAY_OFFSET));
	printf("REG_QSWITCH_WIDTH_OFFSET:    0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_QSWITCH_WIDTH_OFFSET));
	printf("REG_BACK_PULSE_DELAY_OFFSET:     0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_BACK_PULSE_DELAY_OFFSET));
	printf("REG_BACK_PULSE_CNT_OFFSET:     0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_BACK_PULSE_CNT_OFFSET));
	printf("REG_BACK_PULSE_WIDTH_OFFSET:      0x%X\n", *(unsigned int *)(pST_TSSG->pBaseAddr+REG_BACK_PULSE_WIDTH_OFFSET));
*/

	
	printf("REG_START_STOP_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpStartStop);
	printf("REG_LAMP_CLK_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpLampClkHz);
	printf("REG_LAMP_WIDTH_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpLampClkWidth);
	printf("REG_QSWITCH_DELAY_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpQSwitchDely);
	printf("REG_QSWITCH_WIDTH_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpQSwitchWidth);
	printf("REG_BACK_PULSE_DELAY_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseDelay);
	printf("REG_BACK_PULSE_CNT_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseCnt);
	printf("REG_BACK_PULSE_WIDTH_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseWidth);

	printf("REG_BTW_LTU_DG535_IN:	  0x%X\n", pST_TSSG->pBaseAddr->gpBtwLTU_dg535_input);
	printf("REG_INPUT_PULSE_CNT:	  0x%X\n", pST_TSSG->pBaseAddr->gpRealPulseCnt);
	


	pST_TSSG->pST_CtrlThread = make_TSSG_controlThreadConfig(pST_TSSG);
	if(!pST_TSSG->pST_CtrlThread) return;

	epicsPrintf("make TSSG control thread ok!: %p\n", pST_TSSG->pST_CtrlThread);


	epicsAtExit((VOIDFUNCPTR) destroy_TSSGTask, NULL);

	return;
}

static void destroy_TSSGTask(void)
{
	printf("Exit Hook: Stopping Task %s ... \n", pST_TSSG->taskName); 

	munmap(pST_TSSG->pBaseAddr, MAP_SIZE);
	close( pST_TSSG->fd );

/*	if( pST_TSSG->pBaseAddr)
		free(pST_TSSG->pBaseAddr);
*/
	if( pST_TSSG->pST_CtrlThread)
		free(pST_TSSG->pST_CtrlThread);
	
	if( pST_TSSG )
		free(pST_TSSG);		

	printf("IOC:TSSG exit\n");
	return;
}


static ST_threadCfg*  make_TSSG_controlThreadConfig()
{
	ST_threadCfg *pST_CtrlThread  = NULL;

	pST_CtrlThread  = (ST_threadCfg*)malloc(sizeof(ST_threadCfg));
	if(!pST_CtrlThread) return pST_CtrlThread; 

	sprintf(pST_CtrlThread->threadName, "%s_ctrl", pST_TSSG->taskName); 

	pST_CtrlThread->threadPriority = epicsThreadPriorityLow;
	pST_CtrlThread->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);  /* epicsThreadStackMedium   */
	pST_CtrlThread->threadFunc      = (EPICSTHREADFUNC) threadFunc_TSSG_control;
	pST_CtrlThread->threadParam     = (void*) pST_TSSG;

	pST_CtrlThread->threadQueueId   = epicsMessageQueueCreate(1000,sizeof(ST_threadQueueData));
                                                /* Increase queue size to avoid buffer overflow, KKH May 14, 2008 */

	epicsThreadCreate(pST_CtrlThread->threadName,
			  pST_CtrlThread->threadPriority,
			  pST_CtrlThread->threadStackSize,
			  (EPICSTHREADFUNC) pST_CtrlThread->threadFunc,
			  (void*) pST_CtrlThread->threadParam);

	return pST_CtrlThread;
}

static void threadFunc_TSSG_control(void *param)
{
	ST_TSSG *pTSSGCfg = (ST_TSSG*) param;
	ST_threadCfg *pST_controlThread;
	ST_threadQueueData		queueData;

	while (!pTSSGCfg->pST_CtrlThread)  epicsThreadSleep(.1);

	pST_controlThread = (ST_threadCfg*) pTSSGCfg->pST_CtrlThread;
	while(TRUE) 
	{
		EXECFUNCQUEUE            pFunc;
		ST_execParam                *pST_execParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		epicsMessageQueueReceive(pST_controlThread->threadQueueId,
				         (void*) &queueData,
					 sizeof(ST_threadQueueData));

		pFunc      = queueData.pFunc;
		pST_execParam = &queueData.param;	
		precord    = (struct dbCommon*) pST_execParam->precord;
		prset      = (struct rset*) precord->rset;

		if(!pFunc) continue;
		else pFunc(pST_execParam);

		if(!precord) continue;

		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}

	return;
}


static void devTSSG_BO_SHOW_REG(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("REG_START_STOP_OFFSET:     %d: 0x%X\n", pST_TSSG->pBaseAddr->gpStartStop, pST_TSSG->pBaseAddr->gpStartStop);
	epicsPrintf("REG_LAMP_CLK_OFFSET:      %d Hz: 0x%X\n", 1000/(pST_TSSG->pBaseAddr->gpLampClkHz/100000), pST_TSSG->pBaseAddr->gpLampClkHz);
	epicsPrintf("REG_LAMP_WIDTH_OFFSET:    %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpLampClkWidth*10/1000, pST_TSSG->pBaseAddr->gpLampClkWidth);
	epicsPrintf("REG_QSWITCH_DELAY_OFFSET:      %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpQSwitchDely*10/1000, pST_TSSG->pBaseAddr->gpQSwitchDely);
	epicsPrintf("REG_QSWITCH_WIDTH_OFFSET:     %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpQSwitchWidth*10/1000, pST_TSSG->pBaseAddr->gpQSwitchWidth);
	epicsPrintf("REG_BACK_PULSE_DELAY_OFFSET:     %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseDelay*10/1000, pST_TSSG->pBaseAddr->gpBackPulseDelay);
	epicsPrintf("REG_BACK_PULSE_CNT_OFFSET:     %d: 0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseCnt, pST_TSSG->pBaseAddr->gpBackPulseCnt);
	epicsPrintf("REG_BACK_PULSE_WIDTH_OFFSET:     %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseWidth*10/1000, pST_TSSG->pBaseAddr->gpBackPulseWidth);
/*	epicsPrintf("REG_BACK_PULSE_CNT_OFFSET:      %d ns: 0x%X\n",  (pST_TSSG->pBaseAddr->gpQdcPulseDelay>>16)*10, pST_TSSG->pBaseAddr->gpQdcPulseDelay);
	epicsPrintf("REG_BACK_PULSE_WIDTH_OFFSET:      %d ns: 0x%X\n", pST_TSSG->pBaseAddr->gpQdcPulseWidth*10, pST_TSSG->pBaseAddr->gpQdcPulseWidth);

	epicsPrintf("REG_BTW_LTU_LASER_IN:      %d ns: 0x%X\n", (unsigned int)pST_TSSG->pBaseAddr->gpBtwLTU_laser_input*10, pST_TSSG->pBaseAddr->gpBtwLTU_laser_input); */
	epicsPrintf("REG_BTW_LTU_DG535_IN:      %d ns: 0x%X\n", (unsigned int)pST_TSSG->pBaseAddr->gpBtwLTU_dg535_input*10, pST_TSSG->pBaseAddr->gpBtwLTU_dg535_input);
	epicsPrintf("REG_INPUT_PULSE_CNT:       %d : 0x%X\n", (unsigned int)pST_TSSG->pBaseAddr->gpRealPulseCnt, pST_TSSG->pBaseAddr->gpRealPulseCnt);

	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_BO_START(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	unsigned int value = pST_TSSG->pBaseAddr->gpStartStop;

	if( (int)pParam->setValue ) /* start command */
		pST_TSSG->pBaseAddr->gpStartStop =  SWAP32(value & 0xfffffffe);
	else  /* stop command */
		pST_TSSG->pBaseAddr->gpStartStop = SWAP32( value | 0x1);

	epicsPrintf("REG_START_STOP_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpStartStop);

	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_BO_STOP_PULSE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	unsigned int value = pST_TSSG->pBaseAddr->gpStartStop;

	if( (int)pParam->setValue ) /* stop command */
	{
		pST_TSSG->pBaseAddr->gpStartStop =  SWAP32(value | 0x8000);
		scanIoRequest(pST_TSSG->ioScanPvt_status);
	}
	else  /* release command */
	{
		pST_TSSG->pBaseAddr->gpStartStop = SWAP32( value & 0xffff7fff);
		pST_TSSG->pBaseAddr->gpRealPulseCnt = 0x0; 
	}

	epicsPrintf("REG_START_STOP_OFFSET:	  0x%X\n", pST_TSSG->pBaseAddr->gpStartStop);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_AO_LASER_CLOCK(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: Hz */
	pST_TSSG->pBaseAddr->gpLampClkHz = SWAP32( (1000/(unsigned int)pParam->setValue)*100000 ) ; 

	epicsPrintf("REG_LAMP_CLK_OFFSET:	  %d Hz: 0x%X\n", 1000/(pST_TSSG->pBaseAddr->gpLampClkHz/100000), pST_TSSG->pBaseAddr->gpLampClkHz);	
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_AO_LASER_CLOCK_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: us */
	pST_TSSG->pBaseAddr->gpLampClkWidth = SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("REG_LAMP_WIDTH_OFFSET:	  %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpLampClkWidth*10/1000, pST_TSSG->pBaseAddr->gpLampClkWidth);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_AO_QSWITCH_DELAY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: us */
	pST_TSSG->pBaseAddr->gpQSwitchDely = SWAP32( (unsigned int)pParam->setValue*100  ) ; 

	epicsPrintf("REG_QSWITCH_DELAY:	  %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpQSwitchDely*10/1000, pST_TSSG->pBaseAddr->gpQSwitchDely);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_AO_QSWITCH_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: us */
	pST_TSSG->pBaseAddr->gpQSwitchWidth= SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("REG_QSWITCH_WIDTH:   %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpQSwitchDely*10/1000, pST_TSSG->pBaseAddr->gpQSwitchDely);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}


/*
static void devTSSG_AO_LTU_LEAD_TIME(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}
*/
static void devTSSG_AO_BG_PULSE_DELAY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	
	/* input unit: us */
	pST_TSSG->pBaseAddr->gpBackPulseDelay = SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("REG_QSWITCH_WIDTH_OFFSET:	  %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseDelay*10/1000, pST_TSSG->pBaseAddr->gpBackPulseDelay);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);

}
static void devTSSG_AO_BG_PULSE_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: us */
	pST_TSSG->pBaseAddr->gpBackPulseWidth = SWAP32( (unsigned int)pParam->setValue*100 ) ; 
	
	epicsPrintf("REG_BACK_PULSE_DELAY_OFFSET:	  %d us: 0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseWidth*10/1000, pST_TSSG->pBaseAddr->gpBackPulseWidth);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}
static void devTSSG_AO_BG_PULSE_CNT(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: us */
	pST_TSSG->pBaseAddr->gpBackPulseCnt= SWAP32( (unsigned int)pParam->setValue ) ; 
	
	epicsPrintf("REG_BACK_PULSE_CNT:	  %d: 0x%X\n", pST_TSSG->pBaseAddr->gpBackPulseCnt, pST_TSSG->pBaseAddr->gpBackPulseCnt);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

#if 0
static void devTSSG_AO_QDC_IN_COARSE_DELAY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	unsigned int fine = pST_TSSG->pBaseAddr->gpQdcPulseDelay & 0xffff;

	/* input unit: ns */
	pST_TSSG->pBaseAddr->gpQdcPulseDelay = SWAP32( ( (unsigned int)pParam->setValue/10 << 16 ) | fine  ) ; 

	epicsPrintf("REG_BACK_PULSE_CNT_OFFSET:coarse %d ns: 0x%X\n", (pST_TSSG->pBaseAddr->gpQdcPulseDelay>>16)*10, pST_TSSG->pBaseAddr->gpQdcPulseDelay>>16);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}
static void devTSSG_AO_QDC_IN_FINE_DELAY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	unsigned int coarse = pST_TSSG->pBaseAddr->gpQdcPulseDelay & 0xffff0000;

	/* input unit: count */
	pST_TSSG->pBaseAddr->gpQdcPulseDelay = SWAP32( coarse |  (unsigned int)pParam->setValue  ) ; 

	epicsPrintf("REG_BACK_PULSE_CNT_OFFSET:fine   %d: 0x%X\n", pST_TSSG->pBaseAddr->gpQdcPulseDelay&0xffff, pST_TSSG->pBaseAddr->gpQdcPulseDelay&0xffff);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}

static void devTSSG_AO_QDC_INPUT_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;

	/* input unit: ns */
	pST_TSSG->pBaseAddr->gpQdcPulseWidth = SWAP32( (unsigned int)pParam->setValue/10 ) ; 

	epicsPrintf("REG_BACK_PULSE_WIDTH_OFFSET:	  %d ns: 0x%X\n", pST_TSSG->pBaseAddr->gpQdcPulseWidth*10, pST_TSSG->pBaseAddr->gpQdcPulseWidth);
	epicsPrintf("%s: %s ( %d )\n", pST_TSSG->taskName, precord->name, (int)pParam->setValue);
}
#endif



static long devAoTSSG_init_record(aoRecord *precord)
{
	ST_devTSSG_dpvt *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) malloc(sizeof(ST_devTSSG_dpvt));
	int nval;

	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pdevTSSG_dpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s", pdevTSSG_dpvt->arg0, pdevTSSG_dpvt->arg1);
#if PRINT_DEV_SUPPORT_ARG
			printf("devAoTSSG arg num: %d: %s %s\n", nval,  pdevTSSG_dpvt->arg0, pdevTSSG_dpvt->arg1);
#endif
			pdevTSSG_dpvt->ptaskConfig = pST_TSSG;
			if(!pdevTSSG_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoTSSG (init_record) Illegal OUT field: task_name");
				free(pdevTSSG_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoTSSG (init_record) Illegal OUT field");
			free(pdevTSSG_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}



	if (!strcmp(pdevTSSG_dpvt->arg0, AO_LASER_CLOCK_STR)) {
		pdevTSSG_dpvt->ind = AO_LASER_CLOCK;
	} else if (!strcmp(pdevTSSG_dpvt->arg0, AO_LASER_CLOCK_WIDTH_STR)) {
		pdevTSSG_dpvt->ind = AO_LASER_CLOCK_WIDTH;
	} else if (!strcmp(pdevTSSG_dpvt->arg0, AO_QSWITCH_DELAY_STR)) {
		pdevTSSG_dpvt->ind = AO_QSWITCH_DELAY;
	} else if (!strcmp(pdevTSSG_dpvt->arg0, AO_QSWITCH_WIDTH_STR)) {
		pdevTSSG_dpvt->ind = AO_QSWITCH_WIDTH;
	}
	/* else if (!strcmp(pdevTSSG_dpvt->arg0, AO_LTU_LEAD_TIME_STR)) {
		pdevTSSG_dpvt->ind = AO_LTU_LEAD_TIME;
	} */ else if (!strcmp(pdevTSSG_dpvt->arg0, AO_BG_PULSE_DELAY_STR)) {
		pdevTSSG_dpvt->ind = AO_BG_PULSE_DELAY;
	} else if (!strcmp(pdevTSSG_dpvt->arg0, AO_BG_PULSE_WIDTH_STR)) {
		pdevTSSG_dpvt->ind = AO_BG_PULSE_WIDTH;
	} else if (!strcmp(pdevTSSG_dpvt->arg0, AO_BG_PULSE_CNT_STR)) {
		pdevTSSG_dpvt->ind = AO_BG_PULSE_CNT;
	}
	/* else if (!strcmp(pdevTSSG_dpvt->arg0, AO_QDC_IN_COARSE_DELAY_STR)) {
		pdevTSSG_dpvt->ind = AO_QDC_IN_COARSE_DELAY;
	} else if (!strcmp(pdevTSSG_dpvt->arg0, AO_QDC_IN_FINE_DELAY_STR)) {
		pdevTSSG_dpvt->ind = AO_QDC_IN_FINE_DELAY;
	}
	else if (!strcmp(pdevTSSG_dpvt->arg0, AO_QDC_INPUT_WIDTH_STR)) {
		pdevTSSG_dpvt->ind = AO_QDC_INPUT_WIDTH;
	} */
	else {
		pdevTSSG_dpvt->ind = -1;
		epicsPrintf("ERROR! devAoTSSG_init_record: arg0 \"%s\" \n",  pdevTSSG_dpvt->arg0 );
		
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevTSSG_dpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoTSSG_write_ao(aoRecord *precord)
{
	ST_devTSSG_dpvt        *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) precord->dpvt;
	ST_TSSG          *pTSSGCfg;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pdevTSSG_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pTSSGCfg			= pdevTSSG_dpvt->ptaskConfig;
	pControlThreadConfig		= pTSSGCfg->pST_CtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pdevTSSG_dpvt->ind) 
		{
			case AO_LASER_CLOCK:
				qData.pFunc = devTSSG_AO_LASER_CLOCK;
				break;
			case AO_LASER_CLOCK_WIDTH:
				qData.pFunc = devTSSG_AO_LASER_CLOCK_WIDTH;
				break;
			case AO_QSWITCH_DELAY:
				qData.pFunc = devTSSG_AO_QSWITCH_DELAY;
				break;
			case AO_QSWITCH_WIDTH:
				qData.pFunc = devTSSG_AO_QSWITCH_WIDTH;
				break;
/*
			case AO_LTU_LEAD_TIME:
				qData.pFunc = devTSSG_AO_LTU_LEAD_TIME;
				break;
	*/			
			case AO_BG_PULSE_DELAY:
				qData.pFunc = devTSSG_AO_BG_PULSE_DELAY;
				break;
			case AO_BG_PULSE_WIDTH:
				qData.pFunc = devTSSG_AO_BG_PULSE_WIDTH;
				break;
			case AO_BG_PULSE_CNT:
				qData.pFunc = devTSSG_AO_BG_PULSE_CNT;
				break;
/*
			case AO_QDC_IN_COARSE_DELAY:
				qData.pFunc = devTSSG_AO_QDC_IN_COARSE_DELAY;
				break;
			case AO_QDC_IN_FINE_DELAY:
				qData.pFunc = devTSSG_AO_QDC_IN_FINE_DELAY;
				break;				
			case AO_QDC_INPUT_WIDTH:
				qData.pFunc = devTSSG_AO_QDC_INPUT_WIDTH;
				break;
	*/			
				
			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*(0)=>(success ) */
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		
		switch(pdevTSSG_dpvt->ind) 
		{
/*			case AO_SET_INTERCLK: precord->val = 0; break; */
			default: break;
		}
		return 0; /*(0)=>(success ) */
	}

	return -1;
}



static long devAiTSSG_init_record(aiRecord *precord)
{
	ST_devTSSG_dpvt *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) malloc(sizeof(ST_devTSSG_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pdevTSSG_dpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s", pdevTSSG_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devAiTSSG arg num: %d: %s\n", nval, pdevTSSG_dpvt->arg0);
#endif
			pdevTSSG_dpvt->ptaskConfig = pST_TSSG;
			if(!pdevTSSG_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiTSSG (init_record) Illegal INP field: task name");
				free(pdevTSSG_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiTSSG (init_record) Illegal INP field");
			free(pdevTSSG_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pdevTSSG_dpvt->arg0, AI_BTW_LTU_DG535_IN_STR))
		pdevTSSG_dpvt->ind = AI_BTW_LTU_DG535_IN;
	else if(!strcmp(pdevTSSG_dpvt->arg0, AI_INPUT_PULSE_CNT_STR))
		pdevTSSG_dpvt->ind = AI_INPUT_PULSE_CNT;
	
	else {
		pdevTSSG_dpvt->ind = -1;
		epicsPrintf("ERROR! devAiTSSG_init_record: arg0 \"%s\" \n",  pdevTSSG_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevTSSG_dpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiTSSG_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_devTSSG_dpvt *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) precord->dpvt;
	ST_TSSG   *pTSSGCfg;

	if(!pdevTSSG_dpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pTSSGCfg = pdevTSSG_dpvt->ptaskConfig;
	*ioScanPvt  = pTSSGCfg->ioScanPvt_status;
	return 0;
}

static long devAiTSSG_read_ai(aiRecord *precord)
{ 
	ST_devTSSG_dpvt	*pdevTSSG_dpvt = (ST_devTSSG_dpvt*) precord->dpvt;
	ST_TSSG		*pTSSGCfg;

	if(!pdevTSSG_dpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pTSSGCfg = pdevTSSG_dpvt->ptaskConfig;

	switch(pdevTSSG_dpvt->ind) 
	{
		case AI_BTW_LTU_DG535_IN:
			precord->val = pST_TSSG->pBaseAddr->gpBtwLTU_dg535_input / 100; /* unit: us */
			epicsPrintf("REG_BTW_LTU_DG535_IN:  %d us: 0x%X\n", (unsigned int)precord->val , pST_TSSG->pBaseAddr->gpBtwLTU_dg535_input );
			break;
		case AI_INPUT_PULSE_CNT:
			precord->val = pST_TSSG->pBaseAddr->gpRealPulseCnt; 
			epicsPrintf("REG_INPUT_PULSE_CNT:   %d : 0x%X\n", (unsigned int)precord->val , pST_TSSG->pBaseAddr->gpRealPulseCnt);
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pTSSGCfg->taskName, pdevTSSG_dpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}

static long devBoTSSG_init_record(boRecord *precord)
{
	ST_devTSSG_dpvt *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) malloc(sizeof(ST_devTSSG_dpvt));
	int nval;
	
	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pdevTSSG_dpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s", pdevTSSG_dpvt->arg0);
#if PRINT_DEV_SUPPORT_ARG
			printf("devBoTSSG arg num: %d: %s\n", nval,  pdevTSSG_dpvt->arg0);
#endif
			pdevTSSG_dpvt->ptaskConfig = pST_TSSG;
			if(!pdevTSSG_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devBoTSS (init_record) Illegal OUT field: task_name");
				free(pdevTSSG_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devBoTSS (init_record) Illegal OUT field");
			free(pdevTSSG_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(pdevTSSG_dpvt->arg0, BO_SHOW_REG_STR)) {
		pdevTSSG_dpvt->ind = BO_SHOW_REG;
	} 
	else if(!strcmp(pdevTSSG_dpvt->arg0, BO_START_STR)) {
		pdevTSSG_dpvt->ind = BO_START;
	}
	else if(!strcmp(pdevTSSG_dpvt->arg0, BO_STOP_PULSE_STR)) {
		pdevTSSG_dpvt->ind = BO_STOP_PULSE;
	}
	

	else {
		pdevTSSG_dpvt->ind = -1;
		epicsPrintf("ERROR! devBoTSSG_init_record: arg0 \"%s\" \n",  pdevTSSG_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevTSSG_dpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoTSSG_write_bo(boRecord *precord)
{
	ST_devTSSG_dpvt		*pdevTSSG_dpvt = (ST_devTSSG_dpvt*) precord->dpvt;
	ST_TSSG   *pTSSCfg;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pdevTSSG_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pTSSCfg			= pdevTSSG_dpvt->ptaskConfig;
	pControlThreadConfig		= pTSSCfg->pST_CtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;


        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{
		precord->pact = TRUE;
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pdevTSSG_dpvt->ind) {
			case BO_SHOW_REG:
				qData.pFunc = devTSSG_BO_SHOW_REG;
				break;
			case BO_START:
				qData.pFunc = devTSSG_BO_START;
				break;
			case BO_STOP_PULSE:
				qData.pFunc = devTSSG_BO_STOP_PULSE;
				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
/*
		switch(pdevTSSG_dpvt->ind) {
			case BO_SYS_ARMING: 
				if( pTSSCfg->StatusTSS & TASK_ARM_ENABLED )  precord->val = 1;
				else  precord->val = 0;
				break;
			case BO_SYS_RUN: 
				if( pTSSCfg->StatusTSS & TASK_DAQ_STARTED)  precord->val = 1;
				else  precord->val = 0;				
				break;
			default: break;
		}
*/
		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devBiTSSG_init_record(biRecord *precord)
{
	ST_devTSSG_dpvt *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) malloc(sizeof(ST_devTSSG_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pdevTSSG_dpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s", pdevTSSG_dpvt->arg0 );
#if PRINT_DEV_SUPPORT_ARG
			printf("devBiTSSG arg num: %d: %s\n", nval, pdevTSSG_dpvt->arg0);
#endif
			pdevTSSG_dpvt->ptaskConfig = pST_TSSG;
			if(!pdevTSSG_dpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBiTSSG (init_record) Illegal INP field: task name");
				free(pdevTSSG_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiTSSG (init_record) Illegal INP field");
			free(pdevTSSG_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pdevTSSG_dpvt->arg0, "xxx"))
		pdevTSSG_dpvt->ind = 1;
	else {
		pdevTSSG_dpvt->ind = -1;
		epicsPrintf("ERROR! devBiTSSG_init_record: arg0 \"%s\" \n",  pdevTSSG_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevTSSG_dpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiTSSG_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_devTSSG_dpvt *pdevTSSG_dpvt = (ST_devTSSG_dpvt*) precord->dpvt;
	ST_TSSG   *pTSSGCfg;

	if(!pdevTSSG_dpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pTSSGCfg = pdevTSSG_dpvt->ptaskConfig;
	*ioScanPvt  = pTSSGCfg->ioScanPvt_status;
	return 0;
}

static long devBiTSSG_read_bi(biRecord *precord)
{ 
	ST_devTSSG_dpvt	*pdevTSSG_dpvt = (ST_devTSSG_dpvt*) precord->dpvt;
	ST_TSSG		*pTSSGCfg;

	if(!pdevTSSG_dpvt) return -1;

	pTSSGCfg = pdevTSSG_dpvt->ptaskConfig;

	switch(pdevTSSG_dpvt->ind) 
	{
		case 1:
			precord->val = 0;
/*			epicsPrintf("%s StatusDev: 0x%X \n", pTSSGCfg->taskName, (int)precord->val); */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pTSSGCfg->taskName, pdevTSSG_dpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert, don't convert)*/
}


