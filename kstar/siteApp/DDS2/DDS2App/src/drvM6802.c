
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

#include <sys/time.h>


#include "drvM6802.h"
#include "devMDSplus.h"

#include "drvFPDP.h"

#include "drvOzfunc.h"


#if 0
#define DEBUG
#endif


LOCAL drvM6802Config          *pdrvM6802Config = NULL;

LOCAL drvBufferConfig	*pdrvBufferConfig = NULL;

epicsMessageQueueId bufferingQueueID;

/*************  for open VME device *********************/
#define		STR_FILE_DEV	"/dev/vme_a24a16_user"
#define		NUM_OF_BYTE		32

#define DUMMY_DATA	0x0

#define	MM_PGA_ARRAY_1		0
#define	MM_PGA_ARRAY_2		2
#define	MM_PGA_ARRAY_3		4
#define	MM_PGA_ARRAY_4		6
#define	MM_ADC_HPFE			8
#define	MM_ADC_ZCAL			10
#define	MM_CNTRL_REG_1		12
#define	MM_CNTRL_REG_2		14

int		_fd;
unsigned int	gran_loss;
unsigned short	buf_with_dummy[16];

/********************************************************/


/* test function */
unsigned long long int drvM6802_getCurrentUsec(void)
{
	unsigned long long int x;
	struct timeval tv;

/*	__asm__ volatile(".byte 0x0f, 0x31": "=A"(x));	*/
	gettimeofday( &tv, NULL);

	x = tv.tv_sec;
	x = (x*1.E+6) + tv.tv_usec; 
	
/*	return timecurrent.tv_sec*1000 + timecurrent.tv_usec/	
	return x;
*/
	return x;
	
}
/*
double drvM6802_intervalUSec(unsigned long long int start, unsigned long long int stop)
{
	return 1.E+6 * (double)(stop-s:tart)/(double)(pdrvM6802Config->one_sec_interval);
}
*/
double drvM6802_intervalUSec(unsigned long long int start, unsigned long long int stop)
{
	return (stop-start);
}


static int initDAQboard(drvM6802_taskConfig *ptaskConfig); 
static void init_channelGain(drvM6802_channelConfig *pchannelConfig);
static void kill_FPDP();

/*static void set_allChanGain(drvM6802_taskConfig *ptaskConfig, int gain);
static void set_chanGain(drvM6802_taskConfig *ptaskConfig, int chan, int gain);
*/
static void set_ZeroCalibration(drvM6802_taskConfig *ptaskConfig,uint16 val);
static void set_samplingRate(drvM6802_taskConfig *ptaskConfig, uint16 samplingRate);


static int make_drvConfig(uint16 vme_addr);
static drvM6802_taskConfig* make_taskConfig(char *taskName, uint16 vme_addr);
static drvM6802_taskConfig* find_taskConfig(char *taskName);
static drvM6802_channelConfig* find_channelConfig(drvM6802_taskConfig *ptaskConfig, char *chanName);
/*static drvM6802_channelConfig* find_channelConfig_fromID(drvM6802_taskConfig *ptaskConfig, int ch); */
/*static void print_channelConfig(drvM6802_taskConfig *ptaskConfig, int level); */

static drvM6802_controlThreadConfig* make_controlThreadConfig(drvM6802_taskConfig *ptaskConfig);
static drvM6802_controlThreadConfig* make_fpdpThreadConfig(drvM6802_taskConfig *ptaskConfig);
static drvM6802_controlThreadConfig* make_ringThreadConfig(drvM6802_taskConfig *ptaskConfig);

static void controlThreadFunc(void *param);
static void FPDPthreadFunc(void *param);
static void RingThreadFunc(void *param);

static void clearAllOzTasks(void);


static void createTaskCallFunction(const iocshArgBuf *args);
static void createChannelCallFunction(const iocshArgBuf *args);



static const iocshArg createTaskArg0 = {"task name", iocshArgString};
static const iocshArg createTaskArg1 = {"vme address", iocshArgString};
static const iocshArg* const createTaskArgs[] = { &createTaskArg0 , &createTaskArg1 };
static const iocshFuncDef createTaskFuncDef = { "createTask",
 						   2,
					           createTaskArgs };

static const iocshArg createChannelArg0 = {"task name", iocshArgString};
static const iocshArg createChannelArg1 = {"channel number", iocshArgString};
static const iocshArg createChannelArg2 = {"channel name", iocshArgString};
static const iocshArg* const createChannelArgs[] = { &createChannelArg0, &createChannelArg1, &createChannelArg2 };
static const iocshFuncDef createChannelFuncDef = {"createChannel",
							   3,
							   createChannelArgs};




static void epicsShareAPI drvM6802Registrar(void)
{
	static int firstTime =1;
	
	if(firstTime) {
		firstTime = 0;
		iocshRegister(&createTaskFuncDef, createTaskCallFunction);
		iocshRegister(&createChannelFuncDef, createChannelCallFunction);
	}
}
epicsExportRegistrar(drvM6802Registrar);




LOCAL long drvM6802_io_report(int level);
LOCAL long drvM6802_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvM6802 = {
       2,
       drvM6802_io_report,
       drvM6802_init_driver
};

epicsExportAddress(drvet, drvM6802);


static void kill_FPDP()
{
	if( !stopFpdp() )	printf("\n>>> kill_FPDP : can't stop FPDP \n");
	
	if( fpRaw != NULL )
		fclose(fpRaw);	
	
	terminate();
}

