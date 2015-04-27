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
add function " devCLTU_AO_SET_CAL_VALUE() "


2007. 11. 20
changed in Func devCLTU_AO_mTrig()....
-	ptaskConfig->mPortConfig[pParam->nArg0].nTrigMode = xxxxxx;
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

/*
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

#include "dbAccess.h"  /*#include "dbAccessDefs.h" --> S_db_badField */

#include "boRecord.h"
#include "aiRecord.h"
#include "aoRecord.h"
#include "stringinRecord.h"

#include "epicsExport.h"


#include "drvCLTU.h"

#include "pvList.h"
#include "cltu_Def.h"

#include "oz_xtu2.h"
#include "oz_cltu.h"


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
void devCLTU_AO_SET_CAL_VALUE(execParam *pParam);

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
	char arg3[60];
	char recordName[80];

	ST_drvCLTU *pCfg;

	unsigned	ind;
	int  nArg0;

} devCLTUdpvt;

typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];
	char arg3[60];
	char recordName[80];

	ST_drvCLTU *pCfg;

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
unsigned int gap_R1_R2 = 0;

void devCLTU_BO_PORT_ENABLE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. (%d)\n", pCfg->u8Version);
		return;
	}
	pCfg->xPortConfig[ pParam->nArg0 ].cOnOff = (unsigned char)pParam->setValue;
	if( pCfg->xPortConfig[ pParam->nArg0 ].cOnOff ) 
		printf("%s: port %d 'Enabled'\n", pCfg->taskName, pParam->nArg0);
	else
		printf("%s: port %d 'Disabled'\n", pCfg->taskName, pParam->nArg0);
 #if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
 #endif
/*	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf(), ); */
}
void devCLTU_BO_PORT_MODE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. (%d)\n", pCfg->u8Version);
		return;
	}
	pCfg->xPortConfig[ pParam->nArg0 ].cTrigOnOff = (unsigned char)pParam->setValue;

	if( pCfg->xPortConfig[ pParam->nArg0 ].cTrigOnOff ) 
		printf("%s: port %d set 'Trigger'\n", pCfg->taskName, pParam->nArg0);
	else
		printf("%s: port %d set 'Clock'\n", pCfg->taskName, pParam->nArg0);
	
 #if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
 #endif
/*	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf()); */
}
void devCLTU_BO_PORT_ActiveLow(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. (%d)\n", pCfg->u8Version);
		return;
	}
	pCfg->xPortConfig[ pParam->nArg0 ].cActiveLow = (unsigned char)pParam->setValue;

	if( pCfg->xPortConfig[ pParam->nArg0 ].cActiveLow ) 
		printf("%s: port %d set 'Active Low'\n", pCfg->taskName, pParam->nArg0);
	else
		printf("%s: port %d set 'Active High'\n", pCfg->taskName, pParam->nArg0);

#if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
 #endif
/*	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf()); */
}



void devCLTU_BO_SETUP(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pParam->nArg0 >= NUM_PORT_CNT) {
		printf("CLTU_ERROR! devCLTU_BO_SETUP (%d): Port must be 0~8\n", pParam->nArg0);
		return ;
	}

	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
	
	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf());
}

void devCLTU_BO_SHOW_INFO(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */
#if WITH_TIMINGBOARD
	drvCLTU_show_info(pCfg);
#endif
/*	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf()); */
}


void devCLTU_BO_SHOT_START(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	epicsTimeStamp epics_time;
	char buf[64];
#if WITH_TIMINGBOARD
	if( pCfg->u8Version == VERSION_2 && pCfg->u32Type == XTU_MODE_CTU )
	{
/*************************************************/
/*  first tried....
		pCfg = get_first_CLTUdev();
		while(pCfg) {
			if( drvCLTU_set_shotStart (pCfg) == WR_ERROR )
				printf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
	  		pCfg = (ST_drvCLTU*) ellNext(&pCfg->node);
		}
		pCfg = pParam->pCfg;
*/

		ST_drvCLTU *pTSS=NULL, *pOld = NULL;
		PCI_COM_S* msg;
		unsigned int current_time=0, current_tick=0;
		unsigned int shot_start_time, shot_start_tick;  
		unsigned int shot_End_Time, shot_End_Tick;


		pTSS = get_first_CLTUdev();
  		pOld = (ST_drvCLTU*) ellNext(&pTSS->node);

		if(pOld->pciDevice.init != 0xFFFFFFFF)
		{
		    CLTU_ASSERT(); return;
		}

/*************************************************/
/*  second tried..
		if ( cltu_cmd_set_shot_start( pOld ) == WR_ERROR ) {
			printf("\n>>> drvCLTU_set_shotStart : R1, cltu_cmd_set_shot_start() ... error!\n" );
			return ;
		}

		if( drvCLTU_set_shotStart (pTSS) == WR_ERROR ){
			printf("\n>>> devCLTU_BO_SHOT_START : New version ... error!\n" );
			return ;
		}
*/

/*************************************************/
/* third tried.... */

		msg = (PCI_COM_S*) (pOld->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);


		shot_End_Time = 0xffffffff;
		shot_End_Tick = 0xffffffff;

/*		xtu_GetCurrentTime(pTSS, &current_time, &current_tick);*/
		current_time      = READ32(pTSS->pciDevice.base0 + XTUR010_);
		current_tick      = READ32(pTSS->pciDevice.base0 + XTUR011_);

/*		shot_start_tick = current_tick + 0x00030d40 + gap_R1_R2; */
		shot_start_tick = current_tick + gap_R1_R2;  /* included 1ms in gap_R1_R2 */

		if (shot_start_tick > 0x0BEBC1FF)
		{
			shot_start_tick = shot_start_tick % 0x0BEBC1FF;
			shot_start_time = current_time + 1;
		}
		else
		{
			shot_start_tick = shot_start_tick;
			shot_start_time = current_time;
		}


		if( msg->status != 0xFFFFFFFF)	{
			printf("R1, status error in shot start!\n");
			return;
		}
#ifdef _CLTU_VXWORKS_ 
		taskLock();
#endif

		msg->address = 0;
		msg->status  = 0;
		msg->data    = 0;
		msg->cmd     = CLTU_CMD_SHOT_START;

/*		xtu_SetShotTime(pTSS, shot_start_time, shot_start_tick, 0xffffffff, 0xffffffff);*/
		WRITE32(pTSS->pciDevice.base0 + XTUR008_TSSTM, shot_start_time);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR009_TSSTL, shot_start_tick);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00A_TSETM, shot_End_Time);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00B_TSETL, shot_End_Tick);
		DELAY_WAIT(0);
