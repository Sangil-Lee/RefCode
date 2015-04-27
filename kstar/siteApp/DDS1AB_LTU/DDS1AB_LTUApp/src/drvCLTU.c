/******************************************************************************
 *  Copyright (c) 2007 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/


/*==============================================================================
                        EDIT HISTORY FOR MODULE


2007. 09. 17. 
int cltu_cmd_endian( void )
if( endian_signal == ENDIAN_SIGNAL ) true than return;


2007. 09. 19
delete mknod() in initialize function.


2007. 09. 20 
+ enable drvCLTU_set_shotTerm() for LTU mode.
+ enable drvCLTU_set_shotStart() for LTU


2007. 10. 16
int make_taskConfig(char *taskName)
+ ioc_debug 


2007. 10. 23
STATUS cltu_set_multiClock();
int cltu_cmd_set_ct_ms( );
+ argument :  int trg0, int trg1, int trg2, int trg3, int trg4, 


2007. 10. 25
change working flow for port config....


2007.10.31
STATUS drvCLTU_set_clockOnShot (int nArg0, int onoff)


2007. 11. 06.
clearAllOzTasks() function is only for Linux system.


2007. 11. 15.
+ STATUS drvCLTU_put_calibratedVal(int calibVal);


2007. 11. 20.
changed STATUS cltu_set_multiClock();

int check_ctu( void ) {
+ System_Info* mSys;
+ mSys = (System_Info*) (pCfg->pciDevice.base2);
+ sw_id = mSys->ltu.id;
- sw_id = cltu_cmd_read32( 0x71200000 + ( OFFSET_32BIT_REG * 0));
}

2007. 11. 23.
add interrupt service.........


2007. 11. 29.
add VxWorks interrupt service......
modify some
- #define LOG_EN (1)


2007. 11. 30
modify intLock();

2010. 3. 26
design changed for multi board

2010.4.4
check VxWorks part..

*/

#include "cltu_Def.h"
#include "drvCLTU.h"

#include "oz_cltu.h"
#include "oz_xtu2.h"



#define USE_INTLOCK   0















static void createTaskCLTU_CallFunction(const iocshArgBuf *args);

static const iocshArg createTaskCLTUArg0 = {"task_name", iocshArgString};
static const iocshArg createTaskCLTUArg1 = {"dev", iocshArgString};
static const iocshArg createTaskCLTUArg2 = {"ver", iocshArgString};
static const iocshArg createTaskCLTUArg3 = {"type", iocshArgString};
static const iocshArg createTaskCLTUArg4 = {"id", iocshArgString};



static const iocshArg* const createTaskCLTUArgs[] = { &createTaskCLTUArg0 , &createTaskCLTUArg1 , &createTaskCLTUArg2, &createTaskCLTUArg3, &createTaskCLTUArg4};
static const iocshFuncDef createTaskCltuFuncDef = { "createTaskCLTU", 5, createTaskCLTUArgs };

/* static void epicsShareAPI drvCLTURegistrar(void) */
static void drvCLTURegister(void)
{
	iocshRegister(&createTaskCltuFuncDef, createTaskCLTU_CallFunction);
}
epicsExportRegistrar(drvCLTURegister);



static drvCLTU_Config          *pdrvCLTU_Config = NULL;

/* test function */
ULONG drvCLTU_getCurrentTick(void)
{
	ULONG x;

/*	__asm__ volatile(".byte 0x0f, 0x31": "=A"(x));	*/
#ifdef _CLTU_VXWORKS_
	x = tickGet ();
#else
	struct timeval tv;
	gettimeofday( &tv, NULL);
	x = tv.tv_sec;
	x = (x*1.E+6) + tv.tv_usec; 
#endif
	
/*	return timecurrent.tv_sec*1000 + timecurrent.tv_usec/	
	return x;
*/

	return x;
}
/*
double drvCLTU_intervalmSec(unsigned long long int start, unsigned long long int stop)
{
	return 1.E+6 * (double)(stop-s:tart)/(double)(pdrvCLTUConfig->one_sec_interval);
}
*/
double drvCLTU_intervalmSec(ULONG start, ULONG stop)
{
	return (stop-start);
}

static int make_drvConfig();
/* static ST_drvCLTU* init_taskConfig(char * taskName, char *devf);  */
static int init_taskConfig(ST_drvCLTU *pCfg);

static drvCLTU_controlThreadConfig* make_controlThreadConfig(ST_drvCLTU *pCfg);
#if 0
static drvCLTU_controlThreadConfig* make_interruptThreadConfig(ST_drvCLTU *pCfg);
#endif

static void controlThreadFunc(void *param);
/*static void interruptThreadFunc(void *param);*/

#ifdef _CLTU_LINUX_
static void clearAllOzTasks(void);
#endif

STATUS cltu_set_multiClock(ST_drvCLTU *pCfg);


LOCAL long drvCLTU_io_report(int level);
LOCAL long drvCLTU_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvCLTU = {
       2,
       drvCLTU_io_report,
       drvCLTU_init_driver
};

epicsExportAddress(drvet, drvCLTU);





#ifdef _CLTU_VXWORKS_
STATUS cltu_isr( )
{
	STATUS Status;

	 ST_drvCLTU *pCfg = (ST_drvCLTU*) ellFirst(pdrvCLTU_Config->ptaskList);

#if USE_INTLOCK
	pCfg->pciDevice.intLockKey = intLock ();
#else
	intDisable((int)pCfg->pciDevice.int_line);
#endif
	
	pCfg->pciDevice.int_handler++;

	Status = semGive(pCfg->pciDevice.int_sem);
	if( Status == ERROR )
	{    
		pCfg->pciDevice.int_error++;
	}
/*	intUnlock ( pCfg->pciDevice.intLockKey ); */
/*	intEnable((int)pCfg->pciDevice.int_line); */
/*
	if( pCfg->taskStatus == TASK_FINISH ) intDisable((int)pCfg->pciDevice.int_line); 
*/
	return WR_OK;
}
#endif


