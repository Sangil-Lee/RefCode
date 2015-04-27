#ifndef drvDNST_H
#define drvDNST_H


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



#define STR_DEVICE_TYPE_2   "DNSTdev"

/*
 *  Address of control register
 */

/*#include "CodacPcsRun.h"*/

/******* make for RING buffer   ************************************/
#define MAX_RING_BUF_NUM            12
#define MAX_CUTOFF_BUF_NODE         4
/*#define BUFLEN        (800)  */ /* @@todo read from device */
#define BUFLEN  (200)   /* 2012. 8. 18 */ /* 100 sample * 2 channel ( 100KHz) */

typedef struct {
	ELLNODE                 node;
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	__u32 data[ BUFLEN ];
} ST_buf_node;

typedef struct {
	ST_buf_node     *pNode;
	unsigned int	u32Cnt;
	unsigned int	size;
	char			opcode;
} ST_User_Buf_node;
/****************************************************/



typedef struct {
	/* add your device dependent parameters */
	unsigned long int cntDAQ;
	double		daqStepTime;
	int 	useScanIoRequest;


	FILE *fp_cnt;
	FILE *fp_fault;
	int fd_shot;


	double clkStartTime;


	// for WebStatistic
	float raw_vltg1;
	float raw_vltg2;
	double  density;
	double  pid_u; // actuator command
	double  pid_r; // reference signal
	double  pid_e; // error between r and actuator output
	int m_PID_BLKCNT;
	unsigned int m_loop_cnt;

} 
ST_DNST;



int init_my_sfwFunc_DNST( ST_STD_device *pSTDdev);
int create_DNST_taskConfig( ST_STD_device *pSTDdev);
void clear_DNST_taskConfig(void *pArg, double arg1, double arg2);



void threadFunc_DNST_DAQ(void *param);
void threadFunc_DNST_RingBuf(void *param);


void func_DNST_OP_MODE(void *, double, double);
void func_DNST_BOARD_SETUP(void *, double, double);
void func_DNST_SYS_ARMING(void *, double, double);
void func_DNST_SYS_RUN(void *, double , double );
void func_DNST_SAMPLING_RATE(void *, double , double );
void func_DNST_DATA_SEND(void *, double , double );
void func_DNST_SYS_RESET(void *, double , double );
void func_DNST_TEST_PUT(void *, double , double );



#endif /* drvDNST_H */

