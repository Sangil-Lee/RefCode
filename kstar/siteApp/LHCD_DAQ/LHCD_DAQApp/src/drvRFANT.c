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
#include "rfant.h"

RFANT_INFO	lhcd_rfant;

void datafileOpen(RFANT_INFO *lhcd_info, int mode);
void datafileClose(RFANT_INFO *lhcd_info);

void datafileOpen(RFANT_INFO *lhcd_info, int mode)
{
	int i;
	char szTemp[256];

	for(i=0; i < RFANT_MAX_OFF; i++) {
		sprintf(szTemp, "/tmp/rfant_iqi_%d.dat", i);
		if(mode == 1) lhcd_info->iq_i_fp[i] = fopen(szTemp, "w+");
		else lhcd_info->iq_i_fp[i] = fopen(szTemp, "r");
	}
	for(i=0; i < RFANT_MAX_OFF; i++) {
		sprintf(szTemp, "/tmp/rfant_iqq_%d.dat", i);
		if(mode == 1) lhcd_info->iq_q_fp[i] = fopen(szTemp, "w+");
		else lhcd_info->iq_q_fp[i] = fopen(szTemp, "r");
	}
	for(i=0; i < RFANT_MAX_POW; i++) {
		sprintf(szTemp, "/tmp/rfant_fwd_%d.dat", i);
		if(mode == 1) lhcd_info->fwd_fp[i] = fopen(szTemp, "w+");
		else lhcd_info->fwd_fp[i] = fopen(szTemp, "r");
	}
	for(i=0; i < RFANT_MAX_POW; i++) {
		sprintf(szTemp, "/tmp/rfant_rev_%d.dat", i);
		if(mode == 1) lhcd_info->rev_fp[i] = fopen(szTemp, "w+");
		else lhcd_info->rev_fp[i] = fopen(szTemp, "r");
	}
}

void datafileClose(RFANT_INFO *lhcd_info)
{
	int i;

	for(i=0; i < RFANT_MAX_OFF; i++) {
		if(lhcd_info->iq_i_fp[i] != NULL)
			fclose(lhcd_info->iq_i_fp[i]);
	}
	for(i=0; i < RFANT_MAX_OFF; i++) {
		if(lhcd_info->iq_q_fp[i] != NULL)
			fclose(lhcd_info->iq_q_fp[i]);
	}
	for(i=0; i < RFANT_MAX_POW; i++) {
		if(lhcd_info->fwd_fp[i] != NULL)
			fclose(lhcd_info->fwd_fp[i]);
	}
	for(i=0; i < RFANT_MAX_POW; i++) {
		if(lhcd_info->rev_fp[i] != NULL)
			fclose(lhcd_info->rev_fp[i]);
	}
}

