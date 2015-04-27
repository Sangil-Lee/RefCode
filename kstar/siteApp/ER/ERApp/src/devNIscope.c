
#include "aiRecord.h"
#include "aoRecord.h"
#include "boRecord.h"
#include "stringoutRecord.h"
#include "dbAccess.h"  /*#include "dbAccessDefs.h" --> S_db_badField */


#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvNIscope.h"
#include "pvListScope.h"
#include "myMDSplus.h"

LOCAL long devAoNIscopeControl_init_record(aoRecord *precord);
LOCAL long devAoNIscopeControl_write_ao(aoRecord *precord);

LOCAL long devStringoutNIscopeControl_init_record(stringoutRecord *precord);
LOCAL long devStringoutNIscopeControl_write_stringout(stringoutRecord *precord);


LOCAL long devBoNIscope_init_record(boRecord *precord);
LOCAL long devBoNIscope_write_bo(boRecord *precord);

LOCAL long devAiNIscope_init_record(aiRecord *precord);
LOCAL long devAiNIscope_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiNIscope_read_ai(aiRecord *precord);
struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiNIscope = {
	6,
	NULL,
	NULL,
	devAiNIscope_init_record,
	devAiNIscope_get_ioint_info,
	devAiNIscope_read_ai,
	NULL
};
epicsExportAddress(dset, devAiNIscope);



struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoNIscope = {
    5,
    NULL,
    NULL,
    devBoNIscope_init_record,
    NULL,
    devBoNIscope_write_bo
};
epicsExportAddress(dset, devBoNIscope);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoNIscopeControl = {
	6,
	NULL,
	NULL,
	devAoNIscopeControl_init_record,
	NULL,
	devAoNIscopeControl_write_ao,
	NULL
};
epicsExportAddress(dset, devAoNIscopeControl);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutNIscopeControl = {
	6,
	NULL,
	NULL,
	devStringoutNIscopeControl_init_record,
	NULL,
	devStringoutNIscopeControl_write_stringout,
	NULL
};
epicsExportAddress(dset, devStringoutNIscopeControl);



/*
int g_ShotNumber = 1;
int g_OpMode = OPMODE_TEST;
*/

LOCAL void devNIscope_STRINGOUT_TRIG_SRC(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
		
	strcpy(pNI5122->triggerSource, pParam->setStr);
	if( drvNIscope_cfg_triggerEdge(pSTDdev) == WR_OK ) {
		epicsPrintf(">>> %s: Trigger analog edge (src:%s)\n", pSTDdev->taskName, pNI5122->triggerSource ); 
	}
	else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_TRIGER;

	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devNIscope_STRINGOUT_TAG_NAME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;
	char buffer[100], fbits[100];

	if( strcmp(pSTDch->strTagName, pParam->setStr) != 0 ) 
	{
		strcpy( pSTDch->strTagName, pParam->setStr);
		sprintf(buffer, "%s:RAW", pParam->setStr);
		sprintf(fbits, "%s:FBITS", pParam->setStr);
		epicsPrintf("%s/%s: RAW: %s, FBITS: %s\n",pSTDdev->taskName, pSTDch->chName, buffer, fbits);
		
#if USE_SEGMENTED_ARCHIVING
		// 3-2. add nodes to Tree object: last argument is temporary file name.
		pNI5122_ch->pSfwTreeNode = sfwTreeAddNode (pNI5122->pSfwTree, buffer, sfwTreeNodeWaveform, sfwTreeNodeFloat64, NULL);
		if (NULL == pNI5122_ch->pSfwTreeNode) {
			kuDebug (kuERR, "cannot create TreeNode object\n");
		}

		pNI5122_ch->pSfwTreeScaleNode = sfwTreeAddNode (pNI5122->pSfwTree, fbits, sfwTreeNodeValue, sfwTreeNodeFloat64, NULL);
		if (NULL == pNI5122_ch->pSfwTreeScaleNode) {
			kuDebug (kuERR, "cannot create TreeScaleNode object\n");
		}
#endif

	}
	
}

LOCAL void devNIscope_BO_TREE_PUT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	epicsPrintf(">>> %s: Do all of channels put process... \n", pSTDdev->taskName);

	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
	pNI5122->myStatusDev &= ~SCOPE_STORAGE_FINISH;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
#if 0
	if( pAdminCfg->ST_Base.opMode != OPMODE_REMOTE )
		dbProc_call_DATA_PROCESS(pSTDdev, 1); /* 2009-11-18 */
#endif
	mds_Connect(pSTDdev);

	mds_Disconnect(pSTDdev);

	pNI5122->myStatusDev |= SCOPE_STORAGE_FINISH;
	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;	
	pSTDdev->StatusDev |= TASK_STANDBY;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	scanIoRequest(pSTDdev->ioScanPvt_status);

	notify_refresh_master_status();
#if 0
	if( pAdminCfg->ST_Base.opMode != OPMODE_REMOTE )
		dbProc_call_DATA_PROCESS(pSTDdev, 0); /* 2009-11-18 */
#endif
	epicsPrintf("OK\n");
}


