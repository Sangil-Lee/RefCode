#include "acqHTstream.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h> /* for ETIMEDOUT */
#include <sys/ioctl.h> 

//#include "statCommon.h"
/* I don't use "epicsPrintf" i want get print out immediately             2009. 10. 17 */
//int acq200_debug = 0;

#if 0 
int acq196_initMapping(DeviceMapping *mapping, int slot)
{
	char fname[128];

	sprintf(fname, "/dev/acq200/acq200.%d.%s", slot, mapping->id);

	mapping->fd = open(fname, O_RDWR);
        if (mapping->fd < 0){
		err("failed to open %s ", fname);
		printf("\n>>> initMapping(slot:%d): open() ... ERROR!\n", slot );
		return WR_ERROR;
	}

	mapping->pbuf = mmap(0, BUFLEN, 
				PROT_READ|PROT_WRITE, 
				MAP_PRIVATE|MAP_NONBLOCK, 
				mapping->fd, 0);

	if (mapping->pbuf == MAP_FAILED){
		err("mmap() failed");
		printf("\n>>> initMapping(slot:%d) : mmap() ... ERROR!\n", slot );
		return WR_ERROR;
	}
	return WR_OK;
}

void acq196_closeMapping( DeviceMapping *mapping )
{
	munmap(mapping->pbuf, BUFLEN);
	close(mapping->fd);
}
#endif

