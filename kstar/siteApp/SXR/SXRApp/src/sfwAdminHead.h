#ifndef _DRV_ADMIN_HEAD_H
#define _DRV_ADMIN_HEAD_H

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


#include "sfwGlobalDef.h"
#include "sfwLocalDef.h"
#include <NIDAQmx.h>





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
	unsigned short opMode;  
	int32 shotNumber;
	float   fBlipTime; 
	int nSamplingRate;
	double   dT0[SIZE_CNT_MULTI_TRIG];
	double   dT1[SIZE_CNT_MULTI_TRIG];
}
ST_BASE_SET;


typedef struct {	
	char            taskName[SIZE_TASK_NAME];
	epicsMutexId    lock_mds;
	IOSCANPVT       ioScanPvt_status;
	unsigned short   StatusAdmin;
	unsigned short   ErrorAdmin;
	char   ErrorString[SIZE_STRINGOUT_VAL];

	ELLLIST        *pList_DeviceTask;	
	ST_threadCfg  *pST_adminCtrlThread;

	ST_BASE_SET ST_Base;
	
	char   cUseAutoSave;
	unsigned short n16DeviceNum;
	char n8MdsPutFlag;
	char n8After_Processing_Flag;
	char n8DC_OnOff_Flag;	
	char n8EscapeWhile;

	DBADDR db_SYS_ARMING;
	DBADDR db_SYS_RUN;

	unsigned long long int one_sec_interval;
}
ST_ADMIN;


typedef struct {
	ELLNODE			node;
	char			taskName[SIZE_TASK_NAME];
	unsigned short		BoardID;
	unsigned int    StatusDev;
	unsigned int    StatusMds;
	unsigned int    ErrorDev;
	char   ErrorString[SIZE_STRINGOUT_VAL];
	char   devType[SIZE_TASK_NAME];

	epicsMutexId		taskLock;
	IOSCANPVT		ioScanPvt_status;
	IOSCANPVT		ioScanPvt_userCall;

	ST_ADMIN   *pST_parentAdmin;
	ST_threadCfg    *pST_stdCtrlThread;

	epicsEventId	epicsEvent_CatchEnd;
	epicsEventId	epicsEvent_DAQthread;
	
	#if USE_LOCAL_DATA_POOL
	ST_threadCfg	*pST_BuffThread;
	ELLLIST 	*pList_BufferNode;
	#endif

	ELLLIST 	*pList_Channel;

	/* twice in Admin *********************/
	ST_BASE_SET ST_Base;

	/* 2009. 11. 05.   ... prevent input value during data storing *****/
	ST_BASE_SET ST_mds_fetch;

	/* use for MDSplus ***********************************/ 
	int socket; /* Will contain the handle to the remote mdsip server */
	char treeName[CNT_OPMODE][SIZE_TREE_NAME];
	char treeIPport[CNT_OPMODE][SIZE_TREE_IP_PORT];
	char treeEventIP[CNT_OPMODE][SIZE_TREE_IP_PORT];


	void *pUser;

}
ST_STD_device;

typedef struct {
	ELLNODE		node;
	unsigned short ChannelID;
	unsigned short StatusCh;
	char		chName[SIZE_TASK_NAME];

	ST_STD_device *pST_parentSTDdev;
	
	char cOnOff;
	float f32Gain;
	float f32Offset;
	char strTagName[SIZE_TAG_NAME];

	void *pUser;

}ST_STD_channel;





typedef struct {
	char devName[SIZE_TASK_NAME];
	char arg0[SIZE_DPVT_ARG];
	char arg1[SIZE_DPVT_ARG];
	char recordName[SIZE_PV_NAME];
	ST_STD_device *pSTDdev;
	ST_STD_channel *pSTDch;
	int    ind;
	int    n32Arg0;
} ST_dpvt;

typedef struct {
	ST_STD_device    *pSTDdev;
	ST_STD_channel   *pSTDch;
	void    *precord;
	double   setValue;
	char     setStr[SIZE_STRINGOUT_VAL];
	int      n32Arg0;
} ST_execParam;

typedef void (*EXECFUNCQUEUE) (ST_execParam *pParam);

typedef void (*VOIDFUNCPTR)(void *arg);

typedef struct {
	EXECFUNCQUEUE     pFunc;
	ST_execParam      param;
} ST_threadQueueData;





ST_ADMIN* drvAdmin_get_AdminPtr();
ST_STD_device* get_STDev_from_name(char *taskName);
ST_STD_device* get_first_STDdev();
ST_STD_channel* get_STCh_from_STDev_chName(ST_STD_device *pTargetDev, char *chanName);
ST_STD_channel* get_STCh_from_STDev_chID(ST_STD_device *pTargetDev, int ch);
ST_STD_channel* get_STCh_from_devName_chName(char *devName, char *chanName);
ST_STD_channel* get_STCh_from_devName_chID(char *devName, int ch);





void notify_refresh_admin_status();
void notify_admin_error_info(int level, const char *fmt, ...);

int admin_spinLock_mds_put_flag( ST_STD_device * );
void admin_all_taskStatus_reset();
int admin_check_Arming_condition();
int admin_check_Release_condition();
int admin_check_Run_condition();
int admin_check_Stop_condition();

void STDdev_status_reset(ST_STD_device *);


void flush_STDdev_to_MDSfetch( ST_STD_device *);
void flush_StBase_to_STDdev( ST_STD_device *);
void flush_STbase_to_All_STDdev( );
void flush_Blip_to_All_STDdev( );
void flush_ShotNum_to_All_STDdev( );
void flush_dT0_to_All_STDdev( );
ST_STD_device* get_STDev_from_Dev_Type(char *taskName);
void flush_STDdev_to_mdsplus( void *pArg );






#ifdef __cplusplus
}
#endif

#endif



