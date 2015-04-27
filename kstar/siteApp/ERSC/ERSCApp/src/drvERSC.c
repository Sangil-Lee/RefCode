#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvERSC.h"
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h> /* open() */


#include "myMDSplus.h"


static long drvERSC_io_report(int level);
static long drvERSC_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvERSC = {
       2,
       drvERSC_io_report,
       drvERSC_init_driver
};

epicsExportAddress(drvet, drvERSC);



int create_ERSC_taskConfig( ST_STD_device *pSTDdev)
{
	ST_ERSC* pERSC;
	sfwTreeErr_e	eReturn;

	pERSC = (ST_ERSC *)malloc(sizeof(ST_ERSC));
	if( !pERSC) return WR_ERROR;
	pSTDdev->pUser = pERSC;

	pERSC->fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (pERSC->fd  < 0)
	{
		fprintf(stdout, "Device open error !!!!!!\n");
		return WR_ERROR;
	}
/*	pST_ERSC->pBaseAddr = (ST_REG_MAP *)malloc(MAP_SIZE ); */
	pERSC->pBaseAddr  = mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_BASEADDR & ~MAP_MASK);
//	pERSC->pBaseAddr  = mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_BASEADDR);
	if ( pERSC->pBaseAddr == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pBaseAddr]\n");
		return WR_ERROR;
	}

	pERSC->pTxC_BD= mmap(0, MAP_BD_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_MEM0_BASEADDR);
	if ( pERSC->pTxC_BD == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pTxC_BD]\n");
		return WR_ERROR;
	}
	pERSC->pTxC_DATA= mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_MEM0_DATAADDR & ~MAP_MASK);
	if ( pERSC->pTxC_DATA == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pTxC_DATA]\n");
		return WR_ERROR;
	}

	pERSC->pTxD_BD= mmap(0, MAP_BD_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_MEM2_BASEADDR);
	if ( pERSC->pTxD_BD == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pTxD_BD]\n");
		return WR_ERROR;
	}
	pERSC->pTxD_DATA= mmap(0, MAP_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_MEM2_BASEADDR + 0x10000);
	if ( pERSC->pTxD_DATA == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pTxD_DATA]\n");
		return WR_ERROR;
	}
/**********************************/
	pERSC->pRxA_BD= mmap(0, MAP_BD_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_MEM1_BASEADDR);
	if ( pERSC->pRxA_BD == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pRxA_BD]\n");
		return WR_ERROR;
	}
	pERSC->pRxB_BD= mmap(0, MAP_BD_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, C_MEM3_BASEADDR);
	if ( pERSC->pRxB_BD == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pRxB_BD]\n");
		return WR_ERROR;
	}
	
