/*
         2010-03-20 : modify by TaeGu Lee  for vxi & visa

*/

#define DEBUG   1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include <epicsPrint.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <ellLib.h>
#include <callback.h>
#include <iocsh.h>
#include <taskwd.h>

#include <dbCommon.h>
#include <drvSup.h>
#include <devSup.h>
#include <recSup.h>
#include <devLib.h>
#include <dbScan.h>
#include <dbLock.h>
#include <epicsExit.h>

#include <epicsExport.h>

#include <visa.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
/* #include <nivxi.h>  Remove 20110705 at windows */

#include "drvVXI1564a.h"

/* change the varible for epoch time*/
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif



struct timezone
{
  int tz_minuteswest; /* minutes W of Greenwich */
  int tz_dsttime;     /* type of dst correction */
};




/* An example of Data readout using the MDSplus Data Access library (mdslib) */
int dtypeFloat = DTYPE_FLOAT; /* We are going to read a signal made of float values */
int dtypeDouble = DTYPE_DOUBLE; /* We are going to read a signal made of float64 values */
int dtypeLong = DTYPE_LONG; /* The returned value of SIZE() is an       integer */
int dtypeULong = DTYPE_ULONG;
int dtypeSigned = DTYPE_SHORT;

LOCAL drvVXI1564aConfig          *pdrvVXI1564aConfig = NULL;

epicsMessageQueueId controlStoreQueueID;

/* Function Complate Current Time Calc function */
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    // system time을 구하기
    GetSystemTimeAsFileTime(&ft);

    // unsigned 64 bit로 만들기
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    // 100nano를 1micro로 변환하기    tmpres /= 10;
    // epoch time으로 변환하기
    tmpres -= DELTA_EPOCH_IN_MICROSECS;    

    // sec와 micorsec으로 맞추기
    tv->tv_sec = (tmpres / 1000000UL);
    tv->tv_usec = (tmpres % 1000000UL);
  }

  // timezone 처리
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}



unsigned long long int drvVXI1564a_rdtsc(void)
{
	unsigned long long int x;
  struct timeval tv;
/* 	__asm__ volatile(".byte 0x0f, 0x31": "=A"(x));  */
  gettimeofday( &tv, NULL);
  x = tv.tv_sec;
  x = (x*1.E+6) + tv.tv_usec;
	return x;
}

double drvVXI1564a_intervalUSec(unsigned long long int start, unsigned long long int stop)
{
	return 1.E+6 * (double)(stop-start)/(double)(pdrvVXI1564aConfig->one_sec_interval);
}

static int send_dataChannelAndStore(drvVXI1564a_taskConfig *ptaskConfig);
static void init_task(drvVXI1564a_taskConfig *ptaskConfig);
static void set_taskExTrigger(drvVXI1564a_taskConfig *ptaskConfig, unsigned int exTrig); /* Ex Trig or soft Trig and Remote /Local Opeation Include */
static void set_taskCallback(drvVXI1564a_taskConfig *ptaskConfig);	/* Not used -- sem used. */
static void start_task(drvVXI1564a_taskConfig *ptaskConfig);
static void stop_task(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setStop);
static void abort_task(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setAbort);
static int regUpdate_task(drvVXI1564a_taskConfig *ptaskConfig);
static void init_aoChannel(drvVXI1564a_aoChannelConfig *paoChannelConfig);
#if 0
static void init_dataPool(drvVXI1564a_taskConfig *ptaskConfig);
static void set_treeServer(drvVXI1564a_taskConfig *ptaskConfig, char *treeServer);
static void set_treeName(drvVXI1564a_taskConfig *ptaskConfig, char *treeName);
static void set_treeNode(drvVXI1564a_aoChannelConfig *pchannelConfig, char *treeNode);
static void set_lowpassFilterALLTasks(float64 cutOffFreq);
static void set_samplingRateALLTasks(float64 samplingRate);
static void set_samplingRate(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplingRate);/* Sample Period  I don't know that so delay  */
static int send_dataTree(drvVXI1564a_taskConfig *ptaskConfig);
#endif
static void set_channelRange(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *pchannelConfig, uInt16 setRange); 
static void set_channelFilter(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *pchannelConfig, uInt16 setFilter);

static void set_sampleMode(drvVXI1564a_taskConfig *ptaskConfig, int sampleMode);

static void set_samplePeriod(drvVXI1564a_taskConfig *ptaskConfig, float64 samplePeriod);/* Sample Period (interval)  */
static void set_sampleCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 sampleCount);   /* Sample Count */
static void set_samplePreCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplePreCount); /* Sample Pre-Count:Pre-trigger */

static void set_shotNumber(drvVXI1564a_taskConfig *ptaskConfig, int shotNumber);
static void set_blipTime(drvVXI1564a_taskConfig *ptaskConfig, float64 blipTime);
static void set_daqTime(drvVXI1564a_taskConfig *ptaskConfig, float64 daqTime);


static void set_operMode(drvVXI1564a_taskConfig *ptaskConfig, unsigned int operMode);   /* Net Used   */
static void set_triggerType(drvVXI1564a_taskConfig *ptaskConfig, int triggerType);      /* Net Used   */

/* function prototypes  for vxi system add by TG.Lee */
static void reset(drvVXI1564a_taskConfig *ptaskConfig);
static void err_handler (drvVXI1564a_taskConfig *ptaskConfig, ViStatus err);

static int mdsConnectAndOpen(drvVXI1564a_taskConfig *ptaskConfig);
static int mdsDisconnectAndClose(drvVXI1564a_taskConfig *ptaskConfig);
static int create_newTree(drvVXI1564a_taskConfig *ptaskConfig);



static int make_drvConfig(void);
static drvVXI1564a_taskConfig* make_taskConfig(char *taskName, char *serverName, char *treeName);
static drvVXI1564a_taskConfig* find_taskConfig(char *taskName);
static drvVXI1564a_channelConfig* find_channelConfig(drvVXI1564a_taskConfig *ptaskConfig, char *chanName);
static void print_channelConfig(drvVXI1564a_taskConfig *ptaskConfig, int level);

static drvVXI1564a_controlThreadConfig* make_controlThreadConfig(drvVXI1564a_taskConfig *ptaskConfig);
static drvVXI1564a_controlThreadConfig* make_eventMonitorConfig(drvVXI1564a_taskConfig *ptaskConfig);
/*
static drvVXI1564a_controlThreadConfig* make_controlStoreConfig(drvVXI1564a_taskConfig *ptaskConfig);
*/
/*   Call Back Function - temp delete..........
static int32 _VI_FUNC taskCallback(ViSession taskHandle, ViEventType etype, ViEvent event, ViAddr *callbackData);
static int32 CVICALLBACK taskCallback(TaskHandle taskHandle, int32 everyNsampleEventType, uInt32 nSamples, void *callbackData);
*/
static void controlThread(void *param);
static void eventMonitorFunc(void *param);
/*
static void controlStoreFunc(void *param);
*/
/*
static void clearAllVXITasks(void);
*/
/*void clearAllVXITasks(void);*/
static void createVXITaskCallFunction(const iocshArgBuf *args);
static void createVXI1564aChannelCallFunction(const iocshArgBuf *args);



static const iocshArg createVXITaskArg0 = {"task name", iocshArgString};
static const iocshArg createVXITaskArg1 = {"server name", iocshArgString};
static const iocshArg createVXITaskArg2 = {"tree name", iocshArgString};
static const iocshArg* const createVXITaskArgs[] = { &createVXITaskArg0,
						     &createVXITaskArg1,
						     &createVXITaskArg2};
static const iocshFuncDef createVXITaskFuncDef = { "createVXITask",
 						   3,
					           createVXITaskArgs };

static const iocshArg createVXI1564aChannelArg0 = {"task name", iocshArgString};
static const iocshArg createVXI1564aChannelArg1 = {"channel name", iocshArgString};
static const iocshArg createVXI1564aChannelArg2 = {"channel num", iocshArgString};
static const iocshArg createVXI1564aChannelArg3 = {"channel node", iocshArgString};
static const iocshArg* const createVXI1564aChannelArgs[] = { &createVXI1564aChannelArg0,
	                                                    &createVXI1564aChannelArg1,
							    &createVXI1564aChannelArg2,
							    &createVXI1564aChannelArg3};
static const iocshFuncDef createVXI1564aChannelFuncDef = {"createVXI1564aChannel",
						   	 4,
							 createVXI1564aChannelArgs};


static void epicsShareAPI drvVXI1564aRegistrar(void)
{
    static int firstTime =1;

    if(firstTime) {
        firstTime = 0;
  	iocshRegister(&createVXITaskFuncDef, createVXITaskCallFunction);
	iocshRegister(&createVXI1564aChannelFuncDef, createVXI1564aChannelCallFunction);
    }
}

epicsExportRegistrar(drvVXI1564aRegistrar);



LOCAL long drvVXI1564a_io_report(int level);
LOCAL long drvVXI1564a_init_driver(void);

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvVXI1564a = {
       2,
       drvVXI1564a_io_report,
       drvVXI1564a_init_driver
};

epicsExportAddress(drvet, drvVXI1564a);

static void init_task(drvVXI1564a_taskConfig *ptaskConfig)
{
	ViStatus err = 0;
	unsigned short val;
	int valMem;

	ptaskConfig->samplingMaxRate = INIT_SAMPLE_RATE_VXI1564a;
	ptaskConfig->samplePeriodMax = INIT_SAMPLE_INTERVAL_MAX; 
	ptaskConfig->samplePeriodMin = INIT_SAMPLE_INTERVAL_MIN;
	ptaskConfig->opMode = INIT_OPERATION;
#if 0
	ptaskConfig->eventDone = 0;
	ptaskConfig->triggerSource = INIT_TRIGGER_SOURCE_REG; 
	ptaskConfig->sampleSource = INIT_SAMPLE_SOURCE_REG;
#endif
	viOpenDefaultRM(&ptaskConfig->taskRM);
#if 0
  	epicsPrintf("Init Task Function DefaultRM = %d\n",ptaskConfig->taskRM);
#endif 
	/* digitizer logical address switch = 40 (factory setting) */
	/*	viOpen(defaultRM,"VXI0::88::INSTR",VI_NULL,VI_NULL,&digitizer); */
	viOpen(ptaskConfig->taskRM,ptaskConfig->taskName,VI_NULL,VI_NULL,&ptaskConfig->taskHandle);

  err = hpe1564_init(ptaskConfig->taskName, VI_TRUE, VI_FALSE, &ptaskConfig->hpeHandle);


#if 0
  	epicsPrintf("Init Task Function taskHandle =%d\n",ptaskConfig->taskHandle);
#endif 

	/* reset the VT1563A/VT1564A  */
        reset(ptaskConfig);

	/********* read the digitizer's ID and Device Type registers *********/
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_MANU_ID,&ptaskConfig->idReg);      /*  00 - Manufacturer ID Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_DEVICE_TYPE,&ptaskConfig->devTypeReg); /*  02 - Device Type Register   */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#if 0
  	epicsPrintf("ID register = 0x%4X\n",ptaskConfig->idReg);
  	epicsPrintf("Device Type register = 0x%4X\n",ptaskConfig->devTypeReg);
#endif 


	/**************** read the digitizer's status register **************/
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_STATUS_CONTROL,&ptaskConfig->statusReg);  /*  04 - Status/Control Register -Memory Size */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

#if 0
	epicsPrintf("Status register = 0x%4X\n", ptaskConfig->statusReg);
#endif 

	val = ptaskConfig->statusReg;
	valMem = (int)(val & 0x0F00);
	switch( valMem ) {
	    	case 0 :    ptaskConfig->sampleMaxCount = 524287 	;     break; /* 4MB */
    		case 256 :  ptaskConfig->sampleMaxCount = 1048575  ;     break; /* 8MB */ 
    		case 512 :  ptaskConfig->sampleMaxCount = 2097151  ;     break; /* 16MB */
    		case 768 :  ptaskConfig->sampleMaxCount = 4194303  ;     break; /* 32MB */
    		case 1024 :  ptaskConfig->sampleMaxCount = 8388607 ;     break; /* 64MB */
    		case 1280 :  ptaskConfig->sampleMaxCount = 16777215 ;    break; /* 128MB */
		default : epicsPrintf("\n>>> SetSample Count : Memory Size: %d error \n", valMem);
        	return;
	}
#ifdef DEBUG
	epicsPrintf("Module Memory Size = %f\n", ptaskConfig->sampleMaxCount); 
#endif
/* Just read Register and Check the Register */
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_OFFSET,&ptaskConfig->offsetReg); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,&ptaskConfig->fifoHigh); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,&ptaskConfig->fifoLow); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
/* Write Registers -  Interrupt Control Register  */ 
 	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_INTR_CONTROL,&ptaskConfig->interruptControl); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_INTR_SOURCE,&ptaskConfig->interruptSource); 
 	       if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

/* Just read Register and Check the Register */
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH1,&ptaskConfig->cvtableCh1); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH2,&ptaskConfig->cvtableCh2); 
       		if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH3,&ptaskConfig->cvtableCh3); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH4,&ptaskConfig->cvtableCh4); 
  	        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_TAKEN_HIGH,&ptaskConfig->sampleTakenHigh); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_TAKEN_LOW,&ptaskConfig->sampleTakenLow); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

/* Write Registers -  Calibration Register  */ 
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CAL_ROM_ADDR,&ptaskConfig->calFlashRomAdr); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CAL_ROM_DATA,&ptaskConfig->calFlashRomData); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CAL_SOURCE,&ptaskConfig->calSource); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CACHE_COUNT,&ptaskConfig->cacheCount); 
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
/* untill Just read Register and Check the Register */

/* Write Registers -  Range, Filter Register  */ 
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_RANG_FILTER_12,&ptaskConfig->voltFilter12); /* 0x24 -Range, Filter Ch 1&2 Connect Reg */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_RANG_FILTER_34,&ptaskConfig->voltFilter34); /* 0x26 -Range, Filter Ch 3&4 Connect Reg */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_TRIG_INTR_LEVEL1,&ptaskConfig->triggerLevel); /* 0x28 -Range,Filter Ch 3&4 Connect Reg */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

/* Write Registers -  Sampling Rate, Pre-trigger, Sample count Register  */ 
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_PERIOD_HIGH,&ptaskConfig->sampleRateHigh); /* 0x30 -Sample Period High Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_PERIOD_LOW,&ptaskConfig->sampleRateLow);  /* 0x32 - Sample Period Low Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_PRE_TRIG_HIGH,&ptaskConfig->preTriggerHigh);  /* 0x34 - Pre-Trig Count High Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_PRE_TRIG_LOW,&ptaskConfig->preTriggerLow);  /* 0x36 - Pre-Trig Count Low Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_COUNT_HIGH,&ptaskConfig->sampleCountHigh);  /* 0x38 - Sample Count High Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_COUNT_LOW,&ptaskConfig->sampleCountLow);  /* 0x3A - Sample Count Low Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	
/* Write Registers -  Trig Source, Sample Source Register  */ 
	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_TRIG_CONTROL,&ptaskConfig->triggerSource);  /* 0x3C - Trig Source/Ctrl Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_CONTROL,&ptaskConfig->sampleSource);  /* 0x3E - Sample Source/Ctrl Register */
        	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
	
	epicsMutexLock(ptaskConfig->bufferLock);	
  	ptaskConfig->taskStatus |= TASK_INITIALIZED;
  	ptaskConfig->taskStatus |= TASK_READYSHOT;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	scanIoRequest(ptaskConfig->ioScanPvt);
#if 0
		epicsPrintf("Offset register 06= 0x%4X\n", ptaskConfig->offsetReg);
		epicsPrintf("FIFO High register 08= 0x%4X\n", ptaskConfig->fifoHigh);
		epicsPrintf("FIFO LOW register 0A= 0x%4X\n", ptaskConfig->fifoLow);
		epicsPrintf("Interrupt Control register 0C= 0x%4X\n", ptaskConfig->interruptControl);
		epicsPrintf("Interrupt Source register 0E= 0x%4X\n", ptaskConfig->interruptSource);
		epicsPrintf("CVTable Ch1 register 10= 0x%4X\n", ptaskConfig->cvtableCh1);
		epicsPrintf("CVTable Ch2 register 12= 0x%4X\n", ptaskConfig->cvtableCh2);
		epicsPrintf("CVTable Ch3 register 14= 0x%4X\n", ptaskConfig->cvtableCh3);
		epicsPrintf("CVTable register 16= 0x%4X\n", ptaskConfig->cvtableCh4);
		epicsPrintf("Sample Taken High register 18= 0x%4X\n", ptaskConfig->sampleTakenHigh);
		epicsPrintf("Sample Taken Low register 1A= 0x%4X\n", ptaskConfig->sampleTakenLow);
		epicsPrintf("Calibration Flash ROM Addr register 1C= 0x%4X\n", ptaskConfig->calFlashRomAdr);
		epicsPrintf("Calibration Flash ROM Data register 1E= 0x%4X\n", ptaskConfig->calFlashRomData);
		epicsPrintf("Calibration Source register 20= 0x%4X\n", ptaskConfig->calSource);
		epicsPrintf("Cache Count register 22= 0x%4X\n", ptaskConfig->cacheCount);
		epicsPrintf("Range, Filter, Connect Ch1&2 register 24= 0x%4X\n", ptaskConfig->voltFilter12);
		epicsPrintf("Range, Filter, Connect Ch1&2 register 26= 0x%4X\n", ptaskConfig->voltFilter34);
		epicsPrintf("Trigger Interrupt Level Ch1 register 28= 0x%4X\n", ptaskConfig->triggerLevel);
		epicsPrintf("Sample Period High register 30= 0x%4X\n", ptaskConfig->sampleRateHigh);
		epicsPrintf("Sample Period Low register 32= 0x%4X\n", ptaskConfig->sampleRateLow);
		epicsPrintf("Pre-Trigger Count High register 34= 0x%4X\n", ptaskConfig->preTriggerHigh);
		epicsPrintf("Pre-Trigger Count Low register 36= 0x%4X\n", ptaskConfig->preTriggerLow);
		epicsPrintf("Post-Trigger(Sample) Count register 38= 0x%4X\n", ptaskConfig->sampleCountHigh);
		epicsPrintf("Post-Trigger(Sample) Count register 3A= 0x%4X\n", ptaskConfig->sampleCountLow);
		epicsPrintf("Trigger Control Source register 3C= 0x%4X\n", ptaskConfig->triggerSource);
		epicsPrintf("Sample Control Source register 3E= 0x%4X\n", ptaskConfig->sampleSource);
#endif     
	   return;

}

