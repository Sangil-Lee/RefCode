#ifndef drvVXI1564a_H
#define drvVXI1564a_H

#include "visa.h"
#include "visatype.h"

#include "epicsTypes.h"
#include "epicsRingBytes.h"
#include "epicsMessageQueue.h"
#include "epicsThread.h"

#include "ellLib.h"
#include "callback.h"
#include "dbScan.h"

#include <semaphore.h>

#include <math.h>

/* Useful definition for handling returned status values */
#define statusOk(status) (((status) & 1) == 1)

#define OPMODE_TEST 			0
#define OPMODE_REMOTE           	1

#define VXI1564a_CHANNEL		(0x0001<<0)

#define TASK_NOT_INIT			0x0000
#define TASK_INITIALIZED		(0x0001<<0)
#define TASK_STARTED			(0x0001<<1)
#define TASK_STOPED			(0x0001<<2)
#define TASK_WAITTRIG			(0x0001<<3)
#define TASK_DATAGATHER			(0x0001<<4)
#define TASK_DATADIVIDE			(0x0001<<5)
#define TASK_DATASTORE			(0x0001<<6)
#define TASK_READYSHOT			(0x0001<<7)
#define TASK_CONTHREAD          	(0x0001<<8)
#define TASK_CONTQUEUE          	(0x0001<<9)
#define TASK_EVENTQUEUE          	(0x0001<<10)
#define TASK_DESTROIED          	(0x0001<<11)

#define CHANNEL_INITIALIZED		(0x0001<<0)
#define INIT_DATAPOOLSIZE             4

/* Interval for Check the Configuration in run as second */
/* #define INIT_CHECK_INTERVAL      1        */
/* PCI6122 -16ch, PXI6133 - 8chMax Channel number of DAQ unit    */
/* #define INIT_MAX_CHAN_NUM        16        */
/* PCI6122 - 250000,PXI6133 - 1000000, number of sampling data per channel  */

/* Memory Size 4MB=524,287, 8MB=1,048,575, 16MB=2,097,151, 32MB=4,194,303, 64MB=8,388,607, 128MB=16,777,215   */
/* 4MB=7,FFFF, 8MB=F,FFFF, 16MB=1F,FFFF, 32MB=3F,FFFF, 64MB=7F,FFFF, 128MB=FF,FFFF */

#define INIT_VXI1564a_MAX_DATA_SIZE   	16777215   
#define INIT_SAMPLE_RATE_VXI1564a	800000 
#define INIT_SAMPLE_INTERVAL_MIN	0.0000013 
#define INIT_SAMPLE_INTERVAL_MAX	0.8 

#define INIT_TRIGGER_SOURCE_REG		0x180   /* Inter Trig-9bit 0, Trig Slop Pos-8bit, Soft Trig Imdi-7bit or 0x300=Extrig,Pos*/
#define INIT_SAMPLE_SOURCE_REG		0x21A0  /* SW-ARM 30ms delay, Pos, Soft-Sample, INT-Clock or 0x120=INT-Clock,Pos(External Samp) */
#define INIT_OPERATION			0       /* Init operation mode , 0 - local, 1 -Remote   */
/*
#define CHAN_NUM        1
*/
#define ERR_MSG_SIZE        2048        /* length for error message */
#define INIT_LOW_LIMIT       -10.0
#define INIT_HIGH_LIMIT      10.0
#define READ_TIME_OUT       10.0

#define ACCESS_NAME_SIZE    20
#define TASK_NAME_SIZE      60

