/****************************************************************************

Module      : sfwDBseq.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2009. 06. 23
2010. 6. 8  major modification.   by woong.
2010. 7. 26  add true db process 

*****************************************************************************/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#include "dbDefs.h"
#include "dbBase.h"
#include "dbBkpt.h"
#include "dbAccess.h"
#include "dbNotify.h"
#include "dbScan.h"
#include "dbStaticLib.h"
#include "db_access_routines.h"
#include "dbAccessDefs.h"
#include "errlog.h" /* errMessage() */


#include <cadef.h> 


#include "sfwAdminHead.h"
#include "sfwDBSeq.h"
#include "sfwGlobalDef.h"



#ifndef MIN
#   define MIN(x,y)  (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#   define MAX(x,y)  (((x) < (y)) ? (x) : (y))
#endif

/* Structure representing one PV (= channel) */
typedef struct 
{
	char name[SIZE_PV_NAME];
	chid  chid;
	long  dbfType;
	long  dbrType;
	unsigned long nElems;
	unsigned long reqElems;
	int status;
	void* value;
	char onceConnected;
	char firstEvent;
	
} ST_pv;

static unsigned long eventMask = DBE_VALUE | DBE_ALARM;   /* Event mask used */
ST_pv* pST_pvs;				  /* Array of PV structures */


static void event_handler (evargs args)
{
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	ST_pv* _pv = args.usr;
	void* val_ptr_type;
	unsigned  int u32Val;
	char strBuf[24];

	_pv->status = args.status;
	if (args.status != ECA_NORMAL)
	{
		epicsPrintf("ERROR! CA event handler argument isn't NORMAL\n");
		return;
	}
	_pv->dbrType = args.type;
	memcpy(_pv->value, args.dbr, dbr_size_n(args.type, args.count));

	val_ptr_type = dbr_value_ptr(_pv->value, _pv->dbrType); 
	if ( strcmp( _pv->name, str_CCS_SHOTSEQ_START ) ==0 ) 
	{
		if( _pv->firstEvent == 1 ) {
			printf("\"%s\"  - This is first event!ignore it.\n", _pv->name);
			_pv->firstEvent = 0;
			return;
		}
		u32Val = ((unsigned int*) val_ptr_type)[0]; /* for SHOTSEQ_START, SHOT_NUMBER, CTU_Start */
		epicsPrintf("ShotSeq from CCS: (%d)\n", u32Val);
		switch( u32Val ) {
		case 0:
			epicsPrintf("CCS shot sequence finished!\n");
			if( (pAdminCfg->ST_Base.opMode == OPMODE_REMOTE) &&
				(pAdminCfg->StatusAdmin & TASK_ARM_ENABLED ) &&
				!(pAdminCfg->StatusAdmin & TASK_DAQ_STARTED)    )
			{
				DBproc_put("SXR_SYS_ARMING", "0");
			}
			break;
		case 1:
			epicsPrintf("CCS shot sequence started!\n");
			if( (pAdminCfg->ST_Base.opMode == OPMODE_REMOTE) &&
				(pAdminCfg->StatusAdmin & TASK_SYSTEM_READY) &&
				(pAdminCfg->StatusAdmin & TASK_SYSTEM_IDLE)    )
			{
				DBproc_get("SXR_CCS_SHOT_NUMBER", strBuf);
				sscanf(strBuf, "%lu", &pAdminCfg->ST_Base.shotNumber);
				flush_ShotNum_to_All_STDdev();
				
				DBproc_get("SXR_CCS_BLIP_TIME", strBuf);
				sscanf(strBuf, "%f", &pAdminCfg->ST_Base.fBlipTime);
				flush_Blip_to_All_STDdev();
				/* get dT0 from RBA_dT0 */
				DBproc_get("SXR_LTU_T0_SEC", strBuf);
				sscanf(strBuf, "%f", &pAdminCfg->ST_Base.fBlipTime);
				flush_dT0_to_All_STDdev();

				DBproc_put("SXR_SYS_ARMING", "1");
			}
			else 
				epicsPrintf("ERROR!  Not remote mode or Ready!\n");

			notify_refresh_admin_status();
			break;
		default: 
			epicsPrintf("ERROR!  SeqStart Not valid value (%d)\n", u32Val);
			break;
		}
	}
	else if ( strcmp( _pv->name, str_CCS_CTU_shotStart ) ==0 ) 
	{
		if( _pv->firstEvent == 1 ) {
			printf("\"%s\"  - This is first event!ignore it.\n", _pv->name);
			_pv->firstEvent = 0;
			return;
		}
		epicsPrintf("CTU trigged!(%d)\n", ((unsigned int*) val_ptr_type)[0]); 
		if( (pAdminCfg->ST_Base.opMode == OPMODE_REMOTE) &&
			(pAdminCfg->StatusAdmin & TASK_ARM_ENABLED) ) 
		{
			DBproc_put("SXR_SYS_RUN", "1");
		}
		else 
			epicsPrintf("ERROR!  Not remote mode or Arming!\n");

		notify_refresh_admin_status();
	}

}


