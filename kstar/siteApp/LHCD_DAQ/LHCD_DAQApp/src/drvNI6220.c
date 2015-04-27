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

#include "drvNI6220.h"

#include "nidaq.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK EveryNCallback6220(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);

int NiPci6220Config();

DaqAiPvt daq6220pvt;

static int NiPci6220Exit(void)
{
    SetDaqAbort(&daq6220pvt);
    printf("NI 6220 Exit....\n");

    return 0;
}

int NiPci6220Config()
{
    int32 error = 0;
    char errBuff[2048];

    daq6220pvt.sampsPerChan = (uInt64)((daq6220pvt.t1 - daq6220pvt.t0) * daq6220pvt.rate);

    epicsPrintf("**** Initialize Task 6220 [%d] ****\n", daq6220pvt.sampsPerChan);
    DAQmxErrChk(DAQmxCreateTask("", &daq6220pvt.taskHandle));

    DAQmxErrChk(DAQmxCreateAIVoltageChan(daq6220pvt.taskHandle, 
                                         daq6220pvt.devName, 
                                         "", 
                                         daq6220pvt.terminalConfig, 
                                         -10.0, 
                                         10.0, 
                                         DAQmx_Val_Volts, 
                                         NULL));

    DAQmxErrChk(DAQmxCfgSampClkTiming(daq6220pvt.taskHandle, 
                                          "", 
                                          daq6220pvt.rate, 
                                          DAQmx_Val_Rising, 
                                          daq6220pvt.smp_mode, 
                                          daq6220pvt.sampsPerChan));

    if(daq6220pvt.trigMode == 1) {
        DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(daq6220pvt.taskHandle,
                                              daq6220pvt.trgName,
                                              DAQmx_Val_Rising));

        if(daq6220pvt.smp_mode == CONT_MODE) {
            DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6220pvt.taskHandle,
                                                         DAQmx_Val_Acquired_Into_Buffer,
                                                         daq6220pvt.rate,
                                                         0,
                                                         EveryNCallback6220,
                                                         NULL));
        }
        else {
            DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(daq6220pvt.taskHandle,
                                                         DAQmx_Val_Acquired_Into_Buffer,
                                                         daq6220pvt.sampsPerChan,
                                                         0,
                                                         EveryNCallback6220,
                                                         NULL));
        }

#if 0
        DAQmxErrChk (DAQmxRegisterDoneEvent(daq6220pvt.taskHandle,0,DoneCallback6220,NULL));
#endif
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

