#ifndef drv16aiss8ao4_H
#define drv16aiss8ao4_H

#include <ctype.h>
#include <time.h>
#include <asm/types.h>
#include <sys/ioctl.h>

#include "16aiss8ao4.h"
#include "16aiss8ao4_dsl.h"
#include "util_close.h"
#include "util_count.h"
#include "util_id.h"
#include "util_label.h"
#include "util_open.h"
#include "util_select.h"

#include "util_auto_cal.h"
#include "util_init.h"
#include "util_input_config.h"
#include "util_input_buffer.h"
#include "util_io.h"
#include "util_output_buffer.h"
#include "util_output_config.h"
#include "util_rate_gen.h"

#include "util_reg.h"


#include "sfwDriver.h"
#include "dbAccess.h"   

#include "rfmAddrMap.h"


#define STR_DEVICE_TYPE_1   "16aiss8ao4"

#define MAX_NUM_SECTION   32
#define CNT_AI_CH    8
#define CNT_AO_CH    4

#define READ_TIME_OUT   300

#define INIT_TX_CLOCK   1000		/* 1KHz */
#define INIT_RX_CLOCK   10000	/* 10KHz */


#define QUEUE_OPCODE_NORMAL        0x0
#define QUEUE_OPCODE_CLOSE_FILE   (0x1<<0)
#define QUEUE_OPCODE_DAQ_STOP     (0x1<<1)


#define STR_CHANNEL_DATA_DIR   "/data/rawData"

#define ID_16ADIO_CH0   0
#define ID_16ADIO_CH1   1
#define ID_16ADIO_CH2   2
#define ID_16ADIO_CH3   3
#define ID_16ADIO_CH4   4
#define ID_16ADIO_CH5   5
#define ID_16ADIO_CH6   6
#define ID_16ADIO_CH7   7

#define CH_ID_BTM_I_AI   0
#define CH_ID_BTM_V_AI   1
#define CH_ID_MID_I_AI   2
#define CH_ID_MID_V_AI   3
#define CH_ID_TOP_I_AI   4
#define CH_ID_TOP_V_AI   5



#define TAG_NAME_CH0    "\\RMPB_I:FOO"
#define TAG_NAME_CH1    "\\RMPB_V:FOO"
#define TAG_NAME_CH2    "\\RMPM_I:FOO"
#define TAG_NAME_CH3    "\\RMPM_V:FOO"
#define TAG_NAME_CH4    "\\RMPT_I:FOO"
#define TAG_NAME_CH5    "\\RMPT_V:FOO"
#define TAG_NAME_CH6    "\\NONE"
#define TAG_NAME_CH7    "\\NONE"

#define TAG_CMD_CH0    "\\RMPB_IC:FOO"
#define TAG_CMD_CH1    "\\RMPM_IC:FOO"
#define TAG_CMD_CH2    "\\RMPT_IC:FOO"
#define TAG_CMD_CH3    "\\NONE"

#define USE_LAST_CH4_AO    0


#define TIME_FIX_LTU_START    (14.0)

#define MAX_WAVEFORM_NODE_CNT 100

#define INIT_MAX_CURRENT	6000  // Amphere

#define ID_PS_BOTTOM	0
#define ID_PS_MIDDLE	1
#define ID_PS_TOP		2

/* move to rfmAddrMap.h   
#define RM_OFSET_TOP_MODE	0x6ec
#define RM_OFSET_MID_MODE	0x6fc
#define RM_OFSET_BTM_MODE	0x70c

#define RM_OFSET_TOP_I	0x6e8
#define RM_OFSET_MID_I	0x6f8
#define RM_OFSET_BTM_I	0x708

#define RM_OFSET_TOP_V	0x6e4
#define RM_OFSET_MID_V	0x6f4
#define RM_OFSET_BTM_V	0x704
*/

/* 1: write single raw value to RFM,  0: write 10 average value */
#define USE_SINGLE_RAW_VALUE_RFM  0  




/******* make for RING buffer   ************************************/
#define MAX_RING_BUF_NUM            12
#define MAX_CUTOFF_BUF_NODE         4
/*#define BUFLEN	(800)  */ /* @@todo read from device */
#define BUFLEN	(80)   /* 2012. 2. 6 */	/* if clock is 10KHz, then 1KHz event  generated */