static void set_taskExTrigger(drvVXI1564a_taskConfig *ptaskConfig, unsigned int exTrig)
{
	/* Set the trigger source and Slope. External Trigger Disable(0)/Enable(1), Negative(0)/Positive(1) Slope at Same Time */
	/* Ex_Trigger and Slope is Enable (and Positive) when exTrig value is 1. Disable and Negative Slope when exTrig value is 0. */
	
	unsigned short tempTrig, tempSample;
	
	tempTrig = ptaskConfig->triggerSource;	
	tempSample = ptaskConfig->sampleSource;	
		
	ptaskConfig->triggerType = exTrig ;  /* User Info for Remote / Local Function */
	if(ptaskConfig->opMode == exTrig){
		if(exTrig==0)
		{
			tempTrig = tempTrig & 0xFC7F;                   /* 7,8,9 bits zero : Slope Neg and ExTrigger Disable, 7bit Enable for Soft Trig  */
			tempSample = tempSample & 0x1F;                 /* All bits init(zero) for Soft Trig. Execption 4,3,2,1,0 bits */
			ptaskConfig->triggerSource = tempTrig | 0x300;   /* 0x80 for soft Trig -7 bit Enable for Soft Trig, but always I will used external trig/clock */
			ptaskConfig->sampleSource = tempSample | 0x4340; /* 0x21A0 : 5, 7, 8, 13 bit Enable for Soft Trigger <-> 0x83C0 : 6,7,8,9,15 bit Enable for External Trig/Clock */
		}else if(exTrig==1){
			tempTrig = tempTrig & 0xFC7F;                   /* Soft Trig Disable When the External Trigger Enable.  */
			tempSample = tempSample & 0x1F;                 /* All disable exception Internal Clock(5 bit) When the External Trigger Enable.  */
			ptaskConfig->triggerSource = tempTrig | 0x300;  /* 8,9bits one setting : Slope Pos and ExTrigger Enable and Soft Trig Disable     */
			ptaskConfig->sampleSource = tempSample | 0x4340;  /* This Register setting for sample control. so, same soft and ex-trigger state : for external Trig/Clock */
		}else epicsPrintf("\n External Trigger Setting value worng !!!!!! - You must check the value. \n");	
	}else epicsPrintf("\n Error -- Trigger Setting. Operation Mode and Trigger Setting check!!! You must Same value.\n");
	return;
}

static void set_taskCallback(drvVXI1564a_taskConfig *ptaskConfig)
{
	/* Not used -- sem used. */
	/* The Interrupt source are controlled by the interrupt control register. There are several sources of interrupt.    */
	/* A logical OR is performed on the enabled sources to determine if an IRQ should be pulled.  */
	unsigned short tempInterrupt;
	
	if(!(ptaskConfig->taskStatus & TASK_STARTED)) {
		tempInterrupt = ptaskConfig->interruptControl;	
		ptaskConfig->interruptControl = tempInterrupt | 0x400F;	
		
#ifdef DEBUG
		epicsPrintf("Task Interrupt Control Register Setting Done. \n");
#endif
		epicsThreadSleep(0.1);
	}

	if(ptaskConfig->taskStatus & TASK_STARTED) {
		epicsPrintf("Task Not Stopped...Do Not set_taskCallback Action!!! \n");
	}

        return;
}
#if 0
static void init_dataPool(drvVXI1564a_taskConfig *ptaskConfig)
{
	float64 *pdata_pool, *pdata;
	dataPoolConfig *pdataPoolConfig_pool, *pdataPoolConfig;
	int i;

	pdata_pool = malloc(sizeof(float64)
    	    	            *ellCount(ptaskConfig->pchannelConfig)
		            *INIT_DATAPOOLSIZE);
	pdataPoolConfig_pool = malloc(sizeof(dataPoolConfig) * INIT_DATAPOOLSIZE);

	for(i = 0; i< INIT_DATAPOOLSIZE;i++) {
		pdata = pdata_pool + (ellCount(ptaskConfig->pchannelConfig) * i);
		pdataPoolConfig = pdataPoolConfig_pool +i;
		pdataPoolConfig->used_count = 0;
		pdataPoolConfig->pdata = pdata;

		epicsMutexLock(ptaskConfig->bufferLock);
 			ellAdd(ptaskConfig->pdataPoolList, &pdataPoolConfig->node);
		epicsMutexUnlock(ptaskConfig->bufferLock);

	}

}
#endif

static void start_task(drvVXI1564a_taskConfig *ptaskConfig)
{  
/* Always You must consider before start task about the task Abort(sample control all disable). so you must doing set the function for remote/local */
	ViStatus err = 0;
	int semStatus;
	unsigned short tempTrig, tempSample;
	
	tempTrig = ptaskConfig->triggerSource;	
	tempSample = ptaskConfig->sampleSource;	
	if((ptaskConfig->taskStatus & TASK_INITIALIZED) && !(ptaskConfig->taskStatus & TASK_STARTED)) {
		/* **  reset function for interrupt register refrish when second data acquisition time.    **/
		reset(ptaskConfig);
		epicsThreadSleep(0.1);
		epicsMutexLock(ptaskConfig->bufferLock);	
		ptaskConfig->abortStop = 0;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		/******************** make measurements *******************/
		/* set Channel 1 and 2 to ?V range  0x7373 sets 4 V (0x7373 )range */
     	     	err=viOut16(ptaskConfig->taskHandle, VI_A16_SPACE, VXI_RANG_FILTER_12, ptaskConfig->voltFilter12);
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
		/* set Channel 3 and 4 to ? V range 0x7373 sets 4 (0x7373)V range  */
     	     	err=viOut16(ptaskConfig->taskHandle, VI_A16_SPACE, VXI_RANG_FILTER_34, ptaskConfig->voltFilter34);
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

		/* set pre-trigger count, high word = 0 */
		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_PRE_TRIG_HIGH, ptaskConfig->preTriggerHigh);
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
     	     	err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_PRE_TRIG_LOW, ptaskConfig->preTriggerLow);  
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

		/* set sample count high word = 19=(1638400), low word = 100000 (0x2710) */
     	     	err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_SAMPLE_COUNT_HIGH, ptaskConfig->sampleCountHigh); 
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
     	     	err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_SAMPLE_COUNT_LOW, ptaskConfig->sampleCountLow);
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

		/* Setup Sample Rate 0.1 micro second order minimux= 1.3microsecond, Max, High byte sample interval= 10microsec */
		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_PERIOD_HIGH, ptaskConfig->sampleRateHigh);
       	       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_SAMPLE_PERIOD_LOW, ptaskConfig->sampleRateLow);
		if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#if 0        
		/* set interrupt source control. Memory is full or post trigger acquisition is done.  */
		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_INTR_CONTROL, ptaskConfig->interruptControl);
    	    	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
		if(ptaskConfig->triggerType==0)
		{
			tempTrig = tempTrig & 0xFC7F;                   /* 7,8,9 bits zero : Slope Neg and ExTrigger Disable, 7bit Enable for Soft Trig  */
			tempSample = tempSample & 0x1F;                 /* All bits init(zero) for Soft Trig. Execption 4,3,2,1,0 bits */
			ptaskConfig->triggerSource = tempTrig | 0x300;   /* 0x80 for soft Trig -7 bit Enable for Soft Trig, but always I will used external trig/clock */
			ptaskConfig->sampleSource = tempSample | 0x4340;  /* 0x21A0 : 5, 7, 8, 13 bit Enable for Soft Trigger <-> 0x83C0 : 6,7,8,9,15 bit Enable for External Trig/Clock */
		}else if(ptaskConfig->triggerType==1){
			tempTrig = tempTrig & 0xFC7F;                   /* Soft Trig Disable When the External Trigger Enable.  */
			tempSample = tempSample & 0x1F;                 /* All disable exception Internal Clock(5 bit) When the External Trigger Enable.  */
			ptaskConfig->triggerSource = tempTrig | 0x300;  /* 8,9bits one setting : Slope Pos and ExTrigger Enable and Soft Trig Disable     */
			ptaskConfig->sampleSource = tempSample | 0x4340;  /* This Register setting for sample control. so, same soft and ex-trigger state : for external Trig/Clock */
		}else epicsPrintf("\n External Trigger Setting value worng !!!!!! - You must check the value. \n");	
		/* set trigger source Ext, Positive Slope, set bits 8 and 9  */
		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_TRIG_CONTROL, ptaskConfig->triggerSource);
		if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
		/* initiate a reading with a internal clock, initaite after 30msec delay when ti is set to "1" set bits 5,7,8 & 13(21A0) or 15(81A1) */
		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE, VXI_SAMPLE_CONTROL, ptaskConfig->sampleSource);
    	    	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#if 0   
		/* Event Handler Install Enable  */
		err = viInstallHandler (ptaskConfig->taskHandle, VI_EVENT_VXI_SIGP, taskCallback, VI_NULL);
   	   	if (err < VI_SUCCESS) err_handler(ptaskConfig,err);
	
		/* Event Handler Enable  */
   	   	err = viEnableEvent (ptaskConfig->taskHandle, VI_EVENT_VXI_SIGP, VI_HNDLR, VI_NULL);
   	   	if (err < VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
#if 0	
		/* Event Type is not supported by the resource. - VI_EVENT_SERVICE_REQ  */	
		err = viInstallHandler (ptaskConfig->taskHandle, VI_EVENT_IO_COMPLETION, taskCallback, VI_NULL);
   	   	if (err < VI_SUCCESS) err_handler(ptaskConfig,err);
	
		/* Event Handler Enable  */
   	   	err = viEnableEvent (ptaskConfig->taskHandle, VI_EVENT_IO_COMPLETION, VI_HNDLR, VI_NULL);
   	   	if (err < VI_SUCCESS) err_handler(ptaskConfig,err);
		ptaskConfig->eventDone = 1;
#endif
		epicsMutexLock(ptaskConfig->bufferLock);	
		ptaskConfig->taskStatus &= ~TASK_READYSHOT;
		ptaskConfig->taskStatus &= ~TASK_STOPED;
		ptaskConfig->taskStatus |= TASK_STARTED;
		epicsMutexUnlock(ptaskConfig->bufferLock);
/* rm TG 20110706		semStatus=sem_post( &ptaskConfig->DAQsemaphore);  */
    epicsEventSignal (ptaskConfig->DAQsemaphore);
		epicsPrintf("Semaphore Post Done \n "); 
	}
	else {
		epicsPrintf("fail to start task %s or aready task started!!\n", ptaskConfig->taskName);
		return;
	}

	return;
}

static void stop_task(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setStop)
{
	ViStatus err = 0;
	unsigned short tempAbort;
	
	tempAbort = ptaskConfig->sampleSource;
	tempAbort = tempAbort & 0x0;
	ptaskConfig->sampleSource = tempAbort | 0x1000;
	
	/*  Abort Function - Add??? 0x3E - Sample source control : Abort is Measurement abort and flushes all reading data in a memory	 */
     	/*  reset function is not reasonable this function - why? : Beacause reset function doing at Power on state reset(ptaskConfig);  */
	if(ptaskConfig->taskStatus & TASK_INITIALIZED) {
     		err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_CONTROL,ptaskConfig->sampleSource); /* 0x3E -Sample Source/Ctrl Register */
       		if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#ifdef DEBUG
		epicsPrintf("Task Stop and Abort :%s, Sample Control Source register 3E= 0x%4X\n",
								ptaskConfig->taskName, ptaskConfig->sampleSource);
		epicsPrintf("Task Not Close session !!!!!!!!!!!!!! \n");
#endif
	}
	else {
		epicsPrintf("fail to stop task %s or aready task stopped!!\n", ptaskConfig->taskName);
		return;
	}

	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;
	ptaskConfig->taskStatus &= ~TASK_DATAGATHER;
	ptaskConfig->taskStatus &= ~TASK_DATADIVIDE;
	ptaskConfig->taskStatus &= ~TASK_DATASTORE;
	ptaskConfig->taskStatus |= TASK_READYSHOT;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	scanIoRequest(ptaskConfig->ioScanPvt);

        return;
}

static void abort_task(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setAbort)
{
	ViStatus err = 0;
	unsigned short tempAbort;
	
	tempAbort = ptaskConfig->sampleSource;
	
	/*  Abort Function - Add??? 0x3E - Sample source control : Abort is Measurement abort and flushes all reading data in a memory	 */
     	/*  reset function is not reasonable this function - why? : Beacause reset function doing at Power on state. reset(ptaskConfig);  */
	tempAbort = tempAbort & 0x0;
	ptaskConfig->sampleSource = tempAbort | 0x1000;
	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->abortStop = 1;  /* pass function in the eventFunction Thread  */	
	epicsMutexUnlock(ptaskConfig->bufferLock);
     	err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_CONTROL,ptaskConfig->sampleSource); /* 0x3E -Sample Source/Ctrl Register */
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#ifdef DEBUG
	epicsPrintf("Abort Task:%s, Sample Control Source register 3E= 0x%4X\n",ptaskConfig->taskName, ptaskConfig->sampleSource);
#endif
/*	err= viUninstallHandler (ptaskConfig->taskHandle, VI_EVENT_VXI_SIGP, ptaskConfig->cviCallback, VI_NULL);   */
/*	err= viUninstallHandler (ptaskConfig->taskHandle, VI_EVENT_IO_COMPLETION, ptaskConfig->cviCallback, VI_NULL);  */
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#ifdef DEBUG
	epicsPrintf("Uninsatll Event Handler :%s in stop_task function !!!!!!!!!!!!!! \n",ptaskConfig->taskName);
#endif
	if(!(ptaskConfig->taskStatus & TASK_DATAGATHER)){
		epicsMutexLock(ptaskConfig->bufferLock);	
		ptaskConfig->taskStatus &= ~TASK_STARTED;
		ptaskConfig->taskStatus |= TASK_STOPED;
		ptaskConfig->taskStatus &= ~TASK_DATAGATHER;
		ptaskConfig->taskStatus &= ~TASK_DATADIVIDE;
		ptaskConfig->taskStatus &= ~TASK_DATASTORE;
		ptaskConfig->taskStatus |= TASK_READYSHOT;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		scanIoRequest(ptaskConfig->ioScanPvt);
	}
        return;
}
static int regUpdate_task(drvVXI1564a_taskConfig *ptaskConfig)
{
	ViStatus err = 0;
#if 0
	/********* read the digitizer's ID and Device Type registers *********/
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_MANU_ID,&ptaskConfig->idReg);      /*  00 - Manufacturer ID Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_DEVICE_TYPE,&ptaskConfig->devTypeReg); /*  02 - Device Type Register   */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
	/**************** read the digitizer's status register **************/
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_STATUS_CONTROL,&ptaskConfig->statusReg);  /*  04 - Status/Control Register -Memory Size */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
	epicsThreadSleep(0.1);
/* Just read Register and Check the Register */
	/* Error ???  ERROR = Bus error occurred during transfer. */
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_OFFSET,&ptaskConfig->offsetReg); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,&ptaskConfig->fifoHigh); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,&ptaskConfig->fifoLow); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

/* Write Registers -  Interrupt Control Register  */ 
 	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_INTR_CONTROL,&ptaskConfig->interruptControl); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
	epicsMutexLock(ptaskConfig->bufferLock);	
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_INTR_SOURCE,&ptaskConfig->interruptSource); 
	epicsMutexUnlock(ptaskConfig->bufferLock);
 	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
	epicsThreadSleep(0.1);

/* Just read Register and Check the Register */
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH1,&ptaskConfig->cvtableCh1); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH2,&ptaskConfig->cvtableCh2); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH3,&ptaskConfig->cvtableCh3); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CVTABLE_CH4,&ptaskConfig->cvtableCh4); 
  	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_TAKEN_HIGH,&ptaskConfig->sampleTakenHigh); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_TAKEN_LOW,&ptaskConfig->sampleTakenLow); 
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

	epicsThreadSleep(0.1);
/* Write Registers -  Calibration Register  */ 
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CAL_ROM_ADDR,&ptaskConfig->calFlashRomAdr); 
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CAL_ROM_DATA,&ptaskConfig->calFlashRomData); 
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CAL_SOURCE,&ptaskConfig->calSource); 
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_CACHE_COUNT,&ptaskConfig->cacheCount); 
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
/* untill Just read Register and Check the Register */

/* Write Registers -  Range, Filter Register  */ 
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_RANG_FILTER_12,&ptaskConfig->voltFilter12); /* 0x24 -Range, Filter Ch 1&2 Connect Reg */
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_RANG_FILTER_34,&ptaskConfig->voltFilter34); /* 0x26 -Range, Filter Ch 3&4 Connect Reg */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_TRIG_INTR_LEVEL1,&ptaskConfig->triggerLevel); /* 0x28 -Range,Filter Ch 3&4 Connect Reg */
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

	epicsThreadSleep(0.1);
/* Write Registers -  Sampling Rate, Pre-trigger, Sample count Register  */ 
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_PERIOD_HIGH,&ptaskConfig->sampleRateHigh); /* 0x30 -Sample Period High Register */
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_PERIOD_LOW,&ptaskConfig->sampleRateLow);  /* 0x32 - Sample Period Low Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_PRE_TRIG_HIGH,&ptaskConfig->preTriggerHigh);  /* 0x34 - Pre-Trig Count High Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_PRE_TRIG_LOW,&ptaskConfig->preTriggerLow);  /* 0x36 - Pre-Trig Count Low Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_COUNT_HIGH,&ptaskConfig->sampleCountHigh);  /* 0x38 - Sample Count High Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_COUNT_LOW,&ptaskConfig->sampleCountLow);  /* 0x3A - Sample Count Low Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	
/* Write Registers -  Trig Source, Sample Source Register  */ 
	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_TRIG_CONTROL,&ptaskConfig->triggerSource);  /* 0x3C - Trig Source/Ctrl Register */
       	if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
  	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_SAMPLE_CONTROL,&ptaskConfig->sampleSource);  /* 0x3E - Sample Source/Ctrl Register */
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

        return 1;
}
/*
 *    Init  Wf1564 channel 20090121
 */