#if 0 
// TG Remove
int acq196_htstream(ST_STD_device *pSTDdev)
{
	int next = BUFFERA;
	int timeouts = 0;
/*#define FD (ST_ACQ196*)(pSTDdev->pUser)->drv_bufAB[BUFFERA].fd */
	int started = 0;
	int rc=0;
	unsigned long int syscalls = 0;
	ST_ACQ196 *pAcq196;
	pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

#define FD pAcq196->drv_bufAB[BUFFERA].fd

	
	ST_User_Buff_node queueData;
	ST_buf_node *pbufferNode = NULL;
//	ST_threadCfg *pDAQST_threadConfig=NULL;

//	pDAQST_threadConfig = (ST_threadCfg*) pSTDdev.ST_BuffThread;
//	if( !pDAQST_threadConfig ) {
//		printf("ERROR!  %s can't recive Buff Thread\n", pSTDdev->taskName );
//		return WR_ERROR;
//	}	

	if( acq196_initMapping(pAcq196->drv_bufAB+0, (int)pAcq196->slot) == WR_ERROR )
		printf("ERROR!  acq196_initMapping() buf 0\n");
	if( acq196_initMapping(pAcq196->drv_bufAB+1, (int)pAcq196->slot) == WR_ERROR )
		printf("ERROR!  acq196_initMapping() buf 1\n");

#if PRINT_DAQ_DEBUG
	epicsPrintf("\n %s: pSTDdev: %p, pAcq196: %p , pST_BuffThread: %p\n",pSTDdev->taskName, pSTDdev, pAcq196, pSTDdev.ST_BuffThread );
#endif

	if( acq196_setDIO_rising(pSTDdev) == WR_ERROR ) {
		printf("\n>>> drvACQ196_RUN_start: acq196_setDIO_rising()... ERROR" );
		acq196_closeMapping(pAcq196->drv_bufAB+0);
		acq196_closeMapping(pAcq196->drv_bufAB+1);
		return WR_ERROR;
	}


	while( pSTDdev->StatusDev & DEV_IN_LOOP_HTSTREAM )
	{
#if DEBUG_PRINT_HTSTREAM
		printf("\n>>> acq196_htstream: call ioctl(xxx)...stopmsg:%d, %p. ", pAcq196->gotDAQstopMsg, pAcq196); 
		rc = ioctl(FD, ACQ200_BUFFER_AB_IOWR, &(pAcq196->drv_ops) );
		printf("return(%d)... msg:%d, %p \n", rc, pAcq196->gotDAQstopMsg, pAcq196);
#else
		rc = ioctl(FD, ACQ200_BUFFER_AB_IOWR, &(pAcq196->drv_ops) );
#endif
	
		if (rc >= 0)
		{
			if (acq200_debug && timeouts != 0){
				printf("acq200_debug && timeouts != 0\n");
			}
			timeouts = 0;
			started = 1;
		} else if (rc == -ETIMEDOUT){
			if (acq200_debug){
				fputc('.', stderr); fflush(stderr);
			}
			if (started && ++timeouts >= MAXTO){
				perror("TIMEOUT?");
				acq196_closeMapping(pAcq196->drv_bufAB+0);
				acq196_closeMapping(pAcq196->drv_bufAB+1);
				#if USE_FILE_POINTER
				fclose(pAcq196->fpRaw);
				#else
				close(pAcq196->R_file);
				#endif
				return rc;
			}else{
				continue;
			}
		} else {
			perror("ERROR ioctl(ACQ200_BUFFER_AB_IOWR)");
			acq196_closeMapping(pAcq196->drv_bufAB+0);
			acq196_closeMapping(pAcq196->drv_bufAB+1);
			#if USE_FILE_POINTER
				fclose(pAcq196->fpRaw);
				#else
				close(pAcq196->R_file);
				#endif
			return rc;
		}
		pAcq196->drv_ops.ci.recycle = 0;
/*		queueData.u32Cnt	= syscalls; */

		if (pAcq196->drv_ops.ds.full&BUFFER_FLAG_LUT[next])
		{
			pAcq196->rawDataSize += pAcq196->drv_ops.ds.len[next];
#if DEBUG_PRINT_HTSTREAM
			printf("1N:%d %8d %8d :", next, pAcq196->drv_ops.ds.sid[next], pAcq196->drv_ops.ds.len[next]);
#endif
			pAcq196->drv_ops.ci.recycle |= BUFFER_FLAG_LUT[next];

			pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
			ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);
			memcpy( pbufferNode->data, pAcq196->drv_bufAB[next].pbuf, pAcq196->drv_ops.ds.len[next]);
			queueData.pNode		= pbufferNode;
			queueData.size		= pAcq196->drv_ops.ds.len[next];
			queueData.opcode	= QUEUE_OPCODE_NORMAL;
			epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));

			if( pAcq196->gotDAQstopMsg )
			{
				if( pAcq196->rawDataSize >= pAcq196->needDataSize )
				{
					queueData.pNode = NULL;
					queueData.opcode	= QUEUE_OPCODE_CLOSE_FILE;
					/* if there is some data in next buffer, then do not send EOB message */
					if( !(pAcq196->drv_ops.ds.full&BUFFER_FLAG_LUT[!next]) )
					{
						pSTDdev->StatusDev &= ~DEV_IN_LOOP_HTSTREAM;
#if PRINT_DMA_NUM
						printf("\n>>> [2.1]%s: send EOB message.\n", pSTDdev->taskName);
#endif
						epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));
						pAcq196->gotDAQstopMsg = 0;
					} else {
						printf("\n>>> %s: got Stop Msg in FirstBuffer. will be passed to next buffer.\n", pSTDdev->taskName);
					}
				}
#if DEBUG_PRINT_HTSTREAM
				else{
					printf("\n>>> %s'st gotDAQstopMsg but need more data!\n", pSTDdev->taskName);
				}
#endif
			}
/*			write(pSTDdev->R_file, pSTDdev->drv_bufAB[next].pbuf, pSTDdev->drv_ops.ds.len[next]); */

