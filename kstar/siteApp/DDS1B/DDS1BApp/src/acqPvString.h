#ifndef PV_STRING_H
#define PV_STRING_H


/*
BO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define BO_CH_MASK      1
#define BO_CH_MASK_STR      "mask"

#define BO_CLOCK_MODE   2
#define BO_CLOCK_MODE_STR   "clk_mode"

#define BO_INIT_FILE   3
#define BO_INIT_FILE_STR   "init_file"




#define MBBI_DEV_STATUS    1
#define MBBI_DEV_STATUS_STR   "dev_status"


/*
AI record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
#define  AI_READ_GAIN	   (0x00000001<<0)
#define  AI_READ_VAL	   (0x00000001<<1)
#define  AI_READ_STATE	   (0x00000001<<2)
#define  AI_PARSING_CNT		(0x001<<3)
#define  AI_MDS_SND_CNT		(0x001<<4)
#define  AI_DAQING_CNT		(0x001<<5)


#define  AI_READ_GAIN_STR			"gain"
#define  AI_READ_VAL_STR			"val"
#define  AI_READ_STATE_STR			"state"
#define  AI_PARSING_CNT_STR		"pars_cnt"
#define  AI_MDS_SND_CNT_STR			"snd_cnt"
#define  AI_DAQING_CNT_STR		"daqing_cnt"



/*
AO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  AO_SAMPLING_RATE			101
#define  AO_GAIN					102
#define  AO_MASK				103
#define  AO_DEV_ARMING			104
#define  AO_DEV_RUN			105
#define  AO_LOCAL_STORAGE		106
#define  AO_REMOTE_STORAGE		107
#define  AO_R_ONOFF				108
#define  AO_M_ONOFF				109
#define  AO_L_ONOFF				110
#define  AO_SET_INTERCLK			111
#define  AO_SET_EXTERCLK			112
#define  AO_START_TIME					113
#define  AO_RUN_PERIOD					114
#define  AO_TEST_PUT			115
#define  AO_SET_CLEAR_STATUS		116
#define  AO_MAXDMA			117
#define  AO_CLOCK_DIV			118
#define AO_CARD_MODE		119
#define  AO_STOP_TIME					120


#define  AO_SAMPLING_RATE_STR				"smpl_rate"
#define  AO_GAIN_STR   				"ch_gain"
#define  AO_MASK_STR					"ch_useNum"
#define  AO_DEV_ARMING_STR   		"adc_start"
#define  AO_DEV_RUN_STR   		"daq_start"
#define  AO_LOCAL_STORAGE_STR   	"local_save"
#define  AO_REMOTE_STORAGE_STR   	"remote_save"

#define  AO_R_ONOFF_STR					"chR_onoff"
#define  AO_M_ONOFF_STR					"chM_onoff"
#define  AO_L_ONOFF_STR					"chL_onoff"
#define  AO_SET_INTERCLK_STR					"internal_clk"
#define  AO_SET_EXTERCLK_STR					"external_clk"
#define  AO_START_TIME_STR					"start_time"
#define  AO_RUN_PERIOD_STR					"run_period"
#define  AO_TEST_PUT_STR				"test_put"
#define  AO_SET_CLEAR_STATUS_STR		"clear_status"
#define  AO_MAXDMA_STR					"maxdma"
/* 
2009. 08.31  should be use single CLK/TRIG line 
make master and slave mode
*/
#define  AO_CLOCK_DIV_STR			"divider"			
#define AO_CARD_MODE_STR		"card_mode"
#define  AO_STOP_TIME_STR					"stop_time"



/*
Stringout record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  STRINGOUT_TAG					1
#define  STRINGOUT_IP_PORT				2
#define  STRINGOUT_TREE_NAME			3


#define  STRINGOUT_TAG_STR				"tag"
#define  STRINGOUT_IP_PORT_STR			"ip_port"
#define  STRINGOUT_TREE_NAME_STR		"tree_name"



#endif

