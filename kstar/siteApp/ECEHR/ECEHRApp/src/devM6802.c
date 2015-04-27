/******************************************************************************
 *  Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved.                     *
 ******************************************************************************/

/*==========================================================
     EDIT HISTORY




2007. 12. 27
LOCAL void devM6802_set_RemoteStorage(execParam *pParam)
+  if( g_OpMode == OPMODE_REMOTE ) { ... }


*/

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
#include "epicsExport.h"


#include "drvM6802.h"

#include "devMDSplus.h"
#include "ecehrGlobal.h"

#include "drvFPDP.h"

#if 0
#define DEBUG
#endif




#define  READ_GAIN	  	(0x00000001<<0)
#define  READ_VAL	   	(0x00000001<<1)
#define  READ_STATE	   	(0x00000001<<2)
#define  READ_GAIN_STR		"gain"
#define  READ_VAL_STR		"val"
#define  READ_STATE_STR		"state"

#define  CONTROL_SAMPLING	(0x00000001<<0)
#define  CONTROL_GAIN		(0x00000001<<1)
#define  CONTROL_MASK		(0x00000001<<2)
#define  CONTROL_ADC_START	(0x00000001<<3)
#define  CONTROL_ADC_STOP	(0x00000001<<4)
#define  CONTROL_DAQ_START	(0x00000001<<5)
#define  CONTROL_DAQ_STOP	(0x00000001<<6)
#define  CONTROL_LOCAL_STORAGE	(0x00000001<<7)
#define  CONTROL_REMOTE_STORAGE	(0x00000001<<8)
#define  CONTROL_TAG		(0x00000001<<9)
#define  CONTROL_ONOFF		(0x00000001<<10)
#define  CONTROL_SHOTNUM	(0x00000001<<11)
#define  CONTROL_CREATESHOT	(0x00000001<<12)
#define  CONTROL_SET_MODE	(0x00000001<<13)
#define  CONTROL_SET_INTERCLK	(0x00000001<<14)
#define  CONTROL_SET_EXTERCLK	(0x00000001<<15)
#define  CONTROL_SET_T0		(0x00000001<<16)
#define  CONTROL_SET_T1		(0x00000001<<17)
#define  CONTROL_TRIG_INTER     (0x00000001<<18)
#define  CONTROL_TRIG_EXTER     (0x00000001<<19)
#define  CONTROL_BLIP_TIME	(0x00000001<<20)         /* Add Blip Start Time information. Modify TG.Lee at 20080929 */
#define  CONTROL_BO_ZCAL	(0X00000001<<21)
#define  CONTROL_AVR_ECE	(0X00000001<<22)
#define  CONTROL_SET_ECEMODE	(0x00000001<<23)
/*
#define  CONTROL_DATA2ASCII	(0x00000001<<12)
#define  CONTROL_TREE_WRITE	(0x00000001<<13)
#define  CONTROL_TREE_READ	(0x00000001<<14)
*/
#define  CONTROL_SAMPLING_STR		"smpl_rate"
#define  CONTROL_GAIN_STR   		"ch_gain"
#define  CONTROL_MASK_STR		"ch_useNum"
#define  CONTROL_ADC_START_STR  	"adc_start"
#define  CONTROL_ADC_STOP_STR   	"adc_stop"
#define  CONTROL_DAQ_START_STR  	"daq_start"
#define  CONTROL_DAQ_STOP_STR   	"daq_stop"
#define  CONTROL_LOCAL_STORAGE_STR	"local_save"
#define  CONTROL_REMOTE_STORAGE_STR   	"remote_save"
#define  CONTROL_TAG_STR		"tag"
#define  CONTROL_ONOFF_STR		"ch_onoff"
#define  CONTROL_SHOTNUM_STR		"shot_num"
#define  CONTROL_CREATESHOT_STR		"create_shot"
#define  CONTROL_SET_MODE_STR		"set_mode"
#define  CONTROL_SET_INTERCLK_STR	"internal_clk"
#define  CONTROL_SET_EXTERCLK_STR	"external_clk"
#define  CONTROL_SET_T0_STR		"time_t0"
#define  CONTROL_SET_T1_STR		"time_t1"
#define  CONTROL_TRIG_INTER_STR         "inter_trig"
#define  CONTROL_TRIG_EXTER_STR         "exter_trig"
#define  CONTROL_BLIP_TIME_STR		"blip_time"     /* Add Blip Start Time information. Modify TG.Lee at 20080929 */
#define  CONTROL_BO_ZCAL_STR		"bo_zcal"
#define  CONTROL_AVR_ECE_STR		"avr_ece"
#define  CONTROL_SET_ECEMODE_STR	"ece_calMode"  /*   val ==0 do not action, val == 1 calibration mode */


/*
#define  CONTROL_DATA2ASCII_STR		"raw2ascii"
#define  CONTROL_TREE_WRITE_STR		"tree_write"
#define  CONTROL_TREE_READ_STR		"tree_read"
*/



LOCAL void devM6802_set_samplingRate(execParam *pParam);
/*LOCAL void devM6802_set_chanGain(execParam *pParam); */
LOCAL void devM6802_set_chanMask(execParam *pParam);
LOCAL void devM6802_set_ADCstart(execParam *pParam);
LOCAL void devM6802_set_ADCstop(execParam *pParam);
LOCAL void devM6802_set_DAQstart(execParam *pParam);
LOCAL void devM6802_set_DAQstop(execParam *pParam);
LOCAL void devM6802_set_LocalStorage(execParam *pParam);
LOCAL void devM6802_set_AvrEceChannel(execParam *pParam);
LOCAL void devM6802_set_RemoteStorage(execParam *pParam);
/*
LOCAL void devM6802_set_dataConvert(execParam *pParam);
LOCAL void devM6802_tree_dataWrite(execParam *pParam);
LOCAL void devM6802_tree_dataRead(execParam *pParam);
*/
LOCAL void devM6802_set_tagName(execParam *pParam);
LOCAL void devM6802_set_chOnOff(execParam *pParam);
LOCAL void devM6802_set_shotNum(execParam *pParam);
LOCAL void devM6802_set_CreateShot(execParam *pParam);
LOCAL void devM6802_set_setOPmode(execParam *pParam);
LOCAL void devM6802_set_IntlClock(execParam *pParam);
LOCAL void devM6802_set_ExtlClock(execParam *pParam);
LOCAL void devM6802_set_T0(execParam *pParam);
LOCAL void devM6802_set_ECECalMode(execParam *pParam);
LOCAL void devM6802_set_T1(execParam *pParam);
LOCAL void devM6802_set_IntlTrigger(execParam *pParam);
LOCAL void devM6802_set_ExtlTrigger(execParam *pParam);
LOCAL void devM6802_set_ZeroCalibration(execParam *pParam);
LOCAL void devM6802_set_blipTime(execParam *pParam);          /* Add Blip Start Time information. Modify TG.Lee at 20080929 */

typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];

	char recordName[80];

	drvM6802_taskConfig *ptaskConfig;
/*	drvM6802_channelConfig *pchannelConfig; */

	unsigned        ind;

} devAoM6802Controldpvt;

