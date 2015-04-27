/* ------------------------------------------------------------------------- */
/* htstream.c High Throughput Streaming for ACQ196                           */
/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2006 Peter Milne, D-TACQ Solutions Ltd
 *                      <Peter dot Milne at D hyphen TACQ dot com>
                                                                               
 This program is free software; you can redistribute it and/or modify
 it under the terms of Version 2 of the GNU General Public License
 as published by the Free Software Foundation;
                                                                               
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
                                                                               
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                   */
/* ------------------------------------------------------------------------- */


/** @file htstream.c $Revision: 1.7 $ High Throughput Streaming for ACQ196. */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <epicsThread.h>

#include "drvAcq196.h"
#include "Acq196Dt100.h"

#define TIMEOUT	1000	/* @@todo ticks. should be msec. */

#if 0
#define INITIAL_TIMEOUT (24*3600*100)
#endif

#define REVID "$Revision: 1.7 $ B1000"

#define D_FILE	0

int SLOT = 2;
int acq200_debug = 0;
int timeout = TIMEOUT;
int MAXTO = 60;			/* max timeout retries */
int SLEEP_WORK = 0;
int Acq_Init_Map = 0;

unsigned long long needDataLength = 0;

#if 0
struct DeviceMapping {
	int fd;
	void *pbuf;
	char id[8];
};
#endif

int G_file;	/* file handle to write to */

tagQueueData queueData;

#if 0
static struct DeviceMapping bufAB[2] = {
	[BUFFERA] = {
		.id = "bufA"
	},
	[BUFFERB] = {
		.id = "bufB"
	}
};
#endif

void initMapping(DeviceMapping *mapping, int slot);
void acq196_closeMapping(DeviceMapping *mapping );

void initMapping(DeviceMapping *mapping, int slot)
{
	char fname[128];

	sprintf(fname, "/dev/acq200/acq200.%d.%s", slot, mapping->id);

	mapping->fd = open(fname, O_RDWR);
        if (mapping->fd < 0){
		err("failed to open %s ", fname);
		perror("open");
		exit(errno);
	}

	mapping->pbuf = mmap(0, BUFLEN, 
				PROT_READ|PROT_WRITE, 
				MAP_PRIVATE|MAP_NONBLOCK, 
				mapping->fd, 0);

	if (mapping->pbuf == MAP_FAILED){
		err("mmap() failed");
		perror("mmap");
		exit(errno);
	}
}

void acq196_closeMapping(DeviceMapping *mapping )
{
    munmap(mapping->pbuf, BUFLEN);
    close(mapping->fd);
}

#if 0
static void fileWork(int index, int len)
{
	tagEllNode *pNode1 = NULL;

	dbg(1, "index %d len %d", index, len);

#if D_FILE
	write(G_file, bufAB[index].pbuf, len);
#else
	pNode1 = (tagEllNode *)ellFirst(pNodeList);
	ellDelete(pNodeList, &pNode1->node);
        memcpy(pNode1->data, bufAB[index].pbuf, len);
	queueData.opcode = OP_CODE_DAQ_WRITE;
        queueData.size = len;
        queueData.pNode = pNode1;
	epicsMessageQueueSend(DaqQueueId, (void*)&queueData, sizeof(tagQueueData)); 
#endif
}
#endif

static void defaultWork(int index, int len)
{
	
}

