#ifndef drvDio24_H
#define drvDio24_H

#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>


#include "dio24_api.h"



#include "sfwDriver.h"



#define STR_DEVICE_TYPE_3   "Dio24"


typedef struct {

	void *dev;


	uint32 portA_io; /* DIO24_GPIO_DIRECTION_INPUT				0 */
	uint32 portB_io; /* DIO24_GPIO_DIRECTION_OUTPUT				1*/
	uint32 portC_io;
	uint32 portCX_io[8];

	

} 
ST_Dio24;



int init_my_SFW_Dio24( ST_STD_device *pSTDdev);


int create_Dio24_taskConfig( ST_STD_device *pSTDdev);
void clear_Dio24_taskConfig(void *pArg, double arg1, double arg2);



/*ST_threadCfg*  make_controlThreadConfig(ST_Dio24 *pdio24); */



void func_Dio24_BOARD_SETUP(void *, double, double);
void func_Dio24_SYS_ARMING(void *, double, double);
void func_Dio24_SYS_RUN(void *, double , double );



#endif /* drvDio24_H */

