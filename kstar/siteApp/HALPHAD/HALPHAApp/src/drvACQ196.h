#ifndef _NB1A_DRV_ACQ196_H
#define _NB1A_DRV_ACQ196_H

#define NO_RT_CH 0  // 0 - No channenlization mode, 
#define RT_CH 1  // 1 - On Real-Time channenlization mode, 

#include "statCommon.h"

#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for open() */

#define CLOCK_INTERNAL   0
#define CLOCK_EXTERNAL   1

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------
// There is no command for new tree
// When system is armed, tree is created automatically
#define	AUTO_CREATE_TREE			0			

// NBI only : if mode is local then SYS_RUN is not required
#define	AUTO_RUN				0			

// NBI only : SYS_ARMING (for trending), SYS_RUN(for calcuration & storing)
#define	TREND_ENABLE				1			

// converts scaling eu value to raw value of 16bits resolution
#define	USE_SCALING_VALUE			1			

#define BUFLEN  				0x400000    /* @@todo read from device */

// signal type
#define NBI_INPUT_TYPE_DT_STR			"Delta-T"	// Delta T
#define NBI_INPUT_TYPE_TC_STR			"T/C"		// Thermocouple

#define ECEHR_INPUT_TE_TYPE_STR			"ECEHR"		// Electron temperature
#define PI					3.141592	
#define ECE_ME					9.1e-31		// electron mess
#define ECE_E					1.602e-19	// electron charge
#define KSTAR_MU0				4*PI*1e-7	// permeability (N/A^2) of free space
#define KSTAR_TTN				56*16		// total turn number of KSTAR TF Coil(turn:56, TF coil 16)
#define KSTAR_RMIN				1.26		// inner radial limit of KSTAR Tokamak
#define KSTAR_RMAX				2.315		// outer radial limit of KSTAR Tokamak
 
#define DENSITY_CALC_STR			"Density_Calc"	// Density calc
#define MDS_PARAM_PUT_STR			"MDS_Param"	// MDS Parameter value put in MDSplus DB

#define NBI_POWER_CONST				0.0697
#define	NBI_TEMP_PER_VOLT			4	// 0.25V/.C

#define	WAVE_RAW_VALUE_STR			"wave_raw"
#define	WAVE_VALUE_STR				"wave_calc"
#define	WAVE_TE_VALUE_STR			"wave_te_val"
#define	WAVE_TE_POS_STR				"wave_te_pos"
// input PV
#define AI_READ_STATE_STR			"state"
#define	AI_RAW_VALUE_STR			"raw_value"
#define	AI_VALUE_STR				"value"
#define	AI_TE_VALUE_STR				"tevalue"
#define	AI_TE_POS1ST_STR			"tepos1st"
#define	AI_TE_POS2ND_STR			"tepos2nd"
#define	AI_TE_POS3RD_STR			"tepos3rd"
#define	AI_POWER_STR				"power"

#define	AI_DENSITY_STR				"density"
#define AI_DAQ_MIN_VOLT_R_STR                   "min_val_r"
#define AI_DAQ_MAX_VOLT_R_STR                   "max_val_r"
#define AI_DAQ_SAMPLING_TIME_R_STR              "smp_time_r"
#define AI_DAQ_SAMPLING_RATE_R_STR              "smp_rate_r"

#define AI_READ_STATE				1001
#define	AI_RAW_VALUE				1002
#define	AI_VALUE				1003
#define	AI_POWER				1004

#define AI_DAQ_MIN_VOLT_R                	1005
#define AI_DAQ_MAX_VOLT_R                	1006
#define AI_DAQ_SAMPLING_TIME_R                  1007
#define AI_DAQ_SAMPLING_RATE_R                  1008
#define	AI_DENSITY				1009
#define	AI_TE_VALUE				1010
#define	AI_TE_POS1ST				1011
#define	AI_TE_POS2ND				1012
#define	AI_TE_POS3RD				1013

#define	WAVE_VALUE				1020
#define	WAVE_RAW_VALUE				1021
#define	WAVE_TE_VALUE				1022
#define	WAVE_TE_POS				1023