/*   Special Registers for 1564a module configuring  */
/*  Write and Read Riegisters    */
#define VXI_STATUS_CONTROL   		0x04  /* Status Control Register  */
#define VXI_OFFSET			0x06  /* Offset Register   */
#define VXI_INTR_CONTROL		0x0C  /* Interrupt Control Register */
#define VXI_INTR_SOURCE			0x0E  /* Interrupt Source Register */
#define VXI_CAL_ROM_ADDR		0x1C  /* Calibration Flash ROM Address Register */
#define VXI_CAL_ROM_DATA		0x1E  /* Calibration Flash ROM Data Register */
#define VXI_CAL_SOURCE			0x20  /* Calibration Source Register */
#define VXI_CACHE_COUNT			0x22  /* Cache Count Register */
#define VXI_RANG_FILTER_12		0x24  /* Range, Filter, Connect Channel 1 and 2 Register */
#define VXI_RANG_FILTER_34		0x26  /* Range, Filter, Connect Channel 3 and 4 Register */
#define VXI_TRIG_INTR_LEVEL1		0x28  /* Trigger/Interrupt Level Channel 1 Register */
#define VXI_TRIG_INTR_LEVEL2		0x2A  /* Trigger/Interrupt Level Channel 2 Register */
#define VXI_TRIG_INTR_LEVEL3		0x2C  /* Trigger/Interrupt Level Channel 3 Register */
#define VXI_TRIG_INTR_LEVEL4		0x2E  /* Trigger/Interrupt Level Channel 4 Register */
#define VXI_SAMPLE_PERIOD_HIGH		0x30  /*  Period - Sampling Rate High */
#define VXI_SAMPLE_PERIOD_LOW		0x32  /*  Period - Sampling Rate Low */
#define VXI_PRE_TRIG_HIGH		0x34  /*  Used Pre-Trigger counts */
#define VXI_PRE_TRIG_LOW		0x36  /*  Used Pre-Trigger counts */
#define VXI_SAMPLE_COUNT_HIGH		0x38  /*  Sample count - total number of reading and including Pre-trigger reading */
#define VXI_SAMPLE_COUNT_LOW		0x3A  /*  Sample count - Min value is 1. Zero(0) causes continuous reading and will not stop acq */
#define VXI_TRIG_CONTROL		0x3C  /*  Used Trigger Control-Source */
#define VXI_SAMPLE_CONTROL		0x3E  /*  Used Sample Control-Source  */
/*  Read Registers    */
#define VXI_MANU_ID			0x00  /* Manufacturer ID Register  */
#define VXI_DEVICE_TYPE			0x02  /* Device Type Register  */
#define VXI_FIFO_HIGH			0x08  /* FIFO High Word Register  */
#define VXI_FIFO_LOW			0x0A  /* FIFO Low Word Register  */
#define VXI_CVTABLE_CH1			0x10  /* CVTable Channel 1 Register  */
#define VXI_CVTABLE_CH2			0x12  /* CVTable Channel 2 Register  */
#define VXI_CVTABLE_CH3			0x14  /* CVTable Channel 3 Register  */
#define VXI_CVTABLE_CH4			0x16  /* CVTable Channel 4 Register  */
#define VXI_SAMPLE_TAKEN_HIGH		0x18  /* Samples Taken High Word Register  */
#define VXI_SAMPLE_TAKEN_LOW		0x1A  /* Samples Taken High Word Register  */


/* epics Type define  */
#ifndef INT_TYPES
#define INT_TYPES
typedef signed char        int8;
typedef signed short       int16;
typedef signed long        int32;
#endif

typedef unsigned char      uInt8;
typedef unsigned short     uInt16;
typedef unsigned long      uInt32;
typedef float              float32;
typedef double             float64;
typedef long long int      int64;
typedef unsigned long long uInt64;

typedef uInt32             bool32;
typedef uInt32             TaskHandle;

sem_t 	DATAsemaphore;

typedef struct {
	void             *ptaskConfig;
	void             *pchannelConfig;   /* Add 06.23 */
	void             *precord;
	double           setValue;
	char 		 setStr[256];
} execParam;

typedef void (*EXECFUNCQUEUE) (execParam *pParam);

typedef struct {
	EXECFUNCQUEUE     pFunc;
	execParam         param;
} controlThreadQueueData;


typedef struct {
	char                          threadName[60];
	epicsThreadId                 threadId;
	unsigned int                  threadPriority;
	unsigned int                  threadStackSize;
	EPICSTHREADFUNC               threadFunc;
	void                          *threadParam;

	epicsMessageQueueId           threadQueueId;

} drvVXI1564a_controlThreadConfig;


typedef struct {
        ELLLIST         *ptaskList;
        ELLLIST		*pdestroiedList;

	unsigned long long int one_sec_interval;
} drvVXI1564aConfig;