static void connection_handler ( struct connection_handler_args args )
{
	ST_pv *ppv = ( ST_pv * ) ca_puser ( args.chid );
	if ( args.op == CA_OP_CONN_UP ) 
	{
		/* Get natural type and array count */
		ppv->nElems  = ca_element_count(ppv->chid);
		ppv->dbfType = ca_field_type(ppv->chid);
		/* Set up value structures */
		ppv->dbrType = dbf_type_to_DBR_TIME(ppv->dbfType); /* Use native type */
		ppv->onceConnected = 1;
		ppv->firstEvent = 1;
		/* Issue CA request */
		/* ---------------- */
		/* install monitor once with first connect */
		if ( ! ppv->value ) 
		{
			/* Allocate value structure */
			ppv->value = calloc(1, dbr_size_n(ppv->dbrType, ppv->nElems));
			if ( ppv->value ) 
			{
				ppv->status = ca_create_subscription(ppv->dbrType,
						ppv->nElems,
						ppv->chid,
						eventMask,
						event_handler,
						(void*)ppv,
						NULL);
				if ( ppv->status != ECA_NORMAL ) {
					free ( ppv->value );
				}
			}
		}
	}
	
	else if ( args.op == CA_OP_CONN_DOWN ) 
	{
		ppv->status = ECA_DISCONN;		
		ppv->onceConnected = 0;
		ppv->firstEvent = 1;
		printf("ERROR!   \"%s\" ca disconnection!\n", ppv->name );
	}
}

static int create_pvs (ST_pv* pvs, int nPvs, caCh *pCB)
{
	int n;
	int result;
	int returncode = 0;

	for (n = 0; n < nPvs; n++) 
	{
	        result = ca_create_channel (pvs[n].name,
	                                    pCB,
	                                    &pvs[n],
	                                    0,  /* #define DEFAULT_CA_PRIORITY 0  Default CA priority  */
	                                    &pvs[n].chid);
		if (result != ECA_NORMAL) {
			fprintf(stderr, "CA error %s occurred while trying "
				"to create channel '%s'.\n", ca_message(result), pvs[n].name);
			pvs[n].status = result;
			returncode = 1;
		}
	}
	
	return returncode;
}


static void caSeqThread(void *pvt)
{
	int returncode = 0;
	int nval = 0;

	pST_pvs= calloc (SIZE_CNT_CA_ACCESS, sizeof(ST_pv));
	if (!pST_pvs)
	{
		fprintf(stderr, "Memory allocation for channel structures failed.\n");
		return ;
	}

	strcpy(pST_pvs[ID_SHOTSEQ_START].name, str_CCS_SHOTSEQ_START);
	strcpy(pST_pvs[ID_CCS_CTU_shotStart].name, str_CCS_CTU_shotStart);

	for( nval=0; nval < SIZE_CNT_CA_ACCESS; nval++) {
		pST_pvs[nval].onceConnected = 0;
	}

	returncode = ca_context_create(ca_enable_preemptive_callback);
	if (returncode != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying " 
			"to start channel access.\n", ca_message(returncode) );
		return ;
	}

	returncode = create_pvs(pST_pvs, SIZE_CNT_CA_ACCESS, connection_handler);
	if ( returncode ) {
		printf("ERROR!  create_pvs() failed!\n");
		return ;
	}

	ca_pend_event(1.0);
	for (nval = 0; nval < SIZE_CNT_CA_ACCESS; nval++)
	{
		if (!pST_pvs[nval].onceConnected)
			printf("ERROR!  \"%s\" not yet connected!\n", pST_pvs[nval].name);
	}
	ca_pend_event(0);
	epicsThreadSleep(0.3);
	ca_context_destroy();
	free( pST_pvs );
}

