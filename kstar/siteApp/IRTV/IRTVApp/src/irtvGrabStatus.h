#ifndef _IRTV_GRAB_STATUS_H
#define _IRTV_GRAB_STATUS_H

#include "dbScan.h"		// for IOSCANPVT

#ifdef __cplusplus
extern "C" {
#endif

#define	MAX_STRINGIN_SIZE				40		// size of val for stringRecord
#define	MAX_STORED_NUC					100		// maximum of stored NUCs
#define	MAX_PRESET_NUM					4		// maximum number of preset

typedef struct {
	// SFW
	double		start_t0;				// $(SYS)_START_T0_SEC1
	double		stop_t1;				// $(SYS)_STOP_T1_SEC1
	double		sample_rate;
	double		sample_time;			// stop_t1 - start_t0
	int			shotNumber;

	// IRTV
	int 		bo_soft_trigger;		// $(SYS)_SOFT_TRIGGER
	int 		bo_live_view;			// $(SYS)_LIVE_VIEW
	double		ao_sync_source;			// $(SYS)_SYNC_SOURCE
	double		ao_grab_time;			// $(SYS)_GRAB_TIME
	double		ao_refresh_time;		// $(SYS)_REFRESH_TIME
	double		ao_roi_x;				// $(SYS)_ROI_X
	double		ao_roi_y;				// $(SYS)_ROI_Y
	double		ao_roi_width;			// $(SYS)_ROI_WIDTH
	double		ao_roi_height;			// $(SYS)_ROI_HEIGHT
	double		ao_roi_lifetime;		// $(SYS)_ROI_LIFETIME
	double		ao_roi_color;			// $(SYS)_ROI_COLOR
	double		bo_roi_coordi;			// $(SYS)_ROI_COORDI
	double		bo_roi_save;			// $(SYS)_ROI_SAVE
	double		bo_roi_full_scale;		// $(SYS)_ROI_FULL_SCALE
	char		so_image_path[256];		// $(SYS)_IMAGE_PATH
	double		ai_timestamp;			// $(SYS)_TIMESTAMP
	double		ai_max_rate;			// $(SYS)_MAX_RATE

	double		ai_sts_fpa_temp;		// $(SYS)_STATUS_FPA_TEMP
	double		ai_sts_preset;			// $(SYS)_STATUS_PRESET
	double		ai_sts_int_time;		// $(SYS)_STATUS_INT_TIME
	double		ai_sts_x;				// $(SYS)_STATUS_X
	double		ai_sts_y;				// $(SYS)_STATUS_Y
	double		ai_sts_width;			// $(SYS)_STATUS_WIDTH
	double		ai_sts_height;			// $(SYS)_STATUS_HEIGHT
	double		ai_sts_frame_rate;		// $(SYS)_STATUS_FRAME_RATE
	int			bi_sts_conn;			// $(SYS)_STATUS_CONN
	double		mbbi_sts_sync_src;		// $(SYS)_STATUS_SYNC_SRC
	double		mbbi_sts_sync_polarity;	// $(SYS)_STATUS_SYNC_POLARITY

	//
	int			ao_gain_level;			// $(SYS)_GAIN_LEVEL
	int			bo_invert_enable;		// $(SYS)_INVERT_ENABLE
	int			bo_revert_enable;		// $(SYS)_REVERT_ENABLE
	int			bo_sync_polarity;		// $(SYS)_SYNC_POLARITY
	int			bo_cooler_on;			// $(SYS)_FPA_COOLER_ON
	int			mbbo_cooler_type;		// $(SYS)_FPA_COOLER_TYPE
	int			mbbo_cooler_auto;		// $(SYS)_FPA_COOLER_AUTO
	double		ao_fpa_off_temp;		// $(SYS)_FPA_OFF_TEMP
	double		ao_fpa_on_temp;			// $(SYS)_FPA_ON_TEMP
	int			ao_sel_preset_num;		// $(SYS)_SEL_PRESET_NUM : 설정을 위해 선택된 Preset #
	double		ai_active_preset_num;	// $(SYS)_ACTIVE_PRESET_NUM
	double		ai_config_preset_num;	// $(SYS)_ACTIVE_PRESET_NUM
	int			bo_set_active_preset;	// $(SYS)_SET_ACTIVE_PRESET
	int			bo_preset_seq_mode;		// $(SYS)_PRESET_SEQ_MODE
	int			bi_preset_cycling;		// $(SYS)_PRESET_CYCLING
	int			bo_full_nuc_enable;		// $(SYS)_FULL_NUC_ENABLE
	int			mbbo_nuc_action_type;	// $(SYS)_NUC_ACTION_TYPE
	int			bo_nuc_action_load;		// $(SYS)_NUC_ACTION_LOAD
	int			mbbi_nuc_action_status;	// $(SYS)_NUC_ACTION_STATUS
	double		ai_nuc_mem_free;		// $(SYS)_NUC_MEM_FREE
	int			bo_get_stored_nucs;		// $(SYS)_GET_STORED_NUCS
	char		so_sel_nuc_name[256];	// $(SYS)_SEL_NUC_NAME
	int			mbbo_cam_model_type;	// $(SYS)_CAM_MODEL_TYPE

	int			num_stored_nucs;		// number of stored nucs
	char		wave_stored_nucs[MAX_STORED_NUC][MAX_STRINGIN_SIZE];	// $(SYS)_STORED_NUC_LIST

	// Preset selection
	double		ai_preset_rate[MAX_PRESET_NUM];
	double		ai_preset_gain[MAX_PRESET_NUM];
	double		ao_preset_int_time[MAX_PRESET_NUM];	// $(SYS)_FPA_INT_TIME#
	double		ai_preset_win_x[MAX_PRESET_NUM];
	double		ai_preset_win_y[MAX_PRESET_NUM];
	double		ai_preset_win_width[MAX_PRESET_NUM];
	double		ai_preset_win_height[MAX_PRESET_NUM];
	char		si_preset_nuc_name[MAX_PRESET_NUM][MAX_STRINGIN_SIZE];	// $(SYS)_FPA_NUC_NAME#

	//
	int			bInitSetting;			// 카메라 설정값 로드 완료 여부
	int			bInitNUCs;				// NUC 정보 로드 완료 여부
	int			bRunGrabFrame;			// 1(Run)
	int			bStoreGrabFrame;		// 1(Store)
	int			nGrabFrameCnt;			// 이미지 취득 개수

	// Event
	int			totalRead;

	// EPICS Resources
	IOSCANPVT		ioScanPvt_userCall;	// scan io
	epicsEventId	threadEventId;		// event id
	epicsMutexId	grabVideoLock;		// mutex lock id
} ST_IRTV;

extern ST_IRTV *getFirstDriver ();

#ifdef __cplusplus
}
#endif

#endif	// _IRTV_GRAB_STATUS_H
