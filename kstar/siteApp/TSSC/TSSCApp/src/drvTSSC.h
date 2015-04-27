#ifndef drvTSSC_H
#define drvTSSC_H


#include "sfwDriver.h"
#include "dbAccess.h"   



#define STR_DEVICE_TYPE_1   "scVTX5"



/*
 *  Address of control register
 */
#define MAP_SIZE	65536UL
#define MAP_MASK 	(MAP_SIZE - 1)


#define C_BASEADDR			0x86000000

#define REG_START_STOP_OFFSET		0x00

#define REG_LASER_CLK_OFFSET		0x04
#define REG_LASER_WIDTH_OFFSET		0x08
#define REG_QSWITCH_DELAY_OFFSET		0x0C
#define REG_QSWITCH_WIDTH_OFFSET		0x10

#define REG_BACK_PULSE_DELAY_OFFSET		0x14
#define REG_BACK_PULSE_CNT_OFFSET		0x18
#define REG_BACK_PULSE_WIDTH_OFFSET		0x1C

#define REG_BTW_LTU_DG535_IN		0x20

#define REG_INPUT_PULSE_CNT           0x24
#define REG_LASER_OUT_PULSE_CNT           0x28
#define REG_Q_OUT_PULSE_CNT           0x2C

#define REG_GPIO_IN           0x30
#define REG_GPIO_OUT           0x34
#define REG_GPIO_DIR           0x38










typedef struct
{
	volatile unsigned int gpStartStop;			// 00
	
	volatile unsigned int gpLaserClkHz;			// 04
	volatile unsigned int gpLaserClkWidth;			// 08
	
	volatile unsigned int gpQSwitchDely;			// 0C
	volatile unsigned int gpQSwitchWidth;			// 10
	
	volatile unsigned int gpBackPulseDelay;			// 14
	volatile unsigned int gpBackPulseCnt;			// 18
	volatile unsigned int gpBackPulseWidth;			// 1C
	
	volatile unsigned int gpBtwLTU_dg535_input;			// 20
	
	volatile unsigned int gpRealPulseCnt;			// 24	
	volatile unsigned int gpLaserOutCnt;			// 28
	volatile unsigned int gpQswtOutCnt;			// 2C


	volatile unsigned int gpGPIO_IN;			// 30
	volatile unsigned int gpGPIO_OUT;			// 34
	volatile unsigned int gpGPIO_Direction;			// 38
	
}
ST_REG_MAP;


typedef struct {

	int  fd;
	ST_REG_MAP *pBaseAddr;

} 
ST_TSSC;



int init_my_sfwFunc_TSSC( ST_STD_device *pSTDdev);
int create_TSSC_taskConfig( ST_STD_device *pSTDdev);
void clear_TSSC_taskConfig(void *pArg, double arg1, double arg2);


void threadFunc_TSSC_DAQ(void *param);

void func_TSSC_OP_MODE(void *, double, double);
void func_TSSC_TEST_PUT(void *, double, double);
void func_TSSC_SYS_ARMING(void *, double, double);
void func_TSSC_SYS_RUN(void *, double , double );
void func_TSSC_DATA_SEND(void *, double , double );
void func_TSSC_SYS_RESET(void *, double , double );





/*****************************************/
/* application */



#endif /* drvTSSC_H */

