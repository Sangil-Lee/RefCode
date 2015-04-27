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
#include "sfwMDSplus.h"



/**********************************
user include files
***********************************/
#include "drvERSC.h"

/**********************************
user include files 
***********************************/

#define PRINT_DBPRO_PHASE_INFO	0



#define BO_SHOW_INFO		1
#define BO_TX_C_RUN		2
#define BO_TX_D_RUN		3
#define BO_PRINT_BD		4
#define BO_A_TRIG_MODE		5
#define BO_B_TRIG_MODE		6
#define BO_RX_A_RUN		7
#define BO_RX_B_RUN		8
#define BO_RX_A_ENABLE		9
#define BO_RX_B_ENABLE		10
#define BO_TX_C_ENABLE		11
#define BO_TX_D_ENABLE		12





#define BO_SHOW_INFO_STR	"show_info"
#define BO_TX_C_RUN_STR		"run_c"
#define BO_TX_D_RUN_STR		"run_d"
#define BO_PRINT_BD_STR		"print_bd"
#define BO_A_TRIG_MODE_STR		"aTrig_ext"
#define BO_B_TRIG_MODE_STR		"bTrig_ext"
#define BO_RX_A_RUN_STR		"run_a"
#define BO_RX_B_RUN_STR		"run_b"
#define BO_RX_A_ENABLE_STR		"enable_a"
#define BO_RX_B_ENABLE_STR		"enable_b"
#define BO_TX_C_ENABLE_STR		"enable_c"
#define BO_TX_D_ENABLE_STR		"enable_d"




#define BI_TEST_1		1
#define BI_TEST_2		2
#define BI_TEST_1_STR	"bi1"
#define BI_TEST_2_STR	"bi2"



#define AO_RECORD_NUM  		1
#define AO_RECORD_SIZE		2
#define AO_RECORD_NUM_STR  		"rec_no"
#define AO_RECORD_SIZE_STR		"rec_size"


#define AI_TEST_1 		1
#define AI_TEST_2 		2
#define AI_TEST_1_STR		"ai1"
#define AI_TEST_2_STR		"ai2"


#define STROUT_TX_C_FILE_NAME		1
#define STROUT_TX_D_FILE_NAME		2
#define STROUT_TX_PATH_NAME		3
#define STROUT_RX_A_TAG		4
#define STROUT_RX_B_TAG		5
#define STROUT_TX_C_FILE_NAME_STR		"C_name"
#define STROUT_TX_D_FILE_NAME_STR	"D_name"
#define STROUT_TX_PATH_NAME_STR		"path"
#define STROUT_RX_A_TAG_STR		"A_tag"
#define STROUT_RX_B_TAG_STR		"B_tag"





/*******************************************************/
/*******************************************************/

static long devAoERSC_init_record(aoRecord *precord);
static long devAoERSC_write_ao(aoRecord *precord);

static long devAiERSC_init_record(aiRecord *precord);
static long devAiERSC_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiERSC_read_ai(aiRecord *precord);

static long devBoERSC_init_record(boRecord *precord);
static long devBoERSC_write_bo(boRecord *precord);

static long devBiERSC_init_record(biRecord *precord);
static long devBiERSC_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBiERSC_read_bi(biRecord *precord);

static long devStringoutERSC_init_record(stringoutRecord *precord);
static long devStringoutERSC_write_stringout(stringoutRecord *precord);


/*******************************************************/

BINARYDSET devBoERSC = { 6,  NULL,  NULL,   devBoERSC_init_record,  NULL,  devBoERSC_write_bo,  NULL };
BINARYDSET  devAoERSC = { 6, NULL, NULL,  devAoERSC_init_record, NULL, devAoERSC_write_ao, NULL };
BINARYDSET  devAiERSC = { 6, NULL, NULL, devAiERSC_init_record, devAiERSC_get_ioint_info, devAiERSC_read_ai, NULL };
BINARYDSET  devBiERSC = { 6, NULL, NULL, devBiERSC_init_record, devBiERSC_get_ioint_info, devBiERSC_read_bi, NULL };
BINARYDSET  devStringoutERSC = { 6, NULL, NULL, devStringoutERSC_init_record, NULL, devStringoutERSC_write_stringout, NULL };


