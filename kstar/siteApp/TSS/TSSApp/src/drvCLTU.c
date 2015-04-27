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
+ STATUS drvCLTU_put_calibratedTick(int calibVal);


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


2013. 3. 19
FPGA major updated.

*/

#include "cltu_Def.h"
#include "drvCLTU.h"

#include "oz_cltu.h"
#include "oz_xtu2.h"



#define USE_INTLOCK   0



/* for DB put *********/
#include "dbDefs.h"
#include "dbBase.h"
#include "dbScan.h"
#include "dbStaticLib.h"
#include "db_access_routines.h"
#include "dbAccessDefs.h"
/* for DB put *********/













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
/* static ST_drvCLTU* init_variables(char * taskName, char *devf);  */
static int init_variables(ST_drvCLTU *pCfg);

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


int db_put(const char *pname, const char *pvalue)
{
	long  status;
	DBADDR addr;

	 if (!pname || !*pname || !pvalue) {
		printf("Usage: db_put \"pv name\", \"value\"\n");
		return WR_ERROR;
	}

	status=dbNameToAddr(pname, &addr);
	if (status) {
		printf("Record '%s' not found\n", pname);
		return WR_ERROR;
	}
	/* For enumerated types must allow for ENUM rather than string*/
	/* If entire field is digits then use DBR_ENUM else DBR_STRING*/
	if (addr.dbr_field_type == DBR_ENUM && !*pvalue &&
	strspn(pvalue,"0123456789") == strlen(pvalue)) {
		unsigned short value;

		sscanf(pvalue, "%hu", &value);
		status = dbPutField(&addr, DBR_ENUM, &value, 1L);
	} else if (addr.dbr_field_type == DBR_CHAR &&
		addr.no_elements > 1) {
		status = dbPutField(&addr, DBR_CHAR, pvalue, strlen(pvalue) + 1);
	} else {
		status = dbPutField(&addr, DBR_STRING, pvalue, 1L);
	}
	if (status) {
		errMessage(status,"- dbPutField error\n");
		return WR_ERROR;
	}
	return WR_OK;
}

int db_get(const char *pname, char *pvalue)
{
	long  status;
	DBADDR addr;
	
/*	DBADDR *paddr;
	dbFldDes  	*pdbFldDes;
	dbRecordType    *pdbRecordType; */
	
	char	*pfield_value;
	DBENTRY	dbentry;
	DBENTRY	*pdbentry = &dbentry;
	

	 if (!pname || !*pname || !pvalue) {
		printf( "ERROR!Usage: db_get \"pv name\", \"value\"\n");
		return WR_ERROR;
	}

	status=dbNameToAddr(pname, &addr);
	if (status) {
		printf( "ERROR!Record '%s' not found\n", pname);
		return WR_ERROR;
	}
/*	paddr = &addr;
	pdbFldDes = paddr->pfldDes;
	pdbRecordType = pdbFldDes->pdbRecordType; */
	

	dbInitEntry(pdbbase,pdbentry);
	status = dbFindRecord(pdbentry,pname);
	if(status) {
		errMessage(status,pname);
		return WR_ERROR;
	}
	
	status = dbFindField(pdbentry,"VAL:");
	pfield_value = (char *)dbGetString(pdbentry);
	sprintf(pvalue, "%s",  (pfield_value ? pfield_value : "null"));

	/*printf("\"%s\":VAL, %s\n", pname, pvalue );*/
	
	return WR_OK;
}



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
	pCfg->use_mmap_ctl = 1; /* important . only for VxWroks  2013. 3. 19 */

	
	/* endian control */
	if( pCfg->u8Version == VERSION_1) {
		cltu_cmd_endian(pCfg);
	} else
	{
		drvCLTU_set_Endian(pCfg);
		drvCLTU_set_bootStop(pCfg); /* important. must do it at this time. 2013. 5. 13 */
/*		drvCLTU_set_clockConfig(pCfg);
		drvCLTU_set_Mode(pCfg);
		drvCLTU_set_IRIG_B_Src(pCfg); */
		printf(">> %s CLTU driver init .... OK!!\n", pCfg->taskName);
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
		drvCLTU_set_Endian(pCfg);
		drvCLTU_set_bootStop(pCfg); /* important. must do it at this time. 2013. 5. 13 */
/*		drvCLTU_set_clockConfig(pCfg);
		drvCLTU_set_Mode(pCfg); 
		drvCLTU_set_IRIG_B_Src(pCfg); */
	}
	printf(">> %s CLTU driver init .... OK!!\n", pCfg->taskName);

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
	else if( pCfg->u8Version == VERSION_2 ) {
/*		if( pCfg->use_mmap_ctl ) */
			munmap( pCfg->pciDevice.base0, MMAP_SIZE_R2 );
	}

	/* Close Device File */
	close( pCfg->pciDevice.fd );

	return WR_OK;
}
#endif




/*static ST_drvCLTU* init_variables(char * taskName, char* devf)*/
static int init_variables(ST_drvCLTU *pCfg)
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
	pCfg->use_mmap_ctl = 1;
#else
	pCfg->use_mmap_ctl = 0;