#ifdef _CLTU_VXWORKS_
STATUS cltu_epics_device_init( ST_drvCLTU *pCfg )
{
  STATUS vstatus;
/*  unsigned int val; */
/*  int val; */

  /* pci device struct init */
  memset( &pCfg->pciDevice, 0x00, sizeof(CLTU_PCI_DEVICE));

	/* pci find device */
	if( pCfg->u8Version == VERSION_1 )
		vstatus = pciFindDevice( CLTU_VENDOR_ID, CLTU_DEVICE_ID, 0, &pCfg->pciDevice.BusNo, &pCfg->pciDevice.DeviceNo, &pCfg->pciDevice.FuncNo );
	else
		vstatus = pciFindDevice( CLTU_VENDOR_ID_2, CLTU_DEVICE_ID_2, 0, &pCfg->pciDevice.BusNo, &pCfg->pciDevice.DeviceNo, &pCfg->pciDevice.FuncNo );
	if( vstatus == ERROR )
	{
		printf("[CLTU] Device Not Found.........ERROR!\n");
		return WR_ERROR;
	}
	else
	{
		printf("[CLUT] Device pciFindDevice.........OK!\n");
		printf("[CLTU] BusNo 0x%x, DevNo 0x%x, FuncNo 0x%x\n", pCfg->pciDevice.BusNo, pCfg->pciDevice.DeviceNo, pCfg->pciDevice.FuncNo);
	}

#if USE_MANUAL_FIND_PCI
	pCfg->pciDevice.DeviceNo = strtoul(pCfg->deviceFile, NULL, 0);
	printf("[CLTU] from  arguments: BusNo 0x%x, DeviceNo 0x%x, FuncNo 0x%x\n", pCfg->pciDevice.BusNo, pCfg->pciDevice.DeviceNo, pCfg->pciDevice.FuncNo);
#endif

	/* get the pci bar0 adderess */
	vstatus = pciConfigInLong( pCfg->pciDevice.BusNo, pCfg->pciDevice.DeviceNo, pCfg->pciDevice.FuncNo, PCI_CFG_BASE_ADDRESS_0, &pCfg->pciDevice.base0);
	if( vstatus == ERROR )
	{
		printf("[CLTU] Get bar0 address.........ERROR!\n");
		return WR_ERROR;
	}
	else
	{
		epicsPrintf("[CLTU] %s, Get bar0 address.........OK! (0x%x)\n", pCfg->taskName, pCfg->pciDevice.base0);
	}

	/* get the pci bar2 adderess */
	vstatus = pciConfigInLong( pCfg->pciDevice.BusNo, pCfg->pciDevice.DeviceNo, pCfg->pciDevice.FuncNo, PCI_CFG_BASE_ADDRESS_2, &pCfg->pciDevice.base2);
	if( vstatus == ERROR )
	{
		printf("[CLUT] Get bar2 address.........ERROR!\n");
		return WR_ERROR;
	}
	else
	{
/*		val = READ32(pCfg->pciDevice.base0 + 0x0); */
/*		printf(">> XTU first READ: base0:  0x%x\n", val); */
		
		epicsPrintf("[CLTU] %s, Get bar2 address.........OK! (0x%x)\n", pCfg->taskName, pCfg->pciDevice.base2);
/*		printf("[CLTU] bar2 address = 0x%x\n", pCfg->pciDevice.base2); */

/*		val = READ32(pCfg->pciDevice.base0 + 0x0); */
/*		printf(">> XTU second READ: base0:  0x%x\n", val); */
	}

	if(pCfg->pciDevice.base0 == 0x00000000) return WR_ERROR;

	printf("XTU pciDevice.base0: 0x%x\n", pCfg->pciDevice.base0);


	/* pCfg->pciDevice init ok */
	pCfg->pciDevice.init = 0xFFFFFFFF;

	
	/* endian control */
	if( pCfg->u8Version == VERSION_1) {
		cltu_cmd_endian(pCfg);
	} else
	{
		xtu_SetEndian(pCfg);
		xtu_SetMode(pCfg, pCfg->u32Type, pCfg->xtu_ID);
		xtu_reset(pCfg);
		printf(">> EPICS driver init .... OK!!\n");
	}
	

	return WR_OK;
}
#endif

#ifdef _CLTU_LINUX_
STATUS cltu_epics_device_init( ST_drvCLTU *pCfg )
{
	unsigned int val;

	memset((void*) &pCfg->pciDevice,(int) 0x00, sizeof(CLTU_PCI_DEVICE));

	/* Device File Create */
/*	result = mknod(DEVICE_FILENAME, S_IFCHR|S_IRWXU|S_IRWXG, (240<<8)|1);
	if( result < 0 )
	{
		perror("mknod");
	}
*/
	/* Device File Open */
	pCfg->pciDevice.fd = open(pCfg->deviceFile, O_RDWR|O_NDELAY );
	if( pCfg->pciDevice.fd < 0)
	{
		perror("open");
		return WR_ERROR;
	}
	
	/* Memory mapping */
	if( pCfg->u8Version == VERSION_1 ) {
		pCfg->pciDevice.base2 = (char *) mmap(0, MMAP_SIZE_R1, PROT_READ|PROT_WRITE, MAP_SHARED, pCfg->pciDevice.fd, 0);

		if(pCfg->pciDevice.base2 == 0x0) return WR_ERROR;
	}
	else if( pCfg->u8Version == VERSION_2 ) {
		pCfg->pciDevice.base0 = (char *) mmap(0, MMAP_SIZE_R2, PROT_READ|PROT_WRITE, MAP_SHARED, pCfg->pciDevice.fd, 0);

		if(pCfg->pciDevice.base0 == 0x0){
			printf("CLTU_ERROR:mmap ... error!\n");
			return WR_ERROR;
			}
	}
	else {
		printf("CLTU_ERROR:cltu_epics_device_init: version ... error!\n");
		return WR_ERROR;
		}

	/* pCfg->pciDevice init ok */
	pCfg->pciDevice.init = 0xFFFFFFFF;


	/* endian control */
	if( pCfg->u8Version == VERSION_1) {
		val = READ32(pCfg->pciDevice.base2 + 0x0);
		printf(">> CLTU first READ: base2:  0x%x\n", val);
		
		cltu_cmd_endian(pCfg);
	}
	else
	{
		val = READ32(pCfg->pciDevice.base0 + 0x0);
/*		printf(">> XTU first READ: base0:  0x%x\n", val);

		val = READ32(pCfg->pciDevice.base0 + 0x7c);
		printf(">> XTU first READ: base0 7c:  0x%x\n", val);
*/		
		if( is_mmap_ok(pCfg) == WR_OK) {
			xtu_SetEndian(pCfg);
			xtu_SetMode(pCfg, pCfg->u32Type, pCfg->xtu_ID);
			xtu_reset(pCfg);
		} else {
#ifdef _CLTU_LINUX_
			int status;
			val = 0x01234567;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SET_ENDIAN, &val);
			if (status == -1) {
				printf("CLTU_ERROR:IOCTL_CLTU_R2_SET_ENDIAN... error!\n");
				return WR_ERROR;
				}

			val = (unsigned int)pCfg->xtu_ID;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_NODE_ID, &val);
			if (status == -1) {
				printf("CLTU_ERROR:IOCTL_CLTU_R2_NODE_ID... error!\n");
				return WR_ERROR;
				}

			val = (unsigned int)pCfg->u32Type;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SET_MODE, &val);
			if (status == -1){
				printf("CLTU_ERROR:IOCTL_CLTU_R2_SET_MODE... error!\n");
				return WR_ERROR;
				}
