
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
#include "waveformRecord.h"


#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "sfwDriver.h"
#include "sfwCommon.h"
#include "sfwMDSplus.h"



#include "drv16aiss8ao4.h" 
#include "pv_list.h" 


/*******************************************************/

static long devBo16aiss8ao4_init_record(boRecord *precord);
static long devBo16aiss8ao4_write_bo(boRecord *precord);

static long devBi16aiss8ao4_init_record(biRecord *precord);
static long devBi16aiss8ao4_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt);
static long devBi16aiss8ao4_read_bi(biRecord *precord);

static long devAo16aiss8ao4_init_record(aoRecord *precord);
static long devAo16aiss8ao4_write_ao(aoRecord *precord);

static long devAi16aiss8ao4_init_record(aiRecord *precord);
static long devAi16aiss8ao4_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
static long devAi16aiss8ao4_read_ai(aiRecord *precord);

static long devWf16aiss8ao4_init_record(waveformRecord *precord);
static long devWf16aiss8ao4_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt);
static long devWf16aiss8ao4_read_wf(waveformRecord *precord);



BINARYDSET devBo16aiss8ao4 = { 6,  NULL,  NULL,   devBo16aiss8ao4_init_record,  NULL,  devBo16aiss8ao4_write_bo,  NULL };
BINARYDSET devBi16aiss8ao4 = { 6,  NULL,  NULL,   devBi16aiss8ao4_init_record,  devBi16aiss8ao4_get_ioint_info,  devBi16aiss8ao4_read_bi,  NULL };
BINARYDSET devAo16aiss8ao4 = { 6,  NULL,  NULL,   devAo16aiss8ao4_init_record,  NULL,  devAo16aiss8ao4_write_ao,  NULL };
BINARYDSET devAi16aiss8ao4 = { 6, NULL, NULL, devAi16aiss8ao4_init_record, devAi16aiss8ao4_get_ioint_info, devAi16aiss8ao4_read_ai, NULL };
BINARYDSET devWf16aiss8ao4 = { 6, NULL, NULL, devWf16aiss8ao4_init_record, devWf16aiss8ao4_get_ioint_info, devWf16aiss8ao4_read_wf, NULL };


epicsExportAddress(dset, devBo16aiss8ao4);
epicsExportAddress(dset, devBi16aiss8ao4);
epicsExportAddress(dset, devAo16aiss8ao4);
epicsExportAddress(dset, devAi16aiss8ao4);
epicsExportAddress(dset, devWf16aiss8ao4);



/*******************************************************/
static void devAo16aiss8ao4_AO_LIMIT_DURATION(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	
	p16aiss8ao4->d32CurrentLimitDuration = pParam->setValue;

	printf("%s: current limit duration: %fsec\n", pSTDdev->taskName, p16aiss8ao4->d32CurrentLimitDuration);
}
static void devAo16aiss8ao4_AO_CMD_CURRENT_LIMIT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	
	p16aiss8ao4->u32CmdCurrentLimit = (unsigned int)pParam->setValue;

	printf("%s: current command limit: %dA\n", pSTDdev->taskName, p16aiss8ao4->u32CmdCurrentLimit);
}
static void devAo16aiss8ao4_AO_CFG_A_VALUE(ST_execParam *pParam)
{
	int ps = pParam->n32Arg0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( (unsigned int)pParam->setValue > 2 ) {
		printf(">>>> ERROR. %s: got wrong value. => %d\n", pSTDdev->taskName, (unsigned int)pParam->setValue );
		return;
	}
	p16aiss8ao4->u32Mode_a[ps] = (unsigned int)pParam->setValue;

	drv16aiss8ao4_update_coilMode(pSTDdev);

	printf("%s: CH %d: mode %d, pcs %d\n", pSTDdev->taskName, ps,  p16aiss8ao4->u32Mode_tag[ps], p16aiss8ao4->u32Mode_PCS[ps]);
}
static void devAo16aiss8ao4_AO_CFG_B_VALUE(ST_execParam *pParam)
{
	int ps = pParam->n32Arg0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( (unsigned int)pParam->setValue > 2 ) {
		printf(">>>> ERROR. %s: got wrong value. => %d\n", pSTDdev->taskName, (unsigned int)pParam->setValue );
		return;
	}
	p16aiss8ao4->u32Mode_b[ps] = (unsigned int)pParam->setValue;
	drv16aiss8ao4_update_coilMode(pSTDdev);
	printf("%s: CH %d: mode %d, pcs %d\n", pSTDdev->taskName, ps,  p16aiss8ao4->u32Mode_tag[ps], p16aiss8ao4->u32Mode_PCS[ps]);
}
static void devAo16aiss8ao4_AO_CFG_C_VALUE(ST_execParam *pParam)
{
	int ps = pParam->n32Arg0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( (unsigned int)pParam->setValue > 2 ) {
		printf(">>>> ERROR. %s: got wrong value. => %d\n", pSTDdev->taskName, (unsigned int)pParam->setValue );
		return;
	}
	p16aiss8ao4->u32Mode_c[ps] = (unsigned int)pParam->setValue;	
	drv16aiss8ao4_update_coilMode(pSTDdev);
	printf("%s: CH %d: mode %d, pcs %d\n", pSTDdev->taskName, ps,  p16aiss8ao4->u32Mode_tag[ps], p16aiss8ao4->u32Mode_PCS[ps]);
}
static void devAo16aiss8ao4_AO_CFG_D_VALUE(ST_execParam *pParam)
{
	int ps = pParam->n32Arg0;
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( (unsigned int)pParam->setValue > 2 ) {
		printf(">>>> ERROR. %s: got wrong value. => %d\n", pSTDdev->taskName, (unsigned int)pParam->setValue );
		return;
	}
	p16aiss8ao4->u32Mode_d[ps] = (unsigned int)pParam->setValue;
	drv16aiss8ao4_update_coilMode(pSTDdev);
	printf("%s: CH %d: mode %d, pcs %d\n", pSTDdev->taskName, ps,  p16aiss8ao4->u32Mode_tag[ps], p16aiss8ao4->u32Mode_PCS[ps]);
}

