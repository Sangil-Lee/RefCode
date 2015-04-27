#ifndef PV_STRING_5412_H
#define PV_STRING_5412_H

/*

AI record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

*/

#define AI_SCOPE_STATUS                 1
#define AI_SCOPE_ERROR_STATUS                 2


#define AI_SCOPE_STATUS_STR         "status"
#define AI_SCOPE_ERROR_STATUS_STR                 "error"






/*
BO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
/* not related channel */
#define BO_AUTO_SETUP				101
#define BO_GET_ACTUAL_INFO		102
#define BO_SET_TRIG_IMMEDIATE		103
#define BO_INIT_ACQUITION			104
#define BO_ABORT_ACQUITION		105
#define BO_TREE_PUT					106
#define BO_MULTI_TRIGGER		107



#define	BO_AUTO_SETUP_STR			"auto_setup"
#define BO_GET_ACTUAL_INFO_STR		"get_info"
#define BO_SET_TRIG_IMMEDIATE_STR		"trig_now"
#define BO_INIT_ACQUITION_STR			"init_acq"
#define BO_ABORT_ACQUITION_STR		"abort_acq"
#define BO_TREE_PUT_STR				"tree_put"
#define BO_MULTI_TRIGGER_STR		"multi_trig"






/* relation with channel */
#define BO_READ_DATA				200
#define BO_FETCH_DATA				201
#define BO_FETCH_BINARY				202

#define BO_READ_DATA_STR		"read_data"
#define BO_FETCH_DATA_STR		"fetch_data"
#define BO_FETCH_BINARY_STR		"fetch_binary"


/*
AO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
/* related with channel */
#define	AO_TEST_PUT				100
#define	AO_INPUT_RANGE			101
#define	AO_OFFSET					102
#define	AO_PROBE_ATTENU			103
#define	AO_IMPEDANCE			104
#define AO_FILTER_TYPE			105
#define AO_FILTER_CUTOFF_FREQ		106
#define AO_FILTER_CENTER_FREQ		107
#define AO_FILTER_WIDTH			108
#define AO_MAX_FREQ			109



#define	AO_TEST_PUT_STR			"test_put"
#define	AO_INPUT_RANGE_STR		"range"
#define	AO_OFFSET_STR			"offset"
#define	AO_PROBE_ATTENU_STR		"probe"
#define	AO_IMPEDANCE_STR			"impedance"
#define AO_FILTER_TYPE_STR			"filter_type"
#define AO_FILTER_CUTOFF_FREQ_STR		"cutoff_freq"
#define AO_FILTER_CENTER_FREQ_STR		"center_freq"
#define AO_FILTER_WIDTH_STR			"filter_width"
#define AO_MAX_FREQ_STR			"max_freq"





/* no relation with channel */
#define	AO_SAMPLE_RATE			200
#define	AO_REC_LENGTH				201
#define AO_TRIG_MODE			202
#define AO_REC_NUM				203


#define	AO_SAMPLE_RATE_STR		"smp_rate"
#define	AO_REC_LENGTH_STR		"rec_len"
#define AO_TRIG_MODE_STR			"trig_mode"
#define AO_REC_NUM_STR			"rec_num"





/*
Stringout record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
/* related with channel */
#define	STRINGOUT_WF_FILENAME				100
#define STRINGOUT_TAG_NAME				101
#define STRINGOUT_TRIG_SRC					102

#define STRINGOUT_WF_FILENAME_STR		"wfm_name"
#define STRINGOUT_TAG_NAME_STR			"tag"
#define STRINGOUT_TRIG_SRC_STR			"trig_src"



#endif

