/****************************************************************************

Module      : CodacPcsRunStub.h

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: woong   2011-10-1


*****************************************************************************/

#ifndef __CODAC_PCS_RUN_STUB_H__
#define  __CODAC_PCS_RUN_STUB_H__

//#include "CodacPcsRun.h"
#include <asm/types.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif



#define C_A1    (0.20790)
#define C_B1    (-0.01320)
#define C_C1    (0.25360)
#define C_D1    (0.08940)

#define C_A2    (0.20790)
#define C_B2    (-0.01280)
#define C_C2    (0.23920)
#define C_D2    (0.07640)

#define LMT_U   (0.1550)
#define LMT_L   (0.0450)




void init_controller();


/*  PID operation       */
	void pid_initialize(double kp, double ki, double kd, double error_thresh, double step_time);
	double pid_getRefSigVoltage(__u32 cnt);
	double pid_getRefSigQuantized(__u32 cnt);
	double pid_processError(double ref, double H );
	double pid_update(double error);	
	int pid_set_sectEndTime(int sect, double time);
	int pid_set_sectEndDensity(int sect, double density);
	int pid_set_sectCounter(__u32 cnt);
	int pid_make_refPattern();
	double pid_get_mapping_time(__u32 cnt);
	void pid_set_enable(int enable);
	int pid_get_enable(); // global enable.
	int pid_get_enable_cnt( __u32 cnt); // whether this cnt enable or not

	double pid_get_startTime();
	void pid_set_startTime(double time);
	void pid_set_stopTime(double time);
	

	/* density calculation */
	void dnst_initialize(double a1,double b1,double c1,double d1,double a2,double b2,double c2,double d2, double limU, double limL);
	double dnst_process (int elofData, __u32 *aiBuf);
	int dnst_load_dataFile(const char *rawFile, int ab);
	int dnst_read_dataFile(const char *rawFile, int ab);	
	__u32 dnst_getData_ch0(__u32 cnt);
	__u32 dnst_getData_ch1(__u32 cnt);

	double dnst_getRaw_ch0(__u32 cnt);
	double dnst_getRaw_ch1(__u32 cnt);

	/* miscellaneous */
	void op_memoryLock(int size);
	void op_set_bliptime(int blip);
	void op_set_dummy_raw();
	int op_get_bliptime();
	double op_get_voltage(__u32 raw);
	__u32 op_get_maxRefCnt();
	void op_set_startTime(double time);
	double op_get_startTime();
	void op_set_stopTime(double time);
	double op_get_stopTime();


	void show_internal_value();
	void sys_reset();
	

	
#ifdef __cplusplus
}
#endif

#endif	// __CODAC_PCS_RUN_STUB_H__

