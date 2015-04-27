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

#include "drvHoneywell.h"
#include "modbus_tcp.h"

typedef struct __HONEYWELL {
    int sock;
    char ipaddr[256];
} honeywell_data;

honeywell_data honey_data;

int WatchdogModbusTracking(const int iRet, honeywell_data *honey)
{
    char strErr[256];
    switch(iRet){
        case 0:
	    strcpy(strErr,"Comms Failure");
            break;
        case -1:
	    strcpy(strErr,"Illegal Function");
            break;
        case -2:
	    strcpy(strErr,"Illegal Data Address");
            break;
        case -3:
	    strcpy(strErr,"Illegal Data Value");
            break;
        case -4:
	    strcpy(strErr,"Slave Device Failure");
            break;
        case -5:
	    strcpy(strErr,"Acknowledge");
            break;
        case -6:
	    strcpy(strErr,"Slave Device Busy");
            break;
        case -7:
	    strcpy(strErr,"Negative Acknowledge");
            break;
        case -8:
	    strcpy(strErr,"Memory Parity Error");
            break;
	default:
	    break;
    }
    epicsPrintf("Modbus Comm's Error[%s][%d] - %s\n",honey->ipaddr,honey->sock,strErr);

	close(honey->sock);
	honey->sock = -1;
	epicsThreadSleep(1.0);

    honey->sock = set_up_tcp(honey->ipaddr);     
    epicsPrintf("Connect OK[%s][%d]\n",honey->ipaddr,honey->sock);

    return 0;
}

static void GetMessageModbusThread(void *lParam)
{
    honeywell_data *honey = (honeywell_data *)lParam;
    int nVal[20];
    int iRet;

    nVal[0] = 0;
    nVal[1] = 0;
    while(1)
    {
        iRet = read_holding_registers_tcp(1, 1, 2, nVal, 2, honey->sock);

        if(iRet < 0)
            WatchdogModbusTracking(iRet, honey);
#if 0
        else
            printf("Read Value : [%x], [%x]\n", nVal[0], nVal[1]);
#endif
        epicsThreadSleep(0.5);
    }
}

int
drvAsynModbusConfigure(char *portName,
                     char *ipaddr,
                     int devId,
                     int noAutoConnect)
{
    char thName[256];

    sprintf(honey_data.ipaddr, "%s", ipaddr);

    honey_data.sock = set_up_tcp(honey_data.ipaddr);

    if(honey_data.sock < 0 ) printf("Connect Faile!!\n");
    else printf("Connect OK\n");

    sprintf(thName, "H2_%s", portName);

    epicsThreadCreate(thName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageModbusThread, &honey_data);

    return 0;
}

static const iocshArg drvAsynModbusConfigureArg0 = { "port name",iocshArgString};
static const iocshArg drvAsynModbusConfigureArg1 = { "ipaddr",iocshArgString};
static const iocshArg drvAsynModbusConfigureArg2 = { "dev",iocshArgInt};
static const iocshArg drvAsynModbusConfigureArg3 = { "disable auto-connect",iocshArgInt};
static const iocshArg *drvAsynModbusConfigureArgs[] = {
    &drvAsynModbusConfigureArg0, &drvAsynModbusConfigureArg1,
    &drvAsynModbusConfigureArg2, &drvAsynModbusConfigureArg3};
static const iocshFuncDef drvAsynModbusConfigureFuncDef =
                      {"drvAsynModbusConfigure",4,drvAsynModbusConfigureArgs};
static void drvAsynModbusConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynModbusConfigure(args[0].sval, args[1].sval, args[2].ival, args[3].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynModbusRegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynModbusConfigureFuncDef,drvAsynModbusConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynModbusRegisterCommands);