// output PV
#define AO_INTEGRAL_TIME_STR	 		"integral_time"
#define AO_FLOW_STR			 	"flow"
// TG.Lee
#define AO_DAQ_MIN_VOLT_STR                     "min_val"
#define AO_DAQ_MAX_VOLT_STR                     "max_val"
#define AO_DAQ_SAMPLING_RATE_STR                "smp_rate"
#define AO_DAQ_SAMPLING_TIME_STR                "smp_time"

#define AO_DENSITY_FRINGE_STR			"fringe"
#define AO_DENSITY_LIMITE_UP_STR		"dlimup"
#define AO_DENSITY_LIMITE_LOW_STR		"dlimlow"
#define AO_DENSITY_CONSTANT_A_STR		"cona"
#define AO_DENSITY_CONSTANT_B_STR		"conb"
#define AO_DENSITY_CONSTANT_C_STR		"conc"
#define AO_DENSITY_CONSTANT_D_STR		"cond"
// MDSplus Node parameter value setting each ECH Channel
#define AO_MDS_PARAM_VALUE1_STR			"gain_ch"
#define AO_MDS_PARAM_VALUE2_STR			"offset_ch"
#define AO_MDS_PARAM_VALUE3_STR			"a_ch"
#define AO_MDS_PARAM_VALUE4_STR			"b_ch"
#define AO_MDS_PARAM_VALUE5_STR			"c_ch"

#define AO_DAQ_KSTAR_TF_CURRENT_STR             "tf_current"

#define BO_AUTO_RUN_STR                         "auto_run"
#define BO_TREND_RUN_STR			"trend_run"
#define BO_CALC_RUN_STR				"calc_run"
#define BO_DAQ_STOP_STR				"daq_stop"
#define BO_STORE_TO_MDS_STR			"select_store"

#define AO_DAQ_BEAM_PLUSE_T0                    1001
#define AO_DAQ_BEAM_PLUSE_T1                    1002
#define AO_DAQ_BEAM_PLUSE 			1003
#define AO_INTEGRAL_TIME	 		1101
#define AO_FLOW				 	1102
// TG.Lee
#define AO_DAQ_MIN_VOLT                         1103
#define AO_DAQ_MAX_VOLT                         1104
#define AO_DAQ_SAMPLING_RATE                    1105
#define AO_DAQ_SAMPLING_TIME                    1106

#define AO_DENSITY_FRINGE			1107	
#define AO_DENSITY_LIMITE_UP			1108
#define AO_DENSITY_LIMITE_LOW			1109
#define AO_DENSITY_CONSTANT_A			1110
#define AO_DENSITY_CONSTANT_B			1111
#define AO_DENSITY_CONSTANT_C			1112
#define AO_DENSITY_CONSTANT_D			1113
// MDSplus Node parameter value setting each ECH Channel
#define AO_MDS_PARAM_VALUE1			1114	
#define AO_MDS_PARAM_VALUE2			1115
#define AO_MDS_PARAM_VALUE3	   		1116
#define AO_MDS_PARAM_VALUE4			1117
#define AO_MDS_PARAM_VALUE5			1118
#define AO_DAQ_KSTAR_TF_CURRENT			1119

#define BO_AUTO_RUN                             3001
#define BO_TREND_RUN				3002
#define BO_CALC_RUN				3003
#define BO_DAQ_STOP				3004
#define BO_STORE_TO_MDS				3005

// PVs
#define PV_LOCAL_SHOT_NUMBER_STR 	"HALPHAD_SHOT_NUMBER"
#define PV_SYS_ARMING_STR 		"HALPHAD_SYS_ARMING"
#define PV_SYS_RUN_STR			"HALPHAD_SYS_RUN"
#define PV_LTU_TIG_T0_STR		"VBS_LTU_P2_SEC0_T0"
#define PV_START_TIG_T0_STR		"HALPHAD_START_T0_SEC1"
#define PV_STOP_TIG_T1_STR		"HALPHAD_STOP_T1_SEC1"
#define PV_RESET_STR 			"HALPHAD_RESET"

#define PV_CALC_RUN_STR 		"HALPHAD_CALC_RUN"
#define PV_TREND_RUN_STR 		"HALPHAD_TREND_RUN"
#define PV_DAQ_STOP_STR 		"HALPHAD_DAQ_STOP"