#endif

	pCfg->calib_target_ltu = 0;
	pCfg->nCCS_shotNumber = 0;
	pCfg->taskStatus = 0x0;
	pCfg->enable_IRIGB = 1;
	pCfg->enable_LTU_freeRun = 0;
	pCfg->enable_CTU_comp = 0;
	pCfg->permanentClk = 0x0;
	pCfg->dev_num = numbering;

	for(i=0; i<NUM_PORT_CNT; i++) {
		memset(&pCfg->xtu_pcfg[i], 0, sizeof(ozPCONFIG_T));
		xtu_set_fillEnd(pCfg, i);
	}
	pCfg->portPolarity = 0x0;
	pCfg->compPutTime.ns = 1000000; /* recommand value is 1ms for CTU and all LTUs. */
	memset(&pCfg->absShotTime, 0, sizeof(ozSHOT_TIME_T));
	

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

/*		ST_drvCLTU    *pCfg; */

		epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId,
				         (void*) &queueData,
					 sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;	
		precord    = (struct dbCommon*) pexecParam->precord;
		prset      = (struct rset*) precord->rset;
/*		pCfg = (ST_drvCLTU *) pexecParam->pCfg; */

		if(!pFunc) continue;
		else pFunc(pexecParam);

		if(!precord) continue;


		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}


	return;
}


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


	pCfg = (ST_drvCLTU*) malloc(sizeof(ST_drvCLTU));
	if(!pCfg) return;

	pCfg->u8Version = (unsigned char)Ver;
	strcpy(pCfg->taskName, task_name);
	strcpy(pCfg->deviceFile, devf );
	pCfg->u32Type = (type == 0) ? XTU_MODE_LTU : XTU_MODE_CTU;
	pCfg->xtuMode.mode = (type == 0) ? XTU_MODE_LTU : XTU_MODE_CTU;
	pCfg->xtu_ID = ID;

	if( pCfg->u32Type == XTU_MODE_CTU) {
		sprintf(pCfg->strPVhead, "%s_CTU", pCfg->taskName);
		pCfg->xtuMode.irigb_sel    = XTU_IRIGB_EXT;
		pCfg->xtuMode.tlkRxClk_en  = XTU_TLKRX_CLK_DISABLE;
		pCfg->xtuMode.tlkTxClk_sel = XTU_TLKTX_CLK_REF;
		pCfg->xtuMode.mclk_sel     = XTU_MCLK_REF;
	} else {
		sprintf(pCfg->strPVhead, "%s_LTU", pCfg->taskName);
		pCfg->xtuMode.irigb_sel    = XTU_IRIGB_EXT;
		pCfg->xtuMode.tlkRxClk_en  = XTU_TLKRX_CLK_ENABLE;
		pCfg->xtuMode.tlkTxClk_sel = XTU_TLKTX_CLK_ONBOARD;
		pCfg->xtuMode.mclk_sel     = XTU_MCLK_TLK_RX;
	}
	pCfg->compPutTime.ns = 100;

	
		
	if(init_variables(pCfg) == WR_ERROR) return;


	pCfg->pdrvConfig = pdrvCLTU_Config;

	pCfg->pcontrolThreadConfig = make_controlThreadConfig(pCfg);
	if(!pCfg->pcontrolThreadConfig) return;

	
#ifdef _CLTU_VXWORKS_ 
	if( cltu_epics_device_init(pCfg) == WR_ERROR ) {
		printf("\n>>> drvCLTU_init_driver : cltu_epics_device_init() ... error!\n" );
		free(pCfg);
		return ;
	}
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
					sscanf(buf, "%d",  &pCfg->compPutTime.ns);
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
			fprintf(fp, "%d\n", pCfg->compPutTime.ns);
			fclose(fp);				
		}

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

STATUS drvCLTU_set_shotTime (ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		return WR_ERROR;
	}
	else 
	{
		/* R2 need shot end precedently. */
		/* 2013. 11. 04.   function be retrieved.  */
		if( pCfg->taskStatus & TASK_STATE_GSHOT_ON ) {
			drvCLTU_set_shotEnd(pCfg);
			epicsThreadSleep(0.1); 
		}  /*2013. 7. 29  always send shot end when starting*/
		
		if( is_mmap_ok( pCfg) )
			xtu_SetShotTime(pCfg, &pCfg->absShotTime);
		else {
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_SHOT_TIME, &pCfg->absShotTime);
			if (status == -1) return WR_ERROR;
#endif
		}
		printf("start time   : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pCfg->absShotTime.start.day
															,pCfg->absShotTime.start.hour
															,pCfg->absShotTime.start.min
															,pCfg->absShotTime.start.sec
															,pCfg->absShotTime.start.ms
															,pCfg->absShotTime.start.ns);
/*		printf("stop time   : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pCfg->absShotTime.stop.day
															,pCfg->absShotTime.stop.hour
															,pCfg->absShotTime.stop.min
															,pCfg->absShotTime.stop.sec
															,pCfg->absShotTime.stop.ms
															,pCfg->absShotTime.stop.ns); */
	}
	
	return WR_OK;

}

