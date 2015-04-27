#ifndef _TSSG_DEFINE_H
#define _TSSG_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include <stdio.h>

#include <unistd.h> /* for read(), close(), write() */
#include <sys/time.h>

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
#include "dbCa.h"
#include "dbCommon.h"


#include <epicsStdio.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>
#include <errlog.h> /* for errMessage(xxx), epicsPrintf */


#ifndef WR_ERROR
#define WR_ERROR	0
#endif

#ifndef WR_OK
#define WR_OK		1
#endif


#define PRINT_PHASE_INFO         0
#define PRINT_DEV_SUPPORT_ARG    0


#define SIZE_PV_NAME        61   /* Record Name */
#define SIZE_TASK_NAME      40

/********************************************************/
#define TASK_NOT_INIT			0x0
/*****************/
#define TASK_SYSTEM_READY		(0x0001<<0)
#define TASK_ARM_ENABLED  		(0x0001<<1)
#define TASK_DAQ_STARTED  		(0x0001<<2)
#define TASK_AFTER_SHOT_PROCESS		(0x0001<<3)
/*****************/
#define TASK_DATA_PUT_STORAGE		(0x0001<<4)
#define TASK_SYSTEM_IDLE		(0x0001<<5)
/********************************************************/

#define ERROR_NONE			0x0
#define ERROR_AFTER_SHOT_PROCESS	(0x0001<<0)
#define ERROR_DATA_PUT_STORAGE         (0x0001<<1)


#define LEV8LSB(x) ((x) & 0xff )
#define SFT8MSB(x) (((x) >> 8) & 0xff )
#define SFT16MSB(x) (((x) >> 16) & 0xff )
#define SFT24MSB(x) (((x) >> 24) & 0xff )
#define SWAP16(x)	( (LEV8LSB(x) << 8) | SFT8MSB(x) )
#define SWAP32(x)	(  (SFT24MSB(x)) | (SFT16MSB(x)<<8) | (SFT8MSB(x)<<16) | ( LEV8LSB(x)<<24 ) ) 


/*
 *  Address of control register
 */

#define MAP_SIZE	65536UL
#define MAP_MASK 	(MAP_SIZE - 1)


#define C_BASEADDR			0x86000000
#define REG_START_STOP_OFFSET		0x00

#define REG_LAMP_CLK_OFFSET		0x04
#define REG_LAMP_WIDTH_OFFSET		0x08
#define REG_QSWITCH_DELAY_OFFSET		0x0C
#define REG_QSWITCH_WIDTH_OFFSET		0x10

#define REG_BACK_PULSE_DELAY_OFFSET		0x14
#define REG_BACK_PULSE_CNT_OFFSET		0x18
#define REG_BACK_PULSE_WIDTH_OFFSET		0x1C

#define REG_BTW_LTU_DG535_IN		0x20
#define REG_INPUT_PULSE_CNT           0x24





typedef struct
{
	volatile unsigned int gpStartStop;
	volatile unsigned int gpLampClkHz;
	volatile unsigned int gpLampClkWidth;
	volatile unsigned int gpQSwitchDely;
	volatile unsigned int gpQSwitchWidth;
/*	volatile unsigned int gpLtuLeadTime; */
	volatile unsigned int gpBackPulseDelay;
	volatile unsigned int gpBackPulseCnt;
	volatile unsigned int gpBackPulseWidth;
	
/*	volatile unsigned int gpPulseGapDelay;
	volatile unsigned int gpQdcPulseWidth;
	volatile unsigned int gpBtwLTU_laser_input; */
	volatile unsigned int gpBtwLTU_dg535_input;
	volatile unsigned int gpRealPulseCnt;	
} ST_REG_MAP;




typedef struct {
	char                  threadName[SIZE_TASK_NAME];
	epicsThreadId         threadId;
	unsigned int          threadPriority;
	unsigned int          threadStackSize;
	EPICSTHREADFUNC       threadFunc;
	void                 *threadParam;
	epicsMessageQueueId   threadQueueId;
}
ST_threadCfg;


typedef struct {	
	char            taskName[SIZE_TASK_NAME];
	IOSCANPVT       ioScanPvt_status;
	unsigned short   StatusTSSG;
	unsigned short   ErrorTSSG;

	ELLLIST        *pList_DeviceTask;	
	ST_threadCfg  *pST_CtrlThread;

/*	void *pBaseAddr; */
	int  fd;
	ST_REG_MAP *pBaseAddr;

}
ST_TSSG;





#ifdef __cplusplus
}
#endif

#endif



