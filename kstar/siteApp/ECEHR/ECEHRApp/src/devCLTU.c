/******************************************************************************
 *  Copyright (c) 2007 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/


/*==============================================================================
                        EDIT HISTORY FOR MODULE


2007. 10. 16.
add "ioc_debug" routine


2007. 10. 22.
add "multi-trigger" routine


2007. 10. 25
change working flow for port config....


2007. 11. 5
add  " set calibration test "


2007. 11. 6
add printf for show offset value
add function " get LTU ID number  "
remove "sys/time.h"


2007. 11. 15
add function " devCLTU_AO_putCalVal() "


2007. 11. 20
changed in Func devCLTU_AO_mTrig()....
-	ptaskConfig->mPortConfig[pParam->nPort].nTrigMode = xxxxxx;
+	ptaskConfig->mPortConfig[0].nTrigMode = xxxxxx;

2007. 11. 26
+ drvCLTU_set_rtc() call function.


2007. 12. 20
modify for mix cltu m6802

2009. 11. 8
+ stringin board time

2009. 11. 10
source architecture changed...


*/


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

#include "boRecord.h"
#include "aiRecord.h"
#include "aoRecord.h"
#include "stringinRecord.h"

#include "epicsExport.h"


#include "drvCLTU.h"

#include "pvList.h"
#include "globalDef.h"


#define WITH_TIMINGBOARD	1


/*
typedef struct
{
    IOSCANPVT      ioscanpvt;
} timeSCANst;
*/

void devCLTU_AO_ShotTerm(execParam *pParam);
void devCLTU_setup(execParam *pParam);
void devCLTU_set_shotStart(execParam *pParam);
void devCLTU_set_clockOnShot(execParam *pParam);
void devCLTU_AO_calibration(execParam *pParam);
void devCLTU_AO_putCalVal(execParam *pParam);

void devCLTU_AO_Trig(execParam *pParam);
void devCLTU_AO_Clock(execParam *pParam);
void devCLTU_AO_T0(execParam *pParam);
void devCLTU_AO_T1(execParam *pParam);
void devCLTU_AO_mTrig(execParam *pParam);
void devCLTU_AO_mClock(execParam *pParam);
void devCLTU_AO_mT0(execParam *pParam);
void devCLTU_AO_mT1(execParam *pParam);

typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];
	char recordName[80];

	drvCLTU_taskConfig *ptaskConfig;

	unsigned	ind;
	int  nPort;

} devCLTUdpvt;

typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];
	char recordName[80];

	drvCLTU_taskConfig *ptaskConfig;

	unsigned	ind;
} devAiCLTURawReaddpvt;


LOCAL long devBoCLTUControl_init_record(boRecord *precord);
LOCAL long devBoCLTUControl_write_bo(boRecord *precord);

LOCAL long devAoCLTUControl_init_record(aoRecord *precord);
LOCAL long devAoCLTUControl_write_ao(aoRecord *precord);

LOCAL long devAiCLTURawRead_init_record(aiRecord *precord);
LOCAL long devAiCLTURawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiCLTURawRead_read_ai(aiRecord *precord);


LOCAL long devStringinCLTU_init_record(stringinRecord *precord);
LOCAL long devStringinCLTU_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devStringinCLTU_read_stringin(stringinRecord *precord);


struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoCLTUControl = {
    5,
    NULL,
    NULL,
    devBoCLTUControl_init_record,
    NULL,
    devBoCLTUControl_write_bo
};
epicsExportAddress(dset, devBoCLTUControl);


struct {
	long		number;
	DEVSUPFUN	dev_report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_stringin;
} devStringinCLTU = {
	6,
	NULL,
	NULL,
	devStringinCLTU_init_record,
	devStringinCLTU_get_ioint_info,
	devStringinCLTU_read_stringin
};
epicsExportAddress(dset, devStringinCLTU);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoCLTUControl = {
	6,
	NULL,
	NULL,
	devAoCLTUControl_init_record,
	NULL,
	devAoCLTUControl_write_ao,
	NULL
};
epicsExportAddress(dset, devAoCLTUControl);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiCLTURawRead = {
	6,
	NULL,
	NULL,
	devAiCLTURawRead_init_record,
	devAiCLTURawRead_get_ioint_info,
	devAiCLTURawRead_read_ai,
	NULL
};