STATUS drvCLTU_set_shotStart (ST_drvCLTU *pCfg )
{

	if( pCfg->u8Version == VERSION_1)
	{
		if ( cltu_cmd_set_shot_start( pCfg ) == WR_ERROR ) {
			printf("\n>>> drvCLTU_set_shotStart : cltu_cmd_set_shot_start() ... error!\n" );
			return WR_ERROR;
		}
	}
	else 
	{
		/* R2 need shot end precedently. */
		/* 2013. 11. 04.   function be retrieved.  */
		if( pCfg->taskStatus & TASK_STATE_GSHOT_ON ) {
			drvCLTU_set_shotEnd(pCfg);
			epicsThreadSleep(0.1); 
		}  /*2013. 7. 29  always send shot end when starting*/
/*		drvCLTU_set_shotEnd(pCfg);
		epicsThreadSleep(0.1);  */
		
		if( is_mmap_ok( pCfg) )
			xtu_StartShotNow(pCfg);
		else {
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SHOT_START, 0x0);
			if (status == -1) return WR_ERROR;
#endif
		}
	}
	
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

		if( is_mmap_ok( pCfg) ) {

			xtu_StopShotNow(pCfg);
		} 
		else {
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SHOT_END, 0x0);
			if (status == -1) printf("devCLTU_BO_SHOT_END: IOCTL_XTU_SHOT_END error!!!\n");
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
		xtu_set_ConfigParsing( pCfg, nPort);
/*		xtu_print_decoded_cfg(pCfg, nPort); */

		xtu_set_ConfigRegWrite(pCfg, nPort);

	}

	if( ioc_debug == 1) printf("\n>>> drvCLTU_set_portConfig : cltu_cmd_set_ct_ss(port:%d) ... OK!\n", nPort );
	
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
			if( xtu_SetMode(pCfg, pCfg->xtuMode.mode, pCfg->xtu_ID) == ERROR ) 
				return WR_ERROR;
		}
		else 
		{
#ifdef _CLTU_LINUX_
			int status;
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_MODE, &pCfg->xtuMode.mode);
			if (status == -1) {
				printf("drvCLTU_run_calibration: IOCTL_XTU_SET_MODE.  error!!\n");
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
STATUS drvCLTU_put_calibratedTick(ST_drvCLTU *pCfg)
{
	if( pCfg->u8Version == VERSION_1)
	{
		if( cltu_cmd_write32(pCfg,  0x70000000, pCfg->compPutTime.ns) == WR_ERROR ) {
			printf("\n>>> cltu_cmd_write32( 0x70000000, %d)... error!\n", pCfg->compPutTime.ns);
			return WR_ERROR;
		}
	} 
	else if( pCfg->u8Version == VERSION_2)
	{
/*		unsigned int calVal;
		calVal = calibVal/2 + 31; */

/*		unsigned int CMPST_Tick;
		xtu_GetCompensationValue( &CMPST_Tick );
		calibVal = CMPST_Tick / 2 + 31; */

		if( is_mmap_ok(pCfg) ) {
			if( pCfg->u32Type == XTU_MODE_LTU )
				xtu_SetLTUShotDelay(pCfg);
			else
				xtu_SetCTUShotDelay(pCfg);
		} 
		else 
		{
#ifdef _CLTU_LINUX_
			int status;
			if( pCfg->u32Type == XTU_MODE_LTU ) 
				status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_LTU_DELAY, &(pCfg->compPutTime.ns));
			else 
				status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_CTU_DEFAULT_DELAY, &(pCfg->compPutTime.ns));
			
			if (status == -1) return WR_ERROR;
#endif
		}
		printf("%s: ltu compensation ns    : %d\n", pCfg->taskName, pCfg->compPutTime.ns);
	}
	else return WR_ERROR;


	return WR_OK;
}