int make_KSTAR_CA_seq()
{
	epicsThreadCreate("KSTAR_ShotSeq", epicsThreadPriorityHigh,
	   epicsThreadGetStackSize(epicsThreadStackMedium), caSeqThread, 0);
	
	return WR_OK;
}

int DBproc_put(const char *pname, const char *pvalue)
{
	long  status;
	DBADDR addr;

	 if (!pname || !*pname || !pvalue) {
		printf("Usage: DBproc_put \"pv name\", \"value\"\n");
		return WR_ERROR;
	}

	status=dbNameToAddr(pname, &addr);
	if (status) {
		printf("Record '%s' not found\n", pname);
		return WR_ERROR;
	}
	/* For enumerated types must allow for ENUM rather than string*/
	/* If entire field is digits then use DBR_ENUM else DBR_STRING*/
	if (addr.dbr_field_type == DBR_ENUM && !*pvalue &&
	strspn(pvalue,"0123456789") == strlen(pvalue)) {
		unsigned short value;

		sscanf(pvalue, "%hu", &value);
		status = dbPutField(&addr, DBR_ENUM, &value, 1L);
	} else if (addr.dbr_field_type == DBR_CHAR &&
		addr.no_elements > 1) {
		status = dbPutField(&addr, DBR_CHAR, pvalue, strlen(pvalue) + 1);
	} else {
		status = dbPutField(&addr, DBR_STRING, pvalue, 1L);
	}
	if (status) {
		errMessage(status,"- dbPutField error\n");
		return WR_ERROR;
	}
	return WR_OK;
}

int DBproc_get(const char *pname, char *pvalue)
{
	long  status;
	DBADDR addr;
	DBADDR *paddr;
	
	dbFldDes  	*pdbFldDes;
	dbRecordType    *pdbRecordType;
	
	char	*pfield_value;
	DBENTRY	dbentry;
	DBENTRY	*pdbentry = &dbentry;
	

	 if (!pname || !*pname || !pvalue) {
		printf("Usage: DBproc_get \"pv name\", \"value\"\n");
		return WR_ERROR;
	}

	status=dbNameToAddr(pname, &addr);
	if (status) {
		printf("Record '%s' not found\n", pname);
		return WR_ERROR;
	}
	paddr = &addr;
	pdbFldDes = paddr->pfldDes;
	pdbRecordType = pdbFldDes->pdbRecordType;
	

	dbInitEntry(pdbbase,pdbentry);
	status = dbFindRecord(pdbentry,pname);
	if(status) {
		errMessage(status,pname);
		return WR_ERROR;
	}
	
	status = dbFindField(pdbentry,"VAL:");
	pfield_value = (char *)dbGetString(pdbentry);
	sprintf(pvalue, "%s",  (pfield_value ? pfield_value : "null"));

	printf("\"%s\":VAL, %s\n", pname, pvalue );
	
	return WR_OK;
}


