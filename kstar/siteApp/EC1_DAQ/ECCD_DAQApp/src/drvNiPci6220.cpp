#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include <arpa/inet.h>

#include <osiUnistd.h>
#include <cantProceed.h>
#include <errlog.h>
#include <iocsh.h>
#include <epicsAssert.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <epicsTimer.h>
#include <osiUnistd.h>

#include <epicsExport.h>

#include "drvNiPci6220.h"
#include <NIDAQmx.h>

#include "nidaq.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

#if 0
#define AI_DEV	"Dev2/ai0:15"
#endif

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int NiPci6220Config();

#if 0
int SetDaqStart();
int SetDaqStop();
int SetDaqAbort();
#endif

DaqAiPvt daq6220pvt;

int NiPci6220Config()
{
    int32 error = 0;
    char errBuff[2048];

    daq6220pvt.sampsPerChan = (uInt64)((daq6220pvt.t1 - daq6220pvt.t0) * daq6220pvt.rate);

    epicsPrintf("**** Initialize Task 6220 ****\n");
    epicsPrintf("**** 6220 : sampsPerChan = %d****\n", daq6220pvt.sampsPerChan);
    DAQmxErrChk(DAQmxCreateTask("", &daq6220pvt.taskHandle));
    DAQmxErrChk(DAQmxCreateAIVoltageChan(daq6220pvt.taskHandle, "Dev3/ai0:15","", daq6220pvt.terminalConfig, -10.0, 10.0, DAQmx_Val_Volts, NULL));

    if(daq6220pvt.extMode == 1) {
        DAQmxErrChk(DAQmxCfgSampClkTiming(daq6220pvt.taskHandle, "/Dev3/PFI7", daq6220pvt.rate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, daq6220pvt.sampsPerChan));
    }
    else {
        DAQmxErrChk(DAQmxCfgSampClkTiming(daq6220pvt.taskHandle, "", daq6220pvt.rate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, daq6220pvt.sampsPerChan));
    }

    if(daq6220pvt.trigMode == 1) {
        DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(daq6220pvt.taskHandle,"/Dev3/PFI0",DAQmx_Val_Rising));
        DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6220pvt.taskHandle,DAQmx_Val_Acquired_Into_Buffer,daq6220pvt.sampsPerChan,0,EveryNCallback,NULL));
        DAQmxErrChk (DAQmxRegisterDoneEvent(daq6220pvt.taskHandle,0,DoneCallback,NULL));
    }

    return 0;

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6220pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6220pvt.taskHandle);
        DAQmxClearTask(daq6220pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6220): %s\n",errBuff);

    return -1;
}

#if 0
int SetDaqStart()
{
    int32 error = 0;
    char errBuff[2048];
    int32 read;

    epicsPrintf("**** DAQmxStartTask ****\n");

    DAQmxErrChk(DAQmxStartTask(daq6220pvt.taskHandle));

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6220pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6220pvt.taskHandle);
        DAQmxClearTask(daq6220pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6220): %s\n",errBuff);

    return -1;
}

int SetDaqStop()
{
    int32 error = 0;
    char errBuff[2048];

    epicsPrintf("**** DAQmxStopTask ****\n");

    DAQmxErrChk(DAQmxStopTask(daq6220pvt.taskHandle));

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6220pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6220pvt.taskHandle);
        DAQmxClearTask(daq6220pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6220): %s\n",errBuff);

    return -1;
}

