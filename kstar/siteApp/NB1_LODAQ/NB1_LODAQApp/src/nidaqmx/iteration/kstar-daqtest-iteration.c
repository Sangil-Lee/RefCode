// --------------------------------------------------------------
// Test program for DAQmx
// --------------------------------------------------------------
#define _REENTRANT
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>

#include <NIDAQmx.h>

#define	DEV_NUM			2
#define	SAMPLES			1000
#define	MAX_SAMPLES		(5 * SAMPLES)	// 5 secodns

typedef struct {
	int			id;
	char		devName[64];
	char		chName[64];
	TaskHandle	taskHandle;
	int32		nReadTotal;
	int32		bRunningTask;
} TaskInfoTag;

#define DAQmxErrChk(functionCall)		if( DAQmxFailed(error=(functionCall)) ) goto Error; else
#define DAQmxErrChk2(name,functionCall)	if( DAQmxFailed(error=(functionCall)) ) {printlog("[%s] \n",name); goto Error;} else

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);

int start_daq (TaskInfoTag *pTaskInfo);
void *daq_task (void *arg);
void printlog (const char *fmt, ...);

int	gbUseResetDevice;

int main (int argc, char **argv)
{
	pthread_t	tid;
	int			id[DEV_NUM];
	int			i;

	if (argc < 2) {
		printf ("Usage: %s {0 | 1} \n", argv[0]);
		printf ("   0 : without DAQmxResetDevice() \n");
		printf ("   1 : with DAQmxResetDevice() \n");
		return (0);
	}

	gbUseResetDevice	= atoi(argv[1]) > 0 ? 1 : 0;

	for (i = 0; i < DEV_NUM; i++) {
		id[i]	= i;
		pthread_create (&tid, NULL, daq_task, (void *)&id[i]);
	}

	while (1) {
		sleep(10);
	}

	return(0);
}

void *daq_task (void *arg)
{
	TaskInfoTag	taskInfo;
	taskInfo.id	= *((int *)arg);

	sprintf (taskInfo.devName, "Dev%d", taskInfo.id+1);
	sprintf (taskInfo.chName , "Dev%d/ai0", taskInfo.id+1);

	printlog ("[%s] thread 0x%x was started\n", taskInfo.devName, pthread_self());

	while (1) {
		start_daq (&taskInfo);
		sleep(5);
	}

	return (NULL);
}

int start_daq (TaskInfoTag *pTaskInfo)
{
	int32       error=0;
	char        errBuff[2048]={'\0'};

	if (gbUseResetDevice) {
		printlog ("[%s] ResetDevice ...\n", pTaskInfo->devName);
		DAQmxErrChk2 ("ResetDevice", DAQmxResetDevice(pTaskInfo->devName));
	}

	DAQmxErrChk2 ("CreateTask", DAQmxCreateTask("",&pTaskInfo->taskHandle));
	DAQmxErrChk2 ("CreateAIVoltageChan"  , DAQmxCreateAIVoltageChan(pTaskInfo->taskHandle,pTaskInfo->chName,"",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk2 ("CfgSampClkTiming", DAQmxCfgSampClkTiming(pTaskInfo->taskHandle,"",SAMPLES,DAQmx_Val_Rising,DAQmx_Val_ContSamps,SAMPLES));
	//DAQmxErrChk2 ("CfgDigEdgeStartTrig", DAQmxCfgDigEdgeStartTrig(pTaskInfo->taskHandle,TRIG_NAME,DAQmx_Val_Rising));
	DAQmxErrChk2 ("RegisterEveryNSamplesEvent", DAQmxRegisterEveryNSamplesEvent(pTaskInfo->taskHandle,DAQmx_Val_Acquired_Into_Buffer,SAMPLES,0,EveryNCallback,pTaskInfo));

	printlog ("[%s] StartTask ...\n", pTaskInfo->devName);

	pTaskInfo->nReadTotal	= 0;
	pTaskInfo->bRunningTask	= 1;

	DAQmxErrChk2 ("StartTask", DAQmxStartTask(pTaskInfo->taskHandle));
	
	while (pTaskInfo->bRunningTask) {
		sleep (1);
	}

Error:

	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		printlog("DAQmx Error: %u(0x%x), %s\n", error, error, errBuff);
	}

	if( pTaskInfo->taskHandle ) {
		printlog ("[%s] ClearTask ...\n", pTaskInfo->devName);
		//DAQmxStopTask(pTaskInfo->taskHandle);
		DAQmxClearTask(pTaskInfo->taskHandle);
		pTaskInfo->taskHandle = 0;
	}

	if( DAQmxFailed(error) ) {
		printlog("DAQmx Error: %u(0x%x), %s\n", error, error, errBuff);
	}

	printlog ("[%s] End of processing ...\n", pTaskInfo->devName);
	return 0;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32           error=0;
	char            errBuff[2048]={'\0'};
	int32           nRead;
	float64         data[SAMPLES];
	TaskInfoTag		*pTaskInfo	= (TaskInfoTag *)callbackData;

	DAQmxErrChk2 ("ReadAnalogF64", DAQmxReadAnalogF64(taskHandle,SAMPLES,10.0,DAQmx_Val_GroupByChannel,data,SAMPLES,&nRead,NULL));
	
	pTaskInfo->nReadTotal += nRead;

	printlog ("\t[%s] Callback : total(%d) \n", pTaskInfo->devName, pTaskInfo->nReadTotal);

	if (MAX_SAMPLES <= pTaskInfo->nReadTotal) {
		printlog ("[%s] StopTask ...\n", pTaskInfo->devName);

		DAQmxTaskControl(pTaskInfo->taskHandle, DAQmx_Val_Task_Abort);
		DAQmxStopTask(pTaskInfo->taskHandle);

		pTaskInfo->bRunningTask	= 0;

		printlog ("[%s] StopTask end \n", pTaskInfo->devName);
	}

Error:

	return 0;
}

void printlog (const char *fmt, ...)
{
	char buf[1024];
	char bufTime[64];
	va_list argp;
	time_t epoch;
	struct tm *tms;

	va_start (argp, fmt);
	vsprintf (buf, fmt, argp);
	va_end (argp);

	time (&epoch);
	tms	= localtime (&epoch);

	strftime (bufTime, sizeof(bufTime), "%m/%d %H:%M:%S", tms);
	fprintf (stdout, "[%s] %s", bufTime, buf);
}

