
#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"

#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drvACQ196.h" 
#include "acqHTstream.h"
#include "myMDSplus.h"
#include "acqPvString.h"


#if 0
#define DEBUG
#endif


/*******************************************************/
/*******************************************************/

static long devAoACQ196Control_init_record(aoRecord *precord);
static long devAoACQ196Control_write_ao(aoRecord *precord);

static long devStringoutACQ196Control_init_record(stringoutRecord *precord);
static long devStringoutACQ196Control_write_stringout(stringoutRecord *precord);

static long devAiACQ196RawRead_init_record(aiRecord *precord);
static long devAiACQ196RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiACQ196RawRead_read_ai(aiRecord *precord);

static long devBoACQ196Control_init_record(boRecord *precord);
static long devBoACQ196Control_write_bo(boRecord *precord);

static long devMbbiACQ196_init_record(mbbiRecord *precord);
static long devMbbiACQ196_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
static long devMbbiACQ196_read_mbbi(mbbiRecord *precord);


/*******************************************************/


struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoACQ196Control = {
    5,
    NULL,
    NULL,
    devBoACQ196Control_init_record,
    NULL,
    devBoACQ196Control_write_bo
};
epicsExportAddress(dset, devBoACQ196Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoACQ196Control = {
	6,
	NULL,
	NULL,
	devAoACQ196Control_init_record,
	NULL,
	devAoACQ196Control_write_ao,
	NULL
};
epicsExportAddress(dset, devAoACQ196Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutACQ196Control = {
	6,
	NULL,
	NULL,
	devStringoutACQ196Control_init_record,
	NULL,
	devStringoutACQ196Control_write_stringout,
	NULL
};
epicsExportAddress(dset, devStringoutACQ196Control);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiACQ196RawRead = {
	6,
	NULL,
	NULL,
	devAiACQ196RawRead_init_record,
	devAiACQ196RawRead_get_ioint_info,
	devAiACQ196RawRead_read_ai,
	NULL
};
epicsExportAddress(dset, devAiACQ196RawRead);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_write;
	DEVSUPFUN	special_linconv;
} devMbbiACQ196 = {
	5,
	NULL,
	NULL,
	devMbbiACQ196_init_record,
	devMbbiACQ196_get_ioint_info,
	devMbbiACQ196_read_mbbi 
};
epicsExportAddress(dset, devMbbiACQ196);



/*******************************************************/
/*******************************************************/



unsigned long long int start, stop, _interval;



static void devACQ196_BO_DATA_PARSING(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

/*	pSTDdev->ST_Base.dT0 = (float)pParam->setValue; */
/*	dbProc_call_LSAVE_start(); */
	if( (pSTDdev->StatusDev & TASK_IN_PROGRESS ) ||
		( pSTDdev->StatusDev & TASK_POST_PROCESS)  ) 
	{
		epicsPrintf("\n>>> devACQ196_BO_DATA_PARSING(%s) : DAQ should be stopped\n", pSTDdev->taskName);
		return;
	}
	else {
		pSTDdev->StatusDev |= TASK_POST_PROCESS;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		if( !proc_dataChannelization( pSTDdev ) )
		{
			epicsPrintf("\n>>> devACQ196_BO_DATA_PARSING(%s) : proc_dataChannelization() ... fail\n", pSTDdev->taskName);
			pSTDdev->StatusDev &= ~TASK_POST_PROCESS;
			pSTDdev->StatusDev |= DEV_CHANNELIZATION_FAIL;			
			scanIoRequest(pSTDdev->ioScanPvt_status);
			pMaster->ErrorAdmin |= ERROR_AFTER_SHOT_PROCESS;
			scanIoRequest(pMaster->ioScanPvt_status);
			return;
		}
		
	}
	pSTDdev->StatusDev &= ~TASK_POST_PROCESS;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	epicsPrintf(" %s %s (%s)  OK!\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}




static void devACQ196_AO_TEST_PUT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

/*	pSTDdev->dT0 = (float)pParam->setValue; */
/*	dbProc_call_LSAVE_start(); */
/*	dbProc_call_RESET_shotseq_started(); */

	
	epicsPrintf("control thread (test PUT): %s %s (%s), val: %f\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), (float)pParam->setValue);
}

static void devACQ196_AO_CARD_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	switch( (uint32)pParam->setValue ) {
		case CARD_MODE_SINGLE: 			
/*			if( acq196_set_SingleMode( pSTDdev ) == WR_ERROR ) {
				epicsPrintf("\nERROR: acq196_set_SingleMode (%s)\n", precord->name );
				return ;
			} */
			pAcq196->nCardMode = CARD_MODE_SINGLE;
			epicsPrintf("Card mode: %s (Single)\n", precord->name );
/*			printlog("Card mode: %s (Single)\n", precord->name ); */
			break;
		case CARD_MODE_MASTER:
/*			if( acq196_set_MasterMode( pAcq196 ) == WR_ERROR ) {
				epicsPrintf("\nERROR: acq196_set_MasterMode (%s)\n", precord->name );
				return ;
			} */
			pAcq196->nCardMode = CARD_MODE_MASTER;
			epicsPrintf("Card mode: %s (Master)\n", precord->name );
/*			printlog("Card mode: %s (Master)\n", precord->name ); */
			break;
		case CARD_MODE_SLAVE:
/*			if( acq196_set_SlaveMode( pAcq196 ) == WR_ERROR ) {
				epicsPrintf("\nERROR: acq196_set_SlaveMode (%s)\n", precord->name );
				return ;
			} */
			pAcq196->nCardMode = CARD_MODE_SLAVE;
			epicsPrintf("Card mode: %s (Slave)\n", precord->name );
/*			printlog("Card mode: %s (Slave)\n", precord->name ); */
			break;
		default:
			epicsPrintf("\nERROR: %s (%d)\n", precord->name, (uint32)pParam->setValue );
			printlog("\nERROR: %s (%d)\n", precord->name, (uint32)pParam->setValue );
			epicsPrintf("Current card mode: %s (%d)\n", precord->name, pAcq196->nCardMode  );
			break;
		}
	
}
	
static void devACQ196_AO_DIVIDER(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if( pParam->setValue == 0 ) {
		epicsPrintf("\n>>> %s: must be bigger than zero ( %f )\n", precord->name, pParam->setValue);
		return;
	}
	pAcq196->clockDivider = (uint32)pParam->setValue;
	epicsPrintf("\n>>> %s: %s ( %d )\n", pSTDdev->taskName, precord->name, pAcq196->clockDivider);
}
	
static void devACQ196_AO_MAXDMA(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	pAcq196->n32_maxdma = (uint32)pParam->setValue;
	if( acq196_set_maxdma(pSTDdev) == WR_OK ) {
		epicsPrintf("\n>>> %s: %s ( %d )\n", pSTDdev->taskName, precord->name, pAcq196->n32_maxdma);
	}
}

static void devACQ196_AO_SET_CLEAR_STATUS(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	pSTDdev->StatusDev &= ~TASK_STANDBY;
	pSTDdev->StatusDev &= ~DEV_CHANNELIZATION_FAIL;
	pSTDdev->StatusDev &= ~DEV_STORAGE_FAIL;
	pSTDdev->StatusDev &= ~DEV_STORAGE_FINISH;
	pSTDdev->StatusDev &= ~DEV_READY_TO_SAVE;
/*	pSTDdev->StatusDev |= TASK_ARM_DISABLED;
	pSTDdev->StatusDev |= TASK_DAQ_STOPED; */
/*	scanIoRequest(pSTDdev->ioScanPvt_status); */

/*	notify_sys_ready(); */
	notify_refresh_master_status();

	epicsPrintf("control thread (Clear Status): %s : %s\n", pSTDdev->taskName, precord->name );
}


static void devACQ196_AO_SAMPLING(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED) 
	{
		epicsPrintf("ERROR! %s: ADC must stop! \n", pSTDdev->taskName );		
	}
	else {
		drvACQ196_set_samplingRate(pSTDdev, (int)pParam->setValue);
		drvACQ196_notify_InitCondition( pSTDdev );
	
		epicsPrintf("%s: %s: %dKHz\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.nSamplingRate / 1000);
	}
	
}

static void devACQ196_AO_SET_T0(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
	if(pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR! %s: ADC must stop! \n", pSTDdev->taskName );
	}
	else {
		pSTDdev->ST_Base.dT0[0] = (float)pParam->setValue;
		epicsPrintf("%s: %s: %f sec\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dT0[0]);
/*		printlog("%s: %s: %f sec\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dT0[0]); */
		pAcq196->boardConfigure |= ACQ196_SET_T0;
		drvACQ196_notify_InitCondition( pSTDdev);
/*
		if( drvACQ196_check_AllStorageFinished() == WR_OK  ) {
			pSTDdev->mds_dT0 = pSTDdev->ST_Base.dT0[0];
		} 
		else {
			epicsPrintf("ERROR! %s: current mds T0 is %f sec \n", pSTDdev->taskName, pSTDdev->mds_dT0[0]);
		}
*/		
	}
	
}

static void devACQ196_AO_SET_T1(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if(pSTDdev->StatusDev & TASK_ARM_ENABLED ) 
	{
		epicsPrintf("ERROR! %s: ADC must stop! \n", pSTDdev->taskName );
	}
	else {
		pSTDdev->ST_Base.dT1[0] = (float)pParam->setValue;
		epicsPrintf("%s: %s: %f sec\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dT1[0]);
/*		printlog("%s: %s: %f sec\n", pSTDdev->taskName, precord->name, pSTDdev->ST_Base.dT1[0]); */
		pAcq196->boardConfigure |= ACQ196_SET_T1;
		drvACQ196_notify_InitCondition( pSTDdev);
/*
		if( drvACQ196_check_AllStorageFinished() == WR_OK  ) {
			pSTDdev->mds_dT1 = pSTDdev->ST_Base.dT1;
		} 
		else {
			epicsPrintf("ERROR! %s: current mds T1 is %f sec \n", pSTDdev->taskName, pSTDdev->mds_dT1);
		}
*/
	}
	
}

static void devACQ196_AO_SET_EXTERCLK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) {
		epicsPrintf("ERROR! %s : ADC started!", pSTDdev->taskName );
		return;
	}

	/* val == 1 exteranl */
	pAcq196->clockSource = CLOCK_EXTERNAL;
	if( !drvACQ196_set_cardMode(pSTDdev) ) {
		epicsPrintf("\n>>> devACQ196_AO_SET_EXTERCLK() : drvACQ196_set_cardMode()... failed\n");
		return ;
	}

/*	epicsPrintf("control thread (Exteranl CLK): %s %s (%s)\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf()); */
	epicsPrintf("%s: %s: use External Clock\n", pSTDdev->taskName, precord->name );
/*	printlog("%s: External Clock\n", pSTDdev->taskName, precord->name ); */


}

static void devACQ196_AO_SET_INTERCLK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) {
		epicsPrintf("ERROR! %s : ADC started!", pSTDdev->taskName );
		return;
	}

	/* val == 0 interanl */
	pAcq196->clockSource = CLOCK_INTERNAL;
	if( !drvACQ196_set_cardMode(pSTDdev) ) {
		epicsPrintf("\n>>> devACQ196_AO_SET_INTERCLK() : drvACQ196_set_cardMode()... failed\n");
		return ;
	}

	epicsPrintf("%s: %s: use Internal Clock\n", pSTDdev->taskName, precord->name );
/*	printlog("%s: Internal Clock\n", pSTDdev->taskName, precord->name ); */
}


