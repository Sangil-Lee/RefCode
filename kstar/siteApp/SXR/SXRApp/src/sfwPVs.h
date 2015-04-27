#ifndef _SWF_PVS_H
#define _SWF_PVS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
#define BI_SYS_READY     1
#define BI_SYS_READY_STR   "sys_ready"
*/

#define MBBI_ADMIN_STATUS     1
#define MBBI_ADMIN_STATUS_STR   "status"
#define MBBI_ADMIN_ERROR     2
#define MBBI_ADMIN_ERROR_STR   "error"




#define BO_AUTO_SAVE		1
#define BO_SYS_ARMING       2
#define BO_SYS_RUN       3
#define BO_DATA_SEND       4
#define BO_SYS_RESET       5
#define BO_BOARD_SETUP   6

#define BO_AUTO_SAVE_STR	"auto_save"
#define BO_SYS_ARMING_STR    "arm_enable"
#define BO_SYS_RUN_STR    "sys_run"
#define BO_DATA_SEND_STR       "snd_data"
#define BO_SYS_RESET_STR       "sys_reset"
#define BO_BOARD_SETUP_STR   "hw_setup"



/*
#define AI_SYS_STATUS		1
#define AI_SYS_STATUS_STR		"status"
*/


#define  AO_OP_MODE		1
#define  AO_SET_BLIP		2
#define  AO_CREATE_LOCAL_SHOT		3
#define  AO_SHOT_NUMBER		4
#define  AO_SAMPLING_RATE		5
#define  AO_START_T0		6
#define  AO_STOP_T1		7




#define  AO_OP_MODE_STR			"set_mode"
#define  AO_SET_BLIP_STR		"set_blip"
#define  AO_CREATE_LOCAL_SHOT_STR		"create_shot"
#define  AO_SHOT_NUMBER_STR			"shot_num"
#define  AO_SAMPLING_RATE_STR		"sample_rate"
#define  AO_START_T0_STR		"start_t0"
#define  AO_STOP_T1_STR		"stop_t1"



#define STRINGIN_ERROR_STRING   1
#define STRINGIN_ERROR_STRING_STR   "error_str"




#ifdef __cplusplus
}
#endif

#endif