LOCAL void devNIscope_STRINGOUT_WF_FILENAME(ST_execParam *pParam)
{
	FILE *fp = NULL;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;
/*	struct dbCommon *precord = pParam->precord;	*/
/*	char buf[128];
	sprintf(buf, "%s:%s.txt", pSTDdev->taskName, pSTDch->chName);
	strcat(pParam->setStr, buf);
*/	strcpy( pNI5122_ch->cstrWFfileName, pParam->setStr);

#if 1
	fp = fopen(pNI5122_ch->cstrWFfileName, "w");
	if (fp) {
		int i;
		for (i = 0; i<pNI5122->n32ActualRecordLength; i++) {
			fprintf(fp,"%d: %f\n", i, pNI5122_ch->f64WaveformPtr[i]);
		}
		fclose(fp);
	} else
		epicsPrintf("\n>>> file open error!\n");
#endif
	
	epicsPrintf(">>> %s/%s: WF file name \"%s\"\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->cstrWFfileName );
}

LOCAL void devNIscope_AO_TRIG_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	switch((int)pParam->setValue){
		case 0: 
			if( drvNIscope_set_trigImmediate(pSTDdev) == WR_OK)
				epicsPrintf(">>> %s: Trigger Immediate\n", pSTDdev->taskName );
			else 
				pSTDdev->ErrorDev = NISCOPE_ERROR_TRIGER;

			break;
		case 1:
			strcpy(pNI5122->triggerSource, "TRIG");
			if( drvNIscope_cfg_triggerEdge(pSTDdev) == WR_OK )
				epicsPrintf(">>> %s: Trigger analog edge (src:%s)\n", pSTDdev->taskName, pNI5122->triggerSource ); 
			else 
				pSTDdev->ErrorDev = NISCOPE_ERROR_TRIGER;
			
			break;

		default: 
			pSTDdev->ErrorDev = NISCOPE_ERROR_TRIGER;
			epicsPrintf("ERROR! %s: got %d (0:immediate, 1:PF0 rising)\n", pSTDdev->taskName, (int)pParam->setValue ); 
			break;
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
}
	
LOCAL void devNIscope_AO_SAMPLE_RATE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	pSTDdev->StatusDev &= ~TASK_STANDBY;
	pNI5122->f64minSampleRate = (double)pParam->setValue;
#if CFG_NI_SCOPE_IMMEDIATELY
	if( drvNIscope_h_Cfg_board(pSTDdev) == WR_OK) {
		epicsPrintf(">>> %s: Sampling rate: %fKHz\n", 
			pSTDdev->taskName, pNI5122->f64minSampleRate/1000.0 );
	}
	else pSTDdev->ErrorDev = NISCOPE_ERROR_SMP_RATE;
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();

}
LOCAL void devNIscope_AO_REC_LENGTH(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	pSTDdev->StatusDev &= ~TASK_STANDBY;
	pNI5122->n32minRecordLength = (double)pParam->setValue;
#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_h_Cfg_board(pSTDdev) == WR_OK) {
		pNI5122->myStatusDev &= ~SCOPE_GET_ACTUAL_CONFIRM;
		epicsPrintf(">>> %s: Record Length: %d\n", pSTDdev->taskName, (int)pNI5122->n32minRecordLength );
	}
	else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_REC_LENGTH;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();

}

LOCAL void devNIscope_AO_REC_NUM(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	pSTDdev->StatusDev &= ~TASK_STANDBY;
	pNI5122->n32numRecords = (double)pParam->setValue;
#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_h_Cfg_board(pSTDdev) == WR_OK) {
		pNI5122->myStatusDev &= ~SCOPE_GET_ACTUAL_CONFIRM;
		epicsPrintf(">>> %s: Record number: %d\n", pSTDdev->taskName, (int)pNI5122->n32numRecords );
	}
	else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_REC_NUMBER;
#endif

	if ( pNI5122->n32numRecords == 1 ) {
		pNI5122->useMultiTrigger = 0;
		printf("%s: use single trigger input!\n", pSTDdev->taskName );
	}
	else {
		pNI5122->useMultiTrigger = 1;
		printf("%s: use multi trigger input!\n", pSTDdev->taskName );
	}

	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
}

LOCAL void devNIscope_AO_INPUT_RANGE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	pNI5122_ch->f64VRange = (double)pParam->setValue;
#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_v_Cfg_board(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Vertical input range %fVpp\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->f64VRange );
	}
	else pSTDdev->ErrorDev = NISCOPE_ERROR_VPP_RANGE;
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devNIscope_AO_OFFSET(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	pNI5122_ch->f64VOffset = (double)pParam->setValue;
#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_v_Cfg_board(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Vertical Offset %f\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->f64VOffset );
	}
	else pSTDdev->ErrorDev = NISCOPE_ERROR_OFFSET;
#endif
	scanIoRequest(pSTDdev->ioScanPvt_status);

}
LOCAL void devNIscope_AO_PROBE_ATTENU(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	pNI5122_ch->f64probeAttenuation = (double)pParam->setValue;
#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_v_Cfg_board(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Probe attenuation: %f\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->f64probeAttenuation );
	}
	else pSTDdev->ErrorDev = NISCOPE_ERROR_PROBE_ATTEN;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devNIscope_AO_FILTER_TYPE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	if( pParam->setValue < 0 || pParam->setValue > 3 ) {
		epicsPrintf(">>> %s/%s: Input value error!(%d)\n", 
			pSTDdev->taskName, pSTDch->chName, (int)pParam->setValue);
		pSTDdev->ErrorDev = NISCOPE_ERROR_FILTER_TYPE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}

	pNI5122_ch->filterType = (int)pParam->setValue;

