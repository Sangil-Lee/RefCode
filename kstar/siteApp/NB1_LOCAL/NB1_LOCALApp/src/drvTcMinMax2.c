#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

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

#include <cadef.h>

#include "TcMinMax2.h"

int m_TcnMaxInd2;
int m_TcnMinInd2;
float m_TcfMax2;
float m_TcfMin2;
TCMINMAX2 tcMinMax2;

/*
typedef struct {
    int  ok[200];
    char minmax[200][50];
} TCMINMAX;

TCMINMAX tcMinMax;
*/

static void GetMessageTcMinMax2Thread(void *lParam)
{
    int loop = 0;
    int i;
    int nMaxInd = 0;
    int nMinInd = 0;
    int status;
    float fMax;
    float fMin;
    char bufLineScan[50];
    char buff[50];
    double data;
    chid   mychid[TC_MAX_CHAN2];
    FILE *fp;

    epicsPrintf("-- Min Max Thread Start!! --\n");

    fp = fopen(MINMAX_CONFIG2, "r");
    if(fp == NULL)
    {
        epicsPrintf("TC Min Max Config file Read Faile...!!!\n");
        return;
    }

    i = 0;
    while(1)
    {
        if(fgets(bufLineScan, 50, fp) == NULL) break;
        
        if(bufLineScan[0] == '#') ;
        else if(bufLineScan[0] == ' ') ;
        else {
            sscanf(bufLineScan, "%s", buff);
            tcMinMax2.ok[i] = 1;
            strcpy(tcMinMax2.minmax[i], buff);
            
            i++;
            if(i >= TC_MAX_CHAN2) break;
        }
    }

    loop = i;

    epicsPrintf("-- Min Max Total PV Count [%d] --\n", loop);

/*
    ca_context_create(ca_disable_preemptive_callback);
*/
    ca_context_create(ca_enable_preemptive_callback);

    for(i =0; i < loop; i++) {
        status = ca_create_channel(tcMinMax2.minmax[i],NULL,NULL,0,&mychid[i]);
        ca_pend_io(1.0);
        if(status != ECA_NORMAL) {
            epicsPrintf("ca_create_channel faile...\n");
        }
    }

    while(1)
    {
        /* ca get */
        fMax = 0.0;
        fMin = 9999;
        for(i=0; i < loop; i++) {
            data = 0.0;
            status = ca_get(DBR_DOUBLE,mychid[i],(void *)&data);
            ca_pend_io(1.0);

            if(status != ECA_NORMAL) {
                epicsPrintf("-- TC Min MAX Get Error : %s --\n", tcMinMax2.minmax[i]);
            }
            else {
                if(i == 0) {
                    fMax = data;
                    fMin = data;
                }
                if(fMax < data) {
                    fMax = data;
                    nMaxInd = i;
                }
                if(fMin > data) {
                    fMin = data;
                    nMinInd = i;
                }
            }
        }

        m_TcfMax2 = fMax;
        m_TcfMin2 = fMin;
        m_TcnMaxInd2 = nMaxInd;
        m_TcnMinInd2 = nMinInd;
        
#if 0
        epicsPrintf("-- Max     : %f --\n", m_TcfMax);
        epicsPrintf("-- Max PV  : %s --\n", tcMinMax.minmax[m_TcnMaxInd]);
        epicsPrintf("-- Min     : %f --\n", m_TcfMin);
        epicsPrintf("-- Min PV  : %s --\n", tcMinMax.minmax[m_TcnMinInd]);
#endif
        epicsThreadSleep(1.0);
    }
}

int
drvAsynTcMinMax2Configure(char *portName)
{
    char thName[256];

    sprintf(thName, "TC_%s", portName);

    epicsPrintf("-- TC Min Max 2 Start --\n");
    epicsThreadCreate(thName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageTcMinMax2Thread, NULL);

    return 0;
}

static const iocshArg drvAsynTcMinMax2ConfigureArg0 = { "port name",iocshArgString};
static const iocshArg *drvAsynTcMinMax2ConfigureArgs[] = {
    &drvAsynTcMinMax2ConfigureArg0};
static const iocshFuncDef drvAsynTcMinMax2ConfigureFuncDef =
                      {"drvAsynTcMinMax2Configure",1,drvAsynTcMinMax2ConfigureArgs};
static void drvAsynTcMinMax2ConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynTcMinMax2Configure(args[0].sval);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynTcMinMax2RegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynTcMinMax2ConfigureFuncDef,drvAsynTcMinMax2ConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynTcMinMax2RegisterCommands);