static void init_aoChannel(drvVXI1564a_aoChannelConfig *paoChannelConfig)
{
	unsigned short val12, val34, maskFilter, maskGain, valFilter, valGain;
/*	int valFilter, valGain;   */
	drvVXI1564a_taskConfig *ptaskConfig = paoChannelConfig->ptaskConfig;

	if(paoChannelConfig->channelStatus & CHANNEL_INITIALIZED) return;
	
	val12 = ptaskConfig->voltFilter12;
	val34 = ptaskConfig->voltFilter34;
/*	valMem = (int)(val & 0x0F00);
	reData = (oldData >>= shift) & mask;
*/
	if (paoChannelConfig->chanNum == 0 || paoChannelConfig->chanNum == 2){
		maskFilter=0x7000;
		maskGain=0x700;
		if(paoChannelConfig->chanNum == 0){
			valFilter = val12 & maskFilter;
			valFilter >>= 12;
			valGain = val12 & maskGain;
			valGain >>= 8;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 1 Filter:0x%4X, Gain:0x%4X \n", valFilter, valGain);
#endif
		}
		else {
			valFilter = val34 & maskFilter;
			valFilter >>= 12;
			valGain = val34 & maskGain;
			valGain >>= 8;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 3 Filter:0x%4X, Gain:0x%4X \n", valFilter, valGain);
#endif
		}
		switch((int)valFilter ) {
	    		case 0 :  paoChannelConfig->filterRangeSet = 1.5 ;     break; /* 000 - 1.5kHz    */
    			case 1 :  paoChannelConfig->filterRangeSet = 6   ;     break; /* 001 - 6kHz      */ 
    			case 2 :  paoChannelConfig->filterRangeSet = 25  ;     break; /* 010 - 25kHz     */
    			case 3 :  paoChannelConfig->filterRangeSet = 100 ;     break; /* 011 - 100kHz    */
    			case 7 :  paoChannelConfig->filterRangeSet = 0   ;     break; /* 111 - NO filter */
			default : epicsPrintf("\n>>> Set Filter Ch 1 or Ch 3: %d error \n", valFilter);
        		return;
		}
		switch((int)valGain ) {
	    		case 0 :  paoChannelConfig->voltRangeSet = 0.0625 ;     break; /* 000 - 62.5mV */
    			case 1 :  paoChannelConfig->voltRangeSet = 0.25   ;     break; /* 001 - 0.25V  */ 
    			case 2 :  paoChannelConfig->voltRangeSet = 1.0    ;     break; /* 010 - 1.0V   */
    			case 3 :  paoChannelConfig->voltRangeSet = 4.0    ;     break; /* 011 - 4.0V   */
    			case 4 :  paoChannelConfig->voltRangeSet = 16     ;     break; /* 100 - 16V    */
    			case 5 :  paoChannelConfig->voltRangeSet = 64     ;     break; /* 101 - 64V    */
    			case 6 :  paoChannelConfig->voltRangeSet = 256    ;     break; /* 110 - 256V   */
    			case 7 :  paoChannelConfig->voltRangeSet = 256    ;     break; /* 111 -Also 256V  */
			default : epicsPrintf("\n>>> Set Gain(Volt) Ch1 or Ch3: %d error \n", valGain);
        		return;
		}
	}
	if (paoChannelConfig->chanNum == 1 || paoChannelConfig->chanNum == 3){
		maskFilter=0x70;
		maskGain=0x7;
		if(paoChannelConfig->chanNum == 1){
			valFilter = val12 & maskFilter;
			valFilter >>= 4;
			valGain = val12 & maskGain;
			valGain >>= 0;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 2 Filter:0x%4X, Gain:0x%4X \n", valFilter, valGain);
#endif
		}
		else {
			valFilter = val34 & maskFilter;
			valFilter >>= 4;
			valGain = val34 & maskGain;
			valGain >>= 0;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 4 Filter:0x%4X, Gain:0x%4X \n", valFilter, valGain);
#endif
		}
		switch((int)valFilter ) {
	    		case 0 :  paoChannelConfig->filterRangeSet = 1.5 ;     break; /* 000 - 1.5kHz    */
    			case 1 :  paoChannelConfig->filterRangeSet = 6   ;     break; /* 001 - 6kHz      */ 
    			case 2 :  paoChannelConfig->filterRangeSet = 25  ;     break; /* 010 - 25kHz     */
    			case 3 :  paoChannelConfig->filterRangeSet = 100 ;     break; /* 011 - 100kHz    */
    			case 7 :  paoChannelConfig->filterRangeSet = 0   ;     break; /* 111 - NO filter */
			default : epicsPrintf("\n>>> Set Filter Ch 1 or Ch 3: %d error \n", valFilter);
        		return;
		}
		switch((int)valGain ) {
	    		case 0 :  paoChannelConfig->voltRangeSet = 0.0625 ;     break; /* 000 - 62.5mV */
    			case 1 :  paoChannelConfig->voltRangeSet = 0.25   ;     break; /* 001 - 0.25V  */ 
    			case 2 :  paoChannelConfig->voltRangeSet = 1.0    ;     break; /* 010 - 1.0V   */
    			case 3 :  paoChannelConfig->voltRangeSet = 4.0    ;     break; /* 011 - 4.0V   */
    			case 4 :  paoChannelConfig->voltRangeSet = 16     ;     break; /* 100 - 16V    */
    			case 5 :  paoChannelConfig->voltRangeSet = 64     ;     break; /* 101 - 64V    */
    			case 6 :  paoChannelConfig->voltRangeSet = 256    ;     break; /* 110 - 256V   */
    			case 7 :  paoChannelConfig->voltRangeSet = 256    ;     break; /* 111 -Also 256V  */
			default : epicsPrintf("\n>>> Set Gain(Volt) Ch1 or Ch3: %d error \n", valGain);
        		return;
		}
	}

#ifdef DEBUG
	epicsPrintf("init_aoChannel: task: %s %d, channel: %s, paoCh VoltR:%f, paoCh FilterR:%f \n", ptaskConfig->taskName, (unsigned) ptaskConfig->taskHandle, paoChannelConfig->chanName,paoChannelConfig->voltRangeSet,paoChannelConfig->filterRangeSet);
#endif
#ifdef DEBUG
	epicsPrintf("init_ao: %s \n", paoChannelConfig->chanName);
#endif
	paoChannelConfig->channelStatus |= CHANNEL_INITIALIZED;
	return;
}
/* Maybe you can test about remove scanIoRequest function...... so, finish the routine, you can do it.   */
static int send_dataChannelAndStore(drvVXI1564a_taskConfig *ptaskConfig)
{
  drvVXI1564a_channelConfig *pchannelConfig;
  	ViStatus err;
	
	unsigned long long int start, stop;
	int mdsConStatus;

	signed short u16Val1, u16Val2, u16Val3, u16Val4;
	signed short  *data1, *data2, *data3, *data4;
#if 0
	float *data1, *data2, *data3, *data4;
#endif
	uInt32 i;
	
	unsigned int chNu;
/* Add Send Data Tree Function */
	float64         sampleRate;
	float64         sampleCount;
	float64         samplePreCount;
	float64         blipTime;
	float64         daqTime;
	int			   shotNumber;
#if 0
	uInt32          shotNumber;
	shotNumber = ptaskConfig->shotNumber;
   */
#endif
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	char serverName[60];
	char treeName[60];

	/*      float data[5000], time[5000];  Array of floats used for signal */
	/*      float *data;   Array of floats used for signal */
	/*      signed short *data;   Array of signed short used for signal */

	int dataDesc; /* Signal descriptor */
	int rateDesc, startDesc, endDesc, voltRangeDesc;     /* Sample Num, Sample Rate, Data Start Time  */

	float realTimeGap, startTime, endTime, voltRange;
	int totalCounts;   /* Important variable type "int" ... about that dataDescr.. */

  int32 readCounts;

	uInt32 memTotal;
	int jTest; /* test for 20points read and write  */	
	signed short *data13, *data24, *data;

/* TG Test for MCWIN to read multi points   */
	int j2;
	
	signed short u16Val13[2], u16Val24[2];

/* remove TG 20110706	int dataSemStatus;
	dataSemStatus = sem_wait(&DATAsemaphore );  */
	epicsEventWait(DATAsemaphore );
	epicsThreadSleep(0.01);
  err = 0;

  start = drvVXI1564a_rdtsc();
	
	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->taskStatus &= ~TASK_DATAGATHER;
	ptaskConfig->taskStatus |= TASK_DATADIVIDE;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	scanIoRequest(ptaskConfig->ioScanPvt);

	sampleRate = ptaskConfig->samplingRate;
	sampleCount = ptaskConfig->sampleCount;
	samplePreCount = ptaskConfig->samplePreCount;
	totalCounts = sampleCount + samplePreCount;
	blipTime = ptaskConfig->blipTime;
	daqTime = ptaskConfig->daqTime;
	shotNumber = (int)ptaskConfig->shotNumber;


	data = (signed short *)malloc(sizeof(signed short)*totalCounts*4);
	data13 = (signed short *)malloc(sizeof(signed short)*totalCounts*2);
	data24 = (signed short *)malloc(sizeof(signed short)*totalCounts*2);

#if 0
	data1 = (float *)malloc(sizeof(float)*totalCounts);
	data2 = (float *)malloc(sizeof(float)*totalCounts);
	data3 = (float *)malloc(sizeof(float)*totalCounts);
	data4 = (float *)malloc(sizeof(float)*totalCounts);
#endif

	strcpy(serverName, ptaskConfig->mdsServer);
	strcpy(treeName,ptaskConfig->mdsTree);

	rateDesc = descr(&dtypeFloat, &realTimeGap, &null);
	startDesc = descr(&dtypeFloat, &startTime, &null);
	endDesc = descr(&dtypeFloat, &endTime, &null);	

	if( ptaskConfig->opMode == OPMODE_REMOTE ) {
		/* Connect to MDSplus  ptaskConfig->mdsStatus  */
		epicsPrintf("MDSplus Data Put ---- Remote Mode!!! \n");
#ifdef DEBUG
		epicsPrintf("\n%s >>> MdsOpen(\"%s\", shot: %d)... OK\n", ptaskConfig->taskName, ptaskConfig->mdsTree, shotNumber);
#endif
		if(blipTime != 0){
			startTime = (daqTime - blipTime) * sampleRate - samplePreCount - 1;
			/*              endTime = (sampleReCount-1) + startTime;   -1 count  good ? */
			/*              endTime = totalCounts + startTime-2;       -2 count  bad -> time counts 2points  */
			endTime = totalCounts + startTime - samplePreCount-1;  /* samplePreCount and -2 count is bad */
		} else {
			if(daqTime !=0){
				startTime = daqTime * sampleRate - samplePreCount - 1;
				endTime = totalCounts + startTime - samplePreCount-1;  /* samplePreCount and -2 count is bad */
			} else {
				startTime = sampleRate - samplePreCount - 1;
				endTime = totalCounts + startTime - samplePreCount-1;  /* samplePreCount and -2 count is bad */
			}
		}
	}
	else {
		epicsPrintf("MDSplus Data Put ---- Local Mode!!! \n");
		/* Local Mode is Direct Open tree */
		startTime = -1 - samplePreCount;   /* Data Zero Start Time with preTrigger sample Counts */
		/*              endTime = totalCounts -2;            endTime = sampleReCount- 2;  Last data Zero Over Count.  */
		endTime = totalCounts - samplePreCount - 2;
		/* endTime = sampleReCount- 2 with preTrigger sample Counts; Last data Zero Over Count.*/
	}

	realTimeGap = ptaskConfig->samplePeriod;

   	/* Event type is a number which identifies the event type that has been received. */


/* Step by Step do the data gethering and storing  */
/*	dataSemStatus = sem_wait(&DATAsemaphore );   */
	if(ptaskConfig->abortStop == 1){
		epicsPrintf(" Abort Channelization And Data Store !!!!! \n");
		mdsDisconnectAndClose(ptaskConfig);
/* remove TG 20110706		dataSemStatus = sem_post(&DATAsemaphore);  */
    epicsEventSignal(DATAsemaphore);
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->taskStatus &= ~TASK_STARTED;
		ptaskConfig->taskStatus |= TASK_STOPED;
		ptaskConfig->taskStatus &= ~TASK_DATAGATHER;
		ptaskConfig->taskStatus &= ~TASK_DATADIVIDE;
		ptaskConfig->taskStatus &= ~TASK_DATASTORE;
		ptaskConfig->taskStatus |= TASK_READYSHOT;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		scanIoRequest(ptaskConfig->ioScanPvt);
	}
#if 1
/* I used this function at 2010 plasma operation  */  
	memTotal = totalCounts*2;
	data1 = (signed short *)malloc(sizeof(signed short)*totalCounts);
	data2 = (signed short *)malloc(sizeof(signed short)*totalCounts);
	data3 = (signed short *)malloc(sizeof(signed short)*totalCounts);
	data4 = (signed short *)malloc(sizeof(signed short)*totalCounts);
#endif

#if 0 
/*       error does not support function   */
	int retCount;

	err = viRead (ptaskConfig->taskHandle, data, memTotal, &retCount);
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);

	printf("viRead Function return Counts %d.\n",retCount);
#endif 

	
#if 0	
	memTotal = totalCounts*2;
	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,&data13);
	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,&data24);
	int retCount;
	memTotal = totalCounts*4;
	err = viRead (ptaskConfig->taskHandle, VI_A16_SPACE,VXI_FIFO_HIGH, memTotal, &retCount);
	epicsPrintf("viRead Function return Counts %d.\n",retCount);

#endif
#if 1
/* 
	ViStatus _VI_FUNC  viMove  (ViSession vi, ViUInt16 srcSpace, ViBusAddress srcOffset,
                                    ViUInt16 srcWidth, ViUInt16 destSpace, ViBusAddress destOffset, ViUInt16 destWidth, ViBusSize srcLength);
	err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,memTotal, data13);
	err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,memTotal, data24);
	
*/
#if 0 
/*	2Channel Read,  all points read by viMove function. Process time ???  */
	UINT32 dmaVal32;	
	NIVXI_STATUS ret;
	UINT16 source_parms, dest_parms;     /* Parameters for transfer. */
	UINT32 source_address1, source_address2;        /* Addresses  for transfer. */
	BUS_ADDRESS dest_address13, dest_address24;
	UINT16 width;                 /* Set to 1: Byte
				       *        2: Word
				       *        4: Long Word */
	UINT8 *dest_ptr;              /*Pointer to local buffer*/
	void *useraddr;               /* Used in VXImemAlloc/VXImemFree */	
		
	UINT32 length;
	
	ret = InitVXIlibrary();       /* Always Init the VXI library */		


	LPeek (0x1B04, 4, &dmaVal32);
	printf("################################## Print and Check DMA Default Param Before: 0x%4X \n",dmaVal32);
	dmaVal32 |= 0x06; 
        LPoke (0x1B04, 4, dmaVal32);	
	printf("################################## Print and Check DMA Default Param After : 0x%4X \n",dmaVal32);

	if (ret < NIVXI_OK)
	{
		printf("Error %hd in InitVXIlibrary\n",ret);
		return -1;
	}

	width = 1;                    /* Currently set to long word transfer */
	source_parms = 1;             /* specify source parameters. Right now it is
				       * set for A32 space.*/
	source_address1 = 0xCA08L; /* ptaskConfig->taskHandle + VXI_FIFO_HIGH; -error   specify location of source data to move.
					 				* Right now it is set for address 0x200000. */
	source_address2 = 0xCA0AL; /* ptaskConfig->taskHandle + VXI_FIFO_LOW; -error */
	length = 0x200L;
	/* Address  base address = C000(16) + (logical address * 64)(16)
           Register address = base address + register offset   
	   40 => 0xCA00 + offset (VXI_FIFO_HIGH = 0x08,  VXI_FIFO_LOW = 0x0A */
	dest_address13 = (BUS_ADDRESS)malloc(sizeof(signed short)*totalCounts*2);
	dest_address24 = (BUS_ADDRESS)malloc(sizeof(signed short)*totalCounts*2);
 
	dest_parms = 0;               /* Specify destination to be local memory */
	/* Using shared memory as a source.  This overrides the address 0x200000 that
	 * I specified above.  Remove VXImemAlloc if not using shared memory. */

	memTotal = totalCounts*2;
	
	ret = VXImove(source_parms, source_address1,0,dest_address13, memTotal, width);
	ret = VXImove(source_parms, source_address2,0,dest_address24, memTotal, width);
	if (ret < NIVXI_OK)
	{
		printf("Error %hd in VXImemAlloc\n",ret);
	}

	data13 = (signed short *)dest_address13;
	data24 = (signed short *)dest_address24;
	for(i=0; i<512; i+=2){
      		data1[i] = data13[i];
      		data3[i] = data13[i+1];
      		data2[i] = data24[i];
      		data4[i] = data24[i+1];
	}
#if 0
	ret = CloseVXIlibrary();
	if (ret < NIVXI_OK)
	{
		printf("Error %hd in VXImemAlloc\n",ret);
	}
#endif
#endif
/* 4channel Read all points used by hpe1564_fetch_Q function 20110721 */
  err=hpe1564_fetch_Q(ptaskConfig->hpeHandle,totalCounts,data1,data2,data3,data4,&readCounts);

#if 0 
/*	2Channel Read,  all points read by viMove function. Process time ???  ERROR = Invalid length specified */
	int j;
	memTotal = totalCounts*2;
	err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,memTotal-1, data13);
	err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,memTotal-1, data24);
	for(j=0,i=0; i<memTotal; i+=2, j++){
      		data1[j] = data13[i];
      		data3[j] = data13[i+1];
      		data2[j] = data24[i];
      		data4[j] = data24[i+1];
	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
/*	err = viRead (ptaskConfig->taskHandle, data, memTotal, &retCount);
	signed short *data1, *data2, *data3, *data4;
	data1 = (signed short *)malloc(sizeof(signed short)*sampleCounts);
	data2 = (signed short *)malloc(sizeof(signed short)*sampleCounts);
	data3 = (signed short *)malloc(sizeof(signed short)*sampleCounts);
	data4 = (signed short *)malloc(sizeof(signed short)*sampleCounts);
*/
#if 0
/*	2Channel Read,  1 points per 1channel .. Process time is 9.8sec(8.7sec)   OK....Success  */
	signed short u16Val13[2], u16Val24[2];
	for(i=0; i<(totalCounts); i++){
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,2, u16Val13);
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,2, u16Val24);
      		data1[i] = u16Val13[0];
      		data3[i] = u16Val13[1];
      		data2[i] = u16Val24[0];
      		data4[i] = u16Val24[1];
	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
#if 0 
/*	2Channel Read, 4 points per 1channel .. Process time is 9.8sec(8.7sec)   OK....Success  */
	signed short u16Val13[16], u16Val24[16];
	for(i=0; i<memTotal; i+=8){
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,16, u16Val13);
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,16, u16Val24);
      		data1[i] = u16Val13[0];
      		data3[i] = u16Val13[1];
      		data1[i+1] = u16Val13[2];
      		data3[i+1] = u16Val13[3];
      		data1[i+2] = u16Val13[4];
      		data3[i+2] = u16Val13[5];
      		data1[i+3] = u16Val13[6];
      		data3[i+3] = u16Val13[7];
      		data1[i+4] = u16Val13[8];
      		data3[i+4] = u16Val13[9];
      		data1[i+5] = u16Val13[10];
      		data3[i+5] = u16Val13[11];
      		data1[i+6] = u16Val13[12];
      		data3[i+6] = u16Val13[13];
      		data1[i+7] = u16Val13[14];
      		data3[i+7] = u16Val13[15];
      		data2[i] = u16Val24[0];
      		data4[i] = u16Val24[1];
      		data2[i+1] = u16Val24[2];
      		data4[i+1] = u16Val24[3];
      		data2[i+2] = u16Val24[4];
      		data4[i+2] = u16Val24[5];
      		data2[i+3] = u16Val24[6];
      		data4[i+3] = u16Val24[7];
      		data2[i+4] = u16Val24[8];
      		data4[i+4] = u16Val24[9];
      		data2[i+5] = u16Val24[10];
      		data4[i+5] = u16Val24[11];
      		data2[i+6] = u16Val24[12];
      		data4[i+6] = u16Val24[13];
      		data2[i+7] = u16Val24[14];
      		data4[i+7] = u16Val24[15];
	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
