#include <stdio.h>

#include <NIDAQmx.h>
#include "nidaq.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int SetDaqStart(DaqAiPvt *daqpvt)
{
    int32 error = 0;
    char errBuff[2048];
    int32 read;

    epicsPrintf("**** DAQmxStartTask ****\n");

    DAQmxErrChk(DAQmxStartTask(daqpvt->taskHandle));

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daqpvt->taskHandle!=0 ) {
        DAQmxStopTask(daqpvt->taskHandle);
        DAQmxClearTask(daqpvt->taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error: %s\n",errBuff);

    return -1;
}

int SetDaqStop(DaqAiPvt *daqpvt)
{
    int32 error = 0;
    char errBuff[2048];

    epicsPrintf("**** DAQmxStopTask ****\n");

    DAQmxErrChk(DAQmxStopTask(daqpvt->taskHandle));

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daqpvt->taskHandle!=0 ) {
        DAQmxStopTask(daqpvt->taskHandle);
        DAQmxClearTask(daqpvt->taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error: %s\n",errBuff);

    return -1;
}

int SetDaqAbort(DaqAiPvt *daqpvt)
{
    int32 error = 0;
    char errBuff[2048];

    epicsPrintf("**** DAQmx Task Abort****\n");

    DAQmxErrChk(DAQmxStopTask(daqpvt->taskHandle));
    DAQmxErrChk(DAQmxClearTask(daqpvt->taskHandle));

    daqpvt->status = DAQ_STATUS_STOP;

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daqpvt->taskHandle!=0 ) {
        DAQmxStopTask(daqpvt->taskHandle);
        DAQmxClearTask(daqpvt->taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error: %s\n",errBuff);

    return -1;
}

int SetDaqFinshStop(DaqAiPvt *daqpvt)
{
    int32 error = 0;
    char errBuff[2048];

    DAQmxErrChk(DAQmxTaskControl(daqpvt->taskHandle, DAQmx_Val_Task_Abort));
    DAQmxErrChk(DAQmxStopTask(daqpvt->taskHandle));

    daqpvt->status = DAQ_STATUS_STOP;

    return 0;

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daqpvt->taskHandle!=0 ) {
        DAQmxStopTask(daqpvt->taskHandle);
        DAQmxClearTask(daqpvt->taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error: %s\n",errBuff);

    return -1;
}