#endif

		}
/*
		val = READ32(pCfg->pciDevice.base0 + 0x0);
		printf(">> XTU second READ: base0:  0x%x\n", val);
*/
	}
	printf(">> EPICS driver init .... OK!!\n");

	return WR_OK;
}
#endif



#ifdef _CLTU_LINUX_
STATUS cltu_device_quit( ST_drvCLTU *pCfg )
{

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		CLTU_ASSERT(); return WR_ERROR;
	}

	memset( &pCfg->pciDevice, 0x0, sizeof(CLTU_PCI_DEVICE));
  
	/* Memory unmapping */
	if( pCfg->u8Version == VERSION_1 ) 
		munmap( pCfg->pciDevice.base2, MMAP_SIZE_R1 );
	else if( pCfg->u8Version == VERSION_2 ) 
		munmap( pCfg->pciDevice.base0, MMAP_SIZE_R2 );

	/* Close Device File */
	close( pCfg->pciDevice.fd );

	return WR_OK;
}
#endif




/*static ST_drvCLTU* init_taskConfig(char * taskName, char* devf)*/
static int init_taskConfig(ST_drvCLTU *pCfg)
{
	static int numbering = 0;
	int i;
	
	pCfg->taskLock = epicsMutexCreate();
	pCfg->bufferLock = epicsMutexCreate();
	scanIoInit(&pCfg->ioScanPvt);

	pCfg->pcontrolThreadConfig = NULL;
	pCfg->pinterruptThreadConfig = NULL;

	pCfg->nShotTerm_msb = 0x0;
	pCfg->nShotTerm_lsb = 0x11E1A300;
	

	for(i=0; i< 4; i++) {
		pCfg->sPortConfig[i].nTrigMode = 0xffffffff;
		pCfg->sPortConfig[i].nClock = 100000;
		
		pCfg->sPortConfig[i].nT0_msb = 0x0;
		pCfg->sPortConfig[i].nT0_lsb = 0x5f5e100;
		
		pCfg->sPortConfig[i].nT1_msb = 0x0;
		pCfg->sPortConfig[i].nT1_lsb = 0xbebc200;
		pCfg->curPortConfigStatus[i] = PORT_NOT_INIT;
	}

	for(i=0; i<3; i++) {
		pCfg->mPortConfig[i].nTrigMode = 0xffffffff;
		pCfg->mPortConfig[i].nClock = 100000;
		
		pCfg->mPortConfig[i].nT0_msb = 0x0;
		pCfg->mPortConfig[i].nT0_lsb = 0x5f5e100;
		
		pCfg->mPortConfig[i].nT1_msb = 0x0;
		pCfg->mPortConfig[i].nT1_lsb = 0xbebc200;
	}
	pCfg->curPortConfigStatus[4] = PORT_NOT_INIT; /* this is last multi-trig port */

	if( getenv("CLTU_IOC_DEBUG") ){
		ioc_debug = atoi(getenv("CLTU_IOC_DEBUG") );
		printf("CLTU_IOC_DEBUG : %d\n", ioc_debug);
	} else ioc_debug = 0;


	if( getenv("TOP") ){
		sprintf(pCfg->strTOP, "%s", getenv("TOP")  );
		printf("%s: TOP is \"%s\"\n",pCfg->taskName, pCfg->strTOP);
	} else {
		printf("CLTU_ERROR: %s: There is no \"TOP\" environment value\n",pCfg->taskName);
		return WR_ERROR;
	}

	if( getenv("IOC") ){
		sprintf(pCfg->strIOC, "%s", getenv("IOC") );
		printf("%s: IOC is \"%s\"\n",pCfg->taskName, pCfg->strIOC);
	} else {
		printf("CLTU_ERROR: %s: There is no \"IOC\" environment value\n",pCfg->taskName);
		return WR_ERROR;
	}
	

	
#if defined(_CLTU_VXWORKS_)
	pCfg->pciDevice.intLockKey = 0;
	pCfg->pciDevice.int_count = 0;
	pCfg->pciDevice.int_handler = 0;
	pCfg->pciDevice.int_error = 0;

	/* it just for DDS1, two PCM card. */
	pCfg->pciDevice.BusNo = PCI_BUS_NO;
	pCfg->pciDevice.DeviceNo = PCI_DEVICE_NO;
	pCfg->pciDevice.FuncNo = PCI_FUNC_NO;		
#endif

	pCfg->calib_target_ltu = 0;

	pCfg->nCCS_shotNumber = 0;

	pCfg->taskStatus = 0x0;

	pCfg->enable_IRIGB = 1;
	pCfg->enable_LTU_freeRun = 0;
	pCfg->enable_CTU_comp = 0;
	pCfg->u32Calib_val = 1;

	pCfg->dev_num = numbering;

	numbering++;

	return WR_OK;

}

