/****************************************************************************

Module      : sfwCommon.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2009-11-24

2011. 1. 11  rename and reconfiguration. by woong

2013. 5. 24
add function "get_plasma_duration_sec()"  by woong, request from TGLee

*****************************************************************************/
#include <stdio.h>
#if !defined(WIN32)
#include <sys/time.h>
#endif
#include <string.h>
#include <stdlib.h>

/* for DB put *********/
#include "dbDefs.h"
#include "dbBase.h"
/*#include "dbBkpt.h" */
/*#include "dbNotify.h" */
#include "dbScan.h"
#include "dbStaticLib.h"
#include "db_access_routines.h"
#include "dbAccessDefs.h"
/* for DB put *********/



#include <dbAccess.h>
#include "errlog.h" /* for errMessage(xxx) */

#include "sfwDriver.h"
#include "sfwGlobalDef.h"
#include "sfwCommon.h"





int wf_print_date()
{
	
	struct tm *clock;
	time_t current;
	time(& current);
	clock=localtime(& current);
	
	printf("%d-%d-%d, ", clock->tm_year+1900, clock->tm_mon+1, clock->tm_mday);
	printf("%d:%d:%d\n", clock->tm_hour, clock->tm_min, clock->tm_sec);

	return WR_OK;
}


void printlog(const char *fmt, ... )
{
	char buf[1024];
	char bufTime[64];
	va_list argp;
	FILE *fpLog;
	int len;
	epicsTimeStamp epics_time;
	char fname[128];
	ST_MASTER *pMaster = get_master();

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	len = strlen(buf);
	if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;

	epicsTimeGetCurrent(&epics_time);
	epicsTimeToStrftime(bufTime, sizeof(bufTime), "%Y/%m/%d %H:%M:%S", &epics_time);

	
	if( pMaster->n8SaveLog ) 
	{
		sprintf(fname, "%s/%s", pMaster->strTOP, STR_LOG_FNAME);
		fpLog = fopen(fname, "a+");
		if( fpLog == NULL) {
			epicsPrintf("can't open log file: \"%s\"\n", fname);
		} 
		else {			
			fprintf(fpLog, "%s\t%s\n", bufTime, buf);
			fclose(fpLog);
		}
	}
	else
		printf("%s\t%s\n", bufTime, buf);
	
}

void kfwPrint(int level, const char *fmt, ... )
{
	char buf[1024];
	char buf_print[1024];
	char bufTime[64];
	int len;
	va_list argp;
	epicsTimeStamp epics_time;
	ST_MASTER *pMaster = get_master();

	if( level > pMaster->s32ShowLevel ) return;	/* default master level: 0 */

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	len = strlen(buf);
	if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;

	epicsTimeGetCurrent(&epics_time);
	epicsTimeToStrftime(bufTime, sizeof(bufTime), "%Y/%m/%d %H:%M:%S", &epics_time);
	sprintf(buf_print, "[%s] %s", bufTime, buf);

	printf("%s\n", buf_print ); 
	
}


unsigned long long int wf_getCurrentUsec(void)
{
#if !defined(WIN32)
	unsigned long long int x;
	struct timeval tv;

/*	__asm__ volatile(".byte 0x0f, 0x31": "=A"(x));	*/
	gettimeofday( &tv, NULL);

	x = tv.tv_sec;
	x = (x*1.E+6) + tv.tv_usec; 
	
/*	return timecurrent.tv_sec*1000 + timecurrent.tv_usec/	
	return x;
*/
	return x;
#else
	unsigned long long int x;
	epicsTimeStamp epics_time;

	epicsTimeGetCurrent(&epics_time);

	x  = epics_time.secPastEpoch * 1000000 + epics_time.nsec / 1000; 

	return x;
#endif
	
}

double wf_intervalUSec(unsigned long long int start, unsigned long long int stop)
{
	return (stop-start);
}