STATUS drvCLTU_set_bootStop(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1) return WR_ERROR;


	pCfg->xtuMode.irigb_sel = 1;
	drvCLTU_set_IRIG_B_Src(pCfg );

	pCfg->xtuMode.mode = XTU_MODE_CTU;
	drvCLTU_set_Mode(pCfg);

	pCfg->xtuMode.tlkRxClk_en = XTU_TLKRX_CLK_DISABLE;
	drvCLTU_set_clockConfig(pCfg);

	pCfg->xtuMode.tlkTxClk_sel = XTU_TLKTX_CLK_ONBOARD;
	drvCLTU_set_clockConfig(pCfg);

	pCfg->xtuMode.mclk_sel = 0;
	drvCLTU_set_clockConfig(pCfg);

	drvCLTU_set_shotEnd(pCfg);


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
	else 
	{
		if( is_mmap_ok(pCfg) )
			xtu_GetXtuInfo(pCfg);
		else {
			xtu_GetXtuInfo_2(pCfg);
		}
	}

	return WR_OK;
}
STATUS drvCLTU_show_status(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1) return WR_ERROR;

	if( is_mmap_ok(pCfg) )	xtu_GetXtuStatus(pCfg);
	else  xtu_GetXtuStatus_2(pCfg);
	
	return WR_OK;
}
STATUS drvCLTU_show_time(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1) return WR_ERROR;

	if( is_mmap_ok(pCfg) )
		xtu_GetCurrTimeStatus(pCfg);
	else
		xtu_GetCurrTimeStatus_2(pCfg);
	
	return WR_OK;
}
STATUS drvCLTU_show_tlk(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1) return WR_ERROR;

	if( is_mmap_ok(pCfg) )
		xtu_GetTlkRxTimeStatus(pCfg);
	else
		xtu_GetTlkRxTimeStatus_2(pCfg);
	
	return WR_OK;
}
STATUS drvCLTU_show_log_gshot(ST_drvCLTU *pCfg )
{
	unsigned int logbuf[XTU_BMEM_SIZE];
	int i;
	unsigned char hD, mD, sD;
	unsigned int  msD, nsD;

	if( pCfg->u8Version == VERSION_1) return WR_ERROR;


	if( is_mmap_ok(pCfg) )
		 xtu_GetGshotLog( pCfg, logbuf);
	else {
#ifdef _CLTU_LINUX_		
		i = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_LOG_GSHOT, logbuf);
		if (i == -1) { printf("ioctl, IOCTL_XTU_GET_LOG_GSHOT, error\n"); return WR_ERROR; }
#endif
	}

	printf("\n>>> Global Shot log .....\n");
	for(i = 0; i < XTU_BMEM_SIZE/4; i++)
	{
		hms_HtoD( &(logbuf[i*4+1]), &hD, &mD, &sD  );
		ms100MTick_HtoD( &logbuf[i*4+2], &msD, &nsD );

		if(0xa0000000 == (logbuf[i*4] & 0xf0000000) )
			printf("start-> %03d_%02d:%02d:%02d.%03d.%07d\n",logbuf[i*4] & 0x00000fff,hD,mD,sD,msD,nsD);
		if(0xb0000000 == (logbuf[i*4] & 0xf0000000) )
			printf(" stop <-%03d_%02d:%02d:%02d.%03d.%07d\n",logbuf[i*4] & 0x00000fff,hD,mD,sD,msD,nsD);
	}

	 
	
	
	return WR_OK;
}

STATUS drvCLTU_show_log_port(ST_drvCLTU *pCfg, int port )
{
	unsigned int i;
	unsigned char hD, mD, sD;
	unsigned int	msD, nsD;
	unsigned int	id_tmp;
	unsigned int	hli_tmp;
	unsigned int	lli_tmp;
	unsigned int logbuf[XTU_BMEM_SIZE];

	if( pCfg->u8Version == VERSION_1) return WR_ERROR;

	printf("%s: show port '%d' log.\n", pCfg->taskName, port);

	if( is_mmap_ok(pCfg) )
		 xtu_GetPortLog( pCfg,  port, logbuf);
	else {
#ifdef _CLTU_LINUX_
		i = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SELECT_PORT, &port);
		if (i == -1) { printf("ioctl, IOCTL_XTU_SELECT_PORT, error\n"); return WR_ERROR; }
			
		i = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_LOG_PORT, logbuf);
		if (i == -1) { printf("ioctl, IOCTL_XTU_GET_LOG_PORT, error\n"); return WR_ERROR; }
#endif
	}

	for(i = 0; i < XTU_BMEM_SIZE/2; i++)
	{
		id_tmp  = (logbuf[i*2] & 0xF0000000)>>28;

		hD = (logbuf[i*2] & 0x0003f000)>>12;
		mD = (logbuf[i*2] & 0x00000fc0)>>6;
		sD = (logbuf[i*2] & 0x0000003f);


		ms200MTick_HtoD( &logbuf[i*2+1], &msD, &nsD );

		hli_tmp = ((logbuf[i*2] & 0x0ff80000)>>19)/4 - 1;
		lli_tmp = ((logbuf[i*2] & 0x0ff80000)>>19)/4 - 16;


		switch(id_tmp)
		{
		case 0x00000008 :
			printf("start   : %03dh:%02dm:%02ds/%03dms.%07dns\n",hD,mD,sD,msD,nsD);
			break;
		case 0x00000004 :
			printf("hli[%3d]: %03dh:%02dm:%02ds/%03dms.%07dns\n",hli_tmp,hD,mD,sD,msD,nsD);
			break;
		case 0x00000002 :
			printf("  lli[%3d]: %03dh:%02dm:%02ds/%03dms.%07dns\n",lli_tmp,hD,mD,sD,msD,nsD);
			break;
		case 0x00000001 :
			printf("-->g_shot period : %03dh:%02dm:%02ds/%03dms.%07dns\n",hD,mD,sD,msD,nsD);
			break;
		default :			
			return WR_OK;
			break;
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

/* default: use PIO control */
STATUS is_mmap_ok(ST_drvCLTU *pCfg )
{
	unsigned int val;
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: mmap check is only for R2 \n");
		return WR_ERROR;
	}
	
	if( pCfg->use_mmap_ctl ) 
	{
		val = READ32(pCfg->pciDevice.base0 + 0x0);
		
		if( val == 0xffffffff ) {
#ifdef _CLTU_VXWORKS_ 		
			printf("CLTU_ERROR: mmap check val = 0x%x \n", val);
#endif
			return WR_ERROR;
		}
		return WR_OK;
	}
	
	return WR_ERROR;
}

STATUS drvCLTU_set_PortEnable(ST_drvCLTU *pCfg, int port )
{

#ifdef _CLTU_VXWORKS_
	xtu_SetPortCtrl(pCfg, port);
#else
	int status, value;

	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SELECT_PORT, &port);
	if (status == -1) {
		printf("error IOCTL_XTU_SELECT_PORT\n");
		return WR_ERROR;
	}
	value = pCfg->xtu_pcfg[port].nhli_num;
	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_NHLI_NUM, &value );
	if (status == -1) {
		printf("error IOCTL_XTU_SET_PORT_NHLI_NUM\n"); 
		return WR_ERROR;
	}
			
	value = pCfg->xtu_pcfg[port].port_en;
	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_CTL, &value);
	if (status == -1) {
		printf("error IOCTL_XTU_SET_PORT_CTL\n"); 
		return WR_ERROR;
	}