static void devACQ196_AO_DEV_ARMING(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	if( (int)pParam->setValue == 1 ) /* in case of arming  */
	{
	/* arming can do only admin task!!!! */
	/*
		if( drvACQ196_openFile(pSTDdev) == WR_ERROR) {
			printf("\n>>> %s: drvACQ196_openFile() ERROR", pSTDdev->taskName);
			return ;
		}
		pSTDdev->StatusDev &= ~DEV_STORAGE_FINISH; 

		if( drvACQ196_ARM_enable( pSTDdev ) == WR_ERROR )
		{
			epicsPrintf("\n>>> devACQ196_AO_DEV_ARMING : drvACQ196_ARM_enable... failed\n");
			return ;
		}
	*/

	}
	else 
	{
		if( pAcq196->fpRaw != NULL )
			fclose(pAcq196->fpRaw);
		pAcq196->fpRaw = NULL;
		
		acq196_set_ABORT(pSTDdev);
		
		printf("\"%s\" setAbort. ok.\n", pSTDdev->taskName );

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
	epicsPrintf("%s: %s (%s)\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf());
}

static void devACQ196_AO_DEV_RUN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	if( (int)pParam->setValue == 1 ) /* in case of arming  */
	{
		if( drvACQ196_check_Run_condition(pSTDdev)  == WR_ERROR) 
			return;
		
		if( drvACQ196_RUN_start(pSTDdev) == WR_ERROR )
		{	
			printf("\n>>> devACQ196_AO_DEV_RUN : drvACQ196_RUN_start() ... fail\n");
			return;
		}
		pSTDdev->StatusDev &= ~DEV_STORAGE_FINISH;  /* 2009. 10. 16 : storage status clear for check end of saving signal...*/

	}
	else 
	{
		if( drvACQ196_check_Stop_condition(pSTDdev)  == WR_ERROR) 
			return;

		drvACQ196_RUN_stop(pSTDdev);

	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
	printf("%s: %s (%s)\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf());
}

static void devACQ196_AO_LOCAL_STORAGE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

	start = wf_getCurrentUsec();
	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
	scanIoRequest(pSTDdev->ioScanPvt_status);

	if( proc_sendData2Tree( pSTDdev ) == WR_ERROR )
	{
		epicsPrintf("\n>>> devACQ196_AO_LOCAL_STORAGE(%s) : proc_sendData2Tree() ... fail\n", pSTDdev->taskName);
		pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
		pSTDdev->StatusDev |= DEV_STORAGE_FAIL;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		pMaster->ErrorAdmin |= ERROR_DATA_PUT_STORAGE;
		scanIoRequest(pMaster->ioScanPvt_status);
		epicsPrintf("\n>>> %s: Status(0x%X) \n", pSTDdev->taskName, pSTDdev->StatusDev);
		return ;
	}
	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
	pSTDdev->StatusDev |= DEV_STORAGE_FINISH;

	stop = wf_getCurrentUsec();
	_interval = wf_intervalUSec(start, stop);
	epicsPrintf("%s: Transmit time: %d sec\n", pSTDdev->taskName, (int)(1.E-3 * (double)_interval)/1000 );
/*	printlog("%s: Transmit time: %d sec\n", pSTDdev->taskName, (int)(1.E-3 * (double)_interval)/1000 ); */

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

static void devACQ196_AO_REMOTE_STORAGE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

	pSTDdev->StatusDev = TASK_NOT_INIT;
	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
/*	pSTDdev->StatusDev |= TASK_ARM_DISABLED;
	pSTDdev->StatusDev |= TASK_DAQ_STOPED; */
	scanIoRequest(pSTDdev->ioScanPvt_status);

	start = wf_getCurrentUsec();
	if( proc_sendData2Tree( pSTDdev ) == WR_ERROR)
	{
		epicsPrintf("\n>>> devACQ196_AO_REMOTE_STORAGE(%s) : proc_sendData2Tree() ... fail\n", pSTDdev->taskName);
		pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
		pSTDdev->StatusDev |= DEV_STORAGE_FAIL;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		pMaster->ErrorAdmin |= ERROR_DATA_PUT_STORAGE;
		scanIoRequest(pMaster->ioScanPvt_status);
		epicsPrintf("\n>>> %s: Status(0x%X) \n", pSTDdev->taskName, pSTDdev->StatusDev);
		return ;
	}
	stop = wf_getCurrentUsec();

	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
	pSTDdev->StatusDev |= DEV_STORAGE_FINISH;

	_interval = wf_intervalUSec(start, stop);
	epicsPrintf("%s: Transmit time: %d sec\n", pSTDdev->taskName, (int)(1.E-3 * (double)_interval)/1000 );
	scanIoRequest(pSTDdev->ioScanPvt_status);
}


static void devACQ196_STRINGOUT_TAG(ST_execParam *pParam)
{
	int i;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
	if( pParam->n32Arg0 > (96*MAX_ACQ196) ) {
		epicsPrintf("\n>>> %d,  input channel limit : %d\n", pParam->n32Arg0, 96*MAX_ACQ196);
		return;
	}

/*	strcpy(channelTagName[pParam->n32Arg0], "\\");
	strcat(channelTagName[pParam->n32Arg0], pParam->setStr);
*/
/*	strcpy(channelTagName[pParam->n32Arg0], pParam->setStr); */
	strcpy(pAcq196->strTagName[pParam->n32Arg0], pParam->setStr);
	pAcq196->cTouchTag[pParam->n32Arg0] = 0x1;

	pAcq196->boardConfigure |= ACQ196_SET_TAG;
	for( i=0; i<96; i++) 
	{
		if( (pAcq196->channelOnOff[i] == 1) && (pAcq196->cTouchTag[i] == 0x0) ) 
		{
			pAcq196->boardConfigure &= ~ACQ196_SET_TAG;
			break;
		}
	}

/*	drvACQ196_notify_InitCondition( pAcq196 ); */
#if 0
	epicsPrintf("control thread (tag Name): %s (%d: %s)\n", pSTDdev->taskName, pParam->n32Arg0, pParam->setStr );
#endif
}
static void devACQ196_STRINGOUT_IP_PORT(ST_execParam *pParam)
{
/*	ST_STD_device *pSTDdev = pParam->pSTDdev;
	strcpy( pSTDdev->treeIPport, pParam->setStr ); */

/*
	if( pAcq196->StatusMds & MDS_CONNECTED )  mds_Disconnect(pAcq196);
	
	mds_Connect(pAcq196 );
*/
	
}
static void devACQ196_STRINGOUT_TREE_NAME(ST_execParam *pParam)
{
/*	ST_STD_device *pSTDdev = pParam->pSTDdev;

	strcpy( pSTDdev->treeName, pParam->setStr );	
	epicsPrintf(">>> %s: Tree name \"%s\"\n", pSTDdev->taskName, pSTDdev->treeName ); */

/*	pSTDdev->StatusDev |= EREF_SET_TREE_NAME;
	scanIoRequest(pSTDdev->ioScanPvt_status); */
}





static void devACQ196_AO_R_ONOFF(ST_execParam *pParam)
{
	int i, ntemp, CntOn=0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	int dval = (int)pParam->setValue;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	
	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		pAcq196->channelOnOff[i] = ( dval & ntemp ) ? 1 : 0;

		if( pAcq196->channelOnOff[i] ) {
			CntOn++;
/*			epicsPrintf(" %s:  ch%d ON\n", pSTDdev->taskName, i );
			epicsThreadSleep(0.1); */
		}
	}
	pAcq196->boardConfigure |= ACQ196_SET_R_BITMASK;
	drvACQ196_notify_InitCondition( pSTDdev );
	epicsPrintf("%s: %s:(1-32) on/off - %d/%d\n", pSTDdev->taskName, precord->name, CntOn, 32-CntOn);
}


static void devACQ196_AO_M_ONOFF(ST_execParam *pParam)
{
	int i, ntemp, CntOn=0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	int dval = (int)pParam->setValue;

	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		pAcq196->channelOnOff[32+i] = ( dval & ntemp ) ? 1 : 0;

		if( pAcq196->channelOnOff[32+i] ) {
			CntOn++;
/*			epicsPrintf(" %s:  ch%d ON\n", pSTDdev->taskName, 32+i );
			epicsThreadSleep(0.1); */
		}

	}
	pAcq196->boardConfigure |= ACQ196_SET_M_BITMASK;
	drvACQ196_notify_InitCondition( pSTDdev );
	epicsPrintf("%s: %s:(33-64) on/off - %d/%d\n", pSTDdev->taskName, precord->name, CntOn, 32-CntOn);
}