int env_SYS_pvname_assign(char *sysName)
{
	ST_MASTER *pMaster = get_master();

	sprintf( pMaster->strPvNames[NUM_SYS_OP_MODE], "%s_OP_MODE", sysName);
	sprintf( pMaster->strPvNames[NUM_SYS_ARMING], "%s_SYS_ARMING", sysName);
	sprintf( pMaster->strPvNames[NUM_SYS_RUN], "%s_SYS_RUN", sysName);
	sprintf( pMaster->strPvNames[NUM_CA_SHOT_NUMBER], "%s_CCS_SHOT_NUMBER", sysName);
	sprintf( pMaster->strPvNames[NUM_CA_BLIP_TIME], "%s_CCS_BLIP_TIME", sysName);
	sprintf( pMaster->strPvNames[NUM_SYS_CREATE_LOCAL_TREE], "%s_CREATE_LOCAL_TREE", sysName);
#if 0
	if( getenv("SYS_OP_MODE") ){
		strcpy( pMaster->strPvNames[NUM_SYS_OP_MODE], getenv("SYS_OP_MODE") );
	} else return WR_ERROR;

	if( getenv("SYS_ARMING") ){
		strcpy( pMaster->strPvNames[NUM_SYS_ARMING], getenv("SYS_ARMING") );
	} else return WR_ERROR;

	if( getenv("SYS_RUN") ){
		strcpy( pMaster->strPvNames[NUM_SYS_RUN], getenv("SYS_RUN") );
	} else return WR_ERROR;

	if( getenv("SYS_SHOT_NUMBER") ){
		strcpy( pMaster->strPvNames[NUM_CA_SHOT_NUMBER], getenv("SYS_SHOT_NUMBER") );
	} else return WR_ERROR;

	if( getenv("SYS_BLIP_TIME") ){
		strcpy( pMaster->strPvNames[NUM_CA_BLIP_TIME], getenv("SYS_BLIP_TIME") );
	} else return WR_ERROR;
#endif

	return WR_OK;
}

int env_MDS_Info_assign( )
{
	ST_MASTER *pMaster = get_master();

	if( getenv("MDS_LOCAL_IP_PORT") ){
		strcpy( pMaster->ST_mds.treeIPport[OPMODE_LOCAL], getenv("MDS_LOCAL_IP_PORT") );
	} else strcpy( pMaster->ST_mds.treeIPport[OPMODE_LOCAL], "Oops!" );
	if( getenv("MDS_LOCAL_TREE_NAME") ){
		strcpy( pMaster->ST_mds.treeName[OPMODE_LOCAL], getenv("MDS_LOCAL_TREE_NAME") );
	} else strcpy( pMaster->ST_mds.treeName[OPMODE_LOCAL], "Oops!" );
	if( getenv("MDS_LOCAL_EVENT_IP") ){
		strcpy( pMaster->ST_mds.treeEventIP[OPMODE_LOCAL], getenv("MDS_LOCAL_EVENT_IP") );
	} else strcpy( pMaster->ST_mds.treeEventIP[OPMODE_LOCAL], "Oops!" );

	if( getenv("MDS_REMOTE_IP_PORT") ){
		strcpy( pMaster->ST_mds.treeIPport[OPMODE_REMOTE], getenv("MDS_REMOTE_IP_PORT") );
	} else strcpy( pMaster->ST_mds.treeIPport[OPMODE_REMOTE], "Oops!" );
	if( getenv("MDS_REMOTE_TREE_NAME") ){
		strcpy( pMaster->ST_mds.treeName[OPMODE_REMOTE], getenv("MDS_REMOTE_TREE_NAME") );
	} else strcpy( pMaster->ST_mds.treeName[OPMODE_REMOTE], "Oops!" );
	if( getenv("MDS_REMOTE_EVENT_IP") ){
		strcpy( pMaster->ST_mds.treeEventIP[OPMODE_REMOTE], getenv("MDS_REMOTE_EVENT_IP") );
	} else strcpy( pMaster->ST_mds.treeEventIP[OPMODE_REMOTE], "Oops!" );

	if( getenv("MDS_CAL_IP_PORT") ){
		strcpy( pMaster->ST_mds.treeIPport[OPMODE_CALIBRATION], getenv("MDS_CAL_IP_PORT") );
	} else strcpy( pMaster->ST_mds.treeIPport[OPMODE_CALIBRATION], "Oops!" );
	if( getenv("MDS_CAL_TREE_NAME") ){
		strcpy( pMaster->ST_mds.treeName[OPMODE_CALIBRATION], getenv("MDS_CAL_TREE_NAME") );
	} else strcpy( pMaster->ST_mds.treeName[OPMODE_CALIBRATION], "Oops!" );
	if( getenv("MDS_CAL_EVENT_IP") ){
		strcpy( pMaster->ST_mds.treeEventIP[OPMODE_CALIBRATION], getenv("MDS_CAL_EVENT_IP") );
	} else strcpy( pMaster->ST_mds.treeEventIP[OPMODE_CALIBRATION], "Oops!" );

	strcpy( pMaster->ST_mds.treeIPport[OPMODE_INIT], "Init. IP");
	strcpy( pMaster->ST_mds.treeName[OPMODE_INIT], "Init. name");
	strcpy( pMaster->ST_mds.treeEventIP[OPMODE_INIT], "Init. event");

	if( getenv("MDS_EVENT_ARG") ){
		strcpy( pMaster->ST_mds.treeEventArg, getenv("MDS_EVENT_ARG") );
	} else strcpy( pMaster->ST_mds.treeEventArg, "None" );

#if 0
{
	int i;
	for( i=0; i<CNT_OPMODE; i++) {
		epicsPrintf("%s: %s, ", pMaster->taskName, pMaster->ST_mds.treeIPport[i] );
		epicsPrintf(" %s, ", pMaster->ST_mds.treeName[i] );		
		epicsPrintf(" %s\n", pMaster->ST_mds.treeEventIP[i] );
	}
}
#endif

	return WR_OK;
}