static drvCLTU_controlThreadConfig*  make_controlThreadConfig(ST_drvCLTU *pCfg)
{
	drvCLTU_controlThreadConfig *pcontrolThreadConfig  = NULL;

	pcontrolThreadConfig  = (drvCLTU_controlThreadConfig*)malloc(sizeof(drvCLTU_controlThreadConfig));
	if(!pcontrolThreadConfig) return pcontrolThreadConfig; 

	sprintf(pcontrolThreadConfig->threadName, "%s_CLTU", pCfg->taskName);
	
	pcontrolThreadConfig->threadPriority = epicsThreadPriorityHigh;
	pcontrolThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pcontrolThreadConfig->threadFunc      = (EPICSTHREADFUNC) controlThreadFunc;
	pcontrolThreadConfig->threadParam     = (void*) pCfg;

	pcontrolThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));

	epicsThreadCreate(pcontrolThreadConfig->threadName,
			  pcontrolThreadConfig->threadPriority,
			  pcontrolThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pcontrolThreadConfig->threadFunc,
			  (void*) pcontrolThreadConfig->threadParam);


	return pcontrolThreadConfig;
}
#if 0
static drvCLTU_controlThreadConfig*  make_interruptThreadConfig(ST_drvCLTU *pCfg)
{
	int vstatus;
	
	drvCLTU_controlThreadConfig *pinterruptThreadConfig  = NULL;

	pinterruptThreadConfig  = (drvCLTU_controlThreadConfig*)malloc(sizeof(drvCLTU_controlThreadConfig));
	if(!pinterruptThreadConfig) return pinterruptThreadConfig; 

	sprintf(pinterruptThreadConfig->threadName, "%s_interrupt", pCfg->taskName);
	
	pinterruptThreadConfig->threadPriority = epicsThreadPriorityHigh;
	pinterruptThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pinterruptThreadConfig->threadFunc      = (EPICSTHREADFUNC) interruptThreadFunc;
	pinterruptThreadConfig->threadParam     = (void*) pCfg;

	pinterruptThreadConfig->threadQueueId   = 0; /* we don't need this */

	vstatus = WR_OK;
	
#if defined(_CLTU_LINUX_)
/* this is for linux kernel  */
	epicsThreadCreate(pinterruptThreadConfig->threadName,
			  pinterruptThreadConfig->threadPriority,
			  pinterruptThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pinterruptThreadConfig->threadFunc,
			  (void*) pinterruptThreadConfig->threadParam);

#elif defined(_CLTU_VXWORKS_)
	 /* get the pci int line */
	vstatus = pciConfigInByte( pCfg->pciDevice.BusNo, pCfg->pciDevice.DeviceNo, pCfg->pciDevice.FuncNo, PCI_CFG_DEV_INT_LINE, &pCfg->pciDevice.int_line);
	if( vstatus == ERROR )
	{
		printf("[CLTU] Get pci int line .........ERROR!\n");
		return NULL;
	}
	else
	{
		printf("[CLUT] Get pci int line .........OK!\n");
		printf("[CLTU] int line = 0x%x\n", pCfg->pciDevice.int_line);
	}

	pCfg->pciDevice.int_sem = semBCreate( SEM_Q_FIFO, SEM_EMPTY );
	if( pCfg->pciDevice.int_sem == NULL )
	{
		printf("[CLTU] semBCreate ERROR!\n");
	}


	pCfg->tPCI = taskSpawn(CLTU_PCI_TASK_NAME, CLTU_PCI_TASK_PRI, 8, 100000, (FUNCPTR)interruptThreadFunc,
	      (int) pinterruptThreadConfig->threadParam, 0, 0, 0, 0, 0, 0, 0, 0, 0);  

#if USE_INTLOCK
	intLockLevelSet(1);
	vstatus = intLockLevelGet();
	printf("\n>>> make_interruptThreadConfig : intLockLevelGet() : %d\n", vstatus );
#endif
/* don't use interrupt service routine.    2011.4.11 */
/*
	vstatus = intConnect((VOIDFUNCPTR *)(pCfg->pciDevice.int_line), (VOIDFUNCPTR)cltu_isr, (int)NULL);
	if( vstatus == ERROR ) {
		printf("[CLTU] intConnect .........ERROR!\n");
		return NULL;
	} else {
		printf("[CLTU] intConnect.........OK!\n");
	}
*/

/*	vstatus = READ32(pCfg->pciDevice.base0+0x68);
	if( ioc_debug==1) printf("[CLTU] default INTCSR value = 0x%x\n", vstatus );

	vstatus = READ32(pCfg->pciDevice.base0+0x68);
	if( ioc_debug==1) printf("[CLTU] default INTCSR value = 0x%x\n", vstatus ); 
*/
#endif

	return pinterruptThreadConfig;
}
#endif

static void controlThreadFunc(void *param)
{

	ST_drvCLTU *pCfg = (ST_drvCLTU*) param;
	drvCLTU_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData		queueData;

	while( !pCfg->pcontrolThreadConfig ) epicsThreadSleep(.1);
	pcontrolThreadConfig = (drvCLTU_controlThreadConfig*) pCfg->pcontrolThreadConfig;


/*	if( ioc_debug==1) */
	printf("task launching: %s thread for %s task\n",pcontrolThreadConfig->threadName, pCfg->taskName);

	while(TRUE) {
		EXECFUNCQUEUE            pFunc;
		execParam                *pexecParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		ST_drvCLTU    *pCfg;

		epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId,
				         (void*) &queueData,
					 sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;	
		precord    = (struct dbCommon*) pexecParam->precord;
		prset      = (struct rset*) precord->rset;
		pCfg = (ST_drvCLTU *) pexecParam->pCfg;

		if(!pFunc) continue;
		else pFunc(pexecParam);

		if(!precord) continue;


		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}


	return;
}

#if 0
static void interruptThreadFunc(void *param)
{
	int rval;
	char buff[8];
	ST_drvCLTU *pCfg = (ST_drvCLTU*) param;

	strcpy(buff,"");
	rval=0;

	while( !pCfg ) epicsThreadSleep(.1);

	printf("task launching: Interrupt thread for %s task\n", pCfg->taskName);

#ifdef _CLTU_LINUX_
/*	state = ioctl( pCfg->pciDevice.fd, IOCTL_CLTU_INTERRUPT_ENABLE, &rval); */
#endif

	while(TRUE)
	{
#ifdef _CLTU_LINUX_
/*		read( pCfg->pciDevice.fd, buff, 1); */

		epicsThreadSleep(1.);
		
		scanIoRequest(pCfg->ioScanPvt);
#elif defined(_CLTU_VXWORKS_)
/*
		state = semTake( pCfg->pciDevice.int_sem, WAIT_FOREVER );
		if( state == ERROR )
		{
			printf("[PCI] semTake ERROR!\n");      
			return;
		}

		pCfg->pciDevice.int_count++;
		if( ioc_debug==3) {
			printf("[PCI] int_count=%d, int_handler=%d\n",pCfg->pciDevice.int_count, pCfg->pciDevice.int_handler);
			printf("[PCI] int_error = %d!\n",pCfg->pciDevice.int_error);
		}

#if USE_INTLOCK
		intUnlock ( pCfg->pciDevice.intLockKey );
#else
		intEnable((int)pCfg->pciDevice.int_line);
#endif
*/
		epicsThreadSleep(1.);

		scanIoRequest(pCfg->ioScanPvt);
#endif

		if( pCfg->taskStatus == TASK_FINISH ) break;

	}

	return;
}
#endif