static void GetRFANT_Data_Thread(void *lParam)
{
	RFANT_INFO *lhcd_info = (RFANT_INFO *)lParam;

	char buff[4];
	int ret;
	int i, j, ind;
	int data_count = 0;
	char szTemp[256];

	epicsPrintf("*** RFANT Thread Start ***\n");

	while(1) {
		buff[0] = 0x0;

		ret = recv(lhcd_info->sock, buff, 1, 0);

		epicsPrintf("*** RFANT ... RECV [%d] --> %x ***\n", ret, buff[0]);

		if(ret <= 0) {
			epicsPrintf("*** RFANT Recv... Error ***\n");
			close(lhcd_info->sock);
			lhcd_info->sock = -1;
			lhcd_info->con_stat = 0;

			epicsThreadSleep(1);
			lhcd_info->sock = set_up_tcp(lhcd_info->ip, RFANT_PORT);

			if(lhcd_info->sock > 0) lhcd_info->con_stat = 1;
			continue;
		}

		if(buff[0] == MDSDATA_INIT_HEAD) {
			char s_temp[sizeof(RFANT_MDS_HEAD)];
			RFANT_MDS_HEAD  head;

			ret = recv_data_get(lhcd_info->sock, s_temp, sizeof(s_temp));

			printf("-- head return size : %d --\n", ret);

			memcpy(&head, s_temp, sizeof(RFANT_MDS_HEAD));

			for(i=0; i < RFANT_MAX_POW; i++) {
				lhcd_info->fwd_1st[i] = head.fwd_1st[i];
				lhcd_info->fwd_2st[i] = head.fwd_2st[i];
				lhcd_info->fwd_its[i] = head.fwd_its[i];
				lhcd_info->fwd_ofs[i] = head.fwd_ofs[i];
				lhcd_info->fwd_loss[i] = head.fwd_loss[i];

				lhcd_info->rev_1st[i] = head.rev_1st[i];
				lhcd_info->rev_2st[i] = head.rev_2st[i];
				lhcd_info->rev_its[i] = head.rev_its[i];
				lhcd_info->rev_ofs[i] = head.rev_ofs[i];
				lhcd_info->rev_loss[i] = head.rev_loss[i];
			}
			for(i=0; i < RFANT_MAX_OFF; i++) {
				lhcd_info->iq_i_offset[i] = head.iqi_offset[i];
				lhcd_info->iq_q_offset[i] = head.iqq_offset[i];
				lhcd_info->iq_gain[i] = head.iq_gain[i];
			}

			data_count = 0;
			lhcd_info->data_count = head.count + 1;
			if(head.samp_clk == 0) lhcd_info->samplclk = 1000.0;
			else if(head.samp_clk == 1) lhcd_info->samplclk = 2000.0;
			else if(head.samp_clk == 2) lhcd_info->samplclk = 5000.0;
			else lhcd_info->samplclk = 1000.0;

			printf("RFANT data count : %d\n", lhcd_info->data_count);
			printf("RFANT sample rate : %f Hz\n", lhcd_info->samplclk);
			printf("RFANT fwd loss : %f\n", lhcd_info->fwd_loss[0]);
			printf("RFANT IQ I OFF : %f\n", lhcd_info->iq_i_offset[0]);
			printf("RFANT IQ Q OFF : %f\n", lhcd_info->iq_q_offset[0]);
			printf("RFANT IQ Q GAIN: %f\n", lhcd_info->iq_gain[0]);
			printf("RFANT save data file open...\n");
			datafileOpen(lhcd_info, 1);
		}
		else if(buff[0] == MDSDATA_ARRY_DATA) {
			RFANT_MDS_RAW u_data[100];
			char  temp[sizeof(u_data)];

			recv_data_get(lhcd_info->sock, temp, sizeof(temp));

			memcpy(&u_data, temp, sizeof(temp));

			printf("*** data save ***\n");
			for(i=0; i < 100; i++) {

#if 0  /* IQ - old */
				for(j=0; j < RFANT_MAX_OFF; j++) {
					if(lhcd_info->iq_i_fp[j] != NULL) {
						fwrite(&u_data[i].iq_i[j], sizeof(uint16), 1, lhcd_info->iq_i_fp[j]);
						fflush(lhcd_info->iq_i_fp[j]);
					}
				}
				for(j=0; j < RFANT_MAX_OFF; j++) {
					if(lhcd_info->iq_q_fp[j] != NULL) {
						fwrite(&u_data[i].iq_q[j], sizeof(uint16), 1, lhcd_info->iq_q_fp[j]);
						fflush(lhcd_info->iq_q_fp[j]);
					}
				}
#endif
				for(j=0; j < RFANT_MAX_POW; j++) {
					if(lhcd_info->fwd_fp[j] != NULL) {
						fwrite(&u_data[i].fwd[j], sizeof(uint16), 1, lhcd_info->fwd_fp[j]);
						fflush(lhcd_info->fwd_fp[j]);
					}
				}
				for(j=0; j < RFANT_MAX_POW; j++) {
					if(lhcd_info->rev_fp[j] != NULL) {
						fwrite(&u_data[i].rev[j], sizeof(uint16), 1, lhcd_info->rev_fp[j]);
						fflush(lhcd_info->rev_fp[j]);
					}
				}
			}

			printf(" data index [%d] \n", data_count);

			data_count += 100;

			if(data_count >= (lhcd_info->data_count)) {

				printf("--- data save file close ---\n");

				datafileClose(lhcd_info);

				data_count = 0;
				lhcd_info->mds_status = RFANT_DATA_GET_OK;

				printf("--- data save file close ok...\n");
			}
		}

		epicsThreadSleep(0.01);
	}
}