int env_Get_EPICS_ENV()
{
	ST_MASTER *pMaster = get_master();
	char strTemp[SIZE_ENV_IOC];

	if( getenv("TOP") ){
		sprintf(pMaster->strTOP, "%s", getenv("TOP")  );
		kfwPrint(0, "%s: TOP is \"%s\"\n",pMaster->taskName, pMaster->strTOP);
	} else {
		kfwPrint(0,"ERROR!!! %s: There is no \"TOP\" environment value\n",pMaster->taskName);
		sprintf(pMaster->strTOP, "%s", STR_LOG_LOCATION  );
	}

	if( getenv("IOC") ){
		sprintf(pMaster->strIOC, "%s", getenv("IOC") );
		kfwPrint(0, "%s: IOC is \"%s\"\n",pMaster->taskName, pMaster->strIOC);
	} else {
		kfwPrint(0, "ERROR!!! %s: There is no \"IOC\" environment value\n",pMaster->taskName);
		return WR_ERROR;
	}

	if( getenv("SFW_LOG") ){
		sprintf(strTemp, "%s", getenv("SFW_LOG") );
		if( !strcmp("DISABLE", strTemp) )
			pMaster->n8SaveLog = 0;
		kfwPrint(0, "%s: SFW_LOG is \"%s\" -> %d\n",pMaster->taskName, strTemp, pMaster->n8SaveLog);
	} else {
		kfwPrint(0, "ERROR!!! %s: There is no \"SFW_LOG\" environment value\n",pMaster->taskName);
		return WR_ERROR;
	}
	
	return WR_OK;
}


/******************************************************************************
*
*  FUNCTION:   notify_error
*  PURPOSE:   error status update through "$(SYS)_SYS_ERROR"
*  PARAMETERS: 
		level: 
			0-> just call scanIoRequest, 
			1->both call scanIoRequest and print
			else -> just printf.
		*fmt:
******************************************************************************/
void notify_error(int level, const char *fmt, ...)
{
	char buf_large[1024];
	char buf_print[512];
	char bufTime[64];
	int len;
	va_list argp;
	epicsTimeStamp epics_time;
	ST_MASTER *pMaster = get_master();

	va_start(argp, fmt);
	vsprintf(buf_large, fmt, argp);
	va_end(argp);

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (bufTime, sizeof(bufTime), "[%m/%d %H:%M:%S]", &epics_time);

/*	pMaster->StatusAdmin &= ~TASK_STANDBY;  2012. 3. 10 */
	len = strlen(buf_large);
	if( len > SIZE_STRINGOUT_VAL ) 
	{
		sprintf(pMaster->ErrorString, "OV,");
		strncat(pMaster->ErrorString, buf_large, SIZE_STRINGOUT_VAL-4);
	}
	else {
		sprintf(pMaster->ErrorString, "%s", buf_large);
		if ( pMaster->ErrorString[len-1] == '\n' || (pMaster->ErrorString[len-1] == '\0')  ) 
			pMaster->ErrorString[len-1] = 0L;
	}
	sprintf(buf_print, "%s %s\n", bufTime, buf_large);
	switch(level) {
		case ERR_SCN: 
			scanIoRequest(pMaster->ioScanPvt_status);
			break;
		case ERR_SCN_PNT:
			fprintf(stderr, "%s\n", buf_print);
			scanIoRequest(pMaster->ioScanPvt_status);
			break;
		case ERR_PNT:
			printf("%s\n", buf_print ); 
		default:
			printf("[SFW] error %s\n", buf_print ); 
			break;
	}

	return;
}