int action(Acq196Drv *acqDrv) {
#if 0
	struct BufferAB_Ops ops = {
		.ci.timeout = INITIAL_TIMEOUT
	};
#endif
	int next = BUFFERA;
	char status[80];
	int syscalls = 1;
	int timeouts = 0;
#define FD acqDrv->drv_bufAB[BUFFERA].fd

	int started = 0;
	int rc = 0;
	int loop = 1;

	unsigned long long total = 0;

	tagEllNode *pNode1 = NULL;

	dbg(1, "timeout %d", acqDrv->ops.ci.timeout);

	initMapping(acqDrv->drv_bufAB+0, acqDrv->slot);
	initMapping(acqDrv->drv_bufAB+1, acqDrv->slot);

	while(loop){
		rc = ioctl(FD, ACQ200_BUFFER_AB_IOWR, &acqDrv->ops);

		if (rc >= 0){
			if (acq200_debug && timeouts != 0){
				printf("\n");
			}
			timeouts = 0;
			started = 1;
            acqDrv->softTrg = 1;
		}else if (rc == -ETIMEDOUT){
			if (started && ++timeouts >= MAXTO){
				perror("TIMEOUT?");
				acq196_closeMapping(acqDrv->drv_bufAB+0);
				acq196_closeMapping(acqDrv->drv_bufAB+1);
				return -1;
			}else{
				continue;
			}
		}else{
			printf("ioctl(ACQ200_BUFFER_AB_IOWR) %d (signal)", rc);
			acq196_closeMapping(acqDrv->drv_bufAB+0);
			acq196_closeMapping(acqDrv->drv_bufAB+1);
			return -1;
		}
		
		acqDrv->ops.ci.recycle = 0;


		if (acqDrv->ops.ds.full&BUFFER_FLAG_LUT[next]){
			total += acqDrv->ops.ds.len[next];
			printf("N:%d %8d %8d\n", next, acqDrv->ops.ds.sid[next], acqDrv->ops.ds.len[next]);

			acqDrv->ops.ci.recycle |= BUFFER_FLAG_LUT[next];

			pNode1 = (tagEllNode *)ellFirst(acqDrv->pNodeList);
	        ellDelete(acqDrv->pNodeList, &pNode1->node);
	        memcpy(pNode1->data, acqDrv->drv_bufAB[next].pbuf, acqDrv->ops.ds.len[next]);
	        queueData.opcode = OP_CODE_DAQ_WRITE;
	        queueData.size = acqDrv->ops.ds.len[next];
	        queueData.pNode = pNode1;
	        epicsMessageQueueSend(acqDrv->DaqQueueId, (void*)&queueData, sizeof(tagQueueData));

			if(total >= needDataLength && loop == 0) {
				if(!(acqDrv->ops.ds.full&BUFFER_FLAG_LUT[!next])) {
					loop = 0;
				}
			}

			next = !next;

			if (acqDrv->ops.ds.full&BUFFER_FLAG_LUT[next]){
				total += acqDrv->ops.ds.len[next];
				printf("N:%d %8d %8d\n", next, acqDrv->ops.ds.sid[next], acqDrv->ops.ds.len[next]);
				acqDrv->ops.ci.recycle |= BUFFER_FLAG_LUT[next];

				pNode1 = (tagEllNode *)ellFirst(acqDrv->pNodeList);
		        ellDelete(acqDrv->pNodeList, &pNode1->node);
	        	memcpy(pNode1->data, acqDrv->drv_bufAB[next].pbuf, acqDrv->ops.ds.len[next]);
		        queueData.opcode = OP_CODE_DAQ_WRITE;
		        queueData.size = acqDrv->ops.ds.len[next];   
		        queueData.pNode = pNode1;
		        epicsMessageQueueSend(acqDrv->DaqQueueId, (void*)&queueData, sizeof(tagQueueData));

                if(total >= needDataLength && loop == 0) {
                    if(!(acqDrv->ops.ds.full&BUFFER_FLAG_LUT[!next])) {
                        loop = 0;
                    }
                }

				next = !next;
			}
		}

		if (acqDrv->ops.ds.full&FULL_EOF || total >= needDataLength){
			acq196_closeMapping(acqDrv->drv_bufAB+0);
      		acq196_closeMapping(acqDrv->drv_bufAB+1);
			return total;
		}
	}

	acq196_closeMapping(acqDrv->drv_bufAB+0);
    acq196_closeMapping(acqDrv->drv_bufAB+1);

	fprintf(stderr, "Total bytes: %Lu\n", total);

	return total;
}

int acqRunThread(Acq196Drv *acqDrv, int num)
{	
	unsigned long long total = 0;

#if D_FILE
	system("rm -rf /media/data1/acq196.2.dat");

	G_file = open(DATA_FILE, O_WRONLY|O_CREAT);

	if (G_file < 0){
		printf("-- htstream file open error --\n");
		return -1;
	}
#endif
    needDataLength = (unsigned long long)((acqDrv->t1 - acqDrv->t0) * (acqDrv->clock * 192));

	printf("Need Data Length [%d] --> %lld, T0 --> %f, T1 --> %f\n", acqDrv->card, needDataLength, acqDrv->t0, acqDrv->t1);

    acqDrv->st = 1;
	printf("acq196 slot number [%d]\n", acqDrv->slot);

	total = action(acqDrv);

    if(total <= 0) return -1;

	acqDrv->totalNum = total;
    acqDrv->softTrg = 0;
    acqDrv->st = 0;

	printf("-- acqRunThread Return [%Lu]--\n", total);

#if D_FILE
	close(G_file);
#endif
    return 0;
}