/*			printf("index %d len %d\n", next, pSTDdev->drv_ops.ds.len[next]); */

			next = !next;

			if (pAcq196->drv_ops.ds.full&BUFFER_FLAG_LUT[next])
			{
				pAcq196->rawDataSize += pAcq196->drv_ops.ds.len[next];
#if DEBUG_PRINT_HTSTREAM
				printf("2N:%d %8d %8d :", next, pAcq196->drv_ops.ds.sid[next], pAcq196->drv_ops.ds.len[next]);
#endif
				pAcq196->drv_ops.ci.recycle |= BUFFER_FLAG_LUT[next];
				
				pbufferNode = (ST_buf_node *)ellFirst(pSTDdev->pList_BufferNode);
				ellDelete(pSTDdev->pList_BufferNode, &pbufferNode->node);
				memcpy( pbufferNode->data, pAcq196->drv_bufAB[next].pbuf, pAcq196->drv_ops.ds.len[next]);
				queueData.pNode		= pbufferNode;
				queueData.size			= pAcq196->drv_ops.ds.len[next];
				queueData.opcode	= QUEUE_OPCODE_NORMAL;
				epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));

				if( pAcq196->gotDAQstopMsg )
				{
					if( pAcq196->rawDataSize >= pAcq196->needDataSize ) 
					{
						pSTDdev->StatusDev &= ~DEV_IN_LOOP_HTSTREAM;
						queueData.pNode = NULL;
						queueData.opcode	= QUEUE_OPCODE_CLOSE_FILE;
#if PRINT_DMA_NUM						
						printf("\n>>> [2.2]%s: send EOB message.\n", pSTDdev->taskName);
#endif
						epicsMessageQueueSend(pSTDdev->ST_RingBufThread.threadQueueId, (void*) &queueData, sizeof(ST_User_Buff_node));
						pAcq196->gotDAQstopMsg = 0;
					}
#if DEBUG_PRINT_HTSTREAM
					else{
						printf("\n>>> %s'nd gotDAQstopMsg but need more data!\n", pSTDdev->taskName);
					}
#endif
				}
/*				write(pSTDdev->R_file, pSTDdev->drv_bufAB[next].pbuf, pSTDdev->drv_ops.ds.len[next]); */

/*				printf("index %d len %d\n", next, pSTDdev->drv_ops.ds.len[next]); */

				next = !next;
			} /* second buffer task... */
			
		} /* first buffer task... */

		if (pAcq196->drv_ops.ds.full&FULL_EOF){
			break;
		} /* if (pSTDdev->drv_ops.ds.full&FULL_EOF){ */

		syscalls++;
	}	/* while( pSTDdev->StatusDev & DEV_IN_LOOP_HTSTREAM ) */
/*	move to ring buffer for waiting time consume process..
	acq196_closeMapping(pSTDdev->drv_bufAB+0);
	acq196_closeMapping(pSTDdev->drv_bufAB+1);
*/
	printf(">>> %s: Total/Need::  %.2f/%.2f MB Cnt:%lu\n", pSTDdev->taskName, 
						(float)pAcq196->rawDataSize/1024.0/1024.0,
						pAcq196->needDataSize/1024.0/1024.0,	syscalls);

	return WR_OK;
}
#endif


int acq196_target_status(void)
{
	FILE *pfp;
	char buff[128];

	// char strCmd[128]={"nc -i 10 10.0.196.100 100 53535"};
	char strCmd[128]={"/home/dt100/bin/connect-statemon"};
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>>: nc 10.0.196.100 100 53535 ... failed\n");
		return -1;
	}
	while(fgets(buff, 128, pfp) != NULL ) {
		printf("\n>>> : %s\n",buff);
	} pclose(pfp);
	return WR_OK;
}
       

int acq196_stream_to_elastic_store(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	char strCmd[128]={"/home/dt100/bin/stream-to-elastic-store"};
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: stream-to-elastic-store ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	return WR_OK;
}

