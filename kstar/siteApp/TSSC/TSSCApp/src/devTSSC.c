/****************************************************************************

Module      : devXXX.c

Copyright(c): 2012 NFRI. All Rights Reserved.

Revision History:
Author: woong   2012-4-27
  

*****************************************************************************/
#include <sys/mman.h>
#include <fcntl.h> /* open() */


#include "aiRecord.h"
#include "aoRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "stringoutRecord.h"


#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
/*#include "sfwMDSplus.h"*/



/**********************************
user include files
***********************************/
#include "drvTSSC.h"

/**********************************
user include files 
***********************************/

#define PRINT_DBPRO_PHASE_INFO	0



#define BO_SHOW_INFO		1
#define BO_START		2
#define BO_START_QSW		3
#define BO_STOP_PULSE		4
#define BO_GPIO_OUT		5
#define BO_GPIO_DIR		6


#define BO_SHOW_INFO_STR	"show_info"
#define BO_START_STR	"start"
#define BO_START_QSW_STR		"start_qsw"
#define BO_STOP_PULSE_STR		"stop_pulse"
#define BO_GPIO_OUT_STR		"gpio_out"
#define BO_GPIO_DIR_STR		"gpio_dir"




#define BI_GPIO_IN		1
#define BI_TEST_2		2
#define BI_GPIO_IN_STR	"gpio_in"
#define BI_TEST_2_STR	"bi2"



#define AO_LASER_CLOCK  	1
#define AO_LASER_CLOCK_WIDTH	2
#define AO_QSWITCH_DELAY  	3
#define AO_QSWITCH_WIDTH	4
#define AO_BG_PULSE_DELAY		5
#define AO_BG_PULSE_WIDTH		6
#define AO_BG_PULSE_CNT		7
#define AO_SW_ON_DEALY		8


#define AO_LASER_CLOCK_STR  	"lamp_clock"
#define AO_LASER_CLOCK_WIDTH_STR	"lamp_width"
#define AO_QSWITCH_DELAY_STR  	"q_delay"
#define AO_QSWITCH_WIDTH_STR	"q_width"
#define AO_BG_PULSE_DELAY_STR		"bg_delay"
#define AO_BG_PULSE_WIDTH_STR		"bg_width"
#define AO_BG_PULSE_CNT_STR		"bg_cnt"
#define AO_SW_ON_DEALY_STR		"sw_delay"





#define AI_BTW_LTU_DG535_IN		1
#define AI_INPUT_PULSE_CNT		2
#define AI_LASER_OUT_PULSE_CNT		3
#define AI_Q_OUT_PULSE_CNT 		4

#define AI_BTW_LTU_DG535_IN_STR		"btw_dg535_in"
#define AI_INPUT_PULSE_CNT_STR		"pulse_cnt"
#define AI_LASER_OUT_PULSE_CNT_STR		"lout_cnt"
#define AI_Q_OUT_PULSE_CNT_STR 		"qout_cnt"








/*******************************************************/
/*******************************************************/

static long devAoTSSC_init_record(aoRecord *precord);
static long devAoTSSC_write_ao(aoRecord *precord);

static long devAiTSSC_init_record(aiRecord *precord);
static long devAiTSSC_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiTSSC_read_ai(aiRecord *precord);

static long devBoTSSC_init_record(boRecord *precord);
static long devBoTSSC_write_bo(boRecord *precord);

static long devBiTSSC_init_record(biRecord *precord);
static long devBiTSSC_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiTSSC_read_bi(biRecord *precord);


/*******************************************************/

BINARYDSET devBoTSSC = { 6,  NULL,  NULL,   devBoTSSC_init_record,  NULL,  devBoTSSC_write_bo,  NULL };
BINARYDSET  devAoTSSC = { 6, NULL, NULL,  devAoTSSC_init_record, NULL, devAoTSSC_write_ao, NULL };
BINARYDSET  devAiTSSC = { 6, NULL, NULL, devAiTSSC_init_record, devAiTSSC_get_ioint_info, devAiTSSC_read_ai, NULL };
BINARYDSET  devBiTSSC = { 6, NULL, NULL, devBiTSSC_init_record, devBiTSSC_get_ioint_info, devBiTSSC_read_bi, NULL };


