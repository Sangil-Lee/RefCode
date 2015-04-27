#ifndef _VBS_DRV_NI6250_H
#define _VBS_DRV_NI6250_H

#include "statCommon.h"

#define	VBS_SFW_TREE_USE			1

#if VBS_SFW_TREE_USE

// SEG-0
#include "sfwTree.h"

#define	STR_LOCAL_SEG_DATA_DIR		"/data/segdata/"
#define	VBS_SFW_TREE_STORE_RAW		1

#endif	// VBS_SFW_TREE_USE

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

#define AI_READ_STATE				1001
#define	AI_RAW_VALUE				1002
#define	AI_VALUE					1003

// output PV
#define AO_DAQ_MIN_VOLT_STR			"min_volt"
#define AO_DAQ_MAX_VOLT_STR			"max_volt"
#define AO_DAQ_GAIN_STR				"gain"
#define AO_DAQ_OFFSET_STR			"offset"
#define AO_DAQ_CUTOFF_FREQ_STR		"cutoff_freq"
#define BO_AUTO_RUN_STR				"auto_run"
#define BO_DAQ_STOP_STR				"daq_stop"

#define AO_DAQ_MIN_VOLT				2001
#define AO_DAQ_MAX_VOLT				2002
#define AO_DAQ_GAIN					2003
#define AO_DAQ_OFFSET				2004
#define AO_DAQ_CUTOFF_FREQ			2005
#define BO_AUTO_RUN					2101
#define BO_DAQ_STOP					2102

// PVs
#define PV_LOCAL_SHOT_NUMBER_STR 	"VBS_SHOT_NUMBER"
#define PV_SYS_RUN_STR 				"VBS_SYS_RUN"
#define PV_SYS_ARMING_STR 			"VBS_SYS_ARMING"
#define PV_RESET_STR 				"VBS_RESET"
#define PV_DAQ_STOP_STR 			"VBS_DAQ_STOP"

// initial value
#define INIT_CHECK_INTERVAL			1		/* Interval for Check the Configuration in run as second */
#define INIT_MAX_CHAN_NUM			8		/* Max Channel number of DAQ unit */
#define INIT_SAMPLE_RATE			10000	// Hz
#define INIT_MAX_CHAN_SAMPLES  		500000  /* PXI-6250 Max Rate : 500,000 Hz. so number of sampling data per channel  */
#define INIT_TOT_EVENT_SAMPLES		(INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

#define EU_MIN_VOLT					-10.0
#define EU_MAX_VOLT					10.0
#define INIT_LOW_LIMIT				-5.0	// SCXI (-5.0)
#define INIT_HIGH_LIMIT				5.0		// SCXI (5.0)
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

	// st.cmd
	int			inputType;				// type of input
	char		physical_channel[40];	// SC1Mod1/ai0

	// for MDSplus
	char		inputTagName[40];		// createChannel() in st.cmd
	char		strTagName[40];			// \B0_CH00:FOO
#if VBS_SFW_TREE_USE
	char		strTagNameSeg[40];		// \B0_CH00:RAW
	char		strTagNameScale[40];	// \B0_CH00:FBITS
#endif	// VBS_SFW_TREE_USE
	char		file_name[40];			// B0_CH00
	char		path_name[100];			// directory path and file name for raw data
	char		path_den_name[100];     // directory path and file name for density data
	FILE		*write_fp;

	// input from DAQ
	intype		rawValue;

	// for OPI
	float64		euValue;				// ai : PV.VAL

#if VBS_SFW_TREE_USE
	sfwTreeNodePtr	pSfwTreeNode;		// object for TreeNode
	sfwTreeNodePtr	pSfwTreeScale;		// object for TreeNode
#endif	// VBS_SFW_TREE_USE

} ST_NI6250_channel;

typedef struct {
	TaskHandle	taskHandle;
	char		triggerSource[30];		// /Dev1/PFI0

	// SFW
	int32		fault_ipmin;			// Ip minimum fault
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
	float64		cutoff_freq;			// cutoff frequency (Hz)
	float64		rawGradient;			// scale / (max - min)
	float64		euGradient;				// (max - min) / scale

	// Event
	int32		totalRead;

	int			auto_run_flag;			// 

	ST_NI6250_channel ST_Ch[INIT_MAX_CHAN_NUM];
	short		readArrayScan[INIT_TOT_EVENT_SAMPLES];		// buffer for raw sampling data
	short		readArray[INIT_TOT_EVENT_SAMPLES];			// buffer for raw sampling data
	float64		fScaleReadArray[INIT_TOT_EVENT_SAMPLES];	// buffer for scaling sampling data

#if VBS_SFW_TREE_USE
	sfwTreePtr	pSfwTree;				// object for Tree
#endif	// VBS_SFW_TREE_USE

	ELLLIST		*data_buf; 
} ST_NI6250;

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

extern float64 getTotSampsChan (ST_NI6250 *pNI6250);
extern float64 getTotAcqSampsChan (ST_NI6250 *pNI6250);
extern float64 getStartTime (ST_STD_device *pSTDdev);
extern float64 getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int resetDeviceAll ();

#endif	// _VBS_DRV_NI6250_H