int CAproc_get(const char *pname, char *pvalue)
{
	int result;                 /* CA result */
	ST_pv* pPvs;				  /* Array of PV structures */
	void *val_ptr;
/*
	result = ca_context_create(ca_disable_preemptive_callback);
	if (result != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying "
		"to start channel access '%s'.\n", ca_message(result), pname);
		return 1;
	}
*/
	pPvs = calloc (1, sizeof(ST_pv));
	if (!pPvs)
	{
		fprintf(stderr, "Memory allocation for channel structures failed.\n");
		return WR_ERROR;
	}
	strcpy(pPvs->name, pname);

	result = create_pvs(pPvs, 1, connection_handler);
	if ( result ) {
		printf("ERROR!  create_pvs() failed!\n");
		return WR_ERROR;
	}
	ca_pend_event(0.1);
	if (!pPvs->onceConnected) {
		printf("ERROR!  \"%s\" not yet connected!\n", pPvs->name);
		return WR_ERROR;
	}

	pPvs->nElems  = ca_element_count(pPvs->chid);
	pPvs->dbfType = ca_field_type(pPvs->chid);
/*	pPvs->dbrType = dbf_type_to_DBR_TIME(pPvs->dbfType); */
	pPvs->dbrType = dbf_type_to_DBR(pPvs->dbfType);
	pPvs->reqElems = 1;
	

	pPvs->value = calloc(1, dbr_size_n(pPvs->dbrType, pPvs->reqElems));
	if(!pPvs->value){ 
		printf(" ca calloc error!\n");
		return WR_ERROR;
	}
	result = ca_array_get(pPvs->dbrType,
						pPvs->reqElems,
						pPvs->chid,
						pPvs->value);
	
	if( result != ECA_NORMAL){
		free(pPvs->value);
		printf("ca_array get error!\n");
		return WR_ERROR;
	}
	
	result = ca_pend_io( 1.0);
	if ( result == ECA_TIMEOUT ) {
		free(pPvs->value);
		printf(" pend io err, time out. \n");
		return WR_ERROR;
	}
	
	val_ptr = dbr_value_ptr(pPvs->value, pPvs->dbrType);

	switch( pPvs->dbrType )
	{
		case DBF_STRING:
			sprintf(pvalue, "%s", ((dbr_string_t*) val_ptr)[0] );
			printf("%s, DBF_STRING(%d), %s\n", pPvs->name, DBF_STRING, pvalue );
			break;
		case DBR_INT: /*case DBF_INT: */
			sprintf(pvalue, "%d", ((dbr_int_t*) val_ptr)[0] );
			printf("%s, DBF_SHORT(%d), %s\n", pPvs->name, DBF_SHORT, pvalue );
			break;
		case DBF_FLOAT:
			sprintf(pvalue, "%f", ((dbr_float_t*) val_ptr)[0]  );
			printf("%s, DBF_FLOAT(%d), %s\n", pPvs->name, DBF_FLOAT, pvalue );
			break;
		case DBF_ENUM:
			printf("%s, DBF_ENUM(%d), not defined\n", pPvs->name, DBF_ENUM );
			break;
		case DBF_CHAR:
			sprintf(pvalue, "%c", ((dbr_char_t*) val_ptr)[0] );
			printf("%s, DBF_CHAR(%d), %s\n", pPvs->name, DBF_CHAR, pvalue );
			break;
		case DBF_LONG:
			sprintf(pvalue, "%d", ((dbr_long_t*) val_ptr)[0]  );
			printf("%s, DBF_LONG(%d), %s\n", pPvs->name, DBF_LONG, pvalue );
			break;
		case DBF_DOUBLE:
			sprintf(pvalue, "%lf", ((dbr_double_t*) val_ptr)[0] );
			printf("%s, DBF_DOUBLE(%d), %s\n", pPvs->name, DBF_DOUBLE, pvalue );
			break;
		default:
			free(pPvs->value);
			printf("%s, Bad dbrType (%lu)\n", pPvs->name, pPvs->dbrType);
			return WR_ERROR;
	}
/*	printf("CA Get %s, %s\n", pPvs->name, pvalue ); */
	free(pPvs->value);
	free(pPvs); 
/*	ca_context_destroy(); */
	return WR_OK;
}