typedef struct {
	char taskName[60];
	char option1[60];
	char chanName[60];
	
	char recordName[80];

	drvM6802_taskConfig *ptaskConfig;
	drvM6802_channelConfig *pchannelConfig; 

	unsigned        ind;

} devBoM6802Controldpvt;

typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];

	char recordName[80];

	drvM6802_taskConfig *ptaskConfig;
	int chennelID;

	unsigned        ind;

} devStringoutM6802Controldpvt;

typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];
	char recordName[80];

	drvM6802_taskConfig *ptaskConfig;
	drvM6802_channelConfig *pchannelConfig;

	unsigned	ind;
} devMbbiM6802RawReaddpvt;



LOCAL long devAoM6802Control_init_record(aoRecord *precord);
LOCAL long devAoM6802Control_write_ao(aoRecord *precord);

LOCAL long devBoM6802Control_init_record(boRecord *precord);
LOCAL long devBoM6802Control_write_bo(boRecord *precord);

LOCAL long devStringoutM6802Control_init_record(stringoutRecord *precord);
LOCAL long devStringoutM6802Control_write_stringout(stringoutRecord *precord);

LOCAL long devMbbiM6802RawRead_init_record(mbbiRecord *precord);
LOCAL long devMbbiM6802RawRead_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devMbbiM6802RawRead_read_mbbi(mbbiRecord *precord);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoM6802Control = {
	6,
	NULL,
	NULL,
	devAoM6802Control_init_record,
	NULL,
	devAoM6802Control_write_ao,
	NULL
};
epicsExportAddress(dset, devAoM6802Control);


struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_bo;
	DEVSUPFUN	special_linconv;
} devBoM6802Control = {
	6,
	NULL,
	NULL,
	devBoM6802Control_init_record,
	NULL,
	devBoM6802Control_write_bo,
	NULL
};
epicsExportAddress(dset, devBoM6802Control);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_stringout;
	DEVSUPFUN	special_linconv;
} devStringoutM6802Control = {
	6,
	NULL,
	NULL,
	devStringoutM6802Control_init_record,
	NULL,
	devStringoutM6802Control_write_stringout,
	NULL
};
epicsExportAddress(dset, devStringoutM6802Control);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_mbbi;
	DEVSUPFUN	special_linconv;
} devMbbiM6802RawRead = {
	6,
	NULL,
	NULL,
	devMbbiM6802RawRead_init_record,
	devMbbiM6802RawRead_get_ioint_info,
	devMbbiM6802RawRead_read_mbbi,
	NULL
};

epicsExportAddress(dset, devMbbiM6802RawRead);

unsigned long long int start, stop, _interval;

int g_ShotNumber = 1;               /* Just an example shot number */
/* float g_BlipTime = 0.0;            Init Data Start Time is Zero. Add TG.Lee Modify 20080929  */
int g_OpMode = OPMODE_TEST;

unsigned int		systemStatus=0;



LOCAL void devM6802_set_ExtlTrigger(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

/* val == 1 external trigger */
	if( !drvM6802_set_triggerSource(ptaskConfig, 1) ) {
		epicsPrintf("\n>>> devM6802_set_ExtlTrigger : drvM6802_set_triggerSource(1) ... error\n");
		return ;
	}

	epicsPrintf("control thread (External Trig): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
    epicsThreadGetNameSelf());
}

LOCAL void devM6802_set_ZeroCalibration(execParam *pParam)
{
	unsigned long long int startCal, stopCal, currentCal;
	startCal = drvM6802_getCurrentUsec();
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig; 
/*	drvM6802_channelConfig *pchannelConfig = pParam->pchannelConfig;   */
	struct dbCommon *precord = pParam->precord;  
/*	int chanInfo;
	unsigned short zCalVal;
	int val = (uint16)pParam->precord;	
	chanInfo = ptaskConfig->chInfo;
	zCalVal = ptaskConfig->zCal;
*/
	uint16 val = (uint16)pParam->setValue;
	
	/*int drvM6802_set_zeroCalibration(drvM6802_taskConfig *ptaskConfig,uint16 val)*/
	if( !drvM6802_set_zeroCalibration(ptaskConfig, val) ) {
		epicsPrintf("\n>>> devM6802_set_ZeroCalibration : drvM6802_set_zeroCalibration ... error\n");
		return ;
	}
	stopCal = drvM6802_getCurrentUsec();
	currentCal = drvM6802_intervalUSec(startCal, stopCal);
	epicsPrintf("control thread (External Trig): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
    epicsThreadGetNameSelf());
	epicsPrintf("\nZero Calibration time is %lf msec\n", 1.E-3 * (double)currentCal);
}

LOCAL void devM6802_set_IntlTrigger(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

/* val == 0 internal trigger */
	if( !drvM6802_set_triggerSource(ptaskConfig, 0) ) {
		epicsPrintf("\n>>> devM6802_set_IntlTrigger : drvM6802_set_triggerSource(0) ... error\n");
		return ;
	}

	epicsPrintf("control thread (Internal Trig): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
    epicsThreadGetNameSelf());
}

LOCAL void devM6802_set_ECECalMode(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	ptaskConfig->eceCalMode = (int)pParam->setValue;


	epicsPrintf("control thread (Ece Cal Mode Select): %s %s (%s), %u sec\n", ptaskConfig->taskName, precord->name,
    										epicsThreadGetNameSelf(), ptaskConfig->eceCalMode);
}
LOCAL void devM6802_set_T0(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	ptaskConfig->fT0 = (float)pParam->setValue;


	epicsPrintf("control thread (T0): %s %s (%s), %f sec\n", ptaskConfig->taskName, precord->name,
    									epicsThreadGetNameSelf(), ptaskConfig->fT0);
}

LOCAL void devM6802_set_T1(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	ptaskConfig->fT1 = (float)pParam->setValue;


	epicsPrintf("control thread (T1): %s %s (%s), %f sec\n", ptaskConfig->taskName, precord->name,
    epicsThreadGetNameSelf(), ptaskConfig->fT1);
}

LOCAL void devM6802_set_ExtlClock(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

/* val == 1 external */
	if( !drvM6802_set_clockSource(ptaskConfig, 1) ) {
		epicsPrintf("\n>>> devM6802_set_ExtClock : drvM6802_set_clockSource(1) ... error\n");
		return ;
	}


	epicsPrintf("control thread (External Clk): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
    epicsThreadGetNameSelf());
}

LOCAL void devM6802_set_IntlClock(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

/* val == 0 internal */
	if( !drvM6802_set_clockSource(ptaskConfig, 0) ) {
		epicsPrintf("\n>>> devM6802_set_IntClock : drvM6802_set_clockSource(0) ... error\n");
		return ;
	}


	epicsPrintf("control thread (Internal Clk): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
    epicsThreadGetNameSelf());
}