int SetDaqAbort()
{
    int32 error = 0;
    char errBuff[2048];

    epicsPrintf("**** DAQmx Task Abort****\n");

    DAQmxErrChk(DAQmxStopTask(daq6220pvt.taskHandle));
    DAQmxErrChk(DAQmxClearTask(daq6220pvt.taskHandle));

    daq6220pvt.status = DAQ_STATUS_STOP;

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6220pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6220pvt.taskHandle);
        DAQmxClearTask(daq6220pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6220): %s\n",errBuff);

    return -1;
}
#endif

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32 error = 0;
    char errBuff[2048];
    int32 read;

    int32 sampsPerChan = daq6220pvt.sampsPerChan;
    int32 buff_size = daq6220pvt.sampsPerChan*16;

    int   fd;

    epicsPrintf("**** DAQmxReadAnalogF64 ****\n");

    DAQmxErrChk(DAQmxReadAnalogF64(daq6220pvt.taskHandle, sampsPerChan, 10.0, DAQmx_Val_GroupByChannel, daq6220pvt.data,
                                     buff_size, &read, NULL));

    fd = open(DATAFILE_6220, O_WRONLY | O_CREAT);
    write(fd, daq6220pvt.data, buff_size*8);
    close(fd);

    epicsPrintf("**** File Save Byte : %d ****\n", buff_size*8);

    return 0;

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6220pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6220pvt.taskHandle);
        DAQmxClearTask(daq6220pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6220): %s\n",errBuff);

    return -1;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
    int32   error=0;
    char    errBuff[2048]={'\0'};
    DaqQueueData  queueData;

    printf("Read Done!!!\n");
    DAQmxErrChk (status);

    DAQmxErrChk(DAQmxStopTask(daq6220pvt.taskHandle));

    daq6220pvt.status = DAQ_STATUS_STOP;

    queueData.opcode = OP_CODE_DAQ_MDSPLUS;
    epicsMessageQueueSend(daq6220pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
Error:
    if( DAQmxFailed(error) ) {
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxClearTask(daq6220pvt.taskHandle);
        printf("DAQmx Error(6220): %s\n",errBuff);
    }
    return -1;
}

void NiPci6220MainThread(void *arg)
{
    int i;
    int ret;
    DaqQueueData pDAQData;

    ret = NiPci6220Config();

    if(ret > 0) {
        epicsPrintf("NiPci6220 Initialize Error\n");
        return;
    }

    epicsPrintf("NiPci6220 Initialize OK\n");

    while(1) {
        epicsMessageQueueReceive(daq6220pvt.DaqQueueId, (void *)&pDAQData, sizeof(DaqQueueData));
    epicsPrintf("NiPci6220 received message : %d\n", pDAQData.opcode);

        switch(pDAQData.opcode) {
        case OP_CODE_DAQ_RUN:
            if(daq6220pvt.status == DAQ_STATUS_STOP) {
                system("rm -f /tmp/nidaq_6220.dat");
                daq6220pvt.status = DAQ_STATUS_RUN;
                epicsPrintf("NI6220 OP_CODE_DAQ_RUN\n");

                ret = SetDaqStart(&daq6220pvt);
            }
            break;
        case OP_CODE_DAQ_STOP:  /* Stop Task */
            if(daq6220pvt.status == DAQ_STATUS_RUN) {
                daq6220pvt.status = DAQ_STATUS_STOP;
                epicsPrintf("NI6220 OP_CODE_DAQ_STOP\n");
            }
            break;
        case OP_CODE_DAQ_CONFIG:
            if(daq6220pvt.status != DAQ_STATUS_RUN) {
                SetDaqAbort(&daq6220pvt);
                NiPci6220Config();
            }
            break;
        case OP_CODE_DAQ_ABORT: /* Task clear and Initialize */
            if(daq6220pvt.status == DAQ_STATUS_RUN) {
                epicsPrintf("**** Abort ****\n");
                SetDaqAbort(&daq6220pvt);
                NiPci6220Config();
            }
            break;
        case OP_CODE_DAQ_MDSPLUS: /* MDSPlus Data Write */
            epicsPrintf("**** MDSPlus Write ****\n");
            daq6220pvt.status = DAQ_STATUS_DONE;
#if 0
            acqMdsPutData(&daq6220pvt);
#endif
            break;
        }

        epicsThreadSleep(0.1);
    }
}

