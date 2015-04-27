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

#include "drvNiPci6254.h"
#include <NIDAQmx.h>

#include "nidaq.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

#if 0
#define AI_DEV	"Dev1/ai0:15"
#endif

int32 CVICALLBACK EveryNCallback6254(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback6254(TaskHandle taskHandle, int32 status, void *callbackData);

int NiPci6254Config();

#if 0
int SetDaqStart();
int SetDaqStop();
int SetDaqAbort();
#endif

DaqAiPvt daq6254pvt;

int NiPci6254Config()
{
    int32 error = 0;
    char errBuff[2048];

    daq6254pvt.sampsPerChan = (uInt64)((daq6254pvt.t1 - daq6254pvt.t0) * daq6254pvt.rate);

    epicsPrintf("**** Initialize Task 6254 ****\n");
    DAQmxErrChk(DAQmxCreateTask("", &daq6254pvt.taskHandle));
    DAQmxErrChk(DAQmxCreateAIVoltageChan(daq6254pvt.taskHandle, "Dev2/ai0:15", "", daq6254pvt.terminalConfig, -10.0, 10.0, DAQmx_Val_Volts, NULL));

    if(daq6254pvt.extMode == 1) {
        DAQmxErrChk(DAQmxCfgSampClkTiming(daq6254pvt.taskHandle, "/Dev2/PFI7", daq6254pvt.rate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, daq6254pvt.sampsPerChan));
    }
    else {
        DAQmxErrChk(DAQmxCfgSampClkTiming(daq6254pvt.taskHandle, "", daq6254pvt.rate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, daq6254pvt.sampsPerChan));
    }

    if(daq6254pvt.trigMode == 1) {
        DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(daq6254pvt.taskHandle,"/Dev2/PFI0",DAQmx_Val_Rising));
        DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6254pvt.taskHandle,DAQmx_Val_Acquired_Into_Buffer,daq6254pvt.sampsPerChan,0,EveryNCallback6254,NULL));
        DAQmxErrChk (DAQmxRegisterDoneEvent(daq6254pvt.taskHandle,0,DoneCallback6254,NULL));
    }

    return 0;

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6254pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6254pvt.taskHandle);
        DAQmxClearTask(daq6254pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6254): %s\n",errBuff);

    return -1;
}

int32 CVICALLBACK EveryNCallback6254(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32 error = 0;
    char errBuff[2048];
    int32 read;

    int32 sampsPerChan = daq6254pvt.sampsPerChan;
    int32 buff_size = daq6254pvt.sampsPerChan*16;

    int   fd;

    epicsPrintf("**** NI 6254 Trigger Start ... DAQmxReadAnalogF64 ****\n");

    DAQmxErrChk(DAQmxReadAnalogF64(daq6254pvt.taskHandle, sampsPerChan, 10.0, DAQmx_Val_GroupByChannel, daq6254pvt.data,
                                     buff_size, &read, NULL));

    fd = open(DATAFILE_6254, O_WRONLY | O_CREAT);
    write(fd, daq6254pvt.data, buff_size*8);
    close(fd);

    poloScan.msg = POLOSCAN_END;

    epicsPrintf("**** File Save Byte : %d ****\n", buff_size*8);

    return 0;

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6254pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6254pvt.taskHandle);
        DAQmxClearTask(daq6254pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("DAQmx Error(6254): %s\n",errBuff);

    return -1;
}

int32 CVICALLBACK DoneCallback6254(TaskHandle taskHandle, int32 status, void *callbackData)
{
    int32   error=0;
    char    errBuff[2048]={'\0'};
    DaqQueueData  queueData;

    printf("Read Done!!!\n");
    DAQmxErrChk (status);

    DAQmxErrChk(DAQmxStopTask(daq6254pvt.taskHandle));

    daq6254pvt.status = DAQ_STATUS_STOP;

    queueData.opcode = OP_CODE_DAQ_MDSPLUS;
    epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
Error:
    if( DAQmxFailed(error) ) {
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxClearTask(daq6254pvt.taskHandle);
        printf("DAQmx Error(6254): %s\n",errBuff);
    }
    return -1;
}

