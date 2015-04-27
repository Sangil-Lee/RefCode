#ifndef PV_LIST_H
#define PV_LIST_H



/*
Binary Output record....
*/
#define  BO_SHOT_START		1
#define  BO_SETUP			2
#define  BO_SHOW_INFO			3

	
	
#define BO_SHOT_START_STR		"shot_start"
#define BO_SETUP_STR			"setup"
#define  BO_SHOW_INFO_STR		"show_info"





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


/*
Analog Output record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  AO_SHOT_TERM			1
#define  AO_CLOCK_ONSHOT		2
#define  AO_CALIBRATION		3
#define  AO_PUT_CALED_VAL	4

#define  AO_TRIG			5
#define  AO_CLOCK		6
#define  AO_T0			7
#define  AO_T1			8
#define  AO_MTRIG		9
#define  AO_MCLOCK		10
#define  AO_MT0			11
#define  AO_MT1			12



#define AO_SHOT_TERM_STR   	"shot_term"
#define AO_CLOCK_ONSHOT_STR	"clkOnShot"
#define AO_CALIBRATION_STR	      "set_cal"
#define AO_PUT_CALED_VAL_STR		"put_cal"

#define AO_TRIG_STR		"trig"
#define AO_CLOCK_STR   	"clock_hz"
#define AO_T0_STR   		"t0"
#define AO_T1_STR   		"t1"
#define AO_MTRIG_STR		"mtrig"
#define AO_MCLOCK_STR   	"mclock_hz"
#define AO_MT0_STR   		"mt0"
#define AO_MT1_STR   		"mt1"


/*
String input record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define STRINGIN_CLTU_TIME	        	   1
#define STRINGIN_CLTU_TIME_STR	   "hhmmss_time"



#endif



