#include "sfwDriver.h"
#include "sfwMDSplus.h"
#include "sfwInitControl.h"

void func_xxx_OP_MODE(void *, double, double);
void func_xxx_BOARD_SETUP(void *, double, double);
void func_xxx_SYS_ARMING(void *, double, double);
void func_xxx_SYS_RUN(void *, double , double );
void func_xxx_SAMPLING_RATE(void *, double , double );
void func_xxx_DATA_SEND(void *, double , double );

/********************************************************************************
!! Important.  this function name is not changable. must keep "initControl" !!!
*********************************************************************************/
void initControl(const char *name)
/********************************************************************************/
{
	ST_MASTER *pMaster = get_master();

	pMaster->ST_Func._OP_MODE = func_xxx_OP_MODE;
	pMaster->ST_Func._BOARD_SETUP = func_xxx_BOARD_SETUP;
	pMaster->ST_Func._SYS_ARMING= func_xxx_SYS_ARMING;
	pMaster->ST_Func._SYS_RUN = func_xxx_SYS_RUN;
	pMaster->ST_Func._DATA_SEND = func_xxx_DATA_SEND;

//	pMaster->ST_Func._Exit_Call = func_xxx_Exit_Call;
}

/*
	OPMODE_INIT
	OPMODE_REMOTE 
	OPMODE_LOCAL
	OPMODE_CALIBRATION
	
	TASK_STANDBY 
	TASK_ARM_ENABLED 
	TASK_WAIT_FOR_TRIGGER 
	TASK_IN_PROGRESS 
	TASK_POST_PROCESS 
	TASK_DATA_TRANSFER  
	TASK_SYSTEM_IDLE 

	//pMaster->ST_Base.xxx
	unsigned short     opMode;
        unsigned long int  shotNumber;
        float              fBlipTime;
        unsigned int       nSamplingRate;
        double             dT0[SIZE_CNT_MULTI_TRIG];
        double             dT1[SIZE_CNT_MULTI_TRIG];
        double             dRunPeriod; // unit: seconds 

*/

void func_xxx_OP_MODE(void *pArg, double ignore, double ignore1)
{
	ST_MASTER *pMaster = (ST_MASTER *)pArg;
	printf("[myApp] OP_MODE of master: %d\n", pMaster->ST_Base.opMode);
	
	//TODO
	// don't need status setting manually. system transit to "Standby" automatically.

}

void func_xxx_BOARD_SETUP(void *pArg, double ignore, double ignore1)
{
	printf("[myApp] receive BOARD_SETUP command.\n");

	//TODO

}

void func_xxx_SYS_ARMING(void *pArg, double arg1, double ignore1)
{
	printf("[myApp] SYS_ARMING with arg1: %f\n",  arg1);
	//TODO


	set_master_status_manually( TASK_ARM_ENABLED, (uint32)arg1);
}

void func_xxx_SYS_RUN(void *pArg, double arg1, double ignore)
{
	printf("[myApp] SYS_RUN with arg1: %f\n",  arg1);
	//TODO


	set_master_status_manually( TASK_IN_PROGRESS, (uint32)arg1);
}

void func_xxx_DATA_SEND(void *pArg, double ignore, double ignore1)
{
	printf("[myApp] receive DATA_SEND command.\n");
	//TODO
	// don't need status setting manually.

}





