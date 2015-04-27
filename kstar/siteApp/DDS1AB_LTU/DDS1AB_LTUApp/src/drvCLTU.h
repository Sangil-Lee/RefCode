/******************************************************************************
 *  Copyright (c) 2006 ~ by NFRI. All Rights Reserved.                     *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE


2007. 10. 16. 
Add debug condition.   get state from envronment....
int ioc_debug



2007. 10. 23
-	CLTU_PORT_CONFIG portConfig[4];
+	CLTU_PORT_CONFIG portConfig[5];



2007. 10. 25
change working flow for port config....



2007. 11. 15
+ drvCLTU_put_calibratedVal()


2007. 11. 21
add set RTC timer

2007. 11. 27
+ #include <drv/pci/pciConfigShow.h>

2007. 11. 29
+  unsigned int int_handler;

2007. 11. 30
+  int			intLockKey;

2009. 10. 09
+ #include <errlog.h>  for 3.14.11


*/


#ifndef __drvCLTU_H__
#define __drvCLTU_H__

#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include <stddef.h>

#include <unistd.h> /* for read(), close(), write() */
/*#include <sys/time.h> */

#include <ellLib.h>
#include <callback.h>
#include <alarm.h>
#include <iocsh.h>
#include <taskwd.h>

#include <recSup.h>
#include <recGbl.h>

#include <drvSup.h>
#include <devSup.h>
#include <devLib.h>

#include <dbScan.h>
#include <dbAddr.h> /* DBADDR */
#include "dbEvent.h"
#include "dbLock.h"
/*#include "dbCa.h" */
#include "dbCommon.h"


#include <epicsStdio.h>
/*#include <epicsExit.h> */  /* old Epics does not support this  in vxWorks */
#include <epicsExport.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>
#include <errlog.h> /* for errMessage(xxx), printf */


/*********************************************/

#include "nfrc_sys.h"

#include "cltu_Def.h"

#if defined(_CLTU_VXWORKS_)
#include <vxWorks.h>
#include <intLib.h>
#include <semLib.h>
#include <taskLib.h>
#include <iv.h>
#include <tickLib.h>
#include <timers.h>
#include <drv/pci/pciConfigLib.h>
#include <sysLib.h>
#include <drv/pci/pciConfigShow.h>


#elif defined(_CLTU_LINUX_)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/vt.h>
#include <sys/time.h>

#include <linux/rtc.h> 

#include "ioctl_cltu.h"

#include <epicsExit.h>

#else
#error plaease select OS!
#endif



#if defined(_CLTU_VXWORKS_)
#define CLTU_PCI_TASK_NAME "tCLTU_PCI"
#define CLTU_PCI_TASK_PRI (90)
#endif

#define CLTU_VENDOR_ID (0x10B5)
#define CLTU_DEVICE_ID (0x9601)

#define CLTU_VENDOR_ID_2 (0x10EE)
#define CLTU_DEVICE_ID_2 (0x0007)






#define CLTU_ASSERT()                                                          \
    printf("file:%s line:%d ASSERT!!\n",__FILE__,__LINE__)                     \

#if defined(_CLTU_VXWORKS_)
	#define DELAY_WAIT(time) taskDelay(time)  
#elif defined(_CLTU_LINUX_)
	#define DELAY_WAIT(time)  printf( " . "); usleep(time*100)
#else
	#error plaease select OS!
#endif


#if defined(_CLTU_LINUX_) 
#define STATUS int
#define ERROR (-1)
#define OK (0)
typedef unsigned long long int ULONG;
#endif

#if defined(_CLTU_VXWORKS_)
typedef struct
{
  unsigned int init;
  int BusNo;
  int DeviceNo;
  int FuncNo;  
  unsigned int base0;
  unsigned int base1;
  unsigned int base2;
  unsigned int base3;
  unsigned int base4;
  unsigned char int_line;
  SEM_ID        int_sem;
  int			intLockKey;
  unsigned int int_count;
  unsigned int int_handler;
  unsigned int int_error;  
}CLTU_PCI_DEVICE;
#elif defined(_CLTU_LINUX_)
typedef struct
{
  int fd;
  unsigned int init;
  char* base0;
  char* base1;
  char* base2;
  char* base3;
  char* base4;

}CLTU_PCI_DEVICE;
#else
#error plaease select OS!
#endif


/* int fd; */

int ioc_debug;

typedef	signed char			int8;
typedef	unsigned char		uint8;
typedef	signed short		int16;
typedef	unsigned short		uint16;
typedef	signed int			int32;
typedef	unsigned int		uint32;
typedef	float				float32;
typedef	double				float64;



typedef struct {
	void		*pCfg;
	void		*precord;
	double		setValue;
	int			nArg0;
	int			nArg1;
} execParam;

typedef void (*EXECFUNCQUEUE) (execParam *pParam);