/*		ctu_SerdesTxImmediate(pTSS);*/
		WRITE32(pTSS->pciDevice.base0 + XTUR00E_, BIT31);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00E_, 0);
		DELAY_WAIT(0);

#ifdef _CLTU_VXWORKS_ 
		taskUnlock( ); 
#endif



	}
	else {
		if( drvCLTU_set_shotStart (pCfg) == WR_ERROR )
			printf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
		}
#endif
	epicsTimeGetCurrent(&epics_time);
	epicsTimeToStrftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &epics_time);

	printf("%s (%s): %s, %d\n", precord->name, epicsThreadGetNameSelf(), buf, pCfg->nCCS_shotNumber);
}

void devCLTU_BO_IRIGB_ENABLE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version != VERSION_2 ){
		printf("CLTU_ERROR: IRIG-B control is only for R2 \n");
		return;
	}

	if( (pCfg->u32Type == XTU_MODE_CTU) || (pCfg->u32Type == XTU_MODE_CTU_CMPST) ) 
	{
		pCfg->enable_IRIGB = (unsigned char)pParam->setValue;
		if( ctu_SetIRIG_B_Enable(pCfg, pCfg->enable_IRIGB) == ERROR ) {
			printf("CLTU_ERROR: devCLTU_BO_IRIGB_ENABLE:ctu_SetIRIG_B_Enable()...failed!\n");
		}
#if 0
		if( pCfg->enable_IRIGB )
			printf("%s: IRIG-B enabled!\n", pCfg->taskName);
		else
			printf("%s: IRIG-B disabled!\n", pCfg->taskName);
#endif
	} 
	else 
		printf("CLTU_ERROR: IRIG-B control is only for CTU. \n");

}

void devCLTU_BO_SET_FREE_RUN(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	
	if( pCfg->u8Version != VERSION_2 ){
		printf("CLTU_ERROR: free run control is only for R2 \n");
		return;
	}
	if( (pCfg->u32Type == XTU_MODE_CTU) || (pCfg->u32Type == XTU_MODE_CTU_CMPST) ){
		printf("CLTU_ERROR: free run control is only for LTU\n");
		return;
	}

	pCfg->enable_LTU_freeRun = (unsigned char)pParam->setValue;
	if( pCfg->enable_LTU_freeRun ) {
		pCfg->u32Type = XTU_MODE_LTU_FREE;
		printf("%s: LTU id(%d)  is standalone operation.\n", pCfg->taskName, pCfg->xtu_ID);		
	} else {
		pCfg->u32Type = XTU_MODE_LTU;
		printf("%s: LTU id(%d)  is synchronized to CTU!\n", pCfg->taskName, pCfg->xtu_ID);
	}
	
	if( is_mmap_ok(pCfg) ) {
		xtu_SetMode(pCfg, pCfg->u32Type, pCfg->xtu_ID);
		xtu_reset(pCfg);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SET_MODE, &pCfg->u32Type);
		if (status == -1) printf("devCLTU_BO_SET_FREE_RUN: IOCTL_CLTU_R2_SET_MODE.  error!!\n");
#endif
	}
	
}
/*
void devCLTU_BO_SET_COMP_RUN(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	
	if( pCfg->u8Version != VERSION_2 ){
		printf("CLTU_ERROR: Compensation control is only for R2 \n");
		return;
	}
	if( (pCfg->u32Type == XTU_MODE_LTU) || (pCfg->u32Type == XTU_MODE_LTU_FREE) ){
		printf("CLTU_ERROR: Compensation control is only for CTU\n");
		return;
	}

	pCfg->enable_CTU_comp = (unsigned char)pParam->setValue;
	if( pCfg->enable_CTU_comp ) {
		pCfg->u32Type = XTU_MODE_CTU_CMPST;
		printf("%s: CTU id(%d) set Compensation Mode\n", pCfg->taskName, pCfg->xtu_ID);
	} else {
		pCfg->u32Type = XTU_MODE_CTU;
		printf("%s: CTU id(%d) set Normal Operation.\n", pCfg->taskName, pCfg->xtu_ID);
	}
	xtu_SetMode(pCfg, pCfg->u32Type, pCfg->xtu_ID);
	xtu_reset(pCfg);
	
}
*/

