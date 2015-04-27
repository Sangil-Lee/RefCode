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

#include "drvNIfgen.h"
#include "pvListFgen.h"
#include <string.h>


typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];

	char recordName[80];

	ST_NIFGEN *ptaskConfig;
	ST_NIFGEN_channel *pchannelConfig;

	unsigned        ind;

} devNIfgenControldpvt;


LOCAL long devAoNIfgenControl_init_record(aoRecord *precord);
LOCAL long devAoNIfgenControl_write_ao(aoRecord *precord);

LOCAL long devAiNIfgen_init_record(aiRecord *precord);
LOCAL long devAiNIfgen_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiNIfgen_read_ai(aiRecord *precord);


LOCAL long devStringoutNIfgenControl_init_record(stringoutRecord *precord);
LOCAL long devStringoutNIfgenControl_write_stringout(stringoutRecord *precord);

LOCAL long devBoNIfgen_init_record(boRecord *precord);
LOCAL long devBoNIfgen_write_bo(boRecord *precord);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiNIfgen = {
	6,
	NULL,
	NULL,
	devAiNIfgen_init_record,
	devAiNIfgen_get_ioint_info,
	devAiNIfgen_read_ai,
	NULL
};
epicsExportAddress(dset, devAiNIfgen);


struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoNIfgen = {
    5,
    NULL,
    NULL,
    devBoNIfgen_init_record,
    NULL,
    devBoNIfgen_write_bo
};
epicsExportAddress(dset, devBoNIfgen);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoNIfgenControl = {
	6,
	NULL,
	NULL,
	devAoNIfgenControl_init_record,
	NULL,
	devAoNIfgenControl_write_ao,
	NULL
};
epicsExportAddress(dset, devAoNIfgenControl);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutNIfgenControl = {
	6,
	NULL,
	NULL,
	devStringoutNIfgenControl_init_record,
	NULL,
	devStringoutNIfgenControl_write_stringout,
	NULL
};
epicsExportAddress(dset, devStringoutNIfgenControl);


unsigned long long int start, stop, _interval;


LOCAL void devNIfgen_AO_EXPORT_SIG_IN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	char buf[32];

	switch( (int)pParam->setValue ) {
		case NI_FGEN_EXPORT_NOTHING: 
			pNI5412->exportSigIn = NIFGEN_VAL_NONE; /* only for 5404 .... reference niFgen.h */
			strcpy(buf, "None");
			break;
		case NI_FGEN_SIGNAL_START_TRIGGER: 
			pNI5412->exportSigIn = NIFGEN_VAL_START_TRIGGER;
			strcpy(buf, "Start Trigger");
			break;
		case NI_FGEN_SIGNAL_PFI_0: 
			pNI5412->exportSigIn = NIFGEN_VAL_PFI_0; /* only for 5404 .... reference niFgen.h */
			strcpy(buf, "PFI 0");
			break;
		case NI_FGEN_SIGNAL_PFI_1: 
			pNI5412->exportSigIn = NIFGEN_VAL_PFI_1; /* not used */
			strcpy(buf, "PFI 1");
			break;
		default: 
			epicsPrintf("ERROR! >>> %s: Input target signal is not defined! (%d) \n", pSTDdev->taskName,  (int)pParam->setValue );
			return;
	}
	
	if( drvNIfgen_ExportSignal(pSTDdev ) == WR_OK ){
		epicsPrintf(">>> %s: Export target signal is (%s) \n", pSTDdev->taskName,  buf );
	} else {
		epicsPrintf("ERROR! %s: Export target signal is (%s) \n", pSTDdev->taskName,  buf );
	}

}

