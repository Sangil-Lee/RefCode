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
STATUS drvCLTU_set_clockOnShot (int nPort, int onoff)


2007. 11. 06.
clearAllOzTasks() function is only for Linux system.


2007. 11. 15.
+ STATUS drvCLTU_put_calibratedVal(int calibVal);


2007. 11. 20.
changed STATUS cltu_set_multiClock();

int check_ctu( void ) {
+ System_Info* mSys;
+ mSys = (System_Info*) (ptaskConfig->pciDevice.base2);
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


#include "drvCLTU.h"
#include "ioctl_cltu.h"
#include "globalDef.h"



#define USE_INTLOCK   0

#if defined(_CLTU_LINUX_KERNEL_26_)
/*	#define DEVICE_FILENAME "/dev/tss/cltu.0" */
	#define MMAP_SIZE 0x8000
	#include <sys/time.h>
#endif

#if defined(_CLTU_VXWORK_)
	#define DELAY_WAIT(time) taskDelay(time)  
	
#elif defined(_CLTU_LINUX_KERNEL_26_)
	#define DELAY_WAIT(time) epicsPrintf( " . "); sleep(time)
	
#else
	#error plaease select OS!
#endif

#define MAX_CMD_WAIT_TIME_OUT  (500)

#if defined(_CLTU_VXWORK_)
#if 1 /* copyed from "$WIN_BASE/target/h/drv/pci/pciConfigLib.h" */
#define	PCI_CFG_BASE_ADDRESS_0	0x10
#define	PCI_CFG_BASE_ADDRESS_2	0x18
#define	PCI_CFG_DEV_INT_LINE	  0x3c
#endif
#endif


#define READ8(port)   (*((volatile byte*)(port)))
#define READ16(port)  (*((volatile unsigned short int*)(port)))
#define READ32(port)  (*((volatile unsigned int*)(port)))

#define WRITE8( port, val )   (*((volatile byte*)(port))=((byte)(val)))
#define WRITE16( port, val )  (*((volatile unsigned short int*)(port))=((unsigned short int)(val)))
#define WRITE32( port, val )  (*((volatile unsigned int*)(port))=((unsigned int)(val)))

#define CMD_WAIT_TIME_OUT (5)

#define OFFSET_32BIT_REG (4)

#define PCI_INFO_MEM_BASEADDR_OFFSET ( 0x0000 )
#define PCI_INFO_MEM_HIGHADDR_OFFSET ( 0x07FF )
#define PCI_INFO_MEM_SIZE            ( PCI_INFO_MEM_HIGHADDR_OFFSET - PCI_INFO_MEM_BASEADDR_OFFSET + 1)

#define PCI_BASE2PMC_BASEADDR_OFFSET ( 0x0800 )
#define PCI_BASE2PMC_HIGHADDR_OFFSET ( 0x0BFF )
#define PCI_BASE2PMC_SIZE            ( PCI_BASE2PMC_HIGHADDR_OFFSET - PCI_BASE2PMC_BASEADDR_OFFSET + 1)

#define PCI_PMC2BASE_BASEADDR_OFFSET ( 0x0C00 )
#define PCI_PMC2BASE_HIGHADDR_OFFSET ( 0x0FFF )
#define PCI_PMC2BASE_SIZE            ( PCI_PMC2BASE_HIGHADDR_OFFSET - PCI_PMC2BASE_BASEADDR_OFFSET + 1)

int cltu_cmd_endian( drvCLTU_taskConfig *ptaskConfig );
int cltu_cmd_write32(drvCLTU_taskConfig *ptaskConfig,  int address, int data );
int cltu_cmd_read32( drvCLTU_taskConfig *,  int address );
int cltu_cmd_set_mode( drvCLTU_taskConfig *, int mode );
int cltu_cmd_set_shot_start( drvCLTU_taskConfig * );
int cltu_cmd_set_shot_term(drvCLTU_taskConfig *, unsigned int term_msb, unsigned int term_lsb );
int cltu_cmd_set_ct_ss(drvCLTU_taskConfig *, int portnum, int trigger_level, int clock_hz,
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1 );
int cltu_cmd_set_ct_ss_rfm(drvCLTU_taskConfig *, int portnum, int rfm_en, int rfm_clk_hz );
int cltu_cmd_set_ct_ms(drvCLTU_taskConfig *,
					int portnum, int trigger_level, 
                                    int clk0, int clk1, int clk2, int clk3, int clk4, 
									int trg0, int trg1, int trg2, int trg3, int trg4, 
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1,
                                    int msb_t2, int lsb_t2,
                                    int msb_t3, int lsb_t3,
                                    int msb_t4, int lsb_t4,
                                    int msb_t5, int lsb_t5 );
int cltu_cmd_cal_ltu(drvCLTU_taskConfig *, int ltu_num );




static void createTaskCLTU_CallFunction(const iocshArgBuf *args);

static const iocshArg createTaskCLTUArg0 = {"task_name", iocshArgString};
static const iocshArg createTaskCLTUArg1 = {"dev", iocshArgString};
static const iocshArg* const createTaskCLTUArgs[] = { &createTaskCLTUArg0 , &createTaskCLTUArg1 };
static const iocshFuncDef createTaskCltuFuncDef = { "createTaskCLTU", 2, createTaskCLTUArgs };

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
#ifdef _CLTU_VXWORK_
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
static drvCLTU_taskConfig* make_taskConfig(char * taskName, char *devf);

/*int make_taskConfig(char *taskName); */

static drvCLTU_controlThreadConfig* make_controlThreadConfig(drvCLTU_taskConfig *ptaskConfig);
static drvCLTU_controlThreadConfig* make_interruptThreadConfig(drvCLTU_taskConfig *ptaskConfig);

static void controlThreadFunc(void *param);
static void interruptThreadFunc(void *param);

#ifdef _CLTU_LINUX_KERNEL_26_
static void clearAllOzTasks(void);
#endif

STATUS cltu_set_multiClock();


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





#ifdef _CLTU_VXWORK_
/*******************************************************************************
Company   : Olzetek
Function  : cltu_isr
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
STATUS cltu_isr( )
{
	STATUS Status;

	 drvCLTU_taskConfig *ptaskConfig = (drvCLTU_taskConfig*) ellFirst(pdrvCLTU_Config->ptaskList);

#if USE_INTLOCK
	ptaskConfig->pciDevice.intLockKey = intLock ();
#else
	intDisable((int)ptaskConfig->pciDevice.int_line);
#endif
	
	ptaskConfig->pciDevice.int_handler++;

	Status = semGive(ptaskConfig->pciDevice.int_sem);
	if( Status == ERROR )
	{    
		ptaskConfig->pciDevice.int_error++;
	}
/*	intUnlock ( ptaskConfig->pciDevice.intLockKey ); */
/*	intEnable((int)ptaskConfig->pciDevice.int_line); */
/*
	if( ptaskConfig->taskStatus == TASK_FINISH ) intDisable((int)ptaskConfig->pciDevice.int_line); 
*/
	return WR_OK;
}
#endif