#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_set_filterType(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Filter type: %d\n", 
			pSTDdev->taskName, pSTDch->chName, (int)pNI5122_ch->filterType);
	} else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_FILTER_TYPE;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIscope_AO_FILTER_CUTOFF_FREQ(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	pNI5122_ch->cutoffFrequency = pParam->setValue;

#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_set_filterCutoffFreq(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Cutoff Freq: %f\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->cutoffFrequency);
	} else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_FILTER_CUTOFF_FREQ;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIscope_AO_FILTER_CENTER_FREQ(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	pNI5122_ch->centerFrequency = pParam->setValue;

#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_set_filterCenterFreq(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Center Freq: %f\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->centerFrequency);
	} else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_FILTER_CENTER_FREQ;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIscope_AO_FILTER_WIDTH(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	pNI5122_ch->bandpassWidth = pParam->setValue;

#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_set_filterBandWidth(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Bandpath Width: %f\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->bandpassWidth );
	} else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_FILTER_BND_WIDTH;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIscope_AO_MAX_FREQ(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;


	switch((int)pParam->setValue ) {
		case 0:
			pNI5122_ch->f64maxInputFrequency = 0.0;
			break;
		case 1:
			pNI5122_ch->f64maxInputFrequency = 20000000.0; 
			break;
		case 2:
			pNI5122_ch->f64maxInputFrequency = 40000000.0; 
			break;
		case 3:
			pNI5122_ch->f64maxInputFrequency = 100000000.0; 
			break;
		default:
			epicsPrintf(">>> %s/%s: max freq error(%d)! choose 0-3 \n", 
			pSTDdev->taskName, pSTDch->chName, (int)pParam->setValue );
			pSTDdev->ErrorDev = NISCOPE_ERROR_MAX_FREQ;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;
	}

#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_v_Cfg_ChanCharacteristics(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Max frequency: %dHz\n", 
			pSTDdev->taskName, pSTDch->chName, (int)pNI5122_ch->f64maxInputFrequency );
	} else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_MAX_FREQ;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);

}



LOCAL void devNIscope_AO_IMPEDANCE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

	if( pParam->setValue == 0 ) {
		pNI5122_ch->f64InputImpedance = INIT_SCOPE_IMPEDANCE_50;
	} else if ( pParam->setValue == 1 ) {
		pNI5122_ch->f64InputImpedance = INIT_SCOPE_IMPEDANCE_1M;
	} else {
		epicsPrintf(">>> %s/%s: impedance error(%d)! choose 0 or 1 \n", 
			pSTDdev->taskName, pSTDch->chName, (int)pParam->setValue );
		pSTDdev->ErrorDev = NISCOPE_ERROR_IMPEDANCE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}

#if CFG_NI_SCOPE_IMMEDIATELY	
	if( drvNIscope_v_Cfg_ChanCharacteristics(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Input impedance: %f\n", 
			pSTDdev->taskName, pSTDch->chName, pNI5122_ch->f64InputImpedance );
	} else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_IMPEDANCE;