/**********************************/	
	pERSC->pRxA_DATA= mmap(0, MAP_ADC_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, ADDR_START_RX_A);
	if ( pERSC->pRxA_DATA == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pRxA_DATA]\n");
		return WR_ERROR;
	}
	pERSC->pRxB_DATA= mmap(0, MAP_ADC_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, pERSC->fd, ADDR_START_RX_B);
	if ( pERSC->pRxB_DATA == MAP_FAILED ) {
		fprintf(stdout, "MMAP error [pRxB_DATA]\n");
		return WR_ERROR;
	}
	

	
	printf("REG_REVISION:		0x%x\n", pERSC->pBaseAddr->gpRevision);
	printf("REG_BUILD_DATE:		0x%x\n", pERSC->pBaseAddr->gpBuildDate);
	printf("REG_SERIAL_NUMBER:	0x%x\n", pERSC->pBaseAddr->gpSerialNum);
	printf("REG_READ_WRITE_REG:	0x%x\n", pERSC->pBaseAddr->gpReadWrite);
	printf("REG_CONTROL:		0x%X\n", pERSC->pBaseAddr->gpBoardControl);

	pERSC->pBaseAddr->gpReadWrite = 0x1234abcd;
	printf("read/write -> 0x%x\n", pERSC->pBaseAddr->gpReadWrite);
	pERSC->pBaseAddr->gpReadWrite = 0x0;
	printf("read/write -> 0x%x\n", pERSC->pBaseAddr->gpReadWrite);


	printf("mapping pointer: \n");
	printf("\t pTxC_BD: %p \n", pERSC->pTxC_BD);
	printf("\t pTxC_DATA: %p \n", pERSC->pTxC_DATA);
	printf("\t pTxD_BD: %p \n", pERSC->pTxD_BD);
	printf("\t pTxD_DATA: %p \n", pERSC->pTxD_DATA);
	printf("\t pRxA_BD: %p \n", pERSC->pRxA_BD);
	printf("\t pRxB_BD: %p \n", pERSC->pRxB_BD);
	printf("\t pRxA_DATA: %p \n", pERSC->pRxA_DATA);
	printf("\t pRxB_DATA: %p \n", pERSC->pRxB_DATA);
	

	strcpy(pERSC->strTx_filePath, STR_FILE_PATH_JFFS);
	pERSC->ch_mask = BCR_ALL_TXRX_RUN;
	pERSC->recordNum = 8;
	pERSC->recordSize = INIT_SIZE_OF_RECORD;
	pERSC->run_period = 10;

 
 /************************************* for segmented archiving */
	 pERSC->pSfwTree = NULL;
 // 1. creates an object for Tree per each task (or device) : object & thread
	 if (NULL == (pERSC->pSfwTree = sfwTreeInit ("ERSCtreeHandler"))) {
		 fprintf(stdout, "sfwTreeInit() failed\n");
		 return WR_ERROR;
	 }
 
	 // 2. setup general configurations : at taskConfig() or arming
	 eReturn = sfwTreeSetLocalFileHome (pERSC->pSfwTree, NULL); /* home directory must be exist */
	 if (sfwTreeOk != eReturn) {
		 fprintf(stdout, "cannot set home directory for local data file\n");
		 return WR_ERROR;
	 }
	 
	 eReturn = sfwTreeSetPutType (pERSC->pSfwTree, sfwTreePutSegment);
 //  eReturn = sfwTreeSetSamplingRate (pNI5122->pSfwTree, TEST_SAMPLING_RATE); // move to "Arming".
 
 


	return WR_OK;
}


int init_my_sfwFunc_ERSC( ST_STD_device *pSTDdev)
{
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/
	pSTDdev->ST_Func._OP_MODE = func_ERSC_OP_MODE;
	pSTDdev->ST_Func._BOARD_SETUP = func_ERSC_BOARD_SETUP;
	pSTDdev->ST_Func._SYS_ARMING= func_ERSC_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_ERSC_SYS_RUN;
	pSTDdev->ST_Func._SAMPLING_RATE= func_ERSC_SAMPLING_RATE;
	pSTDdev->ST_Func._DATA_SEND = func_ERSC_DATA_SEND;

	pSTDdev->ST_Func._POST_SEQSTOP = func_ERSC_POST_SEQSTOP;

	pSTDdev->ST_Func._SYS_RESET = func_ERSC_SYS_RESET;
	pSTDdev->ST_Func._Exit_Call = clear_ERSC_taskConfig;


	/*********************************************************/

	pSTDdev->ST_DAQThread.threadFunc = (EPICSTHREADFUNC)threadFunc_ERSC_DAQ;
	if( make_STD_DAQ_thread(pSTDdev) == WR_ERROR ) {
		printf("ERROR! %s can't create \"Standard DAQ thread routine\" \n", pSTDdev->taskName );
		return WR_ERROR;
	}
		


	return WR_OK;

}


void clear_ERSC_taskConfig(void *pArg, double arg1, double arg2)
{
	sfwTreeErr_e	eReturn;
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
	
	printf("Exit Hook: Stopping Task %s ... \n", pSTDdev->taskName); 

	/* 5. releases tree object */
	eReturn = sfwTreeDelete (&pERSC->pSfwTree);

	munmap(pERSC->pBaseAddr, MAP_SIZE);
	
	munmap(pERSC->pTxC_BD, MAP_BD_SIZE);
	munmap(pERSC->pTxC_DATA, MAP_SIZE);
	munmap(pERSC->pTxD_BD, MAP_BD_SIZE);
	munmap(pERSC->pTxD_DATA, MAP_SIZE);
	close( pERSC->fd );

}

