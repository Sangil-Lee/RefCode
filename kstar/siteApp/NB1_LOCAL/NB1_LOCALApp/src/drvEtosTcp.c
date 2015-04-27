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

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "etos_tcp.h"
#include "SoftMdsplus.h"

#define STATUS_OK(status) (((status) & 1) == 1)

int etos_fd;
tagLake lake;
tagVacG vg;
tagMfc  mfc;
tagMfcSet mfc_set_ion;
tagMfcSet mfc_set_neu;

tagMfcSet mfc_set_ion2;
tagMfcSet mfc_set_neu2;

MdsInfo mdsinfo;

int softtrigger = 0;
int mfc_write = 0;
int StartSoftDAQ = 0;

int MdsFinish = 0;

int set_up_etos( char *ip_address )
{
    int sfd;
    struct sockaddr_in server;
    int connect_stat;

    sfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

    server.sin_family = AF_INET;
    server.sin_port = htons( ETOS_PORT );
    server.sin_addr.s_addr = inet_addr(ip_address);

    if( sfd >= 0 )
    {
        connect_stat = connect( sfd, (struct sockaddr *)&server,
                                        sizeof(struct sockaddr_in) );

        if( connect_stat < 0 )
        {
            close( sfd );
            sfd = -1;
        }
    }

    return( sfd );
}

int mds_createNewShot()
{
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
        int shot, op;
        int tstat, len;
        char buf[128];
        int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an      integer */
        int idesc = descr(&dtype_Long, &tstat, &null);

        shot = 3120;

                sprintf(buf, "TCL(\"set tree nbi/shot=-1\")");
                status = MdsValue(buf, &idesc, &null, &len);
                if ( !((status) & 1) )
                {
                        printf("-- TCL Set tree Error --\n");
                        return -1;
                }
                sprintf(buf, "TCL(\"create pulse %d\")", shot);
                status = MdsValue(buf, &idesc, &null, &len);
                if ( !((status) & 1) )
                {
                        printf("-- Create Pulse Error --\n");
                        return -1;
                }
                status = MdsValue("TCL(\"close\")", &idesc, &null, &len);

        return 0;
}

