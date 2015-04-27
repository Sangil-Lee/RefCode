#ifndef drvDummy_H
#define drvDummy_H

#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>


#include "sfwDriver.h"



#define STR_DEVICE_TYPE_1   "Dummy"


typedef struct {

	void *dev;


	uint32 portA_io; /* DIO24_GPIO_DIRECTION_INPUT				0 */
	uint32 portB_io; /* DIO24_GPIO_DIRECTION_OUTPUT				1*/
	uint32 portC_io;
	uint32 portCX_io[8];

	

} 
ST_Dummy;



int init_my_SFW_Dummy( ST_STD_device *pSTDdev);


int create_Dummy_taskConfig( ST_STD_device *pSTDdev);
void clear_Dummy_taskConfig(void *pArg, double arg1, double arg2);



/*ST_threadCfg*  make_controlThreadConfig(ST_Dummy *pDummy); */
void threadFunc_Dummy_DAQ(void *param);



void func_Dummy_BOARD_SETUP(void *, double, double);
void func_Dummy_SYS_ARMING(void *, double, double);
void func_Dummy_SYS_RUN(void *, double , double );



#endif /* drvDummy_H */