void notify_refresh_master_status()
{
	int bit[7], i;
	ST_MASTER *pMaster = get_master();
	ST_STD_device *pSTDdev = get_first_STDdev();
	for( i=0; i<7; i++) bit[i]=0;

	while(pSTDdev) {
		if( pSTDdev->StatusDev & TASK_STANDBY ) bit[0]++;
		if( pSTDdev->StatusDev & TASK_ARM_ENABLED ) bit[1]++;
		if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ) bit[2]++;
		if( pSTDdev->StatusDev & TASK_IN_PROGRESS) bit[3]++;
		if( pSTDdev->StatusDev & TASK_POST_PROCESS ) bit[4]++;
		if( pSTDdev->StatusDev & TASK_DATA_TRANSFER ) bit[5]++;
		if( pSTDdev->StatusDev & TASK_SYSTEM_IDLE ) bit[6]++;
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	if( pMaster->s32DeviceNum > 0 ) /* add this condition check for master standalone mode */
	{
		if( bit[0] == pMaster->s32DeviceNum ) pMaster->StatusAdmin |= TASK_STANDBY;
		else pMaster->StatusAdmin &= ~TASK_STANDBY;

		if( bit[1] == pMaster->s32DeviceNum )  pMaster->StatusAdmin |= TASK_ARM_ENABLED;
		else pMaster->StatusAdmin &= ~TASK_ARM_ENABLED;

		if( bit[2] == pMaster->s32DeviceNum  )  pMaster->StatusAdmin |= TASK_WAIT_FOR_TRIGGER;
		else pMaster->StatusAdmin &= ~TASK_WAIT_FOR_TRIGGER;

		if( bit[3] == pMaster->s32DeviceNum  )  pMaster->StatusAdmin |= TASK_IN_PROGRESS;
		else pMaster->StatusAdmin &= ~TASK_IN_PROGRESS;

		if( bit[4] == 0 )  pMaster->StatusAdmin &= ~TASK_POST_PROCESS;
		else pMaster->StatusAdmin |= TASK_POST_PROCESS;

		if( bit[5] == 0 )  pMaster->StatusAdmin &= ~TASK_DATA_TRANSFER;
		else pMaster->StatusAdmin |= TASK_DATA_TRANSFER;

		if( bit[6] == pMaster->s32DeviceNum )  pMaster->StatusAdmin |= TASK_SYSTEM_IDLE;
		else pMaster->StatusAdmin &= ~TASK_SYSTEM_IDLE;
	}

	scanIoRequest(pMaster->ioScanPvt_status);
	
}


SfwShareFunc void set_master_status_manually(unsigned int mask, unsigned int enable)
{
	ST_MASTER *pMaster = get_master();

/*	printf("[SFW] input mask: 0x%x, enable: %d\n", mask, enable); */
	
	switch(mask) 
	{
		case TASK_ARM_ENABLED :
			if( enable ) { 
				pMaster->StatusAdmin &= ~TASK_SYSTEM_IDLE;
				pMaster->StatusAdmin |= TASK_ARM_ENABLED;
			} else  {
				pMaster->StatusAdmin &= ~TASK_ARM_ENABLED;
				pMaster->StatusAdmin |= TASK_SYSTEM_IDLE;
			}
			break;
		case TASK_IN_PROGRESS :
			if( enable ) { 
/*				pMaster->StatusAdmin &= ~TASK_WAIT_FOR_TRIGGER; */
				pMaster->StatusAdmin |= TASK_IN_PROGRESS;
			} else  {
				pMaster->StatusAdmin &= ~TASK_IN_PROGRESS;
/*				pMaster->StatusAdmin |= TASK_WAIT_FOR_TRIGGER; */
			}
			break;
		case TASK_STANDBY :
		case TASK_SYSTEM_IDLE :
		case TASK_WAIT_FOR_TRIGGER :
		case TASK_POST_PROCESS :
		case TASK_DATA_TRANSFER :
			if( enable ) pMaster->StatusAdmin |= mask;
			else pMaster->StatusAdmin &= ~mask;
			break;

		default: break;
	}

/*	printf("[SFW] StatusAdmin: 0x%x\n", pMaster->StatusAdmin); */

	scanIoRequest(pMaster->ioScanPvt_status);
	
}

