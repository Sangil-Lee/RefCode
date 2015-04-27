#ifndef drvM6802_H
#define drvM6802_H

#include "epicsRingBytes.h"
#include "epicsMessageQueue.h"
#include "epicsThread.h"
#include <epicsMutex.h>

#include "ellLib.h"
#include "callback.h"
#include "dbScan.h"


#include "ecehrGlobal.h"

#include <stdio.h>
#include <stdlib.h>


FILE *fpRaw;
/* int fd; */

typedef	signed char		int8;
typedef	unsigned char		uint8;
typedef	signed short		int16;
typedef	unsigned short		uint16;
typedef	signed int		int32;
typedef	unsigned int		uint32;
typedef	float			float32;
typedef	double			float64;


typedef struct {
	void		*ptaskConfig;
	void		*pchannelConfig;
	void		*precord;
	int		channelID;		/* use for TAG name setting */
	double		setValue;
	char		setStr[40];
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
	
} drvM6802_controlThreadConfig;



typedef struct {
		ELLLIST		*pbufferList;

} drvBufferConfig;

typedef struct {
	ELLNODE			node;
	unsigned int	nCnt;
	int nStop;

	int data[(1024*1024*3) / 4];

} drvBufferConfig_Node;


typedef struct {
	drvBufferConfig_Node *pNode;

} bufferingThreadQueueData;
	

typedef struct {
		ELLLIST		*ptaskList;

		int	openBus;

		unsigned long long int one_sec_interval;
} drvM6802Config;



typedef struct {
	ELLNODE			node;
	char			taskName[60];
	uint16			BoardID;
	uint16			vme_addr;
	epicsMutexId		taskLock;
	epicsMutexId		bufferLock;
	IOSCANPVT		ioScanPvt;

	unsigned int		taskStatus;
	unsigned int		eceCalMode;    /* val == 0 No Cal, val == 1 Ece Calibration Function. */
	uint32			shotNumber;

	uint16		masterMode;
	int		samplingRate;
	int		appSampleRate;
	int		channelMask;	
	uint16		clockSource;	/* val == 0 internal , val == 1 external */
/*	int		msgFPDP; */
	unsigned int    cnt_DMAcallback;
	int             nTrigPolarity;
	float           fT0;
	float           fT1;
/* Add ABS Time Zero Infomation is Data Start Time - Blip Start Time : Modify TG.Lee at 20080929 */ 
	float 		blipTime;               
/*  Delete and Create- why? : Globle Varable as ShotNumber - Data Transport at Function Start -> taskConfig add */
	unsigned short	register_m6802[16];

	unsigned short	zCal;  /* Add zcal, chInfo, hFilter TG.Lee */
	int		chInfo;
	unsigned short  hFilter;

	ELLLIST		*pchannelConfig;

	drvM6802_controlThreadConfig    *pcontrolThreadConfig;
	drvM6802_controlThreadConfig    *pfpdpThreadConfig;
	drvM6802_controlThreadConfig    *pringThreadConfig;
	drvM6802Config		*pdrvConfig;


	double          callbackTimeUsec;

	double          adjTime_smplRate_Usec;
	double          adjTime_Gain_Usec;

	unsigned        adjCount_smplRate;
	unsigned        adjCount_Gain;

} drvM6802_taskConfig;


typedef struct {
	ELLNODE		node;
	unsigned short channelStatus;
	char		chanName[60];
	unsigned  	chanIndex;

	int		gain;
/*	unsigned short	zCal;   Add zcal, chInfo, hFilter TG.Lee 
	int		chInfo;
	unsigned short  hFilter;  */
	
	drvM6802_taskConfig     *ptaskConfig;
	
	double		conversionTime_usec;

} drvM6802_channelConfig;

/* BO Record  Add TG.Lee	
typedef struct {
	ELLNODE		node;
	unsigned short  channelStatus;
	char		chanName[60];
	unsigned	chanIndex;

	int		gain;
	unsigned short	zcal;
	int		chInfo;
	unsigned short  hFilter;
		
	drvM6802_taskConfig	*ptaskConfig;
	
	double		conversionTime_usec;
} drvM6802_boChannelConfig;
*/


typedef void (*EPICSEXITFUNC)(void *arg);




drvM6802_taskConfig* drvM6802_find_taskConfig(char *taskName);
/*
drvM6802_channelConfig* drvM6802_find_channelConfig(char *taskName, char *chanName);
drvM6802_channelConfig* drvM6802_find_channelConfig_fromID(char *taskName, int ch);
*/

/*void drvM6802_set_chanGain(drvM6802_taskConfig *ptaskConfig, int channel, int gain); */
int drvM6802_set_zeroCalibration(drvM6802_taskConfig *ptaskConfig, uint16 val);
void drvM6802_set_samplingRate(uint16 samplingRate);
void drvM6802_set_channelMask(drvM6802_taskConfig *ptaskConfig, int cnt);
int drvM6802_set_clockSource(drvM6802_taskConfig *ptaskConfig, uint16 val);
int drvM6802_set_triggerSource(drvM6802_taskConfig *ptaskConfig, uint16 val);
int drvM6802_set_DAQready(drvM6802_taskConfig *ptaskConfig);
int drvM6802_set_DAQstart(drvM6802_taskConfig *ptaskConfig);
int drvM6802_set_DAQstop(drvM6802_taskConfig *ptaskConfig);
int drvM6802_set_DAQclear(drvM6802_taskConfig *ptaskConfig);


unsigned long long int drvM6802_getCurrentUsec(void);
double                 drvM6802_intervalUSec(unsigned long long int start, unsigned long long int stop);


#endif /* drvM6802_H */

