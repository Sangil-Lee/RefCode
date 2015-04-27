#ifndef PV_STRING_H
#define PV_STRING_H


/*
BO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define BO_GPIO_OUT      1
#define BO_AO_START      2
#define BO_AI_START      3
#define BO_AI_USE_EXT_CLK      4
#define BO_AO_MODE_REGISTERED      5
#define BO_AO_USE_EXT_CLK      6
#define BO_MAKE_PATTERN      7
#define BO_DIO24_GPIO_PORTA_DIR      8
#define BO_DIO24_GPIO_PORTB_DIR      9
#define BO_DIO24_GPIO_PORTC_DIR      10
#define BO_DIO24_GPIO_PORTC_ALL_DIR      11
#define BO_DIO24_PUT_PORT_A      12
#define BO_DIO24_PUT_PORT_B      13
#define BO_DIO24_PUT_PORT_C      14
#define BO_GPIO_MODE      15
#define BO_16ADIO_LINK_PS      16
#define BO_IP_MIN_FAULT_LINK     17
#define BO_DIRECT_CONTROL     18
#define BO_ALL_RDY_ON      19
#define BO_ALL_RDY_OFF      20








#define BO_GPIO_OUT_STR      "gpio"
#define BO_AO_START_STR   "ao_start"
#define BO_AI_START_STR    "ai_start"
#define BO_AI_USE_EXT_CLK_STR      "ai_ext_clk"
#define BO_AO_MODE_REGISTERED_STR      "ao_mode"
#define BO_AO_USE_EXT_CLK_STR      "ao_ext_clk"
#define BO_MAKE_PATTERN_STR      "pattern"
#define BO_DIO24_GPIO_PORTA_DIR_STR      "dir_pa"
#define BO_DIO24_GPIO_PORTB_DIR_STR      "dir_pb"
#define BO_DIO24_GPIO_PORTC_DIR_STR      "dir_pc"
#define BO_DIO24_GPIO_PORTC_ALL_DIR_STR      "dir_pc_all"
#define BO_DIO24_PUT_PORT_A_STR      "put_a"
#define BO_DIO24_PUT_PORT_B_STR      "put_b"
#define BO_DIO24_PUT_PORT_C_STR      "put_c"
#define BO_GPIO_MODE_STR      "gpio_io"
#define BO_16ADIO_LINK_PS_STR      "link_ps"
#define BO_IP_MIN_FAULT_LINK_STR     "ip_min_fault"
#define BO_DIRECT_CONTROL_STR     "pcs_cntl"
#define BO_ALL_RDY_ON_STR      "rdy_on"
#define BO_ALL_RDY_OFF_STR      "rdy_off"






#define BI_GPIO_IN      1

#define BI_GPIO_IN_STR      "gpio_in"





/*
AI record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
#define  AI_16ADIO_GET_GPIO	   (0x00000001<<0)

#define  AI_READ_STATE	   (0x00000001<<2)
#define  AI_PARSING_CNT		(0x001<<3)
#define  AI_MDS_SND_CNT		(0x001<<4)
#define  AI_DAQING_CNT		(0x001<<5)
#define AI_DIO24_GET_PORT_A      (0x001<<6)
#define AI_DIO24_GET_PORT_B      (0x001<<7)
#define AI_DIO24_GET_PORT_C      (0x001<<8)
#define  AI_16ADIO_CUR_AI_VAL	   (0x00000001<<9)



#define  AI_16ADIO_GET_GPIO_STR			"get_gpio"
#define  AI_READ_STATE_STR			"state"
#define  AI_PARSING_CNT_STR		"pars_cnt"
#define  AI_MDS_SND_CNT_STR			"snd_cnt"
#define  AI_DAQING_CNT_STR		"daqing_cnt"
#define AI_DIO24_GET_PORT_A_STR      "get_pa"
#define AI_DIO24_GET_PORT_B_STR      "get_pb"
#define AI_DIO24_GET_PORT_C_STR      "get_pc"
#define  AI_16ADIO_CUR_AI_VAL_STR	   "cur_ai"






/*
AO record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  AO_SET_READ_OFFSET			101
#define  AO_RFM_WRITE_VALUE			102
#define  AO_NUM_OF_SECTION			103
#define  AO_SECTION_END_VOLT			104
#define  AO_SECTION_END_CUR			105
#define  AO_SECTION_END_TIME			106

#define  AO_AI_PERIOD			107
#define  AO_AO_WRITE_CH			108
#define  AO_FG_CLK_RATE			109
#define  AO_AO_PERIOD			110
#define  AO_CHANNEL_MASK			111
#define  AO_MAX_CURRENT			112
#define  AO_CFG_TAG_VALUE			113
#define  AO_CFG_A_VALUE			114
#define  AO_CFG_B_VALUE			115
#define  AO_CFG_C_VALUE			116
#define  AO_CFG_D_VALUE			117
#define  AO_CMD_CURRENT_LIMIT			118
#define  AO_LIMIT_DURATION			119










#define  AO_SET_READ_OFFSET_STR				"set_offset"
#define  AO_RFM_WRITE_VALUE_STR				"write_value"
#define  AO_NUM_OF_SECTION_STR			"section_cnt"
#define  AO_SECTION_END_VOLT_STR			"sec_end_vlt"
#define  AO_SECTION_END_CUR_STR			"sec_end_cur"
#define  AO_SECTION_END_TIME_STR			"sec_end_time"

#define  AO_AI_PERIOD_STR			"ai_period"
#define  AO_AO_WRITE_CH_STR			"ao_val"
#define  AO_FG_CLK_RATE_STR			"fg_rate_c"
#define  AO_AO_PERIOD_STR			"ao_period"
#define  AO_CHANNEL_MASK_STR			"ch_mask"
#define  AO_MAX_CURRENT_STR			"max_cur"

#define  AO_CFG_TAG_VALUE_STR			"cfg_mds"
#define  AO_CFG_A_VALUE_STR			"cfg_a"
#define  AO_CFG_B_VALUE_STR			"cfg_b"
#define  AO_CFG_C_VALUE_STR			"cfg_c"
#define  AO_CFG_D_VALUE_STR			"cfg_d"
#define  AO_CMD_CURRENT_LIMIT_STR			"cur_limit"
#define  AO_LIMIT_DURATION_STR			"lmt_duration"












/*
Stringout record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  STRINGOUT_READ_OFFSET					1


#define  STRINGOUT_READ_OFFSET_STR				"read_offset"



/*
Stringin record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  STRINGIN_READ_VALUE			1


#define  STRINGIN_READ_VALUE_STR		"read_value"


/*
Waveform record....>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#define  WF_READ_VALUE			1
#define  WF_PTN_B_X			2
#define  WF_PTN_B_Y			3
#define  WF_PTN_M_X			4
#define  WF_PTN_M_Y			5
#define  WF_PTN_T_X			6
#define  WF_PTN_T_Y			7

#define  WF_READ_VALUE_STR		"read_wf"
#define  WF_PTN_B_X_STR		"b_x"
#define  WF_PTN_B_Y_STR		"b_y"
#define  WF_PTN_M_X_STR		"m_x"
#define  WF_PTN_M_Y_STR		"m_y"
#define  WF_PTN_T_X_STR		"t_x"
#define  WF_PTN_T_Y_STR		"t_y"




#endif

