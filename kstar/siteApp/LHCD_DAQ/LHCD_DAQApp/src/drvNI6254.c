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

#include "drvNI6254.h"

#include "nidaq.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK EveryNCallback6254(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);

int NiPci6254Config();
int NiPci6254ConfigNor();

DaqAiPvt daq6254pvt;

static int NiPci6254Exit(void)
{
    SetDaqAbort(&daq6254pvt);
    printf("NI 6254 Exit....\n");

    return 0;
}

int NiPci6254Config()
{
    int32 error = 0;
    char errBuff[2048];

    daq6254pvt.sampsPerChan = (uInt64)((daq6254pvt.t1 - daq6254pvt.t0) * daq6254pvt.rate);

    epicsPrintf("**** Initialize Task 6254 [%d] ****\n", daq6254pvt.sampsPerChan);
/*
    DAQmxErrChk(DAQmxResetDevice(daq6254pvt.devName);
*/
    DAQmxErrChk(DAQmxCreateTask("", &daq6254pvt.taskHandle));

    DAQmxErrChk(DAQmxCreateAIVoltageChan(daq6254pvt.taskHandle, 
                                         daq6254pvt.devName, 
                                         "", 
                                         daq6254pvt.terminalConfig, 
                                         -10.0, 
                                         10.0, 
                                         DAQmx_Val_Volts, 
                                         NULL));

    if(daq6254pvt.extMode == 1) {
        DAQmxErrChk(DAQmxCfgSampClkTiming(daq6254pvt.taskHandle, 
                                          "/Dev1/PFI7", 
                                          daq6254pvt.rate, 
                                          DAQmx_Val_Rising, 
                                          DAQmx_Val_FiniteSamps, 
                                          daq6254pvt.sampsPerChan));
    }
    else {
        DAQmxErrChk(DAQmxCfgSampClkTiming(daq6254pvt.taskHandle, 
                                          "", 
                                          daq6254pvt.rate, 
                                          DAQmx_Val_Rising, 
                                          daq6254pvt.smp_mode, 
                                          daq6254pvt.sampsPerChan));
    }

    if(daq6254pvt.trigMode == 1) {
        DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(daq6254pvt.taskHandle,
                                              daq6254pvt.trgName,
                                              DAQmx_Val_Rising));

        if(daq6254pvt.smp_mode == CONT_MODE) {
            DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6254pvt.taskHandle,
                                                         DAQmx_Val_Acquired_Into_Buffer,
                                                         daq6254pvt.rate,
                                                         0,
                                                         EveryNCallback6254,
                                                         NULL));
        }
        else {
            DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6254pvt.taskHandle,
                                                         DAQmx_Val_Acquired_Into_Buffer,
                                                         daq6254pvt.sampsPerChan,
                                                         0,
                                                         EveryNCallback6254,
                                                         NULL));
        }

#if 0
        DAQmxErrChk (DAQmxRegisterDoneEvent(daq6254pvt.taskHandle,0,DoneCallback6254,NULL));
#endif
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

int NiPci6254ConfigNor()
{
    int32       error=0;
    TaskHandle  taskHandle=0;
    char        errBuff[2048]={'\0'};

    DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(daq6254pvt.taskHandle,daq6254pvt.devName,"",daq6254pvt.terminalConfig,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(daq6254pvt.taskHandle,"",1000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

    DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6254pvt.taskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL));
/*
    DAQmxErrChk (DAQmxRegisterDoneEvent(daq6254pvt.taskHandle,0,DoneCallback,NULL));
*/

    DAQmxErrChk (DAQmxStartTask(daq6254pvt.taskHandle));

    return 0;
Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);

    if( daq6254pvt.taskHandle!=0 ) {
        DAQmxStopTask(daq6254pvt.taskHandle);
        DAQmxClearTask(daq6254pvt.taskHandle);
    }
    if( DAQmxFailed(error) )
        epicsPrintf("NI 6254 -- Nor : DAQmx Error(6254): %s\n",errBuff);

    return -1;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32       error=0;
    char        errBuff[2048]={'\0'};
    static int  totalRead=0;
    int32       read=0;
    float64     data[1000*16];

    DAQmxErrChk (DAQmxReadAnalogF64(daq6254pvt.taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000*16,&read,NULL));

    if( read>0 ) {
        epicsPrintf("CH0: %f\n", data[0]);
    }

Error:
    if( DAQmxFailed(error) ) {
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(daq6254pvt.taskHandle);
        DAQmxClearTask(daq6254pvt.taskHandle);
        printf("NI6254 Nor -- DAQmx Error: %s\n",errBuff);
    }
    return 0;
}