#endif

	return WR_OK;
}


/*  MCLK_SEL, TLKTX_SEL, TLKRX_ENABLE  */
/* pCfg->xtuMode.tlkRxClk_en | pCfg->xtuMode.tlkTxClk_sel | pCfg->xtuMode.mclk_sel  */
STATUS drvCLTU_set_clockConfig(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: clockConfig is only for R2 \n");
		return WR_ERROR;
	}

	if( is_mmap_ok(pCfg) ) {
		xtu_SetClkConfig(pCfg);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_CLK_CONFIG, &pCfg->xtuMode);
		if (status == -1) 
		{
			printf("drvCLTU_set_clockConfig: IOCTL_XTU_SET_CLK_CONFIG.  error!!\n");
			return WR_ERROR;
		}
#endif
	}

	return WR_OK;
}

STATUS drvCLTU_set_Mode(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: clockConfig is only for R2 \n");	return WR_ERROR;
	}

	if( is_mmap_ok(pCfg) ) {
		xtu_SetMode(pCfg, pCfg->xtuMode.mode, pCfg->xtu_ID);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_MODE, &pCfg->xtuMode.mode);
		if (status == -1) 
		{
			printf("drvCLTU_set_Mode: IOCTL_XTU_SET_MODE.  error!!\n");
			return WR_ERROR;
		}
#endif
	}
	
	return WR_OK;
}

STATUS drvCLTU_set_IRIG_B_Src(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: clockConfig is only for R2 \n");	return WR_ERROR;
	}

	if( is_mmap_ok(pCfg) ) {
		xtu_SetIRIG_B_Src(pCfg);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_IRIGB_SRC, &pCfg->xtuMode.irigb_sel);
		if (status == -1) 
		{
			printf("drvCLTU_set_IRIG_B_Src: IOCTL_XTU_SET_IRIGB_SRC.  error!!\n");
			return WR_ERROR;
		}
#endif
	}
	
	return WR_OK;
}

STATUS drvCLTU_set_PortActiveLevel(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: clockConfig is only for R2 \n");	return WR_ERROR;
	}

	if( is_mmap_ok(pCfg) ) {
		xtu_SetPortActiveLevel(pCfg, pCfg->portPolarity);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_ACTIVE_LEVEL, &pCfg->portPolarity);
		if (status == -1) { printf("ioctl, IOCTL_XTU_SET_PORT_ACTIVE_LEVEL, error\n"); return WR_ERROR; }
#endif
	}
	
	return WR_OK;
}
STATUS drvCLTU_set_Endian(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: drvCLTU_set_Endian is only for R2 \n");	return WR_ERROR;
	}

	if( is_mmap_ok(pCfg) ) {
		xtu_SetEndian(pCfg);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_ENDIAN, &pCfg->portPolarity);
		if (status == -1) { printf("ioctl, IOCTL_XTU_SET_ENDIAN, error\n"); return WR_ERROR; }
#endif
	}
	printf("[XTU] ioctl, IOCTL_XTU_SET_ENDIAN, ok\n");
	
	return WR_OK;
}

STATUS drvCLTU_set_PermanentClk(ST_drvCLTU *pCfg )
{
	if( pCfg->u8Version == VERSION_1)
	{
		printf("CLTU_ERROR: drvCLTU_set_PermanentClk is only for R2 \n");	return WR_ERROR;
	}

	if( is_mmap_ok(pCfg) ) {
		xtu_SetPermClk(pCfg);
	}
	else {
#ifdef _CLTU_LINUX_		
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PERMANENT_CLK, &pCfg->permanentClk);
		if (status == -1) { printf("ioctl, IOCTL_XTU_SET_PERMANENT_CLK, error\n"); return WR_ERROR; }
#endif
	}
	return WR_OK;
}