void threadFunc_ERSC_DAQ(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_ERSC *pERSC;
	ST_MASTER *pMaster = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return;

	if( !pSTDdev ) {
		printf("ERROR! threadFunc_user_DAQ() has null pointer of STD device.\n");
		return;
	}
	pERSC = (ST_ERSC *)pSTDdev->pUser;

	while(TRUE) {
		epicsEventWait( pSTDdev->ST_DAQThread.threadEventId);
//		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
	//	notify_refresh_master_status();
		epicsThreadSleep(pERSC->run_period);

#if 0
		printf(">>> %s: DAQthreadFunc() : Got epics Event and DAQ start\n", pSTDdev->taskName);
#endif

/*		if( pERSC->u8_ip_fault != 1 ) 
		{ */
			db_put("ERSC_SYS_RUN", "0");
			epicsThreadSleep(1.0);
			db_put("ERSC_SYS_ARMING", "0");
			epicsThreadSleep(1.0);
/*		} */

		if( pMaster->cUseAutoSave ) 
			db_put("ERSC_SEND_DATA", "1");
		
	}
}


static long drvERSC_init_driver(void)
{
	ST_MASTER *pMaster = NULL;
	ST_ERSC *pERSC = NULL;
	ST_STD_device *pSTDdev = NULL;
	
	pMaster = get_master();
	if(!pMaster)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) != 0 ) {
			pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
			continue;
		}

		if(create_ERSC_taskConfig( pSTDdev ) == WR_ERROR) {
			printf("ERROR!  \"%s\" create_ERSC_taskConfig() failed.\n", pSTDdev->taskName );
			return 1;
		}
		
		init_my_sfwFunc_ERSC(pSTDdev);

		drvERSC_clear_ADC_BD(pSTDdev, CH_RX_A);
		drvERSC_clear_ADC_BD(pSTDdev, CH_RX_B);
		
	
		pERSC = (ST_ERSC *)pSTDdev->pUser;



		pSTDdev->StatusDev |= TASK_STANDBY;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	} /* while(pERSC) { */

	notify_refresh_master_status();

	printf("****** ERSC local device init OK!\n");

	return 0;
}


static long drvERSC_io_report(int level)
{
	ST_STD_device *pSTDdev;
	ST_ERSC *pERSC;
	ST_MASTER *pMaster = get_master();

	if(!pMaster) return 0;

	if(ellCount(pMaster->pList_DeviceTask))
		pSTDdev = (ST_STD_device*) ellFirst (pMaster->pList_DeviceTask);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

  	epicsPrintf("Totoal %d task(s) found\n",ellCount(pMaster->pList_DeviceTask));

	if(level<1) return 0;

	while(pSTDdev) {
		pERSC = (ST_ERSC *)pSTDdev->pUser;
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, 
			    (unsigned int)pSTDdev, 
			    pSTDdev->StatusDev );
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", pSTDdev->ST_Base.nSamplingRate );
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
/*		
			epicsPrintf("   callback time: %fusec\n", pERSC->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pERSC->adjCount_smplRate,
					                                                 pERSC->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pERSC->adjCount_Gain,
					                                                   pERSC->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pERSC->pchannelConfig)>0) print_channelConfig(pERSC,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}



void func_ERSC_OP_MODE(void *pArg, double arg1, double arg2)
{
/*	ST_STD_device* pSTDdev = (ST_STD_device *)pArg; */

	if( (int)arg1 == OPMODE_REMOTE ) {

		
	}
	else if ((int)arg1 == OPMODE_LOCAL ) {

		
	}
}

void func_ERSC_BOARD_SETUP(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

/*	printf("call func_ERSC_BOARD_SETUP with %s, %f, %f \n", pSTDdev->taskName, arg1, arg2); */

	if( (int)arg1 ) {

		drvERSC_set_ADC_BD(pSTDdev, CH_RX_A);
		printf("ACD chA Buffer descriptor configuration finished!\n");
		drvERSC_set_ADC_BD(pSTDdev, CH_RX_B);
		printf("ACD chB Buffer descriptor configuration finished!\n");
	}


}

void func_ERSC_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	sfwTreeErr_e	eReturn;
	unsigned short opmode;
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
//	char strBuf[16];
	int numRec = pERSC->recordNum;
	opmode = pSTDdev->ST_Base.opMode;

	if( numRec > MAX_ADC_BD_CNT) numRec = MAX_ADC_BD_CNT;

	if ( (int)arg1   ) 
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;


		db_put("ERSC_RESET", "1");

		drvERSC_get_Multi_Trig_Info(pSTDdev);

		db_put("ERSC_RESET", "0");

		if( pERSC->pBaseAddr->gpBoardControl  &  BCR_RX_A_EXT_TRIG ) 
			pERSC->run_period = pSTDdev->ST_Base.dT1[numRec-1] + 10.0;
		else
			pERSC->run_period = 5.0;
		
