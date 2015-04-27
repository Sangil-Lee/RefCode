#ifndef _NB1A_DRV_NI6254_H
#define _NB1A_DRV_NI6254_H

#include "kutil.h"
#include "nbiCommon.h"
#include "myMDSplus.h"

#include <NIDAQmx.h>

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------
// There is no command for new tree
// When system is armed, tree is created automatically
#define	AUTO_CREATE_TREE			1			

// NBI only : if mode is local then SYS_RUN is not required
#define	AUTO_RUN					1			

// NBI only : SYS_ARMING (for trending), SYS_RUN(for calcuration & storing)
#define	TREND_ENABLE				1			

// converts scaling eu value to raw value of 16bits resolution
#define	USE_SCALING_VALUE			1			

#define BUFLEN  					0x400000    /* @@todo read from device */

// signal type
#define NBI_INPUT_TYPE_DT_STR		"Delta-T"	// Delta T
#define NBI_INPUT_TYPE_TC_STR		"T/C"		// Thermocouple

// Terminal config
#define NBI_TERM_CFG_DIFF_STR		"DIFF"		// Differencial
#define NBI_TERM_CFG_RSE_STR		"RSE"		// RSE
#define NBI_TERM_CFG_NRSE_STR		"NRSE"		// NRSE

#define NBI_POWER_CONST				0.0697
#define	NBI_TEMP_PER_VOLT			4	// 0.25V/.C

// input PV
#define AI_READ_STATE_STR			"state"
#define	AI_RAW_VALUE_STR			"raw_value"
#define	AI_VALUE_STR				"value"
#define	AI_POWER_STR				"power"
#define	AI_FLOW_STR					"flow"

#define AI_READ_STATE				1
#define	AI_RAW_VALUE				2
#define	AI_VALUE					3
#define	AI_POWER					4
#define	AI_FLOW						5

// output PV
#define AO_DAQ_BEAM_PULSE_STR 		"beam_pulse"
#define AO_INTEGRAL_STIME_STR	 	"integral_stime"
#define AO_INTEGRAL_TIME_STR	 	"integral_time"
#define BO_TREND_RUN_STR			"trend_run"
#define BO_CALC_RUN_STR				"calc_run"
#define BO_DAQ_STOP_STR				"daq_stop"

#define AO_DAQ_BEAM_PULSE 			1001
#define AO_INTEGRAL_STIME	 		1002
#define AO_INTEGRAL_TIME	 		1101
#define BO_TREND_RUN				3002
#define BO_CALC_RUN					3003
#define BO_DAQ_STOP					3004

// PVs
#define PV_LOCAL_SHOT_NUMBER_STR 	"NB1_LODAQ_SHOT_NUMBER"

// initial value
#define INIT_CHECK_INTERVAL			1       /* Interval for Check the Configuration in run as second */
#define INIT_MAX_CHAN_NUM			32		/* Max Channel number of DAQ unit    */
#define INIT_SAMPLE_RATE			10		// Hz
#define INIT_MAX_CHAN_SAMPLES  		100000  /* number of sampling data per channel  */
#define INIT_TOT_EVENT_SAMPLES		(INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

#define DAQ_CALLBACK_RATE			10		// Hz

#define INIT_LOW_LIMIT				-10.0
#define INIT_HIGH_LIMIT				10.0
#define READ_TIME_OUT				10.0

#define	I16_FULL_SCALE				65536.0
#define EU_GRADIENT					((INIT_HIGH_LIMIT - INIT_LOW_LIMIT) / I16_FULL_SCALE)
#define RAW_GRADIENT				(I16_FULL_SCALE / (INIT_HIGH_LIMIT - INIT_LOW_LIMIT))
#define GET_RAW_VALUE(x)			((x)*(RAW_GRADIENT))

#define NIERROR(errno)	NI_err_message(__FILE__,__LINE__,errno)
#define GET_CHID(idx)	(idx % INIT_MAX_CHAN_NUM)

typedef short	intype;					// input type

// content of inttype
enum {
	NBI_INPUT_NONE		= 0,
	NBI_INPUT_DELTA_T	= 1,
	NBI_INPUT_TC		= 2
};

typedef struct {
	int			snapShot;				// 1 : do snap shot
	int			currShot;				// 
} ST_User_Data;

#define MAX_IS_NUM	10

typedef struct {
	float64		beam_pulse;				// $(SYS)_BEAM_PULSE
	float64		integral_stime;			// $(SYS)_INT_STIME
} ST_IS_Setup;

typedef struct {
	int			used;					// 0 : Not used, 1 : Used

	// st.cmd
	int			inputType;				// type of input : 1(Delta-T), 2(T/C)
	int			termCfg;				// type of terminal : DAQmx_Val_RSE, DAQmx_Val_Diff
	char		physical_channel[40];	// Dev1/ai0
	char		flowPVName[40];			// PV name for flow

	// for MDSplus
	char		inputTagName[40];		// createChannel() in st.cmd
	char		strTagName[40];			// \B0_CH00:FOO
	char		file_name[40];			// B0_CH00
	char		path_name[100];
	FILE		*write_fp;

	// input from DAQ
	intype		rawValue;

	// with OPI
	float64		flow;					// ao : PV.FLOW
	float64		integral_time;			// ao : PV.TIME
	float64		integral_ecnt;			// integral_time * sample_rate. end count
	float64		euValue;				// ai : PV.VAL
	float64		euVolt;					// Volt
	float64		power;					// ai : PV.POWER
	int			ionSourceNum;			// Ion Source Number

	// compensation
	int16		offset;					// base offset for compensation

	// temporary
	float64		sample_cnt;				// accumulate of integral time
	float64		accum_cnt;				// accumulate of integral time
	float64		accum_temp;				// accumulate of Delta-T(offsetCompRawValue)
	float64		accum_flow;				// accumulate of flow
	float64		beam_pulse;				// $(SYS)_BEAM_PULSE
	float64		integral_stime;			// $(SYS)_INT_STIME
	float64		integral_scnt;			// integral_stime * sample_rate
} ST_NI6254_channel;

typedef struct {
	TaskHandle	taskHandle;
	char		triggerSource[30];		// /Dev1/PFI0

	// SFW
	int32		smp_mode;
	float64		start_t0;				// $(SYS)_START_T0_SEC1
	float64		stop_t1;				// $(SYS)_STOP_T1_SEC1
	float64		sample_rate;
	int32		sample_time;			// stop_t1 - start_t0

	// DAQmx
	int32		terminal_config;		// DAQmx_Val_RSE, DAQmx_Val_Diff, DAQmx_Val_NRSE
	int32		units;
	float64		minVal;
	float64		maxVal;

	// Event
	int32		totalRead;
	int32		callbackRate;

	int			auto_run_flag;			// 

	ST_NI6254_channel ST_Ch[INIT_MAX_CHAN_NUM];
	short		readArray[INIT_TOT_EVENT_SAMPLES];			// buffer for raw sampling data
	float64		fScaleReadArray[INIT_TOT_EVENT_SAMPLES];	// buffer for scaling sampling data

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

extern int getTotSampsChan (ST_NI6254 *pNI6254);
extern float64 getStartTime (ST_STD_device *pSTDdev);
extern float64 getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int resetDeviceAll ();

extern int processTrendRun ();
extern int processCalcRun ();

extern ST_User_Data	*getUserData();
extern void setUserSnapShot (int shot);
extern int getUserSnapShot ();
extern void setUserCurrShot (int enable);
extern int getUserCurrShot ();

extern ST_IS_Setup	gIonSourceSetup[];

#endif	// _NB1A_DRV_NI6254_H