static int initDAQboard(drvM6802_taskConfig *ptaskConfig)
{

	if( !ozSetGlobalReset( ptaskConfig ) ) {
		epicsPrintf("\n>>> ID:%d initDAQboard : ozSetGlobalReset...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}
	epicsPrintf("\n>>> ID:%d, 0x%4X initDAQboard : ozSetGlobalReset  ok\n", ptaskConfig->BoardID,ptaskConfig->vme_addr );

	epicsThreadSleep(0.5);

#ifdef DEBUG
        epicsPrintf("initDAQboard: %s addr:0x%X\n", ptaskConfig->taskName, ptaskConfig->vme_addr);
#endif /* DEBUG */

#if 1
	if( ptaskConfig->masterMode ) {
		if( !ozSetADsFIFOreset( ptaskConfig ) ) {
			epicsPrintf("\n>>> id:%d initDAQboard : ozSetADsFIFOreset...failed! \n", ptaskConfig->BoardID);
			return ERROR;
		}
/*
		if( !ozSetTriggerReset( ptaskConfig ) ) {
			epicsPrintf("\n>>> id:%d initDAQboard : ozSetTriggerReset...failed! \n", ptaskConfig->BoardID);		
			return ERROR;
		}
*/
	}

	if( !ozSetTermDisable( ptaskConfig ) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetTermDisable...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}


	if( !ozSetSampleClkDivider( ptaskConfig, 1 ) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetSampleClkDivider...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}
#endif

	if( !ozSetSamplingRate( ptaskConfig, ptaskConfig->samplingRate) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetSamplingRate...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}

	if( !ozSetTriggerPolarity( ptaskConfig) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetTriggerPolarity...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}
#if 1
	if( !ozSetMasterOrSlave( ptaskConfig ) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetMasterOrSlave...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}
#endif

	if( !ozSetClockInterExter( ptaskConfig) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetClockInterExter...failed! \n", ptaskConfig->BoardID);
		return ERROR;
	}


	if( !ozSetChannelMask( ptaskConfig ) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetChannelMask...failed! \n", ptaskConfig->BoardID);		
		return ERROR;
	}


	if( !ozSetBoardIDBit( ptaskConfig ) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetBoardIDBit...failed! \n", ptaskConfig->BoardID);		
		return ERROR;
	}
	
	if( !ozSetStrobeTTL( ptaskConfig ) ) {
		epicsPrintf("\n>>> id:%d initDAQboard : ozSetStrobeTTL...failed! \n", ptaskConfig->BoardID);		
		return ERROR;
	}
	

	ptaskConfig->taskStatus |= TASK_INITIALIZED;
	
	return OK;
}

static void init_channelGain(drvM6802_channelConfig *pchannelConfig)
{
	drvM6802_taskConfig *ptaskConfig = pchannelConfig->ptaskConfig;

	if(pchannelConfig->channelStatus & CHANNEL_INITIALIZED) return;
	
#ifdef DEBUG
	epicsPrintf("init_channel: task: %s, channel: %s\n", 
		    ptaskConfig->taskName, pchannelConfig->chanName);
#endif
	
	if( !ozSetAIGain(ptaskConfig, pchannelConfig->chanIndex, pchannelConfig->gain) ) {
		epicsPrintf("\n>>> init_channelGain : ozSetAIGain...failed! \n");

		return;
	}

	pchannelConfig->channelStatus |= CHANNEL_INITIALIZED;
	return;
}


/*
static void set_allChanGain(drvM6802_taskConfig *ptaskConfig, int gain)
{
	drvM6802_channelConfig *pchannelConfig;	
	unsigned long long int start, stop;

	if(!ptaskConfig) return;

	start = drvM6802_getCurrentUsec();

	if( ptaskConfig->taskStatus & TASK_ADC_STARTED ) {
		epicsPrintf("\n>>> set_allChanGain : DAQ Started!\n");
		return ;
	}


	pchannelConfig = (drvM6802_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {		
		pchannelConfig->gain = gain;
		
		if( !ozSetAIGain(ptaskConfig, pchannelConfig->chanIndex, pchannelConfig->gain) ) {
			epicsPrintf("\n>>> set_allChanGain : ozSetAIGain...failed! \n");
			return;
		}

		pchannelConfig = (drvM6802_channelConfig*) ellNext(&pchannelConfig->node);
	}


	stop = drvM6802_getCurrentUsec();
	ptaskConfig->adjTime_Gain_Usec = drvM6802_intervalUSec(start, stop);
	ptaskConfig->adjCount_Gain++;


	return;
}


static void set_chanGain(drvM6802_taskConfig *ptaskConfig, int chan, int gain)
{

	unsigned long long int start, stop;

	if(!ptaskConfig) return;
	if( ptaskConfig->taskStatus & TASK_ADC_STARTED ) {
		epicsPrintf("\n>>> set_chanGain : DAQ Started!\n");
		return ;
	}

	start = drvM6802_getCurrentUsec();


	if( !ozSetAIGain(ptaskConfig, chan, gain) ) {
		epicsPrintf("\n>>> set_chanGain : ozSetAIGain...failed! \n");
		return;
	}


	stop = drvM6802_getCurrentUsec();
	ptaskConfig->adjTime_Gain_Usec = drvM6802_intervalUSec(start, stop);
	ptaskConfig->adjCount_Gain++;

	return;
}
*/
static void set_ZeroCalibration(drvM6802_taskConfig *ptaskConfig, uint16 val)
{

	int chan;
/*	unsigned short val;  */
	
	chan = ptaskConfig->chInfo;
/*	val = ptaskConfig->zCal;  */

	if(!ptaskConfig) return;
	if( ptaskConfig->taskStatus & TASK_ADC_STARTED ) {
		epicsPrintf("\n>>> set_chanGain : DAQ Started!\n");
		return ;
	}

	if( !ozSetZeroCalibration(ptaskConfig, chan, val) ) {
		epicsPrintf("\n>>> set_ZeroCalibration : ozSetZeroCalibration...failed! \n");
		return;
	}
	return;
}

static void set_samplingRate(drvM6802_taskConfig *ptaskConfig, uint16 samplingRate)
{
	unsigned long long int start, stop;


	start = drvM6802_getCurrentUsec();

	if ( ptaskConfig->taskStatus & TASK_ADC_STARTED ) {
		epicsPrintf("\n>>> set_samplingRate : DAQ already started! \n");
		return;
	}

	ptaskConfig->samplingRate = samplingRate;

/*	epicsPrintf("\n>>> set_samplingRate : %s (%d) \n", ptaskConfig->taskName, samplingRate); */

	if( !ozSetSamplingRate( ptaskConfig,	ptaskConfig->samplingRate) ) {
		epicsPrintf("\n>>> set_samplingRate : ozSetSamplingRate...failed! \n");

	} 


	epicsPrintf("\n>>> set_samplingRate : taskStatus 0x%X\n", ptaskConfig->taskStatus);
	stop = drvM6802_getCurrentUsec();
	ptaskConfig->adjTime_smplRate_Usec = drvM6802_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;
}


static int make_drvConfig(uint16 vme_addr)
{
/*	static int cnt = 0;
	epicsPrintf(">>>>> call make_drvConfig cnt : %d\n", cnt);
	cnt++;
*/
/* just call only once */

	if(pdrvM6802Config) return 0;

/*	epicsPrintf(">>>>> %d\n", cnt);	*/
	

	pdrvM6802Config = (drvM6802Config*) malloc(sizeof(drvM6802Config));
	if(!pdrvM6802Config) return 1;

	pdrvBufferConfig = (drvBufferConfig*) malloc(sizeof(drvBufferConfig));
	if(!pdrvBufferConfig) return 1;

	pdrvM6802Config->openBus = 0;
	if( ozInitDevice( vme_addr ) == ERROR ) return 0;
	else epicsPrintf("\n>>> make_drvConfig(0x%04X) : ozInitDevice .... OK\n", vme_addr);
	pdrvM6802Config->openBus = 1;


	pdrvM6802Config->ptaskList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvM6802Config->ptaskList) return 1;
	else ellInit(pdrvM6802Config->ptaskList);


	pdrvBufferConfig->pbufferList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvBufferConfig->pbufferList) return 1;
	else ellInit(pdrvBufferConfig->pbufferList);



	return 0;
}

