#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#if 0
#include <errno.h>
#endif

#include <osiUnistd.h>
#include <cantProceed.h>
#include <errlog.h>
#include <iocsh.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <epicsTimer.h>
#include <osiUnistd.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsExport.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>

#include "drvAcq196.h"
#include "Acq196Dt100.h"

#define WR_OK		 0
#define WR_ERROR	-1

DtacqDrv dtacqDrv[MAX_CARD][96];
Acq196Drv acq196drv[MAX_CARD];
ShotInfo  shotinfo;
float    nbiPNB_A[2];
float    nbiPNB_B[2];

int acq196_set_command(char *cmd);

int acq196_set_command(char *cmd)
{
        FILE *pfp;
        char buff[128];

        pfp=popen( cmd, "r");
        if(!pfp) {
		printf("[ERROR] %s \r\n", cmd);
                return WR_ERROR;
        } 
        while(fgets(buff, 128, pfp) != NULL ) {
        } pclose(pfp);

        return WR_OK;
}

int Acq196Dt100_Init(int slot)
{
	char szTemp[200];
    /* Initialize */

	sprintf(szTemp, "acqcmd -b %d setModeTriggeredContinuous 0 100", slot);
    acq196_set_command(szTemp);
	sprintf(szTemp, "acqcmd -b %d setExternalClock DI0 1 DO1", slot);
    acq196_set_command(szTemp);
	sprintf(szTemp, "acqcmd -s %d set.ext_clk DI0", slot);
    acq196_set_command(szTemp);
    sprintf(szTemp, "acqcmd -s %d set.route d0 in mezz out fpga pxi", slot);
    acq196_set_command(szTemp);
    sprintf(szTemp, "acqcmd -s %d set.dtacq_drv FIFERR 0x00000000", slot);
    acq196_set_command(szTemp);

    sprintf(szTemp, "acqcmd -b %d -- setDIO ------", slot);
    acq196_set_command(szTemp);
    sprintf(szTemp,"acqcmd -s %d set.trig DI3 rising", slot);
    acq196_set_command(szTemp);
    sprintf(szTemp, "acqcmd -s %d set.route d3 in lemo out fpga", slot);
    acq196_set_command(szTemp);

    sprintf(szTemp, "acqcmd -s %d set.sys /sys/module/acq200_mu/parameters/max_dma 4000", slot);
    acq196_set_command(szTemp);
    sprintf(szTemp, "acqcmd -s %d set.sys /sys/module/acq200_mu/parameters/maxchain_clip 16", slot);
    acq196_set_command(szTemp);
    sprintf(szTemp, "acqcmd -s %d set.dtacq_drv hitide 4", slot);

    return 0;
}

static void Acq196Dt100(void *lParam)
{
	Acq196Drv *acqDrv = (Acq196Drv *)lParam;
	int slot = acqDrv->slot;

	char szTemp[200];

    tagQueueData queueData;

    printf("--- Init Acq196 [%d] ---\n", slot);

    Acq196Dt100_Init(slot);

    while(1) {
        switch(acqDrv->isWrite) {
        case ACQ_ABORT:
			sprintf(szTemp, "acqcmd -b %d setAbort", slot);
            acq196_set_command(szTemp);

            acqDrv->isWrite = 0;
            acqDrv->arm = ACQ_ST_STOP;
            break;
        case ACQ_ARM:
            if(acqDrv->t1 <= acqDrv->t0) {
                printf("T1 is T0 big\n");
                break;
            }
			sprintf(szTemp, "acqcmd -b %d setArm", slot);
            acq196_set_command(szTemp);

            acqDrv->isWrite = 0;
            acqDrv->arm = ACQ_ST_RUN;
			shotinfo.mdsput_ok[acqDrv->card] = 1;

            queueData.opcode = OP_CODE_DAQ_RUN;
            queueData.size = 0;
            queueData.pNode = NULL;
            epicsMessageQueueSend(acqDrv->DaqQueueId, (void *)&queueData, sizeof(tagQueueData));
            printf("-- D-TACQ [%d] : RUN --\n", slot);
            break;
        case ACQ_TRG_RISING:
            acqDrv->isWrite = 0;
            break;
        case ACQ_TRG_FALLING:
            acqDrv->isWrite = 0;
            break;
        case ACQ_INT_CLOCK:
            acqDrv->isWrite = 0;
        case ACQ_GET_DATA_TIME:
            acqDrv->isWrite = 0;
        default:
            break;
        }
        epicsThreadSleep(0.5);
    }
}

