/****************************************************************************

Module      : CodacPcsRun.h

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: woong   2011-10-1


*****************************************************************************/

#ifndef __CODAC_PCS_RUN_H__
#define  __CODAC_PCS_RUN_H__

#include <cmath>

#include <asm/types.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//#include "shareLib.h"
//#include "epicsTypes.h"

#define MAX_ACCUM_DATA    300
#define USED_INPUT_CH   2
#define USED_ADC_CH   2  // 1 ~ 8 value

#define FRINGE		(0.2078)

#define INIT_PID_RUN_TIME      (10)   // sec.
#define INIT_PID_RUN_RATE      (1000)  // Hz
#define MAX_PID_CNT   1000000
#define INIT_INV_REF_SCALE   (1e-19)

/*#define INIT_BLIP_TIME  16 */
#define INIT_REF_SECT_CNT  5
#define INIT_START_TIME   (-5)
#define INIT_STOP_TIME   (10)



#define TYPE_A   0
#define TYPE_B   1

#define MAX_REF_SECTION   16


#ifndef ERROR
#define ERROR 1
#endif

#ifndef OK
#define OK 0
#endif


#define STATE_MAKE_PATTERN	(0x1 << 0)
#define STATE_PID_INIT	(0x1 << 1)
#define STATE_DENSITY_INIT	(0x1 << 2)
#define STATE_SYS_ARMING	(0x1 << 3)
#define STATE_SYS_RUN	(0x1 << 4)






typedef struct {
	double y2; /* density amplitude */
	double x2;  /* time unit : second */
	double a;
//	double delta_d;
//	int16 delta_hex; /* need plus 0x8000 */
	int matchingCnt; /* correct index with given section */
} ST_PATTERN;


//class epicsShareClass CODAC_Controller {
class CODAC_Controller {

	
public:
	CODAC_Controller();
	~CODAC_Controller();


	/*  PID operation       */
	void pid_initialize(double kp, double ki, double kd, double error_thresh, double step_time);
	int pid_load_refFile(const char *filename, int ab);
	double pid_getRefSigVoltage(__u32 cnt);
	double pid_getRefSigQuantized(__u32 cnt);
	double pid_processError(double ref, double H );
	double pid_update(double error);	
	int pid_set_sectEndTime(int sect, double time);
	int pid_set_sectEndDensity(int sect, double density);
	int pid_set_sectCounter(__u32 cnt);
	int pid_make_refPattern();
	double pid_get_mapping_time(__u32 cnt);
	void pid_set_enable(bool enable);
	bool pid_get_enable(); // global enable.
	bool pid_get_enable_cnt( __u32 cnt); // whether this cnt enable or not

	double pid_get_startTime() { return (fb_start_time); };
	void pid_set_startTime(double time);
	void pid_set_stopTime(double time);
	

	/* density calculation */
	void dnst_initialize(double a1,double b1,double c1,double d1,double a2,double b2,double c2,double d2, double limU, double limL);
	double dnst_process (int elofData, __u32 *aiBuf);
	double dnst_process_test (__u32 cnt);
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

	void save_log();	
	void save_ref_data();
	void show_internal_value();
	void sys_reset();
	
	


private:
	/**********************************/
	/* PCS operation                        */
	int m_bliptime;
	double fb_start_time;
	double fb_stop_time;
	double fb_period;
	double op_start_time;
	double op_stop_time;
//	__u32 m_op_freq;
	unsigned int m_status;
	
	/**********************************/
	/* relation with PID                         */
	bool m_PID_enable;
	bool m_started;
	double m_kp, m_ki, m_kd;
	double m_h;  /* 0.001 (second) */ 
	double m_inv_h, m_prev_error, m_error_thresh, m_integral;
	double m_curr_error;
	double m_curr_ref;
	double m_cmdU;
	__u32  m_maxRefCnt; // true number of Ref. data
	double *m_pRefData; // true reference data
	bool *m_pRefPidEnable; // true reference data information
	__u32 m_pidCnt; // PID update count

	ST_PATTERN ST_Ref_step[MAX_REF_SECTION];
//	int ref_step_time[MAX_REF_SECTION];
//	double ref_step_density[MAX_REF_SECTION];
	__u32 ref_section_cnt;

	/**********************************/
	/* density calculation                        */
	bool isFirst;
	__u32 curCH;
	double m_a1, m_b1, m_c1, m_d1, m_a2, m_b2, m_c2, m_d2; 
	double m_limU, m_limL;
	double m_density;
	double m_density_mean;
	double m_dtemp;
	double m_invDensityScale; //2012. 3.2 : (1e-19)
	int m_offlineABbit; // 2012. 3. 26

//	bool useOfflineDensity;
	__u16 **pOffline_rawData;
//	__u32  OffCnt;
	FILE *fpDensity;
	FILE *fpDMean;
	FILE *fpPIDlog;
	double offline_rawData[2][500000]; // temporary 2012.3.26 voltage value.


	double dnst_average_from_block(unsigned int *aiBuf, int EOB, int chNo);
	int open_message_file();
	__u32 dnst_check_data_cnt(const char *rawFile, int ab);
	int pid_find_section(__u32 cnt);

	
};

#endif	// __CODAC_PCS_RUN_H__

