#ifndef PV_LIST_H
#define PV_LIST_H



/*
Binary Output record....
*/
#define  BO_SHOT_START		1
#define  BO_SETUP			2
#define  BO_SHOW_INFO			3
#define  BO_PORT_ENABLE			4
#define  BO_PORT_ActiveLow			5
#define  BO_PORT_MODE			6
#define  BO_SHOT_END		7
#define  BO_IRIGB_ENABLE		8
#define  BO_SET_FREE_RUN		9
#define BO_RUN_CALIBRATION	      10
#define BO_USE_REF_CLK	      11







	
	
#define BO_SHOT_START_STR		"shot_start"
#define BO_SETUP_STR			"setup"
#define  BO_SHOW_INFO_STR		"show_info"
#define  BO_PORT_ENABLE_STR			"enable"
#define  BO_PORT_ActiveLow_STR			"active_low"
#define  BO_PORT_MODE_STR			"mode"
#define  BO_SHOT_END_STR		"shot_end"
#define  BO_IRIGB_ENABLE_STR		"irig_b"
#define  BO_SET_FREE_RUN_STR		"set_free"
#define BO_RUN_CALIBRATION_STR	      "set_cal"
#define BO_USE_REF_CLK_STR	      "use_ref"





/*
Analog Input record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  AI_CLTU_MCLOCK	   		1
#define  AI_CLTU_MCLOCK_STR		"mclock"

#define  AI_CLTU_TIME	   			2
#define  AI_CLTU_TIME_STR		"my_time"

#define  AI_CLTU_OFFSET	   		3
#define  AI_CLTU_OFFSET_STR		"offset"

#define  AI_CLTU_ID	   			4
#define  AI_CLTU_ID_STR		"id_name"

#define  AI_GET_COMP_TICK	   			5
#define  AI_GET_COMP_TICK_STR		"comp_tick"



/*
Analog Output record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  AO_SHOT_TERM			1
#define  AO_CLOCK_ONSHOT		2

#define  AO_PUT_CALED_VAL	3

#define  AO_TRIG			4
#define  AO_CLOCK		5
#define  AO_T0			6
#define  AO_T1			7
#define  AO_MTRIG		8
#define  AO_MCLOCK		9
#define  AO_MT0			10
#define  AO_MT1			11
#define  AO_SHOT_NUMBER		12

#define AO_SECTION_CLK     13
#define AO_SECTION_T0     14
#define AO_SECTION_T1     15

#define AO_GAP_R1R2     16





#define AO_SHOT_TERM_STR   	"shot_term"
#define AO_CLOCK_ONSHOT_STR	"clkOnShot"

#define AO_PUT_CALED_VAL_STR		"put_cal"

#define AO_TRIG_STR		"trig"
#define AO_CLOCK_STR   	"clock_hz"
#define AO_T0_STR   		"t0"
#define AO_T1_STR   		"t1"
#define AO_MTRIG_STR		"mtrig"
#define AO_MCLOCK_STR   	"mclock_hz"
#define AO_MT0_STR   		"mt0"
#define AO_MT1_STR   		"mt1"
#define  AO_SHOT_NUMBER_STR		"ccs_number"

#define AO_SECTION_CLK_STR     "xclock"
#define AO_SECTION_T0_STR    "xt0"
#define AO_SECTION_T1_STR     "xt1"

#define AO_GAP_R1R2_STR     "gap"




/*
String input record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define STRINGIN_CLTU_TIME	        	   1
#define STRINGIN_CLTU_TIME_STR	   "hhmmss_time"



#endif



