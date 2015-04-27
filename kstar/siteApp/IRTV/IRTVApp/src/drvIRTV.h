#ifndef _IRTV_DRV_H
#define _IRTV_DRV_H

#include "sfwCommon.h"
#include "sfwMDSplus.h"
#include "sfwDriver.h"

#include "irtvCommon.h"
#include "irtvGrabStatus.h"

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------

enum {
	AI_TIMESTAMP,
	AI_MAX_RATE,
	AI_FPA_TEMP,
	AI_PRESET,
	AI_INT_TIME,
	AI_WIDTH,
	AI_HEIGHT,
	AI_FRAME_RATE,
	BI_STS_CONN,
	MBBI_SYNC_SRC,
	MBBI_SYNC_POLARITY,
	SI_SYNC_SRC_NAME,
	SI_SYNC_POLARITY_NAME,

	AO_GRAB_TIME,
	AO_ROI_X,
	AO_ROI_Y,
	AO_ROI_WIDTH,
	AO_ROI_HEIGHT,
	AO_ROI_LIFETIME,
	AO_ROI_COLOR,
	AO_REFRESH_TIME,
	AO_SYNC_SOURCE,
	BO_SOFT_TRIGGER,
	BO_LIVE_VIEW,
	BO_ROI_SAVE,
	BO_ROI_FULL_SCALE,
	BO_ROI_COORDI,
	BO_DAQ_STOP,
	BO_CONN_STATUS,
	BO_AGC_ENABLE,
	SO_IMAGE_PATH,

	// 2012
	WAVE_STORED_NUC_LIST,
	MBBO_FPA_COOLER_TYPE,
	MBBO_FPA_COOLER_AUTO,
	MBBO_NUC_ACTION_TYPE,
	MBBO_CAM_MODEL_TYPE,
	MBBI_NUC_ACTION_STATUS,
	AO_GAIN_LEVEL,
	AO_FPA_OFF_TEMP,
	AO_FPA_ON_TEMP,
	AO_FPA_INT_TIME,
	AO_SEL_PRESET_NUM,
	AI_NUC_MEM_FREE,
	AI_FPA_RATE,
	AI_FPA_GAIN,
	AI_FPA_WIN_X,
	AI_FPA_WIN_Y,
	AI_FPA_WIN_WIDTH,
	AI_FPA_WIN_HEIGHT,
	AI_ACTIVE_PRESET_NUM,
	AI_CONFIG_PRESET_NUM,
	BO_INVERT_ENABLE,
	BO_REVERT_ENABLE,
	BO_SYNC_POLARITY,
	BO_FPA_COOLER_ON,
	BO_SET_ACTIVE_PRESET,
	BO_PRESET_SEQ_MODE,
	BO_FULL_NUC_ENABLE,
	BO_NUC_ACTION_LOAD,
	BO_GET_STORED_NUCS,
	BI_PRESET_CYCLING,
	SO_SEL_NUC_NAME,
	SI_FPA_NUC_NAME
};

// input PV
#define AI_TIMESTAMP_STR			"ai_timestamp"
#define AI_MAX_RATE_STR				"ai_max_rate"
#define AI_FPA_TEMP_STR				"ai_sts_fpa_temp"
#define AI_PRESET_STR				"ai_sts_preset"
#define AI_INT_TIME_STR				"ai_sts_int_time"
#define AI_WIDTH_STR				"ai_sts_width"
#define AI_HEIGHT_STR				"ai_sts_height"
#define AI_FRAME_RATE_STR			"ai_sts_frame_rate"
#define BI_STS_CONN_STR				"bi_sts_conn"
#define MBBI_SYNC_SRC_STR			"mbbi_sts_sync_src"
#define MBBI_SYNC_POLARITY_STR		"mbbi_sts_sync_polarity"
#define SI_SYNC_SRC_NAME_STR		"si_sts_sync_src_name"
#define SI_SYNC_POLARITY_NAME_STR	"si_sts_sync_polarity_name"

