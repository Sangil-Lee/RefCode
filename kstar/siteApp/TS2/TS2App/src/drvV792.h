#ifndef _DRV_V792_H
#define _DRV_V792_H


#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for open() */

#include "sfwDriver.h"


#include "CAENVMETool/cvt_board_commons.h"
#include "CAENVMETool/cvt_common_defs.h"
#include "CAENVMETool/cvt_V792.h"


#define STR_DEVICE_TYPE_1   "V792"
#define STR_DEVICE_TYPE_2   "V792N"
#define STR_DEVICE_TYPE_3   "NotDefined"





#define PRINT_DAQ_DEBUG  0
#define PRINT_RING_BUF_DEBUG  1

#define STR_CHANNEL_RAW_DIR   "/home/rawTemp"
#define STR_CHANNEL_DATA_DIR   "/home/qtpData"
#define STR_Te_DATA_DIR   "/home/Te"
#define STR_K2_DATA_DIR   "/home/K2"



#define CH_CNT_MAX			16


#define OPCODE_NORMAL_RUN		1
#define OPCODE_NORMAL_CLOSE		2
/*
#define OPCODE_CALIB_GET_MEAN		2
#define OPCODE_CALIB_GET_MEAN_END	3 
*/


#define MAX_THRESHOLD_NUM  32					/*!< \brief Maximun Number of threshold registers */

#define CALIB_ITER_CNT      20
#define CALIB_PEDESTAL_INCRSE_CNT      5

#define CNT_QUEUE_SND_CUTOFF  3

#define TIME_DAQ_THREAD_DELAY   (0.05)
/* 20Hz = 0.050,  0.2/0.05 = 8 -->  TIME_DAQ_THREAD_DELAY/0.05 */
/* 100Hz = 0.010,  0.05/0.01 = 5 -->  TIME_DAQ_THREAD_DELAY/0.01 */

#define CNT_QTP_READ_BUFF_EXTRA   40

#define INIT_IPAD_VALUE    180


#define STATE_ACTUAL_PULSE   (0x01<<0)

#define CNT_LOOKUP_TABLE     300


#define MAX_PLYC_PER_BOARD  6

#define USE_REAL_TIME_CAL   0

#define TEMP_HEADER_DIFF_VAL   100

#define USE_TSSC_FPGA    1





#define INIT_GAIN			1
#define INIT_CH_MASK		96


typedef struct {
    UINT16 m_thresholds_buff[ MAX_THRESHOLD_NUM];				/*!< The zero suppression threshold buffer : for dual range boards (V965, V1785) these are the low range thresholds */
    UINT16 m_high_thresholds_buff[ MAX_THRESHOLD_NUM];			/*!< The high range zero suppression threshold buffer (just for dual range boards (V965, V1785)) */
    BOOL m_step_threshold;										/*!< The zero suppression threshold resolution */
} ST_zero_suppression;


typedef struct {
    BOOL m_sliding_scale_enable;						/*!< Enable/disable sliding scale. */
    BOOL m_zero_suppression_enable;						/*!< Enable/disable zero suppression. */
    BOOL m_overflow_suppression_enable;					/*!< Enable/disable overflow suppression. */
    BOOL m_empty_enable;								/*!< Enable/disable empty events. */
    BOOL m_count_all_events;							/*!< Enable/disable counting all triggers or accepted one only. */
} ST_ACQ_MODE;

typedef struct {
	char isActual; /* 1: actual,      0: backgnd */
	
	UINT16 raw_cur_2;
	UINT16 raw_cur_1;
	UINT16 raw_cur;
	
} ST_DATA_SET;

typedef struct {
	ELLNODE  node;
	ST_DATA_SET STbuf[CH_CNT_MAX][CNT_QTP_READ_BUFF_EXTRA];
} ST_DATA_node;


/******* make for RING buffer   ************************************/
typedef struct {
	ELLNODE			node;
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	UINT16 qtp_rawData[CH_CNT_MAX][CNT_QTP_READ_BUFF_EXTRA];
} ST_buf_node;

typedef struct {
/*	ST_buf_node	*pNode; */
	ST_DATA_node	*pNode;
	unsigned int		cnt_block_read;
	UINT32 accum_cnt;
	char				opcode;
	char    cReset_IncreaseIped;
	UINT16 Iped_num;
	
/*	UINT32 acuum_value[CH_CNT_MAX];
	float mean_value[CH_CNT_MAX];
*/
} ST_User_Buf_node;
/****************************************************/

/******* make for Calc RING buffer   ************************************/
typedef struct {
	ST_DATA_node	*pNode;
	unsigned int		cnt_block_read;
	char				opcode;

} ST_Calc_Buff_node;
/****************************************************/



typedef struct {
	char strTagName[40];

/*	UINT32 state; */


	UINT32 acuum_value;
	float mean_value;
	UINT32 acuum_value_bg;
	float mean_value_bg;

	UINT16 rawVal_current;
	UINT16 rawVal_max;
	UINT16 rawVal_min;
	
/*	UINT16 rawVal_prev[2]; */

	float Iped_ref[256];

/*	UINT16 tmp_rawData[CNT_QTP_READ_BUFF_EXTRA]; */
	UINT16 tmp_prev_2;
	UINT16 tmp_prev_1;
	ST_DATA_SET ST_Calc[CNT_QTP_READ_BUFF_EXTRA];

	
}
ST_V792_channel;

