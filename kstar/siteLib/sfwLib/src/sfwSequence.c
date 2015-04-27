/****************************************************************************

Module      : sfwDBseq.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2009. 06. 23
2010. 6. 8  major modification.   by woong.
2010. 7. 26  add true db process 

2013. 5. 24
add PV "CCS_PCS_FAULT_IP_MINIMUM"  by woong, request from SWYun


*****************************************************************************/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "errlog.h" /* errMessage() */
#include <cadef.h> 

#include "sfwDriver.h"
#include "sfwGlobalDef.h"
#include "sfwCommon.h"




#if 1
#define str_CCS_SHOTSEQ_START    "CCS_SHOTSEQ_START"  /* located CCS_J , 201 */
#define str_CCS_CTU_shotStart   "TSS_CTU_shotStart"  /*located TSS, 210*/
#define str_CCS_SHOTSEQ_MDSTREE_CREATE   "CCS_SHOTSEQ_MDSTREE_CREATE"  /*located CCS_J, 201*/
#define str_CCS_PCS_FAULT_IP_MINIMUM   "CCS_PCS_FAULT_IP_MINIMUM"  /*located CCS_T, 2013. 5.22 request from swyun*/
#define str_CCS_RT_PLASMA_STATUS   "CCS_RT_PLASMA_STATUS"  /*located CCS_T, 2013. 8.28 */


#else
#define str_CCS_SHOTSEQ_START    "ATCA_SHOTSEQ_START"
#define str_CCS_CTU_shotStart   "ATCA_CTU_shotStart"
#endif
#define ID_SHOTSEQ_START   0
#define ID_CCS_CTU_shotStart   1
#define ID_SHOTSEQ_MDSTREE_CREATE   2
#define ID_PCS_FAULT_IP_MINIMUM 3
#define ID_RT_PLASMA_STATUS 4



#define SIZE_CNT_CA_ACCESS      5