typedef struct {
	ELLNODE			node;
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	__u32 data[ BUFLEN ];  
} ST_buf_node;

typedef struct {
	ST_buf_node	*pNode;
	unsigned int		u32Cnt;
	unsigned int		size;
	char				opcode;

} ST_User_Buf_node;
/****************************************************/

typedef struct {
	double y2; /* voltage amplitude */
	double x2;  /* time unit : second */
	double a;
	double delta_v;
	int16 delta_hex; /* need plus 0x8000 */

} ST_PATTERN;


typedef struct {

	int num_section;
	ST_PATTERN  CH0[MAX_NUM_SECTION]; /* Bottom */
	ST_PATTERN  CH1[MAX_NUM_SECTION]; /* Middle */
	ST_PATTERN  CH2[MAX_NUM_SECTION]; /* Top */
	ST_PATTERN  CH3[MAX_NUM_SECTION];
	
		
	int		fd;


	uint16 u16GPIO;
	int GPIO_0_IO;
	int GPIO_1_IO;

	double currAI[CNT_AI_CH];

	uint8 u8_Link_PS;
	uint8 u8_ip_fault;
	uint8 u8_PCS_control; /* default: 0 */

	epicsEventId	epicsEvent_FGthread;

	uint32 out_ch_mask;		/*AO_CH_ALL_ENABLE*/
	uint32 out_io_mode; 		/* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
	uint32 out_mode; 	/* OUT_MODE_REGISTER, OUT_MODE_BUFFERED */
	uint32 out_simultaneous; /* TRUE, FALSE */
	uint32 out_clock_initiator_mode; /*OUT_CLK_INITIATOR_RATE_C, OUT_CLK_INITIATOR_HARDWARE*/
	uint32 out_range;			/*OUTPUT_RANGE_10, OUTPUT_RANGE_2_5, OUTPUT_RANGE_5*/	
	uint32 out_burst_enable;	/* FALSE */
	uint32 out_clock_c;	/* 40MHz */
	uint32 out_rate_c;		/* 1 K */
	uint32 out_rate_gen_c_enable;	/*OUT_CLK_DISABLE, OUT_CLK_ENABLE */
	uint32 out_clock_enable;		/*OUT_CLK_DISABLE, OUT_CLK_ENABLE */
	uint32 out_buffer_threshold;
	uint32 out_circular_buffer_enable; /* FALSE */
	uint32 out_period;	/* 5 */ /* not used replaced to STD run_period */



	uint32 in_ch_mask;		/* ENABLE_INPUT_0, ENABLE_INPUT_1, xxx, ENABLE_INPUT_ALL */
	uint32 in_io_mode; 		/* DMA_DISABLE, DMA_ENABLE, DMA_DEMAND_MODE */
	uint32 in_clock_initiator_mode;	/*default:CLK_INITIATOR_OUTPUT, CLK_INITIATOR_INPUT*/
	uint32 in_mode; 	/* AIM_DIFFERENTIAL, AIM_SINGLE_ENDED, xxx*/
	uint32 in_range_a;	/*INPUT_RANGE_10, INPUT_RANGE_2_5, INPUT_RANGE_5*/	/* 0,1,4,5 */
	uint32 in_range_b;	/*INPUT_RANGE_10, INPUT_RANGE_2_5, INPUT_RANGE_5*/	/* 2,3,6,7 */
	uint32 in_burst_enable;	/* FALSE */
	uint32 in_clock_a;	/* 40MHz */
	uint32 in_rate_a;		/* 10K */
	uint32 in_rate_gen_a_enable;	/* TRUE */
	uint32 in_buffer_threshold;
	uint32 in_buffer_enable; /* TRUE */
	uint32 in_period;	/* 5*/ /* not used replaced to STD run_period */

	
	uint32 input_format;	/* DATA_FORMAT_OFFSET_BINARY */


	FILE *fpRaw[CNT_AI_CH];


	char strTagName[CNT_AI_CH][SIZE_TAG_NAME];
	char strTagCmd[CNT_AO_CH][SIZE_TAG_NAME];

	double d32CurValue[CNT_AI_CH];

	float f32MaxCurrent;
	unsigned int u32CmdCurrentLimit;
	double d32CurrentLimitDuration;
	unsigned int u32Mode_tag[3];
	unsigned int u32Mode_PCS[3];

	unsigned int u32Mode_a[3];
	unsigned int u32Mode_b[3];
	unsigned int u32Mode_c[3];
	unsigned int u32Mode_d[3];
	

} 
ST_16aiss8ao4;