static void devACQ196_AO_L_ONOFF(ST_execParam *pParam)
{
	int i, ntemp, CntOn=0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	int dval = (int)pParam->setValue;

	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		pAcq196->channelOnOff[64+i] = ( dval & ntemp ) ? 1 : 0;

		if( pAcq196->channelOnOff[64+i] ) {
			CntOn++;
/*			epicsPrintf(" %s:  ch%d ON\n", pSTDdev->taskName, 64+i );
			epicsThreadSleep(0.1); */
		}

	}
	pAcq196->boardConfigure |= ACQ196_SET_L_BITMASK;
	drvACQ196_notify_InitCondition( pSTDdev );
	epicsPrintf("%s: %s:(65-96) on/off - %d/%d\n", pSTDdev->taskName, precord->name, CntOn, 32-CntOn);
}


static void devACQ196_AO_GAIN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ACQ196 *pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	drvACQ196_set_chanGain(pAcq196, pParam->n32Arg0, pParam->setValue);
	epicsPrintf("control thread (gain): %s %s %d (%s)\n", pSTDdev->taskName, precord->name, (int)pParam->setValue,
			                                   epicsThreadGetNameSelf());
}

static void devACQ196_AO_MASK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	drvACQ196_set_channelMask(pSTDdev, pParam->setValue);
	epicsPrintf("control thread (Mask): %s %s %d (%s)\n", pSTDdev->taskName, precord->name, (int)pParam->setValue,
			                                   epicsThreadGetNameSelf());
}

