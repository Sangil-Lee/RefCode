/****************************************************************************

Module      : CodacPcsRunStub.h

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: woong   2011-10-1


*****************************************************************************/

#include "CodacPcsRun.h"
#include "CodacPcsRunStub.h"

CODAC_Controller	*gpController = NULL;

void init_controller()
{
	printf ("init_controller() in ...\n");

	CODAC_Controller	*pController = new CODAC_Controller();

	if (NULL == pController) {
		printf ("cannot create CODAC_Controller \n");
		return;
	}

	gpController = pController;

	printf ("init_controller() ok ...\n");
}



void pid_initialize(double kp, double ki, double kd, double error_thresh, double step_time)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return;
	}
	
	gpController->pid_initialize (kp, ki, kd, error_thresh, step_time);
}

double pid_getRefSigVoltage(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return -1;
	}
	return gpController->pid_getRefSigVoltage( cnt);
}

double pid_getRefSigQuantized(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return -1;
	}
	return gpController->pid_getRefSigQuantized( cnt);
}
double pid_processError(double ref, double H )
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return -1;
	}
	return gpController->pid_processError(ref, H);
}
double pid_update(double error)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return -1;
	}
	return gpController->pid_update(error);
}
int pid_set_sectEndTime(int sect, double time)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->pid_set_sectEndTime(sect, time);
}

int pid_set_sectEndDensity(int sect, double density)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->pid_set_sectEndDensity(sect, density);
}

int pid_set_sectCounter(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->pid_set_sectCounter(cnt);
}

int pid_make_refPattern()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->pid_make_refPattern();

}
double pid_get_mapping_time(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->pid_get_mapping_time(cnt);
}
void pid_set_enable(int enable)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->pid_set_enable(enable);
}
int pid_get_enable() // global enable.
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return (int)gpController->pid_get_enable();
}
int pid_get_enable_cnt( __u32 cnt) // whether this cnt enable or not
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return (int)gpController->pid_get_enable_cnt(cnt);
}

double pid_get_startTime()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->pid_get_startTime();
}
void pid_set_startTime(double time)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->pid_set_startTime(time);
}
void pid_set_stopTime(double time)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->pid_set_stopTime(time);
}
	

	/* density calculation */
void dnst_initialize(double a1,double b1,double c1,double d1,double a2,double b2,double c2,double d2, double limU, double limL)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->dnst_initialize(a1, b1, c1, d1, a2, b2, c2, d2, limU, limL);
}
double dnst_process (int elofData, __u32 *aiBuf)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_process(elofData, aiBuf);
}
int dnst_load_dataFile(const char *rawFile, int ab)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_load_dataFile(rawFile, ab);
}
int dnst_read_dataFile(const char *rawFile, int ab)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_read_dataFile(rawFile, ab);
}
__u32 dnst_getData_ch0(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_getData_ch0(cnt);
}

__u32 dnst_getData_ch1(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_getData_ch1(cnt);
}

double dnst_getRaw_ch0(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_getRaw_ch0(cnt);

}
double dnst_getRaw_ch1(__u32 cnt)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->dnst_getRaw_ch1(cnt);
}

	/* miscellaneous */
void op_memoryLock(int size)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->op_memoryLock(size);
}
void op_set_bliptime(int blip)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->op_set_bliptime(blip);
}
void op_set_dummy_raw()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ;
	}
	gpController->op_set_dummy_raw();
}
int op_get_bliptime()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->op_get_bliptime();
}

double op_get_voltage(__u32 raw)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->op_get_voltage(raw);
}
__u32 op_get_maxRefCnt()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->op_get_maxRefCnt();
}
void op_set_startTime(double time)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return;
	}
	gpController->op_set_startTime(time);
}
double op_get_startTime()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->op_get_startTime();
}
void op_set_stopTime(double time)
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return;
	}
	gpController->op_set_stopTime(time);
}
double op_get_stopTime()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return ERROR;
	}
	return gpController->op_get_stopTime();
}


void show_internal_value()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return;
	}
	gpController->show_internal_value();
}
void sys_reset()
{
	if (NULL == gpController) {
		printf ("object is null\n");
		return;
	}
	gpController->sys_reset();
}