typedef struct {
	ELLNODE         node;
	char            taskName[60];
	ViSession       taskRM;
	ViSession       taskHandle;
 	epicsMutexId    taskLock;
	epicsMutexId    bufferLock;
 	IOSCANPVT       ioScanPvt;

	sem_t 	DAQsemaphore;
	unsigned short	taskStatus;

	unsigned short  idReg;          /* 0x00 - Reading the ID register for Device - init state */
	unsigned short  devTypeReg;     /* 0x02 - Reading the Device Type Register -E1564A 4ch : 7267 init state  */
	unsigned short  statusReg;      /* 0x04 - Status and Control Register -Mem size:8,9,10bit,1Moto big/little 0Intel endian  */

	unsigned short  offsetReg;      /* 0x06 - Offset of the Module in A24 space  */
	unsigned short  fifoHigh;       /* 0x08 - FIFO High WORD  */
	unsigned short  fifoLow;        /* 0x0A - FIFO LOW WORD */
	unsigned short  interruptControl; /* 0x0C - Interrupt Level and the interrupt source controll. - bit14 -Mom full or acq done */
	unsigned short  interruptSource; /* 0x0E - Eight events can be enabled to interrupt the digitizer. */

	unsigned short  cvtableCh1;     /* 0x10 - Holds the last value of the 2's complements data stored in FIFO for Ch 1. */
	unsigned short  cvtableCh2;     /* 0x12 - Holds the last value of the 2's complements data stored in FIFO for Ch 2. */
	unsigned short  cvtableCh3;     /* 0x14 - Holds the last value of the 2's complements data stored in FIFO for Ch 3. */
	unsigned short  cvtableCh4;     /* 0x16 - Holds the last value of the 2's complements data stored in FIFO for Ch 4. */

	unsigned short  sampleTakenHigh;/* 0x18 - The number of samples taken (number of readings) */
	unsigned short  sampleTakenLow; /* 0x1A - The number of samples taken (number of readings) */
	unsigned short  calFlashRomAdr; /* 0x1C - The register holds the address of the calibration flash ROM */
	unsigned short  calFlashRomData; /* 0x1E - The register holds the data of the calibration flash ROM */
	unsigned short  calSource;      /* 0x20 - The E1564A 4-Channel Digitizer has an on-board calibration source */
	unsigned short  cacheCount;     /* 0x22 - The total number of samples taken by the digitizer is (cache count x 4ch) + sample count(registers at offset 0x18 and 0x1A) */

	unsigned short	voltFilter12;   /* base +24 = 0.0625V, 0.25V, 1V, 4V, 16V, 64V, 256V select per 2 channel */
	unsigned short	voltFilter34;   /* base +26 = Off, 1.5kHz, 6kHz, 25kHz, 100kHz select per 2 channel       */
	unsigned short	triggerLevel;   /* base +28 - I don't know yet. Not used!  */
	unsigned short	triggerLeve2;   /* base +2A - I don't know yet. Not used!  */
	unsigned short	triggerLeve3;   /* base +2C - I don't know yet. Not used!  */
	unsigned short	triggerLeve4;   /* base +2E - I don't know yet. Not used!  */

	unsigned short  sampleRateHigh; /* Sampling Rate - Period  High */
	unsigned short  sampleRateLow;  /* Sampling Rate - Period  Low  */
	unsigned short  preTriggerHigh; /* Pre-Trigger Counts High - Max size of memory /4 (E1564) - 6/16 bits used */
	unsigned short  preTriggerLow;  /* Pre-Trigger Counts Low - Number of reading stored - Min 0  */
	unsigned short	sampleCountHigh;/* Sample Counts is the total number of reading to be taken including the pre-trigger reading */
	unsigned short	sampleCountLow; /* The min val is 1,Zero causes continuous reading and will not stop the acquistion until all of memory is full */
	unsigned short  triggerSource;  /* Control the trigger system - Master/Slaver, Ex-Trig,Pos/Neg, Soft-Trig, TTL setting */
	unsigned short  sampleSource;   /* Control the sample system - Int-clock, Ext-clock, Soft-sample, Pos/Neg, Abort, Init setting */

	float64  	samplingMaxRate;/* Max Sampling Rate of DAQ - User input data */
	float64  	samplePeriodMax;/* Max Sampling Rate of DAQ - User input data */
	float64  	samplePeriodMin;/* Max Sampling Rate of DAQ - User input data */
	float64  	sampleMaxCount; /* Max Count Sample Counts of Data per channel(4ch) - User input data */
	float64  	samplingRate;  	/* Sampling Rate of DAQ - User input data */
	float64  	samplePeriod;  	/* Sampling Rate of DAQ - User input data */
	float64  	sampleCount;  	/* Sample Counts of Data per channel(4ch) - User input data */
	float64  	samplePreCount;	/* Sample of Pre Trigger - User input data  */

	unsigned int    triggerType;   	/* triggerTypeVXI1564a  - User Interface input 0-soft Trig, 1-Ex-Trigger  */

 	int             sampleMode;   	/* data acquisition , finite number or continue    */
 	int32           actualSampleCount; 	/* Actual number of samples read from each channel  */
	int 		abortStop;	/* Pass to WaitTrig/Store Func for Abort Function. 0-normal, 1-passFunction (in eventFunction Thread)  */
	int          shotNumber;		/* KSTAR Current Shot Number  */
	float64		blipTime;		/* MPS Blip Time  */
	float64		daqTime;		/* VXI DAQ Start Time  */
	char		mdsServer[60]; 		/*  MDSplus Data Server Name */
	char		mdsTree[60];		/*   MDSplus Data DB Name */
	unsigned int   	opMode; 		/* KSTAR Operation Mode 0 = Local Mode, 1= Remote Mode , No Cal Mode - maybe */
	int32		mdsStatus;  		/*  MDSplus Status......  */

	int		eventDone;
	int 		dataPutDone;	/* I don't know who module complete the data store. So I check the done number. 0(Not yet),1(Done) */
	int32 _VI_FUNCH* cviCallback;

 	ELLLIST		   *pdataPoolList;
	ELLLIST		   *pchannelConfig;

	float64		   *pdata;

	drvVXI1564a_controlThreadConfig    *pcontrolThreadConfig;
	drvVXI1564a_controlThreadConfig    *peventMonitorConfig;
/*	drvVXI1564a_controlThreadConfig    *pcontrolStoreConfig;   */

	double          callbackTimeUsec;

	double          adjTime_smplRate_Usec;
	double          adjTime_cernoxGain_Usec;
	double          adjTime_lowPass_Usec;

	unsigned        adjCount_smplRate;
	unsigned        adjCount_cernoxGain;
	unsigned        adjCount_lowPass;
} drvVXI1564a_taskConfig;

