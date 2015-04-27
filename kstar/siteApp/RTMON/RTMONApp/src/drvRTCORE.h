#ifndef drvRTCORE_H
#define drvRTCORE_H


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


#include "ioctl_intLTU.h"
#include "intLTU_Def.h"

#include "rfmAddrMap.h"
#include "rfm2g_api.h"

#define STR_DEVICE_TYPE_1   "RTcoreDev"


#define USE_CPU_AFFINITY_RT   1
#define USE_RTCORE_DAQRING    0

#define USE_KERNEL_ACK_HIGH      0
#define USE_KERNEL_ACK_LOW       0
#define USE_RTCORE_DAQ_HIGH      0
#define USE_RTCORE_DAQ_LOW       0
#define USE_RMCHK_ACK_HIGH       0
#define USE_RMCHK_ACK_LOW        0
#define USE_RMCHK_DAQ_PRE_IO_HIGH    1
#define USE_RMCHK_DAQ_POST_IO_HIGH   0
#define USE_RMCHK_DAQ_POST_IO_LOW    1
#define USE_RMCHK_RING_IO_HIGH   0
#define USE_RMCHK_RING_IO_LOW    0


#define PRIOTY_RTCORE_RT      98
#define AFFINITY_RTCORE_RT      "5"

#define PRIOTY_RTCORE_DAQ     92
#define AFFINITY_RTCORE_DAQ      "4"

#define PRIOTY_RTCORE_DivA     92
#define AFFINITY_RTCORE_DivA      "3"

#define PRIOTY_RTCORE_DivB     92
#define AFFINITY_RTCORE_DivB      "3"

#define PRIOTY_RTCORE_RING    92
#define AFFINITY_RTCORE_RING      "3"

#define PRIOTY_RMCHK_DAQ    97
#define AFFINITY_RMCHK_DAQ      "5"

#define PRIOTY_RMCHK_RING    96
#define AFFINITY_RMCHK_RING      "5"

#define USE_DIVIDER_A          0
#define DIVIDER_A                 4    /* 5KHz, when 20KHz */
#define USE_RTCORE_DivA_HIGH      0
#define USE_RTCORE_DivA_LOW       0

#define USE_DIVIDER_B          0
#define DIVIDER_B                20	/* 1KHz, when 20KHz */
#define USE_RTCORE_DivB_HIGH      0
#define USE_RTCORE_DivB_LOW       0

//#define CFG_DMA_THRESHOLD    0xffffffff
#define CFG_DMA_THRESHOLD    0x10 /* 0x100: 256 */



/******* make for RING buffer   ************************************/
#define MAX_RING_BUF_NUM            10
#define MAX_CUTOFF_BUF_NODE         1
#define BUFLEN	(1000)   /* 2012. 2. 6 */	/* if clock is 10KHz, then 1KHz event  generated */
/* can save 100 element with 1KHz speed when 10KHz interrupt */
#define LOOP_CNT    (1000)
#define LOOP_ELM    (13)


typedef union {
	PF_return		RM_PF_return[14];
	TF_return		RM_TF_return;
	IVC_return 	RM_IVC_return;
} UN_RFM_tag;

#if 0
typedef struct {
	ELLNODE			node;
	__u32 data[ BUFLEN ];  
} ST_buf_node;
#endif

typedef struct {
	ELLNODE			node;
	PF_return RM_PF_return[11];
	TF_return RM_TF_return;
	IVC_return RM_IVC_return;
} ST_buf_node;


typedef struct {
	ST_buf_node	*pNode;
	unsigned int	timeStamp;

} ST_User_Buf_node;
/****************************************************/



typedef struct epicsThreadOSD {
    ELLNODE            node;
    pthread_t          tid;
    pthread_attr_t     attr;
    struct sched_param schedParam;
    EPICSTHREADFUNC    createFunc;
    void              *createArg;
    epicsEventId       suspendEvent;
    int                isSuspended;
    int                isEpicsThread;
    int                isOnThreadList;
    unsigned int       osiPriority;
    char              *name;
#if defined (SHOW_LWP)
    pid_t              lwpId;
#endif /* SHOW_LWP */
} epicsThreadOSD;


/*
 *  Address of control register
 */



typedef struct {
	/* add your device dependent parameters */
/*	volatile unsigned int cntDAQ; */
	volatile unsigned int cntDAQ_loop; /* not use */
	volatile unsigned int cntAccum; /* not use */
	
/*	double		daqStepTime; */
	int 	useScanIoRequest;

	int  fd_event;
	char *base0;
	
	ST_threadCfg ST_DivThreadA; /* 2013. 6. 4  for divide X*/
	ST_threadCfg ST_DivThreadB; /* 2013. 6. 4  for divide X*/



/*	double clkStartTime; */

	

} 
ST_RTcore;



int init_my_sfwFunc_RTcore( ST_STD_device *pSTDdev);
int create_RTcore_taskConfig( ST_STD_device *pSTDdev);
void clear_RTcore_taskConfig(void *pArg, double arg1, double arg2);
int init_DivThreads_RTcore( ST_STD_device *pSTDdev);



void threadFunc_RTcore_DAQ(void *param);
void threadFunc_RTcore_RT(void *param);
void threadFunc_RTcore_RingBuf(void *param);

void threadFunc_RTcore_DivideA(void *param);
void threadFunc_RTcore_DivideB(void *param);





void func_RTcore_OP_MODE(void *, double, double);
void func_RTcore_BOARD_SETUP(void *, double, double);
void func_RTcore_SYS_ARMING(void *, double, double);
void func_RTcore_SYS_RUN(void *, double , double );
void func_RTcore_SAMPLING_RATE(void *, double , double );
void func_RTcore_SYS_RESET(void *, double , double );


#endif /* drvRTCORE_H */