epicsExportAddress(dset, devBoTSSC);
epicsExportAddress(dset, devAoTSSC);
epicsExportAddress(dset, devAiTSSC);
epicsExportAddress(dset, devBiTSSC);




static void devBoTSSC_BO_SHOW_INFO(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		printf("\n");
		printf("REG_START_STOP_OFFSET:         0x%X\n", pTSSC->pBaseAddr->gpStartStop);
		printf("REG_LASER_CLK_OFFSET:          %d Hz: 0x%X\n", 1000/(pTSSC->pBaseAddr->gpLaserClkHz/100000), pTSSC->pBaseAddr->gpLaserClkHz);
		printf("REG_LASER_WIDTH_OFFSET:        %d us: 0x%X\n", pTSSC->pBaseAddr->gpLaserClkWidth*10/1000, pTSSC->pBaseAddr->gpLaserClkWidth);
		printf("REG_QSWITCH_DELAY_OFFSET:      %d us: 0x%X\n", pTSSC->pBaseAddr->gpQSwitchDely*10/1000, pTSSC->pBaseAddr->gpQSwitchDely);
		printf("REG_QSWITCH_WIDTH_OFFSET:      %d us: 0x%X\n", pTSSC->pBaseAddr->gpQSwitchWidth*10/1000, pTSSC->pBaseAddr->gpQSwitchWidth);
		printf("REG_BACK_PULSE_DELAY_OFFSET:   %d us: 0x%X\n", pTSSC->pBaseAddr->gpBackPulseDelay*10/1000, pTSSC->pBaseAddr->gpBackPulseDelay);
		printf("REG_BACK_PULSE_CNT_OFFSET:     %d: 0x%X\n", pTSSC->pBaseAddr->gpBackPulseCnt, pTSSC->pBaseAddr->gpBackPulseCnt);
		printf("REG_BACK_PULSE_WIDTH_OFFSET:   %d us: 0x%X\n", pTSSC->pBaseAddr->gpBackPulseWidth*10/1000, pTSSC->pBaseAddr->gpBackPulseWidth);
		printf("REG_BTW_LTU_DG535_IN:          %d ns: 0x%X\n", (unsigned int)pTSSC->pBaseAddr->gpBtwLTU_dg535_input*10, pTSSC->pBaseAddr->gpBtwLTU_dg535_input);
		printf("REG_INPUT_PULSE_CNT:           %d : 0x%X\n", (unsigned int)pTSSC->pBaseAddr->gpRealPulseCnt, pTSSC->pBaseAddr->gpRealPulseCnt);
		printf("REG_LASER_OUT_PULSE_CNT:       %d : 0x%X\n", (unsigned int)pTSSC->pBaseAddr->gpLaserOutCnt, pTSSC->pBaseAddr->gpLaserOutCnt);
		printf("REG_Q_OUT_PULSE_CNT:           %d : 0x%X\n", (unsigned int)pTSSC->pBaseAddr->gpQswtOutCnt, pTSSC->pBaseAddr->gpQswtOutCnt);

		printf("REG_GPIO_IN:         0x%X\n",  pTSSC->pBaseAddr->gpGPIO_IN);
		printf("REG_GPIO_OUT:        0x%X\n", pTSSC->pBaseAddr->gpGPIO_OUT);
		printf("REG_GPIO_DIR:        0x%X\n", pTSSC->pBaseAddr->gpGPIO_Direction);
	}


	printf("%s: %s ( %d )\n", pSTDdev->taskName, precord->name, (int)pParam->setValue);
}
	
