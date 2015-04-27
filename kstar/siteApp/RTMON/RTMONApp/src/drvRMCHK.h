#ifndef drvRMCHK_H
#define drvRMCHK_H


#include "sfwDriver.h"
#include "dbAccess.h"   

#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h> /* open() */


#include "rfmAddrMap.h"

#include "rfm2g_api.h"

#define INIT_RFM_MAP_OFFSET	0
#define INIT_RFM_MAP_PAGE	1
#define INIT_RFM_MAP_BYTES	0x8000000  /* 128MB */

#define	USE_RFM_MMAP            1

#define MAX_USER_OFFSET_NUM	50
/*#define MAX_USER_POKE32_OFFSET_NUM	30 */




#define MAX_TAG_CNT  300


#define STR_DEVICE_TYPE_2   "RFMdev"

/*
 *  Address of control register
 */


typedef struct {
	/* add your device dependent parameters */
/*	volatile unsigned int cntDAQ; */
/*	volatile unsigned int cntDAQ_loop;
	volatile unsigned int cntAccum; */
	
/*	double		daqStepTime; */
	int 	useScanIoRequest;


	RFM2GHANDLE    Handle;
	RFM2G_NODE     NodeId;
	RFM2G_UINT64 Offset;
	RFM2G_UINT32 Pages;
	RFM2G_UINT32 Bytes;

	volatile unsigned int *mapBuf;
/*	volatile char *mapBuf; */

	FILE *fp_cnt;
/*	FILE *fp_fault; */
	FILE *fp_raw;
/*	int fd_shot; */


	PCS_cmd_PF RM_PCS_PF;
	PCS_cmd_IVC RM_PCS_IVC;
	CCS_from_PCS	RM_CCS_SHARE;
	PF_return		RM_PF_return[14];
	TF_return		RM_TF_return;
	IVC_return 	RM_IVC_return;


/*	double clkStartTime; */

	unsigned int user_offset[MAX_USER_OFFSET_NUM];
/*	unsigned int user_poke32_offset[MAX_USER_POKE32_OFFSET_NUM]; */



	unsigned int cntTag;
	char   strTagName[MAX_TAG_CNT][SIZE_STRINGOUT_VAL];
	char channelOnOff[MAX_TAG_CNT];
	unsigned int  tagAddr[MAX_TAG_CNT];


} 
ST_RMCHK;



int init_my_sfwFunc_RMCHK( ST_STD_device *pSTDdev);
int create_RMCHK_taskConfig( ST_STD_device *pSTDdev);
void clear_RMCHK_taskConfig(void *pArg, double arg1, double arg2);

void test_mmap_working();


void threadFunc_RMCHK_DAQ(void *param);
void threadFunc_RMCHK_RingBuf(void *param);

void drvRMCHK_init_file( ST_STD_device *pSTDdev);
void drvRMCHK_save_file( ST_STD_device *pSTDdev);




void func_RMCHK_OP_MODE(void *, double, double);
void func_RMCHK_BOARD_SETUP(void *, double, double);
void func_RMCHK_SYS_ARMING(void *, double, double);
void func_RMCHK_SYS_RUN(void *, double , double );
void func_RMCHK_SAMPLING_RATE(void *, double , double );
void func_RMCHK_DATA_SEND(void *, double , double );
void func_RMCHK_SYS_RESET(void *, double , double );


#endif /* drvRMCHK_H */