typedef struct {

	cvt_V792_data board_data;
	UINT16 m_base_address;	  /*!< The board base address */
	CVT_QTP_TYPES m_qtp_type; /*CVT_V792_TYPE_A, CVT_V792_TYPE_N */


	UINT16 firmware_rev;
	UINT8  piggy_back_type;
	UINT16 serial_number;
	UINT16 chNum;
	UINT8 PolycMaxNum;


	ST_ACQ_MODE m_ST_AcqModeParam;
	ST_zero_suppression  m_ST_zeroParam;

	ST_threadCfg	*pST_BuffThread;
	ELLLIST 	*pList_BufferNode;


/*	UINT32 data_buff[34]; */
/*	UINT32 data_size; */

	FILE *fpRaw[CH_CNT_MAX];
//	FILE *fpTe[MAX_PLYC_PER_BOARD];
//	FILE *fpK2[MAX_PLYC_PER_BOARD];
//	UINT32 cntTe;



	UINT16  Pedestal;


	char cmd_DAQ_stop;	
	UINT16 cnt_block_read; /* 한번 읽은 블럭 수 */
	char toggle_get_MeanVal;
	char use_file_save;
	char toggle_rt_calc;
	
	UINT32 accum_cnt;
	UINT16 rawVal_current[CH_CNT_MAX];
	UINT16 raw_curr[CH_CNT_MAX][CNT_QTP_READ_BUFF_EXTRA];
	char strTagName[CH_CNT_MAX][SIZE_TAG_NAME];


	UINT16 calib_iter_cnt; /* init vaule: 100 */
	UINT16 calib_increase_cnt; /* init vaule: 1 */

	UINT16 my_shot_period;

	UINT32 time_LTU_DG535_us;
/*	UINT32 time_LTU_LASER_us; */
	UINT32 count_DG535_IN_PULSE;
	UINT32 time_Bg_delay_us;

	UINT32 mask_channel;


	/*******************************/
	/* for real time calculation ***/	
#if 0
	UINT8 plycW1ChMap[MAX_PLYC_PER_BOARD];
	UINT8 plycW2ChMap[MAX_PLYC_PER_BOARD];
	UINT8 plycW3ChMap[MAX_PLYC_PER_BOARD];
	UINT8 plycW4ChMap[MAX_PLYC_PER_BOARD];
	UINT8 plycW5ChMap[MAX_PLYC_PER_BOARD];

	float64 SP1[MAX_PLYC_PER_BOARD][CNT_LOOKUP_TABLE];
	float64 SP2[MAX_PLYC_PER_BOARD][CNT_LOOKUP_TABLE];
	float64 SP3[MAX_PLYC_PER_BOARD][CNT_LOOKUP_TABLE];
	float64 SP4[MAX_PLYC_PER_BOARD][CNT_LOOKUP_TABLE];
	float64 SP5[MAX_PLYC_PER_BOARD][CNT_LOOKUP_TABLE];
	
	float32 f32AvrBg[CH_CNT_MAX];
	INT32 n32ScatterData[CH_CNT_MAX];
	float64 f64WeightFactor[CH_CNT_MAX];

	float64 f64K1[MAX_PLYC_PER_BOARD], f64K2, f64K3;
	float64 f64Step2[CNT_LOOKUP_TABLE];
	float64 f64Te[MAX_PLYC_PER_BOARD];

	float64 f64Step1[CH_CNT_MAX][CNT_LOOKUP_TABLE];
	float64 f64Step3[MAX_PLYC_PER_BOARD][CNT_LOOKUP_TABLE];
#endif	

} 
ST_V792;


int CAENVME_handle_init();
void CAENVME_handle_release();

int init_my_SFW_v792( ST_STD_device *pSTDdev);
int create_v792_taskConfig( ST_STD_device *pSTDdev);
ST_threadCfg*  make_controlThreadConfig(ST_V792 *pV792);
void clear_V792_Locally(ST_STD_device*); 


void func_V792_DATA_SEND(void *pArg, double arg1, double arg2);
void func_V792_SYS_ARMING(void *pArg, double arg1, double arg2);
void func_V792_SYS_RUN(void *pArg, double arg1, double arg2);
void func_V792_OP_MODE(void *pArg, double arg1, double arg2);
void func_V792_TEST_PUT(void *pArg, double arg1, double arg2);







int drvV792_open_card(ST_STD_device *);
int drvV792_close_card(ST_STD_device *);
int drvV792_show_card_info(ST_STD_device *);

int drvV792_set_defaultValue(ST_STD_device *);

int drvV792_set_testFunc(ST_STD_device *);


int drvV792_read_chanData_Loop(ST_STD_device *);
int drvV792_read_chanData_clear(ST_STD_device *);
int drvV792_read_FullData(ST_STD_device *);
int drvV792_check_DataReady(ST_STD_device *);
/*int drvV792_read_chanData_Loop(ST_STD_device *); */

int drvV792_ARM_enable(ST_STD_device *);
int drvV792_ARM_disable(ST_STD_device *);
int drvV792_RUN_start(ST_STD_device *);
int drvV792_RUN_stop(ST_STD_device *);


int drvV792_reset_calibration_param(ST_STD_device *);
//int drvV792_set_TagName(ST_STD_device *);

int drvV792_assign_after_shot_info();
int drvV792_get_info_befor_shot();
void drvV792_print_Jitter();
int drvV792_get_header_dreg();






void threadFunc_V792_DAQ(void *param);
//void threadFunc_V792_RingBuf(void *param);
void threadFunc_V792_CatchEnd(void *param);


int drvV792_openRawFile(ST_STD_device *pSTDdev);
int drvV792_openTeFile(ST_STD_device *pSTDdev);


int drvV792_make_calcBuff(ST_STD_device *pSTDdev);
/*void drvV792_init_SP_Table();*/

int drvV792_init_Lookup_table(ST_STD_device *pSTDdev);
int drvV792_init_Polyc_Ch_mapping(ST_STD_device *pSTDdev);






#endif /* _DRV_V792_H */