#if 0 
/*  I will used this function   */ 
/*	2Channel Read, 4 points per 1channel .. Process time is 9.8sec(8.7sec)   OK....Success  */
/*	2Channel Read, 20 points per 1channel ..u16Val13[40], u16Val24[40]; Fail */
/*	2Channel Read, 16 points per 1channel ..u16Val13[32], u16Val24[32]; Fail */
/*	2Channel Read, 10 points per 1channel ..u16Val13[20], u16Val24[20]; OK. But Data is not acceptable. */
/*	2Channel Read, 14 points per 1channel ..u16Val13[28], u16Val24[28]; Fail. */
/*	8points per 2Channel Read, 4 points per 1channel ..u16Val13[8], u16Val24[8]; OK.Data OK. */
/*	10points per 2Channel Read, 5 points per 1channel ..u16Val13[10], u16Val24[10]; OK.Data OK. */
/*	12points per 2Channel Read, 6 points per 1channel ..u16Val13[12], u16Val24[12]; OK.Data OK. */
/*	14points per 2Channel Read, 7 points per 1channel ..u16Val13[14], u16Val24[14]; OK.Data OK. */
/*	16points per 2Channel Read, 8 points per 1channel ..u16Val13[16], u16Val24[16]; OK.But Data was wrong. */
/*   BASE_ADDR = C000 + (LADDR(40 -default) * 60)  */
/*   Recommand. err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH + BASE_ADDR,40, u16Val13); 
		Input value and test : 0xCA08 and 0xCA0A - data wrong and error message "ERROR = Invalid offset specified." 
		Input value and test : 51720 and 51722 - data wrong and error message "ERROR = Invalid offset specified." */
/*              I try read 28byte(buffer 14)data, but I cann't read data. error message "ERROR = Invalid offset specified."
                so, I think that is wrong recommand.  */
	int j2;
	
	signed short u16Val13[20], u16Val24[20];
	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH, data13);
	err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW, data24);
	for(i=0,j2=0; i<(totalCounts*2); i+=2,j2++){
			data1[j2] = data13[i];
      			data3[j2] = data13[i+1];
      			data2[j2] = data24[i];
      			data4[j2] = data24[i+1];

	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
/*        if (err<VI_SUCCESS) err_handler(ptaskConfig,err); */
#endif
#if 0
/*  I will used this function   */ 
/*	2Channel Read, 4 points per 1channel .. Process time is 9.8sec(8.7sec)   OK....Success  */
/*	2Channel Read, 20 points per 1channel ..u16Val13[40], u16Val24[40]; Fail */
/*	2Channel Read, 16 points per 1channel ..u16Val13[32], u16Val24[32]; Fail */
/*	2Channel Read, 10 points per 1channel ..u16Val13[20], u16Val24[20]; OK. But Data is not acceptable. */
/*	2Channel Read, 14 points per 1channel ..u16Val13[28], u16Val24[28]; Fail. */
/*	8points per 2Channel Read, 4 points per 1channel ..u16Val13[8], u16Val24[8]; OK.Data OK. */
/*	10points per 2Channel Read, 5 points per 1channel ..u16Val13[10], u16Val24[10]; OK.Data OK. */
/*	12points per 2Channel Read, 6 points per 1channel ..u16Val13[12], u16Val24[12]; OK.Data OK. */
/*	14points per 2Channel Read, 7 points per 1channel ..u16Val13[14], u16Val24[14]; OK.Data OK. */
/*	16points per 2Channel Read, 8 points per 1channel ..u16Val13[16], u16Val24[16]; OK.But Data was wrong. */
/*   BASE_ADDR = C000 + (LADDR(40 -default) * 60)  */
/*   Recommand. err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH + BASE_ADDR,40, u16Val13); 
		Input value and test : 0xCA08 and 0xCA0A - data wrong and error message "ERROR = Invalid offset specified." 
		Input value and test : 51720 and 51722 - data wrong and error message "ERROR = Invalid offset specified." */
/*              I try read 28byte(buffer 14)data, but I cann't read data. error message "ERROR = Invalid offset specified."
                so, I think that is wrong recommand.  */

/* I tested buffer 14 but data was acceptable.  20110503 */
/* I tested buffer 8  but data was acceptable.  20110503 */
/*  move in init function
	int j2;
	
	signed short u16Val13[2], u16Val24[2];
*/
	for(i=0; i<totalCounts; i+=1){
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,2, u16Val13);
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,2, u16Val24);
      		for(jTest=0, j2=0; jTest<2; j2++,jTest+=2){
			data1[i+j2] = u16Val13[jTest];
      			data3[i+j2] = u16Val13[jTest+1];
      			data2[i+j2] = u16Val24[jTest];
      			data4[i+j2] = u16Val24[jTest+1];
		}

	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
/*        if (err<VI_SUCCESS) err_handler(ptaskConfig,err); */
#endif
#if 0 
/*	2Channel Read, 4 points per 1channel .. Process time is 9.8sec(8.7sec)   OK....Success  */
/*	2Channel Read, 20 points per 1channel ..u16Val13[40], u16Val24[40]; Fail */
/*	2Channel Read, 16 points per 1channel ..u16Val13[32], u16Val24[32]; Fail */
/*	2Channel Read, 10 points per 1channel ..u16Val13[20], u16Val24[20]; OK. But Data is not acceptable. */
/*	2Channel Read, 14 points per 1channel ..u16Val13[28], u16Val24[28]; Fail. */
/*	8points per 2Channel Read, 4 points per 1channel ..u16Val13[8], u16Val24[8]; OK.Data OK. */
/*	10points per 2Channel Read, 5 points per 1channel ..u16Val13[10], u16Val24[10]; OK.Data OK. */
/*	12points per 2Channel Read, 6 points per 1channel ..u16Val13[12], u16Val24[12]; OK.Data OK. */
/*	14points per 2Channel Read, 7 points per 1channel ..u16Val13[14], u16Val24[14]; OK.Data OK. */
/*	16points per 2Channel Read, 8 points per 1channel ..u16Val13[16], u16Val24[16]; OK.But Data was wrong. */
	int j2;
	signed short u16Val13[14], u16Val24[14];
	for(i=0; i<memTotal; i+=14){
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,14, u16Val13);
		err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,14, u16Val24);
		memcpy(&data13[i], &u16Val13, sizeof(signed short)*14);
		memcpy(&data24[i], &u16Val24, sizeof(signed short)*14);
	}
      	for(j2=0, jTest=0; j2<totalCounts; j2++, jTest+=2){
		data1[j2] = data13[jTest];
      		data3[j2] = data13[jTest+1];
      		data2[j2] = data24[jTest];
      		data4[j2] = data24[jTest+1];
	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
#if 0 
/* 2Channel Read,  all points read .. Process time. We did not success this  */
/* we did parameter changed from VI_A16_SPACE to VI_LOCAL_SPACE. But we did not success. Error is "invalided address space specified." */
	memTotal = totalCounts*2;
	err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,128, data13);
	err=viMoveIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,128, data24);
	for(i=0; i<(totalCounts); i+=2){
      		data1[i] = data13[i];
      		data3[i] = data13[i+1];
      		data2[i] = data24[i];
      		data4[i] = data24[i+1];
	}
        if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
#endif
#endif



#if  0 
/* I used this function at 2010 plasma operation  */  
	for(i=0; i<totalCounts; i++){
		err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,&u16Val1);
		err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,&u16Val2);
		err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_HIGH,&u16Val3);
		err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_FIFO_LOW,&u16Val4);
		data1[i] = u16Val1;
		data2[i] = u16Val2;
		data3[i] = u16Val3;
		data4[i] = u16Val4;
	}
#endif
#if 0
		data1[i] = data13[i];
		data2[i] = data24[i];
		data3[i] = data13[i+1];
		data4[i] = data24[i+1];

/*
		data1[i] = data[i];
		data2[i] = data[i+1];
		data3[i] = data[i+2];
		data4[i] = data[i+3];
*/
	}
