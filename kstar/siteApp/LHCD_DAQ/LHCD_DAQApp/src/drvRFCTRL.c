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
#include <epicsAssert.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <epicsTimer.h>
#include <osiUnistd.h>
#include <epicsExport.h>

#include "nidaq.h"
#include "tcp_comm.h"
#include "rfctrl.h"

RFCTRL_INFO	lhcd_rfctrl;

static void GetRFCTRL_Data_Thread(void *lParam)
{
	RFCTRL_INFO *lhcd_info = (RFCTRL_INFO *)lParam;

	char buff[4];
	int ret;
	int i, ind;
	int data_count = 0;

	epicsPrintf("*** RFCTRL Thread Start ***\n");

	while(1) {
		buff[0] = 0x0;

		ret = recv(lhcd_info->sock, buff, 1, 0);

		epicsPrintf("*** RFCTRL ... RECV [%d] ***\n", ret);

		if(ret <= 0) {
			epicsPrintf("*** RFCTRL Recv... Error ***\n");
			close(lhcd_info->sock);
			lhcd_info->sock = -1;
			lhcd_info->con_stat = 0;

			epicsThreadSleep(1);
			lhcd_info->sock = set_up_tcp(lhcd_info->ip, RFCTRL_PORT);

			if(lhcd_info->sock >= 0) lhcd_info->con_stat = 1;
			continue;
		}

		if(buff[0] == MDSDATA_INIT_HEAD) {
			char s_temp[sizeof(MDSDATA_HEAD)];
			MDSDATA_HEAD  head;
			recv_data_get(lhcd_info->sock, s_temp, sizeof(s_temp));

			memcpy(&head, s_temp, sizeof(MDSDATA_HEAD));

			epicsPrintf("RF CTRL Head... Recv OK...\n");
			epicsPrintf("DATA count : %d, Sample Rate : %d\n", head.count, head.samp_clk);
			epicsPrintf("fwd_1st  : %f\n", head.fwd_1st);
			epicsPrintf("fwd_2st  : %f\n", head.fwd_2st);
			epicsPrintf("fwd_its  : %f\n", head.fwd_its);
			epicsPrintf("fwd_ofs  : %f\n", head.fwd_ofs);
			epicsPrintf("fwd_loss : %f\n", head.fwd_loss);

			lhcd_info->fwd_1st = head.fwd_1st;
			lhcd_info->fwd_2st = head.fwd_2st;
			lhcd_info->fwd_its = head.fwd_its;
			lhcd_info->fwd_ofs = head.fwd_ofs;
			lhcd_info->fwd_loss = head.fwd_loss;

			epicsPrintf("rev_1st  : %f\n", head.rev_1st);
			epicsPrintf("rev_2st  : %f\n", head.rev_2st);
			epicsPrintf("rev_its  : %f\n", head.rev_its);
			epicsPrintf("rev_ofs  : %f\n", head.rev_ofs);
			epicsPrintf("rev_loss : %f\n", head.rev_loss);

			lhcd_info->rev_1st = head.rev_1st;
			lhcd_info->rev_2st = head.rev_2st;
			lhcd_info->rev_its = head.rev_its;
			lhcd_info->rev_ofs = head.rev_ofs;
			lhcd_info->rev_loss = head.rev_loss;

			epicsPrintf("phase_offset  : %f\n", head.phase_offset);
			epicsPrintf("iq_i_offset   : %f\n", head.iq_i_offset);
			epicsPrintf("iq_q_offset   : %f\n", head.iq_q_offset);

			epicsPrintf("sampling clk : %d\n", head.samp_clk);

			lhcd_info->phase_offset = head.phase_offset;
			lhcd_info->iq_i_offset = head.iq_i_offset;
			lhcd_info->iq_q_offset = head.iq_q_offset;

			data_count = 0;
			lhcd_info->data_count = head.count + 1;
			if(head.samp_clk == 0) lhcd_info->samplclk = 1000.0;
			else if(head.samp_clk == 1) lhcd_info->samplclk = 2000.0;
			else if(head.samp_clk == 2) lhcd_info->samplclk = 5000.0;
			else lhcd_info->samplclk = 1000.0;

			lhcd_info->fp[0] = fopen("/tmp/rf_fwd_pow.dat", "w+");
			lhcd_info->fp[1] = fopen("/tmp/rf_rev_pow.dat", "w+");
			lhcd_info->fp[2] = fopen("/tmp/rf_iqi_pow.dat", "w+");
			lhcd_info->fp[3] = fopen("/tmp/rf_iqq_pow.dat", "w+");
		}
		if(buff[0] == MDSDATA_ARRY_DATA) {
			uint16 u_data[4][100];
			char  temp[sizeof(uint16)*4*100];

			recv_data_get(lhcd_info->sock, temp, sizeof(temp));

			memcpy(&u_data, temp, sizeof(temp));

			printf("*** data save ***\n");
			for(i=0; i < 100; i++) {
				fwrite(&u_data[0][i], sizeof(uint16), 1, lhcd_info->fp[0]);
				fwrite(&u_data[1][i], sizeof(uint16), 1, lhcd_info->fp[1]);
				fwrite(&u_data[2][i], sizeof(uint16), 1, lhcd_info->fp[2]);
				fwrite(&u_data[3][i], sizeof(uint16), 1, lhcd_info->fp[3]);

				fflush(lhcd_info->fp[0]);
				fflush(lhcd_info->fp[1]);
				fflush(lhcd_info->fp[2]);
				fflush(lhcd_info->fp[3]);
			}

			printf(" data index [%d] \n", data_count);

			data_count += 100;

			if(data_count >= (lhcd_info->data_count)) {

				printf("--- data save file close ---\n");

				fclose(lhcd_info->fp[0]);
				fclose(lhcd_info->fp[1]);
				fclose(lhcd_info->fp[2]);
				fclose(lhcd_info->fp[3]);

				data_count = 0;
				lhcd_info->mds_status = DATA_GET_OK;

				printf("--- data save file close ok...\n");
			}
		}

		epicsThreadSleep(0.01);
	}
}