static void GetDataThread(void *lParam)
{
	Acq196Drv *acqDrv = (Acq196Drv *)lParam;

    int ret;
    tagQueueData queueData;

    epicsThreadSleep(10);

    printf("GetDataThread Start [%d]\n", acqDrv->slot);

    while(1) {

        if(acqDrv->arm == ACQ_ST_RUN && acqDrv->t1 > acqDrv->t0) {
			acqDrv->kstarshot = shotinfo.ca_kstarshot;
			acqDrv->shot_local = shotinfo.shot_local;
			printf("---KSTAR SHOT NUMBER : %d, MODE : %d---\n", acqDrv->kstarshot, shotinfo.kstar);
			printf("---LOCAL SHOT NUMBER : %d, shot info : %d\n", acqDrv->shot_local, shotinfo.shot_local);

			if(shotinfo.kstar && shotinfo.create_shot == 0) {
				shotinfo.create_shot = 1;
				mds_createNewShot(acqDrv->kstarshot);
			}
			else if(shotinfo.create_shot == 0){
				printf("--- mds_createNewShot ---\n");
				shotinfo.create_shot = 1;
                mds_createNewShot(acqDrv->shot_local);
            }

            ret = acqRunThread(acqDrv, 0);

            queueData.opcode = OP_CODE_DAQ_CLOSE;
            queueData.size = 0;
            queueData.pNode = NULL;
            epicsMessageQueueSend(acqDrv->DaqQueueId, (void *)&queueData, sizeof(tagQueueData));

            acqDrv->arm = ACQ_ST_STOP;
            acqDrv->kstardaq = 0;
        }
        else if(acqDrv->t1 <= acqDrv->t0) {
            acqDrv->set_fault = 1;
            acqDrv->arm = ACQ_ST_STOP;
        }

        epicsThreadSleep(0.01);
    }
}