typedef struct {
	EXECFUNCQUEUE     pFunc;
	execParam         param;
} controlThreadQueueData;


typedef struct {
	char                          threadName[60];
	epicsThreadId                 threadId;
	unsigned int                  threadPriority;
	unsigned int                  threadStackSize;
	EPICSTHREADFUNC               threadFunc;
	void                          *threadParam;
	
	epicsMessageQueueId           threadQueueId;
	
} drvCLTU_controlThreadConfig;
	

typedef struct
{
	int nTrigMode;
	int nClock;
  
	unsigned int nT0_msb;
	unsigned int nT0_lsb;
  
	unsigned int nT1_msb;
	unsigned int nT1_lsb;
  
} CLTU_PORT_CONFIG;

/*
typedef struct
{
	unsigned char cOnOff;
	unsigned char cTrigOnOff;
	unsigned char cActiveLow;

	unsigned long long int startOffset[NUM_SECTION_CNT];
	unsigned long long int stopOffset[NUM_SECTION_CNT];
	unsigned int  clock[NUM_SECTION_CNT];
  
} CLTU2_PORT_CONFIG;
*/

typedef struct {
		ELLLIST		*ptaskList;
		unsigned long long int one_sec_interval;
} drvCLTU_Config;


typedef struct {
	ELLNODE			node;
	int dev_num;
	char			taskName[60];
	char			deviceFile[60];
	epicsMutexId		taskLock;
	epicsMutexId		bufferLock;
	IOSCANPVT		ioScanPvt;

	drvCLTU_Config		*pdrvConfig;

	unsigned int	taskStatus;
	unsigned int	curPortConfigStatus[NUM_PORT_CNT];
	unsigned char u8Version; /* 0: old,    1: new */
	unsigned int u32Type; /* 0: XTU_MODE_CTU,    1: XTU_MODE_CTU_CMPST, 2:XTU_MODE_LTU,  BIT27:XTU_MODE_LTU_FREE  */
	unsigned char xtu_ID; /*   */
	unsigned char enable_IRIGB;
	unsigned char enable_LTU_freeRun;
	unsigned char enable_CTU_comp;
	unsigned int CMPST_Tick;
	unsigned int u32Calib_val;
	


	int fd_rtc;

	drvCLTU_controlThreadConfig    *pcontrolThreadConfig;
	drvCLTU_controlThreadConfig    *pinterruptThreadConfig;
/*	drvCLTUConfig		*pdrvConfig;*/
	
	CLTU_PCI_DEVICE pciDevice;
	int tPCI;
	
	CLTU_PORT_CONFIG sPortConfig[4];
	CLTU_PORT_CONFIG mPortConfig[3];
	CLTU2_PORT_CONFIG xPortConfig[NUM_PORT_CNT];
/*	CLTU_PORT_CONFIG curPortConfig; */
	
/*	CLTU_PORT_CONFIG tmpMultiTrigPort[5]; */
	
	unsigned int nShotTerm_msb;
	unsigned int nShotTerm_lsb;


	int calib_target_ltu;

	unsigned int nCCS_shotNumber;

	char strTOP[64];
	char strIOC[16];
	
}
ST_drvCLTU;


typedef void (*EPICSEXITFUNC)(void *arg);

/*ST_drvCLTU* drvCLTU_get_taskConfig(); */
ST_drvCLTU* drvCLTU_find_taskConfig(char *taskName);
ST_drvCLTU* get_first_CLTUdev();


ULONG drvCLTU_getCurrentTick(void);
double drvCLTU_intervalmSec(ULONG start, ULONG stop);



STATUS drvCLTU_set_shotTerm(ST_drvCLTU *pCfg );
STATUS drvCLTU_set_shotStart (ST_drvCLTU *pCfg );
STATUS drvCLTU_set_shotEnd (ST_drvCLTU *pCfg );
STATUS drvCLTU_set_portConfig (ST_drvCLTU *pCfg , unsigned int nPort);
STATUS drvCLTU_set_clockOnShot (ST_drvCLTU *pCfg , unsigned int nPort, int onoff);
STATUS drvCLTU_run_calibration(ST_drvCLTU *pCfg );
STATUS drvCLTU_set_RefClock(ST_drvCLTU *pCfg , int ref_clk_sel);
STATUS drvCLTU_put_calibratedVal(ST_drvCLTU *pCfg , unsigned int calibVal);

/* just call by function */
STATUS drvCLTU_set_rtc(ST_drvCLTU *pCfg , unsigned int);

STATUS drvCLTU_show_info(ST_drvCLTU *pCfg );

STATUS is_mmap_ok(ST_drvCLTU *pCfg );

/*
STATUS drvCLTU_set_tGap(unsigned int gap);
*/

#endif /* __drvCLTU_H__ */