static drvM6802_taskConfig* make_taskConfig(char *taskName, uint16 vme_addr)
{
	static uint16 boardcnt = 1;
	drvM6802_taskConfig *ptaskConfig = NULL;
	char task_name[60];
	int i;

	strcpy(task_name, taskName);

	ptaskConfig = (drvM6802_taskConfig*) malloc(sizeof(drvM6802_taskConfig));
	if(!ptaskConfig) return ptaskConfig;


	ptaskConfig->BoardID = boardcnt;
	ptaskConfig->vme_addr = vme_addr;
	strcpy(ptaskConfig->taskName, task_name);
	ptaskConfig->taskLock = epicsMutexCreate();
	ptaskConfig->bufferLock = epicsMutexCreate();
	scanIoInit(&ptaskConfig->ioScanPvt);
/*	ptaskConfig->masterMode = 0; */
	ptaskConfig->masterMode = ( ptaskConfig->BoardID == 1 ) ? 1 : 0;
	ptaskConfig->samplingRate = INIT_SAMPLING;
	ptaskConfig->appSampleRate = INIT_SAMPLING;
	ptaskConfig->channelMask = INIT_CH_MASK;
	ptaskConfig->clockSource = (ptaskConfig->BoardID == 1) ? 0 : 1; /* default 0 : internal clock ,  1: external*/
/*	ptaskConfig->msgFPDP	=	0; */
	ptaskConfig->cnt_DMAcallback = 0;
	ptaskConfig->nTrigPolarity = INIT_TRIG_POLARITY;
/*	ptaskConfig->pcallback = (CALLBACK*) taskCallback; */
	ptaskConfig->fT0 = 1.;
	ptaskConfig->fT1 = 2.;

	boardcnt++;

	ptaskConfig->pcontrolThreadConfig = NULL;
	ptaskConfig->pfpdpThreadConfig = NULL;


	for( i=0; i< 16 ; i++) 
		ptaskConfig->register_m6802[i] = DUMMY_DATA;

	ptaskConfig->adjTime_smplRate_Usec = 0.;
	ptaskConfig->adjTime_Gain_Usec = 0.;

	ptaskConfig->adjCount_smplRate = 0;
	ptaskConfig->adjCount_Gain = 0;


	ptaskConfig->pchannelConfig = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!ptaskConfig->pchannelConfig) return NULL;
	else ellInit(ptaskConfig->pchannelConfig);

	ptaskConfig->taskStatus = TASK_NOT_INIT;

	return ptaskConfig;

}

static drvM6802_taskConfig* find_taskConfig(char *taskName)
{
	drvM6802_taskConfig *ptaskConfig = NULL;
	char task_name[60];

	strcpy(task_name, taskName);

	ptaskConfig = (drvM6802_taskConfig*) ellFirst(pdrvM6802Config->ptaskList);
	while(ptaskConfig) {
		if(!strcmp(ptaskConfig->taskName, task_name)) return ptaskConfig;
  		ptaskConfig = (drvM6802_taskConfig*) ellNext(&ptaskConfig->node);
	}

    	return ptaskConfig;
}
/**********************************************************************************/
/*	��忡 ��ä� ã´�																						*/
/**********************************************************************************/

static drvM6802_channelConfig* find_channelConfig(drvM6802_taskConfig *ptaskConfig, char *chanName)
{
	drvM6802_channelConfig *pchannelConfig = NULL;
	char channel_name[60];

	strcpy(channel_name, chanName);

	pchannelConfig = (drvM6802_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		if(!strcmp(pchannelConfig->chanName, channel_name)) return pchannelConfig;
		pchannelConfig = (drvM6802_channelConfig*) ellNext(&pchannelConfig->node);
	}

	return pchannelConfig;
}


static unsigned find_channelIndex(drvM6802_taskConfig *ptaskConfig, char *chanName)
{
	drvM6802_channelConfig *pchannelConfig = find_channelConfig(ptaskConfig, chanName);
	if(!pchannelConfig) return -1;
	else return pchannelConfig->chanIndex;
}

/*
static drvM6802_channelConfig* find_channelConfig_fromID(drvM6802_taskConfig *ptaskConfig, int ch)
{
	drvM6802_channelConfig *pchannelConfig = NULL;

	pchannelConfig = (drvM6802_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		if( pchannelConfig->chanIndex == ch) return pchannelConfig;
		pchannelConfig = (drvM6802_channelConfig*) ellNext(&pchannelConfig->node);
	}

	fprintf(stderr,"\n>>> find_channelConfig_fromID : can't find ch %d\n", ch);
	return NULL;
}
*/
/*
static void print_channelConfig(drvM6802_taskConfig *ptaskConfig, int level)
{
	drvM6802_channelConfig *pchannelConfig;

	pchannelConfig = (drvM6802_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		epicsPrintf("    %s", pchannelConfig->chanName);
		if(level>2) {
			epicsPrintf("\tChannel, status: 0x%x, index: %d, gain: %f", 
				    pchannelConfig->channelStatus,
				    pchannelConfig->chanIndex,
				    pchannelConfig->gain );
			if(level>3) {
				epicsPrintf(", conv.T: %f usec", pchannelConfig->conversionTime_usec);
			}

		}
		epicsPrintf("\n");
  		pchannelConfig = (drvM6802_channelConfig*) ellNext(&pchannelConfig->node);
	}

	return;

}
*/
static drvM6802_controlThreadConfig*  make_controlThreadConfig(drvM6802_taskConfig *ptaskConfig)
{
	drvM6802_controlThreadConfig *pcontrolThreadConfig  = NULL;

	pcontrolThreadConfig  = (drvM6802_controlThreadConfig*)malloc(sizeof(drvM6802_controlThreadConfig));
	if(!pcontrolThreadConfig) return pcontrolThreadConfig; 

	sprintf(pcontrolThreadConfig->threadName, "%s_ctrl", ptaskConfig->taskName);
	
	pcontrolThreadConfig->threadPriority = epicsThreadPriorityLow;
	pcontrolThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pcontrolThreadConfig->threadFunc      = (EPICSTHREADFUNC) controlThreadFunc;
	pcontrolThreadConfig->threadParam     = (void*) ptaskConfig;

	pcontrolThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));

	epicsThreadCreate(pcontrolThreadConfig->threadName,
			  pcontrolThreadConfig->threadPriority,
			  pcontrolThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pcontrolThreadConfig->threadFunc,
			  (void*) pcontrolThreadConfig->threadParam);


	return pcontrolThreadConfig;
}

