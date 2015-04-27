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

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/tcp.h>
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

#include "drvPoloidalScan.h"

#include "nidaq.h"

PoloScan poloScan;

#define ETOS_PORT 40001

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

static void GetMessageEtosThread(void *lParam)
{
    int etos_fd;
    int ret;
    int ind = 0;
    char buff[7];
    float val;

    etos_fd = set_up_etos("172.17.121.253");
 
    while(1)
    {
        ret = recv(etos_fd, buff, 7, 0);
 
        if(ret <= 0) {
            close(etos_fd);
            etos_fd = -1;
#if 0
            printf("Etos Connection Faile\n");
#endif
            epicsThreadSleep(1);
            etos_fd = set_up_etos("172.17.121.253");
            continue;
        }
        if(ret == 7 && buff[5] == 0x0d && buff[6] == 0x0a && buff[0] == '#') {
            memcpy(&val, &buff[1], 4);
#if 0
            printf("-- Poloidal Ang : [%f] --\n", val);
#endif
            poloScan.val = val;
        }
        epicsThreadSleep(0.05);
    }
}

void PoloidalScanThread(void *arg)
{
    int i = 0;

    while(1) {
        if(poloScan.msg == POLOSCAN_START) {
            poloScan.mdsval[i++] = poloScan.val;
        }
        else if(poloScan.msg == POLOSCAN_END) {
            poloScan.sample = i;
            i = 0;
            epicsPrintf("-- Poloidal Scan Sample : [%d] --\n", poloScan.sample);
            epicsPrintf("-- Poloidal Scan T0 : [%f] --\n", poloScan.t0);
            epicsPrintf("-- Poloidal Scan T1 : [%f] --\n", poloScan.t1);
            poloScan.msg = POLOSCAN_MDS;
        }
        else if(poloScan.msg == POLOSCAN_DONE) {
            i = 0;
        }
        epicsThreadSleep(0.1);
    }
}

int drvAsynPoloidalScanConfigure(int start)
{
    int i;

    poloScan.msg = POLOSCAN_DONE;
    poloScan.t0 = 0.0;
    poloScan.t1 = 1.0;

    epicsThreadCreate("POLO",
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)PoloidalScanThread, NULL);

    epicsThreadCreate("ETOS-T",
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageEtosThread, NULL);

    return 0;
}

static const iocshArg drvAsynPoloidalScanConfigureArg0 = { "start",iocshArgInt};

static const iocshArg *drvAsynPoloidalScanConfigureArgs[] = {
    &drvAsynPoloidalScanConfigureArg0
};
static const iocshFuncDef drvAsynPoloidalScanConfigureFuncDef =
                      {"drvAsynPoloidalScanConfigure",1,drvAsynPoloidalScanConfigureArgs};
static void drvAsynPoloidalScanConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynPoloidalScanConfigure(args[0].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynPoloidalScanRegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynPoloidalScanConfigureFuncDef,drvAsynPoloidalScanConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynPoloidalScanRegisterCommands);
