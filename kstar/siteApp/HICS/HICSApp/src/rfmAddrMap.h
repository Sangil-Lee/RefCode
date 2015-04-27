#ifndef _RFM_ADDR_MAP_H_
#define _RFM_ADDR_MAP_H_


#define INIT_RFM_MAP_OFFSET	0
#define INIT_RFM_MAP_PAGE	1
#define INIT_RFM_MAP_BYTES		0x8000000  /* 128MB */

#define	USE_RFM_MMAP  1



#define RFM_BASE_ADDR         0x95fe0000

#define MAX_USER_OFFSET_NUM	30


#define RFM_PCS_TO_PF    0x100
#define RFM_PCS_TO_IVC   0x190

#define RFM_PF1    0x300
#define RFM_PF2    0x380
#define RFM_PF3U   0x400
#define RFM_PF3L   0x440
#define RFM_PF4U   0x480
#define RFM_PF4L   0x4C0
#define RFM_PF5U   0x500
#define RFM_PF5L   0x540
#define RFM_PF6U   0x580
#define RFM_PF6L   0x5C0
#define RFM_PF7    0x600
#define RFM_TF     0x680

#define RFM_IVC   0x6A0  /* 0x95fe06A0  */


#define RFM_CCS_SHARE   0x3800000  /* 0x997e0000, start at 56MB */
#define RFM_PCS_RTSCOPE   0x4000000  /* 0x99fe0000, start at 64MB */


#define RM_PCS_CNT    0x10c
#define RM_PF1_CNT    0x30c
#define RM_PF2_CNT    0x38c
#define RM_PF3U_CNT    0x40c
#define RM_PF3L_CNT    0x44c
#define RM_PF4U_CNT    0x48c
#define RM_PF4L_CNT    0x4cc
#define RM_PF5U_CNT    0x50c
#define RM_PF5L_CNT    0x54c
#define RM_PF6U_CNT    0x58c
#define RM_PF6L_CNT    0x5cc
#define RM_PF7_CNT    0x60c
#define RM_IVC_CNT    0x6b0

#define RM_RMP_TOP_MODE	0x6ec
#define RM_RMP_MID_MODE	0x6fc
#define RM_RMP_BTM_MODE	0x70c

#define RM_RMP_TOP_I	0x6e8
#define RM_RMP_MID_I	0x6f8
#define RM_RMP_BTM_I	0x708

#define RM_RMP_TOP_V    0x6e4
#define RM_RMP_MID_V    0x6f4
#define RM_RMP_BTM_V    0x704

#define RM_PCS_FAULT    0x3800008
#define RM_PCS_FAST_INTLOCK    0x3800010







typedef struct
{
	volatile int    id;
	volatile int    control_method;
	volatile int    current_direction;
	volatile unsigned int    comm_counter;
	volatile int    rt_mode;
	volatile int    pf_command[14]; /*  11 -> 14   2010.6.14 woong */
	volatile int    testvar;
	volatile int    pf_current_traj[14]; /*  new   2010.6.14 woong */
} PCS_cmd_PF;

typedef struct
{
	volatile int    id;
	volatile int    control_method;
	volatile int    current_type;
	volatile unsigned int    comm_IVC;
	volatile int    current_target_IVC;
	volatile int    command_IRC;
	volatile int    current_target_IRC;
} PCS_cmd_IVC;


#define PCS_S_V_NCPUS      10
#define PCS_S_V_RINGSIZE   1000
#define PCS_S_V_NSIGNALS   50
#define PCS_S_SIGNAMLEN    16

typedef struct 
{
	volatile int     id;
	volatile int     status;
	volatile int     cput_status[PCS_S_V_NCPUS];
	volatile int     shotnumber;
	volatile int     scope_buffer_index;
	volatile int     last_update_time;
	volatile int     update_frequency;
	volatile char    scope_sig_names[PCS_S_V_NSIGNALS][PCS_S_SIGNAMLEN];
	volatile float   scope_y_spans[PCS_S_V_NSIGNALS][2];
	volatile int     scope_buffer_time[PCS_S_V_RINGSIZE];
	volatile float   scope_buffer_data[PCS_S_V_RINGSIZE][PCS_S_V_NSIGNALS];
} pcs_rtscope;



typedef struct
{
	volatile int         id;
	volatile unsigned    pcs_current_time;
	volatile int         pcs_fault_code;
	volatile int         force_pcs_abort;
	volatile int         force_fast_interlock; /* 2012. 8. 1 request by shhan */
} CCS_share;

typedef struct
{
	volatile int       id;
	volatile int       vpf_measured;   /* scale 1000:32767 */
	volatile int       ipf_measured;   /* scale 1000:32767 */
	volatile unsigned int   comm_counter;
	volatile int       ipf_delta_measured;  /* new 2010.6.23 */
	volatile int       ipf_wye_measured;  /* new 2010.6.23 */
	volatile int       ipf_limit_flag;
	volatile int       alpha_delta_measured;  /* scale 1/100 */  /* name changed (alpha_measured) */
	volatile int       alpha_wye_measured;       /* new 2010.6.23 */
	volatile int       vqp_measured;   /* scale 1:1 */
	volatile int       vbris_measured; /* scale 1:1 */
	volatile int       vpf_delta_measured;   /* new 2010.6.23 */
	volatile int       vpf_wye_measured;    /* new 2010.6.23 */
} PF_command;

typedef struct
{
	volatile int       id;
	volatile int       itf_measured;          /* scale 1:1 */
	volatile int       vtf_measured;          /* scale 1:100 */
	volatile int       vbris_measured;        /* scale 1:100 */  /* slow_discharge_volt */
	volatile int       vqp_measured;          /* scale 1:1 */  /* fast_discharge_volt */
} TF_command;


typedef struct
{
	volatile int       id;
	volatile int       voltage_measured; 
	volatile int       current_measured;
	volatile int       vdclink_measured;
	volatile unsigned int  comm_counter;    
	volatile int       ivc_limit_flag;
} IVC_command;




















#endif