#endif

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIscope_BO_READ_DATA(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
/*	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser; */
		
	if( drvNIscope_get_readData(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Read the data (Initiate the acquisition, and fetch the data)\n", 
			pSTDdev->taskName, pSTDch->chName );
	} 
	else pSTDdev->ErrorDev = NISCOPE_ERROR_READ_DATA;
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devNIscope_BO_FETCH_BINARY(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
/*	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser; */
		
	if( drvNIscope_get_fetchBinary(pSTDdev, pSTDch) == WR_OK) {
		epicsPrintf(">>> %s/%s: Fetch Binary data.\n", 
			pSTDdev->taskName, pSTDch->chName );
	}
	else pSTDdev->ErrorDev = NISCOPE_ERROR_FETCH_BINARY;
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devNIscope_BO_FETCH_DATA(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;


	if( pNI5122->myStatusDev & SCOPE_BEGIN_ACQUSITION ) {
		epicsPrintf(">>> %s: Acquisition not completed!\n", pSTDdev->taskName);
		pSTDdev->ErrorDev = NISCOPE_ERROR_FETCH_DATA;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}
	if( drvNIscope_get_AcquisitionStatus(pSTDdev) !=  NISCOPE_VAL_ACQ_COMPLETE ) {
		epicsPrintf(">>> %s: ACQ Status is not Complete!\n", pSTDdev->taskName);
		pSTDdev->ErrorDev = NISCOPE_ERROR_FETCH_DATA;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}
	pNI5122_ch->myStatusCh |= SCOPE_START_FETCH;
	if( drvNIscope_get_fetchData(pSTDdev, pSTDch) == WR_OK) {
		pNI5122_ch->myStatusCh |= SCOPE_FETCH_SUCCESS;
		epicsPrintf(">>> %s/%s: Fetch the data.\n", 
			pSTDdev->taskName, pSTDch->chName );
	} 
	else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_FETCH_DATA;

	pNI5122_ch->myStatusCh &= ~SCOPE_START_FETCH;

	scanIoRequest(pSTDdev->ioScanPvt_status);
	
}

LOCAL void devNIscope_BO_GET_ACTUAL_INFO(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = NULL;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	ST_NISCOPE_ch *pNI5122_ch = NULL;
		
	pSTDdev->StatusDev &= ~TASK_STANDBY;
	if( pNI5122->useMultiTrigger ) {
		if( pNI5122->n32numRecords > SIZE_CNT_MULTI_TRIG ) {
			pSTDdev->ErrorDev = NISCOPE_ERROR_REC_LENGTH;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			notify_error(1,"Multi-Trigger limited by 8 Rec. num.");
			return ;
		}
	}
	else {
		if( pNI5122->n32numRecords != 1 ) {
			pSTDdev->ErrorDev = NISCOPE_ERROR_REC_LENGTH;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			notify_error(1,"Single-Trigger support only 1 Rec. num.");
			return ;
		}
	}
	if( drvNIscope_get_RecordLength(pSTDdev) == WR_OK) {
		epicsPrintf(">>> %s: actual record length: %d\n", pSTDdev->taskName, (int)pNI5122->n32ActualRecordLength ); 
	} else {
		pSTDdev->ErrorDev = NISCOPE_ERROR_REC_LENGTH;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return ;
	}
	if( drvNIscope_get_SampleRate(pSTDdev) == WR_OK) {
		epicsPrintf(">>> %s: actual sample rate: %f\n", pSTDdev->taskName, pNI5122->f64ActualSampleRate ); 
	}
	else {
		pSTDdev->ErrorDev = NISCOPE_ERROR_SMP_RATE;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		return;
	}
	pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
	while(pSTDch) {
		pNI5122_ch = (ST_NISCOPE_ch*)pSTDch->pUser;
		if( drvNIscope_get_NumOfWfm(pSTDdev, pSTDch) == WR_OK) {
			epicsPrintf(">>> %s/%s: Current number of waveforms: %lu\n", pSTDdev->taskName, pSTDch->chName, pNI5122_ch->n32ActualNumWaveform ); 
		} else {
			pSTDdev->ErrorDev = NISCOPE_ERROR_GET_NUM_WFM;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;
		}
		
		if (pNI5122_ch->wfmInfoPtr) {
			printf("call free ( %s wfmInfoPtr) \n", pSTDch->chName);
			free (pNI5122_ch->wfmInfoPtr);
		}
		pNI5122_ch->wfmInfoPtr = malloc(sizeof(struct niScope_wfmInfo) * pNI5122_ch->n32ActualNumWaveform );
		if (pNI5122_ch->wfmInfoPtr == NULL) {
			epicsPrintf("\n>>> %s/%s: NISCOPE_ERROR_INSUFFICIENT_MEMORY (wfmInfoPtr)\n", pSTDdev->taskName, pSTDch->chName );
			pSTDdev->ErrorDev = NISCOPE_ERROR_MALLOC;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;
		}
		/*******************************************/
		if (pNI5122_ch->f64WaveformPtr) {
			printf("call free (%s f64WaveformPtr) \n", pSTDch->chName);
			free( pNI5122_ch->f64WaveformPtr);
		}
		pNI5122_ch->f64WaveformPtr = (ViReal64*) malloc (sizeof (ViReal64) * pNI5122->n32ActualRecordLength * pNI5122_ch->n32ActualNumWaveform );
		if (pNI5122_ch->f64WaveformPtr == NULL) {
			epicsPrintf("\n>>> %s/%s: NISCOPE_ERROR_INSUFFICIENT_MEMORY (f64WaveformPtr)\n", pSTDdev->taskName, pSTDch->chName );
			pSTDdev->ErrorDev = NISCOPE_ERROR_MALLOC;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			free(pNI5122_ch->wfmInfoPtr);
			return;
		}
		/*******************************************/
#if USE_BINARY_WFM
		if (pNI5122_ch->binaryWfmPtr) {
			printf("call free (pNI5122_ch->binaryWfmPtr) \n");
			free( pNI5122_ch->binaryWfmPtr);
		}
		pNI5122_ch->binaryWfmPtr = (void *) malloc (sizeof (ViInt16) * pNI5122->n32ActualRecordLength * pNI5122_ch->n32ActualNumWaveform );
		if (pNI5122_ch->binaryWfmPtr == NULL) {
			epicsPrintf("\n>>> %s/%s: NISCOPE_ERROR_INSUFFICIENT_MEMORY (binaryWfmPtr)\n", pSTDdev->taskName, pSTDch->chName );
			pSTDdev->ErrorDev = NISCOPE_ERROR_MALLOC;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;
		}
#endif
		/*******************************************/
		
		epicsThreadSleep(0.2);
		
		pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
	}
#if 1
	pNI5122->myStatusDev |= SCOPE_GET_ACTUAL_CONFIRM;
#endif

	pSTDdev->StatusDev |= TASK_STANDBY;
	pSTDdev->ErrorDev = NISCOPE_OK_CONFIRM;
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
	
}
LOCAL void devNIscope_BO_SET_TRIG_IMMEDIATE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	if( drvNIscope_set_trigImmediate(pSTDdev) == WR_OK) {
		epicsPrintf(">>> %s: Trigger Immediate\n", pSTDdev->taskName );
	} 
	else 
		pSTDdev->ErrorDev = NISCOPE_ERROR_TRIGER;
	
	scanIoRequest(pSTDdev->ioScanPvt_status);
}
LOCAL void devNIscope_BO_INIT_ACQUISITION(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
/*	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin; */

/*	pAdminCfg->StatusAdmin &= ~SCOPE_STORAGE_FINISH; */
	pNI5122->myStatusDev &= ~SCOPE_FINISH_ACQUISITION;

	if( !(pNI5122->myStatusDev & SCOPE_GET_ACTUAL_CONFIRM) )
	{
		pSTDdev->ErrorDev = NISCOPE_ERROR_GEN;
		scanIoRequest(pSTDdev->ioScanPvt_status);
		epicsPrintf("ERROR %s: must be get Actual information of ADC!\n", pSTDdev->taskName );
		return;
	}

	if( pNI5122->myStatusDev & SCOPE_BEGIN_ACQUSITION ) return;

	
	if( drvNIscope_start_acquisition(pSTDdev) == WR_OK) {
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;

		pNI5122->myStatusDev |= SCOPE_BEGIN_ACQUSITION;
		pNI5122->myStatusDev &= ~SCOPE_STORAGE_FINISH;
			
		flush_STDdev_to_MDSfetch(pSTDdev);

/*			if( pReflectoCfg->opMode != OPMODE_REMOTE ) */
/*			dbProc_call_SYS_RUN(pSTDdev, 1);*/ /* 2009-11-18 */


		epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId );
//		epicsEventSignal( pSTDdev->epicsEvent_DAQthread );



		epicsPrintf(">>> %s: Start acquition.\n", pSTDdev->taskName );
		epicsPrintf(">>> %s: Please wait acquition complete.\n", pSTDdev->taskName );
	} 
	else {
		pSTDdev->ErrorDev = NISCOPE_ERROR_GEN;
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);
	notify_refresh_master_status();
	
}
LOCAL void devNIscope_BO_ABORT_ACQUITION(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	pNI5122->myStatusDev |= SCOPE_ABORT_ACQUSITION;
	if( drvNIscope_abort_acquition(pSTDdev) == WR_OK) {		
		epicsPrintf(">>> %s: Abort acquition.\n", pSTDdev->taskName );
	}
	else pSTDdev->ErrorDev = NISCOPE_ERROR_ABORT;

	scanIoRequest(pSTDdev->ioScanPvt_status);
}