LOCAL void devM6802_set_setOPmode(execParam *pParam)
{
	g_OpMode = (int)pParam->setValue;
	switch(g_OpMode) {
	case OPMODE_REMOTE:
		epicsPrintf("\n>>> Operation mode : Remote mode \n");
		break;
	case OPMODE_CALIBRATION:
		epicsPrintf("\n>>> Operation mode : Calibration mode \n");
		break;
	case OPMODE_TEST:
		epicsPrintf("\n>>> Operation mode : Test mode \n");
		break;
	default:
		epicsPrintf("\n>>> devM6802_set_setOPmode( %d )... error!\n", g_OpMode);
		break;
	}
}

LOCAL void devM6802_set_shotNum(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	ptaskConfig->shotNumber = (uint32)pParam->setValue;

	g_ShotNumber = (int)pParam->setValue;
	epicsPrintf("\n>>> devM6802_set_shotNum : %d \n", g_ShotNumber);

	epicsPrintf("control thread (set ShotNumber): %s %s (%s), %u sec\n", ptaskConfig->taskName, precord->name,
    										epicsThreadGetNameSelf(), ptaskConfig->shotNumber);
}

LOCAL void devM6802_set_CreateShot(execParam *pParam)
{
	if( !createNewShot( g_ShotNumber ) )
	{
		epicsPrintf("\n>>> devM6802_set_CreateShot : createNewShot(\"%d\") ... error\n", g_ShotNumber);
		return ;
	}

	fprintf(stdout,"\n>>> devM6802_set_CreateShot : createNewShot(\"%d\")... OK\n", g_ShotNumber);

}

LOCAL void devM6802_set_ADCstart(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	
	/* Initialization need, run only first time... */
/*	printf("\n ***** first ioc ADCstart \n"); */

	if (systemStatus & TASK_STORAGE_FAIL )
        {
                systemStatus &= ~TASK_STORAGE_FAIL;
                epicsPrintf("\n>>> devM6802_set_ADCstart : TASK STORAGE Fail Reset! We did not acquire the data. May be DMA count is 0  \n");
		scanIoRequest(ptaskConfig->ioScanPvt);
	}	
	if( !drvM6802_set_DAQready(ptaskConfig) )
	{
		epicsPrintf("\n>>> devM6802_set_ADCstart : drvM6802_set_DAQready... failed\n");
		return ;
	}

	systemStatus = TASK_NOT_INIT;
	systemStatus |= TASK_ADC_STARTED;
	
	scanIoRequest(ptaskConfig->ioScanPvt);

}

LOCAL void devM6802_set_ADCstop(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	
	printf("\n ***** ADC Stop Command !!!!!!!! \n"); 
	
	if( systemStatus & TASK_DAQ_STARTED ) 
	{
		epicsPrintf("\n>>> devM6802_set_ADCstop : DAQ must be stop! \n");
		return;
	}
		

	if( systemStatus & TASK_ADC_STARTED )
	{
		if( !drvM6802_set_DAQclear(ptaskConfig) )
		{
			epicsPrintf("\n>>> devM6802_set_ADCstop : drvM6802_set_DAQclear... failed\n");
			return ;
		}

		systemStatus &= ~TASK_ADC_STARTED;
		systemStatus |= TASK_ADC_STOPED;
		systemStatus |= TASK_INITIALIZED;
	}
	else {
		epicsPrintf("\n>>> devM6802_set_ADCstop : ADC not started! \n");
	}
	
	scanIoRequest(ptaskConfig->ioScanPvt);
}


LOCAL void devM6802_set_DAQstart(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	if( systemStatus & TASK_ADC_STARTED )
	{
/*  woong edit. 2007.08.13. sync to first DMA done message */
		if( !drvM6802_set_DAQstart(ptaskConfig) )
			return ;
		

		start = drvM6802_getCurrentUsec();

		systemStatus |= TASK_DAQ_STARTED;

	} else {
		epicsPrintf("\n>>> devM6802_set_DAQstart : ADC must be start! \n");
	}

	epicsPrintf("control thread (DAQ started): %s %s (%s)\n", ptaskConfig->taskName, precord->name, epicsThreadGetNameSelf());

	scanIoRequest(ptaskConfig->ioScanPvt);

}

