#ifndef PV_STRING_H
#define PV_STRING_H


/*
BO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define BO_READ_ALL_DATA     1
#define BO_SHOW_STATUS1    2
#define BO_SHOW_STATUS2    3
#define BO_DEV_ARMING    4
#define BO_DEV_RUN    5
#define BO_TOGGLE_mean_value    6
#define BO_SHOW_MEAN_VALUE   7
#define BO_SHOW_CALIB_INFO   8
#define BO_TOGGLE_make_file 9
#define BO_TOGGLE_realtime_calc 10








#define BO_READ_ALL_DATA_STR     "read_once"
#define BO_SHOW_STATUS1_STR    "status_reg1"
#define BO_SHOW_STATUS2_STR    "status_reg2"
#define BO_DEV_ARMING_STR    "dev_arm"
#define BO_DEV_RUN_STR    "dev_run"
#define BO_TOGGLE_mean_value_STR    "mean_on"
#define BO_SHOW_MEAN_VALUE_STR    "mean_value"
#define BO_SHOW_CALIB_INFO_STR   "cal_show"
#define BO_TOGGLE_make_file_STR   "file_save"
#define BO_TOGGLE_realtime_calc_STR "rt_calc"









/*
AI record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  AI_READ_STATE	   1
#define  AI_RAW_CUR_VALUE	   2
#define  AI_RAW_MEAN_VALUE	   3
#define  AI_Te_VALUE	   4
#define  AI_ne_VALUE	   5
#define  AI_RAW_BG_MEAN_VALUE	   6







#define  AI_READ_STATE_STR			"state"
#define  AI_RAW_CUR_VALUE_STR	   "raw_cur"
#define  AI_RAW_MEAN_VALUE_STR	   "raw_mean"
#define  AI_Te_VALUE_STR	   "Te"
#define  AI_ne_VALUE_STR	   "ne"
#define  AI_RAW_BG_MEAN_VALUE_STR	   "bg_mean"







/*
AO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/


#define  AO_TEST_PUT			101
#define AO_SET_PEDESTAL    102
#define AO_SET_CALB_ITERATION   103
#define AO_SET_CALB_INCREASE_CNT   104
#define AO_SET_STOP_TIME   105
#define AO_CHANNEL_MASK   106





#define  AO_TEST_PUT_STR				"test_put"
#define AO_SET_PEDESTAL_STR    "pedestal"
#define AO_SET_CALB_ITERATION_STR   "cal_iter"
#define AO_SET_CALB_INCREASE_CNT_STR   "cal_inc"
#define AO_SET_STOP_TIME_STR   "stop_seq"
#define AO_CHANNEL_MASK_STR   "mask"






/*
Stringout record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  STRINGOUT_TAG					1


#define  STRINGOUT_TAG_STR				"tag"



#endif