static int make_drvConfig()
{
	if(pdrvCLTU_Config) return 0;

	pdrvCLTU_Config = (drvCLTU_Config*) malloc(sizeof(drvCLTU_Config));
	if(!pdrvCLTU_Config) return 1;

	pdrvCLTU_Config->ptaskList = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pdrvCLTU_Config->ptaskList) return 1;
	else ellInit(pdrvCLTU_Config->ptaskList);

	return 0;
}
static void createTaskCLTU_CallFunction(const iocshArgBuf *args)
{
	ST_drvCLTU *pCfg = NULL;
	char task_name[60];
	char devf[60];
	int Ver=0;
	int type=0;
	unsigned char ID=0;


#ifdef _CLTU_VXWORKS_ 
	sysClkRateSet(1000);
#endif


	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(devf, args[1].sval);
	if(args[2].sval) Ver = strtoul(args[2].sval,NULL,0);
	if(args[3].sval) type = strtoul(args[3].sval,NULL,0);
	if(args[4].sval) ID = strtoul(args[4].sval,NULL,0);

	printf("CreateTaskCLTU arguments: %s, %s, %d, %d, %d\n", task_name, devf, Ver, type, ID);

	if(make_drvConfig()) return;

/*	printf("1\n");
	epicsThreadSleep(1.0);  */

	pCfg = (ST_drvCLTU*) malloc(sizeof(ST_drvCLTU));
	if(!pCfg) return;

	pCfg->u8Version = (unsigned char)Ver;
	strcpy(pCfg->taskName, task_name);
	strcpy(pCfg->deviceFile, devf );
	pCfg->u32Type = (type == 0) ? XTU_MODE_LTU : XTU_MODE_CTU;
	pCfg->xtu_ID = ID;
		
	if(init_taskConfig(pCfg) == WR_ERROR) return;

/*	printf("CLTU task initialize ok (%p)\n", pCfg); */

	pCfg->pdrvConfig = pdrvCLTU_Config;

	pCfg->pcontrolThreadConfig = make_controlThreadConfig(pCfg);
	if(!pCfg->pcontrolThreadConfig) return;

/*	printf("make control thread ok...pcontrolThreadConfig: %p\n", pCfg->pcontrolThreadConfig); */
	
#ifdef _CLTU_VXWORKS_ 
	if( cltu_epics_device_init(pCfg) == WR_ERROR ) {
		printf("\n>>> drvCLTU_init_driver : cltu_epics_device_init() ... error!\n" );
		free(pCfg);
		return ;
	}
#endif

	/* must do after cltu_driver initialized */
/* 2011. 4. 27.   don't use interrupt service routine */
#if 0
	pCfg->pinterruptThreadConfig = make_interruptThreadConfig(pCfg);
	if(!pCfg->pinterruptThreadConfig) return ;
#endif

	ellAdd(pdrvCLTU_Config->ptaskList, &pCfg->node);

	return;
}



LOCAL long drvCLTU_init_driver(void)
{
	unsigned long long int a;

	ST_drvCLTU *pCfg = NULL;

	if(!pdrvCLTU_Config)  return 0;

	pCfg = (ST_drvCLTU*) ellFirst(pdrvCLTU_Config->ptaskList);
	while(pCfg) 
	{
#ifdef _CLTU_LINUX_	
		if( cltu_epics_device_init(pCfg) == WR_ERROR ) {
			printf("\n>>> drvCLTU_init_driver : cltu_epics_device_init() ... error!\n" );
			return -1;
		}
#endif

#if PRINT_PMC_INFO
		drvCLTU_show_info(pCfg);
#endif
		if( pCfg->u8Version == VERSION_2 && (pCfg->u32Type == XTU_MODE_LTU))
		{
			FILE *fp=NULL;
			char fname[128];
			char buf[16];

			sprintf(fname, "%s/iocBoot/%s/%s%d", pCfg->strTOP, pCfg->strIOC, FILE_CALIB_VALUE, pCfg->xtu_ID);

			fp = fopen(fname, "r");
			if(!fp) {
				printf("CLTU_ERROR: can't open file \"%s\". \n", fname);
			} 
			else 
			{
				if( fgets(buf, 16, fp) == NULL ) {
					printf("CLTU_ERROR: can't read string from \"%s\". \n", fname);
					fclose(fp); 
				}
				else {
					sscanf(buf, "%d",  &pCfg->u32Calib_val);		
					fclose(fp);
				}
			}
		}

		pCfg = (ST_drvCLTU*) ellNext(&pCfg->node);
	} /* while(pCfg) { */

#ifdef _CLTU_LINUX_
	epicsAtExit((EPICSEXITFUNC) clearAllOzTasks, NULL); 
/*	
	pCfg->fd_rtc = open ("/dev/rtc", O_RDONLY);
	if (pCfg->fd_rtc ==  -1) { 
		perror("/dev/rtc"); 
		return 0;
	}
*/
#endif

	a = drvCLTU_getCurrentTick();
	epicsThreadSleep(1.);
	pdrvCLTU_Config->one_sec_interval = drvCLTU_getCurrentTick() - a;

	printf("drvCLTU: measured one second time is %f sec\n", 1.E-3 * (double)pdrvCLTU_Config->one_sec_interval); 


		
#ifdef _CLTU_VXWORKS_ 
	/* interrupt enable  */
/*	intEnable((int)pCfg->pciDevice.int_line);
	WRITE32(pCfg->pciDevice.base0 + 0x68, 0x9010f); */
#endif

	return 0;
}

#ifdef _CLTU_LINUX_
static void clearAllOzTasks(void)
{
	ST_drvCLTU *pCfg;

	pCfg = (ST_drvCLTU*) ellFirst (pdrvCLTU_Config->ptaskList);

	while(pCfg) {
		/*	close(pCfg->fd_rtc); */

		if( pCfg->u8Version == VERSION_2 && (pCfg->u32Type == XTU_MODE_LTU))
		{
			FILE *fp=NULL;
			char fname[128];

			sprintf(fname, "%s/iocBoot/%s/%s%d", pCfg->strTOP, pCfg->strIOC, FILE_CALIB_VALUE, pCfg->xtu_ID);

			fp = fopen(fname, "w");
			if(!fp) {
				printf("CLTU_ERROR: can't open file \"%s\". \n", fname);
				return;
			}
			fprintf(fp, "%d\n", pCfg->u32Calib_val);
			fclose(fp);				
		}

		
		pCfg->taskStatus = TASK_FINISH;
	
		if( cltu_device_quit(pCfg) == WR_ERROR  )
			printf("\n>>> clearAllOzTasks : cltu_device_quit() ... error!\n");

		pCfg = (ST_drvCLTU*) ellNext(&pCfg->node);
	}
	printf("CLTU: exit!\n");	
	return;
}
#endif

LOCAL long drvCLTU_io_report(int level)
{
	ST_drvCLTU *pCfg;

	pCfg = (ST_drvCLTU*) ellFirst (pdrvCLTU_Config->ptaskList);

	if(!pdrvCLTU_Config) return 0;
	
	if(ellCount(pdrvCLTU_Config->ptaskList))
		pCfg = (ST_drvCLTU*) ellFirst (pdrvCLTU_Config->ptaskList);
	else {
		printf("Task not found\n");
		return 0;
	}

	printf("Totoal %d task(s) found\n",ellCount(pdrvCLTU_Config->ptaskList));

	if(level<1) return 0;

	while(pCfg) {
		
		printf("  Task name: %s, status: 0x%x\n", pCfg->taskName, pCfg->taskStatus	);
		if(level>2) {
			printf("   Sampling Rate: /sec\n" );
		}
		
		if(level>3 ) {
			printf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			printf("   ");
		}
		
		pCfg = (ST_drvCLTU*) ellNext(&pCfg->node);
	}
	
	return 0;
}

