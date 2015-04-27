#ifndef drvRTcore_H
#define drvRTcore_H


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

#define STR_DEVICE_TYPE_1   "RTcoreDev"

/********************************** usere code */
#include "16aiss8ao4.h"
#include "16aiss8ao4_dsl.h"
#include "util_close.h"
#include "util_count.h"
#include "util_id.h"
#include "util_label.h"
#include "util_open.h"
#include "util_select.h"

#include "util_auto_cal.h"
#include "util_init.h"
#include "util_input_config.h"
#include "util_input_buffer.h"
#include "util_io.h"
#include "util_output_buffer.h"
#include "util_output_config.h"
#include "util_rate_gen.h"

#include "util_reg.h"

#define INIT_TX_CLOCK   1000            /* 1KHz */
#define INIT_RX_CLOCK   10000   /* 10KHz */

/********************************** usere code */

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
	unsigned long int cntDAQ;
	double		daqStepTime;
	int 	useScanIoRequest;



	FILE *fp_cnt;
	FILE *fp_fault;
	int fd_shot;

	double clkStartTime;



/********************************** usere code */
	int             fd;
	uint16 u16GPIO;
	int GPIO_0_IO;
	int GPIO_1_IO;

	uint32 out_ch_mask;             /*AO_CH_ALL_ENABLE*/
        uint32 out_io_mode;             /* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
        uint32 out_mode;        /* OUT_MODE_REGISTER, OUT_MODE_BUFFERED */
        uint32 out_simultaneous; /* TRUE, FALSE */
        uint32 out_clock_initiator_mode; /*OUT_CLK_INITIATOR_RATE_C, OUT_CLK_INITIATOR_HARDWARE*/
        uint32 out_range;                       /*OUTPUT_RANGE_10, OUTPUT_RANGE_2_5, OUTPUT_RANGE_5*/
        uint32 out_burst_enable;        /* FALSE */
        uint32 out_clock_c;     /* 40MHz */
        uint32 out_rate_c;              /* 10 K */
        uint32 out_rate_gen_c_enable;   /*OUT_CLK_DISABLE, OUT_CLK_ENABLE */
        uint32 out_clock_enable;                /*OUT_CLK_DISABLE, OUT_CLK_ENABLE */
        uint32 out_buffer_threshold;
        uint32 out_circular_buffer_enable; /* FALSE */
        uint32 out_period;      /* 5 */



        uint32 in_ch_mask;              /* ENABLE_INPUT_0, ENABLE_INPUT_1, xxx, ENABLE_INPUT_ALL */
        uint32 in_io_mode;              /* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
        uint32 in_clock_initiator_mode; /*default:CLK_INITIATOR_OUTPUT, CLK_INITIATOR_INPUT*/
        uint32 in_mode;         /* AIM_DIFFERENTIAL, AIM_SINGLE_ENDED, xxx*/
        uint32 in_range_a;      /*INPUT_RANGE_10, INPUT_RANGE_2_5, INPUT_RANGE_5*/      /* 0,1,4,5 */
        uint32 in_range_b;      /*INPUT_RANGE_10, INPUT_RANGE_2_5, INPUT_RANGE_5*/      /* 2,3,6,7 */
        uint32 in_burst_enable; /* FALSE */
        uint32 in_clock_a;      /* 40MHz */
        uint32 in_rate_a;               /* 10K */
        uint32 in_rate_gen_a_enable;    /* TRUE */
        uint32 in_buffer_threshold;
        uint32 in_buffer_enable; /* TRUE */
        uint32 in_period;       /* 5*/


        uint32 input_format;    /* DATA_FORMAT_OFFSET_BINARY */


} 
ST_RTcore;



int init_my_sfwFunc_RTcore( ST_STD_device *pSTDdev);
int create_RTcore_taskConfig( ST_STD_device *pSTDdev);
void clear_RTcore_taskConfig(void *pArg, double arg1, double arg2);


void threadFunc_RTcore_DAQ(void *param);
void threadFunc_RTcore_RT(void *param);



void func_RTcore_OP_MODE(void *, double, double);
void func_RTcore_BOARD_SETUP(void *, double, double);
void func_RTcore_SYS_ARMING(void *, double, double);
void func_RTcore_SYS_RUN(void *, double , double );
void func_RTcore_SAMPLING_RATE(void *, double , double );
void func_RTcore_SYS_RESET(void *, double , double );


#endif /* drvRTcore_H */

