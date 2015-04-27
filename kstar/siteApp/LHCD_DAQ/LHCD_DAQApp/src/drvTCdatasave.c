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

#include <sys/select.h>
#include <sys/time.h>

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
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <epicsTimer.h>
#include <osiUnistd.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsExport.h>

#include "tcp_comm.h"
#include "tc_save.h"

TCMDS_INFO	tcmds_info[MAX_SYS_TC];

double swap_double(double dval)
{
	int i;
	double frtnval = 0.0;
	char buf[8];
	unsigned char *tmp;

	tmp = (unsigned char *)&dval;

	for(i=0; i < 8; i++)
	{
		buf[7-i] = *tmp++;
	}

	memcpy(&frtnval, buf, 8);

	return frtnval;
}

static void GetTCdatasave(void *lParam)
{
	TCMDS_INFO *tcmds = (TCMDS_INFO*)lParam;
	int RUN = 0;
	char recvBuff[5];
	int nRet = 0;
	int count = 0;
	int i, j, k;

	LTU_HEAD	ltu_head;

_RECONNECT:
	tcmds->sock = set_up_tcp(tcmds->ip, 2020);

	if(tcmds->sock <= -1) {
		epicsPrintf("TC IP : %s, Connect\n", tcmds->ip);
		RUN = 1;
	}
	else {
		RUN = 1;
	}
	while(RUN) {
		recvBuff[0] = 0x0;
		nRet = recv(tcmds->sock, recvBuff, 1, 0);

		if(recvBuff[0] == LTU_CMD_HEAD) {
			char temp[sizeof(LTU_HEAD)];

			nRet = recv_data_get(tcmds->sock, temp, sizeof(LTU_HEAD));

			if(nRet != -1) {
				memcpy(&ltu_head, temp, sizeof(LTU_HEAD));

				ltu_head.nDataCnt = SWAP_UINT(ltu_head.nDataCnt);

				char buff[ltu_head.nDataCnt*MAX_CARD_NUM*TC_MAX_CH*8];
				double LTU_DATA[ltu_head.nDataCnt][MAX_CARD_NUM][TC_MAX_CH];
				double save_data[MAX_CARD_NUM][TC_MAX_CH][ltu_head.nDataCnt];

				nRet = recv_data_get(tcmds->sock, buff, sizeof(buff));

				memcpy(&LTU_DATA, buff, sizeof(buff));

				for(i=0; i < MAX_CARD_NUM; i++) {
					for(j=0; j < TC_MAX_CH; j++) {
						for(k=0; k < ltu_head.nDataCnt; k++) {
							save_data[i][j][k] = swap_double(LTU_DATA[k][i][j]);
						}
					}
				}

				if(ltu_head.nDataCnt == 10 && tcmds->status == 1) {
					epicsPrintf("--- TC %d, data file save, shot : %d ---\n", tcmds->card, tcmds->shot);

					for(i=0; i < MAX_CARD_NUM; i++) {
						if(tcmds->fp[i] != NULL) {
							fwrite(&save_data[i][0][0], sizeof(double)*ltu_head.nDataCnt*8, 1, tcmds->fp[i]);
							fflush(tcmds->fp[i]);
						}
						else {
							epicsPrintf("--- TC %d, file is null\n", tcmds->card);
						}
					}

					count++;

					if(tcmds->t1 <= count) {
						count = 0;
						epicsPrintf("--- TC %d, data file close ---\n", tcmds->card);

						for(i=0; i < MAX_CARD_NUM; i++) {
							fclose(tcmds->fp[i]);
							tcmds->fp[i] = NULL;
						}

						epicsPrintf("--- TC %d, mdsput...\n", tcmds->card);
						acqMdsPutTCData(tcmds, 0);
					}
				}
			}
			else {
				epicsPrintf("** TC %d, Connection faile\n", tcmds->card);
				tcmds->sock = -1;
				epicsThreadSleep(5);
				goto _RECONNECT;
			}
		}
		else if(nRet < 0) {
			epicsPrintf("** TC %d, Data Head... error...\n", tcmds->card);
			tcmds->sock = -1;
			epicsThreadSleep(5);
			goto _RECONNECT;
		}
	}
}