int init_my_SFW_16aiss8ao4( ST_STD_device *pSTDdev);
int init_my_MDS_tag( ST_STD_device *pSTDdev);



int create_16aiss8ao4_taskConfig( ST_STD_device *pSTDdev);
void clear_16aiss8ao4_taskConfig(void *pArg, double arg1, double arg2);



/*ST_threadCfg*  make_controlThreadConfig(ST_16aiss8ao4 *p16aiss8ao4); */
int make_16aiss8ao4_FG_thread(ST_STD_device *pSTDdev);




void threadFunc_16aiss8ao4_DAQ(void *param);
void threadFunc_16aiss8ao4_FG(void *param);
void threadFunc_16aiss8ao4_RingBuf(void *param);
void threadFunc_16aiss8ao4_CatchEnd(void *param);


void func_16aiss8ao4_OP_MODE(void *, double, double);
void func_16aiss8ao4_BOARD_SETUP(void *, double, double);
void func_16aiss8ao4_SYS_ARMING(void *, double, double);
void func_16aiss8ao4_SYS_RUN(void *, double , double );
void func_16aiss8ao4_SAMPLING_RATE(void *, double , double );
void func_16aiss8ao4_DATA_SEND(void *, double , double );





int drv16aiss8ao4_dio_out(ST_STD_device *pSTDdev, int ch, uint32 value);
int drv16aiss8ao4_AO_trigger(ST_STD_device *pSTDdev);
int drv16aiss8ao4_AI_trigger(ST_STD_device *pSTDdev);
/*int drv16aiss8ao4_make_pattern(ST_STD_device *pSTDdev); */
int drv16aiss8ao4_make_pattern_2(ST_STD_device *pSTDdev);
int drv16aiss8ao4_find_section(ST_16aiss8ao4 *, int ch, int step);
int drv16aiss8ao4_set_AI_clock_mode(ST_STD_device *pSTDdev, uint8 value);
int drv16aiss8ao4_set_AO_mode_registered(ST_STD_device *pSTDdev, uint8 value);
int drv16aiss8ao4_set_AO_clock_mode(ST_STD_device *pSTDdev, uint8 value);




int drv16aiss8ao4_set_section_end_time(ST_STD_device *pSTDdev, int ch, int sect, float time);
int drv16aiss8ao4_set_section_end_voltage(ST_STD_device *pSTDdev, int ch, int sect, float vltg);
int drv16aiss8ao4_set_section_end_current(ST_STD_device *pSTDdev, int ch, int sect, float cur);


int drv16aiss8ao4_set_GPIO_OUT(ST_STD_device *pSTDdev, int which, int inout);


int gsc_pre_calc_pattern(ST_16aiss8ao4 *p16aiss8ao4);
int gsc_reset_pattern(ST_16aiss8ao4 *p16aiss8ao4);


int gsc_dio_0_out(int fd, char data);
int gsc_dio_1_out(int fd, char data);

int gsc_dio_0_in(int fd, uint32 *data);
int gsc_dio_1_in(int fd, uint32 *data);


/*void _init_data(uint32 mask, long rate, int seconds);*/
void _init_data_2(uint32 mask, long rate, int seconds);
int _io_write(ST_STD_device *pSTDdev);
int _io_read(ST_STD_device *pSTDdev);

int drv16aiss8ao4_copy_data(int ch, int cnt,  unsigned short *data);

int drv16aiss8ao4_copy_to_waveform(ST_STD_device *pSTDdev, int ch, void *vptr);
int drv16aiss8ao4_copy_to_wf_pattern_x(ST_STD_device *pSTDdev, int psID, void *vptr);
int drv16aiss8ao4_copy_to_wf_pattern_y(ST_STD_device *pSTDdev, int psID, void *vptr);

int drv16aiss8ao4_update_coilMode(ST_STD_device *pSTDdev);

int drv16aiss8ao4_rfm_reset(void *);
int drv16aiss8ao4_rfm_write_coil_mode(void *, void *);

void drv16aiss8ao4_dio_call_stop();







#endif /* drv16aiss8ao4_H */