/*
static void devBo16aiss8ao4_AO_CFG_TAG_VALUE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	p16aiss8ao4->u32Mode_tag[pParam->n32Arg0] = (unsigned int)pParam->setValue;

	printf("%s: mode:  B:%d, M:%d, T:%d\n", pSTDdev->taskName, p16aiss8ao4->u32Mode_tag[ID_PS_BOTTOM], p16aiss8ao4->u32Mode_tag[ID_PS_MIDDLE], p16aiss8ao4->u32Mode_tag[ID_PS_TOP] );
}
*/
static void devAo16aiss8ao4_AO_MAX_CURRENT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	float prev;

	prev = p16aiss8ao4->f32MaxCurrent;

	p16aiss8ao4->f32MaxCurrent = pParam->setValue;
	printf("%s: maximum current changed from %.1f A to %.1f A.\n", pSTDdev->taskName, prev, p16aiss8ao4->f32MaxCurrent);
}
static void devAo16aiss8ao4_AO_FG_CLK_RATE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	unsigned int prev;

	prev = p16aiss8ao4->out_rate_c;

	p16aiss8ao4->out_rate_c = (unsigned int)pParam->setValue;
	dev_rate_gen_c_ndiv(-1, p16aiss8ao4->fd, 0, p16aiss8ao4->out_rate_c, p16aiss8ao4->out_clock_c);
	printf("%s: FG clock rate changed from %dHz to %dHz.\n", pSTDdev->taskName, prev, p16aiss8ao4->out_rate_c);
}
static void devAo16aiss8ao4_AO_AO_WRITE_CH(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	uint32 val = 0x8000;
	int errs = 0;

	/* using offset binary format */
	val = (unsigned int)(((pParam->setValue + 10.0) * 0xffff) / 20.0 );

	switch( pParam->n32Arg0 ) {
		case 0: 
			errs	+= reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_0_REG, val);
			break;
		case 1: 
			errs	+= reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_1_REG, val);
			break;
		case 2: 
			errs	+= reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_2_REG, val);
			break;
		case 3: 
			errs	+= reg_gsc_write(p16aiss8ao4->fd, ANALOG_OUT_3_REG, val);
			break;
		default: 
			errs++; 
			break;
	}
	if( errs > 0 ) {
		printf("%s: ERROR!  %s\n", pSTDdev->taskName, precord->name);
		return;
	}
	printf("%s: AO registered value: %0.4fV(0x%x), ch:%d\n", pSTDdev->taskName, pParam->setValue, val, pParam->n32Arg0 );
	
}
static void devAo16aiss8ao4_AO_AI_PERIOD(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	unsigned int prev;

	prev = p16aiss8ao4->in_period; /* not used replaced to STD run_period */

	p16aiss8ao4->in_period = (unsigned int)pParam->setValue;
	printf("%s: AI period changed from %d to %d.\n", pSTDdev->taskName, prev, p16aiss8ao4->in_period);
}
static void devAo16aiss8ao4_AO_CHANNEL_MASK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
//	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	pSTDdev->channel_mask = (unsigned int)pParam->setValue;

	printf("%s: Channel Mask is 0x%x\n", pSTDdev->taskName, pSTDdev->channel_mask);
}
static void devAo16aiss8ao4_AO_AO_PERIOD(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	unsigned int prev;

	prev = p16aiss8ao4->out_period; /* not used replaced to STD run_period */

	p16aiss8ao4->out_period = (unsigned int)pParam->setValue;
	printf("%s: AO period changed from %d to %d.\n", pSTDdev->taskName, prev, p16aiss8ao4->out_period);
}
static void devAo16aiss8ao4_AO_NUM_OF_SECTION(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	char buf[16];


	p16aiss8ao4->num_section = (int)pParam->setValue;				
//	p16aiss8ao4->out_period = p16aiss8ao4->CH0[p16aiss8ao4->num_section-1].x2;
/* run period value is get from Top coil patern 2013. 6. 27*/
	sprintf(buf, "%f", p16aiss8ao4->CH2[p16aiss8ao4->num_section-1].x2);
	db_put("RMP_RUN_PERIOD", buf);
	db_put("RMP_16ADIO_AO_PERIOD", buf);
	db_put("RMP_16ADIO_AI_PERIOD", buf);

//	scanIoRequest(pSTDdev->ioScanPvt_userCall1);

	epicsPrintf("%s: section count: %d\n", pSTDdev->taskName, p16aiss8ao4->num_section);
}

