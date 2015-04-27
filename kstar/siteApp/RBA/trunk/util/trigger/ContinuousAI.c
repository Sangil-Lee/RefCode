#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <NIDAQmx.h>

typedef struct {
	char		devName[40];
	char		chName[64];
	char		trigName[40];
	TaskHandle	nTaskHandle;
	int			nReadTotal;
} TaskInfo;

TaskInfo	gTaskInfoList[] = {
	{ "Dev3", "Dev3/ai0", "/Dev3/PFI0", 0, 0 },
	{ "Dev1", "Dev1/ai0", "/Dev3/ai/StartTrigger", 0, 0 },
	{ "Dev2", "Dev2/ai0", "/Dev3/ai/StartTrigger", 0, 0 },
};

#define DIM(x)	( sizeof(x)/sizeof(x[0]) )

#define DAQmxErrChk(functionCall) 		if( DAQmxFailed(error=(functionCall)) ) goto Error; else
//#define DAQmxErrChk2(name,functionCall) if( DAQmxFailed(error=(functionCall)) ) {printf("[%s] \n",name); goto Error;} else
#define DAQmxErrChk2(name,functionCall) printf("[%s] \n",name); DAQmxErrChk(functionCall)

static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[]);

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int getNumOfDev ()
{
	return (DIM(gTaskInfoList));
}

int start_daq (void)
{
	int32       error=0;
	char        errBuff[2048]={'\0'};
	int			i;

	for (i = 0; i < getNumOfDev(); i++) {
		printf ("\n------------------------------------------------------------\n");
		printf ("Create Task for Dev(%s)\n", gTaskInfoList[i].devName);

		DAQmxErrChk2 ("CreateTask", DAQmxCreateTask("",&gTaskInfoList[i].nTaskHandle));

		DAQmxErrChk2 ("CreateAIVoltageChan"  , DAQmxCreateAIVoltageChan(gTaskInfoList[i].nTaskHandle,
					gTaskInfoList[i].chName,"",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));

		DAQmxErrChk2 ("CfgSampClkTiming", DAQmxCfgSampClkTiming(gTaskInfoList[i].nTaskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

		//DAQmxErrChk2 ("SetRefClkSrc", DAQmxSetRefClkSrc(gTaskInfoList[i].nTaskHandle,"PXI_Clk1"));
		//DAQmxErrChk2 ("SetRefClkRate", DAQmxSetRefClkRate(gTaskInfoList[i].nTaskHandle,10000000.0));
		DAQmxErrChk2 ("CfgDigEdgeStartTrig", DAQmxCfgDigEdgeStartTrig(gTaskInfoList[i].nTaskHandle,gTaskInfoList[i].trigName,DAQmx_Val_Rising));

		DAQmxErrChk2 ("RegisterEveryNSamplesEvent", DAQmxRegisterEveryNSamplesEvent(gTaskInfoList[i].nTaskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL));
		DAQmxErrChk2 ("RegisterDoneEvent", DAQmxRegisterDoneEvent(gTaskInfoList[i].nTaskHandle,0,DoneCallback,NULL));

		printf ("Dev(%s) : Task(%u) was created \n", gTaskInfoList[i].devName, gTaskInfoList[i].nTaskHandle);
	}

	printf ("\n------------------------------------------------------------\n");

	for (i = 0; i < getNumOfDev(); i++) {
		gTaskInfoList[i].nReadTotal	= 0;

		DAQmxErrChk2 ("StartTask", DAQmxStartTask(gTaskInfoList[i].nTaskHandle));
	}
	
	printf ("\n------------------------------------------------------------\n");

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
	for (i = 0; i < getNumOfDev(); i++) {
		if( gTaskInfoList[i].nTaskHandle ) {
			printf ("stop task%d (%u) ...\n", i+1, gTaskInfoList[i].nTaskHandle);

			DAQmxStopTask(gTaskInfoList[i].nTaskHandle);
			DAQmxClearTask(gTaskInfoList[i].nTaskHandle);

			gTaskInfoList[i].nTaskHandle = 0;
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

	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&nRead,NULL));
	
	printf ("[%10u]", taskHandle);

	for (i = 0; i < getNumOfDev(); i++) {
		if(taskHandle == gTaskInfoList[i].nTaskHandle) {
			gTaskInfoList[i].nReadTotal += nRead;
		}
		printf("\t%10d", gTaskInfoList[i].nReadTotal);
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
