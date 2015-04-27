/******************************************************************************
 *  Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved.                     *
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

/*
#include "epicsRingBytes.h"
*/

#include <epicsMutex.h>
#include <epicsThread.h>
#include <ellLib.h>
#include <callback.h>
#include <iocsh.h>
#include <taskwd.h>

#include <dbCommon.h>
#include <drvSup.h>
#include <devSup.h>
#include <recSup.h>
#include <devLib.h>
#include <dbScan.h>
#include <dbLock.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsMessageQueue.h>
#include <errlog.h> /* for errMessage(), epicsPrintf() */

#include <string.h>
#include <stdlib.h>

/*********************************************/
/* from Mr. Park */
#include "nfrc_sys.h"

#if defined(_CLTU_VXWORK_)
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

#elif defined(_CLTU_LINUX_KERNEL_26_)
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/vt.h>
#include <string.h>

#include <linux/rtc.h> 

#else
#error plaease select OS!
#endif



#if defined(_CLTU_VXWORK_)
#define CLTU_PCI_TASK_NAME "tCLTU_PCI"
#define CLTU_PCI_TASK_PRI (90)
#endif

#define CLTU_VENDOR_ID (0x10B5)
#define CLTU_DEVICE_ID (0x9601)


#define CLTU_ASSERT()                                                          \
    printf("file:%s line:%d ASSERT!!\n",__FILE__,__LINE__)                     \


#if defined(_CLTU_LINUX_KERNEL_26_) 
#define STATUS int
#define ERROR (-1)
#define OK (0)
typedef unsigned long long int ULONG;
#endif

#if defined(_CLTU_VXWORK_)
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
#elif defined(_CLTU_LINUX_KERNEL_26_)
typedef struct
{
  int dev;
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

/*********************************************/
/* from Mr. Park */

#define T_YEAR_DIVIDER  (365*24*60*60)
#define T_DAY_DIVIDER  (24*60*60)
#define T_HOUR_DIVIDER (60*60)
#define T_MIN_DIVIDER  (60)
#define T_SEC_DIVIDER  (1)


#ifndef ERROR
#define ERROR	0
#endif

#ifndef OK
#define OK		1
#endif

#define TASK_NOT_INIT			0x0000
#define TASK_INITIALIZED		(0x0001<<0)
#define TASK_ADC_STARTED  		(0x0001<<1)
#define TASK_ADC_STOPED			(0x0001<<2)
#define TASK_DAQ_STARTED  		(0x0001<<3)
#define TASK_DAQ_STOPED			(0x0001<<4)
#define TASK_CHANNELIZATION	(0x0001<<5)
#define TASK_LOCAL_STORAGE		(0x0001<<6)
#define TASK_REMOTE_STORAGE	(0x0001<<7)

#define TASK_ARM_OUT				(0x0001<<8)
#define TASK_ARM_RESET			(0x0001<<9)

#define TASK_FINISH  1
	
#define PORT_NOT_INIT		0x0000
#define PORT_TRIG_OK  		(0x0001<<0)
#define PORT_CLOCK_OK		(0x0001<<1)
#define PORT_T0_OK  		(0x0001<<2)
#define PORT_T1_OK			(0x0001<<3)

#define ACTIVE_HIGHT_VAL		0

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
	void		*ptaskConfig;
	void		*precord;
	double		setValue;
	int			nPort;
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

typedef struct {
		ELLLIST		*ptaskList;
		unsigned long long int one_sec_interval;
} drvCLTU_Config;


typedef struct {
	ELLNODE			node;
	char			taskName[60];
	char			deviceFile[60];
	epicsMutexId		taskLock;
	epicsMutexId		bufferLock;
	IOSCANPVT		ioScanPvt;

	drvCLTU_Config		*pdrvConfig;

	unsigned int	taskStatus;
	unsigned int	curPortConfigStatus[5];

	int fd_rtc;

	drvCLTU_controlThreadConfig    *pcontrolThreadConfig;
	drvCLTU_controlThreadConfig    *pinterruptThreadConfig;
/*	drvCLTUConfig		*pdrvConfig;*/
	
	CLTU_PCI_DEVICE pciDevice;
	int tPCI;
	
	CLTU_PORT_CONFIG sPortConfig[4];
	CLTU_PORT_CONFIG mPortConfig[3];
/*	CLTU_PORT_CONFIG curPortConfig; */
	
/*	CLTU_PORT_CONFIG tmpMultiTrigPort[5]; */
	
	unsigned int nShotTerm_msb;
	unsigned int nShotTerm_lsb;


	int calib_target_ltu;
	
}
drvCLTU_taskConfig;


typedef void (*EPICSEXITFUNC)(void *arg);

/*drvCLTU_taskConfig* drvCLTU_get_taskConfig(); */
drvCLTU_taskConfig* drvCLTU_find_taskConfig(char *taskName);


ULONG drvCLTU_getCurrentTick(void);
double drvCLTU_intervalmSec(ULONG start, ULONG stop);



STATUS drvCLTU_set_shotTerm(drvCLTU_taskConfig *ptaskConfig );
STATUS drvCLTU_set_shotStart (drvCLTU_taskConfig *ptaskConfig );
STATUS drvCLTU_set_portConfig (drvCLTU_taskConfig *ptaskConfig , int nPort);
STATUS drvCLTU_set_clockOnShot (drvCLTU_taskConfig *ptaskConfig , int nPort, int onoff);
STATUS drvCLTU_set_calibration(drvCLTU_taskConfig *ptaskConfig );
STATUS drvCLTU_set_RefClock(drvCLTU_taskConfig *ptaskConfig , int ref_clk_sel);
STATUS drvCLTU_put_calibratedVal(drvCLTU_taskConfig *ptaskConfig , int calibVal);

/* just call by function */
STATUS drvCLTU_set_rtc(drvCLTU_taskConfig *ptaskConfig , unsigned int);


#ifdef FEATURE_SINGLE_UPDATE
int cltu_cmd_single_mt_update( void );
#endif
int cltu_pmc_info_show( drvCLTU_taskConfig * );


#endif /* __drvCLTU_H__ */

