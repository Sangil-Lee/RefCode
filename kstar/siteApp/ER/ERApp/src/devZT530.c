/*
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "alarm.h"
#include "callback.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "link.h"
#include "dbCommon.h"
*/
#include "aiRecord.h" 
#include "aoRecord.h"
/*#include "biRecord.h" */
#include "boRecord.h"
#include "stringoutRecord.h"
/*
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h" 
*/

#include "dbAccess.h"  /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"


/*
#include "epicsExport.h"
*/

#include "drvZT530.h"
#include "pvListFgen.h"
#include <string.h>

#define CFG_ZT530_IMMEDIATELY 0

typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];

	char recordName[80];

	ST_ZT530 *ptaskConfig;
	ST_ZT530_channel *pchannelConfig;

	unsigned        ind;

} devZT530Controldpvt;


LOCAL long devAoZT530Control_init_record(aoRecord *precord);
LOCAL long devAoZT530Control_write_ao(aoRecord *precord);

LOCAL long devAiZT530_init_record(aiRecord *precord);
LOCAL long devAiZT530_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiZT530_read_ai(aiRecord *precord);


LOCAL long devStringoutZT530_init_record(stringoutRecord *precord);
LOCAL long devStringoutZT530_write_stringout(stringoutRecord *precord);

LOCAL long devBoZT530_init_record(boRecord *precord);
LOCAL long devBoZT530_write_bo(boRecord *precord);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiZT530 = {
	6,
	NULL,
	NULL,
	devAiZT530_init_record,
	devAiZT530_get_ioint_info,
	devAiZT530_read_ai,
	NULL
};
epicsExportAddress(dset, devAiZT530);


struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoZT530 = {
    5,
    NULL,
    NULL,
    devBoZT530_init_record,
    NULL,
    devBoZT530_write_bo
};
epicsExportAddress(dset, devBoZT530);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoZT530Control = {
	6,
	NULL,
	NULL,
	devAoZT530Control_init_record,
	NULL,
	devAoZT530Control_write_ao,
	NULL
};
epicsExportAddress(dset, devAoZT530Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutZT530Control = {
	6,
	NULL,
	NULL,
	devStringoutZT530_init_record,
	NULL,
	devStringoutZT530_write_stringout,
	NULL
};
epicsExportAddress(dset, devStringoutZT530Control);


unsigned long long int start, stop, _interval;


LOCAL void devZT530_AO_EXPORT_SIG_IN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	char buf[32];

	switch( (int)pParam->setValue ) {
		case ZT530_OUTPUT_ARM_EVENT: 
			pZT530->exportSigIn = ZT530_OUTPUT_ARM_EVENT; 
			strcpy(buf, "Arm");
			break;
		case ZT530_OUTPUT_TRIG_EVENT: 
			pZT530->exportSigIn = ZT530_OUTPUT_TRIG_EVENT;
			strcpy(buf, "Trigger");
			break;
		case ZT530_OUTPUT_CONST_STATE: 
			pZT530->exportSigIn = ZT530_OUTPUT_CONST_STATE;
			strcpy(buf, "Const");
			break;
		case ZT530_OUTPUT_OP_COMPLETE: 
			pZT530->exportSigIn = ZT530_OUTPUT_OP_COMPLETE;
			strcpy(buf, "OP Complete");
			break;
		default: 
			epicsPrintf("ERROR! >>> %s: Routing source signal is not defined! (%d) \n", pSTDdev->taskName,  (int)pParam->setValue );
			return;
	}
	
	if( drvZT530_ExportSignal(pSTDdev ) == WR_OK ){
		epicsPrintf(">>> %s: Routing source signal is (%s) \n", pSTDdev->taskName,  buf );
	} else {
		epicsPrintf("ERROR! %s: Routing source signal is (%s) \n", pSTDdev->taskName,  buf );
	}

}