// output PV
#define AO_GRAB_TIME_STR 		    "ao_grab_time"
#define AO_ROI_X_STR 		        "ao_roi_x"
#define AO_ROI_Y_STR 		        "ao_roi_y"
#define AO_ROI_WIDTH_STR 		    "ao_roi_width"
#define AO_ROI_HEIGHT_STR 		    "ao_roi_height"
#define AO_ROI_LIFETIME_STR 		"ao_roi_lifetime"
#define AO_ROI_COLOR_STR 		    "ao_roi_color"
#define AO_REFRESH_TIME_STR 	    "ao_refresh_time"
#define AO_SYNC_SOURCE_STR 		    "ao_sync_source"
#define BO_SOFT_TRIGGER_STR			"bo_soft_trigger"
#define BO_LIVE_VIEW_STR			"bo_live_view"
#define BO_ROI_SAVE_STR				"bo_roi_save"
#define BO_ROI_FULL_SCALE_STR		"bo_roi_full_scale"
#define BO_ROI_COORDI_STR			"bo_roi_coordi"
#define BO_DAQ_STOP_STR				"bo_daq_stop"
#define BO_AGC_ENABLE_STR			"bo_agc_enable"
#define SO_IMAGE_PATH_STR			"so_image_path"

// 2012
#define WAVE_STORED_NUC_LIST_STR	"wave_stored_nucs"
#define MBBO_FPA_COOLER_TYPE_STR	"mbbo_cooler_type"
#define MBBO_FPA_COOLER_AUTO_STR	"mbbo_cooler_auto"
#define MBBO_NUC_ACTION_TYPE_STR	"mbbo_nuc_action_type"
#define MBBO_CAM_MODEL_TYPE_STR		"mbbo_cam_model_type"
#define MBBI_NUC_ACTION_STATUS_STR	"mbbi_nuc_action_status"

#define AO_GAIN_LEVEL_STR			"ao_gain_level"
#define AO_FPA_OFF_TEMP_STR			"ao_fpa_off_temp"
#define AO_FPA_ON_TEMP_STR			"ao_fpa_on_temp"
#define AO_FPA_INT_TIME_STR			"ao_preset_int_time"
#define AO_SEL_PRESET_NUM_STR		"ao_sel_preset_num"
#define AI_NUC_MEM_FREE_STR			"ai_nuc_mem_free"
#define AI_FPA_RATE_STR				"ai_preset_rate"
#define AI_FPA_GAIN_STR				"ai_preset_gain"
#define AI_FPA_WIN_X_STR			"ai_preset_win_x"
#define AI_FPA_WIN_Y_STR			"ai_preset_win_y"
#define AI_FPA_WIN_WIDTH_STR		"ai_preset_win_width"
#define AI_FPA_WIN_HEIGHT_STR		"ai_preset_win_height"
#define AI_ACTIVE_PRESET_NUM_STR	"ai_active_preset_num"
#define AI_CONFIG_PRESET_NUM_STR	"ai_config_preset_num"

#define BO_INVERT_ENABLE_STR		"bo_invert_enable"
#define BO_REVERT_ENABLE_STR		"bo_revert_enable"
#define BO_SYNC_POLARITY_STR		"bo_sync_polarity"
#define BO_FPA_COOLER_ON_STR		"bo_cooler_on"
#define BO_SET_ACTIVE_PRESET_STR	"bo_set_active_preset"
#define BO_PRESET_SEQ_MODE_STR		"bo_preset_seq_mode"
#define BO_FULL_NUC_ENABLE_STR		"bo_full_nuc_enable"
#define BO_NUC_ACTION_LOAD_STR		"bo_nuc_action_load"
#define BO_GET_STORED_NUCS_STR		"bo_get_stored_nucs"
#define BI_PRESET_CYCLING_STR		"bi_preset_cycling"

#define SO_SEL_NUC_NAME_STR			"so_sel_nuc_name"
#define SI_FPA_NUC_NAME_STR			"si_preset_nuc_name"

// initial value
#define INIT_CHECK_INTERVAL			1       // Interval for Check the Configuration in run as second
#define INIT_SAMPLE_RATE			1		// Hz

#define CALIB_SAMPLE_RATE			30		// Hz

extern int getStartTime (ST_STD_device *pSTDdev);
extern int getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int resetDeviceAll ();

extern ST_IRTV *getDriver (ST_STD_device *pSTDdev);
extern ST_IRTV *getFirstDriver ();

extern void setStopT1 (ST_IRTV *pIRTV, float64 val);

#endif	// _IRTV_DRV_H