/*		printf("%s: run period: %f\n", pSTDdev->taskName, pERSC->run_period ); */


//		eReturn = sfwTreeSetSamplingRate (pERSC->pSfwTree, (epicsUInt32)INIT_SAMPLE_CLOCK_B);
// 3-1. setup shot information for archiving : at arming
//	eReturn = sfwTreeSetShotInfo (pSfwTree, TEST_SHOT_NUM, TEST_TREE_NAME, TEST_MDS_ADDR, TEST_EVENT_NAME, TEST_EVENT_ADDR);
		eReturn = sfwTreeSetShotInfo (pERSC->pSfwTree,
								pSTDdev->ST_Base.shotNumber, 
								pSTDdev->ST_mds.treeName[opmode], 
								pSTDdev->ST_mds.treeIPport[opmode],
								pSTDdev->ST_mds.treeEventArg, 
								pSTDdev->ST_mds.treeEventIP[opmode]);
		if (sfwTreeOk != eReturn) {
			kuDebug (kuERR, "cannot set shot information\n");
		}
		sfwTreePrintInfo (pERSC->pSfwTree);

		

/*		flush_STDdev_to_MDSfetch( pSTDdev ); */
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;
	}
	else
	{
		if( check_dev_release_condition(pSTDdev)  == WR_ERROR) 
			return;

		

		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	}
}

void func_ERSC_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;

	volatile unsigned int t__0_ = (pERSC->pBaseAddr->gpBoardControl & 0xfffff0ff) | pERSC->ch_mask;

	if ( (int)arg1   ) 
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;
		
		pERSC->pBaseAddr->gpBoardControl  = t__0_;
		printf("Masked channel start.\n");

		epicsEventSignal( pSTDdev->ST_DAQThread.threadEventId);

		pSTDdev->StatusDev |= TASK_IN_PROGRESS;
	}
	else {
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;
		
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_ALL_TXRX_RUN;
		printf("ALL channel stop.\n");

		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
	}

	
}

void func_ERSC_SAMPLING_RATE(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
//	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
	uint32 prev = pSTDdev->ST_Base.nSamplingRate;

	pSTDdev->ST_Base.nSamplingRate = (uint32)arg1;

	printf("%s: sampling rate changed from %d to %d.\n", pSTDdev->taskName, prev, pSTDdev->ST_Base.nSamplingRate);
}
void func_ERSC_POST_SEQSTOP(void *pArg, double arg1, double arg2)
{
/*	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser; */

	if( (int)arg1 ) {
		db_put("ERSC_SYS_RUN", "0");
		epicsThreadSleep(1.0);
		db_put("ERSC_SYS_ARMING", "0");
		epicsThreadSleep(1.0);			
		db_put("ERSC_DATA_SEND", "1");
	}
}

void func_ERSC_DATA_SEND(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
	pSTDdev->StatusDev |= TASK_DATA_TRANSFER;
	notify_refresh_master_status();

	ersc_put_raw_value(pSTDdev);


	pSTDdev->StatusDev &= ~TASK_DATA_TRANSFER;
	pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
	notify_refresh_master_status();

}

void func_ERSC_SYS_RESET(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;

/*	call your device reset functions */

	if( (int)arg1 ) {
		pERSC->pBaseAddr->gpBoardControl  |= BCR_RESET_GLOBAL;
		printf("Board reset!!!\n");
	}
	else {
		pERSC->pBaseAddr->gpBoardControl  &= ~BCR_RESET_GLOBAL;
		printf("Reset clear!!!\n");
	}

}




