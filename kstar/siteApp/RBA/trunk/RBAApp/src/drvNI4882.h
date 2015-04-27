#ifndef _RBA_DRV_NI4882_H
#define _RBA_DRV_NI4882_H

#include "statCommon.h"
#include "RBA.h"

//#include <ni4882.h>

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------

// input PV

// output PV
#define BO_AMP_RESET_STR			"amp_reset"
#define BO_AMP_START_STR			"amp_start"
#define BO_AMP_GAIN_GET_STR			"amp_gain_get"
#define BO_AMP_GAIN_SET_STR			"amp_gain_set"
#define BO_AMP_FLT_GET_STR			"amp_flt_get"
#define BO_AMP_FLT_SET_STR			"amp_flt_set"
#define BO_AMP_T_GET_STR			"amp_t_get"

#define BO_AMP_RESET				2001
#define BO_AMP_START				2002
#define BO_AMP_GAIN_GET				2101
#define BO_AMP_GAIN_SET				2102
#define BO_AMP_FLT_GET				2103
#define BO_AMP_FLT_SET				2104
#define BO_AMP_T_GET				2105

#define PV_AMP_RESET_STR 			"RBA_AMP_RESET"
#define PV_AMP_START_STR 			"RBA_AMP_START"

// initial value
#define NI4882_MAX_MCU				3		// Max MCU
#define NI4882_MAX_CHAN				4		// Max Channel number of MCU
#define NI4882_BOARD_IDX			0		// Board Index
#define NI4882_TIME_OUT				T1s		// Timeout
#define NI4882_ASSERT_EOI			1		// Assert EOI line at end of write
#define NI4882_MODE_EOS				0		// EOS termination mode
#define NI4882_MAX_RETRY			1		// Retry count

#define NI4882_INIT_GAIN			8		// 5000
#define NI4882_INIT_FILTER			1		// 200Hz

#define NI4882_EU_MIN_VOLT			-10.0
#define NI4882_EU_MAX_VOLT			10.0
#define	NI4882_I16_FULL_SCALE		65536.0
#define NI4882_EU_GRADIENT			((NI4882_EU_MAX_VOLT - NI4882_EU_MIN_VOLT) / NI4882_I16_FULL_SCALE)
#define NI4882_RAW_GRADIENT			(NI4882_I16_FULL_SCALE / (NI4882_EU_MAX_VOLT - NI4882_EU_MIN_VOLT))

#define AMP_CMD_RESET				2	// Reset all devices
#define AMP_CMD_START				3	// Start a defined procedure
#define AMP_CMD_T1					18	// Get value from temperature sensor
#define AMP_CMD_SG_MIN				20	// Set gain. 1
#define AMP_CMD_SG_MAX				27	// Set gain. 8
#define AMP_CMD_SF_MIN				28	// Set filter. 1
#define AMP_CMD_SF_MAX				31	// Set filter. 4
#define AMP_CMD_GF					83	// Get filter
#define AMP_CMD_GG					84	// Get gain

typedef struct {
	int			chNo;
	int			gain;
	int			filter;
	float64		temp;					// temperature
	float64		euValue;				// ai : PV.VAL
} ST_NI4882_CH;

typedef struct {
	int			used;					// 0 : Not used, 1 : Used

	int			device;					// Device unit descriptor
	int			boardIdx;				// Board Index
	int			priAddr;				// Primary Address
	int			secAddr;				// Secondary Address
	int			timeout;				// Timeout

	ST_NI4882_CH	ST_CH[NI4882_MAX_CHAN];

} ST_NI4882_MCU;

typedef struct {
	int			taskHandle;				// Device unit descriptor

	float64		rawGradient;			// scale / (max - min)
	float64		euGradient;				// (max - min) / scale

	int			auto_run_flag;			// 
	ST_NI4882_MCU ST_MCU[NI4882_MAX_MCU];

	ELLLIST		*data_buf; 
} ST_NI4882;

extern int processAmpReset ();
extern int processAmpStart ();
extern int processGetAmpTemp ();
extern int processGetAmpGain ();
extern int processSetAmpGain ();
extern int processGetAmpFilter ();
extern int processSetAmpFilter ();

extern ST_NI4882_MCU *getGpibMCU (int nMCU);
extern ST_NI4882_CH *getGpibChannel (int nMCU, int nChannel);
extern int getAmpGainValue (int idx);
extern int getAmpFilterValue (int idx);

extern ST_NI4882	*pNI4882;

#endif	// _RBA_DRV_NI4882_H