static void PutDataThread(void *lParam)
{
	Acq196Drv *acqDrv = (Acq196Drv *)lParam;
    FILE *fpRaw = NULL;
    int i;
    int ret;
    int ellCnt;
	char szTemp[200];

    tagQueueData pDAQData;
    tagEllNode *pNode;

	printf("PutDataThread Start\n");
    for(i=0; i < 50; i++) {
        tagEllNode *pNode1 = NULL;

        pNode1 = (tagEllNode*) malloc(sizeof(tagEllNode));
        ellAdd(acqDrv->pNodeList, &pNode1->node);
    }
	printf("PutDataThread Init OK\n");

    while(1) {

        epicsMessageQueueReceive(acqDrv->DaqQueueId, (void *)&pDAQData, sizeof(tagQueueData));

        if(pDAQData.opcode == OP_CODE_DAQ_RUN) {
            /* file open */
			sprintf(szTemp, "rm -fr /media/data1/acq196.%d.dat", acqDrv->slot);
			system(szTemp);
			sprintf(szTemp, "/media/data1/acq196.%d.dat", acqDrv->slot);

            fpRaw = fopen(szTemp, "wb");
            if(fpRaw < 0) printf("-- file create error --\n");

			if(shotinfo.kstar) {
				acqDrv->shot = shotinfo.ca_kstarshot;
			}
			else {
				acqDrv->shot = shotinfo.shot_local;
			}
        } 
        else if(pDAQData.opcode == OP_CODE_DAQ_WRITE) {
            pNode = pDAQData.pNode;

            fwrite( pNode->data, pDAQData.size, 1, fpRaw);

            ellAdd(acqDrv->pNodeList, &pNode->node);
            ellCnt = ellCount(acqDrv->pNodeList);
            printf("-- Queue Data File Write [%d] --\n", acqDrv->slot);
        }
        else if(pDAQData.opcode == OP_CODE_DAQ_CLOSE) {
            fclose(fpRaw);
            fpRaw = NULL;

#if 0
            if(acqDrv->kstar) {
#else
			if(shotinfo.kstar) {
#endif
                printf("-- KSTAR Mdsput --\n");
				/*
				acqDrv->shot = shotinfo.ca_kstarshot;
                ret = acqMdsPutData(acqDrv, KSTAR_MDS);
                ret = acqMdsPutData(acqDrv, LOCAL_MDS);
				*/
				acqDrv->mdsput_rdy = MDS_PUT_RDY;
            }
            else {
                printf("-- Local Mdsput --\n");
				/* acqDrv->shot = shotinfo.shot_local; */
                /* ret = acqMdsPutData(acqDrv, LOCAL_MDS); */
                /* shotinfo.shot_local++; */
				acqDrv->mdsput_rdy = MDS_PUT_RDY;
            }

     		shotinfo.create_shot = 0;
            acqDrv->arm = ACQ_ST_STOP;
        }
    }
}

static void PutMdsThread(void *lParam)
{
	int i;

	while(1)
	{
		for(i = 0; i < shotinfo.slot_count; i++)
		{
			if(acq196drv[i].mdsput_rdy == MDS_PUT_RDY)
			{
				if(shotinfo.kstar) {
					acqMdsPutData(&acq196drv[i], KSTAR_MDS);
					acqMdsPutData(&acq196drv[i], LOCAL_MDS);
				}
				else {
					acqMdsPutData(&acq196drv[i], LOCAL_MDS);
				}

				acq196drv[i].mdsput_rdy = MDS_PUT_OK;
			}
		}

		if(shotinfo.mdsput_ok[0] == 1 && shotinfo.mdsput_ok[1] == 1) {
			if(acq196drv[0].mdsput_rdy == MDS_PUT_OK && acq196drv[1].mdsput_rdy == MDS_PUT_OK) {
				acq196drv[0].mdsput_rdy = MDS_PUT_NONE;
				acq196drv[1].mdsput_rdy = MDS_PUT_NONE;
				shotinfo.mdsput_ok[0] = 0;
				shotinfo.mdsput_ok[1] = 0;
				shotinfo.shot_local++;
			}
		}
		else {
			if(acq196drv[0].mdsput_rdy == MDS_PUT_OK && shotinfo.mdsput_ok[0] == 1) {
				acq196drv[0].mdsput_rdy = MDS_PUT_NONE;
				shotinfo.mdsput_ok[0] = 0;
				shotinfo.shot_local++;
			}
			else if(acq196drv[1].mdsput_rdy == MDS_PUT_OK && shotinfo.mdsput_ok[1] == 1) {
				acq196drv[1].mdsput_rdy = MDS_PUT_NONE;
				shotinfo.mdsput_ok[1] = 0;
				shotinfo.shot_local++;
			}
		}

		epicsThreadSleep(0.1);
	}
}

int dtacqSetupChannel(int card, int ch, int mode, char *node)
{
	/* mdsput -> 0 = NOT, 1 = PUT, 2 = Gain/Offset */
	dtacqDrv[card][ch].mdsput = mode;
	dtacqDrv[card][ch].gain = 0.0;
	dtacqDrv[card][ch].offset = 0.0;

	sprintf(dtacqDrv[card][ch].node_name, "%s", node);

	return 0;
}

int
drvAsynAcq196Configure(char *ipaddr,
                     int card,
					 int slot
                     )
{
    epicsThreadId id[3];
    int nTemp, nVal;
    int i;
    char buf[256];
	FILE *fp;

    acq196drv[card].isWrite = 0;
    acq196drv[card].clock = 5000;
    acq196drv[card].arm = 0;
    acq196drv[card].state[0] = 0x0;
    acq196drv[card].shot = 0;
    acq196drv[card].BlipTime = 16;
    acq196drv[card].set_fault = 0;
    acq196drv[card].st = 0;
    acq196drv[card].t0 = 0;
    acq196drv[card].t1 = 1;
    acq196drv[card].kstardaq = 0;
	acq196drv[card].kstar = 0;
    acq196drv[card].slot = slot;
	acq196drv[card].card = card;
	acq196drv[card].mdsput_rdy = MDS_PUT_NONE;

	sprintf(acq196drv[card].drv_bufAB[BUFFERA].id, "bufA");
	sprintf(acq196drv[card].drv_bufAB[BUFFERB].id, "bufB");

	acq196drv[card].ops.ci.timeout = INITIAL_TIMEOUT;

	shotinfo.create_shot = 0;

	printf("acq196 slot [%d]\n", acq196drv[card].slot);

	acq196drv[card].DaqQueueId = epicsMessageQueueCreate(50, sizeof(tagQueueData));

    acq196drv[card].pNodeList = (ELLLIST*) malloc(sizeof(ELLLIST));

    ellInit(acq196drv[card].pNodeList);

	sprintf(buf, "%s", STR_CH_MAPPING_FILE);
    fp = fopen(buf, "r");
	    if( fp == NULL) {
			epicsPrintf("can't open '%s'\n", buf);
		    return(0);
    }

    for( i=0; i < 96; i++)
    {
        if( fgets(buf, 32, fp) == NULL ) {
            fprintf(stdout, "file EOF error!\n");
            return(0);
        }
        sscanf(buf, "%d %d", &nTemp, &nVal);
        if( nTemp != (i+1) ) {
            fprintf(stdout, "channel index order error!\n");
            return (0);
        }
        acq196drv[card].channelMapping[nVal] = i;
    }

    fclose(fp);
				

	sprintf(buf, "%s_HT", ipaddr);
    id[0] = epicsThreadCreate(buf,
                 epicsThreadPriorityHigh,
                 epicsThreadGetStackSize(epicsThreadStackSmall),
                 (EPICSTHREADFUNC)GetDataThread, &acq196drv[card]);

	sprintf(buf, "%s_ACQ", ipaddr);
    id[1] = epicsThreadCreate(buf,
                 epicsThreadPriorityHigh,
                 epicsThreadGetStackSize(epicsThreadStackSmall),
                 (EPICSTHREADFUNC)Acq196Dt100, &acq196drv[card]);

	sprintf(buf, "%s_PUT", ipaddr);
    id[2] = epicsThreadCreate(buf,
                 epicsThreadPriorityHigh,
                 epicsThreadGetStackSize(epicsThreadStackSmall),
                 (EPICSTHREADFUNC)PutDataThread, &acq196drv[card]);

    return 0;
}

int
drvMdsplusConfigure(int start)
{

	shotinfo.slot_count = start;

	epicsThreadCreate("MDSPUT",
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)PutMdsThread, NULL);

    return 0;
}

