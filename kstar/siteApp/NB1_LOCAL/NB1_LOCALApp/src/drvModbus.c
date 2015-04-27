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

#include "modbus_tcp.h"
#include "bending_magnet.h"

/*
*/

#if 0
typedef struct EUROTHERM_DATA {
    int sock;
    char ipaddr[256];
    int auto_connect;
    int isWrite;

    short status;                 /* 40001 */
    short fault_set;              /* 40002 */
    short limit_Current;          /* 40003 */
    short pro_vout;               /* 40004 */
    short ctrl_iout;              /* 40005 */
    short vout;                   /* 40006 */
    short iout;                   /* 40007 */
    short dc_volt;                /* 40008 */
    short dc_curr;                /* 40009 */
    short inv_curr;               /* 40010 */
    short fault_status;           /* 40011 */
    short req_vout_set;           /* 40012 */
    short req_iout_set;           /* 40013 */
    short req_vout_under_volt;    /* 40014 */
    short req_vout_under_curr;    /* 40015 */
    short touch_status;           /* 40016 */
    short set_iout_under;         /* 40017 */
    short set_vout_under;         /* 40018 */
    short set_vout;               /* 40019 */
    short set_iout;               /* 40020 */

    short wr_vout_under;
    short wr_iout_under;
    short wr_vout;
    short wr_iout;
} Bending_Data;

#endif

Bending_Data bend_data[MAX_BEND];

int WatchdogModbusTracking(const int iRet, Bending_Data *bend)
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
    epicsPrintf("Modbus Comm's Error[%s][%d] - %s\n",bend->ipaddr,bend->sock,strErr);

    close(bend->sock);
    epicsThreadSleep(0.5);

    bend->sock = set_up_tcp(bend->ipaddr);     
    epicsPrintf("Connect OK[%s][%d]\n",bend->ipaddr,bend->sock);
    epicsThreadSleep(0.5);

    return 0;
}

static void GetMessageModbusThread(void *lParam)
{
    Bending_Data *bend = (Bending_Data *)lParam;
    int nVal[22];
    int iRet;

    while(1)
    {
/*
int read_input_registers_tcp( int slave, int start_addr, int count,
                                int *dest, int dest_size, int fd );
*/
#if 1
        iRet = read_holding_registers_tcp(BMT_SLAVE, 1, 21, nVal, 21, bend->sock);
#else
        iRet = read_input_registers_tcp(BMT_SLAVE, 1, 20, nVal, 20, bend->sock);
#endif
        if(iRet > 0) {
            bend->status = (short)nVal[0]&0xFFFF;
            bend->fault_set = (short)nVal[1]&0xFFFF;
            bend->limit_curr = (short)nVal[2]&0xFFFF;
            bend->pro_vout = (short)nVal[3]&0xFFFF;
            bend->ctrl_iout = (short)nVal[4]&0xFFFF;
            bend->vout = (short)nVal[5]&0xFFFF;
            bend->iout = (short)nVal[6]&0xFFFF;
            bend->dc_volt = (short)nVal[7]&0xFFFF;
            bend->dc_curr = (short)nVal[8]&0xFFFF;
            bend->inv_curr = (short)nVal[9]&0xFFFF;
            bend->fault_status = (short)nVal[10]&0xFFFF;
            bend->req_vout_set = (short)nVal[11]&0xFFFF;
            bend->req_iout_set = (short)nVal[12]&0xFFFF;
            bend->req_vout_under_volt = (short)nVal[13]&0xFFFF;
            bend->req_vout_under_curr = (short)nVal[14]&0xFFFF;
            bend->touch_status = (short)nVal[15]&0xFFFF;
            bend->set_iout_under = (short)nVal[16]&0xFFFF;
            bend->set_vout_under = (short)nVal[17]&0xFFFF;
            bend->set_vout = (short)nVal[18]&0xFFFF;
            bend->set_iout = (short)nVal[19]&0xFFFF;
            bend->fault_code = (short)nVal[20]&0xFFFF;
        }
		else 
			WatchdogModbusTracking(iRet,bend);

        if(bend->isWrite > 0) {
            switch(bend->isWrite) {
            case TOUCH_CMD_WRITE:
                iRet = preset_single_register_tcp(BMT_SLAVE, TOUCH_CMD_WRITE, bend->btn_val, bend->sock);
                break;
            case IOUT_UNDER_WRITE:
                iRet = preset_single_register_tcp(BMT_SLAVE, bend->isWrite, bend->wr_iout_under, bend->sock);
                break;
            case VOUT_UNDER_WRITE:
                iRet = preset_single_register_tcp(BMT_SLAVE, bend->isWrite, bend->wr_vout_under, bend->sock);
                break;
            case VOUT_WRITE:
                iRet = preset_single_register_tcp(BMT_SLAVE, bend->isWrite, bend->wr_vout, bend->sock);
                break;
            case IOUT_WRITE:
                iRet = preset_single_register_tcp(BMT_SLAVE, bend->isWrite, bend->wr_iout, bend->sock);
                break;
            }

            if(iRet < 0)
                WatchdogModbusTracking(iRet, bend);

            bend->isWrite = 0;
        }

        epicsThreadSleep(0.1);
    }
}

int
drvAsynModbusConfigure(char *portName,
                     char *ipaddr,
                     int devId,
                     int noAutoConnect)
{
    char thName[256];

    sprintf(bend_data[devId].ipaddr, "%s", ipaddr);

    bend_data[devId].sock = set_up_tcp(bend_data[devId].ipaddr);

    if(bend_data[devId].sock < 0 ) printf("Connect Faile!!\n");
    else printf("Connect OK\n");

    sprintf(thName, "EURO_%s", portName);

    epicsThreadCreate(thName,
        epicsThreadPriorityHigh,
        epicsThreadGetStackSize(epicsThreadStackSmall),
        (EPICSTHREADFUNC)GetMessageModbusThread, &bend_data[devId]);

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
