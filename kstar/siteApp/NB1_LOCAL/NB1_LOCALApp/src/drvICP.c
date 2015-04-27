#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <asm/ioctls.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

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

#include "icp.h"

float cenox_val[4];
float vac_val;

int setup_icp_etos( char *ip_address, int port )
{
    int sfd;
    struct sockaddr_in server;
    int connect_stat;

    sfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
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

static void GetMessageICPThread(void *lParam)
{
    int sock;
    int ret;
    int i;
    int timeout = 0;
    char buff[30];

    sock = setup_icp_etos("172.17.101.130", 40002);

    while(1)
    {
        ret = recv(sock, buff, 19, 0);

        if(ret < 0)
        {
            close(sock);
            sock = -1;
            printf("ICP Lake Connection Faile\n");
            epicsThreadSleep(1);
            sock = setup_icp_etos("172.17.101.130", 40002);
            continue;
        }
        
        if(ret < 19)
        {
            timeout = 0;
            while(1)
            {
                i = recv(sock, &buff[ret], 19-ret, 0);
                ret += i;
                if(ret == 19 || timeout > 100) break;
                timeout++;
            }
        }
        if(ret == 19 && buff[0] == '#' && buff[ret-1] == 0x0a)
        {
            memcpy(&cenox_val[0], &buff[1], 4);
            memcpy(&cenox_val[1], &buff[5], 4);
            memcpy(&cenox_val[2], &buff[9], 4);
            memcpy(&cenox_val[3], &buff[13], 4);
            sscanf(buff, "0, %f,", &vac_val);
        }

        epicsThreadSleep(0.1);
    }
}

static void GetMessageICPVacThread(void *lParam)
{
    int sock;
    int ret;
    int i;
    int timeout = 0;
    char buff[30];

    sock = setup_icp_etos("172.17.101.130", 40001);

    while(1)
    {
        ret = recv(sock, buff, 29, 0);

        if(ret < 0)
        {
            close(sock);
            sock = -1;
            printf("ICP VAC Connection Faile\n");
            epicsThreadSleep(1);
            sock = setup_icp_etos("172.17.101.130", 40001);
            continue;
        }
        
        if(ret < 29)
        {
            timeout = 0;
            while(1)
            {
                i = recv(sock, &buff[ret], 29-ret, 0);
                ret += i;
                if(ret == 29 || timeout > 100) break;
                timeout++;
            }
        }
        if(ret == 29 && buff[ret-1] == 0x0a)
        {
            sscanf(buff, "0, %f,", &vac_val);
        }

        epicsThreadSleep(0.1);
    }
}

int
drvAsynICPConfigure(char *portName,
                     int noAutoConnect)
{
    char thName[256];

    sprintf(thName, "ICP_%s", portName);

#if 1
    epicsThreadCreate(thName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageICPThread, NULL);
#endif
    epicsThreadCreate("ICPVAC",
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageICPVacThread, NULL);

    return 0;
}

static const iocshArg drvAsynICPConfigureArg0 = { "port name",iocshArgString};
static const iocshArg drvAsynICPConfigureArg1 = { "disable auto-connect",iocshArgInt};
static const iocshArg *drvAsynICPConfigureArgs[] = {
    &drvAsynICPConfigureArg0, &drvAsynICPConfigureArg1};

static const iocshFuncDef drvAsynICPConfigureFuncDef =
                      {"drvAsynICPConfigure",2,drvAsynICPConfigureArgs};
static void drvAsynICPConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynICPConfigure(args[0].sval, args[1].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynICPRegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynICPConfigureFuncDef,drvAsynICPConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynICPRegisterCommands);