void rf_datafileOpen(RFCTRL_INFO *lhcd_info)
{
	lhcd_info->fp[0] = fopen("/tmp/rf_fwd_pow.dat", "r");
	lhcd_info->fp[1] = fopen("/tmp/rf_rev_pow.dat", "r");
	lhcd_info->fp[2] = fopen("/tmp/rf_iqi_pow.dat", "r");
	lhcd_info->fp[3] = fopen("/tmp/rf_iqq_pow.dat", "r");
}

void rf_datafileClose(RFCTRL_INFO *lhcd_info)
{
	fclose(lhcd_info->fp[0]);
	fclose(lhcd_info->fp[1]);
	fclose(lhcd_info->fp[2]);
	fclose(lhcd_info->fp[3]);
	lhcd_info->fp[0] = NULL;
	lhcd_info->fp[1] = NULL;
	lhcd_info->fp[2] = NULL;
	lhcd_info->fp[3] = NULL;
}

static void PutRFCTRL_MDS_Thread(void *lParam)
{
	RFCTRL_INFO *lhcd_info = (RFCTRL_INFO *)lParam;

	while(1) {

		if(lhcd_info->mds_status == DATA_GET_OK) {
			if(KstarMode) {
				rf_datafileOpen(lhcd_info);
				rfMdsPutData(lhcd_info, KSTAR_MDS);
				rf_datafileClose(lhcd_info);

				epicsThreadSleep(0.1);

				rf_datafileOpen(lhcd_info);
				rfMdsPutData(lhcd_info, LOCAL_MDS);
				rf_datafileClose(lhcd_info);
			}
			else {
				rf_datafileOpen(lhcd_info);
				rfMdsPutData(lhcd_info, LOCAL_MDS);
				rf_datafileClose(lhcd_info);
			}

			lhcd_info->mds_status = 0;
		}

		epicsThreadSleep(0.1);
	}
}

int drvRFCTRL_Configure(int start, char *ip, int port)
{
	int i;

	epicsPrintf("*** RF CTRL START ***\n");

	sprintf(lhcd_rfctrl.ip, "%s", ip);
	lhcd_rfctrl.sock = set_up_tcp( ip, RFCTRL_PORT);

	if(lhcd_rfctrl.sock >= 0) lhcd_rfctrl.con_stat = 1;

	epicsPrintf("*** RF CTRL Connection OK ***\n");

	lhcd_rfctrl.mds_status = 0;
	lhcd_rfctrl.t0 = 0.0;

	epicsThreadCreate("RFCTRL", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)GetRFCTRL_Data_Thread, &lhcd_rfctrl);

	epicsThreadCreate("RFCTRL", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)PutRFCTRL_MDS_Thread, &lhcd_rfctrl);


	return 0;
}

static const iocshArg drvRFCTRL_ConfigureArg0 = {"start", iocshArgInt};
static const iocshArg drvRFCTRL_ConfigureArg1 = {"ipaddr", iocshArgString};
static const iocshArg drvRFCTRL_ConfigureArg2 = {"port", iocshArgInt};

static const iocshArg *drvRFCTRL_ConfigureArgs[] = {
	&drvRFCTRL_ConfigureArg0,
	&drvRFCTRL_ConfigureArg1,
	&drvRFCTRL_ConfigureArg2
};

static const iocshFuncDef drvRFCTRL_ConfigureFuncDef = 
			{ "drvRFCTRL_Configure", 3, drvRFCTRL_ConfigureArgs};

static void drvRFCTRL_ConfigureCallFunc(const iocshArgBuf *args)
{
	drvRFCTRL_Configure(args[0].ival, args[1].sval, args[2].ival);
}

static void
drvRFCTRL_ConfigureRegisterCommands(void)
{
	static int firstTime = 1;
	if (firstTime) {
		iocshRegister(&drvRFCTRL_ConfigureFuncDef, drvRFCTRL_ConfigureCallFunc);
		firstTime = 0;
	}
}
epicsExportRegistrar(drvRFCTRL_ConfigureRegisterCommands);