static long devBoACQ196Control_init_record(boRecord *precord)
{
	ST_dpvt *pdevACQ196Controldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	
	int i;
	
		switch(precord->out.type) {
			case INST_IO:
				strcpy(pdevACQ196Controldpvt->recordName, precord->name);
				i = sscanf(precord->out.value.instio.string, "%s %s",
					   pdevACQ196Controldpvt->devName,
						   pdevACQ196Controldpvt->arg0);
	
#if 0
		epicsPrintf("devBoACQ196Control arg num: %d: %s %s\n",i, pdevACQ196Controldpvt->devName, 
									pdevACQ196Controldpvt->arg0);
#endif
				pdevACQ196Controldpvt->pSTDdev = get_STDev_from_name(pdevACQ196Controldpvt->devName);
				if(!pdevACQ196Controldpvt->pSTDdev) {
					recGblRecordError(S_db_badField, (void*) precord, 
							  "devBoACQ196Control (init_record) Illegal INP field: task_name");
					free(pdevACQ196Controldpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			
					   
				break;
			default:
				recGblRecordError(S_db_badField,(void*) precord,
						  "devAoACQ196Control (init_record) Illegal OUT field");
				free(pdevACQ196Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
				
		}
	
	
		if(!strcmp(pdevACQ196Controldpvt->arg0, BO_DATA_PARSING_STR)) {
			pdevACQ196Controldpvt->ind = BO_DATA_PARSING;
		} 
		else {
			pdevACQ196Controldpvt->ind = 0;
			epicsPrintf("ERROR! devBoACQ196Control_init_record: arg0 \"%s\" \n",  pdevACQ196Controldpvt->arg0 );
		}

	
		precord->udf = FALSE;
		precord->dpvt = (void*) pdevACQ196Controldpvt;
		
		return 2;	  /* don't convert */
	}


static long devBoACQ196Control_write_bo(boRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	ST_dpvt		*pdevACQ196Controldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg 	*pControlThreadConfig;
	ST_threadQueueData	qData;



	if(!pdevACQ196Controldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pdevACQ196Controldpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pdevACQ196Controldpvt->ind) {

			case BO_DATA_PARSING:
				qData.pFunc = devACQ196_BO_DATA_PARSING;
				break;

		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
#if 0
		switch(pdevACQ196Controldpvt->ind) {
			case AO_SET_INTERCLK: precord->val = 0; break;
			case AO_SET_EXTERCLK: precord->val = 0; break;
			case AO_DEV_RUN: precord->val = 0; break;
			case AO_DAQ_STOP: precord->val = 0; break;
			case AO_DEV_ARMING: precord->val = 0; break;
			case AO_ADC_STOP: precord->val = 0; break;
			case AO_LOCAL_STORAGE: precord->val = 0; break;
			case AO_REMOTE_STORAGE: precord->val = 0; break;
			case AO_SET_CLEAR_STATUS: precord->val = 0; break;
		}
#endif
		return 2;    /* don't convert */
	}

	return 0;
}


static long devAoACQ196Control_init_record(aoRecord *precord)
{
	ST_dpvt *pdevACQ196Controldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevACQ196Controldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s",
				   pdevACQ196Controldpvt->devName,
			           pdevACQ196Controldpvt->arg0);

#if 0
	epicsPrintf("devAoACQ196Control arg num: %d: %s %s\n",i, pdevACQ196Controldpvt->devName, 
								pdevACQ196Controldpvt->arg0);
#endif
			pdevACQ196Controldpvt->pSTDdev = get_STDev_from_name(pdevACQ196Controldpvt->devName);
			if(!pdevACQ196Controldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoACQ196Control (init_record) Illegal INP field: task_name");
				free(pdevACQ196Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoACQ196Control (init_record) Illegal OUT field");
			free(pdevACQ196Controldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevACQ196Controldpvt->arg0, AO_SAMPLING_STR)) {
		pdevACQ196Controldpvt->ind = AO_SAMPLING;
/*		pdevACQ196Controldpvt->pchannelConfig 
			= (ST_ACQ196_channel*)ellFirst(pdevACQ196Controldpvt->pSTDdev->pchannelConfig); */
	} else if(!strcmp(pdevACQ196Controldpvt->arg0, AO_GAIN_STR)) {
		pdevACQ196Controldpvt->ind = AO_GAIN;
/*		pdevACQ196Controldpvt->pchannelConfig
			= (ST_ACQ196_channel*)ellFirst(pdevACQ196Controldpvt->pSTDdev->pchannelConfig); */



	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_DEV_RUN_STR)) {
		pdevACQ196Controldpvt->ind = AO_DEV_RUN;

	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_DEV_ARMING_STR)) {
		pdevACQ196Controldpvt->ind = AO_DEV_ARMING;


	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_LOCAL_STORAGE_STR)) {
		pdevACQ196Controldpvt->ind = AO_LOCAL_STORAGE;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_REMOTE_STORAGE_STR)) {
		pdevACQ196Controldpvt->ind = AO_REMOTE_STORAGE;

	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_MASK_STR)) {
		pdevACQ196Controldpvt->ind = AO_MASK;

	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_R_ONOFF_STR)) {
		pdevACQ196Controldpvt->ind = AO_R_ONOFF;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_M_ONOFF_STR)) {
		pdevACQ196Controldpvt->ind = AO_M_ONOFF;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_L_ONOFF_STR)) {
		pdevACQ196Controldpvt->ind = AO_L_ONOFF;
	}
