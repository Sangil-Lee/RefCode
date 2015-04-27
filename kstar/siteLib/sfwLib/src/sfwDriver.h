#ifndef _DRV_ADMIN_HEAD_H
#define _DRV_ADMIN_HEAD_H


#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcpy */
#include <stddef.h>

#if !defined(WIN32)
#include <unistd.h> /* for read(), close(), write() */
#include <sys/time.h>
#endif

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
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>
#include <errlog.h> /* for errMessage(xxx), epicsPrintf */

#include "sfwGlobalDef.h"
#include "sfwCommon.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*VOIDFUNCPTR)(void *arg, double arg1, double arg2);

typedef struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_write;
	DEVSUPFUN	special_linconv;
} BINARYDSET;

typedef struct {
	char                  threadName[SIZE_TASK_NAME];
	epicsThreadId         threadId;
	unsigned int          threadPriority;
	unsigned int          threadStackSize;
	EPICSTHREADFUNC       threadFunc;
	void                 *threadParam;
	epicsMessageQueueId   threadQueueId;
	epicsEventId          threadEventId; /* 2010. 1. 27 */
}
ST_threadCfg;

typedef struct {
	unsigned short     opMode;  
	unsigned long int  shotNumber;
	float        fBlipTime;    /* default: 16, sec*/
	unsigned int   nSamplingRate; /* unit: Hz, default: 1KHz */
	float fStepTime; /* default: 1ms , 2013. 6. 19 */
	double   dT0[SIZE_CNT_MULTI_TRIG]; /* start time: rising time in normal 1 sec*/
	double   dT1[SIZE_CNT_MULTI_TRIG]; /* stop time: falling time in normal 10 sec*/
	double   dRunPeriod; /* unit: seconds, 10 sec */
}
ST_BASE_SET;

typedef struct {
	unsigned int    StatusMds;
	int   socket; /* Will contain the handle to the remote mdsip server */
	char  treeName[CNT_OPMODE][SIZE_TREE_NAME];
	char  treeIPport[CNT_OPMODE][SIZE_TREE_IP_PORT];
	char  treeEventIP[CNT_OPMODE][SIZE_TREE_IP_PORT];
	char  treeEventArg[SIZE_TREE_NAME];
}
ST_MDS_PLUS;

typedef struct {
	VOIDFUNCPTR _OP_MODE;
	VOIDFUNCPTR _BOARD_SETUP;
	VOIDFUNCPTR _SYS_ARMING;
	VOIDFUNCPTR _SYS_RUN;
	VOIDFUNCPTR _DATA_SEND;	
	VOIDFUNCPTR _SAMPLING_RATE;
	VOIDFUNCPTR _SYS_T0;
	VOIDFUNCPTR _SYS_T1;
	VOIDFUNCPTR _SHOT_NUMBER;
	VOIDFUNCPTR _CREATE_LOCAL_TREE;
	VOIDFUNCPTR _TEST_PUT;
	VOIDFUNCPTR _SYS_RESET;	
	VOIDFUNCPTR _Exit_Call;
	VOIDFUNCPTR _PRE_SEQSTART;
	VOIDFUNCPTR _PRE_SHOTSTART;	
	VOIDFUNCPTR _PRE_SEQSTOP;
	VOIDFUNCPTR _POST_SEQSTOP;
	VOIDFUNCPTR _TREE_CREATE;
	VOIDFUNCPTR _FAULT_IP_MINIMUM;	
	VOIDFUNCPTR _PLASMA_STATUS;
}
ST_CALL_FUNC;