typedef struct {
	ELLNODE		    node;
	unsigned long	used_count;
	int32		    num_read_data;
	float64		    *pdata;
} dataPoolConfig;

/*   I will not use this function. T.G 2010.03.20
typedef struct {
	ELLNODE			node;
	unsigned short	channelType;
	unsigned short	channelStatus;
	char			chanName[60];
	char			treeNode[60];
	unsigned		chanNum;
	unsigned		chanIndex;

	double  vxi1564aChBuffer[INIT_VXI1564a_MAX_DATA_SIZE];
	float64			gain;
	float64			cutOffFreq;

	float64			minRange;
	float64			maxRange;
	int32		    units;

	double          conversionTime_usec;

	drvVXI1564a_taskConfig	*ptaskConfig;

} drvVXI1564a_wfChannelConfig;
*/
typedef struct {
	ELLNODE			node;
	unsigned short	channelType;
	unsigned short	channelStatus;
	char			chanName[60];
	unsigned		chanNum;
	unsigned		chanIndex;

	float64			gain;
	float64			cutOffFreq;

	/* Module status check for channel function   */
	float32			voltRange;     /* 0.0625V, 0.25V, 1V, 4V, 16V, 64V, 256V select per channel */
	float32			filterRange;   /* Off, 1.5kHz, 6kHz, 25kHz, 100kHz select per channel       */

	float64			minRange;
	float64			maxRange;

	drvVXI1564a_taskConfig	*ptaskConfig;

} drvVXI1564a_aiChannelConfig;

