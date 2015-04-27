
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



#include "drvV792.h" 
#include "myMDSplus.h"
#include "tsPvString.h"


#if 0
#define DEBUG
#endif


/*******************************************************/
/*******************************************************/

static long devAoV792Control_init_record(aoRecord *precord);
static long devAoV792Control_write_ao(aoRecord *precord);

static long devStringoutV792Control_init_record(stringoutRecord *precord);
static long devStringoutV792Control_write_stringout(stringoutRecord *precord);

static long devAiV792RawRead_init_record(aiRecord *precord);
static long devAiV792RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAiV792RawRead_read_ai(aiRecord *precord);

static long devBoV792Control_init_record(boRecord *precord);
static long devBoV792Control_write_bo(boRecord *precord);

/*******************************************************/


struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoV792Control = {
    5,
    NULL,
    NULL,
    devBoV792Control_init_record,
    NULL,
    devBoV792Control_write_bo
};
epicsExportAddress(dset, devBoV792Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoV792Control = {
	6,
	NULL,
	NULL,
	devAoV792Control_init_record,
	NULL,
	devAoV792Control_write_ao,
	NULL
};
epicsExportAddress(dset, devAoV792Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutV792Control = {
	6,
	NULL,
	NULL,
	devStringoutV792Control_init_record,
	NULL,
	devStringoutV792Control_write_stringout,
	NULL
};
epicsExportAddress(dset, devStringoutV792Control);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiV792RawRead = {
	6,
	NULL,
	NULL,
	devAiV792RawRead_init_record,
	devAiV792RawRead_get_ioint_info,
	devAiV792RawRead_read_ai,
	NULL
};
epicsExportAddress(dset, devAiV792RawRead);


/*******************************************************/
/*******************************************************/



unsigned long long int start, stop, _interval;



static void devV792_STRINGOUT_TAG(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_V792 *pST_V792 = (ST_V792*)pSTDdev->pUser;
	
	if( pParam->n32Arg0 > CH_CNT_MAX ) {
		epicsPrintf("\n>>> %d,  input channel limit : %d\n", pParam->n32Arg0, CH_CNT_MAX);
		return;
	}

/*	strcpy(channelTagName[pParam->n32Arg0], "\\");
	strcat(channelTagName[pParam->n32Arg0], pParam->setStr);
*/

//	strcpy(pST_V792->ST_Ch[pParam->n32Arg0].strTagName, pParam->setStr);
	strcpy(pST_V792->strTagName[pParam->n32Arg0], pParam->setStr);

#if 0
//	printf("control thread (tag Name): %s (%d: %s)\n", pSTDdev->taskName, pParam->n32Arg0, pST_V792->ST_Ch[pParam->n32Arg0].strTagName );
	printf("control thread (tag Name): %s (%d: %s)\n", pSTDdev->taskName, pParam->n32Arg0, pST_V792->strTagName[pParam->n32Arg0] );
#endif
}

static void devV792_AO_TEST_PUT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
/*
	char strval[64];
*/
/*	pSTDdev->dT0 = (float)pParam->setValue; */
/*	dbProc_call_LSAVE_start(); */
/*	dbProc_call_RESET_shotseq_started(); */

/*	drvV792_set_testFunc(pSTDdev); */

/*	CAproc_get("CCS_SHOT_NUMBER", strval); */
/*	sprintf(strval, "%d", (int)pParam->setValue );
	CAproc_put("atca:ai1", strval); */

/*
	db_get("TS1_CCS_SHOT_NUMBER", strval);
	db_get("TS1_CCS_BLIP_TIME", strval);
*/


	
	epicsPrintf("control thread (test PUT): %s %s (%s), val: %f\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), (float)pParam->setValue);
}

static void devV792_AO_SET_CALB_ITERATION(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	UINT16 prev;
	
	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}
	prev = pV792->calib_iter_cnt;
	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->calib_iter_cnt = (UINT16)pParam->setValue;
			
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	pSTDdev = pParam->pSTDdev;
	pV792 = (ST_V792 *)pSTDdev->pUser;
	epicsPrintf("Iteration cnt change %d to %d: %s (%s)\n", prev, pV792->calib_iter_cnt, pSTDdev->taskName, precord->name);
}
static void devV792_AO_SET_CALB_INCREASE_CNT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	UINT16 prev;
	
	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}
	prev = pV792->calib_increase_cnt;

	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->calib_increase_cnt = (UINT16)pParam->setValue;
			
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	pSTDdev = pParam->pSTDdev;
	pV792 = (ST_V792 *)pSTDdev->pUser;
	epicsPrintf("Increasement change %d to %d: %s (%s)\n", prev, pV792->calib_increase_cnt, pSTDdev->taskName, precord->name);
}