/*	else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SHOT_NUMBER_STR)) {
		pdevACQ196Controldpvt->ind = AO_SHOT_NUMBER;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_OP_MODE_STR)) {
		pdevACQ196Controldpvt->ind = AO_OP_MODE;
	}*/
	else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SET_INTERCLK_STR)) {
		pdevACQ196Controldpvt->ind = AO_SET_INTERCLK;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SET_EXTERCLK_STR)) {
		pdevACQ196Controldpvt->ind = AO_SET_EXTERCLK;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SET_T0_STR)) {
		pdevACQ196Controldpvt->ind = AO_SET_T0;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SET_T1_STR)) {
		pdevACQ196Controldpvt->ind = AO_SET_T1;
	}/* else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SET_BLIP_STR)) {
		pdevACQ196Controldpvt->ind = AO_SET_BLIP;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_CREATE_LOCAL_SHOT_STR)) {
		pdevACQ196Controldpvt->ind = AO_CREATE_LOCAL_SHOT;
	}*/ else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_SET_CLEAR_STATUS_STR)) {
		pdevACQ196Controldpvt->ind = AO_SET_CLEAR_STATUS;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_MAXDMA_STR)) {
		pdevACQ196Controldpvt->ind = AO_MAXDMA;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_DIVIDER_STR)) {
		pdevACQ196Controldpvt->ind = AO_DIVIDER;
	} else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_CARD_MODE_STR)) {
		pdevACQ196Controldpvt->ind = AO_CARD_MODE;
	}
	
	
	
	else if (!strcmp(pdevACQ196Controldpvt->arg0, AO_TEST_PUT_STR)) {
		pdevACQ196Controldpvt->ind = AO_TEST_PUT;
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevACQ196Controldpvt;
	
	return 2;     /* don't convert */
}