#endif
	printf("memTotal = %d counts, ",memTotal);
	printf("readCounts = %d counts, ",readCounts);
       for(i=0; i<20; i++){	
	printf("Channel 1 = %lf Volts, ", data1[i]*0.0001221);
	printf("Channel 2 = %lf Volts, ", data2[i]*0.0001221);
	printf("Channel 3 = %lf Volts, ", data3[i]*0.0001221);
	printf("Channel 4 = %lf Volts, ", data4[i]*0.0001221);
	}
	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->taskStatus &= ~TASK_DATADIVIDE;
	ptaskConfig->taskStatus |= TASK_DATASTORE;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	scanIoRequest(ptaskConfig->ioScanPvt);
	epicsThreadSleep(0.1);
	
	free(data13);
	free(data24);	
	free(data);	
	
	stop = drvVXI1564a_rdtsc();
	epicsPrintf("Channelization from device memory taskName:%s, Current Time %lf msec \n",ptaskConfig->taskName, 1.E-3 * (double)drvVXI1564a_intervalUSec(start, stop));
	epicsThreadSleep(0.5);

    start = drvVXI1564a_rdtsc();

	mdsConStatus= mdsConnectAndOpen(ptaskConfig); 
	pchannelConfig = (drvVXI1564a_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig)
	{
		drvVXI1564a_aoChannelConfig *paoChannelConfig = (drvVXI1564a_aoChannelConfig*) pchannelConfig;
		chNu=paoChannelConfig->chanNum;
		switch(chNu) {
			case 0 :{
						voltRange =(float)paoChannelConfig->voltRangeSet/32768;
#ifdef DEBUG
						epicsPrintf("channel Number :%d, Volt Range : %f, calVoltRange :%f. \n ",chNu,paoChannelConfig->voltRangeSet, voltRange);
						epicsPrintf("sampleTotalCounts :%d, SampleRate :%f, StartTime : %f, EndTime :%f. \n ",
								totalCounts, sampleRate ,startTime, endTime);
#endif
						voltRangeDesc = descr(&dtypeFloat, &voltRange, &null);
/*						dataDesc = descr(&dtypeFloat, data1, &totalCounts, &null);   */
						dataDesc = descr(&dtypeSigned, data1, &totalCounts, &null);
#ifdef DEBUG
						epicsPrintf("channel Number :%d paoChannel-- data desc after delay \n ",chNu);
#endif
						epicsThreadSleep(0.1);
#if 1
						status = MdsPut(paoChannelConfig->treeNode, "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &voltRangeDesc, &dataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);
						epicsThreadSleep(0.1);
						if ( !statusOk(status) )
						{
							fprintf(stderr,"Error writing signal ch%d : %s\n", chNu, MdsGetMsg(status));
						}
#endif
						fprintf(stdout,"ch%d ch_Name:%s ,Tree Node:%s \n", chNu, paoChannelConfig->chanName, paoChannelConfig->treeNode);
					};     break; 
			case 1 :{
						voltRange =(float)paoChannelConfig->voltRangeSet/32768;
#ifdef DEBUG
						epicsPrintf("channel Number :%d, Volt Range : %f, calVoltRange :%f. \n ",chNu,paoChannelConfig->voltRangeSet, voltRange);
						epicsPrintf("sampleTotalCounts :%d, SampleRate :%f, StartTime : %f, EndTime :%f. \n ",
								totalCounts, sampleRate ,startTime, endTime);
#endif
						voltRangeDesc = descr(&dtypeFloat, &voltRange, &null);
/*						dataDesc = descr(&dtypeFloat, data2, &totalCounts, &null);  */
						dataDesc = descr(&dtypeSigned, data2, &totalCounts, &null);
#ifdef DEBUG
						epicsPrintf("channel Number :%d paoChannel-- data desc after delay \n ",chNu);
#endif
						epicsThreadSleep(0.1);
#if 1
						status = MdsPut(paoChannelConfig->treeNode, "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &voltRangeDesc, &dataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);
						epicsThreadSleep(0.1);
						if ( !statusOk(status) )
						{
							fprintf(stderr,"Error writing signal ch%d : %s\n", chNu, MdsGetMsg(status));
						}
#endif
						fprintf(stdout,"ch%d ch_Name:%s ,Tree Node:%s \n", chNu, paoChannelConfig->chanName, paoChannelConfig->treeNode);
					};     break; 
			case 2 :{
						voltRange =(float)paoChannelConfig->voltRangeSet/32768;
#ifdef DEBUG
						epicsPrintf("channel Number :%d, Volt Range : %f, calVoltRange :%f. \n ",chNu,paoChannelConfig->voltRangeSet, voltRange);
						epicsPrintf("sampleTotalCounts :%d, SampleRate :%f, StartTime : %f, EndTime :%f. \n ",
								totalCounts, sampleRate ,startTime, endTime);
#endif
						voltRangeDesc = descr(&dtypeFloat, &voltRange, &null);
/*						dataDesc = descr(&dtypeFloat, data3, &totalCounts, &null);  */
						dataDesc = descr(&dtypeSigned, data3, &totalCounts, &null);
#ifdef DEBUG
						epicsPrintf("channel Number :%d paoChannel-- data desc after delay \n ",chNu);
#endif
						epicsThreadSleep(0.1);
#if 1
						status = MdsPut(paoChannelConfig->treeNode, "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &voltRangeDesc, &dataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);
						epicsThreadSleep(0.1);
						if ( !statusOk(status) )
						{
							fprintf(stderr,"Error writing signal ch%d : %s\n", chNu, MdsGetMsg(status));
						}
#endif
						fprintf(stdout,"ch%d ch_Name:%s ,Tree Node:%s \n", chNu, paoChannelConfig->chanName, paoChannelConfig->treeNode);

					};     break; 
			case 3 :{
						voltRange =(float)paoChannelConfig->voltRangeSet/32768;
#ifdef DEBUG
						epicsPrintf("channel Number :%d, Volt Range : %f, calVoltRange :%f. \n ",chNu,paoChannelConfig->voltRangeSet, voltRange);
						epicsPrintf("sampleTotalCounts :%d, SampleRate :%f, StartTime : %f, EndTime :%f. \n ",
								totalCounts, sampleRate ,startTime, endTime);
#endif
						voltRangeDesc = descr(&dtypeFloat, &voltRange, &null);
/*						dataDesc = descr(&dtypeFloat, data4, &totalCounts, &null);  */
						dataDesc = descr(&dtypeSigned, data4, &totalCounts, &null);
#ifdef DEBUG
						epicsPrintf("channel Number :%d paoChannel-- data desc after delay \n ",chNu);
#endif
						epicsThreadSleep(0.1);
#if 1
						status = MdsPut(paoChannelConfig->treeNode, "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &voltRangeDesc, &dataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);
						epicsThreadSleep(0.1);
						if ( !statusOk(status) )
						{
							fprintf(stderr,"Error writing signal ch%d : %s\n", chNu, MdsGetMsg(status));
						}
#endif
						fprintf(stdout,"ch%d ch_Name:%s ,Tree Node:%s \n", chNu, paoChannelConfig->chanName, paoChannelConfig->treeNode);
					};     break; 
			default : epicsPrintf("\n>>>ERROR Data Copy for Channels : %d \n", chNu); return 0;
		}

#ifdef DEBUG
        epicsPrintf("ch%d ch_Name:%s , Node:%s \n", chNu, paoChannelConfig->chanName, paoChannelConfig->treeNode);
#endif
	pchannelConfig = (drvVXI1564a_channelConfig*) ellNext(&pchannelConfig->node);
	}
	mdsDisconnectAndClose(ptaskConfig);
	free(data1);
	free(data2);
	free(data3);
	free(data4);
	epicsThreadSleep(0.1);
/* remove TG 20110706	dataSemStatus = sem_post(&DATAsemaphore); */
	epicsEventSignal(DATAsemaphore); 
	stop = drvVXI1564a_rdtsc();
	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;
	ptaskConfig->taskStatus &= ~TASK_DATASTORE;
	ptaskConfig->taskStatus |= TASK_READYSHOT;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	scanIoRequest(ptaskConfig->ioScanPvt);
	epicsThreadSleep(0.05);

	epicsPrintf("Data write to MDSplus Node : Current Time %lf msec \n", 1.E-3 * (double)drvVXI1564a_intervalUSec(start, stop));
	return 1;
}

static int mdsConnectAndOpen(drvVXI1564a_taskConfig *ptaskConfig)
{
        int status; /* Will contain the status of the data access operation */
	char serverName[60];
	char treeName[60];
	int		shotNumber;

	shotNumber =(int)ptaskConfig->shotNumber;

	strcpy(serverName, ptaskConfig->mdsServer);
	strcpy(treeName,ptaskConfig->mdsTree);

	if( ptaskConfig->opMode == OPMODE_REMOTE ) {
        	/* Connect to MDSplus  ptaskConfig->mdsStatus  */
#ifdef DEBUG
		epicsPrintf("MDSplus Data Put ---- Remote Mode!!! \n");
#endif
		status = MdsConnect(serverName);
        	if ( status == -1 )
        	{
                	fprintf(stderr,"Error connecting to mdsip server(%s).\n", serverName);
                	return -1;
        	}
#ifdef DEBUG
        	epicsPrintf(" %s >>> MdsConnect(\"%s\")... OK\n", ptaskConfig->taskName, serverName);
#endif
	        /* Open tree */
#if 1
	        status = MdsOpen(treeName, &shotNumber);
        	if ( !statusOk(status) )
        	{
                	fprintf(stderr,"Error opening tree for shot %d: %s.\n",shotNumber, MdsGetMsg(status));
                	MdsDisconnect();
                	return -1;
        	}
        	epicsPrintf("\n%s >>> MdsOpen(\"%s\", shot: %d)... OK\n", ptaskConfig->taskName, ptaskConfig->mdsTree, shotNumber);
#endif
	}
	else {
/*		create_newTree(ptaskConfig);   Remove this function because we have not to each create tree same shot
		at each task. so I remove this function.   */
#ifdef DEBUG
		epicsPrintf("MDSplus Data Put ---- Local Mode!!! \n");
#endif
	        /* Local Mode is Direct Open tree */
#if 1 
	        status = MdsOpen(treeName, &shotNumber);
        	if ( !statusOk(status) )
        	{
                	fprintf(stderr,"Error opening tree for shot %d: %s.\n",shotNumber, MdsGetMsg(status));
                	MdsDisconnect();
                	return -1;
        	}
        	epicsPrintf("\n%s >>> MdsOpen(\"%s\", shot: %d)... OK\n", ptaskConfig->taskName, ptaskConfig->mdsTree, shotNumber);
#endif
	}
	return 1;
}


static int mdsDisconnectAndClose(drvVXI1564a_taskConfig *ptaskConfig)
{
        int status; /* Will contain the status of the data access operation */
	char serverName[60];
	char treeName[60];
	int		shotNumber;

	shotNumber =(int) ptaskConfig->shotNumber;

	strcpy(serverName, ptaskConfig->mdsServer);
	strcpy(treeName,ptaskConfig->mdsTree);

	if( ptaskConfig->opMode == OPMODE_REMOTE ) {
        	/* Connect to MDSplus  ptaskConfig->mdsStatus  */
        	status = MdsClose(treeName, &shotNumber);
        	if ( !statusOk(status) )
        	{
                	fprintf(stderr,"Error closing tree for shot %d: %s.\n",shotNumber, MdsGetMsg(status));
			return -1;
 	        }
		MdsDisconnect();
#ifdef DEBUG
		epicsThreadSleep(0.1);
		epicsPrintf("MDSplus Data Closing Shot Number : %d --- Remote Mode!!! \n", shotNumber);
        	epicsPrintf("MdsDisconnect(Server : %s  ... OK\n", serverName);
#endif
/*		status = completeDataPut(); Used after test complated..... You must  */
	}
	else {
        	status = MdsClose(treeName, &shotNumber);
        	if ( !statusOk(status) )
        	{
                fprintf(stderr,"Error closing tree for shot %d: %s.\n",shotNumber, MdsGetMsg(status));
		return -1;
 	        }
#ifdef DEBUG
		epicsThreadSleep(0.1);
		epicsPrintf("MDSplus Data Closing Shot Number : %d --- Local Mode!!! \n",shotNumber);
#endif
	}
	return 1;
}

int completeDataPut()
{
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
        int socket; /* Will contain the handle to the remote mdsip server */

        char buf[512];

        int tstat, len;
        int idesc = descr(&dtypeLong, &tstat, &null);


        /* Connect to MDSplus */
        socket = MdsConnect("172.17.100.200:8300");
        if ( socket == -1 )
        {
                epicsPrintf("Error connecting to mdsip server(%s).\n", "172.17.100.200:8300");
                return 0;
        }
        epicsPrintf("\nMDSplus >>> MdsConnect(\"%s\")... OK\n", "172.17.100.200:8300");

        sprintf(buf, "TCL(\"setevent MC\")");
        status = MdsValue(buf, &idesc, &null, &len);
        if( !statusOk(status) )
        {
                epicsPrintf("Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
                return 0;
        }
        MdsDisconnect("172.17.100.200:8300");
        return 1;
}
#if 0
static int send_dataTree(drvVXI1564a_taskConfig *ptaskConfig)
{
	drvVXI1564a_channelConfig *pchannelConfig;
/*
	drvVXI1564a_wfChannelConfig *pwfChannelConfig = NULL;
	uInt32		startIndex;
	uInt32		endIndex;
*/
	unsigned long long int start, stop;

        start = drvVXI1564a_rdtsc();

	ptaskConfig->taskStatus &= ~TASK_DATADIVIDE;
	ptaskConfig->taskStatus |= TASK_DATASTORE;
	scanIoRequest(ptaskConfig->ioScanPvt);
		
	uInt32 		usedChannel;
	float64 	sampleRate;
	float64		sampleCount;
	float64		samplePreCount;
	float64 	blipTime;
	float64		daqTime;
	uInt32		shotNumber;

	int i;
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
	char serverName[60];
	char treeName[60];

	int countTest;
/*	float data[5000], time[5000];  Array of floats used for signal */
	float *data; /*  Array of floats used for signal */
/*	signed short *data;   Array of signed short used for signal */

	int dataDesc; /* Signal descriptor */
	int rateDesc, startDesc, endDesc, voltRangeDesc;     /* Sample Num, Sample Rate, Data Start Time  */

	float realTimeGap, startTime, endTime, voltRange;
	int totalCounts;   /* Important variable type "int" ... about that dataDescr.. */

	usedChannel = (uInt32)ellCount(ptaskConfig->pchannelConfig);
	sampleRate = ptaskConfig->samplingRate;
	sampleCount = ptaskConfig->sampleCount;
	samplePreCount = ptaskConfig->samplePreCount;
	totalCounts = sampleCount + samplePreCount;
	blipTime = ptaskConfig->blipTime;
        daqTime = ptaskConfig->daqTime;
	shotNumber = ptaskConfig->shotNumber;

	strcpy(serverName, ptaskConfig->mdsServer);
	strcpy(treeName,ptaskConfig->mdsTree);

#ifdef DEBUG
	epicsPrintf("Server Name :%s \n", serverName);
	epicsPrintf("Tree Name :%s \n", treeName);
#endif

/*	realTimeGap = 1.0 / sampleRate;   */
	realTimeGap = ptaskConfig->samplePeriod;

        data = (float *)malloc(sizeof(float)*totalCounts); 
#if 0
/*      data = (float *)malloc(sizeof(float)*g_sampleDataCnt);     */
/*      data = (float *)malloc(4*g_sampleDataCnt);   Data Size is int 4byte But float is 8Byte */
/*      memset(data, 0, sizeof(float)*g_sampleDataCnt);  */
        data = (float64 *)malloc(sizeof(float64)*totalCounts); 
        data = (signed short *)malloc(sizeof(signed short)*totalCounts);
        data = (float *)malloc(sizeof(float)*totalCounts); 
        time = (float *)malloc(sizeof(float)*totalCounts); 
#endif
/*
        range10Desc = descr(&dtypeFloat, &range10, &null);
        range5Desc = descr(&dtypeFloat, &range5, &null);
        sampleDesc = descr(&dtypeFloat, &totalCounts, &null);   Last Data Zero .. .. Errro
*/
        rateDesc = descr(&dtypeFloat, &realTimeGap, &null);
        startDesc = descr(&dtypeFloat, &startTime, &null);
        endDesc = descr(&dtypeFloat, &endTime, &null);


	if( ptaskConfig->opMode == OPMODE_REMOTE ) {
        	/* Connect to MDSplus  ptaskConfig->mdsStatus  */
		epicsPrintf("MDSplus Data Put ---- Remote Mode!!! \n");
#if 0
		status = MdsConnect(serverName);
        	if ( status == -1 )
        	{
                	fprintf(stderr,"Error connecting to mdsip server(%s).\n", serverName);
			free(data);
                	return -1;
        	}
        	epicsPrintf("\%s >>> MdsConnect(\"%s\")... OK\n", ptaskConfig->taskName, serverName);
	        /* Open tree */
	        status = MdsOpen(treeName, &shotNumber);
        	if ( !statusOk(status) )
        	{
                	fprintf(stderr,"Error opening tree for shot %ld: %s.\n",shotNumber, MdsGetMsg(status));
                	MdsDisconnect();
			free(data);
                	return -1;
        	}
#endif
#ifdef DEBUG
        	epicsPrintf("\n%s >>> MdsOpen(\"%s\", shot: %ld)... OK\n", ptaskConfig->taskName, ptaskConfig->mdsTree, shotNumber);
#endif
/*	        startTime = (daqTime - blipTime) * sampleRate - 1;   This value is index of the count for the start Time ..... */
/*              This value is index of the count for the start Time .....And preTrigger counts           */
	        startTime = (daqTime - blipTime) * sampleRate - samplePreCount - 1;  
/*              endTime = (sampleReCount-1) + startTime;   -1 count */
/*              endTime = totalCounts + startTime-2;       -2 count  */
                endTime = totalCounts + startTime - samplePreCount -2;  /* samplePreCount and -2 count  */

	}
	else {
		epicsPrintf("MDSplus Data Put ---- Local Mode!!! \n");
	        /* Local Mode is Direct Open tree */
#if 0
	        status = MdsOpen(treeName, &shotNumber);
        	if ( !statusOk(status) )
        	{
                	fprintf(stderr,"Error opening tree for shot %ld: %s.\n",shotNumber, MdsGetMsg(status));
			free(data);
                	return -1;
        	}
#ifdef DEBUG
        	epicsPrintf("\n%s >>> MdsOpen(\"%s\", shot: %ld)... OK\n", ptaskConfig->taskName, ptaskConfig->mdsTree, shotNumber);
#endif
#endif
/*              startTime = 0;          Data Zero Start Time */
/*              startTime = -1;         Data Zero Start Time */
                startTime = -1 -samplePreCount;   /* Data Zero Start Time with preTrigger sample Counts */
/*              endTime = totalCounts -2;            endTime = sampleReCount- 2;  Last data Zero Over Count.  */
                endTime = totalCounts -samplePreCount -2;  /* endTime = sampleReCount- 2 with preTrigger sample Counts; Last data Zero Over Count.*/
	}

   /*   MDSplus Data Put Function Start   */
	countTest = 5000;
	pchannelConfig = (drvVXI1564a_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
        /* Local Mode is Direct Open tree */
	while(pchannelConfig)
	{
		drvVXI1564a_aoChannelConfig *paoChannelConfig = (drvVXI1564a_aoChannelConfig*) pchannelConfig;
		i=paoChannelConfig->chanNum;
		epicsPrintf("channel Number :%d paoChannel\n ",i);
#if 0
		startIndex = i*totalCounts;
		endIndex = (i*totalCounts + (totalCounts-1));
		memcpy(data, &paoChannelConfig->vxi1564aChBuffer[0], sizeof(signed short)*totalCounts); 
		memcpy(data, &paoChannelConfig->vxi1564aChBuffer[0], sizeof(float)*totalCounts); 
/* 
        	memcpy(data, &ptaskConfig->vxi1564aBuffer[startIndex], sizeof(float64)*totalCounts);
*/
#endif
		memcpy(data, &paoChannelConfig->vxi1564aChBuffer[0], sizeof(float)*totalCounts); 
		voltRange =(float)paoChannelConfig->voltRangeSet/32768;
#ifdef DEBUG
		epicsPrintf("channel Number :%d, Volt Range : %f, calVoltRange :%f. \n ",i,paoChannelConfig->voltRangeSet, voltRange);
		epicsPrintf("sampleTotalCounts :%d, SampleRate :%f, StartTime : %f, EndTime :%f. \n ",
											 totalCounts, sampleRate ,startTime, endTime);
#endif
		epicsThreadSleep(0.5);
#if 0
		int countTest;
		countTest = 5000;
		for(j=0; j<5000; j++){
			data[j]=(float)j * voltRange;
			time[j]=j/sampleRate;
		}
		printf("\n");
		epicsThreadSleep(0.5);
		printf("\n");
		for(j=5000 -10; j<5000; j++){
			printf("  %f, ",data[j]);
		}
		printf("\n");
		dataDesc = descr(&dtypeFloat,data, &countTest, &null);
		timeDesc = descr(&dtypeFloat,time, &countTest, &null);
        	status = MdsPut("\\MC1P10:FOO", "BUILD_SIGNAL($,,$)", &dataDesc, &timeDesc, &null);
#endif
		voltRangeDesc = descr(&dtypeFloat, &voltRange, &null);
		dataDesc = descr(&dtypeFloat, data, &totalCounts, &null);
#ifdef DEBUG
		epicsPrintf("channel Number :%d paoChannel-- data desc after delay \n ",i);
#endif
		epicsThreadSleep(0.5);

        	status = MdsPut(paoChannelConfig->treeNode, "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &voltRangeDesc, &dataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);


		if ( !statusOk(status) )
        	{
        		fprintf(stderr,"Error writing signal ch%d : %s\n", i, MdsGetMsg(status));
        	}
#ifdef DEBUG
        	fprintf(stdout,"ch%d ch_Name:%s ,Tree Node:%s \n", i, paoChannelConfig->chanName, paoChannelConfig->treeNode);
#endif
		pchannelConfig = (drvVXI1564a_channelConfig*) ellNext(&pchannelConfig->node);
		epicsThreadSleep(0.5);
	}
/* Close Current Shot tree  */
#if 0
        status = MdsClose(treeName, &shotNumber);
        if ( !statusOk(status) )
        {
                fprintf(stderr,"Error closing tree for shot %ld: %s.\n",shotNumber, MdsGetMsg(status));
		free(data);
		return 1;
        }
 /*  MDSplus Disconnect Function  */

	if( ptaskConfig->opMode == OPMODE_REMOTE ) {
        	/* Connect to MDSplus  ptaskConfig->mdsStatus  */
		MdsDisconnect();
		free(data);
               	return -1;
	}
#endif
        fprintf(stdout,"\nMDSplus >>> Data size 8 : %dKB/ch\n", (8*totalCounts)/1024 );
        fprintf(stdout,"\nMDSplus >>> Data size 64: %dKB/ch\n", (64*totalCounts)/1024 );
        fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
        epicsPrintf("\n>>> MDSplus Data Put Function Complated......... ... DONE\n");
	free(data);
	stop = drvVXI1564a_rdtsc();
	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;
	ptaskConfig->taskStatus &= ~TASK_DATASTORE;
	ptaskConfig->taskStatus |= TASK_READYSHOT;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	scanIoRequest(ptaskConfig->ioScanPvt);
	epicsPrintf("Data write to MDSplus Node : Current Time %lf msec \n", 1.E-3 * (double)drvVXI1564a_intervalUSec(start, stop));
	return 1;
}
#endif

static int create_newTree(drvVXI1564a_taskConfig *ptaskConfig)
{
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */

	int	shotNumber;
        char buf[512];
	char serverName[60];
	char treeName[60];


   int tstat, len;
   int idesc = descr(&dtypeLong, &tstat, &null);
        
	strcpy(serverName, ptaskConfig->mdsServer);
	strcpy(treeName, ptaskConfig->mdsTree);
	shotNumber = ptaskConfig->shotNumber;

	if( ptaskConfig->opMode == OPMODE_TEST ) {
       	/* Connect to MDSplus  ptaskConfig->mdsStatus Local Mode is not Mdsconnect */
/*		status = MdsConnect(serverName);
        	if ( status == -1 )
        	{
                	fprintf(stderr,"Error connecting to mdsip server(%s).\n", ptaskConfig->mdsServer);
                	return -1;
        	}
        	epicsPrintf("\%s >>> MdsConnect(\"%s\")... OK\n", ptaskConfig->taskName, ptaskConfig->mdsServer);
*/
                sprintf(buf, "TCL(\"set tree %s/shot=-1\")", treeName);
                status = MdsValue(buf, &idesc, &null, &len);
                if ( !statusOk(status) )
                {
                        fprintf(stderr,"Error TCL command: %s.\n",MdsGetMsg(status));
                        MdsDisconnect();
                        return -1;
                }
                sprintf(buf, "TCL(\"create pulse %d\")", shotNumber);
                status = MdsValue(buf, &idesc, &null, &len);
                if ( !statusOk(status) )
                {
                       	fprintf(stderr,"Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
                       	MdsDisconnect();
                       	return -1;
                }
                status = MdsValue("TCL(\"close\")", &idesc, &null, &len);

                epicsPrintf(" %s >>> Create new tree(\"%d\")... OK\n", ptaskConfig->taskName, shotNumber);
       	}
	else epicsPrintf(" %s >>> Operation Mode is Remote Mode. So.. Do not Create Tree ..\n", ptaskConfig->taskName);
	return 1;
}

static void set_sampleCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 sampleCount)
{
	unsigned long long int start, stop;

	unsigned long tempHighCount, tempLowCount;

        start = drvVXI1564a_rdtsc();

        ptaskConfig->sampleCount = sampleCount;
        tempHighCount= sampleCount;
        tempLowCount= sampleCount;

        ptaskConfig->sampleCountHigh = (tempHighCount & 0xFF0000) >> 16;
        ptaskConfig->sampleCountLow = tempLowCount & 0xFFFF;

#ifdef DEBUG
        epicsPrintf("Setting Sample Count : %f, Bit devide High : 0x%4X, Low 0x%4X. \n",
                                                ptaskConfig->sampleCount, ptaskConfig->sampleCountHigh, ptaskConfig->sampleCountLow);
#endif
	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}

static void set_samplePreCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplePreCount)
{
	unsigned long long int start, stop;

	unsigned long tempHighPreCount, tempLowPreCount;

        start = drvVXI1564a_rdtsc();

        ptaskConfig->samplePreCount = samplePreCount;
        tempHighPreCount= samplePreCount;
        tempLowPreCount= samplePreCount;

        ptaskConfig->preTriggerHigh = (tempHighPreCount & 0xFF0000) >> 16;
        ptaskConfig->preTriggerLow = tempLowPreCount & 0xFFFF;

#ifdef DEBUG
        epicsPrintf("Setting Sample Pre Trigger Count : %f, Bit devide High : 0x%4X, Low 0x%4X. \n",
                                                ptaskConfig->samplePreCount, ptaskConfig->preTriggerHigh, ptaskConfig->preTriggerLow);
#endif
	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_channelRange(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *paoChannelConfig, uInt16 setRange)
{
	unsigned long long int start, stop;
	
	unsigned short tempSetRange, tempVF12, tempVF34, val12, val34, valGain, maskGain;

	start = drvVXI1564a_rdtsc();
	
#ifdef DEBUG
        epicsPrintf("1 Setting Channel Volt Range Before Task : %s, Chan Num : %d, Val12: 0x%4X, Val34: 0x%4X, setValue:%d \n", 
										ptaskConfig->taskName, paoChannelConfig->chanNum,
										ptaskConfig->voltFilter12, ptaskConfig->voltFilter34,setRange);
#endif
	tempSetRange = setRange;
	tempVF12 = ptaskConfig->voltFilter12;
	tempVF34 = ptaskConfig->voltFilter34;
#ifdef DEBUG
        epicsPrintf("2 Setting Channel Volt Range Before Task : %s, Chan Num : %d, Val12: 0x%4X, Val34: 0x%4X, setValue:%d \n", 
						ptaskConfig->taskName, paoChannelConfig->chanNum, tempVF12, tempVF34, tempSetRange);
		
#endif
	if (paoChannelConfig->chanNum == 0 || paoChannelConfig->chanNum == 2){
		maskGain=0x700;
		tempSetRange <<= 8;
		if(paoChannelConfig->chanNum == 0){
			tempVF12 = tempVF12 & 0xF8FF;
			ptaskConfig->voltFilter12 = tempVF12 | tempSetRange;
			/*  Reload setting Value and Chanel Gain value put.  */
#ifdef DEBUG
			epicsPrintf(">>>3 Set Ch 1 voltFilter12:0x%4X,tempSetRange:0x%4X,tempVF12:0x%4X \n",
								ptaskConfig->voltFilter12,tempSetRange,tempVF12);
#endif
			val12 = ptaskConfig->voltFilter12;
			valGain = val12 & maskGain;
#ifdef DEBUG
			epicsPrintf(">>>4 Set Ch 1 val12:0x%4X,maskGain:0x%4X, Gain:0x%4X \n",val12,maskGain, valGain);
#endif
			valGain >>= 8;
#ifdef DEBUG
			epicsPrintf(">>>5 Set Ch 1 val12:0x%4X,maskGain:0x%4X, shift 8 Gain:0x%4X \n",val12,maskGain, valGain);
#endif
		}
		else {
			tempVF34 = tempVF34 & 0xF8FF;
			ptaskConfig->voltFilter34 = tempVF34 | tempSetRange;
			/*  Reload setting Value and Chanel Gain value put.  */
			val34 = ptaskConfig->voltFilter34;
			valGain = val34 & maskGain;
			valGain >>= 8;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 3 Gain:0x%4X \n", valGain);
#endif
		}
	}
	if (paoChannelConfig->chanNum == 1 || paoChannelConfig->chanNum == 3){
		maskGain=0x7;
		if(paoChannelConfig->chanNum == 1){
			tempVF12 = tempVF12 & 0xFFF8;
			ptaskConfig->voltFilter12 = tempVF12 | tempSetRange;
			/*  Reload setting Value and Chanel Gain value put.  */
			val12 = ptaskConfig->voltFilter12;
			valGain = val12 & maskGain;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 2 Gain:0x%4X \n", valGain);
#endif
		}
		else {
			tempVF34 = tempVF34 & 0xFFF8;
			ptaskConfig->voltFilter34 = tempVF34 | tempSetRange;
			/*  Reload setting Value and Chanel Gain value put.  */
			val34 = ptaskConfig->voltFilter34;
			valGain = val34 & maskGain;
#ifdef DEBUG
			epicsPrintf(">>> Set Ch 4 Gain:0x%4X \n", valGain);
#endif
		}
	}
	switch((int)valGain ) {
		case 0 :  paoChannelConfig->voltRangeSet = 0.0625 ;     break; /* 000 - 62.5mV */
    		case 1 :  paoChannelConfig->voltRangeSet = 0.25   ;     break; /* 001 - 0.25V  */ 
    		case 2 :  paoChannelConfig->voltRangeSet = 1.0    ;     break; /* 010 - 1.0V   */
    		case 3 :  paoChannelConfig->voltRangeSet = 4.0    ;     break; /* 011 - 4.0V   */
    		case 4 :  paoChannelConfig->voltRangeSet = 16     ;     break; /* 100 - 16V    */
    		case 5 :  paoChannelConfig->voltRangeSet = 64     ;     break; /* 101 - 64V    */
    		case 6 :  paoChannelConfig->voltRangeSet = 256    ;     break; /* 110 - 256V   */
    		case 7 :  paoChannelConfig->voltRangeSet = 256    ;     break; /* 111 -Also 256V  */
		default : epicsPrintf("\n>>>ERROR Set Gain(Volt) Ch : %d Set Value : %d \n", paoChannelConfig->chanNum, valGain);
        	return;
	}
	
#ifdef DEBUG
        epicsPrintf("Setting Channel After Volt Range Channel Name : %s, Set Val: %f \n", paoChannelConfig->chanName, paoChannelConfig->voltRangeSet);
#endif

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_channelFilter(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *paoChannelConfig, uInt16 setFilter)
{
	unsigned long long int start, stop;
	unsigned short tempSetFilter, tempVF12, tempVF34, val12, val34, valFilter, maskFilter;

	start = drvVXI1564a_rdtsc();
	tempSetFilter = setFilter;
	tempVF12 = ptaskConfig->voltFilter12;
	tempVF34 = ptaskConfig->voltFilter34;
#ifdef DEBUG
        epicsPrintf("Setting Channel Filter Range Before Task : %s, Chan Num : %d, Val12: 0x%4X, Val34: 0x%4X, setValue : %d \n", 
									ptaskConfig->taskName, paoChannelConfig->chanNum, 
									ptaskConfig->voltFilter12, ptaskConfig->voltFilter34, setFilter);

#endif
	if (paoChannelConfig->chanNum == 0 || paoChannelConfig->chanNum == 2){
		maskFilter=0x7000;
		tempSetFilter <<= 12;
		if(paoChannelConfig->chanNum == 0){
			tempVF12 = tempVF12 & 0x8FFF;
			ptaskConfig->voltFilter12 = tempVF12 | tempSetFilter;
			/*  Reload setting Value and Chanel Filter value put.  */
			val12 = ptaskConfig->voltFilter12;
			valFilter = val12 & maskFilter;
			valFilter >>= 12;
			epicsPrintf(">>> Set Ch 1 Filter:0x%4X \n", valFilter);
		}
		else {
			tempVF34 = tempVF34 & 0x8FFF;
			ptaskConfig->voltFilter34 = tempVF34 | tempSetFilter;
			/*  Reload setting Value and Chanel Filter value put.  */
			val34 = ptaskConfig->voltFilter34;
			valFilter = val34 & maskFilter;
			valFilter >>= 12;
			epicsPrintf(">>> Set Ch 3 Filter:0x%4X \n", valFilter);
		}
	}
	if (paoChannelConfig->chanNum == 1 || paoChannelConfig->chanNum == 3){
		maskFilter=0x70;
		tempSetFilter <<= 4;
		if(paoChannelConfig->chanNum == 1){
			tempVF12 = tempVF12 & 0xFF8F;
			ptaskConfig->voltFilter12 = tempVF12 | tempSetFilter;
			/*  Reload setting Value and Chanel Filter value put.  */
			val12 = ptaskConfig->voltFilter12;
			valFilter = val12 & maskFilter;
			valFilter >>= 4;
			epicsPrintf(">>> Set Ch 2 Filter:0x%4X \n", valFilter);
		}
		else {
			tempVF34 = tempVF34 & 0xFF8F;
			ptaskConfig->voltFilter34 = tempVF34 | tempSetFilter;
			/*  Reload setting Value and Chanel Filter value put.  */
			val34 = ptaskConfig->voltFilter34;
			valFilter = val34 & maskFilter;
			valFilter >>= 4;
			epicsPrintf(">>> Set Ch 4 Filter:0x%4X \n", valFilter);
		}
	}

	switch((int)valFilter ) {
    		case 0 :  paoChannelConfig->filterRangeSet = 1.5 ;     break; /* 000 - 1.5kHz    */
 		case 1 :  paoChannelConfig->filterRangeSet = 6   ;     break; /* 001 - 6kHz      */ 
    		case 2 :  paoChannelConfig->filterRangeSet = 25  ;     break; /* 010 - 25kHz     */
    		case 3 :  paoChannelConfig->filterRangeSet = 100 ;     break; /* 011 - 100kHz    */
    		case 7 :  paoChannelConfig->filterRangeSet = 0   ;     break; /* 111 - NO filter */
		default : epicsPrintf("\n>>>ERROR  Set Filter(kHz) Ch : %d Set Value : %d \n", paoChannelConfig->chanNum, valFilter);
       		return;
	}
        epicsPrintf("Setting Channel After Filter Channel Name : %s, Set Val: %f \n", paoChannelConfig->chanName, paoChannelConfig->filterRangeSet);
	epicsThreadSleep(0.1);
	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_sampleMode(drvVXI1564a_taskConfig *ptaskConfig, int sampleMode)
{
	/* data acquisition , finite number or continue   But Not Used */
	unsigned long long int start, stop;

	start = drvVXI1564a_rdtsc();

	ptaskConfig->sampleMode = sampleMode;

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_triggerType(drvVXI1564a_taskConfig *ptaskConfig, int triggerType)
{
	/* triggerTypeVXI1564a  Not used  */
	unsigned long long int start, stop;

	start = drvVXI1564a_rdtsc();


	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_shotNumber(drvVXI1564a_taskConfig *ptaskConfig, int shotNumber)
{
	unsigned long long int start, stop;

	start = drvVXI1564a_rdtsc();

	ptaskConfig->shotNumber = shotNumber;

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_blipTime(drvVXI1564a_taskConfig *ptaskConfig, float64 blipTime)
{
	unsigned long long int start, stop;

	start = drvVXI1564a_rdtsc();

	ptaskConfig->blipTime = blipTime;

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
static void set_daqTime(drvVXI1564a_taskConfig *ptaskConfig, float64 daqTime)
{
	unsigned long long int start, stop;

	start = drvVXI1564a_rdtsc();

	ptaskConfig->daqTime = daqTime;

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}

static void set_operMode(drvVXI1564a_taskConfig *ptaskConfig, unsigned int operMode)
{
/*  Not used - Include Function Remote or Local Operation in ExTrigger Function */
	unsigned long long int start, stop;

	start = drvVXI1564a_rdtsc();
/*  Just Test Remote store on Internal Trigger function */
	ptaskConfig->opMode = operMode;
	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}
#if 0
static void set_samplingRateALLTasks(float64 smaplingRate)
{
	/* All Task Sampling Rate change Not used  */
	drvVXI1564a_taskConfig *ptaskConfig = (drvVXI1564a_taskConfig*) ellFirst(pdrvVXI1564aConfig->ptaskList);

	while(ptaskConfig) {
		if(ptaskConfig->taskStatus & TASK_INITIALIZED) {
			set_samplingRate(ptaskConfig, smaplingRate);
		}
		ptaskConfig = (drvVXI1564a_taskConfig*) ellNext(&ptaskConfig->node);
	}
}
#endif
static void set_samplePeriod(drvVXI1564a_taskConfig *ptaskConfig, float64 samplePeriod)
{
/*
 VXI1564a Sampling Clock source used Internal Clock 10MHz source.
  The min and max sampling periods possible. (1.25 micro sec and 0.8 sec, but you min rate is 1.3micro sec when you used 10M clock) 
  So, we have to change from sampling rate(Hz) to divider(integer) counts for register level.
  How do you get?  10M / n(divider-integer) = sampling rate(Hz). 
  So, you can get.  10M / sampling rate(Hz) = n(divider-integer) value.  
*/
	unsigned long long int start, stop;

	unsigned long tempHighRate, tempLowRate;
	
        start = drvVXI1564a_rdtsc();

	ptaskConfig->samplePeriod = samplePeriod;
	ptaskConfig->samplingRate = (double)1./samplePeriod;
/* I had mistake the calc out about tempLowRate value when I calc this "tempHighRate= 10000000. * samplePeriod;".
   On 2010 Experiment, I was wrong Oscillator divider value at 500kHz and 250kHz sampling time. 
   Because, The computer have mistake and missing precision about Floating point and Fixed point.   */		
        tempHighRate= samplePeriod / 0.0000001; /* Internal Clock Source 10M - 10000000 and remember bits calc */
        tempLowRate= samplePeriod / 0.0000001; /* Internal Clock Source 10M - 10000000 and remember bits calc */

	epicsPrintf("Check the Sample Period########## .8f:%.8f, and TempLowRate############:%lu \n",samplePeriod,tempLowRate);

        ptaskConfig->sampleRateHigh = (tempHighRate & 0xFF0000) >> 16;
        ptaskConfig->sampleRateLow = tempLowRate & 0xFFFF;
	epicsPrintf("Sample Period Low Reg-----########################################: 0x%4X \n", ptaskConfig->sampleRateLow);
#ifdef DEBUG
	epicsPrintf("set_taskTiming:Name: %s, Rate:%f (Hz),Period(Interval):%.9f (sec), Counts:%f, chanNum:%d\n",
		 				 ptaskConfig->taskName, ptaskConfig->samplingRate, ptaskConfig->samplePeriod,
						 ptaskConfig->sampleCount, ellCount(ptaskConfig->pchannelConfig));
	epicsPrintf("Double 1 print :%.9f \n",(double)1.);
#endif /* DEBUG */

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvVXI1564a_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;

}


static int make_drvConfig(void)
{
	if(pdrvVXI1564aConfig) return 0;

	pdrvVXI1564aConfig = (drvVXI1564aConfig*) malloc(sizeof(drvVXI1564aConfig));
	if(!pdrvVXI1564aConfig) return 1;

	pdrvVXI1564aConfig->ptaskList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvVXI1564aConfig->ptaskList) return 1;
	else ellInit(pdrvVXI1564aConfig->ptaskList);

	pdrvVXI1564aConfig->pdestroiedList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvVXI1564aConfig->pdestroiedList) return 1;
	else ellInit(pdrvVXI1564aConfig->pdestroiedList);

	return 0;
}

static drvVXI1564a_taskConfig* make_taskConfig(char *taskName, char *serverName, char *treeName)
{
	drvVXI1564a_taskConfig *ptaskConfig = NULL;
	char task_name[60];
	char server_name[60];
	char tree_name[60];

	strcpy(task_name, taskName);
	strcpy(server_name, serverName);
	strcpy(tree_name, treeName);

	ptaskConfig = (drvVXI1564a_taskConfig*) malloc(sizeof(drvVXI1564a_taskConfig));
	if(!ptaskConfig) return ptaskConfig;

	ptaskConfig->taskHandle = 0;
	strcpy(ptaskConfig->taskName, task_name);
	strcpy(ptaskConfig->mdsServer, server_name);
	strcpy(ptaskConfig->mdsTree, tree_name);
	ptaskConfig->taskLock = epicsMutexCreate();
	ptaskConfig->bufferLock = epicsMutexCreate();
	scanIoInit(&ptaskConfig->ioScanPvt);
	scanIoInit(&ptaskConfig->ioScanPvt); 
/*	ptaskConfig->samplingRate = INIT_SAMPLE_RATE_VXI1564a;       */
/*	ptaskConfig->cviCallback = (int32 _VI_FUNC*) taskCallback;    */
/*	ptaskConfig->pcontrolStoreConfig = NULL;  */

	ptaskConfig->pcontrolThreadConfig = NULL;

	ptaskConfig->adjTime_smplRate_Usec = 0.;
	ptaskConfig->adjTime_lowPass_Usec = 0.;

	ptaskConfig->adjCount_smplRate = 0;
	ptaskConfig->adjCount_lowPass = 0;
#if 1
	ptaskConfig->pdataPoolList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!ptaskConfig->pdataPoolList) return NULL;
	else ellInit(ptaskConfig->pdataPoolList);
#endif
	ptaskConfig->pchannelConfig = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!ptaskConfig->pchannelConfig) return NULL;
	else ellInit(ptaskConfig->pchannelConfig);

	epicsMutexLock(ptaskConfig->bufferLock);	
	ptaskConfig->taskStatus = TASK_NOT_INIT;
	epicsMutexUnlock(ptaskConfig->bufferLock);

	return ptaskConfig;

}

static drvVXI1564a_taskConfig* find_taskConfig(char *taskName)
{
	drvVXI1564a_taskConfig *ptaskConfig = NULL;
	char task_name[60];

	strcpy(task_name, taskName);
#if 0
	epicsPrintf("Epics Message Find taskConfig Name:%s \n",task_name);
#endif
	ptaskConfig = (drvVXI1564a_taskConfig*) ellFirst(pdrvVXI1564aConfig->ptaskList);
	while(ptaskConfig) {
		if(!strcmp(ptaskConfig->taskName, task_name)) return ptaskConfig;
  		ptaskConfig = (drvVXI1564a_taskConfig*) ellNext(&ptaskConfig->node);
	}

    	return ptaskConfig;
}

static drvVXI1564a_channelConfig* find_channelConfig(drvVXI1564a_taskConfig *ptaskConfig, char *chanName)
{
	drvVXI1564a_channelConfig *pchannelConfig = NULL;
	char channel_name[60];

	strcpy(channel_name, chanName);

	pchannelConfig = (drvVXI1564a_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		if(!strcmp(pchannelConfig->chanName, channel_name)) return pchannelConfig;
		pchannelConfig = (drvVXI1564a_channelConfig*) ellNext(&pchannelConfig->node);
	}

	return pchannelConfig;
}
#if 0
static unsigned find_channelIndex(drvVXI1564a_taskConfig *ptaskConfig, char *chanName)
{
	drvVXI1564a_channelConfig *pchannelConfig = find_channelConfig(ptaskConfig, chanName);
	if(!pchannelConfig) return -1;
	else return pchannelConfig->chanIndex;
}
#endif


static void print_channelConfig(drvVXI1564a_taskConfig *ptaskConfig, int level)
{
	drvVXI1564a_channelConfig *pchannelConfig;
	pchannelConfig = (drvVXI1564a_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	epicsPrintf("\n");
	epicsPrintf(" *********************************************** \n");
	epicsPrintf("   pchannelConfig Counts :%d \n", ellCount(ptaskConfig->pchannelConfig));
	while(pchannelConfig) {
		epicsPrintf("    %s", pchannelConfig->chanName);
		if(level>2) {
			switch(pchannelConfig->channelType) {
				case VXI1564a_CHANNEL:
/*					epicsPrintf("\tAI1564a Channel, status:0x%x, index: %d, gain: %f, cut-off: %f, val: %f Volts",
						    pchannelConfig->channelStatus,
						    pchannelConfig->chanIndex,
						    pchannelConfig->gain,
						    pchannelConfig->cutOffFreq,
						    ptaskConfig->pdata[pchannelConfig->chanIndex]);
*/
					if(level>3) {
						drvVXI1564a_aoChannelConfig *paoChannelConfig
							= (drvVXI1564a_aoChannelConfig*) pchannelConfig;
						epicsPrintf("\n\t Channel Name: %s, Tree Name: %s ",paoChannelConfig->chanName,
													paoChannelConfig->treeNode);
						if(level>4) {
						epicsPrintf("\n\t Channel Number: %d Ch, Volt Range :%f V, Filter Range :%f kHz ",
													paoChannelConfig->chanNum,
													paoChannelConfig->voltRangeSet,
													paoChannelConfig->filterRangeSet);
/*							for(i=0; i<10; i++){
                       						 epicsPrintf("%lf ,",paoChannelConfig->vxi1564aChBuffer[i]);
               						 }
                					epicsPrintf("\n");
*/
						}
					}
					break;
				default:
					break;
			}
		}

		epicsPrintf("\n");
  		pchannelConfig = (drvVXI1564a_channelConfig*) ellNext(&pchannelConfig->node);
	}

	return;

}



static drvVXI1564a_controlThreadConfig*  make_controlThreadConfig(drvVXI1564a_taskConfig *ptaskConfig)
{
/* rm TG 20110706	int dataSemStatus;  */
	drvVXI1564a_controlThreadConfig *pcontrolThreadConfig  = NULL;

	pcontrolThreadConfig  = (drvVXI1564a_controlThreadConfig*)malloc(sizeof(drvVXI1564a_controlThreadConfig));
	if(!pcontrolThreadConfig) return pcontrolThreadConfig;

	sprintf(pcontrolThreadConfig->threadName, "%s_ctrl", ptaskConfig->taskName);

	pcontrolThreadConfig->threadPriority = epicsThreadPriorityLow;
	pcontrolThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pcontrolThreadConfig->threadFunc      = (EPICSTHREADFUNC) controlThread;
	pcontrolThreadConfig->threadParam     = (void*) ptaskConfig;

	/* Add function for store pcontrolThreadConfig parameter   */
	ptaskConfig->pcontrolThreadConfig = pcontrolThreadConfig;

	pcontrolThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));
	if(pcontrolThreadConfig->threadQueueId) ptaskConfig->taskStatus |= TASK_CONTQUEUE;

	epicsThreadCreate(pcontrolThreadConfig->threadName,
			  pcontrolThreadConfig->threadPriority,
			  pcontrolThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pcontrolThreadConfig->threadFunc,
			  (void*) pcontrolThreadConfig->threadParam);

	epicsPrintf("Epics Message Create thread Name:%s, ID :%p \n",pcontrolThreadConfig->threadName
								    ,pcontrolThreadConfig->threadQueueId);
/* remove TG 20110706	dataSemStatus = sem_init( &DATAsemaphore, 0, 0);
	dataSemStatus = sem_post(&DATAsemaphore);  */
	DATAsemaphore = epicsEventCreate(epicsEventEmpty);
	epicsEventSignal (DATAsemaphore);
	return pcontrolThreadConfig;
}
static drvVXI1564a_controlThreadConfig*  make_eventMonitorConfig(drvVXI1564a_taskConfig *ptaskConfig)
{
	drvVXI1564a_controlThreadConfig *peventMonitorConfig  = NULL;

	peventMonitorConfig  = (drvVXI1564a_controlThreadConfig*)malloc(sizeof(drvVXI1564a_controlThreadConfig));
	if(!peventMonitorConfig) return peventMonitorConfig;

	sprintf(peventMonitorConfig->threadName, "%seventMctrl",ptaskConfig->taskName);
#if 0
	peventMonitorConfig->threadPriority = epicsThreadPriorityLow;
	peventMonitorConfig->threadPriority = epicsThreadPriorityHigh;
	peventMonitorConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
#endif
	peventMonitorConfig->threadPriority = epicsThreadPriorityHigh;
	peventMonitorConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	peventMonitorConfig->threadFunc      = (EPICSTHREADFUNC) eventMonitorFunc;
	peventMonitorConfig->threadParam     = (void*) ptaskConfig;

	ptaskConfig->peventMonitorConfig = peventMonitorConfig;

	peventMonitorConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));
	if(peventMonitorConfig->threadQueueId) ptaskConfig->taskStatus |= TASK_EVENTQUEUE;   

	epicsThreadCreate(peventMonitorConfig->threadName,
			  peventMonitorConfig->threadPriority,
			  peventMonitorConfig->threadStackSize,
			  (EPICSTHREADFUNC) peventMonitorConfig->threadFunc,
			  (void*) peventMonitorConfig->threadParam);


	epicsPrintf("Epics Message Create thread Name:%s, ID :%p \n",peventMonitorConfig->threadName
								    ,peventMonitorConfig->threadQueueId);
	return peventMonitorConfig;
}
#if 0
static drvVXI1564a_controlThreadConfig*  make_controlStoreConfig(drvVXI1564a_taskConfig *ptaskConfig)
{
	drvVXI1564a_controlThreadConfig *pcontrolStoreConfig  = NULL;

	pcontrolStoreConfig  = (drvVXI1564a_controlThreadConfig*)malloc(sizeof(drvVXI1564a_controlThreadConfig));
	if(!pcontrolStoreConfig) return pcontrolStoreConfig;

	sprintf(pcontrolStoreConfig->threadName, "Store_ctrl");

	pcontrolStoreConfig->threadPriority = epicsThreadPriorityLow;
	pcontrolStoreConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pcontrolStoreConfig->threadFunc      = (EPICSTHREADFUNC) controlStoreFunc;
	pcontrolStoreConfig->threadParam     = (void*) ptaskConfig;

	ptaskConfig->pcontrolStoreConfig = pcontrolStoreConfig;

	pcontrolStoreConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));
	controlStoreQueueID = pcontrolStoreConfig->threadQueueId;
/*
	pcontrolStoreConfig->threadQueueId   = 0;
	if(pcontrolStoreConfig->threadQueueId) ptaskConfig->taskStatus |= TASK_CONTQUEUE;
*/
	epicsThreadCreate(pcontrolStoreConfig->threadName,
			  pcontrolStoreConfig->threadPriority,
			  pcontrolStoreConfig->threadStackSize,
			  (EPICSTHREADFUNC) pcontrolStoreConfig->threadFunc,
			  (void*) pcontrolStoreConfig->threadParam);


	epicsPrintf("Epics Message Create thread Name:%s, ID :%p \n",pcontrolStoreConfig->threadName
								    ,pcontrolStoreConfig->threadQueueId);
	return pcontrolStoreConfig;
}
#endif
#if 0
static int32 _VI_FUNCH taskCallback(ViSession taskHandle, ViEventType etype, ViEvent event, ViAddr *callbackData)
{
	unsigned  long long int start, stop;
	unsigned short statusID;
 
	drvVXI1564a_taskConfig *ptaskConfig = (drvVXI1564a_taskConfig*) callbackData;

	controlThreadQueueData queueData;
	drvVXI1564a_controlThreadConfig *pcontrolStoreConfig = (drvVXI1564a_controlThreadConfig*) ptaskConfig->pcontrolStoreConfig;

	 
 	/* Event type is a number which identifies the event type that has been received. */
   	viGetAttribute (event, VI_ATTR_SIGP_STATUS_ID, &statusID);
   	epicsPrintf("An event was received.  The Status/ID value is 0x%x \n",statusID);

   	epicsPrintf("\n Task Call Back Task Name : %s ************************* \n",ptaskConfig->taskName);

	send_dataChannel(ptaskConfig);	
	
	epicsMessageQueueSend(pcontrolStoreConfig->threadQueueId, (void*) &queueData, sizeof(controlThreadQueueData));
	/*  epicsPrintf("\n>>>Last snd ID: %d,  DMA cnt: %d\n", (int)pfpdpThreadConfig->threadQueueId, pbufferNode->nCnt); */

	
   	/* System calls are allowed to be used inside the event handler on all VISA supported platforms other than Macintosh. */
   	return VI_SUCCESS;
}
#endif
#if 0
static int32 CVICALLBACK taskCallback(TaskHandle taskHandle, int32 everyNsampleEventType, uInt32 nSamples, void *callbackData)
{
	unsigned  long long int start, stop;

	int32 error = 0;
	char errBuff[2048] = { '\0' };
	drvVXI1564a_taskConfig *ptaskConfig = (drvVXI1564a_taskConfig*) callbackData;

	printf("callBackFunction -> Read data -> Stop Task TG.Lee Request DataCount :%lu,****************** 1\n",
						 (uInt32)ellCount(ptaskConfig->pchannelConfig)*ptaskConfig->sampleCount);
	start = drvVXI1564a_rdtsc();
#if 0
	dataPoolConfig *pdataPoolConfig;
	pdataPoolConfig = (dataPoolConfig*) ellFirst(ptaskConfig->pdataPoolList);
	ellDelete(ptaskConfig->pdataPoolList, &pdataPoolConfig->node);
	ellAdd(ptaskConfig->pdataPoolList, &pdataPoolConfig->node);
	pdataPoolConfig->used_count++;
#endif

/* Add Need TG.Lee Read Data All
	DAQmxErrChk(DAQmxReadAnalogF64(ptaskConfig->taskHandle,
		 	  	       ptaskConfig->sampleCount,
			   	       -1,
			   	       DAQmx_Val_GroupByChannel,
			   	       pdataPoolConfig->pdata,
			   	       (uInt32)ellCount(ptaskConfig->pchannelConfig)*ptaskConfig->sampleCount,
			   	       &ptaskConfig->sampsPerChanRead,
			   	       NULL));
   Backup - Error.... TG.Lee
	epicsMutexLock(ptaskConfig->bufferLock);
*/
/*   by jh 2009.0924
	printf("callBackFunction -> Read data -> Stop Task TG.Lee ****************** 2\n");
	DAQmxErrChk(DAQmxReadAnalogF64(ptaskConfig->taskHandle,
		 	  	       ptaskConfig->sampleCount,
			   	       10,
			   	       DAQmx_Val_GroupByChannel,
			   	       ptaskConfig->vxi1564aBuffer,
			   	       (uInt32)ellCount(ptaskConfig->pchannelConfig)*ptaskConfig->sampleCount,
			   	       &ptaskConfig->sampsPerChanRead,
			   	       NULL));
*/

/*	DAQmxErrChk(DAQmxReadAnalogF64(ptaskConfig->taskHandle,
		 	  	       DAQmx_Val_Auto,
			   	       DAQmx_Val_WaitInfinitely,
			   	       DAQmx_Val_GroupByChannel,
			   	       pdataPoolConfig->pdata,
			   	       ellCount(ptaskConfig->pchannelConfig),
			   	       &pdataPoolConfig->num_read_data,
				       NULL));
*/
/*   VXI1564a_Old Function
	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle,
						nivxi1564a->readDataNumberVXI1564a,
						READ_TIME_OUT,
						DAQmx_Val_GroupByChannel,
						nivxi1564a->vxi1564aBuffer,
					    nivxi1564a->readDataNumberVXI1564a * MAX_CHAN_NUM,
						&read,
						NULL));
*/


/*	epicsMutexLock(ptaskConfig->#bufferLock);
		ptaskConfig->pdata = pdataPoolConfig->pdata;

	epicsMutexUnlock(ptaskConfig->bufferLock);
*/
	printf("callBackFunction -> Read data -> Stop Task TG.Lee sampsPerChanRead: %ld *************** 3\n",ptaskConfig->sampsPerChanRead);

#if 0
	send_dataChannel(ptaskConfig);
#endif
	epicsThreadSleep(1.);

	printf("callBackFunction -> Read data size :%d, Data 0 :%lf, Data 1 :%lf, Data 2:%lf , Data 3:%lf.... \n",sizeof(ptaskConfig->vxi1564aBuffer),
		 ptaskConfig->vxi1564aBuffer[0], ptaskConfig->vxi1564aBuffer[1],ptaskConfig->vxi1564aBuffer[2], ptaskConfig->vxi1564aBuffer[3]);


	scanIoRequest(ptaskConfig->ioScanPvt);
	epicsThreadSleep(1.);

	stop = drvVXI1564a_rdtsc();
	ptaskConfig->callbackTimeUsec = drvVXI1564a_intervalUSec(start, stop);
/* Pulse Operation So.... Stop Task..   */
	printf("callBackFunction -> Read data -> Stop Task TG.Lee **************3 \n");

 	stop_task(ptaskConfig);
	epicsPrintf("CallBackFunction Done and Task Stopped. \n");
/*
	epicsThreadSleep(0.1);

	epicsPrintf(" MDSplus Data Put Start. \n");
	send_dataTree(ptaskConfig);
	epicsPrintf(" MDSplus Data Put Complated! \n");
*/
	printf("callBackFunction -> Read data size :%d, Data 0 :%lf, Data 1 :%lf, Data 2:%lf , Data 3:%lf \n",sizeof(ptaskConfig->vxi1564aBuffer),
		 ptaskConfig->vxi1564aBuffer[0], ptaskConfig->vxi1564aBuffer[1],ptaskConfig->vxi1564aBuffer[2], ptaskConfig->vxi1564aBuffer[3]);

	return 0;
/*	return 0;  */

Error:
        if( DAQmxFailed(error) )
        	DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
                DAQmxStopTask(ptaskConfig->taskHandle);
                DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
                epicsPrintf("DAQmx Error: %s\n",errBuff);
        return 0;

}
#endif
static void controlThread(void *param)
{
	drvVXI1564a_taskConfig *ptaskConfig = (drvVXI1564a_taskConfig*) param;
	drvVXI1564a_controlThreadConfig *pcontrolThreadConfig; /* = (drvVXI1564a_controlThreadConfig*) ptaskConfig->pcontrolThreadConfig;  */
	controlThreadQueueData		queueData;

/*  Epics Sleep Function is defending Create Before complate the pcontrolThreadConfig        */
	while(!ptaskConfig->pcontrolThreadConfig) epicsThreadSleep(0.1);
	pcontrolThreadConfig = (drvVXI1564a_controlThreadConfig*) ptaskConfig->pcontrolThreadConfig;

	while(TRUE) {
		EXECFUNCQUEUE            pFunc;
		execParam                *pexecParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		drvVXI1564a_taskConfig    *ptaskConfig;
/*	epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId, (void*) &queueData,sizeof(controlThreadQueueData)); */
		epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId, (void*) &queueData ,sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;
		precord    = (struct dbCommon*) pexecParam->precord;
		prset      = (struct rset*) precord->rset;
		ptaskConfig = (drvVXI1564a_taskConfig *) pexecParam->ptaskConfig;

		if(!pFunc) continue;
		else pFunc(pexecParam);

		if(!precord) continue;

		dbScanLock(precord);
		(*prset->process)(precord);
		dbScanUnlock(precord);
	}
	return;
}

static void eventMonitorFunc(void *param)
{
	ViStatus err = 0;
	int semStatus, trigFuncOnce, dataFuncOnce, closeDisOnce;
	int dataPutComplate, dataPutDoneCounts;
	unsigned short evDoneMonitor;
	unsigned long long int start, stop;

#if 0
	ptaskConfig = (drvVXI1564a_taskConfig *) pexecParam->ptaskConfig;
	pFunc      = queueData.pFunc;
        pexecParam = &queueData.param;
	controlThreadQueueData		queueData;
	evDoneMonitor = ptaskConfig->interruptSource;
	printf("Interrupt Source register TG.Lee \n");
	printf("Interrupt Source Semaphore init In \n");
	printf("Interrupt Source Semaphore init Out : %d \n",semStatus);
#endif
	drvVXI1564a_taskConfig *ptaskConfig = (drvVXI1564a_taskConfig*) param;
	drvVXI1564a_controlThreadConfig *peventMonitorConfig; /* = (drvVXI1564a_controlThreadConfig*) ptaskConfig->peventMonitorConfig;   */


/*  Epics Sleep Function is defending Create Before complate the pcontrolStoreConfig        */
	while(!ptaskConfig->peventMonitorConfig) epicsThreadSleep(0.1);
	peventMonitorConfig = (drvVXI1564a_controlThreadConfig*) ptaskConfig->peventMonitorConfig;
	

/* rm TG 20110706	semStatus=sem_init( &ptaskConfig->DAQsemaphore, 0, 0);  */
  ptaskConfig->DAQsemaphore = epicsEventCreate(epicsEventEmpty);
	while(TRUE) {
/* rm TG 20110706		semStatus = sem_wait(&ptaskConfig->DAQsemaphore ); */
    semStatus = epicsEventWait (ptaskConfig->DAQsemaphore);
		printf("Interrupt Source Semaphore wait : %d \n",semStatus);
		printf("Interrupt Source register Semaphore Done \n");
		trigFuncOnce = 1;
		dataFuncOnce = 1;
		evDoneMonitor = 0;
		closeDisOnce = 0;
		/* Seperited control thread and eventMoniter thread. so, you must mdsconnect(open,close,disconnect) do in the mds put function before much mdsconnect and open traffic */
		start = drvVXI1564a_rdtsc();
		while((int)evDoneMonitor!=16448 && (ptaskConfig->abortStop == 0)){
			err=viIn16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_INTR_SOURCE,&ptaskConfig->interruptSource); 
			if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
			evDoneMonitor = ptaskConfig->interruptSource & 0x4040; /* bit 14,6 Done -Memory is full of post-trigger acq is complete */
			if((trigFuncOnce != 0) && ((int)(ptaskConfig->interruptSource & 0x8080) !=32896)){
				epicsMutexLock(ptaskConfig->bufferLock);	
				ptaskConfig->taskStatus |= TASK_WAITTRIG;
				epicsMutexUnlock(ptaskConfig->bufferLock);
				scanIoRequest(ptaskConfig->ioScanPvt);
				trigFuncOnce = 0;
				epicsPrintf(">>> Check the interrupt Source Trig:Intr source:taskName:%s, 0x%4X,evDoneMon:0x%4X \n",
																		ptaskConfig->taskName,ptaskConfig->interruptSource, evDoneMonitor);
			}
			if((dataFuncOnce !=0) && ((int)(ptaskConfig->interruptSource & 0x8080) == 32896)){
				epicsMutexLock(ptaskConfig->bufferLock);	
				ptaskConfig->taskStatus &= ~TASK_WAITTRIG;
				ptaskConfig->taskStatus |= TASK_DATAGATHER;
				epicsMutexUnlock(ptaskConfig->bufferLock);
				scanIoRequest(ptaskConfig->ioScanPvt);
				dataFuncOnce = 0;
				epicsPrintf(">>> Check the interrupt Source Data:Intr source:taskName:%s, 0x%4X,evDoneMon:0x%4X \n", 
																		ptaskConfig->taskName,ptaskConfig->interruptSource, evDoneMonitor);
			}
			epicsThreadSleep(0.2);

		}
		stop = drvVXI1564a_rdtsc();
		epicsPrintf("Interrupt Done Monitor : Current Time %lf msec \n", 1.E-3 * (double)drvVXI1564a_intervalUSec(start, stop));
#if 0
		epicsPrintf("Interrupt Source register 0E= 0x%4X\n", ptaskConfig->interruptSource);
		if((ptaskConfig->eventDone ==1) && (evDoneMonitor == 16448))
			ptaskConfig->eventDone = 0; 
#endif
		if(ptaskConfig->abortStop == 0){
			epicsPrintf(" Channelization from Device Memory %s . \n", ptaskConfig->taskName);
			send_dataChannelAndStore(ptaskConfig);
			epicsThreadSleep(1.0);
			epicsPrintf(" MDSplus Data Put Complated! Task Name %s. \n", ptaskConfig->taskName);
		}else epicsPrintf("\n >>>  Interrupt Abort Stop Function ..OK >>>??  \n");
#if 0
		/* TG.Lee Test Multi mdsdisconnection and close tree. Do not action (bad sequence at this time)  */
		mdsDisconnectAndClose(ptaskConfig);
		/* Protection Continue mesage Queue.When data store complated We Mem refrish and then eventDone Value is Zero return.  */
#endif		
	}

	printf("Event Monitoring Function  >>> Terminated!\n");

	return;
}
#if 0
static void controlStoreFunc(void *param)
{

	drvVXI1564a_taskConfig *ptaskConfig = (drvVXI1564a_taskConfig*) param;
	drvVXI1564a_controlThreadConfig *pcontrolStoreConfig = (drvVXI1564a_controlThreadConfig*) ptaskConfig->pcontrolStoreConfig;


/*  Epics Sleep Function is defending Create Before complate the pcontrolStoreConfig        */
	while(!ptaskConfig->pcontrolStoreConfig) epicsThreadSleep(0.1);
	pcontrolStoreConfig = (drvVXI1564a_controlThreadConfig*) ptaskConfig->pcontrolStoreConfig;
	while(TRUE) {

#if 0
		EXECFUNCQUEUE            pFunc;
		execParam                *pexecParam;
/*		struct dbCommon          *precord;
		struct rset              *prset;
*/
		drvVXI1564a_taskConfig    *ptaskConfig;

		epicsMessageQueueReceive(pcontrolStoreConfig->threadQueueId, (void*) &queueData ,sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;
		ptaskConfig = (drvVXI1564a_taskConfig *) pexecParam->ptaskConfig;

		epicsPrintf(" MDSplus Data Put Start. Task Name %s . \n", ptaskConfig->taskName);
		send_dataTree(ptaskConfig);
		epicsPrintf(" MDSplus Data Put Complated! \n");

/*
		dbScanLock(precord);
		(*prset->process)(precord);
		dbScanUnlock(precord);
*/
#endif		
	}

	epicsPrintf("controlStore Function  >>> Terminated!\n");

	return;
}
#endif


/* static void clearAllVXITasks(void) */
void clearAllVXITasks(void)
{
	int32 err = 0;
/*
	char errBuf[2048] = { '\0'};
*/
	drvVXI1564a_taskConfig *ptaskConfig;


	ptaskConfig = (drvVXI1564a_taskConfig *) ellFirst (pdrvVXI1564aConfig->ptaskList);

	while(ptaskConfig) {
		if(ptaskConfig->taskStatus & (TASK_INITIALIZED | TASK_STARTED | TASK_STOPED)) {
			/* reset function clear Module before exit */ 
			reset(ptaskConfig);
			err=viClose(ptaskConfig->taskHandle);  /* Closes the specified session, event, or find list */
			epicsThreadSleep(1.0);
			epicsMutexLock(ptaskConfig->bufferLock);	
			ptaskConfig->taskStatus &= ~TASK_INITIALIZED;
			ptaskConfig->taskStatus |= TASK_DESTROIED;
			epicsMutexUnlock(ptaskConfig->bufferLock);

			epicsPrintf("Exit Hook: Clear Task %s\n", ptaskConfig->taskName);
/* rm TG 20110706			sem_destroy(&ptaskConfig->DAQsemaphore);  */
    epicsEventDestroy (ptaskConfig->DAQsemaphore);
		}
		ptaskConfig = (drvVXI1564a_taskConfig*) ellNext(&ptaskConfig->node);
	}
/* rm TG 20110706	sem_destroy(&DATAsemaphore);  */
  epicsEventDestroy (DATAsemaphore);
  
	return;
}

static void exitHandler(int signo)
{
	clearAllVXITasks();
	exit(0);
}

static void createVXITaskCallFunction(const iocshArgBuf *args)
{
	drvVXI1564a_taskConfig *ptaskConfig = NULL;
	char task_name[60];
	char server_name[60];
	char tree_name[60];
	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(server_name, args[1].sval);
	if(args[2].sval) strcpy(tree_name, args[2].sval);

	if(make_drvConfig()) return;

	ptaskConfig = make_taskConfig(task_name, server_name, tree_name);
	if(!ptaskConfig) return;

	ptaskConfig->pcontrolThreadConfig = make_controlThreadConfig(ptaskConfig);
	if(!ptaskConfig->pcontrolThreadConfig) return;
	else ptaskConfig->taskStatus |= TASK_CONTHREAD;

       	ptaskConfig->peventMonitorConfig = make_eventMonitorConfig(ptaskConfig);
       	if(!ptaskConfig->peventMonitorConfig) return;

#if 0
	if(!strcmp(ptaskConfig->taskName, "VXI0::40::INSTR")) 
    	{
        	ptaskConfig->pcontrolStoreConfig = make_controlStoreConfig(ptaskConfig);
        	if(!ptaskConfig->pcontrolStoreConfig) return;
    	}
#endif

	ellAdd(pdrvVXI1564aConfig->ptaskList, &ptaskConfig->node);
	return;
}

static void createVXI1564aChannelCallFunction(const iocshArgBuf *args)
{
	drvVXI1564a_taskConfig		*ptaskConfig = NULL;
	drvVXI1564a_aoChannelConfig	*paoChannelConfig = NULL;
	char task_name[60];
	char channel_name[60];
	char channel_num[60];
	char channel_node[60];

	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(channel_name, args[1].sval);
	if(args[2].sval) strcpy(channel_num, args[2].sval);
	if(args[3].sval) strcpy(channel_node, args[3].sval);

	ptaskConfig = find_taskConfig(task_name);
	if(!ptaskConfig) return;

	paoChannelConfig = (drvVXI1564a_aoChannelConfig*) malloc(sizeof(drvVXI1564a_aoChannelConfig));
	if(!paoChannelConfig) return;

	paoChannelConfig->channelType = VXI1564a_CHANNEL;
	paoChannelConfig->channelStatus = 0x0000;
	strcpy(paoChannelConfig->chanName, channel_name);
	paoChannelConfig->chanNum =(unsigned)atoi(channel_num);
	paoChannelConfig->chanIndex = ellCount(ptaskConfig->pchannelConfig);
	strcpy(paoChannelConfig->treeNode, channel_node);
/*   Delete - Not used 
	pwfChannelConfig->gain = 1;
	pwfChannelConfig->cutOffFreq = 1;

	pwfChannelConfig->minRange = INIT_LOW_LIMIT;
	pwfChannelConfig->maxRange = INIT_HIGH_LIMIT;
*/
	paoChannelConfig->ptaskConfig = ptaskConfig;

	ellAdd(ptaskConfig->pchannelConfig, &paoChannelConfig->node);

	return;
}


LOCAL long drvVXI1564a_io_report(int level)
{
	drvVXI1564a_taskConfig *ptaskConfig;
/*
	int i;
	dataPoolConfig *pdataPoolConfig;   

*/

	if(!pdrvVXI1564aConfig) return 0;

	if(ellCount(pdrvVXI1564aConfig->ptaskList))
		ptaskConfig = (drvVXI1564a_taskConfig*) ellFirst (pdrvVXI1564aConfig->ptaskList);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d VXI task(s) found\n",ellCount(pdrvVXI1564aConfig->ptaskList));

	if(level<1) return 0;

	while(ptaskConfig) {
		epicsPrintf("  Task name: %s, handle: 0x%x, status: 0x%x, connected channels: %d\n",
			    ptaskConfig->taskName,
			    (unsigned )ptaskConfig->taskHandle,
			    ptaskConfig->taskStatus,
			    ellCount(ptaskConfig->pchannelConfig));
		if(level>2) {
/*
			epicsPrintf("   Sampling Rate: %f/sec, Buffer-Pool: %d\n",
					 ptaskConfig->samplingRate, ellCount(ptaskConfig->pdataPoolList));
*/
			epicsPrintf("   SamplePeriod Min : %.8f/sec,SamplePeriod :%.8f/sec, Sample Max Counts: %f, Pre-Sample Conuts:%f \n",
							 ptaskConfig->samplePeriodMin,ptaskConfig->samplePeriod, ptaskConfig->sampleMaxCount, ptaskConfig->samplePreCount );
			epicsPrintf("   Sampling Rate: %.8f/sec, Sample Counts: %f, Pre-Sample Conuts:%f \n",
							 ptaskConfig->samplingRate, ptaskConfig->sampleCount, ptaskConfig->samplePreCount );
			epicsPrintf("  ShotNumber : %d, Blip Time: %f, DAQ start Time :%f \n",
						 	ptaskConfig->shotNumber, ptaskConfig->blipTime,ptaskConfig->daqTime );
			epicsPrintf("   ");
			epicsPrintf("\n");
			epicsPrintf("  Task MDSplus Server : %s, Task MDSplus Tree Name: %s \n",
								 ptaskConfig->mdsServer, ptaskConfig->mdsTree );
		}
		if(level>3) {
			epicsPrintf("   ");
			epicsPrintf("\n");
			epicsPrintf("   Status of Register *********************************************************  \n");
			epicsPrintf("   ");
			epicsPrintf("\n");
			epicsPrintf("   ID  Reg ---------------------------: 0x%4X \n", ptaskConfig->idReg);
			epicsPrintf("   Device Type Reg -------------------: 0x%4X \n", ptaskConfig->devTypeReg);
			epicsPrintf("   Status Control --------------------: 0x%4X \n", ptaskConfig->statusReg);
			epicsPrintf("   Offset Reg -------------: 0x%4X \n", ptaskConfig->offsetReg);
			epicsPrintf("   FIFO High Reg ----------: 0x%4X \n", ptaskConfig->fifoHigh);
			epicsPrintf("   FIFO Low Reg -----------: 0x%4X \n", ptaskConfig->fifoLow);
			epicsPrintf("   Interrupt Control Reg --: 0x%4X \n", ptaskConfig->interruptControl);
			epicsPrintf("   Interrupt Source Reg ---: 0x%4X \n", ptaskConfig->interruptSource);
			epicsPrintf("   CVTable Channel 1 Reg --: 0x%4X \n", ptaskConfig->cvtableCh1);
			epicsPrintf("   CVTable Channel 2 Reg --: 0x%4X \n", ptaskConfig->cvtableCh2);
			epicsPrintf("   CVTable Channel 3 Reg --: 0x%4X \n", ptaskConfig->cvtableCh3);
			epicsPrintf("   CVTable Channel 4 Reg --: 0x%4X \n", ptaskConfig->cvtableCh4);
			epicsPrintf("   Samples Taken High Reg-------------: 0x%4X \n", ptaskConfig->sampleTakenHigh);
			epicsPrintf("   Samples Taken Low Reg -------------: 0x%4X \n", ptaskConfig->sampleTakenLow);
			epicsPrintf("   Cal Flash ROM Addr Reg--: 0x%4X \n", ptaskConfig->calFlashRomAdr);
			epicsPrintf("   Cal Flash ROM Data Reg--: 0x%4X \n", ptaskConfig->calFlashRomData);
			epicsPrintf("   Cal Source  Reg --------: 0x%4X \n", ptaskConfig->calSource);
			epicsPrintf("   Cache Sample Count Reg--: 0x%4X \n", ptaskConfig->cacheCount);
			epicsPrintf("   Range, Filter Ch1&2 Reg------------: 0x%4X \n", ptaskConfig->voltFilter12);
			epicsPrintf("   Range, Filter Ch3&4 Reg------------: 0x%4X \n", ptaskConfig->voltFilter34);
			epicsPrintf("   Trig/Int-Rupt Lev Ch1 Reg: 0x%4X \n", ptaskConfig->triggerLevel);
			epicsPrintf("   Trig/Int-Rupt Lev Ch2 Reg: 0x%4X \n", ptaskConfig->triggerLeve2);
			epicsPrintf("   Trig/Int-Rupt Lev Ch3 Reg: 0x%4X \n", ptaskConfig->triggerLeve3);
			epicsPrintf("   Trig/Int-Rupt Lev Ch4 Reg: 0x%4X \n", ptaskConfig->triggerLeve4);
			epicsPrintf("   Sample Period High Reg-------------: 0x%4X \n", ptaskConfig->sampleRateHigh);
			epicsPrintf("   Sample Period Low Reg--------------: 0x%4X \n", ptaskConfig->sampleRateLow);
			epicsPrintf("   Pre-trigger Count High Reg---------: 0x%4X \n", ptaskConfig->preTriggerHigh);
			epicsPrintf("   Pre-trigger Count Low Reg----------: 0x%4X \n", ptaskConfig->preTriggerLow);
			epicsPrintf("   Sample (Post-trig) Count High Reg--: 0x%4X \n", ptaskConfig->sampleCountHigh);
			epicsPrintf("   Sample (Post-trig) Count Low Reg---: 0x%4X \n", ptaskConfig->sampleCountLow);
			epicsPrintf("   Trigger Control/Source Reg --------: 0x%4X \n", ptaskConfig->triggerSource);
			epicsPrintf("   Sample Control/Source Reg ---------: 0x%4X \n", ptaskConfig->sampleSource);
		}
		if(ellCount(ptaskConfig->pchannelConfig)>0) print_channelConfig(ptaskConfig,level);
		ptaskConfig = (drvVXI1564a_taskConfig*) ellNext(&ptaskConfig->node);

	}

	return 0;
}


LOCAL long drvVXI1564a_init_driver(void)
{
	drvVXI1564a_taskConfig *ptaskConfig = NULL;
	drvVXI1564a_channelConfig *pchannelConfig = NULL;
/*	drvVXI1564a_aiChannelConfig *paiChannelConfig = NULL;  */
	drvVXI1564a_aoChannelConfig *paoChannelConfig = NULL;

	unsigned long long int a;
	unsigned idx, dix;
	epicsThreadSleep(1.);
	if(!pdrvVXI1564aConfig)  return 0;

	ptaskConfig = (drvVXI1564a_taskConfig*) ellFirst(pdrvVXI1564aConfig->ptaskList);
	while(ptaskConfig) {
		if(ellCount(ptaskConfig->pchannelConfig)) {
			init_task(ptaskConfig);
			idx = 0;
			dix = 0;
#ifdef DEBUG
			printf("init_task done..TG.Lee channel Config Counts:%d\n",ellCount(ptaskConfig->pchannelConfig));
#endif
			epicsThreadSleep(1.);
/*	Not Need first int do not action ... */
			pchannelConfig = (drvVXI1564a_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
			while(pchannelConfig) {
				switch(pchannelConfig->channelType) {
					case VXI1564a_CHANNEL:
						paoChannelConfig = (drvVXI1564a_aoChannelConfig*) pchannelConfig;
						init_aoChannel(paoChannelConfig);
						idx++;
						break;
				}
				pchannelConfig = (drvVXI1564a_channelConfig*) ellNext(&pchannelConfig->node);
			}
/* Auto Run - DAQ start function - Remove for Diagnostic DAQ. 
			if(idx) {
				set_taskExTrigger(ptaskConfig);
				set_taskCallback(ptaskConfig);
				init_dataPool(ptaskConfig);
				start_task(ptaskConfig);
			}
*/
		}
		ptaskConfig = (drvVXI1564a_taskConfig*) ellNext(&ptaskConfig->node);
	}

	epicsAtExit((EPICSEXITFUNC) clearAllVXITasks, NULL);
        signal(SIGTERM, exitHandler);
        signal(SIGINT, exitHandler);

	a = drvVXI1564a_rdtsc();
	epicsThreadSleep(1.);
	pdrvVXI1564aConfig->one_sec_interval = drvVXI1564a_rdtsc() - a;

	epicsPrintf("drvVXI1564a: measured clock speed %6.4lf GHz\n", 1.E-9 * (double)pdrvVXI1564aConfig->one_sec_interval);

	return 0;
}




drvVXI1564a_taskConfig* drvVXI1564a_find_taskConfig(char *taskName)
{
	return find_taskConfig(taskName);
}

drvVXI1564a_channelConfig* drvVXI1564a_find_channelConfig(char *taskName, char *chanName)
{
	drvVXI1564a_taskConfig *ptaskConfig = find_taskConfig(taskName);
	if(!ptaskConfig) return NULL;

	return find_channelConfig(ptaskConfig, chanName);
}


void drvVXI1564a_set_taskExTrigger(drvVXI1564a_taskConfig *ptaskConfig, unsigned int exTrig)
{
	set_taskExTrigger(ptaskConfig,exTrig);
	return;
}
void drvVXI1564a_set_taskCallback(drvVXI1564a_taskConfig *ptaskConfig)
{
	/* Not used -- sem used. */
	set_taskCallback(ptaskConfig);
	return;
}
void drvVXI1564a_set_startTask(drvVXI1564a_taskConfig *ptaskConfig)
{
	start_task(ptaskConfig);
	return;
}
void drvVXI1564a_set_stopTask(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setStop)
{
	stop_task(ptaskConfig, setStop);
	return;
}
void drvVXI1564a_set_abortTask(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setAbort)
{
	abort_task(ptaskConfig, setAbort);
	return;
}
void drvVXI1564a_set_regUpdateTask(drvVXI1564a_taskConfig *ptaskConfig)
{
	regUpdate_task(ptaskConfig);
	return;
}
void drvVXI1564a_set_sampleCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 sampleCount)
{
	set_sampleCount(ptaskConfig, sampleCount);
/*	set_taskCallback(ptaskConfig);  SNL Function Add */
	return;
}
void drvVXI1564a_set_samplePreCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplePreCount)
{
	set_samplePreCount(ptaskConfig, samplePreCount);
	return;
}

void drvVXI1564a_set_triggerType(drvVXI1564a_taskConfig *ptaskConfig,  int triggerType)
{
	set_triggerType(ptaskConfig, triggerType);
	return;
}
void drvVXI1564a_set_operMode(drvVXI1564a_taskConfig *ptaskConfig, unsigned int operMode)
{
	set_operMode(ptaskConfig, operMode);
	return;
}

void drvVXI1564a_set_sampleMode(drvVXI1564a_taskConfig *ptaskConfig,  int sampleMode)
{
	set_sampleMode(ptaskConfig, sampleMode);
	return;
}
void drvVXI1564a_set_channelRange(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *paoChannelConfig, uInt16 setRange)
{
	set_channelRange(ptaskConfig, paoChannelConfig, setRange);
	return;
}
void drvVXI1564a_set_channelFilter(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *paoChannelConfig, uInt16 setFilter)
{
	set_channelFilter(ptaskConfig, paoChannelConfig, setFilter);
	return;
}
void drvVXI1564a_set_samplePeriod(drvVXI1564a_taskConfig *ptaskConfig, float64 samplePeriod)
{
	set_samplePeriod(ptaskConfig, (float64) samplePeriod);
	return;
}
#if 0
void drvVXI1564a_set_samplingRate(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplingRate)
{
	set_samplingRate(ptaskConfig, (uInt32) samplingRate);
	return;
}
#endif
void drvVXI1564a_set_shotNumber(drvVXI1564a_taskConfig *ptaskConfig, int shotNumber)
{
	set_shotNumber(ptaskConfig, (int) shotNumber);
	return;
}
void drvVXI1564a_set_blipTime(drvVXI1564a_taskConfig *ptaskConfig, double blipTime)
{
	set_blipTime(ptaskConfig, (float64) blipTime);
	return;
}
void drvVXI1564a_set_daqTime(drvVXI1564a_taskConfig *ptaskConfig, double daqTime)
{
	set_daqTime(ptaskConfig, (float64) daqTime);
	return;
}

int drvVXI1564a_mds_connectOpen(drvVXI1564a_taskConfig *ptaskConfig)
{
	mdsConnectAndOpen(ptaskConfig);
	return 1;
}
int drvVXI1564a_mds_disconnectClose(drvVXI1564a_taskConfig *ptaskConfig)
{
	mdsDisconnectAndClose(ptaskConfig);
	return 1;
}
#if 0
int drvVXI1564a_send_dataTree(drvVXI1564a_taskConfig *ptaskConfig)
{
	send_dataTree(ptaskConfig);
	return 1;
}
#endif
int drvVXI1564a_create_newTree(drvVXI1564a_taskConfig *ptaskConfig)
{
	create_newTree(ptaskConfig);
	return 1;
}
#if 0
void drvVXI1564a_set_treeServer(drvVXI1564a_taskConfig *ptaskConfig, char *treeServer)
{
	set_treeServer(ptaskConfig, (char) &treeServer);
	return;
}

void drvVXI1564a_set_treeName(drvVXI1564a_taskConfig *ptaskConfig, char *treeName)
{
	set_treeName(ptaskConfig, (char) &treeName);
	return;
}
void drvVXI1564a_set_treeNode(drvVXI1564a_wfChannelConfig *pchannelConfig, char *treeNode)
{
	set_treeNode(pchannelConfig, (char*) treeNode );
	return;
}
#endif
/*
 * Below code for only vxi HP E 1564a
 */

/************************************************************/
static void reset(drvVXI1564a_taskConfig *ptaskConfig)
{
   /* reset the digitizer (write a 1 to status bit 0) delay 1 second for reset 	   		 */
   /* then set bit back to 0 to allow module to operate                        			 */
   /* write a "1" to the reset bit then set the bit back to "0"               			 */
   /* Writing a "1" to this bit resets the digitizer to the power-on state.    			 */
   /* You must set bit 0 back to a logical "0" before  resuming normal operations of the module. */
   ViStatus err = 0;
   
   err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_STATUS_CONTROL,1);  /* 0x04 -  set reset bit to "1" */
   if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
   err=viOut16(ptaskConfig->taskHandle,VI_A16_SPACE,VXI_STATUS_CONTROL,0);  /* 0x04 -  set reset bit to "0" */
   if (err<VI_SUCCESS) err_handler(ptaskConfig,err);
/*    epicsPrintf("Init Task Function taskHandle = %s \n",ptaskConfig->taskName);   */
   return; 
}


/*** Error handling function ***/
static void err_handler (drvVXI1564a_taskConfig *ptaskConfig, ViStatus err)
{
   char buf[1024] = {0};
   viStatusDesc (ptaskConfig->taskHandle, err, buf);   /* retrieve error description */
   epicsPrintf ("ERROR = %s\n", buf);
   return;
}