typedef struct {
	ELLNODE			node;
	unsigned short	channelType;
	unsigned short	channelStatus;
	char			chanName[60];
	unsigned		chanNum;
	unsigned		chanIndex;

	char			treeNode[60];
	/* mbbo used value select function   */
	float32			voltRangeSet;    /* 0.0625V, 0.25V, 1V, 4V, 16V, 64V, 256V select per channel */
	float32			filterRangeSet;  /* Off, 1.5kHz, 6kHz, 25kHz, 100kHz select per channel       */
	/* Remove - why > Each module get 1 register function. AO channel make 4 channel for channel, so over space have if this used
	float  vxi1564aChBuffer[INIT_VXI1564a_MAX_DATA_SIZE];
	signed short  vxi1564aChBuffer[INIT_VXI1564a_MAX_DATA_SIZE];
	unsigned short		voltFilter12;    
	unsigned short		voltFilter34;   
	unsigned short		triggerLevel;  
	*/
	drvVXI1564a_taskConfig	*ptaskConfig;

} drvVXI1564a_aoChannelConfig;


typedef struct {
	ELLNODE		node;
	unsigned short channelType;
	unsigned short channelStatus;
	char		chanName[60];
	unsigned  	chanIndex;

	float64		gain;
	float64  	cutOffFreq;

} drvVXI1564a_channelConfig;

typedef void (*EPICSEXITFUNC)(void *arg);

drvVXI1564a_taskConfig* drvVXI1564a_find_taskConfig(char *taskName);
drvVXI1564a_channelConfig* drvVXI1564a_find_channelConfig(char *taskName, char *chanName);

void drvVXI1564a_set_channelGain(drvVXI1564a_taskConfig *ptaskConfig, double gain);
void drvVXI1564a_set_lowpassFilter(drvVXI1564a_taskConfig *ptaskConfig, double cutOffFreq);

void drvVXI1564a_set_taskExTrigger(drvVXI1564a_taskConfig *ptaskConfig, unsigned int exTrig);

void drvVXI1564a_set_taskCallback(drvVXI1564a_taskConfig *ptaskConfig);
void drvVXI1564a_set_startTask(drvVXI1564a_taskConfig *ptaskConfig);
void drvVXI1564a_set_stopTask(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setStop);
void drvVXI1564a_set_abortTask(drvVXI1564a_taskConfig *ptaskConfig, unsigned int setAbort);
void drvVXI1564a_set_regUpdateTask(drvVXI1564a_taskConfig *ptaskConfig);

void drvVXI1564a_set_sampleCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 sampleCount);
void drvVXI1564a_set_samplePreCount(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplePreCount);

void drvVXI1564a_set_triggerType(drvVXI1564a_taskConfig *ptaskConfig, int triggerType);

void drvVXI1564a_set_sampleMode(drvVXI1564a_taskConfig *ptaskConfig, int sampleMode);
/* void drvVXI1564a_set_samplingRate(drvVXI1564a_taskConfig *ptaskConfig, uInt32 samplingRate);  */
void drvVXI1564a_set_samplePeriod(drvVXI1564a_taskConfig *ptaskConfig, float64 samplePeriod);
void drvVXI1564a_set_shotNumber(drvVXI1564a_taskConfig *ptaskConfig, int shotNumber);
void drvVXI1564a_set_blipTime(drvVXI1564a_taskConfig *ptaskConfig, double blipTime);
void drvVXI1564a_set_daqTime(drvVXI1564a_taskConfig *ptaskConfig, double daqTime);
void drvVXI1564a_set_operMode(drvVXI1564a_taskConfig *ptaskConfig, unsigned int operMode);

void drvVXI1564a_set_channelRange(drvVXI1564a_taskConfig *ptaskConfig, drvVXI1564a_aoChannelConfig *paoChannelConfig, uInt16 setRange); 
void drvVXI1564a_set_channelFilter(drvVXI1564a_taskConfig *ptaskConfig,drvVXI1564a_aoChannelConfig *paoChannelConfig, uInt16 setFilter);

/* int drvVXI1564a_send_dataTree(drvVXI1564a_taskConfig *ptaskConfig);  */
int drvVXI1564a_create_newTree(drvVXI1564a_taskConfig *ptaskConfig);
int drvVXI1564a_mds_connectOpen(drvVXI1564a_taskConfig *ptaskConfig);
int drvVXI1564a_mds_disconnectCloe(drvVXI1564a_taskConfig *ptaskConfig);

int completeDataPut();

unsigned long long int drvVXI1564a_rdtsc(void);
double                 drvVXI1564a_intervalUSec(unsigned long long int start, unsigned long long int stop);

/*void clearAllVXITasks(void);*/
#endif /* drvVXI1564a_H */