LOCAL void devM6802_set_DAQstop(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	
/*	printf("\n ***** first ioc DAQstop \n"); */
/*	DPIO 2 Error Test 2009.05.20 100KHz 300sec ACQ Error		*/
	printf("\n ***** End DAQ stop Command !!!! \n"); 
	epicsPrintf("!!! set DAQ Stop command Action start....!!!!! 2009.05.20   \n");
	if ( systemStatus & TASK_DAQ_STARTED )
	{
		stop = drvM6802_getCurrentUsec();

		if( !drvM6802_set_DAQstop(ptaskConfig) ) {
			epicsPrintf("\n>>> devM6802_set_DAQstop : drvM6802_set_DAQstop(ptaskConfig) ... fail \n");
			return;
		}

		_interval = drvM6802_intervalUSec(start, stop);
		epicsPrintf("\nDAQ time is %lf msec\n", 1.E-3 * (double)_interval);


		systemStatus &= ~TASK_DAQ_STARTED;
		systemStatus |= TASK_DAQ_STOPED;
	} 
	else {
		epicsPrintf("\n>>> devM6802_set_DAQstop : DAQ not started! \n");
	}


	epicsPrintf("control thread (DAQ STOP): %s %s (%s)\n", ptaskConfig->taskName, precord->name, epicsThreadGetNameSelf());

	scanIoRequest(ptaskConfig->ioScanPvt);
}
LOCAL void devM6802_set_AvrEceChannel(execParam *pParam)
{
#if 1 

	unsigned long long int t0ch, t1ch;
	t0ch = drvM6802_getCurrentUsec();
	
	if( !channelAverageForECE() )
	{
		epicsPrintf("\n>>> devM6802_set_AvrEceChannel : channelAverageForECE() ... fail\n");
		return;
	}

	t1ch = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(t0ch, t1ch);
	epicsPrintf("\nData Cnannelization time is %lf msec\n", 1.E-3 * (double)_interval);

}
#endif 
/* Modify 20080930-  Blip Time Infomation Transport LocalStorage and RemoteStorage Function : Add g_BlipTime at sendEDtoTreeFromChannelData */
LOCAL void devM6802_set_LocalStorage(execParam *pParam)
{
#if 1 

	unsigned long long int t0ch, t1ch;
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	t0ch = drvM6802_getCurrentUsec();

	systemStatus = TASK_NOT_INIT;
	
	systemStatus |= TASK_LOCAL_STORAGE;
	scanIoRequest(ptaskConfig->ioScanPvt);
	
	if( !dataChannelizationAndSendData(g_ShotNumber, g_OpMode) )
	{
		epicsPrintf("\n>>> devM6802_set_LocalStorage : dataChannelizationAndSendData() ... fail\n");
		systemStatus = TASK_NOT_INIT;
		systemStatus |= TASK_STORAGE_FAIL;
		scanIoRequest(ptaskConfig->ioScanPvt);
		return;
	}
	systemStatus &= ~TASK_LOCAL_STORAGE;
	epicsPrintf("\n>>> devM6802_set_LocalStorage : dataChannelization() ... DONE\n");

	t1ch = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(t0ch, t1ch);
	epicsPrintf("\nData Cnannelization time is %lf msec\n", 1.E-3 * (double)_interval);

	systemStatus = TASK_INITIALIZED;

	scanIoRequest(ptaskConfig->ioScanPvt);

#endif 




#if 0		/* Delete TG.Lee dataChannelizationAndSendData Function Used Time */
	unsigned long long int t0, t1, t0ch, t1ch;
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	t0ch = drvM6802_getCurrentUsec();
	epicsPrintf("Now, do channelization\n");

	systemStatus = TASK_NOT_INIT;
	systemStatus |= TASK_CHANNELIZATION;
	scanIoRequest(ptaskConfig->ioScanPvt);
	
	if( !dataChannelization() )
	{
		epicsPrintf("\n>>> devM6802_set_LocalStorage : dataChannelization() ... fail\n");
		systemStatus = TASK_NOT_INIT;
		systemStatus |= TASK_CHANNELIZATION_FAIL;
		scanIoRequest(ptaskConfig->ioScanPvt);
		return;
	}
	systemStatus &= ~TASK_CHANNELIZATION;
	epicsPrintf("\n>>> devM6802_set_LocalStorage : dataChannelization() ... DONE\n");
	t1ch = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(t0ch, t1ch);
	epicsPrintf("\nData Cnannelization time is %lf msec\n", 1.E-3 * (double)_interval);

	epicsPrintf("Now, transfer to local model tree\n");
	systemStatus |= TASK_LOCAL_STORAGE;
	scanIoRequest(ptaskConfig->ioScanPvt);

	t0 = drvM6802_getCurrentUsec();
	if( !sendEDtoTreeFromChannelData(g_ShotNumber, g_OpMode) )
	{
		epicsPrintf("\n>>> devM6802_set_LocalStorage : sendEDtoTreeFromChannelData() ... fail\n");
		systemStatus = TASK_NOT_INIT;
		systemStatus &= TASK_STORAGE_FAIL;
		scanIoRequest(ptaskConfig->ioScanPvt);
		return ;
	}
	systemStatus &= ~TASK_LOCAL_STORAGE;
	epicsPrintf("\n>>> devM6802_set_LocalStorage : sendEDtoTreeFromChannelData() ... DONE\n");

	systemStatus = TASK_INITIALIZED;

	t1 = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(t0, t1);
	epicsPrintf("\nMDSplus Transfer time is %lf msec\n", 1.E-3 * (double)_interval);
	scanIoRequest(ptaskConfig->ioScanPvt);
#endif
#if 0
	start = drvM6802_getCurrentUsec();

	epicsPrintf("Now, checking data miss\n");
	if( !checkDataMiss() )
	{
		epicsPrintf("\n>>> devM6802_set_DAQstop : checkDataMiss() ... fail\n");
		return ;
	}
	epicsPrintf("\n>>> devM6802_set_DAQstop : checkDataMiss() ... OK\n");
/*
	epicsPrintf("Now, transfer to local model tree\n");
	systemStatus &= ~TASK_INITIALIZED;
	systemStatus |= TASK_STORAGE;
	if( !sendEDtoTreeFromBigFile(g_ShotNumber) )
	{
		epicsPrintf("\n>>> devM6802_set_DAQstop : sendEDtoTreeFromBigFile() ... fail\n");
		systemStatus |= TASK_INITIALIZED;
		systemStatus &= ~TASK_STORAGE;
		return ;
	}
	systemStatus &= ~TASK_STORAGE;
	epicsPrintf("\n>>> devM6802_set_DAQstop : sendEDtoTreeFromBigFile() ... DONE\n");

	systemStatus |= TASK_INITIALIZED;

	stop = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(start, stop);
	epicsPrintf("\nbig dump time is %lf msec\n", 1.E-3 * (double)_interval);
*/

#endif

}

/* CLTU opi always call this function when DAQ stoped */
/* Modify 20080930 -  Blip Time Infomation Transport LocalStorage and RemoteStorage Function : Add g_BlipTime at sendEDtoTreeFromChannelData */
LOCAL void devM6802_set_RemoteStorage(execParam *pParam)
{
#if 1 
	unsigned long long int t0ch, t1ch;
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	t0ch = drvM6802_getCurrentUsec();

	systemStatus = TASK_NOT_INIT;
	
	systemStatus |= TASK_REMOTE_STORAGE;
	scanIoRequest(ptaskConfig->ioScanPvt);
	
	if( !dataChannelizationAndSendData(g_ShotNumber, g_OpMode) )
	{
		epicsPrintf("\n>>> devM6802_set_RemoteStorage : sendEDtoTreeFromChannelData() ... fail\n");
		systemStatus = TASK_NOT_INIT;
		systemStatus |= TASK_STORAGE_FAIL;
		scanIoRequest(ptaskConfig->ioScanPvt);
		return ;
	}
	systemStatus &= ~TASK_REMOTE_STORAGE;
	epicsPrintf("\n>>> devM6802_set_RemoteStorage : sendEDtoTreeFromChannelData() ... DONE\n");

	systemStatus = TASK_INITIALIZED;

	t1ch = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(t0ch, t1ch);
	epicsPrintf("\nData Cnannelization And Transfer time is %lf msec\n", 1.E-3 * (double)_interval);

	scanIoRequest(ptaskConfig->ioScanPvt);
#endif 

#if 0
/*  Modify TG.Lee Current time channelization and Transfer time */
	unsigned long long int t0, t1, t0ch, t1ch;
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;

	t0ch = drvM6802_getCurrentUsec();/*  Modify TG.Lee Current time channelization and Transfer time */
	if( g_OpMode == OPMODE_REMOTE ) 
	{
		epicsPrintf("Now, do channelization\n");
	
		systemStatus = TASK_NOT_INIT;
		systemStatus |= TASK_CHANNELIZATION;
		scanIoRequest(ptaskConfig->ioScanPvt);
		
		if( !dataChannelization() )
		{
			epicsPrintf("\n>>> devM6802_set_LocalStorage : dataChannelization() ... fail\n");
			systemStatus = TASK_NOT_INIT;
			systemStatus |= TASK_CHANNELIZATION_FAIL;
			scanIoRequest(ptaskConfig->ioScanPvt);
			return;
		}
		systemStatus &= ~TASK_CHANNELIZATION;
		epicsPrintf("\n>>> devM6802_set_LocalStorage : dataChannelization() ... DONE\n");
	}
	/*  Modify TG.Lee Current time channelization and Transfer time */
	t1ch = drvM6802_getCurrentUsec();
	_interval = drvM6802_intervalUSec(t0ch, t1ch);
	epicsPrintf("\nChannelization time is %lf msec\n", 1.E-3 * (double)_interval);

	epicsPrintf("Now, transfer to remote model tree\n");
	systemStatus |= TASK_REMOTE_STORAGE;
	scanIoRequest(ptaskConfig->ioScanPvt);
	
	t0 = drvM6802_getCurrentUsec(); /*  Modify TG.Lee Current time channelization and Transfer time */
	if( !sendEDtoTreeFromChannelData(g_ShotNumber, OPMODE_REMOTE ) )
	{
		epicsPrintf("\n>>> devM6802_set_RemoteStorage : sendEDtoTreeFromChannelData() ... fail\n");
		systemStatus = TASK_NOT_INIT;
		systemStatus |= TASK_STORAGE_FAIL;
		scanIoRequest(ptaskConfig->ioScanPvt);
		return ;
	}
	t1 = drvM6802_getCurrentUsec();

	systemStatus &= ~TASK_REMOTE_STORAGE;
	epicsPrintf("\n>>> devM6802_set_RemoteStorage : sendEDtoTreeFromChannelData() ... DONE\n");

	systemStatus = TASK_INITIALIZED;

	_interval = drvM6802_intervalUSec(t0, t1);
	epicsPrintf("\nTransfer time is %lf msec\n", 1.E-3 * (double)_interval);

	scanIoRequest(ptaskConfig->ioScanPvt);
#endif 
}