// PVs for ECH param
#define PV_ECH_POL_PH1_STR		"ECH_POL_ANG_PHI1_MDS"
#define PV_ECH_POL_PH2_STR		"ECH_POL_ANG_PHI2_MDS"
#define PV_ECH_ANT_POL_STR		"ECH_ANT_ANG_POL_MDS"
#define PV_ECH_ANT_TOR_STR		"ECH_ANT_ANG_TOR_MDS"

// initial value
#define INIT_CHECK_INTERVAL		1       /* Interval for Check the Configuration in run as second */
#define INIT_MAX_CHAN_NUM		96	/* ACQ196 96channel   */	
#define INIT_SAMPLE_RATE		100000		// HALPHAD default 100,000Hz
#define INIT_SAMPLE_RATE_LIMIT		500000		// Hz, for NI6220 Device. It's depends on device and channel 
#define INIT_MAX_CHAN_SAMPLES  		500000  /* first value : 100,000 but PXI-DAQ Max Rate : 500,000 Hz. so number of sampling data per channel  */
#define INIT_TOT_EVENT_SAMPLES		(INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

#define INIT_LOW_LIMIT				-10.0
#define INIT_HIGH_LIMIT				10.0
#define READ_TIME_OUT				10.0

#define	I16_FULL_SCALE				65536.0
#define	I14_FULL_SCALE				16384.0
#define EU_GRADIENT				((INIT_HIGH_LIMIT - INIT_LOW_LIMIT) / I16_FULL_SCALE)
#define EU_GRADIENT14				((INIT_HIGH_LIMIT - INIT_LOW_LIMIT) / I14_FULL_SCALE)
#define RAW_GRADIENT				(I16_FULL_SCALE / (INIT_HIGH_LIMIT - INIT_LOW_LIMIT))
#define RAW14_GRADIENT				(I14_FULL_SCALE / (INIT_HIGH_LIMIT - INIT_LOW_LIMIT))
#define GET_RAW_VALUE(x)			((x)*(RAW_GRADIENT))
#define GET_RAW14_VALUE(x)			((x)*(RAW14_GRADIENT))

#define NIERROR(errno)	NI_err_message(__FILE__,__LINE__,errno)
#define GET_CHID(idx)	(idx % INIT_MAX_CHAN_NUM)

#define MAX_RING_BUF_NUM            140
#define STR_CH_MAPPING_FILE    "/usr/local/epics/siteApp/HALPHA/acq196_ch_mapping"
#define STR_CH_VIN_FILE        "/usr/local/epics/siteApp/HALPHA/acq196_ch_vin"


typedef short	intype;					// input type

// content of inttype
enum {
	OTHER_INPUT_NONE	= 0,
	NBI_INPUT_DELTA_T	= 1,
	NBI_INPUT_TC		= 2,
	DENSITY_CALC		= 3,
	MDS_PARAM_PUT		= 4,
	ECEHR_INPUT_TE		= 5
};

