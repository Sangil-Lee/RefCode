#define DEBUG_

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

#include <NIDAQmx.h>
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
#include "drvPXISCXI.h"


LOCAL drvPXISCXIConfig          *pdrvPXISCXIConfig = NULL;


/* test function */
unsigned long long int drvPXISCXI_rdtsc(void)
{
	unsigned long long int x;

	__asm__ volatile(".byte 0x0f, 0x31": "=A"(x));

	return x;
}

double drvPXISCXI_intervalUSec(unsigned long long int start, unsigned long long int stop)
{
	return 1.E+6 * (double)(stop-start)/(double)(pdrvPXISCXIConfig->one_sec_interval);
}


static bridgeConfig pbridgeConfig[] = { {BRIDGE_FULLBRIDGE_I, DAQmx_Val_FullBridgeI},
					{BRIDGE_FULLBRIDGE_II, DAQmx_Val_FullBridgeII},
					{BRIDGE_FULLBRIDGE_III, DAQmx_Val_FullBridgeIII},
					{BRIDGE_HALFBRIDGE_I, DAQmx_Val_HalfBridgeI},
					{BRIDGE_HALFBRIDGE_II, DAQmx_Val_HalfBridgeII},
					{BRIDGE_QUARTERBRIDGE_I, DAQmx_Val_QuarterBridgeI},
					{BRIDGE_QUARTERBRIDGE_II, DAQmx_Val_QuarterBridgeII},
					{NULL,                    -1}};



static void init_task(drvPXISCXI_taskConfig *ptaskConfig); 
static void init_taskTiming(drvPXISCXI_taskConfig *ptaskConfig);
static void init_taskCallback(drvPXISCXI_taskConfig *ptaskConfig);
static void init_dataPool(drvPXISCXI_taskConfig *ptaskConfig);
static void start_task(drvPXISCXI_taskConfig *ptaskConfig); 
static void stop_task(drvPXISCXI_taskConfig *ptaskConfig);
static void init_cernoxChannel(drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig);
static void init_strainChannel(drvPXISCXI_strainChannelConfig *pstrainChannelConfig);
static void init_hallChannel(drvPXISCXI_hallChannelConfig *phallChannelConfig);
static void set_cernoxChanGain(drvPXISCXI_taskConfig *ptaskConfig, float64 gain);
static void set_cernoxChanGainALLTasks(float64 gain);
static void set_lowpassFilter(drvPXISCXI_taskConfig *ptaskConfig, float64 cutOffFreq);
static void set_lowpassFilterALLTasks(float64 cutOffFreq);
static void set_samplingRate(drvPXISCXI_taskConfig *ptaskConfig, float64 samplingRate);
static void set_samplingRateALLTasks(float64 samplingRate);
static int32 find_bridgeType(char *bridgeStr);


static int make_drvConfig(void);
static drvPXISCXI_taskConfig* make_taskConfig(char *taskName);
static drvPXISCXI_taskConfig* find_taskConfig(char *taskName);
static drvPXISCXI_channelConfig* find_channelConfig(drvPXISCXI_taskConfig *ptaskConfig, char *chanName);
static void print_channelConfig(drvPXISCXI_taskConfig *ptaskConfig, int level);
static cernoxDataConfig* load_cernoxDataOnebyOne(FILE *file);
static cernoxDataConfig *find_cernoxDataConfig(char *serial);

static drvPXISCXI_controlThreadConfig* make_controlThreadConfig(drvPXISCXI_taskConfig *ptaskConfig);

static int32 CVICALLBACK taskCallback(TaskHandle taskHandle, int32 everyNsampleEventType, uInt32 nSamples, void *callbackData);

static void controlThread(void *param);


static void clearAllDAQmxTasks(void);

static void cernoxDataCallFunction(const iocshArgBuf *args);
static void cernoxListCallFunction(const iocshArgBuf *args);
static void loadCernoxListCallFunction(const iocshArgBuf *args);
static void createPXITaskCallFunction(const iocshArgBuf *args);
static void createPXICernoxChannelCallFunction(const iocshArgBuf *args);
static void createPXIStrainChannelCallFunction(const iocshArgBuf *args);
static void createPXIHallChannelCallFunction(const iocshArgBuf *args);


static const iocshArg cernoxDataArg0 = {"serial", iocshArgString};
static const iocshArg* cernoxDataArgs[] = { &cernoxDataArg0 };
static const iocshFuncDef cernoxDataFuncDef = { "cernoxData",
						1,
						cernoxDataArgs };

static const iocshArg cernoxListArg0 = {"level", iocshArgInt};
static const iocshArg* cernoxListArgs[] = { &cernoxListArg0 };
static const iocshFuncDef cernoxListFuncDef = { "cernoxList",
						1,
						cernoxListArgs };

static const iocshArg loadCernoxListArg0 = {"cernox list file", iocshArgString};
static const iocshArg* loadCernoxListArgs[] = { &loadCernoxListArg0 };
static const iocshFuncDef loadCernoxListFuncDef = { "loadCernoxList",
						    1,
						    loadCernoxListArgs };

static const iocshArg createPXITaskArg0 = {"task name", iocshArgString};
static const iocshArg* const createPXITaskArgs[] = { &createPXITaskArg0 };
static const iocshFuncDef createPXITaskFuncDef = { "createPXITask",
 						   1,
					           createPXITaskArgs };

static const iocshArg createPXICernoxChannelArg0 = {"task name", iocshArgString};
static const iocshArg createPXICernoxChannelArg1 = {"channel name", iocshArgString};
static const iocshArg* const createPXICernoxChannelArgs[] = { &createPXICernoxChannelArg0,
	                                                      &createPXICernoxChannelArg1 };
static const iocshFuncDef createPXICernoxChannelFuncDef = {"createPXICernoxChannel",
							   2,
							   createPXICernoxChannelArgs};