LOCAL void devM6802_set_tagName(execParam *pParam)
{
/*	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig; */
	if( pParam->channelID > 255 ) {
		epicsPrintf("\n>>> input channel limit : 255\n");
		return;
	}
/*
	strcpy(channelTagName[pParam->channelID], "\\");
	strcat(channelTagName[pParam->channelID], pParam->setStr);
*/
	strcpy(channelTagName[pParam->channelID], pParam->setStr);

#if 0
	epicsPrintf("control thread (tag Name): %s (%d: %s)\n", ptaskConfig->taskName, pParam->channelID, pParam->setStr );
#endif
}

LOCAL void devM6802_set_chOnOff(execParam *pParam)
{
	int i, ntemp, accumNum=0;
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	int dval = (int)pParam->setValue;

	switch( ptaskConfig->BoardID )
	{
		case 1: accumNum = 0; break;
		case 2: accumNum = 32; break;
		case 3: accumNum = 64; break;
		case 4: accumNum = 96; break;
		case 5: accumNum = 128; break;
		case 6: accumNum = 160; break;
		case 7: accumNum = 192; break;
		case 8: accumNum = 224; break;
		default: 
			epicsPrintf("\n>>> devM6802_set_chOnOff(%s): BoardID %d... error\n", ptaskConfig->taskName, ptaskConfig->BoardID );
			return;
	}


	for(i=0; i<32; i++)
	{
		ntemp = 0x0;
		ntemp |=  0x1 << i;
		channelOnOff[accumNum+i] = ( dval & ntemp ) ? 1 : 0;
#if 0
		if( channelOnOff[accumNum+i] ) {
			epicsPrintf(" %s:  ch%d ON\n", ptaskConfig->taskName, i+1 );
			epicsThreadSleep(0.01);
		}
#endif
	}

}


LOCAL void devM6802_set_samplingRate(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	drvM6802_set_samplingRate(pParam->setValue); /* TG.Lee Comment Remove and Test 08.1121 */ 
	ptaskConfig->appSampleRate = pParam->setValue;
	epicsPrintf("control thread (app smapling rate): %s (%s), %dKHz\n", precord->name,
			                                            epicsThreadGetNameSelf(), 
																		ptaskConfig->appSampleRate);
}

/*
LOCAL void devM6802_set_chanGain(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	drvM6802_set_chanGain(pParam->ptaskConfig, pParam->channelID, pParam->setValue);
	epicsPrintf("control thread (gain): %s %s %d (%s)\n", ptaskConfig->taskName, precord->name, (int)pParam->setValue,
			                                   epicsThreadGetNameSelf());
}
*/

LOCAL void devM6802_set_chanMask(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;
	drvM6802_set_channelMask(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (Mask): %s %s %d (%s)\n", ptaskConfig->taskName, precord->name, (int)pParam->setValue,
			                                   epicsThreadGetNameSelf());
}
/*  Add Blip Start Time information. Modify TG.Lee at 20080930 */
/* taskConfig Value blipTime has BlipTime PV Value. */ 
LOCAL void devM6802_set_blipTime(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;
        struct dbCommon *precord = pParam->precord;
        ptaskConfig->blipTime = (float)pParam->setValue;
/*	g_BlipTime = (float)pParam->setValue;  Delete Globle Value Function TG.Lee -20081025
	epicsPrintf("\n>>> devM6802_set_blipTime : %f \n", g_BlipTime);  */
	epicsPrintf("control thread (app set blipTime): %s (%s), %f sec \n",
							 precord->name,
                                                         epicsThreadGetNameSelf(),
                                                	 ptaskConfig->blipTime);
}

/*
LOCAL void devM6802_set_dataConvert(execParam *pParam)
{
	drvM6802_taskConfig *ptaskConfig = pParam->ptaskConfig;

	if( (ptaskConfig->taskStatus & TASK_ADC_STARTED) )
	{
		epicsPrintf("\n>>> devM6802_set_dataConvert : ADC still Running\n");
		return ;
	}

	if( !dataChannelization() )
	{
		epicsPrintf("\n>>> devM6802_set_dataConvert : dataChannelization() ... fail\n");
		return ;
	}
	epicsPrintf("\n>>> devM6802_set_dataConvert : dataChannelization() ... DONE\n");

	return ;
}
*/