void NiPci6254MainThread(void *arg)
{
    int i;
    int ret;
    DaqQueueData pDAQData;

    ret = NiPci6254Config();

    if(ret > 0) {
        epicsPrintf("NiPci6254 Initialize Error\n");
        return;
    }

    epicsPrintf("NiPci6254 Initialize OK\n");

    while(1) {
        epicsMessageQueueReceive(daq6254pvt.DaqQueueId, (void *)&pDAQData, sizeof(DaqQueueData));

        switch(pDAQData.opcode) {
        case OP_CODE_DAQ_RUN:
            if(daq6254pvt.status == DAQ_STATUS_STOP) {
                system("rm -f /tmp/nidaq_6254.dat");
                daq6254pvt.status = DAQ_STATUS_RUN;
                epicsPrintf("OP_CODE_DAQ_RUN\n");

                ret = SetDaqStart(&daq6254pvt);
            }
            break;
        case OP_CODE_DAQ_STOP:  /* Stop Task */
            if(daq6254pvt.status == DAQ_STATUS_RUN) {
                daq6254pvt.status = DAQ_STATUS_STOP;
                epicsPrintf("OP_CODE_DAQ_STOP\n");
            }
            break;
        case OP_CODE_DAQ_CONFIG:
            if(daq6254pvt.status != DAQ_STATUS_RUN) {
                SetDaqAbort(&daq6254pvt);
                NiPci6254Config();
            }
            break;
        case OP_CODE_DAQ_ABORT: /* Task clear and Initialize */
            if(daq6254pvt.status == DAQ_STATUS_RUN) {
                epicsPrintf("**** Abort ****\n");
                SetDaqAbort(&daq6254pvt);
                NiPci6254Config();
            }
            break;
        case OP_CODE_DAQ_MDSPLUS: /* MDSPlus Data Write */
            epicsPrintf("**** MDSPlus Write ****\n");
            daq6254pvt.status = DAQ_STATUS_DONE;
#if 0
            acqMdsPutData(&daq6254pvt);
#endif
            break;
        }

        epicsThreadSleep(0.1);
    }
}