ST_drvCLTU* drvCLTU_find_taskConfig(char *taskName)
{
	ST_drvCLTU *pCfg = NULL;
	char task_name[60];

	strcpy(task_name, taskName);

	pCfg = (ST_drvCLTU*) ellFirst(pdrvCLTU_Config->ptaskList);
	while(pCfg) {
		if(!strcmp(pCfg->taskName, task_name)) return pCfg;
  		pCfg = (ST_drvCLTU*) ellNext(&pCfg->node);
	}

	return pCfg;
}

ST_drvCLTU* get_first_CLTUdev()
{
	if( !pdrvCLTU_Config) {
		printf("ERROR_CLTU: Null master pointer!\n");
		return NULL;
	}
	return (ST_drvCLTU*) ellFirst(pdrvCLTU_Config->ptaskList);
}


/*
ST_drvCLTU* drvCLTU_get_taskConfig()
{
	return pCfg;
}
*/
STATUS drvCLTU_set_shotStart (ST_drvCLTU *pCfg )
{
/*
	if( !check_ctu() ) {
  		printf("\n>>> I'm LTU , calibration run only CTU\n");
		return WR_ERROR;
	}
*/
	if( pCfg->u8Version == VERSION_1)
	{
		if ( cltu_cmd_set_shot_start( pCfg ) == WR_ERROR ) {
			printf("\n>>> drvCLTU_set_shotStart : cltu_cmd_set_shot_start() ... error!\n" );
			return WR_ERROR;
		}
	}
	else 
	{
		unsigned int current_time=0, current_tick;
		unsigned int shot_start_time, shot_start_tick;  

		/* R2 need shot end precedently. */
		drvCLTU_set_shotEnd(pCfg);
		
		epicsThreadSleep(0.2);

		if( is_mmap_ok( pCfg) )
		{
			xtu_GetCurrentTime(pCfg, &current_time, &current_tick);
			

			shot_start_tick = current_tick + 0x00030d40;

	/*		shot_start_tick += 34000;  *//* x5 = 170 us */
	/*		shot_start_tick += gap_R1_R2; */

			if (shot_start_tick > 0x0BEBC1FF)
			{
				shot_start_tick = shot_start_tick % 0x0BEBC1FF;
				shot_start_time = current_time + 1;
			}
			else
			{
				shot_start_tick = shot_start_tick;
				shot_start_time = current_time;
			}

			xtu_SetShotTime(pCfg, shot_start_time, shot_start_tick, 0xffffffff, 0xffffffff);
			ctu_SerdesTxImmediate(pCfg);
		}
		else {
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SHOT_START, &current_time);
			if (status == -1) return WR_ERROR;
#endif
		}

		
	}
/*
	if( ioc_debug==1)
		printf("\n>>> drvCLTU_set_shotStart : cltu_cmd_set_shot_start() ... OK!\n" );
*/
	return WR_OK;
}

STATUS drvCLTU_set_shotEnd(ST_drvCLTU * pCfg)
{
	if( pCfg->u8Version == VERSION_1)
	{
		;
	}
	else 
	{
		unsigned int current_tm, current_tk;
		unsigned int shot_end_tm, shot_end_tk;  

		if( is_mmap_ok( pCfg) ) {
			xtu_GetCurrentTime(pCfg, &current_tm, &current_tk);

			shot_end_tk = current_tk + 0x00030d40;

			if (shot_end_tk > 0x0BEBC1FF)
			{
				shot_end_tk = shot_end_tk % 0x0BEBC1FF;
				shot_end_tm = current_tm + 1;
			}
			else
			{
				shot_end_tk = shot_end_tk;
				shot_end_tm = current_tm;
			}

			xtu_SetShotTime(pCfg, 0xffffffff, 0xffffffff, shot_end_tm, shot_end_tk);

			ctu_SerdesTxImmediate(pCfg);
		} 
		else {
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SHOT_END, 0x0);
			if (status == -1) printf("devCLTU_BO_SHOT_END: IOCTL_CLTU_R2_SHOT_END error!!!\n");
#endif
		}

		
	}

	return WR_OK;
}


STATUS drvCLTU_set_portConfig (ST_drvCLTU *pCfg , unsigned int nPort)
{
	if( pCfg->u8Version == VERSION_1)
	{
		if( nPort == 4 ) {
			if( cltu_set_multiClock(pCfg) == WR_ERROR ){
				printf("\n>>> drvCLTU_set_portConfig (%d): cltu_set_multiTrig() ... error! \n", nPort);
				return WR_ERROR;
			}
			return WR_OK;
		}		
		if( nPort > 4 || nPort < 0 ) {
			printf("\n>>> drvCLTU_set_portConfig (%d): Port must be 0~4\n", nPort);
			return WR_ERROR;
		}
		
		if ( (pCfg->curPortConfigStatus[nPort] & PORT_TRIG_OK) &&
				(pCfg->curPortConfigStatus[nPort] & PORT_CLOCK_OK) &&
				(pCfg->curPortConfigStatus[nPort] & PORT_T0_OK) &&
				(pCfg->curPortConfigStatus[nPort] & PORT_T1_OK) ) 
		{
			if ( cltu_cmd_set_ct_ss( pCfg, nPort, pCfg->sPortConfig[nPort].nTrigMode, pCfg->sPortConfig[nPort].nClock,
									pCfg->sPortConfig[nPort].nT0_msb, pCfg->sPortConfig[nPort].nT0_lsb, 
									pCfg->sPortConfig[nPort].nT1_msb, pCfg->sPortConfig[nPort].nT1_lsb ) == ERROR ) {
				printf("\n>>> drvCLTU_set_portConfig : cltu_cmd_set_ct_ss(port:%d) ... error!\n", nPort );
				return WR_ERROR;
			}
		} else {
			printf("\n>>> drvCLTU_set_portConfig (%d): All port config must be setted!\n", nPort);
			return WR_ERROR;
		}
	}
	else
	{
		unsigned char sect;

		if( is_mmap_ok(pCfg) ) 
		{
			if( xtu_SetPortConfig(pCfg, (unsigned char)nPort, 
				pCfg->xPortConfig[nPort].cOnOff, 
				pCfg->xPortConfig[nPort].cActiveLow, 
				pCfg->xPortConfig[nPort].cTrigOnOff) == ERROR ) 
			{
					printf("\nCLTU_ERROR: drvCLTU_set_portConfig : xtu_SetPortConfig(port:%d) ... error!\n", nPort );
					return WR_ERROR;
			}
			
			for( sect=0; sect < NUM_SECTION_CNT; sect++) 
			{
				if( xtu_SetIntervalConfig(pCfg, (unsigned char)nPort, 
					sect, 
					pCfg->xPortConfig[nPort].startOffset[sect], 
					pCfg->xPortConfig[nPort].stopOffset[sect],
					pCfg->xPortConfig[nPort].clock[sect]	) == ERROR ) 
					{
						printf("\nCLTU_ERROR: drvCLTU_set_portConfig : xtu_SetIntervalConfig(port:%d, sect:%d) ... error!\n", nPort, sect );
						return WR_ERROR;
					}
			}
			if( xtu_SetPortRegister(pCfg) == ERROR )
			{
				printf("\n>>> devCLTU_BO_SETUP : xtu_SetPortRegister() ... error!\n");
				return WR_ERROR;
			}
		}
		else 
		{
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SELECT_PORT, &nPort);
			if (status == -1) return WR_ERROR;

			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SETUP_PORT, &pCfg->xPortConfig[nPort]);
			if (status == -1) return WR_ERROR;
#endif
		}

	}

	if( ioc_debug == 1) printf("\n>>> drvCLTU_set_portConfig : cltu_cmd_set_ct_ss(port:%d) ... OK!\n", nPort );