void devCLTU_BO_SHOT_END(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord;
	epicsTimeStamp epics_time; */
/*	char buf[64]; */

	if( pCfg->u8Version == VERSION_2 )
	{

		drvCLTU_set_shotEnd(pCfg);
/*
		printf("Current time        : %d sec - 0x%08x tick \n",current_time,current_tick);  
		printf("CTU Shot Start time : %d sec - 0x%08x tick \n",shot_end_time,shot_end_tick);  
		printf("CTU Shot End time   : %d sec - 0x%08x tick \n",0xffffffff,0xffffffff);  
  */
	}
	else 
	{

	}

}

void devCLTU_BO_USE_REF_CLK(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version == VERSION_1 && pCfg->u32Type == XTU_MODE_CTU)
	{
		if( (int)pParam->setValue ) {
			drvCLTU_set_RefClock(pCfg, 0xffffffff );
			printf("CTUV1 use external 50MHz clock!\n");
		}
		else {
			drvCLTU_set_RefClock(pCfg, 0x0 );
			printf("CTUV1 use internal clock!\n");
		}
	}
}

void devCLTU_BO_RUN_CALIBRATION(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pCfg->u8Version == VERSION_1 )
		pCfg->calib_target_ltu = (int)pParam->setValue;
	else if ( pCfg->u8Version == VERSION_2 )
	{
		if( (pCfg->u32Type == XTU_MODE_LTU) || (pCfg->u32Type == XTU_MODE_LTU_FREE) ){
			printf("CLTU_ERROR: Calibration control is only for CTU\n");
			return;
		}

		pCfg->enable_CTU_comp = (unsigned char)pParam->setValue;
		if( pCfg->enable_CTU_comp ) {
			pCfg->u32Type = XTU_MODE_CTU_CMPST;
			printf("%s: CTU id(%d) set Compensation Mode\n", pCfg->taskName, pCfg->xtu_ID);
		} else {
			pCfg->u32Type = XTU_MODE_CTU;
			printf("%s: CTU id(%d) set Normal Operation.\n", pCfg->taskName, pCfg->xtu_ID);
		}
	}
	else {
		printf("CLTU_ERROR! Wrong version. (%d)\n", pCfg->u8Version);
		return;
	}
		

	if( drvCLTU_run_calibration (pCfg ) == WR_ERROR )
		printf("\n>>> devCLTU_AO_calibration : drvCLTU_run_calibration(%d) ... error!\n", (int)pParam->setValue );

	if( ioc_debug == 1)
		printf("control thread (calibarion for %f): %s (%s)\n", pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}


void devCLTU_AO_GAP_R1R2(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. Gap supported by only R2.(%d)\n", pCfg->u8Version);
		return;
	}
/*	drvCLTU_set_tGap((unsigned int) pParam->setValue); */

	printf("Old gap value: %d\n", gap_R1_R2);
	gap_R1_R2 = (unsigned int) pParam->setValue;
	printf("New gap value: %d\n", gap_R1_R2);

}

/* 100MHz / (value+1) */
void devCLTU_AO_SECTION_CLK(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. (%d)\n", pCfg->u8Version);
		return;
	}
	if( (unsigned int)pParam->setValue == 0) {
		printf("CLTU_ERROR! 0Hz is invalid. (%dHz)  change to 1Hz\n", (unsigned int)pParam->setValue);
		pParam->setValue = 1.0;
	}
	pCfg->xPortConfig[ pParam->nArg0 ].clock[pParam->nArg1] = (unsigned int)((EPICS_CLOCK_100M / (unsigned int)pParam->setValue) -1);
	if( ioc_debug == 2)
		printf("%s: port:%d, sec:%d, clock:%dHz, 0x%x\n", pCfg->taskName, pParam->nArg0, pParam->nArg1,
		(unsigned int)pParam->setValue, pCfg->xPortConfig[ pParam->nArg0 ].clock[pParam->nArg1]  );
#if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
#endif
	if( ioc_debug == 1)
		printf("control thread (ccs shot number : %d): %s (%s)\n", (unsigned int)pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}
void devCLTU_AO_SECTION_T0(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. (%d)\n", pCfg->u8Version);
		return;
	}
	pCfg->xPortConfig[ pParam->nArg0 ].startOffset[pParam->nArg1] = (unsigned long long int)(pParam->setValue * EPICS_CLOCK_200M + 1  );
	if( ioc_debug == 2)
		printf("%s: port:%d, sec:%d, t0:%f -> %lld\n", pCfg->taskName, pParam->nArg0, pParam->nArg1,
		pParam->setValue, pCfg->xPortConfig[ pParam->nArg0 ].startOffset[pParam->nArg1]  );