static long devAoACQ196Control_write_ao(aoRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	ST_dpvt        *pdevACQ196Controldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;



	if(!pdevACQ196Controldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pdevACQ196Controldpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.n32Arg0		= pdevACQ196Controldpvt->pchannelConfig->chanIndex; */
	qData.param.setValue		= precord->val;



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pdevACQ196Controldpvt->ind) {
			case AO_SAMPLING:
				qData.pFunc = devACQ196_AO_SAMPLING;
				break;
			case AO_GAIN:
				qData.pFunc = devACQ196_AO_GAIN;
				break;
			case AO_DEV_RUN:
				qData.pFunc = devACQ196_AO_DEV_RUN;
				break;
			case AO_DEV_ARMING:
				qData.pFunc = devACQ196_AO_DEV_ARMING;
				break;
			case AO_LOCAL_STORAGE:
				qData.pFunc = devACQ196_AO_LOCAL_STORAGE;
				break;
			case AO_REMOTE_STORAGE:
				qData.pFunc = devACQ196_AO_REMOTE_STORAGE;
				break;
			case AO_MASK:
				qData.pFunc = devACQ196_AO_MASK;
				break;
			case AO_R_ONOFF:
				qData.pFunc = devACQ196_AO_R_ONOFF;
				break;
			case AO_M_ONOFF:
				qData.pFunc = devACQ196_AO_M_ONOFF;
				break;
			case AO_L_ONOFF:
				qData.pFunc = devACQ196_AO_L_ONOFF;
				break;
/*			case AO_SHOT_NUMBER:
				qData.pFunc = devACQ196_AO_SHOT_NUMBER;
				break;
			case AO_OP_MODE:
				qData.pFunc = devACQ196_AO_OP_MODE;
*/				break;
			case AO_SET_INTERCLK:
				qData.pFunc = devACQ196_AO_SET_INTERCLK;
				break;
			case AO_SET_EXTERCLK:
				qData.pFunc = devACQ196_AO_SET_EXTERCLK;
				break;
			case AO_SET_T0:
				qData.pFunc = devACQ196_AO_SET_T0;
				break;
			case AO_SET_T1:
				qData.pFunc = devACQ196_AO_SET_T1;
				break;
			case AO_TEST_PUT:
				qData.pFunc = devACQ196_AO_TEST_PUT;
				break;
/*			case AO_SET_BLIP:
				qData.pFunc = devACQ196_AO_SET_BLIP;
				break;
			case AO_CREATE_LOCAL_SHOT:
				qData.pFunc = devACQ196_AO_CREATE_LOCAL_SHOT;
				break;
*/			case AO_SET_CLEAR_STATUS:
				qData.pFunc = devACQ196_AO_SET_CLEAR_STATUS;
				break;
			case AO_MAXDMA:
				qData.pFunc = devACQ196_AO_MAXDMA;
				break;
			case AO_DIVIDER:
				qData.pFunc = devACQ196_AO_DIVIDER;
				break;
			case AO_CARD_MODE:
				qData.pFunc = devACQ196_AO_CARD_MODE;
				break;
				
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
/*		
		switch(pdevACQ196Controldpvt->ind) {
			case AO_SET_INTERCLK: precord->val = 0; break;
			case AO_SET_EXTERCLK: precord->val = 0; break;
			case AO_DEV_RUN: precord->val = 0; break;
			case AO_DEV_ARMING: precord->val = 0; break;
			case AO_LOCAL_STORAGE: precord->val = 0; break;
			case AO_REMOTE_STORAGE: precord->val = 0; break;
			case AO_SET_CLEAR_STATUS: precord->val = 0; break;
		}
*/
		return 2;    /* don't convert */
	}

	return 0;
}