static void PutRFANT_MDS_Thread(void *lParam)
{
	RFANT_INFO *lhcd_info = (RFANT_INFO *)lParam;
	char szTemp[256];
	int i;

	while(1) {

		if(lhcd_info->mds_status == RFANT_DATA_GET_OK) {

#if 1
			if(KstarMode) {
				datafileOpen(lhcd_info, 0);
				rfantMdsPutData(lhcd_info, KSTAR_MDS);
				datafileClose(lhcd_info);
				epicsThreadSleep(0.1);
				datafileOpen(lhcd_info, 0);
				rfantMdsPutData(lhcd_info, LOCAL_MDS);
				datafileClose(lhcd_info);
			}
			else {
				datafileOpen(lhcd_info, 0);
				rfantMdsPutData(lhcd_info, LOCAL_MDS);
				datafileClose(lhcd_info);
			}
#endif

			lhcd_info->mds_status = 0;
		}

		epicsThreadSleep(0.1);
	}
}

int drvRFANT_Configure(int start, char *ip, int port)
{
	int i;

	epicsPrintf("*** RF ANT START ***\n");

	sprintf(lhcd_rfant.ip, "%s", ip);
	lhcd_rfant.sock = set_up_tcp( ip, RFANT_PORT);

	if(lhcd_rfant.sock >= 0) {
		lhcd_rfant.con_stat = 1;
		epicsPrintf("*** RF ANT Connection OK ***\n");
	}
	else {
		epicsPrintf("*** RF ANT Disconnect.... ***\n");
	}

	lhcd_rfant.mds_status = 0;
	lhcd_rfant.t0 = 0.0;

	epicsThreadCreate("RFANT_MSG", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)GetRFANT_Data_Thread, &lhcd_rfant);

	epicsThreadCreate("RFANT_MDS", 
			epicsThreadPriorityHigh,
			epicsThreadGetStackSize(epicsThreadStackSmall),
			(EPICSTHREADFUNC)PutRFANT_MDS_Thread, &lhcd_rfant);


	return 0;
}

static const iocshArg drvRFANT_ConfigureArg0 = {"start", iocshArgInt};
static const iocshArg drvRFANT_ConfigureArg1 = {"ipaddr", iocshArgString};
static const iocshArg drvRFANT_ConfigureArg2 = {"port", iocshArgInt};

static const iocshArg *drvRFANT_ConfigureArgs[] = {
	&drvRFANT_ConfigureArg0,
	&drvRFANT_ConfigureArg1,
	&drvRFANT_ConfigureArg2
};

static const iocshFuncDef drvRFANT_ConfigureFuncDef = 
			{ "drvRFANT_Configure", 3, drvRFANT_ConfigureArgs};

static void drvRFANT_ConfigureCallFunc(const iocshArgBuf *args)
{
	drvRFANT_Configure(args[0].ival, args[1].sval, args[2].ival);
}

static void
drvRFANT_ConfigureRegisterCommands(void)
{
	static int firstTime = 1;
	if (firstTime) {
		iocshRegister(&drvRFANT_ConfigureFuncDef, drvRFANT_ConfigureCallFunc);
		firstTime = 0;
	}
}
epicsExportRegistrar(drvRFANT_ConfigureRegisterCommands);