LOCAL long devAoM6802Control_init_record(aoRecord *precord)
{
	devAoM6802Controldpvt *pdevAoM6802Controldpvt = (devAoM6802Controldpvt*) malloc(sizeof(devAoM6802Controldpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevAoM6802Controldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s",
				   pdevAoM6802Controldpvt->taskName,
			           pdevAoM6802Controldpvt->option1);

#ifdef DEBUG
	epicsPrintf("devAoM6802Control arg num: %d: %s %s\n",i, pdevAoM6802Controldpvt->taskName, 
								pdevAoM6802Controldpvt->option1);
#endif
			pdevAoM6802Controldpvt->ptaskConfig = drvM6802_find_taskConfig(pdevAoM6802Controldpvt->taskName);
			if(!pdevAoM6802Controldpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoM6802Control (init_record) Illegal INP field: task_name");
				free(pdevAoM6802Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoM6802Control (init_record) Illegal OUT field");
			free(pdevAoM6802Controldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SAMPLING_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SAMPLING;
/*		pdevAoM6802Controldpvt->pchannelConfig 
			= (drvM6802_channelConfig*)ellFirst(pdevAoM6802Controldpvt->ptaskConfig->pchannelConfig); */
	} else if(!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_GAIN_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_GAIN;
/*
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_DAQ_START_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_DAQ_START;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_DAQ_STOP_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_DAQ_STOP;


	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_ADC_START_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_ADC_START;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_ADC_STOP_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_ADC_STOP;
*/
/* Move at BO Record Process Port  20081024  TG.Lee
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_LOCAL_STORAGE_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_LOCAL_STORAGE;

	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_REMOTE_STORAGE_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_REMOTE_STORAGE;
*/
/*
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_DATA2ASCII_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_DATA2ASCII;
		pdevAoM6802Controldpvt->pchannelConfig
			= (drvM6802_channelConfig*)ellFirst(pdevAoM6802Controldpvt->ptaskConfig->pchannelConfig);
*/
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_MASK_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_MASK;
/*	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_TREE_WRITE_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_TREE_WRITE;
		pdevAoM6802Controldpvt->pchannelConfig
			= (drvM6802_channelConfig*)ellFirst(pdevAoM6802Controldpvt->ptaskConfig->pchannelConfig);
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_TREE_READ_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_TREE_READ;
		pdevAoM6802Controldpvt->pchannelConfig
			= (drvM6802_channelConfig*)ellFirst(pdevAoM6802Controldpvt->ptaskConfig->pchannelConfig);
*/
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_ONOFF_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_ONOFF;

	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SHOTNUM_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SHOTNUM;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_CREATESHOT_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_CREATESHOT;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SET_MODE_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SET_MODE;
/*	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SET_INTERCLK_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SET_INTERCLK;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SET_EXTERCLK_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SET_EXTERCLK;
*/
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SET_ECEMODE_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SET_ECEMODE;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SET_T0_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SET_T0;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_SET_T1_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_SET_T1;
/*	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_TRIG_EXTER_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_TRIG_EXTER;
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_TRIG_INTER_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_TRIG_INTER_STR;
*/      /* Add Blip Start Time information. Modify TG.Lee at 20080929 */
	} else if (!strcmp(pdevAoM6802Controldpvt->option1, CONTROL_BLIP_TIME_STR)) {
		pdevAoM6802Controldpvt->ind = CONTROL_BLIP_TIME;
	}


	systemStatus |= TASK_INITIALIZED;
/*	systemStatus |= TASK_DAQ_STOPED;  TGLEE */
/*	systemStatus |= TASK_ADC_STOPED;  */

	for(i=0; i<256; i++)
		channelOnOff[i] = 1;


	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAoM6802Controldpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoM6802Control_write_ao(aoRecord *precord)
{
	devAoM6802Controldpvt        *pdevAoM6802Controldpvt = (devAoM6802Controldpvt*) precord->dpvt;
	drvM6802_taskConfig          *ptaskConfig;
	drvM6802_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;



	if(!pdevAoM6802Controldpvt || precord->udf == TRUE) {
		precord->pact = FALSE;  /* pact is TRUE -> Record Processing (Not Completed) */
		return 0;
	}

	ptaskConfig			= pdevAoM6802Controldpvt->ptaskConfig;
	pcontrolThreadConfig		= ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig		= ptaskConfig;
	qData.param.precord		= (struct dbCommon *)precord;
/*	qData.param.channelID		= pdevAoM6802Controldpvt->pchannelConfig->chanIndex; */
	qData.param.setValue		= precord->val;



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;


		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());

		switch(pdevAoM6802Controldpvt->ind) {
			case CONTROL_SAMPLING:
				qData.pFunc = devM6802_set_samplingRate;
				break;
/*			case CONTROL_GAIN:
				qData.pFunc = devM6802_set_chanGain;
				break;
*/
/* Move at BO Record Process  20081024 TG.Lee 
			case CONTROL_DAQ_START:
				qData.pFunc = devM6802_set_DAQstart;
				break;
			case CONTROL_DAQ_STOP:
				qData.pFunc = devM6802_set_DAQstop;
				break;
			case CONTROL_ADC_START:
				qData.pFunc = devM6802_set_ADCstart;
				break;
			case CONTROL_ADC_STOP:
				qData.pFunc = devM6802_set_ADCstop;
				break;
			case CONTROL_LOCAL_STORAGE:
				qData.pFunc = devM6802_set_LocalStorage;
				break;
			case CONTROL_REMOTE_STORAGE:
				qData.pFunc = devM6802_set_RemoteStorage;
				break;
*/
/*			case CONTROL_DATA2ASCII:
				qData.pFunc = devM6802_set_dataConvert;
				break;
*/			case CONTROL_MASK:
				qData.pFunc = devM6802_set_chanMask;
				break;
/*			case CONTROL_TREE_WRITE:
				qData.pFunc = devM6802_tree_dataWrite;
				break;
			case CONTROL_TREE_READ:
				qData.pFunc = devM6802_tree_dataRead;
				break;
*/			case CONTROL_ONOFF:
				qData.pFunc = devM6802_set_chOnOff;
				break;
			case CONTROL_SHOTNUM:
				qData.pFunc = devM6802_set_shotNum;
				break;
			case CONTROL_CREATESHOT:
				qData.pFunc = devM6802_set_CreateShot;
				break;
			case CONTROL_SET_MODE:
				qData.pFunc = devM6802_set_setOPmode;
				break;
/* Move at BO Record Process  20081024 TG.Lee 
			case CONTROL_SET_INTERCLK:
				qData.pFunc = devM6802_set_IntlClock;
				break;
			case CONTROL_SET_EXTERCLK:
				qData.pFunc = devM6802_set_ExtlClock;
				break;
*/	
			case CONTROL_SET_ECEMODE:
				qData.pFunc = devM6802_set_ECECalMode;
				break;
			case CONTROL_SET_T0:
				qData.pFunc = devM6802_set_T0;
				break;
			case CONTROL_SET_T1:
				qData.pFunc = devM6802_set_T1;
				break;
/* Move at BO Record Process  20081024 TG.Lee 
				
			case CONTROL_TRIG_EXTER:
				qData.pFunc = devM6802_set_ExtlTrigger;
				break;
			case CONTROL_TRIG_INTER:
				qData.pFunc = devM6802_set_IntlTrigger;
				break;
*/
/* Add Blip Start Time information. Modify TG.Lee at 20080929 */
			case CONTROL_BLIP_TIME:
				qData.pFunc = devM6802_set_blipTime;
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
/*		stop = drvM6802_getCurrentUsec(); */
		return 2;    /* don't convert */
	}

	return 0;
}

/* Bo Record Editing  Start Point  TG.Lee 20081024 */


LOCAL long devBoM6802Control_init_record(boRecord *precord)
{
        devBoM6802Controldpvt *pdevBoM6802Controldpvt = (devBoM6802Controldpvt*) malloc(sizeof(devBoM6802Controldpvt));
        int i;

        switch(precord->out.type) {
                case INST_IO:
                        strcpy(pdevBoM6802Controldpvt->recordName, precord->name);
                        i = sscanf(precord->out.value.instio.string, "%s %s %s",
                                   pdevBoM6802Controldpvt->taskName,
                                   pdevBoM6802Controldpvt->option1,
				   pdevBoM6802Controldpvt->chanName);
					
#ifdef DEBUG
        epicsPrintf("devBoM6802Control arg num: %d: %s %s\n",i, pdevBoM6802Controldpvt->taskName,
                                                                pdevBoM6802Controldpvt->option1);
#endif
                        pdevBoM6802Controldpvt->ptaskConfig = drvM6802_find_taskConfig(pdevBoM6802Controldpvt->taskName);
                        if(!pdevBoM6802Controldpvt->ptaskConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devBoM6802Control (init_record) Illegal INP field: task_name");
                                free(pdevBoM6802Controldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }


                        break;
                default:
                        recGblRecordError(S_db_badField,(void*) precord,
                                          "devBoM6802Control (init_record) Illegal OUT field");
                        free(pdevBoM6802Controldpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;

        }

	if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_LOCAL_STORAGE_STR)) {
                pdevBoM6802Controldpvt->ind = CONTROL_LOCAL_STORAGE;
        } else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_REMOTE_STORAGE_STR)) {
                pdevBoM6802Controldpvt->ind = CONTROL_REMOTE_STORAGE;
        } else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_AVR_ECE_STR)) {
                pdevBoM6802Controldpvt->ind = CONTROL_AVR_ECE;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_DAQ_START_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_DAQ_START;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_DAQ_STOP_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_DAQ_STOP;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_ADC_START_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_ADC_START;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_ADC_STOP_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_ADC_STOP;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_SET_INTERCLK_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_SET_INTERCLK;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_SET_EXTERCLK_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_SET_EXTERCLK;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_TRIG_EXTER_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_TRIG_EXTER;
	} else if (!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_TRIG_INTER_STR)) {
		pdevBoM6802Controldpvt->ind = CONTROL_TRIG_INTER;
	} else if(!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_BO_ZCAL_STR)) {
            	pdevBoM6802Controldpvt->ind = CONTROL_BO_ZCAL;
	}

