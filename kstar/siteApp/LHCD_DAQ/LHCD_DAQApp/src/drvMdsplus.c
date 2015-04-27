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

DaqIntegralPvt daqIntegral;

void DeltaT_Integral(DaqAiPvt *daqpvt, double *rawData, int ch)
{
    int i;
    int j;
    int nIndex = 0;
    int nNumSampleCnt = daqpvt->sampsPerChan;
    float fRealTimeGap;
    float integral = 0.0;
    int integral_t = 0;
    int integral_sample = 0;
    float fConstant = 0.0697;
    int integral_s = 0;
	double tempVal = 0.0;

#if 0
    double getData[nNumSampleCnt];
#else
    double *getData;

    getData = (double *)malloc(sizeof(double)*nNumSampleCnt);
#endif

    fRealTimeGap = 1.0 / (float64)daqpvt->rate;

    integral_s = (int)(daqIntegral.integral_s * (int)daqpvt->rate);

    integral = 0.0;
    integral_sample = (int)(daqIntegral.integral_t[ch] * (int)daqpvt->rate);

    printf("integral Time : CH%d, INT_START : [%d], INT_SAMPLE : [%d], Sample Count : [%d]\n", ch, integral_s, integral_sample, nNumSampleCnt);

    getData[0] = (rawData[integral_s]*4.0);

	for(i=0; i<10; i++) {
        tempVal += rawData[integral_s+i];
    }

	getData[0] = ((tempVal)/10) * 4.0;

    for(j=integral_s+1, i=1; j < integral_sample; j++, i++) {
        getData[i] = (rawData[j]*4.0);
        integral += (float)((getData[i]-getData[0]) * fRealTimeGap);
    }

#if 0
    for(i=integral_s; i < integral_sample; i++) {
        printf("-- [%f] --\n", rawData[i]);
    }
#endif

    daqIntegral.integral_val[ch] = integral;
    daqIntegral.pw[ch] = (integral * daqIntegral.lpm[ch] * fConstant)/daqIntegral.beam_pw;

    printf("[%s] Delta T : Power [%f], lpm [%f], Beam PW [%f], Integral [%f] ----\n", daqpvt->mdsnode[ch], daqIntegral.pw[ch], daqIntegral.lpm[ch], daqIntegral.beam_pw, integral);

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
                ret = acqMdsPutData(&daq6254pvt, KSTAR_MDS);
                ret = acqMdsPutData(&daq6254pvt, LOCAL_MDS);
            }
            else {
                printf("** MDSPlus NI6254 Write **\n");
                ret = acqMdsPutData(&daq6254pvt, LOCAL_MDS);
            }
            if(ret == 0) {
				printf("*** MDSPlus NI 6254 Write OK ***\n");
                daq6254pvt.status = DAQ_STATUS_MDSPLUS;
            }
            else {
                printf("** Mdsplus Put Error... NIDAQ 6254 **\n");
                daq6254pvt.status = DAQ_STATUS_MDSPLUS;
            }
        }
        else if(daq6220pvt.status == DAQ_STATUS_DONE) {
            if(KstarMode) {
                ret = acqMdsPutData(&daq6220pvt, KSTAR_MDS);
                ret = acqMdsPutData(&daq6220pvt, LOCAL_MDS);
            }
            else {
                printf("** MDSPlus NI6220 Write **\n");
                ret = acqMdsPutData(&daq6220pvt, LOCAL_MDS);
            }

            if(ret == 0) {
				printf("*** MDSPlus NI 6220 Write OK ***\n");
                daq6220pvt.status = DAQ_STATUS_MDSPLUS;
            }
            else {
                printf("** Mdsplus Put Error... NIDAQ 6220 **\n");
                daq6220pvt.status = DAQ_STATUS_MDSPLUS;
            }
        }
		else if(daq6254pvt.status == DAQ_STATUS_MDSPLUS && daq6220pvt.status == DAQ_STATUS_MDSPLUS) {
            Mds_LocalShot++;
            epicsPrintf("*** Mds LocalShot Next Number : %d ***\n", Mds_LocalShot);
            daq6254pvt.status = DAQ_STATUS_STOP;
            daq6220pvt.status = DAQ_STATUS_STOP;
        }

        epicsThreadSleep(0.1);
    }
}

int drvAsynMdsplusConfigure(int start)
{
    int i;

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