int32 CVICALLBACK EveryNCallback6254(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32 error = 0;
    char errBuff[2048];
    int32 read;

    int32 sampsPerChan = daq6254pvt.rate;
    int32 buff_size = daq6254pvt.rate*16;

    int	i, j, ind;
    float64        *data;

    data = (float64 *)malloc(sizeof(float64)*buff_size);

    DAQmxErrChk(DAQmxReadAnalogF64(daq6254pvt.taskHandle, sampsPerChan, 10.0, DAQmx_Val_GroupByChannel, data,
                                     buff_size, &read, NULL));


    ind = 0;
    daq6254pvt.totalRead += sampsPerChan;

    for(i=0; i < 16; i++) {
        if(daq6254pvt.mdsput[i]) {
            ind = i*sampsPerChan;
            fwrite(&data[ind], sizeof(float64)*sampsPerChan, 1, daq6254pvt.fp[i]);
            fflush(daq6254pvt.fp[i]);

            if(daq6254pvt.totalRead >= daq6254pvt.sampsPerChan) {
                fclose(daq6254pvt.fp[i]);
                daq6254pvt.fp[i] = NULL;
                epicsPrintf("*** data file save finish... ***\n");
            }
        }
    }

    if(daq6254pvt.totalRead >= daq6254pvt.sampsPerChan) {
        DaqQueueData queueData;

        daq6254pvt.totalRead = 0;
        SetDaqFinshStop(&daq6254pvt);
        daq6254pvt.status = DAQ_STATUS_STOP;

        queueData.opcode = OP_CODE_DAQ_MDSPLUS;
        epicsMessageQueueSend(daq6254pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
    }

    free(data);
    data = NULL;

    epicsPrintf("**** NI 6254 File Save Byte : %ld ****\n", buff_size*8);

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

void NiPci6254MainThread(void *arg)
{
    int i;
    int ret;
    char szFile[256];
    DaqQueueData pDAQData;

#if 1
    ret = NiPci6254Config();
#else
    ret = NiPci6254ConfigNor();
#endif

    epicsAtExit((EPICSEXITFUNC) NiPci6254Exit, NULL);

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

                NiPci6254Config();

                epicsPrintf("NI6254 : OP_CODE_DAQ_RUN\n");

                ret = SetDaqStart(&daq6254pvt);
                for(i=0; i < 16; i++) {
                    if(daq6254pvt.mdsput[i] == 1 && daq6254pvt.fp[i] == NULL) {
                        sprintf(szFile, "/tmp/%s_%d.dat", daq6254pvt.fileName, i);
                        daq6254pvt.fp[i] = fopen(szFile, "w+");
                    }
                }
            }
            break;
        case OP_CODE_DAQ_STOP:  /* Stop Task */
            if(daq6254pvt.status == DAQ_STATUS_RUN) {
                daq6254pvt.status = DAQ_STATUS_STOP;
                epicsPrintf("NI6254 : OP_CODE_DAQ_STOP\n");
            }
            break;
        case OP_CODE_DAQ_CONFIG:
            if(daq6254pvt.status != DAQ_STATUS_RUN) {
#if 0
                SetDaqAbort(&daq6254pvt);
                NiPci6254Config();
#endif
            }
            break;
        case OP_CODE_DAQ_ABORT: /* Task clear and Initialize */
            if(daq6254pvt.status == DAQ_STATUS_RUN) {
                epicsPrintf("NI6254 : Abort\n");
#if 1
                SetDaqAbort(&daq6254pvt);
                NiPci6254Config();
#endif
            }
            break;
        case OP_CODE_DAQ_MDSPLUS: /* MDSPlus Data Write */
            epicsPrintf("**** NI6254 MDSPlus Write ****\n");
            daq6254pvt.status = DAQ_STATUS_DONE;
            break;
        }

        epicsThreadSleep(0.1);
    }
}

int drvNIPCI6254Setup(char *portName, int ch, char *mds, int gain, int offset)
{
    char szFile[256];

    daq6254pvt.mdsput[ch] = 1;
    daq6254pvt.gain_mode[ch] = gain;
    daq6254pvt.offset_mode[ch] = offset;

    sprintf(daq6254pvt.fileName, "%s", portName); 
    sprintf(szFile, "/tmp/%s_%d.dat", portName, ch);
    daq6254pvt.fp[ch] = fopen(szFile, "w+");

    if(daq6254pvt.fp[ch] == NULL) {
        epicsPrintf("drvNIPCI6254Setup faile... not open file...\n");
    }

    sprintf(daq6254pvt.mdsnode[ch], "%s", mds);

    epicsPrintf("*** NIPCI6254 Setup [%d] : %s, %s\n", ch, portName, daq6254pvt.mdsnode[ch]);

    return 0;
}

int drvAsynNiPci6254Configure(char *portName, char *trgName, int extMode, int trigMode, int smp_mode)
{
    int i;

    sprintf(daq6254pvt.devName, "%s", portName);
    sprintf(daq6254pvt.trgName, "%s", trgName);

    daq6254pvt.trigMode = trigMode;
    daq6254pvt.extMode = extMode;

    if(smp_mode) /* Cont */
        daq6254pvt.smp_mode = CONT_MODE;
    else
        daq6254pvt.smp_mode = FINITE_MODE;

    daq6254pvt.rate = 10000.0;
    daq6254pvt.sampsPerChan = 10000;
    daq6254pvt.terminalConfig = DAQmx_Val_RSE;
    daq6254pvt.pretriggerSamples = 10000; 
    daq6254pvt.triggerEdge = DAQmx_Val_Rising;
    daq6254pvt.status = DAQ_STATUS_STOP;

    daq6254pvt.totalRead = 0;

    daq6254pvt.t0 = 0;
    daq6254pvt.t1 = 1;
    daq6254pvt.fBlipTime = -16;

    for(i=0; i < 16; i++) {
        daq6254pvt.mdsput[i] = 0;
        daq6254pvt.int_mode[i] = 0;
        daq6254pvt.gain[i] = 0.0;
        daq6254pvt.offset[i] = 0.0;
    }

    daq6254pvt.DaqQueueId = epicsMessageQueueCreate(50,sizeof(DaqQueueData));

    daq6254pvt.threadId = epicsThreadCreate(portName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)NiPci6254MainThread, NULL);

    return 0;
}

static const iocshArg drvAsynNiPci6254ConfigureArg0 = { "portName",iocshArgString};
static const iocshArg drvAsynNiPci6254ConfigureArg1 = { "trgName",iocshArgString};
static const iocshArg drvAsynNiPci6254ConfigureArg2 = { "extMode",iocshArgInt};
static const iocshArg drvAsynNiPci6254ConfigureArg3 = { "trigMode",iocshArgInt};
static const iocshArg drvAsynNiPci6254ConfigureArg4 = { "sample mode",iocshArgInt};

static const iocshArg *drvAsynNiPci6254ConfigureArgs[] = {
    &drvAsynNiPci6254ConfigureArg0, &drvAsynNiPci6254ConfigureArg1,
    &drvAsynNiPci6254ConfigureArg2, &drvAsynNiPci6254ConfigureArg3,
    &drvAsynNiPci6254ConfigureArg4
};
static const iocshFuncDef drvAsynNiPci6254ConfigureFuncDef =
                      {"drvAsynNiPci6254Configure",5,drvAsynNiPci6254ConfigureArgs};
static void drvAsynNiPci6254ConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynNiPci6254Configure(args[0].sval, args[1].sval, args[2].ival, args[3].ival, args[4].ival);
}

static const iocshArg drvNIPCI6254SetupArg0 = {"portName", iocshArgString};
static const iocshArg drvNIPCI6254SetupArg1 = {"ch", iocshArgInt};
static const iocshArg drvNIPCI6254SetupArg2 = {"mds node", iocshArgString};
static const iocshArg drvNIPCI6254SetupArg3 = {"gain", iocshArgInt};
static const iocshArg drvNIPCI6254SetupArg4 = {"offset", iocshArgInt};

static const iocshArg *drvNIPCI6254SetupArgs[] = {
    &drvNIPCI6254SetupArg0, &drvNIPCI6254SetupArg1,
    &drvNIPCI6254SetupArg2, &drvNIPCI6254SetupArg3,
    &drvNIPCI6254SetupArg4
};

static const iocshFuncDef drvNIPCI6254SetupFuncDef = 
                      {"drvNIPCI6254Setup", 5, drvNIPCI6254SetupArgs};

static void drvNIPCI6254SetupCallFunc(const iocshArgBuf *args)
{
    drvNIPCI6254Setup(args[0].sval, args[1].ival, args[2].sval, args[3].ival, args[4].ival);
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
        iocshRegister(&drvNIPCI6254SetupFuncDef, drvNIPCI6254SetupCallFunc);
        firstTime = 0;
    }
}

epicsExportRegistrar(drvAsynNiPci6254RegisterCommands);