static const iocshArg createPXIStrainChannelArg0 = {"task name", iocshArgString};
static const iocshArg createPXIStrainChannelArg1 = {"channel name", iocshArgString};
static const iocshArg createPXIStrainChannelArg2 = {"bridge type", iocshArgString};
static const iocshArg createPXIStrainChannelArg3 = {"min value", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg4 = {"max value", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg5 = {"excitation voltage", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg6 = {"gauge factor", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg7 = {"poisson rate", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg8 = {"initial bridge voltage", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg9 = {"nominal gauge resistance", iocshArgDouble};
static const iocshArg createPXIStrainChannelArg10 = {"lead wire resistnace", iocshArgDouble};
static const iocshArg* const createPXIStrainChannelArgs[] = { &createPXIStrainChannelArg0,
	                                                      &createPXIStrainChannelArg1,
							      &createPXIStrainChannelArg2,
							      &createPXIStrainChannelArg3,
							      &createPXIStrainChannelArg4,
                                                              &createPXIStrainChannelArg5,
                                                              &createPXIStrainChannelArg6,
                                                              &createPXIStrainChannelArg7,
                                                              &createPXIStrainChannelArg8,
                                                              &createPXIStrainChannelArg9,
                                                              &createPXIStrainChannelArg10 };
static const iocshFuncDef createPXIStrainChannelFuncDef = {"createPXIStrainChannel",
	                                                   11,
							   createPXIStrainChannelArgs};

static const iocshArg createPXIHallChannelArg0 = {"task name", iocshArgString};
static const iocshArg createPXIHallChannelArg1 = {"channel name", iocshArgString};
static const iocshArg* const createPXIHallChannelArgs[] = { &createPXIHallChannelArg0,
	                                                    &createPXIHallChannelArg1};
static const iocshFuncDef createPXIHallChannelFuncDef = {"createPXIHallChannel",
						   	 2,
							 createPXIHallChannelArgs};



static void epicsShareAPI drvPXISCXIRegistrar(void)
{
    static int firstTime =1;

    if(firstTime) {
        firstTime = 0;
	iocshRegister(&cernoxDataFuncDef, cernoxDataCallFunction);
	iocshRegister(&cernoxListFuncDef, cernoxListCallFunction);
	iocshRegister(&loadCernoxListFuncDef, loadCernoxListCallFunction);
        iocshRegister(&createPXITaskFuncDef, createPXITaskCallFunction);
	iocshRegister(&createPXICernoxChannelFuncDef, createPXICernoxChannelCallFunction);
	iocshRegister(&createPXIStrainChannelFuncDef, createPXIStrainChannelCallFunction);
	iocshRegister(&createPXIHallChannelFuncDef, createPXIHallChannelCallFunction);
    }
}

epicsExportRegistrar(drvPXISCXIRegistrar);




LOCAL long drvPXISCXI_io_report(int level);
LOCAL long drvPXISCXI_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvPXISCXI = {
       2,
       drvPXISCXI_io_report,
       drvPXISCXI_init_driver
};

epicsExportAddress(drvet, drvPXISCXI);


static void init_task(drvPXISCXI_taskConfig *ptaskConfig)
{
	int32 error =0;
	char errBuff[2048] = {'\0' };

	DAQmxErrChk(DAQmxCreateTask(ptaskConfig->taskName, &(ptaskConfig->taskHandle)));
	ptaskConfig->taskStatus |= TASK_INITIALIZED;

#ifdef DEBUG
        epicsPrintf("init_task: %s %d\n", ptaskConfig->taskName, (unsigned) ptaskConfig->taskHandle);
#endif /* DEBUG */
	
	return;

Error:
        if( DAQmxFailed(error) )
                DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
               DAQmxStopTask(ptaskConfig->taskHandle);
               DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
               epicsPrintf("DAQmx Error: %s\n",errBuff);
        return;

}

static void init_taskTiming(drvPXISCXI_taskConfig *ptaskConfig)
{
	int32 error = 0;
	char errBuff[2048] = { '\0'};

#ifdef DEBUG
	epicsPrintf("init_taskTiming: %s\n", ptaskConfig->taskName);
#endif /* DEBUG */

	DAQmxErrChk(DAQmxCfgSampClkTiming((TaskHandle) ptaskConfig->taskHandle,
				          (const char*) NULL,
					  (float64) ptaskConfig->samplingRate,
					  (int32) DAQmx_Val_Rising,
					  (int32) DAQmx_Val_ContSamps,
					  (uInt64) ellCount(ptaskConfig->pchannelConfig)));
	ptaskConfig->taskStatus |= TASK_TIMING;

	return;
Error:
        if( DAQmxFailed(error) )
                DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
                DAQmxStopTask(ptaskConfig->taskHandle);
                DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
                epicsPrintf("DAQmx Error: %s\n",errBuff);
        return;
}


static void init_taskCallback(drvPXISCXI_taskConfig *ptaskConfig)
{
	int32 error = 0;
	char errBuff[2048] = {'\0'};


	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(ptaskConfig->taskHandle,
						    DAQmx_Val_Acquired_Into_Buffer,
					    	    1,
					 	    0,
						    (DAQmxEveryNSamplesEventCallbackPtr) ptaskConfig->cviCallback,
						    (void *) ptaskConfig));
	ptaskConfig->taskStatus |= TASK_CALLBACK;

	return;

Error:
        if( DAQmxFailed(error) )
                DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
                DAQmxStopTask(ptaskConfig->taskHandle);
                DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
                epicsPrintf("DAQmx Error: %s\n",errBuff);
        return;
}

static void init_dataPool(drvPXISCXI_taskConfig *ptaskConfig)
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


static void start_task(drvPXISCXI_taskConfig *ptaskConfig)
{
	int32 error = 0;
	char errBuff[2048] = { '\0' };

	if((ptaskConfig->taskStatus & TASK_INITIALIZED) && !(ptaskConfig->taskStatus & TASK_STARTED)) {
		DAQmxErrChk(DAQmxStartTask(ptaskConfig->taskHandle));
	}
	else {
		epicsPrintf("fail to start task %s\n", ptaskConfig->taskName);
		return;
	}

	ptaskConfig->taskStatus &= ~TASK_STOPED;
	ptaskConfig->taskStatus |= TASK_STARTED;


	return;

Error:
        if( DAQmxFailed(error) )
              DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
               DAQmxStopTask(ptaskConfig->taskHandle);
               DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
               epicsPrintf("DAQmx Error: %s\n",errBuff);
        return;

}

static void stop_task(drvPXISCXI_taskConfig *ptaskConfig)
{
	int32 error = 0;
	char errBuff[2048] = { '\0' };

	if((ptaskConfig->taskStatus & TASK_INITIALIZED) && !(ptaskConfig->taskStatus & TASK_STOPED)) {
		DAQmxErrChk(DAQmxStopTask(ptaskConfig->taskHandle));
	}
	else {
		epicsPrintf("fail to stop task %s\n", ptaskConfig->taskName);
		return;
	}

	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;

	return;


Error:
        if( DAQmxFailed(error) )
              DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
               DAQmxStopTask(ptaskConfig->taskHandle);
               DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
               epicsPrintf("DAQmx Error: %s\n",errBuff);
        return;

}

static void init_cernoxChannel(drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig)
{
	drvPXISCXI_taskConfig *ptaskConfig = pcernoxChannelConfig->ptaskConfig;
	int32 error = 0;
	char errBuff[2048] = {'\0'};

	if(pcernoxChannelConfig->channelStatus & CHANNEL_INITIALIZED) return;
#ifdef DEBUG
	epicsPrintf("init_cernoxChannel: task: %s %d, channel: %s\n", 
		    ptaskConfig->taskName, (unsigned) ptaskConfig->taskHandle, pcernoxChannelConfig->chanName);
#endif
	

	DAQmxErrChk(DAQmxCreateAIVoltageChan(ptaskConfig->taskHandle,
					     pcernoxChannelConfig->chanName,
					     NULL,
					     DAQmx_Val_Diff,
					     pcernoxChannelConfig->minRange,
					     pcernoxChannelConfig->maxRange,
					     DAQmx_Val_Volts,
					     NULL));
	
	DAQmxErrChk(DAQmxSetAIGain(ptaskConfig->taskHandle,
				   pcernoxChannelConfig->chanName,
				   pcernoxChannelConfig->gain));
	DAQmxErrChk(DAQmxGetAIGain(ptaskConfig->taskHandle,
				   pcernoxChannelConfig->chanName,
				   &pcernoxChannelConfig->gain));

	DAQmxErrChk(DAQmxSetAILowpassEnable(ptaskConfig->taskHandle,
                                            pcernoxChannelConfig->chanName,
                                            1));
        DAQmxErrChk(DAQmxSetAILowpassCutoffFreq(ptaskConfig->taskHandle,
                                                pcernoxChannelConfig->chanName,
                                                pcernoxChannelConfig->cutOffFreq));
        DAQmxErrChk(DAQmxGetAILowpassCutoffFreq(ptaskConfig->taskHandle,
                                                pcernoxChannelConfig->chanName,
                                                &pcernoxChannelConfig->cutOffFreq));

	

#ifdef DEBUG	
	epicsPrintf("init_cernox: %s %d\n", pcernoxChannelConfig->chanName, (unsigned) error);
#endif

	pcernoxChannelConfig->channelStatus |= CHANNEL_INITIALIZED;
	return;

Error:
        if( DAQmxFailed(error) )
              DAQmxGetExtendedErrorInfo(errBuff,2048);

        if( ptaskConfig->taskHandle!=0 ) {
              DAQmxStopTask(ptaskConfig->taskHandle);
              DAQmxClearTask(ptaskConfig->taskHandle);
        }
        if( DAQmxFailed(error) )
              epicsPrintf("DAQmx Error: %s\n",errBuff);
        return;
}


static void init_strainChannel(drvPXISCXI_strainChannelConfig *pstrainChannelConfig)
{
	drvPXISCXI_taskConfig *ptaskConfig = pstrainChannelConfig->ptaskConfig;
	int32 error = 0;
	char errBuff[2048] = {'\0'};

	if(pstrainChannelConfig->channelStatus & CHANNEL_INITIALIZED) return;

#ifdef DEBUG
	epicsPrintf("init_strainChannel: task: %s %d, channel: %s\n",
		    ptaskConfig->taskName, (unsigned) ptaskConfig->taskHandle, pstrainChannelConfig->chanName);
#endif

	DAQmxErrChk(DAQmxCreateAIStrainGageChan(ptaskConfig->taskHandle,
				                pstrainChannelConfig->chanName,
						NULL,
						pstrainChannelConfig->minRange,
						pstrainChannelConfig->maxRange,
						pstrainChannelConfig->units,
						pstrainChannelConfig->bridgeType,
						pstrainChannelConfig->excitationSource,
						pstrainChannelConfig->excitationVoltage,
						pstrainChannelConfig->gageFactor,
						pstrainChannelConfig->initialBridgeVoltage,
						pstrainChannelConfig->nominalGageResistance,
						pstrainChannelConfig->poissonRate,
						pstrainChannelConfig->leadWireResistance,
						NULL));
	DAQmxErrChk(DAQmxGetAIGain(ptaskConfig->taskHandle,
				   pstrainChannelConfig->chanName,
				   &pstrainChannelConfig->gain));


	pstrainChannelConfig->channelStatus |= CHANNEL_INITIALIZED;
	return;

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

	if( ptaskConfig->taskHandle != 0 ) {
 		DAQmxStopTask(ptaskConfig->taskHandle);
		DAQmxClearTask(ptaskConfig->taskHandle);
	}

	if( DAQmxFailed(error))
		epicsPrintf("DAQmx Error: %s\n", errBuff);

	return;
}

/*
  Init  Hall channel 20070110
 */
static void init_hallChannel(drvPXISCXI_hallChannelConfig *phallChannelConfig)
{
	drvPXISCXI_taskConfig *ptaskConfig = phallChannelConfig->ptaskConfig;
	int32 error = 0;
	char errBuff[2048] = {'\0'};

	if(phallChannelConfig->channelStatus & CHANNEL_INITIALIZED) return;
#ifdef DEBUG
	epicsPrintf("init_hallChannel: task: %s %d, channel: %s\n",
		     ptaskConfig->taskName, (unsigned) ptaskConfig->taskHandle, phallChannelConfig->chanName);
#endif


	DAQmxErrChk(DAQmxCreateAIVoltageChan(ptaskConfig->taskHandle,
					     phallChannelConfig->chanName,
					     NULL,
					     DAQmx_Val_Diff,
					     phallChannelConfig->minRange,
					     phallChannelConfig->maxRange,
					     DAQmx_Val_Volts,
					     NULL));
	DAQmxErrChk(DAQmxGetAIGain(ptaskConfig->taskHandle,
				   phallChannelConfig->chanName,
				   &phallChannelConfig->gain));
#ifdef DEBUG
	epicsPrintf("init_hall: %s %d\n", phallChannelConfig->chanName, (unsigned) error);
#endif
	phallChannelConfig->channelStatus |= CHANNEL_INITIALIZED;
	return;
Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);

	if( ptaskConfig->taskHandle!=0 ) { 
		DAQmxStopTask(ptaskConfig->taskHandle);
		DAQmxClearTask(ptaskConfig->taskHandle);
	}
	if( DAQmxFailed(error) )
		epicsPrintf("DAQmx Error: %s\n",errBuff);
	return;
}


static void set_cernoxChanGain(drvPXISCXI_taskConfig *ptaskConfig, float64 gain)
{
	drvPXISCXI_channelConfig *pchannelConfig;
	drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig;

	unsigned long long int start, stop;

	int32 error = 0;
	char errBuff[2048] = { '\0' };


	if(!ptaskConfig) return;

	start = drvPXISCXI_rdtsc();

	DAQmxErrChk(DAQmxStopTask(ptaskConfig->taskHandle));
	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;

	pchannelConfig = (drvPXISCXI_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		if(pchannelConfig->channelType == CERNOX_CHANNEL) {
			pcernoxChannelConfig = (drvPXISCXI_cernoxChannelConfig*) pchannelConfig;
			pcernoxChannelConfig->gain = gain;

			DAQmxErrChk(DAQmxSetAIGain(ptaskConfig->taskHandle,
						   pcernoxChannelConfig->chanName,
						   pcernoxChannelConfig->gain));
			DAQmxErrChk(DAQmxGetAIGain(ptaskConfig->taskHandle,
						   pcernoxChannelConfig->chanName,
						   &pcernoxChannelConfig->gain));

		}
		pchannelConfig = (drvPXISCXI_channelConfig*) ellNext(&pchannelConfig->node);
	}

	DAQmxErrChk(DAQmxStartTask(ptaskConfig->taskHandle));
	ptaskConfig->taskStatus &= ~TASK_STOPED;
	ptaskConfig->taskStatus |= TASK_STARTED;

	stop = drvPXISCXI_rdtsc();
	ptaskConfig->adjTime_cernoxGain_Usec = drvPXISCXI_intervalUSec(start, stop);
	ptaskConfig->adjCount_cernoxGain++;


	return;

Error:
	if(DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

	if(ptaskConfig->taskHandle !=0) {
		DAQmxStopTask(ptaskConfig->taskHandle);
		DAQmxClearTask(ptaskConfig->taskHandle);
	}

	if(DAQmxFailed(error))
		epicsPrintf("DAQmx Error: %s\n", errBuff);
	return;
}

static void set_cernoxChanGainALLTasks(float64 gain)
{
	drvPXISCXI_taskConfig *ptaskConfig = (drvPXISCXI_taskConfig*) ellFirst (pdrvPXISCXIConfig->ptaskList);

	while(ptaskConfig) {
		if(ptaskConfig->taskStatus & TASK_INITIALIZED) {
			set_cernoxChanGain(ptaskConfig, gain);
		}
		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}

	return;
}

static void set_lowpassFilter(drvPXISCXI_taskConfig *ptaskConfig, float64 cutOffFreq)
{
	drvPXISCXI_channelConfig *pchannelConfig;

	unsigned long long int start, stop;

	int32 error = 0;
	char errBuff[2048] = { '\0' };

	if(!ptaskConfig) return;

	start = drvPXISCXI_rdtsc();

	DAQmxErrChk(DAQmxStopTask(ptaskConfig->taskHandle));
	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;

	pchannelConfig = (drvPXISCXI_channelConfig*) ellFirst (ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		pchannelConfig->cutOffFreq = cutOffFreq;
		
		DAQmxErrChk(DAQmxSetAILowpassEnable(ptaskConfig->taskHandle,
					            pchannelConfig->chanName,
						    1));
		DAQmxErrChk(DAQmxSetAILowpassCutoffFreq(ptaskConfig->taskHandle,
					                pchannelConfig->chanName,
							pchannelConfig->cutOffFreq));
		DAQmxErrChk(DAQmxGetAILowpassCutoffFreq(ptaskConfig->taskHandle,
					                pchannelConfig->chanName,
							&pchannelConfig->cutOffFreq));
							
		pchannelConfig = (drvPXISCXI_channelConfig*) ellNext (&pchannelConfig->node);
	}

	DAQmxErrChk(DAQmxStartTask(ptaskConfig->taskHandle));
	ptaskConfig->taskStatus &= ~TASK_STOPED;
	ptaskConfig->taskStatus |= TASK_STARTED;

	stop = drvPXISCXI_rdtsc();
	ptaskConfig->adjTime_lowPass_Usec = drvPXISCXI_intervalUSec(start, stop);
	ptaskConfig->adjCount_lowPass++;

	return;


Error:
	if(DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if(ptaskConfig->taskHandle !=0) {
		DAQmxStopTask(ptaskConfig->taskHandle);
		DAQmxClearTask(ptaskConfig->taskHandle);
	}

	if(DAQmxFailed(error))
		epicsPrintf("DAQmx Error: %s\n", errBuff);
	return;
}

static void set_lowpassFilterALLTasks(float64 cutOffFreq)
{
	drvPXISCXI_taskConfig *ptaskConfig 
		= (drvPXISCXI_taskConfig *) ellFirst (pdrvPXISCXIConfig->ptaskList);
	while(ptaskConfig) {
		if(ptaskConfig->taskStatus & TASK_INITIALIZED) {
			set_lowpassFilter(ptaskConfig, cutOffFreq);
		}
		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}
}

static void set_samplingRate(drvPXISCXI_taskConfig *ptaskConfig, float64 samplingRate)
{
	unsigned long long int start, stop;

	int32 error = 0;
	char errBuff[2048] = { '\0' };

	start = drvPXISCXI_rdtsc();

	DAQmxErrChk(DAQmxStopTask(ptaskConfig->taskHandle));
	ptaskConfig->taskStatus &= ~TASK_STARTED;
	ptaskConfig->taskStatus |= TASK_STOPED;

	ptaskConfig->samplingRate = samplingRate;

	DAQmxErrChk(DAQmxCfgSampClkTiming(ptaskConfig->taskHandle,
				          NULL,
					  ptaskConfig->samplingRate,
					  DAQmx_Val_Rising,
					  DAQmx_Val_ContSamps,
					  (uInt64) ellCount(ptaskConfig->pchannelConfig)));

	DAQmxErrChk(DAQmxStartTask(ptaskConfig->taskHandle));
	ptaskConfig->taskStatus &= ~TASK_STOPED;
	ptaskConfig->taskStatus |= TASK_STARTED;

	stop = drvPXISCXI_rdtsc();
	ptaskConfig->adjTime_smplRate_Usec = drvPXISCXI_intervalUSec(start, stop);
	ptaskConfig->adjCount_smplRate++;

	return;


Error:
	if(DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if(ptaskConfig->taskHandle != 0) {
		DAQmxStopTask(ptaskConfig->taskHandle);
		DAQmxClearTask(ptaskConfig->taskHandle);
	}

	if(DAQmxFailed(error))
		epicsPrintf("DAQmx Error: %s\n", errBuff);
	return;
}

static void set_samplingRateALLTasks(float64 smaplingRate)
{
	drvPXISCXI_taskConfig *ptaskConfig = (drvPXISCXI_taskConfig*) ellFirst(pdrvPXISCXIConfig->ptaskList);
	
	while(ptaskConfig) {
		if(ptaskConfig->taskStatus & TASK_INITIALIZED) {
			set_samplingRate(ptaskConfig, smaplingRate);
		}
		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}
}

static int32 find_bridgeType(char *bridgeStr)
{
	int i=0;

	while((pbridgeConfig+i)->bridgeStr) {
		if(!strcmp((pbridgeConfig+i)->bridgeStr, bridgeStr)) break;
		i++;
	}

#ifdef DEBUG
	epicsPrintf("find_bridgeType: %s (index: %d)\n",
		    bridgeStr, i);
#endif

	return (pbridgeConfig+i)->bridgeType;

}

	


static int make_drvConfig(void)
{
	if(pdrvPXISCXIConfig) return 0;

	pdrvPXISCXIConfig = (drvPXISCXIConfig*) malloc(sizeof(drvPXISCXIConfig));
	if(!pdrvPXISCXIConfig) return 1;

	pdrvPXISCXIConfig->ptaskList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvPXISCXIConfig->ptaskList) return 1;
	else ellInit(pdrvPXISCXIConfig->ptaskList);

	pdrvPXISCXIConfig->pdestroiedList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvPXISCXIConfig->pdestroiedList) return 1;
	else ellInit(pdrvPXISCXIConfig->pdestroiedList);

	pdrvPXISCXIConfig->pcernoxDataList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvPXISCXIConfig->pcernoxDataList) return 1;
	else ellInit(pdrvPXISCXIConfig->pcernoxDataList);

	return 0;
}

static drvPXISCXI_taskConfig* make_taskConfig(char *taskName)
{
	drvPXISCXI_taskConfig *ptaskConfig = NULL;
	char task_name[60];

	strcpy(task_name, taskName);

	ptaskConfig = (drvPXISCXI_taskConfig*) malloc(sizeof(drvPXISCXI_taskConfig));
	if(!ptaskConfig) return ptaskConfig;

	ptaskConfig->taskHandle = 0;
	strcpy(ptaskConfig->taskName, task_name);
	ptaskConfig->taskLock = epicsMutexCreate();
	ptaskConfig->bufferLock = epicsMutexCreate();
	scanIoInit(&ptaskConfig->ioScanPvt);
	ptaskConfig->samplingRate = INIT_SAMPLING;
	ptaskConfig->cviCallback = (int32 CVICALLBACK*) taskCallback;

	ptaskConfig->pcontrolThreadConfig = NULL;

	ptaskConfig->adjTime_smplRate_Usec = 0.;
	ptaskConfig->adjTime_cernoxGain_Usec = 0.;
	ptaskConfig->adjTime_lowPass_Usec = 0.;

	ptaskConfig->adjCount_smplRate = 0;
	ptaskConfig->adjCount_cernoxGain = 0;
	ptaskConfig->adjCount_lowPass = 0;

	ptaskConfig->pdataPoolList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!ptaskConfig->pdataPoolList) return NULL;
	else ellInit(ptaskConfig->pdataPoolList);

	ptaskConfig->pchannelConfig = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!ptaskConfig->pchannelConfig) return NULL;
	else ellInit(ptaskConfig->pchannelConfig);

	ptaskConfig->taskStatus = TASK_NOT_INIT;

	return ptaskConfig;

}

static drvPXISCXI_taskConfig* find_taskConfig(char *taskName)
{
	drvPXISCXI_taskConfig *ptaskConfig = NULL;
	char task_name[60];

	strcpy(task_name, taskName);

	ptaskConfig = (drvPXISCXI_taskConfig*) ellFirst(pdrvPXISCXIConfig->ptaskList);
	while(ptaskConfig) {
		if(!strcmp(ptaskConfig->taskName, task_name)) return ptaskConfig;
  		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}

    	return ptaskConfig;
}

static drvPXISCXI_channelConfig* find_channelConfig(drvPXISCXI_taskConfig *ptaskConfig, char *chanName)
{
	drvPXISCXI_channelConfig *pchannelConfig = NULL;
	char channel_name[60];

	strcpy(channel_name, chanName);

	pchannelConfig = (drvPXISCXI_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		if(!strcmp(pchannelConfig->chanName, channel_name)) return pchannelConfig;
		pchannelConfig = (drvPXISCXI_channelConfig*) ellNext(&pchannelConfig->node);
	}

	return pchannelConfig;
}

static unsigned find_channelIndex(drvPXISCXI_taskConfig *ptaskConfig, char *chanName)
{
	drvPXISCXI_channelConfig *pchannelConfig = find_channelConfig(ptaskConfig, chanName);
	if(!pchannelConfig) return -1;
	else return pchannelConfig->chanIndex;
}



static void print_channelConfig(drvPXISCXI_taskConfig *ptaskConfig, int level)
{
	drvPXISCXI_channelConfig *pchannelConfig;

	pchannelConfig = (drvPXISCXI_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
	while(pchannelConfig) {
		epicsPrintf("    %s", pchannelConfig->chanName);
		if(level>2) {
			switch(pchannelConfig->channelType) {
				case CERNOX_CHANNEL:
					epicsPrintf("\tCernox Channel, status: 0x%x, index: %d, gain: %f, cut-off: %f, val: %f Volts", 
						    pchannelConfig->channelStatus,
						    pchannelConfig->chanIndex,
						    pchannelConfig->gain,
						    pchannelConfig->cutOffFreq,
						    ptaskConfig->pdata[pchannelConfig->chanIndex]);
					if(level>3) {
						drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig
							 = (drvPXISCXI_cernoxChannelConfig*) pchannelConfig;
					        epicsPrintf("\n\t minRange: %f, maxRange: %f, excitationCurrent: %f",
							     pcernoxChannelConfig->minRange,
							     pcernoxChannelConfig->maxRange,
							     pcernoxChannelConfig->excitationCurrent);
					
						if(level>4) {
							epicsPrintf(", conv.T: %f usec", pcernoxChannelConfig->conversionTime_usec);
						}
					}
					break;
				case HALL_CHANNEL:
					epicsPrintf("\tHall Channel, status:0x%x, index: %d, gain: %f, cut-off: %f, val: %f Volts",
						    pchannelConfig->channelStatus,
						    pchannelConfig->chanIndex,
						    pchannelConfig->gain,
						    pchannelConfig->cutOffFreq,
						    ptaskConfig->pdata[pchannelConfig->chanIndex]);
					if(level>3) {
						drvPXISCXI_hallChannelConfig *phallChannelConfig
							= (drvPXISCXI_hallChannelConfig*) pchannelConfig;
						epicsPrintf("\n\t minRange: %f, maxRange: %f",
								phallChannelConfig->minRange,
								phallChannelConfig->maxRange);
					}
					break;
				case STRAIN_CHANNEL:
					epicsPrintf("\tStrain Channel, status: 0x%x, index: %d, gain: %f, cut-off: %f, val:%f Strain",
						    pchannelConfig->channelStatus,
						    pchannelConfig->chanIndex,
						    pchannelConfig->gain,
						    pchannelConfig->cutOffFreq,
						    ptaskConfig->pdata[pchannelConfig->chanIndex]);
					if(level>3) {
						drvPXISCXI_strainChannelConfig *pstrainChannelConfig
							 = (drvPXISCXI_strainChannelConfig*) pchannelConfig;
					 	epicsPrintf("\n\t minVal: %f, maxVal: %f, excitionVoltage: %f, gageFactor: %f, poissonRate: %f ",
								pstrainChannelConfig->minRange,
								pstrainChannelConfig->maxRange,
								pstrainChannelConfig->excitationVoltage,
								pstrainChannelConfig->gageFactor,
								pstrainChannelConfig->poissonRate);
						if(level>4) {
						        epicsPrintf("\n\t initBridgeVoltage: %f, nominalGageResistance: %f, leadWireResistance: %f",
								pstrainChannelConfig->initialBridgeVoltage,
								pstrainChannelConfig->nominalGageResistance,
								pstrainChannelConfig->leadWireResistance);
						}
					}
					break;
				case DISPLACEMENT_CHANNEL:
					epicsPrintf("\tDisplacement Channel, status:0x%x, index: %d",
						    pchannelConfig->channelStatus,
						    pchannelConfig->chanIndex);
					break;
				default:
					break;
			}
		}

		epicsPrintf("\n");
  		pchannelConfig = (drvPXISCXI_channelConfig*) ellNext(&pchannelConfig->node);
	}

	return;

}

static cernoxDataConfig* load_cernoxDataOnebyOne(FILE *file)
{
	cernoxDataConfig *pcernoxData = NULL;
	cernoxSubData    *pcernoxSubData = NULL;

	int tmp;
	int i;

	pcernoxData = (cernoxDataConfig*) malloc(sizeof(cernoxDataConfig));
	pcernoxSubData = (cernoxSubData*) malloc(sizeof(cernoxSubData) * 3);

	if(!pcernoxData) return NULL;
	if(!pcernoxSubData) return NULL;

	pcernoxData->pLowTemp = pcernoxSubData + 0;
	pcernoxData->pMidTemp = pcernoxSubData + 1;
	pcernoxData->pHighTemp = pcernoxSubData + 2;

	if(1 != (tmp=fscanf(file, "%s\n", pcernoxData->serial))) { 
		if(EOF != tmp) epicsPrintf("error: loadCernoxList (reading serial)\n");
		return NULL;
	}

	pcernoxSubData = pcernoxData->pLowTemp;
	if(2 != fscanf(file, "%lf %lf\n", &pcernoxSubData->zl, &pcernoxSubData->zu)) {
		epicsPrintf("error: loadCernoxList (serial: %s) reading ZL and ZU for LowTemp\n", pcernoxData->serial);
		return NULL;
	}
	for(i=0;i<6;i++) {
		if(2 != fscanf(file, "%d %lf\n", &(pcernoxSubData->index[i]), &(pcernoxSubData->coeff[i]))) {
			epicsPrintf("error: loadCernoxList (serial %s) reading coefficients [%d] for LowTemp\n",
			            pcernoxData->serial, i);
			return NULL;
		}
		if(pcernoxSubData->index[i] != i) {
			epicsPrintf("error: loadCernoxList (serial %s) reading coefficients [%d]: index %d for LowTemp\n",
				    pcernoxData->serial, i, pcernoxSubData->index[i]);
			return NULL;
		}
	}
	pcernoxSubData->index[i] = -1;

	pcernoxSubData = pcernoxData->pMidTemp;
	if(2 != fscanf(file, "%lf %lf\n", &pcernoxSubData->zl, &pcernoxSubData->zu)) {
		epicsPrintf("error: loadCernoxList (serial %s) reading ZL and ZU for MidTemp\n", pcernoxData->serial);
		return NULL;
	}
	for(i=0;i<6;i++) {
		if(2 != fscanf(file, "%d %lf\n", &(pcernoxSubData->index[i]), &(pcernoxSubData->coeff[i]))) {
			epicsPrintf("error: loadCernoxList (serial %s) reading coefficients [%d] for MidTemp\n",
				    pcernoxData->serial, i);
			return NULL;
		}
		if(pcernoxSubData->index[i] != i) {
			epicsPrintf("error: loadCernoxList (serial %s) reading coefficients [%d]: index %d for MidTemp\n",
				    pcernoxData->serial, i, pcernoxSubData->index[i]);
			return NULL;
		}


	}
	pcernoxSubData->index[i] = -1;

	pcernoxSubData = pcernoxData->pHighTemp;
	if(2 != fscanf(file, "%lf %lf\n", &pcernoxSubData->zl, &pcernoxSubData->zu)) {
		epicsPrintf("error: loadCernoxList (serial %s) reading ZL and ZU for HighTemp\n", pcernoxData->serial);
		return NULL;
	}
	for(i=0;i<8;i++) {
		if(2 != fscanf(file, "%d %lf\n", &(pcernoxSubData->index[i]), &(pcernoxSubData->coeff[i]))) {
			epicsPrintf("error: loadCernoxList (serial %s) reading coefficients [%d] for HighTemp\n",
				    pcernoxData->serial, i);
			return NULL;
		}
		if(pcernoxSubData->index[i] != i) {
			epicsPrintf("error: loadCernoxList (serial %s) reading coefficients [%d]: index %d for HighTemp\n",
				    pcernoxData->serial, i, pcernoxSubData->index[i]);
		}
	}
	pcernoxSubData->index[i] = -1;


	return pcernoxData;
}

static cernoxDataConfig *find_cernoxDataConfig(char *serial)
{
	cernoxDataConfig *pcernoxDataConfig = NULL;
	char serial_buff[60];

	strcpy(serial_buff,serial);

	if(!ellCount(pdrvPXISCXIConfig->pcernoxDataList)) return pcernoxDataConfig;

	pcernoxDataConfig = (cernoxDataConfig*) ellFirst (pdrvPXISCXIConfig->pcernoxDataList);
	while(pcernoxDataConfig) {
		if(!strcmp(serial_buff, pcernoxDataConfig->serial)) break;
		pcernoxDataConfig = (cernoxDataConfig*) ellNext (&pcernoxDataConfig->node);
	}

	return pcernoxDataConfig;
}


static drvPXISCXI_controlThreadConfig*  make_controlThreadConfig(drvPXISCXI_taskConfig *ptaskConfig)
{
	drvPXISCXI_controlThreadConfig *pcontrolThreadConfig  = NULL;

	pcontrolThreadConfig  = (drvPXISCXI_controlThreadConfig*)malloc(sizeof(drvPXISCXI_controlThreadConfig));
	if(!pcontrolThreadConfig) return pcontrolThreadConfig; 

	sprintf(pcontrolThreadConfig->threadName, "%s_ctrl", ptaskConfig->taskName);
	
	pcontrolThreadConfig->threadPriority = epicsThreadPriorityLow;
	pcontrolThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pcontrolThreadConfig->threadFunc      = (EPICSTHREADFUNC) controlThread;
	pcontrolThreadConfig->threadParam     = (void*) ptaskConfig;

	pcontrolThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));
	if(pcontrolThreadConfig->threadQueueId) ptaskConfig->taskStatus |= TASK_CONTQUEUE;

	epicsThreadCreate(pcontrolThreadConfig->threadName,
			  pcontrolThreadConfig->threadPriority,
			  pcontrolThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pcontrolThreadConfig->threadFunc,
			  (void*) pcontrolThreadConfig->threadParam);


	return pcontrolThreadConfig;
}

static int32 CVICALLBACK taskCallback(TaskHandle taskHandle, int32 everyNsampleEventType, uInt32 nSamples, void *callbackData)
{
	unsigned  long long int start, stop;

	int32 error = 0;
	char errBuff[2048] = { '\0' };	
	drvPXISCXI_taskConfig *ptaskConfig = (drvPXISCXI_taskConfig*) callbackData;
	dataPoolConfig *pdataPoolConfig;

	start = drvPXISCXI_rdtsc();

	pdataPoolConfig = (dataPoolConfig*) ellFirst(ptaskConfig->pdataPoolList);
	ellDelete(ptaskConfig->pdataPoolList, &pdataPoolConfig->node);
	ellAdd(ptaskConfig->pdataPoolList, &pdataPoolConfig->node);
	pdataPoolConfig = (dataPoolConfig*) ellFirst(ptaskConfig->pdataPoolList);
	pdataPoolConfig->used_count++;


	DAQmxErrChk(DAQmxReadAnalogF64(ptaskConfig->taskHandle,
		 	  	       DAQmx_Val_Auto,
			   	       DAQmx_Val_WaitInfinitely,
			   	       DAQmx_Val_GroupByChannel,
			   	       pdataPoolConfig->pdata,
			   	       ellCount(ptaskConfig->pchannelConfig),
			   	       &pdataPoolConfig->num_read_data,
			   	       NULL));

	epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->pdata = pdataPoolConfig->pdata;
	epicsMutexUnlock(ptaskConfig->bufferLock);

	scanIoRequest(ptaskConfig->ioScanPvt);

	stop = drvPXISCXI_rdtsc();
	ptaskConfig->callbackTimeUsec = drvPXISCXI_intervalUSec(start, stop);

	return 0;

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



static void controlThread(void *param)
{

	drvPXISCXI_taskConfig *ptaskConfig = (drvPXISCXI_taskConfig*) param;
	drvPXISCXI_controlThreadConfig *pcontrolThreadConfig = (drvPXISCXI_controlThreadConfig*) ptaskConfig->pcontrolThreadConfig;
	controlThreadQueueData		queueData;


	epicsPrintf("task launching: %s thread for %s task\n",pcontrolThreadConfig->threadName, ptaskConfig->taskName);

	while(TRUE) {
		EXECFUNCQUEUE            pFunc;
		execParam                *pexecParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		drvPXISCXI_taskConfig    *ptaskConfig;

		epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId,
				         (void*) &queueData,
					 sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;	
		precord    = (struct dbCommon*) pexecParam->precord;
		prset      = (struct rset*) precord->rset;
		ptaskConfig = (drvPXISCXI_taskConfig *) pexecParam->ptaskConfig;

		if(!pFunc) continue;
		else pFunc(pexecParam);

		if(!precord) continue;


		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}


	return;
}





static void clearAllDAQmxTasks(void)
{
	int32 error = 0;
	char errBuf[2048] = { '\0'};
	drvPXISCXI_taskConfig *ptaskConfig;

	
	ptaskConfig = (drvPXISCXI_taskConfig*) ellFirst (pdrvPXISCXIConfig->ptaskList);

	while(ptaskConfig) {

		if(ptaskConfig->taskStatus & (TASK_INITIALIZED | TASK_STARTED)) {
			/*
			ellDelete(pdrvPXISCXIConfig->ptaskList, &ptaskConfig->node);
			*/
			if(ptaskConfig->taskStatus & TASK_STARTED) {
				DAQmxErrChk(DAQmxStopTask(ptaskConfig->taskHandle));
				ptaskConfig->taskStatus &= ~TASK_STARTED;
				ptaskConfig->taskStatus |= TASK_STOPED;

				epicsPrintf("Exit Hook: Stoping Task %s\n", ptaskConfig->taskName);
			}
			DAQmxErrChk(DAQmxClearTask(ptaskConfig->taskHandle));
			ptaskConfig->taskStatus &= ~TASK_INITIALIZED;
			ptaskConfig->taskStatus |= TASK_DESTROIED;

			epicsPrintf("Exit Hook: Clear Task %s\n", ptaskConfig->taskName);
				
			/*
			ellAdd(pdrvPXISCXIConfig->pdestroiedList, &ptaskConfig->node);
			*/
		}

		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}




	return;

Error:
	if(DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuf, 2048);

	if(ptaskConfig->taskHandle != 0) {
		DAQmxStopTask(ptaskConfig->taskHandle);
	        DAQmxClearTask(ptaskConfig->taskHandle);
	}

	if(DAQmxFailed(error))
		epicsPrintf("DAQmx Error: %s\n", errBuf);	

	return;
}

static void cernoxDataCallFunction(const iocshArgBuf *args)
{
	int i;
	char serial[60];
	cernoxDataConfig *pcernoxDataConfig;
	cernoxSubData    *pcernoxSubData;

	if(!args[0].sval) { 
		epicsPrintf("usage: cernoxData sensor_serial\n");
		return;
	}

	strcpy(serial, args[0].sval);

	pcernoxDataConfig = find_cernoxDataConfig(serial);
	if(!pcernoxDataConfig) {
		epicsPrintf("Cernox Data not found for serial %s\n", serial);
		return;
	}

	epicsPrintf("serial: %s\n", pcernoxDataConfig->serial);
	
	pcernoxSubData = pcernoxDataConfig->pLowTemp;	
	epicsPrintf("  Low Temp. Range: ZL=%f, ZU=%f\n", pcernoxSubData->zl, pcernoxSubData->zu);
	for(i=0;i<6;i++) {
		epicsPrintf("    [%d] %f\n", pcernoxSubData->index[i], pcernoxSubData->coeff[i]);
	}
	pcernoxSubData = pcernoxDataConfig->pMidTemp;
	epicsPrintf("  Mid Temp. Range: ZL=%f, ZU=%f\n", pcernoxSubData->zl, pcernoxSubData->zu);
	for(i=0;i<6;i++) {
		epicsPrintf("    [%d] %f\n", pcernoxSubData->index[i], pcernoxSubData->coeff[i]);
	}
	pcernoxSubData = pcernoxDataConfig->pHighTemp;
	epicsPrintf("  High Temp. Range: ZL=%f, ZU=%f\n", pcernoxSubData->zl, pcernoxSubData->zu);
	for(i=0;i<8;i++) {
		epicsPrintf("    [%d] %f\n", pcernoxSubData->index[i], pcernoxSubData->coeff[i]);
	}


	return;
}
static void cernoxListCallFunction(const iocshArgBuf *args)
{
	int level = args[0].ival;
	cernoxDataConfig *pcernoxDataConfig;
	cernoxSubData    *pcernoxSubData;
	int i;

	epicsPrintf("cernoxList: totoal %d sensor data found\n",ellCount(pdrvPXISCXIConfig->pcernoxDataList));
	if(!ellCount(pdrvPXISCXIConfig->pcernoxDataList)) return;

	if(level<1) return;

	pcernoxDataConfig = (cernoxDataConfig*) ellFirst(pdrvPXISCXIConfig->pcernoxDataList);

	while(pcernoxDataConfig) {
		epicsPrintf("serial: %s\n", pcernoxDataConfig->serial);
		
		pcernoxSubData = pcernoxDataConfig->pLowTemp;
		if(level>1) epicsPrintf("  Low Temp. Range: ZL=%f, ZU=%f\n", pcernoxSubData->zl, pcernoxSubData->zu);
	        if(level>2) {
			for(i=0;i<6;i++) epicsPrintf("    [%d] %f\n", pcernoxSubData->index[i], pcernoxSubData->coeff[i]);	
		}
		pcernoxSubData = pcernoxDataConfig->pMidTemp;
		if(level>1) epicsPrintf("  Mid Temp. Range: ZL=%f, ZU=%f\n", pcernoxSubData->zl, pcernoxSubData->zu);
		if(level>2) {
			for(i=0;i<6;i++) epicsPrintf("    [%d] %f\n", pcernoxSubData->index[i], pcernoxSubData->coeff[i]);
		}
		pcernoxSubData = pcernoxDataConfig->pHighTemp;
		if(level>1) epicsPrintf("  High Temp. Range: ZL=%f, ZU=%f\n", pcernoxSubData->zl, pcernoxSubData->zu);
		if(level>2) {
			for(i=0;i<8;i++) epicsPrintf("    [%d] %f\n", pcernoxSubData->index[i], pcernoxSubData->coeff[i]);
		}

		pcernoxDataConfig = (cernoxDataConfig*) ellNext(&pcernoxDataConfig->node);
	}
}
static void loadCernoxListCallFunction(const iocshArgBuf *args)
{
	char file_name[80] = { '\0' };;
	cernoxDataConfig *pcernoxDataConfig;
	FILE *file;

	if(args[0].sval) strcpy(file_name, args[0].sval);

	if(make_drvConfig()) return;

 	file = fopen(file_name, "r");
	if(!file) {
		epicsPrintf("Can not load %s\n", file_name);
		return;
	}

	pcernoxDataConfig = load_cernoxDataOnebyOne(file);
	while(pcernoxDataConfig) {
		ellAdd(pdrvPXISCXIConfig->pcernoxDataList, &pcernoxDataConfig->node);
		pcernoxDataConfig = load_cernoxDataOnebyOne(file);
	}

	epicsPrintf("cernoxData %d are read\n", ellCount(pdrvPXISCXIConfig->pcernoxDataList));

	fclose(file);
	return;
}
static void createPXITaskCallFunction(const iocshArgBuf *args)
{
	drvPXISCXI_taskConfig *ptaskConfig = NULL;
	char task_name[60];

	if(args[0].sval) strcpy(task_name, args[0].sval);

	if(make_drvConfig()) return;

	ptaskConfig = make_taskConfig(task_name);
	if(!ptaskConfig) return;
	
	ptaskConfig->pcontrolThreadConfig = make_controlThreadConfig(ptaskConfig);
	if(!ptaskConfig->pcontrolThreadConfig) return;
	else ptaskConfig->taskStatus |= TASK_CONTHREAD;


	ellAdd(pdrvPXISCXIConfig->ptaskList, &ptaskConfig->node);

	return;
}

static void createPXICernoxChannelCallFunction(const iocshArgBuf *args)
{
	drvPXISCXI_taskConfig *ptaskConfig = NULL;
	drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig = NULL;
	char task_name[60];
	char channel_name[60];

	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(channel_name, args[1].sval);

	ptaskConfig = find_taskConfig(task_name);
	if(!ptaskConfig) return;

	pcernoxChannelConfig = (drvPXISCXI_cernoxChannelConfig*) malloc(sizeof(drvPXISCXI_cernoxChannelConfig));
	if(!pcernoxChannelConfig) return;

	pcernoxChannelConfig->channelType = CERNOX_CHANNEL;
	pcernoxChannelConfig->channelStatus = 0x0000;
	strcpy(pcernoxChannelConfig->chanName, channel_name);
	pcernoxChannelConfig->chanIndex = ellCount(ptaskConfig->pchannelConfig);

	pcernoxChannelConfig->gain = INIT_GAIN_CERNOX;
	pcernoxChannelConfig->cutOffFreq = INIT_CUTOFF;
	pcernoxChannelConfig->minRange = INIT_MINRANGE_CERNOX;
	pcernoxChannelConfig->maxRange = INIT_MAXRANGE_CERNOX;
	pcernoxChannelConfig->excitationCurrent  = INIT_EXCITATIONCURR_CERNOX;
	pcernoxChannelConfig->ptaskConfig = ptaskConfig;
	pcernoxChannelConfig->conversionTime_usec = 0.;

	ellAdd(ptaskConfig->pchannelConfig, &pcernoxChannelConfig->node);
	
	return;
}

static void createPXIStrainChannelCallFunction(const iocshArgBuf *args)
{
	drvPXISCXI_taskConfig             *ptaskConfig = NULL;
	drvPXISCXI_strainChannelConfig    *pstrainChannelConfig = NULL;
	char task_name[60];
	char channel_name[60];
	char bridge_name[60];

	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(channel_name, args[1].sval);
	if(args[2].sval) strcpy(bridge_name, args[2].sval);

	ptaskConfig = find_taskConfig(task_name);
	if(!ptaskConfig) return;

	pstrainChannelConfig = (drvPXISCXI_strainChannelConfig*) malloc(sizeof(drvPXISCXI_strainChannelConfig));
	if(!pstrainChannelConfig) return;

	pstrainChannelConfig->channelType = STRAIN_CHANNEL;
	pstrainChannelConfig->channelStatus = 0x0000;
	strcpy(pstrainChannelConfig->chanName, channel_name);
	strcpy(pstrainChannelConfig->bridgeStr, bridge_name);
	pstrainChannelConfig->chanIndex = ellCount(ptaskConfig->pchannelConfig);

	pstrainChannelConfig->minRange = args[3].dval;
	pstrainChannelConfig->maxRange = args[4].dval;
	pstrainChannelConfig->units    = INIT_UNIT_STRAIN;
	
	pstrainChannelConfig->bridgeType = find_bridgeType(pstrainChannelConfig->bridgeStr);
	
	pstrainChannelConfig->excitationSource = INIT_EXCITATIONSOURCE_STRAIN;
	pstrainChannelConfig->excitationVoltage = args[5].dval;
	pstrainChannelConfig->gageFactor = args[6].dval;
	pstrainChannelConfig->poissonRate = args[7].dval;
	pstrainChannelConfig->initialBridgeVoltage = args[8].dval;
	pstrainChannelConfig->nominalGageResistance = args[9].dval;
	pstrainChannelConfig->leadWireResistance = args[10].dval;

	pstrainChannelConfig->ptaskConfig = ptaskConfig;



	ellAdd(ptaskConfig->pchannelConfig, &pstrainChannelConfig->node);

	return;
}

static void createPXIHallChannelCallFunction(const iocshArgBuf *args)
{
	drvPXISCXI_taskConfig		*ptaskConfig = NULL;
	drvPXISCXI_hallChannelConfig	*phallChannelConfig = NULL;
	char task_name[60];
	char channel_name[60];

	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(channel_name, args[1].sval);
	
	ptaskConfig = find_taskConfig(task_name);
	if(!ptaskConfig) return;

	phallChannelConfig = (drvPXISCXI_hallChannelConfig*) malloc(sizeof(drvPXISCXI_hallChannelConfig));
	if(!phallChannelConfig) return;
	
	phallChannelConfig->channelType = HALL_CHANNEL;
	phallChannelConfig->channelStatus = 0x0000;
	strcpy(phallChannelConfig->chanName, channel_name);
	phallChannelConfig->chanIndex = ellCount(ptaskConfig->pchannelConfig);

	phallChannelConfig->gain = INIT_GAIN_HALL;
	phallChannelConfig->cutOffFreq = INIT_CUTOFFFREQ_HALL;

	phallChannelConfig->minRange = INIT_MINRANGE_HALL;
	phallChannelConfig->maxRange = INIT_MAXRANGE_HALL;

	phallChannelConfig->ptaskConfig = ptaskConfig;

	ellAdd(ptaskConfig->pchannelConfig, &phallChannelConfig->node);
	
	return;
}


LOCAL long drvPXISCXI_io_report(int level)
{

	drvPXISCXI_taskConfig *ptaskConfig;
	dataPoolConfig *pdataPoolConfig;

	if(!pdrvPXISCXIConfig) return 0;

	if(ellCount(pdrvPXISCXIConfig->ptaskList))
		ptaskConfig = (drvPXISCXI_taskConfig*) ellFirst (pdrvPXISCXIConfig->ptaskList);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d PXI task(s) found\n",ellCount(pdrvPXISCXIConfig->ptaskList));

	if(level<1) return 0;

	while(ptaskConfig) {
		epicsPrintf("  Task name: %s, handle: 0x%x, status: 0x%x, connected channels: %d\n",
			    ptaskConfig->taskName, 
			    (unsigned )ptaskConfig->taskHandle, 
			    ptaskConfig->taskStatus,
			    ellCount(ptaskConfig->pchannelConfig));
		if(level>2) {
			epicsPrintf("   Sampling Rate: %f/sec, Buffer-Pool: %d\n",
				    ptaskConfig->samplingRate,
				    ellCount(ptaskConfig->pdataPoolList));
		}

		if(level>3 && ellCount(ptaskConfig->pdataPoolList)) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			pdataPoolConfig = (dataPoolConfig*) ellFirst(ptaskConfig->pdataPoolList);
			while(pdataPoolConfig) {
				epicsPrintf("(%ld/%ld/%p) ", 
					    pdataPoolConfig->used_count,
					    pdataPoolConfig->num_read_data,
					    pdataPoolConfig->pdata);
				pdataPoolConfig = (dataPoolConfig*) ellNext(&pdataPoolConfig->node);
			}
			
			epicsPrintf("\n");
			epicsPrintf("   callback time: %fusec\n", ptaskConfig->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", ptaskConfig->adjCount_smplRate,
					                                                 ptaskConfig->adjTime_smplRate_Usec);
			epicsPrintf("   Lowpass  adj. counter: %d, adj. time: %fusec\n", ptaskConfig->adjCount_lowPass,
					                                                 ptaskConfig->adjTime_lowPass_Usec);
			epicsPrintf("   CernoxGain adj. counter: %d, adj. time: %fusec\n", ptaskConfig->adjCount_cernoxGain,
					                                                   ptaskConfig->adjTime_cernoxGain_Usec);
		}


		if(ellCount(ptaskConfig->pchannelConfig)>0) print_channelConfig(ptaskConfig,level);
		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}

	return 0;
}


LOCAL long drvPXISCXI_init_driver(void)
{
	drvPXISCXI_taskConfig *ptaskConfig = NULL;
	drvPXISCXI_channelConfig *pchannelConfig = NULL;
	drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig = NULL;
	drvPXISCXI_strainChannelConfig *pstrainChannelConfig = NULL;
	drvPXISCXI_hallChannelConfig *phallChannelConfig = NULL;

	unsigned long long int a;

	if(!pdrvPXISCXIConfig)  return 0;

	ptaskConfig = (drvPXISCXI_taskConfig*) ellFirst(pdrvPXISCXIConfig->ptaskList);
	while(ptaskConfig) {
		if(ellCount(ptaskConfig->pchannelConfig)) {
			init_task(ptaskConfig); 

			pchannelConfig = (drvPXISCXI_channelConfig*) ellFirst(ptaskConfig->pchannelConfig);
			while(pchannelConfig) {
				switch(pchannelConfig->channelType) {
					case CERNOX_CHANNEL:
						pcernoxChannelConfig = (drvPXISCXI_cernoxChannelConfig*) pchannelConfig;
						init_cernoxChannel(pcernoxChannelConfig);
						break;
					case HALL_CHANNEL:
						phallChannelConfig = (drvPXISCXI_hallChannelConfig*) pchannelConfig;
						init_hallChannel(phallChannelConfig);
						break;
					case STRAIN_CHANNEL:
						pstrainChannelConfig = (drvPXISCXI_strainChannelConfig*) pchannelConfig;
						init_strainChannel(pstrainChannelConfig);
						break;
					case DISPLACEMENT_CHANNEL:
						break;
				}
				pchannelConfig = (drvPXISCXI_channelConfig*) ellNext(&pchannelConfig->node);
			}  

			init_taskTiming(ptaskConfig);
			init_taskCallback(ptaskConfig); 
			init_dataPool(ptaskConfig);
			start_task(ptaskConfig); 
			
		} 

		ptaskConfig = (drvPXISCXI_taskConfig*) ellNext(&ptaskConfig->node);
	}


	epicsAtExit((EPICSEXITFUNC) clearAllDAQmxTasks, NULL);

	a = drvPXISCXI_rdtsc();
	epicsThreadSleep(1.);
	pdrvPXISCXIConfig->one_sec_interval = drvPXISCXI_rdtsc() - a;

	epicsPrintf("drvPXISCXI: measured clock speed %6.4lf GHz\n", 1.E-9 * (double)pdrvPXISCXIConfig->one_sec_interval);

	return 0;
}



drvPXISCXI_taskConfig* drvPXISCXI_find_taskConfig(char *taskName)
{
	return find_taskConfig(taskName);
}

drvPXISCXI_channelConfig* drvPXISCXI_find_channelConfig(char *taskName, char *chanName)
{
	drvPXISCXI_taskConfig *ptaskConfig = find_taskConfig(taskName);
	if(!ptaskConfig) return NULL;

	return find_channelConfig(ptaskConfig, chanName);
}

cernoxDataConfig* drvPXISCXI_find_cernoxDataConfig(char *serial)
{
	return find_cernoxDataConfig(serial);
}

void drvPXISCXI_set_cernoxChanGain(drvPXISCXI_taskConfig *ptaskConfig, double gain)
{
	set_cernoxChanGain(ptaskConfig, gain);
	return;
}

void drvPXISCXI_set_cernoxChanGainAllTasks(double gain)
{
	set_cernoxChanGainALLTasks(gain);
	return;
}

void drvPXISCXI_set_lowpassFilter(drvPXISCXI_taskConfig *ptaskConfig, double cutOffFreq)
{
	set_lowpassFilter(ptaskConfig, cutOffFreq);
	return;
}

void drvPXISCXI_set_samplingRate(drvPXISCXI_taskConfig *ptaskConfig, double samplingRate)
{
	set_samplingRate(ptaskConfig, (float64) samplingRate);
	return;
}

void drvPXISCXI_set_lowpassFilterALLTasks(double cutOffFreq)
{
	set_lowpassFilterALLTasks(cutOffFreq);
	return;
}
