#ifndef drvXXX_H
#define drvXXX_H


#include "sfwDriver.h"
#include "dbAccess.h"   



#define STR_DEVICE_TYPE_1   "XXXtype"

/*
 *  Address of control register
 */

/******* make for RING buffer   ************************************/
#define MAX_RING_BUF_NUM            12
#define MAX_CUTOFF_BUF_NODE         4
/*#define BUFLEN	(800)  */ /* @@todo read from device */
#define BUFLEN	(80)   /* 2012. 2. 6 */	/* if clock is 10KHz, then 1KHz event  generated */


typedef struct {
	ELLNODE			node;
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	__u32 data[ BUFLEN ];  
} ST_buf_node;

typedef struct {
	ST_buf_node	*pNode;
	unsigned int		u32Cnt;
	unsigned int		size;
	char				opcode;

} ST_User_Buf_node;
/****************************************************/



typedef struct {
	/* add your device dependent parameters */

	int  fd;
	

} 
ST_XXX;



int init_my_sfwFunc_XXX( ST_STD_device *pSTDdev);
int create_XXX_taskConfig( ST_STD_device *pSTDdev);
void clear_XXX_taskConfig(void *pArg, double arg1, double arg2);


void threadFunc_XXX_DAQ(void *param);

void func_XXX_OP_MODE(void *, double, double);
void func_XXX_BOARD_SETUP(void *, double, double);
void func_XXX_SAMPLING_RATE(void *, double , double );
void func_XXX_TEST_PUT(void *, double, double);
void func_XXX_SYS_ARMING(void *, double, double);
void func_XXX_SYS_RUN(void *, double , double );
void func_XXX_DATA_SEND(void *, double , double );
void func_XXX_SYS_RESET(void *, double , double );


#endif /* drvXXX_H */