int acq196_set_SlaveMode(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

	if( pAcq196->clockSource != CLOCK_EXTERNAL ) {
		printf("\nERROR: %s: clock source is not external! (%d)\n", pSTDdev->taskName, pAcq196->clockSource );
		return WR_ERROR;
	}
	
	if( acq196_set_externalClk_DI0(pSTDdev) == WR_ERROR ) return WR_ERROR;
	
	if( acq196_setDIO_input(pSTDdev)==WR_ERROR) return WR_ERROR;

	if( acq196_route_PXI_to_FPGA(pSTDdev) == WR_ERROR ) return WR_ERROR;
		
	if( acq196_set_TriggerConfig(pSTDdev) == WR_ERROR ) return WR_ERROR;

	return WR_OK;
}


int acq196_set_MasterMode(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

	if( pAcq196->clockSource == CLOCK_INTERNAL ) {
		if( acq196_set_internalClk_DO0(pSTDdev)==WR_ERROR) return WR_ERROR;
		if( acq196_setDIO_falling(pSTDdev)== WR_ERROR) return WR_ERROR;	
		if( acq196_route_FPGA_to_PXI(pSTDdev)==WR_ERROR) return WR_ERROR;
	}
	else if( pAcq196->clockSource == CLOCK_EXTERNAL ) {
		if( acq196_set_externalClk_DI0(pSTDdev) == WR_ERROR ) return WR_ERROR;
		if( acq196_setDIO_input(pSTDdev) == WR_ERROR ) return WR_ERROR;
		if( acq196_route_LEMO_to_FpgaPXI(pSTDdev) == WR_ERROR ) return WR_ERROR;
	}
	else {
		printf("\nERROR: acq196_set_MasterMode: clock source is not defined! (%d)\n", pAcq196->clockSource );
		return WR_ERROR;
	}
	
	if( acq196_set_TriggerConfig(pSTDdev)==WR_ERROR) return WR_ERROR;
	
	return WR_OK;
}

int acq196_set_SingleMode(ST_STD_device *pSTDdev)
{
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

	if( pAcq196->clockSource == CLOCK_INTERNAL ) {
		if( acq196_set_internalClk_DO0(pSTDdev)==WR_ERROR) return WR_ERROR;
		if( acq196_setDIO_falling(pSTDdev)== WR_ERROR) return WR_ERROR;
		if( acq196_route_FPGA_to_FPGA(pSTDdev)==WR_ERROR) return WR_ERROR;
	}
	else if( pAcq196->clockSource == CLOCK_EXTERNAL ) {
		if( acq196_set_externalClk_DI0(pSTDdev) == WR_ERROR ) return WR_ERROR;
		if( acq196_setDIO_input(pSTDdev) == WR_ERROR ) return WR_ERROR;
		if( acq196_route_LEMO_to_FPGA(pSTDdev) == WR_ERROR ) return WR_ERROR;
	}
	else {
		printf("\nERROR: acq196_set_SingleMode: clock source is not defined! (%d)\n", pAcq196->clockSource );
		return WR_ERROR;
	}
	
	if( acq196_set_TriggerConfig(pSTDdev)== WR_ERROR) return WR_ERROR;

	return WR_OK;
}


int acq196_setDIO_input(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -b %d -- setDIO ------ ", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: setDIO ------ ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	return WR_OK;
}

int acq196_setDIO_rising(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -b %d -- setDIO 1--1-- ", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: setDIO 1--1-- ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	return WR_OK;
}
int acq196_setDIO_falling(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -b %d -- setDIO 0--0-- ", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: setDIO 0--0-- ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) {
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	}pclose(pfp);
	
	return WR_OK;
}