static void devV792_AO_SET_PEDESTAL(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
/*	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin; */
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	UINT16 reg_value, cur_value;

	if( (UINT16)pParam->setValue > 255 ) 
	{
		epicsPrintf("ERROR! %s: Over the max value (%d)\n", pSTDdev->taskName, (UINT16)pParam->setValue );
		return;
	}	
	cur_value = (UINT16)pParam->setValue;

	cvt_read_reg( &pV792->board_data.m_common_data, CVT_V792_IPED_INDEX, &reg_value);
	epicsPrintf("%s: Current Pedestal (%d)\n", pSTDdev->taskName, reg_value);

	if ( !cvt_V792_set_pedestal( &pV792->board_data, cur_value)) {
		notify_error(1,"%s: pedestal(%d)", pSTDdev->taskName, cur_value);
		return;		
	}
	cvt_read_reg( &pV792->board_data.m_common_data,  CVT_V792_IPED_INDEX, &reg_value);
	pV792->Pedestal = reg_value;
	epicsPrintf("New Pedestal:%d  %s %s (%s)\n", pV792->Pedestal, 
		pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}

static void devV792_AO_SET_STOP_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	UINT16 prev;
/*
	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}
*/	prev = pV792->my_shot_period;
	

	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->my_shot_period = (UINT16)pParam->setValue;
			
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	pSTDdev = pParam->pSTDdev;
	pV792 = (ST_V792 *)pSTDdev->pUser;
	epicsPrintf("My shot stop time changed %d to %d: %s (%s)\n", prev, pV792->my_shot_period, pSTDdev->taskName, precord->name);
}

static void devV792_AO_CHANNEL_MASK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
/*	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin; */
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
	UINT32 prev;

	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}
	prev = pV792->mask_channel;
	
	pV792->mask_channel = (UINT32)pParam->setValue;

	epicsPrintf("Prev Mask 0x%X to 0x%X: %s (%s)\n", prev, pV792->mask_channel, pSTDdev->taskName, precord->name);
}