static void devBoTSSC_BO_START (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = pTSSC->pBaseAddr->gpStartStop;

	if( (int)pParam->setValue ) {
		pTSSC->pBaseAddr->gpStartStop =  SWAP32(value & 0xfffffffd);
	}
	else {
		pTSSC->pBaseAddr->gpStartStop = SWAP32( value | 0x2);
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devBoTSSC_BO_START_QSW (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = pTSSC->pBaseAddr->gpStartStop;

	if( (int)pParam->setValue ) {
		pTSSC->pBaseAddr->gpStartStop =  SWAP32(value & 0xfffffffb);
	}
	else {
		pTSSC->pBaseAddr->gpStartStop = SWAP32( value | 0x4);
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devBoTSSC_BO_STOP_PULSE (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = pTSSC->pBaseAddr->gpStartStop;

	if( (int)pParam->setValue ) {
		pTSSC->pBaseAddr->gpStartStop =  SWAP32(value | 0x4000); // REG(14)
//		pTSSC->pBaseAddr->gpStartStop =  SWAP32(value | 0x8000); // REG(15)
		scanIoRequest(pSTDdev->ioScanPvt_userCall); // temporary 2012. 8. 6
	}
	else {
		pTSSC->pBaseAddr->gpStartStop  = SWAP32( value & 0xffffbfff);// REG(14)
//		pTSSC->pBaseAddr->gpStartStop  = SWAP32( value & 0xffff7fff); // REG(15)
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}


static void devBoTSSC_BO_GPIO_OUT (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = pTSSC->pBaseAddr->gpGPIO_OUT;

	if( (int)pParam->setValue ) {
		pTSSC->pBaseAddr->gpGPIO_OUT  = SWAP32( value |( 0x1 << pParam->n32Arg0 ));
	}
	else {
		pTSSC->pBaseAddr->gpGPIO_OUT  = SWAP32( value & ( ~( 0x1 << pParam->n32Arg0 )));
//		pTSSC->pBaseAddr->gpGPIO_OUT  &= ~( 0x1 << pParam->n32Arg0 );
	}

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devBoTSSC_BO_GPIO_DIR (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	unsigned int value = pTSSC->pBaseAddr->gpGPIO_Direction;

	if( (int)pParam->setValue ) {   // 1: input mode
//		pTSSC->pBaseAddr->gpGPIO_Direction |= ( 0x1 << pParam->n32Arg0 );
		pTSSC->pBaseAddr->gpGPIO_Direction = SWAP32(value |( 0x1 << pParam->n32Arg0 ));
	}
	else {  // 0 : output mode
//		pTSSC->pBaseAddr->gpGPIO_Direction  &= ~( 0x1 << pParam->n32Arg0 );
		pTSSC->pBaseAddr->gpGPIO_Direction  =SWAP32(value & (~( 0x1 << pParam->n32Arg0 )));
	}

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}





static void devTSSC_AO_LASER_CLOCK(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	/* input unit: Hz */
	pTSSC->pBaseAddr->gpLaserClkHz = SWAP32( (1000/(unsigned int)pParam->setValue)*100000 ) ; 

	epicsPrintf("REG_LASER_CLK_OFFSET:	  %d Hz: 0x%X\n", 1000/(pTSSC->pBaseAddr->gpLaserClkHz/100000), pTSSC->pBaseAddr->gpLaserClkHz);	
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devTSSC_AO_LASER_CLOCK_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	/* input unit: us */
	pTSSC->pBaseAddr->gpLaserClkWidth = SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devTSSC_AO_QSWITCH_DELAY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	/* input unit: us */
	pTSSC->pBaseAddr->gpQSwitchDely = SWAP32( (unsigned int)pParam->setValue*100  ) ; 

	epicsPrintf("REG_QSWITCH_DELAY:	  %d us: 0x%X\n", pTSSC->pBaseAddr->gpQSwitchDely*10/1000, pTSSC->pBaseAddr->gpQSwitchDely);
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devTSSC_AO_QSWITCH_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	/* input unit: us */
	pTSSC->pBaseAddr->gpQSwitchWidth= SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("REG_QSWITCH_WIDTH:   %d us: 0x%X\n", pTSSC->pBaseAddr->gpQSwitchWidth*10/1000, pTSSC->pBaseAddr->gpQSwitchWidth);
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devTSSC_AO_BG_PULSE_DELAY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	
	/* input unit: us */
	pTSSC->pBaseAddr->gpBackPulseDelay = SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devTSSC_AO_BG_PULSE_CNT(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	pTSSC->pBaseAddr->gpBackPulseCnt= SWAP32( (unsigned int)pParam->setValue ) ; 
	
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devTSSC_AO_BG_PULSE_WIDTH(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;
	
	/* input unit: us */
	pTSSC->pBaseAddr->gpBackPulseWidth = SWAP32( (unsigned int)pParam->setValue*100 ) ; 

	epicsPrintf("REG_BACK_PULSE_WIDTH:	  %d us\n", pTSSC->pBaseAddr->gpBackPulseWidth*10/1000);
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
	
static void devTSSC_AO_SW_ON_DEALY(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_TSSC* pTSSC = (ST_TSSC *)pSTDdev->pUser;

	unsigned int value = pTSSC->pBaseAddr->gpStartStop & 0x7fff;
	
	/* input unit: us */
	unsigned int delay = (unsigned int)pParam->setValue*100  ;
		
	pTSSC->pBaseAddr->gpStartStop = SWAP32((delay << 15) | value);

	epicsPrintf("%s ( %d us )\n", precord->name, (int)pParam->setValue);
}









/*********************************************************
 initial setup of records 
*********************************************************/
static long devBoTSSC_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBoTSSC (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoTSSC (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, BO_SHOW_INFO_STR)) 
		pDpvt->ind = BO_SHOW_INFO;

	else if (!strcmp(arg0, BO_START_STR)) 
		pDpvt->ind = BO_START;
	else if (!strcmp(arg0, BO_START_QSW_STR)) 
		pDpvt->ind = BO_START_QSW;
	else if (!strcmp(arg0, BO_STOP_PULSE_STR)) 
		pDpvt->ind = BO_STOP_PULSE;
	else if (!strcmp(arg0, BO_GPIO_OUT_STR)) {
		pDpvt->ind = BO_GPIO_OUT;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* port id */	
	}
	else if (!strcmp(arg0, BO_GPIO_DIR_STR)) {
		pDpvt->ind = BO_GPIO_DIR;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* port id */	
	}
		
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoTSSC_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoTSSC_write_bo(boRecord *precord)
{
	ST_dpvt		*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg 	*pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;
	qData.param.n32Arg0		= pDpvt->n32Arg0;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) {

			case BO_SHOW_INFO:
				qData.pFunc = devBoTSSC_BO_SHOW_INFO;
				break;
			case BO_START: qData.pFunc = devBoTSSC_BO_START; break;
			case BO_START_QSW: qData.pFunc = devBoTSSC_BO_START_QSW; break;
			case BO_STOP_PULSE: qData.pFunc = devBoTSSC_BO_STOP_PULSE; break;
			case BO_GPIO_OUT: qData.pFunc = devBoTSSC_BO_GPIO_OUT; break;
			case BO_GPIO_DIR: qData.pFunc = devBoTSSC_BO_GPIO_DIR; break;


			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pDpvt->ind) {
			case BO_SHOW_INFO: 
				/*do something */
				/*precord->val = 0; */
				break;
			default: break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}


static long devBiTSSC_init_record(biRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;

	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBiTSSC (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiTSSC (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, BI_GPIO_IN_STR)) {
		pDpvt->ind = BI_GPIO_IN;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */	
	}
	else if(!strcmp(arg0, BI_TEST_2_STR))
		pDpvt->ind = BI_TEST_2;

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devBiTSSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiTSSC_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devBiTSSC_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_TSSC *pTSSC;
	uint32 nCh=0;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pTSSC = (ST_TSSC*)pSTDdev->pUser;

	nCh = pDpvt->n32Arg0; /* port ID */

	switch(pDpvt->ind) 
	{
		case BI_GPIO_IN: 
			precord->val = (pTSSC->pBaseAddr->gpGPIO_IN & (0x1 << nCh ) ) ? 1 : 0 ;
			break;
		case BI_TEST_2: 
			/* precord->val = your value */
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... TSSC \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoTSSC_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			nval = sscanf(precord->out.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoTSSC (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoTSSC (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, AO_LASER_CLOCK_STR))
		pDpvt->ind = AO_LASER_CLOCK;
	else if (!strcmp(arg0, AO_LASER_CLOCK_WIDTH_STR))
		pDpvt->ind = AO_LASER_CLOCK_WIDTH;
	else if (!strcmp(arg0, AO_QSWITCH_DELAY_STR))
		pDpvt->ind = AO_QSWITCH_DELAY;
	else if (!strcmp(arg0, AO_QSWITCH_WIDTH_STR))
		pDpvt->ind = AO_QSWITCH_WIDTH;
	else if (!strcmp(arg0, AO_BG_PULSE_DELAY_STR))
		pDpvt->ind = AO_BG_PULSE_DELAY;
	else if (!strcmp(arg0, AO_BG_PULSE_WIDTH_STR))
		pDpvt->ind = AO_BG_PULSE_WIDTH;
	else if (!strcmp(arg0, AO_BG_PULSE_CNT_STR))
		pDpvt->ind = AO_BG_PULSE_CNT;
	else if (!strcmp(arg0, AO_SW_ON_DEALY_STR))
		pDpvt->ind = AO_SW_ON_DEALY;
	
/*	else if (!strcmp(arg0, AO_TEST_2_STR)) {
		pDpvt->ind = AO_TEST_2;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); // chennel id 
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); // section id 
	}
*/


	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAoTSSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoTSSC_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_TSSC *pTSSC;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;


	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;

	pTSSC = (ST_TSSC*)pSTDdev->pUser;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_LASER_CLOCK:
				qData.pFunc = devTSSC_AO_LASER_CLOCK;
				break;
			case AO_LASER_CLOCK_WIDTH:
				qData.pFunc = devTSSC_AO_LASER_CLOCK_WIDTH;
				break;
			case AO_QSWITCH_DELAY:
				qData.pFunc = devTSSC_AO_QSWITCH_DELAY;
				break;
			case AO_QSWITCH_WIDTH:
				qData.pFunc = devTSSC_AO_QSWITCH_WIDTH;
				break;
			case AO_BG_PULSE_DELAY:
				qData.pFunc = devTSSC_AO_BG_PULSE_DELAY;
				break;
			case AO_BG_PULSE_WIDTH:
				qData.pFunc = devTSSC_AO_BG_PULSE_WIDTH;
				break;
			case AO_BG_PULSE_CNT:
				qData.pFunc = devTSSC_AO_BG_PULSE_CNT;
				break;
			case AO_SW_ON_DEALY:
				qData.pFunc = devTSSC_AO_SW_ON_DEALY;
				break;
			
		
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
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_LASER_CLOCK:
				/*do something */
				break;
			case AO_LASER_CLOCK_WIDTH:
				/*do something */
				break;

			default:
				break;
		}

		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0; /*(0)=>(success ) */
	}

	return -1;
}



static long devAiTSSC_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiTSSC (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiTSSC (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, AI_BTW_LTU_DG535_IN_STR))
		pDpvt->ind = AI_BTW_LTU_DG535_IN;
	
	else if (!strcmp(arg0, AI_INPUT_PULSE_CNT_STR)) {
		pDpvt->ind = AI_INPUT_PULSE_CNT;
	}
	else if (!strcmp(arg0, AI_LASER_OUT_PULSE_CNT_STR)) {
		pDpvt->ind = AI_LASER_OUT_PULSE_CNT;
	}
	else if (!strcmp(arg0, AI_Q_OUT_PULSE_CNT_STR)) {
		pDpvt->ind = AI_Q_OUT_PULSE_CNT;
	}

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAiTSSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiTSSC_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiTSSC_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_TSSC *pTSSC;


	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;
	pTSSC = (ST_TSSC*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_BTW_LTU_DG535_IN: 
			precord->val = pTSSC->pBaseAddr->gpBtwLTU_dg535_input / 100; /* unit: us */
			epicsPrintf("REG_BTW_LTU_DG535_IN:  %d us: 0x%X\n", (unsigned int)precord->val , pTSSC->pBaseAddr->gpBtwLTU_dg535_input );
			break;
		case AI_INPUT_PULSE_CNT: 
			precord->val = pTSSC->pBaseAddr->gpRealPulseCnt; 
			break;
		case AI_LASER_OUT_PULSE_CNT: 
			precord->val = pTSSC->pBaseAddr->gpLaserOutCnt; 
			break;
		case AI_Q_OUT_PULSE_CNT: 
			precord->val = pTSSC->pBaseAddr->gpQswtOutCnt;
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... TSSC \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}