/*******************************************************************************
Company   : Olzetek
Function  : cltu_device_init
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
STATUS cltu_device_init( drvCLTU_taskConfig *ptaskConfig )
{
#if defined(_CLTU_VXWORK_)
  STATUS vstatus;
/*  int val; */

  /* pci device struct init */
  memset( &ptaskConfig->pciDevice, 0x00, sizeof(CLTU_PCI_DEVICE));

  /* pci find device */
  vstatus = pciFindDevice( CLTU_VENDOR_ID, CLTU_DEVICE_ID, 0, &ptaskConfig->pciDevice.BusNo, &ptaskConfig->pciDevice.DeviceNo, &ptaskConfig->pciDevice.FuncNo );
  if( vstatus == ERROR )
  {
    printf("[CLTU] Device Not Found.........ERROR!\n");
    return WR_ERROR;
  }
  else
  {
    epicsPrintf("[CLUT] Device Found.........OK!\n");
	epicsPrintf("[CLTU] from  pciFindDevice: BusNo 0x%x, DeviceNo 0x%x, FuncNo 0x%x\n", ptaskConfig->pciDevice.BusNo, ptaskConfig->pciDevice.DeviceNo, ptaskConfig->pciDevice.FuncNo);
  }

#if USE_MANUAL_FIND_PCI
	ptaskConfig->pciDevice.DeviceNo = strtoul(ptaskConfig->deviceFile, NULL, 0);
	epicsPrintf("[CLTU] from  arguments: BusNo 0x%x, DeviceNo 0x%x, FuncNo 0x%x\n", ptaskConfig->pciDevice.BusNo, ptaskConfig->pciDevice.DeviceNo, ptaskConfig->pciDevice.FuncNo);
#endif

  /* get the pci bar0 adderess */
  vstatus = pciConfigInLong( ptaskConfig->pciDevice.BusNo, ptaskConfig->pciDevice.DeviceNo, ptaskConfig->pciDevice.FuncNo, PCI_CFG_BASE_ADDRESS_0, &ptaskConfig->pciDevice.base0);
  if( vstatus == ERROR )
  {
    printf("[CLTU] Get bar0 address.........ERROR!\n");
    return WR_ERROR;
  }
  else
  {
    epicsPrintf("[CLTU] Get bar0 address.........OK!\n");
    epicsPrintf("[CLTU] bar0 address = 0x%x\n", ptaskConfig->pciDevice.base0);
  }

  /* get the pci bar2 adderess */
  vstatus = pciConfigInLong( ptaskConfig->pciDevice.BusNo, ptaskConfig->pciDevice.DeviceNo, ptaskConfig->pciDevice.FuncNo, PCI_CFG_BASE_ADDRESS_2, &ptaskConfig->pciDevice.base2);
  if( vstatus == ERROR )
  {
    printf("[CLUT] Get bar2 address.........ERROR!\n");
    return WR_ERROR;
  }
  else
  {
    epicsPrintf("[CLTU] Get bar2 address.........OK!\n");
    epicsPrintf("[CLTU] bar2 address = 0x%x\n", ptaskConfig->pciDevice.base2);
  }

  /* ptaskConfig->pciDevice init ok */
  ptaskConfig->pciDevice.init = 0xFFFFFFFF;

  /* endian control */
  cltu_cmd_endian(ptaskConfig);

  return WR_OK;
#elif defined(_CLTU_LINUX_KERNEL_26_)

/*	int result;
	char * prtdata; */

/* void *memset(void *s, int c, size_t n); */

  memset((void*) &ptaskConfig->pciDevice,(int) 0x00, sizeof(CLTU_PCI_DEVICE));

	/* Device File Create */
/*	result = mknod(DEVICE_FILENAME, S_IFCHR|S_IRWXU|S_IRWXG, (240<<8)|1);
	if( result < 0 )
	{
		perror("mknod");
	}
*/
	/* Device File Open */
	ptaskConfig->pciDevice.dev = open(ptaskConfig->deviceFile, O_RDWR|O_NDELAY );
	if( ptaskConfig->pciDevice.dev < 0)
	{
		perror("open");
		return WR_ERROR;
	}
	
	/* Memory mapping */
	ptaskConfig->pciDevice.base2 = (char *) mmap(0, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, ptaskConfig->pciDevice.dev, 0);
/*	if( ioc_debug==1) */
		printf(" ptaskConfig->pciDevice.base2 = 0x%x \n", (unsigned int)ptaskConfig->pciDevice.base2);

  /* ptaskConfig->pciDevice init ok */
  ptaskConfig->pciDevice.init = 0xFFFFFFFF;

  /* endian control */
  cltu_cmd_endian(ptaskConfig);


  return WR_OK;
#else

#error plaease select OS!

