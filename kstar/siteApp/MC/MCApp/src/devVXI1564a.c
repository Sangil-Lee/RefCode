
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

#include "epicsExport.h"

#include "drvVXI1564a.h"


#define  READ_SAMPLING_RATE		(0x00000001<<0)
#define  READ_SAMPLE_COUNT  		(0x00000001<<1)
#define  READ_SAMPLE_PRECOUNT  		(0x00000001<<2)
#define  READ_SAMPLE_MODE     		(0x00000001<<3)
#define  READ_TRIGGER_TYPE	     	(0x00000001<<4)
#define  READ_VAL			(0x00000001<<5)
#define  READ_WAVEFORM		     	(0x00000001<<6)
#define  READ_STATUS		     	(0x00000001<<7)
#define  READ_OPER_MODE		     	(0x00000001<<8)

#define  READ_SAMPLING_RATE_STR     	"rateRead"
#define  READ_SAMPLE_COUNT_STR      	"dataCount"
#define  READ_SAMPLE_PRECOUNT_STR      	"dataPrecount"
#define  READ_SAMPLE_MODE_STR       	"sammode"
#define  READ_TRIGGER_TYPE_STR      	"trigger"
#define  READ_VAL_STR               	"val"
#define  READ_WAVEFORM_STR	    	"waveform"			
#define  READ_STATUS_STR	    	"taskStatus"			
#define  READ_OPER_MODE_STR	    	"operMode"			

#define  CONTROL_SAMPLE_PERIOD     	(0x00000001<<0)
#define  CONTROL_SAMPLE_COUNT      	(0x00000001<<1)
#define  CONTROL_SAMPLE_PRECOUNT      	(0x00000001<<2)
#define  CONTROL_SAMPLE_MODE       	(0x00000001<<3)
#define  CONTROL_TRIGGER_TYPE      	(0x00000001<<4)
#define  CONTROL_SHOT_NUMBER       	(0x00000001<<5)
#define  CONTROL_BLIP_TIME        	(0x00000001<<6)
#define  CONTROL_DAQ_TIME          	(0x00000001<<7)

#define  CONTROL_TASK_TRIG         	(0x00000001<<8)
#define  CONTROL_TASK_CALL         	(0x00000001<<9)
#define  CONTROL_TASK_START        	(0x00000001<<10)
#define  CONTROL_TASK_STOP         	(0x00000001<<11)

#define  CONTROL_TREE_SERV         	(0x00000001<<12)
#define  CONTROL_TREE_NAME         	(0x00000001<<13)
#define  CONTROL_TREE_NODE         	(0x00000001<<14)
#define  CONTROL_CREATE_TREE       	(0x00000001<<15)
#define  CONTROL_MDS_DATA_PUT      	(0x00000001<<16)
#define  CONTROL_OPER_MODE         	(0x00000001<<17)
#define  CONTROL_MDS_CONNECT         	(0x00000001<<18)
#define  CONTROL_MDS_DISCONNECT        	(0x00000001<<19)

#define  CONTROL_RANGE          	(0x00000001<<20)
#define  CONTROL_FILTER          	(0x00000001<<21)
#define  CONTROL_TASK_ABORT         	(0x00000001<<22)
#define  CONTROL_TASK_REG_UPDATE       	(0x00000001<<23)

#define  CONTROL_SAMPLE_PERIOD_STR     	"smpl_period"
#define  CONTROL_SAMPLE_COUNT_STR   	"smpl_count"
#define  CONTROL_SAMPLE_PRECOUNT_STR   	"smpl_precount"
#define  CONTROL_SAMPLE_MODE_STR     	"smpl_mode"
#define  CONTROL_SHOT_NUMBER_STR    	"shot_number" 
#define  CONTROL_BLIP_TIME_STR       	"blip_time"
#define  CONTROL_DAQ_TIME_STR        	"daq_time"

#define  CONTROL_TRIGGER_TYPE_STR    	"trig_type"
#define  CONTROL_TASK_TRIG_STR       	"task_trigger"
#define  CONTROL_OPER_MODE_STR       	"operation_mode"  /* 0 -local , 1- remote  */
#define  CONTROL_TASK_CALL_STR       	"task_call"
#define  CONTROL_TASK_START_STR      	"task_start"
#define  CONTROL_TASK_STOP_STR       	"task_stop"
#define  CONTROL_TASK_ABORT_STR       	"task_abort"
#define  CONTROL_TASK_REG_UPDATE_STR   	"task_regupdate"

#define  CONTROL_CREATE_TREE_STR     	"create_tree"   /* Only Local (Test) Mode Action */
#define  CONTROL_MDS_CONNECT_STR        "mds_conOpen" 
#define  CONTROL_MDS_DISCONNECT_STR     "mds_disClose"

#define  CONTROL_TREE_SERV_STR       	"tree_server"
#define  CONTROL_TREE_NAME_STR       	"tree_name"
#define  CONTROL_TREE_NODE_STR       	"tree_node"

#define  CONTROL_RANGE_STR       	"ch_range"	
#define  CONTROL_FILTER_STR        	"ch_filter"

#define  MASK(k)	(0x01<<(k))


LOCAL void devVXI1564a_set_taskExTrigger(execParam *pParam);
LOCAL void devVXI1564a_set_operMode(execParam *pParam);
LOCAL void devVXI1564a_set_taskCallback(execParam *pParam);
LOCAL void devVXI1564a_set_startTask(execParam *pParam);
LOCAL void devVXI1564a_set_stopTask(execParam *pParam);
LOCAL void devVXI1564a_set_abortTask(execParam *pParam);
LOCAL void devVXI1564a_set_regUpdateTask(execParam *pParam);
LOCAL void devVXI1564a_set_createTree(execParam *pParam);
LOCAL void devVXI1564a_set_mdsconnectOpen(execParam *pParam);
LOCAL void devVXI1564a_set_mdsdisconnectClose(execParam *pParam);
#if 0
LOCAL void devVXI1564a_set_samplingRate(execParam *pParam);
#endif
LOCAL void devVXI1564a_set_samplePeriod(execParam *pParam);
LOCAL void devVXI1564a_set_sampleCount(execParam *pParam);
LOCAL void devVXI1564a_set_samplePreCount(execParam *pParam);
LOCAL void devVXI1564a_set_sampleMode(execParam *pParam);
LOCAL void devVXI1564a_set_triggerType(execParam *pParam);
LOCAL void devVXI1564a_set_shotNumber(execParam *pParam);
LOCAL void devVXI1564a_set_blipTime(execParam *pParam);
LOCAL void devVXI1564a_set_daqTime(execParam *pParam);

LOCAL void devVXI1564a_set_channelRange(execParam *pParam);
LOCAL void devVXI1564a_set_channelFilter(execParam *pParam);
#if 1
LOCAL void devVXI1564a_set_treeServer(execParam *pParam);
LOCAL void devVXI1564a_set_treeName(execParam *pParam);
LOCAL void devVXI1564a_set_treeNode(execParam *pParam);
#endif 

typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];
	char recordName[80];

	drvVXI1564a_taskConfig *ptaskConfig;
	drvVXI1564a_channelConfig *pchannelConfig;

	unsigned        ind;

} devAoVXI1564aControldpvt;

typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];
	char recordName[80];

	drvVXI1564a_taskConfig *ptaskConfig;
	drvVXI1564a_channelConfig *pchannelConfig;
/*	drvVXI1564a_wfChannelConfig *pwfChannelConfig;   */

	unsigned        ind;

} devStringoutVXI1564aControldpvt;

typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];
	char recordName[80];

	drvVXI1564a_taskConfig *ptaskConfig;
	drvVXI1564a_channelConfig *pchannelConfig;

	unsigned	ind;
} devAiVXI1564aRawReaddpvt;