SfwShareFunc int get_status_auto_save()
{
	ST_MASTER *pMaster = get_master();
	
	return (int)pMaster->cUseAutoSave;
}

int admin_check_Arming_condition()
{
	ST_MASTER *pMaster = get_master();
	if( !(pMaster->StatusAdmin & TASK_STANDBY) ) 
	{
		epicsPrintf(">> Request Arming but \"%s\" not ready! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
	if( !(pMaster->StatusAdmin & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf(">> Request Arming but \"%s\" not Idle condition! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}
int admin_check_Release_condition()
{
	ST_MASTER *pMaster = get_master();
	if( !(pMaster->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request release but \"%s\" not yet armed! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
	if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
		pMaster->StatusAdmin & TASK_IN_PROGRESS ) 
	{
		epicsPrintf(">> Request release but \"%s\" already run! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}

int admin_check_Run_condition()
{
	ST_MASTER *pMaster = get_master();
	if( !(pMaster->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request Run but \"%s\" need to arming! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
	if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
		pMaster->StatusAdmin & TASK_IN_PROGRESS ) 
	{
		epicsPrintf(">> Request Run but \"%s\" already run! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
	return WR_OK;
}
int admin_check_Stop_condition()
{
	ST_MASTER *pMaster = get_master();
/*	if( !(pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ) 
	{
		epicsPrintf(">> Request Stop but \"%s\" not run! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
*/
	if( pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
		pMaster->StatusAdmin & TASK_IN_PROGRESS ) 
	{
		return WR_OK;
	}
	else 
		epicsPrintf(">> Request Stop but \"%s\" not run! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
	
	return WR_ERROR;
}

void admin_all_taskStatus_reset()
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	pMaster->StatusAdmin = TASK_NOT_INIT;
	pMaster->StatusAdmin |= TASK_SYSTEM_IDLE;
	pMaster->ErrorAdmin = ERROR_NONE;

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) {
		set_STDdev_status_reset(pSTDdev);
		
  		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}

int check_dev_arming_condition(void *pArg)
{
	ST_STD_device *pSTDdev=(ST_STD_device *)pArg;
#if 0
	ST_MASTER *pMaster = get_master();
	if( !(pMaster->StatusAdmin & TASK_STANDBY) ) 
	{
		epicsPrintf(">> Request Arming but \"%s\" not ready! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
#endif
	if( !(pSTDdev->StatusDev & TASK_STANDBY) ) 
	{
		epicsPrintf(">> Request Arming but \"%s\" not ready! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	if( !(pSTDdev->StatusDev & TASK_SYSTEM_IDLE) )
	{
		epicsPrintf(">> Request Arming but \"%s\" not Idle condition! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	return WR_OK;
}
int check_dev_release_condition(void *pArg)
{
	ST_STD_device *pSTDdev=(ST_STD_device *)pArg;
#if 0
	ST_MASTER *pMaster = get_master();
	if( !(pMaster->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request release but \"%s\" not yet armed! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
#endif
	if( !(pSTDdev->StatusDev & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request release but \"%s\" not yet armed! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	if( pSTDdev->StatusDev& TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev& TASK_IN_PROGRESS ) 
	{
		epicsPrintf(">> Request release but \"%s\" already run! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	return WR_OK;
}

int check_dev_run_condition(void *pArg)
{
	ST_STD_device *pSTDdev=(ST_STD_device *)pArg;
#if 0
	ST_MASTER *pMaster = get_master();
	if( !(pMaster->StatusAdmin & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request Run but \"%s\" need to arming! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
#endif
	if( !(pSTDdev->StatusDev & TASK_ARM_ENABLED) ) 
	{
		epicsPrintf(">> Request Run but \"%s\" need to arming! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	if( pSTDdev->StatusDev& TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev& TASK_IN_PROGRESS ) 
	{
		epicsPrintf(">> Request Run but \"%s\" already run! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
		return WR_ERROR;
	}
	return WR_OK;
}
int check_dev_stop_condition(void *pArg)
{
	ST_STD_device *pSTDdev=(ST_STD_device *)pArg;
/*	ST_MASTER *pMaster = get_master(); 
	if( !(pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER) ) 
	{
		epicsPrintf(">> Request Stop but \"%s\" not run! (0x%x)\n", pMaster->taskName, pMaster->StatusAdmin);
		return WR_ERROR;
	}
*/
	if( pSTDdev->StatusDev & TASK_WAIT_FOR_TRIGGER ||
		pSTDdev->StatusDev & TASK_IN_PROGRESS ) 
	{
		return WR_OK;
	}
	else 
		epicsPrintf(">> Request Stop but \"%s\" not run! (0x%x)\n", pSTDdev->taskName, pSTDdev->StatusDev);
	
	return WR_ERROR;
}


int master_is_locked()
{
	ST_MASTER *pMaster = get_master();

	if( pMaster->StatusAdmin & TASK_ARM_ENABLED ||
		pMaster->StatusAdmin & TASK_WAIT_FOR_TRIGGER ||
		pMaster->StatusAdmin & TASK_IN_PROGRESS ) 
	{
		return WR_OK;
	}
	return WR_ERROR;
}


void call_PRE_SEQSTART(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._PRE_SEQSTART )
		(pMaster->ST_Func._PRE_SEQSTART)(pMaster, setValue, 0);

	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._PRE_SEQSTART )
		{
/*			printf("call '_PRE_SEQSTART': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._PRE_SEQSTART)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void call_PRE_SEQSTOP(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._PRE_SEQSTOP )
		(pMaster->ST_Func._PRE_SEQSTOP)(pMaster, setValue, 0);
	
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._PRE_SEQSTOP )
		{
/*			printf("call '_PRE_SEQSTOP': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._PRE_SEQSTOP)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void call_POST_SEQSTOP(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._POST_SEQSTOP )
		(pMaster->ST_Func._POST_SEQSTOP)(pMaster, setValue, 0);
	
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._POST_SEQSTOP )
		{
/*			printf("call '_POST_SEQSTOP': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._POST_SEQSTOP)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
}
void call_PRE_SHOTSTART(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._PRE_SHOTSTART )
		(pMaster->ST_Func._PRE_SHOTSTART)(pMaster, setValue, 0);
	
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._PRE_SHOTSTART )
		{
/*			printf("call '_PRE_SHOTSTART': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._PRE_SHOTSTART)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

void call_TREE_CREATE(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._TREE_CREATE)
		(pMaster->ST_Func._TREE_CREATE)(pMaster, setValue, 0);
	
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._TREE_CREATE )
		{
/*			printf("call '_TREE_CREATE': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._TREE_CREATE)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

void call_FAULT_IP_MINIMUM(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._FAULT_IP_MINIMUM)
		(pMaster->ST_Func._FAULT_IP_MINIMUM)(pMaster, setValue, 0);
	
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._FAULT_IP_MINIMUM )
		{
/*			printf("call '_FAULT_IP_MINIMUM': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._FAULT_IP_MINIMUM)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}

void call_PLASMA_STATUS(double setValue)
{
	ST_STD_device *pSTDdev=NULL;
	ST_MASTER *pMaster = get_master();

	if( pMaster->ST_Func._PLASMA_STATUS)
		(pMaster->ST_Func._PLASMA_STATUS)(pMaster, setValue, 0);
	
	pSTDdev = (ST_STD_device*) ellFirst(pMaster->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( pSTDdev->ST_Func._PLASMA_STATUS )
		{
/*			printf("call '_PLASMA_STATUS': %s\n", pSTDdev->taskName); */
			(pSTDdev->ST_Func._PLASMA_STATUS)(pSTDdev, setValue, 0);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

}


/*
DBproc_put, DBproc_get will be erased.
please use db_put, db_get
*/
int DBproc_put(const char *pname, const char *pvalue)
{
	return db_put( pname, pvalue);
}

int DBproc_get(const char *pname, char *pvalue)
{
	return db_get(pname, pvalue);
}

int db_put(const char *pname, const char *pvalue)
{
	long  status;
	DBADDR addr;

	 if (!pname || !*pname || !pvalue) {
		kfwPrint(0, "ERROR!!! Usage: db_put \"pv name\", \"value\"\n");
		return WR_ERROR;
	}

	status=dbNameToAddr(pname, &addr);
	if (status) {
		kfwPrint(0, "ERROR!Record '%s' not found\n", pname);
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
		kfwPrint(0, "ERROR!Usage: db_get \"pv name\", \"value\"\n");
		return WR_ERROR;
	}

	status=dbNameToAddr(pname, &addr);
	if (status) {
		kfwPrint(0, "ERROR!Record '%s' not found\n", pname);
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

#if 0
int CAproc_get(const char *pname, char *pvalue)
{
	int result;                 /* CA result */
	ST_pv* pPvs;				  /* Array of PV structures */
	void *val_ptr;
/*
	result = ca_context_create(ca_disable_preemptive_callback);
	if (result != ECA_NORMAL) {
		fprintf(stderr, "CA error %s occurred while trying "
		"to start channel access '%s'.\n", ca_message(result), pname);
		return 1;
	}
*/
	pPvs = calloc (1, sizeof(ST_pv));
	if (!pPvs)
	{
		fprintf(stderr, "Memory allocation for channel structures failed.\n");
		return WR_ERROR;
	}
	strcpy(pPvs->name, pname);

	result = create_pvs(pPvs, 1, connection_handler);
	if ( result ) {
		printf("[SFW]  create_pvs() failed!\n");
		return WR_ERROR;
	}
	ca_pend_event(0.1);
	if (!pPvs->onceConnected) {
		printf("[SFW]  \"%s\" not yet connected!\n", pPvs->name);
		return WR_ERROR;
	}

	pPvs->nElems  = ca_element_count(pPvs->chid);
	pPvs->dbfType = ca_field_type(pPvs->chid);
/*	pPvs->dbrType = dbf_type_to_DBR_TIME(pPvs->dbfType); */
	pPvs->dbrType = dbf_type_to_DBR(pPvs->dbfType);
	pPvs->reqElems = 1;
	

	pPvs->value = calloc(1, dbr_size_n(pPvs->dbrType, pPvs->reqElems));
	if(!pPvs->value){ 
		printf(" ca calloc [SFW]\n");
		return WR_ERROR;
	}
	result = ca_array_get(pPvs->dbrType,
						pPvs->reqElems,
						pPvs->chid,
						pPvs->value);
	
	if( result != ECA_NORMAL){
		free(pPvs->value);
		printf("ca_array get [SFW]\n");
		return WR_ERROR;
	}
	
	result = ca_pend_io( 1.0);
	if ( result == ECA_TIMEOUT ) {
		free(pPvs->value);
		printf(" pend io err, time out. \n");
		return WR_ERROR;
	}
	
	val_ptr = dbr_value_ptr(pPvs->value, pPvs->dbrType);

	switch( pPvs->dbrType )
	{
		case DBF_STRING:
			sprintf(pvalue, "%s", ((dbr_string_t*) val_ptr)[0] );
			printf("%s, DBF_STRING(%d), %s\n", pPvs->name, DBF_STRING, pvalue );
			break;
		case DBR_INT: /*case DBF_INT: */
			sprintf(pvalue, "%d", ((dbr_int_t*) val_ptr)[0] );
			printf("%s, DBF_SHORT(%d), %s\n", pPvs->name, DBF_SHORT, pvalue );
			break;
		case DBF_FLOAT:
			sprintf(pvalue, "%f", ((dbr_float_t*) val_ptr)[0]  );
			printf("%s, DBF_FLOAT(%d), %s\n", pPvs->name, DBF_FLOAT, pvalue );
			break;
		case DBF_ENUM:
			printf("%s, DBF_ENUM(%d), not defined\n", pPvs->name, DBF_ENUM );
			break;
		case DBF_CHAR:
			sprintf(pvalue, "%c", ((dbr_char_t*) val_ptr)[0] );
			printf("%s, DBF_CHAR(%d), %s\n", pPvs->name, DBF_CHAR, pvalue );
			break;
		case DBF_LONG:
			sprintf(pvalue, "%d", ((dbr_long_t*) val_ptr)[0]  );
			printf("%s, DBF_LONG(%d), %s\n", pPvs->name, DBF_LONG, pvalue );
			break;
		case DBF_DOUBLE:
			sprintf(pvalue, "%lf", ((dbr_double_t*) val_ptr)[0] );
			printf("%s, DBF_DOUBLE(%d), %s\n", pPvs->name, DBF_DOUBLE, pvalue );
			break;
		default:
			free(pPvs->value);
			printf("%s, Bad dbrType (%lu)\n", pPvs->name, pPvs->dbrType);
			return WR_ERROR;
	}
/*	printf("CA Get %s, %s\n", pPvs->name, pvalue ); */
	free(pPvs->value);
	free(pPvs); 
/*	ca_context_destroy(); */
	return WR_OK;
}

int CAproc_put(const char *pname, const char *pvalue)
{
	int result;                 /* CA result */
	ST_pv* pPvs;				  /* Array of PV structures */

	pPvs = calloc (1, sizeof(ST_pv));
	if (!pPvs)
	{
		fprintf(stderr, "Memory allocation for channel structures failed.\n");
		return WR_ERROR;
	}
	strcpy(pPvs->name, pname);

	result = create_pvs(pPvs, 1, connection_handler);
	if ( result ) {
		printf("[SFW]  create_pvs() failed!\n");
		return WR_ERROR;
	}
	ca_pend_event(0.1);
	if (!pPvs->onceConnected) {
		printf("[SFW]  \"%s\" not yet connected!\n", pPvs->name);
		return WR_ERROR;
	}

	pPvs->dbfType = DBR_STRING;
	pPvs->value = calloc (strlen(pvalue), sizeof(char));
	if(!pPvs->value){ 
		printf(" ca calloc _SFW\n");
		return WR_ERROR;
	}
	printf("%s: put %s\n", pPvs->name, (char *)pPvs->value);
	
	result = ca_array_put (pPvs->dbrType, 1, pPvs->chid,  pPvs->value);
	if( result != ECA_NORMAL){
		free(pPvs->value);
		free(pPvs); 
		printf("ca_array_put _SFW\n");
		return WR_ERROR;
	}
	result = ca_pend_io( 1.0);
	if ( result == ECA_TIMEOUT ) {
		free(pPvs->value);
		free(pPvs); 
		printf(" pend io err, time out. \n");
		return WR_ERROR;
	}
	free(pPvs->value);
	free(pPvs); 
	return WR_OK;
}
#endif


SfwShareFunc int send_master_stop_event( )
{
	ST_MASTER *pMaster = get_master();
/*	pMaster->ST_Base.dRunPeriod = pMaster->ST_Base.dT1 - pMaster->ST_Base.dT0; */
	epicsEventSignal( pMaster->ST_StopEventThread.threadEventId);
	return WR_OK;
}

/* This is called every time at MDS tree creation */
SfwShareFunc int reset_event_flag()
{
	ST_MASTER *pMaster = get_master();
	
	pMaster->cStatus_ip_min_fault = 0;
	pMaster->flag_plasma_status = 0;
	pMaster->flag_sequence_on = 0;
	pMaster->flag_shot_start = 0;
	pMaster->flag_distruption = 0;

	return WR_OK;
}

SfwShareFunc int get_plasma_duration_sec()
{
	ST_MASTER *pMaster = get_master();
	unsigned int start, ipmin, duration;
/*
	start  = (double)pMaster->time_shot_start.secPastEpoch + (double)pMaster->time_shot_start.nsec * 1.e-9;
	ipmin = (double)pMaster->time_pcs_ipmin.secPastEpoch + (double)pMaster->time_pcs_ipmin.nsec * 1.e-9;
*/
	start  = pMaster->time_shot_start.secPastEpoch;
	ipmin = pMaster->time_pcs_ipmin.secPastEpoch;
/*	printf(" start: %d, ipmin: %d\n", start, ipmin); */
	duration = ipmin - start - (int)pMaster->ST_Base.fBlipTime;

	if( duration < 1)
	{
		kfwPrint(0, "Abnormal plasma duration %d", duration);
		duration = 1;
	}

	return duration;
}

SfwShareFunc int get_plasma_live_time()
{
	ST_MASTER *pMaster = get_master();
	unsigned int shotTime, onTime, offTime, curTime, duration=0;
	epicsTimeStamp current_time;
	epicsTimeGetCurrent(&current_time);
	curTime = current_time.secPastEpoch;

	shotTime = pMaster->time_shot_start.secPastEpoch;
	onTime  = pMaster->time_plasma_on.secPastEpoch;
	offTime = pMaster->time_plasma_off.secPastEpoch;
/*	printf(" start: %d, ipmin: %d\n", start, ipmin); */

	if( pMaster->flag_shot_start ) {
		if( onTime > shotTime ) {
			if( offTime > onTime ) /* dischare stop */
				duration = offTime - onTime;
			else /* discharge going on */
				duration = curTime - onTime;
		}
		else  /* not yet start-up  or shot fault! */
			duration = 0;
	} 
	else /* not yest shot started */
		duration = 0;

	return duration;
}