int DataSamplingFunc()
{
    int _null = 0;
    int sampleCnt;
    int count;
    int dtype_Float = DTYPE_FLOAT;
    int dtype_Short = DTYPE_SHORT;
    float *rawDataMfcIon = NULL;
    float *rawDataMfcNeu = NULL;
    float *rawDataFg1 = NULL;
    float *rawDataFg3 = NULL;

    float fStartTime;
    float fEndTime;
    float fRealTimeGap;
    int shot = 3120;
    int status;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc, voltageTDesc;

    int   mdsSock;

    printf("-- MdsDataPut Func --\n");
    sampleCnt = (mdsinfo.t1 - mdsinfo.t0) * 100;
    rawDataMfcIon = (float *)malloc(sizeof(float)*sampleCnt);
    rawDataMfcNeu = (float *)malloc(sizeof(float)*sampleCnt);
    rawDataFg1 = (float *)malloc(sizeof(float)*sampleCnt);
    rawDataFg3 = (float *)malloc(sizeof(float)*sampleCnt);

    count = 0;
    while(1) {
        if(count >= sampleCnt) break;

        rawDataMfcNeu[count] = mfc.val[0];
        rawDataMfcIon[count] = mfc.val[1];
        rawDataFg1[count] = vg.fg[0];
        rawDataFg3[count] = vg.fg[2];

        count++;
 
        epicsThreadSleep(0.01);
    }

    printf("-- MdsConnect --\n");
    mdsSock = MdsConnect("172.17.101.244:8000");

    if(mdsSock == -1) {
        printf("--- MDS Connect Faile ---\n");
        return -1;
    }

    printf("-- mds_createNewShot --\n");
    mds_createNewShot();

    printf("-- MdsOpen --\n");
    status = MdsOpen("nbi", &shot);

    if ( !STATUS_OK(status) )
    {
      fprintf(stderr,"Error opening tree for shot %d.\n",shot);
      printf("%s\n", MdsGetMsg(status));
      return -1;
    }

    fStartTime = (mdsinfo.t0) * 100;
    fEndTime = sampleCnt + fStartTime - 1.0;

    fRealTimeGap = 1.0 / 100.0;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

    rawDataDesc = descr(&dtype_Float, rawDataMfcNeu, &sampleCnt, &_null);

    printf("-- MdsPut1 --\n");
    status = MdsPut("MFC_NEU" , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"sccm\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
                                                &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

    rawDataDesc = descr(&dtype_Float, rawDataMfcIon, &sampleCnt, &_null);

    printf("-- MdsPut2 --\n");
    status = MdsPut("MFC_ION" , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"sccm\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
                                                &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

    rawDataDesc = descr(&dtype_Float, rawDataFg1, &sampleCnt, &_null);

    status = MdsPut("FG1" , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"sccm\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
                                                &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

    rawDataDesc = descr(&dtype_Float, rawDataFg3, &sampleCnt, &_null);

    status = MdsPut("FG3" , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"sccm\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
                                                &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

    status = MdsClose("nbi", &shot);

    free(rawDataMfcNeu);
    free(rawDataMfcIon);
    free(rawDataFg1);
    free(rawDataFg3);

    rawDataMfcNeu = NULL;
    rawDataMfcIon = NULL;
    rawDataFg1 = NULL;
    rawDataFg3 = NULL;

    return 0;
}

static void SoftTriggerThread(void *lParam)
{
    int i;

    mdsinfo.t0 = 0;
    mdsinfo.t1 = 0;

    while(1) {
        if(softtrigger == 1 && StartSoftDAQ == 0) {
            StartSoftDAQ == 1;
            printf("Mds Data Get\n");
            DataSamplingFunc();
        }
        if(softtrigger == 0)
            StartSoftDAQ = 0;

        epicsThreadSleep(0.01);
    }
}

static void GetMessageEtosThread(void *lParam)
{
    char *ipaddr = (char *)lParam;
    int ret;
    int ind = 0;
    char head;

    while(1)
    {
        ret = recv(etos_fd, &head, 1, 0);

#if 1
        if(ret <= 0) {
            close(etos_fd);
            etos_fd = -1;
            printf("Etos Connection Faile\n");
            etos_fd = set_up_etos("172.17.121.123");
            epicsThreadSleep(1);
            continue;
        }
#endif

        if(head == 0x01) {
            ret = recv(etos_fd, &lake, sizeof(lake), 0);
            send(etos_fd, "#1\r\n", 4, 0);
            ind = 1;
        }
        else if(head == 0x02) {
            ret = recv(etos_fd, &vg, sizeof(vg), 0);
            send(etos_fd, "#1\r\n", 4, 0);
            ind = 2;
        }
        else if(head == 0x03) {
            ret = recv(etos_fd, &mfc, sizeof(mfc), 0);
            send(etos_fd, "#1\r\n", 4, 0);
        }

        if(mfc_write == MFC_SET_NEU) {
            mfc_set_neu.head[0] = '!';
            mfc_set_neu.head[1] = '1';
            mfc_set_neu.tail[0] = 0x0d;
            mfc_set_neu.tail[1] = 0x0a;
            send(etos_fd, &mfc_set_neu, sizeof(mfc_set_neu), 0);
            mfc_write = 0;
        }
        else if(mfc_write == MFC_SET_ION) {
            mfc_set_ion.head[0] = '!';
            mfc_set_ion.head[1] = '2';
            mfc_set_ion.tail[0] = 0x0d;
            mfc_set_ion.tail[1] = 0x0a;
            send(etos_fd, &mfc_set_ion, sizeof(mfc_set_ion), 0);
            mfc_write = 0;
        }
		else if(mfc_write == MFC_SET_NEU2) {
            mfc_set_neu2.head[0] = '!';
            mfc_set_neu2.head[1] = '3';
            mfc_set_neu2.tail[0] = 0x0d;
            mfc_set_neu2.tail[1] = 0x0a;
            send(etos_fd, &mfc_set_neu2, sizeof(mfc_set_neu2), 0);
            mfc_write = 0;
        }
        else if(mfc_write == MFC_SET_ION2) {
            mfc_set_ion2.head[0] = '!';
            mfc_set_ion2.head[1] = '4';
            mfc_set_ion2.tail[0] = 0x0d;
            mfc_set_ion2.tail[1] = 0x0a;
            send(etos_fd, &mfc_set_ion2, sizeof(mfc_set_ion2), 0);
            mfc_write = 0;
        }


        epicsThreadSleep(0.05);
#if 0
        if(head == 0x01 && ret == 130 && lake.tail[0] == 0x0d && lake.tail[1] == 0x0a) {

        printf("******* Lake *******\n");
        printf("1 : %f, %f, %f, %f, %f, %f, %f, %f\n", lake.val1[0], lake.val1[1], lake.val1[2], lake.val1[3], lake.val1[4], lake.val1[5],
                                                        lake.val1[6], lake.val1[7]);

        printf("2 : %f, %f, %f, %f, %f, %f, %f, %f\n", lake.val2[0], lake.val2[1], lake.val2[2], lake.val2[3], lake.val2[4], lake.val2[5],
                                                        lake.val2[6], lake.val2[7]);

        printf("3 : %f, %f, %f, %f, %f, %f, %f, %f\n", lake.val3[0], lake.val3[1], lake.val3[2], lake.val3[3], lake.val3[4], lake.val3[5],
                                                        lake.val3[6], lake.val3[7]);

        printf("4 : %f, %f, %f, %f, %f, %f, %f, %f\n", lake.val4[0], lake.val4[1], lake.val4[2], lake.val4[3], lake.val4[4], lake.val4[5],
                                                        lake.val4[6], lake.val4[7]);
        }
        else if(head == 0x02 && vg.tail[0] == 0x0d && vg.tail[1] == 0x0a) {
            printf("******* VG *******\n");
            printf("PG  : %g, %g, %g, %g, %g, %g, %g\n", vg.pg[0], vg.pg[1], vg.pg[2], vg.pg[3], vg.pg[4], vg.pg[5], vg.pg[6]);
            printf("FG  : %g, %g, %g, %g, %g, %g\n", vg.fg[0], vg.fg[1], vg.fg[2], vg.fg[3], vg.fg[4], vg.fg[5]);
            printf("CG  : %g, %g, %g, %g\n", vg.cg[0], vg.cg[1], vg.cg[2], vg.cg[3]);
            printf("ION : %g, %g\n", vg.ion[0], vg.ion[1]);
            send(etos_fd, "#1\r\n", 4, 0);
        }
        else if(head == 0x03 && mfc.tail[0] == 0x0d && mfc.tail[1] == 0x0a) {
            printf("****** MFC ********\n");
            printf("Flow   : %f, %f\n", mfc.val[0], mfc.val[1]);
            printf("Status : %x, %x\n", mfc.status[0], mfc.status[1]);
            send(etos_fd, "#1\r\n", 4, 0);
        }
#endif
    } /* while */
}

int
drvAsynEtosConfigure(char *portName,
                     char *ipaddr,
                     int devId,
                     int noAutoConnect)
{
    char thName[256];

    etos_fd = set_up_etos(ipaddr);

    if(etos_fd < 0 ) printf("Connect Faile!!\n");
    else printf("Connect OK\n");

    sprintf(thName, "ET_%s", portName);

    epicsThreadCreate(thName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageEtosThread, ipaddr);

    sprintf(thName, "SoftTrg");
    epicsThreadCreate(thName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)SoftTriggerThread, NULL);

    return 0;
}

static const iocshArg drvAsynEtosConfigureArg0 = { "port name",iocshArgString};
static const iocshArg drvAsynEtosConfigureArg1 = { "ipaddr",iocshArgString};
static const iocshArg drvAsynEtosConfigureArg2 = { "dev",iocshArgInt};
static const iocshArg drvAsynEtosConfigureArg3 = { "disable auto-connect",iocshArgInt};
static const iocshArg *drvAsynEtosConfigureArgs[] = {
    &drvAsynEtosConfigureArg0, &drvAsynEtosConfigureArg1,
    &drvAsynEtosConfigureArg2, &drvAsynEtosConfigureArg3};
static const iocshFuncDef drvAsynEtosConfigureFuncDef =
                      {"drvAsynEtosConfigure",4,drvAsynEtosConfigureArgs};
static void drvAsynEtosConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynEtosConfigure(args[0].sval, args[1].sval, args[2].ival, args[3].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynEtosRegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynEtosConfigureFuncDef,drvAsynEtosConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynEtosRegisterCommands);