#if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
#endif
	if( ioc_debug == 1)
		printf("control thread (ccs shot number : %d): %s (%s)\n", (unsigned int)pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}
void devCLTU_AO_SECTION_T1(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pCfg->u8Version != VERSION_2 )	{
		printf("CLTU_ERROR! not new version. (%d)\n", pCfg->u8Version);
		return;
	}
	pCfg->xPortConfig[ pParam->nArg0 ].stopOffset[pParam->nArg1] = (unsigned long long int)(pParam->setValue * EPICS_CLOCK_200M + 1 );
	if( ioc_debug == 2)
		printf("%s: port:%d, sec:%d, t1:%f -> %lld\n", pCfg->taskName, pParam->nArg0, pParam->nArg1,
		pParam->setValue, pCfg->xPortConfig[ pParam->nArg0 ].stopOffset[pParam->nArg1]  );

#if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
#endif
	if( ioc_debug == 1)
		printf("control thread (ccs shot number : %d): %s (%s)\n", (unsigned int)pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}	



void devCLTU_AO_SHOT_NUMBER(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	pCfg->nCCS_shotNumber = (unsigned int)pParam->setValue;

	if( ioc_debug == 1)
		printf("control thread (ccs shot number : %d): %s (%s)\n", (unsigned int)pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}

void devCLTU_AO_SET_CAL_VALUE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	FILE *fp=NULL;
	char fname[128];
	unsigned int calval = 0;
/*	static int isFirst = 1; */

	if( (unsigned int)pParam->setValue == 0 ) {
		calval = pCfg->u32Calib_val;
/*		isFirst = 0; */
	} else
		calval = (unsigned int)pParam->setValue;

	if( pCfg->u8Version != VERSION_2 ){
		printf("CLTU_ERROR: put CalVal is only for R2 \n");
		return;
	}

	if( (pCfg->u32Type == XTU_MODE_CTU) || (pCfg->u32Type == XTU_MODE_CTU_CMPST) ){
		printf("CLTU_ERROR: put compensation value is only for LTU\n");
		return;
	}

	if( drvCLTU_put_calibratedVal(pCfg,  calval ) == WR_ERROR )
		printf("\n>>> devCLTU_AO_SET_CAL_VALUE : drvCLTU_put_calibratedVal(%d) ... error!\n", calval );
	else 
	{
		sprintf(fname, "%s/iocBoot/%s/%s%d", pCfg->strTOP, pCfg->strIOC, FILE_CALIB_VALUE, pCfg->xtu_ID);

		fp = fopen(fname, "w");
		if(!fp) {
			printf("CLTU_ERROR: can't open file \"%s\". \n", fname);
			return;
		}
		fprintf(fp, "%d\n", calval );
		fclose(fp);
	}

	if( ioc_debug == 1)
		printf("control thread (calibrated value : %d): %s (%s)\n", calval, precord->name,
			                                            epicsThreadGetNameSelf());
}

/*
void devCLTU_set_shotStart(execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotStart () == WR_ERROR )
		printf("\n>>> devCLTU_set_shotStart : drvCLTU_set_shotStart() ... error!\n" );
#endif

	if( ioc_debug == 1)
		printf("control thread (shot start): %s (%s)\n", precord->name,
			                                            epicsThreadGetNameSelf());
}
*/
void devCLTU_AO_ShotTerm(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;

	pCfg->nShotTerm_msb = (unsigned int) (time64>>32);
	pCfg->nShotTerm_lsb = (unsigned int) time64;

	if( ioc_debug == 1) {	
		printf(">>> shot term msb : 0x%x (%f sec)\n", pCfg->nShotTerm_msb, pParam->setValue );
		printf(">>> shot term lsb : 0x%x\n", pCfg->nShotTerm_lsb );
	}
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotTerm (pCfg) == WR_ERROR )
		printf("\n>>> devCLTU_AO_ShotTerm : drvCLTU_set_shotTerm() ... error!\n" );
#endif
	if( ioc_debug == 1)
		printf("control thread (shot period): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
}

/*  onoff: 1=on, 0=off */
void devCLTU_set_clockOnShot(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_clockOnShot( pCfg, pParam->nArg0,  pParam->setValue) == WR_ERROR )
		printf("\n>>> devCLTU_set_clockOnShot : drvCLTU_set_clockOnShot(%d, %d) ... error!\n", pParam->nArg0 , (int)pParam->setValue);