LOCAL void devNIscope_BO_AUTO_SETUP(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	if( drvNIscope_auto_setup(pSTDdev) == WR_ERROR) {
		epicsPrintf("\n>>> devNIscope_CONTROL_AUTO_SETUP() : drvNIscope_auto_setup()... failed\n");
		pSTDdev->ErrorDev = NISCOPE_ERROR_AUTO_SETUP;
	} else 
		epicsPrintf("OK!\n");

	scanIoRequest(pSTDdev->ioScanPvt_status);
}

LOCAL void devNIscope_BO_MULTI_TRIGGER(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;
	struct dbCommon *precord = pParam->precord;
	ST_MASTER *pAdminCfg = get_master();
	char strBuff[32];

	if( (pSTDdev->StatusDev & TASK_ARM_ENABLED) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}
	sprintf(strBuff, "%s_%s:RECORD_NUMBER", pAdminCfg->taskName, pSTDdev->taskName);

	if( (int)pParam->setValue == 0 ) {
		pNI5122->useMultiTrigger = 0;
		printf("%s: use single trigger input!\n", pSTDdev->taskName );
		
//		db_put( strBuff, "1");
	} 
	else if( (int)pParam->setValue == 1 ) {
		pNI5122->useMultiTrigger = 1;
		printf("%s: use multi trigger input!\n", pSTDdev->taskName );
		
//		db_put( strBuff, "3");
	}
	else {
		printf("ERROR, %s: check your input!(%s) - got :%d\n", pSTDdev->taskName, precord->name,  (int)pParam->setValue );
	}

}


LOCAL void devNIscope_test_put(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_STD_channel *pSTDch = pParam->pSTDch;
	struct dbCommon *precord = pParam->precord;

/*	ptaskConfig->fT0 = (float)pParam->setValue; */

	epicsPrintf("control thread (test PUT): %s/%s %s (%s), val: %f\n", pSTDdev->taskName, pSTDch->chName, 
		precord->name, epicsThreadGetNameSelf(), (float)pParam->setValue);
}


LOCAL long devAiNIscope_init_record(aiRecord *precord)
{
	ST_dpvt *pdevNIscopeCtrldpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
		
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevNIscopeCtrldpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevNIscopeCtrldpvt->devName,
				   pdevNIscopeCtrldpvt->arg0,
				   pdevNIscopeCtrldpvt->arg1);
#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("ST_dpvt arg num: %d: %s %s %s\n",i, pdevNIscopeCtrldpvt->devName, 
								pdevNIscopeCtrldpvt->arg0,
								pdevNIscopeCtrldpvt->arg1);
#endif
			pdevNIscopeCtrldpvt->pSTDdev = get_STDev_from_name(pdevNIscopeCtrldpvt->devName);
			if(!pdevNIscopeCtrldpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "ST_dpvt (init_record) Illegal INP field: task name");
				free(pdevNIscopeCtrldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "ST_dpvt (init_record) Illegal INP field");
			free(pdevNIscopeCtrldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if( i == 2 ){
		if(!strcmp(pdevNIscopeCtrldpvt->arg0, AI_SCOPE_STATUS_STR))
			pdevNIscopeCtrldpvt->ind = AI_SCOPE_STATUS;
		else if( !strcmp(pdevNIscopeCtrldpvt->arg0, AI_SCOPE_ERROR_STATUS_STR))
			pdevNIscopeCtrldpvt->ind = AI_SCOPE_ERROR_STATUS;
	
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevNIscopeCtrldpvt;

	return 2;	 /* don't convert */

}
LOCAL long devAiNIscope_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt	*pdevNIscopeCtrldpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevNIscopeCtrldpvt) {
		ioScanPvt = NULL;
		return 0;
	}	
	pSTDdev = pdevNIscopeCtrldpvt->pSTDdev;
	if( pdevNIscopeCtrldpvt->ind == AI_SCOPE_STATUS ||
		pdevNIscopeCtrldpvt->ind == AI_SCOPE_ERROR_STATUS )
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else 
		*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}
LOCAL long devAiNIscope_read_ai(aiRecord *precord)
{
	ST_dpvt *pdevNIscopeCtrldpvt = (ST_dpvt*) precord->dpvt;
		
	ST_STD_device *pSTDdev;
	ST_NISCOPE_dev *pNI5122;


	if(!pdevNIscopeCtrldpvt) return 0;

	pSTDdev 	= pdevNIscopeCtrldpvt->pSTDdev;
	pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;

	switch(pdevNIscopeCtrldpvt->ind) {
		case AI_SCOPE_STATUS:
			precord->val = pNI5122->myStatusDev;
/*			epicsPrintf("%s taskStatus: 0x%X = %d\n", pSTDdev->taskName, (int)precord->val, (int)precord->val); */
			break;
		case AI_SCOPE_ERROR_STATUS:
			precord->val = pSTDdev->ErrorDev;
/*			epicsPrintf("%s errorStatus: 0x%X = %d\n", pSTDdev->taskName, (int)precord->val, (int)precord->val); */
			break;
		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevNIscopeCtrldpvt->ind); 
			break;
	}
	return (2);  /* don't convert */ 


}