epicsExportAddress(dset, devBoERSC);
epicsExportAddress(dset, devAoERSC);
epicsExportAddress(dset, devAiERSC);
epicsExportAddress(dset, devBiERSC);
epicsExportAddress(dset, devStringoutERSC);




static void devBoERSC_BO_SHOW_INFO(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	printf("REG_REVISION:		0x%x\n", pERSC->pBaseAddr->gpRevision);
	printf("REG_BUILD_DATE:		0x%x\n", pERSC->pBaseAddr->gpBuildDate);
	printf("REG_SERIAL_NUMBER:	0x%x\n", pERSC->pBaseAddr->gpSerialNum);
	printf("REG_READ_WRITE_REG:	0x%x\n", pERSC->pBaseAddr->gpReadWrite);
	printf("REG_CONTROL:		0x%X\n", pERSC->pBaseAddr->gpBoardControl);

	printf("gpIntMask:		0x%X\n",  pERSC->pBaseAddr->gpIntMask);
	printf("gpIntStatus:		0x%X\n",  pERSC->pBaseAddr->gpIntStatus);
	printf("gpA_RX_Desc:	0x%X\n",  pERSC->pBaseAddr->gpA_RX_Desc);
	printf("gpB_RX_Desc:	0x%X\n",  pERSC->pBaseAddr->gpB_RX_Desc);
	printf("gpC_TX_Desc:	0x%X\n",  pERSC->pBaseAddr->gpC_TX_Desc);
	printf("gpD_TX_Desc:	0x%X\n",  pERSC->pBaseAddr->gpD_TX_Desc);
	printf("gpVfbc1_xx_cnt:  0x%08X\t0x%08X\t0x%08X\n",  pERSC->pBaseAddr->gpVfbc1_tloop_cnt, 
												pERSC->pBaseAddr->gpVfbc1_txfer_cnt,
												pERSC->pBaseAddr->gpVfbc1_tfread_cnt);
	printf("gpVfbc2_xx_cnt:  0x%08X\t0x%08X\n",  pERSC->pBaseAddr->gpVfbc2_ttrg_cnt,
										pERSC->pBaseAddr->gpVfbc2_txfer_cnt);
	printf("gpVfbc3_xx_cnt:  0x%08X\t0x%08X\t0x%08X\n",  pERSC->pBaseAddr->gpVfbc3_tloop_cnt, 
												pERSC->pBaseAddr->gpVfbc3_txfer_cnt,
												pERSC->pBaseAddr->gpVfbc3_tfread_cnt);
	printf("gpVfbc4_xx_cnt:  0x%08X\t0x%08X\n",  pERSC->pBaseAddr->gpVfbc4_ttrg_cnt,
										pERSC->pBaseAddr->gpVfbc4_txfer_cnt);

	printf("Channel mask: 		0x%x\n", pERSC->ch_mask);
	printf("Record number: 		%d\n", pERSC->recordNum);
	printf("Record size: 		%dMB\n", pERSC->recordSize / 1024 / 1024);

	printf("mapping pointer: \n");
        printf("\t pTxC_BD: %p \n", pERSC->pTxC_BD);
        printf("\t pTxC_DATA: %p \n", pERSC->pTxC_DATA);
        printf("\t pTxD_BD: %p \n", pERSC->pTxD_BD);
        printf("\t pTxD_DATA: %p \n", pERSC->pTxD_DATA);
        printf("\t pRxA_BD: %p \n", pERSC->pRxA_BD);
        printf("\t pRxB_BD: %p \n", pERSC->pRxB_BD);
        printf("\t pRxA_DATA: %p \n", pERSC->pRxA_DATA);
        printf("\t pRxB_DATA: %p \n", pERSC->pRxB_DATA);


	epicsPrintf("%s: %s ( %d )\n", pSTDdev->taskName, precord->name, (int)pParam->setValue);
}
	
