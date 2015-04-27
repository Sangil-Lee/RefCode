#ifndef _LHCD_DIAG_DRV_NI6254_H
#define _LHCD_DIAG_DRV_NI6254_H

#include "statCommon.h"

#include <NIDAQmx.h>

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------

// testing when there is no digitizer : ex) MDSplus
#define	TEST_WITHOUT_DAQ			0

#define BUFLEN  					0x400000    /* @@todo read from device */

// input PV
#define AI_READ_STATE_STR			"state"
#define	AI_RAW_VALUE_STR			"raw_value"
#define	AI_VALUE_STR				"value"
#define	AI_CALC_VALUE_STR			"calc_value"
#define	AI_VOLT_VALUE_STR			"volt_value"
#define	MBBI_DIG_STAT_STR			"dig_stat"
#define	MBBI_MDS_STAT_STR			"mds_stat"

#define AI_READ_STATE				1001
#define	AI_RAW_VALUE				1002
#define	AI_VALUE					1003
#define	AI_CALC_VALUE				1004
#define	AI_VOLT_VALUE				1005
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
#define PV_LOCAL_SHOT_NUMBER_STR 	"LHCD_DIAG_SHOT_NUMBER"
#define PV_SYS_RUN_STR 				"LHCD_DIAG_SYS_RUN"
#define PV_SYS_ARMING_STR 			"LHCD_DIAG_SYS_ARMING"
#define PV_RESET_STR 				"LHCD_DIAG_RESET"
#define PV_DAQ_STOP_STR 			"LHCD_DIAG_DAQ_STOP"

// initial value
#define INIT_CHECK_INTERVAL			1		/* Interval for Check the Configuration in run as second */
#define INIT_MAX_CHAN_NUM			16		/* Max Channel number of DAQ unit */
#define INIT_SAMPLE_RATE			50000	// Hz
//TODO
#define INIT_MAX_CHAN_SAMPLES  		100000  /* 1MHz. so number of sampling data per channel  */
#define INIT_TOT_EVENT_SAMPLES		(INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

#define EU_MIN_VOLT					-10.0
#define EU_MAX_VOLT					10.0
#define INIT_LOW_LIMIT				-10.0
#define INIT_HIGH_LIMIT				10.0
#define INIT_GAIN					1.0
#define READ_TIME_OUT				10.0

#define	I16_FULL_SCALE				65536.0
#define EU_GRADIENT					((EU_MAX_VOLT - EU_MIN_VOLT) / I16_FULL_SCALE)
#define RAW_GRADIENT				(I16_FULL_SCALE / (EU_MAX_VOLT - EU_MIN_VOLT))

#define NIERROR(errno)				NI_err_message(__FILE__,__LINE__,errno)

typedef short	intype;					// input type

// content of inttype
enum {
	INPUT_TYPE_NONE	= 0
};

typedef struct {
	int			used;					// 0 : Not used, 1 : Used
	int			enabled;				// 0 : Disabled, 1 : Enabled
	float64		gain;					// gain
	float64		offset;					// offset
	int			convType;				// 0(None), 1(G), 11(alpha), 12(beta)

	// st.cmd
	int			inputType;				// type of input
	char		physical_channel[40];	// SC1Mod1/ai0

	// for MDSplus
	char		inputTagName[40];		// createChannel() in st.cmd
	char		strTagName[40];			// \B0_CH00:FOO
	char		file_name[40];			// B0_CH00
	char		path_name[100];			// directory path and file name for raw data
	FILE		*write_fp;				// value

	// input from DAQ
	intype		rawValue;				// Raw value : 16bits
	float64		euVolt;					// EU
	float64		euValue;				// for OPI. ax + b
	float64		calcValue;				// Calcuration

	float64		fVoltArray[INIT_MAX_CHAN_SAMPLES];		// volt
	float64		fEuArray  [INIT_MAX_CHAN_SAMPLES];		// eu
	float64		fCalcArray[INIT_MAX_CHAN_SAMPLES];		// calc

} ST_NI6254_channel;

typedef struct {
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
	float64		rawGradient;			// scale / (max - min)
	float64		euGradient;				// (max - min) / scale

	// Event
	int32		totalRead;
	int32		callbackRate;

	int			auto_run_flag;			// 

	ST_NI6254_channel ST_Ch[INIT_MAX_CHAN_NUM];
	short		readArray[INIT_TOT_EVENT_SAMPLES];			// buffer for raw sampling data
	float64		fScaleReadArray[INIT_TOT_EVENT_SAMPLES];	// buffer for scaling sampling data

	// Calcuration
	float64		maxAmpl;				// max amplitude

	ELLLIST		*data_buf; 
} ST_NI6254;

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

extern float64 getTotSampsChan (ST_NI6254 *pNI6254);
extern float64 getStartTime (ST_STD_device *pSTDdev);
extern float64 getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int resetDeviceAll ();

#endif	// _LHCD_DIAG_DRV_NI6254_H