LOCAL void devZT530_AO_EXPORT_SIG_TO(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;

	switch( (int)pParam->setValue ) {
		case NI_FGEN_EXPORT_NOTHING:
			strcpy( pZT530->strExportSigTO, "");
			pZT530->exportSigTO = ZT530_SOURCE_SOFTWARE; /* 0 */
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI0:
			strcpy( pZT530->strExportSigTO, "PXI_Trig0");
			pZT530->exportSigTO = ZT530_SOURCE_PXI_TTL0; /* 1 */
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI1:
			strcpy( pZT530->strExportSigTO, "PXI_Trig1");
			pZT530->exportSigTO = ZT530_SOURCE_PXI_TTL1; /* 2 */
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI2:
			strcpy( pZT530->strExportSigTO, "PXI_Trig2");
			pZT530->exportSigTO = 3;
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI3:
			strcpy( pZT530->strExportSigTO, "PXI_Trig3");
			pZT530->exportSigTO = 4;
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI4:
			strcpy( pZT530->strExportSigTO, "PXI_Trig4");
			pZT530->exportSigTO = 5;
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI5:
			strcpy( pZT530->strExportSigTO, "PXI_Trig5");
			pZT530->exportSigTO = 6;
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI6:
			strcpy( pZT530->strExportSigTO, "PXI_Trig6");
			pZT530->exportSigTO = ZT530_SOURCE_PXI_TTL6; /* 7 */
			break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI7:
			strcpy( pZT530->strExportSigTO, "PXI_Trig7");
			pZT530->exportSigTO = ZT530_SOURCE_PXI_TTL7; /* 8 */
			break;
		case NI_FGEN_EXPORT_SIG_TO_RefOut: strcpy( pZT530->strExportSigTO, "RefOut"); break;
		case NI_FGEN_EXPORT_SIG_TO_PFI0: strcpy( pZT530->strExportSigTO, "PFI0"); break;
		case NI_FGEN_EXPORT_SIG_TO_PFI1: strcpy( pZT530->strExportSigTO, "PFI1"); break;
		case NI_FGEN_EXPORT_SIG_TO_PXIStar:
			strcpy( pZT530->strExportSigTO, "PXIStar");
			pZT530->exportSigTO = ZT530_SOURCE_PXI_STAR_TRIG; /* 9 */
			break;

		default: 
			pZT530->exportSigTO = 0;
			epicsPrintf("ERROR! >>> %s: Signal Source is wrong! (%d) \n", pSTDdev->taskName,  (int)pParam->setValue );
			return;
	}
	
	if( drvZT530_ExportSignal(pSTDdev ) == WR_OK ){
		epicsPrintf(">>> %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pZT530->strExportSigTO );
	} else {
		epicsPrintf("ERROR! %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pZT530->strExportSigTO );
	}

}

LOCAL void devZT530_AO_TRIG_TYPE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	pZT530->trigType = (ViInt32)pParam->setValue;

/*	printf("trigType: %d\n", pZT530->trigType); */
	if( drvZT530_cfg_trigType(pSTDdev) == WR_OK ) {
		switch(pZT530->trigType) {
		case ZT530_SOURCE_SOFTWARE:
			epicsPrintf(">>> %s:(%d)-SOFTWARE\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL0:
			epicsPrintf(">>> %s:(%d)-TTL0\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL1:
			epicsPrintf(">>> %s:(%d)-TTL1\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL2:
			epicsPrintf(">>> %s:(%d)-TTL2\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL3:
			epicsPrintf(">>> %s:(%d)-TTL3\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL4:
			epicsPrintf(">>> %s:(%d)-TTL4\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL5:
			epicsPrintf(">>> %s:(%d)-TTL5\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL6:
			epicsPrintf(">>> %s:(%d)-TTL6\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_TTL7:
			epicsPrintf(">>> %s:(%d)-TTL7\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_PXI_STAR_TRIG:
			epicsPrintf(">>> %s:(%d)-STAR\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_EXT_TRIG:
			epicsPrintf(">>> %s:(%d)-EXT\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		case ZT530_SOURCE_IMM:
			epicsPrintf(">>> %s:(%d)-IMM\n", pSTDdev->taskName, (int)pZT530->trigType );
			break;
		}
	}
	else {
		pSTDdev->ErrorDev = NI5412_ERROR_TRIG_TYPE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	
}
LOCAL void devZT530_AO_TRIG_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	pZT530->trigMode = (ViInt32)pParam->setValue;
	
	if( drvZT530_cfg_trigMode(pSTDdev) == WR_OK ) {
		switch( pZT530->trigMode ) {
		case NIFGEN_VAL_SINGLE:
			epicsPrintf(">>> %s: Tirg Mode (%d) : SINGLE\n", pSTDdev->taskName, (int)pZT530->trigMode ); break;
		case NIFGEN_VAL_CONTINUOUS:
			epicsPrintf(">>> %s: Tirg Mode (%d) : CONTINUOUS\n", pSTDdev->taskName, (int)pZT530->trigMode ); break;
		case NIFGEN_VAL_STEPPED:
			epicsPrintf(">>> %s: Tirg Mode (%d) : STEPPED\n", pSTDdev->taskName, (int)pZT530->trigMode ); break;
		case NIFGEN_VAL_BURST:
			epicsPrintf(">>> %s: Tirg Mode (%d) : BURST\n", pSTDdev->taskName, (int)pZT530->trigMode ); break;
		}
	}
}
LOCAL void devZT530_AO_ARB_CH_OFFSET(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	
	double prev_offset;
	prev_offset = pchannelConfig->f64DCoffset;

	pchannelConfig->f64DCoffset = (double)pParam->setValue;
#if CFG_ZT530_IMMEDIATELY
	if( drvZT530_cfg_arbitraryWF(pSTDdev, pSTDch) == WR_ERROR ) {
		pchannelConfig->f64DCoffset = prev_offset;
		pSTDdev->ErrorDev = NI5412_ERROR_OFFSET;
	} else {
		epicsPrintf(">>> %s/%s: Offset %f\n", pSTDdev->taskName, pSTDch->chName, pchannelConfig->f64DCoffset );
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devZT530_AO_ARB_CH_GAIN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	
	double prev_Gain;
	prev_Gain = pchannelConfig->f64Gain;
	
	pchannelConfig->f64Gain = (double)pParam->setValue;
#if CFG_ZT530_IMMEDIATELY
	if( drvZT530_cfg_arbitraryWF(pSTDdev, pSTDch ) == WR_ERROR ) {
		pchannelConfig->f64Gain = prev_Gain;
		pSTDdev->ErrorDev = NI5412_ERROR_GAIN;
	} else {
		epicsPrintf(">>> %s/%s: Gain %f\n", pSTDdev->taskName, pSTDch->chName, pchannelConfig->f64Gain );
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devZT530_STRINGOUT_EXPORT_SIG_TO(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;

/*	[RTSI0|RTSI1|RTSI2|RTSI3|RTSI4|RTSI5|RTSI6|RTSI7|RefOut|PFI0|PFI1|PXIStar] */
	if( !strcmp("PXI_Trig0", pParam->setStr) ||
		!strcmp("RTSI1", pParam->setStr) ||
		!strcmp("RTSI2", pParam->setStr) ||
		!strcmp("RTSI3", pParam->setStr) ||
		!strcmp("RTSI4", pParam->setStr) ||
		!strcmp("RTSI5", pParam->setStr) ||
		!strcmp("RTSI6", pParam->setStr) ||
		!strcmp("RTSI7", pParam->setStr) ||
		!strcmp("RefOut", pParam->setStr) ||
		!strcmp("PFI0", pParam->setStr) ||
		!strcmp("PFI1", pParam->setStr) ||
		!strcmp("PXIStar", pParam->setStr) ) 
	{
		strcpy( pZT530->strExportSigTO, pParam->setStr);
	} else {
		epicsPrintf("ERROR! >>> %s: Signal Source is wrong! \"%s\" \n", pSTDdev->taskName,  pParam->setStr );
		pSTDdev->ErrorDev = NI5412_ERROR_ROUTE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}
	
	if( drvZT530_ExportSignal(pSTDdev ) == WR_OK ){
		epicsPrintf(">>> %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pZT530->strExportSigTO );

	} else {
		epicsPrintf("ERROR! %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pZT530->strExportSigTO );
		pSTDdev->ErrorDev = NI5412_ERROR_ROUTE;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
	
}


LOCAL void devZT530_STRINGOUT_TRIG_SRC(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	
	strcpy( pZT530->triggerSource, pParam->setStr);
	epicsPrintf(">>> %s: Trigger Source: %s\n", pSTDdev->taskName,  pZT530->triggerSource );
}

LOCAL void devZT530_STRINGOUT_WF_FILENAME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	
	strcpy(pchannelConfig->wfmFileName, WF_ROOT_DIR);
	strcat(pchannelConfig->wfmFileName, pParam->setStr);

/*epicsPrintf(">>> %s/%s: waveform name %s\n", pSTDdev->taskName, pSTDch->chName, pchannelConfig->wfmFileName ); */

	if( drvZT530_create_WFdownload(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf(">>> %s/%s: Create the waveform and download.\n", pSTDdev->taskName, pSTDch->chName ); 
		notify_error(0, "Clean");
	}
	else 
	{
		pSTDdev->ErrorDev = NI5412_ERROR_WF_DOWNLOAD;
		notify_error(1, "can't load %s", pParam->setStr);
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

/* not use this function.... we use "devZT530_STRINGOUT_WF_FILENAME"   */
LOCAL void devZT530_AO_CREATE_WF_DL(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
/*	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser; */
	
	if( drvZT530_create_WFdownload(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf(">>> %s/%s: Create the waveform and download.\n", pSTDdev->taskName, pSTDch->chName ); 
	} 

}

LOCAL void devZT530_AO_CFG_ARB_WF(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;

	pSTDdev->StatusDev &= ~TASK_STANDBY;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
	
	if( drvZT530_cfg_arbitraryWF(pSTDdev, pSTDch ) == WR_OK ) {
		pSTDdev->StatusDev |= TASK_STANDBY;
		epicsPrintf(">>> %s/%s: Select the ARB Waveform to generate.\n", pSTDdev->taskName, pSTDch->chName ); 
		epicsPrintf("\nch name: %s, wfmHandle:%lu, Gain:%f, DCoffset:%f\n", pSTDch->chName,
		pchannelConfig->wfmHandle, 
		pchannelConfig->f64Gain,
		pchannelConfig->f64DCoffset);
	} 
	else  {
		pSTDdev->ErrorDev = NI5412_ERROR_CFG_ARB;
		}
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
}

LOCAL void devZT530_AO_CFG_STD_WF(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;

	pSTDdev->StatusDev &= ~TASK_STANDBY;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
	if( drvZT530_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		pSTDdev->StatusDev |= TASK_STANDBY;
		epicsPrintf(">>> %s/%s: Configure the STD Function to generate.\n", pSTDdev->taskName, pSTDch->chName ); 
		epicsPrintf("ch name: %s, type:%d, amp:%f, offset:%f, freq:%f, ph:%f\n", pSTDch->chName,
						  pchannelConfig->wfmType,
						  pchannelConfig->f64Amplitude,
						  pchannelConfig->f64DCoffset,
						  pchannelConfig->f64Frequency,
						  pchannelConfig->f64StartPhase);
	} 
	else {		
		pSTDdev->ErrorDev = NI5412_ERROR_CFG_STD;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
}

LOCAL void devZT530_AO_ENABLE_OUTPUT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ST_ZT530_channel *pZT530ch = (ST_ZT530_channel*)pSTDch->pUser; 
/*	struct dbCommon *precord = pParam->precord; */
	
	
	if( (epicsUInt16)pParam->setValue > 1){
		epicsPrintf(">>> %s/%s: Wrong arg (%d)\n", pSTDdev->taskName, pSTDch->chName, (epicsUInt16)pParam->setValue ); 
		return;
	}

	if( pParam->n32Arg0 == 0 ) {
		pZT530->ch0Enable = (epicsUInt16)pParam->setValue ? ZT530_OUTPUT_ACTIVE : ZT530_OUTPUT_INACTIVE;
		printf("%s: ch #1 enable: %d\n", pSTDch->chName, pZT530->ch0Enable);
	}
	else if( pParam->n32Arg0 == 1 ) {
		pZT530->ch1Enable = (epicsUInt16)pParam->setValue ? ZT530_OUTPUT_ACTIVE : ZT530_OUTPUT_INACTIVE;
		printf("%s: ch #2 enable: %d\n", pSTDch->chName, pZT530->ch1Enable);
	}
	else {
		epicsPrintf(">>> %s/%s: Wrong channel ID (%d)\n", pSTDdev->taskName, pSTDch->chName, (epicsUInt16)pParam->n32Arg0 ); 
		return;
	}
	pZT530ch->u16OutputState = (epicsUInt16)pParam->setValue ? ZT530_OUTPUT_ACTIVE : ZT530_OUTPUT_INACTIVE;
	
	if( drvZT530_set_outputEnable(pSTDdev, pSTDch) == WR_OK ) {
		if( (epicsUInt16)pParam->setValue == 1) {
			epicsPrintf(">>> %s/%s: output Enabled.\n", pSTDdev->taskName, pSTDch->chName ); 
			pSTDdev->StatusDev |= TASK_STANDBY;
		}
		else {
			epicsPrintf(">>> %s/%s: output Disabled.\n", pSTDdev->taskName, pSTDch->chName ); 
			pSTDdev->StatusDev &= ~TASK_STANDBY;
		}

		pSTDdev->ErrorDev = 0;
		scanIoRequest(pSTDdev->ioScanPvt_status);
	} 
	else {
		pSTDdev->StatusDev &= ~TASK_STANDBY;
		pSTDdev->ErrorDev = NI5412_ERROR_OUTPUT_ENABLE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	notify_refresh_master_status();
}
LOCAL void devZT530_BO_ABORT_GEN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	
	if( drvZT530_abort_Generation(pSTDdev) == WR_OK ) {
		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
		epicsPrintf(">>> %s: generation aborted!\n", pSTDdev->taskName );
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	notify_refresh_master_status();
}

LOCAL void devZT530_BO_RESET_INIT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	if( (int)pParam->setValue ) {
		pSTDdev->StatusDev &= ~TASK_STANDBY;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		notify_refresh_master_status();

		if( drvZT530_reset_init(pSTDdev) == WR_OK ) 
			epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
		else
			epicsPrintf(" %s %s (%d)  ERROR!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
	}
}

LOCAL void devZT530_BO_FUNC_GEN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	if( (int)pParam->setValue ) {
		pSTDdev->StatusDev &= ~TASK_STANDBY;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		notify_refresh_master_status();

		if( drvZT530_set_function_generator(pSTDdev ) == WR_OK ) {
			pSTDdev->StatusDev |= TASK_STANDBY;
			epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
		} 
		else 
			epicsPrintf(" %s %s (%d)  ERROR!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
		
		scanIoRequest(pSTDdev->ioScanPvt_status);
		notify_refresh_master_status();
		
		
	}

}

LOCAL void devZT530_BO_SIG_GEN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	

	if( (ViInt32)pParam->setValue ) {
		if( drvZT530_init_Generation(pSTDdev) == WR_OK ) {
			pSTDdev->StatusDev |= TASK_ARM_ENABLED;
			pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
/*			pSTDdev->StatusDev |= TASK_IN_PROGRESS; */ /* block to do. 'cause it's not DAQ H/W 2010.08.11 woong */
			epicsPrintf(">>> %s: signal generated!\n", pSTDdev->taskName );
		} 
		else {
			pSTDdev->ErrorDev = NI5412_ERROR_SIG_GEN;
		}
	}
	else 
	{
		if( drvZT530_abort_Generation(pSTDdev) == WR_OK ) {
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
			pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
			epicsPrintf(">>> %s: generation aborted!\n", pSTDdev->taskName );
		}
		else {
			pSTDdev->ErrorDev = NI5412_ERROR_SIG_GEN;
		}
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);

	notify_refresh_master_status();
}

LOCAL void devZT530_AO_WFM_TYPE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
/*
	ZT530_FUNC_DC_WAVE                                      0x0000
 * ZT530_FUNC_SINE_WAVE                                    0x0001
 * ZT530_FUNC_SQUARE_WAVE                                  0x0002
 * ZT530_FUNC_TRIANGLE_WAVE                                0x0003
 * ZT530_FUNC_SINC_WAVE                                    0x0004
 * ZT530_FUNC_PULSE_WAVE                                   0x0005
 * ZT530_FUNC_RAMP_WAVE                                    0x0006
 * ZT530_FUNC_NOISE_WAVE                                   0x0007
 * ZT530_FUNC_MULTI_TONE_WAVE                              0x0008
 * ZT530_FUNC_PATT_WAVE                                    0x0009
 * ZT530_FUNC_DBL_PULSE_WAVE                               0x000A
 * ZT530_FUNC_AM_WAVE                                      0x000B
 * ZT530_FUNC_FM_WAVE					   0x000C
 * ZT530_FUNC_USER_DEFINED_WAVE				   0x0100
*/
	
	
	pchannelConfig->wfmType = (ViInt32)pParam->setValue;
/*	printf("WFM type: %d (0x%x)\n", pchannelConfig->wfmType, pchannelConfig->wfmType ); */
	switch(pchannelConfig->wfmType){
		case 0: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_DC_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_DC_WAVE;
			break;
		case 1: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_SINE_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_SINE_WAVE;
			break;
		case 2: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_SQUARE_WAVE\n", pSTDdev->taskName, pSTDch->chName);
			pchannelConfig->wfmType = ZT530_FUNC_SQUARE_WAVE;
			break;
		case 3: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_TRIANGLE_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_TRIANGLE_WAVE;
			break;
		case 4: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_RAMP_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_RAMP_WAVE;
			break;
		case 5: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_RAMP_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_RAMP_WAVE;
			break;
		case 6: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_DC_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_DC_WAVE;
			break;
		case 7: 
			epicsPrintf(">>> %s/%s: ZT530_FUNC_USER_DEFINED_WAVE\n", pSTDdev->taskName, pSTDch->chName); 
			pchannelConfig->wfmType = ZT530_FUNC_USER_DEFINED_WAVE;
			break;
		default: 
			epicsPrintf(">>> %s/%s: Wrong type (%lu)\n", pSTDdev->taskName, pSTDch->chName, (ViInt32)pParam->setValue ); 
			break;
	}

}

LOCAL void devZT530_AO_AMPLITUDE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64Amplitude;
/* max val: 12	, 24Vpp */
	pchannelConfig->f64Amplitude = pParam->setValue;

#if CFG_ZT530_IMMEDIATELY
	if( drvZT530_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf("control thread (set Amplitude): %s/%s %s (%s): %lfVpp\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64Amplitude );
	} else {
		epicsPrintf("Maximum value : 12 \n");
		pchannelConfig->f64Amplitude = fVal;
		pSTDdev->ErrorDev = NI5412_ERROR_AMPLITUDE;
	}	
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devZT530_AO_DC_OFFSET(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64DCoffset;
	
	pchannelConfig->f64DCoffset = pParam->setValue;
#if CFG_ZT530_IMMEDIATELY	
	if( drvZT530_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf("control thread (set DCoffset): %s/%s %s (%s): %lf\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64DCoffset );
	} else {
		pchannelConfig->f64DCoffset = fVal;
		pSTDdev->ErrorDev = NI5412_ERROR_OFFSET;
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devZT530_AO_FREQ(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64Frequency;
/* Max: 20MHz */	
	pchannelConfig->f64Frequency = pParam->setValue;

#if CFG_ZT530_IMMEDIATELY
	if( drvZT530_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf("control thread (set Frequency): %s/%s %s (%s): %lfKHz\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64Frequency/1000.0 );
	} else {
		epicsPrintf("Maximum value : 20MHz \n");
		pchannelConfig->f64Frequency = fVal;
		pSTDdev->ErrorDev = NI5412_ERROR_FREQ;
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devZT530_AO_PHASE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64StartPhase;

	pchannelConfig->f64StartPhase = pParam->setValue;
#if CFG_ZT530_IMMEDIATELY
	if( drvZT530_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf("control thread (set StartPhase): %s/%s %s (%s): %lf\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64StartPhase );
	} else {
		pchannelConfig->f64StartPhase = fVal;
		pSTDdev->ErrorDev = NI5412_ERROR_PHASE;
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}				
/* not implemented */
LOCAL void devZT530_AO_CLOCK_SOURCE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530_channel *pchannelConfig = (ST_ZT530_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("control thread (xxx): %s/%s %s (%s): %lf\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64DCoffset );
}

LOCAL void devZT530_AO_ARB_SAMPLE_RATE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;	
	ST_ZT530_channel *pZT530ch = (ST_ZT530_channel*)pSTDch->pUser;
	double fval = pZT530ch->f64Frequency;

	pZT530->sampleRate = pParam->setValue;
	pZT530ch->f64Frequency = pParam->setValue;
/*
	if( drvZT530_arb_sampleRate(pSTDdev) == WR_OK ) {
		epicsPrintf("control thread (arb sample rate): %s (%s): %lfKHz\n", 
									precord->name, epicsThreadGetNameSelf(), pZT530->sampleRate/1000. );
	} else {
		pZT530->sampleRate = fval;
		pSTDdev->ErrorDev = NI5412_ERROR_FREQ;
	}
*/
#if CFG_ZT530_IMMEDIATELY
	if( drvZT530_cfg_arbitraryWF(pSTDdev, pSTDch ) == WR_ERROR ) {
		pZT530ch->f64Frequency = fval;
		pSTDdev->ErrorDev = NI5412_ERROR_GAIN;
	} else {
		epicsPrintf(">>> %s/%s: Gain %f\n", pSTDdev->taskName, pSTDch->chName, pZT530ch->f64Gain );
	}
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devZT530_AO_CLOCK_MODE(ST_execParam *pParam)
{
	ViInt32 n32Prev;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	
	n32Prev = pZT530->clockMode;
	pZT530->clockMode = (ViInt32)pParam->setValue;
	if( drvZT530_set_clockMode(pSTDdev) == WR_OK ) {
		switch(pZT530->clockMode){
			case NIFGEN_VAL_HIGH_RESOLUTION: 
				epicsPrintf(">>> %s: clock mode: HIGH_RESOLUTION\n", pSTDdev->taskName); break;
			case NIFGEN_VAL_DIVIDE_DOWN: 
				epicsPrintf(">>> %s: clock mode: DIVIDE_DOWN\n", pSTDdev->taskName); break;
			case NIFGEN_VAL_AUTOMATIC: 
				epicsPrintf(">>> %s: clock mode: AUTOMATIC\n", pSTDdev->taskName); break;
			default: 
				epicsPrintf(">>> %s: Wrong type (%lu)\n", pSTDdev->taskName, (ViInt32)pParam->setValue ); 
				pZT530->clockMode = n32Prev;
				break;
		}
	} else {
		pZT530->clockMode = n32Prev;
		pSTDdev->ErrorDev = NI5412_ERROR_CLOCK_MODE;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

				
				
/* just select arbitrary function generator or not */
/* in case of ZT530, it does not needed */
LOCAL void devZT530_AO_OUTPUT_MODE(ST_execParam *pParam)
{
	ViInt32 n32Prev;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
/*	struct dbCommon *precord = pParam->precord; */
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;

	
	switch((ViInt32)pParam->setValue){
		case 0: /* 0L */
			epicsPrintf(">>> %s: Configure output for standard function mode...\n", pSTDdev->taskName); 
			db_put("ER_ZT530_1_CH0:WFM_TYPE", "0");
			db_put("ER_ZT530_1_CH1:WFM_TYPE", "0");
			break;
		case 1: /* 1L */
			epicsPrintf(">>> %s: Configure output for arbitrary mode...\n", pSTDdev->taskName); 
			db_put("ER_ZT530_1_CH0:WFM_TYPE", "7");
			db_put("ER_ZT530_1_CH1:WFM_TYPE", "7");
			break;
		case 2: /* 2L */
			epicsPrintf(">>> %s: Configure output for sequence mode...\n", pSTDdev->taskName); 
			break;
		default: 
			epicsPrintf(">>> %s: got the wrong output mode (%lu)\n", pSTDdev->taskName, (ViInt32)pParam->setValue ); 
			return;
	}
	n32Prev = pZT530->outputMode;
	pZT530->outputMode = (ViInt32)pParam->setValue;
/*	if( drvZT530_cfg_ouputMode(pSTDdev) == WR_ERROR) {		
		epicsPrintf("\n>>> devZT530_set_ouputMode() : drvZT530_set_ouputMode()... failed\n");
		pZT530->outputMode = n32Prev;
		pSTDdev->ErrorDev = NI5412_ERROR_OUT_MODE;
		return ;
	} 
	else {
		epicsPrintf("OK!\n");
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
*/	
/*	epicsPrintf("control thread (set output mode): %s %s (%s)\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf());
*/
}


LOCAL void devZT530_test_put(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

/*	ptaskConfig->fT0 = (float)pParam->setValue; */
	drvZT530_abort_Generation(pSTDdev);
/*
	mds_createNewShot( (int)pParam->setValue );
*/	
	epicsPrintf("control thread (test PUT): %s %s (%s), val: %f\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), (float)pParam->setValue);
}

LOCAL long devAoZT530Control_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
/*			strcpy(pDpvt->recordName, precord->name); */
			i = sscanf(precord->out.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);

#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devAoZT530Control arg num: %d: %s %s %s\n",i, pDpvt->devName, arg0, arg1);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoZT530Control (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			if( i> 2)  {
				unsigned short chid;
				chid = strtoul(arg0, NULL, 0);
				pDpvt->n32Arg0 = chid;
				pDpvt->pSTDch = get_STCh_from_devName_chID(pDpvt->devName, chid);
				if(!pDpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAoZT530Controld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n", precord->name, pDpvt->devName, arg0, arg1);
					free(pDpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			}
		

			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoZT530Control (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if( i==2){
		if (!strcmp(arg0, CONTROL_TEST_PUT_STR)) {
			pDpvt->ind = CONTROL_TEST_PUT;
		} else if (!strcmp(arg0, CONTROL_SET_OUTPUT_MODE_STR)) {
			pDpvt->ind = CONTROL_SET_OUTPUT_MODE;
		} else if (!strcmp(arg0, CONTROL_SET_CLOCK_SRC_STR)) {
			pDpvt->ind = CONTROL_SET_CLOCK_SRC;
		}  else if (!strcmp(arg0, CONTROL_SET_CLOCK_MODE_STR)) {
			pDpvt->ind = CONTROL_SET_CLOCK_MODE;
		} else if (!strcmp(arg0, AO_TRIG_TYPE_STR)) {
			pDpvt->ind = AO_TRIG_TYPE;
		}  else if (!strcmp(arg0, AO_EXPORT_SIG_IN_STR)) {
			pDpvt->ind = AO_EXPORT_SIG_IN;
		} else if (!strcmp(arg0, AO_EXPORT_SIG_TO_STR)) {
			pDpvt->ind = AO_EXPORT_SIG_TO;
		} else if (!strcmp(arg1, AO_TRIG_MODE_STR)) {
			pDpvt->ind = AO_TRIG_MODE;
		}
		


		 
	} 
	else if( i==3 ) {
		if (!strcmp(arg1, CONTROL_SET_WFM_TYPE_STR)) {
			pDpvt->ind = CONTROL_SET_WFM_TYPE;
		} else if (!strcmp(arg1, CONTROL_SET_AMPLITUDE_STR)) {
			pDpvt->ind = CONTROL_SET_AMPLITUDE;
		} else if (!strcmp(arg1, CONTROL_SET_DCOFFSET_STR)) {
			pDpvt->ind = CONTROL_SET_DCOFFSET;
		} else if (!strcmp(arg1, CONTROL_SET_FREQUENCY_STR)) {
			pDpvt->ind = CONTROL_SET_FREQUENCY;
		} else if (!strcmp(arg1, CONTROL_SET_PHASE_STR)) {
			pDpvt->ind = CONTROL_SET_PHASE;
		} else if (!strcmp(arg1, CONTROL_SET_OUTPUT_ENABLE_STR)) {
			pDpvt->ind = CONTROL_SET_OUTPUT_ENABLE;
		} else if (!strcmp(arg1, CONTROL_CFG_STD_WF_STR)) {
			pDpvt->ind = CONTROL_CFG_STD_WF;
		} else if (!strcmp(arg1, CONTROL_CREATE_WF_DOWNLOAD_STR)) {
			pDpvt->ind = CONTROL_CREATE_WF_DOWNLOAD;
		} else if (!strcmp(arg1, CONTROL_CFG_ARB_WF_STR)) {
			pDpvt->ind = CONTROL_CFG_ARB_WF;
		} else if (!strcmp(arg1, AO_ARB_CH_GAIN_STR)) {
			pDpvt->ind = AO_ARB_CH_GAIN;
		} else if (!strcmp(arg1, AO_ARB_CH_OFFSET_STR)) {
			pDpvt->ind = AO_ARB_CH_OFFSET;
		} else if (!strcmp(arg1, CONTROL_ARB_SAMPLE_RATE_STR)) {
			pDpvt->ind = CONTROL_ARB_SAMPLE_RATE;
		}
		
				
	}
	
	
	


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoZT530Control_write_ao(aoRecord *precord)
{
#if SHOW_PHASE_NIFGEN
	static int   kkh_cnt;
#endif
	ST_dpvt			*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device   *ptaskConfig;
	ST_STD_channel  *pchannelConfig;
	ST_threadCfg  *pcontrolThreadConfig;
	ST_threadQueueData			qData;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig			= pDpvt->pSTDdev;
	pchannelConfig		= pDpvt->pSTDch;
	pcontrolThreadConfig		= ptaskConfig->pST_stdCtrlThread;
	qData.param.pSTDdev		= ptaskConfig;
	qData.param.pSTDch		= pchannelConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pDpvt->pSTDch->chanIndex; */
	qData.param.setValue		= precord->val;
	qData.param.n32Arg0 = pDpvt->n32Arg0;
	qData.param.n32Arg1 = pDpvt->n32Arg1;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif

		switch(pDpvt->ind) {
			case CONTROL_TEST_PUT:
				qData.pFunc = devZT530_test_put;
				break;
			case CONTROL_SET_OUTPUT_MODE:
				qData.pFunc = devZT530_AO_OUTPUT_MODE;
				break;
			case CONTROL_SET_CLOCK_SRC:
				qData.pFunc = devZT530_AO_CLOCK_SOURCE;
				break;
			case CONTROL_ARB_SAMPLE_RATE:
				qData.pFunc = devZT530_AO_ARB_SAMPLE_RATE;
				break;
			case CONTROL_SET_CLOCK_MODE:
				qData.pFunc = devZT530_AO_CLOCK_MODE;
				break;


			case CONTROL_SET_WFM_TYPE: qData.pFunc = devZT530_AO_WFM_TYPE; break;
			case CONTROL_SET_AMPLITUDE: qData.pFunc = devZT530_AO_AMPLITUDE; break;
			case CONTROL_SET_DCOFFSET: qData.pFunc = devZT530_AO_DC_OFFSET; break;
			case CONTROL_SET_FREQUENCY: qData.pFunc = devZT530_AO_FREQ; break;
			case CONTROL_SET_PHASE: qData.pFunc = devZT530_AO_PHASE; break;
			case CONTROL_SET_OUTPUT_ENABLE: qData.pFunc = devZT530_AO_ENABLE_OUTPUT; break;
				
			case CONTROL_CFG_STD_WF: qData.pFunc = devZT530_AO_CFG_STD_WF; break;
			case CONTROL_CREATE_WF_DOWNLOAD: qData.pFunc = devZT530_AO_CREATE_WF_DL; break;
			case CONTROL_CFG_ARB_WF: qData.pFunc = devZT530_AO_CFG_ARB_WF; break;
			case AO_ARB_CH_GAIN: qData.pFunc = devZT530_AO_ARB_CH_GAIN; break;
			case AO_ARB_CH_OFFSET: qData.pFunc = devZT530_AO_ARB_CH_OFFSET; break;
			case AO_TRIG_MODE: qData.pFunc = devZT530_AO_TRIG_MODE; break;
			case AO_TRIG_TYPE: qData.pFunc = devZT530_AO_TRIG_TYPE; break;
			case AO_EXPORT_SIG_TO: qData.pFunc = devZT530_AO_EXPORT_SIG_TO; break;
			case AO_EXPORT_SIG_IN: qData.pFunc = devZT530_AO_EXPORT_SIG_IN; break;

		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		
		switch(pDpvt->ind) {
			case CONTROL_TEST_PUT: precord->val = 0; break;
			case CONTROL_CFG_STD_WF: precord->val = 0; break;
			case CONTROL_CFG_ARB_WF: precord->val = 0; break;
		}

		return 2;    /* don't convert */
	}

	return 0;
}

LOCAL long devStringoutZT530_init_record(stringoutRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) {
		case INST_IO:
			i = sscanf(precord->out.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);

#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devStringoutZT530Control arg num: %d: %s %s %s\n",i, pDpvt->devName, arg0, arg1);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutZT530_init_record (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			
			if( i> 2)  {
				unsigned short chid;
				chid = strtoul(arg0, NULL, 0);
				pDpvt->pSTDch = get_STCh_from_devName_chID(pDpvt->devName, chid);
				if(!pDpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAoZT530Controld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n", precord->name, pDpvt->devName, arg0, arg1);
					free(pDpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			}

				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutZT530Control (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if( i==2 ) {
		if(!strcmp(arg0, STRINGOUT_TRIG_SRC_STR)) {
			pDpvt->ind = STRINGOUT_TRIG_SRC;
		} else if(!strcmp(arg0, STRINGOUT_EXPORT_SIG_TO_STR)) {
			pDpvt->ind = STRINGOUT_EXPORT_SIG_TO;
		} 

		


	} else if( i==3 ) {
		if(!strcmp(arg1, CONTROL_WF_FILENAME_STR)) {
			pDpvt->ind = CONTROL_WF_FILENAME;
		} 
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /* don't convert */
}


LOCAL long devStringoutZT530_write_stringout(stringoutRecord *precord)
{
	ST_dpvt			*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *ptaskConfig;
	ST_STD_channel *pchannelConfig;
	ST_threadCfg *pcontrolThreadConfig;
	ST_threadQueueData qData;
	
	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}
	
	ptaskConfig			= pDpvt->pSTDdev;
	pchannelConfig		= pDpvt->pSTDch;
	pcontrolThreadConfig		= ptaskConfig->pST_stdCtrlThread;
	qData.param.pSTDdev		= ptaskConfig;
	qData.param.pSTDch		= pchannelConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pDpvt->pSTDch->chanIndex; */

	strcpy( qData.param.setStr, precord->val ); 

	/* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif

		switch(pDpvt->ind) {
			case CONTROL_WF_FILENAME:
				qData.pFunc = devZT530_STRINGOUT_WF_FILENAME;
				break;
			case STRINGOUT_TRIG_SRC:
				qData.pFunc = devZT530_STRINGOUT_TRIG_SRC;
				break;
			case STRINGOUT_EXPORT_SIG_TO:
				qData.pFunc = devZT530_STRINGOUT_EXPORT_SIG_TO;
				break;

		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId, (void*) &qData, sizeof(ST_threadQueueData));
		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		precord->pact = FALSE;
        	precord->udf = FALSE;
		return 2;    /* don't convert */
	}
	return 0;
}

LOCAL long devBoZT530_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

    switch(precord->out.type) {
        case INST_IO:
            i = sscanf(precord->out.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
#if PRINT_DEV_SUPPORT_ARG
			epicsPrintf("devBoZT530 arg num: %d: %s %s %s\n",i, pDpvt->devName, arg0, arg1);
#endif
            pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
            if(!pDpvt->devName) { /* port not found */
                recGblRecordError(S_db_badField, (void*)precord,
                                  "devBoZT530 (init_record) Illegal OUT field: port not found");
                free(pDpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            if( i> 2)  {
				unsigned short chid;
				chid = strtoul(arg0, NULL, 0);
				pDpvt->pSTDch = get_STCh_from_devName_chID(pDpvt->devName, chid);
				if(!pDpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devBoZT530Controld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n", precord->name, pDpvt->devName, arg0, arg1);
					free(pDpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			} /* end of switch for pDpvt->param_first */
			
			
			
            break;  /* end of inst_io */
        default:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devBoZT530 (init_record) Illegal OUT field");
            free(pDpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    } /* end of switch statement for precord->out.type */
    
    
    if( i==2){
		if (!strcmp(arg0, BO_ABORT_GEN_STR)) {
			pDpvt->ind = BO_ABORT_GEN;
		} else if( !strcmp(arg0, BO_SIG_GEN_STR)) {
			pDpvt->ind = BO_SIG_GEN;
		}  else if( !strcmp(arg0, BO_FUNC_GEN_STR)) {
			pDpvt->ind = BO_FUNC_GEN;
		}
		else if( !strcmp(arg0, BO_RESET_INIT_STR)) {
			pDpvt->ind = BO_RESET_INIT;
		}
		 
	} else if( i==3 ) {

	}

    precord->udf = FALSE;
    precord->dpvt = (void*)pDpvt;
    return 0;
}


LOCAL long devBoZT530_write_bo(boRecord *precord)
{
	ST_dpvt			*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *ptaskConfig;
	ST_STD_channel *pchannelConfig;
	ST_threadCfg			*pcontrolThreadConfig;
	ST_threadQueueData			qData;


    if(!precord->dpvt || precord->udf == TRUE) {
        precord->pact = TRUE;
        return 0;
	}

	ptaskConfig			= pDpvt->pSTDdev;
	pchannelConfig		= pDpvt->pSTDch;
	pcontrolThreadConfig		= ptaskConfig->pST_stdCtrlThread;
	qData.param.pSTDdev		= ptaskConfig;
	qData.param.pSTDch		= pchannelConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pDpvt->pSTDch->chanIndex; */
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase I %s (%s):%d\n", precord->name,
				                                epicsThreadGetNameSelf(), pDpvt->ind );
#endif
		switch(pDpvt->ind) {
			/* not related with channel.... */
			case BO_ABORT_GEN:
				qData.pFunc = devZT530_BO_ABORT_GEN;
				break;
			case BO_SIG_GEN:
				qData.pFunc = devZT530_BO_SIG_GEN;
				break;
			case BO_FUNC_GEN:
				qData.pFunc = devZT530_BO_FUNC_GEN;
				break;
			case BO_RESET_INIT:
				qData.pFunc = devZT530_BO_RESET_INIT;
				break;
				
			default: break;

		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
/*
		switch(pDpvt->ind) {
			case BO_ABORT_GEN: precord->val = 0; break;

		}
*/
		return 2;    /* don't convert */
	}

	return 0;
}


LOCAL long devAiZT530_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
		
	switch(precord->inp.type) {
		case INST_IO:
			i = sscanf(precord->inp.value.instio.string, "%s %s %s", pDpvt->devName, arg0, arg1);
#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devAiZT530 arg num: %d: %s %s %s\n",i, pDpvt->devName, 
								arg0,
								arg1);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiZT530 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			if( i > 2)
			{
				unsigned short chid;
				chid = strtoul(arg0, NULL, 0);
				pDpvt->pSTDch = get_STCh_from_devName_chID(pDpvt->devName, chid);
				if(!pDpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiZT530 (init_record) Illegal INP field: channel name");
					free(pDpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiZT530 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if( i == 2 ){
		if(!strcmp(arg0, AI_ERROR_CODE_STR))
			pDpvt->ind = AI_ERROR_CODE;
		
		
	} else {
	
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devAiZT530_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiZT530dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiZT530dpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiZT530dpvt->pSTDdev;
	if( pdevAiZT530dpvt->ind == AI_ERROR_CODE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else 
		*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

LOCAL long devAiZT530_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_STD_channel *pSTDch;


	if(!pDpvt) return 0;

	pSTDdev		= pDpvt->pSTDdev;
	pSTDch	= pDpvt->pSTDch;
	

	switch(pDpvt->ind) {
		case AI_ERROR_CODE:
			precord->val = pSTDdev->ErrorDev;
/*			epicsPrintf("%s taskStatus: 0x%X : 0x%X\n", pSTDdev->taskName, (int)precord->val, ptaskConfig->boardConfigure);  */
			break;
		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2);
}