typedef struct {	
	char            taskName[SIZE_TASK_NAME];
	epicsMutexId    lock_mds;
	IOSCANPVT       ioScanPvt_status;
	unsigned short   StatusAdmin;
	unsigned short   ErrorAdmin;
	char   ErrorString[SIZE_STRINGOUT_VAL];

	ELLLIST        *pList_DeviceTask;	
	ST_threadCfg  *pST_MstrCtrlThread;
	ST_threadCfg ST_StopEventThread;

	ST_BASE_SET ST_Base;
	
	ST_MDS_PLUS  ST_mds;

	/* 2012. 3. 6 add external call function in order to run as a controller */
	ST_CALL_FUNC ST_Func;
	
	char   cUseAutoSave;
	char   cEnable_IPMIN; /* 2013. 5.24 */
	char   cUseAutoCreateTree;
	int s32DeviceNum;
	volatile char n8MdsPutFlag;
	char n8EscapeWhile;
	char n8SaveLog;
	int s32ShowLevel; /* default level: 0 */

	char strPvNames[NUM_PV_STR_MAX][SIZE_PV_NAME];

	char strTOP[SIZE_ENV_TOP];
	char strIOC[SIZE_ENV_IOC];

	unsigned long long int one_sec_interval;
	epicsTimeStamp time_shot_start; /* 2013. 5.23 */
	epicsTimeStamp time_pcs_ipmin;
	epicsTimeStamp time_plasma_on;
	epicsTimeStamp time_plasma_off;

	char cStatus_ip_min_fault; /* 2013. 5.24 */
	epicsUInt16 flag_plasma_status; /* 2013. 8.28 */
	epicsUInt16 flag_sequence_on; /* 2013. 8. 28 */
	epicsUInt16 flag_shot_start; /* 2013. 8. 28 */
	epicsUInt16 flag_distruption; /* 2013. 8. 29  set to 0 when seq. start. / set to 1 when disruption */

	void *pUser;
}
ST_MASTER;


typedef struct {
	ELLNODE			node;
	uint16		BoardID; /* first: '0' */
	char ErrorString[SIZE_STRINGOUT_VAL];
	unsigned int    StatusDev;
	unsigned int    ErrorDev;
	
	char devType[SIZE_TASK_NAME];
	char taskName[SIZE_TASK_NAME];
	char strArg0[SIZE_STR_ARG];
	char strArg1[SIZE_STR_ARG];
	char strArg2[SIZE_STR_ARG];
	char strArg3[SIZE_STR_ARG];
	char strArg4[SIZE_STR_ARG];
	char strArg5[SIZE_STR_ARG];
	char strArg6[SIZE_STR_ARG];
	char strArg7[SIZE_STR_ARG];
	char strArg8[SIZE_STR_ARG];
	char strArg9[SIZE_STR_ARG];

	epicsMutexId		taskLock;
	IOSCANPVT		ioScanPvt_status;
	IOSCANPVT		ioScanPvt_userCall;
	/* request from TG.Lee for waveform record. 2011.6.1*/
	IOSCANPVT		ioScanPvt_userCall1;
	IOSCANPVT		ioScanPvt_userCall2;
	IOSCANPVT		ioScanPvt_userCall3;
	

	ST_MASTER   *pST_parentAdmin;
	ST_threadCfg    *pST_stdCtrlThread;

	ST_threadCfg ST_DAQThread;
	ST_threadCfg ST_CatchEndThread;	
/* delete it */ ST_threadCfg ST_StopEventThread; /* not used anywhere.  maybe delete 2012. 8. 16*/
	ST_threadCfg	ST_RingBufThread;	
	ELLLIST 	*pList_BufferNode;
	unsigned long   maxMessageSize;
	ST_threadCfg ST_RTthread; /* 2012. 8. 16  for RTMON*/


	ST_CALL_FUNC ST_Func;

	ELLLIST 	*pList_Channel;

	/* twice in Admin *********************/
	ST_BASE_SET ST_Base;
	/* 2009. 11. 05.   ... prevent input value during data storing *****/
	ST_BASE_SET ST_Base_fetch;

	/* use for MDSplus ************/ 
	ST_MDS_PLUS  ST_mds;

	/* use for channel On/Off masking  *******/ 
	epicsUInt32 channel_mask;

	/* related for Real time performance   2012. 8. 16 *********/
	volatile epicsUInt32 stdCoreCnt;
	volatile epicsUInt32 stdDAQCnt;
	epicsUInt32 rtClkRate; /* default: 1KHz, hold on every device */
	epicsFloat32 rtStepTime; /* default: 1ms, hold on every device */


	void *pUser;

}
ST_STD_device;