/*
	printf("clock: %d, t0_msb: 0x%x, t0_lsb: 0x%x\n", pCfg->portConfig[nPort].nClock,
												pCfg->portConfig[nPort].nT0_msb, 
												pCfg->portConfig[nPort].nT0_lsb );
*/
	return WR_OK;
}

STATUS drvCLTU_set_shotTerm(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		if ( cltu_cmd_set_shot_term( pCfg, pCfg->nShotTerm_msb, pCfg->nShotTerm_lsb ) == WR_ERROR )
			return WR_ERROR;
	} 
	else
		{


		}

	if( ioc_debug==1) printf("\n>>> drvCLTU_set_shotTerm : cltu_cmd_set_shot_term() ... OK!\n" );
	
	return WR_OK;
}

STATUS drvCLTU_set_clockOnShot (ST_drvCLTU *pCfg , unsigned int nPort, int onoff)
{
	if( pCfg->u8Version == VERSION_1)
	{
		if( cltu_cmd_set_ct_ss_rfm ( pCfg,  nPort, onoff, pCfg->sPortConfig[nPort].nClock) == WR_ERROR )
			return WR_ERROR;
	}
	else
		{

		}
	
	if( ioc_debug==1) printf("\n>>> drvCLTU_set_changeClock : cltu_cmd_set_ct_ss_rfm(port:%d) ... OK!\n", nPort );
		
	return WR_OK;
}

STATUS drvCLTU_run_calibration ( ST_drvCLTU *pCfg  )
{
	if( pCfg->u8Version == VERSION_1)
	{
	  	if( !check_ctu(pCfg) ) {
	  		printf("\n>>> I'm LTU , calibration run only CTU\n");
			return WR_ERROR;
		}
		if( cltu_cmd_cal_ltu( pCfg, pCfg->calib_target_ltu ) == WR_ERROR ) {
			printf("\n>>> drvCLTU_run_calibration : cltu_cmd_cal_ltu(ltu:%d) ... error!\n", pCfg->calib_target_ltu  );
			return WR_ERROR;
		}
	}
	else
	{
		if( is_mmap_ok(pCfg) ) {
			if( xtu_SetMode(pCfg, pCfg->u32Type, pCfg->xtu_ID) == ERROR ) 
				return WR_ERROR;
			if( xtu_reset(pCfg) == ERROR ) 
				return WR_ERROR;
		}
		else 
		{
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SET_MODE, &pCfg->u32Type);
			if (status == -1) {
				printf("drvCLTU_run_calibration: IOCTL_CLTU_R2_SET_MODE.  error!!\n");
				return WR_ERROR;
				}
#endif
		}
	}
	
	return WR_OK;	
}


/*
 0xffffffff -> external mode 
 0x0 -> internal mode
 */
STATUS drvCLTU_set_RefClock(ST_drvCLTU *pCfg, int ref_clk_sel)
{
	if( pCfg->u8Version == VERSION_1)
	{
		if( !check_ctu(pCfg) ) {
	  		printf("\n>>> I'm LTU , It's run only CTU\n");
			return WR_ERROR;
		}
		if( cltu_cmd_set_ref_clk(pCfg,  ref_clk_sel ) == WR_ERROR ){
			printf("\n>>> drvCLTU_set_RefClock : cltu_cmd_set_ref_clk(0x%x) ... error!\n", ref_clk_sel );
			return WR_ERROR;
		}
	}
	else
		{

		}
	return WR_OK;
}


STATUS cltu_set_multiClock(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		if ( (pCfg->curPortConfigStatus[4] & PORT_TRIG_OK) &&
				(pCfg->curPortConfigStatus[4] & PORT_CLOCK_OK) &&
				(pCfg->curPortConfigStatus[4] & PORT_T0_OK) &&
				(pCfg->curPortConfigStatus[4] & PORT_T1_OK) ) 
		{
/*
int cltu_cmd_set_ct_ms( int portnum, int trigger_level, 
                                    int clk0, int clk1, int clk2, int clk3, int clk4, 
									int trg0, int trg1, int trg2, int trg3, int trg4, 
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1,
                                    int msb_t2, int lsb_t2,
                                    int msb_t3, int lsb_t3,
                                    int msb_t4, int lsb_t4,
                                    int msb_t5, int lsb_t5 );
*/
			if( cltu_cmd_set_ct_ms( pCfg, 4, pCfg->mPortConfig[0].nTrigMode, 
										pCfg->mPortConfig[0].nClock, 0,
										pCfg->mPortConfig[1].nClock, 0,
										pCfg->mPortConfig[2].nClock,
				1, 0, 1, 0, 1,
	                                    pCfg->mPortConfig[0].nT0_msb, pCfg->mPortConfig[0].nT0_lsb, 
	                                    pCfg->mPortConfig[0].nT1_msb, pCfg->mPortConfig[0].nT1_lsb,
	                                    pCfg->mPortConfig[1].nT0_msb, pCfg->mPortConfig[1].nT0_lsb,
	                                    pCfg->mPortConfig[1].nT1_msb, pCfg->mPortConfig[1].nT1_lsb,
	                                    pCfg->mPortConfig[2].nT0_msb, pCfg->mPortConfig[2].nT0_lsb,
	                                    pCfg->mPortConfig[2].nT1_msb, pCfg->mPortConfig[2].nT1_lsb ) == WR_ERROR )
			{
				printf("\n>>> cltu_set_multiClock : cltu_cmd_set_ct_ms(port:4) ... error!\n");
				return WR_ERROR;
			} 
		}
		else {
			printf("\n>>> cltu_set_multiClock(): All multi-trig section must be setted!\n");
			return WR_ERROR;
		}
	}
	else
		{


		}

	if( ioc_debug==1) printf("\n>>> cltu_set_multiClock : cltu_cmd_set_ct_ms(port:4) ... OK!\n");

	return WR_OK;
}