static long devStringoutACQ196Control_init_record(stringoutRecord *precord)
{
	ST_dpvt *pdevStringoutACQ196Controldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevStringoutACQ196Controldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevStringoutACQ196Controldpvt->devName,
			           pdevStringoutACQ196Controldpvt->arg0 ,
			           pdevStringoutACQ196Controldpvt->arg1);

#if 0
	epicsPrintf("devAoACQ196Control arg num: %d: %s %s\n",i, pdevStringoutACQ196Controldpvt->devName, 
								pdevStringoutACQ196Controldpvt->arg0);
#endif
			pdevStringoutACQ196Controldpvt->pSTDdev = get_STDev_from_name(pdevStringoutACQ196Controldpvt->devName);
			if(!pdevStringoutACQ196Controldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutACQ196Control (init_record) Illegal INP field: task_name");
				free(pdevStringoutACQ196Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutACQ196Control (init_record) Illegal OUT field");
			free(pdevStringoutACQ196Controldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevStringoutACQ196Controldpvt->arg0, STRINGOUT_TAG_STR)) {
		pdevStringoutACQ196Controldpvt->ind = STRINGOUT_TAG;
		pdevStringoutACQ196Controldpvt->n32Arg0 = atoi( pdevStringoutACQ196Controldpvt->arg1 );
	} else if( !strcmp(pdevStringoutACQ196Controldpvt->arg0, STRINGOUT_IP_PORT_STR)) {
		pdevStringoutACQ196Controldpvt->ind = STRINGOUT_IP_PORT;
	} else if( !strcmp(pdevStringoutACQ196Controldpvt->arg0, STRINGOUT_TREE_NAME_STR)) {
		pdevStringoutACQ196Controldpvt->ind = STRINGOUT_TREE_NAME;
	} 

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevStringoutACQ196Controldpvt;
	
	return 2;     /* don't convert */
}