/*	if (i==3) {
		if(!strcmp(pdevBoM6802Controldpvt->option1, CONTROL_BO_ZCAL_STR)) {
            	pdevBoM6802Controldpvt->pchannelConfig
                            = drvM6802_find_channelConfig(pdevBoM6802Controldpvt->taskName,
                                    pdevBoM6802Controldpvt->chanName);
                	if(!pdevBoM6802Controldpvt->pchannelConfig) {
            		recGblRecordError(S_db_badField, (void*) precord,
                    		"devBoM6802Control Zero Cal (init_record) Illegal INP field: channel_name");
            		free(pdevBoM6802Controldpvt);
            		precord->udf = TRUE;
            		precord->dpvt = NULL;
            		return S_db_badField;	
			}
		}

	}
*/
        systemStatus |= TASK_INITIALIZED;
/*      systemStatus |= TASK_DAQ_STOPED;  TGLEE */
/*      systemStatus |= TASK_ADC_STOPED;  */

        precord->udf = FALSE;
        precord->dpvt = (void*) pdevBoM6802Controldpvt;

        return 2;     /* don't convert */
}


/* Bo Initial Record Editing End Point 20081024 TG.Lee      */
/* Bo Write Record Editing Start Point 20081024 TG.Lee      */

LOCAL long devBoM6802Control_write_bo(boRecord *precord)
{
	int chanInfo;
        devBoM6802Controldpvt        *pdevBoM6802Controldpvt = (devBoM6802Controldpvt*) precord->dpvt;
        drvM6802_taskConfig          *ptaskConfig;
        drvM6802_controlThreadConfig *pcontrolThreadConfig;
        controlThreadQueueData         qData;
/* PV Record 0 -> return , 1 -> Process continue */
        if(pdevBoM6802Controldpvt->ind == CONTROL_BO_ZCAL ) {
        	if(!pdevBoM6802Controldpvt || precord->udf == TRUE ) {
/*              precord->pact = TRUE;    */
                precord->pact = FALSE;
		epicsPrintf("Write Action BO Record UDF is TRUE. or devBoM6802Controldpvt Value is Error !! \n");
                return 0;
        	}
	} else {
        	if(!pdevBoM6802Controldpvt || precord->udf == TRUE || precord->val == 0) {
/*              precord->pact = TRUE;    */
                precord->pact = FALSE;
		epicsPrintf("Write Action BO Record Value is ONE. Zero is Not Action. Only Record value Changed !! \n");
                return 0;
        	}

	}

        ptaskConfig                     = pdevBoM6802Controldpvt->ptaskConfig;
        pcontrolThreadConfig            = ptaskConfig->pcontrolThreadConfig;
        qData.param.ptaskConfig         = ptaskConfig;
        qData.param.precord             = (struct dbCommon *)precord;
/*      qData.param.channelID           = pdevBoM6802Controldpvt->pchannelConfig->chanIndex; */
        qData.param.setValue            = precord->val;

        /* db processing: phase I */
        if(precord->pact == FALSE) {
                precord->pact = TRUE;

                epicsPrintf("db processing: phase I %s Status2 %d (%s)\n", precord->name,precord->val,
						epicsThreadGetNameSelf());
/* Move at BO Record Process  20081024 TG.Lee    */ 
                switch(pdevBoM6802Controldpvt->ind) {
			case CONTROL_LOCAL_STORAGE:
                                qData.pFunc = devM6802_set_LocalStorage;
                                break;
			case CONTROL_AVR_ECE:
                                qData.pFunc = devM6802_set_AvrEceChannel;
                                break;
                        case CONTROL_REMOTE_STORAGE:
                                qData.pFunc = devM6802_set_RemoteStorage;
                                break;
			case CONTROL_DAQ_START:
				qData.pFunc = devM6802_set_DAQstart;
				break;
			case CONTROL_DAQ_STOP:
				qData.pFunc = devM6802_set_DAQstop;
				break;
			case CONTROL_ADC_START:
				qData.pFunc = devM6802_set_ADCstart;
				break;
			case CONTROL_ADC_STOP:
				qData.pFunc = devM6802_set_ADCstop;
				break;
			case CONTROL_SET_INTERCLK:
				qData.pFunc = devM6802_set_IntlClock;
				break;
			case CONTROL_SET_EXTERCLK:
				qData.pFunc = devM6802_set_ExtlClock;
				break;
			case CONTROL_TRIG_EXTER:
				qData.pFunc = devM6802_set_ExtlTrigger;
				break;
			case CONTROL_TRIG_INTER:
				qData.pFunc = devM6802_set_IntlTrigger;
				break;
			case CONTROL_BO_ZCAL:
				chanInfo = atoi(pdevBoM6802Controldpvt->chanName);
				ptaskConfig->chInfo = chanInfo;
				ptaskConfig->zCal = precord->val; 
			 	qData.pFunc = devM6802_set_ZeroCalibration; 
				break;
		}

                epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
                                      (void*) &qData,
                                      sizeof(controlThreadQueueData));

                return 0;
        }

        /* db processing: phase II -post processing */
        if(precord->pact == TRUE) {
                epicsPrintf("db processing: phase II %s Status2 %d (%s)\n", precord->name,precord->val,
                                                                 epicsThreadGetNameSelf());
                precord->pact = FALSE;
                precord->udf = FALSE;
		
/*              stop = drvM6802_getCurrentUsec(); */
                return 2;    /* don't convert */
        }

        return 0;
}


/* Bo Write Record Editing End Point 20081024 TG.Lee      */



