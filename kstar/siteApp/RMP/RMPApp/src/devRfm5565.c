
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

#include "pv_list.h" 
#include "drvRfm5565.h" 


/*******************************************************/

static long devAoRfm5565_init_record(aoRecord *precord);
static long devAoRfm5565_write_ao(aoRecord *precord);


static long devStringinRfm5565_init_record(stringinRecord *precord);
static long devStringinRfm5565_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt);
static long devStringinRfm5565_read_stringin(stringinRecord *precord);


BINARYDSET devAoRfm5565 = { 6,  NULL,  NULL,   
	devAoRfm5565_init_record,  
	NULL,  
	devAoRfm5565_write_ao,  
	NULL };

BINARYDSET devStringinRfm5565 = { 6,  NULL,  NULL,   
	devStringinRfm5565_init_record,  
	devStringinRfm5565_get_ioint_info,  
	devStringinRfm5565_read_stringin,  
	NULL };

epicsExportAddress(dset, devAoRfm5565);
epicsExportAddress(dset, devStringinRfm5565);


/*******************************************************/


static long devStringinRfm5565_init_record(stringinRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %d", pDpvt->devName, pDpvt->arg0, &pDpvt->n32Arg0);
#if 0
	epicsPrintf("devStringinACQ196Control arg num: %d: %s %s\n",i, devStringinRfm5565->devName, devStringinRfm5565->arg0);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devStringinRfm5565 (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devStringinRfm5565 (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(pDpvt->arg0, STRINGIN_READ_VALUE_STR)) {
		pDpvt->ind = STRINGIN_READ_VALUE;
	} 
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devStringinRfm5565_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 0;    /*returns: (-1,0)=>(failure,success)*/
}

static long devStringinRfm5565_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	if(!pDpvt) {
		ioScanPvt = NULL;
		return -1;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}


static long devStringinRfm5565_read_stringin(stringinRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	ST_dpvt		*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_RFM5565* pRFM5565;
	RFM2G_STATUS   result;
	RFM2G_UINT32   buffer;
//	RFM2G_UINT32   buffer[1]; /* Data shared with another node     */

	if(!pDpvt) return -1;

	pSTDdev			= pDpvt->pSTDdev;
	pRFM5565 = (ST_RFM5565 *)pSTDdev->pUser;
       
	switch(pDpvt->ind) {

		case STRINGIN_READ_VALUE:
			 /* Now read data from the other board from OFFSET_1 */
		        result = RFM2gRead( pRFM5565->Handle, pRFM5565->Offset, &buffer, sizeof(RFM2G_UINT32)*1 );
//			 result = RFM2gRead( pRFM5565->Handle, 0x1000, (void *)buffer, BUFFER_SIZE*4 );
		        if( result != RFM2G_SUCCESS )
		        {
		            printf( "\nERROR: Could not read data from Reflective Memory.\n" );
		            RFM2gClose( &pRFM5565->Handle );
		            return(-1); /*returns: (-1,0)=>(failure,success)*/
		        }
//			epicsPrintf("0x%X -> 0x%X\n", pRFM5565->Offset, buffer);
			sprintf(precord->val, "0x%x: 0x%x, swap(%x)", pRFM5565->Offset, buffer, HSWAP32(buffer));
			break;
			
		default: 
			break;
	}

	return (0); /*returns: (-1,0)=>(failure,success)*/
}


static long devAoRfm5565_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;

	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s", pDpvt->devName, pDpvt->arg0 );
#if 0
			printf("devAo arg num: %d: %s %s\n", nval,  pDpvt->arg0, pDpvt->arg1);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoRfm5565 (init_record) Illegal OUT field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAo (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if (!strcmp(pDpvt->arg0, AO_SET_READ_OFFSET_STR)) {
		pDpvt->ind = AO_SET_READ_OFFSET;
	} else if (!strcmp(pDpvt->arg0, AO_RFM_WRITE_VALUE_STR)) {
		pDpvt->ind = AO_RFM_WRITE_VALUE;
	}

	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR_Rfm5565! devAoRfm5565_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
		
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoRfm5565_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_RFM5565* pRFM5565;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;
	RFM2G_STATUS   result;
	RFM2G_UINT32   buffer;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pRFM5565 = (ST_RFM5565 *)pSTDdev->pUser;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if 0
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_SET_READ_OFFSET:
				pRFM5565->Offset = (RFM2G_UINT32)precord->val;
				precord->pact = FALSE;
				precord->udf = FALSE;
				return 0; /*(0)=>(success ) */

			case AO_RFM_WRITE_VALUE:
				buffer = (RFM2G_UINT32)precord->val;
				result = RFM2gWrite( pRFM5565->Handle, pRFM5565->Offset, &buffer, sizeof(RFM2G_UINT32)*1 );
			        if( result != RFM2G_SUCCESS )
			        {
			            printf( "ERROR: Could not write data to Reflective Memory.\n" );
			            RFM2gClose( &pRFM5565->Handle );
			            return(-1);
			        }
				precord->pact = FALSE;
				precord->udf = FALSE;
				return 0; /*(0)=>(success ) */

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
#if 0
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
//			case AO_SET_READ_OFFSET: precord->val = pRFM5565->Offset ; break;

			default: break;
		}

		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0; /*(0)=>(success ) */
	}

	return -1;
}