static void devBoERSC_BO_RX_A_RUN (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_ADC_A_RUN;
		printf("ADC 'A' start\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_ADC_A_RUN;
		printf("ADC 'A' stop\n");
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devBoERSC_BO_RX_B_RUN (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_ADC_B_RUN;
		printf("ADC 'B' start\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_ADC_B_RUN;
		printf("ADC 'B' stop\n");
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}


static void devBoERSC_BO_TX_C_RUN (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_DAC_C_RUN;
//		pERSC->pBaseAddr->gpBoardControl  = 0x00800000;
		printf("DAC 'C' start\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_DAC_C_RUN;
//		pERSC->pBaseAddr->gpBoardControl  = SWAP32(0x0);
		printf("DAC 'C' stop\n");
	}

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devBoERSC_BO_TX_D_RUN (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_DAC_D_RUN;
//		pERSC->pBaseAddr->gpBoardControl  = 0x00200000;
		printf("DAC 'D' start\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_DAC_D_RUN;
//		pERSC->pBaseAddr->gpBoardControl  = 0x0;
		printf("DAC 'D' stop\n");
	}

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devBoERSC_BO_PRINT_BD(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;

	drvERSC_print_BufferDescriptor( pSTDdev);

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devBoERSC_BO_A_TRIG_MODE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_RX_A_EXT_TRIG;
		printf("ADC 'A' use external trigger.\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_RX_A_EXT_TRIG;
		printf("ADC 'A' use internal trigger.\n");
	}

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devBoERSC_BO_B_TRIG_MODE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_RX_B_EXT_TRIG;
		printf("ADC 'B' use external trigger.\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_RX_B_EXT_TRIG;
		printf("ADC 'B' use internal trigger.\n");
	}

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devBoERSC_BO_RX_A_ENABLE (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->ch_mask |= BCR_ADC_A_RUN;
		printf("ADC 'A' enable\n");
	}
	else {
		pERSC->ch_mask  &= ~BCR_ADC_A_RUN;
		printf("ADC 'A' disable\n");
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devBoERSC_BO_RX_B_ENABLE (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->ch_mask |= BCR_ADC_B_RUN;
		printf("ADC 'B' enable\n");
	}
	else {
		pERSC->ch_mask  &= ~BCR_ADC_B_RUN;
		printf("ADC 'B' disable\n");
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devBoERSC_BO_TX_C_ENABLE (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->ch_mask |= BCR_DAC_C_RUN;
		printf("DAC 'C' enable\n");
	}
	else {
		pERSC->ch_mask  &= ~BCR_DAC_C_RUN;
		printf("DAC 'C' disable\n");
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}
static void devBoERSC_BO_TX_D_ENABLE (ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( (int)pParam->setValue ) {
		pERSC->ch_mask |= BCR_DAC_D_RUN;
		printf("DAC 'D' enable\n");
	}
	else {
		pERSC->ch_mask  &= ~BCR_DAC_D_RUN;
		printf("DAC 'D' disable\n");
	}
	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}





static void devAoERSC_AO_RECORD_NUM(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	pERSC->recordNum = (unsigned int)pParam->setValue;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

/* input value is MB unit, so we must to unit conversion */
static void devAoERSC_AO_RECORD_SIZE(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	pERSC->recordSize = (unsigned int)pParam->setValue * 1024 * 1024 ;
//	pERSC->recordSize = 16 * 1024 ;

	epicsPrintf("%s ( %d )\n", precord->name, (int)pParam->setValue);
}

static void devStringoutERSC_TX_C_FILE_NAME(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) ) {
		epicsPrintf("%s must be 'Idle'.\n", pSTDdev->taskName);
		return;
	}

	strcpy( pERSC->strTxC_fileName, pParam->setStr);
	if( drvERSC_load_pattern(pSTDdev, CH_TX_C) == WR_ERROR ) {
		epicsPrintf("%s ( load '%s' ... failed! )\n", precord->name, pParam->setStr);
		notify_error(1, "'%s' failed!" ,  pParam->setStr);
	} else {
		epicsPrintf("%s ( load '%s' ... succeed! )\n", precord->name, pParam->setStr);
		notify_error(1, "'%s' ok." ,  pParam->setStr);
		}

}

static void devStringoutERSC_TX_D_FILE_NAME(ST_execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) ) {
		epicsPrintf("%s must be 'Idle'.\n", pSTDdev->taskName);
		return;
	}

	strcpy( pERSC->strTxD_fileName, pParam->setStr);
	if( drvERSC_load_pattern(pSTDdev, CH_TX_D) == WR_ERROR ) {
		epicsPrintf("%s ( load '%s' ... failed! )\n", precord->name, pParam->setStr);
		notify_error(1, "'%s' failed!" ,  pParam->setStr);
	} else {
		epicsPrintf("%s ( load '%s' ... succeed! )\n", precord->name, pParam->setStr);
		notify_error(1, "'%s' ok." ,  pParam->setStr);
	}

}
static void devStringoutERSC_TX_PATH_NAME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	epicsPrintf("Previous path: %s\n", pERSC->strTx_filePath);
	strcpy( pERSC->strTx_filePath, pParam->setStr);
	epicsPrintf("Current path: %s\n", pERSC->strTx_filePath);

}
static void devStringoutERSC_STROUT_RX_A_TAG(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;
	char buffer[100], fbits[100];

	if( strcmp(pERSC->strTagName[CH_RX_A], pParam->setStr) != 0 ) 
	{
		strcpy( pERSC->strTagName[CH_RX_A], pParam->setStr);
		sprintf(buffer, "%s:RAW", pParam->setStr);
		sprintf(fbits, "%s:FBITS", pParam->setStr);
		epicsPrintf("Current Port A RAW name: %s, FBITS: %s\n", buffer, fbits);

		// 3-2. add nodes to Tree object: last argument is temporary file name.
		pERSC->pSfwTreeNode[CH_RX_A] = sfwTreeAddNodeWithSamplingRate (pERSC->pSfwTree, buffer, sfwTreeNodeWaveform, sfwTreeNodeInt16, NULL, (epicsUInt32)INIT_SAMPLE_CLOCK_A);
		// 3-2-1. add nodes for scale to Tree object
		pERSC->pSfwTreeScaleNode[CH_RX_A] = sfwTreeAddNode (pERSC->pSfwTree, fbits, sfwTreeNodeValue, sfwTreeNodeFloat64, NULL);
	}	

}
static void devStringoutERSC_STROUT_RX_B_TAG(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;
	char buffer[100], fbits[100];

	if( strcmp(pERSC->strTagName[CH_RX_B], pParam->setStr) != 0 ) 
	{
		strcpy( pERSC->strTagName[CH_RX_B], pParam->setStr);
		sprintf(buffer, "%s:RAW", pParam->setStr);
		sprintf(fbits, "%s:FBITS", pParam->setStr);
		epicsPrintf("Current Port B RAW name: %s, FBITS: %s\n", buffer, fbits);

		pERSC->pSfwTreeNode[CH_RX_B] = sfwTreeAddNodeWithSamplingRate (pERSC->pSfwTree, buffer, sfwTreeNodeWaveform, sfwTreeNodeInt16, NULL, (epicsUInt32)INIT_SAMPLE_CLOCK_B);
		pERSC->pSfwTreeScaleNode[CH_RX_B] = sfwTreeAddNode (pERSC->pSfwTree, fbits, sfwTreeNodeValue, sfwTreeNodeFloat64, NULL);
	}	
}





/*********************************************************
 initial setup of records 
*********************************************************/
static long devBoERSC_init_record(boRecord *precord)
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
				recGblRecordError(S_db_badField, (void*) precord,  "devBoERSC (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoERSC (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, BO_SHOW_INFO_STR)) 
		pDpvt->ind = BO_SHOW_INFO;

	else if (!strcmp(arg0, BO_TX_C_RUN_STR)) 
		pDpvt->ind = BO_TX_C_RUN;
	else if (!strcmp(arg0, BO_TX_D_RUN_STR)) 
		pDpvt->ind = BO_TX_D_RUN;
	else if (!strcmp(arg0, BO_PRINT_BD_STR)) 
		pDpvt->ind = BO_PRINT_BD;
	else if (!strcmp(arg0, BO_A_TRIG_MODE_STR)) 
		pDpvt->ind = BO_A_TRIG_MODE;
	else if (!strcmp(arg0, BO_B_TRIG_MODE_STR)) 
		pDpvt->ind = BO_B_TRIG_MODE;
	else if (!strcmp(arg0, BO_RX_A_RUN_STR)) 
		pDpvt->ind = BO_RX_A_RUN;
	else if (!strcmp(arg0, BO_RX_B_RUN_STR)) 
		pDpvt->ind = BO_RX_B_RUN;

	else if (!strcmp(arg0, BO_RX_A_ENABLE_STR)) 
		pDpvt->ind = BO_RX_A_ENABLE;
	else if (!strcmp(arg0, BO_RX_B_ENABLE_STR)) 
		pDpvt->ind = BO_RX_B_ENABLE;
	else if (!strcmp(arg0, BO_TX_C_ENABLE_STR)) 
		pDpvt->ind = BO_TX_C_ENABLE;
	else if (!strcmp(arg0, BO_TX_D_ENABLE_STR)) 
		pDpvt->ind = BO_TX_D_ENABLE;
	
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBoERSC_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoERSC_write_bo(boRecord *precord)
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
				qData.pFunc = devBoERSC_BO_SHOW_INFO;
				break;
			case BO_TX_C_RUN: qData.pFunc = devBoERSC_BO_TX_C_RUN; break;
			case BO_TX_D_RUN: qData.pFunc = devBoERSC_BO_TX_D_RUN; break;
			case BO_PRINT_BD: qData.pFunc = devBoERSC_BO_PRINT_BD; break;
			case BO_A_TRIG_MODE: qData.pFunc = devBoERSC_BO_A_TRIG_MODE; break;
			case BO_B_TRIG_MODE: qData.pFunc = devBoERSC_BO_B_TRIG_MODE; break;
			case BO_RX_A_RUN: qData.pFunc = devBoERSC_BO_RX_A_RUN; break;
			case BO_RX_B_RUN: qData.pFunc = devBoERSC_BO_RX_B_RUN; break;
			case BO_RX_A_ENABLE: qData.pFunc = devBoERSC_BO_RX_A_ENABLE; break;
			case BO_RX_B_ENABLE: qData.pFunc = devBoERSC_BO_RX_B_ENABLE; break;
			case BO_TX_C_ENABLE: qData.pFunc = devBoERSC_BO_TX_C_ENABLE; break;
			case BO_TX_D_ENABLE: qData.pFunc = devBoERSC_BO_TX_D_ENABLE; break;
			

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


static long devBiERSC_init_record(biRecord *precord)
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
						  "devBiERSC (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBiERSC (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, BI_TEST_1_STR)) {
		pDpvt->ind = BI_TEST_1;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */	
	}
	else if(!strcmp(arg0, BI_TEST_2_STR))
		pDpvt->ind = BI_TEST_2;

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devBiERSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devBiERSC_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devBiERSC_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ERSC *pERSC;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	pERSC = (ST_ERSC*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case BI_TEST_1: 
			/* precord->val = your value */
			break;
		case BI_TEST_2: 
			/* precord->val = your value */
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... ERSC \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

static long devAoERSC_init_record(aoRecord *precord)
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
						  "devAoERSC (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoERSC (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if (!strcmp(arg0, AO_RECORD_NUM_STR))
		pDpvt->ind = AO_RECORD_NUM;
	else if (!strcmp(arg0, AO_RECORD_SIZE_STR))
		pDpvt->ind = AO_RECORD_SIZE;
	
/*	else if (!strcmp(arg0, AO_TEST_2_STR)) {
		pDpvt->ind = AO_TEST_2;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); // chennel id 
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); // section id 
	}
*/


	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAoERSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoERSC_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_ERSC *pERSC;
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

	pERSC = (ST_ERSC*)pSTDdev->pUser;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_RECORD_NUM:
				qData.pFunc = devAoERSC_AO_RECORD_NUM;
				break;
			case AO_RECORD_SIZE:
				qData.pFunc = devAoERSC_AO_RECORD_SIZE;
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
			case AO_RECORD_NUM:
				/*do something */
				break;
			case AO_RECORD_SIZE:
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



static long devAiERSC_init_record(aiRecord *precord)
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
						  "devAiERSC (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiERSC (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, AI_TEST_1_STR))
		pDpvt->ind = AI_TEST_1;
	
	else if (!strcmp(arg0, AI_TEST_2_STR)) {
		pDpvt->ind = AI_TEST_2;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR!! devAiERSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiERSC_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
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

static long devAiERSC_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_ERSC *pERSC;


	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;
	pERSC = (ST_ERSC*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_TEST_1: 
			/* precord->val = your value */
			break;
		case AI_TEST_2: 
			/* precord->val = your value */
			break;
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... ERSC \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}


static long devStringoutERSC_init_record(stringoutRecord *precord)
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
						  "devStringout (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringout (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(arg0, STROUT_TX_C_FILE_NAME_STR)) {
		pDpvt->ind = STROUT_TX_C_FILE_NAME;
	}  
	else if (!strcmp(arg0, STROUT_TX_D_FILE_NAME_STR))
		pDpvt->ind = STROUT_TX_D_FILE_NAME;
	else if (!strcmp(arg0, STROUT_TX_PATH_NAME_STR))
		pDpvt->ind = STROUT_TX_PATH_NAME;
	else if (!strcmp(arg0, STROUT_RX_A_TAG_STR))
		pDpvt->ind = STROUT_RX_A_TAG;
	else if (!strcmp(arg0, STROUT_RX_B_TAG_STR))
		pDpvt->ind = STROUT_RX_B_TAG;

	else {
		pDpvt->ind = -1;
		epicsPrintf("[SFW] devStringoutERSC_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}


static long devStringoutERSC_write_stringout(stringoutRecord *precord)
{
	int len;
	ST_dpvt 	   *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_ERSC *pERSC;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData		   qData;


	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}
	
	pSTDdev 		= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev 	= pSTDdev;
	qData.param.precord 	= (struct dbCommon *)precord;
	len = strlen(precord->val);
	if( len > SIZE_STRINGOUT_VAL ) 
	{
		printf("string out size limited, %d\n", len);
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}
	strcpy( qData.param.setStr, precord->val );
	
	pERSC = (ST_ERSC*)pSTDdev->pUser;


 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif			
		switch(pDpvt->ind) 
		{
			case STROUT_TX_C_FILE_NAME:
				qData.pFunc = devStringoutERSC_TX_C_FILE_NAME;
				break;
			case STROUT_TX_D_FILE_NAME:
				qData.pFunc = devStringoutERSC_TX_D_FILE_NAME;
				break;
			case STROUT_TX_PATH_NAME:
				qData.pFunc = devStringoutERSC_TX_PATH_NAME;
				break;

			case STROUT_RX_A_TAG:
				qData.pFunc = devStringoutERSC_STROUT_RX_A_TAG;
				break;
			case STROUT_RX_B_TAG:
				qData.pFunc = devStringoutERSC_STROUT_RX_B_TAG;
				break;



			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId, (void*) &qData, sizeof(ST_threadQueueData));

		return 0; /*(-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_DBPRO_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1; /*(-1,0)=>(failure,success)*/
}