/* Modify 20070511  
typedef struct {
	char taskName[60];
	char chanName[60];
	char option[60];
	char posNum[10];
	char recordName[80];

	drvVXI1564a_taskConfig *ptaskConfig;
	drvVXI1564a_channelConfig *pchannelConfig;
	unsigned	ind;
} devBiVXI1564aControldpvt;
*/
typedef struct {
	char taskName[60];
	char chanName[60];
	char option[60];
	char posNum[10];
	char recordType[20];
	char recordName[80];

	drvVXI1564a_taskConfig *ptaskConfig;
	drvVXI1564a_channelConfig *pchannelConfig;
	unsigned	ind;
} devBoVXI1564aControldpvt;

typedef struct {
    	char taskName[60];
    	char chanName[60];
    	char option[60];
	char posNum[10];
    	char recordType[20];
    	char recordName[80];

    	drvVXI1564a_taskConfig *ptaskConfig;
    	drvVXI1564a_channelConfig *pchannelConfig;
    	unsigned    ind;
} devMbboVXI1564aControldpvt;

typedef struct {
    	char taskName[60];
    	char option1[60];
    	char option2[60];
    	char recordName[80];

    	drvVXI1564a_taskConfig *ptaskConfig;
    	drvVXI1564a_channelConfig *pchannelConfig;
    	unsigned    ind;
} devMbbiVXI1564aRawReaddpvt;

/*
typedef struct {
	char taskName[60];
	char chanName[60];
	char chanIndex[60];
	char recordName[80];
	char option[80];

	drvVXI1564a_taskConfig *ptaskConfig;
	drvVXI1564a_channelConfig *pchannelConfig;
	drvVXI1564a_wfChannelConfig *pwfChannelConfig;
	unsigned	ind;
} devWfVXI1564aControldpvt;
*/


/* Modify 20070511 
LOCAL long devBiVXI1564aControl_init_record(biRecord *precord);
LOCAL long devBiVXI1564aControl_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioSccanPvt);
LOCAL long devBiVXI1564aControl_read_bi(biRecord *precord);      
*/
LOCAL long devAoVXI1564aControl_init_record(aoRecord *precord);
LOCAL long devAoVXI1564aControl_write_ao(aoRecord *precord);

LOCAL long devAiVXI1564aRawRead_init_record(aiRecord *precord);
LOCAL long devAiVXI1564aRawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiVXI1564aRawRead_read_ai(aiRecord *precord);


LOCAL long devStringoutVXI1564aControl_init_record(stringoutRecord *precord);
LOCAL long devStringoutVXI1564aControl_write_stringout(stringoutRecord *precord);

LOCAL long devBoVXI1564aControl_init_record(boRecord *precord);
LOCAL long devBoVXI1564aControl_write_bo(boRecord *precord);

LOCAL long devMbboVXI1564aControl_init_record(mbboRecord *precord);
LOCAL long devMbboVXI1564aControl_write_mbbo(mbboRecord *precord);

LOCAL long devMbbiVXI1564aRawRead_init_record(mbbiRecord *precord);
LOCAL long devMbbiVXI1564aRawRead_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devMbbiVXI1564aRawRead_read_mbbi(mbbiRecord *precord);
/*
LOCAL long devWaveVXI1564aMcData_init_record(waveformRecord *precord);
LOCAL long devWaveVXI1564aMcData_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devWaveVXI1564aMcData_read_wave(waveformRecord *precord);
*/
/*
Below struct is Device support entry table 
*/

struct {
        long            number;
        DEVSUPFUN       report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record;
        DEVSUPFUN       get_ioinit_info;
        DEVSUPFUN       write_bo;
        DEVSUPFUN       special_linconv;
} devBoVXI1564aControl = {
        6,
        NULL,
        NULL,
        devBoVXI1564aControl_init_record,
        NULL,
        devBoVXI1564aControl_write_bo,
        NULL
};

epicsExportAddress(dset, devBoVXI1564aControl);

/* Modify 20070511 
struct {
        long            number;
        DEVSUPFUN       report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record;
        DEVSUPFUN       get_ioinit_info;
        DEVSUPFUN       read_bi;
        DEVSUPFUN       special_linconv;
} devBiVXI1564aControl = {
        6,
        NULL,
        NULL,
        devBiVXI1564aControl_init_record,
        devBiVXI1564aControl_get_ioint_info,
        devBiVXI1564aControl_read_bi,
        NULL
};

epicsExportAddress(dset, devBiVXI1564aControl);
*/
struct {
        long            number;
        DEVSUPFUN       report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record;
        DEVSUPFUN       get_ioinit_info;
        DEVSUPFUN       write_mbbo;
        DEVSUPFUN       special_linconv;
} devMbboVXI1564aControl = {
        6,
        NULL,
        NULL,
        devMbboVXI1564aControl_init_record,
        NULL,
        devMbboVXI1564aControl_write_mbbo,
        NULL
};

epicsExportAddress(dset, devMbboVXI1564aControl);

struct {
        long            number;
        DEVSUPFUN       report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record;
        DEVSUPFUN       get_ioinit_info;
        DEVSUPFUN       read_mbbi;
        DEVSUPFUN       special_linconv;
} devMbbiVXI1564aRawRead = {
        6,
        NULL,
        NULL,
        devMbbiVXI1564aRawRead_init_record,
        devMbbiVXI1564aRawRead_get_ioint_info,
        devMbbiVXI1564aRawRead_read_mbbi,
        NULL
};

epicsExportAddress(dset, devMbbiVXI1564aRawRead);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoVXI1564aControl = {
	6,
	NULL,
	NULL,
	devAoVXI1564aControl_init_record,
	NULL,
	devAoVXI1564aControl_write_ao,
	NULL
};

epicsExportAddress(dset, devAoVXI1564aControl);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiVXI1564aRawRead = {
	6,
	NULL,
	NULL,
	devAiVXI1564aRawRead_init_record,
	devAiVXI1564aRawRead_get_ioint_info,
	devAiVXI1564aRawRead_read_ai,
	NULL
};

epicsExportAddress(dset, devAiVXI1564aRawRead);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutVXI1564aControl = {
	6,
	NULL,
	NULL,
	devStringoutVXI1564aControl_init_record,
	NULL,
	devStringoutVXI1564aControl_write_stringout,
	NULL
};

epicsExportAddress(dset, devStringoutVXI1564aControl);
/*
struct {
    long        number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_wf;
} devWaveVXI1564aMcData = {
    5,
    NULL,
    NULL,
    devWaveVXI1564aMcData_init_record,
    devWaveVXI1564aMcData_get_ioint_info,
    devWaveVXI1564aMcData_read_wave,
};

epicsExportAddress(dset, devWaveVXI1564aMcData);
*/



