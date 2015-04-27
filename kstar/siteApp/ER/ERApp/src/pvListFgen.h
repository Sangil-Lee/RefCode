#ifndef PV_STRING_fgen_H
#define PV_STRING_fgen_H


/*
BO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
#define BO_ABORT_GEN				1
#define BO_SIG_GEN					2
#define BO_FUNC_GEN					3
#define BO_RESET_INIT					4





#define BO_ABORT_GEN_STR				"abort_gen"
#define BO_SIG_GEN_STR				"gen"
#define BO_FUNC_GEN_STR					"func_gen"
#define BO_RESET_INIT_STR					"reset"




/*

AI record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

*/

#define AI_ERROR_CODE			1

#define AI_ERROR_CODE_STR		"error_code"


/*
AO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define	CONTROL_TEST_PUT				100
#define CONTROL_SET_OUTPUT_MODE			101
#define CONTROL_SET_CLOCK_SRC			102
#define CONTROL_ARB_SAMPLE_RATE			103
#define CONTROL_SET_CLOCK_MODE			104
/*#define CONTROL_SET_GENERATION			105 */


#define CONTROL_SET_WFM_TYPE			106
#define CONTROL_SET_AMPLITUDE			107
#define CONTROL_SET_DCOFFSET			108
#define CONTROL_SET_FREQUENCY			109
#define CONTROL_SET_PHASE				110
#define CONTROL_CFG_STD_WF				111
#define CONTROL_CFG_ARB_WF				112
#define CONTROL_SET_OUTPUT_ENABLE		113
#define CONTROL_CREATE_WF_DOWNLOAD		114

#define AO_ARB_CH_GAIN			115
#define AO_ARB_CH_OFFSET		116
#define AO_TRIG_MODE			117
#define AO_TRIG_TYPE			118

#define AO_EXPORT_SIG_IN		119
#define AO_EXPORT_SIG_TO		120






#define	CONTROL_TEST_PUT_STR			"test_put"
#define	CONTROL_SET_OUTPUT_MODE_STR		"output_mode"
#define CONTROL_SET_CLOCK_SRC_STR		"clk_source"
#define CONTROL_ARB_SAMPLE_RATE_STR		"smpl_rate"
#define CONTROL_SET_CLOCK_MODE_STR		"clk_mode"
/*#define CONTROL_SET_GENERATION_STR		"gen" */

#define CONTROL_SET_WFM_TYPE_STR		"wfm_type"
#define CONTROL_SET_AMPLITUDE_STR		"amp"
#define CONTROL_SET_DCOFFSET_STR		"dc_offset"
#define CONTROL_SET_FREQUENCY_STR		"freq"
#define CONTROL_SET_PHASE_STR			"phase"
#define CONTROL_CFG_STD_WF_STR			"std_wf"
#define CONTROL_CFG_ARB_WF_STR			"arb_wf"
#define CONTROL_CREATE_WF_DOWNLOAD_STR		"create_wf_dl"
#define CONTROL_SET_OUTPUT_ENABLE_STR	"out_enable"

#define AO_ARB_CH_GAIN_STR		"arb_chgain"
#define AO_ARB_CH_OFFSET_STR		"arb_offset"
#define AO_TRIG_MODE_STR			"trig_mode"
#define AO_TRIG_TYPE_STR			"trig_type"

#define AO_EXPORT_SIG_IN_STR		"export_in"
#define AO_EXPORT_SIG_TO_STR		"export_to"



/*
Stringout record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define	CONTROL_WF_FILENAME		1
#define STRINGOUT_TRIG_SRC			2
#define STRINGOUT_EXPORT_SIG_TO	3

#define CONTROL_WF_FILENAME_STR		"wfm_name"
#define STRINGOUT_TRIG_SRC_STR		"trig_src"
#define STRINGOUT_EXPORT_SIG_TO_STR		"export_to"



#endif