int acq196_channel_Block_Mask(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

/*	sprintf(strCmd, "acqcmd -s %d set.channelBlockMask %d", pAcq196->slot, pAcq196->nChannelMask);  */
	sprintf(strCmd, "acqcmd -s %d set.channelBlockMask 111", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> set.channelBlockMask ... failed\n");
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	kLog (K_MON, "[acqcmd] %d : acq196_channel_Block_Mask cmd(%s) \n",  pAcq196->slot, strCmd);
	return WR_OK;
}

int acq196_route_LEMO_to_FPGA(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

	sprintf(strCmd, "acqcmd -s %d set.route d0 in lemo out fpga", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> set.route d0 in lemo out fpga ... failed\n");
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	kLog (K_MON, "[acqcmd] %d : acq196_route_LEMO_to_FPGA(%s) \n",  pAcq196->slot, strCmd);
	
	sprintf(strCmd, "acqcmd -s %d set.route d3 in lemo out fpga ", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> set.route d3 in lemo out fpga ... failed\n");
		return WR_ERROR;
	}	
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	kLog (K_MON, "[acqcmd] %d : acq196_route_LEMO_to_FPGA 2(%s) \n",  pAcq196->slot, strCmd);

	return WR_OK;
}


int acq196_route_LEMO_to_FpgaPXI(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

	sprintf(strCmd, "acqcmd -s %d set.route d0 in lemo out fpga pxi", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> set.route d0 in lemo out fpga pxi ... failed\n");
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_LEMO_to_FpgaPXI(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

	
	sprintf(strCmd, "acqcmd -s %d set.route d3 in lemo out fpga pxi", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> set.route d3 in lemo out fpga pxi ... failed\n");
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_LEMO_to_FpgaPXI 2(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	return WR_OK;	
}

int acq196_route_FPGA_to_PXI(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -s %d set.route d0 in fpga out pxi", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.route d0 in fpga out pxi ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_FPGA_to_PXI(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	sprintf(strCmd, "acqcmd -s %d set.route d3 in fpga out pxi", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.route d3 in fpga out pxi ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_FPGA_to_PXI 2(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

	return WR_OK;	
}

int acq196_route_FPGA_to_FPGA(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -s %d set.route d0 in fpga out fpga", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.route d0 in fpga ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_FPGA_to_FPGA(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	sprintf(strCmd, "acqcmd -s %d set.route d3 in fpga out fpga", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.route d3 in fpga ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_FPGA_to_FPGA 2(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

	return WR_OK;
}

int acq196_route_PXI_to_FPGA(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -s %d set.route d0 in pxi out fpga", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.route d0 in pxi out fpga ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_route_PXI_to_FPGA(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	sprintf(strCmd, "acqcmd -s %d set.route d3 in pxi out fpga", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.route d3 in pxi out fpga ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
		
	return WR_OK;
}


int acq196_set_ARM(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char buff[128]; 
	char strCmd[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;

/* TG change	sprintf(strCmd, "acqcmd -b %d -- setArm", pAcq196->slot ); */
	sprintf(strCmd, "acqcmd -b %d setArm", pAcq196->slot );
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> acq196_set_ARM ... failed\n");
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_set_ARM(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

//	system(strCmd);

	return WR_OK;
}

int acq196_set_MODE(ST_STD_device *pSTDdev)
{
	FILE *pfp;	
	char buff[128];
	char strCmd[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
		
/* TG.Remove	sprintf(strCmd, "acqcmd -b %d setModeTriggeredContinuous 10000 10000", pAcq196->slot);   */
	sprintf(strCmd, "acqcmd -b %d setMode SOFT_CONTINUOUS 1024", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: setMode SOFT_CONTINUOUS 1024... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_set_MODE(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
//	system(strCmd);

	return WR_OK;
}
int acq196_set_ABORT(ST_STD_device *pSTDdev)
{
	FILE *pfp;	
	char buff[128]; 
	char strCmd[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
		
/* TG change 	sprintf(strCmd, "acqcmd -b %d -- setAbort", pAcq196->slot );  */
	sprintf(strCmd, "acqcmd -b %d setAbort", pAcq196->slot );
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> acq196_set_ABORT ... failed\n");
		return WR_ERROR;
	}  
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	kLog (K_MON, "[acqcmd] %d : acq196_set_ABORT(%s) \n", pAcq196->slot, strCmd);
//	system(strCmd);

	return WR_OK;
}

int acq196_set_internalClk_DO0(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -b %d setInternalClock %d", pAcq196->slot, pSTDdev->ST_Base.nSamplingRate);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: setInternalClock ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_set_internalClk_DO0(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	return WR_OK;
}

int acq196_set_externalClk_DI0(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
/*     Modify TG Lee
 	sprintf(strCmd, "acqcmd -b %d setExternalClock DI0 %d", pAcq196->slot, pAcq196->clockDivider); 
	acqcmd -b 100 setExternalClock DI0 0 => ACQ32:ERROR bad arg "0" div wanted

*/
	sprintf(strCmd, "acqcmd -b %d setExternalClock DI0", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: acqcmd -b %d setExternalClock DI0", pSTDdev->taskName, pAcq196->slot);
		return WR_ERROR;
	} else kLog (K_MON, "[acqcmd] %d : acq196_set_externalClk_DI0(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	return WR_OK;
}

int acq196_set_TriggerConfig(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -s %d set.trig DI3 rising", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.trig DI3 rising ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}else kLog (K_MON, "[acqcmd] %d : acq196_set_TriggerConfig(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
#if 0
// TG Remove -- error ??? I have to check 	
	sprintf(strCmd, "acqcmd -s %d set.event0 DI3 falling", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.event0 DI3 falling ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	}else kLog (K_MON, "[acqcmd] %d : acq196_set_TriggerConfig 2(%s) \n",
                                pAcq196->slot, strCmd); 
	while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
#endif
		
	return WR_OK;
}

int acq196_initialize(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;


	acq196_set_ABORT(pSTDdev);
	acq196_set_MODE(pSTDdev);

// TG. remove because create the mode function
	sprintf(strCmd, "acqcmd -b %d setModeTriggeredContinuous 10000 10000", pAcq196->slot);   
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: setModeTriggeredContinuous ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);


/*	sprintf(strCmd, "acqcmd -s %d set.dtacq_drv FIFERR 0x00022040", pAcq196->slot); */
// TG Temove I don't nedd this function
	sprintf(strCmd, "acqcmd -s %d set.dtacq_drv FIFERR 0x00000000", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.dtacq_drv FIFERR 0x00022040 ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

	
#if 0	
	sprintf(strCmd, "acqcmd -s %d set.sys /sys/module/acq200_mu/parameters/max_dma %d", pAcq196->slot, pAcq196->n32_maxdma);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.sys max_dma %d ... failed\n", pSTDdev->taskName, pAcq196->n32_maxdma);
		return WR_ERROR;
	} while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
	
	sprintf(strCmd, "acqcmd -s %d set.sys /sys/module/acq200_mu/parameters/maxchain_clip 16", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.sys maxchain_clip ... failed\n", pSTDdev->taskName );
		return WR_ERROR;
	} while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);
#endif	
// TG Temove I don't nedd this function
	sprintf(strCmd, "acqcmd -s %d set.dtacq_drv hitide 4", pAcq196->slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.dtacq_drv hitide ... failed\n", pSTDdev->taskName);
		return WR_ERROR;
	} while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

	
	return WR_OK;
}


// TG Temove I don't nedd this function
#if 0
int acq196_set_maxdma(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
	sprintf(strCmd, "acqcmd -s %d set.sys /sys/module/acq200_mu/parameters/max_dma %d", pAcq196->slot, pAcq196->n32_maxdma);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		printf("\n>>> %s: set.sys max_dma %d ... failed\n", pSTDdev->taskName, pAcq196->n32_maxdma);
		return WR_ERROR;
	} while(fgets(buff, 128, pfp) != NULL ) { 
#if PRINT_ACQCMD
		printf("\n>>> %s: %s: %s", pSTDdev->taskName, strCmd, buff); 
#endif
	} pclose(pfp);

	return WR_OK;	
}

#endif