int32 CVICALLBACK EveryNCallback6220(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32 error = 0;
    char errBuff[2048];
    int32 read;

    int32 sampsPerChan = daq6220pvt.rate;
    int32 buff_size = daq6220pvt.rate*16;

    int	i, j, ind;
    float64        *data;

    data = (float64 *)malloc(sizeof(float64)*buff_size);

    DAQmxErrChk(DAQmxReadAnalogF64(daq6220pvt.taskHandle, sampsPerChan, 10.0, DAQmx_Val_GroupByChannel, data,
                                     buff_size, &read, NULL));

    ind = 0;
    daq6220pvt.totalRead += sampsPerChan;

    for(i=0; i < 16; i++) {
        if(daq6220pvt.mdsput[i]) {
            if(daq6220pvt.fp[i] != NULL) {
                ind = i*sampsPerChan;
                fwrite(&data[ind], sizeof(float64)*sampsPerChan, 1, daq6220pvt.fp[i]);
                fflush(daq6220pvt.fp[i]);

                if(daq6220pvt.totalRead >= daq6220pvt.sampsPerChan) {
                    fclose(daq6220pvt.fp[i]);
                    daq6220pvt.fp[i] = NULL;
                    epicsPrintf("*** data file save finish... ***\n");
                }
            }
        }
    }

    if(daq6220pvt.totalRead >= daq6220pvt.sampsPerChan) {
        DaqQueueData queueData;

        daq6220pvt.totalRead = 0;

        SetDaqFinshStop(&daq6220pvt);
        daq6220pvt.status = DAQ_STATUS_STOP;

        queueData.opcode = OP_CODE_DAQ_MDSPLUS;
        epicsMessageQueueSend(daq6220pvt.DaqQueueId, (void *)&queueData, sizeof(DaqQueueData));
    }

    free(data);
    data = NULL;

    epicsPrintf("**** NI 6220 File Save Byte : %ld ****\n", buff_size*8);

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

void NiPci6220MainThread(void *arg)
{
    int i;
    int ret;
    char szFile[256];
    DaqQueueData pDAQData;

    ret = NiPci6220Config();

    epicsAtExit((EPICSEXITFUNC) NiPci6220Exit, NULL);

    if(ret > 0) {
        epicsPrintf("NiPci6220 Initialize Error\n");
        return;
    }

    epicsPrintf("NiPci6220 Initialize OK\n");

    while(1) {
        epicsMessageQueueReceive(daq6220pvt.DaqQueueId, (void *)&pDAQData, sizeof(DaqQueueData));

        switch(pDAQData.opcode) {
        case OP_CODE_DAQ_RUN:
            if(daq6220pvt.status == DAQ_STATUS_STOP) {
                system("rm -f /tmp/nidaq_6220.dat");
                daq6220pvt.status = DAQ_STATUS_RUN;
                epicsPrintf("NI6220 : OP_CODE_DAQ_RUN\n");

                for(i=0; i < 16; i++) {
                    if(daq6220pvt.mdsput[i] == 1 && daq6220pvt.fp[i] == NULL) {
                        sprintf(szFile, "/tmp/%s_%d.dat", daq6220pvt.fileName, i);
                        daq6220pvt.fp[i] = fopen(szFile, "w+");
                    }
                }

                ret = SetDaqStart(&daq6220pvt);
            }
            break;
        case OP_CODE_DAQ_STOP:  /* Stop Task */
            if(daq6220pvt.status == DAQ_STATUS_RUN) {
                daq6220pvt.status = DAQ_STATUS_STOP;
                epicsPrintf("NI6220 : OP_CODE_DAQ_STOP\n");
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
                epicsPrintf("NI6220 : Abort\n");
                SetDaqAbort(&daq6220pvt);
                NiPci6220Config();
            }
            break;
        case OP_CODE_DAQ_MDSPLUS: /* MDSPlus Data Write */
            epicsPrintf("**** NI6220 MDSPlus Write ****\n");
            daq6220pvt.status = DAQ_STATUS_DONE;
            break;
        }

        epicsThreadSleep(0.1);
    }
}

int drvNIPCI6220Setup(char *portName, int ch, char *mds, int gain, int offset)
{
    char szFile[256];

    daq6220pvt.mdsput[ch] = 1;
    daq6220pvt.int_mode[ch] = 1;
    daq6220pvt.gain_mode[ch] = gain;
    daq6220pvt.offset_mode[ch] = offset;

    sprintf(daq6220pvt.fileName, "%s", portName); 
    sprintf(szFile, "/tmp/%s_%d.dat", portName, ch);
    daq6220pvt.fp[ch] = fopen(szFile, "w+");

    if(daq6220pvt.fp[ch] == NULL) {
        epicsPrintf("drvNIPCI6220Setup faile... not open file...\n");
    }

    sprintf(daq6220pvt.mdsnode[ch], "%s", mds);

    epicsPrintf("*** NIPCI6220 Setup [%d] : %s, %s\n", ch, portName, daq6220pvt.mdsnode[ch]);

    return 0;
}

int drvAsynNiPci6220Configure(char *portName, char *trgName, int extMode, int trigMode, int smp_mode)
{
    int i;

    sprintf(daq6220pvt.devName, "%s", portName);
    sprintf(daq6220pvt.trgName, "%s", trgName);

    daq6220pvt.trigMode = trigMode;
    daq6220pvt.extMode = extMode;

    if(smp_mode) /* Cont */
        daq6220pvt.smp_mode = CONT_MODE;
    else
        daq6220pvt.smp_mode = FINITE_MODE;

    daq6220pvt.rate = 1000.0;
    daq6220pvt.sampsPerChan = 1000;
    daq6220pvt.terminalConfig = DAQmx_Val_RSE;
    daq6220pvt.pretriggerSamples = 1000; 
    daq6220pvt.triggerEdge = DAQmx_Val_Rising;
    daq6220pvt.status = DAQ_STATUS_STOP;

    daq6220pvt.totalRead = 0;

    daq6220pvt.t0 = 0;
    daq6220pvt.t1 = 1;
    daq6220pvt.fBlipTime = -16;

    for(i=0; i < 16; i++) {
        daq6220pvt.mdsput[i] = 0;
        daq6220pvt.int_mode[i] = 0;
        daq6220pvt.gain[i] = 0.0;
        daq6220pvt.offset[i] = 0.0;
    }

    daq6220pvt.DaqQueueId = epicsMessageQueueCreate(50,sizeof(DaqQueueData));

    daq6220pvt.threadId = epicsThreadCreate(portName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)NiPci6220MainThread, NULL);

    return 0;
}