static drvM6802_controlThreadConfig*  make_fpdpThreadConfig(drvM6802_taskConfig *ptaskConfig)
{
	drvM6802_controlThreadConfig *pfpdpThreadConfig  = NULL;

	pfpdpThreadConfig  = (drvM6802_controlThreadConfig*)malloc(sizeof(drvM6802_controlThreadConfig));
	if(!pfpdpThreadConfig) return pfpdpThreadConfig; 

	sprintf(pfpdpThreadConfig->threadName, "FPDP_ctrl");	
	pfpdpThreadConfig->threadPriority = epicsThreadPriorityHigh;
	pfpdpThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackBig);
	pfpdpThreadConfig->threadFunc      = (EPICSTHREADFUNC) FPDPthreadFunc;
	pfpdpThreadConfig->threadParam     = (void*) ptaskConfig;

	pfpdpThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(bufferingThreadQueueData));
	bufferingQueueID = pfpdpThreadConfig->threadQueueId;
/*	pfpdpThreadConfig->threadQueueId   = 0;	*/
/*	if(pfpdpThreadConfig->threadQueueId) ptaskConfig->taskStatus |= TASK_CONTQUEUE; */

	epicsThreadCreate(pfpdpThreadConfig->threadName,
			  pfpdpThreadConfig->threadPriority,
			  pfpdpThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pfpdpThreadConfig->threadFunc,
			  (void*) pfpdpThreadConfig->threadParam);


	return pfpdpThreadConfig;
}

static drvM6802_controlThreadConfig*  make_ringThreadConfig(drvM6802_taskConfig *ptaskConfig)
{
	drvM6802_controlThreadConfig *pringThreadConfig  = NULL;

	pringThreadConfig  = (drvM6802_controlThreadConfig*)malloc(sizeof(drvM6802_controlThreadConfig));
	if(!pringThreadConfig) return pringThreadConfig; 

	sprintf(pringThreadConfig->threadName, "RING_ctrl");	
	pringThreadConfig->threadPriority = epicsThreadPriorityLow;
	pringThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackBig);
	pringThreadConfig->threadFunc      = (EPICSTHREADFUNC) RingThreadFunc;
	pringThreadConfig->threadParam     = (void*) ptaskConfig;

/*	pringThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData)); */
	pringThreadConfig->threadQueueId   = 0; 	/* we don't need this */
/*	if(pringThreadConfig->threadQueueId) ptaskConfig->taskStatus |= TASK_CONTQUEUE; */

	epicsThreadCreate(pringThreadConfig->threadName,
			  pringThreadConfig->threadPriority,
			  pringThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pringThreadConfig->threadFunc,
			  (void*) pringThreadConfig->threadParam);


	return pringThreadConfig;
}


static void controlThreadFunc(void *param)
{

	drvM6802_taskConfig *ptaskConfig = (drvM6802_taskConfig*) param;
	drvM6802_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData		queueData;

	while( !ptaskConfig->pcontrolThreadConfig ) epicsThreadSleep(.1);
	pcontrolThreadConfig = (drvM6802_controlThreadConfig*) ptaskConfig->pcontrolThreadConfig;


	epicsPrintf("task launching: %s thread for %s task\n",pcontrolThreadConfig->threadName, ptaskConfig->taskName);

	while(TRUE) {
		EXECFUNCQUEUE            pFunc;
		execParam                *pexecParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		drvM6802_taskConfig    *ptaskConfig;

		epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId,
				         (void*) &queueData,
					 sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;	
		precord    = (struct dbCommon*) pexecParam->precord;
		prset      = (struct rset*) precord->rset;
		ptaskConfig = (drvM6802_taskConfig *) pexecParam->ptaskConfig;

		if(!pFunc) continue;
		else pFunc(pexecParam);

		if(!precord) continue;


		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}


	return;
}

