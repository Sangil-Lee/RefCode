#include <stdio.h>
#include <NIDAQmx.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

#if 0
const uInt32	numberOfChannels=2;
const char*		physicalChannels[]={"Dev1/ai0","Dev1/ai1"};
const char*		channels[]={"Channel0","Channel1"};
const float64	voltageMin[]={-10,-5};
const float64	voltageMax[]={10,5};
const uInt32	numberOfSamplesPerChannel=1000;
#else
const uInt32	numberOfChannels=3;
const char*		physicalChannels[]={"Dev1/ai0","Dev1/ai1","Dev1/ai2"};
//const char*		physicalChannels[]={"Dev2/ai0","Dev2/ai1","Dev2/ai2"};
const char*		channels[]={"Channel0","Channel1","Channel2"};
const float64	voltageMin[]={-10,-5,0};
const float64	voltageMax[]={10,5,10};
const uInt32	numberOfSamplesPerChannel=2;
#endif

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
			//DAQmx_Val_Cfg_Default,		// terminal configuration
			//TODO
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
	DAQmxErrChk (numberOfCoefficients=DAQmxGetAIDevScalingCoeff(taskHandle, channels[0], NULL, 0));

	// Allocate Memory for coefficient array
	coefficients=(float64*)(malloc(sizeof(float64)*numberOfCoefficients*numberOfChannels));
	// Check for allocation error
	if (coefficients==NULL) goto Error;
	
	// Get Coefficients for each channel
	for (i=0; i < numberOfChannels; i++) {
		DAQmxErrChk (DAQmxGetAIDevScalingCoeff(taskHandle, channels[i], coefficients+(numberOfCoefficients*i), numberOfCoefficients));

		printf ("numberOfCoefficients = %d\n", numberOfCoefficients);

		printf ("%s : offset(%.10f) ", physicalChannels[i], *(coefficients+(numberOfCoefficients*i)));
		printf ("coeff(");

		for (j=1; j < numberOfCoefficients; j++) {
			printf ("%.10f ", *(coefficients+(numberOfCoefficients*i)+j));
		}
		printf (")\n");
	}
	
	DAQmxErrChk (DAQmxCfgSampClkTiming(
			taskHandle,
			"",
			//10000.0,						// rate
			1.0,						// rate
			DAQmx_Val_Rising,				// sample clock edge
			DAQmx_Val_FiniteSamps,			// acquisition mode
			numberOfSamplesPerChannel));	// samples to acquire per channel

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadBinaryI16(
			taskHandle,									
			numberOfSamplesPerChannel,					
			10.0,											// timeout
			DAQmx_Val_GroupByChannel,
			data,											// analog data
			numberOfSamplesPerChannel*numberOfChannels,		// buffer size
			&numberOfSampsRead,
			NULL));

	printf("Acquired %d points\n",numberOfSampsRead);

	/*********************************************/
	// Scaling and Printing Data
	/*********************************************/

	for ( i=0; i<numberOfChannels; i++) {
		printf("%s Raw : ",channels[i]);
		for ( j=0; j<numberOfSamplesPerChannel; j++ ){
			printf("%d, ", data[i*numberOfSamplesPerChannel+j]);
		}
		printf("\n");
	}

	for ( i=0; i<numberOfChannels; i++) {
		printf("%s Data: ",channels[i]);
		for ( j=0; j<numberOfSamplesPerChannel; j++ ){

			// Set offset as initial value
			scaledData[i*numberOfSamplesPerChannel + j] = coefficients[i*numberOfCoefficients]; 
			printf ("%f ", scaledData[i*numberOfSamplesPerChannel + j]);

			// Calculate gain error with the remaining coefficients (for M Series
			// There are 3.  For E-Series there is only 1.
			for ( k=1; k<numberOfCoefficients; k++){
				scaledData[i*numberOfSamplesPerChannel + j] += 
						coefficients[i*numberOfCoefficients+k]*pow(data[i*numberOfSamplesPerChannel+j],k+0.0);
				printf (" -> %f(%f/%d) \n",
						scaledData[i*numberOfSamplesPerChannel + j], 
						coefficients[i*numberOfCoefficients+k],
						data[i*numberOfSamplesPerChannel+j]);
			}
			printf("\n");
			printf("%f, ",scaledData[i*numberOfSamplesPerChannel+j]);
		}
		printf("\n");
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