LOCAL long devAoNIscopeControl_init_record(aoRecord *precord)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pST_dpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s", pST_dpvt->devName,
			           pST_dpvt->arg0,
			           pST_dpvt->arg1);

#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devAoNIscopeControl arg num: %d: %s %s %s\n",i, pST_dpvt->devName, 
								pST_dpvt->arg0, pST_dpvt->arg1);
#endif
			pST_dpvt->pSTDdev = get_STDev_from_name(pST_dpvt->devName);
			if(!pST_dpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoNIscopeControl (init_record) Illegal INP field: task_name");
				free(pST_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			if( i> 2)  {
				unsigned short chid;
				chid = strtoul(pST_dpvt->arg0, NULL, 0);
				pST_dpvt->pSTDch = get_STCh_from_devName_chID(pST_dpvt->devName, chid);
				if(!pST_dpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAoNIscopeControld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n",
						    pST_dpvt->recordName,
						    pST_dpvt->devName,
						    pST_dpvt->arg0,
						    pST_dpvt->arg1);
					free(pST_dpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			}
		

			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoNIscopeControl (init_record) Illegal OUT field");
			free(pST_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if( i==2){
		 if (!strcmp(pST_dpvt->arg0, AO_SAMPLE_RATE_STR)) {
			pST_dpvt->ind = AO_SAMPLE_RATE;
		} else if (!strcmp(pST_dpvt->arg0, AO_REC_LENGTH_STR)) {
			pST_dpvt->ind = AO_REC_LENGTH;
		} else if (!strcmp(pST_dpvt->arg0, AO_TRIG_MODE_STR)) {
			pST_dpvt->ind = AO_TRIG_MODE;
		} else if (!strcmp(pST_dpvt->arg0, AO_REC_NUM_STR)) {
			pST_dpvt->ind = AO_REC_NUM;
		} 
	} else if( i==3 ) {
		if (!strcmp(pST_dpvt->arg1, AO_TEST_PUT_STR)) {
			pST_dpvt->ind = AO_TEST_PUT;
		} else if (!strcmp(pST_dpvt->arg1, AO_INPUT_RANGE_STR)) {
			pST_dpvt->ind = AO_INPUT_RANGE;
		} else if (!strcmp(pST_dpvt->arg1, AO_OFFSET_STR)) {
			pST_dpvt->ind = AO_OFFSET;
		} else if (!strcmp(pST_dpvt->arg1, AO_PROBE_ATTENU_STR)) {
			pST_dpvt->ind = AO_PROBE_ATTENU;
		} else if (!strcmp(pST_dpvt->arg1, AO_IMPEDANCE_STR)) {
			pST_dpvt->ind = AO_IMPEDANCE;
		}
		else if (!strcmp(pST_dpvt->arg1, AO_FILTER_TYPE_STR)) {
			pST_dpvt->ind = AO_FILTER_TYPE;
		} else if (!strcmp(pST_dpvt->arg1, AO_FILTER_CUTOFF_FREQ_STR)) {
			pST_dpvt->ind = AO_FILTER_CUTOFF_FREQ;
		} else if (!strcmp(pST_dpvt->arg1, AO_FILTER_CENTER_FREQ_STR)) {
			pST_dpvt->ind = AO_FILTER_CENTER_FREQ;
		} else if (!strcmp(pST_dpvt->arg1, AO_FILTER_WIDTH_STR)) {
			pST_dpvt->ind = AO_FILTER_WIDTH;
		}
		else if(!strcmp(pST_dpvt->arg1, AO_MAX_FREQ_STR)) {
			pST_dpvt->ind = AO_MAX_FREQ;
		} 


		
		
		
	}
	


	precord->udf = FALSE;
	precord->dpvt = (void*) pST_dpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoNIscopeControl_write_ao(aoRecord *precord)
{
#if SHOW_PHASE_NISCOPE
	static int   kkh_cnt;
#endif
	ST_dpvt			*pST_dpvt = (ST_dpvt*) precord->dpvt;
	
	ST_STD_device	*pSTDdev;
	ST_STD_channel	*pSTDch;
	ST_threadCfg  *pcontrolThreadConfig;
	ST_threadQueueData			qData;


	if(!pST_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pST_dpvt->pSTDdev;
	pSTDch		= pST_dpvt->pSTDch;
	pcontrolThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.pSTDch		= pSTDch;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pST_dpvt->pSTDch->chanIndex; */
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

#if SHOW_PHASE_NISCOPE
		epicsPrintf("db processing: phase I %s (%s):%d\n", precord->name,
				                                epicsThreadGetNameSelf(), pST_dpvt->ind);
#endif

		switch(pST_dpvt->ind) {
			case AO_SAMPLE_RATE:
				qData.pFunc = devNIscope_AO_SAMPLE_RATE; 
				break;
			case AO_REC_LENGTH:
				qData.pFunc = devNIscope_AO_REC_LENGTH; 
				break;
			case AO_TRIG_MODE:
				qData.pFunc = devNIscope_AO_TRIG_MODE;
				break;
			case AO_REC_NUM:
				qData.pFunc = devNIscope_AO_REC_NUM;
				break;
						
				
			case AO_TEST_PUT:
				qData.pFunc = devNIscope_test_put; break;
			case AO_INPUT_RANGE: 
				qData.pFunc = devNIscope_AO_INPUT_RANGE; break;
			case AO_OFFSET: 
				qData.pFunc = devNIscope_AO_OFFSET; break;
			case AO_PROBE_ATTENU: 
				qData.pFunc = devNIscope_AO_PROBE_ATTENU; break;
			case AO_IMPEDANCE:
				qData.pFunc = devNIscope_AO_IMPEDANCE; break;
			case AO_FILTER_TYPE:
				qData.pFunc = devNIscope_AO_FILTER_TYPE; break;
			case AO_FILTER_CUTOFF_FREQ:
				qData.pFunc = devNIscope_AO_FILTER_CUTOFF_FREQ; break;
			case AO_FILTER_CENTER_FREQ:
				qData.pFunc = devNIscope_AO_FILTER_CENTER_FREQ; break;
			case AO_FILTER_WIDTH:
				qData.pFunc = devNIscope_AO_FILTER_WIDTH; break;
			case AO_MAX_FREQ:
				qData.pFunc = devNIscope_AO_MAX_FREQ; break;
				

		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if SHOW_PHASE_NISCOPE
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
/*
		switch(pST_dpvt->ind) {
			case BO_ABORT_ACQUITION: precord->val = 0; break;
		}
*/
		return 2;    /* don't convert */
	}

	return 0;
}

LOCAL long devStringoutNIscopeControl_init_record(stringoutRecord *precord)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pST_dpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pST_dpvt->devName,
			           pST_dpvt->arg0,
			           pST_dpvt->arg1);

#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devStringoutNIscopeControl arg num: %d: %s %s %s\n",i, pST_dpvt->devName, 
								pST_dpvt->arg0, pST_dpvt->arg1);
#endif
			pST_dpvt->pSTDdev = get_STDev_from_name(pST_dpvt->devName);
			if(!pST_dpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutNIscopeControl (init_record) Illegal INP field: task_name");
				free(pST_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			
			if( i> 2)  {
				unsigned short chid;
				chid = strtoul(pST_dpvt->arg0, NULL, 0);
				pST_dpvt->pSTDch = get_STCh_from_devName_chID(pST_dpvt->devName,
												chid);
				if(!pST_dpvt->pSTDch) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAoNIscopeControld (init_record) Illegal INP filed: channel name");
					epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n",
						    pST_dpvt->recordName,
						    pST_dpvt->devName,
						    pST_dpvt->arg0,
						    pST_dpvt->arg1);
					free(pST_dpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
	
				}
			}

				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutNIscopeControl (init_record) Illegal OUT field");
			free(pST_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if( i==3 ) {
		if(!strcmp(pST_dpvt->arg1, STRINGOUT_WF_FILENAME_STR)) {
			pST_dpvt->ind = STRINGOUT_WF_FILENAME;
		} else if (!strcmp(pST_dpvt->arg1, STRINGOUT_TAG_NAME_STR)) {
			pST_dpvt->ind = STRINGOUT_TAG_NAME;
		}
	} else if ( i==2) {
		if(!strcmp(pST_dpvt->arg0, STRINGOUT_TRIG_SRC_STR)) {
			pST_dpvt->ind = STRINGOUT_TRIG_SRC;
		} 

	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pST_dpvt;
	
	return 2;     /* don't convert */
}


LOCAL long devStringoutNIscopeControl_write_stringout(stringoutRecord *precord)
{
	ST_dpvt			*pST_dpvt = (ST_dpvt*) precord->dpvt;

	ST_STD_device	*pSTDdev;
	ST_STD_channel	*pSTDch;
	ST_threadCfg  *pcontrolThreadConfig;
	ST_threadQueueData			qData;

	
	if(!pST_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}
	
	pSTDdev			= pST_dpvt->pSTDdev;
	pSTDch		= pST_dpvt->pSTDch;
	pcontrolThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.pSTDch		= pSTDch;
	qData.param.precord		= (struct dbCommon *)precord;

	strcpy( qData.param.setStr, precord->val ); 

	/* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NISCOPE
		epicsPrintf("db processing: phase I %s (%s):%d\n", precord->name,
				                                epicsThreadGetNameSelf(), pST_dpvt->ind);
#endif
		switch(pST_dpvt->ind) {
			case STRINGOUT_WF_FILENAME:
				qData.pFunc = devNIscope_STRINGOUT_WF_FILENAME;
				break;
			case STRINGOUT_TAG_NAME:
				qData.pFunc = devNIscope_STRINGOUT_TAG_NAME;
				break;
			case STRINGOUT_TRIG_SRC:
				qData.pFunc = devNIscope_STRINGOUT_TRIG_SRC;
				break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId, (void*) &qData, sizeof(ST_threadQueueData));
		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if SHOW_PHASE_NISCOPE
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
        	precord->udf = FALSE;
		return 2;    /* don't convert */
	}
	return 0;
}


LOCAL long devBoNIscope_init_record(boRecord *precord)
{
    ST_dpvt *pdevNIscopedpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
    int i;

    switch(precord->out.type) {
        case INST_IO:
        	strcpy(pdevNIscopedpvt->recordName, precord->name);
            i = sscanf(precord->out.value.instio.string, "%s %s %s", pdevNIscopedpvt->devName,
                                                                 pdevNIscopedpvt->arg0,
                                                                 pdevNIscopedpvt->arg1);
#if PRINT_DEV_SUPPORT_ARG
			epicsPrintf("devBoNIscope arg num: %d: %s %s %s\n",i, pdevNIscopedpvt->devName, 
								pdevNIscopedpvt->arg0, pdevNIscopedpvt->arg1);
#endif
            pdevNIscopedpvt->pSTDdev = get_STDev_from_name(pdevNIscopedpvt->devName);
            if(!pdevNIscopedpvt->devName) { /* port not found */
                recGblRecordError(S_db_badField, (void*)precord,
                                  "devBoNIscope (init_record) Illegal OUT field: port not found");
                free(pdevNIscopedpvt);
                precord->udf = TRUE;
                precord->dpvt = NULL;
                return S_db_badField;
            }

            if( i> 2)  {
			unsigned short chid;
			chid = strtoul(pdevNIscopedpvt->arg0, NULL, 0);
			pdevNIscopedpvt->pSTDch = get_STCh_from_devName_chID(pdevNIscopedpvt->devName,
											chid);
			if(!pdevNIscopedpvt->pSTDch) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBoNIscopeControld (init_record) Illegal INP filed: channel name");
				epicsPrintf("record: %s, task: %s, chan: %s, arg1: %s\n",
					    pdevNIscopedpvt->recordName,
					    pdevNIscopedpvt->devName,
					    pdevNIscopedpvt->arg0,
					    pdevNIscopedpvt->arg1);
				free(pdevNIscopedpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;

			}
		} /* end of switch for pdevNIscopedpvt->param_first */
			
			
			
            break;  /* end of inst_io */
        default:
            recGblRecordError(S_db_badField, (void*) precord,
                              "devBoNIscope (init_record) Illegal OUT field");
            free(pdevNIscopedpvt);
            precord->udf = TRUE;
            precord->dpvt = NULL;
            return S_db_badField;
    } /* end of switch statement for precord->out.type */
    
    
    if( i==2){
		if (!strcmp(pdevNIscopedpvt->arg0, BO_AUTO_SETUP_STR)) {
			pdevNIscopedpvt->ind = BO_AUTO_SETUP;
		} else if (!strcmp(pdevNIscopedpvt->arg0, BO_GET_ACTUAL_INFO_STR)) {
			pdevNIscopedpvt->ind = BO_GET_ACTUAL_INFO;
		} else if (!strcmp(pdevNIscopedpvt->arg0, BO_SET_TRIG_IMMEDIATE_STR)) {
			pdevNIscopedpvt->ind = BO_SET_TRIG_IMMEDIATE;
		} else if (!strcmp(pdevNIscopedpvt->arg0, BO_INIT_ACQUITION_STR)) {
			pdevNIscopedpvt->ind = BO_INIT_ACQUITION;
		} else if (!strcmp(pdevNIscopedpvt->arg0, BO_ABORT_ACQUITION_STR)) {
			pdevNIscopedpvt->ind = BO_ABORT_ACQUITION;
		} else if (!strcmp(pdevNIscopedpvt->arg0, BO_TREE_PUT_STR)) {
			pdevNIscopedpvt->ind = BO_TREE_PUT;
		} else if (!strcmp(pdevNIscopedpvt->arg0, BO_MULTI_TRIGGER_STR)) {
			pdevNIscopedpvt->ind = BO_MULTI_TRIGGER;
		} 
		 
	} else if( i==3 ) {
		if (!strcmp(pdevNIscopedpvt->arg1, BO_READ_DATA_STR)) {
			pdevNIscopedpvt->ind = BO_READ_DATA;
		} else if (!strcmp(pdevNIscopedpvt->arg1, BO_FETCH_DATA_STR)) {
			pdevNIscopedpvt->ind = BO_FETCH_DATA;
		} else if (!strcmp(pdevNIscopedpvt->arg1, BO_FETCH_BINARY_STR)) {
			pdevNIscopedpvt->ind = BO_FETCH_BINARY;
		}
	}

    precord->udf = FALSE;
    precord->dpvt = (void*)pdevNIscopedpvt;
    return 0;
}


LOCAL long devBoNIscope_write_bo(boRecord *precord)
{
	ST_dpvt			*pST_dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device	*pSTDdev;
	ST_STD_channel	*pSTDch;
	ST_threadCfg  *pcontrolThreadConfig;
	ST_threadQueueData			qData;
	

    if(!precord->dpvt || precord->udf == TRUE) {
        precord->pact = TRUE;
        return 0;
    }

	pSTDdev			= pST_dpvt->pSTDdev;
	pSTDch		= pST_dpvt->pSTDch;
	pcontrolThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.pSTDch		= pSTDch;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pST_dpvt->pSTDch->chanIndex; */
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
#if SHOW_PHASE_NISCOPE
		epicsPrintf("db processing: phase I %s (%s):%d\n", precord->name,
				                                epicsThreadGetNameSelf(), pST_dpvt->ind );
#endif
		switch(pST_dpvt->ind) {
			/* not related with channel.... */
			case BO_AUTO_SETUP:
				qData.pFunc = devNIscope_BO_AUTO_SETUP;
				break;
			case BO_GET_ACTUAL_INFO:
				qData.pFunc = devNIscope_BO_GET_ACTUAL_INFO;
				break;
			case BO_SET_TRIG_IMMEDIATE:
				qData.pFunc = devNIscope_BO_SET_TRIG_IMMEDIATE;
				break;
			case BO_INIT_ACQUITION:
				qData.pFunc = devNIscope_BO_INIT_ACQUISITION;
				break;
			case BO_ABORT_ACQUITION:
				qData.pFunc = devNIscope_BO_ABORT_ACQUITION;
				break;	
			case BO_TREE_PUT:
				qData.pFunc = devNIscope_BO_TREE_PUT;
				break;
			case BO_MULTI_TRIGGER:
				qData.pFunc = devNIscope_BO_MULTI_TRIGGER;
				break;

				
						
			/* related with channel.... */
			case BO_READ_DATA: qData.pFunc = devNIscope_BO_READ_DATA; break;
			case BO_FETCH_DATA:	qData.pFunc = devNIscope_BO_FETCH_DATA; break;
			case BO_FETCH_BINARY: qData.pFunc = devNIscope_BO_FETCH_BINARY; break;

		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
#if SHOW_PHASE_NISCOPE
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
/*
		switch(pST_dpvt->ind) {
			case BO_INIT_ACQUITION: precord->val = 0; break;
			case BO_ABORT_ACQUITION: precord->val = 0; break;
			default: break;
		}
*/
/*		precord->val = 0; */

		return 2;    /* don't convert */
	}

	return 0;
}