static void RingThreadFunc(void *param)
{
	int i,j,ss;

/*	unsigned long long int start, stop,currentCal;
	uint32 shotNu;
*/
	drvM6802_taskConfig *ptaskConfig = (drvM6802_taskConfig*) param;
	drvM6802_controlThreadConfig *pringThreadConfig;

	bufferingThreadQueueData queueData;
	drvBufferConfig_Node *pbufferNode;


	while( !ptaskConfig->pringThreadConfig ) epicsThreadSleep(.1);
	pringThreadConfig = (drvM6802_controlThreadConfig*) ptaskConfig->pringThreadConfig;


	for(i = 0; i< 73; i++)
	{
		drvBufferConfig_Node *pbufferNode = NULL;
		pbufferNode = (drvBufferConfig_Node*) malloc(sizeof(drvBufferConfig_Node));
		if(!pbufferNode) {
			epicsPrintf("\n>>> malloc(sizeof(drvBufferConfig_Node))... fail\n");
			return;
		}
		ellAdd(pdrvBufferConfig->pbufferList, &pbufferNode->node);
	}


	epicsPrintf("ellCount: %d <init task launching> \n", ellCount(pdrvBufferConfig->pbufferList) );
	epicsPrintf("task launching: %s thread for %s task\n",pringThreadConfig->threadName, ptaskConfig->taskName);

	i = INIT_CH_MASK;   
	ss = 0;	
	while(TRUE) 
	{
		epicsMessageQueueReceive(bufferingQueueID, (void*) &queueData, sizeof(bufferingThreadQueueData));

	/*	epicsThreadSleep(.5); */
#if 0
		epicsPrintf(">>> rcv ID: %d, dma cnt: %d, stop: %d\n", bufferingQueueID, 
				queueData.cnt_DMAcallback, 
				queueData.nStop);
#endif
		pbufferNode = queueData.pNode;

		if(pbufferNode) 
		{
			if( pbufferNode->nStop ) {
#if 0
				/*	if( queueData.nStop)*/
				/*	epicsPrintf(">>> rcv ID: %d, dma cnt: %d, stop: %d\n", bufferingQueueID, 
					queueData.cnt_DMAcallback, 
					queueData.nStop);
				 */
#endif
				fwrite( &i, 4, 1, fpRaw ); /* number of used channel = whole channel */
				fwrite( &dmaSize, 4, 1, fpRaw );
				fwrite( &pbufferNode->nCnt, 4, 1, fpRaw );
				fclose(fpRaw);
				fpRaw = NULL;
				/*	nDMAcnt = 0; */
				/*	epicsPrintf("last ell count before : %d \n", ellCount(pdrvBufferConfig->pbufferList) ); */
				ellAdd(pdrvBufferConfig->pbufferList, &pbufferNode->node);
				epicsPrintf("ellCount: %d <end> \n", ellCount(pdrvBufferConfig->pbufferList) );
				ss = ellCount(pdrvBufferConfig->pbufferList);
				if(ss<73){
					for(j = ss; j <= 73; j++)
					{
						ellAdd(pdrvBufferConfig->pbufferList, &pbufferNode->node);
					}
					epicsPrintf("Ell Buffer lack add Complated : ellCnt %d \n", ellCount(pdrvBufferConfig->pbufferList) ); 
				}

			} else {
#if 0    /* TG.Lee Enable 20081111 */
				epicsPrintf("RingThreadFunc Time Register Address TG.Lee\n");
				epicsPrintf(">>> MM_CNTRL_REG_1 : 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_1] );
				epicsPrintf(">>> MM_CNTRL_REG_2 : 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_2] );
				epicsPrintf(">>> MM_PGA_ARRAY_1 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_1] );
				epicsPrintf(">>> MM_PGA_ARRAY_2 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_2] );
				epicsPrintf(">>> MM_PGA_ARRAY_3 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_3] );
				epicsPrintf(">>> MM_PGA_ARRAY_4 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_4] );
				epicsPrintf(">>> MM_ADC_HPFE : 0x%X\n", ptaskConfig->register_m6802[MM_ADC_HPFE] );
				epicsPrintf(">>> MM_ADC_ZCAL : 0x%X\n", ptaskConfig->register_m6802[MM_ADC_ZCAL] );

				/*	epicsPrintf(">>> rcv ID: %d, dma cnt: %d, stop: %d\n", bufferingQueueID, 
					queueData.cnt_DMAcallback, 
					queueData.nStop);
				 */
				/*     if( nDMAcnt != pbufferNode->nCnt )
				       epicsPrintf("\n>>> RingThreadFunc : Get Message order error! cnt(%d) \n", pbufferNode->nCnt);
				 */
#endif
				fwrite( pbufferNode->data, dmaSize, 1, fpRaw );
				/*	nDMAcnt = queueData.cnt_DMAcallback +1; */
				/*	epicsPrintf("ell count before : %d \n", ellCount(pdrvBufferConfig->pbufferList) ); */
				ellAdd(pdrvBufferConfig->pbufferList, &pbufferNode->node);
				epicsPrintf("ellCount :ellCnt %d <first> \n", ellCount(pdrvBufferConfig->pbufferList) );  
			}
#if 0
			/*		epicsPrintf("Wait Ring ?? %d \n", ss++);   */
			/*
			   if (ptaskConfig->eceCalMode) {
			   start = drvM6802_getCurrentUsec();
			   drvM6802_set_DAQstop(ptaskConfig);
			   epicsThreadSleep(0.1);
			   drvM6802_set_DAQclear(ptaskConfig);
			   epicsThreadSleep(0.1);
			   dataChannelizationAndSendData(ptaskConfig->shotNumber, 3);							
			   epicsThreadSleep(0.1);
			   shotNu = ptaskConfig->shotNumber;
			   ptaskConfig->shotNumber = shotNu + 1;
			   epicsPrintf("Local Mode - ECE Calibration => shot Number : %d \n",ptaskConfig->shotNumber);
			   pbufferNode->nStop = 1;
			   epicsThreadSleep(0.1);
			   drvM6802_set_DAQready(ptaskConfig);
			   epicsThreadSleep(0.1);
			   drvM6802_set_DAQstart(ptaskConfig);
			   epicsThreadSleep(0.1);
			   stop = drvM6802_getCurrentUsec();
			   currentCal = drvM6802_intervalUSec(start, stop);
			   epicsPrintf("\nECE Calibration Cycle time is %lf msec\n", 1.E-3 * (double)currentCal);	
			   }
			 */
			/*			pbufferNode = (drvBufferConfig_Node*) ellNext(&pbufferNode->node); */

#endif
		} else
			epicsPrintf("\n>>> RingThreadFunc : no ellFirst node \n");
	}
}

static void FPDPthreadFunc(void *param)
{
	unsigned long long int start, stop,currentCal;
	uint32 shotNu;
	int toggle,i;
	int statusVal;  /* Add TG.Lee 20101108 */

	dmaToggle = 1;
	toggle = 1;
	drvBufferConfig_Node *pbufferNode = NULL;
	drvM6802_taskConfig *ptaskConfig = (drvM6802_taskConfig*) param;
	drvM6802_controlThreadConfig *pfpdpThreadConfig;

	bufferingThreadQueueData queueData;


	while( !ptaskConfig->pfpdpThreadConfig ) epicsThreadSleep(.1);
	pfpdpThreadConfig = (drvM6802_controlThreadConfig*) ptaskConfig->pfpdpThreadConfig;


	if( drvM6802_init_FPDP() == (-1) ) {
		epicsPrintf("\n>>> FPDPthreadFunc : drvM6802_init_FPDP() ... fail\n");
		return;
	} else 
		epicsPrintf("\n>>> FPDPthreadFunc : drvM6802_init_FPDP() ... OK\n");
/*
	if( !startFpdp() ) {
		printf(" can't start FPDP \n");
		terminate();
		return;
	}
*/
	i=0;
	while(TRUE) {
		/* Wait for DMA transfer to complete */
		sem_wait (&dmaDoneSemaphore_in);

		statusVal = doCacheInvalidate(); 
		if(statusVal ==1){	
		if( dmaToggle ) {
			pbufferNode = (drvBufferConfig_Node *)ellFirst(pdrvBufferConfig->pbufferList);
			ellDelete(pdrvBufferConfig->pbufferList, &pbufferNode->node);    
			memcpy( pbufferNode->data, buffer_in, dmaSize/2); 
			dmaToggle = 0;
			toggle = 0;
			epicsPrintf("sem_wait_toggle zero status:%d, :ellCnt %d \n", statusVal, ellCount(pdrvBufferConfig->pbufferList) ); 
		} else {
			memcpy( pbufferNode->data+(dmaSize/2)/sizeof(int), buffer_in+(dmaSize/2)/sizeof(int), dmaSize/2);
			pbufferNode->nCnt = ptaskConfig->cnt_DMAcallback;
			pbufferNode->nStop = 0;
			queueData.pNode = pbufferNode;
			epicsMessageQueueSend(pfpdpThreadConfig->threadQueueId, (void*) &queueData, sizeof(bufferingThreadQueueData));
/*			epicsPrintf("\n>>> snd ID: %d, DMA cnt: %d\n", pfpdpThreadConfig->threadQueueId, ptaskConfig->cnt_DMAcallback);  */

			dmaToggle = 1;
			toggle = 1;
			ptaskConfig->cnt_DMAcallback++;
			epicsPrintf("sem_wait_toggle one status:%d, :ellCnt %d \n", statusVal, ellCount(pdrvBufferConfig->pbufferList) ); 

		}
/*		epicsPrintf("Wait FPDP ?? %d \n", i++);   */
		if (ptaskConfig->eceCalMode && toggle ==1) {
			start = drvM6802_getCurrentUsec();
			epicsThreadSleep(1.1);
			drvM6802_set_DAQstop(ptaskConfig);
			epicsThreadSleep(1.1);
			drvM6802_set_DAQclear(ptaskConfig);
			epicsThreadSleep(0.1);
			dataChannelizationAndSendData(ptaskConfig->shotNumber, 3);							
			shotNu = ptaskConfig->shotNumber;
			ptaskConfig->shotNumber = shotNu + 1;
			epicsPrintf("Local Mode - ECE Calibration => shot Number : %d \n",ptaskConfig->shotNumber);
			drvM6802_set_DAQready(ptaskConfig);
			epicsThreadSleep(0.1);
			drvM6802_set_DAQstart(ptaskConfig);
			epicsThreadSleep(0.1);
			stop = drvM6802_getCurrentUsec();
			currentCal = drvM6802_intervalUSec(start, stop);
			epicsPrintf("\nECE Calibration Cycle time is %lf msec\n", 1.E-3 * (double)currentCal);	
			}
		} else {
			epicsPrintf("doCacheInvalidate F-Error stat:%d,:ellCnt %d \n", statusVal, ellCount(pdrvBufferConfig->pbufferList)); 
		
		}

	} /* while(TRUE) */

	terminate();
	printf("FPDP >>> Terminated!\n");

	return;
}



LOCAL long drvM6802_init_driver(void)
{
	drvM6802_taskConfig *ptaskConfig = NULL;
/*	drvM6802_taskConfig *ptaskConfigNext = NULL; */
	drvM6802_channelConfig *pchannelConfig = NULL;
	FILE *fp;
	int temp=0;

	unsigned long long int a;

	if(!pdrvM6802Config)  return 0;

	fp = fopen(FILE_NAME_RAWDATA, "wb");
	if( fp != NULL ) {
		fwrite( &temp, 4, 1, fp);
		fclose(fp);
	}


	ptaskConfig = (drvM6802_taskConfig*) ellFirst(pdrvM6802Config->ptaskList);
	while(ptaskConfig) 
	{
		if(ellCount(ptaskConfig->pchannelConfig)) 
		{

			initDAQboard(ptaskConfig);

			pchannelConfig = (drvM6802_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
			while(pchannelConfig) {
				init_channelGain(pchannelConfig);
				pchannelConfig = (drvM6802_channelConfig*) ellNext(&pchannelConfig->node);
			}

		} 

		ptaskConfig = (drvM6802_taskConfig*) ellNext(&ptaskConfig->node);
	}
	ptaskConfig = (drvM6802_taskConfig*) ellLast(pdrvM6802Config->ptaskList);
	if(ptaskConfig) 
	{
		ozSetTermEnable(ptaskConfig);
		epicsPrintf("\n>>> drvM6802_init_driver : Board %d : Termination Enabled\n", ptaskConfig->BoardID );
	} else 
		epicsPrintf("\n>>> drvM6802_init_driver : Board %d : Termination Error!\n", ptaskConfig->BoardID );


	epicsAtExit((EPICSEXITFUNC) clearAllOzTasks, NULL);

	a = drvM6802_getCurrentUsec();
	epicsThreadSleep(1.);
	pdrvM6802Config->one_sec_interval = drvM6802_getCurrentUsec() - a;

	epicsPrintf("drvM6802: measured one second time is %lf msec\n", 1.E-3 * (double)pdrvM6802Config->one_sec_interval);



	return 0;
}

static void clearAllOzTasks(void)
{
	drvM6802_taskConfig *ptaskConfig;
	FILE *fp;
	int temp=0;

#ifdef DEBUG
	epicsPrintf("clearAllOzTasks... ");
#endif

	ptaskConfig = (drvM6802_taskConfig*) ellFirst (pdrvM6802Config->ptaskList);

	kill_FPDP();

	epicsThreadSleep(0.5); /* temporary   xxx */
	
	fp = fopen(FILE_NAME_RAWDATA, "wb");
	if( fp != NULL ) {
		fwrite( &temp, 4, 1, fp);
		fclose(fp);
	}
	
	while(ptaskConfig) {
#if 0
		if(ptaskConfig->taskStatus & (TASK_INITIALIZED | TASK_ADC_STARTED)) {
			/*
			ellDelete(pdrvM6802Config->ptaskList, &ptaskConfig->node);
			*/
			if(ptaskConfig->taskStatus & TASK_ADC_STARTED) {
				ozStopTask(ptaskConfig);
				ptaskConfig->taskStatus &= ~TASK_ADC_STARTED;
				ptaskConfig->taskStatus |= TASK_ADC_STOPED;

				epicsPrintf("Exit Hook: Stoping Task %s\n", ptaskConfig->taskName);
			}
			ozClearTask(ptaskConfig);
			ptaskConfig->taskStatus &= ~TASK_INITIALIZED;
			ptaskConfig->taskStatus |= TASK_DESTROIED;

			epicsPrintf("Exit Hook: Clear Task %s\n", ptaskConfig->taskName);
				
			/*
			ellAdd(pdrvM6802Config->pdestroiedList, &ptaskConfig->node);
			*/
		}
#endif
		epicsPrintf("Exit Hook: Stoping Task %s\n", ptaskConfig->taskName);

		ozSetGlobalReset(ptaskConfig);

		ptaskConfig = (drvM6802_taskConfig*) ellNext(&ptaskConfig->node);
	}

#ifdef DEBUG
	epicsPrintf(" ok\n");
#endif

	return;
}

static void createTaskCallFunction(const iocshArgBuf *args)
{
	drvM6802_taskConfig *ptaskConfig = NULL;
	char task_name[60];
	uint16 vme_addr=0;


	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) vme_addr = strtoul(args[1].sval,NULL,0);


	if(make_drvConfig(vme_addr)) return;


	ptaskConfig = make_taskConfig(task_name, vme_addr);
	if(!ptaskConfig) return;


	ptaskConfig->pdrvConfig = pdrvM6802Config;
	
	ptaskConfig->pcontrolThreadConfig = make_controlThreadConfig(ptaskConfig);
	if(!ptaskConfig->pcontrolThreadConfig) return;

#if 1
	if( ptaskConfig->BoardID == 1 )
	{
		ptaskConfig->pfpdpThreadConfig = make_fpdpThreadConfig(ptaskConfig);
		if(!ptaskConfig->pfpdpThreadConfig) return;

		ptaskConfig->pringThreadConfig = make_ringThreadConfig(ptaskConfig);
		if(!ptaskConfig->pringThreadConfig) return;

	}
#endif


	ellAdd(pdrvM6802Config->ptaskList, &ptaskConfig->node);


#ifdef DEBUG
	epicsPrintf("createTaskCallFunction... ok\n");
#endif /* DEBUG */

	return;
}

static void createChannelCallFunction(const iocshArgBuf *args)
{
	drvM6802_taskConfig *ptaskConfig = NULL;
	drvM6802_channelConfig *pchannelConfig = NULL;
	char task_name[60];
	char channel_name[60];
	int channel_ID=0;

	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) channel_ID = strtoul(args[1].sval,NULL,0);
	if(args[2].sval) strcpy(channel_name, args[2].sval);

	ptaskConfig = find_taskConfig(task_name);
	if(!ptaskConfig) return;

	pchannelConfig = (drvM6802_channelConfig*) malloc(sizeof(drvM6802_channelConfig));
	if(!pchannelConfig) return;

	pchannelConfig->channelStatus = 0x0000;
	strcpy(pchannelConfig->chanName, channel_name);
	pchannelConfig->chanIndex = channel_ID;

	pchannelConfig->gain = INIT_GAIN;
	pchannelConfig->ptaskConfig = ptaskConfig;
	pchannelConfig->conversionTime_usec = 0.;

	ellAdd(ptaskConfig->pchannelConfig, &pchannelConfig->node);


#ifdef DEBUG
	epicsPrintf("createChannelCallFunction... ok\n");
#endif /* DEBUG */
	
	return;
}

LOCAL long drvM6802_io_report(int level)
{

	drvM6802_taskConfig *ptaskConfig;

	if(!pdrvM6802Config) return 0;

	if(ellCount(pdrvM6802Config->ptaskList))
		ptaskConfig = (drvM6802_taskConfig*) ellFirst (pdrvM6802Config->ptaskList);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pdrvM6802Config->ptaskList));

	if(level<1) return 0;

	while(ptaskConfig) {
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x, connected channels: %d\n",
			    ptaskConfig->taskName, 
			    ptaskConfig->vme_addr, 
			    ptaskConfig->taskStatus,
			    ellCount(ptaskConfig->pchannelConfig));
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", ptaskConfig->samplingRate );
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
			epicsPrintf("   callback time: %fusec\n", ptaskConfig->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", ptaskConfig->adjCount_smplRate,
					                                                 ptaskConfig->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", ptaskConfig->adjCount_Gain,
					                                                   ptaskConfig->adjTime_Gain_Usec);
		}


/*		if(ellCount(ptaskConfig->pchannelConfig)>0) print_channelConfig(ptaskConfig,level); */
		ptaskConfig = (drvM6802_taskConfig*) ellNext(&ptaskConfig->node);
	}

	return 0;
}


drvM6802_taskConfig* drvM6802_find_taskConfig(char *taskName)
{
	return find_taskConfig(taskName);
}
/*
drvM6802_channelConfig* drvM6802_find_channelConfig(char *taskName, char *chanName)
{
	drvM6802_taskConfig *ptaskConfig = find_taskConfig(taskName);
	if(!ptaskConfig) return NULL;

	return find_channelConfig(ptaskConfig, chanName);
}
*/
/*
drvM6802_channelConfig* drvM6802_find_channelConfig_fromID(char *taskName, int ch)
{
	drvM6802_taskConfig *ptaskConfig = find_taskConfig(taskName);
	if(!ptaskConfig) return NULL;

	return find_channelConfig_fromID(ptaskConfig, ch);
}
*/
/*
void drvM6802_set_chanGain(drvM6802_taskConfig *ptaskConfig, int channel, int gain)
{
	set_chanGain(ptaskConfig, channel, gain);
	return;
}
*/
int drvM6802_set_zeroCalibration(drvM6802_taskConfig *ptaskConfig,uint16 val)
{
	set_ZeroCalibration(ptaskConfig, val);
	return OK;
}
void drvM6802_set_samplingRate(uint16 samplingRate)
{
	drvM6802_taskConfig *ptaskConfig = (drvM6802_taskConfig*) ellFirst(pdrvM6802Config->ptaskList);
	while(ptaskConfig) 
	{
		if(ptaskConfig->taskStatus & TASK_INITIALIZED) {
			set_samplingRate(ptaskConfig, samplingRate);
		}
		ptaskConfig = (drvM6802_taskConfig*) ellNext(&ptaskConfig->node);
	} /* 	while(ptaskConfig)  */

	return;
}

int drvM6802_set_DAQready(drvM6802_taskConfig *ptaskConfig)
{

/*	ozSetTriggerReset(ptaskConfig);
	epicsThreadSleep(0.1);
	ozSetTriggerRelease(ptaskConfig);
*/

	fpRaw = fopen(FILE_NAME_RAWDATA, "wb");
	if( fpRaw == NULL ) {
		printf(" can't open temp.dat \n");
		return ERROR;
	}
/*	fcntl(fileno(fpRaw), F_SETFD, O_NONBLOCK); */
/*
#else
	fd = open(FILE_NAME_RAWDATA, O_WRONLY|O_CREAT);
	if (fd < 0) {
		printf(" can't open temp.dat \n");
		return ERROR;
	}
*/

	if( !startFpdp() ) {
		printf(" can't start FPDP \n");
		terminate();
		return ERROR;
	}

/*
	if(!ozStartADC( ptaskConfig ) )
		return ERROR;
*/

	return OK;
}

int drvM6802_set_DAQstart(drvM6802_taskConfig *ptaskConfig)
{
	drvM6802_taskConfig *ptaskAll = (drvM6802_taskConfig*) ellFirst(pdrvM6802Config->ptaskList);
	
	if( ptaskConfig->taskStatus & TASK_ADC_STARTED ) {
		epicsPrintf("\n>>> ozStartADC : Already Started!\n");
		return ERROR;
	}
	ptaskConfig->cnt_DMAcallback=0;
	dmaToggle = 1;

	while(ptaskAll) 
	{	
		ozSetArmOut(ptaskAll);
/*		epicsThreadSleep(0.1); */
		ptaskAll = (drvM6802_taskConfig*) ellNext(&ptaskAll->node);
	}
	
/*
	epicsPrintf("\n>>> startAcqProcess : taskStatus 0x%X\n", ptaskConfig->taskStatus);
*/

	ozSetTriggerRelease(ptaskConfig);


/*	epicsThreadSleep(0.1); */

	if( ptaskConfig->taskStatus & TASK_INITIALIZED )
	{
		if ( !ozSetADsFIFOrun( ptaskConfig) ) {
			epicsPrintf("\n>>> ozStartADC : ozSetADsFIFOrun...failed! \n");
			return ERROR;
		}
		ptaskConfig->taskStatus &= ~TASK_ADC_STOPED;
		ptaskConfig->taskStatus |= TASK_ADC_STARTED;
	} else {
		epicsPrintf("\n>>> ozStartADC : not ready to run! \n");
		return ERROR;
	}


#if 0    /* TG.Lee Enable 20081111 */
	epicsPrintf("set DaqStart Func Time Register Address TG.Lee\n");
	epicsPrintf("ozWriteRegister(MM_CNTRL_REG_1): 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_1] );
	epicsPrintf("ozWriteRegister(MM_CNTRL_REG_2): 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_2] );
				epicsPrintf(">>> MM_PGA_ARRAY_1 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_1] );
				epicsPrintf(">>> MM_PGA_ARRAY_2 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_2] );
				epicsPrintf(">>> MM_PGA_ARRAY_3 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_3] );
				epicsPrintf(">>> MM_PGA_ARRAY_4 : 0x%X\n", ptaskConfig->register_m6802[MM_PGA_ARRAY_4] );
				epicsPrintf(">>> MM_ADC_HPFE : 0x%X\n", ptaskConfig->register_m6802[MM_ADC_HPFE] );
				epicsPrintf(">>> MM_ADC_ZCAL : 0x%X\n", ptaskConfig->register_m6802[MM_ADC_ZCAL] );
#endif


	return OK;
}

int drvM6802_set_DAQstop(drvM6802_taskConfig *ptaskConfig)
{
	drvBufferConfig_Node *pbufferNode = NULL;
	bufferingThreadQueueData queueData;
	drvM6802_controlThreadConfig *pfpdpThreadConfig = (drvM6802_controlThreadConfig*) ptaskConfig->pfpdpThreadConfig;

	if ( !ozSetADsFIFOreset( ptaskConfig) ) {
		epicsPrintf("\n>>> ozStopADC : ozSetADsFIFOreset...failed! \n");
		return ERROR;
	}
/*	epicsThreadSleep(0.1); */

	ozSetTriggerReset(ptaskConfig); 
	
	if( stopFpdp() != OK ) printf(" can't stop FPDP \n");


/*	epicsThreadSleep(0.1); */
	
	pbufferNode = (drvBufferConfig_Node *)ellFirst(pdrvBufferConfig->pbufferList);
	ellDelete(pdrvBufferConfig->pbufferList, &pbufferNode->node); 
	pbufferNode->nCnt = ptaskConfig->cnt_DMAcallback;
	pbufferNode->nStop = 1;

	printf("\nFPDP >>> DMA done count= %d\n", ptaskConfig->cnt_DMAcallback);
	epicsPrintf("DAQStop after :ellCnt %d \n", ellCount(pdrvBufferConfig->pbufferList) ); 
	queueData.pNode = pbufferNode;

	epicsMessageQueueSend(pfpdpThreadConfig->threadQueueId, (void*) &queueData, sizeof(bufferingThreadQueueData));
/*	epicsPrintf("\n>>>Last snd ID: %d,  DMA cnt: %d\n", (int)pfpdpThreadConfig->threadQueueId, pbufferNode->nCnt); */

	return OK;
}

int drvM6802_set_DAQclear(drvM6802_taskConfig *ptaskConfig)
{
	
	drvM6802_taskConfig *ptaskAll = NULL;

	if( (ptaskConfig->taskStatus & TASK_ADC_STARTED)  )
	{

		ptaskAll = (drvM6802_taskConfig*) ellFirst(pdrvM6802Config->ptaskList);
		while(ptaskAll) 
		{
			ozSetArmReset(ptaskAll);
/*			epicsThreadSleep(0.1); */
			ptaskAll = (drvM6802_taskConfig*) ellNext(&ptaskAll->node);
		}
	
		ptaskConfig->taskStatus &= ~TASK_ADC_STARTED;
		ptaskConfig->taskStatus |= TASK_ADC_STOPED;

	} else {
		epicsPrintf("\n>>> ozStopADC : DAQ not started! \n");
		return ERROR;
	}

	return OK;
}

void drvM6802_set_channelMask(drvM6802_taskConfig *ptaskConfig, int cnt)
{
/*	ptaskConfig->channelMask = cnt;
	ozSetChannelMask(ptaskConfig);
*/

	if(!pdrvM6802Config)  return;
	drvM6802_taskConfig *ptaskConfig1 = NULL;
	ptaskConfig1 = (drvM6802_taskConfig*) ellFirst(pdrvM6802Config->ptaskList);
	while(ptaskConfig1) 
	{
		if(ellCount(ptaskConfig1->pchannelConfig))
		{
			ptaskConfig1->channelMask = cnt;
			ozSetChannelMask(ptaskConfig1);
		} 
		ptaskConfig1 = (drvM6802_taskConfig*) ellNext(&ptaskConfig1->node);
	} /* 	while(ptaskConfig1)  */

}


/* val == 0 internal */
/* val == 1 external */
int drvM6802_set_clockSource(drvM6802_taskConfig *ptaskConfig, uint16 val)
{
	if ( val != 0 && val != 1 ) {
		epicsPrintf("\n>>> drvM6802_set_clockSource : wrong input value: %d, must '0=internal' or '1=external'\n", val);
		return ERROR;
	}
	ptaskConfig->clockSource = val;
	if( !ozSetClockInterExter(ptaskConfig) ) {
		epicsPrintf("\n>>> drvM6802_set_clockSource : ozSetClockInterExter...failed! \n");
		return ERROR;
	}
	if( val == 1 ) {
/*		ozSetTriggerExternal(ptaskConfig); */
/*		ozSetSamplingRate(ptaskConfig, 200); */
/* TG.Lee Delete 08.1121		drvM6802_set_samplingRate(200);  Quad mode, div 64 */
	} else {
/*		ozSetTriggerInternal (ptaskConfig); */
/*		ozSetSamplingRate(ptaskConfig, 100); */
/*TG.Lee Delete 08.1121 		drvM6802_set_samplingRate(100);   */
	}
/*
	epicsPrintf(">>> MM_CNTRL_REG_1 : 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_1] );
	epicsPrintf(">>> MM_CNTRL_REG_2 : 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_2] );
*/

	return OK;
}

/* val == 0 internal */
/* val == 1 external */
int drvM6802_set_triggerSource(drvM6802_taskConfig *ptaskConfig, uint16 val)
{
	int rtn;
	if ( val != 0 && val != 1 ) {
		epicsPrintf("\n>>> drvM6802_set_triggerSource : wrong input value: %d, must '0=internal' or '1=external'\n", val);
		return ERROR;
	}
	rtn = (val == 1) ? ozSetTriggerExternal(ptaskConfig) : ozSetTriggerInternal (ptaskConfig) ;
	return rtn;
}