int CAproc_put(const char *pname, const char *pvalue)
{
	int result;                 /* CA result */
	ST_pv* pPvs;				  /* Array of PV structures */

	pPvs = calloc (1, sizeof(ST_pv));
	if (!pPvs)
	{
		fprintf(stderr, "Memory allocation for channel structures failed.\n");
		return WR_ERROR;
	}
	strcpy(pPvs->name, pname);

	result = create_pvs(pPvs, 1, connection_handler);
	if ( result ) {
		printf("ERROR!  create_pvs() failed!\n");
		return WR_ERROR;
	}
	ca_pend_event(0.1);
	if (!pPvs->onceConnected) {
		printf("ERROR!  \"%s\" not yet connected!\n", pPvs->name);
		return WR_ERROR;
	}

	pPvs->dbfType = DBR_STRING;
	pPvs->value = calloc (strlen(pvalue), sizeof(char));
	if(!pPvs->value){ 
		printf(" ca calloc error!\n");
		return WR_ERROR;
	}
	printf("%s: put %s\n", pPvs->name, (char *)pPvs->value);
	
	result = ca_array_put (pPvs->dbrType, 1, pPvs->chid,  pPvs->value);
	if( result != ECA_NORMAL){
		free(pPvs->value);
		free(pPvs); 
		printf("ca_array_put error!\n");
		return WR_ERROR;
	}
	result = ca_pend_io( 1.0);
	if ( result == ECA_TIMEOUT ) {
		free(pPvs->value);
		free(pPvs); 
		printf(" pend io err, time out. \n");
		return WR_ERROR;
	}
	free(pPvs->value);
	free(pPvs); 
	return WR_OK;
}

int make_Admin_CA()
{
        epicsThreadCreate("my_chaccess", epicsThreadPriorityHigh,
           epicsThreadGetStackSize(epicsThreadStackMedium), caSeqThread, 0);
        
        return WR_OK;
}


/*************************************************
        user add here
**************************************************/
#if 0

/*
DBADDR db_SOFT_SYS_RUN;
DBADDR db_SOFT_DATA_PROCESS;
DBADDR db_SYS_FETCH_DATA;
*/

int dbProc_Init(void *ptr)
{
        long      status;
        char buf[64];

        ST_STD_device *pSTDdev = (ST_STD_device*)ptr;
        ST_STD_channel *pSTDch = NULL;
        
        ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser; 
        ST_NISCOPE_ch *pNI5122_ch = NULL;
        
        pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
        while(pSTDch) {

                pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

                sprintf(buf, "ER_%s_CH%s:FETCH_DATA", pSTDdev->taskName, pSTDch->chName);
                status=dbNameToAddr(buf, &pNI5122_ch->db_SYS_FETCH_DATA);
                if(status) {
                        errMessage(status,"dbNameToAddr error");
                        epicsPrintf("ERROR! %s/%s: \"%s\" can not founded!\n", pSTDdev->taskName, pSTDch->chName, buf);
                        return WR_ERROR;
                }
                pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
        }

        status=dbNameToAddr("ER_SOFT_SYS_RUN", &pNI5122->db_SOFT_SYS_RUN);
        if(status) {
                errMessage(status,"dbNameToAddr error: \"EREF_SOFT_SYS_RUN\" ");
                return WR_ERROR;
        }
        status=dbNameToAddr("ER_SOFT_DATA_PROCESS", &pNI5122->db_SOFT_DATA_PROCESS);
        if(status) {
                errMessage(status,"dbNameToAddr error: \"ERFF_SOFT_DATA_PROCESS\" ");
                return WR_ERROR;
        }

        return WR_OK;

}

int dbProc_call_Fetch_start(void *ptr)
{
        long  status;
        double dVal = 1;

        ST_STD_device *pSTDdev = (ST_STD_device*)ptr;
        ST_STD_channel *pSTDch = NULL;
        
/*      ST_NISCOPE_dev *pNI5122 = (ST_NISCOPE_dev *)pSTDdev->pUser;  */
        ST_NISCOPE_ch *pNI5122_ch = NULL;
        

        pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
        while(pSTDch) {
                pNI5122_ch = (ST_NISCOPE_ch *)pSTDch->pUser;

                status=dbPutField(&pNI5122_ch->db_SYS_FETCH_DATA, DBR_DOUBLE, &dVal, 1L);
                if(status) {
                        errMessage(status,"dbPutField error");
                        return WR_ERROR;
                }
        
                pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
        }


        return WR_OK;
}
#endif