static void devV792_BO_TOGGLE_realtime_calc(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_V792 *pV792 = NULL;
	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin;

	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->toggle_rt_calc = (char)pParam->setValue;
		if( pV792->toggle_rt_calc )
			epicsPrintf("%s: enable rt_calc. (%d)\n", pSTDdev->taskName, pV792->toggle_rt_calc);
		else
			epicsPrintf("%s: disable rt_calc. (%d)\n", pSTDdev->taskName, pV792->toggle_rt_calc);
	
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	pSTDdev = pParam->pSTDdev;
	epicsPrintf("%s:  %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}
	
static void devV792_BO_TOGGLE_make_file(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_V792 *pV792 = NULL;
	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin;
/*
	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}
*/
	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->use_file_save = (char)pParam->setValue;
		if( pV792->use_file_save )
			epicsPrintf("%s: use file save (%d)\n", pSTDdev->taskName, pV792->use_file_save);
		else
			epicsPrintf("%s: skip making raw files (%d)\n", pSTDdev->taskName, pV792->use_file_save);
	
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	pSTDdev = pParam->pSTDdev;
	epicsPrintf("%s:  %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}

static void devV792_BO_DEV_ARMING(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	if( (int)pParam->setValue == 1 ) /* in case of arming  */
	{
		drvV792_ARM_enable(pSTDdev);
	}
	else 
	{
		drvV792_ARM_disable(pSTDdev);
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);

	epicsPrintf("%s:  %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}

static void devV792_BO_DEV_RUN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	if( (int)pParam->setValue == 1 ) /* in case of arming  */
	{
		drvV792_RUN_start(pSTDdev);
	} 
	else
	{
		drvV792_RUN_stop(pSTDdev);
	}
	scanIoRequest(pSTDdev->ioScanPvt_status);

	epicsPrintf("%s:  %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}


static void devV792_BO_SHOW_STATUS1(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	UINT16 reg_value= 0;

	if( !cvt_V792_get_status_1( &pV792->board_data, &reg_value))
	{
		epicsPrintf( "\nError %s: executing cvt_V792_get_status_1 \n", pSTDdev->taskName );
		return ;
	}
	epicsPrintf("%s: - 0x%X   CVT_V792_STATUS_1_INDEX\n", pSTDdev->taskName, reg_value );

	if( reg_value & 0x1 ) epicsPrintf("\t [0]-1 Data Ready\n");
	else epicsPrintf("\t [0]-0 No Data Ready\n");

	if( reg_value & 0x2 ) epicsPrintf("\t [1]-1 At least one module has Data Ready.\n");
	else epicsPrintf("\t [1]-0 No module has Data Ready.\n");

	if( reg_value & 0x4 ) epicsPrintf("\t [2]-1 Module Busy.\n");
	else epicsPrintf("\t [2]-0 Module not Busy.\n");

	if( reg_value & 0x8 ) epicsPrintf("\t [3]-1 At least one module is Busy.\n");
	else epicsPrintf("\t [3]-0 No module is Busy.\n");

	if( reg_value & 0x10 ) epicsPrintf("\t [4]-1 GEO is not available from the JAUX.\n");
	else epicsPrintf("\t [4]-0 GEO is picked from the JAUX.\n");

	if( reg_value & 0x20 ) epicsPrintf("\t [5]-1 the board is purged.\n");
	else epicsPrintf("\t [5]-0 the board is not purged.\n");

	if( reg_value & 0x40 ) epicsPrintf("\t [6]-1 all Control Bus Terminations are ON.\n");
	else epicsPrintf("\t [6]-0 not all Control Bus Terminations are ON.\n");

	if( reg_value & 0x80 ) epicsPrintf("\t [7]-1 all Control Bus Terminations are OFF.\n");
	else epicsPrintf("\t [7]-0 not all Control Bus Terminations are OFF.\n");

	if( reg_value & 0x100 ) epicsPrintf("\t [8]-1 Indicate 1, not implemented\n");
	else epicsPrintf("\t [8]-0 Indicate 0, not implemented\n");

	epicsPrintf("%s:  %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());

}
static void devV792_BO_SHOW_STATUS2(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	UINT16 reg_value= 0;

	if( !cvt_V792_get_status_2( &pV792->board_data, &reg_value))
	{
		epicsPrintf( "\nError %s: executing cvt_V792_get_status_2 \n", pSTDdev->taskName );
		return ;
	}
	epicsPrintf("%s: - 0x%X   CVT_V792_STATUS_2_INDEX\n", pSTDdev->taskName, reg_value );

	epicsPrintf("\t [0]-null\n");

	if( reg_value & 0x2 ) epicsPrintf("\t [1]-1 Buffer empty.\n");
	else epicsPrintf("\t [1]-0 Buffer not empty.\n");

	if( reg_value & 0x4 ) epicsPrintf("\t [2]-1 Buffer full.\n");
	else epicsPrintf("\t [2]-0 Buffer not full.\n");

	epicsPrintf("\t [3]-null\n");
/*
	if( reg_value & 0x10 ) epicsPrintf("\t [4]-1 GEO is not available from the JAUX.\n");
	else epicsPrintf("\t [4]-0 GEO is picked from the JAUX.\n");

	if( reg_value & 0x20 ) epicsPrintf("\t [5]-1 the board is purged.\n");
	else epicsPrintf("\t [5]-0 the board is not purged.\n");

	if( reg_value & 0x40 ) epicsPrintf("\t [6]-1 all Control Bus Terminations are ON.\n");
	else epicsPrintf("\t [6]-0 not all Control Bus Terminations are ON.\n");

	if( reg_value & 0x80 ) epicsPrintf("\t [7]-1 all Control Bus Terminations are OFF.\n");
	else epicsPrintf("\t [7]-0 not all Control Bus Terminations are OFF.\n");
*/
	epicsPrintf("%s:  %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}


static void devV792_BO_READ_ALL_DATA(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;

	drvV792_read_FullData(pSTDdev);
 
	epicsPrintf("control thread: %s %s (%s), val: %f\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), (float)pParam->setValue);
}

static void devV792_BO_TOGGLE_mean_value(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_MASTER *pAdminCfg = pSTDdev->pST_parentAdmin;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	if( pSTDdev->ST_Base.opMode == OPMODE_CALIBRATION )
	{
		notify_error(1, "%s: Not supported in calib. mode!\n", pSTDdev->taskName);
		return;
	}


	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev)
	{
		pV792 = (ST_V792 *)pSTDdev->pUser;
		pV792->toggle_get_MeanVal = (char)pParam->setValue;
		
		if( pV792->toggle_get_MeanVal ) 
		{
			drvV792_reset_calibration_param(pSTDdev);
			epicsPrintf("%s: get mean value. ON\n", pSTDdev->taskName);
		}
		else
			epicsPrintf("%s: get mean value. OFF\n", pSTDdev->taskName);

		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	
	pSTDdev = pParam->pSTDdev;
	epicsPrintf("%s %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf() );

#if 0
	pV792->toggle_get_MeanVal = (char)pParam->setValue;

	if( pV792->toggle_get_MeanVal ) 
	{
	/*
		int i;
		for(i = 0; i<pV792->chNum; i++) {
			pV792->ST_Ch[i].acuum_value = 0;
			pV792->ST_Ch[i].mean_value = 0;
		}
		pV792->accum_cnt = 0;
		*/
		drvV792_reset_calibration_param(pSTDdev);
	}
	epicsPrintf("%s %s (%s), val: %d\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), pV792->toggle_get_MeanVal);
	#endif
}

static void devV792_BO_SHOW_MEAN_VALUE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
//	int i;

	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}

	printf("%s:  Ipad: %d, pulse width: ??? \n", pSTDdev->taskName, pV792->Pedestal );
/*	for( i=0; i< pV792->chNum; i++) 
	{
		printf("      ch%d, %f, (= %d/%d ) \n",  i,
			pV792->ST_Ch[i].mean_value, 
			pV792->ST_Ch[i].acuum_value, 
			pV792->accum_cnt );
	}
*/
	epicsPrintf("%s %s (%s), val: %d\n", pSTDdev->taskName, precord->name,
										 epicsThreadGetNameSelf(), pV792->toggle_get_MeanVal);
}

static void devV792_BO_SHOW_CALIB_INFO(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;
//	int i,j;

	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf("ERROR!   %s not Idle condition! please check again. (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return;
	}

	printf("%s:  total Cnt: %d, pulse width: ??? \n", pSTDdev->taskName, pV792->accum_cnt );
/*	for( i=0; i<10; i++) 
	{
		printf("Iped:%d, ", i );
		for( j=0; j< pV792->chNum; j++) {
			printf("%.2f ",  pV792->ST_Ch[j].Iped_ref[i] );
		}
		printf("\n");
	}
	*/
	epicsPrintf("%s %s (%s)\n", pSTDdev->taskName, precord->name, epicsThreadGetNameSelf());
}


static long devBoV792Control_init_record(boRecord *precord)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	
	switch(precord->out.type) 
	{
		case INST_IO:
			i = sscanf(precord->out.value.instio.string, "%s %s", pST_dpvt->devName, arg0);
#if PRINT_DEV_SUPPORT_ARG
	epicsPrintf("devBoV792Control arg num: %d: %s %s\n",i, pST_dpvt->devName, arg0);
#endif
			pST_dpvt->pSTDdev = get_STDev_from_name(pST_dpvt->devName);
			if(!pST_dpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devBoV792Control (init_record) Illegal INP field: task_name");
				free(pST_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devBoV792Control (init_record) Illegal OUT field");
			free(pST_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}
	
	
	if(!strcmp(arg0, BO_READ_ALL_DATA_STR)) 
		pST_dpvt->ind = BO_READ_ALL_DATA;

	else if(!strcmp(arg0, BO_SHOW_STATUS1_STR)) 
		pST_dpvt->ind = BO_SHOW_STATUS1;

	else if(!strcmp(arg0, BO_SHOW_STATUS2_STR)) 
		pST_dpvt->ind = BO_SHOW_STATUS2;

	else if(!strcmp(arg0, BO_DEV_ARMING_STR)) 
		pST_dpvt->ind = BO_DEV_ARMING;

	else if(!strcmp(arg0, BO_DEV_RUN_STR)) 
		pST_dpvt->ind = BO_DEV_RUN;

	else if(!strcmp(arg0, BO_TOGGLE_mean_value_STR)) 
		pST_dpvt->ind = BO_TOGGLE_mean_value;

	else if(!strcmp(arg0, BO_SHOW_MEAN_VALUE_STR)) 
		pST_dpvt->ind = BO_SHOW_MEAN_VALUE;
	else if(!strcmp(arg0, BO_SHOW_CALIB_INFO_STR)) 
		pST_dpvt->ind = BO_SHOW_CALIB_INFO;	

	else if(!strcmp(arg0, BO_TOGGLE_make_file_STR)) 
		pST_dpvt->ind = BO_TOGGLE_make_file;
	else if(!strcmp(arg0, BO_TOGGLE_realtime_calc_STR)) 
		pST_dpvt->ind = BO_TOGGLE_realtime_calc;

	else {
		pST_dpvt->ind = -1;
		epicsPrintf("ERROR! devBoV792Control_init_record: arg0 \"%s\" \n",  arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pST_dpvt;
		
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBoV792Control_write_bo(boRecord *precord)
{
	ST_dpvt		*pST_dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg 	*pControlThreadConfig;
	ST_threadQueueData	qData;

	if(!pST_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev			= pST_dpvt->pSTDdev;
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
		switch(pST_dpvt->ind) {

			case BO_READ_ALL_DATA:
				qData.pFunc = devV792_BO_READ_ALL_DATA;
				break;
			case BO_SHOW_STATUS1:
				qData.pFunc = devV792_BO_SHOW_STATUS1;
				break;
			case BO_SHOW_STATUS2:
				qData.pFunc = devV792_BO_SHOW_STATUS2;
				break;
			case BO_DEV_ARMING:
				qData.pFunc = devV792_BO_DEV_ARMING;
				break;
			case BO_DEV_RUN:
				qData.pFunc = devV792_BO_DEV_RUN;
				break;
			case BO_TOGGLE_mean_value:
				qData.pFunc = devV792_BO_TOGGLE_mean_value;
				break;
			case BO_SHOW_MEAN_VALUE:  qData.pFunc = devV792_BO_SHOW_MEAN_VALUE; break;
			case BO_SHOW_CALIB_INFO:  qData.pFunc = devV792_BO_SHOW_CALIB_INFO; break;
			case BO_TOGGLE_make_file:  qData.pFunc = devV792_BO_TOGGLE_make_file; break;
			case BO_TOGGLE_realtime_calc:  qData.pFunc = devV792_BO_TOGGLE_realtime_calc; break;
			
			default: break;

		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
#if 0
		switch(pST_dpvt->ind) {
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

	return -1;
}


static long devAoV792Control_init_record(aoRecord *precord)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			i = sscanf(precord->out.value.instio.string, "%s %s", pST_dpvt->devName, arg0);

#if PRINT_DEV_SUPPORT_ARG
			epicsPrintf("devAoV792Control arg num: %d: %s %s\n",i, pST_dpvt->devName,  arg0);
#endif
			pST_dpvt->pSTDdev = get_STDev_from_name(pST_dpvt->devName);
			if(!pST_dpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoV792Control (init_record) Illegal INP field: task_name");
				free(pST_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}	   
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoV792Control (init_record) Illegal OUT field");
			free(pST_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(arg0, AO_SET_PEDESTAL_STR)) {
		pST_dpvt->ind = AO_SET_PEDESTAL;
/*		pST_dpvt->pchannelConfig 
			= (ST_V792_channel*)ellFirst(pST_dpvt->pSTDdev->pchannelConfig); */
	} 	
	else if (!strcmp(arg0, AO_TEST_PUT_STR)) {
		pST_dpvt->ind = AO_TEST_PUT;
	}
	else if (!strcmp(arg0, AO_SET_CALB_ITERATION_STR)) {
		pST_dpvt->ind = AO_SET_CALB_ITERATION;
	}
	else if (!strcmp(arg0, AO_SET_CALB_INCREASE_CNT_STR)) {
		pST_dpvt->ind = AO_SET_CALB_INCREASE_CNT;
	}
	else if (!strcmp(arg0, AO_SET_STOP_TIME_STR)) {
		pST_dpvt->ind = AO_SET_STOP_TIME;
	}
	else if (!strcmp(arg0, AO_CHANNEL_MASK_STR)) {
		pST_dpvt->ind = AO_CHANNEL_MASK;
	}
	else {
		pST_dpvt->ind = -1;
		epicsPrintf("ERROR! devAoV792Control_init_record: arg0 \"%s\" \n",  arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pST_dpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAoV792Control_write_ao(aoRecord *precord)
{
	ST_dpvt        *pST_dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pST_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev			= pST_dpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.n32Arg0		= pST_dpvt->pchannelConfig->chanIndex; */
	qData.param.setValue		= precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pST_dpvt->ind) {
			case AO_SET_PEDESTAL:
				qData.pFunc = devV792_AO_SET_PEDESTAL;
				break;
			
			case AO_TEST_PUT:
				qData.pFunc = devV792_AO_TEST_PUT;
				break;
			case AO_SET_CALB_ITERATION:
				qData.pFunc = devV792_AO_SET_CALB_ITERATION;
				break;
			case AO_SET_CALB_INCREASE_CNT:
				qData.pFunc = devV792_AO_SET_CALB_INCREASE_CNT;
				break;
			case AO_SET_STOP_TIME:
				qData.pFunc = devV792_AO_SET_STOP_TIME;
				break;
			case AO_CHANNEL_MASK:
				qData.pFunc = devV792_AO_CHANNEL_MASK;
				break;
			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*(0)=>(success ) */
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s) \n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;

		return 0; /*(0)=>(success ) */
	}

	return -1;
}


static long devStringoutV792Control_init_record(stringoutRecord *precord)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];

	switch(precord->out.type) 
	{
		case INST_IO:
			i = sscanf(precord->out.value.instio.string, "%s %s %s", pST_dpvt->devName, arg0, arg1);

#if PRINT_DEV_SUPPORT_ARG
			epicsPrintf("devAoV792Control arg num: %d: %s %s\n",i, pST_dpvt->devName,  arg0, arg1);
#endif
			pST_dpvt->pSTDdev = get_STDev_from_name(pST_dpvt->devName);
			if(!pST_dpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutV792Control (init_record) Illegal INP field: task_name");
				free(pST_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutV792Control (init_record) Illegal OUT field");
			free(pST_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(arg0, STRINGOUT_TAG_STR)) {
		pST_dpvt->ind = STRINGOUT_TAG;
		pST_dpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* port id */
	}
	else {
		pST_dpvt->ind = -1;
		epicsPrintf("ERROR! devStringoutV792Control_init_record: arg0 \"%s\" \n",  pST_dpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pST_dpvt;
	
	return 0; /*returns: (-1,0)=>(failure,success)*/
}


static long devStringoutV792Control_write_stringout(stringoutRecord *precord)
{
	ST_dpvt        *pST_dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device          *pSTDdev;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;

	if(!pST_dpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}

	pSTDdev			= pST_dpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.n32Arg0 = pST_dpvt->n32Arg0;
/*	qData.param.setValue		= precord->val; */
	strcpy( qData.param.setStr, precord->val );

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{
		precord->pact = TRUE;
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pST_dpvt->ind)
		{
			case STRINGOUT_TAG:
				qData.pFunc = devV792_STRINGOUT_TAG;
				break;
			default:
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0; /*(-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) 
	{
#if PRINT_PHASE_INFO
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf() );
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1;    /*(-1,0)=>(failure,success)*/
}


static long devAiV792RawRead_init_record(aiRecord *precord)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	
	switch(precord->inp.type)
	{
		case INST_IO:
			i = sscanf(precord->inp.value.instio.string, "%s %s %s", pST_dpvt->devName, arg0, arg1);
#if PRINT_DEV_SUPPORT_ARG
			epicsPrintf("devAiV792RawRead arg num: %d: %s %s %s\n",i, pST_dpvt->devName, 
								arg0,
								arg1);
#endif
			pST_dpvt->pSTDdev = get_STDev_from_name(pST_dpvt->devName);
			if(!pST_dpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiV792RawRead (init_record) Illegal INP field: task name");
				free(pST_dpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			if( i > 2)
			{
				pST_dpvt->pchannelConfig = drvV792_find_channelConfig(pST_dpvt->devName,
											         pST_dpvt->arg0);
				if(!pST_dpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devAiV792RawRead (init_record) Illegal INP field: channel name");
					free(pST_dpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiV792RawRead (init_record) Illegal INP field");
			free(pST_dpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if( i == 2 ){
		if(!strcmp(arg0, AI_READ_STATE_STR))
			pST_dpvt->ind = AI_READ_STATE;
		
		
	}
	else if( i==3 ) {
		if(!strcmp(arg1, AI_RAW_CUR_VALUE_STR)) {
			pST_dpvt->ind = AI_RAW_CUR_VALUE;
			pST_dpvt->n32Arg0 = strtoul(arg0,NULL,0);
		} 
		else if(!strcmp(arg1, AI_RAW_MEAN_VALUE_STR)) {
			pST_dpvt->ind = AI_RAW_MEAN_VALUE;
			pST_dpvt->n32Arg0 = strtoul(arg0,NULL,0);
		} 
		else if(!strcmp(arg1, AI_RAW_BG_MEAN_VALUE_STR)) {
			pST_dpvt->ind = AI_RAW_BG_MEAN_VALUE;
			pST_dpvt->n32Arg0 = strtoul(arg0,NULL,0);
		} 
		else if(!strcmp(arg1, AI_Te_VALUE_STR)) {
			pST_dpvt->ind = AI_Te_VALUE;
			pST_dpvt->n32Arg0 = strtoul(arg0,NULL,0);
		} 
		else pST_dpvt->ind = -1;

	}
	else {
		epicsPrintf("ERROR! devAiV792RawRead_init_record: arg cnt (%d) \n",  i );
		return -1;    /*returns: (-1,0)=>(failure,success)*/
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pST_dpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAiV792RawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pST_dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pST_dpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pST_dpvt->pSTDdev;
	if( pST_dpvt->ind == AI_READ_STATE)
		*ioScanPvt  = pSTDdev->ioScanPvt_status;
	else
		*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAiV792RawRead_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pST_dpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
/*	ST_V792_channel	*pchannelConfig; */
	ST_V792 *pV792;
	int chId;


	if(!pST_dpvt) return 0; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev		= pST_dpvt->pSTDdev;
/*	pchannelConfig	= pST_dpvt->pchannelConfig; */
	chId = pST_dpvt->n32Arg0;

	pV792 = (ST_V792*)pSTDdev->pUser;

	switch(pST_dpvt->ind) {
		case AI_RAW_CUR_VALUE:
			/*
			if( pV792->ST_Ch[chId].ST_Calc[0].isActual )
				precord->val = (float)pV792->ST_Ch[chId].ST_Calc[0].raw_cur;
			else
				precord->val = (float)pV792->ST_Ch[chId].ST_Calc[0].raw_cur_1;
				*/
//			precord->val = (double)pV792->ST_Ch[chId].rawVal_current;
			precord->val = (double)pV792->rawVal_current[chId];
/*			printf("%s:%d raw value: %d (0x%x)\n", pSTDdev->taskName, chId, (int)precord->val, (int)precord->val ); */
			break;
		case AI_RAW_MEAN_VALUE:
//			precord->val = pV792->ST_Ch[chId].mean_value;
/*			printf("%s:%d mean value: %d (0x%x)\n", pSTDdev->taskName, chId, (int)precord->val, (int)precord->val ); */
			break;
		case AI_RAW_BG_MEAN_VALUE:
//			precord->val = pV792->ST_Ch[chId].mean_value_bg;
			break;
		case AI_READ_STATE:
			precord->val = pSTDdev->StatusDev;
/*			epicsPrintf("%s StatusDev: 0x%X : 0x%X\n", pSTDdev->taskName, (int)precord->val, pV792->boardConfigure);  */
			break;
		case AI_Te_VALUE:
/*			precord->val =  pV792->f64Te[chId]; */
			break;
		
		default:
			epicsPrintf("\n>>> %s: %d ... ERROR! \n", pSTDdev->taskName, pST_dpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}