int drvAsynNiPci6220Configure(char *portName, int extMode, int trigMode)
{
    int i;

    daq6220pvt.trigMode = trigMode;
    daq6220pvt.extMode = extMode;

    daq6220pvt.rate = 10000.0;
    daq6220pvt.sampsPerChan = 10000;
    daq6220pvt.terminalConfig = DAQmx_Val_RSE;
    daq6220pvt.pretriggerSamples = 10000; 
    daq6220pvt.triggerEdge = DAQmx_Val_Rising;
    daq6220pvt.status = DAQ_STATUS_STOP;

    daq6220pvt.t0 = 0;
    daq6220pvt.t1 = 1;
    daq6220pvt.fBlipTime = -16;
    daq6220pvt.mdsput[0] = 1;
    daq6220pvt.mdsput[1] = 1;
    daq6220pvt.mdsput[2] = 1;
    daq6220pvt.mdsput[3] = 1;
    daq6220pvt.mdsput[4] = 1;
    daq6220pvt.mdsput[5] = 1;
    daq6220pvt.mdsput[6] = 1;
    daq6220pvt.mdsput[7] = 1;
    daq6220pvt.mdsput[8] = 1;
    daq6220pvt.mdsput[9] = 1;
    daq6220pvt.mdsput[10] = 1;

    for(i=11; i < 16; i++)
        daq6220pvt.mdsput[i] = 0;

    memcpy(&daq6220pvt.mdsnode[0],"EC1_TCLC\0", 9);
    memcpy(&daq6220pvt.mdsnode[1],"EC1_TCLM\0", 9);
    memcpy(&daq6220pvt.mdsnode[2],"EC1_TPRL\0", 9);
    memcpy(&daq6220pvt.mdsnode[3],"EC1_TSWD\0", 9);
    memcpy(&daq6220pvt.mdsnode[4],"EC1_TDCBR\0", 10);
    memcpy(&daq6220pvt.mdsnode[5],"EC1_TBTUN\0", 10);
    memcpy(&daq6220pvt.mdsnode[6],"EC1_TRAD\0", 9);
    memcpy(&daq6220pvt.mdsnode[7],"EC1_TMIRR\0", 10);
    memcpy(&daq6220pvt.mdsnode[8],"EC1_VION1\0", 10);
    memcpy(&daq6220pvt.mdsnode[9],"EC1_VION2\0", 10);
    memcpy(&daq6220pvt.mdsnode[10],"EC1_VION3\0", 10);

    printf("[%s]\n", daq6220pvt.mdsnode[0]);
    printf("[%s]\n", daq6220pvt.mdsnode[1]);
    printf("[%s]\n", daq6220pvt.mdsnode[2]);
    printf("[%s]\n", daq6220pvt.mdsnode[3]);
    printf("[%s]\n", daq6220pvt.mdsnode[4]);
    printf("[%s]\n", daq6220pvt.mdsnode[5]);
    printf("[%s]\n", daq6220pvt.mdsnode[6]);
    printf("[%s]\n", daq6220pvt.mdsnode[7]);
    printf("[%s]\n", daq6220pvt.mdsnode[8]);
    printf("[%s]\n", daq6220pvt.mdsnode[9]);
    printf("[%s]\n", daq6220pvt.mdsnode[10]);

    daq6220pvt.DaqQueueId = epicsMessageQueueCreate(50,sizeof(DaqQueueData));

    system("rm -f /tmp/nidaq_6220.dat");
    daq6220pvt.threadId = epicsThreadCreate(portName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)NiPci6220MainThread, NULL);

    return 0;
}

static const iocshArg drvAsynNiPci6220ConfigureArg0 = { "portName",iocshArgString};
static const iocshArg drvAsynNiPci6220ConfigureArg1 = { "extMode",iocshArgInt};
static const iocshArg drvAsynNiPci6220ConfigureArg2 = { "trigMode",iocshArgInt};

static const iocshArg *drvAsynNiPci6220ConfigureArgs[] = {
    &drvAsynNiPci6220ConfigureArg0, &drvAsynNiPci6220ConfigureArg1,
    &drvAsynNiPci6220ConfigureArg2
};
static const iocshFuncDef drvAsynNiPci6220ConfigureFuncDef =
                      {"drvAsynNiPci6220Configure",3,drvAsynNiPci6220ConfigureArgs};
static void drvAsynNiPci6220ConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynNiPci6220Configure(args[0].sval,args[1].ival, args[2].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynNiPci6220RegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynNiPci6220ConfigureFuncDef,drvAsynNiPci6220ConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynNiPci6220RegisterCommands);