/* manualy put calibrated offset value for LTU timing board */
STATUS drvCLTU_put_calibratedVal(ST_drvCLTU *pCfg , unsigned int calibVal)
{
	if( pCfg->u8Version == VERSION_1)
	{
		if( cltu_cmd_write32(pCfg,  0x70000000, calibVal ) == WR_ERROR ) {
			printf("\n>>> cltu_cmd_write32( 0x70000000, %d)... error!\n", calibVal);
			return WR_ERROR;
		}
	} 
	else if( pCfg->u8Version == VERSION_2)
	{
/*		unsigned int calVal;
		calVal = calibVal/2 + 31; */

/*		unsigned int CMPST_Tick;
		xtu_GetCompensationTime( &CMPST_Tick );
		calibVal = CMPST_Tick / 2 + 31; */

		pCfg->u32Calib_val = calibVal;

		if( is_mmap_ok(pCfg) )
  			ltu_SetCompensationTime(pCfg, calibVal); 
		else 
		{
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_SET_CAL_VALUE, &calibVal);
			if (status == -1) return WR_ERROR;
#endif
		}
		printf("%s: ltu time compensation    : %d\n", pCfg->taskName, pCfg->u32Calib_val);
	}
	else return WR_ERROR;


	return WR_OK;
}

STATUS drvCLTU_set_rtc(ST_drvCLTU *pCfg , unsigned int timer_sec)
{
#if defined(_CLTU_LINUX_)
	int retval; 
	struct rtc_time rtc_tm;
	unsigned int day, hour, min, sec;
	

/*	printf("curr raw GPS time (%ld)\n", timer_sec); */
	
	day = (int) (timer_sec/T_DAY_DIVIDER);
	timer_sec = timer_sec - (day * T_DAY_DIVIDER);
	hour = (int) (timer_sec/T_HOUR_DIVIDER );
	timer_sec = timer_sec - (hour * T_HOUR_DIVIDER);
	min = (int) (timer_sec/T_MIN_DIVIDER );
	timer_sec = timer_sec - (min * T_MIN_DIVIDER);
	sec = (int) (timer_sec/T_SEC_DIVIDER );

	if( ioc_debug == 3) printf("curr GPS time: %d, %d: %d: %d\n", day, hour, min, sec);

/*	fprintf(stderr, " Current RTC date/time is %d-%d-%d, %d:%d:%d. \n",
			rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
			rtc_tm.tm_hour+9, rtc_tm.tm_min, rtc_tm.tm_sec);
*/

	/* Read the RTC time/date */ 
	retval = ioctl(pCfg->fd_rtc, RTC_RD_TIME, &rtc_tm); 
	if (retval == -1) { 
		printf("\n>>> drvCLTU_set_rtc(): ioctl( get rtc )"); 
		return WR_ERROR;
	}
	rtc_tm.tm_hour = hour;
	rtc_tm.tm_min = min;
	rtc_tm.tm_sec = sec;

	retval = ioctl(pCfg->fd_rtc, RTC_SET_TIME, &rtc_tm); 
	if (retval == -1) { 
		printf("\n>>> drvCLTU_set_rtc(): ioctl( get rtc )"); 
		return WR_ERROR;
	}

#if 0
	/* Read the RTC time/date */ 
	retval = ioctl(pCfg->fd_rtc, RTC_RD_TIME, &rtc_tm); 
	if (retval == -1) { 
		printf("\n>>> drvCLTU_set_rtc(): ioctl( get rtc )"); 
		return WR_ERROR;
	} 
	fprintf(stderr, " Current RTC date/time is %d-%d-%d, %d:%d:%d. \n",
			rtc_tm.tm_mday, rtc_tm.tm_mon, rtc_tm.tm_year,
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);  

	
#endif

#if 0
	{
		struct timeval systime;
		char begin[30], *end;
		unsigned int  year, sys_sec, secOfyear = 60*60*24*365;

		gettimeofday(&systime, NULL);
/*    printf("%ld:%ld\n", systime.tv_sec, systime.tv_usec); */
		end = ctime(&systime.tv_sec);
		strcpy(begin, end);
		printf(	" %s\n" , begin);

		sys_sec = systime.tv_sec;
		year = (int) (sys_sec/T_YEAR_DIVIDER);
		sys_sec = sys_sec - (year * T_YEAR_DIVIDER);
		day = (int) (sys_sec/T_DAY_DIVIDER);
		sys_sec = sys_sec - (day * T_DAY_DIVIDER);
		hour = (int) (sys_sec/T_HOUR_DIVIDER );
		sys_sec = sys_sec - (hour * T_HOUR_DIVIDER);
		min = (int) (sys_sec/T_MIN_DIVIDER );
		sys_sec = sys_sec - (min * T_MIN_DIVIDER);
		sec = (int) (sys_sec/T_SEC_DIVIDER );
		
		printf("curr SYS time:y%d, d%d, %d: %d: %d\n", year, day, hour, min, sec);
	
		
		printf("(secOfyear: %ld)\n" , secOfyear);
		systime.tv_sec = secOfyear * (2007-1970) + timer_sec;
		systime.tv_usec = 0;
		
		printf("(%ld: %ld)\n" , systime.tv_sec, systime.tv_usec);
	
		retval = settimeofday(&systime, NULL);
		if( retval != 0 )
			printf("settimeofday() failed!\n");
	}
#endif

#elif defined(_CLTU_VXWORKS_)

#endif

	return WR_OK;
}

STATUS drvCLTU_show_info(ST_drvCLTU *pCfg )
{

	if( pCfg->u8Version == VERSION_1)
		cltu_pmc_info_show(pCfg);
	else {
		if( is_mmap_ok(pCfg) )
			xtu_Status(pCfg);
		else {
			xtu_Status_2(pCfg);
			printf("CLTU call xtu_Status_2().\n");
			}
	}

	return WR_OK;
}
/*
STATUS drvCLTU_set_tGap(unsigned int gap)
{
	printf("Old gap value: %d\n", gap_R1_R2);
	gap_R1_R2 = gap;
	printf("New gap value: %d\n", gap_R1_R2);
	
	return WR_OK;
}
*/

STATUS is_mmap_ok(ST_drvCLTU *pCfg )
{
	unsigned int val;
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: mmap check is only for R2 \n");
		return WR_ERROR;
	}
	val = READ32(pCfg->pciDevice.base0 + 0x0);

	if( val == 0xffffffff ) {
#ifdef _CLTU_VXWORKS_ 		
		printf("CLTU_ERROR: mmap check val = 0x%x \n", val);
#endif
		return WR_ERROR;
	}
	
	return WR_OK;
}