static const iocshArg drvAsynNiPci6220ConfigureArg0 = { "portName",iocshArgString};
static const iocshArg drvAsynNiPci6220ConfigureArg1 = { "trgName",iocshArgString};
static const iocshArg drvAsynNiPci6220ConfigureArg2 = { "extMode",iocshArgInt};
static const iocshArg drvAsynNiPci6220ConfigureArg3 = { "trigMode",iocshArgInt};
static const iocshArg drvAsynNiPci6220ConfigureArg4 = { "sample mode",iocshArgInt};

static const iocshArg *drvAsynNiPci6220ConfigureArgs[] = {
    &drvAsynNiPci6220ConfigureArg0, &drvAsynNiPci6220ConfigureArg1,
    &drvAsynNiPci6220ConfigureArg2, &drvAsynNiPci6220ConfigureArg3,
    &drvAsynNiPci6220ConfigureArg4
};
static const iocshFuncDef drvAsynNiPci6220ConfigureFuncDef =
                      {"drvAsynNiPci6220Configure",5,drvAsynNiPci6220ConfigureArgs};
static void drvAsynNiPci6220ConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynNiPci6220Configure(args[0].sval, args[1].sval, args[2].ival, args[3].ival, args[4].ival);
}

static const iocshArg drvNIPCI6220SetupArg0 = {"portName", iocshArgString};
static const iocshArg drvNIPCI6220SetupArg1 = {"ch", iocshArgInt};
static const iocshArg drvNIPCI6220SetupArg2 = {"mds node", iocshArgString};
static const iocshArg drvNIPCI6220SetupArg3 = {"gain", iocshArgInt};
static const iocshArg drvNIPCI6220SetupArg4 = {"offset", iocshArgInt};

static const iocshArg *drvNIPCI6220SetupArgs[] = {
    &drvNIPCI6220SetupArg0, &drvNIPCI6220SetupArg1,
    &drvNIPCI6220SetupArg2, &drvNIPCI6220SetupArg3,
    &drvNIPCI6220SetupArg4
};

static const iocshFuncDef drvNIPCI6220SetupFuncDef = 
                      {"drvNIPCI6220Setup", 5, drvNIPCI6220SetupArgs};

static void drvNIPCI6220SetupCallFunc(const iocshArgBuf *args)
{
    drvNIPCI6220Setup(args[0].sval, args[1].ival, args[2].sval, args[3].ival, args[4].ival);
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
        iocshRegister(&drvNIPCI6220SetupFuncDef, drvNIPCI6220SetupCallFunc);
        firstTime = 0;
    }
}

epicsExportRegistrar(drvAsynNiPci6220RegisterCommands);
