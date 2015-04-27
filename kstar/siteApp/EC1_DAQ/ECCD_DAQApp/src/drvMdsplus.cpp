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

#include <NIDAQmx.h>
#include "drvMdsplus.h"

#include "nidaq.h"

int mdsShot = 0;
int Mds_LocalShot = 0;
int Mds_KstarShot = 0;
int KstarMode = 0;

DaqGainPvt daqGainPvt;
DaqIntegralPvt daqIntegral;

void DeltaT_Integral(DaqAiPvt *daqpvt)
{
    int i;
    int j;
    int nIndex = 0;
    int nNumSampleCnt = daqpvt->sampsPerChan;
    float64 *getData;
    float fRealTimeGap;
    float integral = 0.0;
    int integral_t = 0;
    int integral_sample = 0;
    float fConstant = 0.0697;
    int integral_s = 0;

    getData = (float64 *)malloc(sizeof(float64)*nNumSampleCnt);
    fRealTimeGap = 1.0 / (float64)daqpvt->rate;

    integral_s = (int)(daqIntegral.integral_s * (int)daqpvt->rate);
    for(i=0; i < 8; i++) {
        integral = 0.0;
        integral_sample = (int)(daqIntegral.integral_t[i] * (int)daqpvt->rate);
        printf("integral Time : %d, %d\n", i, integral_sample);
        getData[0] = (daqpvt->data[nIndex+integral_s]*4.0);
        for(j=integral_s+1; j < integral_sample; j++) {
            getData[j] = (daqpvt->data[j+nIndex]*4.0);
            integral += (float)((getData[j]-getData[0]) * fRealTimeGap);
        }

        daqIntegral.pw[i] = (integral * daqIntegral.lpm[i] * fConstant)/daqIntegral.beam_pw;

        printf("---- Delta T : Power [%f], lpm [%f], Beam PW [%f] ----\n", daqIntegral.pw[i], daqIntegral.lpm[i], daqIntegral.beam_pw);

        nIndex += daqpvt->sampsPerChan;
    }

    free(getData);
    getData = NULL;
}

void MdsplusThread(void *arg)
{
    int i;
    int ret;

    while(1) {
        if(daq6254pvt.status == DAQ_STATUS_DONE) {
            if(KstarMode) {
                ret = acqMdsPutDataKSTAR(&daq6254pvt);
                ret = acqMdsPutData(&daq6254pvt);
            }
            else {
                ret = acqMdsPutData(&daq6254pvt);
            }
            if(ret == 0) daq6254pvt.status = DAQ_STATUS_MDSPLUS;
        }
        else if(daq6220pvt.status == DAQ_STATUS_DONE) {
            if(KstarMode) {
                ret = acqMdsPutDataKSTAR(&daq6220pvt);
                ret = acqMdsPutData(&daq6220pvt);
            }
            else {
                ret = acqMdsPutData(&daq6220pvt);
            }
            DeltaT_Integral(&daq6220pvt);
            if(ret == 0) daq6220pvt.status = DAQ_STATUS_MDSPLUS;
        }
        else if(poloScan.msg == POLOSCAN_MDS) {
            acqMdsPoloPutData(&poloScan);
            poloScan.msg = POLOSCAN_DONE;
        }
        if(daq6254pvt.status == DAQ_STATUS_MDSPLUS && daq6220pvt.status == DAQ_STATUS_MDSPLUS /*&& poloScan.msg == POLOSCAN_DONE*/) {
            Mds_LocalShot++;
            epicsPrintf("*** Mds LocalShot Next Number : %d ***\n", Mds_LocalShot);
            daq6254pvt.status = DAQ_STATUS_STOP;
            daq6220pvt.status = DAQ_STATUS_STOP;
            poloScan.daqReady = 0;
        }

        epicsThreadSleep(0.1);
    }
}

int drvAsynMdsplusConfigure(int start)
{
    int i;

    for(i=0; i < MAX_GAIN; i++) {
        daqGainPvt.gain[i] = 0.0;
        daqGainPvt.offset[i] = 0.0;
    }

    memcpy(&daqGainPvt.mdsnode[0], "EC1_VAPS:\0", 10);
    memcpy(&daqGainPvt.mdsnode[1], "EC1_IAPS:\0", 10);
    memcpy(&daqGainPvt.mdsnode[2], "EC1_VBPS:\0", 10);
    memcpy(&daqGainPvt.mdsnode[3], "EC1_IBPS:\0", 10);
    memcpy(&daqGainPvt.mdsnode[4], "EC1_VCPS:\0", 10);
    memcpy(&daqGainPvt.mdsnode[5], "EC1_ICPS:\0", 10);
    memcpy(&daqGainPvt.mdsnode[6], "EC1_OCAPS:\0", 11);
    memcpy(&daqGainPvt.mdsnode[7], "EC1_RFFWD1:\0", 12);
    memcpy(&daqGainPvt.mdsnode[8], "EC1_RFREF1:\0", 12);
    memcpy(&daqGainPvt.mdsnode[9], "EC1_RFFWD2:\0", 12);
    memcpy(&daqGainPvt.mdsnode[10], "EC1_RFREF2:\0", 12);

    for(i=0; i<MAX_GAIN; i++)
        printf("GAIN : [%s]\n", daqGainPvt.mdsnode[i]);

    epicsThreadCreate("MDSP",
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)MdsplusThread, NULL);

    return 0;
}

static const iocshArg drvAsynMdsplusConfigureArg0 = { "start",iocshArgInt};

static const iocshArg *drvAsynMdsplusConfigureArgs[] = {
    &drvAsynMdsplusConfigureArg0
};
static const iocshFuncDef drvAsynMdsplusConfigureFuncDef =
                      {"drvAsynMdsplusConfigure",1,drvAsynMdsplusConfigureArgs};
static void drvAsynMdsplusConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynMdsplusConfigure(args[0].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynMdsplusRegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynMdsplusConfigureFuncDef,drvAsynMdsplusConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynMdsplusRegisterCommands);
