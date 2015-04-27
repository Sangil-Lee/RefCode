#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <NIDAQmx.h>

#define DEV_NUM		3
#define TRIG_NAME	"/Dev3/PFI0"

static TaskHandle gTaskHandle[DEV_NUM];
static int32      gReadTotal [DEV_NUM];

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
#define DAQmxErrChk2(name,functionCall) if( DAQmxFailed(error=(functionCall)) ) {printf("[%s] \n",name); goto Error;} else

static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[]);

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int start_daq (void)
{
	int32       error=0;
	char        errBuff[2048]={'\0'};
	char	    str1[256],str2[256],trigName[256];
	char		devName[256];
	float64     clkRate;
	int			i;

	for (i = 0; i < DEV_NUM; i++) {
		sprintf (devName, "Dev%d/ai0", i+1);
		DAQmxErrChk2 ("CreateTask", DAQmxCreateTask("",&gTaskHandle[i]));

		DAQmxErrChk2 ("CreateAIVoltageChan"  , DAQmxCreateAIVoltageChan(gTaskHandle[i],devName,"",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
		DAQmxErrChk2 ("CfgSampClkTiming", DAQmxCfgSampClkTiming(gTaskHandle[i],"",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
		DAQmxErrChk2 ("CfgDigEdgeStartTrig", DAQmxCfgDigEdgeStartTrig(gTaskHandle[i],TRIG_NAME,DAQmx_Val_Rising));

		DAQmxErrChk2 ("RegisterEveryNSamplesEvent", DAQmxRegisterEveryNSamplesEvent(gTaskHandle[i],DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL));
		DAQmxErrChk2 ("RegisterDoneEvent", DAQmxRegisterDoneEvent(gTaskHandle[i],0,DoneCallback,NULL));

		printf ("Dev(%s) : Task(%u) was created \n", devName, gTaskHandle[i]);
	}

	for (i = 0; i < DEV_NUM; i++) {
		gReadTotal[i]	= 0;

		printf ("start task%d(%u) ...\n", i+1, gTaskHandle[i]);
		DAQmxErrChk2 ("StartTask", DAQmxStartTask(gTaskHandle[i]));
	}
	
	printf("Acquiring samples continuously. Press Enter to interrupt\n");
	printf("\nHandle:\tTotal:\tDev1\tDev2\tDev3\n");

	getchar();

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		printf("DAQmx Error: %u(0x%x), %s\n", error, error, errBuff);
	}

	/*********************************************/
	// DAQmx Stop Code
	/*********************************************/
	for (i = 0; i < DEV_NUM; i++) {
		if( gTaskHandle[i] ) {
			printf ("stop task%d (%u) ...\n", i+1, gTaskHandle[i]);
			DAQmxStopTask(gTaskHandle[i]);
			DAQmxClearTask(gTaskHandle[i]);
			gTaskHandle[i] = 0;
		}
	}

	if( DAQmxFailed(error) ) {
		printf("DAQmx Error: %u(0x%x), %s\n", error, error, errBuff);
	}

	printf("End of program. Press enter to restart ... \n");
	getchar();
	return 0;
}

int main (int argc, char **argv)
{
	while (1) {
		start_daq();
	}

	return(0);
}

static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[])
{
	int32	error=0;
	char	chan[256];
	char	*slash;

	DAQmxErrChk2 ("GetNthTaskChannel", DAQmxGetNthTaskChannel(taskHandle,1,chan,256));
	DAQmxErrChk2 ("GetPhysicalChanName", DAQmxGetPhysicalChanName(taskHandle,chan,chan,256));
	if( (slash=strchr(chan,'/'))!=NULL ) {
		*slash = '\0';
		*triggerName++ = '/';
		strcat(strcat(strcpy(triggerName,chan),"/"),terminalName);
	} else
		strcpy(triggerName,terminalName);

Error:
	return error;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32           error=0;
	char            errBuff[2048]={'\0'};
	int32           nRead;
	float64         data[1000];
	int				i;

	DAQmxErrChk2 ("ReadAnalogF64", DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&nRead,NULL));
	
	printf ("[%10u]", taskHandle);

	for (i = 0; i < DEV_NUM; i++) {
		if(taskHandle == gTaskHandle[i]) {
			gReadTotal[i] += nRead;
		}
		printf("\t%10d", gReadTotal[i]);
	}
	printf ("\r");
	fflush(stdout);

Error:

	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk2 ("Status", status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}
