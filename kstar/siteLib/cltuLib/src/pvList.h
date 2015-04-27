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
#define  BO_SET_FREE_RUN		9
#define BO_RUN_CALIBRATION	      10
#define BO_USE_REF_CLK	      11
#define BO_USE_MMAP_CTL	      12
#define BO_IRIGB_SEL	13
#define BO_TLKRX_ENABLE	14
#define BO_TLKTX_SEL	15
#define  BO_SHOW_STATUS			16
#define  BO_SHOW_TIME			17
#define  BO_SHOW_TLK			18
#define BO_LOG_GSHOT			19
#define BO_LOG_PORT			20
#define BO_ENABLE_CLK_PERM		21
#define BO_SET_SHOT_TIME		22






#define BO_SHOT_START_STR		"shot_start"
#define BO_SETUP_STR			"setup"
#define  BO_SHOW_INFO_STR		"show_info"
#define  BO_PORT_ENABLE_STR			"enable"
#define  BO_PORT_ActiveLow_STR			"active_low"
#define  BO_PORT_MODE_STR			"mode"
#define  BO_SHOT_END_STR		"shot_end"
#define  BO_SET_FREE_RUN_STR		"set_free"
#define BO_RUN_CALIBRATION_STR	      "set_cal"
#define BO_USE_REF_CLK_STR	      "use_ref"
#define BO_USE_MMAP_CTL_STR	      "use_mmap"
#define BO_IRIGB_SEL_STR	"irigb_sel"
#define BO_TLKRX_ENABLE_STR	"tlkrx_en"
#define BO_TLKTX_SEL_STR	"tlktx_sel"
#define  BO_SHOW_STATUS_STR		"show_status"
#define  BO_SHOW_TIME_STR		"show_time"
#define  BO_SHOW_TLK_STR		"show_tlk"
#define BO_LOG_GSHOT_STR			"log_gshot"
#define BO_LOG_PORT_STR			"log_port"
#define BO_ENABLE_CLK_PERM_STR		"clk_perm"
#define BO_SET_SHOT_TIME_STR		"shot_time"











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

#define  AI_GET_COMP_NS	   			5
#define  AI_GET_COMP_NS_STR		"comp_ns"



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

#define AO_MCLK_SEL		17
#define AO_READ_BUFFER   18

#define AO_SECTION_T0_BLIP    19
#define AO_SECTION_T1_BLIP     20

#define AO_START_TIME_HH     21
#define AO_START_TIME_MM     22
#define AO_START_TIME_SS     23









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

#define AO_MCLK_SEL_STR	"mclk_sel"
#define AO_READ_BUFFER_STR	"read_buf"

#define AO_SECTION_T0_BLIP_STR    "xt0_blip"
#define AO_SECTION_T1_BLIP_STR     "xt1_blip"

#define AO_START_TIME_HH_STR     "start_hour"
#define AO_START_TIME_MM_STR     "start_minute"
#define AO_START_TIME_SS_STR     "start_second"






/*
String input record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define STRINGIN_CLTU_TIME	        	   1
#define STRINGIN_CLTU_TIME_STR	   "hhmmss_time"



#endif



