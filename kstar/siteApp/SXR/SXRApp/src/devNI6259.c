
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

#include "sfwAdminHead.h"
#include "sfwDBSeq.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drvNI6259.h" 
#include "myMDSplus.h"
#include "niPvString.h"


static long devAiNI6259RawRead_init_record(aiRecord *precord);
static long devAiNI6259RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiNI6259RawRead_read_ai(aiRecord *precord);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiNI6259RawRead_ai = {
	6,
	NULL,
	NULL,
	devAiNI6259RawRead_init_record,
	devAiNI6259RawRead_get_ioint_info,
	devAiNI6259RawRead_read_ai,
	NULL
};
epicsExportAddress(dset, devAiNI6259RawRead_ai);



static long devAiNI6259RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pdevAiNI6259RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pdevAiNI6259RawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pdevAiNI6259RawReaddpvt->pSTDdev;
/*	if( pdevAiNI6259RawReaddpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else */
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiNI6259RawRead_init_record(aiRecord *precord)
{
	ST_dpvt *pdevAiNI6259RawReaddpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiNI6259RawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s",
				   pdevAiNI6259RawReaddpvt->arg0);
#if 0
	epicsPrintf("devAiNI6259RawRead arg num: %d: %s %s %s\n",i, pdevAiNI6259RawReaddpvt->devName, 
								pdevAiNI6259RawReaddpvt->arg0,
								pdevAiNI6259RawReaddpvt->arg1);
#endif

				ST_STD_device *pSTDdev = NULL;
				ST_NI6259_dev *pNI6259 = NULL;
				ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
				pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
				
				while(pSTDdev) {


					if(!strcmp(pSTDdev->devType, "NI6259"))
						{
								pdevAiNI6259RawReaddpvt->pSTDdev = pSTDdev;
						}
					pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
				}

			if(!pdevAiNI6259RawReaddpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiNI6259RawRead (init_record) Illegal INP field: task name");
				free(pdevAiNI6259RawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			if( i > 2)
			{
				pdevAiNI6259RawReaddpvt->pchannelConfig = drvNI6259_find_channelConfig(pdevAiNI6259RawReaddpvt->devName,
											         pdevAiNI6259RawReaddpvt->arg0);
				if(!pdevAiNI6259RawReaddpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiNI6259RawRead (init_record) Illegal INP field: channel name");
					free(pdevAiNI6259RawReaddpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiNI6259RawRead (init_record) Illegal INP field");
			free(pdevAiNI6259RawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


		if(!strcmp(pdevAiNI6259RawReaddpvt->arg0, AI_SENSOR_BLOCK1_TEMP_STR)) {
			pdevAiNI6259RawReaddpvt->ind = AI_SENSOR_BLOCK1_TEMP;
			}
		else if(!strcmp(pdevAiNI6259RawReaddpvt->arg0, AI_SENSOR_BLOCK2_TEMP_STR)) {
			pdevAiNI6259RawReaddpvt->ind = AI_SENSOR_BLOCK2_TEMP;
			}
		else if(!strcmp(pdevAiNI6259RawReaddpvt->arg0, AI_SENSOR_BLOCK3_TEMP_STR)) {
			pdevAiNI6259RawReaddpvt->ind = AI_SENSOR_BLOCK3_TEMP;
			}
		else if(!strcmp(pdevAiNI6259RawReaddpvt->arg0, AI_SENSOR_BLOCK4_TEMP_STR)) {
			pdevAiNI6259RawReaddpvt->ind = AI_SENSOR_BLOCK4_TEMP;
			}


	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiNI6259RawReaddpvt;

	return 2;    /* don't convert */ 
}

static long devAiNI6259RawRead_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pdevAiNI6259RawReaddpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	ST_NI6259_dev *pNI6259 = NULL;

	if(!pdevAiNI6259RawReaddpvt) return 0;

	pSTDdev		= pdevAiNI6259RawReaddpvt->pSTDdev;

	pNI6259 = (ST_NI6259_dev*)pSTDdev->pUser;
	

	switch(pdevAiNI6259RawReaddpvt->ind) {

		case AI_SENSOR_BLOCK1_TEMP:
			precord->val = pNI6259->temp[0];
			break;
		case AI_SENSOR_BLOCK2_TEMP:
			precord->val = pNI6259->temp[1];
			break;
		case AI_SENSOR_BLOCK3_TEMP:
			precord->val = pNI6259->temp[2];
			break;
		case AI_SENSOR_BLOCK4_TEMP:
			precord->val = pNI6259->temp[3];
			break;

		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pdevAiNI6259RawReaddpvt->ind); 
			break;
	}
	return (2);
}