LOCAL void devNIfgen_AO_EXPORT_SIG_TO(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;

	switch( (int)pParam->setValue ) {
		case NI_FGEN_EXPORT_NOTHING: strcpy( pNI5412->strExportSigTO, ""); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI0: strcpy( pNI5412->strExportSigTO, "PXI_Trig0"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI1: strcpy( pNI5412->strExportSigTO, "PXI_Trig1"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI2: strcpy( pNI5412->strExportSigTO, "RTSI2"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI3: strcpy( pNI5412->strExportSigTO, "RTSI3"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI4: strcpy( pNI5412->strExportSigTO, "RTSI4"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI5: strcpy( pNI5412->strExportSigTO, "RTSI5"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI6: strcpy( pNI5412->strExportSigTO, "RTSI6"); break;
		case NI_FGEN_EXPORT_SIG_TO_RTSI7: strcpy( pNI5412->strExportSigTO, "RTSI7"); break;
		case NI_FGEN_EXPORT_SIG_TO_RefOut: strcpy( pNI5412->strExportSigTO, "RefOut"); break;
		case NI_FGEN_EXPORT_SIG_TO_PFI0: strcpy( pNI5412->strExportSigTO, "PFI0"); break;
		case NI_FGEN_EXPORT_SIG_TO_PFI1: strcpy( pNI5412->strExportSigTO, "PFI1"); break;
		case NI_FGEN_EXPORT_SIG_TO_PXIStar: strcpy( pNI5412->strExportSigTO, "PXIStar"); break;
		default: 
			epicsPrintf("ERROR! >>> %s: Signal Source is wrong! (%d) \n", pSTDdev->taskName,  (int)pParam->setValue );
			return;
	}
	
	if( drvNIfgen_ExportSignal(pSTDdev ) == WR_OK ){
		epicsPrintf(">>> %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pNI5412->strExportSigTO );
	} else {
		epicsPrintf("ERROR! %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pNI5412->strExportSigTO );
	}

}

LOCAL void devNIfgen_AO_TRIG_TYPE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	pNI5412->trigType = (ViInt32)pParam->setValue;
	
	if( drvNIfgen_cfg_trigType(pSTDdev) == WR_OK ) {
		if( pNI5412->trigType == NO_TRIGGER ) {
			epicsPrintf(">>> %s:(%d)-NO_TRIGGER\n", pSTDdev->taskName, (int)pNI5412->trigType );
		} else if( pNI5412->trigType == SOFTWARE_TRIGGER ) {
			epicsPrintf(">>> %s:(%d)-SOFTWARE_TRIGGER (not used)\n", pSTDdev->taskName, (int)pNI5412->trigType );
		} else if( pNI5412->trigType == DIGITAL_TRIGGER ) {
			epicsPrintf(">>> %s:(%d)-DIGITAL_TRIGGER (%s)\n", pSTDdev->taskName, (int)pNI5412->trigType,
				pNI5412->triggerSource );
		}
	}
	else {
		pSTDdev->ErrorDev = NI5412_ERROR_TRIG_TYPE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	
}
LOCAL void devNIfgen_AO_TRIG_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	pNI5412->trigMode = (ViInt32)pParam->setValue;
	
	if( drvNIfgen_cfg_trigMode(pSTDdev) == WR_OK ) {
		switch( pNI5412->trigMode ) {
		case NIFGEN_VAL_SINGLE:
			epicsPrintf(">>> %s: Tirg Mode (%d) : SINGLE\n", pSTDdev->taskName, (int)pNI5412->trigMode ); break;
		case NIFGEN_VAL_CONTINUOUS:
			epicsPrintf(">>> %s: Tirg Mode (%d) : CONTINUOUS\n", pSTDdev->taskName, (int)pNI5412->trigMode ); break;
		case NIFGEN_VAL_STEPPED:
			epicsPrintf(">>> %s: Tirg Mode (%d) : STEPPED\n", pSTDdev->taskName, (int)pNI5412->trigMode ); break;
		case NIFGEN_VAL_BURST:
			epicsPrintf(">>> %s: Tirg Mode (%d) : BURST\n", pSTDdev->taskName, (int)pNI5412->trigMode ); break;
		}
	}
}
LOCAL void devNIfgen_AO_ARB_CH_OFFSET(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	
	double prev_offset;
	prev_offset = pchannelConfig->f64DCoffset;

	pchannelConfig->f64DCoffset = (double)pParam->setValue;
#if CFG_NI_FGEN_IMMEDIATELY
	if( drvNIfgen_cfg_arbitraryWF(pSTDdev, pSTDch) == WR_ERROR ) {
		pchannelConfig->f64DCoffset = prev_offset;
		pSTDdev->ErrorDev = NI5412_ERROR_OFFSET;
	} else {
		epicsPrintf(">>> %s/%s: Offset %f\n", pSTDdev->taskName, pSTDch->chName, pchannelConfig->f64DCoffset );
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devNIfgen_AO_ARB_CH_GAIN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	
	double prev_Gain;
	prev_Gain = pchannelConfig->f64gain;
	
	pchannelConfig->f64gain = (double)pParam->setValue;
#if CFG_NI_FGEN_IMMEDIATELY
	if( drvNIfgen_cfg_arbitraryWF(pSTDdev, pSTDch ) == WR_ERROR ) {
		pchannelConfig->f64gain = prev_Gain;
		pSTDdev->ErrorDev = NI5412_ERROR_GAIN;
	} else {
		epicsPrintf(">>> %s/%s: Gain %f\n", pSTDdev->taskName, pSTDch->chName, pchannelConfig->f64gain );
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devNIfgen_STRINGOUT_EXPORT_SIG_TO(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;

/*	[RTSI0|RTSI1|RTSI2|RTSI3|RTSI4|RTSI5|RTSI6|RTSI7|RefOut|PFI0|PFI1|PXIStar] */
	if( !strcmp("RTSI0", pParam->setStr) ||
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
		strcpy( pNI5412->strExportSigTO, pParam->setStr);
	} else {
		epicsPrintf("ERROR! >>> %s: Signal Source is wrong! \"%s\" \n", pSTDdev->taskName,  pParam->setStr );
		pSTDdev->ErrorDev = NI5412_ERROR_ROUTE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}
	
	if( drvNIfgen_ExportSignal(pSTDdev ) == WR_OK ){
		epicsPrintf(">>> %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pNI5412->strExportSigTO );

	} else {
		epicsPrintf("ERROR! %s: Export Signal to \"%s\" \n", pSTDdev->taskName,  pNI5412->strExportSigTO );
		pSTDdev->ErrorDev = NI5412_ERROR_ROUTE;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
	
}


LOCAL void devNIfgen_STRINGOUT_TRIG_SRC(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	
	strcpy( pNI5412->triggerSource, pParam->setStr);
	epicsPrintf(">>> %s: Trigger Source: %s\n", pSTDdev->taskName,  pNI5412->triggerSource );
}

LOCAL void devNIfgen_STRINGOUT_WF_FILENAME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	
	strcpy(pchannelConfig->wfmFileName, WF_ROOT_DIR);
	strcat(pchannelConfig->wfmFileName, pParam->setStr);

/*epicsPrintf(">>> %s/%s: waveform name %s\n", pSTDdev->taskName, pSTDch->chName, pchannelConfig->wfmFileName ); */

	if( drvNIfgen_create_WFdownload(pSTDdev, pSTDch ) == WR_OK ) {
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

/* not use this function.... we use "devNIfgen_STRINGOUT_WF_FILENAME"   */
LOCAL void devNIfgen_AO_CREATE_WF_DL(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
/*	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser; */
	
	if( drvNIfgen_create_WFdownload(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf(">>> %s/%s: Create the waveform and download.\n", pSTDdev->taskName, pSTDch->chName ); 
	} 

}

LOCAL void devNIfgen_AO_CFG_ARB_WF(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	
	if( drvNIfgen_cfg_arbitraryWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf(">>> %s/%s: Select the ARB Waveform to generate.\n", pSTDdev->taskName, pSTDch->chName ); 
		epicsPrintf("\nch name: %s, wfmHandle:%lu, Gain:%f, DCoffset:%f\n", pSTDch->chName,
		pchannelConfig->wfmHandle, 
		pchannelConfig->f64gain,
		pchannelConfig->f64DCoffset);
	} 
	else 
		pSTDdev->ErrorDev = NI5412_ERROR_CFG_ARB;
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIfgen_AO_CFG_STD_WF(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;

	if( drvNIfgen_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf(">>> %s/%s: Configure the STD Function to generate.\n", pSTDdev->taskName, pSTDch->chName ); 
		epicsPrintf("\nch name: %s, type:%d, amp:%f, offset:%f, freq:%f, ph:%f\n", pSTDch->chName,
						  pchannelConfig->wfmType,
						  pchannelConfig->f64Amplitude,
						  pchannelConfig->f64DCoffset,
						  pchannelConfig->f64Frequency,
						  pchannelConfig->f64StartPhase);
	} 
	else 
		pSTDdev->ErrorDev = NI5412_ERROR_CFG_STD;

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIfgen_AO_ENABLE_OUTPUT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
/*	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser; */
/*	struct dbCommon *precord = pParam->precord; */
	
	
	if( (epicsUInt16)pParam->setValue > 1){
		epicsPrintf(">>> %s/%s: Wrong arg (%d)\n", pSTDdev->taskName, pSTDch->chName, (epicsUInt16)pParam->setValue ); 
		return;
	}
	if( drvNIfgen_set_outputEnable(pSTDdev, pSTDch->chName, (ViBoolean)pParam->setValue ) == WR_OK ) {
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
LOCAL void devNIfgen_BO_ABORT_GEN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	
	if( drvNIfgen_abort_Generation(pSTDdev) == WR_OK ) {
		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
		epicsPrintf(">>> %s: generation aborted!\n", pSTDdev->taskName );
		scanIoRequest(pSTDdev->ioScanPvt_status);
	}
	notify_refresh_master_status();
}

LOCAL void devNIfgen_BO_SIG_GEN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;	

	if( (ViInt32)pParam->setValue ) {
		if( drvNIfgen_init_Generation(pSTDdev) == WR_OK ) {
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
		if( drvNIfgen_abort_Generation(pSTDdev) == WR_OK ) {
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

LOCAL void devNIfgen_AO_WFM_TYPE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	
	pchannelConfig->wfmType = (ViInt32)pParam->setValue;
	switch(pchannelConfig->wfmType){
		case NIFGEN_VAL_WFM_SINE: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_SINE\n", pSTDdev->taskName, pSTDch->chName); break;
		case NIFGEN_VAL_WFM_SQUARE: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_SQUARE\n", pSTDdev->taskName, pSTDch->chName); break;
		case NIFGEN_VAL_WFM_TRIANGLE: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_TRIANGLE\n", pSTDdev->taskName, pSTDch->chName); break;
		case NIFGEN_VAL_WFM_RAMP_UP: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_RAMP_UP\n", pSTDdev->taskName, pSTDch->chName); break;
		case NIFGEN_VAL_WFM_RAMP_DOWN: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_RAMP_DOWN\n", pSTDdev->taskName, pSTDch->chName); break;
		case NIFGEN_VAL_WFM_DC: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_DC\n", pSTDdev->taskName, pSTDch->chName); break;
		case NIFGEN_VAL_WFM_NOISE: 
			epicsPrintf(">>> %s/%s: NIFGEN_VAL_WFM_NOISE\n", pSTDdev->taskName, pSTDch->chName); break;
		default: 
			epicsPrintf(">>> %s/%s: Wrong type (%lu)\n", pSTDdev->taskName, pSTDch->chName, (ViInt32)pParam->setValue ); 
			break;
	}

}

LOCAL void devNIfgen_AO_AMPLITUDE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64Amplitude;
/* max val: 12	, 24Vpp */
	pchannelConfig->f64Amplitude = pParam->setValue;

#if CFG_NI_FGEN_IMMEDIATELY
	if( drvNIfgen_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf("control thread (set Amplitude): %s/%s %s (%s): %lfVpp\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64Amplitude * 2 );
	} else {
		epicsPrintf("Maximum value : 12 \n");
		pchannelConfig->f64Amplitude = fVal;
		pSTDdev->ErrorDev = NI5412_ERROR_AMPLITUDE;
	}	
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIfgen_AO_DC_OFFSET(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64DCoffset;
	
	pchannelConfig->f64DCoffset = pParam->setValue;
#if CFG_NI_FGEN_IMMEDIATELY	
	if( drvNIfgen_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
		epicsPrintf("control thread (set DCoffset): %s/%s %s (%s): %lf\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64DCoffset );
	} else {
		pchannelConfig->f64DCoffset = fVal;
		pSTDdev->ErrorDev = NI5412_ERROR_OFFSET;
	}
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devNIfgen_AO_FREQ(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64Frequency;
/* Max: 20MHz */	
	pchannelConfig->f64Frequency = pParam->setValue;

#if CFG_NI_FGEN_IMMEDIATELY
	if( drvNIfgen_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
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
LOCAL void devNIfgen_AO_PHASE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;
	double fVal = pchannelConfig->f64StartPhase;

	pchannelConfig->f64StartPhase = pParam->setValue;
#if CFG_NI_FGEN_IMMEDIATELY
	if( drvNIfgen_cfg_StandardWF(pSTDdev, pSTDch ) == WR_OK ) {
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
LOCAL void devNIfgen_AO_CLOCK_SOURCE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NIFGEN_channel *pchannelConfig = (ST_NIFGEN_channel*)pSTDch->pUser;
	struct dbCommon *precord = pParam->precord;

	epicsPrintf("control thread (xxx): %s/%s %s (%s): %lf\n", pSTDdev->taskName, pSTDch->chName, 
											precord->name, epicsThreadGetNameSelf(), pchannelConfig->f64DCoffset );
}

LOCAL void devNIfgen_AO_ARB_SAMPLE_RATE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	double fval = pNI5412->sampleRate;

	pNI5412->sampleRate = pParam->setValue;
	if( drvNIfgen_arb_sampleRate(pSTDdev) == WR_OK ) {
		epicsPrintf("control thread (arb sample rate): %s (%s): %lfKHz\n", 
									precord->name, epicsThreadGetNameSelf(), pNI5412->sampleRate/1000. );
	} else {
		pNI5412->sampleRate = fval;
		pSTDdev->ErrorDev = NI5412_ERROR_FREQ;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIfgen_AO_CLOCK_MODE(ST_execParam *pParam)
{
	ViInt32 n32Prev;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	
	n32Prev = pNI5412->clockMode;
	pNI5412->clockMode = (ViInt32)pParam->setValue;
	if( drvNIfgen_set_clockMode(pSTDdev) == WR_OK ) {
		switch(pNI5412->clockMode){
			case NIFGEN_VAL_HIGH_RESOLUTION: 
				epicsPrintf(">>> %s: clock mode: HIGH_RESOLUTION\n", pSTDdev->taskName); break;
			case NIFGEN_VAL_DIVIDE_DOWN: 
				epicsPrintf(">>> %s: clock mode: DIVIDE_DOWN\n", pSTDdev->taskName); break;
			case NIFGEN_VAL_AUTOMATIC: 
				epicsPrintf(">>> %s: clock mode: AUTOMATIC\n", pSTDdev->taskName); break;
			default: 
				epicsPrintf(">>> %s: Wrong type (%lu)\n", pSTDdev->taskName, (ViInt32)pParam->setValue ); 
				pNI5412->clockMode = n32Prev;
				break;
		}
	} else {
		pNI5412->clockMode = n32Prev;
		pSTDdev->ErrorDev = NI5412_ERROR_CLOCK_MODE;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
}

				
				
				
LOCAL void devNIfgen_AO_OUTPUT_MODE(ST_execParam *pParam)
{
	ViInt32 n32Prev;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
/*	struct dbCommon *precord = pParam->precord; */
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;

	
	switch((ViInt32)pParam->setValue){
		case NIFGEN_VAL_OUTPUT_FUNC: /* 0L */
			epicsPrintf(">>> %s: Configure output for standard function mode...", pSTDdev->taskName); 
			break;
		case NIFGEN_VAL_OUTPUT_ARB: /* 1L */
			epicsPrintf(">>> %s: Configure output for arbitrary mode...", pSTDdev->taskName); 
			break;
		case NIFGEN_VAL_OUTPUT_SEQ: /* 2L */
			epicsPrintf(">>> %s: Configure output for sequence mode...", pSTDdev->taskName); 
			break;
		default: 
			epicsPrintf(">>> %s: got the wrong output mode (%lu)\n", pSTDdev->taskName, (ViInt32)pParam->setValue ); 
			return;
	}
	n32Prev = pNI5412->outputMode;
	pNI5412->outputMode = (ViInt32)pParam->setValue;
	if( drvNIfgen_cfg_ouputMode(pSTDdev) == WR_ERROR) {		
		epicsPrintf("\n>>> devNIfgen_set_ouputMode() : drvNIfgen_set_ouputMode()... failed\n");
		pNI5412->outputMode = n32Prev;
		pSTDdev->ErrorDev = NI5412_ERROR_OUT_MODE;
		return ;
	} 
	else {
		epicsPrintf("OK!\n");
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
	
/*	epicsPrintf("control thread (set output mode): %s %s (%s)\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf());
*/
}


LOCAL void devNIfgen_test_put(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

/*	ptaskConfig->fT0 = (float)pParam->setValue; */
	drvNIfgen_abort_Generation(pSTDdev);
/*
	mds_createNewShot( (int)pParam->setValue );
*/	
	epicsPrintf("control thread (test PUT): %s %s (%s), val: %f\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), (float)pParam->setValue);
}

LOCAL long devAoNIfgenControl_init_record(aoRecord *precord)
{
	ST_dpvt *pdevNIfgenControldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevNIfgenControldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevNIfgenControldpvt->devName,
			           pdevNIfgenControldpvt->arg0,
			           pdevNIfgenControldpvt->arg1);

#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devAoNIfgenControl arg num: %d: %s %s %s\n",i, pdevNIfgenControldpvt->devName, 
								pdevNIfgenControldpvt->arg0, pdevNIfgenControldpvt->arg1);
#endif
			pdevNIfgenControldpvt->pSTDdev = get_STDev_from_name(pdevNIfgenControldpvt->devName);
			if(!pdevNIfgenControldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoNIfgenControl (init_record) Illegal INP field: task_name");
				free(pdevNIfgenControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			if( i> 2)  {
				unsigned short chid;
				chid = strtoul(pdevNIfgenControldpvt->arg0, NULL, 0);
				pdevNIfgenControldpvt->pSTDch = get_STCh_from_devName_chID(pdevNIfgenControldpvt->devName,
												chid);
				if(!pdevNIfgenControldpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAoNIfgenControld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n",
						    pdevNIfgenControldpvt->recordName,
						    pdevNIfgenControldpvt->devName,
						    pdevNIfgenControldpvt->arg0,
						    pdevNIfgenControldpvt->arg1);
					free(pdevNIfgenControldpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			}
		

			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoNIfgenControl (init_record) Illegal OUT field");
			free(pdevNIfgenControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if( i==2){
		if (!strcmp(pdevNIfgenControldpvt->arg0, CONTROL_TEST_PUT_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_TEST_PUT;
		} else if (!strcmp(pdevNIfgenControldpvt->arg0, CONTROL_SET_OUTPUT_MODE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_OUTPUT_MODE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg0, CONTROL_SET_CLOCK_SRC_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_CLOCK_SRC;
		} else if (!strcmp(pdevNIfgenControldpvt->arg0, CONTROL_ARB_SAMPLE_RATE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_ARB_SAMPLE_RATE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg0, CONTROL_SET_CLOCK_MODE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_CLOCK_MODE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg0, AO_TRIG_TYPE_STR)) {
			pdevNIfgenControldpvt->ind = AO_TRIG_TYPE;
		}  else if (!strcmp(pdevNIfgenControldpvt->arg0, AO_EXPORT_SIG_IN_STR)) {
			pdevNIfgenControldpvt->ind = AO_EXPORT_SIG_IN;
		} else if (!strcmp(pdevNIfgenControldpvt->arg0, AO_EXPORT_SIG_TO_STR)) {
			pdevNIfgenControldpvt->ind = AO_EXPORT_SIG_TO;
		}
		


		 
	} else if( i==3 ) {
		if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_SET_WFM_TYPE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_WFM_TYPE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_SET_AMPLITUDE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_AMPLITUDE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_SET_DCOFFSET_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_DCOFFSET;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_SET_FREQUENCY_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_FREQUENCY;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_SET_PHASE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_PHASE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_SET_OUTPUT_ENABLE_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_SET_OUTPUT_ENABLE;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_CFG_STD_WF_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_CFG_STD_WF;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_CREATE_WF_DOWNLOAD_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_CREATE_WF_DOWNLOAD;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_CFG_ARB_WF_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_CFG_ARB_WF;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, AO_ARB_CH_GAIN_STR)) {
			pdevNIfgenControldpvt->ind = AO_ARB_CH_GAIN;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, AO_ARB_CH_OFFSET_STR)) {
			pdevNIfgenControldpvt->ind = AO_ARB_CH_OFFSET;
		} else if (!strcmp(pdevNIfgenControldpvt->arg1, AO_TRIG_MODE_STR)) {
			pdevNIfgenControldpvt->ind = AO_TRIG_MODE;
		} 
		
				
	}
	
	
	


	precord->udf = FALSE;
	precord->dpvt = (void*) pdevNIfgenControldpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoNIfgenControl_write_ao(aoRecord *precord)
{
#if SHOW_PHASE_NIFGEN
	static int   kkh_cnt;
#endif
	ST_dpvt			*pdevNIfgenControldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device   *ptaskConfig;
	ST_STD_channel  *pchannelConfig;
	ST_threadCfg  *pcontrolThreadConfig;
	ST_threadQueueData			qData;

	if(!pdevNIfgenControldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig			= pdevNIfgenControldpvt->pSTDdev;
	pchannelConfig		= pdevNIfgenControldpvt->pSTDch;
	pcontrolThreadConfig		= ptaskConfig->pST_stdCtrlThread;
	qData.param.pSTDdev		= ptaskConfig;
	qData.param.pSTDch		= pchannelConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pdevNIfgenControldpvt->pSTDch->chanIndex; */
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif

		switch(pdevNIfgenControldpvt->ind) {
			case CONTROL_TEST_PUT:
				qData.pFunc = devNIfgen_test_put;
				break;
			case CONTROL_SET_OUTPUT_MODE:
				qData.pFunc = devNIfgen_AO_OUTPUT_MODE;
				break;
			case CONTROL_SET_CLOCK_SRC:
				qData.pFunc = devNIfgen_AO_CLOCK_SOURCE;
				break;
			case CONTROL_ARB_SAMPLE_RATE:
				qData.pFunc = devNIfgen_AO_ARB_SAMPLE_RATE;
				break;
			case CONTROL_SET_CLOCK_MODE:
				qData.pFunc = devNIfgen_AO_CLOCK_MODE;
				break;


			case CONTROL_SET_WFM_TYPE: qData.pFunc = devNIfgen_AO_WFM_TYPE; break;
			case CONTROL_SET_AMPLITUDE: qData.pFunc = devNIfgen_AO_AMPLITUDE; break;
			case CONTROL_SET_DCOFFSET: qData.pFunc = devNIfgen_AO_DC_OFFSET; break;
			case CONTROL_SET_FREQUENCY: qData.pFunc = devNIfgen_AO_FREQ; break;
			case CONTROL_SET_PHASE: qData.pFunc = devNIfgen_AO_PHASE; break;
			case CONTROL_SET_OUTPUT_ENABLE: qData.pFunc = devNIfgen_AO_ENABLE_OUTPUT; break;
				
			case CONTROL_CFG_STD_WF: qData.pFunc = devNIfgen_AO_CFG_STD_WF; break;
			case CONTROL_CREATE_WF_DOWNLOAD: qData.pFunc = devNIfgen_AO_CREATE_WF_DL; break;
			case CONTROL_CFG_ARB_WF: qData.pFunc = devNIfgen_AO_CFG_ARB_WF; break;
			case AO_ARB_CH_GAIN: qData.pFunc = devNIfgen_AO_ARB_CH_GAIN; break;
			case AO_ARB_CH_OFFSET: qData.pFunc = devNIfgen_AO_ARB_CH_OFFSET; break;
			case AO_TRIG_MODE: qData.pFunc = devNIfgen_AO_TRIG_MODE; break;
			case AO_TRIG_TYPE: qData.pFunc = devNIfgen_AO_TRIG_TYPE; break;
			case AO_EXPORT_SIG_TO: qData.pFunc = devNIfgen_AO_EXPORT_SIG_TO; break;
			case AO_EXPORT_SIG_IN: qData.pFunc = devNIfgen_AO_EXPORT_SIG_IN; break;

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
		
		switch(pdevNIfgenControldpvt->ind) {
			case CONTROL_TEST_PUT: precord->val = 0; break;
			case CONTROL_CFG_STD_WF: precord->val = 0; break;
			case CONTROL_CFG_ARB_WF: precord->val = 0; break;
		}

		return 2;    /* don't convert */
	}

	return 0;
}

LOCAL long devStringoutNIfgenControl_init_record(stringoutRecord *precord)
{
	ST_dpvt *pdevNIfgenControldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevNIfgenControldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevNIfgenControldpvt->devName,
			           pdevNIfgenControldpvt->arg0,
			           pdevNIfgenControldpvt->arg1);

#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devStringoutNIfgenControl arg num: %d: %s %s %s\n",i, pdevNIfgenControldpvt->devName, 
								pdevNIfgenControldpvt->arg0, pdevNIfgenControldpvt->arg1);
#endif
			pdevNIfgenControldpvt->pSTDdev = get_STDev_from_name(pdevNIfgenControldpvt->devName);
			if(!pdevNIfgenControldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutNIfgenControl (init_record) Illegal INP field: task_name");
				free(pdevNIfgenControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			
			if( i> 2)  {
				unsigned short chid;
				chid = strtoul(pdevNIfgenControldpvt->arg0, NULL, 0);
				pdevNIfgenControldpvt->pSTDch = get_STCh_from_devName_chID(pdevNIfgenControldpvt->devName,
												chid);
				if(!pdevNIfgenControldpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAoNIfgenControld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n",
						    pdevNIfgenControldpvt->recordName,
						    pdevNIfgenControldpvt->devName,
						    pdevNIfgenControldpvt->arg0,
						    pdevNIfgenControldpvt->arg1);
					free(pdevNIfgenControldpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			}

				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutNIfgenControl (init_record) Illegal OUT field");
			free(pdevNIfgenControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if( i==2 ) {
		if(!strcmp(pdevNIfgenControldpvt->arg0, STRINGOUT_TRIG_SRC_STR)) {
			pdevNIfgenControldpvt->ind = STRINGOUT_TRIG_SRC;
		} else if(!strcmp(pdevNIfgenControldpvt->arg0, STRINGOUT_EXPORT_SIG_TO_STR)) {
			pdevNIfgenControldpvt->ind = STRINGOUT_EXPORT_SIG_TO;
		} 

		


	} else if( i==3 ) {
		if(!strcmp(pdevNIfgenControldpvt->arg1, CONTROL_WF_FILENAME_STR)) {
			pdevNIfgenControldpvt->ind = CONTROL_WF_FILENAME;
		} 
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevNIfgenControldpvt;
	
	return 2;     /* don't convert */
}


LOCAL long devStringoutNIfgenControl_write_stringout(stringoutRecord *precord)
{
	ST_dpvt			*pdevNIfgenControldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *ptaskConfig;
	ST_STD_channel *pchannelConfig;
	ST_threadCfg *pcontrolThreadConfig;
	ST_threadQueueData qData;
	
	if(!pdevNIfgenControldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}
	
	ptaskConfig			= pdevNIfgenControldpvt->pSTDdev;
	pchannelConfig		= pdevNIfgenControldpvt->pSTDch;
	pcontrolThreadConfig		= ptaskConfig->pST_stdCtrlThread;
	qData.param.pSTDdev		= ptaskConfig;
	qData.param.pSTDch		= pchannelConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pdevNIfgenControldpvt->pSTDch->chanIndex; */

	strcpy( qData.param.setStr, precord->val ); 

	/* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif

		switch(pdevNIfgenControldpvt->ind) {
			case CONTROL_WF_FILENAME:
				qData.pFunc = devNIfgen_STRINGOUT_WF_FILENAME;
				break;
			case STRINGOUT_TRIG_SRC:
				qData.pFunc = devNIfgen_STRINGOUT_TRIG_SRC;
				break;
			case STRINGOUT_EXPORT_SIG_TO:
				qData.pFunc = devNIfgen_STRINGOUT_EXPORT_SIG_TO;
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

LOCAL long devBoNIfgen_init_record(boRecord *precord)
{
    ST_dpvt *pdevNIfgendpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
    int i;

    switch(precord->out.type) {
        case INST_IO:
        	strcpy(pdevNIfgendpvt->recordName, precord->name);
            i = sscanf(precord->out.value.instio.string, "%s %s %s", pdevNIfgendpvt->devName,
                                                                 pdevNIfgendpvt->arg0,
                                                                 pdevNIfgendpvt->arg1);
#if PRINT_DEV_SUPPORT_ARG
			epicsPrintf("devBoNIfgen arg num: %d: %s %s %s\n",i, pdevNIfgendpvt->devName, 
								pdevNIfgendpvt->arg0, pdevNIfgendpvt->arg1);
#endif
            pdevNIfgendpvt->pSTDdev = get_STDev_from_name(pdevNIfgendpvt->devName);
            if(!pdevNIfgendpvt->devName) { /* port not found */
                recGblRecordError(S_db_badField, (void*)precord,
                                  "devBoNIfgen (init_record) Illegal OUT field: port not found");
                free(pdevNIfgendpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            if( i> 2)  {
				unsigned short chid;
				chid = strtoul(pdevNIfgendpvt->arg0, NULL, 0);
				pdevNIfgendpvt->pSTDch = get_STCh_from_devName_chID(pdevNIfgendpvt->devName,
												chid);
				if(!pdevNIfgendpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devBoNIfgenControld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n",
						    pdevNIfgendpvt->recordName,
						    pdevNIfgendpvt->devName,
						    pdevNIfgendpvt->arg0,
						    pdevNIfgendpvt->arg1);
					free(pdevNIfgendpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			} /* end of switch for pdevNIfgendpvt->param_first */
			
			
			
            break;  /* end of inst_io */
        default:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devBoNIfgen (init_record) Illegal OUT field");
            free(pdevNIfgendpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    } /* end of switch statement for precord->out.type */
    
    
    if( i==2){
		if (!strcmp(pdevNIfgendpvt->arg0, BO_ABORT_GEN_STR)) {
			pdevNIfgendpvt->ind = BO_ABORT_GEN;
		} else if( !strcmp(pdevNIfgendpvt->arg0, BO_SIG_GEN_STR)) {
			pdevNIfgendpvt->ind = BO_SIG_GEN;
		}
		 
	} else if( i==3 ) {

	}

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevNIfgendpvt;
    return 0;
}


LOCAL long devBoNIfgen_write_bo(boRecord *precord)
{
	ST_dpvt			*pdevNIfgenControldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *ptaskConfig;
	ST_STD_channel *pchannelConfig;
	ST_threadCfg			*pcontrolThreadConfig;
	ST_threadQueueData			qData;


    if(!precord->dpvt || precord->udf == TRUE) {
        precord->pact = TRUE;
        return 0;
	}

	ptaskConfig			= pdevNIfgenControldpvt->pSTDdev;
	pchannelConfig		= pdevNIfgenControldpvt->pSTDch;
	pcontrolThreadConfig		= ptaskConfig->pST_stdCtrlThread;
	qData.param.pSTDdev		= ptaskConfig;
	qData.param.pSTDch		= pchannelConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pdevNIfgenControldpvt->pSTDch->chanIndex; */
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NIFGEN
		epicsPrintf("db processing: phase I %s (%s):%d\n", precord->name,
				                                epicsThreadGetNameSelf(), pdevNIfgenControldpvt->ind );
#endif
		switch(pdevNIfgenControldpvt->ind) {
			/* not related with channel.... */
			case BO_ABORT_GEN:
				qData.pFunc = devNIfgen_BO_ABORT_GEN;
				break;
			case BO_SIG_GEN:
				qData.pFunc = devNIfgen_BO_SIG_GEN;
				break;

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
		switch(pdevNIfgenControldpvt->ind) {
			case BO_ABORT_GEN: precord->val = 0; break;

		}
*/
		return 2;    /* don't convert */
	}

	return 0;
}


LOCAL long devAiNIfgen_init_record(aiRecord *precord)
{
	ST_dpvt *pdevAiNIfgendpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
		
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiNIfgendpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiNIfgendpvt->devName,
				   pdevAiNIfgendpvt->arg0,
				   pdevAiNIfgendpvt->arg1);
#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devAiNIfgen arg num: %d: %s %s %s\n",i, pdevAiNIfgendpvt->devName, 
								pdevAiNIfgendpvt->arg0,
								pdevAiNIfgendpvt->arg1);
#endif
			pdevAiNIfgendpvt->pSTDdev = get_STDev_from_name(pdevAiNIfgendpvt->devName);
			if(!pdevAiNIfgendpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiNIfgen (init_record) Illegal INP field: task name");
				free(pdevAiNIfgendpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			if( i > 2)
			{
				unsigned short chid;
				chid = strtoul(pdevAiNIfgendpvt->arg0, NULL, 0);
				pdevAiNIfgendpvt->pSTDch = get_STCh_from_devName_chID(pdevAiNIfgendpvt->devName,
											         chid);
				if(!pdevAiNIfgendpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiNIfgen (init_record) Illegal INP field: channel name");
					free(pdevAiNIfgendpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiNIfgen (init_record) Illegal INP field");
			free(pdevAiNIfgendpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if( i == 2 ){
		if(!strcmp(pdevAiNIfgendpvt->arg0, AI_ERROR_CODE_STR))
			pdevAiNIfgendpvt->ind = AI_ERROR_CODE;
		
		
	} else {
	
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiNIfgendpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devAiNIfgen_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiNIfgendpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiNIfgendpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiNIfgendpvt->pSTDdev;
	if( pdevAiNIfgendpvt->ind == AI_ERROR_CODE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else 
		*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

LOCAL long devAiNIfgen_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pdevAiNIfgendpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_STD_channel *pSTDch;


	if(!pdevAiNIfgendpvt) return 0;

	pSTDdev		= pdevAiNIfgendpvt->pSTDdev;
	pSTDch	= pdevAiNIfgendpvt->pSTDch;
	

	switch(pdevAiNIfgendpvt->ind) {
		case AI_ERROR_CODE:
			precord->val = pSTDdev->ErrorDev;
/*			epicsPrintf("%s taskStatus: 0x%X : 0x%X\n", pSTDdev->taskName, (int)precord->val, ptaskConfig->boardConfigure);  */
			break;
		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevAiNIfgendpvt->ind); 
			break;
	}
	return (2);
}