typedef struct {
	int		used;		// 0 : Not used, 1 : Used

	int		inputType;	// type of input : 1(Delta-T), 2(T/C), 3(density Calc output Channel -if ch1 & ch2 -> only select mark ch1)
	char		physical_channel[40];	// Dev1/ai0
	float64		minVoltRange;
	float64		maxVoltRange;
	// for MDSplus
	int     	saveToMds;          // 0 : save to mdsplus DB, 1 : Do not save to mdsplus DB
	char		inputTagName[40];		// createChannel() in st.cmd
	char		strTagName[40];			// \B0_CH00:FOO
	char		file_name[40];			// B0_CH00
	char		path_name[100];			// directory path and file name for raw data
	char		path_den_name[100];     // directory path and file name for density data
	FILE		*write_fp;

	// input from DAQ
	intype		rawValue;

	// with OPI
	float64		flow;					// ao : PV.FLOW
	float64		integral_time;			// ao : PV.TIME
	float64		integral_cnt;			// integral_time * sample_rate
	float64		euValue;			// ai : PV.VAL
	float64		power;				// ai : PV.POWER

	// ECE HR factor
	unsigned long int targetFrq;				// ECE target frequency Hz= 1e9 * GHz 
	float64		calFactor;				// ECE calibration factor x raw = Te
	float64		posHar1st;				// ECE Position 1st harmonic = R (m)			
	float64		posHar2nd;				// ECE Position 2nd harmonic = R (m) - we used
	float64		posHar3rd;				// ECE Position 3rd harmonic = R (m)			
	float64		teValue;				// Electron templature : euValue * calFactor

	// Density
	float64		selectCh;				// last event calc density selectCh for remember channel 
	float64		isFirst;				// Calc function is first time ? check. default value is 0; 
	float64		density;				// current density value
	float64		fringe;				    // fringe reference value
	float64		dLimUpper;				// density Upper limite value 
	float64		dLimLow;				// density Low limite value
	float64		conA;					// density Constant A value
	float64		conB;					// density Constant B value
	float64		conC;					// density Constant C value
	float64		conD;					// density Constant D value
	float64		avgNES1;				// voltage average ch#1 - 1:1000
	float64		avgNES2;				// vlotage average ch#2 - 1:1000
	float64		calcDensity[INIT_MAX_CHAN_SAMPLES];	        // buffer for density data
	float64		fScaleReadChannel[INIT_MAX_CHAN_SAMPLES];	// buffer for scaling sampling data of channel data
	// Gain and offset of HighVoltage Probe for ECH DAQ system 20120202
	float64		mdsParam1; // gain
	float64		mdsParam2; // offset
	float64		mdsParam3; // calA
	float64		mdsParam4; // calB
	float64		mdsParam5; // calC
	// compensation
	int16		offset;					// base offset for compensation

	// temporary
	float64		accum_cnt;				// accumulate of integral time
	float64		accum_temp;				// accumulate of Delta-T(offsetCompRawValue)
	float64		coefficient;			// Q(lpm) * C1 / T(Pulse length)
} ST_ACQ196_channel;

typedef struct {
	uint16          slot;
        unsigned int    boardConfigure;

        int     	nChannelMask;
        char            clockSource;
        int 		clockDivider;
        int 		nCardMode;
	FILE		*writeRaw_fp;
	char		path_name[100];			// directory path and file name for raw data
	
	char		triggerSource[30];		// /Dev1/PFI0

	// SFW
	int32		smp_mode;
	float64		start_t0;			// $(SYS)_START_T0_SEC1
	float64		stop_t1;			// $(SYS)_STOP_T1_SEC1
	float64		sample_rate;
	float64		sample_rateLimit;
	float64		sample_time;			// stop_t1 - start_t0

	// DAQmx
	int32		terminal_config;		// DAQmx_Val_RSE, DAQmx_Val_Diff, DAQmx_Val_NRSE
	int32		units;
	float64		minVal;
	float64		maxVal;
	// ECE HR - Real calc position
	float64		currentTF;			// KSTAR MPS TF Current (kA) 
	float64		rtTe[INIT_MAX_CHAN_NUM];	// ECE rtTe for Waveform (eV) 
	float64		rtTePos[INIT_MAX_CHAN_NUM];	// ECE rtTePos for waveform (m) 

	// Event
	int32		totalRead;
	int32		ibuffRead;
	int32		nbuffRead;
	int32		nsamplesRead;
	float64		htimeRead;
	int		fileHandling;   		// file index for process data, It  is important

	int		auto_run_flag;			// DAQ start and stop flag with epicsEvent 
	int		data_acquire_flag;		// Data acquire start and stop flag with epicsEvent 

	ST_ACQ196_channel ST_Ch[INIT_MAX_CHAN_NUM];
	short		readArray[INIT_TOT_EVENT_SAMPLES];			// buffer for raw sampling data
	float64		fScaleReadArray[INIT_TOT_EVENT_SAMPLES];	// buffer for scaling sampling data

	ELLLIST		*data_buf; 
} ST_ACQ196;

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

extern float64 getTotSampsChan (ST_ACQ196 *pACQ196);
extern float64 getStartTime (ST_STD_device *pSTDdev);
extern float64 getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int processDensity (ST_ACQ196 *pACQ196, int chNo);
extern int resetDeviceAll ();

extern int processTrendRun ();
extern int processCalcRun ();

#endif	// _NB1A_DRV_ACQ196_H