LOCAL long devStringoutM6802Control_init_record(stringoutRecord *precord)
{
	devStringoutM6802Controldpvt *pdevStringoutM6802Controldpvt = (devStringoutM6802Controldpvt*) malloc(sizeof(devStringoutM6802Controldpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevStringoutM6802Controldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevStringoutM6802Controldpvt->taskName,
			           pdevStringoutM6802Controldpvt->option1 ,
			           pdevStringoutM6802Controldpvt->option2);

#ifdef DEBUG
	epicsPrintf("devAoM6802Control arg num: %d: %s %s\n",i, pdevStringoutM6802Controldpvt->taskName, 
								pdevStringoutM6802Controldpvt->option1);
#endif
			pdevStringoutM6802Controldpvt->ptaskConfig = drvM6802_find_taskConfig(pdevStringoutM6802Controldpvt->taskName);
			if(!pdevStringoutM6802Controldpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devStringoutM6802Control (init_record) Illegal INP field: task_name");
				free(pdevStringoutM6802Controldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devStringoutM6802Control (init_record) Illegal OUT field");
			free(pdevStringoutM6802Controldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}


	if(!strcmp(pdevStringoutM6802Controldpvt->option1, CONTROL_TAG_STR)) {
		pdevStringoutM6802Controldpvt->ind = CONTROL_TAG;
		pdevStringoutM6802Controldpvt->chennelID = atoi( pdevStringoutM6802Controldpvt->option2 );

	} 

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevStringoutM6802Controldpvt;
	
	return 2;     /* don't convert */
}


LOCAL long devStringoutM6802Control_write_stringout(stringoutRecord *precord)
{
	devStringoutM6802Controldpvt        *pdevStringoutM6802Controldpvt = (devStringoutM6802Controldpvt*) precord->dpvt;
	drvM6802_taskConfig          *ptaskConfig;
	drvM6802_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;



	if(!pdevStringoutM6802Controldpvt || precord->udf == TRUE) {
		precord->pact = FALSE;  /* pact is TRUE = Record Processing(Not Completed) */
		return 0;
	}

	ptaskConfig			= pdevStringoutM6802Controldpvt->ptaskConfig;
	pcontrolThreadConfig		= ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig		= ptaskConfig;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.channelID		= pdevStringoutM6802Controldpvt->chennelID;
/*	qData.param.setValue		= precord->val; */
	strcpy( qData.param.setStr, precord->val );



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		switch(pdevStringoutM6802Controldpvt->ind) {
			case CONTROL_TAG:
				qData.pFunc = devM6802_set_tagName;
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
		return 2;    /* don't convert */
	}

	return 0;
}


LOCAL long devMbbiM6802RawRead_init_record(mbbiRecord *precord)
{
	devMbbiM6802RawReaddpvt *pdevMbbiM6802RawReaddpvt = (devMbbiM6802RawReaddpvt*) malloc(sizeof(devMbbiM6802RawReaddpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevMbbiM6802RawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevMbbiM6802RawReaddpvt->taskName,
				   pdevMbbiM6802RawReaddpvt->option1,
				   pdevMbbiM6802RawReaddpvt->option2);
#ifdef DEBUG
	epicsPrintf("devMbbiM6802RawRead arg num: %d: %s %s %s\n",i, pdevMbbiM6802RawReaddpvt->taskName, 
								pdevMbbiM6802RawReaddpvt->option1,
								pdevMbbiM6802RawReaddpvt->option2);
#endif
			pdevMbbiM6802RawReaddpvt->ptaskConfig = drvM6802_find_taskConfig(pdevMbbiM6802RawReaddpvt->taskName);
			if(!pdevMbbiM6802RawReaddpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devMbbiM6802RawRead (init_record) Illegal INP field: task name");
				free(pdevMbbiM6802RawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
/*			
			if( i > 2)
			{
				pdevMbbiM6802RawReaddpvt->pchannelConfig = drvM6802_find_channelConfig(pdevMbbiM6802RawReaddpvt->taskName,
											         pdevMbbiM6802RawReaddpvt->option1);
				if(!pdevMbbiM6802RawReaddpvt->pchannelConfig) {
					recGblRecordError(S_db_badField, (void*) precord,
							  "devMbbiM6802RawRead (init_record) Illegal INP field: channel name");
					free(pdevMbbiM6802RawReaddpvt);
					precord->udf = TRUE;
					precord->dpvt = NULL;
					return S_db_badField;
				}
			}
*/
			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devMbbiM6802RawRead (init_record) Illegal INP field");
			free(pdevMbbiM6802RawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(!strcmp(pdevMbbiM6802RawReaddpvt->option1, READ_STATE_STR))
		pdevMbbiM6802RawReaddpvt->ind = READ_STATE;
	else if(!strcmp(pdevMbbiM6802RawReaddpvt->option2, READ_GAIN_STR))
		pdevMbbiM6802RawReaddpvt->ind = READ_GAIN;
	else if(!strcmp(pdevMbbiM6802RawReaddpvt->option2, READ_VAL_STR))
		pdevMbbiM6802RawReaddpvt->ind = READ_VAL;	
	else pdevMbbiM6802RawReaddpvt->ind = READ_VAL;
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevMbbiM6802RawReaddpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devMbbiM6802RawRead_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devMbbiM6802RawReaddpvt *pdevMbbiM6802RawReaddpvt = (devMbbiM6802RawReaddpvt*) precord->dpvt;
	drvM6802_taskConfig   *ptaskConfig;

	if(!pdevMbbiM6802RawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevMbbiM6802RawReaddpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devMbbiM6802RawRead_read_mbbi(mbbiRecord *precord)
{ 
	devMbbiM6802RawReaddpvt	*pdevMbbiM6802RawReaddpvt = (devMbbiM6802RawReaddpvt*) precord->dpvt;
	drvM6802_taskConfig		*ptaskConfig;
/*	drvM6802_channelConfig	*pchannelConfig; */
/*	double				*pdata; */


	if(!pdevMbbiM6802RawReaddpvt) return 0;

	ptaskConfig		= pdevMbbiM6802RawReaddpvt->ptaskConfig;
/*	pchannelConfig	= pdevMbbiM6802RawReaddpvt->pchannelConfig; */
	

	switch(pdevMbbiM6802RawReaddpvt->ind) {
/*		case READ_GAIN:
			precord->val = pchannelConfig->gain;
			break;
*/
		case READ_VAL:
			break;
		case READ_STATE:
			if(systemStatus & TASK_INITIALIZED) precord->val =1;
			else if(systemStatus & TASK_ADC_STOPED) precord->val =2;
			else if(systemStatus & TASK_DAQ_STOPED) precord->val =3;
			else if(systemStatus & TASK_ADC_STARTED) precord->val =4;
			else if(systemStatus & TASK_DAQ_STARTED) precord->val =5;
			else if(systemStatus & TASK_LOCAL_STORAGE) precord->val =6;
			else if(systemStatus & TASK_REMOTE_STORAGE) precord->val =7;
			else if(systemStatus & TASK_STORAGE_FAIL) precord->val =8;
			else precord->val =9;
			break;
		default:
			epicsPrintf("\n>>> %s : ind( %d ) ... ERROR! \n",ptaskConfig->taskName,pdevMbbiM6802RawReaddpvt->ind);
			break;
	}
	return (2);
}