STATUS xtu_set_ConfigRegWrite(ST_drvCLTU *pCfg, int port)
{
	unsigned int sect;
	
	if( is_mmap_ok(pCfg) ) 
	{
		if( xtu_SetPortCtrl(pCfg, (unsigned char)port ) == ERROR ) 
		{
			printf("\nCLTU_ERROR: xtu_set_ConfigRegWrite : xtu_SetPortCtrl(port:%d) ... error!\n", port );
			return WR_ERROR;
		}
		
		for( sect=0; sect < pCfg->cur_hli_number; sect++) 
		{
			if( xtu_SetPortHLI(pCfg, port, sect, &pCfg->xtu_pcfg[port].phli[sect] ) == ERROR ) 
			{
				printf("\nCLTU_ERROR: xtu_set_ConfigRegWrite : xtu_SetPortHLI(port:%d, sect:%d) ... error!\n", port, sect );
				return WR_ERROR;
			}
		}
		for( sect=0; sect < pCfg->cur_lli_number; sect++) 
		{
			if( xtu_SetPortLLI(pCfg, port, sect, &pCfg->xtu_pcfg[port].plli[sect] ) == ERROR ) 
			{
				printf("\nCLTU_ERROR: xtu_set_ConfigRegWrite : xtu_SetPortHLI(port:%d, sect:%d) ... error!\n", port, sect );
				return WR_ERROR;
			}
		}
		
	}
	else 
	{
#ifdef _CLTU_LINUX_
		int status, value;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SELECT_PORT, &port);
		if (status == -1) return WR_ERROR;

		value = pCfg->xtu_pcfg[port].nhli_num;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_NHLI_NUM, &value );
		if (status == -1) return WR_ERROR;

		value = pCfg->xtu_pcfg[port].port_en;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_CTL, &value );
		if (status == -1) return WR_ERROR;

/*			for( sect=0; sect < XTU_ADDR_PHLI_MAX_NUM; sect++)  */
		for( sect=0; sect < pCfg->cur_hli_number; sect++) 
		{
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SELECT_HLI, &sect);
			if (status == -1) return WR_ERROR;
			
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_HLI, &pCfg->xtu_pcfg[port].phli[sect]);
			if (status == -1) return WR_ERROR;
		}
/*			for( sect=0; sect < XTU_ADDR_PLLI_MAX_NUM; sect++) */
		for( sect=0; sect < pCfg->cur_lli_number; sect++) 
		{
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SELECT_LLI, &sect);
			if (status == -1) return WR_ERROR;
			
			status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_PORT_LLI, &pCfg->xtu_pcfg[port].plli[sect]);
			if (status == -1) return WR_ERROR;
		}
#endif
	}

	return WR_OK;
}

STATUS xtu_set_ConfigParsing(ST_drvCLTU *pCfg, int port)
{

	/* 1. set clear */
	memset(&pCfg->xtu_pcfg[port], 0, sizeof(ozPCONFIG_T));
	/* 2. configure end point */
	xtu_set_fillEnd(pCfg, port);

	pCfg->cur_hli_number = 1;
	pCfg->cur_lli_number = 0;

	/* 3. port enable / disable */
	pCfg->xtu_pcfg[ port ].port_en = (unsigned int)(pCfg->xPortConfig[ port ].cOnOff) ? XTU_PORT_ENABLE : XTU_PORT_DISABLE;
	pCfg->xtu_pcfg[port].nhli_num = 0;
	
	/* 4. make   */
	pCfg->xtu_pcfg[port].phli[0].intv.ns = XTU_MAX_INF_NS;  /* use only single HLI */
	pCfg->xtu_pcfg[port].phli[0].nhli_num = 0; /* don't care. cause unlimited clock..*/
	pCfg->xtu_pcfg[port].phli[0].nlli_num = 0;
	

	/* 5. configrue section */
	if( pCfg->xPortConfig[port].cTrigOnOff )
	{
		xtu_set_ConfigTrig(pCfg, port);
	}
	else 
	{
		xtu_set_ConfigClock(pCfg, port);

	}

	return WR_OK;
}