int make_KSTAR_CA_seq();



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
	ST_MASTER *pMaster = get_master();
	ST_pv* _pv = args.usr;
	void* val_ptr_type;
	epicsUInt16 u16Val;
	char strBuf[24];

	_pv->status = args.status;
	if (args.status != ECA_NORMAL)
	{
/*		epicsPrintf("[%s - %s] CA event handler argument isn't NORMAL\n", __DATE__, __TIME__); */
		kfwPrint(0, "CA event handler argument isn't NORMAL");
		return;
	}
	_pv->dbrType = args.type;
	memcpy(_pv->value, args.dbr, dbr_size_n(args.type, args.count));

	val_ptr_type = dbr_value_ptr(_pv->value, _pv->dbrType); 
	if ( strcmp( _pv->name, str_CCS_SHOTSEQ_START ) ==0 ) 
	{
		if( _pv->firstEvent == 1 ) {
/*			printf("[%s - %s] \"%s\"  - This is first event!ignore it.\n", __DATE__, __TIME__, _pv->name ); */
			kfwPrint(0, "\"%s\", First event! Ignore it.", _pv->name);
			_pv->firstEvent = 0;
			return;
		}
		u16Val = ((epicsUInt16 *) val_ptr_type)[0]; /* for MDSTREE_CREATE, SHOTSEQ_START, CTU_Start, BI */
		pMaster->flag_sequence_on = u16Val;
/*		epicsPrintf("ShotSeq from CCS: (%d)\n", u16Val); */
		switch( u16Val ) {
		case 0:
			call_PRE_SEQSTOP((double)u16Val);
			kfwPrint(0, "CCS shot sequence finished!");
			if( (pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
				(pMaster->StatusAdmin & TASK_ARM_ENABLED ) &&
				!(pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) &&
				!(pMaster->StatusAdmin & TASK_IN_PROGRESS)  )
			{
				db_put(pMaster->strPvNames[NUM_SYS_ARMING], "0");
			}
			call_POST_SEQSTOP((double)u16Val);			
			break;
		case 1:
			call_PRE_SEQSTART((double)u16Val);
/*			epicsPrintf("[%s - %s] CCS shot sequence started!\n", __DATE__, __TIME__);*/
			kfwPrint(0, "CCS shot sequence started!");
			if( (pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
				(pMaster->StatusAdmin & TASK_STANDBY) &&
				(pMaster->StatusAdmin & TASK_SYSTEM_IDLE)    )
			{
				db_get(pMaster->strPvNames[NUM_CA_SHOT_NUMBER], strBuf);
				sscanf(strBuf, "%lu", &pMaster->ST_Base.shotNumber);
				flush_ShotNum_to_All_STDdev();

				if( pMaster->cUseAutoCreateTree)
					db_put(pMaster->strPvNames[NUM_SYS_CREATE_LOCAL_TREE], strBuf);
				
				db_get(pMaster->strPvNames[NUM_CA_BLIP_TIME], strBuf);
				sscanf(strBuf, "%f", &pMaster->ST_Base.fBlipTime);
				flush_Blip_to_All_STDdev();


				db_put(pMaster->strPvNames[NUM_SYS_ARMING], "1");
			}
			else 
/*				epicsPrintf("[%s - %s]  Not remote mode or Ready!\n", __DATE__, __TIME__);*/
				kfwPrint(0, "Not remote mode or Ready!");

			epicsThreadSleep(1.0);
			notify_refresh_master_status();
			break;
		default: 
/*			epicsPrintf("[%s - %s] SeqStart Not valid value (%d)\n", __DATE__, __TIME__, u16Val); */
			kfwPrint(0, "SeqStart Not valid value (%d)", u16Val);
			break;
		}
	}
	else if ( strcmp( _pv->name, str_CCS_CTU_shotStart ) ==0 ) 
	{
		if( _pv->firstEvent == 1 ) {
/*			printf("[%s - %s] \"%s\"  - This is first event!ignore it.\n", __DATE__, __TIME__, _pv->name ); */
			kfwPrint(0, "\"%s\", First event! Ignore it.", _pv->name);
			_pv->firstEvent = 0;
			return;
		}
		epicsTimeGetCurrent(&pMaster->time_shot_start);
		pMaster->flag_shot_start = 1;
		call_PRE_SHOTSTART(((epicsUInt16*) val_ptr_type)[0]);
/*		epicsPrintf("[%s - %s] CTU trigged!(%d)\n",  __DATE__, __TIME__, ((epicsUInt16 *) val_ptr_type)[0]);  */
		kfwPrint(0, "CTU trigged!(%d)", ((epicsUInt16 *) val_ptr_type)[0] );
		if( (pMaster->ST_Base.opMode == OPMODE_REMOTE) &&
			(pMaster->StatusAdmin & TASK_ARM_ENABLED) ) 
		{
			db_put(pMaster->strPvNames[NUM_SYS_RUN], "1");
		}
		else 
/*			epicsPrintf("[%s - %s] Not remote mode or Arming!\n", __DATE__, __TIME__); */
			kfwPrint(0, "Not remote mode or Arming!");

		epicsThreadSleep(1.0);
		notify_refresh_master_status();
	}
	else if ( strcmp( _pv->name, str_CCS_SHOTSEQ_MDSTREE_CREATE) ==0 ) 
	{
		if( _pv->firstEvent == 1 ) {
			kfwPrint(0, "\"%s\", First event! Ignore it.", _pv->name);
			_pv->firstEvent = 0;
			return;
		}
		u16Val = ((epicsUInt16 *) val_ptr_type)[0]; /* for MDSTREE_CREATE, SHOTSEQ_START, CTU_Start, BI */
		reset_event_flag(); /* 2013. 8. 29 */
		if ( u16Val )  
		{
			if( pMaster->ST_Base.opMode == OPMODE_REMOTE  ) 
				call_TREE_CREATE((double)u16Val);
			else 
				kfwPrint(0, "Received Tree create but not remote mode!");
		}

	}
	else if ( strcmp( _pv->name, str_CCS_PCS_FAULT_IP_MINIMUM) ==0 ) 
	{
		u16Val = ((epicsUInt16 *) val_ptr_type)[0]; /* for MDSTREE_CREATE, SHOTSEQ_START, CTU_Start, BI */
		if( _pv->firstEvent == 1 ) {
			kfwPrint(0, "\"%s\", First event! Ignore it.", _pv->name);
			_pv->firstEvent = 0;
			return;
		}
		pMaster->cStatus_ip_min_fault = (char)u16Val;
		
		if( u16Val ) epicsTimeGetCurrent(&pMaster->time_pcs_ipmin);
		
		if( pMaster->ST_Base.opMode == OPMODE_REMOTE  ) 
			call_FAULT_IP_MINIMUM((double)u16Val);
		else 
			kfwPrint(0, "Received IP_minimum_fault(%d) event but not remote mode!", u16Val);
	}
	else if ( strcmp( _pv->name, str_CCS_RT_PLASMA_STATUS) ==0 ) 
	{
		u16Val = ((epicsUInt16 *) val_ptr_type)[0];
		if( _pv->firstEvent == 1 ) {
			kfwPrint(0, "\"%s\", First event! Ignore it.", _pv->name);
			_pv->firstEvent = 0;
			epicsTimeGetCurrent(&pMaster->time_plasma_on);
			epicsTimeGetCurrent(&pMaster->time_plasma_off);
			return;
		}
		if( u16Val ) epicsTimeGetCurrent(&pMaster->time_plasma_on);
		else epicsTimeGetCurrent(&pMaster->time_plasma_off);
		pMaster->flag_plasma_status = u16Val;
		
		if( pMaster->ST_Base.opMode == OPMODE_REMOTE  ) 
			call_PLASMA_STATUS((double)u16Val);
		else 
			kfwPrint(0, "Received plasma status(%d) event but not remote mode!", u16Val);
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
		kfwPrint(0, " \"%s\" ca disconnection!\n", ppv->name );
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
	strcpy(pST_pvs[ID_SHOTSEQ_MDSTREE_CREATE].name, str_CCS_SHOTSEQ_MDSTREE_CREATE);
	strcpy(pST_pvs[ID_PCS_FAULT_IP_MINIMUM].name, str_CCS_PCS_FAULT_IP_MINIMUM);
	strcpy(pST_pvs[ID_RT_PLASMA_STATUS].name, str_CCS_RT_PLASMA_STATUS);

	for( nval=0; nval < SIZE_CNT_CA_ACCESS; nval++) {
		pST_pvs[nval].onceConnected = 0;
	}
#if 0
	returncode = ca_context_create(ca_enable_preemptive_callback);
	if (returncode != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying " 
			"to start channel access.\n", ca_message(returncode) );
		return ;
	}
#endif
	returncode = create_pvs(pST_pvs, SIZE_CNT_CA_ACCESS, connection_handler);
	if ( returncode ) {
		kfwPrint(0, "create_pvs() failed!\n");
		return ;
	}

	ca_pend_event(1.0);
	for (nval = 0; nval < SIZE_CNT_CA_ACCESS; nval++)
	{
		if (!pST_pvs[nval].onceConnected)
			kfwPrint(0, " \"%s\" not yet connected!\n", pST_pvs[nval].name);
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