static void GetTCMessage(void *lParam)
{
	TCMDS_INFO *tcmds = (TCMDS_INFO*)lParam;
	int i;
	char szFileName[256];

	while(1) {

		switch(tcmds->msg)
		{
		case 1: /* ready */
			for(i=0; i < MAX_CARD_NUM; i++) {
				sprintf(szFileName, "/tmp/tc_%d_%d.dat", tcmds->card, i);
				tcmds->fp[i] = fopen(szFileName, "w+");
			}
			tcmds->status = 1;
			tcmds->msg = 0;
			break;
		case 2: /* shot number */
			break;
		case 3: /* abort */
			tcmds->status = 0;
			break;
		case 4: /* t0 */
			break;
		case 5: /* t1 */
			break;
		}

		epicsThreadSleep(0.1);
	}
}

int
drvAsynTCdatasaveConfigure(char *ipaddr,
							int card,
							int slot
						   )
{
	char buf[50];
	char temp[256];
	int  s, ch, mdsput;
	FILE *fp;

	sprintf(tcmds_info[card].ip, "%s", ipaddr);

	tcmds_info[card].sock = -1;
	tcmds_info[card].t0 = 0;
	tcmds_info[card].t1 = 1;
	tcmds_info[card].localshot = 0;
	tcmds_info[card].kstarshot = 0;
	tcmds_info[card].shot = 0;
	tcmds_info[card].status = -1;

	tcmds_info[card].card = card;

#if 1

	system("rm -f /tmp/tc_*.dat");

	sprintf(buf, "/usr/local/epics/siteApp/config/tc_%d_mds.cfg", card);
	fp = fopen(buf, "r");
	if(fp != NULL) {
		while(1) {
			if(fgets(buf, 50, fp) == NULL) {
				break;
			}
			if(buf[0] == '#') ;
			else if(buf[0] == ' ');
			else {
				sscanf(buf, "%s %d %d %d", temp, &s, &ch, &mdsput);
			
				tcmds_info[card].mdsinfo[s][ch].mdsput = mdsput;
				sprintf(tcmds_info[card].mdsinfo[s][ch].node, "%s", temp);
			}
		}
		fclose(fp);
		fp = NULL;
	}
	else {
		epicsPrintf("TC %d, Config file open error...\n", card);
	}

	epicsPrintf("TC NODE [%s]\n", tcmds_info[card].mdsinfo[0][0].node);

#endif

	sprintf(buf, "TC%02d", card);
	epicsThreadCreate(buf,
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)GetTCdatasave, &tcmds_info[card]);

	sprintf(buf, "TC%02d_MSG", card);
	epicsThreadCreate(buf,
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)GetTCMessage, &tcmds_info[card]);

	return 0;
}

static const iocshArg drvAsynTCdatasaveConfigureArg0 = {"ip", iocshArgString};
static const iocshArg drvAsynTCdatasaveConfigureArg1 = {"card", iocshArgInt};
static const iocshArg drvAsynTCdatasaveConfigureArg2 = {"slot", iocshArgInt};

static const iocshArg *drvAsynTCdatasaveConfigureArgs[] = {
	&drvAsynTCdatasaveConfigureArg0, &drvAsynTCdatasaveConfigureArg1,
	&drvAsynTCdatasaveConfigureArg2
};

static const iocshFuncDef drvAsynTCdatasaveConfigureFuncDef = {
	"drvAsynTCdatasaveConfigure", 3, drvAsynTCdatasaveConfigureArgs
};

static void drvAsynTCdatasaveConfigureCallFunc(const iocshArgBuf *args)
{
	drvAsynTCdatasaveConfigure(args[0].sval, args[1].ival, args[2].ival);
}

static void
drvAsynTCdatasaveRegisterCommands(void)
{
	static int firstTime = 1;
	if(firstTime) {
		iocshRegister(&drvAsynTCdatasaveConfigureFuncDef, drvAsynTCdatasaveConfigureCallFunc);
		firstTime = 0;
	}
}

epicsExportRegistrar(drvAsynTCdatasaveRegisterCommands);