LOCAL void devVXI1564a_set_taskExTrigger(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_taskExTrigger(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (taskStartTrigger setting): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
	scanIoRequest(ptaskConfig->ioScanPvt);
}
LOCAL void devVXI1564a_set_taskCallback(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_taskCallback(pParam->ptaskConfig);
	epicsPrintf("control thread (taskCallback setting): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_startTask(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_startTask(pParam->ptaskConfig);
	epicsPrintf("control thread (start Task): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_stopTask(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_stopTask(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (stop Task): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_abortTask(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_abortTask(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (Abort Task): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_regUpdateTask(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_regUpdateTask(pParam->ptaskConfig);
	epicsPrintf("control thread (Register Update Task): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_mdsconnectOpen(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_mds_connectOpen(pParam->ptaskConfig);
	epicsPrintf("control thread (mdsplus local tree create): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_mdsdisconnectClose(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_mds_disconnectClose(pParam->ptaskConfig);
	epicsPrintf("control thread (mdsplus local tree create): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_createTree(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_create_newTree(pParam->ptaskConfig);
	epicsPrintf("control thread (mdsplus local tree create): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
#if 0
LOCAL void devVXI1564a_set_mdsDataPut(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_send_dataTree(pParam->ptaskConfig);
	epicsPrintf("control thread (mdsplus data put function): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}

LOCAL void devVXI1564a_set_samplingRate(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_samplingRate(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (smapling rate): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}
#endif
LOCAL void devVXI1564a_set_samplePeriod(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_samplePeriod(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (smaple period(Interval)): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                   			         epicsThreadGetNameSelf());
	scanIoRequest(ptaskConfig->ioScanPvt);
}

LOCAL void devVXI1564a_set_sampleCount(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    	struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_sampleCount(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (sample count): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                   epicsThreadGetNameSelf());
	scanIoRequest(ptaskConfig->ioScanPvt);
}
LOCAL void devVXI1564a_set_samplePreCount(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    	struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_samplePreCount(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (sample count): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                   epicsThreadGetNameSelf());
	scanIoRequest(ptaskConfig->ioScanPvt);
}
LOCAL void devVXI1564a_set_sampleMode(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_sampleMode(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (sample mode): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_triggerType(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_triggerType(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (DAQ Trigger type): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_shotNumber(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_shotNumber(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (KSTAR shotNumber type): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_blipTime(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_blipTime(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (KSTAR Blip Time type): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_daqTime(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_daqTime(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (Daq Start Time type): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_operMode(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_operMode(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (Operation Mode type): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}

LOCAL void devVXI1564a_set_channelRange(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	drvVXI1564a_aoChannelConfig *paoChannelConfig = pParam->pchannelConfig;
    	struct dbCommon *precord = pParam->precord;

	drvVXI1564a_set_channelRange(pParam->ptaskConfig, pParam->pchannelConfig, pParam->setValue); 
	epicsPrintf("control thread (sample count): %s %s %s (%s)\n", ptaskConfig->taskName,paoChannelConfig->chanName, precord->name,
			                                   epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_channelFilter(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	drvVXI1564a_aoChannelConfig *paoChannelConfig = pParam->pchannelConfig;
    	struct dbCommon *precord = pParam->precord;
	
	drvVXI1564a_set_channelFilter(pParam->ptaskConfig, pParam->pchannelConfig, pParam->setValue);
	epicsPrintf("control thread (sample count): %s %s %s (%s)\n", ptaskConfig->taskName,paoChannelConfig->chanName, precord->name,
			                                   epicsThreadGetNameSelf());
}
#if 1
LOCAL void devVXI1564a_set_treeServer(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
        struct dbCommon *precord = pParam->precord;
	strcpy( ptaskConfig->mdsServer, pParam->setStr);
	epicsPrintf("control thread (MDSplus Server): %s %s server:%s (%s)\n", ptaskConfig->taskName, precord->name,ptaskConfig->mdsServer,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_treeName(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
    	struct dbCommon *precord = pParam->precord;
	
	strcpy( ptaskConfig->mdsTree, pParam->setStr);
	epicsPrintf("control thread (MDSplus Tree Name): %s %s treeName:%s (%s)\n", ptaskConfig->taskName, precord->name,ptaskConfig->mdsTree,
			                                             epicsThreadGetNameSelf());
}
LOCAL void devVXI1564a_set_treeNode(execParam *pParam)
{
	drvVXI1564a_taskConfig *ptaskConfig = pParam->ptaskConfig;
	drvVXI1564a_aoChannelConfig      *paoChannelConfig = pParam->pchannelConfig;
    	struct dbCommon *precord = pParam->precord;
	
	strcpy( paoChannelConfig->treeNode, pParam->setStr);
	epicsPrintf("control thread (trigger type): %s %s treeNode:%s (%s)\n", ptaskConfig->taskName, precord->name,paoChannelConfig->treeNode,
			                                             epicsThreadGetNameSelf());
}
#endif 

LOCAL long devBoVXI1564aControl_init_record(boRecord *precord)
{
        devBoVXI1564aControldpvt *pdevBoVXI1564aControldpvt = (devBoVXI1564aControldpvt*) malloc(sizeof(devBoVXI1564aControldpvt));
        int i;

        switch(precord->out.type) {
                case INST_IO:
                        strcpy(pdevBoVXI1564aControldpvt->recordName, precord->name);
                        i = sscanf(precord->out.value.instio.string, "%s %s %s %s %s",
                                   pdevBoVXI1564aControldpvt->taskName,
                                   pdevBoVXI1564aControldpvt->chanName,
				   pdevBoVXI1564aControldpvt->option,
				   pdevBoVXI1564aControldpvt->recordType,
				   pdevBoVXI1564aControldpvt->posNum);

                        pdevBoVXI1564aControldpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevBoVXI1564aControldpvt->taskName);
                        if(!pdevBoVXI1564aControldpvt->ptaskConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devBoVXI1564aControl (init_record) Illegal INP field: here task_name");
                                free(pdevBoVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }
                        break;
                default:
                        recGblRecordError(S_db_badField,(void*) precord,
                                          "devBoVXI1564aControl (init_record) Illegal OUT field");
                        free(pdevBoVXI1564aControldpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;

        }
	if(i==2) {
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TASK_TRIG_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TASK_TRIG;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TASK_CALL_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TASK_CALL;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TASK_START_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TASK_START;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TASK_STOP_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TASK_STOP;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TASK_ABORT_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TASK_ABORT;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TASK_REG_UPDATE_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TASK_REG_UPDATE;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_CREATE_TREE_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_CREATE_TREE;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_MDS_CONNECT_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_MDS_CONNECT;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_MDS_DISCONNECT_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_MDS_DISCONNECT;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_OPER_MODE_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_OPER_MODE;
		}
		if(!strcmp(pdevBoVXI1564aControldpvt->chanName, CONTROL_TRIGGER_TYPE_STR)) {
			pdevBoVXI1564aControldpvt->ind = CONTROL_TRIGGER_TYPE;
		}
	}
#if 0  /* Relay Module Not Add .... */
        if(i==3) {
                if(!strcmp(pdevBoVXI1564aControldpvt->option, CONTROL_RELAY_STR)) {
			pdevBoVXI1564aControldpvt->pchannelConfig 
							= drvVXI1564a_find_channelConfig(pdevBoVXI1564aControldpvt->taskName,
					    			pdevBoVXI1564aControldpvt->chanName);
                if(!pdevBoVXI1564aControldpvt->pchannelConfig) {
			recGblRecordError(S_db_badField, (void*) precord,
					"devBoVXI1564aControl Relay (init_record) Illegal INP field: channel_name");
			free(pdevBoVXI1564aControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
		}
		if(pdevBoVXI1564aControldpvt->pchannelConfig->channelType != RELAY_CHANNEL){
			recGblRecordError(S_db_badField, (void*) precord,
					"devBoVXI1564aControl Relay (init_record) Illegal INP field: channel type mismatch");
			free(pdevBoVXI1564aControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
		}
	   	pdevBoVXI1564aControldpvt->ind = CONTROL_RELAY;
                }
	}
        if(i==5) {
		if(!strcmp(pdevBoVXI1564aControldpvt->option, CONTROL_DO8_STR)) {
			pdevBoVXI1564aControldpvt->pchannelConfig 
							= drvVXI1564a_find_channelConfig(pdevBoVXI1564aControldpvt->taskName,
				       			pdevBoVXI1564aControldpvt->chanName);
                if(!pdevBoVXI1564aControldpvt->pchannelConfig) {
			recGblRecordError(S_db_badField, (void*) precord,
					"devBoVXI1564aControl DO8 (init_record) Illegal INP field: channel_name");
			free(pdevBoVXI1564aControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
		}
		if(pdevBoVXI1564aControldpvt->pchannelConfig->channelType != DO8_CHANNEL){
			recGblRecordError(S_db_badField, (void*) precord,
					"devBoVXI1564aControl DO8 (init_record) Illegal INP field: channel type mismatch");
			free(pdevBoVXI1564aControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
		}
	   	pdevBoVXI1564aControldpvt->ind = CONTROL_DO8;
		}
        }
#endif 
        precord->udf = FALSE;
        precord->dpvt = (void*) pdevBoVXI1564aControldpvt;

        return 0;
}

LOCAL long devBoVXI1564aControl_write_bo(boRecord *precord)
{
        devBoVXI1564aControldpvt        *pdevBoVXI1564aControldpvt = (devBoVXI1564aControldpvt*) precord->dpvt;
        drvVXI1564a_taskConfig          *ptaskConfig;
/*	drvVXI1564a_channelConfig       *pchannelConfig;  */
	drvVXI1564a_controlThreadConfig *pcontrolThreadConfig;  
	controlThreadQueueData         qData;

	unsigned long long int start, stop;
        if(!pdevBoVXI1564aControldpvt || precord->udf == TRUE) {
                precord->pact = TRUE;
                return 0;
        }

        ptaskConfig                = pdevBoVXI1564aControldpvt->ptaskConfig;
/*	pchannelConfig	           = pdevBoVXI1564aControldpvt->pchannelConfig;  */
	pcontrolThreadConfig       = ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig    = ptaskConfig;
	qData.param.precord        = (struct dbCommon *)precord;
	qData.param.setValue       = precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		start = drvVXI1564a_rdtsc();

		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
		switch(pdevBoVXI1564aControldpvt->ind) {
			case CONTROL_TASK_TRIG:
               			qData.pFunc = devVXI1564a_set_taskExTrigger;
				break;
			case CONTROL_TASK_CALL:
               			qData.pFunc = devVXI1564a_set_taskCallback;
				break;
			case CONTROL_TASK_START:
               			qData.pFunc = devVXI1564a_set_startTask;
				break;
			case CONTROL_TASK_STOP:
               			qData.pFunc = devVXI1564a_set_stopTask;
				break;
			case CONTROL_TASK_ABORT:
               			qData.pFunc = devVXI1564a_set_abortTask;
				break;
			case CONTROL_TASK_REG_UPDATE:
               			qData.pFunc = devVXI1564a_set_regUpdateTask;
				break;
			case CONTROL_CREATE_TREE:
               			qData.pFunc = devVXI1564a_set_createTree;
				break;
			case CONTROL_MDS_CONNECT:
               			qData.pFunc = devVXI1564a_set_mdsconnectOpen;
				break;
			case CONTROL_MDS_DISCONNECT:
               			qData.pFunc = devVXI1564a_set_mdsdisconnectClose;
				break;
			case CONTROL_OPER_MODE:
				qData.pFunc = devVXI1564a_set_operMode;
				break;
			case CONTROL_TRIGGER_TYPE:
				qData.pFunc = devVXI1564a_set_triggerType;
				break;
#if 0           /* Relay Module Not Add  */
		case CONTROL_RELAY:
			{                           
			drvVXI1564a_relayChannelConfig   *prelayChannelConfig = (drvVXI1564a_relayChannelConfig*) pchannelConfig;
			if(precord->val && prelayChannelConfig->relayPos == 10437) {
			devVXI1564a_set_relayChanConnect(prelayChannelConfig);
				if(prelayChannelConfig->relayPos == 10438) {
					epicsPrintf("Relay Channel Success : Connect relay %s, Connection Time: %f \n",
						       	prelayChannelConfig->relayName, prelayChannelConfig->adjTime_relayConnect_Usec);
				}
				else if (prelayChannelConfig->relayPos == 10437) {
					precord->val = FALSE;
					epicsPrintf("Relay Channel Connect Error : Not Connect relay %s \n",prelayChannelConfig->relayName);
				}
			else epicsPrintf("Bad Error : relayPos Value is not 10438 and 10437");
			return 0;
			}
			else if(!precord->val && prelayChannelConfig->relayPos == 10438) {
				devVXI1564a_set_relayChanDisconnect(prelayChannelConfig);
				if(prelayChannelConfig->relayPos == 10437) {
					epicsPrintf("Relay Channel Success : DisConnect relay %s, Disconnect Time: %f \n",
						       	prelayChannelConfig->relayName, prelayChannelConfig->adjTime_relayConnect_Usec);
			}
			else if (prelayChannelConfig->relayPos == 10438) {
				precord->val = TRUE;
				epicsPrintf("Relay Channel DisConnect Error : Not Disconnect relay %s \n",prelayChannelConfig->relayName);
	        	}
				else epicsPrintf("Bad Error : relayPos Value is not 10438 and 10437");
				return 0;
				}
			}
			break;
        	case CONTROL_DO8:
            		{
			drvPXI6514_dio8ChannelConfig   *pdio8ChannelConfig = (drvPXI6514_dio8ChannelConfig*) pchannelConfig;
			unsigned short recordShift;
			unsigned long long int start=0, stop=0;
			start = drvVXI1564a_rdtsc(); 
			recordShift = atoi(pdevBoVXI1564aControldpvt->posNum);
			devPXIDO_set_maskBoMbbo(pdio8ChannelConfig, recordShift, 1, pdevBoVXI1564aControldpvt->recordType, precord->val);		
			devPXI6514_writeBo_do8Channel(pdio8ChannelConfig,pdio8ChannelConfig->data);
			stop = drvVXI1564a_rdtsc();
                        epicsPrintf("Control_DO8 WriteBo Time: %f\n",drvVXI1564a_intervalUSec(start,stop));
			epicsPrintf("PXI6514 DO8 Channel Success : PortName %s, Success Time: %f \n",
					        pdio8ChannelConfig->chanName, pdio8ChannelConfig->adjTime_do8_Usec);
            		}
       	    		break;
#endif	
		}
		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
			      (void*) &qData,
			      sizeof(controlThreadQueueData));

		return 0;
	}
	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name,
				                                 epicsThreadGetNameSelf());
		precord->val = FALSE;  /* Action the Function When the Val = 1. But the Val =0 when the action complated    ?????? TG.Lee */
		precord->pact = FALSE;
        	precord->udf = FALSE;
		stop = drvVXI1564a_rdtsc();
		return 2;    /* don't convert */
	}

	return 0;
}

/* Add DI8 Channel BI Record 20070622 */
/*
LOCAL long  devBiVXI1564aControl_init_record(biRecord *precord)
{
        devBiVXI1564aControldpvt *pdevBiVXI1564aControldpvt = (devBiVXI1564aControldpvt*) malloc(sizeof(devBiVXI1564aControldpvt));
        int i;

        switch(precord->inp.type) {
                case INST_IO:
                        strcpy(pdevBiVXI1564aControldpvt->recordName, precord->name);
                        i = sscanf(precord->inp.value.instio.string, "%s %s %s %s",
                                   pdevBiVXI1564aControldpvt->taskName,
                                   pdevBiVXI1564aControldpvt->chanName,
                                   pdevBiVXI1564aControldpvt->option,
				   pdevBiVXI1564aControldpvt->posNum);
                        pdevBiVXI1564aControldpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevBiVXI1564aControldpvt->taskName);
                        if(!pdevBiVXI1564aControldpvt->ptaskConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devBiVXI1564aControl (init_record) Illegal INP field: task name");
                                free(pdevBiVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }
                        pdevBiVXI1564aControldpvt->pchannelConfig = drvVXI1564a_find_channelConfig(pdevBiVXI1564aControldpvt->taskName,
                                                                                                 pdevBiVXI1564aControldpvt->chanName);
                        if(!pdevBiVXI1564aControldpvt->pchannelConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devBiVXI1564aControl (init_record) Illegal INP field: channel name");
                                free(pdevBiVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }

                        break;
                default:
                        recGblRecordError(S_db_badField, (void*) precord,
                                          "devBiVXI1564aControl (init_record) Illegal INP field");
                        free(pdevBiVXI1564aControldpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;
        }

        if(i<4) pdevBiVXI1564aControldpvt->ind = READ_DI8;
        else if(!strcmp(pdevBiVXI1564aControldpvt->option, READ_DI8_STR))
                pdevBiVXI1564aControldpvt->ind = READ_DI8;

        precord->udf = FALSE;
        precord->dpvt = (void*) pdevBiVXI1564aControldpvt;

        return 2;    
}

LOCAL long devBiVXI1564aControl_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
        devBiVXI1564aControldpvt *pdevBiVXI1564aControldpvt = (devBiVXI1564aControldpvt*) precord->dpvt;
        drvVXI1564a_taskConfig   *ptaskConfig;

        if(!pdevBiVXI1564aControldpvt) {
                ioScanPvt = NULL;
                return 0;
        }

        ptaskConfig = pdevBiVXI1564aControldpvt->ptaskConfig;
        *ioScanPvt  = ptaskConfig->ioScanPvt;
        return 0;
}

LOCAL long devBiVXI1564aControl_read_bi(biRecord *precord)
{
        devBiVXI1564aControldpvt         *pdevBiVXI1564aControldpvt = (devBiVXI1564aControldpvt*) precord->dpvt;
        drvVXI1564a_taskConfig           *ptaskConfig;
        drvVXI1564a_channelConfig        *pchannelConfig;

        if(!pdevBiVXI1564aControldpvt) return 0;

        ptaskConfig             = pdevBiVXI1564aControldpvt->ptaskConfig;
        pchannelConfig          = pdevBiVXI1564aControldpvt->pchannelConfig;



        switch(pdevBiVXI1564aControldpvt->ind) {
                case READ_DI8:
			{
			unsigned short recordShift;
			drvPXI6514_dio8ChannelConfig   *pdio8ChannelConfig = (drvPXI6514_dio8ChannelConfig*) pchannelConfig;
                        recordShift = atoi(pdevBiVXI1564aControldpvt->posNum);
			precord->rval = devPXIDI_get_maskBi(pdio8ChannelConfig, recordShift);
			}
                        break;
        }
        return 2;
}

*/

LOCAL long devMbboVXI1564aControl_init_record(mbboRecord *precord)
{
        devMbboVXI1564aControldpvt *pdevMbboVXI1564aControldpvt = (devMbboVXI1564aControldpvt*) malloc(sizeof(devMbboVXI1564aControldpvt));
        int i;

        switch(precord->out.type) {
                case INST_IO:
                        strcpy(pdevMbboVXI1564aControldpvt->recordName, precord->name);
                        i = sscanf(precord->out.value.instio.string, "%s %s %s %s",
                                   pdevMbboVXI1564aControldpvt->taskName,
                                   pdevMbboVXI1564aControldpvt->chanName,
                       		   pdevMbboVXI1564aControldpvt->option,
                   		   pdevMbboVXI1564aControldpvt->recordType);

                        pdevMbboVXI1564aControldpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevMbboVXI1564aControldpvt->taskName);
                        if(!pdevMbboVXI1564aControldpvt->ptaskConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devMbboVXI1564aControl (init_record) Illegal INP field: task_name");
                                free(pdevMbboVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }
                        break;
                default:
                        recGblRecordError(S_db_badField,(void*) precord,
                                          "devMbboVXI1564aControl (init_record) Illegal OUT field");
                        free(pdevMbboVXI1564aControldpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;

        }
    if(i==3) {
		if(!strcmp(pdevMbboVXI1564aControldpvt->option, CONTROL_RANGE_STR)) {
                	pdevMbboVXI1564aControldpvt->pchannelConfig = drvVXI1564a_find_channelConfig(pdevMbboVXI1564aControldpvt->taskName,
                                                                					pdevMbboVXI1564aControldpvt->chanName);
                	if(!pdevMbboVXI1564aControldpvt->pchannelConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devMbboVXI1564aControl (init_record) Illegal INP field: channel_name");
                                free(pdevMbboVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }
			pdevMbboVXI1564aControldpvt->ind = CONTROL_RANGE;
		}
		if(!strcmp(pdevMbboVXI1564aControldpvt->option, CONTROL_FILTER_STR)) {
                	pdevMbboVXI1564aControldpvt->pchannelConfig = drvVXI1564a_find_channelConfig(pdevMbboVXI1564aControldpvt->taskName,
                                                                					pdevMbboVXI1564aControldpvt->chanName);
                	if(!pdevMbboVXI1564aControldpvt->pchannelConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devMbboVXI1564aControl (init_record) Illegal INP field: channel_name");
                                free(pdevMbboVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }
			pdevMbboVXI1564aControldpvt->ind = CONTROL_FILTER;
		}
	}
	else if(i==4) {
                	pdevMbboVXI1564aControldpvt->pchannelConfig = drvVXI1564a_find_channelConfig(pdevMbboVXI1564aControldpvt->taskName,
                                                                					pdevMbboVXI1564aControldpvt->chanName);
        }

        precord->udf = FALSE;
        precord->dpvt = (void*) pdevMbboVXI1564aControldpvt;

        return 0;
}

LOCAL long devMbboVXI1564aControl_write_mbbo(mbboRecord *precord)
{
        devMbboVXI1564aControldpvt      *pdevMbboVXI1564aControldpvt = (devMbboVXI1564aControldpvt*) precord->dpvt;
        drvVXI1564a_taskConfig          *ptaskConfig;
	drvVXI1564a_channelConfig       *pchannelConfig;
	drvVXI1564a_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;

	unsigned long long int start, stop;

	if(!pdevMbboVXI1564aControldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig                = pdevMbboVXI1564aControldpvt->ptaskConfig;
	pchannelConfig	           = pdevMbboVXI1564aControldpvt->pchannelConfig;
	pcontrolThreadConfig       = ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig    = ptaskConfig;
	qData.param.pchannelConfig = pchannelConfig;
	qData.param.precord        = (struct dbCommon *)precord;
	qData.param.setValue       = precord->val;

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		start = drvVXI1564a_rdtsc();

		epicsPrintf("Mbbo db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());

		switch(pdevMbboVXI1564aControldpvt->ind) {
			case CONTROL_RANGE:
				qData.pFunc = devVXI1564a_set_channelRange;
				break;
			case CONTROL_FILTER:
				qData.pFunc = devVXI1564a_set_channelFilter;
				break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name,
				                                 epicsThreadGetNameSelf());
		precord->pact = FALSE;
        	precord->udf = FALSE;
		stop = drvVXI1564a_rdtsc();
		return 2;    /* don't convert */
	}

	return 0;
}

LOCAL long devMbbiVXI1564aRawRead_init_record(mbbiRecord *precord)
{
	devMbbiVXI1564aRawReaddpvt *pdevMbbiVXI1564aRawReaddpvt = (devMbbiVXI1564aRawReaddpvt*) malloc(sizeof(devMbbiVXI1564aRawReaddpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevMbbiVXI1564aRawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevMbbiVXI1564aRawReaddpvt->taskName,
				   pdevMbbiVXI1564aRawReaddpvt->option1,
				   pdevMbbiVXI1564aRawReaddpvt->option2);
			pdevMbbiVXI1564aRawReaddpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevMbbiVXI1564aRawReaddpvt->taskName);
			if(!pdevMbbiVXI1564aRawReaddpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devMbbiVXI1564aRawRead (init_record) Illegal INP field: task name");
				free(pdevMbbiVXI1564aRawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devMbbiVXI1564aRawRead (init_record) Illegal INP field");
			free(pdevMbbiVXI1564aRawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(i<3) {
		if(!strcmp(pdevMbbiVXI1564aRawReaddpvt->option1, READ_STATUS_STR)) {
			pdevMbbiVXI1564aRawReaddpvt->ind = READ_STATUS;
		}
	}
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevMbbiVXI1564aRawReaddpvt;

	return 2;     
}


LOCAL long devMbbiVXI1564aRawRead_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devMbbiVXI1564aRawReaddpvt *pdevMbbiVXI1564aRawReaddpvt = (devMbbiVXI1564aRawReaddpvt*) precord->dpvt;
	drvVXI1564a_taskConfig   *ptaskConfig;

	if(!pdevMbbiVXI1564aRawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevMbbiVXI1564aRawReaddpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devMbbiVXI1564aRawRead_read_mbbi(mbbiRecord *precord)
{
	devMbbiVXI1564aRawReaddpvt		*pdevMbbiVXI1564aRawReaddpvt = (devMbbiVXI1564aRawReaddpvt*) precord->dpvt;
	drvVXI1564a_taskConfig		*ptaskConfig;
	drvVXI1564a_channelConfig	*pchannelConfig;
	double				*pdata;

	if(!pdevMbbiVXI1564aRawReaddpvt) return 0;

	ptaskConfig		= pdevMbbiVXI1564aRawReaddpvt->ptaskConfig;
	pchannelConfig		= pdevMbbiVXI1564aRawReaddpvt->pchannelConfig;
	

	switch(pdevMbbiVXI1564aRawReaddpvt->ind) {
		case READ_STATUS:
			precord->rval = ptaskConfig->taskStatus;
                        if( ptaskConfig->taskStatus & TASK_INITIALIZED )
                        {
                                if( ptaskConfig->taskStatus & TASK_READYSHOT ) precord->val = 1; /* ready to run */
                                else if( ptaskConfig->taskStatus & TASK_WAITTRIG ) precord->val = 2;
                                else if( ptaskConfig->taskStatus & TASK_DATAGATHER ) precord->val = 3;
                                else if( ptaskConfig->taskStatus & TASK_DATADIVIDE ) precord->val = 4;
                                else if( ptaskConfig->taskStatus & TASK_DATASTORE ) precord->val = 5;
                                else precord->val = 7; /* ready but busy with mystery */
                        } else {
                                if( ptaskConfig->taskStatus & TASK_READYSHOT ) precord->val = 1; /* ready */
                                else precord->val = 0; /* not initiated or NULL */
                        }
			break;
		default:
                        epicsPrintf("\n>>> %s: ind( %d )... ERROR! \n", ptaskConfig->taskName, pdevMbbiVXI1564aRawReaddpvt->ind);
                        break;
	}

	
	return 2;
}

LOCAL long devAoVXI1564aControl_init_record(aoRecord *precord)
{
	devAoVXI1564aControldpvt *pdevAoVXI1564aControldpvt = (devAoVXI1564aControldpvt*) malloc(sizeof(devAoVXI1564aControldpvt));
	int i;
#if 0
	epicsPrintf(" TG >Lee AO Record Init Process...\n");
#endif
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevAoVXI1564aControldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevAoVXI1564aControldpvt->taskName,
			           pdevAoVXI1564aControldpvt->option1,
			           pdevAoVXI1564aControldpvt->option2);

			pdevAoVXI1564aControldpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevAoVXI1564aControldpvt->taskName);
			
			if(!pdevAoVXI1564aControldpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoVXI1564aControl (init_record) Illegal INP field in AO: task_name");
				free(pdevAoVXI1564aControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoVXI1564aControl (init_record) Illegal OUT field in AO");
			free(pdevAoVXI1564aControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if(i<3) {
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_SAMPLE_PERIOD_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_SAMPLE_PERIOD;
		}
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_SAMPLE_COUNT_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_SAMPLE_COUNT;
		}
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_SAMPLE_PRECOUNT_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_SAMPLE_PRECOUNT;
		}
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_SAMPLE_MODE_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_SAMPLE_MODE;
		}
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_SHOT_NUMBER_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_SHOT_NUMBER;
		}
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_BLIP_TIME_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_BLIP_TIME;
		}
		if(!strcmp(pdevAoVXI1564aControldpvt->option1, CONTROL_DAQ_TIME_STR)) {
			pdevAoVXI1564aControldpvt->ind = CONTROL_DAQ_TIME;
		}
	}
	 else if(i==3) {
        	pdevAoVXI1564aControldpvt->pchannelConfig
            		= drvVXI1564a_find_channelConfig(pdevAoVXI1564aControldpvt->taskName,
                        			            pdevAoVXI1564aControldpvt->option2);
    	}	

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAoVXI1564aControldpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoVXI1564aControl_write_ao(aoRecord *precord)
{
	devAoVXI1564aControldpvt        *pdevAoVXI1564aControldpvt = (devAoVXI1564aControldpvt*) precord->dpvt;
	drvVXI1564a_taskConfig          *ptaskConfig;
	drvVXI1564a_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;
	

	unsigned long long int start, stop;

	if(!pdevAoVXI1564aControldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig                = pdevAoVXI1564aControldpvt->ptaskConfig;
	pcontrolThreadConfig       = ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig    = ptaskConfig;
	qData.param.precord        = (struct dbCommon *)precord;
	qData.param.setValue       = precord->val;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		start = drvVXI1564a_rdtsc();

		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());

		switch(pdevAoVXI1564aControldpvt->ind) {
			case CONTROL_SAMPLE_PERIOD:
                		/*  Device Sampling Rate Limit Cut if User is over Sampling Rate Setting.  */
                		/*  Record pact is TRUE. So I cann't direct change "precord->val" value.   */
		                if(precord->val > ptaskConfig->samplePeriodMax){
                    			qData.param.setValue = ptaskConfig->samplePeriodMax;
                		} else if(precord->val < ptaskConfig->samplePeriodMin){
                    			qData.param.setValue = ptaskConfig->samplePeriodMin;
                		}
                		qData.pFunc = devVXI1564a_set_samplePeriod;
				break;
			case CONTROL_SAMPLE_COUNT:
	                	/*  Device Sample Counts Limit Cut if User is over Sample Counts Setting.  */
				if(precord->val > ptaskConfig->sampleMaxCount){
        		            qData.param.setValue = ptaskConfig->sampleMaxCount;
                		} else if(precord->val <= 0){
                    			qData.param.setValue = 1;
                		}
				qData.pFunc = devVXI1564a_set_sampleCount;
				break;
			case CONTROL_SAMPLE_PRECOUNT:
	                	/*  Device Sample PreTrigger Limit Cut if User is over Sample Pre Counts Setting.  */
				if(precord->val > ptaskConfig->sampleMaxCount){
        		            qData.param.setValue = ptaskConfig->sampleMaxCount;
                		} else if(precord->val < 0){
                    			qData.param.setValue = 0;
                		}
				qData.pFunc = devVXI1564a_set_samplePreCount;
				break;
			case CONTROL_SAMPLE_MODE:
				qData.pFunc = devVXI1564a_set_sampleMode;
				break;
			case CONTROL_SHOT_NUMBER:
				qData.pFunc = devVXI1564a_set_shotNumber;
				break;
			case CONTROL_BLIP_TIME:
				qData.pFunc = devVXI1564a_set_blipTime;
				break;
			case CONTROL_DAQ_TIME:
				qData.pFunc = devVXI1564a_set_daqTime;
				break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name,
				                                 epicsThreadGetNameSelf());
		precord->pact = FALSE;
        	precord->udf = FALSE;

		switch(pdevAoVXI1564aControldpvt->ind) {
            		case CONTROL_SAMPLE_PERIOD:
                		if(precord->val > ptaskConfig->samplePeriodMax){
                		/*  Device Sampling Rate Limit Cut if User is over Sampling Rate Setting.  */
                    			precord->val = ptaskConfig->samplePeriodMax;
                    			epicsPrintf("db processing: phase II %s Control. You Sample Period OverRange Input: %f \n",
                                    									precord->name, precord->val);
                		} else if(precord->val < ptaskConfig->samplePeriodMin){
                    			precord->val = ptaskConfig->samplePeriodMin;
                    			epicsPrintf("db processing: phase II %s Control. You Sample Period UnderRange Input: %f \n",
                                    									precord->name, precord->val);
                		}
                		break;
            		case CONTROL_SAMPLE_COUNT:
                		if(precord->val > ptaskConfig->sampleMaxCount){
                		/*  Device Sampling Counts Limit Cut if User is over Sample Counts Setting.  */
                 			precord->val = ptaskConfig->sampleMaxCount;
                    			epicsPrintf("db processing: phase II %s Control. You Sample Count Over Range Input: %f \n",
                                    									precord->name, precord->val);
                		} else if(precord->val <= 0){
                    			precord->val = 1;
                    			epicsPrintf("db processing: phase II %s Control. You Sample Count UnderRange Input: %f \n",
                                    									precord->name, precord->val);
                		}
                		break;
            		case CONTROL_SAMPLE_PRECOUNT:
                		if(precord->val > ptaskConfig->sampleMaxCount){
                		/*  Device Sample PreTrigger Counts Limit Cut if User is over Sample Counts Setting.  */
                 			precord->val = ptaskConfig->sampleMaxCount;
                    			epicsPrintf("db processing: phase II %s Control. You Sample Pre Count Over Range Input: %f \n",
                                    									precord->name, precord->val);
                		} else if(precord->val < 0){
                    			precord->val = 0;
                    			epicsPrintf("db processing: phase II %s Control. You Sample Count UnderRange Input: %f \n",
                                    									precord->name, precord->val);
                		}
                		break;
        	}

		stop = drvVXI1564a_rdtsc();
		return 2;    /* don't convert */
	}

	return 0;
}


LOCAL long devStringoutVXI1564aControl_init_record(stringoutRecord *precord)
{
        devStringoutVXI1564aControldpvt *pdevStringoutVXI1564aControldpvt = (devStringoutVXI1564aControldpvt*) malloc(sizeof(devStringoutVXI1564aControldpvt));
        int i;

        switch(precord->out.type) {
                case INST_IO:
                        strcpy(pdevStringoutVXI1564aControldpvt->recordName, precord->name);
                        i = sscanf(precord->out.value.instio.string, "%s %s %s",
                                   pdevStringoutVXI1564aControldpvt->taskName,
                                   pdevStringoutVXI1564aControldpvt->option1 ,
                                   pdevStringoutVXI1564aControldpvt->option2);

#ifdef DEBUG
        epicsPrintf("devVXI1564aControl arg num: %d: %s %s\n",i, pdevStringoutVXI1564aControldpvt->taskName,
                                                                pdevStringoutVXI1564aControldpvt->option1);
#endif
                        pdevStringoutVXI1564aControldpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevStringoutVXI1564aControldpvt->taskName);
                        if(!pdevStringoutVXI1564aControldpvt->ptaskConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devStringoutVXI1564aControl (init_record) Illegal INP field in STINGOUT: task_name");
                                free(pdevStringoutVXI1564aControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }


                        break;
                default:
                        recGblRecordError(S_db_badField,(void*) precord,
                                          "devStringoutVXI1564aControl (init_record) Illegal OUT in STRINGOUT field");
                        free(pdevStringoutVXI1564aControldpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;

        }

		if(i<3) {
			if(!strcmp(pdevStringoutVXI1564aControldpvt->option1, CONTROL_TREE_SERV_STR)) {
				pdevStringoutVXI1564aControldpvt->ind = CONTROL_TREE_SERV;
			}
			if(!strcmp(pdevStringoutVXI1564aControldpvt->option1, CONTROL_TREE_NAME_STR)) {
				pdevStringoutVXI1564aControldpvt->ind = CONTROL_TREE_NAME;
			}
		}
		else if(i==3) {
			if(!strcmp(pdevStringoutVXI1564aControldpvt->option1, CONTROL_TREE_NODE_STR)) {
				pdevStringoutVXI1564aControldpvt->pchannelConfig 
					= drvVXI1564a_find_channelConfig(pdevStringoutVXI1564aControldpvt->taskName,
							pdevStringoutVXI1564aControldpvt->option2);
				if(!pdevStringoutVXI1564aControldpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							"devStringoutVXI1564aControl Stringout (init_record) Illegal INP field: channel_name");
					free(pdevStringoutVXI1564aControldpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
				pdevStringoutVXI1564aControldpvt->ind = CONTROL_TREE_NODE;
			}
		}
		precord->udf = FALSE;
		precord->dpvt = (void*) pdevStringoutVXI1564aControldpvt;

		return 2;     /* don't convert */
}



LOCAL long devStringoutVXI1564aControl_write_stringout(stringoutRecord *precord)
{
	devStringoutVXI1564aControldpvt        *pdevStringoutVXI1564aControldpvt = (devStringoutVXI1564aControldpvt*) precord->dpvt;
	drvVXI1564a_taskConfig          *ptaskConfig;
	drvVXI1564a_channelConfig       *pchannelConfig;
	drvVXI1564a_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;
	

	unsigned long long int start, stop;

	if(!pdevStringoutVXI1564aControldpvt || precord->udf == TRUE) {
		precord->pact = TRUE; /* pact is TRUE = Record Processing (Not Completed) */
		return 0;
	}

	ptaskConfig                = pdevStringoutVXI1564aControldpvt->ptaskConfig;
	pchannelConfig	           = pdevStringoutVXI1564aControldpvt->pchannelConfig;
	pcontrolThreadConfig       = ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig    = ptaskConfig;
	qData.param.pchannelConfig = pchannelConfig;
	qData.param.precord        = (struct dbCommon *)precord;
/*	qData.param.setValue       = precord->val;  */
	strcpy( qData.param.setStr, precord->val);

        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		start = drvVXI1564a_rdtsc();

		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());

		switch(pdevStringoutVXI1564aControldpvt->ind) {
#if 1
			case CONTROL_TREE_SERV:
                		qData.pFunc = devVXI1564a_set_treeServer;
				break;
			case CONTROL_TREE_NAME:
                		qData.pFunc = devVXI1564a_set_treeName;
				break;
			case CONTROL_TREE_NODE:
                		qData.pFunc = devVXI1564a_set_treeNode;
				break;
#endif
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name,
				                                 epicsThreadGetNameSelf());
		precord->pact = FALSE;
        	precord->udf = FALSE;
		stop = drvVXI1564a_rdtsc();
		return 2;    /* don't convert */
	}

	return 0;
}




LOCAL long devAiVXI1564aRawRead_init_record(aiRecord *precord)
{
	devAiVXI1564aRawReaddpvt *pdevAiVXI1564aRawReaddpvt = (devAiVXI1564aRawReaddpvt*) malloc(sizeof(devAiVXI1564aRawReaddpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiVXI1564aRawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiVXI1564aRawReaddpvt->taskName,
				   pdevAiVXI1564aRawReaddpvt->option1,
				   pdevAiVXI1564aRawReaddpvt->option2);
			pdevAiVXI1564aRawReaddpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevAiVXI1564aRawReaddpvt->taskName);
			if(!pdevAiVXI1564aRawReaddpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiVXI1564aRawRead (init_record) Illegal INP field: task name");
				free(pdevAiVXI1564aRawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiVXI1564aRawRead (init_record) Illegal INP field");
			free(pdevAiVXI1564aRawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(i<3) {
		if(!strcmp(pdevAiVXI1564aRawReaddpvt->option1, READ_SAMPLING_RATE_STR)) {
			pdevAiVXI1564aRawReaddpvt->ind = READ_SAMPLING_RATE;
		}
		else if(!strcmp(pdevAiVXI1564aRawReaddpvt->option1, READ_SAMPLE_COUNT_STR)) {
			pdevAiVXI1564aRawReaddpvt->ind = READ_SAMPLE_COUNT;
		}
		else if(!strcmp(pdevAiVXI1564aRawReaddpvt->option1, READ_SAMPLE_PRECOUNT_STR)) {
			pdevAiVXI1564aRawReaddpvt->ind = READ_SAMPLE_PRECOUNT;
		}
		else if(!strcmp(pdevAiVXI1564aRawReaddpvt->option1, READ_SAMPLE_MODE_STR)) {
			pdevAiVXI1564aRawReaddpvt->ind = READ_SAMPLE_MODE;
		}
		else if(!strcmp(pdevAiVXI1564aRawReaddpvt->option1, READ_TRIGGER_TYPE_STR)) {
			pdevAiVXI1564aRawReaddpvt->ind = READ_TRIGGER_TYPE;
		}
		else if(!strcmp(pdevAiVXI1564aRawReaddpvt->option1, READ_OPER_MODE_STR)) {
			pdevAiVXI1564aRawReaddpvt->ind = READ_OPER_MODE;
		}
    		else pdevAiVXI1564aRawReaddpvt->ind = READ_VAL;
	}
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiVXI1564aRawReaddpvt;

	return 2;     
}


LOCAL long devAiVXI1564aRawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devAiVXI1564aRawReaddpvt *pdevAiVXI1564aRawReaddpvt = (devAiVXI1564aRawReaddpvt*) precord->dpvt;
	drvVXI1564a_taskConfig   *ptaskConfig;

	if(!pdevAiVXI1564aRawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevAiVXI1564aRawReaddpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devAiVXI1564aRawRead_read_ai(aiRecord *precord)
{
	devAiVXI1564aRawReaddpvt		*pdevAiVXI1564aRawReaddpvt = (devAiVXI1564aRawReaddpvt*) precord->dpvt;
	drvVXI1564a_taskConfig		*ptaskConfig;
	drvVXI1564a_channelConfig	*pchannelConfig;
	double				*pdata;

	if(!pdevAiVXI1564aRawReaddpvt) return 0;

	ptaskConfig		= pdevAiVXI1564aRawReaddpvt->ptaskConfig;
	pchannelConfig		= pdevAiVXI1564aRawReaddpvt->pchannelConfig;
	pdata			= ptaskConfig->pdata;

	

	switch(pdevAiVXI1564aRawReaddpvt->ind) {
		case READ_SAMPLING_RATE:
			precord->val = ptaskConfig->samplingRate;
			break;
		case READ_SAMPLE_COUNT:
			precord->val = ptaskConfig->sampleCount;
			break;
		case READ_SAMPLE_PRECOUNT:
			precord->val = ptaskConfig->samplePreCount;
			break;
		case READ_OPER_MODE:
			precord->val = ptaskConfig->opMode;
			break;
/*
		case READ_SAMPLE_MODE:
			precord->val = ptaskConfig->sampleMode;
			break;
		case READ_TRIGGER_TYPE:
			precord->val = ptaskConfig->triggerType;
			break;
		case READ_VAL:
			precord->val = pdata[pchannelConfig->chanIndex];
			break;
*/
	}
	
	return 2;
}

#if 0

LOCAL long devWaveVXI1564aMcData_init_record(waveformRecord *precord)
{
	devWfVXI1564aControldpvt *pdevWfVXI1564aControldpvt = (devWfVXI1564aControldpvt*) malloc(sizeof(devWfVXI1564aControldpvt));
	int i;

	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevWfVXI1564aControldpvt->recordName, precord->name);
			i=sscanf(precord->inp.value.instio.string, "%s %s %s %s",
					pdevWfVXI1564aControldpvt->taskName,
					pdevWfVXI1564aControldpvt->chanName,
					pdevWfVXI1564aControldpvt->chanIndex,
					pdevWfVXI1564aControldpvt->option);

			pdevWfVXI1564aControldpvt->ptaskConfig = drvVXI1564a_find_taskConfig(pdevWfVXI1564aControldpvt->taskName);
			if(!pdevWfVXI1564aControldpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devWfVXI1564aControl (init_record) Illegal INP field in WAVE: task name");
					epicsPrintf("record: %s, task: %s, chan: %s, chanIndex: %s\n",
					    pdevWfVXI1564aControldpvt->recordName,
					    pdevWfVXI1564aControldpvt->taskName,
					    pdevWfVXI1564aControldpvt->chanName,
					    pdevWfVXI1564aControldpvt->chanIndex);
				free(pdevWfVXI1564aControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			pdevWfVXI1564aControldpvt->pchannelConfig = drvVXI1564a_find_channelConfig(pdevWfVXI1564aControldpvt->taskName,
												pdevWfVXI1564aControldpvt->chanName);
			if(!pdevWfVXI1564aControldpvt->pchannelConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devWfVXI1564aControl (init_record) Illegal INP filed in WAVE: channel name");
						epicsPrintf("record: %s, task: %s, chan: %s, chanIndex: %s\n",
					    pdevWfVXI1564aControldpvt->recordName,
					    pdevWfVXI1564aControldpvt->taskName,
					    pdevWfVXI1564aControldpvt->chanName,
					    pdevWfVXI1564aControldpvt->chanIndex);
				free(pdevWfVXI1564aControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;

			}
			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
			                  "devWfVXI1564aControl (init_record) Illegal INP field in WAVE");
			free(pdevWfVXI1564aControldpvt);
		    precord->udf = TRUE;
		    precord->dpvt = NULL;
		    return S_db_badField;
	}

        if(i<5) pdevWfVXI1564aControldpvt->ind = READ_WAVEFORM;
        else if(!strcmp(pdevWfVXI1564aControldpvt->option, READ_WAVEFORM_STR))
                pdevWfVXI1564aControldpvt->ind = READ_WAVEFORM;

    	precord->udf = FALSE;
        precord->dpvt = (void*)pdevWfVXI1564aControldpvt;

	epicsPrintf("devWave Data Init recored chanIndex S:%s ,Int:%d \n",pdevWfVXI1564aControldpvt->chanIndex,atoi( pdevWfVXI1564aControldpvt->chanIndex));
    	return 2;  /* don't convert */
}


LOCAL long devWaveVXI1564aMcData_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt)
{
	devWfVXI1564aControldpvt *pdevWfVXI1564aControldpvt = (devWfVXI1564aControldpvt*) precord->dpvt;
	drvVXI1564a_taskConfig *ptaskConfig;

	epicsPrintf("devWave Data get Ioint info recored\n");
	if(!pdevWfVXI1564aControldpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevWfVXI1564aControldpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devWaveVXI1564aMcData_read_wave(waveformRecord *precord)
{
	devWfVXI1564aControldpvt   *pdevWfVXI1564aControldpvt = (devWfVXI1564aControldpvt*) precord->dpvt;
	drvVXI1564a_taskConfig           *ptaskConfig;
	drvVXI1564a_wfChannelConfig      *pwfChannelConfig;
	double				*pdata;

	long				status;	
	int				i;	
	int				bufIndex;
	uInt32				sampleCount;
	int 				chanNum;
/*	unsigned long long int start, stop;  */
	
	if(!pdevWfVXI1564aControldpvt) return 0;


	ptaskConfig           = pdevWfVXI1564aControldpvt->ptaskConfig;
	pwfChannelConfig  = pdevWfVXI1564aControldpvt->pwfChannelConfig;
	pdata		      = ptaskConfig->pdata;
/*	bufIndex = pdevWfVXI1564aControldpvt->chanNum * ptaskConfig->sampleCount;  */
	sampleCount = ptaskConfig->sampleCount;
	chanNum = atoi(pdevWfVXI1564aControldpvt->chanIndex); 
	bufIndex = chanNum * sampleCount;
	
	printf("devWave Data channel Index:%s, val: %d \n",pdevWfVXI1564aControldpvt->chanIndex,bufIndex); 
/*  */
	if(sampleCount >0) precord->nelm = sampleCount;
#if 0 
	epicsPrintf("devWave Data channel Index: %d SampleCount:%d rarm :%d \n", 
			bufIndex,ptaskConfig->sampleCount, precord->rarm);

	epicsPrintf("devWave Data channel Num  : %d SampleCount:%d \n", pwfChannelConfig->chanNum,ptaskConfig->sampleCount); 
/*		
	start = drvVXI1564a_rdtsc();
	epicsThreadSleep(3.1);  */

	epicsMutexLock(ptaskConfig->bufferLock);	
	precord->bptr = &ptaskConfig->vxi1564aBuffer[bufIndex];
	precord->bptr = &ptaskConfig->vxi1564aBuffer[bufIndex];
/*	memcpy(precord->val, &ptaskConfig->vxi1564aBuffer[bufIndex],sizeof(float64)*sampleCount);  error  */
	epicsMutexUnlock(ptaskConfig->bufferLock);
	epicsThreadSleep(1.); 
	precord->bptr = &pwfChannelConfig->vxi1564aChBuffer;
#endif
	precord->bptr = &ptaskConfig->vxi1564aBuffer[bufIndex];
/*       Device Support must set this value when it completes.      */

	
	if(sampleCount >0) precord->nord = sampleCount;  

#if 0
	if(bufIndex == 0){
		printf("read buffer data: ");
		for(i=0; i<100; i++){
			printf("%lf ,", *((double *)precord->bptr + i));
		}
		printf("\n");
	}
#endif
	return (status);
}
#endif
