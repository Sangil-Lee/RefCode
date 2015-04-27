#include <stdio.h>
#include <NIDAQmx.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

const uInt32	numberOfChannels=2;
const char*		physicalChannels[]={"Dev2/ai0","Dev2/ai1"};
const char*		channels[]={"Channel0","Channel1"};
const float64	voltageMin[]={-10,-5};
const float64	voltageMax[]={10,5};
const uInt32	numberOfSamplesPerChannel=1000;

int main(void)
{
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       numberOfSampsRead;
	int32		numberOfCoefficients=0;
	float64		average=0.0;
	int16		data[2000];
	float64		scaledData[2000];
	float64*	coefficients;
	char        errBuff[2048]={'\0'};

	uInt32 i;
	uInt32 j;
	int32 k;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/

	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));

	for (i=0; i<numberOfChannels; i++) {
		DAQmxErrChk (DAQmxCreateAIVoltageChan(
			taskHandle,
			physicalChannels[i],		// physical channel name
			channels[i],				// channel name
			DAQmx_Val_RSE,		// terminal configuration
			voltageMin[i],				// channel max and min
			voltageMax[i],					
			DAQmx_Val_Volts,			
			NULL));
	}

	// Obtain the number of coefficients.  All channels in a single task will have the same number of
	// coefficients so we will just look at the first channel.
	// Passing a NULL and a 0 as the array and number of samples respectively
	// causes the function to return the number of samples instead of an error.
	// Positive number errors are not considered errors and will not halt execution
	DAQmxErrChk (numberOfCoefficients = DAQmxGetAIDevScalingCoeff (taskHandle, channels[0], NULL, 0));

	// Allocate Memory for coefficient array
	coefficients=(float64*)(malloc(sizeof(float64)*numberOfCoefficients*numberOfChannels));
	// Check for allocation error
	if (coefficients==NULL) goto Error;
	
	printf ("numberOfCoefficients = %d\n", numberOfCoefficients);

	// Get Coefficients for each channel
	for (i=0; i < numberOfChannels; i++) {
		DAQmxErrChk (DAQmxGetAIDevScalingCoeff(taskHandle, channels[i], 
					coefficients+(numberOfCoefficients*i), numberOfCoefficients));

		printf ("%s : offset(%.10f) ", physicalChannels[i], *(coefficients+(numberOfCoefficients*i)));
		printf ("coeff(");

		for (j=1; j < numberOfCoefficients; j++) {
			printf ("%.4f ", *(coefficients+(numberOfCoefficients*i)+j));
		}
		printf (")\n");
	}

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 )  {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	//printf("End of program, press Enter key to quit\n");
	free ((void*)(coefficients));
	//getchar();
	return 0;
}