int drvAsynNiPci6254Configure(char *portName, int extMode, int trigMode)
{
    int i;

#if 0
    if(trigMode)
        daq6254pvt.trigName = "/Dev1/PFI1";
    else
        daq6254pvt.trigName = NULL;

    if(extMode)
        daq6254pvt.extName = "/Dev1/PFI7";
    else
        daq6254pvt.extName = NULL;
#endif

    daq6254pvt.trigMode = trigMode;
    daq6254pvt.extMode = extMode;

    daq6254pvt.rate = 10000.0;
    daq6254pvt.sampsPerChan = 10000;
    daq6254pvt.terminalConfig = DAQmx_Val_RSE;
    daq6254pvt.pretriggerSamples = 10000; 
    daq6254pvt.triggerEdge = DAQmx_Val_Rising;
    daq6254pvt.status = DAQ_STATUS_STOP;

    daq6254pvt.t0 = 0;
    daq6254pvt.t1 = 1;
    daq6254pvt.fBlipTime = -16;
    daq6254pvt.mdsput[0] = 1;
    daq6254pvt.mdsput[1] = 1;
    daq6254pvt.mdsput[2] = 1;
    daq6254pvt.mdsput[3] = 1;
    daq6254pvt.mdsput[4] = 1;
    daq6254pvt.mdsput[5] = 1;
    daq6254pvt.mdsput[6] = 1;
    daq6254pvt.mdsput[7] = 1;
    daq6254pvt.mdsput[8] = 1;
    daq6254pvt.mdsput[9] = 1;
    daq6254pvt.mdsput[10] = 1;
    daq6254pvt.mdsput[11] = 1;
    daq6254pvt.mdsput[12] = 1;
    daq6254pvt.mdsput[13] = 1;

    for(i=14; i < 16; i++)
        daq6254pvt.mdsput[i] = 0;

    memcpy(&daq6254pvt.mdsnode[0],"EC1_VAPS\0", 9);
    memcpy(&daq6254pvt.mdsnode[1],"EC1_IAPS\0", 9);
    memcpy(&daq6254pvt.mdsnode[2],"EC1_VBPS\0", 9);
    memcpy(&daq6254pvt.mdsnode[3],"EC1_IBPS\0", 9);
    memcpy(&daq6254pvt.mdsnode[4],"EC1_VCPS\0", 9);
    memcpy(&daq6254pvt.mdsnode[5],"EC1_ICPS\0", 9);
    memcpy(&daq6254pvt.mdsnode[6],"EC1_OCAPS\0", 10);
    memcpy(&daq6254pvt.mdsnode[7],"EC1_RFFWD1\0", 11);
    memcpy(&daq6254pvt.mdsnode[8],"EC1_RFREF1\0", 11);
    memcpy(&daq6254pvt.mdsnode[9],"EC1_RFFWD2\0", 11);
    memcpy(&daq6254pvt.mdsnode[10],"EC1_RFREF2\0", 11);
    memcpy(&daq6254pvt.mdsnode[11],"EC1_VMOU\0", 9);
    memcpy(&daq6254pvt.mdsnode[12],"EC1_VDUM\0", 9);
    memcpy(&daq6254pvt.mdsnode[13],"EC1_VTEE\0", 9);

    printf("[%s]\n", daq6254pvt.mdsnode[0]);
    printf("[%s]\n", daq6254pvt.mdsnode[1]);
    printf("[%s]\n", daq6254pvt.mdsnode[2]);
    printf("[%s]\n", daq6254pvt.mdsnode[3]);
    printf("[%s]\n", daq6254pvt.mdsnode[4]);
    printf("[%s]\n", daq6254pvt.mdsnode[5]);
    printf("[%s]\n", daq6254pvt.mdsnode[6]);
    printf("[%s]\n", daq6254pvt.mdsnode[7]);
    printf("[%s]\n", daq6254pvt.mdsnode[8]);
    printf("[%s]\n", daq6254pvt.mdsnode[9]);
    printf("[%s]\n", daq6254pvt.mdsnode[10]);
    printf("[%s]\n", daq6254pvt.mdsnode[11]);
    printf("[%s]\n", daq6254pvt.mdsnode[12]);
    printf("[%s]\n", daq6254pvt.mdsnode[13]);

    daq6254pvt.DaqQueueId = epicsMessageQueueCreate(50,sizeof(DaqQueueData));

    system("rm -f /tmp/nidaq_6254.dat");
    daq6254pvt.threadId = epicsThreadCreate(portName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)NiPci6254MainThread, NULL);

    return 0;
}

static const iocshArg drvAsynNiPci6254ConfigureArg0 = { "portName",iocshArgString};
static const iocshArg drvAsynNiPci6254ConfigureArg1 = { "extMode",iocshArgInt};
static const iocshArg drvAsynNiPci6254ConfigureArg2 = { "trigMode",iocshArgInt};

static const iocshArg *drvAsynNiPci6254ConfigureArgs[] = {
    &drvAsynNiPci6254ConfigureArg0, &drvAsynNiPci6254ConfigureArg1,
    &drvAsynNiPci6254ConfigureArg2
};
static const iocshFuncDef drvAsynNiPci6254ConfigureFuncDef =
                      {"drvAsynNiPci6254Configure",3,drvAsynNiPci6254ConfigureArgs};
static void drvAsynNiPci6254ConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynNiPci6254Configure(args[0].sval,args[1].ival, args[2].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynNiPci6254RegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynNiPci6254ConfigureFuncDef,drvAsynNiPci6254ConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynNiPci6254RegisterCommands);
