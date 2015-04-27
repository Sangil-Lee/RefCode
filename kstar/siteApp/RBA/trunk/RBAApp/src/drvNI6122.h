#ifndef _RBA_DRV_NI6122_H
#define _RBA_DRV_NI6122_H

#include "statCommon.h"
#include "RBA.h"

#include <NIDAQmx.h>

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------

// testing when there is no digitizer : ex) MDSplus
#define	TEST_WITHOUT_DAQ			0

// converts scaling eu value to raw value of 16bits resolution
#define	USE_SCALING_VALUE			1

#define BUFLEN  					0x400000    /* @@todo read from device */

// input PV
#define AI_READ_STATE_STR			"state"
#define	AI_RAW_VALUE_STR			"raw_value"
#define	AI_VALUE_STR				"value"
#define MBBI_AMP_STAT_STR			"amp_stat"
#define	MBBI_DIG_STAT_STR			"dig_stat"
#define	MBBI_MDS_STAT_STR			"mds_stat"

#define AI_READ_STATE				1001
#define	AI_RAW_VALUE				1002
#define	AI_VALUE					1003
#define MBBI_AMP_STAT				1101
#define MBBI_DIG_STAT				1102
#define MBBI_MDS_STAT				1103

// output PV
#define AO_DAQ_MIN_VOLT_STR			"min_volt"
#define AO_DAQ_MAX_VOLT_STR			"max_volt"
#define AO_DAQ_GAIN_STR				"gain"
#define AO_DAQ_OFFSET_STR			"offset"
#define BO_AUTO_RUN_STR				"auto_run"
#define BO_DAQ_STOP_STR				"daq_stop"
#define BO_CH_ENABLE_STR			"enable"

#define AO_DAQ_MIN_VOLT				2001
#define AO_DAQ_MAX_VOLT				2002
#define AO_DAQ_GAIN					2003
#define AO_DAQ_OFFSET				2004
#define BO_AUTO_RUN					2101
#define BO_DAQ_STOP					2102
#define BO_CH_ENABLE				2103

// PVs
#define PV_LOCAL_SHOT_NUMBER_STR 	"RBA_SHOT_NUMBER"
#define PV_SYS_RUN_STR 				"RBA_SYS_RUN"
#define PV_SYS_ARMING_STR 			"RBA_SYS_ARMING"
#define PV_RESET_STR 				"RBA_RESET"
#define PV_DAQ_STOP_STR 			"RBA_DAQ_STOP"
#define PV_AMP_STAT_STR 			"RBA_AMP_STAT"
#define PV_DIG_STAT_STR 			"RBA_DIG_STAT"
#define PV_MDS_STAT_STR 			"RBA_MDS_STAT"

// initial value
#define INIT_CHECK_INTERVAL			1		/* Interval for Check the Configuration in run as second */
#define INIT_MAX_CHAN_NUM			4		/* Max Channel number of DAQ unit */
#define INIT_SAMPLE_RATE			10000	// Hz
//TODO
#define INIT_MAX_CHAN_SAMPLES  		500000  /* PXI-6122 Max Rate : 500,000 Hz. so number of sampling data per channel  */
#define INIT_TOT_EVENT_SAMPLES		(INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

#define EU_MIN_VOLT					-10.0
#define EU_MAX_VOLT					10.0
#if 0	//TODO
#define INIT_LOW_LIMIT				-5.0	// SCXI (-5.0)
#define INIT_HIGH_LIMIT				5.0		// SCXI (5.0)
#else
#define INIT_LOW_LIMIT				-10.0
#define INIT_HIGH_LIMIT				10.0
#endif
#define INIT_GAIN					1.0
#define READ_TIME_OUT				10.0

#define	I16_FULL_SCALE				65536.0
#define EU_GRADIENT					((EU_MAX_VOLT - EU_MIN_VOLT) / I16_FULL_SCALE)
#define RAW_GRADIENT				(I16_FULL_SCALE / (EU_MAX_VOLT - EU_MIN_VOLT))

#define NIERROR(errno)				NI_err_message(__FILE__,__LINE__,errno)
#define GET_CHID(idx)				(idx % INIT_MAX_CHAN_NUM)

typedef short	intype;					// input type

// content of inttype
enum {
	INPUT_TYPE_NONE	= 0
};

typedef struct {
	int			used;					// 0 : Not used, 1 : Used
	int			enabled;				// 0 : Disabled, 1 : Enabled

	// st.cmd
	int			inputType;				// type of input
	char		physical_channel[40];	// SC1Mod1/ai0

	// for MDSplus
	char		inputPvName[64];		// createChannel() in st.cmd
	char		inputTagName[40];		// createChannel() in st.cmd
	char		strTagName[40];			// \B0_CH00:FOO
	char		file_name[40];			// B0_CH00
	char		path_name[100];			// directory path and file name for raw data
	char		path_den_name[100];     // directory path and file name for density data
	FILE		*write_fp;

	// input from DAQ
	intype		rawValue;

	// for OPI
	float64		euValue;				// ai : PV.VAL

} ST_NI6122_channel;

typedef struct {
	int			nAmpDeviceIdx;			// index for NI4882
	TaskHandle	taskHandle;
	char		triggerSource[30];		// /Dev1/PFI0

	// SFW
	int32		smp_mode;
	float64		start_t0;				// $(SYS)_START_T0_SEC1
	float64		stop_t1;				// $(SYS)_STOP_T1_SEC1
	float64		sample_rate;
	float64		sample_time;			// stop_t1 - start_t0

	// DAQmx
	int32		terminal_config;		// DAQmx_Val_RSE, DAQmx_Val_Diff, DAQmx_Val_NRSE
	int32		units;
	float64		minVal;
	float64		maxVal;
	float64		gain;
	float64		offset;
	float64		rawGradient;			// scale / (max - min)
	float64		euGradient;				// (max - min) / scale

	// Event
	int32		totalRead;
	int32		callbackRate;

	int			auto_run_flag;			// 

	ST_NI6122_channel ST_Ch[INIT_MAX_CHAN_NUM];
	short		readArrayScan[INIT_TOT_EVENT_SAMPLES];		// buffer for raw sampling data
	short		readArray[INIT_TOT_EVENT_SAMPLES];			// buffer for raw sampling data
	float64		fScaleReadArray[INIT_TOT_EVENT_SAMPLES];	// buffer for scaling sampling data

	ELLLIST		*data_buf; 
} ST_NI6122;

/******* make for RING buffer   ************************************/
typedef struct {
	ELLNODE			node;
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	char			data[ BUFLEN ];  
} ST_buf_node;

typedef struct {
	ST_buf_node		*pNode;
	unsigned int	size;
	char			opcode;
} ST_User_Buff_node;
/****************************************************/

extern float64 getTotSampsChan (ST_NI6122 *pNI6122);
extern float64 getStartTime (ST_STD_device *pSTDdev);
extern float64 getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int resetDeviceAll ();

extern int isFirstDevice (ST_STD_device *pSTDdev);

#endif	// _RBA_DRV_NI6122_H
