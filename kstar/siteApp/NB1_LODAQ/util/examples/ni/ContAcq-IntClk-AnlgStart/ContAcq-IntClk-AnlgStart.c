/*********************************************************************
*
* ANSI C Example program:
*    ContAcq-IntClk-AnlgStart.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to continuously acquire data using
*    the DAQ device's internal clock and an analog slope start
*    trigger.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage range.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the rate of the acquisition. Also set the Samples to Read
*       control. This will determine how many samples are read each
*       time the while loop iterates. This also determines how many
*       points are plotted on the graph each iteration.
*    Note: The rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*    4. Set the source of the Analog Slope Start Trigger. By default
*       this is APFI0.
*    5. Set the slope and level of desired analog edge condition.
*    6. Set the Hysteresis Level.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    4. Define the parameters for an Analog Slope Start Trigger.
*    5. Call the Start function to start the acquistion.
*    6. Read the data in a loop until the stop button is pressed or
*       an error occurs.
*    7. Call the Clear Task function to clear the task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. Also, make sure your analog trigger
*    terminal matches the Trigger Source Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

#define CNT_CHANNELS 	1
#define SIZE_BUFFERS 	10000

#define RATE_SAMPLES	10
#define EVENT_SAMPLES 	10

#define TIME_ACQUIRE	10
#define TOTAL_SAMPLES	(RATE_SAMPLES * TIME_ACQUIRE)

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	char        errBuff[2048]={'\0'};

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
#if (CNT_CHANNELS == 2)
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai1","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
#endif

#if 1
	// taskHandle, source, rate, activeEdge, sampleMode, sampsPerChanToAcquire

	// Continue
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",RATE_SAMPLES,DAQmx_Val_Rising,DAQmx_Val_ContSamps,SIZE_BUFFERS));
#else
	// finite : 10 seconds
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",RATE_SAMPLES,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,TOTAL_SAMPLES);
#endif

#if 0
	// Trigger
#if 0
	// Analog
	DAQmxErrChk (DAQmxCfgAnlgEdgeStartTrig(taskHandle,"APFI0",DAQmx_Val_Rising,0.0));
	DAQmxErrChk (DAQmxSetAnlgEdgeStartTrigHyst(taskHandle, 1.0));
#elif 1
	// Digital
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(taskHandle,"PFI0",DAQmx_Val_Rising));
#endif
#endif

	// taskHandle, everyNsamplesEventType, nSamples(PerChan), options, callback, callbackData
	DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,EVENT_SAMPLES,0,EveryNCallback,NULL));
	DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	printf("Acquiring samples continuously. Press Enter to interrupt\n");
	getchar();

	return 0;

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	printf ("EveryNCallback ...\n");

	int32       error=0;
	char        errBuff[2048]={'\0'};
	static int  totalRead=0;
	int32       read=0;

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
#if 0
	float64     data[1000];

	//DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByScanNumber,data,1000,&read,NULL));
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,EVENT_SAMPLES,10.0,DAQmx_Val_GroupByScanNumber,data,sizeof(data),&read,NULL));
	if( read>0 ) {
		//printf("Acquired %d samples. Total %d\r",read,totalRead+=read);
		//fflush(stdout);
		printf("Acquired %d samples. Total %d\n",read,totalRead+=read);
	}
#else
	int32		numBytesPerSamp;
	int16     	data[1000];

	DAQmxErrChk (DAQmxReadRaw(taskHandle,EVENT_SAMPLES,10.0,data,sizeof(data),&read,&numBytesPerSamp,NULL));
	if( read>0 ) {
		printf("Acquired %d samples. Total %d\n",read,totalRead+=read);
	}
#endif

	int	i, j, idx;
	for (i = 0, idx = 0; i < EVENT_SAMPLES; i++) {
		if (!(i % 10)) {
			printf("\n");
		}
		for (j = 0; j < CNT_CHANNELS; j++) {
			printf ("%.1f ", (float)data[idx++]);
		}
	}
	printf("\n");

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	printf ("DoneCallback ...\n");

	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}