int drvERSC_load_pattern(ST_STD_device *pSTDdev, int ch)
{
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
	
	char fullName[100];
	FILE *fp = NULL;
	int fileSize;
	int		*s32buffer = NULL;
	unsigned short got, i,j;

	sprintf(fullName, "%s/", pERSC->strTx_filePath);
	
	if(ch == CH_TX_C )
		strcat(fullName, pERSC->strTxC_fileName);
	
	else if(ch == CH_TX_D )
		strcat(fullName, pERSC->strTxD_fileName);
	
	else {
		printf("ERROR! %s : DAC channel error (%d)!\n", pSTDdev->taskName, ch);
		return WR_ERROR;
	}

	if( (fp = fopen(fullName, "rb") ) == NULL )
	{	
		printf("ERROR! %s : Can't open (%s)!\n", pSTDdev->taskName, fullName);
		return WR_ERROR;
	}

	fseek(fp, -2L, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	if( fileSize > 65536UL )	{
		printf("%s: size overflow!\n", pSTDdev->taskName);
		fclose(fp);
		return WR_ERROR;
	}
	printf("%s: \"%s\", size: %dByte (0x%X)\n", pSTDdev->taskName, fullName, fileSize, fileSize);

	s32buffer = (int *)malloc( fileSize );
	if( s32buffer == NULL ) {
		printf("malloc error for read pattern.\n");
		fclose(fp);
		return WR_ERROR;
	}
//	got = fread( s16buffer, 2, fileSize/2, fp);
	got = fread( s32buffer, fileSize, 1, fp);
	printf("read require: %d, return: %d\n", fileSize, got);
	fclose(fp);


	if(ch == CH_TX_C )
	{
		/* Buffer descriptor structure                */
		/*		B(31)........B(2),   B(1),        B(0) */
		/*		Address,              interrupt,  valid */
		/*		Data size                                    */
//		pERSC->pTxC_BD->xBD[0] = 0xcc810001;
		/* DAC use internal FPGA memory size of 64KB, so we write adress start with offset 0 " */
			
		for( i=0, j=0; i< MAX_DAC_BD_CNT; i++) {
			pERSC->pTxC_BD->xBD[j] = 0x00000001;
			pERSC->pTxC_BD->xBD[j+1] = fileSize;
			j += 2;
		}


//		memcpy(pERSC->pTxC_DATA, s32buffer, fileSize); 
#if 1
		for(i=0; i<(fileSize/4); i++) {
//			*(pERSC->pTxC_DATA+i) = 0x8001;
//			pERSC->pTxC_DATA->txDATA[i] = 0xffff;
                        *(pERSC->pTxC_DATA+i) = SWAP32( *(s32buffer+i) );
		}
#endif

	}
	else if(ch == CH_TX_D )
	{
/*		pERSC->pTxD_BD->xBD[0] = 0xcc850001; */
		for( i=0, j=0; i< MAX_DAC_BD_CNT; i++) {
			pERSC->pTxD_BD->xBD[j] = 0x00000001;
			pERSC->pTxD_BD->xBD[j+1] = fileSize;
			j += 2;
		}
			
		for(i=0; i<(fileSize/4); i++) 
			*(pERSC->pTxD_DATA+i) = SWAP32( *(s32buffer+i) );
	}
	else 
	{
		printf("ERROR! %s : DAC channel error (%d)!\n", pSTDdev->taskName, ch);
		return WR_ERROR;
	}

	free(s32buffer);

	return WR_OK;
	
}

void drvERSC_print_BufferDescriptor(ST_STD_device *pSTDdev)
{
	int i, j;
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
	printf("\n");

	for( i=0, j=0; i < MAX_DAC_BD_CNT; i++) {
		printf("DAC[%d] C : 0x%08X, %d (0x%x)\n",  i, pERSC->pTxC_BD->xBD[j], 
			pERSC->pTxC_BD->xBD[j+1], pERSC->pTxC_BD->xBD[j+1] );
		j += 2;
	}
	for( i=0, j=0; i < MAX_DAC_BD_CNT; i++) {
		printf("DAC[%d] D : 0x%08X, %d (0x%x)\n",  i, pERSC->pTxD_BD->xBD[j], 
			pERSC->pTxD_BD->xBD[j+1], pERSC->pTxD_BD->xBD[j+1] );
		j += 2;
	}

	for( i=0, j=0; i < 8; i++) {
		printf("ADC[%d] A : 0x%08X, %dMB (0x%x)\n",  i, pERSC->pRxA_BD->xBD[j], 
			pERSC->pRxA_BD->xBD[j+1]/1024/1024, pERSC->pRxA_BD->xBD[j+1] );
		j += 2;
	}
	for( i=0, j=0; i < 8; i++) {
		printf("ADC[%d] B : 0x%08X, %dMB (0x%x)\n",  i, pERSC->pRxB_BD->xBD[j], 
			pERSC->pRxB_BD->xBD[j+1]/1024/1024, pERSC->pRxB_BD->xBD[j+1] );
		j += 2;
	}


}

int drvERSC_set_ADC_BD(ST_STD_device *pSTDdev, int ch)
{
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
	int i, j;

	/* Buffer descriptor structure                */
	/*		B(31)........B(2),   B(1),        B(0) */
	/*		Address,              interrupt,  valid */
	/*		Data size                                    */
	
	if(ch == CH_RX_A )
	{
		pERSC->pRxA_BD->xBD[0] = ADDR_START_RX_A + 0x1;
		pERSC->pRxA_BD->xBD[1] = pERSC->recordSize;
		for( i=1, j=2; i< MAX_ADC_BD_CNT; i++) {
//			pERSC->pRxA_BD->xBD[j] = pERSC->pRxA_BD->xBD[j-2] + pERSC->recordSize + 0x1;

			if( i < pERSC->recordNum )
				pERSC->pRxA_BD->xBD[j] = ADDR_START_RX_A + ( pERSC->recordSize * i )  + 0x1;
			else /* must do this, because valid bit clear when record number decreased! */
				pERSC->pRxA_BD->xBD[j] = ADDR_START_RX_A + ( pERSC->recordSize * i );
//			pERSC->pRxA_BD->xBD[j] = 0x0;
			pERSC->pRxA_BD->xBD[j+1] = pERSC->recordSize;
			j += 2;
		}
		
	}
	else {
		pERSC->pRxB_BD->xBD[0] = ADDR_START_RX_B + 0x1;
		pERSC->pRxB_BD->xBD[1] = pERSC->recordSize;
		for( i=1, j=2; i< pERSC->recordNum; i++) {
			if( i < pERSC->recordNum )
				pERSC->pRxB_BD->xBD[j] = ADDR_START_RX_B + ( pERSC->recordSize * i )  + 0x1;
			else 
				pERSC->pRxB_BD->xBD[j] = ADDR_START_RX_B + ( pERSC->recordSize * i );
//			pERSC->pRxB_BD->xBD[j] = 0x0;
			pERSC->pRxB_BD->xBD[j+1] = pERSC->recordSize;
			j += 2;
		}

	}


	return WR_OK;
}

int drvERSC_clear_ADC_BD(ST_STD_device *pSTDdev, int ch)
{
	ST_ERSC *pERSC = (ST_ERSC *)pSTDdev->pUser;
	int i, j;
	
	if(ch == CH_RX_A )
	{
		pERSC->pRxA_BD->xBD[0] = 0x0;
		pERSC->pRxA_BD->xBD[1] = pERSC->recordSize;
		for( i=1, j=2; i< pERSC->recordNum; i++) {
			pERSC->pRxA_BD->xBD[j] = 0x0;
			pERSC->pRxA_BD->xBD[j+1] = pERSC->recordSize;
			j += 2;
		}
		
	}
	else {
		pERSC->pRxB_BD->xBD[0] = 0x0;
		pERSC->pRxB_BD->xBD[1] = pERSC->recordSize;
		for( i=1, j=2; i< pERSC->recordNum; i++) {
			pERSC->pRxB_BD->xBD[j] = 0x0;
			pERSC->pRxB_BD->xBD[j+1] = pERSC->recordSize;
			j += 2;
		}
	}

	printf("ADC buffer descriptor clear! (%d)\n", ch);

	return WR_OK;
}


int drvERSC_get_Multi_Trig_Info(ST_STD_device *pSTDdev)
{
	int nval;
	char strBuf[16];

	char strT0[64], strT1[64];

	for( nval=0; nval < SIZE_CNT_MULTI_TRIG; nval++) 
	{
//		sprintf(strT0, "ERSC_ER_LTU_%s_SEC%d_T0", pSTDdev->strArg0, nval);
		sprintf(strT0, "ERSC_%s_LTU_%s_SEC%d_T0", pSTDdev->strArg0, pSTDdev->strArg1, nval);
		if( db_get(strT0, strBuf) == WR_ERROR ) {
			notify_error(1, "'%s' get error!\n", strT0);
			return WR_ERROR;
		} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT0[nval] );

		sprintf(strT1, "ERSC_%s_LTU_%s_SEC%d_T1", pSTDdev->strArg0, pSTDdev->strArg1, nval);
		if( db_get(strT1, strBuf) == WR_ERROR ) {
			notify_error(1, "'%s' get error!\n", strT1);
			return WR_ERROR;
		} sscanf(strBuf, "%lf", &pSTDdev->ST_Base.dT1[nval] );
	}
/*	
	for( nval=0; nval<SIZE_CNT_MULTI_TRIG; nval++) {
		printf("Section%d :  %lf -> %lf\n", nval, pSTDdev->ST_Base.dT0[nval], pSTDdev->ST_Base.dT1[nval] );
	}
*/
	return WR_OK;

}