static const iocshArg drvMdsplusConfigureArg0 = { "start", iocshArgInt };
static const iocshArg *drvMdsplusConfigureArgs[] = {
	&drvMdsplusConfigureArg0
};
static const iocshFuncDef drvMdsplusConfigureFuncDef = 
					{ "drvMdsplusConfigure", 1, drvMdsplusConfigureArgs };
static void drvMdsplusConfigureCallFunc(const iocshArgBuf *args)
{
	drvMdsplusConfigure(args[0].ival);
}

static const iocshArg dtacqSetupChannelArg0 = { "card",iocshArgInt};
static const iocshArg dtacqSetupChannelArg1 = { "ch",iocshArgInt};
static const iocshArg dtacqSetupChannelArg2 = { "mode",iocshArgInt};
static const iocshArg dtacqSetupChannelArg3 = { "node",iocshArgString};
static const iocshArg *dtacqSetupChannelArgs[] = {
    &dtacqSetupChannelArg0, &dtacqSetupChannelArg1,
	&dtacqSetupChannelArg2, &dtacqSetupChannelArg3
};
static const iocshFuncDef dtacqSetupChannelFuncDef = 
					  {"dtacqSetupChannel", 4, dtacqSetupChannelArgs};
static void dtacqSetupChannelCallFunc(const iocshArgBuf *args)
{
    dtacqSetupChannel(args[0].ival, args[1].ival, args[2].ival, args[3].sval);
}

static const iocshArg drvAsynAcq196ConfigureArg0 = { "ipaddr",iocshArgString};
static const iocshArg drvAsynAcq196ConfigureArg1 = { "card",iocshArgInt};
static const iocshArg drvAsynAcq196ConfigureArg2 = { "slot",iocshArgInt};
static const iocshArg *drvAsynAcq196ConfigureArgs[] = {
    &drvAsynAcq196ConfigureArg0, &drvAsynAcq196ConfigureArg1,
	&drvAsynAcq196ConfigureArg2
    };
static const iocshFuncDef drvAsynAcq196ConfigureFuncDef =
                      {"drvAsynAcq196Configure",3,drvAsynAcq196ConfigureArgs};
static void drvAsynAcq196ConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynAcq196Configure(args[0].sval, args[1].ival, args[2].ival
                               );
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynAcq196RegisterCommands(void)
{
    static int firstTime = 1;
    if (firstTime) {
        iocshRegister(&drvAsynAcq196ConfigureFuncDef,drvAsynAcq196ConfigureCallFunc);
		iocshRegister(&dtacqSetupChannelFuncDef, dtacqSetupChannelCallFunc);
		iocshRegister(&drvMdsplusConfigureFuncDef, drvMdsplusConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynAcq196RegisterCommands);