typedef struct {
	ELLNODE		node;	
	unsigned short StatusCh;
	char chName[SIZE_TASK_NAME];
	
	unsigned short channelId;
	char strArg0[SIZE_STR_ARG];
	char strArg1[SIZE_STR_ARG];
	char strArg2[SIZE_STR_ARG];
	char strArg3[SIZE_STR_ARG];
	char strArg4[SIZE_STR_ARG];
	char strArg5[SIZE_STR_ARG];
	char strArg6[SIZE_STR_ARG];
	char strArg7[SIZE_STR_ARG];
	char strArg8[SIZE_STR_ARG];
	char strArg9[SIZE_STR_ARG];


	ST_STD_device *pST_parentSTDdev;
	
	char cOnOff;
	float f32Gain;
	float f32Offset;
	char strTagName[SIZE_TAG_NAME];



	VOIDFUNCPTR _Exit_Call;
	void *pUser;

}
ST_STD_channel;


typedef struct {
	char devName[SIZE_TASK_NAME];
/* delete */	char arg0[SIZE_DPVT_ARG]; /* don't use it. will be deleted */
/* delete */	char arg1[SIZE_DPVT_ARG]; /* don't use it. will be deleted */
/* delete */	char recordName[SIZE_PV_NAME]; /* 2012. 7. 31 don't use it. will be deleted. woong.*/
	ST_MASTER  *pMaster;
	ST_STD_device *pSTDdev;
	ST_STD_channel *pSTDch;
	int    ind;
	int    n32Arg0;
	int    n32Arg1;
} ST_dpvt;

typedef struct {
	ST_STD_device    *pSTDdev;
	ST_STD_channel   *pSTDch;
	void    *precord;
	double   setValue;
	char     setStr[SIZE_STRINGOUT_VAL];
	int      n32Arg0;
	int      n32Arg1;
} ST_execParam;

typedef void (*EXECFUNCQUEUE) (ST_execParam *pParam);
typedef void (*VOIDFUNCPTREXIT)(void *arg);
typedef struct {
	EXECFUNCQUEUE     pFunc;
	ST_execParam      param;
} ST_threadQueueData;


SfwShareFunc int make_STD_Master_Stop_thread();
SfwShareFunc int make_STD_Dev_CtrlThread(ST_STD_device *pSTDdev);

SfwShareFunc int make_STD_DAQ_thread(ST_STD_device *pSTDdev);
SfwShareFunc int make_STD_RingBuf_thread(ST_STD_device *pSTDdev);
SfwShareFunc int make_STD_CatchEnd_thread(ST_STD_device *pSTDdev);
SfwShareFunc int make_STD_RT_thread(ST_STD_device *pSTDdev);



SfwShareFunc ST_MASTER* get_master();


/******************************************************************************
*
*  FUNCTION: get_STDev_from_name
*  PURPOSE: get standard device node pointer with given name
*  PARAMETERS: 
		taskName->give task name

******************************************************************************/
SfwShareFunc ST_STD_device* get_STDev_from_name(char *taskName);


/******************************************************************************
*
*  FUNCTION: get_STDev_from_type
*  PURPOSE: get first matching node with given type
*  PARAMETERS: 
		typeName: given device type

******************************************************************************/
SfwShareFunc ST_STD_device* get_STDev_from_type(char *typeName);


/****************************************************/
/* get first device node in master list */
SfwShareFunc ST_STD_device* get_first_STDdev();

SfwShareFunc ST_STD_channel* get_STCh_from_STDev_chName(ST_STD_device *pTargetDev, char *chanName);
SfwShareFunc ST_STD_channel* get_STCh_from_STDev_chID(ST_STD_device *pTargetDev, unsigned short ch);
SfwShareFunc ST_STD_channel* get_STCh_from_devName_chName(char *devName, char *chanName);
SfwShareFunc ST_STD_channel* get_STCh_from_devName_chID(char *devName, unsigned short ch);

SfwShareFunc void flush_STDdev_to_MDSfetch( ST_STD_device *);
SfwShareFunc void flush_Master_to_STDdev( ST_STD_device *);
SfwShareFunc void flush_STbase_to_All_STDdev( );  /* will be delete, don't use this function */
SfwShareFunc void flush_STBase_to_All_STDdev( );
SfwShareFunc void flush_Blip_to_All_STDdev( );
SfwShareFunc void flush_ShotNum_to_All_STDdev( );

SfwShareFunc void set_STDdev_status_reset(ST_STD_device *);
SfwShareFunc void set_Exit_function();



SfwShareFunc int admin_spinLock_mds_put_flag( ST_STD_device * );



#ifdef __cplusplus
}
#endif

#endif