static void devAo16aiss8ao4_AO_SECTION_END_TIME(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;

	drv16aiss8ao4_set_section_end_time( pSTDdev, pParam->n32Arg0, pParam->n32Arg1, pParam->setValue );

//	scanIoRequest(pSTDdev->ioScanPvt_userCall1);

#if 0
	epicsPrintf(" %s %s Ch:%d, section:%d, %.2fsec  OK!\n", pSTDdev->taskName, precord->name, 
		pParam->n32Arg0, pParam->n32Arg1, pParam->setValue );
#endif
}

static void devAo16aiss8ao4_AO_SECTION_END_VOLT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;

	drv16aiss8ao4_set_section_end_voltage( pSTDdev, pParam->n32Arg0, pParam->n32Arg1, pParam->setValue );
	
#if 0
	epicsPrintf(" %s %s Ch:%d, section:%d, %.2fV  OK!\n", pSTDdev->taskName, precord->name, 
		pParam->n32Arg0, pParam->n32Arg1, pParam->setValue );
#endif
}

static void devAo16aiss8ao4_AO_SECTION_END_CUR(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
//	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;


	if( pParam->setValue > p16aiss8ao4->f32MaxCurrent ) {
		printf("%s: input %dA is overcurrent! changed to %.1f.\n", pSTDdev->taskName, (int)pParam->setValue, p16aiss8ao4->f32MaxCurrent);
		pParam->setValue = p16aiss8ao4->f32MaxCurrent;
	}

	drv16aiss8ao4_set_section_end_current( pSTDdev, pParam->n32Arg0, pParam->n32Arg1, pParam->setValue );

//	scanIoRequest(pSTDdev->ioScanPvt_userCall1);
#if 0
	epicsPrintf(" %s %s Ch:%d, section:%d, %.2fA  OK!\n", pSTDdev->taskName, precord->name, 
		pParam->n32Arg0, pParam->n32Arg1, pParam->setValue );
#endif
}