#endif

	return WR_ERROR;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_device_quit
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
#ifdef _CLTU_LINUX_KERNEL_26_
STATUS cltu_device_quit( drvCLTU_taskConfig *ptaskConfig )
{

  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  memset( &ptaskConfig->pciDevice, 0x0, sizeof(CLTU_PCI_DEVICE));
  
  /* Memory unmapping */
  munmap( ptaskConfig->pciDevice.base2, MMAP_SIZE );

  /* Close Device File */
  close( ptaskConfig->pciDevice.dev );

  return WR_OK;
}
#endif

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_endian
Author    : pianist
Parameter : .
Return    : .
Desc      :  ctl = 0x00000000 (PPC) else (x86)
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_endian( drvCLTU_taskConfig *ptaskConfig )
{
  int endian_signal, ctl;
  PCI_COM_S* msg;
  int time_out = 0;

  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  endian_signal = READ32( ptaskConfig->pciDevice.base2 + PCI_INFO_MEM_BASEADDR_OFFSET );

	if( endian_signal == ENDIAN_SIGNAL ) return WR_OK;
		
  if( endian_signal == ENDIAN_SIGNAL )
  {
    ctl = 0x00000000; /* PPC */
  }
  else
  {
    ctl = 0xFFFFFFFF; /* Intel */
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
  	printf("msg->status = %x\n",msg->status);
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = ctl;
  msg->cmd     = CLTU_CMD_ENDIAN_CTL;

  DELAY_WAIT(1);
  
  return WR_OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_write32
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_write32( drvCLTU_taskConfig *ptaskConfig, int address, int data )
{
  PCI_COM_S* msg;
  int time_out = 0;

  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = address;
  msg->status  = 0;
  msg->data    = data;
  msg->cmd     = CLTU_CMD_WRITE32;
  
  return WR_OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_write32
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_read32( drvCLTU_taskConfig *ptaskConfig, int address )
{

  PCI_COM_S* msg;
  int time_out = 0;
  
  int wait = 1;
  int time_count = 0;

  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }


  msg->address = address;
  msg->status  = 0;
  msg->data    = 0;
  msg->cmd     = CLTU_CMD_READ32;

  while( wait )
  {
    time_count++;
    if( msg->status == 0xFFFFFFFF || time_count > 1000 )
      wait = 0;
  }

  if( time_count > 10000 )
  {
    CLTU_ASSERT();     /* time out error. */
    return WR_ERROR;
  }

  printf("[CLTU] read val = 0x%x\n", msg->data);

  return msg->data;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_mode
Author    : pianist
Parameter : .
Return    : .
Desc      : synch or test mode setting.
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_mode(drvCLTU_taskConfig *ptaskConfig,  int mode )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = mode;
  msg->cmd     = CLTU_CMD_SET_MODE;
  
	return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_shot_start
Author    : pianist
Parameter : .
Return    : .
Desc      :  CTU only 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_shot_start( drvCLTU_taskConfig *ptaskConfig )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }
  
  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->cmd     = CLTU_CMD_SHOT_START;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_shot_term
Author    : pianist
Parameter : .
Return    : .
Desc      : CTU only
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_shot_term( drvCLTU_taskConfig *ptaskConfig, unsigned int term_msb, unsigned int term_lsb )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = term_msb;
  msg->arg[1]  = term_lsb;  
  msg->cmd     = CLTU_CMD_SHOT_TERM;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_ct_ss
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_ct_ss(drvCLTU_taskConfig *ptaskConfig, 
					int portnum, int trigger_level, int clock_hz,
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1 )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
/*    printf("[cltu_cmd_set_ct_ss] msg->status = 0x%x\n", msg->status ); */
/*    printf("[cltu_cmd_set_ct_ss] time_out = 0x%x\n", time_out );     */
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++; 	 
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = portnum;
  msg->arg[1]  = trigger_level;
  msg->arg[2]  = clock_hz;
  msg->arg[3]  = msb_t0;
  msg->arg[4]  = lsb_t0;
  msg->arg[5]  = msb_t1;
  msg->arg[6]  = lsb_t1;
  msg->cmd     = CLTU_CMD_SET_CT_SS;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_ct_ss_rfm
Author    : pianist
Parameter : .
Return    : .
Desc      : change clock on shot
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_ct_ss_rfm( drvCLTU_taskConfig *ptaskConfig,  int portnum, int rfm_en, int rfm_clk_hz )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = portnum;
  msg->arg[1]  = rfm_en;
  msg->arg[2]  = rfm_clk_hz;  
  msg->cmd     = CLTU_CMD_SET_CT_SS_RFM;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_write32
Author    : pianist
Parameter : .
Return    : .
Desc      : multi trigger setting  only port 4
Date      : 2007.06.22(initial create.)
			parameter (trg0 ~ trg4) added. { On => !0, Off => 0}..
*******************************************************************************/
int cltu_cmd_set_ct_ms(drvCLTU_taskConfig *ptaskConfig, 
					int portnum, int trigger_level, 
                                    int clk0, int clk1, int clk2, int clk3, int clk4,
									int trg0, int trg1, int trg2, int trg3, int trg4,
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1,
                                    int msb_t2, int lsb_t2,
                                    int msb_t3, int lsb_t3,
                                    int msb_t4, int lsb_t4,
                                    int msb_t5, int lsb_t5 )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = portnum;
  msg->arg[1]  = trigger_level;

  msg->arg[2]  = clk0;  

  msg->arg[3]  = clk1;  

  msg->arg[4]  = clk2;  

  msg->arg[5]  = clk3;  

  msg->arg[6]  = clk4;



  msg->arg[7]  = trg0;  

  msg->arg[8]  = trg1;  

  msg->arg[9]  = trg2;  

  msg->arg[10]  = trg3;  

  msg->arg[11]  = trg4;



  

  msg->arg[12]   = msb_t0;  

  msg->arg[13]   = lsb_t0;  

  msg->arg[14]   = msb_t1;  

  msg->arg[15]  = lsb_t1;  

  msg->arg[16]  = msb_t2;  

  msg->arg[17]  = lsb_t2;  

  msg->arg[18]  = msb_t3;  

  msg->arg[19]  = lsb_t3;  

  msg->arg[20]  = msb_t4;  

  msg->arg[21]  = lsb_t4;  

  msg->arg[22]  = msb_t5;  

  msg->arg[23]  = lsb_t5; 
  
  msg->cmd     = CLTU_CMD_SET_CT_MS;
  
  return WR_OK;
}



/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_ref_clk
Author    : pianist
Parameter : ref_clk_sel : 0xffffffff -> external mode  or 0x0 -> internal mode
Return    : .
Desc      : only CTU, external 50M clock 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_ref_clk( drvCLTU_taskConfig *ptaskConfig,  int ref_clk_sel )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = ref_clk_sel;
  msg->cmd     = CLTU_REF_CLK_SEL;
  
  return WR_OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_cal_ltu
Author    : pianist
Parameter : arg(ltu_num) : 2^7 integer number  ,    8-on : run for CTU
Return    : .
Desc      : CTU only , 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_cal_ltu( drvCLTU_taskConfig *ptaskConfig,  int ltu_num )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (ptaskConfig->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = ltu_num;
  msg->cmd     = CLTU_CAL_LTU;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : check_ctu
Author    : pianist
Parameter : .
Return    : TRUE -> CTU else LTU
Desc      : 
Date      : 2007.06.22(initial create.)
			2007.11.21( remove read32.... )
*******************************************************************************/
int check_ctu( drvCLTU_taskConfig *ptaskConfig )
{
	int sw_id;
	System_Info* mSys;
	mSys = (System_Info*) (ptaskConfig->pciDevice.base2);
	
	sw_id = mSys->ltu.id;
/*	sw_id = cltu_cmd_read32( 0x71200000 + ( OFFSET_32BIT_REG * 0)); */

	return (0x00000080 & sw_id );
}


/*******************************************************************************
Company   : Olzetek
Function  : nfrc_mon_update_show
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_pmc_info_show( drvCLTU_taskConfig *ptaskConfig )
{
  
  int i;
  System_Info* mSys;
  

  if(ptaskConfig->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  mSys = (System_Info*) (ptaskConfig->pciDevice.base2);
  
  printf("\n=======================================================\n");
  
  printf("mSys->endian_signal               : %d\n", mSys->endian_signal);  
  printf("mSys->ver_info.board              : %d\n", mSys->ver_info.board);
  printf("mSys->ver_info.fpga               : %d\n", mSys->ver_info.fpga);
  printf("mSys->ver_info.sw                 : %d\n\n", mSys->ver_info.sw);
  printf("mSys->single_update               : %d\n", mSys->single_update);
  printf("mSys->single_update_arm           : %d\n\n", mSys->single_update_arm);
  printf("mSys->log_status                  : 0x%x\n", mSys->log_status);
#ifdef FEATURE_HEADER_64_TO_32
  printf("mSys->ShotTerm_msb                : 0x%x\n", mSys->ShotTerm_msb);  
  printf("mSys->ShotTerm_lsb                : 0x%x\n", mSys->ShotTerm_lsb);  
  
  printf("mSys->CurrentTime_msb             : 0x%x\n", mSys->CurrentTime_msb);
  printf("mSys->CurrentTime_lsb             : 0x%x\n", mSys->CurrentTime_lsb);
  
  printf("mSys->ShotStartTime_msb           : 0x%x\n", mSys->ShotStartTime_msb);
  printf("mSys->ShotStartTime_lsb           : 0x%x\n", mSys->ShotStartTime_lsb);
  
  printf("mSys->ShotStopTime_msb            : 0x%x\n", mSys->ShotStopTime_msb);
  printf("mSys->ShotStopTime_lsb            : 0x%x\n", mSys->ShotStopTime_lsb);  
#else
  printf("mSys->ShotTerm                    : 0x%llx\n", mSys->ShotTerm);  
  printf("mSys->CurrentTime                 : 0x%llx\n", mSys->CurrentTime);
  printf("mSys->ShotStartTime               : 0x%llx\n", mSys->ShotStartTime);
  printf("mSys->ShotStopTime                : 0x%llx\n", mSys->ShotStopTime);
#endif  
  printf("mSys->op_mode                     : 0x%x\n", mSys->op_mode);

  printf("\n");
  printf("mSys->serdes.tx_only              : 0x%x\n", mSys->serdes.tx_only);
  printf("mSys->serdes.rx_link_status       : 0x%x\n", mSys->serdes.rx_link_status);
  printf("mSys->serdes.good_cnt             : %d\n", mSys->serdes.good_cnt);
  printf("mSys->serdes.error_cnt            : %d\n", mSys->serdes.error_cnt);



  printf("\n");
  printf("mSys->ctu.irigb_status            : 0x%x\n", mSys->ctu.irigb_status);
  printf("mSys->ctu.ref_clk_source          : 0x%x\n", mSys->ctu.ref_clk_source);
  printf("mSys->ctu.gps_locked              : 0x%x\n", mSys->ctu.gps_locked);
  printf("mSys->ctu.dcm_locked              : 0x%x\n", mSys->ctu.dcm_locked); /* OPI: main ctu M.Clock */
  printf("mSys->ctu.rx_linked_ltu           : 0x%x\n", mSys->ctu.rx_linked_ltu);

  printf("\n");
  printf("mSys->ltu.id                      : 0x%x\n", mSys->ltu.id);  /* LTU ID number : 0 ~ 2^7  */
  printf("mSys->ltu.ref_clk_source          : 0x%x\n", mSys->ltu.ref_clk_source);
  printf("mSys->ltu.dcm_locked              : 0x%x\n", mSys->ltu.dcm_locked);
  printf("mSys->ltu.compensation_offset1    : 0x%x\n", mSys->ltu.compensation_offset1); /* unit; 10 ns */
  printf("mSys->ltu.compensation_offset2    : 0x%x\n", mSys->ltu.compensation_offset2); /* not used */

  printf("\n");
  for( i = 0; i < 4; i++ )
  {
  	printf("mSys->ltu.cts[%d].trigger_level    : 0x%x\n",i, mSys->ltu.cts[i].trigger_level);
  	printf("mSys->ltu.cts[%d].clock            : %d\n",i, mSys->ltu.cts[i].clock);
#ifdef FEATURE_HEADER_64_TO_32
  	printf("mSys->ltu.cts[%d].start_offset_msb : 0x%x\n",i, mSys->ltu.cts[i].start_offset_msb);
  	printf("mSys->ltu.cts[%d].start_offset_lsb : 0x%x\n",i, mSys->ltu.cts[i].start_offset_lsb);
    
  	printf("mSys->ltu.cts[%d].stop_offset_msb  : 0x%x\n",i, mSys->ltu.cts[i].stop_offset_msb);
   	printf("mSys->ltu.cts[%d].stop_offset_lsb  : 0x%x\n",i, mSys->ltu.cts[i].stop_offset_lsb);
#else    
  	printf("mSys->ltu.cts[%d].start_offset     : 0x%llx\n",i, mSys->ltu.cts[i].start_offset);
  	printf("mSys->ltu.cts[%d].stop_offset      : 0x%llx\n",i, mSys->ltu.cts[i].stop_offset);
#endif    
  	printf("mSys->ltu.cts[%d].rfmen            : 0x%x\n",i, mSys->ltu.cts[i].rfmen);
  	printf("mSys->ltu.cts[%d].rfmclock         : %d\n",i, mSys->ltu.cts[i].rfmclock);
  }

  printf("\n");
  printf("mSys->ltu.ctm.trigger_level       : 0x%x\n", mSys->ltu.ctm.trigger_level);
  printf("mSys->ltu.ctm.clock1              : %d\n", mSys->ltu.ctm.clock1);
  printf("mSys->ltu.ctm.clock2              : %d\n", mSys->ltu.ctm.clock2);
  printf("mSys->ltu.ctm.clock3              : %d\n", mSys->ltu.ctm.clock3);
  printf("mSys->ltu.ctm.clock4              : %d\n", mSys->ltu.ctm.clock4);
  printf("mSys->ltu.ctm.clock5              : %d\n", mSys->ltu.ctm.clock5);
#ifdef FEATURE_HEADER_64_TO_32
  printf("mSys->ltu.ctm.offset1_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset1_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset2_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset2_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset3_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset3_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset4_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset4_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset5_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset5_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset6_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset6_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);  
#else  
  printf("mSys->ltu.ctm.offset1             : 0x%llx\n", mSys->ltu.ctm.offset1);
  printf("mSys->ltu.ctm.offset2             : 0x%llx\n", mSys->ltu.ctm.offset2);
  printf("mSys->ltu.ctm.offset3             : 0x%llx\n", mSys->ltu.ctm.offset3);
  printf("mSys->ltu.ctm.offset4             : 0x%llx\n", mSys->ltu.ctm.offset4);
  printf("mSys->ltu.ctm.offset5             : 0x%llx\n", mSys->ltu.ctm.offset5);
  printf("mSys->ltu.ctm.offset6             : 0x%llx\n", mSys->ltu.ctm.offset6);
#endif  

  printf("\n=======================================================\n");
  
  return WR_OK;
}



/*******************************************************************************
Company   : Olzetek
Function  : sec_to_10ns_64bit_value
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int sec_to_10ns_64bit_value( unsigned int sec, unsigned int* msbT, unsigned int* lsbT)
{
  unsigned long long  int time64;

  time64 = sec * 100000000;

  *msbT = (unsigned int) (time64>>32);
  *lsbT = (unsigned int) time64;
  
  return WR_OK;
}











static drvCLTU_taskConfig* make_taskConfig(char * taskName, char* devf)
/* int make_taskConfig(char *taskName) */
{
/*	char task_name[60]; */
	drvCLTU_taskConfig *ptaskConfig = NULL;
	int i;

	ptaskConfig = (drvCLTU_taskConfig*) malloc(sizeof(drvCLTU_taskConfig));
	if(!ptaskConfig) return ptaskConfig;


	strcpy(ptaskConfig->taskName, taskName);
	strcpy(ptaskConfig->deviceFile, devf );
	ptaskConfig->taskLock = epicsMutexCreate();
	ptaskConfig->bufferLock = epicsMutexCreate();
	scanIoInit(&ptaskConfig->ioScanPvt);

	ptaskConfig->pcontrolThreadConfig = NULL;
	ptaskConfig->pinterruptThreadConfig = NULL;

	ptaskConfig->nShotTerm_msb = 0x0;
	ptaskConfig->nShotTerm_lsb = 0x11E1A300;
	

	for(i=0; i< 4; i++) {
		ptaskConfig->sPortConfig[i].nTrigMode = 0xffffffff;
		ptaskConfig->sPortConfig[i].nClock = 100000;
		
		ptaskConfig->sPortConfig[i].nT0_msb = 0x0;
		ptaskConfig->sPortConfig[i].nT0_lsb = 0x5f5e100;
		
		ptaskConfig->sPortConfig[i].nT1_msb = 0x0;
		ptaskConfig->sPortConfig[i].nT1_lsb = 0xbebc200;
		ptaskConfig->curPortConfigStatus[i] = PORT_NOT_INIT;
	}

	for(i=0; i<3; i++) {
		ptaskConfig->mPortConfig[i].nTrigMode = 0xffffffff;
		ptaskConfig->mPortConfig[i].nClock = 100000;
		
		ptaskConfig->mPortConfig[i].nT0_msb = 0x0;
		ptaskConfig->mPortConfig[i].nT0_lsb = 0x5f5e100;
		
		ptaskConfig->mPortConfig[i].nT1_msb = 0x0;
		ptaskConfig->mPortConfig[i].nT1_lsb = 0xbebc200;
	}
	ptaskConfig->curPortConfigStatus[4] = PORT_NOT_INIT; /* this is last multi-trig port */

	if( getenv("CLTU_IOC_DEBUG") ){
		ioc_debug = atoi(getenv("CLTU_IOC_DEBUG") );
		epicsPrintf("CLTU_IOC_DEBUG : %d\n", ioc_debug);
	} else ioc_debug = 0;

	
#if defined(_CLTU_VXWORK_)
	ptaskConfig->pciDevice.intLockKey = 0;
	ptaskConfig->pciDevice.int_count = 0;
	ptaskConfig->pciDevice.int_handler = 0;
	ptaskConfig->pciDevice.int_error = 0;

	/* it just for DDS1, two PCM card. */
	ptaskConfig->pciDevice.BusNo = PCI_BUS_NO;
	ptaskConfig->pciDevice.DeviceNo = PCI_DEVICE_NO;
	ptaskConfig->pciDevice.FuncNo = PCI_FUNC_NO;		
#endif

	ptaskConfig->calib_target_ltu = 0;

	ptaskConfig->taskStatus = TASK_NOT_INIT;

	return ptaskConfig;

}

static drvCLTU_controlThreadConfig*  make_controlThreadConfig(drvCLTU_taskConfig *ptaskConfig)
{
	drvCLTU_controlThreadConfig *pcontrolThreadConfig  = NULL;

	pcontrolThreadConfig  = (drvCLTU_controlThreadConfig*)malloc(sizeof(drvCLTU_controlThreadConfig));
	if(!pcontrolThreadConfig) return pcontrolThreadConfig; 

	sprintf(pcontrolThreadConfig->threadName, "%s_ctrl", ptaskConfig->taskName);
	
	pcontrolThreadConfig->threadPriority = epicsThreadPriorityHigh;
	pcontrolThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pcontrolThreadConfig->threadFunc      = (EPICSTHREADFUNC) controlThreadFunc;
	pcontrolThreadConfig->threadParam     = (void*) ptaskConfig;

	pcontrolThreadConfig->threadQueueId   = epicsMessageQueueCreate(100,sizeof(controlThreadQueueData));

	epicsThreadCreate(pcontrolThreadConfig->threadName,
			  pcontrolThreadConfig->threadPriority,
			  pcontrolThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pcontrolThreadConfig->threadFunc,
			  (void*) pcontrolThreadConfig->threadParam);


	return pcontrolThreadConfig;
}

static drvCLTU_controlThreadConfig*  make_interruptThreadConfig(drvCLTU_taskConfig *ptaskConfig)
{
	int vstatus;
	
	drvCLTU_controlThreadConfig *pinterruptThreadConfig  = NULL;

	pinterruptThreadConfig  = (drvCLTU_controlThreadConfig*)malloc(sizeof(drvCLTU_controlThreadConfig));
	if(!pinterruptThreadConfig) return pinterruptThreadConfig; 

	sprintf(pinterruptThreadConfig->threadName, "%s_interrupt", ptaskConfig->taskName);
	
	pinterruptThreadConfig->threadPriority = epicsThreadPriorityHigh;
	pinterruptThreadConfig->threadStackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
	pinterruptThreadConfig->threadFunc      = (EPICSTHREADFUNC) interruptThreadFunc;
	pinterruptThreadConfig->threadParam     = (void*) ptaskConfig;

	pinterruptThreadConfig->threadQueueId   = 0; /* we don't need this */

	vstatus = WR_OK;
	
#if defined(_CLTU_LINUX_KERNEL_26_)
/* this is for linux kernel  */
	epicsThreadCreate(pinterruptThreadConfig->threadName,
			  pinterruptThreadConfig->threadPriority,
			  pinterruptThreadConfig->threadStackSize,
			  (EPICSTHREADFUNC) pinterruptThreadConfig->threadFunc,
			  (void*) pinterruptThreadConfig->threadParam);

#elif defined(_CLTU_VXWORK_)
	 /* get the pci int line */
	vstatus = pciConfigInByte( ptaskConfig->pciDevice.BusNo, ptaskConfig->pciDevice.DeviceNo, ptaskConfig->pciDevice.FuncNo, PCI_CFG_DEV_INT_LINE, &ptaskConfig->pciDevice.int_line);
	if( vstatus == ERROR )
	{
		epicsPrintf("[CLTU] Get pci int line .........ERROR!\n");
		return NULL;
	}
	else
	{
		epicsPrintf("[CLUT] Get pci int line .........OK!\n");
		epicsPrintf("[CLTU] int line = 0x%x\n", ptaskConfig->pciDevice.int_line);
	}

	ptaskConfig->pciDevice.int_sem = semBCreate( SEM_Q_FIFO, SEM_EMPTY );
	if( ptaskConfig->pciDevice.int_sem == NULL )
	{
		epicsPrintf("[CLTU] semBCreate ERROR!\n");
	}


	ptaskConfig->tPCI = taskSpawn(CLTU_PCI_TASK_NAME, CLTU_PCI_TASK_PRI, 8, 100000, (FUNCPTR)interruptThreadFunc,
	      (int) pinterruptThreadConfig->threadParam, 0, 0, 0, 0, 0, 0, 0, 0, 0);  

#if USE_INTLOCK
	intLockLevelSet(1);
	vstatus = intLockLevelGet();
	epicsPrintf("\n>>> make_interruptThreadConfig : intLockLevelGet() : %d\n", vstatus );
#endif

	vstatus = intConnect((VOIDFUNCPTR *)(ptaskConfig->pciDevice.int_line), (VOIDFUNCPTR)cltu_isr, (int)NULL);
	if( vstatus == ERROR ) {
		epicsPrintf("[CLTU] intConnect .........ERROR!\n");
		return NULL;
	} else {
		epicsPrintf("[CLTU] intConnect.........OK!\n");
	}

/*	vstatus = READ32(ptaskConfig->pciDevice.base0+0x68);
	if( ioc_debug==1) epicsPrintf("[CLTU] default INTCSR value = 0x%x\n", vstatus );

	vstatus = READ32(ptaskConfig->pciDevice.base0+0x68);
	if( ioc_debug==1) epicsPrintf("[CLTU] default INTCSR value = 0x%x\n", vstatus ); 
*/
#endif

	return pinterruptThreadConfig;
}

static void controlThreadFunc(void *param)
{

	drvCLTU_taskConfig *ptaskConfig = (drvCLTU_taskConfig*) param;
	drvCLTU_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData		queueData;

	while( !ptaskConfig->pcontrolThreadConfig ) epicsThreadSleep(.1);
	pcontrolThreadConfig = (drvCLTU_controlThreadConfig*) ptaskConfig->pcontrolThreadConfig;


/*	if( ioc_debug==1) */
	epicsPrintf("task launching: %s thread for %s task\n",pcontrolThreadConfig->threadName, ptaskConfig->taskName);

	while(TRUE) {
		EXECFUNCQUEUE            pFunc;
		execParam                *pexecParam;
		struct dbCommon          *precord;
		struct rset              *prset;

		drvCLTU_taskConfig    *ptaskConfig;

		epicsMessageQueueReceive(pcontrolThreadConfig->threadQueueId,
				         (void*) &queueData,
					 sizeof(controlThreadQueueData));

		pFunc      = queueData.pFunc;
	        pexecParam = &queueData.param;	
		precord    = (struct dbCommon*) pexecParam->precord;
		prset      = (struct rset*) precord->rset;
		ptaskConfig = (drvCLTU_taskConfig *) pexecParam->ptaskConfig;

		if(!pFunc) continue;
		else pFunc(pexecParam);

		if(!precord) continue;


		dbScanLock(precord);
		(*prset->process)(precord); 
		dbScanUnlock(precord);
	}


	return;
}


static void interruptThreadFunc(void *param)
{
	int rval;
	char buff[8];
	drvCLTU_taskConfig *ptaskConfig = (drvCLTU_taskConfig*) param;

	strcpy(buff,"");
	rval=0;

	while( !ptaskConfig ) epicsThreadSleep(.1);

	epicsPrintf("task launching: Interrupt thread for %s task\n", ptaskConfig->taskName);

#ifdef _CLTU_LINUX_KERNEL_26_
/*	state = ioctl( ptaskConfig->pciDevice.dev, IOCTL_CLTU_INTERRUPT_ENABLE, &rval); */
#endif

	while(TRUE)
	{
#ifdef _CLTU_LINUX_KERNEL_26_
/*		read( ptaskConfig->pciDevice.dev, buff, 1); */

		epicsThreadSleep(1.);
		
		scanIoRequest(ptaskConfig->ioScanPvt);
#elif defined(_CLTU_VXWORK_)
/*
		state = semTake( ptaskConfig->pciDevice.int_sem, WAIT_FOREVER );
		if( state == ERROR )
		{
			epicsPrintf("[PCI] semTake ERROR!\n");      
			return;
		}

		ptaskConfig->pciDevice.int_count++;
		if( ioc_debug==3) {
			epicsPrintf("[PCI] int_count=%d, int_handler=%d\n",ptaskConfig->pciDevice.int_count, ptaskConfig->pciDevice.int_handler);
			epicsPrintf("[PCI] int_error = %d!\n",ptaskConfig->pciDevice.int_error);
		}

		
#if USE_INTLOCK
		intUnlock ( ptaskConfig->pciDevice.intLockKey );
#else
		intEnable((int)ptaskConfig->pciDevice.int_line);
#endif
*/
		epicsThreadSleep(1.);

		scanIoRequest(ptaskConfig->ioScanPvt);
#endif

		if( ptaskConfig->taskStatus == TASK_FINISH ) break;

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
	drvCLTU_taskConfig *ptaskConfig = NULL;
	char task_name[60];
	char devf[60];


#ifdef _CLTU_VXWORK_ 
		sysClkRateSet(1000);
#endif


	if(args[0].sval) strcpy(task_name, args[0].sval);
	if(args[1].sval) strcpy(devf, args[1].sval);
/*	if(args[1].sval) devf = strtoul(args[1].sval,NULL,0); */

/*	printf("device file: %s\n", devf ); */


	if(make_drvConfig()) return;

/*	epicsPrintf("1\n");
	epicsThreadSleep(1.0);  */

	ptaskConfig = make_taskConfig(task_name, devf);
	if(!ptaskConfig) return;

	epicsPrintf("make task config ok ... ptaskConfig(%p)\n", ptaskConfig);

	ptaskConfig->pdrvConfig = pdrvCLTU_Config;

	ptaskConfig->pcontrolThreadConfig = make_controlThreadConfig(ptaskConfig);
	if(!ptaskConfig->pcontrolThreadConfig) return;

	epicsPrintf("make control thread ok...pcontrolThreadConfig: %p\n", ptaskConfig->pcontrolThreadConfig);
	
#ifdef _CLTU_VXWORK_ 
	if( cltu_device_init(ptaskConfig) == WR_ERROR ) {
		epicsPrintf("\n>>> drvCLTU_init_driver : cltu_device_init() ... error!\n" );
		return ;
	}
#endif

	/* must do after cltu_driver initialized */
	ptaskConfig->pinterruptThreadConfig = make_interruptThreadConfig(ptaskConfig);
	if(!ptaskConfig->pinterruptThreadConfig) return ;
	

	ellAdd(pdrvCLTU_Config->ptaskList, &ptaskConfig->node);

	return;
}



LOCAL long drvCLTU_init_driver(void)
{
	unsigned long long int a;

	drvCLTU_taskConfig *ptaskConfig = NULL;

	if(!pdrvCLTU_Config)  return 0;

	ptaskConfig = (drvCLTU_taskConfig*) ellFirst(pdrvCLTU_Config->ptaskList);
	while(ptaskConfig) 
	{
#ifdef _CLTU_LINUX_KERNEL_26_	
		if( cltu_device_init(ptaskConfig) == WR_ERROR ) {
			epicsPrintf("\n>>> drvCLTU_init_driver : cltu_device_init() ... error!\n" );
			return -1;
		}
#endif

#if PRINT_PMC_INFO
		cltu_pmc_info_show(ptaskConfig);
#endif

		ptaskConfig = (drvCLTU_taskConfig*) ellNext(&ptaskConfig->node);
	} /* while(ptaskConfig) { */

#ifdef _CLTU_LINUX_KERNEL_26_
	epicsAtExit((EPICSEXITFUNC) clearAllOzTasks, NULL); 
/*	
	ptaskConfig->fd_rtc = open ("/dev/rtc", O_RDONLY);
	if (ptaskConfig->fd_rtc ==  -1) { 
		perror("/dev/rtc"); 
		return 0;
	}
*/
#endif

	a = drvCLTU_getCurrentTick();
	epicsThreadSleep(1.);
	pdrvCLTU_Config->one_sec_interval = drvCLTU_getCurrentTick() - a;

	epicsPrintf("drvCLTU: measured one second time is %f sec\n", 1.E-3 * (double)pdrvCLTU_Config->one_sec_interval); 


		
#ifdef _CLTU_VXWORK_ 
	/* interrupt enable  */
/*	intEnable((int)ptaskConfig->pciDevice.int_line);
	WRITE32(ptaskConfig->pciDevice.base0 + 0x68, 0x9010f); */
#endif

	return 0;
}

#ifdef _CLTU_LINUX_KERNEL_26_
static void clearAllOzTasks(void)
{
	drvCLTU_taskConfig *ptaskConfig;

	ptaskConfig = (drvCLTU_taskConfig*) ellFirst (pdrvCLTU_Config->ptaskList);

	while(ptaskConfig) {
		/*	close(ptaskConfig->fd_rtc); */
		
		ptaskConfig->taskStatus = TASK_FINISH;
	
		if( cltu_device_quit(ptaskConfig) == WR_ERROR  )
			epicsPrintf("\n>>> clearAllOzTasks : cltu_device_quit() ... error!\n");

		ptaskConfig = (drvCLTU_taskConfig*) ellNext(&ptaskConfig->node);
	}
	
	if( ioc_debug==1) epicsPrintf("epics release... ok\n");
	return;
}
#endif

LOCAL long drvCLTU_io_report(int level)
{
	drvCLTU_taskConfig *ptaskConfig;

	ptaskConfig = (drvCLTU_taskConfig*) ellFirst (pdrvCLTU_Config->ptaskList);

	if(!pdrvCLTU_Config) return 0;
	
	if(ellCount(pdrvCLTU_Config->ptaskList))
		ptaskConfig = (drvCLTU_taskConfig*) ellFirst (pdrvCLTU_Config->ptaskList);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

	epicsPrintf("Totoal %d task(s) found\n",ellCount(pdrvCLTU_Config->ptaskList));

	if(level<1) return 0;

	while(ptaskConfig) {
		
		epicsPrintf("  Task name: %s, status: 0x%x\n", ptaskConfig->taskName, ptaskConfig->taskStatus	);
		if(level>2) {
			epicsPrintf("   Sampling Rate: /sec\n" );
		}
		
		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
		}
		
		ptaskConfig = (drvCLTU_taskConfig*) ellNext(&ptaskConfig->node);
	}
	
	return 0;
}

drvCLTU_taskConfig* drvCLTU_find_taskConfig(char *taskName)
{
	drvCLTU_taskConfig *ptaskConfig = NULL;
	char task_name[60];

	strcpy(task_name, taskName);

	ptaskConfig = (drvCLTU_taskConfig*) ellFirst(pdrvCLTU_Config->ptaskList);
	while(ptaskConfig) {
		if(!strcmp(ptaskConfig->taskName, task_name)) return ptaskConfig;
  		ptaskConfig = (drvCLTU_taskConfig*) ellNext(&ptaskConfig->node);
	}

	return ptaskConfig;
}
/*
drvCLTU_taskConfig* drvCLTU_get_taskConfig()
{
	return ptaskConfig;
}
*/
STATUS drvCLTU_set_shotStart (drvCLTU_taskConfig *ptaskConfig )
{
/*
	if( !check_ctu() ) {
  		epicsPrintf("\n>>> I'm LTU , calibration run only CTU\n");
		return WR_ERROR;
	}
*/
	if ( cltu_cmd_set_shot_start( ptaskConfig ) == WR_ERROR ) {
		epicsPrintf("\n>>> drvCLTU_set_shotStart : cltu_cmd_set_shot_start() ... error!\n" );
		return WR_ERROR;
	}
	if( ioc_debug==1)
		epicsPrintf("\n>>> drvCLTU_set_shotStart : cltu_cmd_set_shot_start() ... OK!\n" );
	
	return WR_OK;
}
/* void cltu_pmc_info_show( void ) */

STATUS drvCLTU_set_portConfig (drvCLTU_taskConfig *ptaskConfig , int nPort)
{
	if( nPort == 4 ) {
		if( cltu_set_multiClock(ptaskConfig) == WR_ERROR ){
			epicsPrintf("\n>>> drvCLTU_set_portConfig (%d): cltu_set_multiTrig() ... error! \n", nPort);
			return WR_ERROR;
		}
		return WR_OK;
	}		
	if( nPort > 4 || nPort < 0 ) {
		epicsPrintf("\n>>> drvCLTU_set_portConfig (%d): Port must be 0~4\n", nPort);
		return WR_ERROR;
	}
	
	if ( (ptaskConfig->curPortConfigStatus[nPort] & PORT_TRIG_OK) &&
			(ptaskConfig->curPortConfigStatus[nPort] & PORT_CLOCK_OK) &&
			(ptaskConfig->curPortConfigStatus[nPort] & PORT_T0_OK) &&
			(ptaskConfig->curPortConfigStatus[nPort] & PORT_T1_OK) ) 
	{
		if ( cltu_cmd_set_ct_ss( ptaskConfig, nPort, ptaskConfig->sPortConfig[nPort].nTrigMode, ptaskConfig->sPortConfig[nPort].nClock,
								ptaskConfig->sPortConfig[nPort].nT0_msb, ptaskConfig->sPortConfig[nPort].nT0_lsb, 
								ptaskConfig->sPortConfig[nPort].nT1_msb, ptaskConfig->sPortConfig[nPort].nT1_lsb ) == ERROR ) {
			epicsPrintf("\n>>> drvCLTU_set_portConfig : cltu_cmd_set_ct_ss(port:%d) ... error!\n", nPort );
			return WR_ERROR;
		}
	} else {
		epicsPrintf("\n>>> drvCLTU_set_portConfig (%d): All port config must be setted!\n", nPort);
		return WR_ERROR;
	}

	if( ioc_debug == 1) epicsPrintf("\n>>> drvCLTU_set_portConfig : cltu_cmd_set_ct_ss(port:%d) ... OK!\n", nPort );
/*
	epicsPrintf("clock: %d, t0_msb: 0x%x, t0_lsb: 0x%x\n", ptaskConfig->portConfig[nPort].nClock,
												ptaskConfig->portConfig[nPort].nT0_msb, 
												ptaskConfig->portConfig[nPort].nT0_lsb );
*/
	return WR_OK;
}

STATUS drvCLTU_set_shotTerm(drvCLTU_taskConfig *ptaskConfig )
{
	if ( cltu_cmd_set_shot_term( ptaskConfig, ptaskConfig->nShotTerm_msb, ptaskConfig->nShotTerm_lsb ) == WR_ERROR ) {
		epicsPrintf("\n>>> drvCLTU_set_shotTerm : cltu_cmd_set_shot_term() ... error!\n" );
		return WR_ERROR;
	} 

	if( ioc_debug==1) epicsPrintf("\n>>> drvCLTU_set_shotTerm : cltu_cmd_set_shot_term() ... OK!\n" );
	
	return WR_OK;
}

STATUS drvCLTU_set_clockOnShot (drvCLTU_taskConfig *ptaskConfig , int nPort, int onoff)
{
	if( cltu_cmd_set_ct_ss_rfm ( ptaskConfig,  nPort, onoff, ptaskConfig->sPortConfig[nPort].nClock) == WR_ERROR ) {
		epicsPrintf("\n>>> drvCLTU_set_changeClock : cltu_cmd_set_ct_ss_rfm(port:%d) ... error!\n", nPort );
		return WR_ERROR;
	} 
	
	if( ioc_debug==1) epicsPrintf("\n>>> drvCLTU_set_changeClock : cltu_cmd_set_ct_ss_rfm(port:%d) ... OK!\n", nPort );
		
	return WR_OK;
}

STATUS drvCLTU_set_calibration ( drvCLTU_taskConfig *ptaskConfig  )
{

  	if( !check_ctu(ptaskConfig) ) {
  		epicsPrintf("\n>>> I'm LTU , calibration run only CTU\n");
		return WR_ERROR;
	}
	if( cltu_cmd_cal_ltu( ptaskConfig, ptaskConfig->calib_target_ltu ) == WR_ERROR ) {
		epicsPrintf("\n>>> drvCLTU_set_calibration : cltu_cmd_cal_ltu(ltu:%d) ... error!\n", ptaskConfig->calib_target_ltu  );
		return WR_ERROR;
	}
	return WR_OK;	
}


/*
 0xffffffff -> external mode 
 0x0 -> internal mode
 */
STATUS drvCLTU_set_RefClock(drvCLTU_taskConfig *ptaskConfig, int ref_clk_sel)
{
	if( !check_ctu(ptaskConfig) ) {
  		epicsPrintf("\n>>> I'm LTU , It's run only CTU\n");
		return WR_ERROR;
	}
	if( cltu_cmd_set_ref_clk(ptaskConfig,  ref_clk_sel ) == WR_ERROR ){
		epicsPrintf("\n>>> drvCLTU_set_RefClock : cltu_cmd_set_ref_clk(0x%x) ... error!\n", ref_clk_sel );
		return WR_ERROR;
	}
	return WR_OK;
}


STATUS cltu_set_multiClock(drvCLTU_taskConfig *ptaskConfig )
{

	if ( (ptaskConfig->curPortConfigStatus[4] & PORT_TRIG_OK) &&
			(ptaskConfig->curPortConfigStatus[4] & PORT_CLOCK_OK) &&
			(ptaskConfig->curPortConfigStatus[4] & PORT_T0_OK) &&
			(ptaskConfig->curPortConfigStatus[4] & PORT_T1_OK) ) 
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
		if( cltu_cmd_set_ct_ms( ptaskConfig, 4, ptaskConfig->mPortConfig[0].nTrigMode, 
									ptaskConfig->mPortConfig[0].nClock, 0,
									ptaskConfig->mPortConfig[1].nClock, 0,
									ptaskConfig->mPortConfig[2].nClock,
			1, 0, 1, 0, 1,
                                    ptaskConfig->mPortConfig[0].nT0_msb, ptaskConfig->mPortConfig[0].nT0_lsb, 
                                    ptaskConfig->mPortConfig[0].nT1_msb, ptaskConfig->mPortConfig[0].nT1_lsb,
                                    ptaskConfig->mPortConfig[1].nT0_msb, ptaskConfig->mPortConfig[1].nT0_lsb,
                                    ptaskConfig->mPortConfig[1].nT1_msb, ptaskConfig->mPortConfig[1].nT1_lsb,
                                    ptaskConfig->mPortConfig[2].nT0_msb, ptaskConfig->mPortConfig[2].nT0_lsb,
                                    ptaskConfig->mPortConfig[2].nT1_msb, ptaskConfig->mPortConfig[2].nT1_lsb ) == WR_ERROR ) {
			epicsPrintf("\n>>> cltu_set_multiClock : cltu_cmd_set_ct_ms(port:4) ... error!\n");
			return WR_ERROR;
		} 
		
	} else {
		epicsPrintf("\n>>> cltu_set_multiClock(): All multi-trig section must be setted!\n");
		return WR_ERROR;
	}

	if( ioc_debug==1) epicsPrintf("\n>>> cltu_set_multiClock : cltu_cmd_set_ct_ms(port:4) ... OK!\n");

	return WR_OK;
}

/* manualy put calibrated offset value for LTU timing board */
STATUS drvCLTU_put_calibratedVal(drvCLTU_taskConfig *ptaskConfig , int calibVal)
{
	if( cltu_cmd_write32(ptaskConfig,  0x70000000, calibVal ) == WR_ERROR ) {
		epicsPrintf("\n>>> cltu_cmd_write32( 0x70000000, %d)... error!\n", calibVal);
		return WR_ERROR;
	}

	return WR_OK;
}

STATUS drvCLTU_set_rtc(drvCLTU_taskConfig *ptaskConfig , unsigned int timer_sec)
{
#if defined(_CLTU_LINUX_KERNEL_26_)
	int retval; 
	struct rtc_time rtc_tm;
	unsigned int day, hour, min, sec;
	

/*	epicsPrintf("curr raw GPS time (%ld)\n", timer_sec); */
	
	day = (int) (timer_sec/T_DAY_DIVIDER);
	timer_sec = timer_sec - (day * T_DAY_DIVIDER);
	hour = (int) (timer_sec/T_HOUR_DIVIDER );
	timer_sec = timer_sec - (hour * T_HOUR_DIVIDER);
	min = (int) (timer_sec/T_MIN_DIVIDER );
	timer_sec = timer_sec - (min * T_MIN_DIVIDER);
	sec = (int) (timer_sec/T_SEC_DIVIDER );

	if( ioc_debug == 3) epicsPrintf("curr GPS time: %d, %d: %d: %d\n", day, hour, min, sec);

/*	fprintf(stderr, " Current RTC date/time is %d-%d-%d, %d:%d:%d. \n",
			rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
			rtc_tm.tm_hour+9, rtc_tm.tm_min, rtc_tm.tm_sec);
*/

	/* Read the RTC time/date */ 
	retval = ioctl(ptaskConfig->fd_rtc, RTC_RD_TIME, &rtc_tm); 
	if (retval == -1) { 
		epicsPrintf("\n>>> drvCLTU_set_rtc(): ioctl( get rtc )"); 
		return WR_ERROR;
	}
	rtc_tm.tm_hour = hour;
	rtc_tm.tm_min = min;
	rtc_tm.tm_sec = sec;

	retval = ioctl(ptaskConfig->fd_rtc, RTC_SET_TIME, &rtc_tm); 
	if (retval == -1) { 
		epicsPrintf("\n>>> drvCLTU_set_rtc(): ioctl( get rtc )"); 
		return WR_ERROR;
	}

#if 0
	/* Read the RTC time/date */ 
	retval = ioctl(ptaskConfig->fd_rtc, RTC_RD_TIME, &rtc_tm); 
	if (retval == -1) { 
		epicsPrintf("\n>>> drvCLTU_set_rtc(): ioctl( get rtc )"); 
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
		
		epicsPrintf("curr SYS time:y%d, d%d, %d: %d: %d\n", year, day, hour, min, sec);
	
		
		printf("(secOfyear: %ld)\n" , secOfyear);
		systime.tv_sec = secOfyear * (2007-1970) + timer_sec;
		systime.tv_usec = 0;
		
		printf("(%ld: %ld)\n" , systime.tv_sec, systime.tv_usec);
	
		retval = settimeofday(&systime, NULL);
		if( retval != 0 )
			epicsPrintf("settimeofday() failed!\n");
	}
#endif

#elif defined(_CLTU_VXWORK_)

#endif

	return WR_OK;
}

/*

cltu_cmd_write32 0x77200000, 0x

cltu_cmd_set_ct_ss 0,0xffffffff, 250000, 0,0,1,0
                                    
*/