#endif
	if( ioc_debug == 1)
		printf("control thread (set clock on shot): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
}
/*
void devCLTU_setup(execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_portConfig (pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_setup : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
#endif
	if( ioc_debug == 1)
		printf("control thread (setup p%d): %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf());
}
*/
void devCLTU_AO_Trig(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;


	if( pCfg->u8Version == VERSION_1)
	{
		pCfg->sPortConfig[pParam->nArg0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0;
		pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_TRIG_OK;

		if( ioc_debug == 1) {
			printf(">>> Trigger: port(%d), level(0x%x)\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nTrigMode );
			printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}
	}
	else if( pCfg->u8Version == VERSION_2)
	{
		pCfg->xPortConfig[pParam->nArg0].cActiveLow = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0 : 1;
		pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_TRIG_OK;

		if( ioc_debug == 1) {
			printf(">>> Trigger: port(%d), level(0x%x)\n", pParam->nArg0, pCfg->xPortConfig[pParam->nArg0].cActiveLow);
			printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}
	}
	else
	{
		printf("ERROR_CLTU:  Wrong version (%d), 0x%x\n", pCfg->u8Version, pCfg->u8Version);
		printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}

}

void devCLTU_AO_Clock(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	
	pCfg->sPortConfig[pParam->nArg0].nClock = pParam->setValue;
	pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_CLOCK_OK;
	
	if( ioc_debug == 1) {
		printf(">>> Clock :p(%d) %d Hz\n",pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nClock );
		printf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_T0(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	
	pCfg->sPortConfig[pParam->nArg0].nT0_msb = (unsigned int) (time64>>32);
	pCfg->sPortConfig[pParam->nArg0].nT0_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_T0_OK;

	if( ioc_debug == 1) {
		printf(">>> T0 msb : p(%d) 0x%x  (%f sec)\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT0_msb , pParam->setValue);
		printf(">>> T0 lsb : p(%d) 0x%x\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT0_lsb );
  
		printf("control thread (set t0): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_T1(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	
	pCfg->sPortConfig[pParam->nArg0].nT1_msb = (unsigned int) (time64>>32);
	pCfg->sPortConfig[pParam->nArg0].nT1_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_T1_OK;

	if( ioc_debug == 1) {
		printf(">>> T1 msb : p(%d) 0x%x (%f sec)\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT1_msb , pParam->setValue);
		printf(">>> T1 lsb : p(%d) 0x%x\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT1_lsb );

		printf("control thread (set t1): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

/* In this function(multi-trig mode),  we assume "nArg0" is "section number" */
void devCLTU_AO_mTrig(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
/*	pCfg->mPortConfig[pParam->nArg0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0; */
	pCfg->mPortConfig[0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0;
	pCfg->curPortConfigStatus[4] |= PORT_TRIG_OK;
	if( ioc_debug == 1) {
		printf(">>> mTrigger: level(0x%x)\n", pCfg->mPortConfig[0].nTrigMode );
		printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mClock(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pCfg->u8Version == VERSION_1)
	{
		pCfg->mPortConfig[pParam->nArg0].nClock = pParam->setValue;
		pCfg->curPortConfigStatus[4] |= PORT_CLOCK_OK;
		if( ioc_debug == 1) {
			printf(">>> mClock :section(%d) %d Hz\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nClock );
			printf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}
	}
	else if( pCfg->u8Version == VERSION_2)
	{
		pCfg->xPortConfig[pParam->nArg0].clock[0] = (unsigned int) pParam->setValue;
		pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_CLOCK_OK;
		if( ioc_debug == 1) {
			printf(">>> mClock :section(%d) %d Hz\n", pParam->nArg0, pCfg->xPortConfig[pParam->nArg0].clock[0]);
			printf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}

	}
	else
	{
		printf("ERROR_CLTU:  Wrong version (%d), 0x%x\n", pCfg->u8Version, pCfg->u8Version);
		printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
	
}
void devCLTU_AO_mT0(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	pCfg->mPortConfig[pParam->nArg0].nT0_msb = (unsigned int) (time64>>32);
	pCfg->mPortConfig[pParam->nArg0].nT0_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[4] |= PORT_T0_OK;
	if( ioc_debug == 1) {
		printf(">>> mT0 msb : section(%d) 0x%x (%f sec)\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT0_msb, pParam->setValue);
		printf(">>> mT0 lsb : section(%d) 0x%x\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT0_lsb );
		printf("control thread (set t0): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mT1(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	pCfg->mPortConfig[pParam->nArg0].nT1_msb = (unsigned int) (time64>>32);
	pCfg->mPortConfig[pParam->nArg0].nT1_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[4] |= PORT_T1_OK;
	if( ioc_debug == 1) {
		printf(">>> mT1 msb : section(%d) 0x%x (%f sec)\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT1_msb, pParam->setValue);
		printf(">>> mT1 lsb : section(%d) 0x%x\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT1_lsb );
		printf("control thread (set t1): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

LOCAL long devAoCLTUControl_init_record(aoRecord *precord)
{
	devCLTUdpvt *pdevCLTUdpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int i;
	
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevCLTUdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s %s",
				   pdevCLTUdpvt->taskName,
				   pdevCLTUdpvt->arg1,
			           pdevCLTUdpvt->arg2, 
			           pdevCLTUdpvt->arg3);

			if( ioc_debug == 2)
				printf("devAoCLTUControl arg num: %d: %s %s %s\n",i, pdevCLTUdpvt->taskName, 
								pdevCLTUdpvt->arg1,
								pdevCLTUdpvt->arg2);

/*			pdevCLTUdpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevCLTUdpvt->pCfg = drvCLTU_find_taskConfig(pdevCLTUdpvt->taskName);
			
			if(!pdevCLTUdpvt->pCfg) {
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

	pdevCLTUdpvt->nArg0 = -1;
	
	if( i == 2 ) {
/*		if(!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_START_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_START;
		} else */
		if (!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_TERM_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_TERM;
		} else if (!strcmp(pdevCLTUdpvt->arg1, AO_PUT_CALED_VAL_STR)) {
			pdevCLTUdpvt->ind = AO_PUT_CALED_VAL;
		}  else if(!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_NUMBER_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_NUMBER;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_GAP_R1R2_STR)) {
			pdevCLTUdpvt->ind = AO_GAP_R1R2;
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

/*		pdevCLTUdpvt->nArg0 = atoi(pdevCLTUdpvt->arg2); */
	}
	else if (i ==4 ) 
	{
		if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_CLK_STR))
			pdevCLTUdpvt->ind = AO_SECTION_CLK;
		else if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_T0_STR))
			pdevCLTUdpvt->ind = AO_SECTION_T0;
		else if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_T1_STR))
			pdevCLTUdpvt->ind = AO_SECTION_T1;

	}

	sysCltuStatus = 0; 

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevCLTUdpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

LOCAL long devAoCLTUControl_write_ao(aoRecord *precord)
{
	devCLTUdpvt        *pdevCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	ST_drvCLTU          *pCfg;
	drvCLTU_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;



	if(!pdevCLTUdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pCfg			= pdevCLTUdpvt->pCfg;
	pcontrolThreadConfig		= pCfg->pcontrolThreadConfig;
	qData.param.pCfg		= pCfg;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;
/*	qData.param.nArg0			= pdevCLTUdpvt->nArg0; */
	qData.param.nArg0 = atoi(pdevCLTUdpvt->arg2);
	qData.param.nArg1 = atoi(pdevCLTUdpvt->arg3);



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

/*		start = drvCLTU_getCurrentTick(); */

#if PRINT_PHASE_INFO
		printf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
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
			case AO_PUT_CALED_VAL:
				qData.pFunc = devCLTU_AO_SET_CAL_VALUE;
				break;
			case AO_SHOT_NUMBER:
				qData.pFunc = devCLTU_AO_SHOT_NUMBER;
				break;
/****************************************************************/
/*  this for CLTU version 2 */

			case AO_SECTION_CLK:
				qData.pFunc = devCLTU_AO_SECTION_CLK;
				break;
			case AO_SECTION_T0:
				qData.pFunc = devCLTU_AO_SECTION_T0;
				break;
			case AO_SECTION_T1:
				qData.pFunc = devCLTU_AO_SECTION_T1;
				break;
			case AO_GAP_R1R2:
				qData.pFunc = devCLTU_AO_GAP_R1R2;
				break;
				
			default: break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		
#if PRINT_PHASE_INFO
		printf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		precord->pact = FALSE;
        	precord->udf = FALSE;
#if 1
		switch(pdevCLTUdpvt->ind) {
			case AO_PUT_CALED_VAL:
				precord->val = pCfg->u32Calib_val; 
				break;
			
			default: break;
		}
#endif
		return 0;
	}

	return -1;
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
	printf("devAiCLTURawRead arg num: %d: %s %s %s\n",i, pdevAiCLTURawReaddpvt->taskName, 
								pdevAiCLTURawReaddpvt->arg1,
								pdevAiCLTURawReaddpvt->arg2);

/*			pdevAiCLTURawReaddpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevAiCLTURawReaddpvt->pCfg = drvCLTU_find_taskConfig(pdevAiCLTURawReaddpvt->taskName);
			if(!pdevAiCLTURawReaddpvt->pCfg) {
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
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_GET_COMP_TICK_STR))
		pdevAiCLTURawReaddpvt->ind = AI_GET_COMP_TICK;
	
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
	ST_drvCLTU   *pCfg;

	if(!pdevAiCLTURawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pCfg = pdevAiCLTURawReaddpvt->pCfg;
	*ioScanPvt  = pCfg->ioScanPvt;
	return 0;
}

LOCAL long devAiCLTURawRead_read_ai(aiRecord *precord)
{ 
	devAiCLTURawReaddpvt	*pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) precord->dpvt;
	ST_drvCLTU		*pCfg;

	System_Info* mSys;
	unsigned long long int curtime64, temp64;
	unsigned int  CMPST_ID;
	double prev_val;


	if(!pdevAiCLTURawReaddpvt) return 0;

	pCfg		= pdevAiCLTURawReaddpvt->pCfg;
#if WITH_TIMINGBOARD
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		CLTU_ASSERT();
		precord->val = 0; 
		return -1; /*(0,2)=> success and convert,don't convert)*/
	}
	mSys = (System_Info*) (pCfg->pciDevice.base2);
#else
	mSys = (System_Info *)malloc(sizeof(System_Info) ); /* temporary */
#endif

	switch(pdevAiCLTURawReaddpvt->ind) {
		case AI_CLTU_MCLOCK:
			precord->val = (double)mSys->ctu.dcm_locked;
			if(ioc_debug==3) printf("ctu.dcm_locked: 0x%x (%d)\n", (unsigned int)precord->val, (unsigned int)precord->val);
			break;
		case AI_CLTU_TIME:
#if !WITH_TIMINGBOARD
	mSys->CurrentTime_msb = sysCltuStatus++;
	mSys->CurrentTime_lsb = 0;
#endif
			if( pCfg->u8Version == VERSION_1 ) 
			{
				temp64 = (unsigned long long int)mSys->CurrentTime_msb << 32;
				curtime64 = temp64 + (unsigned long long int)mSys->CurrentTime_lsb;
				precord->val = (double)(curtime64/100000000); 
			} 
			else {

				}
/*			precord->val = (double)sysCltuStatus; */

#if 1
/*			drvCLTU_set_rtc((unsigned int)precord->val); */
			if(ioc_debug==3) printf("current time: %f sec\n", precord->val);
#endif
			
			break;
		case AI_CLTU_OFFSET:
#if !WITH_TIMINGBOARD
	mSys->ltu.compensation_offset1 = sysCltuStatus;
#endif
			precord->val = mSys->ltu.compensation_offset1 * 10; /* unit 10ns */
			if(ioc_debug==3) printf("current offset: %f\n", precord->val);
			break;
		case AI_CLTU_ID:
			if( pCfg->u8Version == VERSION_1 ) 
			{
				precord->val = mSys->ltu.id;
				printf("CLTU R1 id: 0x%X\n", (int)precord->val);
			} 
			else if( pCfg->u8Version == VERSION_2 ) 				
			{
				precord->val = pCfg->xtu_ID;
				printf("CLTU R2 id: 0x%X(%d)\n", (int)precord->val, (int)precord->val);
			}
			break;
		case AI_GET_COMP_TICK:			
			if( pCfg->u8Version == VERSION_1 ) 
				precord->val = 0;
			else if( pCfg->u8Version == VERSION_2 )
			{
				prev_val = precord->val;
				
				if( is_mmap_ok(pCfg) ) {
					xtu_GetCompensationID(pCfg,  &CMPST_ID );
					xtu_GetCompensationTime(pCfg, &pCfg->CMPST_Tick );
				} else {
#ifdef _CLTU_LINUX_
					int status;
					status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_CAL_ID, &CMPST_ID);
					if (status == -1) return (-1);
					status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_CAL_TICK, &pCfg->CMPST_Tick);
					if (status == -1) return (-1);
#endif
				}
				if( (CMPST_ID == (unsigned int)pCfg->xtu_ID) || (pCfg->u32Type == XTU_MODE_CTU_CMPST ))
					precord->val = pCfg->CMPST_Tick;
				else
					precord->val = prev_val;
/*				printf("%s: CMPST_Tick: %d\n", pCfg->taskName, (unsigned int)precord->val); */
			}
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
	printf("devStringinCLTU arg num: %d: %s %s %s\n",i, pdevStringinCLTUdpvt->taskName, 
								pdevStringinCLTUdpvt->arg1,
								pdevStringinCLTUdpvt->arg2);

/*			pdevStringinCLTUdpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevStringinCLTUdpvt->pCfg = drvCLTU_find_taskConfig(pdevStringinCLTUdpvt->taskName);
			if(!pdevStringinCLTUdpvt->pCfg) {
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
	ST_drvCLTU   *pCfg;

	if(!pdevStringinCLTUdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pCfg = pdevStringinCLTUdpvt->pCfg;
	*ioScanPvt  = pCfg->ioScanPvt;
	return 0;
}

LOCAL long devStringinCLTU_read_stringin(stringinRecord *precord)
{ 
	devCLTUdpvt	*pdevStringinCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	ST_drvCLTU *pCfg;

	System_Info* mSys;
	unsigned long long int curtime64, temp64;

	unsigned int day, hour, min, sec;
	unsigned int timer_sec;


	if(!pdevStringinCLTUdpvt) {CLTU_ASSERT(); return -1; }/*returns: (-1,0)=>(failure,success)*/

	pCfg = pdevStringinCLTUdpvt->pCfg;
	
#if WITH_TIMINGBOARD
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		CLTU_ASSERT();
		sprintf(precord->val, "Error!");
		return -1; /*returns: (-1,0)=>(failure,success)*/
	}
	mSys = (System_Info*) (pCfg->pciDevice.base2);
#else
	mSys = (System_Info *)malloc(sizeof(System_Info) ); /* temporary */
#endif

	switch(pdevStringinCLTUdpvt->ind) {
	case STRINGIN_CLTU_TIME:
		if( pCfg->u8Version == VERSION_1 ) 
		{
			
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
		}
		else
		{
			unsigned int xtu_current_time, xtu_current_tick;

			if( is_mmap_ok(pCfg) )
				xtu_GetCurrentTime(pCfg, &xtu_current_time, &xtu_current_tick);
			else {
#ifdef _CLTU_LINUX_
				int status;
				status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_CURRENT_TIME, &xtu_current_time);
				if (status == -1) return (-1);
/*				status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_CURRENT_TICK, &xtu_current_tick);
				if (status == -1) return (-1); */
#endif
				}
/*			xtu_time_print_new(pCfg, xtu_current_time, xtu_current_tick); */

			sec  = xtu_current_time % 60;
			min  = (unsigned int)(xtu_current_time / 60	) % 60;
			hour   = (unsigned int)(xtu_current_time / 3600 ) % 24; 
			
			sprintf(precord->val, "%02d:%02d:%02d", hour, min, sec);
/*			printf("%s\n", precord->val); */
		}

#if 1
/*		drvCLTU_set_rtc((unsigned int)precord->val); */
		if(ioc_debug==3) printf("current time: %s\n", precord->val);
#endif
			
		break;
	default: 
		printf("\nERROR: %s: precord->dpvt(%d)\n", pCfg->taskName, pdevStringinCLTUdpvt->ind );
		break;
	}
	
	return (0); /*returns: (-1,0)=>(failure,success)*/
	
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
		printf("devBoCLTUControldpvt arg num: %d: %s %s %s\n",cnt, pdevCLTUControldpvt->taskName, 
									pdevCLTUControldpvt->arg1, pdevCLTUControldpvt->arg2);

/*			pdevCLTUControldpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevCLTUControldpvt->pCfg = drvCLTU_find_taskConfig(pdevCLTUControldpvt->taskName);

			if(!pdevCLTUControldpvt->pCfg) {
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
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOT_END_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOT_END;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_IRIGB_ENABLE_STR)) 
			pdevCLTUControldpvt->ind = BO_IRIGB_ENABLE;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SET_FREE_RUN_STR)) 
			pdevCLTUControldpvt->ind = BO_SET_FREE_RUN;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_RUN_CALIBRATION_STR)) 
			pdevCLTUControldpvt->ind = BO_RUN_CALIBRATION;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_USE_REF_CLK_STR)) 
			pdevCLTUControldpvt->ind = BO_USE_REF_CLK;
		
		else {
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal OUT field: arg1" );
			free(pdevCLTUControldpvt);
			return S_db_badField;
		}
	}
	else if( cnt == 3) 
	{
		if( !strcmp(pdevCLTUControldpvt->arg1, BO_SETUP_STR) )
			pdevCLTUControldpvt->ind = BO_SETUP;

		else if( !strcmp(pdevCLTUControldpvt->arg1, BO_PORT_ENABLE_STR) )
			pdevCLTUControldpvt->ind = BO_PORT_ENABLE;
		
		else if( !strcmp(pdevCLTUControldpvt->arg1, BO_PORT_MODE_STR) )
			pdevCLTUControldpvt->ind = BO_PORT_MODE;

		else if( !strcmp(pdevCLTUControldpvt->arg1, BO_PORT_ActiveLow_STR) )
			pdevCLTUControldpvt->ind = BO_PORT_ActiveLow;

		else {
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal INP field: arg1" );
			free(pdevCLTUControldpvt);
			return S_db_badField;
		}		
		pdevCLTUControldpvt->nArg0 = atoi(pdevCLTUControldpvt->arg2);
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
	ST_drvCLTU		*pCfg;
	drvCLTU_controlThreadConfig 		*pcontrolThreadConfig;
	controlThreadQueueData	qData;


	if(!pdevCLTUdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*returns: (-1,0)=>(failure,success)*/
	}

	pCfg			= pdevCLTUdpvt->pCfg;
	pcontrolThreadConfig		= pCfg->pcontrolThreadConfig;
	qData.param.pCfg		= pCfg;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;

	qData.param.nArg0			= pdevCLTUdpvt->nArg0;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		printf("db processing: phase I %s (%s)\n", precord->name,
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
				
			case BO_PORT_ENABLE:
				qData.pFunc  = devCLTU_BO_PORT_ENABLE;
				break;
			case BO_PORT_MODE:
				qData.pFunc  = devCLTU_BO_PORT_MODE;
				break;
			case BO_PORT_ActiveLow:
				qData.pFunc  = devCLTU_BO_PORT_ActiveLow;
				break;
			case BO_SHOT_END:
				qData.pFunc  = devCLTU_BO_SHOT_END;
				break;
			case BO_IRIGB_ENABLE:
				qData.pFunc  = devCLTU_BO_IRIGB_ENABLE;
				break;
			case BO_SET_FREE_RUN:
				qData.pFunc  = devCLTU_BO_SET_FREE_RUN;
				break;
			case BO_RUN_CALIBRATION:
				qData.pFunc  = devCLTU_BO_RUN_CALIBRATION;
				break;
			case BO_USE_REF_CLK:
				qData.pFunc  = devCLTU_BO_USE_REF_CLK;
				break;
				
			default: return -1; /*returns: (-1,0)=>(failure,success)*/
		}
		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		printf("db processing: phase II %s (%s) %d\n", precord->name,
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
		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1; /*returns: (-1,0)=>(failure,success)*/
}