epicsExportAddress(dset, devAiCLTURawRead);

ULONG start, stop, _interval;
unsigned int		sysCltuStatus=0;


void devCLTU_BO_SETUP(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_portConfig (ptaskConfig, pParam->nPort) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nPort );
#endif
	epicsPrintf("Setup port %d : %s (%s)\n", pParam->nPort, precord->name, epicsThreadGetNameSelf());
}

void devCLTU_BO_SHOW_INFO(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	cltu_pmc_info_show(ptaskConfig);
#endif
	epicsPrintf("Setup port %d : %s (%s)\n", pParam->nPort, precord->name, epicsThreadGetNameSelf());
}


void devCLTU_BO_SHOT_START(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotStart (ptaskConfig) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
#endif

	epicsPrintf("%s (%s): Local shot start!\n", precord->name, epicsThreadGetNameSelf());
}

void devCLTU_AO_putCalVal(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	if( drvCLTU_put_calibratedVal(ptaskConfig,  (int)pParam->setValue ) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_AO_putCalVal : drvCLTU_put_calibratedVal(%d) ... error!\n", (int)pParam->setValue );

	if( ioc_debug == 1)
		epicsPrintf("control thread (calibrated value : %f): %s (%s)\n", pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}

void devCLTU_AO_calibration(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	ptaskConfig->calib_target_ltu = (int)pParam->setValue;

	if( drvCLTU_set_calibration (ptaskConfig ) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_AO_calibration : drvCLTU_set_calibration(%d) ... error!\n", (int)pParam->setValue );

	if( ioc_debug == 1)
		epicsPrintf("control thread (calibarion for %f): %s (%s)\n", pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}
/*
void devCLTU_set_shotStart(execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotStart () == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_set_shotStart : drvCLTU_set_shotStart() ... error!\n" );
#endif

	if( ioc_debug == 1)
		epicsPrintf("control thread (shot start): %s (%s)\n", precord->name,
			                                            epicsThreadGetNameSelf());
}
*/
void devCLTU_AO_ShotTerm(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;

	ptaskConfig->nShotTerm_msb = (unsigned int) (time64>>32);
	ptaskConfig->nShotTerm_lsb = (unsigned int) time64;

	if( ioc_debug == 1) {	
		epicsPrintf(">>> shot term msb : 0x%x (%f sec)\n", ptaskConfig->nShotTerm_msb, pParam->setValue );
		epicsPrintf(">>> shot term lsb : 0x%x\n", ptaskConfig->nShotTerm_lsb );
	}
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotTerm (ptaskConfig) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_AO_ShotTerm : drvCLTU_set_shotTerm() ... error!\n" );
#endif
	if( ioc_debug == 1)
		epicsPrintf("control thread (shot period): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
}

/*  onoff: 1=on, 0=off */
void devCLTU_set_clockOnShot(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_clockOnShot( ptaskConfig, pParam->nPort,  pParam->setValue) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_set_clockOnShot : drvCLTU_set_clockOnShot(%d, %d) ... error!\n", pParam->nPort , (int)pParam->setValue);
#endif
	if( ioc_debug == 1)
		epicsPrintf("control thread (set clock on shot): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
}
/*
void devCLTU_setup(execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_portConfig (pParam->nPort) == WR_ERROR )
		epicsPrintf("\n>>> devCLTU_setup : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nPort );
#endif
	if( ioc_debug == 1)
		epicsPrintf("control thread (setup p%d): %s (%s)\n", pParam->nPort, precord->name, epicsThreadGetNameSelf());
}
*/
void devCLTU_AO_Trig(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	ptaskConfig->sPortConfig[pParam->nPort].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0;
	ptaskConfig->curPortConfigStatus[pParam->nPort] |= PORT_TRIG_OK;
	
	if( ioc_debug == 1) {
		epicsPrintf(">>> Trigger: port(%d), level(0x%x)\n", pParam->nPort, ptaskConfig->sPortConfig[pParam->nPort].nTrigMode );
		epicsPrintf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_Clock(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	
	ptaskConfig->sPortConfig[pParam->nPort].nClock = pParam->setValue;
	ptaskConfig->curPortConfigStatus[pParam->nPort] |= PORT_CLOCK_OK;
	
	if( ioc_debug == 1) {
		epicsPrintf(">>> Clock :p(%d) %d Hz\n",pParam->nPort, ptaskConfig->sPortConfig[pParam->nPort].nClock );
		epicsPrintf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_T0(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	
	ptaskConfig->sPortConfig[pParam->nPort].nT0_msb = (unsigned int) (time64>>32);
	ptaskConfig->sPortConfig[pParam->nPort].nT0_lsb = (unsigned int) time64;
	ptaskConfig->curPortConfigStatus[pParam->nPort] |= PORT_T0_OK;

	if( ioc_debug == 1) {
		epicsPrintf(">>> T0 msb : p(%d) 0x%x  (%f sec)\n", pParam->nPort, ptaskConfig->sPortConfig[pParam->nPort].nT0_msb , pParam->setValue);
		epicsPrintf(">>> T0 lsb : p(%d) 0x%x\n", pParam->nPort, ptaskConfig->sPortConfig[pParam->nPort].nT0_lsb );
  
		epicsPrintf("control thread (set t0): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_T1(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	
	ptaskConfig->sPortConfig[pParam->nPort].nT1_msb = (unsigned int) (time64>>32);
	ptaskConfig->sPortConfig[pParam->nPort].nT1_lsb = (unsigned int) time64;
	ptaskConfig->curPortConfigStatus[pParam->nPort] |= PORT_T1_OK;

	if( ioc_debug == 1) {
		epicsPrintf(">>> T1 msb : p(%d) 0x%x (%f sec)\n", pParam->nPort, ptaskConfig->sPortConfig[pParam->nPort].nT1_msb , pParam->setValue);
		epicsPrintf(">>> T1 lsb : p(%d) 0x%x\n", pParam->nPort, ptaskConfig->sPortConfig[pParam->nPort].nT1_lsb );

		epicsPrintf("control thread (set t1): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

/* In this function(multi-trig mode),  we assume "nPort" is "section number" */
void devCLTU_AO_mTrig(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
/*	ptaskConfig->mPortConfig[pParam->nPort].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0; */
	ptaskConfig->mPortConfig[0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0;
	ptaskConfig->curPortConfigStatus[4] |= PORT_TRIG_OK;
	if( ioc_debug == 1) {
		epicsPrintf(">>> mTrigger: level(0x%x)\n", ptaskConfig->mPortConfig[0].nTrigMode );
		epicsPrintf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mClock(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	ptaskConfig->mPortConfig[pParam->nPort].nClock = pParam->setValue;
	ptaskConfig->curPortConfigStatus[4] |= PORT_CLOCK_OK;
	if( ioc_debug == 1) {
		epicsPrintf(">>> mClock :section(%d) %d Hz\n", pParam->nPort, ptaskConfig->mPortConfig[pParam->nPort].nClock );
		epicsPrintf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mT0(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	ptaskConfig->mPortConfig[pParam->nPort].nT0_msb = (unsigned int) (time64>>32);
	ptaskConfig->mPortConfig[pParam->nPort].nT0_lsb = (unsigned int) time64;
	ptaskConfig->curPortConfigStatus[4] |= PORT_T0_OK;
	if( ioc_debug == 1) {
		epicsPrintf(">>> mT0 msb : section(%d) 0x%x (%f sec)\n", pParam->nPort, ptaskConfig->mPortConfig[pParam->nPort].nT0_msb, pParam->setValue);
		epicsPrintf(">>> mT0 lsb : section(%d) 0x%x\n", pParam->nPort, ptaskConfig->mPortConfig[pParam->nPort].nT0_lsb );
		epicsPrintf("control thread (set t0): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mT1(execParam *pParam)
{
	drvCLTU_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	ptaskConfig->mPortConfig[pParam->nPort].nT1_msb = (unsigned int) (time64>>32);
	ptaskConfig->mPortConfig[pParam->nPort].nT1_lsb = (unsigned int) time64;
	ptaskConfig->curPortConfigStatus[4] |= PORT_T1_OK;
	if( ioc_debug == 1) {
		epicsPrintf(">>> mT1 msb : section(%d) 0x%x (%f sec)\n", pParam->nPort, ptaskConfig->mPortConfig[pParam->nPort].nT1_msb, pParam->setValue);
		epicsPrintf(">>> mT1 lsb : section(%d) 0x%x\n", pParam->nPort, ptaskConfig->mPortConfig[pParam->nPort].nT1_lsb );
		epicsPrintf("control thread (set t1): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

LOCAL long devAoCLTUControl_init_record(aoRecord *precord)
{
	devCLTUdpvt *pdevCLTUdpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int i;
	
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevCLTUdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevCLTUdpvt->taskName,
				   pdevCLTUdpvt->arg1,
			           pdevCLTUdpvt->arg2);

			if( ioc_debug == 2)
				epicsPrintf("devAoCLTUControl arg num: %d: %s %s %s\n",i, pdevCLTUdpvt->taskName, 
								pdevCLTUdpvt->arg1,
								pdevCLTUdpvt->arg2);

/*			pdevCLTUdpvt->ptaskConfig = drvCLTU_get_taskConfig(); */
			pdevCLTUdpvt->ptaskConfig = drvCLTU_find_taskConfig(pdevCLTUdpvt->taskName);
			
			if(!pdevCLTUdpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoCLTUControl (init_record) Illegal INP field: task_name");
				free(pdevCLTUdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoCLTUControl (init_record) Illegal OUT field");
			free(pdevCLTUdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	pdevCLTUdpvt->nPort = -1;
	
	if( i == 2 ) {
/*		if(!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_START_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_START;
		} else */
		if (!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_TERM_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_TERM;
		} else if (!strcmp(pdevCLTUdpvt->arg1, AO_CALIBRATION_STR)) {
			pdevCLTUdpvt->ind = AO_CALIBRATION;
		} else if (!strcmp(pdevCLTUdpvt->arg1, AO_PUT_CALED_VAL_STR)) {
			pdevCLTUdpvt->ind = AO_PUT_CALED_VAL;
		}

	}
	else if (i ==3 ) 
	{
		
/*		if (!strcmp(pdevCLTUdpvt->arg1, AO_SETUP_STR)) {
			pdevCLTUdpvt->ind = AO_SETUP;
		} else */
		if(!strcmp(pdevCLTUdpvt->arg1, AO_CLOCK_ONSHOT_STR)) {
			pdevCLTUdpvt->ind = AO_CLOCK_ONSHOT;
		
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_TRIG_STR)) {
			pdevCLTUdpvt->ind = AO_TRIG;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_CLOCK_STR)) {
			pdevCLTUdpvt->ind = AO_CLOCK;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_T0_STR)) {
			pdevCLTUdpvt->ind = AO_T0;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_T1_STR)) {
			pdevCLTUdpvt->ind = AO_T1;

		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MTRIG_STR)) {
			pdevCLTUdpvt->ind = AO_MTRIG;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MCLOCK_STR)) {
			pdevCLTUdpvt->ind = AO_MCLOCK;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MT0_STR)) {
			pdevCLTUdpvt->ind = AO_MT0;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MT1_STR)) {
			pdevCLTUdpvt->ind = AO_MT1;
		} 

		pdevCLTUdpvt->nPort = atoi(pdevCLTUdpvt->arg2);
	}

	sysCltuStatus |= TASK_INITIALIZED; 

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevCLTUdpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoCLTUControl_write_ao(aoRecord *precord)
{
	devCLTUdpvt        *pdevCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	drvCLTU_taskConfig          *ptaskConfig;
	drvCLTU_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;



	if(!pdevCLTUdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig			= pdevCLTUdpvt->ptaskConfig;
	pcontrolThreadConfig		= ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig		= ptaskConfig;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;
	qData.param.nPort			= pdevCLTUdpvt->nPort;



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

/*		start = drvCLTU_getCurrentTick(); */

#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		switch(pdevCLTUdpvt->ind) {
/*			case AO_SHOT_START:
				qData.pFunc = devCLTU_set_shotStart;
				break;
*/			case AO_SHOT_TERM:
				qData.pFunc = devCLTU_AO_ShotTerm;
				break;
			case AO_TRIG:
				qData.pFunc = devCLTU_AO_Trig;
				break;
			case AO_CLOCK:
				qData.pFunc = devCLTU_AO_Clock;
				break;
			case AO_T0:
				qData.pFunc = devCLTU_AO_T0;
				break;
			case AO_T1:
				qData.pFunc = devCLTU_AO_T1;
				break;
			case AO_MTRIG:
				qData.pFunc = devCLTU_AO_mTrig;
				break;
			case AO_MCLOCK:
				qData.pFunc = devCLTU_AO_mClock;
				break;
			case AO_MT0:
				qData.pFunc = devCLTU_AO_mT0;
				break;
			case AO_MT1:
				qData.pFunc = devCLTU_AO_mT1;
				break;
/*			case AO_SETUP:
				qData.pFunc = devCLTU_setup;
				break;
*/			case AO_CLOCK_ONSHOT:
				qData.pFunc = devCLTU_set_clockOnShot; /* not used 2009-11-16*/
				break;
			case AO_CALIBRATION:
				qData.pFunc = devCLTU_AO_calibration; /* only for CTU mode 2009-11-16*/
				break;
			case AO_PUT_CALED_VAL:
				qData.pFunc = devCLTU_AO_putCalVal;
				break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		precord->pact = FALSE;
        	precord->udf = FALSE;
#if 0
		switch(pdevCLTUdpvt->ind) {
			case AO_SHOT_START: precord->val = 0; break;
			case AO_SETUP: precord->val = 0; break;
			
			default: break;
		}
#endif
		return 2;    /* don't convert */
	}

	return 0;
}



LOCAL long devAiCLTURawRead_init_record(aiRecord *precord)
{
	devAiCLTURawReaddpvt *pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) malloc(sizeof(devAiCLTURawReaddpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiCLTURawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiCLTURawReaddpvt->taskName,
				   pdevAiCLTURawReaddpvt->arg1,
				   pdevAiCLTURawReaddpvt->arg2);
if( ioc_debug == 2)
	epicsPrintf("devAiCLTURawRead arg num: %d: %s %s %s\n",i, pdevAiCLTURawReaddpvt->taskName, 
								pdevAiCLTURawReaddpvt->arg1,
								pdevAiCLTURawReaddpvt->arg2);

/*			pdevAiCLTURawReaddpvt->ptaskConfig = drvCLTU_get_taskConfig(); */
			pdevAiCLTURawReaddpvt->ptaskConfig = drvCLTU_find_taskConfig(pdevAiCLTURawReaddpvt->taskName);
			if(!pdevAiCLTURawReaddpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiCLTURawRead (init_record) Illegal INP field: task name");
				free(pdevAiCLTURawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiCLTURawRead (init_record) Illegal INP field");
			free(pdevAiCLTURawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_MCLOCK_STR))
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_MCLOCK;
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_TIME_STR)) {
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_TIME;
		
	}
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_OFFSET_STR))
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_OFFSET;
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_ID_STR))
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_ID;
	else {
		recGblRecordError(S_db_badField, (void*) precord,  "devAiCLTURawRead (init_record) error argment");
		free(pdevAiCLTURawReaddpvt);
		precord->udf = TRUE;
		precord->dpvt = NULL;
		return S_db_badField;
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiCLTURawReaddpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devAiCLTURawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devAiCLTURawReaddpvt *pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) precord->dpvt;
	drvCLTU_taskConfig   *ptaskConfig;

	if(!pdevAiCLTURawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevAiCLTURawReaddpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devAiCLTURawRead_read_ai(aiRecord *precord)
{ 
	devAiCLTURawReaddpvt	*pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) precord->dpvt;
	drvCLTU_taskConfig		*ptaskConfig;

	System_Info* mSys;
	unsigned long long int curtime64, temp64;


	if(!pdevAiCLTURawReaddpvt) return 0;

	ptaskConfig		= pdevAiCLTURawReaddpvt->ptaskConfig;
#if WITH_TIMINGBOARD
	mSys = (System_Info*) (ptaskConfig->pciDevice.base2);
#else
	mSys = (System_Info *)malloc(sizeof(System_Info) ); /* temporary */
#endif

	switch(pdevAiCLTURawReaddpvt->ind) {
		case AI_CLTU_MCLOCK:
			precord->val = (double)mSys->ctu.dcm_locked;
			if(ioc_debug==3) epicsPrintf("ctu.dcm_locked: 0x%x (%d)\n", (unsigned int)precord->val, (unsigned int)precord->val);
			break;
		case AI_CLTU_TIME:
#if !WITH_TIMINGBOARD
	mSys->CurrentTime_msb = sysCltuStatus++;
	mSys->CurrentTime_lsb = 0;
#endif
			temp64 = (unsigned long long int)mSys->CurrentTime_msb << 32;
			curtime64 = temp64 + (unsigned long long int)mSys->CurrentTime_lsb;
			precord->val = (double)(curtime64/100000000); 
/*			precord->val = (double)sysCltuStatus; */

#if 1
/*			drvCLTU_set_rtc((unsigned int)precord->val); */
			if(ioc_debug==3) epicsPrintf("current time: %f sec\n", precord->val);
#endif
			
			break;
		case AI_CLTU_OFFSET:
#if !WITH_TIMINGBOARD
	mSys->ltu.compensation_offset1 = sysCltuStatus;
#endif
			precord->val = mSys->ltu.compensation_offset1 * 10; /* unit 10ns */
			if(ioc_debug==3) epicsPrintf("current offset: %f\n", precord->val);
			break;
		case AI_CLTU_ID:
			precord->val = mSys->ltu.id;
			epicsPrintf("cltu id: 0x%X\n", (int)precord->val);
			break;
	}
	return (2);
}


LOCAL long devStringinCLTU_init_record(stringinRecord *precord)
{
	devCLTUdpvt *pdevStringinCLTUdpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevStringinCLTUdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevStringinCLTUdpvt->taskName,
				   pdevStringinCLTUdpvt->arg1,
				   pdevStringinCLTUdpvt->arg2);
if( ioc_debug == 2)
	epicsPrintf("devStringinCLTU arg num: %d: %s %s %s\n",i, pdevStringinCLTUdpvt->taskName, 
								pdevStringinCLTUdpvt->arg1,
								pdevStringinCLTUdpvt->arg2);

/*			pdevStringinCLTUdpvt->ptaskConfig = drvCLTU_get_taskConfig(); */
			pdevStringinCLTUdpvt->ptaskConfig = drvCLTU_find_taskConfig(pdevStringinCLTUdpvt->taskName);
			if(!pdevStringinCLTUdpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devStringinCLTU (init_record) Illegal INP field: task name");
				free(pdevStringinCLTUdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devStringinCLTU (init_record) Illegal INP field");
			free(pdevStringinCLTUdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(!strcmp(pdevStringinCLTUdpvt->arg1, STRINGIN_CLTU_TIME_STR))
		pdevStringinCLTUdpvt->ind = STRINGIN_CLTU_TIME;
	else {
		recGblRecordError(S_db_badField, (void*) precord,  "devStringinCLTU (init_record) error argment");
		free(pdevStringinCLTUdpvt);
		precord->udf = TRUE;
		precord->dpvt = NULL;
		return S_db_badField;
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevStringinCLTUdpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devStringinCLTU_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	devCLTUdpvt *pdevStringinCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	drvCLTU_taskConfig   *ptaskConfig;

	if(!pdevStringinCLTUdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevStringinCLTUdpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devStringinCLTU_read_stringin(stringinRecord *precord)
{ 
	devCLTUdpvt	*pdevStringinCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	drvCLTU_taskConfig *ptaskConfig;

	System_Info* mSys;
	unsigned long long int curtime64, temp64;

	unsigned int day, hour, min, sec;
	unsigned int timer_sec;
	


	if(!pdevStringinCLTUdpvt) return 0;

	ptaskConfig = pdevStringinCLTUdpvt->ptaskConfig;
	
#if WITH_TIMINGBOARD
	mSys = (System_Info*) (ptaskConfig->pciDevice.base2);
#else
	mSys = (System_Info *)malloc(sizeof(System_Info) ); /* temporary */
#endif

	switch(pdevStringinCLTUdpvt->ind) {
		case STRINGIN_CLTU_TIME:
			
			temp64 = (unsigned long long int)mSys->CurrentTime_msb << 32;
			curtime64 = temp64 + (unsigned long long int)mSys->CurrentTime_lsb;

			timer_sec = curtime64/100000000;
			
			day = (int) (timer_sec/T_DAY_DIVIDER);
			timer_sec = timer_sec - (day * T_DAY_DIVIDER);
			hour = (int) (timer_sec/T_HOUR_DIVIDER );
			timer_sec = timer_sec - (hour * T_HOUR_DIVIDER);
			min = (int) (timer_sec/T_MIN_DIVIDER );
			timer_sec = timer_sec - (min * T_MIN_DIVIDER);
			sec = (int) (timer_sec/T_SEC_DIVIDER );

			sprintf(precord->val, "%02d:%02d:%02d", hour, min, sec);

#if 1
/*			drvCLTU_set_rtc((unsigned int)precord->val); */
			if(ioc_debug==3) epicsPrintf("current time: %s\n", precord->val);
#endif
			
			break;
		default: 
			epicsPrintf("\nERROR: %s: precord->dpvt(%d)\n", ptaskConfig->taskName, pdevStringinCLTUdpvt->ind );
			break;
	}
	
	return (2);
	
}

LOCAL long devBoCLTUControl_init_record(boRecord *precord)
{
	devCLTUdpvt *pdevCLTUControldpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int cnt;

	precord->udf = TRUE;
	precord->dpvt = NULL;
	
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevCLTUControldpvt->recordName, precord->name);
			cnt = sscanf(precord->out.value.instio.string, "%s %s %s", 
				pdevCLTUControldpvt->taskName, pdevCLTUControldpvt->arg1, pdevCLTUControldpvt->arg2);
	if( ioc_debug == 2)
		epicsPrintf("devBoCLTUControldpvt arg num: %d: %s %s %s\n",cnt, pdevCLTUControldpvt->taskName, 
									pdevCLTUControldpvt->arg1, pdevCLTUControldpvt->arg2);

/*			pdevCLTUControldpvt->ptaskConfig = drvCLTU_get_taskConfig(); */
			pdevCLTUControldpvt->ptaskConfig = drvCLTU_find_taskConfig(pdevCLTUControldpvt->taskName);

			if(!pdevCLTUControldpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devBoCLTUControl (init_record) Illegal OUT field: task_name");
				free(pdevCLTUControldpvt);
/*				precord->udf = TRUE;
				precord->dpvt = NULL; */
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal OUT field");
			free(pdevCLTUControldpvt);
/*			precord->udf = TRUE;
			precord->dpvt = NULL; */
			return S_db_badField;
				
	}

	if( cnt == 2) {
		if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOT_START_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOT_START;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOW_INFO_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOW_INFO;
		else {
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal OUT field: arg1" );
			free(pdevCLTUControldpvt);
			return S_db_badField;
		}
	}
	else if( cnt == 3) 
	{
		if( !strcmp(pdevCLTUControldpvt->arg1, BO_SETUP_STR) ) {
			pdevCLTUControldpvt->ind = BO_SETUP;
		}
		else {
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal INP field: arg1" );
			free(pdevCLTUControldpvt);
			return S_db_badField;
		}		
		pdevCLTUControldpvt->nPort = atoi(pdevCLTUControldpvt->arg2);
	}
	else{
		recGblRecordError(S_db_badField, (void*) precord,  "devBoCLTUControl (init_record) error argment");
		free(pdevCLTUControldpvt);
		return S_db_badField;
	}
	
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevCLTUControldpvt;
		
	return 2;	  /* don't convert */
}


LOCAL long devBoCLTUControl_write_bo(boRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	devCLTUdpvt		*pdevCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	drvCLTU_taskConfig		*ptaskConfig;
	drvCLTU_controlThreadConfig 		*pcontrolThreadConfig;
	controlThreadQueueData	qData;


	if(!pdevCLTUdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig			= pdevCLTUdpvt->ptaskConfig;
	pcontrolThreadConfig		= ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig		= ptaskConfig;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;

	qData.param.nPort			= pdevCLTUdpvt->nPort;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pdevCLTUdpvt->ind) {
			case BO_SETUP:
				qData.pFunc = devCLTU_BO_SETUP;
				break;
			case BO_SHOT_START:
				qData.pFunc  = devCLTU_BO_SHOT_START;
				break;
			case BO_SHOW_INFO:
				qData.pFunc  = devCLTU_BO_SHOW_INFO;
				break;
			default: return 0;
		}
		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

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
			case AO_DAQ_START: precord->val = 0; break;
			case AO_DAQ_STOP: precord->val = 0; break;
			case AO_ADC_START: precord->val = 0; break;
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