static void devBo16aiss8ao4_BO_ALL_RDY_ON(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	char buf[4];
	sprintf(buf, "%d", (int)pParam->setValue);
	if( p16aiss8ao4->u8_Link_PS ) 
	{
		db_put("RMP_DIO24_DO_A0:BTM_RDY_ON", buf);
		db_put("RMP_DIO24_DO_B0:MID_RDY_ON", buf);
		db_put("RMP_DIO24_DO_C0:TOP_RDY_ON", buf);
	}

//	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_ALL_RDY_OFF(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	char buf[4];
	sprintf(buf, "%d", (int)pParam->setValue);
	if( p16aiss8ao4->u8_Link_PS ) 
	{
		db_put("RMP_DIO24_DO_A1:BTM_RDY_OFF", buf);
		db_put("RMP_DIO24_DO_B1:MID_RDY_OFF", buf);
		db_put("RMP_DIO24_DO_C1:TOP_RDY_OFF", buf);
	}

//	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}


static void devBo16aiss8ao4_BO_DIRECT_CONTROL(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	p16aiss8ao4->u8_PCS_control = (uint8)pParam->setValue;
	if( p16aiss8ao4->u8_PCS_control )
		db_put("RMP_RUN_PERIOD", "40");

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}

static void devBo16aiss8ao4_BO_IP_MIN_FAULT_LINK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	p16aiss8ao4->u8_ip_fault = (uint8)pParam->setValue;

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_16ADIO_LINK_PS(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
	ST_16aiss8ao4* p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;

	p16aiss8ao4->u8_Link_PS = (uint8)pParam->setValue;

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_GPIO_MODE(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
//	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

	drv16aiss8ao4_set_GPIO_OUT(pSTDdev, pParam->n32Arg0, (int)pParam->setValue);

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_MAKE_PATTERN(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
//	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;

	if( (int)pParam->setValue )
	{
		drv16aiss8ao4_make_pattern_2(pSTDdev);
	}

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_GPIO_OUT(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;
//	ST_MASTER *pMaster = pSTDdev->pST_parentAdmin;


	if ( (int)pParam->setValue  > 1 )
		printf(">>> ERROR wrong value!\n");
	else 
		drv16aiss8ao4_dio_out(pSTDdev, pParam->n32Arg0, (int)pParam->setValue );

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}

static void devBo16aiss8ao4_BO_AnalogInput_START(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;


	if ( (int)pParam->setValue   )
		drv16aiss8ao4_AI_trigger(pSTDdev);
		

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_AI_USE_EXT_CLK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;


	if ( (int)pParam->setValue   )
		epicsPrintf("%s: Use External clock to AI\n", pSTDdev->taskName);
	else
		epicsPrintf("%s: Use Internal clock to AI\n", pSTDdev->taskName);
	
	if( drv16aiss8ao4_set_AI_clock_mode(pSTDdev, (uint8)pParam->setValue) == WR_ERROR )
		return;
	
	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_AO_USE_EXT_CLK(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;


	if ( (int)pParam->setValue   )
		epicsPrintf("%s: Use External clock to AO\n", pSTDdev->taskName);
	else
		epicsPrintf("%s: Use Internal clock to AO\n", pSTDdev->taskName);
	
	if( drv16aiss8ao4_set_AO_clock_mode(pSTDdev, (uint8)pParam->setValue) == WR_ERROR )
		return;
	
	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}

static void devBo16aiss8ao4_BO_AnalogOut_START(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;


	if ( (int)pParam->setValue   )
		drv16aiss8ao4_AO_trigger(pSTDdev);
		

	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}
static void devBo16aiss8ao4_BO_AO_MODE_REGISTERED(ST_execParam *pParam)
{
	ST_STD_device *pSTDdev = pParam->pSTDdev;
	struct dbCommon *precord = pParam->precord;


	if ( (int)pParam->setValue   )
		epicsPrintf("%s: Use registerd output mode\n", pSTDdev->taskName);
	else
		epicsPrintf("%s: Use buffered output mode\n", pSTDdev->taskName);
	
	if( drv16aiss8ao4_set_AO_mode_registered(pSTDdev, (uint8)pParam->setValue) == WR_ERROR )
		return;
	
	epicsPrintf(" %s %s (%d)  OK!\n", pSTDdev->taskName, precord->name, (int)pParam->setValue );
}

static long devBo16aiss8ao4_init_record(boRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %d", pDpvt->devName, pDpvt->arg0, &pDpvt->n32Arg0);
#if 0
	epicsPrintf("devBoACQ196Control arg num: %d: %s %s\n",i, devBo16aiss8ao4->devName, devBo16aiss8ao4->arg0);
#endif
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,  "devBo16aiss8ao4 (init_record) Illegal INP field: task_name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBo16aiss8ao4 (init_record) Illegal OUT field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}


	if(!strcmp(pDpvt->arg0, BO_GPIO_OUT_STR)) {
		pDpvt->ind = BO_GPIO_OUT;
/*		pDpvt->n32Arg0 = strtol(pDpvt->arg1,NULL,0); */
	} 
	else if (!strcmp(pDpvt->arg0, BO_AO_START_STR)) {
		pDpvt->ind = BO_AO_START;
	} 
	else if (!strcmp(pDpvt->arg0, BO_AI_START_STR)) {
		pDpvt->ind = BO_AI_START;
	} 
	else if (!strcmp(pDpvt->arg0, BO_AI_USE_EXT_CLK_STR)) {
		pDpvt->ind = BO_AI_USE_EXT_CLK;
	} 
	else if (!strcmp(pDpvt->arg0, BO_AO_MODE_REGISTERED_STR)) {
		pDpvt->ind = BO_AO_MODE_REGISTERED;
	} 
	else if (!strcmp(pDpvt->arg0, BO_AO_USE_EXT_CLK_STR)) {
		pDpvt->ind = BO_AO_USE_EXT_CLK;
	}
	else if (!strcmp(pDpvt->arg0, BO_MAKE_PATTERN_STR)) {
		pDpvt->ind = BO_MAKE_PATTERN;
	}
	else if (!strcmp(pDpvt->arg0, BO_GPIO_MODE_STR)) {
		pDpvt->ind = BO_GPIO_MODE;
	}
	else if (!strcmp(pDpvt->arg0, BO_16ADIO_LINK_PS_STR)) {
		pDpvt->ind = BO_16ADIO_LINK_PS;
	}
	else if (!strcmp(pDpvt->arg0, BO_IP_MIN_FAULT_LINK_STR)) {
		pDpvt->ind = BO_IP_MIN_FAULT_LINK;
	}
	else if (!strcmp(pDpvt->arg0, BO_DIRECT_CONTROL_STR)) {
		pDpvt->ind = BO_DIRECT_CONTROL;
	}
	else if (!strcmp(pDpvt->arg0, BO_ALL_RDY_ON_STR)) {
		pDpvt->ind = BO_ALL_RDY_ON;
	}
	else if (!strcmp(pDpvt->arg0, BO_ALL_RDY_OFF_STR)) {
		pDpvt->ind = BO_ALL_RDY_OFF;
	}
	
	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR! devBo16aiss8ao4_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}


static long devBo16aiss8ao4_write_bo(boRecord *precord)
{
#if 0
	static int   kkh_cnt;
#endif
	ST_dpvt		*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_threadCfg 	*pControlThreadConfig;
	ST_threadQueueData	qData;



	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;
	qData.param.n32Arg0		= pDpvt->n32Arg0;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if 0
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) {

			case BO_GPIO_OUT:
				qData.pFunc = devBo16aiss8ao4_BO_GPIO_OUT;
				break;
			case BO_AO_START:
				qData.pFunc = devBo16aiss8ao4_BO_AnalogOut_START;
				break;
			case BO_AI_START:
				qData.pFunc = devBo16aiss8ao4_BO_AnalogInput_START;
				break;
			case BO_AI_USE_EXT_CLK:
				qData.pFunc = devBo16aiss8ao4_BO_AI_USE_EXT_CLK;
				break;
			case BO_AO_MODE_REGISTERED:
				qData.pFunc = devBo16aiss8ao4_BO_AO_MODE_REGISTERED;
				break;
			case BO_AO_USE_EXT_CLK:
				qData.pFunc = devBo16aiss8ao4_BO_AO_USE_EXT_CLK;
				break;
			case BO_MAKE_PATTERN:
				qData.pFunc = devBo16aiss8ao4_BO_MAKE_PATTERN;
				break;
			case BO_GPIO_MODE:
				qData.pFunc = devBo16aiss8ao4_BO_GPIO_MODE;
				break;
			case BO_16ADIO_LINK_PS:
				qData.pFunc = devBo16aiss8ao4_BO_16ADIO_LINK_PS;
				break;
			case BO_IP_MIN_FAULT_LINK:
				qData.pFunc = devBo16aiss8ao4_BO_IP_MIN_FAULT_LINK;
				break;
			case BO_DIRECT_CONTROL:
				qData.pFunc = devBo16aiss8ao4_BO_DIRECT_CONTROL;
				break;
			case BO_ALL_RDY_ON:
				qData.pFunc = devBo16aiss8ao4_BO_ALL_RDY_ON;
				break;
			case BO_ALL_RDY_OFF:
				qData.pFunc = devBo16aiss8ao4_BO_ALL_RDY_OFF;
				break;

			default: 
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(ST_threadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if 0
		epicsPrintf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
#if 0
		switch(pdevACQ196Controldpvt->ind) {
			case AO_SET_INTERCLK: precord->val = 0; break;
			case AO_SET_EXTERCLK: precord->val = 0; break;
			case AO_DEV_RUN: precord->val = 0; break;
			case AO_DAQ_STOP: precord->val = 0; break;
			case AO_DEV_ARMING: precord->val = 0; break;
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

static long devAo16aiss8ao4_init_record(aoRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];


	switch(precord->out.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->out.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);
#if 0
			printf("devAo arg num: %d: %s %s %s\n", nval,  arg0, arg1, arg2);
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


	if (!strcmp(arg0, AO_NUM_OF_SECTION_STR))
		pDpvt->ind = AO_NUM_OF_SECTION;

	else if (!strcmp(arg0, AO_SECTION_END_VOLT_STR)) {
		pDpvt->ind = AO_SECTION_END_VOLT;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}
	else if (!strcmp(arg0, AO_SECTION_END_CUR_STR)) {
		pDpvt->ind = AO_SECTION_END_CUR;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}
	else if (!strcmp(arg0, AO_SECTION_END_TIME_STR)) {
		pDpvt->ind = AO_SECTION_END_TIME;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
		pDpvt->n32Arg1 = strtoul(arg2, NULL, 0); /* section id */
	}
	else if (!strcmp(arg0, AO_AI_PERIOD_STR)) {
		pDpvt->ind = AO_AI_PERIOD;
	}
	else if (!strcmp(arg0, AO_AO_WRITE_CH_STR)) {
		pDpvt->ind = AO_AO_WRITE_CH;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
	}
	else if (!strcmp(arg0, AO_FG_CLK_RATE_STR)) {
		pDpvt->ind = AO_FG_CLK_RATE;
	}
	else if (!strcmp(arg0, AO_AO_PERIOD_STR)) {
		pDpvt->ind = AO_AO_PERIOD;
	}
	else if (!strcmp(arg0, AO_CHANNEL_MASK_STR)) {
		pDpvt->ind = AO_CHANNEL_MASK;
	}
	else if (!strcmp(arg0, AO_MAX_CURRENT_STR)) {
		pDpvt->ind = AO_MAX_CURRENT;
	}
/*	else if (!strcmp(arg0, AO_CFG_TAG_VALUE_STR)) {
		pDpvt->ind = AO_CFG_TAG_VALUE;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); 
	} */
	else if (!strcmp(arg0, AO_CFG_A_VALUE_STR)) {
		pDpvt->ind = AO_CFG_A_VALUE;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* b, m, t ( 0, 1, 2) */
	}
	else if (!strcmp(arg0, AO_CFG_B_VALUE_STR)) {
		pDpvt->ind = AO_CFG_B_VALUE;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* b, m, t ( 0, 1, 2) */
	}
	else if (!strcmp(arg0, AO_CFG_C_VALUE_STR)) {
		pDpvt->ind = AO_CFG_C_VALUE;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* b, m, t ( 0, 1, 2) */
	}
	else if (!strcmp(arg0, AO_CFG_D_VALUE_STR)) {
		pDpvt->ind = AO_CFG_D_VALUE;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* b, m, t ( 0, 1, 2) */
	}
	else if (!strcmp(arg0, AO_CMD_CURRENT_LIMIT_STR)) {
		pDpvt->ind = AO_CMD_CURRENT_LIMIT;
	}
	else if (!strcmp(arg0, AO_LIMIT_DURATION_STR)) {
		pDpvt->ind = AO_LIMIT_DURATION;
	}
	
	
	
	

	


	else {
		pDpvt->ind = -1;
		epicsPrintf("ERROR_16aiss8ao4! devAo16aiss8ao4_init_record: arg0 \"%s\" \n",  arg0 );
		
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

static long devAo16aiss8ao4_write_ao(aoRecord *precord)
{
	ST_dpvt        *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;
	ST_16aiss8ao4 *p16aiss8ao4;
	ST_threadCfg *pControlThreadConfig;
	ST_threadQueueData         qData;
	double curr_V = 0.0;

	if(!pDpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pSTDdev			= pDpvt->pSTDdev;
	pControlThreadConfig		= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;

	p16aiss8ao4 = (ST_16aiss8ao4*)pSTDdev->pUser;

        /* db processing: phase I */
 	if(precord->pact == FALSE) 
	{	
		precord->pact = TRUE;
#if 0
		epicsPrintf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_NUM_OF_SECTION:
				qData.pFunc = devAo16aiss8ao4_AO_NUM_OF_SECTION;
				break;
			case AO_SECTION_END_VOLT:
				qData.pFunc = devAo16aiss8ao4_AO_SECTION_END_VOLT;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				qData.param.n32Arg1 = pDpvt->n32Arg1; /* section id */
				break;
			case AO_SECTION_END_CUR:
				qData.pFunc = devAo16aiss8ao4_AO_SECTION_END_CUR;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				qData.param.n32Arg1 = pDpvt->n32Arg1; /* section id */
				break;
			case AO_SECTION_END_TIME:
				qData.pFunc = devAo16aiss8ao4_AO_SECTION_END_TIME;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				qData.param.n32Arg1 = pDpvt->n32Arg1; /* section id */
				break;
			case AO_AI_PERIOD:
				qData.pFunc = devAo16aiss8ao4_AO_AI_PERIOD;
				break;
			case AO_AO_WRITE_CH:
				qData.pFunc = devAo16aiss8ao4_AO_AO_WRITE_CH;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				break;
			case AO_FG_CLK_RATE:
				qData.pFunc = devAo16aiss8ao4_AO_FG_CLK_RATE;
				break;
			case AO_AO_PERIOD:
				qData.pFunc = devAo16aiss8ao4_AO_AO_PERIOD;
				break;
			case AO_CHANNEL_MASK:
				qData.pFunc = devAo16aiss8ao4_AO_CHANNEL_MASK;
				break;
			case AO_MAX_CURRENT:
				qData.pFunc = devAo16aiss8ao4_AO_MAX_CURRENT;
				break;
/*			case AO_CFG_TAG_VALUE:
				qData.pFunc = devBo16aiss8ao4_AO_CFG_TAG_VALUE;
				qData.param.n32Arg0 = pDpvt->n32Arg0; 
				break; */
			case AO_CFG_A_VALUE:
				qData.pFunc = devAo16aiss8ao4_AO_CFG_A_VALUE;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				break;
			case AO_CFG_B_VALUE:
				qData.pFunc = devAo16aiss8ao4_AO_CFG_B_VALUE;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				break;
			case AO_CFG_C_VALUE:
				qData.pFunc = devAo16aiss8ao4_AO_CFG_C_VALUE;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				break;
			case AO_CFG_D_VALUE:
				qData.pFunc = devAo16aiss8ao4_AO_CFG_D_VALUE;
				qData.param.n32Arg0 = pDpvt->n32Arg0; /* channel id */
				break;
			case AO_CMD_CURRENT_LIMIT:
				qData.pFunc = devAo16aiss8ao4_AO_CMD_CURRENT_LIMIT;
				break;
			case AO_LIMIT_DURATION:
				qData.pFunc = devAo16aiss8ao4_AO_LIMIT_DURATION;
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
	if(precord->pact == TRUE) 
	{
#if 0
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif
		switch(pDpvt->ind) 
		{
			case AO_SECTION_END_CUR: 
				if( pDpvt->n32Arg0 == 0)
					curr_V = p16aiss8ao4->CH0[ pDpvt->n32Arg1].y2;
				else if ( pDpvt->n32Arg0 == 1)
					curr_V = p16aiss8ao4->CH1[ pDpvt->n32Arg1].y2;
				else if ( pDpvt->n32Arg0 == 2)
					curr_V = p16aiss8ao4->CH2[ pDpvt->n32Arg1].y2;
				else if ( pDpvt->n32Arg0 == 3)
					curr_V = p16aiss8ao4->CH3[ pDpvt->n32Arg1].y2;
				else {
					printf("%s:ERROR....set end current input channel (%d) wrong!!!\n", pSTDdev->taskName, pDpvt->n32Arg0 );
					return -1;
					}
				precord->val = curr_V * p16aiss8ao4->f32MaxCurrent / 10.0 ;  /* 3KA * volt / 10V */
				break;
				
			default:
				break;
		}

		precord->pact = FALSE;
		precord->udf = FALSE;
		return 0; /*(0)=>(success ) */
	}

	return -1;
}

static long devAi16aiss8ao4_init_record(aiRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char arg2[SIZE_DPVT_ARG];
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s %s", pDpvt->devName, arg0, arg1, arg2);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAi16aiss8ao4 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAi16aiss8ao4 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(arg0, AI_16ADIO_GET_GPIO_STR))
		pDpvt->ind = AI_16ADIO_GET_GPIO;

	else if(!strcmp(arg0, AI_16ADIO_CUR_AI_VAL_STR)) {
		pDpvt->ind = AI_16ADIO_CUR_AI_VAL;
		pDpvt->n32Arg0 = strtoul(arg1, NULL, 0); /* chennel id */
	}

	else {
		pDpvt->ind = -1;
		epicsPrintf("_16aiss8ao4 devAi16aiss8ao4_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}


static long devAi16aiss8ao4_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devAi16aiss8ao4_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_16aiss8ao4 *p16aiss8ao4;
//	uint32 status;
	uint32 data=0;
	epicsUInt16 prev_d;

//	unsigned long get = 0;

	if(!pDpvt) return -1; /*(0,2)=> success and convert,don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	p16aiss8ao4 = (ST_16aiss8ao4*)pSTDdev->pUser;

	switch(pDpvt->ind) 
	{
		case AI_16ADIO_GET_GPIO: 
			prev_d = p16aiss8ao4->u16GPIO;
			if( p16aiss8ao4->GPIO_0_IO == 0 ) 
			{
				if( gsc_dio_0_in(p16aiss8ao4->fd, &data) == WR_ERROR )
					p16aiss8ao4->u16GPIO |= 0xff;
				else {					
					p16aiss8ao4->u16GPIO = ( prev_d & 0xff00 )  | data;
//					printf("%s: GPIO_0: 0x%x\n",pSTDdev->taskName, (uint32)precord->val );
				}
			}
			prev_d = p16aiss8ao4->u16GPIO;
			if( p16aiss8ao4->GPIO_1_IO == 0 ) 
			{
				if( gsc_dio_1_in(p16aiss8ao4->fd, &data) == WR_ERROR )
					p16aiss8ao4->u16GPIO |= 0xff00;
				else {
					p16aiss8ao4->u16GPIO = ( prev_d & 0xff )  | (data << 8);
//					printf("%s: GPIO_0: 0x%x\n",pSTDdev->taskName, (uint32)precord->val );
				}
			}

			precord->val = p16aiss8ao4->u16GPIO;
//			printf("%s: GPIO: 0x%x\n",pSTDdev->taskName, (uint32)precord->val );
			break;

		case AI_16ADIO_CUR_AI_VAL:
			precord->val = p16aiss8ao4->d32CurValue[pDpvt->n32Arg0];

			break;
			
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... _16aiss8ao4 \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,don't convert)*/
}



static long devBi16aiss8ao4_init_record(biRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	
	switch(precord->inp.type) 
	{
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pDpvt->devName,
				   pDpvt->arg0,
				   pDpvt->arg1);

			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devBi16aiss8ao4 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
			
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devBi16aiss8ao4 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pDpvt->arg0, BI_GPIO_IN_STR)) {
		pDpvt->ind = BI_GPIO_IN;
		pDpvt->n32Arg0 = strtoul(pDpvt->arg1, NULL, 0); /* chennel id */	
	}
	else if(!strcmp(pDpvt->arg0, "xxx"))
		pDpvt->ind = 99;

	else {
		pDpvt->ind = -1;
		epicsPrintf("_16aiss8ao4 devBi16aiss8ao4_init_record: arg0 \"%s\" \n",  pDpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
}

static long devBi16aiss8ao4_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall;
	return 0;
}

static long devBi16aiss8ao4_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_16aiss8ao4 *p16aiss8ao4;
//	uint32 status;
	uint32 nCh=0;
//	unsigned long get = 0;

	if(!pDpvt) return -1; /*(0,2)=> success and convert, don't convert)*/

	pSTDdev = pDpvt->pSTDdev;

	p16aiss8ao4 = (ST_16aiss8ao4*)pSTDdev->pUser;

	nCh = pDpvt->n32Arg0; /* channel ID */

	switch(pDpvt->ind) 
	{
		case BI_GPIO_IN: 
			if( ( (p16aiss8ao4->GPIO_0_IO == 0) && (nCh < 8) ) || 
				( (p16aiss8ao4->GPIO_1_IO == 0) && (7 < nCh ) ) ) 
			{
				precord->val = (p16aiss8ao4->u16GPIO & (0x1 << nCh ) ) ? 1 : 0 ;

			} 
			else
				precord->val = -1;

//			printf("%s: GPIO 0x%x, IN CH%d, %d\n", pSTDdev->taskName,p16aiss8ao4->u16GPIO,  nCh, (int)precord->val);
			break;
		
		default:
			epicsPrintf("\nERROR: %s: ind( %d )... _16aiss8ao4 \n", pSTDdev->taskName, pDpvt->ind); 
			break;
	}
	return (2); /*(0,2)=> success and convert,   don't convert)*/
}

/****************************************************************
Waveform Record type
*****************************************************************/
static long devWf16aiss8ao4_init_record(waveformRecord *precord)
{
	ST_dpvt *pDpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int nval;
	
	switch(precord->inp.type) {		
		case CONSTANT:
			precord->nord = 0;
			break;			
		case INST_IO:
			strcpy(pDpvt->recordName, precord->name);
			nval = sscanf(precord->inp.value.instio.string, "%s %s %s", pDpvt->devName, pDpvt->arg0, pDpvt->arg1);
			
			pDpvt->pSTDdev = get_STDev_from_name(pDpvt->devName);
			if(!pDpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devWf16aiss8ao4 (init_record) Illegal INP field: task name");
				free(pDpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		case PV_LINK:
		case DB_LINK:
		case CA_LINK:
			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devWf16aiss8ao4 (init_record) Illegal INP field");
			free(pDpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	
	if(!strcmp(pDpvt->arg0, WF_READ_VALUE_STR)) {
		pDpvt->ind = WF_READ_VALUE;
		pDpvt->n32Arg0 = strtoul(pDpvt->arg1, NULL, 0); /* chennel id */	
		precord->nelm = MAX_WAVEFORM_NODE_CNT;
		precord->ftvl = DBF_FLOAT;      /* DBF_DOUBLE  has truncated effect  ... */	
/*		precord->ftvl = DBF_USHORT; DBF_LONG  DBF_FLOAT */
	}
	else if(!strcmp(pDpvt->arg0, WF_PTN_B_X_STR)) {
		pDpvt->ind = WF_PTN_B_X;
		precord->nelm = MAX_NUM_SECTION;
		precord->ftvl = DBF_FLOAT;
	}
	else if(!strcmp(pDpvt->arg0, WF_PTN_B_Y_STR)) {
		pDpvt->ind = WF_PTN_B_Y;
		precord->nelm = MAX_NUM_SECTION;
		precord->ftvl = DBF_FLOAT;
	}
	else if(!strcmp(pDpvt->arg0, WF_PTN_M_X_STR)) {
		pDpvt->ind = WF_PTN_M_X;
		precord->nelm = MAX_NUM_SECTION;
		precord->ftvl = DBF_FLOAT;
	}
	else if(!strcmp(pDpvt->arg0, WF_PTN_M_Y_STR)) {
		pDpvt->ind = WF_PTN_M_Y;
		precord->nelm = MAX_NUM_SECTION;
		precord->ftvl = DBF_FLOAT;
	}
	else if(!strcmp(pDpvt->arg0, WF_PTN_T_X_STR)) {
		pDpvt->ind = WF_PTN_T_X;
		precord->nelm = MAX_NUM_SECTION;
		precord->ftvl = DBF_FLOAT;
	}
	else if(!strcmp(pDpvt->arg0, WF_PTN_T_Y_STR)) {
		pDpvt->ind = WF_PTN_T_Y;
		precord->nelm = MAX_NUM_SECTION;
		precord->ftvl = DBF_FLOAT;
	}

	else {
		pDpvt->ind = -1;
		epicsPrintf("_16aiss8ao4 devBi16aiss8ao4_init_record: arg0 \"%s\" \n",	pDpvt->arg0 );
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pDpvt;

	return 0;    /*returns: (-1,0)=>(failure,success)*/
	
}

static long devWf16aiss8ao4_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device		*pSTDdev;

	if(!pDpvt) {
		ioScanPvt = NULL;
		return 0;
	}
	pSTDdev = pDpvt->pSTDdev;
	*ioScanPvt  = pSTDdev->ioScanPvt_userCall1;
	return 0;
}

static long devWf16aiss8ao4_read_wf(waveformRecord *precord)
{ 
	ST_dpvt	*pDpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_16aiss8ao4 *p16aiss8ao4;
	long nRequest;
	uint32 nCh=0;
	


	if(!pDpvt) return -1; /*returns: (-1,0)=>(failure,success)*/

	pSTDdev = pDpvt->pSTDdev;

	p16aiss8ao4 = (ST_16aiss8ao4*)pSTDdev->pUser;

	nCh = pDpvt->n32Arg0; /* channel ID */

/*
	Waveformdpvt	*pwfdpvt = (Waveformdpvt*) pwf->dpvt;
	long nRequest;
	
	if(!pwfdpvt) return 0;
*/
	switch(pDpvt->ind) {
		case WF_READ_VALUE:			
//			nRequest = precord->nord;
			precord->nord = 50;
			precord->nelm = MAX_WAVEFORM_NODE_CNT;


//			if( nRequest != MAX_WAVEFORM_NODE_CNT ) {
//				epicsPrintf("Input counter not matching to Kernel size!\n");
//				return 0;
//			}
			drv16aiss8ao4_copy_to_waveform(pSTDdev, nCh, precord );
			break;
		case WF_PTN_B_X:
//			precord->nord = p16aiss8ao4->num_section;
//			precord->nelm = p16aiss8ao4->num_section;
//			drv16aiss8ao4_copy_to_wf_pattern_x(pSTDdev, ID_PS_BOTTOM, precord );
			break;
		case WF_PTN_B_Y:
//			precord->nord = p16aiss8ao4->num_section;
//			precord->nelm = p16aiss8ao4->num_section;
//			drv16aiss8ao4_copy_to_wf_pattern_y(pSTDdev, ID_PS_BOTTOM, precord );
			break;
		case WF_PTN_M_X:
//			precord->nord = p16aiss8ao4->num_section;
//			precord->nelm = p16aiss8ao4->num_section;
//			drv16aiss8ao4_copy_to_wf_pattern_x(pSTDdev, ID_PS_MIDDLE, precord );
			break;
		case WF_PTN_M_Y:
//			precord->nord = p16aiss8ao4->num_section;
//			precord->nelm = p16aiss8ao4->num_section;
//			drv16aiss8ao4_copy_to_wf_pattern_y(pSTDdev, ID_PS_MIDDLE, precord );
			break;
		case WF_PTN_T_X:
//			precord->nord = p16aiss8ao4->num_section;
//			precord->nelm = p16aiss8ao4->num_section;
//			drv16aiss8ao4_copy_to_wf_pattern_x(pSTDdev, ID_PS_TOP, precord );
			break;
		case WF_PTN_T_Y:
//			precord->nord = p16aiss8ao4->num_section;
//			precord->nelm = p16aiss8ao4->num_section;
//			drv16aiss8ao4_copy_to_wf_pattern_y(pSTDdev, ID_PS_TOP, precord );
			break;
		default: 
			epicsPrintf("\n>>> Wrong pDpvt->ind!\n");
			precord->udf = TRUE;
			return -1;
	}
	return (0); /*returns: (-1,0)=>(failure,success)*/
}