STATUS xtu_set_ConfigTrig(ST_drvCLTU *pCfg, int port)
{
	double dSrc;
	int r1, r2;
	int oneSect = 0;


	/* 1. first invalid section, should be high or low */
	pCfg->xtu_pcfg[port].plli[0].clk_freq = 0;
	pCfg->xtu_pcfg[port].plli[0].nlli_num = 1;
	pCfg->xtu_pcfg[port].plli[0].option = XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON ;

	if (  pCfg->xPortConfig[port].dT1[0] <= pCfg->xPortConfig[port].dT0[0] ) { 
		/* when user set all time to zero,  this port is not available */
		pCfg->xtu_pcfg[port].plli[0].nlli_num = 0;
		pCfg->xtu_pcfg[port].plli[0].intv.ns = XTU_MAX_INF_NS;
		pCfg->cur_lli_number = 1;
		return OK;
	} 
	else if( pCfg->xPortConfig[port].dT0[0] < 0.00000020 ) {
		/* HLI takes 55ns, LLI takes 65ns */
		/* if user set the first section time as zero, we consider this consume time */
		xtu_conv_sec2ozTime( &(pCfg->xtu_pcfg[port].plli[0].intv), 0.00000020 );
		oneSect = 1;
	} 	
	else {
		xtu_conv_sec2ozTime( &(pCfg->xtu_pcfg[port].plli[0].intv), pCfg->xPortConfig[port].dT0[0]);
	}


	
	for( r1=0, r2=1; r1<NUM_SECTION_CNT; r1++, r2+=2) 
	{
		
		/* first valid section, first T0 event */
		pCfg->xtu_pcfg[port].plli[r2].clk_freq = 0;
		pCfg->xtu_pcfg[port].plli[r2].nlli_num = r2+1;
		dSrc = pCfg->xPortConfig[port].dT1[r1] - pCfg->xPortConfig[port].dT0[r1];
		if( oneSect ) { 
			/*  It available only the first section, so first section pulse will be shorter than origin */
			dSrc -= 0.00000020;
			oneSect = 0;
		}
		xtu_conv_sec2ozTime( &pCfg->xtu_pcfg[port].plli[r2].intv, dSrc);
		pCfg->xtu_pcfg[port].plli[ r2 ].option = XTU_PORT_CLK_OPT_ENABLE | XTU_PORT_LLI_OPT_LOG_ON ;

		/* next  invalid section  */
		if( pCfg->xPortConfig[port].dT0[ r1+1 ] <= pCfg->xPortConfig[port].dT1[r1] ) 
		{
			/* shot end */
			pCfg->xtu_pcfg[port].plli[r2+1].clk_freq = 0;
			pCfg->xtu_pcfg[port].plli[r2+1].nlli_num = r2+1;
			pCfg->xtu_pcfg[port].plli[r2+1].intv.ns = XTU_MAX_INF_NS;
			pCfg->xtu_pcfg[port].plli[r2+1].option = XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON ;
			pCfg->cur_lli_number = r2+2;
			break;
		}
		else 
		{
			pCfg->xtu_pcfg[port].plli[r2+1].clk_freq = 0;
			pCfg->xtu_pcfg[port].plli[r2+1].nlli_num = r2+2;
			dSrc = pCfg->xPortConfig[port].dT0[r1+1] - pCfg->xPortConfig[port].dT1[r1];
			xtu_conv_sec2ozTime( &pCfg->xtu_pcfg[port].plli[r2+1].intv, dSrc);
			pCfg->xtu_pcfg[port].plli[r2+1].option = XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON ;

		}
	}
		
	return OK;
}
STATUS xtu_set_ConfigClock(ST_drvCLTU *pCfg, int port)
{
	double dSrc;
	int r1, r2;
	int oneSect = 0;
	
	/* 1. first invalid section, should be high or low */
	pCfg->xtu_pcfg[port].plli[0].clk_freq = 0;
	pCfg->xtu_pcfg[port].plli[0].nlli_num = 1;
	pCfg->xtu_pcfg[port].plli[0].option = XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON ;

	if (  pCfg->xPortConfig[port].dT1[0] <= pCfg->xPortConfig[port].dT0[0] ) { 
		/* when user set all time to zero,  this port is not available */
		pCfg->xtu_pcfg[port].plli[0].nlli_num = 0;
		pCfg->xtu_pcfg[port].plli[0].intv.ns = XTU_MAX_INF_NS;
		pCfg->cur_lli_number = 1;
		return OK;
	} 
	else if( pCfg->xPortConfig[port].dT0[0] < 0.00000020 ) {
		/* HLI takes 55ns, LLI takes 65ns */
		/* if user set the first section time as zero, we consider this consume time */
		xtu_conv_sec2ozTime( &(pCfg->xtu_pcfg[port].plli[0].intv), 0.00000020 );
		oneSect = 1;
	} 	
	else {
		xtu_conv_sec2ozTime( &(pCfg->xtu_pcfg[port].plli[0].intv), pCfg->xPortConfig[port].dT0[0]);
	}


	for( r1=0, r2=1; r1<NUM_SECTION_CNT; r1++, r2+=2) 
	{
		/* first valid section, first T0 event */
		if( pCfg->xPortConfig[port].r2Clock[r1] == 100000000 ) { /* 100MHz */
			pCfg->xtu_pcfg[port].plli[r2].clk_freq = 0;
			pCfg->xtu_pcfg[port].plli[r2].option = XTU_PORT_CLK_OPT_100MHz | XTU_PORT_LLI_OPT_LOG_ON ;

		} else if (pCfg->xPortConfig[port].r2Clock[r1] == 50000000 ) { 
			pCfg->xtu_pcfg[port].plli[r2].clk_freq = 0;
			pCfg->xtu_pcfg[port].plli[r2].option = XTU_PORT_CLK_OPT_50MHz | XTU_PORT_LLI_OPT_LOG_ON ;

		}
		else {
			pCfg->xtu_pcfg[port].plli[r2].clk_freq = pCfg->xPortConfig[port].r2Clock[r1];
			pCfg->xtu_pcfg[port].plli[r2].option = XTU_PORT_CLK_OPT_VARFREQ | XTU_PORT_LLI_OPT_LOG_ON ;
		}
		pCfg->xtu_pcfg[port].plli[r2].nlli_num = r2+1;
		dSrc = pCfg->xPortConfig[port].dT1[r1] - pCfg->xPortConfig[port].dT0[r1];
		if( oneSect ) { 
			/*  It available only the first section, so first section pulse will be shorter than origin */
			dSrc -= 0.00000020;
			oneSect = 0;
		}
		xtu_conv_sec2ozTime( &pCfg->xtu_pcfg[port].plli[r2].intv, dSrc);
		

		/* next  invalid section  */
		if( pCfg->xPortConfig[port].dT0[ r1+1 ] <= pCfg->xPortConfig[port].dT1[r1] ) 
		{
			/* shot end */
			pCfg->xtu_pcfg[port].plli[r2+1].clk_freq = 0;
			pCfg->xtu_pcfg[port].plli[r2+1].nlli_num = r2+1;
			pCfg->xtu_pcfg[port].plli[r2+1].intv.ns = XTU_MAX_INF_NS;
			pCfg->xtu_pcfg[port].plli[r2+1].option = XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON ;
			pCfg->cur_lli_number = r2+2;
			break;
		}
		else 
		{
			pCfg->xtu_pcfg[port].plli[r2+1].clk_freq = 0;
			pCfg->xtu_pcfg[port].plli[r2+1].nlli_num = r2+2;
			dSrc = pCfg->xPortConfig[port].dT0[r1+1] - pCfg->xPortConfig[port].dT1[r1];
			xtu_conv_sec2ozTime( &pCfg->xtu_pcfg[port].plli[r2+1].intv, dSrc);
			pCfg->xtu_pcfg[port].plli[r2+1].option = XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON ;

		}
	}

	return OK;
}