static long devStringoutACQ196Control_write_stringout(stringoutRecord *precord)
{
#if PRINT_PHASE_INFO
        static int kkh_cnt;
#endif
	ST_dpvt        *pdevStringoutACQ196Controldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device          *pSTDdev;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;



	if(!pdevStringoutACQ196Controldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pdevStringoutACQ196Controldpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.n32Arg0 = pdevStringoutACQ196Controldpvt->n32Arg0;
/*	qData.param.setValue		= precord->val; */
	strcpy( qData.param.setStr, precord->val );



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		switch(pdevStringoutACQ196Controldpvt->ind) {
			case STRINGOUT_TAG:
				qData.pFunc = devACQ196_STRINGOUT_TAG;
				break;
			case STRINGOUT_IP_PORT:
				qData.pFunc = devACQ196_STRINGOUT_IP_PORT;
				break;
			case STRINGOUT_TREE_NAME:
				qData.pFunc = devACQ196_STRINGOUT_TREE_NAME;
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if PRINT_PHASE_INFO

		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(), ++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 2;    /* don't convert */
	}

	return 0;
}


static long devAiACQ196RawRead_init_record(aiRecord *precord)
{
	ST_dpvt *pdevAiACQ196RawReaddpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiACQ196RawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiACQ196RawReaddpvt->devName,
				   pdevAiACQ196RawReaddpvt->arg0,
				   pdevAiACQ196RawReaddpvt->arg1);
#if 0
	epicsPrintf("devAiACQ196RawRead arg num: %d: %s %s %s\n",i, pdevAiACQ196RawReaddpvt->devName, 
								pdevAiACQ196RawReaddpvt->arg0,
								pdevAiACQ196RawReaddpvt->arg1);
#endif
			pdevAiACQ196RawReaddpvt->pSTDdev = get_STDev_from_name(pdevAiACQ196RawReaddpvt->devName);
			if(!pdevAiACQ196RawReaddpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiACQ196RawRead (init_record) Illegal INP field: task name");
				free(pdevAiACQ196RawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			if( i > 2)
			{
				pdevAiACQ196RawReaddpvt->pchannelConfig = drvACQ196_find_channelConfig(pdevAiACQ196RawReaddpvt->devName,
											         pdevAiACQ196RawReaddpvt->arg0);
				if(!pdevAiACQ196RawReaddpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiACQ196RawRead (init_record) Illegal INP field: channel name");
					free(pdevAiACQ196RawReaddpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiACQ196RawRead (init_record) Illegal INP field");
			free(pdevAiACQ196RawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if( i == 2 ){
		if(!strcmp(pdevAiACQ196RawReaddpvt->arg0, AI_READ_STATE_STR))
			pdevAiACQ196RawReaddpvt->ind = AI_READ_STATE;
		else if( !strcmp(pdevAiACQ196RawReaddpvt->arg0, AI_PARSING_CNT_STR))
			pdevAiACQ196RawReaddpvt->ind = AI_PARSING_CNT;
		else if( !strcmp(pdevAiACQ196RawReaddpvt->arg0, AI_MDS_SND_CNT_STR))
			pdevAiACQ196RawReaddpvt->ind = AI_MDS_SND_CNT;
		else if( !strcmp(pdevAiACQ196RawReaddpvt->arg0, AI_DAQING_CNT_STR))
			pdevAiACQ196RawReaddpvt->ind = AI_DAQING_CNT;
		
	} else {
/*		if(!strcmp(pdevAiACQ196RawReaddpvt->arg1, AI_READ_GAIN_STR))
			pdevAiACQ196RawReaddpvt->ind = AI_READ_GAIN;
		else if(!strcmp(pdevAiACQ196RawReaddpvt->arg1, AI_READ_VAL_STR))
			pdevAiACQ196RawReaddpvt->ind = AI_READ_VAL;
		else pdevAiACQ196RawReaddpvt->ind = AI_READ_VAL;
		*/
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiACQ196RawReaddpvt;

	return 2;    /* don't convert */ 
}


static long devAiACQ196RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiACQ196RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiACQ196RawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiACQ196RawReaddpvt->pSTDdev;
	if( pdevAiACQ196RawReaddpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else 
		*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiACQ196RawRead_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pdevAiACQ196RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
/*	ST_ACQ196_channel	*pchannelConfig; */
	ST_ACQ196 *pAcq196;


	if(!pdevAiACQ196RawReaddpvt) return 0;

	pSTDdev		= pdevAiACQ196RawReaddpvt->pSTDdev;
/*	pchannelConfig	= pdevAiACQ196RawReaddpvt->pchannelConfig; */

	pAcq196 = (ST_ACQ196*)pSTDdev->pUser;
	

	switch(pdevAiACQ196RawReaddpvt->ind) {
/*		case AI_READ_GAIN:
			precord->val = pchannelConfig->gain;
			break;
		case AI_READ_VAL:
			break; */
		case AI_READ_STATE:
			precord->val = pSTDdev->StatusDev;
/*			epicsPrintf("%s StatusDev: 0x%X : 0x%X\n", pSTDdev->taskName, (int)precord->val, pAcq196->boardConfigure);  */
			break;
		case AI_PARSING_CNT:
			precord->val = pAcq196->parsing_remained_cnt;
/*			epicsPrintf("%s remained: %d\n", pSTDdev->taskName, (int)precord->val); */
			break;
		case AI_MDS_SND_CNT:
			precord->val = pAcq196->mdsplus_snd_cnt;
/*			epicsPrintf("%s MDSplus snd: %d\n", pSTDdev->taskName, (int)precord->val); */
			break;
		case AI_DAQING_CNT:
			precord->val = pAcq196->daqing_cnt;
			break;
		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevAiACQ196RawReaddpvt->ind); 
			break;
	}
	return (2);
}


static long devMbbiACQ196_init_record(mbbiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pDpvt->devName,
				   pDpvt->arg0,
				   pDpvt->arg1);
#if 0
	epicsPrintf("devMbbiACQ196_init_record arg num: %d: %s %s %s\n",nval, pDpvt->devName, 
								pDpvt->arg0,
								pDpvt->arg1);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devMbbiACQ196_init_record (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			
			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devMbbiACQ196 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pDpvt->arg0, MBBI_DEV_STATUS_STR))
		pDpvt->ind = MBBI_DEV_STATUS;
	else if (!strcmp(pDpvt->arg0, "xxx"))
		pDpvt->ind = 0;
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devMbbiACQ196_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;   /*returns: (-1,0)=>(failure,success)*/
}


static long devMbbiACQ196_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_status;
	return 0;
	
}

static long devMbbiACQ196_read_mbbi(mbbiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	
	ST_STD_device *pSTDdev;
	ST_ACQ196 *pAcq196;

	if(!pDpvt) return -1;

	pSTDdev = pDpvt->pSTDdev;
	pAcq196 = (ST_ACQ196*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case MBBI_DEV_STATUS:
			precord->rval = pSTDdev->StatusDev;
			if( pSTDdev->StatusDev & TASK_STANDBY ) 
			{
				if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) precord->val = 1; /* ready to run */
				else if( pSTDdev->StatusDev & TASK_DATA_TRANSFER ) precord->val = 6;
				else if( pSTDdev->StatusDev & TASK_POST_PROCESS ) precord->val = 5;
				else if( pSTDdev->StatusDev & TASK_IN_PROGRESS) precord->val = 4;
				else if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) precord->val = 3;
				else if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) precord->val = 2;
				else precord->val = 8; /* ready but busy with mystery, or maybe expressed error! (???) */
			} else {
				if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) precord->val = 7; /* not ready, Idle */
				else precord->val = 0; /* not initiated or NULL */
			} 
/*			epicsPrintf("%s Status: 0x%X, rval: %d, val: %d \n", pACQ196Cfg->taskName, pACQ196Cfg->StatusACQ196, precord->rval, precord->val);  */
			break;
		
		default:
			epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=>(success, success no convert)*/
}