void xtu_conv_sec2ozTime( ozTIME_T *pTime, double sec )
{
	unsigned int tsec = (unsigned int) sec;
	unsigned int mst;
	unsigned int rest9;

	if (sec < 0 ) sec = 0;
	
	pTime->day =  (unsigned int)(tsec/T_DAY_DIVIDER);
	pTime->hour = (unsigned int)(tsec / 3600 ) % 24; 
	pTime->min = (unsigned int)(tsec / 60 ) % 60;
	pTime->sec = tsec % 60;
	rest9 = (unsigned int)((sec - (pTime->day * T_DAY_DIVIDER) - pTime->hour * 3600 - pTime->min * 60 - pTime->sec) *1000000000.0 + 0.5); 
	pTime->ms = (unsigned int)(rest9 * 0.0000010);

	mst = (unsigned int)(rest9 - pTime->ms*1000000 );

	pTime->ns = mst;
/*	printf("sec double(%.10f): %d_%d:%d:%d.%d.%d\n", sec, pTime->day,pTime->hour, pTime->min,pTime->sec,pTime->ms,pTime->ns); */
	
}

STATUS xtu_set_fillEnd(ST_drvCLTU *pCfg, int port)
{
	/* this is last end point setting */
/*	pCfg->xtu_pcfg[port].phli[NUM_END_HLI_14].intv.ns  = XTU_MAX_INF_NS;
	pCfg->xtu_pcfg[port].phli[NUM_END_HLI_14].nhli_num   = NUM_END_HLI_14;
	pCfg->xtu_pcfg[port].phli[NUM_END_HLI_14].nlli_num   = NUM_END_LLI_111;
*/
	pCfg->xtu_pcfg[port].plli[NUM_END_LLI_111].clk_freq = 0;
	pCfg->xtu_pcfg[port].plli[NUM_END_LLI_111].intv.ns   = XTU_MAX_INF_NS;
	pCfg->xtu_pcfg[port].plli[NUM_END_LLI_111].option = (pCfg->xPortConfig[port].cActiveLow) ? (XTU_PORT_CLK_OPT_ENABLE | XTU_PORT_LLI_OPT_LOG_ON)  : (XTU_PORT_CLK_OPT_DISABLE | XTU_PORT_LLI_OPT_LOG_ON );
	pCfg->xtu_pcfg[port].plli[NUM_END_LLI_111].nlli_num    = NUM_END_LLI_111;

	return WR_OK;
}

void xtu_print_decoded_cfg(ST_drvCLTU *pCfg, int port)
{
	int i;

	printf("port: %d, port_en: %d, nhli_num: %d\n", port, pCfg->xtu_pcfg[port].port_en, pCfg->xtu_pcfg[port].nhli_num);
	
/*	for(i=0; i<XTU_ADDR_PHLI_MAX_NUM; i++) */
	for(i=0; i<2; i++)	
	{
		printf("HLI %d\n", i);
		printf(" sec, ms, ns ( %d, %d, %d)\n", pCfg->xtu_pcfg[port].phli[i].intv.sec, pCfg->xtu_pcfg[port].phli[i].intv.ms, pCfg->xtu_pcfg[port].phli[i].intv.ns);
		printf(" nhli_num, nlli_num ( %d, %d)\n", pCfg->xtu_pcfg[port].phli[i].nhli_num, pCfg->xtu_pcfg[port].phli[i].nlli_num);
	}

/*	for(i=0; i<XTU_ADDR_PLLI_MAX_NUM; i++) */
	for(i=0; i<5; i++)
	{
		printf("LLI %d\n", i);
		printf("  sec, ms, ns ( %d, %d, %d)\n", pCfg->xtu_pcfg[port].plli[i].intv.sec, pCfg->xtu_pcfg[port].plli[i].intv.ms, pCfg->xtu_pcfg[port].plli[i].intv.ns);
		printf("    clk_freq, nlli_num ( %d, %d)\n", pCfg->xtu_pcfg[port].plli[i].clk_freq, pCfg->xtu_pcfg[port].plli[i].nlli_num);
		printf("    option: 0x%x\n", pCfg->xtu_pcfg[port].plli[i].option);
	}
	



}


